// PumpLock.h: interface for the CPumpLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUMPLOCKBYTANK_H__2C481BED_775E_4948_AACB_21225BEB125C__INCLUDED_)
#define AFX_PUMPLOCKBYTANK_H__2C481BED_775E_4948_AACB_21225BEB125C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_STATUS_RETRY	5

class CPumpLockByTank  
{
public:
	BOOL IsWaitForLock();
	BOOL IsLocked();
	BOOL IsLockedByTank();
	void SetPumpLockStatus(LockPumpByTankStatus eLockPumpByModeStatus);
	long GetPumpLockStatus();
	CPumpLockByTank();
	virtual ~CPumpLockByTank();

private:
	long m_lPumpLockStatus;
};

#endif // !defined(AFX_PUMPLOCKBYTANK_H__2C481BED_775E_4948_AACB_21225BEB125C__INCLUDED_)
