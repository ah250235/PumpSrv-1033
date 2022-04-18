#pragma once

#include "NmpDetalis.h"

// RFUEL-2817
class ApplyCalcDiscountNMPDetail : public CNMPDetails
{
public:
	ApplyCalcDiscountNMPDetail();
	~ApplyCalcDiscountNMPDetail() {};

	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT& ola) override;
};

