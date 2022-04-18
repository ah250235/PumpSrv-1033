#pragma once
class CSQLOLATempTrs
{
public:
	CSQLOLATempTrs();
	~CSQLOLATempTrs();
	long Operation(eFileOperation OpCode, CPumpTransact * pTrs, void * pInfo);
};

