#include "stdafx.h"
#include "SQLFuelDeliveriesFile.h"
#include "PumpSrvDatabase.h"
#include "PumpSrvStructures.h"


CSQLFuelDeliveriesFile::CSQLFuelDeliveriesFile()
{
}


CSQLFuelDeliveriesFile::~CSQLFuelDeliveriesFile()
{
}

long CSQLFuelDeliveriesFile::Operation(eFileOperation OpCode, void * pInfo, BOOL bIsSaveByMaint)
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	long lRetVal = OK;
	DELIVERY_INFO	*pDeliveryInfo = NULL;
	pDeliveryInfo = (DELIVERY_INFO *)pInfo;

	switch (OpCode)
	{
	case REC_INSERT:
		lRetVal = PumpSrvDataBase.SaveFuelDelivery(pDeliveryInfo->lIndexNumber,*pDeliveryInfo) ? OK : 1;
		break;
	case REC_READ:
		lRetVal = PumpSrvDataBase.LoadFuelDelivery(pDeliveryInfo->lIndexNumber,*pDeliveryInfo) ? OK : 1;
		break;
	case REC_UPDATE:
		break;
	case REC_DELETE:
		lRetVal = PumpSrvDataBase.DeleteFuelDelivery(pDeliveryInfo->lIndexNumber) ? OK : 1;
		break;
	case REC_READ_FIRST:
		lRetVal = PumpSrvDataBase.LoadFirstFuelDelivery(*pDeliveryInfo) ? OK : 1;
		break;
	case REC_READ_NEXT:
		lRetVal = PumpSrvDataBase.LoadNextFuelDelivery(pDeliveryInfo->lIndexNumber, *pDeliveryInfo) ? OK : 1;
		break;
	case REC_DELETE_TILL_INDEX:
		break;
	}

	return lRetVal;
}
