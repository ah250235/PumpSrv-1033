// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Log.h"
#include <stdio.h>
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog() : m_iTotalBytesWritten(0) , 
                           m_bWrite(TRUE) ,			//Eyal 4.0.0.22 Start
						   m_bIsFileSealed(false) ,
			               m_dPercentFree(0),
						   m_dWarningLevel(0),
						   m_bWithConsole(FALSE),	//Eyal 4.0.0.22 End
						   m_lLastError(0),			//4.0.0.66
						   m_bSwitchedFile(FALSE),  //4.0.1.38
						   m_dwLastSystemError(0),  //4.0.1.38
						   m_lfileRenameRetries(0),	//TD-42563 Boazf 
						   m_TimeStamp(0)
{

	InitializeCriticalSection(&m_csLogFile);
	InitializeCriticalSection(&m_csDebug);  //Eyal 4.0.0.22

	m_hFile = INVALID_HANDLE_VALUE;

	//4.0.0.22  Eyal
	m_sFileName.Empty();

	//4.0.1.38
	memset(m_sWorkingPath , 0 , sizeof(m_sWorkingPath));

	SetTimeStampMethod(CTimeFactory::factory(TIME_STAMPS_CLOCK));

	//TD-42563 boazf	if system started with no space on the disk
	//it'll take the system 64K of writes until realizing it cant write (if it didn't crushed).
	//adding the next function call will seal the file and the Log wont write to file.
	m_lLastError = CheckDisk();
}

CLog::~CLog()
{
// 	CloseLogFile();
// 	DeleteCriticalSection(&m_csLogFile);
// 	
// 	//Eyal 4.0.0.22 Start
// 	if(m_bWithConsole)
// 		FreeConsole();
// 
// 	DeleteCriticalSection(&m_csDebug);
// 	//Eyal 4.0.0.22 End
// 
// 	delete m_TimeStamp;
}

void CLog::CleanUp()
{
	CloseLogFile();
	DeleteCriticalSection(&m_csLogFile);
	
	//Eyal 4.0.0.22 Start
	if(m_bWithConsole)
		FreeConsole();

	DeleteCriticalSection(&m_csDebug);
	//Eyal 4.0.0.22 End

	delete m_TimeStamp;
}

void CLog::SetTimeStampMethod(CTimeStamp* newVal)
{
	if(m_TimeStamp)
	{
		delete m_TimeStamp;
	}

	m_TimeStamp = newVal;
}

HANDLE CLog::WriteToDefaultLogFile()
{
	LPVOID lpMsgBuf = NULL;
	
	//Write the reason for the log change in the default log
	DWORD dwLastSystemError = GetLastError();
	DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |   //Allocate the buffer
		            FORMAT_MESSAGE_FROM_SYSTEM |       //System define message
					FORMAT_MESSAGE_IGNORE_INSERTS;     //No inserts (%s , %d, etc.)
	
	FormatMessage(dwFlags,                   //Flags
				  NULL,						 //This parameter is ignored because of the flsg settings
				  dwLastSystemError,
				  0,                         //Default language
				  (LPTSTR)&lpMsgBuf,         //Buffer containing error message
				  0,                         //Minimal buffer length
                  NULL);                     //Argumnents for insetrs (arguments for %s , %d , etc.)

	char sTmpMsg[256] , sName[MAX_FILE_NAME] , sPath[MAX_FILE_PATH];
	long lVal = 0;
	BYTE sTmpFileName[MAX_FILE_PATH]; 

	memset(sName , 0 , sizeof(sName));
	memset(sPath , 0 , sizeof(sPath));
	
	//4.0.0.65
	//4.0.1.38 GetProcessName memsets sTmpFileName to 0 at its beginning
	//so we don't need to memset it before the function call.
	DWORD lRet = GetProcessName((LPSTR)sTmpFileName , MAX_FILE_PATH);
	GetProcessNameDetails((LPSTR)sTmpFileName , lRet , (LPSTR)sPath , (LPSTR)sName);
	
	strcat((char*)sPath , "\\DefaultLog\\");
	CreateDirectory((LPCSTR)sPath , NULL);

	//Reset sTmpFileName, so we can use it for the new filename
	memset((LPSTR)sTmpFileName , 0 , sizeof(sTmpFileName));
	strcpy((LPSTR)sTmpFileName , (LPSTR)sPath);
	
	//Get the log file's name, or use default
	if (FALSE == m_sFileName.IsEmpty())
	{
		strcat((char*)sTmpFileName, (char*)m_sFileName.GetBuffer(0)); //4.0.0.65
	}
		
	
	else
	{
		strcat((char*)sTmpFileName , sName); //4.0.0.65 & 4.0.1.38
		strcat((char*)sTmpFileName , ".Log"); //4.0.1.38
	}

	m_hFile = CreateFile((LPCTSTR)sTmpFileName,  // pointer to name of the file
					     GENERIC_WRITE,          // access (read-write) mode
					     FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,		 // share mode
						 NULL,				     // no security                               
						 OPEN_ALWAYS,            // how to create
						 FILE_ATTRIBUTE_NORMAL,  // file attributes
						 NULL);


	//4.0.0.65
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		//4.0.1.38
		if(!m_bSwitchedFile || (dwLastSystemError != m_dwLastSystemError))
		{	
			m_bSwitchedFile = TRUE;
			m_dwLastSystemError = dwLastSystemError;
		
			memset(sTmpMsg , 0, sizeof(sTmpMsg));
			
			//Save original message in tmp buffer
			strcpy(sTmpMsg, m_sBuff.GetBuffer(0));

			//Create special error message and write it to the log
			m_sBuff.Format(m_sBuff, "*******ERROR: %s*******FAILED TO WRITE TO FILE %s\r\n", lpMsgBuf, m_sFilePath);
			WriteLogFile(m_sBuff.GetLength(), &lVal);

			//4.0.1.38
			//Free buffer containing error message
			LocalFree(lpMsgBuf);

			//Put original message back in m_sBuff, so that it will be written
			//to the new log.
			m_sBuff.Format("%s", sTmpMsg);

			//4.0.1.38 Reset working path for disk space checks
			strcpy((char*)m_sWorkingPath , (char*)sPath);
			strcat((char*)m_sWorkingPath , (char*)sName);
		}
	}
	
	return m_hFile;
}

HANDLE CLog::OpenLogFile()
{		
	DWORD dwFileAttributes = GetFileAttributes((LPCTSTR)m_sFilePath);
	DWORD dwLastError = GetLastError();

	if ((INVALID_HANDLE_VALUE == m_hFile) || ((INVALID_HANDLE_VALUE != m_hFile) && (dwFileAttributes == INVALID_FILE_ATTRIBUTES)))
	{
		
		if (INVALID_HANDLE_VALUE != m_hFile)
			{
				CloseLogFile();
			}
		
		// Open File in share mode
		//4.0.2.12 Added FILE_SHARE_DELETE
		//4.0.3.19 removed FILE_SHARE_DELETE

		m_hFile = CreateFile((LPCTSTR)m_sFilePath,     // pointer to name of the file
							 GENERIC_WRITE,            // access (read-write) mode
							 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // share mode
							 NULL,				       // no security                               
							 OPEN_ALWAYS,              // how to create
							 FILE_ATTRIBUTE_NORMAL,    // file attributes
							 NULL);

		if(m_hFile != INVALID_HANDLE_VALUE)
		{
			if(m_bSwitchedFile)
			{
				//If log file was a default file, reset working path
				//to original value
				strcpy((char*)m_sWorkingPath , (char*)m_sBaseFilePath.GetBuffer(0));
				m_bSwitchedFile = FALSE;
			}
		}
		
		//if we failed to open a log file, we will attempt
		//to open one in the drive
		//where the current working directory is.
		//This is relevant only when no valid path could be created.
		else
		{
			m_hFile = WriteToDefaultLogFile();
		}

		
	}

	return m_hFile;
}

long CLog::WriteLogFile(DWORD dwWrittenToBuff , long * pRetVal , long lDiskWarning)
{
	DWORD dwWrittenToFile = 0;
	ULONGLONG llCheckInterval = 0;
	
	if (INVALID_HANDLE_VALUE != OpenLogFile())
	{
	

		//Eyal 4.0.0.22 Start
		m_iTotalBytesWritten += dwWrittenToBuff;

		//Eyal 4.0.0.44 start
		if(m_lLastError)     //4.0.1.38 changed condition from if(m_bIsFileSealed)
		{
			llCheckInterval = SPACE_CHECK_INTERVAL / 4;	
			if(pRetVal)
			{
				*pRetVal = m_lLastError;   //4.0.0.66
			}
		}
		else
			llCheckInterval = SPACE_CHECK_INTERVAL;
		//Eyal 4.0.0.44 end

		//Eyal 4.0.1.32 Check disk if warning percent OR log percent greater than 0
		//4.0.2.38 , 	
		if((m_dPercentFree || m_dWarningLevel || lDiskWarning) && (m_iTotalBytesWritten >= llCheckInterval))
		{
			if(pRetVal)
			{
				*pRetVal = CheckDisk();
				m_lLastError = *pRetVal;  //4.0.0.66 4.0.1.38
			}
				
			else
				CheckDisk();
		}
		
		char * sTextToWrite = NULL;
		
		if(m_bWrite)
			sTextToWrite = (char*)m_sBuff.GetBuffer(0);
			
		else if(!m_bIsFileSealed)
		{
			sTextToWrite = FILE_SEALED_MESSAGE;
			dwWrittenToBuff = FILE_SEALED_MESSAGE_LENGTH;
			m_bIsFileSealed = true;
	
		}

		if(sTextToWrite)
		{
			OpenLogFile();

			DWORD dwPos = SetFilePointer(m_hFile,0,NULL,FILE_END);
			LockFile(m_hFile,dwPos,0,dwPos+dwWrittenToBuff,0);

			WriteFile(m_hFile,			  // handle to file to write to
					  sTextToWrite,		  // pointer to data to write to file
					  dwWrittenToBuff,    // number of bytes to write
					  &dwWrittenToFile,	  // pointer to number of bytes written
					  NULL);			  // pointer to structure for overlapped I/O
			UnlockFile(m_hFile,dwPos,0,dwPos+dwWrittenToBuff,0);
		}
	}
	return dwWrittenToFile;
}

void CLog::CloseLogFile()
{
			
	if (INVALID_HANDLE_VALUE != m_hFile)
	{
		// Flushing the O.S. write buffer.
		FlushFileBuffers(m_hFile);

		SetEndOfFile(m_hFile);		
		// Close log file , only if the handle is valid. 

		if (NULL == CloseHandle(m_hFile))
			{
				//If Closing the file failed - try to write it to the file
				//even though the file has a problem... if the closehandle fails it is probably gonna crush the system
				long iErrorRet = GetLastError();

				m_sBuff.Format("%s Closing the file handle Failed Reason: %d \r\n", m_TimeStamp->getTimeStamp(), iErrorRet);
				DWORD dwWrittenToBuff = m_sBuff.GetLength();
			
				WriteLogFile(dwWrittenToBuff,&iErrorRet);
			}
	}

	m_hFile = INVALID_HANDLE_VALUE;
}

//Eyal 4.0.0.22 Start
long CLog::CheckDisk()
{
	char sDiskName[4];

	//4.0.1.38
	strncpy(sDiskName , (const char*)m_sWorkingPath , 3);
	
	sDiskName[3] = '\0';

	double dPercentCurrentlyFree = 0;
	FreeSpacePercentage(sDiskName , &dPercentCurrentlyFree);

	//TD-42563 boazf added a '=' to the if (if threshold is 0 then the if will never be true)
	//There is not enough space left on the disk.
	if(dPercentCurrentlyFree <= m_dPercentFree)
	{
		m_bWrite = FALSE;
		m_iTotalBytesWritten = 0;
		return NO_SPACE;
	}

	//There is sufficient space on the disk.
	m_bWrite = TRUE;
	m_bIsFileSealed = false;
	m_iTotalBytesWritten = 0;

	//We are approaching dangerously high log size.
	if(dPercentCurrentlyFree < m_dWarningLevel)		return LOW_SPACE;
	

	return OK_SPACE;
}

void CLog::ResetLogPercentage(int iPercent)
{
	//if the log percent has been reset to zero,
	//we always write to the log.
	if(iPercent == 0)
		m_bWrite = TRUE;

	m_dPercentFree = iPercent;
}

/******************************************************************************
 Description:	Reset the lo path during runtime. 	
 Returns:      	void.
 Parameters:   	char * sNewPath - new target path for log file.
 Assumptions:   none
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			29/1/2003   19:09		Start
******************************************************************************/

void CLog::ResetLogPath(char * sNewPath)
{
	EnterCriticalSection(&m_csLogFile);

	m_sBaseFilePath.Empty();
	m_sFilePath.Empty();
	
	m_sBaseFilePath += sNewPath;
	//4.0.3.45 - fixed the base file setting
	m_sBaseFilePath += "\\Log\\";
	m_sFilePath += m_sBaseFilePath;
	m_sBaseFilePath += "\\PumpSrv";

	//We may have reset the path to a directory
	//with no "Log" subdirectory, so we attempt to create it here
	CreateDirectory((LPCSTR)m_sFilePath.GetBuffer(0) , NULL);
	m_sFilePath += m_sFileName;

	InitWorkingDirectory();

	LeaveCriticalSection(&m_csLogFile);
}

void CLog::ConsoleMsg(char *sMsg)
{
	EnterCriticalSection(&m_csDebug);
	//Eyal 4.0.0.62
	if(m_bWithConsole)
		printf("%s\n",sMsg);
	LeaveCriticalSection(&m_csDebug);
}

//*******************************************************
// allow GUI to use CONSOL IO
//*******************************************************
void CLog::RedirectIOToConsole()	// 4.0.33.21
{
	HWND					   hWnd = 0;
	HMENU					   hMenu = 0;
	char					   TempConsoleTitle[MAX_PATH];

	AllocConsole();

	// Disable the closing of the console window.
	//=============================================
	sprintf_s(TempConsoleTitle, MAX_PATH,
		"12345678987654321%ld", GetCurrentProcessId());

	long lCounter = 10;
	SetConsoleTitle(TempConsoleTitle); //to change the console title to a unique title. 

	Sleep(10); //in order to give the console a chance to get the Caption change 
	while (!hWnd && lCounter)
	{
		// Look for NewWindowTitle.
		hWnd = FindWindow(NULL, TempConsoleTitle);  //TD - 48328
		lCounter--;
	}

	if (hWnd)
	{
		hMenu = GetSystemMenu(hWnd, FALSE);
		if (hMenu)
		{
			DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
		}
	}

	SetConsoleTitle(m_sFilePath.GetBuffer());

	HANDLE con_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int sys_out = _open_osfhandle(intptr_t(con_out), _O_TEXT);
	FILE *out = _fdopen(sys_out, "w");

	HANDLE con_err = GetStdHandle(STD_ERROR_HANDLE);
	int sys_err = _open_osfhandle(intptr_t(con_err), _O_TEXT);
	FILE *err = _fdopen(sys_err, "w");

	HANDLE con_in = GetStdHandle(STD_INPUT_HANDLE);
	int sys_in = _open_osfhandle(intptr_t(con_in), _O_TEXT);
	FILE *in = _fdopen(sys_in, "r");

	ios::sync_with_stdio(true);

	freopen_s(&in, "CONIN$", "r", stdin);
	freopen_s(&out, "CONOUT$", "w", stdout);
	freopen_s(&err, "CONOUT$", "w", stderr);
	m_bWithConsole = TRUE;
}

void CLog::SetLogLevel(DWORD dwOldLevel, DWORD dwNewLevel)
{
	//check parameter validity
	if ((dwNewLevel >= LOG_LEVEL_0) && (dwNewLevel <= LOG_LEVEL_5))
	{
		if((dwOldLevel == LOG_LEVEL_5) && (dwNewLevel != LOG_LEVEL_5))
		{
			FreeConsole();
			m_bWithConsole = FALSE;
		}

		if((dwOldLevel != LOG_LEVEL_5) && (dwNewLevel == LOG_LEVEL_5))
		{
			RedirectIOToConsole();
			m_bWithConsole = TRUE;
		}
	}
}

int CLog::FreeSpacePercentage(char * sPath , double * pdFreeSpace)
{
	LONGLONG iBytesFree , iTotalBytesOnDisk;
	int iRetVal = GetDiskFreeSpaceEx(sPath , 
			                         (PULARGE_INTEGER)&iBytesFree , 
					                 (PULARGE_INTEGER)&iTotalBytesOnDisk ,
					                 NULL);

	if(iRetVal)
	{
		*pdFreeSpace = (double)iBytesFree / 
			           (double)iTotalBytesOnDisk * 100;
		return 0;
	}

	//This ensures that if disk reading has failed,
	//no writing will take place.
	*pdFreeSpace = 100;
	return 1;
}
//Eyal 4.0.0.22 End


void CLog::GetProcessNameDetails(LPSTR sFullName, int iFullNameLen , LPSTR sPath, LPSTR sName)
{
	int iPosition = iFullNameLen - 1 , iNameLen = 0 , iTmpPosition = 0;
	char sNameFromBackToFront[32];

	memset((char*)sNameFromBackToFront , 0 , sizeof(sNameFromBackToFront));

	while(sFullName[iPosition] != '\\')
	{
		sNameFromBackToFront[iTmpPosition] = sFullName[iPosition];

		iTmpPosition++;
		iNameLen++;

		iPosition--;
	}

	//4.0.2.15
	//iNameLen++;
	
	if(sName)
	{
		for(int i = iNameLen - 1 , j = 0;i >= 0;i--,j++)
		{
			if(sNameFromBackToFront[i] != '.')
				sName[j] = sNameFromBackToFront[i];

			else
			{
				sName[j] = '\0';
				break;
			}
		}
	}

	memcpy(sPath , sFullName , iFullNameLen - iNameLen);
	sPath[iFullNameLen - iNameLen] = '\0';

	return;
}

DWORD CLog::GetProcessName(LPSTR sName, int iSize)
{
	memset(sName , 0 , iSize);

	//Get handle to this module.
	HMODULE hModuleHandle = GetModuleHandle(NULL);

	//Use handle to get module path and name
	DWORD lRet = GetModuleFileName(hModuleHandle , (LPSTR)sName , iSize);
	
	return lRet;
}

// 4.0.23.2260  TD 396530
int CLog::ArchiveOldLogFile(LPSTR sDateBuf , LPSTR sLastLogSuffix)
{
	long lVal = 0;
	BOOL bResult = FALSE;

	if(m_lfileRenameRetries > 0)
	{
		if(m_lfileRenameRetries < FILE_RENAME_MAX_RETRIES)
			m_lfileRenameRetries++;
		else
		{
			m_lfileRenameRetries = 0;
		}
		return -1;
	}

	try
	{
		BYTE sSaveFilePath[MAX_FILE_PATH] = {0};
		BYTE sCurrentFileName[MAX_FILE_PATH] = {0};

		char* sFileSuffixTmp; 
		if (NULL != m_sFilePath.GetBuffer(0))	
			sFileSuffixTmp = strchr((char*)m_sFilePath.GetBuffer(0),'.');
		
		strcat((char*)sCurrentFileName , (char*)m_sWorkingPath);
		strcat((char*)sCurrentFileName , (char*)sFileSuffixTmp);
		
		memcpy((char*)sSaveFilePath, (char*)m_sWorkingPath,MAX_FILE_PATH);
		strcat((char*)sSaveFilePath, (char*)sLastLogSuffix);
			
		DeleteLogFile(sSaveFilePath);
		
		CloseLogFile();
		
		if(RenameLogFile(sCurrentFileName, sSaveFilePath))
			return OK;

		bResult = BackupLogFile(sCurrentFileName, sSaveFilePath);

		if(bResult)
			return OK;
		else 
			return -1;
	}
	catch(...)
	{
		DWORD dwLastError = GetLastError();
     
		m_sBuff.Format("%s *******LOG: Error!!! Last error = %d \r\n", m_TimeStamp->getTimeStamp(), dwLastError);
		WriteLogFile(m_sBuff.GetLength(), &lVal);

		return -1;
	}
}


// 4.0.23.2260  TD 396530
BOOL CLog::IsFileExist(const char *fileName)
{
	//This will get the file attributes bitlist of the file
	DWORD fileAtt = GetFileAttributesA(fileName);
	
	//If an error occurred it will equal to INVALID_FILE_ATTRIBUTES
	if(fileAtt == INVALID_FILE_ATTRIBUTES)
		//So lets throw an exception when an error has occurred
		throw GetLastError();
	
	//If the path referers to a directory it should also not exists.
	return ( ( fileAtt & FILE_ATTRIBUTE_DIRECTORY ) == 0 );
}

// 4.0.23.2260  TD 396530
int CLog::DeleteLogFile( BYTE * sSaveFilePath)
{
	long lVal = 0;

	// Delete it if exist 
	int iResult  = _unlink( (char *)sSaveFilePath );
	
	//If the Delete failed for reason other than file not found (if file not found we dont care)
	if ((iResult != 0) && (ENOENT != errno) )
	{		
		//in the case of sharing violation try changing the file's attributes
		_chmod( (char *)sSaveFilePath, _S_IREAD | _S_IWRITE );
		
		if (INVALID_HANDLE_VALUE != OpenLogFile())
		{
			m_sBuff.Format("%s *******LOG: FAILED TO DELETE %s, REASON: Access violation, File Attributes changed and will be deleted next run \r\n", m_TimeStamp->getTimeStamp(), (char *)sSaveFilePath);
			WriteLogFile(m_sBuff.GetLength(), &lVal);
		}
		
		m_lfileRenameRetries++;
		
		
	}

	if(iResult == 0)
	{
		m_sBuff.Format("%s *******LOG: DELETION of %s SUCCEDED \r\n", m_TimeStamp->getTimeStamp(), (char *)sSaveFilePath);
		WriteLogFile(m_sBuff.GetLength(), &lVal);
	}

	return iResult;
}

BOOL CLog::ValidateFile( BYTE * sFileName)
{
	int iResult;
	long lVal = 0;
	if (FALSE == IsFileExist( (const char *)sFileName))
	{
		
		m_sBuff.Format("%s *******LOG: FILE %s DOES NOT EXIST!", m_TimeStamp->getTimeStamp(), (char *)sFileName);
		WriteLogFile(m_sBuff.GetLength(), &lVal);
		return FALSE;
		
	}
	
	if(iResult = _access((char *)sFileName, 0) != 0)	//checking existence of the file //4.0.23.830 168816
	{
		m_sBuff.Format("%s *******LOG: FILE %s IS IN READ ONLY MODE!, REASON: %d\r\n", m_TimeStamp->getTimeStamp(), (char *)sFileName, iResult);
		WriteLogFile(m_sBuff.GetLength(), &lVal);
		return FALSE;	
	}

		
	return TRUE;
}

BOOL CLog::BackupLogFile( BYTE * sCurrentFileName, BYTE * sSaveFilePath)
{
	long lVal; 

	BOOL bResult = CopyFile((LPCTSTR)sCurrentFileName,(LPCTSTR)sSaveFilePath,true);
	if(bResult)
	{
		bResult = DeleteFile((LPCTSTR)sCurrentFileName);

		m_sBuff.Format("%s *******LOG: COPY of %s to %s SUCCEDED \r\n", m_TimeStamp->getTimeStamp(), (char *)sCurrentFileName, (char *)sSaveFilePath);
		WriteLogFile(m_sBuff.GetLength(), &lVal);
	}
	if(FALSE == bResult)
	{
		DWORD dwLastError = GetLastError();
		m_sBuff.Format("%s *******LOG: Error!!! Last error = %d \r\n", m_TimeStamp->getTimeStamp(), dwLastError);
		WriteLogFile(m_sBuff.GetLength(), &lVal);
	}

	return bResult;
	
}


BOOL CLog::RenameLogFile(BYTE * sCurrentFileName, BYTE * sSaveFilePath)
{
	// Rename The xxx.Log to xxx.dd (dd is today.day)
	
	long lVal = 0;
	int iResult;
	
	if(FALSE == ValidateFile(sCurrentFileName))
		return FALSE;
	
	iResult = rename( (char *)sCurrentFileName , (char *)sSaveFilePath );
	
	if (OK == iResult)
	{	
		m_lfileRenameRetries = 0;
		
		if (INVALID_HANDLE_VALUE != OpenLogFile())
		{
			m_sBuff.Format("%s *******LOG: RENAMING of %s to %s SUCCEDED \r\n", m_TimeStamp->getTimeStamp(), (char *)sCurrentFileName, (char *)sSaveFilePath);
			WriteLogFile(m_sBuff.GetLength(), &lVal);
		}
		
		return TRUE;
	}
	else
	{
		m_sBuff.Format("%s *******LOG: FAILED TO RENAME %s To %s, REASON: %d, \r\n", m_TimeStamp->getTimeStamp(), (char *)sCurrentFileName, (char *)sSaveFilePath, errno);		
		if (INVALID_HANDLE_VALUE != OpenLogFile())
		{
			WriteLogFile(m_sBuff.GetLength(), &lVal);
		}
		
		m_lfileRenameRetries++;
		
		return FALSE;
	}
}