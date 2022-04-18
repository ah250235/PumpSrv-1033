#pragma once


class CRunQueryNMPDetails: public CNMPDetails
{
public:
	CRunQueryNMPDetails();
	~CRunQueryNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola);
};
