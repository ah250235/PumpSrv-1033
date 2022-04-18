#pragma once
class CParitalApproveNMPDetails : public CNMPDetails
{
public:
	CParitalApproveNMPDetails(void);
	virtual ~CParitalApproveNMPDetails(void) {}
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
	void GetMessageForTerminal( long lPumpNumber ,PAY_AT_PUMP_INFO & cTmpInfo);
	virtual short CreateGeneralNMPProperties() override;
};

