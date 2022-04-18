// DeliveryEmulationSingleTankServerInfo.cpp: implementation of the CDeliveryEmulationSingleTankServerInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeliveryEmulationSingleTankServerInfo.h"

//******************************************************************************
//******************************************************************************

//				CTankDeliveryEmulationServerInfo class

//******************************************************************************
//******************************************************************************

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeliveryEmulationSingleTankServerInfo::CDeliveryEmulationSingleTankServerInfo()
{
	InitializeCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);
}

CDeliveryEmulationSingleTankServerInfo::~CDeliveryEmulationSingleTankServerInfo()
{ 
	DeleteCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);
}

//Set and get functions:

/******************************************************************************
 Description:	Set Single tank 's Delivery emulation new status.  
 Returns:   	None
 Parameters:    Tank number, byVal  - Process status (DES_SINGLE_TANK_IDLE,
                                                      DES_SINGLE_TANK_START_LOCK,
													  DES_SINGLE_TANK_WAIT_FOR_LOCK,
													  DES_SINGLE_TANK_WAIT_TIMEOUT,
													  DES_SINGLE_TANK_REQUEST_TANK_READING,
													  DES_SINGLE_TANK_WAIT_TANK_READING,
													  DES_SINGLE_TANK_UNLOCK)
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CDeliveryEmulationSingleTankServerInfo::SetDeliveryEmulationSingleTankStatus(const long lTankNumber, const BYTE byVal)
{
	EnterCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString strLog;

		if (!TANK_NOT_VALID(lTankNumber))
		{
		/*	strLog.Format("Set delivery emulation for single tank (Tank - %ld) status new: %d, old : %d Delivery Process: %d.", 
			lTankNumber, byVal , m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetDeliveryEmulationStatus() , m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetDeliveryEmulationProcessType());
			_LOGMSG.LogMsg(strLog);*/ //4.0.22.510
		}
		else
		{
			strLog.Format("Set delivery emulation for single tank status: Tank number not valid  %ld !!", lTankNumber); 
			_LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
		}		
	}

	if (!TANK_NOT_VALID(lTankNumber))
		m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].SetDeliveryEmulationStatus(byVal);

	LeaveCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

}

/******************************************************************************
 Description:	Get Single Tank's Delivery emulation new status.  
 Returns:   	Process status (DES_SINGLE_TANK_IDLE,
                                DES_SINGLE_TANK_START_LOCK,
								DES_SINGLE_TANK_WAIT_FOR_LOCK,
								DES_SINGLE_TANK_WAIT_TIMEOUT,
								DES_SINGLE_TANK_REQUEST_TANK_READING,
								DES_SINGLE_TANK_WAIT_TANK_READING,
								DES_SINGLE_TANK_UNLOCK)
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
BYTE CDeliveryEmulationSingleTankServerInfo::GetDeliveryEmulationSingleTankStatus(const long lTankNumber)
{
	BYTE byVal = DES_SINGLE_TANK_IDLE;

	if (!TANK_NOT_VALID(lTankNumber))
		byVal =  m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetDeliveryEmulationStatus(); 
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString strLog;
		    strLog.Format("Get delivery emulation for single tank status: Tank number not valid  %ld !!", lTankNumber); 
		    _LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
		}
	}

	return byVal;

}

/******************************************************************************
 Description:	Set Single Tank's Delivery emulation new process type.  
 Returns:   	None
 Parameters:    lTankNumber, byVal  - new process type (PD_SINGLE_TANK_NONE,					
		                                                PD_SINGLE_TANK_START,					     
		                                                PD_SINGLE_TANK_END,
		                                                PD_SINGLE_TANK_FORCE_RELEASE).
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CDeliveryEmulationSingleTankServerInfo::SetDeliveryEmulationSingleTankProcessType(const long lTankNumber, const ProcessDeliverySingleTank lVal)
{
	EnterCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString strLog;

		if (!TANK_NOT_VALID(lTankNumber))
		{
			/*strLog.Format("Set delivery emulation for single tank (Tank - %ld) process type new: %d, old : %d Delivery Status: %d.", 
			lTankNumber, lVal , m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetDeliveryEmulationProcessType(), m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetDeliveryEmulationStatus()); 
			_LOGMSG.LogMsg(strLog);*/ //4.0.22.510
		}
		else
		{
			strLog.Format("Set delivery emulation for single tank process type: Tank number not valid  %ld !!", lTankNumber); 
			_LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
		}		
	}

	if (!TANK_NOT_VALID(lTankNumber))
		m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].SetDeliveryEmulationProcessType(lVal);

	LeaveCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

}

/******************************************************************************
 Description:	Get Single Tank's Delivery emulation new process type .  
 Returns:   	New process type (PD_SINGLE_TANK_NONE,					
		                          PD_SINGLE_TANK_START,					     
		                          PD_SINGLE_TANK_END,
		                          PD_SINGLE_TANK_FORCE_RELEASE).
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
ProcessDeliverySingleTank CDeliveryEmulationSingleTankServerInfo::GetDeliveryEmulationSingleTankProcessType(const long lTankNumber)
{
	ProcessDeliverySingleTank lVal = PD_SINGLE_TANK_NONE;

	if (FALSE == TANK_NOT_VALID(lTankNumber))
		lVal =  m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetDeliveryEmulationProcessType();
	else
	{
		if(LOG_FCC_FLOW_CONTROL)
		{
			CString strLog;
		    strLog.Format("Get delivery emulation for single tank process type: Tank number not valid  %ld !!", lTankNumber); 
		    _LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
		}
	}

	return lVal;

}

/******************************************************************************
 Description:	Set Single Tank's Delivery emulation new success status.  
 Returns:   	None
 Parameters:    lTankNumber, byVal  - new success status (DESF_SINGLE_TANK_NOT_DONE, 
                                                          DESF_SINGLE_TANK_TIMEOUT,
                                                          DESF_SINGLE_TANK_FAILURE,
														  DESF_SINGLE_TANK_DONE).
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CDeliveryEmulationSingleTankServerInfo::SetDeliveryEmulationSingleTankSuccess(const long lTankNumber, const BYTE byVal)
{
	EnterCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString strLog;

		if (!TANK_NOT_VALID(lTankNumber))
		{
		/*	strLog.Format("Set delivery emulation for single tank (Tank - %ld) Success new: %d, old : %d", lTankNumber, byVal , m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetDeliveryEmulationSuccess);
			_LOGMSG.LogMsg(strLog);*/ //4.0.22.510
		}
		else
		{
			strLog.Format("Set delivery emulation for single tank Success: Tank number not valid  %ld !!", lTankNumber); 
			_LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
		}		
	}

	if (!TANK_NOT_VALID(lTankNumber))
		m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].SetDeliveryEmulationSuccess(byVal);

	LeaveCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

}

/******************************************************************************
 Description:	Get Single Tank's Delivery emulation success status.  
 Returns:   	Success status (DESF_SINGLE_TANK_NOT_DONE, 
                                DESF_SINGLE_TANK_TIMEOUT,
                                DESF_SINGLE_TANK_FAILURE,
					            DESF_SINGLE_TANK_DONE).
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
BYTE CDeliveryEmulationSingleTankServerInfo::GetDeliveryEmulationSingleTankSuccess(const long lTankNumber)
{
	BYTE byVal = FALSE;

	if (!TANK_NOT_VALID(lTankNumber))
		byVal =  m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetDeliveryEmulationSuccess();
    else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString strLog;
		    strLog.Format("Get delivery emulation for single tank Success: Tank number not valid  %ld !!", lTankNumber); 
		    _LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
		}
	}

	return byVal;

}

/******************************************************************************
 Description:	Set Single Tank Delivery emulation new "Process Stopped" flag value.  
               (SINGLE_TANK_PROCESS_NOT_STOPPED,
                SINGLE_TANK_PROCESS_STOPPED_BY_ABORT,
                SINGLE_TANK_PROCESS_STOPPED_BY_TIMEOUT,
                SINGLE_TANK_PROCESS_STOPPED_BY_FAILURE)

 Returns:   	None
 Parameters:    lTankNumber, byVal  - new value
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CDeliveryEmulationSingleTankServerInfo::SetDeliveryEmulationSingleTankProcessStopped(const long lTankNumber, const SingleTankProcessStopped lVal)
{
	EnterCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString strLog;

		if (!TANK_NOT_VALID(lTankNumber))
		{
			/*strLog.Format("Set delivery emulation for single tank (Tank - %ld) Process Stopped flag: %d, old : %d", lTankNumber, lVal , m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetDeliveryEmulationProcessStopped);
			_LOGMSG.LogMsg(strLog);*/ //4.0.22.510
		}
		else
		{
			strLog.Format("Set delivery emulation for single tank Process Stopped flag: Tank number not valid  %ld !!", lTankNumber); 
			_LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
		}		
	}

	if (!TANK_NOT_VALID(lTankNumber))
		m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].SetDeliveryEmulationProcessStopped(lVal);

	LeaveCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

}

/******************************************************************************
 Description:	Get Single Tank Delivery emulation "Process Stopped" flag value
               (SINGLE_TANK_PROCESS_NOT_STOPPED,
                SINGLE_TANK_PROCESS_STOPPED_BY_ABORT,
                SINGLE_TANK_PROCESS_STOPPED_BY_TIMEOUT,
                SINGLE_TANK_PROCESS_STOPPED_BY_FAILURE). 
 Returns:   	Single Tank's "Process Stopped" flag value.
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
SingleTankProcessStopped CDeliveryEmulationSingleTankServerInfo::GetDeliveryEmulationSingleTankProcessStopped(const long lTankNumber)
{
	SingleTankProcessStopped lVal = SINGLE_TANK_PROCESS_NOT_STOPPED;

	if (!TANK_NOT_VALID(lTankNumber))
		lVal =  m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetDeliveryEmulationProcessStopped();
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString strLog;
		    strLog.Format("Get delivery emulation for single tank Process Stopped flag: Tank number not valid  %ld !!", lTankNumber); 
		    _LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
		}
	}

	return lVal;

}

/******************************************************************************
 Description:	Set Single Tank's new lock status.  
 Returns:   	None
 Parameters:    lTankNumber, byVal  - new lock status
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CDeliveryEmulationSingleTankServerInfo::SetSingleTankLockStatus(const long lTankNumber, const SingleTankLockStatus lVal)
{
	EnterCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString strLog;

		if (!TANK_NOT_VALID(lTankNumber))
		{
			/*strLog.Format("Set Single Tank Lock Status (Tank - %ld) new: %d, old : %d", lTankNumber, lVal , m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetLockStatus());
			_LOGMSG.LogMsg(strLog);*/ //4.0.22.510
		}
		else
		{
			strLog.Format("Set Single Tank Lock Status : Tank number not valid  %ld !!", lTankNumber); 
			_LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
		}		
	}

	if (!TANK_NOT_VALID(lTankNumber))
		m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].SetLockStatus(lVal);

	LeaveCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

}

/******************************************************************************
 Description:	Get Single Tank's new lock status.  
 Returns:   	New lock status.
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
SingleTankLockStatus CDeliveryEmulationSingleTankServerInfo::GetSingleTankLockStatus(const long lTankNumber)
{
	SingleTankLockStatus lVal = SINGLE_TANK_UNLOCKED;

	if (!TANK_NOT_VALID(lTankNumber))
		lVal =  m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].GetLockStatus();
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString strLog;
		    strLog.Format("Get single tank Lock Status: Tank number not valid  %ld !!", lTankNumber); 
		    _LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
		}
	}

	return lVal;

}

//Init functions:

/******************************************************************************
 Description:	Reset single tank flags (Delivery emulation and lock).
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Shelly B		24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CDeliveryEmulationSingleTankServerInfo::InitSingleTankFlags()
{
	EnterCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

	for (int lTankNumber = 1; lTankNumber <= _Module.m_server.GetLastValidTank(); lTankNumber++ )
		m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].InitFlags();
	
	LeaveCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);
}

/******************************************************************************
 Description:	Reset single tank flags of a certain tank (Delivery emulation and lock).
 Returns:       None.	
 Parameters:   	Tank Number.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Shelly B		24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CDeliveryEmulationSingleTankServerInfo::InitSingleSpecificTankFlags(const long lTankNumber)
{
	EnterCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);

	m_cTankDeliveryEmulationServerInfoArray[lTankNumber - 1].InitFlags();
	
	LeaveCriticalSection(&m_csDeliveryEmulationSingleTankServerInfo);
}


//******************************************************************************
//******************************************************************************

//				CTankDeliveryEmulationServerInfo class

//******************************************************************************
//******************************************************************************

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTankDeliveryEmulationServerInfo::CTankDeliveryEmulationServerInfo()
{
	InitializeCriticalSection(&m_csTankDeliveryEmulationServerInfo);
	InitFlags();
}

CTankDeliveryEmulationServerInfo::~CTankDeliveryEmulationServerInfo()
{
	DeleteCriticalSection(&m_csTankDeliveryEmulationServerInfo);
}

//Set and get functions:

/******************************************************************************
 Description:	Set Single tank 's Delivery emulation new status.  
 Returns:   	None
 Parameters:    Tank number, byVal  - Process status (DES_SINGLE_TANK_IDLE,
                                                      DES_SINGLE_TANK_START_LOCK,
													  DES_SINGLE_TANK_WAIT_FOR_LOCK,
													  DES_SINGLE_TANK_WAIT_TIMEOUT,
													  DES_SINGLE_TANK_REQUEST_TANK_READING,
													  DES_SINGLE_TANK_WAIT_TANK_READING,
													  DES_SINGLE_TANK_UNLOCK)
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CTankDeliveryEmulationServerInfo::SetDeliveryEmulationStatus(const BYTE byVal)
{
	EnterCriticalSection(&m_csTankDeliveryEmulationServerInfo);

	m_byDeliveryEmulationSingleTankStatus = byVal;

	LeaveCriticalSection(&m_csTankDeliveryEmulationServerInfo);

}	
  
/******************************************************************************
 Description:	Get Single Tank's Delivery emulation new status.  
 Returns:   	Process status (DES_SINGLE_TANK_IDLE,
                                DES_SINGLE_TANK_START_LOCK,
								DES_SINGLE_TANK_WAIT_FOR_LOCK,
								DES_SINGLE_TANK_WAIT_TIMEOUT,
								DES_SINGLE_TANK_REQUEST_TANK_READING,
								DES_SINGLE_TANK_WAIT_TANK_READING,
								DES_SINGLE_TANK_UNLOCK)
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
BYTE CTankDeliveryEmulationServerInfo::GetDeliveryEmulationStatus()
{
	return m_byDeliveryEmulationSingleTankStatus;
}

/******************************************************************************
 Description:	Set Single Tank's Delivery emulation new process type.  
 Returns:   	None
 Parameters:    lTankNumber, byVal  - new process type (PD_SINGLE_TANK_NONE,					
		                                                PD_SINGLE_TANK_START,					     
		                                                PD_SINGLE_TANK_END,
		                                                PD_SINGLE_TANK_FORCE_RELEASE).
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CTankDeliveryEmulationServerInfo::SetDeliveryEmulationProcessType(const ProcessDeliverySingleTank lVal)
{
	EnterCriticalSection(&m_csTankDeliveryEmulationServerInfo);

	m_lDeliveryEmulationSingleTankProcess = lVal;

	LeaveCriticalSection(&m_csTankDeliveryEmulationServerInfo);

}

/******************************************************************************
 Description:	Get Single Tank's Delivery emulation new process type .  
 Returns:   	New process type (PD_SINGLE_TANK_NONE,					
		                          PD_SINGLE_TANK_START,					     
		                          PD_SINGLE_TANK_END,
		                          PD_SINGLE_TANK_FORCE_RELEASE).
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
ProcessDeliverySingleTank CTankDeliveryEmulationServerInfo::GetDeliveryEmulationProcessType()
{
	return m_lDeliveryEmulationSingleTankProcess;
}
	
/******************************************************************************
 Description:	Set Single Tank's Delivery emulation new success status.  
 Returns:   	None
 Parameters:    lTankNumber, byVal  - new success status (DESF_SINGLE_TANK_NOT_DONE, 
                                                          DESF_SINGLE_TANK_TIMEOUT,
                                                          DESF_SINGLE_TANK_FAILURE,
														  DESF_SINGLE_TANK_DONE).
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CTankDeliveryEmulationServerInfo::SetDeliveryEmulationSuccess(const BYTE byVal)
{
	EnterCriticalSection(&m_csTankDeliveryEmulationServerInfo);

	m_byDeliveryEmulationSingleTankSuccess = byVal;

	LeaveCriticalSection(&m_csTankDeliveryEmulationServerInfo);

}

/******************************************************************************
 Description:	Get Single Tank's Delivery emulation success status.  
 Returns:   	Success status (DESF_SINGLE_TANK_NOT_DONE, 
                                DESF_SINGLE_TANK_TIMEOUT,
                                DESF_SINGLE_TANK_FAILURE,
					            DESF_SINGLE_TANK_DONE).
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
BYTE CTankDeliveryEmulationServerInfo::GetDeliveryEmulationSuccess()
{
	return m_byDeliveryEmulationSingleTankSuccess;
}
	
/******************************************************************************
 Description:	Set Single Tank Delivery emulation new "Process Stopped" flag value.  
               (SINGLE_TANK_PROCESS_NOT_STOPPED,
                SINGLE_TANK_PROCESS_STOPPED_BY_ABORT,
                SINGLE_TANK_PROCESS_STOPPED_BY_TIMEOUT,
                SINGLE_TANK_PROCESS_STOPPED_BY_FAILURE)

 Returns:   	None
 Parameters:    lTankNumber, byVal  - new value
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CTankDeliveryEmulationServerInfo::SetDeliveryEmulationProcessStopped(const SingleTankProcessStopped lVal)
{
	EnterCriticalSection(&m_csTankDeliveryEmulationServerInfo);

	m_lDeliveryEmulationSingleTankProcessStopped = lVal;

	LeaveCriticalSection(&m_csTankDeliveryEmulationServerInfo);

}

/******************************************************************************
 Description:	Get Single Tank Delivery emulation "Process Stopped" flag value
               (SINGLE_TANK_PROCESS_NOT_STOPPED,
                SINGLE_TANK_PROCESS_STOPPED_BY_ABORT,
                SINGLE_TANK_PROCESS_STOPPED_BY_TIMEOUT,
                SINGLE_TANK_PROCESS_STOPPED_BY_FAILURE). 
 Returns:   	Single Tank's "Process Stopped" flag value.
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
SingleTankProcessStopped CTankDeliveryEmulationServerInfo::GetDeliveryEmulationProcessStopped()
{
	return m_lDeliveryEmulationSingleTankProcessStopped;
}

/******************************************************************************
 Description:	Set Single Tank's new lock status.  
 Returns:   	None
 Parameters:    lTankNumber, byVal  - new lock status
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CTankDeliveryEmulationServerInfo::SetLockStatus(const SingleTankLockStatus lVal)
{
	EnterCriticalSection(&m_csTankDeliveryEmulationServerInfo);

	m_lSingleTankLockStatus = lVal;

	LeaveCriticalSection(&m_csTankDeliveryEmulationServerInfo);

}

/******************************************************************************
 Description:	Get Single Tank's new lock status.  
 Returns:   	New lock status.
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
SingleTankLockStatus CTankDeliveryEmulationServerInfo::GetLockStatus()
{
	return m_lSingleTankLockStatus;
}

//Init functions:

/******************************************************************************
 Description:	Reset single tank flags of a certain tank (Delivery emulation and lock).
 Returns:       None.	
 Parameters:   	Tank Number.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Shelly B		24/11/2008   15:30		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CTankDeliveryEmulationServerInfo::InitFlags()
{
	EnterCriticalSection(&m_csTankDeliveryEmulationServerInfo);

	SetDeliveryEmulationStatus(DES_SINGLE_TANK_IDLE);
	SetDeliveryEmulationProcessType(PD_SINGLE_TANK_NONE);
    SetDeliveryEmulationSuccess(DESF_SINGLE_TANK_NOT_DONE);
	SetDeliveryEmulationProcessStopped(SINGLE_TANK_PROCESS_NOT_STOPPED);
	SetLockStatus(SINGLE_TANK_UNLOCKED);
	
	LeaveCriticalSection(&m_csTankDeliveryEmulationServerInfo);

}

