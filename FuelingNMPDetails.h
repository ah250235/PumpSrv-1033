#pragma once


class CFuelingNMPDetails: public CNMPDetails
{
public:
	CFuelingNMPDetails();
	~CFuelingNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;

};