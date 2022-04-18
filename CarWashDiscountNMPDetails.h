#pragma once



//4.0.23.950 216343
class CCarWashDiscountNMPDetails: public CNMPDetails
{
public:
	CCarWashDiscountNMPDetails();
	~CCarWashDiscountNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola) override;
};
