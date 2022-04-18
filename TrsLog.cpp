// TrsLog.cpp: implementation of the CTrsLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TrsLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrsLog::CTrsLog() : m_bActive(FALSE)
{
	
}

CTrsLog::~CTrsLog()
{
	KillThread();  //4.0.3.58
	Cleanup();	   //4.0.3.58
}

//4.0.3.58 Modified method to support runtime reconfiguration for QLogger
void CTrsLog::Init()
{
	DWORD dwTmp = 0;
	
	m_sBaseFilePath.Empty();
	m_sBaseFilePath += "C:\\PumpSrv";
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam","TrsLogActive",&dwTmp,dwTmp,TRUE,FALSE);
	
	if(dwTmp)
	{
		char sTmp[MAX_FILE_PATH] = { 0 };
		if (_Module.m_server.m_cParam.GetParam("Paths", 1, 0, "PSTransactionDir", FIELD_SZ, (char *)sTmp, MAX_FILE_PATH, NULL, FALSE))
		{
			m_sBaseFilePath.Empty(); 
			m_sBaseFilePath += "C:\\PumpSrv";
			_Module.m_server.m_cParam.GetParam("Paths", 1, 0, "PSTransactionDir", FIELD_SZ, (char *)sTmp, MAX_FILE_PATH, NULL, TRUE);
		}

		m_sBaseFilePath = sTmp;
		m_sBaseFilePath += "\\PSTransaction";
	
		m_sFilePath.Empty();
		m_sFilePath = m_sBaseFilePath;
		m_sFilePath += ".Log";

		//4.0.1.38
		InitWorkingDirectory();
		m_sFileName.Format("PSTransaction.Log");
	}
	
	//4.0.3.58
	SetActiveParam((BOOL)(dwTmp ? TRUE : FALSE));

	return;
}

void CTrsLog::InitWorkingDirectory(char * sPath)
{
	if(!sPath)
		strcpy((LPSTR)m_sWorkingPath , (LPSTR)m_sBaseFilePath.GetBuffer(0));
	else
		strcpy((LPSTR)m_sWorkingPath , (LPSTR)sPath);
	return;
}

/******************************************************************************
 Description:	Do nothing, this file should not have swipe ability 	
 Returns:      	BOOL - always FALSE
 Parameters:   	None.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			10/7/2003   11:55		Start
******************************************************************************/
BOOL CTrsLog::SwapFile()
{
	return FALSE;
}

long CTrsLog::LogTrs(CPumpTransact *pTrs)
{
	SYSTEMTIME st;
	long lServiceMode = 0;
	
	//only if the file is active
	if(m_bActive)
	{
		GetLocalTime(&st);

		if (pTrs->m_lPump > 0)
			lServiceMode= _Module.m_server.m_cPumpArray[pTrs->m_lPump-1].GetStat().GetServiceMode();

		// -------------------------
		// Critical Section - Start
		// -------------------------
		EnterCriticalSection(&m_csLogFile);


		if(OpenLogFile() != INVALID_HANDLE_VALUE)
		{
			CString strTrs;
			strTrs.Format("%02d-%02d-%04d, %02d:%02d:%02d:%03d,Trs, %d, %d, %d, %d, %d, %d, %d, %d, %d",
								st.wDay,
								st.wMonth,
								st.wYear,
								st.wHour,
								st.wMinute,
								st.wSecond,
								st.wMilliseconds,
								pTrs->m_lNumber,
								pTrs->m_lPump,
								pTrs->m_nNozzle,
								pTrs->m_lRoundedVolume,
								pTrs->m_lRoundedValue,
								pTrs->m_nGrade,
								pTrs->m_lGradePrice,
								pTrs->m_nLevel,
								lServiceMode);

			m_sBuff.Format("%s\r\n", (LPCSTR)strTrs);
			DWORD dwWrittenToBuff = m_sBuff.GetLength();
			if (dwWrittenToBuff)
			{	
				long lTmpRet=0;  //4.0.2.38
				WriteLogFile(dwWrittenToBuff, &lTmpRet, _Module.m_server.m_lDiskWarning); //4.0.2.38
			}							
		}

		CloseLogFile();

		// -------------------------
		// Critical Section - End
		// -------------------------
		LeaveCriticalSection(&m_csLogFile);
	}
	
	return LOG_OK;
}

long CTrsLog::LogShift(long lShiftNumber)
{
	SYSTEMTIME st;

	//only if the file is active
	if(m_bActive)
	{
		GetLocalTime(&st);

		// -------------------------
		// Critical Section - Start
		// -------------------------
		EnterCriticalSection(&m_csLogFile);

		//The shift number is 1 less then current
		if(lShiftNumber == 1)
			lShiftNumber = _Module.m_server.GetMaxShiftNumber();		//TD 417989

		if(OpenLogFile() != INVALID_HANDLE_VALUE)
		{
			CString strTrs;
			strTrs.Format("%02d-%02d-%04d, %02d:%02d:%02d:%03d, -- EOS --,%d",
								st.wDay,
								st.wMonth,
								st.wYear,
								st.wHour,
								st.wMinute,
								st.wSecond,
								st.wMilliseconds,
								lShiftNumber);

			m_sBuff.Format("%s\r\n",(LPCSTR)strTrs);
			if (m_sBuff.GetLength())
			{	
				long lTmpRet=0;  //4.0.2.38
				WriteLogFile(m_sBuff.GetLength(), &lTmpRet, _Module.m_server.m_lDiskWarning); //4.0.2.38
			}							
		}

		CloseLogFile();

		// -------------------------
		// Critical Section - End
		// -------------------------
		LeaveCriticalSection(&m_csLogFile);
	}

	return LOG_OK;
}

long CTrsLog::LogPumpTotals(long lPumpNum, short nNuzzle, long lGrade, char* pszVolume, char* pszValueA)
{
	SYSTEMTIME st;

	//only if the file is active
	if(m_bActive)
	{
		GetLocalTime(&st);

		// -------------------------
		// Critical Section - Start
		// -------------------------
		EnterCriticalSection(&m_csLogFile);

		if(OpenLogFile() != INVALID_HANDLE_VALUE)
		{
			CString strTrs;
			strTrs.Format("%02d-%02d-%04d, %02d:%02d:%02d:%03d, PTotals, %d, %d, %d, %.20s, %.20s",
								st.wDay,
								st.wMonth,
								st.wYear,
								st.wHour,
								st.wMinute,
								st.wSecond,
								st.wMilliseconds,
								lPumpNum,
								nNuzzle,
								lGrade,
								pszVolume,
								pszValueA);

			m_sBuff.Format("%s\r\n",(LPCSTR)strTrs);
			if (m_sBuff.GetLength())
			{	
				long lTmpRet=0;  //4.0.2.38
				WriteLogFile(m_sBuff.GetLength(), &lTmpRet, _Module.m_server.m_lDiskWarning); //4.0.2.38
			}							
		}

		CloseLogFile();

		// -------------------------
		// Critical Section - End
		// -------------------------
		LeaveCriticalSection(&m_csLogFile);
	}

	return LOG_OK;
}

long CTrsLog::LogTankTotals(long lTankNum, long lGrade, long lVolume)
{
	SYSTEMTIME st;

	//only if the file is active
	if(m_bActive)
	{
		GetLocalTime(&st);

		// -------------------------
		// Critical Section - Start
		// -------------------------
		EnterCriticalSection(&m_csLogFile);

		if(OpenLogFile() != INVALID_HANDLE_VALUE)
		{
			CString strTrs;
			strTrs.Format("%02d-%02d-%04d, %02d:%02d:%02d:%03d, TTotals, %d, %d, %d",
								st.wDay,
								st.wMonth,
								st.wYear,
								st.wHour,
								st.wMinute,
								st.wSecond,
								st.wMilliseconds,
								lTankNum,
								lGrade,
								lVolume);

			m_sBuff.Format("%s\r\n",(LPCSTR)strTrs);
			if (m_sBuff.GetLength())
			{	
				long lTmpRet=0;  //4.0.2.38
				 WriteLogFile(m_sBuff.GetLength(), &lTmpRet , _Module.m_server.m_lDiskWarning); //4.0.2.38
			}							
		}

		CloseLogFile();

		// -------------------------
		// Critical Section - End
		// -------------------------
		LeaveCriticalSection(&m_csLogFile);
	}

	return LOG_OK;
}

BOOL CTrsLog::IsTrsLogActive()
{
	return m_bActive;
}

/******************************************************************************
 Description:	Set the m_bActive member of CTrsLog.
 Returns:      	void.
 Parameters:   	bActive - value to set.
 Assumptions:   none.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			7/10/2003   15:59	 Documentation   4.0.3.58
******************************************************************************/
void CTrsLog::SetActiveParam(BOOL bActive)
{
	if(bActive && !m_bActive)
	{
		Allocate();
		CreateThread();
		StartThread();
	}

	else if(!bActive && m_bActive)
	{
		Cleanup();
		KillThread();
	}

	m_bActive = bActive;	

	return;
}

//4.0.3.58 Start
long CTrsLog::LogCatastrophicFailure()
{
	long lRetVal = 0;

	_LOGMSG.LogMsg("All attempts to write to TRS log have failed!!!" , LOG_LEVEL_0);

	return lRetVal;
}
//4.0.3.58 End


void CStuckTrsLog::Init()
{	
	m_sBaseFilePath.Empty();
	m_sBaseFilePath += "C:\\PumpSrv";
	char sTmp[MAX_FILE_PATH] = { 0 };

	if (_Module.m_server.m_cParam.GetParam("Paths", 1, 0, "LogDir", FIELD_SZ, (char *)sTmp, MAX_FILE_PATH, NULL, FALSE))
	{
		m_sBaseFilePath.Empty();
		m_sBaseFilePath += "C:\\PumpSrv";
		_Module.m_server.m_cParam.GetParam("Paths", 1, 0, "HomeDir", FIELD_SZ, (char *)sTmp, MAX_FILE_PATH, NULL, TRUE);
	}

	m_sBaseFilePath = sTmp;
	m_sBaseFilePath += "\\log\\StuckTransaction";
	
	m_sFilePath.Empty();
	m_sFilePath = m_sBaseFilePath;
	m_sFilePath += ".Log";

	InitWorkingDirectory();
	m_sFileName.Format("StuckTransaction.log");
	m_bActive = TRUE;
	
}

long CStuckTrsLog::LogTrs(CPumpTransact *pTrs)
{
	SYSTEMTIME st;
	long lServiceMode = 0;
	
	//only if the file is active
	if(m_bActive)
	{
		GetLocalTime(&st);

		if (pTrs->m_lPump > 0)
			lServiceMode= _Module.m_server.m_cPumpArray[pTrs->m_lPump-1].GetStat().GetServiceMode();

		// -------------------------
		// Critical Section - Start
		// -------------------------
		EnterCriticalSection(&m_csLogFile);

		DWORD dwThreadId = GetCurrentThreadId();


		if(OpenLogFile() != INVALID_HANDLE_VALUE)
		{
			CString strTrs;
			strTrs.Format("%02d-%02d-%04d %02d:%02d:%02d (%04d) TRS %05d, Pump = %d, Nozzle = %d, Volume = %d, Value = %d, Grade = %d, Grade Price = %d, Level = %d, Service Mode = %d",
								st.wDay,
								st.wMonth,
								st.wYear,
								st.wHour,
								st.wMinute,
								st.wSecond,
								dwThreadId,
								pTrs->m_lNumber,
								pTrs->m_lPump,
								pTrs->m_nNozzle,
								pTrs->m_lRoundedVolume,
								pTrs->m_lRoundedValue,
								pTrs->m_nGrade,
								pTrs->m_lGradePrice,
								pTrs->m_nLevel,
								lServiceMode);

			m_sBuff.Format("%s\r\n",(LPCSTR)strTrs);
			if (m_sBuff.GetLength())
			{	
				long lTmpRet=0;  //4.0.2.38
				WriteLogFile(m_sBuff.GetLength(), &lTmpRet, _Module.m_server.m_lDiskWarning); //4.0.2.38
			}							
		}

		CloseLogFile();

		// -------------------------
		// Critical Section - End
		// -------------------------
		LeaveCriticalSection(&m_csLogFile);
	}
	
	return LOG_OK;
}

void CStuckTrsLog::InitWorkingDirectory(char * sPath)
{
	if(!sPath)
		strcpy((LPSTR)m_sWorkingPath , (LPSTR)m_sBaseFilePath.GetBuffer(0));
	else
		strcpy((LPSTR)m_sWorkingPath , (LPSTR)sPath);
	return;
}