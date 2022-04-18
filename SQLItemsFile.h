#pragma once
class CSQLItemsFile
{
public:
	CSQLItemsFile();
	~CSQLItemsFile();
	long Operation(eFileOperation OpCode, void *pInfo);
};

