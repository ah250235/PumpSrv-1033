#include "stdafx.h"
#include "SQLPumpSrvSingleShiftAccumulation.h"
#include "PumpSrvDatabase.h"

CSQLPumpSrvSingleShiftAccumulation::CSQLPumpSrvSingleShiftAccumulation()
{
}


CSQLPumpSrvSingleShiftAccumulation::~CSQLPumpSrvSingleShiftAccumulation()
{
}

long CSQLPumpSrvSingleShiftAccumulation::Operation(long lOpCode, void *pRec, void *pData)
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	long lRetVal = 1;
	SINGLE_SHIFT_ACCUMULATION_REC	*singleShiftAccumulationInfo = NULL;

	if (pData  && pRec)
	{
		singleShiftAccumulationInfo = (SINGLE_SHIFT_ACCUMULATION_REC *)pData;
	}
	else
		lRetVal = 999;


	switch (lOpCode)
	{
	case REC_READ:
		lRetVal = PumpSrvDataBase.LoadSingleShiftRecord(*singleShiftAccumulationInfo, singleShiftAccumulationInfo->lIndexNumber) ? OK : 1;
		break;
	case REC_UPDATE:
		lRetVal = PumpSrvDataBase.SaveSingleShiftRecord(*singleShiftAccumulationInfo, singleShiftAccumulationInfo->lIndexNumber) ? OK : 1;
		break;
	
	}

	return lRetVal;
}
