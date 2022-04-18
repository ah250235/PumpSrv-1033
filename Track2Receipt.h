//this class will inherit from CBaseReceipt
//amram



#ifndef _CTRACK2_RECEIPT_H
#define _CTRACK2_RECEIPT_H

class CTrack2Receipt : public CBaseReceipt
{
	// Get Track2 Index Methods
	long	GetReceiptIndex(char* sIndexOut, PAY_AT_PUMP_INFO* pInfoIn, const long lPumpNumber = 0); //RFUEL - 172
	long	GetReceiptIndex(char* sIndexOut, const long lIndexSize, const BSTR sTrack2In, const long lFlags/*lFlags not in use*/, const long lPumpNumber = 0, const long lTrsNumber = 0);	
	long	GetReceiptIndex(char* sIndexOut, const long lIndexSize, const char* sTrack2In, const long lFlags/*lFlags not in use*/, const long lPumpNumber = 0);	

	// Parsing Methods
	void    ParseCardInfo(CARD_SALE_ALL3* cTmpCardSaleData, PAY_AT_PUMP_INFO*	PayAtPumpInfo);
 	void 	ParseCardInfo(CXMLInterface* cTmpCardSaleData, CXMLInterface* PayAtPumpInfo);

};



#endif //_CTRACK2_RECEIPT_H

