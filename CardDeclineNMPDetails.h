#pragma once
class CCardDeclineNMPDetails : public CNMPDetails
{
public:
	CCardDeclineNMPDetails(void);
	virtual ~CCardDeclineNMPDetails(void);
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
};

