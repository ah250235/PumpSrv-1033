#pragma once
class CSQLAsh2000
{
public:
	CSQLAsh2000();
	~CSQLAsh2000();
	long Operation(eFileOperation OpCode, void *pInfo);
	long InsertData(long lPumpNumber, void *pData, long lSize, CPumpTransact *pTrs);
};

