#pragma once


//4.0.22.500 start
class CSwipePaymentNMPDetails: public CNMPDetails
{
public:
	CSwipePaymentNMPDetails();
	~CSwipePaymentNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;

protected:
	virtual short CreateGeneralNMPProperties(); // TD 339257
};
