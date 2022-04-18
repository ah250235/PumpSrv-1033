#pragma once



//4.0.23.20 76383
class CMethodOfPaymentNMPDetails: public CNMPDetails
{
public:
	CMethodOfPaymentNMPDetails();
	~CMethodOfPaymentNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
};