
#pragma once

// CR 405435
class CGetMemberInfoNmpDetalis : public CNMPDetails
{
public:
	CGetMemberInfoNmpDetalis();
	virtual ~CGetMemberInfoNmpDetalis();
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
};

