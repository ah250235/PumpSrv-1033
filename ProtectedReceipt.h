// ProtectedReceipt.h: interface for the ProtectedReceipt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTECTEDRECEIPT_H__1356BC68_73A8_4217_82B7_0CC3C0A83A63__INCLUDED_)
#define AFX_PROTECTEDRECEIPT_H__1356BC68_73A8_4217_82B7_0CC3C0A83A63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProtectedReceipt : public CBaseReceipt  
{
public:
	// Get AccountNum/TrsNum (Receipt Index) Methods
	long	GetReceiptIndex(char* sIndexOut, PAY_AT_PUMP_INFO* pInfoIn, const long lPumpNumber = 0); //RFUEL - 172
	long	GetReceiptIndex(char* sIndexOut, const long lIndexSize /*lIndexSize not in use*/, const BSTR sTrack2In, const long lFlags, const long lPumpNumber = 0, const long lTrsNumber = 0);	
	long	GetReceiptIndex(char* sIndexOut, const long lIndexSize /*lIndexSize not in use*/, const char* sTrack2In, const long lFlags, const long lPumpNumber = 0);	

 	// Parsing Methods
	void	ParseCardInfo(CARD_SALE_ALL3* cTmpCardSaleData, PAY_AT_PUMP_INFO* PayAtPumpInfo);
	void 	ParseCardInfo(CXMLInterface* cTmpCardSaleData, CXMLInterface* PayAtPumpInfo);
	
	void	SetNoneOPTReceiptAccountMask(PAY_AT_PUMP_INFO* pcPayAtPumpInfo, const char* pszAccountNum)const;
};

#endif // !defined(AFX_PROTECTEDRECEIPT_H__1356BC68_73A8_4217_82B7_0CC3C0A83A63__INCLUDED_)
