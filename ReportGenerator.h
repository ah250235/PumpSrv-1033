// ReportGenerator.h: interface for the CReportGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTGENERATOR_H__29683966_F8EF_4748_9485_6C8DA140892E__INCLUDED_)
#define AFX_REPORTGENERATOR_H__29683966_F8EF_4748_9485_6C8DA140892E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************************************************************************
*******************************************************************************
						CReportGenerator class

This class is responsible for managing an async report process.
4.0.22.501 - TD 69905
 
*******************************************************************************
******************************************************************************/

#define TIME_OUT_GENERATE_REPORT_CHECK			10000    //(10 secs.) 


class CReportGenerator  
{
public:
	CReportGenerator();
	virtual ~CReportGenerator();

	void HandleGenerateReportProcess(BOOL bInit = FALSE, long lFlags = 0, BOOL bReportDone = FALSE);

	void SetReportRequestState(const ReportRequestState eState, const char* const functionCaller, BOOL bLogInfo = TRUE);   // TD 351225
	ReportRequestState GetReportRequestState();   // TD 351225
	void SetReportRequestFailureResult(const ReportRequestError eError, const char* const functionCaller, BOOL bLogInfo = TRUE);
	ReportRequestError GetReportRequestFailureResult();          // TD 351225
	
	_bstr_t	GetReportBuffer();
	void	SetReportBuffer(BSTR wszReportBuffer);

	SYSTEMTIME GetStartTime(){return m_cStartProcessTime;}

private:
	CRITICAL_SECTION	m_csReportGenerator;

	ReportRequestState	m_eReportRequestState;             // TD 351225
	ReportRequestError	m_eReportRequestFailureResult;     // TD 351225
	DWORD				m_dwStateTimestamp;                // TD 351225

	SYSTEMTIME			m_cWaitReportTime;
	SYSTEMTIME			m_cWaitTankReadingTime;

	_bstr_t				m_bstrReportBuffer;

	SYSTEMTIME			m_cStartProcessTime;
	
	void InitWaitForReportTime(){ GetLocalTime(&m_cWaitReportTime); }
	void ResetWaitForReportTime(){ memset(&m_cWaitReportTime, 0, sizeof(SYSTEMTIME));}
	BOOL IsTimeOutWaitGenerateReport();	

	void InitWaitForTankReadingTime(){ GetLocalTime(&m_cWaitTankReadingTime); }
	void ResetWaitForTankReadingTime(){ memset(&m_cWaitTankReadingTime, 0, sizeof(SYSTEMTIME));}
	BOOL IsTimeOutWaitTankReading();
	
	void HandleInitReportProcessIdleState();
	void HandleReportWaitTankReadingState(long lFlags);
	void HandleReportWaitSendRequestState();
	void HandleReportWaitGenerateReportState(BOOL bDone);
	void HandleStateProgress();		//TD 351225 - Wetstock report does not print after wetstock process had arrived into a bad state
	void HandleReportTankReadingTimeout();
};

#endif // !defined(AFX_REPORTGENERATOR_H__29683966_F8EF_4748_9485_6C8DA140892E__INCLUDED_)
