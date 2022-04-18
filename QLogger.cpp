// QLogger.cpp: implementation of the CQLogger class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "CL2PumpSrv.h"  not generic, removed
#include "QLogger.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQLogger::CQLogger(long lMaxMessageSize , long lMaxMessageCount , long lSleepDuration) : m_lCurrentOffset(0),
																						 m_lMaxMessageSize(lMaxMessageSize),
																						 m_lMaxMessageCount(lMaxMessageCount),
																						 CThread(FALSE , lSleepDuration , FALSE , THREAD_PRIORITY_NORMAL)
{
	m_sBuffer.Empty();
	//Allocate();  Leave this call to the derived class
}

CQLogger::~CQLogger()
{
	KillThread();
	
	Cleanup();
}

////////////////////////////////////////////////////////////////////////////////////
//Memory allocation and class configuration                                       //
////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 Description:	Allocate memory for messages and store in m_sBuffer.
				If allocation fails, use CLog::m_sBuff.
 Returns:      	void.
 Parameters:   	none.
 Assumptions:   none.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/9/2003   14:52	 Documentation
******************************************************************************/
void CQLogger::Allocate()
{

	if (m_sBuffer.GetLength())
	{
		m_sBuffer.Empty();
	}	
	else
	{
		m_sBuffer = m_sBuff;
		m_lMaxMessageCount = 1;
	}

	return;
}

/******************************************************************************
 Description:	Get the paths needed to write a default log in case regular
				log writing fails.
 Returns:      	void.
 Parameters:   	none.
 Assumptions:   The derived class' Init has already been called.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/9/2003   14:53	 Documentation
******************************************************************************/
void CQLogger::GetDefaultLogConfig()
{
	char sTmpFileName[MAX_FILE_PATH] , sName[MAX_FILE_NAME];

	memset((char*)m_sDefaultLogPath , 0 , sizeof(m_sDefaultLogPath));
	memset((char*)sTmpFileName , 0 , sizeof(sTmpFileName));
	memset((char*)sName , 0 , sizeof(sName));

	strcpy(sName , "DefaultLog.Log");

	DWORD lRet = GetProcessName((LPSTR)sTmpFileName , MAX_FILE_PATH);
	GetProcessNameDetails((LPSTR)sTmpFileName , lRet , (LPSTR)m_sDefaultLogPath , (LPSTR)sName);
	
	strcat((char*)m_sDefaultLogPath , "\\DefaultLog\\");
	CreateDirectory((LPCSTR)m_sDefaultLogPath , NULL);

	if(strcmp((char*)m_sFileName.GetBuffer(0) , ""))
		strcpy(sName, m_sFileName.GetBuffer(0));

	strcat(m_sDefaultLogPath , sName);

	return;
}

/******************************************************************************
 Description:	Free memory allocated on construction. 	
 Returns:      	void.
 Parameters:   	none.
 Assumptions:   none.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/9/2003   14:56	 Documentation
******************************************************************************/
void CQLogger::Cleanup()
{
	m_sBuffer.Empty();

	return;
}

////////////////////////////////////////////////////////////////////////////////////
//File handling                                                                   //
////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 Description:	Opens a log file for writing. 	
 Returns:      	HANDLE to the file opened or INVALID_HANDLE_VALUE in case of failure.
 Parameters:   	none.
 Assumptions:   none.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/9/2003   14:58	 Documentation
******************************************************************************/
HANDLE CQLogger::OpenLogFile()
{
	m_hFile = INVALID_HANDLE_VALUE;

	m_hFile = CreateFile((LPCTSTR)m_sFilePath,     // pointer to name of the file
						 GENERIC_WRITE,            // access (read-write) mode
						 FILE_SHARE_READ,		   // share mode
						 NULL,				       // no security                               
						 OPEN_ALWAYS,              // how to create
						 FILE_ATTRIBUTE_NORMAL,    // file attributes
						 NULL);

	return m_hFile;
}

/******************************************************************************
 Description:	Open the default log file and write an error message. 	
 Returns:      	HANDLE to the new file created.
 Parameters:   	none.
 Assumptions:   none.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/9/2003   14:59	 Documentation
******************************************************************************/
HANDLE CQLogger::WriteToDefaultLogFile()
{
	GetDefaultLogConfig();

	m_hFile = CreateFile((LPCTSTR)m_sFilePath,     // pointer to name of the file
						 GENERIC_WRITE,            // access (read-write) mode
						 FILE_SHARE_READ,		   // share mode
						 NULL,				       // no security                               
						 OPEN_ALWAYS,              // how to create
						 FILE_ATTRIBUTE_NORMAL,    // file attributes
						 NULL);

	return m_hFile;
}

/******************************************************************************
 Description:	Make sure we are within the allocated bounds of m_sBuffer 	
 Returns:      	TRUE if within bounds, FALSE otherwise.  A message is within 
				bounds if the whole message can fit in the space allocated.
 Parameters:   	lMessageLength - length of the message we wish to write.
 Assumptions:   none.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/9/2003   15:01	 Documentation
******************************************************************************/
BOOL CQLogger::WithinBufferBounds(long lMessageLength)
{
	BOOL bRetVal = TRUE;

	long lExpectedOffset = m_lCurrentOffset + lMessageLength;

	long lTotalBuffSize = m_lMaxMessageCount * m_lMaxMessageSize;

	if(lExpectedOffset > lTotalBuffSize)
		bRetVal = FALSE;

	return bRetVal;
}

/******************************************************************************
 Description:	Appends a message at the end of m_sBuffer.  If m_sBuffer was empty,
				the thread is restarted.
 Returns:      	Total length of data stored in m_sBuffer AFTER appending new
				message if successful, zero otherwise.
 Parameters:   	sMessage - message to add.
				lMessageLength - length of data in sMessage.
 Assumptions:   none.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/9/2003   15:03	 Documentation
******************************************************************************/
long CQLogger::AddMessage(char * sMessage , long lMessageLength)
{
	long lRetVal = 0;

	if(WithinBufferBounds(lMessageLength))
	{
		if(0 == m_lCurrentOffset) //If this is the first message,
			StartThread();    //start disk writing thread

		m_lCurrentOffset += lMessageLength;

		lRetVal = m_lCurrentOffset;
	}
	

	return lRetVal;
}

/******************************************************************************
 Description:	Clear m_sBuffer, reset the offset pointer, and stop the thread. 	
 Returns:      	void.
 Parameters:   	none.
 Assumptions:   none.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/9/2003   15:44	 Documentation
******************************************************************************/
void CQLogger::ClearMessageBuffer()
{
	m_sBuffer.Empty();

	m_lCurrentOffset = 0;

	//Buffer is empty, we can stop the thread now.
	StopThread();

	return;
}

/******************************************************************************
 Description:	Write messages stored in m_sBuffer to the disk. 	
 Returns:      	void.
 Parameters:   	none.
 Assumptions:   This method is only called from ThreadRoutine()
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/9/2003   15:45	 Documentation
 Eyal			2/9/2003   15:20	 Changed returned type to BOOL (from void)
******************************************************************************/
BOOL CQLogger::WriteToDisk()
{
	//The following parameters are allocated statically to avoid
	//constant reallocation every thread iteration.
	static DWORD dwBytesWrittenToFile = 0;
	static DWORD dwPos = 0;

	dwPos = SetFilePointer(m_hFile,0,NULL,FILE_END);

	LockFile(m_hFile , dwPos , 0 , dwPos + m_lCurrentOffset , 0);

	BOOL bRetVal = WriteFile(m_hFile , m_sBuffer , m_lCurrentOffset , &dwBytesWrittenToFile , NULL);

	UnlockFile(m_hFile , dwPos , 0 , dwPos + m_lCurrentOffset , 0);

	return bRetVal;
}

/******************************************************************************
 Description:	Perform asynchroneous log writing, by flushing m_sBuffer to the
				disk.  The thread is started once the buffer is written to (see
				AddMessage) and stopped after it is flushed to the disk.
 Returns:      	void.
 Parameters:   	pData - not in use.
 Assumptions:   All derived classes use the m_csLogFile critical section before
				attempting any writing to the log.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/9/2003   15:47	 Documentation
******************************************************************************/
void CQLogger::ThreadRoutine(void * pData)
{
	static BOOL bSuccesfulWrite = FALSE;

	EnterCriticalSection(&m_csLogFile);

	for(int i = 0;i < MAX_WRITE_ATTEMPTS;i++)
	{
		if(OpenLogFile() != INVALID_HANDLE_VALUE)
		{
			if(m_lCurrentOffset)
				bSuccesfulWrite = WriteToDisk();
			
			CloseLogFile();

			if(bSuccesfulWrite)
			{
				ClearMessageBuffer();

				break;
			}
		}

		else
			Sleep(SLEEP_INTERVAL_BETWEEN_ATTEMPTS);
	}

	//If the offset is not zero, we haven't cleared the buffer, which means we
	//haven't written to the real log.
	if(m_lCurrentOffset)
	{
		//m_hFile will be used to store the HANDLE to the default log.
		if(WriteToDefaultLogFile() != INVALID_HANDLE_VALUE)
		{
			bSuccesfulWrite = WriteToDisk();

			CloseLogFile();

			if(bSuccesfulWrite)
				ClearMessageBuffer();

			else
				LogCatastrophicFailure();
		}
	}
	
	LeaveCriticalSection(&m_csLogFile);

	return;
}

/******************************************************************************
 Description:	Store messages in m_sBuffer. This method is vitrual, and is necessary
				in order to seamlessly integrate CQLogger with existing log
				writing code.
 Returns:      	See AddMessage()
 Parameters:   	dwWrittenToBuff - length of data in m_sBuff, including line feed and 
								  newline characters.
				pRetVal - storage for additional error code data, not used.
				lDiskWarning - not in use.
 Assumptions:   none.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/9/2003   15:53	 Documentation
******************************************************************************/
long CQLogger::WriteLogFile(DWORD dwWrittenToBuff , long * pRetVal , long lDiskWarning)
{
	return AddMessage((char*)m_sBuff.GetBuffer(0), (long)dwWrittenToBuff);
}