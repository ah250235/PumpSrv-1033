
#include "stdafx.h"
#include "ExceptionHandler.h"


#include "windows.h"
#include <tchar.h>

CExceptionHandler CExceptionHandler::m_cExceptionHandler;

CExceptionHandler& CExceptionHandler::ExceptionHandler()
{
	return m_cExceptionHandler;
}

CExceptionHandler::CExceptionHandler() :
 m_lDumpCounter(0),
 m_hprintf_index(0),
 m_lFilePointer(0),
 m_bFirstTime(TRUE),
 m_bMiniDumpSupported(TRUE),
 CreateDumpFile(NULL),
 m_bOverrideDumpFiles(FALSE),
 m_enExceptionLogLevel(ExceptionLogLevelNone)	// TD 371850 (Merges from TD 356420)

{
	InitializeCriticalSection(&m_csErrorLogFile);
	memset(m_sFilesPath,0x00,sizeof(m_sFilesPath));
	
	m_pDumpLib = LoadLibrary( "DbgHelp.DLL" ) ;
    if( (m_pDumpLib ) == NULL )
	{
		m_bMiniDumpSupported = FALSE;	
	}
	else
	{
		CreateDumpFile = (WriteDumpFile)GetProcAddress(m_pDumpLib,"MiniDumpWriteDump");
		if (!CreateDumpFile)
			m_bMiniDumpSupported = FALSE;
	}
}

CExceptionHandler::~CExceptionHandler()
{
	DeleteCriticalSection(&m_csErrorLogFile);
	if (m_pDumpLib)
		FreeLibrary(m_pDumpLib);
}

int __cdecl CExceptionHandler::HandleException(LPEXCEPTION_POINTERS pExceptionPointers, const char *Message)
{

	EnterCriticalSection(&m_csErrorLogFile);

	// TD 371850 (Merges from TD 356420)
	if (m_enExceptionLogLevel != ExceptionLogLevelHigh)
	{
		LeaveCriticalSection(&m_csErrorLogFile);
		return EXCEPTION_EXECUTE_HANDLER;
	}


	if (m_bFirstTime)
	{
		//On first exception load the exceptions file directory - Currently log directory.
		if (!m_sFilesPath[0]) // no directory is saved
		{
			CreateDefaultLogPath();
		}
				
		// Init m_sFilesPath = %Path%
		// Dumps will be written to %Path%\Log\ExceptionFile...

		// Creating directories if needed.
		if (!CheckLoggingPath())
		{
			CreateDefaultLogPath();
		}

		// Getting the latest log file there.
	}
	
	// Create a filename to record the error information to.
	// Storing it in the executable directory works well.
	
	
	///////////////////////////////////////////////////////////////////////////
	//
	// write minidump
	//
	///////////////////////////////////////////////////////////////////////////
	// Replace the filename with our minidump file name
	
	// Create the file
	char szDumpFileName[MAX_FILE_PATH] = {0};	

	HANDLE hMiniDumpFile = NULL;
	do
	{
		GetNextFileName (szDumpFileName);
		
		hMiniDumpFile = CreateFile( 
			szDumpFileName,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_NEW | OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
			NULL);
		
		if (hMiniDumpFile == INVALID_HANDLE_VALUE)
			hMiniDumpFile = NULL;
	}
	while(!hMiniDumpFile);

	// Write the minidump to the file
	if ( (hMiniDumpFile != INVALID_HANDLE_VALUE) && m_bMiniDumpSupported)
	{
		DumpMiniDump(hMiniDumpFile, pExceptionPointers);

		// Close file
		CloseHandle(hMiniDumpFile);

	/*	char szMsg[MAX_PATH] = {0};
		sprintf(szMsg, "Exception dump file is available: File name - %s." ,szDumpFileName );
		_LOGMSG.LogMsg(szMsg);*/
	}


	//Write information to error log.
	char szErrorFileName[MAX_FILE_PATH] = {0};	
	sprintf(szErrorFileName, "%s\\%s",m_sFilesPath,XCRASHREPORT_ERROR_LOG_FILE );
	

	HANDLE hLogFile = CreateFile(
		szErrorFileName, 
		GENERIC_WRITE, 
		0, 
		0,
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 
		0);

	if (hLogFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString(_T("Error creating exception report\r\n"));
		
		LeaveCriticalSection(&m_csErrorLogFile);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	// Append to the error log
	SetFilePointer(hLogFile, 0, 0, FILE_END);

	// Print out a blank line to separate this error log from any previous ones
	//hprintf(hLogFile, _T("\r\n"));

	PEXCEPTION_RECORD Exception = pExceptionPointers->ExceptionRecord;
	PCONTEXT          Context   = pExceptionPointers->ContextRecord;

	TCHAR szCrashModulePathName[MAX_PATH*2];
	memset(szCrashModulePathName,0x00,  sizeof(szCrashModulePathName));

	TCHAR *pszCrashModuleFileName = _T("Unknown");
	

	// Print out the beginning of the error log in a Win95 error window
	// compatible format.
	hprintf(hLogFile, _T("%s caused %s (0x%08x) \r\nin module %s at %04x:%08x.\r\n\r\n"),
				szErrorFileName, GetExceptionDescription(Exception->ExceptionCode),
				Exception->ExceptionCode,
				pszCrashModuleFileName, Context->SegCs, Context->Eip);

	hprintf(hLogFile, _T("Exception handler called in %s.\r\n"), Message);

	if (m_bMiniDumpSupported)
		hprintf(hLogFile, _T("Exception Dump file is available. File name: %s.\r\n"), szDumpFileName);
	else
		hprintf(hLogFile, _T("Exception Dump file is not available. Failed to load %s file .\r\n"), "DbgHelp.DLL");

	DumpSystemInformation(hLogFile);

	// If the exception was an access violation, print out some additional
	// informationd, to the error log and the debugger.
	if (Exception->ExceptionCode == STATUS_ACCESS_VIOLATION &&
				Exception->NumberParameters >= 2)
	{
		TCHAR szDebugMessage[1000];
		const TCHAR* readwrite = _T("Read from");
		if (Exception->ExceptionInformation[0])
			readwrite = _T("Write to");
		wsprintf(szDebugMessage, _T("%s location %08x caused an access violation.\r\n"),
					readwrite, Exception->ExceptionInformation[1]);

#ifdef	_DEBUG
		// The Visual C++ debugger doesn't actually tell you whether a read
		// or a write caused the access violation, nor does it tell what
		// address was being read or written. So I fixed that.
		OutputDebugString(_T("Exception handler: "));
		OutputDebugString(szDebugMessage);
#endif

		hprintf(hLogFile, _T("%s"), szDebugMessage);
	}

	//Currently unnecessary
	//DumpRegisters(hLogFile, Context);

	// Print out the bytes of code at the instruction pointer. Since the
	// crash may have been caused by an instruction pointer that was bad,
	// this code needs to be wrapped in an exception handler, in case there
	// is no memory to read. If the dereferencing of code[] fails, the
	// exception handler will print '??'.

	//Registers status unnecessary - disabled
	/*	hprintf(hLogFile, _T("\r\nBytes at CS:EIP:\r\n"));
	BYTE * code = (BYTE *)Context->Eip;
	for (int codebyte = 0; codebyte < NUMBER_OF_CODE_BYTES; codebyte++)
	{
		__try
		{
			hprintf(hLogFile, _T("%02x "), code[codebyte]);

		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			hprintf(hLogFile, _T("?? "));
		}
	}*/

	// Time to print part or all of the stack to the error log. This allows
	// us to figure out the call stack, parameters, local variables, etc.

	// Esp contains the bottom of the stack, or at least the bottom of
	// the currently used area
	DWORD* pStack = (DWORD *)Context->Esp;

	DumpStack(hLogFile, pStack);

	DumpModuleList(hLogFile);

	hprintf(hLogFile, _T("\r\n===== [end of %s] =====\r\n"), 
		XCRASHREPORT_ERROR_LOG_FILE);
	hflush(hLogFile);
	CloseHandle(hLogFile);
	

	// return the magic value which tells Win32 that this handler didn't
	// actually handle the exception - so that things will proceed as per
	// normal.	
	LeaveCriticalSection(&m_csErrorLogFile);
	return EXCEPTION_EXECUTE_HANDLER;
}

char* CExceptionHandler::GetExceptionDescription(DWORD ExceptionCode)
{
	struct ExceptionNames
	{
		DWORD	ExceptionCode;
		TCHAR *	ExceptionName;
	};

#if 0  // from winnt.h
#define STATUS_WAIT_0                    ((DWORD   )0x00000000L)    
#define STATUS_ABANDONED_WAIT_0          ((DWORD   )0x00000080L)    
#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)    
#define STATUS_TIMEOUT                   ((DWORD   )0x00000102L)    
#define STATUS_PENDING                   ((DWORD   )0x00000103L)    
#define STATUS_SEGMENT_NOTIFICATION      ((DWORD   )0x40000005L)    
#define STATUS_GUARD_PAGE_VIOLATION      ((DWORD   )0x80000001L)    
#define STATUS_DATATYPE_MISALIGNMENT     ((DWORD   )0x80000002L)    
#define STATUS_BREAKPOINT                ((DWORD   )0x80000003L)    
#define STATUS_SINGLE_STEP               ((DWORD   )0x80000004L)    
#define STATUS_ACCESS_VIOLATION          ((DWORD   )0xC0000005L)    
#define STATUS_IN_PAGE_ERROR             ((DWORD   )0xC0000006L)    
#define STATUS_INVALID_HANDLE            ((DWORD   )0xC0000008L)    
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)    
#define STATUS_ILLEGAL_INSTRUCTION       ((DWORD   )0xC000001DL)    
#define STATUS_NONCONTINUABLE_EXCEPTION  ((DWORD   )0xC0000025L)    
#define STATUS_INVALID_DISPOSITION       ((DWORD   )0xC0000026L)    
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((DWORD   )0xC000008CL)    
#define STATUS_FLOAT_DENORMAL_OPERAND    ((DWORD   )0xC000008DL)    
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((DWORD   )0xC000008EL)    
#define STATUS_FLOAT_INEXACT_RESULT      ((DWORD   )0xC000008FL)    
#define STATUS_FLOAT_INVALID_OPERATION   ((DWORD   )0xC0000090L)    
#define STATUS_FLOAT_OVERFLOW            ((DWORD   )0xC0000091L)    
#define STATUS_FLOAT_STACK_CHECK         ((DWORD   )0xC0000092L)    
#define STATUS_FLOAT_UNDERFLOW           ((DWORD   )0xC0000093L)    
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((DWORD   )0xC0000094L)    
#define STATUS_INTEGER_OVERFLOW          ((DWORD   )0xC0000095L)    
#define STATUS_PRIVILEGED_INSTRUCTION    ((DWORD   )0xC0000096L)    
#define STATUS_STACK_OVERFLOW            ((DWORD   )0xC00000FDL)    
#define STATUS_CONTROL_C_EXIT            ((DWORD   )0xC000013AL)    
#define STATUS_FLOAT_MULTIPLE_FAULTS     ((DWORD   )0xC00002B4L)    
#define STATUS_FLOAT_MULTIPLE_TRAPS      ((DWORD   )0xC00002B5L)    
#define STATUS_ILLEGAL_VLM_REFERENCE     ((DWORD   )0xC00002C0L)     
#endif

	ExceptionNames ExceptionMap[] =
	{
		{0x40010005, _T("a Control-C")},
		{0x40010008, _T("a Control-Break")},
		{0x80000002, _T("a Datatype Misalignment")},
		{0x80000003, _T("a Breakpoint")},
		{0xc0000005, _T("an Access Violation")},
		{0xc0000006, _T("an In Page Error")},
		{0xc0000017, _T("a No Memory")},
		{0xc000001d, _T("an Illegal Instruction")},
		{0xc0000025, _T("a Noncontinuable Exception")},
		{0xc0000026, _T("an Invalid Disposition")},
		{0xc000008c, _T("a Array Bounds Exceeded")},
		{0xc000008d, _T("a Float Denormal Operand")},
		{0xc000008e, _T("a Float Divide by Zero")},
		{0xc000008f, _T("a Float Inexact Result")},
		{0xc0000090, _T("a Float Invalid Operation")},
		{0xc0000091, _T("a Float Overflow")},
		{0xc0000092, _T("a Float Stack Check")},
		{0xc0000093, _T("a Float Underflow")},
		{0xc0000094, _T("an Integer Divide by Zero")},
		{0xc0000095, _T("an Integer Overflow")},
		{0xc0000096, _T("a Privileged Instruction")},
		{0xc00000fD, _T("a Stack Overflow")},
		{0xc0000142, _T("a DLL Initialization Failed")},
		{0xe06d7363, _T("a Microsoft C++ Exception")},
	};

	for (int i = 0; i < sizeof(ExceptionMap) / sizeof(ExceptionMap[0]); i++)
		if (ExceptionCode == ExceptionMap[i].ExceptionCode)
			return ExceptionMap[i].ExceptionName;

	return _T("an Unknown exception type");
}

void CExceptionHandler::hflush(HANDLE LogFile)
{
	if (m_hprintf_index > 0)
	{
		DWORD NumBytes;
		WriteFile(LogFile, m_hprintf_buffer, min(lstrlen(m_hprintf_buffer),sizeof(m_hprintf_buffer)), &NumBytes, 0);		//4.0.15.450
		m_hprintf_index = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
// hprintf
void CExceptionHandler::hprintf(HANDLE LogFile, LPCTSTR Format, ...)
{
	if (m_hprintf_index > (HPRINTF_BUFFER_SIZE-1024))
	{
		DWORD NumBytes;
		WriteFile(LogFile, m_hprintf_buffer, min(lstrlen(m_hprintf_buffer),sizeof(m_hprintf_buffer)), &NumBytes, 0);		//4.0.15.450
		m_hprintf_index = 0;
	}

	va_list arglist;
	va_start( arglist, Format);
	m_hprintf_index += wvsprintf(&m_hprintf_buffer[m_hprintf_index], Format, arglist);
	va_end( arglist);
}

void CExceptionHandler::DumpSystemInformation(HANDLE LogFile)
{
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	TCHAR szTimeBuffer[100];
	FormatTime(szTimeBuffer, CurrentTime);

	hprintf(LogFile, _T("Error occurred at %s.\r\n"), szTimeBuffer);

	char szModuleName[MAX_PATH*2];
	memset(szModuleName,0x00, sizeof(szModuleName));
	if (GetModuleFileName(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpy(szModuleName, _T("Unknown"));

	TCHAR szUserName[200];
	memset(szUserName, 0x00, sizeof(szUserName));
	DWORD UserNameSize = _countof(szUserName)-2;
	if (!GetUserName(szUserName, &UserNameSize))
		lstrcpy(szUserName, _T("Unknown"));

	hprintf(LogFile, _T("%s, run by %s.\r\n"), szModuleName, szUserName);

	// print out operating system
	TCHAR szWinVer[50], szMajorMinorBuild[50];
	int nWinVer;
	GetWindowsVer(szWinVer, &nWinVer, szMajorMinorBuild);
	hprintf(LogFile, _T("Operating system:  %s (%s).\r\n"), 
		szWinVer, szMajorMinorBuild);

	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);
	hprintf(LogFile, _T("%d processor(s), type %d.\r\n"),
				SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);

	MEMORYSTATUS MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);

	// Print out info on memory, rounded up.
	hprintf(LogFile, _T("%d%% memory in use.\r\n"), MemInfo.dwMemoryLoad);
	hprintf(LogFile, _T("%d MBytes physical memory.\r\n"), (MemInfo.dwTotalPhys +
				ONEM - 1) / ONEM);
	hprintf(LogFile, _T("%d MBytes physical memory free.\r\n"), 
		(MemInfo.dwAvailPhys + ONEM - 1) / ONEM);
	hprintf(LogFile, _T("%d MBytes paging file.\r\n"), (MemInfo.dwTotalPageFile +
				ONEM - 1) / ONEM);
	hprintf(LogFile, _T("%d MBytes paging file free.\r\n"), 
		(MemInfo.dwAvailPageFile + ONEM - 1) / ONEM);
	hprintf(LogFile, _T("%d MBytes user address space.\r\n"), 
		(MemInfo.dwTotalVirtual + ONEM - 1) / ONEM);
	hprintf(LogFile, _T("%d MBytes user address space free.\r\n"), 
		(MemInfo.dwAvailVirtual + ONEM - 1) / ONEM);
}

void CExceptionHandler::DumpStack(HANDLE LogFile, DWORD *pStack)
{
	hprintf(LogFile, _T("\r\n\r\nStack:\r\n"));

	__try
	{
		// Esp contains the bottom of the stack, or at least the bottom of
		// the currently used area.
		DWORD* pStackTop;

		__asm
		{
			// Load the top (highest address) of the stack from the
			// thread information block. It will be found there in
			// Win9x and Windows NT.
			mov	eax, fs:[4]
			mov pStackTop, eax
		}

		if (pStackTop > pStack + MAX_STACK_DUMP)
			pStackTop = pStack + MAX_STACK_DUMP;

		int Count = 0;

		DWORD* pStackStart = pStack;

		int nDwordsPrinted = 0;

		while (pStack + 1 <= pStackTop)
		{
			if ((Count % STACK_COLUMNS) == 0)
			{
				pStackStart = pStack;
				nDwordsPrinted = 0;
				hprintf(LogFile, _T("0x%08x: "), pStack);
			}

			if ((++Count % STACK_COLUMNS) == 0 || pStack + 2 > pStackTop)
			{
				hprintf(LogFile, _T("%08x "), *pStack);
				nDwordsPrinted++;

				int n = nDwordsPrinted;
				while (n < 4)
				{
					hprintf(LogFile, _T("         "));
					n++;
				}

				for (int i = 0; i < nDwordsPrinted; i++)
				{
					DWORD dwStack = *pStackStart;
					for (int j = 0; j < 4; j++)
					{
						char c = (char)(dwStack & 0xFF);
						if (c < 0x20 || c > 0x7E)
							c = '.';
						hprintf(LogFile, _T("%c"), c);

						dwStack = dwStack >> 8;
					}
					pStackStart++;
				}

				hprintf(LogFile, _T("\r\n"));
			}
			else
			{
				hprintf(LogFile, _T("%08x "), *pStack);
				nDwordsPrinted++;
			}
			pStack++;
		}
		hprintf(LogFile, _T("\r\n"));
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		hprintf(LogFile, _T("Exception encountered during stack dump.\r\n"));
	}
}


void CExceptionHandler::DumpRegisters(HANDLE LogFile, PCONTEXT Context)
{
	// Print out the register values in an XP error window compatible format.
	hprintf(LogFile, _T("\r\n"));
	hprintf(LogFile, _T("Context:\r\n"));
	hprintf(LogFile, _T("EDI:    0x%08x  ESI: 0x%08x  EAX:   0x%08x\r\n"),
				Context->Edi, Context->Esi, Context->Eax);
	hprintf(LogFile, _T("EBX:    0x%08x  ECX: 0x%08x  EDX:   0x%08x\r\n"),
				Context->Ebx, Context->Ecx, Context->Edx);
	hprintf(LogFile, _T("EIP:    0x%08x  EBP: 0x%08x  SegCs: 0x%08x\r\n"),
				Context->Eip, Context->Ebp, Context->SegCs);
	hprintf(LogFile, _T("EFlags: 0x%08x  ESP: 0x%08x  SegSs: 0x%08x\r\n"),
				Context->EFlags, Context->Esp, Context->SegSs);
}

void CExceptionHandler::DumpModuleList(HANDLE LogFile)
{
	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);

	const size_t PageSize = SystemInfo.dwPageSize;

	// Set NumPages to the number of pages in the 4GByte address space,
	// while being careful to avoid overflowing ints
	const size_t NumPages = 4 * size_t(ONEG / PageSize);
	size_t pageNum = 0;
	void *LastAllocationBase = 0;

	int nModuleNo = 1;

	while (pageNum < NumPages)
	{
		MEMORY_BASIC_INFORMATION MemInfo;
		if (VirtualQuery((void *)(pageNum * PageSize), &MemInfo,
					sizeof(MemInfo)))
		{
			if (MemInfo.RegionSize > 0)
			{
				// Adjust the page number to skip over this block of memory
				pageNum += MemInfo.RegionSize / PageSize;
				if (MemInfo.State == MEM_COMMIT && MemInfo.AllocationBase >
							LastAllocationBase)
				{
					// Look for new blocks of committed memory, and try
					// recording their module names - this will fail
					// gracefully if they aren't code modules
					LastAllocationBase = MemInfo.AllocationBase;
					if (DumpModuleInfo(LogFile, 
									   (HINSTANCE)LastAllocationBase, 
									   nModuleNo))
					{
						nModuleNo++;
					}
				}
			}
			else
				pageNum += SIXTYFOURK / PageSize;
		}
		else
			pageNum += SIXTYFOURK / PageSize;

		// If VirtualQuery fails we advance by 64K because that is the
		// granularity of address space doled out by VirtualAlloc()
	}
}
void CExceptionHandler::DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo)
{
	if (excpInfo == NULL) 
	{
		// Generate exception to get proper context in dump
		__try 
		{
			OutputDebugString(_T("raising exception\r\n"));
			RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
		} 
		__except(DumpMiniDump(hFile, GetExceptionInformation()),
				 EXCEPTION_CONTINUE_EXECUTION) 
		{
		}
	} 
	else
	{
		OutputDebugString(_T("writing minidump\r\n"));
		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = excpInfo;
		eInfo.ClientPointers = FALSE;

		// note:  MiniDumpWithIndirectlyReferencedMemory does not work on Win98
		if ((m_bMiniDumpSupported) && (m_pDumpLib) && (CreateDumpFile))
		{
			CreateDumpFile(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MiniDumpWithPrivateReadWriteMemory,
			excpInfo ? &eInfo : NULL,
			NULL,
			NULL);
		}
	}
}

void CExceptionHandler::FormatTime(LPTSTR output, FILETIME TimeToPrint)
{
	output[0] = _T('\0');
	WORD Date, Time;
	if (FileTimeToLocalFileTime(&TimeToPrint, &TimeToPrint) &&
				FileTimeToDosDateTime(&TimeToPrint, &Date, &Time))
	{
		wsprintf(output, _T("%d/%d/%d %02d:%02d:%02d"),
					(Date / 32) & 15, Date & 31, (Date / 512) + 1980,
					(Time >> 11), (Time >> 5) & 0x3F, (Time & 0x1F) * 2);
	}
}

BOOL CExceptionHandler::GetWindowsVer(LPTSTR pszVersion, int *nVersion, LPTSTR pszMajorMinorBuild)
{
	if (!pszVersion || !nVersion || !pszMajorMinorBuild)
		return FALSE;
	lstrcpy(pszVersion, WUNKNOWNSTR);
	*nVersion = WUNKNOWN;

	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&osinfo))
		return FALSE;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;	// Win 95 needs this

	wsprintf(pszMajorMinorBuild, _T("%u.%u.%u"), dwMajorVersion, dwMinorVersion, dwBuildNumber);

	if ((dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4))
	{
		if ((dwMinorVersion < 10) && (dwBuildNumber == 950))
		{
			lstrcpy(pszVersion, W95STR);
			*nVersion = W95;
		}
		else if ((dwMinorVersion < 10) && 
				((dwBuildNumber > 950) && (dwBuildNumber <= 1080)))
		{
			lstrcpy(pszVersion, W95SP1STR);
			*nVersion = W95SP1;
		}
		else if ((dwMinorVersion < 10) && (dwBuildNumber > 1080))
		{
			lstrcpy(pszVersion, W95OSR2STR);
			*nVersion = W95OSR2;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber == 1998))
		{
			lstrcpy(pszVersion, W98STR);
			*nVersion = W98;
		}
		else if ((dwMinorVersion == 10) && 
				((dwBuildNumber > 1998) && (dwBuildNumber < 2183)))
		{
			lstrcpy(pszVersion, W98SP1STR);
			*nVersion = W98SP1;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber >= 2183))
		{
			lstrcpy(pszVersion, W98SESTR);
			*nVersion = W98SE;
		}
		else if (dwMinorVersion == 90)
		{
			lstrcpy(pszVersion, WMESTR);
			*nVersion = WME;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if ((dwMajorVersion == 3) && (dwMinorVersion == 51))
		{
			lstrcpy(pszVersion, WNT351STR);
			*nVersion = WNT351;
		}
		else if ((dwMajorVersion == 4) && (dwMinorVersion == 0))
		{
			lstrcpy(pszVersion, WNT4STR);
			*nVersion = WNT4;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 0))
		{
			lstrcpy(pszVersion, W2KSTR);
			*nVersion = W2K;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 1))
		{
			lstrcpy(pszVersion, WXPSTR);
			*nVersion = WXP;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 2))
		{
			lstrcpy(pszVersion, W2003SERVERSTR);
			*nVersion = W2003SERVER;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_CE)
	{
		lstrcpy(pszVersion, WCESTR);
		*nVersion = WCE;
	}
	return TRUE;
}


BOOL CExceptionHandler::DumpModuleInfo(HANDLE LogFile, HINSTANCE ModuleHandle, int nModuleNo)
{
	bool rc = false;
	TCHAR szModName[MAX_PATH*2];
	ZeroMemory(szModName, sizeof(szModName));

	__try
	{
		if (GetModuleFileName(ModuleHandle, szModName, sizeof(szModName)-2) > 0)
		{
			// If GetModuleFileName returns greater than zero then this must
			// be a valid code module address. Therefore we can try to walk
			// our way through its structures to find the link time stamp.
			IMAGE_DOS_HEADER *DosHeader = (IMAGE_DOS_HEADER*)ModuleHandle;
		    if (IMAGE_DOS_SIGNATURE != DosHeader->e_magic)
	    	    return false;

			IMAGE_NT_HEADERS *NTHeader = (IMAGE_NT_HEADERS*)((TCHAR *)DosHeader
						+ DosHeader->e_lfanew);
		    if (IMAGE_NT_SIGNATURE != NTHeader->Signature)
	    	    return false;

			// open the code module file so that we can get its file date and size
			HANDLE ModuleFile = CreateFile(szModName, GENERIC_READ,
						FILE_SHARE_READ, 0, OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL, 0);

			TCHAR TimeBuffer[100];
			TimeBuffer[0] = _T('\0');
			
			DWORD FileSize = 0;
			if (ModuleFile != INVALID_HANDLE_VALUE)
			{
				FileSize = GetFileSize(ModuleFile, 0);
				FILETIME LastWriteTime;
				if (GetFileTime(ModuleFile, 0, 0, &LastWriteTime))
				{
					FormatTime(TimeBuffer, LastWriteTime);
				}
				CloseHandle(ModuleFile);
			}
			hprintf(LogFile, _T("Module %d\r\n"), nModuleNo);
			hprintf(LogFile, _T("%s\r\n"), szModName);
			hprintf(LogFile, _T("Image Base: 0x%08x  Image Size: 0x%08x\r\n"), 
				NTHeader->OptionalHeader.ImageBase, 
				NTHeader->OptionalHeader.SizeOfImage), 

			hprintf(LogFile, _T("Checksum:   0x%08x  Time Stamp: 0x%08x\r\n"), 
				NTHeader->OptionalHeader.CheckSum,
				NTHeader->FileHeader.TimeDateStamp);

			hprintf(LogFile, _T("File Size:  %-10d  File Time:  %s\r\n"),
						FileSize, TimeBuffer);

			hprintf(LogFile, _T("Version Information:\r\n"));

			CMiniVersion cVersion(szModName);
			TCHAR szBuf[200];
			WORD dwBuf[4];

			cVersion.GetCompanyName(szBuf, sizeof(szBuf)-1);
			hprintf(LogFile, _T("   Company:    %s\r\n"), szBuf);

			cVersion.GetProductName(szBuf, sizeof(szBuf)-1);
			hprintf(LogFile, _T("   Product:    %s\r\n"), szBuf);

			cVersion.GetFileDescription(szBuf, sizeof(szBuf)-1);
			hprintf(LogFile, _T("   FileDesc:   %s\r\n"), szBuf);

			cVersion.GetFileVersion(dwBuf);
			hprintf(LogFile, _T("   FileVer:    %d.%d.%d.%d\r\n"), 
				dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);

			cVersion.GetProductVersion(dwBuf);
			hprintf(LogFile, _T("   ProdVer:    %d.%d.%d.%d\r\n"), 
				dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);

			cVersion.VersionRelease();

			hprintf(LogFile, _T("\r\n"));

			rc = true;
		}
	}
	// Handle any exceptions by continuing from this point.
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
	return rc;
}

CMiniVersion::CMiniVersion(LPCTSTR lpszPath)
{
	memset(m_szPath, 0x00, sizeof(m_szPath));

	if (lpszPath && lpszPath[0] != 0)
	{
		lstrcpyn(m_szPath, lpszPath, sizeof(m_szPath)-1);
	}
	else
	{
	}

	m_pData = NULL;
	m_dwHandle = 0;

	for (int i = 0; i < 4; i++)
	{
		m_wFileVersion[i] = 0;
		m_wProductVersion[i] = 0;
	}

	m_dwFileFlags = 0;
	m_dwFileOS = 0;
	m_dwFileType = 0;
	m_dwFileSubtype = 0;

	memset(m_szCompanyName, 0x00, sizeof(m_szCompanyName));
	memset(m_szProductName, 0x00,  sizeof(m_szProductName));
	memset(m_szFileDescription, 0x00, sizeof(m_szFileDescription));

	VersionInit();
}

BOOL CMiniVersion::VersionInit()
{
	DWORD dwHandle;
	DWORD dwSize;
	BOOL rc;

	dwSize = ::GetFileVersionInfoSize((LPSTR)(LPCTSTR)m_szPath, &dwHandle);
	if (dwSize == 0)
		return FALSE;
		
	m_pData = new BYTE [dwSize + 1];	
	memset(m_pData, 0x00, dwSize+1);

	rc = ::GetFileVersionInfo((LPSTR)(LPCTSTR)m_szPath, dwHandle, dwSize, m_pData);
	if (!rc)
		return FALSE;

	// get fixed info

	VS_FIXEDFILEINFO FixedInfo;
	
	if (GetFixedInfo(FixedInfo))
	{
		m_wFileVersion[0] = HIWORD(FixedInfo.dwFileVersionMS);
		m_wFileVersion[1] = LOWORD(FixedInfo.dwFileVersionMS);
		m_wFileVersion[2] = HIWORD(FixedInfo.dwFileVersionLS);
		m_wFileVersion[3] = LOWORD(FixedInfo.dwFileVersionLS);

		m_wProductVersion[0] = HIWORD(FixedInfo.dwProductVersionMS);
		m_wProductVersion[1] = LOWORD(FixedInfo.dwProductVersionMS);
		m_wProductVersion[2] = HIWORD(FixedInfo.dwProductVersionLS);
		m_wProductVersion[3] = LOWORD(FixedInfo.dwProductVersionLS);

		m_dwFileFlags   = FixedInfo.dwFileFlags;
		m_dwFileOS      = FixedInfo.dwFileOS;
		m_dwFileType    = FixedInfo.dwFileType;
		m_dwFileSubtype = FixedInfo.dwFileSubtype;
	}
	else
		return FALSE;

	// get string info

	GetStringInfo(_T("CompanyName"),     m_szCompanyName);
	GetStringInfo(_T("FileDescription"), m_szFileDescription);
	GetStringInfo(_T("ProductName"),     m_szProductName);

	return TRUE;		
}

///////////////////////////////////////////////////////////////////////////////
// Release
void CMiniVersion::VersionRelease()
{
	// do this manually, because we can't use objects requiring
	// a dtor within an exception handler
	if (m_pData)
		delete [] m_pData;
	m_pData = NULL;
}


BOOL CMiniVersion::GetFileVersion(WORD * pwVersion)
{
	for (int i = 0; i < 4; i++)
		*pwVersion++ = m_wFileVersion[i];
	return TRUE;
}					 	 

///////////////////////////////////////////////////////////////////////////////
// GetProductVersion
BOOL CMiniVersion::GetProductVersion(WORD * pwVersion)
{
	for (int i = 0; i < 4; i++)
		*pwVersion++ = m_wProductVersion[i];
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetFileFlags
BOOL CMiniVersion::GetFileFlags(DWORD& rdwFlags)
{
	rdwFlags = m_dwFileFlags;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetFileOS
BOOL CMiniVersion::GetFileOS(DWORD& rdwOS)
{
	rdwOS = m_dwFileOS;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetFileType
BOOL CMiniVersion::GetFileType(DWORD& rdwType)
{
	rdwType = m_dwFileType;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetFileSubtype
BOOL CMiniVersion::GetFileSubtype(DWORD& rdwType)
{
	rdwType = m_dwFileSubtype;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetCompanyName
BOOL CMiniVersion::GetCompanyName(LPTSTR lpszCompanyName, int nSize)
{
	if (!lpszCompanyName)
		return FALSE;
	ZeroMemory(lpszCompanyName, nSize);
	lstrcpyn(lpszCompanyName, m_szCompanyName, nSize-1);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetFileDescription
BOOL CMiniVersion::GetFileDescription(LPTSTR lpszFileDescription, int nSize)
{
	if (!lpszFileDescription)
		return FALSE;
	ZeroMemory(lpszFileDescription, nSize);
	lstrcpyn(lpszFileDescription, m_szFileDescription, nSize-1);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetProductName
BOOL CMiniVersion::GetProductName(LPTSTR lpszProductName, int nSize)
{
	if (!lpszProductName)
		return FALSE;
	ZeroMemory(lpszProductName, nSize);
	lstrcpyn(lpszProductName, m_szProductName, nSize-1);
	return TRUE;
}

BOOL CMiniVersion::GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo)
{
	BOOL rc;
	UINT nLength;
	VS_FIXEDFILEINFO *pFixedInfo = NULL;

	if (!m_pData)
		return FALSE;

	if (m_pData)
		rc = ::VerQueryValue(m_pData, _T("\\"), (void **) &pFixedInfo, &nLength);
	else
		rc = FALSE;
		
	if (rc)
		memcpy (&rFixedInfo, pFixedInfo, sizeof (VS_FIXEDFILEINFO));	

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// GetStringInfo
BOOL CMiniVersion::GetStringInfo(LPCSTR lpszKey, LPTSTR lpszReturnValue)
{
	BOOL rc;
	DWORD *pdwTranslation;
	UINT nLength;
	LPTSTR lpszValue;
	
	if (m_pData == NULL)
		return FALSE;

	if (!lpszReturnValue)
		return FALSE;

	if (!lpszKey)
		return FALSE;

	*lpszReturnValue = 0;

	rc = ::VerQueryValue(m_pData, _T("\\VarFileInfo\\Translation"), 
								(void**) &pdwTranslation, &nLength);
	if (!rc)
		return FALSE;

	char szKey[2000];
	wsprintf(szKey, _T("\\StringFileInfo\\%04x%04x\\%s"),
				 LOWORD (*pdwTranslation), HIWORD (*pdwTranslation),
				 lpszKey);

	rc = ::VerQueryValue(m_pData, szKey, (void**) &lpszValue, &nLength);

	if (!rc)
		return FALSE;
		
	lstrcpy(lpszReturnValue, lpszValue);

	return TRUE;
}

void CExceptionHandler::SetLoggingPath(const char* szPath)
{	
	if (szPath)
	{
		// Zeroing the old value
		memset (m_sFilesPath,0x00,sizeof(m_sFilesPath));
		strncpy(m_sFilesPath,szPath,min(strlen(szPath),sizeof(m_sFilesPath)));
		m_bFirstTime = TRUE; // Next exception will check the new path and create it.
	}
}

BOOL CExceptionHandler::CheckLoggingPath()
{
	BOOL bRetval = TRUE;

	HANDLE hDir = CreateFile(m_sFilesPath,GENERIC_READ,FILE_SHARE_WRITE | FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);

	if ((hDir == NULL) || (hDir == INVALID_HANDLE_VALUE))
	{
		bRetval = CreateDirectory(m_sFilesPath,NULL);

		if (!bRetval)
			memset(m_sFilesPath,0x00,sizeof(m_sFilesPath));
	}
	else
		CloseHandle(hDir);

	return bRetval;
}

void CExceptionHandler::CreateDefaultLogPath()
{
	//Get handle to this module.
	HMODULE hModuleHandle = GetModuleHandle(NULL);

	char sProcessName[MAX_PATH];
	// Use handle to get module path and name
	GetModuleFileName(hModuleHandle , (LPSTR)sProcessName, MAX_PATH);

	char* szLoc = strrchr(sProcessName,'\\');
	long lFileDirSeperatorLocation = 0;
	
	if (szLoc)
	{
		lFileDirSeperatorLocation = szLoc - sProcessName;
	
		memcpy(m_sFilesPath,sProcessName,min(lFileDirSeperatorLocation,MAX_FILE_PATH - 1));
		m_sFilesPath[min((lFileDirSeperatorLocation),MAX_FILE_PATH - 1)] = '\0';
		strcat((char*)m_sFilesPath , "\\DefaultLog");
	}
	else
	{
		// Creating a default directory
		sprintf(m_sFilesPath,"C:\\DefaultLog");
	}
}

BOOL CExceptionHandler::GetNextFileName(char* szFileName)
{
	BOOL bRetval = TRUE;
	char szDumpFileName[MAX_FILE_PATH] = {0};	
	char szFileExtention[4] = {0};
	HANDLE hMiniDumpFileTest = INVALID_HANDLE_VALUE;
	DWORD dwCreationFlags;
	do
	{	
		if (hMiniDumpFileTest != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hMiniDumpFileTest);
			hMiniDumpFileTest = INVALID_HANDLE_VALUE;
		}

		if (m_lDumpCounter >= MAX_DUMP_FILES)
		{
			m_lDumpCounter = 1;
			m_bOverrideDumpFiles = TRUE;
		}

		sprintf(szDumpFileName, "%s\\%s",m_sFilesPath,XCRASHREPORT_MINI_DUMP_FILE);
		l2a0(++m_lDumpCounter, (BYTE*)szFileExtention, sizeof(szFileExtention)-1);
		strcat(szDumpFileName, szFileExtention);
		strcat(szDumpFileName, ".DMP");
		
		dwCreationFlags = (m_bOverrideDumpFiles)?OPEN_EXISTING | CREATE_NEW:CREATE_NEW;

		CheckLoggingPath();
		hMiniDumpFileTest = CreateFile( 
				szDumpFileName,
				GENERIC_WRITE,
				0,
				NULL,
				dwCreationFlags,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
				NULL);
	}
	while(hMiniDumpFileTest == INVALID_HANDLE_VALUE);
	
	CloseHandle(hMiniDumpFileTest);
	if (szFileName)
		strcpy(szFileName,szDumpFileName);

	return TRUE;
}

// TD 371850 (Merges from TD 356420)
void CExceptionHandler::SetExceptionLogLevel(ExceptionLogLevel val)
{
	 m_enExceptionLogLevel = val;
}
