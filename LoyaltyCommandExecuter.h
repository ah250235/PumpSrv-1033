// LoyaltyCommandExecuterHandler.h: interface for the CLoyaltyCommandExecuterHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOYALTYCOMMANDEXECUTERHANDLER_H__B2591D7C_E8E7_448F_9EC8_3E15F8F451EB__INCLUDED_)
#define AFX_LOYALTYCOMMANDEXECUTERHANDLER_H__B2591D7C_E8E7_448F_9EC8_3E15F8F451EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommandExecuter.h"

class CLoyaltyCommandExecuter 
{
public:
	CLoyaltyCommandExecuter();
	virtual ~CLoyaltyCommandExecuter();
	virtual long Restore_COM( HRESULT hResFailure );

	virtual BOOL HandleComException(long lPumpNumber , HRESULT hRes, char *szMsgFuncName);
	virtual void ReleaseLoyaltyInterface() = 0; 
	virtual void CreateEventHandler() = 0;
	virtual void DestuctLoyaltyPtr() {}
	
	// COM methods
	virtual BOOL Query(long lPumpNumber,PAY_AT_PUMP_INFO & pInfo) = 0; 
	virtual BOOL AddMemberInfo(long lPumpNumber,long lSegmentId) = 0;
	virtual long InitPumps ( _bstr_t bstrPumpsArr, long lFlag ) = 0; ;
	virtual BOOL Init() = 0;
	virtual long StartTicketWithAddItem ( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag, _bstr_t bstrXMLAddItemData ) = 0;
	virtual long StartTicket ( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag ) = 0;
	virtual long AddMemberCard ( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lFlags, PAY_AT_PUMP_INFO & pInfo) = 0;
	virtual long EndTicket ( long lPumpNumber, long lFlag ) = 0;
	virtual long CancelTicketWithAddItem ( long lPumpNumber, long lFlag) = 0 ;
	virtual long CancelTicket ( long lPumpNumber, long lFlag ) = 0 ;
	virtual long AddItemFuelAndNonFuel ( _bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, _bstr_t bstrXMLCarWashData ) = 0 ;
	virtual long AddItem(_bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, OLA_STAT & ola, long lTrsNum) = 0;
	virtual long PhoneLookUp ( _bstr_t bstrXMLPhoneNumber, long lPumpNumber ) = 0;
	virtual long AddCarWashItem(long lPumpNumber,long lFlags,_bstr_t bstrXMLPhoneNumber) = 0;
	virtual long SetParam ( long lPumpNumber,long lTrsNumber,long lLanguageId, _bstr_t bstrXml) = 0;
	virtual long GetTargetMessage(long lPumpNumber) = 0;
	virtual long GetMemberInfo(long lPumpNumber,_bstr_t bstrXml) = 0; // CR 405435
	virtual BOOL CreateAddItemXML(CComBSTR& bstrOutXml, PumpItems& cItems, long lPumpNumber, CPumpTransact trs, long lTotalDiscount) { return FALSE; }
};


class CGLILoyaltyCommandExecuter : public CLoyaltyCommandExecuter // exe interface
{
public:
	CGLILoyaltyCommandExecuter() {};
	virtual ~CGLILoyaltyCommandExecuter() {};
	
	virtual void ReleaseLoyaltyInterface() override;	
	virtual void CreateEventHandler() override;
	virtual void DestuctLoyaltyPtr() override;
	
	
	// COM methods
	virtual BOOL Query(long lPumpNumber,PAY_AT_PUMP_INFO & pInfo) override;
	virtual BOOL AddMemberInfo(long lPumpNumber,long lSegmentId) override;
	virtual long InitPumps ( _bstr_t bstrPumpsArr, long lFlag ) override;
	virtual BOOL Init() override;
	virtual long StartTicketWithAddItem( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag, _bstr_t bstrXMLAddItemData) override;
	virtual long StartTicket( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag) override;
	virtual long AddMemberCard(_bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lFlags, PAY_AT_PUMP_INFO & pInfo) override;
	virtual long EndTicket(long lPumpNumber, long lFlag) override;
	virtual long CancelTicketWithAddItem(long lPumpNumber, long lFlag) override;
	virtual long CancelTicket(long lPumpNumber, long lFlag) override;
	virtual long AddItemFuelAndNonFuel( _bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, _bstr_t bstrXMLCarWashData) override;
	virtual long AddItem(_bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, OLA_STAT &ola, long lTrsNum = 0) override;
	virtual long PhoneLookUp( _bstr_t bstrXMLPhoneNumber, long lPumpNumber) override;
	virtual long AddCarWashItem(long lPumpNumber,long lFlags,_bstr_t bstrXMLCarWashItem) override;
	virtual long SetParam(long lPumpNumber,long lTrsNumber,long lLanguageId, _bstr_t bstrXml) override;
	virtual long GetTargetMessage(long lPumpNumber) override;
	virtual long GetMemberInfo(long lPumpNumber,_bstr_t bstrXml) override; // CR 405435
protected:// // 
	IGLIPtr	m_pGLI;
};

class CRTILoyaltyCommandExecuter : public CLoyaltyCommandExecuter // dll interface
{
public:
	CRTILoyaltyCommandExecuter(){};
	virtual ~CRTILoyaltyCommandExecuter(){};
	
	virtual void ReleaseLoyaltyInterface() override;
	virtual void CreateEventHandler() override;
	virtual void DestuctLoyaltyPtr() override;
	
	// COM methods
	virtual BOOL Query(long lPumpNumber,PAY_AT_PUMP_INFO & pInfo) override;
	virtual BOOL AddMemberInfo(long lPumpNumber,long lSegmentId) override;
	virtual long InitPumps ( _bstr_t bstrPumpsArr, long lFlag ) override;
	virtual BOOL Init() override;
	virtual long StartTicketWithAddItem(_bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag, _bstr_t bstrXMLAddItemData) override;
	virtual long StartTicket(_bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag) override;
	virtual long AddMemberCard( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lFlags, PAY_AT_PUMP_INFO & pInfo) override;
	virtual long EndTicket(long lPumpNumber, long lFlag) override;
	virtual long CancelTicketWithAddItem(long lPumpNumber, long lFlag) override;
	virtual long CancelTicket(long lPumpNumber, long lFlag ) override;
	virtual long AddItemFuelAndNonFuel( _bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, _bstr_t bstrXMLCarWashData) override;
	virtual long AddItem(_bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag,  OLA_STAT & ola, long lTrsNum =0) override;
	virtual long PhoneLookUp( _bstr_t bstrXMLPhoneNumber, long lPumpNumber) override;
	virtual long AddCarWashItem(long lPumpNumber,long lFlags,_bstr_t bstrXMLCarWashItem) override;
	virtual long SetParam(long lPumpNumber,long lTrsNumber,long lLanguageId, _bstr_t bstrXml) override;
	virtual long GetTargetMessage(long lPumpNumber) override;
	virtual long GetMemberInfo(long lPumpNumber, _bstr_t bstrXml) override; // CR 405435

protected:// // 
	IRTIPtr	m_pRTI;
};

class CRTIStandAloneLoyaltyCommandExecuter : public CRTILoyaltyCommandExecuter // dll interface
{
public:
	CRTIStandAloneLoyaltyCommandExecuter() {};
	virtual ~CRTIStandAloneLoyaltyCommandExecuter() {};

	virtual long CancelTicket(long lPumpNumber, long lFlag) override;
	virtual long CancelTicketWithAddItem(long lPumpNumber, long lFlag) override;
};

#endif // !defined(AFX_LOYALTYCOMMANDEXECUTERHANDLER_H__B2591D7C_E8E7_448F_9EC8_3E15F8F451EB__INCLUDED_)
