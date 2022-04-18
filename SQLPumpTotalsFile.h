#pragma once
class CSQLPumpTotalsFile
{
public:
	CSQLPumpTotalsFile();
	~CSQLPumpTotalsFile();
	long Operation(eFileOperation OpCode, void *pInfo, BOOL bIsSaveByMaint = FALSE);
	BOOL TestLoatFirst();
	BOOL TestDelete(long lIndex);
};

