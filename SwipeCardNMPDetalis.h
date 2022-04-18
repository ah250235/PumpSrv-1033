#pragma once


class CSwipeCardNMP : public CNMPDetails
{
public:
	CSwipeCardNMP(void);
	virtual ~CSwipeCardNMP(void) {}
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
	virtual short CreateGeneralNMPProperties() override;
};

