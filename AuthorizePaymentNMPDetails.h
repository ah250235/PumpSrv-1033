#pragma once

#include "NmpDetalis.h"

class CAuthorizePaymentNMPDetails: public CNMPDetails
{
public:
	CAuthorizePaymentNMPDetails();
	~CAuthorizePaymentNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
};


