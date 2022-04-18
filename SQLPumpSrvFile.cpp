#include "stdafx.h"
#include "SQLPumpSrvFile.h"
#include "PumpSrvStructures.h"
#include "PumpClass.h"
#include "PumpSrvDatabase.h"
#include "PumpSrvRealTimeInfoDB.h"
#include "Logger.h"
#include "ExeModule.h"
#include "PumpTransact.h"

CSQLPumpSrvFile::CSQLPumpSrvFile()
{
}

CSQLPumpSrvFile::~CSQLPumpSrvFile()
{
}

CSQLPumpSrvFile & CSQLPumpSrvFile::GetInstance(void)
{
	static CSQLPumpSrvFile SQLPumpSrvFile;

	return SQLPumpSrvFile;
}

long CSQLPumpSrvFile::SaveInfo(long lPumpNumber, LPCSTR szFunction, int LineNumber, BOOL bLocal)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	PUMP_INFO	cPumpInfo;
	long		lFlags = 0;
	long		lClientEventData = LOAD_PUMPSTAT_INFO;
	long		lPumpNumMask = lPumpNumber << 16;			//4.0.0.78 - Pump number data is passed in bit 17 and higher.

	lClientEventData |= lPumpNumMask;

	if (PUMP_NOT_VALID(lPumpNumber))  //4.0.20.56
	{
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("CPumpFile::SaveInfo - PUMP_NOT_VALID %ld", lPumpNumber);
			_LOGMSG.LogMsg(str, LOG_LEVEL_1);
		}
		return 0;
	}

	_Module.m_server.GetPumpInfo(lPumpNumber, &cPumpInfo);

	return (PumpSrvDatabase.SavePumpInfo(cPumpInfo, lPumpNumber, szFunction, LineNumber) ? OK : 1);
}

long CSQLPumpSrvFile::LoadInfo(long lPumpNumber, BOOL bForMultiSync) //4.0.12.513 (add param bForMultiSync)  
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	PUMP_INFO pumpInfo;
	memset(&pumpInfo, 0, sizeof(PUMP_INFO));

	if (sizeof(PUMP_INFO) > MAX_PUMP_INFO_SIZE)
	{
		CString str;
		str.Format("%s:%d - the sizeof PUMP_INFO (%d), is larger than the allocated DB buffer (%d)", __FUNCTION__, __LINE__, sizeof(PUMP_INFO), MAX_PUMP_INFO_SIZE);
		_LOGMSG.LogMsg(str, LOG_LEVEL_1);
	}

	if (!PumpSrvDatabase.LoadPumpInfo(pumpInfo, lPumpNumber))
	{
		return 0;
	}

	CPumpTransact cCurrTrsInfo; //4.0.12.511
	CPump cTmpPump;

	cTmpPump = _Module.m_server.m_cPumpArray[lPumpNumber - 1];

	cTmpPump.SetNumber(lPumpNumber);
	cTmpPump.SetType(pumpInfo.m_lType);
	cTmpPump.SetName((char *)pumpInfo.m_sPumpMenufacture, MAX_NAME_LENGTH);


	{
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);
		cTmpPump.SetStat(pumpInfo.m_cPumpStat);
	}

	if (lPumpNumber > _Module.m_server.GetMaxPumps())
	{
		cTmpPump.SetLockStatus(UNLOCK);
		cTmpPump.SetShiftNumber(_Module.m_server.GetActiveShiftNumber());
	}

	else
	{
		cTmpPump.SetLockStatus(pumpInfo.m_lLockStatus);
		cTmpPump.SetShiftNumber(pumpInfo.m_lShiftNumber);
	}

	cTmpPump.SetOlaStat(&pumpInfo.m_cOLA);

	for (int t = 0; t < MAX_TRS_IN_LIST; t++)
	{
		if (bForMultiSync)
		{
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsByIndex(t, &cCurrTrsInfo);

			if ((cCurrTrsInfo.m_lNumber == pumpInfo.m_cTrs[t].m_lNumber) && (cCurrTrsInfo.m_lNumber != 0)) //4.0.20.505
			{
				CString str;
				str.Format("the current trs=%ld, time=%s, qdx trs=%ld, time=%s", cCurrTrsInfo.m_lNumber, cCurrTrsInfo.GetTime().Format("%D %H:%M:%S"), pumpInfo.m_cTrs[t].m_lNumber, CTime(pumpInfo.m_cTrs[t].m_cStartTime).Format("%D %H:%M:%S"));
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
			}

			//Check who is the more updated transaction:
			//First check if it is the same transaction number, if it is - Paid trs come before Unpaid.
			if ((cCurrTrsInfo.m_lNumber == pumpInfo.m_cTrs[t].m_lNumber) &&
				(cCurrTrsInfo.m_lStatus == UNPAID && pumpInfo.m_cTrs[t].m_lStatus == PAID))
			{
				cTmpPump.SetTrsByIndex(t, &pumpInfo.m_cTrs[t], FALSE);

				if (cCurrTrsInfo.m_lNumber > 0)	//4.0.20.505
				{
					CString str;
					str.Format("Take the trs information from the qdx, status more updated.");
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
				}
			}
			//If there is no decision yet, Update the transaction information from the qdx only if the qdx information is more new. 
			else if (cCurrTrsInfo.GetTime() <= CTime(pumpInfo.m_cTrs[t].m_cStartTime).GetTime()) //4.0.12.516 Add equal
			{
				cTmpPump.SetTrsByIndex(t, &pumpInfo.m_cTrs[t], FALSE);

				if ((cCurrTrsInfo.m_lNumber == pumpInfo.m_cTrs[t].m_lNumber) && (cCurrTrsInfo.m_lNumber != 0)) //4.0.15.450
				{
					CString str;
					str.Format("Take the trs information from the qdx, time more updated.");
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
				}
			}
		}
		else
		{
			cTmpPump.SetTrsByIndex(t, &pumpInfo.m_cTrs[t], FALSE);
		}
	}

	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	_Module.m_server.m_cPumpArray.SetAtGrow(lPumpNumber - 1, cTmpPump);

	return 0;
}

long CSQLPumpSrvFile::LoadServerInfo(void)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	SERVER_INFO info;

	CSingleLock slServerInfo(&_Module.m_server.m_cServerInfo.m_csServerInfo, TRUE);

	memset(&info, 0, sizeof(SERVER_INFO));
	BOOL bRetCode = PumpSrvDatabase.LoadServerInfo(info);
	if (bRetCode)
	{
		_Module.m_server.m_cServerInfo.SetInfo(info);

		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("Load Shift Info , File OffSet %d, BlockSize %d",
				offsetof(PUMP_FILE, currentShift),
				sizeof(SHIFT_REC));
			_LOGMSG.LogMsg(str);
		}
	}

	return !bRetCode;
}

long CSQLPumpSrvFile::SaveServerInfo(void)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	SERVER_INFO info;

	CSingleLock slServerInfo(&_Module.m_server.m_cServerInfo.m_csServerInfo, TRUE);

	memset(&info, 0, sizeof(SERVER_INFO));

	// get server main data into server info	
	_Module.m_server.m_cServerInfo.GetInfo(info);   //4.0.8.500

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("Save Server Info , File OffSet %d, BlockSize %d",
			offsetof(PUMP_FILE, serverInfo),
			sizeof(SERVER_INFO)
		);
		_LOGMSG.LogMsg(str);
	}

	BOOL bRetCode = PumpSrvDatabase.SaveServerInfo(info);

	return !bRetCode;
}

long CSQLPumpSrvFile::LoadShiftInfo()
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	PS::SHIFT_REC info;
	memset(&info, 0, sizeof(PS::SHIFT_REC));

	if (PumpSrvDatabase.LoadShift(info))
	{
		_Module.m_server.SetCurrentShift(&info);

		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("Load Shift Info , File OffSet %d, BlockSize %d",
				offsetof(PUMP_FILE, currentShift),
				sizeof(SHIFT_REC));
			_LOGMSG.LogMsg(str);
		}
	}
	else
	{
		return 1;
	}

	return 0;
}

long CSQLPumpSrvFile::SaveShiftInfo()
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	SHIFT_REC info;
	memset(&info, 0, sizeof(SHIFT_REC));

	// get server main data into server info	
	_Module.m_server.GetCurrentShift(&info);

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("Save Shift Info , File OffSet %d, BlockSize %d",
			offsetof(PUMP_FILE, currentShift),
			sizeof(SHIFT_REC));
		_LOGMSG.LogMsg(str);
	}

	if (!PumpSrvDatabase.SaveShift(info))
	{
		return 1;
	}

	return 0;
}

long CSQLPumpSrvFile::SavePAPInfo(long lPumpNumber, LPCSTR szFunction, int LineNumber)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	PAY_AT_PUMP_INFO papInfo;

	long lRtc = 0;

	if (PUMP_NOT_VALID(lPumpNumber))  //4.0.20.56
	{
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("CPumpFile::SavePAPInfo - PUMP_NOT_VALID %ld", lPumpNumber);
			_LOGMSG.LogMsg(str, LOG_LEVEL_0);
		}
		return 0;
	}

	try	//4.0.19.502
	{
		memset(&papInfo, 0, sizeof(PAY_AT_PUMP_INFO));
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&papInfo);

		_Module.m_server.m_cProtectedDataHandler.RemoveConfidencesData(&papInfo); // RFUEL-1347

		PumpSrvDatabase.SavePayAtPumpInfo(papInfo, lPumpNumber, szFunction, LineNumber);
	}
	catch (...)	//4.0.19.502
	{
		_LOGMSG.LogMsg("CPumpFile::SavePAPInfo, Unexpected error has occured", LOG_LEVEL_1);
	}

	return 0;
}

long CSQLPumpSrvFile::LoadPAPInfo(long lPumpNumber)
{//4.0.18.501
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	PAY_AT_PUMP_INFO papInfo;
	memset(&papInfo, 0, sizeof(PAY_AT_PUMP_INFO));

	if (PumpSrvDatabase.LoadPayAtPumpInfo(papInfo, lPumpNumber))
	{
		PumpSrvDatabase.LoadPayAtPumpInfo(papInfo, lPumpNumber);

		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		//If a new pump has been added during run time, we assume
		//that LoadInfo has already been called, otherwise pump index will
		//be invalid!
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&papInfo);
	}
	//4.0.5.26

	return 0;
}

long CSQLPumpSrvFile::SaveRealTimeInfo(long lPumpNumber, LPCSTR szFunction, int LineNumber, BOOL bLocal)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();

	long	lRtc = 0;
	long	lFlags = 0;
	long	lClientEventData = LOAD_PUMPSTAT_INFO;
	long	lPumpNumMask = lPumpNumber << 16;			//4.0.0.78 - Pump number data is passed in bit 17 and higher.

	lClientEventData |= lPumpNumMask;

	if (PUMP_NOT_VALID(lPumpNumber))  //4.0.20.56
	{
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("CPumpFile::SaveInfo - PUMP_NOT_VALID %ld", lPumpNumber);
			_LOGMSG.LogMsg(str, LOG_LEVEL_1);
		}
		return 0;
	}

	try	//4.0.19.502
	{
		PUMP_INFO	cPumpInfo;
		PAY_AT_PUMP_INFO papInfo;

		memset(&papInfo, 0, sizeof(PAY_AT_PUMP_INFO));
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&papInfo);
		_Module.m_server.GetPumpInfo(lPumpNumber, &cPumpInfo);

		_Module.m_server.m_cProtectedDataHandler.RemoveConfidencesData(&papInfo); // RFUEL-1347

		lRtc = PumpSrvDatabase.SaveRealTimeInfo(papInfo, cPumpInfo, lPumpNumber, szFunction, LineNumber);
	}
	catch (...)	//4.0.19.502
	{
		CString csMessage;
		csMessage.Format("%s, Unexpected error has occured", __FUNCTION__);
		_LOGMSG.LogMsg(csMessage, LOG_LEVEL_1);
	}

	return lRtc ? OK : 1;
}

