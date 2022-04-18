	
// GPI.h : Declaration of the CGPI

#ifndef __GPI_H_
#define __GPI_H_

#include "resource.h"       // main symbols
#include "PumpSrvCP.h"




/////////////////////////////////////////////////////////////////////////////
// CGPI
class ATL_NO_VTABLE CGPI : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CGPI, &CLSID_GPI>,
	public IDispatchImpl<IGPI, &IID_IGPI, &LIBID_PUMPSRVLib>,
	public IConnectionPointContainerImpl<CGPI>,
	public CProxyIGPIEvents< CGPI >
{
public:
	CGPI()
	{
		m_lPosNumber	= NO_POS_NUMBER;
//		m_pUnkMarshaler = NULL;
		m_bInitDone		= FALSE;
	}

	~CGPI()
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			_Module.m_server.m_cLogger.LogReq(0,"IGPI", "Release", "disconnected from server"); 				
//			_Module.m_server.m_cLogger.LogReq(m_lPosNumber,"IGPI", "Release", "disconnected from server"); 				
		}	
	}


DECLARE_REGISTRY_RESOURCEID(IDR_GPI)
DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(CGPI)
	COM_INTERFACE_ENTRY(IGPI)
	COM_INTERFACE_ENTRY(IDispatch)
//	COM_INTERFACE_ENTRY(ISupportErrorInfo)
//	COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

//	HRESULT FinalConstruct()
//	{
//		return CoCreateFreeThreadedMarshaler(
//			GetControllingUnknown(), &m_pUnkMarshaler.p);
//	}

//	void FinalRelease()
//	{
//		m_pUnkMarshaler.Release();
//	}

	void FinalRelease()
	{
		long lRetVal = 0; 
		if(_Module.m_server.m_bControllerManagePrinterStatus)
		{
			SetProcess(PROCESS_PRINTER_STATUS, m_lPosNumber, PRINTER_OFFLINE, &lRetVal);
			if( lRetVal)
			{
				CString sMsg;   //4.0.5.46
				sMsg.Format("GPI::FinalRelease: SetProcess with PROCESS_PRINTER_STATUS has failed. Return Value:%d ", lRetVal);
				_Module.m_server.m_cLogger.LogMsg(sMsg); 
			}
		}
	}

//	CComPtr<IUnknown> m_pUnkMarshaler;

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IGPI
public:
	STDMETHOD(SetTrsState2)(/*[in]*/ long lPumpNumber, /*[in]*/long lTrsNumber,/*[in]*/ long lState, /*[in]*/ long  lFlags,/*[out , retval]*/ long *pVal );
	STDMETHOD(AddItem2Trs)(/*[in]*/ long lPumpNumber , /*[in]*/ long lItemSelected , /*[in]*/ long  lFlags, /*[out , retval]*/ long * plRetVal);
	STDMETHOD(GetAllTrsQuery)(/*[in]*/ long lPumpNumber,/*[out]*/ BSTR * sData , /*[in , out]*/ long * plRecCount , /*[in]*/ long lFlags , /*[out , retval]*/ long * plRetVal);
	STDMETHOD(GetPumpGradeInfo)(/*[in]*/ long lPump, /*[out]*/ BSTR * sGradesData, /*[out]*/ long * lRecordcount, /*[in]*/long lFlags, /*[out,retval]*/long * plVal);
	STDMETHOD(GetReceiptRawData)(/*[in]*/long lPumpNumber, /*[in]*/ BSTR sTrack2, /*[in , out]*/ short *nIndex, /*[out]*/ BSTR *sReceiptRawData, /*[in,out]*/ long *plFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetReport)(/*[in]*/long lNumber, /*[in]*/long lType, /*[out]*/BSTR * sReportData,/*[in]*/long lFlags,/*[out,retval]*/long* lRet);
	STDMETHOD(CancelWashCode2)(/*[in,out]*/ long * plSyncNumber,/*[in]*/ long lDevice,/*[in]*/ long lFlags,/*[in]*/ BSTR sData,/*[in]*/ long lRefundCarWash ,/*[out, retval]*/ long *pVal);
	STDMETHOD(GetVersion)(/*[in]*/ BSTR *sVersionList, long lFlags ,long *pVal);
	STDMETHOD(SetPrePayTrsState)(/*[in]*/ long lPumpNumber, /*[in]*/ long lSyncNumber, /*[in]*/ long lStatus, /*[in]*/ long lFlags, /*[out,retval]*/ long *plVal);
	STDMETHOD(SetAccumulateState)(/*[in]*/ long lShiftNumber , /*[in]*/ long lState,/*[in]*/ long lFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetAccumulateTotals)(long lShiftNumber ,  long lState, long lPosNumber, long lFlags,  long lVal, BSTR sStartTime, BSTR sEndTime, long *lRecordCount,   BSTR* sRec, long *pVal);
	STDMETHOD(LockPump)(/*[in]*/ long lPumpNumber, /*[in]*/ long lLock,/*[in]*/ long lFlags, /*[out,retval]*/ long *plVal);
	STDMETHOD(GetPAPTrs4)(/*[in]*/ long lPumpNumber, /*[out]*/  BSTR *pRec,  /*[in]*/ long lFlags,/*[out,retval]*/  long *pVal);
	STDMETHOD(GetPAKQuery)(/*[out]*/ BSTR * sData , /*[in , out]*/ long * plRecCount , /*[in]*/ long lFlags , /*[out , retval]*/ long * plRetVal);
	STDMETHOD(GetAlarmById)(/*[in]*/long lAlarmId,/*[in]*/ long lFlags, /*[out]*/ BSTR *pData, /*[out, retval]*/ long *plVal);
	void GetAlarmByCategory__(long lCategory, long lSubCategory, long lDevice, long lFlags, long *plTotalAlarms, BSTR *pData, long *plVal);
	STDMETHOD(GetAlarmByCategory)(/*[in]*/ long  lCategory, /*[in]*/  long lSubCategory ,/*[in]*/ long lDevice, /*[in]*/ long lFlags, /*[out]*/ long  *plTotalAlarms, /*[out]*/ BSTR *pData, /*[out ,retval]*/ long *plVal);
	STDMETHOD(GetPumpModeScheme3)(/*[in]*/ long lPumpNumber, /*[in]*/ long lScheme, /*[out]*/  long  *plMode , /*[out]*/   BSTR *psPeriodDateTimeStart, /*[out]*/  BSTR *psPeriodDateTimeEnd, /*[out]*/   BSTR *psDateTimeStart, /*[out]*/ BSTR *psDateTimeEnd, /*[out]*/ long *plServiceFeeNo, /*[out]*/   long *plFlags, /*[out, retval]*/  long *pVal);
	STDMETHOD(SetPumpModeScheme3)(/*[in]*/  long lPumpNumber,  /*[in]*/  long lScheme,  /*[in]*/ long lMode, /*[in]*/  BSTR sPeriodDateTimeStart, /*[in]*/  BSTR sPeriodDateTimeEnd, /*[in]*/  BSTR sDateTimeStart, /*[in]*/  BSTR sDateTimeEnd,  /*[in]*/ long lServiceFeeNo, /*[in]*/  long lFlags,  /*[out, retval]*/  long *pVal);
	STDMETHOD(AckAlarm)(/*[in]*/long lCategory, /*[in]*/  long lSubCategory , /*[in,]*/long lAlarmId,/*[in]*/ long lDevice, /*[in]*/ long lAlarmNumber, /*[in]*/long lCustomerId, /*[in]*/ long lFlags, /*[in]*/ BSTR sData, /*[out ,retval]*/ long *plVal );
	STDMETHOD(GetAlarmInfo)(/*[in]*/ long lCategory, /*[in]*/ long lSubCategory, /*[in]*/ long lDevice, /*[in,out]*/long *plAlarmId ,/*[in]*/ long lFlags, /*[out]*/  BSTR *pData, /*[out ,retval]*/ long *plVal);
	STDMETHOD(GetSingleProductInfo)(/*[in]*/long lPump , /*[in]*/ long lFlags ,/*[in,out]*/ long *lGrade,/*[out, retval]*/ long *pVal);
	STDMETHOD(SwitchServer)(/*[in]*/ long lNewServerType , /*[out , retval]*/ long * plRetVal);
	STDMETHOD(QueryReceipt)(/*[in]*/ long lPosNumber , /*[in]*/ BSTR sTrack2 , /*[in , out]*/ short * nIndex , /*[in]*/ short nFlags , /*[out]*/ BSTR * pData , /*[out , retval]*/ long  * lRetVal);
	STDMETHOD(ReceiptPrinted2)(/*[in]*/ BSTR sTrack2, /*[in]*/ long lIndex, /*[in]*/ long lFlags ,/*[out, retval]*/ long *pVal);
	STDMETHOD(GetReceipt2)(/*[in]*/long lPosNumber, /*[in]*/ BSTR sTrack2, /*[in]*/ long lIndex,  /*[out]*/ BSTR *sReceiptData, /*[in]*/ long lFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(QueryExtraInfoData)(/*[in]*/ long lPumpNumber, /*[in,out]*/ long *lIndexNumber, /*[in]*/ short nFlags, /*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(AuthorizeAndLimit2)(/*[in]*/ long lPumpNumber, /*[in]*/ short nLevel, /*[in]*/ long lGradePrice, /*[in]*/ long lMaxValue, /*[in]*/ long lMaxVolume, /*[in]*/ long lFlags, /*[in,out]*/ long *lSyncNumber ,/*[in]*/ BSTR sRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetStationStatus)(/*[in , out]*/ long * lPumpMap1 , /*[in , out]*/ long * lPumpMap2 , /*[in]*/ long lFlags , /*[out , retval]*/ long * pRetVal);
	STDMETHOD(SetTransactionState)(/*[in]*/ long lPumpNumber , /*[in]*/ BSTR pRec ,/*[in]*/long lFlags , /*[out , retval]*/ long *pVal);
	STDMETHOD(GetTankInfo)(/*[in]*/ long lTankNumber, /*[out]*/ BSTR *pRec, /*[in]*/ long lFlags, /*[out , retval]*/ long *pVal);
	STDMETHOD(GetOlaMemInfoStatus)(/*[in]*/ long lFlags , /*[out]*/ BSTR * pRec, /*[out, retval]*/ long *pVal);
	STDMETHOD(SetMemOla)(/*[in]*/long lPump , /*[in]*/ long lTrsNumber , /*[in]*/ long lFlags, /*[in]*/ long lVal , /*[out,retval]*/ long  *pVal);
	STDMETHOD(GetOLAMemInfo2)(/*[in]*/ long lPumpNumber , /*[in,out]*/ short *nIndex , /*[in]*/  long lFlags, /*[in]*/ long lVal,/*[in]*/ BSTR sVal, /*[out]*/  BSTR *pRec, /*[out,retval]*/ long  *pVal);
	STDMETHOD(GetLevelPrice)(/*[in]*/ long lPumpNumber, /*[in]*/ short nGrade, /*[in]*/ short nLevel, /*[in]*/ long lServiceMode, /*[in]*/ long lFlags, /*[out]*/ long *lPrice, /*[out , retval]*/ long *pVal);
	STDMETHOD(GetGradePrices)(/*[in]*/ short nGrade, /*[in]*/ long lFlags, /*[out]*/ long *lCashFullPrice, /*[out]*/ long *lCashSelfPrice, /*[out]*/ long *lCreditFullPrice, /*[out]*/ long *lCreditSelfPrice, /*[out , retval]*/ long *pVal);
	STDMETHOD(GetMinimalPumpsInfo3)(/*[out]*/ BSTR * pRec,  /*[in]*/ long lFlags, /*[out,retval]*/ long * pVal);
	STDMETHOD(GetServiceFee)(/*[in]*/ long lPump, /*[in]*/ long lTrsType, /*[in]*/ long lTrsNumber,/*[in]*/ BSTR stTime, /*[in]*/ long lFlags, /*[out]*/ long* pValue,    /*[out, retval]*/long *pRetVal);
	STDMETHOD(GetPumpInfo3)(/*[in]*/ long lPumpNumber, /*[out]*/ BSTR *pRec, /*[in]*/ long lFlags, /*[out , retval]*/ long *pVal);
	STDMETHOD(GetOLAMemInfo)(/*[in]*/ long lPumpNumber , /*[in]*/short nIndex ,/*[in]*/ long lFlags, /*[out]*/ BSTR *pRec, /*[out,retval]*/ long  *pVal);
	STDMETHOD(ReceiptPrinted)(/*[in]*/ BSTR sTrack2, /*[in]*/ short nIndex ,/*[out, retval]*/  long  *pVal);
	STDMETHOD(GetReceipt)(/*[in]*/ long  lPumpNumber, /*[in]*/ BSTR sTrack2, /*[in]*/ short nIndex, /*[out]*/ BSTR *sReceiptData, /*[in]*/ long lFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(QueryPAPTrs2)(/*[in]*/ long lPumpNumber, /*[in]*/ long lIndexNumber, /*[in]*/ short nFlags, /*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPAPTrs3)(/*[in]*/ long lPumpNumber, /*[out]*/  BSTR *pRec,  /*[out,retval]*/  long *pVal);
	STDMETHOD(StopPump2)(/*[in]*/ long lPumpNumber,/*[in]*/ short nGroup,/*[out, retval]*/ long * pVal);
	STDMETHOD(AuthorizePump2)(/*[in]*/ long lPumpNumber,/*[in]*/ short nLevel,/*[in]*/ long lGradePrice,/*[in]*/ short lFlags,/*[in]*/ long lLimit,/*[in]*/ short nGroup, /*[out, retval]*/ long *pVal);
	STDMETHOD(CancelWashCode)(/*[in]*/ long lSyncNumber,/*[in]*/ long lDevice, /*[in]*/ long lFlags, /*[in]*/ BSTR sData, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetWashCode)(/*[in]*/ long lSyncNumber, /*[out]*/ long *lResult, /*[out]*/ BSTR *sData, /*[in,out]*/ long *lFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(GenerateWashCode)(/*[in]*/ long lDevice,  /*[in]*/ long lWashProgram, /*[in]*/ long lFlags, /*[out]*/ long *lSyncNumber, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetParam2)(/*[in,out]*/ BSTR *sTableFieldAndVal,/*[in]*/ long lKey1,/*[in]*/ long lKey2,/*[in]*/ short nFiledType,/*[out]*/ long *lVal, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetProcess)(/*[in]*/ long lProcessNumber, /*[out]*/ long *plParam1, /*[out]*/ long *plParam2, /*[out,retval]*/ long * pVal);
	STDMETHOD(SetProcess)(/*[in]*/ long lProcessNumber, /*[in]*/ long lParam1, /*[in]*/ long lParam2, /*[out,retval]*/ long * pVal);
	STDMETHOD(QueryPrePaidTrs)(/*[in]*/ long lPumpNumber, /*[in]*/ long lIndexNumber, /*[in]*/ short nFlags, /*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(QueryPAPTrs)(/*[in]*/ long lPumpNumber, /*[in]*/ long lIndexNumber, /*[in]*/ short nFlags, /*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetParam)(/*[in]*/ BSTR sTableAndFieldName,/*[in]*/  long lKey1,/*[in]*/ long lKey2, /*[in]*/ short nFiledType, /*[out]*/ BSTR *sVal, /*[out]*/ long *lVal, /*[out,retval]*/ long * pVal);
	STDMETHOD(GetPumpStatus2)(/*[in]*/ long lPumpNumber, /*[out]*/ BSTR * pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPAPTrs2)(/*[in]*/ long lPumpNumber, /*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPrePaidTrs2)(/*[in]*/ long lPumpNumber, /*[in]*/ long lSyncNumber, /*[in]*/ short nFlags,/*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPumpInfo2)(/*[in]*/ long lPumpNumber, /*[out]*/ BSTR *pRec,/*[out , retval]*/ long *pVal);
	STDMETHOD(GetPumpTransact2)(/*[in]*/ long lPumpNumber, /*[in]*/ long lTrsNumber , /*[out]*/ BSTR *pRec,  /*[out,retval]*/ long *pVal);
	STDMETHOD(SetPumpMode)(/*[in]*/ long lPumpNumber, /*[in]*/ long lMode, /*[out, retval]*/ long *pVal);
	STDMETHOD(InitInterface2)(/*[in]*/ long lPosNumber,/*[in]*/ long lVersion,/*[in]*/ long lFlags1,/*[in]*/ long lFlags2,/*[in]*/ long lFlags3, /*[out,retval]*/ long * pVal);
	STDMETHOD(GetMinimalPumpsInfo2)(/*[out]*/ BSTR * pRec, /*[out,retval]*/ long * pVal);
	STDMETHOD(RemovePrePaidTrs)(/*[in]*/ long lPumpNumber, /*[in]*/ long lSyncNumber, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPrePaidTrs)(/*[in]*/ long lPumpNumber, /*[in]*/ long lSyncNumber, /*[in]*/ short nFlags,/*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(RemovePAPTrs)(/*[in]*/ long lPumpNumber, /*[in]*/ long lSeqNumber, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPAPTrs)(/*[in]*/ long lPumpNumber, /*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(UnReservePump)(/*[in]*/ long lPumpNumber, /*[in]*/ long lSyncNumber, /*[in]*/ BOOL bForceRelease, /*[in]*/ long lByDevice, /*[out,retval]*/ long *pVal);
	STDMETHOD(ReservePump)(/*[in]*/ long lPumpNumber, /*[in]*/ long lSyncNumber, /*[out,retval]*/ long *pVal);
	STDMETHOD(AuthorizeAndLimit)(/*[in]*/ long lPumpNumber, /*[in]*/ short nLevel, /*[in]*/ long lGradePrice, /*[in]*/ long lMaxValue, /*[in]*/ long lMaxVolume, /*[in]*/ short nFlags, /*[in,out]*/ long *lSyncNumber, /*[out,retval]*/ long *pVal);
	
//	void * GetDataBuff(void * buff, int iControlSize, int iDataSize);
//	void SetRetCode(void * buff,int iRetCodeSize, GPIRetCode retCode);
	STDMETHOD(RestartPump)(/*[in]*/ long lPumpNumber, /*[out,retval]*/ long *pVal);
	STDMETHOD(StopPump)(/*[in]*/ long lPumpNumber, /*[out,retval]*/ long *pVal);
	STDMETHOD(AuthorizePump)(/*[in]*/ long lPumpNumber, /*[in]*/ short nLevel, /*[in]*/ long lGradePrice, /*[out,retval]*/ long * pVal);
	STDMETHOD(SetTrsState)(/*[in]*/ long lPumpNumber, /*[in]*/ long lTrsNumber, /*[in]*/ long lState, /*[out , retval]*/ long *pVal );
	STDMETHOD(GetMinimalPumpsInfo)(/*[out]*/ BSTR *pRec,/*[out,retval]*/ long *pVal);
	STDMETHOD(GetPumpTransact)(/*[in]*/ long lPumpNumber, /*[in]*/ long lTrsNumber , /*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPumpStatus)(/*[in]*/ long lPumpNumber, /*[out]*/ BSTR *pRec, /*[out , retval]*/ long *pVal);
	STDMETHOD(GetPumpInfo)(/*[in]*/ long lPumpNumber, /*[out]*/ BSTR *pRec, /*[out , retval]*/ long *pVal);
	STDMETHOD(GetNumberOfPumps)(/*[out , retval]*/ long *pVal);
	STDMETHOD(GetServerState)(/*[out , retval]*/ long *pVal);
	STDMETHOD(InitInterface)(/*[in]*/ long lPosNumber, /*[in]*/ long lVersion, /*[out , retval]*/ long *pVal);
	STDMETHOD(InitEventsInterface)(/*[in]*/ long lFlags,/*[in,out]*/ long *pClientId,/*[out,retval]*/ long* pVal);
	STDMETHOD(CloseEventsInterface)(/*[in]*/ long lClientId,/*[out,retval]*/ long* pVal);
	STDMETHOD(CreateManualPostVoidTransaction)(/*[in]*/ long lPumpNumber , /*[in]*/ BSTR pRec ,/*[in]*/long lFlags, /*[out]*/ long* lNewTrsNumber, /*[out , retval]*/long *pVal); //CR 351687: Coles Enhanced post void transaction
	STDMETHOD(SetTrsFlags)(/*[in]*/ long lPumpNumber, /*[in]*/ long lTrsNumber, /*[in]*/ long lMiscFlags, /*[in]*/ long lMiscFlag2, /*[out , retval]*/ long *pVal); //CR 351687: Coles Enhanced post void transaction
	STDMETHOD(DisablePriceDisplay)(/*[out , retval]*/ long *pVal); //CR 404591
	STDMETHOD(EnablePriceDisplay)(long *pVal) /*CR 404591 */;
	STDMETHOD(InitDebit)(/*[in]*/ long lPumpGroup01, /*[in]*/ long lPumpGroup2, /*[in]*/ long lFlags, /*[out,retval]*/long *pVal);
	STDMETHOD(GetOfflineTanks)(/*[in]*/ long lFlags,/*[out]*/ BSTR* sTankList,/*[out,retval]*/ long *pVal);
	STDMETHOD(SetProcessExt)(/*[in]*/ long lProcessNumber, /*[in]*/ long lParam1, /*[in]*/ BSTR sInData, /*[out]*/ BSTR *sOutData, /*[out,retval]*/ long * pVal);
	STDMETHOD(QueryPumpInfo)(/*[in]*/ long lPumpNumber, /*[in]*/ long lFlags, /*[out]*/ BSTR sInData, /*[out]*/ BSTR* sOutData,/*[out,retval]*/ long *pVal);
	STDMETHOD(GetAllWashProgram)(/*[in]*/ long lFlags, /*[out]*/ BSTR* sOutData,/*[out,retval]*/ long *pVal);
	STDMETHOD(SaveWashProgram)(/*[in]*/ BSTR sInData,/*[out,retval]*/ long *pVal);
	STDMETHOD(GenerateWashCode2)(/*[in]*/ long lDevice,  /*[in]*/ BSTR sWashProgram, /*[in]*/ long lFlags, /*[out]*/ long *lSyncNumber, /*[out, retval]*/ long *pVal);

	long GetPosNumber(){ return m_lPosNumber; }


protected:
	
	GPIRetCode CreateAndSaveOLARec(CPumpTransact cPumpTrs, CARD_SALE_ALL3 cardDataAll3);
	GPIRetCode GetPrepayBySyncNumber(long lSyncNumber, CPumpTransact &cPumpTrs);
	GPIRetCode CreateAndSaveReceiptRec(CPumpTransact cPumpTrs, CARD_SALE_ALL3 cardDataAll3); //4.0.28.500 - TD 240006/251326
	void FinalizePrePayTrs__(long lPumpNumber, long lFlags, long lFlags2, long lSyncNumber, BSTR sRec, __int64 lPurchaseID,long *pVal);
	GPIRetCode GetPrepayByPurchaseId(__int64 lPurchaseId, short nFlags, TAG_PUMP_TRANSACT5 &tagTrs);
	void GetVersion__(BSTR *sVersionList, long lFlags, long *pVal);

	void LockPump__(long lPumpNumber, long lLock, long lFlags, long *plVal); //4.0.5.24 
	void GetReceipt2__(long lPosNumber, BSTR sTrack2, long lIndex, BSTR *sReceiptData, long lFlags, long *pVal);
	void ReceiptPrinted2__(BSTR sTrack2, long lIndex, long lFlags ,long *pVal);
	void QueryExtraInfoData__(long lPumpNumber, long *lIndexNumber, short nFlags, BSTR *pRec, long *pVal);
	void QueryExtraInfoData2__(long lPumpNumber, long *lIndexNumber, short nFlags, BSTR *pRec, BSTR *pExtraRec, long *pVal); //4.0.24.100 118124
	void AuthorizeAndLimit2__(long lPumpNumber, short nLevel, long lGradePrice, long lMaxValue, long lMaxVolume, long lFlags, long *lSyncNumber, BSTR sRec, long *pVal, __int64 lPurchaseID = 0); //4.0.20.54 58220
	void GetStationStatus__(long * lPumpMap1 , long * lPumpMap2 , long lFlags , long * pRetVal);
	void CreatePostVoidTransaction__(long lPumpNumber, BSTR pRec, long lFlags, long *pVal, LONG& lNewTrsNumber, BOOL bCreateNewTransactionNumber = FALSE );
	void SetTrsFlags__(long lPumpNumber, long lTrsNumber, long lMiscFlags, long lMiscFlags2, long *pVal);
	void DisablePriceDisplay__(long *pVal);
	void EnablePriceDisplay__(long *pVal) /*CR 404591 */;

	GPIRetCode CreatePostVoidTransactionFromXML		 (long lPumpNumber, BSTR pRec, long* &pVal, LONG& lNewTrsNumber, BOOL bCreateNewTransactionNumber = FALSE); //CR 351687: Coles Enhanced post void transaction
	GPIRetCode CreatePostVoidTransactionFromStructure(long lPumpNumber, long lFlags, BSTR pRec, LONG& lNewTrsNumber, BOOL bCreateNewTransactionNumber = FALSE); //CR 351687: Coles Enhanced post void transaction
	GPIRetCode VerifyPostVoidTransactionValues(CPumpTransact &trs); //CR 351687: Coles Enhanced post void transaction
	GPIRetCode VerifyPostVoidTransaction(long lPumpNumber); //CR 351687: Coles Enhanced post void transaction
	GPIRetCode VerifySetTrsFlags(long lPumpNumber); //CR 351687: Coles Enhanced post void transaction

	void LogDuplicatePostVoidTransaction( CPumpTransact &trsTmp, int iRetNum ); //CR 351687: Coles Enhanced post void transaction
	void LogPostVoidTransaction(long lPumpNumber, CPumpTransact &trs, int iRetNum); //CR 351687: Coles Enhanced post void transaction
	void UpdateQdxData(long lPumpNumber, BOOL bUpdateQdxData, EXTRA_INFO& cExtraInfo); //CR 351687: Coles Enhanced post void transaction
	void InsertDispensingVolumes(long lPumpNumber, EXTRA_INFO &cExtraInfo, CPumpTransact &trs, TAG_POST_VOID_EX &cTagPostVoid, BOOL& bUpdateQdxData); //CR 351687: Coles Enhanced post void transaction
	void InsertLAMInfo(long lPumpNumber, EXTRA_INFO &cExtraInfo, CPumpTransact &trs, TAG_POST_VOID_EX &cTagPostVoid, BOOL& bUpdateQdxData); //CR 351687: Coles Enhanced post void transaction


    void GetReceipt__(long lPumpNumber, BSTR sTrack2, short nIndex, BSTR *sReceiptData, long lFlags, long *pVal);
	void GetOLAMemInfo2__(long lPumpNumber, short *nIndex, long lFlags, long lVal, BSTR sVal, BSTR *pRec, long *pVal, const long lFlags2 = 0 );
	void GetMinimalPumpsInfo3__(BSTR *pRec, long lFlags, long *pVal);
	void GetServiceFee__(long lPump, long lTrsType,long lTrsNumber , BSTR stTime, long lFlags, long *pValue,long *pRetVal);
	void GetLevelPrice__(long lPumpNumber, short nGrade, short nLevel, long lServiceMode, long lFlags, long *lPrice, long *pVal);
	void GetGradePrices__(short nGrade, long lFlags, long *lCashFullPrice, long *lCashSelfPrice, long *lCreditFullPrice, long *lCreditSelfPrice, long *pVal);
	void GetPumpInfo3__(long lPumpNumber, BSTR *pRec,long lFlags, long *pVal);
	void GetOLAMemInfo__(long lPumpNumber, short nIndex, long lFlags, BSTR *pRec, long *pVal);
	void ReceiptPrinted__(BSTR sTrack2, short nIndex, long *pVal);
	void QueryPAPTrs2__(long lPumpNumber, long lIndexNumber, short nFlags, BSTR *pRec,  long *pVal);
	void GetPAPTrs3__(long lPumpNumber, BSTR *pRec, long *pVal , long lFlags= 0);
	void GetTankInfo__(long lTankNumber, BSTR *pRec, long lFlags, long *pVal);
	void QueryReceipt__(long lPosNumber, BSTR sTrack2, short * nIndex, short nFlags , BSTR *pData, long *lRetVal);
	//4.0.9.506
	void GetReceiptRawData__(long lPumpNumber,BSTR sTrack2,short *nIndex,BSTR *sReceiptRawData, long *plFlags,long *pVal);

	//4.0.3.21
	void GetPAKQuery__(BSTR * sData , long * plRecCount , long lFlags , long * plRetVal);
	void GetPAKQuery2__(BSTR * sData , long * plRecCount , long lFlags , long * plRetVal);

	//4.0.20.501 - TD 65908
	void GetPAKTrs__(long lPumpNumber, long lSyncNumber, long lFlags, BSTR *pRec, long *pVal);
	void FinalizePAKTrs__(long lPumpNumber, long lFlags, long lSyncNumber, BSTR sRec, long *pVal);

	void GetPumpsTrsInfo__(BSTR *pPumpsInfo, BSTR *pAllTrsInfo, long * pTrsCount, long lGetPumpsFlags, long lGetTrsFlags, long lParam, long *pVal);		//4.0.22.500 - TD 69173
	
	long		m_lPosNumber;
	SHORT		m_nPumpVer[MAX_PUMPS_64];
	SHORT		m_nPumpDetailedVer[MAX_PUMPS_64];
	BOOL		m_bInitDone;

	void StopPump2__(long lPumpNumber, short nGroup, long *pVal);
	void AuthorizePump2__(long lPumpNumber, short nLevel, long lGradePrice, long lFlags, long lLimit, short nGroup, long *pVal, BSTR sRec = NULL);
	void GenerateWashCode__(long lDevice, long lWashProgram, long lFlags, long *lSyncNumber, long *pVal);
	void GenerateWashCode2__(long lDevice, BSTR sWashProgram, long lFlags, long *lSyncNumber, long *pVal);
	void GetWashCode__(long lSyncNumber, long *lResult, BSTR *sData, long *lFlags, long *pVal);
	void CancelWashCode__(long * lSyncNumber,long lDevice, long lFlags, BSTR sData,long lRefundCarWash, long *pVal);
	void ValidateWashCode__(long lDevice, long *lSyncNumber, long lWashCode, long lWashProgramID, long lFlags, long *pVal); //4.0.26.501 144221
	void GetParam2__(BSTR *sTableFieldAndVal,long lKey1,long lKey2,short nFiledType,long *lVal,long *pVal);
	void GetProcess__(long lProcessNumber,long *plParam1,long *plParam2,long * pVal);
	void SetProcess__(long lProcessNumber,long lParam1,long lParam2,long * pVal);
	void SetProcessExt__(long lProcessNumber, long lParam1, BSTR sInData, BSTR *sOutData, long * pVal);
	void QueryPrePaidTrs__(long lPumpNumber, long lIndexNumber, short nFlags, BSTR *pRec, long *pVal,const long lFlags2 = 0);
	void QueryPAPTrs__(long lPumpNumber, long lIndexNumber, short nFlags, BSTR *pRec,  long *pVal);
	void GetParam__(BSTR sTableAndFieldName, long lKey1,long lKey2, short nFiledType, BSTR *sVal, long *lVal, long * pVal);
	void GetPumpStatus2__(long lPumpNumber, BSTR * pRec, long *pVal);
	void GetPAPTrs2__(long lPumpNumber, BSTR *pRec, long *pVal);
	void GetPrePaidTrs2__(long lPumpNumber, __int64 lSyncNumber, short nFlags,BSTR *pRec, long *pVal);
	void GetPumpInfo2__(long lPumpNumber, BSTR *pRec,long *pVal);
	void GetPumpTransact2__(long lPumpNumber, long lTrsNumber , BSTR *pRec,  long *pVal);
	void SetPumpMode__(long lPumpNumber, long lMode, long *pVal);
	void InitInterface2__(long lPosNumber,long lVersion,long lFlags1,long lFlags2,long lFlags3, long * pVal);
	void GetMinimalPumpsInfo2__(BSTR * pRec, long * pVal);
	void RemovePrePaidTrs__(long lPumpNumber, long lSyncNumber, long *pVal);
	void GetPrePaidTrs__(long lPumpNumber, long lSyncNumber, short nFlags,BSTR *pRec, long *pVal);
	void RemovePAPTrs__(long lPumpNumber, long lSeqNumber, long *pVal);
	void GetPAPTrs__(long lPumpNumber, BSTR *pRec, long *pVal);
	void UnReservePump__(long lPumpNumber, long lSyncNumber, BOOL bForceRelease, long lByDevice, long *pVal);
	void ReservePump__(long lPumpNumber, long lSyncNumber, long *pVal);
	void AuthorizeAndLimit__(long lPumpNumber, short nLevel, long lGradePrice, long lMaxValue, long lMaxVolume, long lFlags, long *lSyncNumber, long *pVal, BSTR sLoyaltyInfo = NULL, __int64 lPurchaseID = 0); //4.0.20.54 58220
	
	void AuthorizeAndLimitHandlePrePay( long p, long &lFlags, GPIRetCode &retCode, CString &sLogMsg, long lPumpNumber, short nUnpaid, CPumpTransact &trs, PRE_PAY_INFO &info, long * &lSyncNumber, BSTR bstrPrePay, long &lRtc );
	
	void IsAllowPrePay( long p, GPIRetCode &retCode, long & lFlags, CPumpStatus &stat, CString &sLogMsg, long lPumpNumber, short nUnpaid );
	
	void InitDebit__(long lPumpGroup01, long lPumpGroup2, long lFlags, long *pVal);
	void QueryPumpInfo__(long lPumpNumber, long lFlags, BSTR sInData, BSTR* sOutData, long *pVal);
	CString GetPumpInfoStateText(long lPumpNumber);
	BOOL IsAllowPrePayTrs(long lPumpNumber);
	GPIRetCode ValidateMovePrePay(long lFlags, long lPumpNumber, GPIRetCode retCode, CPumpTransact &trs, PRE_PAY_INFO &info, short nUnpaid);
	long MovePrePay( long lPumpNumber, CPumpTransact &trs,PRE_PAY_INFO &info, long * lSyncNumber, GPIRetCode &retCode );
	
	void RestartPump__(long lPumpNumber, long *pVal);
	void StopPump__(long lPumpNumber, long *pVal);
	void AuthorizePump__(long lPumpNumber, short nLevel, long lGradePrice, long * pVal);
	void SetTrsState__(long lPumpNumber, long lTrsNumber, long lState, long *pVal,long lFlags =0, BSTR sData = NULL);//4.0.11.504 //4.0.22.501 - TD 69904
	void SetPrePayTrsState_ (long lPumpNumber, long lSyncNumber, long lStatus, long lFlags, long *plVal); // 4.0.5.39
	void GetMinimalPumpsInfo__(BSTR *pRec,long *pVal);
	void GetPumpTransact__(long lPumpNumber, long lTrsNumber , BSTR *pRec, long *pVal);
	void GetPumpStatus__(long lPumpNumber, BSTR *pRec, long *pVal);
	void GetPumpInfo__(long lPumpNumber, BSTR *pRec, long *pVal);
	void GetNumberOfPumps__(long *pVal);
	void GetServerState__(long *pVal);
	void InitInterface__(long lPosNumber, long lVersion, long *pVal);
	long GetCarWashRecord(CAR_WASH_INFO *pInfo, char *szCarWashCode);
	void GetOlaMemInfoStatus__(long lFlags, BSTR *pRec, long *pVal);
	void SetMemOla__(long lPump, long lTrsNumber, long lFlags, long lVal, long *pVal);
	void CheckAndReserve__(long lPumpNumber, long lPosNumber, long lPumpRes, long lStatus, BSTR sRec, long lFlags, long *lTrsNumber, long *pVal); //4.0.17.20 - TD 26384
	void GetPumpTransact5__(long lPumpNumber, long lTrsNumber , BSTR *pRec,  long *pVal);
	void SetANPRCommand__(long lCommand, long lPumpNumber, long lFlags1, long lFlags2, BSTR pRec, long *pVal);	//4.0.23.500 72717
	void GetServerStateEx__(long *pServerStateEx, long lFlag, long *pVal);										//4.0.23.507 89047
	void SetPumpTestFlags(long lPumpNumber, long lFlags, CMD_STRUCT_AUTHORIZE_LIMIT &cmdAuthorize, BSTR sRec);		 //4.0.25.1140 TD 412765
	void GetOfflineTanks__(long lFlags, BSTR * sTankList, long *pVal);  //CR 443524


	//Eyal 4.0.0.22
	void CGPI::SwitchServer__(long lNewServerType, long *plRetVal);
private:

	GPIRetCode GetPumpGradeInfoImpl(long lPumpNumber, CComBSTR &bstrGrades, long * lRecordCount);		//4.0.25.1130 
	GPIRetCode GetAllGradesData(long lPumpNumber, CComBSTR &bstrGrades, long * lRecordCount, long lFlags);			//4.0.25.1130 
	GPIRetCode GetGradesPositionData(long lPumpNumber, CComBSTR &bstrGrades, long * lRecordCount);		//4.0.25.1130 

	long GetFlagsForOperation(CarWashOperation eOperation, long lFlags); //4.0.23.504 76432
	void GetPumpGradeInfo__(long lPumpNumber, BSTR *sGradesData, long *lRecordCount, long lFlags, long *plVal);
	void AddItem2Trs__(long lPumpNumber, long lItemSelected, long lFlags, long *plRetVal);
	void GetAllTrsQuery__(long lPumpNumber, BSTR *sData, long *plRecCount, long lFlags, long *plRetVal);
	void GetAllTrsQuery2__(long lPumpNumber, BSTR *sData, long *plRecCount, long lFlags, long *plRetVal);
	BOOL GetGradeInfo(long lGrade, TAG_GARDE_INFO &cTagGradeInfo); //4.0.9.507 
	BOOL GetGradeAllInfo(long lGrade, TAG_GARDE_ALL_INFO &cTagGradeInfo, long lFlags); //4.0.12.505 //4.0.26.508 147183
	void GetReport__(long lNumber, long lType, BSTR *sReportData, long lFlags, long *lRet);
	void SetAccumulateState__(long lShiftNumber, long lState, long lFlags, long *pVal);
	void GetAccumulateTotals__(long lShiftNumber ,  long lState, long lPosNumber, long lFlags,  long lVal, BSTR sStartTime, BSTR sEndTime, long *lRecordCount,   BSTR* sRec, long *pVal);
	void GetPAPTrs4__(long lPumpNumber, BSTR *pRec, long lFlags, long *pVal);
	void GetPAPTrs5__(long lPumpNumber, BSTR *pRec, long lFlags, long *pVal);
	void GetPAPTrs6__(long lPumpNumber, BSTR *pRec, BSTR *pLoyaltySummaryXml,long lFlags, long *pVal);	//4.0.19.505

	void AttachLoyaltySummaryXml(long lTrsNum, long lPumpNumber, BOOL &bRetCode, GPIRetCode &retCode, long lFlags, BSTR * &pRec, BSTR *&pLoyaltySummaryXml);

	void GetAlarmById__(long lAlarmId, long lFlags, BSTR *pData, long *plVal);
	void GetPumpModeScheme3__(long lPumpNumber, long lScheme, long *plMode, BSTR *psPeriodDateTimeStart, BSTR *psPeriodDateTimeEnd, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plServiceFeeNo, long *plFlags, long *pVal);
	void SetPumpModeScheme3__(long lPumpNumber, long lScheme, long lMode, BSTR sPeriodDateTimeStart, BSTR sPeriodDateTimeEnd, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lServiceFeeNo, long lFlags, long *pVal);
	void AckAlarm__(long lCategory, long lSubCategory, long lAlarmId, long lDevice, long lAlarmNumber, long lCustomerId, long lFlags, BSTR sData, long *plVal);
	void GetAlarmInfo__(long lCategory, long lSubCategory,long lDevice, long *plAlarmId, long lFlags, BSTR *pData, long *plVal);
	void GetSingleProductInfo__(long lPump, long lFlags, long *lGrade, long *pVal);
//	BOOL IsPumpIdle(short nPump);
	void CloseEventsInterface_(long lClientId, long *pVal);
	void InitEventInterface_(long lFlags,long * pClientId,long *pVal);

	BOOL IsTargetPumpReadyForMovedPrepay(long lTargetPumpIndex);

	GPIRetCode GetAppropriateRetCode(long lServerState);

	//4.0.12.503
	void SetPumpStatus__(long lPumpNumber, long lStatus, long lRes, long lResExt, long *pVal); 
	void PAPSwipe__(long lPumpNumber, long lDeviceNumber, long *lSeqNumber, long lFlags, BSTR sData, long *pVal); 
	void PAPReadyTransaction__(long lPumpNum, long lSeqNum, long lTrsNum, short nNozzle, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long lFlags, BSTR sData, BSTR* sRetData, long* pVal);
	void GetParam3__(BSTR sParamPosition,long lKey1,long lKey2,short nFiledType,BSTR *sVal,long *lVal,BOOL bCreate, long *pVal); 

	//4.0.19.504 - 47559
	void SetPumpMisc__(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal);
	void SetPumpMiscExt__(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal);
	void SetAlarm__(long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus, long lAlarmValue,long lFlags, long *pAlarmId,  BSTR sRec, long *pVal);
	void PAPNoFuelReadyTrs__(long lPumpNumber, long lFlags, BSTR sData, BSTR sParam1, long lParam2, long *pVal);
	BOOL IsAuthorizeMobileTrs(long lPumpNumber, long m_lPosNumber, CPumpStatus PumpStat);
	BOOL IsAuthorizeTerminalSrvTrs(long lPumpNumber, long m_lPosNumber, CPumpStatus PumpStat);
	GPIRetCode CancelTrsByExternal(long lTrsNumber, long lPumpNumber, CPumpStatus &pumpStat);


	GPIRetCode HandleGetReport(long lNumber, long lType, BSTR *sReportData, long lFlags); //4.0.22.501 - TD 69905
	void RequestReportReady__(long lType, long lFlags, BOOL *pbIsReady, BSTR *pbstrReportBuffer, long *lpResultCode, long *pVal); //4.0.22.501 - TD 69905

		void SetLimit(long lPumpNumber , TAG_DISCOUNT_LOYALTY_INFO4 & cLoyaltyInfo , PRE_PAY_INFO & info); // 4.0.23.1050 TD 233242
	long HandleLoyaltyPrepPay(long lPumpNumber ,  CPumpTransact & trs , BSTR sLoyaltyInfo , TAG_DISCOUNT_LOYALTY_INFO4 & cLoyaltyInfo ,PRE_PAY_INFO & info); // 4.0.23.1050 TD 233242

	//4.0.19.501 - TD 116227
	void GetParam4__(BSTR *sTableFieldAndVal,long lKey1,long lKey2,short nFiledType,long lFlags,long *lVal,long *pVal);

	long GetMiddleOfProcId() const; // TD 254219 4.0.28.501
	void SendForceRemoveAlarm(long lState , CPumpTransact cTrsStatus) const;  // TD 236009 4.0.25.220 // TD 254948/254959 4.0.25.290

	void SaveTenderType(CPumpTransact & trs, BSTR bstrTenderType);					   // 4.0.25.600   CR 296433
	void SavePumpMode( long lPumpNumber, long lMode );	//4.0.31.0 - TD 326234

 	void AuthorizePumpCheckValidInput( IN long lPumpNumber, IN short nPrePayWaiting, IN short nUnpaid, OUT BOOL &bAuthAll , OUT GPIRetCode &retCode );
 	void AuthoirzeAndLimitPAPExternal(IN long lPumpNumber , IN short nLevel, IN long lGradePrice, IN long lLimit , IN OUT long & lFlags);
	GPIRetCode AuthotizePumpPrePayFlow(long p, CPumpTransact &trs, CPumpStatus &stat);
	void AuthotizePumpPayAtKisokFlow(CPumpStatus &stat, long &lFlags, GPIRetCode &retCode, long lPumpNumber, short nLevel, long lGradePrice, long lLimit, BOOL bForceIgnoreVit, BSTR sRec);
	GPIRetCode GetReceiptFromTerminalSrv(TAG_OLA_REC11 &tagRec, short nFlags, char * szReceiptKey, short * nIndex, long lenReciptKey = NULL);
	void GetAllWashProgram__(long lFlags, BSTR * sOutData, long * pVal);
	void SaveWashProgram__(BSTR sData, long * pVal);
	void LogFlags(long lGetTrsFlags, long lGetMinimalPumpsFlags, long lGetPumpInfo3Flags, long lParam);//On loglevel=5 log getTrsInfo flags

public :
	STDMETHOD(GetCurrentTransactionNumber)(long lPumpNumber, long* pTrsNumber, long* pVal);//TD 453656
	STDMETHOD(GetPrePaidTrs3)(long lPumpNumber, __int64 lSyncNumber, short nFlags, BSTR *pRec, long *pVal); // cinooc
	STDMETHOD(FinalizePrePayTrs2)(/*[in]*/ long lPumpNumber, /*[in ]*/ long lFlags, /*[in ]*/ long lFlags2, /*[in]*/ long lSyncNumber, /*[in]*/ BSTR sRec, /*[in]*/ __int64 lPurchaseID, /*[out,retval]*/ long *pVal); // cinooc
	STDMETHOD(AuthorizeAndLimit4)(/*[in]*/ long lPumpNumber, /*[in]*/ short nLevel, /*[in]*/ long lGradePrice, /*[in]*/ long lMaxValue, /*[in]*/ long lMaxVolume, /*[in]*/ long lFlags, /*[in,out]*/ long *lSyncNumber, /*[in]*/ BSTR sRec, /*[in]*/ __int64 lPurchaseID, /*[out,retval]*/ long *pVal); // cinooc
	STDMETHOD(SetPumpMiscExt)(/*[in]*/ long lPumpNumber, /*[in]*/ long lMisc,  /*[in]*/ long bRemoveMisc, /*[out,retval]*/ long *pVal);
	STDMETHOD(ValidateWashCode)(/*[in]*/ long lDevice,/*[out]*/ long *lSyncNumber, /*[in]*/ long lWashCode, /*[in]*/ long lWashProgramID, /*[in]*/ long lFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetParam4)(/*[in,out]*/ BSTR *sTableFieldAndVal,/*[in]*/ long lKey1,/*[in]*/ long lKey2,/*[in]*/ short nFiledType, /*[in]*/ long lFlags, /*[out]*/ long *lVal, /*[out,retval]*/ long *pVal); //4.0.19.501 - TD 116227
	STDMETHOD(QueryExtraInfoData2)(/*[in]*/ long lPumpNumber, /*[in,out]*/ long *lIndexNumber, /*[in]*/ short nFlags, /*[out]*/ BSTR *pRec, /*[out]*/ BSTR *pExtraRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetServerStateEx)(/*[in,out]*/ long* pServerStateEx, /*[in]*/ long lFlag, /*[out,retval]*/ long* pVal);
	STDMETHOD(SetANPRCommand)(/*[in]*/ long lCommand, /*[in]*/ long lPumpNumber, /*[in]*/ long lFlags1, /*[in]*/ long lFlags2, /*[in]*/ BSTR pRec, /*[out, retval]*/ long *pVal);
	STDMETHOD(SetTrsState3)(/*[in]*/ long lPumpNumber, /*[in]*/long lTrsNumber,/*[in]*/ long lState, /*[in]*/ long  lFlags, /*[in]*/ BSTR sData, /*[out , retval]*/ long *pVal);
	STDMETHOD(RequestReportReady)(/*[in]*/ long lType, /*[in]*/ long lFlags, /*[out]*/ BOOL *pbIsReady, /*[out]*/ BSTR *pbstrReportBuffer, /*[out]*/ long *lpResultCode, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPumpsTrsInfo)(/* [IN]*/long lPumpNumber,/*[out]*/ BSTR *pPumpsInfo, /*[out]*/ BSTR *pAllTrsInfo, /*[out]*/ BSTR *pGetPumpsInfo3,/*[in , out]*/ long * pTrsCount, /*[in]*/ long lGetPumpsFlags, /*[in]*/ long lGetTrsFlags, /*[in]*/ long lGetPumpInfo3Flags,/*[in]*/ long lParam, /*[out , retval]*/ long *pVal);
	STDMETHOD(FinalizePAKTrs)(/*[in]*/ long lPumpNumber, /*[in]*/ long lFlags, /*[in]*/ long lSyncNumber, /*[in]*/ BSTR sRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPAKTrs)(/*[in]*/ long lPumpNumber, /*[in]*/ long lSyncNumber, /*[in]*/ long lFlags, /*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(FinalizePrePayTrs)(/*[in]*/ long lPumpNumber, /*[in ]*/ long lFlags, /*[in ]*/ long lFlags2, /*[in]*/ long lSyncNumber, /*[in]*/ BSTR sRec, /*[in]*/ long lPurchaseID, /*[out,retval]*/ long *pVal);
	STDMETHOD(AuthorizeAndLimit3)(/*[in]*/ long lPumpNumber, /*[in]*/ short nLevel, /*[in]*/ long lGradePrice, /*[in]*/ long lMaxValue, /*[in]*/ long lMaxVolume, /*[in]*/ long lFlags, /*[in,out]*/ long *lSyncNumber, /*[in]*/ BSTR sRec, /*[in]*/ long lPurchaseID, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPAPTrs6)(/*[in]*/ long lPumpNumber, /*[out]*/  BSTR *pRec,  /*[out]*/  BSTR *pLoyaltySummaryXml, /*[in]*/ long lFlags,/*[out,retval]*/  long *pVal);
	STDMETHOD(PAPNoFuelReadyTrs)(/*[in]*/long lPumpNumber,/*[in]*/ long lFlags, /*[in]*/ BSTR sData , /*[in]*/ BSTR sParam1, /*[in]*/ long lParam2, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetAlarm)(/*[in]*/ long lCatagory, /*[in]*/ long lSubCatagory, /*[in]*/ long lAlarmNumber ,/*[in]*/ long lDevice,  /*[in]*/ long lAlarmStatus, /*[in]*/ long lAlarmValue, /*[in]*/ long lFlags,/*[out]*/ long *pAlarmId, /*[in]*/ BSTR sRec, /*[out, retval]*/ long *pVal);
	STDMETHOD(SetPumpMisc)(/*[in]*/ long lPumpNumber, /*[in]*/ long lMisc,  /*[in]*/ long bRemoveMisc, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetSaleExtraDetails)(/*[in]*/long lPumpNumber,/*[in]*/ long lFlags, /*[in]*/ BSTR sData , /*[out]*/ BSTR *sRetData, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetOLAMemInfo3)(/*[in]*/ long lPumpNumber , /*[in,out]*/ short *nIndex , /*[in]*/  long lFlags, /*[in]*/ long lVal,/*[in]*/ BSTR sVal, /*[out]*/  BSTR *pRec, /*[out,retval]*/ long  *pVal);
	STDMETHOD(GetAllTrsQuery2)(/*[in]*/ long lPumpNumber,/*[out]*/ BSTR * sData , /*[in , out]*/ long * plRecCount , /*[in]*/ long lFlags , /*[out , retval]*/ long * plRetVal);
	STDMETHOD(QueryPrePaidTrs2)(/*[in]*/ long lPumpNumber, /*[in]*/ long lIndexNumber, /*[in]*/ short nFlags, /*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPAKQuery2)(/*[out]*/ BSTR * sData , /*[in , out]*/ long * plRecCount , /*[in]*/ long lFlags , /*[out , retval]*/ long * plRetVal);
	STDMETHOD(GetPumpTransact5)(/*[in]*/ long lPumpNumber, /*[in]*/ long lTrsNumber , /*[out]*/ BSTR *pRec,  /*[out,retval]*/ long *pVal);
	STDMETHOD(CheckAndReserve)(/*[in]*/ long lPumpNumber, /*[in]*/ long lPosNumber, /*[in]*/ long lPumpRes, /*[in]*/ long lStatus, /*[in]*/ BSTR sRec, /*[in]*/ long lFlags, /*[in,out]*/ long *lTrsNumber, /*[out,retval]*/ long *pVal); //4.0.17.20 - TD 26384
	STDMETHOD(GetPAPTrs5)(/*[in]*/ long lPumpNumber, /*[out]*/  BSTR *pRec,  /*[in]*/ long lFlags,/*[out,retval]*/  long *pVal);
	STDMETHOD(AuthorizePump3)(/*[in]*/ long lPumpNumber,/*[in]*/ short nLevel,/*[in]*/ long lGradePrice,/*[in]*/ long lFlags, /*[in]*/ long lFlags2, /*[in]*/ long lLimit, /*[in]*/ short nGroup, /*[out,retval]*/ long * pVal); //4.0.16.500 - TD 34675
	STDMETHOD(SetPumpStatus)(/*[in]*/ long lPumpNumber, /*[in]*/ long lStatus, /*[in]*/ long lRes, /*[in]*/ long lResExt, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetParam3)(/*[in]*/ BSTR sParamPosition, /*[in]*/ long lKey1, /*[in]*/ long lKey2, /*[in]*/ short nFiledType, /*[in,out]*/ BSTR *sVal, /*[in,out]*/ long *lVal, /*[in]*/ BOOL bCreate, /*[out, retval]*/ long *pVal);
	STDMETHOD(PAPReadyTransaction)(/*[in]*/long lPumpNum, /*[in]*/long lSeqNum,/*[in]*/ long lTrsNum, /*[in]*/ short nNozzle, /*[in]*/ long lVolume, /*[in]*/ long lValue, /*[in]*/ short nLevel, /*[in]*/ long lGradePrice, /*[in]*/ short nGrade, /*[in]*/ long lFlags, /*[in]*/ BSTR sData, /*[out]*/ BSTR *sRetData, /*[out,retval]*/ long *pVal);
	STDMETHOD(PAPSwipe)(/*[in]*/ long lPumpNumber, /*[in]*/ long lDeviceNumber, /*[in,out]*/ long *lSeqNumber, /*[in]*/ long lFlags, /*[in]*/ BSTR sData, /*[out, retval]*/ long *pVal);
	STDMETHOD(AuthorizePump4)(/*[in]*/ long lPumpNumber,/*[in]*/ short nLevel,/*[in]*/ long lGradePrice,/*[in]*/ long lFlags, /*[in]*/ long lFlags2, /*[in]*/ long lLimit, /*[in]*/ short nGroup, /*[in]*/ BSTR sRec, /*[out,retval]*/ long * pVal); //TD 412765

BEGIN_CONNECTION_POINT_MAP(CGPI)
	CONNECTION_POINT_ENTRY(DIID_IGPIEvents)
END_CONNECTION_POINT_MAP()

};

#endif //__GPI_H_
