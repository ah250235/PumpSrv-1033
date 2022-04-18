#pragma once

#include "NmpDetalis.h"


class CAuthorizeLoyaltyNMPDetails: public CNMPDetails
{
public:
	CAuthorizeLoyaltyNMPDetails();
	~CAuthorizeLoyaltyNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola) override;
};

