// TimeOutManager.h: interface for the TimeOutManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEOUTMANAGER_H__16D1FB4D_12D7_4529_AEE7_60C627655A5E__INCLUDED_)
#define AFX_TIMEOUTMANAGER_H__16D1FB4D_12D7_4529_AEE7_60C627655A5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*********************************************************************************************************************
* Class: TimeOutManager
* Description: The timeout manager allows to set a new timeout and 
*			  receive notification if the timeout expired.
*			  The timeout manager will need the starting time and the timeout interval.
*
* How to use: use one of the following options:
*
* 1.Construct a new timeout by calling TimeOutManager CTOR with timeout interval in seconds as parameter
* In option 1 the starting time will be set to the current time.
*			 2.Construct a new timeout by calling TimeOutManager with timeout interval in seconds and
*			   starting time as parameters.
*			 3.Construct a new timeout by calling default CTOR. Set the starting time and timeout interval when available.
* Services:
*
* - SetTimeOutInterval - Set the timeout interval. If start time was not passed. use the current time.
* - IsExpired - Report if timeout expired 
*
*   WHO			WHEN                    WHAT		Version
------------------------------------------------------------------------------------------------------------------------
    OmerT		11/08/2011				Start		4.0.23.1050 TD 233242
************************************************************************************************************************/
class CTimeOutManager  
{
	// methods	
public:

	// CTORs
	CTimeOutManager();
	CTimeOutManager(UINT iSecondsTimeOutInterval); // starting time is set to current time
	CTimeOutManager(CTime cStartTime, UINT iSecondsTimeOutInterval); // pass the starting time
	CTimeOutManager(const CTimeOutManager & cTimeOutManager);

	// DTORs
	virtual ~CTimeOutManager() {};

	// operators
	CTimeOutManager & operator=(const CTimeOutManager & cTimeOutManager);

	//public methods
	void SetTimeOutIntervalInSeconds(UINT iSecondsTimeOutInterval , CTime cStartTime = CTime::GetCurrentTime());
	void SetTimeOutIntervalInMiuntes(UINT iMinutesTimeOutInterval , CTime cStartTime = CTime::GetCurrentTime());
	BOOL IsExpired() const;

	// members
protected:
	UINT  m_iSecondsTimeOutInterval;
	CTime m_cStartTime;
};

#endif // !defined(AFX_TIMEOUTMANAGER_H__16D1FB4D_12D7_4529_AEE7_60C627655A5E__INCLUDED_)
