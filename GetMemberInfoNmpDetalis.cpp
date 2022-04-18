#include "stdafx.h"
#include "GetMemberInfoNmpDetalis.h"

// CR 405435
CGetMemberInfoNmpDetalis::CGetMemberInfoNmpDetalis()
{
	SetStateType(OLA_LOYALTY_WAIT_GET_MEMBER_INFO_RESPONSE);
}


CGetMemberInfoNmpDetalis::~CGetMemberInfoNmpDetalis()
{
}

void CGetMemberInfoNmpDetalis::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	_Module.m_server.GetMemberInfo(lPumpNumber,& ola, lPumpNumber-1);
}
