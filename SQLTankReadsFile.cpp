#include "stdafx.h"
#include "SQLTankReadsFile.h"
#include "PumpSrvDatabase.h"


CSQLTankReadsFile::CSQLTankReadsFile()
{
}


CSQLTankReadsFile::~CSQLTankReadsFile()
{
}

long CSQLTankReadsFile::Operation(eFileOperation OpCode, void * pInfo, BOOL bIsSaveByMaint)
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	long lRetVal = OK;
	TankRead		*pTotalInfo = NULL;
	pTotalInfo = (TankRead*)pInfo;
	
	switch (OpCode)
	{
	case REC_INSERT:
		lRetVal = PumpSrvDataBase.SaveTankReading(pTotalInfo->lIndexNumber, *pTotalInfo) ? OK : 1;
		break;
	case REC_READ:
		lRetVal =  PumpSrvDataBase.LoadTankReading(pTotalInfo->lIndexNumber, *pTotalInfo) ? OK : 1;
		break;
	case REC_UPDATE:
		lRetVal = PumpSrvDataBase.SaveTankReading(pTotalInfo->lIndexNumber, *pTotalInfo) ? OK : 1;
		break;
	case REC_DELETE:
		lRetVal =  PumpSrvDataBase.DeleteTankReading(pTotalInfo->lIndexNumber) ? OK : 1;
		break;
	case REC_READ_FIRST:
		lRetVal =  PumpSrvDataBase.LoadFirstTankReading(*pTotalInfo) ? OK : 1;
		break;
	case REC_READ_NEXT:
		lRetVal = PumpSrvDataBase.LoadNextTankReading(pTotalInfo->lIndexNumber, *pTotalInfo) ? OK : 1;
		break;
	case REC_DELETE_TILL_INDEX:
		break;
	}
	
	return lRetVal;
}
