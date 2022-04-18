//class the implement the receipt index and encrypt

#ifndef _CBASE_RECEIPT_H
#define _CBASE_RECEIPT_H

class CBaseReceipt{
public:
	CBaseReceipt();		
	~CBaseReceipt();	
	
	// Convert to/from OLA Methods
	long			ConvertOlaToReceipt(CPumpTransact* pTrs, void* pInfo, RECEIPT_INFO* pReceipt, short lOlaState = 0, const long lPumpNumber = 0, long lFlags = 0); //4.0.26.0 - TD 69677
	long		    ConvertOlaToReceipt(CPumpTransact *pTrs, PAY_AT_PUMP_INFO *pPAPInfo, CReceiptInfo & ReceiptInfo, short lOlaState = 0);

	// Get Index (Track2/AccountNumber/TrsNum)
	virtual long	GetReceiptIndex(char* sIndexOut, PAY_AT_PUMP_INFO * pInfo, const long lPumpNumber = 0) = 0; //RFUEL-172 
	virtual long	GetReceiptIndex(char* sIndexOut, const long lIndexSize, const BSTR sTrack2In, const long lFlags, const long lPumpNumber = 0, const long lTrsNumber = 0) = 0;	
	virtual long	GetReceiptIndex(char* sIndexOut, const long lIndexSize, const char* sTrack2In, const long lFlags, const long lPumpNumber = 0) = 0;	
 	virtual long    GetReceiptIndexByTrsNum(char* sIndexOut , const BSTR bstrTrsNumIn); // TD 270322 //4.0.30.0
 	
	// Parsing Methods
	virtual void	ParseCardInfo(CARD_SALE_ALL3* cTmpCardSaleData,PAY_AT_PUMP_INFO* PayAtPumpInfo) = 0;
	virtual void 	ParseCardInfo(CXMLInterface* cTmpCardSaleData, CXMLInterface* PayAtPumpInfo) = 0;

	//
	virtual void	SetNoneOPTReceiptAccountMask(PAY_AT_PUMP_INFO* pcPayAtPumpInfo, const char* pszAccountNum)const{/*EMPTY METHOD*/}
};


#endif//