// QDXMaintThread.cpp: implementation of the CQDXMaintThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QDXMaintThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQDXMaintThread::CQDXMaintThread():m_lTimeout(100) // : CThread(FALSE , 1000 , TRUE) 
{
	m_bCreated = FALSE;
	m_bActive = FALSE;
	m_lFileNum = 0;
	m_bUploadOnly = FALSE;
}

CQDXMaintThread::~CQDXMaintThread()
{

}

/******************************************************************************
 Description:	Call the Files backup manager main procedure. 	
 Returns:      	void
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			27/08/2007   15:00		Start
 AmitH          19/11/2007              Cont. - Add logs
******************************************************************************/
void CQDXMaintThread::ThreadRoutine(void * pData)
{
	CThreadRetCode eThreadRetCode = THREAD_OK;
	CTime cStartTime;
	CTimeSpan ts;
	CTime cCurTime;
	_Module.m_server.m_cServerInfo.SetRequiredQDXMaintenance(FALSE);

	// The moment the QDX maintenance is finished stop the thread
	eThreadRetCode = StopThread();
	if(eThreadRetCode != THREAD_OK)
	{
		CString sStr;
		sStr.Format("*** CQDXMaintThread::ThreadRoutine - Error in stopping the QDX maintenance thread. Return Code: %d", eThreadRetCode);
		_LOGMSG.LogMsg(sStr,LOG_LEVEL_3);						
	}
	else
	{
		m_bActive = FALSE;
		_LOGMSG.LogMsg("*** QDXMaint - QDX maintenance thread stopped successfully");//4.0.18.22 //4.0.18.40
	}
		
	
	return;
}


CThreadRetCode CQDXMaintThread::Init()
{
	CThreadRetCode eThreadRetCode = THREAD_OK; 

	eThreadRetCode = CreateThread();
	if( eThreadRetCode == THREAD_OK )
	{
		m_bCreated = TRUE;
		_LOGMSG.LogMsg("QDXMaint - QDX maintenance thread was created"); //4.0.18.40
	}
	else
	{
		_LOGMSG.LogMsg("QDXMaint - QDX maintenance thread wasn't created"); //4.0.18.40
		eThreadRetCode = THREAD_NOT_CREATED;
	}

	return eThreadRetCode;
}

CThreadRetCode CQDXMaintThread::StartMaintAll(BOOL bUploadOnly)
{
//4.0.18.30
	CTime cStartTime = CTime::GetCurrentTime();
	CTimeSpan ts;
	ts = CTime::GetCurrentTime() - cStartTime;
	//If we are in the middle of reorganize() or DoAutomaticReceiptCheck() postpone maintenace process.
	while ((ts.GetTotalSeconds() < m_lTimeout) && (_Module.m_server.GetIsInTheMiddleOfDoAutomaticReceiptCheck()))
	{
		Sleep(m_lTimeout);
		ts = CTime::GetCurrentTime() - cStartTime;
	}
	return THREAD_OK;

}


CThreadRetCode CQDXMaintThread::StartQDXMaintThread(long lFileNum, BOOL bUploadOnly)
{
	CThreadRetCode eThreadRetCode = THREAD_OK; 

	m_lFileNum = lFileNum;
	m_bUploadOnly = bUploadOnly;
	
	if (m_bActive)
		eThreadRetCode = THREAD_ALREADY_RUNNING;

	if (!eThreadRetCode)
	{
		if (m_bCreated && !m_bActive)
		{
			if ((eThreadRetCode = StartThread()) == THREAD_OK)
			{
				m_bActive = TRUE;
				_LOGMSG.LogMsg("*** QDX maintenance thread started successfully");
			}
			else
			{
				_LOGMSG.LogMsg("*** Failed to start QDX maintenance thread", LOG_LEVEL_1);
			}
		}
		else
			eThreadRetCode = THREAD_NOT_CREATED;
	}

	return eThreadRetCode;
}

CThreadRetCode CQDXMaintThread::CleanUp()
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	//Killing the QDX maintenance thread
	eThreadRetCode = KillThread();
	if(THREAD_OK == eThreadRetCode)
	{
		m_bActive = FALSE;
		m_bCreated = FALSE;
		_LOGMSG.LogMsg("QDX maintenance thread terminated successfully");
	}
	else
	{
		_LOGMSG.LogMsg("QDX maintenance thread not terminated successfully", LOG_LEVEL_1);
		eThreadRetCode = THREAD_FAILED_TO_TERMINATE;
	}
		

	return(eThreadRetCode);

}
