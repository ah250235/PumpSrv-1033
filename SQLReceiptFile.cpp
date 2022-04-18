#include "stdafx.h"
#include "SQLReceiptFile.h"
#include "PumpSrvDatabase.h"

CSQLReceiptFile::CSQLReceiptFile()
{
}


CSQLReceiptFile::~CSQLReceiptFile()
{
}


long CSQLReceiptFile::OperationEx(eFileOperation OpCode, RECEIPT_INFO * pInfo, BOOL bIsSaveByMaint)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	long lRetVal = 0;
	BYTE byIndexNumber = 0;

	RECEIPT_REC_EX 		sRec;
	memset(&sRec, 0x00, sizeof(sRec));
	long lPCNumber = 0;
	long lTimeStampOffset = 0;
	long lRecNum = 0;
	long lNumOfRecs = 0;
	long lExistNumOfRecs = 0;

	//4.0.19??
	RECEIPT_INFO  & Receipt = *pInfo;


	switch (OpCode)
	{
	case REC_INSERT:
		return PumpSrvDatabase.Append(Receipt) ? OK : 1;
		break;

	case REC_READ:
		return PumpSrvDatabase.Load(Receipt) ? OK : 1;
		break;

	case REC_READ_FIRST:
	{
		lRetVal = PumpSrvDatabase.GetFirstRecord(Receipt) ? OK : 1;
		break;
	}

	case REC_UPDATE:
	{
		lRetVal = PumpSrvDatabase.Save(Receipt) ? OK : 1;
		break;
	}
	case REC_UPDATE_PREV:
	{
		RECEIPT_INFO  TmpReceipt = Receipt;

		--TmpReceipt.cReceipt1.Index.byIndex;
		--TmpReceipt.cReceipt2.Index.byIndex;
		--TmpReceipt.cReceipt3.Index.byIndex;
		--TmpReceipt.cReceipt4.Index.byIndex;
		--TmpReceipt.cReceipt5.Index.byIndex;
		--TmpReceipt.cReceipt6.Index.byIndex;

		lRetVal = PumpSrvDatabase.Save(TmpReceipt) ? OK : 1;
		break;
	}

	case REC_DELETE:
	{
		lRetVal = PumpSrvDatabase.DeleteRecord(Receipt) ? OK : 1;
		if (!lRetVal)
		{
			memset(&Receipt, 0, sizeof(Receipt));
		}
		break;
	}

	case REC_READ_NEXT:
	{
		lRetVal = PumpSrvDatabase.GetNextRecord(Receipt) ? OK : 1;
		break;
	}
	case REC_READ_SEQ:
	{
		lRetVal = PumpSrvDatabase.GetNextRecord(Receipt) ? OK : 1;
		break;
	}
	}

	return lRetVal;
}

