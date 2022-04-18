#pragma once

#include "DB.h"

class CSQLReceiptDB
{
public:
	CSQLReceiptDB();
	virtual ~CSQLReceiptDB();

public:
	long GetFirstReceiptRecord(CReceiptInfo & ReceiptInfo);
	long GetNextReceiptRecord(int TrsIndex, CReceiptInfo & ReceiptInfo);
	long InsertReceipt(CReceiptInfo & ReceiptInfo, const CPumpTransact & Trs, long *Index);
	long InsertReceipt(CReceiptInfo & ReceiptInfo, BYTE *pbyIndex = 0);
	long LoadReceipt(int TrsIndex, CReceiptInfo & ReceiptInfo);
	long OperationEx(eFileOperation OpCode, CReceiptInfo & ReceiptInfo, BOOL bIsSaveByMaint = FALSE); //4.0.18.501
	long UpdateReceipt(CReceiptInfo & ReceiptInfo);

public:
	static CSQLReceiptDB	&	GetInstance(void);
};

inline CSQLReceiptDB & CSQLReceiptDB::GetInstance(void)
{
	static CSQLReceiptDB SQLReceiptDB;

	return SQLReceiptDB;
}

