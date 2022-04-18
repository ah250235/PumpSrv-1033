

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Fri Apr 15 15:06:41 2022
 */
/* Compiler settings for PumpSrv.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

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

#ifndef __PumpSrv_h__
#define __PumpSrv_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IGPI_FWD_DEFINED__
#define __IGPI_FWD_DEFINED__
typedef interface IGPI IGPI;

#endif 	/* __IGPI_FWD_DEFINED__ */


#ifndef __IGCI_FWD_DEFINED__
#define __IGCI_FWD_DEFINED__
typedef interface IGCI IGCI;

#endif 	/* __IGCI_FWD_DEFINED__ */


#ifndef __IGCIEvents_FWD_DEFINED__
#define __IGCIEvents_FWD_DEFINED__
typedef interface IGCIEvents IGCIEvents;

#endif 	/* __IGCIEvents_FWD_DEFINED__ */


#ifndef __ICtrl_FWD_DEFINED__
#define __ICtrl_FWD_DEFINED__
typedef interface ICtrl ICtrl;

#endif 	/* __ICtrl_FWD_DEFINED__ */


#ifndef __ICtrlEvents_FWD_DEFINED__
#define __ICtrlEvents_FWD_DEFINED__
typedef interface ICtrlEvents ICtrlEvents;

#endif 	/* __ICtrlEvents_FWD_DEFINED__ */


#ifndef __IGPIEvents_FWD_DEFINED__
#define __IGPIEvents_FWD_DEFINED__
typedef interface IGPIEvents IGPIEvents;

#endif 	/* __IGPIEvents_FWD_DEFINED__ */


#ifndef __GPI_FWD_DEFINED__
#define __GPI_FWD_DEFINED__

#ifdef __cplusplus
typedef class GPI GPI;
#else
typedef struct GPI GPI;
#endif /* __cplusplus */

#endif 	/* __GPI_FWD_DEFINED__ */


#ifndef __GCI_FWD_DEFINED__
#define __GCI_FWD_DEFINED__

#ifdef __cplusplus
typedef class GCI GCI;
#else
typedef struct GCI GCI;
#endif /* __cplusplus */

#endif 	/* __GCI_FWD_DEFINED__ */


#ifndef __Ctrl_FWD_DEFINED__
#define __Ctrl_FWD_DEFINED__

#ifdef __cplusplus
typedef class Ctrl Ctrl;
#else
typedef struct Ctrl Ctrl;
#endif /* __cplusplus */

#endif 	/* __Ctrl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_PumpSrv_0000_0000 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_PumpSrv_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_PumpSrv_0000_0000_v0_0_s_ifspec;

#ifndef __IGPI_INTERFACE_DEFINED__
#define __IGPI_INTERFACE_DEFINED__

/* interface IGPI */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IGPI;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7C9373A1-1D37-11D1-B8A5-0000E8D0CBF8")
    IGPI : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitInterface( 
            /* [in] */ long lPosNumber,
            /* [in] */ long lVersion,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetServerState( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNumberOfPumps( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpInfo( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpStatus( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpTransact( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMinimalPumpsInfo( 
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTrsState( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuthorizePump( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StopPump( 
            /* [in] */ long lPumpNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RestartPump( 
            /* [in] */ long lPumpNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuthorizeAndLimit( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ short nFlags,
            /* [out][in] */ long *lSyncNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReservePump( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnReservePump( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [in] */ BOOL bForceRelease,
            /* [in] */ long lByDevice,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPAPTrs( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemovePAPTrs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSeqNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPrePaidTrs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemovePrePaidTrs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMinimalPumpsInfo2( 
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitInterface2( 
            /* [in] */ long lPosNumber,
            /* [in] */ long lVersion,
            /* [in] */ long lFlags1,
            /* [in] */ long lFlags2,
            /* [in] */ long lFlags3,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpMode( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMode,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpTransact2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpInfo2( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPrePaidTrs2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPAPTrs2( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpStatus2( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParam( 
            /* [in] */ BSTR sTableAndFieldName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out] */ BSTR *sVal,
            /* [out] */ long *lVal,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryPAPTrs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryPrePaidTrs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetProcess( 
            /* [in] */ long lProcessNumber,
            /* [in] */ long lParam1,
            /* [in] */ long lParam2,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProcess( 
            /* [in] */ long lProcessNumber,
            /* [out] */ long *plParam1,
            /* [out] */ long *plParam2,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParam2( 
            /* [out][in] */ BSTR *sTableFieldAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out] */ long *lVal,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GenerateWashCode( 
            /* [in] */ long lDevice,
            /* [in] */ long lWashProgram,
            /* [in] */ long lFlags,
            /* [out] */ long *lSyncNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWashCode( 
            /* [in] */ long lSyncNumber,
            /* [out] */ long *lResult,
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CancelWashCode( 
            /* [in] */ long lSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuthorizePump2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nFlags,
            /* [in] */ long lLimit,
            /* [in] */ short nGroup,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StopPump2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nGroup,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPAPTrs3( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryPAPTrs2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReceipt( 
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sTrack2,
            /* [in] */ short nIndex,
            /* [out] */ BSTR *sReceiptData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReceiptPrinted( 
            /* [in] */ BSTR sTrack2,
            /* [in] */ short nIndex,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOLAMemInfo( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nIndex,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *Pval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpInfo3( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetServiceFee( 
            /* [in] */ long lPump,
            /* [in] */ long lTrsType,
            /* [in] */ long lTrsNumber,
            /* [in] */ BSTR stTime,
            /* [in] */ long lFlags,
            /* [out] */ long *pValue,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMinimalPumpsInfo3( 
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGradePrices( 
            /* [in] */ short nGrade,
            /* [in] */ long lFlags,
            /* [out] */ long *lCashFullPrice,
            /* [out] */ long *lCashSelfPrice,
            /* [out] */ long *lCreditFullPrice,
            /* [out] */ long *lCreditSelfPrice,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLevelPrice( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nGrade,
            /* [in] */ short nLevel,
            /* [in] */ long lServiceMode,
            /* [in] */ long lFlags,
            /* [out] */ long *lPrice,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOLAMemInfo2( 
            /* [in] */ long lPumpNumber,
            /* [out][in] */ short *nIndex,
            /* [in] */ long lFlags,
            /* [in] */ long lVal,
            /* [in] */ BSTR sVal,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMemOla( 
            /* [in] */ long lPump,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lVal,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOlaMemInfoStatus( 
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTankInfo( 
            /* [in] */ long lTankNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [out][retval] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTransactionState( 
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetStationStatus( 
            /* [out][in] */ long *lPumpMap1,
            /* [out][in] */ long *lPumpMap2,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuthorizeAndLimit2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryExtraInfoData( 
            /* [in] */ long lPumpNumber,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReceipt2( 
            /* [in] */ long lPosNumber,
            /* [in] */ BSTR sTrack2,
            /* [in] */ long lIndex,
            /* [out] */ BSTR *sReceiptData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReceiptPrinted2( 
            /* [in] */ BSTR sTrack2,
            /* [in] */ long lIndex,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryReceipt( 
            /* [in] */ long lPosNumber,
            /* [in] */ BSTR sTrack2,
            /* [out][in] */ short *nIndex,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *lRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SwitchServer( 
            /* [in] */ long lNewServerType,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSingleProductInfo( 
            /* [in] */ long lPump,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lGrade,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAlarmInfo( 
            /* [in] */ long lCategory,
            /* [in] */ long lSubCategory,
            /* [in] */ long lDevice,
            /* [out][in] */ long *plAlarmId,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AckAlarm( 
            /* [in] */ long lCategory,
            /* [in] */ long lSubCategory,
            /* [in] */ long lAlarmId,
            /* [in] */ long lDevice,
            /* [in] */ long lAlarmNumber,
            /* [in] */ long lCustomerId,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpModeScheme3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [in] */ long lMode,
            /* [in] */ BSTR sPeriodDateTimeStart,
            /* [in] */ BSTR sPeriodDateTimeEnd,
            /* [in] */ BSTR sDateTimeStart,
            /* [in] */ BSTR sDateTimeEnd,
            /* [in] */ long lServiceFeeNo,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpModeScheme3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [out] */ long *plMode,
            /* [out] */ BSTR *psPeriodDateTimeStart,
            /* [out] */ BSTR *psPeriodDateTimeEnd,
            /* [out] */ BSTR *psDateTimeStart,
            /* [out] */ BSTR *psDateTimeEnd,
            /* [out] */ long *plServiceFeeNo,
            /* [out] */ long *plFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAlarmByCategory( 
            /* [in] */ long lCategory,
            /* [in] */ long lSubCategory,
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [out] */ long *plTotalAlarms,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAlarmById( 
            /* [in] */ long lAlarmId,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPAKQuery( 
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPAPTrs4( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LockPump( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lLock,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAccumulateTotals( 
            /* [in] */ long lShiftNumber,
            /* [in] */ long lState,
            /* [in] */ long lPosNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lVal,
            /* [in] */ BSTR sStartTime,
            /* [in] */ BSTR sEndTime,
            /* [out] */ long *lRecordCount,
            /* [out][in] */ BSTR *sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAccumulateState( 
            /* [in] */ long lShiftNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPrePayTrsState( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [in] */ long lStatus,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVersion( 
            /* [out] */ BSTR *sVersionList,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CancelWashCode2( 
            /* [out][in] */ long *plSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [in] */ long lRefundCarWash,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReport( 
            /* [in] */ long lNumber,
            /* [in] */ long lType,
            /* [out] */ BSTR *sReportData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *lRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReceiptRawData( 
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sTrack2,
            /* [out][in] */ short *nIndex,
            /* [out] */ BSTR *sReceiptRawData,
            /* [out][in] */ long *plFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpGradeInfo( 
            /* [in] */ long lPump,
            /* [out] */ BSTR *sGradesData,
            /* [out] */ long *lRecordcount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAllTrsQuery( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddItem2Trs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lItemSelected,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTrsState2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitEventsInterface( 
            /* [in] */ long lFlags,
            /* [out][in] */ long *pClientId,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CloseEventsInterface( 
            /* [in] */ long lClientId,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PAPSwipe( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDeviceNumber,
            /* [out][in] */ long *lSeqNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PAPReadyTransaction( 
            /* [in] */ long lPumpNum,
            /* [in] */ long lSeqNum,
            /* [in] */ long lTrsNum,
            /* [in] */ short nNozzle,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParam3( 
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpStatus( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lStatus,
            /* [in] */ long lRes,
            /* [in] */ long lResExt,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuthorizePump3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lFlags,
            /* [in] */ long lFlags2,
            /* [in] */ long lLimit,
            /* [in] */ short nGroup,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPAPTrs5( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CheckAndReserve( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lPosNumber,
            /* [in] */ long lPumpRes,
            /* [in] */ long lStatus,
            /* [in] */ BSTR sRec,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lTrsNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpTransact5( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPAKQuery2( 
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryPrePaidTrs2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAllTrsQuery2( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOLAMemInfo3( 
            /* [in] */ long lPumpNumber,
            /* [out][in] */ short *nIndex,
            /* [in] */ long lFlags,
            /* [in] */ long lVal,
            /* [in] */ BSTR sVal,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpMisc( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMisc,
            /* [in] */ long bRemoveMisc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAlarm( 
            /* [in] */ long lCatagory,
            /* [in] */ long lSubCatagory,
            /* [in] */ long lAlarmNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lAlarmStatus,
            /* [in] */ long lAlarmValue,
            /* [in] */ long lFlags,
            /* [out] */ long *pAlarmId,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PAPNoFuelReadyTrs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [in] */ BSTR sParam1,
            /* [in] */ long lParam2,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSaleExtraDetails( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPAPTrs6( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [out] */ BSTR *pLoyaltySummaryXml,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuthorizeAndLimit3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [in] */ long lPurchaseID,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FinalizePrePayTrs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lFlags2,
            /* [in] */ long lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [in] */ long lPurchaseID,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPAKTrs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FinalizePAKTrs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpsTrsInfo( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pPumpsInfo,
            /* [out] */ BSTR *pAllTrsInfo,
            /* [out] */ BSTR *pGetPumpsInfo3,
            /* [out][in] */ long *pTrsCount,
            /* [in] */ long lGetPumpsFlags,
            /* [in] */ long lGetTrsFlags,
            /* [in] */ long lGetPumpInfo3Flags,
            /* [in] */ long lParam,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RequestReportReady( 
            /* [in] */ long lType,
            /* [in] */ long lFlags,
            /* [out] */ BOOL *pbIsReady,
            /* [out] */ BSTR *pbstrReportBuffer,
            /* [out] */ long *lpResultCode,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTrsState3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetANPRCommand( 
            /* [in] */ long lCommand,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags1,
            /* [in] */ long lFlags2,
            /* [in] */ BSTR pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetServerStateEx( 
            /* [out][in] */ long *pServerStateEx,
            /* [in] */ long lFlag,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryExtraInfoData2( 
            /* [in] */ long lPumpNumber,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [out] */ BSTR *pExtraRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParam4( 
            /* [out][in] */ BSTR *sTableFieldAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ long lFlags,
            /* [out] */ long *lVal,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ValidateWashCode( 
            /* [in] */ long lDevice,
            /* [out] */ long *lSyncNumber,
            /* [in] */ long lWashCode,
            /* [in] */ long lWashProgramID,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpMiscExt( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMisc,
            /* [in] */ long bRemoveMisc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuthorizeAndLimit4( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [in] */ __int64 lPurchaseID,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FinalizePrePayTrs2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lFlags2,
            /* [in] */ long lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [in] */ __int64 lPurchaseID,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPrePaidTrs3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ __int64 lSyncNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateManualPostVoidTransaction( 
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR pRec,
            /* [in] */ long lFlags,
            /* [out] */ long *lNewTrsNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTrsFlags( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lMiscFlags,
            /* [in] */ long lMiscFlags2,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DisablePriceDisplay( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnablePriceDisplay( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuthorizePump4( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lFlags,
            /* [in] */ long lFlags2,
            /* [in] */ long lLimit,
            /* [in] */ short nGroup,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOfflineTanks( 
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sTankList,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetProcessExt( 
            /* [in] */ long lProcessNumber,
            /* [in] */ long lParam1,
            /* [in] */ BSTR sInData,
            /* [out] */ BSTR *sOutData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurrentTransactionNumber( 
            long lPumpNumber,
            long *pTrsNumber,
            long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitDebit( 
            /* [in] */ long lPumpGroup01,
            /* [in] */ long lPumpGroup2,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryPumpInfo( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlag,
            /* [in] */ BSTR sInData,
            /* [out] */ BSTR *sOutData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAllWashProgram( 
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sOutData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GenerateWashCode2( 
            /* [in] */ long lDevice,
            /* [in] */ BSTR lWashProgram,
            /* [in] */ long lFlags,
            /* [out] */ long *lSyncNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveWashProgram( 
            /* [in] */ BSTR sInData,
            /* [retval][out] */ long *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IGPIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGPI * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGPI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGPI * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGPI * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGPI * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGPI * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGPI * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitInterface )( 
            IGPI * This,
            /* [in] */ long lPosNumber,
            /* [in] */ long lVersion,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetServerState )( 
            IGPI * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNumberOfPumps )( 
            IGPI * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpInfo )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpStatus )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpTransact )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMinimalPumpsInfo )( 
            IGPI * This,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTrsState )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuthorizePump )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StopPump )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RestartPump )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuthorizeAndLimit )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ short nFlags,
            /* [out][in] */ long *lSyncNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReservePump )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnReservePump )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [in] */ BOOL bForceRelease,
            /* [in] */ long lByDevice,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPAPTrs )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemovePAPTrs )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSeqNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPrePaidTrs )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemovePrePaidTrs )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMinimalPumpsInfo2 )( 
            IGPI * This,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitInterface2 )( 
            IGPI * This,
            /* [in] */ long lPosNumber,
            /* [in] */ long lVersion,
            /* [in] */ long lFlags1,
            /* [in] */ long lFlags2,
            /* [in] */ long lFlags3,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpMode )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMode,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpTransact2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpInfo2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPrePaidTrs2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPAPTrs2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpStatus2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParam )( 
            IGPI * This,
            /* [in] */ BSTR sTableAndFieldName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out] */ BSTR *sVal,
            /* [out] */ long *lVal,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryPAPTrs )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryPrePaidTrs )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetProcess )( 
            IGPI * This,
            /* [in] */ long lProcessNumber,
            /* [in] */ long lParam1,
            /* [in] */ long lParam2,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProcess )( 
            IGPI * This,
            /* [in] */ long lProcessNumber,
            /* [out] */ long *plParam1,
            /* [out] */ long *plParam2,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParam2 )( 
            IGPI * This,
            /* [out][in] */ BSTR *sTableFieldAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out] */ long *lVal,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GenerateWashCode )( 
            IGPI * This,
            /* [in] */ long lDevice,
            /* [in] */ long lWashProgram,
            /* [in] */ long lFlags,
            /* [out] */ long *lSyncNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWashCode )( 
            IGPI * This,
            /* [in] */ long lSyncNumber,
            /* [out] */ long *lResult,
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CancelWashCode )( 
            IGPI * This,
            /* [in] */ long lSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuthorizePump2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nFlags,
            /* [in] */ long lLimit,
            /* [in] */ short nGroup,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StopPump2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nGroup,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPAPTrs3 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryPAPTrs2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReceipt )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sTrack2,
            /* [in] */ short nIndex,
            /* [out] */ BSTR *sReceiptData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReceiptPrinted )( 
            IGPI * This,
            /* [in] */ BSTR sTrack2,
            /* [in] */ short nIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOLAMemInfo )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nIndex,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *Pval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpInfo3 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetServiceFee )( 
            IGPI * This,
            /* [in] */ long lPump,
            /* [in] */ long lTrsType,
            /* [in] */ long lTrsNumber,
            /* [in] */ BSTR stTime,
            /* [in] */ long lFlags,
            /* [out] */ long *pValue,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMinimalPumpsInfo3 )( 
            IGPI * This,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetGradePrices )( 
            IGPI * This,
            /* [in] */ short nGrade,
            /* [in] */ long lFlags,
            /* [out] */ long *lCashFullPrice,
            /* [out] */ long *lCashSelfPrice,
            /* [out] */ long *lCreditFullPrice,
            /* [out] */ long *lCreditSelfPrice,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevelPrice )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nGrade,
            /* [in] */ short nLevel,
            /* [in] */ long lServiceMode,
            /* [in] */ long lFlags,
            /* [out] */ long *lPrice,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOLAMemInfo2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out][in] */ short *nIndex,
            /* [in] */ long lFlags,
            /* [in] */ long lVal,
            /* [in] */ BSTR sVal,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMemOla )( 
            IGPI * This,
            /* [in] */ long lPump,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lVal,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOlaMemInfoStatus )( 
            IGPI * This,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTankInfo )( 
            IGPI * This,
            /* [in] */ long lTankNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [out][retval] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTransactionState )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetStationStatus )( 
            IGPI * This,
            /* [out][in] */ long *lPumpMap1,
            /* [out][in] */ long *lPumpMap2,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuthorizeAndLimit2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryExtraInfoData )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReceipt2 )( 
            IGPI * This,
            /* [in] */ long lPosNumber,
            /* [in] */ BSTR sTrack2,
            /* [in] */ long lIndex,
            /* [out] */ BSTR *sReceiptData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReceiptPrinted2 )( 
            IGPI * This,
            /* [in] */ BSTR sTrack2,
            /* [in] */ long lIndex,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryReceipt )( 
            IGPI * This,
            /* [in] */ long lPosNumber,
            /* [in] */ BSTR sTrack2,
            /* [out][in] */ short *nIndex,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *lRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SwitchServer )( 
            IGPI * This,
            /* [in] */ long lNewServerType,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSingleProductInfo )( 
            IGPI * This,
            /* [in] */ long lPump,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lGrade,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAlarmInfo )( 
            IGPI * This,
            /* [in] */ long lCategory,
            /* [in] */ long lSubCategory,
            /* [in] */ long lDevice,
            /* [out][in] */ long *plAlarmId,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AckAlarm )( 
            IGPI * This,
            /* [in] */ long lCategory,
            /* [in] */ long lSubCategory,
            /* [in] */ long lAlarmId,
            /* [in] */ long lDevice,
            /* [in] */ long lAlarmNumber,
            /* [in] */ long lCustomerId,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpModeScheme3 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [in] */ long lMode,
            /* [in] */ BSTR sPeriodDateTimeStart,
            /* [in] */ BSTR sPeriodDateTimeEnd,
            /* [in] */ BSTR sDateTimeStart,
            /* [in] */ BSTR sDateTimeEnd,
            /* [in] */ long lServiceFeeNo,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpModeScheme3 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [out] */ long *plMode,
            /* [out] */ BSTR *psPeriodDateTimeStart,
            /* [out] */ BSTR *psPeriodDateTimeEnd,
            /* [out] */ BSTR *psDateTimeStart,
            /* [out] */ BSTR *psDateTimeEnd,
            /* [out] */ long *plServiceFeeNo,
            /* [out] */ long *plFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAlarmByCategory )( 
            IGPI * This,
            /* [in] */ long lCategory,
            /* [in] */ long lSubCategory,
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [out] */ long *plTotalAlarms,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAlarmById )( 
            IGPI * This,
            /* [in] */ long lAlarmId,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPAKQuery )( 
            IGPI * This,
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPAPTrs4 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LockPump )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lLock,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAccumulateTotals )( 
            IGPI * This,
            /* [in] */ long lShiftNumber,
            /* [in] */ long lState,
            /* [in] */ long lPosNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lVal,
            /* [in] */ BSTR sStartTime,
            /* [in] */ BSTR sEndTime,
            /* [out] */ long *lRecordCount,
            /* [out][in] */ BSTR *sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAccumulateState )( 
            IGPI * This,
            /* [in] */ long lShiftNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPrePayTrsState )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [in] */ long lStatus,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            IGPI * This,
            /* [out] */ BSTR *sVersionList,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CancelWashCode2 )( 
            IGPI * This,
            /* [out][in] */ long *plSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [in] */ long lRefundCarWash,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReport )( 
            IGPI * This,
            /* [in] */ long lNumber,
            /* [in] */ long lType,
            /* [out] */ BSTR *sReportData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *lRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReceiptRawData )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sTrack2,
            /* [out][in] */ short *nIndex,
            /* [out] */ BSTR *sReceiptRawData,
            /* [out][in] */ long *plFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpGradeInfo )( 
            IGPI * This,
            /* [in] */ long lPump,
            /* [out] */ BSTR *sGradesData,
            /* [out] */ long *lRecordcount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAllTrsQuery )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddItem2Trs )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lItemSelected,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTrsState2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitEventsInterface )( 
            IGPI * This,
            /* [in] */ long lFlags,
            /* [out][in] */ long *pClientId,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CloseEventsInterface )( 
            IGPI * This,
            /* [in] */ long lClientId,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PAPSwipe )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDeviceNumber,
            /* [out][in] */ long *lSeqNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PAPReadyTransaction )( 
            IGPI * This,
            /* [in] */ long lPumpNum,
            /* [in] */ long lSeqNum,
            /* [in] */ long lTrsNum,
            /* [in] */ short nNozzle,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParam3 )( 
            IGPI * This,
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpStatus )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lStatus,
            /* [in] */ long lRes,
            /* [in] */ long lResExt,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuthorizePump3 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lFlags,
            /* [in] */ long lFlags2,
            /* [in] */ long lLimit,
            /* [in] */ short nGroup,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPAPTrs5 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CheckAndReserve )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lPosNumber,
            /* [in] */ long lPumpRes,
            /* [in] */ long lStatus,
            /* [in] */ BSTR sRec,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lTrsNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpTransact5 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPAKQuery2 )( 
            IGPI * This,
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryPrePaidTrs2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAllTrsQuery2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOLAMemInfo3 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out][in] */ short *nIndex,
            /* [in] */ long lFlags,
            /* [in] */ long lVal,
            /* [in] */ BSTR sVal,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpMisc )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMisc,
            /* [in] */ long bRemoveMisc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAlarm )( 
            IGPI * This,
            /* [in] */ long lCatagory,
            /* [in] */ long lSubCatagory,
            /* [in] */ long lAlarmNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lAlarmStatus,
            /* [in] */ long lAlarmValue,
            /* [in] */ long lFlags,
            /* [out] */ long *pAlarmId,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PAPNoFuelReadyTrs )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [in] */ BSTR sParam1,
            /* [in] */ long lParam2,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSaleExtraDetails )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPAPTrs6 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [out] */ BSTR *pLoyaltySummaryXml,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuthorizeAndLimit3 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [in] */ long lPurchaseID,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FinalizePrePayTrs )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lFlags2,
            /* [in] */ long lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [in] */ long lPurchaseID,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPAKTrs )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSyncNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FinalizePAKTrs )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpsTrsInfo )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pPumpsInfo,
            /* [out] */ BSTR *pAllTrsInfo,
            /* [out] */ BSTR *pGetPumpsInfo3,
            /* [out][in] */ long *pTrsCount,
            /* [in] */ long lGetPumpsFlags,
            /* [in] */ long lGetTrsFlags,
            /* [in] */ long lGetPumpInfo3Flags,
            /* [in] */ long lParam,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RequestReportReady )( 
            IGPI * This,
            /* [in] */ long lType,
            /* [in] */ long lFlags,
            /* [out] */ BOOL *pbIsReady,
            /* [out] */ BSTR *pbstrReportBuffer,
            /* [out] */ long *lpResultCode,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTrsState3 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetANPRCommand )( 
            IGPI * This,
            /* [in] */ long lCommand,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags1,
            /* [in] */ long lFlags2,
            /* [in] */ BSTR pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetServerStateEx )( 
            IGPI * This,
            /* [out][in] */ long *pServerStateEx,
            /* [in] */ long lFlag,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryExtraInfoData2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [out] */ BSTR *pExtraRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParam4 )( 
            IGPI * This,
            /* [out][in] */ BSTR *sTableFieldAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ long lFlags,
            /* [out] */ long *lVal,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ValidateWashCode )( 
            IGPI * This,
            /* [in] */ long lDevice,
            /* [out] */ long *lSyncNumber,
            /* [in] */ long lWashCode,
            /* [in] */ long lWashProgramID,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpMiscExt )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMisc,
            /* [in] */ long bRemoveMisc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuthorizeAndLimit4 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [in] */ __int64 lPurchaseID,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FinalizePrePayTrs2 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lFlags2,
            /* [in] */ long lSyncNumber,
            /* [in] */ BSTR sRec,
            /* [in] */ __int64 lPurchaseID,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPrePaidTrs3 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ __int64 lSyncNumber,
            /* [in] */ short nFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateManualPostVoidTransaction )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR pRec,
            /* [in] */ long lFlags,
            /* [out] */ long *lNewTrsNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTrsFlags )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lMiscFlags,
            /* [in] */ long lMiscFlags2,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DisablePriceDisplay )( 
            IGPI * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnablePriceDisplay )( 
            IGPI * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuthorizePump4 )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lFlags,
            /* [in] */ long lFlags2,
            /* [in] */ long lLimit,
            /* [in] */ short nGroup,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOfflineTanks )( 
            IGPI * This,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sTankList,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetProcessExt )( 
            IGPI * This,
            /* [in] */ long lProcessNumber,
            /* [in] */ long lParam1,
            /* [in] */ BSTR sInData,
            /* [out] */ BSTR *sOutData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCurrentTransactionNumber )( 
            IGPI * This,
            long lPumpNumber,
            long *pTrsNumber,
            long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitDebit )( 
            IGPI * This,
            /* [in] */ long lPumpGroup01,
            /* [in] */ long lPumpGroup2,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryPumpInfo )( 
            IGPI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlag,
            /* [in] */ BSTR sInData,
            /* [out] */ BSTR *sOutData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAllWashProgram )( 
            IGPI * This,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sOutData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GenerateWashCode2 )( 
            IGPI * This,
            /* [in] */ long lDevice,
            /* [in] */ BSTR lWashProgram,
            /* [in] */ long lFlags,
            /* [out] */ long *lSyncNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveWashProgram )( 
            IGPI * This,
            /* [in] */ BSTR sInData,
            /* [retval][out] */ long *pVal);
        
        END_INTERFACE
    } IGPIVtbl;

    interface IGPI
    {
        CONST_VTBL struct IGPIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGPI_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGPI_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGPI_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGPI_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IGPI_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IGPI_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IGPI_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IGPI_InitInterface(This,lPosNumber,lVersion,pVal)	\
    ( (This)->lpVtbl -> InitInterface(This,lPosNumber,lVersion,pVal) ) 

#define IGPI_GetServerState(This,pVal)	\
    ( (This)->lpVtbl -> GetServerState(This,pVal) ) 

#define IGPI_GetNumberOfPumps(This,pVal)	\
    ( (This)->lpVtbl -> GetNumberOfPumps(This,pVal) ) 

#define IGPI_GetPumpInfo(This,lPumpNumber,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPumpInfo(This,lPumpNumber,pRec,pVal) ) 

#define IGPI_GetPumpStatus(This,lPumpNumber,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPumpStatus(This,lPumpNumber,pRec,pVal) ) 

#define IGPI_GetPumpTransact(This,lPumpNumber,lTrsNumber,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPumpTransact(This,lPumpNumber,lTrsNumber,pRec,pVal) ) 

#define IGPI_GetMinimalPumpsInfo(This,pRec,pVal)	\
    ( (This)->lpVtbl -> GetMinimalPumpsInfo(This,pRec,pVal) ) 

#define IGPI_SetTrsState(This,lPumpNumber,lTrsNumber,lState,pVal)	\
    ( (This)->lpVtbl -> SetTrsState(This,lPumpNumber,lTrsNumber,lState,pVal) ) 

#define IGPI_AuthorizePump(This,lPumpNumber,nLevel,lGradePrice,pVal)	\
    ( (This)->lpVtbl -> AuthorizePump(This,lPumpNumber,nLevel,lGradePrice,pVal) ) 

#define IGPI_StopPump(This,lPumpNumber,pVal)	\
    ( (This)->lpVtbl -> StopPump(This,lPumpNumber,pVal) ) 

#define IGPI_RestartPump(This,lPumpNumber,pVal)	\
    ( (This)->lpVtbl -> RestartPump(This,lPumpNumber,pVal) ) 

#define IGPI_AuthorizeAndLimit(This,lPumpNumber,nLevel,lGradePrice,lMaxValue,lMaxVolume,nFlags,lSyncNumber,pVal)	\
    ( (This)->lpVtbl -> AuthorizeAndLimit(This,lPumpNumber,nLevel,lGradePrice,lMaxValue,lMaxVolume,nFlags,lSyncNumber,pVal) ) 

#define IGPI_ReservePump(This,lPumpNumber,lSyncNumber,pVal)	\
    ( (This)->lpVtbl -> ReservePump(This,lPumpNumber,lSyncNumber,pVal) ) 

#define IGPI_UnReservePump(This,lPumpNumber,lSyncNumber,bForceRelease,lByDevice,pVal)	\
    ( (This)->lpVtbl -> UnReservePump(This,lPumpNumber,lSyncNumber,bForceRelease,lByDevice,pVal) ) 

#define IGPI_GetPAPTrs(This,lPumpNumber,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPAPTrs(This,lPumpNumber,pRec,pVal) ) 

#define IGPI_RemovePAPTrs(This,lPumpNumber,lSeqNumber,pVal)	\
    ( (This)->lpVtbl -> RemovePAPTrs(This,lPumpNumber,lSeqNumber,pVal) ) 

#define IGPI_GetPrePaidTrs(This,lPumpNumber,lSyncNumber,nFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPrePaidTrs(This,lPumpNumber,lSyncNumber,nFlags,pRec,pVal) ) 

#define IGPI_RemovePrePaidTrs(This,lPumpNumber,lSyncNumber,pVal)	\
    ( (This)->lpVtbl -> RemovePrePaidTrs(This,lPumpNumber,lSyncNumber,pVal) ) 

#define IGPI_GetMinimalPumpsInfo2(This,pRec,pVal)	\
    ( (This)->lpVtbl -> GetMinimalPumpsInfo2(This,pRec,pVal) ) 

#define IGPI_InitInterface2(This,lPosNumber,lVersion,lFlags1,lFlags2,lFlags3,pVal)	\
    ( (This)->lpVtbl -> InitInterface2(This,lPosNumber,lVersion,lFlags1,lFlags2,lFlags3,pVal) ) 

#define IGPI_SetPumpMode(This,lPumpNumber,lMode,pVal)	\
    ( (This)->lpVtbl -> SetPumpMode(This,lPumpNumber,lMode,pVal) ) 

#define IGPI_GetPumpTransact2(This,lPumpNumber,lTrsNumber,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPumpTransact2(This,lPumpNumber,lTrsNumber,pRec,pVal) ) 

#define IGPI_GetPumpInfo2(This,lPumpNumber,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPumpInfo2(This,lPumpNumber,pRec,pVal) ) 

#define IGPI_GetPrePaidTrs2(This,lPumpNumber,lSyncNumber,nFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPrePaidTrs2(This,lPumpNumber,lSyncNumber,nFlags,pRec,pVal) ) 

#define IGPI_GetPAPTrs2(This,lPumpNumber,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPAPTrs2(This,lPumpNumber,pRec,pVal) ) 

#define IGPI_GetPumpStatus2(This,lPumpNumber,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPumpStatus2(This,lPumpNumber,pRec,pVal) ) 

#define IGPI_GetParam(This,sTableAndFieldName,lKey1,lKey2,nFiledType,sVal,lVal,pVal)	\
    ( (This)->lpVtbl -> GetParam(This,sTableAndFieldName,lKey1,lKey2,nFiledType,sVal,lVal,pVal) ) 

#define IGPI_QueryPAPTrs(This,lPumpNumber,lIndexNumber,nFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> QueryPAPTrs(This,lPumpNumber,lIndexNumber,nFlags,pRec,pVal) ) 

#define IGPI_QueryPrePaidTrs(This,lPumpNumber,lIndexNumber,nFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> QueryPrePaidTrs(This,lPumpNumber,lIndexNumber,nFlags,pRec,pVal) ) 

#define IGPI_SetProcess(This,lProcessNumber,lParam1,lParam2,pVal)	\
    ( (This)->lpVtbl -> SetProcess(This,lProcessNumber,lParam1,lParam2,pVal) ) 

#define IGPI_GetProcess(This,lProcessNumber,plParam1,plParam2,pVal)	\
    ( (This)->lpVtbl -> GetProcess(This,lProcessNumber,plParam1,plParam2,pVal) ) 

#define IGPI_GetParam2(This,sTableFieldAndVal,lKey1,lKey2,nFiledType,lVal,pVal)	\
    ( (This)->lpVtbl -> GetParam2(This,sTableFieldAndVal,lKey1,lKey2,nFiledType,lVal,pVal) ) 

#define IGPI_GenerateWashCode(This,lDevice,lWashProgram,lFlags,lSyncNumber,pVal)	\
    ( (This)->lpVtbl -> GenerateWashCode(This,lDevice,lWashProgram,lFlags,lSyncNumber,pVal) ) 

#define IGPI_GetWashCode(This,lSyncNumber,lResult,sData,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetWashCode(This,lSyncNumber,lResult,sData,lFlags,pVal) ) 

#define IGPI_CancelWashCode(This,lSyncNumber,lDevice,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> CancelWashCode(This,lSyncNumber,lDevice,lFlags,sData,pVal) ) 

#define IGPI_AuthorizePump2(This,lPumpNumber,nLevel,lGradePrice,nFlags,lLimit,nGroup,pVal)	\
    ( (This)->lpVtbl -> AuthorizePump2(This,lPumpNumber,nLevel,lGradePrice,nFlags,lLimit,nGroup,pVal) ) 

#define IGPI_StopPump2(This,lPumpNumber,nGroup,pVal)	\
    ( (This)->lpVtbl -> StopPump2(This,lPumpNumber,nGroup,pVal) ) 

#define IGPI_GetPAPTrs3(This,lPumpNumber,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPAPTrs3(This,lPumpNumber,pRec,pVal) ) 

#define IGPI_QueryPAPTrs2(This,lPumpNumber,lIndexNumber,nFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> QueryPAPTrs2(This,lPumpNumber,lIndexNumber,nFlags,pRec,pVal) ) 

#define IGPI_GetReceipt(This,lPumpNumber,sTrack2,nIndex,sReceiptData,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetReceipt(This,lPumpNumber,sTrack2,nIndex,sReceiptData,lFlags,pVal) ) 

#define IGPI_ReceiptPrinted(This,sTrack2,nIndex,pVal)	\
    ( (This)->lpVtbl -> ReceiptPrinted(This,sTrack2,nIndex,pVal) ) 

#define IGPI_GetOLAMemInfo(This,lPumpNumber,nIndex,lFlags,pRec,Pval)	\
    ( (This)->lpVtbl -> GetOLAMemInfo(This,lPumpNumber,nIndex,lFlags,pRec,Pval) ) 

#define IGPI_GetPumpInfo3(This,lPumpNumber,pRec,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetPumpInfo3(This,lPumpNumber,pRec,lFlags,pVal) ) 

#define IGPI_GetServiceFee(This,lPump,lTrsType,lTrsNumber,stTime,lFlags,pValue,pRetVal)	\
    ( (This)->lpVtbl -> GetServiceFee(This,lPump,lTrsType,lTrsNumber,stTime,lFlags,pValue,pRetVal) ) 

#define IGPI_GetMinimalPumpsInfo3(This,pRec,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetMinimalPumpsInfo3(This,pRec,lFlags,pVal) ) 

#define IGPI_GetGradePrices(This,nGrade,lFlags,lCashFullPrice,lCashSelfPrice,lCreditFullPrice,lCreditSelfPrice,pVal)	\
    ( (This)->lpVtbl -> GetGradePrices(This,nGrade,lFlags,lCashFullPrice,lCashSelfPrice,lCreditFullPrice,lCreditSelfPrice,pVal) ) 

#define IGPI_GetLevelPrice(This,lPumpNumber,nGrade,nLevel,lServiceMode,lFlags,lPrice,pVal)	\
    ( (This)->lpVtbl -> GetLevelPrice(This,lPumpNumber,nGrade,nLevel,lServiceMode,lFlags,lPrice,pVal) ) 

#define IGPI_GetOLAMemInfo2(This,lPumpNumber,nIndex,lFlags,lVal,sVal,pRec,pVal)	\
    ( (This)->lpVtbl -> GetOLAMemInfo2(This,lPumpNumber,nIndex,lFlags,lVal,sVal,pRec,pVal) ) 

#define IGPI_SetMemOla(This,lPump,lTrsNumber,lFlags,lVal,pVal)	\
    ( (This)->lpVtbl -> SetMemOla(This,lPump,lTrsNumber,lFlags,lVal,pVal) ) 

#define IGPI_GetOlaMemInfoStatus(This,lFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> GetOlaMemInfoStatus(This,lFlags,pRec,pVal) ) 

#define IGPI_GetTankInfo(This,lTankNumber,pRec,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetTankInfo(This,lTankNumber,pRec,lFlags,pVal) ) 

#define IGPI_SetTransactionState(This,lPumpNumber,pRec,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetTransactionState(This,lPumpNumber,pRec,lFlags,pVal) ) 

#define IGPI_GetStationStatus(This,lPumpMap1,lPumpMap2,lFlags,pRetVal)	\
    ( (This)->lpVtbl -> GetStationStatus(This,lPumpMap1,lPumpMap2,lFlags,pRetVal) ) 

#define IGPI_AuthorizeAndLimit2(This,lPumpNumber,nLevel,lGradePrice,lMaxValue,lMaxVolume,lFlags,lSyncNumber,sRec,pVal)	\
    ( (This)->lpVtbl -> AuthorizeAndLimit2(This,lPumpNumber,nLevel,lGradePrice,lMaxValue,lMaxVolume,lFlags,lSyncNumber,sRec,pVal) ) 

#define IGPI_QueryExtraInfoData(This,lPumpNumber,lIndexNumber,nFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> QueryExtraInfoData(This,lPumpNumber,lIndexNumber,nFlags,pRec,pVal) ) 

#define IGPI_GetReceipt2(This,lPosNumber,sTrack2,lIndex,sReceiptData,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetReceipt2(This,lPosNumber,sTrack2,lIndex,sReceiptData,lFlags,pVal) ) 

#define IGPI_ReceiptPrinted2(This,sTrack2,lIndex,lFlags,pVal)	\
    ( (This)->lpVtbl -> ReceiptPrinted2(This,sTrack2,lIndex,lFlags,pVal) ) 

#define IGPI_QueryReceipt(This,lPosNumber,sTrack2,nIndex,nFlags,pData,lRetVal)	\
    ( (This)->lpVtbl -> QueryReceipt(This,lPosNumber,sTrack2,nIndex,nFlags,pData,lRetVal) ) 

#define IGPI_SwitchServer(This,lNewServerType,plRetVal)	\
    ( (This)->lpVtbl -> SwitchServer(This,lNewServerType,plRetVal) ) 

#define IGPI_GetSingleProductInfo(This,lPump,lFlags,lGrade,pVal)	\
    ( (This)->lpVtbl -> GetSingleProductInfo(This,lPump,lFlags,lGrade,pVal) ) 

#define IGPI_GetAlarmInfo(This,lCategory,lSubCategory,lDevice,plAlarmId,lFlags,pData,plVal)	\
    ( (This)->lpVtbl -> GetAlarmInfo(This,lCategory,lSubCategory,lDevice,plAlarmId,lFlags,pData,plVal) ) 

#define IGPI_AckAlarm(This,lCategory,lSubCategory,lAlarmId,lDevice,lAlarmNumber,lCustomerId,lFlags,sData,plVal)	\
    ( (This)->lpVtbl -> AckAlarm(This,lCategory,lSubCategory,lAlarmId,lDevice,lAlarmNumber,lCustomerId,lFlags,sData,plVal) ) 

#define IGPI_SetPumpModeScheme3(This,lPumpNumber,lScheme,lMode,sPeriodDateTimeStart,sPeriodDateTimeEnd,sDateTimeStart,sDateTimeEnd,lServiceFeeNo,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetPumpModeScheme3(This,lPumpNumber,lScheme,lMode,sPeriodDateTimeStart,sPeriodDateTimeEnd,sDateTimeStart,sDateTimeEnd,lServiceFeeNo,lFlags,pVal) ) 

#define IGPI_GetPumpModeScheme3(This,lPumpNumber,lScheme,plMode,psPeriodDateTimeStart,psPeriodDateTimeEnd,psDateTimeStart,psDateTimeEnd,plServiceFeeNo,plFlags,pVal)	\
    ( (This)->lpVtbl -> GetPumpModeScheme3(This,lPumpNumber,lScheme,plMode,psPeriodDateTimeStart,psPeriodDateTimeEnd,psDateTimeStart,psDateTimeEnd,plServiceFeeNo,plFlags,pVal) ) 

#define IGPI_GetAlarmByCategory(This,lCategory,lSubCategory,lDevice,lFlags,plTotalAlarms,pData,plVal)	\
    ( (This)->lpVtbl -> GetAlarmByCategory(This,lCategory,lSubCategory,lDevice,lFlags,plTotalAlarms,pData,plVal) ) 

#define IGPI_GetAlarmById(This,lAlarmId,lFlags,pData,plVal)	\
    ( (This)->lpVtbl -> GetAlarmById(This,lAlarmId,lFlags,pData,plVal) ) 

#define IGPI_GetPAKQuery(This,sData,plRecCount,lFlags,plRetVal)	\
    ( (This)->lpVtbl -> GetPAKQuery(This,sData,plRecCount,lFlags,plRetVal) ) 

#define IGPI_GetPAPTrs4(This,lPumpNumber,pRec,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetPAPTrs4(This,lPumpNumber,pRec,lFlags,pVal) ) 

#define IGPI_LockPump(This,lPumpNumber,lLock,lFlags,plVal)	\
    ( (This)->lpVtbl -> LockPump(This,lPumpNumber,lLock,lFlags,plVal) ) 

#define IGPI_GetAccumulateTotals(This,lShiftNumber,lState,lPosNumber,lFlags,lVal,sStartTime,sEndTime,lRecordCount,sRec,pVal)	\
    ( (This)->lpVtbl -> GetAccumulateTotals(This,lShiftNumber,lState,lPosNumber,lFlags,lVal,sStartTime,sEndTime,lRecordCount,sRec,pVal) ) 

#define IGPI_SetAccumulateState(This,lShiftNumber,lState,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetAccumulateState(This,lShiftNumber,lState,lFlags,pVal) ) 

#define IGPI_SetPrePayTrsState(This,lPumpNumber,lSyncNumber,lStatus,lFlags,plVal)	\
    ( (This)->lpVtbl -> SetPrePayTrsState(This,lPumpNumber,lSyncNumber,lStatus,lFlags,plVal) ) 

#define IGPI_GetVersion(This,sVersionList,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetVersion(This,sVersionList,lFlags,pVal) ) 

#define IGPI_CancelWashCode2(This,plSyncNumber,lDevice,lFlags,sData,lRefundCarWash,pVal)	\
    ( (This)->lpVtbl -> CancelWashCode2(This,plSyncNumber,lDevice,lFlags,sData,lRefundCarWash,pVal) ) 

#define IGPI_GetReport(This,lNumber,lType,sReportData,lFlags,lRet)	\
    ( (This)->lpVtbl -> GetReport(This,lNumber,lType,sReportData,lFlags,lRet) ) 

#define IGPI_GetReceiptRawData(This,lPumpNumber,sTrack2,nIndex,sReceiptRawData,plFlags,pVal)	\
    ( (This)->lpVtbl -> GetReceiptRawData(This,lPumpNumber,sTrack2,nIndex,sReceiptRawData,plFlags,pVal) ) 

#define IGPI_GetPumpGradeInfo(This,lPump,sGradesData,lRecordcount,lFlags,plVal)	\
    ( (This)->lpVtbl -> GetPumpGradeInfo(This,lPump,sGradesData,lRecordcount,lFlags,plVal) ) 

#define IGPI_GetAllTrsQuery(This,lPumpNumber,sData,plRecCount,lFlags,plRetVal)	\
    ( (This)->lpVtbl -> GetAllTrsQuery(This,lPumpNumber,sData,plRecCount,lFlags,plRetVal) ) 

#define IGPI_AddItem2Trs(This,lPumpNumber,lItemSelected,lFlags,plRetVal)	\
    ( (This)->lpVtbl -> AddItem2Trs(This,lPumpNumber,lItemSelected,lFlags,plRetVal) ) 

#define IGPI_SetTrsState2(This,lPumpNumber,lTrsNumber,lState,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetTrsState2(This,lPumpNumber,lTrsNumber,lState,lFlags,pVal) ) 

#define IGPI_InitEventsInterface(This,lFlags,pClientId,pVal)	\
    ( (This)->lpVtbl -> InitEventsInterface(This,lFlags,pClientId,pVal) ) 

#define IGPI_CloseEventsInterface(This,lClientId,pVal)	\
    ( (This)->lpVtbl -> CloseEventsInterface(This,lClientId,pVal) ) 

#define IGPI_PAPSwipe(This,lPumpNumber,lDeviceNumber,lSeqNumber,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> PAPSwipe(This,lPumpNumber,lDeviceNumber,lSeqNumber,lFlags,sData,pVal) ) 

#define IGPI_PAPReadyTransaction(This,lPumpNum,lSeqNum,lTrsNum,nNozzle,lVolume,lValue,nLevel,lGradePrice,nGrade,lFlags,sData,sRetData,pVal)	\
    ( (This)->lpVtbl -> PAPReadyTransaction(This,lPumpNum,lSeqNum,lTrsNum,nNozzle,lVolume,lValue,nLevel,lGradePrice,nGrade,lFlags,sData,sRetData,pVal) ) 

#define IGPI_GetParam3(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal)	\
    ( (This)->lpVtbl -> GetParam3(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal) ) 

#define IGPI_SetPumpStatus(This,lPumpNumber,lStatus,lRes,lResExt,pVal)	\
    ( (This)->lpVtbl -> SetPumpStatus(This,lPumpNumber,lStatus,lRes,lResExt,pVal) ) 

#define IGPI_AuthorizePump3(This,lPumpNumber,nLevel,lGradePrice,lFlags,lFlags2,lLimit,nGroup,pVal)	\
    ( (This)->lpVtbl -> AuthorizePump3(This,lPumpNumber,nLevel,lGradePrice,lFlags,lFlags2,lLimit,nGroup,pVal) ) 

#define IGPI_GetPAPTrs5(This,lPumpNumber,pRec,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetPAPTrs5(This,lPumpNumber,pRec,lFlags,pVal) ) 

#define IGPI_CheckAndReserve(This,lPumpNumber,lPosNumber,lPumpRes,lStatus,sRec,lFlags,lTrsNumber,pVal)	\
    ( (This)->lpVtbl -> CheckAndReserve(This,lPumpNumber,lPosNumber,lPumpRes,lStatus,sRec,lFlags,lTrsNumber,pVal) ) 

#define IGPI_GetPumpTransact5(This,lPumpNumber,lTrsNumber,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPumpTransact5(This,lPumpNumber,lTrsNumber,pRec,pVal) ) 

#define IGPI_GetPAKQuery2(This,sData,plRecCount,lFlags,plRetVal)	\
    ( (This)->lpVtbl -> GetPAKQuery2(This,sData,plRecCount,lFlags,plRetVal) ) 

#define IGPI_QueryPrePaidTrs2(This,lPumpNumber,lIndexNumber,nFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> QueryPrePaidTrs2(This,lPumpNumber,lIndexNumber,nFlags,pRec,pVal) ) 

#define IGPI_GetAllTrsQuery2(This,lPumpNumber,sData,plRecCount,lFlags,plRetVal)	\
    ( (This)->lpVtbl -> GetAllTrsQuery2(This,lPumpNumber,sData,plRecCount,lFlags,plRetVal) ) 

#define IGPI_GetOLAMemInfo3(This,lPumpNumber,nIndex,lFlags,lVal,sVal,pRec,pVal)	\
    ( (This)->lpVtbl -> GetOLAMemInfo3(This,lPumpNumber,nIndex,lFlags,lVal,sVal,pRec,pVal) ) 

#define IGPI_SetPumpMisc(This,lPumpNumber,lMisc,bRemoveMisc,pVal)	\
    ( (This)->lpVtbl -> SetPumpMisc(This,lPumpNumber,lMisc,bRemoveMisc,pVal) ) 

#define IGPI_SetAlarm(This,lCatagory,lSubCatagory,lAlarmNumber,lDevice,lAlarmStatus,lAlarmValue,lFlags,pAlarmId,sRec,pVal)	\
    ( (This)->lpVtbl -> SetAlarm(This,lCatagory,lSubCatagory,lAlarmNumber,lDevice,lAlarmStatus,lAlarmValue,lFlags,pAlarmId,sRec,pVal) ) 

#define IGPI_PAPNoFuelReadyTrs(This,lPumpNumber,lFlags,sData,sParam1,lParam2,pVal)	\
    ( (This)->lpVtbl -> PAPNoFuelReadyTrs(This,lPumpNumber,lFlags,sData,sParam1,lParam2,pVal) ) 

#define IGPI_GetSaleExtraDetails(This,lPumpNumber,lFlags,sData,sRetData,pVal)	\
    ( (This)->lpVtbl -> GetSaleExtraDetails(This,lPumpNumber,lFlags,sData,sRetData,pVal) ) 

#define IGPI_GetPAPTrs6(This,lPumpNumber,pRec,pLoyaltySummaryXml,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetPAPTrs6(This,lPumpNumber,pRec,pLoyaltySummaryXml,lFlags,pVal) ) 

#define IGPI_AuthorizeAndLimit3(This,lPumpNumber,nLevel,lGradePrice,lMaxValue,lMaxVolume,lFlags,lSyncNumber,sRec,lPurchaseID,pVal)	\
    ( (This)->lpVtbl -> AuthorizeAndLimit3(This,lPumpNumber,nLevel,lGradePrice,lMaxValue,lMaxVolume,lFlags,lSyncNumber,sRec,lPurchaseID,pVal) ) 

#define IGPI_FinalizePrePayTrs(This,lPumpNumber,lFlags,lFlags2,lSyncNumber,sRec,lPurchaseID,pVal)	\
    ( (This)->lpVtbl -> FinalizePrePayTrs(This,lPumpNumber,lFlags,lFlags2,lSyncNumber,sRec,lPurchaseID,pVal) ) 

#define IGPI_GetPAKTrs(This,lPumpNumber,lSyncNumber,lFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPAKTrs(This,lPumpNumber,lSyncNumber,lFlags,pRec,pVal) ) 

#define IGPI_FinalizePAKTrs(This,lPumpNumber,lFlags,lSyncNumber,sRec,pVal)	\
    ( (This)->lpVtbl -> FinalizePAKTrs(This,lPumpNumber,lFlags,lSyncNumber,sRec,pVal) ) 

#define IGPI_GetPumpsTrsInfo(This,lPumpNumber,pPumpsInfo,pAllTrsInfo,pGetPumpsInfo3,pTrsCount,lGetPumpsFlags,lGetTrsFlags,lGetPumpInfo3Flags,lParam,pVal)	\
    ( (This)->lpVtbl -> GetPumpsTrsInfo(This,lPumpNumber,pPumpsInfo,pAllTrsInfo,pGetPumpsInfo3,pTrsCount,lGetPumpsFlags,lGetTrsFlags,lGetPumpInfo3Flags,lParam,pVal) ) 

#define IGPI_RequestReportReady(This,lType,lFlags,pbIsReady,pbstrReportBuffer,lpResultCode,pVal)	\
    ( (This)->lpVtbl -> RequestReportReady(This,lType,lFlags,pbIsReady,pbstrReportBuffer,lpResultCode,pVal) ) 

#define IGPI_SetTrsState3(This,lPumpNumber,lTrsNumber,lState,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> SetTrsState3(This,lPumpNumber,lTrsNumber,lState,lFlags,sData,pVal) ) 

#define IGPI_SetANPRCommand(This,lCommand,lPumpNumber,lFlags1,lFlags2,pRec,pVal)	\
    ( (This)->lpVtbl -> SetANPRCommand(This,lCommand,lPumpNumber,lFlags1,lFlags2,pRec,pVal) ) 

#define IGPI_GetServerStateEx(This,pServerStateEx,lFlag,pVal)	\
    ( (This)->lpVtbl -> GetServerStateEx(This,pServerStateEx,lFlag,pVal) ) 

#define IGPI_QueryExtraInfoData2(This,lPumpNumber,lIndexNumber,nFlags,pRec,pExtraRec,pVal)	\
    ( (This)->lpVtbl -> QueryExtraInfoData2(This,lPumpNumber,lIndexNumber,nFlags,pRec,pExtraRec,pVal) ) 

#define IGPI_GetParam4(This,sTableFieldAndVal,lKey1,lKey2,nFiledType,lFlags,lVal,pVal)	\
    ( (This)->lpVtbl -> GetParam4(This,sTableFieldAndVal,lKey1,lKey2,nFiledType,lFlags,lVal,pVal) ) 

#define IGPI_ValidateWashCode(This,lDevice,lSyncNumber,lWashCode,lWashProgramID,lFlags,pVal)	\
    ( (This)->lpVtbl -> ValidateWashCode(This,lDevice,lSyncNumber,lWashCode,lWashProgramID,lFlags,pVal) ) 

#define IGPI_SetPumpMiscExt(This,lPumpNumber,lMisc,bRemoveMisc,pVal)	\
    ( (This)->lpVtbl -> SetPumpMiscExt(This,lPumpNumber,lMisc,bRemoveMisc,pVal) ) 

#define IGPI_AuthorizeAndLimit4(This,lPumpNumber,nLevel,lGradePrice,lMaxValue,lMaxVolume,lFlags,lSyncNumber,sRec,lPurchaseID,pVal)	\
    ( (This)->lpVtbl -> AuthorizeAndLimit4(This,lPumpNumber,nLevel,lGradePrice,lMaxValue,lMaxVolume,lFlags,lSyncNumber,sRec,lPurchaseID,pVal) ) 

#define IGPI_FinalizePrePayTrs2(This,lPumpNumber,lFlags,lFlags2,lSyncNumber,sRec,lPurchaseID,pVal)	\
    ( (This)->lpVtbl -> FinalizePrePayTrs2(This,lPumpNumber,lFlags,lFlags2,lSyncNumber,sRec,lPurchaseID,pVal) ) 

#define IGPI_GetPrePaidTrs3(This,lPumpNumber,lSyncNumber,nFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPrePaidTrs3(This,lPumpNumber,lSyncNumber,nFlags,pRec,pVal) ) 

#define IGPI_CreateManualPostVoidTransaction(This,lPumpNumber,pRec,lFlags,lNewTrsNumber,pVal)	\
    ( (This)->lpVtbl -> CreateManualPostVoidTransaction(This,lPumpNumber,pRec,lFlags,lNewTrsNumber,pVal) ) 

#define IGPI_SetTrsFlags(This,lPumpNumber,lTrsNumber,lMiscFlags,lMiscFlags2,pVal)	\
    ( (This)->lpVtbl -> SetTrsFlags(This,lPumpNumber,lTrsNumber,lMiscFlags,lMiscFlags2,pVal) ) 

#define IGPI_DisablePriceDisplay(This,pVal)	\
    ( (This)->lpVtbl -> DisablePriceDisplay(This,pVal) ) 

#define IGPI_EnablePriceDisplay(This,pVal)	\
    ( (This)->lpVtbl -> EnablePriceDisplay(This,pVal) ) 

#define IGPI_AuthorizePump4(This,lPumpNumber,nLevel,lGradePrice,lFlags,lFlags2,lLimit,nGroup,sRec,pVal)	\
    ( (This)->lpVtbl -> AuthorizePump4(This,lPumpNumber,nLevel,lGradePrice,lFlags,lFlags2,lLimit,nGroup,sRec,pVal) ) 

#define IGPI_GetOfflineTanks(This,lFlags,sTankList,pVal)	\
    ( (This)->lpVtbl -> GetOfflineTanks(This,lFlags,sTankList,pVal) ) 

#define IGPI_SetProcessExt(This,lProcessNumber,lParam1,sInData,sOutData,pVal)	\
    ( (This)->lpVtbl -> SetProcessExt(This,lProcessNumber,lParam1,sInData,sOutData,pVal) ) 

#define IGPI_GetCurrentTransactionNumber(This,lPumpNumber,pTrsNumber,pVal)	\
    ( (This)->lpVtbl -> GetCurrentTransactionNumber(This,lPumpNumber,pTrsNumber,pVal) ) 

#define IGPI_InitDebit(This,lPumpGroup01,lPumpGroup2,lFlags,pVal)	\
    ( (This)->lpVtbl -> InitDebit(This,lPumpGroup01,lPumpGroup2,lFlags,pVal) ) 

#define IGPI_QueryPumpInfo(This,lPumpNumber,lFlag,sInData,sOutData,pVal)	\
    ( (This)->lpVtbl -> QueryPumpInfo(This,lPumpNumber,lFlag,sInData,sOutData,pVal) ) 

#define IGPI_GetAllWashProgram(This,lFlags,sOutData,pVal)	\
    ( (This)->lpVtbl -> GetAllWashProgram(This,lFlags,sOutData,pVal) ) 

#define IGPI_GenerateWashCode2(This,lDevice,lWashProgram,lFlags,lSyncNumber,pVal)	\
    ( (This)->lpVtbl -> GenerateWashCode2(This,lDevice,lWashProgram,lFlags,lSyncNumber,pVal) ) 

#define IGPI_SaveWashProgram(This,sInData,pVal)	\
    ( (This)->lpVtbl -> SaveWashProgram(This,sInData,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGPI_SetProcessExt_Proxy( 
    IGPI * This,
    /* [in] */ long lProcessNumber,
    /* [in] */ long lParam1,
    /* [in] */ BSTR sInData,
    /* [out] */ BSTR *sOutData,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IGPI_SetProcessExt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGPI_GetCurrentTransactionNumber_Proxy( 
    IGPI * This,
    long lPumpNumber,
    long *pTrsNumber,
    long *pVal);


void __RPC_STUB IGPI_GetCurrentTransactionNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGPI_InitDebit_Proxy( 
    IGPI * This,
    /* [in] */ long lPumpGroup01,
    /* [in] */ long lPumpGroup2,
    /* [in] */ long lFlags,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IGPI_InitDebit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGPI_QueryPumpInfo_Proxy( 
    IGPI * This,
    /* [in] */ long lPumpNumber,
    /* [in] */ long lFlag,
    /* [in] */ BSTR sInData,
    /* [out] */ BSTR *sOutData,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IGPI_QueryPumpInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGPI_GetAllWashProgram_Proxy( 
    IGPI * This,
    /* [in] */ long lFlags,
    /* [out] */ BSTR *sOutData,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IGPI_GetAllWashProgram_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGPI_GenerateWashCode2_Proxy( 
    IGPI * This,
    /* [in] */ long lDevice,
    /* [in] */ BSTR lWashProgram,
    /* [in] */ long lFlags,
    /* [out] */ long *lSyncNumber,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IGPI_GenerateWashCode2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGPI_SaveWashProgram_Proxy( 
    IGPI * This,
    /* [in] */ BSTR sInData,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IGPI_SaveWashProgram_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGPI_INTERFACE_DEFINED__ */


#ifndef __IGCI_INTERFACE_DEFINED__
#define __IGCI_INTERFACE_DEFINED__

/* interface IGCI */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IGCI;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7C9373A3-1D37-11D1-B8A5-0000E8D0CBF8")
    IGCI : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpStatus( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lStatus,
            /* [in] */ long lRes,
            /* [in] */ long lResExt,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteToLog( 
            /* [in] */ long lPumpNumber,
            /* [in] */ BOOL bErrorRep,
            /* [in] */ long lError,
            /* [in] */ BOOL bWarningRep,
            /* [in] */ long lWarning,
            /* [in] */ BOOL bDeviceRep,
            /* [in] */ long lDeviceCode,
            /* [in] */ long lDevice,
            /* [in] */ BOOL bMiscRep,
            /* [in] */ long lMisc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitInterface( 
            /* [in] */ BOOL bControllerIsMaster,
            /* [in] */ BOOL bControllerGenTrsNumber,
            /* [in] */ BSTR strControllerName,
            /* [in] */ long lControllerVer,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CancelTransaction( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PAKReadyTransaction( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ short nNozzle,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PAPReadyTransaction( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSeqNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ short nNozzle,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RollingUpdate( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TransactionStatusCM( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nIndex,
            /* [in] */ long lStatus,
            /* [in] */ long lOnOffer,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TransactionStatusCS( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ short nNozzle,
            /* [in] */ long lStatus,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoAutomaticPAKReservation( 
            /* [out] */ BOOL *bAutoRes,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurrentTransactionNumber( 
            /* [in] */ long lPumpNumber,
            /* [out] */ long *pTrsNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PAPSwipe( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDeviceNumber,
            /* [out][in] */ long *lSeqNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PumpTotals( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ short nFlags,
            /* [in] */ long lVolumeH,
            /* [in] */ long lVolumeL,
            /* [in] */ long lValueAH,
            /* [in] */ long lValueAL,
            /* [in] */ long lValueBH,
            /* [in] */ long lValueBL,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PumpMode( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lPumpMode,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GradePrice( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ short nFlags,
            /* [in] */ long lPriceA,
            /* [in] */ long lPriceB,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TankReading( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lProduct,
            /* [in] */ short nFlags,
            /* [in] */ long lStatus,
            /* [in] */ long lFuelLevel,
            /* [in] */ long lWaterLevel,
            /* [in] */ long lTemperature,
            /* [in] */ long lVolume,
            /* [in] */ long lWaterVolume,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParam( 
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRealTimeClock( 
            /* [out] */ BSTR *psDateTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeliveryReport( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lProduct,
            /* [in] */ short nFlags,
            /* [in] */ long lStatus,
            /* [in] */ long lTag,
            /* [in] */ long lVolume,
            /* [in] */ BSTR sDateAndTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGradePrice( 
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TankStatus( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lStatus,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitInterface2( 
            /* [in] */ BSTR strControllerName,
            /* [in] */ long lControllerVer,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Watchdog( 
            /* [in] */ long lParam,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetParam( 
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AVIStart( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lNozzleNumber,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AVIStop( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lNozzleNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPAPSwipeData( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TankReading2( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lProduct,
            /* [in] */ short nFlags,
            /* [in] */ long lStatus1,
            /* [in] */ long lFuelLevel,
            /* [in] */ long lWaterLevel,
            /* [in] */ long lTemperature,
            /* [in] */ long lVolume,
            /* [in] */ long lWaterVolume,
            /* [in] */ long lStatus2,
            /* [in] */ long lFuelValue,
            /* [in] */ long lPricePerLitre,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PumpTotals2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ short nFlags,
            /* [in] */ long lVolumeH,
            /* [in] */ long lVolumeL,
            /* [in] */ long lValueAH,
            /* [in] */ long lValueAL,
            /* [in] */ long lValueBH,
            /* [in] */ long lValueBL,
            /* [in] */ long lPumpStatus,
            /* [in] */ long lNozzleStatus,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeliveryReport2( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lProduct,
            /* [in] */ short nFlags,
            /* [in] */ long lStatus,
            /* [in] */ long lTag,
            /* [in] */ long lVolume,
            /* [in] */ BSTR sStartDateAndTime,
            /* [in] */ BSTR sEndDateAndTime,
            /* [in] */ long lStartVolume,
            /* [in] */ long lEndVolume,
            /* [in] */ long lTemperature,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Alarm( 
            /* [in] */ long lCatagory,
            /* [in] */ long lCode,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sStartDateAndTime,
            /* [in] */ BSTR sEndDateAndTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CarWashStatus( 
            /* [in] */ long lDevice,
            /* [in] */ long lSrc,
            /* [in] */ long lStatus,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WashCodeResult( 
            /* [in] */ long lSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lResult,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPAPSwipeData( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParam3( 
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetParam3( 
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReceiptRowData( 
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sTrack2,
            /* [in] */ short nIndex,
            /* [out] */ BSTR *sReceiptRowData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCardInfo( 
            /* [in] */ BSTR sAccountNumber,
            /* [in] */ BSTR sTrack1,
            /* [in] */ BSTR sTrack2,
            /* [in] */ BSTR sShortCardName,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReceiptCounter( 
            /* [out] */ long *plConuter,
            /* [in] */ long iType,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetServiceFee( 
            /* [in] */ long lPump,
            /* [in] */ long lTrsType,
            /* [in] */ long lTrsNumber,
            /* [in] */ BSTR stTime,
            /* [in] */ long lFlags,
            /* [out] */ long *pValue,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReportRequest( 
            /* [in] */ long lParam1,
            /* [in] */ long lParam2,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [out] */ long *lRefNum,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTrsState( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReceiptPrinted( 
            /* [in] */ BSTR sTrack2,
            /* [in] */ long lIndex,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReceiptRawData2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sTrack2,
            /* [out][in] */ short *nIndex,
            /* [out] */ BSTR *sReceiptRawData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpInfo( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSingleProductInfo( 
            /* [in] */ long lPump,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lGrade,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DebitProcessResultCmd( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSessionId,
            /* [in] */ long lFlag,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDebitDataInfo( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReceiptRawData3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sTrack2,
            /* [out][in] */ short *nIndex,
            /* [out] */ BSTR *sReceiptRawData,
            /* [out][in] */ long *plFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDebitData( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TankReading3( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lProduct,
            /* [in] */ short nFlags,
            /* [in] */ long lStatus1,
            /* [in] */ long lFuelLevel,
            /* [in] */ long lWaterLevel,
            /* [in] */ long lTemperature,
            /* [in] */ long lVolume,
            /* [in] */ long lWaterVolume,
            /* [in] */ long lStatus2,
            /* [in] */ long lFuelValue,
            /* [in] */ long lPricePerLitre,
            /* [in] */ long lTcVolume,
            /* [in] */ long lUllage,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAlarm( 
            /* [in] */ long lCatagory,
            /* [in] */ long lSubCatagory,
            /* [in] */ long lAlarmNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lAlarmStatus,
            /* [in] */ long lAlarmValue,
            long lFlags,
            /* [out] */ long *pAlarmId,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCashAcceptorData( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lSeqNum,
            /* [in] */ long lFlags,
            /* [in] */ BSTR CashSaleData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCashAcceptorData( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lSeqNum,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *CashSaleData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DisplayInfo( 
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sMsg,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryData( 
            /* [in] */ long lFileNum,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ BSTR sKey,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetShift( 
            /* [in] */ short nDayOfWeek,
            /* [in] */ short nShiftNumber,
            /* [in] */ short nShiftFlags,
            /* [in] */ BSTR sStartTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOLABuffer( 
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sCardData,
            /* [out] */ BSTR *sOLABuffer,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMaintenance( 
            /* [in] */ long lLock,
            /* [in] */ long lParam,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCardInfo2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDeviceNumber,
            /* [out][in] */ long *lSeqNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveTerminalTransactionToPump( 
            /* [in] */ long lTerminalId,
            /* [in] */ long lPumpId,
            /* [in] */ long lStatus,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PAKReadyTransaction2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ short nNozzle,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [in] */ long lFlage,
            /* [in] */ long lTotalVal,
            /* [in] */ BSTR pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpMode( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lPumpMode,
            long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCardInfo3( 
            /* [in] */ BSTR sAccountNumber,
            /* [in] */ BSTR sTrack1,
            /* [in] */ BSTR sTrack2,
            /* [in] */ BSTR sShortCardName,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lSesionType,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetProcess( 
            /* [in] */ long lProcessNumber,
            /* [in] */ long lParam1,
            /* [in] */ long lParam2,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetConnectivityStatus( 
            /* [in] */ BSTR sPSConnStatus,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPAKQuery( 
            /* [in] */ BSTR sRestrictionData,
            /* [out] */ BSTR *sOutData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpMode( 
            /* [in] */ long lPumpNumber,
            /* [out] */ long *plPumpMode,
            /* [out] */ long *plFlags,
            /* [out] */ long *plMsgNumber,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ParseOLABuffer( 
            /* [in] */ long lPumpNumber,
            /* [out][in] */ BSTR *cCardSaleALL3,
            /* [in] */ BSTR sOLABuffer,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetVersion( 
            /* [in] */ BSTR sVersionList,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetStationStatus( 
            /* [out][in] */ long *lPumpMap1,
            /* [out][in] */ long *lPumpMap2,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UploadBuffer( 
            /* [in] */ long lTerminalId,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSessionId,
            /* [in] */ long lUploadType,
            /* [in] */ BSTR sInBuffer,
            /* [out] */ BSTR *sOutBuffer,
            /* [in] */ long lExDataSize,
            /* [in] */ BSTR sExData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWetStockData( 
            /* [in] */ long lWetStockFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTankMemoryData( 
            /* [in] */ long lTankNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lCounter,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParam4( 
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [in] */ long lFlags,
            /* [in] */ long lDataBase,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetParam4( 
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [in] */ long lFlags,
            /* [in] */ long lDataBase,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAttTrs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sIndex,
            /* [out] */ BSTR *sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PriceChangeUpdate( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lPosition,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetGradesRestriction( 
            /* [in] */ BSTR sTagPumpsNuzzlesRestrict,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTransListQuery( 
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long *lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCashAcceptorStatus( 
            /* [in] */ long lTerminalId,
            /* [in] */ long lFlags,
            /* [in] */ long lStatus,
            /* [out] */ BSTR *sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCashAcceptorReport( 
            /* [in] */ long lTerminalId,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sHeader,
            /* [out] */ BSTR *sRecords,
            /* [out] */ long *lRecordCount,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCardInfoEx( 
            /* [in] */ long lClientId,
            /* [in] */ BSTR sInBuffer,
            /* [in] */ BSTR sInfoType,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TankReadingEx( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeliveryReportEx( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadyOfflineTRS( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [in] */ BSTR sParam1,
            /* [in] */ long lParam2,
            /* [in] */ long lParam3,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpStatus2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lStatus,
            /* [in] */ long lRes,
            /* [in] */ long lResExt,
            /* [in] */ long lFlags,
            /* [in] */ short nGrade,
            /* [in] */ short nPosition,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpMisc( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMisc,
            /* [in] */ long bRemoveMisc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpMiscExt( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMisc,
            /* [in] */ long bRemoveMisc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReportReady( 
            /* [in] */ long lFlags,
            /* [in] */ BSTR wszReportText,
            /* [in] */ long lParam,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CarWashGeneralReqResponse( 
            /* [in] */ long lDevice,
            /* [in] */ long lResult,
            /* [in] */ long lFlags1,
            /* [in] */ long lFlags2,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PriceDisplayPanelStatus( 
            /* [in] */ long lDevice,
            /* [in] */ long lStatus,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IGCIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGCI * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGCI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGCI * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGCI * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGCI * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGCI * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGCI * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpStatus )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lStatus,
            /* [in] */ long lRes,
            /* [in] */ long lResExt,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteToLog )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ BOOL bErrorRep,
            /* [in] */ long lError,
            /* [in] */ BOOL bWarningRep,
            /* [in] */ long lWarning,
            /* [in] */ BOOL bDeviceRep,
            /* [in] */ long lDeviceCode,
            /* [in] */ long lDevice,
            /* [in] */ BOOL bMiscRep,
            /* [in] */ long lMisc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitInterface )( 
            IGCI * This,
            /* [in] */ BOOL bControllerIsMaster,
            /* [in] */ BOOL bControllerGenTrsNumber,
            /* [in] */ BSTR strControllerName,
            /* [in] */ long lControllerVer,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CancelTransaction )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PAKReadyTransaction )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ short nNozzle,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PAPReadyTransaction )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSeqNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ short nNozzle,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RollingUpdate )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TransactionStatusCM )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nIndex,
            /* [in] */ long lStatus,
            /* [in] */ long lOnOffer,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TransactionStatusCS )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ short nNozzle,
            /* [in] */ long lStatus,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DoAutomaticPAKReservation )( 
            IGCI * This,
            /* [out] */ BOOL *bAutoRes,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCurrentTransactionNumber )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ long *pTrsNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PAPSwipe )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDeviceNumber,
            /* [out][in] */ long *lSeqNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PumpTotals )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ short nFlags,
            /* [in] */ long lVolumeH,
            /* [in] */ long lVolumeL,
            /* [in] */ long lValueAH,
            /* [in] */ long lValueAL,
            /* [in] */ long lValueBH,
            /* [in] */ long lValueBL,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PumpMode )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lPumpMode,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GradePrice )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ short nFlags,
            /* [in] */ long lPriceA,
            /* [in] */ long lPriceB,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TankReading )( 
            IGCI * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lProduct,
            /* [in] */ short nFlags,
            /* [in] */ long lStatus,
            /* [in] */ long lFuelLevel,
            /* [in] */ long lWaterLevel,
            /* [in] */ long lTemperature,
            /* [in] */ long lVolume,
            /* [in] */ long lWaterVolume,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParam )( 
            IGCI * This,
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRealTimeClock )( 
            IGCI * This,
            /* [out] */ BSTR *psDateTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeliveryReport )( 
            IGCI * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lProduct,
            /* [in] */ short nFlags,
            /* [in] */ long lStatus,
            /* [in] */ long lTag,
            /* [in] */ long lVolume,
            /* [in] */ BSTR sDateAndTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetGradePrice )( 
            IGCI * This,
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TankStatus )( 
            IGCI * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lStatus,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitInterface2 )( 
            IGCI * This,
            /* [in] */ BSTR strControllerName,
            /* [in] */ long lControllerVer,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Watchdog )( 
            IGCI * This,
            /* [in] */ long lParam,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetParam )( 
            IGCI * This,
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AVIStart )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lNozzleNumber,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AVIStop )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lNozzleNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPAPSwipeData )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TankReading2 )( 
            IGCI * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lProduct,
            /* [in] */ short nFlags,
            /* [in] */ long lStatus1,
            /* [in] */ long lFuelLevel,
            /* [in] */ long lWaterLevel,
            /* [in] */ long lTemperature,
            /* [in] */ long lVolume,
            /* [in] */ long lWaterVolume,
            /* [in] */ long lStatus2,
            /* [in] */ long lFuelValue,
            /* [in] */ long lPricePerLitre,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PumpTotals2 )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ short nFlags,
            /* [in] */ long lVolumeH,
            /* [in] */ long lVolumeL,
            /* [in] */ long lValueAH,
            /* [in] */ long lValueAL,
            /* [in] */ long lValueBH,
            /* [in] */ long lValueBL,
            /* [in] */ long lPumpStatus,
            /* [in] */ long lNozzleStatus,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeliveryReport2 )( 
            IGCI * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lProduct,
            /* [in] */ short nFlags,
            /* [in] */ long lStatus,
            /* [in] */ long lTag,
            /* [in] */ long lVolume,
            /* [in] */ BSTR sStartDateAndTime,
            /* [in] */ BSTR sEndDateAndTime,
            /* [in] */ long lStartVolume,
            /* [in] */ long lEndVolume,
            /* [in] */ long lTemperature,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Alarm )( 
            IGCI * This,
            /* [in] */ long lCatagory,
            /* [in] */ long lCode,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sStartDateAndTime,
            /* [in] */ BSTR sEndDateAndTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CarWashStatus )( 
            IGCI * This,
            /* [in] */ long lDevice,
            /* [in] */ long lSrc,
            /* [in] */ long lStatus,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WashCodeResult )( 
            IGCI * This,
            /* [in] */ long lSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lResult,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPAPSwipeData )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParam3 )( 
            IGCI * This,
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetParam3 )( 
            IGCI * This,
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReceiptRowData )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sTrack2,
            /* [in] */ short nIndex,
            /* [out] */ BSTR *sReceiptRowData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCardInfo )( 
            IGCI * This,
            /* [in] */ BSTR sAccountNumber,
            /* [in] */ BSTR sTrack1,
            /* [in] */ BSTR sTrack2,
            /* [in] */ BSTR sShortCardName,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReceiptCounter )( 
            IGCI * This,
            /* [out] */ long *plConuter,
            /* [in] */ long iType,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetServiceFee )( 
            IGCI * This,
            /* [in] */ long lPump,
            /* [in] */ long lTrsType,
            /* [in] */ long lTrsNumber,
            /* [in] */ BSTR stTime,
            /* [in] */ long lFlags,
            /* [out] */ long *pValue,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReportRequest )( 
            IGCI * This,
            /* [in] */ long lParam1,
            /* [in] */ long lParam2,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [out] */ long *lRefNum,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTrsState )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReceiptPrinted )( 
            IGCI * This,
            /* [in] */ BSTR sTrack2,
            /* [in] */ long lIndex,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReceiptRawData2 )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sTrack2,
            /* [out][in] */ short *nIndex,
            /* [out] */ BSTR *sReceiptRawData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpInfo )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSingleProductInfo )( 
            IGCI * This,
            /* [in] */ long lPump,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lGrade,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DebitProcessResultCmd )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSessionId,
            /* [in] */ long lFlag,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDebitDataInfo )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReceiptRawData3 )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sTrack2,
            /* [out][in] */ short *nIndex,
            /* [out] */ BSTR *sReceiptRawData,
            /* [out][in] */ long *plFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDebitData )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TankReading3 )( 
            IGCI * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lProduct,
            /* [in] */ short nFlags,
            /* [in] */ long lStatus1,
            /* [in] */ long lFuelLevel,
            /* [in] */ long lWaterLevel,
            /* [in] */ long lTemperature,
            /* [in] */ long lVolume,
            /* [in] */ long lWaterVolume,
            /* [in] */ long lStatus2,
            /* [in] */ long lFuelValue,
            /* [in] */ long lPricePerLitre,
            /* [in] */ long lTcVolume,
            /* [in] */ long lUllage,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAlarm )( 
            IGCI * This,
            /* [in] */ long lCatagory,
            /* [in] */ long lSubCatagory,
            /* [in] */ long lAlarmNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lAlarmStatus,
            /* [in] */ long lAlarmValue,
            long lFlags,
            /* [out] */ long *pAlarmId,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCashAcceptorData )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lSeqNum,
            /* [in] */ long lFlags,
            /* [in] */ BSTR CashSaleData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCashAcceptorData )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lSeqNum,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *CashSaleData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DisplayInfo )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sMsg,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryData )( 
            IGCI * This,
            /* [in] */ long lFileNum,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ BSTR sKey,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetShift )( 
            IGCI * This,
            /* [in] */ short nDayOfWeek,
            /* [in] */ short nShiftNumber,
            /* [in] */ short nShiftFlags,
            /* [in] */ BSTR sStartTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOLABuffer )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ BSTR sCardData,
            /* [out] */ BSTR *sOLABuffer,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMaintenance )( 
            IGCI * This,
            /* [in] */ long lLock,
            /* [in] */ long lParam,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCardInfo2 )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDeviceNumber,
            /* [out][in] */ long *lSeqNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveTerminalTransactionToPump )( 
            IGCI * This,
            /* [in] */ long lTerminalId,
            /* [in] */ long lPumpId,
            /* [in] */ long lStatus,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PAKReadyTransaction2 )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTrsNumber,
            /* [in] */ short nNozzle,
            /* [in] */ long lVolume,
            /* [in] */ long lValue,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ short nGrade,
            /* [in] */ long lFlage,
            /* [in] */ long lTotalVal,
            /* [in] */ BSTR pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpMode )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lPumpMode,
            long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCardInfo3 )( 
            IGCI * This,
            /* [in] */ BSTR sAccountNumber,
            /* [in] */ BSTR sTrack1,
            /* [in] */ BSTR sTrack2,
            /* [in] */ BSTR sShortCardName,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lSesionType,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetProcess )( 
            IGCI * This,
            /* [in] */ long lProcessNumber,
            /* [in] */ long lParam1,
            /* [in] */ long lParam2,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetConnectivityStatus )( 
            IGCI * This,
            /* [in] */ BSTR sPSConnStatus,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPAKQuery )( 
            IGCI * This,
            /* [in] */ BSTR sRestrictionData,
            /* [out] */ BSTR *sOutData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpMode )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ long *plPumpMode,
            /* [out] */ long *plFlags,
            /* [out] */ long *plMsgNumber,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ParseOLABuffer )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [out][in] */ BSTR *cCardSaleALL3,
            /* [in] */ BSTR sOLABuffer,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetVersion )( 
            IGCI * This,
            /* [in] */ BSTR sVersionList,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetStationStatus )( 
            IGCI * This,
            /* [out][in] */ long *lPumpMap1,
            /* [out][in] */ long *lPumpMap2,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UploadBuffer )( 
            IGCI * This,
            /* [in] */ long lTerminalId,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSessionId,
            /* [in] */ long lUploadType,
            /* [in] */ BSTR sInBuffer,
            /* [out] */ BSTR *sOutBuffer,
            /* [in] */ long lExDataSize,
            /* [in] */ BSTR sExData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWetStockData )( 
            IGCI * This,
            /* [in] */ long lWetStockFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTankMemoryData )( 
            IGCI * This,
            /* [in] */ long lTankNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lCounter,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParam4 )( 
            IGCI * This,
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [in] */ long lFlags,
            /* [in] */ long lDataBase,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetParam4 )( 
            IGCI * This,
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [in] */ long lFlags,
            /* [in] */ long lDataBase,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAttTrs )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sIndex,
            /* [out] */ BSTR *sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PriceChangeUpdate )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lPosition,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetGradesRestriction )( 
            IGCI * This,
            /* [in] */ BSTR sTagPumpsNuzzlesRestrict,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTransListQuery )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ BSTR *sData,
            /* [out][in] */ long *plRecCount,
            /* [in] */ long *lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCashAcceptorStatus )( 
            IGCI * This,
            /* [in] */ long lTerminalId,
            /* [in] */ long lFlags,
            /* [in] */ long lStatus,
            /* [out] */ BSTR *sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCashAcceptorReport )( 
            IGCI * This,
            /* [in] */ long lTerminalId,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sHeader,
            /* [out] */ BSTR *sRecords,
            /* [out] */ long *lRecordCount,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCardInfoEx )( 
            IGCI * This,
            /* [in] */ long lClientId,
            /* [in] */ BSTR sInBuffer,
            /* [in] */ BSTR sInfoType,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TankReadingEx )( 
            IGCI * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeliveryReportEx )( 
            IGCI * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadyOfflineTRS )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [in] */ BSTR sParam1,
            /* [in] */ long lParam2,
            /* [in] */ long lParam3,
            /* [out] */ BSTR *sRetData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpStatus2 )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lStatus,
            /* [in] */ long lRes,
            /* [in] */ long lResExt,
            /* [in] */ long lFlags,
            /* [in] */ short nGrade,
            /* [in] */ short nPosition,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpMisc )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMisc,
            /* [in] */ long bRemoveMisc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpMiscExt )( 
            IGCI * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMisc,
            /* [in] */ long bRemoveMisc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReportReady )( 
            IGCI * This,
            /* [in] */ long lFlags,
            /* [in] */ BSTR wszReportText,
            /* [in] */ long lParam,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CarWashGeneralReqResponse )( 
            IGCI * This,
            /* [in] */ long lDevice,
            /* [in] */ long lResult,
            /* [in] */ long lFlags1,
            /* [in] */ long lFlags2,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PriceDisplayPanelStatus )( 
            IGCI * This,
            /* [in] */ long lDevice,
            /* [in] */ long lStatus,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        END_INTERFACE
    } IGCIVtbl;

    interface IGCI
    {
        CONST_VTBL struct IGCIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGCI_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGCI_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGCI_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGCI_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IGCI_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IGCI_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IGCI_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IGCI_SetPumpStatus(This,lPumpNumber,lStatus,lRes,lResExt,pVal)	\
    ( (This)->lpVtbl -> SetPumpStatus(This,lPumpNumber,lStatus,lRes,lResExt,pVal) ) 

#define IGCI_WriteToLog(This,lPumpNumber,bErrorRep,lError,bWarningRep,lWarning,bDeviceRep,lDeviceCode,lDevice,bMiscRep,lMisc,pVal)	\
    ( (This)->lpVtbl -> WriteToLog(This,lPumpNumber,bErrorRep,lError,bWarningRep,lWarning,bDeviceRep,lDeviceCode,lDevice,bMiscRep,lMisc,pVal) ) 

#define IGCI_InitInterface(This,bControllerIsMaster,bControllerGenTrsNumber,strControllerName,lControllerVer,pVal)	\
    ( (This)->lpVtbl -> InitInterface(This,bControllerIsMaster,bControllerGenTrsNumber,strControllerName,lControllerVer,pVal) ) 

#define IGCI_CancelTransaction(This,lPumpNumber,lTrsNumber,pVal)	\
    ( (This)->lpVtbl -> CancelTransaction(This,lPumpNumber,lTrsNumber,pVal) ) 

#define IGCI_PAKReadyTransaction(This,lPumpNumber,lTrsNumber,nNozzle,lVolume,lValue,nLevel,lGradePrice,nGrade,pVal)	\
    ( (This)->lpVtbl -> PAKReadyTransaction(This,lPumpNumber,lTrsNumber,nNozzle,lVolume,lValue,nLevel,lGradePrice,nGrade,pVal) ) 

#define IGCI_PAPReadyTransaction(This,lPumpNumber,lSeqNumber,lTrsNumber,nNozzle,lVolume,lValue,nLevel,lGradePrice,nGrade,lFlags,sData,sRetData,pVal)	\
    ( (This)->lpVtbl -> PAPReadyTransaction(This,lPumpNumber,lSeqNumber,lTrsNumber,nNozzle,lVolume,lValue,nLevel,lGradePrice,nGrade,lFlags,sData,sRetData,pVal) ) 

#define IGCI_RollingUpdate(This,lPumpNumber,lTrsNumber,lVolume,lValue,lGradePrice,nGrade,pVal)	\
    ( (This)->lpVtbl -> RollingUpdate(This,lPumpNumber,lTrsNumber,lVolume,lValue,lGradePrice,nGrade,pVal) ) 

#define IGCI_TransactionStatusCM(This,lPumpNumber,lTrsNumber,nNozzle,nIndex,lStatus,lOnOffer,lVolume,lValue,nLevel,lGradePrice,nGrade,pVal)	\
    ( (This)->lpVtbl -> TransactionStatusCM(This,lPumpNumber,lTrsNumber,nNozzle,nIndex,lStatus,lOnOffer,lVolume,lValue,nLevel,lGradePrice,nGrade,pVal) ) 

#define IGCI_TransactionStatusCS(This,lPumpNumber,lTrsNumber,nNozzle,lStatus,lVolume,lValue,nLevel,lGradePrice,nGrade,pVal)	\
    ( (This)->lpVtbl -> TransactionStatusCS(This,lPumpNumber,lTrsNumber,nNozzle,lStatus,lVolume,lValue,nLevel,lGradePrice,nGrade,pVal) ) 

#define IGCI_DoAutomaticPAKReservation(This,bAutoRes,pVal)	\
    ( (This)->lpVtbl -> DoAutomaticPAKReservation(This,bAutoRes,pVal) ) 

#define IGCI_GetCurrentTransactionNumber(This,lPumpNumber,pTrsNumber,pVal)	\
    ( (This)->lpVtbl -> GetCurrentTransactionNumber(This,lPumpNumber,pTrsNumber,pVal) ) 

#define IGCI_PAPSwipe(This,lPumpNumber,lDeviceNumber,lSeqNumber,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> PAPSwipe(This,lPumpNumber,lDeviceNumber,lSeqNumber,lFlags,sData,pVal) ) 

#define IGCI_PumpTotals(This,lPumpNumber,nNozzle,nGrade,nFlags,lVolumeH,lVolumeL,lValueAH,lValueAL,lValueBH,lValueBL,pVal)	\
    ( (This)->lpVtbl -> PumpTotals(This,lPumpNumber,nNozzle,nGrade,nFlags,lVolumeH,lVolumeL,lValueAH,lValueAL,lValueBH,lValueBL,pVal) ) 

#define IGCI_PumpMode(This,lPumpNumber,lPumpMode,pVal)	\
    ( (This)->lpVtbl -> PumpMode(This,lPumpNumber,lPumpMode,pVal) ) 

#define IGCI_GradePrice(This,lPumpNumber,nNozzle,nGrade,nFlags,lPriceA,lPriceB,pVal)	\
    ( (This)->lpVtbl -> GradePrice(This,lPumpNumber,nNozzle,nGrade,nFlags,lPriceA,lPriceB,pVal) ) 

#define IGCI_TankReading(This,lTankNumber,lProduct,nFlags,lStatus,lFuelLevel,lWaterLevel,lTemperature,lVolume,lWaterVolume,pVal)	\
    ( (This)->lpVtbl -> TankReading(This,lTankNumber,lProduct,nFlags,lStatus,lFuelLevel,lWaterLevel,lTemperature,lVolume,lWaterVolume,pVal) ) 

#define IGCI_GetParam(This,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal)	\
    ( (This)->lpVtbl -> GetParam(This,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal) ) 

#define IGCI_GetRealTimeClock(This,psDateTime,pVal)	\
    ( (This)->lpVtbl -> GetRealTimeClock(This,psDateTime,pVal) ) 

#define IGCI_DeliveryReport(This,lTankNumber,lProduct,nFlags,lStatus,lTag,lVolume,sDateAndTime,pVal)	\
    ( (This)->lpVtbl -> DeliveryReport(This,lTankNumber,lProduct,nFlags,lStatus,lTag,lVolume,sDateAndTime,pVal) ) 

#define IGCI_GetGradePrice(This,nGrade,pVal)	\
    ( (This)->lpVtbl -> GetGradePrice(This,nGrade,pVal) ) 

#define IGCI_TankStatus(This,lTankNumber,lStatus,pVal)	\
    ( (This)->lpVtbl -> TankStatus(This,lTankNumber,lStatus,pVal) ) 

#define IGCI_InitInterface2(This,strControllerName,lControllerVer,lFlags,pVal)	\
    ( (This)->lpVtbl -> InitInterface2(This,strControllerName,lControllerVer,lFlags,pVal) ) 

#define IGCI_Watchdog(This,lParam,pVal)	\
    ( (This)->lpVtbl -> Watchdog(This,lParam,pVal) ) 

#define IGCI_SetParam(This,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal)	\
    ( (This)->lpVtbl -> SetParam(This,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal) ) 

#define IGCI_AVIStart(This,lPumpNumber,lNozzleNumber,sData,pVal)	\
    ( (This)->lpVtbl -> AVIStart(This,lPumpNumber,lNozzleNumber,sData,pVal) ) 

#define IGCI_AVIStop(This,lPumpNumber,lNozzleNumber,pVal)	\
    ( (This)->lpVtbl -> AVIStop(This,lPumpNumber,lNozzleNumber,pVal) ) 

#define IGCI_GetPAPSwipeData(This,lPumpNumber,lFlags,pData,pVal)	\
    ( (This)->lpVtbl -> GetPAPSwipeData(This,lPumpNumber,lFlags,pData,pVal) ) 

#define IGCI_TankReading2(This,lTankNumber,lProduct,nFlags,lStatus1,lFuelLevel,lWaterLevel,lTemperature,lVolume,lWaterVolume,lStatus2,lFuelValue,lPricePerLitre,pVal)	\
    ( (This)->lpVtbl -> TankReading2(This,lTankNumber,lProduct,nFlags,lStatus1,lFuelLevel,lWaterLevel,lTemperature,lVolume,lWaterVolume,lStatus2,lFuelValue,lPricePerLitre,pVal) ) 

#define IGCI_PumpTotals2(This,lPumpNumber,nNozzle,nGrade,nFlags,lVolumeH,lVolumeL,lValueAH,lValueAL,lValueBH,lValueBL,lPumpStatus,lNozzleStatus,pVal)	\
    ( (This)->lpVtbl -> PumpTotals2(This,lPumpNumber,nNozzle,nGrade,nFlags,lVolumeH,lVolumeL,lValueAH,lValueAL,lValueBH,lValueBL,lPumpStatus,lNozzleStatus,pVal) ) 

#define IGCI_DeliveryReport2(This,lTankNumber,lProduct,nFlags,lStatus,lTag,lVolume,sStartDateAndTime,sEndDateAndTime,lStartVolume,lEndVolume,lTemperature,pVal)	\
    ( (This)->lpVtbl -> DeliveryReport2(This,lTankNumber,lProduct,nFlags,lStatus,lTag,lVolume,sStartDateAndTime,sEndDateAndTime,lStartVolume,lEndVolume,lTemperature,pVal) ) 

#define IGCI_Alarm(This,lCatagory,lCode,lFlags,sStartDateAndTime,sEndDateAndTime,pVal)	\
    ( (This)->lpVtbl -> Alarm(This,lCatagory,lCode,lFlags,sStartDateAndTime,sEndDateAndTime,pVal) ) 

#define IGCI_CarWashStatus(This,lDevice,lSrc,lStatus,pVal)	\
    ( (This)->lpVtbl -> CarWashStatus(This,lDevice,lSrc,lStatus,pVal) ) 

#define IGCI_WashCodeResult(This,lSyncNumber,lDevice,lResult,sData,pVal)	\
    ( (This)->lpVtbl -> WashCodeResult(This,lSyncNumber,lDevice,lResult,sData,pVal) ) 

#define IGCI_SetPAPSwipeData(This,lPumpNumber,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> SetPAPSwipeData(This,lPumpNumber,lFlags,sData,pVal) ) 

#define IGCI_GetParam3(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal)	\
    ( (This)->lpVtbl -> GetParam3(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal) ) 

#define IGCI_SetParam3(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal)	\
    ( (This)->lpVtbl -> SetParam3(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal) ) 

#define IGCI_GetReceiptRowData(This,lPumpNumber,sTrack2,nIndex,sReceiptRowData,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetReceiptRowData(This,lPumpNumber,sTrack2,nIndex,sReceiptRowData,lFlags,pVal) ) 

#define IGCI_GetCardInfo(This,sAccountNumber,sTrack1,sTrack2,sShortCardName,sRetData,pVal)	\
    ( (This)->lpVtbl -> GetCardInfo(This,sAccountNumber,sTrack1,sTrack2,sShortCardName,sRetData,pVal) ) 

#define IGCI_GetReceiptCounter(This,plConuter,iType,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetReceiptCounter(This,plConuter,iType,lFlags,pVal) ) 

#define IGCI_GetServiceFee(This,lPump,lTrsType,lTrsNumber,stTime,lFlags,pValue,pRetVal)	\
    ( (This)->lpVtbl -> GetServiceFee(This,lPump,lTrsType,lTrsNumber,stTime,lFlags,pValue,pRetVal) ) 

#define IGCI_ReportRequest(This,lParam1,lParam2,lFlags,sData,lRefNum,pVal)	\
    ( (This)->lpVtbl -> ReportRequest(This,lParam1,lParam2,lFlags,sData,lRefNum,pVal) ) 

#define IGCI_SetTrsState(This,lPumpNumber,lTrsNumber,lState,lFlags,sData,plRetVal)	\
    ( (This)->lpVtbl -> SetTrsState(This,lPumpNumber,lTrsNumber,lState,lFlags,sData,plRetVal) ) 

#define IGCI_ReceiptPrinted(This,sTrack2,lIndex,lFlags,pVal)	\
    ( (This)->lpVtbl -> ReceiptPrinted(This,sTrack2,lIndex,lFlags,pVal) ) 

#define IGCI_GetReceiptRawData2(This,lPumpNumber,sTrack2,nIndex,sReceiptRawData,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetReceiptRawData2(This,lPumpNumber,sTrack2,nIndex,sReceiptRawData,lFlags,pVal) ) 

#define IGCI_GetPumpInfo(This,lPumpNumber,pRec,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetPumpInfo(This,lPumpNumber,pRec,lFlags,pVal) ) 

#define IGCI_GetSingleProductInfo(This,lPump,lFlags,lGrade,pVal)	\
    ( (This)->lpVtbl -> GetSingleProductInfo(This,lPump,lFlags,lGrade,pVal) ) 

#define IGCI_DebitProcessResultCmd(This,lPumpNumber,lSessionId,lFlag,sData,pVal)	\
    ( (This)->lpVtbl -> DebitProcessResultCmd(This,lPumpNumber,lSessionId,lFlag,sData,pVal) ) 

#define IGCI_GetDebitDataInfo(This,lPumpNumber,lFlags,pData,pVal)	\
    ( (This)->lpVtbl -> GetDebitDataInfo(This,lPumpNumber,lFlags,pData,pVal) ) 

#define IGCI_GetReceiptRawData3(This,lPumpNumber,sTrack2,nIndex,sReceiptRawData,plFlags,pVal)	\
    ( (This)->lpVtbl -> GetReceiptRawData3(This,lPumpNumber,sTrack2,nIndex,sReceiptRawData,plFlags,pVal) ) 

#define IGCI_SetDebitData(This,lPumpNumber,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> SetDebitData(This,lPumpNumber,lFlags,sData,pVal) ) 

#define IGCI_TankReading3(This,lTankNumber,lProduct,nFlags,lStatus1,lFuelLevel,lWaterLevel,lTemperature,lVolume,lWaterVolume,lStatus2,lFuelValue,lPricePerLitre,lTcVolume,lUllage,sRec,pVal)	\
    ( (This)->lpVtbl -> TankReading3(This,lTankNumber,lProduct,nFlags,lStatus1,lFuelLevel,lWaterLevel,lTemperature,lVolume,lWaterVolume,lStatus2,lFuelValue,lPricePerLitre,lTcVolume,lUllage,sRec,pVal) ) 

#define IGCI_SetAlarm(This,lCatagory,lSubCatagory,lAlarmNumber,lDevice,lAlarmStatus,lAlarmValue,lFlags,pAlarmId,sRec,pVal)	\
    ( (This)->lpVtbl -> SetAlarm(This,lCatagory,lSubCatagory,lAlarmNumber,lDevice,lAlarmStatus,lAlarmValue,lFlags,pAlarmId,sRec,pVal) ) 

#define IGCI_SetCashAcceptorData(This,lPumpNumber,lDevice,lSeqNum,lFlags,CashSaleData,pVal)	\
    ( (This)->lpVtbl -> SetCashAcceptorData(This,lPumpNumber,lDevice,lSeqNum,lFlags,CashSaleData,pVal) ) 

#define IGCI_GetCashAcceptorData(This,lPumpNumber,lDevice,lSeqNum,lFlags,CashSaleData,pVal)	\
    ( (This)->lpVtbl -> GetCashAcceptorData(This,lPumpNumber,lDevice,lSeqNum,lFlags,CashSaleData,pVal) ) 

#define IGCI_DisplayInfo(This,lPumpNumber,sMsg,lFlags,pRetVal)	\
    ( (This)->lpVtbl -> DisplayInfo(This,lPumpNumber,sMsg,lFlags,pRetVal) ) 

#define IGCI_QueryData(This,lFileNum,lIndexNumber,sKey,lFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> QueryData(This,lFileNum,lIndexNumber,sKey,lFlags,pRec,pVal) ) 

#define IGCI_SetShift(This,nDayOfWeek,nShiftNumber,nShiftFlags,sStartTime,pVal)	\
    ( (This)->lpVtbl -> SetShift(This,nDayOfWeek,nShiftNumber,nShiftFlags,sStartTime,pVal) ) 

#define IGCI_GetOLABuffer(This,lPumpNumber,sCardData,sOLABuffer,lFlags,pRet)	\
    ( (This)->lpVtbl -> GetOLABuffer(This,lPumpNumber,sCardData,sOLABuffer,lFlags,pRet) ) 

#define IGCI_SetMaintenance(This,lLock,lParam,pVal)	\
    ( (This)->lpVtbl -> SetMaintenance(This,lLock,lParam,pVal) ) 

#define IGCI_GetCardInfo2(This,lPumpNumber,lDeviceNumber,lSeqNumber,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> GetCardInfo2(This,lPumpNumber,lDeviceNumber,lSeqNumber,lFlags,sData,pVal) ) 

#define IGCI_MoveTerminalTransactionToPump(This,lTerminalId,lPumpId,lStatus,lFlags,pVal)	\
    ( (This)->lpVtbl -> MoveTerminalTransactionToPump(This,lTerminalId,lPumpId,lStatus,lFlags,pVal) ) 

#define IGCI_PAKReadyTransaction2(This,lPumpNumber,lTrsNumber,nNozzle,lVolume,lValue,nLevel,lGradePrice,nGrade,lFlage,lTotalVal,pRec,pVal)	\
    ( (This)->lpVtbl -> PAKReadyTransaction2(This,lPumpNumber,lTrsNumber,nNozzle,lVolume,lValue,nLevel,lGradePrice,nGrade,lFlage,lTotalVal,pRec,pVal) ) 

#define IGCI_SetPumpMode(This,lPumpNumber,lPumpMode,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetPumpMode(This,lPumpNumber,lPumpMode,lFlags,pVal) ) 

#define IGCI_GetCardInfo3(This,sAccountNumber,sTrack1,sTrack2,sShortCardName,lFlags,lSesionType,lPumpNumber,sRetData,pVal)	\
    ( (This)->lpVtbl -> GetCardInfo3(This,sAccountNumber,sTrack1,sTrack2,sShortCardName,lFlags,lSesionType,lPumpNumber,sRetData,pVal) ) 

#define IGCI_SetProcess(This,lProcessNumber,lParam1,lParam2,plRetVal)	\
    ( (This)->lpVtbl -> SetProcess(This,lProcessNumber,lParam1,lParam2,plRetVal) ) 

#define IGCI_SetConnectivityStatus(This,sPSConnStatus,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetConnectivityStatus(This,sPSConnStatus,lFlags,pVal) ) 

#define IGCI_GetPAKQuery(This,sRestrictionData,sOutData,plRecCount,lFlags,plRetVal)	\
    ( (This)->lpVtbl -> GetPAKQuery(This,sRestrictionData,sOutData,plRecCount,lFlags,plRetVal) ) 

#define IGCI_GetPumpMode(This,lPumpNumber,plPumpMode,plFlags,plMsgNumber,plVal)	\
    ( (This)->lpVtbl -> GetPumpMode(This,lPumpNumber,plPumpMode,plFlags,plMsgNumber,plVal) ) 

#define IGCI_ParseOLABuffer(This,lPumpNumber,cCardSaleALL3,sOLABuffer,lFlags,pRet)	\
    ( (This)->lpVtbl -> ParseOLABuffer(This,lPumpNumber,cCardSaleALL3,sOLABuffer,lFlags,pRet) ) 

#define IGCI_SetVersion(This,sVersionList,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetVersion(This,sVersionList,lFlags,pVal) ) 

#define IGCI_GetStationStatus(This,lPumpMap1,lPumpMap2,lFlags,pRetVal)	\
    ( (This)->lpVtbl -> GetStationStatus(This,lPumpMap1,lPumpMap2,lFlags,pRetVal) ) 

#define IGCI_UploadBuffer(This,lTerminalId,lPumpNumber,lSessionId,lUploadType,sInBuffer,sOutBuffer,lExDataSize,sExData,pVal)	\
    ( (This)->lpVtbl -> UploadBuffer(This,lTerminalId,lPumpNumber,lSessionId,lUploadType,sInBuffer,sOutBuffer,lExDataSize,sExData,pVal) ) 

#define IGCI_GetWetStockData(This,lWetStockFlags,pVal)	\
    ( (This)->lpVtbl -> GetWetStockData(This,lWetStockFlags,pVal) ) 

#define IGCI_GetTankMemoryData(This,lTankNumber,pRec,lFlags,lCounter,pVal)	\
    ( (This)->lpVtbl -> GetTankMemoryData(This,lTankNumber,pRec,lFlags,lCounter,pVal) ) 

#define IGCI_GetParam4(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,lFlags,lDataBase,pVal)	\
    ( (This)->lpVtbl -> GetParam4(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,lFlags,lDataBase,pVal) ) 

#define IGCI_SetParam4(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,lFlags,lDataBase,pVal)	\
    ( (This)->lpVtbl -> SetParam4(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,lFlags,lDataBase,pVal) ) 

#define IGCI_GetAttTrs(This,lPumpNumber,lFlags,sIndex,sRec,pVal)	\
    ( (This)->lpVtbl -> GetAttTrs(This,lPumpNumber,lFlags,sIndex,sRec,pVal) ) 

#define IGCI_PriceChangeUpdate(This,lPumpNumber,lPosition,lFlags,plVal)	\
    ( (This)->lpVtbl -> PriceChangeUpdate(This,lPumpNumber,lPosition,lFlags,plVal) ) 

#define IGCI_SetGradesRestriction(This,sTagPumpsNuzzlesRestrict,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetGradesRestriction(This,sTagPumpsNuzzlesRestrict,lFlags,pVal) ) 

#define IGCI_GetTransListQuery(This,lPumpNumber,sData,plRecCount,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetTransListQuery(This,lPumpNumber,sData,plRecCount,lFlags,pVal) ) 

#define IGCI_SetCashAcceptorStatus(This,lTerminalId,lFlags,lStatus,sRec,pVal)	\
    ( (This)->lpVtbl -> SetCashAcceptorStatus(This,lTerminalId,lFlags,lStatus,sRec,pVal) ) 

#define IGCI_GetCashAcceptorReport(This,lTerminalId,lFlags,sHeader,sRecords,lRecordCount,pVal)	\
    ( (This)->lpVtbl -> GetCashAcceptorReport(This,lTerminalId,lFlags,sHeader,sRecords,lRecordCount,pVal) ) 

#define IGCI_GetCardInfoEx(This,lClientId,sInBuffer,sInfoType,lFlags,sRetData,pVal)	\
    ( (This)->lpVtbl -> GetCardInfoEx(This,lClientId,sInBuffer,sInfoType,lFlags,sRetData,pVal) ) 

#define IGCI_TankReadingEx(This,lTankNumber,lFlags,sRec,pVal)	\
    ( (This)->lpVtbl -> TankReadingEx(This,lTankNumber,lFlags,sRec,pVal) ) 

#define IGCI_DeliveryReportEx(This,lTankNumber,lFlags,sRec,pVal)	\
    ( (This)->lpVtbl -> DeliveryReportEx(This,lTankNumber,lFlags,sRec,pVal) ) 

#define IGCI_ReadyOfflineTRS(This,lPumpNumber,lFlags,sData,sParam1,lParam2,lParam3,sRetData,pVal)	\
    ( (This)->lpVtbl -> ReadyOfflineTRS(This,lPumpNumber,lFlags,sData,sParam1,lParam2,lParam3,sRetData,pVal) ) 

#define IGCI_SetPumpStatus2(This,lPumpNumber,lStatus,lRes,lResExt,lFlags,nGrade,nPosition,sData,pVal)	\
    ( (This)->lpVtbl -> SetPumpStatus2(This,lPumpNumber,lStatus,lRes,lResExt,lFlags,nGrade,nPosition,sData,pVal) ) 

#define IGCI_SetPumpMisc(This,lPumpNumber,lMisc,bRemoveMisc,pVal)	\
    ( (This)->lpVtbl -> SetPumpMisc(This,lPumpNumber,lMisc,bRemoveMisc,pVal) ) 

#define IGCI_SetPumpMiscExt(This,lPumpNumber,lMisc,bRemoveMisc,pVal)	\
    ( (This)->lpVtbl -> SetPumpMiscExt(This,lPumpNumber,lMisc,bRemoveMisc,pVal) ) 

#define IGCI_ReportReady(This,lFlags,wszReportText,lParam,pVal)	\
    ( (This)->lpVtbl -> ReportReady(This,lFlags,wszReportText,lParam,pVal) ) 

#define IGCI_CarWashGeneralReqResponse(This,lDevice,lResult,lFlags1,lFlags2,sData,pVal)	\
    ( (This)->lpVtbl -> CarWashGeneralReqResponse(This,lDevice,lResult,lFlags1,lFlags2,sData,pVal) ) 

#define IGCI_PriceDisplayPanelStatus(This,lDevice,lStatus,sRec,pVal)	\
    ( (This)->lpVtbl -> PriceDisplayPanelStatus(This,lDevice,lStatus,sRec,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGCI_INTERFACE_DEFINED__ */


#ifndef __IGCIEvents_INTERFACE_DEFINED__
#define __IGCIEvents_INTERFACE_DEFINED__

/* interface IGCIEvents */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IGCIEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9BAC1F61-1ECE-11d1-B8A5-0000E8D0CBF8")
    IGCIEvents : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuthorizeAndLimit( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ short nFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StopPump( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResumePump( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpStatus( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( 
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpLimit( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nLevel,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ short nFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReservePump( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lRes,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReleasePump( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lRes,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpMode( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lMode,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpMode( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetGradePrice( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ short nFlags,
            /* [in] */ long lPriceA,
            /* [in] */ long lPriceB,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGradePrice( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpTotals( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PAPSwipeResult( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDeviceNumber,
            /* [in] */ long lSeqNumber,
            /* [in] */ long lRetcode,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRealTimeClock( 
            /* [in] */ long lSrc,
            /* [in] */ BSTR sDateTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdateParam( 
            /* [in] */ long lSrc,
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CancelTrs( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lRes,
            /* [in] */ long lPumpStatus,
            /* [in] */ long lTrsNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTankReading( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lReadStatus,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeliveryReport( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTankStatus( 
            /* [in] */ long lTankNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTrsState( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Watchdog( 
            /* [in] */ long lSrc,
            /* [in] */ long lTimeInterval,
            /* [out][in] */ long *lParam,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNewWashCode( 
            /* [in] */ long lSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lSrc,
            /* [in] */ long lWashProgram,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCarWashStatus( 
            /* [in] */ long lDevice,
            /* [in] */ long lSrc,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CancelWashCode( 
            /* [in] */ long lSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lSrc,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWetStockData( 
            /* [in] */ long lWetStockFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReConfigure( 
            /* [in] */ long lConfigureFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FormatReceipt( 
            /* [in] */ BSTR sReceiptData,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sReceiptText,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PrintReport( 
            /* [in] */ long lTerminalType,
            /* [in] */ long lTerminalNumber,
            /* [in] */ BSTR sData,
            /* [in] */ long lFlags,
            /* [in] */ long lTextLength,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuthorizeAndLimit2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ short nFlags,
            /* [in] */ BSTR sDiscountMap,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TLSCommand( 
            /* [in] */ BSTR sCommand,
            /* [in] */ long lCommand,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DebitProcessCmd( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSessionId,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GeneralSystemEvent( 
            /* [in] */ long lEventId,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpMode2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lMode,
            /* [in] */ long lFlags,
            /* [in] */ long lServiceMode,
            /* [in] */ long lServiceFee,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReConfigure2( 
            /* [in] */ long lTableId,
            /* [in] */ long lSubTableId,
            /* [in] */ long lSubTableIdExt,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CardInfoResult( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDeviceNumber,
            /* [in] */ long lSeqNumber,
            /* [in] */ long lRetcode,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TankGaugeData( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DisplayMsg( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTimeOut,
            /* [in] */ BSTR sRec,
            /* [in] */ long lMsgLen,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FormatReport( 
            /* [in] */ BSTR sReport,
            /* [in] */ long lType,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sReportText,
            /* [out][retval] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpMode3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lMode,
            /* [in] */ long lFlags,
            /* [in] */ long lServiceMode,
            /* [in] */ long lPriceLevel,
            /* [in] */ long lServiceFee,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuthorizeAndLimit3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetGradePriceWithTime( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ short nFlags,
            /* [in] */ long lPriceA,
            /* [in] */ long lPriceB,
            /* [in] */ BSTR sDateTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTrsState3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GeneralSystemEvent2( 
            /* [in] */ long lEventId,
            /* [in] */ long lFlags,
            /* [in] */ BSTR bstrData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CarWashGeneralReq( 
            /* [in] */ long lDevice,
            /* [in] */ long lFlags1,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWashCodeStatus( 
            /* [in] */ long lSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lSrc,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DisablePriceDisplay( 
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [in] */ long lDisablePriceDisplayPanel,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnablePriceDisplay( 
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [in] */ long lEnablePriceDisplay,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SmartReconfigure( 
            /* [in] */ long lDeviceNumberMask1,
            /* [in] */ long lDeviceNumberMask2,
            /* [in] */ long lTableId,
            /* [in] */ long lSubTableId,
            /* [in] */ long lSubTableIdExt,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IGCIEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGCIEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGCIEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGCIEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGCIEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGCIEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGCIEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGCIEvents * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuthorizeAndLimit )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ short nFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StopPump )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResumePump )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpStatus )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IGCIEvents * This,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpLimit )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nLevel,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ short nFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReservePump )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lRes,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReleasePump )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lRes,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpMode )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lMode,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpMode )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetGradePrice )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ short nFlags,
            /* [in] */ long lPriceA,
            /* [in] */ long lPriceB,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetGradePrice )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpTotals )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PAPSwipeResult )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDeviceNumber,
            /* [in] */ long lSeqNumber,
            /* [in] */ long lRetcode,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetRealTimeClock )( 
            IGCIEvents * This,
            /* [in] */ long lSrc,
            /* [in] */ BSTR sDateTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdateParam )( 
            IGCIEvents * This,
            /* [in] */ long lSrc,
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CancelTrs )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lRes,
            /* [in] */ long lPumpStatus,
            /* [in] */ long lTrsNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTankReading )( 
            IGCIEvents * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lReadStatus,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeliveryReport )( 
            IGCIEvents * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lSrc,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTankStatus )( 
            IGCIEvents * This,
            /* [in] */ long lTankNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTrsState )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Watchdog )( 
            IGCIEvents * This,
            /* [in] */ long lSrc,
            /* [in] */ long lTimeInterval,
            /* [out][in] */ long *lParam,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNewWashCode )( 
            IGCIEvents * This,
            /* [in] */ long lSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lSrc,
            /* [in] */ long lWashProgram,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCarWashStatus )( 
            IGCIEvents * This,
            /* [in] */ long lDevice,
            /* [in] */ long lSrc,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CancelWashCode )( 
            IGCIEvents * This,
            /* [in] */ long lSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lSrc,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWetStockData )( 
            IGCIEvents * This,
            /* [in] */ long lWetStockFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReConfigure )( 
            IGCIEvents * This,
            /* [in] */ long lConfigureFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FormatReceipt )( 
            IGCIEvents * This,
            /* [in] */ BSTR sReceiptData,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sReceiptText,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PrintReport )( 
            IGCIEvents * This,
            /* [in] */ long lTerminalType,
            /* [in] */ long lTerminalNumber,
            /* [in] */ BSTR sData,
            /* [in] */ long lFlags,
            /* [in] */ long lTextLength,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuthorizeAndLimit2 )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ short nFlags,
            /* [in] */ BSTR sDiscountMap,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TLSCommand )( 
            IGCIEvents * This,
            /* [in] */ BSTR sCommand,
            /* [in] */ long lCommand,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DebitProcessCmd )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSessionId,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GeneralSystemEvent )( 
            IGCIEvents * This,
            /* [in] */ long lEventId,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpMode2 )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lMode,
            /* [in] */ long lFlags,
            /* [in] */ long lServiceMode,
            /* [in] */ long lServiceFee,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReConfigure2 )( 
            IGCIEvents * This,
            /* [in] */ long lTableId,
            /* [in] */ long lSubTableId,
            /* [in] */ long lSubTableIdExt,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CardInfoResult )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lDeviceNumber,
            /* [in] */ long lSeqNumber,
            /* [in] */ long lRetcode,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TankGaugeData )( 
            IGCIEvents * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DisplayMsg )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lTimeOut,
            /* [in] */ BSTR sRec,
            /* [in] */ long lMsgLen,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FormatReport )( 
            IGCIEvents * This,
            /* [in] */ BSTR sReport,
            /* [in] */ long lType,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sReportText,
            /* [out][retval] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpMode3 )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lMode,
            /* [in] */ long lFlags,
            /* [in] */ long lServiceMode,
            /* [in] */ long lPriceLevel,
            /* [in] */ long lServiceFee,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuthorizeAndLimit3 )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lLevel,
            /* [in] */ long lGradePrice,
            /* [in] */ long lMaxValue,
            /* [in] */ long lMaxVolume,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetGradePriceWithTime )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ short nFlags,
            /* [in] */ long lPriceA,
            /* [in] */ long lPriceB,
            /* [in] */ BSTR sDateTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTrsState3 )( 
            IGCIEvents * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lSrc,
            /* [in] */ long lTrsNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GeneralSystemEvent2 )( 
            IGCIEvents * This,
            /* [in] */ long lEventId,
            /* [in] */ long lFlags,
            /* [in] */ BSTR bstrData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CarWashGeneralReq )( 
            IGCIEvents * This,
            /* [in] */ long lDevice,
            /* [in] */ long lFlags1,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWashCodeStatus )( 
            IGCIEvents * This,
            /* [in] */ long lSyncNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lSrc,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DisablePriceDisplay )( 
            IGCIEvents * This,
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [in] */ long lDisablePriceDisplayPanel,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnablePriceDisplay )( 
            IGCIEvents * This,
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [in] */ long lEnablePriceDisplay,
            /* [in] */ BSTR sData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SmartReconfigure )( 
            IGCIEvents * This,
            /* [in] */ long lDeviceNumberMask1,
            /* [in] */ long lDeviceNumberMask2,
            /* [in] */ long lTableId,
            /* [in] */ long lSubTableId,
            /* [in] */ long lSubTableIdExt,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal);
        
        END_INTERFACE
    } IGCIEventsVtbl;

    interface IGCIEvents
    {
        CONST_VTBL struct IGCIEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGCIEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGCIEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGCIEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGCIEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IGCIEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IGCIEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IGCIEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IGCIEvents_AuthorizeAndLimit(This,lPumpNumber,lSrc,nLevel,lGradePrice,lMaxValue,lMaxVolume,nFlags,pVal)	\
    ( (This)->lpVtbl -> AuthorizeAndLimit(This,lPumpNumber,lSrc,nLevel,lGradePrice,lMaxValue,lMaxVolume,nFlags,pVal) ) 

#define IGCIEvents_StopPump(This,lPumpNumber,lSrc,pVal)	\
    ( (This)->lpVtbl -> StopPump(This,lPumpNumber,lSrc,pVal) ) 

#define IGCIEvents_ResumePump(This,lPumpNumber,lSrc,pVal)	\
    ( (This)->lpVtbl -> ResumePump(This,lPumpNumber,lSrc,pVal) ) 

#define IGCIEvents_GetPumpStatus(This,lPumpNumber,lSrc,pVal)	\
    ( (This)->lpVtbl -> GetPumpStatus(This,lPumpNumber,lSrc,pVal) ) 

#define IGCIEvents_Reset(This,lSrc,pVal)	\
    ( (This)->lpVtbl -> Reset(This,lSrc,pVal) ) 

#define IGCIEvents_SetPumpLimit(This,lPumpNumber,lSrc,nLevel,lMaxValue,lMaxVolume,nFlags,pVal)	\
    ( (This)->lpVtbl -> SetPumpLimit(This,lPumpNumber,lSrc,nLevel,lMaxValue,lMaxVolume,nFlags,pVal) ) 

#define IGCIEvents_ReservePump(This,lPumpNumber,lSrc,lRes,pVal)	\
    ( (This)->lpVtbl -> ReservePump(This,lPumpNumber,lSrc,lRes,pVal) ) 

#define IGCIEvents_ReleasePump(This,lPumpNumber,lSrc,lRes,pVal)	\
    ( (This)->lpVtbl -> ReleasePump(This,lPumpNumber,lSrc,lRes,pVal) ) 

#define IGCIEvents_SetPumpMode(This,lPumpNumber,lSrc,lMode,pVal)	\
    ( (This)->lpVtbl -> SetPumpMode(This,lPumpNumber,lSrc,lMode,pVal) ) 

#define IGCIEvents_GetPumpMode(This,lPumpNumber,lSrc,pVal)	\
    ( (This)->lpVtbl -> GetPumpMode(This,lPumpNumber,lSrc,pVal) ) 

#define IGCIEvents_SetGradePrice(This,lPumpNumber,lSrc,nNozzle,nGrade,nFlags,lPriceA,lPriceB,pVal)	\
    ( (This)->lpVtbl -> SetGradePrice(This,lPumpNumber,lSrc,nNozzle,nGrade,nFlags,lPriceA,lPriceB,pVal) ) 

#define IGCIEvents_GetGradePrice(This,lPumpNumber,lSrc,nNozzle,nGrade,pVal)	\
    ( (This)->lpVtbl -> GetGradePrice(This,lPumpNumber,lSrc,nNozzle,nGrade,pVal) ) 

#define IGCIEvents_GetPumpTotals(This,lPumpNumber,lSrc,pVal)	\
    ( (This)->lpVtbl -> GetPumpTotals(This,lPumpNumber,lSrc,pVal) ) 

#define IGCIEvents_PAPSwipeResult(This,lPumpNumber,lDeviceNumber,lSeqNumber,lRetcode,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> PAPSwipeResult(This,lPumpNumber,lDeviceNumber,lSeqNumber,lRetcode,lFlags,sData,pVal) ) 

#define IGCIEvents_SetRealTimeClock(This,lSrc,sDateTime,pVal)	\
    ( (This)->lpVtbl -> SetRealTimeClock(This,lSrc,sDateTime,pVal) ) 

#define IGCIEvents_UpdateParam(This,lSrc,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,pVal)	\
    ( (This)->lpVtbl -> UpdateParam(This,lSrc,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,pVal) ) 

#define IGCIEvents_CancelTrs(This,lPumpNumber,lSrc,lRes,lPumpStatus,lTrsNumber,pVal)	\
    ( (This)->lpVtbl -> CancelTrs(This,lPumpNumber,lSrc,lRes,lPumpStatus,lTrsNumber,pVal) ) 

#define IGCIEvents_GetTankReading(This,lTankNumber,lSrc,lReadStatus,pVal)	\
    ( (This)->lpVtbl -> GetTankReading(This,lTankNumber,lSrc,lReadStatus,pVal) ) 

#define IGCIEvents_GetDeliveryReport(This,lTankNumber,lSrc,pVal)	\
    ( (This)->lpVtbl -> GetDeliveryReport(This,lTankNumber,lSrc,pVal) ) 

#define IGCIEvents_GetTankStatus(This,lTankNumber,pVal)	\
    ( (This)->lpVtbl -> GetTankStatus(This,lTankNumber,pVal) ) 

#define IGCIEvents_SetTrsState(This,lPumpNumber,lSrc,lTrsNumber,lState,pVal)	\
    ( (This)->lpVtbl -> SetTrsState(This,lPumpNumber,lSrc,lTrsNumber,lState,pVal) ) 

#define IGCIEvents_Watchdog(This,lSrc,lTimeInterval,lParam,pVal)	\
    ( (This)->lpVtbl -> Watchdog(This,lSrc,lTimeInterval,lParam,pVal) ) 

#define IGCIEvents_GetNewWashCode(This,lSyncNumber,lDevice,lSrc,lWashProgram,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetNewWashCode(This,lSyncNumber,lDevice,lSrc,lWashProgram,lFlags,pVal) ) 

#define IGCIEvents_GetCarWashStatus(This,lDevice,lSrc,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetCarWashStatus(This,lDevice,lSrc,lFlags,pVal) ) 

#define IGCIEvents_CancelWashCode(This,lSyncNumber,lDevice,lSrc,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> CancelWashCode(This,lSyncNumber,lDevice,lSrc,lFlags,sData,pVal) ) 

#define IGCIEvents_GetWetStockData(This,lWetStockFlags,pVal)	\
    ( (This)->lpVtbl -> GetWetStockData(This,lWetStockFlags,pVal) ) 

#define IGCIEvents_ReConfigure(This,lConfigureFlags,pVal)	\
    ( (This)->lpVtbl -> ReConfigure(This,lConfigureFlags,pVal) ) 

#define IGCIEvents_FormatReceipt(This,sReceiptData,lFlags,sReceiptText,pVal)	\
    ( (This)->lpVtbl -> FormatReceipt(This,sReceiptData,lFlags,sReceiptText,pVal) ) 

#define IGCIEvents_PrintReport(This,lTerminalType,lTerminalNumber,sData,lFlags,lTextLength,pVal)	\
    ( (This)->lpVtbl -> PrintReport(This,lTerminalType,lTerminalNumber,sData,lFlags,lTextLength,pVal) ) 

#define IGCIEvents_AuthorizeAndLimit2(This,lPumpNumber,lSrc,nLevel,lGradePrice,lMaxValue,lMaxVolume,nFlags,sDiscountMap,pVal)	\
    ( (This)->lpVtbl -> AuthorizeAndLimit2(This,lPumpNumber,lSrc,nLevel,lGradePrice,lMaxValue,lMaxVolume,nFlags,sDiscountMap,pVal) ) 

#define IGCIEvents_TLSCommand(This,sCommand,lCommand,lTankNumber,lFlags,plRetVal)	\
    ( (This)->lpVtbl -> TLSCommand(This,sCommand,lCommand,lTankNumber,lFlags,plRetVal) ) 

#define IGCIEvents_DebitProcessCmd(This,lPumpNumber,lSessionId,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> DebitProcessCmd(This,lPumpNumber,lSessionId,lFlags,sData,pVal) ) 

#define IGCIEvents_GeneralSystemEvent(This,lEventId,lFlags,pVal)	\
    ( (This)->lpVtbl -> GeneralSystemEvent(This,lEventId,lFlags,pVal) ) 

#define IGCIEvents_SetPumpMode2(This,lPumpNumber,lSrc,lMode,lFlags,lServiceMode,lServiceFee,pVal)	\
    ( (This)->lpVtbl -> SetPumpMode2(This,lPumpNumber,lSrc,lMode,lFlags,lServiceMode,lServiceFee,pVal) ) 

#define IGCIEvents_ReConfigure2(This,lTableId,lSubTableId,lSubTableIdExt,lFlags,pRetVal)	\
    ( (This)->lpVtbl -> ReConfigure2(This,lTableId,lSubTableId,lSubTableIdExt,lFlags,pRetVal) ) 

#define IGCIEvents_CardInfoResult(This,lPumpNumber,lDeviceNumber,lSeqNumber,lRetcode,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> CardInfoResult(This,lPumpNumber,lDeviceNumber,lSeqNumber,lRetcode,lFlags,sData,pVal) ) 

#define IGCIEvents_TankGaugeData(This,lTankNumber,lFlags,sRec,pVal)	\
    ( (This)->lpVtbl -> TankGaugeData(This,lTankNumber,lFlags,sRec,pVal) ) 

#define IGCIEvents_DisplayMsg(This,lPumpNumber,lTimeOut,sRec,lMsgLen,plVal)	\
    ( (This)->lpVtbl -> DisplayMsg(This,lPumpNumber,lTimeOut,sRec,lMsgLen,plVal) ) 

#define IGCIEvents_FormatReport(This,sReport,lType,lFlags,sReportText,pVal)	\
    ( (This)->lpVtbl -> FormatReport(This,sReport,lType,lFlags,sReportText,pVal) ) 

#define IGCIEvents_SetPumpMode3(This,lPumpNumber,lSrc,lMode,lFlags,lServiceMode,lPriceLevel,lServiceFee,pVal)	\
    ( (This)->lpVtbl -> SetPumpMode3(This,lPumpNumber,lSrc,lMode,lFlags,lServiceMode,lPriceLevel,lServiceFee,pVal) ) 

#define IGCIEvents_AuthorizeAndLimit3(This,lPumpNumber,lSrc,lLevel,lGradePrice,lMaxValue,lMaxVolume,lFlags,sRec,pVal)	\
    ( (This)->lpVtbl -> AuthorizeAndLimit3(This,lPumpNumber,lSrc,lLevel,lGradePrice,lMaxValue,lMaxVolume,lFlags,sRec,pVal) ) 

#define IGCIEvents_SetGradePriceWithTime(This,lPumpNumber,lSrc,nNozzle,nGrade,nFlags,lPriceA,lPriceB,sDateTime,pVal)	\
    ( (This)->lpVtbl -> SetGradePriceWithTime(This,lPumpNumber,lSrc,nNozzle,nGrade,nFlags,lPriceA,lPriceB,sDateTime,pVal) ) 

#define IGCIEvents_SetTrsState3(This,lPumpNumber,lSrc,lTrsNumber,lState,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> SetTrsState3(This,lPumpNumber,lSrc,lTrsNumber,lState,lFlags,sData,pVal) ) 

#define IGCIEvents_GeneralSystemEvent2(This,lEventId,lFlags,bstrData,pVal)	\
    ( (This)->lpVtbl -> GeneralSystemEvent2(This,lEventId,lFlags,bstrData,pVal) ) 

#define IGCIEvents_CarWashGeneralReq(This,lDevice,lFlags1,pVal)	\
    ( (This)->lpVtbl -> CarWashGeneralReq(This,lDevice,lFlags1,pVal) ) 

#define IGCIEvents_GetWashCodeStatus(This,lSyncNumber,lDevice,lSrc,lFlags,sData,pVal)	\
    ( (This)->lpVtbl -> GetWashCodeStatus(This,lSyncNumber,lDevice,lSrc,lFlags,sData,pVal) ) 

#define IGCIEvents_DisablePriceDisplay(This,lDevice,lFlags,lDisablePriceDisplayPanel,sData,pVal)	\
    ( (This)->lpVtbl -> DisablePriceDisplay(This,lDevice,lFlags,lDisablePriceDisplayPanel,sData,pVal) ) 

#define IGCIEvents_EnablePriceDisplay(This,lDevice,lFlags,lEnablePriceDisplay,sData,pVal)	\
    ( (This)->lpVtbl -> EnablePriceDisplay(This,lDevice,lFlags,lEnablePriceDisplay,sData,pVal) ) 

#define IGCIEvents_SmartReconfigure(This,lDeviceNumberMask1,lDeviceNumberMask2,lTableId,lSubTableId,lSubTableIdExt,lFlags,pRetVal)	\
    ( (This)->lpVtbl -> SmartReconfigure(This,lDeviceNumberMask1,lDeviceNumberMask2,lTableId,lSubTableId,lSubTableIdExt,lFlags,pRetVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGCIEvents_INTERFACE_DEFINED__ */


#ifndef __ICtrl_INTERFACE_DEFINED__
#define __ICtrl_INTERFACE_DEFINED__

/* interface ICtrl */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICtrl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("43464621-C31A-11D1-8625-0000E81A8D8B")
    ICtrl : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpMode( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMode,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpMode( 
            /* [in] */ long lPumpNumber,
            /* [out] */ long *lMode,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetGradePrice( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ long lMode,
            /* [in] */ short nLevel,
            /* [in] */ long lPrice,
            /* [in] */ BSTR sTimeValid,
            /* [in] */ short nFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGradePrice( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ long lMode,
            /* [in] */ short nLevel,
            /* [out] */ long *lPrice,
            /* [out] */ BSTR *sDateTimeValid,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetShift( 
            /* [in] */ short nDayOfWeek,
            /* [in] */ short nShiftNumber,
            /* [in] */ short nShiftFlags,
            /* [in] */ BSTR sStartTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetShift( 
            /* [in] */ short nDayOfWeek,
            /* [in] */ short nShiftNumber,
            /* [out] */ short *nShiftFlags,
            /* [out] */ BSTR *sStartTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpTotals( 
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lPump,
            /* [out] */ short *nNozzle,
            /* [out] */ short *nGrade,
            /* [out] */ short *nTotalsFlags,
            /* [out] */ BSTR *sVolume,
            /* [out] */ BSTR *sValueA,
            /* [out] */ BSTR *sValueB,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDateTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemovePumpTotals( 
            /* [in] */ short nFlags,
            /* [in] */ long lRecIndex,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTankReading( 
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lTank,
            /* [out] */ long *lProduct,
            /* [out] */ long *lStatus,
            /* [out] */ short *nReadingFlags,
            /* [out] */ long *lFuelLevel,
            /* [out] */ long *lWaterLevel,
            /* [out] */ long *lTemperature,
            /* [out] */ long *lFuelVolume,
            /* [out] */ long *lWaterVolume,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDateTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveTankReading( 
            /* [in] */ short nFlags,
            /* [in] */ long lRecIndex,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeliveryReport( 
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lTank,
            /* [out] */ long *lProduct,
            /* [out] */ long *lStatus,
            /* [out] */ short *nDeliveryFlags,
            /* [out] */ long *lTag,
            /* [out] */ long *lDeliveredVolume,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDeliveryDateTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveDeliveryReport( 
            /* [in] */ short nFlags,
            /* [in] */ long lRecIndex,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRealTimeClock( 
            /* [in] */ BSTR sDateAndTime,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ControlStatus( 
            /* [in] */ short nFlags,
            /* [out][in] */ long *lTotalIndex,
            /* [out][in] */ long *lTankReadIndex,
            /* [out][in] */ long *lDeliveryIndex,
            /* [out] */ long *lActiveShiftNumber,
            /* [out] */ short *nStationStatus,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProcessStatus( 
            /* [in] */ long lProcessNumber,
            /* [out] */ long *lProcessStatus,
            /* [out] */ long *lPumpNumberA,
            /* [out] */ long *lPumpProblemA,
            /* [out] */ long *lPumpNumberB,
            /* [out] */ long *lPumpProblemB,
            /* [out] */ long *lTankNumber,
            /* [out] */ long *lTankProblem,
            /* [out] */ long *lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetParam( 
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParam( 
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpModeScheme( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [in] */ long lMode,
            /* [in] */ BSTR sDateTimeStart,
            /* [in] */ BSTR sDateTimeEnd,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpModeScheme( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [out] */ long *plMode,
            /* [out] */ BSTR *psDateTimeStart,
            /* [out] */ BSTR *psDateTimeEnd,
            /* [out] */ long *plFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetModeDef( 
            /* [in] */ long lMode,
            /* [in] */ BSTR sName,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetModeDef( 
            /* [in] */ long lMode,
            /* [out] */ BSTR *psName,
            /* [out] */ long *plFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParam1( 
            /* [in] */ BSTR sTableAndFieldName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out] */ BSTR *sVal,
            /* [out] */ long *lVal,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetParam1( 
            /* [in] */ BSTR sTableAndFieldName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetParam2( 
            /* [in] */ BSTR sTableFieldAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParam2( 
            /* [out][in] */ BSTR *sTableFieldAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out] */ long *lVal,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRecord( 
            /* [out][in] */ BSTR *sTableAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRecord( 
            /* [in] */ BSTR sTableAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTankReading2( 
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lTank,
            /* [out] */ long *lProduct,
            /* [out] */ long *lStatus1,
            /* [out] */ short *nReadingFlags,
            /* [out] */ long *lFuelLevel,
            /* [out] */ long *lWaterLevel,
            /* [out] */ long *lTemperature,
            /* [out] */ long *lFuelVolume,
            /* [out] */ long *lWaterVolume,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDateTime,
            /* [out] */ long *lStatus2,
            /* [out] */ long *lFuelValue,
            /* [out] */ long *lPricePerLitre,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpTotals2( 
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lPump,
            /* [out] */ short *nNozzle,
            /* [out] */ short *nGrade,
            /* [out] */ short *nTotalsFlags,
            /* [out] */ BSTR *sVolume,
            /* [out] */ BSTR *sValueA,
            /* [out] */ BSTR *sValueB,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDateTime,
            /* [out] */ long *lPumpStatus,
            /* [out] */ long *lNozzleStatus,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeliveryReport2( 
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lTank,
            /* [out] */ long *lProduct,
            /* [out] */ long *lStatus,
            /* [out] */ short *nDeliveryFlags,
            /* [out] */ long *lTag,
            /* [out] */ long *lDeliveredVolume,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sStartDeliveryDateTime,
            /* [out] */ BSTR *sEndDeliveryDateTime,
            /* [out] */ long *lStartVolume,
            /* [out] */ long *lEndVolume,
            /* [out] */ long *lTemperature,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAlarm( 
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *plCatagory,
            /* [out] */ long *plCode,
            /* [out] */ long *plParameter,
            /* [out] */ BSTR *psStartDateAndTime,
            /* [out] */ BSTR *psEndDateAndTime,
            /* [out] */ long *plActiveReadNumber,
            /* [out] */ long *plShiftNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveAlarm( 
            /* [in] */ short nFlags,
            /* [in] */ long lRecIndex,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetServiceStatus( 
            /* [in] */ long lService,
            /* [out] */ long *lStatus,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StopService( 
            /* [in] */ long lService,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartService( 
            /* [in] */ long lService,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWetStockData( 
            /* [in] */ long lWetStockFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReConfigure( 
            /* [in] */ long lConfigureFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPOSInfo( 
            /* [out] */ BSTR *sPOSInfo,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMaintenance( 
            /* [in] */ long lLock,
            /* [in] */ long lParam,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParam3( 
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetParam3( 
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetGradePrice2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ long lMode,
            /* [in] */ short nLevel,
            /* [in] */ long lPrice,
            /* [in] */ BSTR sTimeValid,
            /* [in] */ short nFlags,
            /* [in] */ long lTaxRate,
            /* [in] */ long lDiscountPerUnit,
            /* [in] */ long lMaxUnitForDiscount,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpModeScheme2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [in] */ long lMode,
            /* [in] */ BSTR sPeriodDateTimeStart,
            /* [in] */ BSTR sPeriodDateTimeEnd,
            /* [in] */ BSTR sDateTimeStart,
            /* [in] */ BSTR sDateTimeEnd,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpModeScheme2( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [out] */ long *plMode,
            /* [out] */ BSTR *psPeriodDateTimeStart,
            /* [out] */ BSTR *psPeriodDateTimeEnd,
            /* [out] */ BSTR *psDateTimeStart,
            /* [out] */ BSTR *psDateTimeEnd,
            /* [out] */ long *plFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetServiceFee( 
            /* [in] */ long lPump,
            /* [in] */ long lTrsType,
            /* [in] */ long lTrsNumber,
            /* [in] */ BSTR stTime,
            /* [in] */ long lFlags,
            /* [out] */ long *pValue,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetServiceFee( 
            /* [in] */ long lFeeNumber,
            /* [in] */ BSTR sName,
            /* [in] */ long lFlags,
            /* [in] */ long lPakFee,
            /* [in] */ long lPrePayFee,
            /* [in] */ long lPapFee,
            /* [in] */ long lAviFee,
            /* [in] */ long lAttendFee,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPumpModeScheme3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [in] */ long lMode,
            /* [in] */ BSTR sPeriodDateTimeStart,
            /* [in] */ BSTR sPeriodDateTimeEnd,
            /* [in] */ BSTR sDateTimeStart,
            /* [in] */ BSTR sDateTimeEnd,
            /* [in] */ long lServiceFeeNo,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpModeScheme3( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [out] */ long *plMode,
            /* [out] */ BSTR *psPeriodDateTimeStart,
            /* [out] */ BSTR *psPeriodDateTimeEnd,
            /* [out] */ BSTR *psDateTimeStart,
            /* [out] */ BSTR *psDateTimeEnd,
            /* [out] */ long *plServiceFeeNo,
            /* [out] */ long *plFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitEventsInterface( 
            /* [in] */ long lFlags,
            /* [out][in] */ long *pClientId,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CloseEventsInterface( 
            /* [in] */ long lClientId,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetServiceFee2( 
            /* [in] */ long lFeeNumber,
            /* [in] */ BSTR sName,
            /* [in] */ long lFlags,
            /* [in] */ long lPakFee,
            /* [in] */ long lPrePayFee,
            /* [in] */ long lPapFee,
            /* [in] */ long lAviFee,
            /* [in] */ long lAttendFee,
            /* [in] */ long lSelfPakFee,
            /* [in] */ long lSelfPrePayFee,
            /* [in] */ long lSelfPapFee,
            /* [in] */ long lSelfAviFee,
            /* [in] */ long lSelfAttendFee,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TLSCommand( 
            /* [in] */ BSTR sCommand,
            /* [in] */ long lCommand,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResetQdxInfo( 
            /* [in] */ long lFileNum,
            /* [in] */ long lPumpNum,
            /* [in] */ long lFlags1,
            /* [in] */ long lFlags2,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitDebit( 
            /* [in] */ long lPumpGroup01,
            /* [in] */ long lPumpGroup2,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdateControllerSoftware( 
            /* [in] */ BSTR sFileFullPath,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTankReading3( 
            /* [in] */ short nOpCode,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lTank,
            /* [out] */ long *lProduct,
            /* [out] */ long *lStatus1,
            /* [out] */ short *nReadingFlags,
            /* [out] */ long *lFuelLevel,
            /* [out] */ long *lWaterLevel,
            /* [out] */ long *lTemperature,
            /* [out] */ long *lFuelVolume,
            /* [out] */ long *lWaterVolume,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDateTime,
            /* [out] */ long *lStatus2,
            /* [out] */ long *lFuelValue,
            /* [out] */ long *lPricePerLitre,
            /* [out] */ long *lTcVolume,
            /* [out] */ long *lUllage,
            /* [out][in] */ long *lFlags,
            /* [out] */ BSTR *sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReloadPricePolesData( 
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpDevices( 
            /* [in] */ long lGrade,
            /* [in] */ long lNacsCode,
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [out] */ long *pVal,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTankInfo( 
            /* [in] */ long lTankNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpTotalsInfo( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeliveryInfo( 
            /* [in] */ long lTankNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reconfigure2( 
            /* [in] */ long lTableId,
            /* [in] */ long lSubTableId,
            /* [in] */ long lSubTableIdExt,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GeneralSystemRequest( 
            /* [in] */ long lVal,
            /* [in] */ BSTR sRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTankMemoryData( 
            /* [in] */ long lTankNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lCounter,
            /* [out][retval] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DBTankGauge( 
            /* [in] */ long lOffset,
            /* [out][in] */ BSTR *sRec,
            /* [in] */ long lRecSize,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TankGaugeData( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetConnectivityStatus( 
            /* [out] */ BSTR *sPSConnStatus,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAccumulateTotals( 
            /* [in] */ long lShiftNumber,
            /* [in] */ long lState,
            /* [in] */ long lPosNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lVal,
            /* [in] */ BSTR sStartTime,
            /* [in] */ BSTR sEndTime,
            /* [out] */ long *lRecordCount,
            /* [out][in] */ BSTR *sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAccumulateState( 
            /* [in] */ long lShiftNumber,
            /* [in] */ long lPosNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetModeDef2( 
            /* [in] */ long lMode,
            /* [in] */ BSTR sRec,
            /* [in] */ long lFlags,
            /* [in] */ long lMsgNumber,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetModeDef2( 
            /* [in] */ long lMode,
            /* [out] */ BSTR *psRec,
            /* [out] */ long *plFlags,
            /* [out] */ long *plMsgNumber,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGeneralData( 
            /* [in] */ long lRecordType,
            /* [out] */ BSTR *sData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetServiceFeeMode( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFeeSet,
            /* [in] */ long Flags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetServiceFeeMode( 
            /* [in] */ long lPumpNumber,
            /* [out] */ long *lFeeSet,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitParam( 
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sPCIXml,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPaidTrs( 
            /* [in] */ long lClientId,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [out][in] */ long *lCountRec,
            /* [retval][out] */ long *plRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpTotalsMemoData( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [out] */ long *lCounter,
            /* [out][in] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReceipt2( 
            /* [in] */ long lPosNumber,
            /* [in] */ BSTR sTrack2,
            /* [in] */ long lIndex,
            /* [out] */ BSTR *sReceiptData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTankReadingEx( 
            /* [in] */ long lOpCode,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeliveryReportEx( 
            /* [in] */ long lOpCode,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsUnmanned( 
            /* [out] */ BOOL *pbUnmanned,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVersion( 
            /* [out] */ BSTR *sVersionList,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetupEx( 
            /* [in] */ BSTR sXmlRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartQdxMaintenance( 
            /* [in] */ long lFileNum,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetStationStatus( 
            /* [out][in] */ long *lPumpMap1,
            /* [out][in] */ long *lPumpMap2,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeliveryEmulationStatus( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [out] */ long *plStatus,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetChangeProductInTankStatus( 
            /* [in] */ long lFlags,
            /* [out] */ long *plStatus,
            /* [out] */ long *plErrorCode,
            /* [out] */ BSTR *pbstrData,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProcessStatusEx( 
            /* [in] */ long lFlags,
            /* [in] */ long lProcessType,
            /* [out] */ long *plStatus,
            /* [out] */ long *plErrorCode,
            /* [out] */ BSTR *pbstrData,
            /* [retval][out] */ long *plVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetParam4( 
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPumpTotalsEx( 
            /* [in] */ long lOpCode,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lNozzle,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRollingUpdateInfo( 
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pbstrData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAlarm( 
            /* [in] */ long lCatagory,
            /* [in] */ long lSubCatagory,
            /* [in] */ long lAlarmNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lAlarmStatus,
            /* [in] */ long lAlarmValue,
            /* [in] */ long lFlags,
            /* [out] */ long *pAlarmId,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetConfiguration( 
            /* [in] */ BSTR sClientName,
            /* [out] */ BSTR *sConfigurationData,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTankState( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTankState( 
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [out][in] */ BSTR *pRec,
            /* [retval][out] */ long *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SmartReconfigure( 
            /* [in] */ long lDeviceNumberMask1,
            /* [in] */ long lDeviceNumberMask2,
            /* [in] */ long lTableId,
            /* [in] */ long lSubTableId,
            /* [in] */ long lSubTableIdExt,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICtrlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICtrl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICtrl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICtrl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICtrl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICtrl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICtrl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICtrl * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpMode )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lMode,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpMode )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ long *lMode,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetGradePrice )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ long lMode,
            /* [in] */ short nLevel,
            /* [in] */ long lPrice,
            /* [in] */ BSTR sTimeValid,
            /* [in] */ short nFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetGradePrice )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ long lMode,
            /* [in] */ short nLevel,
            /* [out] */ long *lPrice,
            /* [out] */ BSTR *sDateTimeValid,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetShift )( 
            ICtrl * This,
            /* [in] */ short nDayOfWeek,
            /* [in] */ short nShiftNumber,
            /* [in] */ short nShiftFlags,
            /* [in] */ BSTR sStartTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetShift )( 
            ICtrl * This,
            /* [in] */ short nDayOfWeek,
            /* [in] */ short nShiftNumber,
            /* [out] */ short *nShiftFlags,
            /* [out] */ BSTR *sStartTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpTotals )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lPump,
            /* [out] */ short *nNozzle,
            /* [out] */ short *nGrade,
            /* [out] */ short *nTotalsFlags,
            /* [out] */ BSTR *sVolume,
            /* [out] */ BSTR *sValueA,
            /* [out] */ BSTR *sValueB,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDateTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemovePumpTotals )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [in] */ long lRecIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTankReading )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lTank,
            /* [out] */ long *lProduct,
            /* [out] */ long *lStatus,
            /* [out] */ short *nReadingFlags,
            /* [out] */ long *lFuelLevel,
            /* [out] */ long *lWaterLevel,
            /* [out] */ long *lTemperature,
            /* [out] */ long *lFuelVolume,
            /* [out] */ long *lWaterVolume,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDateTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveTankReading )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [in] */ long lRecIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeliveryReport )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lTank,
            /* [out] */ long *lProduct,
            /* [out] */ long *lStatus,
            /* [out] */ short *nDeliveryFlags,
            /* [out] */ long *lTag,
            /* [out] */ long *lDeliveredVolume,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDeliveryDateTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveDeliveryReport )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [in] */ long lRecIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetRealTimeClock )( 
            ICtrl * This,
            /* [in] */ BSTR sDateAndTime,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ControlStatus )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [out][in] */ long *lTotalIndex,
            /* [out][in] */ long *lTankReadIndex,
            /* [out][in] */ long *lDeliveryIndex,
            /* [out] */ long *lActiveShiftNumber,
            /* [out] */ short *nStationStatus,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProcessStatus )( 
            ICtrl * This,
            /* [in] */ long lProcessNumber,
            /* [out] */ long *lProcessStatus,
            /* [out] */ long *lPumpNumberA,
            /* [out] */ long *lPumpProblemA,
            /* [out] */ long *lPumpNumberB,
            /* [out] */ long *lPumpProblemB,
            /* [out] */ long *lTankNumber,
            /* [out] */ long *lTankProblem,
            /* [out] */ long *lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetParam )( 
            ICtrl * This,
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParam )( 
            ICtrl * This,
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpModeScheme )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [in] */ long lMode,
            /* [in] */ BSTR sDateTimeStart,
            /* [in] */ BSTR sDateTimeEnd,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpModeScheme )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [out] */ long *plMode,
            /* [out] */ BSTR *psDateTimeStart,
            /* [out] */ BSTR *psDateTimeEnd,
            /* [out] */ long *plFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetModeDef )( 
            ICtrl * This,
            /* [in] */ long lMode,
            /* [in] */ BSTR sName,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetModeDef )( 
            ICtrl * This,
            /* [in] */ long lMode,
            /* [out] */ BSTR *psName,
            /* [out] */ long *plFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParam1 )( 
            ICtrl * This,
            /* [in] */ BSTR sTableAndFieldName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out] */ BSTR *sVal,
            /* [out] */ long *lVal,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetParam1 )( 
            ICtrl * This,
            /* [in] */ BSTR sTableAndFieldName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetParam2 )( 
            ICtrl * This,
            /* [in] */ BSTR sTableFieldAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParam2 )( 
            ICtrl * This,
            /* [out][in] */ BSTR *sTableFieldAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out] */ long *lVal,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRecord )( 
            ICtrl * This,
            /* [out][in] */ BSTR *sTableAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetRecord )( 
            ICtrl * This,
            /* [in] */ BSTR sTableAndVal,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTankReading2 )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lTank,
            /* [out] */ long *lProduct,
            /* [out] */ long *lStatus1,
            /* [out] */ short *nReadingFlags,
            /* [out] */ long *lFuelLevel,
            /* [out] */ long *lWaterLevel,
            /* [out] */ long *lTemperature,
            /* [out] */ long *lFuelVolume,
            /* [out] */ long *lWaterVolume,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDateTime,
            /* [out] */ long *lStatus2,
            /* [out] */ long *lFuelValue,
            /* [out] */ long *lPricePerLitre,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpTotals2 )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lPump,
            /* [out] */ short *nNozzle,
            /* [out] */ short *nGrade,
            /* [out] */ short *nTotalsFlags,
            /* [out] */ BSTR *sVolume,
            /* [out] */ BSTR *sValueA,
            /* [out] */ BSTR *sValueB,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDateTime,
            /* [out] */ long *lPumpStatus,
            /* [out] */ long *lNozzleStatus,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeliveryReport2 )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lTank,
            /* [out] */ long *lProduct,
            /* [out] */ long *lStatus,
            /* [out] */ short *nDeliveryFlags,
            /* [out] */ long *lTag,
            /* [out] */ long *lDeliveredVolume,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sStartDeliveryDateTime,
            /* [out] */ BSTR *sEndDeliveryDateTime,
            /* [out] */ long *lStartVolume,
            /* [out] */ long *lEndVolume,
            /* [out] */ long *lTemperature,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAlarm )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *plCatagory,
            /* [out] */ long *plCode,
            /* [out] */ long *plParameter,
            /* [out] */ BSTR *psStartDateAndTime,
            /* [out] */ BSTR *psEndDateAndTime,
            /* [out] */ long *plActiveReadNumber,
            /* [out] */ long *plShiftNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveAlarm )( 
            ICtrl * This,
            /* [in] */ short nFlags,
            /* [in] */ long lRecIndex,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetServiceStatus )( 
            ICtrl * This,
            /* [in] */ long lService,
            /* [out] */ long *lStatus,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StopService )( 
            ICtrl * This,
            /* [in] */ long lService,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StartService )( 
            ICtrl * This,
            /* [in] */ long lService,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWetStockData )( 
            ICtrl * This,
            /* [in] */ long lWetStockFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReConfigure )( 
            ICtrl * This,
            /* [in] */ long lConfigureFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPOSInfo )( 
            ICtrl * This,
            /* [out] */ BSTR *sPOSInfo,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMaintenance )( 
            ICtrl * This,
            /* [in] */ long lLock,
            /* [in] */ long lParam,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParam3 )( 
            ICtrl * This,
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [out][in] */ BSTR *sVal,
            /* [out][in] */ long *lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetParam3 )( 
            ICtrl * This,
            /* [in] */ BSTR sParamPosition,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetGradePrice2 )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ short nNozzle,
            /* [in] */ short nGrade,
            /* [in] */ long lMode,
            /* [in] */ short nLevel,
            /* [in] */ long lPrice,
            /* [in] */ BSTR sTimeValid,
            /* [in] */ short nFlags,
            /* [in] */ long lTaxRate,
            /* [in] */ long lDiscountPerUnit,
            /* [in] */ long lMaxUnitForDiscount,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpModeScheme2 )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [in] */ long lMode,
            /* [in] */ BSTR sPeriodDateTimeStart,
            /* [in] */ BSTR sPeriodDateTimeEnd,
            /* [in] */ BSTR sDateTimeStart,
            /* [in] */ BSTR sDateTimeEnd,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpModeScheme2 )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [out] */ long *plMode,
            /* [out] */ BSTR *psPeriodDateTimeStart,
            /* [out] */ BSTR *psPeriodDateTimeEnd,
            /* [out] */ BSTR *psDateTimeStart,
            /* [out] */ BSTR *psDateTimeEnd,
            /* [out] */ long *plFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetServiceFee )( 
            ICtrl * This,
            /* [in] */ long lPump,
            /* [in] */ long lTrsType,
            /* [in] */ long lTrsNumber,
            /* [in] */ BSTR stTime,
            /* [in] */ long lFlags,
            /* [out] */ long *pValue,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetServiceFee )( 
            ICtrl * This,
            /* [in] */ long lFeeNumber,
            /* [in] */ BSTR sName,
            /* [in] */ long lFlags,
            /* [in] */ long lPakFee,
            /* [in] */ long lPrePayFee,
            /* [in] */ long lPapFee,
            /* [in] */ long lAviFee,
            /* [in] */ long lAttendFee,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPumpModeScheme3 )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [in] */ long lMode,
            /* [in] */ BSTR sPeriodDateTimeStart,
            /* [in] */ BSTR sPeriodDateTimeEnd,
            /* [in] */ BSTR sDateTimeStart,
            /* [in] */ BSTR sDateTimeEnd,
            /* [in] */ long lServiceFeeNo,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpModeScheme3 )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lScheme,
            /* [out] */ long *plMode,
            /* [out] */ BSTR *psPeriodDateTimeStart,
            /* [out] */ BSTR *psPeriodDateTimeEnd,
            /* [out] */ BSTR *psDateTimeStart,
            /* [out] */ BSTR *psDateTimeEnd,
            /* [out] */ long *plServiceFeeNo,
            /* [out] */ long *plFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitEventsInterface )( 
            ICtrl * This,
            /* [in] */ long lFlags,
            /* [out][in] */ long *pClientId,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CloseEventsInterface )( 
            ICtrl * This,
            /* [in] */ long lClientId,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetServiceFee2 )( 
            ICtrl * This,
            /* [in] */ long lFeeNumber,
            /* [in] */ BSTR sName,
            /* [in] */ long lFlags,
            /* [in] */ long lPakFee,
            /* [in] */ long lPrePayFee,
            /* [in] */ long lPapFee,
            /* [in] */ long lAviFee,
            /* [in] */ long lAttendFee,
            /* [in] */ long lSelfPakFee,
            /* [in] */ long lSelfPrePayFee,
            /* [in] */ long lSelfPapFee,
            /* [in] */ long lSelfAviFee,
            /* [in] */ long lSelfAttendFee,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TLSCommand )( 
            ICtrl * This,
            /* [in] */ BSTR sCommand,
            /* [in] */ long lCommand,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResetQdxInfo )( 
            ICtrl * This,
            /* [in] */ long lFileNum,
            /* [in] */ long lPumpNum,
            /* [in] */ long lFlags1,
            /* [in] */ long lFlags2,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitDebit )( 
            ICtrl * This,
            /* [in] */ long lPumpGroup01,
            /* [in] */ long lPumpGroup2,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdateControllerSoftware )( 
            ICtrl * This,
            /* [in] */ BSTR sFileFullPath,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTankReading3 )( 
            ICtrl * This,
            /* [in] */ short nOpCode,
            /* [out][in] */ long *lIndexNumber,
            /* [out] */ long *lTank,
            /* [out] */ long *lProduct,
            /* [out] */ long *lStatus1,
            /* [out] */ short *nReadingFlags,
            /* [out] */ long *lFuelLevel,
            /* [out] */ long *lWaterLevel,
            /* [out] */ long *lTemperature,
            /* [out] */ long *lFuelVolume,
            /* [out] */ long *lWaterVolume,
            /* [out] */ long *lActiveReadNumber,
            /* [out] */ long *lShiftNumber,
            /* [out] */ BSTR *sDateTime,
            /* [out] */ long *lStatus2,
            /* [out] */ long *lFuelValue,
            /* [out] */ long *lPricePerLitre,
            /* [out] */ long *lTcVolume,
            /* [out] */ long *lUllage,
            /* [out][in] */ long *lFlags,
            /* [out] */ BSTR *sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReloadPricePolesData )( 
            ICtrl * This,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpDevices )( 
            ICtrl * This,
            /* [in] */ long lGrade,
            /* [in] */ long lNacsCode,
            /* [in] */ long lDevice,
            /* [in] */ long lFlags,
            /* [out] */ long *pVal,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTankInfo )( 
            ICtrl * This,
            /* [in] */ long lTankNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpTotalsInfo )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeliveryInfo )( 
            ICtrl * This,
            /* [in] */ long lTankNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Reconfigure2 )( 
            ICtrl * This,
            /* [in] */ long lTableId,
            /* [in] */ long lSubTableId,
            /* [in] */ long lSubTableIdExt,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GeneralSystemRequest )( 
            ICtrl * This,
            /* [in] */ long lVal,
            /* [in] */ BSTR sRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTankMemoryData )( 
            ICtrl * This,
            /* [in] */ long lTankNumber,
            /* [out] */ BSTR *pRec,
            /* [in] */ long lFlags,
            /* [out][in] */ long *lCounter,
            /* [out][retval] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DBTankGauge )( 
            ICtrl * This,
            /* [in] */ long lOffset,
            /* [out][in] */ BSTR *sRec,
            /* [in] */ long lRecSize,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TankGaugeData )( 
            ICtrl * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetConnectivityStatus )( 
            ICtrl * This,
            /* [out] */ BSTR *sPSConnStatus,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAccumulateTotals )( 
            ICtrl * This,
            /* [in] */ long lShiftNumber,
            /* [in] */ long lState,
            /* [in] */ long lPosNumber,
            /* [in] */ long lFlags,
            /* [in] */ long lVal,
            /* [in] */ BSTR sStartTime,
            /* [in] */ BSTR sEndTime,
            /* [out] */ long *lRecordCount,
            /* [out][in] */ BSTR *sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAccumulateState )( 
            ICtrl * This,
            /* [in] */ long lShiftNumber,
            /* [in] */ long lPosNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetModeDef2 )( 
            ICtrl * This,
            /* [in] */ long lMode,
            /* [in] */ BSTR sRec,
            /* [in] */ long lFlags,
            /* [in] */ long lMsgNumber,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetModeDef2 )( 
            ICtrl * This,
            /* [in] */ long lMode,
            /* [out] */ BSTR *psRec,
            /* [out] */ long *plFlags,
            /* [out] */ long *plMsgNumber,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetGeneralData )( 
            ICtrl * This,
            /* [in] */ long lRecordType,
            /* [out] */ BSTR *sData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetServiceFeeMode )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFeeSet,
            /* [in] */ long Flags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetServiceFeeMode )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [out] */ long *lFeeSet,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitParam )( 
            ICtrl * This,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *sPCIXml,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPaidTrs )( 
            ICtrl * This,
            /* [in] */ long lClientId,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [out][in] */ long *lCountRec,
            /* [retval][out] */ long *plRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpTotalsMemoData )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [out] */ long *lCounter,
            /* [out][in] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReceipt2 )( 
            ICtrl * This,
            /* [in] */ long lPosNumber,
            /* [in] */ BSTR sTrack2,
            /* [in] */ long lIndex,
            /* [out] */ BSTR *sReceiptData,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTankReadingEx )( 
            ICtrl * This,
            /* [in] */ long lOpCode,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeliveryReportEx )( 
            ICtrl * This,
            /* [in] */ long lOpCode,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsUnmanned )( 
            ICtrl * This,
            /* [out] */ BOOL *pbUnmanned,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            ICtrl * This,
            /* [out] */ BSTR *sVersionList,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetupEx )( 
            ICtrl * This,
            /* [in] */ BSTR sXmlRec,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StartQdxMaintenance )( 
            ICtrl * This,
            /* [in] */ long lFileNum,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetStationStatus )( 
            ICtrl * This,
            /* [out][in] */ long *lPumpMap1,
            /* [out][in] */ long *lPumpMap2,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeliveryEmulationStatus )( 
            ICtrl * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [out] */ long *plStatus,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChangeProductInTankStatus )( 
            ICtrl * This,
            /* [in] */ long lFlags,
            /* [out] */ long *plStatus,
            /* [out] */ long *plErrorCode,
            /* [out] */ BSTR *pbstrData,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProcessStatusEx )( 
            ICtrl * This,
            /* [in] */ long lFlags,
            /* [in] */ long lProcessType,
            /* [out] */ long *plStatus,
            /* [out] */ long *plErrorCode,
            /* [out] */ BSTR *pbstrData,
            /* [retval][out] */ long *plVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetParam4 )( 
            ICtrl * This,
            /* [in] */ BSTR sTableName,
            /* [in] */ long lKey1,
            /* [in] */ long lKey2,
            /* [in] */ BSTR sFiledName,
            /* [in] */ short nFiledType,
            /* [in] */ BSTR sVal,
            /* [in] */ long lVal,
            /* [in] */ BOOL bCreate,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPumpTotalsEx )( 
            ICtrl * This,
            /* [in] */ long lOpCode,
            /* [out][in] */ long *lIndexNumber,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lNozzle,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRollingUpdateInfo )( 
            ICtrl * This,
            /* [in] */ long lPumpNumber,
            /* [in] */ long lFlags,
            /* [out] */ BSTR *pbstrData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetAlarm )( 
            ICtrl * This,
            /* [in] */ long lCatagory,
            /* [in] */ long lSubCatagory,
            /* [in] */ long lAlarmNumber,
            /* [in] */ long lDevice,
            /* [in] */ long lAlarmStatus,
            /* [in] */ long lAlarmValue,
            /* [in] */ long lFlags,
            /* [out] */ long *pAlarmId,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetConfiguration )( 
            ICtrl * This,
            /* [in] */ BSTR sClientName,
            /* [out] */ BSTR *sConfigurationData,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTankState )( 
            ICtrl * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lState,
            /* [in] */ long lFlags,
            /* [in] */ BSTR sRec,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTankState )( 
            ICtrl * This,
            /* [in] */ long lTankNumber,
            /* [in] */ long lFlags,
            /* [out][in] */ BSTR *pRec,
            /* [retval][out] */ long *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SmartReconfigure )( 
            ICtrl * This,
            /* [in] */ long lDeviceNumberMask1,
            /* [in] */ long lDeviceNumberMask2,
            /* [in] */ long lTableId,
            /* [in] */ long lSubTableId,
            /* [in] */ long lSubTableIdExt,
            /* [in] */ long lFlags,
            /* [retval][out] */ long *pRetVal);
        
        END_INTERFACE
    } ICtrlVtbl;

    interface ICtrl
    {
        CONST_VTBL struct ICtrlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICtrl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICtrl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICtrl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICtrl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICtrl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICtrl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICtrl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ICtrl_SetPumpMode(This,lPumpNumber,lMode,pVal)	\
    ( (This)->lpVtbl -> SetPumpMode(This,lPumpNumber,lMode,pVal) ) 

#define ICtrl_GetPumpMode(This,lPumpNumber,lMode,pVal)	\
    ( (This)->lpVtbl -> GetPumpMode(This,lPumpNumber,lMode,pVal) ) 

#define ICtrl_SetGradePrice(This,lPumpNumber,nNozzle,nGrade,lMode,nLevel,lPrice,sTimeValid,nFlags,pVal)	\
    ( (This)->lpVtbl -> SetGradePrice(This,lPumpNumber,nNozzle,nGrade,lMode,nLevel,lPrice,sTimeValid,nFlags,pVal) ) 

#define ICtrl_GetGradePrice(This,lPumpNumber,nNozzle,nGrade,lMode,nLevel,lPrice,sDateTimeValid,pVal)	\
    ( (This)->lpVtbl -> GetGradePrice(This,lPumpNumber,nNozzle,nGrade,lMode,nLevel,lPrice,sDateTimeValid,pVal) ) 

#define ICtrl_SetShift(This,nDayOfWeek,nShiftNumber,nShiftFlags,sStartTime,pVal)	\
    ( (This)->lpVtbl -> SetShift(This,nDayOfWeek,nShiftNumber,nShiftFlags,sStartTime,pVal) ) 

#define ICtrl_GetShift(This,nDayOfWeek,nShiftNumber,nShiftFlags,sStartTime,pVal)	\
    ( (This)->lpVtbl -> GetShift(This,nDayOfWeek,nShiftNumber,nShiftFlags,sStartTime,pVal) ) 

#define ICtrl_GetPumpTotals(This,nFlags,lIndexNumber,lPump,nNozzle,nGrade,nTotalsFlags,sVolume,sValueA,sValueB,lActiveReadNumber,lShiftNumber,sDateTime,pVal)	\
    ( (This)->lpVtbl -> GetPumpTotals(This,nFlags,lIndexNumber,lPump,nNozzle,nGrade,nTotalsFlags,sVolume,sValueA,sValueB,lActiveReadNumber,lShiftNumber,sDateTime,pVal) ) 

#define ICtrl_RemovePumpTotals(This,nFlags,lRecIndex,pVal)	\
    ( (This)->lpVtbl -> RemovePumpTotals(This,nFlags,lRecIndex,pVal) ) 

#define ICtrl_GetTankReading(This,nFlags,lIndexNumber,lTank,lProduct,lStatus,nReadingFlags,lFuelLevel,lWaterLevel,lTemperature,lFuelVolume,lWaterVolume,lActiveReadNumber,lShiftNumber,sDateTime,pVal)	\
    ( (This)->lpVtbl -> GetTankReading(This,nFlags,lIndexNumber,lTank,lProduct,lStatus,nReadingFlags,lFuelLevel,lWaterLevel,lTemperature,lFuelVolume,lWaterVolume,lActiveReadNumber,lShiftNumber,sDateTime,pVal) ) 

#define ICtrl_RemoveTankReading(This,nFlags,lRecIndex,pVal)	\
    ( (This)->lpVtbl -> RemoveTankReading(This,nFlags,lRecIndex,pVal) ) 

#define ICtrl_GetDeliveryReport(This,nFlags,lIndexNumber,lTank,lProduct,lStatus,nDeliveryFlags,lTag,lDeliveredVolume,lActiveReadNumber,lShiftNumber,sDeliveryDateTime,pVal)	\
    ( (This)->lpVtbl -> GetDeliveryReport(This,nFlags,lIndexNumber,lTank,lProduct,lStatus,nDeliveryFlags,lTag,lDeliveredVolume,lActiveReadNumber,lShiftNumber,sDeliveryDateTime,pVal) ) 

#define ICtrl_RemoveDeliveryReport(This,nFlags,lRecIndex,pVal)	\
    ( (This)->lpVtbl -> RemoveDeliveryReport(This,nFlags,lRecIndex,pVal) ) 

#define ICtrl_SetRealTimeClock(This,sDateAndTime,pVal)	\
    ( (This)->lpVtbl -> SetRealTimeClock(This,sDateAndTime,pVal) ) 

#define ICtrl_ControlStatus(This,nFlags,lTotalIndex,lTankReadIndex,lDeliveryIndex,lActiveShiftNumber,nStationStatus,pVal)	\
    ( (This)->lpVtbl -> ControlStatus(This,nFlags,lTotalIndex,lTankReadIndex,lDeliveryIndex,lActiveShiftNumber,nStationStatus,pVal) ) 

#define ICtrl_GetProcessStatus(This,lProcessNumber,lProcessStatus,lPumpNumberA,lPumpProblemA,lPumpNumberB,lPumpProblemB,lTankNumber,lTankProblem,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetProcessStatus(This,lProcessNumber,lProcessStatus,lPumpNumberA,lPumpProblemA,lPumpNumberB,lPumpProblemB,lTankNumber,lTankProblem,lFlags,pVal) ) 

#define ICtrl_SetParam(This,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal)	\
    ( (This)->lpVtbl -> SetParam(This,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal) ) 

#define ICtrl_GetParam(This,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal)	\
    ( (This)->lpVtbl -> GetParam(This,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal) ) 

#define ICtrl_SetPumpModeScheme(This,lPumpNumber,lScheme,lMode,sDateTimeStart,sDateTimeEnd,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetPumpModeScheme(This,lPumpNumber,lScheme,lMode,sDateTimeStart,sDateTimeEnd,lFlags,pVal) ) 

#define ICtrl_GetPumpModeScheme(This,lPumpNumber,lScheme,plMode,psDateTimeStart,psDateTimeEnd,plFlags,pVal)	\
    ( (This)->lpVtbl -> GetPumpModeScheme(This,lPumpNumber,lScheme,plMode,psDateTimeStart,psDateTimeEnd,plFlags,pVal) ) 

#define ICtrl_SetModeDef(This,lMode,sName,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetModeDef(This,lMode,sName,lFlags,pVal) ) 

#define ICtrl_GetModeDef(This,lMode,psName,plFlags,pVal)	\
    ( (This)->lpVtbl -> GetModeDef(This,lMode,psName,plFlags,pVal) ) 

#define ICtrl_GetParam1(This,sTableAndFieldName,lKey1,lKey2,nFiledType,sVal,lVal,pVal)	\
    ( (This)->lpVtbl -> GetParam1(This,sTableAndFieldName,lKey1,lKey2,nFiledType,sVal,lVal,pVal) ) 

#define ICtrl_SetParam1(This,sTableAndFieldName,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal)	\
    ( (This)->lpVtbl -> SetParam1(This,sTableAndFieldName,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal) ) 

#define ICtrl_SetParam2(This,sTableFieldAndVal,lKey1,lKey2,nFiledType,lVal,bCreate,pVal)	\
    ( (This)->lpVtbl -> SetParam2(This,sTableFieldAndVal,lKey1,lKey2,nFiledType,lVal,bCreate,pVal) ) 

#define ICtrl_GetParam2(This,sTableFieldAndVal,lKey1,lKey2,nFiledType,lVal,pVal)	\
    ( (This)->lpVtbl -> GetParam2(This,sTableFieldAndVal,lKey1,lKey2,nFiledType,lVal,pVal) ) 

#define ICtrl_GetRecord(This,sTableAndVal,lKey1,lKey2,pVal)	\
    ( (This)->lpVtbl -> GetRecord(This,sTableAndVal,lKey1,lKey2,pVal) ) 

#define ICtrl_SetRecord(This,sTableAndVal,lKey1,lKey2,pVal)	\
    ( (This)->lpVtbl -> SetRecord(This,sTableAndVal,lKey1,lKey2,pVal) ) 

#define ICtrl_GetTankReading2(This,nFlags,lIndexNumber,lTank,lProduct,lStatus1,nReadingFlags,lFuelLevel,lWaterLevel,lTemperature,lFuelVolume,lWaterVolume,lActiveReadNumber,lShiftNumber,sDateTime,lStatus2,lFuelValue,lPricePerLitre,pVal)	\
    ( (This)->lpVtbl -> GetTankReading2(This,nFlags,lIndexNumber,lTank,lProduct,lStatus1,nReadingFlags,lFuelLevel,lWaterLevel,lTemperature,lFuelVolume,lWaterVolume,lActiveReadNumber,lShiftNumber,sDateTime,lStatus2,lFuelValue,lPricePerLitre,pVal) ) 

#define ICtrl_GetPumpTotals2(This,nFlags,lIndexNumber,lPump,nNozzle,nGrade,nTotalsFlags,sVolume,sValueA,sValueB,lActiveReadNumber,lShiftNumber,sDateTime,lPumpStatus,lNozzleStatus,pVal)	\
    ( (This)->lpVtbl -> GetPumpTotals2(This,nFlags,lIndexNumber,lPump,nNozzle,nGrade,nTotalsFlags,sVolume,sValueA,sValueB,lActiveReadNumber,lShiftNumber,sDateTime,lPumpStatus,lNozzleStatus,pVal) ) 

#define ICtrl_GetDeliveryReport2(This,nFlags,lIndexNumber,lTank,lProduct,lStatus,nDeliveryFlags,lTag,lDeliveredVolume,lActiveReadNumber,lShiftNumber,sStartDeliveryDateTime,sEndDeliveryDateTime,lStartVolume,lEndVolume,lTemperature,pVal)	\
    ( (This)->lpVtbl -> GetDeliveryReport2(This,nFlags,lIndexNumber,lTank,lProduct,lStatus,nDeliveryFlags,lTag,lDeliveredVolume,lActiveReadNumber,lShiftNumber,sStartDeliveryDateTime,sEndDeliveryDateTime,lStartVolume,lEndVolume,lTemperature,pVal) ) 

#define ICtrl_GetAlarm(This,nFlags,lIndexNumber,plCatagory,plCode,plParameter,psStartDateAndTime,psEndDateAndTime,plActiveReadNumber,plShiftNumber,pVal)	\
    ( (This)->lpVtbl -> GetAlarm(This,nFlags,lIndexNumber,plCatagory,plCode,plParameter,psStartDateAndTime,psEndDateAndTime,plActiveReadNumber,plShiftNumber,pVal) ) 

#define ICtrl_RemoveAlarm(This,nFlags,lRecIndex,pVal)	\
    ( (This)->lpVtbl -> RemoveAlarm(This,nFlags,lRecIndex,pVal) ) 

#define ICtrl_GetServiceStatus(This,lService,lStatus,pVal)	\
    ( (This)->lpVtbl -> GetServiceStatus(This,lService,lStatus,pVal) ) 

#define ICtrl_StopService(This,lService,pVal)	\
    ( (This)->lpVtbl -> StopService(This,lService,pVal) ) 

#define ICtrl_StartService(This,lService,pVal)	\
    ( (This)->lpVtbl -> StartService(This,lService,pVal) ) 

#define ICtrl_GetWetStockData(This,lWetStockFlags,pVal)	\
    ( (This)->lpVtbl -> GetWetStockData(This,lWetStockFlags,pVal) ) 

#define ICtrl_ReConfigure(This,lConfigureFlags,pVal)	\
    ( (This)->lpVtbl -> ReConfigure(This,lConfigureFlags,pVal) ) 

#define ICtrl_GetPOSInfo(This,sPOSInfo,pVal)	\
    ( (This)->lpVtbl -> GetPOSInfo(This,sPOSInfo,pVal) ) 

#define ICtrl_SetMaintenance(This,lLock,lParam,pVal)	\
    ( (This)->lpVtbl -> SetMaintenance(This,lLock,lParam,pVal) ) 

#define ICtrl_GetParam3(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal)	\
    ( (This)->lpVtbl -> GetParam3(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal) ) 

#define ICtrl_SetParam3(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal)	\
    ( (This)->lpVtbl -> SetParam3(This,sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal) ) 

#define ICtrl_SetGradePrice2(This,lPumpNumber,nNozzle,nGrade,lMode,nLevel,lPrice,sTimeValid,nFlags,lTaxRate,lDiscountPerUnit,lMaxUnitForDiscount,pVal)	\
    ( (This)->lpVtbl -> SetGradePrice2(This,lPumpNumber,nNozzle,nGrade,lMode,nLevel,lPrice,sTimeValid,nFlags,lTaxRate,lDiscountPerUnit,lMaxUnitForDiscount,pVal) ) 

#define ICtrl_SetPumpModeScheme2(This,lPumpNumber,lScheme,lMode,sPeriodDateTimeStart,sPeriodDateTimeEnd,sDateTimeStart,sDateTimeEnd,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetPumpModeScheme2(This,lPumpNumber,lScheme,lMode,sPeriodDateTimeStart,sPeriodDateTimeEnd,sDateTimeStart,sDateTimeEnd,lFlags,pVal) ) 

#define ICtrl_GetPumpModeScheme2(This,lPumpNumber,lScheme,plMode,psPeriodDateTimeStart,psPeriodDateTimeEnd,psDateTimeStart,psDateTimeEnd,plFlags,pVal)	\
    ( (This)->lpVtbl -> GetPumpModeScheme2(This,lPumpNumber,lScheme,plMode,psPeriodDateTimeStart,psPeriodDateTimeEnd,psDateTimeStart,psDateTimeEnd,plFlags,pVal) ) 

#define ICtrl_GetServiceFee(This,lPump,lTrsType,lTrsNumber,stTime,lFlags,pValue,pRetVal)	\
    ( (This)->lpVtbl -> GetServiceFee(This,lPump,lTrsType,lTrsNumber,stTime,lFlags,pValue,pRetVal) ) 

#define ICtrl_SetServiceFee(This,lFeeNumber,sName,lFlags,lPakFee,lPrePayFee,lPapFee,lAviFee,lAttendFee,pRetVal)	\
    ( (This)->lpVtbl -> SetServiceFee(This,lFeeNumber,sName,lFlags,lPakFee,lPrePayFee,lPapFee,lAviFee,lAttendFee,pRetVal) ) 

#define ICtrl_SetPumpModeScheme3(This,lPumpNumber,lScheme,lMode,sPeriodDateTimeStart,sPeriodDateTimeEnd,sDateTimeStart,sDateTimeEnd,lServiceFeeNo,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetPumpModeScheme3(This,lPumpNumber,lScheme,lMode,sPeriodDateTimeStart,sPeriodDateTimeEnd,sDateTimeStart,sDateTimeEnd,lServiceFeeNo,lFlags,pVal) ) 

#define ICtrl_GetPumpModeScheme3(This,lPumpNumber,lScheme,plMode,psPeriodDateTimeStart,psPeriodDateTimeEnd,psDateTimeStart,psDateTimeEnd,plServiceFeeNo,plFlags,pVal)	\
    ( (This)->lpVtbl -> GetPumpModeScheme3(This,lPumpNumber,lScheme,plMode,psPeriodDateTimeStart,psPeriodDateTimeEnd,psDateTimeStart,psDateTimeEnd,plServiceFeeNo,plFlags,pVal) ) 

#define ICtrl_InitEventsInterface(This,lFlags,pClientId,pVal)	\
    ( (This)->lpVtbl -> InitEventsInterface(This,lFlags,pClientId,pVal) ) 

#define ICtrl_CloseEventsInterface(This,lClientId,pVal)	\
    ( (This)->lpVtbl -> CloseEventsInterface(This,lClientId,pVal) ) 

#define ICtrl_SetServiceFee2(This,lFeeNumber,sName,lFlags,lPakFee,lPrePayFee,lPapFee,lAviFee,lAttendFee,lSelfPakFee,lSelfPrePayFee,lSelfPapFee,lSelfAviFee,lSelfAttendFee,pRetVal)	\
    ( (This)->lpVtbl -> SetServiceFee2(This,lFeeNumber,sName,lFlags,lPakFee,lPrePayFee,lPapFee,lAviFee,lAttendFee,lSelfPakFee,lSelfPrePayFee,lSelfPapFee,lSelfAviFee,lSelfAttendFee,pRetVal) ) 

#define ICtrl_TLSCommand(This,sCommand,lCommand,lTankNumber,lFlags,plRetVal)	\
    ( (This)->lpVtbl -> TLSCommand(This,sCommand,lCommand,lTankNumber,lFlags,plRetVal) ) 

#define ICtrl_ResetQdxInfo(This,lFileNum,lPumpNum,lFlags1,lFlags2,pVal)	\
    ( (This)->lpVtbl -> ResetQdxInfo(This,lFileNum,lPumpNum,lFlags1,lFlags2,pVal) ) 

#define ICtrl_InitDebit(This,lPumpGroup01,lPumpGroup2,lFlags,pVal)	\
    ( (This)->lpVtbl -> InitDebit(This,lPumpGroup01,lPumpGroup2,lFlags,pVal) ) 

#define ICtrl_UpdateControllerSoftware(This,sFileFullPath,lFlags,pVal)	\
    ( (This)->lpVtbl -> UpdateControllerSoftware(This,sFileFullPath,lFlags,pVal) ) 

#define ICtrl_GetTankReading3(This,nOpCode,lIndexNumber,lTank,lProduct,lStatus1,nReadingFlags,lFuelLevel,lWaterLevel,lTemperature,lFuelVolume,lWaterVolume,lActiveReadNumber,lShiftNumber,sDateTime,lStatus2,lFuelValue,lPricePerLitre,lTcVolume,lUllage,lFlags,sRec,pVal)	\
    ( (This)->lpVtbl -> GetTankReading3(This,nOpCode,lIndexNumber,lTank,lProduct,lStatus1,nReadingFlags,lFuelLevel,lWaterLevel,lTemperature,lFuelVolume,lWaterVolume,lActiveReadNumber,lShiftNumber,sDateTime,lStatus2,lFuelValue,lPricePerLitre,lTcVolume,lUllage,lFlags,sRec,pVal) ) 

#define ICtrl_ReloadPricePolesData(This,lFlags,pVal)	\
    ( (This)->lpVtbl -> ReloadPricePolesData(This,lFlags,pVal) ) 

#define ICtrl_GetPumpDevices(This,lGrade,lNacsCode,lDevice,lFlags,pVal,pRetVal)	\
    ( (This)->lpVtbl -> GetPumpDevices(This,lGrade,lNacsCode,lDevice,lFlags,pVal,pRetVal) ) 

#define ICtrl_GetTankInfo(This,lTankNumber,pRec,lFlags,pRetVal)	\
    ( (This)->lpVtbl -> GetTankInfo(This,lTankNumber,pRec,lFlags,pRetVal) ) 

#define ICtrl_GetPumpTotalsInfo(This,lPumpNumber,lFlags,pRec,pRetVal)	\
    ( (This)->lpVtbl -> GetPumpTotalsInfo(This,lPumpNumber,lFlags,pRec,pRetVal) ) 

#define ICtrl_GetDeliveryInfo(This,lTankNumber,pRec,lFlags,pRetVal)	\
    ( (This)->lpVtbl -> GetDeliveryInfo(This,lTankNumber,pRec,lFlags,pRetVal) ) 

#define ICtrl_Reconfigure2(This,lTableId,lSubTableId,lSubTableIdExt,lFlags,pRetVal)	\
    ( (This)->lpVtbl -> Reconfigure2(This,lTableId,lSubTableId,lSubTableIdExt,lFlags,pRetVal) ) 

#define ICtrl_GeneralSystemRequest(This,lVal,sRec,lFlags,plVal)	\
    ( (This)->lpVtbl -> GeneralSystemRequest(This,lVal,sRec,lFlags,plVal) ) 

#define ICtrl_GetTankMemoryData(This,lTankNumber,pRec,lFlags,lCounter,pVal)	\
    ( (This)->lpVtbl -> GetTankMemoryData(This,lTankNumber,pRec,lFlags,lCounter,pVal) ) 

#define ICtrl_DBTankGauge(This,lOffset,sRec,lRecSize,lFlags,pVal)	\
    ( (This)->lpVtbl -> DBTankGauge(This,lOffset,sRec,lRecSize,lFlags,pVal) ) 

#define ICtrl_TankGaugeData(This,lTankNumber,lFlags,sRec,pVal)	\
    ( (This)->lpVtbl -> TankGaugeData(This,lTankNumber,lFlags,sRec,pVal) ) 

#define ICtrl_GetConnectivityStatus(This,sPSConnStatus,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetConnectivityStatus(This,sPSConnStatus,lFlags,pVal) ) 

#define ICtrl_GetAccumulateTotals(This,lShiftNumber,lState,lPosNumber,lFlags,lVal,sStartTime,sEndTime,lRecordCount,sRec,pVal)	\
    ( (This)->lpVtbl -> GetAccumulateTotals(This,lShiftNumber,lState,lPosNumber,lFlags,lVal,sStartTime,sEndTime,lRecordCount,sRec,pVal) ) 

#define ICtrl_SetAccumulateState(This,lShiftNumber,lPosNumber,lState,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetAccumulateState(This,lShiftNumber,lPosNumber,lState,lFlags,pVal) ) 

#define ICtrl_SetModeDef2(This,lMode,sRec,lFlags,lMsgNumber,plVal)	\
    ( (This)->lpVtbl -> SetModeDef2(This,lMode,sRec,lFlags,lMsgNumber,plVal) ) 

#define ICtrl_GetModeDef2(This,lMode,psRec,plFlags,plMsgNumber,pVal)	\
    ( (This)->lpVtbl -> GetModeDef2(This,lMode,psRec,plFlags,plMsgNumber,pVal) ) 

#define ICtrl_GetGeneralData(This,lRecordType,sData,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetGeneralData(This,lRecordType,sData,lFlags,pVal) ) 

#define ICtrl_SetServiceFeeMode(This,lPumpNumber,lFeeSet,Flags,pVal)	\
    ( (This)->lpVtbl -> SetServiceFeeMode(This,lPumpNumber,lFeeSet,Flags,pVal) ) 

#define ICtrl_GetServiceFeeMode(This,lPumpNumber,lFeeSet,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetServiceFeeMode(This,lPumpNumber,lFeeSet,lFlags,pVal) ) 

#define ICtrl_InitParam(This,lFlags,sPCIXml,pVal)	\
    ( (This)->lpVtbl -> InitParam(This,lFlags,sPCIXml,pVal) ) 

#define ICtrl_GetPaidTrs(This,lClientId,lFlags,pRec,lCountRec,plRetVal)	\
    ( (This)->lpVtbl -> GetPaidTrs(This,lClientId,lFlags,pRec,lCountRec,plRetVal) ) 

#define ICtrl_GetPumpTotalsMemoData(This,lPumpNumber,lFlags,pRec,lCounter,pRetVal)	\
    ( (This)->lpVtbl -> GetPumpTotalsMemoData(This,lPumpNumber,lFlags,pRec,lCounter,pRetVal) ) 

#define ICtrl_GetReceipt2(This,lPosNumber,sTrack2,lIndex,sReceiptData,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetReceipt2(This,lPosNumber,sTrack2,lIndex,sReceiptData,lFlags,pVal) ) 

#define ICtrl_GetTankReadingEx(This,lOpCode,lIndexNumber,lTankNumber,lFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> GetTankReadingEx(This,lOpCode,lIndexNumber,lTankNumber,lFlags,pRec,pVal) ) 

#define ICtrl_GetDeliveryReportEx(This,lOpCode,lIndexNumber,lFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> GetDeliveryReportEx(This,lOpCode,lIndexNumber,lFlags,pRec,pVal) ) 

#define ICtrl_IsUnmanned(This,pbUnmanned,pVal)	\
    ( (This)->lpVtbl -> IsUnmanned(This,pbUnmanned,pVal) ) 

#define ICtrl_GetVersion(This,sVersionList,lFlags,pVal)	\
    ( (This)->lpVtbl -> GetVersion(This,sVersionList,lFlags,pVal) ) 

#define ICtrl_SetupEx(This,sXmlRec,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetupEx(This,sXmlRec,lFlags,pVal) ) 

#define ICtrl_StartQdxMaintenance(This,lFileNum,pVal)	\
    ( (This)->lpVtbl -> StartQdxMaintenance(This,lFileNum,pVal) ) 

#define ICtrl_GetStationStatus(This,lPumpMap1,lPumpMap2,lFlags,pRetVal)	\
    ( (This)->lpVtbl -> GetStationStatus(This,lPumpMap1,lPumpMap2,lFlags,pRetVal) ) 

#define ICtrl_GetDeliveryEmulationStatus(This,lTankNumber,lFlags,plStatus,pRec,pRetVal)	\
    ( (This)->lpVtbl -> GetDeliveryEmulationStatus(This,lTankNumber,lFlags,plStatus,pRec,pRetVal) ) 

#define ICtrl_GetChangeProductInTankStatus(This,lFlags,plStatus,plErrorCode,pbstrData,plVal)	\
    ( (This)->lpVtbl -> GetChangeProductInTankStatus(This,lFlags,plStatus,plErrorCode,pbstrData,plVal) ) 

#define ICtrl_GetProcessStatusEx(This,lFlags,lProcessType,plStatus,plErrorCode,pbstrData,plVal)	\
    ( (This)->lpVtbl -> GetProcessStatusEx(This,lFlags,lProcessType,plStatus,plErrorCode,pbstrData,plVal) ) 

#define ICtrl_SetParam4(This,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,lFlags,pVal)	\
    ( (This)->lpVtbl -> SetParam4(This,sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,lFlags,pVal) ) 

#define ICtrl_GetPumpTotalsEx(This,lOpCode,lIndexNumber,lPumpNumber,lNozzle,lFlags,pRec,pVal)	\
    ( (This)->lpVtbl -> GetPumpTotalsEx(This,lOpCode,lIndexNumber,lPumpNumber,lNozzle,lFlags,pRec,pVal) ) 

#define ICtrl_GetRollingUpdateInfo(This,lPumpNumber,lFlags,pbstrData,pVal)	\
    ( (This)->lpVtbl -> GetRollingUpdateInfo(This,lPumpNumber,lFlags,pbstrData,pVal) ) 

#define ICtrl_SetAlarm(This,lCatagory,lSubCatagory,lAlarmNumber,lDevice,lAlarmStatus,lAlarmValue,lFlags,pAlarmId,sRec,pVal)	\
    ( (This)->lpVtbl -> SetAlarm(This,lCatagory,lSubCatagory,lAlarmNumber,lDevice,lAlarmStatus,lAlarmValue,lFlags,pAlarmId,sRec,pVal) ) 

#define ICtrl_GetConfiguration(This,sClientName,sConfigurationData,pVal)	\
    ( (This)->lpVtbl -> GetConfiguration(This,sClientName,sConfigurationData,pVal) ) 

#define ICtrl_SetTankState(This,lTankNumber,lState,lFlags,sRec,pRetVal)	\
    ( (This)->lpVtbl -> SetTankState(This,lTankNumber,lState,lFlags,sRec,pRetVal) ) 

#define ICtrl_GetTankState(This,lTankNumber,lFlags,pRec,pRetVal)	\
    ( (This)->lpVtbl -> GetTankState(This,lTankNumber,lFlags,pRec,pRetVal) ) 

#define ICtrl_SmartReconfigure(This,lDeviceNumberMask1,lDeviceNumberMask2,lTableId,lSubTableId,lSubTableIdExt,lFlags,pRetVal)	\
    ( (This)->lpVtbl -> SmartReconfigure(This,lDeviceNumberMask1,lDeviceNumberMask2,lTableId,lSubTableId,lSubTableIdExt,lFlags,pRetVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICtrl_INTERFACE_DEFINED__ */



#ifndef __PUMPSRVLib_LIBRARY_DEFINED__
#define __PUMPSRVLib_LIBRARY_DEFINED__

/* library PUMPSRVLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_PUMPSRVLib;

#ifndef __ICtrlEvents_DISPINTERFACE_DEFINED__
#define __ICtrlEvents_DISPINTERFACE_DEFINED__

/* dispinterface ICtrlEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_ICtrlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("DCCF1278-5F73-4a6e-BC58-68A37316612D")
    ICtrlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ICtrlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICtrlEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICtrlEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICtrlEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICtrlEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICtrlEvents * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } ICtrlEventsVtbl;

    interface ICtrlEvents
    {
        CONST_VTBL struct ICtrlEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICtrlEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICtrlEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICtrlEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICtrlEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICtrlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICtrlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICtrlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ICtrlEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IGPIEvents_DISPINTERFACE_DEFINED__
#define __IGPIEvents_DISPINTERFACE_DEFINED__

/* dispinterface IGPIEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_IGPIEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("3059961A-1632-44C7-A0E8-D0E75270FC81")
    IGPIEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IGPIEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGPIEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGPIEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGPIEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGPIEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGPIEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGPIEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGPIEvents * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IGPIEventsVtbl;

    interface IGPIEvents
    {
        CONST_VTBL struct IGPIEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGPIEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGPIEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGPIEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGPIEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IGPIEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IGPIEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IGPIEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IGPIEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_GPI;

#ifdef __cplusplus

class DECLSPEC_UUID("7C9373A2-1D37-11D1-B8A5-0000E8D0CBF8")
GPI;
#endif

EXTERN_C const CLSID CLSID_GCI;

#ifdef __cplusplus

class DECLSPEC_UUID("7C9373A4-1D37-11D1-B8A5-0000E8D0CBF8")
GCI;
#endif

EXTERN_C const CLSID CLSID_Ctrl;

#ifdef __cplusplus

class DECLSPEC_UUID("43464622-C31A-11D1-8625-0000E81A8D8B")
Ctrl;
#endif
#endif /* __PUMPSRVLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


