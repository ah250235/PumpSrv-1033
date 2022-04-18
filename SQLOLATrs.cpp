#include "stdafx.h"
#include "SQLOLATrs.h"
#include "PumpSrvDatabase.h"

CSQLOLATrs::CSQLOLATrs()
{
}


CSQLOLATrs::~CSQLOLATrs()
{
}

long CSQLOLATrs::OperationEx(eFileOperation OpCode, CPumpTransact * pTrs, void * pInfo) //4.0.18.501
{
	long lRetVal = OK;
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();

	PAY_AT_PUMP_INFO *pPAPInfoToQdx = NULL;
	if (pInfo)
		pPAPInfoToQdx = (PAY_AT_PUMP_INFO*)pInfo;

	if (OpCode == REC_INSERT || OpCode == REC_UPDATE)
		_Module.m_server.m_cProtectedDataHandler.RemoveConfidencesData(pPAPInfoToQdx);
		
	if (OpCode == REC_INSERT || (OpCode == REC_READ) || (OpCode == REC_UPDATE) || (OpCode == REC_READ_NEXT))
	{
		if (pPAPInfoToQdx)
			pPAPInfoToQdx->CardSaleInfo.lIndexNumber = pTrs->GetLinkNumber();
	}

	switch (OpCode)
	{
		case REC_INSERT:
		{
			lRetVal = PumpSrvDataBase.SaveOLATrs(pPAPInfoToQdx->CardSaleInfo.lIndexNumber,*pPAPInfoToQdx, *pTrs) ? OK : 1;
			break;
		}
		case REC_READ:
		{
			lRetVal = PumpSrvDataBase.LoadOLATrs(pPAPInfoToQdx->CardSaleInfo.lIndexNumber,*pPAPInfoToQdx, *pTrs) ? OK : 1;
			break;
		}
		case REC_READ_FIRST:
		{
			lRetVal = PumpSrvDataBase.LoadFirstOLATrs(*pPAPInfoToQdx, *pTrs) ? OK : 1;
			break;
		}

		case REC_UPDATE:
		{
			lRetVal = PumpSrvDataBase.SaveOLATrs(pPAPInfoToQdx->CardSaleInfo.lIndexNumber,*pPAPInfoToQdx, *pTrs) ? OK : 1;
			break;
		}
		case REC_DELETE:
		{
			lRetVal = PumpSrvDataBase.DeleteOLATrs(pTrs->GetLinkNumber()) ? OK : 1;
			if (lRetVal)
			{
				CString sMsg;
				sMsg.Format("Fail to DELETE ola record=%ld, lRetVal=%ld", pPAPInfoToQdx->CardSaleInfo.lIndexNumber, lRetVal);
				_LOGMSG.LogMsg(sMsg, LOG_LEVEL_0);
			}
			break;
		}

		case REC_READ_NEXT:
		{
			lRetVal = PumpSrvDataBase.LoadNextOLATrs(pPAPInfoToQdx->CardSaleInfo.lIndexNumber, *pPAPInfoToQdx, *pTrs) ? OK : 1;
			break;
		}
	}

	return lRetVal;
}

