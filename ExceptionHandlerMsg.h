#ifndef	EXCEPTIONHANDLER_MSG
#define	EXCEPTIONHANDLER_MSG

#define XCRASHREPORT_MINI_DUMP_FILE		_T("Exception")
#define XCRASHREPORT_ERROR_LOG_FILE		_T("Exception.log")


#define FUNCTION_STACK_TRACE(x) \
	char * function_stack_trace_buffer = (char *) _alloca(25); \
	CopyMemory(function_stack_trace_buffer, x, 25);


#define MAX_FILE_PATH	200
#define MAX_FILE_NAME   32

#define HPRINTF_BUFFER_SIZE (8*1024)// must be at least 2048

#define NUMBER_OF_CODE_BYTES 16  	// Number of code bytes to record.
#define MAX_STACK_DUMP		 3072	// Maximum number of DWORDS in stack dumps.
#define STACK_COLUMNS        4		// Number of columns in stack dump.
#define MAX_DUMP_FILES		 10

#ifndef VER_PLATFORM_WIN32s
#define VER_PLATFORM_WIN32s             0
#endif
#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS      1
#endif
#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT           2
#endif
#ifndef VER_PLATFORM_WIN32_CE
#define VER_PLATFORM_WIN32_CE           3
#endif


//#define _countof(array) (sizeof(array)/sizeof(array[0]))


//Windows versions
#define WUNKNOWNSTR	_T("unknown Windows version")

#define W95STR			_T("Windows 95")
#define W95SP1STR		_T("Windows 95 SP1")
#define W95OSR2STR		_T("Windows 95 OSR2")
#define W98STR			_T("Windows 98")
#define W98SP1STR		_T("Windows 98 SP1")
#define W98SESTR		_T("Windows 98 SE")
#define WMESTR			_T("Windows ME")

#define WNT351STR		_T("Windows NT 3.51")
#define WNT4STR			_T("Windows NT 4")
#define W2KSTR			_T("Windows 2000")
#define WXPSTR			_T("Windows XP")
#define W2003SERVERSTR	_T("Windows 2003 Server")

#define WCESTR			_T("Windows CE")


#define WUNKNOWN	0

#define W9XFIRST	1
#define W95			1
#define W95SP1		2
#define W95OSR2		3
#define W98			4
#define W98SP1		5
#define W98SE		6
#define WME			7
#define W9XLAST		99

#define WNTFIRST	101
#define WNT351		101
#define WNT4		102
#define W2K			103
#define WXP			104
#define W2003SERVER	105
#define WNTLAST		199

#define WCEFIRST	201
#define WCE			201
#define WCELAST		299


#define	ONEK			1024
#define	SIXTYFOURK		(64*ONEK)
#define	ONEM			(ONEK*ONEK)
#define	ONEG			(ONEK*ONEK*ONEK)


#endif