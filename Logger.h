// Logger.h: interface for the CLogger class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __LOGGER_H__ 
#define __LOGGER_H__ 


#include "Log.h"
//#define _DEBUG_LOCATION 1

#define LOG_LEVEL_0		(DWORD)0 // LOG Only errors/exceptions in server operation
#define LOG_LEVEL_1		(DWORD)1 // Same as 0 + logic (business rules) false results  
#define LOG_LEVEL_2		(DWORD)2 // Same as 1 + basic flow control reporting
#define LOG_LEVEL_3		(DWORD)3 // Same as 2 + detailed flow control reporting
#define DEFAULT_LOG_LEVEL LOG_LEVEL_3
#define LOG_LEVEL_4		(DWORD)4 // Same as 2 + detailed flow control reporting + fcc 
#define LOG_LEVEL_5     (DWORD)5 // Open output console.

#define LOG_ERROR				(_Module.m_server.m_cLogger.GetLogLevel() >= LOG_LEVEL_0)
#define LOG_LOGIC_ERROR			(_Module.m_server.m_cLogger.GetLogLevel() >= LOG_LEVEL_1)
#define LOG_BASIC_FLOW_CONTROL	(_Module.m_server.m_cLogger.GetLogLevel() >= LOG_LEVEL_2)			
#define LOG_DETAIL_FLOW_CONTROL	(_Module.m_server.m_cLogger.GetLogLevel() >= LOG_LEVEL_3)			
#define LOG_FCC_FLOW_CONTROL	(_Module.m_server.m_cLogger.GetLogLevel() >= LOG_LEVEL_4)
#define LOG_ALL_FLOW_CONTROL	(_Module.m_server.m_cLogger.GetLogLevel() >= LOG_LEVEL_5)


//exception  log flags 
#define	 EXCEPTION_MESS_FORCE_REMOVE		1
#define	 EXCEPTION_MESS_SHIFT_FORCE_REMOVE	2

//Type of log line 
#define LOG_NONE		0 
#define LOG_PUMP		1 
#define LOG_TERM		2   
#define LOG_POS 		3   

#define SYS_DOWN_STR_LEN	12
#define SYS_DOWN			"System Down"
#define SYS_UP				"System Up"

#define MSG_MAX_BUFFER		2048


class CLogger : public CLog 
{
protected:
	DWORD	 			m_dwLogLevel;
	BYTE				m_sLastLogDay[4]; // Examp. ".12" 

	BOOL				m_IsLoggerInitialised;

public:	
	void	LogMsgOLAStartSession(long lPumpNumber,char *sInBuffer, char *sOlaType,long lInBufferSize);
	void    LogSystemDownData(long lState = -1); //Shmilo 03-03-2003 //4.0.2.37
	void	LogMiddleOfProcessReason();
	long	LogLocationOut(LPCSTR szLocation,long lLevel = 1,long lPosNumber = 0);
	long	LogLocationIn(LPCSTR szLocation,long lLevel = 1,long lPosNumber = 0);
	long	LogOLAMsg(long lPumpNumber, short byStat, long lSessionId, long lLevel = DEFAULT_LOG_LEVEL);  //4.0.9.507
	long    LogOLAMsg(long lPumpNumber,short byStat,long lSessionId, CString strMsg,long lLevel = DEFAULT_LOG_LEVEL); //4.0.9.507
	BOOL	SwapFile();
	void	Init();
	DWORD	GetLogLevel();
	void	SetLogLevel(DWORD dwLogLevel);
	long	LogGCIReport(long lPumpNumber, BOOL bErrorRep, long lError, BOOL bWarningRep, long lWarning, BOOL bDeviceRep, long lDevice, long lDeviceCode, BOOL bMiscRep, long lMisc);

	CString	GetDefaultLogPath() {return m_sDefualtLogPath;}

	//Eyal 4.0.0.62 start added default BOOL for console writing
	long	LogResult(long lSrc,LPCSTR szInterfaceName, LPCSTR szReq, BOOL bLogicResult, long lRetCode, BOOL bComResult, HRESULT hRes , BOOL bConsoleMsg = TRUE);
	long	LogResult(long lSrc,LPCSTR szInterfaceName, LPCSTR szReq, BOOL bLogicResult, long lRetCode, BOOL bComResult, HRESULT hRes,long lParam , BOOL bConsoleMsg = TRUE);
	long	LogResult(long lSrc,LPCSTR szInterfaceName, LPCSTR szReq, BOOL bLogicResult, long lRetCode, BOOL bComResult, HRESULT hRes,_com_error & e , BOOL bConsoleMsg = TRUE);
	long	LogMsg(CString strMsg,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);
	long	LogMsg(LPCSTR szMsg,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);

	//	long				LogMsg(const CString & strMsg,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);

	long				LogMsg(const CString &  sClassName, const CString &  sFuncName, long lPumpNum, long lLogType, const CString &  strMsg, long lLevel = DEFAULT_LOG_LEVEL, BOOL bConsoleMsg = TRUE);
	long				LogMsg(long lNum, long lLogType, CString strMsg, long lLevel = DEFAULT_LOG_LEVEL, BOOL bConsoleMsg = TRUE);
	long				LogMsg(long lNum1, long lLogType1, long lNum2, long lLogType2, const CString &  strMsg, long lLevel = DEFAULT_LOG_LEVEL, BOOL bConsoleMsg = TRUE);

	CString GetFinalLineMsg(short nLinkFlags);

	CString AddNumInfo(long lNum,long lLogType,CString strMsg);

	long    LogMsgWithRet(long lPumpNumber,long lLogType,CString strMsg,LPCSTR sSource,long lRetCode,long lLevel = DEFAULT_LOG_LEVEL,BOOL bConsoleMsg = TRUE);

	long	LogOLABufferMsg(long lNum,long lLogType,CString strMsg,LPCSTR sBuffer,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);
	long	LogOLABufferMsg(CString strMsg,LPCSTR sBuffer,long lLevel = DEFAULT_LOG_LEVEL, BOOL bConsoleMsg = TRUE);
	long	LogXmlOLABufferMsg(long lNum,long lLogType,CString strMsg,CXMLInterface* cXmlBuffer,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);
	long	LogXmlOLABufferMsg(CString strMsg,CXMLInterface* cXmlBuffer,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);

	long	LogPumpResult(long lSrc,LPCSTR szInterfaceName, LPCSTR szReq, BOOL bLogicResult, long lRetCode, BOOL bComResult, HRESULT hRes , BOOL bConsoleMsg = TRUE);

	long	LogClassMsg(const char* szClassName,const char* szFuncName,long lPumpNum,long lLogType,const char* szMsg,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);	//4.0.19.502
	long	LogFlagsMsg(long lPumpNum,long lLogType,const char* strMsg,long lFlags,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);

	long	LogPumpMsg(CString sClassName,CString sFuncName,long lPumpNum,CString strMsg,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);
	
	long	LogReq(long lSrc,LPCSTR szInterfaceName, LPCSTR szReq, LPCSTR szComment , BOOL bConsoleMsg = TRUE) ;
	//Eyal 4.0.0.62 end
	
	long    LogPumpStatus(long lPump, long lStatus, long lRes, long lResExt, long lRetCode);

	long	LogCOMError(long lPumpNumber ,LPCSTR szMethodName, HRESULT hRes, int iRetry);

	HANDLE	OpenReadLogFile();
	long	SystemUp();
	long	SystemDown();
	long	FindAndWriteSystemDownTime(HANDLE hSysDownFile);

	CLogger();
	virtual ~CLogger();

	virtual void RaiseDiskWarning(long lWarning);

	//4.0.1.38
	virtual void InitWorkingDirectory(char * sPath = NULL);

	long	LogMsg2(long lNum,long lLogType,const char* szMsg,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);
	long	LogMsg2(long lNum1,long lLogType1,long lNum2,long lLogType2, char* szMsg,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);
	long	LogMsg2(const char* szClassName,const char* szFuncName,long lPumpNum,long lLogType,const char* szMsg,long lLevel = DEFAULT_LOG_LEVEL , BOOL bConsoleMsg = TRUE);
	CString GetStateString(int iState) const;

	DWORD				GetExceptionLogLevel()  {return m_dwExceptionLogLevel;} // TD 371850 (Merges from TD 356420)
private:
	BOOL m_bPciActive;
	CString m_sSiteId; //4.0.2.37
	char m_sSysDownLogFileName[MAX_FILE_PATH];  //eran 4.0.2.37 // No TD 4.0.28.501
	char m_sSysDownLogFilePath[MAX_FILE_PATH];  //eran 4.0.2.37
	DWORD				m_dwExceptionLogLevel; // TD 371850 (Merges from TD 356420)

	long GetReturnCodeIDR(const char * sSource , long lRetCode);
	int GetReturnCodeDescription(LPCSTR sSource , long lRetCode , LPSTR sDesc , int iLen);
	void AddNumInfo2(long lNum, long lLogType,const char* szMsg, char* sOutMsg,int iBufferSize);
	
	
};


class CShiftLog : public CLog  
{
public:
	void Init();

	CShiftLog();
	virtual ~CShiftLog();
	BOOL SwapFile();
	long LogTrs(CPumpTransact *pTrs, BOOL bCancel, BOOL bRemove = FALSE , BOOL bPosVoid  = FALSE ,long lCarWash=0);

	//4.0.1.38
	virtual void InitWorkingDirectory(char * sPath = NULL);
	
protected:
	long	m_lFormalShiftNumber;
};


class CExceptionLog : public CLog  
{
public:	
	void Init();

	CExceptionLog();
	virtual ~CExceptionLog();
	BOOL SwapFile();
	long LogTrs(CPumpTransact *pTrs, CARD_SALE_DATA *pData,CARD_SALE_EXTRA_DATA *pExtraData , long lFlags=0);
	long LogTrs(CPumpTransact *pTrs, long lFlags=0);
	
	//4.0.1.38
	virtual void InitWorkingDirectory(char * sPath = NULL);
	
protected:
	BYTE				m_sLastLogMonth[4]; // Examp. ".12" 
};




#endif 
