// IOpenFolder.h : Declaration of the COpenFolder

#ifndef __OPENFOLDER_H_
#define __OPENFOLDER_H_

/*lint -e537 */

#include "resource.h"    // main symbols
#include "OSVersion.h"
using namespace darka;
/*lint +e537 */

// * At the end of a line indicates my additions

#include <shlguid.h>
#include <comdef.h> // GUID values for "IShellExtInit" and "IContextMenu"
#include <shlobj.h> // The shell objects

struct __declspec(uuid("{000214E4-0000-0000-C000-000000000046}")) IContextMenu;
//struct __declspec(uuid("{000214F4-0000-0000-C000-000000000046}")) IContextMenu2;
//struct __declspec(uuid("{BCFCE0A0-EC17-11d0-8D10-00A0C90F2719}")) IContextMenu3;

	// Our Command Verbs
const char OPENFOLDERADMIN[] = "OpenFolderAdmin";
const wchar_t OPENFOLDERADMINU[] = L"OpenFolderAdmin";
const char OPENFOLDER[] = "OpenFolder";
const wchar_t OPENFOLDERU[] = L"OpenFolder";

/////////////////////////////////////////////////////////////////////////////
// COpenFolder

/*lint -e665 -e1550 -e1926 */
class ATL_NO_VTABLE OpenFolder : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<OpenFolder, &CLSID_OpenFolder>,
	public IShellExtInit, // So that Windows can initialize our shell extension *
	public IContextMenu // Context menu handler *
{
private:
    TCHAR szFileName_[_MAX_PATH]; // The folder / file selected
	OSVersion obVersion_;

	// Various ANSI and UNICODE descriptions for the context menu






public:
	OpenFolder()
		
	{ 
		
	}

	~OpenFolder()
	{
		//DeleteObject(bmpMenu_);
		//bmpMenu_ = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_OPENFOLDER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

/*lint -e778 */
BEGIN_COM_MAP(OpenFolder)
	COM_INTERFACE_ENTRY(IContextMenu) // *
	COM_INTERFACE_ENTRY(IShellExtInit) // *
END_COM_MAP()
/*lint +e778 */

	// IOpenFolder
public:
		// Our "IShellExtInit" implementation
	STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder, LPDATAOBJECT lpdObj, HKEY hKeyProgId); // *

		// Our "IContextMenu" implementation
	STDMETHOD(GetCommandString)(UINT idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax); // *
	STDMETHOD(QueryContextMenu)(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags); // *
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO pCmdInfo); // *
};
/*lint +e665 +e1550 +e1926 */

#endif //__OPENFOLDER_H_
//UINT_PTR
