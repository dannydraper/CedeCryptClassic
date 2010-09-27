// IOpenFolder.h : Declaration of the COpenFolder

#ifndef __OPENFOLDER_H_
#define __OPENFOLDER_H_


/*lint -e537 */
#include "DynList.h"
#include "MemoryBuffer.h"
#include "RegistryHandler.h"
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

	HBITMAP bmpMenu_; // For the shield 'icon' bitmap next to the menu item.
	MENUITEMINFO obAdminMenuData_; // Our 'Admin' menu item

		// Various ANSI and UNICODE descriptions for the context menu
	std::string menuText_;
	std::string menuTextAdmin_;
	std::string menuDesc_;
	std::string menuDescAdmin_;
	std::wstring menuDescU_;
	std::wstring menuDescAdminU_;

		// The command id's for our menu items
	unsigned int cmdOpenFolder_;
	unsigned int cmdOpenFolderAdmin_;
	unsigned int cmdOpenFolderOffset_; // These are the offets of our commands
	unsigned int cmdOpenFolderAdminOffset_;

	char m_szCedeCryptPath[SIZE_STRING];
public:
	OpenFolder()		
	{ 		
	}

	~OpenFolder()
	{	
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
	STDMETHOD(GetCommandString)(UINT_PTR idCmd,  UINT uFlags, UINT* /* pwReserved */, LPSTR pszName, UINT cchMax); // *
	STDMETHOD(QueryContextMenu)(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags); // *
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO pici); // *

	bool PrepareShellInfoPath ();
	void PrepareCedeCryptPath ();
	bool SaveFileList (unsigned int iAction);
	bool FileExists (char *FileName);

	RegistryHandler m_registry;
	TCHAR m_szSelectedFile[MAX_PATH+2];
	DynList m_dlFileList;
	char m_szShellInfoPath[SIZE_STRING];

	bool m_bCedeCryptPathfound;
	
};
/*lint +e665 +e1550 +e1926 */

#endif //__OPENFOLDER_H_
//UINT_PTR
