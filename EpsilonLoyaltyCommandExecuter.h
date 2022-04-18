#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "LoyaltyCommandExecuter.h"

class CEpsilonLoyaltyCommandExecuter : public CLoyaltyCommandExecuter // dll interface
{
public:
	CEpsilonLoyaltyCommandExecuter(){};
	virtual ~CEpsilonLoyaltyCommandExecuter();
	
	virtual void ReleaseLoyaltyInterface() override {};
	virtual void CreateEventHandler() override;
	virtual void DestuctLoyaltyPtr() override;

	// COM methods
	virtual BOOL Query(long lPumpNumber, PAY_AT_PUMP_INFO & pInfo) override;
	virtual BOOL AddMemberInfo(long lPumpNumber,long lSegmentId) override;
	virtual long InitPumps(_bstr_t bstrPumpsArr, long lFlag) override;
	virtual BOOL Init() override;
	virtual long StartTicketWithAddItem(_bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag, _bstr_t bstrXMLAddItemData) override;
	virtual long StartTicket( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag) override;
	virtual long AddMemberCard( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lFlags, PAY_AT_PUMP_INFO & pInfo) override;
	virtual long EndTicket(long lPumpNumber, long lFlag) override;
	virtual long CancelTicketWithAddItem(long lPumpNumber, long lFlag) override;
	virtual long CancelTicket(long lPumpNumber, long lFlag) override;
	virtual long AddItemFuelAndNonFuel( _bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, _bstr_t bstrXMLCarWashData) override;
	virtual long AddItem(_bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, OLA_STAT &ola, long lTrsNum = 0);
	virtual long PhoneLookUp(_bstr_t bstrXMLPhoneNumber, long lPumpNumber) override;
	virtual long GetTargetMessage( long lPumpNumber);
	virtual long GetMemberInfo(long lPumpNumber,_bstr_t bstrXml) override;
	virtual long AddCarWashItem(long lPumpNumber, long lFlags, _bstr_t bstrXMLPhoneNumber) override;
	virtual long SetParam(long lPumpNumber, long lTrsNumber, long lLanguageId, _bstr_t bstrXml) override;

	virtual long Restore_COM(HRESULT hResFailure) override;
	virtual BOOL HandleComException(long lPumpNumber, HRESULT hRes, char *szMsgFuncName) override;


protected:// // 
	//IRTIPtr	m_pRTI;*/
};

