// Ctrl.h : Declaration of the CCtrl

#ifndef __CTRL_H_
#define __CTRL_H_

#include "resource.h"       // main symbols
#include "PumpSrvCP.h"

#define SETUP_EX_FUEL_GENERAL_CONFIGURATION		0x00000001		//4.0.22.45 TD 72716

/////////////////////////////////////////////////////////////////////////////
// CControl
class ATL_NO_VTABLE CCtrl : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CCtrl, &CLSID_Ctrl>,
	public IDispatchImpl<ICtrl, &IID_ICtrl, &LIBID_PUMPSRVLib>,
	public CProxyICtrlEvents< CCtrl >,
	public IConnectionPointContainerImpl<CCtrl>
{
public:
	CCtrl()
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			_Module.m_server.m_cLogger.LogMsg("Ctrl Connection established !!  ");
		}
	}

	~CCtrl()
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			_Module.m_server.m_cLogger.LogMsg("Ctrl Close Connection !!  ");
		}
	}


DECLARE_REGISTRY_RESOURCEID(IDR_CTRL)

BEGIN_COM_MAP(CCtrl)
	COM_INTERFACE_ENTRY(ICtrl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()



// ICtrl
public:
	STDMETHOD(GetTankState)(/*[in]*/ long lTankNumber,  /*[in]*/ long lFlags, /*[in, out]*/ BSTR* pRec, /*[out , retval]*/ long * pRetVal); //4.0.25.1130 
	STDMETHOD(SetTankState)(/*[in]*/ long lTankNumber, /*[in]*/ long lState, /*[in]*/ long lFlags, /*[in]*/ BSTR sRec, /*[out , retval]*/ long * pRetVal); //4.0.25.1130 
	STDMETHOD(GetConfiguration)(/*[in]*/    BSTR  sClientName, /*[out]*/  BSTR * sConfigurationData, /*[out, retval]*/ long *pVal);
	STDMETHOD(SetAlarm)(/*[in]*/ long lCatagory, /*[in]*/ long lSubCatagory, /*[in]*/ long lAlarmNumber,/*[in]*/ long lDevice,  /*[in]*/ long lAlarmStatus, /*[in]*/ long lAlarmValue, /*[in]*/ long lFlags,/*[out]*/ long *pAlarmId, /*[in]*/ BSTR sRec, /*[out, retval]*/ long *pVal);
	STDMETHOD(SetServiceFee2)(/*[in]*/ long lFeeNumber, /*[in]*/ BSTR sName,/*[in]*/ long lFlags ,/*[in]*/ long lPakFee, /*[in]*/ long lPrePayFee,/*[in]*/ long lPapFee, /*[in]*/ long lAviFee, /*[in]*/ long lAttendFee,/*[in]*/ long lSelfPakFee, /*[in]*/ long lSelfPrePayFee,/*[in]*/ long lSelfPapFee, /*[in]*/ long lSelfAviFee, /*[in]*/ long lSelfAttendFee, /*[out, retval]*/ long *pRetVal);
	STDMETHOD(GetPumpModeScheme3)(/*[in]*/ long lPumpNumber, /*[in]*/ long lScheme, /*[out]*/  long  *plMode , /*[out]*/   BSTR *psPeriodDateTimeStart, /*[out]*/  BSTR *psPeriodDateTimeEnd, /*[out]*/   BSTR *psDateTimeStart, /*[out]*/ BSTR *psDateTimeEnd, /*[out]*/ long *plServiceFeeNo, /*[out]*/   long *plFlags, /*[out, retval]*/  long *pVal);
	STDMETHOD(SetPumpModeScheme3)(/*[in]*/  long lPumpNumber,  /*[in]*/  long lScheme,  /*[in]*/ long lMode, /*[in]*/  BSTR sPeriodDateTimeStart, /*[in]*/  BSTR sPeriodDateTimeEnd, /*[in]*/  BSTR sDateTimeStart, /*[in]*/  BSTR sDateTimeEnd,  /*[in]*/ long lServiceFeeNo, /*[in]*/  long lFlags,  /*[out, retval]*/  long *pVal);
	STDMETHOD(SetServiceFee)(/*[in]*/ long lFeeNumber,/*[in]*/ BSTR sName,/*[in]*/ long lFlags ,/*[in]*/long lPakFee,/*[in]*/ long lPrePayFee,/*[in]*/ long lPapFee,/*[in]*/ long lAviFee, /*[in]*/long lAttendFee,/*[out, retval]*/ long *pRetVal);
	STDMETHOD(GetServiceFee)(/*[in]*/ long lPump, /*[in]*/ long lTrsType,/*[in]*/ long lTrsNumber, /*[in]*/ BSTR stTime, /*[in]*/ long lFlags, /*[out]*/ long* pValue,    /*[out, retval]*/long  *pRetVal);
	STDMETHOD(GetPumpModeScheme2)(/*[in]*/ long lPumpNumber, /*[in]*/ long lScheme, /*[out]*/  long  *plMode , /*[out]*/   BSTR *psPeriodDateTimeStart, /*[out]*/  BSTR *psPeriodDateTimeEnd, /*[out]*/   BSTR *psDateTimeStart, /*[out]*/ BSTR *psDateTimeEnd,  /*[out]*/   long *plFlags, /*[out, retval]*/  long *pVal);
	STDMETHOD(SetPumpModeScheme2)(/*[in]*/  long lPumpNumber,  /*[in]*/  long lScheme,  /*[in]*/ long lMode, /*[in]*/  BSTR sPeriodDateTimeStart, /*[in]*/  BSTR sPeriodDateTimeEnd, /*[in]*/  BSTR sDateTimeStart, /*[in]*/  BSTR sDateTimeEnd,  /*[in]*/  long lFlags,  /*[out, retval]*/  long *pVal);
	STDMETHOD(SetGradePrice2)(/*[in]*/ long lPumpNumber, /*[in]*/ short nNozzle, /*[in]*/ short nGrade,  /*[in]*/ long lMode, /*[in]*/ short nLevel, /*[in]*/ long lPrice, /*[in]*/ BSTR sTimeValid, /*[in]*/ short nFlags, /*[in]*/ long lTaxRate, /*[in]*/ long lDiscountPerUnit , /*[in]*/ long lMaxUnitForDiscount, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetParam3)(/*[in]*/  BSTR sParamPosition, /*[in]*/  long lKey1, /*[in]*/ long lKey2, /*[in]*/ short nFiledType,  /*[in]*/ BSTR sVal, /*[in]*/  long lVal, /*[in]*/ BOOL bCreate, /*[out, retval]*/  long *pVal);
	STDMETHOD(GetParam3)(/*[in]*/ BSTR sParamPosition, /*[in]*/ long lKey1, /*[in]*/  long lKey2, /*[in]*/  short nFiledType, /*[in,out]*/  BSTR *sVal, /*[in,out]*/  long *lVal, /*[in]*/  BOOL bCreate,  /*[out, retval]*/  long *pVal);
	STDMETHOD(SetMaintenance)(/*[in]*/ long lLock, /*[in]*/ long lParam, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetPOSInfo)(/*[out]*/ BSTR *sPOSInfo, /*[out, retval]*/ long *pVal);
	STDMETHOD(ReConfigure)(/*[in]*/ long lConfigureFlags, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetWetStockData)(/*[in]*/ long lWetStockFlags, /*[out,retval]*/ long *pVal);
	STDMETHOD(StartService)(/*[in]*/ long lService, /*[out,retval]*/ long *pVal);
	STDMETHOD(StopService)(/*[in]*/ long lService, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetServiceStatus)(/*[in]*/ long lService,/*[out]*/ long *lStatus, /*[out,retval]*/ long *pVal);
	STDMETHOD(RemoveAlarm)(/*[in]*/ short nFlags, /*[in]*/ long lRecIndex, /*[out,retval]*/ long * pVal);
	STDMETHOD(GetAlarm)(/*[in]*/ short nFlags, /*[in,out]*/ long *lIndexNumber,/*[out]*/ long *plCatagory, /*[out]*/ long *plCode, /*[out]*/ long *plParameter, /*[out]*/ BSTR *psStartDateAndTime, /*[out]*/ BSTR *psEndDateAndTime,/*[out]*/ long *plActiveReadNumber, /*[out]*/ long *plShiftNumber, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetDeliveryReport2)(short nFlags, long *lIndexNumber,long *lTank,long *lProduct,long *lStatus,short *nDeliveryFlags,long *lTag,long  *lDeliveredVolume,long *lActiveReadNumber,long *lShiftNumber,BSTR *sStartDeliveryDateTime,BSTR *sEndDeliveryDateTime,long *lStartVolume,long *lEndVolume,long *lTemperature,long *pVal);
	STDMETHOD(GetPumpTotals2)(short nFlags, long *lIndexNumber, long *lPump, short *nNozzle, short *nGrade, short *nTotalsFlags, BSTR *sVolume, BSTR *sValueA, BSTR *sValueB, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime,long *lPumpStatus, long *lNozzleStatus, long *pVal);
	STDMETHOD(GetTankReading2)(short nFlags, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus1, short *nReadingFlags, long *lFuelLevel, long *lWaterLevel, long *lTemperature, long *lFuelVolume, long *lWaterVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *lStatus2, long *lFuelValue, long *lPricePerLitre, long *pVal);
	STDMETHOD(SetRecord)(/*[in]*/ BSTR sTableAndVal, /*[in]*/ long lKey1,/*[in]*/ long lKey2,/*[out,retval]*/ long *pVal);
	STDMETHOD(GetRecord)(/*[in,out]*/ BSTR *sTableAndVal, /*[in]*/ long lKey1,/*[in]*/ long lKey2,/*[out,retval]*/ long *pVal);
	STDMETHOD(GetParam2)(/*[in,out]*/ BSTR *sTableFieldAndVal,/*[in]*/ long lKey1,/*[in]*/ long lKey2,/*[in]*/ short nFiledType,/*[out]*/ long *lVal, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetParam2)(/*[in]*/ BSTR sTableFieldAndVal, /*[in]*/ long lKey1,/*[in]*/ long lKey2,/*[in]*/ short nFiledType,/*[in]*/ long lVal,/*[in]*/ BOOL bCreate, /*[out,retval]*/ long * pVal);
	STDMETHOD(SetParam1)(/*[in]*/ BSTR sTableAndFieldName, /*[in]*/ long lKey1,/*[in]*/ long lKey2,/*[in]*/ short nFiledType,/*[in]*/ BSTR sVal,/*[in]*/ long lVal,/*[in]*/ BOOL bCreate, /*[out,retval]*/ long * pVal);
	STDMETHOD(GetParam1)(/*[in]*/ BSTR sTableAndFieldName,/*[in]*/ long lKey1,/*[in]*/ long lKey2,/*[in]*/ short nFiledType,/*[out]*/ BSTR *sVal,/*[out]*/ long *lVal, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetModeDef)(/*[in]*/ long lMode,/*[out]*/ BSTR *psName, /*[out]*/ long *plFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(SetModeDef)(/*[in]*/ long lMode, /*[in]*/ BSTR sName, /*[in]*/ long lFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetPumpModeScheme)(/*[in]*/ long lPumpNumber, /*[in]*/ long lScheme, /*[out]*/ long  *plMode, /*[out]*/ BSTR *psDateTimeStart, /*[out]*/ BSTR *psDateTimeEnd, /*[out]*/ long *plFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(SetPumpModeScheme)(/*[in]*/ long lPumpNumber, /*[in]*/ long lScheme, /*[in]*/ long lMode, /*[in]*/ BSTR sDateTimeStart, /*[in]*/ BSTR sDateTimeEnd, /*[in]*/ long lFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetParam)(/*[in]*/ BSTR sTableName, /*[in]*/ long lKey1, /*[in]*/ long lKey2, /*[in]*/ BSTR sFiledName, /*[in]*/ short nFiledType, /*[in,out]*/ BSTR *sVal, /*[in,out]*/ long *lVal, /*[in]*/ BOOL bCreate, /*[out,retval]*/ long * pVal);
	STDMETHOD(SetParam)(/*[in]*/ BSTR sTableName, /*[in]*/ long lKey1, /*[in]*/ long lKey2, /*[in]*/ BSTR sFiledName, /*[in]*/ short nFiledType, /*[in]*/ BSTR sVal, /*[in]*/ long lVal,/*[in]*/ BOOL bCreate, /*[out,retval]*/ long * pVal);
	STDMETHOD(GetProcessStatus)(/*[in]*/ long lProcessNumber, /*[out]*/ long *lProcessStatus, /*[out]*/ long *lPumpNumberA, /*[out]*/ long *lPumpProblemA, /*[out]*/ long *lPumpNumberB, /*[out]*/ long *lPumpProblemB, /*[out]*/ long *lTankNumber, /*[out]*/ long *lTankProblem, /*[out]*/ long *lFlags, /*[out,retval]*/ long *pVal);
	STDMETHOD(ControlStatus)(/*[in]*/ short nFlags, /*[in,out]*/ long *lTotalIndex, /*[in,out]*/ long *lTankReadIndex, /*[in,out]*/ long *lDeliveryIndex, /*[out]*/ long *lActiveShiftNumber, /*[out]*/ short *nStationStatus,/*[out,retval]*/ long *pVal);
	STDMETHOD(SetRealTimeClock)(/*[in]*/ BSTR sDateAndTime, /*[out,retval]*/ long *pVal);
	STDMETHOD(RemoveDeliveryReport)(/*[in]*/ short nFlags, /*[in]*/ long lRecIndex, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetDeliveryReport)(/*[in]*/ short nFlags, /*[in,out]*/ long *lIndexNumber, /*[out]*/ long *lTank, /*[out]*/ long *lProduct, /*[out]*/ long *lStatus, /*[out]*/ short *nDeliveryFlags,/*[out]*/ long *lTag, /*[out]*/ long  *lDeliveredVolume, /*[out]*/ long *lActiveReadNumber, /*[out]*/ long *lShiftNumber, /*[out]*/ BSTR *sDeliveryDateTime, /*[out,retval]*/ long *pVal);
	STDMETHOD(RemoveTankReading)(/*[in]*/ short nFlags, /*[in]*/ long lRecIndex, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetTankReading)(/*[in]*/ short nFlags, /*[in,out]*/ long *lIndexNumber, /*[out]*/ long *lTank, /*[out]*/ long *lProduct, /*[out]*/ long *lStatus, /*[out]*/ short *nReadingFlags, /*[out]*/ long *lFuelLevel, /*[out]*/ long *lWaterLevel, /*[out]*/ long *lTemperature, /*[out]*/ long *lFuelVolume, /*[out]*/ long *lWaterVolume, /*[out]*/ long *lActiveReadNumber, /*[out]*/ long *lShiftNumber, /*[out]*/ BSTR *sDateTime, /*[out,retval]*/ long *pVal);
	STDMETHOD(RemovePumpTotals)(/*[in]*/ short nFlags, /*[in]*/ long lRecIndex, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPumpTotals)(/*[in]*/ short nFlags, /*[in,out]*/ long *lIndexNumber, /*[out]*/ long *lPump, /*[out]*/ short *nNozzle, /*[out]*/ short *nGrade, /*[out]*/ short *nTotalsFlags, /*[out]*/ BSTR *sVolume, /*[out]*/ BSTR *sValueA, /*[out]*/ BSTR *sValueB, /*[out]*/ long *lActiveReadNumber, /*[out]*/ long *lShiftNumber, /*[out]*/ BSTR *sDateTime, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetShift)(/*[in]*/ short nDayOfWeek, /*[in]*/ short nShiftNumber, /*[out]*/ short *nShiftFlags, /*[out]*/ BSTR *sStartTime, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetShift)(/*[in]*/ short nDayOfWeek, /*[in]*/ short nShiftNumber, /*[in]*/ short nShiftFlags, /*[in]*/ BSTR sStartTime, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetGradePrice)(/*[in]*/ long lPumpNumber, /*[in]*/ short nNozzle, /*[in]*/ short nGrade,  /*[in]*/ long lMode, /*[in]*/ short nLevel, /*[out]*/ long *lPrice, /*[out]*/ BSTR *sDateTimeValid, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetGradePrice)(/*[in]*/ long lPumpNumber, /*[in]*/ short nNozzle, /*[in]*/ short nGrade,  /*[in]*/ long lMode, /*[in]*/ short nLevel, /*[in]*/ long lPrice, /*[in]*/ BSTR sTimeValid, /*[in]*/ short nFlags, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPumpMode)(/*[in]*/ long lPumpNumber, /*[out]*/ long *lMode, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetPumpMode)(/*[in]*/ long lPumpNumber, /*[in]*/ long lMode, /*[out,retval]*/ long *pVal);
	STDMETHOD(CloseEventsInterface)(/*[in]*/ long lClientId,/*[out,retval]*/ long* pVal);
	STDMETHOD(InitEventsInterface)(/*[in]*/ long lFlags,/*[in,out]*/ long *pClientId,/*[out,retval]*/ long* pVal);
private:	
	void SetupEx__(BSTR sXmlRec, long lFlags, long *pVal); //4.0.16.500 - CR 34815
	void SetServiceFeeMode__(long lPumpNumber, long lFeeSet, long lFlags, long *pVal);
	void GetServiceFeeMode__(long lPumpNumber, long *lFeeSet, long lFlags, long *pVal);

	long GetShiftSales(PS_SHIFT_SALES *sRec,BSTR *sData);
	long GetStartupRecord(PS_REC_STARTUP* sRec, BSTR *sData);
	void SetAlarm__(long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus, long lAlarmValue, long lFlags, long *pAlarmId, BSTR sRec, long *pVal); //4.0.25.994 - TD 325383
	void GetGeneralData__(long lRecordType, BSTR *sData, long lFlags, long *pVal);
	void SetAccumulateState__(long lShiftNumber, long lPosNumber,long lState, long lFlags, long *pVal);
	void GetAccumulateTotals__(long lShiftNumber ,  long lState, long lPosNumber, long lFlags,  long lVal, BSTR sStartTime, BSTR sEndTime, long *lRecordCount,   BSTR* sRec, long *pVal);
	void GetConnectivityStatus__(BSTR *sPSConnStatus, long lFlags, long *pVal);
	void TankGaugeData__(long lTankNumber, long lFlags, BSTR sRec, long *pVal);
	void DBTankGauge__(long lOffset, BSTR *sRec, long lRecSize, long lFlags, long *pVal);
	void GetTankMemoryData__(long lTankNumber, BSTR *pRec, long lFlags, long *lCounter ,long *pVal);
	void GeneralSystemRequest__(long lVal, BSTR sRec, long lFlags, long *plVal);
	void GetDeliveryInfo__(long lTankNumber , BSTR * pRec ,  long lFlags , long *pRetVal);
	void GetPumpTotalInfo__(long lPumpNumber , long lFlags , BSTR * pRec , long * pRetVal);
	void GetTankInfo__(long lTankNumber, BSTR *pRec, long lFlags, long *pRetVal);
	void GetPumpDevices__(long lGrade, long lNacsCode, long lDevice, long lFlags, long *pVal, long *pRetVal);
	void ReloadPricePolesData__(long lFlags,long *pVal);
	void GetTankReading3__(short nOpCode, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus1, short *nReadingFlags, long *lFuelLevel, long *lWaterLevel, long *lTemperature, long *lFuelVolume, long *lWaterVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *lStatus2, long *lFuelValue, long *lPricePerLitre,long *lTcVolume, long *lUllage, long *lFlags, BSTR *pRec, long *pVal);
	void UpdateControllerSoftware__(BSTR sFileFullPath, long lFlags, long *pVal);
	void InitDebit__(long lPumpGroup01, long lPumpGroup2, long lFlags, long *pVal);
	void ResetQdxInfo__(long lFileNum, long lPumpNum, long  lFlags1, long lFlags2, long* pVal);
	void SetServiceFee2__(long lFeeNumber, BSTR sName,long lFlags ,long lPakFee,  long lPrePayFee, long lPapFee, long lAviFee, long lAttendFee,long lSelfPakFee, long lSelfPrePayFee,long lSelfPapFee,  long lSelfAviFee,  long lSelfAttendFee, long *pRetVal);
	//Add by Yaron for the CtrlEvent handling
	void CloseEventsInterface_(long lClientId, long *pVal);
	void InitEventInterface_(long lFlags,long * pClientId,long *pVal);
	//End add by Yaron
	void SetServiceFee__(long lFeeNumber, BSTR sName ,long lFlags ,long lPakFee, long lPrePayFee, long lPapFee, long lAviFee, long lAttendFee, long *pRetVal);
	void GetPumpModeScheme3__(long lPumpNumber, long lScheme, long *plMode, BSTR *psPeriodDateTimeStart, BSTR *psPeriodDateTimeEnd, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plServiceFeeNo, long *plFlags, long *pVal);
	void SetPumpModeScheme3__(long lPumpNumber, long lScheme, long lMode, BSTR sPeriodDateTimeStart, BSTR sPeriodDateTimeEnd, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lServiceFeeNo, long lFlags, long *pVal);
	void GetServiceFee__(long lPump, long lTrsType,long lTrsNumber, BSTR stTime, long lFlags, long *pValue, long *pRetVal);
	void GetPumpModeScheme2__(long lPumpNumber, long lScheme, long *plMode, BSTR *psPeriodDateTimeStart, BSTR *psPeriodDateTimeEnd, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plFlags, long *pVal);
	void SetPumpModeScheme2__(long lPumpNumber, long lScheme, long lMode, BSTR sPeriodDateTimeStart, BSTR sPeriodDateTimeEnd, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lFlags, long *pVal);
	BOOL IsManualPricePolePriceChangeRequired(long nFlags); //4.0.26.508 147183

	void SetGradePrice2__(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long lPrice, BSTR sTimeValid, short nFlags, long lTaxRate, long lDiscountPerUnit, long lMaxUnitForDiscount, long *pVal);
	void GetParam3__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long *pVal,BOOL bIniFile=FALSE);  //4.0.8.503
	void SetParam3__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal ,BOOL bIniFile=FALSE); //4.0.8.503
	void SetMaintenance__(long lLock,long lParam,long *pVal);
	void GetPOSInfo__(BSTR *sPOSInfo, long *pVal);
	void ReConfigure__(long lConfigureFlags, long *pVal);
	void GetWetStockData__(long lWetStockFlags, long *pVal);
	void GetServiceStatus__(long lService, long *lStatus, long *pVal);
	void StopService__(long lService, long *pVal);
	void StartService__(long lService, long *pVal);
	void RemoveAlarm__(short nFlags, long lRecIndex, long *pVal);
	void GetAlarm__(short nFlags, long *lIndexNumber,long *plCatagory, long *plCode, long *plParameter, BSTR *psStartDateAndTime, BSTR *psEndDateAndTime, long *plActiveReadNumber, long *plShiftNumber,long *pVal);
	void SetGradePrice__(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long lPrice, BSTR sTimeValid, short nFlags, long * pVal);
	void GetGradePrice__(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long * lPrice, BSTR * sDateTimeValid, long * pVal);
	void SetShift__(short nDayOfWeek, short nShiftNumber, short nShiftFlags, BSTR sStartTime, long * pVal);
	void GetShift__(short nDayOfWeek, short nShiftNumber, short * nShiftFlags, BSTR * sStartTime, long * pVal);
	void GetPumpTotals__(short nFlags, long *lIndexNumber, long *lPump, short *nNozzle, short *nGrade, short *nTotalsFlags, BSTR *sVolume, BSTR *sValueA, BSTR *sValueB, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *pVal);
	void GetPumpTotals2__(short nFlags, long *lIndexNumber, long *lPump, short *nNozzle, short *nGrade, short *nTotalsFlags, BSTR *sVolume, BSTR *sValueA, BSTR *sValueB, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *lPumpStatus, long *lNozzleStatus, long *pVal);
	void RemovePumpTotals__(short nFlags, long lRecIndex, long * pVal);
	void GetTankReading__(short nFlags, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus, short *nReadingFlags, long *lFuelLevel, long *lWaterLevel, long *lTemperature, long *lFuelVolume, long *lWaterVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *pVal);
	void GetTankReading2__(short nFlags, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus1, short *nReadingFlags, long *lFuelLevel, long *lWaterLevel, long *lTemperature, long *lFuelVolume, long *lWaterVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *lStatus2, long *lFuelValue, long *lPricePerLitre, long *pVal);
	void RemoveTankReading__(short nFlags, long lRecIndex, long * pVal);
	void GetDeliveryReport__(short nFlags, long *lIndexNumber,long *lTank,long *lProduct,long *lStatus,short *nDeliveryFlags,long *lTag,long  *lDeliveredVolume,long *lActiveReadNumber,long *lShiftNumber,BSTR *sDeliveryDateTime,long *pVal);
	void GetDeliveryReport2__(short nFlags, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus, short *nDeliveryFlags, long *lTag, long *lDeliveredVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sStartDeliveryDateTime, BSTR *sEndDeliveryDateTime, long *lStartVolume, long *lEndVolume, long *lTemperature, long *pVal);
	void RemoveDeliveryReport__(short nFlags, long lRecIndex, long * pVal);
	void SetRealTimeClock__(BSTR sDateAndTime, long * pVal);
	void ControlStatus__(short nFlags, long * lTotalIndex, long * lTankReadIndex, long * lDeliveryIndex, long * lActiveShiftNumber, short * nStationStatus, long * pVal);
	void GetProcessStatus__(long lProcessNumber, long * lProcessStatus, long * lPumpNumberA, long * lPumpProblemA, long * lPumpNumberB, long * lPumpProblemB, long * lTankNumber, long * lTankProblem, long * lFlags, long * pVal);
	void SetParam__(BSTR sTableName, long lKey1, long lKey2, BSTR sFiledName, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal);
	void GetParam__(BSTR sTableName, long lKey1, long lKey2, BSTR sFiledName, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long *pVal);
	void SetPumpMode__(long lPumpNumber, long lMode, long * pVal);
	void GetPumpMode__(long lPumpNumber, long * lMode, long * pVal);
	void SetPumpModeScheme__(long lPumpNumber, long lScheme, long lMode, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lFlags, long *pVal);
	void GetPumpModeScheme__(long lPumpNumber, long lScheme, long *plMode, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plFlags, long *pVal);
	void SetModeDef__(long lMode, BSTR sName, long lFlags, long *pVal);
	void SetModeDef2__(long lMode, BSTR sRec, long lFlags, long lMsgNumber, long *plVal); //4.0.5.41
	void GetModeDef__(long lMode, BSTR *psName, long *plFlags, long *pVal);
	void GetModeDef2__(long lMode, BSTR *psRec, long *plFlags, long *plMsgNumber, long *plVal); //4.0.5.41
	void GetParam1__(BSTR sTableAndFieldName, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, long *pVal);
	void SetParam1__(BSTR sTableAndFieldName, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal);
	void SetParam2__(BSTR sTableFieldAndVal, long lKey1, long lKey2, short nFiledType, long lVal, BOOL bCreate, long *pVal);
	void GetParam2__(BSTR *sTableFieldAndVal, long lKey1, long lKey2, short nFiledType, long *lVal, long *pVal);
	void GetRecord__(BSTR *sTableAndVal, long lKey1, long lKey2, long *pVal);
	void SetRecord__(BSTR sTableAndVal, long lKey1, long lKey2, long *pVal);
	void TLSCommand__(BSTR& sCommand , long lCommand , long lTankNumber , long lFlags , long * lRetVal);
	void GetPaidTrs__(long lClientId, long lFlags, BSTR *pRec, long *lCountRec, long *plRetVal);
	void Reconfigure2__(long lTableId , long lSubTableId , long lSubTableIdExt , long lFlags , long * pRetVal);
	void GetPumpTotalsMemoData__(long lPumpNumber,long lFlags,BSTR * pRec,long *lCounter,long * pRetVal);
	void GetReceipt2__(long lPosNumber, BSTR sTrack2, long lIndex, BSTR *sReceiptData, long lFlags, long *pVal); //4.0.12.512
	void GetTankReadingEx__(long lOpCode, long *lIndexNumber, long lTankNumber, long lFlags, BSTR *pRec, long *pVal); //4.0.13.503
	void GetDeliveryReportEx__(long lOpCode, long *lIndexNumber, long lFlags, BSTR *pRec, long *pVal); //4.0.13.504
	void GetVersion__(BSTR *sVersionList, long lFlags, long *pVal);
	void StartQdxMaintenance_(long lFileNum, long *pVal); //33898
	void SetGradePrice2Direct__(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long lPrice, BSTR sTimeValid, short nFlags, long lTaxRate, long lDiscountPerUnit, long lMaxUnitForDiscount, long *pVal); //TD 58094
	void GetStationStatus__(long *lPumpMap1, long *lPumpMap2, long lFlags, long *pRetVal); //4.0.19.508
	void GetDeliveryEmulationStatus__(long lTankNumber, long lFlags, long *plStatus, BSTR *pRec, long *pRetVal); //4.0.21.500 TD 66935
	long HandleSingleTankReguest(const long lProcessType, const CString sProcessString, const BSTR sRec);//4.0.21.500 TD 66935
	long GetGPLForDeliverySingleTank(const long lRetCode);//4.0.21.500 TD 66935
	void GetChangeProductInTankStatus__(long lFlags, long *plStatus, long *plErrorCode, BSTR *pbstrData, long *plVal); //4.0.22.503 - TD 72189
	long HandleCancelDeliverySingleTankReguest(long lTankNumber);//4.0.23.500 TD 74964
	void GetProcessStatusEx__(long lFlags, long lProcessType, long *plStatus, long *plErrorCode, BSTR *pbstrData, long *plVal); //4.0.19.820 - TD 78775
	void GetPumpTotalsEx__(long lOpCode, long *lIndexNumber, long lPumpNumber, long lNozzle, long lFlags, BSTR *pRec, long *pVal); //4.0.25.50 - TD 108311
	void SetParam4__(BSTR sTableName, long lKey1, long lKey2, BSTR sFiledName, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long lFlags, long *pVal); //4.0.26.501 - TD 116227
	void GetRollingUpdateInfo__(long lPumpNumber, long lFlags, BSTR *pbstrData, long *pVal); //4.0.26.501- TD 108311
	void GetConfiguration__(BSTR sClientName, BSTR * sConfigurationData, long * pVal);  //4.0.25.994 - TD 325383
	void SetTankState__(long lTankNumber, long lState, long lFlags, BSTR sRec, long *pRetVal); //4.0.25.1130 
	void GetTankState__(long lTankNumber, long lFlags, BSTR *pRec, long *pRetVal); //4.0.25.1130 
	void SmartReconfigure__(long lDeviceNumberMask1, long lDeviceNumberMask2, long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags, long * pRetVal);   //CR 442586
	
public :
	STDMETHOD(GetRollingUpdateInfo)(/*[in]*/ long lPumpNumber, /*[in]*/ long lFlags, /*[out]*/ BSTR *pbstrData, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetPumpTotalsEx)(/*[in]*/ long lOpCode, /*[in,out]*/ long *lIndexNumber, /*[in]*/ long lPumpNumber, /*[in]*/ long lNozzle, /*[in]*/ long lFlags, /*[in,out]*/ BSTR *pRec, /*[out,retval]*/ long * pVal);
	STDMETHOD(SetParam4)(/*[in]*/ BSTR sTableName, /*[in]*/ long lKey1, /*[in]*/ long lKey2, /*[in]*/ BSTR sFiledName, /*[in]*/ short nFiledType, /*[in]*/ BSTR sVal, /*[in]*/ long lVal, /*[in]*/ BOOL bCreate,  /*[in]*/ long lFlags, /*[out,retval]*/ long * pVal);
	STDMETHOD(GetProcessStatusEx)(/*[in]*/ long lFlags, /*[in]*/ long lProcessType, /*[out]*/ long *plStatus, /*[out]*/ long *plErrorCode, /*[out]*/ BSTR *pbstrData, /*[out, retval]*/ long *plVal);
	STDMETHOD(GetChangeProductInTankStatus)(/*[in]*/ long lFlags, /*[out]*/ long *plStatus, /*[out]*/ long * plErrorCode, /*[out]*/ BSTR * pbstrData, /*[out,retval]*/ long *plVal);
	STDMETHOD(GetDeliveryEmulationStatus)(/*[in]*/ long lTankNumber, /*[in]*/ long lFlags, /*[out]*/ long * plStatus,/*[out]*/ BSTR *pRec, /*[out , retval]*/ long * pRetVal);
	STDMETHOD(GetStationStatus)(/*[in , out]*/ long * lPumpMap1 , /*[in , out]*/ long * lPumpMap2 , /*[in]*/ long lFlags , /*[out , retval]*/ long * pRetVal);
	STDMETHOD(StartQdxMaintenance)(/*[in]*/ long lFileNum, /*[out,retval]*/ long *pVal);
	STDMETHOD(SetupEx)(/*[in]*/ BSTR sXmlRec, /*[in]*/ long lFlags, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetVersion)(/*[out]*/ BSTR *sVersionList, /*[in]*/ long lFlags , /*[out,retval]*/ long *pVal);
	STDMETHOD(IsUnmanned)(/*[out]*/ BOOL * pbUnmanned, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetDeliveryReportEx)(/*[in]*/ long lOpCode,/*[in , out]*/ long *lIndexNumber,/*[in]*/ long lFlags,/*[out]*/ BSTR *pRec,/*[out,retval]*/ long *pVal);
	STDMETHOD(GetTankReadingEx)(/*[in]*/ long lOpCode,/*[in , out]*/ long *lIndexNumber,/*[in]*/ long lTankNumber,  /*[in]*/ long lFlags, /*[out]*/ BSTR *pRec, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetReceipt2)(long lPosNumber,BSTR sTrack2,long lIndex,BSTR *sReceiptData,long lFlags,long *pVal);
	STDMETHOD(GetPumpTotalsMemoData)(/*[in]*/ long lPumpNumber,/*[in]*/ long lFlags,/*[out]*/ BSTR * pRec,/*[out]*/  long *lCounter ,/*[in,out]*/ long * pRetVal);
	STDMETHOD(GetPaidTrs)(/*[in]*/ long lClientId,/*[in]*/ long lFlags,/*[out]*/ BSTR * pRec,/*[in,out]*/ long * lCountRec,/*[out,retval]*/ long * plRetVal);
	STDMETHOD(InitParam)(/*[in]*/ long lFlags,/*[out]*/ BSTR  * sPCIXml,/*[out,retval]*/ long *pVal);
	STDMETHOD(GetServiceFeeMode)(/*[in]*/long lPumpNumber, /*[out]*/long *lFeeSet, /*[in]*/long lFlags, /*[out, retval]*/long *pVal);
	STDMETHOD(SetServiceFeeMode)(long lPumpNumber, long lFeeSet, long lFlags, long *pVal);
	STDMETHOD(GetGeneralData)(/*[in]*/ long lRecordType,/*[out]*/ BSTR* sData, /*[in]*/ long lFlags,/*[out, retval]*/ long *pVal);
	STDMETHOD(SetModeDef2)(/*[in]*/ long lMode,  /*[in]*/ BSTR sRec,  /*[in]*/ long lFlags, /*[in]*/ long lMsgNumber, /*[out, retval]*/ long *plVal);
	STDMETHOD(GetModeDef2)(/*[in]*/ long lMode,/*[out]*/ BSTR *psRec, /*[out]*/ long *plFlags, /*[out]*/ long *plMsgNumber,/*[out, retval]*/ long *plVal);
	STDMETHOD(SetAccumulateState)(/*[in]*/ long lShiftNumber ,/*in*/long lPosNumber, /*[in]*/ long lState,/*[in]*/ long lFlags, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetAccumulateTotals)(/*[in]*/ long lShiftNumber , /*[in]*/ long lState, /*[in]*/ long lPosNumber, /*[in]*/ long lFlags, /*[in ]*/ long lVal,  /*[in]*/ BSTR sStartTime, /*[in]*/ BSTR sEndTime, /*[out]*/ long *lRecordCount,   /*[in, out]*/ BSTR* sRec, /*[out, retval]*/ long *pVal);
	STDMETHOD(GetConnectivityStatus)(/*[out]*/ BSTR* sPSConnStatus, /*[in]*/ long lFlags, /*[out,retval]*/ long *pVal);
	STDMETHOD(TankGaugeData)(/*[in]*/long lTankNumber, /*[in]*/ long lFlags, /*[in]*/ BSTR sRec,  /*[in, retval]*/long *pVal);
	STDMETHOD(DBTankGauge)(/*[in]*/ long lOffset ,/*[in,out]*/ BSTR *sRec, /*[in]*/ long lRecSize, /*[in]*/ long lFlags, /*[in, retval]*/long *pVal);
	STDMETHOD(GetTankMemoryData)(/*[in]*/ long lTankNumber,/*[out]*/ BSTR  *pRec, /*[in]*/ long lFlags, /*[in,out]*/ long *lCounter, /*[retval]*/long *pVal);
	STDMETHOD(GeneralSystemRequest)(/*[in]*/ long lVal, /*[in]*/ BSTR sRec, /*[in]*/ long lFlags , /*[out,retval]*/ long *plVal);
	STDMETHOD(Reconfigure2)(/*[in]*/ long lTableId , /*[in]*/ long lSubTableId , /*[in]*/ long lSubTableIdExt , /*[in]*/ long lFlags , /*[out , retval]*/ long * pRetVal);
	STDMETHOD(GetDeliveryInfo)(/*[in]*/ long lTankNumber , /*[out]*/ BSTR * pRec , /*[in]*/ long lFlags , /*[out , retval]*/ long * pRetVal);
	STDMETHOD(GetPumpTotalsInfo)(/*[in]*/ long lPumpNumber , /*[in]*/ long lFlags , /*[out]*/ BSTR * pRec , /*[out , retval]*/ long * pRetVal);
	STDMETHOD(GetTankInfo)(/*[in]*/ long lTankNumber , /*[out]*/ BSTR * pRec , /*[in]*/ long lFlags , /*[out , retval]*/ long * pRetVal);
	STDMETHOD(GetPumpDevices)(/*[in]*/ long lGrade , /*[in]*/ long lNacsCode , /*[in]*/ long lDevice , /*[in]*/ long lFlags , /*[out]*/ long * pVal , /*[out ,retval]*/ long * pRetVal);
	STDMETHOD(ReloadPricePolesData)(/*[in]*/ long lFlags, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetTankReading3)(/*[in]*/ short nOpCode, /*[in,out]*/ long *lIndexNumber, /*[out]*/ long *lTank, /*[out]*/ long *lProduct, /*[out]*/ long *lStatus1, /*[out]*/ short *nReadingFlags, /*[out]*/ long *lFuelLevel, /*[out]*/ long *lWaterLevel, /*[out]*/ long *lTemperature, /*[out]*/ long *lFuelVolume, /*[out]*/ long *lWaterVolume, /*[out]*/ long *lActiveReadNumber, /*[out]*/ long *lShiftNumber, /*[out]*/ BSTR *sDateTime, /*[out]*/ long *lStatus2, /*[out]*/ long *lFuelValue, /*[out]*/ long *lPricePerLitre, /*[in]*/long *lTcVolume, long *lUllage,/*[in][out]*/long *lFlags, /*[out]*/ BSTR *pRec ,/*[out,retval]*/ long *pVal);
	STDMETHOD(UpdateControllerSoftware)(/*[in]*/ BSTR sFileFullPath, /*[in]*/ long lFlags,/*[out,retval]*/ long *pVal);
	STDMETHOD(InitDebit)(/*[in]*/ long lPumpGroup01 , /*[in]*/ long lPumpGroup2 , /*[in]*/ long lFlags , /*[out,retval]*/long *pVal);
	STDMETHOD(ResetQdxInfo)(/*[in]*/ long lFileNum, /*[in]*/ long lPumpNum, /*[in]*/ long lFlags1, /*[in]*/ long lFlags2, /*[out,retval]*/ long* pVal);
	STDMETHOD(TLSCommand)(/*[in]*/ BSTR sCommand , /*[in]*/ long lCommand , /*[in]*/ long lTankNumber , /*[in]*/ long lFlags , /*[out , retval]*/ long *plRetVal);
	STDMETHOD(SmartReconfigure)(/*[in]*/ long lDeviceNumberMask1, /*[in]*/ long lDeviceNumberMask2,/*[in]*/ long lTableId, /*[in]*/ long lSubTableId, /*[in]*/ long lSubTableIdExt, /*[in]*/ long lFlags, /*[out , retval]*/ long * pRetVal); //CR 442586

BEGIN_CONNECTION_POINT_MAP(CCtrl)
	CONNECTION_POINT_ENTRY(DIID_ICtrlEvents)
END_CONNECTION_POINT_MAP()

	

};

#endif //__CTRL_H_
