#pragma once
class CSQLTankGauge
{
public:
	CSQLTankGauge();
	~CSQLTankGauge();
	long Operation(long lOpCode, void *pRec, void *pData);
};

