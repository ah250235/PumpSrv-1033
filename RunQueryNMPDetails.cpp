#include "stdafx.h"
#include "RunQueryNMPDetails.h"


//4.0.23.860 212541 start
CRunQueryNMPDetails::CRunQueryNMPDetails() 
{
	m_eNMPType = NMP_TYPE_RunQuery;

	SetStateType(OLA_LOYALTY_RUN_QUERY);
}

void CRunQueryNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	long p = lPumpNumber - 1;
	CString sMsg;

	if ((_Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber) == CancelBeforeStartTicket) ||
		(_Module.m_server.m_cFlowControlManager.GetPumpSrvAddMemberCardRes(lPumpNumber) > 0)) //4.0.31.560  TD 457141
	{
		sMsg.Format("CRunQueryNMPDetails::Execute skip query, CancelTime = %ld, PumpSrvAddMemberRes = %ld", _Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber), _Module.m_server.m_cFlowControlManager.GetPumpSrvAddMemberCardRes(lPumpNumber)); //4.0.23.610 133300 
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg);
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cTmpInfo, TRUE);
	}
	else
	{
		_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
		ola.m_byState = OLA_LOYALTY_RUN_QUERY | SESSION_AUTHORIZE;
		memset(cTmpInfo.CardSaleInfo.cardData.sResultMsg, ' ', sizeof(cTmpInfo.CardSaleInfo.cardData.sResultMsg));

		_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&cTmpInfo);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "RunQuery");
	}
}
//4.0.23.860 212541 end
