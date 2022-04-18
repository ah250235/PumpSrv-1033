
#include "SQLPaidTrsFile.h"
#include "stdafx.h"
#include "PumpSrvDatabase.h"


CSQLPaidTrsFile::CSQLPaidTrsFile(BOOL bIsTest) : m_bIsTest(bIsTest)
{
	if (m_bIsTest)
	{
		m_lMaxNumOfPaidTrs = 20;
		m_lGetPaidPAK = TRUE;
	}
	
}

CSQLPaidTrsFile::~CSQLPaidTrsFile()
{

}

/******************************************************************************
Description:	Check the type of the trs and according to the flags
/				we save it or not.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amram			5/6/2005   15:51		Start
******************************************************************************/

BOOL CSQLPaidTrsFile::CheckIfSaveToQDX(PAID_TRS_EX & cTrs)
{
	BOOL bRetCode = FALSE;
	long lRes = a2l(cTrs.cTagPump.sRes, sizeof(cTrs.cTagPump.sRes));
	long lResExt = a2l(cTrs.cTagPump.sResExt, sizeof(cTrs.cTagPump.sResExt));
	if ((lRes == 0) && (lResExt == 0))						 
	{
		if ((m_lGetPaidAtt == 0) && (m_lGetPaidAVI == 0) && (m_lGetPaidPAK == 0) && (m_lGetPaidPRP == 0) && (m_lGetPaidPAP == 0))
			bRetCode = FALSE;
		else
			bRetCode = TRUE;
	}
	else
	{
		if ((lRes == PAY_AT_KIOSK) && (lResExt == RES_EXT_ATTENDANT))
			bRetCode = m_lGetPaidAtt;
		else if ((lRes == PAY_AT_PUMP) && (lResExt == RES_EXT_AVI))
			bRetCode = m_lGetPaidAVI;
		else if (lRes == PAY_AT_KIOSK)
			bRetCode = m_lGetPaidPAK;
		else if (lRes == PRE_PAY_AT_POS)
			bRetCode = m_lGetPaidPRP;
		else if (lRes == PAY_AT_PUMP)
			bRetCode = m_lGetPaidPAP;
	}

	if (!(bRetCode))
		_LOGMSG.LogMsg("CPumpPaidTrs::CheckIfSaveToQDX Can't read from QDX all flags are FALSE");


	return bRetCode;
}

int CSQLPaidTrsFile::GetPaidTrsCount(long lPumpNumber)
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	return PumpSrvDataBase.GetPaidTrsCount(lPumpNumber);
}

BOOL CSQLPaidTrsFile::GetPaidTrsActive()
{
	BOOL bRetCode = FALSE;
	if ((m_lGetPaidAtt == 0) && (m_lGetPaidAVI == 0) && (m_lGetPaidPAK == 0) &&
		(m_lGetPaidPRP == 0) && (m_lGetPaidPAP == 0))
		bRetCode = FALSE;
	else
		bRetCode = TRUE;

	return bRetCode;
}

long CSQLPaidTrsFile::Operation(eFileOperation OpCode, void * pInfo, BOOL bIsSaveByMaint)
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	long lRetVal = OK;
	PAID_TRS_EX		*pPaidTrsInfo = NULL;
	pPaidTrsInfo = (PAID_TRS_EX *)pInfo;
	m_csFile.Lock();

	if (FALSE == CheckIfSaveToQDX(*pPaidTrsInfo))
		return ERR_NOT_FOUND;

	long lPumpNumber = a2l(pPaidTrsInfo->cTagPump.sPump, sizeof pPaidTrsInfo->cTagPump.sPump);
	long lIndex = a2l(pPaidTrsInfo->cTagPump.sNumber, sizeof pPaidTrsInfo->cTagPump.sNumber);

	switch (OpCode)
	{
	case REC_INSERT:
	{
		long lNumOfTrs = GetPaidTrsCount(lPumpNumber);
		if (lNumOfTrs < (long)m_lMaxNumOfPaidTrs)
		{
			lRetVal = PumpSrvDataBase.SavePaidTrs(*pPaidTrsInfo, lIndex, lPumpNumber) ? OK : 1;
			
		}
		else
		{
			lRetVal = PumpSrvDataBase.LoadFirstPaidTrs(*pPaidTrsInfo) ? OK : 1;
			long lIndex = a2l(pPaidTrsInfo->cTagPump.sNumber, sizeof pPaidTrsInfo->cTagPump.sNumber);
			if(lRetVal)
				lRetVal = PumpSrvDataBase.Delete(*pPaidTrsInfo, lIndex) ? OK : 1;
			if (lRetVal)
				lRetVal = PumpSrvDataBase.SavePaidTrs(*pPaidTrsInfo, lIndex, lPumpNumber) ? OK : 1;

		}

	}
	break;

	case REC_READ:
		lRetVal = PumpSrvDataBase.Load(*pPaidTrsInfo, lIndex) ? OK : 1;
		break;

	case REC_UPDATE:
		lRetVal = PumpSrvDataBase.SavePaidTrs(*pPaidTrsInfo, lIndex, lPumpNumber) ? OK : 1;
		break;

	case REC_DELETE:
		lRetVal = PumpSrvDataBase.Delete(*pPaidTrsInfo, lIndex) ? OK : 1;
		break;

	case REC_READ_FIRST:
		lRetVal = PumpSrvDataBase.LoadFirstPaidTrs(*pPaidTrsInfo) ? OK : 1;
		break;

	case REC_READ_NEXT:
		break;

	case REC_DELETE_TILL_INDEX:
		break;
	}

	m_csFile.Unlock();

	return lRetVal;
}

