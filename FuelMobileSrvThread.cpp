#include "stdafx.h"
#include "FuelMobileSrvThread.h"


CFuelMobileSrvThread::CFuelMobileSrvThread()
{
}


CFuelMobileSrvThread::~CFuelMobileSrvThread()
{
}

void CFuelMobileSrvThread::ThreadRoutine(void * pData /*= NULL*/)
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	eFuelMobileSrvStatus enumFmStatus;

	enumFmStatus = _Module.m_server.CheckFuelMobileSrv();


	return;
}

CThreadRetCode CFuelMobileSrvThread::StartThread()
{
	CThreadRetCode eThreadRetCode = THREAD_OK;


	if (m_bActive)
		eThreadRetCode = THREAD_ALREADY_RUNNING;

	if (!eThreadRetCode)
	{
		if (m_bCreated && !m_bActive)
		{
			if ((eThreadRetCode = CThread::StartThread()) == THREAD_OK)
			{
				m_bActive = TRUE;
				_Module.m_server.m_cLogger.LogMsg("FuelMobileSrvThread thread started successfully.");
			}
			else
			{
				_Module.m_server.m_cLogger.LogMsg("Failed to start FuelMobileSrvThread thread.");
			}
		}
		else
			eThreadRetCode = THREAD_NOT_CREATED;
	}

	return eThreadRetCode;
}

CThreadRetCode CFuelMobileSrvThread::Init()
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	eThreadRetCode = CreateThread();
	if (eThreadRetCode == THREAD_OK)
	{
		m_bCreated = TRUE;
		_Module.m_server.m_cLogger.LogMsg("FuelMobileSrv Thread - thread was created.");

	
		SetSleepDuration(SLEEP_DURATION);
	}
	else
	{
		_Module.m_server.m_cLogger.LogMsg("FuelMobileSrv Thread - thread wasn't created.");
		eThreadRetCode = THREAD_NOT_CREATED;
	}

	return eThreadRetCode;
}

CThreadRetCode CFuelMobileSrvThread::CleanUp()
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	
	eThreadRetCode = KillThread();
	if ((THREAD_OK == eThreadRetCode) || (eThreadRetCode == THREAD_TERMINATED_SUCCESSFULLY)) //4.0.24.60
	{
		m_bActive = FALSE;
		m_bCreated = FALSE;
		_Module.m_server.m_cLogger.LogMsg("FuelMobileSrv thread terminated successfully.");
	}
	else
	{
		_Module.m_server.m_cLogger.LogMsg("FuelMobileSrv thread not terminated successfully.");
		eThreadRetCode = THREAD_FAILED_TO_TERMINATE;
	}


	return(eThreadRetCode);
}
