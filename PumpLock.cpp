// PumpLock.cpp: implementation of the CPumpLock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PumpLock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPumpLock::CPumpLock()
{
	m_lPumpLockStatus = PM_UNLOCK;
	m_lRetryCounter = 0;
}

CPumpLock::~CPumpLock()
{

}

long CPumpLock::GetPumpLockStatus()
{
	return m_lPumpLockStatus;
}

void CPumpLock::SetPumpLockStatus(LockPumpByModeStatus eLockPumpByModeStatus)
{
	m_lPumpLockStatus = eLockPumpByModeStatus;
	//every state change restart the counter
	m_lRetryCounter = 0;
}

BOOL CPumpLock::IsLockedByMode()
{
	BOOL bLockedByMode = FALSE;

	if(m_lPumpLockStatus != PM_UNLOCK)
		bLockedByMode = TRUE;

	return bLockedByMode;
}

BOOL CPumpLock::IsLocked()
{
	BOOL bIsLocked = FALSE;

	if(m_lPumpLockStatus == PM_LOCKED)
		bIsLocked = TRUE;

	return bIsLocked;
}

BOOL CPumpLock::IsWaitForLock()
{
	BOOL bIsWaitLocked = FALSE;

	if(m_lPumpLockStatus == PM_START_LOCK || m_lPumpLockStatus == PM_WAIT_LOCK)
		bIsWaitLocked = TRUE;

	return bIsWaitLocked;
}

void CPumpLock::IncreaseRetryCounter()
{
	if(m_lRetryCounter == MAX_STATUS_RETRY)
	{
		//here check to what status we should return
		switch(m_lPumpLockStatus)
		{
			case PM_WAIT_LOCK:
			{
				SetPumpLockStatus(PM_START_LOCK);
				break;
			}
			case PM_WAIT_UNLOCK:
			{
				SetPumpLockStatus(PM_START_UNLOCK);
				break;
			}
		}
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString tmpStr;
			tmpStr.Format("CPumpLock::IncreaseRetryCounter:reached max retry, New Status:%d",m_lPumpLockStatus);
			_LOGMSG.LogMsg(tmpStr);
		}
	}
	else
	{
		m_lRetryCounter++;
	}
}
