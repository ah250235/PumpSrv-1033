//this class will inherit from CBaseReceipt
//amram



#ifndef _CACCOUNT_RECEIPT_H
#define _CACCOUNT_RECEIPT_H

class CAccountReceipt : public CBaseReceipt
{

public:
	// Get AccountNum (Index) Methods
	long	GetReceiptIndex(char* sIndexOut, PAY_AT_PUMP_INFO* pInfoIn, const long lPumpNumber = 0); //RFUEL - 172
	long	GetReceiptIndex(char* sIndexOut, const long lIndexSize /*lIndexSize not in use*/, const BSTR sTrack2In, const long lFlags, const long lPumpNumber = 0, const long lTrsNumber = 0);	
	long	GetReceiptIndex(char* sIndexOut, const long lIndexSize /*lIndexSize not in use*/, const char* sTrack2In, const long lFlags, const long lPumpNumber = 0);

	// Parsing Methods
	void	ParseCardInfo(CARD_SALE_ALL3* cTmpCardSaleData, PAY_AT_PUMP_INFO*	PayAtPumpInfo);
	void 	ParseCardInfo(CXMLInterface* cTmpCardSaleData, CXMLInterface* PayAtPumpInfo);	
};



#endif //_CACCOUNT_RECEIPT_H

