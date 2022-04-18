#pragma once


class CManualLoyaltyNMPDetails: public CNMPDetails
{
public:
	CManualLoyaltyNMPDetails();
	~CManualLoyaltyNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
};
