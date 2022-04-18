#include "stdafx.h"
#include "SQLReceiptDB.h"
#include "PumpSrvDatabase.h"
#include "ReceiptInfo.h"

CSQLReceiptDB::CSQLReceiptDB()
{
}

CSQLReceiptDB::~CSQLReceiptDB()
{
}

long CSQLReceiptDB::GetFirstReceiptRecord(CReceiptInfo & ReceiptInfo)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.GetFirstReceiptRecord(ReceiptInfo) ? OK : 1;
}

long CSQLReceiptDB::GetNextReceiptRecord(int TrsIndex, CReceiptInfo & ReceiptInfo)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.GetNextReceiptRecord(TrsIndex, ReceiptInfo) ? OK : 1;
}

long CSQLReceiptDB::InsertReceipt(CReceiptInfo & ReceiptInfo, const CPumpTransact & Trs, long *Index)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.InsertReceipt(ReceiptInfo) ? OK : 1;
}

long CSQLReceiptDB::InsertReceipt(CReceiptInfo & ReceiptInfo, BYTE *pbyIndex)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.InsertReceipt(ReceiptInfo) ? OK : 1;
}

long CSQLReceiptDB::LoadReceipt(int TrsIndex, CReceiptInfo & ReceiptInfo)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.LoadReceipt(TrsIndex, ReceiptInfo) ? OK : 1;
}

long CSQLReceiptDB::OperationEx(eFileOperation OpCode, CReceiptInfo & ReceiptInfo, BOOL bIsSaveByMaint)
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
		return PumpSrvDatabase.SaveReceipt(ReceiptInfo) ? OK : 1;
		break;

	case REC_READ:
		return PumpSrvDatabase.LoadReceipt(ReceiptInfo.m_TrsIndex, ReceiptInfo) ? OK : 1;
		break;

	case REC_READ_FIRST:
	{
		lRetVal = PumpSrvDatabase.GetFirstReceiptRecord(ReceiptInfo) ? OK : 1;
		break;
	}

	case REC_UPDATE:
	{
		lRetVal = PumpSrvDatabase.SaveReceipt(ReceiptInfo) ? OK : 1;
		break;
	}
	case REC_UPDATE_PREV:
	{
		lRetVal = PumpSrvDatabase.SaveReceipt(ReceiptInfo) ? OK : 1;
		break;
	}

	case REC_DELETE:
	{
		lRetVal = PumpSrvDatabase.DeleteReceiptRecord(ReceiptInfo) ? OK : 1;
		if (!lRetVal)
		{
			ReceiptInfo.Clear();
		}
		break;
	}

	case REC_READ_NEXT:
	{
		lRetVal = PumpSrvDatabase.GetNextReceiptRecord(ReceiptInfo.m_TrsIndex, ReceiptInfo) ? OK : 1;
		break;
	}
	case REC_READ_SEQ:
	{
		lRetVal = PumpSrvDatabase.GetNextReceiptRecord(ReceiptInfo.m_TrsIndex, ReceiptInfo) ? OK : 1;
		break;
	}
	}

	return lRetVal;
}

long CSQLReceiptDB::UpdateReceipt(CReceiptInfo & ReceiptInfo)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	return PumpSrvDatabase.SaveReceipt(ReceiptInfo) ? OK : 1;
}