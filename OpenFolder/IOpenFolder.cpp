// IOpenFolder.cpp : Implementation of COpenFolder
#define WINVER 0x0501 // Change this to the appropriate value to target Windows Me or later.
#include "stdafx.h"
#include <io.h>
#include <stdio.h>
#include <direct.h>
#include "OpenFolder.h"
#include "IOpenFolder.h"
#include "DynList.h"
#include "MemoryBuffer.h"

#include <strsafe.h>

#define ACTION_ENCRYPT			100
#define ACTION_DECRYPT			200
#define ACTION_ENCRYPTPACKAGE	300
#define ACTION_SECUREDELETE		400

/////////////////////////////////////////////////////////////////////////////
// OpenFolder

// Everything *

	// Our "IShellExtInit" implementation
/*lint -e18 -e534 -e1776 */
HRESULT OpenFolder::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdObj, HKEY /* hKeyProgId */)
{
	if(lpdObj) // DATAOBJECT provided, so process the files/folders that it gives us
	{
		//MessageBox (NULL, "Initialize - One", "Diagnositcs", MB_OK);

		FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		//MessageBox (NULL, "Initialize - One - 1", "Diagnositcs", MB_OK);
		STGMEDIUM stg = { TYMED_HGLOBAL };
		//MessageBox (NULL, "Initialize - One - 2", "Diagnositcs", MB_OK);
		HDROP     hDrop;

		//MessageBox (NULL, "Initialize - One - End", "Diagnositcs", MB_OK);

		// Look for CF_HDROP data in the data object.
		if ( FAILED( lpdObj->GetData ( &fmt, &stg ))) {
			// Nope! Return an "invalid argument" error back to Explorer.
			return E_INVALIDARG;
		}

		//MessageBox (NULL, "Initialize - Two", "Diagnositcs", MB_OK);

		// Get a pointer to the actual data.
		hDrop = (HDROP) GlobalLock ( stg.hGlobal );

		// Make sure it worked.
		if ( NULL == hDrop ) {
			return E_INVALIDARG;
		}
	    
		//MessageBox (NULL, "Initialize - Three", "Diagnositcs", MB_OK);

		// Sanity check - make sure there is at least one filename.
		UINT uNumFiles = DragQueryFile ( hDrop, 0xFFFFFFFF, NULL, 0 );

		if ( 0 == uNumFiles ) {
			GlobalUnlock ( stg.hGlobal );
			ReleaseStgMedium ( &stg );
			return E_INVALIDARG;
		}

		HRESULT hr = S_OK;

		//MessageBox (NULL, "Initialize - Four", "Diagnositcs", MB_OK);

		ZeroMemory (m_szShellInfoPath, SIZE_STRING);
		PrepareCedeCryptPath ();

		m_dlFileList.Clear ();

		for (int d=0;d<uNumFiles;d++) {
			ZeroMemory (m_szSelectedFile, MAX_PATH);
			if (DragQueryFile (hDrop, d, m_szSelectedFile, MAX_PATH) != 0) {
				//MessageBox (NULL, m_szSelectedFile, "File Selected.", MB_OK);
				m_dlFileList.AddItem (m_szSelectedFile, strlen (m_szSelectedFile), false);
			}
		}

		//MessageBox (NULL, "Initialize - Five", "Diagnositcs", MB_OK);

		GlobalUnlock ( stg.hGlobal );
		ReleaseStgMedium ( &stg );



		//MessageBox (NULL, "Initialize - Six", "Diagnositcs", MB_OK);

		return hr;
	
	} else { // No DATAOBJECT provided, so could be a right click on desktop or explorer backgrounds
		return E_INVALIDARG;
	}
}

void OpenFolder::PrepareCedeCryptPath ()
{
	// This function will read the standard registry location for
	// to obtain the location of the cedecrypt executable
	// for calling. The registry location is set when cedecrypt
	// is initially called with cedecrypt.exe /i . The /i switch
	// is called by the cedecryptsetup application.
	m_bCedeCryptPathfound = false;
	ZeroMemory (m_szCedeCryptPath, SIZE_STRING);
	
	//m_registry.WriteString ("Software\\CedeSoft\\CedeCrypt", "ShellExtensionPath", szModulepath);
	if (m_registry.DoesValueExistCU ("Software\\CedeSoft\\CedeCrypt", "ShellExtensionPath", REG_SZ) == true) {
		m_bCedeCryptPathfound = true;
		strcpy_s (m_szCedeCryptPath, SIZE_STRING, m_registry.ReadStringCU ("Software\\CedeSoft\\CedeCrypt", "ShellExtensionPath"));
	} else {
		m_bCedeCryptPathfound = false;
	}
}

bool OpenFolder::PrepareShellInfoPath ()
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

bool OpenFolder::SaveFileList (unsigned int iAction)
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

	/*if (bEncrypt == true) {
		dwFlag = 100;
	} else {
		dwFlag = 200;
	}*/

	dwFlag = iAction;

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

bool OpenFolder::FileExists (char *FileName)
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


		// Our "IContextMenu" implementation
HRESULT OpenFolder::GetCommandString(UINT_PTR idCmd,  UINT uFlags, UINT* /* pwReserved */, LPSTR pszName, UINT cchMax)
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
	
	//return S_OK; // must undo this finished diagnostics!!!
}

HRESULT OpenFolder::QueryContextMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
	//MessageBox (NULL, "QueryContextMenu - One", "Diagnositcs", MB_OK);
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
    InsertMenu ( hSubmenu, 1, MF_BYPOSITION, uID++, _T("Encrypt to Self-Decrypting &Package"));
	InsertMenu ( hSubmenu, 2, MF_BYPOSITION, uID++, _T("&Decrypt") );
	InsertMenu ( hSubmenu, 3, MF_BYPOSITION | MF_SEPARATOR, uID++, _T("") );
	InsertMenu ( hSubmenu, 4, MF_BYPOSITION, uID++, _T("&Secure Delete") );
	

    // Insert the submenu into the ctx menu provided by Explorer.
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };

    mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID | MIIM_CHECKMARKS;
    mii.wID = uID++;
    mii.hSubMenu = hSubmenu;
    mii.dwTypeData = _T("C&edeCrypt");
	mii.hbmpChecked = hbmIcon;
	mii.hbmpUnchecked = hbmIcon;

    InsertMenuItem ( hmenu, uMenuIndex, TRUE, &mii );


    //return MAKE_HRESULT (SEVERITY_SUCCESS, FACILITY_NULL, uID - uidFirstCmd + 1);
	return MAKE_HRESULT (SEVERITY_SUCCESS, 0, uID - uidFirstCmd + 1);
	
	//MessageBox (NULL, "QueryContextMenu - Two", "Diagnositcs", MB_OK);
}

		// And finally, open the Command Prompt itself
HRESULT OpenFolder::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
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
			if (SaveFileList (ACTION_ENCRYPT) == true) {
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
			if (SaveFileList (ACTION_ENCRYPTPACKAGE) == true) {
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
        case 2:
        {
			PrepareShellInfoPath ();
			if (SaveFileList (ACTION_DECRYPT) == true) {
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
		case 4:
		{
			PrepareShellInfoPath ();
			if (SaveFileList (ACTION_SECUREDELETE) == true) {

				//PrepareCedeCryptPath ();

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
/*lint +e18 +e534 +e1776 */
