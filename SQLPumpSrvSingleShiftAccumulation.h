#pragma once
class CSQLPumpSrvSingleShiftAccumulation
{
public:
	CSQLPumpSrvSingleShiftAccumulation();
	~CSQLPumpSrvSingleShiftAccumulation();
	long Operation(long lOpCode, void *pRec, void *pData);
};

