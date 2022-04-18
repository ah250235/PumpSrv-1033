#include "stdafx.h"
#include "SQLCarWashFile.h"
#include "PumpSrvDatabase.h"


CSQLCarWashFile::CSQLCarWashFile()
{
}

CSQLCarWashFile::~CSQLCarWashFile()
{
}

long CSQLCarWashFile::Operation(eFileOperation OpCode, void * pInfo)
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	long lRetVal = 1;
	CAR_WASH_INFO	*carWashInfo = NULL;

	if (pInfo)
		carWashInfo = (CAR_WASH_INFO *)(pInfo);
	

	switch (OpCode)
	{
	case REC_INSERT:
		lRetVal = PumpSrvDataBase.SaveCarWashInfo(*carWashInfo, carWashInfo->lIndexNumber) ? OK : 1;
		break;
	case REC_READ:	
		lRetVal = PumpSrvDataBase.LoadCarWashInfoRecord(*carWashInfo,carWashInfo->lIndexNumber) ? OK : 1;
		break;
	case REC_UPDATE:
		lRetVal = PumpSrvDataBase.SaveCarWashInfo(*carWashInfo, carWashInfo->lIndexNumber) ? OK : 1;
		break;
	case REC_DELETE:
		lRetVal = PumpSrvDataBase.Delete(*carWashInfo) ? OK : 1;
		break;
	case REC_READ_FIRST:
		lRetVal =  PumpSrvDataBase.LoadFirstCarWashInfoRecord(*carWashInfo) ? OK : 1;
		break;
	case REC_READ_NEXT:
		lRetVal = PumpSrvDataBase.LoadNextCarWashInfoRecord(carWashInfo->lIndexNumber,*carWashInfo) ? OK : 1;
		break;
	case REC_DELETE_TILL_INDEX:
		break;
	}

	return lRetVal;
}
