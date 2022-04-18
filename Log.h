// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__5208E993_31D9_11D2_9FA2_004005615A06__INCLUDED_)
#define AFX_LOG_H__5208E993_31D9_11D2_9FA2_004005615A06__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define MAX_FILE_PATH	200
#define MAX_FILE_NAME   32
#define MAX_MSG_LEN     4096

#define TIME_STAMP_LENGTH	100

#define LOG_OK 0
#define LOG_NOT_OK 1

#define WARNING_RANGE 1.5
#define DEFAULT_LOG_PERCENT 0
#define NO_SPACE 2
#define LOW_SPACE 1
#define OK_SPACE 0

#define SPACE_CHECK_INTERVAL 64 * 1024  //check every 64 KB

#define MAX_CONSOLE_LINES    500

#define FILE_SEALED_MESSAGE "********File Sealed********\n"
#define FILE_SEALED_MESSAGE_LENGTH strlen(FILE_SEALED_MESSAGE)

#define FILE_RENAME_MAX_RETRIES	50

#define TIME_STAMPS_CLOCK		0
#define TIME_STAMPS_INTERVAL	1
#define TIME_STAMPS_CLOCK_TRS	2

#include <stdio.h>
#include <FCNTL.h>
#include <direct.h>
#include <errno.h>
#include <sys/stat.h>


class CTimeStamp
{
public:
	virtual char* getTimeStamp() =0;	
protected:
	char m_sBuffer[TIME_STAMP_LENGTH];
};

class CClockTimeStamp : public CTimeStamp
{
public:
	char* getTimeStamp()
	{
		memset(m_sBuffer,0,TIME_STAMP_LENGTH);

		SYSTEMTIME st;
		GetLocalTime(&st);
		
		sprintf_s(m_sBuffer,sizeof m_sBuffer,"%02d-%02d-%04d %02d:%02d:%02d:%03d ",
				st.wDay,
				st.wMonth,
				st.wYear,
				st.wHour,
				st.wMinute,
				st.wSecond,
				st.wMilliseconds);

		return m_sBuffer;
	}

};

class CClockTimeStampTrsStyle : public CTimeStamp
{
public:
	char* getTimeStamp()
	{
		memset(m_sBuffer,0,TIME_STAMP_LENGTH);

		SYSTEMTIME st;
		GetLocalTime(&st);
		
		sprintf_s(m_sBuffer,sizeof m_sBuffer,"%02d-%02d-%04d, %02d:%02d:%02d:%03d ",
				st.wDay,
				st.wMonth,
				st.wYear,
				st.wHour,
				st.wMinute,
				st.wSecond,
				st.wMilliseconds);

		return m_sBuffer;
	}

};

class CInterValTimeStamp : public CTimeStamp
{
public:
	char* getTimeStamp()
	{
		memset(m_sBuffer,0,TIME_STAMP_LENGTH);

		DWORD time = GetTickCount();
		sprintf_s(m_sBuffer,sizeof m_sBuffer,"%d ",time);
		return m_sBuffer;

	}

};

class CTimeFactory
{
	public:
		static CTimeStamp* factory(int id)
		{
			switch (id)
				{
				case TIME_STAMPS_CLOCK:		return dynamic_cast<CTimeStamp*>(new CClockTimeStamp);
				case TIME_STAMPS_INTERVAL:	return dynamic_cast<CTimeStamp*>(new CInterValTimeStamp);
				case TIME_STAMPS_CLOCK_TRS: return dynamic_cast<CTimeStamp*>(new CClockTimeStampTrsStyle);
				}
			 
			return NULL;
		}


};

// CLog - Pure virtual class
class CLog  
{
public:
	CLog();
	virtual ~CLog();
	
	//Pure methods
	//Initialize CLog members here
	virtual void Init()=0;

	//Handle old log file archiving
	virtual BOOL SwapFile()=0;

	//Implement this method in such a way as to ensure
	//that m_sWorkingPath gets m_sBaseFilePath for best results.
	//The m_sWorkingPath member represents the directory where
	//the log really is.  That can change, from the configured
	//path to the default path, and back to the configured path again.
	virtual void InitWorkingDirectory(char * sPath = NULL) = 0;

	//Virtual methods
	//Disk Space Warning
	virtual void RaiseDiskWarning(long lWarning) { return; }
	
	//File Writing
	virtual long WriteLogFile(DWORD dwWrittenToBuff , long * pRetVal = NULL, long lDiskWarning = 0);


	//DOS Console
	void RedirectIOToConsole();
	void ConsoleMsg(char *sMsg);
	
	//Disk Space
	int FreeSpacePercentage(char * sPath , double * pdFreeSpace);
	void ResetLogPercentage(int iPercent);
	void ResetWarningLevel(int iPercent) { m_dWarningLevel = iPercent; }

	//Log Path
	void ResetLogPath(char * sNewPath);

	//File Handlers
	virtual HANDLE OpenLogFile();
	virtual HANDLE WriteToDefaultLogFile();
	void CloseLogFile();

	void SetTimeStampMethod(CTimeStamp* newVal);
	void CleanUp();

protected:
	//Methods
	long CheckDisk();
	void SetLogLevel(DWORD dwOldLevel , DWORD dwNewLevel);
	void GetProcessNameDetails(LPSTR sFullName , int iFullNameLen , LPSTR sPath , LPSTR sName);
	DWORD GetProcessName(LPSTR sName , int iSize);
	int ArchiveOldLogFile(LPSTR sDateBuf , LPSTR sLastLogSuffix);
	BOOL BackupLogFile( BYTE * sCurrentFileName, BYTE * sSaveFilePath);
	BOOL RenameLogFile( BYTE * sCurrentFileName, BYTE * sSaveFilePaths);	
	BOOL ValidateFile( BYTE * sCurrentFileName);
	int DeleteLogFile( BYTE * sSaveFilePath);
	BOOL IsFileExist(const char *fileName);

	//Members
	char m_sWorkingPath[MAX_FILE_PATH];
	HANDLE m_hFile;

	BOOL m_bWithConsole;

					 
	CRITICAL_SECTION m_csDebug;
	CRITICAL_SECTION m_csLogFile;
	

	CString m_sDefualtLogPath;
	CString m_sBaseFilePath;
	CString m_sFilePath;
	CString m_sFileName;
	BOOL m_bWrite;
	bool m_bIsFileSealed;
	double m_dPercentFree;
	double m_dWarningLevel;
	ULONGLONG m_iTotalBytesWritten;
	CString m_sBuff;
	CString dwWrittenToBuffm_sBuff;

	long m_lLastError;  //4.0.0.66
	
	long m_lfileRenameRetries;	
private:
	DWORD m_dwLastSystemError;

protected:
	BOOL m_bSwitchedFile;

	CTimeStamp*	m_TimeStamp;

};


#endif // !defined(AFX_LOG_H__5208E993_31D9_11D2_9FA2_004005615A06__INCLUDED_)
