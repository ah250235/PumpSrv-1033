#include "stdafx.h"
#include "SQLPumpTotalsFile.h"
#include "PumpSrvDatabase.h"
#include "PumpSrvStructures.h"

CSQLPumpTotalsFile::CSQLPumpTotalsFile()
{
}


CSQLPumpTotalsFile::~CSQLPumpTotalsFile()
{
}

long CSQLPumpTotalsFile::Operation(eFileOperation OpCode, void * pInfo, BOOL bIsSaveByMaint)
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	long lRetValue = OK;
	PumpTotals		*pTotalInfo = NULL;
	pTotalInfo = (PumpTotals *)pInfo;

	switch (OpCode)
	{
		case REC_INSERT:
			lRetValue = PumpSrvDataBase.SavePumpTotals(pTotalInfo->lIndexNumber, *pTotalInfo) ? OK : 1;
			break;
		case REC_READ:
			lRetValue = PumpSrvDataBase.LoadPumpTotals(pTotalInfo->lIndexNumber, *pTotalInfo) ? OK : 1;
			break;
		case REC_UPDATE:
			break;
		case REC_DELETE:
			lRetValue = PumpSrvDataBase.DeletePumpTotals(pTotalInfo->lIndexNumber) ? OK : 1;
			break;
		case REC_READ_FIRST:
			lRetValue = PumpSrvDataBase.LoadFirstPumpTotals(*pTotalInfo) ? OK : 1;
			break;
		case REC_READ_NEXT:
			lRetValue = PumpSrvDataBase.LoadNextPumpTotals(pTotalInfo->lIndexNumber, *pTotalInfo) ? OK : 1;
			break;
		case REC_DELETE_TILL_INDEX:
			break;
	}	

	return lRetValue;
}

BOOL CSQLPumpTotalsFile::TestLoatFirst()
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();

	PumpTotals	pTotalInfo;

	return PumpSrvDataBase.LoadFirstPumpTotals(pTotalInfo);
}

BOOL CSQLPumpTotalsFile::TestDelete(long lIndex)
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();

	bool bResult = PumpSrvDataBase.DeletePumpTotals(lIndex);

	return bResult;
}
