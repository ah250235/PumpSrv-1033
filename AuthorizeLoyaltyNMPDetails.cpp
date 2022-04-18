#include "stdafx.h"
#include "AuthorizeLoyaltyNMPDetails.h"


CAuthorizeLoyaltyNMPDetails::CAuthorizeLoyaltyNMPDetails()
{
	m_eNMPType = NMP_TYPE_AuthorizeLoyalty;
	SetStateType(OLA_LOYALTY_AUTHORIZE_LOYALTY);	

};

void CAuthorizeLoyaltyNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	long p = lPumpNumber-1;
	CString sMsg;

	if ((_Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber) == CancelBeforeStartTicket) ||
		(_Module.m_server.m_cFlowControlManager.GetPumpSrvAddMemberCardRes(lPumpNumber) > 0))//4.0.23.330 103166 115000 //4.0.23.390 116544
	{
		sMsg.Format("CAuthorizeLoyaltyNMPDetails::Execute skip Authorize Loyalty card NMP, CancelTime = %ld, PumpSrvAddMemberRes = %ld", _Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber), _Module.m_server.m_cFlowControlManager.GetPumpSrvAddMemberCardRes(lPumpNumber)); //4.0.23.610 133300 
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cTmpInfo, TRUE);
	}
	else
	{
		_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 
		ola.m_byState = 	OLA_LOYALTY_ADD_MEMBER_CARD | SESSION_AUTHORIZE;	//4.0.22.504 69344

		_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"CAuthorizeLoyaltyNMPDetails::Execute Requested Authorize loyalty card NMP");
	}
}
