#include "stdafx.h"
#include "ParitalApproveNMPDetalis.h"


CParitalApproveNMPDetails::CParitalApproveNMPDetails(void)
{
	m_nOlaState = OLA_LOYALTY_DISPLAY_PARITAL_APPROVAL_SCREEN;	
}

void CParitalApproveNMPDetails::Execute( PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola )
{
	long p = lPumpNumber-1;

	_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 

	GetMessageForTerminal(lPumpNumber,cTmpInfo);

	m_lScreenNumber = SCREEN_PAP_PARTIAL_PAYMENT;	
	OpenNeedMoreParam(cTmpInfo ,lPumpNumber,m_sMessage,1,NMP_DEFAULT_TIMEOUT,CreateGeneralNMPProperties());

	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "CParitalApproveNMPDetalis::Execute");
}

void CParitalApproveNMPDetails::GetMessageForTerminal( long lPumpNumber ,PAY_AT_PUMP_INFO & cTmpInfo)
{
	DOUBLE lRequestedAuthorizedLimit = _Module.m_server.GetRequstedAuthorizeAmount(lPumpNumber);

	DOUBLE lTenderAuthroizedLimit =   _Module.m_server.GetFirstTenderAuthorizedAmountLimit(lPumpNumber);

	DOUBLE lRemaningBalance =  _Module.m_server.GetRemainingBalance(lPumpNumber); 

	CString sMessage;
	sMessage.Format("Request for: $%3.2f|Authorize for: $%3.2f|Remaining Balance: $%3.2f|Insert Payment Card|Continue with Authorized Amount",lRequestedAuthorizedLimit/100,lTenderAuthroizedLimit/100,lRemaningBalance/100);

	SetMessage(sMessage.GetBuffer(0));
}

short CParitalApproveNMPDetails::CreateGeneralNMPProperties()
{
	short iBitMap = 0;

	iBitMap |= ONMP_OpenCardReader;

	iBitMap |= ONMP_OpenYesNoKeys;

	iBitMap |= ONMP_OpenContaless; // TD 460484 [7/28/2016 MaximB]

	return iBitMap;
}
