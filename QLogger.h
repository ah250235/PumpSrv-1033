/*
In order to ensure that messages are written to the log, 
and that the original order of the messages remains intact, 
we store each message in a queue.  
When a class derived from CQLogger calls CQLogger::WriteToLog, 
the message is appended at the end of a message buffer.
	
Every once in a while, CQLogger::ThreadRoutine is invoked by the CThread class 
the CQLogger has been derived from.  This method checks the size of the message 
buffer.  If the message buffer is not empty, the log file CQLogger is responsible 
for is opened.  The messages in the buffer are flushed to the disk, after which 
the file handle is closed.

If the attempt to write to the disk fails, we attempt as many as MAX_WRITE_ATTEMPTS 
before writing the message to a default log file.  The handle to the log file is 
closed only after a successful write, or if MAX_WRITE_ATTEMPTS has been reached.

If the queue is empty, the thread is stopped for improved performance 
(no unnecessary resources wasted on polling).  
The first message stored in the outgoing message buffer restarts the thread.  
Once the buffer has been cleared, the thread stops again.

*/

// QLogger.h: interface for the CQLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QLOGGER_H__1FEDE91D_8B7B_4F40_94B1_01285734CD78__INCLUDED_)
#define AFX_QLOGGER_H__1FEDE91D_8B7B_4F40_94B1_01285734CD78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFAULT_MAX_MESSAGE_SIZE			256
#define DEFAULT_MAX_MESSAGE_COUNT			100
#define SLEEP_INTERVAL_BETWEEN_ATTEMPTS		10
#define MAX_WRITE_ATTEMPTS					5

#include "Thread.h"
#include "Log.h"

class CQLogger : public CThread , public CLog  
{
public:
	CQLogger(long lMaxMessageSize = DEFAULT_MAX_MESSAGE_SIZE , long lMaxMessageCount = DEFAULT_MAX_MESSAGE_COUNT , long lSleepDuration = DEFAULT_SLEEP_DURATION);
	virtual ~CQLogger();

	//Every derived class MUST implement this method in order to be able
	//to log catastrophic failures to the applications main log,
	//if such a log exists besides the log derived from this class.  
	//NEVER attempt writing to the same log that originated the 
	//catstrophic failure.
	virtual long LogCatastrophicFailure() = 0;

	virtual long WriteLogFile(DWORD dwWrittenToBuff , long * pRetVal = NULL, long lDiskWarning = 0);
	
protected:
	virtual void ThreadRoutine(void * pData = NULL);

	virtual HANDLE OpenLogFile();
	virtual HANDLE WriteToDefaultLogFile();

	void Allocate();
	void Cleanup();

	//Members
	long m_lCurrentOffset , m_lMaxMessageSize , m_lMaxMessageCount;

private:
	void GetDefaultLogConfig();

	BOOL WithinBufferBounds(long lMessageLength);

	long AddMessage(char * sMessage , long lMessageLength);
	void ClearMessageBuffer();

	BOOL WriteToDisk();

	//Members
	CString m_sBuffer;
	char m_sDefaultLogPath[MAX_PATH];
};

#endif // !defined(AFX_QLOGGER_H__1FEDE91D_8B7B_4F40_94B1_01285734CD78__INCLUDED_)
