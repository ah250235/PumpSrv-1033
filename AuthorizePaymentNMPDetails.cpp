#include "stdafx.h"
#include "AuthorizePaymentNMPDetails.h"


CAuthorizePaymentNMPDetails::CAuthorizePaymentNMPDetails()
{
	m_eNMPType = NMP_TYPE_AuthorizePayment;
	SetStateType(OLA_LOYALTY_AUTHORIZE_PAYMENT);	
}

void CAuthorizePaymentNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	long p = lPumpNumber-1;

	_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 
	ola.m_byState = 	OLA_REQUEST_TO_SEND | SESSION_AUTHORIZE;

	_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Execute Requested Authorize payment card NMP");
}
