#pragma once
class CSQLFuelDeliveriesFile
{
public:
	CSQLFuelDeliveriesFile();
	virtual ~CSQLFuelDeliveriesFile();
	long Operation(eFileOperation OpCode, void *pInfo, BOOL bIsSaveByMaint = FALSE);
};

