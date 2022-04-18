#pragma once

#include "DB.h"

class CSQLPumpSrvReceiptExtraInfoDB
{
public:
	CSQLPumpSrvReceiptExtraInfoDB();
	virtual ~CSQLPumpSrvReceiptExtraInfoDB();

public:
	long GetFirstReceiptExtraInfoRecord(PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo);
	long GetNextReceiptExtraInfoRecord(int TrsIndex, PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo);
	long InsertReceiptExtraInfo(PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo, const CPumpTransact & Trs, long *Index);
	long InsertReceiptExtraInfo(PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo, BYTE *pbyIndex = 0);
	long LoadReceiptExtraInfo(int TrsIndex, PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo);
	long OperationEx(eFileOperation OpCode, PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo, BOOL bIsSaveByMaint = FALSE); //4.0.18.501
	long UpdateReceiptExtraInfo(PS::RECEIPT_EXTRA_INFO_REC & ReceiptInfo);
	

public:
	static CSQLPumpSrvReceiptExtraInfoDB	&	GetInstance(void);
};

inline CSQLPumpSrvReceiptExtraInfoDB & CSQLPumpSrvReceiptExtraInfoDB::GetInstance(void)
{
	static CSQLPumpSrvReceiptExtraInfoDB SQLReceiptDB;

	return SQLReceiptDB;
}

