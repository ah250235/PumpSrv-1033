#pragma once


// UDF - Epsilon
class CAlternateIDNMPDetails : public CNMPDetails
{
public:
	CAlternateIDNMPDetails();
	~CAlternateIDNMPDetails() {};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
};


class CAlternateIDRetryNMPDetails : public CNMPDetails
{
public:
	CAlternateIDRetryNMPDetails();
	~CAlternateIDRetryNMPDetails() {};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
};

