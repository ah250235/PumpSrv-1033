// PumpLock.cpp: implementation of the CPumpLock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PumpLockByTank.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPumpLockByTank::CPumpLockByTank()
{
	m_lPumpLockStatus = PTL_UNLOCK;
}

CPumpLockByTank::~CPumpLockByTank()
{

}

long CPumpLockByTank::GetPumpLockStatus()
{
	return m_lPumpLockStatus;
}

void CPumpLockByTank::SetPumpLockStatus(LockPumpByTankStatus eLockPumpByTankStatus)
{
	m_lPumpLockStatus = eLockPumpByTankStatus;
}

BOOL CPumpLockByTank::IsLockedByTank()
{
	return (m_lPumpLockStatus != PTL_UNLOCK);
}

BOOL CPumpLockByTank::IsLocked()
{
	return (m_lPumpLockStatus == PTL_LOCKED);

}

BOOL CPumpLockByTank::IsWaitForLock()
{
	BOOL bIsWaitLocked = FALSE;

	if(m_lPumpLockStatus == PTL_START_LOCK || m_lPumpLockStatus == PTL_WAIT_LOCK)
		bIsWaitLocked = TRUE;

	return bIsWaitLocked;
}