// TimeOutManager.cpp: implementation of the TimeOutManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TimeOutManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeOutManager::CTimeOutManager()
{
	m_cStartTime = NULL;
	m_iSecondsTimeOutInterval = 0;	
}
CTimeOutManager::CTimeOutManager(CTime cStartTime , UINT iSecondsTimeOutInterval )
: m_cStartTime(cStartTime) , m_iSecondsTimeOutInterval(iSecondsTimeOutInterval)
{	
}

CTimeOutManager::CTimeOutManager( UINT iSecondsTimeOutInterval ): m_iSecondsTimeOutInterval(iSecondsTimeOutInterval)
{
	m_cStartTime = CTime::GetCurrentTime();
}

CTimeOutManager::CTimeOutManager(const CTimeOutManager & cTimeOutManager)
{
	m_cStartTime = cTimeOutManager.m_cStartTime;
	m_iSecondsTimeOutInterval = cTimeOutManager.m_iSecondsTimeOutInterval;
}

CTimeOutManager & CTimeOutManager::operator=(const CTimeOutManager & cTimeOutManager)
{
	m_cStartTime = cTimeOutManager.m_cStartTime;
	m_iSecondsTimeOutInterval = cTimeOutManager.m_iSecondsTimeOutInterval;
	return *this;
}

/******************************************************************************
 Description: Sets start time and timeout interval in seconds.
			  . 
 Assumption:    				
 Returns:     	
 Parameters:  
			   
 
 WHO			WHEN                    WHAT		Version
-------------------------------------------------------------------------------
 OmerT			11/08/2011   11:54		Start		4.0.23.1050 TD 233242
******************************************************************************/
void CTimeOutManager::SetTimeOutIntervalInSeconds(UINT iSecondsTimeOutInterval , CTime cStartTime)
{
	m_iSecondsTimeOutInterval = iSecondsTimeOutInterval;
	m_cStartTime = cStartTime;
}

/******************************************************************************
 Description: Sets start time and timeout interval in in minutes.
			  . 
 Assumption:    				
 Returns:     	
 Parameters:  
			   
 
 WHO			WHEN                    WHAT		Version
-------------------------------------------------------------------------------
 OmerT			11/08/2011   11:54		Start		4.0.23.1050 TD 233242
******************************************************************************/
void CTimeOutManager::SetTimeOutIntervalInMiuntes(UINT iMinutesTimeOutInterval , CTime cStartTime)
{
	m_iSecondsTimeOutInterval = iMinutesTimeOutInterval * SECONDS_IN_MINUTE;
	m_cStartTime = cStartTime;
}


/******************************************************************************
 Description: Returns True if: [current time - start time (in seconds) > timeout interval (in seconds)]
			  . 
 Assumption:    				
 Returns:     	
 Parameters:  
			   
 
 WHO			WHEN                    WHAT		Version
-------------------------------------------------------------------------------
 OmerT			11/08/2011   11:54		Start		4.0.23.1050 TD 233242
******************************************************************************/
BOOL CTimeOutManager::IsExpired() const
{
	if(m_cStartTime != NULL)
	{
		CTimeSpan cTimeSpan = CTime::GetCurrentTime() - m_cStartTime;
		return (cTimeSpan.GetTotalSeconds() >= m_iSecondsTimeOutInterval);
	}
	else
		return FALSE;

}
