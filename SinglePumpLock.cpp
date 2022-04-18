
#include "stdafx.h"
#include "SinglePumpLock.h"

/******************************************************************************
 Description:	 Constructing a CSinglePumpLock object	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			28/01/2004   12:37		Start
******************************************************************************/

CSinglePumpLock::CSinglePumpLock()
{
	//Initializing all pump lock statuses with the value of PUMP_EXCLUSIVELY_UNLOCKED - 
	//assuming they're all unlocked at first
	memset(m_lPumpLockStateArray, PUMP_EXCLUSIVELY_UNLOCKED, sizeof(SinglePumpLockState) * MAX_PUMPS_64);
}

/******************************************************************************
 Description:	 Setting a SinglePump state (handling the Single Pump state machine)	
 Returns:      	
 Parameters:     long lPumpNumber, long lStatus, long lRes, long &lResExt	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			29/01/2004   12:37		Start
******************************************************************************/
  
void CSinglePumpLock::SetSinglePumpState(long lPumpNumber, long lStatus, long lRes, long &lResExt)
{
	//Indicates if the given pump is currently locked
	BOOL bCurrentlyLocked = FALSE;
	//Indicates if the pump state should be changed 
	BOOL bChangeState = TRUE;
		
	CString sStringStateBeforeChange, sStringStateAfterChange; 
	sStringStateBeforeChange = GetCurrentStateString(lPumpNumber); 

	//Only if all 2 following conditions are met is the pump locked. Otherwise it's not
	if( (READY == lStatus || REQUESTING == lStatus) && (PUMPSRV == lRes ) )
		bCurrentlyLocked = TRUE;

	//If the given pump lock status is either PUMP_EXCLUSIVELY_LOCKED or PUMP_EXCLUSIVELY_UNLOCKED
	//it means we haven't requested to lock or unlock a pump - which means our machine state has nothing to do
	if((PUMP_EXCLUSIVELY_LOCKED == m_lPumpLockStateArray[lPumpNumber - 1]) || (PUMP_EXCLUSIVELY_UNLOCKED == m_lPumpLockStateArray[lPumpNumber - 1]) )
		bChangeState = FALSE;
	
	if(bChangeState)
	{
		//Logging the state before the change
		//We have just been locked now so our state will be changed to PUMP_EXCLUSIVELY_LOCKED
		if (bCurrentlyLocked && PUMP_WAIT_TO_BE_EXCLUSIVELY_LOCKED == m_lPumpLockStateArray[lPumpNumber - 1])
		{
			m_lPumpLockStateArray[lPumpNumber - 1]  = PUMP_EXCLUSIVELY_LOCKED;
			lResExt = RES_EXT_PUMP_SINGLE_LOCK;
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetLockStatus(LOCKED);
			_Module.m_server.SavePumpInfo(lPumpNumber);		
		}
		//We have just been unlocked now so our state will be changed to PUMP_EXCLUSIVELY_UNLOCKED
		else if (!bCurrentlyLocked && PUMP_WAIT_TO_BE_EXCLUSIVELY_UNLOCKED == m_lPumpLockStateArray[lPumpNumber - 1])
		{
			m_lPumpLockStateArray[lPumpNumber - 1]  = PUMP_EXCLUSIVELY_UNLOCKED;
			lResExt = NULL_RES_EXT;						
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetLockStatus(UNLOCK);
			_Module.m_server.SavePumpInfo(lPumpNumber);		
		}

		//Logging the state after the change
		sStringStateAfterChange = GetCurrentStateString(lPumpNumber); 
	    CreateLogString(lPumpNumber, sStringStateBeforeChange, sStringStateAfterChange);
	}
}

/******************************************************************************
 Description:	 Setting the PUMP_WAIT_TO_BE_EXCLUSIVELY_LOCKED state to a pump
 Returns:      	
 Parameters:     long lPumpNumber	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			29/01/2004   12:37		Start
******************************************************************************/

void CSinglePumpLock::SetPumpStateWaitLocked(long lPumpNumber)
{
	//CStrings for writing logs 
	CString sStringStateBeforeChange, sStringStateAfterChange; 
	sStringStateBeforeChange = GetCurrentStateString(lPumpNumber); 

	//Changing the state to PUMP_WAIT_TO_BE_EXCLUSIVELY_LOCKED
	m_lPumpLockStateArray[lPumpNumber - 1] = PUMP_WAIT_TO_BE_EXCLUSIVELY_LOCKED;

	sStringStateAfterChange = GetCurrentStateString(lPumpNumber); 
	CreateLogString(lPumpNumber, sStringStateBeforeChange, sStringStateAfterChange);
}

/******************************************************************************
 Description:	 Setting the PUMP_WAIT_TO_BE_EXCLUSIVELY_UNLOCKED state to a pump
 Returns:      	
 Parameters:     long lPumpNumber	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			29/01/2004   12:38		Start
******************************************************************************/

void CSinglePumpLock::SetPumpStateWaitUnlocked(long lPumpNumber)
{
	//CStrings for writing logs 
	CString sStringStateBeforeChange, sStringStateAfterChange; 
	sStringStateBeforeChange = GetCurrentStateString(lPumpNumber); 

	//Changing the state to PUMP_WAIT_TO_BE_EXCLUSIVELY_UNLOCKED
	m_lPumpLockStateArray[lPumpNumber - 1] = PUMP_WAIT_TO_BE_EXCLUSIVELY_UNLOCKED;

    sStringStateAfterChange = GetCurrentStateString(lPumpNumber);

	CreateLogString(lPumpNumber, sStringStateBeforeChange, sStringStateAfterChange);
}

/******************************************************************************
 Description:	Getting the string which describes a pump's current LockSingle Pump state
 Returns:      	CString
 Parameters:    long lPumpNumber	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			29/01/2004   12:38		Start
******************************************************************************/
CString CSinglePumpLock::GetCurrentStateString(long lPumpNumber)
{
	//The string which will be returned by the function
	CString sRes;
	//Getting the appropriate string for logging purposes 
	switch(m_lPumpLockStateArray[lPumpNumber - 1])
	{
		case PUMP_EXCLUSIVELY_UNLOCKED:
			sRes = "Pump Exclusively Unlocked "; 
			break;
	    case PUMP_WAIT_TO_BE_EXCLUSIVELY_UNLOCKED:
			sRes = "Pump waits to be Exclusively Unlocked "; 
			break;
		case PUMP_EXCLUSIVELY_LOCKED:
			sRes = "Pump Exclusively Locked "; 
			break;
		case PUMP_WAIT_TO_BE_EXCLUSIVELY_LOCKED:
			sRes = "Pump waits to be Exclusively Locked "; 
			break;
		default:
			sRes = "Unsupported state";
	}
	return sRes;
}

/******************************************************************************
 Description:	Creating the string which describes a pump's statuses before the 
                change was executed and and afterwards 
 Returns:      	CString
 Parameters:    long lPumpNumber	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			29/01/2004   12:38		Start
******************************************************************************/

void CSinglePumpLock::CreateLogString(long lPumpNumber, CString sStateBeforeChange, CString sStateAfterChange)
{
	CString sLogString;
	sLogString.Format("Changing SinglePumpState for pump %d. State Before Change: %s. State after change: %s", lPumpNumber, sStateBeforeChange, sStateAfterChange );
	_LOGMSG.LogMsg(sLogString);
}

/******************************************************************************
 Description:	Determining if a pump is exclusively locked
 Returns:      	BOOL
 Parameters:    long lPumpNumber	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			29/01/2004   16:31		Start
******************************************************************************/
BOOL CSinglePumpLock::IsExclusivelyLocked (long lPumpNumber)
{
	BOOL bRetCode;
	if( PUMP_EXCLUSIVELY_LOCKED == m_lPumpLockStateArray[lPumpNumber - 1])
		bRetCode =  TRUE;
	else
		bRetCode = FALSE;

	return bRetCode;
}

/******************************************************************************
 Description:	Setting the PUMP_EXCLUSIVELY_LOCKED state for pumps which
                are initially locked
 Returns:      	
 Parameters:    	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			03/03/2004   11:14		Start
******************************************************************************/
void CSinglePumpLock::SetInitiallyLockedPumps(long lPumpNumber)
{
	CPumpStatus cPumpStatus;

	memset(&cPumpStatus, 0 , sizeof(CPumpStatus) );

	//4.0.7.502
	if(lPumpNumber > 0 && lPumpNumber < MAX_PUMPS_64)
	{
		cPumpStatus = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();
		if( ( cPumpStatus.GetRes() == PUMPSRV ) && (cPumpStatus.GetResExt() == RES_EXT_PUMP_SINGLE_LOCK ) ) 
		{
			m_lPumpLockStateArray[lPumpNumber-1]  = PUMP_EXCLUSIVELY_LOCKED;
		}
		else
			m_lPumpLockStateArray[lPumpNumber-1]  = PUMP_EXCLUSIVELY_UNLOCKED;
	}
	else
	{
		//Looping over all pumps and setting the state of an exclusively locked pump
		//to PUMP_EXCLUSIVELY_LOCKED. Else it remains as it was initialized - PUMP_EXCLUSIVELY_UNLOCKED 
		for(long lLoopCounter = 0; lLoopCounter < _Module.m_server.m_cPumpArray.GetSize(); lLoopCounter++ ) //4.0.22.503
		{
	 		cPumpStatus = _Module.m_server.m_cPumpArray[lLoopCounter].GetStat();
			if( ( cPumpStatus.GetRes() == PUMPSRV ) && (cPumpStatus.GetResExt() == RES_EXT_PUMP_SINGLE_LOCK ) ) 
			{
				m_lPumpLockStateArray[lLoopCounter]  = PUMP_EXCLUSIVELY_LOCKED;
			} 
		}
	}
}



