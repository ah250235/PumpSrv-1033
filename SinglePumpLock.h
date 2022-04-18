// AlarmData.h: interface for the AlarmData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLE_PUMP_LOCK_H__3D4A3688_B27E_11D3_BB71_00400430AD7E__INCLUDED_)
#define AFX_SINGLE_PUMP_LOCK_H__3D4A3688_B27E_11D3_BB71_00400430AD7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Pump.h"

//An enumeration of the SinglePump possible LockStates 
enum SinglePumpLockState //4.0.5.24
{
	PUMP_EXCLUSIVELY_UNLOCKED = 0,
	PUMP_WAIT_TO_BE_EXCLUSIVELY_UNLOCKED  = 1,
	PUMP_EXCLUSIVELY_LOCKED = 2, 
	PUMP_WAIT_TO_BE_EXCLUSIVELY_LOCKED = 3 
};

class CSinglePumpLock   //4.0.5.24
{
public:
	CSinglePumpLock();
	//Setting the pump single state including taking care of the State Machine
	//relevant to the pump lock state 
    void SetSinglePumpState(long lPumpNumber, long lStatus, long lRes, long &lResExt);

	//Locking a single pump
	void SetPumpStateWaitLocked(long lPumpNumber);
	//Unlocking a single pump
	void SetPumpStateWaitUnlocked(long lPumpNumber);
	//Checking if a pump is exclusively locked
	BOOL IsExclusivelyLocked (long lPumpNumber);
	//Initially setting the state machine of the exclusively locked pumps
	void SetInitiallyLockedPumps(long lPumpNumber); //4.0.5.27

protected:
	//Returning the string describing the current state
	CString GetCurrentStateString(long lPumpNumber);

	//Writing the string which describes the StateMachine states changing to the log
    void CreateLogString(long lPumpNumber, CString sStateBeforeChange, CString sStateAfterChange);

private:
	//The following array will hold the lock status of each pump in the system 
	SinglePumpLockState m_lPumpLockStateArray[MAX_PUMPS_64];

};

#endif // !defined(AFX_SINGLE_PUMP_LOCK_H__3D4A3688_B27E_11D3_BB71_00400430AD7E__INCLUDED_)

