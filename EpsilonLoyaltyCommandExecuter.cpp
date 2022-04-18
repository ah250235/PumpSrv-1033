// CopientCommandExecuter.cpp: implementation of the CCopientCommandExecuterHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EpsilonLoyaltyCommandExecuter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEpsilonLoyaltyCommandExecuter::~CEpsilonLoyaltyCommandExecuter()
{

}


BOOL CEpsilonLoyaltyCommandExecuter::AddMemberInfo(long lPumpNumber,long lSegmentId)
{
	return FALSE;
}


BOOL CEpsilonLoyaltyCommandExecuter::Init()
{
	BOOL bRetCode = TRUE;
	return bRetCode;
}

void CEpsilonLoyaltyCommandExecuter::CreateEventHandler()
{
		
}

BOOL CEpsilonLoyaltyCommandExecuter::Query(long lPumpNumber,PAY_AT_PUMP_INFO & pInfo)	//4.0.22.504
{
	return FALSE;
}


long CEpsilonLoyaltyCommandExecuter::StartTicketWithAddItem( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag, _bstr_t bstrXMLAddItemData )
{
	_Module.m_server.m_cFuelLoayaltyPS.ClearTimer(lPumpNumber-1); // RFUEL-1087
	return FALSE;
}

long CEpsilonLoyaltyCommandExecuter::StartTicket( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag )
{
	return FALSE;
}

long CEpsilonLoyaltyCommandExecuter::InitPumps( _bstr_t bstrPumpsArr, long lFlag )
{
	return FALSE;
}

long CEpsilonLoyaltyCommandExecuter::AddMemberCard( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lFlags,PAY_AT_PUMP_INFO & pInfo )
{
	OLA_STAT  cOla;
	long p = lPumpNumber-1;
	
	_Module.m_server.m_cPumpArray[p].GetOlaStat(&cOla); 
	cOla.m_byState = 	OLA_REQUEST_TO_SEND | SESSION_GET_CUSTOMER_INFO;
	
	_Module.m_server.m_cPumpArray[p].SetOlaStat(&cOla); 

	return 1;
}

long CEpsilonLoyaltyCommandExecuter::EndTicket( long lPumpNumber, long lFlag )
{
	return FALSE;
}

long CEpsilonLoyaltyCommandExecuter::CancelTicketWithAddItem( long lPumpNumber, long lFlag)
{
	return FALSE;
}

long CEpsilonLoyaltyCommandExecuter::CancelTicket( long lPumpNumber, long lFlag )
{	
	return FALSE;
}

long CEpsilonLoyaltyCommandExecuter::AddItemFuelAndNonFuel( _bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, _bstr_t bstrXMLCarWashData )
{
	return FALSE;
}

long CEpsilonLoyaltyCommandExecuter::AddItem(_bstr_t bstrXMLStuctData,long lPumpNumber, long lFlag, OLA_STAT & ola, long lTrsNum)
{
	long p = lPumpNumber - 1;
	PAY_AT_PUMP_INFO cInfo;
	memset(&cInfo,' ' ,sizeof(PAY_AT_PUMP_INFO));
	_Module.m_server.m_cPumpArray[p].GetPAPInfo(&cInfo);


	if ((!ChrAll((char*)cInfo.CardSaleInfo.extraData.sLoyalId, sizeof(cInfo.CardSaleInfo.extraData.sLoyalId),' ')) ||
		((_Module.m_server.m_dwLoyaltySendAllTrsToLoyalty) &&  (_Module.m_server.m_dwLoyaltyClubNumber  == CLUB_TYPE_PUNCHH)))  

	{
		_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 
		ola.m_byState = 	OLA_REQUEST_TO_SEND | SESSION_SET_CUSTOMER_INFO;		
	}
	else
		ola.m_byState = SESSION_COMPLETE | OLA_REQUEST_TO_SEND;
	 
	return TRUE;
}

long CEpsilonLoyaltyCommandExecuter::AddCarWashItem(long lPumpNumber, long lFlags, _bstr_t bstrXMLPhoneNumber)
{
	throw std::logic_error("The method or operation is not implemented.");
}

long CEpsilonLoyaltyCommandExecuter::SetParam(long lPumpNumber, long lTrsNumber, long lLanguageId, _bstr_t bstrXml)
{
	throw std::logic_error("The method or operation is not implemented.");
}

long CEpsilonLoyaltyCommandExecuter::PhoneLookUp( _bstr_t bstrXMLPhoneNumber, long lPumpNumber )
{
	return FALSE;
}

void CEpsilonLoyaltyCommandExecuter::DestuctLoyaltyPtr()
{
	
}

long CEpsilonLoyaltyCommandExecuter::GetTargetMessage( long lPumpNumber )
{
	return FALSE;
}

long CEpsilonLoyaltyCommandExecuter::GetMemberInfo(long lPumpNumber, _bstr_t bstrXml)
{	
	return 0;
}

long CEpsilonLoyaltyCommandExecuter::Restore_COM(HRESULT hResFailure)
{
	throw std::logic_error("The method or operation is not implemented.");
}

BOOL CEpsilonLoyaltyCommandExecuter::HandleComException(long lPumpNumber, HRESULT hRes, char *szMsgFuncName)
{
	throw std::logic_error("The method or operation is not implemented.");
}

