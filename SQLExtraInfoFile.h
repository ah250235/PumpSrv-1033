#pragma once
class CSQLExtraInfoFile
{
public:
	CSQLExtraInfoFile();
	~CSQLExtraInfoFile();
	long Operation(eFileOperation OpCode, void * pInfo, CString &sExtraInfoXml); 
	long InsertPrePayRecord(CPumpTransact *pTrs, void *pData);
	long InsertPayAtKioskRecord(CPumpTransact *pTrs, void *pData);
	long DeleteRecord(CPumpTransact *pTrs);
};

