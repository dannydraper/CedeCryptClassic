// IOpenFolder.cpp : Implementation of COpenFolder
#include "stdafx.h"

#include "OpenFolder.h"
#include "IOpenFolder.h"



#include <strsafe.h>

/////////////////////////////////////////////////////////////////////////////
// OpenFolder

// Everything *

	// Our "IShellExtInit" implementation
/*lint -e18 -e534 -e1776 */
HRESULT OpenFolder::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdObj, HKEY /* hKeyProgId */)
{
	/*
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP     hDrop;

    // Look for CF_HDROP data in the data object.
    if ( FAILED( lpdObj->GetData ( &fmt, &stg ))) {
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

	*/

	HRESULT hr = S_OK;
    return hr;
	

}



		// Our "IContextMenu" implementation
HRESULT OpenFolder::GetCommandString(UINT idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
	/*
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
	*/
	return S_OK;
}

HRESULT OpenFolder::QueryContextMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
	/*
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
    mii.dwTypeData = _T("C&yberCede v2");
	mii.hbmpChecked = hbmIcon;
	mii.hbmpUnchecked = hbmIcon;

    InsertMenuItem ( hmenu, uMenuIndex, TRUE, &mii );


    //return MAKE_HRESULT (SEVERITY_SUCCESS, FACILITY_NULL, uID - uidFirstCmd + 1);
	//return MAKE_HRESULT (SEVERITY_SUCCESS, 0, uID - uidFirstCmd + 1);
	*/
	return S_OK;
}

		// And finally, open the Command Prompt itself
HRESULT OpenFolder::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	/*
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
	*/
	return S_OK;
}
/*lint +e18 +e534 +e1776 */
