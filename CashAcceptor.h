// CashAcceptor.h: interface for the CCashAcceptor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASHACCEPTOR_H__026AFF84_E98E_11D6_BB7D_00500470AD8E__INCLUDED_)
#define AFX_CASHACCEPTOR_H__026AFF84_E98E_11D6_BB7D_00500470AD8E__INCLUDED_

#include "Pump.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCashAcceptor  
{
public:
	GCIRetCode CashAcceptorUpdateData(long &lPumpNumber, TAG_CASH_SALE_DATA &cTagCashSaleData, CPumpTransact  & trs);
	void GenerateCashAcceptorPayIn(long lPumpNumber, CPumpTransact & trs, long lFlags = 0);	//4.0.21.500 69415
	long GenerateReceiptRowData(CPumpTransact &trs);
	long GenerateReceiptRowDataUnrecognizedBill(CPumpTransact &trs, 	EXTRA_INFO_REC &ExtraInfoRec);
	long FinalizeTrs(CPumpTransact  & trs);
	long SendDataToOLA(CPumpTransact  & trs ,BOOL bPreAuthDetailsFlags= FALSE);
	void CreateOLARec(PAY_AT_PUMP_INFO & PayAtPumpInfo , CPumpTransact &trs ,EXTRA_INFO_REC	 & ExtraInfoRec ,BOOL bPreAuthDetailsFlags=FALSE, BOOL bUpdateUnrecognizedBill=FALSE); //4.0.17.90 - CR 40564
	long SendDataToOLA(CPumpTransact & trs, EXTRA_INFO_REC & ExtraInfoRec,BOOL bPreAuthDetailsFlags= FALSE, BOOL bUpdateUnrecognizedBill=FALSE); //4.0.17.90 - CR 40564
	void GetCashAcceptorInformation(EXTRA_INFO_REC &ExtraInfoRec, TAG_CASH_SALE_DATA &cTagCashSaleData);
	void UpdateFinalInformation(EXTRA_INFO_REC &ExtraInfoRec, TAG_CASH_SALE_DATA &cTagCashSaleData);
	void UpdatebillsInformation(long lPumpNumber, long lDevice,long lSeqNum,EXTRA_INFO_REC  &ExtraInfoRec ,TAG_CASH_SALE_DATA &cTagCashSaleData);
	CCashAcceptor();
	virtual ~CCashAcceptor();

};

#endif // !defined(AFX_CASHACCEPTOR_H__026AFF84_E98E_11D6_BB7D_00500470AD8E__INCLUDED_)
