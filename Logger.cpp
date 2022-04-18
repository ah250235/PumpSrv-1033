// Logger.cpp: implementation of the CLogger class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Logger.h"	
#include "ProductVersion.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//Eyal 4.0.0.22
//CLogger
//CLASS WIDE CHANGES:
//In every call to WriteLogFile, lRetVal has been added.
//After every call to WriteLogFile, lRetVal is tested, and
//a disk warning is raised if necessary.

//Eyal 4.0.0.42
//CLogger
//CLASS WIDE CHANGES
//Changed disk warnings from Miscs to SystemError

CLogger::CLogger()
{
	Init();	// locating the init at the constructor - 
	// make sure that the logger is ready before 
	// ServerMain is constructed.
}

CLogger::~CLogger()
{
	
	CloseLogFile();
	//LogMsg("Distructor Logger");//4.0.3.45
}

void CLogger::Init()
{
	m_sBaseFilePath.Empty();
	m_sBaseFilePath += "C:\\PumpSrv";
	
	char sTmp[MAX_FILE_PATH] = {0};


	//4.0.1.38 added parameter lParamNotLoaded to make code more readable
	long lParamNotLoaded = _Module.m_server.m_cParam.GetParam("Paths",1,0,
		"LogDir",FIELD_SZ, 
		(char *)sTmp,
		MAX_FILE_PATH,NULL,FALSE);

	m_sBaseFilePath = sTmp;
	
	//Eyal 4.0.0.53 start.  The order of the conditions is imortant.
	if (lParamNotLoaded || (!strcmp((char*)m_sBaseFilePath.GetBuffer(0) , "")))
	{		
		m_sBaseFilePath.Empty();
		char sTmp[MAX_FILE_PATH];
		_Module.m_server.m_cParam.GetParam("Paths",1,0,
			"HomeDir",FIELD_SZ, 
			(char *)sTmp,
			MAX_FILE_PATH,NULL,TRUE);
		m_sBaseFilePath.Format("%s", sTmp);
	}
	
	//If we don't have a valid name, we attempt to create the log dir in the
	//working directory where the EXE file is.
	if(!strcmp((char*)m_sBaseFilePath.GetBuffer(0) , ""))
	{
		//4.0.1.38
		char sTmp[MAX_FILE_PATH];
		DWORD sNameLen = GetProcessName((LPSTR)sTmp , MAX_FILE_PATH);
		
		GetProcessNameDetails((LPSTR)sTmp,
			sNameLen,
			(LPSTR)m_sBaseFilePath.GetBuffer(),
			NULL);
	}
	//Eyal 4.0.0.53 end
	//4.0.2.37
	strcpy_s((char *)m_sSysDownLogFilePath,MAX_FILE_PATH,(char*)m_sBaseFilePath.GetBuffer(0));	

	m_sBaseFilePath += "\\Log";

	//4.0.0.38 Eyal Start
	BOOL bRes = CreateDirectory((const char*)m_sBaseFilePath.GetBuffer(0),     //directory to create
		NULL);                            //default security attributes
	//4.0.0.38 Eyal End

	m_sDefualtLogPath = m_sBaseFilePath.GetBuffer(0);
	m_sBaseFilePath += "\\PumpSrv";
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam","LogLevel"	, &m_dwLogLevel	, DEFAULT_LOG_LEVEL,TRUE,FALSE);
	
	m_sFilePath.Empty();
	m_sFilePath = m_sBaseFilePath;
	m_sFilePath += ".Log";

	m_sFileName.Format("PumpSrv.Log");

	char sTmpBuf[10];
	memset (sTmpBuf,0,sizeof(sTmpBuf));
	_strdate_s( sTmpBuf, 10);

	m_sLastLogDay[0] = '.';
	m_sLastLogDay[1] = sTmpBuf[3];
	m_sLastLogDay[2] = sTmpBuf[4];
	m_sLastLogDay[3] = 0;
	
	
	//Eyal 4.0.0.22 Start
	DWORD dwLogPercent;
	_Module.m_server.m_cParam.LoadParam("GeneralParam" ,
		"LogPercent"	,
		&dwLogPercent ,
		DEFAULT_LOG_PERCENT ,
		TRUE ,
		FALSE);
	
	ResetLogPercentage(dwLogPercent);
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam" ,
		"WarningPercent"	,
		&dwLogPercent ,
		DEFAULT_LOG_PERCENT ,
		TRUE ,
		FALSE);
	
	ResetWarningLevel(dwLogPercent);
	
	//Eyal 4.0.0.62 moved this call to CServerMain::InitServer()
	/*
	if(m_dwLogLevel == LOG_LEVEL_5)
	RedirectIOToConsole();
	*/
	
	//Eyal 4.0.0.22 End
	
	//4.0.1.38
	//4.0.2.37
    char tmpStr[100];
	memset(tmpStr,0,sizeof(tmpStr));
	_Module.m_server.m_cParam.LoadParam("GeneralParam","SiteID",tmpStr,sizeof(tmpStr),"",TRUE);
	m_sSiteId.Format("%s", tmpStr);
	InitWorkingDirectory();


	_Module.m_server.m_cParam.LoadParam("GeneralParam", "ExceptionHandlerLogLevel", &m_dwExceptionLogLevel, ExceptionLogLevelHigh, TRUE, FALSE);// TD 371850 (Merges from TD 356420)
	if (m_dwExceptionLogLevel > 0)
	{
		m_dwExceptionLogLevel = ExceptionLogLevelHigh;
	}
	else
	{
		m_dwExceptionLogLevel = ExceptionLogLevelNone;
	}

	//4.0.2.37
	
	//	LogSystemDownData();  //4.0.2.37
}

long CLogger::LogReq(long lSrc,LPCSTR szInterfaceName, LPCSTR szReq, LPCSTR szComment , BOOL bConsoleMsg) 
{
	
	CString str;
	long lRes;
	
	str.Format("%.30s (%s)",szReq,szComment);
	
	if (lSrc == 0)
	{
		lRes = LogMsg(str,DEFAULT_LOG_LEVEL,bConsoleMsg);
	}
	else
	{
		lRes = LogMsg(lSrc,LOG_POS,str,DEFAULT_LOG_LEVEL,bConsoleMsg);
	}
	
	return lRes;
}

/*
long CLogger::LogMsg(const CString &  strMsg,long lLevel , BOOL bConsoleMsg)
{
	return LogMsg((LPCSTR)strMsg,lLevel,bConsoleMsg);
}
*/

long CLogger::LogMsg(CString strMsg,long lLevel , BOOL bConsoleMsg)
{
	return LogMsg((LPCSTR)strMsg,lLevel,bConsoleMsg);
}

long CLogger::LogMsg(LPCSTR szMsg, long lLevel , BOOL bConsoleMsg)
{
	
	DWORD dwWrittenToFile = 0;
	long lRetVal = 0;
	SYSTEMTIME st;
	GetLocalTime(&st);
	
	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csLogFile);
	
	DWORD dwThreadId = GetCurrentThreadId();
	
	SwapFile();
	
	if(strlen(szMsg) > 4096)
	{
		m_sBuff.Format("%02d-%02d-%04d %02d:%02d:%02d:%03d (%05d) ...Exception 0.... %s %.4096s \r\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, dwThreadId, "<><><><> STRING SIZE BIGGER THAN 4096 BYTES <><><><><> ", szMsg);

	}
	
	else if(lLevel == LOG_LEVEL_0) 
	{		
		m_sBuff.Format("%02d-%02d-%04d %02d:%02d:%02d:%03d (%05d) ...Exception 0.... %s \r\n",
			st.wDay,
			st.wMonth,
			st.wYear,
			st.wHour,
			st.wMinute,
			st.wSecond,
			st.wMilliseconds,
			dwThreadId,
			szMsg);
		
	}
	
	else if(lLevel == LOG_LEVEL_1) 
	{
		m_sBuff.Format("%02d-%02d-%04d %02d:%02d:%02d:%03d (%05d) ...Exception 1.... %s \r\n",
			st.wDay,
			st.wMonth,
			st.wYear,
			st.wHour,
			st.wMinute,
			st.wSecond,
			st.wMilliseconds,
			dwThreadId,
			szMsg);
	}
	else 
	{
		m_sBuff.Format("%02d-%02d-%04d %02d:%02d:%02d:%03d (%05d) .................. %s \r\n",
			st.wDay,
			st.wMonth,
			st.wYear,
			st.wHour,
			st.wMinute,
			st.wSecond,
			st.wMilliseconds,
			dwThreadId,
			szMsg);	
	}
	
	DWORD dwWrittenToBuff = m_sBuff.GetLength();
	if (dwWrittenToBuff)
	{
		dwWrittenToFile = WriteLogFile(dwWrittenToBuff, &lRetVal, _Module.m_server.m_lDiskWarning); //4.0.2.38
		
		if((m_dwLogLevel == LOG_LEVEL_5) && (bConsoleMsg))
			ConsoleMsg(m_sBuff.GetBuffer(0));
		
		if(lRetVal == NO_SPACE)
			RaiseDiskWarning(SYS_ERROR_DISK_FULL);
		
		else
		{
			if(lRetVal == LOW_SPACE)
				RaiseDiskWarning(SYS_ERROR_DISK_WARNING);
			
			else //no error
				RaiseDiskWarning(SYS_NO_ERROR);
		}
	}		
	
	
	CloseLogFile();					// TD 227394
	
	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csLogFile);
	
	if (dwWrittenToFile != 0)
		return LOG_OK;
	else
		return LOG_NOT_OK;
}

/******************************************************************************
Description:	Perform LogMsg with the pump num / Term num in the prefix of the line.      	

  WHO			WHEN                    WHAT
  -------------------------------------------------------------------------------
  Ravit			01/11/2005   16:20		Start
  ******************************************************************************/
  long CLogger::LogMsg(const CString & sClassName, const CString & sFuncName, long lNum, long lLogType, const CString & strMsg, long lLevel, BOOL bConsoleMsg)
  {
	  CString str;
	  
	  switch(lLogType)
	  {
	  case LOG_PUMP:
		 // str.Format("Pump - %02ld, %s",lNum,strMsg);
		  str.Format("Pump - %02ld, %s::%s, %s",lNum,sClassName,sFuncName,strMsg);
		  break;
	  case LOG_TERM:
		  str.Format("Term - %02ld, %s",lNum,strMsg);
		  //			str.Format("Term - %02ld, %s::%s, %s",lNum,sClassName,sFuncName,strMsg);
		  break;
	  case LOG_POS:
		  str.Format("Pos  - %02ld, %s",lNum,strMsg);
		  break;
	  case LOG_NONE:
		  str.Format("%s",strMsg);
		  //			str.Format("%s::%s, %s",sClassName,sFuncName,strMsg);
		  break;
	  default:	
		  break;
	  }
	  
	  return LogMsg(str,lLevel,bConsoleMsg);
  }
  
  long CLogger::LogMsg(long lNum,long lLogType,CString strMsg,long lLevel,BOOL bConsoleMsg)
  {
	  CString str;
	  
	  str = AddNumInfo(lNum,lLogType,strMsg);
	  
	  return LogMsg(str,lLevel,bConsoleMsg);
  }
  
  long CLogger::LogMsg(long lNum1,long lLogType1,long lNum2,long lLogType2, const CString & strMsg,long lLevel , BOOL bConsoleMsg)
  {
	  
	  CString str;
	  
	  str = AddNumInfo(lNum2,lLogType2,strMsg);
	  str = AddNumInfo(lNum1,lLogType1,str);
	  
	  return LogMsg(str,lLevel,bConsoleMsg);
  }
  
  CString CLogger::AddNumInfo(long lNum,long lLogType,CString strMsg)
  {
	  CString str;
	  
	  switch(lLogType)
	  {
	  case LOG_PUMP:
		  str.Format("Pump - %02ld, %s",lNum,strMsg);
		  break;
	  case LOG_TERM:
		  str.Format("Term - %02ld, %s",lNum,strMsg);
		  break;
	  case LOG_POS:
		  str.Format("Pos  - %02ld, %s",lNum,strMsg);
		  break;
	  case LOG_NONE:
		  str.Format("%s",strMsg);
		  break;
	  default:	
		  break;
	  }
	  
	  return str;
	  
  }
  
/*  long CLogger::LogClassMsg(CString sClassName,CString sFuncName,long lNum,long lLogType,CString strMsg,long lLevel,BOOL bConsoleMsg)
  {
	  CString str;
	  
	  switch(lLogType)
	  {
	  case LOG_PUMP:
		  str.Format("Pump - %02ld, %s::%s, %s",lNum,sClassName,sFuncName,strMsg);
		  break;
	  case LOG_TERM:
		  str.Format("Term - %02ld, %s::%s, %s",lNum,sClassName,sFuncName,strMsg);
		  break;
	  case LOG_NONE:
		  str.Format("%s::%s, %s",sClassName,sFuncName,strMsg);
		  break;
	  default:	
		  break;
	  }
	  
	  return LogMsg(str,lLevel,bConsoleMsg);
  }
  
  
  long CLogger::LogFlagsMsg(long lNum,long lLogType,CString strMsg,long lFlags,long lLevel,BOOL bConsoleMsg)
  {
	  CString str;
	  str.Format("%s, Flags=%ld",strMsg,lFlags);
	  return LogMsg(lNum,lLogType,str,lLevel,bConsoleMsg);
  }
*/  
  CString CLogger::GetFinalLineMsg(short nLinkFlags)
  {
	  
	  CString str;
	  
	  switch(nLinkFlags)
	  {
	  case TRS_NO_LINK:
		  str.Format("End of PAK transaction");
		  break;
	  case TRS_LINK_PREPAY_NUMBER:
		  str.Format("End of PRP transaction");
		  break;
	  case TRS_LINK_OLA_NUMBER:
		  str.Format("End of PAP transaction");
		  break;
	  case TRS_LINK_PAK_AND_LOYALTY:
		  str.Format("End of PAK + Loyalty transaction");
		  break;
	  case TRS_LINK_PREPAY_NUMBER_AND_LOYALTY:
		  str.Format("End of Prepay + Loyalty transaction");
		  break;
	  case TRS_LINK_ATTEND:
		  str.Format("End of PAY ATTENDANT reservation");
		  break;
	  case TRS_LINK_CASH_ACCEPTOR_INFO:
		  str.Format("End of Cash acceptor transaction");
		  break;
	  case TRS_LINK_FLEET:
		  str.Format("End of PrePay fleet transaction");
		  break;
	  default:
		  str.Format("End of transaction");
		  break;
	  }
	  
	  return str;
  }
  
  long CLogger::LogResult(long lSrc, LPCSTR szInterfaceName, LPCSTR szReq, BOOL bLogicResult, long lRetCode, BOOL bComResult, HRESULT hRes,long lParam , BOOL bConsoleMsg)
  {
	  
	  CString str;
	  CString strTmp;
	  
	  long lRes;
	  
	  strTmp.Format("%.20s (%d) ",szReq,lParam);			
	  str = strTmp;
	  
	  if( bLogicResult)
	  {
		  //4.0.1.38 start
		  char sRetCodeDescription[128];
		  memset((char*)sRetCodeDescription , 0 , sizeof(sRetCodeDescription));
		  GetReturnCodeDescription(szInterfaceName , lRetCode , sRetCodeDescription , sizeof(sRetCodeDescription));
		  
		  str.Format("%s Logic result: %ld (%s)",strTmp,lRetCode,sRetCodeDescription);
		  strTmp = str; 
		  //4.0.1.38 end
	  }
	  
	  if( bComResult)
	  {
		  str.Format("%s COM Result: 0x%x ",strTmp,hRes);
	  }
	  
	  if (lSrc == 0)
	  {
		  lRes = LogMsg(str,DEFAULT_LOG_LEVEL,bConsoleMsg);
	  }
	  else
	  {
		  lRes = LogMsg(lSrc,LOG_POS,str,DEFAULT_LOG_LEVEL,bConsoleMsg);
	  }
	  
	  return lRes;
}

long CLogger::LogResult(long lSrc,LPCSTR szInterfaceName, LPCSTR szReq, BOOL bLogicResult, long lRetCode, BOOL bComResult, HRESULT hRes, BOOL bConsoleMsg)
{
	
	CString str;
	CString strTmp;
	
	long lRes;
	
	strTmp.Format("%.20s ",szReq);	
	str = strTmp;
	
	if( bLogicResult)
	{
		//4.0.1.38 start
		char sRetCodeDescription[128];
		memset((char*)sRetCodeDescription , 0 , sizeof(sRetCodeDescription));
		GetReturnCodeDescription(szInterfaceName , lRetCode , sRetCodeDescription , sizeof(sRetCodeDescription));
		
		str.Format("%s Logic result: %ld (%s)",strTmp,lRetCode,sRetCodeDescription);
		strTmp = str; 
		//4.0.1.38 end
	}
	
	if( bComResult)
	{
		str.Format("%s COM Result: 0x%x ",strTmp,hRes);
	}
	
	if (lSrc == 0)
	{
		lRes = LogMsg(str,DEFAULT_LOG_LEVEL,bConsoleMsg);
	}
	else
	{
		lRes = LogMsg(lSrc,LOG_POS,str,DEFAULT_LOG_LEVEL,bConsoleMsg);
	}
	
	return lRes;
}

long CLogger::LogResult(long lSrc,LPCSTR szInterfaceName, LPCSTR szReq, BOOL bLogicResult, long lRetCode, BOOL bComResult, HRESULT hRes,_com_error & e, BOOL bConsoleMsg)
{
	
	CString str;
	CString strTmp;
	
	long lRes;
	
	strTmp.Format("Exception !!! %.30s, %.60s",szReq,e.ErrorMessage());			
	str = strTmp;
	
	if( bLogicResult)
	{
		//4.0.1.38 start
		char sRetCodeDescription[128];
		memset((char*)sRetCodeDescription , 0 , sizeof(sRetCodeDescription));
		GetReturnCodeDescription(szInterfaceName , lRetCode , sRetCodeDescription , sizeof(sRetCodeDescription));
		
		str.Format("%s Logic result: %ld (%s)",strTmp,lRetCode,sRetCodeDescription);
		strTmp = str; 
		//4.0.1.38 end
	}
	
	if( bComResult)
	{
		str.Format("%s COM Result: 0x%x ",strTmp,hRes);
	}
	
	if (lSrc == 0)
	{
		lRes = LogMsg(str,DEFAULT_LOG_LEVEL,bConsoleMsg);
	}
	else
	{
		lRes = LogMsg(lSrc,LOG_POS,str,DEFAULT_LOG_LEVEL,bConsoleMsg);
	}
	
	return lRes;
}

long CLogger::LogPumpResult(long lSrc,LPCSTR szInterfaceName, LPCSTR szReq, BOOL bLogicResult, long lRetCode, BOOL bComResult, HRESULT hRes, BOOL bConsoleMsg)
{
	
	CString str;
	CString strTmp;
	
	long lRes;
	
	strTmp.Format("%.20s ",szReq);	
	str = strTmp;
	
	if( bLogicResult)
	{
		//4.0.1.38 start
		char sRetCodeDescription[128];
		memset((char*)sRetCodeDescription , 0 , sizeof(sRetCodeDescription));
		GetReturnCodeDescription(szInterfaceName , lRetCode , sRetCodeDescription , sizeof(sRetCodeDescription));
		
		str.Format("%s Logic result: %ld (%s)",strTmp,lRetCode,sRetCodeDescription);
		strTmp = str; 
		//4.0.1.38 end
	}
	
	if( bComResult)
	{
		str.Format("%s COM Result: 0x%x ",strTmp,hRes);
	}
	
	if (lSrc == 0)
	{
		lRes = LogMsg(str,DEFAULT_LOG_LEVEL,bConsoleMsg);
	}
	else
	{
		lRes = LogMsg(lSrc,LOG_PUMP,str,DEFAULT_LOG_LEVEL,bConsoleMsg);
	}
	
	return lRes;
}


long CLogger::LogMsgWithRet(long lPumpNumber,long lLogType,CString strMsg,LPCSTR sSource,long lRetCode,long lLevel,BOOL bConsoleMsg)
{
	CString str;
	long lRes;
	
	char sRetCodeDescription[MAX_FIELD_VAL];
	memset((char*)sRetCodeDescription , 0 , sizeof(sRetCodeDescription));
	GetReturnCodeDescription(sSource,lRetCode,sRetCodeDescription,sizeof(sRetCodeDescription));
	
	str.Format("%s, %s RetCode = %ld: %s",strMsg,sSource,lRetCode,sRetCodeDescription);
	
	lRes = LogMsg(lPumpNumber,lLogType,str,lLevel,bConsoleMsg);
	
	return lRes;
}

long CLogger::LogOLABufferMsg(long lNum,long lLogType, CString strMsg, LPCSTR sBuffer, long lLevel, BOOL bConsoleMsg)
{
	CString str;
	long lPartsNum;
	long lRes; 
	
	str.Format("%s %.1000s", strMsg, sBuffer);
	lRes = LogMsg(lNum, lLogType, str, lLevel, bConsoleMsg);
	
	lPartsNum = (strlen(sBuffer) / 1000) + 1;
	
	for (int i = 1; i < lPartsNum; i++)
	{
		str.Format("%.1000s", sBuffer + (1000 * i));
		lRes = LogMsg(lNum, lLogType, str, lLevel, bConsoleMsg);
	}
	
	return lRes;
}

long CLogger::LogOLABufferMsg(CString strMsg,LPCSTR sBuffer,long lLevel, BOOL bConsoleMsg)
{
	return LogOLABufferMsg(0,LOG_NONE,strMsg,sBuffer,lLevel,bConsoleMsg);
}

long CLogger::LogXmlOLABufferMsg(long lNum,long lLogType,CString strMsg,CXMLInterface* cXmlBuffer,long lLevel,BOOL bConsoleMsg)
{
	char sTmpBuff[MAX_XML_BUF_LEN];
	char sVal[MAX_FIELD_VAL];
	CXMLInterface cTmpXml;
	const long lSizeOfParamResult = 113;
	
	memset(&sTmpBuff,0,sizeof(sTmpBuff));
	cXmlBuffer->GetXmlString((char*)sTmpBuff);
	
	if (_Module.m_server.m_cProtectedDataHandler.IsNeedToRemoveCardDataFromLog()) //4.0.22.501 TD 72716
	{
		cTmpXml.InitDoc();
		
		cTmpXml.LoadXmlString((char*)sTmpBuff);
		
		memset(&sVal,' ',sizeof(sVal));
		cTmpXml.SetXmlFieldWithSize(ATT_TRACK2_DATA,sVal,TRACK_2_DATA_SIZE);
		
		memset(&sVal,0,sizeof(sVal));
		cTmpXml.SetXmlFieldWithSize(ATT_TRACK1_DATA,sVal,TRACK_1_DATA_SIZE);
		
		if(!_Module.m_server.m_cProtectedDataHandler.GetExcludesLoyalty())			//4.0.22.501 TD 72716
			cTmpXml.SetXmlFieldWithSize(ATT_CLUB_CARD_TRACK2_DATA,sVal,TRACK_2_DATA_SIZE);

		cTmpXml.SetXmlFieldWithSize(ATT_ACCOUNT_NUMBER,sVal,ACCOUNT_NUM_SIZE);
		cTmpXml.SetXmlFieldWithSize(ATT_PIN_BLOCK,sVal,PIN_BLOCK_SIZE);  //4.0.17.500
		cTmpXml.SetXmlFieldWithSize(ATT_PARAM_RESULT,sVal,lSizeOfParamResult);  //4.0.20.52
		
		memset(&sTmpBuff,0,sizeof(sTmpBuff));
		cTmpXml.GetXmlString((char*)sTmpBuff);
		
		cTmpXml.CloseDoc();
	}
	
	return LogOLABufferMsg(lNum,lLogType,strMsg,sTmpBuff,lLevel,bConsoleMsg);
	
}

long CLogger::LogXmlOLABufferMsg(CString strMsg,CXMLInterface* cXmlBuffer,long lLevel,BOOL bConsoleMsg)
{
	return LogXmlOLABufferMsg(0,LOG_NONE,strMsg,cXmlBuffer,lLevel,bConsoleMsg);
}

DWORD CLogger::GetLogLevel()
{
	return m_dwLogLevel;
}

//Eyal 4.0.0.22 Start
void CLogger::SetLogLevel(DWORD dwLogLevel)
{
	CLog::SetLogLevel(m_dwLogLevel , dwLogLevel);
	
	m_dwLogLevel = dwLogLevel;
	
	//4.0.2.0
	CString sMsg;
	sMsg.Format("Log level changed to %d" , dwLogLevel);
	LogMsg(sMsg , LOG_LEVEL_0);
}
//Eyal 4.0.0.22 End

long CLogger::LogGCIReport(long lPumpNumber, BOOL bErrorRep, long lError, BOOL bWarningRep, long lWarning, BOOL bDeviceRep, long lDevice, long lDeviceCode, BOOL bMiscRep, long lMisc)
{
	DWORD dwWrittenToFile = 0;
	long lRetVal = 0;
	SYSTEMTIME st;
	GetLocalTime(&st);
	
	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csLogFile);
	
	DWORD dwThreadId = GetCurrentThreadId();
	
	SwapFile();
	
	if(OpenLogFile() != INVALID_HANDLE_VALUE)
	{		
		m_sBuff.Format("%02d-%02d-%04d %02d:%02d:%02d:%03d (%04d) Controller Report: ",
			st.wDay,
			st.wMonth,
			st.wYear,
			st.wHour,
			st.wMinute,
			st.wSecond,
			st.wMilliseconds,
			dwThreadId);

		if (lPumpNumber)
		{
			CString sTmp;
			sTmp.Format("Pump %02ld ", lPumpNumber);
			m_sBuff += sTmp;
		}

		if( bDeviceRep)
		{
			CString str;			
			switch(lDevice)
			{
				case NO_DEVICE:					str = "NO_DEVICE"; break;
				case TERMINAL:					str = "TERMINAL"; break;
				case TERMINAL_CARD_READER:		str = "TERMINAL_CARD_READER"; break;
				case TERMINAL_PRINTER:			str = "TERMINAL_PRINTER"; break;
				case TERMINAL_KEYBOARD:			str = "TERMINAL_KEYBOARD"; break;
				case TERMINAL_DISPLAY:			str = "TERMINAL_DISPLAY"; break;
				case TERMINAL_SPEAKER:			str = "TERMINAL_SPEAKER"; break;
				case TERMINAL_CASH_ACCEPTOR:	str = "TERMINAL_CASH_ACCEPTOR"; break;
				case DISPENSER:					str = "DISPENSER"; break;
					// start 2.9.2.0															
				case RFID_ANTENNA:				str = "RFID_ANTENNA"; break; 
					// end 2.9.2.0															
				
				default :						str = "UnKnown Device"; break;
			}
			
			CString sTmp;
			sTmp.Format(" Device(%d) %s ", lDevice, (LPCTSTR)str);
			m_sBuff += sTmp;
		}
		if( bErrorRep)
		{
			CString str;			
			if(lPumpNumber)
			{
				switch(lError)
				{
				case PUMP_NO_ERROR:				str = "NO_ERROR"; break;
				case PUMP_ERROR_NON_SPECIFIC:	str = "ERROR_NON_SPECIFIC"; break;
				case PUMP_ERROR_CONFIG:			str = "ERROR_CONFIG"; break;
				case PUMP_ERROR_BLENDER:		str = "ERROR_BLENDER"; break;
				case PUMP_ERROR_CASH_READER:	str = "ERROR_CASH_READER"; break;
				case PUMP_ERROR_CARD_READER:	str = "ERROR_CARD_READER"; break;
				default :						str = "UnKnown Error"; break;
				}
				
			}
			else // System Error 
			{
				switch(lError)
				{
				case SYS_NO_ERROR:				str = "SYS_NO_ERROR"; break;
				case SYS_ERROR_NON_SPECIFIC:	str = "SYS_ERROR_NON_SPECIFIC"; break;
				case SYS_ERROR_CONFIG:			str = "SYS_ERROR_CONFIG"; break;
				case SYS_ERROR_HW:				str = "SYS_ERROR_HW"; break;
				case SYS_ERROR_SW:				str = "SYS_ERROR_SW"; break;
				case SYS_ERROR_COMM:			str = "SYS_ERROR_COMM"; break;
				case SYS_ERROR_TANK_CONFIG:		str = "SYS_ERROR_TANK_CONFIG"; break;
				case SYS_ERROR_MISSING_TANK_READING:	str = "SYS_ERROR_MISSING_TANK_READING"; break;
				case SYS_ERROR_DELIVERY_FILE_FULL:		str = "SYS_ERROR_DELIVERY_FILE_FULL"; break;				
				case SYS_AUDIT_OFFLINE:			str = "SYS_AUDIT_OFFLINE"; break;
				case SYS_AUDIT_OUT_OF_PAPER:	str = "SYS_AUDIT_OUT_OF_PAPER"; break;
				case SYS_AUDIT_GENERAL_ERROR:	str = "SYS_AUDIT_GENERAL_ERROR"; break;
				case SYS_POWER_WARNING:			str = "SYS_POWER_WARNING"; break;
				case SYS_POWER_FAILED:			str = "SYS_POWER_FAILED"; break;
				case SYS_SHUTDOWN_POS:			str = "SYS_SHUTDOWN_POS"; break;
				case SYS_SHUTDOWN_POWER:		str = "SYS_SHUTDOWN_POWER"; break;
				case SYS_SHUTDOWN_TASK:			str = "SYS_SHUTDOWN_TASK" ;break;
				case SYS_SHUTDOWN_CONFIG:		str = "SYS_SHUTDOWN_CONFIG"; break;
				case SYS_SHUTDOWN_LEFT:			str = "SYS_SHUTDOWN_LEFT"; break;
				case SYS_PROCESS_LOCK:			str = "SYS_PROCESS_LOCK"; break;
				case SYS_ENCRYPTION_LOAD_FAILED: 
					str = " SYS_ENCRYPTION_LOAD_FAILED"; break;  
					// set flag to send key again....
				case SYS_ERROR_OLA:
					str = "CLOSE CONNECTION TO OLA !!!!!!!!!!!!!!!!"; break;
					
					
				default :						str = "UnKnown Sys Error"; break;
				}
			}
			
			CString sTmp;
			sTmp.Format(" Error(%d) %s ", lError, (LPCTSTR)str);
			m_sBuff += sTmp;
		}
		if( bWarningRep)
		{
			
			CString str;			
			
			switch(lWarning)
			{
			case PUMP_DEVICE_NO_WARNING:			str = "NO_WARNING"; break;
			case PUMP_DEVICE_WARNING_NON_SPECIFIC:	str = "WARNING_NON_SPECIFIC"; break;
			case PUMP_DEVICE_WARNING_PAPER_LOW:	str = "WARNING_PAPER_LOW"; break;
			case PUMP_DEVICE_WARNING_OUT_OF_PAPER:	str = "WARNING_OUT_OF_PAPER"; break;
			case PUMP_DEVICE_WARNING_PAPER_JAM:	str = "WARNING_PAPER_JAM"; break;
			case PUMP_DEVICE_WARNING_CASH_REJECTED: str = "WARNING_CASH_REJECTED"; break;
			case PUMP_DEVICE_WARNING_CASH_JAMED:				str = "WARNING_CASH_JAMED"; break;
			case PUMP_DEVICE_WARNING_CASH_CASSETTE_FULL:		str = "WARNING_CASH_CASSETTE_FULL"; break;
			case PUMP_DEVICE_WARNING_CASH_CASSETTE_REMOVED:	str = "WARNING_CASH_CASSETTE_REMOVED"; break;
			case PUMP_DEVICE_WARNING_CARD_ENCRYPTION:			str = "WARNING_CARD_ENCRYPTION"; break;
			case PUMP_DEVICE_WARNING_OFFLINE:					str = "WARNING_OFFLINE"; break;
				
				
			default :						str = "UnKnown Warning"; break;
			}
			
			CString sTmp;
			sTmp.Format(" Warning(%d) %s ", lWarning, (LPCTSTR)str);
			m_sBuff += sTmp;
		}
		
		if( bMiscRep )
		{
			CString str;			
			if(lPumpNumber)
			{
				//2.9.2.0 start
				if 	(lDevice == RFID_ANTENNA )
				{
					switch(lMisc)
					{
					case PUMP_MSG_TAG_CHALLANGE_FAIL:   str = "TAG CHALLANGE FAIL"; break;
					case PUMP_MSG_TAG_GONE:				str = "TAG GONE"; break;
					case PUMP_MSG_TAG_NOT_FOUND:		str = "TAG NOT FOUND"; break;
					case PUMP_MSG_TAG_CHANGED:			str = "TAG CHANGED"; break;
					case PUMP_MSG_CAR_TAG_READ:			str = "CAR TAG READ"; break;
					case PUMP_MSG_HAND_TAG_READ:		str = "HAND TAG READ"; break;
					case PUMP_MSG_TAG_ERROR:			str = "TAG ERROR"; break;
					case PUMP_MSG_TAG_READ:				str = "TAG READ"; break;
					case PUMP_MSG_GENERAL_SPEED_PASS_DECLINE:   str = "PUMP MSG GENERAL SPEED PASS DECLINE "; break;
					case PUMP_MSG_TAG_SALE_CANCELED:	str = "TAG SALE CANCELED"; break; //3.0.2.3
					case PUMP_MSG_TAG_SALE_DECLINED:	str = "TAG SALE DECLINED"; break; //3.0.2.3	
					default : 							str = "UnKnown Misc"; break;
					}
				}
				//2.9.2.0 end ..
				else
				{
					switch(lMisc)
					{
					case PUMP_MISC_EVENT_HELP_KEY:			str = "EVENT_HELP_KEY"; break;
					case PUMP_MISC_EVENT_CANCEL_KEY:		str = "EVENT_CANCEL_KEY"; break;
					case PUMP_MISC_EVENT_CARD_SWIPE_ERROR:	str = "PUMP_MISC_EVENT_CARD_SWIPE_ERROR"; break;
					default :								str = "UnKnown Misc"; break;
					}
				}
			}	
			
			CString sTmp;
			sTmp.Format(" Misc(%d) %s", lMisc, (LPCTSTR)str);
			m_sBuff += sTmp;
		}
		
		DWORD dwWrittenToBuff = m_sBuff.GetLength();
		if (dwWrittenToBuff)
		{
			m_sBuff += "\r\n";
			dwWrittenToFile = WriteLogFile(dwWrittenToBuff, &lRetVal, _Module.m_server.m_lDiskWarning); //4.0.2.38
			
			if(lRetVal == NO_SPACE)
				RaiseDiskWarning(SYS_ERROR_DISK_FULL);
			
			else
			{
				if(lRetVal == LOW_SPACE)
					RaiseDiskWarning(SYS_ERROR_DISK_WARNING);
				
				else //no error
					RaiseDiskWarning(SYS_NO_ERROR);
			}
		}					
	}
	
	CloseLogFile(); // TD 227394
	
	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csLogFile);
	
	if (dwWrittenToFile != 0)
		return LOG_OK;
	else
		return LOG_NOT_OK;
}

// See if the date changed and rename the log
BOOL CLogger::SwapFile()
{
	long lRetVal = 0;
	BOOL	bDone = FALSE;		
	char	sTmpBuf[10];
	
	memset (sTmpBuf,0,sizeof(sTmpBuf));
	_strdate_s( sTmpBuf );
	
	if (( m_sLastLogDay[2] != sTmpBuf[4] ) || (m_sLastLogDay[1] != sTmpBuf[3]))// check last digit of day in month
	{
		
		int iFailedToRename = ArchiveOldLogFile((LPSTR)sTmpBuf , (LPSTR)m_sLastLogDay);
		
		if(!iFailedToRename)
		{	
			m_sLastLogDay[1] = sTmpBuf[3];
			m_sLastLogDay[2] = sTmpBuf[4];
		
			
			SYSTEMTIME st;
			
			GetLocalTime(&st);
		
			m_sBuff.Format("%02d-%02d-%04d %02d:%02d:%02d:%03d .................. %s (Controller : %s)\r\n",
				st.wDay,
				st.wMonth,
				st.wYear,
				st.wHour,
				st.wMinute,								
				st.wSecond,     //4.0.3.41
				st.wMilliseconds,
				PUMPSRV_VERSION,
				_Module.m_server.m_strControllerName);		

			DWORD dwWrittenToBuff = m_sBuff.GetLength();
			if(dwWrittenToBuff)
			{
				WriteLogFile(dwWrittenToBuff , &lRetVal,_Module.m_server.m_lDiskWarning); //4.0.2.38
				
				if(lRetVal == NO_SPACE)
					RaiseDiskWarning(SYS_ERROR_DISK_FULL);
				
				else
				{
					if(lRetVal == LOW_SPACE)
						RaiseDiskWarning(SYS_ERROR_DISK_WARNING);
					
					//4.0.1.38
					else
						RaiseDiskWarning(SYS_NO_ERROR);
				}
			}						
			
		}
	}	
	return bDone;
}


long CLogger::LogOLAMsg(long lPumpNumber, short byStat, long lSessionId, long lLevel)
{
	CString tmpStr;	
	tmpStr.Empty();
	return LogOLAMsg(lPumpNumber, byStat, lSessionId, tmpStr,lLevel);
}

long CLogger::LogOLAMsg(long lPumpNumber, short byStat, long lSessionId, CString strMsg,long lLevel)
{
	LPCSTR szMsg = (LPCSTR)strMsg;
	LPCSTR szOlaMsg;
	SYSTEMTIME st;
	DWORD dwWrittenToBuff = 0;
	DWORD dwWrittenToFile = 0;
	long lRetVal = 0;
	
	GetLocalTime(&st);
	
	if (!_Module.m_server.m_cOLASrv->CanPrintLogMessages())
	{
		//return 0;
		return LOG_OK;
	}
	
	
	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csLogFile);
	
	DWORD dwThreadId = GetCurrentThreadId();
	
	SwapFile();
	
	if(OpenLogFile() !=  INVALID_HANDLE_VALUE)
	{
		CString str;
		str.Format("Pump - %02ld, >>> OLA, Session %d: ",lPumpNumber,lSessionId);
		switch(SESSION_TYPE(byStat))
		{
		case OLA_NONE:
			str += "NONE";
			break;
		case SESSION_AUTHORIZE:
			str += "AUTHORIZE";
			break;
		case SESSION_GET_RECEIPT:
			str += "GET_RECEIPT";
			break;
		case SESSION_RECEIPT_AUTH:
			str += "RECEIPT_AUTH";
			break;
		case SESSION_COMPLETE:
			str += "COMPLETE";
			break;
		case SESSION_RECEIPT_UPDATE:
			str += "RECEIPT_UPDATE";
			break;
		case SESSION_CANCEL:
			str += "CANCEL";
			break;
		case SESSION_CANCEL_AUTH:
			str += "CANCEL_AUTH";
			break;
		case SESSION_SALE: //3.0.1.61
			str += "SALE";
			break;
		case SESSION_GENERATE_REPORT:
			str += "GEN_REPORT";
			break;
		case SESSION_GET_REPORT:
			str += "GET_REPORT";
			break;
			//husky april start
		case SESSION_INIT_DEBIT:   //3.2.0.9
			str += "INIT_DEBIT";
			break;
		case SESSION_SHIFT_DEBIT: //3.2.0.9
			str += "SHIFT_DEBIT"; 
			break;
		case SESSION_CAPTURE:   //5.0.0.55
			str += "CAPTURE"; 
			break;
		case SESSION_CLOCK:   //5.0.0.55
			str += "CLOCK"; 
			break;				
		case SESSION_INQUIRY:
			str += "INQUIRY"; 
			break;
		case SESSION_REPORT_PRICE_CHANGE:
			str += "PRICE CHANGE"; 
			break;
		case SESSION_GET_CUSTOMER_INFO:
			str += "GET_CUSTOMER_INFO";
			break;
		case SESSION_SET_CUSTOMER_INFO:
			str += "SET_CUSTOMER_INFO";
			break;
		}
		
		str += ", State: ";
		switch (OLA_STATE(byStat))
		{
		case OLA_NONE:
			str += "NONE";
			break;
		case OLA_REQUEST_TO_SEND:
			str += "REQUEST_TO_SEND";
			break;
		case OLA_WAIT_RESPONSE:
			str += "WAIT_RESPONSE";
			break;
		case OLA_WAIT_FOR_MORE_INFO:
			str += "WAIT_FOR_MORE_INFO";
			break;
		case OLA_REQUEST_TO_SEND_MORE:
			str += "REQUEST_TO_SEND_MORE";
			break;
		case OLA_SEND_END:
			str += "SEND_END";
			break;
		case OLA_LOYALTY_WAIT_RESPONSE:
			str += "LOYALTY_WAIT_RESPONSE";
			break;
		case OLA_LOYALTY_START_TICKET:
			str += "LOYALTY_START_TICKET";
			break;
		}
		
		
		if(strlen(szMsg) > 4096)
		{
			m_sBuff.Format("%02d-%02d-%04d %02d:%02d:%02d:%03d (%05d) ...Exception 0.... %s %.4096s \r\n",
				st.wDay,
				st.wMonth,
				st.wYear,
				st.wHour,
				st.wMinute,
				st.wSecond,	
				st.wMilliseconds,
				dwThreadId,
				"<><><><> STRING SIZE BIGGER THAN 4096 BYTES <><><><><> ", 
				szMsg);	

			dwWrittenToBuff = m_sBuff.GetLength();
		}
		
		else if((lLevel == LOG_LEVEL_0)||(lLevel == LOG_LEVEL_1)) 
		{		
			szOlaMsg = (LPCSTR)str;
			m_sBuff.Format("%02d-%02d-%04d %02d:%02d:%02d:%03d (%05d) ...Exception %1d.... %s (%s)\r\n",
				st.wDay,
				st.wMonth,
				st.wYear,
				st.wHour,
				st.wMinute,
				st.wSecond,		
				dwThreadId,
				st.wMilliseconds,
				lLevel,
				szOlaMsg,			
				szMsg);		

			dwWrittenToBuff = m_sBuff.GetLength();
		}
		else 
		{
			szOlaMsg = (LPCSTR)str;
			m_sBuff.Format("%02d-%02d-%04d %02d:%02d:%02d:%03d (%05d) .................. %s (%s)\r\n",
				st.wDay,
				st.wMonth,
				st.wYear,
				st.wHour,
				st.wMinute,
				st.wSecond,	
				st.wMilliseconds,
				dwThreadId,
				szOlaMsg,
				szMsg);		

			dwWrittenToBuff = m_sBuff.GetLength();
		}
		
		if(dwWrittenToBuff)
		{
			dwWrittenToFile = WriteLogFile(dwWrittenToBuff , &lRetVal, _Module.m_server.m_lDiskWarning); //4.0.2.38
			
			if(m_dwLogLevel == LOG_LEVEL_5)
				ConsoleMsg(m_sBuff.GetBuffer(0));
			
			if(lRetVal == NO_SPACE)
				RaiseDiskWarning(SYS_ERROR_DISK_FULL);
			
			else
			{
				if(lRetVal == LOW_SPACE)
					RaiseDiskWarning(SYS_ERROR_DISK_WARNING);
				
				else //no error
					RaiseDiskWarning(SYS_NO_ERROR);
			}
		}		
	}
	
	CloseLogFile(); // TD 227394
	
	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csLogFile);
	
	if(dwWrittenToFile != 0)
		return LOG_OK;
	else
		return LOG_NOT_OK;	
}


//Eyal 4.0.0.22 Start
void CLogger::RaiseDiskWarning(long lWarning)
{
	long lRetVal = 0;
	//If the new error is an all O.K. flag, we update
	//the error field only if the previous error was a disk error.
	//If we didn't check this, we would cancel out other errors every
	//time the disk check would be successful.
	if(lWarning == SYS_NO_ERROR)
	{
		
		if(_Module.m_server.m_lDiskWarning == SYS_ERROR_DISK_FULL) 
		{
			//4.0.2.38
			if (_Module.m_server.GetAlarmSrvMode() == ALRMSRV_DIRECT_ACTIVE)
			{
				lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, SOFTWARE_DISK_FULL_ERR,0, 0,0,0);
			}
			
			_Module.m_server.m_lDiskWarning = lWarning; //4.0.2.23		  						
			
			//4.0.2.38			
		}
		else if (_Module.m_server.m_lDiskWarning == SYS_ERROR_DISK_WARNING)
		{
			//4.0.2.38
			if (_Module.m_server.GetAlarmSrvMode() == ALRMSRV_DIRECT_ACTIVE)
			{
				lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, SOFTWARE_DISK_FULL_WARNING,0, 0,0,0);
			}
			
			_Module.m_server.m_lDiskWarning = lWarning; //4.0.2.23	
			
			//4.0.2.38
			//_Module.m_server.m_lSysMisc = lWarning;   //4,0,20 //4.0.2.2
		}
		
		//FOR FURURE USE WITH ALARMSRV
		//_Module.m_server.WriteToLogMsg(0,0,0,0,0,0,0,0,TRUE, lWarning ,&lVal); //4.0.2.0
	}
	
	//If there is a disk error, we update anyway.
	else
	{
		if (lWarning != _Module.m_server.m_lDiskWarning)
		{
			if (_Module.m_server.GetAlarmSrvMode() == ALRMSRV_DIRECT_ACTIVE)
			{
				switch(lWarning)
				{
				case SYS_ERROR_DISK_FULL:
					{
						if (_Module.m_server.m_lDiskWarning == SYS_ERROR_DISK_WARNING)
						{
							lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, SOFTWARE_DISK_FULL_WARNING,0, 0,0,0);	
						}
						
						lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, SOFTWARE_DISK_FULL_ERR, 0,1, 0,0);
					}
					break;
				case SYS_ERROR_DISK_WARNING:
					{
						
						if (_Module.m_server.m_lDiskWarning == SYS_ERROR_DISK_FULL)
							lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, SOFTWARE_DISK_FULL_ERR,0, 0,0,0);
						
						lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, SOFTWARE_DISK_FULL_WARNING,0, 1,0,0);	
					}
					break;
				}
			}
		}
		
		_Module.m_server.m_lDiskWarning = lWarning; //4.0.2.23
	}
}
//Eyal 4.0.0.22 End

//4.0.1.38
void CLogger::InitWorkingDirectory(char * sPath)
{
	if(!sPath)
		strcpy_s((LPSTR)m_sWorkingPath ,MAX_FILE_PATH, (LPSTR)m_sBaseFilePath.GetBuffer(0));
	
	else
		strcpy_s((LPSTR)m_sWorkingPath ,MAX_FILE_PATH, (LPSTR)sPath);
	
	return;
}

//////////////////////////////////////////////////////////////////////
// CShiftLog Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShiftLog::CShiftLog()
{
	
}

CShiftLog::~CShiftLog()
{
	
}

void CShiftLog::Init()
{
	m_sBaseFilePath.Empty();
	m_sBaseFilePath += "C:\\PumpSrv";
	
	char sTmp[MAX_FILE_PATH] = {0};
	if (_Module.m_server.m_cParam.GetParam("Paths", 1, 0, "LogDir", FIELD_SZ, (char *)sTmp, MAX_FILE_PATH, NULL, FALSE))
	{
		m_sBaseFilePath.Empty();
		m_sBaseFilePath += "C:\\PumpSrv";
		_Module.m_server.m_cParam.GetParam("Paths",1,0,"HomeDir",FIELD_SZ, (char *)sTmp, MAX_FILE_PATH,NULL,TRUE);
	}
	
	m_sBaseFilePath = sTmp;
	m_sBaseFilePath += "\\Log\\Shift";

	m_sFilePath.Empty();
	m_sFilePath = m_sBaseFilePath;
	m_sFilePath += ".Log";
	
	m_lFormalShiftNumber = _Module.m_server.GetFormalShiftNumber();	
	
	//4.0.1.38
	InitWorkingDirectory();
	m_sFileName.Format("Shift.log");
}


BOOL CShiftLog::SwapFile()
{
	
	BOOL	bDone = FALSE;		
	
	if ( m_lFormalShiftNumber != _Module.m_server.GetFormalShiftNumber() )
	{
		
		BYTE	sLastShift[4]; // Examp. ".12" 
		memset (sLastShift,0,sizeof(sLastShift));		
		sLastShift[0] = '.';
		m_lFormalShiftNumber = m_lFormalShiftNumber%100;
		
		l2a0(m_lFormalShiftNumber, &sLastShift[1],2);
		
		//4.0.1.38
		/*
		BYTE sSaveFilePath[MAX_FILE_PATH];
		
		  memset(sSaveFilePath,0,sizeof(sSaveFilePath));
		  memcpy(sSaveFilePath, m_sBaseFilePath,sizeof(sSaveFilePath));
		  strcat((char *)sSaveFilePath,(char *)sLastShift);
		  
			// Delete it if exist 
			int iDelTmp = _unlink( (char *)sSaveFilePath );
			// Rename it
			int iResult = rename( (char *)m_sFilePath , (char *)sSaveFilePath );
		*/
		
		if(LOG_BASIC_FLOW_CONTROL)
		{	
			CString str;
			str.Format("Save shift file, File %s%s, ( %d )", m_sWorkingPath , sLastShift , m_lFormalShiftNumber );
			_LOGMSG.LogMsg(str);								
		}
		
		ArchiveOldLogFile(NULL , (LPSTR)sLastShift);
		
		m_lFormalShiftNumber = _Module.m_server.GetFormalShiftNumber();
		bDone = TRUE;
	}
	
	return bDone;
}

long CShiftLog::LogTrs(CPumpTransact *pTrs, BOOL bCancel, BOOL bRemove, BOOL bPosVoid , long lCarWash)
{
	DWORD dwWrittenToFile = 0;
	SYSTEMTIME st;
	GetLocalTime(&st);
	
	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csLogFile);
	
	DWORD dwThreadId = GetCurrentThreadId();
	
	SwapFile();

	if(OpenLogFile() != INVALID_HANDLE_VALUE)
	{
		CString strTrs;
		strTrs.Format("%02d-%02d-%04d %02d:%02d:%02d TRS %05d, ",
			st.wDay,
			st.wMonth,
			st.wYear,
			st.wHour,
			st.wMinute,
			st.wSecond,
			pTrs->m_lNumber);
		
		if(bRemove)
			strTrs += "(Remove), ";
		else if(bCancel)
			strTrs += "(Cancel), ";
		else if (bPosVoid)
			strTrs += "(Pos Void), ";
		else
			strTrs += "(Final ), ";
		
		switch(pTrs->m_lRes)
		{
		case ATTEND:	
			strTrs += "ATTEND        ";
			break;
		case TEST:		
			strTrs += "TEST          ";
			break;
		case PAY_AT_PUMP:				
			// start 3.1.1.6
			if (pTrs->m_lResExt == RES_EXT_AVI)
				strTrs += "AVI           ";		
			else if (pTrs->m_lResExt == RES_EXT_MOBILE)//CR 444830
			{
				strTrs += "PAY_AT_MOBILE ";
			}
			else
				// End 3.1.1.6
				strTrs += "PAY_AT_PUMP   ";
			break;
		case PAY_AT_KIOSK:
			if (pTrs->m_lResExt == RES_EXT_ATTENDANT)  //4.0.1.37
			{
				strTrs += "ATTENDANT     ";						
			}
			else if (pTrs->m_lResExt == RES_EXT_PAK_AUTO_RECONCILE)  //4.0.7.504
				strTrs += "PAK RECONCILE  ";
			else
				strTrs += "PAY_AT_KIOSK  ";
			break;
		case PAY_AT_OPT:	
			strTrs += "PAY_AT_OPT    ";
			break;
		case PRE_PAY_AT_POS:
			strTrs += "PRE_PAY_AT_POS";				
			break;
		case NULL_RES:
		default:
			strTrs += "NULL_RES      ";
			break;
			
		}
		
		CString sTmpStr;
		sTmpStr.Format(", Pump %02d, Grade %02d, Volume %06d, Value %06d",pTrs->m_lPump, pTrs->m_nGrade, pTrs->m_lRoundedVolume, pTrs->m_lRoundedValue);
		strTrs += sTmpStr;
		
		m_sBuff.Format("%s\r\n", (LPCSTR)strTrs);
		DWORD dwWrittenToBuff = m_sBuff.GetLength();
		if (dwWrittenToBuff)
		{	
			long lTmpRet=0;  //4.0.2.38
			dwWrittenToFile = WriteLogFile(dwWrittenToBuff, &lTmpRet, _Module.m_server.m_lDiskWarning); //4.0.2.38
		}							
	}
	
	CloseLogFile(); // TD 227394
	
	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csLogFile);
	
	//4.0.3.39 - calling the TrsLogger
	if(LOG_TRS_FILE)
		_Module.m_server.m_cTrsLog.LogTrs(pTrs);
	
	if (ACCUMULATE_SHIFT_TOTAL)
		_Module.m_server.m_cAccumulationShiftTotal.AddAccumulatedTrs(*pTrs, bPosVoid ,lCarWash );  //4.0.13.501
	
	if (dwWrittenToFile != 0)
		return LOG_OK;
	else
		return LOG_NOT_OK;
}


//4.0.1.38
void CShiftLog::InitWorkingDirectory(char * sPath)
{
	if(!sPath)
		strcpy_s((LPSTR)m_sWorkingPath ,MAX_FILE_PATH, (LPSTR)m_sBaseFilePath.GetBuffer());
	
	else
		strcpy_s((LPSTR)m_sWorkingPath ,MAX_FILE_PATH, (LPSTR)sPath);
	
	return;
}

//////////////////////////////////////////////////////////////////////
// CExceptionLog Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExceptionLog::CExceptionLog()
{
	Init();
}

CExceptionLog::~CExceptionLog()
{
	
}

void CExceptionLog::Init()
{
	m_sBaseFilePath.Empty();
	m_sBaseFilePath += "C:\\PumpSrv";
	
	char sTmp[MAX_FILE_PATH] = {0};
	if (_Module.m_server.m_cParam.GetParam("Paths",1,0,"LogDir",FIELD_SZ, (char *)sTmp, MAX_FILE_PATH,NULL,FALSE))
	{
		m_sBaseFilePath.Empty();
		m_sBaseFilePath += "C:\\PumpSrv";
		_Module.m_server.m_cParam.GetParam("Paths",1,0,"HomeDir",FIELD_SZ, (char *)sTmp, MAX_FILE_PATH,NULL,TRUE);
	}

	m_sBaseFilePath = sTmp;
	m_sBaseFilePath += "\\Log\\Exception";
	

	m_sFilePath.Empty();
	m_sFilePath = m_sBaseFilePath;
	m_sFilePath += ".Log";
	
	char sTmpBuf[10];
	memset (sTmpBuf,0,sizeof(sTmpBuf));
	_strdate_s( sTmpBuf );
	m_sLastLogMonth[0] = '.';
	m_sLastLogMonth[1] = sTmpBuf[0];
	m_sLastLogMonth[2] = sTmpBuf[1];
	m_sLastLogMonth[3] = 0;
	
	//4.0.1.38
	InitWorkingDirectory();
	m_sFileName.Format("Exception.Log");
}


BOOL CExceptionLog::SwapFile()
{
	BOOL	bDone = FALSE;		
	
	char	sTmpBuf[10];
	
	memset (sTmpBuf,0,sizeof(sTmpBuf));
	_strdate_s( sTmpBuf );
	
	if (( m_sLastLogMonth[2] != sTmpBuf[1] ) || (m_sLastLogMonth[1] != sTmpBuf[0]))// check last digit of day in month
	{
		int lFailed = ArchiveOldLogFile((LPSTR)sTmpBuf , (LPSTR)m_sLastLogMonth);

		if(!lFailed)
		{
			m_sLastLogMonth[1] = sTmpBuf[0];
			m_sLastLogMonth[2] = sTmpBuf[1];
			
			SYSTEMTIME st;
			
			GetLocalTime(&st);
			
			m_sBuff.Format("%02d-%02d-%04d %02d:%02d:%02d .................. %s (Controller : %s)\r\n",
				st.wDay,
				st.wMonth,
				st.wYear,
				st.wHour,
				st.wMinute,
				st.wSecond,
				PUMPSRV_VERSION,
				_Module.m_server.m_strControllerName);			
			
			DWORD dwWrittenToBuff = m_sBuff.GetLength();
			if (dwWrittenToBuff)
			{
				long lTmpRet = 0;
				WriteLogFile(dwWrittenToBuff, &lTmpRet, _Module.m_server.m_lDiskWarning);
			}						
		}
	}

	return bDone;
}

long CExceptionLog::LogTrs(CPumpTransact *pTrs, long lFlags)
{
	
	CARD_SALE_DATA cCardSaleData;
	CARD_SALE_EXTRA_DATA cCardSaleExtraData;
	
	return LogTrs(pTrs,&cCardSaleData,&cCardSaleExtraData,lFlags);
}

long CExceptionLog::LogTrs(CPumpTransact *pTrs, CARD_SALE_DATA *pData,CARD_SALE_EXTRA_DATA *pExtraData,long lFlags)
{
	DWORD dwWrittenToFile = 0;
	SYSTEMTIME st;
	GetLocalTime(&st);
	
	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csLogFile);
	
	DWORD dwThreadId = GetCurrentThreadId();
	
	SwapFile();
	
	
	if(OpenLogFile() != INVALID_HANDLE_VALUE)
	{
		CString strTrs;
		strTrs.Format("%02d-%02d-%04d %02d:%02d:%02d (%04d) TRS %05d, ",
			st.wDay,
			st.wMonth,
			st.wYear,
			st.wHour,
			st.wMinute,
			st.wSecond,
			dwThreadId,
			pTrs->m_lNumber);
		
		if (lFlags == EXCEPTION_MESS_FORCE_REMOVE)			 //4.0.8.500
			strTrs += " Force remove ";			
		else if (lFlags == EXCEPTION_MESS_SHIFT_FORCE_REMOVE)
			strTrs += " EOD stuck trs";			
		
		switch(pTrs->m_lRes)
		{
		case ATTEND:	
			strTrs += "ATTEND        ";
			break;
		case TEST:		
			strTrs += "TEST          ";
			break;
		case PAY_AT_PUMP:
			// start 3.1.1.6
			if (pTrs->m_lResExt == RES_EXT_AVI)
				strTrs += "AVI           ";		
			else if (pTrs->m_lResExt == RES_EXT_MOBILE)//CR 444830
			{
				strTrs += "PAY_AT_MOBILE ";
			}
			else
				// End 3.1.1.6
				strTrs += "PAY_AT_PUMP   ";
			break;
		case PAY_AT_KIOSK:
			if (pTrs->m_lResExt == RES_EXT_ATTENDANT)  //4.0.1.36
			{
				strTrs += "ATTENDANT     ";						
			}
			else
				strTrs += "PAY_AT_KIOSK  ";
			break;
		case PAY_AT_OPT:	
			strTrs += "PAY_AT_OPT    ";
			break;
		case PRE_PAY_AT_POS:
			strTrs += "PRE_PAY_AT_POS";				
			break;
		case NULL_RES:
		default:
			strTrs += "NULL_RES      ";
			break;
			
		}
		
		CString sTmpStr;
		sTmpStr.Format(", Pump %02d, Grade %02d, Volume %06d, Value %06d, GradePrice %06d",
			pTrs->m_lPump, 
			pTrs->m_nGrade, 
			pTrs->m_lRoundedVolume, 
			pTrs->m_lRoundedValue,
			pTrs->m_lGradePrice);
		strTrs += sTmpStr;
		
		m_sBuff.Format("%s\r\n",(LPCSTR)strTrs);
		if (m_sBuff.GetLength())
		{
			long lTmpRet=0;
			dwWrittenToFile = WriteLogFile(m_sBuff.GetLength(), &lTmpRet, _Module.m_server.m_lDiskWarning);
		}							
	}
	
	CloseLogFile(); // TD 227394
	
	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csLogFile);
	
	if (dwWrittenToFile != 0)
		return LOG_OK;
	else
		return LOG_NOT_OK;
}


long CLogger::GetReturnCodeIDR(const char * sSource , long lRetCode)
{
	long lIDROffset = 0 , lStringIDR = lRetCode;
	
	//The following sources use GPI return codes.  Calls to GCIEvents
	//in QueueHandle aer translated to GPI return codes as well.
	if((!_stricmp("IGPI" , sSource)) || (!_stricmp("Internal" , sSource)))
		lIDROffset = 101;
	else if(!_stricmp("IGCI" , sSource))
		lIDROffset = 251;
	else if(!_stricmp("ICTRL" , sSource))
		lIDROffset = 401;
	else if(!_stricmp("IGCIEvents" , sSource))//4.0.5.24
		lIDROffset = 551;	
	else if(!_stricmp("GCI_RES" , sSource))
		lIDROffset = 600;	
	else if(!_stricmp("GCI_RES_EXT" , sSource))
		lIDROffset = 650;	
	else if(!_stricmp("PUMP_SRV_STATUS" , sSource))
		lIDROffset = 700;	
	else if(!_stricmp("PUMP_STATUS" , sSource))
		lIDROffset = 750;	
	
	if(lIDROffset)
		lStringIDR += lIDROffset;
	else
		lStringIDR = -1;
	
	return lStringIDR;
}

int CLogger::GetReturnCodeDescription(LPCSTR sSource, long lRetCode, LPSTR sDesc , int iLen)
{
	int iDescLen = 0; // CID_184577 Uninitialized scalar variable
	long lIDR = 0;
	
	if (lRetCode != 9999)
	{
		lIDR = GetReturnCodeIDR(sSource , lRetCode);
	}
	else
	{
		lIDR = 9999;
	}
	
	if(lIDR > 0)
		iDescLen = LoadString(_Module.m_hInst , (UINT)lIDR , sDesc , iLen);
	
	//If we could not retrieve the IDR or no string exists for that IDR,
	//use default message
	if((lIDR <= 0) || (iDescLen == 0))
	{
		memset(sDesc , 0 , iLen);
		strncat_s(sDesc ,iLen,"Error description not available" , min(strlen("Error description not available") , (UINT)(iLen - 1)));
	}
	
	return iDescLen;
}


//4.0.1.38
void CExceptionLog::InitWorkingDirectory(char * sPath)
{
	if(!sPath)
		strcpy_s((LPSTR)m_sWorkingPath ,MAX_FILE_PATH, (LPSTR)m_sBaseFilePath.GetBuffer(0));
	
	else
		strcpy_s((LPSTR)m_sWorkingPath ,MAX_FILE_PATH, (LPSTR)sPath);
	
	return;
}

void CLogger::LogMiddleOfProcessReason()
{
	CString cMsg = "In the middle of ";
	
	SERVER_INFO sInfo;
	//4.0.8.500_Module.m_server.GetInfo(&sInfo);
	_Module.m_server.m_cServerInfo.GetInfo(sInfo);
	
	
	if (sInfo.bRequiredShiftProcess)
		cMsg += "Shift change Process";		
	else if(sInfo.bRequiredPriceChange)
		cMsg += "Price change Process";	
	else if(sInfo.bRequiredModeProcess)
		cMsg += "Mode change Process";	
	else if(sInfo.byRequiredMaintenanceProcess)
		cMsg += "Maintenance Process";
	//else if (sInfo.byWetStockFlags) //4.0.9.503
	else if (sInfo.lWetStockFlags)
		cMsg += "WetStock Process";	
	else if (sInfo.byMemoInProcess)
		cMsg += "WetStock by Memo Process";	
    else if(sInfo.byDeliveryEmulationStatus != DES_IDLE)
		cMsg += "Tank Delivery Emulation Process";
	else if (sInfo.bInPriceChangeProcess)
		cMsg += "Price change with rollback Process";
	else //66935
	{
		long lTankNumber;
		if (_Module.m_server.m_cCommonFunction.IsDeliveryEmulationSingleTankInReadingProcess(lTankNumber))
			cMsg += "Delivery Emulation Single Tank Reading Process";
	}
	
	
	LogMsg(cMsg);
}

//4.0.2.37
void CLogger::LogSystemDownData(long lState)
{
	FILE * pFile = NULL;
	char sBuffer[2048];
	char sLine[2048];
	memset((char*)sLine , 0 , sizeof(sLine));
	memset((char*)sBuffer , 0 , sizeof(sBuffer));
	long lSize   = 0;
	long lOffset = 0;
	SYSTEMTIME st;
	long iTmp = 0;
	
	GetLocalTime(&st);
	
	// Read max. of 2048 bytes from the end of pumpsrv.log file 
	pFile = fopen((char*)m_sFilePath.GetBuffer(0) , "r+t");
	if (pFile != NULL)
	{
		fseek(pFile , 0L , SEEK_END);
		lSize = ftell(pFile);
		//lOffset = min(2048 , lSize); 4.0.10.508
		lOffset = min(2047 , lSize);
		fflush(pFile);
		long lStartPos = lSize - lOffset;
		fseek(pFile , lStartPos , SEEK_SET);
		iTmp = fread(sBuffer , sizeof(char) , sizeof(sBuffer), pFile);
		fclose(pFile);
	}
	
	long lPos = lOffset;
	long lCount = 0; 
	bool bFound = false; 
	
	// Get last log line from the buffer
	if (iTmp > 0)
	{
		while ((!bFound) && (lPos > 0))
		{
			//4.0.6.507
			if(sBuffer[lPos] == char(10))
				bFound = true; 
			lPos--;
			/*if ((lCount >> 0) && (!bFound))
			lCount++; */   
			
			// Allows getting the data before the char(10)
			if ((bFound) && (lCount == 0))
			{
				lCount = 1;
				bFound = false;
			}
		}
	}
	
	// Write data to ..\PumpSrv\SysDown.log
	strcpy_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,(char*)m_sSysDownLogFilePath);
	strcat_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,"\\SysDown");
	strcat_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,m_sSiteId.GetBuffer(0));
	strcat_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,".Log");	
	pFile = fopen(m_sSysDownLogFileName, "a+t");
	if (pFile != NULL) 
	{
		// Write to file the last time system was up
		if ((bFound) && (lState == -1))
		{
			strncpy_s(sLine,sBuffer+lPos+2,23);
			strcat_s((char*)sLine,2048," System Down\n");
			fwrite(sLine, sizeof( char ), strlen(sLine), pFile);    
		}
		
		// Write to file when system is up or when there 
		// is no controller or convert layer connected 
		
		if (lState >= -1) //4.0.3.35 ( >= instead of > )
		{
			CString s = "System Up";
			switch (ServerState(lState))
			{
			case SERVER_NO_CONVERT : s = "Convert Layer Down";
				break;
			case SERVER_STARTUP    : s = "Controller Down";
				break;
			}
			
			CString sMsg;
			sMsg.Format("%02d-%02d-%04d %02d:%02d:%02d:%03d %s\n" , 
				st.wDay,
				st.wMonth,
				st.wYear,
				st.wHour,
				st.wMinute,
				st.wSecond,
				st.wMilliseconds, s);
			
			fwrite(sMsg, sizeof( char ), strlen(sMsg), pFile);    
		}
		fclose(pFile);
	}
}
//4.0.2.37

void CLogger::LogMsgOLAStartSession(long lPumpNumber, char *sInBuffer, char *sOlaType, long lInBufferSize)
{	
	CString str;
	long	lInSize =  lInBufferSize *2;
	char	*szResult= NULL;  //4.0.5.13
	char	*szResult1= NULL;
	long    lRetVal = 0;
	int		iMulSize = 0;
	long	lOutBuffLen = lInSize;
	BOOL	bFailedEncrypt = FALSE;	
	
	
	szResult = new char [lInSize];
	szResult1= new char [lInSize];
	memset(szResult1 , 0 , lInSize);
	memset(szResult  , 0 , lInSize);
	
	memcpy(szResult1 ,sInBuffer , min(lInSize , lInBufferSize)); //4.0.10.506
	
	////////////////////////////////////////////// 4.0.10.47////////////////////////////////
	lOutBuffLen = lInSize;
	memset(szResult1 , 0 , lInSize);
	//4.0.5.12
	//Encrypt Track1 data
	lRetVal = _Module.m_server.m_cProtectedDataHandler.EncryptOLAString("ClubCardNumber[S]", szResult ,szResult1, &lOutBuffLen); //4.0.22.501 TD 72716
	
	// Succeded encrypt  track1
	if ((!lRetVal))
	{
		lOutBuffLen = lInSize;
		memset(szResult , 0, lInSize);
	}
	
	/////////////////////////////////////////////
	
	lOutBuffLen = lInSize;
	memset(szResult , 0 , lInSize);
	//4.0.5.12
	//Encrypt Track1 data
	lRetVal = _Module.m_server.m_cProtectedDataHandler.EncryptOLAString("Track1[S]=", szResult1 ,szResult,&lOutBuffLen); 
	
	// Succeded encrypt  track1
	if ((!lRetVal))		//4.0.22.13	TD 72716
	{ 			
		lOutBuffLen = lInSize;
		memset(szResult1 , 0, lInSize);
		
		//Encrpt account data
		lRetVal =  _Module.m_server.m_cProtectedDataHandler.EncryptOLAString("AccountNumber[S]=", szResult ,szResult1,&lOutBuffLen);
		
		if ((!lRetVal))
		{
			str.Format(">>> OLA:StartSession, %.20s",sOlaType);
			LogOLABufferMsg(lPumpNumber,LOG_PUMP,str,szResult1);

		}
		else if (lRetVal) 
		{
			bFailedEncrypt = TRUE;
		}
	}
	else if (lRetVal) 
	{
		bFailedEncrypt = TRUE;
	}
		
	if (bFailedEncrypt)
	{
		str.Format(">>> OLA:StartSession at Pump %02ld, %.20s , failed encrypt ",lPumpNumber,sOlaType);
	}
	
	LogMsg(str);
	
	if (szResult)
		delete []szResult;
	if (szResult1)
		delete []szResult1;
}


/******************************************************************************
Description:	Perform LogMsg with the pump num in the prefix of the line.      	

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ravit			01/11/2005   16:20		Start
******************************************************************************/
long CLogger::LogPumpMsg(CString sClassName,CString sFuncName,long lPumpNum,CString strMsg,long lLevel,BOOL bConsoleMsg)
{
	CString str;
	str.Format("Pump - %02ld, %s",lPumpNum,strMsg);
	//	str.Format("%s::%s: Pump - %02ld, %s",sClassName,sFuncName,lPumpNum,strMsg);
	return LogMsg(str,lLevel,bConsoleMsg);
}
  
long CLogger::LogPumpStatus(long lPump, long lStatus, long lRes, long lResExt, long lRetCode)
{
	CString str;
	char sResDescription[MAX_FIELD_VAL];
	char sResExtDescription[MAX_FIELD_VAL];
	char sStatusDescription[MAX_FIELD_VAL];

	memset((char*)sResDescription , 0 , sizeof(sResDescription));
	GetReturnCodeDescription("GCI_RES",lRes,sResDescription,sizeof(sResDescription));

	memset((char*)sResExtDescription , 0 , sizeof(sResExtDescription));
	GetReturnCodeDescription("GCI_RES_EXT",lResExt,sResExtDescription,sizeof(sResExtDescription));

	memset((char*)sStatusDescription , 0 , sizeof(sStatusDescription));
	GetReturnCodeDescription("PUMP_SRV_STATUS",lStatus,sStatusDescription,sizeof(sStatusDescription));

	str.Format("SetPumpStatus(Pump - %02ld, Status %d = %s, Res %d = %s, ResExt %d = %s)", lPump, lStatus, sStatusDescription, lRes, sResDescription, lResExt, sResExtDescription);

	return LogMsgWithRet(0,LOG_NONE,str,"IGCI",lRetCode);
}
  
  
HANDLE CLogger::OpenReadLogFile()
{  
	CloseLogFile();
	// Open File in share mode
	m_hFile = CreateFile(	(LPCTSTR)m_sFilePath,     // pointer to name of the file
						  GENERIC_READ | GENERIC_WRITE,   // access (read-write) mode
						  FILE_SHARE_READ,		   // share mode
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
			strcpy_s((char*)m_sWorkingPath ,MAX_FILE_PATH, (char*)m_sBaseFilePath.GetBuffer(0));
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
	// Clear temp Buffer 
	m_sBuff.Empty();

	return m_hFile;
}
  
/******************************************************************************
Description:	When the system is going up, we check if the log end's with "System Down"   	
if not: we write the "System Down" line with the time of the prev line,
we add the log - "System Up".
Returns:      	None
Parameters:   	long lRetCode 

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ravit			13/11/2005   16:20		Start
******************************************************************************/
long CLogger::SystemUp()
{
	HANDLE hFile;

	long lRetVal = 0;
	SYSTEMTIME st;
	char sLine[MAX_FIELD_VAL];

	DWORD dwWrittenToFile;

	long lFileLen;
	long lLen;
	unsigned long lBytesRead;

	char sStr[MAX_FIELD_VAL];
	memset(sStr,' ',sizeof(sStr));
	memset(sLine,' ',sizeof(sLine));

	GetLocalTime(&st);

	// -------------------------
	// Critical Section - Start
	// -------------------------
	//	EnterCriticalSection(&m_csLogFile);

	//	DWORD dwThreadId = GetCurrentThreadId();

	SwapFile();


	// Write data to ..\PumpSrv\SysDown.log
	strcpy_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,(char*)m_sSysDownLogFilePath);
	strcat_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,"\\SysDown");
	strcat_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,m_sSiteId.GetBuffer(0));
	strcat_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,".Log");	

	hFile = CreateFile((LPCTSTR)m_sSysDownLogFileName,     // pointer to name of the file
	GENERIC_READ | GENERIC_WRITE,   // access (read-write) mode
	FILE_SHARE_READ,		   // share mode
	NULL,				       // no security                               
	OPEN_ALWAYS,              // how to create
	FILE_ATTRIBUTE_NORMAL,    // file attributes
	NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		//get the len of the log file 
		lFileLen = GetFileSize(hFile, (LPDWORD)&lLen); 

		//if the log file is not empty: 
		//we check the last string of the log file, to see if the system closed ok.      
		if (lFileLen > SYS_DOWN_STR_LEN)
		{
			//go back to the beginning of the last string  
			long lOpSysStrLen;
			lOpSysStrLen = -1 * SYS_DOWN_STR_LEN - 2;
			SetFilePointer(hFile,lOpSysStrLen,NULL,FILE_END);

			if (ReadFile(hFile,sStr,SYS_DOWN_STR_LEN - 1,&lBytesRead, NULL))
			{
				sStr[SYS_DOWN_STR_LEN - 1] = '\0';
				//check: if the last string in the log was "System down" then the system was 
				//closed ok, else there was a problem.   
				if (!(strcmp(sStr,SYS_DOWN) == 0))
				{
					FindAndWriteSystemDownTime(hFile);
				}
			}
		}

		sprintf_s(sLine,MAX_FIELD_VAL,"%02d-%02d-%04d %02d:%02d:%02d:%03d  .................. %s \r\n",	st.wDay,
																						st.wMonth,
																						st.wYear,
																						st.wHour,
																						st.wMinute,
																						st.wSecond,	
																						st.wMilliseconds,
																						SYS_UP);

		WriteFile(hFile,		  // handle to file to write to
		sLine,			  // pointer to data to write to file
		strlen(sLine),	  // number of bytes to write
		&dwWrittenToFile,	  // pointer to number of bytes written
		NULL);			  // pointer to structure for overlapped I/O
	}

	CloseHandle(hFile);

	return LOG_OK;
}

  
/******************************************************************************
Description:	Find the last time in the log, and add the line "System Down"
with that time.
Returns:      	None
Parameters:   	long lRetCode 
  
WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ravit			13/11/2005   16:20		Start
******************************************************************************/

long CLogger::FindAndWriteSystemDownTime(HANDLE hSysDownFile)
{
	int const TIME_LEN = 24;
	char sTimeStr[TIME_LEN];
	
	char sCh;
	long lStartOfLine = 0; 
	unsigned long lBytesRead = 1;
	long lOpTimeLen; 
	long lRetVal = 0;
	
	DWORD dwWrittenToFile;
	CString cSysDown;
	
	char sLine[MAX_FIELD_VAL];
	memset(sLine,' ',sizeof(sLine));
	
	EnterCriticalSection(&m_csLogFile);
	SwapFile();
	
	if(OpenReadLogFile() !=  INVALID_HANDLE_VALUE)
	{
		//look for the time when the system was down (in the pumpSrv log).  
		//we go back to the previously line as much as we known we can, without get to the line before.   
		
		lOpTimeLen = -1 * TIME_LEN;
		SetFilePointer(m_hFile,lOpTimeLen,NULL,FILE_END);
		
		BOOL bIsFileOK = TRUE;											//4.0.23.510 TD 100241
			
		//now go to the start of the current line 
		while ((bIsFileOK == TRUE && !lStartOfLine && 0 != lBytesRead))					//4.0.23.510 TD 100241
		{		
			bIsFileOK = ReadFile(m_hFile,&sCh,1,&lBytesRead, NULL);		//4.0.23.510 TD 100241
			
			if (bIsFileOK == TRUE && sCh == '\n')						//4.0.23.510 TD 100241
			{
				//read the time from the pumpSrv log
				bIsFileOK = ReadFile(m_hFile,sTimeStr,TIME_LEN - 1,&lBytesRead, NULL);
				sTimeStr[TIME_LEN - 1] = '\0';
				
				//return the log file pointer to the end.  
				SetFilePointer(m_hFile,0,NULL,FILE_END);
				
				//return the SysDown file pointer to the end.  
				SetFilePointer(hSysDownFile,0,NULL,FILE_END);
				
				sprintf_s(sLine,MAX_FIELD_VAL,"%s  .................. %s \r\n",
					sTimeStr,
					SYS_DOWN);
				
				//			   fwrite(sLine,sizeof(char),strlen(sLine),hSysDownFile);    
				
				bIsFileOK = WriteFile(hSysDownFile,	   // handle to file to write to
				sLine,			   // pointer to data to write to file
				strlen(sLine),	   // number of bytes to write
				&dwWrittenToFile, // pointer to number of bytes written
				NULL);			   // pointer to structure for overlapped I/O
				
				
				lStartOfLine = 1;
			}

			SetFilePointer(m_hFile,-2,NULL,FILE_CURRENT);
		}

		SetFilePointer(hSysDownFile, 0, NULL, FILE_END);
		SetFilePointer(m_hFile, 0, NULL, FILE_END);
		CloseLogFile();
	}
	
	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csLogFile);
	
	return LOG_OK;
}

	
long CLogger::SystemDown()
{
	HANDLE hFile;
	DWORD dwWrittenToFile;
	
	SYSTEMTIME st;
	char sLine[MAX_FIELD_VAL];
	memset(sLine,' ',sizeof(sLine));
	
	GetLocalTime(&st);
	
	// Write data to ..\PumpSrv\SysDown.log
	strcpy_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,(char*)m_sSysDownLogFilePath);
	strcat_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,"\\SysDown");
	strcat_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,m_sSiteId.GetBuffer(0));
	strcat_s((char*)m_sSysDownLogFileName,MAX_FILE_PATH,".Log");	
	
	hFile = CreateFile((LPCTSTR)m_sSysDownLogFileName,     // pointer to name of the file
		GENERIC_READ | GENERIC_WRITE,   // access (read-write) mode
		FILE_SHARE_READ,		   // share mode
		NULL,				       // no security
		OPEN_ALWAYS,              // how to create
		FILE_ATTRIBUTE_NORMAL,    // file atributes
		NULL);
	
	if(hFile != INVALID_HANDLE_VALUE)
	{	
		SetFilePointer(hFile,0,NULL,FILE_END);
		
		sprintf_s(sLine,MAX_FIELD_VAL,"%02d-%02d-%04d %02d:%02d:%02d:%03d  .................. %s \r\n",
			st.wDay,
			st.wMonth,
			st.wYear,
			st.wHour,
			st.wMinute,
			st.wSecond,	
			st.wMilliseconds,
			SYS_DOWN);			
		
		WriteFile(hFile,	           // handle to file to write to
			sLine,			   // pointer to data to write to file
			strlen(sLine),	   // number of bytes to write
			&dwWrittenToFile, // pointer to number of bytes written
			NULL);			   // pointer to structure for overlapped I/O
	}
	
	CloseHandle(hFile);
	
	return LOG_OK;
}	

//4.0.15.450 {
long CLogger::LogClassMsg(const char* szClassName, const char* szFuncName, long lNum,long lLogType,const char* szMsg,long lLevel,BOOL bConsoleMsg)
{
	char str[MAX_FIELD_VAL];
	memset(str, 0, sizeof(str));

	switch(lLogType)
	{
		case LOG_PUMP:
			sprintf_s(str,MAX_FIELD_VAL,"Pump - %02ld, %s::%s, %s",lNum,szClassName,szFuncName,szMsg);
			break;
		case LOG_TERM:
			sprintf_s(str,MAX_FIELD_VAL,"Term - %02ld, %s::%s, %s",lNum,szClassName,szFuncName,szMsg);
			break;
		case LOG_NONE:
			sprintf_s(str,MAX_FIELD_VAL,"%s::%s, %s",szClassName,szFuncName,szMsg);
			break;
		default:	
			break;
	}

	return LogMsg((LPCSTR)str, lLevel, bConsoleMsg);
}
//4.0.15.450 }


//4.0.15.450 {
long CLogger::LogMsg2(long lNum, long lLogType, const char* szMsg, long lLevel, BOOL bConsoleMsg)
{
	char	szStr[MSG_MAX_BUFFER];

	AddNumInfo2(lNum,lLogType,szMsg,szStr,MSG_MAX_BUFFER);

	return LogMsg(szStr,lLevel,bConsoleMsg);
}
//4.0.15.450  }

/******************************************************************************
 Description:	Perform LogMsg with the pump num / Term num in the prefix of the line.      	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena 			16/03/2008   18:47 . 4.0.15.450		Start
******************************************************************************/
long CLogger::LogMsg2(const char* szClassName,const char* szFuncName,long lPumpNum,long lLogType,const char* szMsg,long lLevel , BOOL bConsoleMsg)
{	
	return LogMsg2( lPumpNum, lLogType, szMsg, lLevel, bConsoleMsg);
}


//4.0.15.450 {
long CLogger::LogMsg2(long lNum1,long lLogType1,long lNum2,long lLogType2, char* szMsg,long lLevel , BOOL bConsoleMsg)
{
	char	szStr[MAX_FIELD_VAL];
	char	szStr2[MAX_FIELD_VAL];
	
	AddNumInfo2(lNum2,lLogType2,szMsg,szStr,MAX_FIELD_VAL);
	AddNumInfo2(lNum1,lLogType1,szStr,szStr2,MAX_FIELD_VAL);

	return LogMsg((LPCSTR)szStr2,lLevel,bConsoleMsg);
}
//4.0.15.450 }

//4.0.15.450 {
void CLogger::AddNumInfo2(long lNum, long lLogType,const char* szMsg, char* sOutMsg,int iBufferSize)
{
	switch(lLogType)
	{
		case LOG_PUMP:
			sprintf_s(sOutMsg,iBufferSize,"Pump - %02ld, %s",lNum,szMsg);
			break;
		case LOG_TERM:
			sprintf_s(sOutMsg,iBufferSize,"Term - %02ld, %s",lNum,szMsg);
			break;
		case LOG_NONE:
			sprintf_s(sOutMsg,iBufferSize,"%s",szMsg);
			break;
		case LOG_POS:
			sprintf_s(sOutMsg,iBufferSize,"Pos  - %02ld, %s",lNum,szMsg);
			break;
		default:	
			break;
	}
}
// 4.0.15.450 }


long CLogger::LogFlagsMsg(long lNum,long lLogType,const char* strMsg,long lFlags,long lLevel,BOOL bConsoleMsg)
{
	char strTmp[MAX_FIELD_VAL];
	sprintf_s(strTmp,MAX_FIELD_VAL,"%s, Flags = 0x%08x",strMsg,lFlags);

	return LogMsg2(lNum,lLogType,strTmp,lLevel,bConsoleMsg); //4.0.15.450
}

/******************************************************************************
 Description: Convert from integer state to string state and prints it into the log	
 Returns:      	

 Parameters: iCurrState - current loyalty state
			 iNextState - next loylaty state
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			12/10/2011       		Start			4.0.23.1220 TD 155754
******************************************************************************/
CString CLogger::GetStateString(int iState) const
{
	CString sState;

	switch(OLA_STATE(iState))
	{
		case OLA_NONE:
			sState += "OLA_NONE";
			break;
		case OLA_LOYALTY_START_TICKET:
			sState += "START_TICKET";
			break;
		case OLA_LOYALTY_WAIT_RESPONSE:
			sState += "WAIT_RESPONSE";
			break;
		case OLA_LOYALTY_ADD_ITEM:
			sState += "ADD_ITEM";
			break;
		case OLA_LOYALTY_END_TICKET:
			sState += "End Ticket";
			break;
		case OLA_SEND_ENDEX:
			sState += "Send Exdex";
			break;
		case OLA_WAIT_CONTROLLER_RESPONSE:
			sState += "WAIT_CONTROLLER_RESPONSE";
			break;
		case OLA_LOYALTY_CANCEL_TICKET_AFTER_START_TICKET:
			sState += "CANCEL_TICKET_AFTER_START_TICKET";
			break;
		case OLA_WAIT_CONTROLLER_MSG:
			sState += "WAIT_CONTROLLER_MSG";
			break;
		case OLA_LOYALTY_WAIT_MORE_DATA:
			sState += "WAIT_MORE_DATA";
			break;
		case OLA_LOYALTY_START_FUELING:
			sState += "START_FUELING";
			break;
		case OLA_LOYALTY_CANCEL_TICKET_BEFORE_START_TICKET:
			sState += "CANCEL_TICKET_BEFORE_START_TICKET";
			break;
		case OLA_LOYALTY_WAIT_FUELING:
			sState += "WAIT_FUELING";
			break;
		case OLA_LOYALTY_RUN_QUERY:
			sState += "RUN_QUERY";
			break;
		case OLA_LOYALTY_APPLY_PAYMENT_DISCOUNT:
			sState += "ADD_MEMBER_CARD_APPLY_DISCOUNT";
			break;
		case OLA_LOYALTY_ADD_MEMBER_CARD:
			sState += "ADD_MEMBER_CARD";
			break;

		case OLA_LOYALTY_WAIT_CONTROLLER_MSG_START_TICKET_FAIL:
			sState += "WAIT_CONTROLLER_MSG_START_TICKET_FAIL";
			break;
		case OLA_LOYALTY_START_NMP_BEFORE_FUELING_BEFORE_START_TICKET:
			sState += "START_NMP_BEFORE_FUELING_BEFORE_START_TICKET";
			break;
		case OLA_LOYALTY_CW_BEFORE_FUELING_BEFORE_START_TICKET:
			sState = "CW_BEFORE_FUELING_BEFORE_START_TICKET";
			break;
		case OLA_LOYALTY_COUPONS_BEFORE_FUELING_BEFORE_START_TICKET:
			sState += "COUPONS_BEFORE_FUELING_BEFORE_START_TICKET";
			break;
		case OLA_LOYALTY_MEMBER_CARD_BEFORE_FUELING_BEFORE_START_TICKET:
			sState += "MEMBER_CARD_BEFORE_FUELING_BEFORE_START_TICKET";
			break;
		case OLA_LOYALTY_PROMPT_BEFORE_FUELING_BEFORE_START_TICKET:
			sState += "PROMPT_BEFORE_FUELING_BEFORE_START_TICKET";
			break;
		case OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_BEFORE_START_TICKET:
			sState += "DEFERRAL_BEFORE_FUELING_BEFORE_START_TICKET";
			break;
		case OLA_LOYALTY_START_NMP_BEFORE_FUELING_AFTER_START_TICKET:
			sState += "START_NMP_BEFORE_FUELING_AFTER_START_TICKET";
			break;
		case OLA_LOYALTY_CW_BEFORE_FUELING_AFTER_START_TICKET:
			sState += "CW_BEFORE_FUELING_AFTER_START_TICKET";
			break;
		case OLA_LOYALTY_COUPONS_BEFORE_FUELING_AFTER_START_TICKET:
			sState += "COUPONS_BEFORE_FUELING_AFTER_START_TICKET";
			break;
		case OLA_LOYALTY_MEMBER_CARD_BEFORE_FUELING_AFTER_START_TICKET:
			sState += "MEMBER_CARD_BEFORE_FUELING_AFTER_START_TICKET";
			break;
		case OLA_LOYALTY_PROMPT_BEFORE_FUELING_AFTER_START_TICKET:
			sState += "PROMPT_BEFORE_FUELING_AFTER_START_TICKET";
			break;
		case OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET:
			sState += "DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET";
			break;
		case OLA_LOYALTY_START_NMP_AFTER_FUELING_BEFORE_ADD_ITEM:
			sState += "START_NMP_AFTER_FUELING_BEFORE_ADD_ITEM";
			break;
		case OLA_LOYALTY_CW_AFTER_FUELING_BEFORE_ADD_ITEM:
			sState += "CW_AFTER_FUELING_BEFORE_ADD_ITEM";
			break;
		case OLA_LOYALTY_COUPONS_AFTER_FUELING_BEFORE_ADD_ITEM:
			sState += "COUPONS_AFTER_FUELING_BEFORE_ADD_ITEM";
			break;
		case OLA_LOYALTY_MEMBER_CARD_AFTER_FUELING_BEFORE_ADD_ITEM:
			sState += "MEMBER_CARD_AFTER_FUELING_BEFORE_ADD_ITEM";
			break;
		case OLA_LOYALTY_PROMPT_AFTER_FUELING_BEFORE_ADD_ITEM:
			sState += "PROMPT_AFTER_FUELING_BEFORE_ADD_ITEM";
			break;
		case OLA_LOYALTY_DEFERRAL_AFTER_FUELING_BEFORE_ADD_ITEM:
			sState += "DEFERRAL_AFTER_FUELING_BEFORE_ADD_ITEM";
			break;
		case OLA_LOYALTY_SWIPE_PAYMENT:
			sState += "SWIPE_PAYMENT";
			break;
		case OLA_LOYALTY_AUTHORIZE_PAYMENT:
			sState += "AUTHORIZE_PAYMENT";
			break;
		case OLA_LOYALTY_SWIPE_LOYALTY:
			sState += "SWIPE_LOYALTY";
			break;
		case OLA_LOYALTY_AUTHORIZE_LOYALTY:
			sState += "AUTHORIZE_LOYALTY";
			break;
		case OLA_LOYALTY_CAR_WASH:
			sState += "CAR_WASH";
			break;
		case OLA_LOYALTY_PROMPT:
			sState += "PROMPT";
			break;
		case OLA_LOYALTY_METHOD_OF_PAYMENT:
			sState += "METHOD_OF_PAYMENT";
			break;
		case OLA_LOYALTY_CAR_WASH_DISCOUNT:
			sState += "CAR_WASH_DISCOUNT";
			break;
		case OLA_LOYALTY_PHONE_LOOKUP:
			sState += "PHONE_LOOKUP";
			break;
		case OLA_LOYALTY_PHONE_LOOKUP_ENTER_PHONE_NUMBER:
			sState += "PHONE_LOOKUP_ENTER_PHONE_NUMBER";
			break;			
		case OLA_LOYALTY_PHONE_LOOKUP_ENTER_REWARD_CARD:
			sState += "PHONE_LOOKUP_ENTER_REWARD_CARD";
			break;
		case OLA_LOYALTY_WAIT_ADD_MEMBER_INFO_RESPONSE:
			sState += "WAIT_ADD_MEMBER_INFO_RESPONSE";
			break;
		case OLA_LOYALTY_WAIT_GET_MEMBER_INFO_RESPONSE:
			sState += "WAIT_GET_MEMBER_INFO_RESPONSE";
			break;
		case OLA_LOYALTY_SWIPE_CARD:
			sState += "SWIPE_CARD";
			break;
		case OLA_LOYALTY_AUTHORIZE_CARD:
			sState += "AUTHORIZE_CARD";
			break;
		case OLA_LOYALTY_CARWASH_SALE:
			sState += "CARWASH_SALE";
			break;
		case OLA_LOYALTY_CARWASH_PROMOTION:
			sState += "LOYALTY_CARWASH_PROMOTION";
			break;
		case OLA_LOYALTY_DISPLAY_PRE_AUTHOIRZE_SCREEN:
			sState += "DISPLAY_PRE_AUTHOIRZE_SCREEN";
			break;
		case OLA_LOYALTY_DISPLAY_PARITAL_APPROVAL_SCREEN:
			sState += "PARITAL_APPROVE_SCREEN";
			break;
		case OLA_LOYALTY_DECLINE:
			sState += "OLA_DECLINE";
			break;
		case OLA_LOYALTY_ALTERNATE_ID:
			sState += "ALTERNATE_ID";
			break;
		case OLA_LOYALTY_ALTERNATE_ID_MANUAL_ENTER:
			sState += "ALTERNATE_ID_MANUAL_ENTER";
			break;
		case OLA_LOYALTY_ALTERNATE_ID_SWIPE_LOYALTY_CARD:
			sState += "ALTERNATE_ID_SWIPE_LOYALTY_CARD";
			break;
		case OLA_LOYALTY_MEMBER_NOT_ACTIVE:
			sState += "MEMBER_NOT_ACTIVE";
			break;
		case OLA_LOYALTY_WAIT_MEMBER_INFO_RESPONSE:
			sState += "WAIT_MEMBER_INFO_RESPONSE";
			break;
		case OLA_LOYALTY_INVALID_INPUT:
			sState += "INVALID_INPUT";
			break;
		case OLA_LOYALTY_ALTERNATE_ID_RETRY:
			sState += "ALTERNATE_ID_RETRY";
			break;
		case OLA_LOYALTY_WAIT_FOR_ALTERNATE_ID_RETRY_ANSWER:
			sState += "WAIT_FOR_ALTERNATE_ID_RETRY_ANSWER";
			break;
	}

	return sState;
	
}

long CLogger::LogCOMError(long lPumpNumber ,LPCSTR szMethodName, HRESULT hRes, int iRetry)
{
	DWORD dwWrittenToFile = 0;
	if (m_IsLoggerInitialised)
	{
		long lRetVal = OK_SPACE;
		SYSTEMTIME st;
		
		GetLocalTime(&st);
		
		// -------------------------
		// Critical Section - Start
		// -------------------------
		
		EnterCriticalSection(&m_csLogFile);
		
		SwapFile();
		
		if(OpenLogFile() != INVALID_HANDLE_VALUE)
		{
				m_sBuff.Format("%s COM EXCEPTION ....  Pump %ld Failed to call LoyaltyClient::%.30s, hRes = 0x%.8X, Try Number %d !!!",
				m_TimeStamp->getTimeStamp(),
				lPumpNumber,	
				szMethodName,
				hRes,
				iRetry);			
			
			DWORD dwWrittenToBuff = m_sBuff.GetLength();
			if (dwWrittenToBuff)
			{
				m_sBuff += "\r\n";
				dwWrittenToFile = WriteLogFile(dwWrittenToBuff, &lRetVal);
			}				
		}
		
		
		
		// -------------------------
		// Critical Section - End
		// -------------------------
		LeaveCriticalSection(&m_csLogFile);
		
		if (dwWrittenToFile)
			return 0;
		else
			return 1;
	}
	return 1;
}