#pragma once
#include "LoyaltyCommandExecuter.h"
class CEmeraldLoyaltyCommandExecuter : public CRTILoyaltyCommandExecuter
{

public:
	CEmeraldLoyaltyCommandExecuter();
	virtual ~CEmeraldLoyaltyCommandExecuter();

	virtual long AddMemberCard(_bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lFlags, PAY_AT_PUMP_INFO & pInfo);
	virtual long StartTicket(_bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag);

	/* TODO: implement query and query result to loyaltyFuel.dll*/
	virtual BOOL Query(long lPumpNumber, PAY_AT_PUMP_INFO & pInfo);

	virtual long AddItem(_bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, OLA_STAT & ola, long lTrsNum);
	virtual BOOL AddMemberInfo(long lPumpNumber, long lSegmentId) { return FALSE; }
	//virtual long InitPumps(_bstr_t bstrPumpsArr, long lFlag) { return FALSE; }
	virtual long EndTicket(long lPumpNumber, long lFlag); 
	virtual long CancelTicketWithAddItem(long lPumpNumber, long lFlag) { return FALSE; }
	virtual long CancelTicket(long lPumpNumber, long lFlag);
	virtual long AddItemFuelAndNonFuel(_bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, _bstr_t bstrXMLCarWashData) { return FALSE; }
	virtual long PhoneLookUp(_bstr_t bstrXMLPhoneNumber, long lPumpNumber) { return FALSE; }
	virtual long GetTargetMessage(long lPumpNumber) { return FALSE; }
	virtual long GetMemberInfo(long lPumpNumber, _bstr_t bstrXml) { return 0; }
	long StartTicketWithAddItem(_bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag, _bstr_t bstrXMLAddItemData) { return FALSE; }
	virtual long SetParam(long lPumpNumber, long lTrsNumber, long lLanguageId, _bstr_t bstrXml) { return 0; }
	virtual long AddCarWashItem(long lPumpNumber, long lFlags, _bstr_t bstrXMLPhoneNumber) { return 0; }
	virtual long CancelTicketWithAddItem(long lPumpNumber, long lFlag, _bstr_t bstrXMLAddItemData) { return 0; }
	virtual BOOL CreateAddItemXML(CComBSTR& bstrOutXml, PumpItems& cItems, long lPumpNumber, CPumpTransact trs, long lTotalDiscount);
private:
	void GetCarWashDetalisFromCardSaleData( __int64 &lPluCode, PAY_AT_PUMP_INFO &cTmpPapInfo, long &lPrice, long &lQuantity ,CString &sName );
	BOOL AddCarWashItemToXml(PAY_AT_PUMP_INFO &cTmpPapInfo ,CComBSTR& sXMLOutData);
	void AddTicketPromotionsToXML(long lPumpNumber,CXMLInterface &xmlInitDoc, CXMLPointers & pTriggeredPromElements, long &lCount, long &lTotalDiscount); // add carwash to xml returned to Emerald
};



