#include "stdafx.h"
#include "SQLOLATempTrs.h"
#include "PumpSrvDatabase.h"

CSQLOLATempTrs::CSQLOLATempTrs()
{
}


CSQLOLATempTrs::~CSQLOLATempTrs()
{
}

long CSQLOLATempTrs::Operation(eFileOperation OpCode, CPumpTransact * pTrs, void * pInfo) //4.0.18.501
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	long lRetVal = OK;
	PAY_AT_PUMP_INFO *pPAPInfoToQdx = NULL;                      // TD 347994 

	if (pInfo)
	{ 
		pPAPInfoToQdx = (PAY_AT_PUMP_INFO*)pInfo;
		if(pTrs)
			pPAPInfoToQdx->CardSaleInfo.lIndexNumber = pTrs->GetLinkNumber();
	}

	if ((OpCode == REC_INSERT) || (OpCode == REC_UPDATE))
		_Module.m_server.m_cProtectedDataHandler.RemoveConfidencesData(pPAPInfoToQdx);
		
	

	switch (OpCode)
	{
	case REC_INSERT:
	{
		lRetVal = PumpSrvDataBase.SaveOLATempTrs(pPAPInfoToQdx->CardSaleInfo.lIndexNumber, *pPAPInfoToQdx, *pTrs) ? OK : 1;
		break;
	}
	case REC_READ:
	{
		lRetVal = PumpSrvDataBase.LoadOLATempTrs(pPAPInfoToQdx->CardSaleInfo.lIndexNumber,*pPAPInfoToQdx, *pTrs) ? OK : 1;
		break;
	}
	case REC_READ_FIRST:
	{
		lRetVal = PumpSrvDataBase.LoadFirstOLATempTrs(*pPAPInfoToQdx, *pTrs) ? OK : 1;
		break;
	}

	case REC_UPDATE:
		break;
	
	case REC_DELETE:
	{
		lRetVal = PumpSrvDataBase.DeleteOLATempTrs(pPAPInfoToQdx->CardSaleInfo.lIndexNumber) ? OK : 1;
		if (FALSE == lRetVal)
		{
			CString sMsg;
			sMsg.Format("Fail to DELETE olaTemp record=%ld, lRetVal=%ld", pPAPInfoToQdx->CardSaleInfo.lIndexNumber, lRetVal);
			_LOGMSG.LogMsg(sMsg, LOG_LEVEL_0);
		}

		break;
	}

	case REC_READ_NEXT:
	{
		lRetVal = PumpSrvDataBase.LoadNextOLATempTrs(pPAPInfoToQdx->CardSaleInfo.lIndexNumber,*pPAPInfoToQdx, *pTrs) ? OK : 1;
		break;
	}
	}

	return lRetVal;
}

