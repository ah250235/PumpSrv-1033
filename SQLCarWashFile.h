#pragma once

class CSQLCarWashFile
{
public:
	CSQLCarWashFile();
	~CSQLCarWashFile();
	long Operation(eFileOperation OpCode, void *pInfo);
};


