// TerminalMessages.cpp: implementation of the CTerminalMessages class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TerminalMessages.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTerminalMessages::CTerminalMessages() : m_nStatus(STATUS_OK)
{
	memset((char*)m_sTerminalMessageArray , ' ' , sizeof(m_sTerminalMessageArray));

	m_hMutex = CreateMutex(NULL,
		                   FALSE,
						   NULL);

	if(!m_hMutex)
		m_nStatus = STATUS_NO_MUTEX;
}

CTerminalMessages::~CTerminalMessages()
{
	CloseHandle(m_hMutex);
}

BOOL CTerminalMessages::AddMessage(BSTR & bstrMessage, long lPumpNumber)
{
	int iRetVal = 0;

	BOOL bRetVal = FALSE;

	DWORD dwLastSystemError = 0;
	CString sLogMsg;

	long lPumpIndex = lPumpNumber - 1;

	if(AcquireMutex(TRUE))
	{
		//Clear buffer
		memset((char*)m_sTerminalMessageArray[lPumpIndex] , ' '  , MAX_TERMINAL_MESSAGE_LENGTH);
		
		iRetVal = WideCharToMultiByte(CP_ACP,
									  0,
									  bstrMessage,
									  -1,
									  (char*)m_sTerminalMessageArray[lPumpIndex],
									  MAX_TERMINAL_MESSAGE_LENGTH,
									  NULL,
									  NULL);
	
		if(!iRetVal)
		{
			dwLastSystemError = GetLastError();

			//In case of ERROR_INSUFFICIENT_BUFFER, characters are still copied,
			//so we can send a message, but it may be truncated.
			if(dwLastSystemError == ERROR_INSUFFICIENT_BUFFER)
				sLogMsg.Format("CTerminalMessages::AddMessage() - The message was too long, and may have been truncated");
			
			else
				memset((char*)m_sTerminalMessageArray[lPumpIndex] , ' ' , MAX_TERMINAL_MESSAGE_LENGTH);
		}

		//Delete null terminator
		else
		{
			//sLogMsg.Format("CTerminalMessages::AddMessage() - %s" , m_sTerminalMessageArray[lPumpIndex]);
			m_sTerminalMessageArray[lPumpIndex][iRetVal - 1] = ' ';
		}

		ReleaseMutex();

		bRetVal = TRUE;
	}

	else
		sLogMsg.Format("Failed to acquire mutex, message not updated");
	
	//All actual disk writing is done outside the scope of the mutex,
	//in order to minimize the time the mutex is being held.
	if(!sLogMsg.IsEmpty())
		_LOGMSG.LogMsg(sLogMsg);

	return bRetVal;
}

BOOL CTerminalMessages::GetMessage(long lPumpNumber, char *sMessage, int iMsgLen)
{
	BOOL bRetVal = FALSE;
	int iNumBytesToCopy = min(iMsgLen , MAX_TERMINAL_MESSAGE_LENGTH);

	long lPumpIndex = lPumpNumber - 1;

	if(AcquireMutex())
	{
		memcpy(sMessage , m_sTerminalMessageArray[lPumpIndex] , iNumBytesToCopy);

		ReleaseMutex();

		bRetVal = TRUE;
	}

	else
	{
		_LOGMSG.LogMsg("Failed to acquire mutex, returning empty message");

		memset(sMessage , ' ' , iMsgLen);
	}
	
	return bRetVal;
}

BOOL CTerminalMessages::AcquireMutex(BOOL bWithWait)
{
	BOOL bRetVal = TRUE;
	DWORD dwWaitVal = 0 , dwWaitInterval = 0;

	if(m_nStatus != STATUS_NO_MUTEX)
	{
		if(bWithWait)
			dwWaitInterval = INFINITE;

		dwWaitVal = WaitForSingleObject(m_hMutex , dwWaitInterval);

		if(dwWaitVal != WAIT_OBJECT_0)
			bRetVal = FALSE;
	}

	return bRetVal;
}

BOOL CTerminalMessages::ReleaseMutex()
{
	BOOL bRetVal = TRUE;

	if(m_nStatus != STATUS_NO_MUTEX)
		bRetVal = ::ReleaseMutex(m_hMutex);

	return bRetVal;
}

//4.0.2.0 Start
void CTerminalMessages::ClearMessages(long lPumpNumber)
{
	memset((char*)m_sTerminalMessageArray[lPumpNumber - 1] , ' '  , MAX_TERMINAL_MESSAGE_LENGTH);

	return;
}
//4.0.2.0 End