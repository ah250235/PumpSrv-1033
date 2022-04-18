#pragma once
class CSQLOLATrs
{
public:
	CSQLOLATrs();
	~CSQLOLATrs();
	long OperationEx(eFileOperation OpCode, CPumpTransact * pTrs, void * pInfo);

};

