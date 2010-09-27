

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue Dec 15 15:41:51 2009
 */
/* Compiler settings for .\OpenFolder.idl:
    Oicf, W1, Zp8, env=Win64 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __OpenFolder_h__
#define __OpenFolder_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IOpenFolder_FWD_DEFINED__
#define __IOpenFolder_FWD_DEFINED__
typedef interface IOpenFolder IOpenFolder;
#endif 	/* __IOpenFolder_FWD_DEFINED__ */


#ifndef __OpenFolder_FWD_DEFINED__
#define __OpenFolder_FWD_DEFINED__

#ifdef __cplusplus
typedef class OpenFolder OpenFolder;
#else
typedef struct OpenFolder OpenFolder;
#endif /* __cplusplus */

#endif 	/* __OpenFolder_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IOpenFolder_INTERFACE_DEFINED__
#define __IOpenFolder_INTERFACE_DEFINED__

/* interface IOpenFolder */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IOpenFolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("90A7C85C-C7B7-4D83-81CD-E76A9CFAE412")
    IOpenFolder : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IOpenFolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOpenFolder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOpenFolder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOpenFolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOpenFolder * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOpenFolder * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOpenFolder * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOpenFolder * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IOpenFolderVtbl;

    interface IOpenFolder
    {
        CONST_VTBL struct IOpenFolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOpenFolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOpenFolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOpenFolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOpenFolder_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOpenFolder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOpenFolder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOpenFolder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IOpenFolder_INTERFACE_DEFINED__ */



#ifndef __OPENFOLDERLib_LIBRARY_DEFINED__
#define __OPENFOLDERLib_LIBRARY_DEFINED__

/* library OPENFOLDERLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_OPENFOLDERLib;

EXTERN_C const CLSID CLSID_OpenFolder;

#ifdef __cplusplus

class DECLSPEC_UUID("2DE2822E-4FFC-4DBC-B214-D94B8087C5BE")
OpenFolder;
#endif
#endif /* __OPENFOLDERLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


