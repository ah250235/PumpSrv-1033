#pragma once
#include "NmpDetalis.h"

class CCarWashSaleNMPDetails : public CNMPDetails
{
public:
	CCarWashSaleNMPDetails(void);
	virtual ~CCarWashSaleNMPDetails(void);
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
};

