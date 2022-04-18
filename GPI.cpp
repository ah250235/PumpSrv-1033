// GPI.cpp : Implementation of CGPI
#include "stdafx.h"
#include "PumpSrv.h"
#include "GPI.h"

#include "OLAPositiveSrv.h"
#include "OLATcpBase.h"
#include "OLARouteSrv.h"
#include "OLAPositiveEventSrv.h"
#include "OLAPositive_FIPAYSrv.h"
#include "OLAProgressSrv.h"
#include "OLATcpPositiveSrv.h"
#include "AlarmConvert.h"
#include "FleetCard.h"	//4.0.9.507
#include "PumpSrvDatabase.h"
#include "commonDef.h"
#include "SQLReceiptDB.h"
#include "RapidJsonIncludes.h"
#include <iostream>
#include <cctype>

/////////////////////////////////////////////////////////////////////////////
// CGPI

#define LOG_EXTERNALCALL(a) _LOGMSG.LogClassMsg("GPI", "External Call", m_lPosNumber, LOG_NONE, (a), LOG_LEVEL_4);

STDMETHODIMP CGPI::InterfaceSupportsErrorInfo(REFIID riid)
{
	LOG_EXTERNALCALL("CGPI::InterfaceSupportsErrorInfo   start");
	static const IID* arr[] =
	{
		&IID_IGPI,
	};
	for (int i = 0; i<sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i], riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CGPI::InitInterface(long lPosNumber, long lVersion, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::InitInterface   start");


#ifdef CATCH_GPI
	__try
	{
#endif
		InitInterface__(lPosNumber, lVersion, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::InitInterface"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("GPI", "InitInterface()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


void CGPI::InitInterface__(long lPosNumber, long lVersion, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	BOOL		bRegPos = TRUE; // register new pos 

	CSingleLock slPosMap(&(_Module.m_server.m_csPosMap));
	CString str;
	str.Format("POS Version: %ld, POSPump Version: %ld", lVersion / 100000000, lVersion % 100000000);

	if (lPosNumber == SERVER_POS_NUMBER)
	{
		retCode = GPI_INVALID_POS_NUMBER;
		bRegPos = FALSE;
	}
	else
	{
		if (m_lPosNumber == NO_POS_NUMBER)
		{
			bRegPos = TRUE;
		}
		else
		{
			CPosInfo tmpPos;
			slPosMap.Lock(INFINITE);
			if (_Module.m_server.m_cPosMap.Lookup(m_lPosNumber, tmpPos))
			{
				tmpPos.RemoveEventReceipt();
			}

			_Module.m_server.m_cPosMap.RemoveKey(m_lPosNumber);
			slPosMap.Unlock();
			bRegPos = TRUE;
		}
	}

	if (bRegPos) // need to register pos
	{
		CPosInfo tmpPos;

		m_lPosNumber = lPosNumber;

		if (_Module.m_server.m_cPosMap.Lookup(lPosNumber, tmpPos))
		{
			if (tmpPos.GetEventReceipt() == NULL)		//4.0.19.508 (61994) {
			{
				slPosMap.Lock(INFINITE);

				tmpPos.CreateEventReceipt();
				_Module.m_server.m_cPosMap.SetAt(m_lPosNumber, tmpPos);

				slPosMap.Unlock();
			}

			if (LOG_BASIC_FLOW_CONTROL)
			{
				_LOGMSG.LogReq(m_lPosNumber, "IGPI", "InitInterface", "re-connected to server");
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, str);
			}
		}
		else
		{
			CPosInfo newPos(m_lPosNumber, lVersion);

			slPosMap.Lock(INFINITE);

			newPos.CreateEventReceipt();
			_Module.m_server.m_cPosMap.SetAt(m_lPosNumber, newPos);

			slPosMap.Unlock();

			if (LOG_BASIC_FLOW_CONTROL)
			{
				_LOGMSG.LogReq(m_lPosNumber, "IGPI", "InitInterface", "connected to server");
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, str);
			}
		}


		m_bInitDone = TRUE;
	}

	for (int i = 0; i < _Module.m_server.GetLastValidPump(); i++)  //4.0.0.79
	{
		m_nPumpVer[i] = PUMP_START_VER;
		m_nPumpDetailedVer[i] = PUMP_START_VER;
	}

	if (LOG_BASIC_FLOW_CONTROL)
	{
		DWORD dwThreadId = GetCurrentThreadId();
		CString tmpStr;
		tmpStr.Format("InitInterface, thread ID = %ld", dwThreadId);
		_LOGMSG.LogMsg("CGPI", "InitInterface", lPosNumber, LOG_POS, tmpStr);
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				lPosNumber, // Src
				"IGPI",
				"InitInterface",
				TRUE, retCode,
				FALSE, 0);


	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetServerState(long * pVal)
{
//	LOG_EXTERNALCALL("CGPI::GetServerState   start");


#ifdef CATCH_GPI
	__try
	{
#endif
		GetServerState__(pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetServerState"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("GPI", "GetServerState()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::GetServerState__(long *pVal)
{
	if (pVal != NULL)
		*pVal = _Module.m_server.GetServerState();
}

STDMETHODIMP CGPI::GetNumberOfPumps(long * pVal)
{
//	LOG_EXTERNALCALL("CGPI::GetNumberOfPumps   start");


#ifdef CATCH_GPI
	__try
	{
#endif
		GetNumberOfPumps__(pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetNumberOfPumps"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("GPI", "GetNumberOfPumps()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::GetNumberOfPumps__(long *pVal)
{
	int iNumberOfConfiguredPumps = 0;

	if (pVal != NULL)
	{
		if (SERVER_NOT_IDLE)
			*pVal = 0;
		else
		{
			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			//for(int p =0; p< MAX_PUMPS; p++)
			for (int p = 0; p < _Module.m_server.GetLastValidPump(); p++)  //4.0.0.79
			{
				if ((_Module.m_server.m_cPumpArray[p].GetStat().GetStatus() != UNKNOWN_STATUS) &&
					(!_Module.m_server.m_cPumpArray[p].IsPumpDefineAsTerminal()))    //4.0.3.60					
					iNumberOfConfiguredPumps++;
			}

			*pVal = iNumberOfConfiguredPumps;
		}
	}
}

STDMETHODIMP CGPI::GetPumpInfo(long lPumpNumber, BSTR * pRec, long *pVal)
{
#ifdef DEBUG
	if (LOG_ALL_FLOW_CONTROL)
		LOG_EXTERNALCALL("CGPI::GetPumpInfo   start");
#endif
	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPumpInfo, Throw COM Exception : E_POINTER", LOG_LEVEL_0);


		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetPumpInfo__(lPumpNumber, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetPumpInfo"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPumpInfo()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);
		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::GetPumpInfo__(long lPumpNumber, BSTR *pRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	TAG_PUMP		pump;
	memset(&pump, ' ', sizeof(TAG_PUMP));


	if (retCode == GPI_OK)
	{
		long p = lPumpNumber - 1;


		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		_Module.m_server.m_cPumpArray[p].GetTag(&pump);
	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPumpInfo",
				TRUE, retCode,
				FALSE, 0);
	}

	BYTE sTmp[sizeof(TAG_PUMP) + 2];
	Str2StrNull((BYTE *)&pump, sizeof(TAG_PUMP), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	//Save the last time the POS call GPI method.
	_Module.m_server.m_cPosMap[m_lPosNumber].SetLastCmd(CMD_NULL, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetPumpStatus(long lPumpNumber, BSTR * pRec, long *pVal)
{
#ifdef DEBUG
	if (LOG_ALL_FLOW_CONTROL)
		LOG_EXTERNALCALL("CGPI::GetPumpStatus   start");
#endif


	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPumpStatus, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetPumpStatus__(lPumpNumber, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetPumpStatus"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPumpStatus()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::GetPumpStatus__(long lPumpNumber, BSTR *pRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	TAG_PUMP_STATUS	pumpStat;
	memset(&pumpStat, ' ', sizeof(TAG_PUMP_STATUS));


	if (retCode == GPI_OK)
	{
		long p = lPumpNumber - 1;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		_Module.m_server.m_cPumpArray[p].GetTagStat(&pumpStat);
	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPumpStatus",
				TRUE, retCode,
				FALSE, 0);
	}

	BYTE sTmp[sizeof(TAG_PUMP_STATUS) + 2];
	Str2StrNull((BYTE *)&pumpStat, sizeof(TAG_PUMP_STATUS), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);

	*pRec = strResult.Copy();

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetPumpTransact(long lPumpNumber, long lTrsNumber, BSTR * pRec, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPumpTransact   start");


	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPumpTransact, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetPumpTransact__(lPumpNumber, lTrsNumber, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetPumpTransact"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPumpTransact()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;
}

void CGPI::GetPumpTransact__(long lPumpNumber, long lTrsNumber, BSTR *pRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	TAG_PUMP_TRANSACT	trs;
	memset(&trs, ' ', sizeof(TAG_PUMP_TRANSACT));


	if (retCode == GPI_OK)
	{
		int		iRetNum = 0;
		long	p = lPumpNumber - 1;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		iRetNum = _Module.m_server.m_cPumpArray[p].GetTagTrsByNumber(lTrsNumber, &trs);

		if (iRetNum != 0)
			retCode = GPI_TRS_NOT_FOUND;
	}

	if (retCode)
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPumpTransact",
				TRUE, retCode,
				FALSE, 0);
	}

	BYTE sTmp[sizeof(TAG_PUMP_TRANSACT) + 2];
	Str2StrNull((BYTE *)&trs, sizeof(TAG_PUMP_TRANSACT), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetMinimalPumpsInfo(BSTR * pRec, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::GetMinimalPumpsInfo   start");
	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "GetMinimalPumpsInfo", m_lPosNumber, LOG_POS, "GetMinimalPumpsInfo, Throw COM Exception : E_POINTER", LOG_LEVEL_0);

		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetMinimalPumpsInfo__(pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetMinimalPumpsInfo"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetMinimalPumpsInfo()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GetMinimalPumpsInfo__(BSTR *pRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;


	TAG_MINIMAL_ALL_PUMPS	info;
	memset(&info, ' ', sizeof(TAG_MINIMAL_ALL_PUMPS));

	if (retCode == GPI_OK)
	{
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		//4.0.1.0
		//int nPumps = _Module.m_server.m_cPumpArray.GetSize();
		int nPumps = MAX_PUMPS;

		i2a((short int)nPumps,
			info.sNumberOfPumps,
			(unsigned int)sizeof(info.sNumberOfPumps));

		for (int i = 0; i < MAX_PUMPS; i++)
		{
			_Module.m_server.m_cPumpArray[i].GetTagMinimal(&(info.minimalPump[i]));
		}
	}
	else
	{
		if (LOG_LOGIC_ERROR)

			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetMinimalPumpsInfo",
				TRUE, retCode,
				FALSE, 0);
	}

	BYTE sTmp[sizeof(TAG_MINIMAL_ALL_PUMPS) + 2];
	Str2StrNull((BYTE *)&info, sizeof(TAG_MINIMAL_ALL_PUMPS), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	//Save the last time the POS call GPI method.
	_Module.m_server.m_cPosMap[m_lPosNumber].SetLastCmd(CMD_NULL, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::SetTrsState(long lPumpNumber, long lTrsNumber, long lState, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetTrsState   start");


#ifdef CATCH_GPI
	__try
	{
#endif

		SetTrsState__(lPumpNumber, lTrsNumber, lState, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::SetTrsState"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetTrsState()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 



	return S_OK;
}

void CGPI::SetTrsState__(long lPumpNumber, long lTrsNumber, long lState, long *pVal, long lFlags, BSTR sData) //4.0.22.501 - TD 69904 //4.0.22.501 - TD 69904
{
	GPIRetCode retCode = GPI_OK;
	long lRtc = 0;
	CString		str;
	long lPosNumber = 0;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;
	else if (SERVER_STATE_STARTUP)	//4.0.21.14 68250
	{
		if ((_Module.m_server.ISPAKFinalizationFlagSet()) || (!PUMPSRV_REPORT_ON_TRS_PAID)) //4.0.5.46
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);
		else if (_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
		{
			retCode = GPI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
		}
		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GPI_FAILED_IO;
	else if (SERVER_NOT_IDLE)
	{
		// do not let paid trs when controller is charge on clear trs.		
		if ((_Module.m_server.ISPAKFinalizationFlagSet()) && (!PUMPSRV_REPORT_ON_TRS_PAID))
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());
		else
			retCode = GPI_SERVER_NOT_READY;
	}

	if (lFlags & TRS_WITH_EXTRA_POS_NUMBER)//4.0.11.506 in case lPumpNumber bigger than 100
	{
		str.Format("CGPI::SetTrsState Pump Number %ld lflags = %ld", lPumpNumber, lFlags);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
	}
	else
	{
		if (lPumpNumber)
		{
			if (PUMP_NOT_VALID(lPumpNumber))
				retCode = GPI_INVALID_PUMP_NUMBER;
		}
	}



	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("Pump %02d, Trs %d, State %d , lFlags = %ld", lPumpNumber, lTrsNumber, lState, lFlags);
		_LOGMSG.LogReq(m_lPosNumber,
			"IGPI",
			"SetTrsState",
			(LPCSTR)str);
	}


	if ((retCode == GPI_OK) || (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED))
	{
		CPumpTransact	trs;

		if (lFlags & TRS_DELETE_TRANSACTIONS)
		{

			long lNumOfPaidTrs = 0;
			long lNumOfRecsToDelete = 0;
			PAID_TRS_EX  cPaidTrs;
			char	  sKey[20];
			PAID_TRS_EX  cPaidTrsArr[MAX_PAID_TRS];
			memset(&cPaidTrs, 0, sizeof(cPaidTrs));

			//Delete until the LoyaltyTransactionCount 
			//First, Check the number of records

			long lRtc = 0;
			long lTmpPump = 0;
			memset(&cPaidTrs, ' ', sizeof(cPaidTrs));
			memset(&cPaidTrsArr, ' ', sizeof(cPaidTrsArr));
			memset(&sKey, ' ', sizeof(sKey));

			_Module.m_server.SetPumpNumberToQDX(lPumpNumber, sKey);//4.0.11.510

			lRtc = _Module.m_server.IndexOperation(PAID_TRS_FILE, REC_READ_FIRST, &cPaidTrsArr[0]);

			if (lRtc)
			{
				if (LOG_BASIC_FLOW_CONTROL)
				{
					str.Format("Can't find any active transaction  Rtc=%ld", lRtc);
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
				}
			}
			else
			{

				// Copy all transaction to tmp buffer.
				lTmpPump = a2l(cPaidTrsArr[lNumOfPaidTrs].sIndex, 3);
				if (lTmpPump == lPumpNumber)//if we have at list one row
				{
					lNumOfPaidTrs++;
					while (!(lRtc))
					{
						//copy the key
						memcpy(cPaidTrsArr[lNumOfPaidTrs].sIndex, cPaidTrsArr[lNumOfPaidTrs - 1].sIndex, sizeof(cPaidTrsArr[lNumOfPaidTrs - 1].sIndex));
						lRtc = _Module.m_server.IndexOperation(PAID_TRS_FILE, REC_READ_NEXT, &cPaidTrsArr[lNumOfPaidTrs]);
						if (lRtc)//4.0.11.515
							break;
						lTmpPump = a2l(cPaidTrsArr[lNumOfPaidTrs].sIndex, 3);
						if (lTmpPump != lPumpNumber)//if we have at list one row
							break;
						lNumOfPaidTrs++;
					}
				}

				if (lNumOfPaidTrs)
				{
					// delete extra record , which are more define in GetLoyaltyTransactionCount()
					if (lNumOfPaidTrs > _Module.m_server.GetLoyaltyTransactionCount())
					{
						long lTmpIndex = 0;
						lNumOfPaidTrs -= _Module.m_server.GetLoyaltyTransactionCount();

						while (lNumOfPaidTrs > 0)
						{
							if (LOG_BASIC_FLOW_CONTROL)
							{
								str.Format("Delete from paid Qdx record trs=%.5s", cPaidTrsArr[lTmpIndex].cTagPump.sNumber);
								_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
							}

							lRtc = _Module.m_server.IndexOperation(PAID_TRS_FILE, REC_DELETE, &cPaidTrsArr[lTmpIndex]);
							lTmpIndex++;

							lNumOfPaidTrs--;
						}
					}
				}
			}
		}


		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		long lp, lTrsIndex;
		if ((lPumpNumber == 0) && (lTrsNumber == 0)) // No Trs In Ticket - clear every trs for this POS 
		{
			if (lState == STATE_NOT_ON_OFFER)
			{
				for (lp = 0; lp < _Module.m_server.GetLastValidPump(); lp++)  //4.0.0.79
				{
					for (lTrsIndex = 0; lTrsIndex < _Module.m_server.m_cPumpArray[lp].GetTrsArraySize(); lTrsIndex++)
					{
						_Module.m_server.m_cPumpArray[lp].GetTrsByIndex(lTrsIndex, &trs);

						if ((trs.GetStatus() == UNPAID) || (trs.GetStatus() == ACCEPTED))
						{
							if (trs.GetOnOffer() == m_lPosNumber)
							{
								trs.SetStateRelease(m_lPosNumber);
							}
						}
					}
				}
				retCode = GPI_OK;
			}
			else
			{
				retCode = GPI_TRS_NOT_FOUND;
			}
		}
		else if (lTrsNumber == 0) //4.0.19.504 - 47559
		{
			if (lState == STATE_RELEASE_CONTROLLER)
			{
				if (lFlags & CANCEL_FROM_EXT_TERMINAL)
				{
					long			p = lPumpNumber - 1;

					int iRetVal = _Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&trs);
					if (iRetVal)
					{
						long lTrsNum;
						int iRetVal = _Module.m_server.m_cPumpArray[p].GetCurrentTrsNumber(&lTrsNum);
						if (!iRetVal)
						{
							lTrsNumber = lTrsNum;
						}
					}
					else
						lTrsNumber = trs.m_lNumber;

					if (!iRetVal)
					{
						trs.m_lNumber = lTrsNumber;
						if (!trs.m_lRoundedValue && !trs.m_lRoundedVolume)
						{
							PumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[p].GetStat().GetStatus();

							if ((trs.GetStatus() == WAIT_PROGRESS) && (cPumpStatus != OUT_OF_SERVICE))
								long lRc = _Module.m_server.WideRemoveTrs(&trs, &lRtc);
						}
						else
						{
							//in case there was fueling, send SetTrsState(PAID) to CL and do not cancel
							CMD_STRUCT_RELEASE_CONTROLLER_TRS cTRSCmd;
							cTRSCmd.lTrsNumber = lTrsNumber;
							cTRSCmd.lState = STATE_RELEASE_CONTROLLER;

							_Module.m_server.SetCommand(CMD_RELEASE_CONTROLLER_TRS,
								0,
								trs.m_lPump,
								&cTRSCmd,
								sizeof(cTRSCmd));
						}
					}
				}

				CPumpStatus		pumpStat;
				pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();

				if (((pumpStat.GetRes() == PAY_AT_PUMP) && (pumpStat.GetResExt() == RES_EXT_PAP_EXTERNAL || pumpStat.GetResExt() == RES_EXT_MOBILE)) || (lFlags & CANCEL_FROM_EXT_TERMINAL))
				{
					retCode = CancelTrsByExternal(lTrsNumber, lPumpNumber, pumpStat);
				}
			}
		}
		else
		{
			long p;
			int iRetVal = 1;  //51147, merge from 4.0.16.220

			if (lPumpNumber > POS_NUMBER_PAID_QDX)
				p = lPumpNumber - 1 - POS_NUMBER_PAID_QDX;
			else
				p = lPumpNumber - 1;

			if (p >= 0)
			{
				iRetVal = _Module.m_server.m_cPumpArray[p].GetTrsByNumber(lTrsNumber, &trs);
			}
			else
			{
				if (LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("Pump %02d, Trs %d, State %d , lFlags = %ld Error !!!!!", p, lTrsNumber, lState, lFlags);
					_LOGMSG.LogReq(m_lPosNumber,
						"IGPI",
						"SetTrsState",
						(LPCSTR)str);
				}
			}

			if (lFlags & TRS_LOYALTY_CARD)//4.0.11.504 get the trs from QDX
			{
				iRetVal = !(_Module.m_server.GetTrsFromPaidQdx(lPumpNumber, lTrsNumber, trs));
			}

			//If the transaction was found in the transaction list
			if (iRetVal == 0)
			{
				if ((lState == STATE_FORCE_REMOVE) || (lState == STATE_FORCE_REPAIR)) //4.0.21.12 66982
				{
					if (LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("Force Remove Trs : Trs %d, Status %d, OnOffer %d, link Number %d, Misc %x",
							trs.m_lNumber,
							trs.m_lStatus,
							trs.m_lOnOffer,
							trs.m_lLinkNumber,
							trs.m_lMisc);
						_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
						_Module.m_server.m_cExLog.LogTrs(&trs, NULL, NULL, 1);
					}

					long lRc = _Module.m_server.WideRemoveTrs(&trs, &lRtc);

					if (lRc == 1)
						retCode = GPI_FAILED_IO;

					else if (lRc == 2)
						retCode = GPI_CANT_CANCEL_TRS;

					SendForceRemoveAlarm(lState, trs); // TD 236009 4.0.25.220 // TD 254948/254959 4.0.25.290
				}
				else
				{
					if (LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("Trs Info: (before seting new state) Trs %d, Status %d, OnOffer %d, link Number %d, Misc %x",
							trs.m_lNumber,
							trs.m_lStatus,
							trs.m_lOnOffer,
							trs.m_lLinkNumber,
							trs.m_lMisc);
						_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
					}

					if ((trs.GetStatus() == UNPAID) || (trs.GetStatus() == ACCEPTED))
					{
						retCode = GPI_OK;

						switch (lState)
						{
						case STATE_PAID_LOYLATY:
						{
							long  lRtc = _Module.m_server.DeleteFromPaidTrsQDX(trs);
							if (!(lRtc))
							{
								str.Format("CServerMain::SetAttendantTrs Delete trs num %ld from paid QDX", trs.m_lNumber);
							}
							else
							{
								str.Format("CServerMain::SetAttendantTrs Can't Delete trs num %ld from paid QDX", trs.m_lNumber);
							}
							_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
						}
						break;
						case STATE_NOT_ON_OFFER:
							if (lFlags & TRS_LOYALTY_CARD)
							{
								_Module.m_server.UpdatePaidTrsQDX(trs, 1);
							}
							else
								retCode = trs.SetStateRelease(m_lPosNumber);
							break;
						case STATE_FORCE_RELEASE:
							retCode = trs.SetStateForcedRelease(m_lPosNumber);
							break;
						case STATE_ON_OFFER:
							if (lFlags & TRS_LOYALTY_CARD)
							{
								_Module.m_server.UpdatePaidTrsQDX(trs, 1);
							}
							else
							{
								if ((_Module.m_server.GetRegion() == REGION_UK) && (!_Module.m_server.IsAllowedMultiPosOwnersOfPumpTrs())) //4.0.23.501 78146
								{
									//pump currently on offer at another pos.
									if (_Module.m_server.m_cPumpArray[p].AnyTrsOnOffer(trs))
										retCode = GPI_ONE_OF_PUMP_TRS_ON_OFFER;
								}
								if (retCode == GPI_OK)
								{
									//in case it's diesel trnsaction check, when it's stat. 
									if ((trs.m_lResExt == RES_EXT_FLEET) && _Module.m_server.m_cServerInfo.GetWaitForPriceChangeAckFlags()) //4.0.10.592
									{
										PAY_AT_PUMP_INFO cPatAtPumpInfo; //4.0.10.592
										CPumpTransact cTmpTrs;
										cTmpTrs = trs;
										long lQDXRetCode = _Module.m_server.IndexOperation(PUMP_OLA_TEMP_FILE, REC_READ, &cTmpTrs, &cPatAtPumpInfo); //4.0.10.49

										if (lQDXRetCode == OK)
										{
											if (cPatAtPumpInfo.CardSaleExtraData3.sWaitForPriceChange == '1') //4.0.10.811
											{
												if (LOG_DETAIL_FLOW_CONTROL)
												{
													CString str;
													str.Format("Pump %02ld trs=%ld can't be offer due to Price change process",
														lPumpNumber,
														trs.m_lNumber);
													_LOGMSG.LogMsg(str);
												}
												retCode = GPI_TRS_NOT_FOR_SALE;
											}
										}
									}

									retCode = trs.SetStateOnOffer(m_lPosNumber, lFlags); //4.0.23.508 - TD 89646
								}
							}
							break;
						case STATE_ACCEPTED:
							retCode = trs.SetStateAccepted(m_lPosNumber);
							break;
						case STATE_PAID:
							retCode = trs.SetStatePaid(m_lPosNumber, lFlags, sData); //4.0.22.501 - TD 78063  //4.0.23.503 

							lPosNumber = POS_NUMBER_PAID_QDX + m_lPosNumber;
							_Module.m_server.InsertToPaidQDX(trs, lPosNumber);//4.0.12.501

																			  //4.0.23.507 90446
							if (!retCode && _Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag() && (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse())) // CR 444830
							{
								_Module.m_server.m_cCommonFunction.DeleteLoyaltyXmslFile(trs.m_lPump, trs.m_lNumber);
							}
							break;

						case STATE_SUPRESS_TIMOUT:
							if (trs.m_lMisc & MISC_UNPAID_TRS_TIMEOUT)
							{
								long lIndex = 0;
								CTime cCurTime = CTime::GetCurrentTime();

								trs.m_lMisc |= MISC_SUPPRESSED_UNPAID_TRS_TIMEOUT;
								_Module.m_server.m_cPumpArray[p].SetTrsByNumber(lTrsNumber, &trs, FALSE);
							}
							break;

						case STATE_RELEASE_CONTROLLER:
						{

							CMD_STRUCT_RELEASE_CONTROLLER_TRS cTRSCmd;
							cTRSCmd.lTrsNumber = lTrsNumber;
							cTRSCmd.lState = STATE_RELEASE_CONTROLLER;


							_Module.m_server.SetCommand(CMD_RELEASE_CONTROLLER_TRS,
								0,
								trs.m_lPump,
								&cTRSCmd,
								sizeof(cTRSCmd));
						}
						break;
						default:
							retCode = GPI_UNKNOWN_TRS_STATE;
							break;
						}
					}
					else if (lState == STATE_RELEASE_CONTROLLER) //4.0.19.506 - TD 47559
					{
						CMD_STRUCT_RELEASE_CONTROLLER_TRS cTRSCmd;
						cTRSCmd.lTrsNumber = lTrsNumber;
						cTRSCmd.lState = STATE_RELEASE_CONTROLLER;


						_Module.m_server.SetCommand(CMD_RELEASE_CONTROLLER_TRS,
							0,
							trs.m_lPump,
							&cTRSCmd,
							sizeof(cTRSCmd));
					}
					else
					{
						//If the cashier attempted to finalize this transaction, but
						//the transaction had already been finalized by this POS, 
						//return OK
						if ((lState == STATE_PAID) && (trs.GetStatus() == PAID) && m_lPosNumber == trs.GetOnOffer())
						{
							if (LOG_DETAIL_FLOW_CONTROL)
							{
								CString str;
								str.Format("Transaction was PAID, do nothing !!!!!! : Trs %d, Status %d, OnOffer %d, link Number %d, Misc %x",
									trs.m_lNumber,
									trs.m_lStatus,
									trs.m_lOnOffer,
									trs.m_lLinkNumber,
									trs.m_lMisc);
								_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
							}
							if (lState == STATE_PAID_LOYLATY)
							{
								CString str;
								long  lRtc = _Module.m_server.DeleteFromPaidTrsQDX(trs);
								if (!(lRtc))
								{
									str.Format("CGPI::SetTrsState__ Delete trs num %ld from paid QDX", trs.m_lNumber);

								}
								else
								{
									str.Format("CGPI::SetTrsState Can't Delete trs num %ld from paid QDX", trs.m_lNumber);
								}
								_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
							}

							retCode = GPI_OK;
						}
						else
						{
							if (lState == STATE_PAID_LOYLATY)
							{
								CString str;
								long  lRtc = _Module.m_server.DeleteFromPaidTrsQDX(trs);
								if (!(lRtc))
								{
									str.Format("CGPI::SetTrsState__ Delete trs num %ld from paid QDX", trs.m_lNumber);

								}
								else
								{
									str.Format("CGPI::SetTrsState Can't Delete trs num %ld from paid QDX", trs.m_lNumber);
								}
								_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
							}
							else
								retCode = GPI_TRS_NOT_FOR_SALE;
						}
					}
				}
			}

			else if (PUMPSRV_IS_SLAVE)
			{
				//If the transaction wasn't found, and PUMPSRV is in SLAVE
				//mode, we may be waiting for the controller to update the
				//transaction for us.  
				//We wait 12 watchdogs (1 minute), before assuming
				//that the transaction doesn't exist.

				if (_Module.m_server.IsControllerUpdateTrs())
					retCode = GPI_TRS_NOT_FOUND;

				else
					retCode = GPI_WAIT_FOR_CONTROLLER_TRS_UPDATE;
			}

			else
				retCode = GPI_TRS_NOT_FOUND;
		}
	}

	//?.??.?.?? - TD 335186
	if ((retCode == GPI_OK) && (m_lPosNumber != TERMINAL_SRV_POS_NUMBER)) //to prevent throwing event to IFSFTerminalSrv if originated from it
		_Module.m_server.m_cGPIEventMgr.AddTrsStateChangedEvent(lPumpNumber, lTrsNumber, lState, lFlags);


	if (retCode)
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(m_lPosNumber, // Src
				"IGPI",
				"SetTrsState",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE,
				lRtc);
	}

	if (pVal != NULL)
		*pVal = retCode;
}




STDMETHODIMP CGPI::AuthorizePump(long lPumpNumber, short nLevel, long lGradePrice, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::AuthorizePump   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		AuthorizePump__(lPumpNumber, nLevel, lGradePrice, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::AuthorizePump"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "AuthorizePump()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::AuthorizePump__(long lPumpNumber, short nLevel, long lGradePrice, long * pVal)
{
	BOOL bAuthAll = FALSE, bFoundPOS = TRUE;
	GPIRetCode retCode = GPI_OK;

	CPosInfo tmpPos;

	short nPrePayWaiting = 0, nFree = 0, nUnpaid = 0;
	if (lPumpNumber > 0)
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsListCount(&nPrePayWaiting, &nFree, &nUnpaid);

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;
	else if (_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
	{
		retCode = GPI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
	}
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
	{
		retCode = GPI_FAILED_IO;
	}
	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);
		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}
	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());
		else
			retCode = GPI_SERVER_NOT_READY;
	}
	else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
	{
		retCode = GPI_SERVER_IN_MIDDLE_LOCK_SHIFT;
	}
	else if ((lPumpNumber > 0) && (nPrePayWaiting + nUnpaid >= (long)_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetMaxTrsNumber()))
	{
		retCode = GPI_PUMP_TRS_LIST_FULL;
	}
	else if (_Module.m_server.m_cServerInfo.GetInStopAllPumps())
	{
		retCode = GPI_PUMP_LOCKED;
	}
	else if (!_Module.m_server.m_cFuelingRestriction.IsApproveAllowed(lPumpNumber, m_lPosNumber))	//4.0.22.501 TD 70956
	{
		retCode = GPI_MAX_CONCURRENT_TRS_HAS_BEEN_REACHED;
	}
	else if (lPumpNumber)
	{
		if (PUMP_NOT_VALID(lPumpNumber))
			retCode = GPI_INVALID_PUMP_NUMBER;

		//If the pump is locked we won't authorize the pump
		else if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetRes() == PUMPSRV) // 4.0.8.503
		{
			retCode = GPI_PUMP_LOCKED;
		}
	}
	else
	{
		bAuthAll = TRUE;
	}

	if (retCode == GPI_OK)
	{
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		long p = lPumpNumber - 1;
		long lLastPump = lPumpNumber;

		if (bAuthAll)
		{
			p = 0L;
			lLastPump = _Module.m_server.GetLastValidPump();
		}

		CString sTmpLogMsg;

		for (; p < lLastPump; p++)
		{
			//Authorizing all pumps did not check that pumps are in view
			_Module.m_server.m_csPosMap.Lock(INFINITE);
			bFoundPOS = _Module.m_server.m_cPosMap.Lookup(m_lPosNumber, tmpPos);
			_Module.m_server.m_csPosMap.Unlock();

			if (bFoundPOS)
			{
				if (!tmpPos.IsPumpInFieldOfView(p + 1))
				{
					//Return GPI_PUMP_OUT_OF_VIEW only if a single
					//pump is being authorized.  Otherwise, just move
					//on.
					if (lPumpNumber)
						retCode = GPI_PUMP_OUT_OF_VIEW;

					continue;
				}
			}

			{
				// 4.0.25.1140 TD 420361 
				long nPosition = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetPosition();
				BOOL bNozzleLocked = _Module.m_server.m_cCommonFunction.IsNozzleLocked(lPumpNumber, nPosition);
				if (TRUE == bNozzleLocked)
				{

					sTmpLogMsg.Format("Could not authorize pump #%02d because the nozzle #%02d is lock.", p + 1, nPosition);
					_Module.m_server.m_cLogger.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, sTmpLogMsg);
					retCode = GPI_PUMP_LOCKED;
					continue;
				}
			}

			if (_Module.m_server.m_cPumpArray[p].PumpStoppedByController() && (!_Module.m_server.m_AllowAuthOnStopByController)) //4.0.20.511  
			{
				sTmpLogMsg.Format("could not authorize pump because it has been stopped by controller");
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, sTmpLogMsg);

				retCode = GPI_PUMP_STOPPED_BY_CONTROLLER;
			}
			else
			{
				CPumpStatus stat = _Module.m_server.m_cPumpArray[p].GetStat();
				// special PrePay Operation
				// the GPI::AuthorizePump command will ditect 
				// that there is wait_auth trs and :
				// 1. change the trs status to wait_progres
				// 2. return pump to ready
				// 3. set trs time to current time.
				// 4, send authorize and limit instead of authorizePump.
				if (((_Module.m_server.AllowAuthOnNonRequest()) && (stat.GetRes() == NULL_RES)) ||
					((stat.GetStatus() == ERROR_TERMINATED) && (PUMPSRV_REPORT_ON_TRS_PAID)) || (stat.GetStatus() == REQUESTING) ||
					((_Module.m_server.m_bAllowAuthOnNonRequestPapResPumpCL) && (stat.GetRes() == PAY_AT_PUMP)) ||	//4.0.21.500 TD 69683
					((_Module.m_server.m_AllowAuthOnStopByController) && (stat.GetStatus() == STOPPED_BY_CONTROLLER)))	//4.0.23.501 TD 78654	
				{
					CPumpTransact trs;	//4.0.22.504 73359
					if (stat.GetRes() == PRE_PAY_AT_POS)
					{
						//CPumpTransact trs;
						if (_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_AUTH, TRS_LINK_PREPAY_NUMBER, &trs))
						{
							if (LOG_BASIC_FLOW_CONTROL)
							{
								CString str;
								str.Format("Operator Authorize Pump - Change to Authorize and Limit (Trs Link number %ld) ", trs.m_lLinkNumber);
								_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
							}

							PRE_PAY_INFO info;
							long lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &trs, &info);
							if (lRtc)
							{
								// ERROR !!!!
								if (LOG_BASIC_FLOW_CONTROL)
								{
									CString str;
									str.Format("Trs %d, TrsLink %d, TrsLinkOperation return %d", trs.m_lNumber, trs.m_nLinkFlags, lRtc);
									_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
								}

								if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
								{
									retCode = GPI_TRS_NOT_FOUND;
									trs.m_lRes = stat.GetRes();
									trs.m_lResExt = stat.GetResExt();
									trs.m_lMode = stat.GetMode();
									trs.UpdateTime();
									trs.m_lStatus = PAID;

									_Module.m_server.CentralUpdateTrs(p + 1, trs.m_lNumber, &trs, FALSE);
									stat.SetStatus(READY);
									_Module.m_server.m_cPumpArray[p].SetStat(stat);

									if (LOG_LOGIC_ERROR)
									{
										CString str;
										str.Format("Throwing Away PrePay TRS !!!!! : Trs %d, TrsLink %d, TrsLinkOperation return %d", trs.m_lNumber, trs.m_nLinkFlags, lRtc);
										_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str, LOG_LEVEL_1);
										_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, "And ... Release Pump ..!!!");
									}

									CMD_STRUCT_RELEASE_PUMP cmdInfo;
									cmdInfo.lRes = PRE_PAY_AT_POS;
									cmdInfo.lResExt = 0;
									_Module.m_server.SetCommand(CMD_RELEASE_PUMP, SERVER_POS_NUMBER, p + 1, &cmdInfo, sizeof(CMD_STRUCT_RELEASE_PUMP));
								}
								else
									retCode = GPI_FAILED_PREPAY_IO;
							}
							else
							{
								if (LOG_BASIC_FLOW_CONTROL)
								{
									CString str;
									str.Format("Authorize Pump (Auto PrePay), switch to WAIT_PROGRESS");
									_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
								}

								stat.SetStatus(READY);
								_Module.m_server.m_cPumpArray[p].SetStat(stat);

								trs.SetStatus(WAIT_PROGRESS);
								trs.UpdateTime();
								// M.L 8/5/98 Lock PumpArray in large scope 
								_Module.m_server.CentralUpdateTrs(p + 1, trs.m_lNumber, &trs, FALSE);

								CMD_STRUCT_AUTHORIZE_LIMIT cmdInfo;
								memset(&cmdInfo, 0, sizeof(cmdInfo));

								cmdInfo.nLevel = info.nLevel;
								cmdInfo.lGradePrice = info.lGradePrice;

								if (LOG_DETAIL_FLOW_CONTROL)
								{
									CString str;
									str.Format("AuthorizeAndLimit ,Trs Sync %ld, Level %d (Level limit %ld) , GradePrice %d, MaxValue %ld, MaxVolume %ld, Flags %x",
										info.lSyncNumber,
										info.nLevel,
										_Module.m_server.GetLimit(info.nLevel, p + 1),
										info.lGradePrice,
										info.lMaxValue,
										info.lMaxVolume,
										info.lFlags);

									_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
								}

								cmdInfo.lMaxVolume = info.lMaxVolume;
								cmdInfo.lSyncNumber = info.lSyncNumber;
								cmdInfo.lFlags = info.lFlags;
								cmdInfo.lMaxValue = info.lMaxValue;

								_Module.m_server.SetCommand(CMD_AUTHORIZE_LIMIT, SERVER_POS_NUMBER, p + 1, &cmdInfo, sizeof(CMD_STRUCT_AUTHORIZE_LIMIT));
							}
						}
					}
					else if ((stat.GetRes() == PAY_AT_KIOSK) && ((_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_AUTH, TRS_LINK_PREPAY_NUMBER, &trs)) || (_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_RES, TRS_LINK_PREPAY_NUMBER, &trs))))
					{//4.0.22.504 73359
						retCode = GPI_PUMP_ALREADY_RESERVED;
						if (LOG_DETAIL_FLOW_CONTROL)
						{
							CString str;
							str.Format("AuthorizePump Pump %02ld , Authorizing PAK is denied since a pre pay is already authorized on the pump.",
								p + 1);

							_LOGMSG.LogMsg(str);
						}
					}
					else // PAK 
					{
						BOOL bNullRes = FALSE;
						if ((stat.GetRes() == NULL_RES) && (_Module.m_server.AllowAuthOnNonRequest()))
						{
							bNullRes = TRUE; // Null reservation allowed.
						}

						if ((bNullRes) ||
							(stat.GetRes() == PAY_AT_KIOSK) ||
							(stat.GetRes() == ATTEND) ||
							(stat.GetRes() == TEST))
						{
							short nPrePayWaiting, nFree, nUnpaid;
							CPumpTransact TmpTrs;
							_Module.m_server.m_cPumpArray[p].GetTrsListCount(&nPrePayWaiting, &nFree, &nUnpaid);

							if ((_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS, TRS_LINK_OLA_NUMBER, &TmpTrs)) ||							//3.2.1.59
								(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_RES, TRS_LINK_PREPAY_NUMBER, &TmpTrs)) ||
								(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_AUTH, TRS_LINK_PREPAY_NUMBER, &TmpTrs)) ||
								(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS, TRS_NO_LINK, &TmpTrs) && (stat.GetStatus() != REQUESTING)))  //4.0.15.504
							{
								retCode = GPI_TRS_ALREADY_IN_PROGRESS;
							}
							else if ((_Module.m_server.m_cPumpArray[p].IsCurrentTrs()) &&
								!((stat.GetStatus() == ERROR_TERMINATED) && (PUMPSRV_REPORT_ON_TRS_PAID)) &&
								!((stat.GetStatus() == STOPPED_BY_CONTROLLER) && _Module.m_server.m_AllowAuthOnStopByController))  //
																																   //4.0.5.34
							{
								retCode = GPI_PUMP_NOT_IN_WORKING_STATE;
							}
							else if (nUnpaid >= MAX_ALLOWED_UNPAID_TRS(p))
							{
								retCode = GPI_PUMP_TRS_LIST_FULL;
							}
							else if (_Module.m_server.PumpNotAllowedMixPAKAndPrePay(p + 1, PRE_PAY_AT_POS)) // 4.0.9.501
							{
								retCode = GPI_PUMP_TRS_LIST_FULL;
								if (LOG_DETAIL_FLOW_CONTROL)
								{
									CString str;
									str.Format("AuthorizeAndLimit ,failed due to waiting PrePay");
									_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
								}
							}
							else
							{
								if (PUMPSRV_IS_MODE_MASTER)
								{
									PumpReservation lPumpRes;
									if (bNullRes)
									{
										lPumpRes = PAY_AT_KIOSK;

										if (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse()) //4.0.19.504 - 47559 // CR 444830
										{
											stat.SetRes(lPumpRes, stat.GetResExt());
											_Module.m_server.m_cPumpArray[p].SetStat(stat);
										}

										if (LOG_BASIC_FLOW_CONTROL)
										{
											CString str;
											str.Format("Pump not Reserved ,Authorize for PAK transaction");
											_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
										}
									}
									else
									{
										lPumpRes = stat.GetRes();
									}
									if (_Module.m_server.m_cModeArray[stat.GetMode() - 1].ResNotValid(lPumpRes))
									{
										retCode = GPI_NOT_VALID_IN_CURRENT_MODE;
									}
								}
							}
						}

						//Allow "PD Error" release in any case: //4.0.24.60
						//====================================
						if (retCode != GPI_OK)
						{
							if ((stat.GetStatus() == STOPPED_BY_CONTROLLER) && (_Module.m_server.m_AllowAuthOnStopByController))
							{
								retCode = GPI_OK;
								if (LOG_BASIC_FLOW_CONTROL)
								{
									_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, "AuthorizePump called - clear PD Error.");
								}
							}
						}

						if (retCode == GPI_OK)
						{
							CMD_STRUCT_AUTHORIZE_LIMIT cmdAuthorize;
							// had here CID_199723 Memset fill value of '0' - added C'tor to the struct instead

							cmdAuthorize.nLevel = nLevel;
							cmdAuthorize.lGradePrice = lGradePrice;
							cmdAuthorize.lFlags = LIMIT_FILL_UP;

							_Module.m_server.SetSiteLimit(p, nLevel, cmdAuthorize); // 4.0.22.850 TD 307758

							cmdAuthorize.lSyncNumber = 0;
							memset(cmdAuthorize.GradeAmountLimit, ' ', sizeof(cmdAuthorize.GradeAmountLimit));//4.0.22.506 67786
							if (!_Module.m_server.CheckForLoyaltyDiscount(p + 1, cmdAuthorize))
							{

							}

							_Module.m_server.SetCommand(CMD_AUTHORIZE_LIMIT, m_lPosNumber, p + 1, &cmdAuthorize, sizeof(CMD_STRUCT_AUTHORIZE_LIMIT));

							if (LOG_BASIC_FLOW_CONTROL)
							{
								CString str;
								str.Format("Pump Authorized by Pos %ld - Fill Up ", m_lPosNumber);
								_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
							}
						}
					}
				}
				else
				{
					retCode = GPI_PUMP_NOT_REQUESTING;
				}

				if (bAuthAll)
					retCode = GPI_OK;
			}
		}
	}

	if (bAuthAll && (retCode != GPI_OK))
	{
		if ((retCode != GPI_MUST_INIT_FIRST) ||
			(retCode != GPI_CONTROLLER_NOT_AVAILABLE) ||
			(retCode != GPI_SERVER_NOT_READY) ||
			(retCode != GPI_FAILED_IO))
		{
			retCode = GPI_OK;
		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"AuthorizePump",
				TRUE, retCode,
				FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::StopPump(long lPumpNumber, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::StopPump   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		StopPump2__(lPumpNumber, 0, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::StopPump"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "StopPump()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::StopPump__(long lPumpNumber, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	CString str;

	str.Format("CGPI::StopPump2__ Insert method ");
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);
		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());
		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (lPumpNumber)
	{
		if (PUMP_NOT_VALID(lPumpNumber))
			retCode = GPI_INVALID_PUMP_NUMBER;
	}


	if (retCode == GPI_OK)
	{
		BOOL			bStopAll = FALSE;
		long			p = 0;

		if (lPumpNumber == 0)
			bStopAll = TRUE;
		else
			p = lPumpNumber - 1;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if (bStopAll)
		{
			for (int p = 0; p < _Module.m_server.GetLastValidPump(); p++)  //4.0.0.79
			{
				if (_Module.m_server.m_cPumpArray[p].CanStopPump())
				{
					_Module.m_server.SetCommand(CMD_STOP_PUMP, m_lPosNumber, p + 1);
				}
			}
		}
		else
		{
			if (_Module.m_server.m_cPumpArray[p].CanStopPump())
				_Module.m_server.SetCommand(CMD_STOP_PUMP, m_lPosNumber, lPumpNumber);
			else if (_Module.m_server.m_cPumpArray[p].GetStat().GetStatus() == STOPED_BY_OPERATOR)
				retCode = GPI_PUMP_ALREADY_SUSPENDED;
			else if (_Module.m_server.m_cPumpArray[p].PumpStoppedByController())
				retCode = GPI_PUMP_STOPPED_BY_CONTROLLER;
		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"StopPump",
				TRUE, retCode,
				FALSE, 0);


	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::RestartPump(long lPumpNumber, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::RestartPump   start");


#ifdef CATCH_GPI
	__try
	{
#endif
		RestartPump__(lPumpNumber, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::RestartPump"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "RestartPump()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::RestartPump__(long lPumpNumber, long *pVal)
{
	GPIRetCode retCode = GPI_OK;
	BOOL bReStartPump = TRUE; // 4.0.5.26

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (lPumpNumber)
	{
		if (PUMP_NOT_VALID(lPumpNumber))
			retCode = GPI_INVALID_PUMP_NUMBER;
		else if (PUMP_STOPPED_BY_PUMPSRV(lPumpNumber))
			retCode = GPI_RESTART_PUMP_NOT_ALLOWED;
	}

	if (retCode == GPI_OK)
	{
		SERVER_INFO sInfo;
		_Module.m_server.m_cServerInfo.GetInfo(sInfo);

		if (sInfo.bRequiredShiftProcess || sInfo.bRequiredPriceChange || sInfo.bRequiredModeProcess)
		{
			retCode = GPI_RESTART_PUMP_NOT_ALLOWED;
			_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
		}

	}
	if (retCode == GPI_OK)
	{
		BOOL			bRestartAll = FALSE;
		long			p = 0;

		if (lPumpNumber == 0)
			bRestartAll = TRUE;
		else
			p = lPumpNumber - 1;



		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if (bRestartAll)
		{
			for (int p = 0; p < _Module.m_server.GetLastValidPump(); p++)  //4.0.0.79
			{
				if (_Module.m_server.m_cPumpArray[p].GetStat().GetStatus() == STOPED_BY_OPERATOR)
				{
					if (!PUMP_STOPPED_BY_PUMPSRV(p + 1))
					{
						CPumpTransact trs;
						if (!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))
						{
							trs.UpdateTime();
							_Module.m_server.CentralUpdateTrs(p + 1, trs.m_lNumber, &trs, FALSE);

						}
						else  // 4.0.5.26
						{
							// in case  PrePay is in wait time out , do not allowed to restart pump. 
							if (_Module.m_server.m_cPumpArray[p].GetStat().GetRes() == PRE_PAY_AT_POS)
							{
								bReStartPump = FALSE;
							}
						}

						if (bReStartPump) // 4.0.5.26
							_Module.m_server.SetCommand(CMD_RESTART_PUMP, m_lPosNumber, p + 1);
					}
				}
			}

			//Now that we signaled all stopped pumps to restart, we unlock
			//the forecourt
			if (_Module.m_server.GetLockOnStopAll())
			{
				_Module.m_server.m_cServerInfo.SetRequiredMaintenanceProcessFlags(MP_FORCED_UNLOCK);
				_Module.m_server.m_cServerInfo.SetInStopAllPumps(FALSE); //4.0.13.80

				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "RestartPump");
			}
		}
		else
		{
			if (_Module.m_server.m_cPumpArray[p].GetStat().GetStatus() == STOPED_BY_OPERATOR)
			{
				CPumpTransact trs;

				if (!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))
				{
					if (trs.GetStatus() != IN_PROGRESS)
					{
						trs.UpdateTime();
						_Module.m_server.CentralUpdateTrs(lPumpNumber, trs.m_lNumber, &trs, FALSE);
					}
				}
				else  // 4.0.5.26
				{
					// in case  PrePay is in wait time out , do not allowed to restart pump. 
					if (_Module.m_server.m_cPumpArray[p].GetStat().GetRes() == PRE_PAY_AT_POS)
					{
						bReStartPump = FALSE;
					}
				}

				if (bReStartPump) // 4.0.5.26				
					_Module.m_server.SetCommand(CMD_RESTART_PUMP, m_lPosNumber, lPumpNumber);
				else
					retCode = GPI_RESTART_PUMP_NOT_ALLOWED;  //4.0.5.26
			}
			else
				retCode = GPI_PUMP_NOT_SUSPENDED;
		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"RestartPump",
				TRUE, retCode,
				FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}


// M.L 13.7.98 - Change lSyncNumber to be in/out
STDMETHODIMP CGPI::AuthorizeAndLimit(long lPumpNumber, short nLevel, long lGradePrice, long lMaxValue, long lMaxVolume, short nFlags, long *lSyncNumber, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::AuthorizeAndLimit   start");

#ifdef CATCH_GPI
	__try
	{
#endif

		AuthorizeAndLimit__(lPumpNumber, nLevel, lGradePrice, lMaxValue, lMaxVolume, nFlags, lSyncNumber, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::AuthorizeAndLimit"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "AuthorizeAndLimit()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}



void CGPI::AuthorizeAndLimit__(long lPumpNumber, short nLevel, long lGradePrice, long lMaxValue, long lMaxVolume, long lFlags, long *lSyncNumber, long *pVal, BSTR bstrPrePay /* = 0*/, __int64 lPurchaseID /* = 0 */)
{
	GPIRetCode retCode = GPI_OK;
	long lRtc = OK;

	//4.0.1.36 moved all log messages to this CString,
//instead of declaring new ones all the time.
	CString sLogMsg;

	sLogMsg.Format("lPumpNumber %d, nLevel %d, lGradePrice %d, lMaxValue %d, lMaxVolume %d, lFlags %d, *lSyncNumber %d, *pVal %d, lPurchaseID %d ",
					lPumpNumber	, nLevel, lGradePrice, lMaxValue, lMaxVolume, lFlags, *lSyncNumber, *pVal, lPurchaseID);
	_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);

	short nPrePayWaiting = 0, nFree = 0, nUnpaid = 0;
	if (lPumpNumber > 0)
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsListCount(&nPrePayWaiting, &nFree, &nUnpaid);

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
		retCode = GPI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	else if (PUMP_NOT_IN_WORKING_STATE(lPumpNumber))
		retCode = GPI_PUMP_NOT_IN_WORKING_STATE;

	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GPI_FAILED_PREPAY_IO;
	else if ((IS_SERVER_IN_MIDDLE_LOCK_SHIFT) && (!(lFlags & PREPAY_CANCEL))) //4.0.22.630 157555 Allow canceling the prepay in the middle of EOS //4.0.26.507
	{
		//if the flag is on than we will continue the regular flow
		//it's means that we change shift with lock pumps even they not idle
		if (_Module.m_server.GetShiftRecData().bLockShiftDuringProcess == 0)//4.0.11.506
			retCode = GPI_SERVER_IN_MIDDLE_LOCK_SHIFT;   //4.0.1.40
	}

	else if ((lPumpNumber > 0) && (nPrePayWaiting + nUnpaid >= (long)_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetMaxTrsNumber()) && !(lFlags & PREPAY_CANCEL || lFlags & RE_AUTHORIZE_TIMEOUT)) //4.0.17.50 - TD 46036, 4.0.17.240 - TD 51966
	{
		retCode = GPI_PUMP_TRS_LIST_FULL;
	}
	else if (lPumpNumber) //4.0.8.503
	{
		//If the pump is locked we won't authorize the pump
		if ((_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetRes() == PUMPSRV) && (!(lFlags & PREPAY_CANCEL)))	//4.0.20.40 59680	Allow pre pay trs canceling while the pump is locked. Merge from 1017.
		{
			retCode = GPI_PUMP_LOCKED;
		}
		else if (_Module.m_server.PumpNotAllowedMixPAKAndPrePay(lPumpNumber, PAY_AT_KIOSK)) // 4.0.9.505
		{
			retCode = GPI_PUMP_TRS_LIST_FULL;
		}
	}


	if (LOG_BASIC_FLOW_CONTROL)
	{
		if (lFlags & PREPAY_CANCEL)
			_LOGMSG.LogReq(m_lPosNumber, "GPI", "AuthorizeAndLimit", "Cancel Operation");
		else if (lFlags & RE_AUTHORIZE_TIMEOUT)
			_LOGMSG.LogReq(m_lPosNumber, "GPI", "AuthorizeAndLimit", "Re Authorize timed out");
	}


	if (retCode == GPI_OK)
	{
		long  p = lPumpNumber - 1;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		CPumpTransact	trs;
		PRE_PAY_INFO	info;
		BOOL			bFoundLocal = FALSE, bFoundInFile = FALSE;
		short			nLinkFlags = 0;

		memset(&info, 0, sizeof(PRE_PAY_INFO)); //4.0.19.504 TD 60613

		if (lFlags & PREPAY_WITH_TENDER_INFO)
			SaveTenderType(trs, bstrPrePay); // 4.0.25.600   CR 296433

		if (lFlags & PREPAY_INCLUDES_GRADES_RESTRICTIONS)  //4.0.27.20 - TD 219740
		{
			if (!(lFlags & LOYALTY_DISCOUNT_DATA))
				lFlags |= LOYALTY_DISCOUNT_DATA;
		}

		_Module.m_server.m_cCommonFunction.GeneratePrePayTrsLinkFlags(lFlags, &nLinkFlags);			//4.0.24.73 No TD	
		trs.SetLink(nLinkFlags, *lSyncNumber);
		trs.m_lPump = lPumpNumber;

		if (*lSyncNumber)
		{
			lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &trs, &info);

			if (lRtc == OK)// found in prepay file 
			{
				bFoundInFile = TRUE;

				if (lFlags & PREPAY_MOVE)
				{
					retCode = ValidateMovePrePay(lPumpNumber, lFlags, retCode, trs, info, nUnpaid);
				}
			}


			if ((lRtc == OK) || (lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			{
				lRtc = 0;
			}
			else
			{
				retCode = GPI_FAILED_PREPAY_IO;
			}

			if (!_Module.m_server.FindTrsByLinkNumber(&trs))
				bFoundLocal = TRUE;
		}

		if (LOG_BASIC_FLOW_CONTROL)
		{
			sLogMsg.Format("Flags %d :FoundLocal %d, FoundInFile %d, Sync Number %d , lGradePrice=%ld, retCode %d", lFlags, bFoundLocal, bFoundInFile, *lSyncNumber, lGradePrice, retCode);
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
		}

		if (retCode == GPI_OK)
		{
			if (lFlags & PREPAY_CANCEL)  // cancel pre pay
			{
				lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &trs, &info);

				//Checking if the POS has requested to offer the transaction
				if (info.nOwningPos != m_lPosNumber) //Transaction is owned by another POS or not owned at all
				{
					if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
					{
						retCode = GPI_TRS_LOCKED;
						lRtc = ERR_EXISTS;
					}
					else //Transaction isn't owned by anyone, We'll acquire ownership
					{
						info.nOwningPos = (short)m_lPosNumber;
						lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &trs, &info);
					}
				}
			}
		}


		if (retCode == GPI_OK)
		{
			if (lFlags & PREPAY_CANCEL)  // cancel pre pay
										 //====================================================
			{
				if ((trs.GetStatus() != WAIT_TIME_OUT) && !(lFlags & CHECK_AND_RESERVE_FLAG)) //4.0.17.20 - TD 26384
				{
					retCode = GPI_TRS_NOT_TIMEOUT;
					if (LOG_BASIC_FLOW_CONTROL)
					{
						sLogMsg.Format("Can't cancel PrePay trs, Trs # %d, Status %d, LinkFlag %d, Link # %d", trs.m_lNumber, trs.GetStatus(), trs.GetLinkFlags(), trs.GetLinkNumber());
						_LOGMSG.LogMsg(sLogMsg);
					}
				}
				else if ((!bFoundLocal) && (!bFoundInFile))
				{
					retCode = GPI_TRS_NOT_FOUND;
				}
				else if (bFoundLocal)
				{
					// In case of CheckAndReserve (End) totally delete the transaction and release the pump
					if ((lFlags & CHECK_AND_RESERVE_FLAG) && (trs.m_lMisc & MISC_PUMP_CHECK_AND_RESERVE))
					{
						CPumpStatus stat = _Module.m_server.m_cPumpArray[p].GetStat();
						long lMisc = stat.GetMisc();

						if (_Module.m_server.WideRemoveTrs(&trs, &lRtc))
						{
							if (LOG_BASIC_FLOW_CONTROL)
							{
								sLogMsg.Format("Failed to execute WideRemoveTrs: retCode=%ld, trs number=%ld", lRtc, trs.m_lNumber);
								_LOGMSG.LogMsg(p + 1, LOG_PUMP, sLogMsg, LOG_LEVEL_1);
							}
						}

						trs.m_lMisc &= MISC_NOT_PUMP_CHECK_AND_RESERVE;
						lMisc &= MISC_NOT_PUMP_CHECK_AND_RESERVE;

						if (stat.GetRes() == PRE_PAY_AT_POS) //4.0.24.501 - SR 118680
						{
							CMD_STRUCT_RELEASE_PUMP cmdInfo;
							cmdInfo.lRes = stat.GetRes();
							cmdInfo.lResExt = 0;
							_Module.m_server.SetCommand(CMD_RELEASE_PUMP, SERVER_POS_NUMBER, p + 1, &cmdInfo, sizeof(CMD_STRUCT_RELEASE_PUMP));

							stat.SetStatus(READY);
							stat.SetRes(NULL_RES, NULL_RES_EXT); //4.0.17.500							
						}

						stat.SetMisc(lMisc);
						_Module.m_server.m_cPumpArray[p].SetStat(stat);
					}
					else
					{
						int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_GPI, trs.m_lPump, trs.m_lNumber, &lRtc);
						switch (iRetNum)
						{
						case TRS_CANCEL: retCode = GPI_OK; break;
						case TRS_NOT_FOUND: retCode = GPI_TRS_NOT_FOUND; break;
						case TRS_CANT_CANCEL: // too late 							
						{
							if (nUnpaid >= MAX_ALLOWED_UNPAID_TRS(p))
							{
								retCode = GPI_PUMP_TRS_LIST_FULL;
							}
							else if (lFlags & PREPAY_CHANGE_TO_PAY_AT_KIOSK) //4.0.9.507
							{
								CPumpTransact CanceledTrs;
								CanceledTrs = trs;

								// Creating a new PAK transaction and cancel the Pre Pay transaction
								// -----------------------------------------------------------------
								trs.m_lNumber = _Module.m_server.NewTransactNumber();
								trs.m_lOnOffer = 0;
								trs.m_lRes = PAY_AT_KIOSK;
								trs.m_lResExt = NULL_RES_EXT;
								trs.m_lLinkNumber = 0;
								trs.m_nLinkFlags = TRS_NO_LINK;

								long p = info.lPumpNumber - 1;

								CPumpStatus		pumpStat = _Module.m_server.m_cPumpArray[p].GetStat();
								pumpStat.SetRes(PAY_AT_KIOSK, NULL_RES_EXT);
								_Module.m_server.m_cPumpArray[p].SetStat(pumpStat);
								_Module.m_server.m_cPumpArray[p].m_lChangedTrsNumber = CanceledTrs.m_lNumber;

								iRetNum = _Module.m_server.SpecialUpdateTrs(
									info.lPumpNumber,
									CanceledTrs.m_lNumber,
									&trs,
									TRS_FIELD_NUMBER |
									TRS_FIELD_ON_OFFER |
									TRS_FIELD_RES |
									TRS_FIELD_RESEXT |
									TRS_FIELD_LINK_NUMBER |
									TRS_FIELD_LINK_FLAGS,
									IN_PROGRESS);
								if (iRetNum == 0)
								{
									_Module.m_server.TrsLinkOperation(REC_DELETE, &CanceledTrs, NULL);
									_Module.m_server.m_cShiftLog.LogTrs(&CanceledTrs, TRUE, FALSE);
									retCode = GPI_OK;
								}
								else
								{
									retCode = GPI_TRS_ALREADY_IN_PROGRESS;
									_Module.m_server.m_cPumpArray[p].m_lChangedTrsNumber = 0;
								}
							}
							else
								retCode = GPI_TRS_ALREADY_IN_PROGRESS;
						}
						break;

						case TRS_NOT_PREPAY: retCode = GPI_TRS_NOT_PREPAY; break;
						case TRS_FAILED_IO: retCode = GPI_FAILED_PREPAY_IO; break;
						}
						if (iRetNum == 0)
						{
							if (LOG_BASIC_FLOW_CONTROL)
							{
								sLogMsg.Format("AuthorizeAndLimit: Trs %02ld Canceled", trs.m_lNumber);
								_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, trs.m_lPump, LOG_PUMP, sLogMsg);
							}
						}
					}
				}
				else if ((!bFoundLocal) && (bFoundInFile))
				{
					if (trs.GetStatus() == PAID)
					{
						retCode = GPI_CANT_CANCEL_TRS;
						if (LOG_BASIC_FLOW_CONTROL)
						{
							_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, trs.m_lPump, LOG_PUMP, "Cant Cancel TRS ");
						}
					}
					else
					{
						lRtc = _Module.m_server.TrsLinkOperation(REC_DELETE, &trs, NULL);
						if (lRtc)
							retCode = GPI_FAILED_PREPAY_IO;

						else if (LOG_BASIC_FLOW_CONTROL)
						{
							sLogMsg.Format("PrePay Trs %d, deleted due to cancel operation", trs.m_lNumber);
							_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, trs.m_lPump, LOG_PUMP, sLogMsg);
						}

						// Remove also from PumpStat qdx
						_Module.m_server.m_cPumpArray[p].ClearTrs(trs.m_lNumber);
						_Module.m_server.SavePumpInfo(lPumpNumber);
						_Module.m_server.SetPrePayReTryReservation(p, TIME_BETWEEN_RETRYING_RESERVED_PRE_PAID_PUMP); //4.0.24.351 //4.0.124.355 //4.0.27.60 231255
					}
				}
			}
			else if (lFlags & RE_AUTHORIZE_TIMEOUT)  // re-authorize pre pay //4.0.9.507
													 //====================================================
			{
				if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
				{
					retCode = GPI_TRS_LOCKED;
				}
				else if (trs.GetStatus() != WAIT_TIME_OUT)
				{
					retCode = GPI_TRS_NOT_TIMEOUT;
					if (LOG_BASIC_FLOW_CONTROL)
					{
						sLogMsg.Format("Trs # %d, Status %d, LinkFlag %d, Link # %d", trs.m_lNumber, trs.GetStatus(), trs.GetLinkFlags(), trs.GetLinkNumber());
						_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, trs.m_lPump, LOG_PUMP, sLogMsg);
					}
				}

				//4.0.1.36 Eyal Do not allow reauthorization of timed out prepay
				//if max number of unpaid PAKs has been reached 
				//and AVI conflict is configured, because if an AVI conflict
				//occurs, an unpaid P@K trs is created.
				else if (nUnpaid > MAX_ALLOWED_UNPAID_TRS(p) &&  /* 4.0.23.509 - > instead of >= */
					_Module.m_server.CheckVITConflict())
				{
					retCode = GPI_PUMP_TRS_LIST_FULL;

					sLogMsg.Format("attempted to reauthorize prepay when trs list is full (number of Unpaid = %d)", nUnpaid); //4.0.23.509 -log
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, sLogMsg);
				}

				//4.0.2.23 do not allow reauthorization of a timed out prepay
				//if pump has been stopped by controller
				else if (_Module.m_server.m_cPumpArray[p].PumpStoppedByController())
				{
					sLogMsg.Format("attempted to reauthorize prepay on pump, but pump has been stopped by controller");
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, sLogMsg);

					retCode = GPI_PUMP_STOPPED_BY_CONTROLLER;
				}

				else
				{
					if (!bFoundInFile)
					{
						retCode = GPI_TRS_NOT_FOUND;
					}
					else if (!bFoundLocal)
					{
						retCode = GPI_TRS_NOT_FOUND;
					}
					else
					{
						BOOL bUpdate = FALSE;

						//4.0.1.0
						if (_Module.m_server.IsNotAllowedWaitingPrePay())
						{
							if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].IsPumpIdle())
							{
								bUpdate = TRUE;
							}
							else
								retCode = GPI_PUMP_ALREADY_RESERVED;

						}
						else if ((_Module.m_server.m_cPumpArray[lPumpNumber - 1].IsAllowPrePayOnRequestingPump()) ||
							(!_Module.m_server.m_cPumpArray[lPumpNumber - 1].IsAllowPrePayOnRequestingPump()) &&
							(_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetStatus() == READY) &&
							_Module.m_server.m_cPumpArray[lPumpNumber - 1].IsPumpIdle())//4.0.11.504
						{
							bUpdate = TRUE;
						}
						else
						{
							retCode = GPI_PUMP_ALREADY_RESERVED;
						}


						if (retCode == GPI_OK)
						{
							trs.SetStatus(WAIT_RES);
							lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &trs, &info);
							if (lRtc)
							{
								int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_GPI, lPumpNumber, trs.m_lNumber, &lRtc);
								switch (iRetNum)
								{
								case TRS_CANCEL: retCode = GPI_OK; break;
								case TRS_NOT_FOUND: retCode = GPI_TRS_NOT_FOUND; break;
								case TRS_CANT_CANCEL: retCode = GPI_TRS_ALREADY_IN_PROGRESS; break;
								case TRS_NOT_PREPAY: retCode = GPI_TRS_NOT_PREPAY; break;
								case TRS_FAILED_IO: retCode = GPI_FAILED_PREPAY_IO; break;
								}
							}
							else
							{
								_Module.m_server.CentralUpdateTrs(info.lPumpNumber, trs.m_lNumber, &trs, FALSE);
								retCode = GPI_OK;
							}
						}
					}
				}
			}
			else // update or new one.
				 //====================================================
			{
				if (LOG_BASIC_FLOW_CONTROL)
				{
					sLogMsg.Format("Update/New AuthorizeAndLimit - FoundLocal %d, FoundInFile %d", bFoundLocal, bFoundInFile);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
				}

				if (!_Module.m_server.IsPrePayEnabled())
				{
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "PrePay not enabled");
					retCode = GPI_NOT_VALID_IN_CURRENT_MODE;
				}
				else if ((_Module.m_server.GetPrePayLimit()) && (_Module.m_server.GetPrePayLimit() < lMaxValue))    //4.0.3.32
				{
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "PrePay Request passed the PrePay forecourt limit");
					retCode = GPI_PREPAY_REQUEST_PASSED_FORECOURT_LIMIT;

				}
				else
				{
					if (lFlags & PREPAY_MOVE) //2.9.2.1
					{
						lRtc = MovePrePay(lPumpNumber, trs, info, lSyncNumber, retCode);

					}
					else
					{
						// M.L 13.7.98 - Change lSyncNumber to be in/out
						info.lSyncNumber = *lSyncNumber;
						info.lPumpNumber = lPumpNumber;
						info.nLevel = nLevel;
						info.lGradePrice = lGradePrice;
						info.lMaxValue = lMaxValue;
						info.lMaxVolume = lMaxVolume;
						info.lFlags = lFlags;
						info.lPurchaseID = lPurchaseID; //4.0.20.54

						trs.m_lGradePrice = lGradePrice;
						if (info.lFlags & GRADE_PRICE_IS_POSITION_NUM)
						{
							trs.m_nNozzle = (short)lGradePrice;
							trs.m_nGrade = _Module.m_server.m_cPumpArray[info.lPumpNumber - 1].NozzleToGrade((short)info.lGradePrice);
						}

						// Update PreSet values
						if (lMaxValue > 0)
						{
							info.sPreSetLimitType = LIMIT_BY_VALUE;
							info.lPreSetLimit = lMaxValue;
						}
						else if (lMaxVolume > 0)
						{
							info.sPreSetLimitType = LIMIT_BY_VOLUME;
							info.lPreSetLimit = lMaxVolume;
						}
						else
							info.lPreSetLimit = 0;
					}


					if ((!bFoundLocal) && (!bFoundInFile) && !(lFlags & PREPAY_MOVE)) // New Pre Pay TRS
																					  //------------------------------------------------------
					{
						AuthorizeAndLimitHandlePrePay(p, lFlags, retCode, sLogMsg, lPumpNumber, nUnpaid, trs, info, lSyncNumber, bstrPrePay, lRtc);

					}
					else if (bFoundInFile)  // Update Pre Pay TRS	or Duplicate				     
											//-------------------------------------------------------------------
					{
						//4.0.23.502 - TD 75947
						if (!(lFlags & PREPAY_MOVE))
						{
							if ((lFlags & PREPAY_WITH_XML_BUFF))  //4.0.23.504 - TD 73059	//4.0.24.71 No TD
							{
								long lRetCode = _Module.m_server.InsertXMLReceiptIntoReceiptQDX(lFlags, bstrPrePay, trs, lPumpNumber);

								if (!lRetCode)
									retCode = GPI_TRS_PARAM_NOT_VALID;
							}
							else if ((lFlags & PREPAY_WITH_XML_BUFF_WITH_LOYALTY_DISCOUNT_DATA))					//4.0.24.90 TD 121218
							{
								char	szXmlBuff[MAX_XML_BUF_LEN];
								memset(&szXmlBuff, 0, sizeof(szXmlBuff));

								int lBuffSize = Bstr2Buffer(bstrPrePay, szXmlBuff, sizeof(szXmlBuff));

								if (lBuffSize > 0)
								{
									_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "CGPI::AuthoirzeAndLimit__ PREPAY_WITH_XML_BUFF_ and _LOYALTY_DISCOUNT_DATA, bFoundInFile=TRUE");
#ifdef DEBUG
									CString str;
									str.Format("CGPI::AuthoirzeAndLimit__ Recieved  xml is: %s", szXmlBuff);
									_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);

#endif

									_Module.m_server.UpdatePrePayWithXmlReceiptAndDiscountData(lPumpNumber, szXmlBuff, &trs, &info);
								}
							}
						}

						if (!_Module.m_server.m_cPumpArray[p].IsUpdatePrepayAllow() && !(trs.m_lMisc & MISC_PUMP_CHECK_AND_RESERVE)) //4.0.17.20 - TD 26384
							retCode = GPI_PUMP_TRS_LIST_FULL;

						if (trs.GetStatus() == PAID)
							retCode = GPI_DUPLICATE_SYNC_NUMBER;
						else
						{
							// in error do not continue process the prepay
							if (retCode == GPI_OK)
							{
								short nLinkToExtraInfo = 0;

								if ((lFlags & PREPAY_MOVE) &&     //4.0.9.507
									((trs.GetLinkFlags() & TRS_LINK_PREPAY_NUMBER_AND_LOYALTY) || (trs.GetLinkFlags() & TRS_LINK_EXTRA_INFO)))
								{
									nLinkToExtraInfo = trs.GetLinkFlags();
									trs.SetLink(TRS_LINK_PREPAY_NUMBER, trs.m_lNumber);
									_Module.m_server.CentralUpdateTrs(trs.m_lPump, trs.m_lNumber, &trs, FALSE);//4.0.3.43
								}

								int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_GPI, trs.m_lPump, trs.m_lNumber, &lRtc, TRUE);
								switch (iRetNum)
								{
								case 0: retCode = GPI_OK; break;
								case 1: retCode = GPI_TRS_NOT_FOUND; break;
								case 2: retCode = GPI_TRS_ALREADY_IN_PROGRESS; break;
								case 3: retCode = GPI_TRS_NOT_PREPAY; break;
								case 5: retCode = GPI_FAILED_PREPAY_IO; break;
								}

								if (iRetNum == GPI_OK)
								{
									CPumpStatus	stat = _Module.m_server.m_cPumpArray[p].GetStat();
									if (!_Module.m_server.m_cModeArray[stat.GetMode() - 1].AutoAuthorize(PRE_PAY_AT_POS, NULL_RES_EXT))
									{
										trs.SetStatus(WAIT_TIME_OUT);

										//Store time out in trs history
										trs.m_lMisc |= MISC_PREPAY_TIMED_OUT;

										// In case we came from checkAndReserve release the pump
										if (trs.m_lMisc & MISC_PUMP_CHECK_AND_RESERVE)
										{
											CMD_STRUCT_RELEASE_PUMP cmdInfo;
											cmdInfo.lRes = stat.GetRes();
											cmdInfo.lResExt = 0;
											_Module.m_server.SetCommand(CMD_RELEASE_PUMP, SERVER_POS_NUMBER, p + 1, &cmdInfo, sizeof(CMD_STRUCT_RELEASE_PUMP));

											stat.SetStatus(READY);
											_Module.m_server.m_cPumpArray[p].SetStat(stat);
										}
									}
									else
									{
										if (_Module.m_server.IsNotAllowedWaitingPrePay())
										{
											if (!_Module.m_server.m_cPumpArray[p].IsPumpIdle())
											{
												if (!(trs.m_lMisc & MISC_PUMP_CHECK_AND_RESERVE)) //4.0.17.20 - TD 26384
													trs.SetStatus(WAIT_RES);
											}
											else
											{
												trs.SetStatus(WAIT_TIME_OUT);
												trs.m_lMisc |= MISC_PREPAY_TIMED_OUT;
											}
										}
										else
											if (!(trs.m_lMisc & MISC_PUMP_CHECK_AND_RESERVE)) //4.0.17.20 - TD 26384
												trs.SetStatus(WAIT_RES);
									}

									trs.m_lPump = lPumpNumber; // in case of pump change

									if (!(lFlags & CHECK_AND_RESERVE_FLAG) && (trs.m_lMisc & MISC_PUMP_CHECK_AND_RESERVE))
									{
										long lMisc = stat.GetMisc();
										trs.m_lMisc &= MISC_NOT_PUMP_CHECK_AND_RESERVE;

										lMisc &= MISC_NOT_PUMP_CHECK_AND_RESERVE;
										stat.SetMisc(lMisc);
										_Module.m_server.m_cPumpArray[p].SetStat(stat);
									}

									if (nLinkToExtraInfo)
									{
										trs.SetLink(nLinkToExtraInfo, trs.m_lNumber);
										_Module.m_server.CentralUpdateTrs(p + 1, trs.m_lNumber, &trs, FALSE);
									}

									//4.0.26.0 - TD 69677
									if (lFlags & PREPAY_MOVE) // if move prepay
									{
										if (info.lFlags & PREPAY_WITH_XML_BUFF)
										{
											// NO TD - Update pump number in the receipt table
											_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GPI::AuthorizeAndLimit, Move PrePay receipt!");

											CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
											bool bRetCode = PumpSrvDatabase.UpdateReceiptPumpNumber(trs.m_lNumber, trs.m_lPump);

											sLogMsg.Format("Move PrePay receipt, Update Receipt (lRtc %d)", bRetCode);
											_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
										}
									}

									//4.0.24.60 TD 112315 - In case there is PREPAY_WITH_XML_BUFF flag - we might have inserted Extra Info data
									//So do not delete it in CentralInsertTrs (in CheckVlidityTransactionNumber)
									BOOL bDoNotValidateExtraInfo = ((lFlags & PREPAY_MOVE) || (lFlags & PREPAY_WITH_XML_BUFF) || (lFlags & PREPAY_WITH_XML_BUFF_WITH_LOYALTY_DISCOUNT_DATA) || lFlags & MULTIPLE_VOLUME_AND_AMOUNT_LIMIT);

									int iRetNum = _Module.m_server.CentralInsertTrs(lPumpNumber, &trs, FALSE, bDoNotValidateExtraInfo);	//4.0.11.451					 
									switch (iRetNum)
									{
									case 0: retCode = GPI_TRS_WAIT_IN_LIST; break;
									case 1: retCode = GPI_PUMP_TRS_LIST_FULL; break;
									case 3: retCode = GPI_TRS_FIRST_IN_LIST; break;
									case 4: retCode = GPI_TRS_NOT_FOUND; break;
									}
									if (retCode != GPI_PUMP_TRS_LIST_FULL)
									{
										if (lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &trs, &info, NULL, TRUE))  //4.0.3.44 //4.0.3.49
										{
											int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_GPI, lPumpNumber, trs.m_lNumber, &lRtc);
											switch (iRetNum)
											{
											case 0: retCode = GPI_OK; break;
											case 1: retCode = GPI_TRS_NOT_FOUND; break;
											case 2: retCode = GPI_TRS_ALREADY_IN_PROGRESS; break;
											case 3: retCode = GPI_TRS_NOT_PREPAY; break;
											case 5: retCode = GPI_FAILED_PREPAY_IO; break;
											}
										}
									}
								}
							}
						}
					}
					else if ((bFoundLocal) && (!bFoundInFile))
						//--------------------------------------------
					{
						if (LOG_LOGIC_ERROR)
						{
							_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "PrePay not found in file - create an empty record", LOG_LEVEL_1);
						}

						if (lRtc = _Module.m_server.TrsLinkOperation(REC_INSERT, &trs, &info))
						{
							int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_GPI, trs.m_lPump, trs.m_lNumber, &lRtc);
							switch (iRetNum)
							{
							case 0: retCode = GPI_OK; break;
							case 1: retCode = GPI_TRS_NOT_FOUND; break;
							case 2: retCode = GPI_TRS_ALREADY_IN_PROGRESS; break;
							case 3: retCode = GPI_TRS_NOT_PREPAY; break;
							case 5: retCode = GPI_FAILED_PREPAY_IO; break;
							}
						}
					}
				}
			}
		}
	}


	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"AuthorizeAndLimit",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE, lRtc);


	if (pVal != NULL)
		*pVal = retCode;
}
// Reserve Pump For PrePay operstion.
// will be done automaticly by PumpSrv when a pre pay is stacked 
// and the pump condition is ready.
// POS dont have to invoke this method.

STDMETHODIMP CGPI::ReservePump(long lPumpNumber, long lSyncNumber, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::ReservePump   start");


#ifdef CATCH_GPI
	__try
	{
#endif
		ReservePump__(lPumpNumber, lSyncNumber, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::ReservePump"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("GPI", "ReservePump()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::ReservePump__(long lPumpNumber, long lSyncNumber, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	else if (PUMP_NOT_IN_WORKING_STATE(lPumpNumber))
		retCode = GPI_PUMP_NOT_IN_WORKING_STATE;

	else if (retCode == GPI_OK)
	{

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		// M.L 12.7.98 Reserve pump for pre pay operation
		//-----------------------------------------------		
		if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetLockStatus() == UNLOCK)
		{
			if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].CanReserve())
			{
				CMD_STRUCT_RESERVE_PUMP cmdReservePump;
				cmdReservePump.lRes = PRE_PAY_AT_POS;
				cmdReservePump.lResExt = m_lPosNumber;
				_Module.m_server.SetCommand(CMD_RESERVE_PUMP, m_lPosNumber, lPumpNumber, &cmdReservePump, sizeof(CMD_STRUCT_RESERVE_PUMP));
			}
			else
			{
				retCode = GPI_PUMP_ALREADY_RESERVED;
			}
		}
		else
		{
			retCode = GPI_PUMP_LOCKED;
		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"ReservePump",
				TRUE, retCode,
				FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::UnReservePump(long lPumpNumber, long lSyncNumber, BOOL bForceRelease, long lByDevice, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::UnReservePump   start");


#ifdef CATCH_GPI
	__try
	{
#endif
		UnReservePump__(lPumpNumber, lSyncNumber, bForceRelease, lByDevice, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::UnReservePump"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("GPI", "UnReservePump()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::UnReservePump__(long lPumpNumber, long lSyncNumber, BOOL bForceRelease, long lByDevice, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	else if (PUMP_NOT_IN_WORKING_STATE(lPumpNumber))
		retCode = GPI_PUMP_NOT_IN_WORKING_STATE;

	else if (retCode == GPI_OK)
	{

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		// M.L 12.7.98 Reserve pump for pre pay operation
		//-----------------------------------------------		
		if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetLockStatus() == UNLOCK)
		{
			if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].CanReserve())
			{
				retCode = GPI_OK;
			}
			else
			{
				CMD_STRUCT_RELEASE_PUMP cmdInfo;
				cmdInfo.lRes = PRE_PAY_AT_POS;
				cmdInfo.lResExt = m_lPosNumber;

				if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetResExt() == RES_EXT_MOBILE)
				{
					cmdInfo.lRes = PAY_AT_PUMP;
					cmdInfo.lResExt = RES_EXT_MOBILE;
				}

				_Module.m_server.SetCommand(CMD_RELEASE_PUMP, SERVER_POS_NUMBER, lPumpNumber, &cmdInfo, sizeof(CMD_STRUCT_RELEASE_PUMP));
			}
		}
		else
		{
			retCode = GPI_PUMP_LOCKED;
		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"UnReservePump",
				TRUE, retCode,
				FALSE, 0);


	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetPAPTrs(long lPumpNumber, BSTR * pRec, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPAPTrs   start");
	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPAPTrs, Throw COM Exception : E_POINTER", LOG_LEVEL_0);

		return E_POINTER;
	}


#ifdef CATCH_GPI
	__try
	{
#endif
		GetPAPTrs__(lPumpNumber, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetPAPTrs"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPAPTrs()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GetPAPTrs__(long lPumpNumber, BSTR *pRec, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	long		lRtc = 0;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	TAG_OLA_REC tagOlaRec;
	memset(&tagOlaRec, ' ', sizeof(TAG_OLA_REC));


	if (retCode == GPI_OK)
	{
		if (lPumpNumber) // search by pump 
		{
			if (PUMP_NOT_VALID(lPumpNumber))
				retCode = GPI_INVALID_PUMP_NUMBER;
			else
			{
				retCode = GPI_NOT_SUPPORTED;
			}
		}
		else // search for any PAP concluded trs
		{

			CPumpTransact		trs;
			PAY_AT_PUMP_INFO	info;

			trs.SetLink(TRS_LINK_OLA_NUMBER, 0);
			memset(&info, 0, sizeof(PAY_AT_PUMP_INFO));

			lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST, &trs, &info);
			if (lRtc == OK)
			{
				if (LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("GetPAPTrs__ - First Record number %d, trs %d, Shift %d", info.CardSaleInfo.lIndexNumber, trs.m_lNumber, trs.m_lActiveShiftNumber);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
				}

				l2a(info.CardSaleInfo.lIndexNumber, tagOlaRec.lIndexNumber, sizeof(tagOlaRec.lIndexNumber));
				tagOlaRec.cardData = info.CardSaleInfo.cardData;
				tagOlaRec.extraData = info.CardSaleInfo.extraData;
				trs.GetTag(&tagOlaRec.tagTrs);

#if SPEEDPASS_SIMULATOR
				char sUDF_AccountNumber[100];
				if (!_Module.m_server.m_cParam.LoadParam("Allied", "RFID_AccountNumber", sUDF_AccountNumber, sizeof(sUDF_AccountNumber), FALSE, FALSE))
				{
					strcpy((char *)tagOlaRec.cardData.sAccountNumber, sUDF_AccountNumber);
				}
				if (!_Module.m_server.m_cParam.LoadParam("Allied", "RFID_TranType", sUDF_AccountNumber, sizeof(sUDF_AccountNumber), FALSE, FALSE))
				{
					tagOlaRec.cardData.sTranType = sUDF_AccountNumber[0];
				}
#endif
			}
			else if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			{
				retCode = GPI_TRS_NOT_FOUND;
			}
			else
				retCode = GPI_FAILED_PREPAY_IO;
		}
	}

	BYTE sTmp[sizeof(TAG_OLA_REC) + 2];
	Str2StrNull((BYTE *)&tagOlaRec, sizeof(TAG_OLA_REC), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	if ((retCode) && (retCode != GPI_TRS_NOT_FOUND))
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPAPTrs",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE, lRtc);
	}
	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::RemovePAPTrs(long lPumpNumber, long lSeqNumber, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::RemovePAPTrs   start");


#ifdef CATCH_GPI
	__try
	{
#endif
		RemovePAPTrs__(lPumpNumber, lSeqNumber, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::RemovePAPTrs")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "RemovePAPTrs()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::RemovePAPTrs__(long lPumpNumber, long lSeqNumber, long *pVal)
{
	GPIRetCode retCode = GPI_OK;
	long lRtc = OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	if (retCode == GPI_OK)
	{
		if (lSeqNumber < 0)			// - > RemovePAPTrs flags			//4.0.24.90 1022 HOT Fix
		{
			if ((~lSeqNumber) & (~GPI_REMOVE_PAP_TRS_REMOVE_FIRST))
			{
				CPumpTransact		cPumpTransact;
				PAY_AT_PUMP_INFO	info;

				cPumpTransact.SetLink(TRS_LINK_OLA_NUMBER, 0);
				memset(&info, 0, sizeof(PAY_AT_PUMP_INFO));

				lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST, &cPumpTransact, &info);

				if (lRtc == OK)
				{
					if (LOG_BASIC_FLOW_CONTROL)
					{
						CString strLogMsg;
						strLogMsg.Format("RemovePAPTrs - GPI_REMOVE_PAP_TRS_REMOVE_FIRST, First Record number %d, POS %d, trs %d, Shift %d", info.CardSaleInfo.lIndexNumber, m_lPosNumber, cPumpTransact.m_lNumber, cPumpTransact.m_lActiveShiftNumber);
						_LOGMSG.LogMsg(strLogMsg, LOG_LEVEL_1);
					}

					lSeqNumber = info.CardSaleInfo.lIndexNumber;

					_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, PAP_TRS_IN_OLA_QDX, cPumpTransact.m_lPump, ALARM_ON, cPumpTransact.m_lNumber, 0);
				}
			}
		}

		if (lRtc == OK)
		{
			CPumpTransact		trs;

			_Module.m_server.DeleteLink2Ola(lSeqNumber);

			trs.SetLink(TRS_LINK_OLA_NUMBER, lSeqNumber);
			lRtc = _Module.m_server.TrsLinkOperation(REC_DELETE, &trs, NULL);

			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("RemovePAPTrs - POS %d, SeqNumber %d, Rtc %d", m_lPosNumber, lSeqNumber, lRtc);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
			}
		}

		if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			retCode = GPI_TRS_NOT_FOUND;
		else if (lRtc)
			retCode = GPI_FAILED_PREPAY_IO;
	}

	if (pVal != NULL)
		*pVal = retCode;

	if ((retCode) && (retCode != GPI_TRS_NOT_FOUND))
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"RemovePAPTrs",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE, lRtc);
}

STDMETHODIMP CGPI::GetPrePaidTrs(long lPumpNumber, long lSyncNumber, short nFlags, BSTR * pRec, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPrePaidTrs   start");

	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPrePaidTrs, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}


#ifdef CATCH_GPI
	__try
	{
#endif
		GetPrePaidTrs__(lPumpNumber, lSyncNumber, nFlags, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetPrePaidTrs")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPrePaidTrs", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::GetPrePaidTrs__(long lPumpNumber, long lSyncNumber, short nFlags, BSTR *pRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;
	long lRtc = 0; // CID_184856 Uninitialized scalar variable

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;


	TAG_PUMP_TRANSACT	tagTrs;
	memset(&tagTrs, ' ', sizeof(TAG_PUMP_TRANSACT));


	if (retCode == GPI_OK)
	{
		CPumpTransact		trs;
		PRE_PAY_INFO		info;

		trs.SetLink(TRS_LINK_PREPAY_NUMBER, lSyncNumber);

		switch (nFlags)
		{
		case PREPAY_GET_ANY:
		{
			retCode = GPI_NOT_SUPPORTED;
			break;
		}
		case PREPAY_GET_BY_SYNC_NUMBER:
		{
			lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &trs, &info);
			break;
		}
		case PREPAY_GET_BY_PUMP_NUMBER:
		{
			if (PUMP_NOT_VALID(lPumpNumber))
				retCode = GPI_INVALID_PUMP_NUMBER;
			else
			{
				retCode = GPI_NOT_SUPPORTED;
			}
			break;
		}
		default:
			retCode = GPI_NOT_SUPPORTED;
			break;
		}

		if (retCode == GPI_OK)
		{
			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Pos %ld Pump %02ld , GetPrePaidTrs lSyncNumber %ld, nFlags %d ", m_lPosNumber, lPumpNumber, lSyncNumber, nFlags);
				_LOGMSG.LogMsg(str);
			}

			if ((trs.m_lPump) &&   //4.0.10.811
				(trs.GetLinkFlags() & TRS_LINK_FLEET) &&
				((trs.GetLinkFlags() & TRS_LINK_FLEET_PRICE_CHANGE)) &&
				(_Module.m_server.m_cPumpArray[trs.m_lPump - 1].GetDieselPumpFlags()) &&
				(_Module.m_server.m_cServerInfo.GetWaitForPriceChangeAckFlags())) 	//4.0.10.811 
			{
				if (LOG_BASIC_FLOW_CONTROL)  //4.0.10.183
				{
					CString str;
					str.Format("POS  %ld Pump %02ld , GetPrePaidTrs lSyncNumber %ld, nFlags %d Trs is lock due to Price change ", m_lPosNumber, trs.m_lPump, lSyncNumber, nFlags); //4.0.10.185 
					_LOGMSG.LogMsg(str);

				}
				trs.GetTag(&tagTrs); //4.0.10.191					 
				retCode = GPI_TRS_LOCKED;
			}
			else if (lRtc == OK)
			{
				if ((trs.m_lStatus == WAIT_RES) ||
					(trs.m_lStatus == WAIT_AUTH) ||
					(trs.m_lStatus == WAIT_PROGRESS) ||
					(trs.m_lStatus == IN_PROGRESS))
				{
					if (LOG_BASIC_FLOW_CONTROL)  //4.0.10.183
					{
						CString str;
						str.Format("POS %ld Pump %02ld , GetPrePaidTrs lSyncNumber %ld, nFlags %d Trs is lock due to trs inprocess ", m_lPosNumber, trs.m_lPump, lSyncNumber, nFlags); //4.0.10.185
						_LOGMSG.LogMsg(str);
					}

					retCode = GPI_TRS_LOCKED;

				}

				trs.GetTag(&tagTrs); //4.0.10.191					 
			}
			else if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			{
				retCode = GPI_TRS_NOT_FOUND;
			}
			else
				retCode = GPI_FAILED_PREPAY_IO;
		}
	}

	BYTE sTmp[sizeof(TAG_PUMP_TRANSACT) + 2];
	Str2StrNull((BYTE *)&tagTrs, sizeof(TAG_PUMP_TRANSACT), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	if (pVal != NULL)
		*pVal = retCode;

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPrePaidTrs",
				TRUE,
				retCode,
				(lRtc != 0) ? TRUE : FALSE,
				lRtc);
}

STDMETHODIMP CGPI::RemovePrePaidTrs(long lPumpNumber, long lSyncNumber, long * pVal)
{
	LOG_EXTERNALCALL("CGPI::RemovePrePaidTrs   start");


#ifdef CATCH_GPI
	__try
	{
#endif
		RemovePrePaidTrs__(lPumpNumber, lSyncNumber, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::RemovePrePaidTrs")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "RemovePrePaidTrs()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::RemovePrePaidTrs__(long lPumpNumber, long lSyncNumber, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	long lRtc = 0;


	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	if (retCode == GPI_OK)
	{
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		lRtc = _Module.m_server.m_cCommonFunction.RemovePrePaidTrs__(lPumpNumber, lSyncNumber);

		//LeaveCriticalSection( &_Module.m_server.m_csPumpArray ); //4.0.11.502

		if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			retCode = GPI_TRS_NOT_FOUND;
		else if (retCode)
			retCode = GPI_FAILED_PREPAY_IO;
	}

	if (pVal != NULL)
		*pVal = retCode;

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"RemovePrePaidTrs",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE, lRtc);
}

STDMETHODIMP CGPI::GetMinimalPumpsInfo2(BSTR *pRec, long *pVal)
{
//	LOG_EXTERNALCALL("CGPI::GetMinimalPumpsInfo2   start");
	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "GetMinimalPumpsInfo2", m_lPosNumber, LOG_POS, "GetMinimalPumpsInfo2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetMinimalPumpsInfo2__(pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetMinimalPumpsInfo2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetMinimalPumpsInfo2()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::GetMinimalPumpsInfo2__(BSTR * pRec, long * pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;

	TAG_MINIMAL_ALL_PUMPS_2	info;
	memset(&info, ' ', sizeof(TAG_MINIMAL_ALL_PUMPS_2));

	if (retCode == GPI_OK)
	{
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		int nPumps = MAX_PUMPS;

		i2a((short int)nPumps,
			info.sNumberOfPumps,
			(unsigned int)sizeof(info.sNumberOfPumps));

		if (_Module.m_server.GetAlarmSrvMode() == ALRMSRV_DIRECT_ACTIVE)
		{
			if (_Module.m_server.m_cAlarm.m_lAlarmPumpMap[0])
			{
				l2a(ALARM_SRV_CODE,
					info.sSysError,
					(unsigned int)sizeof(info.sSysError));
			}
			else
			{
				l2a(0,
					info.sSysError,
					(unsigned int)sizeof(info.sSysError));
			}

			if (_Module.m_server.m_lDiskWarning || _Module.m_server.m_lSysMisc)
				l2a(ALARM_SRV_CODE,
					info.sSysMisc,
					(unsigned int)sizeof(info.sSysMisc));
			else
				l2a(0,
					info.sSysMisc,
					(unsigned int)sizeof(info.sSysMisc));
		}
		else
		{
			l2a(_Module.m_server.m_lSysError,
				info.sSysError,
				(unsigned int)sizeof(info.sSysError));

			if (_Module.m_server.m_lSysMisc)
				l2a(_Module.m_server.m_lSysMisc,
					info.sSysMisc,
					(unsigned int)sizeof(info.sSysMisc));
			else if (_Module.m_server.m_lDiskWarning)
				l2a(_Module.m_server.m_lDiskWarning,
					info.sSysMisc,
					(unsigned int)sizeof(info.sSysMisc));
			else
				l2a(0,
					info.sSysMisc,
					(unsigned int)sizeof(info.sSysMisc));
		}

		memset(info.sConvertLayerVer, ' ', sizeof(info.sConvertLayerVer));
		memcpy(info.sConvertLayerVer,
			(LPCTSTR)_Module.m_server.m_strConvertLayerVersion,
			MIN(sizeof(info.sConvertLayerVer), _Module.m_server.m_strConvertLayerVersion.GetLength()));

		memset(info.sPumpSrvVer, ' ', sizeof(info.sPumpSrvVer));
		memcpy(info.sPumpSrvVer,
			(LPCTSTR)_Module.m_server.m_strPumpSrvVersion,
			MIN(sizeof(info.sPumpSrvVer), _Module.m_server.m_strPumpSrvVersion.GetLength()));

		for (int i = 0; i < MAX_PUMPS; i++)
		{
			_Module.m_server.m_cPumpArray[i].GetTagMinimal(&(info.minimalPump[i]), 2);
		}
	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetMinimalPumpsInfo2",
				TRUE, retCode,
				FALSE, 0);
	}

	BYTE sTmp[sizeof(TAG_MINIMAL_ALL_PUMPS_2) + 2];
	Str2StrNull((BYTE *)&info, sizeof(TAG_MINIMAL_ALL_PUMPS_2), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	//Save the last time the POS call GPI method.
	_Module.m_server.m_cPosMap[m_lPosNumber].SetLastCmd(CMD_NULL, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::InitInterface2(long lPosNumber, long lVersion, long lFlags1, long lFlags2, long lFlags3, long *pVal)
{
//	LOG_EXTERNALCALL("CGPI::InitInterface2   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		InitInterface2__(lPosNumber, lVersion, lFlags1, lFlags2, lFlags3, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::InitInterface2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "InitInterface2()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::InitInterface2__(long lPosNumber, long lVersion, long lFlags1, long lFlags2, long lFlags3, long * pVal)
{
	GPIRetCode retCode;
	long lRetCode;
	BOOL bUpdatePosMap = FALSE;

	InitInterface(lPosNumber, lVersion, &lRetCode);

	retCode = (GPIRetCode)lRetCode;

	switch (retCode)
	{
	case GPI_INVALID_POS_NUMBER:
		break;
	case GPI_OK:
		if (LOG_BASIC_FLOW_CONTROL)
			_LOGMSG.LogReq(m_lPosNumber, "IGPI", "InitInterface2", "connected to server");
		bUpdatePosMap = TRUE;
		break;
	case GPI_POS_ALREADY_CONNECTED:
		if (LOG_BASIC_FLOW_CONTROL)
			_LOGMSG.LogReq(m_lPosNumber, "IGPI", "InitInterface2", "re - connected to server");
		bUpdatePosMap = TRUE;
		break;
	}

	if (bUpdatePosMap)
	{

		CSingleLock slPosMap(&(_Module.m_server.m_csPosMap));
		slPosMap.Lock(INFINITE);

		CPosInfo tmpPos;
		if (_Module.m_server.m_cPosMap.Lookup(lPosNumber, tmpPos))
		{
			if (lFlags2 & INIT_REAL_POS)
				tmpPos.SetRealPOS();

			tmpPos.SetPumpsInView(lFlags1, lFlags3);   //4.0.0.79
			_Module.m_server.m_cPosMap.SetAt(lPosNumber, tmpPos);
			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Pumps in view %x---%x", lFlags1, lFlags3);  //4.0.0.79
				_LOGMSG.LogMsg("CGPI", "InitInterface2", m_lPosNumber, LOG_POS, str);
			}

		}
		else
		{
			retCode = GPI_INVALID_POS_NUMBER;
		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				lPosNumber, // Src
				"IGPI",
				"InitInterface2",
				TRUE, retCode,
				FALSE, 0);


	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::SetPumpMode(long lPumpNumber, long lMode, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetPumpMode   start");


#ifdef CATCH_GPI
	__try
	{
#endif
		SetPumpMode__(lPumpNumber, lMode, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::SetPumpMode")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetPumpMode()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;

}

void CGPI::SetPumpMode__(long lPumpNumber, long lMode, long *pVal)
{
	CString str;
	GPIRetCode retCode = GPI_OK;
	BOOL	   lReadyForMode = TRUE;

	if (lPumpNumber != 0)
	{
		if (PUMP_NOT_VALID(lPumpNumber))
			retCode = GPI_INVALID_PUMP_NUMBER;

		else if (PUMP_NOT_IN_WORKING_STATE(lPumpNumber))
			retCode = GPI_PUMP_NOT_IN_WORKING_STATE;
	}

	if (retCode == GPI_OK)
	{
		if (MODE_NOT_VALID(lMode) && lMode != 0) //Mode zero is back to Auto mode.
			retCode = GPI_INVALID_PUMP_MODE;
		else if (FALSE == _Module.m_server.DBConnectionAvailable())
			retCode = GPI_FAILED_IO;
		else if (SERVER_STATE_STARTUP)
		{
			if (_Module.m_server.ISPAKFinalizationFlagSet())
				retCode = GetAppropriateRetCode(SERVER_STARTUP);
			else
				retCode = GPI_CONTROLLER_NOT_AVAILABLE;
		}
	}

	if (retCode == GPI_OK)
	{
		//amram AGIP check if the pumps ready for mode change
		if (lPumpNumber)
		{
			//check if pump valid
			if (!_Module.m_server.IsPumpReadyForModeChange(lPumpNumber - 1))//4.0.9.506 //amram
			{
				str.Format("ModeSchemeProcess:New Mode can't change because the pump =%ld not valid ", lPumpNumber);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
				lReadyForMode = FALSE;
				retCode = GPI_PUMP_NOT_READY_FOR_MODE_CHANGE;
			}
		}
		else
		{
			int p = 0;
			BOOL bPumpValid = TRUE;
			while (p < _Module.m_server.GetLastValidPump())
			{
				bPumpValid = _Module.m_server.IsPumpReadyForModeChange(p);
				if (!bPumpValid)
				{
					str.Format("ModeSchemeProcess:New Mode can't change because the pump =%ld not valid ", p + 1);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
					lReadyForMode = FALSE;
					retCode = GPI_PUMP_NOT_READY_FOR_MODE_CHANGE;
					break;
				}
				p++;
			}
		}
		if (lReadyForMode)
		{
			//4.0.31.0 - TD 326234
			long lLastValidPump = (lPumpNumber == 0) ? _Module.m_server.GetLastValidPump() : lPumpNumber;

			for (int p = (lPumpNumber == 0) ? 1 : lPumpNumber; p <= lLastValidPump; p++)
			{
				SavePumpMode(p, lMode);
			}

			_Module.m_server.SetReqiredModeProcess();
		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"SetPumpMode",
				TRUE, retCode,
				FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetPumpTransact2(long lPumpNumber, long lTrsNumber, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPumpTransact2   start");


	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPumpTransact2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}


#ifdef CATCH_GPI
	__try
	{
#endif
		GetPumpTransact2__(lPumpNumber, lTrsNumber, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetPumpTransact2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPumpTransact2()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;

}

void CGPI::GetPumpTransact2__(long lPumpNumber, long lTrsNumber, BSTR *pRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	TAG_PUMP_TRANSACT2	tmpTrs;
	memset(&tmpTrs, ' ', sizeof(TAG_PUMP_TRANSACT2));
	TAG_PUMP_TRANSACT5	trs;
	memset(&trs, ' ', sizeof(TAG_PUMP_TRANSACT5));


	if ((retCode == GPI_OK) || (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED))
	{
		int		iRetNum = 0;
		long	p = lPumpNumber - 1;


		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		iRetNum = _Module.m_server.m_cPumpArray[p].GetTagTrsByNumber(lTrsNumber, &trs, 2);

		//4.0.26.501 - TD 116227
		//Convert extended grade
		if (_Module.m_server.GetMaxGrade999())
			_Module.m_server.GetExtGradeByGrade(trs.sGrade, sizeof(trs.sGrade), lPumpNumber);

		if (iRetNum != 0)
			retCode = GPI_TRS_NOT_FOUND;
	}

	if (retCode)
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPumpTransact2",
				TRUE, retCode,
				FALSE, 0);
	}


	//TD 38654 - backward competability -
	//convert from TAG_PUMP_TRANSACT2 back to TAG_PUMP_TRANSACT5
	BYTE sTmp[sizeof(TAG_PUMP_TRANSACT2) + 2];
	_Module.m_server.m_cCommonFunction.ConvertFromTrs5ToTrs2(&trs, &tmpTrs);
	Str2StrNull((BYTE *)&tmpTrs, sizeof(TAG_PUMP_TRANSACT2), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	if (pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGPI::GetPumpInfo2(long lPumpNumber, BSTR *pRec, long *pVal)
{
//	LOG_EXTERNALCALL("CGPI::GetPumpInfo2   start");
	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPumpInfo2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}


#ifdef CATCH_GPI
	__try
	{
#endif
		GetPumpInfo2__(lPumpNumber, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetPumpInfo2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPumpInfo2()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GetPumpInfo2__(long lPumpNumber, BSTR *pRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
		{
			if (PUMPSRV_IS_MASTER)
				retCode = GPI_OK;  //Intentionally left this retcode for FCC compatability

			else
				retCode = GPI_CONTROLLER_NOT_AVAILABLE;
		}

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	TAG_PUMP2		tmpPump;
	memset(&tmpPump, ' ', sizeof(TAG_PUMP2));

	TAG_PUMP2_EX		pump;
	memset(&pump, ' ', sizeof(TAG_PUMP2_EX));



	if (retCode == GPI_OK)
	{
		long p = lPumpNumber - 1;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		_Module.m_server.m_cPumpArray[p].GetTag(&pump, 2);
	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPumpInfo2",
				TRUE, retCode,
				FALSE, 0);
	}

	BYTE sTmp[sizeof(TAG_PUMP2) + 2];
	_Module.m_server.m_cCommonFunction.ConvertFromTagPump2ExToTagPump2(&pump, &tmpPump);
	Str2StrNull((BYTE *)&tmpPump, sizeof(TAG_PUMP2), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	//Save the last time the POS call GPI method.
	_Module.m_server.m_cPosMap[m_lPosNumber].SetLastCmd(CMD_NULL, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetPrePaidTrs2(long lPumpNumber, long lSyncNumber, short nFlags, BSTR *pRec, long *pVal)
{

	LOG_EXTERNALCALL("CGPI::GetPrePaidTrs2   start");

	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPrePaidTrs2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}


#ifdef CATCH_GPI
	__try
	{
#endif
		GetPrePaidTrs2__(lPumpNumber, lSyncNumber, nFlags, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetPrePaidTrs2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPrePaidTrs2()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
/*	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString str, sRetVal;
		str.Format("CGPI::GetPrePaidTrs2   END.  lPumpNumber=%ld, lSyncNumber=%ld, nFlags=%ld", lPumpNumber, lSyncNumber, nFlags);
		if (pVal)
			sRetVal.Format(" RetVal=%d", *pVal);
			str = str + sRetVal;
		_LOGMSG.LogMsg(str);
	}*/
	return S_OK;
}

void CGPI::GetPrePaidTrs2__(long lPumpNumber, __int64 lSyncNumber, short nFlags, BSTR *pRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;
	PRE_PAY_INFO		info;
	BOOL bClosedTrsFound = FALSE;
	long lRtc = 0; // CID_199501 Uninitialized scalar variable

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	CString str;
	str.Format("GPI::GetPrePaidTrs2__ Start flags = %ld SyncNumber = %ld", nFlags, lSyncNumber);
#ifdef DEBUG
	if (LOG_ALL_FLOW_CONTROL)
		_LOGMSG.LogMsg(str);
#endif

	TAG_PUMP_TRANSACT2	tmpTagTrs;
	memset(&tmpTagTrs, ' ', sizeof(TAG_PUMP_TRANSACT2));

	TAG_PUMP_TRANSACT5	tagTrs;

	memset(&tagTrs, ' ', sizeof(TAG_PUMP_TRANSACT5));
	memset(&info, 0x00, sizeof(PRE_PAY_INFO));  //4.0.3.62


	if (retCode == GPI_OK)
	{
		CPumpTransact		trs;

		switch (nFlags & PREPAY_GET_OPTIONS_MASK) //4.0.5.39
		{
		case PREPAY_GET_ANY:
		{

			trs.SetLink(TRS_LINK_PREPAY_NUMBER, (long)lSyncNumber);

			lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST, &trs, &info);

			while ((lRtc == OK) && !bClosedTrsFound)
			{
				if (lRtc == OK)
				{
					if (trs.GetStatus() == PAID)
					{
						BOOL bEndLoop = TRUE;
						//Checking if the POS has requested to offer the transaction
						if (nFlags & GPPT_OFFER_TRS_TO_POS)
						{
							retCode = GPI_OK;
							lRtc = 0;

							if (info.nOwningPos != m_lPosNumber) //Transaction is owned by another POS or not owned at all
							{
								if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
								{
									retCode = GPI_TRS_LOCKED;
									lRtc = ERR_EXISTS;
								}
								else //Transaction isn't owned by anyone, We'll acquire ownership
								{
									info.nOwningPos = (short)m_lPosNumber;
									lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &trs, &info);
								}
							}
						}

						if (bEndLoop)    //4.0.11.30
						{
							trs.GetTag(&tagTrs, 2, info.lMaxValue, 0, info.nGrade2); //4.0.9.507										
							bClosedTrsFound = TRUE;
						}
					}

					if (!bClosedTrsFound) //4.0.11.30
						lRtc = _Module.m_server.TrsLinkOperation(REC_READ_NEXT, &trs, &info);

				}
				else
				{
					retCode = GPI_TRS_NOT_FOUND;
				}
			}

			if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			{
				retCode = GPI_TRS_NOT_FOUND;
			}
			else if (lRtc && (retCode != GPI_TRS_LOCKED))
				retCode = GPI_FAILED_PREPAY_IO;

			break;
		}
		case PREPAY_GET_BY_SYNC_NUMBER:
		case PREPAY_GET_BY_SYNC_NUMBER | PREPAY_IGNORE_EXTENDED_GRADE_ID_CONVERSION: //4.0.19.1060 - TD 123837 / 122869
		{
#ifdef DEBUG
			if (LOG_ALL_FLOW_CONTROL)
				_LOGMSG.LogMsg("CGPI::GetPrePaidTrs2__ PREPAY_GET_BY_SYNC_NUMBER");
#endif
			trs.SetLink(TRS_LINK_PREPAY_NUMBER, (long)lSyncNumber);
			lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &trs, &info);

			if (LOG_BASIC_FLOW_CONTROL)  //4.0.11.509
			{
				CString str;
				str.Format("POS - %ld , Pump %02ld , GetPrePaidTrs2 lSyncNumber %ld, nFlags %d ", m_lPosNumber, trs.m_lPump, lSyncNumber, nFlags); //4.0.10.110
				_LOGMSG.LogMsg(str);
			}

			if (lRtc == OK)
			{

				if ((trs.m_lPump) &&   //4.0.10.811
					(trs.GetLinkFlags() & TRS_LINK_FLEET) &&
					((trs.GetLinkFlags() & TRS_LINK_FLEET_PRICE_CHANGE)) &&
					(_Module.m_server.m_cPumpArray[trs.m_lPump - 1].GetDieselPumpFlags()) &&
					(_Module.m_server.m_cServerInfo.GetWaitForPriceChangeAckFlags())) 	//4.0.10.811 
				{
					retCode = GPI_TRS_LOCKED;
					if (LOG_BASIC_FLOW_CONTROL)  //4.0.10.183
					{
						CString str;
						str.Format("Pump %02ld , GetPrePaidTrs2 lSyncNumber %ld, nFlags %d Trs is lock due to Price change ", trs.m_lPump, lSyncNumber, nFlags); //4.0.10.183
						_LOGMSG.LogMsg(str);

					}


				}
				else if ((trs.m_lStatus == WAIT_RES) ||
					(trs.m_lStatus == WAIT_AUTH) ||
					(trs.m_lStatus == WAIT_PROGRESS) ||
					(trs.m_lStatus == IN_PROGRESS) ||
					((trs.m_lStatus == ACCEPTED) && (PUMPSRV_REPORT_ON_TRS_PAID)))  //TD 65030 4.0.20.500
				{
					retCode = GPI_TRS_LOCKED;

					if (LOG_BASIC_FLOW_CONTROL)  //4.0.11.118
					{
						CString str;
						str.Format("POS - %ld , Pump %02ld , Trs locked lSyncNumber %ld, nFlags %d, trs status %ld ", m_lPosNumber, trs.m_lPump, lSyncNumber, nFlags, trs.m_lStatus); //4.0.10.110	//TD 65030 4.0.20.500
						_LOGMSG.LogMsg(str);
					}

				}
				else
				{
					if (lRtc == OK)
					{
						//Checking if the POS has requested to offer the transaction
						if (nFlags & GPPT_OFFER_TRS_TO_POS)
						{
							if (info.nOwningPos != m_lPosNumber) //Transaction is owned by another POS or not owned at all
							{
								if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
								{
									retCode = GPI_TRS_LOCKED;
									lRtc = ERR_EXISTS;
								}
								else //Transaction isn't owned by anyone, We'll acquire ownership
								{
									info.nOwningPos = (short)m_lPosNumber;
									lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &trs, &info);
								}
							}
						}
					}
					trs.GetTag(&tagTrs, 2, info.lMaxValue);

					if (trs.GetLinkFlags() & TRS_LINK_EXTRA_INFO)
					{
						EXTRA_INFO_REC	ExtraInfoRec;
						memset(&ExtraInfoRec, 0, sizeof(EXTRA_INFO_REC));
						ExtraInfoRec.info.lIndexNumber = (LONG)lSyncNumber;
						lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);

						if (!lRtc)
						{
							l2a(ExtraInfoRec.info.lCarWashDiscount, tagTrs.sCarWashDiscount, sizeof(tagTrs.sCarWashDiscount));
							l2a(ExtraInfoRec.info.lCarWashAmount * 10, tagTrs.sCarWashAmount, sizeof(tagTrs.sCarWashAmount));
							l2a(ExtraInfoRec.info.lCarWashTax, tagTrs.sCarWashTax, sizeof(tagTrs.sCarWashTax));
							memcpy(tagTrs.sWashCode, ExtraInfoRec.info.sWashCode, sizeof(ExtraInfoRec.info.sWashCode)); //4.0.21.501 - TD 70634
							memcpy(tagTrs.sCarWashProgram, &ExtraInfoRec.info.sCarWashProgram, sizeof(ExtraInfoRec.info.sCarWashProgram));
							memcpy(tagTrs.sWashExpire, ExtraInfoRec.info.sWashExpire, sizeof(tagTrs.sWashExpire));
							l2a(1, tagTrs.sCarWashQty, sizeof(tagTrs.sCarWashQty));
							memcpy(tagTrs.sCarWashUnitPrice, tagTrs.sCarWashAmount, sizeof(tagTrs.sCarWashUnitPrice));

						}

					}
				}

			}
			else if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			{
				retCode = GPI_TRS_NOT_FOUND;
			}
			else if (retCode != GPI_TRS_LOCKED)
				retCode = GPI_FAILED_PREPAY_IO;


			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Pos %ld Call GetPrePaidTrs2 trs %ld ,retCode=%d , lRtc=%ld ", m_lPosNumber, lSyncNumber, retCode, lRtc);
				_LOGMSG.LogMsg(str);
			}

			break;
		}
		case PREPAY_GET_BY_PUMP_NUMBER:
		{
			if (PUMP_NOT_VALID(lPumpNumber))
				retCode = GPI_INVALID_PUMP_NUMBER;
			else
			{
				retCode = GPI_NOT_SUPPORTED;
			}
			break;
		}
		// need to take all trs  with loop 4.0.11.30
		case PREPAY_GET_NEXT_INDEX:
		{

			trs.SetLink(TRS_LINK_PREPAY_NUMBER, (long)lSyncNumber);
#ifdef DEBUG
			if (LOG_ALL_FLOW_CONTROL)
				_LOGMSG.LogMsg("CGPI::GetPrePaidTrs2__ Read Next Row");
#endif
			if (LOG_BASIC_FLOW_CONTROL)
			{
				_LOGMSG.LogReq(m_lPosNumber, "IGPI", "QueryPrePaidTrs", "QUERY_CLOSE_TRS, GET_NEXT_INDEX");

			}

			lRtc = _Module.m_server.TrsLinkOperation(REC_READ_NEXT, &trs, &info);


			while ((lRtc == OK) && !bClosedTrsFound)
			{
				if (lRtc == OK)
				{
					if (trs.GetStatus() == PAID)
					{
						BOOL bEndLoop = TRUE;
						//Checking if the POS has requested to offer the transaction
						if (nFlags & GPPT_OFFER_TRS_TO_POS)
						{
							retCode = GPI_OK;
							lRtc = 0;

							if (info.nOwningPos != m_lPosNumber) //Transaction is owned by another POS or not owned at all
							{
								if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
								{
									retCode = GPI_TRS_LOCKED;
									lRtc = ERR_EXISTS;
								}
								else //Transaction isn't owned by anyone, We'll acquire ownership
								{
									info.nOwningPos = (short)m_lPosNumber;
									lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &trs, &info);
								}
							}
						}

						if (bEndLoop)    //4.0.11.30
						{
							trs.GetTag(&tagTrs, 2, info.lMaxValue, 0, info.nGrade2); //4.0.9.507										
							bClosedTrsFound = TRUE;
						}
					}

					if (!bClosedTrsFound) //4.0.11.30
						lRtc = _Module.m_server.TrsLinkOperation(REC_READ_NEXT, &trs, &info);

				}
				else
				{
					retCode = GPI_TRS_NOT_FOUND;
				}
			}


			if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			{
				retCode = GPI_TRS_NOT_FOUND;
			}
			break;
		}
		case PREPAY_GET_BY_PURCHASE_ID_PAID_ONLY:	//4.0.20.500
		case PREPAY_GET_BY_PURCHASE_ID://4.0.19.180
		case PREPAY_GET_BY_PURCHASE_ID | PREPAY_IGNORE_EXTENDED_GRADE_ID_CONVERSION: //4.0.19.1081 - TD 127170
		case PREPAY_GET_BY_PURCHASE_ID_PAID_ONLY | PREPAY_IGNORE_EXTENDED_GRADE_ID_CONVERSION: //4.0.19.1060 - TD 123837 / 122869

		{
			//In this case, SyncNumber is the PurchaseId
			retCode = GetPrepayByPurchaseId(lSyncNumber, nFlags, tagTrs);
			break;
		}

		}
	}

	//4.0.26.501 - TD 116227
	//Make extended grade conversion if needed
	if (_Module.m_server.GetMaxGrade999() && !(nFlags & PREPAY_IGNORE_EXTENDED_GRADE_ID_CONVERSION))
		_Module.m_server.GetExtGradeByGrade(tagTrs.sGrade, sizeof(tagTrs.sGrade), lPumpNumber);


	if (PREPAY_GET_BY_STRUCT_TRANSACT3 & nFlags) //4.0.3.62
	{
		TAG_PUMP_TRANSACT3	tagTrs3;
		BYTE sTmp[sizeof(TAG_PUMP_TRANSACT3) + 2];
		memcpy(&tagTrs3, &tagTrs, min(sizeof(tagTrs), sizeof(tagTrs3))); //38654
		l2a(info.lTotalReward, tagTrs3.sRewardsTotal, sizeof(tagTrs3.sRewardsTotal));
		l2a(info.lPreSetLimit, tagTrs3.sPreSetLimit, sizeof(tagTrs3.sPreSetLimit));
		tagTrs3.sPreSetLimitType = info.sPreSetLimitType;

		memcpy(&tagTrs3.sStartDateTime, &tagTrs.sStartDateTime, sizeof(tagTrs3.sStartDateTime));	//38654 cnt.

		Str2StrNull((BYTE *)&tagTrs3, sizeof(TAG_PUMP_TRANSACT3), sTmp);
		CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
		*pRec = strResult.Copy();

	}
	else
	{
		//RFUEL-3309 Get the maximal discount allowed for the grade 
		if (a2l(tagTrs.sNumber, sizeof(tagTrs.sNumber)) > 0)  // Skip if RetCode is GP_OK but no transaction exists
		{
			long lGrade = a2l(tagTrs.sGrade, sizeof(tagTrs.sGrade));
			long lMaxAllowedDiscount = _Module.m_server.GetMaximalDiscountForGrade(lGrade);  
			l2a(lMaxAllowedDiscount, tagTrs.sMaxDiscountAllowed, sizeof(tagTrs.sMaxDiscountAllowed));
		}
		BYTE sTmp[sizeof(TAG_PUMP_TRANSACT5) + 2];
		Str2StrNull((BYTE *)&tagTrs, sizeof(TAG_PUMP_TRANSACT5), sTmp);
		CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
		*pRec = strResult.Copy();
#ifdef DEBUG
		if (LOG_ALL_FLOW_CONTROL)
		{
			CString str1(strResult);
			CString str = "GetPrePaidTrs2__ returns Buf[" + str1 + "]";
			_LOGMSG.LogMsg(str);
		}
#endif
	}


	if (pVal != NULL)
		*pVal = retCode;

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPrePaidTrs2",
				TRUE,
				retCode,
				(lRtc != 0) ? TRUE : FALSE,
				lRtc);

}

STDMETHODIMP CGPI::GetPAPTrs2(long lPumpNumber, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPAPTrs2   start");
	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPAPTrs2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetPAPTrs2__(lPumpNumber, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetPAPTrs2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPAPTrs2", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;

}

void CGPI::GetPAPTrs2__(long lPumpNumber, BSTR *pRec, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	long		lRtc = 0;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	TAG_OLA_REC2 tmpTagOlaRec;
	memset(&tmpTagOlaRec, ' ', sizeof(TAG_OLA_REC2));
	TAG_OLA_REC2_EX tagOlaRec;
	memset(&tagOlaRec, ' ', sizeof(TAG_OLA_REC2_EX));

	if (retCode == GPI_OK)
	{
		if (lPumpNumber) // search by pump 
		{
			if (PUMP_NOT_VALID(lPumpNumber))
				retCode = GPI_INVALID_PUMP_NUMBER;
			else
			{
				retCode = GPI_NOT_SUPPORTED;
			}
		}
		else // search for any PAP concluded trs
		{
			CPumpTransact		trs;
			PAY_AT_PUMP_INFO	info;

			trs.SetLink(TRS_LINK_OLA_NUMBER, 0);
			memset(&info, 0, sizeof(PAY_AT_PUMP_INFO));

			lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST, &trs, &info);
			if (lRtc == OK)
			{
				if (LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("GetPAPTrs2__ - First Record number %d, trs %d, Shift %d, (GradePrice = %ld, Nozzle %d, Value %ld)", info.CardSaleInfo.lIndexNumber, trs.m_lNumber, trs.m_lActiveShiftNumber, trs.m_lGradePrice, trs.m_nNozzle, trs.m_lRoundedValue);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
				}

				l2a(info.CardSaleInfo.lIndexNumber, tagOlaRec.lIndexNumber, sizeof(tagOlaRec.lIndexNumber));
				tagOlaRec.cardData = info.CardSaleInfo.cardData;
				tagOlaRec.extraData = info.CardSaleInfo.extraData;
				trs.GetTag(&tagOlaRec.tagTrs, 2);




			}
			else if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			{
				retCode = GPI_TRS_NOT_FOUND;
			}
			else
				retCode = GPI_FAILED_PREPAY_IO;

		}
	}

	BYTE sTmp[sizeof(TAG_OLA_REC2) + 2];

	_Module.m_server.m_cCommonFunction.ConvertFromTagOlaRec2ExToTagOlaRec2(&tagOlaRec, &tmpTagOlaRec);
	Str2StrNull((BYTE *)&tmpTagOlaRec, sizeof(TAG_OLA_REC2), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	if ((retCode) && (retCode != GPI_TRS_NOT_FOUND))
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPAPTrs2",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE, lRtc);
	}

	if (pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CGPI::GetPumpStatus2(long lPumpNumber, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPumpStatus2   start");


	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPumpStatus2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetPumpStatus2__(lPumpNumber, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetPumpStatus2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPumpStatus2", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GetPumpStatus2__(long lPumpNumber, BSTR * pRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	TAG_PUMP_STATUS2	pumpStat;
	memset(&pumpStat, ' ', sizeof(TAG_PUMP_STATUS2));


	if (retCode == GPI_OK)
	{
		long p = lPumpNumber - 1;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		_Module.m_server.m_cPumpArray[p].GetTagStat(&pumpStat, 2);
	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPumpStatus2",
				TRUE, retCode,
				FALSE, 0);
	}


	BYTE sTmp[sizeof(TAG_PUMP_STATUS2) + 2];
	Str2StrNull((BYTE *)&pumpStat, sizeof(TAG_PUMP_STATUS2), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetParam(BSTR sTableAndFieldName, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, long *pVal)
{
//	LOG_EXTERNALCALL("CGPI::GetParam   start");
	USES_CONVERSION;

	if (((sVal == NULL) && (nFiledType == FIELD_SZ))
		|| ((lVal == NULL) && (nFiledType == FIELD_DWORD)))
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "GetParam", m_lPosNumber, LOG_POS, "GetParam, Throw COM Exception : E_POINTER", LOG_LEVEL_0);
		return E_POINTER;
	}


#ifdef CATCH_GPI
	__try
	{
#endif
		GetParam__(sTableAndFieldName, lKey1, lKey2, nFiledType, sVal, lVal, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetParam")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetParam", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GetParam__(BSTR sTableAndFieldName, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, long * pVal)
{
	GPIRetCode retCode = GPI_OK;
	BOOL bAllocEmpty = FALSE;
	CString strEmptyName;
	DWORD dwLastErr = 0;

	long lRtc = -1;
	char szFieldVal[MAX_FIELD_VAL];
	char szTableAndFieldName[MAX_TABLE_NAME + MAX_FIELD_NAME];


	memset(szTableAndFieldName, 0, sizeof(szTableAndFieldName));
	int iLen = WideCharToMultiByte(CP_ACP, 0, sTableAndFieldName, -1, szTableAndFieldName, sizeof(szTableAndFieldName), NULL, NULL);
	if (iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GPI_TABLE_NAME_TOO_LONG;
	}
	else
	{
		strEmptyName = szTableAndFieldName;
		strEmptyName.TrimLeft();
		if (strEmptyName.IsEmpty())
			retCode = GPI_INVALID_TABLE_NAME;
	}

	char szTableName[MAX_TABLE_NAME];
	char szFieldName[MAX_FIELD_NAME];
	memset(szTableName, 0, sizeof(szTableName));
	memset(szFieldName, 0, sizeof(szFieldName));


	if (retCode == GPI_OK)
	{
		BYTE *pTmp;
		pTmp = _Module.m_server.GetStringTillDelimiter((BYTE *)szTableAndFieldName, ',', (BYTE *)szTableName);
		if (pTmp)
			memcpy(szFieldName, (const void *)pTmp, sizeof(szFieldName));
		else
		{
			pTmp = _Module.m_server.GetStringTillDelimiter((BYTE *)szTableAndFieldName, ';', (BYTE *)szTableName);
			if (pTmp)
				memcpy(szFieldName, (const void *)pTmp, sizeof(szFieldName));
			else
				retCode = GPI_INVALID_TABLE_NAME;
		}


		if (LOG_DETAIL_FLOW_CONTROL)
		{

			CString str;
			str.Format("TableName %.20s ,Key1 %ld ,Key2 %ld ,FieldName %.20s, FiledType %s",
				szTableName, lKey1, lKey2, szFieldName, (nFiledType) ? "FIELD_SZ" : "FIELD_DW");
			_LOGMSG.LogReq(m_lPosNumber,
				"IGPI",
				"GetParam",
				(LPCSTR)str);
		}
	}


	BOOL bAllowCreate = FALSE;


	if (nFiledType == FIELD_SZ)
	{
		memset(szFieldVal, 0, sizeof(szFieldVal));

		if (retCode == GPI_OK)
		{
			lRtc = _PARAM.GetParam(szTableName, lKey1, lKey2, szFieldName, nFiledType, szFieldVal, sizeof(szFieldVal), NULL, bAllowCreate);
			if (lRtc != 0)
			{
				bAllocEmpty = TRUE;
			}
			else
			{
				CString strRetVal(szFieldVal);
				if (strRetVal.GetLength())
				{
					*sVal = strRetVal.AllocSysString();
				}
				else
				{
					bAllocEmpty = TRUE;
				}
			}
		}
		else
		{
			bAllocEmpty = TRUE;
		}
	}
	else // field DW 
	{
		DWORD dwRetVal = 0;

		if (retCode == GPI_OK)
		{
			if (!strcmp(szFieldName, "ActiveShiftNumber")) //TD 33846 , 38407 , 38498
				lRtc = _PARAM.LoadParam(szTableName, szFieldName, &dwRetVal, 1, FALSE, TRUE, 0, 0, TRUE);
			else
				lRtc = _PARAM.GetParam(szTableName, lKey1, lKey2, szFieldName, nFiledType, NULL, 0, &dwRetVal, bAllowCreate);
		}

		*lVal = (long)dwRetVal;

		bAllocEmpty = TRUE;
	}


	if (bAllocEmpty)
	{
		CString strRetVal("  ");
		*sVal = strRetVal.AllocSysString();
	}

	if (lRtc != -1) // lRtc have been used - something happened.
	{
		switch (lRtc)
		{
		case 0: retCode = GPI_OK; break;
		case 1: retCode = GPI_FAILED_TO_OPEN_TABLE; break;
		case 3: retCode = GPI_PARAM_NOT_FOUND_FAILED_TO_CREATE; break;
		case 4: retCode = GPI_PARAM_NOT_FOUND; break;
		}
	}




	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetParam",
				TRUE, retCode,
				(dwLastErr != 0) ? TRUE : FALSE, dwLastErr);


	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::QueryPAPTrs(long lPumpNumber, long lIndexNumber, short nFlags, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::QueryPAPTrs   start");
	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "QueryPAPTrs, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		QueryPAPTrs__(lPumpNumber, lIndexNumber, nFlags, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::QueryPAPTrs")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "QueryPAPTrs", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::QueryPAPTrs__(long lPumpNumber, long lIndexNumber, short nFlags, BSTR *pRec, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	long		lRtc = 0;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	TAG_OLA_REC2 tmpTagOlaRec;
	memset(&tmpTagOlaRec, ' ', sizeof(TAG_OLA_REC2));
	TAG_OLA_REC2_EX tagOlaRec;
	memset(&tagOlaRec, ' ', sizeof(TAG_OLA_REC2_EX));

	if (retCode == GPI_OK)
	{
		CPumpTransact		trs;
		PAY_AT_PUMP_INFO	info;

		trs.SetLink(TRS_LINK_OLA_NUMBER, lIndexNumber);
		memset(&info, 0, sizeof(PAY_AT_PUMP_INFO));

		switch (nFlags)
		{
		case GET_FIRST_INDEX:
		{
			lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST, &trs, &info);
			break;
		}
		case GET_DIRECT_INDEX:
		{
			lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &trs, &info);
			break;
		}
		case GET_NEXT_INDEX:
		{
			lRtc = _Module.m_server.TrsLinkOperation(REC_READ_NEXT, &trs, &info);
			break;
		}
		}


		////////////////////////////////////////////////////////////////
		if (lRtc == OK)
		{
			l2a(info.CardSaleInfo.lIndexNumber, tagOlaRec.lIndexNumber, sizeof(tagOlaRec.lIndexNumber));
			tagOlaRec.cardData = info.CardSaleInfo.cardData;
			tagOlaRec.extraData = info.CardSaleInfo.extraData;
			trs.GetTag(&tagOlaRec.tagTrs, 2);
		}
		else if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
		{
			retCode = GPI_TRS_NOT_FOUND;
		}
		else
			retCode = GPI_FAILED_IO;
	}


	BYTE sTmp[sizeof(TAG_OLA_REC2) + 2];
	_Module.m_server.m_cCommonFunction.ConvertFromTagOlaRec2ExToTagOlaRec2(&tagOlaRec, &tmpTagOlaRec);
	Str2StrNull((BYTE *)&tmpTagOlaRec, sizeof(TAG_OLA_REC2), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	if ((retCode) && (retCode != GPI_TRS_NOT_FOUND))
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"QueryPAPTrs",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE, lRtc);
	}

	if (pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGPI::QueryPrePaidTrs(long lPumpNumber, long lIndexNumber, short nFlags, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::QueryPrePaidTrs   start");

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "QueryPrePaidTrs, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		QueryPrePaidTrs__(lPumpNumber, lIndexNumber, nFlags, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::QueryPrePaidTrs")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "QueryPrePaidTrs", LOG_LEVEL_0);
	}
#endif 


	return S_OK;

}

void CGPI::QueryPrePaidTrs__(long lPumpNumber, long lIndexNumber, short nFlags, BSTR *pRec, long *pVal, const long lFlags2)
{

	USES_CONVERSION;
	GPIRetCode retCode = GPI_OK;
	CPumpTransact trs;
	PRE_PAY_INFO info; //CID_199785 Uninitialized scalar variable - added C'tor to the struct instead

	long lRtc = 0; // CID_199866 Uninitialized scalar variable

	CString str;
	str.Format("GPI::QueryPrePaidTrs()__ Start flags = %ld , flags2 = %ld", nFlags, lFlags2);
	_LOGMSG.LogMsg(str);
	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	TAG_PRE_PAY_REC		tmpTagPrePayRec;
	memset(&tmpTagPrePayRec, ' ', sizeof(TAG_PRE_PAY_REC));
	TAG_PRE_PAY_REC_EX		tagPrePayRec;
	memset(&tagPrePayRec, ' ', sizeof(TAG_PRE_PAY_REC_EX));


	if (lPumpNumber)
		if (PUMP_NOT_VALID(lPumpNumber))
			retCode = GPI_INVALID_PUMP_NUMBER;

	if (retCode == GPI_OK)
	{

		trs.SetLink(TRS_LINK_PREPAY_NUMBER, lIndexNumber);


		// Query closed - Unbalanced transactions 
		//-------------------------------------------
		if ((nFlags & QUERY_MASK) || (nFlags & QUERY_MASK2)) //4.0.10.504
		{
			BOOL bClosedTrsFound = FALSE;
			CTime cCurTime = CTime::GetCurrentTime();

			switch (nFlags & GET_INDEX_MASK)
			{
			case GET_FIRST_INDEX:
			{
				if (LOG_BASIC_FLOW_CONTROL)
				{
					_LOGMSG.LogReq(m_lPosNumber, "IGPI", "QueryPrePaidTrs", "QUERY_CLOSE_TRS,GET_FIRST_INDEX");
				}

				lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST, &trs, &info);
				if (lRtc == OK)
				{
					//Checking if the POS has requested to offer the transaction
					if (nFlags & GPPT_OFFER_TRS_TO_POS)
					{
						if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
						{
							retCode = GPI_TRS_LOCKED;
							lRtc = ERR_EXISTS;
						}
					}
				}

				while ((lRtc == OK) && (!bClosedTrsFound))
				{
					if (LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("TrsLinkOperation Return %ld, trs = %ld", lRtc, trs.m_lNumber);
						_LOGMSG.LogMsg(str);
					}

					if (lRtc == OK)
					{
						if (trs.GetStatus() == PAID)
						{
							if (lPumpNumber)
							{
								if (trs.m_lPump == lPumpNumber)
								{
									if (((nFlags & QUERY_MASK) & QUERY_CLOSE_TIMEOUT) || //4.0.10.31
										((nFlags & QUERY_MASK2) & QUERY_CLOSE_TIMEOUT))
									{
										CTimeSpan ts;
										if (cCurTime > trs.GetTime())
										{
											ts = cCurTime - trs.GetTime();
											if (ts.GetTotalSeconds() >= (LONG)_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetClosedPrePayTimeOut())
											{
												bClosedTrsFound = TRUE;
											}
										}
									}
									else
									{
										bClosedTrsFound = TRUE;
									}
								}
							}
							else
							{
								//Passing zero as the punmp number means that we aren't
								//querying a specific pump, but simply going over the records
								//in the Q-Dex one by one.
								//if( (nFlags & QUERY_MASK) & QUERY_CLOSE_TIMEOUT )
								if (((nFlags & QUERY_MASK) & QUERY_CLOSE_TIMEOUT) || //4.0.10.504
									((nFlags & QUERY_MASK2) & QUERY_CLOSE_TIMEOUT))

								{
									CTimeSpan ts;
									if (cCurTime > trs.GetTime())
									{
										ts = cCurTime - trs.GetTime();
										if (PUMP_NOT_VALID(trs.m_lPump))
											_LOGMSG.LogMsg("CGPI::QueryPrePaidTrs__(), found trs with invalid pump number", LOG_LEVEL_0);

										else if (ts.GetTotalSeconds() >= (LONG)_Module.m_server.m_cPumpArray[trs.m_lPump - 1].GetClosedPrePayTimeOut())
										{
											bClosedTrsFound = TRUE;
										}
									}
								}
								else
								{
									bClosedTrsFound = TRUE;
								}
							}
						}

						if (!bClosedTrsFound)
						{
							lRtc = _Module.m_server.TrsLinkOperation(REC_READ_NEXT, &trs, &info);
						}
					}
					else
					{
						retCode = GPI_TRS_NOT_FOUND;
					}
				}

				if (!bClosedTrsFound)
				{
					retCode = GPI_TRS_NOT_FOUND;
				}

				break;
			}
			case GET_DIRECT_INDEX:
			{
				if (LOG_BASIC_FLOW_CONTROL)
				{
					_LOGMSG.LogReq(m_lPosNumber, "IGPI", "QueryPrePaidTrs", "QUERY_CLOSE_TRS, GET_DIRECT_INDEX");
				}

				lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &trs, &info);

				if (lRtc == OK)
				{
					//Checking if the POS has requested to offer the transaction
					if (nFlags & GPPT_OFFER_TRS_TO_POS)
					{
						if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
						{
							retCode = GPI_TRS_LOCKED;
							lRtc = ERR_EXISTS;
						}
					}
				}

				if ((lRtc == OK) && (trs.GetStatus() != PAID))
					lRtc = ERR_NOT_FOUND;
				break;
			}
			case GET_NEXT_INDEX:
			{
				if (LOG_BASIC_FLOW_CONTROL)
				{
					_LOGMSG.LogReq(m_lPosNumber, "IGPI", "QueryPrePaidTrs", "QUERY_CLOSE_TRS, GET_NEXT_INDEX");
				}

				lRtc = _Module.m_server.TrsLinkOperation(REC_READ_NEXT, &trs, &info);

				if (lRtc == OK)
				{
					//Checking if the POS has requested to offer the transaction
					if (nFlags & GPPT_OFFER_TRS_TO_POS)
					{
						if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
						{
							retCode = GPI_TRS_LOCKED;
							lRtc = ERR_EXISTS;
						}
					}
				}

				while ((lRtc == OK) && (!bClosedTrsFound))
				{
					if (LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("TrsLinkOperation Return %ld, trs = %ld, Pump %02ld", lRtc, trs.m_lNumber, trs.m_lPump);
						_LOGMSG.LogMsg(str);
					}


					if (lRtc == OK)
					{
						if (trs.GetStatus() == PAID)
						{
							if (lPumpNumber)
							{
								if (trs.m_lPump == lPumpNumber)
								{
									if (((nFlags & QUERY_MASK) & QUERY_CLOSE_TIMEOUT) || //4.0.10.504
										((nFlags & QUERY_MASK2) & QUERY_CLOSE_TIMEOUT))

									{
										CTimeSpan ts;
										if (cCurTime > trs.GetTime())
										{
											ts = cCurTime - trs.GetTime();
											if (ts.GetTotalSeconds() >= (LONG)_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetClosedPrePayTimeOut())
											{
												bClosedTrsFound = TRUE;
											}
										}
									}
									else
									{
										bClosedTrsFound = TRUE;
									}
								}
							}
							else
							{
								if (((nFlags & QUERY_MASK) & QUERY_CLOSE_TIMEOUT) || //4.0.10.504
									((nFlags & QUERY_MASK2) & QUERY_CLOSE_TIMEOUT))

								{
									//Passing zero as the punmp number means that we aren't
									//querying a specific pump, but simply going over the records
									//in the Q-Dex one by one.
									CTimeSpan ts;
									if (cCurTime > trs.GetTime())
									{
										ts = cCurTime - trs.GetTime();
										if (PUMP_NOT_VALID(trs.m_lPump))
											_LOGMSG.LogMsg("CGPI::QueryPrePaidTrs__(), found trs with invalid pump number", LOG_LEVEL_0);

										else if (ts.GetTotalSeconds() >= (LONG)_Module.m_server.m_cPumpArray[trs.m_lPump - 1].GetClosedPrePayTimeOut())
										{
											bClosedTrsFound = TRUE;
										}
									}
								}
								else
								{
									bClosedTrsFound = TRUE;
								}
							}
						}

						if (!bClosedTrsFound)
						{
							lRtc = _Module.m_server.TrsLinkOperation(REC_READ_NEXT, &trs, &info);
						}
					}
					else
					{
						retCode = GPI_TRS_NOT_FOUND;
					}
				}

				if (!bClosedTrsFound)
				{
					retCode = GPI_TRS_NOT_FOUND;
				}

				break;
			}
			}
		}

		// Query all transactions 
		//------------------------
		else // Regular Query 
		{
			_LOGMSG.LogMsg("CGPI::QueryPrePaidTrs Reguler Query");
			switch (nFlags & GET_INDEX_MASK)
			{
			case GET_FIRST_INDEX:
			{
				lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST, &trs, &info);

				if (lRtc == OK)
				{
					//Checking if the POS has requested to offer the transaction
					if (nFlags & GPPT_OFFER_TRS_TO_POS)
					{
						if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
						{
							retCode = GPI_TRS_LOCKED;
							lRtc = ERR_EXISTS;
						}
					}
				}

				break;
			}
			case GET_DIRECT_INDEX:
			{
				lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &trs, &info);

				if (lRtc == OK)
				{
					//Checking if the POS has requested to offer the transaction
					if (nFlags & GPPT_OFFER_TRS_TO_POS)
					{
						if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
						{
							retCode = GPI_TRS_LOCKED;
							lRtc = ERR_EXISTS;
						}
					}
				}

				break;
			}
			case GET_NEXT_INDEX:
			{
				lRtc = _Module.m_server.TrsLinkOperation(REC_READ_NEXT, &trs, &info);

				if (lRtc == OK)
				{
					//Checking if the POS has requested to offer the transaction
					if (nFlags & GPPT_OFFER_TRS_TO_POS)
					{
						if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
						{
							retCode = GPI_TRS_LOCKED;
							lRtc = ERR_EXISTS;
						}
					}
				}

				break;
			}
			}
		}

		if (lRtc == OK ||
			((lRtc == ERR_EXISTS) && (retCode == GPI_TRS_LOCKED))) //TD 56221 4.0.17.340
		{
			trs.GetTag(&tagPrePayRec.tagTrs, 2, info.lMaxValue); /* CID_199785 Uninitialized scalar variable: TrsLinkOperation
																 updated lMaxValue already */

			l2a(info.lFlags, tagPrePayRec.tagInfo.sFlags, sizeof(tagPrePayRec.tagInfo.sFlags));		//4.0.9.507
			l2a(info.lGradePrice, tagPrePayRec.tagInfo.sGradePrice, sizeof(tagPrePayRec.tagInfo.sGradePrice));
			i2a(info.nLevel, tagPrePayRec.tagInfo.sLevel, sizeof(tagPrePayRec.tagInfo.sLevel));
			l2a(info.lMaxValue, tagPrePayRec.tagInfo.sMaxValue, sizeof(tagPrePayRec.tagInfo.sMaxValue));
			l2a(info.lMaxVolume, tagPrePayRec.tagInfo.sMaxVolume, sizeof(tagPrePayRec.tagInfo.sMaxVolume));
			l2a(info.lPOSNumber, tagPrePayRec.tagInfo.sPOSNumber, sizeof(tagPrePayRec.tagInfo.sPOSNumber));
			l2a(info.lPumpNumber, tagPrePayRec.tagInfo.sPumpNumber, sizeof(tagPrePayRec.tagInfo.sPumpNumber));
			l2a(info.lSyncNumber, tagPrePayRec.tagInfo.sSyncNumber, sizeof(tagPrePayRec.tagInfo.sSyncNumber));
			long lPresetLimitType = 0;
			if ((info.lMaxValue == 0) && (info.lMaxVolume > 0))
			{
				lPresetLimitType = 1;
			}
			l2a(lPresetLimitType, &tagPrePayRec.tagTrs.sPreSetType, sizeof(tagPrePayRec.tagTrs.sPreSetType));//4.0.22.503 72394
		}
		// Not OK
		else if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
		{
			retCode = GPI_TRS_NOT_FOUND;
		}
		// Not OK 
		else if (retCode != GPI_TRS_LOCKED)
			retCode = GPI_FAILED_PREPAY_IO;
	}

	//4.0.26.501 - TD 116227
	//Make extended grade conversion if needed
	if (_Module.m_server.GetMaxGrade999())
		_Module.m_server.GetExtGradeByGrade(tagPrePayRec.tagTrs.sGrade, sizeof(tagPrePayRec.tagTrs.sGrade), lPumpNumber);


	if (GET_PRE_PAY_TAG2 & nFlags)
	{
		TAG_PRE_PAY_REC2 tmpTagPrePayRec2;
		TAG_PRE_PAY_REC2_EX tagPrePayRec2;

		memset(&tmpTagPrePayRec2, ' ', sizeof(tmpTagPrePayRec2));
		memset(&tagPrePayRec2, ' ', sizeof(tagPrePayRec2));
		memcpy(&tagPrePayRec2, &tagPrePayRec, sizeof(tagPrePayRec));

		l2a(info.lTotalReward, tagPrePayRec2.sTotalReward, sizeof(tagPrePayRec2.sTotalReward));

		if (LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("QueryPrePaidTrs trs = %ld, reward=%ld", trs.m_lNumber, info.lTotalReward);
			_LOGMSG.LogMsg(str);
		}

		//TD 38654 - support new structs accordinf to flag QUERY_FLAGS2
		if (lFlags2 & QUERY_FLAGS2)
		{
			BYTE sTmp[sizeof(TAG_PRE_PAY_REC2_EX) + 2];
			Str2StrNull((BYTE *)&tagPrePayRec2, sizeof(TAG_PRE_PAY_REC2_EX), sTmp);
			CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
			*pRec = strResult.Copy();
		}
		else
		{
			BYTE sTmp[sizeof(TAG_PRE_PAY_REC2) + 2];
			_Module.m_server.m_cCommonFunction.ConvertFromTagPrepay2ExToTagPrepay2(&tagPrePayRec2, &tmpTagPrePayRec2);
			Str2StrNull((BYTE *)&tmpTagPrePayRec2, sizeof(TAG_PRE_PAY_REC2), sTmp);
			CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
			*pRec = strResult.Copy();
		}


	}
	else
	{
		if (lFlags2 & QUERY_FLAGS2)		//TD 38654
		{
			BYTE sTmp[sizeof(TAG_PRE_PAY_REC_EX) + 2];
			Str2StrNull((BYTE *)&tagPrePayRec, sizeof(TAG_PRE_PAY_REC_EX), sTmp);
			CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
			*pRec = strResult.Copy();
		}
		else
		{
			BYTE sTmp[sizeof(TAG_PRE_PAY_REC) + 2];
			_Module.m_server.m_cCommonFunction.ConvertFromTagPrepayExToTagPrepay(&tagPrePayRec, &tmpTagPrePayRec);
			Str2StrNull((BYTE *)&tmpTagPrePayRec, sizeof(TAG_PRE_PAY_REC), sTmp);
			CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
			*pRec = strResult.Copy();
		}
	}



	if (pVal != NULL)
		*pVal = retCode;

	if ((retCode) && (retCode != GPI_TRS_NOT_FOUND))
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"QueryPrePaidTrs",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE, lRtc);
}


STDMETHODIMP CGPI::SetProcess(long lProcessNumber, long lParam1, long lParam2, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetProcess   start");

#ifdef CATCH_GPI
	__try
	{
#endif
		// 		char str[256];
		// 		sprintf(str,"Set process from GPI lProcess = %ld, lParam1 = %ld, lParam2 = %ld", lProcessNumber, lParam1, lParam2);
		// 		_LOGMSG.LogMsg(str);
		SetProcess__(lProcessNumber, lParam1, lParam2, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::SetProcess")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetProcess()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;

}

void CGPI::SetProcess__(long lProcessNumber, long lParam1, long lParam2, long * pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	if (PROCESS_PRINTER_STATUS == lProcessNumber)
		lParam1 = m_lPosNumber;

	if (GPI_OK == retCode)
		_Module.m_server.m_cCommonFunction.SetProcess(lProcessNumber, lParam1, lParam2, 0, 0, (long*)&retCode);


	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"SetProcess",
				TRUE, retCode,
				FALSE, 0);


	if (pVal != NULL)
		*pVal = retCode;
}


void CGPI::SetProcessExt__(long lProcessNumber, long lParam1, BSTR sInData, BSTR *sOutData, long * pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;


	if (GPI_OK == retCode)
		_Module.m_server.m_cCommonFunction.SetProcessExt(lProcessNumber, lParam1, sInData, sOutData, (long*)&retCode);


	if (retCode)
		if (LOG_LOGIC_ERROR)
			_Module.m_server.m_cLogger.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"etProcessExt",
				TRUE, retCode,
				FALSE, 0);


	if (pVal != NULL)
		*pVal = retCode;
}



STDMETHODIMP CGPI::GetProcess(long lProcessNumber, long *plParam1, long *plParam2, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetProcess   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		GetProcess__(lProcessNumber, plParam1, plParam2, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetProcess")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetProcess()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;

}

void CGPI::GetProcess__(long lProcessNumber, long *plParam1, long *plParam2, long * pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;


	if (retCode == GPI_OK)
	{
		switch (lProcessNumber)
		{

		case PROCESS_LOCK:
			//==============================================================
		{
			DWORD dwRetVal = 0;

			long lRtc = _PARAM.GetParam("GeneralParam", 0, 0, "ProcessLockStatus", FIELD_DWORD, NULL, 0, &dwRetVal, FALSE);

			*plParam1 = (long)dwRetVal;

			if (lRtc != -1) // lRtc have been used - something happened.
			{
				switch (lRtc)
				{
				case 0: retCode = GPI_OK; break;
				case 1: retCode = GPI_FAILED_TO_OPEN_TABLE; break;
				case 3: retCode = GPI_PARAM_NOT_FOUND_FAILED_TO_CREATE; break;
				case 4: retCode = GPI_PARAM_NOT_FOUND; break;
				}
			}

		}
		break;

		case PROCESS_GET_SHIFT_GPI: //4.0.16.500 - TD 38127
		{
			long lShiftStatus = 0;
			*plParam1 = 0;

			SERVER_INFO sInfo;

			// Before getting shift status, check we are not in middle of other process
			_Module.m_server.m_cServerInfo.GetInfo(sInfo);

			if (sInfo.byRequiredMaintenanceProcess || sInfo.byDeliveryEmulationStatus || sInfo.byMemoInProcess || sInfo.lWetStockFlags)
			{
				retCode = GPI_IN_MIDDLE_OF_PROCESS;
			}


			if (!retCode)
			{
				_Module.m_server.m_cCommonFunction.GetShiftProcessStatus(plParam1, &lShiftStatus);

				if (lShiftStatus)
					retCode = GPI_SERVER_IN_MIDDLE_LOCK_SHIFT;
			}


			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("GPI Shift process: ShiftNumber = %ld, ShiftStatus = %d", *plParam1, retCode);
				_LOGMSG.LogMsg(str);
			}

		}
		break;

		case PROCESS_ANY:
		{
			if (_Module.m_server.m_cCommonFunction.IsIntheMiddleOfProc())
			{
				retCode = GPI_IN_MIDDLE_OF_PROCESS;
				*plParam1 = GetMiddleOfProcId(); //TD 254219  4.0.28.501
			}
			else
			{
				_LOGMSG.LogMsg("CGPI::GetProcess__() - Not in a middle of any proc");
				retCode = GPI_OK;
			}

		}
		break;

		default:
			//==============================================================
			retCode = GPI_INVALID_PROCESS_NUMBER;
			break;
		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetProcess",
				TRUE, retCode,
				FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetParam2(BSTR *sTableFieldAndVal, long lKey1, long lKey2, short nFiledType, long *lVal, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetParam2   start");
	USES_CONVERSION;

	if (sTableFieldAndVal == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "GetParam2", m_lPosNumber, LOG_POS, "GetParam2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);
		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetParam2__(sTableFieldAndVal, lKey1, lKey2, nFiledType, lVal, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetParam2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetParam2", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::GetParam2__(BSTR *sTableFieldAndVal, long lKey1, long lKey2, short nFiledType, long *lVal, long *pVal)
{
	GPIRetCode retCode = GPI_OK;
	BOOL bAllocEmpty = FALSE;
	CString strEmptyName;
	DWORD dwLastErr = 0;

	long lRtc = -1;
	char szTableFieldAndVal[MAX_TABLE_NAME + MAX_FIELD_NAME + MAX_FIELD_VAL];
	memset(szTableFieldAndVal, 0, sizeof(szTableFieldAndVal));
	int iLen = WideCharToMultiByte(CP_ACP, 0, *sTableFieldAndVal, -1, szTableFieldAndVal, sizeof(szTableFieldAndVal), NULL, NULL);
	if (iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GPI_TABLE_NAME_TOO_LONG;
	}
	else
	{

		strEmptyName = szTableFieldAndVal;
		strEmptyName.TrimLeft();
		if (strEmptyName.IsEmpty())
			retCode = GPI_PARAM_NOT_FOUND;

	}

	SysFreeString(*sTableFieldAndVal);


	char szTableName[MAX_TABLE_NAME];
	char szFieldName[MAX_FIELD_NAME];
	char szFieldVal[MAX_FIELD_VAL];

	memset(szTableName, 0, sizeof(szTableName));
	memset(szFieldName, 0, sizeof(szFieldName));
	memset(szFieldVal, 0, sizeof(szFieldVal));


	if (retCode == CTRL_OK)
	{
		BYTE *pTmp;

		BOOL bDelimiterFound =
			_Module.m_server.GetStringTillDelimiterOrEnd((BYTE *)szTableFieldAndVal,
			(BYTE *)szTableName,
				sizeof(szTableName),
				&pTmp);
		if (bDelimiterFound)
		{
			bDelimiterFound =
				_Module.m_server.GetStringTillDelimiterOrEnd((BYTE *)pTmp,
				(BYTE *)szFieldName,
					sizeof(szFieldName),
					&pTmp);
		}
		else
		{
			retCode = GPI_PARAM_NOT_FOUND;
		}

		if (bDelimiterFound)
			bDelimiterFound =
			_Module.m_server.GetStringTillDelimiterOrEnd((BYTE *)pTmp,
			(BYTE *)szFieldVal,
				sizeof(szFieldVal),
				&pTmp);
	}


	BOOL bAllowCreate = FALSE;

	if (nFiledType == FIELD_SZ)
	{
		memset(szFieldVal, 0, sizeof(szFieldVal));

		if (retCode == GPI_OK)
		{
			lRtc = _PARAM.GetParam(szTableName, lKey1, lKey2, szFieldName, nFiledType, szFieldVal, sizeof(szFieldVal), NULL, bAllowCreate);
			if (lRtc != 0)
			{
				bAllocEmpty = TRUE;
			}
			else
			{
				CString strRetVal(szFieldVal);
				if (strRetVal.GetLength())
				{
					*sTableFieldAndVal = strRetVal.AllocSysString();

				}
				else
				{
					bAllocEmpty = TRUE;
				}
			}
		}
		else
		{
			bAllocEmpty = TRUE;
		}
	}
	else // field DW 
	{
		DWORD dwRetVal = 0;

		if (retCode == GPI_OK)
			lRtc = _PARAM.GetParam(szTableName, lKey1, lKey2, szFieldName, nFiledType, NULL, 0, &dwRetVal, bAllowCreate);

		*lVal = (long)dwRetVal;

		bAllocEmpty = TRUE;
	}


	if (bAllocEmpty)
	{
		CString strRetVal("  ");
		*sTableFieldAndVal = strRetVal.AllocSysString();
	}

	if (lRtc != -1) // lRtc have been used - something happened.
	{
		switch (lRtc)
		{
		case 0: retCode = GPI_OK; break;
		case 1: retCode = GPI_FAILED_TO_OPEN_TABLE; break;
		case 3: retCode = GPI_PARAM_NOT_FOUND_FAILED_TO_CREATE; break;
		case 4: retCode = GPI_PARAM_NOT_FOUND; break;
		}
	}

	if (retCode)
	{ //3.2.0.69
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("TableName %.20s ,Key1 %ld ,Key2 %ld ,FieldName %.20s, FiledType %s, FieldVal %.30s",
				szTableName, lKey1, lKey2, szFieldName, (nFiledType) ? "FIELD_SZ" : "FIELD_DW", szFieldVal);
			_LOGMSG.LogReq(m_lPosNumber,
				"IGPI",
				"GetParam2",
				(LPCSTR)str);
		}
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetParam2",
				TRUE, retCode,
				(dwLastErr != 0) ? TRUE : FALSE, dwLastErr);
	}

	if (pVal != NULL)
		*pVal = retCode;
}


//4.0.23.504 76432
long CGPI::GetFlagsForOperation(CarWashOperation eOperation, long lFlags)
{
	long lReturnedFlags = 0;

	switch (eOperation)
	{
	case CW_GetWashCode:
	case CW_GenerateWashCode:
	{
		lReturnedFlags = (lFlags & CAR_WASH_SYNC_FUNC) ? CAR_WASH_SYNC_FUNC : lFlags;
		lReturnedFlags = (lFlags & CAR_WASH_SETUP_FUNC) ? CAR_WASH_SETUP_FUNC : lFlags;
		lReturnedFlags = (lFlags & CAR_WASH_COLD_START_FUNC) ? CAR_WASH_COLD_START_FUNC : lFlags;
		break;
	}
	default:
	{
		lReturnedFlags = lFlags;
	}
	}
	return lReturnedFlags;
}


STDMETHODIMP CGPI::GenerateWashCode(long lDevice, long lWashProgram, long lFlags, long *lSyncNumber, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GenerateWashCode   start");
	if (lSyncNumber == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "GenerateWashCode", m_lPosNumber, LOG_POS, "GenerateWashCode, Throw COM Exception : E_POINTER", LOG_LEVEL_0);


		return E_POINTER;
	}
#ifdef CATCH_GPI
	__try
	{
#endif
		GenerateWashCode__(lDevice, lWashProgram, lFlags, lSyncNumber, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GenerateWashCode")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GenerateWashCode()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

STDMETHODIMP CGPI::GenerateWashCode2(long lDevice, BSTR bstrWashProgram, long lFlags, long *lSyncNumber, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GenerateWashCode2   start");
	//CString strWashCode;

	if (lSyncNumber == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "GenerateWashCode", m_lPosNumber, LOG_POS, "GenerateWashCode, Throw COM Exception : E_POINTER", LOG_LEVEL_0);


		return E_POINTER;
	}
#ifdef CATCH_GPI
	__try
	{
#endif
		GenerateWashCode2__(lDevice, bstrWashProgram, lFlags, lSyncNumber, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GenerateWashCode")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GenerateWashCode()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GenerateWashCode2__(long lDevice, BSTR bstrWashProgram, long lFlags, long *lSyncNumber, long *pVal)
{
	CString strWashCode(bstrWashProgram);
	long lWashProgram = _Module.m_server.GetWashIndexByProgramCode(strWashCode);
	GenerateWashCode__(lDevice, lWashProgram, lFlags, lSyncNumber, pVal);
}

void CGPI::GenerateWashCode__(long lDevice, long lWashProgram, long lFlags, long *lSyncNumber, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (!_Module.m_server.m_cCarWash.CarWashEnabled())
		retCode = GPI_CAR_WASH_NOT_ACTIVE;
	else if ((!_Module.m_server.m_cCarWash.CarWashSessionEmpty()) || (!_Module.m_server.m_cCarWashGeneralRequest.CarWashSessionEmpty())) //4.0.7.502 //4.0.23.504 76432
		retCode = GPI_CAR_WASH_IN_MIDDLE_OF_ANOTHER_SESSION;


	if (retCode == GPI_OK)
	{//4.0.23.504 76432

	 //select the first flag in case multiple flags were sent. 
		lFlags = GetFlagsForOperation(CW_GenerateWashCode, lFlags);

		switch (lFlags)
		{
		case CAR_WASH_COLD_START_FUNC:
		case CAR_WASH_SYNC_FUNC:
		case CAR_WASH_SETUP_FUNC:
		{
			_Module.m_server.m_cCarWashGeneralRequest.GenerateWashCode(lDevice, lWashProgram, lFlags, lSyncNumber, (long *)&retCode, m_lPosNumber);
		}
		break;

		default:	//Regular car wash
		{
			_Module.m_server.m_cCarWash.GenerateWashCode(lDevice, lWashProgram, lFlags, lSyncNumber, (long *)&retCode, m_lPosNumber);
		}

		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GenerateWashCode",
				TRUE, retCode,
				FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetWashCode(long lSyncNumber, long *lResult, BSTR *sData, long *lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetWashCode   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		GetWashCode__(lSyncNumber, lResult, sData, lFlags, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetWashCode")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetWashCode", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GetWashCode__(long lSyncNumber, long *lResult, BSTR *sData, long *lFlags, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
		retCode = GPI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (!_Module.m_server.m_cCarWash.CarWashEnabled())
		retCode = GPI_CAR_WASH_NOT_ACTIVE;

	*lResult = CW_DATA_NOT_YET_AVAILABLE; //Until prove it doesn't

	if (retCode == GPI_OK)
	{
		//4.0.23.504 76432

		//select the first flag in case multiple flags were sent. 
		*lFlags = GetFlagsForOperation(CW_GetWashCode, *lFlags);

		switch (*lFlags)
		{
		case CAR_WASH_COLD_START_FUNC:
		case CAR_WASH_SYNC_FUNC:
		case CAR_WASH_SETUP_FUNC:
		{
			_Module.m_server.m_cCarWashGeneralRequest.GetWashCode(lSyncNumber, lResult, sData, lFlags, (long *)&retCode, m_lPosNumber);
		}
		break;

		default:	//Regular car wash  and CAR_WASH_REFUND_FUNC TD 144221
		{
			_Module.m_server.m_cCarWash.GetWashCode(lSyncNumber, lResult, sData, lFlags, (long *)&retCode, m_lPosNumber);
		}

		}
	}


	if ((retCode != 0) && (retCode != 1))
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetWashCode",
				TRUE, retCode,
				FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::CancelWashCode(long lSyncNumber, long lDevice, long lFlags, BSTR sData, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::CancelWashCode   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		CancelWashCode__(&lSyncNumber, lDevice, lFlags, sData, FALSE, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::CancelWashCode")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "CancelWashCode()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::CancelWashCode__(long * lSyncNumber, long lDevice, long lFlags, BSTR sData, long lRefundCarWash, long *pVal)
{
	DWORD dwLastErr = 0L;
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
		retCode = GPI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (!_Module.m_server.m_cCarWash.CarWashEnabled())
		retCode = GPI_CAR_WASH_NOT_ACTIVE;

	if (retCode == GPI_OK)
	{
		//4.0.23.504 76432

		//select the first flag in case multiple flags were sent. 
		lFlags = GetFlagsForOperation(CW_GenerateWashCode, lFlags);

		switch (lFlags)
		{
		case CAR_WASH_COLD_START_FUNC:
		case CAR_WASH_SYNC_FUNC:
		case CAR_WASH_SETUP_FUNC:
		{
			_Module.m_server.m_cCarWashGeneralRequest.CancelWashCode(lSyncNumber, lDevice, lFlags, sData, lRefundCarWash, (long *)&retCode, m_lPosNumber);
		}
		break;

		default:	//Regular car wash and CAR_WASH_REFUND_FUNC TD 144221
		{
			_Module.m_server.m_cCarWash.CancelWashCode(lSyncNumber, lDevice, lFlags, sData, lRefundCarWash, (long *)&retCode, m_lPosNumber);
		}
		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"CancelWashCode",
				TRUE, retCode,
				(dwLastErr != 0) ? TRUE : FALSE, dwLastErr);

	if (pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGPI::AuthorizePump2(long lPumpNumber, short nLevel, long lGradePrice, short nFlags, long lLimit, short nGroup, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::AuthorizePump2   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		AuthorizePump2__(lPumpNumber, nLevel, lGradePrice, nFlags, lLimit, nGroup, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::AuthorizePump2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "AuthorizePump2()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::AuthorizePump2__(long lPumpNumber, short nLevel, long lGradePrice, long lFlags, long lLimit, short nGroup, long *pVal, BSTR sRec /*= NULL*/) //4.0.9.507
{
	BOOL bAuthAll = FALSE, bFoundPOS = TRUE;
	GPIRetCode retCode = GPI_OK;
	BOOL bForceIgnoreVit = FALSE;

	CPosInfo tmpPos;

	short nPrePayWaiting = 0, nFree = 0, nUnpaid = 0;
	if (lPumpNumber > 0)
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsListCount(&nPrePayWaiting, &nFree, &nUnpaid);

	if (lFlags & FORCE_IGNORE_VIT)
		bForceIgnoreVit = TRUE;

	AuthorizePumpCheckValidInput(lPumpNumber, nPrePayWaiting, nUnpaid, bAuthAll, retCode); // TD 370688

	if (retCode == GPI_OK)
	{
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		long p = lPumpNumber - 1;
		long lLastPump = lPumpNumber;

		if (bAuthAll)
		{
			p = 0L;
			lLastPump = _Module.m_server.GetLastValidPump();
		}

		CString sTmpLogMsg;

		for (; p < lLastPump; p++)
		{
			//Authorizing all pumps did not check that pumps are in view
			_Module.m_server.m_csPosMap.Lock(INFINITE);
			bFoundPOS = _Module.m_server.m_cPosMap.Lookup(m_lPosNumber, tmpPos);
			_Module.m_server.m_csPosMap.Unlock();

			if (bFoundPOS)
			{
				if (!tmpPos.IsPumpInFieldOfView(p + 1))
				{
					//Return GPI_PUMP_OUT_OF_VIEW only if a single
					//pump is being authorized.  Otherwise, just move
					//on.
					if (lPumpNumber)
						retCode = GPI_PUMP_OUT_OF_VIEW;

					continue;
				}
			}

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("AuthorizePump2 Flags %x", lFlags);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
			}

			{
				// 4.0.25.1140 TD 420361 
				long nPosition = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetPosition();
				BOOL bNozzleLocked = _Module.m_server.m_cCommonFunction.IsNozzleLocked(lPumpNumber, nPosition);
				if (TRUE == bNozzleLocked)
				{

					sTmpLogMsg.Format("Could not authorize pump #%02d because the nozzle #%02d is lock.", p + 1, nPosition);
					_Module.m_server.m_cLogger.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, sTmpLogMsg);

					retCode = GPI_PUMP_LOCKED;
					continue;
				}
			}

			if ((_Module.m_server.m_cPumpArray[p].PumpStoppedByController()) && (!(_Module.m_server.m_cPumpArray[p].GetStat().GetMisc() & MISC_VIT_CONFLICT) && !bForceIgnoreVit) && (!_Module.m_server.m_AllowAuthOnStopByController)) //4.0.22.511
			{
				sTmpLogMsg.Format("Could not authorize pump #%02d because it has been stopped by controller", p + 1);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, sTmpLogMsg);

				retCode = GPI_PUMP_STOPPED_BY_CONTROLLER;
			}
			else
			{
				//Check that pump match the group
				if (nGroup)
				{
					//Get the group number from the misc field
					long lMisc = _Module.m_server.m_cPumpArray[p].GetStat().GetMisc();
					lMisc >>= 12; //xxxxXxxx --> 000xxxxX
					lMisc &= 3;	  //000xxxxX --> 0000000X
					if ((lMisc + 1) != nGroup)
						continue;
				}

				CPumpStatus stat = _Module.m_server.m_cPumpArray[p].GetStat();

				if ((lFlags & PAK_WITH_LIMIT) &&	//4.0.9.507
					((stat.GetRes() == NULL_RES) &&
					(stat.GetResExt() == NULL_RES_EXT) &&
						(stat.GetStatus() == READY) || (stat.GetStatus() == NOZZLE_LEFT_OUT))) //4.0.3.50
				{
					CString str;
					str.Format("AuthorizePump2, Set Pump Reserve to PAK Pump %02ld Flags %x", p + 1, lFlags);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
					stat.SetRes(PAY_AT_KIOSK, NULL_RES_EXT);
					stat.SetStatus(REQUESTING);
					_Module.m_server.m_cPumpArray[p].SetStat(stat);
				}

				// special PrePay Operation
				// the GPI::AuthorizePump command will ditect 
				// that there is wait_auth trs and :
				// 1. change the trs status to wait_progres
				// 2. return pump to ready
				// 3. set trs time to current time.
				// 4, send authorize and limit insted of authorizePump.
				if (((_Module.m_server.AllowAuthOnNonRequest()) && (stat.GetRes() == NULL_RES)) ||
					((stat.GetStatus() == ERROR_TERMINATED) && (PUMPSRV_REPORT_ON_TRS_PAID)) ||  //4.0.5.34
					/*(lFlags & FORCE_IGNORE_VIT) ||*/ (stat.GetStatus() == REQUESTING) ||        // remarked 4.0.22.1270 TD 367952
					((_Module.m_server.m_bAllowAuthOnNonRequestPapResPumpCL) && (stat.GetRes() == PAY_AT_PUMP)) ||
					((_Module.m_server.m_AllowAuthOnStopByController) && (stat.GetStatus() == STOPPED_BY_CONTROLLER)))	//4.0.23.501 TD 78654	//4.0.21.500 TD 69683
				{
					CPumpTransact trs;	//4.0.22.504 73359
					if (stat.GetRes() == PRE_PAY_AT_POS)
					{
						retCode = AuthotizePumpPrePayFlow(p, trs, stat);
					}
					else if ((stat.GetRes() == PAY_AT_KIOSK) && ((_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_AUTH, TRS_LINK_PREPAY_NUMBER, &trs)) || (_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_RES, TRS_LINK_PREPAY_NUMBER, &trs))))
					{//4.0.22.504 73359
						retCode = GPI_PUMP_ALREADY_RESERVED;
						if (LOG_DETAIL_FLOW_CONTROL)
						{
							CString str;
							str.Format("AuthorizePump2 Pump %02ld , Authorizing PAK is denied since a pre pay is already authorized on the pump.",
								p + 1);

							_LOGMSG.LogMsg(str);
						}
					}
					else // PAK 
					{
						AuthotizePumpPayAtKisokFlow(stat, lFlags, retCode, lPumpNumber, nLevel, lGradePrice, lLimit, bForceIgnoreVit, sRec);
					}
				}
				else   if ((IsAuthorizeTerminalSrvTrs(lPumpNumber, m_lPosNumber, stat)
					|| IsAuthorizeMobileTrs(lPumpNumber, m_lPosNumber, stat))
					&& (_Module.m_server.AllowAuthOnNonRequest()))
				{
					BOOL bIsAllowAuthByMode = TRUE;

					if ((stat.GetRes() == PAY_AT_PUMP) && (stat.GetResExt() == RES_EXT_MOBILE) && (m_lPosNumber == MOBILE_SRV_POS_NUMBER)) // mobile TRS
					{
						bIsAllowAuthByMode = (TRUE == _Module.m_server.m_cModeArray[stat.GetMode() - 1].AutoAuthorize(PAY_AT_PUMP, RES_EXT_MOBILE));
					}

					if (TRUE == bIsAllowAuthByMode)
					{
						AuthoirzeAndLimitPAPExternal(lPumpNumber, nLevel, lGradePrice, lLimit, lFlags);  // TD 370688
					}
					else
						retCode = GPI_OK;
				}
				else
				{
					retCode = GPI_PUMP_NOT_REQUESTING;
				}

				if (bAuthAll)
					retCode = GPI_OK;
			}
		}
	}

	if (bAuthAll && (retCode != GPI_OK))
	{
		if ((retCode != GPI_MUST_INIT_FIRST) ||
			(retCode != GPI_CONTROLLER_NOT_AVAILABLE) ||
			(retCode != GPI_SERVER_NOT_READY) ||
			(retCode != GPI_FAILED_IO))
		{
			retCode = GPI_OK;
		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"AuthorizePump2",
				TRUE, retCode,
				FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}



void  CGPI::AuthotizePumpPayAtKisokFlow(CPumpStatus &stat, long &lFlags, GPIRetCode &retCode, long lPumpNumber, short nLevel, long lGradePrice, long lLimit, BOOL bForceIgnoreVit, BSTR sRec)
{
	long p = lPumpNumber - 1;

	BOOL bNullRes = FALSE;
	if ((stat.GetRes() == NULL_RES) && (_Module.m_server.AllowAuthOnNonRequest()))
	{
		bNullRes = TRUE; // Null reservation allowed.
	}

	if ((bNullRes) ||
		(stat.GetRes() == PAY_AT_KIOSK) ||
		(stat.GetRes() == ATTEND) ||
		(stat.GetRes() == TEST) ||
		(lFlags & FORCE_IGNORE_VIT) ||  //4.0.9.507
		(lFlags &IGNORE_VIT_DETECTION && stat.GetResExt() == RES_EXT_AVI)) //3.2.1.75)
	{
		short nPrePayWaiting, nFree, nUnpaid;
		CPumpTransact TmpTrs;

		_Module.m_server.m_cPumpArray[p].GetTrsListCount(&nPrePayWaiting, &nFree, &nUnpaid);


		if ((_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS, TRS_LINK_OLA_NUMBER, &TmpTrs)) ||
			(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_RES, TRS_LINK_PREPAY_NUMBER, &TmpTrs)) ||
			(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_AUTH, TRS_LINK_PREPAY_NUMBER, &TmpTrs)) ||
			(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS, TRS_NO_LINK, &TmpTrs) && (stat.GetStatus() != REQUESTING)))  //4.0.15.504
		{
			retCode = GPI_TRS_ALREADY_IN_PROGRESS;
		}
		else if ((_Module.m_server.m_cPumpArray[p].IsCurrentTrs()) &&
			!((stat.GetStatus() == ERROR_TERMINATED) && (PUMPSRV_REPORT_ON_TRS_PAID)) &&
			!((stat.GetStatus() == STOPPED_BY_CONTROLLER) && _Module.m_server.m_AllowAuthOnStopByController))	//4.0.23.501							 )  //4.0.5.34
		{
			retCode = GPI_PUMP_NOT_IN_WORKING_STATE;
		}
		else if (nUnpaid >= MAX_ALLOWED_UNPAID_TRS(p))
		{
			if (!_Module.m_server.UpdatePAKReconcileTrans(lPumpNumber))
				retCode = GPI_PUMP_TRS_LIST_FULL;

		}
		else if (_Module.m_server.PumpNotAllowedMixPAKAndPrePay(lPumpNumber, PRE_PAY_AT_POS)) // 4.0.9.501
		{
			retCode = GPI_PUMP_TRS_LIST_FULL;
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("AuthorizeAndLimit2 Pump %02ld , failed due to waiting PrePay",
					p + 1);

				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
			}
		}
		else
		{
			if (PUMPSRV_IS_MODE_MASTER)
			{
				PumpReservation lPumpRes;
				if (bNullRes)
				{
					lPumpRes = PAY_AT_KIOSK;
					if (LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("Pump %02d not Reserved ,Authorize for PAK transaction", p + 1);
						_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
					}
				}
				else
				{
					lPumpRes = stat.GetRes();
				}

				if ((_Module.m_server.m_cModeArray[stat.GetMode() - 1].ResNotValid(lPumpRes, stat.GetResExt())) && !(lFlags & FORCE_IGNORE_VIT) && !(lFlags & FORCE_PAK_WHEN_NOT_IN_MODE)) //4.0.9.507 //4.0.16.500 - TD 34675
				{
					retCode = GPI_NOT_VALID_IN_CURRENT_MODE;
				}
				else if ((lFlags & IGNORE_VIT_DETECTION) || (lFlags & FORCE_IGNORE_VIT))  //4.0.9.507
				{
					// Authorize ignore avi (due to collision or to a request in advance) only if PAK is valid.  
					if (_Module.m_server.m_cModeArray[stat.GetMode() - 1].ResNotValid(PAY_AT_KIOSK, NULL_RES_EXT))
					{
						retCode = GPI_NOT_VALID_IN_CURRENT_MODE;
						_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, "Requested to ignore Avi and authorize PAK - but PAK not valid ");
					}
				}
			}
		}
	}

	if ((lFlags & PAK_WITH_LIMIT) && (retCode) &&
		((stat.GetRes() == PAY_AT_KIOSK) && (stat.GetStatus() == REQUESTING)))
	{
		CString str;
		str.Format("Authorize Pump2  Fail  Pump Reserve to PAK Pump %02ld Flags %x", p + 1, lFlags);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
		stat.SetRes(NULL_RES, NULL_RES_EXT);
		stat.SetStatus(READY);
		_Module.m_server.m_cPumpArray[p].SetStat(stat);
	}

	//Allow "PD Error" release in any case: //4.0.24.60
	//====================================
	if (retCode != GPI_OK)
	{
		if ((stat.GetStatus() == STOPPED_BY_CONTROLLER) && (_Module.m_server.m_AllowAuthOnStopByController))
		{
			retCode = GPI_OK;
			if (LOG_BASIC_FLOW_CONTROL)
			{
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, "AuthorizePump2 called - clear PD Error.");
			}
		}
	}

	if (retCode == GPI_OK) //4.0.17.500
	{
		long lMisc = stat.GetMisc();
		if (lFlags & CHECK_AND_RESERVE_FLAG)
		{
			//Check that pump is not checked and reserved already
			if (lMisc & MISC_PUMP_CHECK_AND_RESERVE)
			{
				retCode = GPI_PUMP_ALREADY_RESERVED;
			}
			else
			{
				lMisc |= MISC_PUMP_CHECK_AND_RESERVE;
				stat.SetMisc(lMisc);
				_Module.m_server.m_cPumpArray[p].SetStat(stat);
			}
		}
		else
		{
			if (lMisc & MISC_PUMP_CHECK_AND_RESERVE)
			{
				// remove the misc
				lMisc &= MISC_NOT_PUMP_CHECK_AND_RESERVE;
				stat.SetMisc(lMisc);
				_Module.m_server.m_cPumpArray[p].SetStat(stat);
			}
		}
	}

	if (retCode == GPI_OK && !(lFlags & CHECK_AND_RESERVE_FLAG)) // 4.0.17.20 - TD 26384
	{
		long lFlagsTmp = lFlags; //4.0.9.507

		CMD_STRUCT_AUTHORIZE_LIMIT cmdAuthorize;
		// had here CID_200210 Memset fill value of '0' - added C'tor to the struct instead

		cmdAuthorize.nLevel = nLevel;
		cmdAuthorize.lGradePrice = lGradePrice;

		if (lLimit)
		{
			BOOL bPreSetLimitation = FALSE;  //4.0.3.44

			if (lFlagsTmp & PAK_WITH_LIMIT)	 //4.0.9.507
			{
				if (lFlagsTmp & GRADE_PRICE_IS_POSITION_NUM)
				{
					// We don't want GRADE_PRICE_IS_POSITION_NUM (0x0020) to be a part of the mask
					lFlagsTmp = lFlags & 0xB70F;
				}
				else	//4.0.9.508 TD 56001}
				{
					// We don't want FORCE_IGNORE_VIT (0x0800) to be a part of the mask
					lFlagsTmp = lFlags & 0xB7FF; //4.0.5.41
				}

				if (_Module.m_server.LimitedPreSet())
					bPreSetLimitation = TRUE;
			}

			switch (lFlagsTmp)  //4.0.9507
			{
			case LIMIT_BY_VALUE:
				if (bPreSetLimitation)
				{
					cmdAuthorize.lMaxValue = _Module.m_server.GetLimit(PRICE_LEVEL_CASH, p + 1);

					if (cmdAuthorize.lMaxValue > lLimit)
						cmdAuthorize.lMaxValue = lLimit;
				}
				else
				{
					cmdAuthorize.lMaxValue = lLimit;
				}

				cmdAuthorize.lMaxVolume = 0;
				break;

			case LIMIT_BY_VOLUME:

				cmdAuthorize.lMaxValue = 0;

				if (bPreSetLimitation)
				{
					cmdAuthorize.lMaxVolume = _Module.m_server.GetStationLimitVolume();
					if (cmdAuthorize.lMaxVolume > lLimit)
						cmdAuthorize.lMaxVolume = lLimit;
				}
				else
					cmdAuthorize.lMaxVolume = lLimit;
				break;

			case LIMIT_FILL_UP:
			default:
				lFlags = LIMIT_FILL_UP;  //4.0.9.507
				_Module.m_server.SetSiteLimit(p, nLevel, cmdAuthorize); // 4.0.22.850 TD 307758

				if (_Module.m_server.IsSiteLimitByVolume()) // TD 309771
					lFlags |= LIMIT_BY_VOLUME;
			}
		}
		else
		{
			cmdAuthorize.lFlags = LIMIT_FILL_UP;

			switch (lFlags) //4.0.9.507
			{
			case LIMIT_BY_VOLUME:
				if (_Module.m_server.IsSiteLimitByVolume())
				{
					cmdAuthorize.lMaxValue = 0;
					cmdAuthorize.lMaxVolume = _Module.m_server.GetStationLimitVolume();
				}
				break;
			case LIMIT_FILL_UP: //4.0.9.500
			{
				cmdAuthorize.lMaxValue = _Module.m_server.GetLimit(nLevel, p + 1);
				cmdAuthorize.lMaxVolume = 0;//4.0.9.500											
			}
			break;

			default:
				_Module.m_server.SetSiteLimit(p, nLevel, cmdAuthorize); // 4.0.22.850 TD 307758
				lFlags |= LIMIT_FILL_UP; //4.0.9.507
				if (_Module.m_server.IsSiteLimitByVolume()) // TD 309771
					lFlags |= LIMIT_BY_VOLUME;
			}
		}
		cmdAuthorize.lSyncNumber = 0;
		cmdAuthorize.lFlags = lFlags; //4.0.9.507

									  // turn on PumpTest Flags again
		SetPumpTestFlags(lPumpNumber, lFlagsTmp, cmdAuthorize, sRec);		 //4.0.25.1140 TD 412765


																			 //If the FORCE_IGNORE_VIT flag is a part of the nFlags parameter of the function
																			 //then we must assert it will reach the GCI with cmdAuthorize.nFlags
		if (bForceIgnoreVit)
			cmdAuthorize.lFlags |= FORCE_IGNORE_VIT;

		memset(cmdAuthorize.GradeAmountLimit, ' ', sizeof(cmdAuthorize.GradeAmountLimit));//4.0.22.506 67786
		if (!_Module.m_server.CheckForLoyaltyDiscount(lPumpNumber, cmdAuthorize))
		{

		}


		_Module.m_server.SetCommand(CMD_AUTHORIZE_LIMIT, m_lPosNumber, p + 1, &cmdAuthorize, sizeof(CMD_STRUCT_AUTHORIZE_LIMIT));

		if (LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("Pump Authorized2 by %d - Fill Up ", m_lPosNumber);
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
		}
	}
}

GPIRetCode CGPI::AuthotizePumpPrePayFlow(long p, CPumpTransact &trs, CPumpStatus &stat)
{
	GPIRetCode retCode = GPI_OK;

	if (_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_AUTH, TRS_LINK_PREPAY_NUMBER, &trs))
	{
		if (LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("Operator Authorize Pump %02d - Change to Authorize and Limit (Trs Link number %ld) ", p + 1, trs.m_lLinkNumber);//4.0.9.507
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
		}

		PRE_PAY_INFO info;
		long lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &trs, &info);
		if (lRtc)
		{
			// ERROR !!!!
			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Trs %d, TrsLink %d, TrsLinkOperation return %d", trs.m_lNumber, trs.m_nLinkFlags, lRtc);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
			}

			if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			{
				retCode = GPI_TRS_NOT_FOUND;
				trs.m_lRes = stat.GetRes();
				trs.m_lResExt = stat.GetResExt();
				trs.m_lMode = stat.GetMode();
				trs.UpdateTime();
				trs.m_lStatus = PAID;

				_Module.m_server.CentralUpdateTrs(p + 1, trs.m_lNumber, &trs, FALSE);
				stat.SetStatus(READY);
				_Module.m_server.m_cPumpArray[p].SetStat(stat);

				if (LOG_LOGIC_ERROR)
				{
					CString str;
					str.Format("Throwing Away PrePay TRS !!!!! : Pump %02d, Trs %d, TrsLink %d, TrsLinkOperation return %d", p + 1, trs.m_lNumber, trs.m_nLinkFlags, lRtc);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str, LOG_LEVEL_1);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, "And ... Release Pump ..!!!");
				}

				CMD_STRUCT_RELEASE_PUMP cmdInfo;
				cmdInfo.lRes = PRE_PAY_AT_POS;
				cmdInfo.lResExt = 0;
				_Module.m_server.SetCommand(CMD_RELEASE_PUMP, SERVER_POS_NUMBER, p + 1, &cmdInfo, sizeof(CMD_STRUCT_RELEASE_PUMP));

			}
			else
				retCode = GPI_FAILED_PREPAY_IO;

		}
		else
		{
			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Authorize Pump %02d (Auto PrePay), switch to WAIT_PROGRESS", p + 1);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
			}

			stat.SetStatus(READY);
			_Module.m_server.m_cPumpArray[p].SetStat(stat);

			trs.SetStatus(WAIT_PROGRESS);
			trs.UpdateTime();

			_Module.m_server.CentralUpdateTrs(p + 1, trs.m_lNumber, &trs, FALSE);

			CMD_STRUCT_AUTHORIZE_LIMIT cmdInfo;
			// had here CID_199910 Memset fill value of '0' - added C'tor to the struct instead

			cmdInfo.nLevel = info.nLevel;
			cmdInfo.lGradePrice = info.lGradePrice;

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("AuthorizeAndLimit Pump %02ld ,Trs Sync %ld, Level %d (Level limit %ld) , GradePrice %d, MaxValue %ld, MaxVolume %ld, Flags %x",
					p + 1,
					info.lSyncNumber,
					info.nLevel,
					_Module.m_server.GetLimit(info.nLevel, p + 1),
					info.lGradePrice,
					info.lMaxValue,
					info.lMaxVolume,
					info.lFlags);

				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, str);
			}

			cmdInfo.lMaxVolume = info.lMaxVolume;
			cmdInfo.lSyncNumber = info.lSyncNumber;
			cmdInfo.lFlags = info.lFlags;	//4.0.9.507
			cmdInfo.lMaxValue = info.lMaxValue;

			_Module.m_server.SetCommand(CMD_AUTHORIZE_LIMIT, SERVER_POS_NUMBER, p + 1, &cmdInfo, sizeof(CMD_STRUCT_AUTHORIZE_LIMIT));
		}
	}
	return retCode;
}

STDMETHODIMP CGPI::StopPump2(long lPumpNumber, short nGroup, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::StopPump2   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		StopPump2__(lPumpNumber, nGroup, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::StopPump2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "StopPump2", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


void CGPI::StopPump2__(long lPumpNumber, short nGroup, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	CString str;

	str.Format("CGPI::StopPump2__ Insert method Group = %ld", nGroup);
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (lPumpNumber)
	{
		if (PUMP_NOT_VALID(lPumpNumber))
			retCode = GPI_INVALID_PUMP_NUMBER;
	}


	if (retCode == GPI_OK)
	{
		BOOL			bStopAll = FALSE;
		long			p = 0;

		if (lPumpNumber == 0)
			bStopAll = TRUE;
		else
			p = lPumpNumber - 1;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if (bStopAll)
		{
			//4.0.22.500 - TD 69907
			// Send pump number 0 when controller supports stop all pump
			if (_Module.m_server.m_bControllerSupportStopAllPumps)
			{
				_Module.m_server.SetCommand(CMD_STOP_PUMP, m_lPosNumber, 0);
				if (LOG_BASIC_FLOW_CONTROL)
					_LOGMSG.LogMsg("Controller supports stop all pumps, Send StopPump with Pump number 0.");
			}
			else
			{
				for (int p = 0; p < _Module.m_server.GetLastValidPump(); p++)  //4.0.0.79
				{
					//Check that pump match the group
					if (nGroup)
					{
						//Get the group number from the misc field
						long lMisc = _Module.m_server.m_cPumpArray[p].GetStat().GetMisc();
						lMisc >>= 12; //xxxxXxxx --> 000xxxxX
						lMisc &= 3;	  //000xxxxX --> 0000000X
						if ((lMisc + 1) != nGroup)
							continue;
					}

					if (_Module.m_server.m_cPumpArray[p].CanStopPump())
					{
						_Module.m_server.SetCommand(CMD_STOP_PUMP, m_lPosNumber, p + 1);
					}
				}
			}

			//Now that we sent stop commands to all pumps, lock all the pumps.
			//This will cause all the pumps that have not stopped to be locked,
			//and all the stopped pumps to enter a lock pending state.
			if (!nGroup && _Module.m_server.GetLockOnStopAll())
			{
				BYTE byTmpRequiredMaintenanceProcess = _Module.m_server.m_cServerInfo.GetRequiredMaintenanceProcessFlags();

				//If server is in the middle of a lock process, or already
				//locked, do not attempt to lock again.
				if (byTmpRequiredMaintenanceProcess != MP_LOCK &&
					byTmpRequiredMaintenanceProcess != MP_EMERGENCY_LOCK &&
					_Module.m_server.m_cServerInfo.GetServerStatus() == UNLOCK) //4.0.28.500 - TD 242205
				{
					_Module.m_server.m_cServerInfo.SetRequiredMaintenanceProcessFlags(MP_EMERGENCY_LOCK);

					_Module.m_server.m_cServerInfo.SetInStopAllPumps(TRUE); //4.0.13.80

					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "Lock all Pump");
				}

				else
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "called for all pumps, but forecourt already locked");
			}
		}
		else
		{
			if (_Module.m_server.m_cPumpArray[p].CanStopPump())
				_Module.m_server.SetCommand(CMD_STOP_PUMP, m_lPosNumber, lPumpNumber);
			else if (_Module.m_server.m_cPumpArray[p].GetStat().GetStatus() == STOPED_BY_OPERATOR)
				retCode = GPI_PUMP_ALREADY_SUSPENDED;

			else if (_Module.m_server.m_cPumpArray[p].PumpStoppedByController())
				retCode = GPI_PUMP_STOPPED_BY_CONTROLLER;
		}
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"StopPump2",
				TRUE, retCode,
				FALSE, 0);


	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetPAPTrs3(long lPumpNumber, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPAPTrs3   start");
	// TODO: Add your implementation code here

	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPAPTrs3, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetPAPTrs3__(lPumpNumber, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetPAPTrs3")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPAPTrs3", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GetPAPTrs3__(long lPumpNumber, BSTR *pRec, long *pVal, long lFlags)
{
	GPIRetCode	retCode = GPI_OK;
	ASH2000_REC	sAsh2000Rec;
	BOOL		bFoundAsh2000Record = FALSE;
	long		lRtc = 0;
	long		lOlaRecordSize = 0;
	CPumpTransact		trs;		//4.0.9999.0 
	PAY_AT_PUMP_INFO	info;    //4.0.10.492

	if (!m_bInitDone)
	{
		retCode = GPI_MUST_INIT_FIRST;
	}
	TAG_OLA_REC5 tmpTagOlaRec;   //4.0.5.0
	memset(&tmpTagOlaRec, ' ', sizeof(TAG_OLA_REC5));  //4.0.5.0
	TAG_OLA_REC5_EX tagOlaRec;
	memset(&tagOlaRec, ' ', sizeof(TAG_OLA_REC5_EX));  //4.0.5.0

	if (retCode == GPI_OK)
	{
		if (lPumpNumber) // search by pump 
		{
			if (PUMP_NOT_VALID(lPumpNumber))
				retCode = GPI_INVALID_PUMP_NUMBER;
			else
			{
				retCode = GPI_NOT_SUPPORTED;
			}
		}
		else // search for any PAP concluded trs
		{
			trs.SetLink(TRS_LINK_OLA_NUMBER, 0);
			memset(&info, 0, sizeof(PAY_AT_PUMP_INFO));

			lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST, &trs, &info);

			if (lRtc == OK)
			{
				if (_Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags())
				{
					if (trs.m_lActiveShiftNumber  > _Module.m_server.GetActiveShiftNumber())
					{
						lRtc = ERR_NOT_FOUND;
					}
				}
			}


			if (lRtc == OK)
			{
				if (LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("GetPAPTrs3__ - First Record number %d, trs %d, Shift %d, (GradePrice = %ld, Nozzle %d, Value %ld)", info.CardSaleInfo.lIndexNumber, trs.m_lNumber, trs.m_lActiveShiftNumber, trs.m_lGradePrice, trs.m_nNozzle, trs.m_lRoundedValue);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
				}

				_Module.m_server.UnMaskAccountNumber(lPumpNumber, &info); // TD 332389

				l2a(info.CardSaleInfo.lIndexNumber, tagOlaRec.lIndexNumber, sizeof(tagOlaRec.lIndexNumber));
				tagOlaRec.cardData = info.CardSaleInfo.cardData;
				tagOlaRec.extraData = info.CardSaleInfo.extraData;
				tagOlaRec.extraData2 = info.CardSaleExtraData2;
				trs.GetTag(&tagOlaRec.tagTrs, 2);
				tagOlaRec.extraData3 = info.CardSaleExtraData3;

				_Module.m_server.m_cBaseReceipt->GetReceiptIndex((char*)tagOlaRec.cardData.sTrack2Data, &info, lPumpNumber);		//4.0.24.90 TD 120918

				if (_Module.m_server.GetOLAFlags(info.CardSaleExtraData2.sFlags, FL_CONNECTED_RECORD_IN_ASH2000_FILE))
				{
					long lQdxRetVal = 0;
					sAsh2000Rec.lIndexNumber = trs.m_lNumber;

					if (lQdxRetVal = _Module.m_server.IndexOperation(ASH2000_FILE, REC_READ, &sAsh2000Rec))//4.0.3.49
					{
						if (LOG_BASIC_FLOW_CONTROL)
						{
							DWORD dwThreadId = GetCurrentThreadId();
							CString tmpStr;
							tmpStr.Format("failed=%ld to Record %ld from file Ash 2000 info", lQdxRetVal, trs.m_lNumber);
							_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, tmpStr, LOG_LEVEL_1);
						}
					}
					else
					{
						bFoundAsh2000Record = TRUE;

						if (LOG_BASIC_FLOW_CONTROL)
						{
							CString tmpStr;
							tmpStr.Format("Found ash 2000 record trs=%ld", trs.m_lNumber);
							_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, tmpStr);
						}

					}

				}
			}
			else if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			{
				retCode = GPI_TRS_NOT_FOUND;
			}
			else
				retCode = GPI_FAILED_PREPAY_IO;

		}
	}

	lOlaRecordSize = (lFlags & GET_PAP_THREE_OLA_REC) ? sizeof(TAG_OLA_REC5) : sizeof(TAG_OLA_REC3);

	if (bFoundAsh2000Record)
	{
		//4.0.6.81 - start, change the "new" operation handler.
		try
		{
			BYTE *sTmpAllocationBuffer = new BYTE[lOlaRecordSize + ASH_2000_BUFFER_SIZE + 2];
			if (sTmpAllocationBuffer == NULL)
			{
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, "GetPAPTrs3, Fail to allocate buffer");
				retCode = GPI_TRS_NOT_FOUND;
			}
			else
			{
				memset(sTmpAllocationBuffer, 0, sizeof(lOlaRecordSize + ASH_2000_BUFFER_SIZE + 2));
				_Module.m_server.m_cCommonFunction.ConvertFromTagOlaRec5ExToTagOlaRec5(&tagOlaRec, &tmpTagOlaRec);

				//Copy the trs date and time to sTrsStartDateTime (which was sFiler17) //4.0.17.11 
				memset(&tmpTagOlaRec.extraData3.sTrsStartDateTime, ' ', sizeof(tmpTagOlaRec.extraData3.sTrsStartDateTime));
				memcpy(&tmpTagOlaRec.extraData3.sTrsStartDateTime, &tagOlaRec.tagTrs.sStartDateTime, sizeof(tagOlaRec.tagTrs.sStartDateTime));

				memcpy(sTmpAllocationBuffer, (BYTE *)&tmpTagOlaRec, lOlaRecordSize);
				memcpy(&sTmpAllocationBuffer[lOlaRecordSize], sAsh2000Rec.sData, sizeof(sAsh2000Rec.sData));

				CComBSTR strResult(lOlaRecordSize + ASH_2000_BUFFER_SIZE + 2, (LPCSTR)sTmpAllocationBuffer);
				delete[] sTmpAllocationBuffer;
				*pRec = strResult.Copy();
			}
		}
		catch (...)
		{
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPAPTrs3, Threw an unknow exception", LOG_LEVEL_1);
			retCode = GPI_TRS_NOT_FOUND;
		}
	}
	else
	{
		try
		{
			if ((lFlags & GET_PAP_XML_RECORD) && (!retCode))
			{
				char *sTmpXML = NULL; // TD 396348
				_Module.m_server.m_cCardSaleDataXML.CreateFleetPAPXML(trs, info, sTmpXML); //4.0.9999.0  //4.0.10.492
				CComBSTR strResult(sTmpXML);
				*pRec = strResult.Copy();
				if (sTmpXML)
					delete[] sTmpXML;
			}
			else
			{
				BYTE *sTmpAllocationBuffer = new BYTE[lOlaRecordSize + 2];
				if (sTmpAllocationBuffer == NULL)
				{
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "Fail to allocate buffer");
					retCode = GPI_TRS_NOT_FOUND;
				}
				else
				{
					_Module.m_server.m_cCommonFunction.ConvertFromTagOlaRec5ExToTagOlaRec5(&tagOlaRec, &tmpTagOlaRec);

					//Copy the trs date and time to sTrsStartDateTime (which was sFiler17) //4.0.17.11 
					memset(&tmpTagOlaRec.extraData3.sTrsStartDateTime, ' ', sizeof(tmpTagOlaRec.extraData3.sTrsStartDateTime));
					memcpy(&tmpTagOlaRec.extraData3.sTrsStartDateTime, &tagOlaRec.tagTrs.sStartDateTime, sizeof(tagOlaRec.tagTrs.sStartDateTime));

					Str2StrNull((BYTE *)&tmpTagOlaRec, lOlaRecordSize, sTmpAllocationBuffer);
					CComBSTR strResult(lOlaRecordSize + 2, (LPCSTR)sTmpAllocationBuffer);
					delete[]sTmpAllocationBuffer;
					*pRec = strResult.Copy();
					strResult.Empty();			//4.0.18.501 merge from 4.0.15.285
				}

			}

		}
		catch (...)
		{
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "Threw an unknow exception", LOG_LEVEL_1);
			retCode = GPI_TRS_NOT_FOUND;
		}
	}

	if ((retCode) && (retCode != GPI_TRS_NOT_FOUND))
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPAPTrs3",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE, lRtc);
	}

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::QueryPAPTrs2(long lPumpNumber, long lIndexNumber, short nFlags, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::QueryPAPTrs2   start");
	// TODO: Add your implementation code here
	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "QueryPAPTrs2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		QueryPAPTrs2__(lPumpNumber, lIndexNumber, nFlags, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::QueryPAPTrs2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "QueryPAPTrs2()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 



	return S_OK;
}

/******************************************************************************
Description:	Query pap Trs
Returns:
Parameters:   	long lPumpNumber, long lIndexNumber, short nFlags, BSTR *pRec, long *pVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Marik			28/10/2003   15:46		support three ola record
******************************************************************************/
void CGPI::QueryPAPTrs2__(long lPumpNumber, long lIndexNumber, short nFlags, BSTR *pRec, long *pVal)
{
	TAG_OLA_REC5	tmpTagOlaRec;
	TAG_OLA_REC5_EX	tagOlaRec;
	GPIRetCode		retCode = GPI_OK;

	long			lOlaRecordSize = (QUERY_PAP_THREE_OLA_REC & nFlags) ? sizeof(TAG_OLA_REC5) : sizeof(TAG_OLA_REC3); //4.0.5.0
	long			lRtc = 0;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	memset(&tmpTagOlaRec, ' ', sizeof(TAG_OLA_REC5));
	memset(&tagOlaRec, ' ', sizeof(TAG_OLA_REC5_EX));

	if (retCode == GPI_OK)
	{
		CPumpTransact		trs;
		PAY_AT_PUMP_INFO	info;

		trs.SetLink(TRS_LINK_OLA_NUMBER, lIndexNumber);
		memset(&info, 0, sizeof(PAY_AT_PUMP_INFO));

		switch (nFlags)
		{
		case GET_FIRST_INDEX:
		{
			lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST, &trs, &info);
			break;
		}
		case GET_DIRECT_INDEX:
		{
			lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &trs, &info);
			break;
		}
		case GET_NEXT_INDEX:
		{
			lRtc = _Module.m_server.TrsLinkOperation(REC_READ_NEXT, &trs, &info);
			break;
		}
		}

		if (lRtc == OK)
		{
			l2a(info.CardSaleInfo.lIndexNumber, tagOlaRec.lIndexNumber, sizeof(tagOlaRec.lIndexNumber));
			tagOlaRec.cardData = info.CardSaleInfo.cardData;
			tagOlaRec.extraData = info.CardSaleInfo.extraData;
			trs.GetTag(&tagOlaRec.tagTrs, 2);
			tagOlaRec.extraData2 = info.CardSaleExtraData2;

			tagOlaRec.extraData3 = info.CardSaleExtraData3;

		}
		else if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
		{
			retCode = GPI_TRS_NOT_FOUND;
		}
		else
			retCode = GPI_FAILED_IO;
	}

	BYTE *sTmpAllocationBuffer = new BYTE[lOlaRecordSize + 2];
	_Module.m_server.m_cCommonFunction.ConvertFromTagOlaRec5ExToTagOlaRec5(&tagOlaRec, &tmpTagOlaRec);
	Str2StrNull((BYTE *)&tmpTagOlaRec, lOlaRecordSize, sTmpAllocationBuffer);
	CComBSTR strResult(lOlaRecordSize + 2, (LPCSTR)sTmpAllocationBuffer);
	delete[]sTmpAllocationBuffer; //4.0.5.0
	*pRec = strResult.Copy();

	CString str;
	str.Format("query PAP len is =%ld", strResult.Length());
	_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);



	if ((retCode) && (retCode != GPI_TRS_NOT_FOUND))
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"QueryPAPTrs2",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE, lRtc);
	}

	if (pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CGPI::GetReceipt(long lPumpNumber, BSTR sTrack2, short nIndex, BSTR *sReceiptData, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetReceipt   start");
	// TODO: Add your implementation code here

	USES_CONVERSION;

	if (sTrack2 == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetReceipt, Throw COM Exception : E_POINTER", LOG_LEVEL_0);

		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetReceipt__(lPumpNumber, sTrack2, nIndex, sReceiptData, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetReceipt")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetReceipt", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}

// nIndex = 0  first , 1-second
void CGPI::GetReceipt__(long lPosNumber, BSTR sTrack2, short nIndex, BSTR *sReceiptData, long lFlags, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	long		lRtc = 0;
	short		lPosIndex = 0;
	DWORD		dwLastErr;
	int			iLen = 0;

	char szTrack2[ENCRYPTED_DATA_SIZE];
	memset(szTrack2, 0, sizeof(szTrack2));

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STARTUP);
		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());
		else
			retCode = GPI_SERVER_NOT_READY;
	}

	if (retCode == GPI_OK)
	{
		iLen = WideCharToMultiByte(CP_ACP, 0, sTrack2, -1, szTrack2, sizeof(szTrack2), NULL, NULL);

		PAY_AT_PUMP_INFO	PayAtPumpInfo;
		memset(&PayAtPumpInfo, ' ', sizeof(PAY_AT_PUMP_INFO));

		//call get card info to get the account number 
		//after we encrypt the account and this become the key to the QDX 

		if (iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GPI_NO_RECEIPT;
		}
		else
		{
			long lFound = RECEIPT_NOT_FOUND;

			CMD_STRUCT_FORMAT_RECEIPT cmdInfo;
			//TAG_OLA_REC9_EX	tagOlaRec; 
			TAG_OLA_REC13 tagOlaRec; //4.0.22.61	TD 75605 //4.0.27.46

			CStringA cTrack2(szTrack2, sizeof(szTrack2));
			lFound = _Module.m_server.BuildRequestReceiptInfo(cmdInfo, szTrack2, (long)nIndex, lFlags, lPosNumber, &tagOlaRec);

			if (lFound == RECEIPT_FOUND)
			{
				if (LOG_DETAIL_FLOW_CONTROL)
				{
					char szResult[MAX_FIELD_VAL] = { 0 };

					_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, (char*)szTrack2, iLen);

					CString	str;
					str.Format("Send Receipt to be format for lPos=%ld szTrack2=%s,  nIndex=%d retCode=%d ", lPosNumber, szResult, nIndex, retCode);

					_LOGMSG.LogMsg("CGPI", "GetReceipt", m_lPosNumber, LOG_POS, str);
				}

				_Module.m_server.SetCommand(CMD_FORMAT_RECEIPT, SERVER_POS_NUMBER, lPosNumber, &cmdInfo, sizeof(CMD_STRUCT_FORMAT_RECEIPT));

				//Get handle Pos receipt 
				CSingleLock slPosMap(&(_Module.m_server.m_csPosMap));
				slPosMap.Lock(INFINITE);
				CPosInfo tmpPos;

				_Module.m_server.m_cPosMap.Lookup(lPosNumber, tmpPos);
				slPosMap.Unlock();

				// if we get the event in 1 second we get receipt
				if (tmpPos.WaitForSingleObjectReceipt())
				{
					CComBSTR strResult;
					//Get handle Pos receipt 
					CSingleLock slPosMap(&(_Module.m_server.m_csPosMap));
					slPosMap.Lock(INFINITE);
					CPosInfo tmpPos;

					////////////////////////////////////////////////////////////////////////////////////////
					if (LOG_BASIC_FLOW_CONTROL)
					{//4.0.20.501 TD 60236						
						char szResult[MAX_FIELD_VAL] = { 0 };

						_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, szTrack2, iLen);

						CString str;
						// str.Format("Received Format receipt lPos=%ld, szTrack2=%s,Index=%ld,retCode=%d ", lPosNumber  ,szResult ,nIndex ,  retCode); 
						str.Format("Received Format receipt lPos=%ld, Index=%ld,retCode=%d ", lPosNumber, nIndex, retCode);

						_LOGMSG.LogMsg("CGPI", "GetReceipt", m_lPosNumber, LOG_POS, str);
					}
					/////////////////////////////////////////////////////////////////////////////////////////

					_Module.m_server.m_cPosMap.Lookup(lPosNumber, tmpPos);

					strResult = tmpPos.GetReceiptText();
					*sReceiptData = strResult.Copy();

					slPosMap.Unlock();
				}
				else
				{
					retCode = GPI_NO_RECEIPT;
				}
			}
			else
			{
				if (lFound == TOO_MANY_RECEIPTS)
					retCode = GPI_CANNOT_PRINT_RECEIPT;

				else
					retCode = GPI_NO_RECEIPT;
			}
		}
	}

	if (LOG_BASIC_FLOW_CONTROL)
	{
		char szResult[MAX_FIELD_VAL] = { 0 };

		_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, szTrack2, iLen);

		CString	str;
		str.Format("szTrack2=%s  lIndex=%ld  retCode=%d", szResult, nIndex, retCode);

		_LOGMSG.LogMsg("CGPI", "GetReceipt", m_lPosNumber, LOG_POS, str);
	}

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::ReceiptPrinted(BSTR sTrack2, short nIndex, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::ReceiptPrinted   start");
	// TODO: Add your implementation code here

	USES_CONVERSION;

	if (sTrack2 == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "ReceiptPrinted", m_lPosNumber, LOG_POS, "ReceiptPrinted, Throw COM Exception : E_POINTER", LOG_LEVEL_0);

		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		ReceiptPrinted__(sTrack2, nIndex, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::ReceiptPrinted")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "ReceiptPrinted()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}

void CGPI::ReceiptPrinted__(BSTR sTrack2, short nIndex, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	long		lRtc = 0;
	short		lPosIndex = nIndex;
	DWORD		dwLastErr;
	CXMLPointers cTmpPtr;

	char szTrack2[MAX_TABLE_NAME];
	memset(szTrack2, 0, sizeof(szTrack2));

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	if (retCode == GPI_OK)
	{
		CReceiptInfo	info;

		int iLen = WideCharToMultiByte(CP_ACP, 0, sTrack2, -1, szTrack2, sizeof(szTrack2), NULL, NULL);

		if (iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GPI_NO_RECEIPT;
		}
		else
		{
			info.Clear();
			info.m_HashedKey.SetString(szTrack2, _countof(szTrack2));
			lRtc = CSQLReceiptDB::GetInstance().GetFirstReceiptRecord(info);
			for (long lIndex = 1; (lIndex <= lPosIndex) && (lRtc == OK); lIndex++)
			{
				lRtc = CSQLReceiptDB::GetInstance().GetNextReceiptRecord(info.m_TrsIndex, info);
			}

			if ((lRtc == OK) && info.m_HashedKey == CStringA(szTrack2, _countof(szTrack2)))
			{
				info.m_NumberOfCopies++;
				lRtc = CSQLReceiptDB::GetInstance().UpdateReceipt(info);
			}
			else
				lRtc = ERR_NOT_FOUND;

		}

	}

	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString	str;
		str.Format("Update receipt copies, szTrack2=%.4s  lIndex=%ld  lRtc=%d", szTrack2 + TRACK_2_DATA_SIZE - 4, nIndex, lRtc);
		_LOGMSG.LogMsg("CGPI", "ReceiptPrinted", m_lPosNumber, LOG_POS, str);
	}

	if ((lRtc == ERR_NOT_FOUND) || (lRtc == Q_ERROR))
	{

		retCode = GPI_FAILED_IO;
	}

	*pVal = retCode;
}

STDMETHODIMP CGPI::GetOLAMemInfo(long lPumpNumber, short nIndex, long lFlags, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetOLAMemInfo   start");
	USES_CONVERSION;

#ifdef CATCH_GPI
	__try
	{
#endif
		GetOLAMemInfo__(lPumpNumber, nIndex, lFlags, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetOLAMemInfo")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetOLAMemInfo()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}

void CGPI::GetOLAMemInfo__(long lPumpNumber, short nIndex, long lFlags, BSTR *pRec, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	CMemPumpInfo Info;
	TAG_OLA_MEM_REC tmpTagMemTrs;
	memset(&tmpTagMemTrs, ' ', sizeof(TAG_OLA_MEM_REC));

	TAG_OLA_MEM_REC_EX tagMemTrs;
	memset(&tagMemTrs, ' ', sizeof(TAG_OLA_MEM_REC_EX));

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if ((lPumpNumber > _Module.m_server.GetMaxPumps()) || (lPumpNumber < 1))   //4.0.0.79
		retCode = GPI_INVALID_PUMP_NUMBER;
	else
	{
		if (_Module.m_server.m_cMemPumpArray[lPumpNumber - 1].GetMemPumpInfo(&Info, nIndex))
		{
			long lTotal, lTotalValue;
			//amram if pci then return the account
			if (_Module.m_server.m_cProtectedDataHandler.GetPCIActiveFlag() == PCI_FLAG_ON)
			{
				memset(Info.cCardData.sTrack2Data, ' ', sizeof(Info.cCardData.sTrack2Data));
				memcpy(Info.cCardData.sTrack2Data, Info.cCardData.sAccountNumber, sizeof(Info.cCardData.sAccountNumber));
			}

			lTotalValue = ConvertStringsToLong(Info.cCardSaleExtra.sTotalAmt, sizeof(Info.cCardSaleExtra.sTotalAmt), Info.cCardSaleExtra.sTotalAmt_Msb, sizeof(Info.cCardSaleExtra.sTotalAmt_Msb));

			long lWashPrice = a2l((unsigned char *)Info.cCardSaleExtra.SALES[0].sUnitPrice, sizeof(Info.cCardSaleExtra.SALES[0].sUnitPrice));
			long lWashDiscount = a2l((unsigned char *)Info.cCardSaleExtra.SALES[0].sDiscount, sizeof(Info.cCardSaleExtra.SALES[0].sDiscount));

			lTotal = lTotalValue + lWashDiscount * 10 - lWashPrice * 10;

			l2a(lTotal, (unsigned char *)tagMemTrs.sFuelTotal, sizeof(tagMemTrs.sFuelTotal));

			memcpy(&tagMemTrs.cardData, &Info.cCardData, sizeof(tagMemTrs.cardData));
			Info.cTrs2.GetTag(&tagMemTrs.tagTrs, 2);	//TD 38654

			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString	str;
				str.Format("Total=%ld ,lWashPrice=%ld  lWashDiscount=%ld sFuelTotal=%.8s", lTotalValue, lWashPrice, lWashDiscount, tagMemTrs.sFuelTotal);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
			}
		}
		else
			retCode = GPI_PARAM_NOT_FOUND;
	}

	BYTE sTmp[sizeof(TAG_OLA_MEM_REC) + 2];

	_Module.m_server.m_cCommonFunction.ConvertFromTagOlaMemRecExToTagOlaMemRec(&tagMemTrs, &tmpTagMemTrs);
	Str2StrNull((BYTE *)&tmpTagMemTrs, sizeof(TAG_OLA_MEM_REC), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	/////////////////////////////////////////////////////////////////////////////////////////
	if (LOG_BASIC_FLOW_CONTROL)
	{//4.0.20.501 TD 60236				
		char szResult[MAX_FIELD_VAL] = { 0 };
		_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, (char*)tagMemTrs.cardData.sTrack2Data, sizeof(tagMemTrs.cardData.sTrack2Data));

		CString	str;
		str.Format("After copy Index=%d, Data=%s, FuelAmount=%.8s , TotalAmount=%.9s", nIndex, szResult, tagMemTrs.sFuelTotal, tagMemTrs.sTotalAmount);

		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
	}
	/////////////////////////////////////////////////////////////////////////////////////////	

	*pVal = retCode;
}

STDMETHODIMP CGPI::GetPumpInfo3(long lPumpNumber, BSTR *pRec, long lFlags, long *pVal)
{
//	LOG_EXTERNALCALL("CGPI::GetPumpInfo3   start");
	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPumpInfo3, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}


#ifdef CATCH_GPI
	__try
	{
#endif
		GetPumpInfo3__(lPumpNumber, pRec, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetPumpInfo3")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPumpInfo3", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;

}

void CGPI::GetPumpInfo3__(long lPumpNumber, BSTR *pRec, long lFlags, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	//4.0.2.23  changed from else if to if because introduction of
	//ISPAKFinalizationAllowedWhenSystemDown has changed the flow logic - we might want
	//to check pump number validity even though server is not IDLE
	if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	BYTE* pDataBuff = NULL; // CID_184041 Deleting void pointer
	long lVersion = 0, lSize = 0;

	if (lFlags & STRUCT_IS_TAG_PUMP3)
	{
		lSize = sizeof(TAG_PUMP3_EX);

		lVersion = TAG_PUMP_VERSION_3;
	}

	else if (lFlags & STRUCT_IS_TAG_PUMP4)
	{
		lSize = sizeof(TAG_PUMP4_EX);
		lVersion = TAG_PUMP_VERSION_4;
	}

	else if (lFlags & STRUCT_IS_TAG_PUMP5)
	{
		lSize = sizeof(TAG_PUMP5_EX);
		lVersion = TAG_PUMP_VERSION_5;
	}

	else if (lFlags & STRUCT_IS_TAG_PUMP6)
	{
		lSize = sizeof(TAG_PUMP6_EX);
		lVersion = TAG_PUMP_VERSION_6;
	}
	else if (lFlags & STRUCT_IS_TAG_PUMP7)
	{
		lSize = sizeof(TAG_PUMP7_EX);
		lVersion = TAG_PUMP_VERSION_7;
	}

	else if (lFlags & STRUCT_IS_TAG_PUMP8)
	{
		lSize = sizeof(TAG_PUMP8_EX);
		lVersion = TAG_PUMP_VERSION_8;
	}

	else if (lFlags & STRUCT_IS_TAG_PUMP9)
	{
		lSize = sizeof(TAG_PUMP9);
		lVersion = TAG_PUMP_VERSION_9;
	}

	//This return code is imoprtant, because if we did not enter
	//any of the other conditions, pDataBuff is NULL
	else
	{
		retCode = GPI_NOT_SUPPORTED;
		lSize = sizeof(TAG_PUMP3_EX);
		lVersion = TAG_PUMP_VERSION_3;			    //4.0.5.23
	}

	pDataBuff = new BYTE[lSize]; //4.0.5.23

	if (pDataBuff)
		memset(pDataBuff, ' ', lSize);

	if ((retCode == GPI_OK) || (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED))
	{
		CString sLogMsg;
		unsigned long lPumpNumberWithOffset = lPumpNumber << 16;

		if (lFlags & START_TERMINAL_MSG)
		{
			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,
				0,
				GCI_EVENT_START_TERMINAL_MESSAGE | lPumpNumberWithOffset);

			sLogMsg.Format("Sent GCI_EVENT_START_TERMINAL_MESSAGE for pump %d", lPumpNumber);
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
		}

		else if (lFlags & END_TERMINAL_MSG)
		{
			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,
				0,
				GCI_EVENT_STOP_TERMINAL_MESSAGE | lPumpNumberWithOffset);

			sLogMsg.Format("Sent GCI_EVENT_STOP_TERMINAL_MESSAGE for pump %d", lPumpNumber);
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);

			_Module.m_server.m_cTerminalMessages.ClearMessages(lPumpNumber);
		}

		long p = lPumpNumber - 1;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if (pDataBuff)
			_Module.m_server.m_cPumpArray[p].GetTag(pDataBuff, lVersion);
	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(m_lPosNumber, // Src
				"IGPI",
				"GetPumpInfo3",
				TRUE,
				retCode,
				FALSE,
				0);
	}

	//Prepare outgoing data
	BYTE * sTmp = NULL;
	void * tmpPump = NULL;

	try
	{
		// CID_198785 Pointer to local outside scope
		TAG_PUMP3 tmpPump3;
		TAG_PUMP4 tmpPump4;
		TAG_PUMP5 tmpPump5;
		TAG_PUMP6 tmpPump6;
		TAG_PUMP7 tmpPump7;
		TAG_PUMP8 tmpPump8;
		TAG_PUMP9 * tmpPump9;

		if (lFlags & STRUCT_IS_TAG_PUMP3)
		{
			memset(&tmpPump3, ' ', sizeof(TAG_PUMP3));
			_Module.m_server.m_cCommonFunction.ConvertFromTagPump3ExToTagPump3((TAG_PUMP3_EX *)pDataBuff, &tmpPump3);
			lSize = MIN(lSize, sizeof(tmpPump3)); // CID_198852 Out-of-bounds access
			tmpPump = &tmpPump3;
		}

		else if (lFlags & STRUCT_IS_TAG_PUMP4)
		{
			memset(&tmpPump4, ' ', sizeof(TAG_PUMP4));
			_Module.m_server.m_cCommonFunction.ConvertFromTagPump4ExToTagPump4((TAG_PUMP4_EX *)pDataBuff, &tmpPump4);
			lSize = MIN(lSize, sizeof(tmpPump4)); // CID_198852 Out-of-bounds access
			tmpPump = &tmpPump4;
		}

		else if (lFlags & STRUCT_IS_TAG_PUMP5)
		{
			memset(&tmpPump5, ' ', sizeof(TAG_PUMP5));
			_Module.m_server.m_cCommonFunction.ConvertFromTagPump5ExToTagPump5((TAG_PUMP5_EX *)pDataBuff, &tmpPump5);
			lSize = MIN(lSize, sizeof(tmpPump5)); // CID_198852 Out-of-bounds access
			tmpPump = &tmpPump5;
		}
		else if (lFlags & STRUCT_IS_TAG_PUMP6)
		{
			memset(&tmpPump6, ' ', sizeof(TAG_PUMP6));
			_Module.m_server.m_cCommonFunction.ConvertFromTagPump6ExToTagPump6((TAG_PUMP6_EX *)pDataBuff, &tmpPump6);
			lSize = MIN(lSize, sizeof(tmpPump6)); // CID_198852 Out-of-bounds access
			tmpPump = &tmpPump6;
		}
		else if (lFlags & STRUCT_IS_TAG_PUMP7)
		{
			memset(&tmpPump7, ' ', sizeof(TAG_PUMP7));
			_Module.m_server.m_cCommonFunction.ConvertFromTagPump7ExToTagPump7((TAG_PUMP7_EX *)pDataBuff, &tmpPump7);
			lSize = MIN(lSize, sizeof(tmpPump7)); // CID_198852 Out-of-bounds access
			tmpPump = &tmpPump7;
		}
		else if (lFlags & STRUCT_IS_TAG_PUMP8)
		{
			memset(&tmpPump8, ' ', sizeof(TAG_PUMP8));
			_Module.m_server.m_cCommonFunction.ConvertFromTagPump8ExToTagPump8((TAG_PUMP8_EX *)pDataBuff, &tmpPump8);
			lSize = MIN(lSize, sizeof(tmpPump8)); // CID_198852 Out-of-bounds access
			tmpPump = &tmpPump8;
		}
		else if (lFlags & STRUCT_IS_TAG_PUMP9)
		{
			if (_Module.m_server.GetMaxGrade999())
			{
				//4.0.26.501 - TD 116227
				tmpPump9 = (TAG_PUMP9 *)pDataBuff;

				for (int i = 0; i < MAX_TRS_IN_LIST; i++)
				{
					if (a2l(tmpPump9->cTagPump4.pumpTrs[i].sGrade, sizeof(tmpPump9->cTagPump4.pumpTrs[i].sGrade)) > 0)
						_Module.m_server.GetExtGradeByGrade(tmpPump9->cTagPump4.pumpTrs[i].sGrade, sizeof(tmpPump9->cTagPump4.pumpTrs[i].sGrade), lPumpNumber);
				}
				lSize = MIN(lSize, sizeof(TAG_PUMP9)); // CID_198852 Out-of-bounds access
			}

			//Return new struct that includes the startDate
			tmpPump = pDataBuff;
		}

		else
		{
			memset(&tmpPump3, ' ', sizeof(TAG_PUMP3));
			_Module.m_server.m_cCommonFunction.ConvertFromTagPump3ExToTagPump3((TAG_PUMP3_EX *)pDataBuff, &tmpPump3);
			tmpPump = &tmpPump3;
		}

		sTmp = new BYTE[lSize + 2];

		//Check for successful allocation
		if (sTmp)
		{
			if (((retCode == GPI_OK) || (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED)) &&
				(pDataBuff != NULL))
				Str2StrNull((BYTE *)tmpPump, lSize, sTmp);

			else
			{
				memset(sTmp, ' ', lSize);
				sTmp[lSize] = sTmp[lSize + 1] = '\0';
			}

			CComBSTR strResult(lSize + 2, (LPCSTR)sTmp);


			*pRec = strResult.Copy();

			//free unneeded buffer
			delete[] sTmp;
		}

		//if allocation did not succeed, chances are we will not
		//be able to create the output BSTR, so we don't try
	}

	catch (...)
	{
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "SEVERE ERROR - ALLOCATION FAILURE", LOG_LEVEL_1);
		retCode = GPI_FAILED_IO;

		sTmp = NULL;
	}

	if (pDataBuff)
		delete[] pDataBuff;
	//Save the last time the POS call GPI method.
	_Module.m_server.m_cPosMap[m_lPosNumber].SetLastCmd(CMD_NULL, 0);

	if (pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CGPI::GetServiceFee(long lPump, long lTrsType, long lTrsNumber, BSTR stTime, long lFlags, long *pValue, long *pRetVal)
{
	LOG_EXTERNALCALL("CGPI::GetServiceFee   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		GetServiceFee__(lPump, lTrsType, lTrsNumber, stTime, lFlags, pValue, pRetVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetServiceFee")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetServiceFee()", lPump, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GetServiceFee__(long lPump, long lTrsType, long lTrsNumber, BSTR stTime, long lFlags, long *pValue, long *pRetVal)
{

	long retCode = GPI_OK;

	/////////////////
	_Module.m_server.GetServiceFeeInfo(lPump, lTrsType, lTrsNumber, stTime, lFlags, pValue, &retCode);

	switch (retCode)
	{
	case SERVICE_FEE_OK:
		retCode = GPI_OK;
		break;
	case SERVICE_FEE_NOT_VALID:
		retCode = GPI_SERVICE_FEE_NOT_VALID;
		break;
	case SERVICE_FEE_INVALID_PUMP_NUMBER:
		retCode = GPI_SERVICE_FEE_NOT_VALID;
		break;

	case SERVICE_FEE_TRS_NOT_FOUND:
		retCode = GPI_TRS_NOT_FOUND;
		break;
	case SERVICE_FEE_PARAM_NOT_FOUND:
		retCode = GPI_PARAM_NOT_FOUND;
		break;

	}


	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGPI",
				"GetServiceFee",
				TRUE, retCode,
				FALSE, 0);

	if (pRetVal != NULL)
		*pRetVal = retCode;
}

STDMETHODIMP CGPI::GetMinimalPumpsInfo3(BSTR *pRec, long lFlags, long *pVal)
{
	//LOG_EXTERNALCALL("CGPI::GetMinimalPumpsInfo3   start");
	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "GetMinimalPumpsInfo3", m_lPosNumber, LOG_POS, "GetMinimalPumpsInfo3, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}
#ifdef CATCH_GPI
	__try
	{
#endif
		GetMinimalPumpsInfo3__(pRec, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetMinimalPumpsInfo3")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetMinimalPumpsInfo3", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GetMinimalPumpsInfo3__(BSTR *pRec, long lFlags, long *pVal)
{
	GPIRetCode retCode = GPI_OK, cOriginalRetCode = GPI_OK;
	BOOL bNoTransactionsOnPumps = TRUE;

	long lActiveShift = 0;  //4.0.24.10

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;
	else if (SERVER_STATE_STARTUP || SERVER_NOT_IDLE)
	{
		if (_Module.m_server.IsQdexFilesAlarmOn(PUMP_STAT_FILE))
			retCode = GPI_SYSTEM_DOWN_QDEX_NOT_READY;

		else if (lFlags & ENABLE_PAK_PAYMENT_WHEN_PUMPSRV_IN_STARTUP)
		{
			if (PUMPSRV_IS_MASTER)
			{
				if (SERVER_STATE_STARTUP)
					cOriginalRetCode = GPI_CONTROLLER_NOT_AVAILABLE;

				else if (SERVER_NOT_IDLE)
					cOriginalRetCode = GPI_SERVER_NOT_READY;

				//SetPAKFinalizationAllowedWhenSystemDown will succeed
				//only if no unpaid transactions exist in the system.
				if ((!_HOST.m_lHydraActive) && (_Module.m_server.IsRedundancyByCL() != TRUE) && _Module.m_server.SetPAKFinalizationAllowedWhenSystemDown(!PUMPSRV_REPORT_ON_TRS_PAID))	//4.0.23.508 //4.0.23.509 96962
					retCode = GPI_SYSTEM_DOWN_PAK_ENABLED;
				else
					retCode = cOriginalRetCode;
			}
			else

			{
				_Module.m_server.SetPAKFinalizationAllowedWhenSystemDown(FALSE);

				if (SERVER_STATE_STARTUP)
					retCode = GPI_CONTROLLER_NOT_AVAILABLE;
				else
					retCode = GPI_SERVER_NOT_READY;
			}
		}
		else
		{
			_Module.m_server.SetPAKFinalizationAllowedWhenSystemDown(FALSE);

			if (SERVER_STATE_STARTUP)
				retCode = GPI_CONTROLLER_NOT_AVAILABLE;
			else
				retCode = GPI_SERVER_NOT_READY;
		}
	}

	//System is back to normal, so we don't want to allow any abnormal
	//flow we may have allowed when system was down
	else
		_Module.m_server.SetPAKFinalizationAllowedWhenSystemDown(FALSE);

	if (lFlags & USE_TAG_MINIMAL_ALL_PUMPS_5)
	{
		TAG_MINIMAL_ALL_PUMPS_5 info;
		memset(&info, ' ', sizeof(info));

		TAG_MINIMAL_PUMP_5 sMinimalPump33To64[MAX_PUMPS];
		memset(sMinimalPump33To64, ' ', MAX_PUMPS * sizeof(TAG_MINIMAL_PUMP_5));

		if ((retCode == GPI_OK) || (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED))
		{
			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			int nPumps = 0;
			nPumps = MAX_PUMPS_64;	//TD 37244

			i2a((short int)nPumps,
				info.sNumberOfPumps,
				(unsigned int)sizeof(info.sNumberOfPumps));

			if (_Module.m_server.GetAlarmSrvMode() == ALRMSRV_DIRECT_ACTIVE)
			{
				if (_Module.m_server.m_cAlarm.m_lAlarmPumpMap[0])
				{
					l2a(ALARM_SRV_CODE,
						info.sSysError,
						(unsigned int)sizeof(info.sSysError));
				}
				else
				{
					l2a(0,
						info.sSysError,
						(unsigned int)sizeof(info.sSysError));
				}

				if (_Module.m_server.m_cAlarm.m_lAlarmSpecialGroupMap[ALARM_GROUP_OLA])
					info.sOLAAlarmGroup = '1';


				if (_Module.m_server.m_cAlarm.m_lAlarmSpecialGroupMap[ALARM_GROUP_TANK])
					info.sTankAlarmGroup = '1';

				// if there is any misc error or warning we will report on sysAlarm
				l2a(0, info.sSysMisc, (unsigned int)sizeof(info.sSysMisc));
			}
			else
			{
				l2a(_Module.m_server.m_lSysError,
					info.sSysError,
					(unsigned int)sizeof(info.sSysError));

				if (_Module.m_server.m_lSysMisc)
					l2a(_Module.m_server.m_lSysMisc,
						info.sSysMisc,
						(unsigned int)sizeof(info.sSysMisc));
				else if (_Module.m_server.m_lDiskWarning)
					l2a(_Module.m_server.m_lDiskWarning,
						info.sSysMisc,
						(unsigned int)sizeof(info.sSysMisc));
				else
					l2a(0,
						info.sSysMisc,
						(unsigned int)sizeof(info.sSysMisc));
			}

			if (_Module.m_server.m_cServerInfo.GetLockByUpsFlags())
				info.sServerStatus = '4';
			else if (_Module.m_server.m_cServerInfo.GetServerReservation() & MAINTENANCE)
				info.sServerStatus = '1';
			else if (_Module.m_server.m_cServerInfo.GetRequiredMaintenanceProcessFlags() == MP_EMERGENCY_LOCK)
				info.sServerStatus = '2';   //4.0.5.24

			if (_Module.m_server.m_cServerInfo.GetServerReservation() & DATABASE_MAINTENANCE)
			{
				for (int i = 0; i < MAX_PUMPS; i++)
				{
					l2a(RES_EXT_MAINTENANCE_DATABASE, info.minimalPump[i].sResExt, sizeof(info.minimalPump[i].sResExt));
				}
			}

			memset(info.sConvertLayerVer, ' ', sizeof(info.sConvertLayerVer));
			memcpy(info.sConvertLayerVer,
				(LPCTSTR)_Module.m_server.m_strConvertLayerVersion,
				MIN(sizeof(info.sConvertLayerVer), _Module.m_server.m_strConvertLayerVersion.GetLength()));

			memset(info.sPumpSrvVer, ' ', sizeof(info.sPumpSrvVer));
			memcpy(info.sPumpSrvVer,
				(LPCTSTR)_Module.m_server.m_strPumpSrvVersion,
				MIN(sizeof(info.sPumpSrvVer), _Module.m_server.m_strPumpSrvVersion.GetLength()));

			if (_Module.m_server.m_cCarWash.GetCarWashStatus())
				info.sCarWashStatus = '0';	// 0ff line  //4.0.9.504
			else
				info.sCarWashStatus = '1';   //on line //4.0.9.504


			if (PRICE_DISPLAY_OFFLINE == _Module.m_server.m_cServerInfo.GetPriceDisplayStatus())  //TD 426985
				info.sPriceDisplayPanelStatus = '0';	// 0ff 
			else	if (PRICE_DISPLAY_IDLE == _Module.m_server.m_cServerInfo.GetPriceDisplayStatus())
				info.sPriceDisplayPanelStatus = '1';
			else	if (PRICE_DISPLAY_BLANKED == _Module.m_server.m_cServerInfo.GetPriceDisplayStatus())
				info.sPriceDisplayPanelStatus = '2';

			if (_Module.m_server.dwPosRequestReport & REPORT_TYPE_ULLAGE)
				info.sUllageReportDone = '1';

			if (_Module.m_server.dwPosRequestReport & REPORT_TYPE_DERIVERY)
				info.sDeliveryReportDone = '1';

			for (int i = 0; i < MAX_PUMPS; i++)
			{
				_Module.m_server.m_cPumpArray[i].GetTagMinimal(&(info.minimalPump[i]), 5); //TD 37244

				if (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED)
				{
					if (a2l(info.minimalPump[i].sNumberOfUnPaidTrs, sizeof((info.minimalPump[i].sNumberOfUnPaidTrs))) ||
						a2l(info.minimalPump[i].sNumberOfOnOfferOrAcceptedTrs, sizeof((info.minimalPump[i].sNumberOfOnOfferOrAcceptedTrs))))
						bNoTransactionsOnPumps = FALSE;
				}
			}

			//This flags preserves backwards compatibility -if this flag is set
			//we add extra data for 64 pumps at the END of the output buffer
			//(see declaration of TAG_MINIMAL_ALL_PUMPS_5).
			//Otherwise, the buffer remains the same.
			if ((lFlags & USE_TAG_MINIMAL_ALL_PUMPS_5) && //TD 37244
				(_Module.m_server.m_cPumpArray.GetSize() > MAX_PUMPS))
			{
				for (int j = 0, i = MAX_PUMPS; i < MAX_PUMPS_64; i++, j++)
				{
					_Module.m_server.m_cPumpArray[i].GetTagMinimal(&(sMinimalPump33To64[j]), 5); //TD 37244

					if (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED)
					{
						if (a2l(sMinimalPump33To64[j].sNumberOfUnPaidTrs, sizeof((sMinimalPump33To64[j].sNumberOfUnPaidTrs))) ||
							a2l(sMinimalPump33To64[j].sNumberOfOnOfferOrAcceptedTrs, sizeof((sMinimalPump33To64[j].sNumberOfOnOfferOrAcceptedTrs))))
							bNoTransactionsOnPumps = FALSE;
					}
				}
			}

			if (_Module.m_server.GetSeverityAlarmsCount() > 0)
				info.sAlarmsSeverityOn = '1';
			else
				info.sAlarmsSeverityOn = '0';

			if (_Module.m_server.IsMasterComputer())
				info.byIsMaster = '1';
			else
				info.byIsMaster = '0';

			//Put the active shift number
			lActiveShift = _Module.m_server.GetActiveShiftNumber(); //4.0.24.10
			l2a(lActiveShift, info.sActiveShiftNumber, (unsigned int)sizeof(info.sActiveShiftNumber));

			//Put the server lock status: STARTED ('0') , WAIT_LOCK ('1'), LOCKED ('2') etc'... //4.0.21.150
			LockStatus eLockStatus = _Module.m_server.GetLockStatus();
			l2a(eLockStatus, info.byServerLockStatus, (unsigned int)sizeof(info.byServerLockStatus));

			//4.0.26.508 147183 start
			l2a(_Module.m_server.GetPriceChangeResponse(), info.byPriceChangeStatus, sizeof(info.byPriceChangeStatus));
			l2a(_Module.m_server.GetPriceChangePOSMsg(), info.byPriceChangePOSMsg, sizeof(info.byPriceChangePOSMsg)); //4.0.27.42
			long lBatchId = 0;
			lBatchId = _Module.m_server.GetPriceChangeBatchNumber();
			l2a(lBatchId, info.sPriceChangeBatchNumber, sizeof(info.sPriceChangeBatchNumber));
			//4.0.26.508 147183 end

			long lPOSNumber;
			CPosInfo tmpPos;
			POSITION position = _Module.m_server.m_cPosMap.GetStartPosition();

			while (position)
			{
				_Module.m_server.m_cPosMap.GetNextAssoc(position, lPOSNumber, tmpPos);

				if (tmpPos.GetOLAEvent())
				{
					memset(info.sOLACode, ' ', sizeof(info.sOLACode));
					l2a(tmpPos.GetOLAEvent(), info.sOLACode, (unsigned int)sizeof(info.sOLACode));
					l2a(lPOSNumber, info.sTerminalNumber, (unsigned int)sizeof(info.sTerminalNumber));
					break;
				}
			}
		}

		if (bNoTransactionsOnPumps && (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED))
			retCode = cOriginalRetCode;

		if (retCode != GPI_OK)
		{
			if (LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					m_lPosNumber, // Src
					"IGPI",
					"GetMinimalPumpsInfo3",
					TRUE, retCode,
					FALSE, 0);
		}


		TAG_MINIMAL_ALL_PUMPS_5 info64;

		//Copy all info up to new pump array
		memcpy(&info64, &info, offsetof(TAG_MINIMAL_ALL_PUMPS_5, minimalPump2));

		//Copy additional pump info, which may be empty if the flag 
		//was set by mistake
		memcpy((char*)info64.minimalPump2,
			(char*)sMinimalPump33To64,
			sizeof(sMinimalPump33To64));

		//Copy remaining TAG_MINIMAL_ALL_PUMPS_5 info
		memcpy((char*)info64.sPumpSrvVer,
			(char*)info.sPumpSrvVer,
			sizeof(TAG_MINIMAL_ALL_PUMPS_5) - offsetof(TAG_MINIMAL_ALL_PUMPS_5, sPumpSrvVer));

		info64.sServerStatus = info.sServerStatus;
		info64.sTankAlarmGroup = info.sTankAlarmGroup;		//4.0.2.23
		info64.sOLAAlarmGroup = info.sOLAAlarmGroup;		//4.0.3.55

		BYTE sTmp[sizeof(TAG_MINIMAL_ALL_PUMPS_5) + 2];
		Str2StrNull((BYTE *)&info64, sizeof(TAG_MINIMAL_ALL_PUMPS_5), sTmp);
		CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
		*pRec = strResult.Copy();
	}
	else	//USE_TAG_MINIMAL_ALL_PUMPS_4 or less
	{
		TAG_MINIMAL_ALL_PUMPS_3 info;
		memset(&info, ' ', sizeof(info));

		TAG_MINIMAL_PUMP_3 sMinimalPump33To64[MAX_PUMPS];
		memset(sMinimalPump33To64, ' ', MAX_PUMPS * sizeof(TAG_MINIMAL_PUMP_3));

		if ((retCode == GPI_OK) || (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED))
		{
			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			int nPumps = 0;
			if ((lFlags & USE_TAG_MINIMAL_ALL_PUMPS_4) || (lFlags & USE_TAG_MINIMAL_ALL_PUMPS_5)) //TD 37244
				nPumps = MAX_PUMPS_64;
			else
				nPumps = MAX_PUMPS;

			i2a((short int)nPumps,
				info.sNumberOfPumps,
				(unsigned int)sizeof(info.sNumberOfPumps));

			if (_Module.m_server.GetAlarmSrvMode() == ALRMSRV_DIRECT_ACTIVE)
			{
				if (_Module.m_server.m_cAlarm.m_lAlarmPumpMap[0])
				{
					l2a(ALARM_SRV_CODE,
						info.sSysError,
						(unsigned int)sizeof(info.sSysError));
				}
				else
				{
					l2a(0,
						info.sSysError,
						(unsigned int)sizeof(info.sSysError));
				}

				if (_Module.m_server.m_cAlarm.m_lAlarmSpecialGroupMap[ALARM_GROUP_OLA])
					info.sOLAAlarmGroup = '1';

				if (_Module.m_server.m_cAlarm.m_lAlarmSpecialGroupMap[ALARM_GROUP_TANK])
					info.sTankAlarmGroup = '1';

				// if there is any misc error or warning we will report on sysAlarm
				l2a(0, info.sSysMisc, (unsigned int)sizeof(info.sSysMisc));
			}
			else
			{
				l2a(_Module.m_server.m_lSysError,
					info.sSysError,
					(unsigned int)sizeof(info.sSysError));

				if (_Module.m_server.m_lSysMisc)
					l2a(_Module.m_server.m_lSysMisc,
						info.sSysMisc,
						(unsigned int)sizeof(info.sSysMisc));
				else if (_Module.m_server.m_lDiskWarning)
					l2a(_Module.m_server.m_lDiskWarning,
						info.sSysMisc,
						(unsigned int)sizeof(info.sSysMisc));
				else
					l2a(0,
						info.sSysMisc,
						(unsigned int)sizeof(info.sSysMisc));
			}

			if (_Module.m_server.m_cServerInfo.GetLockByUpsFlags())
				info.sServerStatus = '4';
			else if (_Module.m_server.m_cServerInfo.GetServerReservation() & MAINTENANCE)
				info.sServerStatus = '1';
			else if (_Module.m_server.m_cServerInfo.GetRequiredMaintenanceProcessFlags() == MP_EMERGENCY_LOCK)
				info.sServerStatus = '2';   //4.0.5.24

			if (_Module.m_server.m_cServerInfo.GetServerReservation() & DATABASE_MAINTENANCE)
			{
				for (int i = 0; i < MAX_PUMPS; i++)
				{
					l2a(RES_EXT_MAINTENANCE_DATABASE, info.minimalPump[i].sResExt, sizeof(info.minimalPump[i].sResExt));
				}
			}

			memset(info.sConvertLayerVer, ' ', sizeof(info.sConvertLayerVer));
			memcpy(info.sConvertLayerVer,
				(LPCTSTR)_Module.m_server.m_strConvertLayerVersion,
				MIN(sizeof(info.sConvertLayerVer), _Module.m_server.m_strConvertLayerVersion.GetLength()));

			memset(info.sPumpSrvVer, ' ', sizeof(info.sPumpSrvVer));
			memcpy(info.sPumpSrvVer,
				(LPCTSTR)_Module.m_server.m_strPumpSrvVersion,
				MIN(sizeof(info.sPumpSrvVer), _Module.m_server.m_strPumpSrvVersion.GetLength()));

			if (_Module.m_server.m_cCarWash.GetCarWashStatus())
				info.sCarWashStatus = '0';	// 0ff line  //4.0.9.504
			else
				info.sCarWashStatus = '1';   //on line //4.0.9.504

			if (_Module.m_server.dwPosRequestReport & REPORT_TYPE_ULLAGE)
				info.sUllageReportDone = '1';

			if (_Module.m_server.dwPosRequestReport & REPORT_TYPE_DERIVERY)
				info.sDeliveryReportDone = '1';


			for (int i = 0; i < MAX_PUMPS; i++)
			{
				_Module.m_server.m_cPumpArray[i].GetTagMinimal(&(info.minimalPump[i]), 3);

				if (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED)
				{
					if (a2l(info.minimalPump[i].sNumberOfUnPaidTrs, sizeof((info.minimalPump[i].sNumberOfUnPaidTrs))) ||
						a2l(info.minimalPump[i].sNumberOfOnOfferOrAcceptedTrs, sizeof((info.minimalPump[i].sNumberOfOnOfferOrAcceptedTrs))))
						bNoTransactionsOnPumps = FALSE;
				}

			}

			//This flags preserves backwards compatibility -if this flag is set
			//we add extra data for 64 pumps at the END of the output buffer
			//(see declaration of TAG_MINIMAL_ALL_PUMPS_4).
			//Otherwise, the buffer remains the same.
			if ((lFlags & USE_TAG_MINIMAL_ALL_PUMPS_4) &&
				(_Module.m_server.m_cPumpArray.GetSize() > MAX_PUMPS))
			{
				for (int j = 0, i = MAX_PUMPS; i < MAX_PUMPS_64; i++, j++)
				{
					_Module.m_server.m_cPumpArray[i].GetTagMinimal(&(sMinimalPump33To64[j]), 3);

					if (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED)
					{
						if (a2l(sMinimalPump33To64[j].sNumberOfUnPaidTrs, sizeof((sMinimalPump33To64[j].sNumberOfUnPaidTrs))) ||
							a2l(sMinimalPump33To64[j].sNumberOfOnOfferOrAcceptedTrs, sizeof((sMinimalPump33To64[j].sNumberOfOnOfferOrAcceptedTrs))))
							bNoTransactionsOnPumps = FALSE;
					}
				}
			}

			if (_Module.m_server.GetSeverityAlarmsCount() > 0)
				info.sAlarmsSeverityOn = '1';
			else
				info.sAlarmsSeverityOn = '0';


			if (_Module.m_server.IsMasterComputer())
				info.byIsMaster = '1';
			else
				info.byIsMaster = '0';

			long lPOSNumber;
			CPosInfo tmpPos;
			POSITION position = _Module.m_server.m_cPosMap.GetStartPosition();

			while (position)
			{
				_Module.m_server.m_cPosMap.GetNextAssoc(position, lPOSNumber, tmpPos);

				if (tmpPos.GetOLAEvent())
				{
					memset(info.sOLACode, ' ', sizeof(info.sOLACode));
					l2a(tmpPos.GetOLAEvent(), info.sOLACode, (unsigned int)sizeof(info.sOLACode));
					l2a(lPOSNumber, info.sTerminalNumber, (unsigned int)sizeof(info.sTerminalNumber));
					break;
				}
			}
		}

		if (bNoTransactionsOnPumps && (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED))
			retCode = cOriginalRetCode;

		if (retCode != GPI_OK)
		{
			if (LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					m_lPosNumber, // Src
					"IGPI",
					"GetMinimalPumpsInfo3",
					TRUE, retCode,
					FALSE, 0);
		}

		if (lFlags & USE_TAG_MINIMAL_ALL_PUMPS_4)
		{
			TAG_MINIMAL_ALL_PUMPS_4 info64;

			//Copy all info up to new pump array
			memcpy(&info64, &info, offsetof(TAG_MINIMAL_ALL_PUMPS_4, minimalPump2));

			//Copy additional pump info, which may be empty if the flag 
			//was set by mistake
			memcpy((char*)info64.minimalPump2,
				(char*)sMinimalPump33To64,
				sizeof(sMinimalPump33To64));

			//Copy remaining TAG_MINIMAL_ALL_PUMPS_3 info
			memcpy((char*)info64.sPumpSrvVer,
				(char*)info.sPumpSrvVer,
				sizeof(TAG_MINIMAL_ALL_PUMPS_3) - offsetof(TAG_MINIMAL_ALL_PUMPS_3, sPumpSrvVer));

			info64.sServerStatus = info.sServerStatus;
			info64.sTankAlarmGroup = info.sTankAlarmGroup;		//4.0.2.23
			info64.sOLAAlarmGroup = info.sOLAAlarmGroup;		//4.0.3.55

			BYTE sTmp[sizeof(TAG_MINIMAL_ALL_PUMPS_4) + 2];
			Str2StrNull((BYTE *)&info64, sizeof(TAG_MINIMAL_ALL_PUMPS_4), sTmp);
			CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
			*pRec = strResult.Copy();
		}
		else
		{
			BYTE sTmp[sizeof(TAG_MINIMAL_ALL_PUMPS_3) + 2];
			Str2StrNull((BYTE *)&info, sizeof(TAG_MINIMAL_ALL_PUMPS_3), sTmp);
			CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
			*pRec = strResult.Copy();

		}
	}

	if (SERVER_STATE_EX_SHOW_GPI_MSG_CONVERT_LAYER_GONE)				//4.0.23.510 TD 97894
	{
		retCode = GPI_SERVER_NOT_READY;
	}

	//Save the last time the POS call GPI method.
	_Module.m_server.m_cPosMap[m_lPosNumber].SetLastCmd(CMD_NULL, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetGradePrices(short lGradeNumber, long lFlags, long *lCashFullPrice, long *lCashSelfPrice, long *lCreditFullPrice, long *lCreditSelfPrice, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetGradePrices   start");
	USES_CONVERSION;

#ifdef CATCH_GPI
	__try
	{
#endif
		GetGradePrices__(lGradeNumber, lFlags, lCashFullPrice, lCashSelfPrice, lCreditFullPrice, lCreditSelfPrice, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetGradePrices")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetGradePrices", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}

void CGPI::GetGradePrices__(short nGrade, long lFlags, long *lCashFullPrice, long *lCashSelfPrice, long *lCreditFullPrice, long *lCreditSelfPrice, long *pVal)
{
	GPIRetCode retCode = GPI_OK;
	CString str = "Grades\\Grade";
	CString sNumber;
	DWORD dwPriceChangeDone;

	//4.0.26.501 - TD 116227
	//Make extended grade conversion if needed
	if ((nGrade > 0) && _Module.m_server.GetMaxGrade999() && !(lFlags & IGNORE_EXTENDED_GRADE_ID_CONVERSION))
	{
		if (!_Module.m_server.GetGradeByExtGrade(nGrade, (long *)&nGrade))
			retCode = GPI_INVALID_GRADE_NUMBER;
	}

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (GRADE_NOT_VALID(nGrade))
		retCode = GPI_INVALID_GRADE_NUMBER;

	if (retCode == GPI_OK)
	{
		sNumber.Format("%02d", nGrade);
		str += sNumber;

		dwPriceChangeDone = _Module.m_server.m_cPriceChangeFlags.GetPriceChangeStatus();  //4.0.8.501

		if (dwPriceChangeDone)
		{
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCash_SelfService", (DWORD *)lCashSelfPrice, (DWORD)100, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCredit_SelfService", (DWORD *)lCreditSelfPrice, (DWORD)100, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCash_FullService", (DWORD *)lCashFullPrice, (DWORD)100, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCredit_FullService", (DWORD *)lCreditFullPrice, (DWORD)100, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
		}
		else
		{
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Old_PriceCash_SelfService", (DWORD *)lCashSelfPrice, (DWORD)100, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Old_PriceCredit_SelfService", (DWORD *)lCreditSelfPrice, (DWORD)100, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Old_PriceCash_FullService", (DWORD *)lCashFullPrice, (DWORD)100, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Old_PriceCredit_FullService", (DWORD *)lCreditFullPrice, (DWORD)100, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
		}

		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString	str;
			str.Format("GetGradePrices, CashSelfPrice=[%ld] CreditSelfPrice=[%ld] CashFullPrice[%ld] CreditFullPrice[%ld]", *lCashSelfPrice, *lCreditSelfPrice, *lCashFullPrice, *lCreditFullPrice);
			_LOGMSG.LogMsg("CGPI", "GetGradePrices", m_lPosNumber, LOG_POS, str);
		}

	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetGradePrices",
				TRUE, retCode,
				FALSE, 0);
	}

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetLevelPrice(long lPumpNumber, short nGrade, short nLevel, long lServiceMode, long lFlags, long *lPrice, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetLevelPrice   start");
	USES_CONVERSION;

#ifdef CATCH_GPI
	__try
	{
#endif
		GetLevelPrice__(lPumpNumber, nGrade, nLevel, lServiceMode, lFlags, lPrice, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetLevelPrice")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetLevelPrice()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}


void CGPI::GetLevelPrice__(long lPumpNumber, short nGrade, short nLevel, long lServiceMode, long lFlags, long *lPrice, long *pVal)
{
	GPIRetCode retCode = GPI_OK;
	CString PathStr = "Grades\\Grade";
	CString FieldStr;
	CString sNumber;
	DWORD dwPriceChangeDone;

	//4.0.26.501 - TD 116227
	//Make extended grade conversion if needed
	if ((nGrade > 0) && _Module.m_server.GetMaxGrade999() && !(lFlags & IGNORE_EXTENDED_GRADE_ID_CONVERSION))		//4.0.26.508 - TD 211018
	{
		if (!_Module.m_server.GetGradeByExtGrade(nGrade, (long *)&nGrade))
			retCode = GPI_INVALID_GRADE_NUMBER;
	}

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	else if (GRADE_NOT_VALID(nGrade))
		retCode = GPI_INVALID_GRADE_NUMBER;

	else if (LEVEL_NOT_VALID(nLevel))
		retCode = GPI_INVALID_LEVEL_NUMBER;

	else if (SERVICE_MODE_NOT_VALID(lServiceMode))
		retCode = GPI_INVALID_SERVICE_MODE_NUMBER;

	if (retCode == GPI_OK)
	{
		sNumber.Format("%02d", nGrade);
		PathStr += sNumber;

		if (lPumpNumber > 0)
			lServiceMode = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetServiceMode();

		dwPriceChangeDone = _Module.m_server.m_cPriceChangeFlags.GetPriceChangeStatus();  //4.0.8.501

		if (dwPriceChangeDone)
			FieldStr = "New_";
		else
			FieldStr = "Old_";

		if (nLevel == PRICE_LEVEL_CASH)
		{
			if (lServiceMode == SELF_SERVICE_MODE)
				FieldStr += "PriceCash_SelfService";
			else if (lServiceMode == FULL_SERVICE_MODE)
				FieldStr += "PriceCash_FullService";
		}
		else if (nLevel == PRICE_LEVEL_CREDIT)
		{
			if (lServiceMode == SELF_SERVICE_MODE)
				FieldStr += "PriceCredit_SelfService";
			else if (lServiceMode == FULL_SERVICE_MODE)
				FieldStr += "PriceCredit_FullService";
		}

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)PathStr), (char *)((LPCTSTR)FieldStr), (DWORD *)lPrice, (DWORD)100, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);  //4.0.8.503

		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString	str;
			str.Format("GetLevelPrice, Price=[%ld]", *lPrice);
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
		}
	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetLevelPrice",
				TRUE, retCode,
				FALSE, 0);
	}

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetOLAMemInfo2(long lPumpNumber, short *nIndex, long lFlags, long lVal, BSTR sVal, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetOLAMemInfo2   start");

	USES_CONVERSION;

#ifdef CATCH_GPI
	__try
	{
#endif
		GetOLAMemInfo2__(lPumpNumber, nIndex, lFlags, lVal, sVal, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetOLAMemInfo2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetOLAMemInfo2()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}

void CGPI::GetOLAMemInfo2__(long lPumpNumber, short *nIndex, long lFlags, long lVal, BSTR sVal, BSTR *pRec, long *pVal, const long lFlags2)
{
	GPIRetCode	retCode = GPI_OK;
	CMemPumpInfo Info;
	TAG_OLA_MEM_REC tmpTagMemTrs;
	TAG_OLA_MEM_REC_EX tagMemTrs;
	short nTmpIndex = *nIndex;
	BOOL bFoundRec = FALSE;
	BOOL bDoNotPassDeclineRecord = (lFlags & OLA_MEM_WITHOUT_CANCEL) ? TRUE : FALSE;
	long lTmpFlags = lFlags & 0xEFFFFF;


	CTime cCurTime = CTime::GetCurrentTime();

	memset(&tmpTagMemTrs, ' ', sizeof(TAG_OLA_MEM_REC));
	memset(&tagMemTrs, ' ', sizeof(TAG_OLA_MEM_REC_EX));

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if ((lPumpNumber > _Module.m_server.GetMaxPumps()) || (lPumpNumber < 1))   //4.0.0.79
		retCode = GPI_INVALID_PUMP_NUMBER;

	else
	{
		if ((!nTmpIndex) && _Module.m_server.GetIFSFTerminalSrvInUse())
		{
			_Module.m_server.m_cCommonFunction.GetTransactionList4IFSFTerminal(lPumpNumber, m_lPosNumber, GET_RECEIPT_TRS_BY_PUMP);
		}

		switch (lTmpFlags)
		{
		case OLA_MEM_ALL:
		{
			while (_Module.m_server.m_cMemPumpArray[lPumpNumber - 1].GetMemPumpInfo(&Info, nTmpIndex))
			{
				BOOL bRecOK = TRUE;

				PAY_AT_PUMP_INFO	cPAPInfo;										//4.0.23.20 77627
				char				sReceiptIndex[ENCRYPTED_DATA_SIZE];

				memset(&cPAPInfo, ' ', sizeof(cPAPInfo));
				memset(sReceiptIndex, ' ', sizeof(sReceiptIndex));

				// set Info(CMemPumpInfo) to cPAPInfo(PAY_AT_PUMP_INFO)
				memcpy(&cPAPInfo.CardSaleInfo.cardData, &Info.cCardData, min(sizeof(cPAPInfo.CardSaleInfo.cardData), sizeof(Info.cCardData)));
				memcpy(&cPAPInfo.CardSaleInfo.extraData, &Info.cCardSaleExtra, min(sizeof(cPAPInfo.CardSaleInfo.extraData), sizeof(Info.cCardSaleExtra)));
				memcpy(&cPAPInfo.CardSaleExtraData2, &Info.cCardSaleExtra2, min(sizeof(cPAPInfo.CardSaleExtraData2), sizeof(Info.cCardSaleExtra2)));

				_Module.m_server.m_cBaseReceipt->GetReceiptIndex(sReceiptIndex, &cPAPInfo, lPumpNumber);

				memcpy(Info.cCardData.sTrack2Data, sReceiptIndex, sizeof(Info.cCardData.sTrack2Data));		//get only 37 bytes

																											//Added this check in order to determine whether to
																											//include canceled transactions.  This check has been modified
																											//in all of the next cases except OLA_MEM_CHARGE_AVI_TRS and
																											//OLA_MEM_AMOUNT.  Those cases don't check for 
																											//cancelled transactions because charged transactions couldn't
																											//have been cancelled. Same  goes for searching by amount.

				if (bDoNotPassDeclineRecord)
				{
					bRecOK = (Info.cTrs2.m_lNumber && Info.cTrs2.m_lRoundedValue) ? TRUE : FALSE; //4.0.124.?? 349844
				}
				else
				{//4.0.124.?? 349844
					if (Info.cTrs2.m_lNumber == 0)
						bRecOK = FALSE;
				}

				if (bRecOK)
				{
					bFoundRec = TRUE;
					break;
				}

				nTmpIndex++;
			}

			if (bFoundRec)
				*nIndex = nTmpIndex;
			else
				retCode = GPI_PARAM_NOT_FOUND;
		}
		break;

		case OLA_MEM_OPEN_AVI:
		{
			while (_Module.m_server.m_cMemPumpArray[lPumpNumber - 1].GetMemPumpInfo(&Info, nTmpIndex))
			{
				BOOL bRecOK = TRUE;

				if (bDoNotPassDeclineRecord)
					bRecOK = (Info.cTrs2.m_lNumber || Info.cTrs2.m_lRoundedValue) ? TRUE : FALSE;

				if ((Info.cCardData.sTranType == CD_TYPE_AVI) &&
					(Info.cRecordStatus == OLA_MEM_RECORD_NOT_USE) &&
					bRecOK)
				{
					if (cCurTime > Info.cTrs2.GetTime())
					{
						CTimeSpan ts;
						ts = cCurTime - Info.cTrs2.GetTime();
						LONGLONG lTmp = ts.GetTotalSeconds();
						if (ts.GetTotalSeconds() <= _Module.m_server.GetAviTrsTimeOut())
						{
							bFoundRec = TRUE;
							break;
						}
					}

				}
				nTmpIndex++;
			}

			if (bFoundRec)
			{
				*nIndex = nTmpIndex;
			}
			else
			{
				retCode = GPI_PARAM_NOT_FOUND;
			}

		}
		break;

		case OLA_MEM_TIME_OUT_AVI:
		{
			while (_Module.m_server.m_cMemPumpArray[lPumpNumber - 1].GetMemPumpInfo(&Info, nTmpIndex))
			{
				BOOL bRecOK = TRUE;

				if (bDoNotPassDeclineRecord)
					bRecOK = (Info.cTrs2.m_lNumber || Info.cTrs2.m_lRoundedValue) ? TRUE : FALSE;

				//All credit/AVI records have a default setting
				//of OLA_MEM_RECORD_NOT_USE.
				if ((Info.cCardData.sTranType == CD_TYPE_AVI) &&
					(cCurTime > Info.cTrs2.GetTime()) &&
					(Info.cRecordStatus == OLA_MEM_RECORD_NOT_USE) &&
					bRecOK)

				{
					LONGLONG lTmp;
					CTimeSpan ts;
					ts = cCurTime - Info.cTrs2.GetTime();
					lTmp = ts.GetTotalSeconds();
					if (ts.GetTotalSeconds() > _Module.m_server.GetAviTrsTimeOut())
					{
						bFoundRec = TRUE;
						break;
					}
				}
				nTmpIndex++;
			}

			if (bFoundRec)
			{
				*nIndex = nTmpIndex;
			}
			else
			{
				retCode = GPI_PARAM_NOT_FOUND;
			}

		}
		break;

		case OLA_MEM_CHARGE_AVI_TRS:
		{
			while (_Module.m_server.m_cMemPumpArray[lPumpNumber - 1].GetMemPumpInfo(&Info, nTmpIndex))
			{
				if (Info.cRecordStatus == OLA_MEM_RECORD_USE)
				{
					bFoundRec = TRUE;
					break;
				}

				nTmpIndex++;
			}

			if (bFoundRec)
			{
				*nIndex = nTmpIndex;
			}
			else
			{
				retCode = GPI_PARAM_NOT_FOUND;
			}

		}
		break;

		case OLA_MEM_AMOUNT:
		{

			long lTotalValue;

			while (_Module.m_server.m_cMemPumpArray[lPumpNumber - 1].GetMemPumpInfo(&Info, nTmpIndex))
			{
				lTotalValue = ConvertStringsToLong(Info.cCardSaleExtra.sFuelAmt, sizeof(Info.cCardSaleExtra.sFuelAmt), Info.cCardSaleExtra.sFuelAmt_Msb, sizeof(Info.cCardSaleExtra.sFuelAmt_Msb));

				if ((Info.cCardData.sTranType == CD_TYPE_AVI) &&
					(Info.cRecordStatus == OLA_MEM_RECORD_NOT_USE) &&
					lTotalValue == lVal)
				{
					if (cCurTime > Info.cTrs2.GetTime())
					{
						CTimeSpan ts;
						ts = cCurTime - Info.cTrs2.GetTime();
						if (ts.GetTotalSeconds() <= _Module.m_server.GetAviTrsTimeOut())
						{
							bFoundRec = TRUE;
							break;
						}
					}

				}
				nTmpIndex++;
			}

			if (bFoundRec)
			{
				*nIndex = nTmpIndex;
			}
			else
			{
				retCode = GPI_PARAM_NOT_FOUND;
			}

		}
		break;

		case OLA_MEM_CREDIT:
			while (_Module.m_server.m_cMemPumpArray[lPumpNumber - 1].GetMemPumpInfo(&Info, nTmpIndex))
			{
				if (Info.cCardData.sTranType == CD_TYPE_CREDIT)
				{
					BOOL bRecOk = TRUE;

					if (bDoNotPassDeclineRecord)
						bRecOk = (Info.cTrs2.m_lNumber || Info.cTrs2.m_lRoundedValue) ? TRUE : FALSE;

					if (bRecOk)
					{
						bFoundRec = TRUE;
						break;
					}
				}

				nTmpIndex++;
			}

			if (bFoundRec)
				*nIndex = nTmpIndex;

			else
				retCode = GPI_PARAM_NOT_FOUND;
			break;  //end case OLA_MEM_CREDIT

		case OLA_MEM_DEBIT:
			while (_Module.m_server.m_cMemPumpArray[lPumpNumber - 1].GetMemPumpInfo(&Info, nTmpIndex))
			{
				if (Info.cCardData.sTranType == CD_TYPE_DEBIT)
				{
					BOOL bRecOk = TRUE;

					if (bDoNotPassDeclineRecord)
						bRecOk = (Info.cTrs2.m_lNumber || Info.cTrs2.m_lRoundedValue) ? TRUE : FALSE;

					if (bRecOk)
					{
						bFoundRec = TRUE;
						break;
					}
				}

				nTmpIndex++;
			}

			if (bFoundRec)
				*nIndex = nTmpIndex;

			else
				retCode = GPI_PARAM_NOT_FOUND;
			break;  //end case OLA_MEM_DEBIT

		case OLA_PAK_TO_PAP_DECLINE:						// 4.0.3.45
		{
			while (_Module.m_server.m_cMemPumpArray[lPumpNumber - 1].GetMemPumpInfo(&Info, nTmpIndex))
			{
				if (Info.cCardData.sTranType == CD_TYPE_PAK_TO_PAP_DECLINE)
				{
					BOOL bRecOk = TRUE;

					if (bRecOk)
					{
						bFoundRec = TRUE;
						break;
					}
				}
				nTmpIndex++;
			}

			if (bFoundRec)
				*nIndex = nTmpIndex;

			else
				retCode = GPI_PARAM_NOT_FOUND;

		}
		break;
		default:
			retCode = GPI_PARAM_NOT_FOUND;


		}

	}


	if (bFoundRec)
	{
		long lTotal, lTotalValue, lAuthAmount;

		if (OLA_BY_CONTROLLER)
		{
			lTotalValue = ConvertStringsToLong(Info.cCardSaleExtra.sTotalAmt, sizeof(Info.cCardSaleExtra.sTotalAmt), Info.cCardSaleExtra.sTotalAmt_Msb, sizeof(Info.cCardSaleExtra.sTotalAmt_Msb));
			if (lTotalValue == 0) //TD 69955 4.0.21.500
			{
				// in case the sTotalAmt is zero (when the discount equals the Total amount)
				// use  the sFuelAmt.
				lTotalValue = ConvertStringsToLong(Info.cCardSaleExtra.sFuelAmt, sizeof(Info.cCardSaleExtra.sFuelAmt), Info.cCardSaleExtra.sFuelAmt_Msb, sizeof(Info.cCardSaleExtra.sFuelAmt_Msb));
			}

			long lWashPrice = a2l((unsigned char *)Info.cCardSaleExtra.SALES[0].sUnitPrice, sizeof(Info.cCardSaleExtra.SALES[0].sUnitPrice));
			long lWashDiscount = a2l((unsigned char *)Info.cCardSaleExtra.SALES[0].sDiscount, sizeof(Info.cCardSaleExtra.SALES[0].sDiscount));

			lTotal = lTotalValue + lWashDiscount * 10 - lWashPrice * 10;
			lAuthAmount = ConvertStringsToLong(Info.cCardData.sAuthAmt, sizeof(Info.cCardData.sAuthAmt), Info.cCardSaleExtra.sAuthAmt_Msb, sizeof(Info.cCardSaleExtra.sAuthAmt_Msb));

			lTotalValue = lTotalValue / 10; //4.0.8.503
		}
		else
		{
			lTotal = ConvertStringsToLong(Info.cCardSaleExtra.sFuelAmt, sizeof(Info.cCardSaleExtra.sFuelAmt), Info.cCardSaleExtra.sFuelAmt_Msb, sizeof(Info.cCardSaleExtra.sFuelAmt_Msb));

			lTotalValue = ConvertStringsToLong(Info.cCardSaleExtra.sTotalAmt, sizeof(Info.cCardSaleExtra.sTotalAmt), Info.cCardSaleExtra.sTotalAmt_Msb, sizeof(Info.cCardSaleExtra.sTotalAmt_Msb));
			lTotalValue = lTotalValue / 10;
			lAuthAmount = ConvertStringsToLong(Info.cCardData.sAuthAmt, sizeof(Info.cCardData.sAuthAmt), Info.cCardSaleExtra.sAuthAmt_Msb, sizeof(Info.cCardSaleExtra.sAuthAmt_Msb));
		}

		l2a(lTotal, (unsigned char *)tagMemTrs.sFuelTotal, sizeof(tagMemTrs.sFuelTotal));

		// toatal Sale in the Pump
		l2a(lTotalValue, tagMemTrs.sTotalAmount, sizeof(tagMemTrs.sTotalAmount));

		l2a(lAuthAmount, tagMemTrs.sAuthAmount, sizeof(tagMemTrs.sAuthAmount));

		memcpy(&tagMemTrs.cardData, &Info.cCardData, sizeof(tagMemTrs.cardData));
		Info.cTrs2.GetTag(&tagMemTrs.tagTrs, 2); //TD 38654
	}


	if (lFlags2 & OLA_MEM_FLAGS2)	//TD 38654
	{
		BYTE sTmp[sizeof(TAG_OLA_MEM_REC_EX) + 2];
		Str2StrNull((BYTE *)&tagMemTrs, sizeof(TAG_OLA_MEM_REC_EX), sTmp);
		CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
		*pRec = strResult.Copy();
	}
	else
	{
		BYTE sTmp[sizeof(TAG_OLA_MEM_REC) + 2];
		_Module.m_server.m_cCommonFunction.ConvertFromTagOlaMemRecExToTagOlaMemRec(&tagMemTrs, &tmpTagMemTrs);
		Str2StrNull((BYTE *)&tmpTagMemTrs, sizeof(TAG_OLA_MEM_REC), sTmp);
		CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
		*pRec = strResult.Copy();
	}


	/////////////////////////////////////////////////////////////////////////////////////////
	if (LOG_BASIC_FLOW_CONTROL)
	{//4.0.20.501 TD 60236		
		char szResult[MAX_FIELD_VAL] = { 0 };

		_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, (char*)tagMemTrs.cardData.sTrack2Data, sizeof(tagMemTrs.cardData.sTrack2Data));

		CString str;
		str.Format("After copy sTrack2=%s, flags=0x%x, FuelAmount=%.8s, TotalAmount=%.9s Found=%d, auth amt= %.9s, trs num = %ld", szResult, lFlags, tagMemTrs.sFuelTotal, tagMemTrs.sTotalAmount, bFoundRec, tagMemTrs.sAuthAmount, Info.cTrs2.m_lNumber); //4.0.124.?? 349844

		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
	}

	/////////////////////////////////////////////////////////////////////////////////////////	

	*pVal = retCode;
}



STDMETHODIMP CGPI::SetMemOla(long lPump, long lTrsNumber, long lFlags, long lVal, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetMemOla   start");
	USES_CONVERSION;

#ifdef CATCH_GPI
	__try
	{
#endif
		SetMemOla__(lPump, lTrsNumber, lFlags, lVal, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::SetMemOla")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetMemOla()", lPump, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;


}

void CGPI::SetMemOla__(long lPump, long lTrsNumber, long lFlags, long lVal, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	CMemPumpInfo Info;
	BOOL bFoundRec = FALSE;
	short nTmpIndex = 0;
	long lCurrentTrsNumber = 0;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if ((lPump > _Module.m_server.GetMaxPumps()) || (lPump < 1))   //4.0.0.79
		retCode = GPI_INVALID_PUMP_NUMBER;
	else
	{
		DWORD dwTmp = 0;
		BOOL  bPaidAllOpenAviTrs = FALSE;
		char sFullVehicleId[FULL_VEHICLE_ID];

		memset(sFullVehicleId, ' ', sizeof(sFullVehicleId));

		_Module.m_server.m_cParam.LoadParam((char *)"GeneralParam", "PaidAllAviTrs", &dwTmp, (DWORD)0, FALSE);

		bPaidAllOpenAviTrs = (dwTmp) ? TRUE : FALSE;

		// TODO: Add your implementation code here	
		while (_Module.m_server.m_cMemPumpArray[lPump - 1].GetMemPumpInfo(&Info, nTmpIndex))
		{
			lCurrentTrsNumber = Info.cTrs2.m_lNumber;

			if (lCurrentTrsNumber == lTrsNumber)
			{
				if (Info.cRecordStatus == OLA_MEM_RECORD_USE)
				{
					retCode = GPI_DUPLICATE_SYNC_NUMBER;
				}
				else
				{
					Info.cRecordStatus = OLA_MEM_RECORD_USE;
					_Module.m_server.m_cMemPumpArray[lPump - 1].SetMemPumpInfo(Info);
					memcpy(sFullVehicleId, Info.cCardData.sFullVehicleId, sizeof(FULL_VEHICLE_ID));
				}

				bFoundRec = TRUE;
				break;
			}

			nTmpIndex++;
		}

		if (bPaidAllOpenAviTrs && bFoundRec)
		{
			for (int i = 0; i < _Module.m_server.GetLastValidPump(); i++)  //4.0.0.79
			{
				nTmpIndex = 0;

				while (_Module.m_server.m_cMemPumpArray[i].GetMemPumpInfo(&Info, nTmpIndex))
				{
					// if the flags PaidAllAviTrs was set, than scane all the sales and signed them as used.
					if ((!memcmp(sFullVehicleId, Info.cCardData.sFullVehicleId, sizeof(FULL_VEHICLE_ID))) &&
						(Info.cCardData.sTranType == CD_TYPE_AVI) &&
						Info.cRecordStatus != OLA_MEM_RECORD_USE)
					{
						Info.cRecordStatus = OLA_MEM_RECORD_USE;
						_Module.m_server.m_cMemPumpArray[i].SetMemPumpInfo(Info);
					}

					nTmpIndex++;
				}

			}
		}


		if (!bFoundRec)
		{
			retCode = GPI_TRS_NOT_FOUND;
		}
	}

	if (LOG_LOGIC_ERROR)
		_LOGMSG.LogPumpResult(
			lPump, // Src
			"IGPI",
			"SetMemOla",
			TRUE, retCode,
			FALSE, 0);


	*pVal = retCode;

}

STDMETHODIMP CGPI::GetOlaMemInfoStatus(long lFlags, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetOlaMemInfoStatus   start");
	// TODO: Add your implementation code here
	USES_CONVERSION;

#ifdef CATCH_GPI
	__try
	{
#endif
		GetOlaMemInfoStatus__(lFlags, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetOlaMemInfoStatus")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetOlaMemInfoStatus()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

void CGPI::GetOlaMemInfoStatus__(long lFlags, BSTR *pRec, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	CMemPumpInfo Info;
	BOOL bFoundRec = FALSE;
	short nTmpIndex = 0;
	long lCurrentTrsNumber = 0;
	int maxPumps = _Module.m_server.GetMaxPumps();
	char * sAviPolling = new char[_Module.m_server.GetMaxPumps()];
	CTime cCurTime = CTime::GetCurrentTime();
	BOOL bDoNotPassDeclineRecord = (lFlags & OLA_MEM_WITHOUT_CANCEL) ? TRUE : FALSE;

	memset(sAviPolling, ' ', _Module.m_server.GetMaxPumps());

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;

	else
	{
		for (int i = 0; i < _Module.m_server.GetLastValidPump(); i++)  //4.0.0.79
		{
			short nTmpIndex = 0;
			bFoundRec = FALSE;

			while (_Module.m_server.m_cMemPumpArray[i].GetMemPumpInfo(&Info, nTmpIndex))
			{
				BOOL bRecOK = TRUE;

				if (bDoNotPassDeclineRecord)
				{
					bRecOK = (Info.cTrs2.m_lNumber) ? TRUE : FALSE;
				}

				if ((Info.cCardData.sTranType == CD_TYPE_AVI) &&
					(Info.cRecordStatus == OLA_MEM_RECORD_NOT_USE) &&
					bRecOK)
				{
					if (cCurTime > Info.cTrs2.GetTime())
					{
						CTimeSpan ts;
						ts = cCurTime - Info.cTrs2.GetTime();
						long long lTmp = ts.GetTotalSeconds();
						if (ts.GetTotalSeconds() <= _Module.m_server.GetAviTrsTimeOut())
						{
							bFoundRec = TRUE;
							break;
						}
					}
				}
				nTmpIndex++;
			}

			if (bFoundRec)
			{
				if (i < maxPumps) sAviPolling[i] = '1';
			}

		}

	}

	//4.0.1.0  Although 64 pumps may be configured, we may have some
	//old client which supports only 32 pumps.  To avoid crashes, we check for a flag
	//indicating a 64 pump client
	if ((lFlags & USE_TAG_OLA_MEM_STATUS_AVI2) &&
		(_Module.m_server.GetMaxPumps() > MAX_PUMPS))
	{
		BYTE sTmp[sizeof(TAG_OLA_MEM_STATUS_AVI2) + 2];
		Str2StrNull((BYTE *)sAviPolling, sizeof(TAG_OLA_MEM_STATUS_AVI2), sTmp);
		CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
		*pRec = strResult.Copy();
	}

	else
	{
		BYTE sTmp[sizeof(TAG_OLA_MEM_STATUS_AVI) + 2];
		Str2StrNull((BYTE *)sAviPolling, sizeof(TAG_OLA_MEM_STATUS_AVI), sTmp);
		CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
		*pRec = strResult.Copy();
	}

	if (LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGPI",
			"GetOlaMemInfoStatus",
			TRUE, retCode,
			FALSE, 0);

	*pVal = retCode;

	delete[]sAviPolling;
}

// Function name	: CGPI::GetTankInfo
// Description	    : Sets *pRec with tanks info according to tank number.
//					  If tank number is 0 then all tanks info.
// Return type		: STDMETHODIMP 
// Argument         : long lTankNumber
// Argument         : BSTR *pRec
// Argument         : long lFlags
// Argument         : long *pVal
STDMETHODIMP CGPI::GetTankInfo(long lTankNumber, BSTR *pRec, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetTankInfo   start");
	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "GetTankInfo", m_lPosNumber, LOG_POS, "GetTankInfo, Throw COM Exception : E_POINTER", LOG_LEVEL_0);


		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetTankInfo__(lTankNumber, pRec, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetTankInfo")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetTankInfo", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}


// Function name	: CGPI::GetTankInfo__
// Description	    : Sets *pRec with tanks info according to tank number.
//					  If tank number is 0 then all tanks info.
// Return type		: void 
// Argument         : long lTankNumber
// Argument         : BSTR *pRec
// Argument         : long lFlags  must be 1 in order to support 32 tanks.
// Argument         : long *pVal
void CGPI::GetTankInfo__(long lTankNumber, BSTR *pRec, long lFlags, long *pVal)
{

	GPIRetCode retCode = GPI_OK;
	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (TANK_NOT_VALID(lTankNumber) && lTankNumber != READ_ALL_TANKS)
		retCode = GPI_INVALID_TANK_NUMBER;


	if (retCode == GPI_OK)

	{
		long lCounter = 0;
		long lRecordType = (lFlags & STRUCT_INCLUDES_DELIVERY) ? TRM_TYPE_INCLUDE_DELIVERY_DATA : 0;   //4.0.7.501

		_Module.m_server.m_cCommonFunction.GetTankMemoInfo(lTankNumber, pRec, lFlags, (long *)&retCode, &lCounter, lRecordType);

	}//end if(retCode == GPI_OK)
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetTankInfo",
				TRUE, retCode,
				FALSE, 0);
	}

	//Save the last time the POS call GPI method.
	_Module.m_server.m_cPosMap[m_lPosNumber].SetLastCmd(CMD_NULL, 0);

	if (pVal != NULL)
		*pVal = retCode;
}



STDMETHODIMP CGPI::SetTransactionState(long lPumpNumber, BSTR pRec, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetTransactionState   start");
	// TODO: Add your implementation code here
	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "SetTransactionState, Throw COM Exception : E_POINTER", LOG_LEVEL_0);


		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif		
		LONG lDummy = 0;
		BOOL bCreateNewTransaction = FALSE;
		CreatePostVoidTransaction__(lPumpNumber, pRec, lFlags, pVal, lDummy, bCreateNewTransaction); //CR 351687: Coles Enhanced post void transaction
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::SetTransactionState")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetTransactionState", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 



	return S_OK;
}



GPIRetCode CGPI::VerifyPostVoidTransaction(long lPumpNumber) //CR 351687: Enhanced post void transactions
{
	GPIRetCode retCode = GPI_OK;
	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;


	else if (FALSE == _Module.m_server.GetPosVoidFlags())
	{
		retCode = GPI_POS_VOID_NOT_ALLOWED;
	}

	else if (PUMP_NOT_IN_WORKING_STATE(lPumpNumber))
		retCode = GPI_PUMP_NOT_IN_WORKING_STATE;

	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GPI_FAILED_PREPAY_IO;
	return retCode;
}


GPIRetCode CGPI::CreatePostVoidTransactionFromStructure(long lPumpNumber, long lFlags, BSTR pRec, LONG& lNewTrsNumber, BOOL bCreateNewTransactionNumber /* = FALSE */) //CR 351687: Coles Enhanced post void transaction
{
	CString sMsg;
	GPIRetCode retCode = GPI_OK;
	char szFieldVal[MAX_REC_VALUES];
	memset(szFieldVal, 0, sizeof(szFieldVal));
	CString sLogBuffer;

	TAG_POST_VOID_EX		cTagPostVoid;
	long lRecordSize = (lFlags & ST_STATUS_WITH_ENLARGE_REC) ? sizeof(TAG_POST_VOID_EX) : sizeof(TAG_PUMP_TRANSACT5);
	int iLen = WideCharToMultiByte(CP_ACP, 0, pRec, -1, szFieldVal, sizeof(szFieldVal), NULL, NULL);


	sLogBuffer.Format("CreatePostVoidTransactionFromStructure: szFieldVal[%.1000s]", szFieldVal);
	sMsg.Format("%s", sLogBuffer);
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg);

	if (iLen == 0)
	{
		DWORD dwLastErr = GetLastError();
		retCode = GPI_TABLE_NAME_TOO_LONG;
	}
	else
	{
		if (iLen >= sizeof(TAG_PUMP_TRANSACT5))
		{
			memcpy(&cTagPostVoid, szFieldVal, lRecordSize); //4.0.5.44
		}
		else
		{
			retCode = GPI_INVALID_TABLE_NAME;
		}
	}

	if (retCode == GPI_OK)
	{
		CPumpTransact	trs;
		CPumpTransact	trsTmp;
		int		iRetNum = 0;
		long	p = lPumpNumber - 1;
		short nPrePayWaiting, nFree, nUnpaid;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		_Module.m_server.m_cPumpArray[p].GetTrsListCount(&nPrePayWaiting, &nFree, &nUnpaid);

		if (!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trsTmp))
		{
			if ((trsTmp.GetStatus() == IN_PROGRESS) &&
				(trsTmp.m_lRes == PAY_AT_KIOSK))
			{
				nUnpaid++;
			}
		}

		retCode = (GPIRetCode)_Module.m_server.IsPostVoidTrsAllowed(lPumpNumber, nUnpaid);

		if (retCode == GPI_OK)
		{
			trs.Clear();
			_Module.m_server.m_cPumpArray[p].SetTagPumpTrsToTrs(cTagPostVoid.cTagPumpTransact, trs);
			if (bCreateNewTransactionNumber)  //CR 351687: Coles Enhanced post void transaction: Supporting manual post void
			{
				lNewTrsNumber = trs.m_lNumber = _Module.m_server.NewTransactNumber();
			}
			retCode = VerifyPostVoidTransactionValues(trs);

			if (GPI_OK == retCode)
			{
				if (_Module.m_server.FindTrsByTrsNumber(trs.m_lNumber))
				{
					//4.0.25.30 - TD 69655
					EXTRA_INFO  cExtraInfo;
					cExtraInfo.ClearRecord();
					BOOL bUpdateQdxData = FALSE;

					if (lFlags & ST_STATUS_INCLUDE_LAM)
					{
						InsertLAMInfo(lPumpNumber, cExtraInfo, trs, cTagPostVoid, bUpdateQdxData); //4.0.25.30 - TD 69655
					}

					if (lFlags & ST_STATUS_INCLUDE_DISPENSING_VOLUMES)
					{
						InsertDispensingVolumes(lPumpNumber, cExtraInfo, trs, cTagPostVoid, bUpdateQdxData);
					}

					UpdateQdxData(lPumpNumber, bUpdateQdxData, cExtraInfo);
					iRetNum = _Module.m_server.CentralInsertTrs(lPumpNumber, &trs, TRUE);
					LogPostVoidTransaction(lPumpNumber, trs, iRetNum);


					if (!iRetNum)
						_Module.m_server.m_cShiftLog.LogTrs(&trs, FALSE, FALSE, TRUE);
				}
				else
				{
					LogDuplicatePostVoidTransaction(trsTmp, iRetNum);
					retCode = GPI_DUPLICATE_TRS;
				}
			}
		}

		if (iRetNum != 0)
		{
			retCode = GPI_TRS_NOT_FOUND;
		}
	}
	return retCode;
}

void CGPI::InsertLAMInfo(long lPumpNumber, EXTRA_INFO &cExtraInfo, CPumpTransact &trs, TAG_POST_VOID_EX &cTagPostVoid, BOOL& bUpdateQdxData) //CR 351687: Coles Enhanced post void transaction
{
	cExtraInfo.lIndexNumber = trs.m_lNumber;
	trs.SetLink(TRS_LINK_EXTRA_INFO + TRS_NO_LINK, 0L);
	cExtraInfo.sLAMFlags = (cTagPostVoid.sLAMFlags == LAMF_DATA_VALID) ? cTagPostVoid.sLAMFlags : LAMF_DATA_NOT_VALID;	//4.0.6.501					

	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString sMsg;
		sMsg.Format("Add new record to extra info with LAM details index=%ld, Flags=%c,", cExtraInfo.lIndexNumber, cExtraInfo.sLAMFlags);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sMsg);
	}
}

void CGPI::InsertDispensingVolumes(long lPumpNumber, EXTRA_INFO &cExtraInfo, CPumpTransact &trs, TAG_POST_VOID_EX &cTagPostVoid, BOOL& bUpdateQdxData) //CR 351687: Coles Enhanced post void transaction
{
	CString sMsg;
	cExtraInfo.lIndexNumber = trs.m_lNumber;
	trs.SetLink(TRS_LINK_EXTRA_INFO + TRS_NO_LINK, 0L);
	bUpdateQdxData = TRUE;

	memcpy(cExtraInfo.sPreDispensingVolumeLow, cTagPostVoid.sPreDispensingVolumeLow, sizeof(cTagPostVoid.sPreDispensingVolumeLow));
	memcpy(cExtraInfo.sPreDispensingVolumeHigh, cTagPostVoid.sPreDispensingVolumeHigh, sizeof(cTagPostVoid.sPreDispensingVolumeHigh));
	memcpy(cExtraInfo.sPostDispensingVolumeLow, cTagPostVoid.sPostDispensingVolumeLow, sizeof(cTagPostVoid.sPostDispensingVolumeLow));
	memcpy(cExtraInfo.sPostDispensingVolumeHigh, cTagPostVoid.sPostDispensingVolumeHigh, sizeof(cTagPostVoid.sPostDispensingVolumeHigh));

	if (LOG_BASIC_FLOW_CONTROL)
	{
		sMsg.Format("Add new record to extra info with Dispensing Counters details index=%ld, sPreDispensingVolumeLow = %.5s, sPreDispensingVolumeHigh = %.15s, sPostDispensingVolumeLow = %.5s, sPostDispensingVolumeHigh = %.15s",
			cExtraInfo.lIndexNumber,
			cExtraInfo.sPreDispensingVolumeLow,
			cExtraInfo.sPreDispensingVolumeHigh,
			cExtraInfo.sPostDispensingVolumeLow,
			cExtraInfo.sPostDispensingVolumeHigh);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sMsg);
	}
}



void CGPI::UpdateQdxData(long lPumpNumber, BOOL bUpdateQdxData, EXTRA_INFO& cExtraInfo) //CR 351687: Coles Enhanced post void transaction
{
	if (bUpdateQdxData) //4.0.25.30 - TD 69655
	{
		long lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_INSERT, &cExtraInfo);
		CString sMsg;
		if (LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("Insert to extra info Qdex, Rtc=%ld", lRtc);
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sMsg);
		}
	}
}

void CGPI::LogPostVoidTransaction(long lPumpNumber, CPumpTransact &trs, int iRetNum) //CR 351687: Coles Enhanced post void transaction
{
	CString sMsg;
	sMsg.Format("Post Void for Trs=%ld , Pump=%ld, Value=%ld, Volume=%ld, GradePrice=%ld , Grade=%d, Res=%ld m_nNozzle=%d Rtc=%d",
		trs.m_lNumber,
		trs.m_lPump,
		trs.m_lRoundedValue,
		trs.m_lRoundedVolume,
		trs.m_lGradePrice,
		trs.m_nGrade,
		trs.m_lRes,
		trs.m_nNozzle,  //4.0.8.505
		iRetNum);

	_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sMsg, LOG_LEVEL_3);
}

GPIRetCode CGPI::VerifyPostVoidTransactionValues(CPumpTransact &trs) //CR 351687: Coles Enhanced post void transaction
{
	GPIRetCode retCode = GPI_OK;
	if ((trs.m_lNumber < 1) || (trs.m_lNumber > MAX_TRANSACT_SEQ_NUMBER))
	{
		retCode = GPI_TRS_NOT_ACCEPTED;
	}
	else if ((trs.m_lRoundedValue <= 0) || (trs.m_lRoundedVolume <= 0))
	{
		retCode = GPI_TRS_PARAM_NOT_VALID;
	}
	else if (GRADE_NOT_VALID(trs.m_nGrade))
	{
		retCode = GPI_INVALID_GRADE_NUMBER;
	}
	return retCode;
}

void CGPI::LogDuplicatePostVoidTransaction(CPumpTransact &trsTmp, int iRetNum) //CR 351687: Coles Enhanced post void transaction
{
	CString sMsg;
	sMsg.Format("Duplicate Post Void for Trs=%ld , Pump=%ld, Value=%ld, Volume=%ld, GradePrice=%ld , Grade=%d Rtc=%d",
		trsTmp.m_lNumber,
		trsTmp.m_lPump,
		trsTmp.m_lRoundedValue,
		trsTmp.m_lRoundedVolume,
		trsTmp.m_lGradePrice,
		trsTmp.m_nGrade,
		iRetNum);
	_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, trsTmp.m_lPump, LOG_PUMP, sMsg, LOG_LEVEL_3);
}


GPIRetCode CGPI::CreatePostVoidTransactionFromXML(long lPumpNumber, BSTR pRec, long* &pVal, LONG& lNewTrsNumber, BOOL bCreateNewTransactionNumber /* = FALSE */) //CR 351687: Coles Enhanced post void transaction
{
	char			szFieldVal[MAX_REC_VALUES];
	char			szValXML[10000]; //4.0.12.517
	CString			str;
	GPIRetCode		retCode = GPI_OK;
	DWORD			dwLastErr;
	memset(szFieldVal, 0, sizeof(szFieldVal));

	if (LOG_BASIC_FLOW_CONTROL)
	{

		str.Format("CreatePostVoidTransaction: Creating transaction from XML");
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
	}

	int iLen = WideCharToMultiByte(CP_ACP, 0, pRec, -1, szValXML, sizeof(szValXML), NULL, NULL);
	if (iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GPI_TABLE_NAME_TOO_LONG;
		str.Format("CreatePostVoidTransactionFromXML: ERROR, GetLastError[%d]", dwLastErr);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
	}
	*pVal = _Module.m_server.m_cCardSaleDataXML.CreatePakTrsFromXML(szValXML, lNewTrsNumber, bCreateNewTransactionNumber);
	return retCode;
}

GPIRetCode CGPI::VerifySetTrsFlags(long lPumpNumber) //CR 351687: Enhanced post void transactions
{
	GPIRetCode retCode = GPI_OK;
	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}
	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	else if (PUMP_NOT_IN_WORKING_STATE(lPumpNumber))
		retCode = GPI_PUMP_NOT_IN_WORKING_STATE;

	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GPI_FAILED_PREPAY_IO;
	return retCode;
}


STDMETHODIMP CGPI::CreateManualPostVoidTransaction(long lPumpNumber, BSTR pRec, long lFlags, long* lNewTrsNumber, long *pVal) //CR 351687: Enhanced post void transactions
{
	LOG_EXTERNALCALL("CGPI::CreateManualPostVoidTransaction   start");
	if (NULL == pRec || NULL == lNewTrsNumber)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "CreateManualPostVoidTransaction, Throw COM Exception : E_POINTER", LOG_LEVEL_0);


		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif		
		BOOL bCreateNewTransaction = TRUE;
		CreatePostVoidTransaction__(lPumpNumber, pRec, lFlags, pVal, *lNewTrsNumber, bCreateNewTransaction);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::CreateManualPostVoidTransaction")) //4.0.15.450 //4.0.27.140
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "CreateManualPostVoidTransaction", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}




STDMETHODIMP CGPI::SetTrsFlags(long lPumpNumber, long lTrsNumber, long lMiscFlags, long lMiscFlag2, long *pVal) //CR 351687: Enhanced post void transactions
{
	LOG_EXTERNALCALL("CGPI::SetTrsFlags   start");

#ifdef CATCH_GPI
	__try
	{
#endif		
		SetTrsFlags__(lPumpNumber, lTrsNumber, lMiscFlags, lMiscFlag2, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::SetTrsFlags"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
		{
			_LOGMSG.LogClassMsg("CGPI", "SetTrsFlags", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);
		}
		exit(1);
	}
#endif 
	return S_OK;
}


void CGPI::SetTrsFlags__(long lPumpNumber, long lTrsNumber, long lMiscFlags, long lMiscFlags2, long *pVal) //CR 351687: Enhanced post void transactions
{
	CPumpTransact	trs;
	CString			str;
	GPIRetCode		retCode;

	if (LOG_BASIC_FLOW_CONTROL)
	{
		str.Format("SetTrsFlags pump[%ld] TrsNumber[%d] miscFlags[%ld] miscFlags2[%ld]", lPumpNumber, lTrsNumber, lMiscFlags, lMiscFlags2);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
	}
	retCode = VerifySetTrsFlags(lPumpNumber);
	if (GPI_OK == retCode)
	{
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		int iRetVal = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsByNumber(lTrsNumber, &trs);
		if (0 == iRetVal) //Transaction found
		{
			str.Format("SetTrsFlags: BEFORE CHANGE. Trs misc[%ld]", trs.m_lMisc);
			str.Format("SetTrsFlags: BEFORE CHANGE. Trs misc2[%ld]", trs.m_lMisc2);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

			trs.m_lMisc = trs.m_lMisc & lMiscFlags;
			trs.m_lMisc2 = trs.m_lMisc2 & lMiscFlags2;
			_Module.m_server.CentralUpdateTrs(lPumpNumber, lTrsNumber, &trs, FALSE);

			str.Format("SetTrsFlags: AFTER CHANGE Trs misc[%ld]", trs.m_lMisc);
			str.Format("SetTrsFlags: AFTER CHANGE Trs misc2[%ld]", trs.m_lMisc2);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
		}
		else
		{
			retCode = GPI_TRS_NOT_FOUND;
		}
	}

	if (retCode)
	{
		if (LOG_LOGIC_ERROR)
		{
			_LOGMSG.LogResult(m_lPosNumber, "IGPI", "SetTrsFlags", TRUE, retCode, FALSE, 0);
		}
	}

	if (NULL != pVal)
	{
		*pVal = retCode;
	}
}

STDMETHODIMP CGPI::DisablePriceDisplay(long *pVal) //CR 404591
{
	LOG_EXTERNALCALL("CGPI::DisablePriceDisplay   start");
#ifdef CATCH_GPI
	__try
	{
#endif		
		DisablePriceDisplay__(pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::DisablePriceDisplay"))
	{
		if (LOG_ERROR)
		{
			_LOGMSG.LogClassMsg("CGPI", "DisablePriceDisplay", 0, LOG_NONE, "", LOG_LEVEL_0);
		}
		exit(1);
	}
#endif 
	return S_OK;
}
void CGPI::DisablePriceDisplay__(long *pVal) //CR 404591
{
	CString			str;
	GPIRetCode		retCode = GPI_OK;
	if (LOG_BASIC_FLOW_CONTROL)
	{
		str.Format("DisablePriceDisplay__ ");
		_LOGMSG.LogMsg(0, LOG_PUMP, str);
	}

	if (GPI_OK == retCode)
	{
		_Module.m_server.SetCommand(CMD_DISABLE_PD_PANEL, m_lPosNumber, 0);
	}
	if (retCode)
	{
		if (LOG_LOGIC_ERROR)
		{
			_LOGMSG.LogResult(m_lPosNumber, "IGPI", "DisablePriceDisplay__", TRUE, retCode, FALSE, 0);
		}
	}

	if (NULL != pVal)
	{
		*pVal = retCode;
	}
}

STDMETHODIMP CGPI::EnablePriceDisplay(long *pVal) //CR 404591
{
	LOG_EXTERNALCALL("CGPI::EnablePriceDisplay   start");
#ifdef CATCH_GPI
	__try
	{
#endif		
		EnablePriceDisplay__(pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::EnablePriceDisplay"))
	{
		if (LOG_ERROR)
		{
			_LOGMSG.LogClassMsg("CGPI", "EnablePriceDisplay", 0, LOG_NONE, "", LOG_LEVEL_0);
		}
		exit(1);
	}
#endif 
	return S_OK;
}

STDMETHODIMP CGPI::InitDebit(long lPumpGroup01, long lPumpGroup2, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::InitDebit   start");
#ifdef CATCH_GPI
	__try
	{
#endif		
		InitDebit__(lPumpGroup01, lPumpGroup2, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::EnablePriceDisplay"))
	{
		if (LOG_ERROR)
		{
			_LOGMSG.LogClassMsg("CGPI", "EnablePriceDisplay", 0, LOG_NONE, "", LOG_LEVEL_0);
		}
		exit(1);
	}
#endif 
	return S_OK;
}
void CGPI::EnablePriceDisplay__(long *pVal) //CR 404591
{
	CString			str;
	GPIRetCode		retCode = GPI_OK;
	if (LOG_BASIC_FLOW_CONTROL)
	{
		str.Format("EnablePriceDisplay__ ");
		_LOGMSG.LogMsg(0, LOG_PUMP, str);
	}

	if (GPI_OK == retCode)
	{
		_Module.m_server.SetCommand(CMD_ENABLE_PD_PANEL, m_lPosNumber, 0);
	}
	if (retCode)
	{
		if (LOG_LOGIC_ERROR)
		{
			_LOGMSG.LogResult(m_lPosNumber, "IGPI", "EnablePriceDisplay__", TRUE, retCode, FALSE, 0);
		}
	}

	if (NULL != pVal)
	{
		*pVal = retCode;
	}
}

void CGPI::CreatePostVoidTransaction__(long lPumpNumber, BSTR pRec, long lFlags, long *pVal, LONG& lNewTrsNumber, BOOL bCreateNewTransactionNumber /* = FALSE */)
{
	DWORD		dwLastErr = 0;
	long		lRtc = -1;
	GPIRetCode	retCode = VerifyPostVoidTransaction(lPumpNumber);
	CString		str;

	if (LOG_BASIC_FLOW_CONTROL)
	{
		str.Format("SetTransactionState pump number = %ld, flags = %ld ", lPumpNumber, lFlags);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

	}

	CString sMsg;


	if (lFlags & ST_STATUS_USING_XML)
	{
		retCode = CreatePostVoidTransactionFromXML(lPumpNumber, pRec, pVal, lNewTrsNumber, bCreateNewTransactionNumber);
	}//4.0.12.517
	else
	{
		retCode = CreatePostVoidTransactionFromStructure(lPumpNumber, lFlags, pRec, lNewTrsNumber, bCreateNewTransactionNumber);
	}

	if (retCode)
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"SetTransactionState",
				TRUE, retCode,
				FALSE, 0);
	}

	if (pVal != NULL)
		*pVal = retCode;
}


//Get complete picture of pumps active in forecourt.
STDMETHODIMP CGPI::GetStationStatus(long *lPumpMap1, long *lPumpMap2, long lFlags, long *pRetVal)
{
	LOG_EXTERNALCALL("CGPI::GetStationStatus   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		GetStationStatus__(lPumpMap1, lPumpMap2, lFlags, pRetVal);
#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("GPI::GetStationSatus")) //4.0.15.450
	{
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetStationSatus()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}


/******************************************************************************
Description:	Ascertain whether all pumps are idle (READY && NULL_RES)
Returns:      	void
Parameters:   	long *lPumpMap1, long *lPumpMap2, long flags (currently unused), long *pRetVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Eyal			17/12/2001   17:31		Start
******************************************************************************/

void CGPI::GetStationStatus__(long *lPumpMap1, long *lPumpMap2, long lFlags, long *pRetVal)
{
	GPIRetCode lRetCode = GPI_OK;

	if (!m_bInitDone)
		lRetCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		lRetCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		lRetCode = GPI_SERVER_NOT_READY;

	*lPumpMap1 = *lPumpMap2 = 0;

	//No errors so far - continue.
	if (lRetCode == GPI_OK)
	{
		_Module.m_server.m_cCommonFunction.GetStationStatus(lPumpMap1, lPumpMap2, lFlags, pRetVal);
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString sMsg;
		sMsg.Format("CGPI::GetStationStatus() pump bit maps - 1 to 32 %x , 33 to 64 %x , returned %d",
			*lPumpMap1,
			*lPumpMap2,
			lRetCode);

		_LOGMSG.LogMsg("CGPI", "GetStationSatus", m_lPosNumber, LOG_POS, sMsg);
	}

	if (pRetVal)
		*pRetVal = lRetCode;
}

STDMETHODIMP CGPI::AuthorizeAndLimit2(long lPumpNumber, short nLevel, long lGradePrice, long lMaxValue, long lMaxVolume, long lFlags, long *lSyncNumber, BSTR sRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::AuthorizeAndLimit2   start");
	// TODO: Add your implementation code here	
#ifdef CATCH_GPI
	__try
	{
#endif

		AuthorizeAndLimit2__(lPumpNumber, nLevel, lGradePrice, lMaxValue, lMaxVolume, lFlags, lSyncNumber, sRec, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::AuthorizeAndLimit")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "AuthorizeAndLimit()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	LOG_EXTERNALCALL("CGPI::AuthorizeAndLimit2   end");
	return S_OK;
}

void CGPI::AuthorizeAndLimit2__(long lPumpNumber, short nLevel, long lGradePrice, long lMaxValue, long lMaxVolume, long lFlags, long *lSyncNumber, BSTR sRec, long *pVal, __int64 lPurchaseID /* = 0*/)
{
	AuthorizeAndLimit__(lPumpNumber, nLevel, lGradePrice, lMaxValue, lMaxVolume, lFlags, lSyncNumber, pVal, sRec, lPurchaseID);//4.0.20.54
}

STDMETHODIMP CGPI::QueryExtraInfoData(long lPumpNumber, long *lIndexNumber, short nFlags, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::QueryExtraInfoData   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		QueryExtraInfoData__(lPumpNumber, lIndexNumber, nFlags, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::QueryExtraInfoData")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "QueryExtraInfoData()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


void CGPI::QueryExtraInfoData__(long lPumpNumber, long *lIndexNumber, short nFlags, BSTR *pRec, long *pVal)
{
	GPIRetCode					retCode = GPI_OK;
	long						lRtc = OK;
	long						lTagRecSize = _Module.m_server.m_cCommonFunction.GetTagRecSize(nFlags);
	BOOL						bRetCode = FALSE;

	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("CGPI:QueryExtraInfoData Insert with those parameters  lPumpNumber=%ld, lIndexNumber=%ld,  nFlags=%d, lTagRecSize=%ld",
			lPumpNumber, *lIndexNumber, nFlags, lTagRecSize);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
	}

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	if ((retCode == GPI_OK) && !(nFlags & QEI_FLEET_PAK))
	{
		EXTRA_INFO_REC	    info;
		info.ClearRecord(); // RFUEL-3708
		CString pExtraInfoXml(" ");

		lRtc = _Module.m_server.m_cCommonFunction.GetExtraInfoRecFromDB(&info, nFlags, lIndexNumber, &pExtraInfoXml);					//4.0.23.500 TD 69914

		if (lRtc == OK)
		{
			BYTE	*pTmpTagRec = NULL;

			if (info.info.lFlag & TRS_LINK_FLEET)
			{
				TAG_DIESEL_SOLD_INFO	sTagRec;
				memset(&sTagRec, ' ', sizeof(sTagRec));

				pTmpTagRec = new BYTE[sizeof(sTagRec) + 2];

				_Module.m_server.m_cCommonFunction.ConvertExtraInfoRecord2Tag(&sTagRec, info, lPumpNumber, m_lPosNumber);

				Str2StrNull((BYTE*)&sTagRec, sizeof(sTagRec), pTmpTagRec);

				CComBSTR strResult(sizeof(sTagRec) + 2, (LPCSTR)pTmpTagRec);
				*pRec = strResult.Copy();
			}
			else
			{
				pTmpTagRec = new BYTE[lTagRecSize + 2];

				if (nFlags & QEI_STRUCT_IS_LOYALTY_INFO2)
				{
					TAG_DISCOUNT_LOYALTY_INFO2	sTagRec;
					memset(&sTagRec, ' ', sizeof(sTagRec));

					_Module.m_server.m_cCommonFunction.ConvertExtraInfoRecord2Tag(&sTagRec, info, lPumpNumber, m_lPosNumber);

					Str2StrNull((BYTE *)&sTagRec, lTagRecSize, pTmpTagRec);
				}
				else if (nFlags & QEI_STRUCT_IS_LOYALTY_INFO3)
				{
					TAG_DISCOUNT_LOYALTY_INFO3	sTagRec;
					memset(&sTagRec, ' ', sizeof(sTagRec));

					_Module.m_server.m_cCommonFunction.ConvertExtraInfoRecord2Tag(&sTagRec, info, lPumpNumber, m_lPosNumber);

					Str2StrNull((BYTE *)&sTagRec, lTagRecSize, pTmpTagRec);
				}
				else if (nFlags & QEI_STRUCT_IS_LOYALTY_INFO4)
				{
					TAG_DISCOUNT_LOYALTY_INFO4	sTagRec;
					memset(&sTagRec, ' ', sizeof(sTagRec));

					_Module.m_server.m_cCommonFunction.ConvertExtraInfoRecord2Tag(&sTagRec, info, lPumpNumber, m_lPosNumber);

					if (LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("CGPI:QueryExtraInfoData eQEI_STRUCT_IS_LOYALTY_INFO4  read lPumpNumber = %ld, lIndexNumber = %ld, nFlags = %d, lTagRecSize = %ld, Rtc = %d, sClubType = %.2s, len = %d",
							lPumpNumber, *lIndexNumber, nFlags, lTagRecSize, retCode, sTagRec.sClubType, sizeof(TAG_DISCOUNT_LOYALTY_INFO4));
						_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
					}

					Str2StrNull((BYTE *)&sTagRec, lTagRecSize, pTmpTagRec);

					for (int i = 0; i < sizeof(TAG_DISCOUNT_LOYALTY_INFO4); i++)
					{
						if (pTmpTagRec[i] == 0)
						{
							if (LOG_BASIC_FLOW_CONTROL)
							{
								CString str;
								str.Format("CGPI:QueryExtraInfoData found NULL at position %d", i);
#ifdef DEBUG								
								_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
#endif
								pTmpTagRec[i] = ' ';
							}
						}
					}


					if (LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("CGPI:QueryExtraInfoData() - before exist with those parameters lPumpNumber = %ld, lIndexNumber = %ld, nFlags = %d, lTagRecSize = %ld, Rtc = %d, sClubType = %.2s",
							lPumpNumber, *lIndexNumber, nFlags, lTagRecSize, retCode, sTagRec.sClubType);
						_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
					}

				}
				else if (nFlags & QEI_STRUCT_IS_ANPR_INFO_PLUS)
				{
					TAG_ANPR_INFO_PLUS	sTagRec;
					memset(&sTagRec, ' ', sizeof(sTagRec));

					_Module.m_server.m_cCommonFunction.ConvertExtraInfoRecord2Tag(&sTagRec, info, lPumpNumber);

					Str2StrNull((BYTE *)&sTagRec, lTagRecSize, pTmpTagRec);
				}
				else if (nFlags & QEI_STRUCT_IS_PUMP_TEST_INFO)
				{
					TAG_PUMP_TEST_INFO	sTagRec;
					memset(&sTagRec, ' ', sizeof(sTagRec));

					_Module.m_server.m_cCommonFunction.ConvertExtraInfoRecord2Tag(&sTagRec, info, lPumpNumber);

					Str2StrNull((BYTE *)&sTagRec, lTagRecSize, pTmpTagRec);
				}
				else
				{
					TAG_DISCOUNT_LOYALTY_INFO	sTagRec;
					memset(&sTagRec, ' ', sizeof(sTagRec));

					_Module.m_server.m_cCommonFunction.ConvertExtraInfoRecord2Tag(&sTagRec, info, lPumpNumber, m_lPosNumber);

					Str2StrNull((BYTE *)&sTagRec, lTagRecSize, pTmpTagRec);
				}

				*lIndexNumber = info.info.lIndexNumber;

				if (nFlags & QEI_STRUCT_IS_LOYALTY_INFO4_XML)   //RFUEL-520
				{
					CComBSTR strResult(pExtraInfoXml);
					*pRec = strResult.Copy();
				}
				else
				{
					CComBSTR strResult(lTagRecSize + 2, (LPCSTR)pTmpTagRec);
					*pRec = strResult.Copy();
				}

				if (LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("CGPI:QueryExtraInfoData before exist with those parameters  lPumpNumber=%ld, lIndexNumber=%ld,  nFlags=%d, lTagRecSize=%ld, Rtc=%d",
						lPumpNumber, *lIndexNumber, nFlags, lTagRecSize, retCode);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
				}
			}

			if (pTmpTagRec != NULL)
				delete[]pTmpTagRec;

			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("CGPI:QueryExtraInfoData exist with those parameters  lPumpNumber=%ld, lIndexNumber=%ld,  nFlags=%d, lTagRecSize=%ld, Rtc=%d",
					lPumpNumber, *lIndexNumber, nFlags, lTagRecSize, retCode);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
			}
		}
		else if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
		{
			retCode = GPI_TRS_NOT_FOUND;
		}
		else
			retCode = GPI_FAILED_IO;
	}
	else						//if( (retCode != GPI_OK) || (nFlags & QEI_FLEET_PAK))
	{
		if (nFlags & QEI_FLEET_PAK)										//4.0.9.509
		{

			char *sTmpXML = NULL; // TD 396348
			CPumpTransact cTrs;
			cTrs.m_lNumber = *lIndexNumber;
			if (_Module.m_server.m_cCardSaleDataXML.CreateFleetPAKXML(cTrs, sTmpXML))
			{
				retCode = GPI_TRS_NOT_FOUND;							//4.0.10.504

				CComBSTR strResult(" ");
				*pRec = strResult.Copy();
			}
			else
			{
				CComBSTR strResult(sTmpXML);
				*pRec = strResult.Copy();
			}

			if (sTmpXML)
			{
				delete[] sTmpXML;
			}
		}
		else
		{
			TAG_DISCOUNT_LOYALTY_INFO3	tagRec;
			memset(&tagRec, ' ', sizeof(tagRec));

			BYTE *sTmp = new BYTE[lTagRecSize + 2];
			Str2StrNull((BYTE *)&tagRec, lTagRecSize, sTmp);

			CComBSTR strResult(lTagRecSize + 2, (LPCSTR)sTmp);
			*pRec = strResult.Copy();

			if (sTmp != NULL)										//4.0.5.26
				delete[]sTmp;
		}
	}

	if ((retCode) && (retCode != GPI_TRS_NOT_FOUND))
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(m_lPosNumber,				// Src
				"IGPI",
				"QueryExtraInfoData",
				TRUE,
				retCode,
				(lRtc != 0) ? TRUE : FALSE,
				lRtc
			);
	}

	if (pVal != NULL)
		*pVal = retCode;
	//print out record RFUEL - 2424,2496
#ifdef DEBUG
	CString str = "QueryExtraInfoData buf[" + CString(*pRec) +"]";
	if (LOG_ALL_FLOW_CONTROL)
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str,LOG_LEVEL_3);
#endif
}

STDMETHODIMP CGPI::GetReceipt2(long lPosNumber, BSTR sTrack2, long lIndex, BSTR *sReceiptData, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetReceipt2   start");
	// TODO: Add your implementation code here

	USES_CONVERSION;


	if (sTrack2 == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "GetReceipt2", m_lPosNumber, LOG_POS, "GetReceipt2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);

		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetReceipt2__(lPosNumber, sTrack2, lIndex, sReceiptData, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetReceipt2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetReceipt2()", lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

STDMETHODIMP CGPI::ReceiptPrinted2(BSTR sTrack2, long lIndex, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::ReceiptPrinted2   start");
	// TODO: Add your implementation code here

	USES_CONVERSION;

	if (sTrack2 == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "ReceiptPrinted2", m_lPosNumber, LOG_POS, "ReceiptPrinted2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);

		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		ReceiptPrinted2__(sTrack2, lIndex, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::ReceiptPrinted2")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "ReceiptPrinted2", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}

void CGPI::ReceiptPrinted2__(BSTR sTrack2, long lIndex, long lFlags, long *pVal)
{
	GPIRetCode			retCode = GPI_OK;
	long				lRtc = 0;
	long				lPosIndex = lIndex;

	DWORD				dwLastErr;
	RECEIPT_INFO		Info;
	CPumpTransact		trs;
	PAY_AT_PUMP_INFO	PayAtPumpInfo;
	int iLen = 0;

	char szTrack2[ENCRYPTED_DATA_SIZE];

	long lTrack2Length = sizeof(szTrack2);		//4.0.20.506
	memset(szTrack2, ' ', lTrack2Length);

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	if (retCode == GPI_OK)
	{
		iLen = WideCharToMultiByte(CP_ACP, 0, sTrack2, -1, szTrack2, lTrack2Length, NULL, NULL);

		//call get card info to get the account number 
		//after we encrypt the account and this become the key to the QDX 

		if (iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GPI_NO_RECEIPT;
		}
		else
		{
			char sIndex[ENCRYPTED_DATA_SIZE];
			memset(sIndex, ' ', sizeof(sIndex));

			_Module.m_server.m_cBaseReceipt->GetReceiptIndex(sIndex, lTrack2Length, sTrack2, lFlags, 0, lIndex);		//4.0.26.503 TD 167039

			memcpy(szTrack2, sIndex, ENCRYPTED_DATA_SIZE);

			for (int count = 0; count < ENCRYPTED_DATA_SIZE; count++) //4.0.27.30 TD 219348
			{
				if (szTrack2[count] == 0)
					szTrack2[count] = ' ';
			}

			if (lFlags & RECEIPT_INDEX_IS_TRS)  //3.2.0.22
			{
				long lTrsNumber = 0;
				Info.cReceipt1.Index.byIndex = 0;

				memcpy(Info.cReceipt1.Index.sTrack2, szTrack2, lTrack2Length);

				CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
				lRtc = 1;//PumpSrvDatabase.GetLastReceiptRecord(Info) ? OK : 1;
				memset(&PayAtPumpInfo, ' ', sizeof(PAY_AT_PUMP_INFO));
				_Module.m_server.ConvertReceiptToOla(&Info, &trs, &PayAtPumpInfo);
				lTrsNumber = a2l(PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr, sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr));


				memset(&PayAtPumpInfo, ' ', sizeof(PAY_AT_PUMP_INFO));
				_Module.m_server.ConvertReceiptToOla(&Info, &trs, &PayAtPumpInfo);
				lTrsNumber = a2l(PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr, sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr));

				for (long lPosIndex = 0; ((lRtc == OK) && (lTrsNumber != lIndex) &&
					(!memcmp(Info.cReceipt1.Index.sTrack2, szTrack2, lTrack2Length))); lPosIndex++)
				{

					lRtc = _Module.m_server.IndexOperation(RECEIPT_FILE, REC_READ_NEXT, &Info);

					memset(&PayAtPumpInfo, ' ', sizeof(PAY_AT_PUMP_INFO));
					_Module.m_server.ConvertReceiptToOla(&Info, &trs, &PayAtPumpInfo);
					lTrsNumber = a2l(PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr, sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr));

				}

				if (((lRtc == OK) && !memcmp(Info.cReceipt1.Index.sTrack2, szTrack2, lTrack2Length)) &&
					(lTrsNumber == lIndex))
				{
					BYTE byNumberOfCopies = Info.cReceipt1.cInfoData.byNumberOfCopies;

					if (lFlags & RECEIPT_ORIGINAL_PRINTED)
					{
						PAY_AT_PUMP_INFO	PAPInfo;
						CPumpTransact		Trs;
						CInfoData_			cInfoData;  //4.0.3.29

						BYTE byIndex = Info.cReceipt1.Index.byIndex;

						memcpy(&cInfoData, &Info.cReceipt1.cInfoData, sizeof(CInfoData_));

						_Module.m_server.ConvertReceiptToOla(&Info, &Trs, &PAPInfo);
						PAPInfo.CardSaleExtraData2.sReceiptPrinted = '1';
						_Module.m_server.m_cBaseReceipt->ConvertOlaToReceipt(&Trs, &PAPInfo, &Info, 0);		//4.0.26.505 TD 168981

						memcpy(&Info.cReceipt1.cInfoData, &cInfoData, sizeof(CInfoData_));

						Info.cReceipt1.Index.byIndex = byIndex;
						Info.cReceipt2.Index.byIndex = byIndex + 1;
						Info.cReceipt3.Index.byIndex = byIndex + 2;  //4.0.9.500
						Info.cReceipt4.Index.byIndex = byIndex + 3;  //4.0.18.501	//4.0.20.506
						Info.cReceipt5.Index.byIndex = byIndex + 4;  //4.0.18.501	//4.0.20.506
						Info.cReceipt6.Index.byIndex = byIndex + 5;  //4.0.22.501	TD 72716
						Info.cReceipt7.Index.byIndex = byIndex + 6;
					}
					Info.cReceipt1.cInfoData.byNumberOfCopies = byNumberOfCopies + 1;

					CString strLogMsg;
					strLogMsg.Format("ReceiptPrinted__ RECEIPT_INDEX_IS_TRS copy number %ld", (long)byNumberOfCopies);
					_LOGMSG.LogMsg("CGPI", "ReceiptPrinted2", m_lPosNumber, LOG_POS, strLogMsg);

					CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
					lRtc = 1;//PumpSrvDatabase.SaveReceipt(Info) ? OK : 1;
				}
				else
					lRtc = ERR_NOT_FOUND;
			}
			else
			{

				CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
				long lRtc = 1;//PumpSrvDatabase.GetLastReceiptRecord(Info) ? OK : 1;

				if ((lRtc == OK) && !memcmp(&Info.cReceipt1.Index.sTrack2, szTrack2, lTrack2Length))
				{
					BYTE byNumberOfCopies = Info.cReceipt1.cInfoData.byNumberOfCopies;

					CString strLogMsg;
					strLogMsg.Format("ReceiptPrinted__ copy number %ld", (long)byNumberOfCopies);
					_LOGMSG.LogMsg("CGPI", "ReceiptPrinted2", m_lPosNumber, LOG_POS, strLogMsg);

					if (lFlags & RECEIPT_ORIGINAL_PRINTED)
					{
						PAY_AT_PUMP_INFO	PAPInfo;
						CPumpTransact		Trs;
						CInfoData_			cInfoData;		//4.0.3.29

						BYTE byIndex = Info.cReceipt1.Index.byIndex;

						memcpy(&cInfoData, &Info.cReceipt1.cInfoData, sizeof(CInfoData_));

						_Module.m_server.ConvertReceiptToOla(&Info, &Trs, &PAPInfo);
						PAPInfo.CardSaleExtraData2.sReceiptPrinted = '1';
						_Module.m_server.m_cBaseReceipt->ConvertOlaToReceipt(&Trs, &PAPInfo, &Info, 0);//4.0.9.503	

						memcpy(&Info.cReceipt1.cInfoData, &cInfoData, sizeof(CInfoData_));

						Info.cReceipt1.Index.byIndex = byIndex;
						Info.cReceipt2.Index.byIndex = byIndex + 1;
						Info.cReceipt3.Index.byIndex = byIndex + 2;  //4.0.9.500
						Info.cReceipt4.Index.byIndex = byIndex + 3;  //4.0.18.501	//4.0.20.506
						Info.cReceipt5.Index.byIndex = byIndex + 4;  //4.0.18.501	//4.0.20.506
						Info.cReceipt6.Index.byIndex = byIndex + 5;  //4.0.22.501	TD 72716
						Info.cReceipt7.Index.byIndex = byIndex + 6;
					}

					Info.cReceipt1.cInfoData.byNumberOfCopies = byNumberOfCopies + 1;

					CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
					lRtc = 1;//PumpSrvDatabase.SaveReceipt(Info) ? OK : 1;
				}
				else
					lRtc = ERR_NOT_FOUND;
			}
		}
	}

	if (LOG_BASIC_FLOW_CONTROL)
	{//4.0.20.501 TD 60236
		char szResult[MAX_FIELD_VAL] = { 0 };
		_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, szTrack2, iLen);

		CString	str;
		str.Format("ReceiptPrinted, Data=%s,  lIndex=%ld, lFlags=%ld lRtc=%d", szResult, lIndex, lFlags, lRtc);

		_LOGMSG.LogMsg("CGPI", "ReceiptPrinted2", m_lPosNumber, LOG_POS, str);
	}

	if (((lRtc == ERR_NOT_FOUND) || (lRtc == Q_ERROR)) && (!_Module.m_server.GetIFSFTerminalSrvInUse())) //4.0.19.504 - 47559
	{
		retCode = GPI_FAILED_IO;
	}

	if (pVal != NULL)
		*pVal = retCode;
}


void CGPI::GetReceipt2__(long lPosNumber, BSTR sTrack2, long lIndex, BSTR *sReceiptData, long lFlags, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	long		lRtc = 0;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	lRtc = retCode;

	if (retCode == GPI_OK)
	{

		_Module.m_server.m_cCommonFunction.GetReceipt2(lPosNumber, sTrack2, lIndex, sReceiptData, lFlags, m_lPosNumber, &lRtc);

	}

	if (lRtc)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetReceipt2",
				TRUE, lRtc,
				FALSE, 0);


	if (pVal != NULL)
		*pVal = lRtc;

}

STDMETHODIMP CGPI::QueryReceipt(long lPosNumber, BSTR sTrack2, short *nIndex, short nFlags, BSTR *pData, long *lRetVal)
{
	LOG_EXTERNALCALL("CGPI::QueryReceipt   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		QueryReceipt__(lPosNumber, sTrack2, nIndex, nFlags, pData, lRetVal);

#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("GPI::QueryReceipt")) //4.0.15.450
	{

		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "QueryReceipt()", lPosNumber, LOG_PUMP, "", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}

/******************************************************************************
Description:	Quey Receipt.qdx for old receipts  using Track2 values as the search index.
Returns:      	void
Parameters:   	long lPosNumber, BSTR sTrack2, short *nIndex, short nFlags, BSTR *pData, long *lRetVal
Assumptions:   none

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Eyal			3/2/2002   11:30		Start
Eyal           2/6/2002   14:30        Modify
marik			28/06/2003 15:23        support three ola records.
******************************************************************************/

//The following modifications have been made
//1.  Added support for REC_READ_NEXT
//2.  Changed flow, so that index returned is always even, i.e. index
//    of the FIRST half of the record, instead of index of the second half.
void CGPI::QueryReceipt__(long lPosNumber, BSTR sReceiptKey, short *nIndex, short nFlags, BSTR *pData, long *lRetVal)
{
	GPIRetCode	retCode = GPI_OK;
	long		lRtc = 0;
	int         i = 0;
	short		lPosIndex = 0;
	long		lOlaRecordSize = (QUERY_RECEIPT_THREE_OLA_REC & nFlags) ? sizeof(TAG_OLA_REC5_EX) : sizeof(TAG_OLA_REC11);
	short		nOpCode;

	TAG_OLA_REC11 tagRec;
	memset(&tagRec, ' ', sizeof(tagRec));

	nOpCode = (nFlags & REC_READ) | (nFlags & REC_READ_FIRST) |
		(nFlags & REC_READ_SEQ) | (nFlags & REC_READ_NEXT);

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	if (!(nOpCode))
		retCode = GPI_NOT_SUPPORTED;

	if (!pData)
		retCode = GPI_FAILED_IO;

	if (retCode == GPI_OK)
	{
		if (!_Module.m_server.GetIFSFTerminalSrvInUse())
		{
			CReceiptInfo cInfo;
			cInfo.m_TrsIndex = *nIndex;
			lRtc = CSQLReceiptDB::GetInstance().OperationEx((eFileOperation)nOpCode, cInfo);
			if (lRtc == OK)
			{
				PAY_AT_PUMP_INFO cPAPInfo;
				CPumpTransact cTrsInfo;
				_Module.m_server.ConvertReceiptToOla(cInfo, &cTrsInfo, &cPAPInfo);

				tagRec.tagOlaRec10.cardData = cPAPInfo.CardSaleInfo.cardData;
				tagRec.tagOlaRec10.extraData = cPAPInfo.CardSaleInfo.extraData;
				cTrsInfo.GetTag(&tagRec.tagOlaRec10.tagTrs, 2);
				if (tagRec.tagOlaRec10.cardData.sTranType == CD_TYPE_AVI)
				{
					memcpy((char*)tagRec.outputData.bReceiptKey, tagRec.tagOlaRec10.cardData.sFullVehicleId, sizeof(tagRec.tagOlaRec10.cardData.sFullVehicleId));
					_LOGMSG.LogMsg("CGPI", "QueryReceipt", m_lPosNumber, LOG_POS, "Car registration Transaction 1 !");
				}
				else
					_Module.m_server.m_cBaseReceipt->GetReceiptIndex((char*)tagRec.outputData.bReceiptKey, &cPAPInfo);		//4.0.26.509

				if (lRtc == OK)
					*nIndex = cInfo.m_TrsIndex;
			}
		}
		else  //4.0.19.504 - 47559
		{
			char	szReceiptKey[ENCRYPTED_DATA_SIZE + 1];
			memset(szReceiptKey, 0, sizeof(szReceiptKey));
			int iLen = Bstr2Buffer(sReceiptKey, (char *)szReceiptKey, ENCRYPTED_DATA_SIZE);
			retCode = GetReceiptFromTerminalSrv(tagRec, nFlags, szReceiptKey, nIndex, iLen);

		}

		if (lRtc)
			retCode = GPI_NO_RECEIPT;
	}

	if (pData)
	{
		BYTE *sTmpAllocationBuffer = new BYTE[lOlaRecordSize + 2];
		Str2StrNull((BYTE*)&tagRec, lOlaRecordSize, sTmpAllocationBuffer);
		CComBSTR sReceiptData(lOlaRecordSize + 2, (LPCSTR)sTmpAllocationBuffer);

		CString str;
		str.Format("query receipt len is =%ld", sReceiptData.Length());
		_LOGMSG.LogMsg("CGPI", "QueryReceipt", m_lPosNumber, LOG_POS, str);

		*pData = sReceiptData.Copy();
		delete[]sTmpAllocationBuffer; //4.0.5.0
	}


	if (LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			m_lPosNumber, // Src
			"IGPI",
			"QueryReceipt",
			TRUE, retCode,
			FALSE, 0);

	if (lRetVal)
		*lRetVal = retCode;

}

//Eyal 4.0.0.25 THIS FUNCTION CHANGED, REAL SERVER SWITCHING
//CODE MOVED TO CServerMain::SwitchServer__(long).
//THIS FUNCTION ONLY CHECKS PARAMETER VALIDITY.
void CGPI::SwitchServer__(long lNewServerType, long *plRetVal)
{
	long lRetCode = GPI_OK, lTmpSrvType = lNewServerType;
	BOOL bNewServerRemote = lNewServerType & IS_OLA_REMOTE;
	BYTE sRemoteServerName[32];

	memset((BYTE*)sRemoteServerName, 0, sizeof(sRemoteServerName));

	HKEY hKey;
	DWORD dwSize = sizeof(sRemoteServerName), dwType;
	long lErr = 0;

	if (bNewServerRemote)
	{
		lErr = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			"SOFTWARE\\Positive\\System\\Servers\\PSRemote_OLA",
			0,
			KEY_ALL_ACCESS,
			&hKey);

		lErr = RegQueryValueEx(hKey,
			"MasterHost",
			0,
			&dwType,
			sRemoteServerName,
			&dwSize);

		if (!strcmp((char*)sRemoteServerName, "noserver"))
			lRetCode = GPI_FAILED_IO;

		//make M.S. bit zero
		lTmpSrvType = lNewServerType ^ IS_OLA_REMOTE;
	}

	if (bNewServerRemote && (lTmpSrvType != OLASRV_POSITIVE))
		lRetCode = GPI_NOT_SUPPORTED;


	if (_Module.m_server.GetOLAServiceStatus() == SERVICE_IN_MIDDLE_OF_OPERATION)
	{
		lRetCode = GPI_TRS_ALREADY_IN_PROGRESS;
		_LOGMSG.LogMsg("CGPI", "SwitchServer", m_lPosNumber, LOG_POS, "SwitchServer, called while OLA in session");
	}

	//IMPORTANT
	//The different types of OLA are defined as
	//sequential numbers, 0 through 7.
	//If a new OLA provider has been added, be sure 
	//to update this.
	if ((lTmpSrvType < OLASRV_NOT_CONFIGURED) ||
		(lTmpSrvType > OLASRV_POSITIVE_32_EVENTS))
	{
		lRetCode = GPI_FAILED_IO;
		_LOGMSG.LogMsg("CGPI", "SwitchServer", m_lPosNumber, LOG_POS, "SwitchServer, called with invalid server type");
	}

	//If all is well, post a request to switch to new server.
	//This request will be picked up by CmdTx.
	if (lRetCode == GPI_OK)
		_Module.m_server.SetNewOLAServer(lNewServerType);

	if (plRetVal)
		*plRetVal = lRetCode;

	return;
}


STDMETHODIMP CGPI::SwitchServer(long lNewServerType, long *plRetVal)
{
	LOG_EXTERNALCALL("CGPI::SwitchServer   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		SwitchServer__(lNewServerType, plRetVal);

#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("GPI::SwitchServer")) //4.0.15.450
	{
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SwitchServer", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);
	}
#endif	

	return S_OK;
}

STDMETHODIMP CGPI::GetSingleProductInfo(long lPump, long lFlags, long *lGrade, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetSingleProductInfo   start");
	// TODO: Add your implementation code here
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		GetSingleProductInfo__(lPump, lFlags, lGrade, pVal);

#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("GPI::GetSingleProductInfo")) //4.0.15.450
	{

		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetSingleProductInfo", lPump, LOG_PUMP, "", LOG_LEVEL_0);
	}
#endif
	return S_OK;
}

void CGPI::GetSingleProductInfo__(long lPump, long lFlags, long *lGrade, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (PUMP_NOT_VALID(lPump))
		retCode = GPI_INVALID_PUMP_NUMBER;

	if (retCode == GPI_OK)
	{
		long lTmpGrade = 0;
		long lLastGrade = 0;
		*lGrade = 0;

		for (int i = 1; i <= MAX_POSITIONS_PER_PUMP; i++)
		{
			lTmpGrade = _Module.m_server.m_cPumpArray[lPump - 1].NozzleToGrade(i);

			if (lTmpGrade)
			{
				if (lLastGrade)
				{
					if (lTmpGrade != lLastGrade)
						retCode = GPI_PARAM_NOT_FOUND;
				}
				else
				{
					lLastGrade = lTmpGrade;
					retCode = GPI_OK;
				}
			}
		}

		if (retCode == GPI_OK)
		{
			*lGrade = lLastGrade;

			//4.0.26.501 - TD 116227
			//Make extended grade conversion if needed
			if ((_Module.m_server.GetMaxGrade999()) && (lLastGrade > 0))
				_Module.m_server.GetExtGradeByGrade(lLastGrade, lGrade, lPump);
		}
	}
	if (pVal != NULL)
		*pVal = retCode;
}

/******************************************************************************
Description:	Get Alarm Information by Index
Returns:      	GPI ret code
Parameters:   long lCategory, long lSubCategory, long *plAlarmId, long lFlags, BSTR *pData, long *plVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Marik			17/11/2002   16:26		Start
******************************************************************************/

STDMETHODIMP CGPI::GetAlarmInfo(long lCategory, long lSubCategory, long lDevice, long *plAlarmId, long lFlags, BSTR *pData, long *plVal)
{
	LOG_EXTERNALCALL("CGPI::GetAlarmInfo   start");

	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		GetAlarmInfo__(lCategory, lSubCategory, lDevice, plAlarmId, lFlags, pData, plVal);

#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("GPI::GetAlarmInfo")) //4.0.15.450
	{

		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetAlarmInfo", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}
/******************************************************************************
Description:	Get Alarm Information by Index
Returns:      	GPI ret code
Parameters:   long lCategory, long lSubCategory, long *plAlarmId, long lFlags, BSTR *pData, long *plVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Marik			17/11/2002   16:26		Start
******************************************************************************/
void CGPI::GetAlarmInfo__(long lCategory, long lSubCategory, long lDevice, long *plAlarmId, long lFlags, BSTR *pData, long *plVal)
{
	GPIRetCode retCode = GPI_OK;
	long lRetVal = 0;

	if (pData == NULL)
		retCode = GPI_PARAM_NOT_FOUND;

	if (retCode == GPI_OK)
	{
		if (_Module.m_server.GetAlarmSrvMode())
		{
			try
			{
				lRetVal = _ALARM_SRV.m_pGAI->GetAlarm(lCategory, lSubCategory, lDevice, plAlarmId, lFlags, pData);
				if (lRetVal)
					retCode = GPI_PARAM_NOT_FOUND;

			}
			catch (_com_error & e)
			{
				retCode = GPI_ALARMSRV_FAIL_RETRIEVED_DATA;

				_LOGMSG.LogResult(
					0, // Src
					"IGPI",
					"GetAlarmInfo",
					FALSE, 0,
					TRUE, e.Error(), e);

				_ALARM_SRV.Restore_COM();


			}
			catch (...)
			{
				retCode = GPI_ALARMSRV_FAIL_RETRIEVED_DATA;

				if (LOG_LOGIC_ERROR)
					_LOGMSG.LogResult(
						0, // Src
						"IGPI",
						"GetAlarmInfo",
						TRUE, 0,
						(lRetVal != 0) ? TRUE : FALSE, lRetVal);

				_ALARM_SRV.Restore_COM();
			}
		}
		else
		{
			retCode = GPI_ALARMSRV_NOT_ACTIVE;
		}
	}
	if (retCode != GPI_ALARMSRV_NOT_ACTIVE)
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("GetAlarmInfo, lCategory=%ld, lSubCategory=%ld,lDevice=%ld, plAlarmId=%ld , lFlags=%ld, lRetVal=%ld, GPI_Ret=%d",
				lCategory, lSubCategory, lDevice, *plAlarmId, lFlags, lRetVal, retCode);

			_LOGMSG.LogMsg("CGPI", "GetAlarmInfo", m_lPosNumber, LOG_POS, str);
		}

	if (LOG_LOGIC_ERROR && retCode)
		_LOGMSG.LogResult(
			m_lPosNumber, // Src
			"IGPI",
			"GetAlarmInfo",
			TRUE, retCode,
			TRUE, lRetVal);


	if (plVal != NULL)
		*plVal = retCode;

}

/******************************************************************************
Description:	Confirm accepted alarm.
Returns:      	GPI ret code
Parameters:   	long lCategory, long lSubCategory, long lAlarmId, long lDevice, long lAlarmNumber, long lCustomerId, long lFlags, BSTR sData, long *plVal)

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Marik			17/11/2002   16:27		Start
******************************************************************************/
STDMETHODIMP CGPI::AckAlarm(long lCategory, long lSubCategory, long lAlarmId, long lDevice, long lAlarmNumber, long lCustomerId, long lFlags, BSTR sData, long *plVal)
{
	LOG_EXTERNALCALL("CGPI::AckAlarm   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		AckAlarm__(lCategory, lSubCategory, lAlarmId, lDevice, lAlarmNumber, lCustomerId, lFlags, sData, plVal);

#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("CGPI::AckAlarm"))
	{

		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "AckAlarm", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}
/******************************************************************************
Description:	Confirm accepted alarm.
Returns:      	GPI ret code
Parameters:   	long lCategory, long lSubCategory, long lAlarmId, long lDevice, long lAlarmNumber, long lCustomerId, long lFlags, BSTR sData, long *plVal)

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Marik			17/11/2002   16:27		Start
******************************************************************************/

void CGPI::AckAlarm__(long lCategory, long lSubCategory, long lAlarmId, long lDevice, long lAlarmNumber, long lCustomerId, long lFlags, BSTR sData, long *plVal)
{
	GPIRetCode retCode = GPI_OK;
	long lRetVal = 0;


	if (_Module.m_server.GetAlarmSrvMode())
	{
		try
		{
			lRetVal = _ALARM_SRV.m_pGAI->AckAlarm(lCategory, lSubCategory, lAlarmId, lDevice, lAlarmNumber, lCustomerId, lFlags, sData);

			if (lRetVal)
				retCode = GPI_ALARMSRV_FAIL_RETRIEVED_DATA;
		}
		catch (_com_error & e)
		{
			retCode = GPI_ALARMSRV_FAIL_RETRIEVED_DATA;

			_LOGMSG.LogResult(
				0, // Src
				"IGPI",
				"GetAlarmInfo",
				FALSE, 0,
				TRUE, e.Error(), e);

			_ALARM_SRV.Restore_COM();

		}
		catch (...)
		{
			retCode = GPI_ALARMSRV_FAIL_RETRIEVED_DATA;

			if (LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"IGPI",
					"GetAlarmInfo",
					TRUE, 0,
					(lRetVal != 0) ? TRUE : FALSE, lRetVal);

			_ALARM_SRV.Restore_COM();
		}
	}
	else
	{
		retCode = GPI_ALARMSRV_NOT_ACTIVE;
	}
	if (retCode != GPI_ALARMSRV_NOT_ACTIVE)
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("AckAlarm, lCategory=%ld, lSubCategory=%ld, lDevice=%ld lFlags=%ld, lRetVal=%ld, GPI_Ret=%d",
				lCategory, lSubCategory, lDevice, lFlags, lRetVal, retCode);
			_LOGMSG.LogMsg("CGPI", "AckAlarm", m_lPosNumber, LOG_POS, str);
		}


	if (plVal != NULL)
		*plVal = retCode;

}
/******************************************************************************
Description:	Update mode scheme information
Returns:      	GPI ret code
Parameters:   	long lPumpNumber, long lScheme, long lMode, BSTR sPeriodDateTimeStart, BSTR sPeriodDateTimeEnd, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lServiceFeeNo, long lFlags, long *pVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Marik			28/11/2002   16:27		Start
******************************************************************************/

STDMETHODIMP CGPI::SetPumpModeScheme3(long lPumpNumber, long lScheme, long lMode, BSTR sPeriodDateTimeStart, BSTR sPeriodDateTimeEnd, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lServiceFeeNo, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetPumpModeScheme3   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		SetPumpModeScheme3__(lPumpNumber, lScheme, lMode, sPeriodDateTimeStart, sPeriodDateTimeEnd, sDateTimeStart, sDateTimeEnd, lServiceFeeNo, lFlags, pVal);
#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("CGPI::SetPumpModeScheme3"))
	{

		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetPumpModeScheme3", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}

void CGPI::SetPumpModeScheme3__(long lPumpNumber, long lScheme, long lMode, BSTR sPeriodDateTimeStart, BSTR sPeriodDateTimeEnd, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lServiceFeeNo, long lFlags, long *pVal)
{
	USES_CONVERSION;
	GPIRetCode retCode = GPI_OK;

	if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	else if (MODE_NOT_VALID(lMode))
		retCode = GPI_INVALID_PUMP_MODE;

	else if (SCHEME_NOT_VALID(lScheme))
		retCode = GPI_INVALID_SCHEME_NUMBER;


	if (retCode == CTRL_OK)
	{
		_Module.m_server.SetPumpModeScheme3(lPumpNumber, lScheme, lMode, sPeriodDateTimeStart, sPeriodDateTimeEnd, sDateTimeStart, sDateTimeEnd, lServiceFeeNo, lFlags, pVal);
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGPI",
				"SetPumpModeScheme3",
				TRUE, retCode,
				FALSE, 0);
	if (pVal != NULL)
		*pVal = retCode;
}

/******************************************************************************
Description:	retrieved mode scheme information
Returns:      	GPI ret code
Parameters:   	long lPumpNumber, long lScheme, long *plMode, BSTR *psPeriodDateTimeStart, BSTR *psPeriodDateTimeEnd, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plServiceFeeNo, long *plFlags, long *pVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Marik			28/11/2002   16:27		Start
******************************************************************************/
STDMETHODIMP CGPI::GetPumpModeScheme3(long lPumpNumber, long lScheme, long *plMode, BSTR *psPeriodDateTimeStart, BSTR *psPeriodDateTimeEnd, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plServiceFeeNo, long *plFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPumpModeScheme3   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		GetPumpModeScheme3__(lPumpNumber, lScheme, plMode, psPeriodDateTimeStart, psPeriodDateTimeEnd, psDateTimeStart, psDateTimeEnd, plServiceFeeNo, plFlags, pVal);

#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("CGPI::GetPumpModeScheme3"))
	{

		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPumpModeScheme3", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);
	}
#endif
	return S_OK;
}

void CGPI::GetPumpModeScheme3__(long lPumpNumber, long lScheme, long *plMode, BSTR *psPeriodDateTimeStart, BSTR *psPeriodDateTimeEnd, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plServiceFeeNo, long *plFlags, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	else if (SCHEME_NOT_VALID(lScheme))
		retCode = GPI_INVALID_SCHEME_NUMBER;


	if (retCode == CTRL_OK)
	{
		_Module.m_server.GetPumpModeScheme3(lPumpNumber, lScheme, plMode, psPeriodDateTimeStart, psPeriodDateTimeEnd, psDateTimeStart, psDateTimeEnd, plServiceFeeNo, plFlags, pVal);
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGPI",
				"GetPumpModeScheme3",
				TRUE, retCode,
				FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}


GPIRetCode CGPI::GetAppropriateRetCode(long lServerState)
{
	GPIRetCode retCode = GPI_OK;

	if (lServerState == SERVER_STARTUP)
	{
		//Added MultiSync check
		if ((PUMPSRV_IS_MASTER) && (!_HOST.m_lHydraActive) && (_Module.m_server.IsRedundancyByCL() != TRUE))		//4.0.23.508
			retCode = GPI_SYSTEM_DOWN_PAK_ENABLED;

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (lServerState != SERVER_IDLE)
	{
		//Added MultiSync check
		if ((PUMPSRV_IS_MASTER) && (!_HOST.m_lHydraActive) && (_Module.m_server.IsRedundancyByCL() != TRUE))		//4.0.23.508
			retCode = GPI_SYSTEM_DOWN_PAK_ENABLED;

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	return retCode;
}

STDMETHODIMP CGPI::GetAlarmByCategory(long lCategory, long lSubCategory, long lDevice, long lFlags, long *plTotalAlarms, BSTR *pData, long *plVal)
{
//	LOG_EXTERNALCALL("CGPI::GetAlarmByCategory   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		GetAlarmByCategory__(lCategory, lSubCategory, lDevice, lFlags, plTotalAlarms, pData, plVal);

#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("CGPI::GetAlarmByCategory"))
	{

		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetAlarmByCategory", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}

void CGPI::GetAlarmByCategory__(long lCategory, long lSubCategory, long  lDevice, long lFlags, long *plTotalAlarms, BSTR *pData, long *plVal)
{
	GPIRetCode retCode = GPI_OK;
	long lRetVal = 0;
	*plTotalAlarms = 0;

	if (LOG_FCC_FLOW_CONTROL) //4.0.16.504 - merge from 4.0.16.11
	{
		CString str;
		str.Format("Request Alarm By Category, lCategory=%ld, lSubCategory=%ld, lDevice=%ld, lFlags=%ld",
			lCategory, lSubCategory, lDevice, lFlags);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, str);
	}

	if (_Module.m_server.GetAlarmSrvMode())
	{
		try
		{
			lFlags = GABC_ONLY_DISPLAY_MESSAGES; //4.0.8.506

			if (lCategory == PUMPSRV_SYSTEM_GROUP)
			{

				_bstr_t  bstrContainer;
				BOOL	 bElementFound = FALSE;
				long	 lTotalElement = 0;

				/***************************************/
				/********* 	CATEGORY_SOFTWARE **********/
				/***************************************/
				//We do not want the OLA alarm : ALARM 0301013	//1.0.19.506 (TD 61048,61107,47892)
				long lTmpFlag = GABC_ONLY_DISPLAY_MESSAGES | GABC_WITHOUT_SOFTWARE_GENERAL_OLA_ERR;

				BSTR bstrElement1 = NULL;
				*plTotalAlarms = 0;
				lRetVal = _ALARM_SRV.m_pGAI->GetAlarmByCategory(CATEGORY_SOFTWARE, lSubCategory, lDevice, lTmpFlag, plTotalAlarms, &bstrElement1);
				_bstr_t bstrTmp1(bstrElement1, false);

				if ((*plTotalAlarms > 0) && (!lRetVal))
				{
					bstrContainer += bstrTmp1;
					bElementFound = TRUE;
					lTotalElement += *plTotalAlarms;
				}


				/**************************************/
				/********* 	CATEGORY_DEVICES **********/
				/**************************************/

				BSTR bstrElement2 = NULL;
				*plTotalAlarms = 0;
				lRetVal = _ALARM_SRV.m_pGAI->GetAlarmByCategory(CATEGORY_DEVICES, lSubCategory, lDevice, lFlags, plTotalAlarms, &bstrElement2);
				_bstr_t bstrTmp2(bstrElement2, false);

				if ((*plTotalAlarms > 0) && (!lRetVal))
				{
					bstrContainer += bstrTmp2;
					bElementFound = TRUE;
					lTotalElement += *plTotalAlarms;
				}


				/**************************************/
				/********* 	CATEGORY_CONTROLLER *******/
				/**************************************/
				BSTR bstrElement3 = NULL;
				*plTotalAlarms = 0;
				lRetVal = _ALARM_SRV.m_pGAI->GetAlarmByCategory(CATEGORY_CONTROLLER, lSubCategory, lDevice, lFlags, plTotalAlarms, &bstrElement3);
				_bstr_t bstrTmp3(bstrElement3, false);

				if ((*plTotalAlarms > 0) && (!lRetVal))
				{
					bstrContainer += bstrTmp3;
					bElementFound = TRUE;
					lTotalElement += *plTotalAlarms;
				}

				if (bElementFound)
				{
					lRetVal = GPI_OK;
					*plTotalAlarms = lTotalElement;
					*pData = bstrContainer.copy();
				}

			}
			else
			{
				/**************************************/
				/********* 	FREE CATEGORY *************/
				/**************************************/
				lRetVal = _ALARM_SRV.m_pGAI->GetAlarmByCategory(lCategory, lSubCategory, lDevice, lFlags, plTotalAlarms, pData);
			}

			if (lRetVal)
				retCode = GPI_ALARMSRV_FAIL_RETRIEVED_DATA;
		}
		catch (_com_error & e)
		{
			retCode = GPI_ALARMSRV_FAIL_RETRIEVED_DATA;

			_LOGMSG.LogResult(
				0, // Src
				"IGPI",
				"GetAlarmByCategory",
				FALSE, 0,
				TRUE, e.Error(), e);

			_ALARM_SRV.Restore_COM();

		}
		catch (...)
		{
			retCode = GPI_ALARMSRV_FAIL_RETRIEVED_DATA;

			if (LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"IGPI",
					"GetAlarmByCategory",
					TRUE, 0,
					(lRetVal != 0) ? TRUE : FALSE, lRetVal);

			_ALARM_SRV.Restore_COM();
		}
	}
	else
	{
		retCode = GPI_ALARMSRV_NOT_ACTIVE;
	}

	if (retCode != GPI_ALARMSRV_NOT_ACTIVE)
		if (LOG_DETAIL_FLOW_CONTROL  && retCode)
		{
			CString str;
			str.Format("GetAlarmByCategory, lCategory=%ld, lSubCategory=%ld, lDevice=%ld, lFlags=%ld, lRetVal=%ld, GPI_Ret=%d",
				lCategory, lSubCategory, lDevice, lFlags, lRetVal, retCode);
			_LOGMSG.LogMsg("CGPI", "GetAlarmByCategory", m_lPosNumber, LOG_POS, str);
		}


	if (plVal != NULL)
		*plVal = retCode;

}


STDMETHODIMP CGPI::GetAlarmById(long lAlarmId, long lFlags, BSTR *pData, long *plVal)
{
	LOG_EXTERNALCALL("CGPI::GetAlarmById   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		GetAlarmById__(lAlarmId, lFlags, pData, plVal);

#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("CGPI::GetAlarmById"))
	{

		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetAlarmById", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);
	}
#endif
	return S_OK;
}

void CGPI::GetAlarmById__(long lAlarmId, long lFlags, BSTR *pData, long *plVal)
{
	GPIRetCode retCode = GPI_OK;
	long lRetVal = 0;

	if (pData == NULL)
		retCode = GPI_PARAM_NOT_FOUND;

	if (retCode == GPI_OK)
	{
		if (_Module.m_server.GetAlarmSrvMode())
		{
			try
			{
				lRetVal = _ALARM_SRV.m_pGAI->GetAlarmById(lAlarmId, lFlags, pData);

				if (lRetVal)
					retCode = GPI_PARAM_NOT_FOUND;

			}
			catch (_com_error & e)
			{
				retCode = GPI_ALARMSRV_FAIL_RETRIEVED_DATA;

				_LOGMSG.LogResult(
					0, // Src
					"IGPI",
					"GetAlarmById",
					FALSE, 0,
					TRUE, e.Error(), e);

				_ALARM_SRV.Restore_COM();


			}
			catch (...)
			{
				retCode = GPI_ALARMSRV_FAIL_RETRIEVED_DATA;

				if (LOG_LOGIC_ERROR)
					_LOGMSG.LogResult(
						0, // Src
						"IGPI",
						"GetAlarmById",
						TRUE, 0,
						(lRetVal != 0) ? TRUE : FALSE, lRetVal);

				_ALARM_SRV.Restore_COM();
			}
		}
		else
		{
			retCode = GPI_ALARMSRV_NOT_ACTIVE;
		}
	}
	if (retCode != GPI_ALARMSRV_NOT_ACTIVE)
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("GetAlarmById, lAlarmId=%ld,  lFlags=%ld, lRetVal=%ld, GPI_Ret=%d",
				lAlarmId, lFlags, lRetVal, retCode);

			_LOGMSG.LogMsg("CGPI", "GetAlarmById", m_lPosNumber, LOG_POS, str);
		}

	if (LOG_LOGIC_ERROR && retCode)
		_LOGMSG.LogResult(
			m_lPosNumber, // Src
			"IGPI",
			"GetAlarmById",
			TRUE, retCode,
			TRUE, lRetVal);


	if (plVal != NULL)
		*plVal = retCode;

}

/******************************************************************************
Description:
Returns:
Parameters:   	sData - a buffer containing lRecCount TAG_PUMP_TRANSACT2 appended
one after the other
lRecCount - number of TAG_PUMP_TRANSACT2 in sData
lFlags - reserved for future use, set to 0
plRetVal - a GPI return code
Assumptions:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Eyal			20/4/2003   15:17		Start
******************************************************************************/
void CGPI::GetPAKQuery__(BSTR * sData, long * plRecCount, long lFlags, long *plRetVal)
{
	GetPAKQueryRetCode eRetCode;
	long lRetVal = GPI_OK;


	if (!m_bInitDone)
		lRetVal = GPI_MUST_INIT_FIRST;


	eRetCode = _Module.m_server.m_cCommonFunction.GetPAKQuery(sData, plRecCount, &lFlags);

	switch (eRetCode)
	{
	case GetPakQuery_OK:
		lRetVal = GPI_OK;
		break;
	case GetPakQuery_MissingData:
		lRetVal = GPI_LAST_ERROR;
		break;
	}


	if (LOG_LOGIC_ERROR && lRetVal)
		_LOGMSG.LogResult(
			0, // Src
			"IGPI",
			"GetPAKQuery",
			TRUE, lRetVal,
			FALSE, 0);

	if (plRetVal != NULL)
		*plRetVal = lRetVal;

}

STDMETHODIMP CGPI::GetPAKQuery(BSTR *sData, long *plRecCount, long lFlags, long *plRetVal)
{
	LOG_EXTERNALCALL("CGPI::GetPAKQuery   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		GetPAKQuery__(sData, plRecCount, lFlags, plRetVal);
#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("CGPI::GetPAKQuery"))
	{
		_LOGMSG.LogClassMsg("CGPI", "GetPAKQuery", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}


BOOL CGPI::IsTargetPumpReadyForMovedPrepay(long lTargetPumpIndex)
{
	BOOL bRetVal = TRUE;

	CPumpTransact cTrsOnTargetPump;
	_Module.m_server.m_cPumpArray[lTargetPumpIndex].GetCurrentTrs(&cTrsOnTargetPump);

	//If this is a prepay
	if (cTrsOnTargetPump.m_lRes == PRE_PAY_AT_POS)
	{
		//Or if moved to dispensing, but no fuel has been pumped
		if ((cTrsOnTargetPump.GetStatus() == IN_PROGRESS) &&
			(cTrsOnTargetPump.m_lRoundedValue == 0))
			bRetVal = FALSE;

		else if (cTrsOnTargetPump.GetStatus() == WAIT_TIME_OUT)
			bRetVal = FALSE;
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString sLogMsg;
		sLogMsg.Format("check on pump %d returned %d", lTargetPumpIndex + 1, bRetVal);

		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lTargetPumpIndex, LOG_PUMP, sLogMsg);
	}

	return bRetVal;
}

STDMETHODIMP CGPI::GetPAPTrs4(long lPumpNumber, BSTR *pRec, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPAPTrs4   start");
	// TODO: Add your implementation code here
	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPAPTrs4, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetPAPTrs4__(lPumpNumber, pRec, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetPAPTrs4"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPAPTrs4", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GetPAPTrs4__(long lPumpNumber, BSTR *pRec, long lFlags, long *pVal)
{
	GetPAPTrs3__(lPumpNumber, pRec, pVal, lFlags);
}




/******************************************************************************
Description:	Locking/Unlocking a single pump	exposed function
Returns:
Parameters:    long lPumpNumber, long lLock, long lFlags, long *plVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Udi F			28/01/2004   10:33		Start
******************************************************************************/

STDMETHODIMP CGPI::LockPump(long lPumpNumber, long lLock, long lFlags, long *plVal)
{
	LOG_EXTERNALCALL("CGPI::LockPump   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		LockPump__(lPumpNumber, lLock, lFlags, plVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::LockPump"))
	{
		//We should arrive this point only if an error occurs!
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "LockPump", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);
		exit(1);
	}
#endif 

	return S_OK;
}


/******************************************************************************
Description:	Locking/Unlocking a single pump	inner function
Returns:
Parameters:    long lPumpNumber, long lLock, long lFlags, long *plVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Udi F			28/01/2004   10:33		Start
******************************************************************************/

void CGPI::LockPump__(long lPumpNumber, long lLock, long lFlags, long *plVal)
{
	//We instantiate the return code with GPI_OK by default, and any error
	//which will occur later will be inserted to it. If there's no error- it'll stay
	//with the retCode of GPI_OK till the function's end 
	GPIRetCode retCode = GPI_OK;
	//The command structs which will be sent to the Convert Layer 
	CMD_STRUCT_RESERVE_PUMP cmdReserveSinglePump;
	CMD_STRUCT_RELEASE_PUMP cmdReleaseSinglePump;

	//For getting the pump's status
	CPumpStatus cPumpStatus;
	//A CString used for logging
	CString sLogStr;

	//Zeroing the command structs
	memset(&cmdReserveSinglePump, 0, sizeof(CMD_STRUCT_RESERVE_PUMP));
	memset(&cmdReleaseSinglePump, 0, sizeof(CMD_STRUCT_RELEASE_PUMP));
	memset(&cPumpStatus, 0, sizeof(CPumpStatus));

	cPumpStatus = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();
	//Checking if there's any kind of error...
	if (lLock != MP_LOCK &&  lLock != MP_UNLOCK)
		retCode = GPI_NOT_SUPPORTED;

	else if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);
		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());
		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	else if (PUMP_NOT_IN_WORKING_STATE(lPumpNumber))
		retCode = GPI_PUMP_NOT_IN_WORKING_STATE;

	else if (MP_LOCK == lLock || MP_EMERGENCY_LOCK == lLock)
	{
		//Dont let lock / unlock when we are in Emergency lock !
		if ((_Module.m_server.m_cServerInfo.GetServerReservation() & EMERGENCY_LOCK) && (MP_LOCK == lLock)) //4.0.21.501
		{
			retCode = GPI_PUMP_LOCKED;
			sLogStr.Format("Can't Lock pump while server is in EMERGENCY_LOCK !");
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogStr);
		}

		else if (cPumpStatus.GetRes() == PUMPSRV)
		{
			retCode = GPI_PUMP_ALREADY_RESERVED;
		}
		else if (cPumpStatus.GetStatus() != REQUESTING)
		{
			if ((cPumpStatus.GetStatus() != READY))
				retCode = GPI_CANT_RESERVEPUMP;
			else if ((cPumpStatus.GetRes() != NULL_RES) && (cPumpStatus.GetRes() != PAY_AT_KIOSK))
			{
				retCode = GPI_CANT_RESERVEPUMP;
			}
		}
	}


	if (((_Module.m_server.m_cServerInfo.GetServerReservation() & MAINTENANCE) || (_Module.m_server.m_cServerInfo.GetServerReservation() & DATABASE_MAINTENANCE)) && (cPumpStatus.GetStatus() != REQUESTING)) //4.0.5550.1504
	{
		retCode = GPI_INVALID_PUMP_MODE;
	}


	if (GPI_OK == retCode) //No error
	{
		//Entering the PumpArray Critical Section
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].CanReserve() || cPumpStatus.GetStatus() == REQUESTING)
		{
			if (MP_LOCK == lLock || MP_EMERGENCY_LOCK == lLock)
			{
				//Filling the command of reserving a pump and sending it
				cmdReserveSinglePump.lRes = PUMPSRV;
				cmdReserveSinglePump.lResExt |= PUMP_SINGLE_LOCK;

				_Module.m_server.SetCommand(CMD_RESERVE_PUMP, SERVER_POS_NUMBER, lPumpNumber, &cmdReserveSinglePump, sizeof(CMD_STRUCT_RESERVE_PUMP));

				sLogStr.Format("Sent a command to reserve pump %d\n", lPumpNumber);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogStr);
				//Changing the state to PUMP_WAIT_TO_BE_EXCLUSIVELY_LOCKED in the state machine
				_Module.m_server.m_cSinglePumpLock.SetPumpStateWaitLocked(lPumpNumber);
			}
			else
			{
				if (!_Module.m_server.m_cSinglePumpLock.IsExclusivelyLocked(lPumpNumber))
				{
					retCode = GPI_OK;
				}
				else
				{
					//Filling the command of releasing a pump and sending it
					cmdReleaseSinglePump.lRes = PUMPSRV;
					cmdReleaseSinglePump.lResExt = IDLE;
					_Module.m_server.SetCommand(CMD_RELEASE_PUMP, SERVER_POS_NUMBER, lPumpNumber, &cmdReleaseSinglePump, sizeof(CMD_STRUCT_RELEASE_PUMP));

					sLogStr.Format("Sent a command to release pump %d\n", lPumpNumber);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogStr);

					//Changing the state to PUMP_WAIT_TO_BE_EXCLUSIVELY_UNLOCKED in the state machine
					_Module.m_server.m_cSinglePumpLock.SetPumpStateWaitUnlocked(lPumpNumber);
				}
			}
		}
		//The pump is already reserved
		else
		{
			if (MP_LOCK == lLock || MP_EMERGENCY_LOCK == lLock)
			{
				retCode = GPI_PUMP_ALREADY_RESERVED;
			}
			else
			{
				//Filling the command of releasing a pump and sending it
				cmdReleaseSinglePump.lRes = PUMPSRV;
				cmdReleaseSinglePump.lResExt = IDLE;
				_Module.m_server.SetCommand(CMD_RELEASE_PUMP, SERVER_POS_NUMBER, lPumpNumber, &cmdReleaseSinglePump, sizeof(CMD_STRUCT_RELEASE_PUMP));

				sLogStr.Format("Sent a command to release pump %d\n", lPumpNumber);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogStr);

				//Changing the state to PUMP_WAIT_TO_BE_EXCLUSIVELY_UNLOCKED in the state machine
				_Module.m_server.m_cSinglePumpLock.SetPumpStateWaitUnlocked(lPumpNumber);
			}
		}
	}

	//In case there was an error - we're logging it
	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"LockPump",
				TRUE, retCode,
				FALSE, 0);

	if (plVal != NULL)
		*plVal = retCode;
}


STDMETHODIMP CGPI::GetAccumulateTotals(long lShiftNumber, long lState, long lPosNumber, long lFlags, long lVal, BSTR sStartTime, BSTR sEndTime, long *lRecordCount, BSTR *sRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetAccumulateTotals   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		GetAccumulateTotals__(lShiftNumber, lState, lPosNumber, lFlags, lVal, sStartTime, sEndTime, lRecordCount, sRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetAccumulateTotals"))
	{
		//We should arrive this point only if an error occurs!
		if (LOG_ERROR)
			_LOGMSG.LogMsg2("CGPI", "GetAccumulateTotals", m_lPosNumber, LOG_POS, "GetAccumulateTotals", LOG_LEVEL_0);
		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::GetAccumulateTotals__(long lShiftNumber, long lState, long lPosNumber, long lFlags, long lVal, BSTR sStartTime, BSTR sEndTime, long *lRecordCount, BSTR *sRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;
	DWORD dwLastErr;
	SYSTEMTIME cEndTime, cStartTime;
	long lRtc = 0;


	if (!ACCUMULATE_SHIFT_TOTAL)
		retCode = GPI_ACCUMULATE_SHIFT_TOTAL_NOT_ACTIVE;
	else if (lFlags & GAT_BY_GRADE)
	{
		if ((lVal > MAX_GRADES) || (lVal < 0))
			retCode = GPI_INVALID_GRADE_NUMBER;
	}

	if (retCode == GPI_OK)
	{

		if (lFlags & GAT_BY_DATE)
		{
			SZ_TAG_SYSTEMTIME szTagTime;
			memset(&szTagTime, 0, sizeof(SZ_TAG_SYSTEMTIME));
			int iLen = WideCharToMultiByte(CP_ACP, 0, sStartTime, -1, (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME), NULL, NULL);
			if (iLen == 0)
			{
				dwLastErr = GetLastError();
				retCode = GPI_LAST_ERROR;
			}
			else
			{
				Tag2SystemTime(&szTagTime.sTime, &cStartTime);
			}

			if (retCode == CTRL_OK)
			{
				SZ_TAG_SYSTEMTIME szTagTime;
				memset(&szTagTime, 0, sizeof(SZ_TAG_SYSTEMTIME));
				int iLen = WideCharToMultiByte(CP_ACP, 0, sEndTime, -1, (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME), NULL, NULL);
				if (iLen == 0)
				{
					dwLastErr = GetLastError();
					retCode = GPI_LAST_ERROR;
				}
				else
				{
					Tag2SystemTime(&szTagTime.sTime, &cEndTime);
				}
			}
		}

		if (retCode == CTRL_OK)
		{
			_Module.m_server.m_cCommonFunction.GetReport(lShiftNumber, lState, lPosNumber, lFlags, lVal, cStartTime, cEndTime, lRecordCount, *sRec);

			if (!(*lRecordCount))
				retCode = GPI_RECORD_NOT_FOUND;
		}

	}

	_LOGMSG.LogMsg("CGPI", "GetAccumulateTotals", m_lPosNumber, LOG_POS, "New version GPI");
	if (LOG_LOGIC_ERROR)
	{
		CString sMsg;
		sMsg.Format("GetAccumulateTotals, lPosNumber=%ld ,ShiftNumber=%ld,lState=%ld,lPosNumber=%ld, lFlags=%ld,lVal=%ld , total record=%ld",
			lPosNumber, lShiftNumber, lState, lPosNumber, lFlags, lVal, *lRecordCount);
		_LOGMSG.LogMsg("CGPI", "GetAccumulateTotals", m_lPosNumber, LOG_POS, sMsg);
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGPI",
				"GetAccumulateTotals",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE, lRtc);
	if (pVal)
		*pVal = retCode;

}

STDMETHODIMP CGPI::SetAccumulateState(long lShiftNumber, long lState, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetAccumulateState   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		SetAccumulateState__(lShiftNumber, lState, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::SetAccumulateState"))
	{
		//We should arrive this point only if an error occurs!
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetAccumulateState", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);
		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::SetAccumulateState__(long lShiftNumber, long lState, long lFlags, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (_Module.m_server.m_cCommonFunction.SetRecordStatus(lShiftNumber, lState, m_lPosNumber, lFlags))
		retCode = GPI_RECORD_NOT_FOUND;

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGPI",
				"SetAccumulateState",
				TRUE, retCode,
				(0) ? TRUE : FALSE, 0);

	if (pVal)
		*pVal = retCode;
}


/******************************************************************************
Description:	Setting the state of a Prepayed transaction	exposed function
Returns:
Parameters:    long lPumpNumber, long lSyncNumber, long lStatus, long lFlags, long *plVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Udi F			10/05/2004   11:56		Start
******************************************************************************/
STDMETHODIMP CGPI::SetPrePayTrsState(long lPumpNumber, long lSyncNumber, long lStatus, long lFlags, long *plVal)
{
	LOG_EXTERNALCALL("CGPI::SetPrePayTrsState   start");

#ifdef CATCH_GPI
	__try
	{
#endif
		SetPrePayTrsState_(lPumpNumber, lSyncNumber, lStatus, lFlags, plVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::SetPrePayTrsState"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetPrePayTrsState()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

/******************************************************************************
Description:	Setting the state of a Prepayed transaction	to NULL or to be owned
by a new POS
Returns:
Parameters:    long lPumpNumber, long lSyncNumber, long lStatus, long lFlags, long *plVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Udi F			10/05/2004   11:57		Start
******************************************************************************/

void CGPI::SetPrePayTrsState_(long lPumpNumber, long lSyncNumber, long lStatus, long lFlags, long *plVal)
{
	GPIRetCode retCode = GPI_OK;
	long lRtc = 0;
	CPumpTransact	trs;
	PRE_PAY_INFO	info;

	memset(&trs, 0, sizeof(CPumpTransact));
	memset(&info, 0, sizeof(PRE_PAY_INFO));

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GPI_FAILED_IO;
	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);
		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}
	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());
		else
			retCode = GPI_SERVER_NOT_READY;
	}

	if (lPumpNumber)
	{
		if (PUMP_NOT_VALID(lPumpNumber))
			retCode = GPI_INVALID_PUMP_NUMBER;
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("Pump %02d, Sync Number %d, Status %d, lFlags %d", lPumpNumber, lSyncNumber, lStatus, lFlags);
		_LOGMSG.LogReq(m_lPosNumber,
			"IGPI",
			"SetPrePayTrsState",
			(LPCSTR)str);
	}

	if (retCode == GPI_OK)
	{
		trs.SetLink(TRS_LINK_PREPAY_NUMBER, lSyncNumber);

		lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &trs, &info);

		if (lRtc == OK)
		{
			switch (lStatus)
			{
			case PREPAY_STATE_ON_OFFER:
				//Setting us as the PrePay deal owner
				info.nOwningPos = (short)m_lPosNumber;
				lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &trs, &info);
				break;

			case PREPAY_STATE_FORCE_RELEASE:
				//Nullifying the PrePay deal owner
				info.nOwningPos = NO_POS_OWNER;
				lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &trs, &info);
				break;

			default:
				retCode = GPI_NOT_SUPPORTED;
			}
		}
	}

	if (lRtc != OK)
	{
		if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
		{
			retCode = GPI_TRS_NOT_FOUND;
		}
		else
		{
			retCode = GPI_FAILED_PREPAY_IO;
		}
	}

	if (retCode)
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(m_lPosNumber, // Src
				"IGPI",
				"SetPrePayTrsState",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE,
				lRtc);
	}

	if (plVal != NULL)
		*plVal = retCode;
}



STDMETHODIMP CGPI::GetVersion(BSTR *sVersionList, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetVersion   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		GetVersion__(sVersionList, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetVersion"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetVersion()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}
/******************************************************************************
Description:	Get fuel system component
Returns:      	list of component version
Parameters:   	sVersionList - out bstr

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Marik			17/6/2004   15:10		Start
******************************************************************************/

void CGPI::GetVersion__(BSTR *sVersionList, long lFlags, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (sVersionList != NULL)
	{

		CComBSTR   sRec = _Module.m_server.sSysVersion;

		if (_Module.m_server.sSysVersion.IsEmpty())
		{
			_Module.m_server.sSysVersion.Format("PumpSrv=%s,", _Module.m_server.m_strPumpSrvVersion);
		}
		else
			*sVersionList = sRec.Copy();
	}

	if (pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CGPI::CancelWashCode2(long *plSyncNumber, long lDevice, long lFlags, BSTR sData, long lRefundCarWash, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::CancelWashCode2   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		CancelWashCode__(plSyncNumber, lDevice, lFlags, sData, lRefundCarWash, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::CancelWashCode2"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "CancelWashCode2()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


STDMETHODIMP CGPI::GetReport(long lNumber, long lType, BSTR *sReportData, long lFlags, long *lRet)
{
	LOG_EXTERNALCALL("CGPI::GetReport   start");

	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		GetReport__(lNumber, lType, sReportData, lFlags, lRet);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetReport"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetReport()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}

void CGPI::GetReport__(long lNumber, long lType, BSTR *sReportData, long lFlags, long *lRet)
{
	GPIRetCode	retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;
	else
	{
		//4.0.22.501 - TD 69905
		if (lType == GET_REPORT_WETSTOCK_ASYNC_TYPE)
		{
			_Module.m_server.GetWetStockReportAsync();
		}
		else //for all report types
		{
			retCode = HandleGetReport(lNumber, lType, sReportData, lFlags);
		}
	}

	if (lRet != NULL)
		*lRet = retCode;

}

/******************************************************************************
Description:	Handle the request for all kinds of reports
Returns:		return GPI return code in case of failure
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			03/02/2009   14:50		Start	//4.0.22.501 - TD 69905
******************************************************************************/
GPIRetCode CGPI::HandleGetReport(long lNumber, long lType, BSTR *sReportData, long lFlags)
{
	GPIRetCode	retCode = GPI_OK;

	//Get handle Pos receipt 
	CMD_STRUCT_FORMAT_REPORT cmdInfo;
	CSingleLock slPosMap(&(_Module.m_server.m_csPosMap));
	slPosMap.Lock(INFINITE);
	CPosInfo tmpPos;

	_Module.m_server.m_cPosMap.Lookup(m_lPosNumber, tmpPos);
	slPosMap.Unlock();

	cmdInfo.lPosNumber = m_lPosNumber;
	cmdInfo.lNumber = lNumber;
	cmdInfo.lFlags = lFlags;
	cmdInfo.lType = lType;

	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("HandleGetReport, lPosNumber=%ld lNumber=%ld,lFlags=%ld lType=%ld ", m_lPosNumber, lNumber, lFlags, lType);
		_LOGMSG.LogMsg("CGPI", "GetReport", m_lPosNumber, LOG_POS, str);

	}

	_Module.m_server.SetCommand(CMD_FORMAT_REPORT, SERVER_POS_NUMBER, m_lPosNumber, &cmdInfo, sizeof(CMD_STRUCT_FORMAT_REPORT));
	// if we get the event in 1 second we get receipt
	if (tmpPos.WaitForSingleObjectReceipt())
	{
		CComBSTR strResult;
		//Get handle Pos receipt 
		CSingleLock slPosMap(&(_Module.m_server.m_csPosMap));
		slPosMap.Lock(INFINITE);
		CPosInfo tmpPos;

		////////////////////////////////////////////////////////////////////////////////////////
		if (LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("HandleGetReport, retcode=%d", retCode);
			_LOGMSG.LogMsg("CGPI", "GetReport", m_lPosNumber, LOG_POS, str);

		}
		/////////////////////////////////////////////////////////////////////////////////////////

		_Module.m_server.m_cPosMap.Lookup(m_lPosNumber, tmpPos);

		strResult = tmpPos.GetReceiptText();
		*sReportData = strResult.Copy();

		slPosMap.Unlock();
	}

	else
		retCode = GPI_NO_RECEIPT;

	return retCode;
}


/******************************************************************************
Description:Get track2 and according to this track we get row data from QDX
And convert him to XML
Returns:   return  to pos a row data that built from xml
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amram			15/05/2005   15:10		Start
******************************************************************************/
void CGPI::GetReceiptRawData__(long lPumpNumber,
	BSTR sTrack2,
	short *nIndex,
	BSTR *sReceiptRawData,
	long *plFlags,
	long *pVal)
{
	PAY_AT_PUMP_INFO	PayAtPumpInfo;
	CPumpTransact		trs;
	TAG_OLA_REC9_EX		tagOlaRec;
	GCIRetCode			retCode = GCI_OK;
	DWORD				dwLastErr;
	long				lRtc = 0;
	short				nPosIndex = 0;
	short				nTmpIndex = 0;
	DWORD				dwMaxReceiptCopies;
	char				* sTmpXML = NULL; // TD 396348
	int					iLen = 0;

	char				szTrack2[MAX_TABLE_NAME];
	memset(szTrack2, 0, sizeof(szTrack2));

	CString				str;//4.0.9.503
	CString				strMsg;

	RECEIPT_INFO	Info;
	RECEIPT_INFO	TmpInfo;  //3.2.0.69


	if (RECEIPT_GET_LAST_MEMO_TRACK2 & *plFlags)
	{
		CMemPumpInfo Info;

		if ((lPumpNumber > _Module.m_server.GetMaxPumps()) || (lPumpNumber < 1))   //4.0.0.79
			retCode = GCI_INVALID_PUMP_NUMBER;
		else
		{
			iLen = 1;
			memset(szTrack2, ' ', sizeof(szTrack2));
			if (_Module.m_server.m_cMemPumpArray[lPumpNumber - 1].GetMemPumpInfo(&Info, 0))
			{
				if (Info.cCardData.sTranType == CD_TYPE_AVI)
				{
					memcpy(szTrack2, Info.cCardData.sFullVehicleId, min(sizeof(szTrack2), sizeof(Info.cCardData.sFullVehicleId)));
				}
				else
					memcpy(szTrack2, Info.cCardData.sTrack2Data, min(sizeof(szTrack2), sizeof(Info.cCardData.sTrack2Data)));
			}
			else
				retCode = GCI_NO_RECEIPT;
			{
			}
		}
		strMsg.Format("Get last memory receipt for lPumpNumber=%ld, retCode=%d", lPumpNumber, retCode);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, strMsg);
	}
	else
	{
		iLen = WideCharToMultiByte(CP_ACP, 0, sTrack2, -1, szTrack2, sizeof(szTrack2), NULL, NULL);

		str.Format("Call GetCardInfo");
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);

		memset(&PayAtPumpInfo, ' ', sizeof(PAY_AT_PUMP_INFO));
		memset(&Info, ' ', sizeof(Info));

		//call get card info to get the account number 
		//after we encrypt the account he  become the key to  QDX 
		//first check if avi flag is on if so then he  become the key

		if (LOG_DETAIL_FLOW_CONTROL)
		{
			if (iLen)
			{
				if (RECEIPT_CAR_REGISTRATION_INDEX  & *plFlags)  // 4.0.7.500
				{
					strMsg.Format("Car registration=%s,  nIndex=%d retCode=%d ", szTrack2, *nIndex, retCode);
				}
				else
				{//4.0.20.501 TD 60236
					char	szResult[MAX_FIELD_VAL] = { 0 };
					_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, szTrack2, iLen);

					strMsg.Format("GetReceiptRawData, szTrack2=%s,  nIndex=%d retCode=%d ", szResult, *nIndex, retCode);
				}

				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, strMsg);
			}
		}
	}


	if (iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GCI_NO_RECEIPT;
	}
	else
	{
		BOOL bFound = FALSE;
		memset(&Info, 0, sizeof(RECEIPT_INFO));

		_Module.m_server.m_cParam.LoadParam("Receipt", "MaxReceiptCopies", &dwMaxReceiptCopies, 0, TRUE);  //4.0.7.504

		if (RECEIPT_CAR_REGISTRATION_INDEX  & *plFlags)  // 4.0.7.500
		{
			RECEIPT_EXTRA_INDEX_INFO rec;
			memset(&rec, 0, sizeof(RECEIPT_EXTRA_INDEX_INFO));
			memset(rec.sKeyIndex, ' ', sizeof(rec.sKeyIndex));
			memcpy(rec.sKeyIndex, szTrack2, min(strlen(szTrack2), sizeof(rec.sKeyIndex)));

			lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_INDEX_FILE, REC_READ, &rec);
			if (!lRtc)
			{
				memcpy(&TmpInfo.cReceipt1.Index, &rec.myIndex, sizeof(TmpInfo.cReceipt1.Index));

				CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
				lRtc = 1;//PumpSrvDatabase.LoadReceipt(TmpInfo) ? OK : 1;

				if (!lRtc)
				{
					if (dwMaxReceiptCopies > TmpInfo.cReceipt1.cInfoData.byNumberOfCopies)  //4.0.7.504
					{
						nTmpIndex = TmpInfo.cReceipt1.Index.byIndex;
						bFound = TRUE;
					}
				}
			}
		}
		else
		{
			Track2Format((unsigned char *)szTrack2);

			memcpy(Info.cReceipt1.Index.sTrack2, szTrack2, TRACK_2_DATA_SIZE);

			lRtc = _Module.m_server.IndexOperation(RECEIPT_FILE, REC_READ_FIRST, &Info);

			for (nPosIndex = 0; (lRtc == OK); nPosIndex++)
			{
				if ((lRtc == OK) && !memcmp(&Info.cReceipt1.Index.sTrack2, szTrack2, TRACK_2_DATA_SIZE) &&
					(dwMaxReceiptCopies > Info.cReceipt1.cInfoData.byNumberOfCopies))
				{
					memset(&PayAtPumpInfo, 0, sizeof(PAY_AT_PUMP_INFO));
					_Module.m_server.ConvertReceiptToOla(&Info, &trs, &PayAtPumpInfo);

					if ((PayAtPumpInfo.CardSaleExtraData2.sReceiptPrinted != '1') ||
						(RECEIPT_GET_LATEST_RECEIPT & *plFlags))
					{
						if (lPumpNumber)
						{
							if (trs.m_lPump == lPumpNumber)
							{
								nTmpIndex = nPosIndex;
								TmpInfo = Info;
								bFound = TRUE;
							}
						}
						else
						{
							nTmpIndex = nPosIndex;
							TmpInfo = Info;
							bFound = TRUE;
						}
					}
				}

				lRtc = _Module.m_server.IndexOperation(RECEIPT_FILE, REC_READ_NEXT, &Info);
			}
		}

		// if receipt found , it's mine that we allowed to print.(in ReceiptPrinted we delete old one)
		if (bFound)
		{
			PS::RECEIPT_EXTRA_INFO_REC	cReceiptExtraRec;
			ITEMS_INFO	cItemInfo;
			long  lItemIndex = 0;
			long  lRtc;

			memset(&tagOlaRec, ' ', sizeof(TAG_OLA_REC9_EX));
			memset(&PayAtPumpInfo, ' ', sizeof(PAY_AT_PUMP_INFO)); //4.0.5.1

			_Module.m_server.ConvertReceiptToOla(&TmpInfo, &trs, &PayAtPumpInfo);

			BOOL  bReceiptExtra = _Module.m_server.GetOLAFlags(PayAtPumpInfo.CardSaleExtraData2.sFlags, FL_EXTRA_RECEIPT_REC);

			l2a(PayAtPumpInfo.CardSaleInfo.lIndexNumber, tagOlaRec.lIndexNumber, sizeof(tagOlaRec.lIndexNumber));
			tagOlaRec.cardData = PayAtPumpInfo.CardSaleInfo.cardData;
			tagOlaRec.extraData = PayAtPumpInfo.CardSaleInfo.extraData;
			tagOlaRec.extraData2 = PayAtPumpInfo.CardSaleExtraData2;
			trs.GetTag(&tagOlaRec.tagTrs, 2);
			tagOlaRec.extraData3 = PayAtPumpInfo.CardSaleExtraData3; //4.0.5.25   //4.0.5.213

																	 /////////////////////////////////////////////////////////////////////////
			if (bReceiptExtra)
			{
				memset(&cReceiptExtraRec, 0, sizeof(PS::RECEIPT_EXTRA_INFO_REC));
				cReceiptExtraRec.info.lIndexNumber = a2l(tagOlaRec.cardData.sTranNbr, sizeof(tagOlaRec.cardData.sTranNbr));

				lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, cReceiptExtraRec);
				//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cReceiptExtraRec);

				// Fail to read receipt exatra data
				if (lRtc)
				{
					memset(cReceiptExtraRec.info.sFiler2043, ' ', sizeof(cReceiptExtraRec.info.sFiler2043));
				}

				if (LOG_DETAIL_FLOW_CONTROL)
				{
					CString sMsg;
					sMsg.Format("GetReceiptRawData, Trs=%ld,lRtc=%ld Receipt information was send to controller receipt extra information", trs.m_lNumber, lRtc);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, trs.m_lPump, LOG_PUMP, sMsg);
				}
			}



			lItemIndex = a2l(tagOlaRec.extraData2.sItemsLinkIndex, sizeof(tagOlaRec.extraData2.sItemsLinkIndex));

			if (lItemIndex)
			{
				TAG_OLA_REC9_EX	cTagOlaRec9;
				long  lOlaRecSize = sizeof(TAG_OLA_REC6); //4.0.5.0
				BOOL  bExtarItems = _Module.m_server.GetOLAFlags(tagOlaRec.extraData2.sFlags, FL_EXTRA_ITEM_REC);

				*plFlags = RECEIPT_FORMAT_WITH_ITEMS;

				memset(&cItemInfo, ' ', sizeof(cItemInfo));
				cItemInfo.ItemRec.lIndexNumber = lItemIndex;
				cItemInfo.ItemRec2.lIndexNumber = lItemIndex + 1;

				memset(&cTagOlaRec9, ' ', sizeof(cTagOlaRec9));		//4.0.5.39
				cTagOlaRec9.cardData = tagOlaRec.cardData;
				cTagOlaRec9.extraData = tagOlaRec.extraData;
				cTagOlaRec9.extraData2 = tagOlaRec.extraData2;
				cTagOlaRec9.tagTrs = tagOlaRec.tagTrs;

				if (_Module.m_server.GetOLAFlags(tagOlaRec.extraData2.sFlags, FL_PAP_AS_THIRD_REC))
				{
					*plFlags = RECEIPT_FORMAT_WITH_ITEMS + SWIPE_THREE_OLA_REC;
					cTagOlaRec9.extraData3 = tagOlaRec.extraData3;

					if (bExtarItems)
					{
						lOlaRecSize = sizeof(TAG_OLA_REC7_EX);
						*plFlags += RECEIPT_WITH_EXTRA_ITEM;  //4.0.5.27

						if (bReceiptExtra)
						{
							lOlaRecSize = sizeof(TAG_OLA_REC9_EX);
							*plFlags += RECEIPT_WITH_RECEIPT_EXTRA_DATA;  //4.0.5.39																			
							cTagOlaRec9.ReceiptExtraInfo = cReceiptExtraRec.info;
						}
					}

				}
				else
					lOlaRecSize = sizeof(TAG_OLA_REC4_EX);


				lRtc = _Module.m_server.IndexOperation(ITEMS_FILE, REC_READ, &cItemInfo);

				if (lRtc)
				{
					memset(&cTagOlaRec9.Items, ' ', sizeof(cTagOlaRec9.Items));
					memset(&cTagOlaRec9.Items2, ' ', sizeof(cTagOlaRec9.Items2));
					if (LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("GetReceiptRawData, Fail !!!! read Index=%ld from file Items", cItemInfo.ItemRec.lIndexNumber);
						_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
					}
				}
				else
				{
					CItemsFile cItemsFile;
					BAP_ALL_DATA_STRUCTS  cItemsRec;

					cItemsFile.ConstructItemsRecordToBAP(cItemInfo, cItemsRec);
					cTagOlaRec9.Items = cItemsRec.cBAPReceiptItem;
					cTagOlaRec9.Items2 = cItemsRec.cBAPReceiptItem2;

				}

				if (LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("Process command CMD_FORMAT_RECEIPT, Read ItemIndex=%ld, lFlags=%ld, Invoice=%.10s ", cItemInfo.ItemRec.lIndexNumber, *plFlags, cTagOlaRec9.Items.sInvoiceNumber);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
				}

				_Module.m_server.m_cCardSaleDataXML.CreateFleetPAPXML(trs, PayAtPumpInfo, sTmpXML);  //4.0.9999.0 //4.0.10.492

				CComBSTR strResult(sTmpXML);
				*sReceiptRawData = strResult.Copy();
				*nIndex = nTmpIndex;
				if (sTmpXML)
					delete[] sTmpXML;

			}
			else
			{
				long  lOlaRecSize = sizeof(TAG_OLA_REC3_EX);

				if (_Module.m_server.GetOLAFlags(tagOlaRec.extraData2.sFlags, FL_PAP_AS_THIRD_REC))
				{
					*plFlags = SWIPE_THREE_OLA_REC;  //4.0.5.25
					lOlaRecSize = sizeof(TAG_OLA_REC5_EX);

					if (bReceiptExtra)
					{
						*plFlags += RECEIPT_WITH_RECEIPT_EXTRA_DATA;  //4.0.5.39													
						lOlaRecSize = sizeof(TAG_OLA_REC8_EX);
						tagOlaRec.ReceiptExtraInfo = cReceiptExtraRec.info;
					}
				}

				_Module.m_server.m_cCardSaleDataXML.CreateFleetPAPXML(trs, PayAtPumpInfo, sTmpXML); //4.0.9999.0 //4.0.10.492
				CComBSTR strResult(sTmpXML);
				*sReceiptRawData = strResult.Copy();
				*nIndex = nTmpIndex;
				if (sTmpXML)
					delete[] sTmpXML;
			}
			///////////////////////////////////////////////////////////////////////////

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("GetReceiptRawData, return information  on lPumpNumber=%ld, SeqNum=%.5s", lPumpNumber, PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
			}
		}
		else
			retCode = GCI_NO_RECEIPT;
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("GetTeceiptRawData, nIndex=%d, lFlags=%ld, retCode=%d ", *nIndex, *plFlags, retCode);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
	}

	if (pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CGPI::GetReceiptRawData(long lPumpNumber, BSTR sTrack2,short *nIndex, BSTR *sReceiptRawData, long *plFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetReceiptRawData   start");

	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		GetReceiptRawData__(lPumpNumber, sTrack2, nIndex, sReceiptRawData, plFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetReceiptRawData"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetReceiptRawData", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}

/******************************************************************************
Description: Get Valid grades list on pump

Returns:   Grade array
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
marik			21/06/2005   15:10		Start
******************************************************************************/

STDMETHODIMP CGPI::GetPumpGradeInfo(long lPumpNumber, BSTR *sGradesData, long *lRecordcount, long lFlags, long *plVal)
{
//	LOG_EXTERNALCALL("CGPI::GetPumpGradeInfo   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		GetPumpGradeInfo__(lPumpNumber, sGradesData, lRecordcount, lFlags, plVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetPumpGradeInfo"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPumpGradeInfo", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

BOOL  CGPI::GetGradeInfo(long lGrade, TAG_GARDE_INFO &cTagGradeInfo)
{
	BOOL bVal = FALSE;

	if (lGrade)
	{
		CString sTable;
		sTable.Format("Grades\\Grade%.02ld", lGrade);
		char	szGrade[MAX_FIELD_VAL] = { 0 };

		memset(&cTagGradeInfo, ' ', sizeof(cTagGradeInfo));
		l2a(lGrade, cTagGradeInfo.sGradeNumber, sizeof(cTagGradeInfo.sGradeNumber));
		memset(szGrade, ' ', sizeof(szGrade));
		_Module.m_server.m_cParam.LoadParam((LPCSTR)sTable, "FullName", (char *)szGrade, sizeof(szGrade), "Unleaded", FALSE);
		memcpy(cTagGradeInfo.sGradeName, szGrade, min(sizeof(cTagGradeInfo.sGradeName), strlen(szGrade)));

		bVal = TRUE;
	}
	else
	{
		memset(&cTagGradeInfo, ' ', sizeof(cTagGradeInfo));	//4.0.22.502 71801
		l2a(0, cTagGradeInfo.sGradeNumber, sizeof(cTagGradeInfo.sGradeNumber));
		bVal = TRUE;
	}


	return bVal;

}

STDMETHODIMP CGPI::GetAllTrsQuery(long lPumpNumber, BSTR *sData, long *plRecCount, long lFlags, long *plRetVal)
{
	LOG_EXTERNALCALL("CGPI::GetAllTrsQuery   start");
	// TODO: Add your implementation code here
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		GetAllTrsQuery__(lPumpNumber, sData, plRecCount, lFlags, plRetVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetAllTrsQuery"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetAllTrsQuery", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

	}
#endif 

	return S_OK;
}
/******************************************************************************
Description: Query data for all transaction , include  ability to restrict this query per pump

Returns:    Transaction array
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
marik			21/06/2005   15:10		Start
******************************************************************************/
void CGPI::GetAllTrsQuery__(long lPumpNumber, BSTR *sData, long *plRecCount, long lFlags, long *plRetVal)
{
	GPIRetCode retCode = GPI_OK;
	GetPAKQueryRetCode eRetCode;
	long lRetVal = GPI_OK;


	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;

	else if (lPumpNumber)
	{
		if (PUMP_NOT_VALID(lPumpNumber))
			retCode = GPI_INVALID_PUMP_NUMBER;
	}


	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("GetAllTrsQuery, request for PumpNumber %02ld flags=%x",
			lPumpNumber, lFlags);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
	}


	if (retCode == GPI_OK)
	{
		eRetCode = _Module.m_server.m_cCommonFunction.GetPAKQuery(sData, plRecCount, &lFlags, NULL, lPumpNumber);

		switch (eRetCode)
		{
		case GetPakQuery_OK:
			lRetVal = GPI_OK;
			break;
		case GetPakQuery_MissingData:
			lRetVal = GPI_LAST_ERROR;
			break;
		}

	}


	if (LOG_LOGIC_ERROR && lRetVal)
		_LOGMSG.LogResult(
			0, // Src
			"IGPI",
			"GetAllTrsQuery",
			TRUE, lRetVal,
			FALSE, 0);

	if (plRetVal != NULL)
		*plRetVal = lRetVal;
}

STDMETHODIMP CGPI::AddItem2Trs(long lPumpNumber, long lItemSelected, long lFlags, long *plRetVal)
{
	LOG_EXTERNALCALL("CGPI::AddItem2Trs   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		AddItem2Trs__(lPumpNumber, lItemSelected, lFlags, plRetVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::AddItem2Trs"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "AddItem2Trs", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

	}
#endif 

	return S_OK;
}

void CGPI::AddItem2Trs__(long lPumpNumber, long lItemSelected, long lFlags, long *plRetVal)
{
	GPIRetCode retCode = GPI_OK;
	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;


	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("Attempt to update PumpNumber %02ld with item flags %ld",
			lPumpNumber, lItemSelected);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
	}

	if (retCode == GPI_OK)
	{
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		long p = lPumpNumber - 1;
		CPumpTransact trs;

		if ((_Module.m_server.m_cPumpArray[p].GetDieselPumpFlags()) &&
			(_Module.m_server.m_cPumpArray[p].GetStat().GetRes() == PAY_AT_PUMP) &&
			(!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs)))
		{
			long lTmpItemRestriction = 0;  //4.0.10.50

			PAY_AT_PUMP_INFO Info;
			_Module.m_server.m_cPumpArray[p].GetPAPInfo(&Info);

			lTmpItemRestriction = a2l(Info.CardSaleExtraData3.sItemRestriction, sizeof(Info.CardSaleExtraData3.sItemRestriction));

			if (!(lTmpItemRestriction & lItemSelected))
			{
				lTmpItemRestriction += lItemSelected;
				l2a(lTmpItemRestriction, Info.CardSaleExtraData3.sItemRestriction, sizeof(Info.CardSaleExtraData3.sItemRestriction));
				_Module.m_server.m_cPumpArray[p].SetPAPInfo(&Info);
				_Module.m_server.SavePumpInfo(lPumpNumber); //4.0.10.50
			}
			else
			{
				if (LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("Add item skip on select due exist item currect =%ld , new =%ld", lTmpItemRestriction, lItemSelected);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
				}

			} //4.0.10.50

		}
	}

	if (plRetVal != NULL)
		*plRetVal = retCode;

	if (LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGPI",
			"AddItem2Trs",
			TRUE, retCode,
			FALSE, 0);
}


void CGPI::GetPumpGradeInfo__(long lPumpNumber, BSTR *sGradesData, long *lRecordCount, long lFlags, long *plVal)
{
	GPIRetCode retCode = GPI_OK;
	long	lTmpGrade = 0;
	char	szGrade[MAX_FIELD_VAL] = { 0 };
	CString sTable;
	CComBSTR bstrGrades;
	DWORD dwTmp = 0;

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("GetPumpGradeInfo - lPumpNumber %ld,lFlags =%ld", lPumpNumber, lFlags);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, lPumpNumber, LOG_PUMP, str);
	}

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;

	*lRecordCount = 0;


	if (retCode == GPI_OK)
	{
		if (lFlags & GPG_ALL_GRADES_DATA)//4.0.12.505
		{
			retCode = GetAllGradesData(lPumpNumber, bstrGrades, lRecordCount, lFlags);
		}
		else
			if (lFlags & GPG_GRADES_POSITIONS)
			{
				retCode = GetGradesPositionData(lPumpNumber, bstrGrades, lRecordCount);
			}
			else
				retCode = GetPumpGradeInfoImpl(lPumpNumber, bstrGrades, lRecordCount);


		if (*lRecordCount)
		{
			*sGradesData = bstrGrades.Copy();
		}

	}

	if (plVal != NULL)
		*plVal = retCode;
}




STDMETHODIMP CGPI::SetTrsState2(long lPumpNumber, long lTrsNumber, long lState, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetTrsState2   start");


#ifdef CATCH_GPI
	__try
	{
#endif

		SetTrsState__(lPumpNumber, lTrsNumber, lState, pVal, lFlags);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::SetTrsState2"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetTrsState2()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 



	return S_OK;
}




STDMETHODIMP CGPI::InitEventsInterface(long lFlags, long *pClientId, long *pVal)
{
//	LOG_EXTERNALCALL("CGPI::InitEventsInterface   start");
#ifdef CATCH_CTRL
	__try
	{
#endif
		InitEventInterface_(lFlags, pClientId, pVal);
#ifdef CATCH_CTRL
	}
	__except (_HANDLE_EXCEPTION("CGPI::InitEventsInterface"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::InitEventsInterface()", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


// Function name	: CCtrl::InitEventInterface_
// Description	    : Inits the CtrlEvents interface.
// Return type		: void 
// Argument         : long lFlags - for future use.
// Argument         : long *pClientId - uniqe client id for the connected client.
// Argument         : long *pVal - function return code.
void CGPI::InitEventInterface_(long lFlags, long *pClientId, long *pVal)
{
	IGPIEvents *pGpiObj = NULL;
	long		lClientId = 0;
	CString		str;

	pGpiObj = (IGPIEvents *) this;

	lClientId = _Module.m_server.GetGpiEventClientId();

	_Module.m_server.AddGpiEventClient(lClientId, pGpiObj);

	str.Format("InitEventInterface_ Add new client to GPI Event number %ld flag = %ld", lClientId, lFlags);
	_LOGMSG.LogMsg(str);
	if (pClientId != NULL)
		*pClientId = lClientId;

	if (pVal != NULL)
		*pVal = 0;
}

STDMETHODIMP CGPI::CloseEventsInterface(long lClientId, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::CloseEventsInterface   start");
#ifdef CATCH_CTRL
	__try
	{
#endif
		CloseEventsInterface_(lClientId, pVal);
#ifdef CATCH_CTRL
	}
	__except (_HANDLE_EXCEPTION("CGPI::CloseEventsInterface"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::CloseEventsInterface()", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}




// Function name	: CCtrl::CloseEventsInterface_
// Description	    : Closes the CtrlEvent connection.
// Return type		: void 
// Argument         : long lClientId - client id recieved when called InitEventsInterface 
// Argument         : long *pVal
void CGPI::CloseEventsInterface_(long lClientId, long *pVal)
{
	long lRetVal = CTRL_OK;

	lRetVal = _Module.m_server.RemoveGpiEventPtr(lClientId);

	if (pVal != NULL)
		*pVal = lRetVal;
}


STDMETHODIMP CGPI::SetPumpStatus(long lPumpNumber, long lStatus, long lRes, long lResExt, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetPumpStatus   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		SetPumpStatus__(lPumpNumber, lStatus, lRes, lResExt, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::SetPumpStatus"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetPumpStatus()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 



	return S_OK;
}


void CGPI::SetPumpStatus__(long lPumpNumber, long lStatus, long lRes, long lResExt, long *pVal)
{

	long lFlags = 0;
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;


	if (retCode == GPI_OK)
	{
		//In this case reserve pump for external PAP transaction
		if (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse())
		{
			if ((lRes == PAY_AT_PUMP) && (lStatus == READY) && (lResExt == RES_EXT_PAP_EXTERNAL || lResExt == RES_EXT_MOBILE))
			{
				if (lResExt == RES_EXT_MOBILE)//TD 453823
				{

					short nPrePayWaiting = 0, nFree = 0, nUnpaid = 0;
					_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsListCount(&nPrePayWaiting, &nFree, &nUnpaid);
					if ((lPumpNumber > 0) && (nPrePayWaiting + nUnpaid >= (long)_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetMaxTrsNumber()))
					{
						retCode = GPI_PUMP_TRS_LIST_FULL;
					}


					if ((retCode == GPI_OK) && (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetLockStatus() == UNLOCK))
					{
						if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].CanReserve())// TD 453823
						{
							CPumpStatus stat;		//4.0.1.34
							stat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();
							if (_Module.m_server.m_cModeArray[stat.GetMode() - 1].PAMNotValid())
							{
								retCode = GPI_NOT_VALID_IN_CURRENT_MODE;
							}
						}
						else
						{
							retCode = GPI_PUMP_ALREADY_RESERVED;
						}
					}
					else
					{
						retCode = GPI_PUMP_LOCKED;
					}
				}

				if (retCode == GPI_OK)
				{
					CMD_STRUCT_RESERVE_PUMP cmdReservePump;
					cmdReservePump.lRes = PAY_AT_PUMP;
					//cmdReservePump.lResExt = RES_EXT_PAP_EXTERNAL;
					cmdReservePump.lResExt = lResExt;// CR 444830
					_Module.m_server.SetCommand(CMD_RESERVE_PUMP, SERVER_POS_NUMBER, lPumpNumber, &cmdReservePump, sizeof(CMD_STRUCT_RESERVE_PUMP));
				}
			}
			else if ((lRes == PAY_AT_KIOSK) && ((lStatus == READY) || (lStatus == NOZZLE_LEFT_OUT))) //4.0.25.30 - TD 118690
			{
				//4.0.29.0 - TD 269203
				short nPrePayWaiting, nFree, nUnpaid;
				long p = lPumpNumber - 1;

				_Module.m_server.m_cPumpArray[p].GetTrsListCount(&nPrePayWaiting, &nFree, &nUnpaid);
				if (nUnpaid >= MAX_ALLOWED_UNPAID_TRS(p))
				{
					retCode = GPI_PUMP_TRS_LIST_FULL;
				}

				if (retCode == GPI_OK)
				{
					//Send Pay Inside Pressed event to CL
					_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, lPumpNumber, GCI_EVENT_PAY_INSIDE_PRESSED);
					if (LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("SetPumpStatus Send event GCI_EVENT_PAY_INSIDE_PRESSED for pump  %ld", lPumpNumber);
						_LOGMSG.LogMsg(str);
					}
				}
			}
		}
		else
		{
			CMD_STRUCT_SET_PUMP_STATUS cmdInfo;
			cmdInfo.lStatus = lStatus;
			cmdInfo.lRes = lRes;
			cmdInfo.lResExt = lResExt;
			cmdInfo.lFlags = lFlags;
			cmdInfo.pVal = pVal;
			cmdInfo.lPumpNumber = lPumpNumber;
			_Module.m_server.SetCommand(CMD_SET_PUMP_STATUS, SERVER_POS_NUMBER, lPumpNumber, &cmdInfo, sizeof(CMD_STRUCT_SET_PUMP_STATUS));
		}
	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"SetPumpStatus",
				TRUE, retCode,
				FALSE, 0);

		if (pVal != NULL)
			*pVal = retCode;
	}

}


STDMETHODIMP CGPI::PAPSwipe(long lPumpNumber, long lDeviceNumber, long *lSeqNumber, long lFlags, BSTR sData, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::PAPSwipe   start");

#ifdef CATCH_GPI
	__try
	{
#endif
		PAPSwipe__(lPumpNumber, lDeviceNumber, lSeqNumber, lFlags, sData, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::PAPSwipe"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "PAPSwipe", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;

}

void CGPI::PAPSwipe__(long lPumpNumber, long lDeviceNumber, long *lSeqNumber, long lFlags, BSTR sData, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;


	if (retCode == GPI_OK)
	{

		CARD_SALE_ALL3 cardDataAll3;
		memset(&cardDataAll3, 0, sizeof(CARD_SALE_ALL3));

		int iLen = Bstr2Buffer(sData, (void *)&cardDataAll3, sizeof(CARD_SALE_ALL3));


		_Module.m_server.m_lPAPDeclineTimer[lPumpNumber - 1] = 0;

		CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo; //4.0.27.45
												 //		CMD_STRUCT_PAP_SWIPE_RESULT_XML cmdInfoXML;

		memset(&cmdInfo, ' ', sizeof(cmdInfo));
		cmdInfo.lDeviceNumber = '0';
		cmdInfo.lSeqNumber = *lSeqNumber;
		cmdInfo.lAnswer = CD_OK;
		cmdInfo.lFlags = STRUCT_IS_CARD_ALL_DATA3;




		// 		if (GetPosNumber() == MOBILE_SRV_POS_NUMBER)  // work with XML
		// 			_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml((char*)&cmdInfoXML.data, &cardDataAll3);
		// 		else
		{     // IFSFTerminal continue work with struct
			cmdInfo.data = cardDataAll3.CardSaleAll.data;
			cmdInfo.extData = cardDataAll3.CardSaleAll.extData;
			cmdInfo.extData2 = cardDataAll3.extData2;
			cmdInfo.extData3 = cardDataAll3.extData3;
			cmdInfo.extData4 = cardDataAll3.extData4;
			cmdInfo.extData5 = cardDataAll3.extData5;
			cmdInfo.extData6 = cardDataAll3.extData6;
		}


		_Module.m_server.SaveCardSaleData(lPumpNumber, &cardDataAll3);


		CPumpTransact		trs(lPumpNumber);
		trs.m_lNumber = _Module.m_server.NewTransactNumber();
		trs.m_lActiveReadNumber = _Module.m_server.GetActiveReadNumber();
		trs.SetLink(TRS_LINK_OLA_NUMBER, _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPSeqNumber());
		trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetShiftNumber();
		trs.SetStatus(WAIT_PROGRESS);
		trs.m_lPump = lPumpNumber;

		_Module.m_server.CentralInsertTrs(lPumpNumber, &trs);

		// 		if (GetPosNumber() == MOBILE_SRV_POS_NUMBER)  // work with XML
		// 		{
		// 			_Module.m_server.SetCommand(CMD_SWIPE_RESULT, m_lPosNumber, lPumpNumber, &cmdInfoXML, sizeof(CMD_STRUCT_PAP_SWIPE_RESULT_XML));
		// 		}
		// 		else
		{
			_Module.m_server.SetCommand(CMD_SWIPE_RESULT, m_lPosNumber, lPumpNumber, &cmdInfo, sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));
		}



		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("Pump %02ld lSeqNumber %ld , CGPI::PAPSwipe", lPumpNumber, *lSeqNumber);
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
		}
	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"PAPSwipe",
				TRUE, retCode,
				FALSE, 0);

		if (pVal != NULL)
			*pVal = retCode;
	}
}


STDMETHODIMP CGPI::PAPReadyTransaction(long lPumpNum, long lSeqNum, long lTrsNum, short nNozzle, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long lFlags, BSTR sData, BSTR *sRetData, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::PAPReadyTransaction   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		PAPReadyTransaction__(lPumpNum, lSeqNum, lTrsNum, nNozzle, lVolume, lValue, nLevel, lGradePrice, nGrade, lFlags, sData, sRetData, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::PAPReadyTransaction"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "PAPReadyTransaction", lPumpNum, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::PAPReadyTransaction__(long lPumpNum, long lSeqNum, long lTrsNum, short nNozzle, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long lFlags, BSTR sData, BSTR* sRetData, long* pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (PUMP_NOT_VALID(lPumpNum))
		retCode = GPI_INVALID_PUMP_NUMBER;


	if (retCode == GPI_OK)
	{

		CARD_SALE_ALL3 cardDataAll3;
		memset(&cardDataAll3, 0, sizeof(CARD_SALE_ALL3));
		PAY_AT_PUMP_INFO	cPayAtPumpInfo;
		_Module.m_server.m_cPumpArray[lPumpNum - 1].GetPAPInfo(&cPayAtPumpInfo); //4.0.23.508 - TD 91930

		int iLen = Bstr2Buffer(sData, (void *)&cardDataAll3, sizeof(CARD_SALE_ALL3));

		// Store before overwriting
		memcpy(&cPayAtPumpInfo.CardSaleExtraData3.sOLABatchId, &cardDataAll3.extData3.sOLABatchId, sizeof(cardDataAll3.extData3.sOLABatchId));			//4.0.26.503 TD 164518
		cPayAtPumpInfo.CardSaleExtraData3.sOverfillAdjusted = cardDataAll3.extData3.sOverfillAdjusted; //4.0.26.0 - TD 126195
		memcpy(&cPayAtPumpInfo.CardSaleInfo.extraData.sCouponReward, &cardDataAll3.CardSaleAll.extData.sCouponReward, sizeof(cardDataAll3.CardSaleAll.extData.sCouponReward)); //4.0.26.0 - TD 146072
		_Module.m_server.m_cPumpArray[lPumpNum - 1].SetPAPInfo(&cPayAtPumpInfo); //4.0.26.0 - TD 146072

																				 //4.0.23.508 - TD 91930
		cardDataAll3.CardSaleAll.data = cPayAtPumpInfo.CardSaleInfo.cardData;
		cardDataAll3.CardSaleAll.extData = cPayAtPumpInfo.CardSaleInfo.extraData;
		cardDataAll3.extData2 = cPayAtPumpInfo.CardSaleExtraData2;
		cardDataAll3.extData3 = cPayAtPumpInfo.CardSaleExtraData3;


		CMD_STRUCT_PAP_READY_TRS cmdInfo;
		cmdInfo.lSeqNumber = lSeqNum;
		cmdInfo.lTrsNumber = lTrsNum;
		cmdInfo.nNozzle = nNozzle;
		cmdInfo.lVolume = lVolume;
		cmdInfo.lValue = lValue;
		cmdInfo.nLevel = nLevel;
		cmdInfo.lGradePrice = lGradePrice;
		cmdInfo.nGrade = nGrade;
		cmdInfo.lFlags = lFlags;
		cmdInfo.cardDataAll3 = cardDataAll3;
		cmdInfo.sRetData = sRetData;
		cmdInfo.pVal = pVal;

		_Module.m_server.SetCommand(CMD_PAP_READY_TRS, SERVER_POS_NUMBER, lPumpNum, &cmdInfo, sizeof(CMD_STRUCT_PAP_READY_TRS));
	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"PAPReadyTransaction",
				TRUE, retCode,
				FALSE, 0);

		if (pVal != NULL)
			*pVal = retCode;
	}
}

STDMETHODIMP CGPI::GetParam3(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long* pVal)
{
	LOG_EXTERNALCALL("CGPI::GetParam3   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		GetParam3__(sParamPosition, lKey1, lKey2, nFiledType, sVal, lVal, bCreate, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetParam3"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetParam3()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 



	return S_OK;
}

void CGPI::GetParam3__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long* pVal)
{

	long lFlags = 0;
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;


	if (retCode == GPI_OK)
	{

		CMD_STRUCT_GET_PARAM3 cmdInfo;
		cmdInfo.sParamPosition = sParamPosition;
		cmdInfo.lKey1 = lKey1;
		cmdInfo.lKey2 = lKey2;
		cmdInfo.nFiledType = nFiledType;
		cmdInfo.sVal = sVal;
		cmdInfo.lVal = lVal;
		cmdInfo.bCreate = bCreate;
		cmdInfo.pVal = pVal;
		_Module.m_server.SetCommand(CMD_GET_PARAM3, SERVER_POS_NUMBER, 0, &cmdInfo, sizeof(CMD_STRUCT_GET_PARAM3));
	}
	else
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetParam3",
				TRUE, retCode,
				FALSE, 0);

	}

}


BOOL  CGPI::GetGradeAllInfo(long lGrade, TAG_GARDE_ALL_INFO &cTagGradeInfo, long lFlags) //4.0.26.508 147183
{
	BOOL bVal = FALSE;
	DWORD dwTmp;

	if (lGrade)
	{
		CString sTable;
		sTable.Format("Grades\\Grade%.02ld", lGrade);
		char	szGrade[MAX_FIELD_VAL] = { 0 };
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTable), "ValidEntry", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, FALSE);
		if (dwTmp)//if the grade is valid
		{
			l2a((long)dwTmp, cTagGradeInfo.sValidEntry, sizeof(cTagGradeInfo.sValidEntry));
			memset(&cTagGradeInfo, ' ', sizeof(cTagGradeInfo));
			l2a(lGrade, cTagGradeInfo.sGradeNumber, sizeof(cTagGradeInfo.sGradeNumber));
			memset(szGrade, ' ', sizeof(szGrade));
			_Module.m_server.m_cParam.LoadParam((LPCSTR)sTable, "FullName", (char *)szGrade, sizeof(szGrade), "Unleaded", FALSE);
			memcpy(cTagGradeInfo.sGradeName, szGrade, min(sizeof(cTagGradeInfo.sGradeName), strlen(szGrade)));
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTable), "Code", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			l2a((long)dwTmp, cTagGradeInfo.sCode, sizeof(cTagGradeInfo.sCode));
			if (lFlags & GPG_ALL_GRADES_OLD_PRICES)
			{ //4.0.26.508 147183
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTable), "Old_PriceCredit_FullService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCreditFullService, sizeof(cTagGradeInfo.sNewPriceCreditFullService));
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTable), "Old_PriceCash_FullService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCashFullService, sizeof(cTagGradeInfo.sNewPriceCreditFullService));
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTable), "Old_PriceCredit_SelfService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCreditSelfService, sizeof(cTagGradeInfo.sNewPriceCreditFullService));
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTable), "Old_PriceCash_SelfService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCashSelfService, sizeof(cTagGradeInfo.sNewPriceCreditFullService));

			}
			else
			{
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTable), "New_PriceCredit_FullService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCreditFullService, sizeof(cTagGradeInfo.sNewPriceCreditFullService));
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTable), "New_PriceCash_FullService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCashFullService, sizeof(cTagGradeInfo.sNewPriceCreditFullService));
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTable), "New_PriceCredit_SelfService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCreditSelfService, sizeof(cTagGradeInfo.sNewPriceCreditFullService));
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTable), "New_PriceCash_SelfService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCashSelfService, sizeof(cTagGradeInfo.sNewPriceCreditFullService));
				//RFUEL-3309 Get the maximal discount allowed for the grade 
				dwTmp = _Module.m_server.GetMaximalDiscountForGrade(lGrade, SELF_CASH);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCashSelfService_MaximalDiscount, sizeof(cTagGradeInfo.sNewPriceCashSelfService_MaximalDiscount));
				_Module.m_server.GetMaximalDiscountForGrade(lGrade, FULL_CASH);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCashFullService_MaximalDiscount, sizeof(cTagGradeInfo.sNewPriceCashFullService_MaximalDiscount));
				_Module.m_server.GetMaximalDiscountForGrade(lGrade, SELF_CREDIT);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCreditSelfService_MaximalDiscount, sizeof(cTagGradeInfo.sNewPriceCreditSelfService_MaximalDiscount));
				_Module.m_server.GetMaximalDiscountForGrade(lGrade, FULL_CREDIT);
				l2a((long)dwTmp, cTagGradeInfo.sNewPriceCreditFullService_MaximalDiscount, sizeof(cTagGradeInfo.sNewPriceCreditFullService_MaximalDiscount));
			}
			bVal = TRUE;
		}
		else
			bVal = FALSE;


	}
	return bVal;

}


/******************************************************************************
Description:	 This method is called by PosPump only from version 1017 and on.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amit H			9/5/2007   19:21		Start
******************************************************************************/
STDMETHODIMP CGPI::AuthorizePump3(long lPumpNumber, short nLevel, long lGradePrice, long lFlags, long lFlags2, long lLimit, short nGroup, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::AuthorizePump3   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		AuthorizePump2__(lPumpNumber, nLevel, lGradePrice, lFlags, lLimit, nGroup, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::AuthorizePump3"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "AuthorizePump3()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}


//TD 38654
/******************************************************************************
Description:	 This method is called by PosPump only from version 1017 and on.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amit H			9/5/2007   19:21		Start
******************************************************************************/
STDMETHODIMP CGPI::GetPAPTrs5(long lPumpNumber, BSTR *pRec, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPAPTrs5   start");
	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPAPTrs5, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetPAPTrs5__(lPumpNumber, pRec, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetPAPTrs5"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPAPTrs5", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


/******************************************************************************
Description:	 This method is called by VP only from PumpSv version 6.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amit H			9/5/2007   19:21		Start	//TD 38654
******************************************************************************/
void CGPI::GetPAPTrs5__(long lPumpNumber, BSTR *pRec, long lFlags, long *pVal)
{
	GPIRetCode			retCode = GPI_OK;
	ASH2000_REC			sAsh2000Rec;
	BOOL				bFoundAsh2000Record = FALSE;
	long				lRtc = 0;

	CPumpTransact		trs;
	PAY_AT_PUMP_INFO	info;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	TAG_OLA_REC13 tagOlaRec;   //4.0.26.510 TD 137509
	memset(&tagOlaRec, ' ', sizeof(tagOlaRec));

	if (retCode == GPI_OK)
	{
		if (lPumpNumber) // search by pump 
		{
			if (PUMP_NOT_VALID(lPumpNumber))
				retCode = GPI_INVALID_PUMP_NUMBER;
			else
			{
				retCode = GPI_NOT_SUPPORTED;
			}
		}
		else // search for any PAP concluded trs
		{
			trs.SetLink(TRS_LINK_OLA_NUMBER, 0);
			memset(&info, 0, sizeof(PAY_AT_PUMP_INFO));

			lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST, &trs, &info);
			if (lRtc == OK)
			{
				if (LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("GetPAPTrs5__ - First Record number %d, trs %d, Shift %d, (GradePrice = %ld, Nozzle %d, Value %ld)",
						info.CardSaleInfo.lIndexNumber,
						trs.m_lNumber,
						trs.m_lActiveShiftNumber,
						trs.m_lGradePrice,
						trs.m_nNozzle,
						trs.m_lRoundedValue);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
				}


				//RFUEL-3309 Get the maximal discount allowed for the grade 
				long lMaxAllowedDiscount = _Module.m_server.GetMaximalDiscountForGrade(trs.m_nGrade);
				l2a(lMaxAllowedDiscount, info.CardSaleExtraData6.sMaxDiscountAllowed, sizeof(info.CardSaleExtraData6.sMaxDiscountAllowed));
				memset(&info.CardSaleExtraData6.LoyaltyCard, ' ', sizeof(info.CardSaleExtraData6.LoyaltyCard));  // reset LoyaltyCard
				l2a(info.CardSaleInfo.lIndexNumber, tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber));
				tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData = info.CardSaleInfo.cardData;
				tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData = info.CardSaleInfo.extraData;
				tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2 = info.CardSaleExtraData2;
				trs.GetTag(&tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs, 2);
				tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData3 = info.CardSaleExtraData3;
				tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData4 = info.CardSaleExtraData4; //4.0.17.501
				tagOlaRec.tagOlaRec12.extraData5 = info.CardSaleExtraData5; //4.0.26.510
				tagOlaRec.extraData6 = info.CardSaleExtraData6;

				_Module.m_server.m_cBaseReceipt->GetReceiptIndex((char*)tagOlaRec.tagOlaRec12.tagOlaRec11.outputData.bReceiptKey, &info, lPumpNumber);

				//if (_Module.m_server.GetOLAFlags(info.CardSaleExtraData2.sFlags, FL_CONNECTED_RECORD_IN_ASH2000_FILE))
				//{
				//	long lQdxRetVal = 0;
				//	sAsh2000Rec.lIndexNumber = trs.m_lNumber;

				//	if (lQdxRetVal = _Module.m_server.IndexOperation(ASH2000_FILE, REC_READ, &sAsh2000Rec))//4.0.3.49
				//	{
				//		if (LOG_BASIC_FLOW_CONTROL)
				//		{
				//			DWORD dwThreadId = GetCurrentThreadId();
				//			CString tmpStr;
				//			tmpStr.Format("failed=%ld to Record %ld from file Ash 2000 info", lQdxRetVal, trs.m_lNumber);
				//			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, tmpStr);
				//		}
				//	}
				//	else
				//	{
				//		bFoundAsh2000Record = TRUE;

				//		if (LOG_BASIC_FLOW_CONTROL)
				//		{
				//			CString tmpStr;
				//			tmpStr.Format("Found ash 2000 record trs=%ld", trs.m_lNumber);
				//			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, tmpStr);
				//		}
				//	}
				//}
			}
			else if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
			{
				retCode = GPI_TRS_NOT_FOUND;
			}
			else
				retCode = GPI_FAILED_PREPAY_IO;

		}
	}

	// In case Ash2000 data was found fill it in the OlaRec buffer
	if (bFoundAsh2000Record)
	{
		memcpy((char *)tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.ReceiptExtraInfo.sFiler2043, (char *)sAsh2000Rec.sData, sizeof(sAsh2000Rec.sData)); //4.0.22.511 71868
	}

	try
	{
		/*
		CID_198760 Out-of-bounds access
		EyalP: The following line was commented out after I've seen in 1032 that it doesn't appear there
		*/

		//memcpy(&tagOlaRec.tagOlaRec12.tagOlaRec11, &tagOlaRec, sizeof(tagOlaRec));	//4.0.22.511 71868

		if (lFlags & GET_PAP_AS_EXTENDED_STRUCT) //4.0.20.650 71868 //4.0.22.511
		{
			const long lOlaRecordSize = sizeof(TAG_OLA_REC13);
			BYTE sTmpAllocationBuffer[lOlaRecordSize + 2];

			Str2StrNull((BYTE *)&tagOlaRec, lOlaRecordSize, sTmpAllocationBuffer); //4.0.22.511 get tagolarec10 as in 1020. 71868

			CComBSTR strResult(sizeof(sTmpAllocationBuffer), (LPCSTR)sTmpAllocationBuffer);
			*pRec = strResult.Copy();
			strResult.Empty();			//4.0.18.501 merge from 4.0.15.285
		}
		else
		{
			char szXmlBuff[MAX_XML_BUF_LEN];
			memset(&szXmlBuff, 0, sizeof(szXmlBuff));

			if (_Module.m_server.GetMaxGrade999()) //4.0.26.501 - TD 116227 //Make extended grade conversion if needed
			{
				if (a2l(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sGrade, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sGrade)) > 0)
					_Module.m_server.GetExtGradeByGrade(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sGrade, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sGrade), lPumpNumber);
			}

			if (_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml(szXmlBuff, &tagOlaRec) == FALSE)
			{
				_LOGMSG.LogMsg("GetPAPTrs5__, Failed to convert From Struct to Xml", LOG_LEVEL_1);
			}

			CComBSTR strResult;
			if (_Module.m_server.GetAddEncodingStringToVPXml())
			{
				CString sEncodingXml;
				const char szEncodingString[] = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";

				//Add The encoding to allow all 8 bit characters
				sEncodingXml.Format("%s %s", szEncodingString, szXmlBuff);
				strResult.Append(sEncodingXml);
			}
			else
			{
				strResult.Append(szXmlBuff);
			}

			*pRec = strResult.Copy();
			strResult.Empty();
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "Threw an unknown exception", LOG_LEVEL_1);
		retCode = GPI_TRS_NOT_FOUND;
	}


	if ((retCode) &&
		(retCode != GPI_TRS_NOT_FOUND))
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPAPTrs5",
				TRUE, retCode,
				(lRtc != 0) ? TRUE : FALSE, lRtc);
	}

	if (pVal != NULL)
		*pVal = retCode;
}

/******************************************************************************
Description:	 This method is called by VP only from PumpSv version 6.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Liat			02/04/2008   10:00		Start
******************************************************************************/
void CGPI::GetPAPTrs6__(long lPumpNumber, BSTR *pRec, BSTR *pLoyaltySummaryXml, long lFlags, long *pVal)
{//4.0.19.505
	GPIRetCode	retCode = GPI_OK;
	BOOL bRetCode;
	CString strMsg;
	long lTrsNum;

	try
	{
		if (lFlags & GET_PAP_AS_STRUCT)
		{
			GetPAPTrs4__(lPumpNumber, pRec, lFlags, pVal);

		}
		else
		{
			GetPAPTrs5__(lPumpNumber, pRec, lFlags, pVal);
		}

		if (pVal != NULL)
			retCode = GPIRetCode(*pVal);

		if (retCode == GPI_OK)
		{

			CXMLPointers	pChildNode, pChildNode2;

			int iLen = 0;
			char sXmlBuff[MAX_XML_BUF_LEN];
			memset(&sXmlBuff, 0, MAX_XML_BUF_LEN);

			if (lFlags & GET_PAP_AS_STRUCT)
			{
				TAG_OLA_REC5 TagOlaRec5;   //4.0.5.0
				memset(&TagOlaRec5, ' ', sizeof(TAG_OLA_REC5));  //4.0.5.0

				iLen = Bstr2Buffer(*pRec, (void *)&TagOlaRec5, sizeof(TAG_OLA_REC5));
				if (iLen)
				{
					lTrsNum = a2l(TagOlaRec5.tagTrs.sNumber, sizeof(TagOlaRec5.tagTrs.sNumber));
					lPumpNumber = a2l(TagOlaRec5.tagTrs.sPump, sizeof(TagOlaRec5.tagTrs.sPump));
				}
				else
				{
					_LOGMSG.LogMsg("GetPAPTrs6__, Failed to convert From BSTR to buffer TAG_OLA_REC5", LOG_LEVEL_1);
					retCode = GPI_FAILED_IO;
				}

				strMsg.Format("GetPAPTrs6, Discount is %.10s", TagOlaRec5.extraData2.sTotalDiscount);
				_LOGMSG.LogMsg(strMsg);//4.0.20.507 67232


			}
			else
			{
				TAG_OLA_REC13 tagOlaRec13;	//71868 //4.0.22.511
				memset(&tagOlaRec13, ' ', sizeof(TAG_OLA_REC13));
				if (lFlags & GET_PAP_AS_EXTENDED_STRUCT)	//4.0.22.650 71868 //4.0.22.511
				{
					iLen = Bstr2Buffer(*pRec, (void *)&tagOlaRec13, sizeof(TAG_OLA_REC13));
					if (iLen)
					{
						lTrsNum = a2l(tagOlaRec13.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sNumber, sizeof(tagOlaRec13.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sNumber));
						lPumpNumber = a2l(tagOlaRec13.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sPump, sizeof(tagOlaRec13.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sPump));
					}
					else
					{
						_LOGMSG.LogMsg("GetPAPTrs6__, Failed to convert From BSTR to buffer TAG_OLA_REC13", LOG_LEVEL_1);
						retCode = GPI_FAILED_IO;

					}
				}
				else
				{
					iLen = Bstr2Buffer(*pRec, (void *)&sXmlBuff, MAX_XML_BUF_LEN);
					if (iLen)
					{
						if (!(_Module.m_server.m_cXmlConvertorEx.ConvertFromXmlToStruct(sXmlBuff, &tagOlaRec13)))
						{
							_LOGMSG.LogMsg("XmlProblem: GetPAPTrs6__, Failed to convert From xml to struct", LOG_LEVEL_1);
							retCode = GPI_FAILED_IO;

						}
					}
					else
					{
						_LOGMSG.LogMsg("GetPAPTrs6__, Failed to convert From BSTR to xml buffer", LOG_LEVEL_1);
						retCode = GPI_FAILED_IO;

					}
				}
				if (retCode == GPI_OK)
				{
					lTrsNum = a2l(tagOlaRec13.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sNumber, sizeof(tagOlaRec13.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sNumber));
					lPumpNumber = a2l(tagOlaRec13.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sPump, sizeof(tagOlaRec13.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sPump));
				}
			}
			if (retCode == GPI_OK)
			{
				AttachLoyaltySummaryXml(lTrsNum, lPumpNumber, bRetCode, retCode, lFlags, pRec, pLoyaltySummaryXml);

				_Module.m_server.m_cCommonFunction.DeleteLoyaltyXmslFile(lPumpNumber, lTrsNum);
			}
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "Throw an unknown exception", LOG_LEVEL_1);
		retCode = GPI_TRS_NOT_FOUND;
	}

	if (pVal != NULL)
		*pVal = retCode;

}

void CGPI::AttachLoyaltySummaryXml(long lTrsNum, long lPumpNumber, BOOL &bRetCode, GPIRetCode &retCode, long lFlags, BSTR * &pRec, BSTR *&pLoyaltySummaryXml)
{
	CString strMsg;
	const int GPI_LOYALTY_CORRECTION = 99;	//4.0.22.510 76232 due to resources update this retcode is different between 1020 and 1023, so a correction is needed to keep the same value.
	CXMLInterface	xmlLoyaltySummary;
	try {
		xmlLoyaltySummary.InitDoc();
		if (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())	//4.0.20.54
		{

			strMsg.Format("GetPAPTrs6__: Found PAP trs Num = %ld, pump = %ld, loyalty clientID = %ld",
				lTrsNum,
				lPumpNumber,
				_Module.m_server.GetLoyaltyUniqueClient()); // TD 321602   4.0.111.174
			_LOGMSG.LogMsg(strMsg);
			char szLoyaltyFile[MAX_FILE_PATH] = { 0 };
			_Module.m_server.m_cCommonFunction.GetLoyaltySummaryXmlFileName(lPumpNumber, lTrsNum, szLoyaltyFile); // TD 321602   4.0.111.174
			bRetCode = xmlLoyaltySummary.LoadFile(szLoyaltyFile);

			if (!bRetCode)
			{
				if (DEFAULT_LOG_LEVEL)
				{
					strMsg.Format("GetPAPTrs6, Failed to load xml from file %s", szLoyaltyFile);
					_LOGMSG.LogMsg(strMsg, LOG_LEVEL_1);
				}
				retCode = GPIRetCode(GPI_NO_LOYALTY_SUMMARY + GPI_LOYALTY_CORRECTION); //4.0.22.510 76232
			}
			else if (lFlags & GET_PAP_CONCATENAT_LOYALTY_SUMMARY)
			{
				CXMLPointers pRoot, pLoyaltyInfo, pTriggerProm, pItemElement, pItem, pItemList;
				CXMLInterface cXmlPap;
				cXmlPap.InitDoc();
				_bstr_t bstInXML(*pRec, true);
				cXmlPap.LoadXmlString(bstInXML);

				BOOL bRetCode = cXmlPap.GetRootToElement(&pRoot);
				if (bRetCode)
					bRetCode = xmlLoyaltySummary.GetRootToElement(&pLoyaltyInfo);
				if (bRetCode)
				{
					xmlLoyaltySummary.GetChildNodesList(&pLoyaltyInfo, &pItemList);

					while (xmlLoyaltySummary.GetNextItem(pItemList, pItem))
					{
						cXmlPap.AppendChild(&pItem, &pRoot);
					}
				}

				if (bRetCode)
				{
					char sXmlExtBuff[ENLARGE_XML_BUF];
					memset(&sXmlExtBuff, 0, ENLARGE_XML_BUF);
					cXmlPap.GetXmlString(sXmlExtBuff);
					CComBSTR strResult(sXmlExtBuff);
					*pRec = strResult.Copy();
#ifdef DEBUG
					cXmlPap.SaveToFile("c:\\office\\pumpsrv\\log\\GetPAPTrs5_output.xml");
#endif // DEBUG

					cXmlPap.CloseDoc();
				}
			}
			else
			{
				//4.0.23.507 91135 Extend the buffer size for a larger xml
				char sXmlExtBuff[ENLARGE_XML_BUF];
				memset(&sXmlExtBuff, 0, ENLARGE_XML_BUF);
				xmlLoyaltySummary.GetXmlString(sXmlExtBuff);
				CComBSTR strResult(sXmlExtBuff);
				*pLoyaltySummaryXml = strResult.Copy();
			}
		}
		else
		{
			retCode = GPIRetCode(GPI_NO_LOYALTY_SUMMARY + GPI_LOYALTY_CORRECTION); //4.0.22.510 76232
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "Throw an unknown exception", LOG_LEVEL_1);
		retCode = GPI_TRS_NOT_FOUND;
	}


	xmlLoyaltySummary.CloseDoc();
}

/******************************************************************************
Description:	Check and reserve pump before authorization
Used starting 1017.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella B			2/5/2007   19:20		Start
******************************************************************************/
STDMETHODIMP CGPI::CheckAndReserve(long lPumpNumber, long lPosNumber, long lPumpRes, long lStatus, BSTR sRec, long lFlags, long *lTrsNumber, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::CheckAndReserve   start");
	// TODO: Add your implementation code here	
#ifdef CATCH_GPI
	__try
	{
#endif

		CheckAndReserve__(lPumpNumber, lPosNumber, lPumpRes, lStatus, sRec, lFlags, lTrsNumber, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::CheckAndReserve"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "CheckAndReserve()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


void CGPI::CheckAndReserve__(long lPumpNumber, long lPosNumber, long lPumpRes, long lStatus, BSTR sRec, long lFlags, long *lTrsNumber, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	CString str;
	str.Format("CheckAndReserve: PumpNumber=%ld lPumpRes=%ld lStatus=%ld lFlags=%ld lTrsNumber=%ld", lPumpNumber, lPumpRes, lStatus, lFlags, *lTrsNumber);
	_LOGMSG.LogMsg(str);

	if (lPumpNumber <= 0)
		retCode = GPI_INVALID_PUMP_NUMBER;

	SERVER_INFO sInfo;

	// Before starting check and reserve, check we are not in middle of other process
	_Module.m_server.m_cServerInfo.GetInfo(sInfo);


	if (sInfo.byRequiredMaintenanceProcess && lStatus != CHECK_RESERVE_STATUS_END) //4.0.19.504 - TD 59547
	{
		retCode = GPI_IN_MIDDLE_OF_PROCESS;
	}

	if (retCode == GPI_OK)
	{
		// Mark check and reserve flag in any case
		lFlags |= CHECK_AND_RESERVE_FLAG;

		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		CPumpStatus stat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();

		switch (lPumpRes)
		{
		case PRE_PAY_AT_POS:
		{
			if (lStatus == CHECK_RESERVE_STATUS_END)
				lFlags |= PREPAY_CANCEL;
			else if (lStatus == CHECK_RESERVE_STATUS_START)
			{
				*lTrsNumber = 0;
				////4.0.26.512 First, Search for exiting WaitProgress or In Progress Trs on pump:
				CPumpTransact cTrs;
				BOOL bTrsNotFound = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetInProgressWaitProgressTrs(&cTrs);

				BOOL bCanReserve = _Module.m_server.m_cPumpArray[lPumpNumber - 1].CanReserveForPrePay();

				if (!bCanReserve && !bTrsNotFound)	//4.0.26.512 TD 215073 	
				{
					retCode = GPI_CANT_RESERVEPUMP;
					if (!bTrsNotFound)
					{
						str.Format("CheckAndReserve Failed. Found an active transaction number: %ld on Pump.", cTrs.m_lNumber);
						_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
					}
				}

				if ((lFlags & CHECK_WITHOUT_RESERVE) == 0)							//4.0.19.1250 - TD 236644
				{
					*lTrsNumber = 0;

					//Save the start time
					stat.UpdateTime();
					_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetStat(stat);
				}
			}

			if (retCode == GPI_OK && ((lFlags & CHECK_WITHOUT_RESERVE) == 0))			//4.0.19.1250 - TD 236644
			{
				//Save the start time //4.0.26.512
				stat.UpdateTime();
				_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetStat(stat);

				AuthorizeAndLimit__(lPumpNumber, 0, 0, 0, 0, lFlags, lTrsNumber, pVal, sRec);
			}
		}
		break;
		case PAY_AT_KIOSK:
		{
			if (lStatus == CHECK_RESERVE_STATUS_END)
			{
				//In case of End check and reserve, remove the misc
				if (lPumpNumber > 0)
				{
					long lMisc = stat.GetMisc();
					if (lMisc & MISC_PUMP_CHECK_AND_RESERVE)
					{
						lMisc &= MISC_NOT_PUMP_CHECK_AND_RESERVE;
						stat.SetMisc(lMisc);
						_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetStat(stat);
					}
				}

				_Module.m_server.SetCommand(CMD_GET_PUMP_STATUS, SERVER_POS_NUMBER, lPumpNumber); //4.0.31.0 - TD 324520
			}
			else if (lStatus == CHECK_RESERVE_STATUS_START)
			{
				BOOL bCanReserve = _Module.m_server.m_cPumpArray[lPumpNumber - 1].CanReserve();

				if (lFlags & CHECK_WITHOUT_RESERVE)							//4.0.19.1250 - TD 236644 // 4.0.29.500 - TD 248476
				{
					if (bCanReserve == FALSE && stat.GetStatus() != REQUESTING)
						retCode = GPI_CANT_RESERVEPUMP;
					else
					{
						//4.0.28.500 - TD 248476
						//check also if can be reserved for PrePay
						BOOL bCanReserveForPrP = _Module.m_server.m_cPumpArray[lPumpNumber - 1].CanReserveForPrePay();
						if (bCanReserveForPrP == FALSE)
							retCode = GPI_CANT_RESERVEPUMP;
					}
				}
				else
				{
					//Save the start time
					stat.UpdateTime();
					_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetStat(stat);

					if (bCanReserve == TRUE || stat.GetStatus() == REQUESTING) //4.0.17.500
					{
						AuthorizePump2__(lPumpNumber, 0, 0, lFlags, 0, 0, pVal);
					}
					else
					{
						retCode = GPI_CANT_RESERVEPUMP;
					}
				}
			}
		}
		break;
		case NULL_RES:				//4.0.19.1250 - TD 236644
		{
			if ((lFlags & CHECK_WITHOUT_RESERVE) == 0 || (_Module.m_server.m_cPumpArray[lPumpNumber - 1].CanReserveForPrePay() == FALSE))
				retCode = GPI_UNKNOWN_PUMP_RESERVATION;
		}
		break;
		default:
		{
			retCode = GPI_UNKNOWN_PUMP_RESERVATION;
		}
		break;
		}
	}


	if (retCode)
		if (pVal != NULL)
			*pVal = retCode;

	str.Format("CheckAndReserve: retCode = %ld", *pVal);
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
}


/******************************************************************************
Description:	 This method is called by PosPump only from version 1017 and on.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amit H			9/5/2007   19:20		Start
******************************************************************************/
STDMETHODIMP CGPI::GetPumpTransact5(long lPumpNumber, long lTrsNumber, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPumpTransact5   start");
	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPumpTransact5, Throw COM Exception : E_POINTER", LOG_LEVEL_0);

		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetPumpTransact5__(lPumpNumber, lTrsNumber, pRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetPumpTransact5"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPumpTransact5()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;

}

void CGPI::GetPumpTransact5__(long lPumpNumber, long lTrsNumber, BSTR *pRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	TAG_PUMP_TRANSACT5	trs;
	memset(&trs, ' ', sizeof(TAG_PUMP_TRANSACT5));



	if ((retCode == GPI_OK) || (retCode == GPI_SYSTEM_DOWN_PAK_ENABLED))
	{
		int		iRetNum = 0;
		long	p = lPumpNumber - 1;


		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		iRetNum = _Module.m_server.m_cPumpArray[p].GetTagTrsByNumber(lTrsNumber, &trs, 2);

		if (iRetNum != 0)
			retCode = GPI_TRS_NOT_FOUND;
	}

	if (retCode)
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPumpTransact5",
				TRUE, retCode,
				FALSE, 0);
	}

	BYTE sTmp[sizeof(TAG_PUMP_TRANSACT5) + 2];
	Str2StrNull((BYTE *)&trs, sizeof(TAG_PUMP_TRANSACT5), sTmp);
	CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
	*pRec = strResult.Copy();

	if (pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGPI::GetPAKQuery2(BSTR *sData, long *plRecCount, long lFlags, long *plRetVal)
{
	LOG_EXTERNALCALL("CGPI::GetPAKQuery2   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		GetPAKQuery2__(sData, plRecCount, lFlags, plRetVal);
#ifdef CATCH_GPI
	}

	__except (_HANDLE_EXCEPTION("CGPI::GetPAKQuery2"))
	{
		_LOGMSG.LogClassMsg("CGPI", "GetPAKQuery2", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}

/******************************************************************************
Description:	This method is called by PosPump only from version 1017 and on.
Returns:
Parameters:   	sData - a buffer containing lRecCount TAG_PUMP_TRANSACT2 appended
one after the other
lRecCount - number of TAG_PUMP_TRANSACT5 in sData
lFlags - reserved for future use, set to 0
plRetVal - a GPI return code
Assumptions:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amit H			09/05/2007   15:17		Start
******************************************************************************/
void CGPI::GetPAKQuery2__(BSTR * sData, long * plRecCount, long lFlags, long *plRetVal)
{
	GetPAKQueryRetCode eRetCode;
	long lRetVal = GPI_OK;


	if (!m_bInitDone)
		lRetVal = GPI_MUST_INIT_FIRST;


	eRetCode = _Module.m_server.m_cCommonFunction.GetPAKQuery2(sData, plRecCount, &lFlags);

	switch (eRetCode)
	{
	case GetPakQuery_OK:
		lRetVal = GPI_OK;
		break;
	case GetPakQuery_MissingData:
		lRetVal = GPI_LAST_ERROR;
		break;
	}


	if (LOG_LOGIC_ERROR && lRetVal)
		_LOGMSG.LogResult(
			0, // Src
			"IGPI",
			"GetPAKQuery2",
			TRUE, lRetVal,
			FALSE, 0);

	if (plRetVal != NULL)
		*plRetVal = lRetVal;

}
/******************************************************************************
Description: This method is called by PosPump only from version 1017 and on.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amit H			9/5/2007   19:13		Start
******************************************************************************/
STDMETHODIMP CGPI::QueryPrePaidTrs2(long lPumpNumber, long lIndexNumber, short nFlags, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::QueryPrePaidTrs2   start");
	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "QueryPrePaidTrs2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);

		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		long lFlag2 = 0;
		lFlag2 |= QUERY_FLAGS2;
		QueryPrePaidTrs__(lPumpNumber, lIndexNumber, nFlags, pRec, pVal, lFlag2);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::QueryPrePaidTrs2"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "QueryPrePaidTrs2", LOG_LEVEL_0);
	}
#endif 

	return S_OK;

}

/******************************************************************************
Description: This method is called by PosPump only from version 1017 and on.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amit H			9/5/2007   19:13		Start
******************************************************************************/
STDMETHODIMP CGPI::GetAllTrsQuery2(long lPumpNumber, BSTR *sData, long *plRecCount, long lFlags, long *plRetVal)
{
	LOG_EXTERNALCALL("CGPI::GetAllTrsQuery2   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		GetAllTrsQuery2__(lPumpNumber, sData, plRecCount, lFlags, plRetVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetAllTrsQuery2"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetAllTrsQuery2", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

	}
#endif 

	return S_OK;
}

/******************************************************************************
Description: Query data for all transaction , include  ability to restrict this query per pump
This method is called by PosPump only from version 1017 and on.
Returns:    Transaction array
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amit H			9/5/2007   19:13		Start
******************************************************************************/
void CGPI::GetAllTrsQuery2__(long lPumpNumber, BSTR *sData, long *plRecCount, long lFlags, long *plRetVal)
{
	GPIRetCode retCode = GPI_OK;
	GetPAKQueryRetCode eRetCode;
	long lRetVal = GPI_OK;


	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		retCode = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GPI_SERVER_NOT_READY;

	else if (lPumpNumber)
	{
		if (PUMP_NOT_VALID(lPumpNumber))
			retCode = GPI_INVALID_PUMP_NUMBER;
	}


	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("GetAllTrsQuery2, request for PumpNumber %02ld flags=%x",
			lPumpNumber, lFlags);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
	}


	if (retCode == GPI_OK)
	{
		eRetCode = _Module.m_server.m_cCommonFunction.GetPAKQuery2(sData, plRecCount, &lFlags, NULL, lPumpNumber);

		switch (eRetCode)
		{
		case GetPakQuery_OK:
			lRetVal = GPI_OK;
			break;
		case GetPakQuery_MissingData:
			lRetVal = GPI_LAST_ERROR;
			break;
		}

	}


	if (LOG_LOGIC_ERROR && lRetVal)
		_LOGMSG.LogResult(
			0, // Src
			"IGPI",
			"GetAllTrsQuery2",
			TRUE, lRetVal,
			FALSE, 0);

	if (plRetVal != NULL)
		*plRetVal = lRetVal;
}

/******************************************************************************
Description: This method is called by PosPump only from version 1017 and on.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amit H			9/5/2007   19:13		Start
******************************************************************************/
STDMETHODIMP CGPI::GetOLAMemInfo3(long lPumpNumber, short *nIndex, long lFlags, long lVal, BSTR sVal, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetOLAMemInfo3   start");
	USES_CONVERSION;

#ifdef CATCH_GPI
	__try
	{
#endif
		long lFlag2 = 0;
		lFlag2 |= OLA_MEM_FLAGS2;
		GetOLAMemInfo2__(lPumpNumber, nIndex, lFlags, lVal, sVal, pRec, pVal, lFlag2);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetOLAMemInfo3"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetOLAMemInfo3()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}


/******************************************************************************
Description:	Get sale extra details - used by TwerminalSrv
Returns:      	void
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			30/03/2008   12:00		Start		4.0.19.504 - 47559
******************************************************************************/
STDMETHODIMP CGPI::GetSaleExtraDetails(long lPumpNumber, long lFlags, BSTR sData, BSTR *sRetData, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetSaleExtraDetails   start");
#ifdef CATCH_GPI
	try
	{
#endif

		_Module.m_server.GetSaleExtraDetails__(lPumpNumber, lFlags, sData, sRetData, pVal);

#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetSaleExtraDetails()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}


/******************************************************************************
Description:	Set pump's misc
Returns:      	void
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			30/03/2008   12:00		Start		4.0.19.504 - 47559
******************************************************************************/
STDMETHODIMP CGPI::SetPumpMisc(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetPumpMisc   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	try
	{
#endif

		SetPumpMisc__(lPumpNumber, lMisc, bRemoveMisc, pVal);

#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetPumpMisc()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


void  CGPI::SetPumpMisc__(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal)
{
	_Module.m_server.m_cCommonFunction.SetPumpMisc(lPumpNumber, lMisc, bRemoveMisc, pVal);
}


/******************************************************************************
Description:	Set Alarm to AlarmSrv
Returns:      	void
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Amit H			13/11/2007   19:31		Start		4.0.19.504 - 47559
******************************************************************************/
STDMETHODIMP CGPI::SetAlarm(long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus, long lAlarmValue, long lFlags, long *pAlarmId, BSTR sRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetAlarm   start");
#ifdef CATCH_GPI
	try
	{
#endif

		SetAlarm__(lCatagory, lSubCatagory, lAlarmNumber, lDevice, lAlarmStatus, lAlarmValue, lFlags, pAlarmId, sRec, pVal);

#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetAlarm()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


void CGPI::SetAlarm__(long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus, long lAlarmValue, long lFlags, long *pAlarmId, BSTR sRec, long *pVal)
{
	char		 sMessages[MAX_ALARM_DESC_LEN];
	_bstr_t		bstrMessage(sRec);
	GPIRetCode retCode = GPI_OK;

	long lRetVal = 0;

	if (bstrMessage.length())
	{
		memset(sMessages, 0, sizeof(sMessages));
		memcpy(sMessages, (const char *)bstrMessage, min(sizeof(sMessages), bstrMessage.length()));					//4.0.25.80 TD 142857
		lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(lCatagory, lSubCatagory, lAlarmNumber, lDevice, lAlarmStatus, lAlarmValue, lFlags, sMessages);
	}
	else
	{
		lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(lCatagory, lSubCatagory, lAlarmNumber, lDevice, lAlarmStatus, lAlarmValue, lFlags);
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{

		CString sMsg;
		sMsg.Format("POS Raise Alarm, lCatagory=%ld ,lSubCatagory=%ld,lAlarmNumber=%ld,lDevice=%ld,lAlarmStatus=%ld,lAlarmValue=%ld , lRetVal=%ld ",
			lCatagory, lSubCatagory, lAlarmNumber, lDevice, lAlarmStatus, lAlarmValue, lRetVal);

		_LOGMSG.LogReq(m_lPosNumber,
			"IGPI",
			"SetTrsState",
			(LPCSTR)sMsg);
	}

	if (lRetVal)
		retCode = GPI_FAILED_IO;

	if (pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGPI::PAPNoFuelReadyTrs(long lPumpNumber, long lFlags, BSTR sData, BSTR sParam1, long lParam2, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::PAPNoFuelReadyTrs   start");
#ifdef CATCH_GPI
	try
	{
#endif

		PAPNoFuelReadyTrs__(lPumpNumber, lFlags, sData, sParam1, lParam2, pVal);

#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "PAPNoFuelReadyTrs()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}



void CGPI::PAPNoFuelReadyTrs__(long lPumpNumber, long lFlags, BSTR sData, BSTR sParam1, long lParam2, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	DWORD		dwLastErr = 0;


	CARD_SALE_ALL3 cardDataAll3;
	memset(&cardDataAll3, 0, sizeof(CARD_SALE_ALL3));

	int iLen = Bstr2Buffer(sData, (void *)&cardDataAll3, sizeof(CARD_SALE_ALL3));


	PAY_AT_PUMP_INFO	info;
	memset(&info, 0, sizeof(info));

	if (iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GPI_LAST_ERROR;

		if (LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("lFlags=%ld ,ilen=%d, last error= %ld retCode=%d ", lFlags, iLen, dwLastErr, retCode);
			_LOGMSG.LogMsg("GPI", "PAPNoFuelReadyTrs__", lPumpNumber, LOG_PUMP, str);
		}

	}

	if (retCode == GPI_OK)
	{
		info.CardSaleInfo.cardData = cardDataAll3.CardSaleAll.data;
		info.CardSaleInfo.extraData = cardDataAll3.CardSaleAll.extData;
		info.CardSaleExtraData2 = cardDataAll3.extData2;
		info.CardSaleExtraData3 = cardDataAll3.extData3;

		if (LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("lFlags=%ld ,ilen=%d  ", lFlags, iLen);
			_LOGMSG.LogMsg("GPI", "PAPNoFuelReadyTrs__", lPumpNumber, LOG_PUMP, str);
		}

		CPumpTransact		trs(lPumpNumber);
		long p = lPumpNumber - 1;
		trs.m_lNumber = _Module.m_server.NewTransactNumber();
		trs.m_lStatus = PAID;
		trs.m_nLinkFlags = TRS_NO_LINK;
		trs.m_lRes = PAY_AT_PUMP;
		trs.m_lResExt = RES_EXT_PAP_EXTERNAL;
		trs.m_lActiveReadNumber = _Module.m_server.GetActiveReadNumber();
		trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[p].GetShiftNumber();

		trs.m_lPump = lPumpNumber;
		trs.m_lLinkNumber = 0;		//ToCheck
		trs.m_nNozzle = 0;
		trs.m_lRoundedValue = 0;
		trs.m_lRoundedVolume = 0;
		trs.m_nLevel = 0;
		trs.m_lGradePrice = 0;
		trs.m_nGrade = 0;
		trs.UpdateTime();	//To check the time


		info.CardSaleInfo.lIndexNumber = _Module.m_server.NewOLAIndex();

		trs.SetLink(TRS_LINK_OLA_NUMBER, info.CardSaleInfo.lIndexNumber);

		long lRetCode = _Module.m_server.TrsLinkOperation(REC_INSERT, &trs, &info);

	}


	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGPI",
				"PAPNoFuelReadyTrs__",
				TRUE, retCode,
				(dwLastErr != 0) ? TRUE : FALSE, dwLastErr);


	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;

		str.Format("PAPNoFuelReadyTrs__(Pump %02ld, Flags=%ld)", lPumpNumber, lFlags);

		_LOGMSG.LogMsgWithRet(0, LOG_NONE, str, "IGPI", retCode);
	}

	if (pVal != NULL)
		*pVal = retCode;

}


STDMETHODIMP CGPI::GetPAPTrs6(long lPumpNumber, BSTR *pRec, BSTR *pLoyaltySummaryXml, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPAPTrs6   start");
	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPAPTrs6, Throw COM Exception : E_POINTER", LOG_LEVEL_0);



		return E_POINTER;
	}

#ifdef CATCH_GPI
	__try
	{
#endif
		GetPAPTrs6__(lPumpNumber, pRec, pLoyaltySummaryXml, lFlags, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetPAPTrs6"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPAPTrs6", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

/******************************************************************************
Description: This method is called by TerminalSrv in order to authorize PumpSrv.
It is same as AuthorizeAndLimit2 but with an additonal argument: lPurchaseID.
lPurchaseID would be the unique number for the saving (a prepay like) transaction.

Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella/AmitH		12/06/2008   19:13		Start  4.0.20.54 Merge from 4.0.19.180 58220
******************************************************************************/
STDMETHODIMP CGPI::AuthorizeAndLimit3(long lPumpNumber, short nLevel, long lGradePrice, long lMaxValue, long lMaxVolume, long lFlags, long *lSyncNumber, BSTR sRec, long lPurchaseID, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::AuthorizeAndLimit3   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		AuthorizeAndLimit2__(lPumpNumber, nLevel, lGradePrice, lMaxValue, lMaxVolume, lFlags, lSyncNumber, sRec, pVal, lPurchaseID);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::AuthorizeAndLimit3"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "AuthorizeAndLimit3()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

/******************************************************************************
Description: This method is called by TerminalSrv in order to finalize a prepay trs.
It searches for the right prepay transaction according to the lPurchaseID.
If transaction found, PumpSrv will Save its record in OLA QDX.
PumpSrv will Remove the record from PREAPY QDX and should not save the record to RECEIPT QDX.

Returns:     Zero on success.

Parameters:  BSTR sRec - is for future use.

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
AmitH		12/06/2008   19:13		Start   4.0.19.180
******************************************************************************/
STDMETHODIMP CGPI::FinalizePrePayTrs(long lPumpNumber, long lFlags, long lFlags2, long lSyncNumber, BSTR sRec, long lPurchaseID, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::FinalizePrePayTrs   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		FinalizePrePayTrs__(lPumpNumber, lFlags, lFlags2, lSyncNumber, sRec, lPurchaseID, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::FinalizePrePayTrs"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "FinalizePrePayTrs()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 


	return S_OK;
}

/******************************************************************************
FinalizePrePayTrs
Description:  added support in GPI_EVENT_AUTO_RECONCILE flags.
In case GPI_EVENT_AUTO_RECONCILE flag recived

Returns:

Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
OmerT		    05/09/2011   19:13		Start   // TD 233242 4.0.23.1050
******************************************************************************/
void CGPI::FinalizePrePayTrs__(long lPumpNumber, long lFlags, long lFlags2, long lSyncNumber, BSTR sRec, __int64 lPurchaseID, long *pVal)
{
	if (lFlags & GPI_EVENT_AUTO_RECONCILE)
	{
		// send event to RFS,  
		_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, lPumpNumber, GCI_EVENT_AUTO_RECONCILE);
	} // TD 233242 4.0.23.1050
	else
	{


		long lRetVal = GPI_OK;
		int iLen = 0;
		DWORD dwLastErr = 0;
		CPumpTransact cPumpTrs;
		CARD_SALE_ALL3 cardDataAll3;

		memset(&cardDataAll3, 0, sizeof(CARD_SALE_ALL3));

		iLen = Bstr2Buffer(sRec, (void *)&cardDataAll3, sizeof(CARD_SALE_ALL3));

		if (iLen == 0)
		{
			dwLastErr = GetLastError();
			lRetVal = GPI_LAST_ERROR;
		}
		if (lRetVal == GPI_OK)
		{
			//Finds the prepay according to our transaction number
			lRetVal = GetPrepayBySyncNumber(lSyncNumber, cPumpTrs);

			//Copy the trs date from the trs into CardSaleData3
			CString sTmpDateAndTime;
			CTime  sTagTime = cPumpTrs.GetTime();

			sTmpDateAndTime.Format("%02d%02d%02d%02d%02d%02d",
				((sTagTime.GetYear() >= 2000) ? (sTagTime.GetYear() - 2000) : (sTagTime.GetYear() - 1900)),
				sTagTime.GetMonth(),
				sTagTime.GetDay(),
				sTagTime.GetHour(),
				sTagTime.GetMinute(),
				sTagTime.GetSecond());

			memcpy(cardDataAll3.CardSaleAll.data.sDateTime, sTmpDateAndTime.GetBuffer(), MIN(sizeof cardDataAll3.CardSaleAll.data.sDateTime, sTmpDateAndTime.GetLength()));
			memcpy(cardDataAll3.CardSaleAll.extData.sDateTimeExt, sTmpDateAndTime.GetBuffer() + sizeof cardDataAll3.CardSaleAll.data.sDateTime, MIN(sizeof cardDataAll3.CardSaleAll.extData.sDateTimeExt, sTmpDateAndTime.GetLength()));


		}
		if (lRetVal == GPI_OK)
		{
			//Creates OLA record form TAG_PUMP_TRANSACT5 & sRec
			lRetVal = CreateAndSaveOLARec(cPumpTrs, cardDataAll3);
		}

		//4.0.28.500 - TD 240006/251326
		if ((lRetVal == GPI_OK) && (lFlags & FINALIZE_PREPAY_CREATE_RECEIPT_REC))
		{
			CreateAndSaveReceiptRec(cPumpTrs, cardDataAll3);
		}

		if (lRetVal == GPI_OK)
		{
			//Deletes the prepay record form the prepay qdx file
			RemovePrePaidTrs__(lPumpNumber, lSyncNumber, &lRetVal);
		}

		if (lRetVal)
		{
			if (LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					m_lPosNumber, // Src
					"IGPI",
					"FinalizePrePayTrs",
					TRUE, lRetVal,
					FALSE, 0);
		}


		if (pVal != NULL)
			*pVal = lRetVal;
	}

}

/******************************************************************************
Description: This method founds prepay transaction according to PurchaseId.
If found and caller want to lock it, it will be locked for him.

Returns:     Zero on success.

Parameters:  long lPurchaseId - search will be according to it
short nFlags - came form the interface (GetPrePaidTrs2)

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Yaron		15/06/2008   19:13		Start   4.0.19.180
******************************************************************************/
GPIRetCode CGPI::GetPrepayByPurchaseId(__int64 lPurchaseId, short nFlags, TAG_PUMP_TRANSACT5 &tagTrs)
{
	GPIRetCode lReadOpRetCode = GPI_OK;
	PRE_PAY_INFO		info;
	BOOL bClosedTrsFound = FALSE;
	long lRtc = 0;
	CPumpTransact		trs;

	memset(&info, 0x00, sizeof(PRE_PAY_INFO));


	trs.SetLink(TRS_LINK_PREPAY_NUMBER, (long)lPurchaseId);

	lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST, &trs, &info);
	//4.0.5.39

	while ((lRtc == OK) && !bClosedTrsFound)
	{
		if (lRtc == OK)
		{
			long lTrsStatus = trs.GetStatus();

			if (((nFlags & PREPAY_GET_BY_PURCHASE_ID_PAID_ONLY) && lTrsStatus == PAID) ||
				((nFlags & PREPAY_GET_BY_PURCHASE_ID) && (lTrsStatus == WAIT_PROGRESS || lTrsStatus == WAIT_TIME_OUT)))
			{
				if (lPurchaseId == info.lPurchaseID)
				{
					bClosedTrsFound = TRUE;
					lReadOpRetCode = GPI_OK;

					if (nFlags & GPPT_OFFER_TRS_TO_POS)
					{
						lRtc = 0;

						if (info.nOwningPos != m_lPosNumber) //Transaction is owned by another POS or not owned at all
						{
							if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
							{
								lReadOpRetCode = GPI_TRS_LOCKED;
								lRtc = ERR_EXISTS;
							}
							else //Transaction isn't owned by anyone, We'll acquire ownership
							{
								info.nOwningPos = (short)m_lPosNumber;
								lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &trs, &info);
							}
						}
					}

					if (lReadOpRetCode == GPI_OK)
					{
						trs.GetTag(&tagTrs, 2, info.lMaxValue, 0, info.nGrade2, info.lPurchaseID); //4.0.9.507, 4.0.19.180									
					}
				}
			}
			else if ((nFlags & PREPAY_GET_BY_PURCHASE_ID_PAID_ONLY) && lTrsStatus == IN_PROGRESS && (lPurchaseId == info.lPurchaseID))
			{
				bClosedTrsFound = TRUE;
				lReadOpRetCode = GPI_TRS_ALREADY_IN_PROGRESS;
			} // 4.0.19.2020  TD 380760

			if (!bClosedTrsFound) //4.0.11.30
				lRtc = _Module.m_server.TrsLinkOperation(REC_READ_NEXT, &trs, &info);

		}
		else
		{
			lReadOpRetCode = GPI_TRS_NOT_FOUND;
		}
	}

	if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
	{
		lReadOpRetCode = GPI_TRS_NOT_FOUND;
	}
	else if (lReadOpRetCode && (lReadOpRetCode != GPI_TRS_LOCKED) && (lReadOpRetCode != GPI_TRS_ALREADY_IN_PROGRESS)) // 4.0.19.2020  TD 380760
		lReadOpRetCode = GPI_FAILED_PREPAY_IO;


	return lReadOpRetCode;
}


/******************************************************************************
Description: This method founds prepay transaction according to
SyncNumber = PumpSrv Trs number.


Returns:     Zero on success.

Parameters:  long SyncNumber - search will be according to it
long nFlags - came form the interface (GetPrePaidTrs2)

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Yaron		15/06/2008   19:13		Start   4.0.19.180
******************************************************************************/
GPIRetCode CGPI::GetPrepayBySyncNumber(long lSyncNumber, CPumpTransact &cPumpTrs)
{
	GPIRetCode retCode = GPI_OK;
	PRE_PAY_INFO		info;
	long lRtc = 0;

	memset(&info, 0x00, sizeof(PRE_PAY_INFO));

	cPumpTrs.SetLink(TRS_LINK_PREPAY_NUMBER, lSyncNumber);
	lRtc = _Module.m_server.TrsLinkOperation(REC_READ, &cPumpTrs, &info);

	if (LOG_BASIC_FLOW_CONTROL)  //4.0.11.509
	{
		CString str;
		str.Format("GetPrepayBySyncNumber::POS - %ld , lSyncNumber %ld ", m_lPosNumber, lSyncNumber);
		_LOGMSG.LogMsg(str);
	}

	if (lRtc == OK)
	{
		if ((cPumpTrs.m_lStatus == WAIT_RES) ||
			(cPumpTrs.m_lStatus == WAIT_AUTH) ||
			(cPumpTrs.m_lStatus == WAIT_PROGRESS) ||
			(cPumpTrs.m_lStatus == IN_PROGRESS))
		{
			retCode = GPI_TRS_LOCKED;

			if (LOG_BASIC_FLOW_CONTROL)  //4.0.11.118
			{
				CString str;
				str.Format("GetPrepayBySyncNumber::POS - %ld ,Trs locked lSyncNumber %ld ", m_lPosNumber, lSyncNumber);
				_LOGMSG.LogMsg(str);
			}

		}
		else
		{
			if (lRtc == OK)
			{
				//Checking if the Trs is owned by the calling POS

				if (info.nOwningPos != m_lPosNumber) //Transaction is owned by another POS or not owned at all
				{
					if (info.nOwningPos != NO_POS_OWNER) //Transaction is owned by another POS
					{
						retCode = GPI_TRS_LOCKED;
						lRtc = ERR_EXISTS;
					}
				}

			}
		}

	}
	else if ((lRtc == ERR_NOT_FOUND) || (lRtc == ERR_DELETED))
	{
		retCode = GPI_TRS_NOT_FOUND;
	}
	else if (retCode != GPI_TRS_LOCKED)
		retCode = GPI_FAILED_PREPAY_IO;


	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("GetPrepayBySyncNumber::Pos %ld trs %ld ,retCode=%d , lRtc=%ld ", m_lPosNumber, lSyncNumber, retCode, lRtc);
		_LOGMSG.LogMsg(str);
	}

	return retCode;
}

/******************************************************************************
Description: This method creates OLA record from Trs data and Card data


Returns:     Zero on success.

Parameters:  CPumpTransact cPumpTrs - Transaction data
CARD_SALE_ALL3 cardDataAll3 - Card data

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Yaron		15/06/2008   19:13		Start   4.0.19.180
******************************************************************************/
GPIRetCode CGPI::CreateAndSaveOLARec(CPumpTransact cPumpTrs, CARD_SALE_ALL3 cardDataAll3)
{
	GPIRetCode retCode = GPI_OK;
	PAY_AT_PUMP_INFO	info;
	EXTRA_INFO_REC sExtraInfoRec; //4.0.23.509 97360

	memset(&info, 0, sizeof(PAY_AT_PUMP_INFO));

	memcpy(&info.CardSaleInfo.cardData, &cardDataAll3.CardSaleAll.data, sizeof(cardDataAll3.CardSaleAll.data));
	memcpy(&info.CardSaleInfo.extraData, &cardDataAll3.CardSaleAll.extData, sizeof(cardDataAll3.CardSaleAll.extData));
	memcpy(&info.CardSaleExtraData2, &cardDataAll3.extData2, sizeof(cardDataAll3.extData2));
	memcpy(&info.CardSaleExtraData3, &cardDataAll3.extData3, sizeof(cardDataAll3.extData3));
	memcpy(&info.CardSaleExtraData4, &cardDataAll3.extData4, sizeof(cardDataAll3.extData4));
	memcpy(&info.CardSaleExtraData5, &cardDataAll3.extData5, sizeof(cardDataAll3.extData5));
	memcpy(&info.CardSaleExtraData6, &cardDataAll3.extData6, sizeof(cardDataAll3.extData6));

	//4.0.23.509 97360 start
	if (cPumpTrs.m_lNumber)
	{
		sExtraInfoRec.ClearRecord(); // RFUEL-3708

		sExtraInfoRec.info.lIndexNumber = cPumpTrs.m_lNumber;

		long lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &sExtraInfoRec);
		if (lRtc == OK)
		{
			memcpy(&info.CardSaleExtraData5.sPreDispensingVolumeHigh, &sExtraInfoRec.info.sPreDispensingVolumeHigh, sizeof(info.CardSaleExtraData5.sPreDispensingVolumeHigh));
			memcpy(&info.CardSaleExtraData5.sPreDispensingVolumeLow, &sExtraInfoRec.info.sPreDispensingVolumeLow, sizeof(info.CardSaleExtraData5.sPreDispensingVolumeLow));
			memcpy(&info.CardSaleExtraData5.sPostDispensingVolumeHigh, &sExtraInfoRec.info.sPostDispensingVolumeHigh, sizeof(info.CardSaleExtraData5.sPostDispensingVolumeHigh));
			memcpy(&info.CardSaleExtraData5.sPostDispensingVolumeLow, &sExtraInfoRec.info.sPostDispensingVolumeLow, sizeof(info.CardSaleExtraData5.sPostDispensingVolumeLow));
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("CreateAndSaveOLARec, volume counters were copied from extra info to ola");
				_LOGMSG.LogMsg(cPumpTrs.m_lPump, LOG_PUMP, str);
			}
		}
		else
		{
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("CreateAndSaveOLARec, faield to read from extra info, retcode = %ld", lRtc);
				_LOGMSG.LogMsg(cPumpTrs.m_lPump, LOG_PUMP, str);
			}
		}
	}

	cPumpTrs.m_lStatus = PAID;
	//4.0.23.509 97360 end

	info.CardSaleInfo.lIndexNumber = _Module.m_server.NewOLAIndex();
	cPumpTrs.SetLink(TRS_LINK_OLA_NUMBER, info.CardSaleInfo.lIndexNumber);

	long lRtc = _Module.m_server.TrsLinkOperationEx(REC_INSERT, &cPumpTrs, &info);

	if (lRtc)
	{
		retCode = GPI_FAILED_IO;
	}
	else
	{
		//Throw event to VP to take the trs:
		_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_PAP_TRS, 0);
	}

	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("CreateAndSaveOLARec::Pos %ld trs %ld ,retCode=%d , lRtc=%ld ", m_lPosNumber, cPumpTrs.m_lNumber, retCode, lRtc);
		_LOGMSG.LogMsg(str);
	}

	return retCode;
}


/******************************************************************************
Description:	Get PAK transaction by trs number
Returns:      	GPI ret code
Parameters:    long lPumpNumber, long lSyncNumber, long lFlags, BSTR *pRec, long *pVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			12/08/2008   13:00		Start		4.0.20.501 - TD 65908
******************************************************************************/
STDMETHODIMP CGPI::GetPAKTrs(long lPumpNumber, long lSyncNumber, long lFlags, BSTR *pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetPAKTrs   start");

#ifdef CATCH_GPI
	try
	{
#endif
		GetPAKTrs__(lPumpNumber, lSyncNumber, lFlags, pRec, pVal);

#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPAKTrs()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


/******************************************************************************
Description:	Get PAK transaction by trs number
Returns:      	GPI ret code
Parameters:    long lPumpNumber, long lSyncNumber, long lFlags, BSTR *pRec, long *pVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			12/08/2008   13:00		Start		4.0.20.501 - TD 65908
******************************************************************************/
void CGPI::GetPAKTrs__(long lPumpNumber, long lSyncNumber, long lFlags, BSTR *pRec, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	CString str;
	str.Format("GPI::GetPAKTrs__() flags = %ld SyncNumber = %ld", lFlags, lSyncNumber);
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

	if (retCode == GPI_OK)
	{
		CPumpTransact	trs;

		int iRetVal = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsByNumber(lSyncNumber, &trs);

		if (!iRetVal)
		{
			TAG_PUMP_TRANSACT5	cTagPumpTrs;
			memset(&cTagPumpTrs, ' ', sizeof(TAG_PUMP_TRANSACT5));	//?.??.?.?? - TD 335186
			trs.GetTag(&cTagPumpTrs, 2);

			//4.0.26.501 - TD 116227
			//Make extended grade conversion if needed
			if (_Module.m_server.GetMaxGrade999() && !(lFlags & IGNORE_EXTENDED_GRADE_ID_CONVERSION))
				_Module.m_server.GetExtGradeByGrade(cTagPumpTrs.sGrade, sizeof(cTagPumpTrs.sGrade), lPumpNumber);


			BYTE sTmpStr[sizeof(TAG_PUMP_TRANSACT5) + 2];
			Str2StrNull((BYTE *)&cTagPumpTrs, sizeof(TAG_PUMP_TRANSACT5), sTmpStr);
			CComBSTR strResult(sizeof(sTmpStr), (LPCSTR)sTmpStr);
			*pRec = strResult.Copy();
		}
		else
		{
			retCode = GPI_TRS_NOT_FOUND;
		}
	}

	if (pVal != NULL)
		*pVal = retCode;

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"GetPAKTrs",
				TRUE, retCode,
				FALSE, 0);
}

/******************************************************************************
Description:	Finalize PAK transaction by its trs number and save it as PAP.

Returns:      	GPI ret code
Parameters:    long lPumpNumber, long lSyncNumber, long lFlags, BSTR *pRec, long *pVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			12/08/2008   14:00		Start		4.0.20.501 - TD 65908
******************************************************************************/
STDMETHODIMP CGPI::FinalizePAKTrs(long lPumpNumber, long lFlags, long lSyncNumber, BSTR sRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::FinalizePAKTrs   start");
#ifdef CATCH_GPI
	try
	{
#endif
		FinalizePAKTrs__(lPumpNumber, lFlags, lSyncNumber, sRec, pVal);

#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "FinalizePAKTrs()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

/******************************************************************************
Description:	Finalize PAK transaction by its trs number.
Find the transaction and move move its state to PAID.
Create an OLA record to save the transaction as PAP.

Returns:      	GPI ret code
Parameters:    long lPumpNumber, long lSyncNumber, long lFlags, BSTR *pRec, long *pVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			12/08/2008   13:00		Start		4.0.20.501 - TD 65908
******************************************************************************/
void CGPI::FinalizePAKTrs__(long lPumpNumber, long lFlags, long lSyncNumber, BSTR sRec, long *pVal)
{
	long lRetVal = GPI_OK;
	int iLen = 0;
	DWORD dwLastErr = 0;
	CPumpTransact cPumpTrs;
	CARD_SALE_ALL3 cardDataAll3;

	memset(&cardDataAll3, 0, sizeof(CARD_SALE_ALL3));

	if (PUMP_NOT_VALID(lPumpNumber))
		lRetVal = GPI_INVALID_PUMP_NUMBER;

	if (lRetVal == GPI_OK)
	{
		iLen = Bstr2Buffer(sRec, (void *)&cardDataAll3, sizeof(CARD_SALE_ALL3));

		if (iLen == 0)
		{
			dwLastErr = GetLastError();
			lRetVal = GPI_LAST_ERROR;
		}

		if (lRetVal == GPI_OK)
		{
			//Find the PAK trs according to sync number
			int iRetVal = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsByNumber(lSyncNumber, &cPumpTrs);

			if (!iRetVal)
			{
				//Copy the trs date from the trs into CardSaleData3
				CString sTmpDateAndTime;
				CTime  sTagTime = cPumpTrs.GetTime();

				sTmpDateAndTime.Format("%02d%02d%02d%02d%02d%02d",
					((sTagTime.GetYear() >= 2000) ? (sTagTime.GetYear() - 2000) : (sTagTime.GetYear() - 1900)),
					sTagTime.GetMonth(),
					sTagTime.GetDay(),
					sTagTime.GetHour(),
					sTagTime.GetMinute(),
					sTagTime.GetSecond());


				memcpy(cardDataAll3.CardSaleAll.data.sDateTime, sTmpDateAndTime.GetBuffer(), MIN(sizeof cardDataAll3.CardSaleAll.data.sDateTime, sTmpDateAndTime.GetLength()));
				memcpy(cardDataAll3.CardSaleAll.extData.sDateTimeExt, sTmpDateAndTime.GetBuffer() + sizeof cardDataAll3.CardSaleAll.data.sDateTime, MIN(sizeof(cardDataAll3.CardSaleAll.extData.sDateTimeExt), sTmpDateAndTime.GetLength()));
			}
			else
				lRetVal = GPI_TRS_NOT_FOUND;

		}

		if ((lRetVal == GPI_OK) && (!cPumpTrs.GetOnOffer()))
		{
			lRetVal = cPumpTrs.SetStateOnOffer(m_lPosNumber);
		}

		if (lRetVal == GPI_OK)
		{
			// Change trs state to Accepted
			lRetVal = cPumpTrs.SetStateAccepted(m_lPosNumber);
		}

		if (lRetVal == GPI_OK)
		{
			//Creates OLA record form TAG_PUMP_TRANSACT5 & sRec
			lRetVal = CreateAndSaveOLARec(cPumpTrs, cardDataAll3); //4.0.23.509 97360 only changing lines
		}

		//4.0.28.500 - TD 240006/251326
		if ((lRetVal == GPI_OK) && (lFlags & FINALIZE_PREPAY_CREATE_RECEIPT_REC))
		{
			CreateAndSaveReceiptRec(cPumpTrs, cardDataAll3);
		}

		if (lRetVal == GPI_OK)
		{
			// Change trs state to PAID
			lRetVal = cPumpTrs.SetStatePaid(m_lPosNumber);
		}

		if (lRetVal == GPI_OK)	//4.0.19.710 66705 //4.0.23.504 66705
		{
			if (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag() && _Module.m_server.GetIFSFTerminalSrvInUse()) //4.0.19.710 66705
			{

				//Delete the zones:
				PS::RECEIPT_EXTRA_INFO_REC  cReceiptExtraRec;
				memset(&cReceiptExtraRec, ' ', sizeof(PS::RECEIPT_EXTRA_INFO_REC));
				cReceiptExtraRec.info.lIndexNumber = cPumpTrs.m_lLinkNumber;

				long lQdxRetCode = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_DELETE, cReceiptExtraRec);
				//long lQdxRetCode = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_DELETE,&cReceiptExtraRec); //4.0.25.30 - TD 119691

				CString sMsg; //4.0.19.800 66705

				if (lQdxRetCode)
				{
					sMsg.Format("FinalizePAKTrs__, failed to clear Index=%ld from ReceipteExtraData.qdx , error=%ld", cReceiptExtraRec.info.lIndexNumber, lRetVal);
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg, LOG_LEVEL_1); //4.0.19.800 66705
				}
				else
				{
					sMsg.Format("FinalizePAKTrs__ successfully  cleared Index=%ld from ReceipteExtraData.qdx", cReceiptExtraRec.info.lIndexNumber);
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg); //4.0.19.800 66705
				}

			}
		}
	}

	if (lRetVal)
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"FinalizePAKTrs",
				TRUE, lRetVal,
				FALSE, 0);
	}


	if (pVal != NULL)
		*pVal = lRetVal;

}

/******************************************************************************
Description:	Method replaces 3 GPI calls from POSPump - will be called from POSPump 1023 and on

Returns:      	third word (1 bytes) of pVal is retCode of GetMinimalPumpsInfo3__()  (0x0F00)
second word(1 bytes) of pVal is retCode of GetPAKQuery2__()			 (0x00F0)
low word   (1 bytes) of pVal is retCode of GetPumpInfo3__()			 (0x000F)

Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Gena			14/01/2009   15:00		Start		4.0.22.500 - TD 69173
AmitH			01/02/2009   14:10		Cont. Add third argument and flag for GetPumpInfo3		4.0.22.500 - TD 69173
******************************************************************************/
STDMETHODIMP CGPI::GetPumpsTrsInfo(long lPumpNumber, BSTR *pMinimalPumpsInfo, BSTR *pAllTrsInfo, BSTR *pGetPumpsInfo3, long * pTrsCount, long lGetMinimalPumpsFlags, long lGetTrsFlags, long lGetPumpInfo3Flags, long lParam, long *pVal)
{
#ifdef DEBUG
	if (LOG_ALL_FLOW_CONTROL)
		LOG_EXTERNALCALL("CGPI::GetPumpsTrsInfo   start");
	LogFlags(lGetTrsFlags, lGetMinimalPumpsFlags, lGetPumpInfo3Flags, lParam);
#endif
	BYTE byMininalPumpsInfoRetVal = GPI_OK;
	BYTE byAllTrsInfoRetVal = GPI_OK;
	BYTE byPumpInfo3RetVal = GPI_OK;
	long lFinalRetVal = GPI_OK;

	USES_CONVERSION;

	if (pMinimalPumpsInfo == NULL || pAllTrsInfo == NULL || pGetPumpsInfo3 == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg2(
				"CGPI", "GetPumpsTrsInfo", m_lPosNumber, LOG_POS, "GetPumpsTrsInfo, Throw COM Exception : E_POINTER", LOG_LEVEL_0);

		return E_POINTER;
	}
#ifdef CATCH_GPI
	__try
	{
#endif
		GetMinimalPumpsInfo3__(pMinimalPumpsInfo, lGetMinimalPumpsFlags, pVal);

		byMininalPumpsInfoRetVal = BYTE(*pVal);
		*pVal = 0;

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetPumpsTrsInfo__"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetMinimalPumpsInfo3__()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

#ifdef CATCH_GPI
	__try
	{
#endif
		GetPAKQuery2__(pAllTrsInfo, pTrsCount, lGetTrsFlags, pVal);

		byAllTrsInfoRetVal = BYTE(*pVal);
		*pVal = 0;

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetPumpsTrsInfo__"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPAKQuery2__()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
#ifdef CATCH_GPI
	__try
	{
#endif
		if (0 != lGetPumpInfo3Flags)													//4.0.22.506 G&S
		{
			GetPumpInfo3__(lPumpNumber, pGetPumpsInfo3, lGetPumpInfo3Flags, pVal);
			byPumpInfo3RetVal = BYTE(*pVal);
		}

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::GetPumpsTrsInfo__"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPumpsTrsInfo__()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}

	lFinalRetVal = byMininalPumpsInfoRetVal;
	lFinalRetVal <<= 8;

	lFinalRetVal |= byAllTrsInfoRetVal;
	lFinalRetVal <<= 8;

	lFinalRetVal |= byPumpInfo3RetVal;

	*pVal = lFinalRetVal;

#endif 
	return S_OK;
}


/******************************************************************************
Description:	POSPump has a polling mechanism which checks whether the report is ready.
Used for the asynchronous report generation process.
Once the report is ready, pbIsReady will become TRUE
In case there was a failure lpResultCode will contain the error code
Report text will be in pbstrReportBuffer

Returns:      	GPI return code
Parameters:    long lType, long lFlags, BOOL *pbIsReady, BSTR *pbstrReportBuffer, long *lpResultCode, long *pVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			08/02/2009   17:00		Start		//4.0.22.501 - TD 69905
******************************************************************************/
STDMETHODIMP CGPI::RequestReportReady(/*[in]*/long lType, /*[in]*/long lFlags, /*[out]*/BOOL *pbIsReady, /*[out]*/BSTR *pbstrReportBuffer, /*[out]*/long *lpResultCode, /*[out,retval]*/long *pVal)
{
	LOG_EXTERNALCALL("CGPI::RequestReportReady   start");
#ifdef CATCH_GPI
	try
	{
#endif
		RequestReportReady__(lType, lFlags, pbIsReady, pbstrReportBuffer, lpResultCode, pVal);

#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "RequestReportReady()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


/******************************************************************************
Description:	POSPump has a polling mechanism which checks whether the report is ready.
Used for the asynchronous report generation process.
Once the report is ready, pbIsReady will become TRUE
In case there was a failure lpResultCode will contain the error code
Report text will be in pbstrReportBuffer

Returns:      	GPI return code
Parameters:    long lType, long lFlags, BOOL *pbIsReady, BSTR *pbstrReportBuffer, long *lpResultCode, long *pVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			08/02/2009   17:00		Start		//4.0.22.501 - TD 69905
******************************************************************************/
void CGPI::RequestReportReady__(/*[in]*/long lType, /*[in]*/long lFlags, /*[out]*/BOOL *pbIsReady, /*[out]*/BSTR *pbstrReportBuffer, /*[out]*/long *lpResultCode, /*[out,retval]*/long *pVal)
{
	long lRetVal = GPI_OK;

	if (!pbIsReady || !pbstrReportBuffer || !lpResultCode)
		lRetVal = GPI_NOT_SUPPORTED;

	if (!lRetVal)
	{
		if (lType == GET_REPORT_WETSTOCK_ASYNC_TYPE)
		{
			if (_Module.m_server.m_cServerInfo.GetReportRequestState() == REPORT_READY)
			{
				long lErrCode = _Module.m_server.m_cServerInfo.GetReportRequestFailureResult();

				*pbIsReady = TRUE;
				*lpResultCode = lErrCode;

				if (!lErrCode)
				{
					*pbstrReportBuffer = (_Module.m_server.m_cServerInfo.GetReportBuffer()).copy();
				}
			}
		}
	}

	if (pVal != NULL)
		*pVal = lRetVal;
}


/******************************************************************************
Description:	SetTrsState3 - Extend SetTrsState
Returns:   	None
Parameters:    long lPumpNumber
long lTrsNumber
long lState
long lFlags
BSTR sData

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			12/02/2009   10:30		Start	4.0.22.501 - TD 69904
******************************************************************************/
STDMETHODIMP CGPI::SetTrsState3(long lPumpNumber, long lTrsNumber, long lState, long lFlags, BSTR sData, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetTrsState3   start");

#ifdef CATCH_GPI
	__try
	{
#endif

		SetTrsState__(lPumpNumber, lTrsNumber, lState, pVal, lFlags, sData);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::SetTrsState3"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetTrsState3()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 



	return S_OK;
}

//4.0.23.500 72717
STDMETHODIMP CGPI::SetANPRCommand(long lCommand, long lPumpNumber, long lFlags1, long lFlags2, BSTR pRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetANPRCommand   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		SetANPRCommand__(lCommand, lPumpNumber, lFlags1, lFlags2, pRec, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::SetANPRCommand"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetANPRCommand()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

void CGPI::SetANPRCommand__(long lCommand, long lPumpNumber, long lFlags1, long lFlags2, BSTR pRec, long *pVal) //4.0.23.500 72717 76438
{
	long lRetVal = GPI_OK;
	switch (lCommand)
	{
	case ANPR_UNBLOCK_PUMP:
		//==============================================================
	{
		_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, lPumpNumber, GCI_EVENT_ANPR_UNBLOCK_PUMP);
	}
	break;
	case ANPR_OVERRIDE_PUMP_BLOCK:
		//==============================================================
	{
		_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, lPumpNumber, GCI_EVENT_ANPR_OVERRIDE_PUMP_BLOCK);
	}
	break;

	}

	if (pVal != NULL)
		*pVal = lRetVal;
}

STDMETHODIMP CGPI::GetServerStateEx(long *pServerStateEx, long lFlag, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetServerStateEx   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		GetServerStateEx__(pServerStateEx, lFlag, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("CGPI::GetServerStateEx"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetServerStateEx()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


//4.0.23.508 TD 89047
void CGPI::GetServerStateEx__(long *pServerStateEx, long lFlag, long *pVal)
{
	if (pVal != NULL)
	{
		*pVal = _Module.m_server.GetServerState();
		*pServerStateEx = _Module.m_server.GetServerStateEx(lFlag);
	}
}

//4.0.24.100 118124
STDMETHODIMP CGPI::QueryExtraInfoData2(long lPumpNumber, long *lIndexNumber, short nFlags, BSTR *pRec, BSTR *pExtraRec, long *pVal)
{
	if (LOG_ALL_FLOW_CONTROL)
		LOG_EXTERNALCALL("CGPI::QueryExtraInfoData2   start");
#ifdef CATCH_GPI
	__try
	{
#endif
		QueryExtraInfoData2__(lPumpNumber, lIndexNumber, nFlags, pRec, pExtraRec, pVal);
#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::QueryExtraInfoData")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "QueryExtraInfoData()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

//4.0.24.100 118124
void CGPI::QueryExtraInfoData2__(long lPumpNumber, long *lIndexNumber, short nFlags, BSTR *pRec, BSTR *pExtraRec, long *pVal)
{
	QueryExtraInfoData__(lPumpNumber, lIndexNumber, nFlags, pRec, pVal);

	if (*pVal == GPI_OK)
	{
		const int GPI_LOYALTY_CORRECTION = 99;	//4.0.22.510 76232 due to resources update this retcode is different between 1020 and 1023, so a correction is needed to keep the same value.
		ASH2000_REC			sAsh2000Rec;
		long lQdxRetVal = 0;
		memset(&sAsh2000Rec, ' ', sizeof(sAsh2000Rec));
		sAsh2000Rec.lIndexNumber = *lIndexNumber;

		if (lQdxRetVal = _Module.m_server.IndexOperation(ASH2000_FILE, REC_READ, &sAsh2000Rec))//4.0.3.49
		{
			if (LOG_BASIC_FLOW_CONTROL)
			{
				DWORD dwThreadId = GetCurrentThreadId();
				CString tmpStr;
				tmpStr.Format("QueryExtraInfoData2__, Can not find Additional record %ld from file Ash 2000 info, retCode = %ld", *lIndexNumber, lQdxRetVal);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, tmpStr, LOG_LEVEL_1);
				//*pVal = GPIRetCode(GPI_NO_LOYALTY_SUMMARY + GPI_LOYALTY_CORRECTION); 
			}
		}
		else
		{
			long lRecSize = sizeof(sAsh2000Rec.sData);
			BYTE *sTmp = new BYTE[lRecSize + 2];
			Str2StrNull((BYTE *)&sAsh2000Rec.sData, lRecSize, sTmp);

			CComBSTR strResult(lRecSize + 2, (LPCSTR)sTmp);
			*pExtraRec = strResult.Copy();

			if (sTmp != NULL)										//4.0.5.26
				delete[]sTmp;

			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString tmpStr;
				tmpStr.Format("QueryExtraInfoData2__, Found ash 2000 record trs=%ld", *lIndexNumber);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, tmpStr);
			}
		}

	}


}

/******************************************************************************
Description:	GetParam4 - similar to GetParam2 with extra field - lFlags

Returns:      	GPI ret code
Parameters:    BSTR *sTableFieldAndVal
long lKey1
long lKey2
short nFiledType
long lFlags
long *lVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			04/05/2010   15:00		Start		4.0.26.501 - TD 116227
******************************************************************************/
STDMETHODIMP CGPI::GetParam4(BSTR *sTableFieldAndVal, long lKey1, long lKey2, short nFiledType, long lFlags, long *lVal, long *pVal)
{
#ifdef _DEBUG		
	if (LOG_ALL_FLOW_CONTROL)
		LOG_EXTERNALCALL("CGPI::GetParam4   start");
#endif
#ifdef CATCH_GPI
	try
	{
#endif

		GetParam4__(sTableFieldAndVal, lKey1, lKey2, nFiledType, lFlags, lVal, pVal);

#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetParam4()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


/******************************************************************************
Description:	GetParam4 - similar to GetParam2 with extra field - lFlags

Returns:      	GPI ret code
Parameters:    BSTR *sTableFieldAndVal
long lKey1
long lKey2
short nFiledType
long lFlags
long *lVal

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella			04/05/2010   15:00		Start		4.0.26.501 - TD 116227
******************************************************************************/
void CGPI::GetParam4__(BSTR *sTableFieldAndVal, long lKey1, long lKey2, short nFiledType, long lFlags, long *lVal, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;

	long lGradeId = 0;

	if (lFlags & GET_PARAM_USE_EXTENDED_GRADE_ID)
	{
		// Need to convert extended grade id to grade id
		_Module.m_server.m_cExtendedGradeMap.GetGradeByExtGrade(lKey1, lGradeId);

		if (!lGradeId)
		{
			SysFreeString(*sTableFieldAndVal);				//TD 224805
			*sTableFieldAndVal = SysAllocString(L" ");

			retCode = CTRL_INVALID_GRADE_NUMBER;
		}

	}
	else
		lGradeId = lKey1;

	if (!retCode)
		GetParam2__(sTableFieldAndVal, lGradeId, lKey2, nFiledType, lVal, pVal);


	if (pVal)
		*pVal = retCode;
}

//4.0.26.501 144221
STDMETHODIMP CGPI::ValidateWashCode(long lDevice, long *lSyncNumber, long lWashCode, long lWashProgramID, long lFlags, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::ValidateWashCode   start");
#ifdef CATCH_GPI
	__try
	{
#endif

		ValidateWashCode__(lDevice, lSyncNumber, lWashCode, lWashProgramID, lFlags, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GPI::ValidateWashCode")) //4.0.15.450
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "ValidateWashCode", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

//4.0.26.501 144221
void CGPI::ValidateWashCode__(long lDevice, long *lSyncNumber, long lWashCode, long lWashProgramID, long lFlags, long *pVal)
{
	GPIRetCode retCode = GPI_OK;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);

		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}

	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());

		else
			retCode = GPI_SERVER_NOT_READY;
	}

	else if (!_Module.m_server.m_cCarWash.CarWashEnabled())
		retCode = GPI_CAR_WASH_NOT_ACTIVE;
	else if ((!_Module.m_server.m_cCarWash.CarWashSessionEmpty()) || (!_Module.m_server.m_cCarWashGeneralRequest.CarWashSessionEmpty()))
		retCode = GPI_CAR_WASH_IN_MIDDLE_OF_ANOTHER_SESSION;


	if (retCode == GPI_OK)
	{
		_Module.m_server.m_cCarWash.ValidateWashCode(lDevice, lSyncNumber, lWashCode, lWashProgramID, lFlags, m_lPosNumber, (long *)&retCode);

	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"ValidateWashCode",
				TRUE, retCode,
				FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

/******************************************************************************
Description: This method creates a Receipt record from Trs data and CardSale data


Returns:     Zero on success.

Parameters:  CPumpTransact cPumpTrs - Transaction data
CARD_SALE_ALL3 cardDataAll3 - Card data

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Ella		21/07/2011   10:00			Start		4.0.28.500 - TD 240006/251326
******************************************************************************/
GPIRetCode CGPI::CreateAndSaveReceiptRec(CPumpTransact cPumpTrs, CARD_SALE_ALL3 cardDataAll3)
{
	GPIRetCode retCode = GPI_OK;
	PAY_AT_PUMP_INFO	cPayAtPumpInfo;
	memset(&cPayAtPumpInfo, 0, sizeof(PAY_AT_PUMP_INFO));
	CReceiptInfo ReceiptInfo;


	if (cPumpTrs.m_lNumber > 0)
	{
		memcpy(&cPayAtPumpInfo.CardSaleInfo.cardData, &cardDataAll3.CardSaleAll.data, sizeof(cardDataAll3.CardSaleAll.data));
		memcpy(&cPayAtPumpInfo.CardSaleInfo.extraData, &cardDataAll3.CardSaleAll.extData, sizeof(cardDataAll3.CardSaleAll.extData));
		memcpy(&cPayAtPumpInfo.CardSaleExtraData2, &cardDataAll3.extData2, sizeof(cardDataAll3.extData2));
		memcpy(&cPayAtPumpInfo.CardSaleExtraData3, &cardDataAll3.extData3, sizeof(cardDataAll3.extData3));
		memcpy(&cPayAtPumpInfo.CardSaleExtraData4, &cardDataAll3.extData4, sizeof(cardDataAll3.extData4));
		memcpy(&cPayAtPumpInfo.CardSaleExtraData5, &cardDataAll3.extData5, sizeof(cardDataAll3.extData5));
		memcpy(&cPayAtPumpInfo.CardSaleExtraData6, &cardDataAll3.extData6, sizeof(cardDataAll3.extData6));

		_Module.m_server.m_cBaseReceipt->ConvertOlaToReceipt(&cPumpTrs, &cPayAtPumpInfo, ReceiptInfo);//4.0.9.503
		long lRtc = CSQLReceiptDB::GetInstance().UpdateReceipt(ReceiptInfo);


		if (lRtc != OK)
			retCode = GPI_FAILED_IO;
	}
	else
		retCode = GPI_TRS_NOT_FOUND;

	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString sLogMsg;
		sLogMsg.Format("CreateAndSaveReceiptRec, TrsNumber = %ld, RetCode %d", cPumpTrs.m_lNumber, retCode);
		_LOGMSG.LogMsg(cPumpTrs.m_lPump, LOG_PUMP, sLogMsg);
	}

	return retCode;
}

/******************************************************************************
Description:	GetMiddleOfProcId() - Returns the active process id when the
system is in the middle of process.
Returns:      	long - process id
Parameters:    None

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Omer			26/07/2011   15:00		Start		4.0.28.501 -  TD 254219
******************************************************************************/
long CGPI::GetMiddleOfProcId() const
{

	SERVER_INFO sInfo;
	_Module.m_server.m_cServerInfo.GetInfo(sInfo);
	long lTankNumber;
	long lProcId = 0;

	// Tank Reading and delivery emulation take place simultaneously with wetstock process.
	// Because of this the order of the if else should not be changed.

	if (_Module.m_server.m_cCommonFunction.IsDeliveryEmulationSingleTankInProcess(lTankNumber))
		lProcId = MIDDLE_OF_DELIVERY_EMULATION_SINGLE_TANK_PROCESS;
	else if (sInfo.byDeliveryEmulationStatus)
		lProcId = MIDDLE_OF_DELIVERY_EMULATION_PROCESS;
	else if (sInfo.lWetStockFlags)
		lProcId = MIDDLE_OF_WETSTCOK_PROCESS;
	else if (sInfo.bRequiredShiftProcess)
		lProcId = MIDDLE_OF_SHIFT_CHANGE_PROCESS;
	else if (sInfo.bRequiredPriceChange)
		lProcId = MIDDLE_OF_PRICE_CHNAGE_PROCESS;
	else if (sInfo.bRequiredModeProcess)
		lProcId = MIDDLE_OF_CHNAGE_MODE_PROCESS;
	else if (sInfo.byRequiredMaintenanceProcess)
		lProcId = MIDDLE_OF_QDX_MAINT_PROCESS;
	else if (sInfo.byMemoInProcess)
		lProcId = MIDDLE_OF_WETSTOCK_BY_MEMO_PROCESS;
	else if (sInfo.bInPriceChangeProcess)
		lProcId = MIDDLE_OF_PRICE_CHANGE_WITH_ROLLBACK_PROCESS;
	else if (_Module.m_server.m_cServerInfo.GetChangeProductInTankState() != CPIT_IDLE)
		lProcId = MIDDLE_OF_DELIVERY_CHANGE_PRODUCT_IN_TANK_PROCESS;

	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString	str;
		str.Format("CGPI::GetMiddleOfProcId() ProcessId = %d", lProcId);
		_LOGMSG.LogMsg(str);
	}

	return lProcId;

} // TD 254219  4.0.28.501

  /******************************************************************************
  Description:	SendForceRemoveAlarm - Send an alarm on force remove
  The only exception is when the trs status is WAIT_PROGRESS (authorized)

  Returns:   	 None
  Parameters:    long lState        - The trs new PostPayState
  CPumpTransact cTrs - The trs Status

  WHO			WHEN                    WHAT
  -------------------------------------------------------------------------------
  Omer			28/07/2011   10:30		Start
  ******************************************************************************/
void CGPI::SendForceRemoveAlarm(long lState, CPumpTransact cTrs) const
{

	if (lState == STATE_FORCE_REMOVE && cTrs.GetStatus() != WAIT_PROGRESS)
		_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, ALARM_TRS_FORCE_REMOVE, 0, ALARM_STARTED, cTrs.m_lNumber, 0);

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("Force Remove - Alarm Sent! : Trs %d, Status %d", cTrs.m_lNumber, cTrs.GetStatus());
		_LOGMSG.LogMsg(str);
	}

} // TD 236009 4.0.25.220 // TD 254948/254959 4.0.25.290


  /******************************************************************************
  Description:	Set pump extended misc
  Returns:      	void
  Parameters:

  WHO			WHEN                    WHAT
  -------------------------------------------------------------------------------
  Ella			31/07/2011   12:00		Start		4.0.28.502 - 251326
  ******************************************************************************/
STDMETHODIMP CGPI::SetPumpMiscExt(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SetPumpMiscExt   start");
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	try
	{
#endif

		SetPumpMiscExt__(lPumpNumber, lMisc, bRemoveMisc, pVal);

#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SetPumpMiscExt()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

//4.0.28.502 - 251326
void  CGPI::SetPumpMiscExt__(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal)
{
	bool bMiscExt = TRUE;
	_Module.m_server.m_cCommonFunction.SetPumpMisc(lPumpNumber, lMisc, bRemoveMisc, pVal, bMiscExt);
}

long CGPI::HandleLoyaltyPrepPay(long lPumpNumber, CPumpTransact & trs, BSTR sLoyaltyInfo, TAG_DISCOUNT_LOYALTY_INFO4 & cLoyaltyInfo, PRE_PAY_INFO & info)
{
	long lRtc = OK;

	_Module.m_server.CheckVlidityTransactionNumber(trs.m_lNumber);  //4.0.15.504

	_Module.m_server.ConvertWideCharLoyaltyInfo2RegularInfo(lPumpNumber, &sLoyaltyInfo, &cLoyaltyInfo);

	_Module.m_server.SetAutoReconcileTimeout(lPumpNumber, &cLoyaltyInfo, &trs); // TD 233242 4.0.23.1050

	if (info.sPreSetLimitType == LIMIT_BY_VOLUME)	//4.0.20.450 66977 //4.0.21.50
		SetLimit(lPumpNumber, cLoyaltyInfo, info);

	lRtc = _Module.m_server.TrsLinkOperation(REC_INSERT, &trs, &info, &cLoyaltyInfo);

	return lRtc;

} // NO TD 4.0.23.1050

  /******************************************************************************
  Description:	SetLimit - Called from HandleLoyaltyPrepPay (Refactor)
  Returns:   	None

  WHO			WHEN                    WHAT
  -------------------------------------------------------------------------------
  Omer			05/09/2011				NO TO
  ******************************************************************************/
void CGPI::SetLimit(long lPumpNumber, TAG_DISCOUNT_LOYALTY_INFO4 & cLoyaltyInfo, PRE_PAY_INFO & info)
{
	//take the lowest between OLA limit and Loyalty limit
	long lLoyaltyLimit = a2l(cLoyaltyInfo.cDiscountLoyaltyInfo3.sLoyaltyVolumeLimit, sizeof(cLoyaltyInfo.cDiscountLoyaltyInfo3.sLoyaltyVolumeLimit));//4.0.23.270 111746
	if (lLoyaltyLimit)
	{
		if (info.lPreSetLimit > lLoyaltyLimit)
		{
			info.lPreSetLimit = lLoyaltyLimit;
			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Preset volume limit is taken from loyalty volume limit %ld", lLoyaltyLimit);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
			}

		}
		else	//loyalty volume limit is higher than ola volume limit so we can ignore it
		{
			lLoyaltyLimit = 0;
			l2a(lLoyaltyLimit, cLoyaltyInfo.cDiscountLoyaltyInfo3.sLoyaltyVolumeLimit, sizeof(cLoyaltyInfo.cDiscountLoyaltyInfo3.sLoyaltyVolumeLimit));//4.0.23.270 111746
			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Loyalty volume limit %ld is higher than preset volume limit %ld. Loyalty limit will be ignored", lLoyaltyLimit, info.lPreSetLimit);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
			}
		}
	}
} // NO TD 4.0.23.1050

  /******************************************************************************
  Description:	Save Tender Type to given trs
  The only exception is when the trs status is WAIT_PROGRESS (authorized)

  Returns:   	 None
  Parameters:    BSTR bstrTenderType - received tender type from PosPump
  CPumpTransact & trs - save the tender type to the trs

  WHO			WHEN                    WHAT
  -------------------------------------------------------------------------------
  Omer			29/03/2012   10:30		// 4.0.25.600   CR 296433
  ******************************************************************************/
void CGPI::SaveTenderType(CPumpTransact & trs, BSTR bstrTenderType)
{

	TAG_TENDER_INFO cTagTenderInfo;
	memset(&cTagTenderInfo, 0, sizeof(cTagTenderInfo));
	int iLen = Bstr2Buffer(bstrTenderType, &cTagTenderInfo, sizeof(cTagTenderInfo));
	memcpy(trs.m_sTenderType, cTagTenderInfo.sTenderType, sizeof(trs.m_sTenderType));

	char sTenderType[3] = { 0 };
	memcpy(sTenderType, trs.m_sTenderType, sizeof(trs.m_sTenderType));
	CString str;
	str.Format("SaveTenderType : sTenderType = %s", sTenderType);
	_LOGMSG.LogMsg(str);
}

//4.0.31.0 - TD TD 326234
void CGPI::SavePumpMode(long lPumpNumber, long lMode)
{
	CString str;
	str.Format("Pumps\\Pump%02ld", lPumpNumber);

	//setting to a specific pump its new manual mode.
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "ManualMode", lMode, FALSE, _HOST.m_lGroupNum, 0, lPumpNumber);
	_Module.m_server.m_lPumpsManualModeArray[lPumpNumber - 1] = lMode;

	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	str.Format("%s:%d, EnterCrtitcal", __FUNCTION__, __LINE__);
	_LOGMSG.LogMsg(str);

	CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();
	long lMisc = pumpStat.GetMisc();

	if (lMode)
		lMisc |= MISC_MANUAL_MODE;
	else
		lMisc &= MISC_NOT_MANUAL_MODE;

	pumpStat.SetMisc(lMisc);
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetStat(pumpStat);

	_Module.m_server.SavePumpInfo(lPumpNumber);

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("SetPumpMode, Set Mode %d to Pump %02d", lMode, lPumpNumber);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
	}
}


STDMETHODIMP CGPI::AuthorizeAndLimit4(long lPumpNumber, short nLevel, long lGradePrice, long lMaxValue, long lMaxVolume, long lFlags, long *lSyncNumber, BSTR sRec, __int64 lPurchaseID, long *pVal) // cinooc
{
	LOG_EXTERNALCALL("CGPI::AuthorizeAndLimit4   start");
#ifdef CATCH_GPI
	try
	{
#endif
		AuthorizeAndLimit2__(lPumpNumber, nLevel, lGradePrice, lMaxValue, lMaxVolume, lFlags, lSyncNumber, sRec, pVal, lPurchaseID);
#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "AuthorizeAndLimit4()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 
	return S_OK;
}

STDMETHODIMP CGPI::GetPrePaidTrs3(long lPumpNumber, __int64 lSyncNumber, short nFlags, BSTR *pRec, long *pVal) // cinooc
{
#ifdef _DEBUG		
	if (LOG_ALL_FLOW_CONTROL)
		LOG_EXTERNALCALL("CGPI::GetPrePaidTrs3   start");
#endif

	USES_CONVERSION;

	if (pRec == NULL)
	{
		if (LOG_ERROR)
			_LOGMSG.LogMsg(
				m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "GetPrePaidTrs3, Throw COM Exception : E_POINTER", LOG_LEVEL_0);

		return E_POINTER;
	}

#ifdef CATCH_GPI
	try
	{
#endif
		GetPrePaidTrs2__(lPumpNumber, lSyncNumber, nFlags, pRec, pVal);
#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetPrePaidTrs2()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


STDMETHODIMP CGPI::FinalizePrePayTrs2(long lPumpNumber, long lFlags, long lFlags2, long lSyncNumber, BSTR sRec, __int64 lPurchaseID, long *pVal) // cinooc
{
	("LOG_EXTERNALCALLCGPI::FinalizePrePayTrs2   start");

#ifdef CATCH_GPI
	try
	{
#endif
		FinalizePrePayTrs__(lPumpNumber, lFlags, lFlags2, lSyncNumber, sRec, lPurchaseID, pVal);
#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "FinalizePrePayTrs2()", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}

// TD 378454
long CGPI::MovePrePay(long lPumpNumber, CPumpTransact &trs, PRE_PAY_INFO &info, long * lSyncNumber, GPIRetCode &retCode)
{
	long lRtc = OK;
	CString sLogMsg;

	if (LOG_BASIC_FLOW_CONTROL)
	{
		sLogMsg.Format("AuthorizeAndLimit, Move PrePay from Pump: (%ld)  to pump: (%ld) ", info.lPumpNumber, lPumpNumber);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
	}

	if (info.lPumpNumber > 0) //4.0.19.504 - TD 60613
	{
		EXTRA_INFO_REC	ExtraInfoRec, newExtraInfoRec;
		ExtraInfoRec.ClearRecord(); // RFUEL-3708
		newExtraInfoRec.ClearRecord(); // RFUEL-3708

		// If changes were made to the discounts array
		ExtraInfoRec.info.lIndexNumber = trs.m_lNumber;
		lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);
		if (lRtc == OK)
		{
			if (_Module.m_server.MovePrepayGradesDiscounts(info.lPumpNumber, lPumpNumber, &ExtraInfoRec, &newExtraInfoRec))	// 50269
			{

				_Module.m_server.FillInPromotionsArrayByPumpNum(lPumpNumber);  // TD 378454

				lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_UPDATE, &newExtraInfoRec);

				if (lRtc)
				{
					if (LOG_DETAIL_FLOW_CONTROL)
					{
						sLogMsg.Format("MovePrepay Fail to update Extra Info file ,  Trs =%ld", trs.m_lNumber);
						_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sLogMsg);
					}

				}
			}


			sLogMsg.Format("AuthorizeAndLimit, readjustment to grades discounts pump = %ld", lPumpNumber);
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);

		}
		else
		{
			sLogMsg.Format("MovePrepay Fail to read from Extra Info file ,  Trs =%ld", trs.m_lNumber);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sLogMsg);
		}


		info.lSyncNumber = *lSyncNumber;
		info.lPumpNumber = lPumpNumber;
		trs.m_lGradePrice = info.lGradePrice;
	}
	else
		retCode = GPI_INVALID_PUMP_NUMBER; //4.0.19.504 - TD 60613

	return lRtc;
}


GPIRetCode CGPI::ValidateMovePrePay(long lPumpNumber, long lFlags, GPIRetCode retCode, CPumpTransact &trs, PRE_PAY_INFO &info, short nUnpaid)
{
	long p = lPumpNumber - 1;

	CString sLogMsg;

	// checking that the pump which get the Prepay as the correct Grade.	
	long lTmpMode = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetMode();
	short nPrePayWaiting = _Module.m_server.m_cPumpArray[p].GetNumberOfNonClosedPrePay();
	short nMaxPrePayAllowed = _Module.m_server.m_cPumpArray[p].GetMaxPrePayTrsAllowed();

	if (nMaxPrePayAllowed == 0)
	{
		if (nPrePayWaiting)
			retCode = GPI_PUMP_TRS_LIST_FULL;
	}

	if (_Module.m_server.m_cModeArray[lTmpMode - 1].PrePayNotValid())
	{
		if (!(lFlags & PREPAY_FORCE))
		{
			retCode = GPI_NOT_VALID_IN_CURRENT_MODE;
		}
	}
	//We were checking that the pump that the original PrePay
	//transaction came from was active, when we really wanted to
	//check the state of the target pump.  Hence the change from
	//info.lPumpNumber (original pump) to lPumpNumber (target pump)
	else if ((retCode == GPI_OK) && (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetRes() == PUMPSRV))
	{
		retCode = GPI_PUMP_NOT_IN_WORKING_STATE;
	}
	else if ((retCode == GPI_OK) && (trs.GetStatus() != WAIT_TIME_OUT && trs.GetStatus() != WAIT_RES))
	{
		retCode = GPI_TRS_ALREADY_IN_PROGRESS;
	}

	//If target pump has a TRS that is not dispensing, that trs may
	//not be fuelled.  So, if we move a prepay to this non-dispensing
	//pump, we will have the moved trs at WAIT_TIME_OUT and the 
	//original trs that times out after not being fuelled, leading
	//to two WAIT_TIME_OUT trs at the pump.
	else if ((retCode == GPI_OK) &&
		!IsTargetPumpReadyForMovedPrepay(lPumpNumber - 1))
	{
		retCode = GPI_PUMP_TRS_LIST_FULL;
	}
	else if ((retCode == GPI_OK) && (
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetServiceMode() !=
		_Module.m_server.m_cPumpArray[info.lPumpNumber - 1].GetStat().GetServiceMode()))
	{
		CString str;
		str.Format("AuthorizeAndLimit: PumpNumber %ld mode = %ld,PumpNumber %ld mode = %ld",
			lPumpNumber, _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetServiceMode(),
			info.lPumpNumber, _Module.m_server.m_cPumpArray[info.lPumpNumber - 1].GetStat().GetServiceMode());
		_LOGMSG.LogMsg(str);

		retCode = GPI_INVALID_SERVICE_MODE_NUMBER;
	}
	else if ((retCode == GPI_OK) && (info.lGradePrice))
	{
		if (info.lFlags & GRADE_PRICE_IS_POSITION_NUM)
		{
			short nTmpGrade = _Module.m_server.m_cPumpArray[info.lPumpNumber - 1].NozzleToGrade((short)info.lGradePrice);
			long nTmpPosition = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetGradePosition(nTmpGrade);


			if (LOG_BASIC_FLOW_CONTROL)
			{
				sLogMsg.Format("Flags %d : New Pump=[%ld], old Pump=[%ld] Old position=[%ld],  newposition=[%ld], Grade=[%d], retCode=[%d]", lFlags, lPumpNumber, info.lPumpNumber, info.lGradePrice, nTmpPosition, nTmpGrade, retCode); //50269
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
			}

			if (!nTmpPosition)
			{
				retCode = GPI_INVALID_GRADE_NUMBER;
			}
			else
			{
				info.lGradePrice = nTmpPosition;
				trs.m_lGradePrice = nTmpPosition;	//50269
			}
		}
	}
	//Verify that the source pump and the target pump are of the same pump type (diesel or not)
	if (retCode == GPI_OK) //- TD 22684 4.0.6190.1800
	{
		if (!(lFlags & PREPAY_FORCE))
		{
			BOOL bTargetIsDieselPump = FALSE;
			BOOL bSourceIsDieselPump = FALSE;
			bTargetIsDieselPump = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetDieselPumpFlags();
			bSourceIsDieselPump = _Module.m_server.m_cPumpArray[info.lPumpNumber - 1].GetDieselPumpFlags();
			if (bTargetIsDieselPump != bSourceIsDieselPump)
			{
				retCode = GPI_NOT_VALID_IN_CURRENT_MODE;
				sLogMsg.Format("CGPI::AuthorizeAndLimit() - attempted to move prepay to pump %d (%d) from pump %d(%d), but pumps are not of the same type (Diesel - Gas)", lPumpNumber, bTargetIsDieselPump, info.lPumpNumber, bSourceIsDieselPump);
				_LOGMSG.LogMsg(sLogMsg);
			}
		}
	}

	if (_Module.m_server.CheckVITConflict() && (retCode == GPI_OK))
	{
		CPumpTransact cTmptrs;

		if (!_Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&cTmptrs))
		{
			if ((cTmptrs.m_lRawVolume <= _Module.m_server.GetMaxVolumeForVITConflict()) &&
				(cTmptrs.GetLinkFlags() & TRS_LINK_PREPAY_NUMBER))

			{
				retCode = GPI_TRS_ALREADY_IN_PROGRESS;
			}
		}

		//Do not allow prepay to be moved because target
		//pump has a full trs list, so if we get a AVI conflict under the limit,
		//we will have too many unpaid PAKs
		if (nUnpaid >= MAX_ALLOWED_UNPAID_TRS(p))
		{
			retCode = GPI_PUMP_TRS_LIST_FULL;
			sLogMsg.Format("attempted to move prepay to pump %d from pump %d, but pump %d had a full trs list", lPumpNumber, info.lPumpNumber, lPumpNumber);
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
		}
	}

	if (retCode == GPI_OK)
	{
		CSingleLock slPosMap(&(_Module.m_server.m_csPosMap));
		slPosMap.Lock(INFINITE);
		CPosInfo tmpPos;

		if (_Module.m_server.m_cPosMap.Lookup(m_lPosNumber, tmpPos))
		{
			if (!tmpPos.IsPumpInFieldOfView(lPumpNumber))
			{
				retCode = GPI_PUMP_OUT_OF_VIEW;
			}
		}
		slPosMap.Unlock();
	}


	return retCode;
}


/******************************************************************************
Description:	AuthorizePumpCheckValidInput

Returns:      	void
Parameters:    long lPumpNumber

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
OmerT			02/01/2014   14:00		Start	  CR 370688
******************************************************************************/
void CGPI::AuthorizePumpCheckValidInput(IN long lPumpNumber, IN short nPrePayWaiting, IN short nUnpaid, OUT BOOL &bAuthAll, OUT GPIRetCode &retCode)
{
	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;
	else if (_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
	{
		retCode = GPI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
	}
	else if (SERVER_STATE_STARTUP)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(SERVER_STATE_STARTUP);
		else
			retCode = GPI_CONTROLLER_NOT_AVAILABLE;
	}
	else if (SERVER_NOT_IDLE)
	{
		if (_Module.m_server.ISPAKFinalizationFlagSet())
			retCode = GetAppropriateRetCode(_Module.m_server.GetServerState());
		else
			retCode = GPI_SERVER_NOT_READY;
	}
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
	{
		retCode = GPI_FAILED_IO;
	}
	else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
	{
		retCode = GPI_SERVER_IN_MIDDLE_LOCK_SHIFT;   //4.0.1.40
	}
	else if ((lPumpNumber > 0) && (nPrePayWaiting + nUnpaid >= (long)_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetMaxTrsNumber()))
	{
		retCode = GPI_PUMP_TRS_LIST_FULL;
	}
	else if (_Module.m_server.m_cServerInfo.GetInStopAllPumps())
	{
		retCode = GPI_PUMP_LOCKED;
	}
	else if (lPumpNumber) //
	{
		if (PUMP_NOT_VALID(lPumpNumber))
			retCode = GPI_INVALID_PUMP_NUMBER;
		//If the pump is locked we won't authorize the pump
		else if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetRes() == PUMPSRV) //4.0.8.503
		{
			retCode = GPI_PUMP_LOCKED;
		}
	}
	else
	{
		bAuthAll = TRUE;
	}
}


/******************************************************************************
Description:	AuthoirzeAndLimitPAPExternal

Returns:      	void
Parameters:    long lPumpNumber

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
OmerT			02/01/2014   14:00		Start	  CR 370688
******************************************************************************/
void CGPI::AuthoirzeAndLimitPAPExternal(IN long lPumpNumber, IN short nLevel, IN long lGradePrice, IN long lLimit, IN OUT long & lFlags)
{
	CString str;
	str.Format("CGPI::AuthoirzeAndLimitPAPExternal Called POS - %d: lFlags [%ld], lPumpNumber [%ld] , lGradePrice[%ld] , lLimit [%ld] , nLevel [%d]", m_lPosNumber, lFlags, lPumpNumber, lGradePrice, lLimit, nLevel);
	_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, str);

	long p = lPumpNumber - 1;

	CMD_STRUCT_AUTHORIZE_LIMIT cmdAuthorize;
	cmdAuthorize.nLevel = nLevel;
	cmdAuthorize.lGradePrice = lGradePrice;
	cmdAuthorize.lFlags = lFlags;

	CString sLimitType;

	if (lFlags & MULTIPLE_VOLUME_AND_AMOUNT_LIMIT)
	{
		PAY_AT_PUMP_INFO cPapInfo;
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cPapInfo);

		_Module.m_server.LoadGradeLimitArrayFromCardData(lPumpNumber, cmdAuthorize);

		cmdAuthorize.lMaxValue = 0;
		cmdAuthorize.lMaxVolume = 0;
	}
	else if (lLimit)
	{
		BOOL bPreSetLimitation = FALSE;

		if (lFlags & PAK_WITH_LIMIT)
		{
			if (_Module.m_server.LimitedPreSet())
				bPreSetLimitation = TRUE;
		}

		if (lFlags & LIMIT_BY_VALUE)
		{
			cmdAuthorize.lMaxVolume = 0;

			if (bPreSetLimitation)
			{
				cmdAuthorize.lMaxValue = _Module.m_server.GetLimit(PRICE_LEVEL_CASH, p + 1);

				if (cmdAuthorize.lMaxValue > lLimit)
					cmdAuthorize.lMaxValue = lLimit;
			}
			else
			{
				cmdAuthorize.lMaxValue = lLimit;
			}

			sLimitType = "LIMIT_BY_VALUE";
		}
		else if (lFlags & LIMIT_BY_VOLUME)
		{
			cmdAuthorize.lMaxValue = 0;
			if (bPreSetLimitation)
			{
				cmdAuthorize.lMaxVolume = _Module.m_server.GetStationLimitVolume();
				if (cmdAuthorize.lMaxVolume > lLimit)
					cmdAuthorize.lMaxVolume = lLimit;
			}
			else
				cmdAuthorize.lMaxVolume = lLimit;

			sLimitType = "LIMIT_BY_VOLUME";
		}
		else // LIMIT_FILL_UP
		{
			_Module.m_server.SetSiteLimit(p, nLevel, cmdAuthorize); // 4.0.22.850 TD 307758
			lFlags = LIMIT_FILL_UP;
			if (_Module.m_server.IsSiteLimitByVolume())
				lFlags |= LIMIT_BY_VOLUME;

			sLimitType = "LIMIT_FILL_UP";
		}

		str.Format("AuthorizePump2 Called by  POS - %d: limit type = %s, limit = [%ld]", m_lPosNumber, sLimitType, lLimit);
		_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, str);
	}

	_Module.m_server.SetCommand(CMD_AUTHORIZE_LIMIT, m_lPosNumber, p + 1, &cmdAuthorize, sizeof(CMD_STRUCT_AUTHORIZE_LIMIT));
}

void CGPI::AuthorizeAndLimitHandlePrePay(long p, long &lFlags, GPIRetCode &retCode, CString &sLogMsg, long lPumpNumber, short nUnpaid, CPumpTransact &trs, PRE_PAY_INFO &info, long * &lSyncNumber, BSTR bstrPrePay, long &lRtc)
{
	CString str;
	str.Format("CGPI::AuthorizeAndLimitHandlePrePay Called POS - %d: lFlags [%ld], lPumpNumber [%ld] , nUnpaid [%ld]", m_lPosNumber, lFlags, lPumpNumber, nUnpaid);
	_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, str);

	CPumpStatus stat;
	stat = _Module.m_server.m_cPumpArray[p].GetStat();

	IsAllowPrePay(p, retCode, lFlags, stat, sLogMsg, lPumpNumber, nUnpaid);

	if (retCode == GPI_OK) //4.0.17.20
	{
		// Creating a new Prepay transaction
		// ---------------------------------
		trs.SetOnOffer(m_lPosNumber);
		// 4.0.5.39
		info.nOwningPos = NO_POS_OWNER;
		info.lPOSNumber = m_lPosNumber;
		trs.m_lNumber = _Module.m_server.NewTransactNumber();
		info.lSyncNumber = trs.m_lNumber;
		trs.UpdateTime();
		trs.UpdateStartTime();	//TD 40492
		trs.UpdateStartFuelingTime();	//4.0.22.511 72606
		trs.m_lControllerSeqNum = trs.m_lNumber; //4.0.24.480 278543 

		if (lFlags & CHECK_AND_RESERVE_FLAG)
		{
			long lMisc = stat.GetMisc();
			trs.m_lMisc |= MISC_PUMP_CHECK_AND_RESERVE;

			lMisc |= MISC_PUMP_CHECK_AND_RESERVE;
			stat.SetMisc(lMisc);
			_Module.m_server.m_cPumpArray[p].SetStat(stat);
		}

		short nLinkFlags = 0;
		_Module.m_server.m_cCommonFunction.GeneratePrePayTrsLinkFlags(lFlags, &nLinkFlags);			//4.0.24.73 No TD

		if (lFlags & LIMIT_FLEET)
		{
			trs.m_lResExt = RES_EXT_FLEET;

			if (_Module.m_server.m_cServerInfo.GetWaitForPriceChangeAckFlags())
				nLinkFlags += TRS_LINK_FLEET_PRICE_CHANGE;
		}

		trs.SetLink(nLinkFlags, info.lSyncNumber);

		if (!_Module.m_server.m_cModeArray[stat.GetMode() - 1].AutoAuthorize(PRE_PAY_AT_POS, NULL_RES_EXT) && !(trs.m_lMisc & MISC_PUMP_CHECK_AND_RESERVE)) //4.0.17.20 - TD 26384
		{
			trs.SetStatus(WAIT_TIME_OUT);

			//Store time out in trs history
			trs.m_lMisc |= MISC_PREPAY_TIMED_OUT;
		}
		else
		{
			if (_Module.m_server.IsNotAllowedWaitingPrePay())
			{
				CPumpTransact cTmptrs;
				if (((!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&cTmptrs)) ||
					(!_Module.m_server.m_cPumpArray[p].IsPumpIdle())) &&	//4.0.1.0
					(!(trs.m_lMisc & MISC_PUMP_CHECK_AND_RESERVE)))			//4.0.17.20 - TD 26384
				{
					trs.SetStatus(WAIT_TIME_OUT);
					trs.m_lMisc |= MISC_PREPAY_TIMED_OUT;

					if (LOG_BASIC_FLOW_CONTROL)
					{
						sLogMsg.Format("Trs=%ld Set to Time Out", trs.m_lNumber);
						_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, p + 1, LOG_PUMP, sLogMsg);
					}
				}
				else
					trs.SetStatus(WAIT_RES);
			}
			else
			{
				//In case there is IN_PROGRESS trs, new PrePay trs will move to WAIT_TIME_OUT
				if (_Module.m_server.m_cPumpArray[p].IsCurrentTrs())
				{
					CPumpTransact cCurrentTrs;
					if ((!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&cCurrentTrs)) &&
						(!_Module.m_server.m_cPumpArray[p].IsPumpIdle()) &&
						cCurrentTrs.m_lRoundedValue)
					{
						trs.SetStatus(WAIT_TIME_OUT);
						trs.m_lMisc |= MISC_PREPAY_TIMED_OUT;

						sLogMsg.Format("Move trs status to WAIT_TIME_OUT in case there is IN_PROGRESS trs on this pump");
						_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
					}
					else
						trs.SetStatus(WAIT_RES);
				}
				else
					trs.SetStatus(WAIT_RES);
			}
		}

		trs.m_lActiveShiftNumber = _Module.m_server.GetActiveShiftNumber();
		trs.m_lPump = lPumpNumber;

		//4.0.24.60 TD 112315 - In case there is PREPAY_WITH_XML_BUFF flag - we might have inserted Extra Info data
		//So do not delete it in CentralInsertTrs (in CheckVlidityTransactionNumber)
		BOOL bDoNotValidateExtraInfo = ((lFlags & PREPAY_WITH_XML_BUFF) ||
			(lFlags & PREPAY_WITH_XML_BUFF_WITH_LOYALTY_DISCOUNT_DATA) ||
			lFlags & MULTIPLE_VOLUME_AND_AMOUNT_LIMIT);

		int iRetNum = _Module.m_server.CentralInsertTrs(lPumpNumber, &trs, FALSE, bDoNotValidateExtraInfo);

		switch (iRetNum)
		{
		case 0: retCode = GPI_TRS_WAIT_IN_LIST;
			_Module.m_server.SetPrePayReTryReservation(lPumpNumber - 1, TIME_BETWEEN_RETRYING_RESERVED_PRE_PAID_PUMP); //4.0.24.351 //4.0.124.355 //4.0.27.60 231255
			*lSyncNumber = trs.m_lNumber;
			break;
		case 1: retCode = GPI_PUMP_TRS_LIST_FULL;
			break;
		case 3: retCode = GPI_TRS_FIRST_IN_LIST;
			*lSyncNumber = trs.m_lNumber;
			break;
		case 4:
			retCode = GPI_TRS_NOT_FOUND;
			break;
		}
		if (retCode != GPI_PUMP_TRS_LIST_FULL)
		{
			TAG_DISCOUNT_LOYALTY_INFO4 cLoyaltyInfo;	//4.0.21.501 //4.0.23.270 111746 //4.0.24.60
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				sLogMsg.Format("AuthorizeAndLimit Link Operation (INSERT) Pump %02ld, SyncNumber %ld, Level %d, GradePrice %ld, MaxValue %ld, MaxVolume %ld, Flags %x, sPreSetLimitType %d, lPreSetLimit %ld , lPurchaseID %ld",
					info.lPumpNumber,
					info.lSyncNumber,
					info.nLevel,
					info.lGradePrice,
					info.lMaxValue,
					info.lMaxVolume,
					info.lFlags,
					info.sPreSetLimitType,
					info.lPreSetLimit,
					info.lPurchaseID);//4.0.20.54

				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
			}

			memset(&cLoyaltyInfo, ' ', sizeof(TAG_DISCOUNT_LOYALTY_INFO4)); //4.0.20.450 66977 //4.0.21.501 //4.0.23.270 111746 //4.0.24.60
			if (lFlags & PREPAY_WITH_XML_BUFF)
			{
				//4.0.22.500 - TD 70353
				long lRetCode = _Module.m_server.InsertXMLReceiptIntoReceiptQDX(lFlags, bstrPrePay, trs, lPumpNumber);

				if (!lRetCode)
					retCode = GPI_TRS_PARAM_NOT_VALID;

				lRtc = _Module.m_server.TrsLinkOperation(REC_INSERT, &trs, &info, &cLoyaltyInfo);

			}
			else if ((lFlags & MULTIPLE_VOLUME_AND_AMOUNT_LIMIT) && (FALSE == (lFlags & PREPAY_WITH_XML_BUFF))) // CR 370688
			{
				BOOL bRetCode = _Module.m_server.SaveGradeLimitsAndDiscountsArrayToExtraData(lPumpNumber, bstrPrePay, trs);

				if (!bRetCode)
					retCode = GPI_TRS_PARAM_NOT_VALID;

				lRtc = _Module.m_server.TrsLinkOperation(REC_INSERT, &trs, &info);

			}
			else if ((lFlags & LOYALTY_DISCOUNT_DATA))
			{
				lRtc = HandleLoyaltyPrepPay(lPumpNumber, trs, bstrPrePay, cLoyaltyInfo, info); // TD  233242 4.0.23.1050	
			}
			else if (lFlags & PREPAY_WITH_XML_BUFF_WITH_LOYALTY_DISCOUNT_DATA)						//4.0.24.71 No TD
			{
				char	szXmlBuff[MAX_XML_BUF_LEN];
				memset(&szXmlBuff, 0, sizeof(szXmlBuff));

				int lBuffSize = Bstr2Buffer(bstrPrePay, szXmlBuff, sizeof(szXmlBuff));

				if (lBuffSize > 0)
				{
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, "CGPI::AuthoirzeAndLimitPAPExternal PREPAY_WITH_XML_BUFF_ and _LOYALTY_DISCOUNT_DATA, bAllowPrePay=TRUE");

					_Module.m_server.UpdatePrePayWithXmlReceiptAndDiscountData(lPumpNumber, szXmlBuff, &trs, &info);			//4.0.24.90 TD 121218
				}
			}
			else if (lFlags & LIMIT_FLEET) //4.0.9.507
			{
				char sTmpBuffrer[sizeof(TAG_PRE_AUTH_TRS_DETAILS) + 2];
				memset(sTmpBuffrer, 0, sizeof(sTmpBuffrer));
				TAG_PRE_AUTH_TRS_DETAILS rec;
				DIESEL_INFO	cDieselInfo;

				CComBSTR smmm(bstrPrePay);

				memset(&cDieselInfo, 0, sizeof(cDieselInfo));
				int iLen = WideCharToMultiByte(CP_ACP, 0, bstrPrePay, -1, sTmpBuffrer, sizeof(sTmpBuffrer), NULL, NULL);

				if (iLen)
				{
					CFleetCard cFleetCardConvert;

					memcpy(&rec, sTmpBuffrer, min(sizeof(sTmpBuffrer), sizeof(rec)));
					cDieselInfo.lFlag = lFlags; // + PREPAY_PROMPT_DISABLE;  //4.0.10.32   //4.0.10.46
					cFleetCardConvert.ConvertTAG2RowPreAuthDetails(rec.cItem1, cDieselInfo.cItem1);
					cFleetCardConvert.ConvertTAG2RowPreAuthDetails(rec.cItem2, cDieselInfo.cItem2);

					if ((cDieselInfo.cItem1.lGrade) && (!cDieselInfo.cItem2.lGrade) && (cDieselInfo.cItem1.lGrade == _Module.m_server.m_cPumpArray[p].GetReeferGrade()))   //4.0.10.50
					{
						lFlags += LIMIT_REEFER;
						info.lFlags += LIMIT_REEFER;

						if (LOG_BASIC_FLOW_CONTROL)
						{
							CString str;
							str.Format("Pump choosen reefer only");
							_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
						}
					}

					lRtc = _Module.m_server.TrsLinkOperation(REC_INSERT, &trs, &info, &cDieselInfo);
				}
				else
				{
					DWORD dwTmp = GetLastError();
					if (LOG_BASIC_FLOW_CONTROL)
					{
						sLogMsg.Format("record size =%d Error = %ld - len=%d", sizeof(TAG_PRE_AUTH_TRS_DETAILS), dwTmp, smmm.Length());
						_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
					}
				}

				//detect fleet card.	
				if (LOG_BASIC_FLOW_CONTROL)
				{
					sLogMsg.Format("Detect Fleet card ilen=%d , lRtc=%ld", iLen, lRtc);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
				}
			}
			else
				lRtc = _Module.m_server.TrsLinkOperation(REC_INSERT, &trs, &info, &cLoyaltyInfo);


			if (lRtc)
			{
				sLogMsg.Format("TrsLinkOperation failed . Returned Code: %d ", lRtc);
				_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);

				int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_GPI, lPumpNumber, trs.m_lNumber, &lRtc);

				switch (iRetNum)
				{
				case 0: retCode = GPI_OK; break;
				case 1: retCode = GPI_TRS_NOT_FOUND; break;
				case 2: retCode = GPI_TRS_ALREADY_IN_PROGRESS; break;
				case 3: retCode = GPI_TRS_NOT_PREPAY; break;
				case 5: retCode = GPI_FAILED_PREPAY_IO; break;
				}

				// Total answer for AuthorizeAndLimit operation
				if (retCode == GPI_OK)
					retCode = GPI_FAILED_PREPAY_IO; // report of failed IO

				if (LOG_BASIC_FLOW_CONTROL)
				{
					sLogMsg.Format("Failed to insert PrePay Trs %d, (lRtc %d) Roll back , WideCancel !", trs.m_lNumber, lRtc);
					_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
				}
			}
		}
	}
}

void CGPI::IsAllowPrePay(long p, GPIRetCode &retCode, long & lFlags, CPumpStatus &stat, CString &sLogMsg, long lPumpNumber, short nUnpaid)
{
	if (_Module.m_server.m_cPumpArray[p].GetLockStatus() != UNLOCK)
	{
		retCode = GPI_PUMP_LOCKED;
		return;
	}
	if ((PUMPSRV_IS_MODE_MASTER) && (!(lFlags & PREPAY_FORCE)) && (_Module.m_server.m_cModeArray[stat.GetMode() - 1].PrePayNotValid()))
	{
		retCode = GPI_NOT_VALID_IN_CURRENT_MODE;
		return;
	}

	BOOL bAllowPrePay = TRUE;
	short nPrePayWaiting = _Module.m_server.m_cPumpArray[p].GetNumberOfNonClosedPrePay();
	short nMaxPrePayAllowed = _Module.m_server.m_cPumpArray[p].GetMaxPrePayTrsAllowed();

	if (nMaxPrePayAllowed == 0)
	{
		if (_Module.m_server.m_cPumpArray[p].IsCurrentTrs())
			bAllowPrePay = FALSE;
	}
	else if (nPrePayWaiting >= nMaxPrePayAllowed)
	{
		bAllowPrePay = FALSE;
	}
	else if ((_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()) && (stat.GetStatus() == READY) &&
		((stat.GetRes() == PAY_AT_KIOSK) || ((stat.GetRes() == PAY_AT_PUMP) && (stat.GetResExt() == RES_EXT_PAK_LOYALTY))))
	{//4.0.25.0 125390
		bAllowPrePay = FALSE;
		retCode = GPI_PUMP_TRS_LIST_FULL;
		sLogMsg.Format("Attempt to create a prepay on pump %d while pak with loyalty is in process", p + 1);
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);

	}
	else if (_Module.m_server.CheckVITConflict())
	{
		CPumpTransact cTmptrs;
		if (!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&cTmptrs))
		{
			//If current trs on pump is a prepay trs then
			//allow authorization of second prepay trs only if
			//current trs has exceeded AVIConflict volume, because
			//only then can we be sure that the current TRS will end as 
			//PAID prepay (under the AVIConflict level a prepay reverts
			//to WAIT_TIME_OUT).
			if ((cTmptrs.m_lRawVolume <= _Module.m_server.GetMaxVolumeForVITConflict()) &&
				(cTmptrs.GetLinkFlags() & TRS_LINK_PREPAY_NUMBER))
			{
				bAllowPrePay = FALSE;
			}
		}
		//4.0.1.36 Eyal - prevent creation of prepay
		//if maximum number of unpaid PAKs is reached
		//and mode auto authorizes prepay
		if ((nUnpaid >= MAX_ALLOWED_UNPAID_TRS(p)) &&
			_Module.m_server.m_cModeArray[stat.GetMode() - 1].AutoAuthorize(PRE_PAY_AT_POS, NULL_RES_EXT))
		{
			bAllowPrePay = FALSE;
			retCode = GPI_PUMP_TRS_LIST_FULL;
			sLogMsg.Format("Attempted to create a prepay on pump %d while post pay trs list is full", p + 1);
			_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, sLogMsg);
		}
	}

	//Check that pump is not checked and reserved already
	if (lFlags & CHECK_AND_RESERVE_FLAG)
	{
		long lMisc = stat.GetMisc();
		if (lMisc & MISC_PUMP_CHECK_AND_RESERVE)
		{
			retCode = GPI_PUMP_ALREADY_RESERVED;
		}
	}

	if (FALSE == bAllowPrePay)
	{
		retCode = GPI_PUMP_TRS_LIST_FULL;
	}
}

void CGPI::InitDebit__(long lPumpGroup01, long lPumpGroup2, long lFlags, long *pVal)
{
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if (lPumpGroup01)
	{
		if (PUMP_NOT_VALID(lPumpGroup01))
			retCode = CTRL_INVALID_PUMP_NUMBER;
		if (PUMP_NOT_IN_WORKING_STATE(lPumpGroup01))
			retCode = CTRL_PUMP_NOT_IN_WORKING_STATE;
	}

	// all pump

	if (retCode == CTRL_OK)
	{
		long lPumpNumber = lPumpGroup01; //4.0.3.58

		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if (!lPumpGroup01)
		{
			for (long i = 1; i <= _Module.m_server.GetLastValidPump(); i++)  //4.0.0.79
			{
				if (PUMP_NOT_VALID(i))
					retCode = CTRL_INVALID_PUMP_NUMBER;
				if (PUMP_NOT_IN_WORKING_STATE(i))
					retCode = CTRL_PUMP_NOT_IN_WORKING_STATE;
				else
					_Module.m_server.m_cInitDebitHandler.SetStatus(i, DEBIT_SERVER_REQUEST_TO_START); //4.0.3.58
			}
		}
		else
		{
			if (_Module.m_server.m_cInitDebitHandler.GetStatus(lPumpNumber) == DEBIT_SERVER_IDLE)
			{
				_Module.m_server.m_cInitDebitHandler.SetStatus(lPumpNumber, DEBIT_SERVER_REQUEST_TO_START); //4.0.3.58
			}
			else
			{
				retCode = CTRL_IN_MIDDLE_OF_PROCESS;
				_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
			}
		}
	}

	if (pVal)
		*pVal = retCode;
}

//TD 453656
STDMETHODIMP CGPI::GetCurrentTransactionNumber(long lPumpNumber, long *pTrsNumber, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetCurrentTransactionNumber   start");

#ifdef CATCH_GPI
	try
	{
#endif
		_Module.m_server.GetCurrentTransactionNumber(lPumpNumber, pTrsNumber, pVal);

#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_Module.m_server.m_cLogger.LogClassMsg("CGPI", "GetPrePaidTrs3()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


STDMETHODIMP CGPI::AuthorizePump4(long lPumpNumber, short nLevel, long lGradePrice, long lFlags, long lFlags2, long lLimit, short nGroup, BSTR sRec, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::AuthorizePump4   start");
#ifdef CATCH_GPI
	try
	{
#endif

		AuthorizePump2__(lPumpNumber, nLevel, lGradePrice, lFlags, lLimit, nGroup, pVal, sRec);


#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_Module.m_server.m_cLogger.LogClassMsg("CGPI", "AuthorizePump4()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


STDMETHODIMP CGPI::SetProcessExt(long lProcessNumber, long lParam1, BSTR sInData, BSTR *sOutData, long * pVal)
{


#ifdef CATCH_GPI
	__try
	{
#endif
		SetProcessExt__(lProcessNumber, lParam1, sInData, sOutData, pVal);

#ifdef CATCH_GPI
	}
	__except (_HANDLE_EXCEPTION("GCI::SetProcessExt"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_Module.m_server.m_cLogger.LogClassMsg("CGPI", "SetProcessExt()", m_lPosNumber, LOG_POS, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;

}



BOOL CGPI::IsAuthorizeTerminalSrvTrs(long lPumpNumber, long m_lPosNumber, CPumpStatus PumpStat)
{
	BOOL bRetVal = FALSE;

	bRetVal = (_Module.m_server.GetIFSFTerminalSrvInUse() && ((PumpStat.GetRes() == PAY_AT_PUMP) && (PumpStat.GetResExt() == RES_EXT_PAP_EXTERNAL)));


	CString str;
	str.Format("IsAuthorizeTerminalSrvTrs:  lPumpNumber =%ld, RetVal=%ld ", lPumpNumber, bRetVal);


	return bRetVal;

}


BOOL CGPI::IsAuthorizeMobileTrs(long lPumpNumber, long m_lPosNumber, CPumpStatus PumpStat)
{
	BOOL bRetVal = FALSE;


	bRetVal = (_Module.m_server.IsFuelMobileSrvInUse() && (((PumpStat.GetRes() == PAY_AT_PUMP) && (PumpStat.GetResExt() == RES_EXT_MOBILE))));

	CString str;
	str.Format("IsAuthorizeMobileTrs:  lPumpNumber =%ld, RetVal=%ld ", lPumpNumber, bRetVal);

	_Module.m_server.m_cLogger.LogMsg(m_lPosNumber, LOG_POS, str);


	return bRetVal;

}


GPIRetCode CGPI::CancelTrsByExternal(long lTrsNumber, long lPumpNumber, CPumpStatus &pumpStat)
{
	GPIRetCode retCode = GPI_OK;

	if (lTrsNumber == 0)
	{
		if (LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("CGPI::CancelTrsByExternal: TrsNumber is Epmty. Pump=%ld, retCode=%ld", lPumpNumber, GPI_TRS_NOT_FOUND);
			_Module.m_server.m_cLogger.LogMsg(str);
		}

		return GPI_TRS_NOT_FOUND;
	}


	if (pumpStat.GetStatus() == DISPENSING)
	{
		if (LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("CGPI::CancelTrsByExternal: Pump is DISPENSING. Pump=%ld, retCode=%ld", lPumpNumber, GPI_CANT_CANCEL_TRS);
			_Module.m_server.m_cLogger.LogMsg(str);
		}

		return GPI_CANT_CANCEL_TRS;


	}


	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString	str;
		str.Format("CGPI::CancelTrsByExternal. Send CancelTrs: Pump=%ld, Status=%ld, Res=%ld", lPumpNumber, pumpStat.GetStatus(), pumpStat.GetRes());
		_Module.m_server.m_cLogger.LogMsg(str);
	}

	CMD_STRUCT_TRS_CANCEL cmdInfo;
	cmdInfo.lTrsNumber = lTrsNumber;
	cmdInfo.lPumpStatus = pumpStat.GetStatus();
	cmdInfo.lRes = pumpStat.GetRes();
	cmdInfo.lResExt = pumpStat.GetResExt();
	_Module.m_server.SetCommand(CMD_TRS_CANCEL, SERVER_POS_NUMBER, lPumpNumber, &cmdInfo, sizeof(CMD_STRUCT_TRS_CANCEL));


	if ((pumpStat.GetRes() == PAY_AT_PUMP) && (pumpStat.GetResExt() == RES_EXT_MOBILE))
		_Module.m_server.m_cGPIEventMgr.AddCancelTrs(lPumpNumber,
			cmdInfo.lTrsNumber, cmdInfo.lPumpStatus,
			cmdInfo.lRes, cmdInfo.lResExt);

	return retCode;
}


GPIRetCode CGPI::GetAllGradesData(long lPumpNumber, CComBSTR &bstrGrades, long * lRecordCount, long lFlags)
{
	GPIRetCode retCode = GPI_OK;

	char sTmp[sizeof(TAG_GARDE_ALL_INFO) + 2];
	TAG_GARDE_ALL_INFO cTmpTagGradeAllInfo;

	memset(&cTmpTagGradeAllInfo, ' ', sizeof(cTmpTagGradeAllInfo));

	if (lPumpNumber == 0)//return all the grades in the system
	{
		for (int i = 1; i <= MAX_GRADES_99; i++)
		{
			if (GetGradeAllInfo(i, cTmpTagGradeAllInfo, lFlags)) //4.0.26.508 147183
			{
				memset(sTmp, 0, sizeof(sTmp));
				memcpy(sTmp, &cTmpTagGradeAllInfo, sizeof(cTmpTagGradeAllInfo));
				bstrGrades.Append(sTmp);
				(*lRecordCount)++;
			}
			memset(&cTmpTagGradeAllInfo, ' ', sizeof(cTmpTagGradeAllInfo));


		}
	}
	else	//one Pump
	{
		if (GRADE_NOT_VALID((short)lPumpNumber)) //4.0.22.502 71801
			retCode = GPI_INVALID_GRADE_NUMBER;

		else if (GetGradeAllInfo(lPumpNumber, cTmpTagGradeAllInfo, lFlags)) //4.0.26.508 147183
		{
			memset(sTmp, 0, sizeof(sTmp));
			memcpy(sTmp, &cTmpTagGradeAllInfo, sizeof(cTmpTagGradeAllInfo));
			bstrGrades.Append(sTmp);
			(*lRecordCount)++;
		}

	}

	return retCode;
}

GPIRetCode CGPI::GetGradesPositionData(long lPumpNumber, CComBSTR &bstrGrades, long * lRecordCount)
{
	GPIRetCode retCode = GPI_OK;
	long	lTmpGrade = 0;
	char	szGrade[MAX_FIELD_VAL] = { 0 };
	CString sTable;
	DWORD dwTmp = 0;


	if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GPI_INVALID_PUMP_NUMBER;

	if (retCode == GPI_OK)
	{
		char sTmp[sizeof(TAG_GRADE_POSITION_INFO) + 2];
		TAG_GRADE_POSITION_INFO cTmpGradePositionInfo;


		for (int i = 1; i <= MAX_POSITIONS_PER_PUMP; i++)
		{
			if (TRUE == _Module.m_server.m_cCommonFunction.GetGradePositionInfo(lPumpNumber, i /*Position*/, cTmpGradePositionInfo))
			{
				memset(sTmp, 0, sizeof(sTmp));
				memcpy(sTmp, &cTmpGradePositionInfo, sizeof(cTmpGradePositionInfo));
				bstrGrades.Append(sTmp);
				(*lRecordCount)++;
			}
		}

	}

	return retCode;

}

GPIRetCode CGPI::GetPumpGradeInfoImpl(long lPumpNumber, CComBSTR &bstrGrades, long * lRecordCount)		//4.0.25.1130 
{
	GPIRetCode retCode = GPI_OK;
	long	lTmpGrade = 0;
	char	szGrade[MAX_FIELD_VAL] = { 0 };
	CString sTable;
	DWORD dwTmp = 0;


	if (PUMP_NOT_VALID(lPumpNumber))//4.0.22.502 71801
		retCode = GPI_INVALID_PUMP_NUMBER;
	if (retCode == GPI_OK) //4.0.22.502 71801
	{
		char sTmp[sizeof(TAG_GARDE_INFO) + 2];
		TAG_GARDE_INFO cTmpTagGradeInfo;
		if (!_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetDieselPumpFlags())
		{
			//if flag TANK_MINIM return the map grade
			MAP_GRADE_TANK_LEVEL cMapValidGrade;
			_Module.m_server.m_cTankLevelMap.GetValidGradesTankLevelForOnePump(lPumpNumber, cMapValidGrade);
			for (int y = 0; y < cMapValidGrade.lNumOfValidGrades; y++)
			{
				if (GetGradeInfo(cMapValidGrade.GradeInfo[y].GetGradeNamber(), cTmpTagGradeInfo))
				{
					if (_Module.m_server.GetMaxGrade999())
					{
						//4.0.26.501 - TD 116227
						if (a2l(cTmpTagGradeInfo.sGradeNumber, sizeof(cTmpTagGradeInfo.sGradeNumber)) > 0)
							_Module.m_server.GetExtGradeByGrade(cTmpTagGradeInfo.sGradeNumber, sizeof(cTmpTagGradeInfo.sGradeNumber), lPumpNumber);
					}

					memset(sTmp, 0, sizeof(sTmp));
					memcpy(sTmp, &cTmpTagGradeInfo, sizeof(cTmpTagGradeInfo));
					bstrGrades.Append(sTmp);
					(*lRecordCount)++;
				}
			}


		}
		else
		{

			for (int i = 1; i <= MAX_POSITIONS_PER_PUMP; i++)
			{
				lTmpGrade = _Module.m_server.m_cPumpArray[lPumpNumber - 1].NozzleToGrade(i);

				if (GetGradeInfo(lTmpGrade, cTmpTagGradeInfo))
				{
					memset(sTmp, 0, sizeof(sTmp));
					memcpy(sTmp, &cTmpTagGradeInfo, sizeof(cTmpTagGradeInfo));
					bstrGrades.Append(sTmp);
					(*lRecordCount)++;


					// Check Diesel pumps.
					if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetDieselPumpFlags())
					{
						sTable.Format("Grades\\Grade%.02ld", lTmpGrade);
						_Module.m_server.m_cParam.LoadParam((LPCSTR)sTable, "ConvertCode", &dwTmp, (DWORD)0, FALSE);

						if (GetGradeInfo(dwTmp, cTmpTagGradeInfo))
						{
							sTable.Format("Grades\\Grade%.02ld", dwTmp);
							memset(sTmp, 0, sizeof(sTmp));
							memcpy(sTmp, &cTmpTagGradeInfo, sizeof(cTmpTagGradeInfo));
							bstrGrades.Append(sTmp);
							(*lRecordCount)++;
						}
					}
				}
			}
		}
	}

	return retCode;

}

//4.0.25.1140 TD 412765
void CGPI::SetPumpTestFlags(long lPumpNumber, long lFlags, CMD_STRUCT_AUTHORIZE_LIMIT &cmdAuthorize, BSTR sRec)
{

	if (lFlags & TRS_IS_PUMP_TEST)
		cmdAuthorize.lFlags |= TRS_IS_PUMP_TEST;
	else if (lFlags & TRS_IS_PUMP_CALIBRATION)
		cmdAuthorize.lFlags |= TRS_IS_PUMP_CALIBRATION;

	if ((cmdAuthorize.lFlags & TRS_IS_PUMP_TEST) || (cmdAuthorize.lFlags & TRS_IS_PUMP_CALIBRATION))
		_Module.m_server.m_cCommonFunction.SetPumpCalibrationDetails(lPumpNumber, sRec);
}


STDMETHODIMP CGPI::GetOfflineTanks(long lFlags, BSTR* sTankList, long *pVal)		//CR 443524
{
	LOG_EXTERNALCALL("CGPI::GetOfflineTanks   start");


#ifdef CATCH_GPI
	try

	{
#endif
		GetOfflineTanks__(lFlags, sTankList, pVal);
#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_Module.m_server.m_cLogger.LogClassMsg("CGPI", "GetOfflineTanks()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;
}


/******************************************************************************
Description:	Get list of offline Tanks
Returns:      	void
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
PolinaA			01/02/2016   12:00		Start		CR???
******************************************************************************/
void CGPI::GetOfflineTanks__(long lFlags, BSTR * bstrTankList, long *pVal)	//CR 443524
{
	CString sOfflineTanksBuffer; // e.g 1,3,5

	for (int lTankIndex = 0; lTankIndex < MAX_TANKS; lTankIndex++)
	{
		long lStatus = _Module.m_server.m_cTankArray[lTankIndex].GetTankStatus();

		if (lStatus == TANK_READ_NOT_AVAILABLE)
		{
			CString sTmp;
			sTmp.Format("%d", lTankIndex + 1);
			sOfflineTanksBuffer += sTmp += ",";
		}
	}

	sOfflineTanksBuffer.Delete(sOfflineTanksBuffer.GetLength() - 1, 1);

	// copy the buffer to the returned bstr
	BYTE sTmpStr[1000];
	Str2StrNull((BYTE *)sOfflineTanksBuffer.GetBuffer(0), sOfflineTanksBuffer.GetLength(), sTmpStr);
	CComBSTR strResult(sizeof(sTmpStr), (LPCSTR)sTmpStr);
	*bstrTankList = strResult.Copy();

	CString sLogMsg;
	sLogMsg.Format("GetOfflineTanks: offline tanks list: %s", sOfflineTanksBuffer.GetBuffer(0));
	_Module.m_server.m_cLogger.LogMsg(sLogMsg);

}


STDMETHODIMP CGPI::QueryPumpInfo(long lPumpNumber, long lFlags, BSTR sInData, BSTR* sOutData,/*[out,retval]*/ long *pVal)
{
	LOG_EXTERNALCALL("CGPI::QueryPumpInfo   start");
#ifdef CATCH_GPI
	try
	{
#endif

		//	AuthorizePump2__(lPumpNumber, nLevel, lGradePrice, lFlags, lLimit, nGroup, pVal, sRec);
		QueryPumpInfo__(lPumpNumber, lFlags, sInData, sOutData, pVal);

#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_Module.m_server.m_cLogger.LogClassMsg("CGPI", "QueryPumpInfo()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;

}



/******************************************************************************
Description:	provided the details for all pumps

Returns:      	pump info XML
GPI return code


Parameters:   	lPumpNuber         - 0 ALL, 1..n requested pumps
lFlag			   - 1 pump AVAILABLE for PrePay
BSTR  sIndata	   -
BSTR  sOutdata	   - PumpsInfo xml
long *pVal	 	   - GPI return code

******************************************************************************/
void CGPI::QueryPumpInfo__(long lPumpNumber, long lFlags, BSTR sInData, BSTR* sOutData, long *pVal)
{
	long lRetVal = GPI_OK;


	if (!m_bInitDone)
		lRetVal = GPI_MUST_INIT_FIRST;

	else if (SERVER_STATE_STARTUP)
		lRetVal = GPI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		lRetVal = GPI_SERVER_NOT_READY;

	else
		if (lPumpNumber)
		{
			if (PUMP_NOT_VALID(lPumpNumber))
				lRetVal = GPI_INVALID_PUMP_NUMBER;
		}


	if (lRetVal == GPI_OK)
	{
		long p = lPumpNumber - 1;
		long lLastPump = lPumpNumber;

		if (lPumpNumber == 0)
		{
			p = 0;    // All Pumps
			lLastPump = _Module.m_server.GetLastValidPump();
		}


		CXMLInterface	m_cXmlPumpInfo;
		CXMLPointers	pRootPumpsInfo, pPumpInfo;

		if (m_cXmlPumpInfo.InitDoc() == TRUE)
		{

			m_cXmlPumpInfo.CreateElement("PumpsInfo", &pRootPumpsInfo);
			m_cXmlPumpInfo.AppendRootElement(&pRootPumpsInfo);


			for (; p < lLastPump; p++)
			{

				m_cXmlPumpInfo.CreateElement("PumpInfo", &pPumpInfo);
				m_cXmlPumpInfo.SetLongAttribute("PumpNumber", p + 1, &pPumpInfo);

				CString strStateText;
				strStateText.Format("%s", GetPumpInfoStateText(p + 1));  				// 	AVAILABLE, INUSE, OFFLINE 

				CString strMsg;
				strMsg = strStateText;
				m_cXmlPumpInfo.SetCharAttribute("pumpStatus", strMsg.GetBuffer(), &pPumpInfo);

				BOOL bCanPrePay = IsAllowPrePayTrs(p + 1);
				strMsg = bCanPrePay ? "TRUE" : "FALSE";
				m_cXmlPumpInfo.SetCharAttribute("IsAllowed", strMsg.GetBuffer(), &pPumpInfo);

				m_cXmlPumpInfo.AppendChild(&pPumpInfo, &pRootPumpsInfo);


			}

			if (LOG_FCC_FLOW_CONTROL)
			{
				CString str;
				char sBuff[MAX_XML_BUF_LEN] = { 0 };
				m_cXmlPumpInfo.GetXmlString(sBuff);
				str.Format("QueryPumpInfoToXml. XmlBuffer=%s", sBuff);
				_Module.m_server.m_cLogger.LogMsg(0, LOG_NONE, str);
			}


			m_cXmlPumpInfo.GetXmlString((BSTR*)sOutData);

#ifdef _DEBUG		
			m_cXmlPumpInfo.SaveToFile("c:\\Office\\PumpSrv\\Log\\QueryPumpInfo.xml");
#endif		

			m_cXmlPumpInfo.CloseDoc();

		}

	}

	if (lRetVal)
	{
		if (LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				m_lPosNumber, // Src
				"IGPI",
				"QueryPumpInfo__",
				TRUE, lRetVal,
				FALSE, 0);
	}

	if (pVal != NULL)
		*pVal = lRetVal;


}


CString CGPI::GetPumpInfoStateText(long lPumpNumber)
{
	long p = lPumpNumber - 1;

	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	CPumpStatus stat;
	stat = _Module.m_server.m_cPumpArray[p].GetStat();

	switch (stat.GetStatus())
	{
	case READY:	return _T("AVAILABLE");	break;
	case REQUESTING:
	case AUTHORIZED:
	case NOZZLE_LEFT_OUT:
	case DISPENSING:	return _T("INUSE");	break;
	}

	CString csPumpInfoStatus(_T("OFFLINE"));
	return csPumpInfoStatus;
}


BOOL CGPI::IsAllowPrePayTrs(long lPumpNumber)
{
	BOOL bAllowPrePay = TRUE;

	long p = lPumpNumber - 1;

	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	CPumpStatus stat;
	stat = _Module.m_server.m_cPumpArray[p].GetStat();

	if (_Module.m_server.m_cPumpArray[p].GetLockStatus() != UNLOCK)
		return FALSE;


	if (_Module.m_server.m_cModeArray[stat.GetMode() - 1].PrePayNotValid())
		return FALSE;


	if ((stat.GetStatus() == READY) && (stat.GetRes() == NULL_RES))
	{

	}
	else
		return FALSE;

	short nPrePayWaiting = _Module.m_server.m_cPumpArray[p].GetNumberOfNonClosedPrePay();
	short nMaxPrePayAllowed = _Module.m_server.m_cPumpArray[p].GetMaxPrePayTrsAllowed();

	if (nMaxPrePayAllowed == 0)
	{
		if (_Module.m_server.m_cPumpArray[p].IsCurrentTrs())
			return FALSE;
	}

	if (nPrePayWaiting != 0)
		return FALSE;

	return bAllowPrePay;
}


GPIRetCode CGPI::GetReceiptFromTerminalSrv(TAG_OLA_REC11 &tagRec, short nFlags, char * szReceiptKey, short * nIndex, long lenReciptKey /* = NULL*/)
{
	//Send request for transaction list to TerminalSrv
	GPIRetCode retCode = GPI_OK;
	long lPumpNumber = 0;
	CPosInfo tmpPos;
	CString str1;

	if (!_Module.m_server.m_cPosMap.Lookup(m_lPosNumber, tmpPos))
	{
		retCode = GPI_INVALID_POS_NUMBER;
		str1.Format("GPI::QueryReceipt IFSFTerminalSrvInUse Pos Number %ld doesn't appear in PosMap", m_lPosNumber);
		_LOGMSG.LogMsg(str1);
	}
	else
	{
		if ((eFileOperation)nFlags == REC_READ_FIRST)
		{
			tmpPos.m_cPOSTrsList.RemoveAll();
			_Module.m_server.m_cPosMap.SetAt(m_lPosNumber, tmpPos);
			_Module.m_server.m_cCommonFunction.GetTransactionList4IFSFTerminal(lPumpNumber, m_lPosNumber, GET_RECEIPT_TRS_BY_TRACK2,
				szReceiptKey, lenReciptKey);

			_Module.m_server.m_cPosMap.Lookup(m_lPosNumber, tmpPos);
			if (!tmpPos.m_cPOSTrsList.GetSize())
			{
				str1.Format("GPI::QueryReceipt IFSFTerminalSrvInUse Got empty transactions list for track2");
				_LOGMSG.LogMsg(str1);
				retCode = GPI_NO_RECEIPT;
			}
			else
			{
				str1.Format("GPI::QueryReceipt IFSFTerminalSrvInUse Got transactions list for track2 count = %ld", tmpPos.m_cPOSTrsList.GetSize());
				_LOGMSG.LogMsg(str1);
				memcpy(&tagRec.tagOlaRec10, &tmpPos.m_cPOSTrsList[0], sizeof(tmpPos.m_cPOSTrsList[0]));	//4.0.206.509 1.
																										//*nIndex = 1;	//next record will be in position 1
			}
		}
		else if (((eFileOperation)nFlags == REC_READ_NEXT) || ((eFileOperation)nFlags == REC_READ_SEQ))
		{
			if (*nIndex >= (tmpPos.m_cPOSTrsList.GetSize() - 1))
			{
				str1.Format("GPI::QueryReceipt IFSFTerminalSrvInUse Input index exceeds transactions array size");
				_LOGMSG.LogMsg(str1);
				retCode = GPI_NO_RECEIPT;
			}
			else
			{
				memcpy(&tagRec.tagOlaRec10, &tmpPos.m_cPOSTrsList[++(*nIndex)], sizeof(tmpPos.m_cPOSTrsList[++(*nIndex)]));
				str1.Format("GPI::QueryReceipt IFSFTerminalSrvInUse found next transaction at position = %ld", *nIndex);
				_LOGMSG.LogMsg(str1);
			}
		}
		else
		{
			str1.Format("GPI::QueryReceipt IFSFTerminalSrvInUse Invalid operation on transaction list, nFlags = %ld", nFlags);
			_LOGMSG.LogMsg(str1);
			retCode = GPI_NO_RECEIPT;
		}

	}

	return retCode;
}


STDMETHODIMP CGPI::GetAllWashProgram(long lFlags, BSTR* sOutData, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::GetAllWashProgram   start");
#ifdef CATCH_GPI
	try
	{
#endif
		GetAllWashProgram__(lFlags, sOutData, pVal);
#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "GetAllWashProgram()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;

}

STDMETHODIMP CGPI::SaveWashProgram(BSTR sInData, long *pVal)
{
	LOG_EXTERNALCALL("CGPI::SaveWashProgram   start");
#ifdef CATCH_GPI
	try
	{
#endif
		SaveWashProgram__(sInData, pVal);
#ifdef CATCH_GPI
	}
	catch (...)
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_LOGMSG.LogClassMsg("CGPI", "SaveWashProgram()", 0, LOG_NONE, "", LOG_LEVEL_0);

		exit(1);
	}
#endif 

	return S_OK;

}
struct WashProgramData
{
	int DisplayOrder;
	string FullName;
	int Id;
	int MapTaxLink;
	int Price;
	int Product_Discount;
	string ProductCode;
	string ShortName;
	int Volume_Limit;
};
TRAVERSE_STRUCT(WashProgramData, FIELD(DisplayOrder) FIELD(FullName) FIELD(Id) FIELD(MapTaxLink) FIELD(Price) FIELD(Product_Discount) FIELD(ProductCode) FIELD(ShortName)  FIELD(Volume_Limit))

struct WashProgramDataVector
{
	std::vector<WashProgramData> WashPrograms;
};
TRAVERSE_STRUCT(WashProgramDataVector, FIELD(WashPrograms))

void CGPI::GetAllWashProgram__(long lFlags, BSTR * sOutData, long * pVal)
{
	GPIRetCode retCode = GPI_OK;
	std::stringstream errors;
	CString sMsg;
	string strTmp;


	WashProgramData WashProgramData;
	WashProgramDataVector WashProgramDataVector;

	rapidjson::Document allWashProgramDoc;
	allWashProgramDoc.SetObject();
	rapidjson::Document::AllocatorType& allocator = allWashProgramDoc.GetAllocator();
	size_t sz = allocator.Size();

	rapidjson::StringBuffer strbuf;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;
	else if (!_Module.m_server.m_cCarWash.CarWashEnabled())
		retCode = GPI_CAR_WASH_NOT_ACTIVE;
	else
	{
		try
		{
			for (int i = 1; i <= MAX_CAR_WASH_PROGRAM; ++i)
			{
				if (TRUE == _Module.m_server.m_cCarWashProgramsDetails[i].lValidEntry) // Add valid CW program to JSON
				{
					WashProgramData.DisplayOrder = _Module.m_server.m_cCarWashProgramsDetails[i].lDisplayOrder;
					strTmp = _Module.m_server.m_cCarWashProgramsDetails[i].sFullName;
					WashProgramData.FullName = _Module.m_server.m_cCarWashProgramsDetails[i].sFullName;
					WashProgramData.Id = i;
					WashProgramData.MapTaxLink = _Module.m_server.m_cCarWashProgramsDetails[i].lMapTaxLink;
					WashProgramData.Price = _Module.m_server.m_cCarWashProgramsDetails[i].lPrice;
					WashProgramData.Product_Discount = _Module.m_server.m_cCarWashProgramsDetails[i].lDiscount;
					strTmp = _Module.m_server.m_cCarWashProgramsDetails[i].sProductCode;
					WashProgramData.ProductCode = _Module.m_server.m_cCarWashProgramsDetails[i].sProductCode;
					WashProgramData.ShortName = _Module.m_server.m_cCarWashProgramsDetails[i].sShortName;
					WashProgramData.Volume_Limit = _Module.m_server.m_cCarWashProgramsDetails[i].lVolumeLimit;
					WashProgramDataVector.WashPrograms.push_back(WashProgramData);
				}
			}

			traverse::RapidJsonWriter jsonwriter{ strbuf };
			visit(jsonwriter, WashProgramDataVector);

			CString cTmpStr(strbuf.GetString());
			*sOutData = cTmpStr.AllocSysString();




			if (LOG_FCC_FLOW_CONTROL)
			{
				CString str;
				char sBuff[MAX_XML_BUF_LEN] = { 0 };
				str.Format("GetAllWashProgram. CarWas JSON:%s", cTmpStr);
				_LOGMSG.LogMsg(0, LOG_NONE, str);
			}

		}
		catch (...)
		{
			sMsg.Format("Exception while create JSON file. GetLastError %d", GetLastError());
			_LOGMSG.LogMsg("CGPI", "GetAllWashProgram", m_lPosNumber, LOG_POS, sMsg, LOG_LEVEL_1);
			retCode = GPI_FAILED_IO;
		}
	}

	if (retCode && LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(m_lPosNumber, "IGPI", "GetAllWashProgram", TRUE, retCode, FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}

void CGPI::SaveWashProgram__(BSTR sData, long * pVal)
{
	GPIRetCode retCode = GPI_OK;
	CString sMsg;

	if (!m_bInitDone)
		retCode = GPI_MUST_INIT_FIRST;
	else if (!_Module.m_server.m_cCarWash.CarWashEnabled())
		retCode = GPI_CAR_WASH_NOT_ACTIVE;
	else
	{
		try
		{
			std::string jsonFile = Convert::ConvertBSTRToMBS(sData);
			jsonFile.erase(remove_if(jsonFile.begin(), jsonFile.end(), isspace), jsonFile.end());

			rapidjson::Document document;
			document.Parse(jsonFile.c_str());

			bool error = document.HasParseError();
			if (error)
			{
				size_t offset = document.GetErrorOffset();
				rapidjson::ParseErrorCode code = document.GetParseError();
				const char* msg = GetParseError_En(code);

				if (LOG_LOGIC_ERROR)
				{
					CString str;
					str.Format("Failed in JSON parsing: lRtc %d:%d (%s)", offset, code, msg);
					_LOGMSG.LogMsg("CGPI", "SaveWashProgram", m_lPosNumber, LOG_POS, str, LOG_LEVEL_1);
				}

				retCode = GPI_FAILED_IO;
			}
			else
			{
				WashProgramDataVector vWashProgramData;
				WashProgramData sWashProgramData;
				std::stringstream errors;
				traverse::RapidJsonReader jsonreader{ document, errors };
				visit(jsonreader, vWashProgramData);
				sWashProgramData = vWashProgramData.WashPrograms.front();

				long lProgramId = _Module.m_server.m_cCarWash.FindWashProgramIndexByProdcutCode(sWashProgramData.ProductCode);
				if (lProgramId > MAX_CAR_WASH_PROGRAM) lProgramId = _Module.m_server.m_cCarWash.GetUnusedWashProgramIndex();

				if (lProgramId >= 1 && lProgramId <= MAX_CAR_WASH_PROGRAM)
				{
					sMsg.Format("Saving new wash program %s , product code %s", sWashProgramData.FullName.c_str(), sWashProgramData.ProductCode.c_str());
					_LOGMSG.LogMsg(sMsg);
					_PARAM.SetParam("WashPrograms", lProgramId, 0, "CCH_Product_Code", FIELD_SZ, sWashProgramData.ProductCode, 0, FALSE);
					_PARAM.SetParam("WashPrograms", lProgramId, 0, "FullName", FIELD_SZ, sWashProgramData.FullName, 0, FALSE);
					_PARAM.SetParam("WashPrograms", lProgramId, 0, "ShortName", FIELD_SZ, sWashProgramData.ShortName, 0, FALSE);
					_PARAM.SetParam("WashPrograms", lProgramId, 0, "DisplayOrder", FIELD_DWORD, NULL, (DWORD)sWashProgramData.DisplayOrder, FALSE);
					_PARAM.SetParam("WashPrograms", lProgramId, 0, "ProdcutDiscount", FIELD_DWORD, NULL, (DWORD)sWashProgramData.Product_Discount, FALSE);
					_PARAM.SetParam("WashPrograms", lProgramId, 0, "MapTaxLink", FIELD_DWORD, NULL, (DWORD)sWashProgramData.MapTaxLink, FALSE);
					_PARAM.SetParam("WashPrograms", lProgramId, 0, "Price", FIELD_DWORD, NULL, (DWORD)sWashProgramData.Price, FALSE);
					_PARAM.SetParam("WashPrograms", lProgramId, 0, "Volume_Limit", FIELD_DWORD, NULL, (DWORD)sWashProgramData.Volume_Limit, FALSE);
					_PARAM.SetParam("WashPrograms", lProgramId, 0, "ValidEntry", FIELD_DWORD, NULL, TRUE, FALSE);

					long lRetCode;
					_Module.m_server.m_cCommonFunction.Reconfigure2(CARWASH_TABLE, 0, 0, 0, &lRetCode, FALSE);
					if (lRetCode)
						_LOGMSG.LogMsg("CGPI", "SaveWashProgram", m_lPosNumber, LOG_POS, "Failed to send Reconfigure2 event!", LOG_LEVEL_1);
				}
				else
				{
					_LOGMSG.LogMsg("CGPI", "SaveWashProgram", m_lPosNumber, LOG_POS, "ProgramIndex it out of range!", LOG_LEVEL_1);
				}
			}
		}
		catch (...)
		{
			sMsg.Format("Exception while parsing JSON file. GetLastError %d", GetLastError());
			_LOGMSG.LogMsg("CGPI", "SaveWashProgram", m_lPosNumber, LOG_POS, sMsg, LOG_LEVEL_1);
			retCode = GPI_FAILED_IO;
		}
	}


	if (retCode && LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(m_lPosNumber, "IGPI", "SaveWashProgram", TRUE, retCode, FALSE, 0);

	if (pVal != NULL)
		*pVal = retCode;
}


void CGPI::LogFlags(long lGetTrsFlags, long lGetMinimalPumpsFlags, long lGetPumpInfo3Flags, long lParam)
{
	CString str;
	str.Format("CGPI::GetPumpsTrsInfo   lGetTrsFlags[%08x]   lGetMinimalPumpsFlags[%08x]  lGetPumpInfo3Flags[%08x]  lParam[%08x]", lGetTrsFlags, lGetMinimalPumpsFlags, lGetPumpInfo3Flags, lParam);
	if (LOG_ALL_FLOW_CONTROL)
		_LOGMSG.LogMsg(m_lPosNumber, LOG_POS, str);
}