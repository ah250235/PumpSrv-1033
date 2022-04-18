//4.0.27.40 TD 165585
// FlowRate.h: interface for the CFlowRateCalculator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOWRATE_H__DF41A7FD_F9F7_4B64_8762_A87765336BCD__INCLUDED_)
#define AFX_FLOWRATE_H__DF41A7FD_F9F7_4B64_8762_A87765336BCD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFlowRateCalculator  
{
private:
	long CalculateFlowRateByMinute(CTime cTimeStart, CTime cTimeEnd, long lVolume, long lTrsFlowRate, long lRatesFromDevice);

public:
	CFlowRateCalculator();
	virtual ~CFlowRateCalculator();

	long SetFlowRateByMinute(CPumpTransact & trs, CARD_SALE_ALL3 & cCardData);
	long SetFlowRateByMinute(CPumpTransact & trs, EXTRA_INFO & cExtraInfo);
};

#endif // !defined(AFX_FLOWRATE_H__DF41A7FD_F9F7_4B64_8762_A87765336BCD__INCLUDED_)
