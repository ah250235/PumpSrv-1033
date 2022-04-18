#include "stdafx.h"
#include "AuthorizeCardNMP.h"
#include "OLA.h"


CAuthorizeCardNMP::CAuthorizeCardNMP()
{
	m_nOlaState = OLA_LOYALTY_AUTHORIZE_CARD;
}


CAuthorizeCardNMP::~CAuthorizeCardNMP(void)
{
}

void CAuthorizeCardNMP::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola )
{
	long lMediaMasterType = _Module.m_server.m_cFlowControlManager.GetMediaMasterType(lPumpNumber);

	if (lMediaMasterType == CARD_TYPE_LOYALTY)
	{
		AuthorizeLoyaltyCard(ola, lPumpNumber, cTmpInfo);
	}
	else if ((_Module.m_server.m_cCommonFunction.IsPaymentCard(lMediaMasterType) ||
		(lMediaMasterType == CARD_TYPE_INVALID)) ||
		(lMediaMasterType == CARD_TYPE_NONE))	 // RFUEL-381
	{
		AuthorizePaymentCard(ola, lPumpNumber, cTmpInfo);
	}
	else // altId
	{
		AuthorizeAltId(ola, lPumpNumber, cTmpInfo);
	}
}

void CAuthorizeCardNMP::AuthorizePaymentCard(OLA_STAT &ola, long lPumpNumber, PAY_AT_PUMP_INFO & cTmpInfo)
{
	long lPumpIndex = lPumpNumber - 1;
	
	_Module.m_server.m_cPumpArray[lPumpIndex].GetOlaStat(&ola); 
	ola.m_byState = OLA_REQUEST_TO_SEND | SESSION_AUTHORIZE;
	_Module.m_server.m_cPumpArray[lPumpIndex].SetOlaStat(&ola); 

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"AuthorizePaymentCard");
}


void CAuthorizeCardNMP::AuthorizeLoyaltyCard( OLA_STAT &ola, long lPumpNumber, PAY_AT_PUMP_INFO & cTmpInfo )
{
	long p = lPumpNumber-1;
	CString sMsg;

	if ((_Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber) == CancelBeforeStartTicket) ||
		(_Module.m_server.m_cFlowControlManager.GetPumpSrvAddMemberCardRes(lPumpNumber) > 0))//4.0.23.330 103166 115000 //4.0.23.390 116544
	{
		sMsg.Format("AuthorizeLoyaltyCard::Execute skip Authorize Loyalty card NMP, CancelTime = %ld, PumpSrvAddMemberRes = %ld", _Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber), _Module.m_server.m_cFlowControlManager.GetPumpSrvAddMemberCardRes(lPumpNumber)); //4.0.23.610 133300 
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cTmpInfo, TRUE);
	}
	else
	{
		_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 
		ola.m_byState = 	OLA_LOYALTY_ADD_MEMBER_CARD | SESSION_AUTHORIZE;	//4.0.22.504 69344

		_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP, "AuthorizeLoyaltyCard: ola.m_byState = (OLA_LOYALTY_ADD_MEMBER_CARD | SESSION_AUTHORIZE)");
	}
}

void CAuthorizeCardNMP::AuthorizeAltId(OLA_STAT & ola, long lPumpNumber, PAY_AT_PUMP_INFO & cTmpInfo)
{
	long p = lPumpNumber - 1;
	CString sMsg;

	if ((_Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber) == CancelBeforeStartTicket) ||
		(_Module.m_server.m_cFlowControlManager.GetPumpSrvAddMemberCardRes(lPumpNumber) > 0))//4.0.23.330 103166 115000 //4.0.23.390 116544
	{
		sMsg.Format("AuthorizeAltId::Execute skip Authorize Loyalty card NMP, CancelTime = %ld, PumpSrvAddMemberRes = %ld", _Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber), _Module.m_server.m_cFlowControlManager.GetPumpSrvAddMemberCardRes(lPumpNumber)); //4.0.23.610 133300 
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg);
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cTmpInfo, TRUE);
	}
	else
	{
		_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
		ola.m_byState = OLA_LOYALTY_ADD_MEMBER_CARD | SESSION_AUTHORIZE;	//4.0.22.504 69344

		_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&cTmpInfo);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "AuthorizeAltId: ola.m_byState = (OLA_LOYALTY_ADD_MEMBER_CARD | SESSION_AUTHORIZE)");
	}
}