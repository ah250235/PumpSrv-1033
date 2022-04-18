// TerminalMessages.h: interface for the CTerminalMessages class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERMINALMESSAGES_H__F215698C_8930_409A_91DB_ACC0E0CE5775__INCLUDED_)
#define AFX_TERMINALMESSAGES_H__F215698C_8930_409A_91DB_ACC0E0CE5775__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Pump.h"

#define STATUS_OK           0
#define STATUS_NO_MUTEX     1
#define STATUS_ALLOC_FAILED 2

class CTerminalMessages  
{
public:
	void ClearMessages(long lPumpNumber);
	BOOL GetMessage(long lPumpNumber , char * sMessage , int iMsgLen);
	BOOL AddMessage(BSTR & bstrMessage , long lPumpNumber);

	CTerminalMessages();
	virtual ~CTerminalMessages();

private:
	BOOL ReleaseMutex();
	BOOL AcquireMutex(BOOL bWithWait = FALSE);
	short m_nStatus;
	HANDLE m_hMutex;
	char m_sTerminalMessageArray[MAX_TERMINALS][MAX_TERMINAL_MESSAGE_LENGTH];
};

#endif // !defined(AFX_TERMINALMESSAGES_H__F215698C_8930_409A_91DB_ACC0E0CE5775__INCLUDED_)
