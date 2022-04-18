#pragma once

class CSQLAlarmFile
{
public:
	CSQLAlarmFile();
	virtual ~CSQLAlarmFile();

public:
	long Operation(eFileOperation OpCode, void *pInfo, BOOL bIsSaveByMaint = FALSE); //4.0.18.20 - CR 33898

public:
	static CSQLAlarmFile & GetInstance(void);
};

inline CSQLAlarmFile & CSQLAlarmFile::GetInstance()
{
	static CSQLAlarmFile SQLAlarmFile;

	return SQLAlarmFile;
}
