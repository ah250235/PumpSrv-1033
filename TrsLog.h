// TrsLog.h: interface for the CTrsLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRSLOG_H__5C5FD229_5C79_442A_A5B0_4AE59B506C19__INCLUDED_)
#define AFX_TRSLOG_H__5C5FD229_5C79_442A_A5B0_4AE59B506C19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LOG_TRS_FILE	(_Module.m_server.m_cTrsLog.IsTrsLogActive() == TRUE)

//#include "Log.h"
#include "QLogger.h" //4.0.3.58

class CTrsLog : public CQLogger //4.0.3.58  
{
public:
	void SetActiveParam(BOOL bActive);
	BOOL IsTrsLogActive();
	long LogTankTotals(long lTankNum, long lGrade, long lVolume);
	long LogPumpTotals(long lPumpNum, short nNuzzle, long lGrade, char* pszVolume, char* pszValueA);
	long LogShift(long lShiftNumber);
	long LogTrs(CPumpTransact *pTrs);
	BOOL SwapFile();
	virtual void InitWorkingDirectory(char * sPath = NULL);
	void Init();
	CTrsLog();
	virtual ~CTrsLog();

	//4.0.3.58
	virtual long LogCatastrophicFailure();

private:
	BOOL m_bActive;
};



/**************************************************************************************
	Class: CStuckTrsLog 
	Description: This class is used to log removal of stuck transactions from the system
	Technical Details: inherits from CLog
	Log file name: StuckTransactions.log

	Created in version: // 4.0.24.770  TD 275612
****************************************************************************************/

class CStuckTrsLog : public CLog  
{
public:
	CStuckTrsLog() { m_bActive = FALSE; }
	virtual ~CStuckTrsLog() {}
	virtual void Init();
	virtual long LogTrs(CPumpTransact *pTrs);
	virtual BOOL SwapFile() { return FALSE; }
	virtual void InitWorkingDirectory(char * sPath = NULL);

private:
	BOOL m_bActive;

};

#endif // !defined(AFX_TRSLOG_H__5C5FD229_5C79_442A_A5B0_4AE59B506C19__INCLUDED_)
