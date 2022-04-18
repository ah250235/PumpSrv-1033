// PriceChangeFlags.h: interface for the CPriceChangeFlags class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRICECHANGEFLAGS_H__CB705FE3_3E9D_49AB_BE25_B54C8C345ED8__INCLUDED_)
#define AFX_PRICECHANGEFLAGS_H__CB705FE3_3E9D_49AB_BE25_B54C8C345ED8__INCLUDED_

#include "PumpClass.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



enum ePriceChangePumpTotalsRequestState
{
	PChange_NO_TOTALS_REQUEST = 0,
	PChange_TOTALS_REQUEST_STARTED = 1,
	PChange_TOTALS_REQUEST_RECEIVED = 2
};

enum ePriceChangeTanksReadState
{
	PChange_Tanks_NO_TOTALS_REQUEST = 0,
	PChange_Tanks_TOTALS_REQUEST_STARTED = 1,
	PChange_Tanks_TOTALS_REQUEST_RECEIVED = 2
};

class CPriceChangeFlags  
{
public:
	BOOL CheckInformPriceChangeCounter();
	void ResetInformPriceCounter();
	void SetInformPriceChangeTimer(long lCounter);
	void SavedFlags(SZ_TAG_SYSTEMTIME  & cPriceChangeTime , long lPriceChangeFlags);
	void LoadFlags();
	void GetPriceChangeDate( SZ_TAG_SYSTEMTIME & cDate );
	void SetPriceChangeDate(SZ_TAG_SYSTEMTIME  cPriceChangeTime);
	void SetPriceChangeStatus(long lVal);
	long GetPriceChangeStatus();
	CPriceChangeFlags();
	virtual ~CPriceChangeFlags();
	BOOL IsRequiredTotals(){return m_bPriceChangeTotalsRequired;}

	//CR_475695_start
	void SetPriceChangePumpTotalsRequestState(long lPumpNumber, ePriceChangePumpTotalsRequestState eTotalsRequestState) ;
	ePriceChangePumpTotalsRequestState GetPriceChangeTotalsRequestState(long lPumpNumber); 
	void SetPriceChangelPumpTotalsRequestState( ePriceChangePumpTotalsRequestState eTotalsRequestState );
	ePriceChangeTanksReadState GetPriceChangeTanksReadState( ){return m_ePriceChangeTanksRead;} 
	void SetPriceChangeTanksReadState( ePriceChangeTanksReadState eTanksReadState ){m_ePriceChangeTanksRead = eTanksReadState;}
	//CR_475695_End
private:
	SZ_TAG_SYSTEMTIME m_cPriceChangeTime;
	long m_lPriceChangeDone;
	long m_dwInformPriceChangeTotalCounter; //4.0.10.811
	long m_dwTmpInformPriceChangeCounter;
	BOOL m_bPriceChangeTotalsRequired;
	//CR_475695_start
	ePriceChangePumpTotalsRequestState m_ePriceChangePumpTotalsRequestState[MAX_PUMPS_64];
	ePriceChangeTanksReadState m_ePriceChangeTanksRead;
	//CR_475695_End
};

#endif // !defined(AFX_PRICECHANGEFLAGS_H__CB705FE3_3E9D_49AB_BE25_B54C8C345ED8__INCLUDED_)
