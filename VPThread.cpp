// VPThread.cpp: implementation of the CVPThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VPThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVPThread::CVPThread()
{

}

CVPThread::~CVPThread()
{

}



CThreadRetCode CVPThread::Init()
{
	CThreadRetCode eThreadRetCode = THREAD_OK; 

	eThreadRetCode = CreateThread();
	if( eThreadRetCode == THREAD_OK )
	{
		m_bCreated = TRUE;
		_LOGMSG.LogMsg("VPThread - thread was created"); 
        InitVpRecoveryCounter();
	
		CString sLogMsg;
		sLogMsg.Format("VP recovery - attempt %ld of %ld", m_lVpRecoveryCounter, m_lVpRecoveryMaxRetries);
		SetSleepDuration(SLEEP_DURATION);
	}
	else
	{
		_LOGMSG.LogMsg("VPThread - thread wasn't created"); 
		eThreadRetCode = THREAD_NOT_CREATED;
	}

	return eThreadRetCode;
}

void CVPThread::InitVpRecoveryCounter()
{
	m_lVpRecoveryMaxRetries = _Module.m_server.GetVpRecoveryMaxRetries();
	m_lVpRecoveryCounter = 1;
	if (m_lVpRecoveryMaxRetries > 0)
	{
		CString sLogMsg;
		sLogMsg.Format("VPThread - max VP recovery retries: %ld", m_lVpRecoveryMaxRetries);
		_LOGMSG.LogMsg(sLogMsg);
	}
}
CThreadRetCode CVPThread::StartVPThread()
{
	CThreadRetCode eThreadRetCode = THREAD_OK; 

	
	if (m_bActive)
		eThreadRetCode = THREAD_ALREADY_RUNNING;

	if (!eThreadRetCode)
	{
		if (m_bCreated && !m_bActive)
		{
			if ((eThreadRetCode = StartThread()) == THREAD_OK)
			{
				m_bActive = TRUE;
				_LOGMSG.LogMsg("VP thread started successfully");
			}
			else
			{
				_LOGMSG.LogMsg("Failed to start VP thread");
			}
		}
		else
			eThreadRetCode = THREAD_NOT_CREATED;
	}

	return eThreadRetCode;
}

CThreadRetCode CVPThread::CleanUp()
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	//Killing the VP thread
	eThreadRetCode = KillThread();
	if((THREAD_OK == eThreadRetCode) || (eThreadRetCode == THREAD_TERMINATED_SUCCESSFULLY)) //4.0.24.60
	{
		m_bActive = FALSE;
		m_bCreated = FALSE;
		_LOGMSG.LogMsg("VP thread terminated successfully");
	}
	else
	{
		_LOGMSG.LogMsg("VP thread not terminated successfully");
		eThreadRetCode = THREAD_FAILED_TO_TERMINATE;
	}
		

	return(eThreadRetCode);

}


/******************************************************************************
 Description:	Call the Files backup manager main procedure. 	
 Returns:      	void
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			14/02/2010   11:20		Start
******************************************************************************/
void CVPThread::ThreadRoutine(void * pData)
{
	CThreadRetCode eThreadRetCode = THREAD_OK;
    VpStatus enumVpStatus;

	enumVpStatus =  _Module.m_server.ConnectToVp();

	if (enumVpStatus == VP_OK) 
    {
		ResetVpRecoveryCounter(); 
	} 
	else
	{
		TryToVpRecovery();

	}

	return;
}

void CVPThread::TryToVpRecovery()
{
	_Module.m_server.DisconnectFromVp();
	if (m_lVpRecoveryMaxRetries > 0)
	{
		CheckVpRecoveryState();
	}
	
}


void  CVPThread::CheckVpRecoveryState()
{
	BOOL bDoReConnect = FALSE;
	CString sLogMsg;


	sLogMsg.Format("VPThread - VP recovery attempt %ld of %ld", m_lVpRecoveryCounter, m_lVpRecoveryMaxRetries);
	_LOGMSG.LogMsg(sLogMsg);


	if (IsVpFailedRecovery() == TRUE)
	{
		_Module.m_server.SetVpIsGone(TRUE);

		_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE,SUB_PUMPSRV,ALARM_VP_IS_DOWN,0,ALARM_ON,0,0);
		sLogMsg.Format("VPThread - reached max recovery retry %ld",m_lVpRecoveryMaxRetries);
		_LOGMSG.LogMsg(sLogMsg);

	}
	else
	{
		m_lVpRecoveryCounter++;

	}
}



void CVPThread::ResetVpRecoveryCounter()
{
	m_lVpRecoveryCounter = 1;
}


BOOL CVPThread::IsVpFailedRecovery()
{
	bool bRetVal = (m_lVpRecoveryCounter >= m_lVpRecoveryMaxRetries);
	return(bRetVal);
}
