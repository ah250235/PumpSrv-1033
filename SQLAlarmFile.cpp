#include "stdafx.h"
#include "SQLAlarmFile.h"
#include "PumpSrvDatabase.h"

CSQLAlarmFile::CSQLAlarmFile()
{
}


CSQLAlarmFile::~CSQLAlarmFile()
{
}

long CSQLAlarmFile::Operation(eFileOperation OpCode, void * pInfo, BOOL bIsSaveByMaint) //4.0.18.20
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
	long				lRetVal = OK;
	ALARM_INFO			TempInfo;
	ALARM_INFO		&	AlarmInfo = *static_cast<ALARM_INFO *>(pInfo);
	

	switch (OpCode)
	{
	case REC_INSERT:
		lRetVal = PumpSrvDatabase.SaveAlarmSrv(AlarmInfo) ? OK : 1;
		break;

	case REC_READ:
		lRetVal = PumpSrvDatabase.LoadAlarmSrv(AlarmInfo.lIndexNumber, TempInfo) ? OK : 1;
		if (lRetVal)
		{
			AlarmInfo = TempInfo;
		}
		break;

	case REC_UPDATE:
		lRetVal = PumpSrvDatabase.SaveAlarmSrv(AlarmInfo) ? OK : 1;
		break;

	case REC_DELETE:
		lRetVal = PumpSrvDatabase.Delete(AlarmInfo) ? OK : 1;
		break;

	case REC_READ_FIRST:
		lRetVal = PumpSrvDatabase.LoadFirstAlarmSrv(TempInfo) ? OK : 1;
		if (lRetVal == OK)
		{
			AlarmInfo = TempInfo;
		}
		break;

	case REC_READ_NEXT:
		lRetVal = PumpSrvDatabase.LoadNextAlarmSrv(AlarmInfo.lIndexNumber, TempInfo) ? OK : 1;
		if (lRetVal)
		{
			AlarmInfo = TempInfo;
		}
		break;

	case REC_DELETE_TILL_INDEX:
		break;
	}

	return lRetVal;
}
