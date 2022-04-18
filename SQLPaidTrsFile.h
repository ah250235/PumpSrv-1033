#pragma once

#include "PumpSrvStructures.h"
#include "db.h"
using namespace PS;

class CSQLPaidTrsFile
{
public:
	CSQLPaidTrsFile(BOOL bIsTest = FALSE);
	~CSQLPaidTrsFile();
	BOOL CheckIfSaveToQDX(PAID_TRS_EX & cTrs);
	int GetPaidTrsCount(long lPumpNumber);
	BOOL GetPaidTrsActive();
	long Operation(eFileOperation OpCode, void * pInfo, BOOL bIsSaveByMaint = FALSE);

	CComAutoCriticalSection m_csFile;
	DWORD	m_lGetPaidPAK;
	DWORD	m_lGetPaidPAP;
	DWORD	m_lGetPaidAVI;
	DWORD	m_lGetPaidAtt;
	DWORD	m_lGetPaidPRP;
	DWORD	m_lMaxNumOfPaidTrs;

private:
	BOOL m_bIsTest;
};

