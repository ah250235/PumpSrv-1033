#ifndef	EXCEPTIONHANDLER_H
#define	EXCEPTIONHANDLER_H

enum ExceptionLogLevel // TD 371850 (Merges from TD 356420)
{
	ExceptionLogLevelNone = 0,
	ExceptionLogLevelHigh = 1
};

#include <stdexcept>
#include "DbgHelp.h"
#include "ExceptionHandlerMsg.h"

#pragma warning (disable : 4229)

typedef BOOL (*__cdecl WriteDumpFile)(
  HANDLE hProcess,
  DWORD ProcessId,
  HANDLE hFile,
  MINIDUMP_TYPE DumpType,
  PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
  PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
  PMINIDUMP_CALLBACK_INFORMATION CallbackParam
);

class CExceptionHandler
{
public:		
	int __cdecl HandleException(LPEXCEPTION_POINTERS pExceptionPointers, const char *Message);
	char* GetExceptionDescription(DWORD ExceptionCode);
	void hprintf(HANDLE LogFile, LPCTSTR Format, ...);
	void hflush(HANDLE LogFile);
	void DumpSystemInformation(HANDLE LogFile);
	void DumpRegisters(HANDLE LogFile, PCONTEXT Context);
	void DumpStack(HANDLE LogFile, DWORD *pStack);
	void DumpModuleList(HANDLE LogFile);
	void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo);
	void FormatTime(LPTSTR output, FILETIME TimeToPrint);	
	BOOL DumpModuleInfo(HANDLE LogFile, HINSTANCE ModuleHandle, int nModuleNo);	
	BOOL GetWindowsVer(LPTSTR pszVersion, int *nVersion, LPTSTR pszMajorMinorBuild);
	void SetLoggingPath(const char* szPath);
	BOOL CheckLoggingPath();
	void CreateDefaultLogPath();
	static CExceptionHandler& ExceptionHandler();
	~CExceptionHandler();	
	void SetExceptionLogLevel(ExceptionLogLevel val); // TD 371850 (Merges from TD 356420)
protected:

	CExceptionHandler();		
	CRITICAL_SECTION	m_csErrorLogFile;
	long				m_lDumpCounter;
	long				m_lFilePointer;
	int					m_hprintf_index;
	BOOL				m_bFirstTime;
	BOOL				m_bMiniDumpSupported;
	BOOL				m_bOverrideDumpFiles;

	char				m_sFilesPath[MAX_FILE_PATH];
	char				m_hprintf_buffer[HPRINTF_BUFFER_SIZE];	// wvsprintf never prints more than one K.
	BOOL				GetNextFileName(char* szFileName);
	HINSTANCE			m_pDumpLib;
	WriteDumpFile		CreateDumpFile;
	ExceptionLogLevel	m_enExceptionLogLevel; // TD 371850 (Merges from TD 356420)
	static CExceptionHandler m_cExceptionHandler;

	
	
};

class CMiniVersion
{
// constructors
public:
	CMiniVersion(LPCTSTR lpszPath = NULL);
	BOOL VersionInit();
	void VersionRelease();

// operations
public:

// attributes
public:
	// fixed info
	BOOL GetFileVersion(WORD *pwVersion);
	BOOL GetProductVersion(WORD* pwVersion);
	BOOL GetFileFlags(DWORD& rdwFlags);
	BOOL GetFileOS(DWORD& rdwOS);
	BOOL GetFileType(DWORD& rdwType);
	BOOL GetFileSubtype(DWORD& rdwType);	

	// string info
	BOOL GetCompanyName(LPTSTR lpszCompanyName, int nSize);
	BOOL GetFileDescription(LPTSTR lpszFileDescription, int nSize);
	BOOL GetProductName(LPTSTR lpszProductName, int nSize);	

// implementation
protected:
	BOOL GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo);
	BOOL GetStringInfo(LPCTSTR lpszKey, LPTSTR lpszValue);

	BYTE*		m_pData;
	DWORD		m_dwHandle;
	WORD		m_wFileVersion[4];
	WORD		m_wProductVersion[4];
	DWORD		m_dwFileFlags;
	DWORD		m_dwFileOS;
	DWORD		m_dwFileType;
	DWORD		m_dwFileSubtype;

	TCHAR		m_szPath[MAX_PATH*2];
	TCHAR		m_szCompanyName[MAX_PATH*2];
	TCHAR		m_szProductName[MAX_PATH*2];
	TCHAR		m_szFileDescription[MAX_PATH*2];
};




#endif