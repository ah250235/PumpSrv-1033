// ShiftProcess.cpp: implementation of the CShiftProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShiftProcess.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShiftProcess::CShiftProcess():m_bClearStuckTrs(FALSE)
{

}

CShiftProcess::~CShiftProcess()
{

}

void CShiftProcess::CheckStuckTransaction()
{
	if (GetFlagClearStuckTrs())
	{
		_LOGMSG.LogMsg("ShiftProcess","CheckStuckTransaction",0,LOG_NONE,"Search for stuck transaction");
		for (int p=0;p < _Module.m_server.GetLastValidPump();p++)  //4.0.8.500
		{
			_Module.m_server.m_cPumpArray[p].RemovedStuckTransaction();
		}
	}
}
/******************************************************************************
 Description:	return status of EOD flags to clear stuck trs. 	
 Returns:      	BOOL bVal - TRUE/FALSE
 Parameters:    None. 	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/1/2005   11:21		Start
******************************************************************************/
BOOL CShiftProcess::GetFlagClearStuckTrs()
{
	return m_bClearStuckTrs;
}
/******************************************************************************
 Description:	Update EOD flags to clear stuck trs. 	
 Returns:      	
 Parameters:   	BOOL bVal - TRUE/FALSE
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/1/2005   11:21		Start
******************************************************************************/
void CShiftProcess::SetFlagClearStuckTrs(BOOL bVal)
{
	CString sMsg;
	m_bClearStuckTrs = bVal;
	
	if(LOG_BASIC_FLOW_CONTROL)
	{
		if (bVal)
			sMsg.Format("Set process clear stuck transaction on shift - Active");
		else
			sMsg.Format("Set process clear stuck transaction on shift - no Active");
		
		_LOGMSG.LogMsg("ShiftProcess","SetFlagClearStuckTrs",0,LOG_NONE,sMsg);
	}
	
}
