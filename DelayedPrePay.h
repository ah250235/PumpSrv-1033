// DelayedPrePay.h: interface for the CDelayedPrePay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DELAYEDPREPAY_H__4A44B6E3_31D2_11D7_BB92_00500470AD8E__INCLUDED_)
#define AFX_DELAYEDPREPAY_H__4A44B6E3_31D2_11D7_BB92_00500470AD8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CDelayedPrePay  
{
public:
	long GetPosNumber();
	long CreateNewPrePay(long lPumpNumber, PAY_AT_PUMP_INFO *pInfo);
	//4.0.18.501 long CreateNewPrePay(long lPumpNumber, CXMLInterface *pInfo);
	CDelayedPrePay();
	virtual ~CDelayedPrePay();

};

#endif // !defined(AFX_DELAYEDPREPAY_H__4A44B6E3_31D2_11D7_BB92_00500470AD8E__INCLUDED_)
