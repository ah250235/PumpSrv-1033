#include "stdafx.h"
#include "SQLPumpSrvReceiptExtraInfoDB.h"
#include "PumpSrvDatabase.h"
#include "ReceiptInfo.h"

CSQLPumpSrvReceiptExtraInfoDB::CSQLPumpSrvReceiptExtraInfoDB()
{
}

CSQLPumpSrvReceiptExtraInfoDB::~CSQLPumpSrvReceiptExtraInfoDB()
{
}

long CSQLPumpSrvReceiptExtraInfoDB::GetFirstReceiptExtraInfoRecord(PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.GetFirstReceiptExtraInfoRecord(ReceiptInfo) ? OK : 1;
}

long CSQLPumpSrvReceiptExtraInfoDB::GetNextReceiptExtraInfoRecord(int TrsIndex, PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.GetNextReceiptExtraInfoRecord(TrsIndex, ReceiptInfo) ? OK : 1;
}

long CSQLPumpSrvReceiptExtraInfoDB::InsertReceiptExtraInfo(PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo, const CPumpTransact & Trs, long *Index)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.InsertReceiptExtraInfo(ReceiptInfo) ? OK : 1;
}

long CSQLPumpSrvReceiptExtraInfoDB::InsertReceiptExtraInfo(PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo, BYTE *pbyIndex)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.InsertReceiptExtraInfo(ReceiptInfo) ? OK : 1;
}

long CSQLPumpSrvReceiptExtraInfoDB::LoadReceiptExtraInfo(int TrsIndex, PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.LoadReceiptExtraInfo(TrsIndex, ReceiptInfo) ? OK : 1;
}

long CSQLPumpSrvReceiptExtraInfoDB::OperationEx(eFileOperation OpCode, PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo, BOOL bIsSaveByMaint)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	long lRetVal = 0;
	BYTE byIndexNumber = 0;

	long lPCNumber = 0;
	long lTimeStampOffset = 0;
	long lRecNum = 0;
	long lNumOfRecs = 0;
	long lExistNumOfRecs = 0;

	switch (OpCode)
	{
	case REC_INSERT:
		return PumpSrvDatabase.SaveReceiptExtraInfo(ReceiptInfo) ? OK : 1;
		break;

	case REC_READ:
		return PumpSrvDatabase.LoadReceiptExtraInfo(ReceiptInfo.info.lIndexNumber, ReceiptInfo) ? OK : 1;
		break;

	
	case REC_UPDATE:
	{
		lRetVal = PumpSrvDatabase.SaveReceiptExtraInfo(ReceiptInfo) ? OK : 1;
		break;
	}
	

	case REC_DELETE:
	{
		lRetVal = PumpSrvDatabase.DeleteReceiptExtraInfoRecord(ReceiptInfo) ? OK : 1;

		break;
	}

	
	}

	return lRetVal;
}

long CSQLPumpSrvReceiptExtraInfoDB::UpdateReceiptExtraInfo(PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.SaveReceiptExtraInfo(ReceiptInfo) ? OK : 1;
}
