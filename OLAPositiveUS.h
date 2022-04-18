// OLAPositiveUS.h: interface for the COLAPositiveUS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLAPOSITIVEUS_H__563C1EBA_A9A0_4AE2_B8F3_6D60BC54B33A__INCLUDED_)
#define AFX_OLAPOSITIVEUS_H__563C1EBA_A9A0_4AE2_B8F3_6D60BC54B33A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OLAPositiveSrv.h"	//4.0.23.90 85955 //4.0.23.504

class COLAPositiveUS : public COLAPositiveSrv  
{
public:
	COLAPositiveUS();
	virtual ~COLAPositiveUS();
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL); //4.0.23.90 85955
	virtual void MatchSentLoyaltyCardInfoToOLA(PAY_AT_PUMP_INFO &pInOutPAPBuffer);  //4.0.23.507 89548
	virtual void MatchReceivedLoyaltyCardInfoToOLA(CARD_SALE_ALL3 &pInOutPAPBuffer);  //4.0.23.507 89548

};

#endif // !defined(AFX_OLAPOSITIVEUS_H__563C1EBA_A9A0_4AE2_B8F3_6D60BC54B33A__INCLUDED_)
