// PumpLock.h: interface for the CPumpLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUMPLOCK_H__2C481BED_775E_4948_AACB_21225BEB125C__INCLUDED_)
#define AFX_PUMPLOCK_H__2C481BED_775E_4948_AACB_21225BEB125C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_STATUS_RETRY	5

class CPumpLock  
{
public:
	void IncreaseRetryCounter();
	BOOL IsWaitForLock();
	BOOL IsLocked();
	BOOL IsLockedByMode();
	void SetPumpLockStatus(LockPumpByModeStatus eLockPumpByModeStatus);
	long GetPumpLockStatus();
	CPumpLock();
	virtual ~CPumpLock();

private:
	long m_lRetryCounter;
	long m_lPumpLockStatus;
};

#endif // !defined(AFX_PUMPLOCK_H__2C481BED_775E_4948_AACB_21225BEB125C__INCLUDED_)
