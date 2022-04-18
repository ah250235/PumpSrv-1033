#pragma once
class CSQLTankReadsFile
{
public:
	CSQLTankReadsFile();
	~CSQLTankReadsFile();
	long Operation(eFileOperation OpCode, void *pInfo, BOOL bIsSaveByMaint = FALSE); 
};

