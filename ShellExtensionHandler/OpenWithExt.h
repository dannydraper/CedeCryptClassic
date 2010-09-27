

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Sat Mar 14 09:32:30 2009
 */
/* Compiler settings for .\OpenWithExt.idl:
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

#ifndef __OpenWithExt_h__
#define __OpenWithExt_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IOpenWithCtxMenuExt_FWD_DEFINED__
#define __IOpenWithCtxMenuExt_FWD_DEFINED__
typedef interface IOpenWithCtxMenuExt IOpenWithCtxMenuExt;
#endif 	/* __IOpenWithCtxMenuExt_FWD_DEFINED__ */


#ifndef __OpenWithCtxMenuExt_FWD_DEFINED__
#define __OpenWithCtxMenuExt_FWD_DEFINED__

#ifdef __cplusplus
typedef class OpenWithCtxMenuExt OpenWithCtxMenuExt;
#else
typedef struct OpenWithCtxMenuExt OpenWithCtxMenuExt;
#endif /* __cplusplus */

#endif 	/* __OpenWithCtxMenuExt_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IOpenWithCtxMenuExt_INTERFACE_DEFINED__
#define __IOpenWithCtxMenuExt_INTERFACE_DEFINED__

/* interface IOpenWithCtxMenuExt */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IOpenWithCtxMenuExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4CAB8362-8A59-11DD-8EE8-433856D89593")
    IOpenWithCtxMenuExt : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IOpenWithCtxMenuExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOpenWithCtxMenuExt * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOpenWithCtxMenuExt * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOpenWithCtxMenuExt * This);
        
        END_INTERFACE
    } IOpenWithCtxMenuExtVtbl;

    interface IOpenWithCtxMenuExt
    {
        CONST_VTBL struct IOpenWithCtxMenuExtVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOpenWithCtxMenuExt_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOpenWithCtxMenuExt_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOpenWithCtxMenuExt_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IOpenWithCtxMenuExt_INTERFACE_DEFINED__ */



#ifndef __OPENWITHEXTLib_LIBRARY_DEFINED__
#define __OPENWITHEXTLib_LIBRARY_DEFINED__

/* library OPENWITHEXTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_OPENWITHEXTLib;

EXTERN_C const CLSID CLSID_OpenWithCtxMenuExt;

#ifdef __cplusplus

class DECLSPEC_UUID("1940F972-8A58-11DD-95F6-5D2956D89593")
OpenWithCtxMenuExt;
#endif
#endif /* __OPENWITHEXTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


