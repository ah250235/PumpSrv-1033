// ReportGenerator.cpp: implementation of the CReportGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReportGenerator.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportGenerator::CReportGenerator()
{
	InitializeCriticalSection(&m_csReportGenerator);

	BOOL bLogInfo = FALSE;	                                                                 // TD 351225
	SetReportRequestState(REPORT_PROCESS_IDLE, "CReportGenerator() c'tor", bLogInfo);        // TD 351225
	SetReportRequestFailureResult(REPORT_NO_ERROR, "CReportGenerator() c'tor", bLogInfo);    // TD 351225

	memset(&m_cWaitTankReadingTime, 0, sizeof(SYSTEMTIME));
	memset(&m_cWaitReportTime, 0, sizeof(SYSTEMTIME));
	memset(&m_cStartProcessTime, 0, sizeof(SYSTEMTIME));

	m_bstrReportBuffer = "";
}

CReportGenerator::~CReportGenerator()
{
	DeleteCriticalSection(&m_csReportGenerator);
}

/******************************************************************************
 Description:	Set report request process state
 Returns:   	None
 Parameters:    BYTE byVal - Process state (REPORT_IDLE,
											REPORT_WAIT_TANK_READING,
											REPORT_SEND_REQUEST,
											REPORT_WAIT_GENERATE_REPORT,
											REPORT_READY)
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			03/02/2009   16:00		Start	4.0.22.501 - TD 69905
                24/03/2013                                   TD 351225
******************************************************************************/
void CReportGenerator::SetReportRequestState(const ReportRequestState eState, const char* const functionCaller, BOOL bLogInfo /*= TRUE */)
{
	CString str;
	EnterCriticalSection(&m_csReportGenerator);
	if(LOG_BASIC_FLOW_CONTROL)
	{
		str.Format("SetReportRequestState::%s OldState[%d][%s] changed to newState[%d][%s]", 
			functionCaller,
			m_eReportRequestState,
			ReportRequestState_str[m_eReportRequestState],
			eState,
			ReportRequestState_str[eState]);
	}
	m_eReportRequestState = eState;
	m_dwStateTimestamp = GetTickCount();
	LeaveCriticalSection(&m_csReportGenerator);
	if (bLogInfo && LOG_BASIC_FLOW_CONTROL)
	{
		_LOGMSG.LogMsg(str);
	}
}


/******************************************************************************
 Description:	Get report request process state
 Returns:   	short - process state
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			03/02/2009   16:00		Start	4.0.22.501 - TD 69905
                24/03/2013                                   TD 351225
******************************************************************************/
ReportRequestState CReportGenerator::GetReportRequestState()
{
	long lState = REPORT_PROCESS_IDLE;
	
	EnterCriticalSection(&m_csReportGenerator);

	ReportRequestState eState = m_eReportRequestState;

	LeaveCriticalSection(&m_csReportGenerator);

	return eState;
}


/******************************************************************************
 Description:	Set report request process failure result
 Returns:   	None
 Parameters:    BYTE byVal - Failure result (0 - success, other - failure) 

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			03/02/2009   16:00		Start	4.0.22.501 - TD 69905
                24/03/2013                                   TD 351225
******************************************************************************/
void CReportGenerator::SetReportRequestFailureResult(const ReportRequestError eError, const char* const functionCaller, BOOL bLogInfo /* = TRUE */)
{
	CString str;
	EnterCriticalSection(&m_csReportGenerator);
	if(LOG_BASIC_FLOW_CONTROL)
	{
		str.Format("SetReportRequestFailureResult::%s OldState[%d][%s] changed to newState[%d][%s]", 
			functionCaller,
			m_eReportRequestFailureResult,
			ReportRequestError_str[m_eReportRequestFailureResult],
			eError,
			ReportRequestState_str[eError]);
	}
	m_eReportRequestFailureResult = eError;
	LeaveCriticalSection(&m_csReportGenerator);
	if (bLogInfo && LOG_BASIC_FLOW_CONTROL)
	{
		_LOGMSG.LogMsg(str);
	}

}


/******************************************************************************
 Description:	Get report request process failure result
 Returns:   	long - process state
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			03/02/2009   16:00		Start	4.0.22.501 - TD 69905
                24/03/2013                                   TD 351225
******************************************************************************/
ReportRequestError CReportGenerator::GetReportRequestFailureResult()
{

	EnterCriticalSection(&m_csReportGenerator);

	ReportRequestError eError = m_eReportRequestFailureResult;

	LeaveCriticalSection(&m_csReportGenerator);

	return eError;

}

/******************************************************************************
 Description:	Handle all states for generate report process
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		11/02/2009   11:40			Start		4.0.22.501 TD 69905
******************************************************************************/
void CReportGenerator::HandleGenerateReportProcess(BOOL bInit, long lFlags, BOOL bReportDone)
{
	CString strLog;

	long lReportState = GetReportRequestState();
	
	switch(lReportState)
	{	
		case REPORT_PROCESS_IDLE:
		{
			if (bInit)
				HandleInitReportProcessIdleState();
			
			break;
		}
		case REPORT_SEND_TANK_READING:
		{
			HandleStateProgress();	//TD 351225 - Wetstock report does not print after wetstock process had arrived into a bad state
			break;
		}
			
		case REPORT_WAIT_TANK_READING:
		{
			HandleReportWaitTankReadingState(lFlags);			
			break;
		}

		case REPORT_GOT_TANK_READING_TIMEOUT:
		{
			HandleReportTankReadingTimeout();
			break;
		}

		case REPORT_WAIT_SEND_REQUEST:
		{
			HandleReportWaitSendRequestState();
			break;
		}

		case REPORT_SEND_REQUEST:
		{
			HandleStateProgress();	//TD 351225 - Wetstock report does not print after wetstock process had arrived into a bad state
			break;
		}

		case REPORT_WAIT_GENERATE_REPORT:
		{
			HandleReportWaitGenerateReportState(bReportDone);
			break;
		}

		case  REPORT_READY:
		{
			if (bInit)
			{
				SetReportRequestState(REPORT_PROCESS_IDLE, "HandleGenerateReportProcess");  // TD 351225
				HandleInitReportProcessIdleState();
			}

			break;
		}

		default:
			if (bInit)
			{
				SetReportRequestState(REPORT_PROCESS_IDLE, "HandleGenerateReportProcess");    // TD 351225 
				strLog.Format("CReportGenerator::HandleGenerateReportProcess - Init with REPORT IDLE state.");
				_LOGMSG.LogMsg(strLog);
			}
			else
			{
				strLog.Format("CReportGenerator::HandleGenerateReportProcess - Invalid state.");
				_LOGMSG.LogMsg(strLog);
			}
			
			break;
	}
}

/******************************************************************************
 Description:	When handling a wet stock report request, 
				get wet stock data from CL first and move to wait tank reading state
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		05/02/2009   15:00			Start		4.0.22.501 TD 69905
            24/03/2013                                         TD 351225
******************************************************************************/
void CReportGenerator::HandleInitReportProcessIdleState()
{	
	if (!_Module.m_server.m_cServerInfo.GetWetStockFlags())
	{
		// Get start process system time
		GetLocalTime(&m_cStartProcessTime);
		
		SetReportRequestState(REPORT_SEND_TANK_READING, "HandleInitReportProcessIdleState"); //TD 351225 - Wetstock report does not print after wetstock process had arrived into a bad state.
		//Changed the order from [GetWetStockData__] and only then [SetState(REPORT_SEND_TANK_READING)] to [SetState(REPORT_SEND_TANK_READING)] and only then [GetWetStockData__]
		
		// Start the process with requesting tank reading from CL
		_Module.m_server.GetWetStockData__(WSM_BY_REPORT);
		
		ResetWaitForTankReadingTime();
		InitWaitForTankReadingTime();
		
		m_bstrReportBuffer = "";
		

	}
	else
	{
		_LOGMSG.LogMsg("CReportGenerator. GetWetStockData process is active.");
	}
}


/******************************************************************************
 Description:	Handle report send tank reading state
				Check send tank reading timeout
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AssafW			13/01/2013   12:50			Start		4.0.22.950 TD 351225 - Wetstock report does not print after wetstock process had arrived into a bad state
******************************************************************************/
void CReportGenerator::HandleStateProgress()
{
	CString str;
	DWORD dwPassedms = GetTickCount() - m_dwStateTimestamp;
	DWORD dwThreshold = _Module.m_server.GetWetStockStateTimeout();
		
	if (dwPassedms > dwThreshold)// 15 minutes (900k ms) default
	{
		str.Format("HandleReportSendTankReading:: msPassed[%d], threshold[%d] had been crossed. Resetting state", dwPassedms, dwThreshold);
		SetReportRequestState(REPORT_PROCESS_IDLE, "HandleReportSendTankReading");
		SetReportRequestFailureResult(REPORT_ERROR_GENERATE_REPORT_TIMEOUT, "HandleReportSendTankReading");
	}
}


/******************************************************************************
 Description:	Handle wait tank reading state.
				Check tank reading timeout
 Returns:      	
 Parameters:   long lFlags - wet stock flags

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		11/02/2009   11:20			Start		4.0.22.501 TD 69905
            24/03/2013                                         TD 351225
******************************************************************************/
void CReportGenerator::HandleReportWaitTankReadingState(long lFlags)
{
	if (lFlags > 0 && _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_REPORT)
	{
		if((lFlags & TANK_READ_END_OF_LIST) && (lFlags & TANK_READ_NOT_AVAILABLE))
		{ 
			SetReportRequestState(REPORT_READY, "HandleReportWaitTankReadingState");
			SetReportRequestFailureResult(REPORT_ERROR_ALL_TANKS_NOT_AVAILABLE, "HandleReportWaitTankReadingState");

		}
		else
		{
			SetReportRequestState(REPORT_WAIT_SEND_REQUEST, "HandleReportWaitTankReadingState");
			SetReportRequestFailureResult(REPORT_NO_ERROR, "HandleReportWaitTankReadingState");

		}
	}
}

/******************************************************************************
 Description:	Handle wait send request state
				Send request for report to CL (FormatReport) 
				and move to send request state
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		11/02/2009   11:20			Start		4.0.22.501 TD 69905
******************************************************************************/
void CReportGenerator::HandleReportWaitSendRequestState()
{
	//Send FormatReport to CL with the tank reading data

	CMD_STRUCT_FORMAT_REPORT cmdInfo;
	
	cmdInfo.lPosNumber = 0;
	cmdInfo.lNumber = 0;
	cmdInfo.lFlags  = 0;
	cmdInfo.lType = GET_REPORT_WETSTOCK_ASYNC_TYPE;

	SetReportRequestState(REPORT_SEND_REQUEST, "HandleReportWaitSendRequestState");  // TD 351225

	_Module.m_server.SetCommand(CMD_FORMAT_REPORT, SERVER_POS_NUMBER ,0, &cmdInfo, sizeof(CMD_STRUCT_FORMAT_REPORT));						
	
	ResetWaitForReportTime();
	InitWaitForReportTime();

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("CReportGenerator. State=%ld (REPORT_SEND_REQUEST)", GetReportRequestState());
		_LOGMSG.LogMsg(str);
	}
	
}

/******************************************************************************
 Description:	Handle wait generate report request state.
				Check timeout for generating the report.
				Handle in case report is done
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		11/02/2009   11:20			Start		4.0.22.501 TD 69905
******************************************************************************/
void CReportGenerator::HandleReportWaitGenerateReportState(BOOL bDone)
{
	if (bDone)
	{
		//in case report is ready
		if (m_bstrReportBuffer.length() > 0)
		{
			SetReportRequestFailureResult(REPORT_NO_ERROR, "HandleReportWaitGenerateReportState");
		}
		else
		{
			SetReportRequestFailureResult(REPORT_ERROR_GENERATE, "HandleReportWaitGenerateReportState");
		}
		
		//Move get report process to REPORT_READY state
		SetReportRequestState(REPORT_READY, "HandleReportWaitGenerateReportState");

	}
	else
	{
		// Check generate report timeout
		BOOL bRetVal = FALSE;
		bRetVal = IsTimeOutWaitGenerateReport(); 

		if (bRetVal)
		{
			SetReportRequestState(REPORT_READY, "HandleReportWaitGenerateReportState");
			SetReportRequestFailureResult(REPORT_ERROR_GENERATE_REPORT_TIMEOUT, "HandleReportWaitGenerateReportState");
		}
	}
}

/******************************************************************************
 Description: Checks if there was timeout for generating report
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			08/02/2009   18:05		Start	  4.0.22.501 TD 69905
******************************************************************************/	
BOOL CReportGenerator::IsTimeOutWaitGenerateReport()
{
	return _Module.m_server.m_cCommonFunction.IsTimeOutPassed(m_cWaitReportTime, TIME_OUT_GENERATE_REPORT_CHECK);
}


/******************************************************************************
 Description: Checks if there was timeout for tank reading	
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			09/02/2009   11:20		Start	4.0.22.501 TD 69905
******************************************************************************/	
BOOL CReportGenerator::IsTimeOutWaitTankReading()
{
	BOOL bRetVal = FALSE;
	long lWetStockClearTime = _Module.m_server.GetWetStockClearTime();

    bRetVal = _Module.m_server.m_cCommonFunction.IsTimeOutPassed(m_cWaitTankReadingTime, lWetStockClearTime);
	if (bRetVal)
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("Wet stock report process: Tank Reading TimeOut had been reached");
			_LOGMSG.LogMsg(str);
		}
	}

	return bRetVal;
}

/******************************************************************************
 Description:	Set report text
 Returns:   	None
 Parameters:    

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			11/02/2009   14:50		Start	4.0.22.501 - TD 69905
******************************************************************************/
void CReportGenerator::SetReportBuffer(BSTR wszReportText)
{
	EnterCriticalSection(&m_csReportGenerator);

	m_bstrReportBuffer = wszReportText;

	LeaveCriticalSection(&m_csReportGenerator);	
}

/******************************************************************************
 Description:	Get report buffer
 Returns:   	None
 Parameters:    

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			11/02/2009   14:50		Start	4.0.22.501 - TD 69905
******************************************************************************/
_bstr_t CReportGenerator::GetReportBuffer()
{
	return m_bstrReportBuffer;
}


/******************************************************************************
 Description:	Handle report tank reading timeout
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		16/03/2009   11:00			Start	4.0.22.503 - TD 69905
            24/03/2013                                       TD 351225
******************************************************************************/
void CReportGenerator::HandleReportTankReadingTimeout()
{
	SetReportRequestState(REPORT_WAIT_SEND_REQUEST, "HandleReportTankReadingTimeout");
	SetReportRequestFailureResult(REPORT_NO_ERROR, "HandleReportTankReadingTimeout");
}