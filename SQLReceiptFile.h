#pragma once

class CSQLReceiptFile
{
public:
	CSQLReceiptFile();
	virtual ~CSQLReceiptFile();

public:
//	BYTE FindFirstFreePlace(RECEIPT_INFO *rec, BOOL bIsSaveByMaint = FALSE);//4.0.18.501

	//long DeleteReceiptExtendRecord(RECEIPT_INFO  &cReceipt);
	//long InsertReceipt(const RECEIPT_INFO * pInfo);

	//void ReOrganize(unsigned char *sTrack2);
	long OperationEx(eFileOperation OpCode, RECEIPT_INFO * pInfo, BOOL bIsSaveByMaint = FALSE); //4.0.18.501

	//void FillRecordGaps(RECEIPT_INFO * pFirstReceiptAfterGap, BYTE byLastRecordBeforeGapIndex); //4.0.18.501

	//BOOL FleetSecondTrs(long lTrsNumber); //4.0.15.501
	//void GetFleetSecondPreAuth(long lTrsNumber, long & lGradePrice, long & lGrade);
	//BOOL BalanceFleetCompere(PREPAY_AUTH_TRS cAuth, PREPAY_SOLD_TRS cSold);
	//BOOL CheckFleetLimits(long &lTransactNumber);
	//void FleetSecondApproval(long lPumpNumber, CPumpTransact & cTrs);
	//BOOL AddFleetDetails(long & lTransactionNumber, long &lVal, long  & lVolume, long & lNextMaxValueAproved, CPumpTransact & cFleetSum);
	//long ConvertTagRecord(TAG_EXTRA_INFO  & sTagRec, EXTRA_INFO & sRec, CPumpTransact & cTrs);
	//long InsertPayAtKioskRecord(CPumpTransact *pTrs, void *pData);
	//long DeleteRecord(CPumpTransact *pTrs);
	//long InsertPrePayRecord(CPumpTransact *pTrs, void *pData);
	//long Operation(eFileOperation OpCode, void * pInfo, BOOL bIsSaveByMaint = FALSE); //4.0.18.20 - CR 33898

	//void ClearRecord(EXTRA_INFO  &cExtraInfo);

	//void EmptyExtraInfoFile();					//4.0.23.510 no TD

protected:
	//long ParseBinFile(char * sFileName);

public:
	static CSQLReceiptFile	&	GetInstance(void);
};

inline CSQLReceiptFile & CSQLReceiptFile::GetInstance(void)
{
	static CSQLReceiptFile SQLReceiptFile;

	return SQLReceiptFile;
}

