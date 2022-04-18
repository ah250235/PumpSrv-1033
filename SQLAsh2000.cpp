#include "stdafx.h"
#include "SQLAsh2000.h"
#include "PumpSrvDatabase.h"


CSQLAsh2000::CSQLAsh2000()
{
}


CSQLAsh2000::~CSQLAsh2000()
{
}

long CSQLAsh2000::Operation(eFileOperation OpCode, void * pInfo)
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	long lRetVal = 1;
	ASH2000_REC	*ash2000Record = NULL;

	if (pInfo)
		ash2000Record = (ASH2000_REC *)(pInfo);


	switch (OpCode)
	{
	case REC_INSERT:
		lRetVal = PumpSrvDataBase.SaveAsh200Record(*ash2000Record, ash2000Record->lIndexNumber) ? OK : 1;
		break;
	case REC_READ:
		lRetVal = PumpSrvDataBase.LoadAsh2000Record(*ash2000Record, ash2000Record->lIndexNumber) ? OK : 1;
		break;
	case REC_UPDATE:
		lRetVal = PumpSrvDataBase.SaveAsh200Record(*ash2000Record, ash2000Record->lIndexNumber) ? OK : 1;
		break;
	case REC_DELETE:
		lRetVal = PumpSrvDataBase.Delete(*ash2000Record) ? OK : 1;
		break;
	case REC_READ_FIRST:
		lRetVal = PumpSrvDataBase.LoadFirstAsh2000Record(*ash2000Record) ? OK : 1;
		break;
	}

	return lRetVal;
}

long CSQLAsh2000::InsertData(long lPumpNumber, void *pData, long lSize, CPumpTransact  *pTrs)
{
	CPumpTransact	cTrs;
	ASH2000_REC		cASH2000_REC;
	BOOL			bFound = FALSE;
	CString str;
	long lRtc = 0;

	if (pTrs)
	{
		cTrs = *pTrs;
		bFound = TRUE;
	}
	else
	{
		cTrs.SetLink(TRS_LINK_OLA_NUMBER, _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPSeqNumber());

		if (!_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsByLinkNumber(&cTrs))
		{
			bFound = TRUE;
		}
	}

	if (bFound)
	{
		memset(&cASH2000_REC, 0, sizeof(ASH2000_REC));

		cASH2000_REC.lIndexNumber = cTrs.m_lNumber;
		if (Operation(REC_READ, &cASH2000_REC))
		{
			memset(cASH2000_REC.sData, ' ', sizeof(cASH2000_REC.sData));
			memcpy(cASH2000_REC.sData, (char *)pData, min(lSize, sizeof(cASH2000_REC.sData)));
			lRtc = Operation(REC_INSERT, &cASH2000_REC);
		}
		else
		{
			cASH2000_REC.lIndexNumber = cTrs.m_lNumber;
			memset(cASH2000_REC.sData, ' ', sizeof(cASH2000_REC.sData));
			memcpy(cASH2000_REC.sData, (char *)pData, min(lSize, sizeof(cASH2000_REC.sData)));

			lRtc = Operation(REC_UPDATE, &cASH2000_REC);
		}

		if (LOG_DETAIL_FLOW_CONTROL)
		{
			str.Format("CSQLAsh2000 - InsertData , Index=%ld lRtc=%ld", cTrs.m_lNumber, lRtc);
			_LOGMSG.LogMsg(str);
		}


	}
	else
	{
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			str.Format("CSQLAsh2000 - InsertData Not found Index number");
			_LOGMSG.LogMsg(str);
		}


	}

	return lRtc;
}