// InitDebitHandler.cpp: implementation of the CInitDebitHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InitDebitHandler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInitDebitHandler::CInitDebitHandler()
{

}

CInitDebitHandler::~CInitDebitHandler()
{
	memset(m_byDebitInitArray , 0x00 , sizeof(m_byDebitInitArray));
}
/******************************************************************************
 Description:	Get Debit status for required pump 	
 Returns:      	DebitSrvStatus - Pump status
 Parameters:   	long lPumpNumber - required Pump
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			7/10/2003   13:34		Start
******************************************************************************/
DebitSrvStatus CInitDebitHandler::GetStatus(long lPumpNumber)
{	
	long p = lPumpNumber -1;

	return m_byDebitInitArray[p];

}

/******************************************************************************
 Description:	Set Debit status to specail pump
 Returns:      	None.
 Parameters:   	long lPumpNumber - Pump number
				eNewStatus       - new incoming status


 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			7/10/2003   13:34		Start
******************************************************************************/
void CInitDebitHandler::SetStatus(long  lPumpNumber, DebitSrvStatus eNewStatus)
{
	long p = lPumpNumber -1;

	m_byDebitInitArray[p] = eNewStatus;

	if (_Module.m_server.UseSingleSession() && (eNewStatus == DEBIT_SERVER_IDLE))
		FindNextPump(lPumpNumber);
}

long CInitDebitHandler::GetWorkingPump()
{
	return m_lWorkingPump;	
}

void CInitDebitHandler::SetWorkingPump(long lPumpNumber)
{
	m_lWorkingPump  = lPumpNumber;
}

long CInitDebitHandler::FindNextPump(long &lPumpNumber)
{
	BOOL bFound = FALSE;
	m_lWorkingPump = 0; 


	for(long i=lPumpNumber + 1 ; i <= _Module.m_server.GetMaxPumps(); i++)
	{
		if (GetStatus(i) == DEBIT_SERVER_REQUEST_TO_START)
		{
			CString sMsg;

			sMsg.Format("SetWorkingPump from = (%ld) to (%ld)",lPumpNumber , i);
			_LOGMSG.LogMsg(sMsg);
			//_Module.m_server.m_cInitDebitHandler.SetStatus(i, DEBIT_SERVER_INPROCESS); //4.0.3.58
			bFound = TRUE;
			m_lWorkingPump = 0;
			break;
		}			
	}

	if (!bFound)
	{
		for(long i = 1 ; i < lPumpNumber +1; i++)
		{
			if (GetStatus(i) == DEBIT_SERVER_REQUEST_TO_START)
			{
				bFound = TRUE;
				m_lWorkingPump = 0;
				break;
			}				
		}
	}

	return 0;
}
