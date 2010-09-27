// OpenWithCtxMenuExt.h : Declaration of the COpenWithCtxMenuExt

#ifndef __OPENWITHCTXMENUEXT_H_
#define __OPENWITHCTXMENUEXT_H_


#include "DynList.h"
#include "MemoryBuffer.h"
#include "RegistryHandler.h"
#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// COpenWithCtxMenuExt

class ATL_NO_VTABLE COpenWithCtxMenuExt : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<COpenWithCtxMenuExt, &CLSID_OpenWithCtxMenuExt>,
    public IShellExtInit,
    public IContextMenu
{
public:
    COpenWithCtxMenuExt()
    {
    }

	DECLARE_REGISTRY_RESOURCEID(IDR_OPENWITHCTXMENUEXT)
	DECLARE_NOT_AGGREGATABLE(COpenWithCtxMenuExt)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(COpenWithCtxMenuExt)
		COM_INTERFACE_ENTRY(IShellExtInit)
		COM_INTERFACE_ENTRY(IContextMenu)
	END_COM_MAP()

	public:
		// IShellExtInit
		STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

		// IContextMenu
		STDMETHOD(GetCommandString)(UINT, UINT, UINT*, LPSTR, UINT);
		STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO);
		STDMETHOD(QueryContextMenu)(HMENU, UINT, UINT, UINT, UINT);

		bool PrepareShellInfoPath ();
		void PrepareCedeCryptPath ();
		bool SaveFileList (bool bEncrypt);
		bool FileExists (char *FileName);

	protected:
		RegistryHandler m_registry;
		TCHAR m_szSelectedFile[MAX_PATH+2];
		DynList m_dlFileList;
		char m_szShellInfoPath[SIZE_STRING];

		bool m_bCedeCryptPathfound;
		char m_szCedeCryptPath[SIZE_STRING];
};

#endif //__OPENWITHCTXMENUEXT_H_
