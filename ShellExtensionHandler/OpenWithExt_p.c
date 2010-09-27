

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "OpenWithExt.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _OpenWithExt_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } OpenWithExt_MIDL_TYPE_FORMAT_STRING;

typedef struct _OpenWithExt_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } OpenWithExt_MIDL_PROC_FORMAT_STRING;

typedef struct _OpenWithExt_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } OpenWithExt_MIDL_EXPR_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const OpenWithExt_MIDL_TYPE_FORMAT_STRING OpenWithExt__MIDL_TypeFormatString;
extern const OpenWithExt_MIDL_PROC_FORMAT_STRING OpenWithExt__MIDL_ProcFormatString;
extern const OpenWithExt_MIDL_EXPR_FORMAT_STRING OpenWithExt__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IOpenWithCtxMenuExt_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IOpenWithCtxMenuExt_ProxyInfo;



#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const OpenWithExt_MIDL_PROC_FORMAT_STRING OpenWithExt__MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const OpenWithExt_MIDL_TYPE_FORMAT_STRING OpenWithExt__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IOpenWithCtxMenuExt, ver. 0.0,
   GUID={0x4CAB8362,0x8A59,0x11DD,{0x8E,0xE8,0x43,0x38,0x56,0xD8,0x95,0x93}} */

#pragma code_seg(".orpc")
static const unsigned short IOpenWithCtxMenuExt_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IOpenWithCtxMenuExt_ProxyInfo =
    {
    &Object_StubDesc,
    OpenWithExt__MIDL_ProcFormatString.Format,
    &IOpenWithCtxMenuExt_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IOpenWithCtxMenuExt_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    OpenWithExt__MIDL_ProcFormatString.Format,
    &IOpenWithCtxMenuExt_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IOpenWithCtxMenuExtProxyVtbl = 
{
    0,
    &IID_IOpenWithCtxMenuExt,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IOpenWithCtxMenuExtStubVtbl =
{
    &IID_IOpenWithCtxMenuExt,
    &IOpenWithCtxMenuExt_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    OpenWithExt__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x70001f4, /* MIDL Version 7.0.500 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * _OpenWithExt_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IOpenWithCtxMenuExtProxyVtbl,
    0
};

const CInterfaceStubVtbl * _OpenWithExt_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IOpenWithCtxMenuExtStubVtbl,
    0
};

PCInterfaceName const _OpenWithExt_InterfaceNamesList[] = 
{
    "IOpenWithCtxMenuExt",
    0
};


#define _OpenWithExt_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _OpenWithExt, pIID, n)

int __stdcall _OpenWithExt_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_OpenWithExt_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo OpenWithExt_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _OpenWithExt_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _OpenWithExt_StubVtblList,
    (const PCInterfaceName * ) & _OpenWithExt_InterfaceNamesList,
    0, // no delegation
    & _OpenWithExt_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

