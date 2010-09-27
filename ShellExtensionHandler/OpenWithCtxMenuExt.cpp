// OpenWithCtxMenuExt.cpp : Implementation of COpenWithCtxMenuExt

#include "stdafx.h"
#include <io.h>
#include <stdio.h>
#include <direct.h>
#include "OpenWithExt.h"
#include "OpenWithCtxMenuExt.h"
#include "DynList.h"
#include "MemoryBuffer.h"

#pragma comment(lib,"shlwapi")

/////////////////////////////////////////////////////////////////////////////
// COpenWithCtxMenuExt

HRESULT COpenWithCtxMenuExt::Initialize (LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{	
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP     hDrop;

    // Look for CF_HDROP data in the data object.
    if ( FAILED( pDataObj->GetData ( &fmt, &stg ))) {
        // Nope! Return an "invalid argument" error back to Explorer.
        return E_INVALIDARG;
    }

    // Get a pointer to the actual data.
    hDrop = (HDROP) GlobalLock ( stg.hGlobal );

    // Make sure it worked.
	if ( NULL == hDrop ) {
	    return E_INVALIDARG;
	}
    

    // Sanity check - make sure there is at least one filename.
	UINT uNumFiles = DragQueryFile ( hDrop, 0xFFFFFFFF, NULL, 0 );

    if ( 0 == uNumFiles ) {
        GlobalUnlock ( stg.hGlobal );
        ReleaseStgMedium ( &stg );
        return E_INVALIDARG;
    }

	HRESULT hr = S_OK;

	ZeroMemory (m_szShellInfoPath, SIZE_STRING);
	
	m_dlFileList.Clear ();

	for (int d=0;d<uNumFiles;d++) {
		ZeroMemory (m_szSelectedFile, MAX_PATH);
		if (DragQueryFile (hDrop, d, m_szSelectedFile, MAX_PATH) != 0) {
			//MessageBox (NULL, m_szSelectedFile, "File Selected.", MB_OK);
			m_dlFileList.AddItem (m_szSelectedFile, strlen (m_szSelectedFile), false);
		}
	}

    // Get the name of the first file and store it in our member variable m_szFile.
	//if ( 0 == DragQueryFile ( hDrop, 0, m_szSelectedFile, MAX_PATH )) {
		//hr = E_INVALIDARG;
	//} else {
        // Quote the name if it contains spaces (needed so the cmd line is built properly)
    //    PathQuoteSpaces ( m_szSelectedFile );
    //}

    GlobalUnlock ( stg.hGlobal );
    ReleaseStgMedium ( &stg );

    return hr;
}

HRESULT COpenWithCtxMenuExt::QueryContextMenu (HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
    // If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if (uFlags & CMF_DEFAULTONLY) {
		return MAKE_HRESULT (SEVERITY_SUCCESS, FACILITY_NULL, 0);
	}

	HBITMAP hbmIcon = LoadBitmap (_Module.GetModuleInstance (), MAKEINTRESOURCE (IDB_PADLOCKICON));

	//MessageBox (NULL, "Test", "Test", MB_OK);

    // First, create and populate a submenu.
	HMENU hSubmenu = CreatePopupMenu();
	UINT uID = uidFirstCmd;

    InsertMenu ( hSubmenu, 0, MF_BYPOSITION, uID++, _T("&Encrypt") );
    InsertMenu ( hSubmenu, 1, MF_BYPOSITION, uID++, _T("&Decrypt") );

    // Insert the submenu into the ctx menu provided by Explorer.
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };

    mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID | MIIM_CHECKMARKS;
    mii.wID = uID++;
    mii.hSubMenu = hSubmenu;
    mii.dwTypeData = _T("C&yberCede");
	mii.hbmpChecked = hbmIcon;
	mii.hbmpUnchecked = hbmIcon;

    InsertMenuItem ( hmenu, uMenuIndex, TRUE, &mii );


    //return MAKE_HRESULT (SEVERITY_SUCCESS, FACILITY_NULL, uID - uidFirstCmd + 1);
	return MAKE_HRESULT (SEVERITY_SUCCESS, 0, uID - uidFirstCmd + 1);
}

HRESULT COpenWithCtxMenuExt::GetCommandString (UINT idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
	USES_CONVERSION;

    // Check idCmd, it must be 0 or 1 since we have two menu items.
	if (idCmd > 1) {
		return E_INVALIDARG;
	}

    // If Explorer is asking for a help string, copy our string into the
    // supplied buffer.
    if (uFlags & GCS_HELPTEXT) {
        LPCTSTR szEncrypt = _T("Encrypt the selected file(s) using powerful CyberCede encryption.");
        LPCTSTR szDecrypt = _T("Decrypt the selected file(s) previously encrypted with CyberCede. Your password will be required.");
        LPCTSTR pszText = (0 == idCmd) ? szEncrypt : szDecrypt;

        if ( uFlags & GCS_UNICODE ) {
            // We need to cast pszName to a Unicode string, and then use the
            // Unicode string copy API.
            lstrcpynW ( (LPWSTR) pszName, T2CW(pszText), cchMax );
        } else {
            // Use the ANSI string copy API to return the help string.
            lstrcpynA ( pszName, T2CA(pszText), cchMax );
        }

        return S_OK;
	}

    return E_INVALIDARG;
}

void COpenWithCtxMenuExt::PrepareCedeCryptPath ()
{
	// This function will read the standard registry location for
	// to obtain the location of the cedecrypt executable
	// for calling. The registry location is set when cedecrypt
	// is initially called with cedecrypt.exe /i . The /i switch
	// is called by the cedecryptsetup application.
	m_bCedeCryptPathfound = false;
	ZeroMemory (m_szCedeCryptPath, SIZE_STRING);


	//m_registry.WriteString ("Software\\CedeSoft\\CedeCrypt", "ShellExtensionPath", szModulepath);
	if (m_registry.DoesValueExist ("Software\\CedeSoft\\CedeCrypt", "ShellExtensionPath", REG_SZ) == true) {
		m_bCedeCryptPathfound = true;
		strcpy_s (m_szCedeCryptPath, SIZE_STRING, m_registry.ReadString ("Software\\CedeSoft\\CedeCrypt", "ShellExtensionPath"));
	} else {
		m_bCedeCryptPathfound = false;
	}

	
}

bool COpenWithCtxMenuExt::PrepareShellInfoPath ()
{
	// This function retrieves and sets up the directory necessary
	// under application data for temporarily storing the list of files
	// that have been selected for encryption.
	int iRes = 0;
	char szAppData[SIZE_STRING];
	char szCompanyAppData[SIZE_STRING];
	char szProgramAppData[SIZE_STRING];
	char szInfoAppData[SIZE_STRING];

	ZeroMemory (szAppData, SIZE_STRING);
	ZeroMemory (szCompanyAppData, SIZE_STRING);
	ZeroMemory (szProgramAppData, SIZE_STRING);
	ZeroMemory (szInfoAppData, SIZE_STRING);

	if (GetEnvironmentVariable ("APPDATA", szAppData, SIZE_STRING) == 0) {
		// retrieving environment variable failed.
		//m_diag.OutputText ("Failed to retrieve APPDATA environment variable!");		
		return false;
	} 

	strcpy_s (szCompanyAppData, SIZE_STRING, szAppData);
	strcat_s (szCompanyAppData, SIZE_STRING, "\\CedeSoft");

	strcpy_s (szProgramAppData, SIZE_STRING, szCompanyAppData);
	strcat_s (szProgramAppData, SIZE_STRING, "\\CedeCrypt");

	strcpy_s (szInfoAppData, SIZE_STRING, szProgramAppData);
	strcat_s (szInfoAppData, SIZE_STRING, "\\ShellCommand.dat");

	iRes = _mkdir (szCompanyAppData);
	iRes = _mkdir (szProgramAppData);

	// Set our member variable which is used by the other functions
	ZeroMemory (m_szShellInfoPath, SIZE_STRING);
	strcpy_s (m_szShellInfoPath, SIZE_STRING, szInfoAppData);
	
	return true;
}

bool COpenWithCtxMenuExt::SaveFileList (bool bEncrypt)
{
	// This function saves the list of files the user has selected
	// to a temporary file in the ShellInfoPath - which is normally
	// application data/cedesoft/cedecrypt/shellcommand.dat
	if (strlen (m_szShellInfoPath) == 0) {
		return false;
	}

	if (FileExists (m_szShellInfoPath) == true) {
		if (DeleteFile (m_szShellInfoPath) == false) {
			return false;
		}
	}

	MemoryBuffer memFileList;
	DWORD dwFlag = 0;
	char szCurPath[SIZE_STRING];
	int iNumPaths = 0;
	int iTotalListSize = 0;
	int a = 0;
	int iCursize = 0;

	if (bEncrypt == true) {
		dwFlag = 100;
	} else {
		dwFlag = 200;
	}

	// First we need to work out the size of the total number of
	// items in the dynlist
	for (a=0;a<m_dlFileList.GetNumItems ();a++) {
		ZeroMemory (szCurPath, SIZE_STRING);
		m_dlFileList.GetItem (szCurPath, a);
		iTotalListSize += strlen (szCurPath);
		iTotalListSize += sizeof (int);
		iNumPaths++;
	}

	// Now we know the size, we can set the size;
	if (iNumPaths > 0) {
		
		memFileList.SetSize (iTotalListSize + sizeof (int) + sizeof (DWORD));

		// Now store the type of shell command, and the number of paths specified
		memFileList.Append (&dwFlag, sizeof (DWORD));
		memFileList.Append (&iNumPaths, sizeof (int));

		// Now serialise the dynlist into our memory buffer
		for (a=0;a<m_dlFileList.GetNumItems ();a++) {
			ZeroMemory (szCurPath, SIZE_STRING);
			m_dlFileList.GetItem (szCurPath, a);
			
			iCursize = strlen (szCurPath);
			
			memFileList.Append (&iCursize, sizeof (int));
			memFileList.Append (szCurPath, strlen (szCurPath));
		}
		
		// Now save the file
		if (memFileList.SaveToFile (m_szShellInfoPath) == false) {
			return false;
		} else {
			return true;
		}

	} else {
		return false;
	}

	
}

bool COpenWithCtxMenuExt::FileExists (char *FileName)
{
    FILE* fp = NULL;

    //will not work if you do not have read permissions
    //to the file, but if you don't have read, it
    //may as well not exist to begin with.

    fp = fopen( FileName, "rb" );
    if( fp != NULL )
    {
        fclose( fp );
        return true;
    }

    return false;
}

HRESULT COpenWithCtxMenuExt::InvokeCommand (LPCMINVOKECOMMANDINFO pCmdInfo)
{
    // If lpVerb really points to a string, ignore this function call and bail out.
	if ( 0 != HIWORD( pCmdInfo->lpVerb )) {
		return E_INVALIDARG;
	}
        
    // Get the command index.
    switch (LOWORD( pCmdInfo->lpVerb)) {
        case 0:
        {
			PrepareShellInfoPath ();
			if (SaveFileList (true) == true) {
				PrepareCedeCryptPath ();

				if (m_bCedeCryptPathfound == true) {
					ShellExecute (pCmdInfo->hwnd, _T("open"), m_szCedeCryptPath, NULL, NULL, SW_SHOW);
				} else {
					MessageBox (NULL, "CedeCrypt is not properly installed. Please reinstall the application.", "Unable to locate CedeCrypt", MB_OK | MB_ICONEXCLAMATION);
				}
			}
            
			//MessageBox (NULL, m_szShellInfoPath, "Selected", MB_OK);
            return S_OK;
        }
        break;
        case 1:
        {
			PrepareShellInfoPath ();
			if (SaveFileList (false) == true) {
				PrepareCedeCryptPath ();

				if (m_bCedeCryptPathfound == true) {
					ShellExecute (pCmdInfo->hwnd, _T("open"), m_szCedeCryptPath, NULL, NULL, SW_SHOW);
				} else {
					MessageBox (NULL, "CedeCrypt is not properly installed. Please reinstall the application.", "Unable to locate CedeCrypt", MB_OK | MB_ICONEXCLAMATION);
				}				
			}
            //ShellExecute (pCmdInfo->hwnd, _T("open"), _T("iexplore.exe"), m_szSelectedFile, NULL, SW_SHOW);
			//MessageBox (NULL, "Decrypt Option Selected.", "Selected", MB_OK);
            return S_OK;
        }
        break;
        default:
            return E_INVALIDARG;
        break;
    }
}
