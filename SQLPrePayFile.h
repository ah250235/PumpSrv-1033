#pragma once
class CSQLPrePayFile
{
public:
	CSQLPrePayFile();
	~CSQLPrePayFile();
	long Operation(eFileOperation OpCode, void *pInfo, CPumpTransact *pTrs, BOOL bIsSaveByMaint = FALSE); //4.0.18.20 - CR 33898
};

