 	
// GCI.h : Declaration of the CGCI

#ifndef __GCI_H_
#define __GCI_H_

#include "resource.h"       // main symbols
#include "PumpSrv.h"	// Added by ClassView
#include "ServerMain.h"	// Added by ClassView
#include "Pump.h"	// Added by ClassView
#include "HydraDef.h"


#define MAX_GCIEVENTS_RETRIES						10
#define TIMEOUT_BETWEEN_RETRIES						300		// milisec
#define THRD_CANNOT_WORK							(WM_USER + 1000)
#define THRD_SLEEP_TIME								100		// milisec 
#define TIME_BETWEEN_MODE_SCHEME_CHECKS				60000	// milisec (= 1 minute)
#define TIME_BETWEEN_PRICE_AND_SHIFT_CHECKS			60000	// milisec (= 1 minute)
#define TIME_BETWEEN_DELIVERY_REPORT_CHECKS			10000	// milisec (= 10 sec)//4.0.7.500
#define TIME_BETWEEN_DISPENSING_TIMEOUT_CHECKS		5000	// milisec (= 5 sec)
#define TIME_BETWEEN_UNPAID_TIMEOUT_CHECKS			15000	// milisec (= 15 sec)
#define TIME_BETWEEN_AUTHORIZED_TIMEOUT_CHECKS		10000	// milisec (= 10 sec) 
#define TIME_BETWEEN_CAR_WASH_CHECKS				1000	// milisec (= 1 sec) 
#define TIME_BETWEEN_CAR_WASH_CLEAR_CHECKS			3600000	// milisec (= 1 hour) 
#define TIME_BETWEEN_RECEIPT_CHECKS					1000	// milisec (= 1 sec) 
#define TIME_BETWEEN_RECEIPT_CLEAR_CHECKS			3600000	// milisec (= 1 hour) 
#define TIME_BETWEEN_PRICE_CHANGE_CHECKS			5000	// milisec (= 5 sec) 	//4.0.8.501
#define TIME_BETWEEN_CAR_WASH_GENERAL_REQ_CHECKS	1000	// milisec (= 1 sec) //4.0.23.504 76432
#define TIME_BETWEEN_REALTIME_CLOCK_CHECKS			300000	// milisec (= 5 minute)
#define TIME_BETWEEN_AUTO_RECONCILE_CHECKS			10000 //  milisec (= 10 sec) 	//4.0.23.1050 TD 233242

#ifdef USE_XML_PRICE_CHANGE

#define TIME_BETWEEN_INCOMING_NEW_PRICE_CHECKS		60000	// milisec (= 1 minute)
#define TIME_BETWEEN_NOZZLE_OUT_CHECKS				30000   //(30 secs.)		//4.0.1.38
#define TIME_BETWEEN_LOCK_PUMP_BY_MODE_CHECKS		30000   //(30 secs.)	//4.0.3.49
#define TIME_BETWEEN_EXPORT_SITE_CONFIG_CHECKS		60000   //milisec (= 1 minute)	//4.0.26.508 TD 146575
#define TIME_BETWEEN_PRICE_PROTECTION_CHECKS		1000  // milisec (10 secs)  //4.0.23.2561

#endif


#define THRD_SERVER_IS_MASTER		(pInfo->m_pServer->GetOperatingMode() == MASTER)			
#define THRD_SERVER_IS_SLAVE		(pInfo->m_pServer->GetOperatingMode() == SLAVE)			
#define THRD_SERVER_IS_NOT_MASTER	(pInfo->m_pServer->GetOperatingMode() != MASTER)			
#define THRD_SERVER_IS_NOT_SLAVE	(pInfo	->m_pServer->GetOperatingMode() != SLAVE)

#define PUMP_TEST_MODE        '1'			//TD 231472 4.0.27.60
#define PUMP_CALIBRATION_MODE '2'		    	


/////////////////////////////////////////////////////////////////////////////
// CGCI
class ATL_NO_VTABLE CGCI : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CGCI, &CLSID_GCI>,
	public IConnectionPointContainerImpl<CGCI>,
	public IDispatchImpl<IGCI, &IID_IGCI, &LIBID_PUMPSRVLib>,
	public IConnectionPointImpl<CGCI,&IID_IGCIEvents>
{
public:
	CGCI();
	virtual ~CGCI();

static long		m_lRefCount;			//4.0.23.508 TD 90359

DECLARE_REGISTRY_RESOURCEID(IDR_GCI)
DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(CGCI)
	COM_INTERFACE_ENTRY(IGCI)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CGCI)
	CONNECTION_POINT_ENTRY(IID_IGCIEvents)
END_CONNECTION_POINT_MAP()

public:
	void FinalRelease();

	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IGCI
public:	
	STDMETHOD(CarWashGeneralReqResponse)(/*[in]*/ long lDevice, /*[in]*/ long lResult, /*[in]*/ long lFlags1, /*[in]*/ long lFlags2, /*[in]*/ BSTR sData, /*[out, retval]*/ long *pVal);
	STDMETHOD(ReportReady)(/*[in]*/ long lFlags, /*[in]*/ BSTR wszReportText, /*[in]*/ long lParam, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetPumpMiscExt)(/*[in]*/ long lPumpNumber, /*[in]*/ long lMisc,  /*[in]*/ long bRemoveMisc, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetPumpMisc)(/*[in]*/ long lPumpNumber, /*[in]*/ long lMisc, /*[in]*/ long bRemoveMisc, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetPumpStatus2)(/*[in]*/ long lPumpNumber, /*[in]*/ long lStatus, /*[in]*/ long lRes, /*[in]*/ long lResExt, /*[in]*/ long lFlags, /*[in]*/ short nGrade, /*[in]*/ short nPosition, /*[in]*/ BSTR sData, /*[out,retval]*/ long *pVal); //4.0.19.500 - TD 59171
	STDMETHOD(ReadyOfflineTRS)(/*[in]*/long lPumpNumber,/*[in]*/ long lFlags, /*[in]*/ BSTR sData , /*[in]*/ BSTR sParam1, /*[in]*/ long lParam2, /*[in]*/ long lParam3, /*[out]*/ BSTR *sRetData, /*[out,retval]*/ long *pVal);
	STDMETHOD(DeliveryReportEx)(/*[in]*/ long lTankNumber,  /*[in]*/ long lFlags, /*[in]*/ BSTR sRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(TankReadingEx)(/*[in]*/ long lTankNumber,  /*[in]*/ long lFlags, /*[in]*/ BSTR sRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetCardInfoEx)(/*[in]*/ long lClientId,/*[in]*/ BSTR sInBuffer,/*[in]*/ BSTR sInfoType,/*[in]*/  long lFlags,/*[out]*/ BSTR *sRetData, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetCashAcceptorStatus)(/*[in]*/ long   lTerminalId ,  /*[in]*/ long lFlags ,  /*[in]*/ long lStatus, /*[out]*/ BSTR *sRec, /*[out , retval]*/ long *pVal);
	STDMETHOD(GetCashAcceptorReport)(/*[in]*/long lTerminalId, /*[in]*/long lFlags, BSTR *sHeader, BSTR *sRecords, long *lRecordCount, long *pVal);
	void PriceChangeUpdate__(long lPumpNumber, long lPosition, long lFlags, long *plVal);
	STDMETHOD(GetTransListQuery)(/*[in]*/ long lPumpNumber,/*[out]*/ BSTR *sData,/*[in,out]*/ long *plRecCount, /*[in]*/long *lFlags,/*[out , retval]*/ long *pVal);
	STDMETHOD(SetGradesRestriction)(/*[in]*/BSTR sTagPumpsNuzzlesRestrict,/*[in]*/ long lFlags  ,/*[out,retval]*/ long *pVal);
	STDMETHOD(PriceChangeUpdate)(/*[in]*/ long lPumpNumber , /*[in]*/ long lPosition ,/*[in]*/ long lFlags , /*[out,retval]*/ long * plVal);
	STDMETHOD(GetAttTrs)(/*[in]*/ long lPumpNumber, /*[in]*/ long lFlags, /*[in]*/ BSTR sIndex, /*[out]*/ BSTR * sRec,/*[out,retval]*/ long * pVal );
	STDMETHOD(SetParam4)(/*[in]*/ BSTR sParamPosition, /*[in]*/ long lKey1, /*[in]*/ long lKey2, /*[in]*/ short nFiledType, /*[in]*/ BSTR sVal, /*[in]*/ long lVal, /*[in]*/ BOOL bCreate, /*[in]*/ long lFlags, /*[in]*/ long lDataBase, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetParam4)(/*[in]*/ BSTR sParamPosition, /*[in]*/ long lKey1, /*[in]*/ long lKey2, /*[in]*/ short nFiledType, /*[in,out]*/ BSTR *sVal, /*[in,out]*/ long *lVal, /*[in]*/ BOOL bCreate, /*[in]*/ long lFlags, /*[in]*/ long lDataBase, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetTankMemoryData)(/*[in]*/long lTankNumber, /*[out]*/ BSTR *pRec, /*[in]*/long lFlags, /*[in,out ]*/long *lCounter, /*[out,retval]*/long *pVal);
	STDMETHOD(GetWetStockData)(/*[in]*/ long lWetStockFlags,  /*[out,retval]*/ long *pVal);
	STDMETHOD(UploadBuffer)(/*[in]*/ long lTerminalId,/*[in]*/ long lPumpNumber,/*[in]*/ long lSessionId,/*[in]*/ long lUploadType,/*[in]*/ BSTR sInBuffer,/*[out]*/BSTR *sOutBuffer,/*[in]*/ long lExDataSize,/*[in]*/ BSTR  sExData,/*[out,retval]*/ long *pVal );
	STDMETHOD(GetStationStatus)(/*[in , out]*/ long * lPumpMap1 , /*[in , out]*/ long * lPumpMap2 , /*[in]*/ long lFlags , /*[out , retval]*/ long * pRetVal);
	STDMETHOD(SetVersion)(/*[in]*/ BSTR sVersionList ,  /*[in]*/ long lFlags  ,/*[out retval]*/ long *pVal);
	STDMETHOD(ParseOLABuffer)(/*[in]*/ long lPumpNumber, /*[out]*/BSTR *cCardSaleALL3, /*[in]*/BSTR sOLABuffer, /*[in]*/long lFlags, /*[out , retval]*/ long * pRet);
	STDMETHOD(GetPumpMode)(/*[in]*/  long lPumpNumber,  /*[out]*/ long *plPumpMode, /*[out]*/   long *plFlags, /*[out]*/ long *plMsgNumber, /*[out,retval]*/ long *plVal); //4.0.5.41
	STDMETHOD(GetPAKQuery)(/*[in]*/ BSTR sRestrictionData,/*[out]*/ BSTR * sOutData , /*[in , out]*/ long * plRecCount , /*[in]*/ long lFlags ,  /*[out , retval]*/ long * plRetVal);
	STDMETHOD(SetConnectivityStatus)(/*[in]*/ BSTR sPSConnStatus, /*[in]*/ long lFlags, /*[out,retval]*/  long *pVal);
	STDMETHOD(SetProcess)(/*[in]*/ long lProcessNumber, /*[in]*/ long lParam1, /*[in]*/ long lParam2, /*[out,retval]*/ long *plRetVal);
	STDMETHOD(GetCardInfo3)(/*[in]*/ BSTR sAccountNumber,/*[in]*/ BSTR sTrack1, /*[in]*/ BSTR sTrack2, /*[in]*/ BSTR sShortCardName, /*[in]*/ long lFlags,/*[in,out]*/long *lSesionType ,/*[in]*/ long lPumpNumber,/*[out]*/ BSTR *sRetData, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetPumpMode)(/*[in]*/ long lPumpNumber, /*[in]*/ long lPumpMode,long lFlags, /*[out,retval]*/ long *pVal);
	STDMETHOD(PAKReadyTransaction2)(/*[in]*/long lPumpNumber,/*[in]*/long lTrsNumber,/*[in]*/ short nNozzle,/*[in]*/long lVolume,/*[in]*/long lValue,/*[in]*/short nLevel,/*[in]*/long lGradePrice, /*[in]*/ short nGrade, /*[in]*/ long lFlage, /*[in]*/ long lVal,/*[in]*/ BSTR pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(MoveTerminalTransactionToPump)(/*[in]*/long lTerminalId, /*[in]*/ long lPumpId,/*[in]*/ long lStatus,/*[in]*/ long lFlags, /*[out,retval]*/ long *pVal );
	STDMETHOD(GetCardInfo2)(/*[in]*/ long lPumpNumber, /*[in]*/ long lDeviceNumber, /*[in,out]*/ long *lSeqNumber, /*[in]*/ long lFlags, /*[in]*/ BSTR sData, /*[out, retval]*/ long *pVal);
	GCIRetCode ConverHydServiceCode2GciCode(CFGDBRetCode code);
	STDMETHOD(SetMaintenance)(/*[in]*/ long lLock, /*[in]*/ long lParam, /*[out, retval]*/ long *pVal);
	STDMETHOD(SetShift)(/*[in]*/ short nDayOfWeek, /*[in]*/ short nShiftNumber, /*[in]*/ short nShiftFlags, /*[in]*/ BSTR sStartTime, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetOLABuffer)(/*[in]*/ long lPumpNumber , /*[in]*/ BSTR sCardData , /*[out]*/ BSTR * sOLABuffer , /*[in]*/ long lFlags , /*[out , retval]*/ long * pRet);
	STDMETHOD(QueryData)(/*[in]*/ long lFileNum, /*[in ,out]*/ long *lIndexNumber , /*[in]*/ BSTR sKey , /*[in]*/ long lFlags, /*[out]*/ BSTR *pRec , /*[out,retval]*/ long *pVal);
	STDMETHOD(DisplayInfo)(/*[in]*/ long lPumpNumber , /*[in]*/ BSTR sMsg , /*[in]*/ long lFlags , /*[out , retval]*/ long * pRetVal);
	STDMETHOD(GetCashAcceptorData)(/*[in]*/ long lPumpNumber,/*[in]*/ long lDevice, /*[in]*/ long lSeqNum , /*[in]*/ long lFlags, /*[out]*/ BSTR *CashSaleData , /*[out, retval]*/ long * pVal);
	STDMETHOD(SetCashAcceptorData)(/*[in]*/ long lPumpNumber, /*[in]*/ long lDevice, /*[in]*/ long lSeqNum , /*[in]*/ long lFlags, /*[in]*/ BSTR CashSaleData , /*[out, retval]*/ long * pVal);
	STDMETHOD(SetAlarm)(/*[in]*/ long lCatagory, /*[in]*/ long lSubCatagory, /*[in]*/ long lAlarmNumber ,/*[in]*/ long lDevice,  /*[in]*/ long lAlarmStatus, /*[in]*/ long lAlarmValue, /*[in]*/ long lFlags,/*[out]*/long *pAlarmId, /*[in]*/ BSTR sRec, /*[out, retval]*/ long *pVal);
	STDMETHOD(TankReading3)(/*[in]*/ long lTankNumber,/*[in]*/ long lProduct, /*[in]*/ short nFlags, /*[in]*/ long lStatus1, /*[in]*/ long lFuelLevel, /*[in]*/ long lWaterLevel, /*[in]*/ long lTemperature, /*[in]*/ long lVolume, /*[in]*/ long lWaterVolume, /*[in]*/ long lStatus2, /*[in]*/ long lFuelValue, /*[in]*/ long lPricePerLitre, /*[in]*/ long lTcVolume,/*[in]*/ long lUllage, /*[in]*/ BSTR sRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetDebitData)(/*[in]*/ long lPumpNumber, /*[in]*/ long lFlags, /*[in]*/ BSTR sData, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetReceiptRawData3)(/*[in]*/long lPumpNumber, /*[in]*/ BSTR sTrack2, /*[in , out]*/ short *nIndex, /*[out]*/ BSTR *sReceiptRawData, /*[in,out]*/ long *plFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetDebitDataInfo)(/*[in]*/ long lPumpNumber, /*[in]*/ long lFlags, /*[out]*/ BSTR *pData, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPumpInfo)(/*[in]*/ long lPumpNumber , /*[out]*/ BSTR * pRec , /*[in]*/ long lFlags , /*[out , retval]*/ long * pVal);
	STDMETHOD(SetTrsState)(/*[in]*/ long lPumpNumber , /*[in]*/ long lTrsNumber , /*[in]*/ long lState , /*[in]*/ long lFlags , /*[in]*/ BSTR sData , /*[out , retval]*/ long * plRetVal);
	STDMETHOD(DebitProcessResultCmd)(/*[in]*/ long lPumpNumber , /*[in]*/ long lSessionId , /*[in]*/ long lFlags, /*[in]*/ BSTR sData , /*[out,retval]*/ long *pVal);
	STDMETHOD(GetSingleProductInfo)(/*[in]*/long lPump , /*[in]*/ long lFlags ,/*[in,out]*/ long *lGrade,/*[out, retval]*/ long *pVal);
	STDMETHOD(GetReceiptRawData2)(/*[in]*/long lPumpNumber, /*[in]*/ BSTR sTrack2, /*[in , out]*/ short *nIndex, /*[out]*/ BSTR *sReceiptRawData, /*[in]*/ long lFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(ReceiptPrinted)(/*[in]*/ BSTR sTrack2, /*[in]*/ long lIndex, /*[in]*/ long lFlags ,/*[out, retval]*/ long *pVal);
	STDMETHOD(ReportRequest)(/*[in]*/ long lParam1, /*[in]*/ long lParam2, /*[in]*/ long lFlags, /*[in]*/ BSTR sData, /*[out]*/ long * lRefNum, /*[out,retval]*/ long * pVal);
	STDMETHOD(GetServiceFee)(/*[in]*/ long lPump, /*[in]*/ long lTrsType,/*[in]*/ long lTrsNumber, /*[in]*/ BSTR stTime, /*[in]*/ long lFlags, /*[out]*/ long* pValue,    /*[out, retval]*/long  *pRetVal);
	STDMETHOD(GetReceiptCounter)(/*[out]*/ long *plConuter, /*[in]*/ long lType, /*[in]*/ long lFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetCardInfo)(/*[in]*/ BSTR sAccountNumber,/*[in]*/ BSTR sTrack1, /*[in]*/ BSTR sTrack2, /*[in]*/ BSTR sShortCardName, /*[out]*/ BSTR *sRetData, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetReceiptRowData)(/*[in]*/long lPumpNumber, /*[in]*/ BSTR sTrack2, /*[in]*/ short nIndex, /*[out]*/ BSTR *sReceiptRowData, /*[in]*/ long lFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(SetParam3)(/*[in]*/ BSTR sParamPosition, /*[in]*/ long lKey1, /*[in]*/ long lKey2, /*[in]*/ short nFiledType, /*[in]*/ BSTR sVal, /*[in]*/ long lVal, /*[in]*/ BOOL bCreate, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetParam3)(/*[in]*/ BSTR sParamPosition, /*[in]*/ long lKey1, /*[in]*/ long lKey2, /*[in]*/ short nFiledType, /*[in,out]*/ BSTR *sVal, /*[in,out]*/ long *lVal, /*[in]*/ BOOL bCreate, /*[out, retval]*/ long *pVal);
	STDMETHOD(SetPAPSwipeData)(/*[in]*/ long lPumpNumber, /*[in]*/ long lFlags, /*[in]*/ BSTR sData, /*[out,retval]*/ long *pVal);
	STDMETHOD(WashCodeResult)(/*[in]*/ long lSyncNumber,/*[in]*/ long lDevice, /*[in]*/ long lResult, /*[in]*/ BSTR sData, /*[out, retval]*/ long *pVal);
	STDMETHOD(CarWashStatus)(/*[in]*/ long lDevice, /*[in]*/ long lSrc, /*[in]*/ long lStatus, /*[out, retval]*/ long *pVal);
	STDMETHOD(Alarm)(/*[in]*/ long lCatagory, /*[in]*/ long lCode, /*[in]*/ long lParameter, /*[in]*/ BSTR sStartDateAndTime, /*[in]*/ BSTR sEndDateAndTime, /*[out, retval]*/ long *pVal);
	STDMETHOD(DeliveryReport2)(/*[in]*/ long lTankNumber, /*[in]*/ long lProduct, /*[in]*/ short nFlags, /*[in]*/ long lStatus, /*[in]*/ long lTag, /*[in]*/ long lVolume, /*[in]*/ BSTR sStartDateAndTime, /*[in]*/ BSTR sEndDateAndTime, /*[in]*/ long lStartVolume, /*[in]*/ long lEndVolume, /*[in]*/ long lTemperature,/*[out, retval]*/ long *pVal);
	STDMETHOD(PumpTotals2)(/*[in]*/ long lPumpNumber, /*[in]*/ short nNozzle, /*[in]*/ short nGrade, /*[in]*/ short nFlags, /*[in]*/ long lVolumeH, /*[in]*/ long lVolumeL, /*[in]*/ long lValueAH, /*[in]*/ long lValueAL, /*[in]*/ long lValueBH, /*[in]*/ long lValueBL, /*[in]*/ long lPumpStatus, /*[in]*/ long lNozzleStatus, /*[out,retval]*/ long *pVal);
	STDMETHOD(TankReading2)(/*[in]*/ long lTankNumber,/*[in]*/ long lProduct, /*[in]*/ short nFlags, /*[in]*/ long lStatus1, /*[in]*/ long lFuelLevel, /*[in]*/ long lWaterLevel, /*[in]*/ long lTemperature, /*[in]*/ long lVolume, /*[in]*/ long lWaterVolume, /*[in]*/ long lStatus2, /*[in]*/ long lFuelValue, /*[in]*/ long lPricePerLitre,/*[out,retval]*/ long *pVal);
	STDMETHOD(GetPAPSwipeData)(/*[in]*/ long lPumpNumber, /*[in]*/ long lFlags, /*[out]*/ BSTR *pData, /*[out,retval]*/ long *pVal);
	STDMETHOD(AVIStop)(/*[in]*/ long lPumpNumber, /*[in]*/ long lNozzleNumber, /*[out,retval]*/ long *pVal);
	STDMETHOD(AVIStart)(/*[in]*/ long lPumpNumber, /*[in]*/ long lNozzleNumber, /*[in]*/ BSTR sData, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetParam)(/*[in]*/ BSTR sTableName, /*[in]*/ long lKey1, /*[in]*/ long lKey2, /*[in]*/ BSTR sFiledName, /*[in]*/ short nFiledType, /*[in]*/ BSTR sVal, /*[in]*/ long lVal, /*[in]*/ BOOL bCreate, /*[out,retval]*/ long * pVal);

	STDMETHOD(Watchdog)(/*[in]*/ long lParam, /*[out, retval]*/ long *pVal);
	STDMETHOD(InitInterface2)(/*[in]*/ BSTR strControllerName, /*[in]*/ long lControllerVer, /*[in]*/ long lFlags, /*[out,retval]*/ long *pVal);
	STDMETHOD(TankStatus)(/*[in]*/ long lTankNumber, /*[in]*/ long lStatus, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetGradePrice)(/*[in]*/ short nGrade, /*[out,retval]*/ long *pVal);
	STDMETHOD(DeliveryReport)(/*[in]*/ long lTankNumber, /*[in]*/ long lProduct, /*[in]*/ short nFlags, /*[in]*/ long lStatus, /*[in]*/ long lTag, /*[in]*/ long lVolume, /*[in]*/ BSTR sDateAndTime, /*[out, retval]*/ long *pVal);
	STDMETHOD(TransactionStatusCS)(/*[in]*/long lPumpNumber, /*[in]*/long lTrsNumber, /*[in]*/ short nNozzle, /*[in]*/ long lStatus,/*[in]*/long lVolume, /*[in]*/long lValue, /*[in]*/short nLevel, /*[in]*/long lGradePrice, /*[in]*/short nGrade, /*[out,retval]*/ long *pVal);
	STDMETHOD(TransactionStatusCM)(/*[in]*/long lPumpNumber,/*[in]*/ long lTrsNumber, /*[in]*/ short nNozzle,  /*[in]*/ short nIndex, /*[in]*/ long lStatus,/*[in]*/long lOnOffer, /*[in]*/long lVolume, /*[in]*/long lValue, /*[in]*/short nLevel, /*[in]*/long lGradePrice, /*[in]*/short nGrade,/*[out,retval]*/ long *pVal);
	STDMETHOD(RollingUpdate)(/*[in]*/long lPumpNumber, /*[in]*/ long lTrsNumber,/*[in]*/ long lVolume, /*[in]*/ long lValue, /*[in]*/ long lGradePrice, /*[in]*/ short nGrade, /*[out,retval]*/ long *pVal);	
	STDMETHOD(PAPReadyTransaction)(/*[in]*/long lPumpNumber,/*[in]*/ long lSeqNumber,/*[in]*/long lTrsNumber,/*[in]*/ short nNozzle,/*[in]*/long lVolume,/*[in]*/long lValue,/*[in]*/short nLevel,/*[in]*/long lGradePrice, /*[in]*/ short nGrade,/*[in]*/ long lFlags,/*[in]*/ BSTR sData, /*[out]*/ BSTR *sRetData,/*[out,retval]*/ long *pVal);
	STDMETHOD(PAKReadyTransaction)(/*[in]*/long lPumpNumber,/*[in]*/ long lTrsNumber, /*[in]*/ short nNozzle,/*[in]*/long lVolume,/*[in]*/long lValue,/*[in]*/short nLevel,/*[in]*/long lGradePrice, /*[in]*/ short nGrade,/*[out,retval]*/ long *pVal);
	STDMETHOD(CancelTransaction)(/*[in]*/long lPumpNumber,/* [in] */ long lTrsNumber,/*[out,retval]*/ long *pVal);
	STDMETHOD(InitInterface)(/*[in]*/ BOOL bControllerIsMaster,/*[in]*/ BOOL bControllerGenTrsNumber, /*[in]*/ BSTR strControllerName, /*[in]*/ long lControllerVer, /*[out,retval]*/ long *pVal);
	STDMETHOD(WriteToLog)(/*[in]*/ long lPumpNumber, /*[in]*/ BOOL bErrorRep, /*[in]*/ long lError, /*[in]*/ BOOL bWarningRep ,/*[in]*/ long lWarning, /*[in]*/ BOOL bDeviceRep, /*[in]*/ long lDevice, /*[in]*/ long lDeviceCode, /*[in]*/ BOOL bMiscRep, /*[in]*/ long lMisc, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetPumpStatus)(/*[in]*/ long lPumpNumber, /*[in]*/ long lStatus, /*[in]*/ long lRes, /*[in]*/ long lResExt, /*[out,retval]*/ long *pVal);
	STDMETHOD(DoAutomaticPAKReservation)(/*[out]*/ BOOL *bAutoRes, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetCurrentTransactionNumber)(/*[in]*/ long lPumpNumber, /*[out]*/ long *pTrsNumber, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetRealTimeClock)(/*[out]*/ BSTR *psDateTime, /*[out.retval]*/ long *pVal);
	STDMETHOD(GetParam)(/*([in]*/ BSTR sTableName, /*[in]*/ long lKey1,/* [in] */ long lKey2, /*[in]*/ BSTR sFiledName, /*[in]*/ short nFiledType, /*[in,out]*/ BSTR *sVal,/*[in,out]*/ long *lVal, /*[in]*/ BOOL bCreate, /*[out,retval]*/ long *pVal);	
	STDMETHOD(TankReading)(/*[in]*/ long lTankNumber,/*[in]*/ long lProduct, /*[in]*/ short nFlags, /*[in]*/ long lStatus, /*[in]*/ long lFuelLevel, /*[in]*/ long lWaterLevel, /*[in]*/ long lTemperature, /*[in]*/ long lVolume, /*[in]*/ long lWaterVolume, /*[out,retval]*/ long *pVal);
	STDMETHOD(GradePrice)(/*[in]*/ long lPumpNumber, /*[in]*/ short nNozzle, /*[in]*/ short nGrade, /*[in]*/ short nFlags, /*[in]*/ long lPriceA, /*[in]*/ long lPriceB, /*[out,retval]*/ long *pVal);
	STDMETHOD(PumpMode)(/*[in]*/ long lPumpNumber, /*[in]*/ long lPumpMode, /*[out,retval]*/ long *pVal);
	STDMETHOD(PumpTotals)(/*[in]*/ long lPumpNumber, /*[in]*/ short nNozzle, /*[in]*/ short nGrade, /*[in]*/ short nFlags,/*[in]*/ long lVolumeH, /*[in]*/ long lVolumeL, /*[in]*/ long lValueAH, /*[in]*/ long lValueAL, /*[in]*/ long lValueBH, /*[in]*/ long lValueBL, /*[out,retval]*/ long *pVal);
	STDMETHOD(PAPSwipe)(/*[in]*/ long lPumpNumber, /*[in]*/ long lDeviceNumber, /*[in,out]*/ long *lSeqNumber, /*[in]*/ long lFlags, /*[in]*/ BSTR sData, /*[out, retval]*/ long *pVal);
	STDMETHOD(PriceDisplayPanelStatus)(/*[in]*/ long lDevice , /*[in]*/ long lStatus, /*[in]*/ BSTR sRec ,/*[out, retval] */long *pVal);
	STDMETHOD(LockPump)(/*[in]*/ long lPumpNumber, /*[in]*/ long lLock,/*[in]*/ long lFlags, /*[out,retval]*/ long *plVal);
	IGCIEvents *GetEventPtr();

protected:
	void GetCashAcceptorReport__(long   lTerminalId ,  long lFlags , BSTR  *sHeader, BSTR *sRecords , long * lRecordCount , long *pVal);
	void GetParam4__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long lFlags, long lDataBase, long *pVal);
	void GetOLABuffer__(long lPumpNumber , BSTR sCardData, BSTR * sOLABuffer, long lFlags, long* pRet);
	void QueryData__(long lFileNum, long *lIndexNumber, BSTR sKey, long lFlags, BSTR *pRec, long *pVal);
	void SetAlarm__(long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus, long lAlarmValue,long lFlags, long *pAlarmId,  BSTR sRec, long *pVal);
	void DisplayInfo__(long lPumpNumber , BSTR & sMsg , long lFlags , long * pRetVal);
	void TankReading3__(long lTankNumber, long lProduct, short nFlags, long lStatus1, long lFuelLevel, long lWaterLevel, long lTemperature, long lVolume, long lWaterVolume, long lStatus2, long lFuelValue, long lPricePerLitre, long lTcVolume, long lUllage, BSTR sRec, long *pVal);
	void SetDebitData__(long lPumpNumber, long lFlags, BSTR sData, long *pVal);
	void GetDebitDataInfo__(long lPumpNumber, long lFlags, BSTR *pData, long *pVal);
	void DebitProcessResultCmd__(long lPumpNumber, long lSessionId, long lFlags, BSTR sData, long *pVal);
	void GetPumpInfo__(long lPumpNumber, BSTR *pRec, long lFlags, long *pVal);
	void SetTrsState__(long lPumpNumber, long lTrsNumber, long lState, long lFlags, BSTR sData, long *plRetVal);
	void GetReceiptRawData2__(long lPumpNumber, BSTR sTrack2, short *nIndex, BSTR *sReceiptRawData, long lFlags, long *pVal);
	void GetSingleProductInfo__(long lPump, long lFlags, long *lGrade, long *pVal);
	void ReceiptPrinted__(BSTR sTrack2, long lIndex, long lFlags, long *pVal);
	BOOL FindTransactionNumberByRowIndex__(long lIndex, char* szTrack2, int *nOutTrnsNum); // RFUEL-3093
	short GetReportFreeEntry();
	void GetServiceFee__(long lPump, long lTrsType, long lTrsNumber ,BSTR stTime, long lFlags, long *pValue,long *pRetVal);
	void SetParam3__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal , BOOL bIniFile =FALSE);  
	void GetParam3__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long *pVal,BOOL bIniFile =FALSE);
	void SetProcess__(long lProcessNumber,long lParam1,long lParam2,long *pVal); //Udi 4.0.5.24 
	void GetReceiptRawData3__(long lPumpNumber, BSTR sTrack2, short *nIndex, BSTR *sReceiptRawData, long *plFlags, long *pVal);
	BOOL AVIAlreadyInUse(CARD_SALE_ALL3* pCardData, long lPumpNumber);	//4.0.20.503 66960
	void ReportRequest__(long lParam1, long lParam2, long lFlags, BSTR sData, long *lRefNum, long *pVal);
	void GetReceiptCounter__(long *plCounter, long lType, long lFlags, long *pVal);
	void GetCardInfo__(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *sRetData, long *pVal);
	GCIRetCode SwipeSaveData(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CARD_SALE_ALL3 *pCardData, BOOL *pbAskUserToTryAgain);
	GCIRetCode SwipeSaveDataEx(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CXMLInterface* pCardData, BOOL *pbAskUserToTryAgain);
	void GetReceiptRowData__(long lPumpNumber, BSTR sTrack2, short nIndex, BSTR *sReceiptRowData, long lFlags, long *pVal);
	void SetPAPSwipeData__(long lPumpNumber, long lFlags, BSTR sData, long *pVal);
	void CarWashStatus__(long lDevice, long lSrc, long lStatus, long *pVal);
	void WashCodeResult__(long lSyncNumber,long lDevice, long lResult, BSTR sData, long *pVal);
	void Alarm__(long lCatagory, long lCode, long lParameter, BSTR sStartDateAndTime, BSTR sEndDateAndTime, long *pVal);
	void GetPAPSwipeData__( long lPumpNumber,  long lFlags,  BSTR *pData, long *pVal);
	void AVIStop__( long lPumpNumber,  long lNozzleNumber, long *pVal);
	void AVIStart__( long lPumpNumber,  long lNozzleNumber, BSTR sData, long *pVal);
	void SetParam__( BSTR sTableName,  long lKey1,  long lKey2,  BSTR sFiledName,  short nFiledType,  BSTR sVal,  long lVal,  BOOL bCreate, long * pVal);
	void Watchdog__( long lParam, long *pVal);
	void InitInterface2__( BSTR strControllerName,  long lControllerVer,  long lFlags, long *pVal);
	void TankStatus__( long lTankNumber,  long lStatus, long *pVal);
	void GetGradePrice__( short nGrade, long *pVal);
	void DeliveryReport__( long lTankNumber,  long lProduct,  short nFlags,  long lStatus,  long lTag,  long lVolume,  BSTR sDateAndTime, long *pVal);
	void DeliveryReport2__(long lTankNumber, long lProduct, short nFlags, long lStatus, long lTag, long lVolume, BSTR sStartDateAndTime, BSTR sEndDateAndTime, long lStartVolume, long lEndVolume, long lTemperature, long *pVal);
	void FillDeliveryReport( IN OUT DELIVERY_INFO &info, long lTankNumber, long lProduct, long lStatus, short nFlags, long lVolume, long lStartVolume, long lEndVolume, long lTemperature, BSTR sStartDateAndTime, GCIRetCode &retCode, BSTR sEndDateAndTime,DWORD & dwLastErr); // 4.0.19.2170   CR 409564
	void TransactionStatusCS__(long lPumpNumber, long lTrsNumber,  short nNozzle,  long lStatus,long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long *pVal);
	void TransactionStatusCM__(long lPumpNumber, long lTrsNumber,  short nNozzle,   short nIndex,  long lStatus,long lOnOffer, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long *pVal);
	void RollingUpdate__(long lPumpNumber,  long lTrsNumber, long lVolume,  long lValue,  long lGradePrice,  short nGrade, long *pVal);	
	void PAPReadyTransaction__(long lPumpNumber, long lSeqNumber,long lTrsNumber, short nNozzle,long lVolume,long lValue,short nLevel,long lGradePrice,  short nGrade, long lFlags, BSTR sData,  BSTR *sRetData, long *pVal,BOOL bIsOfflineTrs = FALSE);
	
	void CancelInTheMiddleOfAuthorize(CARD_SALE_ALL3 & cardDataAll3, long lPumpNumber, OLA_STAT & ola, BOOL & bCancelInMiddleOfAuthorize,  CPumpTransact & trs, long lSeqNumber, GCIRetCode & retCode, BOOL & bMasterCancelOperation);
	BOOL  IsInTheMiddleOfOlaSession(OLA_STAT &ola);

	void PAKReadyTransaction__(long lPumpNumber, long lTrsNumber, short nNozzle, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long *pVal, long lFlage = 0, long lVal = 0, BSTR pRec = NULL);
	void CancelTransaction__(long lPumpNumber,long lTrsNumber, long *pVal);
	void InitInterface__( BOOL bControllerIsMaster, BOOL bControllerGenTrsNumber,  BSTR strControllerName,  long lControllerVer, long *pVal);
	void WriteToLog__( long lPumpNumber,  BOOL bErrorRep,  long lError,  BOOL bWarningRep , long lWarning,  BOOL bDeviceRep,  long lDevice,  long lDeviceCode,  BOOL bMiscRep,  long lMisc, long *pVal);
	void SetPumpStatus__( long lPumpNumber,  long lStatus,  long lRes,  long lResExt, long *pVal, long lFlags=0, short nGrade=0, short nPosition=0, BSTR sData=NULL); //4.0.19.500 - TD 59171
	void SetAutoAuthorizePump(long lPumpNumber, CPumpStatus &pumpStat);
	void RemoveLoyaltyTrs(long lPumpNumber, CPumpTransact &trs);

	GCIRetCode CalculateAnprAndFlowRate(long lFlags, BSTR pRec, GCIRetCode retCode, CPumpTransact &trs, EXTRA_INFO &cExtraInfoFlowRateAndAnpr, long lPumpNumber);

	void SetPumpStatusNewReservaionIsPayAtPumpOrPayAttendant( PumpReservation lNewRes, PumpStatus lNewStatus, CPumpStatus &pumpStat, PumpResExt lNewResExt, long lPumpNumber, BOOL& bChange, PumpResExt lPrevResExt );
	void SetPumpStatusNewRerservaionIsExtAttendant( long lPumpNumber, PumpStatus lNewStatus, PumpReservation lNewRes, GCIRetCode &retCode, CPumpStatus &pumpStat,short nUnpaid, PumpResExt lNewResExt, BOOL &bChange );
	void SetPumpStatusNewRerservaionIsPrePayAtPos(short nPrePayWaiting, CPumpStatus &pumpStat, PumpReservation lNewRes, PumpResExt lNewResExt, BOOL &bChange, PumpStatus lNewStatus, long lPumpNumber, GCIRetCode &retCode );
	void SetPumpStatusNewRerservaionIsPumpSrv( PumpResExt lNewResExt, long lPumpNumber, PumpReservation lNewRes, CPumpStatus &pumpStat, BOOL& bChange );
	void SetPumpStatusTerminalSrvFlow( PumpStatus lNewStatus, PumpReservation currentReservation, long p, BOOL &bAddSetPumpStatusEvent, long &lEventFlags, long &lEventParam1 );
	void SetPumpStatusAutoAuthoirze( CPumpStatus &pumpStat, long lPumpNumber );
	void ForceZeroComplete( long lPumpNumber );
	void DoAutomaticPAKReservation__( BOOL *bAutoRes, long *pVal);
	void GetCurrentTransactionNumber__( long lPumpNumber,  long *pTrsNumber, long *pVal);
	void GetRealTimeClock__( BSTR *psDateTime, long *pVal);
	void GetParam__(BSTR sTableName,  long lKey1,long lKey2,  BSTR sFiledName,  short nFiledType,  BSTR *sVal,long *lVal,  BOOL bCreate,long *pVal);	
	void TankReading__( long lTankNumber, long lProduct,  short nFlags,  long lStatus,  long lFuelLevel,  long lWaterLevel,  long lTemperature,  long lVolume,  long lWaterVolume, long *pVal);
	void TankReading2__( long lTankNumber, long lProduct,  short nFlags,  long lStatus1,  long lFuelLevel,  long lWaterLevel,  long lTemperature,  long lVolume,  long lWaterVolume, long lStatus2, long lFuelValue, long lPricePerLitre, long *pVal , long lTcVolume =0 , long lUllage = 0, BSTR sRec= NULL); //4.0.0.78
	void GradePrice__( long lPumpNumber,  short nNozzle,  short nGrade,  short nFlags,  long lPriceA,  long lPriceB, long *pVal);
	void PumpMode__( long lPumpNumber,  long lPumpMode, long *pVal);
	void PumpTotals__( long lPumpNumber,  short nNozzle,  short nGrade,  short nFlags, long lVolumeH,  long lVolumeL,  long lValueAH,  long lValueAL,  long lValueBH,  long lValueBL, long *pVal);
	void PumpTotals2__( long lPumpNumber,  short nNozzle,  short nGrade,  short nFlags, long lVolumeH,  long lVolumeL,  long lValueAH,  long lValueAL,  long lValueBH,  long lValueBL, long lPumpStatus, long lNozzleStatus, long *pVal);
	void PAPSwipe__( long lPumpNumber,  long lDeviceNumber, long *lSeqNumber,  long lFlags, BSTR sData, long *pVal);
	void LockPump__(long lPumpNumber, long lLock, long lFlags, long *plVal);
	
	void GetCardInfoEx__(long lClientId,BSTR sInBuffer,BSTR sInfoType,long lFlags, BSTR *sRetData, long *pVal);
	void TankReadingEx__(long lTankNumber, long lFlags, BSTR sRec, long *pVal); //4.0.13.503
	void ReadyOfflineTRS__(long lPumpNumber, long lFlags, BSTR sData, BSTR sParam1, long lParam1, long lParam2, BSTR *sRetData, long *pVal); //48194
	void SetPumpMisc__(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal);//TD 
	void SetPumpMiscExt__(long lPumpNumber, long lMiscExt, long bRemoveMisc, long *pVal);//TD 65268 4.0.20.451 //4.0.21.501
	void UpdateTrsDetails( CPumpTransact &trs, short nNozzle, long lVolume, long lValue, short nLevel, long lGradePrice, short nFinalGrade, long p, long lPumpNumber, long lFinalTrsNumber );
	void FillTankReadingInfo( IN OUT TANK_READ_INFO &info, long lTankNumber, long lProduct, long lStatus1, short nFlags, long lFuelLevel, long lWaterLevel, long lTemperature, long lVolume, long lWaterVolume, long lStatus2, long lFuelValue, long lPricePerLitre, long lTcVolume, long lUllage ); // 4.0.19.2170   CR 409564
	void GetTankReadingTimeFromINIFile(TANK_READ_INFO *Data); //443035
	int  SaveTankReading(TANK_READ_INFO *Data);//443035

	AVINextState CheckAviConditions(CPumpStatus *pStat, PumpReservation lNewRes, PumpResExt lNewResExt);
	GCIRetCode SwipeCancel(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CARD_SALE_ALL3 *pCardData, BOOL *pbAskUserToTryAgain);	//4.0.20.503 66960
	GCIRetCode SwipeRequest(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CARD_SALE_ALL3* pCardData, BOOL *pbAskUserToTryAgain, long lFlags =0);  //4.0.14.500 //4.0.20.503 66960

	void HandlePreAuthorizeScreen( long lPumpNumber, long lSeqNumber, CARD_SALE_ALL3* pCardData, PAY_AT_PUMP_INFO cInfo, OLA_STAT * pOla );
	void HandleParitalApprovalScreen( long lPumpNumber, long lSeqNumber, CARD_SALE_ALL3* pCardData, PAY_AT_PUMP_INFO &cInfo, OLA_STAT * pOla );
	void ExecuteManualFirstSwipe( long lPumpNumber, CARD_SALE_ALL3* pCardData );
	void PakToPapSwipe( OLA_STAT * pOla, CARD_SALE_ALL3* pCardData, long lPumpNumber, long lSeqNumber );
	short ConvertLoyaltyStatusToOlaStatus( OLA_STAT * pOla, CARD_SALE_ALL3* pCardData, eFuelLoyaltyState eLoyaltyFuel, long lPumpNumber,LOYALTY_FLOW_TYPE &eCurrentFlowType );
	GCIRetCode SwipeExtraInfo(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CARD_SALE_ALL3* pCardData,BOOL *pbAskUserToTryAgain); //4.0.14.500	//4.0.20.503 66960
	GCIRetCode SwipeReceiptCheck(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla ,CARD_SALE_ALL3 *pCardData,BOOL *pbAskUserToTryAgain); //4.0.5.0.//4.0.20.503 66960
	GCIRetCode SwipeReceiptUpdate(long lPumpNumber,long lSeqNumber,OLA_STAT *pOla,CARD_SALE_ALL3 *pCardData,BOOL *pbAskUserToTryAgain);	//4.0.20.503 66960
	BOOL ReSendWithExtraInfo(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CARD_SALE_ALL3 *pCardData);
	BOOL ReSendWithExtraInfoEx(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CXMLInterface* pCardData); //4.0.14.500
	void SaveZeroPakTrs(long lPumpNumber, CPumpTransact &trs);

	long		StopCmdTxThread();
	long		StartCmdTxThread();
	void		ConstructThreadInfo();
	void		DeleteThreadInfo();
	
	static unsigned int _stdcall CmdTx(void *pv);

// Thread Data :
	struct CThreadInfo 
	{
		public:
		CServerMain *m_pServer;
		HANDLE m_hEventStartThread;
		HANDLE m_hEventStopThread;
		HANDLE m_hEventKillThread;
		HANDLE m_hEventThreadKilled;
		CGCI   *m_pGCI;	
	}m_CmdTxThreadInfo;

	HANDLE		m_hThread;
	unsigned int m_usThreadId;

	long QueueHandle(CThreadInfo* pInfo);								// 4.0.23.507 TD 8904

	void ExecuteSwipeResult( CCmdInfo &Cmd, CString &strCmdName, CThreadInfo* pInfo, HRESULT &hRes, IGCIEvents* pEvent, long lEventRetCode );

	HRESULT AuthoirzeAndLimit( HRESULT hRes, IGCIEvents* pEvent, CCmdInfo &Cmd, CMD_STRUCT_AUTHORIZE_LIMIT * pCmdInfo, long lEventRetCode );
	
	HRESULT AuthoirzeAndLimitWithLoyaltyDiscountData( CMD_STRUCT_AUTHORIZE_LIMIT * pCmdInfo, CCmdInfo &Cmd, CThreadInfo* pInfo, HRESULT hRes, IGCIEvents* pEvent, long lEventRetCode );
	void ReconfigurationEvent(long lTableId , long lSubTableId , long lSubTableIdExt , long lFlags);

private:
	bool IsReservationPakWithLoyaltyAsPap(long lPumpNumber); //4.0.29.500 283329
	void SetCashAcceptorStatus__(long lTerminalId, long lFlags, long lStatus, BSTR *sRec, long *pVal);
	void SetCashAcceptorStatus( long   lTerminalId ,   long lFlags ,  long lStatus, BSTR sRec, long *pVal);
	void GetAttTrs__(long lPumpNumber, long lFlags, BSTR sIndex, BSTR *sRec, long *pVal);
	void SetParam4__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long lFlags, long lDataBase, long *pVal);
	void ReleasePumpTotalPump(long lPumpNumber);
	void GetTankMemoryData__(long lTankNumber, BSTR *pRec, long lFlags, long *lCounter, long *pVal);
	void GetWetStockData__(long lWetStockFlags, long *pVal);
	void UploadBuffer__(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData, long *pVal);
	void GetStationStatus__(long *lPumpMap1, long *lPumpMap2, long lFlags, long *pRetVal);
	void SetVersion__(BSTR sVersionList, long lFlags ,long *pVal);
	void ParseOLABuffer__(long lPumpNumber, BSTR *cCardSaleALL3, BSTR sOLABuffer, long lFlags, long *pRet);
	void GetPAKQuery__(BSTR sRestrictionData, BSTR *sOutData, long *plRecCount, long lFlags, long *plRetVal );
	void SetConnectivityStatus__(BSTR sPSConnStatus, long lFlags, long *pVal);
	void GetCardInfo3__(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, long lFlags, long *lSesionType ,long lPumpNumber,BSTR *sRetData, long *pVal);
	void SetPumpMode__(long lPumpNumber, long lPumpMode, long lFlags, long *pVal); //4.0.5.41
	void GetPumpMode__ (long lPumpNumber, long *plPumpMode, long *plFlags, long *plMsgNumber, long *plVal); //4.0.5.41
	void MoveTerminalTransactionToPump__(long lTerminalId, long lPumpId,long lStatus, long lFlags, long *pVal);
	void GetCardInfo2__(long lPumpNumber, long lDeviceNumber, long *lSeqNumber, long lFlags, BSTR sData, long *pVal);
	void SetMaintenance__(long lLock, long lParam, long *pVal);
	void SetShift__(short nDayOfWeek, short nShiftNumber, short nShiftFlags, BSTR sStartTime, long *pVal);
	void GetCashAcceptorData__(long lPumpNumber, long lDevice, long lSeqNum, long lFlags, BSTR *CashSaleData, long *pVal);
	void SetCashAcceptorData__(long lPumpNumber, long lDevice, long lSeqNum, long lFlags, BSTR CashSaleData, long *pVal);
	void DeliveryReportEx__(long lTankNumber, long lFlags, BSTR sRec, long *pVal); //4.0.13.504
	void	SetGradesRestriction__(BSTR sTagPumpsNuzzlesRestrict, long lFlags, long *pVal);//4.0.9.506 //amram
	void	GetTransListQuery__(long lPumpNumber,BSTR *sData, long *plRecCount, long *lFlags, long *pVal);
	
	eFuelLoyaltyState	 GetLoyaltyState(CARD_SALE_ALL3 * cardDataAll3,long lFlags,long lPumpNumber); //4.0.22.500 70767

	void SavePapInfoFromLoyaltyResult(PAY_AT_PUMP_INFO &cTmpInfo, PAY_AT_PUMP_INFO &cTmpCardSaleData , long lMediaMasterType);
	void SaveMediaMasterResultToPapInfo(PAY_AT_PUMP_INFO &cTmpInfo, PAY_AT_PUMP_INFO &cGetCardPapInfoResult, long lMediaMasterType, long lPumpNumber, CARD_SALE_ALL3 * cardDataAll3);

	BOOL	CheckFirstManualCard(CARD_SALE_ALL3 * cardDataAll3, long lFlags, long lPumpNumber); //4.0.22.506 67786
	long	HandleLoyaltyResult(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo, BOOL bIgnoreParamResult, BOOL bCheckLoyaltyCard); //4.0.20.440 68857	4.0.22.504 //4.0.23.240 //4.0.24.60

	void HandleLoyaltyResultCopyLoyaltyData(PAY_AT_PUMP_INFO * pInfo, long &lMediaMasterType, long lPumpNumber);

	void SaveTempPapInfo(PAY_AT_PUMP_INFO * pInfo, CARD_SALE_ALL3 &cTmpCardSaleData, long lPumpNumber);
	void GetCardInfoResultToPapInfo(PAY_AT_PUMP_INFO * pInfo, CARD_SALE_ALL3 &cTmpCardSaleData, long lMediaMasterType, long lPumpNumber);	//CR 404913 

	void    CheckIfPumpNeedToBeLock(long lPumpNumber);//4.0.13.501
	void    HandleDeliveryEmulationSingleTankProcess(const long lTankNumber, const long lVolume, const long lTemperature, const long lWaterVolume, const short nFlags, long lDensity = 0, long lWeight = 0, long lProductLevel = 0, long WaterLevel = 0); // 4.0.25.1060  TD 377336
	void	HandleDeliveryEmulationProcess(long lTankNumber, long lVolume, long lTemprature, long lWaterVolume ,short nFlags);		//4.0.20.52
	void	SaveTrsVolumeForDeliveryEmulation(long lPumpNumber, short nNozzle, long lVolume);					//4.0.20.52
	BOOL    IsValidPAPMode(const long lPumpNumber);                                    //4.0.21.501 TD 67443
	BOOL    IsValidModeForPayment(const long lPumpNumber, const long lMediaMasterType);//4.0.21.501 TD 67443
	void	ReportReady__(long lFlags, BSTR wszReportText, long lParam, long *pVal);	//4.0.22.501 TD 69905
	void	CarWashGeneralReqResponse__(long lDevice, long lResult, long lFlags1, long lFlags2, BSTR sData, long *pVal); //4.0.23.504 76432
	void	UpdateTotalDiscountInExtraInfoRec(long lPumpNumber, TAG_EXTRA_INFO  &cTagExtraInfo, long lGrade, long lGradePrice, CPumpTransact &trs, long lValue); //4.0.23.190 105087 4.0.24.60
	void	UpdatePakLoyaltyExtraInfo(long lPumpNumber, CPumpTransact &cTrs, EXTRA_INFO &cExtraInfoFlowRateAndAnpr, long lFlags, BSTR pRec, GCIRetCode &retCode, long lGrade, long lGradePrice, long lValue, long lVolume); //4.0.23.190 105087 4.0.24.60

	void SetPromotionId(CPumpTransact &cTrs, TAG_EXTRA_INFO &cTagExtraInfo, PAY_AT_PUMP_INFO &PayAtPumpInfo);

	void	CopyLoyaltyLockingKeyToExtraInfo(TAG_EXTRA_INFO & cTagExtraInfo, PAY_AT_PUMP_INFO & PayAtPumpInfo, long lPumpNumber);
	void	CopyLoyaltyMemberInfoToExtraInfo(TAG_EXTRA_INFO & cTagExtraInfo, PAY_AT_PUMP_INFO & PayAtPumpInfo, long lPumpNumber);
	void	UpdatePrePayExtraInfo(long lPumpNumber, CPumpTransact &cTrs, long lFlags, BSTR pRec, GCIRetCode &retCode, long lGrade, long lGradePrice, long lValue); //4.0.23.190 105087 4.0.24.60
	void	UpdateDiscountsArray(long lPumpNumber, EXTRA_INFO_REC	&cExistExtraInfoRec, PAY_AT_PUMP_INFO &PayAtPumpInfo, TAG_EXTRA_INFO  &cTagExtraInfo, BOOL bInitializeData); //4.0.23.190 105087 4.0.24.60
	void	InitLoyaltyPapInfo(long lPumpNumber, PAY_AT_PUMP_INFO &PayAtPumpInfo);//4.0.23.190 105087 4.0.24.60
	void	UpdatePapInfoWithPreAuthAmount(long lPumpNumber, CPumpTransact &trs); //4.0.25.30 111474
	void	SetPumpTestToTrs(long lPumpNumber, CPumpTransact *pTrs); //TD 231472 4.0.27.60
	void	SavePakLoyaltyExtraInfoXml(TAG_EXTRA_INFO  &cTagExtraInfo, CPumpTransact *cTrs, long lPumpNumber, CString *sExtraInfoXml, long lGrade, long lGradePrice, long lValue, long lVolume); // RFUEL-520
	
	void	SetPumpTestToTrsMisc(long lTestOrCalibMode, long lNotMisc, const char* const sMiscStr, CPumpTransact* pTrs, long lPumpNumber);
	
	void	SetPumpMiscExtPumpTest(long lPumpNumber, long lFlags);   //TD 231472 4.0.27.60
	BOOL    IsAutoAuthorize(long lPumpNumber, CPumpStatus cOldPumpStat, long lNewStatus, long lNewRes, long lNewResExt, long nPosition);  //4.0.25.330 - TD 268644
	long	UpdatePrePayExtraInfoWithCarWashData(TAG_EXTRA_INFO & cTagExtraInfo , EXTRA_INFO & cExtraInfo);
	void	SetMiscExtAuthorizeSentIndication(long lPumpNumber, CPumpStatus *cPumpStatus, BOOL bAddMiscExt);	//4.0.30.500 - TD 320068
	BOOL	IsPrePayUnbalanced( CPumpTransact &trs, long lServiceFee, PRE_PAY_INFO &cTmpPrePayinfo );		//4.0.30.500 - TD 325958
	void	HandleLoyaltyPhoneLookUp(long lPumpNumber, long lSeqNumber , OLA_STAT *pOla, PAY_AT_PUMP_INFO & cInfo, CARD_SALE_ALL3* pCardData);
	void	LoyaltyEndPapTrs( long lPumpNumber, long lFlags , long lGradePrice , long lValue , long lVolume , short nGrade , PAY_AT_PUMP_INFO &info, CPumpTransact &trs, OLA_STAT &ola , GCIRetCode & retCode );

	void AuthorizeAndLimitUpdateLimitFromTerminalSrv( CPumpStatus &stat, CMD_STRUCT_AUTHORIZE_LIMIT * pCmdInfo, CThreadInfo* pInfo, CCmdInfo &Cmd );
	HRESULT AuthoirzeAndLimitWithMutipleLimits( CMD_STRUCT_AUTHORIZE_LIMIT * pCmdInfo, CCmdInfo &Cmd, CThreadInfo* pInfo, IGCIEvents* pEvent, long lEventRetCode );
	HRESULT AuthoirzeAndLimitWithFleetOrReefer( CMD_STRUCT_AUTHORIZE_LIMIT * pCmdInfo, CThreadInfo* pInfo, CCmdInfo &Cmd, IGCIEvents* pEvent, long lEventRetCode );
	void	SetLoyaltyTenderAmount( IN OUT  PAY_AT_PUMP_INFO * info, IN long lTotalTransactionAmount);
	void PriceDisplayPanelStatus__(long lDevice, long lPriceDisplayStatus , BSTR sFiller ,long *pVal)/*CR 404591 */;
	void SetPumpStatusNewRerservaionIsNullRes(PumpResExt lPrevResExt, long lPumpNumber, PumpReservation &lNewRes, CPumpStatus &pumpStat, PumpResExt lNewResExt, BOOL &bChange );
	void	ExecuateAuthorizeAndLimit( CString &strCmdName, CCmdInfo &Cmd, CThreadInfo* pInfo, HRESULT &hRes, IGCIEvents* pEvent, long lEventRetCode );

	//void ExecuteSwipeResultXmlFormat( CCmdInfo &Cmd, CString &strCmdName, BOOL &bRet, CThreadInfo* pInfo, HRESULT &hRes, IGCIEvents* pEvent, long lEventRetCode );
	//CR_475695_Start
	void SetTankReadingRequestType(short *nFlags); //4.0.24.1010
	void SetPumpReadingRequestType(short *nFlags); //4.0.24.1010
	//CR_475695_End
	BOOL ShouldUpdatePAKExtraInfo(long lPumpNumber, long lFlags);
};

#endif //__GCI_H_
