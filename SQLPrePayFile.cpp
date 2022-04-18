#include "stdafx.h"
#include "SQLPrePayFile.h"
#include "PumpSrvDatabase.h"
#include "PumpClass.h"
#include "TimingProfiling.h"


CSQLPrePayFile::CSQLPrePayFile()
{
}


CSQLPrePayFile::~CSQLPrePayFile()
{
}

long CSQLPrePayFile::Operation(eFileOperation OpCode, void * pInfo, CPumpTransact *pTrs,BOOL bIsSaveByMaint)
{
	TimingProfile(PROFILE_ALL, _T(""), true);

	long lRetVal = OK;
	try
	{
		CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
		PUMPSRV_PRE_PAY_REC	pPrePayRec;
		PRE_PAY_INFO	*pPrePayInfo = NULL;

		memset(&pPrePayRec, 0, sizeof(PUMPSRV_PRE_PAY_REC));
		if (pInfo)
		{
			pPrePayInfo = (PRE_PAY_INFO *)(pInfo);
			pPrePayRec.info = *pPrePayInfo;

		}

		if (pTrs)
		{
			pPrePayRec.info.lSyncNumber = pTrs->GetLinkNumber();
			pPrePayRec.trs = *pTrs;
		}

		switch (OpCode)
		{
		case REC_INSERT:
			lRetVal = PumpSrvDataBase.SavePrePayInfo(pPrePayRec, pPrePayRec.info.lSyncNumber) ? OK : 1;
			break;
		case REC_READ:
			lRetVal = PumpSrvDataBase.LoadPrePayInfo(pPrePayRec, pPrePayRec.info.lSyncNumber) ? OK : 1;
			if (lRetVal == OK)
			{
				memcpy(pInfo, &pPrePayRec.info, sizeof(PRE_PAY_INFO));
				*pTrs = pPrePayRec.trs;
			}

			break;
		case REC_UPDATE:
			if (pInfo == NULL)
			{
				pPrePayRec.info.lSyncNumber = pTrs->GetLinkNumber();
				lRetVal = PumpSrvDataBase.LoadPrePayInfo(pPrePayRec, pPrePayRec.info.lSyncNumber) ? OK : 1;
				pPrePayRec.trs = *pTrs;
				lRetVal = PumpSrvDataBase.SavePrePayInfo(pPrePayRec, pPrePayRec.info.lSyncNumber) ? OK : 1;
			}
			else
			{
				pPrePayRec.info = *(PUMPSRV_PRE_PAY_INFO *)pInfo;
				pPrePayRec.info.lSyncNumber = pTrs->GetLinkNumber();

				pPrePayRec.trs = *pTrs;
				lRetVal = PumpSrvDataBase.SavePrePayInfo(pPrePayRec, pPrePayRec.info.lSyncNumber) ? OK : 1;
			}

			break;
		case REC_DELETE:
			lRetVal = PumpSrvDataBase.DeletePrepayInfo(pPrePayRec.info.lSyncNumber) ? OK : 1;
			break;
		case REC_READ_FIRST:
			lRetVal = PumpSrvDataBase.LoadFirstPrepayInfo(pPrePayRec) ? OK : 1;
			memcpy(pInfo, &pPrePayRec, sizeof(PRE_PAY_INFO));
			*pTrs = pPrePayRec.trs;
			break;
		case REC_READ_NEXT:
			lRetVal = PumpSrvDataBase.LoadNext(pPrePayRec.info.lSyncNumber, pPrePayRec) ? OK : 1;
			memcpy(pInfo, &pPrePayRec, sizeof(PRE_PAY_INFO));
			*pTrs = pPrePayRec.trs;
			break;
		case REC_DELETE_TILL_INDEX:
			break;

		}
	}
	catch (...)
	{
		CString csLogMessage;
		csLogMessage.Format("%s:%d Unexpected Exception", __FUNCTION__, __LINE__);

		_Module.m_server.m_cLogger.LogMsg(csLogMessage, LOG_LEVEL_0);

		return S_FALSE;
	}

	return lRetVal;
}