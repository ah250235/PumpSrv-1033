// PriceChangeFlags.cpp: implementation of the CPriceChangeFlags class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PriceChangeFlags.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPriceChangeFlags::CPriceChangeFlags()
{
    m_lPriceChangeDone = 1;

   	SYSTEMTIME curSysTime;
	SZ_TAG_SYSTEMTIME szTagTime;
	GetLocalTime(&curSysTime);		

	// initialized date  info.
	szTagTime.sNull = 0;
	SystemTime2Tag(&curSysTime,&szTagTime.sTime);
	m_cPriceChangeTime  =szTagTime;

	m_bPriceChangeTotalsRequired = FALSE;
	//CR_475695_Start
	SetPriceChangelPumpTotalsRequestState(PChange_NO_TOTALS_REQUEST);
	m_ePriceChangeTanksRead = PChange_Tanks_NO_TOTALS_REQUEST;
	//CR_475695_End
}

CPriceChangeFlags::~CPriceChangeFlags()
{
   
}
/******************************************************************************
 Description:	Get price change status flag 	
 Returns:      	m_lPriceChangeDone = in progress / done
 Parameters:   	None.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   11:03		Start
******************************************************************************/

long CPriceChangeFlags::GetPriceChangeStatus()
{
	return m_lPriceChangeDone;
}
/******************************************************************************
 Description:	Set price change status flag 	
 Returns:      	None.
 Parameters:   	m_lPriceChangeDone = in progress / done
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   11:03		Start
******************************************************************************/

void CPriceChangeFlags::SetPriceChangeStatus(long lVal)
{
	m_lPriceChangeDone =  lVal;
	_Module.m_server.m_cParam.SaveParam("GeneralParam", "PriceChangeDone",(DWORD)lVal,TRUE,0,0,0,TRUE); ////4.0.8.512	 
	
}
/******************************************************************************
 Description:	Update date & time of price change process
 Returns:      	None.
 Parameters:   	m_lPriceChangeDone = in progress / done
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   11:03		Start
******************************************************************************/

void CPriceChangeFlags::SetPriceChangeDate(SZ_TAG_SYSTEMTIME cPriceChangeTime)
{
	m_cPriceChangeTime = cPriceChangeTime;
}
/******************************************************************************
 Description:	Get date & time of price change process
 Returns:      	None.
 Parameters:   	m_lPriceChangeDone = in progress / done
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   11:03		Start
******************************************************************************/

void CPriceChangeFlags::GetPriceChangeDate(SZ_TAG_SYSTEMTIME &cDate)
{
	cDate = m_cPriceChangeTime;
}

/******************************************************************************
 Description:	Load active flags of price change 
 Returns:      	None.
 Parameters:   	m_lPriceChangeDone = in progress / done
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   11:03		Start
******************************************************************************/

void CPriceChangeFlags::LoadFlags()
{
	SYSTEMTIME curSysTime;
	SZ_TAG_SYSTEMTIME szTagTime;
	DWORD dwTmp=0;
	GetLocalTime(&curSysTime);		

	szTagTime.sNull = 0;
	SystemTime2Tag(&curSysTime,&szTagTime.sTime);
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PriceChangeTime",(char *)&szTagTime,sizeof(szTagTime),(char *)&szTagTime,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE); //4.0.8.503
	m_cPriceChangeTime = szTagTime;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PriceChangeDone",&dwTmp,(DWORD)1,TRUE ,TRUE,0,HOP_DIRECT_LOCAL,TRUE); //4.0.8.503
	m_lPriceChangeDone = dwTmp;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PriceChangeTotalsRequired",&dwTmp,(DWORD)0, TRUE);				// TD 422282 
	m_bPriceChangeTotalsRequired = dwTmp;

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;
		str.Format("Load price change flags date=%.14s ,status=%ld, TotalsRequired=%ld", &szTagTime , m_lPriceChangeDone, m_bPriceChangeTotalsRequired);  // TD 422282 
		_LOGMSG.LogMsg(str);
	}




}
/******************************************************************************
 Description:	Save active flags of price change 
 Returns:      	None.
 Parameters:   	m_lPriceChangeDone = in progress / done
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   11:03		Start
******************************************************************************/

void CPriceChangeFlags::SavedFlags(SZ_TAG_SYSTEMTIME &cPriceChangeTime, long lPriceChangeFlags)
{
	DWORD dwTmp = lPriceChangeFlags;
	_Module.m_server.m_cParam.SaveParam("GeneralParam", "PriceChangeTime",(char *)&cPriceChangeTime,TRUE,0,0,0,TRUE); //4.0.8.503
	_Module.m_server.m_cParam.SaveParam("GeneralParam", "PriceChangeDone",dwTmp,TRUE,0,0,0,TRUE); //4.0.8.503

	m_lPriceChangeDone = dwTmp;
	m_cPriceChangeTime = cPriceChangeTime;
}

void CPriceChangeFlags::SetInformPriceChangeTimer(long lCounter)
{
	m_dwInformPriceChangeTotalCounter = (lCounter / 5);
}

void CPriceChangeFlags::ResetInformPriceCounter()
{
	m_dwTmpInformPriceChangeCounter =0;
}

// TRUE - Force ended price change pricess
BOOL CPriceChangeFlags::CheckInformPriceChangeCounter()
{
	BOOL bRet = FALSE;

	if (m_dwInformPriceChangeTotalCounter)
	{
		if (m_dwInformPriceChangeTotalCounter <= m_dwTmpInformPriceChangeCounter)
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{	
				CString str;
				str.Format("Force end Price change  process current count =%ld define =%ld" ,m_dwTmpInformPriceChangeCounter ,m_dwInformPriceChangeTotalCounter);
				_LOGMSG.LogMsg(str);
			}

			bRet = TRUE;

		}
		m_dwTmpInformPriceChangeCounter++;
	}

	return  bRet;
}

void CPriceChangeFlags::SetPriceChangelPumpTotalsRequestState( ePriceChangePumpTotalsRequestState eTotalsRequestState )
{
  	for (int p = 0; p < MAX_PUMPS_64; p++)  
	{
		m_ePriceChangePumpTotalsRequestState[p] = eTotalsRequestState;
	}
}


void CPriceChangeFlags::SetPriceChangePumpTotalsRequestState( long lPumpNumber, ePriceChangePumpTotalsRequestState eTotalsRequestState )
{
	m_ePriceChangePumpTotalsRequestState[lPumpNumber - 1] = eTotalsRequestState;
}


ePriceChangePumpTotalsRequestState CPriceChangeFlags::GetPriceChangeTotalsRequestState( long lPumpNumber )
{
	return m_ePriceChangePumpTotalsRequestState[lPumpNumber - 1];
}
