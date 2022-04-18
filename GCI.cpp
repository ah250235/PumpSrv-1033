// GCI.cpp : Implementation of CGCI
#include "stdafx.h"
#include "PumpSrv.h"
#include "GCI.h"
#include <process.h> 
#include "Math.h"
#include "CashAcceptor.h" //4.0.1.38
#include "ServerMain.h"
#include "ShiftTanks.h"
#include "ShiftDelivery.h"
#include "ShiftTotals.h"
#include "FleetCard.h"  //4.0.9.507
#include "XMLInterface.h" //4.0.13.502
#include "SQLPumpSrvFile.h"
#include "PumpSrvDatabase.h"
#include "TimingProfiling.h"
#include "SQLReceiptDB.h"
#include "CStrFormat.h"


#define LOG_MSG(a,b) _Module.m_server.m_cLogger.LogClassMsg("CGCI", (a), -1, LOG_NONE, (b));
#define LOG_MSG_START_METHOD(a)   CString sFuncName=a;LOG_MSG(a, "start");
#define LOG_MSG_FUNCINC(a) LOG_MSG(sFuncName, a);
#define LOG_MSG_FUNCINC_RETVAL(a) CString sRet;sRet.Format("Return Value is %d",a) ;LOG_MSG(sFuncName, sRet);
#define LOG_MSG_END_METHOD   LOG_MSG(sFuncName, "end");


/******************************************************************************
Description:	Convert string buffer of ExtraInfo to Database record.
Returns:
Parameters:   	sTagRec - Extra info string record,

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Marik			24/5/2004   13:41		Start
******************************************************************************/
long ConvertTagRecord(TAG_EXTRA_INFO  & sTagRec, EXTRA_INFO & sRec, CPumpTransact & cTrs)
{
	long lRetCode = 0;

	sRec.lIndexNumber = cTrs.m_lNumber;
	memcpy(sRec.sLoyalId, sTagRec.sLoyalId, sizeof(sRec.sLoyalId));
	for (int i = 0; i< MAX_GRADES; i++)
	{
		sRec.lDiscountArray[i] = a2l(sTagRec.sDiscountMap[i], STR_LONG_VAL6);
		memcpy(sRec.sPromotionNumber, sTagRec.sPromotionNumber, sizeof(sRec.sPromotionNumber));
	}

	sRec.lFlag = a2l(sTagRec.sFlag, sizeof(sTagRec.sFlag));
	memcpy(sRec.sAttendantID, sTagRec.sAttendantID, sizeof(sRec.sAttendantID));
	memcpy(sRec.sAttendantName, sTagRec.sAttendantName, sizeof(sRec.sAttendantName));
	memcpy(sRec.sFullVehicleId, sTagRec.sFullVehicleId, sizeof(sRec.sFullVehicleId));

	sRec.byCashDeposit1 = (char)a2l(sTagRec.byCashDeposit1, sizeof(sTagRec.byCashDeposit1));
	sRec.byCashDeposit2 = (char)a2l(sTagRec.byCashDeposit2, sizeof(sTagRec.byCashDeposit2));
	sRec.byCashDeposit5 = (char)a2l(sTagRec.byCashDeposit5, sizeof(sTagRec.byCashDeposit5));
	sRec.byCashDeposit10 = (char)a2l(sTagRec.byCashDeposit10, sizeof(sTagRec.byCashDeposit10));
	sRec.byCashDeposit20 = (char)a2l(sTagRec.byCashDeposit20, sizeof(sTagRec.byCashDeposit20));
	sRec.byCashDeposit50 = (char)a2l(sTagRec.byCashDeposit50, sizeof(sTagRec.byCashDeposit50));
	sRec.byCashDeposit100 = (char)a2l(sTagRec.sCashDepositTotal, sizeof(sTagRec.sCashDepositTotal));

	memcpy(sRec.sInvoiceNumber, sTagRec.sInvoiceNumber, sizeof(sRec.sInvoiceNumber));
	memcpy(sRec.sTransactionDateTime, sTagRec.sTransactionDateTime, sizeof(sRec.sTransactionDateTime));
	sRec.lTransactionAmount = a2l(sTagRec.sTransactionAmount, sizeof(sTagRec.sTransactionAmount));
	sRec.lRefundAmount = a2l(sTagRec.sRefundAmount, sizeof(sTagRec.sRefundAmount));

	long lCarWashProg = a2l(sTagRec.sCarWashProgram, sizeof(sTagRec.sCarWashProgram)); //4.0.21.500 - TD 14778
	l2a(lCarWashProg, &sRec.sCarWashProgram, sizeof(sRec.sCarWashProgram));			  //4.0.21.500 - TD 14778

	memcpy(sRec.sWashCode, sTagRec.sWashCode, sizeof(sTagRec.sWashCode));			//4.0.21.501 - TD 70634	
	memcpy(sRec.sWashExpire, sTagRec.sWashExpire, sizeof(sRec.sWashExpire));

	sRec.lCarWashTax = a2l(sTagRec.sCarWashTax, sizeof(sTagRec.sCarWashTax));
	sRec.lCarWashAmount = a2l(sTagRec.sCarWashAmount, sizeof(sTagRec.sCarWashAmount));

	sRec.lServiceType = a2l(sTagRec.sServiceType, sizeof(sTagRec.sServiceType));
	sRec.lSalesTax = a2l(sTagRec.sSalesTax, sizeof(sTagRec.sSalesTax));
	sRec.lDiscount = a2l(sTagRec.sServiceType, sizeof(sTagRec.sServiceType));
	memcpy(sRec.sBarCode, sTagRec.sBarCode, sizeof(sRec.sBarCode));
	sRec.lPumpNumber = a2l(sTagRec.sPumpNumber, sizeof(sTagRec.sPumpNumber));
	sRec.lSeqNumber = a2l(sTagRec.sSeqNumber, sizeof(sTagRec.sSeqNumber));
	sRec.lDevice = a2l(sTagRec.sDevice, sizeof(sTagRec.sDevice));
	memcpy(sRec.sAccountNumber, sTagRec.sAccountNumber, sizeof(sRec.sAccountNumber));
	memcpy(sRec.sAuthNumber, sTagRec.sAuthNumber, sizeof(sRec.sAuthNumber));
	sRec.lAuthAmt = a2l(sTagRec.sAuthAmt, sizeof(sTagRec.sAuthAmt));

	memcpy(sRec.sTrack2Data, sTagRec.sTrack2Data, sizeof(sRec.sTrack2Data));
	memcpy(sRec.sTrack1Data, sTagRec.sTrack1Data, sizeof(sRec.sTrack1Data));
	memcpy(sRec.sCustomerName, sTagRec.sCustomerName, sizeof(sRec.sCustomerName));
	memcpy(sRec.sTotalBalance, sTagRec.sTotalBalance, sizeof(sRec.sTotalBalance));

	sRec.sBalanceType = sTagRec.sBalanceType; //4.0.15.11		
	memcpy(sRec.sHostTransId, sTagRec.sHostTransId, sizeof(sRec.sHostTransId));
	sRec.lControllerSeqNum = a2l(sTagRec.sControllerSeqNum, sizeof(sTagRec.sControllerSeqNum));

	pak(sTagRec.sLAM, sRec.sLAM, sizeof(sTagRec.sLAM));
	sRec.sLAMFlags = sTagRec.sLAMFlags;

	//4.0.9.500
	memcpy(sRec.sReferenceNumber, sTagRec.sReferenceNumber, sizeof(sRec.sReferenceNumber));
	memcpy(sRec.sGRDiscountPerGallon, sTagRec.sGRDiscountPerGallon, sizeof(sRec.sGRDiscountPerGallon));  //4.0.11.310
	sRec.sGRDiscountType = sTagRec.sGRDiscountType;

	//4.0.11.320
	memcpy(sRec.sServiceFee, sTagRec.sServiceFee, sizeof(sRec.sServiceFee));
	sRec.sIgnoreServiceFee = sTagRec.sIgnoreServiceFee;
	sRec.sDryItemsRestriction = sTagRec.sDryItemsRestriction;
	memcpy(sRec.sCustomerID, sTagRec.sCustomerID, sizeof(sRec.sCustomerID));
	memcpy(sRec.sClFlags, sTagRec.sClFlags, sizeof(sRec.sClFlags));
	memcpy(sRec.sManagerID, sTagRec.sManagerID, sizeof(sRec.sManagerID));
	memcpy(sRec.sPMTReferenceNumber, sTagRec.sPMTReferenceNumber, sizeof(sRec.sPMTReferenceNumber));
	sRec.lCashDepositTotal = a2l(sTagRec.sCashDepositTotal, sizeof(sTagRec.sCashDepositTotal));

	//4.0.11.321
	memcpy(sRec.sDiscountType, sTagRec.sDiscountType, sizeof(sRec.sDiscountType));
	sRec.sClearanceID = sTagRec.sClearanceID;
	memcpy(sRec.sCardName, sTagRec.sCardName, sizeof(sRec.sCardName));	
	memcpy(sRec.sOLDSession, sTagRec.sOLDSession, sizeof(sRec.sOLDSession));
	memcpy(sRec.sMediaMasterType, sTagRec.sMediaMasterType, sizeof(sRec.sMediaMasterType));
	memcpy(sRec.sExpire, sTagRec.sExpire, sizeof(sRec.sExpire));
	sRec.sPayInCreate = sTagRec.sPayInCreate;
	sRec.lCarWashDiscount = a2l(sTagRec.sCarWashDiscount, sizeof(sTagRec.sCarWashDiscount));
	sRec.lPreSetlimit = a2l(sTagRec.sPreSetLimit, sizeof(sTagRec.sPreSetLimit));
	sRec.sPreSetLimitType = sTagRec.sPreSetLimitType;

	//4.0.15.10
	memcpy(sRec.sNonDiscountTotalAmount, sTagRec.sNonDiscountTotalAmount, sizeof(sRec.sNonDiscountTotalAmount));
	memcpy(sRec.sOriginalUnitPrice, sTagRec.sOriginalUnitPrice, sizeof(sRec.sOriginalUnitPrice));	//Liat LPE		
	memcpy(&sRec.sDeferralAccepted, &sTagRec.sDeferralAccepted, sizeof(sRec.sDeferralAccepted));		//Liat LPE	//4.0.22.504 69344
	memcpy(&sRec.discountAttrsInfo, &sTagRec.discountAttrsInfo, sizeof(sRec.discountAttrsInfo));					//4.0.18.502 38162
	memcpy(&sRec.sDiscountAttrExt, &sRec.sDiscountAttrExt, sizeof(sRec.sDiscountAttrExt));						//4.0.22.509 38162
	memcpy(sRec.sDPTProductCode, sTagRec.sDPTProductCode, sizeof(sRec.sDPTProductCode));						//4.0.21.500 - TD 14778
	memcpy(sRec.sANPRRefNumber, sTagRec.sANPRRefNumber, sizeof(sRec.sANPRRefNumber));						//4.0.23.500 - TD 69914
	memcpy(&sRec.discountAttrsInfo, &sTagRec.discountAttrsInfo, sizeof(sRec.discountAttrsInfo));  //4.0.23.1211
	memcpy(&sRec.sDiscountAttrExt, &sTagRec.sDiscountAttrExt, sizeof(sTagRec.sDiscountAttrExt)); //4.0.22.509 38162

	memcpy(sRec.sLockingKey, sTagRec.sLockingKey, sizeof(sRec.sLockingKey));

	memcpy(sRec.sStoreIDField, sTagRec.sStoreIDField, sizeof(sRec.sStoreIDField));
	memcpy(sRec.sPosId, sTagRec.sPosId, sizeof(sRec.sPosId));
	memcpy(sRec.sTransId, sTagRec.sTransId, sizeof(sRec.sTransId));
	memcpy(sRec.sLoyaltyStartDateTime, sTagRec.sLoyaltyStartDateTime, sizeof(sRec.sLoyaltyStartDateTime));
	memcpy(sRec.sTicketId, sTagRec.sTicketId, sizeof(sRec.sTicketId));

	//4.0.23.509 97360
	memcpy(sRec.sPreDispensingVolumeHigh, sTagRec.sPreDispensingVolumeHigh, sizeof(sRec.sPreDispensingVolumeHigh));
	memcpy(sRec.sPreDispensingVolumeLow, sTagRec.sPreDispensingVolumeLow, sizeof(sRec.sPreDispensingVolumeLow));
	memcpy(sRec.sPostDispensingVolumeHigh, sTagRec.sPostDispensingVolumeHigh, sizeof(sRec.sPostDispensingVolumeHigh));
	memcpy(sRec.sPostDispensingVolumeLow, sTagRec.sPostDispensingVolumeLow, sizeof(sRec.sPostDispensingVolumeLow));

	memcpy(sRec.sANPRTrsStatus, sTagRec.sANPRTrsStatus, sizeof(sRec.sANPRTrsStatus));	//4.0.26.505 157073
	memcpy(&sRec.cVRNDateTime, &sTagRec.cVRNDateTime, sizeof(sRec.cVRNDateTime));			//4.0.26.505 157073

	memcpy(sRec.sTrsFlowRate, sTagRec.sTrsFlowRate, sizeof(sRec.sTrsFlowRate));
	memcpy(sRec.sAverageFlowRate, sTagRec.sAverageFlowRate, sizeof(sRec.sAverageFlowRate));

	//RFUEL-3309 Get the maximal discount allowed for the grade 
	long lMaxAllowedDiscount = _Module.m_server.GetMaximalDiscountForGrade(cTrs.m_nGrade);
	l2a(lMaxAllowedDiscount, sTagRec.sMaxDiscountAllowed, sizeof(sTagRec.sMaxDiscountAllowed));
	memcpy(sRec.sMaxDiscountAllowed, sTagRec.sMaxDiscountAllowed, sizeof(sRec.sMaxDiscountAllowed));
	sRec.sRatesFromDevice = sTagRec.sRatesFromDevice;
	memcpy(sRec.sClubType, sTagRec.sClubType, sizeof(sTagRec.sClubType));
	memcpy(sRec.sPromotionID, sTagRec.sPromotionID, sizeof(sTagRec.sPromotionID));

	return 0;
}

long	CGCI::m_lRefCount = 0;				//4.0.23.508 TD 90359 
/////////////////////////////////////////////////////////////////////////////
// CGCI

CGCI::CGCI()
{
	if(LOG_BASIC_FLOW_CONTROL)
	{
		_Module.m_server.m_cLogger.LogMsg("GCI Construct Thread Info ");
	}
		
	ConstructThreadInfo();
	m_hThread = NULL;
	m_lRefCount ++;					//4.0.23.508 TD 90359		
}

CGCI::~CGCI()
{
	if (LOG_BASIC_FLOW_CONTROL)
	{
		_Module.m_server.m_cLogger.LogMsg("IGCI - Release, disconnected from server");
	}
}

void GetExceptionPointers(DWORD dwExceptionCode,
	EXCEPTION_POINTERS** ppExceptionPointers)
{
	// The following code was taken from VC++ 8.0 CRT (invarg.c: line 104)

	EXCEPTION_RECORD ExceptionRecord;
	CONTEXT ContextRecord;
	memset(&ContextRecord, 0, sizeof(CONTEXT));

#ifdef _X86_

	__asm {
		mov dword ptr[ContextRecord.Eax], eax
			mov dword ptr[ContextRecord.Ecx], ecx
			mov dword ptr[ContextRecord.Edx], edx
			mov dword ptr[ContextRecord.Ebx], ebx
			mov dword ptr[ContextRecord.Esi], esi
			mov dword ptr[ContextRecord.Edi], edi
			mov word ptr[ContextRecord.SegSs], ss
			mov word ptr[ContextRecord.SegCs], cs
			mov word ptr[ContextRecord.SegDs], ds
			mov word ptr[ContextRecord.SegEs], es
			mov word ptr[ContextRecord.SegFs], fs
			mov word ptr[ContextRecord.SegGs], gs
			pushfd
			pop[ContextRecord.EFlags]
	}

	ContextRecord.ContextFlags = CONTEXT_CONTROL;
#pragma warning(push)
#pragma warning(disable:4311)
	ContextRecord.Eip = (ULONG)_ReturnAddress();
	ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
#pragma warning(pop)
	ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress() - 1);


#elif defined (_IA64_) || defined (_AMD64_)

	/* Need to fill up the Context in IA64 and AMD64. */
	RtlCaptureContext(&ContextRecord);

#else  /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ContextRecord, sizeof(ContextRecord));

#endif  /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ExceptionRecord, sizeof(EXCEPTION_RECORD));

	ExceptionRecord.ExceptionCode = dwExceptionCode;
	ExceptionRecord.ExceptionAddress = _ReturnAddress();

	///

	EXCEPTION_RECORD* pExceptionRecord = new EXCEPTION_RECORD;
	memcpy(pExceptionRecord, &ExceptionRecord, sizeof(EXCEPTION_RECORD));
	CONTEXT* pContextRecord = new CONTEXT;
	memcpy(pContextRecord, &ContextRecord, sizeof(CONTEXT));

	*ppExceptionPointers = new EXCEPTION_POINTERS;
	(*ppExceptionPointers)->ExceptionRecord = pExceptionRecord;
	(*ppExceptionPointers)->ContextRecord = pContextRecord;
}

STDMETHODIMP CGCI::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IGCI,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CGCI::FinalRelease()
{
	if (m_hThread)
	{
		if (m_lRefCount > 1)											//4.0.23.508 TD 90359
		{
			_Module.m_server.m_cLogger.LogMsg("RPC was released convert layer connection point after time out!!!", LOG_LEVEL_1);
		}
		else
		{
			DeleteThreadInfo();

			m_hThread = 0;

			if (LOG_BASIC_FLOW_CONTROL)
			{
				_Module.m_server.m_cLogger.LogMsg("Controller Convert Layer Released - CmdTx Ended");
			}

			PostMessage(_Module.m_hWnd, WM_QUIT, 0, NULL);
		}
	}
	else
	{
		if (LOG_BASIC_FLOW_CONTROL)
		{
			_Module.m_server.m_cLogger.LogMsg("External Convert Layer Released ");
		}
	}

	m_lRefCount--;
}

STDMETHODIMP CGCI::SetPumpStatus(long lPumpNumber, long lStatus, long lRes, long lResExt, long * pVal)
{
//	_LOGMSG.LogLocationIn("GCI::SetPumpStatus()",0);

#ifdef CATCH_GCI
	__try
	{
#endif
		SetPumpStatus__(lPumpNumber,lStatus,lRes,lResExt,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::SetPumpStatus")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetPumpStatus()", LOG_LEVEL_0);

		exit(1);
	}	
#endif

//	_LOGMSG.LogLocationOut("GCI::SetPumpStatus()",0);

	return S_OK;
}

void CGCI::SetPumpStatus__(long lPumpNumber, long lStatus, long lRes, long lResExt, long *pVal, long lFlags, short nGrade, short nPosition, BSTR sData) //4.0.19.500 - TD 59171
{
	GCIRetCode retCode = GCI_OK;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	BOOL bLogUpdate = FALSE;

	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	if(retCode == GCI_OK)
	{
		long			p = lPumpNumber-1;
		BOOL			bChange = FALSE;
		short nPrePayWaiting, nFree, nUnpaid;  //4.0.5.39

		CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[p].GetStat();
		CPumpStatus OldpumpStat = _Module.m_server.m_cPumpArray[p].GetStat(); //4.0.10.503
		

		PumpStatus      lNewStatus	= l2PumpStatus(lStatus);
		PumpReservation lNewRes		= l2PumpReservation(lRes);
		PumpResExt		lNewResExt	= l2PumpResExt(lResExt);		

		PumpStatus      lPrevStatus	= pumpStat.GetStatus();	//4.0.22.501 TD 70956
		PumpReservation lPrevRes	= pumpStat.GetRes();
		PumpResExt      lPrevResExt	= pumpStat.GetResExt();  //4.0.23.2440

		if(lNewResExt == UNKNOWN_RES_EXT)
		{
			lNewResExt = lPrevResExt;
		} //4.0.23.2452

		//4.0.26.506 TD 211718
		BOOL	bAddSetPumpStatusEvent = TRUE;
		long	lEventFlags	= 0;																						//4.0.19.1140 TD 165971
		long	lEventParam1 = 0;																						//4.0.19.1140 TD 165971

		PumpReservation currentReservation;
		if(((lNewRes == GCI_IGNORE_FIELD_VALUE)||(lNewRes == UNKNOWN_RES)))
		{
			currentReservation = OldpumpStat.GetRes();	
		}
		else
		{
			currentReservation = lNewRes;
		}

		SetPumpStatusTerminalSrvFlow(lNewStatus, currentReservation, p, bAddSetPumpStatusEvent, lEventFlags, lEventParam1);

		if (_Module.m_server.IsFuelMobileSrvInUse())
		{
			PumpStatus  currentPumpStatus = ((lNewStatus == GCI_IGNORE_FIELD_VALUE) || (lNewStatus == UNKNOWN_STATUS)) ? OldpumpStat.GetStatus() : lNewStatus;
			PumpReservation currentReservation = ((lNewRes == GCI_IGNORE_FIELD_VALUE) || (lNewRes == UNKNOWN_RES)) ? OldpumpStat.GetRes() : lNewRes;
			PumpResExt currentResExt = ((lNewResExt == GCI_IGNORE_FIELD_VALUE) || (lNewResExt == UNKNOWN_RES_EXT)) ? OldpumpStat.GetResExt() : lNewResExt;


			// do not notify FuelMobile about READY, NO_CHANGE,  NO_CHANGE     // TD 471739
			BOOL bSkipReportEvent = ((lNewStatus == READY)
				&& (lRes == GCI_IGNORE_FIELD_VALUE) && (lResExt == GCI_IGNORE_FIELD_VALUE) && (currentResExt == RES_EXT_MOBILE));


			if (bSkipReportEvent == TRUE)
			{
				CString sMsg;

				sMsg.Format("SetPumpStatus, Skip status [%d], Res [%d], ResExt [%d], currentResExt  [%d].", lNewStatus, lRes, lResExt, currentResExt);
				_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, sMsg);
			}

			if (bSkipReportEvent == FALSE)
				_Module.m_server.m_cGPIEventMgr.AddSetPumpStatusEx(lPumpNumber, currentPumpStatus, currentReservation, currentResExt, 0, 0, nGrade, nPosition);
		}

		//Checking if the pump should be logged in 		//4.0.5.39
		if(_Module.m_server.m_cPumpArray[p].IsPerformLogin(lNewStatus) )
		{
			_Module.m_server.m_cPumpArray[p].SetPumpLoginOp(PUMP_WAIT_TO_BE_LOGIN); 
		}
		else 
		{
			//Checking if the pump should be logged out

			//THIS CODE SEGMENT SHOULD BE UNREMARKED, IT WAS ONLY TEMPORARILY REMARKED
			if(_Module.m_server.m_cPumpArray[p].IsPerformLogout(lNewStatus) )
			{
				_Module.m_server.m_cPumpArray[p].SetPumpLoginOp(PUMP_WAIT_TO_BE_LOGOUT);				
			}
		}

		if(_ALARM_SRV.m_lAlarmSrvActive)		//4.0.6.507
			_Module.m_server.m_cAlarm.PumpStatusAlarm(lPumpNumber,pumpStat.GetStatus(),lNewStatus);

		
		if(lPumpNumber > _Module.m_server.GetLastValidPump())		//4.0.0.79 Start
		{
			if(lNewStatus != UNKNOWN_STATUS)
			{
				_Module.m_server.SetLastValidPump(lPumpNumber);
				CString sMsg;

				sMsg.Format("SetPumpStatus, received status %d from pump %d", lNewStatus, lPumpNumber);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
			}
		}

		AVINextState lNextState = CheckAviConditions(
						&pumpStat,
						lNewRes,				// New Res
						lNewResExt);			// New ResExt

		if(lNextState == AVI_DO_NOT_UPDATE_STATUS)
		{
			bLogUpdate = TRUE;
			retCode = GCI_AVI_CONFLICT_STATUS;
		}
		else if(lNextState == AVI_STOP_PUMP)
		{
			bLogUpdate = TRUE;
			_Module.m_server.SetCommand(CMD_STOP_PUMP, SERVER_POS_NUMBER,lPumpNumber);		
			retCode = GCI_AVI_CONFLICT_STATUS;
		}
		else // AVI_UPDATE_STATUS
		{
			_Module.m_server.m_cPumpArray[p].GetTrsListCount(&nPrePayWaiting,&nFree,&nUnpaid); //4.0.5.39
			// --------------------------------------------------------------
			// UPDATE STATUS
			// --------------------------------------------------------------
			if(lStatus != GCI_IGNORE_FIELD_VALUE)
			{
				if(pumpStat.GetStatus() != lNewStatus)
					bLogUpdate = TRUE;
				
				if(lNewStatus != UNKNOWN_STATUS)
				{

	//////// START : CODE NEED TO BE REMOVED WHEN SITE CONTROLLER SEND CANCEL MESSAGE

					// Detecting "Cancel" of PAK transaction 
					// -----------------------------------------------------------------
						
					CPumpTransact trs;
					BOOL bCancelCurrentTrs = FALSE; 
					BOOL bSetPendingCancel = FALSE;
					BOOL bRemovePendingCancel = FALSE;
					BOOL bFound = FALSE;

					if(!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))
						bFound = TRUE;
					if(!bFound)
						bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_NO_LINK,&trs);

					if((_Module.m_server.GetIFSFTerminalSrvInUse() || (pumpStat.GetResExt() == RES_EXT_MOBILE)) && ((lNewStatus == READY) || 					//4.0.19.504 - 47559
						(lNewStatus == REQUESTING && lNewRes == PAY_AT_KIOSK) || (lNewStatus == STOPPED_BY_CONTROLLER)) && //4.0.22.500 - TD 70351
						(pumpStat.GetStatus() == AUTHORIZED || pumpStat.GetStatus() == STOPED_BY_OPERATOR) && (pumpStat.GetResExt() == RES_EXT_PAP_EXTERNAL || pumpStat.GetResExt() == RES_EXT_MOBILE))
					{
						if (!bFound)
							bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&trs);

						if (bFound && (trs.m_lRoundedValue == 0L))
						{
							bCancelCurrentTrs = TRUE;
							
							if (_Module.m_server.GetDebitZeroCompleteDataFlags())							//4.0.19.508 - TD 54600
							{
								//Create an OLA record with 0 values for VP
								PAY_AT_PUMP_INFO info;
								_Module.m_server.m_cPumpArray[p].GetPAPInfo(&info);
								trs.m_lStatus = PAID;
								long lRtc = _Module.m_server.TrsLinkOperation(REC_INSERT,&trs, &info);

								if (lRtc == OK)
									_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_PAP_TRS,0);
							}
						}
					}
					
					if(bFound) 
					{
						// M.L 13.7.98 Detecting final cancel of pending cancel trs
						if(trs.m_lMisc & MISC_TRS_PENDING_CANCEL)
						{
							if	( 
								((lNewStatus == AUTHORIZED) || (lNewStatus == DISPENSING) || (lNewStatus == REQUESTING))   &&
								( (lNewRes == PAY_AT_KIOSK) || ((lNewRes == UNKNOWN_RES)&&(pumpStat.GetRes() == PAY_AT_KIOSK)) )   
								)
							{
								bRemovePendingCancel = TRUE; // do not cancel.
							}
							else
							{
								bCancelCurrentTrs = TRUE; 
							}
						}

						else if( (pumpStat.GetRes() == PAY_AT_KIOSK)&&
							(lNewStatus == READY) )
						{							
							if(pumpStat.GetStatus() == STOPED_BY_OPERATOR)// M.L 17.6.98
							{
								if(trs.m_lRoundedValue == 0L )
								{
									bCancelCurrentTrs = TRUE;
								}
							}
							else if(pumpStat.GetStatus() == AUTHORIZED)
							{
								bCancelCurrentTrs = TRUE;
							}
							else if(pumpStat.GetStatus() == REQUESTING)
							{
								bSetPendingCancel = TRUE;								
							}
							else if((pumpStat.GetStatus() == DISPENSING) || ((pumpStat.GetStatus() == READY) && (lNewRes == NULL_RES)))	//4.0.24.501 123025 
							{
								if(trs.m_lRoundedValue == 0L )
								{
									bCancelCurrentTrs = TRUE;
								}
							}
						}
						else if( (pumpStat.GetRes() == PAY_AT_KIOSK)&&
							(lNewStatus == REQUESTING) )
						{
							if(pumpStat.GetStatus() == STOPED_BY_OPERATOR)
							{
								if(trs.m_lRoundedValue == 0L )
								{
									bCancelCurrentTrs = TRUE;
									if(LOG_BASIC_FLOW_CONTROL)
									{	
										CString str;
										str.Format("changes from STOPED_BY_OPERATOR to REQUESTING - Auto cancel trs %ld !!!",trs.m_lNumber);
										_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
									}
								}
							}
							else if(pumpStat.GetStatus() == AUTHORIZED)
							{
								bCancelCurrentTrs = TRUE;
								if(LOG_BASIC_FLOW_CONTROL)
								{	
									CString str;
									str.Format("changes from AUTHORIZED to REQUESTING - Auto cancel trs %ld !!!",trs.m_lNumber);
									_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
								}
							}
						}
						// detect reefer remove.
						else if( (pumpStat.GetResExt() == RES_EXT_FLEET)&& 
								  ((pumpStat.GetStatus() == AUTHORIZED)) && //4.0.10.225   || (pumpStat.GetStatus() == STOPED_BY_OPERATOR))&& //4.0.10.91 //4.0.12.503
								 (lNewStatus  == READY ) )  //4.0.10.50 
						{
							CFleetCard  cFleetCard;
							CString str;
							if (cFleetCard.CheckReeferWaiting(lPumpNumber, trs.m_lNumber, 0, 0, 0, 0, 0, 0))	
							{								
								if(LOG_BASIC_FLOW_CONTROL)
								{	
									str.Format("trs %ld reefer cancel detect",trs.m_lNumber);
									_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
								}
							}
							else
							{
								if(LOG_BASIC_FLOW_CONTROL)
								{									
									str.Format("trs %ld reefer cancel detect",trs.m_lNumber);
									_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
								}
							}
						}

						
						if (bSetPendingCancel)
						{
							trs.m_lMisc |= MISC_TRS_PENDING_CANCEL;					
							_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);										
							if(LOG_BASIC_FLOW_CONTROL)
							{	
								CString str;
								str.Format("Trs %d set to pending cancel !",trs.m_lNumber);
								_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
							}
						}

						if (bRemovePendingCancel)
						{
							SaveZeroPakTrs(lPumpNumber, trs);
							trs.m_lMisc &= MISC_NOT_TRS_PENDING_CANCEL;					
							_Module.m_server.CentralUpdateTrs(lPumpNumber, trs.m_lNumber, &trs, FALSE);										
							if(LOG_BASIC_FLOW_CONTROL)
							{	
								CString str;
								str.Format("Trs %d remove pending cancel !", trs.m_lNumber);
								_LOGMSG.LogMsg("GCI","SetPumpStatus", lPumpNumber, LOG_PUMP, str);
							}
						}
						
						if( bCancelCurrentTrs )
						{
							if ((_Module.m_server.m_cPumpArray[p].IsForceZeroPakCompletionOn()) &&
								((_Module.m_server.m_cPumpArray[p].IsLoyaltyClub()) || (_Module.m_server.m_cPumpArray[lPumpNumber - 1].IsPapToPakTrs()))) //4.0.14.1390 67387 //4.0.21.0 //4.0.23.450 123025
							{
								ForceZeroComplete(lPumpNumber);
							}

							long lRtc;
							int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_INTERNAL, lPumpNumber, trs.m_lNumber, &lRtc);
							if(LOG_BASIC_FLOW_CONTROL)
							{	
								CString str;
								if(iRetNum == TRS_CANCEL)
								{
									str.Format("Trs %d canceled by PumpSrv (return %d , lRtc %d)", trs.m_lNumber, iRetNum, lRtc);
									_LOGMSG.LogMsg("GCI", "SetPumpStatus", lPumpNumber, LOG_PUMP, str);
								}
								else
								{
									str.Format("GCI::SetPumpStatus() - Error! PumpSrv could not cancel Trs %d ,reason = %d , lRtc %d)",trs.m_lNumber,iRetNum,lRtc);
									_LOGMSG.LogMsg(str, LOG_LEVEL_0);
								}
							}
						}					
					}
					else
					{//4.0.19.506 Copient
						if((pumpStat.GetStatus() == READY) && (pumpStat.GetRes() == PAY_AT_KIOSK) && (lNewStatus == READY) && 
							(lNewRes == NULL_RES) && (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()) && 
							(_Module.m_server.m_cFuelLoayaltyPS.GetLoyaltySrvInUse() == COPIENT)) //4.0.20.450 66977 4.0.21.501 //4.0.22.504 69344
						{
							CPumpTransact trsUnpaid;
							BOOL bFoundUnpaid = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(UNPAID,TRS_NO_LINK,&trsUnpaid);
							if (!bFoundUnpaid)
								_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber);
						}

						///**************************
						// Convert from PAP to PAK
						///**************************
						CPumpTransact TmpTrs; //4.0.24.140 127928
						if ( ((lNewStatus == READY) || (lNewStatus == REQUESTING)) && (lNewRes == PAY_AT_KIOSK) && (_Module.m_server.m_cPumpArray[lPumpNumber -1].IsPapToPakTrs()) )	//4.0.22.506 67786 //4.0.23.22 77845 //4.0.23.500
						{
							if (_Module.m_server.m_cPumpArray[lPumpNumber -1].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&TmpTrs))
							{
								TmpTrs.m_nLinkFlags = TRS_LINK_PAK_AND_LOYALTY;
								TmpTrs.m_lRes = lNewRes; //4.0.23.21 76383
								TmpTrs.m_lResExt = lNewResExt; //4.0.23.21 76383
								if (_Module.m_server.m_cPumpArray[lPumpNumber -1].IsOLAExtraInfoTrs()) //4.0.24.140 118124
								{
									TmpTrs.m_nLinkFlags |= TRS_LINK_OLA_EXTRA_DATA;
								}

								long lRtc =  _Module.m_server.CentralUpdateTrs(lPumpNumber,TmpTrs.m_lNumber,&TmpTrs,FALSE);
							}
						}
						///*****************************
						// Convert from PAP to Cash Acc.
						///*****************************
						else if ( ((lNewStatus == READY) || (lNewStatus == REQUESTING)) && (lNewRes == PRE_PAY_AT_POS) && (_Module.m_server.m_cPumpArray[lPumpNumber -1].IsPapToPakTrs()) )	//4.0.24.140 127928
						{
							if (_Module.m_server.m_cPumpArray[lPumpNumber -1].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&TmpTrs))
							{
								TmpTrs.m_nLinkFlags = TRS_LINK_PREPAY_NUMBER_AND_LOYALTY;
								TmpTrs.m_lRes = lNewRes; //4.0.23.21 76383
								TmpTrs.m_lResExt = lNewResExt; //4.0.23.21 76383
								if (_Module.m_server.m_cPumpArray[lPumpNumber -1].IsOLAExtraInfoTrs()) //4.0.24.140 118124
								{
									TmpTrs.m_nLinkFlags |= TRS_LINK_OLA_EXTRA_DATA;
								}

								long lRtc =  _Module.m_server.CentralUpdateTrs(lPumpNumber,TmpTrs.m_lNumber,&TmpTrs,FALSE);
							}
						}
					}

					// Detecting "Cancel" of PrePay transaction (done at the Pump Keyboard) 
					// --------------------------------------------------------------------
					if (((lNewStatus == READY) && (pumpStat.GetRes() == PRE_PAY_AT_POS)) || 
						 ((lNewStatus == NOZZLE_LEFT_OUT) && (pumpStat.GetRes() == PRE_PAY_AT_POS))) 	//4.0.3.50					
					{
						CPumpTransact trs;					
						BOOL bTimeOutCurrentTrs = FALSE;
						BOOL bFound = FALSE;

						if( !_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))
							bFound = TRUE;
						if(!bFound)											
							bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS, TRS_LINK_PREPAY_NUMBER, &trs ); 
						if(!bFound)											
							bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_AUTH, TRS_LINK_PREPAY_NUMBER, &trs ); //4.0.23.1202 252311

						if(bFound)
						{
							// M.L 18.6.98
							if(pumpStat.GetStatus() == STOPED_BY_OPERATOR)
							{
								if ( (trs.m_lRoundedValue == 0L ) ||  
									 ((pumpStat.GetResExt() == RES_EXT_FLEET) && trs.m_lRoundedValue))  //4.0.11.502
								{
									bTimeOutCurrentTrs = TRUE;
								}
							}
							else if(pumpStat.GetStatus() == AUTHORIZED)
							{
								bTimeOutCurrentTrs = TRUE;
							}
							else if(pumpStat.GetStatus() == REQUESTING)
							{
								bTimeOutCurrentTrs = TRUE;
							}
							else if(pumpStat.GetStatus() == DISPENSING)   //4.0.5.24
							{
								if(trs.m_lRoundedValue == 0L )
								{
									bTimeOutCurrentTrs = TRUE;
								}
							}
							else if ((pumpStat.GetStatus() == READY) &&
								      (lNewStatus == READY) && (lNewRes == NULL_RES))   //4.0.5.38
							{
								if(trs.m_lRoundedValue == 0L )
								{
									bTimeOutCurrentTrs = TRUE;
								}
							}
							else if(pumpStat.GetStatus() == STOPPED_BY_CONTROLLER) //4.0.8.503
							{
								if(trs.m_lRoundedValue == 0L )	
								{
									bTimeOutCurrentTrs = TRUE;
								}
							}
						}
						
						if(trs.m_nLinkFlags & TRS_LINK_PREPAY_NUMBER)
						{						
							if( bTimeOutCurrentTrs )
							{
								trs.UpdateTime();	
								
								if(trs.m_nLinkFlags & TRS_LINK_CASH_ACCEPTOR_INFO)
								{//4.0.17.830 68360 //4.0.24.280 //4.0.26.505
									EXTRA_INFO_REC	ExtraInfoRec;
									memset(&ExtraInfoRec , 0 , sizeof(EXTRA_INFO_REC));
									long lRtc;
									BOOL bZeroDepositCashAcceptorTrs = FALSE;
									ExtraInfoRec.info.lIndexNumber = trs.m_lNumber;
									lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);

									if (!lRtc)
									{
										if(LOG_BASIC_FLOW_CONTROL)
										{	
											CString str;
											str.Format("Cash acceptor trs %ld was found in extra info file for checking PAID, CashDepositTotal = %ld",trs.m_lNumber, ExtraInfoRec.info.lCashDepositTotal);
											_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
										}
										if (ExtraInfoRec.info.lCashDepositTotal == 0)
											bZeroDepositCashAcceptorTrs = TRUE;
									}
									if (!bZeroDepositCashAcceptorTrs)
									{
										trs.m_lStatus	= PAID;  //4.0.11.509
										if (lPumpNumber)
										{
											_Module.m_server.m_cPumpArray[lPumpNumber - 1].IncreaseUnBalancePrePayCounter();  //4.0.11.118

											if(LOG_BASIC_FLOW_CONTROL) //129893
											{	
												CString str;
												
												str.Format("GCI::SetPumpStatus, trs %ld IncreaseUnBalancePrePayCounter", trs.m_lNumber);
												_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
											}
										}
									}
								}
								else
								{
									trs.m_lStatus	= WAIT_TIME_OUT;
									
									//Store time out in trs history
									trs.m_lMisc |= MISC_PREPAY_TIMED_OUT;

									if (_Module.m_server.GetIFSFTerminalSrvInUse())
									{
										if(!(pumpStat.GetStatus() == DISPENSING && lNewStatus == READY && trs.m_lRoundedValue == 0L)) //4.0.140.40 - to avoid loosing small amount trs
											_Module.m_server.m_cGPIEventMgr.AddCancelTrs(lPumpNumber,
															trs.m_lNumber, lNewStatus, 
															lNewRes, lNewResExt);
									}
								}
							}

							_Module.m_server.TrsLinkOperation(REC_UPDATE, &trs , NULL);
							_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);				
						}					
					}
										
					if ( OLA_BY_CONTROLLER )//2.9.1.5 marik 
					{
						// Detecting "Cancel" of PAP transaction (done at the Pump Keyboard befor pass card) 
						// --------------------------------------------------------------------
						bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&trs);
								
						if(bFound) 
						{
							if (TEMPORARY_DEBUG)
							{	
								if(LOG_BASIC_FLOW_CONTROL)
								{	
									CString str;
									str.Format("Found pap trs in  mode  WAIT_PROGRESS trs %ld in list!!!",trs.m_lNumber);
									_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
								}
							}

							/////////////////////////////////////////////////////////////////////////////////////////////////
							
							if((pumpStat.GetRes() == PAY_AT_PUMP)&&
								((pumpStat.GetStatus() == READY) || 
								 (pumpStat.GetStatus() == STOPED_BY_OPERATOR) ||
								 (pumpStat.GetStatus() == AUTHORIZED) ||			//4.0.12.515
								 (pumpStat.GetStatus() == REQUESTING) ||			//4.0.19.502
								 (pumpStat.GetStatus() == NOZZLE_LEFT_OUT)	))		//4.0.23.511 TD 97680
							{
								if((lNewStatus == READY) && (lNewRes == NULL_RES))
								{
									bCancelCurrentTrs = TRUE;
								}
								else if((lNewStatus == REQUESTING) && (lNewRes == PAY_AT_KIOSK))
								{
									bCancelCurrentTrs = TRUE;								
								}
								else if((lNewStatus == READY) && (lNewRes == PAY_AT_KIOSK))
								{
									bCancelCurrentTrs = TRUE;								
								}
								else if((lNewStatus == STOPED_BY_OPERATOR) && (pumpStat.GetRes() == PAY_AT_PUMP))  //4.0.9.500
								{
									bCancelCurrentTrs = TRUE;
								}


								if(LOG_BASIC_FLOW_CONTROL)	//4.0.21.20 TD 69204
								{	
									CString str;
									str.Format("Trs=%ld,bCancelCurrentTrs=%d, OLAByController=%d",trs.m_lNumber ,bCancelCurrentTrs, _Module.m_server.OLAByController());
									_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
								}
								
								
								if (bCancelCurrentTrs && (_Module.m_server.OLAByController() == 2))		//4.0.21.20 TD 69204
								{
									PAY_AT_PUMP_INFO Info; 
									memset(&Info,' ',sizeof(Info));
									_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&Info);					
									
									
									// Transaction should be clear on PAPReadyTransaction
									if(Info.CardSaleInfo.cardData.sTranStatus == CD_APPROVE_OK)
									{
										bCancelCurrentTrs = FALSE;
										
										if(LOG_BASIC_FLOW_CONTROL)
										{	
											CString str;
											str.Format("Trs %ld  skip on clean Transaction, transaction should closed on the afterward ",trs.m_lNumber);
											_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
										}
									}
								}
							
								if(LOG_BASIC_FLOW_CONTROL)
								{	
									CString str;
									str.Format("changes from Stack PAP to Idle  trs %ld !!!",trs.m_lNumber);
									_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
								}

								if( bCancelCurrentTrs )
								{
									long lRtc;									 
									int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_INTERNAL,lPumpNumber, trs.m_lNumber, &lRtc);

									if(LOG_BASIC_FLOW_CONTROL)
									{											
										CString str;
										if(iRetNum == TRS_CANCEL)
										{
											str.Format("Trs %d canceled by PumpSrv (return %d , lRtc %d)",trs.m_lNumber,iRetNum,lRtc);
											_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
										
										}
										else
										{
											str.Format("GCI::SetPumpStatus() - Error! PumpSrv could not cancel Trs %d ,reason = %d , lRtc %d)",trs.m_lNumber,iRetNum,lRtc);
											_LOGMSG.LogMsg(str,LOG_LEVEL_0);
										}
									}
								}
							}
							/////////////////////////////////////////////////////////////////////////////////////////////////
						}
					}


	//////// END : CODE NEED0000 TO BE REMOVED WHEN ALLIED SEND CANCEL MESSAGE
			//////////////////////////////////////////////////
					
					if ((lNewStatus == AUTHORIZED) && (pumpStat.GetStatus() == DISPENSING))// 2.9.3.4 
					{
						CPumpTransact trs;

						if(	!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))
						{	// found 						
							if((pumpStat.GetRes() == PRE_PAY_AT_POS)  && (trs.m_lRoundedValue == 0L))
							{
								trs.SetStatus(WAIT_PROGRESS); 
								trs.UpdateTime();
								_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);																																									
							}
						}
					}
					
					if (((lNewStatus == REQUESTING) && (lNewRes == PAY_AT_KIOSK) && (nPosition > 0) && (nGrade > 0)) || (lNewStatus == DISPENSING)) //4.0.19.500 - TD 59171 //4.0.26.501 - TD 108311
					{
						pumpStat.SetPosition(nPosition);
						pumpStat.SetGrade(nGrade);

						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;
							str.Format("Set Grade = %ld and Position = %ld", nGrade, nPosition);
							_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
						}
					}

					//////////////////////////////////////////////////////////////////////////
					// Start 4.0.23.1800
					pumpStat.RemoveMiscExt(MISC_EXT_PAP_REQUEST_NOTIFICATION);

					if ((lNewStatus == REQUESTING) && (lNewRes == PAY_AT_PUMP)) 
						pumpStat.AddMiscExt(MISC_EXT_PAP_REQUEST_NOTIFICATION);
					// End  4.0.23.1800
					//////////////////////////////////////////////////////////////////////////
					
					PumpStatus lLastStatus = pumpStat.GetStatus(); //4.0.23.502 78545
					pumpStat.SetStatus(lNewStatus);
					bChange = TRUE;
					
					if((pumpStat.GetMiscExt() & MISC_EXT_AUTHORIZE_PUMP_SENT) && lNewStatus != REQUESTING)					//4.0.30.500 - TD 320068
						SetMiscExtAuthorizeSentIndication(lPumpNumber, &pumpStat, FALSE);

					if (((lNewRes == NULL_RES) && (lNewStatus == READY)) || (lNewStatus == AUTHORIZED)) 					//4.0.1.37 //3.2.1.74
					{
						long lMisc = 0; 
						BOOL bUpdateMisc = FALSE;

						if (pumpStat.GetGrade())						//4.0.19.500 - TD 59171
							pumpStat.SetGrade(0);
						if (pumpStat.GetPosition())
							pumpStat.SetPosition(0);
					
						lMisc = pumpStat.GetMisc();
						if (lMisc & MISC_CREDIT_CARD_NOT_AUTHORIZED)
						{

							if (_Module.m_server.GetPAPDeclineTimeValue() == 0)
								lMisc &= MISC_NOT_CREDIT_CARD_NOT_AUTHORIZED;
							
							if(lMisc & MISC_AVI_NOT_AUTHORIZED)							//4.0.2.13
								lMisc &= MISC_NOT_AVI_NOT_AUTHORIZED;
							
							bUpdateMisc = TRUE;										
						}

						if (lMisc & MISC_CREDIT_CARD_DECLINE)						//4.0.13.73
						{							
							if(lMisc & MISC_AVI_NOT_AUTHORIZED)//4.0.2.13
								lMisc &= MISC_NOT_AVI_NOT_AUTHORIZED;
							
							bUpdateMisc = TRUE;										
						}
						
						if(lMisc & MISC_PAK_TO_PAP_FAILED)//4.0.3.45
						{
							lMisc &= MISC_NOT_PAK_TO_PAP_FAILED;
							bUpdateMisc = TRUE;
						}
						if (lMisc & MISC_NEED_MORE_PARAM)
						{
							lMisc &= MISC_NOT_NEED_MORE_PARAM;
							bUpdateMisc = TRUE;
						}
						if (lMisc & MISC_AVI_NOT_AUTHORIZED) //4.0.25.501 TD 132711
						{
							if (_Module.m_server.GetPAPDeclineTimeValue() == 0)
							{
								lMisc &= MISC_NOT_AVI_NOT_AUTHORIZED;
								bUpdateMisc = TRUE;
							}							
						}

						//4.0.17.1230 //4.0.24.71 116500
						if (_Module.m_server.OLAByController() == 2)
						{
							_Module.m_server.m_cCommonFunction.ResetMiscCardNotAuthorized(lMisc);

							if (lMisc & MISC_CREDIT_CARD_DECLINE)
							{
								lMisc &= MISC_NOT_CREDIT_CARD_DECLINE;
							}
							
							bUpdateMisc = TRUE;
						}
						
						if(bUpdateMisc)
						{
							pumpStat.SetMisc(lMisc);
							_Module.m_server.m_cPumpArray[lPumpNumber -1].SetStat(pumpStat);
						}
					}
					///////////////////////////////////////////////////////////
					
					if(lNewStatus == DISPENSING)					
					{
						CPumpTransact trs;
						long lTrsNumber = 0;										
						BOOL bFound = FALSE;
						
/*						CString sTmpStr;
						sTmpStr.Format("##DEBUG In dispensing");
						_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,sTmpStr);*/

						if(	_Module.m_server.m_cPumpArray[p].GetCurrentTrsNumber(&lTrsNumber))
						{	// not found 						
							if(pumpStat.GetRes() == PRE_PAY_AT_POS)
							{
								bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs);
								if (!bFound)
								{//4.0.23.508 86952 Special case where canceling prepay on timeout and lifting nozzle occur at the same time ==> ignore the cancel and continue dispensing.
									bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_TIME_OUT,TRS_LINK_PREPAY_NUMBER,&trs);
									_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "SetPumpStatus dispensing was sent for a pre pay trs which is not in wait_progress status.",LOG_LEVEL_1);
								}
							}
							else if(pumpStat.GetRes() == PAY_AT_PUMP)							
								bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&trs);
							//4.0.1.34
							else if ((pumpStat.GetRes() == PAY_AT_KIOSK) && (pumpStat.GetResExt() == RES_EXT_ATTENDANT))							
								bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_ATTEND,&trs);
							else 
								bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_NO_LINK,&trs);
						
/*							sTmpStr.Format("##DEBUG In progress trs wasn't found, res = %ld, bFound = %ld, link = %ld", pumpStat.GetRes(), bFound, trs.m_nLinkFlags);
							_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,sTmpStr);*/
							if (bFound)
							{
								trs.SetStatus(IN_PROGRESS); 
								trs.UpdateTime();
								//update start fueling time only once at the beginning of fueling
								trs.UpdateStartFuelingTime();	//4.0.22.511 72606
								trs.m_nNozzle = nPosition;		//4.0.26.503 TD 108311

								_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);																											
							}
						}
						else	//4.0.23.502 78545
						{
							if(	!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs)) //Found in_progress trs
							{
								if (lLastStatus == STOPED_BY_OPERATOR)	//changing status from stopped by operator to dispensing again
								{
									CString sTmpStr;
									sTmpStr.Format("Resuming pump after stopped by operator");
									_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,sTmpStr);
									trs.UpdateTime();
									_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);																											
								}
							}
						}
					}
					else if ((lNewStatus == REQUESTING) || (lNewResExt == RES_EXT_AVI_CANCELED)) //The new mechanism for decline AVI has no requesting //4.0.25.30 132711
					{
						CPumpTransact trs;
						//Find the current transaction
						if(	!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))
						{	//found
							if((pumpStat.GetRes() == PRE_PAY_AT_POS)  && (trs.m_lRoundedValue == 0L))
							{
								trs.SetStatus(WAIT_TIME_OUT);

								//Store time out in trs history
								trs.m_lMisc |= MISC_PREPAY_TIMED_OUT;

								_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);
							}
						}

						//4.0.2.13  Added check to ensure that POSPUMP
						//will continue to display declined AVI icon until
						//pump is actually authorized.
						else if((pumpStat.GetResExt() == RES_EXT_AVI) && 							    
								(pumpStat.GetRes() == PAY_AT_PUMP))
						{
							//POSPUMP looks for this reservation extension
							//in order to display declined AVI icon
							long lTmpMisc = pumpStat.GetMisc();
							
							if((lTmpMisc & MISC_CREDIT_CARD_NOT_AUTHORIZED) || (lTmpMisc & MISC_CREDIT_CARD_DECLINE)) //4.0.13.73
							{
								lTmpMisc |= MISC_AVI_NOT_AUTHORIZED;
								pumpStat.SetMisc(lTmpMisc);

								CString sTmpStr;
								sTmpStr.Format("AVI not authorized");
								_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,sTmpStr);
							}
						}
					}
					else if (lNewStatus == AUTHORIZED) // 4.0.9.509
					{
						if(pumpStat.GetRes() == PRE_PAY_AT_POS)
						{
							if(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_CASH_ACCEPTOR_INFO,&trs))  //4.0.10.16
							{
								CCashAcceptor cCashAcceptor; 	  //4.0.10.20
								cCashAcceptor.GenerateCashAcceptorPayIn(p+1,trs);
							}							
						}				
					}
				}
				else			
				{
					retCode = GCI_UNKNOWN_PUMP_STATUS;					
				}
			}

			// --------------------------------------------------------------
			// UPDATE RESERVATION
			// --------------------------------------------------------------
			if(lRes != GCI_IGNORE_FIELD_VALUE)
			{				
				if(_Module.m_server.m_cSinglePumpLock.IsExclusivelyLocked(lPumpNumber))// 4.0.20.500 - TD 65255
				{
					if (lNewRes == NULL_RES && lNewResExt == NULL_RES_EXT)
						_Module.m_server.m_cSinglePumpLock.SetPumpStateWaitUnlocked(lPumpNumber);
				}	

				//Calling m_server.m_cSinglePumpLock's which changes the state in case this function was
				//reached due to locking/unlocking a single pump exclusively
				_Module.m_server.m_cSinglePumpLock.SetSinglePumpState(lPumpNumber, lStatus, lRes, lResExt);		

				if(pumpStat.GetRes() != lNewRes)
					bLogUpdate = TRUE;

				if(lNewRes != UNKNOWN_RES)
				{					
					// New Reservation is PUMPSRV
					//=============================================================
					if(lNewRes == PUMPSRV)
					{					
						SetPumpStatusNewRerservaionIsPumpSrv(lNewResExt,lPumpNumber, lNewRes, pumpStat, bChange);

					}
					// New Reservation is PRE_PAY_AT_POS
					//=============================================================
					else if(lNewRes == PRE_PAY_AT_POS)
					{
						SetPumpStatusNewRerservaionIsPrePayAtPos(nPrePayWaiting, pumpStat, lNewRes, lNewResExt, bChange, lNewStatus, lPumpNumber, retCode);
					}

					//=============================================================
					// New Reservation is NULL_RES					
					//=============================================================					
					else if((lNewRes == NULL_RES)&&(pumpStat.GetRes() == PAY_AT_KIOSK) && ((pumpStat.GetRes() != PAY_AT_KIOSK)  && (pumpStat.GetStatus() != NOZZLE_LEFT_OUT))) //4.0.3.50
					{ 
						SetPumpStatusNewRerservaionIsNullRes(lPrevResExt, lPumpNumber, lNewRes, pumpStat, lNewResExt, bChange);	
					}
					else if (lNewResExt == RES_EXT_ATTENDANT)  
					{
						SetPumpStatusNewRerservaionIsExtAttendant(lPumpNumber, lNewStatus, lNewRes, retCode, pumpStat,nUnpaid, lNewResExt, bChange);

					}		
					//=============================================================
					// New Reservation is PAY_AT_KIOSK or TEST and list in not full
					// New Reservation is PAY_AT_PUMP or PAY_ATTEND
					//=============================================================
					else //  ATTEND, PAY_AT_PUMP, PAY_AT_OPT
					{
						SetPumpStatusNewReservaionIsPayAtPumpOrPayAttendant(lNewRes, lNewStatus, pumpStat, lNewResExt,lPumpNumber, bChange, lPrevResExt);
					}
				}
				else
				{
					retCode = GCI_UNKNOWN_PUMP_RESERVATION;
				}
			}

			if ((lNewStatus == REQUESTING) || ((lNewRes == NULL_RES) && (lNewStatus == READY)) ||
				((lRes == GCI_IGNORE_FIELD_VALUE) && (OldpumpStat.GetRes() == NULL_RES) && (lNewStatus == READY)) || 
				((lNewRes == NULL_RES) && (lStatus == GCI_IGNORE_FIELD_VALUE) && (OldpumpStat.GetRes() == READY)))
			{				
				CheckIfPumpNeedToBeLock(p+1);
			}
			
			if(lResExt)
			{			
				if ( lNewResExt == RES_EXT_PAP_CANCELED )					//4.0.0.48
				{
					if (pumpStat.GetResExt() == RES_EXT_RFID_CT)
						pumpStat.SetRes(lNewRes ,RES_EXT_RFID_CT_CANCELED);
					else if (pumpStat.GetResExt() == RES_EXT_RFID_HT)     //4.0.0.78
						pumpStat.SetRes(lNewRes ,RES_EXT_RFID_HT_CANCELED);
					
					pumpStat.SetMisc(MISC_CURRENT_TRS_PAP_CANCELED);  //4.0.1.2

					CString str;
					str.Format("PAP decline .......)");
					_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);

					bChange = TRUE;
				}
				else if (lNewResExt == RES_EXT_FLEET)  //4.0.10.220
				{
					pumpStat.SetResExt(RES_EXT_FLEET);
					bChange = TRUE;
				}
			}
		}

		if(bChange && (retCode == GCI_OK))  // 3.2.0.76
		{
			if (lNewResExt == RES_EXT_AVI_CANCELED)
				pumpStat.SetRes(PAY_AT_PUMP,RES_EXT_AVI);

			PumpReservation eTmpRes = _Module.m_server.m_cPumpArray[p].GetStat().GetRes();	//4.0.22.504 save the Reservation before changing to the new one.73878
			_Module.m_server.m_cPumpArray[p].SetStat(pumpStat);
			

			if (((lNewRes == NULL_RES) && (lNewStatus == READY)) ||	
				((lNewRes == NULL_RES) && (pumpStat.GetStatus() == READY))) //4.0.5.39
			{

				BOOL bFoundPAK = FALSE;
				CPumpTransact TmpTrs;

				if(  //4.0.12.503
					( 
					 (OldpumpStat.GetResExt() == RES_EXT_FLEET) && 
					  ((OldpumpStat.GetStatus() == DISPENSING) || 
					  (OldpumpStat.GetStatus() == STOPED_BY_OPERATOR))) && 
					  (lNewStatus  == READY ) )  //4.0.10.50 
				{
					CFleetCard  cFleetCard;
					CString str;
					BOOL bFound= FALSE; 
					CPumpTransact trsFleet;

					if(!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trsFleet))
						bFound = TRUE;
					if(!bFound)
						bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_NO_LINK,&trsFleet);

										
					if(!bFound)  //4.0.15.501
						_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_TIME_OUT,TRS_LINK_PREPAY_NUMBER,&trsFleet);

					if (cFleetCard.CheckReeferWaiting(lPumpNumber, trsFleet.m_lNumber, 0, 0, 0, 0, 0, 0))	
					{
						
						if(LOG_BASIC_FLOW_CONTROL)
						{	
							str.Format("Pump %02ld, trs %ld reefer cancel detect after stop/dispensing",lPumpNumber,trsFleet.m_lNumber);
							_LOGMSG.LogMsg(str);
						}

					}
					else
					{
						if(LOG_BASIC_FLOW_CONTROL)
						{	
							
							str.Format("Pump %02ld, trs %ld reefer cancel detect after stop/dispensing fail !!",lPumpNumber,trsFleet.m_lNumber);
							_LOGMSG.LogMsg(str);
						}

					}

				}
				
				//4.0.29.0 - TD 256419
				// When pump is READY / NULL_RES and maintenance not in process, in case pump is LOCKED,
				// the state will be changed to UNLOCK.
				if ((_Module.m_server.m_cPumpArray[p].GetLockStatus() == LOCKED) &&
					(_Module.m_server.m_cServerInfo.GetServerStatus() == UNLOCK) &&
					(!_Module.m_server.m_cServerInfo.GetRequiredMaintenanceProcessFlags()))
				{
					_Module.m_server.m_cPumpArray[p].SetLockStatus(UNLOCK);
					
					if(LOG_BASIC_FLOW_CONTROL)
					{
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Change pump lock state from LOCKED to UNLOCKED!");
					}
				}

				if (_Module.m_server.IsNeedToClearTrsOnIdlePump())				//3.2.1.63
				{
					if(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_NO_LINK,&TmpTrs))
					{
						bFoundPAK = TRUE;
					}
					else if(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&TmpTrs))
					{
						if ((_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()) && (TmpTrs.m_lResExt == RES_EXT_PAK_LOYALTY))//4.0.30.40 310103
						{
							bFoundPAK = TRUE;
						}
						
					}  //4.0.1.34
					else if(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_ATTEND,&TmpTrs))
					{
						if (TmpTrs.m_lResExt == RES_EXT_ATTENDANT)
						{
							bFoundPAK = TRUE;
						}
					}
					else if(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS,TRS_NO_LINK,&TmpTrs))  //4.0.10.504
					{
						if ((TmpTrs.m_lRoundedValue == 0) || 
							(lResExt == RES_EXT_PAK_CANCELED)) //4.0.20.790 TD 76441 - the pak should be canceled in this case.
						{
							if(lResExt == RES_EXT_PAK_CANCELED)
							{
								//Mark trs as RES_EXT_PAK_CANCELED so it will be canceled later.
								pumpStat.SetResExt(RES_EXT_PAK_CANCELED);
								_Module.m_server.m_cPumpArray[p].SetStat(pumpStat);	
							}
							bFoundPAK = TRUE;						
						}						
					}
					else if (_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_PAK_AND_LOYALTY,&TmpTrs))  //marikDebug //4.0.24.60 109168
					{
						if (!_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())
						{
							bFoundPAK = TRUE;
						}

					}
					else if (_Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS, TRS_LINK_ATTEND, &TmpTrs))  //TD 478432
					{
						if (TmpTrs.m_lRoundedValue == 0)
							bFoundPAK = TRUE;
					}



					if (bFoundPAK)
					{
						long lRtc;
						int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_INTERNAL,lPumpNumber, TmpTrs.m_lNumber, &lRtc);
						if(LOG_BASIC_FLOW_CONTROL)
						{	
							CString str;
							if(iRetNum == TRS_CANCEL)
							{
								str.Format("Trs %d canceled by PumpSrv (return %d , lRtc %d)",TmpTrs.m_lNumber,iRetNum,lRtc);
								_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
							
							}
							else
							{
								str.Format("GCI::SetPumpStatus() - Error! PumpSrv could not cancel Trs %d ,reason = %d , lRtc %d)",TmpTrs.m_lNumber,iRetNum,lRtc);
								_LOGMSG.LogMsg(str,LOG_LEVEL_0);
							}
						}
					}
					//4.0.17.660 62457 start //4.0.24.280
					BOOL bFoundCashAcceptorPrePay = FALSE;
					EXTRA_INFO_REC	ExtraInfoRec;
					memset(&ExtraInfoRec , 0 , sizeof(EXTRA_INFO_REC));
					long lRtc;		

					if(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_CASH_ACCEPTOR_INFO,&TmpTrs)) 
					{//4.0.17.830 68360

						if(LOG_BASIC_FLOW_CONTROL)
						{	
							CString str;
							str.Format("Cash acceptor trs %d was found in status %ld.. ",TmpTrs.m_lStatus);
							_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
						}
						ExtraInfoRec.info.lIndexNumber = TmpTrs.m_lNumber;
						lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);

						if (!lRtc)
						{
							if(LOG_BASIC_FLOW_CONTROL)
							{	
								CString str;
								str.Format("Cash acceptor trs %d was found in extra info file",TmpTrs.m_lNumber);
								_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
							}
							if (ExtraInfoRec.info.lCashDepositTotal == 0)
								bFoundCashAcceptorPrePay = TRUE;
						}
					}

					if (bFoundCashAcceptorPrePay)
					{
						long lRtc;
						int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_INTERNAL,lPumpNumber, TmpTrs.m_lNumber, &lRtc);
						if(LOG_BASIC_FLOW_CONTROL)
						{	
							CString str;
							str.Format("Cash acceptor trs %d was canceled by pumpsrv.. (return %d)",TmpTrs.m_lNumber,iRetNum);
							_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
						}
					}
					//4.0.17.660 62457 end
					
				}

				if(LOG_BASIC_FLOW_CONTROL)
				{	
					CString str;
					str.Format("Force save status %02ld",lStatus);
					_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
				}
	
				long lMisc = pumpStat.GetMisc();				//4.0.5.49

				if (lMisc & MISC_VIT_CONFLICT)
				{
					lMisc &=MISC_NOT_VIT_CONFLICT;					
					
					pumpStat.SetMisc(lMisc);
					_Module.m_server.m_cPumpArray[lPumpNumber -1].SetStat(pumpStat);			
				}		
				
				if(!_Module.m_server.m_cPumpArray[lPumpNumber - 1].IsForceZeroPakCompletionOn())
				{
					_Module.m_server.ClearPAPInfo(lPumpNumber);								//4.0.2.15
					if (eTmpRes == PAY_AT_KIOSK)	//4.0.20.180 67388 //4.0.21.0	//4.0.22.504 73878
					{
						_Module.m_server.ClearLoyaltyInfo(lPumpNumber);
						_Module.m_server.m_cFuelLoayaltyPS.InitSavedLoyaltyInfo(lPumpNumber); //RFUEL-2378 clear all former promotions to avoid redundancy
					}

				} // TD 256101	
			}
						
			if(PUMPSRV_IS_MODE_MASTER)
			{				
				if((pumpStat.GetStatus() == REQUESTING)	 && (lNewResExt != RES_EXT_AVI_CANCELED)) //4.0.5.30					 
				{
					if (FALSE == _Module.m_server.DBConnectionAvailable())
					{
						retCode = GCI_FAILED_IO;
					}
					else if ((pumpStat.GetRes() == PAY_AT_KIOSK) && (pumpStat.GetResExt() == NULL_RES_EXT) &&   //4.0.5.39
						(nUnpaid >= MAX_ALLOWED_UNPAID_TRS(p)) && MODE_VALID(pumpStat.GetMode()) && (_Module.m_server.m_cModeArray[pumpStat.GetMode() - 1].AutoAuthorize(PAY_AT_KIOSK, NULL_RES_EXT))) //TD 76535 - 4.0.22.511 
						//This protection is relevant Only if PAK is AutoAuth ! 
					{
						//4.0.9.510 
						if (!_Module.m_server.UpdatePAKReconcileTrans(lPumpNumber)) 
							retCode = GCI_PUMP_TRS_LIST_IS_FULL; //4.0.20.0 - TD 62219
					}						
					
					if ((retCode == GCI_OK) && IsAutoAuthorize(lPumpNumber, OldpumpStat, lStatus, lRes, lResExt, nPosition)) //TD 268644, 236976, 269686 (merge from 1025)
					{															
						SetPumpStatusAutoAuthoirze(pumpStat, lPumpNumber);

					}
				}
			}

			if(_Module.m_server.m_cSinglePumpLock.IsExclusivelyLocked(lPumpNumber)) //In case the pump is exclusively	 locked we want to set its ResExt to RES_EXT_PUMP_SINGLE_LOCK
			{
				pumpStat.SetRes(pumpStat.GetRes(), RES_EXT_PUMP_SINGLE_LOCK); 
				_Module.m_server.m_cPumpArray[lPumpNumber -1].SetStat(pumpStat);			
			}

			if(lNewStatus == AUTHORIZED)
			{
				_Module.m_server.m_cCommonFunction.ClearDeclinedMiscs(lPumpNumber, "SetPumpStatus(Authorized)");
			}			

			//saving new status for backup. 4.0.0.36
			_Module.m_server.SavePumpInfo(p+1);
		}
				
		_Module.m_server.m_cFuelingRestriction.Restrict(lPumpNumber, lNewRes, lNewStatus, lNewResExt, lPrevRes, lPrevStatus); //4.0.22.501 TD 70956

		//4.0.26.508 TD 211718
		if (_Module.m_server.GetIFSFTerminalSrvInUse() && bAddSetPumpStatusEvent == TRUE)
		{
			CPumpStatus cCurrentPumpStatus = _Module.m_server.m_cPumpArray[p].GetStat(); 

			if ((OldpumpStat.GetStatus() != cCurrentPumpStatus.GetStatus()) || (OldpumpStat.GetRes() != cCurrentPumpStatus.GetRes()) || 
				(OldpumpStat.GetResExt() != cCurrentPumpStatus.GetResExt())	)			
			{
				PumpStatus		currentPumpStatus  = ((lNewStatus == GCI_IGNORE_FIELD_VALUE)||(lNewStatus == UNKNOWN_STATUS))? OldpumpStat.GetStatus() : lNewStatus;
				
				_Module.m_server.m_cGPIEventMgr.AddSetPumpStatus(lPumpNumber, currentPumpStatus, currentReservation, lNewResExt, lEventFlags, lEventParam1);
			}
		}
	}

	if(retCode)
	{
		if(bLogUpdate)
			if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"IGCI",
					"SetPumpStatus",
					TRUE,retCode,
					FALSE,0);
	}

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		_LOGMSG.LogPumpStatus(lPumpNumber,lStatus,lRes,lResExt,retCode);
	}


	if(pVal != NULL)
		*pVal = retCode;
}

// This function write warning/error information 
// to the pump array and also write the same information 
// to a local log file
STDMETHODIMP CGCI::WriteToLog(long lPumpNumber, BOOL bErrorRep, long lError, BOOL bWarningRep, long lWarning, BOOL bDeviceRep, long lDevice, long lDeviceCode, BOOL bMiscRep, long lMisc, long * pVal)
{

#ifdef CATCH_GCI
	__try
	{
#endif
		WriteToLog__(lPumpNumber,bErrorRep,lError,bWarningRep,lWarning,bDeviceRep,lDevice,lDeviceCode,bMiscRep,lMisc,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::WriteToLog")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","WriteToLog()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::WriteToLog__( long lPumpNumber,  BOOL bErrorRep,  long lError,  BOOL bWarningRep , long lWarning,  BOOL bDeviceRep,  long lDevice,  long lDeviceCode,  BOOL bMiscRep,  long lMisc, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	if(lPumpNumber)
		if(PUMP_NOT_VALID(lPumpNumber)) 
			retCode = GCI_INVALID_PUMP_NUMBER;

	if(retCode == GCI_OK)
	{
		_Module.m_server.WriteToLogMsg(lPumpNumber, bErrorRep, lError, bWarningRep, lWarning, bDeviceRep, lDevice, lDeviceCode, bMiscRep, lMisc, pVal);
		retCode=(GCIRetCode) *pVal;


	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"WriteToLog",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::InitInterface(BOOL bControllerIsMaster,BOOL bControllerGenTrsNumber, BSTR strControllerName, long lControllerVer, long * pVal)
{
//	_LOGMSG.LogLocationIn("GCI::InitInterface()",0);

	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif
		InitInterface__(bControllerIsMaster,bControllerGenTrsNumber,strControllerName,lControllerVer,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::InitInterface")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::InitInterface()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::InitInterface()",0);
	return S_OK;
}

void CGCI::InitInterface__( BOOL bControllerIsMaster, BOOL bControllerGenTrsNumber,  BSTR strControllerName,  long lControllerVer, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	// Defining Fuel transactions Master 

	CString strConvertLayerName,strConvertLayerVersion;
	CString strTemp = strControllerName;
	int iVerLen = a2i((unsigned char *)(LPCTSTR)strTemp.Left(2), 2);

	if((iVerLen>7) && (iVerLen <30))
	{
		strTemp = strTemp.Right(strTemp.GetLength()-2);
		strConvertLayerVersion = strTemp.Left(iVerLen);
		strConvertLayerName = strTemp.Right(strTemp.GetLength() - iVerLen);		
	}
	else 
	{	
		strConvertLayerName = strTemp;
	}

	_Module.m_server.m_strControllerName = strConvertLayerName;
	_Module.m_server.m_strConvertLayerVersion = strConvertLayerVersion;


	if(bControllerIsMaster)
	{
		_Module.m_server.SetOperatingMode(SLAVE);
		_Module.m_server.m_bControllerGenSeqNumber = TRUE;
		_Module.m_server.m_bControllerManagePumpMode = TRUE;
		_Module.m_server.m_bControllerManagePumpLock = TRUE;
	}
	else
	{
		_Module.m_server.SetOperatingMode(MASTER);
		_Module.m_server.m_bControllerGenSeqNumber = FALSE;
		_Module.m_server.m_bControllerManagePumpMode = FALSE;
		_Module.m_server.m_bControllerManagePumpLock = FALSE;
	}

	_Module.m_server.m_bControllerGenTrsNumber = bControllerGenTrsNumber;
	_Module.m_server.m_bControllerSupportAVI = FALSE ; 

	if(_Module.m_server.GetServerState() == SERVER_NO_CONVERT)
	{		
		_Module.m_server.SetServerState(SERVER_STARTUP); 
		m_CmdTxThreadInfo.m_pGCI = this;
		m_CmdTxThreadInfo.m_pServer = &_Module.m_server;

		m_hThread = (HANDLE) _beginthreadex(	NULL,
									0,
									CmdTx,
									&m_CmdTxThreadInfo,
									0,
									&m_usThreadId );

		StartCmdTxThread();

		if(LOG_BASIC_FLOW_CONTROL)
		{
			_LOGMSG.LogMsg("GCI Thread CmdTx - Started ! ");
		}
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		if(bControllerIsMaster)
			str.Format("SiteController - %s ,Version %ld ,Convert layer version %s, (Controller is Master) ",_Module.m_server.m_strControllerName,lControllerVer,strConvertLayerVersion); 
		else
			str.Format("SiteController - %s ,Version %ld ,Convert layer version %s, (Controller is Slave)  ",_Module.m_server.m_strControllerName,lControllerVer,strConvertLayerVersion); 

		_LOGMSG.LogReq(0,"IGCI", "InitInterface", (LPCSTR)str); 				
	}	


	if(LOG_BASIC_FLOW_CONTROL)
	{
		DWORD dwThreadId = GetCurrentThreadId();
		CString tmpStr;
		tmpStr.Format("GCI thread ID = %ld",dwThreadId);
		_LOGMSG.LogMsg(tmpStr);
	}


	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::CancelTransaction(long lPumpNumber,long lTrsNumber, long * pVal)
{

//	_LOGMSG.LogLocationIn("GCI::CancelTransaction()",0);

#ifdef CATCH_GCI
	__try
	{
#endif
		CancelTransaction__(lPumpNumber,lTrsNumber,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::CancelTransaction")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::CancelTransaction()", LOG_LEVEL_0);	

		exit(1);
	}	
#endif

//	_LOGMSG.LogLocationOut("GCI::CancelTransaction()",0);
	return S_OK;
	
}

void CGCI::CancelTransaction__(long lPumpNumber,long lTrsNumber, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	
	long lRtc = 0;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	if(retCode == GCI_OK)
	{
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_GCI, lPumpNumber, lTrsNumber, &lRtc);					
		switch(iRetNum)
		{
			case TRS_CANCEL: retCode = GCI_OK; break;						
			case TRS_NOT_FOUND:	retCode = GCI_TRS_NOT_FOUND; break;			
			case TRS_NOT_CURRENT: retCode = GCI_TRS_NOT_CURRENT; break;
			case TRS_CANT_CANCEL_PREPAY_FROM_FORECOURT: retCode = GCI_CANT_CANCEL_TRS; break;
		}
		
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"CancelTransaction",
				TRUE,retCode,
				(lRtc!=0)? TRUE:FALSE,lRtc);

	if(pVal != NULL)
		*pVal = retCode;
}

//4.0.3.61
STDMETHODIMP CGCI::PAKReadyTransaction(long lPumpNumber,long lTrsNumber, short nNozzle, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long * pVal)
{
//	_LOGMSG.LogLocationIn("GCI::PAKReadyTransaction()",0);

#ifdef CATCH_GCI
	__try
	{
#endif

		PAKReadyTransaction__(lPumpNumber,lTrsNumber, nNozzle, lVolume, lValue, nLevel, lGradePrice, nGrade,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::PAKReadyTransaction")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
//			_LOGMSG.LogMsg("GCI::PAKReadyTransaction()", LOG_LEVEL_0);	
			_LOGMSG.LogClassMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::PAKReadyTransaction()",0);
	return S_OK;
}

void CGCI::PAKReadyTransaction__(long lPumpNumber, long lTrsNumber,  short nNozzle,long lVolume,long lValue,short nLevel,long lGradePrice,  short nGrade, long *pVal , long lFlags, long lTotalVal, BSTR pRec)
{
	short		nFinalGrade = nGrade;
	GCIRetCode	retCode = GCI_OK;	
	long		lFinalTrsNumber = lTrsNumber; 
	CString     sFinalLineMsg("End of transaction");
	long		cExtraInfo =0;
	long		lPosNumber =0;//4.0.12.501

	LOG_MSG_START_METHOD("PAKReadyTransaction__");

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	else if(_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if(GRADE_NOT_VALID(nGrade))
		retCode = GCI_INVALID_GRADE_NUMBER;

	else if(LEVEL_NOT_VALID(nLevel))
		retCode = GCI_INVALID_LEVEL_NUMBER;

	else if(NOZZLE_NOT_VALID(nNozzle))
		retCode = GCI_INVALID_NOZZLE_NUMBER;

	//4.0.29.500 TD 277379
	if(retCode == GCI_OK)
	{
		retCode = _Module.m_server.CheckPakReadyTransaction(lPumpNumber, lVolume,lValue, lFlags, lTotalVal, lTrsNumber);
		if (retCode != GCI_OK)
		{
			CString str;
			str.Format("PAKReadyTransaction: PAK rejected");
			_LOGMSG.LogMsg("GCI","PAKReadyTransaction__",lPumpNumber,LOG_PUMP,str);		   
		}
	}

	//3.0.0.8 (start)
	if(retCode == GCI_OK)
	{
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		CFleetCard cFleetCard; //4.0.9.507
		if(_Module.m_server.CheckVITConflict())
		{
			CPumpStatus stat = _Module.m_server.m_cPumpArray[lPumpNumber -1].GetStat();  //4.0.0.77
		
			if(lVolume < _Module.m_server.GetMaxVolumeForVITConflict()) 
			{
				long lVal;
				long lRetCode;
				BSTR sRetData = NULL;	
				PAY_AT_PUMP_INFO  savedInfo; // 4.0.10.504

				_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&savedInfo);	// 4.0.10.504					

				if (savedInfo.CardSaleInfo.cardData.sTranStatus == CD_VIT_CONFLICT)  //4.0.10.504
				{
					long p = lPumpNumber-1;
					long lSeqNumber = a2l(savedInfo.CardSaleInfo.cardData.sTranNbr,sizeof(savedInfo.CardSaleInfo.cardData.sTranNbr)); //4.0.10.504
					OLA_STAT ola;
					CPumpTransact trs;
					BOOL bFound;
					int iRetNum = 0;
					char sXmlBuff[MAX_XML_BUF_LEN];
		
					memset(&sXmlBuff,0,MAX_XML_BUF_LEN);
					
					//4.0.10.504
					Convertl2Str2(lValue  ,
						savedInfo.CardSaleInfo.extraData.sTotalAmt_Msb,
						sizeof(savedInfo.CardSaleInfo.extraData.sTotalAmt_Msb) , savedInfo.CardSaleInfo.extraData.sTotalAmt, sizeof(savedInfo.CardSaleInfo.extraData.sTotalAmt));	//4.0.9.222 
					
					//amram TD 18147
					Convertl2Str2(lValue/10 ,
						savedInfo.CardSaleInfo.extraData.sFuelAmt_Msb,
						sizeof(savedInfo.CardSaleInfo.extraData.sFuelAmt_Msb) , savedInfo.CardSaleInfo.extraData.sFuelAmt, sizeof(savedInfo.CardSaleInfo.extraData.sFuelAmt));	//4.0.11.500

						if(LOG_BASIC_FLOW_CONTROL)
						{
							
							CString str;
							str.Format("Avi conflict on pump, overwrite value=%ld ",lValue);
							_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str);
						}

					bFound = _Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&trs);//4.0.12.516

					if(bFound)
					{												
						trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[p].GetShiftNumber();
						trs.m_lPump = lPumpNumber;
					
						// open new transaction.
						//--------------------------------//						

						trs.m_lNumber = _Module.m_server.NewTransactNumber();
						trs.m_lStatus = IN_PROGRESS;
						trs.m_nLinkFlags = TRS_NO_LINK;
						trs.m_lLinkNumber = 0;
						trs.m_lRes = PAY_AT_KIOSK;
						trs.m_lResExt = NULL_RES_EXT;
						trs.m_lActiveReadNumber = _Module.m_server.GetActiveReadNumber();
						trs.UpdateTime();
						trs.UpdateStartTime();	//TD 40492
						trs.m_lTankID =  _Module.m_server.m_cPumpArray[p].GetTankPerPosition(nNozzle); //4.0.27.60  TD 231475

						if(PUMPSRV_IS_MASTER)
						{
							iRetNum = _Module.m_server.CentralInsertTrs(lPumpNumber, &trs);
							switch(iRetNum)
							{
								case 0: 
									retCode = GCI_OK;
									break;
								case 1: 
									retCode = GCI_PUMP_TRS_LIST_IS_FULL;
									break;
								case 2: 
									retCode = GCI_PUMP_ALREADY_HAVE_IN_PROGRESS_TRS;
									break;
								case 4: 
									retCode = GCI_TRS_NOT_FOUND;
									break;
							}							

						}
						
						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;
							str.Format("Trs %d not found !!!  , PumpSrv accept VIT transaction as PAK transaction RetCode=%d ", trs.m_lNumber, iRetNum);
							_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str);
						}				

					}
					
					//if this is a PrePay transaction - crate a new PAP and restore the PrePay
					if(trs.GetLinkFlags() & TRS_LINK_PREPAY_NUMBER)
					{
						//copy transaction datails to a new PAP transaction.
						CPumpTransact trsnew(_Module.m_server.NewTransactNumber(),_Module.m_server.m_cPumpArray[p].GetShiftNumber() );
						
						trsnew.m_lStatus = IN_PROGRESS;
						trsnew.m_lOnOffer = trs.m_lOnOffer;
						trsnew.m_lPump = trs.m_lPump;
						trsnew.m_lRawVolume = trs.m_lRawVolume;
						trsnew.m_lRoundedVolume = trs.m_lRoundedVolume;
						trsnew.m_lRoundedValue = trs.m_lRoundedValue;
						trsnew.m_lGradePrice = trs.m_lGradePrice;
						trsnew.m_nGrade = trs.m_nGrade;
						trsnew.m_nLevel = trs.m_nLevel;
						trsnew.m_lTankID = trs.m_lTankID; //4.0.27.60  TD 231475

						_Module.m_server.CentralInsertTrs(p+1, &trsnew);
												
						//reset the PrePay transaction
						trs.m_lStatus =	WAIT_TIME_OUT;
						trs.m_lRawVolume = 0;
						trs.m_lRoundedVolume = 0;
						trs.m_lRoundedValue = 0;
						trs.m_lGradePrice = 0;
						trs.m_nGrade = 0;

						//Eyal 4.0.0.47 (3.2.0.93)
						//Store time out in trs history
						trs.m_lMisc |= MISC_PREPAY_TIMED_OUT;

						
						_Module.m_server.CentralUpdateTrs(p+1, trs.m_lNumber,&trs ,FALSE);
						lTrsNumber = trsnew.m_lNumber; //46349
					}
					
					_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs);
					
					if (trs.m_lResExt == RES_EXT_ATTENDANT)				//4.0.2.27
						trs.m_lMisc |= MISC_ORIGINAL_ATTENDANT_TRS;

					trs.m_lMisc |= MISC_VIT_CONFLICT;					//3.2.1.44
					
					trs.m_lResExt = RES_EXT_AVI;						//3.2.1.50

					memset(savedInfo.CardSaleExtraData2.sServiceFee , ' ' , sizeof(savedInfo.CardSaleExtraData2.sServiceFee));

					/****************************************************************
					Update taxes in case of avi conflict 4.0.22.230 98465 //4.0.23.509
					*****************************************************************/
					if (lFlags & PAK_INCLUDE_EXTRA_INFO_REC)
					{
						TAG_EXTRA_INFO  cTagExtraInfo;
						memset(&cTagExtraInfo,' ', sizeof(cTagExtraInfo));

						int iLen = Bstr2Buffer(pRec, (void *)&cTagExtraInfo, sizeof(cTagExtraInfo));
						if(iLen == 0)
						{
							DWORD  dwLastErr = GetLastError();
							retCode = GCI_LAST_ERROR;
						}
						else
						{
							memcpy(savedInfo.CardSaleExtraData3.sTaxId, cTagExtraInfo.sTaxId, sizeof(savedInfo.CardSaleExtraData3.sTaxId));
							memcpy(&savedInfo.CardSaleExtraData3.sTaxIncluded, &cTagExtraInfo.sTaxIncluded, sizeof(savedInfo.CardSaleExtraData3.sTaxIncluded));
							memcpy(savedInfo.CardSaleInfo.extraData.sVAT_Rate, cTagExtraInfo.sVAT_Rate, sizeof(savedInfo.CardSaleInfo.extraData.sVAT_Rate));
							memcpy(savedInfo.CardSaleInfo.extraData.sTaxAmt, cTagExtraInfo.sTaxAmt, sizeof(savedInfo.CardSaleInfo.extraData.sTaxAmt)); //4.0.22.231  //4.0.23.509
							memcpy(&savedInfo.CardSaleExtraData5.sVAT_RateMSB, cTagExtraInfo.sVAT_Rate, sizeof(savedInfo.CardSaleInfo.extraData.sVAT_Rate)); //4.0.31.500  TD 404911

							if(LOG_DETAIL_FLOW_CONTROL)
							{
								CString str;
								str.Format("Avi conflict, Tax was copied to the new PAP");
								_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
							}
						}
					}


					_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&savedInfo);	// 4.0.10.504					


					trs.SetLink(TRS_LINK_OLA_NUMBER, lSeqNumber);
					_Module.m_server.CentralUpdateTrs(p+1, trs.m_lNumber,&trs ,FALSE);

					//Mark the Sassion to be SALE:
					memset(&ola,0,sizeof(OLA_STAT));
					ola.m_byState = SESSION_SALE;
					_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
					_Module.m_server.SavePumpInfo(p+1);					

					//Convert the struct to xml //4.0.16.504
					if (!(_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml(sXmlBuff,(&savedInfo))))
					{
						retCode = GCI_FAILED_IO;
					}

					CComBSTR sData(sizeof(sXmlBuff) ,(LPCSTR)sXmlBuff); 			

										
					lRetCode = PAPReadyTransaction(lPumpNumber, lSeqNumber, lTrsNumber, nNozzle, lVolume, lValue, nLevel, lGradePrice, nGrade, STRUCT_IS_CARD_ALL_DATA3, sData, &sRetData, &lVal); //4.0.5.0

					if (sRetData)
						SysFreeString(sRetData);						
						 
					if (lRetCode == GCI_OK)
					{
// 4.0.22.440 133580	- this code needs to move to a later stage. //4.0.25.501
// 						//Delete extra info record if exists 4.0.23.509 97935
// 						EXTRA_INFO_REC ExtraInfoRec;
// 						memset(&ExtraInfoRec , 0, sizeof(EXTRA_INFO_REC));
// 
// 						ExtraInfoRec.info.lIndexNumber = lTrsNumber;
// 						long lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);
// 
// 						if (!lRtc)
// 						{
// 							//Delete record.
// 							lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_DELETE, &ExtraInfoRec);
// 
// 							if(LOG_BASIC_FLOW_CONTROL)
// 							{
// 								CString str;
// 								str.Format("Remove extra info record in avi conflict. Found extra info data for trs number %ld, record removed status %ld  ",lTrsNumber, lRtc);
// 							}
// 
// 						}	

						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;
							str.Format("VIT Conflict");
							_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str);
							str.Format("Trs %d Was converted from PAK to PAP",trs.m_lNumber);
							_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str);
						}
												
						retCode = GCI_TRS_CONVERTED_TO_PAP;
					}
					else 
						_Module.m_server.m_cPumpArray[p].ClearOlaStat();
				}
			}			
			else    //4.0.8.20
			{				
				PAY_AT_PUMP_INFO Info; 
				memset(&Info,' ',sizeof(Info));
				_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&Info);					

				if (Info.CardSaleInfo.cardData.sTranStatus == CD_VIT_CONFLICT)
				{
					Info.CardSaleInfo.cardData.sTranStatus = ' ';
					_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&Info);					
					_Module.m_server.SavePumpInfo(lPumpNumber); 
				}

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("Clear AVI conflict on pump flag"); 
					_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str);
				}
			}
		}

		if (cFleetCard.CheckReeferWaiting(lPumpNumber, lTrsNumber, nNozzle, lVolume, lValue, nLevel, lGradePrice, nGrade))
			retCode = GCI_UPDATE_OLA_MEM_INFO; 

		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}

	if(retCode == GCI_OK)
	{
		// Save pump reservation and mode while transaction was 
		// In Progress 

		CPumpTransact	trs;
		long			p = lPumpNumber-1;
		BOOL			bFound = TRUE;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if ((lFlags & PAK_INCLUDE_IGNORE_VALUE_VOLUME_CHECK) && (!lFinalTrsNumber)) //4.0.11.133 //4.0.21.500 - TD 14778
		{//4.0.12.502
			if ((_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs)) ||
				(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_TIME_OUT,TRS_LINK_PREPAY_NUMBER,&trs)) || //4.0.10.42
				(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs)  ) )
			{
				if ((_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_CASH_ACCEPTOR_INFO,&trs)) ||
					(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_TIME_OUT,TRS_LINK_CASH_ACCEPTOR_INFO,&trs)) || //4.0.10.42
					(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS,TRS_LINK_CASH_ACCEPTOR_INFO,&trs)  ) )
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("PAKReadyTransaction__  Cash Acc Trs with out lFinalTrsNumber  - use Cash Acc trs %ld",trs.m_lNumber);
						_LOGMSG.LogMsg(str);
						lFinalTrsNumber= trs.m_lNumber;
					}
				}

				lFinalTrsNumber = trs.m_lNumber;
			}
			else if (_Module.m_server.m_cPumpArray[p].GetTrsByStatus(PAID,TRS_LINK_CASH_ACCEPTOR_INFO,&trs) && lValue)
			{
				if (!trs.m_lRoundedValue)   //4.0.11.115
				{
					lFinalTrsNumber= trs.m_lNumber;
					trs.m_lStatus =  IN_PROGRESS;
					_Module.m_server.CentralUpdateTrs(lPumpNumber,lFinalTrsNumber,&trs,FALSE);
				}
			}

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("pump %02ld got trs number zero , found internally trs %ld" , lPumpNumber ,lFinalTrsNumber);
				_LOGMSG.LogMsg(str);
			}
		}

		if(_Module.m_server.NozzleLevelLogic())
		{
			if (!nGrade && nNozzle && lPumpNumber)
				nFinalGrade = _Module.m_server.m_cPumpArray[p].NozzleToGrade(nNozzle);
		}

		// CR 474446 - update the nozzle so it will be sent to FuelMobileSrv
		if (nNozzle == 0)
		{
			nNozzle = (short) _Module.m_server.GetGradePositionRestrection(nGrade, lPumpNumber);
		}

		// checking for Pre Pay that was changed to Pay at kiosk 
		if(_Module.m_server.m_cPumpArray[p].GetTrsByNumber(lFinalTrsNumber,&trs))
		{ // Not Found 
			if ((lFinalTrsNumber == _Module.m_server.m_cPumpArray[p].m_lChangedTrsNumber) &&
				(lFinalTrsNumber != 0))
			{
				if(!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))
				{
					lFinalTrsNumber = trs.m_lNumber;
					bFound = TRUE;
					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("Trs %d not found , ChangedTrsNumber %d, Trs Use lFinalNumber %d",lTrsNumber,_Module.m_server.m_cPumpArray[p].m_lChangedTrsNumber,lFinalTrsNumber);
						_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str);
					}	
				}
				else
				{
					bFound = FALSE;
				}
			}
			else
			{				
				bFound = FALSE;
			}
		}
		else
		{
			bFound = TRUE;
		}
		

		// 2.0.6 
		// Just in case PumpSrv deleted transaction, by auto detect of 
		// cancel 0 val transaction , and after a while 
		// got update - final information, PumpSrv reInsert 
		// the transaction into the list and act like the 
		// transaction was there.
		// This rule will be correct for final of lost transaction.
		if(!bFound)
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Trs %d not found !!!  , PumpSrv accept transaction as PAK transaction ",lTrsNumber);
				_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str);
			}

			//If this PAK transaction is converted form PAP
			//mark it as one that was PAP canceled.
			if ( trs.m_lResExt == RES_EXT_PAP_CANCELED )
				trs.m_lMisc |= MISC_TRS_PAP_CANCELED;

			trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[p].GetShiftNumber();
			trs.m_lPump = lPumpNumber;
		

			// open new transaction.
			//--------------------------------//
			
				if (_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_TIME_OUT,TRS_LINK_PREPAY_NUMBER,&trs))
				{
					lFinalTrsNumber = lTrsNumber = trs.m_lNumber;

					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("Pump - %02ld  found wait time out trs",lPumpNumber ,lTrsNumber);
						_LOGMSG.LogMsg(str);
					}
				}
				else if(_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))
				{
					// if transaction not found open new one
					if (!lTrsNumber) //4.0.5.14
						lTrsNumber = _Module.m_server.NewTransactNumber();  //4.0.5.14

					trs.m_lNumber = lTrsNumber;
					trs.m_lStatus = IN_PROGRESS;
					trs.m_nLinkFlags = TRS_NO_LINK;
					trs.m_lLinkNumber = 0;
					trs.m_lRes = PAY_AT_KIOSK;
					trs.m_lResExt = NULL_RES_EXT;					
					trs.m_lActiveReadNumber = _Module.m_server.GetActiveReadNumber();
					trs.UpdateTime();
					trs.UpdateStartTime();	//TD 40492
					lFinalTrsNumber = lTrsNumber;  //4.0.5.14
					trs.m_lActiveShiftNumber  = _Module.m_server.m_cPumpArray[p].GetShiftNumber(); //4.0.5.26
					trs.m_lTankID =  _Module.m_server.m_cPumpArray[p].GetTankPerPosition(nNozzle); //4.0.27.60  TD 231475
					

					//4.0.3.46
					CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[p].GetStat();
					long lMisc = pumpStat.GetMisc();

					if(lMisc & MISC_PAK_TO_PAP_FAILED)
					{
						trs.m_lMisc |= MISC_TRS_PAK_TO_PAP_FAILED;
					}

					if(PUMPSRV_IS_MASTER)
					{
						long iRetNum = _Module.m_server.CentralInsertTrs(lPumpNumber, &trs);
						switch(iRetNum)
						{
							case 0: 
								retCode = GCI_OK;
								break;
							case 1: 
								retCode = GCI_PUMP_TRS_LIST_IS_FULL;
								break;
							case 2: 
								retCode = GCI_PUMP_ALREADY_HAVE_IN_PROGRESS_TRS;
								break;
							case 4: 
								retCode = GCI_TRS_NOT_FOUND;
								break;
						}							
					}

					//4.0.0.83
					//Since this is a recreated transaction, we would like it to be treated as
					//a PAK transaction no matter what.
					if(retCode == GCI_OK)
						trs.m_lRes = PAY_AT_KIOSK;
				}
				else //transaction was found, add current transaction number.
				{					
					lFinalTrsNumber = lTrsNumber = trs.m_lNumber;
				}					
		}

		// make sure that same trs is not updated twice 
		// ( by allowing only one switch from IN_PROGRESS to PAID/UNPAID)
		if((PUMPSRV_IS_MASTER) && (retCode == GCI_OK) || 
			(trs.m_lStatus == WAIT_TIME_OUT) && (bFound)) //3.2.1.44		
		{
			LOG_MSG_FUNCINC(" ((PUMPSRV_IS_MASTER) && (retCode == GCI_OK) || (trs.m_lStatus == WAIT_TIME_OUT) && (bFound)) = TRUE");
			if((trs.m_lStatus == IN_PROGRESS)   ||
			   (trs.m_lStatus == WAIT_PROGRESS) ||
			   (trs.m_lStatus == WAIT_AUTH)     ||  //4.0.3.21
			   ((trs.m_lStatus == WAIT_TIME_OUT) && lValue) // 4.0.1.32 //4.0.11.90
			   )
			{
				BOOL bNoRollingUpdate = FALSE;
				if (!trs.m_lRoundedValue && lValue)
					bNoRollingUpdate = TRUE;

				UpdateTrsDetails(trs, nNozzle, lVolume, lValue, nLevel, lGradePrice, nFinalGrade, p, lPumpNumber, lFinalTrsNumber); // 4.0.19.1770 TD 349356

				CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[p].GetStat();
				long lMisc = pumpStat.GetMisc();

				if(lMisc & MISC_CURRENT_TRS_PAP_CANCELED)
				{
					//This transaction was started as PAP, but ended as PAK.
					LOG_MSG_FUNCINC("lMisc & MISC_CURRENT_TRS_PAP_CANCELED. //This transaction was started as PAP, but ended as PAK." );
					
					trs.m_lStatus = UNPAID; // Pay At Kiosk ...
					trs.m_lMisc |= MISC_TRS_PAP_CANCELED;

					//4.0.9.500
					trs.m_lRes = PAY_AT_KIOSK;
					trs.SetLink(TRS_NO_LINK,0L);

					if (OLA_BY_CONTROLLER)
						trs.m_lMisc |= MISC_TRS_FAILED_CAT; //4.0.22.508 75592

					//3.0.0.5
					lMisc &= MISC_NOT_CURRENT_TRS_PAP_CANCELED;
					pumpStat.SetMisc(lMisc);
					_Module.m_server.m_cPumpArray[p].SetStat(pumpStat);

					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("Hot Pump Flag Clear");
						_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",p + 1,LOG_PUMP,str);
					}
					//Call the get PromID 
					//Cancel ticket from loyalty
					if(_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()) //4.0.23.502
						_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber);
				}				
				else if(trs.GetLinkFlags() & TRS_LINK_PREPAY_NUMBER)
				{
					LOG_MSG_FUNCINC("trs.GetLinkFlags() & TRS_LINK_PREPAY_NUMBER");
					BOOL  bClosePrePay = TRUE;	//4.0.9.507
					long  lNextMaxValueAproved = 0;	
											
					trs.m_lStatus = PAID; // PrePay At Kiosk ...				
					long lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &trs , NULL);
					if(lRtc)
					{
						if(LOG_LOGIC_ERROR)
						{
							CString str;
							str.Format("Update Operation for PrePay Failed !!!  Trs %d, lRtc %d", trs.m_lNumber, lRtc ); 
							_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str,LOG_LEVEL_1);
						}
						retCode = GCI_FAILED_IO;
					}

					if (bClosePrePay)  //4.0.9.507
					{
						// check if the Prepay is Balance Close. 
						if (!lRtc)
						{
							PRE_PAY_INFO	cTmpPrePayinfo;
							CCashAcceptor  cCashAcceptor;  
							long lVal = 0;
							long lRetVal=0;
							BOOL bUpdatePrePayInfo = FALSE;	//4.0.15.500

							// Generate Cash acceptor receipt row data
							if (trs.GetLinkFlags() & TRS_LINK_CASH_ACCEPTOR_INFO) //4.0.9.509
								cCashAcceptor.GenerateReceiptRowData(trs);

							lRtc = _Module.m_server.TrsLinkOperation(REC_READ,&trs,&cTmpPrePayinfo);
							
							
							lPosNumber = cTmpPrePayinfo.lPOSNumber+POS_NUMBER_PAID_QDX;//4.0.12.501

							if (trs.GetLinkFlags() & TRS_LINK_CASH_ACCEPTOR_INFO) //4.0.9.509
								lPosNumber = 0; //4.0.14.506
							
							TAG_EXTRA_INFO  cTagExtraInfo;	//4.0.15.500
							GCIRetCode	retCode = GCI_OK;
							// Start 4.0.15.500 - CR 19675
							// Update PreSet fields and write to PrePay.qdx
							if ((lFlags & PAK_INCLUDE_EXTRA_INFO_REC) || (lFlags & PAK_INCLUDE_EXTRA_INFO_REC_2)) //4.0.21.500 - TD 14778  // TD 381791
							{
								int iLen = Bstr2Buffer(pRec, (void *)&cTagExtraInfo, sizeof(cTagExtraInfo));

								if (!(trs.GetLinkFlags() & TRS_LINK_PREPAY_NUMBER)) //4.0.21.500 - TD 14778
									trs.SetLink(TRS_LINK_PREPAY_NUMBER, trs.m_lNumber);

								if(iLen == 0)
								{
									DWORD  dwLastErr = GetLastError();
									retCode = GCI_LAST_ERROR;
								}
								else
								{
									EXTRA_INFO cExtraInfo;									
									cExtraInfo.lIndexNumber = trs.m_lNumber;
									UpdatePrePayExtraInfoWithCarWashData(cTagExtraInfo , cExtraInfo); // 4.0.29.500 TD 287153

									if ((trs.GetLinkFlags() & TRS_LINK_CASH_ACCEPTOR_INFO) && (_Module.m_server.m_cPumpArray[lPumpNumber -1].IsPapToPakTrs())) //4.0.24.140 127928
									{
										//In case of converting Pap to Cash acceptor, no record exists in ExtraInfo so far, so we need to insert.

										if(!ConvertTagRecord(cTagExtraInfo, cExtraInfo , trs))	//4.0.22.504 73878 insert record only if one already exists.
										{
											if (lRtc == OK)
											{
												lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE,REC_UPDATE,&cExtraInfo);
											}
											else
											{
												lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_INSERT, &cExtraInfo);											

											}								
										}
										if(LOG_DETAIL_FLOW_CONTROL)
										{
											CString sLogMsg; 
											sLogMsg.Format("PAKReadyTransaction, trs %ld was converted to cash acceptor with extra data. Insert retcode = %ld", trs.m_lNumber, lRtc);
											_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg); 
										}	
									}
								}
							}
							// End 4.0.15.500

							//3.2.1.50
							//4.0.3.21 If the service fee has already been charged by the POS,
							//do not add it
							if(!(cTmpPrePayinfo.lFlags & SERVICE_FEE_CHARGED_BY_POS))
								_Module.m_server.GetServiceFeeInfo(p+1,SERVICE_TYPE_PREPAY,0,NULL,GET_FEE_BY_TRANSACTION_TYPE,&lVal,&lRetVal); //marik12345		
							
							if (( _Module.m_server.GetFinishedPrepayIndication() == TRUE) && (!lRtc))	//56040 4.0.19.508	
								_Module.m_server.m_cPumpArray[p].IncreaseUnBalancePrePayCounter();
							else 
							//4.0.3.29 Changed from != to <, so if we fueled for more
							//than we paid for, we do not count it as an unbalanced prepay
							//4.0.22.180 - treat also over dispensing of limit by Volume TD 90090 //4.0.23.508
							if (IsPrePayUnbalanced(trs, lVal, cTmpPrePayinfo) && (!lRtc))	//4.0.25.720 - TD 325958
							{
  								if (!((trs.GetLinkFlags() & TRS_LINK_CASH_ACCEPTOR_INFO) && bNoRollingUpdate && (lVolume == 0)))  //4.0.21.500 - TD 59314  //4.0.24.220 TD 136887 - put in remark
 								{	//*********************************************
									//If its cash acc. and there is No rolling update && (value > 0) and (volume == 0) then //232988 4.0.24.360 //4.0.27.60
									// we have only Car wash in this trs 
									//*********************************************
									_Module.m_server.m_cPumpArray[p].IncreaseUnBalancePrePayCounter();


									if(LOG_BASIC_FLOW_CONTROL) //4.0.24.160
									{	
										CString str;
										
										str.Format("GCI::PAKReadyTransaction4__, trs %ld IncreaseUnBalancePrePayCounter", trs.m_lNumber);
										_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
									}

								}
							}

							//4.0.3.60 //4.0.15.500
							else if((((cTmpPrePayinfo.lMaxValue  > 0) && ((trs.m_lRoundedValue + lVal) > cTmpPrePayinfo.lMaxValue )) || 
								    ((cTmpPrePayinfo.lMaxValue == 0) &&  ((trs.m_lRoundedVolume      ) > cTmpPrePayinfo.lMaxVolume))) &&  //4.0.22.180 - treat also over dispensing of limit bt Volume TD 90090 //4.0.23.508
									(!lRtc))
							{
								//sending the over limit alarm - 4.0.7.501
								CString cAlarmMsg;
								BOOL bOverFueling = TRUE ; 	// 4.0.25.30

								if (cTmpPrePayinfo.lFlags  & LIMIT_BY_VOLUME )   //4.0.25.30
								{
									if (trs.m_lRoundedVolume > cTmpPrePayinfo.lMaxVolume )  //4.0.27.20
									{
										float fTotalValue = (float)(trs.m_lRoundedVolume)/1000;									
										cAlarmMsg.Format("Value is:%.3f",fTotalValue);
									}
									else
										bOverFueling = FALSE;

								}
								else
								{
									float fTotalValue = (float)(trs.m_lRoundedValue + lVal)/1000;									
									cAlarmMsg.Format("Value is:%.3f",fTotalValue);

								}

				
								if((_Module.m_server.IsAlarmSrvActive() == TRUE) && bOverFueling)					//4.0.23.501	TD 78374  //4.0.25.30 - TD 111737
								{
									_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_PUMP,
																			 SUB_TERMINAL_DISPENSER,
																			 DISPENSER_OVER_FUELLING,
																			 trs.m_lPump,
																			 ALARM_ON,
																			 trs.m_lPump,  //volume + service fee
																			 0,
																			 cAlarmMsg.GetBuffer(0));
								}

																//4.0.11.502else
								_Module.m_server.m_cPumpArray[p].IncreaseUnBalancePrePayCounter();
								if(LOG_BASIC_FLOW_CONTROL) //4.0.24.160
								{	
									CString str;
									
									str.Format("GCI::PAKReadyTransaction5__, trs %ld IncreaseUnBalancePrePayCounter", trs.m_lNumber);
									_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
								}

								if (trs.GetLinkFlags() & TRS_LINK_CASH_ACCEPTOR_INFO) //4.0.9.509
								{
									cCashAcceptor.FinalizeTrs(trs); // Create Final records
									if (bNoRollingUpdate) //4.0.21.500 - TD 59314
										_Module.m_server.m_cPumpArray[p].DecreaseUnBalancePrePayCounter();
								}

							}
							else if (trs.GetLinkFlags() & TRS_LINK_CASH_ACCEPTOR_INFO) //4.0.9.509
							{
								cCashAcceptor.FinalizeTrs(trs); // Create Final records
								_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_PAP_TRS,0); //4.0.21.500 - TD 14778

								if (bNoRollingUpdate) //4.0.21.500 - TD 59314
									_Module.m_server.m_cPumpArray[p].DecreaseUnBalancePrePayCounter();
							}

							if (!lRtc) // 
							{
								//Save latest PrePay info
								_Module.m_server.m_cPumpArray[p].SetLatestPrePaySyncNumber(cTmpPrePayinfo.lSyncNumber);
								_Module.m_server.m_cPumpArray[p].SetLatestPrePayPosNumber(cTmpPrePayinfo.lPOSNumber);
								
								//4.0.3.61
								if (lFlags == PAK_INCLUDE_REWARD_DATA)  //4.0.5.44
								{

									cTmpPrePayinfo.lTotalReward = lTotalVal;
									lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE,&trs,&cTmpPrePayinfo);

									if(LOG_DETAIL_FLOW_CONTROL)
									{
										CString str;
										str.Format("Coupon  given reward=%ld, lRtc=%ld",lTotalVal, lRtc);  
										_LOGMSG.LogMsg(str);
									}

								}
								else if (trs.GetLinkFlags() & TRS_LINK_FLEET)  //4.0.999.0
								{
									cTmpPrePayinfo.lMaxValue = lNextMaxValueAproved;
									lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE,&trs,&cTmpPrePayinfo);

									if(LOG_DETAIL_FLOW_CONTROL)
									{
										CString str;
										str.Format("Total Fleet transaction is %ld",lNextMaxValueAproved);  
										_LOGMSG.LogMsg("GCI","PAKReadyTransaction",lPumpNumber,LOG_PUMP,str);
									}

								}
								// 4.0.15.500 - CR 19675
								else if (bUpdatePrePayInfo) 
								{									
									lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE,&trs,&cTmpPrePayinfo);
								}
							}


						}
						// Send command paid to the controller, to realeas transaction.					
						if (PUMPSRV_REPORT_ON_TRS_PAID) //4.0.5.33
						{
							CString sLogMsg;
							//4.0.13.93
							PRE_PAY_INFO	cTmpPrePayinfo;
							long lVal = 0;
							long lRetVal=0;

							CMD_STRUCT_TRS cTRSCmd;

							//4.0.13.93
							lRtc = _Module.m_server.TrsLinkOperation(REC_READ,&trs,&cTmpPrePayinfo);
							if(!(cTmpPrePayinfo.lFlags & SERVICE_FEE_CHARGED_BY_POS))
								_Module.m_server.GetServiceFeeInfo(p+1,SERVICE_TYPE_PREPAY,0,NULL,GET_FEE_BY_TRANSACTION_TYPE,&lVal,&lRetVal);	

							cTRSCmd.lTrsNumber = trs.m_lNumber;
							trs.m_lStatus = ACCEPTED; //PrePay transaction, wait to controller approval. 
							trs.m_lOnOffer = SERVER_OLA_POS_NUMBER;

							if ((_Module.m_server.GetUnbalancedPrPTimer() == 0) || 
								((trs.m_lRoundedValue + lVal) >= cTmpPrePayinfo.lMaxValue)) //Balance PrePay 4.0.13.93
							{
								_Module.m_server.SetCommand(CMD_TRS_PAID,
															SERVER_OLA_POS_NUMBER,
															lPumpNumber,
															&cTRSCmd,
															sizeof(cTRSCmd));


								if(LOG_DETAIL_FLOW_CONTROL)
								{
									sLogMsg.Format("SetStatePaid -  Send cmd to controller to clear PrePay trs.  (Pump %02d, Trs %d, Status %d, link Number %d", 
												   lPumpNumber, 
												   trs.m_lNumber,
												   trs.m_lStatus,
												   trs.m_lLinkNumber );

									_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg); 
								}	

								//4.0.11.503
								//save the trs to Paid QDX
								_Module.m_server.InsertToPaidQDX(trs,lPosNumber);//4.0.12.501
							}
							else
							{
								if(LOG_DETAIL_FLOW_CONTROL)
								{
									sLogMsg.Format("Wait before set state paid, lock the pump, UnbalancedPrPTimer=%ld  (Pump %02d, Trs %d, Status %d, link Number %d",
												   _Module.m_server.GetUnbalancedPrPTimer(),
												   lPumpNumber, 
												   trs.m_lNumber,
												   trs.m_lStatus,
												   trs.m_lLinkNumber );

									_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg); 
								}	

								//4.0.12.501
								//If we already got the Ready,Null status. 
								CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();

								if (pumpStat.GetStatus() == READY && pumpStat.GetRes() == NULL)
								{
									trs.SetOnOffer(lPosNumber);				

									//Lock the pump
									CMD_STRUCT_RESERVE_PUMP cmdReserveSinglePump;
									memset(&cmdReserveSinglePump, 0, sizeof(CMD_STRUCT_RESERVE_PUMP));

									//Filling the command of reserving a pump and sending it
									cmdReserveSinglePump.lRes	= PUMPSRV;
									cmdReserveSinglePump.lResExt |= PUMP_SINGLE_LOCK;

									_Module.m_server.SetCommand(CMD_RESERVE_PUMP, SERVER_POS_NUMBER, lPumpNumber, &cmdReserveSinglePump, sizeof(CMD_STRUCT_RESERVE_PUMP)); 	

									if (LOG_BASIC_FLOW_CONTROL)
									{
										CString sLogStr;
										sLogStr.Format("Sent a command to reserve pump %d\n", lPumpNumber);
										_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogStr); 				
									}
									//Changing the state to PUMP_WAIT_TO_BE_EXCLUSIVELY_LOCKED in the state machine
									_Module.m_server.m_cSinglePumpLock.SetPumpStateWaitLocked(lPumpNumber);

								}

							}

						}
						else
						{
							//4.0.11.503
							//save the trs to Paid QDX
							_Module.m_server.InsertToPaidQDX(trs,lPosNumber);//4.0.12.501
						}
					}
					else //preform next item authorization
					{
						// the data of the second trs should by 0
						trs.m_lRoundedVolume = trs.m_lRoundedValue =0;//4.0.10.1000
					}

					
				}

				else if ( trs.GetLinkFlags() == TRS_LINK_OLA_NUMBER )  // PAY AVI
				{
					LOG_MSG_FUNCINC("trs.GetLinkFlags() == TRS_LINK_OLA_NUMBER");
					if (trs.m_lMisc & MISC_TRS_PAP_CANCELED)
					{
						trs.m_lStatus = UNPAID; // Convert PAP to Pay At Kiosk ...
						
					}					
					else if (OLA_BY_CONTROLLER)
					{
						//Allow PAKReadyTransaction to arrives in the middle of a dispensing PAP - (WOW POSTEC) 
						LOG_MSG_FUNCINC("OLA_BY_CONTROLLER. //Allow PAKReadyTransaction arrive in the middle of a dispensing PAP - (WOW POSTEC)     ");
						if ((_Module.m_server.IsRedundancyByCL() == TRUE) && (trs.m_lRes == PAY_AT_PUMP))			//4.0.24.60 
						{
							trs.SetLink(TRS_NO_LINK,0L);
							trs.m_lStatus = UNPAID; 
							trs.m_lRes = PAY_AT_KIOSK;
							trs.m_lResExt = NULL_RES_EXT;

							if(LOG_LOGIC_ERROR)
							{
								CString str;
								str.Format("Allow getting a PAK transaction Trs %d, Misc=%ld in the middle of PAP.", trs.m_lNumber,  trs.m_lMisc ); 
								_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str,LOG_LEVEL_1);
							}
						} 
						else
						{
							trs.m_lMisc |= MISC_TRS_PAP_CANCELED;
							trs.SetLink(TRS_NO_LINK,0L);
							trs.m_lStatus = UNPAID; // Convert PAP to Pay At Kiosk ...

							if(LOG_LOGIC_ERROR)
							{
								CString str;
								str.Format("Convert PAP to Pak  !!!  Trs %d, Misc=%ld", trs.m_lNumber,  trs.m_lMisc ); 
								_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str,LOG_LEVEL_1);
							}
						}
					}

					else
					{
						LOG_MSG_FUNCINC(" OLA_BY_CONTROLLER = FALSE ");
						//4.0.3.47
						PAY_AT_PUMP_INFO cPayAtPumpInfo;
						memset(&cPayAtPumpInfo,' ',sizeof(cPayAtPumpInfo));							
						_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cPayAtPumpInfo);
						OLA_STAT ola;
						
						if (_Module.m_server.GetOLAFlags(cPayAtPumpInfo.CardSaleExtraData2.sFlags , FL_PAK_TO_PAP))
						{
							trs.SetLink(TRS_NO_LINK, 0L);
							trs.m_lStatus = UNPAID; // Convert PAP to Pay At Kiosk ..
							trs.m_lRes = PAY_AT_KIOSK;
							trs.m_lResExt = NULL_RES_EXT;

							_Module.m_server.m_cPumpArray[lPumpNumber -1 ].GetOlaStat(&ola);
							
							if( OLA_STATE(ola.m_byState) != OLA_NONE )
							{
								ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_SEND_END);
								_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola); 	
								_Module.m_server.SavePumpInfo(lPumpNumber); // save ola stat.							
							}							
						}
						else if (trs.m_lResExt != RES_EXT_FLEET && trs.m_lResExt != RES_EXT_MOBILE && trs.m_lResExt != RES_EXT_PAP_EXTERNAL)
							retCode = GCI_TRS_MISMATCH;
					}
					
				}

				//4.0.2.21 Although attendant transactions start as OLA transactions
				//because the attendant card is swiped, the TRS_LINK_OLA_NUMBER flag
				//is changed to TRS_LINK_ATTEND in CGCI::SetPumpStatus() when
				//the reserervation extension is set to RES_EXT_ATTEND.
				else if(trs.GetLinkFlags() & TRS_LINK_ATTEND)
				{
					LOG_MSG_FUNCINC(" trs.GetLinkFlags() & TRS_LINK_ATTEND ");
					if (pumpStat.GetResExt() == RES_EXT_ATTENDANT) // 3.2.0.76
					{
						CPumpTransact cTmptrs;

						trs.m_lStatus = UNPAID; // Pay At Kiosk ...
						trs.m_lResExt = RES_EXT_ATTENDANT; 
						trs.m_lRes = PAY_AT_KIOSK;

						 PAY_AT_PUMP_INFO PayAtPumpInfo;
						 _Module.m_server.m_cPumpArray[lPumpNumber -1].GetPAPInfo(&PayAtPumpInfo);
						 cTmptrs = trs;
						 cTmptrs.SetLink(TRS_LINK_ATTEND , 0);
						 if (PayAtPumpInfo.CardSaleInfo.cardData.sTranType == 'A')
							_Module.m_server.TrsLinkOperation(REC_INSERT, &cTmptrs ,NULL ,&PayAtPumpInfo);  													
						else
							_Module.m_server.TrsLinkOperation(REC_INSERT, &cTmptrs ,NULL);  													
					}
				}

				else
				{
					LOG_MSG_FUNCINC(" (trs.GetLinkFlags() & TRS_LINK_ATTEND)  = FALSE ");
					if (pumpStat.GetResExt() == RES_EXT_ATTENDANT)
							trs.m_lResExt = RES_EXT_ATTENDANT;

					trs.m_lStatus = UNPAID; // Pay At Kiosk ...
					//4.0.7.500 - setting the unpaid timeout
					long lRunningValueThreshold = _Module.m_server.GetRunningValueThreshold();

					if( (lRunningValueThreshold > 5) && trs.m_lRoundedValue / 1000 >= lRunningValueThreshold)
					{
						_LOGMSG.LogMsg("GCI::PakReadyTrs add trs flag MISC_UNPAID_TRS_TIMEOUT");//Amram remove
						trs.m_lMisc |= MISC_UNPAID_TRS_TIMEOUT;								
					}

				}
				
				if (retCode == GCI_OK)
				{//Liat LPE
					LOG_MSG_FUNCINC(" retCode == GCI_OK ");
					long lPromID = 0, lCentsOff = 0;
					long lFullServicePrice = 0, lSelfServicePrice = 0, lTotalNoDiscount = 0;
					EXTRA_INFO			cExtraInfoFlowRateAndAnpr;
					/*****************************************************/
					/*Update ANPR data + Other data*/
					/*****************************************************/
					retCode = CalculateAnprAndFlowRate(lFlags, pRec, retCode, trs, cExtraInfoFlowRateAndAnpr, lPumpNumber);

					/***********************************************/
					//Loyalty (Copient or LPE) - PAK
					/***********************************************/
					//4.0.19.508
					if (ShouldUpdatePAKExtraInfo(p ,lFlags))  //RFUEL - 2463
					 {
						LOG_MSG_FUNCINC(" ShouldUpdatePAKExtraInfo = TRUE ");
						PAY_AT_PUMP_INFO infoCheckLoyalty;
						_Module.m_server.m_cPumpArray[p].GetPAPInfo(&infoCheckLoyalty);

						if ((_Module.m_server.m_cFuelLoayaltyPS.m_dwLoyaltyEngineType == EPSILON) &&
							((ChrAll((char*)infoCheckLoyalty.CardSaleInfo.extraData.sLoyalId, sizeof(infoCheckLoyalty.CardSaleInfo.extraData.sLoyalId), ' '))))  //4.0.23.2720
						{
							if (LOG_BASIC_FLOW_CONTROL)
							{
								CString str;
								str.Format("No Loyalty information reported on transaction");
								_LOGMSG.LogMsg("GCI", "PAKReadyTransaction", lPumpNumber, LOG_PUMP, str);
							}
						}
						else 
						{   
							UpdatePakLoyaltyExtraInfo(lPumpNumber, trs, cExtraInfoFlowRateAndAnpr, lFlags, pRec, retCode, nGrade, lGradePrice, lValue, lVolume);
						}
						
						if (LOG_DETAIL_FLOW_CONTROL)	//4.0.23.220 105272 BP //4.0.24.60
						{
							CString sLogMsg; 
							sLogMsg.Format("Loyalty ----------   (Trs %d, Status %d, link Number %d)", 
								lTotalVal,
								trs.m_lNumber,
								trs.m_lStatus,
								trs.m_lLinkNumber );
							
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg); 
						}	

					} //Loyalty PAK End
					/***********************************************/
					//Rest cases
					/***********************************************/
					 else
					 {
						 /***********************************************/
						 //Convert layers that come with trs number
						 /***********************************************/
						 LOG_MSG_FUNCINC(" ShouldUpdatePAKExtraInfo = FALSE ");
						if (lFlags & PAK_INCLUDE_CONTROLLER_TRS_NUMBER) 
						{
							LOG_MSG_FUNCINC(" (lFlags & PAK_INCLUDE_CONTROLLER_TRS_NUMBER)  = TRUE ");
							PAY_AT_PUMP_INFO PayAtPumpInfo;	
							memset(&PayAtPumpInfo, ' ' , sizeof(PAY_AT_PUMP_INFO));

							trs.SetLink(TRS_LINK_EXTRA_INFO + TRS_NO_LINK,0L);
							l2a(lTotalVal ,PayAtPumpInfo.CardSaleExtraData3.sControllerSeqNum , sizeof(PayAtPumpInfo.CardSaleExtraData3.sControllerSeqNum));														
							trs.m_lControllerSeqNum = lTotalVal;	//4.0.18.660 68689 //4.0.21.502
							if (lFlags & PAK_NEED_TO_REPORT_PAID)//4.0.23.650 133966 //4.0.25.50
							{
								trs.m_sReportPaid = '1';
							}
							else
							{
								trs.m_sReportPaid = ' ';
							}
							
							_Module.m_server.CentralUpdateTrs(lPumpNumber, trs.m_lNumber ,&trs,FALSE);	//4.0.18.660 68689 //4.0.21.502
							
							/***********************************************/
							//Convert layers that come with trs number + Extra Info
							/***********************************************/
							if (lFlags & PAK_INCLUDE_EXTRA_INFO_REC)		//Currenly ANPR data			//4.0.23.500 TD 69914 //4.0.23.507
							{
								LOG_MSG_FUNCINC(" (lFlags & PAK_INCLUDE_EXTRA_INFO_REC)  = TRUE ");
								TAG_EXTRA_INFO		cTagExtraInfo;

								memset(&cTagExtraInfo, ' ', sizeof(cTagExtraInfo));								
								
								int iLen = Bstr2Buffer(pRec, (void *)&cTagExtraInfo, sizeof(cTagExtraInfo));

								if(iLen == 0)
								{
									DWORD  dwLastErr = GetLastError();
									retCode = GCI_LAST_ERROR;
								}
								else
								{
									trs.SetLink(TRS_LINK_EXTRA_INFO + TRS_NO_LINK, trs.m_lNumber);
											
									EXTRA_INFO			cExtraInfo;
									
									if(!ConvertTagRecord(cTagExtraInfo, cExtraInfo, trs))
									{									
										long				lRetCode = OK;
										EXTRA_INFO_REC		cExistExtraInfoRec;
										CString				strLogMsg, strLogPrefix;
										
										//Init 
										memset(&cExistExtraInfoRec, 0, sizeof(EXTRA_INFO_REC));

										//Update relevant fields
										memcpy(trs.m_sFullVehicleId, cExtraInfo.sFullVehicleId, sizeof(cExtraInfo.sFullVehicleId));

										//Set index for read from DB
										cExistExtraInfoRec.info.lIndexNumber = trs.m_lNumber;									
										
										long lRtcExistExtraInfo = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &cExistExtraInfoRec);
										cExtraInfo.lControllerSeqNum = lTotalVal;	//4.0.23.508 69914		

										if (TRUE == trs.IsTrsPumpTestOrCalib())		 //4.0.25.1140 TD 412765
										{
											memcpy(cExtraInfo.sReferenceNumber, cExistExtraInfoRec.info.sReferenceNumber, sizeof(cExtraInfo.sReferenceNumber));
										}

										
										//Insert or Update DB
										if(lRtcExistExtraInfo == OK)
										{	
											lRetCode = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_UPDATE, &cExtraInfo);
											
											if(LOG_DETAIL_FLOW_CONTROL) 
												strLogPrefix = "Update";																					
										}
										else
										{
											lRetCode = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_INSERT, &cExtraInfo);
										
											if(LOG_DETAIL_FLOW_CONTROL) 
												strLogPrefix = "Insert";	
										}

										if(LOG_DETAIL_FLOW_CONTROL)								
										{
											strLogMsg.Format(" extra info record with PAK transaction (Trs %d, Status %d, link Number %d, RetCode=%ld)", 									
																		trs.m_lNumber, trs.m_lStatus, trs.m_lLinkNumber, lRetCode);										

											_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strLogPrefix + strLogMsg); 
										}
											
									}
								}
							}
							else	//4.0.23.508 69914
							{
								LOG_MSG_FUNCINC(" (lFlags & PAK_INCLUDE_EXTRA_INFO_REC)  = FALSE ");
								_Module.m_server.TrsLinkOperation(REC_INSERT, &trs ,NULL ,&PayAtPumpInfo);  								 

								if(LOG_DETAIL_FLOW_CONTROL)
								{
									CString sLogMsg; 
									sLogMsg.Format("Insert extra info record with SeqNum=%ld PAK transaction   (Trs %d, Status %d, link Number %d)", 
												   lTotalVal,
												   trs.m_lNumber,
												   trs.m_lStatus,
												   trs.m_lLinkNumber );

									_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg); 
								}	
							}

						} //(PAK_INCLUDE_EXTRA_INFO_REC) + PAK_INCLUDE_CONTROLLER_TRS_NUMBER



						/******************************************************************/
						//Treatment for Petro China
						/******************************************************************/
						else if (_Module.m_server.GetIFSFTerminalSrvInUse())	//4.0.23.509 97360
						{	
							LOG_MSG_FUNCINC(" (lFlags & PAK_INCLUDE_CONTROLLER_TRS_NUMBER)  = FALSE  but  _Module.m_server.GetIFSFTerminalSrvInUse() = TRUE");
							EXTRA_INFO		cExtraInfo;
							CString str;
							BOOL bExistExtraInfo = FALSE;
							memset(&cExtraInfo,' ', sizeof(cExtraInfo));
							if (lFlags && PAK_INCLUDE_EXTRA_INFO_REC) //4.0.23.509 97360
							{
								TAG_EXTRA_INFO  cTagExtraInfo;
								memset(&cTagExtraInfo,' ', sizeof(cTagExtraInfo));

								int iLen = Bstr2Buffer(pRec, (void *)&cTagExtraInfo, sizeof(cTagExtraInfo));
								if (iLen <= 0)
								{
									str.Format("PakReadyTransaction, Failed to parse received extra info data");

								}
								else
								{
									if(!ConvertTagRecord(cTagExtraInfo,cExtraInfo , trs))
									{
										str.Format("PakReadyTransaction, received extra info data was successfully parsed");
										bExistExtraInfo = TRUE;
									}
									else
									{
										//Fail
										str.Format("PakReadyTransaction, received extra info data failed to be converted");
									}

								}
								_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 

							}
							/******************************************************************/
							//Treatment for asking promotion ("AddItem") from promotion server
							//in order to get the promotion discount amount.
							/******************************************************************/
							if (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()) //4.0.19.710 66705 For Petro China
							{
								LOG_MSG_FUNCINC(" _Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag() = TRUE");
								trs.SetLink(trs.GetLinkFlags() + TRS_LINK_ZONES ,trs.m_lNumber); //4.0.23.509 97360
								cExtraInfo.lIndexNumber = trs.m_lNumber;
								cExtraInfo.lFlag = trs.m_nLinkFlags;
								l2a(trs.m_lRoundedValue, cExtraInfo.sNonDiscountTotalAmount, sizeof(cExtraInfo.sNonDiscountTotalAmount));
								cExtraInfo.lTransactionAmount = trs.m_lRoundedValue;
								cExtraInfo.lSeqNumber = trs.m_lLinkNumber;

								//4.0.26.501 - TD 140673
								PAY_AT_PUMP_INFO cPayAtPumpInfo;
								_Module.m_server.m_cPumpArray[lPumpNumber -1].GetPAPInfo(&cPayAtPumpInfo);
								memset(cPayAtPumpInfo.CardSaleExtraData4.sPreAuthAmount, ' ' , sizeof(cPayAtPumpInfo.CardSaleExtraData4.sPreAuthAmount));
								_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cPayAtPumpInfo);
								_Module.m_server.SavePumpInfo(lPumpNumber); 

								if (trs.GetRes() == PRE_PAY_AT_POS) //4.0.25.30 111474
								{
									UpdatePapInfoWithPreAuthAmount(lPumpNumber, trs);
								}

								//Using the "Cancel Ticket with TRUE would cause loyalty to call AddItem for PAK:
								OLA_STAT ola;
								_Module.m_server.m_cFuelLoayaltyPS.AddItem(lPumpNumber, trs.m_lNumber, ola);//4.0.19.810 66705 
								bExistExtraInfo = TRUE;
							}
							if (bExistExtraInfo) //4.0.23.509 97360
							{
								LOG_MSG_FUNCINC(" bExistExtraInfo = TRUE");

								if (trs.GetRes() == PAY_AT_KIOSK)
								{
									trs.SetLink(trs.GetLinkFlags() + TRS_LINK_PAK_AND_LOYALTY + TRS_LINK_EXTRA_INFO + TRS_NO_LINK ,trs.m_lNumber); 

								}
								else if (trs.GetRes() == PRE_PAY_AT_POS)
								{
									LOG_MSG_FUNCINC(" (trs.GetRes() == PRE_PAY_AT_POS) = TRUE");
									trs.SetLink(trs.GetLinkFlags() + TRS_LINK_PREPAY_NUMBER_AND_LOYALTY + TRS_LINK_EXTRA_INFO + TRS_NO_LINK ,trs.m_lNumber); 

									long lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &trs , NULL,NULL, TRUE);//No need to update extra info since we will insert it later.
									if(lRtc)
									{
										if(LOG_LOGIC_ERROR)
										{
											CString str;
											str.Format("Update Operation for PrePay with IFSF Failed !!!  Trs %d, lRtc %d", trs.m_lNumber, lRtc ); 
											_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str,LOG_LEVEL_1);
										}
										retCode = GCI_FAILED_IO;
									}

								}
								long lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE,REC_INSERT,&cExtraInfo);
							//	_Module.m_server.TrsLinkOperation(REC_INSERT, &trs, NULL, &PayAtPumpInfo);  	
								if(LOG_BASIC_FLOW_CONTROL)
								{						
									str.Format("New record was inserted to extra info qdx, index=%ld, Flags=%c,lRtc=%ld,",cExtraInfo.lIndexNumber, cExtraInfo.lFlag,lRtc);
									_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
								}			

							}

						}	
						/****************************************************/
						// Start Prepay loyalty Or Cash acceptor + extra info
						/****************************************************/
						else if ((trs.m_lRes == PRE_PAY_AT_POS) && ((lFlags == PAK_INCLUDE_EXTRA_INFO_REC) || (lFlags & PAK_INCLUDE_EXTRA_INFO_REC_2) || (trs.GetLinkFlags() & TRS_LINK_PREPAY_NUMBER_AND_LOYALTY))) //4.0.21.500 - TD 14778	//4.0.20.610 72412 4.0.22.500
						{// Pre pay loyalty
							LOG_MSG_FUNCINC(" (lFlags & PAK_INCLUDE_CONTROLLER_TRS_NUMBER)  = FALSE  but  ((trs.m_lRes == PRE_PAY_AT_POS) && ((lFlags == PAK_INCLUDE_EXTRA_INFO_REC) || (lFlags & PAK_INCLUDE_EXTRA_INFO_REC_2) || (trs.GetLinkFlags() & TRS_LINK_PREPAY_NUMBER_AND_LOYALTY))) = TRUE");
							//4.0.24.60 105087
							UpdatePrePayExtraInfo(lPumpNumber, trs, lFlags, pRec, retCode, nGrade, lGradePrice, lValue);
						}// End Prepay loyalty Or Cash acceptor + extra info
					 }

					//4.0.9.506 //amram delete this line now we have timer
					 //TD 14287 if the timer is 0 we will return reconile trs   //4.0.10.500
					 if (MODE_VALID(trs.m_lMode) && _Module.m_server.m_cModeArray[trs.m_lMode - 1].PAKAutoReconcile())
						if (_Module.m_server.GetPAKAutoReconcileTimer()== 0)  //4.0.7.504
							retCode = GCI_PAK_RECONCILE_TRS;

					//update lowest price 438993
					PAY_AT_PUMP_INFO PayAtPumpInfo;
					_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&PayAtPumpInfo);

					if (_Module.m_server.GetPriceProtectionActive() == PRICE_PROTECTION_ACTIVE_INSIDE_TIME_FRAME)
					{
						trs.m_lLowestGradePrice = _Module.m_server.GetGradePriceFromArray(nFinalGrade, PRICE_PROTECTION_LOWEST_PRICE);
					}

					if ((PayAtPumpInfo.CardSaleExtraData5.sLoyaltyAutoArm == '1') ||
						trs.m_sLoyaltyAutoArm == '1')
					{
						if (_Module.m_server.IsAutomaticAuthForLoyaltyCustomer() == TRUE)
							trs.m_sLoyaltyAutoArm = '1';

						long lOriginalPrice = _Module.m_server.GetGradePriceFromArray(nFinalGrade, _Module.m_server.GetGradePriceProtectionLevel());

						if (((PayAtPumpInfo.CardSaleExtraData5.sPriceProtectionUsed == '1') ||
							(trs.m_sLoyaltyPriceProtection)) && 
							(lOriginalPrice > trs.m_lLowestGradePrice))
						{
							trs.m_sLoyaltyPriceProtection = '1';
						}
					}
					
					if (LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("PAKReadyTransaction (%ld) - lGradePrice %ld, LowestPrice %ld, PriceProtection %c, LoyaltyAutoArm %c", 
							lFinalTrsNumber,
							lGradePrice,
							trs.m_lLowestGradePrice,
							trs.m_sLoyaltyPriceProtection,
							trs.m_sLoyaltyAutoArm);
						_LOGMSG.LogMsg("GCI", "PAKReadyTransaction", lPumpNumber, LOG_PUMP, str);
					}

					// M.L 8/5/98 Lock PumpArray in large scope 
					_Module.m_server.CentralUpdateTrs(lPumpNumber, lFinalTrsNumber, &trs, FALSE);
					

					//4.0.18.504 Merge from 4.0.17.330 start
					//4.0.13.70	
					//In case the PrePay status changed.
					if (trs.GetLinkFlags() & TRS_LINK_PREPAY_NUMBER)	//55857 //4.0.17.330
					{
						LOG_MSG_FUNCINC(" (trs.GetLinkFlags() & TRS_LINK_PREPAY_NUMBER) = TRUE");
						if (!(trs.GetStatus() == PAID)) //4.0.21.500 - TD 59314
						{
							CPumpTransact cTmpTrs(trs);
							cTmpTrs.m_lStatus = PAID; // PrePay At Kiosk ...				
							long lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE, &cTmpTrs , NULL);
							if(lRtc)
							{
								if(LOG_LOGIC_ERROR)
								{
									CString str;
									str.Format("Update Operation for PrePay Failed !!!  Trs %d, lRtc %d", cTmpTrs.m_lNumber, lRtc ); 
									_LOGMSG.LogMsg("GCI","PAKReadyTransaction()",lPumpNumber,LOG_PUMP,str,LOG_LEVEL_1);
								}
								retCode = GCI_FAILED_IO;
							}
						}
					}
					//4.0.18.504 Merge from 4.0.17.330 end

				}

				_Module.m_server.m_cShiftLog.LogTrs(&trs, FALSE, FALSE);

			}
			else
			{
				retCode = GCI_TRS_DUPLICATE_UPDATE;
			}
		}
		else if(PUMPSRV_IS_SLAVE) // PumpSrv is SLAVE
		{
			LOG_MSG_FUNCINC(" PUMPSRV_IS_SLAVE = TRUE");
			if(trs.GetLinkFlags() == TRS_LINK_OLA_NUMBER)
				retCode = GCI_TRS_MISMATCH;
			else
			{
				// In the slave case any update received via 
				// TransactionStatusCM() , 
				// this entry should be used only for 
				// Transaction logging purposes.
				
				trs.m_lStatus			= UNPAID;
				trs.m_nNozzle			= nNozzle;
				trs.m_lRoundedVolume	= lVolume;
				trs.m_lRawVolume		= lVolume;
				trs.m_lRoundedValue		= lValue;
				trs.m_nLevel			= nLevel;
				trs.m_lGradePrice		= lGradePrice;
				trs.m_nGrade			= nFinalGrade;
				trs.UpdateTime();

				// Update Shift Number - critical when doing Non locking shift.
				trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[p].GetShiftNumber();
			
				_Module.m_server.m_cShiftLog.LogTrs(&trs,FALSE,FALSE);
			}
		}

		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetLoyaltyPromptRetry(0);

		//4.0.25.50 - TD 108311
		if ((retCode == GCI_OK) && (_Module.m_server.GetSingleShiftAccumulationsActive()) && (lVolume > 0))
		{
			//Accumulate sales and store in Single shift accumulations Qdex
			_Module.m_server.m_cSingleShiftAccumMgr.AccumulateSales(lPumpNumber, nNozzle, lVolume);
		}

		//4.0.0.76
		CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();
		long lMisc = pumpStat.GetMisc();
		BOOL lNeedUpdate = FALSE;
		if (lMisc & MISC_VIT_CONFLICT)
		{
			lMisc &=MISC_NOT_VIT_CONFLICT;
			lNeedUpdate = TRUE;
		}		
	
		
		if (lMisc & MISC_APPROVED_PAP)
		{
			lMisc &= MISC_NOT_APPROVED_PAP;	
			lNeedUpdate = TRUE;
		}
		if (lNeedUpdate)
		{
			pumpStat.SetMisc(lMisc);
			_Module.m_server.m_cPumpArray[lPumpNumber -1].SetStat(pumpStat);			
		}
		//4.0.0.76

		//4.0.19.504 - 47559
		if ((_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse()) && (retCode == GCI_OK || retCode == GCI_TRS_MISMATCH))
		{
			LOG_MSG_FUNCINC(" ((_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse()) && (retCode == GCI_OK || retCode == GCI_TRS_MISMATCH)) = TRUE");
			// Update trs info
			if (bFound)
				_Module.m_server.CentralUpdateTrs(lPumpNumber,lFinalTrsNumber,&trs,FALSE); //4.0.20.0 - TD 59750

			SALE_EXTRA_DETAILS  cSaleExtraDetails;
			memset(&cSaleExtraDetails , ' ', sizeof(SALE_EXTRA_DETAILS));
			
			PAY_AT_PUMP_INFO info;
			_Module.m_server.m_cPumpArray[p].GetPAPInfo(&info);	

			_Module.m_server.UpdateCardSaleData(lPumpNumber, lTrsNumber,  nNozzle,lVolume,lValue,nLevel,lGradePrice,  nGrade ,&info, trs , cSaleExtraDetails);
			_Module.m_server.UpdatePrePayExtraDetails(lPumpNumber, lTrsNumber, trs, cSaleExtraDetails, nGrade, lVolume, lValue, &info); //4.0.22.511 - TD 74796 //4.0.25.80 - TD 140655
			
			_Module.m_server.m_cGPIEventMgr.AddPakReadyTrs(lPumpNumber, lTrsNumber, nNozzle, lVolume, lValue, nLevel, lGradePrice, nGrade, lFlags, lTotalVal, trs.m_lResExt, cSaleExtraDetails, retCode);
		}

		SaveTrsVolumeForDeliveryEmulation(lPumpNumber, nNozzle, lVolume);		//4.0.20.52 (TD 62818)
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
//		LeaveCriticalSection(&_Module.m_server.m_csPumpArray);

		sFinalLineMsg = _LOGMSG.GetFinalLineMsg(trs.GetLinkFlags());

	}


	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"PAKReadyTransaction",
				TRUE,retCode,
				FALSE,0);

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;

		str.Format("PAKReadyTransaction(Pump %02ld, Trs %05ld, Nozzle %02d, Volume %04ld, Val %04ld, Level %d, Grade Price %04ld, Grade %02d, Flags=%ld)",
			lPumpNumber, lFinalTrsNumber, nNozzle, lVolume, lValue, nLevel, lGradePrice, nFinalGrade,lFlags);

		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);

	}

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sFinalLineMsg);

	if(pVal != NULL)
		*pVal = retCode;
}

GCIRetCode CGCI::CalculateAnprAndFlowRate(long lFlags, BSTR pRec, GCIRetCode retCode, CPumpTransact &trs, EXTRA_INFO &cExtraInfoFlowRateAndAnpr, long lPumpNumber)
{
	LOG_MSG_START_METHOD("CalculateAnprAndFlowRate");
	if (_Module.m_server.IsActiveANPR() || _Module.m_server.IsFlowRateActive()) //4.0.26.505 157073 //4.0.27.41
	{

		TAG_EXTRA_INFO		cTagExtraInfo;
		memset(&cTagExtraInfo, ' ', sizeof(cTagExtraInfo));

		int iLen = 0;

		if (lFlags & PAK_INCLUDE_EXTRA_INFO_REC)
			iLen = Bstr2Buffer(pRec, (void *)&cTagExtraInfo, sizeof(cTagExtraInfo));
		else
			iLen = 1;

		if (iLen == 0)
		{
			DWORD  dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;
			LOG_MSG_FUNCINC(" Len of buffer is 0. Flowrate will not be calculated");
		}
		else
		{
			LOG_MSG_FUNCINC(" about to calculate flowrate");
			trs.SetLink(TRS_LINK_EXTRA_INFO, trs.m_lNumber);
			memset(&cExtraInfoFlowRateAndAnpr, ' ', sizeof(cExtraInfoFlowRateAndAnpr)); //4.0.27.31 TD 224361

			if (!ConvertTagRecord(cTagExtraInfo, cExtraInfoFlowRateAndAnpr, trs))
			{
				LOG_MSG_FUNCINC(" !ConvertTagRecord(cTagExtraInfo, cExtraInfoFlowRateAndAnpr, trs) = TRUE");
				long				lRetCode = OK;
				EXTRA_INFO_REC		cExistExtraInfoRec;
				CString				strLogMsg, strLogPrefix;

				//Init 
				cExistExtraInfoRec.ClearRecord(); // RFUEL-3708

				/* Calculate and update the Flow Rate :*/
				long lFlowRate = _Module.m_server.m_cFlowRate.SetFlowRateByMinute(trs, cExtraInfoFlowRateAndAnpr);					//4.0.27.40 TD 165585


				//Update relevant fields
				memcpy(trs.m_sFullVehicleId, cExtraInfoFlowRateAndAnpr.sFullVehicleId, sizeof(cExtraInfoFlowRateAndAnpr.sFullVehicleId));
				long lANPRStatus = a2l(cTagExtraInfo.sANPRTrsStatus, sizeof(cTagExtraInfo.sANPRTrsStatus));
				switch (lANPRStatus)
				{
				case 1:
				case 2:
				{
					trs.m_lMisc |= MISC_TRS_BLOCKED_ANPR;
					break;
				}
				case 3:
				{
					trs.m_lMisc |= MISC_TRS_AUTH_WITH_FAILED_ANPR;
					break;
				}
				default:
				{

				}
				}

				//Set index for read from DB
				cExistExtraInfoRec.info.lIndexNumber = trs.m_lNumber;
				lRetCode = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_UPDATE, &cExtraInfoFlowRateAndAnpr);

				if (LOG_DETAIL_FLOW_CONTROL)
				{
					strLogMsg.Format("Update extra info record with data (Trs %d, Status %d, link Number %d, ANPR Status = %d, FlowRate = %.4s, RetCode=%ld)",
						trs.m_lNumber, trs.m_lStatus, trs.m_lLinkNumber, lANPRStatus, cExtraInfoFlowRateAndAnpr.sTrsFlowRate, lRetCode); //4.0.26.505 157073										

					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP,strLogMsg);
				}

			}
		}
	}					return retCode;
}

void CGCI::UpdatePapInfoWithPreAuthAmount(long lPumpNumber, CPumpTransact &trs) //4.0.25.30 111474
{

	PAY_AT_PUMP_INFO cPapInfo;

	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cPapInfo);

	PRE_PAY_INFO cTmpPrePayinfo;	

	long lRtc = _Module.m_server.TrsLinkOperation(REC_READ,&trs,&cTmpPrePayinfo);
	if (lRtc)
	{//Fail
		l2a(0, cPapInfo.CardSaleExtraData4.sPreAuthAmount, sizeof(cPapInfo.CardSaleExtraData4.sPreAuthAmount));
		
	}
	else
	{
		if (cTmpPrePayinfo.lFlags & LIMIT_BY_VALUE)
		{
			l2a(cTmpPrePayinfo.lMaxValue, cPapInfo.CardSaleExtraData4.sPreAuthAmount, sizeof(cPapInfo.CardSaleExtraData4.sPreAuthAmount));
		}
		else
		{
			l2a(cTmpPrePayinfo.lMaxVolume, cPapInfo.CardSaleExtraData4.sPreAuthAmount, sizeof(cPapInfo.CardSaleExtraData4.sPreAuthAmount));

		}

	}

	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&cPapInfo);


}

//4.0.23.190 start 105087 4.0.24.60
void CGCI::UpdateTotalDiscountInExtraInfoRec(long lPumpNumber, TAG_EXTRA_INFO  &cTagExtraInfo, long lGrade, long lGradePrice, CPumpTransact &trs, long lValue)
{
	long lTotalNoDiscount = 0;
	long lFullServicePrice = 0, lSelfServicePrice = 0;

	//4.0.20.141 end 66871
	//4.0.23.640 136174	_Module.m_server.m_cFuelLoayaltyPS.GetPricesByGradeCode(lPumpNumber, lGrade, &lFullServicePrice, &lSelfServicePrice);
	//4.0.20.300 68105 //4.0.21.0
	if (_Module.m_server.m_cFuelLoayaltyPS.GetLoyaltySrvInUse() == COPIENT || _Module.m_server.m_cFuelLoayaltyPS.GetLoyaltySrvInUse() == EMERALD)	//RFEUL 2424
	{
		lTotalNoDiscount = a2l(cTagExtraInfo.sNonDiscountTotalAmount, sizeof(cTagExtraInfo.sNonDiscountTotalAmount));
		lTotalNoDiscount *= 10; //RFS sends it divided by 10
		lSelfServicePrice = a2l(cTagExtraInfo.sOriginalUnitPrice, sizeof(cTagExtraInfo.sOriginalUnitPrice));
		long lTotalDiscount = lTotalNoDiscount - lValue;
		l2a(lTotalDiscount, cTagExtraInfo.sLoyaltyTotalDiscount, sizeof(cTagExtraInfo.sLoyaltyTotalDiscount));
	}
	else  // should consider removing following code as we would like to have a unite code for all systems...
	{
		lSelfServicePrice = _Module.m_server.m_cFuelLoayaltyPS.GetSavedOriginalUnitPrice(lPumpNumber, lGrade); //4.0.23.640 136174
		if (lSelfServicePrice > lGradePrice) //4.0.23.460 120915 //4.0.25.0
		{
			//old calculation -> lTotalNoDiscount = (lSelfServicePrice * trs.m_lRoundedVolume / 1000) /10 *10; //4.0.23.440 120915	// + 5) /10 * 10; //4.0.20.300 68105 //4.0.21.0
			//new calculation -> 4.0.23.460 120915
			long lTotalDiscount = (lSelfServicePrice - lGradePrice) * trs.m_lRoundedVolume / 1000;
			lTotalNoDiscount = lValue + lTotalDiscount;
			
			l2a(lTotalDiscount, cTagExtraInfo.sLoyaltyTotalDiscount, sizeof(cTagExtraInfo.sLoyaltyTotalDiscount));
		}
		else
			lTotalNoDiscount = lValue;
	}
	
	if(LOG_DETAIL_FLOW_CONTROL) //4.0.23.460 120915
	{
		CString str;

		str.Format("UpdateTotalDiscountInExtraInfoRec(Trs %05ld, Volume %04ld, Val %04ld, Original Grade Price %04ld, Total amount before discount=%ld)",
			trs.m_lNumber, trs.m_lRoundedVolume, lValue, lSelfServicePrice, lTotalNoDiscount);

		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

	}

	//	lTotalNoDiscount /= 1000;
	memset(cTagExtraInfo.sNonDiscountTotalAmount,0,sizeof(cTagExtraInfo.sNonDiscountTotalAmount));
	l2a(lTotalNoDiscount,cTagExtraInfo.sNonDiscountTotalAmount, sizeof(cTagExtraInfo.sNonDiscountTotalAmount));


	l2a(lValue, cTagExtraInfo.sTransactionAmount, sizeof(cTagExtraInfo.sTransactionAmount));
	l2a(lSelfServicePrice, cTagExtraInfo.sOriginalUnitPrice, sizeof(cTagExtraInfo.sOriginalUnitPrice));

}

void CGCI::UpdateDiscountsArray(long lPumpNumber, EXTRA_INFO_REC	&cExistExtraInfoRec, PAY_AT_PUMP_INFO &PayAtPumpInfo, TAG_EXTRA_INFO  &cTagExtraInfo, BOOL bInitializeData)
{
	char sDiscountMsg[MAX_PATH];
	if (!bInitializeData)
	{
		for(int i=0 ; i< MAX_GRADES; i++)
		{
			l2a(cExistExtraInfoRec.info.lDiscountArray[i], PayAtPumpInfo.CardSaleExtraData2.sDiscountPerGrade[i].sDiscount , sizeof(PayAtPumpInfo.CardSaleExtraData2.sDiscountPerGrade[i].sDiscount));  //4.0.20.141			 
			l2a(cExistExtraInfoRec.info.lDiscountArray[i], cTagExtraInfo.sDiscountMap[i], STR_LONG_VAL6);
			memset(sDiscountMsg, 0, sizeof(sDiscountMsg));
			sprintf_s(sDiscountMsg,MAX_PATH, "Discount for grade %d was updated to %ld", i, cExistExtraInfoRec.info.lDiscountArray[i]);
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP, sDiscountMsg); 
		}
		//4.0.20.640 72936
		memcpy(PayAtPumpInfo.CardSaleExtraData2.sPromotionNumber, cExistExtraInfoRec.info.sPromotionNumber , sizeof(cExistExtraInfoRec.info.sPromotionNumber));
		memcpy(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId , cExistExtraInfoRec.info.sLoyalId , sizeof(cExistExtraInfoRec.info.sLoyalId));
		memcpy(cTagExtraInfo.sLoyalId, cExistExtraInfoRec.info.sLoyalId, sizeof(cTagExtraInfo.sLoyalId));
		memcpy(&cTagExtraInfo.sDeferralAccepted, &cExistExtraInfoRec.info.sDeferralAccepted, sizeof(cTagExtraInfo.sDeferralAccepted));	//4.0.22.505 69344 //4.0.24.60
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Discounts and Loyalty ID were copied from saved data in extra info qdx."); 
	}
	else
	{//4.0.22.504 72936
		for(int i=0 ; i< MAX_GRADES; i++)
		{
			l2a(0, PayAtPumpInfo.CardSaleExtraData2.sDiscountPerGrade[i].sDiscount , sizeof(PayAtPumpInfo.CardSaleExtraData2.sDiscountPerGrade[i].sDiscount));  //4.0.20.141			 
		}
		//4.0.22.504 72936
		memset(PayAtPumpInfo.CardSaleExtraData2.sPromotionNumber, ' ' , sizeof(PayAtPumpInfo.CardSaleExtraData2.sPromotionNumber));
		memset(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId , ' ' , sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId));
		cTagExtraInfo.sDeferralAccepted = '0';	//4.0.22.505 69344 the user chose not to use the discount.
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Failed to read record from Extra Info file for discounts."); 

	}
}



void CGCI::UpdatePrePayExtraInfo(long lPumpNumber, CPumpTransact &cTrs, long lFlags, BSTR pRec, GCIRetCode &retCode, long lGrade, long lGradePrice, long lValue)
{
	long lPromID = 0, lCentsOff = 0;				
	TAG_EXTRA_INFO  cTagExtraInfo;
	EXTRA_INFO		cExtraInfo;
	CString str;
	BOOL bExistsTagExtraInfo = TRUE; //4.0.23.420 120915 //4.0.24.90
	memset(&cTagExtraInfo,' ', sizeof(cTagExtraInfo));

	int iLen = Bstr2Buffer(pRec, (void *)&cTagExtraInfo, sizeof(cTagExtraInfo));

	EXTRA_INFO_REC	cExistExtraInfoRec;
	long lRtcExistExtraInfo= 0;
	//4.0.20.141 start 66871
	// check for exist exatra info records.
	memset(&cExistExtraInfoRec , 0, sizeof(EXTRA_INFO_REC));
	cExistExtraInfoRec.info.lIndexNumber = cTrs.m_lNumber;
	lRtcExistExtraInfo = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &cExistExtraInfoRec);

	PAY_AT_PUMP_INFO PayAtPumpInfo;	//4.0.18.14	//4.0.18.501					 
	 _Module.m_server.m_cPumpArray[lPumpNumber -1].GetPAPInfo(&PayAtPumpInfo);
	cTrs.SetLink(cTrs.GetLinkFlags() + TRS_LINK_PREPAY_NUMBER_AND_LOYALTY + TRS_LINK_EXTRA_INFO + TRS_NO_LINK ,cTrs.m_lLinkNumber); //4.0.18.15 //4.0.24.140 127928 //4.0.28.502 - TD 263774

	BOOL bInitData = TRUE;
	if (!lRtcExistExtraInfo)	//record exists
	{
		bInitData = FALSE;	
	}

	UpdateDiscountsArray(lPumpNumber, cExistExtraInfoRec, PayAtPumpInfo, cTagExtraInfo, bInitData);


	// Print PreSet data to log
	str.Format("PreSet DATA: PreSetLimitType = %c, PreSetLimit = %.10s", cTagExtraInfo.sPreSetLimitType, cTagExtraInfo.sPreSetLimit);
	_LOGMSG.LogMsg(str);

	if(iLen == 0)
	{
		DWORD  dwLastErr = GetLastError();
		//retCode = GCI_LAST_ERROR; //4.0.23.420 120915 //4.0.24.90 
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Extra info data was not sent from CL"); //4.0.23.420 120915 //4.0.24.90
		bExistsTagExtraInfo = FALSE; //4.0.23.420 120915 //4.0.24.90
	}
//	else
//	{
	if (bExistsTagExtraInfo)	//4.0.23.420 120915 //4.0.24.90
	{

		//4.0.20.300 68105 //4.0.21.0
		memcpy(PayAtPumpInfo.CardSaleExtraData4.sLoyaltyOriginalUnitPrice, cTagExtraInfo.sOriginalUnitPrice, sizeof(cTagExtraInfo.sOriginalUnitPrice));
		memcpy(PayAtPumpInfo.CardSaleExtraData3.sNonDiscountTotalAmount, cTagExtraInfo.sNonDiscountTotalAmount, sizeof(cTagExtraInfo.sNonDiscountTotalAmount));
		memcpy(PayAtPumpInfo.CardSaleExtraData2.sTotalDiscount, cTagExtraInfo.sLoyaltyTotalDiscount, sizeof(cTagExtraInfo.sLoyaltyTotalDiscount));
		_Module.m_server.m_cPumpArray[lPumpNumber -1].SetPAPInfo(&PayAtPumpInfo);
	}
		
		if (!lRtcExistExtraInfo) //&& (cExistExtraInfoRec.info.sPromotionNumber))
		{
			memcpy(cTagExtraInfo.sPromotionNumber, cExistExtraInfoRec.info.sPromotionNumber , sizeof(cExistExtraInfoRec.info.sPromotionNumber));
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Promotion Number was copied from data in qdx."); 
		}
		else
			memset(&cTagExtraInfo.sPromotionNumber,' ', sizeof(cTagExtraInfo.sPromotionNumber));
		

		UpdateTotalDiscountInExtraInfoRec(lPumpNumber, cTagExtraInfo, lGrade, lGradePrice, cTrs, lValue);
		memcpy(PayAtPumpInfo.CardSaleExtraData3.sNonDiscountTotalAmount, cTagExtraInfo.sNonDiscountTotalAmount, sizeof(cTagExtraInfo.sNonDiscountTotalAmount));	//4.0.22.504 69344

		if((!ConvertTagRecord(cTagExtraInfo,cExtraInfo , cTrs)) && (!lRtcExistExtraInfo))	//4.0.22.504 73878 insert record only if one already exists.
		{
				long lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE,REC_UPDATE,&cExtraInfo);
				long lNonDiscountTotalAmt = a2l(cExtraInfo.sNonDiscountTotalAmount, sizeof(cExtraInfo.sNonDiscountTotalAmount));//4.0.23.410 120915 //4.0.24.90

				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString sLogMsg; 
					sLogMsg.Format("Transaction already exists, Update extra info record with Pre Pay transaction   (Trs %d, Status %d, link Number %d, RetCode=%ld, lDiscountArray = [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld],[%ld],[%ld]), TransactionAmt = %ld, NonDiscountTotalAmt = %ld, OriginalUnitPrice = %.5s, RoundedVolume = %ld", 									
								   cTrs.m_lNumber,
								   cTrs.m_lStatus,
								   cTrs.m_lLinkNumber , 
								   lRtc,
								   cExtraInfo.lDiscountArray[0], cExtraInfo.lDiscountArray[1], cExtraInfo.lDiscountArray[2], cExtraInfo.lDiscountArray[3],
								   cExtraInfo.lDiscountArray[4], cExtraInfo.lDiscountArray[5], cExtraInfo.lDiscountArray[6], cExtraInfo.lDiscountArray[7],	
								   cExtraInfo.lDiscountArray[8], cExtraInfo.lDiscountArray[9], cExtraInfo.lDiscountArray[10], cExtraInfo.lDiscountArray[11],
								   cExtraInfo.lDiscountArray[12], cExtraInfo.lDiscountArray[13], cExtraInfo.lDiscountArray[14], cExtraInfo.lDiscountArray[15], //4.0.22.504 73878
								   cExtraInfo.lTransactionAmount, lNonDiscountTotalAmt, cExtraInfo.sOriginalUnitPrice, cTrs.m_lRoundedVolume);	//4.0.23.410 120915 //4.0.24.90
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg); 
				}	
		}
		InitLoyaltyPapInfo(lPumpNumber, PayAtPumpInfo);
		_Module.m_server.m_cPumpArray[lPumpNumber -1].SetPAPInfo(&PayAtPumpInfo);
		_LOGMSG.LogMsg("PAKReadyTransaction_ , Loyalty ID was initialized in prepay"); 

//	}
					
}
void CGCI::InitLoyaltyPapInfo(long lPumpNumber, PAY_AT_PUMP_INFO &PayAtPumpInfo)
{
	_Module.m_server.m_cFuelLoayaltyPS.InitSavedLoyaltyInfo(lPumpNumber);//4.0.20.360 68429 //4.0.21.12
	memset(PayAtPumpInfo.CardSaleInfo.cardData.sAccountNumber,' ',sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sAccountNumber)); //4.0.18.15
	memset(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId,' ',sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId));
	if (_Module.m_server.m_cFuelLoayaltyPS.IsCancelTrs(lPumpNumber)) //4.0.22.504
	{
		_Module.m_server.m_cFuelLoayaltyPS.SetCancelTrs(lPumpNumber, NoCancel);
	}

}


void CGCI::UpdatePakLoyaltyExtraInfo(long lPumpNumber, CPumpTransact &cTrs, EXTRA_INFO &cExtraInfoFlowRateAndAnpr, long lFlags, BSTR pRec, GCIRetCode &retCode, long lGrade, long lGradePrice, long lValue, long lVolume)
{
	long lPromID = 0, lFullServicePrice = 0, lSelfServicePrice = 0;
	double fCentsOff = 0 ,  fTotalNoDiscount = 0;

	 PAY_AT_PUMP_INFO PayAtPumpInfo;						 
	 _Module.m_server.m_cPumpArray[lPumpNumber -1].GetPAPInfo(&PayAtPumpInfo);

	 cTrs.SetLink(TRS_LINK_PAK_AND_LOYALTY + TRS_NO_LINK,0L);		
	//3.1.1.70
	PayAtPumpInfo.CardSaleInfo.cardData.sTranType = ' ';
	_Module.m_server.m_cPumpArray[lPumpNumber -1].SetPAPInfo(&PayAtPumpInfo);
	_Module.m_server.SavePumpInfo(lPumpNumber); // save ola stat.

	{
		CString str;
		str.Format("CGCI::UpdatePakLoyaltyExtraInfo() - sTranType = %c", PayAtPumpInfo.CardSaleInfo.cardData.sTranType);
		_LOGMSG.LogMsg(str);
	}

	if ((lFlags & PAK_INCLUDE_EXTRA_INFO_REC) ||  (_Module.m_server.m_cPumpArray[lPumpNumber -1].IsPapToPakTrs()))//4.0.10.11 && (!trs.GetLinkFlags())) //4.0.23.220 105272 BP //4.0.24.60 // TD 381791
	{
		// Save promId for POS						
		TAG_EXTRA_INFO  cTagExtraInfo;  // cTagExtraInfoFromRFS
		EXTRA_INFO		cExtraInfo;
		CString sExtraInfoXml;
		CString str;
		int iLen = 0;					//4.0.23.220 105272 BP //4.0.24.60
		TAG_DISCOUNT_LOYALTY_INFO4	cTagLoyaltyRec;
		memset(&cTagLoyaltyRec, ' ', sizeof(cTagLoyaltyRec));
//							long lPromID = 0, lCentsOff = 0;
		memset(&cTagExtraInfo,' ', sizeof(cTagExtraInfo));
		memset(&cExtraInfo, ' ', sizeof(cExtraInfo));

		if (pRec)						// Marikdebug //4.0.23.220 105272 BP //4.0.24.60
			iLen = Bstr2Buffer(pRec, (void *)&cTagExtraInfo, sizeof(cTagExtraInfo));
		else
			iLen = 1;

		
		//4.0.18.501 trs.SetLink(TRS_LINK_EXTRA_INFO + TRS_NO_LINK,0L);
		cTrs.SetLink(TRS_LINK_PAK_AND_LOYALTY + TRS_LINK_EXTRA_INFO + TRS_NO_LINK,0L);
				
		// Print PreSet data to log
		str.Format("CGCI::UpdatePakLoyaltyExtraInfo() - PreSet DATA: PreSetLimitType = %c, PreSetLimit = %.10s", cTagExtraInfo.sPreSetLimitType, cTagExtraInfo.sPreSetLimit); 
		_LOGMSG.LogMsg(str);
		
		if(iLen == 0)
		{
			DWORD  dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;
		}
		else
		{//4.0.21.0 68105
			memcpy(PayAtPumpInfo.CardSaleExtraData4.sLoyaltyOriginalUnitPrice, cTagExtraInfo.sOriginalUnitPrice, sizeof(cTagExtraInfo.sOriginalUnitPrice));
			memcpy(PayAtPumpInfo.CardSaleExtraData3.sNonDiscountTotalAmount, cTagExtraInfo.sNonDiscountTotalAmount, sizeof(cTagExtraInfo.sNonDiscountTotalAmount));
			memcpy(PayAtPumpInfo.CardSaleExtraData2.sTotalDiscount, cTagExtraInfo.sLoyaltyTotalDiscount, sizeof(cTagExtraInfo.sLoyaltyTotalDiscount));

			if (ChrAll((char*)cTagExtraInfo.sTrack2Data, sizeof(cTagExtraInfo.sTrack2Data), ' ')) // 4.0.23.1250
			{
				if (!ChrAll((char*)PayAtPumpInfo.CardSaleInfo.cardData.sClubCardTrack2Data, sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sClubCardTrack2Data), ' ')) //4.0.23.900 223830
				{
					memcpy(cTagExtraInfo.sTrack2Data, PayAtPumpInfo.CardSaleInfo.cardData.sClubCardTrack2Data, sizeof(cTagExtraInfo.sTrack2Data));
				}
				else if (!ChrAll((char*)PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId, sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId), ' ')) //4.0.23.?? 306140
				{
					memcpy(cTagExtraInfo.sTrack2Data, PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId, min(sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId), sizeof(cTagExtraInfo.sTrack2Data)));
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString sLogMsg; 
						sLogMsg.Format("UpdatePakLoyaltyExtraInfo() - loyalty ID was copied to track2");
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg); 
					}	
				
				}
			}
			if (ChrAll((char*)cTagExtraInfo.sLoyalId, sizeof(cTagExtraInfo.sLoyalId), ' ') && 
				(!ChrAll((char*)PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId, sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId), ' '))) //4.0.23.?? 306140
			{
				memcpy(cTagExtraInfo.sLoyalId, PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId, min(sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId), sizeof(cTagExtraInfo.sTrack2Data)));
				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString sLogMsg; 
					sLogMsg.Format("UpdatePakLoyaltyExtraInfo() - loyalty ID was copied to ExtraInfo");
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg); 
				}	
			}

			memcpy(cTagExtraInfo.sClubType, PayAtPumpInfo.CardSaleExtraData6.sClubType, sizeof(cTagExtraInfo.sClubType));

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString sLogMsg;
				sLogMsg.Format("UpdatePakLoyaltyExtraInfo() - Loyalty id = %.20s, club = %.2s", 
					PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId, 
					PayAtPumpInfo.CardSaleExtraData6.sClubType);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sLogMsg);
			}

			CopyLoyaltyLockingKeyToExtraInfo(cTagExtraInfo, PayAtPumpInfo, lPumpNumber);// TD 425758
			_Module.m_server.m_cPumpArray[lPumpNumber -1].SetPAPInfo(&PayAtPumpInfo);

			_Module.m_server.m_cFuelLoayaltyPS.GetPromotionId(lGrade,lPumpNumber-1,&lPromID, &fCentsOff);  
			memset(cTagExtraInfo.sPromotionNumber,0,sizeof(cTagExtraInfo.sPromotionNumber));
			if (lPromID != 0)
			{
				l2a(lPromID, cTagExtraInfo.sPromotionNumber, sizeof(cTagExtraInfo.sPromotionNumber));  
				l2a(lPromID, PayAtPumpInfo.CardSaleExtraData2.sPromotionNumber, sizeof(PayAtPumpInfo.CardSaleExtraData2.sPromotionNumber)); //4.0.20.40 63627
			}
			else
			{
				memset(&cTagExtraInfo.sPromotionNumber,' ', sizeof(cTagExtraInfo.sPromotionNumber));
				memset(&PayAtPumpInfo.CardSaleExtraData2.sPromotionNumber,' ', sizeof(PayAtPumpInfo.CardSaleExtraData2.sPromotionNumber)); //4.0.20.40 63627
			}

			UpdateTotalDiscountInExtraInfoRec(lPumpNumber, cTagExtraInfo, lGrade, lGradePrice, cTrs, lValue);
			memcpy(PayAtPumpInfo.CardSaleExtraData3.sNonDiscountTotalAmount, cTagExtraInfo.sNonDiscountTotalAmount, sizeof(cTagExtraInfo.sNonDiscountTotalAmount));	//4.0.22.504 69344
			memcpy(PayAtPumpInfo.CardSaleExtraData2.sTotalDiscount, cTagExtraInfo.sLoyaltyTotalDiscount, sizeof(cTagExtraInfo.sLoyaltyTotalDiscount));	
				
			cTagExtraInfo.sDeferralAccepted = '0';	
			PayAtPumpInfo.CardSaleExtraData4.sDeferralAccepted = '0';
			if (!_Module.m_server.m_cFuelLoayaltyPS.IsDeferralDiscountSelected(lPumpNumber))
			{
				cTagExtraInfo.sDeferralAccepted = '1';
				PayAtPumpInfo.CardSaleExtraData4.sDeferralAccepted = '1';
			}

			SetPromotionId(cTrs, cTagExtraInfo, PayAtPumpInfo); // RFUEL-2501

			if (TRUE == _Module.m_server.m_cPumpArray[lPumpNumber - 1].IsPapToPakTrs())
			{
				memcpy(cTagExtraInfo.sANPRTrsStatus, cExtraInfoFlowRateAndAnpr.sANPRTrsStatus, sizeof(cExtraInfoFlowRateAndAnpr.sANPRTrsStatus));
				memcpy(cTagExtraInfo.sTrsFlowRate, cExtraInfoFlowRateAndAnpr.sTrsFlowRate, sizeof(cExtraInfoFlowRateAndAnpr.sTrsFlowRate));
				SavePakLoyaltyExtraInfoXml(cTagExtraInfo, &cTrs, lPumpNumber, &sExtraInfoXml, lGrade, lGradePrice, lValue, lVolume);
			}
			
			if(OK == ConvertTagRecord(cTagExtraInfo,cExtraInfo , cTrs))
			{

				long lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_UPDATE,&cExtraInfo, &sExtraInfoXml);
				
				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString sLogMsg; 
					sLogMsg.Format("Insert extra info record with PAK transaction   (Trs %d, Status %d, link Number %d, RetCode=%ld)", 									
						cTrs.m_lNumber,
						cTrs.m_lStatus,
						cTrs.m_lLinkNumber , 
						lRtc);
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg); 
				}	
				
				//4.0.12.515
				//If the record already exist in extra info qdx - update it. 
				if (lRtc == ERR_EXISTS)
				{
					lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE,REC_UPDATE,&cExtraInfo);
					
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString sLogMsg; 
						sLogMsg.Format("Transaction already exist, Update extra info record with PAK transaction   (Trs %d, Status %d, link Number %d, RetCode=%ld)", 									
							cTrs.m_lNumber,
							cTrs.m_lStatus,
							cTrs.m_lLinkNumber , 
							lRtc);
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg); 
					}	
				}
			}
		}	
	}

	//4.0.18.501 merge from 1018 4.0.18.14
	_Module.m_server.TrsLinkOperation(REC_INSERT, &cTrs ,NULL ,&PayAtPumpInfo);  //4.0.18.14	//4.0.18.15
	CPumpTransact tmpLoyaltyTrs;	//4.0.20.56 64903 
	if(!_Module.m_server.m_cPumpArray[lPumpNumber-1].GetInProgressWaitProgressTrs(&tmpLoyaltyTrs))	//Found
	{
		//Updating last values of PAK trs
		tmpLoyaltyTrs.m_lRoundedValue = lValue;
		tmpLoyaltyTrs.m_lRoundedVolume = lVolume;
		tmpLoyaltyTrs.m_nGrade = (SHORT) lGrade;
		tmpLoyaltyTrs.m_lGradePrice = lGradePrice;

		_Module.m_server.CentralUpdateTrs(lPumpNumber, cTrs.m_lNumber, &tmpLoyaltyTrs,FALSE);
	}
	else
	{
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "PAKReadyTransaction , current trs wasn't found"); 
	//No trs found
	}

	if (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag() && _Module.m_server.m_cFuelLoayaltyPS.GetLoyaltySrvInUse() == LPE)  //MarikDebug //4.0.23.220 105272 BP //4.0.24.60  // Need to move to LPE executor
	{

		Convertl2Str2(lValue ,PayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt_Msb) , PayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt, sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt));
		_Module.m_server.m_cPumpArray[lPumpNumber -1].SetPAPInfo(&PayAtPumpInfo);
		_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber, TRUE);

		InitLoyaltyPapInfo(lPumpNumber, PayAtPumpInfo);
		_Module.m_server.m_cPumpArray[lPumpNumber -1].SetPAPInfo(&PayAtPumpInfo);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "CancelTicket , Loyalty ID was initialized in PAK"); 
	}
	else 
	{
		_Module.m_server.ClearLoyaltyInfo(lPumpNumber);  //4.0.28.503 - TD 263766
		
	}

}

// RFUEL-520 - SNash/Martins - Flows change to support PapToPak with Emerald POS
void CGCI::SavePakLoyaltyExtraInfoXml(TAG_EXTRA_INFO  &cTagExtraInfo, CPumpTransact *cTrs, long lPumpNumber, CString *sExtraInfoXml, long lGrade, long lGradePrice, long lValue, long lVolume) 
{
	CXMLInterface cXmlPak, cLoyaltyXml, cXmlCustomer;
	CXMLPointers pRoot, pLoyaltyInfo, pTriggerProm, pItemElement, pItem, pItemList, pCustomerInfo;
	PumpItems	cPumpItemsGrades;
	PAY_AT_PUMP_INFO  cPapInfo;
	long  lTagRecSize = _Module.m_server.m_cCommonFunction.GetTagRecSize(QEI_STRUCT_IS_LOYALTY_INFO4);
	BYTE	*pTmpTagRec = NULL;
	pTmpTagRec = new BYTE[lTagRecSize + 2];
	char sXmlExtBuff[MAX_XML_BUF_LEN];
	memset(&sXmlExtBuff, 0, MAX_XML_BUF_LEN);

	long lPromID = 0, lTrsNum = cTrs->m_lNumber, lTotalDiscount = 0;
	long lFullServicePrice = 0, lSelfServicePrice = 0, lTotalNoDiscount = 0;

	char sXmlBuffOut[MAX_XML_BUF_LEN];
	memset(&sXmlBuffOut, 0, sizeof(sXmlBuffOut));
	
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cPapInfo);

	lTotalDiscount = a2l(cPapInfo.CardSaleExtraData2.sTotalDiscount, sizeof(cPapInfo.CardSaleExtraData2.sTotalDiscount));
	

	cTagExtraInfo.sDeferralAccepted = '0';
	if (!_Module.m_server.m_cFuelLoayaltyPS.IsDeferralDiscountSelected(lPumpNumber))	//4.0.22.505 69344
	{
		cTagExtraInfo.sDeferralAccepted = '1';
	}

	long lErrorCode = _Module.m_server.m_cFlowControlManager.GetEmeraldLoyaltyErrorCode();   // RFUEl 797
	l2a(lErrorCode, cTagExtraInfo.sLoyaltyServerResultCode, sizeof(cTagExtraInfo.sLoyaltyServerResultCode));	

	
	//RFUEL-3309 Get the maximal discount allowed for the grade 
	long lMaxAllowedDiscount = _Module.m_server.GetMaximalDiscountForGrade((short)lGrade);
	l2a(lMaxAllowedDiscount, cPapInfo.CardSaleExtraData6.sMaxDiscountAllowed, sizeof(cPapInfo.CardSaleExtraData6.sMaxDiscountAllowed));
	memcpy(cTagExtraInfo.sMaxDiscountAllowed, cPapInfo.CardSaleExtraData6.sMaxDiscountAllowed, sizeof(cTagExtraInfo.sMaxDiscountAllowed));

	memcpy(cTagExtraInfo.sTransactionId, cPapInfo.CardSaleExtraData6.sTransactionId, sizeof(cTagExtraInfo.sTransactionId));
	if (_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml(sXmlBuffOut, &cTagExtraInfo))  
	{	
		_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber, cPumpItemsGrades);
		CComBSTR bstrOutXml;
		_Module.m_server.m_cFuelLoayaltyPS.CreateAddItemEmeraldXML(bstrOutXml, cPumpItemsGrades, lPumpNumber, *cTrs, lTotalDiscount);
		
		
		cXmlPak.InitDoc();
		cXmlPak.LoadXmlString(sXmlBuffOut);
		
		cLoyaltyXml.InitDoc();
		_bstr_t bstInXMLLoyal(bstrOutXml, true); // loyalty xml
		cLoyaltyXml.LoadXmlString(bstInXMLLoyal);
		BOOL bRetCode = cXmlPak.GetRootToElement(&pRoot);
		if (bRetCode)
			bRetCode = cLoyaltyXml.GetRootToElement(&pLoyaltyInfo);
		if (bRetCode)
		{
			cXmlPak.GetChildNodesList(&pLoyaltyInfo, &pItemList);

			while (cLoyaltyXml.GetNextItem(pItemList, pItem))
			{
				cXmlPak.AppendChild(&pItem, &pRoot);
			}
		}
		
		if (bRetCode)
		{
			cXmlPak.GetXmlString(sXmlExtBuff);
			sExtraInfoXml->Format("%s", sXmlExtBuff);
	
			
#ifdef DEBUG
			//cXmlPak.SaveToFile("c:\\office\\pumpsrv\\log\\ExtraLoyalty_output.xml");
#endif // DEBUG
			cXmlPak.CloseDoc();
		}
	}
}

//4.0.23.190 end 105087 4.0.24.60

STDMETHODIMP CGCI::PAPReadyTransaction(long lPumpNumber,long lSeqNumber, long lTrsNumber, short nNozzle, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long lFlags, BSTR sData, BSTR *sRetData, long * pVal)
{
//	_LOGMSG.LogLocationIn("GCI::PAPReadyTransaction()",0);

	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif

		PAPReadyTransaction__(lPumpNumber,lSeqNumber,lTrsNumber,nNozzle,lVolume,lValue,nLevel,lGradePrice,nGrade,lFlags,sData,sRetData,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::PAPReadyTransaction")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
//			_LOGMSG.LogMsg("GCI::PAPReadyTransaction()", LOG_LEVEL_0);
			_LOGMSG.LogClassMsg("GCI","PAPReadyTransaction()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}		
#endif

//	_LOGMSG.LogLocationOut("GCI::PAPReadyTransaction()",0);
	return S_OK;
}

//48194
void CGCI::ReadyOfflineTRS__(long lPumpNumber, long lFlags, BSTR sData, BSTR sParam1, long lParam2, long lParam3, BSTR *sRetData, long *pVal)	
{
	GCIRetCode	retCode = GCI_OK;
	DWORD		dwLastErr = 0;
	CXMLInterface cXMLInterface;
	char sXmlBuff[MAX_XML_BUF_LEN];
	memset(&sXmlBuff,0,MAX_XML_BUF_LEN);
	BOOL bIsOfflineTrs = FALSE;
	int iLen =0;
	
	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	//Convert layer sent us xml format
	if ((_Module.m_server.m_bUseXmlBuffer) && (retCode == GCI_OK) && !(lFlags & STRUCT_IS_OFFLINE_PUMP_TRANSACT))  //4.0.20.54 - start 46943 //4.0.24.10 TD 69655
	{
		iLen = Bstr2Buffer(sData, (void *)&sXmlBuff,MAX_XML_BUF_LEN);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("lFlags=%ld ,iLen=%d, last error= %ld retCode=%d (xml)", lFlags ,iLen , dwLastErr ,retCode);
				_LOGMSG.LogMsg("GCI","ReadyOfflineTRS__",lPumpNumber,LOG_PUMP,str);
			}	
		}//4.0.20.54 - end

		if (retCode == GCI_OK)
		{
			if (cXMLInterface.InitDoc())
			{
				
				if (sXmlBuff[0])
				{
					if (!cXMLInterface.LoadXmlString(sXmlBuff))
					{
						retCode = GCI_FAILED_IO; 
						_LOGMSG.LogXmlOLABufferMsg("XmlProblem: ReadyOfflineTRS__, LoadXmlString",&cXMLInterface,LOG_LEVEL_1);  // TD 389915
					}
				}
				else
				{
					_LOGMSG.LogMsg("XmlProblem: ReadyOfflineTRS__, Empty buffer",LOG_LEVEL_1);
					retCode = GCI_FAILED_IO; 
				}
			}
			else
			{
				_LOGMSG.LogMsg("XmlProblem: ReadyOfflineTRS__, InitDoc of XmlInterface",LOG_LEVEL_1);
				retCode = GCI_FAILED_IO; 
			}

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("ReadyOfflineTRS - lFlags=%ld ,ilen=%d (xml) ", lFlags ,iLen );
				_LOGMSG.LogMsg("GCI","ReadyOfflineTRS__",lPumpNumber,LOG_PUMP,str);
			}
		}

		if (retCode == GCI_OK)
		{
			CPumpTransact		trs(lPumpNumber);
			long p = lPumpNumber - 1;
			trs.m_lNumber = _Module.m_server.NewTransactNumber();
			trs.m_lStatus = IN_PROGRESS;
			trs.m_nLinkFlags = TRS_NO_LINK;
			trs.m_lRes = PAY_AT_PUMP;
			trs.m_lResExt = NULL_RES_EXT;
			trs.m_lActiveReadNumber = _Module.m_server.GetActiveReadNumber();
			trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[p].GetShiftNumber();
			trs.m_lPump = lPumpNumber;
			trs.m_lLinkNumber = cXMLInterface.GetXmlLongField("CardData\\ControllerSeqNum"); //4.0.18.180
			trs.m_nNozzle = (short)cXMLInterface.GetXmlLongField("Transaction\\TRNozzle");
			trs.m_lRoundedValue = cXMLInterface.GetXmlLongField("Transaction\\RoundedValue");
			trs.m_lRoundedVolume = cXMLInterface.GetXmlLongField("Transaction\\RoundedVolume");	//4.0.18.180
			trs.m_nLevel = (short)cXMLInterface.GetXmlLongField("Transaction\\Level");
			trs.m_lGradePrice = cXMLInterface.GetXmlLongField("Transaction\\TRGradePrice");
			trs.m_nGrade = (short)cXMLInterface.GetXmlLongField("Transaction\\Grade");
			trs.UpdateTime();	//To check the time
			trs.UpdateStartTime();	
			trs.UpdateStartFuelingTime();	//4.0.22.511 72606


			/********************************/
			//Mark the transaction as offline
			/********************************/
			bIsOfflineTrs = TRUE;

			_Module.m_server.CentralInsertTrs(lPumpNumber,&trs);			
			PAPReadyTransaction__(lPumpNumber, trs.GetLinkNumber(), trs.m_lNumber, trs.m_nNozzle, trs.m_lRoundedVolume, trs.m_lRoundedValue, trs.m_nLevel, trs.m_lGradePrice, trs.m_nGrade, lFlags, sData, sRetData, pVal, bIsOfflineTrs); //4.0.5.0
		}

		cXMLInterface.CloseDoc();
	}
	else 	//Convert layer sent us Structure format //4.0.20.54 - start 46943
	{
		if((lFlags & STRUCT_IS_OFFLINE_PUMP_TRANSACT) && (retCode == GCI_OK))
		{
			TAG_OFFLINE_PUMP_TRANSACT cOfflinePumpTrs;
			memset(&cOfflinePumpTrs,0x00,sizeof(cOfflinePumpTrs));

			iLen = Bstr2Buffer(sData, (void *)&cOfflinePumpTrs,sizeof(cOfflinePumpTrs));
			if(iLen == 0)
			{
				dwLastErr = GetLastError(); 
				retCode = GCI_LAST_ERROR;

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("lFlags=%ld ,ilen=%d, last error= %ld retCode=%d ", lFlags ,iLen , dwLastErr ,retCode);
					_LOGMSG.LogMsg("GCI","ReadyOfflineTRS__",lPumpNumber,LOG_PUMP,str);
				}	
			}
			
			if (retCode == GCI_OK)
			{
				CPumpTransact		trs(lPumpNumber);
				long p = lPumpNumber - 1;
				trs.m_lNumber = _Module.m_server.NewTransactNumber();
				trs.m_lStatus = PAID;
				trs.m_nLinkFlags = TRS_NO_LINK;
				trs.m_lRes = PAY_AT_PUMP;
				trs.m_lResExt = NULL_RES_EXT;
				trs.m_lActiveReadNumber = _Module.m_server.GetActiveReadNumber();
				trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[p].GetShiftNumber();
				trs.m_lPump = lPumpNumber;
				trs.m_lLinkNumber =    a2l(cOfflinePumpTrs.sControllerSeqNum,sizeof(cOfflinePumpTrs.sControllerSeqNum));
				trs.m_nNozzle =		   (short)a2l(cOfflinePumpTrs.sNozzle,sizeof(cOfflinePumpTrs.sNozzle));
				trs.m_lRoundedValue =  a2l(cOfflinePumpTrs.sRoundedValue,sizeof(cOfflinePumpTrs.sRoundedValue));
				trs.m_lRoundedVolume = a2l(cOfflinePumpTrs.sRoundedVolume,sizeof(cOfflinePumpTrs.sRoundedVolume));	
				trs.m_nLevel =		   (short)a2l(cOfflinePumpTrs.sLevel,sizeof(cOfflinePumpTrs.sLevel));
				trs.m_lGradePrice =	   a2l(cOfflinePumpTrs.sGradePrice,sizeof(cOfflinePumpTrs.sGradePrice));
				trs.m_nGrade =		   (short)a2l(cOfflinePumpTrs.sGrade,sizeof(cOfflinePumpTrs.sGrade));
				trs.UpdateTime();	//To check the time
				

				SYSTEMTIME sysStartTime;
				sysStartTime.wYear  = a2i(cOfflinePumpTrs.sYear,sizeof(cOfflinePumpTrs.sYear) );
				sysStartTime.wMonth = a2i(cOfflinePumpTrs.sMonth,sizeof(cOfflinePumpTrs.sMonth) );
				sysStartTime.wDay   = a2i(cOfflinePumpTrs.sDay,sizeof(cOfflinePumpTrs.sDay) );
				sysStartTime.wHour  = a2i(cOfflinePumpTrs.sHour,sizeof(cOfflinePumpTrs.sHour) );
				sysStartTime.wMinute= a2i(cOfflinePumpTrs.sMinute,sizeof(cOfflinePumpTrs.sMinute) );
				sysStartTime.wSecond= a2i(cOfflinePumpTrs.sSecond,sizeof(cOfflinePumpTrs.sSecond) );
				trs.SetStartTime(sysStartTime);
		
								
				PAY_AT_PUMP_INFO	info;
				memset(&info  , ' ' , sizeof(PAY_AT_PUMP_INFO)); 
				 
				info.CardSaleInfo.lIndexNumber = _Module.m_server.NewOLAIndex();
				l2a0(lPumpNumber, info.CardSaleInfo.cardData.sDeviceNumber,sizeof(info.CardSaleInfo.cardData.sDeviceNumber));
				l2a0(trs.m_lNumber, info.CardSaleInfo.cardData.sTranNbr,sizeof(info.CardSaleInfo.cardData.sTranNbr));
				memcpy(info.CardSaleExtraData3.sControllerSeqNum,cOfflinePumpTrs.sControllerSeqNum,sizeof(cOfflinePumpTrs.sControllerSeqNum));
				memcpy(info.CardSaleInfo.cardData.sTrack2Data,cOfflinePumpTrs.sControllerSeqNum,sizeof(cOfflinePumpTrs.sControllerSeqNum));
				l2a0(lPumpNumber, info.CardSaleInfo.cardData.sPumpNumber,sizeof(info.CardSaleInfo.cardData.sPumpNumber)); //4.0.19.201
				l2a(trs.m_nNozzle,&info.CardSaleInfo.cardData.sNozzle,sizeof(info.CardSaleInfo.cardData.sNozzle));

				Convertl2Str2(trs.m_lRoundedValue , 
					info.CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(info.CardSaleInfo.extraData.sTotalAmt_Msb) , 
					info.CardSaleInfo.extraData.sTotalAmt, sizeof(info.CardSaleInfo.extraData.sTotalAmt));		//4.0.19.201
		
				Convertl2Str2(trs.m_lRoundedValue /*/10 */,    //4.0.24.10 - TD 91376 - do not devide by 10.
					info.CardSaleInfo.extraData.sFuelAmt_Msb,sizeof(info.CardSaleInfo.extraData.sFuelAmt_Msb) , 
					info.CardSaleInfo.extraData.sFuelAmt, sizeof(info.CardSaleInfo.extraData.sFuelAmt));	 //4.0.19.201

				info.CardSaleExtraData4.sIsOfflineTrs = '1';

				bIsOfflineTrs = TRUE;

				//Insert to Ola qdx											
				trs.SetLink(TRS_LINK_OLA_NUMBER, info.CardSaleInfo.lIndexNumber);
				long lRtc = _Module.m_server.TrsLinkOperationEx(REC_INSERT,&trs, &info);

				if(lRtc)
				{
					retCode = GCI_FAILED_IO;
				}
				else //success
				{
					//We mark a global flag that there is at least one offline trs in the current shift
					//This will later be asked in CTRL SetShif method
					if (_Module.m_server.GetOfflineTrsInShift() == FALSE) //only once
					{
						_Module.m_server.SetOfflineTrsInShift(TRUE); 
						_PARAM.SaveParam("GeneralParam","OfflineTrsInShift",1,FALSE,0,0,0,TRUE);	
					}				

					//Throw event to VP to take the trs:
					_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_PAP_TRS,0);
				}
			}	
		}		
	} //44.0.20.54 - end
	

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"ReadyOfflineTRS",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);


	if(LOG_DETAIL_FLOW_CONTROL)
	{		
		CString str;
		str.Format("ReadyOfflineTRS - Flags=%ld, DateLen = %ld, lParam2 = %ld ,retCode=%ld ", lFlags ,iLen ,lParam2, retCode);
		_LOGMSG.LogMsg("GCI","ReadyOfflineTRS__",lPumpNumber,LOG_PUMP,str);

	}

	if(pVal != NULL)
		*pVal = retCode;
}

void CGCI::PAPReadyTransaction__(long lPumpNumber, long lSeqNumber,long lTrsNumber, short nNozzle,long lVolume,long lValue,short nLevel,long lGradePrice,  short nGrade, long lFlags, BSTR sData,  BSTR *sRetData, long *pVal,BOOL bIsOfflineTrs /*= FALSE*/)
{
	CString		sMsg;
	short		nFinalGrade = nGrade;
	DWORD		dwLastErr = 0;
	GCIRetCode	retCode = GCI_OK;
	long		lOlaRecordSize = sizeof(CARD_SALE_ALL2);
	CString		sFinalLineMsg("End of transaction");
	long		lCarWashAmt = 0 ;    //4.0.13.501	
	BOOL		bRet;
	RECEIPT_INFO cReceiptInfo;	//4.0.30.0 270322

	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	else if ((_Module.m_server.IsServerInRecoveryProcess() || (!_Module.m_server.m_cOLASrv->Valid() && OLA_BY_PUMPSRV && (!_Module.m_server.GetIFSFTerminalSrvInUse() && _Module.m_server.IsFuelMobileSrvInUse() == FALSE)))  //TD 65165 4.0.20.500 //4.0.22.73 - TD 74795
			&& _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess && !bIsOfflineTrs) //4.0.24.10 TD 69655
		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS; 

	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if(GRADE_NOT_VALID(nGrade))
		retCode = GCI_INVALID_GRADE_NUMBER;

	else if(LEVEL_NOT_VALID(nLevel))
		retCode = GCI_INVALID_LEVEL_NUMBER;

	else if(NOZZLE_NOT_VALID(nNozzle))
		retCode = GCI_INVALID_NOZZLE_NUMBER;

	else if (PUMP_PAP_CANCELED(lPumpNumber))
	{
		long p = lPumpNumber-1;
		CPumpTransact trs;

		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if(!_Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&trs))
		{	
			long lTrsNumber = trs.m_lNumber;
			if((lVolume == 0) && (lValue == 0))
			{
				trs.Clear();
				_Module.m_server.CentralUpdateTrs(lPumpNumber,lTrsNumber,&trs,FALSE);
			}
			else
			{
				trs.m_lRes = PAY_AT_KIOSK;
				trs.SetLink(TRS_NO_LINK,0L);
				_Module.m_server.CentralUpdateTrs(lPumpNumber,lTrsNumber,&trs,FALSE);
				PAKReadyTransaction__(lPumpNumber,lTrsNumber, nNozzle, lVolume, lValue, nLevel, lGradePrice, nGrade,pVal);

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("Trs %dWas converted from PAP to PAK",lPumpNumber,lTrsNumber);
					_LOGMSG.LogMsg("GCI","PAPReadyTransaction",lPumpNumber,LOG_PUMP,str);
				}

				OLA_STAT ola;
				PAY_AT_PUMP_INFO info;

				_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
				_Module.m_server.m_cPumpArray[p].GetPAPInfo(&info);

				_Module.m_server.SendSwipeResult(ola.m_byState,p+1,CD_FORCE_NOT_OK,info.CardSaleInfo.cardData.sTranStatus, (CARD_SALE_ALL3 *)&info.CardSaleInfo.cardData);

				retCode = GCI_TRS_CONVERTED_TO_PAK;
			}
		}
		
		CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[p].GetStat();
		long lMisc = pumpStat.GetMisc();
		if(lMisc & MISC_CURRENT_TRS_PAP_CANCELED)
		{                    			
			lMisc &= MISC_NOT_CURRENT_TRS_PAP_CANCELED;
			pumpStat.SetMisc(lMisc);
			_Module.m_server.m_cPumpArray[p].SetStat(pumpStat);

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Hot Pump Flag Clear for value=0 & Volume=0 ");
				_LOGMSG.LogMsg("GCI","PAPReadyTransaction()",p + 1,LOG_PUMP,str);
			}	
		}		
		
		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}

	//4.0.3.65
	// When we are working with secondary machine,
	// accept final transaction with missing parameter
	
	if ((_HOST.m_lHydraActive && (!lSeqNumber || !lTrsNumber)) && (!(STRUCT_IS_PAP_CANCEL & lFlags))) //0
	{
		long p = lPumpNumber-1;
		CPumpTransact trs;

		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if(!_Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&trs))
		{	
			lSeqNumber = trs.GetLinkNumber();  //4.0.9.512
			lTrsNumber = trs.m_lNumber;

			if(LOG_BASIC_FLOW_CONTROL)
			{
				sMsg.Format("fill missing final transaction , lSeqNumber=%ld,lTrsNumber=%ld", lSeqNumber, lTrsNumber );		
				_LOGMSG.LogMsg("GCI","PAPReadyTransaction()",lPumpNumber,LOG_PUMP,sMsg);
			}
		}

		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}
	
	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("lFlags=%ld start=%d  ", lFlags ,retCode );
		_LOGMSG.LogMsg("GCI","PAPReadyTransaction",lPumpNumber,LOG_PUMP,str);
	}	

	//4.0.7.503
	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	if(retCode == GCI_OK)
	{
		// Save pump reservation and mode while transaction was 
		// In Progress 

		CPumpTransact		trs;
		long				p = lPumpNumber-1;
		PAY_AT_PUMP_INFO	info;   //4.0.18.501
		//PAY_AT_PUMP_INFO	info2;   //4.0.18.501
		OLA_STAT			ola;												  

		// ???? Debugging data
		//_Module.m_server.GetPump(lPumpNumber).GetPAPInfo(&info2);

		// Controller (as Master) report of Cancel
		// This flag will be used only when PumpSrv is slave.
		BOOL bMasterCancelOperation = FALSE;
		BOOL bCancelInMiddleOfAuthorize = FALSE;

		short nDataLen = 0;

		//4.0.14.500
		char sXmlBuff[MAX_XML_BUF_LEN];
		memset(&sXmlBuff,0,MAX_XML_BUF_LEN);
		int iLen = 0;

		if ( STRUCT_IS_CARD_ALL_DATA & lFlags )
			nDataLen = sizeof(CARD_SALE_ALL);

		else if (STRUCT_IS_CARD_SALE_EXTRA_DATA2 & lFlags )
			nDataLen = sizeof(CARD_SALE_EXTRA_DATA2_);	//only new second record

		else if (STRUCT_IS_CARD_ALL_DATA2 & lFlags )
			nDataLen = sizeof(CARD_SALE_ALL2);		 // all record

		else if (STRUCT_IS_CARD_ALL_DATA3 & lFlags )
		{
			lOlaRecordSize =sizeof(CARD_SALE_ALL3);  //4.0.5.0
			nDataLen = sizeof(CARD_SALE_ALL3);		 //4.0.5.0
		}			
		else
			nDataLen = sizeof(CARD_SALE_EXTRA_DATA);

		//4.0.14.500
		//if the buffer is not as xml, convert it to xml

		//4.0.18.501
		//The format of the incoming buffer
		CARD_SALE_ALL3 cardDataAll3;
		memset(&cardDataAll3,' ',sizeof(CARD_SALE_ALL3));

		if (_Module.m_server.m_bUseXmlBuffer == FALSE)
		{
			iLen = Bstr2Buffer(sData, (void *)&cardDataAll3,nDataLen);

			//TD 41144 - Ella 4.0.16.504
			if (!a2l(cardDataAll3.CardSaleAll.data.sPumpNumber, sizeof(cardDataAll3.CardSaleAll.data.sPumpNumber)))
			{
				l2a0(lPumpNumber, cardDataAll3.CardSaleAll.data.sPumpNumber,sizeof(cardDataAll3.CardSaleAll.data.sPumpNumber));
				
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("Pump number is taken from the interface, since it's missing in CardSaleData");
					_LOGMSG.LogMsg("GCI","PAPReadyTransaction",lPumpNumber,LOG_PUMP,str);
				}	
			}
		}
		else
		{
			//The buffer is already in xml format
			iLen = Bstr2Buffer(sData, (void *)&sXmlBuff,MAX_XML_BUF_LEN);
			//Convert the xml to struct
			
			//4.0.19.502 - CR 58096
			if (lFlags & STRUCT_IS_CARD_SALE_EXTRA_DATA4_PLUS)
			{
				CARD_SALE_EXTRA_DATA4_PLUS cCardSaleExtraData4Plus;
				memset(&cCardSaleExtraData4Plus, ' ' ,sizeof(cCardSaleExtraData4Plus));	 //4.0.22.510
				
				if (!(_Module.m_server.m_cXmlConvertorEx.ConvertFromXmlToStruct(sXmlBuff,&cCardSaleExtraData4Plus)))
				{
					_LOGMSG.LogMsg("XmlProblem: PAPReadyTransaction, convert to CardSaleExtraData4Plus",LOG_LEVEL_1);
					retCode = GCI_FAILED_IO;
				}
				else
				{
					memcpy(&cardDataAll3, &cCardSaleExtraData4Plus.CardSaleAll3, sizeof(cardDataAll3));
				}

				// Insert receipt extra data to Qdex
				PS::RECEIPT_EXTRA_INFO_REC	rec;
				long lRtc = 0;
				memset(&rec, ' ', sizeof(PS::RECEIPT_EXTRA_INFO_REC));

				//TD 134014-  4.0.25.50 - In order to support 6 digits seq number while the receipt Extra data qdx key is 5 digits only...
				if ((lSeqNumber > MAX_TRANSACT_SEQ_NUMBER) && (_Module.m_server.m_bControllerReportsSixDigitSeqNumber == TRUE))
				{
					//copy only the Last 5 digits out of 6 i.e: Sequence num = 123456 will cut to -> 23456
					BYTE		sTmpTranNbr[sizeof(cardDataAll3.CardSaleAll.data.sTranNbr)+1];
					memset(&sTmpTranNbr,' ',sizeof(sTmpTranNbr));
					l2a(lSeqNumber, sTmpTranNbr, sizeof(sTmpTranNbr));
					//override the sTranNbr:
					memcpy(cardDataAll3.CardSaleAll.data.sTranNbr,sTmpTranNbr+1,sizeof(cardDataAll3.CardSaleAll.data.sTranNbr));
				}
				
				rec.info.lIndexNumber = a2l(cardDataAll3.CardSaleAll.data.sTranNbr, sizeof(cardDataAll3.CardSaleAll.data.sTranNbr));

				//copy the data: 4.0.20.40
				memset(rec.info.sFiler2043, ' ', sizeof(rec.info.sFiler2043));
				memcpy(rec.info.sFiler2043, cCardSaleExtraData4Plus.cReceiptExtraInfo.sFiler2043, min(sizeof(rec.info.sFiler2043), strlen((char *)cCardSaleExtraData4Plus.cReceiptExtraInfo.sFiler2043)));

				lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_INSERT, rec);
				//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_INSERT,&rec);

				if ((lRtc) && (lRtc != ERR_EXISTS))
				{
					//we failed to insert data to qdx								
					CString str;
					str.Format("Failed to insert extra receipt information Index=%ld , Rtc=%ld", rec.info.lIndexNumber, lRtc);
					_LOGMSG.LogMsg("GCI", "PAPReadyTransaction", p + 1, LOG_PUMP, str, LOG_LEVEL_1);
				}
				else
				{
					long lTmpFlags = a2l(cardDataAll3.extData2.sFlags, sizeof(cardDataAll3.extData2.sFlags));

					if (lRtc == ERR_EXISTS)
					{
						lRtc = 0;
						memset(&rec, ' ', sizeof(PS::RECEIPT_EXTRA_INFO_REC));
						rec.info.lIndexNumber = a2l(cardDataAll3.CardSaleAll.data.sTranNbr, sizeof(cardDataAll3.CardSaleAll.data.sTranNbr));
						// 4.0.25.???? TD 427833 
						memset(rec.info.sFiler2043, ' ', sizeof(rec.info.sFiler2043));
						memcpy(rec.info.sFiler2043, cCardSaleExtraData4Plus.cReceiptExtraInfo.sFiler2043, min(sizeof(rec.info.sFiler2043), strlen((char *)cCardSaleExtraData4Plus.cReceiptExtraInfo.sFiler2043)));

						lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_UPDATE, rec);
						//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_UPDATE,&rec);

						if (LOG_DETAIL_FLOW_CONTROL)
						{
							CString str;
							str.Format("Update extra receipt information Index=%ld , Rtc=%ld", rec.info.lIndexNumber, lRtc);
							_LOGMSG.LogMsg("GCI", "PAPReadyTransaction", p + 1, LOG_PUMP, str);	//4.0.20.40
						}
					}
						
					if (!lRtc)
					{
						//mark that there is extra dtat waiting:
						if (!(lTmpFlags & FL_EXTRA_RECEIPT_REC))
							lTmpFlags |= FL_EXTRA_RECEIPT_REC;
						l2a(lTmpFlags, cardDataAll3.extData2.sFlags ,sizeof(cardDataAll3.extData2.sFlags)); 
					}
				}
			}
			else 
			{
				if (!(_Module.m_server.m_cXmlConvertorEx.ConvertFromXmlToStruct(sXmlBuff,&cardDataAll3)))
				{
					_LOGMSG.LogMsg("XmlProblem: PAPReadyTransaction, convert to CardData3",LOG_LEVEL_1);
					retCode = GCI_FAILED_IO;
				}
			}
		}

		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("strcut size=%ld, lFlags=%ld ,ilen=%d, last error= %ld retCode=%d ", nDataLen,lFlags ,iLen , dwLastErr ,retCode);
				_LOGMSG.LogMsg("GCI","PAPReadyTransaction",lPumpNumber,LOG_PUMP,str);
			}	

		}

		if (retCode == GCI_OK)
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("strcut size=%ld, lFlags=%ld ,ilen=%d  ", nDataLen,lFlags ,iLen );
				_LOGMSG.LogMsg("GCI","PAPReadyTransaction",lPumpNumber,LOG_PUMP,str);
			}	

			_Module.m_server.m_cProtectedDataHandler.GenerateIndex(&cardDataAll3, FALSE, lTrsNumber);		//4.0.24.90 4.0.28.500 TD 243638

			//Copient 4.0.19.502 + TD 59557
			if ((_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()) && (_Module.m_server.m_cFuelLoayaltyPS.IsOLAInLoyaltyNMPState(lPumpNumber)))
			{//4.0.20.611 72527 //4.0.22.500
				OLA_STAT olaTempLoyalty;
				_Module.m_server.m_cPumpArray[lPumpNumber- 1].GetOlaStat(&olaTempLoyalty); 

				if ((lFlags & STRUCT_IS_PAP_CANCEL)|| ((lVolume == 0) && (lValue == 0)))
				{
					if (_Module.m_server.m_cFuelLoayaltyPS.IsOLAInLoyaltyNMPStateAfterStartTicket(lPumpNumber)) // 4.0.32.421
						_Module.m_server.m_cFuelLoayaltyPS.SetCancelTrs(lPumpNumber,CancelAfterStartTicket);
					else 
						_Module.m_server.m_cFuelLoayaltyPS.SetCancelTrs(lPumpNumber,CancelBeforeStartTicket );
				
					cardDataAll3.CardSaleAll.data.sTranStatus = CD_APPROVE_OK;
					//_Module.m_server.m_cPumpArray[p].ClearOlaStat();
					_Module.m_server.SavePumpInfo(lPumpNumber);
				}
				else if (olaTempLoyalty.m_byState == OLA_LOYALTY_WAIT_FUELING) 	//4.0.20.611 72527
				{
					_Module.m_server.m_cPumpArray[p].ClearOlaStat();
					_Module.m_server.SavePumpInfo(lPumpNumber);

				}
				if (!_Module.m_server.m_cFuelLoayaltyPS.IsPaymentFirstInFlow()) //if loyalty first- take track1 from memory //4.0.23.480 125623 //4.0.25.30
				{
					PAY_AT_PUMP_INFO cPapInfo;
					_Module.m_server.m_cPumpArray[p].GetPAPInfo(&cPapInfo);	
					memcpy(cardDataAll3.CardSaleAll.data.sTrack1Data, cPapInfo.CardSaleInfo.cardData.sTrack1Data, sizeof(cardDataAll3.CardSaleAll.data.sTrack1Data));
					memcpy(cardDataAll3.CardSaleAll.data.sTrack2Data, cPapInfo.CardSaleInfo.cardData.sTrack2Data, sizeof(cardDataAll3.CardSaleAll.data.sTrack2Data)); //4.0.27.30 222093

					if(LOG_BASIC_FLOW_CONTROL)
					{
						_LOGMSG.LogMsg("GCI","PAPReadyTransaction",lPumpNumber,LOG_PUMP,"Loyalty first flow. Track1 will be taken from memory");
					}	
					
				}
			}
				
			l2a(nNozzle, &cardDataAll3.CardSaleAll.data.sNozzle, sizeof(cardDataAll3.CardSaleAll.data.sNozzle));   //R10 User Story - 76018 

			long ltmp = a2l((BYTE *)&cardDataAll3.CardSaleAll.extData.SALES[0].sDPTProdCode,sizeof(cardDataAll3.CardSaleAll.extData.SALES[0].sDPTProdCode));

			//4.0.7.500
			//RFUEL-3534 adding carwash amount is already handled in RFS' long CServerMain::CalculatePAPTransTotalAmt()
			/*if(lFlags & PAP_WITH_CARWASH && _Module.m_server.GetTwoTendersInTransaction() == FALSE) // 4.0.32.490
			{
				long lAmount = a2l((BYTE *)&cardDataAll3.CardSaleAll.extData.SALES[0].sUnitPrice,sizeof(cardDataAll3.CardSaleAll.extData.SALES[0].sUnitPrice)) * (_Module.m_server.GetCarWashScalarFactor());

				long lTotalAmount = ConvertStringsToLong(cardDataAll3.CardSaleAll.extData.sTotalAmt,sizeof(cardDataAll3.CardSaleAll.extData.sTotalAmt),cardDataAll3.CardSaleAll.extData.sTotalAmt_Msb,sizeof(cardDataAll3.CardSaleAll.extData.sTotalAmt_Msb));

				long lOrgValue = lValue;

				lValue += lAmount;

				CString str;
				str.Format("CarWash value was added = [%d], Origin Value = [%d], new value = [%d], lTotalAmount = [%ld]" , lAmount,lOrgValue, lValue ,lTotalAmount); 
				_LOGMSG.LogMsg("GCI","PAPReadyTransaction",lPumpNumber,LOG_PUMP,str,LOG_LEVEL_2);	
			}*/

			if(LOG_LOGIC_ERROR)   
			{
				CString str;    
				//4.0.21.0
				str.Format("PAPReadyTransaction() - sTranStatus [%c], sTranType [%c], lFlags [%ld], iLen [%d]" , 
					cardDataAll3.CardSaleAll.data.sTranStatus, cardDataAll3.CardSaleAll.data.sTranType ,lFlags, iLen);    //4.0.8.501 
				_LOGMSG.LogMsg("GCI","PAPReadyTransaction",lPumpNumber,LOG_PUMP,str,LOG_LEVEL_2);
			}
			//3.1.1.67

			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------

			if(_Module.m_server.NozzleLevelLogic())
			{
				if (!nGrade && nNozzle && lPumpNumber)
					nFinalGrade = _Module.m_server.m_cPumpArray[p].NozzleToGrade(nNozzle);
			}

			if (lFlags & PAP_DECLINE_INFO)   //3.0.1.84
			{
				_Module.m_server.m_cPumpArray[p].SetPAPSeqNumber(lSeqNumber);
				
				//4.0.18.501
				_Module.m_server.m_cCommonFunction.ConvertCardSaleAll3ToPayAtPumpInfo(info, cardDataAll3);

				info.CardSaleExtraData4.byGetReceiptMethod = (BYTE)_Module.m_server.GetReceiptLookupMethod();	//4.0.21.501 TD 60203 
				
				l2a(lSeqNumber,info.CardSaleInfo.cardData.sTranNbr,sizeof(info.CardSaleInfo.cardData.sTranNbr)); //4.0.18.501

				if(_Module.m_server.GetPAPDeclineTimeValue())
					_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetMiscCardNotAuthorized();	 //4.0.13.71

				trs.Clear();
				if (!bIsOfflineTrs)	//4.0.23.500 78023
					_Module.m_server.m_cMemPumpArray[p].SetMemPumpInfo(&info ,&trs); //4.0.0.6
					
				if(LOG_BASIC_FLOW_CONTROL)
				{											
					CString str;
					str.Format("SetMemPumpInfo step 0 Update only decline Code Pump=%ld Seqnumber=%ld Authorize by Allied", p + 1 , lSeqNumber);
					_LOGMSG.LogMsg("GCI","PAPReadyTransaction",lPumpNumber,LOG_PUMP,str);
				}
				
				retCode = GCI_UPDATE_OLA_MEM_INFO; 
			}
			//4.0.19.509 {
			else if (lFlags & PAP_OPEN_SESSION_COMPLETE)
			{
				CString sMsg;
				_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
				char sServiceFee[MAX_FIELD_VAL];

				if( OLA_STATE(ola.m_byState) == OLA_NONE )
				{
					// open session only if the seq is equal 
					if (lSeqNumber == _Module.m_server.m_cPumpArray[p].GetPAPSeqNumber())
					{
						if(!_Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&trs))
						{							
							trs.SetLink(TRS_LINK_OLA_NUMBER, lSeqNumber);					
							_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);							

							_Module.m_server.m_cPumpArray[p].GetPAPInfo(&info);						

							// in AVI conflict we changed the Transaction approval due to that we failed to 
							// charge the transaction.
							
							if (info.CardSaleInfo.cardData.sTranStatus == CD_VIT_CONFLICT)  //4.0.5.43 
							{
								info.CardSaleInfo.cardData.sTranStatus =  cardDataAll3.CardSaleAll.data.sTranStatus; //4.0.18.501
							
							}
							
							memcpy(sServiceFee, cardDataAll3.extData2.sServiceFee, sizeof(cardDataAll3.extData2.sServiceFee));
							
							_Module.m_server.m_cCommonFunction.ConvertPayAtPumpInfoToCardSaleAll3(cardDataAll3, info);

							// overwrite the service fee. 
							if(lValue == 0)
							{
								memcpy(cardDataAll3.extData2.sServiceFee, sServiceFee, sizeof(cardDataAll3.extData2.sServiceFee)); //4.0.21.0
							}													
						}
						else
						{
							if(LOG_LOGIC_ERROR)
							{
								CString str;
								str.Format("transaction not found lTrsNumber=%ld , lSeqNumber=%ld" ,lTrsNumber, lSeqNumber);
								_LOGMSG.LogMsg("GCI","PAPReadyTransaction",p + 1,LOG_PUMP,str,LOG_LEVEL_1);
							}
						}						
					}
					else
					{
						if(LOG_BASIC_FLOW_CONTROL)
						{						
							sMsg.Format("Bad seq number Can not open session complete. (Seq input %ld, PumpSrv Seq=%ld %ld)",lSeqNumber,_Module.m_server.m_cPumpArray[p].GetPAPSeqNumber());
							_LOGMSG.LogMsg("GCI","PAPReadyTransaction",p + 1,LOG_PUMP,sMsg);
						}
					}
				}
			}
			//4.0.19.509 }
			// convert PAK transaction to PAP transaction
			else if (lFlags & TRS_PAK_CHANGE_PAP)   //4.0.3.39 //4.0.5.34
			{
				BOOL bFound = FALSE;

				if(!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))				
					bFound = TRUE;
				else  if(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_ANY,&trs)) //4.0.5.41
					bFound = TRUE;

				if (bFound)
				{
					trs.SetLink(TRS_LINK_OLA_NUMBER, lSeqNumber);
					_Module.m_server.m_cPumpArray[p].SetTrsByNumber(trs.m_lNumber, &trs,FALSE);
				}
				
			}
			// convert OLA transaction to PAK transaction
			else if (lFlags & TRS_OLA_TRANSACTION_CHANGE_PAK)    //4.0.5.42
				 
			{
				BOOL bFound = FALSE;    //4.0.6.502
				BOOL bChange = FALSE;

				bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&trs);
				if (bFound)
				{
					bChange=TRUE;
				}
				else if(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS,TRS_LINK_OLA_NUMBER,&trs))
				{
					bChange=TRUE;
				}

				if (bChange)
				{
					trs.m_lRes = PAY_AT_KIOSK;
					trs.m_lResExt = NULL_RES_EXT;
					trs.SetLink(TRS_NO_LINK , 0);
					_Module.m_server.CentralUpdateTrs(p+1,trs.m_lNumber,&trs,FALSE);				
				}
				
				retCode = GCI_CONVERT_OLA_TRNSACTION_TO_PAK; 

				if(LOG_BASIC_FLOW_CONTROL)
				{											
					CString str;
					str.Format("OLA transaction was convert to PAK transaction, Transaction=%ld, Found=%d", trs.m_lNumber ,bFound);
					_LOGMSG.LogMsg("GCI","PAPReadyTransaction",p + 1,LOG_PUMP,str);
				}
				
			}
			else if (lFlags & TRS_OLA_FORCE_CLOSE_AS_ATTENDANT)  //4.0.11.505
			{
				BOOL bFound = FALSE;    //4.0.6.502
				BOOL bChange = FALSE;


				if(!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))				
				{
					bChange=TRUE;
					if(LOG_BASIC_FLOW_CONTROL)
					{											
						CString str;
						str.Format("Pump - %02ld OLA Trs=%ld  in dispensing  was found, ",p+1, trs.m_lNumber);
						_LOGMSG.LogMsg("GCI","PAPReadyTransaction",p + 1,LOG_PUMP,str);
					}

				}
				else
				{
					 bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&trs);
					if (bFound)
					{
						bChange=TRUE;

						if(LOG_BASIC_FLOW_CONTROL)
						{											
							CString str;
							str.Format("Pump - %02ld OLA Trs=%ld  in Wait progress was found, ",p+1, trs.m_lNumber);
							_LOGMSG.LogMsg("GCI","PAPReadyTransaction",p + 1,LOG_PUMP,str);
						}

					}
				}

				if (bChange)
				{
					trs.m_lRes = PAY_AT_KIOSK;
					trs.m_lResExt = RES_EXT_ATTENDANT;
					trs.SetLink(TRS_NO_LINK , 0);
					trs.SetStatus(UNPAID);
					_Module.m_server.CentralUpdateTrs(p+1,trs.m_lNumber,&trs,FALSE);				
				}
				
				retCode = GCI_CONVERT_OLA_TRNSACTION_TO_PAK; 

				if(LOG_BASIC_FLOW_CONTROL)
				{											
					CString str;
					str.Format("Pump - %02ld OLA Trs=%ld, convert to PAK and close as PAK  Found=%d",p+1, trs.m_lNumber ,bFound );
					_LOGMSG.LogMsg("GCI","PAPReadyTransaction",p + 1,LOG_PUMP,str);
				}
			}
			else if ( OLA_BY_CONTROLLER )
			{
				// 2.9.0.8
				// 2.9.0.9

				//In case the controller is doing the OLA, we don't know until the end
				//of the transaction what is the sequence number.
				//Since we would like to live the treatment of finalize PAP transaction the way it is,
				//We implant the sequence number, so it will pass the checking at the
				//following steps of this method.

				//Implant the sequence number to CARD_SALE_DATA at the pump level.
				_Module.m_server.m_cPumpArray[p].SetPAPSeqNumber(lSeqNumber);
				//Implant the sequence number to the current transaction.

				
				BOOL bFound = FALSE;
				if(!_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))
					bFound = TRUE;

			//	if(!_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_NO_LINK,&trs))	//232586 - 4.0.27.501	
				if(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&trs))			
					bFound = TRUE;
				
				if(bFound)
				{
					trs.SetLink(TRS_LINK_OLA_NUMBER, lSeqNumber);
					_Module.m_server.m_cPumpArray[p].SetTrsByNumber(trs.m_lNumber, &trs,FALSE);
				}
			}
			trs.Clear();

			//4.0.12.501
			if(lFlags & STRUCT_IS_UPDATE_OLA)
			{
				CString str;
				_Module.m_server.m_cPumpArray[p].ForceCloseOlaSession(); //4.0.13.501

				if (lValue)	  //4.0.13.500
				{
					str.Format("Found car wash item");
					_LOGMSG.LogMsg("GCI","PAPReadyTransaction",p + 1,LOG_PUMP,str,LOG_LEVEL_3);
					_Module.m_server.UpdateDataInCarWashTrs(trs,lPumpNumber,lSeqNumber);
				}
				else
				{
					str.Format("Cancel car wash item detect");
					_LOGMSG.LogMsg("GCI","PAPReadyTransaction",p + 1,LOG_PUMP,str,LOG_LEVEL_3);

					_Module.m_server.m_cCommonFunction.CardSaleAll3ToPayAtPumpInfo(info, cardDataAll3);					//4.0.23.260 TD 109874
					_Module.m_server.m_cProtectedDataHandler.GenerateIndex(&info, TRUE);		//4.0.23.501 TD 72716
					_Module.m_server.m_cPumpArray[p].SetPAPInfo(&info);							//4.0.25.80 TD 141363 

					SQLPumpSrvFile.LoadPAPInfo(lPumpNumber);
				}

				if (lFlags & STRUCT_IPT_CARWASH)  //4.0.23.503 TD 72229
				{
					
					_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
					
					if (cardDataAll3.CardSaleAll.data.sTranStatus == CD_APPROVE_OK)
						_Module.m_server.SendReceipt(ola.m_byState,p+1, CD_FORCE_OK,cardDataAll3.CardSaleAll.data.sTranStatus,(CARD_SALE_ALL3 *)&cardDataAll3.CardSaleAll.data,&trs);
					else
						_Module.m_server.SendReceipt(ola.m_byState,p+1, CD_FORCE_NOT_OK,cardDataAll3.CardSaleAll.data.sTranStatus, (CARD_SALE_ALL3 *)&cardDataAll3.CardSaleAll.data,&trs);
					
					
				}

				retCode = GCI_UPDATE_OLA_MEM_INFO;
			}
			
			else if (lFlags & PAP_FORCE_CLOSE_SESSION)
			{				
				_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);

				if(LOG_LOGIC_ERROR)
				{
					CString str;
					str.Format("Force close session - ola.m_byStat=%d" , ola.m_byState);
					_LOGMSG.LogMsg("GCI","PAPReadyTransaction",p + 1,LOG_PUMP,str,LOG_LEVEL_3);
				}
								
				if( OLA_STATE(ola.m_byState) != OLA_NONE )
				{
					ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_SEND_END);
					_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola); 	
					_Module.m_server.SavePumpInfo(p+1); // save ola stat.
					retCode = GCI_UPDATE_OLA_MEM_INFO; 
				}
				if(_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag() && (_Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber) == CancelAfterStartTicket))	//4.0.22.504 69344
					_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber);				
			}			
			
			
			// -----------------------------------------------------
			//
			//	Check if lSeqNumber equal to last known transaction
			//
			// -----------------------------------------------------

			else if( (lSeqNumber <= 0) || 
				(lSeqNumber != _Module.m_server.m_cPumpArray[p].GetPAPSeqNumber()) )
			{
				retCode = GCI_INVALID_SEQ_NUMBER;
				if(LOG_LOGIC_ERROR)
				{
					CString str;
					str.Format("SeqNumber %ld, Last known sequence %ld", lSeqNumber,_Module.m_server.m_cPumpArray[p].GetPAPSeqNumber()); 
					_LOGMSG.LogMsg("GCI","PAPReadyTransaction",p + 1,LOG_PUMP,str,LOG_LEVEL_1);
				}
				if ((lSeqNumber > MAX_TRANSACT_SEQ_NUMBER) && (_Module.m_server.m_bControllerReportsSixDigitSeqNumber == TRUE))  //4.0.23.503 merge - start TD 78769 
				{
					//PumpSrv uses 5 digits 99999 - Postec uses 6 digits (999999) sequence number 
					//If flag is 1, Allow it. The 6th digit will be cut from sTranNbr.
					
					retCode = GCI_OK;

					CString str;
					str.Format("Allow SeqNumber %ld, bigger than max %ld. Save it as SeqNumber %ld", lSeqNumber,MAX_TRANSACT_SEQ_NUMBER, _Module.m_server.m_cPumpArray[p].GetPAPSeqNumber()); 
					_LOGMSG.LogMsg("GCI","PAPReadyTransaction",p + 1,LOG_PUMP,str);
				}  //4.0.23.503 - end
			}

			if(retCode == GCI_INVALID_SEQ_NUMBER)
			{
				if(LOG_LOGIC_ERROR)
				{
					trs.m_nNozzle	= nNozzle;
					trs.m_lRoundedVolume = lVolume;
					trs.m_lRawVolume = lVolume;
					trs.m_lRoundedValue = lValue;
					trs.m_nLevel = nLevel;
					trs.m_lGradePrice = lGradePrice;
					trs.m_nGrade = nFinalGrade;
					trs.m_lPump	= lPumpNumber;				
					trs.m_lRes = PAY_AT_PUMP;
					trs.m_lResExt = NULL_RES_EXT;
					trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[p].GetShiftNumber();
					trs.m_lActiveReadNumber = _Module.m_server.GetActiveReadNumber();
					trs.UpdateTime();
					trs.m_lTankID =  _Module.m_server.m_cPumpArray[p].GetTankPerPosition(nNozzle); //4.0.27.60  TD 231475

					_Module.m_server.m_cExLog.LogTrs(&trs);
				}
			}


			// -----------------------------------------------------
			//
			//	Check Case of transaction Canceled 
			//
			// -----------------------------------------------------

			// Get PAP info 
			_Module.m_server.m_cPumpArray[p].GetPAPInfo(&info);
			BOOL bAutoForce = _Module.m_server.IsAutoForce();
			// If AutoForce :
			//		1. Check that transaction is not in middle of force operation
			//		
			// If AutoForce or Not:
			//		1. Check that transaction is not PAID yet.

			if (lFlags & PAP_OPEN_SESSION_COMPLETE)    //4.0.19.509
				bAutoForce = TRUE;

			_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs);

			if (_Module.m_server.GetPriceProtectionActive() == PRICE_PROTECTION_ACTIVE_INSIDE_TIME_FRAME)
			{
				trs.m_lLowestGradePrice = _Module.m_server.GetGradePriceFromArray(nFinalGrade, PRICE_PROTECTION_LOWEST_PRICE);
			}

			if (info.CardSaleExtraData5.sLoyaltyAutoArm == '1')
			{
				// Credit should set the auto auth flag to 0
				/*if (_Module.m_server.IsAutomaticAuthForLoyaltyCustomer() == TRUE)
					trs.m_sLoyaltyAutoArm = '1';*/

				long lOriginalPrice = _Module.m_server.GetGradePriceFromArray(nFinalGrade, _Module.m_server.GetGradePriceProtectionLevel());

				if ((info.CardSaleExtraData5.sPriceProtectionUsed == '1') && 
					(lOriginalPrice > trs.m_lLowestGradePrice))
				{
					trs.m_sLoyaltyPriceProtection = '1';
				}
			}

			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("PAPReadyTransaction(%ld) - lGradePrice %ld, LowestPrice %ld , retCode = %ld", 
					trs.m_lNumber, 
					lGradePrice, 
					trs.m_lLowestGradePrice,
					retCode);
				_LOGMSG.LogMsg("GCI", "PAPReadyTransaction", lPumpNumber, LOG_PUMP, str);
			}

			_Module.m_server.CentralUpdateTrs(lPumpNumber, trs.m_lNumber, &trs, FALSE);
			}

			if ( (retCode == GCI_OK) && (OLA_BY_PUMPSRV) )
			{
				_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);

				if(((lVolume == 0) && (lValue == 0)) && (!(lFlags & STRUCT_IS_SALE_COMMAND)))//4.0.12.501
				{
					CancelInTheMiddleOfAuthorize(cardDataAll3, lPumpNumber, ola, bCancelInMiddleOfAuthorize, trs, lSeqNumber, retCode, bMasterCancelOperation);
				}
				else // check case of final information arrive in middle of session ...
				{
					if (SESSION_TYPE(ola.m_byState) == SESSION_AUTHORIZE)
					{
						if(!_Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&trs))
						{	
							//We got here for the reason we authorize the pump because of 'Hot Pumping'.
							//and the customer manage to finish the transaction before we end the
							//authorization session.
							//So here is what we going to do:
							//1. Convert the transaction from PAP to PAK.
							trs.m_lRes = PAY_AT_KIOSK;
							trs.m_lMisc |= MISC_TRS_PAP_CANCELED;
							_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);
							PAKReadyTransaction__(lPumpNumber,trs.m_lNumber, nNozzle, lVolume, lValue, nLevel, lGradePrice, nGrade,pVal);
							
							//2. Wait for the session to end and then conclude
							//There was no cancel here, but at the rest of this method it should act the same.
							bCancelInMiddleOfAuthorize = TRUE;
							ola.m_byState = SESSION_NEW_TYPE(ola.m_byState,SESSION_CANCEL_AUTH);
							_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 	
							
							retCode = GCI_TRS_CONVERTED_TO_PAK;
							
							if(LOG_BASIC_FLOW_CONTROL)
							{
								CString str;
								str.Format(">>> OLA:Transaction ended while authorize session(Session %ld)",ola.m_lSessionNumber);
								_LOGMSG.LogMsg("GCI","PAPReadyTransaction",lPumpNumber,LOG_PUMP,str);
								str.Format("Trs %d Was converted from PAP to PAK",trs.m_lNumber);
								_LOGMSG.LogMsg("GCI","PAPReadyTransaction",lPumpNumber,LOG_PUMP,str);
							}
						}
					}
					else if(OLA_STATE(ola.m_byState) != OLA_NONE)
					{
						if(LOG_LOGIC_ERROR)
						{
							CString str;
							str.Format("PAPReadyTransaction: Pump in middle of another session - Trs %d, SeqNumber %d, Ola state %x", trs.m_lNumber,lSeqNumber,ola.m_byState); 
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str,LOG_LEVEL_1);
						}
						retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;							
					}
				}			
			}			
			
			
			if (retCode == GCI_OK)
			{
				if(PUMPSRV_IS_MASTER)
					//======================================================
				{
					// if PumpSrv is Master , transaction has a link number 
					// from the very beginning of transaction life cycle - and synchronization 
					// with controller is done via "lSeqNumber"
					
					trs.SetLink(TRS_LINK_OLA_NUMBER, lSeqNumber);
					if(!_Module.m_server.m_cPumpArray[p].GetTrsByLinkNumber(&trs))
					{
						if(trs.m_lStatus == PAID)
							retCode = GCI_TRS_ALREADY_PAID;							
					}
					//Copient 4.0.19.502 + TD 59557
					else if ((_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()) && ((lFlags & STRUCT_IS_PAP_CANCEL)|| ((lVolume == 0) && (lValue == 0))))	
					{
						if (!IsReservationPakWithLoyaltyAsPap(lPumpNumber)) //4.0.23.1130 283329
						{
							CPumpTransact trsLoyalty;//4.0.20.480	//4.0.22.0 66977
							if(_Module.m_server.m_cPumpArray[lPumpNumber-1].GetInProgressWaitProgressTrs(&trsLoyalty))
							{
								//Loyalty case, cancel trs before authorize, no trs exists yet
								retCode = GCI_TRS_NOT_FOUND;
								if(LOG_BASIC_FLOW_CONTROL)
								{	
									CString str;
									str.Format("PapReadyTransaction, loyalty flow, trs not found");
									_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,str,lFlags);
								}
								if (_Module.m_server.m_cFuelLoayaltyPS.IsOLAInLoyaltyNMPState(lPumpNumber))	//4.0.22.504 69344
									_Module.m_server.m_cFuelLoayaltyPS.SetCancelTrs(lPumpNumber, CancelAfterStartTicket);
								if (_Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber) == CancelAfterStartTicket)	//4.0.22.504 69344
								{
									_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber);	
									_Module.m_server.m_cFuelLoayaltyPS.SetCancelTrs(lPumpNumber, NoCancel);
									_Module.m_server.m_cPumpArray[lPumpNumber - 1].ClearOlaStat();  // RFUEL-3144

								}

							}
						}
					}
					else
					{
						//4.0.9.510 TD 14881
						//if we got here we have to check that we are not in start session
						//because then we will authorize  the pump
						//so if we in the start of the session we will cancel it
						if ((SESSION_TYPE(ola.m_byState) == SESSION_AUTHORIZE)&&
							(OLA_STATE(ola.m_byState) == OLA_REQUEST_TO_SEND))
						{
							ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_NONE);
							if (ola.m_lSessionNumber == 0)	//session has not started yet ->clear it 4.0.20.502 65282 merge from 1015
							{
								ola.m_byState = OLA_NONE;	//Remove both authorize session and state 
								if(LOG_LOGIC_ERROR)
								{
									CString str;
									str.Format("Trs was canceled before authorize session started with OLA. Initializing ola state and session type!!!"); 
									_LOGMSG.LogMsg(p+1,LOG_PUMP,str,LOG_LEVEL_1);
								}
							}
							_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
						}
						
						if(LOG_LOGIC_ERROR)
						{
							CString str;
							str.Format("Trs sequence %ld Not Found !!!",lSeqNumber); 
							_LOGMSG.LogMsg(p+1,LOG_PUMP,str,LOG_LEVEL_1);
						}

						retCode = GCI_TRS_NOT_FOUND;
					}
				}
				else // PumpSrv is slave
				//======================================================
				{
					// first try to find the trs by the sequence number
					trs.SetLink(TRS_LINK_OLA_NUMBER, lSeqNumber);	
					if(!_Module.m_server.m_cPumpArray[p].GetTrsByLinkNumber(&trs))
					{
						if(LOG_LOGIC_ERROR)
						{
							CString str;
							str.Format("Trs %ld, found by link number %ld",trs.m_lNumber, trs.m_lLinkNumber); 
							_LOGMSG.LogMsg(p+1,LOG_PUMP,str,LOG_LEVEL_1);
						}
						//4.0.2.38
						//if Transaction has been link number, we can asume that this is the second time report, 
						retCode = GCI_TRS_NOT_FOUND;

						// transaction found !!!
					}
					else
					{
						// if PumpSrv is Slave , transaction has a TrsNumber 
						// from the very beginning of transaction life cycle - and syncronization 
						// can be done via "lTrsNumber"

						if(!_Module.m_server.m_cPumpArray[p].GetTrsByNumber(lTrsNumber,&trs))
						{
							// Update link 
							trs.SetLink(TRS_LINK_OLA_NUMBER, lSeqNumber);	
						}
						else
						{	
							// in case of
							// cancel in middle of authorize    
							// cancel after authorize or 
							// Transaction have not been created !
							if((!bMasterCancelOperation)&&(!bCancelInMiddleOfAuthorize))
							{
								if(LOG_LOGIC_ERROR)
								{
									CString str;
									str.Format("Trs Number %ld Not Found !!!",lTrsNumber); 
									_LOGMSG.LogMsg(p+1,LOG_PUMP,str,LOG_LEVEL_1);
								}
								retCode = GCI_TRS_NOT_FOUND;
							}
						}
					}
				}			
			}


			// In Any case : (auto force and not auto force) 
			//		1. Update Trs Details.		n
			//		2. update pump trs list 
			// If not Auto force 
			//		3. Change Trs Link number into new OLA Index number .
			//		4. insert to OLA file
			//    4. Save PAP info
			// In Any case (auto force and not aut force)
			//		5. build return sCardData
			//		6. Log trs in shift log

			if(retCode == GCI_OK)
			{	
			//
			// If NOT AutoForce :
			//		1. Update "PAID" Stataus
				BOOL bAutoForce = _Module.m_server.IsAutoForce();
				if(!bAutoForce)
				{
					if(PUMPSRV_IS_MASTER)
						trs.m_lStatus	= PAID; // Pay At Pump ...				
				}

			// In any case
				/**********************************/
			//		1. Update Trs Details.		
				/**********************************/
				trs.m_nNozzle	= nNozzle;
				trs.m_lRoundedVolume = lVolume;
				trs.m_lRawVolume = lVolume;
				trs.m_lRoundedValue = lValue;
				trs.m_nLevel = nLevel;
				trs.m_lGradePrice = lGradePrice;
				trs.m_nGrade = nFinalGrade;
				trs.m_lPump	= lPumpNumber;				
				trs.m_lRes = PAY_AT_PUMP;

				trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[p].GetShiftNumber();
				trs.m_lActiveReadNumber = _Module.m_server.GetActiveReadNumber();
				trs.UpdateTime();
				trs.m_lTankID =  _Module.m_server.m_cPumpArray[p].GetTankPerPosition(nNozzle); //4.0.27.60  TD 231475

				if ( OLA_BY_CONTROLLER )
					trs.m_lNumber = lTrsNumber; //4.0.21.10
				
				SaveTrsVolumeForDeliveryEmulation(lPumpNumber, nNozzle, lVolume);		//4.0.20.52 (TD 62818)
				
				/* Calculate and update the Flow Rate:*/
				long lFlowRate = _Module.m_server.m_cFlowRate.SetFlowRateByMinute(trs,cardDataAll3);					//4.0.27.40 TD 165585

				if(LOG_DETAIL_FLOW_CONTROL)
				{
					//update info PAP
					CString str;
					str.Format("Update PAP info"); 
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
				}
				
				//4.0.18.501
				long lGRDiscount = a2l(cardDataAll3.CardSaleAll.extData.sGRActualDiscountPerGallon,sizeof(cardDataAll3.CardSaleAll.extData.sGRActualDiscountPerGallon));
				long lGRMaxVolume = a2l(info.CardSaleInfo.cardData.sGRMaxVolume,sizeof(info.CardSaleInfo.cardData.sGRMaxVolume));
	
				//4.0.19.504 - 47559
				if (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse())// CR 444830
				{
					if ((lValue == 0) && (lVolume == 0))
					{
						CPumpStatus		pumpStat;
						pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();
						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString	str;
							str.Format("CGPI::PAPReadyTransaction. Send CancelTrs: Pump=%ld, Status=%ld, Res=%ld" ,lPumpNumber, pumpStat.GetStatus(), pumpStat.GetRes());
							_LOGMSG.LogMsg(str);
						}

						CMD_STRUCT_TRS_CANCEL cmdInfo;
						cmdInfo.lTrsNumber = lTrsNumber;
						cmdInfo.lPumpStatus = trs.m_lStatus; 
						cmdInfo.lRes		= trs.m_lRes;
						cmdInfo.lResExt		= trs.m_lResExt;	
						_Module.m_server.SetCommand(CMD_TRS_CANCEL, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_TRS_CANCEL));
					}
				}

				SALE_EXTRA_DETAILS  cSaleExtraDetails;
				memset(&cSaleExtraDetails , ' ', sizeof(SALE_EXTRA_DETAILS));


				if (trs.m_lResExt == RES_EXT_MOBILE)  // update LoyaltyTenderAmount from MobileLoyalty
					memcpy(info.CardSaleExtraData5.sLoyaltyTenderAmount, cardDataAll3.extData5.sLoyaltyTenderAmount, sizeof(cardDataAll3.extData5.sLoyaltyTenderAmount));


				_Module.m_server.SetLoyaltyTenderAmount(&info, lValue, cSaleExtraDetails); // 4.0.22.????	TD 410631

				memcpy(cardDataAll3.extData5.sLoyaltyTenderAmount, info.CardSaleExtraData5.sLoyaltyTenderAmount, sizeof(info.CardSaleExtraData5.sLoyaltyTenderAmount));

				{
					CString	str;
					str.Format("CGPI::PAPReadyTransaction. After SetLoyaltyTenderAmount: Pump = %ld, LoyaltyTenderAmount = %.8s", lPumpNumber, cardDataAll3.extData5.sLoyaltyTenderAmount );
					_Module.m_server.m_cLogger.LogMsg(str);
				}

				// Update Info with the received Extra data, 
				//------------------------------------------			

				char sTmpBuf[MAX_XML_BUF_LEN];   
				memset(sTmpBuf,0,sizeof(sTmpBuf));

				BYTE sPumpSrvTranStatus = info.CardSaleInfo.cardData.sTranStatus;				
				BYTE sPumpSrvTranType = info.CardSaleInfo.cardData.sTranType;	//4.0.30.500 316205

				info.CardSaleInfo.cardData = cardDataAll3.CardSaleAll.data;			
				info.CardSaleInfo.extraData = cardDataAll3.CardSaleAll.extData; 

				//4.0.21.501 TD 60203 
				info.CardSaleExtraData4.byGetReceiptMethod = (BYTE)_Module.m_server.GetReceiptLookupMethod();	

				if (( STRUCT_IS_CARD_ALL_DATA2 & lFlags ) || (STRUCT_IS_CARD_ALL_DATA3 & lFlags ) || (lFlags & STRUCT_IS_CARD_SALE_EXTRA_DATA4_PLUS))	//4.0.21.0
				{

					info.CardSaleExtraData2 = cardDataAll3.extData2;			
					info.CardSaleExtraData3 = cardDataAll3.extData3;
					info.CardSaleExtraData4 = cardDataAll3.extData4;
					info.CardSaleExtraData5 = cardDataAll3.extData5;
					info.CardSaleExtraData6 = cardDataAll3.extData6;

					//4.0.9.508
					// detect mismatch between PS & cl2PumpSrv transaction status.
					// possibility 
					if ((sPumpSrvTranStatus != ' ') &&  
						(cardDataAll3.CardSaleAll.data.sTranStatus == ' ' ||
						 cardDataAll3.CardSaleAll.data.sTranStatus == 0) )	//4.0.18.501
					{
						info.CardSaleInfo.cardData.sTranStatus = sPumpSrvTranStatus; 

						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;					
							str.Format("Detect losing transaction status between PumpSrv  %c and cl2PumpSrv %c, force set PumpSrv status" , sPumpSrvTranStatus ,cardDataAll3.CardSaleAll.data.sTranStatus); //4.0.20.506
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
						}
					}
					
					if (info.CardSaleInfo.cardData.sTranType != sPumpSrvTranType) //4.0.30.500 316205
					{

						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;					
							str.Format("Detect mismatch Media Type between PumpSrv  %c and cl2PumpSrv %c, force set PumpSrv type" , sPumpSrvTranType ,cardDataAll3.CardSaleAll.data.sTranType); 
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
						}
						info.CardSaleInfo.cardData.sTranType = sPumpSrvTranType; 

					}
					

					if (STRUCT_IS_PAP_CANCEL & lFlags ) //4.0.6.501
					{
						char sFlagsVal[MAX_FIELD_VAL]; 
						memset(sFlagsVal,0,sizeof(sFlagsVal));
						if (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag() && (_Module.m_server.m_cFuelLoayaltyPS.IsCancelTrs(lPumpNumber)))	//Copient 55286 + Rtaps 59557
						{
							if (!_Module.m_server.GetOLAFlags((BYTE*)sFlagsVal ,FL_CANCEL_REC,sizeof(sFlagsVal)))					
							{

								CString sMsg;
								sMsg.Format(_T("%s:%d Set - FL_CANCEL_REC"), __FUNCTION__, __LINE__);
								_LOGMSG.LogMsg(sMsg);
								long lTmp =  a2l(info.CardSaleExtraData3.sFlags1,sizeof(info.CardSaleExtraData3.sFlags1)) + FL_CANCEL_REC;
								l2a(lTmp ,info.CardSaleExtraData3.sFlags1, sizeof(info.CardSaleExtraData3.sFlags1));
								_Module.m_server.m_cPumpArray[p].SetPAPInfo(&info); //4.0.15.230
							}
							//do nothing here
						}
						else
						{
							
							memcpy(sFlagsVal, info.CardSaleExtraData3.sFlags1, sizeof(info.CardSaleExtraData3.sFlags1));

							if (!_Module.m_server.GetOLAFlags((BYTE*)sFlagsVal ,FL_CANCEL_REC,sizeof(sFlagsVal)))					
							{
								CString sMsg;
								sMsg.Format(_T("%s:%d Set - FL_CANCEL_REC"), __FUNCTION__, __LINE__);
								_LOGMSG.LogMsg(sMsg);
								long lTmp =  a2l(info.CardSaleExtraData3.sFlags1,sizeof(info.CardSaleExtraData3.sFlags1)) + FL_CANCEL_REC;
								l2a(lTmp ,info.CardSaleExtraData3.sFlags1, sizeof(info.CardSaleExtraData3.sFlags1));
							
							}

							if  (( (sPumpSrvTranStatus == 'A')  &&  
								(cardDataAll3.CardSaleAll.data.sTranStatus != CD_APPROVE_OK)) ||
								((sPumpSrvTranStatus == 'c') && ((cardDataAll3.CardSaleAll.data.sTranStatus == CD_APPROVE_OK) || (cardDataAll3.CardSaleAll.data.sTranStatus == CD_READY)))) //4.0.15.380 53190 //4.0.15.610 60508	//4.0.20.501 65330 //4.0.20.506
								
							{
								if(LOG_BASIC_FLOW_CONTROL)
								{
									CString str;					
									str.Format("Detect mismatch PAP in PumpSrv  %c and cl2PumpSrv %c, force set PumpSrv data" , sPumpSrvTranStatus ,cardDataAll3.CardSaleAll.data.sTranStatus);
									_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
								}	

								info.CardSaleInfo.cardData.sTranStatus = CD_APPROVE_OK; //4.0.15.230 //4.0.19.508 60508
								
								// Detect mismatch between PS and cl data , use pre authorization information.
								_Module.m_server.m_cCommonFunction.ConvertPayAtPumpInfoToCardSaleAll3(cardDataAll3, info);

								_Module.m_server.m_cPumpArray[p].SetPAPInfo(&info); //4.0.15.230
							}
						}
					}
					// clear OLA Mac info, 
					memset(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byLen, ' ', sizeof(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byLen));
					memset(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byData, ' ', sizeof(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byData));				

					memset(info.CardSaleExtraData2.sCalculatedMAC,' ',sizeof(info.CardSaleExtraData2.sCalculatedMAC));
					info.CardSaleExtraData2.sPinPadAction =' ';
					info.CardSaleExtraData2.sPinPadResult = ' ';
				}

				// 4.0.23.1250   TD 308902  
				if ((!_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()) && _Module.m_server.GetOLAReportSoldItemOnPump() && nFinalGrade)
				{
							
					if (!a2l(info.CardSaleExtraData3.sOriginalUnitPrice, sizeof(info.CardSaleExtraData3.sOriginalUnitPrice)))
					{
						PumpItems  cPumpItems;						
						long lOriginUnitPrice = 0;
						long lIndex=0;
						long lTmpGrade = nFinalGrade;
						
						_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber,cPumpItems);
						if (_Module.m_server.m_cOLASrv->ConvertGradeItemsToIndex(lPumpNumber,lTmpGrade,lIndex))
						{
							lOriginUnitPrice = _Module.m_server.m_cOLASrv->GetItemPrice(lPumpNumber,&cPumpItems,lIndex); 
							l2a(lOriginUnitPrice, info.CardSaleExtraData3.sOriginalUnitPrice, sizeof(info.CardSaleExtraData3.sOriginalUnitPrice));
						}

					}
				}

				if (SESSION_TYPE(ola.m_byState) == SESSION_CANCEL_AUTH)
				{
					if (info.CardSaleInfo.extraData.sForceCompleteZero == 'Y')
					{
						info.CardSaleInfo.extraData.sForceCompleteZero = CD_FORCE_COMPLETION_ZERO;
					}
				}

				if ( OLA_BY_CONTROLLER   && (!bAutoForce))   //4.0.19.509
				{
					//Since there is no OLA, the following fields 
					//sAccountNumber, sAuthNumber, CardName
					//are sent from Allied. Therefore we need to update CARD_SALE_DATA.

					memcpy(info.CardSaleInfo.cardData.sAccountNumber, cardDataAll3.CardSaleAll.data.sAccountNumber, sizeof(info.CardSaleInfo.cardData.sAccountNumber));
					memcpy(info.CardSaleInfo.cardData.sAuthNumber, cardDataAll3.CardSaleAll.data.sAuthNumber, sizeof(info.CardSaleInfo.cardData.sAuthNumber));
					memcpy(info.CardSaleInfo.cardData.sCardName, cardDataAll3.CardSaleAll.data.sCardName, sizeof(info.CardSaleInfo.cardData.sCardName));
					memcpy(info.CardSaleInfo.cardData.sTrack2Data, cardDataAll3.CardSaleAll.data.sTrack2Data, sizeof(info.CardSaleInfo.cardData.sTrack2Data));
					Track2Format(info.CardSaleInfo.cardData.sTrack2Data);
					
					//TD 134650 4.0.25.40 - In order to spport GetReceipt2
					if ((lSeqNumber > MAX_TRANSACT_SEQ_NUMBER) && (_Module.m_server.m_bControllerReportsSixDigitSeqNumber == TRUE))
					{
						//copy only the Last 5 digits out of 6 i.e: Sequence num = 123456 will cut to -> 23456
						BYTE		sTmpTranNbr[sizeof(info.CardSaleInfo.cardData.sTranNbr)+1];
						memset(&sTmpTranNbr,' ',sizeof(sTmpTranNbr));
						l2a(lSeqNumber, sTmpTranNbr, sizeof(sTmpTranNbr));
						memcpy(info.CardSaleInfo.cardData.sTranNbr,sTmpTranNbr+1,sizeof(info.CardSaleInfo.cardData.sTranNbr));
					}
					else
					{
						l2a(lSeqNumber, info.CardSaleInfo.cardData.sTranNbr, sizeof(info.CardSaleInfo.cardData.sTranNbr));
					}
					memcpy(info.CardSaleInfo.cardData.sResultMsg, cardDataAll3.CardSaleAll.data.sResultMsg, sizeof(info.CardSaleInfo.cardData.sResultMsg));
					l2a(lPumpNumber, info.CardSaleInfo.cardData.sPumpNumber, sizeof(info.CardSaleInfo.cardData.sPumpNumber));
					memcpy(info.CardSaleInfo.extraData.sTotalAmt, cardDataAll3.CardSaleAll.extData.sTotalAmt, sizeof(info.CardSaleInfo.extraData.sTotalAmt));
					memcpy(info.CardSaleInfo.extraData.sPrnDateTime, cardDataAll3.CardSaleAll.extData.sPrnDateTime, sizeof(info.CardSaleInfo.extraData.sPrnDateTime));
					memcpy(&info.CardSaleInfo.extraData.sCarWashProgram, &cardDataAll3.CardSaleAll.extData.sCarWashProgram, sizeof(info.CardSaleInfo.extraData.sCarWashProgram));
					memcpy(info.CardSaleInfo.extraData.sWashCode, cardDataAll3.CardSaleAll.extData.sWashCode, sizeof(info.CardSaleInfo.extraData.sWashCode));
					memcpy(info.CardSaleInfo.extraData.sWashExpire, cardDataAll3.CardSaleAll.extData.sWashExpire, sizeof(info.CardSaleInfo.extraData.sWashExpire));
					memcpy(info.CardSaleInfo.cardData.sAuthCode, cardDataAll3.CardSaleAll.data.sAuthCode, sizeof(info.CardSaleInfo.cardData.sAuthCode));
					memcpy(info.CardSaleInfo.cardData.sAppCodeElectr, cardDataAll3.CardSaleAll.data.sAppCodeElectr, sizeof(info.CardSaleInfo.cardData.sAppCodeElectr));
					memcpy(info.CardSaleExtraData2.sFlags, cardDataAll3.extData2.sFlags, sizeof(cardDataAll3.extData2.sFlags)); //58096 4.0.20.40
					memcpy(info.CardSaleExtraData2.sCustomerMessage, cardDataAll3.extData2.sCustomerMessage, sizeof(cardDataAll3.extData2.sCustomerMessage)); //4.0.20.40


					
					//Encrypt relevant fields. 					
					_Module.m_server.m_cProtectedDataHandler.GenerateIndex(&info, lPumpNumber, TRUE);			//4.0.23.501 TD-60236
					if (!bIsOfflineTrs)																			//4.0.23.500 78023
						_Module.m_server.m_cMemPumpArray[p].SetMemPumpInfo(&info ,&trs); //4.0.0.6
					
					if(LOG_BASIC_FLOW_CONTROL)
					{											
						CString str;
						str.Format("SetMemPumpInfo step 1 Pump Authorize by CL");
						_LOGMSG.LogMsg(p+1,LOG_PUMP,str);
					}
				}

				long lGRTotalDiscount = a2l(cardDataAll3.CardSaleAll.extData.sGRTotalDiscount, sizeof(cardDataAll3.CardSaleAll.extData.sGRTotalDiscount)); //4.0.20.506

				if(lGRTotalDiscount)
				{
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						long lTmpTotal = 0;  // 2.9.1.1
						str.Format("Gas Rewards : GRActualDiscountPerGallon %ld , GRMaxVolume %ld (Volume = %ld)",lGRDiscount, lGRMaxVolume, lVolume ); 
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
						str.Empty();

						lTmpTotal = ConvertStringsToLong(info.CardSaleInfo.extraData.sTotalAmt , sizeof(info.CardSaleInfo.extraData.sTotalAmt),info.CardSaleInfo.extraData.sTotalAmt_Msb ,sizeof(info.CardSaleInfo.extraData.sTotalAmt_Msb));
						str.Format("Gas Rewards Redeem: (Value %ld) - (GRTotalDiscount %ld) = (TotalAmt %ld)",lValue,info.CardSaleInfo.extraData.sGRTotalDiscount,lTmpTotal); 


						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
					}
				}

				/**********************************/
			//		2. update pump trs list
				/**********************************/
				if(PUMPSRV_IS_MASTER)
				{
					_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);
				}
				else // server is slave 
				{
					if((!bMasterCancelOperation)&&(!bCancelInMiddleOfAuthorize))
						_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);
				}


				// step 3 and 4 need to move to final Force stage.
				if ((!bAutoForce) && (!bCancelInMiddleOfAuthorize) || ((_Module.m_server.GetIFSFTerminalSrvInUse() || (trs.GetRes() == PAY_AT_PUMP && trs.m_lResExt == RES_EXT_MOBILE)) && lValue))
				{
					long lRtc =1;  // TD 238845 4.0.28.500 

					/********************************************************/
				//		3. Change Trs Link number into new OLA Index number .
					/********************************************************/

					info.CardSaleInfo.lIndexNumber = _Module.m_server.NewOLAIndex();						
							
					trs.SetLink(TRS_LINK_OLA_NUMBER, info.CardSaleInfo.lIndexNumber);		

					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("New OLA Index Number - %ld (Trs %ld, Value %ld, Volume %ld) ",info.CardSaleInfo.lIndexNumber,trs.m_lNumber,trs.m_lRoundedValue,trs.m_lRoundedVolume); 
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
					}

					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;						
						str.Format("Pap Coupon given reward=%.5s, ",info.CardSaleInfo.extraData.sCouponReward);  
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
					}

					//4.0.19.508 - Transaction stayed in IN_PROGRESS status in OLA qdex (special PAP)
					if ((_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse()) && trs.GetRes() == PAY_AT_PUMP && (trs.m_lResExt == RES_EXT_PAP_EXTERNAL || trs.m_lResExt == RES_EXT_MOBILE))
					{				
						trs.SetStatus(PAID);
						_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);
						
					}

					if (bIsOfflineTrs == TRUE) //4.0.19.502
						info.CardSaleExtraData4.sIsOfflineTrs = '1';
					else
						info.CardSaleExtraData4.sIsOfflineTrs = '0';
					
					if (trs.m_lRoundedValue  && trs.m_lRoundedVolume )
					/**********************************/
					//	4. insert to OLA file
					/**********************************/
						lRtc = _Module.m_server.TrsLinkOperationEx(REC_INSERT,&trs, &info); // TD 238845 4.0.28.500
					
					// insert info to receipt file
					if (lRtc == OK)
					{
						BYTE byIndex = 0;

						BOOL bPapExternalTransaction = (trs.m_lResExt == RES_EXT_PAP_EXTERNAL);
						
						if ((trs.m_lRoundedValue  && trs.m_lRoundedVolume) && (FALSE == bPapExternalTransaction))  // not needed for TerminalSrv 
						{
							//4.0.30.0 270322 RECEIPT_INFO cReceiptInfo;					 //4.0.5.0
						

							BOOL bIsMobileTransaction = (trs.m_lResExt == RES_EXT_MOBILE);

							if (TRUE == bIsMobileTransaction)
								l2a(trs.m_lNumber, info.CardSaleInfo.cardData.sTranNbr, sizeof(info.CardSaleInfo.cardData.sTranNbr));

							/**********************************/
							//  insert to Receipt file
							/**********************************/

							CReceiptInfo ReceiptInfo;
							_Module.m_server.m_cBaseReceipt->ConvertOlaToReceipt(&trs, &info, ReceiptInfo);//4.0.9.503
							long lRtc1 = CSQLReceiptDB::GetInstance().UpdateReceipt(ReceiptInfo);
					
							if(LOG_BASIC_FLOW_CONTROL)
							{	//4.0.20.501 TD 60236
								char sTrack2Buff[ENCRYPTED_DATA_SIZE];
								
								memset(&sTrack2Buff,' ',sizeof(sTrack2Buff));
								memcpy(sTrack2Buff, cReceiptInfo.cReceipt1.Index.sTrack2, ENCRYPTED_DATA_SIZE);

								char szResult[MAX_FIELD_VAL] = {0};
								_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, (char*)sTrack2Buff, sizeof(sTrack2Buff)); 
								
								CString str;
								str.Format("New Receipt Data Ret = %ld", lRtc1); 
								_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 
							}

							if (TRUE == bIsMobileTransaction)
							{
								_Module.m_server.m_cGPIEventMgr.AddGeneralSystemEvent(GPI_EVENT_PAP_READY_CONFIRM, 0, lPumpNumber);
							}
						}

						//Add by Yaron Ctrl Events

						_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_PAP_TRS,0);
						
						if (!bIsOfflineTrs)																//4.0.23.500 78023
							_Module.m_server.m_cMemPumpArray[p].SetMemPumpInfo(&info ,&trs, &cReceiptInfo,byIndex ); //4.0.0.6 //4.0.30.0 270322
						
						if(LOG_BASIC_FLOW_CONTROL)
						{											
							CString str;
							str.Format("SetMemPumpInfo...");						//4.0.26.502
							_LOGMSG.LogMsg(p+1,LOG_PUMP,str);
						}					
					}

					if(lRtc)
					{
						if(LOG_LOGIC_ERROR)
						{
							CString str;
							str.Format("PAPReadyTransaction: Insert Operation for OLAFile Failed !!!  Trs %d, SeqNumber %d, lRtc %d", trs.m_lNumber,lSeqNumber, lRtc ); 
								_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str,LOG_LEVEL_1);
						}
						retCode = GCI_FAILED_IO;
					}

				}
				
				memset(&info.CardSaleExtraData6.LoyaltyCard, ' ', sizeof(info.CardSaleExtraData6.LoyaltyCard));
				// Save PAP Info data including Extra Data
				//------------------------------------------------
				_Module.m_server.m_cPumpArray[p].SetPAPInfo(&info);
		
			//		5. build return sCardData
			// do it in any cancel - operation moved to out of scope.
				
			//		6. Log trs in shift log
				if (!(/*_Module.m_server.GetIFSFTerminalSrvInUse() &&*/ trs.GetRes() == PAY_AT_PUMP && (trs.m_lResExt == RES_EXT_PAP_EXTERNAL || trs.m_lResExt == RES_EXT_MOBILE))) //4.0.20.0 - TD 61778
				{
					if(PUMPSRV_IS_MASTER)
					{
						_Module.m_server.m_cShiftLog.LogTrs(&trs, FALSE,FALSE);
					}
					else
					{	// PumpSrv is slave - write to log only in case that 
						// Trs found as valid one , in other cases 
						// there is no trs to report of.
						if((!bMasterCancelOperation)&&(!bCancelInMiddleOfAuthorize))			
							_Module.m_server.m_cShiftLog.LogTrs(&trs, FALSE,FALSE);
					}

					//4.0.25.50 - TD 108311
					if ((retCode == GCI_OK) && (_Module.m_server.GetSingleShiftAccumulationsActive()) && (lVolume > 0))
					{
						//Accumulate sales and store in Single shift accumulations Qdex
						_Module.m_server.m_cSingleShiftAccumMgr.AccumulateSales(lPumpNumber, nNozzle, lVolume);
					}
				}
				

			// If AutoForce :
			//		1. Trigger OLA server to "Force" operation
				if((bAutoForce)&&(!bCancelInMiddleOfAuthorize))
				{
					if(PUMPSRV_IS_MASTER)
					{
					
						if( (trs.m_lRoundedValue  == 0) && 
							(trs.m_lRoundedVolume == 0) &&
							(info.CardSaleInfo.cardData.sTranStatus != CD_APPROVE_OK) &&
							(info.CardSaleInfo.cardData.sTranStatus != CD_DECLINE_GR) &&
							(info.CardSaleInfo.cardData.sTranStatus != CD_FORCE_COMPLETION_ZERO) &&
							(info.CardSaleInfo.cardData.sTranStatus != ' ')) // RFUEL 1311
						{
						
							if(LOG_LOGIC_ERROR)
							{
								CString str;
								str.Format("Cancel transaction =%ld , status=%x",trs.m_lNumber , info.CardSaleInfo.cardData.sTranStatus);
								_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str,LOG_LEVEL_1);
							}
							
							retCode = GCI_TRS_NOT_FOUND; 
							
							if (!lTrsNumber && info.CardSaleInfo.cardData.sTranStatus == CD_DECLINE_ERROR) //4.0.17.501 merged from 4.0.15.200
							{
								CPumpTransact trs;
								if(!_Module.m_server.m_cPumpArray[lPumpNumber-1].GetInProgressWaitProgressTrs(&trs))
								{
									//solve the w_progress trs on pump
									CString str;
									str.Format("Cancel zero transaction (CD_DECLINE_ERROR)");
									_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str,LOG_LEVEL_1);
									retCode = GCI_OK;
								};								
							}
							if((_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()) && (FALSE == (_Module.m_server.m_cFuelLoayaltyPS.GetLoyaltySrvInUse() == COPIENT))) //4.0.20.51 63626	//4.0.22.504 69344
								_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber);	

						}
					}

					if(retCode == GCI_OK)
					{						
						long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_FORCE,lPumpNumber,lSeqNumber,(CARD_SALE_ALL3 *)&info.CardSaleInfo.cardData, FALSE);	//4.0.19.502 39254
						
						if(lRtc)
						{	
							retCode = GCI_FAILED_IO;								

						}
						else if (trs.m_lResExt != RES_EXT_PAP_EXTERNAL && trs.m_lResExt != RES_EXT_MOBILE) //4.0.19.504 - 47559  // CR 444830
						{						
							// Trigger OLA process
							//--------------------
							if((!bMasterCancelOperation)&&(!(lFlags & STRUCT_IS_SALE_COMMAND)))//4.0.12.501
							{ 

								if( ola.m_byState & SESSION_SALE )
									ola.m_byState = SESSION_SALE | OLA_REQUEST_TO_SEND;
								else if( (ola.m_byState & SESSION_CAPTURE) == SESSION_SALE)   //5.0.0.55
									ola.m_byState = SESSION_CAPTURE | OLA_REQUEST_TO_SEND;							
								else
								{

									LoyaltyEndPapTrs(lPumpNumber, lFlags ,lGradePrice,lValue ,lVolume ,nGrade,info, trs, ola,retCode); // 4.0.23.2070  TD 384668	
									long  lItemRestriction = a2l(cardDataAll3.extData3.sItemRestriction, sizeof(cardDataAll3.extData3.sItemRestriction)); //4.0.20.506

									if (lItemRestriction)
									{
										if ((lItemRestriction & ADD_REEFER) && (lItemRestriction & ADD_DIESEL) &&  //4.0.12.501
											(lValue))  //4.0.10.220.  authorize reefer only if diesel was fueled     
										{
											ola.m_byState = SESSION_COMPLETE | OLA_WAIT_REEFER; //4.0.9.601
											
											l2a(lValue ,info.CardSaleExtraData3.sReeferTotal, sizeof(info.CardSaleExtraData3.sReeferTotal));  //4.0.10.491
											l2a(lVolume ,info.CardSaleExtraData3.sReeferVolume, sizeof(info.CardSaleExtraData3.sReeferVolume));
											l2a(nGrade,info.CardSaleInfo.extraData.SALES[0].sDPTProdCode , sizeof(info.CardSaleInfo.extraData.SALES[0].sDPTProdCode));  //4.0.9999.0
											l2a(lGradePrice ,info.CardSaleExtraData2.sGradePrice , sizeof(info.CardSaleExtraData2.sGradePrice));
											
											trs.m_lRoundedValue = 0;   //4.0.10.49
											trs.m_lRoundedVolume = 0;
											trs.m_lRawVolume = 0;
											_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);
											_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&info);   //4.0.10.491
											// reserved transaction to PAK fleet
											CMD_STRUCT_RESERVE_PUMP cmdReservePump;
											cmdReservePump.lRes = PAY_AT_KIOSK;
											cmdReservePump.lResExt = RES_EXT_FLEET; //4.0.10.47
											_Module.m_server.SetCommand(CMD_RESERVE_PUMP, 0, lPumpNumber,&cmdReservePump,sizeof(CMD_STRUCT_RESERVE_PUMP));						

								

										} 
										else if ((lItemRestriction & ADD_CASH_BACK) || 
											  (lItemRestriction & ADD_DRY_ITEMS))
										{
											CFleetCard cFleetCard;
											// Convert PAP to PAK
											cFleetCard.ConvertFleetPap2Pak(lPumpNumber ,trs , info);
										}
										else  
										//if we work with loyalty then we dont need to change the flags
										//other wise we use the regular flow
										if(!_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())
											ola.m_byState = SESSION_COMPLETE | OLA_REQUEST_TO_SEND;  //4.0.9.507
									}
									else										
										//if we work with loyalty then we dont need to change the flags
										//other wise we use the regular flow
										if(!_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())
											ola.m_byState = SESSION_COMPLETE | OLA_REQUEST_TO_SEND;  //4.0.9.507
								}
							}
							else // Master cancel
								ola.m_byState = SESSION_CANCEL | OLA_REQUEST_TO_SEND;

							ola.m_lSessionNumber = 0; 
							if(LOG_BASIC_FLOW_CONTROL)
								_LOGMSG.LogOLAMsg(lPumpNumber,ola.m_byState,ola.m_lSessionNumber);

							_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 	


						}
					}
				}
			}
			
			switch (retCode)
			{	
				case GCI_FAILED_IO:
				case GCI_PAP_REQUEST_OUT_OF_SYNC:		
					// in this case simply ignore request 
					// assuming that controller keep update us.
					break;

				case GCI_TRS_NOT_FOUND:
				case GCI_TRS_ALREADY_PAID:								 
				case GCI_INVALID_SEQ_NUMBER:
					// Send SwipeResult for the final information -  FORCE_NOT_OK 
					if(OLA_BY_PUMPSRV)
					{
						long lTmpTotalAmt = 0;
						
						lTmpTotalAmt = ConvertStringsToLong(info.CardSaleInfo.extraData.sTotalAmt,sizeof(info.CardSaleInfo.extraData.sTotalAmt),
							info.CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(info.CardSaleInfo.extraData.sTotalAmt_Msb));
						
						if(LOG_BASIC_FLOW_CONTROL)  //4.0.13.202
						{											
							CString str;
							str.Format("PAPReadyTransaction  Pump=%ld  get PreAuthorized=%ld", p + 1 ,lTmpTotalAmt);
							_LOGMSG.LogMsg(str);
						}

						//When we failed to finalizes PAP used the latest information instead used of PreAuth details   
						info.CardSaleInfo.extraData = cardDataAll3.CardSaleAll.extData;
						info.CardSaleInfo.cardData = cardDataAll3.CardSaleAll.data;
						info.CardSaleExtraData2 =  cardDataAll3.extData2;
												
						if (STRUCT_IS_CARD_ALL_DATA3 & lFlags )
						{
							info.CardSaleExtraData3 = cardDataAll3.extData3;
							info.CardSaleExtraData4 = cardDataAll3.extData4;
							info.CardSaleExtraData5 = cardDataAll3.extData5;
							info.CardSaleExtraData6 = cardDataAll3.extData6;

							if (STRUCT_IS_PAP_CANCEL & lFlags )  //4.0.7.54
							{								
								if (!_Module.m_server.GetOLAFlags(info.CardSaleExtraData3.sFlags1 ,FL_CANCEL_REC,sizeof(info.CardSaleExtraData3.sFlags1)))					
								{
									CString sMsg;
									sMsg.Format(_T("%s:%d Set - FL_CANCEL_REC"), __FUNCTION__, __LINE__);
									_LOGMSG.LogMsg(sMsg);
									long lTmp =  a2l(info.CardSaleExtraData3.sFlags1,sizeof(info.CardSaleExtraData3.sFlags1)) + FL_CANCEL_REC;
									l2a(lTmp ,info.CardSaleExtraData3.sFlags1, sizeof(info.CardSaleExtraData3.sFlags1));
								}
							}
						}

						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString tmpStr;
							tmpStr.Format("PAP Force not ok ResExt=%ld"  ,trs.m_lResExt);
							_LOGMSG.LogMsg(tmpStr);
						}

						if (trs.m_lResExt == RES_EXT_FLEET)
						{
							PS::RECEIPT_EXTRA_INFO_REC cReceiptExtraRec;
							CString tmpStr;

							memset(&cReceiptExtraRec, ' ', sizeof(PS::RECEIPT_EXTRA_INFO_REC));

							cReceiptExtraRec.info.lIndexNumber = a2l(info.CardSaleInfo.cardData.sTranNbr, sizeof(info.CardSaleInfo.cardData.sTranNbr));

							long lRtc1 = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_DELETE, cReceiptExtraRec);
							//long lRtc1 = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_DELETE,&cReceiptExtraRec);


							if (lRtc1)
							{
								if (LOG_BASIC_FLOW_CONTROL)
								{
									tmpStr.Format("GCI::PAPReadyTransaction() - fail delete Receipte record trs = %ld, Error = %ld", cReceiptExtraRec.info.lIndexNumber, lRtc1);
									_LOGMSG.LogMsg(tmpStr);
								}
							}
							else
							{
								if(LOG_BASIC_FLOW_CONTROL)
								{
									tmpStr.Format("GCI::PAPReadyTransaction() - Succeeded delete Receipte record trs = %ld", cReceiptExtraRec.info.lIndexNumber);
									_LOGMSG.LogMsg(tmpStr);
								}

							}

						}
						
						if (!(lFlags &  STRUCT_IS_UPDATE_RECEIPT))
						{
							if (!bIsOfflineTrs)														//4.0.23.500 78023
								_Module.m_server.m_cMemPumpArray[p].SetMemPumpInfo(&info ,&trs);
							if(LOG_BASIC_FLOW_CONTROL)
							{											
								CString str;
								str.Format("SetMemPumpInfo  Step 2 Pump Authorize by OLA AmountLimit=%ld" , lTmpTotalAmt); //4.0.13.200
								_LOGMSG.LogMsg(p+1,LOG_PUMP,str);
							}
						}
						
						if(_Module.m_server.m_cProtectedDataHandler.IsEncodeActive())									//4.0.23.501 TD-60236							
						{							
							_Module.m_server.m_cProtectedDataHandler.GenerateIndex(&info);					//4.0.23.501 TD-60236
							_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&info);
							SQLPumpSrvFile.LoadPAPInfo(lPumpNumber);

						}
						
					//4.0.23.801 163818 4.0.26.503 if (!_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())	//4.0.22.504 69344
					{
							//4.0.22.0 70991 - start - special treatment for card block - we need to send the zones for the receipt
						if(info.CardSaleInfo.cardData.sTranStatus == CD_DECLINE_CARD_BLOCKED)
						{
							_Module.m_server.SendReceipt(		ola.m_byState, p + 1, CD_FORCE_NOT_OK, info.CardSaleInfo.cardData.sTranStatus, (CARD_SALE_ALL3 *)&info.CardSaleInfo.cardData, &trs);
						}
						else //4.0.22.0 70991 - end
							_Module.m_server.SendSwipeResult(	ola.m_byState, p + 1, CD_FORCE_NOT_OK, info.CardSaleInfo.cardData.sTranStatus, (CARD_SALE_ALL3 *)&info.CardSaleInfo.cardData);
						}
					}
				 break;
			}
			
			if (lFlags &  STRUCT_IS_UPDATE_RECEIPT)
			{
				CReceiptInfo ReceiptInfo;
				BYTE byIndex = 0;


				//encrypt the account end delete track1,track2
		

				_Module.m_server.m_cProtectedDataHandler.RemoveConfidencesData(&info); // RFUEL-1347 

				if (trs.m_lNumber == 0)
					trs.m_lNumber = lSeqNumber;
				
					//Encrypt relevant fields. 					
				_Module.m_server.m_cProtectedDataHandler.GenerateIndex(&info, FALSE, trs.m_lNumber);			//4.0.23.501 TD-60236

				_Module.m_server.m_cBaseReceipt->ConvertOlaToReceipt(&trs, &info, ReceiptInfo);//4.0.9.503
				//_Module.m_server.ConvertOlaToCReceiptInfo(&trs, &info, ReceiptInfo);
				long lRtc1 = CSQLReceiptDB::GetInstance().UpdateReceipt(ReceiptInfo);

				if (LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("New Receipt index[%d], TotalAmt[%.5s], Ret = %ld", ReceiptInfo.m_TrsIndex, info.CardSaleInfo.extraData.sTotalAmt, lRtc1);
					_Module.m_server.m_cLogger.LogMsg(str);
				}
			}

			if (bCancelInMiddleOfAuthorize )
			{
				if (_Module.m_server.GetOLAFlags(info.CardSaleExtraData3.sFlags1, FL_OLA_EXTRA_DATA, sizeof(info.CardSaleExtraData3.sFlags1))) //4.0.24.90 118124
				{// Delete ola extra info from Ash2000 qdx.
					_Module.m_server.DeleteFromAsh2000Qdx(trs.m_lNumber);
				
				}

				//if (!bIsOfflineTrs)															//4.0.23.500 78023
					//_Module.m_server.m_cMemPumpArray[p].SetMemPumpInfo(&info ,&trs,&ReceiptInfo); //4.0.30.0 270322

				if (!bIsOfflineTrs)		
					_Module.m_server.m_cMemPumpArray[p].SetMemPumpInfo(&info ,&trs);

				if(LOG_BASIC_FLOW_CONTROL)
				{											
					CString str;
					str.Format("SetMemPumpInfo  Step 3, ola = %x , sTranStatus = %c", ola.m_byState, info.CardSaleInfo.cardData.sTranStatus);
					_LOGMSG.LogMsg(p + 1, LOG_PUMP, str);
				}
			
				_Module.m_server.SendSwipeResult(ola.m_byState, p + 1, CD_FORCE_NOT_OK, info.CardSaleInfo.cardData.sTranStatus, (CARD_SALE_ALL3 *)&info.CardSaleInfo.cardData);
			}

			if(LOG_BASIC_FLOW_CONTROL)
			{											
				CString str;
				str.Format("PAPReadyTransaction, final Record len is = %ld ", lOlaRecordSize );
				_LOGMSG.LogMsg(p+1,LOG_PUMP,str);
			}


		//		5. build return sCardData

			char sBuff[MAX_XML_BUF_LEN];
			memset(&sBuff, 0, sizeof(sBuff));

			bRet = _Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml((char*)&sBuff, &info);

			if (!bRet)
				_LOGMSG.LogMsg("XmlProblem: PAPReadyTransaction, GetXmlString of sBuff5",LOG_LEVEL_1);

			if (_Module.m_server.m_bUseXmlBuffer)
			{
				lOlaRecordSize = MAX_XML_BUF_LEN;
			}

			size_t BufferSize = lOlaRecordSize + 2;
			char *sTmpAllocationBuffer = new char[BufferSize];	//4.0.5.0
			memset(sTmpAllocationBuffer,0, BufferSize);

			if (_Module.m_server.m_bUseXmlBuffer)
			{
				//strcpy_s((char*)sTmpAllocationBuffer, strlen(sTmpAllocationBuffer) + 1 /*  lOlaRecordSize+2 */,  (char*)&sBuff);
				strcpy_s(sTmpAllocationBuffer, BufferSize, (LPCSTR)&sBuff);
			}
			else
			{
				Str2StrNull((BYTE *)&info.CardSaleInfo.cardData,lOlaRecordSize, (BYTE *) sTmpAllocationBuffer);
			}

			CComBSTR sCardData(lOlaRecordSize +2 ,(LPCSTR)sTmpAllocationBuffer); 			

			delete[] sTmpAllocationBuffer;   //4.0.5.0
			
			*sRetData = sCardData.Copy(); 

			if (retCode == GCI_OK) //4.0.23.502
			{
				_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&info);
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_FORCE, lPumpNumber, lSeqNumber, (CARD_SALE_ALL3 *)&info.CardSaleInfo.cardData);

				if (lRtc)
					retCode = GCI_FAILED_IO;		
			}

			sFinalLineMsg = _LOGMSG.GetFinalLineMsg(trs.GetLinkFlags());
		
	}
	
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetLoyaltyPromptRetry(0);

	
//  Clear old misc //4.0.1.38
	if (retCode != GCI_INVALID_PUMP_NUMBER)
	{
		CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber -1].GetStat();
		long lMisc = pumpStat.GetMisc();

		if (lMisc & MISC_NEED_MORE_PARAM)
		{
			lMisc &= MISC_NOT_NEED_MORE_PARAM;	
			pumpStat.SetMisc(lMisc);
			_Module.m_server.m_cPumpArray[lPumpNumber -1].SetStat(pumpStat);			

		}

		_Module.m_server.DeleteMiscInfo(lPumpNumber);

		if (lMisc & MISC_APPROVED_PAP)
		{
			lMisc &= MISC_NOT_APPROVED_PAP;	
			pumpStat.SetMisc(lMisc);
			_Module.m_server.m_cPumpArray[lPumpNumber -1].SetStat(pumpStat);			

		}

		if(!(lFlags & SWIPE_VIT_CONFLICT)) // Clear vit conflict
		{											
			if (lMisc & MISC_VIT_CONFLICT)
			{
				lMisc &=MISC_NOT_VIT_CONFLICT;
				pumpStat.SetMisc(lMisc);
				_Module.m_server.m_cPumpArray[lPumpNumber -1].SetStat(pumpStat);			

			}					
		}
	}
	
	if(_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()) //4.0.23.502
		_Module.m_server.m_cFuelLoayaltyPS.SetCancelTrs(lPumpNumber, NoCancel);//4.0.22.504 69344 74977
	if (retCode == GCI_OK)   
	{	
		_Module.m_server.SavePumpInfo(lPumpNumber);
	}
	
	
	// M.L 8/5/98 Lock PumpArray in large scope 
	//-----------------------------------------		
//	LeaveCriticalSection(&_Module.m_server.m_csPumpArray);	//4.0.7.503	 

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"PAPReadyTransaction",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);


	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;

		str.Format("PAPReadyTransaction(Pump %02ld, SeqNumber %05ld, Trs %05ld, Nozzle %02d, Volume %04ld, Value %04ld, Level %d, Grade Price %04ld, Grade %02d, Flags=%ld)",
			lPumpNumber, lSeqNumber, lTrsNumber, nNozzle, lVolume, lValue, nLevel, lGradePrice, nFinalGrade, lFlags);

		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sFinalLineMsg);

	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGCI::RollingUpdate(long lPumpNumber,long lTrsNumber, long lVolume, long lValue, long lGradePrice, short nGrade, long * pVal)
{
	
//	_LOGMSG.LogLocationIn("GCI::RollingUpdate()",0);

#ifdef CATCH_GCI
	__try
	{
#endif
		RollingUpdate__(lPumpNumber,lTrsNumber,lVolume,lValue,lGradePrice,nGrade,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::RollingUpdate")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::RollingUpdate()", LOG_LEVEL_0);

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::RollingUpdate()",0);
	return S_OK;
}

void CGCI::RollingUpdate__(long lPumpNumber,  long lTrsNumber, long lVolume,  long lValue,  long lGradePrice,  short nGrade, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if(GRADE_NOT_VALID(nGrade))
		retCode = GCI_INVALID_GRADE_NUMBER;

	if(retCode == GCI_OK)
	{
		CPumpTransact	trs;
		long			p = lPumpNumber-1;
		BOOL			bFound = FALSE;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		short nFinalGrade = nGrade;
		if(_Module.m_server.NozzleLevelLogic())
			if(nGrade) //In case we use NozzleLevelLogic, the field nGrade use for Nozzle number.
				nFinalGrade = _Module.m_server.m_cPumpArray[p].NozzleToGrade(nGrade);

		if(_Module.m_server.m_cPumpArray[p].GetCurrentTrs(&trs))
		{ // not found 
			if(!_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_NO_LINK,&trs))
			{ // not found 
				retCode = GCI_TRS_NOT_FOUND;
				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("Rolling Update, no current TRS !!!, Volume %ld, Value %ld " , lVolume , lValue); 
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
				}

				if((lVolume != 0L)||(lValue != 0L)) // M.L 1.10.98
				{
					// Trigger controller to send pump status 
					_Module.m_server.SetCommand(CMD_GET_PUMP_STATUS, SERVER_POS_NUMBER, lPumpNumber);

					if(LOG_DETAIL_FLOW_CONTROL)
					{							
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"PumpSrv request for pump status update !!! "); 
					}
				}
			}
			else
			{
				bFound = TRUE;
			}
		}
		else // Trs Found in list - just update.
		{
			bFound = TRUE;
		}


		if(bFound) 
		{
			BOOL bDoUpdate = TRUE;

			if(trs.GetStatus() != IN_PROGRESS) 
				bDoUpdate = FALSE;
							
// 			if ((trs.GetLinkFlags() == TRS_LINK_OLA_NUMBER) && (trs.m_lResExt != RES_EXT_FLEET)) //4.0.10.491
// 			{
// 				OLA_STAT ola;
// 				_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
// 				 if((OLA_STATE(ola.m_byStat) != OLA_NONE) && (!_Module.m_server.IsPumpHotAuthorized(lPumpNumber,ola.m_byStat)) && (OLA_STATE(ola.m_byStat) != OLA_LOYALTY_WAIT_FUELING))	//4.0.22.506 75154
// 					bDoUpdate = FALSE;
// 			}

			if (_Module.m_server.IsRollingUpdateDataNeedtoAddToLog())
			{
				CString str;
				str.Format("RollingUpdate GradeID=%03ld, Price=%08ld, Volume=%08ld, Value=%08ld ", nGrade, lGradePrice, lVolume, lValue);
				_LOGMSG.LogMsg("GCI", "SetPumpStatus", lPumpNumber, LOG_PUMP, str);
			}
							
			if(bDoUpdate)
			{
				//4.0.20.500 start
				BOOL bRolling = TRUE;
				
				//In case it is the first time we would like to update second machine
				if((trs.m_lRoundedVolume == 0) && (trs.m_lRoundedValue == 0))
					bRolling = FALSE;
				//4.0.20.500 end

				//4.0.7390.1800
				if ((trs.m_lRoundedVolume < lVolume) || (trs.GetLinkFlags() && TRS_LINK_PREPAY_NUMBER) )
				{
					trs.m_lRoundedVolume = lVolume;
					trs.m_lRawVolume = lVolume;
				}

				//4.0.7390.1800
				if ((trs.m_lRoundedValue < lValue) || (trs.GetLinkFlags() && TRS_LINK_PREPAY_NUMBER) )
					trs.m_lRoundedValue = lValue;
				else
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{	
						CString str;
						str.Format("Ignore ... Rolling Update for pump %02ld, no current TRS !!! current=%ld, Volume %ld, Value %ld ", lPumpNumber , trs.m_lRoundedValue ,lVolume , lValue); 
						_LOGMSG.LogMsg(str);
					}
				}

				if (lGradePrice)
					trs.m_lGradePrice = lGradePrice;

				if ((nFinalGrade) && (lValue || lVolume))  //4.0.6190.4600
					trs.m_nGrade = nFinalGrade;

				trs.m_sTypeOfMeasure = _Module.m_server.GetTypeOfMeasure(trs.m_nGrade); // TD 363041

				//4.0.19.500 end
				// M.L 8/5/98 Lock PumpArray in large scope 
				_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,bRolling);		//4.0.22.506
			}
			else
			{
				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("Ignore Rolling Update , Trs %ld already finalized !!", trs.m_lNumber);  //4.0.10.49
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
				}
			}
		}
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
//		LeaveCriticalSection(&_Module.m_server.m_csPumpArray);
	}		

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"RollingUpdate",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGCI::TransactionStatusCM(long lPumpNumber, long lTrsNumber, short nNozzle, short nIndex, long lStatus, long lOnOffer, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long * pVal)
{
//	_LOGMSG.LogLocationIn("GCI::TransactionStatusCM()",0);

#ifdef CATCH_GCI
	__try
	{
#endif
		TransactionStatusCM__(lPumpNumber,lTrsNumber,nNozzle, nIndex,lStatus,lOnOffer,lVolume,lValue,nLevel,lGradePrice,nGrade,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::TransactionStatusCM")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::TransactionStatusCM()", LOG_LEVEL_0);

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::TransactionStatusCM()",0);
	return S_OK;
}

void CGCI::TransactionStatusCM__(long lPumpNumber, long lTrsNumber,  short nNozzle,   short nIndex,  long lStatus,long lOnOffer, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if(l2TrsStatus(lStatus) == UNKNOWN_TRS_STATUS)
		retCode = GCI_UNKNOWN_TRANSACTION_STATUS;

	else if(GRADE_NOT_VALID(nGrade))
		retCode = GCI_INVALID_GRADE_NUMBER;

	else if(LEVEL_NOT_VALID(nLevel))
		retCode = GCI_INVALID_LEVEL_NUMBER;

	else if(NOZZLE_NOT_VALID(nNozzle))
		retCode = GCI_INVALID_NOZZLE_NUMBER;


	BOOL nNoInformation = FALSE;

	if( (l2TrsStatus(lStatus) == NULL_TRS) && (lTrsNumber == 0))
		nNoInformation = TRUE;
	
	if((retCode == GCI_OK) && (nNoInformation == FALSE))
	{
		int				iNotFound = 0;
		long			p = lPumpNumber-1;

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		long			lShift = _Module.m_server.m_cPumpArray[p].GetShiftNumber();
		CPumpTransact	trs(lTrsNumber,lShift);

		
		iNotFound = _Module.m_server.m_cPumpArray[p].GetTrsByNumber(lTrsNumber,&trs);
		
		// update information for both cases : 
		// new and known transaction
		TrsStatus eOldTrsStatus = trs.m_lStatus;						//4.0.23.500 78062
			
		trs.m_lNumber			= lTrsNumber;
		trs.m_lOnOffer			= lOnOffer;
		trs.m_nNozzle			= nNozzle;
		trs.m_nLevel			= nLevel;
		trs.m_lGradePrice		= lGradePrice;
		trs.m_nGrade			= nGrade;  
		trs.m_lRoundedVolume	= lVolume;
		trs.m_lRawVolume		= lVolume;
		trs.m_lRoundedValue		= lValue;
		trs.m_lActiveShiftNumber = lShift;
		trs.m_lPump				= lPumpNumber;							
		trs.SetStatus(l2TrsStatus(lStatus));				

		if(iNotFound)
		{ // Trs Not Found in list - new one.

			trs.UpdateTime();											//4.0.23.500 78062 update the time when trs was created.
			if((lStatus == IN_PROGRESS)||(lStatus == WAIT_PROGRESS))
			{
				trs.m_lRes = _Module.m_server.m_cPumpArray[p].GetStat().GetRes();
				trs.m_lResExt = _Module.m_server.m_cPumpArray[p].GetStat().GetResExt();
				trs.m_lMode = _Module.m_server.m_cPumpArray[p].GetStat().GetMode();
				switch(trs.m_lRes)
				{
					case PAY_AT_KIOSK: 
						trs.m_nLinkFlags = TRS_NO_LINK;
						break;
					case PAY_AT_PUMP:
						trs.m_nLinkFlags = TRS_LINK_OLA_NUMBER;
						trs.m_lLinkNumber = _Module.m_server.m_cPumpArray[p].GetPAPSeqNumber();
						break;
					case PRE_PAY_AT_POS:
						trs.m_nLinkFlags = TRS_LINK_PREPAY_NUMBER;
						trs.m_lLinkNumber = lTrsNumber;
						break;
					case NULL_RES:
						trs.m_nLinkFlags = TRS_NO_LINK;
						break;
				}
				if (lStatus == IN_PROGRESS)								//4.0.23.500 78062
				{
					//update start fueling time only once at the beginning of fueling
					trs.UpdateStartFuelingTime(); //4.0.22.511 72606
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("TransactionStatausCM a new trs %d was created and startFuelingTime was updated", lTrsNumber); 
						_LOGMSG.LogMsg(trs.m_lPump,LOG_PUMP,str);
					}
				}
			}
		
			long lRetNum = _Module.m_server.CentralUpdateTrsByIndex(lPumpNumber,lTrsNumber, nIndex, &trs,FALSE);
			if(lRetNum) 
				retCode = GCI_INVALID_INDEX_NUMBER;
		}
		else // Trs Found in list - just update.
		{
			// update information related for 
			// known transaction
			if ((lStatus == IN_PROGRESS) && (eOldTrsStatus != IN_PROGRESS))	//4.0.23.500 78062
			{
				trs.UpdateStartFuelingTime(); //4.0.22.511 72606
				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("TransactionStatausCM a trs %d was found and startFuelingTime was updated", lTrsNumber); 
					_LOGMSG.LogMsg(trs.m_lPump,LOG_PUMP,str);
				}
			}

			long lRetNum = _Module.m_server.CentralUpdateTrsByIndex(lPumpNumber,lTrsNumber, nIndex, &trs,FALSE);
			if(lRetNum) 
				retCode = GCI_INVALID_INDEX_NUMBER;
			

			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("TransactionStatausCM Trs %d, Found in PumpSrv list (Status %d, LinkFlags %d, LinkNumber %ld, Grade %d, GradePrice %ld, old status %d)",trs.m_lNumber,trs.m_lStatus,trs.m_nLinkFlags,trs.m_lLinkNumber, trs.m_nGrade,trs.m_lGradePrice, eOldTrsStatus); 
				_LOGMSG.LogMsg(trs.m_lPump,LOG_PUMP,str);
			}
		}

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"TransactionStatusCM",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::TransactionStatusCS(long lPumpNumber, long lTrsNumber ,short nNozzle, long lStatus, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long * pVal) 	
{
	
//	_LOGMSG.LogLocationIn("GCI::TransactionStatusCS()",0);

#ifdef CATCH_GCI
	__try
	{
#endif
		TransactionStatusCS__(lPumpNumber,lTrsNumber,nNozzle, lStatus, lVolume,lValue,nLevel,lGradePrice,nGrade,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::TransactionStatusCS")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::TransactionStatusCS()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::TransactionStatusCS()",0);

	return S_OK;
}

void CGCI::TransactionStatusCS__(long lPumpNumber, long lTrsNumber,  short nNozzle,  long lStatus,long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if(l2TrsStatus(lStatus) == UNKNOWN_TRS_STATUS)
		retCode = GCI_UNKNOWN_TRANSACTION_STATUS;

	else if(GRADE_NOT_VALID(nGrade))
		retCode = GCI_INVALID_GRADE_NUMBER;

	else if(LEVEL_NOT_VALID(nLevel))
		retCode = GCI_INVALID_LEVEL_NUMBER;

	else if(NOZZLE_NOT_VALID(nNozzle))
		retCode = GCI_INVALID_NOZZLE_NUMBER;


	if(retCode == GCI_OK)
	{
		int				iNotFound = 0;
		long			p = lPumpNumber-1;


		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		long			lShift = _Module.m_server.m_cPumpArray[p].GetShiftNumber();
		CPumpTransact	trs(lTrsNumber,lShift);
		
		iNotFound = _Module.m_server.m_cPumpArray[p].GetTrsByNumber(lTrsNumber,&trs);
		
		// update information for both cases : new 
		// and known transaction

		trs.m_nNozzle			= nNozzle;
		trs.m_nLevel			= nLevel;
		trs.m_lGradePrice		= lGradePrice;
		trs.m_nGrade			= nGrade;
		trs.m_lRoundedVolume	= lVolume;
		trs.m_lRawVolume		= lVolume;
		trs.m_lRoundedValue		= lValue;
		trs.SetStatus(l2TrsStatus(lStatus));				
		trs.m_lActiveShiftNumber = lShift;
		trs.m_lPump = lPumpNumber;

		if(iNotFound)
		{ // Trs Not Found in list - new one.
		
			// M.L 8/5/98 Lock PumpArray in large scope 
			long iRetNum = _Module.m_server.CentralInsertTrs(lPumpNumber,&trs);
			switch(iRetNum)
			{
				case 0: 
					retCode = GCI_OK;
					break;
				case 1: 
					retCode = GCI_PUMP_TRS_LIST_IS_FULL;
					break;
				case 2: 
					retCode = GCI_PUMP_ALREADY_HAVE_IN_PROGRESS_TRS;
					break;
				case 4: 
					retCode = GCI_TRS_NOT_FOUND;
					break;
			}
							
		}
		else // Trs Found in list - just update.
		{
			// update information related for 
			// known transaction

			// M.L 8/5/98 Lock PumpArray in large scope 
			_Module.m_server.CentralUpdateTrs(lPumpNumber,lTrsNumber,&trs,FALSE);
		}

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"TransactionStatusCS",
				TRUE,retCode,
				FALSE,0);
	

	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGCI::DoAutomaticPAKReservation(BOOL * bAutoRes, long * pVal)
{
	
//	_LOGMSG.LogLocationIn("GCI::DoAutomaticPAKReservation()",0);

	if( bAutoRes == NULL )
	{	
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"IGCI::DoAutomaticPAKReservation   Throw COM Exception : E_POINTER", LOG_LEVEL_0);
		return E_POINTER;
	}
	
#ifdef CATCH_GCI
	__try
	{
#endif
		DoAutomaticPAKReservation__(bAutoRes,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::DoAutomaticPAKReservation")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::DoAutomaticPAKReservation()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::DoAutomaticPAKReservation()",0);
	return S_OK;
}

void CGCI::DoAutomaticPAKReservation__( BOOL *bAutoRes, long *pVal)
{
	GCIRetCode retCode = GCI_OK;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	if(retCode == GCI_OK)
	{
		if(AUTOMATIC_PAK_RES)
			*bAutoRes = TRUE;
		else
			*bAutoRes = FALSE;
	}
	else if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"DoAutomaticPAKReservation",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::GetCurrentTransactionNumber(long lPumpNumber, long * pTrsNumber, long * pVal)
{
//	_LOGMSG.LogLocationIn("GCI::GetCurrentTransactionNumber()",0);
	if(pTrsNumber == NULL)
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"IGCI::GetCurrentTransactionNumber   Throw COM Exception : E_POINTER", LOG_LEVEL_0);
		return E_POINTER;
	}
	
#ifdef CATCH_GCI
	__try
	{
#endif
		GetCurrentTransactionNumber__(lPumpNumber,pTrsNumber,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetCurrentTransactionNumber")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","GetCurrentTransactionNumber()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::GetCurrentTransactionNumber()",0);
	return S_OK;
}


void CGCI::GetCurrentTransactionNumber__( long lPumpNumber,  long *pTrsNumber, long *pVal)
{
	_Module.m_server.GetCurrentTransactionNumber(lPumpNumber, pTrsNumber, pVal);
}

unsigned int _stdcall CGCI::CmdTx(void * pv)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (pv == NULL )
		return -1;    // illegal parameter

    CThreadInfo* pInfo = (CThreadInfo*)pv;
	BOOL		bKillThread = FALSE;
	static int	iRetries = 0;
	DWORD		dwData = 0;
	
	long		lPriceProtectionCheckTime = 0;				 //4.0.23.2561  	
	long		lPriceAndShiftCheckTime = 0;				
	long		lDeliveryAndTankCheckTime = 0;		
	long        lWetStockInterval = 0;         //4.0.2.30
	long		lModeSchemeCheckTime = TIME_BETWEEN_PRICE_AND_SHIFT_CHECKS;
	long 		lCheckDispensingTime = 0;
	long		lCheckUnpaidTime = 0;
	long		lCheckAutoReconcile = 0;//4.0.9.506 //amram
	long		lCheckAuthorizedTime = 0;
	long		lCheckCarWashTime = 0;
	long		lCheckCarWashGeneralRequestTime = 0; //4.0.23.504 76432
	long		lCheckCarWashClearTime = 0;
	long		lCheckReceiptTime = 0;
	long		lCheckReceiptClearTime = 0;
	long		lCheckMemoWetstock = 0 ;         //add by Yaron
	long		lCheckWetstockClearTime = 0 ;    //add by Yaron
	long		lCheckRealTimeClockTime = 0 ;    //add by Yaron
	long		lCheckDebitShiftInProcessTime =0;//3.2.0.69
	long		lCheckLockPumpByModeTime = 0;
	long		lPriceChangeCheckTime = 0;  //4.0.8.501
	long		lCheckReserveTime = 0;		//4.0.17.20 - TD 26384
	long		lExportSiteConfigurationTime = 0;	//4.0.26.508	TD 146575
	long		lCheckAuthReconcileTime = 0; // TD 233242 4.0.23.1050
	
	//@@@
	BOOL        bDeliveryTimeOut = FALSE;
	//@@@

#ifdef USE_XML_PRICE_CHANGE  //4.0.0.86
	long		lCheckNewIncomingPrice = 0 ; //add by Yaron
#endif

	unsigned long	lCheckNozzleLeftOut = 0;								//4.0.1.38
	BOOL			bNozzleLeftOutMiscRaised = FALSE;						//4.0.2.161
	unsigned long	lNozzleLeftOutCheckInterval = 0;

	HRESULT			hRes = NULL;
	static BOOL		bFirstTimeModesProcess = TRUE; //4.0.17.501 merge from 4.0.15.170
	DWORD			dwThreadSleepDuration = THRD_SLEEP_TIME;				//4.0.23.507 TD 89047

	hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	_Module.m_server.SetCLIsGone(FALSE);									//4.0.23.507 TD 89047

__try
{

	while (TRUE)
	{
		// Wait until the main application thread asks 
		// this thread to start.
		if (WaitForSingleObject(pInfo->m_hEventStartThread, INFINITE)
			!= WAIT_OBJECT_0)
			break; //  Terminate this thread by existing the proc.

		// Exit the thread if the main application sets the "kill thread"
		// event.

		if (WaitForSingleObject(pInfo->m_hEventKillThread, 0)
			== WAIT_OBJECT_0)
			break; // Terminate this thread by existing the proc.
		
		bKillThread = FALSE;
				
		while (TRUE)	
		{
			if (WaitForSingleObject(pInfo->m_hEventKillThread, 0)
				== WAIT_OBJECT_0)
			{
				bKillThread = TRUE;
				break; // Terminate this thread by existing the proc.			
			}

			if (WaitForSingleObject(pInfo->m_hEventStopThread, 0)
				== WAIT_OBJECT_0)
			{
				_Module.m_server.SetServerState(SERVER_NO_CONVERT);   //4.0.8.508
				break; // Terminate main loop and wait for start
			}
 
			if(pInfo->m_pServer->CmdInQ())  //4.0.15.501
			{				
				//=================================
				// Handle Message Queue
				//=================================

				long	lQueueHandleRetCode = OK;
				dwThreadSleepDuration = 0;						//4.0.23.509 TD 95864

				__try
				{
					lQueueHandleRetCode = pInfo->m_pGCI->QueueHandle(pInfo);

					if (lQueueHandleRetCode != OK) 			//4.0.23.508 TD 89047
					{
						++iRetries;				            // 4.0.31.??  TD 342402 
						char sLogMsg[MAX_FIELD_VAL];
						sprintf_s(sLogMsg,MAX_FIELD_VAL, "GCI:CmdTx. QueueHandle Failed: QueueHandleRetCode=%ld, Error Retries==%ld, Max Retries=%ld" ,lQueueHandleRetCode, iRetries, MAX_GCIEVENTS_RETRIES);
						_LOGMSG.LogMsg(sLogMsg, LOG_LEVEL_1);	
					}
					else
						iRetries = 0;                       // 4.0.31.??  TD 342402

				}
				__except(_HANDLE_EXCEPTION("GCI::QueueHandle")) //4.0.15.450
				{
					// we arrive here on error only !

					if(LOG_ERROR) 
						_LOGMSG.LogMsg("GCI::QueueHandle()", LOG_LEVEL_0);	

					exit(1);
				}

				if(iRetries >= MAX_GCIEVENTS_RETRIES)						//4.0.23.508 TD 89047
				{	
					//THRD_CANNOT_WORK
					if(LOG_ERROR)
					{
						pInfo->m_pServer->m_cLogger.LogMsg("CmdTx Stoped (max retries on GCIEvents)", LOG_LEVEL_1);
					}

					iRetries = 0;
					
					pInfo->m_pGCI->StopCmdTxThread();
					_Module.m_server.SetCLIsGone(TRUE);							//4.0.23.508 TD 89047

					dwThreadSleepDuration = 0;									//4.0.23.508 TD 89047
				}
				else if(iRetries > 0)												
					dwThreadSleepDuration = TIMEOUT_BETWEEN_RETRIES;			//4.0.23.508 TD 89047
			}
			else
			{
				//// Start One cycle thread process
				//------------------------------------------------------
				// Command Tx main process:
				// Read single command from queue
				// activate command depending on the operating mode 
				// If opafaild to operate command - retry MAX_GCIEVENTS_RETRIES
				//
				//Only if in IDLE running all the internal processes.
				//If not running only the watchdog process.

				if(_Module.m_server.GetServerState() == SERVER_IDLE)
				{
					// background checking for automatic operations.
					pInfo->m_pServer->DoAutomaticMaintenanceLock__();
					pInfo->m_pServer->DoAutomaticMaintenanceUnlock__();
 									
					pInfo->m_pServer->DoAutomaticEmulationDelivery__();				//Udi 4.0.5.24

					pInfo->m_pServer->DoAutomaticEmulationDeliverySingleTank__(); //4.0.21.500 TD 66935
				
					pInfo->m_pServer->DoAutomaticPrePay__();
				
					pInfo->m_pServer->DoAutomaticOLA__();

					pInfo->m_pServer->DoAutomaticInitLoyaltyFuel__();//4.0.9.508

					pInfo->m_pServer->DoAutomaticTankLock__(); //4.0.21.500 TD 66935 

					pInfo->m_pServer->DoAutomaticRequestReport__(); //4.0.22.501 TD 69905

					pInfo->m_pServer->DoAutomaticChangeProductInTank__(); //4.0.22.503 - TD 72189
					
					pInfo->m_pServer->DoAutomaticPriceChangeWithAckPerPump__(); //4.0.19.820 - TD 78775

					//pInfo->m_pServer->DoAutomaticQdxMaintenance__();				//4.0.26.501 TD 144220

					//CServerMain has a member, m_lNewOLAServer, which
					//holds the type of an OLA server we wish to
					//switch to.  If we do not wish to make a switch,
					//this value is NO_NEW_SERVER (zero).
					long lNewOLASrv = pInfo->m_pServer->GetNewOLAServer();		//Eyal 4.0.0.25 Start
				
					if(lNewOLASrv != NO_NEW_SERVER)
						pInfo->m_pServer->SwitchServer__(lNewOLASrv);
					
					//================================
					// Shift and Price change checking
					//================================
					lPriceAndShiftCheckTime += THRD_SLEEP_TIME;
					if (lPriceAndShiftCheckTime >= TIME_BETWEEN_PRICE_AND_SHIFT_CHECKS)
					{
						lPriceAndShiftCheckTime = 0;

						if(!pInfo->m_pServer->m_cServerInfo.GetRequiredPriceChangeFlags())
						{							
							//Call CheckShiftChange only if RcvSetShiftCommand is not on, if it is - we will call CheckShiftChange later. //4.0.13.501
							if (pInfo->m_pServer->GetRcvSetShiftCommand() == NOT_RCV_SET_SHIFT)
								pInfo->m_pServer->CheckShiftChange__();				
						}
						if(!pInfo->m_pServer->m_cServerInfo.GetRequiredShiftProcessFlags())
							pInfo->m_pServer->CheckPriceChange__();			

						// if in middle of shiftChange or PriceChange
						// do more frequent checkes.//4.0.8.501
						// mode the price change process down, separate it from Shift change.
						if (pInfo->m_pServer->m_cServerInfo.GetRequiredShiftProcessFlags())
						{
							lPriceAndShiftCheckTime = TIME_BETWEEN_PRICE_AND_SHIFT_CHECKS;					
						}
					}

					lPriceProtectionCheckTime += THRD_SLEEP_TIME;        //4.0.23.2561

					if (lPriceProtectionCheckTime >= TIME_BETWEEN_PRICE_PROTECTION_CHECKS)
					{
						lPriceProtectionCheckTime = 0;
						pInfo->m_pServer->DoAutomaticPriceProtectionCheck__(); // CR 438993
					}
					 
					lPriceChangeCheckTime += THRD_SLEEP_TIME;								//4.0.8.501
					if (lPriceChangeCheckTime >= TIME_BETWEEN_PRICE_CHANGE_CHECKS)
					{
						lPriceChangeCheckTime=0; 
						//4.0.8.501
						if(!pInfo->m_pServer->m_cServerInfo.GetRequiredShiftProcessFlags())
						{
							pInfo->m_pServer->CheckPriceChange__();			
							pInfo->m_pServer->DoAutomaticCheckPriceChangeAck(); //4.0.9.555
						}
					}
			
					if (pInfo->m_pServer->GetRcvSetShiftCommand() && !(pInfo->m_pServer->m_cServerInfo.GetWetStockFlags()))		//4.0.13.501
					{
						if (pInfo->m_pServer->GetRcvSetShiftCommand() == RCV_WITH_FORCE_NON_LOCK)
							pInfo->m_pServer->CheckShiftChange(TRUE);
						else //RCV_WITH_NO_FORCE_NON_LOCK
							pInfo->m_pServer->CheckShiftChange(FALSE);

						//reset the flag
						pInfo->m_pServer->SetRcvSetShiftCommand(NOT_RCV_SET_SHIFT);
					}

					//================================
					// shift change proces will work only 
					// if m_cInfo.bRequiredShiftChange
					//================================
					if(!pInfo->m_pServer->ShiftChangeProcess__())
 					{
						//================================
						// price change process will work only  
						// if m_cInfo.bRequiredPriceChange 
						//================================
						if(!pInfo->m_pServer->PriceChangeProcess__())
						{
							//================================
							// Mode scheme processs will work only  
							// if m_cInfo.bRequiredModeProcess 
							//================================
							pInfo->m_pServer->ModeSchemeProcess__();
						}				
					}

					//=================================
					// Mode scheme time check 
					//=================================
					lModeSchemeCheckTime += THRD_SLEEP_TIME;
					if ((lModeSchemeCheckTime >= TIME_BETWEEN_PRICE_AND_SHIFT_CHECKS) || (bFirstTimeModesProcess == TRUE) ) //4.0.15.170
					{
						lModeSchemeCheckTime = 0; 
						pInfo->m_pServer->SetReqiredModeProcess__();
						if (bFirstTimeModesProcess == TRUE)
						{
							if(LOG_BASIC_FLOW_CONTROL)
								_LOGMSG.LogMsg("CmdTx:Set Mode Process Immediately when Server IDLE.");
						}
						bFirstTimeModesProcess = FALSE;
					}
			
					lDeliveryAndTankCheckTime += THRD_SLEEP_TIME;
					if (lDeliveryAndTankCheckTime >= TIME_BETWEEN_DELIVERY_REPORT_CHECKS)
					{
						//Check if wet stock by date, by interval or none
						BOOL bGetWetStockData = FALSE;
						if ( pInfo->m_pServer->GetWetStockMode() == WSM_BY_INTERVAL)
						{
							lDeliveryAndTankCheckTime += THRD_SLEEP_TIME;

							lWetStockInterval = pInfo->m_pServer->GetWetStockInterval();

							if (lWetStockInterval > 0)
							{
								if (pInfo->m_pServer->IsTimeToProcessByTime(lWetStockInterval)) //4.0.17.501 merge from 4.0.17.91 4.0.19.500
								{
									pInfo->m_pServer->GetWetStockData__(WSM_BY_INTERVAL);					
								
									//Reset CheckMemoWetstock (GetWetStckData will not be called again in the same time because of the memo interval) 
									lCheckMemoWetstock =  0;
								}								
							}
						}
						else if ( pInfo->m_pServer->GetWetStockMode() == WSM_BY_TIME)
						{
							pInfo->m_pServer->GetWetStockData__(WSM_BY_TIME);
						}
						//Reset
						lDeliveryAndTankCheckTime = 0;
					}
					//Add by Yaron
					
					//checking if this interval is valid.
					long lMemoInterval = pInfo->m_pServer->GetMemoInterval();
					if ( lMemoInterval > 0 )
					{				
						if (pInfo->m_pServer->IsTimeToProcessByMemo(lMemoInterval)) //4.0.15.190 4.0.19.500
						{
							pInfo->m_pServer->GetWetStockData__(WSM_BY_MEMO);
							lCheckMemoWetstock =  0;
							
							pInfo->m_pServer->IncreaseWetstockMemoCounter();			//4.0.15.170
						}
					} 
					
					//=================================
					// WetStock Clear time check 
					//=================================

					//checking if the flag is set
					if(pInfo->m_pServer->m_cServerInfo.GetWetStockFlags())  //4.0.8.500
					{
						long lWetStockClearTime = pInfo->m_pServer->GetWetStockClearTime();
						lCheckWetstockClearTime += THRD_SLEEP_TIME;
						if (lCheckWetstockClearTime >= lWetStockClearTime)
						{
							pInfo->m_pServer->SendWetStockEvents(pInfo->m_pServer->m_cServerInfo.GetMemoProcessFlags() ,pInfo->m_pServer->m_cServerInfo.GetWetStockFlags()); //4.0.7.503
							if(LOG_BASIC_FLOW_CONTROL)
								_LOGMSG.LogMsg("CmdTx:Clearing WetStockFlags");
							
							if(LOG_BASIC_FLOW_CONTROL)
							{
								_LOGMSG.LogMsg("CGCI::CmdTx call clear wet stock flags"  );						
							}

							//clear wet stock flag
							pInfo->m_pServer->m_cServerInfo.ClearAllWetStockFlags();

							lCheckWetstockClearTime = 0;
							
							int nErrorRetreived = 0;
							_Module.m_server.SetDeliveryFlags(DES_UNLOCK, PD_NONE, nErrorRetreived, FALSE);						

							bDeliveryTimeOut = TRUE;
							
							if(DES_WAIT_TANK_READING == pInfo->m_pServer->m_cServerInfo.GetDeliveryEmulationStatus())
								pInfo->m_pServer->m_cServerInfo.SetDeliveryEmulationStatus(DES_UNLOCK);							

							//4.0.22.503 - TD 69905
							if (pInfo->m_pServer->m_cServerInfo.GetReportRequestState() == REPORT_WAIT_TANK_READING)
							{								
								pInfo->m_pServer->m_cServerInfo.SetReportRequestState(REPORT_GOT_TANK_READING_TIMEOUT, "CGCI::CmdTx");  // TD 351225
							}
						}	
					}
					else
					{
						//if in the middle of checking WetStock was finished
						if( lCheckWetstockClearTime )
							lCheckWetstockClearTime = 0;
					}

					//end add by Yaron

					lCheckDispensingTime += THRD_SLEEP_TIME;
					if (lCheckDispensingTime >= TIME_BETWEEN_DISPENSING_TIMEOUT_CHECKS)
					{
						lCheckDispensingTime = 0;
						pInfo->m_pServer->CheckDispensingTimeOut__();
					}

					//4.0.9.506 //amram AutoReconcile
					lCheckAutoReconcile += THRD_SLEEP_TIME;
					if (lCheckAutoReconcile >= TIME_BETWEEN_UNPAID_TIMEOUT_CHECKS)
					{
						lCheckAutoReconcile = 0;
					}

					lCheckUnpaidTime += THRD_SLEEP_TIME;
					if (lCheckUnpaidTime >= TIME_BETWEEN_UNPAID_TIMEOUT_CHECKS)
					{
						lCheckUnpaidTime = 0;
						pInfo->m_pServer->CheckUnpaidTimeOut__();
					}

					// M.L 1.7.98 
					lCheckAuthorizedTime += THRD_SLEEP_TIME;
					if (lCheckAuthorizedTime >= TIME_BETWEEN_AUTHORIZED_TIMEOUT_CHECKS)
					{
						lCheckAuthorizedTime = 0;
						pInfo->m_pServer->CheckAuthorizedTimeOut__();
					}

					if( _Module.m_server.GetAutoReconcileTimeout() )
					{
						lCheckAuthReconcileTime += THRD_SLEEP_TIME;
						if (lCheckAuthReconcileTime >= TIME_BETWEEN_AUTO_RECONCILE_CHECKS)
						{
							lCheckAuthReconcileTime = 0;
							pInfo->m_pServer->DoAutomaticTrsAutoReconcile__();
						} 

					} // TD 233242 4.0.23.1050
					
	#ifdef USE_XML_PRICE_CHANGE    //4.0.0.86
				lCheckNewIncomingPrice  += THRD_SLEEP_TIME;

				if (lCheckNewIncomingPrice >= TIME_BETWEEN_INCOMING_NEW_PRICE_CHECKS)
				{
					lCheckNewIncomingPrice = 0 ;
					pInfo->m_pServer->CheckForIncomingNewPrice();				
				}
	#endif

				//4.0.17.20 - TD 26384
				if (_Module.m_server.GetCheckAndReserveTimeout() > 0)
				{
					lCheckReserveTime += THRD_SLEEP_TIME;
					if (lCheckReserveTime >= _Module.m_server.GetCheckAndReserveTimeout())
					{
						lCheckReserveTime = 0;
						pInfo->m_pServer->CheckAndReserveTimeOut__();
					}
				}				

				//4.0.1.38
				lCheckNozzleLeftOut += THRD_SLEEP_TIME;

				//4.0.2.161
				if(bNozzleLeftOutMiscRaised)
					lNozzleLeftOutCheckInterval = TIME_BETWEEN_NOZZLE_OUT_CHECKS / 3;

				else
					lNozzleLeftOutCheckInterval = TIME_BETWEEN_NOZZLE_OUT_CHECKS;

				if(lCheckNozzleLeftOut > lNozzleLeftOutCheckInterval)
				{
					lCheckNozzleLeftOut = 0;
					bNozzleLeftOutMiscRaised = pInfo->m_pServer->CheckNozzleLeftOutTimeout();
				}

					//=================================
					// Receipt Act
					//=================================			
					//Check car wash records, and act
					lCheckReceiptTime += THRD_SLEEP_TIME;
					if (lCheckReceiptTime >= TIME_BETWEEN_RECEIPT_CLEAR_CHECKS)
					{				

						lCheckReceiptTime = 0;
						_Module.m_server.SetInTheMiddleOfDoAutomaticReceiptCheck(TRUE);
						pInfo->m_pServer->DoAutomaticReceiptCheck();
						_Module.m_server.SetInTheMiddleOfDoAutomaticReceiptCheck(FALSE);
									}

					//=================================
					// Car Wash
					//=================================
					
					//Check car wash records, and act
					lCheckCarWashTime += THRD_SLEEP_TIME;
					if (lCheckCarWashTime >= TIME_BETWEEN_CAR_WASH_CHECKS)
					{				
						lCheckCarWashTime = 0;
						pInfo->m_pServer->DoAutomaticCheckCarWash__();
					}

					/*****************************************
					Car wash General Requests 76432 4.0.23.504
					*****************************************/

					lCheckCarWashGeneralRequestTime += THRD_SLEEP_TIME;
					if (lCheckCarWashGeneralRequestTime >= TIME_BETWEEN_CAR_WASH_CHECKS)
					{				
						lCheckCarWashGeneralRequestTime = 0;
						pInfo->m_pServer->DoAutomaticCarWashGeneralReq__();
					}

					//Check if there are car wash record to remove
					lCheckCarWashClearTime += THRD_SLEEP_TIME;
					if (lCheckCarWashClearTime >= TIME_BETWEEN_CAR_WASH_CLEAR_CHECKS)
					{				
						lCheckCarWashClearTime = 0;
						pInfo->m_pServer->DoAutomaticlCarWashClear__();
					}

					// check interval ocking pump at debit shift 	 
					//=================================
					// Debit shift
					//=================================

					lCheckDebitShiftInProcessTime += THRD_SLEEP_TIME;

					//=================================
					// RealTime Clock
					//=================================
					if(_Module.m_server.IsAutoTimeChecker())
					{
						lCheckRealTimeClockTime += THRD_SLEEP_TIME;
						if (lCheckRealTimeClockTime >= TIME_BETWEEN_REALTIME_CLOCK_CHECKS)
						{				
							lCheckRealTimeClockTime = 0;
							pInfo->m_pServer->DoAutomaticRealTime();
						}
					}
					//=================================
					// 4.0.3.49 Lock Pump by Mode
					//=================================
					if(_Module.m_server.IsLockPumpByModeActive())
					{
						lCheckLockPumpByModeTime += THRD_SLEEP_TIME;
						if (lCheckLockPumpByModeTime >= TIME_BETWEEN_LOCK_PUMP_BY_MODE_CHECKS)
						{				
							lCheckLockPumpByModeTime = 0;
							pInfo->m_pServer->DoAutomaticLockPumpByMode();
						}
					}

					if(_Module.m_server.IsExportSiteConfiguration())					//4.0.26.508 TD 146575
					{
						lExportSiteConfigurationTime += THRD_SLEEP_TIME;
						if (lExportSiteConfigurationTime >= TIME_BETWEEN_EXPORT_SITE_CONFIG_CHECKS)
						{				
							lExportSiteConfigurationTime = 0;
							pInfo->m_pServer->DoAutomaticExportSiteConfiguration__();		
						}
					}

					//4.0.12.501
					_Module.m_server.UpdatePAPDeclineTimers(THRD_SLEEP_TIME);
					
					//4.0.5550.1504
					pInfo->m_pServer->DataBaseMaintenanceTimeout(THRD_SLEEP_TIME);

					// set the Qdx Emptied alarm
					if(_Module.m_server.IsQdxEmptied())
					{
						_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, ALARM_EMPTY_QDX, 0, ALARM_STARTED, 0, 0); 
						_Module.m_server.SetQdxEmptied(FALSE);
					}// TD 236009 4.0.27.500 - TD 236009

				}//End If SERVER_IDLE

				if (_Module.m_server.GetServerState() != SERVER_IDLE)
				{
					bFirstTimeModesProcess = TRUE; //4.0.15.170
				}
				//watch dog we are doing all the time
				//=================================
				// Watchdog
				//=================================

				
				long lInterval = pInfo->m_pServer->GetInterval(pInfo->m_pServer->m_tConnectionTimeOut);
				if (lInterval > (long)_Module.m_server.m_dwWatchdogInterval)
				{
					pInfo->m_pServer->DoAutomaticWatchdog();
				}


// 				lCheckWatchdogTime += THRD_SLEEP_TIME;
// 
// 				if (lCheckWatchdogTime >= _Module.m_server.m_dwWatchdogInterval)
// 				{				
// 					lCheckWatchdogTime = 0;
// 					pInfo->m_pServer->DoAutomaticWatchdog();
// 				}

				dwThreadSleepDuration = THRD_SLEEP_TIME;			//4.0.23.509 TD 95864
			}

			Sleep(dwThreadSleepDuration);							//4.0.23.508 TD 89047
			//// End of One cycle thread process
		}
		//@@@
		if(bDeliveryTimeOut)
		{	
			//Should we call also function of single tank - 66935 (Udi's TimeOut)*?*?
			int nErrorRetreived = 0;				
			_Module.m_server.SetDeliveryFlags(DES_UNLOCK, PD_NONE, nErrorRetreived, FALSE);

		}
		//@@@		
		if(bKillThread) // exit loop due to kill command
		{
			break;	
		}
	}

	SetEvent(pInfo->m_hEventThreadKilled);
}  
	__except(_HANDLE_EXCEPTION("GCI::CmdTx")) //4.0.15.460
	{
		// we arrive here on error only !
		if (LOG_ERROR)
		{
			DWORD dwExceptionCode = GetExceptionCode();
			char sMsg[MAX_MSG_LEN] = { 0 };
			sprintf_s(sMsg, MAX_MSG_LEN,"CGCI::CmdTx(), catch exception. ExceptionCode = 0x%x", dwExceptionCode);
			_LOGMSG.LogMsg(sMsg, LOG_LEVEL_0);
		}

		// TD 453785

		EXCEPTION_POINTERS* ppExceptionPointers = NULL;
		GetExceptionPointers(0, &ppExceptionPointers);

		// Translate SEH exception to a C++ exception.  NOTE: compiling with /EHa is required!!
		static char msg[256];
		sprintf_s(msg, 256, "Unhandled exception 0x%08x at 0x%08x",
			ppExceptionPointers->ExceptionRecord->ExceptionCode,
			(unsigned int) ppExceptionPointers->ExceptionRecord->ExceptionAddress);

		CExceptionHandler::ExceptionHandler().HandleException(ppExceptionPointers, msg);

		exit(1);
	}

	CoUninitialize();

	return 0;	
}




/************************************************************************
 *WHO		WHEN		WHAT
 *------	-------- - -------------------------------------------------------- -
 *Maxim	4 / 18 / 2016	TD 454500 - Suncor - Using Reprint function on POSPump causes Fuel to crash.
 ************************************************************************/
long CGCI::QueueHandle(CThreadInfo* pInfo)
{
	CCmdInfo	Cmd;				
	long		lRetCode = 0;
	long		lEventRetCode = GCI_EVENT_OK;
	HRESULT		hRes = S_OK;					
	IGCIEvents* pEvent = NULL;
	BOOL		bInternal=FALSE;
	CString		strCmdName;
	BOOL		bRet;
	long		lQueueHandleRetCode = OK;				//4.0.23.507 TD 89047
	static int  iCmdToOpRetriesCount = 0;				//4.0.23.507 TD 89047
	const int	iMaxCmdToOpRetries = 5;					//4.0.23.507 TD 89047

	pEvent = pInfo->m_pGCI->GetEventPtr();					
	
	if(pEvent)
	{
		Cmd	= pInfo->m_pServer->GetNextCmdInQ();

		//4.0.13.150
		if(LOG_DETAIL_FLOW_CONTROL && (Cmd.GetCommand() != CMD_WATCHDOG))
		{
			CString str;
			str.Format("Queue handle, Command=%ld, Src=%ld, Parameter=%ld",Cmd.GetCommand(),Cmd.GetSrc(),Cmd.GetParameter());
			pInfo->m_pServer->m_cLogger.LogMsg(str);
		}

		switch (Cmd.GetCommand())
		{
			case CMD_SET_REALTIME_CLOCK:
			{
				strCmdName = "SetRealTimeClock";
				CString cTmp = " ";
				BSTR sDateTime = cTmp.AllocSysString();
				hRes = pEvent->SetRealTimeClock(0, sDateTime, &lEventRetCode);

				//4.0.2.15
				SysFreeString(sDateTime);
			}
			break;
			
			case CMD_GET_NEW_WASH_CODE:
			{
				strCmdName = "GetNewWashCode";
				CMD_STRUCT_CAR_WASH *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_CAR_WASH *)Cmd.GetDataPtr();	
				hRes = pEvent->GetNewWashCode(pCmdInfo->lSyncNumber,
											  Cmd.GetParameter(),
											  Cmd.GetSrc(),
											  pCmdInfo->lWashProgram,
											  pCmdInfo->lFlags,
											  &lEventRetCode);
			}
			break;

			case CMD_CAR_WASH_VALIDITY: //4.0.26.501 144221
			{
				strCmdName = "GetWashCodeStatus";
				CMD_STRUCT_CAR_WASH *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_CAR_WASH *)Cmd.GetDataPtr();	
				BYTE sTmp[sizeof(pCmdInfo->carWashData)+2];
				Str2StrNull((BYTE *)&pCmdInfo->carWashData,sizeof(pCmdInfo->carWashData),sTmp);
				CComBSTR sCarWashData(sizeof(sTmp),(LPCSTR)sTmp); 
				hRes = pEvent->GetWashCodeStatus(pCmdInfo->lSyncNumber,
											  Cmd.GetParameter(),
											  Cmd.GetSrc(),
											  pCmdInfo->lFlags,
											  sCarWashData,
											  &lEventRetCode);
			}
			break;

			case CMD_GET_CAR_WASH_STATUS:
			{
				strCmdName = "GetCarWashStatus";
				CMD_STRUCT_CAR_WASH *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_CAR_WASH *)Cmd.GetDataPtr();	
				hRes = pEvent->GetCarWashStatus(Cmd.GetParameter(),
												Cmd.GetSrc(),
												pCmdInfo->lFlags,
												&lEventRetCode);
			}
			break;

			case CMD_CANCEL_WASH_CODE:
			{
				strCmdName = "CancelWashCode";
				CMD_STRUCT_CAR_WASH *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_CAR_WASH *)Cmd.GetDataPtr();	

				BYTE sTmp[sizeof(pCmdInfo->carWashData)+2];
				Str2StrNull((BYTE *)&pCmdInfo->carWashData,sizeof(pCmdInfo->carWashData),sTmp);
				CComBSTR sCarWashData(sizeof(sTmp),(LPCSTR)sTmp); 

				hRes = pEvent->CancelWashCode(pCmdInfo->lSyncNumber,
											  Cmd.GetParameter(),
											  Cmd.GetSrc(),
											  pCmdInfo->lFlags,
											  sCarWashData,
											  &lEventRetCode);
			}
			break;
			case CMD_CAR_WASH_GENERAL_REQUEST:	//4.0.23.504 76432
			{
				strCmdName = "CarWashGeneralRequest";
				CMD_STRUCT_CAR_WASH *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_CAR_WASH *)Cmd.GetDataPtr();	

				hRes = pEvent->CarWashGeneralReq(Cmd.GetParameter(),
											  pCmdInfo->lFlags,
											  &lEventRetCode);
				break;
			}
			case CMD_WATCHDOG:	// PumpSrv generated (Master & Slave -> IGCIEvents)
			//-------------------------------------------------------------------------
			{
				strCmdName = "Watchdog";			
				long lTmp = 0;

				hRes = pEvent->Watchdog(Cmd.GetSrc(),
										Cmd.GetParameter(),
										&lTmp,
										&lEventRetCode);
				
			}
			break;

			case CMD_AUTHORIZE_LIMIT:	// POS generated (Master & Slave -> IGCIEvents)
			//------------------------------------------------------------------------
			{	//Need to do- current status checking
	
				ExecuateAuthorizeAndLimit(strCmdName, Cmd, pInfo, hRes, pEvent, lEventRetCode);
			}
			break;
			
			// 4.0.5.39
			case CMD_DISPLAY_MSG:	
				//------------------------------------------------------------------------
				{	
					strCmdName = "DisplayMsg";
					CMD_STRUCT_DISPLAY_MSG *pCmdInfo;
					pCmdInfo = (CMD_STRUCT_DISPLAY_MSG *)Cmd.GetDataPtr();	
					
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("DisplayMsg: lTimeOut %ld, sMsgDisplay %s, MsgDisplayLength  %d",
							pCmdInfo->lTimeOut, pCmdInfo->sDisplayMsg, pCmdInfo->lDisplayMsgLen );									
						pInfo->m_pServer->m_cLogger.LogMsg(Cmd.GetParameter(),LOG_PUMP,str);							
					}
					//Building a BSTR in order to send the Display Msg in it
					CComBSTR strData(MAX_TERMINAL_MSG_LEN + 2,(LPCSTR) pCmdInfo->sDisplayMsg); 
					
					hRes = pEvent->DisplayMsg(	Cmd.GetParameter(),								
						pCmdInfo->lTimeOut,
						strData,
						pCmdInfo->lDisplayMsgLen,
						&lEventRetCode);				
				}
				break;
				
			case CMD_STOP_PUMP:	// POS generated (Master & Slave -> IGCIEvents)
				//-----------------------------------------------------------------
				{	//Need to do current status checking
					strCmdName = "StopPump";							
					hRes = pEvent->StopPump(Cmd.GetParameter(),
						Cmd.GetSrc(),
						&lEventRetCode);
				
			}
			break;

			case CMD_RESTART_PUMP:	// POS generated (Master & Slave -> IGCIEvents)
			//---------------------------------------------------------------------
			{	//Need to do current status checking 
				strCmdName = "ResumePump";
				hRes = pEvent->ResumePump(	Cmd.GetParameter(),
											Cmd.GetSrc(),
											&lEventRetCode);
			}
			break;				
				
			case CMD_RESET_CONTROLLER:	// Server generated (Master & Slave -> IGCIEvents)
			//----------------------------------------------------------------------------
			{
				strCmdName = "Reset";

				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("Attempt to reset device=%ld  !!!!! ", Cmd.GetSrc());							    
					pInfo->m_pServer->m_cLogger.LogMsg(str);							
				}

				hRes = pEvent->Reset(Cmd.GetSrc(),&lEventRetCode);
			}
			break;

			case CMD_GET_PUMP_STATUS:	// Server Generated (Master & Slave -> IGCIEvents)
			//----------------------------------------------------------------------------
			{
				strCmdName = "GetPumpStatus";							
				hRes = pEvent->GetPumpStatus(	Cmd.GetParameter(),																	
													Cmd.GetSrc(),
													&lEventRetCode);
			}
			break;

			case CMD_RESERVE_PUMP:	//  Master & Slave (for pre pay) -> IGCIEvents
			//--------------------------------------------------------------------
			{ //Need to do current status/reservation checking -
				strCmdName = "ReservePump";
				CMD_STRUCT_RESERVE_PUMP *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_RESERVE_PUMP *)Cmd.GetDataPtr();

				long lSrc = Cmd.GetSrc(); //3.0.0.4

				if ((pCmdInfo->lRes == PUMPSRV) || (pCmdInfo->lRes == PAY_AT_KIOSK) ||
					(pCmdInfo->lRes == PRE_PAY_AT_POS) && (pCmdInfo->lResExt == RES_EXT_CASH_ACCEPTOR) || //4.0.10.49 //4.0.10.53
					((pCmdInfo->lRes == PAY_AT_PUMP) && (pCmdInfo->lResExt == RES_EXT_PAP_EXTERNAL || pCmdInfo->lResExt == RES_EXT_MOBILE) && (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse()))) //4.0.19.504 - 47559
				{
					lSrc = pCmdInfo->lResExt;
				}

				//4.0.7.504
				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;										
					str.Format("Request reserved, Res=%d, ResExt=%d", pCmdInfo->lRes,pCmdInfo->lResExt);									
					pInfo->m_pServer->m_cLogger.LogMsg(Cmd.GetParameter(),LOG_PUMP,str);							
				}

								
				hRes = pEvent->ReservePump(	Cmd.GetParameter(),
												// pCmdInfo->lResExt, // the requesting POS # - src
												lSrc , // 3.0.0.4Cmd.GetSrc(), // the requesting POS # - src
												pCmdInfo->lRes,    
												&lEventRetCode);								


				if (hRes || lEventRetCode)
				{
					//4.0.10.504
					if (pCmdInfo->lRes == PRE_PAY_AT_POS)
					{
						CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

						CPumpTransact cTrs;
						if (pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter() - 1].GetNextWaitingTrs(&cTrs))
						{
							cTrs.SetStatus(WAIT_TIME_OUT);
							cTrs.UpdateTime();			
							cTrs.m_lMisc |= MISC_PREPAY_TIMED_OUT;														
							pInfo->m_pServer->CentralUpdateTrs(Cmd.GetParameter(),cTrs.m_lNumber,&cTrs,FALSE);				

							if(LOG_DETAIL_FLOW_CONTROL)
							{
								CString str;										
								str.Format("Attempt reserved Prepay trs has failed, move trs =%ld to timeout", cTrs.m_lNumber);									
								pInfo->m_pServer->m_cLogger.LogMsg(Cmd.GetParameter(),LOG_PUMP,str);							
							}							
						}

						if (slPumpArray.IsLocked())
							slPumpArray.Unlock();							
					}
				}
			}
			break;

			case CMD_SET_PUMP_LIMITS:	//  Master & Slave (for pre pay) -> IGCIEvents
			//--------------------------------------------------------
			{	//Need to do check first if the pump is reserved 
				//by for the asking pos

				strCmdName = "SetPumpLimit";
				CMD_STRUCT_LIMIT	*pCmdInfo;
				pCmdInfo = (CMD_STRUCT_LIMIT *)Cmd.GetDataPtr();
				hRes = pEvent->SetPumpLimit(Cmd.GetParameter(),
											Cmd.GetSrc(),
											pCmdInfo->nLevel,
											pCmdInfo->lMaxValue,
											pCmdInfo->lMaxVolume,
											pCmdInfo->nFlags,
											&lEventRetCode);

			}
			break;							

			case CMD_RELEASE_PUMP:		//  Master & Slave (for pre pay) -> IGCIEvents
			//--------------------------------------------------------
			{	//Need to do check first if the pump is reserved for the asking pos
				strCmdName = "ReleasePump";						

				CMD_STRUCT_RELEASE_PUMP *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_RELEASE_PUMP *)Cmd.GetDataPtr();

				long lSrc = Cmd.GetSrc();  //3.0.0.4

				if (pCmdInfo->lRes == PUMPSRV)
				{
					lSrc = pCmdInfo->lResExt;
				}

				if ((pCmdInfo->lRes == PAY_AT_PUMP) && (pCmdInfo->lResExt == RES_EXT_MOBILE))
				{
					lSrc = pCmdInfo->lResExt;
				}


				hRes = pEvent->ReleasePump(	Cmd.GetParameter(),
											// Cmd.GetSrc(),
											lSrc , //3.0.0.4pCmdInfo->lResExt,							
											pCmdInfo->lRes,
											&lEventRetCode);								
			}
			break;


			case CMD_SET_PUMP_MODE:		// Slave -> IGCIEvents
			//--------------------------------------------------------
			{
				strCmdName = "SetPumpMode";
				CMD_STRUCT_MODE *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_MODE *)Cmd.GetDataPtr();
				

				if(THRD_PUMPSRV_IS_MODE_MASTER)
				{
					bInternal = TRUE;
					CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

					CPumpStatus stat = pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].GetStat();
					stat.SetMode(pCmdInfo->lMode);
					pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].SetStat(stat);
					pInfo->m_pServer->SavePumpInfo(Cmd.GetParameter());

					if (slPumpArray.IsLocked())
						slPumpArray.Unlock();

					lRetCode = GPI_OK;
				}
				else 
				{
					hRes = pEvent->SetPumpMode(	Cmd.GetParameter(),
												Cmd.GetSrc(),																			
												pCmdInfo->lMode,
												&lEventRetCode);								
				}

			}
			break;

			case CMD_TRS_OFFER:			// Slave -> IGCIEvents , Master -> Internaly
			//--------------------------------------------------------
			{
				strCmdName = "OfferTrs";

				CMD_STRUCT_TRS *pCmdTrs;
				pCmdTrs = (CMD_STRUCT_TRS *)Cmd.GetDataPtr();

				CPumpTransact trs;

				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------
				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);
				
				if(pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].GetTrsByNumber(pCmdTrs->lTrsNumber,&trs) == 0)
				{
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("Internal CMD_TRS_OFFER : (before setting new state), Trs %d, Status %d, OnOffer %d, link Number %d", 
							trs.m_lNumber,
							trs.m_lStatus,
							trs.m_lOnOffer,											
							trs.m_lLinkNumber );
						pInfo->m_pServer->m_cLogger.LogMsg( Cmd.GetParameter(),LOG_PUMP,str ); 
					}	

					if(THRD_PUMPSRV_IS_SLAVE)
					{						
						if((trs.GetStatus() == UNPAID)||(trs.GetStatus() == ACCEPTED))
							hRes = pEvent->SetTrsState(	Cmd.GetParameter(),
													Cmd.GetSrc(),
													pCmdTrs->lTrsNumber,
													STATE_ON_OFFER,
													&lEventRetCode);								

						else
							lRetCode = GPI_TRS_NOT_FOR_SALE;
					}
					else // Server is Master 
					{
						bInternal = TRUE;
						lRetCode = trs.SetStateOnOffer(Cmd.GetSrc());
					}
				}
				else
				{
					lRetCode = GPI_TRS_NOT_FOUND;									
				}
				
				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------	
			}
			break;

			case CMD_TRS_RELEASE:		// Slave -> IGCIEvents , Master -> Internally
			//--------------------------------------------------------
			{
				strCmdName = "ReleaseTrs";
				CMD_STRUCT_TRS *pCmdTrs;
				pCmdTrs = (CMD_STRUCT_TRS *)Cmd.GetDataPtr();

				CPumpTransact trs;

				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------
				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);
				
				if(pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].GetTrsByNumber(pCmdTrs->lTrsNumber,&trs) == 0)
				{
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("Internal CMD_TRS_RELEASE : (before setting new state), Trs %d, Status %d, OnOffer %d, link Number %d", 
							trs.m_lNumber,
							trs.m_lStatus,
							trs.m_lOnOffer,
							trs.m_lLinkNumber );
						pInfo->m_pServer->m_cLogger.LogMsg( Cmd.GetParameter(),LOG_PUMP,str ); 
					}	

					if((trs.GetStatus() == UNPAID)||(trs.GetStatus() == ACCEPTED))
					{										
						if(THRD_PUMPSRV_IS_SLAVE)
						{						
							hRes = pEvent->SetTrsState(	Cmd.GetParameter(),
														Cmd.GetSrc(),
														pCmdTrs->lTrsNumber,
														STATE_NOT_ON_OFFER,
														&lEventRetCode);								
						}
						else // Server is Master 
						{
							bInternal = TRUE;
							lRetCode = trs.SetStateRelease(Cmd.GetSrc());
						}
					}									
					else 
					{
						lRetCode = GPI_TRS_NOT_FOR_SALE; // NULL_TRS, IN_PROGRESS, PAID, INCOMPLETE
					}
				}
				else
				{
					lRetCode = GPI_TRS_NOT_FOUND;												
				}
			}
			break;

			case CMD_TRS_FORCE_RELEASE:	// Slave -> IGCIEvents , Master -> Internaly
			//--------------------------------------------------------
			{
				strCmdName = "ForceReleaseTrs";
				CMD_STRUCT_TRS *pCmdTrs;
				pCmdTrs = (CMD_STRUCT_TRS *)Cmd.GetDataPtr();

				CPumpTransact trs;

				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------
				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);
				
				if(pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].GetTrsByNumber(pCmdTrs->lTrsNumber,&trs) == 0)
				{	
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("Internal CMD_TRS_FORCE_RELEASE : (before setting new state), Trs %d, Status %d, OnOffer %d, link Number %d", 
							trs.m_lNumber,
							trs.m_lStatus,
							trs.m_lOnOffer,
							trs.m_lLinkNumber );
						pInfo->m_pServer->m_cLogger.LogMsg( Cmd.GetParameter(),LOG_PUMP,str ); 
					}	

					if((trs.GetStatus() == UNPAID)||(trs.GetStatus() == ACCEPTED))
					{
						if(THRD_PUMPSRV_IS_SLAVE)
						{						
							hRes = pEvent->SetTrsState(	Cmd.GetParameter(),
														trs.GetOnOffer(), //Cmd.GetSrc()
														pCmdTrs->lTrsNumber,
														STATE_NOT_ON_OFFER,//STATE_FORCE_RELEASE,
														&lEventRetCode);								
						}
						else // Server is Master 
						{							
							bInternal = TRUE;
							lRetCode = trs.SetStateForcedRelease(Cmd.GetSrc());
						}
					}								
					else 
						lRetCode = GPI_TRS_NOT_FOR_SALE; // NULL_TRS, IN_PROGRESS, PAID, INCOMPLETE
				}
				else
				{
					lRetCode = GPI_TRS_NOT_FOUND;											
				}

				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------	
			}
			break;

			case CMD_TRS_ACCEPTED:		// Slave -> IGCIEvents , Master -> Internaly
			//--------------------------------------------------------
			{
				strCmdName = "TrsAccepted";
				CMD_STRUCT_TRS *pCmdTrs;
				pCmdTrs = (CMD_STRUCT_TRS *)Cmd.GetDataPtr();

				CPumpTransact trs;

				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------
				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

				if(pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].GetTrsByNumber(pCmdTrs->lTrsNumber,&trs) == 0)
				{
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("Internal CMD_TRS_ACCEPTED : (before setting new state), Trs %d, Status %d, OnOffer %d, link Number %d", 
							trs.m_lNumber,
							trs.m_lStatus,
							trs.m_lOnOffer,
							trs.m_lLinkNumber );
						pInfo->m_pServer->m_cLogger.LogMsg( Cmd.GetParameter(),LOG_PUMP,str ); 
					}	

					if(THRD_PUMPSRV_IS_SLAVE)
					{						
						if((trs.GetStatus() == UNPAID)||(trs.GetStatus() == ACCEPTED))
							hRes = pEvent->SetTrsState(	Cmd.GetParameter(),
													Cmd.GetSrc(),
													pCmdTrs->lTrsNumber,
													STATE_ACCEPTED,
													&lEventRetCode);								
						else
							lRetCode = GPI_TRS_NOT_FOR_SALE;
					}
					else // Server is Master 
					{
						bInternal = TRUE;
						lRetCode = trs.SetStateAccepted(Cmd.GetSrc());
					}
				}
				else
				{
					lRetCode = GPI_TRS_NOT_FOUND;									
				}

				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------	
			}
			break;

			case CMD_TRS_PAID:			// Slave -> IGCIEvents , Master -> Internaly
			//--------------------------------------------------------
			{
				strCmdName = "TrsPaid";
				CMD_STRUCT_TRS *pCmdTrs;
				pCmdTrs = (CMD_STRUCT_TRS *)Cmd.GetDataPtr();

				CPumpTransact trs;

				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------
				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

				if(pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].GetTrsByNumber(pCmdTrs->lTrsNumber,&trs) == 0)
				{
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("Internal CMD_TRS_PAID : (before setting new state), Trs %d, Status %d, OnOffer %d, link Number %d", 
							trs.m_lNumber,
							trs.m_lStatus,
							trs.m_lOnOffer,
							trs.m_lLinkNumber );
						pInfo->m_pServer->m_cLogger.LogMsg( Cmd.GetParameter(),LOG_PUMP,str ); 
					}	

					if ((THRD_PUMPSRV_IS_SLAVE) || (PUMPSRV_REPORT_ON_TRS_PAID) || (trs.TrsShouldBeReportedPaid())) //4.0.5.33 //4.0.23.650 133966 //4.0.25.50
					{	
						
						if((trs.GetStatus() == UNPAID)||(trs.GetStatus() == ACCEPTED) || (trs.TrsShouldBeReportedPaid()))//4.0.23.650 133966 //4.0.25.50
						{
							long	lExtraInfoRtc = 0;
							BOOL	bCloseTransactionInternal =FALSE; 

							if ((PUMPSRV_REPORT_ON_TRS_PAID) || (trs.TrsShouldBeReportedPaid()))//4.0.23.650 133966  //4.0.5.34 //4.0.25.50
							{
								EXTRA_INFO_REC	 info;							
								CPumpTransact		cTmptrs;
								cTmptrs.SetLink(TRS_LINK_EXTRA_INFO, trs.m_lNumber);				
								info.info.lIndexNumber = trs.m_lNumber;
								lExtraInfoRtc = _Module.m_server.TrsLinkOperation(REC_READ,&cTmptrs,&info);

								if (!lExtraInfoRtc)
								{
									pCmdTrs->lTrsNumber = info.info.lControllerSeqNum;
									
									if ((info.info.sLAMFlags == '1' ) ||
										(info.info.sLAMFlags == '2' ))
											bCloseTransactionInternal = TRUE;
								}

								
								if(LOG_DETAIL_FLOW_CONTROL)
								{
									CString str;
									str.Format("Send CMD_TRS_PAID with controller SeqNumber : Trs %d, Status %d, OnOffer %d, link Number %d", 
										pCmdTrs->lTrsNumber,
										trs.m_lStatus,
										trs.m_lOnOffer,
										trs.m_lLinkNumber );
									pInfo->m_pServer->m_cLogger.LogMsg( Cmd.GetParameter(),LOG_PUMP,str ); 
								}
							}

							//In situation when we not need to inform controller on close transaction, 
							// Close internally the transaction

							if (!bCloseTransactionInternal)
							{
								if (pCmdTrs->lFlags & TRS_PUMP_TEST)
								{
									hRes = pEvent->SetTrsState3(Cmd.GetParameter(),
															Cmd.GetSrc(),
															pCmdTrs->lTrsNumber,
															STATE_PAID,
															pCmdTrs->lFlags,
															pCmdTrs->sData,
															&lEventRetCode);

									SysFreeString(pCmdTrs->sData);	//4.0.23.503 
									pCmdTrs->sData = NULL;
								}
								else
								{
									hRes = pEvent->SetTrsState(	Cmd.GetParameter(),
															Cmd.GetSrc(),
															pCmdTrs->lTrsNumber,
															STATE_PAID,
															&lEventRetCode);
								}
							}

							//4.0.5.34  //4.0.5.44
							// In case extra info record not exist close transaction internally.
							if (PUMPSRV_REPORT_ON_TRS_PAID && ((!lEventRetCode) || lExtraInfoRtc || bCloseTransactionInternal))
							{
								CPumpTransact		cTmptrs;
								long lExtraInfoRtc =0;
								cTmptrs.SetLink(TRS_LINK_EXTRA_INFO, trs.m_lNumber);
								cTmptrs.m_lNumber = trs.m_lNumber;
								trs.SetStatus(PAID);
								trs.UpdateTime(); //4.0.12.516
								pInfo->m_pServer->CentralUpdateTrs(Cmd.GetParameter(),trs.m_lNumber,&trs,FALSE);

								lExtraInfoRtc = _Module.m_server.TrsLinkOperation(REC_DELETE,&cTmptrs,NULL);


								if(LOG_DETAIL_FLOW_CONTROL)
								{
									CString str;
									str.Format("Send CMD_TRS_PAID with controller SeqNumber : Trs %d, Status %d, OnOffer %d, link Number %d", 
										pCmdTrs->lTrsNumber,
										trs.m_lStatus,
										trs.m_lOnOffer,
										trs.m_lLinkNumber );
									pInfo->m_pServer->m_cLogger.LogMsg( Cmd.GetParameter(),LOG_PUMP,str ); 
								}
							}
						}
						else
							lRetCode = GPI_TRS_NOT_FOR_SALE;
					}
					else // Server is Master 
					{
						bInternal = TRUE;
						lRetCode = trs.SetStatePaid(Cmd.GetSrc());
					}
				}
				else
				{
					lRetCode = GPI_TRS_NOT_FOUND;									
				}


				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------	
				if (slPumpArray.IsLocked())
					slPumpArray.Unlock();
			}
			break;

			case CMD_TRS_CANCEL:
			//--------------------------------------------------------
			{
				strCmdName = "CancelTrs";
				CMD_STRUCT_TRS_CANCEL	*pCmdInfo;
				pCmdInfo = (CMD_STRUCT_TRS_CANCEL *)Cmd.GetDataPtr();
				hRes = pEvent->CancelTrs(Cmd.GetParameter(),
										pCmdInfo->lResExt,
										pCmdInfo->lRes,
										pCmdInfo->lPumpStatus,
										pCmdInfo->lTrsNumber,
										&lEventRetCode);
			}
			break;

			case CMD_SWIPE_RESULT:  //4.0.14.500
			//--------------------------------------------------------
			{
				ExecuteSwipeResult(Cmd, strCmdName, pInfo, hRes, pEvent, lEventRetCode);
			}		
			break;
			case CMD_SWIPE_RECEIPT:
			//--------------------------------------------------------
			{	
				BOOL  bFoundReceiptExtraData = FALSE;		 
				long lPumpNumber = Cmd.GetParameter();
				strCmdName = "SwipeResult(receipt)";

				CMD_STRUCT_PAP_SWIPE_RECEIPT3_EX *pCmdInfo;

				RECEIPT_DATA4_EX  cTagReceiptData4;
				pCmdInfo = (CMD_STRUCT_PAP_SWIPE_RECEIPT3_EX *)Cmd.GetDataPtr();

				//4.0.27.45 - do not use xml - use structs:
				cTagReceiptData4.CardSaleData      = pCmdInfo->data;
				cTagReceiptData4.CardSaleExtraData = pCmdInfo->extData;
				cTagReceiptData4.extData2          = pCmdInfo->extData2;
				cTagReceiptData4.extData3          = pCmdInfo->extData3;
				cTagReceiptData4.extData4          = pCmdInfo->extData4;
				cTagReceiptData4.extData5          = pCmdInfo->extData5;	
				cTagReceiptData4.extData6		   = pCmdInfo->extData6;
				cTagReceiptData4.PumpTransact      = pCmdInfo->trs;
				
				pInfo->m_pServer->m_cProtectedDataHandler.EncryptOLAFields(&cTagReceiptData4);			
						
 				long lOlaRecordSize =sizeof(cTagReceiptData4.CardSaleData) +
								     sizeof(cTagReceiptData4.CardSaleExtraData)  + 
								     sizeof(cTagReceiptData4.extData2) +	
								     sizeof(pCmdInfo->trs);

				if ( pCmdInfo->lFlags & STRUCT_IS_RECEIPT_DATA3 )
				{
					lOlaRecordSize +=sizeof(cTagReceiptData4.extData3) ;
					lOlaRecordSize +=sizeof(cTagReceiptData4.extData4) ;
					lOlaRecordSize +=sizeof(cTagReceiptData4.extData5) ;
					lOlaRecordSize +=sizeof(cTagReceiptData4.extData6) ;
				}
				
				//Add the extra receipt information //4.0.5.39
				if (_Module.m_server.GetOLAFlags(cTagReceiptData4.extData2.sFlags , FL_EXTRA_RECEIPT_REC))
				{
					//RECEIPT_EXTRA_DOUBLE_REC	cReceiptExtraRec;
					PS::RECEIPT_EXTRA_INFO_REC cReceiptExtraRec;
					long lRtc = 0;

					lOlaRecordSize += sizeof(cTagReceiptData4.cReceiptExtraInfo);
					pCmdInfo->lFlags = STRUCT_IS_RECEIPT_DATA4;
					bFoundReceiptExtraData = TRUE;

					memset(&cReceiptExtraRec, ' ', sizeof(PS::RECEIPT_EXTRA_INFO));
					cReceiptExtraRec.info.lIndexNumber = a2l(pCmdInfo->data.sTranNbr, sizeof(pCmdInfo->data.sTranNbr));

					lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, cReceiptExtraRec);
					//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cReceiptExtraRec);

					if (!lRtc)
					{
						memcpy(&cTagReceiptData4.cReceiptExtraInfo.sFiler2043, &cReceiptExtraRec.info.sFiler2043, sizeof(cReceiptExtraRec.info.sFiler2043));
					}

					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString sMsg;
						sMsg.Format("SeqNum=%ld,lRtc=%ld Receipt information was send to controller with receipt extra information" , a2l(pCmdInfo->data.sTranNbr , sizeof(pCmdInfo->data.sTranNbr)) , lRtc);
						_LOGMSG.LogMsg(a2l(pCmdInfo->trs.sPump, sizeof(pCmdInfo->trs.sPump)),LOG_PUMP,sMsg);	
						sMsg.Format("Add zone data to receipt");
						_LOGMSG.LogMsg(a2l(pCmdInfo->trs.sPump, sizeof(pCmdInfo->trs.sPump)),LOG_PUMP,sMsg);
					}
				}
				else
				{
					memset(cTagReceiptData4.cReceiptExtraInfo.sFiler2043, ' ', sizeof cTagReceiptData4.cReceiptExtraInfo.sFiler2043);
				}
				
				if (_Module.m_server.m_bUseXmlBuffer)
				{
					lOlaRecordSize = MAX_XML_BUF_LEN;
				}

				BYTE *sTmpAllocationBuffer = new BYTE[lOlaRecordSize + 2];
				memset(sTmpAllocationBuffer,0,lOlaRecordSize + 2);

				if (_Module.m_server.m_bUseXmlBuffer) 
				{
					bRet = _Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml((char*)sTmpAllocationBuffer,&cTagReceiptData4);
					if (!bRet)
						_LOGMSG.LogMsg("XmlProblem: SWIPE_RECEIPT, StructToXml",LOG_LEVEL_1);
				}
				else
				{
					Str2StrNull((BYTE *)&cTagReceiptData4,lOlaRecordSize,sTmpAllocationBuffer);
				}
				
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString strTmp;
					strTmp.Format("Swipe receipt len=%ld",lOlaRecordSize);					
					pInfo->m_pServer->m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,strTmp);
				}


				CComBSTR strData(lOlaRecordSize +2,(LPCSTR)sTmpAllocationBuffer); 
				delete []sTmpAllocationBuffer;

				
				if (pInfo->m_pServer->GetOLAFlags(pCmdInfo->extData3.sFlags1 ,FL_CANCEL_REC,sizeof(pCmdInfo->extData3.sFlags1)))  
				{
					pCmdInfo->lAnswer = CD_CANCEL_OK;
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("Cancel Receipt Pump=%ld" , lPumpNumber );
						pInfo->m_pServer->m_cLogger.LogMsg( lPumpNumber,LOG_PUMP,str ); 
					}
				}
				else if (!ChrAll((char *)pCmdInfo->extData2.sClockFlags,sizeof(pCmdInfo->extData2.sClockFlags)))
				{
					if (pCmdInfo->data.sTranStatus == CD_APPROVE_OK)
						pCmdInfo->lAnswer = CD_CLOCK_OK;
					else
						pCmdInfo->lAnswer = CD_CLOCK_NOT_OK;


					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("Attendant clock result is : Rtc=%ld" , pCmdInfo->lAnswer);
						pInfo->m_pServer->m_cLogger.LogMsg( lPumpNumber,LOG_PUMP,str ); 
					}	
				}	

				hRes = pEvent->PAPSwipeResult( Cmd.GetParameter(),
											pCmdInfo->lDeviceNumber,
											pCmdInfo->lSeqNumber,
											pCmdInfo->lAnswer,
											pCmdInfo->lFlags,							
											strData,						
											&lEventRetCode);
			}
			break;
			
			case CMD_GET_TANK_READING:	// Server Generated (Master & Slave -> IGCIEvents)
			//----------------------------------------------------------------------------
			{
				strCmdName = "GetTankReading";							
				CMD_STRUCT_TANK_READING *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_TANK_READING *)Cmd.GetDataPtr();
				hRes = pEvent->GetTankReading(	Cmd.GetParameter(),															
												Cmd.GetSrc(),
												pCmdInfo->lStatus,
												&lEventRetCode);						
			}
			break;

			case CMD_GET_DELIVERY_REPORT:	// Server Generated (Master & Slave -> IGCIEvents)
			//----------------------------------------------------------------------------
			{
				strCmdName = "GetDeliveryReport";							
				hRes = pEvent->GetDeliveryReport(	Cmd.GetParameter(),
													Cmd.GetSrc(),
													&lEventRetCode);						
			}
			break;

			case CMD_WET_STOCK_DATA:	// Server Generated (Master & Slave -> IGCIEvents)
			//----------------------------------------------------------------------------
			{
				strCmdName = "GetWetStockData";
				hRes = pEvent->GetWetStockData(	Cmd.GetParameter(),
												&lEventRetCode);

				// Move to wait tank reading state in get wetstock report async process
				//4.0.22.503 - TD 69905
				if ((_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_REPORT) &&
					(_Module.m_server.m_cServerInfo.GetReportRequestState() == REPORT_SEND_TANK_READING))
				{
					_Module.m_server.m_cServerInfo.SetReportRequestState(REPORT_WAIT_TANK_READING, "CGCI::QueueHandle");   // TD 351225
				}

				if ( lEventRetCode != GCI_EVENT_OK )
				{
					_Module.m_server.m_cServerInfo.ClearAllWetStockFlags(); //4.0.8.500
				}
			}
			break;

			case CMD_RE_CONFIGURE:	// Server Generated (Master & Slave -> IGCIEvents)
			//----------------------------------------------------------------------------
			{
				strCmdName = "ReConfigure";
				hRes = pEvent->ReConfigure(	Cmd.GetParameter(),
											&lEventRetCode);
			}
			break;
			
			case CMD_SET_GRADE_PRICE:
			//--------------------------------------------------------
			{
				strCmdName = "SetGradePrice";
				CMD_STRUCT_GRADE_PRICE *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_GRADE_PRICE *)Cmd.GetDataPtr();
				hRes = pEvent->SetGradePrice(	Cmd.GetParameter(),
												Cmd.GetSrc(),
												pCmdInfo->nNozzle,
												pCmdInfo->nGrade,
												pCmdInfo->nFlags,
												pCmdInfo->lPriceA,
												pCmdInfo->lPriceB,
												&lEventRetCode);
				
				// TD 415542
				if (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse())
				{
					if (pCmdInfo->nFlags & PRICE_END_OF_LIST)
					_Module.m_server.m_cGPIEventMgr.AddGeneralSystemEvent(GPI_EVENT_LOAD_PUMP_GRADES, 0, 0);
				}				
			}						
			break;

			case CMD_GET_PUMP_TOTALS:
			//--------------------------------------------------------
			{
				strCmdName = "GetPumpTotals";
				hRes = pEvent->GetPumpTotals(	Cmd.GetParameter(),
												Cmd.GetSrc(),
												&lEventRetCode);						 
				
			}						
			break;

			case CMD_UPDATE_PARAM:
			//--------------------------------------------------------
			{
				strCmdName = "UpdateParam";
				CMD_STRUCT_UPDATE_PARAM *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_UPDATE_PARAM *)Cmd.GetDataPtr();

				BYTE sTmp1[sizeof(pCmdInfo->szTableName)+2];
				BYTE sTmp2[sizeof(pCmdInfo->szFieldName)+2];
				BYTE sTmp3[sizeof(pCmdInfo->szFieldVal)+2];

				Str2StrNull((BYTE *)pCmdInfo->szTableName,sizeof(pCmdInfo->szTableName),sTmp1);
				Str2StrNull((BYTE *)pCmdInfo->szFieldName,sizeof(pCmdInfo->szFieldName),sTmp2);
				Str2StrNull((BYTE *)pCmdInfo->szFieldVal,sizeof(pCmdInfo->szFieldVal),sTmp3);

				CComBSTR strTableName(min((strlen((const char *)sTmp1)+2),sizeof(sTmp1)),(LPCSTR)sTmp1);							
				CComBSTR strFieldName(min((strlen((const char *)sTmp2)+2),sizeof(sTmp2)),(LPCSTR)sTmp2);							
				CComBSTR strFieldVal(min((strlen((const char *)sTmp3)+2),sizeof(sTmp3)),(LPCSTR)sTmp3);							

				hRes = pEvent->UpdateParam(Cmd.GetSrc(),
											strTableName,
											pCmdInfo->lKey1,
											pCmdInfo->lKey2,
											strFieldName,
											pCmdInfo->nFieldType,
											strFieldVal,
											pCmdInfo->lVal,
											&lEventRetCode);
			}
			break;
			case CMD_FORMAT_RECEIPT:
			//--------------------------------------------------------
			{
				DWORD			dwLastErr;
				TAG_OLA_REC13	tagOlaRec;								//4.0.22.61	TD 75605				
				long			lOlaRecordSize= sizeof(TAG_OLA_REC13);  //4.0.27.46
				long			lRtc;
				long			lFound = RECEIPT_NOT_FOUND;  //Eyal 3.2.0.67 changed from BOOL to long

				strCmdName = "FormatReceipt";
				CMD_STRUCT_FORMAT_RECEIPT *pCmdInfo ,cmdInfo;
				pCmdInfo = (CMD_STRUCT_FORMAT_RECEIPT *)Cmd.GetDataPtr();

//				BYTE sTmp1[TRACK_2_DATA_SIZE +2];
				BYTE sTmp1[sizeof(pCmdInfo->sTrack2) + 2]; // TD 454500

				Str2StrNull((BYTE *)pCmdInfo->sTrack2,sizeof(pCmdInfo->sTrack2),sTmp1); 
				CComBSTR strTrack2(min((strlen((const char *)sTmp1)+2),sizeof(sTmp1)),(LPCSTR)sTmp1);							
		
				///////////////////////////////////////////////////

				long lPumpNumber = a2l(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sPump, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sPump));

				//Eyal 3.2.0.67 changed from BOOL to long
				CStringA cTrack2(pCmdInfo->sTrack2, sizeof(pCmdInfo->sTrack2));
				lFound = _Module.m_server.BuildRequestReceiptInfo(cmdInfo , cTrack2, pCmdInfo->lIndex , pCmdInfo->lFlags , pCmdInfo->lPosNumber , &tagOlaRec);

				if (lFound == RECEIPT_FOUND)
				{
					ITEMS_INFO	cItemInfo;	//4.0.25.501 TD 138045 merge from 4.0.22.461
					long  lItemIndex = 0;
					CComBSTR sReceiptText;
					BOOL bOLAHasThreeRec  = _Module.m_server.GetOLAFlags( tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sFlags , FL_PAP_AS_THIRD_REC);
					BOOL bBAPExtraRec     =  _Module.m_server.GetOLAFlags(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sFlags , FL_EXTRA_ITEM_REC); 
					BOOL bReceiptExtraRec =  _Module.m_server.GetOLAFlags(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sFlags , FL_EXTRA_RECEIPT_REC);  //4.0.5.39
						
					lItemIndex = a2l(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sItemsLinkIndex,sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sItemsLinkIndex));

					if (lItemIndex)
					{						

						if (bOLAHasThreeRec)
						{
							pCmdInfo->lFlags +=RECEIPT_THREE_OLA_REC;
							
							if (bBAPExtraRec)     //4.0.5.27
							{
								pCmdInfo->lFlags += RECEIPT_WITH_EXTRA_ITEM ;
							}
						
						}
						pCmdInfo->lFlags += RECEIPT_FORMAT_WITH_ITEMS;

						if (LOG_BASIC_FLOW_CONTROL)
						{
							CString	str;
							str.Format("QueueHandle Process command CMD_FORMAT_RECEIPT, Read ItemIndex = %ld, lFlags = %ld",
								tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber,
								pCmdInfo->lFlags);
							_LOGMSG.LogMsg(str);
						}

						// insert qdx index //4.0.25.501 TD 138045 
						memset(&cItemInfo, 0 , sizeof(cItemInfo));
						cItemInfo.ItemRec.lIndexNumber = lItemIndex;
						cItemInfo.ItemRec2.lIndexNumber = lItemIndex + 1;
						cItemInfo.ItemRec3.lIndexNumber = lItemIndex + 2;
						cItemInfo.ItemRec4.lIndexNumber = lItemIndex + 3;
					
						lRtc = _Module.m_server.IndexOperation(ITEMS_FILE,REC_READ,&cItemInfo);
	
						if (lRtc)
						{							
							memset(&tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.Items ,' ', sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.Items));
							memset(&tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.Items2 ,' ', sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.Items2));
							if(LOG_BASIC_FLOW_CONTROL)
							{	
								CString str;
								str.Format("Fail !!!! read first Index = %ld from file Items" , cItemInfo.ItemRec.lIndexNumber);
								_LOGMSG.LogMsg(a2l(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sPump, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sPump)), LOG_PUMP, str, LOG_LEVEL_1);
							}
						}
						else
						{
							CItemsFile cItemsFile;
							BAP_ALL_DATA_STRUCTS  cBAPItem;
							cItemsFile.ConstructItemsRecordToBAP(cItemInfo ,cBAPItem);	
							tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.Items  = cBAPItem.cBAPReceiptItem;
							tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.Items2 = cBAPItem.cBAPReceiptItem2;
						}																																

						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString	str;
							str.Format("QueueHandle Process command CMD_FORMAT_RECEIPT, Read ItemIndex = %ld, lFlags = %ld", 
								cItemInfo.ItemRec.lIndexNumber, 
								pCmdInfo->lFlags);
							_LOGMSG.LogMsg(str);
						}	//4.0.25.501 TD 138045 
						
						// add extra receipt data... //4.0.5.39
						if (bReceiptExtraRec)
						{
							PS::RECEIPT_EXTRA_INFO_REC cReceiptExtraRec;
							memset(&cReceiptExtraRec, 0, sizeof(PS::RECEIPT_EXTRA_INFO_REC));
							cReceiptExtraRec.info.lIndexNumber = a2l(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sTranNbr, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sTranNbr));

							lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, cReceiptExtraRec);
							//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cReceiptExtraRec);

							// Fail to read receipt extra data
							if (lRtc)
							{
								memset(cReceiptExtraRec.info.sFiler2043, ' ', sizeof(cReceiptExtraRec.info.sFiler2043));
							}
							else
							{
								memcpy((char *)tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.ReceiptExtraInfo.sFiler2043, (char *)cReceiptExtraRec.info.sFiler2043, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.ReceiptExtraInfo.sFiler2043));
								pCmdInfo->lFlags += RECEIPT_WITH_RECEIPT_EXTRA_DATA;
							}

							if(LOG_DETAIL_FLOW_CONTROL)
							{
								CString sMsg;
								long lTrs = a2l(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sTranNbr, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sTranNbr));
								sMsg.Format("FormatReceipt, Trs = %ld, Receipt information was send to controller without receipt extra information", lTrs);
								_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg);
							}
						}
// 
// 						//4.0.14.500
// 						if (_Module.m_server.m_bUseXmlBuffer)			FormatReceipt is work only in UseXmlBuffer mode
// 						{
// 							lOlaRecordSize = MAX_XML_BUF_LEN;
// 						}
// 
// 						BYTE *sTmpAllocationBuffer = new BYTE [lOlaRecordSize+2];  //4.0.5.0
// 
// 						//4.0.14.500
// 						if (_Module.m_server.m_bUseXmlBuffer)
// 						{
// 							memset(sTmpAllocationBuffer,0,lOlaRecordSize+2);
// 							_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml((char*)sTmpAllocationBuffer,&tagOlaRec);
// 						}
// 						else
// 						{
// 							Str2StrNull((BYTE *)&tagOlaRec,lOlaRecordSize,sTmpAllocationBuffer);
// 						}

						lOlaRecordSize = MAX_XML_BUF_LEN;

						BYTE *sTmpAllocationBuffer = new BYTE[lOlaRecordSize + 2];  //4.0.5.0

						memset(sTmpAllocationBuffer, 0, lOlaRecordSize + 2);
						_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml((char*)sTmpAllocationBuffer, &tagOlaRec);


						CComBSTR sReceiptData(lOlaRecordSize + 2 ,(LPCSTR)sTmpAllocationBuffer); 	
						delete []sTmpAllocationBuffer;    //4.0.5.08

						hRes = pEvent->FormatReceipt(sReceiptData , pCmdInfo->lFlags, &sReceiptText  , &lEventRetCode);
					}
					else
					{
						if (bOLAHasThreeRec)
						{
							pCmdInfo->lFlags += RECEIPT_THREE_OLA_REC;														
						}

						// add extra receipt data... //4.0.5.39
						if (bReceiptExtraRec)
						{							
							PS::RECEIPT_EXTRA_INFO_REC cReceiptExtraRec;
							memset(&cReceiptExtraRec, 0, sizeof(PS::RECEIPT_EXTRA_INFO_REC));
							cReceiptExtraRec.info.lIndexNumber = a2l(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sTranNbr, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sTranNbr));

							lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, cReceiptExtraRec);
							//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cReceiptExtraRec);

							// Fail to read receipt extra data
							if (lRtc)
							{
								memset(cReceiptExtraRec.info.sFiler2043, ' ', sizeof(cReceiptExtraRec.info.sFiler2043));
							}
							else
							{
								memcpy((char *)tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.ReceiptExtraInfo.sFiler2043, (char *)cReceiptExtraRec.info.sFiler2043, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.ReceiptExtraInfo.sFiler2043));
								pCmdInfo->lFlags += RECEIPT_WITH_RECEIPT_EXTRA_DATA;
							}


							if(LOG_DETAIL_FLOW_CONTROL)
							{
								CString sMsg;
								sMsg.Format("FormatReceipt without items with extra receipt data, Trs=%ld,lFlags=%ld", a2l(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sNumber, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sNumber)) ,pCmdInfo->lFlags);
								_LOGMSG.LogMsg(a2l(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sPump,sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs.sPump)),LOG_PUMP,sMsg);	
							}
						}

						//TD 56250 - if we got the flag to return only original- just for this time, we 
						//			 override the ReceiptPrinted flag with zero before asking the receipt. //4.0.15.450
						if (pCmdInfo->lFlags & RECEIPT_GET_ORIGINAL)
						{
							tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sReceiptPrinted = '0';
						}

// 						//4.0.14.500
// 						if (_Module.m_server.m_bUseXmlBuffer)
// 						{
							lOlaRecordSize = MAX_XML_BUF_LEN;
//						}

						BYTE *sTmpAllocationBuffer = new BYTE [lOlaRecordSize+2];  //4.0.5.0

// 						//4.0.14.500
// 						if (_Module.m_server.m_bUseXmlBuffer)
// 						{
							memset(sTmpAllocationBuffer,0,lOlaRecordSize+2);
							_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml((char*)sTmpAllocationBuffer,&tagOlaRec);
// 						}
// 						else
// 						{
// 							Str2StrNull((BYTE *)&tagOlaRec,lOlaRecordSize,sTmpAllocationBuffer);
// 						}

						CComBSTR sReceiptData(lOlaRecordSize+2,(LPCSTR)sTmpAllocationBuffer); 	
												
						delete [] sTmpAllocationBuffer; //4.0.5.0

						hRes = pEvent->FormatReceipt(sReceiptData , pCmdInfo->lFlags, &sReceiptText  , &lEventRetCode);
					}

				////////////////////////////////////////////////////////////////////////////////////////
					if(LOG_BASIC_FLOW_CONTROL && (hRes == 0)) //4.0.20.501	TD 60236
					{					
						char sBuffRet[3000];
						int iLen;

						iLen = WideCharToMultiByte(CP_ACP,0,sReceiptText,-1,sBuffRet,sizeof(sBuffRet),NULL,NULL);

						dwLastErr = GetLastError();
						if(iLen == 0)
						{
							dwLastErr = GetLastError();
						}
						
						char szResult[MAX_FIELD_VAL] ={0};
						long lRetVal = _Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, pCmdInfo->sTrack2, sizeof(pCmdInfo->sTrack2));
						
						CString	str;
						str.Format("QueueHandle::FormatReceipt after Sent To GCIEventProcess  szTrack2=[%s], lIndex=%ld SizeBuffRet=%d  dwLastErr=%ld retCode=%d, lFlags=%ld" , szResult, pCmdInfo->lIndex, iLen, dwLastErr, lEventRetCode, pCmdInfo->lFlags);
							
						_LOGMSG.LogMsg(str); 				
					}			
				   /////////////////////////////////////////////////////////////////////////////////////////

					if ((lEventRetCode == GCI_EVENT_OK) && (hRes == 0)) //4.0.18.501
					{
						//Get handle Pos receipt 
						CSingleLock slPosMap( &(_Module.m_server.m_csPosMap));	
						slPosMap.Lock(INFINITE);
						CPosInfo tmpPos;
						
						if(_Module.m_server.m_cPosMap.Lookup(pCmdInfo->lPosNumber,tmpPos))
						{
							tmpPos.SetReceiptText(sReceiptText);
							tmpPos.SendEventReceipt();
							_Module.m_server.m_cPosMap.SetAt(pCmdInfo->lPosNumber,tmpPos);
						}
						slPosMap.Unlock();				
					}					
					
					else
						lEventRetCode = GPI_NO_RECEIPT;
				}				
				else
					 lEventRetCode = GPI_NO_RECEIPT;				
				
				///////////////////////////////////////////////				

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("Receipt format end  lIndex=%ld  Found=%d" , lEventRetCode , lFound);
					_LOGMSG.LogMsg(str);
				}			

				///////////////////////////////////////////////////////////////		
			}
			break;
			case CMD_PRINT_REPORT:
			//--------------------------------------------------------
			{
				strCmdName = "PrintReport";
				CMD_STRUCT_PRINT_REPORT *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_PRINT_REPORT *)Cmd.GetDataPtr();
				
				BYTE sTmp[sizeof(pCmdInfo->data)+2];
				Str2StrNull((BYTE *)&pCmdInfo->data,sizeof(pCmdInfo->data),sTmp);
				CComBSTR sCardData(sizeof(sTmp),(LPCSTR)sTmp); 

				hRes = pEvent->PrintReport( pCmdInfo->lOriginTrans,
											pCmdInfo->lDeviceNumber,
											sCardData,
											pCmdInfo->lFlags,
											pCmdInfo->lTextLength,
											&lEventRetCode );
				
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("QueueHandle:: CMD_PRINT_REPORT lTerminalType=%ld lTerminalNumber=%ld lFlags=%ld lLen=%ld retCode=%d" , pCmdInfo->lOriginTrans, pCmdInfo->lDeviceNumber, pCmdInfo->lFlags, pCmdInfo->lTextLength, lEventRetCode);
					_LOGMSG.LogMsg(str);
				}	
			}
			break;

			//Eyal 3.2.0.3 Casey's
			case CMD_TLS:
			{
				strCmdName = "TLSCommand";
				CMD_STRUCT_TLS * pCmdInfo = (CMD_STRUCT_TLS*)Cmd.GetDataPtr();
				
				BYTE sTmp[sizeof(pCmdInfo->sCommand)+2];
				Str2StrNull((BYTE *)&pCmdInfo->sCommand,sizeof(pCmdInfo->sCommand),sTmp);
				CComBSTR sTmpCommand(sizeof(sTmp),(LPCSTR)sTmp); 
				BSTR sCommand = sTmpCommand.Copy();

				pEvent->TLSCommand(sCommand,
								   pCmdInfo->lCommand,
					               pCmdInfo->lTankNumber,
								   pCmdInfo->lFlags,
								   &lEventRetCode);
				
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("QueueHandle: TLSCommand %s , lFlags = %d , lParameters = %d" , pCmdInfo->sCommand , pCmdInfo->lFlags , pCmdInfo->lTankNumber);
					_LOGMSG.LogMsg(str);
				}	
			}
			break;
			case CMD_DEBIT_PROCESS:
			{
				strCmdName = "DebitProcess";
				CMD_STRUCT_DEBIT_PROCESS * pCmdInfo = (CMD_STRUCT_DEBIT_PROCESS*)Cmd.GetDataPtr();
				
				BYTE sTmp[sizeof(pCmdInfo->cOLaDebitInfo)+2];
				Str2StrNull((BYTE *)&pCmdInfo->cOLaDebitInfo,sizeof(pCmdInfo->cOLaDebitInfo),sTmp);
				CComBSTR sTmpCommand(sizeof(sTmp),(LPCSTR)sTmp); 
				BSTR sData = sTmpCommand.Copy();

				pEvent->DebitProcessCmd(pCmdInfo->lPumpNumber, pCmdInfo->lSessionId, pCmdInfo->lFlags, sData,&lEventRetCode);

				//4.0.2.15
				SysFreeString(sData);
				
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("QueueHandle Debit Process : lFlags = %d", pCmdInfo->lFlags );
					_LOGMSG.LogMsg(str);
				}	
			}
			break;//4.0.3.36
			case CMD_GENERAL_SYSTEM_EVENT:
			//----------------------------------------------------------------------------
			{
				strCmdName = "GeneralSystemEvent";
				
				//4.0.22.503 - TD 72189
				if (Cmd.GetParameter() == GCI_EVENT_CHANGE_PRODUCT_IN_TANK)
				{
					CComBSTR bstrInfo;
					_Module.m_server.m_cServerInfo.GetChangeProductInTankInfo(&bstrInfo.m_str);

					pEvent->GeneralSystemEvent2(Cmd.GetParameter(), Cmd.GetSrc(), bstrInfo, &lEventRetCode);

					_Module.m_server.m_cServerInfo.SetChangeProductInTankState(CPIT_WAIT_CL_RESPONSE);
					
					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString	str;
						str.Format("Change product in tank process state=%ld (CPIT_WAIT_CL_RESPONSE)", _Module.m_server.m_cServerInfo.GetChangeProductInTankState());
						_LOGMSG.LogMsg(str);
					}
				}
				else
					pEvent->GeneralSystemEvent(Cmd.GetParameter(), Cmd.GetSrc(), &lEventRetCode);

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("GeneralSystemEvent was sent to convert layer, lEventId = %d, Src = %d",Cmd.GetParameter(), Cmd.GetSrc());
					_LOGMSG.LogMsg(str);
				}
			}
			break;
			//4.0.2.21
			case CMD_SET_PUMP_MODE2:
			{	
				strCmdName = "SetPumpMode2";
				CMD_STRUCT_MODE2 *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_MODE2 *)Cmd.GetDataPtr();

				if(THRD_PUMPSRV_IS_MODE_MASTER)
				{
					bInternal = TRUE;					
					CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

					CPumpStatus stat = pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].GetStat();
					stat.SetMode(pCmdInfo->lMode);
					pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].SetStat(stat);
					pInfo->m_pServer->SavePumpInfo(Cmd.GetParameter());

					if (slPumpArray.IsLocked())
						slPumpArray.Unlock();	

					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString	str;
						str.Format("SetPumpMode2 , Mode=%ld , Pump Service Mode=%ld" ,pCmdInfo->lMode, pCmdInfo->lServiceMode , pCmdInfo->lServiceFee );
						_LOGMSG.LogMsg(Cmd.GetParameter(),LOG_PUMP,str);
					}

				
					try
					{
						// check if need to update controller about service mode.
						hRes = pEvent->SetPumpMode2(	Cmd.GetParameter(),
														Cmd.GetSrc(),																			
														pCmdInfo->lMode,
														pCmdInfo->lFlags,
														pCmdInfo->lServiceMode,
														pCmdInfo->lServiceFee,
														&lEventRetCode);								
					}
					catch(...)
					{
						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString	str;
							str.Format("Error!!! - SetPumpMode2 Mode=%ld , Pump Service Mode=%ld" ,pCmdInfo->lMode , pCmdInfo->lServiceMode );
							_LOGMSG.LogMsg(Cmd.GetParameter(),LOG_PUMP,str,LOG_LEVEL_1);
						}

						lEventRetCode = 0;
					}
				}
				else 
				{
					hRes = pEvent->SetPumpMode(	Cmd.GetParameter(),
												Cmd.GetSrc(),																			
												pCmdInfo->lMode,
												&lEventRetCode);								
				}
			}				
			break;

			case CMD_SET_PUMP_MODE3:
				{	
					strCmdName = "SetPumpMode3";
					CMD_STRUCT_MODE3 *pCmdInfo;
					pCmdInfo = (CMD_STRUCT_MODE3 *)Cmd.GetDataPtr();
					
					if(THRD_PUMPSRV_IS_MODE_MASTER)
					{
						bInternal = TRUE;					
						CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

						CPumpStatus stat = pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].GetStat();
						stat.SetMode(pCmdInfo->lMode);
						pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].SetStat(stat);
						pInfo->m_pServer->SavePumpInfo(Cmd.GetParameter());
						
						if (slPumpArray.IsLocked())
							slPumpArray.Unlock();
						
						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString	str;
							str.Format("SetPumpMode3 , Mode=%ld , Pump Service Mode=%ld , Pump Price Level=%ld" , pCmdInfo->lMode , pCmdInfo->lServiceMode ,pCmdInfo->lPriceLevel);
							_LOGMSG.LogMsg(Cmd.GetParameter(),LOG_PUMP,str);
						}
						
						
						try
						{
							// check if need to update controller about service mode.
							hRes = pEvent->SetPumpMode3(	Cmd.GetParameter(),
								Cmd.GetSrc(),																			
								pCmdInfo->lMode,
								pCmdInfo->lFlags,
								pCmdInfo->lServiceMode,
								pCmdInfo->lPriceLevel,
								pCmdInfo->lServiceFee,
								&lEventRetCode);								
						}
						catch(...)
						{
							if(LOG_BASIC_FLOW_CONTROL)
							{
								CString	str;
								str.Format("SetPumpMode3 , Mode=%ld , Pump Service Mode=%ld , Pump Price Level=%ld" , pCmdInfo->lMode , pCmdInfo->lServiceMode , pCmdInfo->lPriceLevel);
								_LOGMSG.LogMsg(Cmd.GetParameter(),LOG_PUMP,str,LOG_LEVEL_1);
							}
							
							lEventRetCode = 0;
						}					
					}
					else 
					{
						hRes = pEvent->SetPumpMode(	Cmd.GetParameter(),
							Cmd.GetSrc(),																			
							pCmdInfo->lMode,
							&lEventRetCode);								
					}
					
				}				
				break;

			//4.0.2.29 start
			case CMD_RECONFIGURE_EVENT:
			{
				CMD_STRUCT_RECONFIGURATION_EVENT * pParams = (CMD_STRUCT_RECONFIGURATION_EVENT*)Cmd.GetDataPtr();

				//4.0.3.51
				//Check for reconfiguration tasks delegated to CmdTX
				ReconfigurationEvent(pParams->lTableId,
					                 pParams->lSubTableId,
									 pParams->lSubTableIdExt,
									 pParams->lFlags);

				//Send event to CL
				hRes = pEvent->ReConfigure2(pParams->lTableId,
					                        pParams->lSubTableId,
											pParams->lSubTableIdExt,
											pParams->lFlags,
											&lEventRetCode);
				
				//4.0.19.504 - 47559
				if (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse())  // CR 444830
					_Module.m_server.m_cGPIEventMgr.AddReconfigure(pParams->lTableId, pParams->lSubTableId, pParams->lSubTableIdExt, pParams->lFlags);  //4.0.140.18

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("RECONFIGURE_EVENT lTableId=%ld, lSubTableId=%ld,lSubTableIdExt=%ld,lFlags=%ld, lEventRetCode=%ld" ,pParams->lTableId,pParams->lSubTableId, pParams->lSubTableIdExt ,pParams->lFlags,lEventRetCode);
					_LOGMSG.LogMsg(str,LOG_LEVEL_3);
				}
				
				break;
			}
			//4.0.2.29 end
			//4.0.3.21
			case CMD_CARD_INFO_RESULT:
			{				
				long lPumpNumber = Cmd.GetParameter();
				strCmdName = "Card info Result";
				CMD_STRUCT_CARD_INFO_RESULT_XML *pCmdInfo;	//4.0.5.0
				pCmdInfo = (CMD_STRUCT_CARD_INFO_RESULT_XML *)Cmd.GetDataPtr();
				CARD_SALE_ALL3 cCardSaleAll3;
				memset(&cCardSaleAll3,' ',sizeof(cCardSaleAll3));

				long lOlaRecordSize;
				if (_Module.m_server.m_bUseXmlBuffer)
					lOlaRecordSize = MAX_XML_BUF_LEN;
				else
					lOlaRecordSize = (STRUCT_IS_CARD_ALL_DATA3  & pCmdInfo->lFlags)?sizeof(CARD_SALE_ALL3): sizeof(CARD_SALE_ALL2);

				if(LOG_BASIC_FLOW_CONTROL)
				{
					//4.0.5.12
					CString strTmp;
					strTmp.Format("CMD_CARD_INFO_RESULT: Answer=%ld ,lFlags=%ld",
						pCmdInfo->lAnswer, pCmdInfo->lFlags);					

					 pInfo->m_pServer->m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,strTmp);
				}
				
				BYTE *sTmpAllocationBuffer = new BYTE[lOlaRecordSize+2]; //4.0.5.0
				
				memset(sTmpAllocationBuffer, 0, lOlaRecordSize+2);


				if (_Module.m_server.m_bUseXmlBuffer)
				{
					memcpy(sTmpAllocationBuffer, &pCmdInfo->data, lOlaRecordSize);				
				}
				else
				{
					_Module.m_server.m_cXmlConvertorEx.ConvertFromXmlToStruct((char*)&pCmdInfo->data,&cCardSaleAll3);
					memcpy(sTmpAllocationBuffer, &cCardSaleAll3, lOlaRecordSize);				
				}

				CComBSTR strData(lOlaRecordSize + 2,(LPCSTR)sTmpAllocationBuffer); 

				delete []sTmpAllocationBuffer; //4.0.5.0

				hRes = pEvent->CardInfoResult( Cmd.GetParameter(),
											pCmdInfo->lDeviceNumber,
											pCmdInfo->lSeqNumber,
											pCmdInfo->lAnswer,
											pCmdInfo->lFlags,							
											strData,						
											&lEventRetCode);						 
				
			}
			break;

			case CMD_TANK_GAUGE_EMULATION_READING:   //4.0.5.24
			{
				strCmdName = "TankGaugeData";
				long lSize = sizeof(TAG_TGDTankReading);

				CMD_STRUCT_TANK_GAUGE_EMULATION_READING * pCmdInfo = (CMD_STRUCT_TANK_GAUGE_EMULATION_READING*)Cmd.GetDataPtr();
				BYTE *sTmpAllocationBuffer = new BYTE[lSize +2]; //4.0.5.0
				
				memset(sTmpAllocationBuffer, 0,lSize +2);
				memcpy(sTmpAllocationBuffer, &pCmdInfo->cTGDTankReading, lSize);				
				CComBSTR strData(lSize  + 2,(LPCSTR)sTmpAllocationBuffer); 

				delete []sTmpAllocationBuffer; //4.0.5.0


				hRes = pEvent->TankGaugeData(pCmdInfo->lTankNumber,pCmdInfo->lFlags,strData,&lEventRetCode);
					
				
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("QueueHandle: TankGaugeData reading Tank Number=%ld lFlags = %d " ,  pCmdInfo->lTankNumber, pCmdInfo->lFlags);
					_LOGMSG.LogMsg(str);
				}			
			}
			break;
			case CMD_TANK_GAUGE_EMULATION_DELIVERY:   //4.0.5.24
			{
				strCmdName = "TankGaugeData";
				long lSize = sizeof(TAG_TGDDeliveryData);

				CMD_STRUCT_TANK_GAUGE_EMULATION_DELIVERY * pCmdInfo = (CMD_STRUCT_TANK_GAUGE_EMULATION_DELIVERY*)Cmd.GetDataPtr();
				BYTE *sTmpAllocationBuffer = new BYTE[lSize +2]; //4.0.5.0
				
				memset(sTmpAllocationBuffer, 0,lSize +2);
				memcpy(sTmpAllocationBuffer, &pCmdInfo->cTGDDeliveryData, lSize);				
				CComBSTR strData(lSize  + 2,(LPCSTR)sTmpAllocationBuffer); 

				delete []sTmpAllocationBuffer; //4.0.5.0

				hRes = pEvent->TankGaugeData(pCmdInfo->lTankNumber,pCmdInfo->lFlags,strData,&lEventRetCode);
					
				
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("QueueHandle: TankGaugeData emulation Tank Number=%ld lFlags = %d " ,  pCmdInfo->lTankNumber, pCmdInfo->lFlags);
					_LOGMSG.LogMsg(str);
				}			
			}
			break;

			///CR 442586

			case CMD_SMART_RECONFIGURE_EVENT:
			{
				CMD_STRUCT_SMART_RECONFIGURATION_EVENT * pParams = (CMD_STRUCT_SMART_RECONFIGURATION_EVENT*)Cmd.GetDataPtr();

				//4.0.3.51
				//Check for reconfiguration tasks delegated to CmdTX
				ReconfigurationEvent(pParams->lTableId,
					pParams->lSubTableId,
					pParams->lSubTableIdExt,
					pParams->lFlags);

				//Send event to CL
				hRes = pEvent->SmartReconfigure(pParams->lDeviceNumberMask1,
					pParams->lDeviceNumberMask2,
					pParams->lTableId,
					pParams->lSubTableId,
					pParams->lSubTableIdExt,
					pParams->lFlags,
					&lEventRetCode);


				if (_Module.m_server.GetIFSFTerminalSrvInUse())
					_Module.m_server.m_cGPIEventMgr.AddSmartReconfigure(pParams->lDeviceNumberMask1, pParams->lDeviceNumberMask2, pParams->lTableId, pParams->lSubTableId, pParams->lSubTableIdExt, pParams->lFlags);  //4.0.140.18

				if (LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("RECONFIGURE_EVENT_3: Device Mask 1-32=%ld, Device Mask 33-64=%ld, lTableId=%ld, lSubTableId=%ld,lSubTableIdExt=%ld,lFlags=%ld, lEventRetCode=%ld", pParams->lDeviceNumberMask1, pParams->lDeviceNumberMask2, pParams->lTableId, pParams->lSubTableId, pParams->lSubTableIdExt, pParams->lFlags, lEventRetCode);
					_Module.m_server.m_cLogger.LogMsg(str, LOG_LEVEL_3);
				}

				break;
			}

			///END CR 442586
			case CMD_FORMAT_REPORT:  //4.0.9.555
			{
				CComBSTR sReportText;
				CComBSTR sRowRec;  //
				long lTmpRtc=0;
				long lCounter = 0;
				BOOL bKnownReportType = TRUE;
								
				strCmdName = "FormatReport";
				CMD_STRUCT_FORMAT_REPORT *pCmdInfo ;
				pCmdInfo = (CMD_STRUCT_FORMAT_REPORT *)Cmd.GetDataPtr();

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str; //4.0.23.504
					str.Format("QueueHandle: FormatReport Get report row details --> ReportType = %d", pCmdInfo->lType);
					_LOGMSG.LogMsg(str);
				}
						
				//4.0.16.502 - TD 35765
				switch(pCmdInfo->lType)
				{
					//4.0.23.504 - TD 78825
					case GET_REPORT_GENERAL_TYPE:
					case REPORT_TYPE_ULLAGE:
					case REPORT_TYPE_DERIVERY:
					{
						_Module.m_server.m_cCommonFunction.GetTankMemoInfo(pCmdInfo->lNumber ,&sRowRec.m_str, pCmdInfo->lFlags, &lTmpRtc, &lCounter , TANK_MEMO_WITH_DELIVERY);
				
						if (!lTmpRtc)
						{
							if(LOG_BASIC_FLOW_CONTROL)
							{
								CString	str;
								str.Format("Send report to format include %ld elements" ,lCounter); 
								_LOGMSG.LogMsg(str);
							}
						}
						

						break;
					}
					case GET_REPORT_PIC_TYPE:
					{
						CComBSTR sHeader;

						lTmpRtc = _Module.m_server.GetCashAcceptorReportXml__(pCmdInfo->lNumber, pCmdInfo->lFlags, &sHeader.m_str, &sRowRec.m_str, &lCounter, FALSE); //4.0.17.501

						break;
					}
					//4.0.22.501 - TD 69905
					case GET_REPORT_WETSTOCK_ASYNC_TYPE:
					{
						_Module.m_server.m_cCommonFunction.GetTankMemoInfo(pCmdInfo->lNumber ,&sRowRec.m_str, pCmdInfo->lFlags, &lTmpRtc, &lCounter, TRM_TYPE_WETSTOCK_REPORT);

						if (!lTmpRtc)
						{
							// Move to wait generate report state in get wetstock report async process 
							_Module.m_server.m_cServerInfo.SetReportRequestState(REPORT_WAIT_GENERATE_REPORT, "CGCI::QueueHandle");      // TD 351225
						}
						else
						{
							_Module.m_server.m_cServerInfo.SetReportRequestState(REPORT_READY, "CGCI::QueueHandle");                       // TD 351225
							_Module.m_server.m_cServerInfo.SetReportRequestFailureResult(REPORT_ERROR_ALL_TANKS_NOT_AVAILABLE, "CGCI::QueueHandle");         // TD 351225
						}
						break;
					}

					default:
					{
						if(LOG_BASIC_FLOW_CONTROL) //4.0.22.150 - log
							_LOGMSG.LogMsg("QueueHandle: FormatReport Unknown report type",LOG_LEVEL_1); //4.0.22.150 - add log level
						bKnownReportType = FALSE;
						break;
					}
				}

				if (!lTmpRtc && bKnownReportType)
				{
					hRes = pEvent->FormatReport(sRowRec, pCmdInfo->lType ,pCmdInfo->lFlags, &sReportText  , &lEventRetCode);

					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString	str; //4.0.223.504 - log
						str.Format("QueueHandle: FormatReport Get Formated information %ld" ,lEventRetCode); 
						_LOGMSG.LogMsg(str);
					}				


					if (pCmdInfo->lType != GET_REPORT_WETSTOCK_ASYNC_TYPE) //4.0.22.501 - TD 69905
					{
						if (lEventRetCode == GCI_EVENT_OK) 
						{
							//Get handle Pos receipt 
							CSingleLock slPosMap( &(_Module.m_server.m_csPosMap));	
							slPosMap.Lock(INFINITE);
							CPosInfo tmpPos;
							
							if(_Module.m_server.m_cPosMap.Lookup(pCmdInfo->lPosNumber,tmpPos))
							{
								tmpPos.SetReceiptText(sReportText);
								tmpPos.SendEventReceipt();
								_Module.m_server.m_cPosMap.SetAt(pCmdInfo->lPosNumber,tmpPos);

							}
							slPosMap.Unlock();				
						}
						else
							lEventRetCode = GPI_NO_RECEIPT;
					}
				}
				else
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString	str; //4.0.232.504 - log
						str.Format("QueueHandle: FormatReport Fail to get row report data Tank=%ld , lCounter=%ld" ,pCmdInfo->lNumber , lCounter );
						_LOGMSG.LogMsg(str,LOG_LEVEL_1);
					}			
				}
				
			}
			break;

			//4.0.12.503
			case CMD_SET_PUMP_STATUS:
			{
				CMD_STRUCT_SET_PUMP_STATUS *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_SET_PUMP_STATUS *)Cmd.GetDataPtr();	

				SetPumpStatus(Cmd.GetParameter(),pCmdInfo->lStatus,pCmdInfo->lRes,
														  pCmdInfo->lResExt,pCmdInfo->pVal);
			}
			break;

			case CMD_PAP_SWIPE:
			{
				CMD_STRUCT_PAP_SWIPE *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_PAP_SWIPE *)Cmd.GetDataPtr();	

				char sTmpAllocationBuffer[sizeof(CARD_SALE_ALL3 ) + 2]={0};
				memset(sTmpAllocationBuffer ,  0 , sizeof(sTmpAllocationBuffer));
				Str2StrNull((BYTE*)&pCmdInfo->cardDataAll3, sizeof(pCmdInfo->cardDataAll3),(BYTE*)sTmpAllocationBuffer);

				CComBSTR strData(sizeof(sTmpAllocationBuffer),(LPCSTR)sTmpAllocationBuffer);


				PAPSwipe(Cmd.GetParameter(),pCmdInfo->lDeviceNumber,&pCmdInfo->lSeqNumber, 
														  pCmdInfo->lFlags,strData ,pCmdInfo->pVal);
			}
			break;

			case CMD_PAP_READY_TRS:
			{

				CMD_STRUCT_PAP_READY_TRS *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_PAP_READY_TRS *)Cmd.GetDataPtr();	

				char sTmpAllocationBuffer[sizeof(CARD_SALE_ALL3 ) + 2]={0};
				memset(sTmpAllocationBuffer ,  0 , sizeof(sTmpAllocationBuffer));
				Str2StrNull((BYTE*)&pCmdInfo->cardDataAll3, sizeof(pCmdInfo->cardDataAll3),(BYTE*)sTmpAllocationBuffer);

				//CComBSTR strData(sizeof(sTmpAllocationBuffer),(LPCSTR)sTmpAllocationBuffer);

				char sXml[MAX_XML_BUF_LEN] = { 0 };
				_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml(sXml, &pCmdInfo->cardDataAll3);

				CComBSTR strData(sizeof(sXml), (LPCSTR)sXml);

				PAPReadyTransaction(Cmd.GetParameter(),pCmdInfo->lSeqNumber,pCmdInfo->lTrsNumber,pCmdInfo->nNozzle,pCmdInfo->lVolume,
								    pCmdInfo->lValue,pCmdInfo->nLevel,pCmdInfo->lGradePrice,pCmdInfo->nGrade,pCmdInfo->lFlags,strData,pCmdInfo->sRetData,pCmdInfo->pVal);
			}
			break;

			case CMD_GET_PARAM3:
			{

				CMD_STRUCT_GET_PARAM3 *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_GET_PARAM3 *)Cmd.GetDataPtr();	

				GetParam3(pCmdInfo->sParamPosition,pCmdInfo->lKey1,pCmdInfo->lKey2,
						  pCmdInfo->nFiledType,pCmdInfo->sVal,pCmdInfo->lVal,pCmdInfo->bCreate,pCmdInfo->pVal);
			}
			break;

			case CMD_SET_GRADE_PRICE_WITH_TIME: //TD 58094
			{

				strCmdName = "SetGradePriceWithTime";
				CMD_STRUCT_GRADE_PRICE_WITH_TIME *pCmdInfo;
				pCmdInfo = (CMD_STRUCT_GRADE_PRICE_WITH_TIME *)Cmd.GetDataPtr();

				//convert time to BSTR
				BYTE sTmp1[sizeof(TAG_SYSTEMTIME) +2];
				memset(sTmp1,0x00,sizeof(sTmp1));
				BYTE sTmp2[sizeof(TAG_SYSTEMTIME)];
				memset(sTmp2,0x00,sizeof(sTmp2));
				
				memcpy(sTmp2,&pCmdInfo->sDateTime,sizeof(TAG_SYSTEMTIME));
				
				Str2StrNull(sTmp2,sizeof(pCmdInfo->sDateTime),sTmp1);
				CComBSTR strDateTime(min((strlen((const char *)sTmp1)+2),sizeof(sTmp1)),(LPCSTR)sTmp1);

			
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("QueueHandle:SetGradePriceWithTime PumpNumber=%ld , PriceA=%ld" ,pCmdInfo->lPumpNumber , pCmdInfo->lPriceA );
					_LOGMSG.LogMsg(str);
				}		

			
				hRes = pEvent->SetGradePriceWithTime(Cmd.GetParameter(),
												     Cmd.GetSrc(),
												     pCmdInfo->nNozzle,
													 pCmdInfo->nGrade,
												     pCmdInfo->nFlags,
													 pCmdInfo->lPriceA,
													 pCmdInfo->lPriceB,
													 strDateTime,
													&lEventRetCode);
			}
			break;

			case CMD_DISABLE_PD_PANEL://CR 404591
				{
					CMD_STRUCT_DISABLE_ENABLE_PD_PANEL *pCmdInfo;
					_bstr_t tmp;
					pCmdInfo = (CMD_STRUCT_DISABLE_ENABLE_PD_PANEL *)Cmd.GetDataPtr();
					hRes = pEvent->DisablePriceDisplay(pCmdInfo->lDevice
						,pCmdInfo->lFlags
						,Cmd.GetParameter()
						,tmp
						,&lEventRetCode);
				}
			break;
			case CMD_ENABLE_PD_PANEL://CR 404591
				{
					CMD_STRUCT_DISABLE_ENABLE_PD_PANEL *pCmdInfo;
					_bstr_t tmp;
					pCmdInfo = (CMD_STRUCT_DISABLE_ENABLE_PD_PANEL *)Cmd.GetDataPtr();
					hRes = pEvent->EnablePriceDisplay(pCmdInfo->lDevice
						,pCmdInfo->lFlags
						,Cmd.GetParameter()
						,tmp
						,&lEventRetCode);
				}
				break;
			//4.0.19.504 - 47559
			case CMD_RELEASE_CONTROLLER_TRS:
			{
				strCmdName = "TrsReleaseController";
				CMD_STRUCT_RELEASE_CONTROLLER_TRS *pCmdTrs;
				pCmdTrs = (CMD_STRUCT_RELEASE_CONTROLLER_TRS *)Cmd.GetDataPtr();

				hRes = pEvent->SetTrsState(	Cmd.GetParameter(),
											Cmd.GetSrc(),
											pCmdTrs->lTrsNumber,
											STATE_PAID,
											&lEventRetCode);

				CPumpTransact trs;

				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

				if(pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter()-1].GetTrsByNumber(pCmdTrs->lTrsNumber,&trs) == 0)
				{
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("Internal CMD_TRS_PAID : (before seting new state), Trs %d, Status %d, OnOffer %d, link Number %d", 
							trs.m_lNumber,
							trs.m_lStatus,
							trs.m_lOnOffer,
							trs.m_lLinkNumber );
						pInfo->m_pServer->m_cLogger.LogMsg( Cmd.GetParameter(),LOG_PUMP,str ); 
					}
				}

				if (slPumpArray.IsLocked())
					slPumpArray.Unlock();
			}
			
			default:
			//--------------------------------------------------------
			{
				strCmdName = "Unknown";
				bInternal = TRUE;
			}
			break;
		}


		// -------------------------------------------
		//	Summarize and log result of last command.
		// -------------------------------------------
		PosCmdState ePosCmdState	= NACKED;			//4.0.23.507 TD 89047
		long		lLastCmdRetCode = OK;				//4.0.23.507 TD 89047

		if(bInternal)
		{
			lLastCmdRetCode = lRetCode;					//4.0.23.507 TD 89047

			if(lRetCode == GPI_OK)
			{
				ePosCmdState = ACKED;
			}
			else
			{
				ePosCmdState = NACKED;

				if(LOG_LOGIC_ERROR)
				{
					pInfo->m_pServer->m_cLogger.LogResult(	Cmd.GetSrc(), // Src
															"Internal",
															(LPCSTR)strCmdName,
															TRUE,
															lLastCmdRetCode,
															FALSE,
															0,
															Cmd.GetParameter());
				}								
			}
		}
		else // not internal 
		{					
			if(lRetCode == GPI_OK)
			{
				lLastCmdRetCode = lEventRetCode;		//4.0.23.507 TD 89047

				if(SUCCEEDED(hRes))
				{					
					ePosCmdState	= SENT;

					iCmdToOpRetriesCount = 0;

					if(LOG_LOGIC_ERROR)
					{
						if(lEventRetCode)
							pInfo->m_pServer->m_cLogger.LogResult(	Cmd.GetSrc(),
																	"IGCIEvents",
																	(LPCSTR)strCmdName,
																	TRUE,
																	lLastCmdRetCode,
																	FALSE,
																	0,
																	Cmd.GetParameter());
					}
				}
				else
				{
					lQueueHandleRetCode = GCI_CONTROLLER_NOT_AVAILABLE;			//4.0.23.507 TD 89047
					ePosCmdState		= NOT_SENT;

					if ( ++iCmdToOpRetriesCount > iMaxCmdToOpRetries )
						iCmdToOpRetriesCount =  1;

					if (Cmd.GetCommand() == CMD_WATCHDOG)			//4.0.23.510 TD 97894
					{
						_Module.m_server.SetShowGPIMsgCLIsGone();
					}

					CString strLogMsg;
					strLogMsg.Format("%s %d", strCmdName, iCmdToOpRetriesCount);
					
					if(LOG_ERROR)
					{
						
						pInfo->m_pServer->m_cLogger.LogResult(	0,
																"IGCIEvents",
																(LPCSTR)strLogMsg,
																FALSE,
																0,
																TRUE,
																hRes);
					}
				}
			}
			else // lRetCode (GPI) not ok and not internal
			{
				lLastCmdRetCode	= lRetCode;						//4.0.23.507 TD 89047
				ePosCmdState	= NACKED;

				if(LOG_LOGIC_ERROR)
				{
					pInfo->m_pServer->m_cLogger.LogResult(	Cmd.GetSrc(), // Src
															"IGCIEvents",
															(LPCSTR)strCmdName,
															TRUE,
															lLastCmdRetCode,
															FALSE,
															0,
															Cmd.GetParameter());
				}
			}
		}

		pInfo->m_pServer->UpdateLastCmd(
										Cmd.GetSrc(),
										Cmd.GetCommand(),
										Cmd.GetNumber(),
										ePosCmdState,
										lLastCmdRetCode);

		if ( ePosCmdState != NOT_SENT || (iCmdToOpRetriesCount >= iMaxCmdToOpRetries && Cmd.GetCommand() == CMD_WATCHDOG) )
		{
			if ( ePosCmdState == NOT_SENT )
			{
				CString	strLogMsg;
				strLogMsg.Format("QueueHandle: RemoveWatchdogCmdFromQ after %d retries.", iCmdToOpRetriesCount);
				_LOGMSG.LogMsg(strLogMsg);
			}

			pInfo->m_pServer->RemoveCmdFromQ();			
		}
	}
	else // pEvent == NULL
	{							
		lQueueHandleRetCode = GCI_CONTROLLER_NOT_AVAILABLE;			//4.0.23.507 TD 89047
	}

	return lQueueHandleRetCode;
}

long CGCI::StartCmdTxThread()
{

	SetEvent(m_CmdTxThreadInfo.m_hEventStartThread);
	GetLocalTime(&_Module.m_server.m_tConnectionTimeOut);
	return 0;
	
}

long CGCI::StopCmdTxThread()
{
	SetEvent(m_CmdTxThreadInfo.m_hEventStopThread);	

	_Module.m_server.SetServerState(SERVER_STARTUP);
	
	//4.0.7.501 - sending "no convert later" off alarm
	if(_ALARM_SRV.m_lAlarmSrvActive)//4.0.6.507
		_Module.m_server.m_cAlarm.ServerStateAlarm(SERVER_IDLE,SERVER_NO_CONVERT);
	
	return 0;
}



IGCIEvents * CGCI::GetEventPtr()
{
	IUnknown **ppUnk = m_vec.begin();
		
	if(ppUnk != NULL)
	{
		return ((IGCIEvents *) *ppUnk);		
	}
	else 
		return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// ThreadInfo 


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CGCI::ConstructThreadInfo()
{
	m_CmdTxThreadInfo.m_hEventStartThread = CreateEvent(NULL, FALSE, FALSE, NULL); // auto reset, initially reset
	m_CmdTxThreadInfo.m_hEventStopThread = CreateEvent(NULL, FALSE, FALSE, NULL); // auto reset, initially reset
	m_CmdTxThreadInfo.m_hEventKillThread	= CreateEvent(NULL, FALSE, FALSE, NULL); // auto reset, initially reset
	m_CmdTxThreadInfo.m_hEventThreadKilled = CreateEvent(NULL, FALSE, FALSE, NULL); // auto reset, initially reset
	m_CmdTxThreadInfo.m_pGCI = NULL;
}
void CGCI::DeleteThreadInfo()
{
	// Kill the worker thread by setting the "kill thread" event.
	SetEvent(m_CmdTxThreadInfo.m_hEventKillThread);
	SetEvent(m_CmdTxThreadInfo.m_hEventStartThread); 
	WaitForSingleObject(m_CmdTxThreadInfo.m_hEventThreadKilled, INFINITE);
	
	CloseHandle(m_CmdTxThreadInfo.m_hEventStartThread);
	CloseHandle(m_CmdTxThreadInfo.m_hEventStopThread);
	CloseHandle(m_CmdTxThreadInfo.m_hEventKillThread);
	CloseHandle(m_CmdTxThreadInfo.m_hEventThreadKilled);
}


eFuelLoyaltyState CGCI::GetLoyaltyState(CARD_SALE_ALL3 * cardDataAll3,long lFlags,long lPumpNumber)
{//4.0.21.501
	//check if the system supports loyalty
	//if so we check if we in the first swipe and from need more param if we in first time we call getcardinfo to check if we work on loyalty
	eFuelLoyaltyState eRetCode = No_Loyalty;
	BOOL			  bRetCode = FALSE;
	CComBSTR		  sRetData;
	CString			  str;
	CPumpTransact	cTrs;
	PumpReservation eResStat;
	PumpResExt ePumpResExt;  
	PumpStatus ePumpStat;

	PAY_AT_PUMP_INFO  cPayAtPumpInfo;
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cPayAtPumpInfo);
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetCurrentTrs(&cTrs);
	ePumpStat = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetStatus();
	eResStat = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetRes();
	ePumpResExt = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetResExt(); 


	if(_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())
	{	
		str.Format("CheckFirstSwipe, flags= %0x ", lPumpNumber, lFlags); 
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);

		{
			if(_Module.m_server.CheckOLA() != OLA_SERVER_ON)
			{
				str.Format("CheckFirstSwipe, Ola Is Off continue with regular flow without loyalty"); 
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
			}
			else if (cardDataAll3->CardSaleAll.data.sTranType == CD_TYPE_VOUCHER)	//first empty swipe when pre paid voucher button is pressed.
			{
				bRetCode = TRUE;
				eRetCode = PAP_Prepaid_Voucher;
			}
			else if ((ChrAll((char*)cardDataAll3->CardSaleAll.data.sTrack1Data, sizeof(cardDataAll3->CardSaleAll.data.sTrack1Data,' '))) &&
					((ChrAll((char*)cardDataAll3->CardSaleAll.data.sTrack2Data, sizeof(cardDataAll3->CardSaleAll.data.sTrack2Data,' '))) && 
					(cardDataAll3->CardSaleAll.data.sEntryMethod == ' ')) &&	//4.0.22.504 69344
					(cardDataAll3->extData4.sLoyaltyEntryMethod == ' '))
			{
				bRetCode = TRUE;
				eRetCode = PAK_No_Card;
			}
			else
			{
				str.Format("Pump %02d GetLoyaltyState, Fuel loyalty GetCardInfo", lPumpNumber); 
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

				PAY_AT_PUMP_INFO  cLoyaltyResultPapInfo;
				memset(&cLoyaltyResultPapInfo, ' ', sizeof(cLoyaltyResultPapInfo));	//4.0.19.0 //4.0.20.440 68857
				cLoyaltyResultPapInfo.CopyCardSaleAll3(cardDataAll3);// coverity
				//memcpy(&cTmpCardSaleData.CardSaleInfo.cardData, cardDataAll3, sizeof(CARD_SALE_ALL3));

				BOOL bLoyaltyAndPaymentTogether = FALSE;
				if (cardDataAll3->extData4.sLoyaltyEntryMethod == 'B')	//4.0.22.504 69344
				{
					cLoyaltyResultPapInfo.CardSaleInfo.cardData.sEntryMethod = 'B';
					long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, NULL, &cLoyaltyResultPapInfo, TRUE, FALSE); //4.0.20.450 68857	//4.0.22.504 69344 //4.0.24.6

					memcpy(cardDataAll3->CardSaleAll.extData.sLoyalId, cLoyaltyResultPapInfo.CardSaleInfo.extraData.sLoyalId,sizeof(cardDataAll3->CardSaleAll.extData.sLoyalId));
					bLoyaltyAndPaymentTogether = TRUE;
				}

				else if ((cardDataAll3->CardSaleAll.data.sEntryMethod == 'S') && (_Module.m_server.m_cFuelLoayaltyPS.IsPaymentFirstInFlow()))	//4.0.22.511 76482
				{
					//clear sClubCardTrack2Data when payment is expected.
					memset(cPayAtPumpInfo.CardSaleInfo.cardData.sClubCardTrack2Data, ' ', sizeof(cPayAtPumpInfo.CardSaleInfo.cardData.sClubCardTrack2Data));
					memset(cLoyaltyResultPapInfo.CardSaleInfo.cardData.sClubCardTrack2Data, ' ', sizeof(cLoyaltyResultPapInfo.CardSaleInfo.cardData.sClubCardTrack2Data));
					_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cPayAtPumpInfo);	
				}

				long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, NULL, &cLoyaltyResultPapInfo, TRUE, FALSE); //4.0.20.450 68857	//4.0.22.504 69344 //4.0.24.60

				str.Format("GetLoyaltyState() - MediaMasterType = %ld", lMediaMasterType);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

				memcpy(&cPayAtPumpInfo.CardSaleInfo.cardData.sTranType, &cardDataAll3->CardSaleAll.data.sTranType,sizeof(cPayAtPumpInfo.CardSaleInfo.cardData.sTranType)); //4.0.20.450 66977 // RFUEL-1384
			
				SavePapInfoFromLoyaltyResult(cPayAtPumpInfo, cLoyaltyResultPapInfo,lMediaMasterType);

				if ((eResStat != PAY_AT_KIOSK) && (ePumpResExt != RES_EXT_PAK_LOYALTY))	//Not PAK
				{
					if ((lMediaMasterType != CARD_TYPE_LOYALTY) && 
						(lMediaMasterType != CARD_TYPE_TRIPLE_A))	//4.0.23.266?//||(lMasterM==CARD_TYPE_LOYALTY_AND_CREDIT))
					{
						if (cardDataAll3->CardSaleAll.data.sEntryMethod == 'B')//4.0.20.440 68857
						{
							str.Format("CheckFirstSwipe, PAP MediaMasterType = %ld was scanned", lMediaMasterType);
							_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
							bRetCode = TRUE;
							eRetCode = PAP_Payment_Scanned;
						}
						else
						{
							str.Format("CheckFirstSwipe, Start Session MediaMasterType = %ld", lMediaMasterType);
							_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
							bRetCode = TRUE;
							eRetCode = PAP_Payment_Swiped;//4.0.22.500 70767
							//memcpy(cTmpInfo.CardSaleInfo.cardData.sAccountNumber,"",sizeof(cTmpInfo.CardSaleInfo.cardData.sAccountNumber));
						}

						if(!(ChrAll((char*) cardDataAll3->CardSaleAll.extData.sLoyalId, sizeof(cardDataAll3->CardSaleAll.extData.sLoyalId,' '))))	//First time loyalty card was swiped
							memcpy(cPayAtPumpInfo.CardSaleInfo.extraData.sLoyalId, cardDataAll3->CardSaleAll.extData.sLoyalId, 
								sizeof(cardDataAll3->CardSaleAll.extData.sLoyalId)); //4.0.20.480 70932 //4.0.21.502
					}
					else	//PAP Loyalty 
					{
						if ((ChrAll((char*)cardDataAll3->CardSaleAll.extData.sLoyalId, sizeof(cardDataAll3->CardSaleAll.extData.sLoyalId, ' '))))	//First time loyalty card was swiped
						{
							memset(cPayAtPumpInfo.CardSaleExtraData2.sMediaMasterType, ' ', sizeof(cPayAtPumpInfo.CardSaleExtraData2.sMediaMasterType)); //4.0.20.440 change only the sizeof 68857
						}
					
						if (_Module.m_server.m_cFuelLoayaltyPS.IsEpslionLoyaltyEnginelInUse())
						{
							memcpy(cPayAtPumpInfo.CardSaleInfo.cardData.sClubCardTrack2Data, cPayAtPumpInfo.CardSaleInfo.cardData.sTrack2Data, sizeof cPayAtPumpInfo.CardSaleInfo.cardData.sTrack2Data);
						}
						else
						{
							 //4.0.20.440 68857 TBD need to check if the loyalId is in the accountNumber as before or not
							memcpy(cPayAtPumpInfo.CardSaleInfo.extraData.sLoyalId, cLoyaltyResultPapInfo.CardSaleInfo.extraData.sLoyalId, //4.0.20.490 70932 //4.0.21.502
							 sizeof(cLoyaltyResultPapInfo.CardSaleInfo.extraData.sLoyalId)); //4.0.20.440 68857 change only the sizeof 

																						// 
																						// 						str.Format("CheckFirstSwipe, Session Not Start MediaMasterType = %ld , sLoyalId = %.20s", lMediaMasterType, cTmpCardSaleData.CardSaleInfo.extraData.sLoyalId);

						}
						



						_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
						if (_Module.m_server.m_cFuelLoayaltyPS.IsPaymentFirstInFlow())
						{
							eRetCode = PAP_PaymentFirst_Loyalty_swiped; //4.0.22.504 69344	trs should be canceled
						}
						else
						{
							_Module.m_server.CheckTripleAFlow(lPumpNumber, lMediaMasterType); // TD 327153
							eRetCode = PAP_Loyalty_Swiped; //4.0.22.500 70767
						}
					}
				}
				else	//PAK
				{
					if(lMediaMasterType == CARD_TYPE_LOYALTY)
					{
						str.Format("CheckFirstSwipe, Start Session MediaMasterType = %ld", lMediaMasterType); 
						_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
						bRetCode = TRUE;
						eRetCode = PAK_Loyalty_Swiped; //4.0.22.500 70767
						memcpy(cPayAtPumpInfo.CardSaleInfo.extraData.sLoyalId, cLoyaltyResultPapInfo.CardSaleInfo.extraData.sLoyalId, //4.0.20.490 70932 //4.0.21.502
							sizeof(cLoyaltyResultPapInfo.CardSaleInfo.extraData.sLoyalId)); //4.0.20.440 68857 change only the sizeof 
					}
					else
					{
						eRetCode = PAK_Payment_Swiped;	//4.0.22.500 70767
					}
				}
			}
			memset(&cPayAtPumpInfo.CardSaleExtraData4.loyaltyOnOff, '0', sizeof(cPayAtPumpInfo.CardSaleExtraData4.loyaltyOnOff));	//Means ok -Initialize LPE status to working
		}
		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cPayAtPumpInfo);
	}
	else
	{
		str.Format("CheckFirstSwipe, Loyalty flag is off", lPumpNumber); 
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
	}

	str.Format("Pump %d CheckIfLoyaltyFuel return value = %d", lPumpNumber, eRetCode);
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);

	return eRetCode;
}


void CGCI::SavePapInfoFromLoyaltyResult(PAY_AT_PUMP_INFO &cTmpInfo, PAY_AT_PUMP_INFO &cTmpCardSaleData,long lMediaMasterType)
{
	memcpy(cTmpInfo.CardSaleExtraData2.sMediaMasterType, cTmpCardSaleData.CardSaleExtraData2.sMediaMasterType,
		sizeof(cTmpCardSaleData.CardSaleExtraData2.sMediaMasterType));

	memcpy(cTmpInfo.CardSaleExtraData4.sPreAuthAmount, cTmpCardSaleData.CardSaleExtraData4.sPreAuthAmount,
		sizeof(cTmpInfo.CardSaleExtraData4.sPreAuthAmount));

	memcpy(cTmpInfo.CardSaleInfo.cardData.sAccountNumber, cTmpCardSaleData.CardSaleInfo.cardData.sAccountNumber,
		sizeof(cTmpInfo.CardSaleInfo.cardData.sAccountNumber));

	memcpy(&cTmpInfo.CardSaleExtraData3.sCardSegment, &cTmpCardSaleData.CardSaleExtraData3.sCardSegment,
		sizeof(cTmpInfo.CardSaleExtraData3.sCardSegment));

	memcpy(cTmpInfo.CardSaleExtraData6.sClubCardHashNumber, cTmpCardSaleData.CardSaleExtraData6.sClubCardHashNumber,		// 4.0.32.??? TD 405435
		sizeof(cTmpInfo.CardSaleExtraData6.sClubCardHashNumber));
	
	//MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
	if (lMediaMasterType == CARD_TYPE_LOYALTY) // RFUEL-2395
	{
		memcpy(&cTmpInfo.CardSaleExtraData6.LoyaltyCard.sClubNumber, cTmpCardSaleData.CardSaleInfo.extraData.sLoyalId,
			sizeof(cTmpInfo.CardSaleExtraData6.LoyaltyCard.sClubNumber));

		memcpy(&cTmpInfo.CardSaleExtraData6.LoyaltyCard.sAccountNumber, cTmpCardSaleData.CardSaleInfo.cardData.sAccountNumber,
			sizeof(cTmpInfo.CardSaleExtraData6.LoyaltyCard.sAccountNumber));

		memcpy(&cTmpInfo.CardSaleExtraData6.LoyaltyCard.sProgramLongName, cTmpCardSaleData.CardSaleInfo.cardData.sFullCardName,
			sizeof(cTmpInfo.CardSaleExtraData6.LoyaltyCard.sProgramLongName));

		memcpy(&cTmpInfo.CardSaleExtraData6.LoyaltyCard.sProgramShortName, cTmpCardSaleData.CardSaleInfo.cardData.sCardName,
			sizeof(cTmpInfo.CardSaleExtraData6.LoyaltyCard.sProgramShortName));

		memcpy(&cTmpInfo.CardSaleExtraData6.LoyaltyCard.sCardType, cTmpCardSaleData.CardSaleExtraData6.LoyaltyCard.sCardType,
			sizeof(cTmpInfo.CardSaleExtraData6.LoyaltyCard.sCardType));
	}
}

BOOL CGCI::CheckFirstManualCard(CARD_SALE_ALL3 * cardDataAll3, long lFlags, long lPumpNumber) //4.0.22.506 67786
{//4.0.21.501
	BOOL			  bRetCode = FALSE;
	CString			  str;

	str.Format("Start checking first manual card, flags= 0x%x ", lFlags); 
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);

	if ((cardDataAll3->CardSaleAll.data.sEntryMethod == 'M') || (cardDataAll3->extData4.sLoyaltyEntryMethod != 'A') && (cardDataAll3->extData4.sLoyaltyEntryMethod != ' '))
	{
		PAY_AT_PUMP_INFO sTmpPapInfo;
		sTmpPapInfo.CopyCardSaleAll3(cardDataAll3); // // coverity
		//memcpy(&sTmpPapInfo.CardSaleInfo.cardData, cardDataAll3, sizeof(CARD_SALE_ALL3));
		
		long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, NULL, &sTmpPapInfo, TRUE, FALSE); //4.0.20.450 68857	//4.0.22.504 69344	//4.0.24.60
		memcpy(cardDataAll3->CardSaleAll.extData.sLoyalId, sTmpPapInfo.CardSaleInfo.extraData.sLoyalId, sizeof(cardDataAll3->CardSaleAll.extData.sLoyalId));
		l2a(lMediaMasterType, cardDataAll3->extData2.sMediaMasterType, sizeof(cardDataAll3->extData2.sMediaMasterType));	
		bRetCode = TRUE;
	}

	return bRetCode;
}

/******************************************************************************
 Description: Finding the account number out of the data sent from Convert Layer. 
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			02/04/08	15:38		Start 4.0.19.508
 Liat			05/06/08	10:00		Update 
******************************************************************************/
//4.0.24.60
long CGCI::HandleLoyaltyResult(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo, BOOL bIgnoreParamResult, BOOL bCheckLoyaltyCard) //4.0.20.440 68857	4.0.22.504
{
	CString			str;
	BOOL			bSkipParsingOLA = FALSE;
	CComBSTR		sRetData;
	long			lFlags = 0;
	long			lSessionType = 0 ;
	long			lMediaMasterType = 0;	//4.0.22.504 69344
	int				iParamResultSize = 0;		//4.0.28.502 255727 
	BOOL            bDoNotUseXmlBuffer = TRUE; //4.0.25.0
	BYTE			sTmpParamResult[(sizeof(pInfo->CardSaleExtraData3.sParamResult) + sizeof(pInfo->CardSaleExtraData5.sParamResultExt)) + 1] = {0}; //4.0.28.502 255727
	long			lRet = 0;
	long			lClientId = 99;

	
	//4.0.28.502 255727 - start
	memcpy((char *)sTmpParamResult, pInfo->CardSaleExtraData3.sParamResult, sizeof(pInfo->CardSaleExtraData3.sParamResult));
	memcpy(&sTmpParamResult[sizeof(pInfo->CardSaleExtraData3.sParamResult)], pInfo->CardSaleExtraData5.sParamResultExt, sizeof(pInfo->CardSaleExtraData5.sParamResultExt));

	// Debug
	//memcpy(&sTmpParamResult,"B706988500091799447^                          ^591213600900010776?0         |706988500091799447=59121360090001077",
	//sizeof("B706988500091799447^                          ^591213600900010776?0         |706988500091799447=59121360090001077"));
	
	/*pInfo->CardSaleInfo.cardData.sEntryMethod = 'B';

	memcpy(&pInfo->CardSaleExtraData3.sParamResult, "B706988500091799447^                          ^591213600900010776?0         |706988500091799447=59121360090001077",
			sizeof("B706988500091799447^                          ^591213600900010776?0         |706988500091799447=59121360090001077"));
*/

	
	
	CString sTrack((LPCSTR)sTmpParamResult, sizeof(sTmpParamResult));															
	iParamResultSize = sTrack.GetLength();

	if(pInfo->CardSaleInfo.cardData.sTranType == CD_TYPE_PAYPAL)
	{
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Starting PayPal Flow");
		return 0;
	} // 339257


// 	str.Format("Debug: ParamResult = %.113s", pInfo->CardSaleExtraData3.sParamResult);
// 	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
// 
// 
// 	str.Format("Debug: sLoyalId = %.20s", pInfo->CardSaleInfo.extraData.sLoyalId);
// 	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);


	if (IsBlankBuffer((char *)pInfo->CardSaleExtraData3.sParamResult, sizeof pInfo->CardSaleExtraData3.sParamResult) && IsBlankBuffer((char *)pInfo->CardSaleInfo.extraData.sLoyalId, sizeof pInfo->CardSaleInfo.extraData.sLoyalId) && (!bIgnoreParamResult)) //4.0.20.440 68857
	{
		str.Format("HandleLoyaltyResult, sParamResult is empty.");
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
	}
	else
	{
		str.Format("HandleLoyaltyResult, EntryMethod = %.1s, sLoyaltyEntryMethod = %.1s ,IgnoreParamResult = %d, CheckLoyaltyCard = %d, sParamResult = %c, LoyaltyClubNumber=%ld",
			&pInfo->CardSaleInfo.cardData.sEntryMethod,
			&pInfo->CardSaleExtraData4.sLoyaltyEntryMethod,
			bIgnoreParamResult,
			bCheckLoyaltyCard,
			pInfo->CardSaleExtraData3.sParamResult[0],
			_Module.m_server.m_dwLoyaltyClubNumber );
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

		// 4.0.32.391 start
		BYTE	byTrack1[TRACK_1_DATA_SIZE];
		BYTE	byTrack2[TRACK_2_DATA_SIZE];
		memset(byTrack1, ' ', sizeof(byTrack1));  //4.0.23.640 //4.0.25.50 136530
		memset(byTrack2, ' ', sizeof(byTrack2));  //4.0.23.640 //4.0.25.50 136530
		// 4.0.32.391 end

		const  bool bPaymentFiniteState = (pOla && OLA_STATE(pOla->m_byState) == OLA_LOYALTY_SWIPE_PAYMENT);

		if (pInfo->CardSaleExtraData4.sLoyaltyEntryMethod != ' ' && !bPaymentFiniteState)
		{
			if (pInfo->CardSaleExtraData4.sLoyaltyEntryMethod == 'B')
			{
				HandleLoyaltyResultCopyLoyaltyData(pInfo, lMediaMasterType, lPumpNumber);
				bSkipParsingOLA = TRUE;
			}
			else if (pInfo->CardSaleExtraData4.sLoyaltyEntryMethod == 'A')
			{
				const bool bSendCardInfo = pOla &&
					((OLA_STATE(pOla->m_byState) == OLA_LOYALTY_ALTERNATE_ID_SWIPE_LOYALTY_CARD));

				if (bSendCardInfo) // customer swipe a card need to check if it's loyalty
				{
					CComBSTR sAccountNumber("");
					CComBSTR sShortCardNumber("");
					long lRtc = 0;
					
					pInfo->CardSaleInfo.cardData.sEntryMethod = 'S';
					long iPos = sTrack.Find('|');

					if (iPos != -1 && iPos < iParamResultSize)
					{
						memcpy(byTrack1, (char *)sTmpParamResult, min(iPos, TRACK_1_DATA_SIZE));								//4.0.28.502 255727										
						memcpy(byTrack2, (char *)&sTmpParamResult[iPos + 1], min(iParamResultSize - iPos, TRACK_2_DATA_SIZE));	//4.0.28.502 255727				
					}
					else
					{
						memcpy(byTrack2, (char *)&pInfo->CardSaleExtraData3.sParamResult, TRACK_2_DATA_SIZE);
					}

					if (bCheckLoyaltyCard)
						memcpy(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data, byTrack2, sizeof(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data));
					else
					{
						memcpy(pInfo->CardSaleInfo.cardData.sTrack2Data, byTrack2, sizeof(pInfo->CardSaleInfo.cardData.sTrack2Data));
						memcpy(pInfo->CardSaleInfo.cardData.sTrack1Data, byTrack1, sizeof(pInfo->CardSaleInfo.cardData.sTrack1Data));//4.0.23.330 115613 //4.0.24.90
					}

					memset(pInfo->CardSaleExtraData3.sPromptMaxLen, ' ', sizeof(pInfo->CardSaleExtraData3.sPromptMaxLen)); //4.0.28.503 265566 
					memset(pInfo->CardSaleExtraData3.sPromptMinLen, ' ', sizeof(pInfo->CardSaleExtraData3.sPromptMinLen)); //4.0.28.503 265566 
					pInfo->CardSaleExtraData6.sIdRetryCount = ' ';
					_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(pInfo);

					// 4.0.32.391 start
					BYTE sTmpTrack1[TRACK_1_DATA_SIZE + 2];
					Str2StrNull((BYTE *)byTrack1, TRACK_1_DATA_SIZE, sTmpTrack1);
					CComBSTR sTrack1(sizeof(sTmpTrack1), (LPCSTR)sTmpTrack1);

					BYTE sTmpTrack2[TRACK_2_DATA_SIZE + 2];
					Str2StrNull((BYTE *)byTrack2, TRACK_2_DATA_SIZE, sTmpTrack2);
					CComBSTR sTrack2(sizeof(sTmpTrack2), (LPCSTR)sTmpTrack2);
					if (!strlen((char *)sTmpTrack2))
					{
						str.Format(" Pump %02d, Input Track2 is empty", lPumpNumber);
						_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
					}
					// 4.0.32.391 end

					//RFUEL-2817
					if (_Module.m_server.GetOLAInUse() == OLASRV_EPSILON ||
						_Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT
						)
						_Module.m_server.GetCardInfoEx(lPumpNumber, 99, sTmpTrack1, sTmpTrack2, pInfo, lFlags, &sRetData, &lRet);
					else
						lRtc = _Module.m_server.GetCardInfo(sAccountNumber,
							sTrack1,
							sTrack2,
							sShortCardNumber,
							&sRetData,
							lFlags,
							&lSessionType,
							lPumpNumber,
							bDoNotUseXmlBuffer);

				}
				else
				{
					if (pInfo->CardSaleExtraData3.sParamResult[0] != 'N' && pInfo->CardSaleExtraData3.sParamResult[0] != 'Y' && pInfo->CardSaleExtraData3.sParamResult[0] != 'A')
					{
						memset(pInfo->CardSaleInfo.extraData.sLoyalId, ' ', sizeof(pInfo->CardSaleInfo.extraData.sLoyalId));
						memcpy(pInfo->CardSaleInfo.extraData.sLoyalId, pInfo->CardSaleExtraData3.sParamResult, sizeof(pInfo->CardSaleInfo.extraData.sLoyalId));
						HandleLoyaltyResultCopyLoyaltyData(pInfo, lMediaMasterType, lPumpNumber);
					}

					bSkipParsingOLA = TRUE;
				}
			}
		}
		else 
		{		
			switch (pInfo->CardSaleInfo.cardData.sEntryMethod)
			{
			case 'S':	//Swipe
			case 'C':	//EMV
			case 'T':	//tap
			case 'H':	//tap
			case 'W':
				{

					CComBSTR sAccountNumber("");
					CComBSTR sShortCardNumber("");
					long lRtc = 0;
					if (FALSE == bIgnoreParamResult && pInfo->CardSaleInfo.cardData.sEntryMethod == 'S')	//4.0.20.440 68857 // TD 460382 //  TD 464915
					{
						long iPos = sTrack.Find('|');
					
						if(iPos != -1 && iPos < iParamResultSize)
						{
							memcpy(byTrack1, (char *)sTmpParamResult, min(iPos, TRACK_1_DATA_SIZE));								//4.0.28.502 255727										
							memcpy(byTrack2, (char *)&sTmpParamResult[iPos +1], min (iParamResultSize - iPos, TRACK_2_DATA_SIZE));	//4.0.28.502 255727				
						}
						else
						{
							memcpy(byTrack2, (char *)&pInfo->CardSaleExtraData3.sParamResult, TRACK_2_DATA_SIZE);
						}

						if (bCheckLoyaltyCard)
							memcpy(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data, byTrack2, sizeof(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data));
						else
						{
							memcpy(pInfo->CardSaleInfo.cardData.sTrack2Data, byTrack2, sizeof(pInfo->CardSaleInfo.cardData.sTrack2Data));
							memcpy(pInfo->CardSaleInfo.cardData.sTrack1Data, byTrack1, sizeof(pInfo->CardSaleInfo.cardData.sTrack1Data));//4.0.23.330 115613 //4.0.24.90
						}
					
						memset(pInfo->CardSaleExtraData3.sPromptMaxLen, ' ', sizeof(pInfo->CardSaleExtraData3.sPromptMaxLen)); //4.0.28.503 265566 
						memset(pInfo->CardSaleExtraData3.sPromptMinLen, ' ', sizeof(pInfo->CardSaleExtraData3.sPromptMinLen)); //4.0.28.503 265566 
						pInfo->CardSaleExtraData6.sIdRetryCount = ' ';
						_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(pInfo);

						// 4.0.32.391 start
						BYTE sTmpTrack1[TRACK_1_DATA_SIZE + 2];
						Str2StrNull((BYTE *)byTrack1, TRACK_1_DATA_SIZE, sTmpTrack1);
						CComBSTR sTrack1(sizeof(sTmpTrack1), (LPCSTR)sTmpTrack1);

						BYTE sTmpTrack2[TRACK_2_DATA_SIZE + 2];
						Str2StrNull((BYTE *)byTrack2, TRACK_2_DATA_SIZE, sTmpTrack2);
						CComBSTR sTrack2(sizeof(sTmpTrack2), (LPCSTR) sTmpTrack2);
						if (!strlen((char *) sTmpTrack2))
						{
							str.Format(" Pump %02d, Input Track2 is empty", lPumpNumber); 
							_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
						}

						//RFUEL-2817
						if (_Module.m_server.GetOLAInUse() == OLASRV_EPSILON || 
							_Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT)
							_Module.m_server.GetCardInfoEx(lPumpNumber, lClientId, sTmpTrack1, sTmpTrack2, pInfo, lFlags, &sRetData, &lRet);
						else
							lRtc = _Module.m_server.GetCardInfo(sAccountNumber,
															sTrack1,
															sTrack2,
															sShortCardNumber,
															&sRetData,
															lFlags,
															&lSessionType,
															lPumpNumber,
															bDoNotUseXmlBuffer);

					}
					else
					{
						BYTE sTmpTrack1[TRACK_1_DATA_SIZE + 2];
						Str2StrNull((BYTE *)pInfo->CardSaleInfo.cardData.sTrack1Data, TRACK_1_DATA_SIZE, sTmpTrack1);
						CComBSTR sTrack1(sizeof(sTmpTrack1), (LPCSTR)sTmpTrack1);

						BYTE sTmpTrack2[TRACK_2_DATA_SIZE + 2];
						Str2StrNull((BYTE *)pInfo->CardSaleInfo.cardData.sTrack2Data, TRACK_2_DATA_SIZE, sTmpTrack2);
						CComBSTR sTrack2(sizeof(sTmpTrack2), (LPCSTR)sTmpTrack2);
						if (!strlen((char *)sTmpTrack2))
						{
							str.Format("Pump %02d, Input Track2 is empty", lPumpNumber);
							_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
						}

						//RFUEL-2817
						if (_Module.m_server.GetOLAInUse() == OLASRV_EPSILON || 
							_Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT)
							_Module.m_server.GetCardInfoEx(lPumpNumber, lClientId, sTmpTrack1, sTmpTrack2, pInfo, lFlags, &sRetData, &lRet);
						else
							lRtc = _Module.m_server.GetCardInfo(sAccountNumber,
							sTrack1,
							sTrack2,
							sShortCardNumber,
							&sRetData,
							lFlags,
							&lSessionType,
							lPumpNumber,
							bDoNotUseXmlBuffer); //4.0.25.0
					}
				}
				break;
			case 'B':	//Scan
				{
					if (!bIgnoreParamResult) //4.0.20.440 68857
					{
						memcpy(byTrack2, (char *)&pInfo->CardSaleExtraData3.sParamResult, TRACK_2_DATA_SIZE);
						memcpy(pInfo->CardSaleExtraData2.sBarCode,pInfo->CardSaleExtraData3.sParamResult, sizeof(pInfo->CardSaleExtraData2.sBarCode));
						memcpy(pInfo->CardSaleInfo.cardData.sAccountNumber, pInfo->CardSaleExtraData3.sParamResult, sizeof(pInfo->CardSaleInfo.cardData.sAccountNumber));  //RFUEL-2399
						_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(pInfo);
					}
					
					//RFUEL-2399
	//				memcpy(pInfo->CardSaleInfo.extraData.sLoyalId, pInfo->CardSaleExtraData3.sParamResult, sizeof(pInfo->CardSaleInfo.extraData.sLoyalId));
					
					CComBSTR sTrack1("");
					CComBSTR sTrack2("");	
					CComBSTR sAccountNumber("");
					CComBSTR sShortCardNumber("");

					_Module.m_server.MatchSentLoyaltyCardInfoToOLA(*pInfo); //4.0.23.507 89548
					_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(pInfo); //4.0.23.507 89548
	
					BYTE sTmpTrack1[TRACK_1_DATA_SIZE + 2];
					BYTE sTmpAccountNumber[ACCOUNT_NUM_SIZE + 2];
					memset(sTmpTrack1, ' ', sizeof(sTmpTrack1));
					memset(sTmpAccountNumber, ' ', sizeof(sTmpAccountNumber));

					if (IsBlankBuffer((char *)pInfo->CardSaleExtraData3.sParamResult, sizeof pInfo->CardSaleExtraData3.sParamResult))
					{
						Str2StrNull((BYTE *)pInfo->CardSaleExtraData2.sBarCode, ACCOUNT_NUM_SIZE, sTmpAccountNumber);
					}
					else
					{
						Str2StrNull((BYTE *)pInfo->CardSaleExtraData3.sParamResult, ACCOUNT_NUM_SIZE, sTmpAccountNumber);
					}
				
					if (_Module.m_server.GetOLAInUse() == OLASRV_EPSILON ||
						_Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT) //RFUEL-2399
						_Module.m_server.GetCardInfoEx(lPumpNumber, lClientId, sTmpTrack1, sTmpAccountNumber, pInfo, lFlags, &sRetData, &lRet);
				
					else
					{
						long lRtc = _Module.m_server.GetCardInfo(sAccountNumber,
																sTrack1,
																sTrack2,
																sShortCardNumber,
																&sRetData,
																lFlags,
																&lSessionType,
																lPumpNumber,
																bDoNotUseXmlBuffer); //4.0.25.0
					}
				}
				break;
			case 'M':	//Manual
				{
					if (!bIgnoreParamResult) //4.0.20.440 68857
					{
						memcpy(byTrack2, (char *)&pInfo->CardSaleExtraData3.sParamResult, TRACK_2_DATA_SIZE);
						memcpy(pInfo->CardSaleInfo.extraData.sLoyalId, pInfo->CardSaleExtraData3.sParamResult, sizeof(pInfo->CardSaleInfo.extraData.sLoyalId));
						memcpy(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data, pInfo->CardSaleInfo.extraData.sLoyalId, 20);
						_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(pInfo);
					}

					if (pInfo->CardSaleExtraData4.sLoyaltyEntryMethod == 'A')	//loyaltyEntryMethod = 'A' then copy from sLoyalId to clubCardData
						memcpy(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data, pInfo->CardSaleInfo.extraData.sLoyalId, 20);

					CComBSTR sTrack1("");
					CComBSTR sTrack2("");	
					CComBSTR sAccountNumber("");
					CComBSTR sShortCardNumber("");

					_Module.m_server.MatchSentLoyaltyCardInfoToOLA(*pInfo); //4.0.23.507 89548
					_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(pInfo); //4.0.23.507 89548
				

					//RFUEL-2817
					if (_Module.m_server.GetOLAInUse() == OLASRV_EPSILON || 
						_Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT)
					{
						lMediaMasterType = CARD_TYPE_LOYALTY;
						bSkipParsingOLA = TRUE;
					}
					else
					{
						long lRtc = _Module.m_server.GetCardInfo(sAccountNumber,
							sTrack1,
							sTrack2,
							sShortCardNumber,
							&sRetData,
							lFlags,
							&lSessionType,
							lPumpNumber,
							bDoNotUseXmlBuffer); //4.0.25.0

					}
				

				}
				break;
			default:	//No Loyalty	
				{
					bSkipParsingOLA = TRUE;
				}
				break;
			}
		}

		if (!bSkipParsingOLA)
		{
			//Finally, translate the GetCardInfo result into card sale data structure:
			BYTE sGetCardInfoResult[sizeof(CARD_SALE_ALL3)] = { 0 }; //4.0.25.0
			long len = Bstr2Buffer(sRetData,sGetCardInfoResult,sizeof(sGetCardInfoResult)); //4.0.25.0
			if (!len)
			{
				str.Format("HandleLoyaltyResult, Bstr2Buffer failed"); 
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str,LOG_LEVEL_1); //4.0.25.0
			}
			else //4.0.24.60
			{
				CARD_SALE_ALL3	cGetCardInfoCardSaleAll3Result;
				memset(&cGetCardInfoCardSaleAll3Result, ' ', sizeof(CARD_SALE_ALL3));
				memcpy(&cGetCardInfoCardSaleAll3Result, sGetCardInfoResult, min(sizeof(cGetCardInfoCardSaleAll3Result),len));

				lMediaMasterType = a2l(cGetCardInfoCardSaleAll3Result.extData2.sMediaMasterType, sizeof(cGetCardInfoCardSaleAll3Result.extData2.sMediaMasterType));	//4.0.22.504 69344
				if (lMediaMasterType == CARD_TYPE_LOYALTY)
				{
					_Module.m_server.MatchReceivedLoyaltyCardInfoToOLA(cGetCardInfoCardSaleAll3Result); //4.0.23.507 89548

					if (pInfo->CardSaleInfo.cardData.sEntryMethod == 'B'
						&& (ChrAll(cGetCardInfoCardSaleAll3Result.CardSaleAll.extData.sLoyalId, sizeof(cGetCardInfoCardSaleAll3Result.CardSaleAll.extData.sLoyalId)))) //RFUEL-2399
					{
						//RFUEL-2817
						if (_Module.m_server.GetOLAInUse() == OLASRV_EPSILON||
							_Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT)
							memcpy(cGetCardInfoCardSaleAll3Result.CardSaleAll.extData.sLoyalId, pInfo->CardSaleExtraData3.sParamResult, sizeof(cGetCardInfoCardSaleAll3Result.CardSaleAll.extData.sLoyalId));  //Emerald
						else
							memcpy(cGetCardInfoCardSaleAll3Result.CardSaleAll.extData.sLoyalId, pInfo->CardSaleExtraData2.sBarCode, sizeof(cGetCardInfoCardSaleAll3Result.CardSaleAll.extData.sLoyalId));   //Storeline
					}

					memcpy(pInfo->CardSaleInfo.extraData.sLoyalId, cGetCardInfoCardSaleAll3Result.CardSaleAll.extData.sLoyalId, sizeof(pInfo->CardSaleInfo.extraData.sLoyalId));
					memcpy(&pInfo->CardSaleExtraData5.sLoyaltyStatus, &cGetCardInfoCardSaleAll3Result.CardSaleAll.data.sTranStatus, sizeof(&pInfo->CardSaleExtraData5.sLoyaltyStatus));
				}
				else if (bCheckLoyaltyCard)	//4.0.22.509 76060
				{
					memset(pInfo->CardSaleInfo.extraData.sLoyalId,' ', sizeof(pInfo->CardSaleInfo.extraData.sLoyalId));//4.0.23.390 116544 //4.0.24.90
					memset(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data,' ', sizeof(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data));//4.0.23.400 116544 //4.0.24.90	
				}
				else
				{
					SaveTempPapInfo(pInfo, cGetCardInfoCardSaleAll3Result, lPumpNumber);
				}

				
				if (bIgnoreParamResult) //4.0.20.440 for LPE flow 68857
				{
					BYTE sTranNbr[5];
					memcpy(sTranNbr,(pInfo->CardSaleInfo.cardData).sTranNbr,sizeof((pInfo->CardSaleInfo.cardData).sTranNbr));
					memcpy(&pInfo->CardSaleInfo.cardData, &cGetCardInfoCardSaleAll3Result, sizeof(cGetCardInfoCardSaleAll3Result));
					memcpy((pInfo->CardSaleInfo.cardData).sTranNbr,sTranNbr,sizeof((pInfo->CardSaleInfo.cardData).sTranNbr));
				}
				else
				{
					_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(pInfo);
				}
			}
		}
	}

	_Module.m_server.m_cFlowControlManager.SetMediaMasterType(lPumpNumber, lMediaMasterType);

	str.Format("CGCI::HandleLoyaltyResult return mediaMasterType %ld", lMediaMasterType);
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

	return lMediaMasterType	;	//4.0.22.504 69344
}

void CGCI::SaveTempPapInfo(PAY_AT_PUMP_INFO * pInfo, CARD_SALE_ALL3 &cTmpCardSaleData, long lPumpNumber)
{
	memcpy(pInfo->CardSaleExtraData2.sMediaMasterType, cTmpCardSaleData.extData2.sMediaMasterType,
		sizeof(cTmpCardSaleData.extData2.sMediaMasterType));

	if (ChrAll(cTmpCardSaleData.CardSaleAll.data.sAccountNumber, sizeof(cTmpCardSaleData.CardSaleAll.data.sAccountNumber))) // TD 458938
	{
		memcpy(cTmpCardSaleData.CardSaleAll.data.sAccountNumber,pInfo->CardSaleInfo.cardData.sAccountNumber,sizeof(pInfo->CardSaleInfo.cardData.sAccountNumber));
	}
	else
	{
		memcpy(pInfo->CardSaleInfo.cardData.sAccountNumber, cTmpCardSaleData.CardSaleAll.data.sAccountNumber,sizeof(pInfo->CardSaleInfo.cardData.sAccountNumber));
	}

	memcpy(&pInfo->CardSaleExtraData3.sCardSegment, &cTmpCardSaleData.extData3.sCardSegment,
		sizeof(pInfo->CardSaleExtraData3.sCardSegment));

	memcpy(pInfo->CardSaleExtraData6.sClubCardHashNumber, cTmpCardSaleData.extData6.sClubCardHashNumber,	// 4.0.32.??? TD 405435
		sizeof(pInfo->CardSaleExtraData6.sClubCardHashNumber));

	if (ChrAll(cTmpCardSaleData.extData4.sPreAuthAmount, sizeof(cTmpCardSaleData.extData4.sPreAuthAmount))) // TD 456795
	{
		memcpy(&cTmpCardSaleData.extData4.sPreAuthAmount,pInfo->CardSaleExtraData4.sPreAuthAmount,
			sizeof(pInfo->CardSaleExtraData4.sPreAuthAmount));
	}
	else
	{
		memcpy(pInfo->CardSaleExtraData4.sPreAuthAmount, &cTmpCardSaleData.extData4.sPreAuthAmount,
			sizeof(pInfo->CardSaleExtraData4.sPreAuthAmount));
	}

	if (cTmpCardSaleData.CardSaleAll.data.sTranType == ' ') //RFUEL-2732 Unable to Scan loyalty card and payment at the pump
	{
		memcpy(&cTmpCardSaleData.CardSaleAll.data.sTranType, &pInfo->CardSaleInfo.cardData.sTranType,
			sizeof(pInfo->CardSaleInfo.cardData.sTranType));
	}
	else
	{
		memcpy(&pInfo->CardSaleInfo.cardData.sTranType, &cTmpCardSaleData.CardSaleAll.data.sTranType,
			sizeof(pInfo->CardSaleInfo.cardData.sTranType));
	}

	long lOffet = 0;
	if (_Module.m_server.GetNumberOfTendersAuthotized(lPumpNumber - 1) > 0)
	{
		lOffet = sizeof(PAYMENT_CARD_DETAILS);
	}

	memcpy(pInfo->CardSaleExtraData6.PaymentCard1.sCardName + lOffet, cTmpCardSaleData.CardSaleAll.data.sCardName,
		sizeof(pInfo->CardSaleExtraData6.PaymentCard1.sCardName));

	memcpy(pInfo->CardSaleExtraData6.PaymentCard1.sTenderId + lOffet, cTmpCardSaleData.extData6.PaymentCard1.sTenderId + lOffet,
		sizeof(pInfo->CardSaleExtraData6.PaymentCard1.sTenderId));
}

STDMETHODIMP CGCI::PAPSwipe(long lPumpNumber, long lDeviceNumber, long * lSeqNumber, long lFlags, BSTR sData, long * pVal)
{
	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif
		if(LOG_BASIC_FLOW_CONTROL)
		{				
			_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,"PAPSwipe",lFlags);
		}

		PAPSwipe__(lPumpNumber,lDeviceNumber,lSeqNumber,lFlags,sData,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::PAPSwipe")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","PAPSwipe()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::PAPSwipe__( long lPumpNumber,  long lDeviceNumber, long *lSeqNumber,  long lFlags, BSTR sData, long *pVal)
{
	DWORD		dwLastErr = 0;
	GCIRetCode	retCode = GCI_OK;	
	long		lOlaRecSize = (lFlags & SWIPE_THREE_OLA_REC)?sizeof(CARD_SALE_ALL3):sizeof(CARD_SALE_ALL2); //4.0.20.506
	BOOL		bContByEncrypt = TRUE;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if((_Module.m_server.IsServerInRecoveryProcess() || (!_Module.m_server.m_cOLASrv->Valid())) 
			&& _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;
	else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
		retCode = GCI_SERVER_IN_MIDDLE_LOCK_SHIFT;   //4.0.1.40
	else if (_Module.m_server.CheckRealtimeDB(lPumpNumber) == false)
		retCode = GCI_FAILED_IO;	//RFUEL-3028


	_Module.m_server.DeleteMiscInfo(lPumpNumber);

	long lRtc = 0;
	BOOL bAskUserToTryAgain = FALSE;

	short nWaiting = 0,nFree = 0,nUnpaid = 0;
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetTrsListCount(&nWaiting, &nFree, &nUnpaid);
	if (nUnpaid)
	{
		_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_PUMP , SUB_TERMINAL_CARD_READER , PAP_SWIPE_WHILE_UNPAID_PAK_TRANSACTIONS ,lPumpNumber ,ALARM_STARTED , 0 , 0 );
	}

	//4.0.14.500
	CARD_SALE_ALL_CHIP_EXTRA_DATA	cardDataAll3ChipExtra;	//4.0.20.503 66960
	memset(&cardDataAll3ChipExtra, ' ', sizeof(cardDataAll3ChipExtra));

	int iLen = 0;

	if ( retCode == GCI_OK ) //4.0.22.64 //4.0.22.507 73628
	{
		//4.0.20.503 66960
		retCode = _Module.m_server.m_cXmlConvertorEx.GetStructCardSaleAll3ChipExtra(sData, lOlaRecSize, &cardDataAll3ChipExtra, &iLen);	

		if (retCode == GCI_LAST_ERROR)
		{
			_LOGMSG.LogMsg("XmlProblem: PAPSwipe, GetXmlInterface",LOG_LEVEL_1);
			dwLastErr = GetLastError();
		}
	}

	if ( retCode == GCI_OK )
	{		
		// in case OLASrv is authorizer:
		if (lFlags & SWIPE_OLA_COMMAND) // SWIPE_REQUEST, 
										// SWIPE_EXTRA_INFO, 
										// SWIPE_GET_RESPONSE, 
										// SWIPE_CANCEL, 
										// SWIPE_RECEIPT_UPDATE, 
										// SWIPE_RECEIPT_CHECK
		{
			OLA_STAT ola;
		
			long lCardSaleDataPumpNumber = 0;

			lCardSaleDataPumpNumber = a2l(cardDataAll3ChipExtra.cCardSaleAll3.CardSaleAll.data.sPumpNumber, sizeof(cardDataAll3ChipExtra.cCardSaleAll3.CardSaleAll.data.sPumpNumber) );

			if(PUMP_NOT_VALID(lCardSaleDataPumpNumber ))
			{
				l2a(lPumpNumber, cardDataAll3ChipExtra.cCardSaleAll3.CardSaleAll.data.sPumpNumber, sizeof(cardDataAll3ChipExtra.cCardSaleAll3.CardSaleAll.data.sPumpNumber));
			}


			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("PAPSwipe() - iLen = %d, lSeqNumber = %ld, sTranType = %c", 
							iLen, 
							*lSeqNumber, 
							cardDataAll3ChipExtra.cCardSaleAll3.CardSaleAll.data.sTranType);//4.0.21.500 69525 //4.0.22.506
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);		  //4.0.20.56
			}
			

			// 1. Only If First Swipe request - Create Trs and new trs number
			// 2. Set the link number to lSeqNumber, PAY_AT_PUMP 
			// 3. Insert Trs - local 
			// 4. Save PAY_AT_PUMP_INFO at pump x in m_cPumpFile
			// 5. Set OLA status to REQUEST_TO_SEND
		
		
			/******************************************************/
			// The seq number can be 0 in Pak with loyalty. 4.0.24.500 122449
			/******************************************************/

			CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();
			
			if(*lSeqNumber < 0)
			{
				retCode = GCI_INVALID_SEQ_NUMBER;

			}
			else if (( cPumpStatus.GetRes() != PAY_AT_KIOSK ) && (*lSeqNumber == 0))//4.0.23.410 
			{
				retCode = GCI_INVALID_SEQ_NUMBER;
			}
 
			//else
			if ( retCode == GCI_OK )	//4.0.24.500 122449
			{	
				//4.0.20.503	66960	//Need to add encrypt cardsaleall3 struct
				bContByEncrypt = _Module.m_server.m_cProtectedDataHandler.EncodeForTrs(SESSION_AUTHORIZE, &cardDataAll3ChipExtra.cCardSaleAll3);			//4.0.20.501 TD-60236
				if (bContByEncrypt == TRUE)	//4.0.20.501 TD-60236
				{				
					// M.L 8/5/98 Lock PumpArray in large scope 
					//-----------------------------------------
					CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

					// Update in OLA struct link to third record
					//4.0.2.36
					_Module.m_server.m_cOLASrv->ResetConnectionAttempts();

					//4.0.2.32
					_Module.m_server.m_cOLASrv->ResetRetryAttemps(TRUE);

					long  p = lPumpNumber-1;
					_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);

					CString sMsg;
					sMsg.Format("PAPSwipe() - Pump %ld - SeqNumber %ld. OLA state = %s",
						lPumpNumber,
						*lSeqNumber,
						_LOGMSG.GetStateString(ola.m_byState));
					_LOGMSG.LogMsg(sMsg);
					
					if(lFlags & SWIPE_RECEIPT_UPDATE)
					//===================================================
					{
						//4.0.20.503	66960			
						retCode = SwipeReceiptUpdate(lPumpNumber,*lSeqNumber,&ola,&cardDataAll3ChipExtra.cCardSaleAll3,&bAskUserToTryAgain);
					}
					else if(lFlags & SWIPE_RECEIPT_CHECK)
					//===================================================
					{
						//4.0.20.503	66960			
						retCode = SwipeReceiptCheck(lPumpNumber,*lSeqNumber,&ola,&cardDataAll3ChipExtra.cCardSaleAll3,&bAskUserToTryAgain);
					}
					else if(lFlags & SWIPE_EXTRA_INFO)
					//===================================================
					{
						//4.0.20.503	66960			
						retCode = SwipeExtraInfo(lPumpNumber,*lSeqNumber,&ola,&cardDataAll3ChipExtra.cCardSaleAll3,&bAskUserToTryAgain);					
					}
					else if(lFlags & SWIPE_REQUEST) 
					//===================================================
					{
						//4.0.20.20 - start
						//if there is waiting extra chip card info sent by convert layer, get it from the xml
						//we will send it later to the ola in CVMList[S]
						if (lFlags & SWIPE_INCLUDE_EXTRA_CHIP_INFO)
						{
							long lDataLen = 0;
							//4.0.20.503	66960				
							lDataLen = a2l(cardDataAll3ChipExtra.cChipExtraData.byLen, sizeof(cardDataAll3ChipExtra.cChipExtraData.byLen)); 
							
							if (lDataLen > 0)
							{
								//Get the actual data:

								memcpy(_Module.m_server.m_cCvmListInfo[p].byData, cardDataAll3ChipExtra.cChipExtraData.byData, sizeof(cardDataAll3ChipExtra.cChipExtraData.byData));
								//get the data len:
								l2a0(lDataLen, _Module.m_server.m_cCvmListInfo[p].byLen,sizeof(_Module.m_server.m_cCvmListInfo[p].byLen));								

								if(LOG_DETAIL_FLOW_CONTROL)
								{
									CString str;
									str.Format("CGCI::PapSwipe - found CvmList info from Convert Layer. Len = %ld",lDataLen);
									_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 
								}
							}
						}
						//4.0.20.20 - End
						
						//4.0.18.505 Merge from 4.0.14.1010 start
						//Reset the pap decline timer in case the timer is running.  
						_Module.m_server.m_lPAPDeclineTimer[lPumpNumber - 1] = 0;    //4.0.14.1010

						_Module.m_server.m_cCommonFunction.ClearDeclinedMiscs(lPumpNumber, "GCI::PapSwipe");

						if (!_Module.m_server.GetOLAFlags((BYTE*)cardDataAll3ChipExtra.cCardSaleAll3.extData2.sFlags,FL_PAP_AS_THIRD_REC))

						{
							if (lFlags & SWIPE_THREE_OLA_REC)
							{									
								long lTmp = a2l(cardDataAll3ChipExtra.cCardSaleAll3.extData2.sFlags, sizeof(cardDataAll3ChipExtra.cCardSaleAll3.extData2.sFlags)) + FL_PAP_AS_THIRD_REC;
								l2a(lTmp, cardDataAll3ChipExtra.cCardSaleAll3.extData2.sFlags, sizeof(cardDataAll3ChipExtra.cCardSaleAll3.extData2.sFlags));
							}
						}

						if(lFlags & SWIPE_VIT_CONFLICT) //3.0.0.8 - save VIT data when conflict detected
						{								
							retCode = SwipeSaveData(lPumpNumber,*lSeqNumber,&ola,&cardDataAll3ChipExtra.cCardSaleAll3, &bAskUserToTryAgain);
							
							if (retCode == GCI_OK)
							{
								CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[p].GetStat();
								long lMisc = pumpStat.GetMisc();
								lMisc |=MISC_VIT_CONFLICT;
								pumpStat.SetMisc(lMisc);
								_Module.m_server.m_cPumpArray[p].SetStat(pumpStat);
							}
							 //3.2.1.44
						}
						//4.0.20.503 66960
						else if(ReSendWithExtraInfo(lPumpNumber,*lSeqNumber,&ola,&cardDataAll3ChipExtra.cCardSaleAll3))
						{
							retCode = SwipeExtraInfo(lPumpNumber,*lSeqNumber,&ola,&cardDataAll3ChipExtra.cCardSaleAll3, &bAskUserToTryAgain);
						}
						else
						{															
							// This is second Swipe for Payment, do not open new transaction.							
							if (lFlags & SWIPE_PAYMENT)																
							{//4.0.20.503 66960							
								if (!_Module.m_server.GetOLAFlags((BYTE*)cardDataAll3ChipExtra.cCardSaleAll3.extData2.sFlags,FL_DO_NOT_OPEN_TRS)) //4.0.8.510
								{
									long lTmp = a2l(cardDataAll3ChipExtra.cCardSaleAll3.extData2.sFlags, sizeof(cardDataAll3ChipExtra.cCardSaleAll3.extData2.sFlags)) + FL_DO_NOT_OPEN_TRS;
									l2a(lTmp, cardDataAll3ChipExtra.cCardSaleAll3.extData2.sFlags, sizeof(cardDataAll3ChipExtra.cCardSaleAll3.extData2.sFlags));
								}
							}

							//4.0.29.10 - TD 264032
							CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[p].GetStat();
							if (MODE_VALID(pumpStat.GetMode()) && _Module.m_server.m_cModeArray[pumpStat.GetMode() - 1].PAPPostPayValid())
								lFlags |= SWIPE_SALE_COMMAND;

							
							retCode = SwipeRequest(lPumpNumber, *lSeqNumber, &ola, &cardDataAll3ChipExtra.cCardSaleAll3, &bAskUserToTryAgain , lFlags);	//4.0.20.503 66960
						}
					}				
					else if(lFlags & SWIPE_CANCEL)
					//===================================================
					{
						retCode = SwipeCancel(lPumpNumber,*lSeqNumber,&ola,&cardDataAll3ChipExtra.cCardSaleAll3, &bAskUserToTryAgain);	//4.0.20.503 66960
					}				
			
					_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola);
					_Module.m_server.SavePumpInfo(p+1); // save ola stat.


					// M.L 8/5/98 Lock PumpArray in large scope 
					//-----------------------------------------
//					LeaveCriticalSection(&_Module.m_server.m_csPumpArray);
				}
			}			
		}
		else // SWIPE_LOGGER_COMMAND
		{   // in case controller is also authorizer.
			retCode = GCI_NOT_SUPPORTED;
		}

		// Send SwipeResult , close the PAP transaction.
		if((retCode == GCI_FAILED_IO) || (bAskUserToTryAgain) || (bContByEncrypt == FALSE))  //4.0.20.501 TD-60236
		{		
			short bySession = OLA_NONE;  //4.0.9.507
			if(lFlags & SWIPE_RECEIPT_CHECK)					
			{
				bySession = SESSION_GET_RECEIPT;
			}
			else if((lFlags & SWIPE_EXTRA_INFO)||(lFlags & SWIPE_REQUEST))
			{
				bySession = SESSION_AUTHORIZE;
			}
			else if(lFlags & SWIPE_RECEIPT_UPDATE)
			{
				bySession = SESSION_RECEIPT_UPDATE;
			}

			// Call swipe result with summerized code relating 
			// to the requested session, and not relatin to the current ola.byStat
			if(!(lFlags & SWIPE_VIT_CONFLICT)) //3.0.0.8 - no need to send SwipeResult on conflict.
			{
				if(bContByEncrypt == FALSE)	//4.0.20.501 TD-60236
				{	//			__Decline
					_Module.m_server.SendSwipeResult(bySession, lPumpNumber, CD_NOT_OK, CD_DECLINE_DSS_ERROR, &cardDataAll3ChipExtra.cCardSaleAll3);	//4.0.20.503 66960
				}
				else if(bAskUserToTryAgain)
					//			__Decline
					_Module.m_server.SendSwipeResult(bySession,lPumpNumber, CD_NOT_OK,CD_DECLINE_USEALT, &cardDataAll3ChipExtra.cCardSaleAll3);	//4.0.20.503	66960						
				else
					//			__Decline
					_Module.m_server.SendSwipeResult(bySession,lPumpNumber, CD_NOT_OK,CD_DECLINE_ERROR, &cardDataAll3ChipExtra.cCardSaleAll3);	//4.0.20.503	66960

				_Module.m_server.m_cCommonFunction.HandleMiscCardNotAuthorized(lPumpNumber);											
			}
			if (retCode == GCI_FAILED_IO)
			{
				if (LOG_ERROR)
				{
					CString str;
					str.Format("PAPSwipe: Failed IO !!!  SeqNumber %d, lRtc %d ", *lSeqNumber,lRtc ); 
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str, LOG_LEVEL_0);							
				}
			}
		}

		if (!(lFlags & SWIPE_VIT_CONFLICT)) // Clear vit conflict
		{											
			CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();
			long lMisc = pumpStat.GetMisc();
			if (lMisc & MISC_VIT_CONFLICT)
			{
				lMisc &=MISC_NOT_VIT_CONFLICT;
				pumpStat.SetMisc(lMisc);
				_Module.m_server.m_cPumpArray[lPumpNumber -1].SetStat(pumpStat);			

			}					
		}
	}


	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"PAPSwipe",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::PumpTotals(long lPumpNumber, short nNozzle, short nGrade, short nFlags, long lVolumeH ,long lVolumeL, long lValueAH, long lValueAL,long lValueBH, long lValueBL , long * pVal)
{
	
	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif
		PumpTotals__(lPumpNumber,nNozzle,nGrade,nFlags,lVolumeH,lVolumeL,lValueAH,lValueAL,lValueBH,lValueBL,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::PumpTotals")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","PumpTotals()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}	
#endif

	return S_OK;
}

void CGCI::PumpTotals__( long lPumpNumber,  short nNozzle,  short nGrade,  short nFlags, long lVolumeH,  long lVolumeL,  long lValueAH,  long lValueAL,  long lValueBH,  long lValueBL, long *pVal)
{
	short nFinalGrade = nGrade;

	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("Rcved Pump Totals Grade %d",nGrade);
		_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,str,nFlags); 				
	}

	if(retCode == GCI_OK)
	{

		if(nFlags & TOTALS_NOT_AVAILABLE)
		{
			//3.2.0.34
			if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite() ) 
			//3.2.0.34
			{
				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------
				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

				if((_Module.m_server.m_cPumpArray[lPumpNumber-1].GetLockStatus() == WAIT_RESPONSE)||
				   (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetLockStatus() == LOCKED) )
				{
					long lMisc = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetMisc();
					lMisc &= MISC_NOT_WAIT_LOCK_TIMEOUT;
					lMisc &= MISC_NOT_WAIT_RESPONSE_TIMEOUT;

					_Module.m_server.m_cPumpArray[lPumpNumber-1].SetLockStatus(RESPONSE_RECEIVED,lMisc);
					_Module.m_server.SavePumpInfo(lPumpNumber);
				}

				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------
				if (slPumpArray.IsLocked())
					slPumpArray.Unlock();
			}
			if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_PUMP_TOTALS )
			{
				_Module.m_server.m_cServerInfo.ClearPumpTotalsFlags();
			}
		}
		else
		{
			TOTAL_INFO		info;
			memset(&info,0,sizeof(TOTAL_INFO));
			BOOL bWriteReadInfo = FALSE;

			//RFUEL-1265
			CString sVolume;
			sVolume.Format("%015ld%05ld", lVolumeH, lVolumeL);
			memcpy(info.sVolume, sVolume.GetBuffer(0), min(sizeof info.sVolume, sVolume.GetLength()));
			CString sValueA;
			sValueA.Format("%015ld%05ld", lValueAH, lValueAL);
			memcpy(info.sValueA, sValueA.GetBuffer(0), min(sizeof info.sValueA, sVolume.GetLength()));
			CString sValueB;
			sValueB.Format("%015ld%05ld", lValueBH, lValueBL);
			memcpy(info.sValueB, sValueB.GetBuffer(0), min(sizeof info.sValueB, sValueB.GetLength()));
			
			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("Total Volume %.20s, Total Value (A) %.20s, Total Value (B) %.20s", 
					info.sVolume,//4.0.12.505
					info.sValueA,
					info.sValueB);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 				
			}

			//4.0.3.39
			if(LOG_TRS_FILE)
				_Module.m_server.m_cTrsLog.LogPumpTotals(lPumpNumber,nNozzle,nGrade,info.sVolume,info.sValueA);

			if(CONTROLLER_IS_PUMP_LOCK_MASTER)
			{
				bWriteReadInfo = TRUE;
			}
			else
			{
				if(_Module.m_server.IsShiftChangeInProgress() )
				{
					if  (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetShiftNumber() != _Module.m_server.GetActiveShiftNumber())
					{
						bWriteReadInfo = TRUE;
					}
					else
					{ 
						// M.L 16.7.98
						if(LOG_LOGIC_ERROR)
							_LOGMSG.LogMsg("Unexpected pump totals, record ignored  !!! ", LOG_LEVEL_1);								

						bWriteReadInfo = FALSE;
					}
				}
				else
				{
					bWriteReadInfo = TRUE;
				}
			}

			if(bWriteReadInfo)
			{

				if(_Module.m_server.NozzleLevelLogic())
					if(nNozzle)
						nFinalGrade = _Module.m_server.m_cPumpArray[lPumpNumber-1].NozzleToGrade(nNozzle);

				info.lIndexNumber	= _Module.m_server.NewPumpTotalIndex();
				info.lPump			= lPumpNumber;
				info.nNozzle		= nNozzle;
				info.nGrade			= nFinalGrade;
				info.nFlags			= nFlags;

				if ( !_Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags())
					info.nFlags |= TOTALS_ON_DEMAND_REQUEST;

				info.dwActiveReadNumber = _Module.m_server.GetActiveReadNumber();
				info.dwShiftNumber	= _Module.m_server.m_cPumpArray[lPumpNumber-1].GetShiftNumber();
				GetLocalTime(&info.cSysTime);		

				//4.0.14.350 - protect writing to Qdex when flags are off.
				if(_Module.m_server.m_cServerInfo.GetMemoProcessFlags() == FALSE)
				{
					long lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_INSERT,&info);
					if(lRtc)
					{
						retCode = GCI_FAILED_IO;
					}
				}
			}

			if(nFlags & TOTALS_END_OF_LIST )
			{
				//3.2.0.34
				if(PUMPSRV_IS_PUMP_LOCK_MASTER||_Module.m_server.IsMeggitSite() ) 
				//3.2.0.34
				{
					// M.L 8/5/98 Lock PumpArray in large scope 
					//-----------------------------------------
					CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

					if ((_Module.m_server.m_cPumpArray[lPumpNumber-1].GetLockStatus() == WAIT_RESPONSE) ||
						(_Module.m_server.m_cPumpArray[lPumpNumber-1].GetLockStatus() == LOCKED) )
					{
						long lMisc = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetMisc();
						lMisc &= MISC_NOT_WAIT_LOCK_TIMEOUT;
						lMisc &= MISC_NOT_WAIT_RESPONSE_TIMEOUT;

						_Module.m_server.m_cPumpArray[lPumpNumber-1].SetLockStatus(RESPONSE_RECEIVED,lMisc);
						_Module.m_server.SavePumpInfo(lPumpNumber);
					}

					// M.L 8/5/98 Lock PumpArray in large scope 
					//-----------------------------------------
					if (slPumpArray.IsLocked())
						slPumpArray.Unlock();
				}


				if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_PUMP_TOTALS )
				{
					_Module.m_server.m_cServerInfo.ClearPumpTotalsFlags();
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Pump total end of list !!!!");
				}
				//Add by Yaron for Ctrl Events
				_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_PUMPS_TOTALS,0);
			}
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"PumpTotals",
			TRUE,retCode,
			FALSE,0);


	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;

		str.Format("PumpTotals(Pump %ld, Nozzle %d, Grade %d, flags %d, VolumeH %ld, VolumeL %ld, ValueAH %ld, ValueBH %ld, ValueAL %ld, ValueBL %ld)",
			lPumpNumber, nNozzle, nGrade , nFlags, lVolumeH , lVolumeL , lValueAH , lValueBH, lValueAL, lValueBL);

		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}


	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGCI::PumpMode(long lPumpNumber, long lPumpMode, long * pVal)
{

#ifdef CATCH_GCI
	__try
	{
#endif
		PumpMode__(lPumpNumber,lPumpMode,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::PumpMode")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","PumpMode()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::PumpMode__( long lPumpNumber,  long lPumpMode, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if(MODE_NOT_VALID(lPumpMode))
		retCode = GCI_INVALID_PUMP_MODE;

	if(retCode == GCI_OK)
	{
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if(CONTROLLER_IS_MODE_MASTER)
		{
			CPumpStatus stat = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();
			stat.SetMode(lPumpMode);
			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetStat(stat);
			_Module.m_server.SavePumpInfo(lPumpNumber);
		}
		else
		{
			retCode = GCI_NOT_SUPPORTED;
		}

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"PumpMode",
			TRUE,retCode,
			FALSE,0);

	
	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::GradePrice(long lPumpNumber, short nNozzle, short nGrade, short nFlags, long lPriceA, long lPriceB, long * pVal)
{

#ifdef CATCH_GCI
	__try
	{
#endif
		GradePrice__(lPumpNumber,nNozzle,nGrade,nFlags,lPriceA,lPriceB,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GradePrice")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","GradePrice()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::GradePrice__( long lPumpNumber,  short nNozzle,  short nGrade,  short nFlags,  long lPriceA,  long lPriceB, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if(GRADE_NOT_VALID(nGrade))
		retCode = GCI_INVALID_GRADE_NUMBER;


	if(retCode == GCI_OK)
	{
		retCode = GCI_NOT_SUPPORTED;
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"GradePrice",
			TRUE,retCode,
			FALSE,0);

	
	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGCI::TankReading(long lTankNumber, long lProduct, short nFlags, long lStatus, long lFuelLevel, long lWaterLevel, long lTemperature, long lVolume, long lWaterVolume, long *pVal)
{

#ifdef CATCH_GCI
	__try
	{
#endif
		TankReading__(lTankNumber,lProduct,nFlags,lStatus,lFuelLevel,lWaterLevel,lTemperature,lVolume,lWaterVolume,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::TankReading")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::TankReading()", LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::TankReading__( long lTankNumber, long lProduct,  short nFlags,  long lStatus,  long lFuelLevel,  long lWaterLevel,  long lTemperature,  long lVolume,  long lWaterVolume, long *pVal)
{

	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();

	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
		
	else if(!(nFlags & TANK_READ_NOT_AVAILABLE))
	{
		if (TANK_NOT_VALID(lTankNumber))
			retCode = GCI_INVALID_TANK_NUMBER;
	}

	if(retCode == GCI_OK)
	{
		if(nFlags & TANK_READ_NOT_AVAILABLE )
		{
			// not available...
			if(LOG_BASIC_FLOW_CONTROL)
			{		
				CString str;
				if(lTankNumber == 0)//4.0.2.15 All tanks
					str = "Tank Reading not available for All Tanks";
				else
					str.Format("Tank Reading not available for Tank %d !",lTankNumber);
				_LOGMSG.LogMsg(str);
			}	

			// update flag if needed			
			if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite())
			{
				if(!_Module.m_server.m_bTankReadingRcved)
					_Module.m_server.m_bTankReadingRcved = TRUE;
			}


			if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS  )
			{
				_Module.m_server.m_cServerInfo.ClearTankReadingFlags();
			}
		}
		else
		{		
			BOOL bSaveRcvedInfo = FALSE;

			if(CONTROLLER_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite())
			{
				bSaveRcvedInfo = TRUE;
			}
			else if (!_Module.m_server.m_bTankReadingRcved) 
			{
				bSaveRcvedInfo = TRUE;
			}
			else if(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS )
			{
				bSaveRcvedInfo = TRUE;
			}

			if(bSaveRcvedInfo)
			{
							
				TANK_READ_INFO		info;
				FillTankReadingInfo(info, lTankNumber, lProduct, lStatus, nFlags, lFuelLevel, lWaterLevel, lTemperature, lVolume, lWaterVolume, 0, 0, 0, 0, 0);

				//4.0.0.82 - not saving into the qdex if from memo interval process.
				long lRtc = 0;
				
				if (_Module.m_server.m_cServerInfo.GetMemoProcessFlags() == FALSE)
					lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_INSERT,&info);
				if(lRtc)
				{
					retCode = GCI_FAILED_IO;
				}

				SYSTEMTIME cTime;  //443035 since cTime was added to SetTankInfo
				memset(&cTime, 0, sizeof(cTime));

				//add by Yaron
				//checking if having memo process in system
				if ( _Module.m_server.GetMemoInterval() > 0 )
				{
					_Module.m_server.m_cTankArray[lTankNumber-1].SetTankInfo(
					lProduct,lStatus,nFlags, lFuelLevel,lWaterLevel,lTemperature,
 					lVolume, lWaterVolume,0,0,0, cTime);
					_Module.m_server.m_cTankArray[lTankNumber-1].SetTankValidity(true);

					if( nFlags & TANK_READ_END_OF_LIST )
					{
						_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_MEMO_TANK_READING,0);
						_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,0,GCI_EVENT_MEMO_TANK_READING );
					}
				}
				//end add by Yaron
			}
			else
			{
				if(LOG_LOGIC_ERROR)
					_LOGMSG.LogMsg("Unexpected tank reading , record ignored  !!! ", LOG_LEVEL_1);
			}

			if( nFlags & TANK_READ_END_OF_LIST )
			{
				//4.0.1.38 - sending the event only if writing into the qdex.
				if (_Module.m_server.m_cServerInfo.GetMemoProcessFlags() == FALSE)
					_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_TANK_READING,0);//Add by Yaron for Ctrl Events

				if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS )
				{
					_Module.m_server.m_cServerInfo.ClearTankReadingFlags();

					SQLPumpSrvFile.SaveServerInfo();

					_LOGMSG.LogMsg("Tank reading end of list !!!");
				}
				if(lStatus == TANK_STATUS_IDLE)
				{				
					// update flag if needed  //3.2.0.34
					if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite())
					{
						if(!_Module.m_server.m_bTankReadingRcved)
							_Module.m_server.m_bTankReadingRcved = TRUE;
					}
				}
			}
		}
	}
	
	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"TankReading",
			TRUE,retCode,
			FALSE,0);

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;

		str.Format("Tank reading(lTankNumber = %ld, lProduct = %ld, nFlags = %d, lStatus = %ld, lFuelLevel = %ld, lWaterLevel = %ld, lTemperature = %ld, lVolume = %ld, lWaterVolume = %ld)",
			lTankNumber,lProduct,nFlags,lStatus,lFuelLevel,lWaterLevel,lTemperature,lVolume,lWaterVolume);

		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}

	
	if(pVal != NULL)
		*pVal = retCode;
}



STDMETHODIMP CGCI::GetParam(BSTR sTableName, long lKey1, long lKey2, BSTR sFiledName, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long * pVal)
{
	
	USES_CONVERSION;

	if( ((sVal == NULL) && (nFiledType == FIELD_SZ)) || 
		((lVal == NULL) && (nFiledType == FIELD_DWORD)) )
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"IGCI::GetParam   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	
		return E_POINTER;
	}

#ifdef CATCH_GCI
	__try
	{
#endif
		GetParam__(sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetParam")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetParam()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::GetParam__(BSTR sTableName,  long lKey1,long lKey2,  BSTR sFiledName,  short nFiledType,  BSTR *sVal,long *lVal,  BOOL bCreate,long *pVal)
{
	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;	
	
	long lRtc = -1;
	char szTableName[MAX_TABLE_NAME]; 
	char szFieldName[MAX_FIELD_NAME]; 
	char szFieldVal[MAX_FIELD_VAL]; 

	memset(szTableName,0,sizeof(szTableName));
	int iLen = WideCharToMultiByte(CP_ACP,0,sTableName,-1,szTableName,sizeof(szTableName),NULL,NULL);
	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GCI_TABLE_NAME_TOO_LONG;
	}
	
	memset(szFieldName,0,sizeof(szFieldName));
	iLen = WideCharToMultiByte(CP_ACP,0,sFiledName,-1,szFieldName,sizeof(szFieldName),NULL,NULL);
	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GCI_FIELD_NAME_TOO_LONG;
	}

	BOOL bAllowCreate = bCreate;
	if(bCreate)
	{
		if(_Module.m_server.m_cParam.IsPumpSrvTable(szTableName))
			bAllowCreate = FALSE;
		else
			bAllowCreate = TRUE;
	}


	if(nFiledType == FIELD_SZ)
	{
		memset(szFieldVal,0,sizeof(szFieldVal));
		iLen = WideCharToMultiByte(CP_ACP,0,*sVal,-1,szFieldVal,sizeof(szFieldVal),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_DEFAULT_FIELD_VAL_TOO_LONG;
		}
		SysFreeString( *sVal );

		if(retCode == GCI_OK)
		{  			
			lRtc =  _Module.m_server.m_cParam.GetParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,sizeof(szFieldVal),NULL,bAllowCreate); 
			CString strRetVal;
			
			//Check if the string is ""
			if(szFieldVal[0])
			{
				strRetVal = szFieldVal;
			}
			else
			{
				strRetVal = "  ";
				retCode = GCI_FIELD_EMPTY;
				lRtc = -1;
			}
			*sVal = strRetVal.AllocSysString();
		}
		else
		{
			CString strRetVal("  ");
			*sVal = strRetVal.AllocSysString(); 
		}
	}
	else // field DW 
	{
		SysFreeString( *sVal );	

		DWORD dwRetVal = (DWORD)*lVal;
		if(retCode == GCI_OK)	
			lRtc = _Module.m_server.m_cParam.GetParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,NULL,0,&dwRetVal,bAllowCreate); 
		*lVal = (long)dwRetVal;

		CString strRetVal("  ");
		*sVal = strRetVal.AllocSysString(); 
	}

	if(lRtc != -1)
	{
		switch(lRtc)
		{
			case 0 : retCode = GCI_OK; break;
			case 1 : retCode = GCI_FAILED_TO_OPEN_TABLE; break;
			case 2 : retCode = GCI_PARAM_NOT_FOUND_AND_CREATED; break;
			case 3 : retCode = GCI_PARAM_NOT_FOUND_FAILED_TO_CREATE; break;
			case 4 : retCode = GCI_PARAM_NOT_FOUND; break;
				//4.0.3.21
			default:
				retCode  = ConverHydServiceCode2GciCode((CFGDBRetCode)lRtc);
				//4.0.3.21
				

			
		}
	}
////////////////////////////////////////////////////////
	if(retCode)
		if(LOG_LOGIC_ERROR)
		{
			
			CString str;
			str.Format("GetParam szTableName %.20s lKey1=%ld lKey2=%ld szFieldName=%.20s",
				szTableName,lKey1,lKey2, szFieldName);
			_LOGMSG.LogMsg(str);
		}	


//////////////////////////////////////////////////


	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"GetParam",
			TRUE,retCode,
			(dwLastErr!=0)? TRUE:FALSE,dwLastErr);
	
	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::GetRealTimeClock(BSTR * psDateTime, long * pVal)
{

	USES_CONVERSION;

	if(psDateTime == NULL)
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"IGCI::GetRealTimeClock   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	
		return E_POINTER;
	}

	
#ifdef CATCH_GCI
	__try
	{
#endif
		GetRealTimeClock__(psDateTime,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetRealTimeClock")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetRealTimeClock()", LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::GetRealTimeClock__( BSTR *psDateTime, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(retCode == GCI_OK)
	{
		SYSTEMTIME curSysTime;
		GetLocalTime(&curSysTime);					
		SZ_TAG_SYSTEMTIME szTagTime;		
		memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
		SystemTime2Tag(&curSysTime,&szTagTime.sTime);

		CComBSTR sRealTime(sizeof(SZ_TAG_SYSTEMTIME)+1,(LPCSTR)&szTagTime.sTime); 
		*psDateTime = sRealTime.Copy(); 		
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"GetRealTimeClock",
			TRUE,retCode,
			FALSE,0);

	
	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGCI::DeliveryReport(long lTankNumber, long lProduct, short nFlags, long lStatus, long lTag, long lVolume, BSTR sDateAndTime, long * pVal)
{

//	_LOGMSG.LogLocationIn("GCI::DeliveryReport()",0);

	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif
		DeliveryReport__(lTankNumber,lProduct,nFlags,lStatus,lTag,lVolume,sDateAndTime,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::DeliveryReport")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::DeliveryReport()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::DeliveryReport__( long lTankNumber,  long lProduct,  short nFlags,  long lStatus,  long lTag,  long lVolume,  BSTR sDateAndTime, long *pVal)
{
	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;	
	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(!(nFlags & DELIVERY_REPORT_NOT_AVAILABLE))
	{
		if(TANK_NOT_VALID(lTankNumber))
			retCode = GCI_INVALID_TANK_NUMBER;
	}

	if(retCode == GCI_OK)
	{

		if(nFlags & DELIVERY_REPORT_NOT_AVAILABLE)
		{
			// not available...
			if(LOG_BASIC_FLOW_CONTROL)
			{		
				CString str;

				//4.0.2.161
				//All tanks
				if(lTankNumber == 0)
					str.Format("Delivery report not available for all tanks");

				else
					str.Format("Delivery report not available for Tank %d!",lTankNumber);
				
				_LOGMSG.LogMsg(str);
			}	
			
			// update flag if needed 3.2.0.34			 
			if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite() || (nFlags & DELIVERY_FORCE_ACCEPT_DATA) )  //4.0.6.502)
			{
				if(!_Module.m_server.m_bDeliveryReportRcved)
					_Module.m_server.m_bDeliveryReportRcved = TRUE;				
			}

			if (( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY ) || (nFlags & DELIVERY_FORCE_ACCEPT_DATA) )  //4.0.6.502 //4.0.8.500
			{
			
				_Module.m_server.m_cServerInfo.ClearDeliveryReportFlags();										

			}
		}
		else
		{
			BOOL bSaveRcvedInfo = FALSE;

			if(CONTROLLER_IS_PUMP_LOCK_MASTER  ||_Module.m_server.IsMeggitSite())
			{
				bSaveRcvedInfo = TRUE;
			}
			else if (nFlags & DELIVERY_FORCE_ACCEPT_DATA)  //4.0.6.502
			{
				bSaveRcvedInfo = TRUE;
			}
			else if (!_Module.m_server.m_bDeliveryReportRcved) 
			{
				bSaveRcvedInfo = TRUE;
			}
			else if(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY )//4.0.1.38				
			{
				bSaveRcvedInfo = TRUE;
			}

			if(bSaveRcvedInfo)
			{
				DELIVERY_INFO		info;
				FillDeliveryReport(info, lTankNumber, lProduct, lStatus, nFlags, lVolume, 0, 0, 0, sDateAndTime, retCode, NULL,dwLastErr); // 4.0.19.2170  CR 409564

				if(retCode == GCI_OK)
				{
					long lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_INSERT,&info);
					if(lRtc)
					{
						retCode = GCI_FAILED_IO;
					}
				}
			}
			else
			{
				// M.L 16.7.98
				if(LOG_LOGIC_ERROR)
					_LOGMSG.LogMsg("Unexpected Delivery report , record ignored  !!! ", LOG_LEVEL_1);
			}
			
			if(retCode == GCI_OK)
			{
				if(nFlags & DELIVERY_REPORT_END_OF_LIST)
				{	//3.2.0.34
					if(PUMPSRV_IS_PUMP_LOCK_MASTER  ||_Module.m_server.IsMeggitSite())
					{
						// update flag if needed			
						if(!_Module.m_server.m_bDeliveryReportRcved)
							_Module.m_server.m_bDeliveryReportRcved = TRUE;				
					}

					if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY )
					{

						_Module.m_server.m_cServerInfo.ClearDeliveryReportFlags();
						
					}
					//Add by Yaron for Ctrl Events
					_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_DELIVERY_REPORT,0);
				}
			}
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"DeliveryReport",
			TRUE,retCode,
			(dwLastErr!=0)? TRUE:FALSE,dwLastErr);


	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;

		str.Format("Get delivery report(lTankNumber = %ld, Flags %d, Status %ld, Product %ld, Delivered volume %ld)",
		lTankNumber,nFlags,lStatus,lProduct,lVolume); 
		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}


	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::GetGradePrice(short nGrade, long * pVal)
{
	
#ifdef CATCH_GCI
	__try
	{
#endif
		GetGradePrice__(nGrade,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetGradePrice")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetGradePrice()", LOG_LEVEL_0);	

		exit(1);
	}
#endif


	return S_OK;
}

void CGCI::GetGradePrice__( short nGrade, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(GRADE_NOT_VALID(nGrade))
		retCode = GCI_INVALID_GRADE_NUMBER;
	
	if(retCode == GCI_OK)
	{
		_Module.m_server.SendGradePrice(nGrade);		
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"GetGradePrice",
				TRUE,retCode,
				FALSE,0);


	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::TankStatus(long lTankNumber, long lStatus, long * pVal)
{

#ifdef CATCH_GCI
	__try
	{
#endif
		TankStatus__(lTankNumber,lStatus,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::TankStatus")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::TankStatus()", LOG_LEVEL_0);	

		exit(1);
	}		
#endif

	return S_OK;
}

void CGCI::TankStatus__( long lTankNumber,  long lStatus, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	if(retCode == GCI_OK)
	{
		retCode = GCI_NOT_SUPPORTED;
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"TankStatus",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::InitInterface2(BSTR strControllerName, long lControllerVer, long lFlags, long *pVal)
{
	
	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif
		InitInterface2__(strControllerName,lControllerVer,lFlags,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::InitInterface2")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::InitInterface2()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

 	return S_OK;
}

void CGCI::InitInterface2__( BSTR strControllerName,  long lControllerVer,  long lFlags, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	CString strConvertLayerName,strConvertLayerVersion;
	CString strTemp = strControllerName;
	int iVerLen = a2i((unsigned char *)(LPCTSTR)strTemp.Left(2), 2);

	if((iVerLen>7) && (iVerLen <30))
	{
		strTemp = strTemp.Right(strTemp.GetLength()-2);
		strConvertLayerVersion = strTemp.Left(iVerLen);
		strConvertLayerName = strTemp.Right(strTemp.GetLength() - iVerLen);		
	}
	else 
	{
		strConvertLayerName = strTemp;
	}

	if(lFlags & INIT_CONTROLLER_NO_SERVICES)
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("ExternalController - %s ,Version %ld",strConvertLayerName,lControllerVer); 
			_LOGMSG.LogReq(0,"IGCI", "InitInterface", (LPCSTR)str); 				
		}	
	}
	else
	{

		if(lFlags & INIT_CONTROLLER_SUPPORT_AVI)
			_Module.m_server.m_bControllerSupportAVI = TRUE ; 
		else
			_Module.m_server.m_bControllerSupportAVI = FALSE ; 

		//amram 4.0.10.502
		if(lFlags & INIT_CONTROLLER_SUPPORT_NOZZLE_RESTRICTION)
			_Module.m_server.m_bControllerSupportNozzleRestrction = TRUE ; 
		else
			_Module.m_server.m_bControllerSupportNozzleRestrction = FALSE ;
		
		_Module.m_server.m_strControllerName = strConvertLayerName;
		_Module.m_server.m_strConvertLayerVersion = strConvertLayerVersion;

		if(lFlags&INIT_CONTROLLER_IS_MASTER)
			_Module.m_server.SetOperatingMode(SLAVE);
		else
			_Module.m_server.SetOperatingMode(MASTER);

		if(lFlags & INIT_CONTROLLER_GEN_TRS_NUMBER)
			_Module.m_server.m_bControllerGenTrsNumber = TRUE;
		else
			_Module.m_server.m_bControllerGenTrsNumber = FALSE;
		
		if(lFlags & INIT_CONTROLLER_GEN_SEQ_NUMBER)
			_Module.m_server.m_bControllerGenSeqNumber = TRUE;
		else
			_Module.m_server.m_bControllerGenSeqNumber = FALSE;

		if(lFlags & INIT_CONTROLLER_MANAGE_PUMP_MODE)
			_Module.m_server.m_bControllerManagePumpMode = TRUE;
		else
			_Module.m_server.m_bControllerManagePumpMode = FALSE;

		if(lFlags & INIT_CONTROLLER_MANAGE_PUMP_LOCK)
			_Module.m_server.m_bControllerManagePumpLock = TRUE;
		else
			_Module.m_server.m_bControllerManagePumpLock = FALSE;


		if(lFlags & INIT_CONTROLLER_FORCE_REMOVE_TRS)		
			_Module.m_server.m_bControllerReportOnPaidTrs =TRUE;
		else
			_Module.m_server.m_bControllerReportOnPaidTrs =FALSE;
		
		//4.0.5.41
		if(lFlags & INIT_CONTROLLER_MANAGE_PUMP_MODE2)		
			_Module.m_server.m_bControllerManagePumpMode2 = TRUE;
		else
			_Module.m_server.m_bControllerManagePumpMode2 =FALSE;


		//4.0.5.44
		if(lFlags & INIT_CONTROLLER_MANAGE_PRINTER_STATUS)		
			_Module.m_server.m_bControllerManagePrinterStatus = TRUE;
		else
			_Module.m_server.m_bControllerManagePrinterStatus = FALSE;


		//4.0.11.508
		if(lFlags & INIT_CONTROLLER_IN_MIDDLE_RECOVERY_PROCESS)		
			_Module.m_server.m_bSupportControllerInMiddleRecoveryProcess = TRUE;
		else
			_Module.m_server.m_bSupportControllerInMiddleRecoveryProcess = FALSE;
			
		//4.0.13.506
		if(lFlags & INIT_CONTROLLER_EXPECT_ALL_DETAILS_IN_NEED_MORE_PARAM)		
		{
			_Module.m_server.m_bExpectAllDetailsInNeedMoreParam = TRUE;

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("** The controller expect all of the details in need more params **"); 
				_LOGMSG.LogMsg(str); 				
			}	
		}
		else
		{
			_Module.m_server.m_bExpectAllDetailsInNeedMoreParam = FALSE;
		}

		//4.0.14.500
		if(lFlags & INIT_CONTROLLER_USE_XML_BUFFER)		//4.0.21.501 70752
		{
 			if (_Module.m_server.GetIFSFTerminalSrvInUse()) //4.0.22.73 - TD 74795 // CR 444830
 				_Module.m_server.m_bUseXmlBuffer = FALSE;
 			else
			{
				_Module.m_server.m_bUseXmlBuffer = TRUE;

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("The controller will handle card sale data buffer as xml"); 
					_LOGMSG.LogMsg(str); 				
				}	
			}
		}
		else
		{
			_Module.m_server.m_bUseXmlBuffer = FALSE;
		}

		if(lFlags & INIT_CONTROLLER_ALLOW_AUTH_ON_NON_REQUEST_PAP_RES_PUMP)	 //4.0.21.500 TD 69683
		{
			_Module.m_server.m_bAllowAuthOnNonRequestPapResPumpCL = TRUE;

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("PumpSrv will allow authorize PAP on non requesting pump."); 
				_LOGMSG.LogMsg(str); 				
			}	
		}
		else
		{
			_Module.m_server.m_bAllowAuthOnNonRequestPapResPumpCL = FALSE;
		} 

		//4.0.22.500 - TD 69907
		if (lFlags & INIT_CONTROLLER_SUPPORT_STOP_ALL_PUMPS)
		{
			_Module.m_server.m_bControllerSupportStopAllPumps = TRUE;

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Controller supports stopping all pumps at once."); 
				_LOGMSG.LogMsg(str); 				
			}	
		}
		else
			_Module.m_server.m_bControllerSupportStopAllPumps = FALSE;


		if(lFlags & INIT_CONTROLLER_ALLOW_AUTH_ON_STOP_BY_CONTROLLER)	 //4.0.20.780 	- start
		{
			_Module.m_server.m_AllowAuthOnStopByController = TRUE;

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("PumpSrv will allow authorize pump on STOP_BY_CONTROLLER."); 
				_LOGMSG.LogMsg(str); 				
			}	
		}
		else
		{
			_Module.m_server.m_AllowAuthOnStopByController = FALSE;
		}  //4.0.20.780 	- end
		

		if(lFlags & INIT_CONTROLLER_REPORTS_SIX_DIGITS_SEQ_NUMBER)	 //4.0.23.503 	- start TD 78769
		{
			_Module.m_server.m_bControllerReportsSixDigitSeqNumber = TRUE;

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("PumpSrv will allow receiving 6 digits seq number from controller."); 
				_LOGMSG.LogMsg(str); 				
			}	
		}
		else
		{
			_Module.m_server.m_bControllerReportsSixDigitSeqNumber = FALSE;
		}  //4.0.23.503 	- end
		
		//4.0.5.48
		_Module.m_server.m_bWasModeUpdated = FALSE;

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("SiteController - %s ,Version %ld",_Module.m_server.m_strControllerName,lControllerVer); 
			_LOGMSG.LogReq(0,"IGCI", "InitInterface2", (LPCSTR)str); 				
		}	

		if(	_Module.m_server.GetServerState() == SERVER_NO_CONVERT)
		{			
			
			_Module.m_server.SetServerState(SERVER_STARTUP); 
			m_CmdTxThreadInfo.m_pGCI = this;
			m_CmdTxThreadInfo.m_pServer = &_Module.m_server;

			m_hThread = (HANDLE) _beginthreadex(	NULL,
									0,
									CmdTx,
									&m_CmdTxThreadInfo,
									0,
									&m_usThreadId );

			StartCmdTxThread();
			// Triger controller to send pump status (0 - Src, 0 - All pumps)
			_Module.m_server.SetCommand(CMD_GET_PUMP_STATUS, SERVER_POS_NUMBER, 0);

			if(LOG_BASIC_FLOW_CONTROL)
			{
				_LOGMSG.LogMsg("GCI Thread CmdTx - Started ! ");
			}
		}

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("SiteController - %s ,Version %ld, Flags=%ld",_Module.m_server.m_strControllerName,lControllerVer,lFlags); 
			_LOGMSG.LogReq(0,"IGCI", "InitInterface2", (LPCSTR)str); 				
		}	
	}

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::Watchdog(long lParam, long *pVal)
{

#ifdef CATCH_GCI
	__try
	{
#endif
		Watchdog__(lParam,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::Watchdog")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::Watchdog()", LOG_LEVEL_0);

		exit(1);
	}	
#endif

	return S_OK;
}

void CGCI::Watchdog__( long lParam, long *pVal)
{
	GCIRetCode retCode = GCI_OK;

	_Module.m_server.WatchdogRcved();
	
	if(pVal != NULL)
		*pVal = retCode;

}
//4.0.20.503 66960
GCIRetCode CGCI::SwipeReceiptUpdate(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla ,CARD_SALE_ALL3  *pCardData, BOOL *pbAskUserToTryAgain)
{

	GCIRetCode retCode = GCI_OK;

	switch (OLA_STATE(pOla->m_byState))
	{
		case OLA_NONE:								
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_RECEIPT_UPDATE,lPumpNumber,lSeqNumber, pCardData);
			if(lRtc)
			{					
				// Call swipe result with summerized code relating 
				// to the requested session, and not relatin to the current pOla->byStat								
				retCode = GCI_FAILED_IO;								
			}
			else
			{
				// Trigger OLA process
				//--------------------
				pOla->m_byState = OLA_REQUEST_TO_SEND | SESSION_RECEIPT_UPDATE;
				pOla->m_lSessionNumber = 0; 

				if(LOG_BASIC_FLOW_CONTROL)
					_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
			}
		}
		break;

		case OLA_SEND_END:		 
		case OLA_REQUEST_TO_SEND:
		case OLA_WAIT_RESPONSE:
		case OLA_REQUEST_TO_SEND_MORE:						
		{
			retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
			*pbAskUserToTryAgain = TRUE;
		}
		break;

		case OLA_WAIT_FOR_MORE_INFO:
		{
			pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_SEND_END);							 
			if(LOG_BASIC_FLOW_CONTROL)
				_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
			retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;							
			*pbAskUserToTryAgain = TRUE;
		}
		break;
	}

	return retCode;
}

//4.0.20.503 66960
GCIRetCode CGCI::SwipeReceiptCheck(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CARD_SALE_ALL3  *pCardData,BOOL *pbAskUserToTryAgain)
{
	GCIRetCode retCode = GCI_OK;

	switch (OLA_STATE(pOla->m_byState))
	{
		case OLA_NONE:								
		{
			//4.0.18.501
			//4.0.20.503 66960
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_RECEIPT_CHECK, lPumpNumber,lSeqNumber, pCardData);
			if(lRtc)
			{						
				// Call swipe result with summerized code relating 
				// to the requested session, and not relatin to the current pOla->byStat								
				retCode = GCI_FAILED_IO;								
			}
			else
			{
				// Trigger OLA process
				//--------------------
				pOla->m_byState = OLA_REQUEST_TO_SEND | SESSION_GET_RECEIPT;
				pOla->m_lSessionNumber = 0; 

				if(LOG_BASIC_FLOW_CONTROL)
					_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
			}
		}
		break;

		case OLA_SEND_END:		 
		case OLA_REQUEST_TO_SEND:
		case OLA_WAIT_RESPONSE:
		case OLA_REQUEST_TO_SEND_MORE:						
		{
			retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;							
			*pbAskUserToTryAgain = TRUE;
		}
		break;

		case OLA_WAIT_FOR_MORE_INFO:
		{
			pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_SEND_END);							 
			if(LOG_BASIC_FLOW_CONTROL)
				_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
			retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;							
			*pbAskUserToTryAgain = TRUE;
		}
		break;
	}

	return retCode;

}

BOOL CGCI::ReSendWithExtraInfo(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CARD_SALE_ALL3 *pCardData)
{
	BOOL bReSendExtra = FALSE;


	if((OLA_STATE(pOla->m_byState) == OLA_WAIT_FOR_MORE_INFO)||
		(OLA_STATE(pOla->m_byState) == OLA_LOYALTY_WAIT_RESPONSE))//4.0.9.508
	{	 // driver #, Odometer #, 

		long lLastSeqNumber = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPSeqNumber();
		if (lLastSeqNumber == lSeqNumber)
		{ 
			PAY_AT_PUMP_INFO savedInfo; //4.0.5.0.
			_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&savedInfo);
			if(!memcmp(savedInfo.CardSaleInfo.cardData.sTrack2Data,pCardData->CardSaleAll.data.sTrack2Data,sizeof(savedInfo.CardSaleInfo.cardData.sTrack2Data)) )
			{
				if( (savedInfo.CardSaleExtraData2.sAccountType			== '?') || //EMV Costco
					(savedInfo.CardSaleInfo.cardData.sOdometer[0]		== '?') ||
					(savedInfo.CardSaleInfo.cardData.sFullVehicleId[0]	== '?') ||
					(savedInfo.CardSaleInfo.cardData.sPINBlock[0]		== '?') ||
					(savedInfo.CardSaleInfo.cardData.sReturnTank			== '?') ||
					(savedInfo.CardSaleInfo.cardData.sDriverId[0]		== '?') ||
					(savedInfo.CardSaleInfo.cardData.sGRPrompt			== '?') ||
					(savedInfo.CardSaleInfo.cardData.sGRVoucherNumber[0]	== '?') ||
					(savedInfo.CardSaleInfo.cardData.sGeneralID[0]		== '?') ||
					(savedInfo.CardSaleInfo.cardData.sGRKeyCode[0]		== '?') ||
					((savedInfo.CardSaleInfo.cardData.sTranType <= '9')	&& (savedInfo.CardSaleInfo.cardData.sTranType >= '1'))    ||
					(savedInfo.CardSaleExtraData2.sUnit[0] == '?')		  	    ||
					(savedInfo.CardSaleExtraData2.sTripNumber[0] == '?')        || 
					(savedInfo.CardSaleExtraData2.sCustomerReference[0] == '?') ||
					(savedInfo.CardSaleExtraData2.sTractorHub[0] == '?')        ||
					(savedInfo.CardSaleExtraData2.sTrailerNumber[0] == '?')     ||
					(savedInfo.CardSaleExtraData2.sTrailerHub[0] == '?')        ||
					(savedInfo.CardSaleInfo.extraData.sTotalAmt[0] == '?')      ||
					(( savedInfo.CardSaleExtraData2.sDeclinedLoyaltyPrompt >= '1') || ( savedInfo.CardSaleExtraData2.sDeclinedLoyaltyPrompt <= '3')) ||
					(savedInfo.CardSaleExtraData2.sPassCode[0] == '?') ||
					(savedInfo.CardSaleExtraData3.sParamResult[0] == '?') ||
					(savedInfo.CardSaleExtraData3.sRemoteID[0] == '?') ||
					(savedInfo.CardSaleExtraData3.sLogicShiftNumber[0] == '?') ||
					(savedInfo.CardSaleExtraData3.sCVMResult[0] == '?')        ||  //4.0.12.517
					(savedInfo.CardSaleExtraData3.sCombinedCardSelect == '?')  ||
					(savedInfo.CardSaleExtraData3.sTermResponseCode[0] == '?')  ||
					(savedInfo.CardSaleExtraData4.sPumpTestPassword[0] == '?') || //4.0.28.260
					(savedInfo.CardSaleInfo.cardData.sClubCardTrack2Data[0] == '?') ||
					(savedInfo.CardSaleExtraData5.ExtraEMVDataReq == '?')
					)

				{
						bReSendExtra = TRUE;
				}
				else if (savedInfo.CardSaleInfo.cardData.sClubCardTrack2Data[0] == '?')  //mark testing
				{
					bReSendExtra = TRUE;
				}
			}					
		}
	}
	return bReSendExtra;



}

BOOL CGCI::ReSendWithExtraInfoEx(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CXMLInterface* pCardData)
{
	BOOL bReSendExtra = FALSE;


	if((OLA_STATE(pOla->m_byState) == OLA_WAIT_FOR_MORE_INFO)||
		(OLA_STATE(pOla->m_byState) == OLA_LOYALTY_WAIT_RESPONSE))//4.0.9.508
	{	 // driver #, Odometer #, 

		long lLastSeqNumber = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPSeqNumber();
		if (lLastSeqNumber == lSeqNumber)
		{ 
			PAY_AT_PUMP_INFO savedInfo; //4.0.5.0.
			_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&savedInfo);

			char sTrack2Val[MAX_FIELD_VAL]; 
			memset(sTrack2Val,' ',sizeof(sTrack2Val));
			pCardData->GetXmlField(ATT_TRACK2_DATA,sTrack2Val);
			if(!memcmp(savedInfo.CardSaleInfo.cardData.sTrack2Data,sTrack2Val,sizeof(savedInfo.CardSaleInfo.cardData.sTrack2Data)))
			{
				if( (savedInfo.CardSaleExtraData2.sAccountType			== '?') ||//emv costco
					(savedInfo.CardSaleInfo.cardData.sOdometer[0]		== '?') ||
					(savedInfo.CardSaleInfo.cardData.sFullVehicleId[0]	== '?') ||
					(savedInfo.CardSaleInfo.cardData.sPINBlock[0]		== '?') ||
					(savedInfo.CardSaleInfo.cardData.sReturnTank			== '?') ||
					(savedInfo.CardSaleInfo.cardData.sDriverId[0]		== '?') ||
					(savedInfo.CardSaleInfo.cardData.sGRPrompt			== '?') ||
					(savedInfo.CardSaleInfo.cardData.sGRVoucherNumber[0]	== '?') ||
					(savedInfo.CardSaleInfo.cardData.sGeneralID[0]		== '?') ||
					(savedInfo.CardSaleInfo.cardData.sGRKeyCode[0]		== '?') ||
					((savedInfo.CardSaleInfo.cardData.sTranType <= '9')	&& (savedInfo.CardSaleInfo.cardData.sTranType >= '1'))    ||
					(savedInfo.CardSaleExtraData2.sUnit[0] == '?')		  	    ||
					(savedInfo.CardSaleExtraData2.sTripNumber[0] == '?')        || 
					(savedInfo.CardSaleExtraData2.sCustomerReference[0] == '?') ||
					(savedInfo.CardSaleExtraData2.sTractorHub[0] == '?')        ||
					(savedInfo.CardSaleExtraData2.sTrailerNumber[0] == '?')     ||
					(savedInfo.CardSaleExtraData2.sTrailerHub[0] == '?')        ||
					(savedInfo.CardSaleInfo.extraData.sTotalAmt[0] == '?')      ||
					(( savedInfo.CardSaleExtraData2.sDeclinedLoyaltyPrompt >= '1') || ( savedInfo.CardSaleExtraData2.sDeclinedLoyaltyPrompt <= '3')) ||
					(savedInfo.CardSaleExtraData2.sPassCode[0] == '?') ||
					(savedInfo.CardSaleExtraData3.sParamResult[0] == '?') ||
					(savedInfo.CardSaleExtraData3.sRemoteID[0] == '?') ||
					(savedInfo.CardSaleExtraData3.sLogicShiftNumber[0] == '?') ||
					(savedInfo.CardSaleExtraData3.sCVMResult[0] == '?')        ||  //4.0.12.517
					(savedInfo.CardSaleExtraData3.sCombinedCardSelect == '?')  ||
					(savedInfo.CardSaleExtraData3.sTermResponseCode[0] == '?') ||
					(savedInfo.CardSaleExtraData5.ExtraEMVDataReq == '?')

					)

				{
						bReSendExtra = TRUE;
				}
			}					
		}
	}
	return bReSendExtra;



}


//4.0.20.503 66960
GCIRetCode CGCI::SwipeExtraInfo(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CARD_SALE_ALL3* pCardData, BOOL *pbAskUserToTryAgain) 
{
	PAY_AT_PUMP_INFO cTmpInfo;	
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);
	
	GCIRetCode retCode = GCI_OK;

	CString sMsg;
	sMsg.Format("CGCI::SwipeExtraInfo() - Pump %ld - SeqNumber %ld. OLA state = %s", 
				lPumpNumber, 
				lSeqNumber, 
				_LOGMSG.GetStateString(pOla->m_byState));
	_LOGMSG.LogMsg(sMsg);

	//4.0.18.501
	switch (OLA_STATE(pOla->m_byState))
	{
		case OLA_NONE:								
		case OLA_SEND_END:		 
		{
			*pbAskUserToTryAgain = TRUE;
			retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
		}
		break;
		
		case OLA_REQUEST_TO_SEND:
		case OLA_WAIT_RESPONSE:
		case OLA_REQUEST_TO_SEND_MORE:						
		{
			*pbAskUserToTryAgain = FALSE;
			retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
		}
		break;
		
		case OLA_WAIT_FOR_MORE_INFO: // driver #, Odometer #, 
		{
			long lLastSeqNumber = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPSeqNumber();
			if (lLastSeqNumber != lSeqNumber)
			{ 
				pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_SEND_END);								
				if(LOG_BASIC_FLOW_CONTROL)
					_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
				*pbAskUserToTryAgain = TRUE;								
				retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
			}
			else
			{
				// Contructing and Saving PAY_AT_PUMP_INFO
				//----------------------------------------
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO,lPumpNumber,lSeqNumber,pCardData);	//4.0.20.503 66960
				if(lRtc)
				{		
					// Call swipe result with summerized code relating 
					// to the requested session, and not relatin to the current pOla->byStat									
					pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_SEND_END);
					if(LOG_BASIC_FLOW_CONTROL)
						_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
					retCode = GCI_FAILED_IO;
				}
				else
				{
					if(pCardData->CardSaleAll.data.sGRPrompt == 'N') //4.0.22.506 75407 answer to Gas Reward Voucher is No 
					{
						
						// Send OK result to the Pump.
						//---------------------------
						_Module.m_server.SendSwipeResult(pOla->m_byState,lPumpNumber, CD_OK,CD_APPROVE_OK, pCardData);	//4.0.20.503	66960

						// Save the "Approved" in memory, and file 
						//-----------------------------------------
						_Module.m_server.SaveCardSaleData(lPumpNumber, pCardData);	//4.0.20.503 66960
						
						// End current session
						//--------------------									
						pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_SEND_END);									
					}
					else
					{
						// Trigger OLA process
						//--------------------									
						pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_REQUEST_TO_SEND_MORE);									
						if(LOG_BASIC_FLOW_CONTROL)
							_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
					}
				}
			}
		}
		break;
		//amram 
		//in case rtaps we have to start session
		case OLA_WAIT_CONTROLLER_RESPONSE: // driver #, Odometer #, 
		{
			break;
		}
		case OLA_WAIT_CONTROLLER_MSG:
		{
			switch (pCardData->extData3.sParamResult[0])
			{
			case 'Y':
				{
					if(FALSE == _Module.m_server.m_cFlowControlManager.GetEmeraldLoyaltyOnlineFlag())  // Emerald loyalty serice is offline  RFUEL-520
					{
						//Continue the flow from authorize loyalty
						pOla->m_byState = OLA_LOYALTY_RUN_QUERY;
						_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);
						_Module.m_server.SavePumpInfo(lPumpNumber);
						_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Emerald loyalty service is offline. Cancel was selected by the user");
						_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, FALSE);
					}
					else
					{
						_Module.m_server.m_cFlowControlManager.StartAfterAddMemberFlowControl(lPumpNumber, pOla, 0);	//4.0.22.504 69344 
					}
				
					break;
				}
			default:
				{
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Cancel was selected by the user");
					_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber);
					PAY_AT_PUMP_INFO cTmpInfo;
					pOla->m_byState = OLA_NONE;
					
					_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);

					cTmpInfo.CardSaleExtraData2.sDisplayOLAResultMessage = '0';

					_Module.m_server.m_cFuelLoayaltyPS.CancelLoyaltyTrs(cTmpInfo, lPumpNumber, *pOla);
					break;
				}
				
			}
			break;
		}
		case OLA_LOYALTY_WAIT_CONTROLLER_MSG_START_TICKET_FAIL:	//4.0.22.504 69344
		{
			switch (pCardData->extData3.sParamResult[0])
			{
			case 'Y':

				if (FALSE == _Module.m_server.m_cFlowControlManager.GetEmeraldLoyaltyOnlineFlag())  // Emerald loyalty serice is offline  RFUEL-520
				{
					//Continue the flow from authorize loyalty
					pOla->m_byState = OLA_LOYALTY_RUN_QUERY;
					_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);
					_Module.m_server.SavePumpInfo(lPumpNumber);
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Emerald loyalty service is offline. Cancel was selected by the user");
					_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, FALSE);
				}
				else
				{
					_Module.m_server.m_cFlowControlManager.StartAfterStartTicketFlowControl(lPumpNumber, pOla, 0); 
				}
				break;
			default:
			{
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Cancel was selected by the user");
				_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber);
				PAY_AT_PUMP_INFO cTmpInfo;
				pOla->m_byState = OLA_NONE;

				_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);

				_Module.m_server.m_cFuelLoayaltyPS.CancelLoyaltyTrs(cTmpInfo, lPumpNumber, *pOla);

				break;
			}

			}
			break;
		}
		case OLA_LOYALTY_WAIT_MORE_DATA:
		{
			
			break;
		}
		//Copient 55286 Start
		case OLA_LOYALTY_CW_BEFORE_FUELING_BEFORE_START_TICKET:
		case OLA_LOYALTY_CW_BEFORE_FUELING_AFTER_START_TICKET:
		case OLA_LOYALTY_CW_AFTER_FUELING_BEFORE_ADD_ITEM:
		case OLA_LOYALTY_CAR_WASH:	//4.0.22.504 69344
		case OLA_LOYALTY_PROMPT_BEFORE_FUELING_BEFORE_START_TICKET:
		case OLA_LOYALTY_PROMPT_BEFORE_FUELING_AFTER_START_TICKET:
		case OLA_LOYALTY_PROMPT_AFTER_FUELING_BEFORE_ADD_ITEM:
		{
			PAY_AT_PUMP_INFO cTmpInfo;
			
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO,lPumpNumber,lSeqNumber,pCardData);	//4.0.20.503 66960
			_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);
			_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, TRUE);
			
		}
		break;
		case OLA_LOYALTY_MEMBER_CARD_BEFORE_FUELING_BEFORE_START_TICKET:
		case OLA_LOYALTY_MEMBER_CARD_BEFORE_FUELING_AFTER_START_TICKET:
		case OLA_LOYALTY_MEMBER_CARD_AFTER_FUELING_BEFORE_ADD_ITEM:
		case OLA_LOYALTY_SWIPE_LOYALTY:	//4.0.22.504 69344
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData);	//4.0.20.503 66960

			PAY_AT_PUMP_INFO cTmpInfo;
			CString str;
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);
			str.Format("SwipeExtraInfo(OLA_LOYALTY_SWIPE_LOYALTY) - sEntryMethod %c, sParamResult %c",
				cTmpInfo.CardSaleInfo.cardData.sEntryMethod,
				cTmpInfo.CardSaleExtraData3.sParamResult[0]);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

			switch (cTmpInfo.CardSaleInfo.cardData.sEntryMethod)	//4.0.24.500 122327
			{
				case 'W':
				case 'S':	//Swipe
				{
					long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, pOla, &cTmpInfo, FALSE, TRUE); //4.0.20.440 68857 //4.0.21.501 //4.0.23.240

					BOOL bSkipRepeatOption = TRUE;
					if (cTmpInfo.CardSaleExtraData5.sLoyaltyStatus != CD_APPROVE_OK)
					{
						bSkipRepeatOption = FALSE;
					}
					str.Format("SwipeExtraInfo() - lMediaMasterType %ld, sTranStatus %c, bSkipRepeatOption %d",
						lMediaMasterType,
						cTmpInfo.CardSaleExtraData5.sLoyaltyStatus,
						bSkipRepeatOption);
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

					//Save loyalty card number (numeric)
					if ((lMediaMasterType != CARD_TYPE_LOYALTY) && (lMediaMasterType != CARD_TYPE_LOYALTY_AND_CREDIT))//4.0.23.390 116544
					{
						_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_UNKNOWN_ERROR);
						_Module.m_server.m_cFuelLoayaltyPS.HandleAddMemberCardResult(lPumpNumber, pOla, LOYALTY_STATE_UNKNOWN_ERROR);
						// _LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Card type is not loyalty, declined by PumpSrv");
						_LOGMSG.LogMsg("GCI", "SwipeExtraInfo", lPumpNumber, LOG_PUMP, "Card type is not loyalty, declined by PumpSrv");
					}
					else
					{
						_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, bSkipRepeatOption);
					}
				}
				break;
				default:
				{
					switch (cTmpInfo.CardSaleExtraData3.sParamResult[0])
					{
						case 'Y':
						{
							if (_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsNumericAllowedInSwipeLoyalty())	//4.0.23.410 120603
							{
								//Manual NMP
								_Module.m_server.m_cFlowControlManager.HandleNMPManualLoyalty(lPumpNumber, pOla, &cTmpInfo);
								break;
							}
							else if (!_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsNumericAllowedInSwipeLoyalty())
							{
								_Module.m_server.m_cFlowControlManager.HandleNMPManualLoyalty(lPumpNumber, pOla, &cTmpInfo);  //4.0.23.1380
								break;   //4.0.23.1410
							}

							break;
							//else act like 'N' //4.0.23.410 120603
						}
						//4.0.23.420	120603 break;
						case 'N':
						case 'B':	// 4.0.23.410  120603
						// case 'T':	// RFUEL-281
						{
							//Continue to next state

							BOOL bStartFueling = FALSE;

							if (_Module.m_server.m_cFuelLoayaltyPS.m_dwLoyaltyEngineType == EPSILON)
							{
								bStartFueling = TRUE;
							}

							str.Format("SwipeExtraInfo() - Skip to start fueling? bStartFueling %d", bStartFueling);
							_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

							_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, TRUE, bStartFueling);
						}
						break;
						case 'T':	// RFUEL-3705
						case 'J': //4.0.23.481 126185
						{
							_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, &cTmpInfo);
						}
						break;
						case 'A': // UDF - Epsilon
						{
							_Module.m_server.m_cFlowControlManager.HandleNMPAlternateID(lPumpNumber, pOla, &cTmpInfo);
						}
						break;
						default:
						{
							long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, pOla, &cTmpInfo, FALSE, TRUE); //4.0.20.440 68857	//4.0.21.501 //4.0.23.240

																													//Save loyalty card number (numeric)
							if ((lMediaMasterType != CARD_TYPE_LOYALTY) && (lMediaMasterType != CARD_TYPE_LOYALTY_AND_CREDIT))//4.0.23.390 116544
							{
								_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_UNKNOWN_ERROR);
								_Module.m_server.m_cFuelLoayaltyPS.HandleAddMemberCardResult(lPumpNumber, pOla, LOYALTY_STATE_UNKNOWN_ERROR);
								// _LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Card type is not loyalty, declined by PumpSrv");
								_LOGMSG.LogMsg("GCI", "SwipeExtraInfo", lPumpNumber, LOG_PUMP, "Card type is not loyalty, declined by PumpSrv");
							}
							else
							{
								_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo);
							}
						}
					}
				}
			}
		}
		break;
		case OLA_LOYALTY_COUPONS_AFTER_FUELING_BEFORE_ADD_ITEM:
		case OLA_LOYALTY_COUPONS_BEFORE_FUELING_AFTER_START_TICKET:
		case OLA_LOYALTY_COUPONS_BEFORE_FUELING_BEFORE_START_TICKET:
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO,lPumpNumber,lSeqNumber, pCardData);	//4.0.20.503 66960
			PAY_AT_PUMP_INFO cTmpInfo;
			_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);
	
			_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, FALSE);
			
		}
		break;
		//4.0.20.54 64528
		case OLA_LOYALTY_DEFERRAL_AFTER_FUELING_BEFORE_ADD_ITEM:
		case OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET:
		case OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_BEFORE_START_TICKET:
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData);	//4.0.20.503 66960
			_Module.m_server.m_cFlowControlManager.HandleNMPDeferralDiscount(lPumpNumber, pOla);
		}
		break;
		//Copient 55286 End
		//4.0.22.500 start 70767
		case OLA_LOYALTY_SWIPE_PAYMENT:
		{
			std::string sParamResult;

			//
			// Load pCardData->extData3.sParamResult with Track1 | Track2
			// Only if NeedMoreParam option is turned on
			//
			CNMPDetails* pSwipePaymentNMPDetail = nullptr;

			// find SwipePaymentNMPDetail			
			CBaseFlow* pPumpFlow = _Module.m_server.GetFlowControlManager().GetPumpFlow(lPumpNumber);
			if (pPumpFlow != nullptr) {
				pPumpFlow->FindCurrentNMPSession(OLA_LOYALTY_SWIPE_PAYMENT, &pSwipePaymentNMPDetail);
			}

			const BOOL bUseNMPOption = (pSwipePaymentNMPDetail ? pSwipePaymentNMPDetail->GetUseNMPOption() : FALSE);
			if (bUseNMPOption == TRUE)
			{
				//_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "bUseNMPOption");

				// convert the two Tracks to std::string data type
				std::string Track1((char*)pCardData->CardSaleAll.data.sTrack1Data,
					(char*)pCardData->CardSaleAll.data.sTrack1Data + TRACK_1_DATA_SIZE);

				std::string Track2((char*)pCardData->CardSaleAll.data.sTrack2Data,
					(char*)pCardData->CardSaleAll.data.sTrack2Data + TRACK_2_DATA_SIZE);

				// right trim the data
				Track1 = string_right_trim(Track1, " ");
				Track2 = string_right_trim(Track2, " ");

				// Construct a new sParamResult by separating the tracks with | 
				sParamResult = Track1 + '|' + Track2;
			}
			else {
				;
				//_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "bUseNMPOption=FALSE");
			}

			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO, lPumpNumber, lSeqNumber, pCardData);	//4.0.20.503 66960
			PAY_AT_PUMP_INFO cTmpInfo;
			_Module.m_server.GetPAPInfo(lPumpNumber, cTmpInfo);

			// if we have processed sParameterResult, then copy the data into the
			// proper location
			if (sParamResult.length()) {
				cTmpInfo.CopyCardSaleAll3(pCardData);

				memset(cTmpInfo.CardSaleExtraData3.sParamResult,
					' ', sizeof(cTmpInfo.CardSaleExtraData3.sParamResult));
				memcpy(cTmpInfo.CardSaleExtraData3.sParamResult,
					sParamResult.c_str(),
					sParamResult.length());
			}

			HandleLoyaltyResult(lPumpNumber, pOla, &cTmpInfo, FALSE, FALSE); //4.0.22.504 69344 //4.0.24.60
			_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, TRUE);
		}
		break;
		
		case OLA_LOYALTY_SWIPE_CARD:
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO,lPumpNumber,lSeqNumber, pCardData);	//4.0.20.503 66960
			PAY_AT_PUMP_INFO cTmpInfo;
			_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);
			
			//HandleLoyaltyResult(lPumpNumber, pOla, &cTmpInfo, TRUE,FALSE); 
			HandleLoyaltyResult(lPumpNumber, pOla, &cTmpInfo, FALSE,FALSE); 
			_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, TRUE);

		}
		break;
		
		case OLA_LOYALTY_METHOD_OF_PAYMENT: //4.0.23.20 76383
		{
			BOOL bUserPressedCanceled = FALSE;
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);	
			
			switch (cTmpInfo.CardSaleExtraData3.sParamResult[0])
			{
				case 'Y':
				case 'I':	//4.0.23.860 212541
				{
					_Module.m_server.m_cFlowControlManager.HandleNMPMethodOfPayment(lPumpNumber, pOla, &cTmpInfo, FALSE, TRUE);
					
				}
				break;
				
				case 'N':
				case 'T':	// RFUEL-281
				{
					//Pap to Pak requested	
					_Module.m_server.m_cFlowControlManager.HandleNMPMethodOfPayment(lPumpNumber, pOla, &cTmpInfo, TRUE, FALSE);
					
				}
				break;
				case 'J':	// RFUEL-492
				{
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Cancel was selected by the user!");
					_Module.m_server.SendSwipeResult(pOla->m_byState, lPumpNumber, CD_NOT_OK, CD_DECLINE_USEALT, (CARD_SALE_ALL3 *)&cTmpInfo.CardSaleInfo.cardData);
					bUserPressedCanceled = TRUE;
				}
				break;		
				default:
				{
					_Module.m_server.m_cFlowControlManager.HandleNMPMethodOfPayment(lPumpNumber, pOla, &cTmpInfo, FALSE, TRUE);
					HandleLoyaltyResult(lPumpNumber, pOla, &cTmpInfo, FALSE, FALSE); //4.0.22.504 69344 //4.0.24.60
				}
				break;
			}

			if (!bUserPressedCanceled)
				_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo);
		}
		break;

		case OLA_LOYALTY_DISPLAY_PRE_AUTHOIRZE_SCREEN:
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData);

			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);

			_Module.m_server.m_cFlowControlManager.HandlePreAuthoitzeScreen(lPumpNumber, pOla, cTmpInfo, pCardData);
			break;
		}
		case OLA_LOYALTY_PHONE_LOOKUP: // 4.0.23.1220 TD 155754 start
			{
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);	
			
				_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);
			
				HandleLoyaltyPhoneLookUp(lPumpNumber,lSeqNumber,pOla , cTmpInfo , pCardData);	
				break;
			}
		case OLA_LOYALTY_PHONE_LOOKUP_ENTER_PHONE_NUMBER:
			{
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);	
			
				_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);
			
				_Module.m_server.m_cFlowControlManager.HandleNMPGetPhoneNumber(lPumpNumber, pOla, &cTmpInfo);
				break;
			}
		case OLA_LOYALTY_PHONE_LOOKUP_ENTER_REWARD_CARD:
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);
			
			_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);
			
			_Module.m_server.m_cFlowControlManager.HandleNMPGetRewardCard(lPumpNumber, pOla, &cTmpInfo);
			
			break;
		}
		//4.0.23.???? TD 295237
		case OLA_LOYALTY_WAIT_FOR_DEFERAL_ANSWER:
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);
			
			_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);
			
			_Module.m_server.m_cFlowControlManager.HandleManualDeferal(lPumpNumber, pOla, &cTmpInfo);
			
			break;
		}
		case OLA_LOYALTY_ALTERNATE_ID:
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO, lPumpNumber, lSeqNumber, pCardData);	//4.0.20.503 66960

			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);

			switch (cTmpInfo.CardSaleExtraData3.sParamResult[0])
			{
				case 'J':
				case 'T':	// RFUEL-281
				{
					_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, &cTmpInfo);
				}
				break;
				default:
				{
					long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, pOla, &cTmpInfo, FALSE, TRUE);

					_Module.m_server.m_cFlowControlManager.HandleNMPAlternateID(lPumpNumber, pOla, &cTmpInfo);
				}
				break;
			}
		}
		break;
		case OLA_LOYALTY_ALTERNATE_ID_MANUAL_ENTER:
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO, lPumpNumber, lSeqNumber, pCardData);

			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);

			CString str;
			str.Format("SwipeExtraInfo(OLA_LOYALTY_ALTERNATE_ID_MANUAL_ENTER) - sEntryMethod %c, sParamResult %c", 
				cTmpInfo.CardSaleInfo.cardData.sEntryMethod,
				cTmpInfo.CardSaleExtraData3.sParamResult[0]);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

			cTmpInfo.CardSaleExtraData4.sLoyaltyEntryMethod = 'A';

			// CR 501164
			switch (cTmpInfo.CardSaleExtraData3.sParamResult[0])
			{
				case 'J':
				case 'N':
				case 'T':	// RFUEL-281
				{
					_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, &cTmpInfo);
				}
				break;
				default:
				{
					long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, pOla, &cTmpInfo, TRUE, FALSE);

					_Module.m_server.m_cFlowControlManager.HandleNMPAlternateIDManualEntry(lPumpNumber, pOla, &cTmpInfo);
				}
				break;
			}
		}
		break;
		case OLA_LOYALTY_ALTERNATE_ID_SWIPE_LOYALTY_CARD:
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO, lPumpNumber, lSeqNumber, pCardData);

			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);

			long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, pOla, &cTmpInfo, FALSE, TRUE);

			_Module.m_server.m_cFlowControlManager.HandleNMPAlternateIdSwipeLoyaltyCard(lPumpNumber, pOla, &cTmpInfo);
			break;
		}
		case OLA_LOYALTY_WAIT_FOR_ALTERNATE_ID_RETRY_ANSWER:  //
		{
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO, lPumpNumber, lSeqNumber, pCardData);

			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);

			switch (cTmpInfo.CardSaleExtraData3.sParamResult[0])
			{
				case 'A':
					cTmpInfo.CardSaleExtraData6.sIdRetryCount = (cTmpInfo.CardSaleExtraData6.sIdRetryCount <= '0') ? '1' : (cTmpInfo.CardSaleExtraData6.sIdRetryCount + 1);
					_Module.m_server.m_cFlowControlManager.HandleNMPAlternateID(lPumpNumber, pOla, &cTmpInfo);
					break;
				case 'P':
					cTmpInfo.CardSaleExtraData6.sIdRetryCount = '0';
					_Module.m_server.m_cFlowControlManager.HandleNMPAlternateID(lPumpNumber, pOla, &cTmpInfo);
					break;
				case 'J': // RFUEL-137
				case 'N': // RFUEL-137
				case 'T': // RFUEL-281
				{
					_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, &cTmpInfo);
				}
				break;

				default:
				{
					_Module.m_server.m_cFlowControlManager.HandleNMPAlternateID(lPumpNumber, pOla, &cTmpInfo);
					break;
				}
			}
			_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, 0);
			break;
		}
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, TRUE);
	}	
	return retCode;
}



/******************************************************************************
 Description:	This function checks if a valid mode exists for a payment card in the pump.

 Returns:      	TRUE - a valid mode exists for a payment card (OR card is NOT a payment card).
                FALSE - card is a payment card, and a valid mode don't exist for a payment card.

 Parameters:    lPumpNumber - pump number.
                lMediaMasterType - card type.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		10/12/2008   18:15		Start 4.0.21.501 TD 67443
 AmitH          07/06/2010   13:00      TD 126014 - In case the res ext is PATT already, allow swiping payment cards although
										PAP is not valid in current mode. 4.0.25.30
******************************************************************************/
BOOL CGCI::IsValidModeForPayment(const long lPumpNumber, const long lMediaMasterType)
{
	BOOL bRetVal = TRUE;
	CPumpStatus stat;
	stat = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();

	if (_Module.m_server.m_cCommonFunction.IsPaymentCard(lMediaMasterType))
	{
		if (MODE_VALID(stat.GetMode()) && (_Module.m_server.m_cModeArray[stat.GetMode() - 1].PAPNotValid()) &&
			(stat.GetResExt() != RES_EXT_ATTENDANT))  //do not eliminate if we are in RES_EXT_ATTENDANT 4.0.25.30
		{ 
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("CGCI::IsValidModeForPayment - payment card was swiped, but PAP mode not valid !");
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 
			}

			bRetVal = FALSE;
		}

	}

	return bRetVal;
}

/******************************************************************************
 Description:	This function checks if a valid PAP mode exists for the pump.

 Returns:      	TRUE - a valid PAP mode exists for the pump.
                FALSE - a valid PAP mode exists for the pump.

 Parameters:    lPumpNumber - pump number.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		10/12/2008   18:15		Start 4.0.21.501 TD 67443
******************************************************************************/
BOOL CGCI::IsValidPAPMode(const long lPumpNumber)
{
	BOOL bRetVal = TRUE;
	long lMode = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetMode();
	if (MODE_NOT_VALID(lMode))
		return FALSE;

	if ((_Module.m_server.m_cModeArray[lMode - 1].PAPNotValid()) &&
		(_Module.m_server.m_cModeArray[lMode - 1].PayAttendNotValid()) &&
		(_Module.m_server.m_cModeArray[lMode - 1].PayAVINotValid()))
	{
		if (LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("CGCI::IsValidPAPMode - No PAP mode is valid !");
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
		}

		bRetVal = FALSE;
	}

	return bRetVal;
}

//4.0.20.503 66960
//************************************
// Method:    SwipeRequest
// FullName:  CGCI::SwipeRequest
// Access:    protected 
// Returns:   GCIRetCode
// Qualifier:
// Parameter: long lPumpNumber
// Parameter: long lSeqNumber
// Parameter: OLA_STAT * pOla
// Parameter: CARD_SALE_ALL3 * pCardData
// Parameter: BOOL * pbAskUserToTryAgain
// Parameter: long lFlags
//************************************
GCIRetCode CGCI::SwipeRequest(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CARD_SALE_ALL3* pCardData, BOOL *pbAskUserToTryAgain, long lFlags)
{
#ifdef _DEBUG
	LOG_MSG_START_METHOD("SwipeRequest");
	CString strLog;
	strLog.Format("OlaState:[%ld]", pOla->m_byState);
	LOG_MSG_FUNCINC(strLog);
#endif // _DEBUG
	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();
	GCIRetCode retCode = GCI_OK;
	//4.0.20.503 66960
	BOOL bClockSession = (!ChrAll((char *)pCardData->extData2.sClockFlags,sizeof(pCardData->extData2.sClockFlags)))? TRUE : FALSE;  //4.0.6.505
	PAY_AT_PUMP_INFO cInfo;
	memcpy(&cInfo.CardSaleInfo.cardData, pCardData, sizeof(CARD_SALE_ALL3));

	long lMasterType = a2l(pCardData->extData2.sMediaMasterType,sizeof(pCardData->extData2.sMediaMasterType));

	//4.0.21.502 change condition for TD 67443
	if ((PUMPSRV_IS_MODE_MASTER) && ((!IsValidPAPMode(lPumpNumber)) || (!IsValidModeForPayment(lPumpNumber, lMasterType))))
	{
	//4.0.25.0 125621	if(!(lFlags & SWIPE_SALE_COMMAND))//4.0.12.501
		{
			//4.0.3.25
			retCode =  GCI_NOT_VALID_IN_CURRENT_MODE;
			if (!bClockSession) //4.0.6.505 
			{ //4.0.25.0 125621
				_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cInfo);  //
				cInfo.CardSaleInfo.cardData.sTranStatus = ' ';
				_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cInfo);  ////4.0.23.470
				SQLPumpSrvFile.LoadPAPInfo(lPumpNumber);
				_Module.m_server.SendSwipeResult(SESSION_AUTHORIZE, lPumpNumber, CD_NOT_OK,CD_DECLINE_MODE, pCardData);													

			}
			_Module.m_server.m_cCommonFunction.HandleMiscCardNotAuthorized(lPumpNumber);
		}
	}
	else if ( (pCardData->CardSaleAll.data.sTranType == CD_TYPE_AVI) &&
		(!_Module.m_server.AllowParallelAVIUsage())			 &&
		AVIAlreadyInUse(pCardData, lPumpNumber))
	{
		retCode =  GCI_PARALLEL_AVI_NOT_ALLOWED;
		//		__Decline
		if (!bClockSession) //4.0.6.505
			_Module.m_server.SendSwipeResult(SESSION_AUTHORIZE,lPumpNumber, CD_NOT_OK,CD_DECLINE_MODE, pCardData);													
	} // 4.0.2.15
	 else if ((_Module.m_server.m_cInitDebitHandler.GetStatus(lPumpNumber) == DEBIT_SERVER_INPROCESS) && _Module.m_server.UseSingleSession())
	{
		retCode =  GCI_INIT_DEBIT_IN_PROCESS;
	}

	//3.2.0.69
	if(retCode == GCI_OK)//4.0.12.501
	{
		switch (OLA_STATE(pOla->m_byState))
		{
			case OLA_WAIT_FOR_MORE_INFO: // Second card 
			{
				long lLastSeqNumber = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPSeqNumber();
				if (lLastSeqNumber == lSeqNumber)
				{ 
					//4.0.18.501
					PAY_AT_PUMP_INFO savedInfo;
					_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&savedInfo);
					
					CString sSiteID = _Module.m_server.GetSiteID(); //4.0.17.501 merge from 4.0.15.200

					memcpy(pCardData->extData2.sSiteId, (LPCTSTR)sSiteID ,  min(sSiteID.GetLength() ,sizeof(pCardData->extData2.sSiteId))); //4.0.20.503 66960

					// check if expecting for second card 
					if((savedInfo.CardSaleInfo.cardData.sTrack2Data[0] == '?') || ((savedInfo.CardSaleInfo.cardData.sTranType >= '1') && (savedInfo.CardSaleInfo.cardData.sTranType <= '9')) )
					{
						if(_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())
							pOla->m_byState = OLA_LOYALTY_START_TICKET | SESSION_AUTHORIZE;

						// new track2 
						retCode = GCI_OK; // it looks like i got payment card , 
										  // the previous was loyalty card															
					}
					else
					{
						pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_SEND_END);
						if(LOG_BASIC_FLOW_CONTROL)
							_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState, pOla->m_lSessionNumber);
						retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
						*pbAskUserToTryAgain = TRUE;
						
						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;
							str.Format("CGCI::SwipeRequest GCI_PAP_REQUEST_OUT_OF_SYNC 1 !");
							_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str); 
						}						

					}
				}
				else
				{	// in middle of another OLA session.
					// cancel current session.
					pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_SEND_END);
					if(LOG_BASIC_FLOW_CONTROL)
						_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
					*pbAskUserToTryAgain = TRUE;
					retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
										
					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("CGCI::SwipeRequest GCI_PAP_REQUEST_OUT_OF_SYNC 2 !");
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 
					}	

				}				

				// Constructing and Saving PAY_AT_PUMP_INFO
				//----------------------------------------
				if(retCode == GCI_OK)
				{
					// This case should be treated as a "Extra info" and not original request
					// All CardData fields already Contracted.
					
					long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO, lPumpNumber, lSeqNumber, pCardData, 2, FALSE);	//4.0.20.56 //4.0.20.503 66960
					if(lRtc)
					{	
						// Call swipe result with summerized code relating 
						// to the requested session, and not relatin to the current pOla->byStat																				
						pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_SEND_END);
						if(LOG_BASIC_FLOW_CONTROL)
							_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
						retCode = GCI_FAILED_IO;
					}
					else
					{
						// Trigger OLA process
						//--------------------
						pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_REQUEST_TO_SEND_MORE);
						if(LOG_BASIC_FLOW_CONTROL)
							_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
					}
				}
			}
			break;

			case OLA_NONE: // first card 
			{									
				long lLastSeqNumber = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPSeqNumber();

				if (lFlags & SWIPE_PAK_TO_PAP)
				{
					PakToPapSwipe(pOla, pCardData, lPumpNumber, lSeqNumber);

				}
				else if( (lLastSeqNumber != lSeqNumber) && (_Module.m_server.m_cPumpArray[lPumpNumber-1].IsCurrentTrs()) )  
				{ 
					*pbAskUserToTryAgain = TRUE;
					retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("CGCI::SwipeRequest GCI_PAP_REQUEST_OUT_OF_SYNC 3 !");
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 
					}	
				}
				else
				{
					CPumpTransact cTmptrs;

					CString sSiteID = _Module.m_server.GetSiteID();
				    //4.0.1.37
					if( _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsByStatus(WAIT_PROGRESS,TRS_NO_LINK,&cTmptrs))
					{	
						long lRtc =0;
						int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_INTERNAL,lPumpNumber, cTmptrs.m_lNumber, &lRtc);			
						if(LOG_BASIC_FLOW_CONTROL)
						{											
							CString str;
							str.Format("Trs %d canceled by pumpsrv while conflict PAP with trs (return %d)",cTmptrs.m_lNumber,iRetNum);
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
						}

						CPumpStatus stat;
							
							// waiting = WAIT_RES,WAIT_AUTH,WAIT_PROGRESS,WAIT_TIME_OUT 
							stat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();
						if (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag() && //4.0.23.480 126199 //4.0.25.30
							((stat.GetRes() == PAY_AT_KIOSK) || ((stat.GetRes() == PAY_AT_PUMP) && (stat.GetResExt() == RES_EXT_PAK_LOYALTY))))
						{
							char strMsg[MAX_FIELD_VAL];
							sprintf_s(strMsg,MAX_FIELD_VAL, "Illegal swipe was received. CD_NOT_OK result is returned");
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg); 
							_Module.m_server.SendSwipeResult(SESSION_AUTHORIZE,lPumpNumber, CD_NOT_OK,CD_DECLINE_MODE, pCardData);	
							retCode = GCI_REPETED_PAP_SWIPE_REQUEST;
							break;
							
						}	
					}					

					if(lFlags & SWIPE_SALE_COMMAND)
					{
						long lTmp=0;
						lTmp |= FL_SALE_REC; //Add flag to sale trs
						l2a(lTmp,pCardData->extData3.sFlags1,sizeof(pCardData->extData3.sFlags1));
					}
					memcpy(pCardData->extData2.sSiteId, (LPCTSTR)sSiteID ,  min(sSiteID.GetLength() ,sizeof(pCardData->extData2.sSiteId))); 

 					long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData); //4.0.20.503 66960
					if(lRtc)
					{						
						// Call swipe result with summarized code relating 
						// to the requested session, and not related to the current pOla->byStat									
						retCode = GCI_FAILED_IO;								
					}
					else
					{
						// Trigger OLA process
						//--------------------
						//4.0.12.501
						pOla->m_lSessionNumber = 0;
						if(lFlags & SWIPE_SALE_COMMAND)
						{
							CString str;
							str.Format("CGCI::SwipeRequest found car wash flag.start SALE session");
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 				
							pOla->m_byState = OLA_REQUEST_TO_SEND | SESSION_SALE;
							
						}
						else
						{
							LOYALTY_FLOW_TYPE eLoyaltyFlowType;

							eFuelLoyaltyState eLoyaltyFuelState = GetLoyaltyState(pCardData,lFlags,lPumpNumber);	//4.0.22.504 69344
								
							short nOlaState = ConvertLoyaltyStatusToOlaStatus(pOla, pCardData, eLoyaltyFuelState, lPumpNumber,eLoyaltyFlowType);

							if (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag() && (eLoyaltyFuelState != PAP_PaymentFirst_Loyalty_swiped) && (eLoyaltyFuelState != PAK_Payment_Swiped)) //4.0.22.508 74810
							{
								_Module.m_server.m_cFlowControlManager.StartFlowControl(lPumpNumber, nOlaState, *pOla, eLoyaltyFlowType);
							}		
							else if (CheckFirstManualCard(pCardData,lFlags,lPumpNumber))	//4.0.22.506 67786
							{
								ExecuteManualFirstSwipe(lPumpNumber, pCardData);
							}

							if(LOG_BASIC_FLOW_CONTROL)
								_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
						}
					}
				}

			}
			break;

			case OLA_REQUEST_TO_SEND:
			case OLA_WAIT_RESPONSE:
			case OLA_REQUEST_TO_SEND_MORE:
			{
				long lLastSeqNumber = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPSeqNumber();
				if (lLastSeqNumber == lSeqNumber)
				{ 
					//4.0.18.501
					PAY_AT_PUMP_INFO savedInfo;
					_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&savedInfo);
					
					//4.0.20.503 66960
					if(memcmp(pCardData->CardSaleAll.data.sTrack2Data, savedInfo.CardSaleInfo.cardData.sTrack2Data,sizeof(savedInfo.CardSaleInfo.cardData.sTrack2Data)) != 0) 
					{
						// new track2 
						if(	(SESSION_TYPE(pOla->m_byState) == SESSION_AUTHORIZE) ||
							(SESSION_TYPE(pOla->m_byState) == SESSION_RECEIPT_AUTH) )
						{
							pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_SEND_END);
							if(LOG_BASIC_FLOW_CONTROL)
								_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
							*pbAskUserToTryAgain = TRUE;
							retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
							if(LOG_BASIC_FLOW_CONTROL)
							{
								CString str;
								str.Format("CGCI::SwipeRequest GCI_PAP_REQUEST_OUT_OF_SYNC 4 !");
								_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 
							}
						}
						else // in middle of force or receipt update 
						{
							*pbAskUserToTryAgain = TRUE;
							retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
							if(LOG_BASIC_FLOW_CONTROL)
							{
								CString str;
								str.Format("CGCI::SwipeRequest GCI_PAP_REQUEST_OUT_OF_SYNC 5 !");
								_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 
							}	
						}
					}
					else
					{
						// ignore repeted request.
						retCode = GCI_REPETED_PAP_SWIPE_REQUEST;
					}
				}
				else
				{	// in middle of another OLA session.
					// in case of authorize - cancel current session.
					if(	(SESSION_TYPE(pOla->m_byState) == SESSION_AUTHORIZE) ||
						(SESSION_TYPE(pOla->m_byState) == SESSION_RECEIPT_AUTH) )
					{
						pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_SEND_END);
						if(LOG_BASIC_FLOW_CONTROL)
							_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
						*pbAskUserToTryAgain = TRUE;
						retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
												
						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;
							str.Format("CGCI::SwipeRequest GCI_PAP_REQUEST_OUT_OF_SYNC 6 !");
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 
						}	

					}
					// in middle of another OLA session.
					// in case of force , update..
					else 
					{
						*pbAskUserToTryAgain = TRUE;
						retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;
							str.Format("CGCI::SwipeRequest GCI_PAP_REQUEST_OUT_OF_SYNC 7 !");
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 
						}	
					}
				}				
			}
			break;

			case OLA_SEND_END:		 
			{
				if (SESSION_TYPE(pOla->m_byState) == SESSION_INQUIRY)
				{
					retCode = GCI_SERVER_IN_THE_MIDDLE_OF_OLA_SESSION; //4.0.16.70
					*pbAskUserToTryAgain = FALSE;

					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("CGCI::SwipeRequest GCI_SERVER_IN_THE_MIDDLE_OF_OLA_SESSION SwipeResult will not be sent!");
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 
					}
				}
				else
				{
					retCode = GCI_PAP_REQUEST_OUT_OF_SYNC; //4.0.16.504 - merge from 4.0.16.81
					*pbAskUserToTryAgain = TRUE;
				}		
			}
			break;
					//amram 
		//in case rtaps we have to start session
			case OLA_WAIT_CONTROLLER_RESPONSE: // driver #, Odometer #, 
			{
				break;
			}
			case OLA_WAIT_CONTROLLER_MSG:
			{
				switch (pCardData->extData3.sParamResult[0])  //4.0.20.503 66960
				{
				case 'Y':
					{  //RFUEL-2673 PAP with loyalty card - Flow for Scan Member card when Loyalty is Offline is wrong
					if (FALSE == _Module.m_server.m_cFlowControlManager.GetEmeraldLoyaltyOnlineFlag()) // Emerald loyalty service is offline RFUEL-520
					{
						//Continue the flow from authorize loyalty
						PAY_AT_PUMP_INFO cTmpInfo;
						_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);
						pOla->m_byState = OLA_LOYALTY_RUN_QUERY;
						_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);
						_Module.m_server.SavePumpInfo(lPumpNumber);
						_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Emerald loyalty service is offline. Continue without loyalty was selected by the user");
						_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, FALSE);
					}
					else
						_Module.m_server.m_cFlowControlManager.StartAfterAddMemberFlowControl(lPumpNumber, pOla, 0);	//4.0.22.504 69344
					}
					break;
				default:
					{
						_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Cancel was selected by the user");
						_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber);

						pOla->m_byState = OLA_NONE;

						_Module.m_server.m_cFuelLoayaltyPS.CancelLoyaltyTrs(cInfo,lPumpNumber,*pOla);
						break;
					}

				}
				break;
			}
			case OLA_LOYALTY_WAIT_CONTROLLER_MSG_START_TICKET_FAIL:	//4.0.22.504 69344
			{
				switch (pCardData->extData3.sParamResult[0])
				{
				case 'Y':
					{
						_Module.m_server.m_cFlowControlManager.StartAfterStartTicketFlowControl(lPumpNumber, pOla, 0);
					}
					break;
				default:
					{
						_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Cancel was selected by the user");
						_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber);

						pOla->m_byState = OLA_NONE;
					
						_Module.m_server.m_cFuelLoayaltyPS.CancelLoyaltyTrs(cInfo,lPumpNumber,*pOla);
						break;
					}

				}
				break;
			}
			case OLA_LOYALTY_WAIT_MORE_DATA:
			{
				break;
			}
			//Copient 55286 Start 4.0.19.505
			case OLA_LOYALTY_CW_BEFORE_FUELING_BEFORE_START_TICKET:
			case OLA_LOYALTY_CW_BEFORE_FUELING_AFTER_START_TICKET:
			case OLA_LOYALTY_CW_AFTER_FUELING_BEFORE_ADD_ITEM:
			case OLA_LOYALTY_CAR_WASH:	//4.0.22.504 69344
			{
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber,pCardData); //4.0.20.503 66960
				_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo);
				
			}
			break;
			case OLA_LOYALTY_CARWASH_SALE:
				{
					long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber,pCardData); //4.0.20.503 66960
					_Module.m_server.m_cFlowControlManager.HandleNMPCarWashSale(lPumpNumber, pOla, &cInfo);
				}
				break;
			case OLA_LOYALTY_CARWASH_PROMOTION:
				{
					long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber,pCardData); //4.0.20.503 66960
					_Module.m_server.m_cFlowControlManager.HandleNMPCarWashPromotion(lPumpNumber, pOla, &cInfo);
				}
				break;
			case OLA_LOYALTY_MEMBER_CARD_BEFORE_FUELING_BEFORE_START_TICKET:
			case OLA_LOYALTY_MEMBER_CARD_BEFORE_FUELING_AFTER_START_TICKET:
			case OLA_LOYALTY_MEMBER_CARD_AFTER_FUELING_BEFORE_ADD_ITEM:
			case OLA_LOYALTY_SWIPE_LOYALTY:	//4.0.22.504 69344
			{
				//Repeat NMP is not allowed	
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData);	//4.0.20.503 66960

				CString str;
				str.Format("SwipeRequest(OLA_LOYALTY_SWIPE_LOYALTY) - sEntryMethod = %c, ParamResult[0] = %c ", cInfo.CardSaleInfo.cardData.sEntryMethod, cInfo.CardSaleExtraData3.sParamResult[0]);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);  //marikdebug 4.0.23.481

				switch (cInfo.CardSaleInfo.cardData.sEntryMethod)	//4.0.23.440 122327
				{
					case 'W':
					case 'S':	//Swipe
					{
						long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, pOla, &cInfo, FALSE, TRUE); //4.0.20.440 68857 //4.0.21.501 //4.0.23.240

						BOOL bSkipRepeatOption = TRUE;

						if (cInfo.CardSaleExtraData5.sLoyaltyStatus != CD_APPROVE_OK)
						{
							bSkipRepeatOption = FALSE;
						}

						str.Format("SwipeExtraInfo() - lMediaMasterType %ld, sTranStatus %c, bSkipRepeatOption %d", 
							lMediaMasterType, 
							cInfo.CardSaleExtraData5.sLoyaltyStatus, 
							bSkipRepeatOption);
						_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

						if ((lMediaMasterType != CARD_TYPE_LOYALTY) && (lMediaMasterType != CARD_TYPE_LOYALTY_AND_CREDIT))//4.0.23.390 116544
						{
							_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_UNKNOWN_ERROR);
							_Module.m_server.m_cFuelLoayaltyPS.HandleAddMemberCardResult(lPumpNumber, pOla, LOYALTY_STATE_UNKNOWN_ERROR);
							// _LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Card type is not loyalty, declined by PumpSrv");
							_LOGMSG.LogMsg("GCI", "SwipeRequest", lPumpNumber, LOG_PUMP, "Card type is not loyalty, declined by PumpSrv");
						}
						else
						{
							_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo, bSkipRepeatOption);
						}
					}
					break;
					default:
					{
						switch (cInfo.CardSaleExtraData3.sParamResult[0])
						{
							case 'Y':
							{
								if (_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsNumericAllowedInSwipeLoyalty())	//4.0.23.410 120603
								{
									//Manual NMP
									_Module.m_server.m_cFlowControlManager.HandleNMPManualLoyalty(lPumpNumber, pOla, &cInfo);
									break;
								}
								else if (!_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsNumericAllowedInSwipeLoyalty())
								{
									_Module.m_server.m_cFlowControlManager.HandleNMPManualLoyalty(lPumpNumber, pOla, &cInfo);  //4.0.23.1380
									break;  //4.0.23.1410
								}
								break;
								//else act like 'N' //4.0.23.410 120603
							}
							case 'N':
							case 'B': //4.0.23.410  120603
							{
								//Continue to next state without loyalty card
								BOOL bStartFueling = FALSE;

								if (_Module.m_server.m_cFuelLoayaltyPS.m_dwLoyaltyEngineType == EPSILON)
								{
									bStartFueling = TRUE;
								}

								str.Format("SwipeRequest() - Skip to start fueling? bStartFueling %d, sTranType = [%c]", 
									bStartFueling,
									cInfo.CardSaleInfo.cardData.sTranType);
								_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

								_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo, TRUE, bStartFueling);
							}
							break;
							case 'A':
							{
								str.Format("SwipeRequest() - Calling HandleNMPAlternateID()", lPumpNumber);
								_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

								_Module.m_server.m_cFlowControlManager.HandleNMPAlternateID(lPumpNumber, pOla, &cInfo);
							}
							break;
							case 'T':	// RFUEL-3705
							case 'J': //4.0.23.481 126185
							{
								char sTmpLanguagePath[MAX_FIELD_VAL] = { 0 };
								_Module.m_server.m_cCommonFunction.LoadDefaultLaguageString(lPumpNumber, sTmpLanguagePath, sizeof(sTmpLanguagePath));
								_Module.m_server.m_cParam.LoadParam((LPCSTR)sTmpLanguagePath, "LoyaltyTransactionCanceled", (char *)cInfo.CardSaleInfo.cardData.sResultMsg, sizeof(cInfo.CardSaleInfo.cardData.sResultMsg), "", FALSE); //4.0.20.10 //4.0.20.260 67761									
								//mmcpy(cInfo.CardSaleInfo.cardData.sResultMsg ,(char *)"Cancel trs was selected by the user" , sizeof("Cancel trs was selected by the user")); 
								_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, &cInfo);
							}
							break;
							default:
							{
								long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, pOla, &cInfo, FALSE, TRUE); //4.0.20.440 68857 //4.0.21.501 //4.0.23.240

																													 //Save loyalty card number (numeric)
								if ((lMediaMasterType != CARD_TYPE_LOYALTY) && (lMediaMasterType != CARD_TYPE_LOYALTY_AND_CREDIT))//4.0.23.390 116544
								{
									_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_UNKNOWN_ERROR);
									_Module.m_server.m_cFuelLoayaltyPS.HandleAddMemberCardResult(lPumpNumber, pOla, LOYALTY_STATE_UNKNOWN_ERROR);
									// _LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Card type is not loyalty, declined by PumpSrv");
									_LOGMSG.LogMsg("GCI", "SwipeRequest", lPumpNumber, LOG_PUMP, "Card type is not loyalty, declined by PumpSrv");
								}
								else
								{
									_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo);
								}
							}
						}
						break;
					}
				}
			}
			break;
			case OLA_LOYALTY_START_NMP_BEFORE_FUELING_AFTER_START_TICKET:	//This state can be only when loyalty declined message was prompt
			case OLA_LOYALTY_PROMPT_BEFORE_FUELING_BEFORE_START_TICKET:
			case OLA_LOYALTY_PROMPT_BEFORE_FUELING_AFTER_START_TICKET:
			case OLA_LOYALTY_PROMPT_AFTER_FUELING_BEFORE_ADD_ITEM:
			{
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData);	//4.0.20.503 66960
				_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo, TRUE);
			}
			break;
			case OLA_LOYALTY_COUPONS_AFTER_FUELING_BEFORE_ADD_ITEM:
			case OLA_LOYALTY_COUPONS_BEFORE_FUELING_AFTER_START_TICKET:
			case OLA_LOYALTY_COUPONS_BEFORE_FUELING_BEFORE_START_TICKET:
			{
				//Allow repeat NMP
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData); //4.0.20.503 66960
				_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo, FALSE);

			}
			break;
			//4.0.20.54 64528
			case OLA_LOYALTY_DEFERRAL_AFTER_FUELING_BEFORE_ADD_ITEM:
			case OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET:
			case OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_BEFORE_START_TICKET:
			{
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData);	//4.0.20.503 66960
				_Module.m_server.m_cFlowControlManager.HandleNMPDeferralDiscount(lPumpNumber, pOla);
			}
			break;
			//Copient 55286 End
			//4.0.22.500 start 70767
			case OLA_LOYALTY_SWIPE_PAYMENT:
				{
					long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData);	//4.0.20.503 66960
					HandleLoyaltyResult(lPumpNumber, pOla, &cInfo, FALSE, FALSE); //4.0.22.504 69344 //4.0.24.60
					_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo, TRUE);
				}
				break;
				
			case OLA_LOYALTY_SWIPE_CARD:
				{
					long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData);	//4.0.20.503 66960
					HandleLoyaltyResult(lPumpNumber, pOla, &cInfo, FALSE, FALSE); //4.0.22.504 69344 //4.0.24.60
					_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo, TRUE);
				}
				break;
			case OLA_LOYALTY_METHOD_OF_PAYMENT: //4.0.23.20 76383 //RFUEL-1237
			{
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);	

				if (_Module.m_server.GetFlowControlType() == DYNAMIC_PAP_FLOW)
					cInfo.CardSaleExtraData4.sLoyaltyEntryMethod = ' ';			

				switch (cInfo.CardSaleExtraData3.sParamResult[0])
				{
					case 'Y':
					case 'I':	//4.0.23.860 212541
					{
						if (_Module.m_server.GetFlowControlType() == DYNAMIC_PAP_FLOW)
							_Module.m_server.m_cFlowControlManager.HandleNMPMethodOfPayment(lPumpNumber, pOla, &cInfo, FALSE, TRUE);
						else
							_Module.m_server.m_cFlowControlManager.HandleNMPMethodOfPayment(lPumpNumber, pOla, &cInfo, TRUE, FALSE);
					}
					break;
					case 'N':
					case 'T':	// RFUEL-281
					{
						if (_Module.m_server.GetFlowControlType() == DYNAMIC_PAP_FLOW)
							_Module.m_server.m_cFlowControlManager.HandleNMPMethodOfPayment(lPumpNumber, pOla, &cInfo, TRUE, FALSE);
						else
							_Module.m_server.m_cFlowControlManager.HandleNMPMethodOfPayment(lPumpNumber, pOla, &cInfo, FALSE, FALSE);
					}
					break;
					default:
					{
						//Pap to Pak requested	
						_Module.m_server.m_cFlowControlManager.HandleNMPMethodOfPayment(lPumpNumber, pOla, &cInfo, FALSE, TRUE);
						HandleLoyaltyResult(lPumpNumber, pOla, &cInfo, FALSE, FALSE); //4.0.22.504 69344 //4.0.23.240
					}
				}
				_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo, TRUE);
			}
			break;
// 4.0.23.1220 TD 155754 start
			case OLA_LOYALTY_PHONE_LOOKUP:
			{
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);

				HandleLoyaltyPhoneLookUp(lPumpNumber,lSeqNumber,pOla , cInfo , pCardData);
				
				break;
			}
			
			case OLA_LOYALTY_PHONE_LOOKUP_ENTER_PHONE_NUMBER:
				{
					long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);	
					
					_Module.m_server.m_cFlowControlManager.HandleNMPGetPhoneNumber(lPumpNumber, pOla, &cInfo);
					
					break;
				}
			case OLA_LOYALTY_PHONE_LOOKUP_ENTER_REWARD_CARD:
				{
					long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);	
					
					_Module.m_server.m_cFlowControlManager.HandleNMPGetRewardCard(lPumpNumber, pOla, &cInfo);
					
					break;
				}
				//4.0.23.???? TD 295237
			case OLA_LOYALTY_WAIT_FOR_DEFERAL_ANSWER:
				{
					long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);
					
					_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cInfo);
					
					_Module.m_server.m_cFlowControlManager.HandleManualDeferal(lPumpNumber, pOla, &cInfo);
					
					break;
					
				}	
			case OLA_LOYALTY_DISPLAY_PRE_AUTHOIRZE_SCREEN:
				{
					HandlePreAuthorizeScreen(lPumpNumber, lSeqNumber, pCardData, cInfo, pOla);
					break;
				}
			case OLA_LOYALTY_DISPLAY_PARITAL_APPROVAL_SCREEN:
				{
					HandleParitalApprovalScreen(lPumpNumber, lSeqNumber, pCardData, cInfo, pOla);
					break;
				}
			case (SESSION_AUTHORIZE | OLA_REQUEST_TO_SEND):
			case (OLA_LOYALTY_DECLINE):
			//case (OLA_LOYALTY_WAIT_FUELING):
			{
					break;
			}
			case OLA_LOYALTY_ALTERNATE_ID:
			{
				_Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO, lPumpNumber, lSeqNumber, pCardData);	//4.0.20.503 66960

				_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cInfo);

				if(cInfo.CardSaleExtraData3.sParamResult[0] == 'J')
				{
					_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, &cInfo);
				}
				else
				{
					 HandleLoyaltyResult(lPumpNumber, pOla, &cInfo, FALSE, TRUE);
					_Module.m_server.m_cFlowControlManager.HandleNMPAlternateID(lPumpNumber, pOla, &cInfo);
				}
			}
			break;
			case OLA_LOYALTY_ALTERNATE_ID_MANUAL_ENTER:
			{
				_Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData);

				_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cInfo);

				CString str;
				str.Format("SwipeRequest(OLA_LOYALTY_ALTERNATE_ID_MANUAL_ENTER) - sEntryMethod %c, sParamResult %c", 
					cInfo.CardSaleInfo.cardData.sEntryMethod,
					cInfo.CardSaleExtraData3.sParamResult[0]);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

				cInfo.CardSaleExtraData4.sLoyaltyEntryMethod = 'A';

				// CR 501164
				if (cInfo.CardSaleExtraData3.sParamResult[0] == 'J' || cInfo.CardSaleExtraData3.sParamResult[0] == 'N')
				{
					_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, &cInfo);
				}
				else			
				{	
					HandleLoyaltyResult(lPumpNumber, pOla, &cInfo, TRUE, FALSE);
					_Module.m_server.m_cFlowControlManager.HandleNMPAlternateIDManualEntry(lPumpNumber, pOla, &cInfo);
		
				}
			}
			break;
			case OLA_LOYALTY_ALTERNATE_ID_SWIPE_LOYALTY_CARD:
			{
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO, lPumpNumber, lSeqNumber, pCardData);

				_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cInfo);

				HandleLoyaltyResult(lPumpNumber, pOla, &cInfo, FALSE, TRUE);

				_Module.m_server.m_cFlowControlManager.HandleNMPAlternateIdSwipeLoyaltyCard(lPumpNumber, pOla, &cInfo);
			}
			break;
			case OLA_LOYALTY_WAIT_FOR_ALTERNATE_ID_RETRY_ANSWER:
			{
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData);

				_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cInfo);

				switch (cInfo.CardSaleExtraData3.sParamResult[0])
				{
					case 'A':			
						cInfo.CardSaleExtraData6.sIdRetryCount = (cInfo.CardSaleExtraData6.sIdRetryCount <= '0') ? '1' : (cInfo.CardSaleExtraData6.sIdRetryCount + 1);
						_Module.m_server.m_cFlowControlManager.HandleNMPAlternateID(lPumpNumber, pOla, &cInfo);
						break;
					case 'P':
						cInfo.CardSaleExtraData6.sIdRetryCount = '0';
						_Module.m_server.m_cFlowControlManager.HandleNMPAlternateID(lPumpNumber, pOla, &cInfo);
						break;
					case 'J': // RFUEL-137
					case 'N': // RFUEL-137
					{
						_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, &cInfo);
					}
					break;

					default:
					{
						_Module.m_server.m_cFlowControlManager.HandleNMPAlternateID(lPumpNumber, pOla, &cInfo);
						break;
					}
				}
				_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, 0);


				//				_Module.m_server.m_cFlowControlManager.HandleNMPAlternateIDRetry(lPumpNumber, pOla, &cInfo);

				break;
			}
			case OLA_LOYALTY_MEMBER_NOT_ACTIVE:
			{
				long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO, lPumpNumber, lSeqNumber, pCardData);

				_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cInfo);

				_Module.m_server.m_cFlowControlManager.HandleNeedMoreParamInactiveMemberResult(lPumpNumber, pOla, &cInfo);
			}
			break;
			default:
			{
					char strMsg[MAX_FIELD_VAL];
					sprintf_s(strMsg, MAX_FIELD_VAL, "Illegal OLA state 0x%x was received", pOla->m_byState);
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
					_Module.m_server.SendSwipeResult(SESSION_AUTHORIZE, lPumpNumber, CD_NOT_OK, CD_DECLINE_MODE, pCardData);	//4.0.20.503	66960
			}
		}
	}

	if (bClockSession) //4.0.6.505
		*pbAskUserToTryAgain =  FALSE;


//	_LOGMSG.LogLocationOut("GCI::SwipeRequest()",1);
	return retCode;

}


//4.0.20.503 66960
GCIRetCode CGCI::SwipeCancel(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CARD_SALE_ALL3 *pCardData, BOOL *pbAskUserToTryAgain)
{

//	_LOGMSG.LogLocationIn("GCI::SwipeCancel()",1);

	GCIRetCode retCode = GCI_OK;

	switch (OLA_STATE(pOla->m_byState))
	{
		case OLA_NONE:										
		case OLA_SEND_END:		 								
		break;

		case OLA_REQUEST_TO_SEND:
		case OLA_WAIT_RESPONSE:
		case OLA_REQUEST_TO_SEND_MORE:
		case OLA_WAIT_FOR_MORE_INFO:

			pOla->m_byState = OLA_NEW_STATE(pOla->m_byState,OLA_SEND_END);
			if(LOG_BASIC_FLOW_CONTROL)
				_LOGMSG.LogOLAMsg(lPumpNumber,pOla->m_byState,pOla->m_lSessionNumber);
		break;
	}				

//	_LOGMSG.LogLocationOut("GCI::SwipeCancel()",1);

	return retCode;
}

STDMETHODIMP CGCI::SetParam(BSTR sTableName, long lKey1, long lKey2, BSTR sFiledName, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal)
{
	// Control client not allowed to create parameters - 
	// bCreate force to FALSE internaly 

//	_LOGMSG.LogLocationIn("GCI::SetParam()",0);
	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif
		SetParam__(sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::SetParam")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetParam()", LOG_LEVEL_0);

		exit(1);
	}
#endif

	
//	_LOGMSG.LogLocationOut("GCI::SetParam()",0);
	return S_OK;
}

void CGCI::SetParam__( BSTR sTableName,  long lKey1,  long lKey2,  BSTR sFiledName,  short nFiledType,  BSTR sVal,  long lVal,  BOOL bCreate, long * pVal)
{
	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;
	
	char szTableName[MAX_TABLE_NAME]; 
	char szFieldName[MAX_FIELD_NAME]; 
	char szFieldVal[MAX_FIELD_VAL]; 

	memset(szTableName,0,sizeof(szTableName));
	int iLen = WideCharToMultiByte(CP_ACP,0,sTableName,-1,szTableName,sizeof(szTableName),NULL,NULL);
	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GCI_TABLE_NAME_TOO_LONG;
	}

	memset(szFieldName,0,sizeof(szFieldName));
	iLen = WideCharToMultiByte(CP_ACP,0,sFiledName,-1,szFieldName,sizeof(szFieldName),NULL,NULL);
	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GCI_FIELD_NAME_TOO_LONG;
	}

	memset(szFieldVal,0,sizeof(szFieldVal));
	if(nFiledType == FIELD_SZ)
	{
		iLen = WideCharToMultiByte(CP_ACP,0,sVal,-1,szFieldVal,sizeof(szFieldVal),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_DEFAULT_FIELD_VAL_TOO_LONG;
		}
	}
	
	if(retCode == GCI_OK)
	{  
		// bCreate forse to FASLE - 
		// creating parameters via GCI is not allowed 
		long lRtc = _Module.m_server.m_cParam.SetParam( szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,(DWORD)lVal,FALSE); 			
	 	if(lRtc)
		{
			switch(lRtc)
			{
				case 1: retCode = GCI_FAILED_TO_OPEN_TABLE;break;		 
				case 2: retCode = GCI_PARAM_NOT_FOUND;break;
				//4.0.3.21
				default:
				retCode  = ConverHydServiceCode2GciCode((CFGDBRetCode)lRtc);
				//4.0.3.21					
			}
			
		}
		else
			_Module.m_server.UpdateParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,lVal);  //4.0.7.507


	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"SetParam",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);

	
	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::AVIStart(long lPumpNumber, long lNozzleNumber, BSTR sData, long *pVal)
{
	GCIRetCode retCode = GCI_OK;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if(NOZZLE_NOT_VALID((short)lNozzleNumber))
		retCode = GCI_INVALID_NOZZLE_NUMBER;

	if(retCode == GCI_OK)
	{
		///////////////////////////
		retCode = GCI_NOT_SUPPORTED;
		///////////////////////////
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"AVIStart",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;


	return S_OK;
}

STDMETHODIMP CGCI::AVIStop(long lPumpNumber, long lNozzleNumber, long *pVal)
{
	
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if(NOZZLE_NOT_VALID((short)lNozzleNumber))
		retCode = GCI_INVALID_NOZZLE_NUMBER;

	if(retCode == GCI_OK)
	{
		///////////////////////////
		retCode = GCI_NOT_SUPPORTED;
		///////////////////////////
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"AVIStop",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;

	return S_OK;
}

STDMETHODIMP CGCI::GetPAPSwipeData(long lPumpNumber, long lFlags, BSTR *pData, long *pVal)
{
//	_LOGMSG.LogLocationIn("GetPAPSwipeData()");	
	USES_CONVERSION;

// M.L 2.4.4.1 Start
	if(pData == NULL)
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"IGCI::GetPAPSwipeData   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	
		return E_POINTER;
	}
// M.L 2.4.4.1 End

#ifdef CATCH_GCI
	__try
	{
#endif

		GetPAPSwipeData__(lPumpNumber,lFlags,pData,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetPAPSwipeData")) //4.0.15.450catch(...)
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetPAPSwipeData()", LOG_LEVEL_0);	

		exit(1);
	}
#endif


	return S_OK;
}

/******************************************************************************
 Description:	Called by convert layer, pumpsrv will return extra 
				data according to lFlags and pump number
				
 Returns:      	void  	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			26/5/2008   10:41		Old method with addtions
******************************************************************************/
void CGCI::GetPAPSwipeData__( long lPumpNumber,  long lFlags,  BSTR *pData, long *pVal)
{
	GCIRetCode retCode = GCI_OK;

	if(LOG_BASIC_FLOW_CONTROL) //4.0.20.40
	{
		CString str = "";
		str.Format("CGCI::GetPAPSwipeData, lFlags=%ld", lFlags);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
	}
	
	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if((_Module.m_server.IsServerInRecoveryProcess() || (!_Module.m_server.m_cOLASrv->Valid())) 
			&& _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GCI_FAILED_IO;

	if(retCode == GCI_OK) 
	{
		//**************************************
		//We got request for new Chip extra info
		//**************************************
		if(lFlags & GPSD_GET_EXTRA_CHIP_INFO) //4.0.18.300 
		{
			if(!PUMP_NOT_VALID(lPumpNumber))
			{
				long lRecordSize = sizeof(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1]);
				BYTE *sTmpAllocationBuffer = new BYTE[lRecordSize+2];
				Str2StrNull((BYTE *)&(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1]) ,lRecordSize , sTmpAllocationBuffer);
				CComBSTR strResult(lRecordSize + 2,(LPCSTR)sTmpAllocationBuffer);  //4.0.5.0

				delete []sTmpAllocationBuffer;
				*pData = strResult.Copy();
			}			
		}
		else
		{
			OLA_STAT	Ola;
			_Module.m_server.m_cPumpArray[lPumpNumber-1].GetOlaStat(&Ola);
			long lOLaRecordSize = sizeof(CARD_SALE_ALL2); 

			if ( ((_Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetRes() == PAY_AT_PUMP) || ((_Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetRes() == PAY_AT_KIOSK) && (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()))) ||	//4.0.22.504 69344
			 !  (lFlags & GPSD_FOR_CURRENT_PAP) || 
			 (lFlags & GPSD_FOR_CURRENT_DEBIT_PAP ) ||  //4.0.5.23
			 (lFlags & GPSD_FOR_STORAGE_PAP_DETAILS ))  //4.0.2.21
			
			{
				PAY_AT_PUMP_INFO savedInfo; //4.0.5.0
				memset(&savedInfo,' ',sizeof(savedInfo));


				if (GPSD_FOR_THREE_STRUCT & lFlags)
					lOLaRecordSize = sizeof(CARD_SALE_ALL3); 
					

				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------
				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);
					
				_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&savedInfo);						

				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------
				if (slPumpArray.IsLocked())
					slPumpArray.Unlock();


				// in case ola by controler there was no DPT-R therefor no Saved data.
				if (OLA_BY_CONTROLLER   && (_Module.m_server.OLAByController() != 2))	//4.0.19.509
				{
					//4.0.5.0				
					memset((BYTE *)&savedInfo.CardSaleInfo.cardData , ' ', sizeof(CARD_SALE_ALL3));
				}

				//4.0.2.13
				
				//4.0.5.0
				if (lFlags & GPSD_FOR_CURRENT_DEBIT_PAP )
				{				
					
					BYTE *sTmpAllocationBuffer = new BYTE[lOLaRecordSize+sizeof(OLA_EXTRA_DATA)+2];
					memset(sTmpAllocationBuffer, 0, lOLaRecordSize + sizeof(OLA_EXTRA_DATA) + 2); //coverity
					memcpy(sTmpAllocationBuffer, &savedInfo.CardSaleInfo.cardData, lOLaRecordSize);

					if( savedInfo.CardSaleExtraData2.sPinPadAction  != ' ' )
					{				
						memcpy(sTmpAllocationBuffer + lOLaRecordSize,     _Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byLen, 4);
						memcpy(sTmpAllocationBuffer + lOLaRecordSize + 4, _Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byData,a2l(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byLen, 4));
					}				
					
					CComBSTR strResult(lOLaRecordSize + 2,(LPCSTR)sTmpAllocationBuffer); 
					delete[] sTmpAllocationBuffer; //4.0.5.0  
					*pData = strResult.Copy();
				}
				else
				{
					// RFUEL-2817, abstracted the data conversion process
					BYTE* pCard = reinterpret_cast<BYTE *>(&savedInfo.CardSaleInfo.cardData);
					ChangeNullChar2Char((char*)pCard, lOLaRecordSize, SPACE_CHAR);
					*pData = Str2BSTR(pCard, lOLaRecordSize);

					/*  Commented out to preserve the original logic
					BYTE *sTmpAllocationBuffer = new BYTE[lOLaRecordSize + 2];
					BYTE * pCard = (BYTE *)&savedInfo.CardSaleInfo.cardData;
					for (int i = 0; i < lOLaRecordSize; i++)
					{
						if (pCard[i] == 0)
							pCard[i] = ' ';
					}
					Str2StrNull((BYTE *)&savedInfo.CardSaleInfo.cardData, lOLaRecordSize, sTmpAllocationBuffer);
					CComBSTR strResult(lOLaRecordSize + 2, (LPCSTR)sTmpAllocationBuffer);  //4.0.5.0

					delete[]sTmpAllocationBuffer;
					*pData = strResult.Copy();
					*/
				}
				//4.0.2.13  //4.0.5.0
			}
			else
				retCode = GCI_NO_WAITING_TRS;
		}		
		//4.0.23.504 87285
		if ((_Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetRes() == PAY_AT_KIOSK) && (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag()))	//4.0.22.504 69344
		{//4.0.22.504 69344
			retCode = GCI_PAK_WITH_LOYALTY;

		}
	}

	//4.0.19.504 - 47559
	if (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse())  // CR 444830
	{
		retCode = GCI_TERMINAL_SRV_IN_USE;
	}

	
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"GetPAPSwipeData",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;
}

AVINextState CGCI::CheckAviConditions(CPumpStatus *pStat, PumpReservation lNewRes, PumpResExt lNewResExt)
{
	PumpStatus lStatus = pStat->GetStatus();
	PumpReservation lRes = pStat->GetRes();
	PumpResExt lResExt = pStat->GetResExt();
	AVINextState lNextState;


	if( (lNewRes == PAY_AT_PUMP) && _Module.m_server.IsResForAVI(lNewResExt) )
	{
		if(_Module.m_server.m_cModeArray[pStat->GetMode()-1].ResNotValid(lNewRes,lNewResExt))
		{
			lNextState = AVI_DO_NOT_UPDATE_STATUS;
		}
		else
		{
			//CheckCondition...
			//=====================
			switch(lRes) // current reservation 
			{
				case NULL_RES:
				{
					if((lStatus == READY)||(lStatus == NOZZLE_LEFT_OUT) || 
					   ((lStatus == REQUESTING) && (lResExt == NULL_RES_EXT))) //4.0.15.11 - Open the option for a requesting pump.
						lNextState = AVI_UPDATE_STATUS;
					else
						lNextState = AVI_DO_NOT_UPDATE_STATUS;

					if ((lStatus == REQUESTING) && (lResExt == NULL_RES_EXT))
						_LOGMSG.LogMsg("CheckAviConditions(Status = REQUESTING , ResExt = NULL_RES_EXT");	
				
				}
				break;

				case PAY_AT_KIOSK:
				{
					if((lStatus == READY)||(lStatus == REQUESTING)||(lStatus == NOZZLE_LEFT_OUT))
						lNextState = AVI_UPDATE_STATUS;
					else if((lStatus == AUTHORIZED)||(lStatus == DISPENSING))
						lNextState = AVI_STOP_PUMP;
					else
						lNextState = AVI_DO_NOT_UPDATE_STATUS;					
				}	
				break;

				case PRE_PAY_AT_POS:
				{
					if((lStatus == READY)||(lStatus == NOZZLE_LEFT_OUT))
						lNextState = AVI_UPDATE_STATUS;
					else if((lStatus == AUTHORIZED)||(lStatus == REQUESTING)||(lStatus == DISPENSING))
						lNextState = AVI_STOP_PUMP;
					else
						lNextState = AVI_DO_NOT_UPDATE_STATUS;					
				}
				break;

				case PAY_AT_PUMP:
				{
					if(lResExt == NULL_RES_EXT)
					{
						if(lStatus == READY)
						{   // this is 90% conflict , since PAP is in use,
							// the next step will PAPSwipe, and in this function PumpSrv check 
							// validity of sequence and Card Data.
							lNextState = AVI_UPDATE_STATUS;							
						}
						else if((lStatus == AUTHORIZED)||(lStatus == REQUESTING)||(lStatus == DISPENSING))
							lNextState = AVI_STOP_PUMP;
						else
							lNextState = AVI_UPDATE_STATUS;
					}
					else if(_Module.m_server.IsResForAVI(lResExt))
					{
						lNextState = AVI_UPDATE_STATUS;
					}
					else 
					{
						lNextState = AVI_DO_NOT_UPDATE_STATUS;					
					}
				}	 
				break;

				case PUMPSRV:
				{
					lNextState = AVI_DO_NOT_UPDATE_STATUS;
				}
				break;

				default:
					lNextState = AVI_DO_NOT_UPDATE_STATUS;
					break;
			}
		}
	}
	else
	{
		lNextState = AVI_UPDATE_STATUS;
	}
	return lNextState;
}


void CGCI::AVIStop__( long lPumpNumber,  long lNozzleNumber, long *pVal)
{
}
void CGCI::AVIStart__( long lPumpNumber,  long lNozzleNumber, BSTR sData, long *pVal)
{
}

STDMETHODIMP CGCI::TankReading2(long lTankNumber, long lProduct, short nFlags, long lStatus1, long lFuelLevel, long lWaterLevel, long lTemperature, long lVolume, long lWaterVolume, long lStatus2, long lFuelValue, long lPricePerLitre, long *pVal)
{
//	_LOGMSG.LogLocationIn("GCI::TankReading2()",0);

#ifdef CATCH_GCI
	__try
	{
#endif
		TankReading2__(lTankNumber,lProduct,nFlags,lStatus1,lFuelLevel,lWaterLevel,lTemperature,lVolume,lWaterVolume,lStatus2,lFuelValue,lPricePerLitre,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::TankReading2")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::TankReading2()", LOG_LEVEL_0);

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::TankReading2()",0);
	return S_OK;
}

//4.0.0.78
void CGCI::TankReading2__( long lTankNumber, long lProduct,  short nFlags,  long lStatus1,  long lFuelLevel,  long lWaterLevel,  long lTemperature,  long lVolume,  long lWaterVolume, long lStatus2, long lFuelValue, long lPricePerLitre, long *pVal , long lTcVolume , long lUllage, BSTR bstrXml)
{

	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();

	GCIRetCode retCode = GCI_OK;
	BOOL  bDeliveryFailed = FALSE;	
	
	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
		
	else if(!(nFlags & TANK_READ_NOT_AVAILABLE))
	{
		if (TANK_NOT_VALID(lTankNumber))
			retCode = GCI_INVALID_TANK_NUMBER;
	}


	if(retCode == GCI_OK)
	{
		if(nFlags & TANK_READ_NOT_AVAILABLE) 
		{
			CString str;
			// not available...
			if(lTankNumber == 0)//4.0.2.15 All tanks
			{				
				str.Format("Tank Reading not available for All Tanks, Flags=%ld, memo=%d, command flags=0x%08x!", _Module.m_server.m_cServerInfo.GetWetStockFlags() ,_Module.m_server.m_cServerInfo.GetMemoProcessFlags()  , nFlags); //4.0.7.503 //4.0.8.500
				//If we're in the middle of a delivery process we notify the State Machine
				//of a failure, and the bDeliveryFailed variable was declared for that purpose
				bDeliveryFailed = TRUE;

				if (!(nFlags & TANK_READ_END_OF_LIST))  //4.0.8.500
					nFlags +=TANK_READ_END_OF_LIST;

				if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite())
				{
					if(!_Module.m_server.m_bTankReadingRcved)
						_Module.m_server.m_bTankReadingRcved = TRUE;
				}

				if (_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS )
				{
					if (_Module.m_server.m_cServerInfo.GetMemoProcessFlags())
					{
						_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_MEMO_TANK_READING,0);
						_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,
										0,
										GCI_EVENT_MEMO_TANK_READING );		
					}
					
					_Module.m_server.m_cServerInfo.ClearTankReadingFlags();

				}				
			}
			else
			{
				str.Format("Tank Reading not avaliable for Tank %d, Flags=%ld,memo=%d, command flags=0x%08x !", lTankNumber, _Module.m_server.m_cServerInfo.GetWetStockFlags(), _Module.m_server.m_cServerInfo.GetMemoProcessFlags(), nFlags);

				_Module.m_server.m_cTankArray[lTankNumber].SetTankStatus(TANK_READ_NOT_AVAILABLE); //CR 443524
			}


			if(LOG_BASIC_FLOW_CONTROL)  //4.0.8.500
			{							
				_LOGMSG.LogMsg(str);
			}	
		}
		else
		{		
			BOOL bSaveRcvedInfo = FALSE;

			if(LOG_TRS_FILE)
				_Module.m_server.m_cTrsLog.LogTankTotals(lTankNumber,lProduct,lVolume);

			if(CONTROLLER_IS_PUMP_LOCK_MASTER  || _Module.m_server.IsMeggitSite())
			{
				bSaveRcvedInfo = TRUE;
			}
			else if (!_Module.m_server.m_bTankReadingRcved) 
			{
				bSaveRcvedInfo = TRUE;
			}
			else if(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS )
			{
				bSaveRcvedInfo = TRUE;
			}

			if(bSaveRcvedInfo)
			{
				TANK_READ_INFO		info;
				FillTankReadingInfo(info, lTankNumber, lProduct, lStatus1, nFlags, lFuelLevel, lWaterLevel, lTemperature, lVolume, lWaterVolume, lStatus2, lFuelValue, lPricePerLitre, lTcVolume, lUllage);

				//4.0.0.82 - not saving into the qdex if from memo interval process.
				long lRtc = 0;

				if(_Module.m_server.m_cServerInfo.GetMemoProcessFlags() == FALSE)
					lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_INSERT,&info);

				if(lRtc)
					retCode = GCI_FAILED_IO;

				//Write to shift Q-Dex only if in middle of shift
				if(_Module.m_server.IsShiftChangeInProgress() && _Module.m_server.GetWetstockMaintFlag())
				{
					CShiftTanks cShiftTanks;

					lRtc = cShiftTanks.Write(info);

					if(lRtc)
						_LOGMSG.LogMsg("Failed to write to EOD qdx",LOG_LEVEL_1);
				}

				SYSTEMTIME cTime;  //443035 since cTime was added to SetTankInfo
				memset(&cTime, 0, sizeof(cTime));
				
				//add by Yaron
				//checking if having memo process in system
				if ( _Module.m_server.GetMemoInterval() > 0 )
				{
					_Module.m_server.m_cTankArray[lTankNumber-1].SetTankInfo(
					lProduct,lStatus1,nFlags, lFuelLevel,lWaterLevel,lTemperature,
					lVolume, lWaterVolume,lUllage,lTcVolume,lStatus2, cTime);
					_Module.m_server.m_cTankArray[lTankNumber-1].SetTankValidity(true);
					
					//4.0.5.22
				}
				//end add by Yaron

				//**************************************************************************
				//Handle the PumpSrv delivery emulation process (does nothing if not active)
				//**************************************************************************
				if (_Module.m_server.GetSupportDeliveryEmulation() == TRUE)
				{ 
					long lDensity = _Module.m_server.m_cTankXML.GetDensityFromXML(bstrXml); // CR 409564
					HandleDeliveryEmulationSingleTankProcess(lTankNumber, lVolume, lTemperature, lWaterVolume,nFlags,lDensity,info.lFuelWeight,lFuelLevel,lWaterLevel); //4.0.19.410 TD 66935
					HandleDeliveryEmulationProcess(lTankNumber, lVolume, lTemperature, lWaterVolume, nFlags);				//4.0.19.150 (TD 62818) 4.0.19.151 (TD 63506)
				}
				
			}
			else
			{
				// M.L 16.7.98
				if(LOG_LOGIC_ERROR)
					_LOGMSG.LogMsg("Unexpected tank reading , record ignored  !!! ", LOG_LEVEL_1);

			}
		

		}

		if (nFlags & TANK_READ_END_OF_LIST) //4.0.8.500
		{	
			if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_POS )
			{
				if (!(_Module.m_server.dwPosRequestReport & REPORT_TYPE_ULLAGE))
					_Module.m_server.dwPosRequestReport  += REPORT_TYPE_ULLAGE; 

			}
			//4.0.1.38 - sending the event only if writing into the qdex.
			else if(_Module.m_server.m_cServerInfo.GetMemoProcessFlags() == FALSE)
				_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_TANK_READING,0);//Add by Yaron for Ctrl Events
			else if (!(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_POS_PUMP)) //4.0.16.500
			{
				_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_MEMO_TANK_READING,0);
				_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,
								0,
								GCI_EVENT_MEMO_TANK_READING );		
			}

			if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS )
			{
				_Module.m_server.m_cServerInfo.ClearTankReadingFlags();

				SQLPumpSrvFile.SaveServerInfo();

			}


			//4.0.5.24 Added by Udi for Delivery
			//In case the tanks were read for the purpose of delivery, we
			//instantiate the delivery flags with the appropriate values, and
			//notify the State Machine the process has succeeded 
			
			int nErrorRetrieved = 0;
			_Module.m_server.SetDeliveryFlags(DES_UNLOCK, PD_NONE, nErrorRetrieved, TRUE);
		
			if (lStatus1 == TANK_STATUS_IDLE)
			{
				// update flag if needed //3.2.0.34
				if(PUMPSRV_IS_PUMP_LOCK_MASTER  || _Module.m_server.IsMeggitSite())
				{
					if(!_Module.m_server.m_bTankReadingRcved)
						_Module.m_server.m_bTankReadingRcved = TRUE;
				}
			}
			
		} // end of list

	}


	if(retCode && LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(0, "IGCI","TankReading2",TRUE,retCode,FALSE,0);

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;

		str.Format("Tank reading2(lTankNumber = %ld, lProduct = %ld, nFlags = %d, lStatus1 = %ld, lStatus2 = %ld, lFuelLevel = %ld, lWaterLevel = %ld, lTemperature = %ld, lVolume = %ld, lWaterVolume = %ld)",
			lTankNumber,lProduct,nFlags,lStatus1,lStatus2,lFuelLevel,lWaterLevel,lTemperature,lVolume,lWaterVolume);

		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}

	int nErrorRetrieved = 0;

	if(bDeliveryFailed)
	{
		_Module.m_server.SetDeliveryFlags(DES_UNLOCK, PD_NONE, nErrorRetrieved, FALSE);
	}
	
	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGCI::PumpTotals2(long lPumpNumber, short nNozzle, short nGrade, short nFlags, long lVolumeH, long lVolumeL, long lValueAH, long lValueAL, long lValueBH, long lValueBL,long lPumpStatus,long lNozzleStatus, long *pVal)
{
	//_LOGMSG.LogLocationIn("GCI::PumpTotals2()",0);

	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif
		PumpTotals2__(lPumpNumber,nNozzle,nGrade,nFlags,lVolumeH,lVolumeL,lValueAH,lValueAL,lValueBH,lValueBL,lPumpStatus,lNozzleStatus,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::PumpTotals2")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","PumpTotals2()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}	
#endif

	//_LOGMSG.LogLocationOut("GCI::PumpTotals2()",0);
	return S_OK;
}

void CGCI::PumpTotals2__( long lPumpNumber,  short nNozzle,  short nGrade,  short nFlags, long lVolumeH,  long lVolumeL,  long lValueAH,  long lValueAL,  long lValueBH,  long lValueBL, long lPumpStatus, long lNozzleStatus, long *pVal)
{
	short nFinalGrade = nGrade;
	long lNumberOfPumps = 0; //4.0.17.501 merge from 4.0.15.171

	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("Rcved Pump Totals, Grade %d", nGrade);
		_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,str,nFlags); 				
	}

	if(retCode == GCI_OK)
	{

		//Take the exact number of configured pumps at the site:
		long lLastPump = _Module.m_server.GetLastValidPump();
		for(int i = 0; i < lLastPump;i++)
		{
			CString str , sTable("Pumps\\Pump");
			char sNumber[8];
			sprintf_s(sNumber,3,"%02d",i+1);
			str = sTable + sNumber;
			DWORD dwPumpValid = 0;
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwPumpValid,(DWORD)1,FALSE);
			if (dwPumpValid)
				lNumberOfPumps++;
		}
		// CR_475695_Start
		CString str;				
		str.Format("PumpTotals2__  nFlags:0X%08x",nFlags);
		_Module.m_server.m_cLogger.LogMsg(str);
		SetPumpReadingRequestType(&nFlags);
		// CR_475695_Ed
		_Module.m_server.m_cTotalPumpInfo.SetPumpStatus(lPumpNumber, lPumpStatus); //4.0.15.120 - CR 44000

		if (nFlags & TOTALS_NOT_AVAILABLE)
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString strLog;
				strLog.Format("totals not available PumpNumber %ld, Grade %ld", lPumpNumber, nGrade);
				_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,strLog,nFlags);
			}

			ReleasePumpTotalPump(lPumpNumber); 
			//3.2.0.34
			if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite()) 
			{
				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------
				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

				if((_Module.m_server.m_cPumpArray[lPumpNumber-1].GetLockStatus() == WAIT_RESPONSE)||
				   (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetLockStatus() == LOCKED) )
				{
					long lMisc = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetMisc();
					lMisc &= MISC_NOT_WAIT_LOCK_TIMEOUT;
					lMisc &= MISC_NOT_WAIT_RESPONSE_TIMEOUT;

					_Module.m_server.m_cPumpArray[lPumpNumber-1].SetLockStatus(RESPONSE_RECEIVED,lMisc);
					_Module.m_server.SavePumpInfo(lPumpNumber);
				}

				// M.L 8/5/98 Lock PumpArray in large scope 
				//-----------------------------------------
				if (slPumpArray.IsLocked())
					slPumpArray.Unlock();
			}
		}
		else
		{
			TOTAL_INFO		info;
			memset(&info,0,sizeof(TOTAL_INFO));
			BOOL bWriteReadInfo = FALSE;

			//RFUEL-1265
			CString sVolume;
			sVolume.Format("%015ld%05ld", lVolumeH, lVolumeL);
			memcpy(info.sVolume, sVolume.GetBuffer(0), min(sizeof info.sVolume, sVolume.GetLength()));
			CString sValueA;
			sValueA.Format("%015ld%05ld", lValueAH, lValueAL);
			memcpy(info.sValueA, sValueA.GetBuffer(0), min(sizeof info.sValueA, sVolume.GetLength()));
			CString sValueB;
			sValueB.Format("%015ld%05ld", lValueBH, lValueBL);
			memcpy(info.sValueB, sValueB.GetBuffer(0), min(sizeof info.sValueB, sValueB.GetLength()));

			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				//str.Format("Pump (%d): Total Volume %5d%05d, Total Value (A) %05d%05d, Total Value (B) %05d%05d", 
				str.Format("Total Volume %.20s, Total Value (A) %.20s, Total Value (B) %.20s", 
					info.sVolume,
					info.sValueA,
					info.sValueB);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 				
			}
			
			//4.0.12.506
			_Module.m_server.m_cTotalPumpInfo.SetTotalsWitnNewData(lPumpNumber,nGrade,nNozzle,info.sVolume,
				info.sValueA,info.sValueB);

			//Write the volume (9 digits) of pump per position to the INI file" //TD 44000
			/*************************/			
			char  szFieldVal[9];			
			char szTableName[MAX_TABLE_NAME]; 
			char szSubTableName[MAX_TABLE_NAME];
			
			memset(szFieldVal,0,sizeof(szFieldVal));
			memset(szSubTableName,0,sizeof(szSubTableName));
			memset(szTableName,0,sizeof(szTableName));
			
			sprintf_s(szTableName,MAX_TABLE_NAME,"Pump%02ld",lPumpNumber);
			sprintf_s(szSubTableName,MAX_TABLE_NAME,"Position%ld",nNozzle);
			
			memcpy(&szFieldVal,info.sVolume+11,sizeof(szFieldVal)); //  Save Only 9 digits to Ini file (20 - 9 = 11)
			DWORD dwVAl = a2l((unsigned char *)szFieldVal,sizeof(szFieldVal));
			
			_Module.m_server.m_cParam.SetParam3Level("Pumps", szTableName,szSubTableName,0,0,"CumulateVolume",FIELD_DWORD,NULL,dwVAl,FALSE ,TRUE); 
			/*************************/

			//4.0.19.502 Liat 47235
			//Save Current Sales of a position
			dwVAl = 0;
			char sCurrentTotalsVolume[2 * STR_LONG_VAL10] = {0};	//Max of 19 digits for int64 parsing
			memcpy(sCurrentTotalsVolume, info.sVolume + 1, 2 * STR_LONG_VAL10 - 1);	//4.0.19.502
			_Module.m_server.m_cParam.SetParam3Level("Pumps", szTableName,szSubTableName,0,0,"CurrentTotals",FIELD_SZ,sCurrentTotalsVolume,dwVAl ,FALSE ,TRUE); 

			char sCurrentTotalsAmount[2 * STR_LONG_VAL10] = {0};	//Max of 19 digits for int64 parsing
			memcpy(sCurrentTotalsAmount, info.sValueA  + 1, 2 * STR_LONG_VAL10 - 1);	//4.0.31.??? TD 405462 Cash Amount
			_Module.m_server.m_cParam.SetParam3Level("Pumps", szTableName,szSubTableName,0,0,"CurrentAmountTotals",FIELD_SZ,sCurrentTotalsAmount,dwVAl ,FALSE ,TRUE); 


			//4.0.3.39
			if(LOG_TRS_FILE)
				_Module.m_server.m_cTrsLog.LogPumpTotals(lPumpNumber,nNozzle,nGrade,info.sVolume,info.sValueA);

			//3.2.0.34
			if(CONTROLLER_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite())
			{
				bWriteReadInfo = TRUE;
			}
			else
			{
				if(_Module.m_server.IsShiftChangeInProgress() )
				{
					if  (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetShiftNumber() != _Module.m_server.GetActiveShiftNumber())
					{
						bWriteReadInfo = TRUE;
					}
					else
					{ 
						// M.L 16.7.98
						if(LOG_LOGIC_ERROR)
							_LOGMSG.LogMsg("Unexpected pump totals, record ignored  !!! ", LOG_LEVEL_1);								

						bWriteReadInfo = FALSE;
					}
				}
				else
				{
					bWriteReadInfo = TRUE;
				}
			}

			if(bWriteReadInfo)
			{
				DWORD   dwTmp;								
					
				if(_Module.m_server.NozzleLevelLogic())
					if(nNozzle)
						nFinalGrade = _Module.m_server.m_cPumpArray[lPumpNumber-1].NozzleToGrade(nNozzle);

				//3.2.0.34 end
				_Module.m_server.m_cParam.LoadParam("Pumps", "VolumeMultiply",			&dwTmp, (DWORD)1, FALSE);

				if (dwTmp > 1 )
				{
					if ((dwTmp%10) == 0)
					{
						
						double  x = log10((double)dwTmp);

						for (int j=(int)x ;j> 0;j--)
						{
							for (int i =1;MAX_NAME_LENGTH > i ; i++)
							{
								info.sVolume[i -1] = info.sVolume[i];
								
							}
							
							info.sVolume[MAX_NAME_LENGTH - j] = '0';	
						}			
												 						  											
					}
					
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;					
						str.Format("Total Volume %.20s The volume was multiply by =(%ld)" , 
							info.sVolume ,
							(long)dwTmp);
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str); 				
					}
	
				}
				//3.2.0.34 end
				//4.0.12.505
				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

				info.lIndexNumber	= _Module.m_server.NewPumpTotalIndex();

				if (slPumpArray.IsLocked())
					slPumpArray.Unlock();

				info.lPump			= lPumpNumber;
				info.nNozzle		= nNozzle;
				info.nGrade			= nFinalGrade;
				info.nFlags			= nFlags;
				if ( !_Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags() ) //4.0.8.500
					info.nFlags |= TOTALS_ON_DEMAND_REQUEST;
				else
					info.nFlags |= TOTALS_END_OF_SHIFT; //4.0.23.500 - TD 77765

				info.byPumpStatus	= (BYTE)lPumpStatus;
				info.byNozzleStatus	= (BYTE)lNozzleStatus;
				info.dwActiveReadNumber = _Module.m_server.GetActiveReadNumber();
				info.dwShiftNumber	= _Module.m_server.m_cPumpArray[lPumpNumber-1].GetShiftNumber();
				GetLocalTime(&info.cSysTime);		
				
				//4.0.25.50 - TD 108311
				if (_Module.m_server.GetSingleShiftAccumulationsActive())
				{
					SINGLE_SHIFT_ACCUM_PUMP_POSITION_RECORD cSingleShiftAccumPumpPositionRec;
					if (!_Module.m_server.m_cSingleShiftAccumMgr.GetPumpPositionRecord(lPumpNumber, nNozzle, cSingleShiftAccumPumpPositionRec))
					{
						info.lShiftAccumulatedPumpTest = cSingleShiftAccumPumpPositionRec.lShiftAccumulatedPumpTest;
						info.lShiftAccumulatedSales = cSingleShiftAccumPumpPositionRec.lShiftAccumulatedSales;
					}
				}

				if(LOG_BASIC_FLOW_CONTROL)//4.0.12.505
				{
					CString str;
					str.Format("PumpTotals2 new index added %ld",info.lIndexNumber);
					_LOGMSG.LogMsg(str);
				}
				//4.0.14.350 - protect writing to Qdex when flags are off.
				if(_Module.m_server.m_cServerInfo.GetMemoProcessFlags() == FALSE)
				{
					long lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_INSERT,&info);
					if(lRtc)
					{
						retCode = GCI_FAILED_IO;
					}
				}

				//4.0.1.32 Start
				if(_Module.m_server.IsShiftChangeInProgress() && _Module.m_server.GetWetstockMaintFlag())
				{
					CShiftTotals cShiftTotals;
					cShiftTotals.Write(info);
				}
				//4.0.1.32 End
				//CR_475695_Start
				if (nFlags & TOTALS_END_OF_LIST )
				{
					CString str;
					str.Format("info.IndexNumber %ld, info.PumpNumber %ld ,info.nFlags 0X%08x ", info.lIndexNumber,info.lPump,info.nFlags);
					_Module.m_server.m_cLogger.LogMsg(str);
				}
				//CR_475695_End
			}

		}

		//4.0.8.500
		if (nFlags & TOTALS_END_OF_LIST )
		{
			static long lstasticNumberOfEntries = 0; //4.0.17.501 merge from 4.0.15.180
			
			ReleasePumpTotalPump(lPumpNumber);			

			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			if (_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_PUMP_TOTALS )				
			{
				//We want the event to be sent only when the last pump data has arrived !!!: 4.0.15.180
				
				if( (_Module.m_server.m_cServerInfo.GetMemoProcessFlags() == TRUE) && 
					!(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_POS_PUMP)) //4.0.14.400 4.0.15.180
				{		
					_Module.m_server.m_cServerInfo.SetSendMemoPumpEvent(TRUE);	 //4.0.15.182																
				}

				_Module.m_server.m_cServerInfo.ClearPumpTotalsFlags();
			}
			if (nFlags & TOTALS_END_OF_LIST )
			{
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString strLog;
					strLog.Format("pump totals end of list flags received");
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strLog);
				}
			}

			if(_Module.m_server.m_cServerInfo.GetSendMemoPumpEvent() == TRUE) //4.0.15.200
			{
				//Count the number of times we reach this. The number of TOTALS_END_OF_LIST times 
				//Should be equal to the number of reported Pumps by the 
				lstasticNumberOfEntries = lstasticNumberOfEntries + 1;
			}
			
			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();

			//Add by Yaron for Ctrl Events
			_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_PUMPS_TOTALS,0);

			if (lstasticNumberOfEntries == lNumberOfPumps && (_Module.m_server.m_cServerInfo.GetSendMemoPumpEvent() == TRUE))
			{
				long lFlags = 0; //4.0.19.502 47235
				if (_Module.m_server.GetStartupBaseTotalsUpdate()) //4.0.19.502 47235
				{
					lFlags = 1;
					_Module.m_server.SetStartupBaseTotalsUpdate(FALSE);
				}
				
				_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_MEMO_PUMPTOTAL_READING, lFlags);	
				lstasticNumberOfEntries = 0;
				_Module.m_server.m_cServerInfo.SetSendMemoPumpEvent(FALSE); //4.0.15.182
				CString strLog;
				strLog.Format("Fire CTRL_EVENT_MEMO_PUMPTOTAL_READING event for %ld pumps.",lNumberOfPumps);
				_LOGMSG.LogMsg(0,LOG_PUMP,strLog);	
			}	
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"PumpTotals2",
			TRUE,retCode,
			FALSE,0);

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;

		str.Format("PumpTotals2(Pump %ld, Nozzle %d, Grade %d, flags %d, VolumeH %ld, VolumeL %ld, ValueAH %ld, ValueBH %ld, ValueAL %ld, ValueBL %ld)",
			lPumpNumber, nNozzle, nGrade , nFlags, lVolumeH , lVolumeL , lValueAH , lValueBH, lValueAL, lValueBL);

		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::DeliveryReport2(long lTankNumber, long lProduct, short nFlags, long lStatus, long lTag, long lVolume, BSTR sStartDateAndTime, BSTR sEndDateAndTime, long lStartVolume, long lEndVolume, long lTemperature, long *pVal)
{
//	_LOGMSG.LogLocationIn("GCI::DeliveryReport2()",0);

	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif
		DeliveryReport2__(lTankNumber,lProduct,nFlags,lStatus,lTag,lVolume,sStartDateAndTime,sEndDateAndTime,lStartVolume,lEndVolume,lTemperature,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::DeliveryReport2")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::DeliveryReport2()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::DeliveryReport2()",0);

	return S_OK;
}

void CGCI::DeliveryReport2__(long lTankNumber, long lProduct, short nFlags, long lStatus, long lTag, long lVolume, BSTR sStartDateAndTime, BSTR sEndDateAndTime, long lStartVolume, long lEndVolume, long lTemperature, long *pVal)
{
	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;
	BOOL bUpdateDataBase = FALSE;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(!(nFlags & DELIVERY_REPORT_NOT_AVAILABLE))
	{
		if(TANK_NOT_VALID(lTankNumber) && lTankNumber != 0) //4.0.12.517
			retCode = GCI_INVALID_TANK_NUMBER;
	}

	if(retCode == GCI_OK)
	{
		bUpdateDataBase = FALSE;

		if(nFlags & DELIVERY_REPORT_NOT_AVAILABLE)
		{
			// not available...
			CString str;			
			//4.0.2.161
			//All tanks
			if(lTankNumber == 0)
			{
				//4.0.8.500 
				str.Format("Delivery report not available for all tanks , Wet stock flags=%ld , command flag=%d" , _Module.m_server.m_cServerInfo.GetWetStockFlags() , nFlags);
				if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite() || (nFlags & DELIVERY_FORCE_ACCEPT_DATA) )  //4.0.6.502)
				{
					if(!_Module.m_server.m_bDeliveryReportRcved)
						_Module.m_server.m_bDeliveryReportRcved = TRUE;				
				}
				if (!(nFlags & DELIVERY_REPORT_END_OF_LIST))
					nFlags += DELIVERY_REPORT_END_OF_LIST;
			}
			else
				str.Format("Delivery report not available for Tank %d! ,Wet stock flags=%ld, command flag=%d",lTankNumber , _Module.m_server.m_cServerInfo.GetWetStockFlags(), nFlags); //4.0.8.500



			if(LOG_BASIC_FLOW_CONTROL)
			{		
				_LOGMSG.LogMsg(str);
			}				

		}
		else
		{
			BOOL bSaveRcvedInfo = FALSE;

			if(CONTROLLER_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite())
			{
				bSaveRcvedInfo = TRUE;
			}
			else if (nFlags & DELIVERY_FORCE_ACCEPT_DATA)  //4.0.6.507
			{
				bSaveRcvedInfo = TRUE;
			}			
			else if (!_Module.m_server.m_bDeliveryReportRcved) 
			{
				bSaveRcvedInfo = TRUE;
			}
			else if(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY )//4.0.1.38
			{
				bSaveRcvedInfo = TRUE;
			}
			//4.0.12.512
			else if (nFlags & DELIVERY_REPORT_INVOICE_DATA)
			{
				bSaveRcvedInfo = TRUE;
			}			

			if(bSaveRcvedInfo)
			{
				DELIVERY_INFO		info;
				FillDeliveryReport(info, lTankNumber, lProduct, lStatus, nFlags, lVolume, lStartVolume, lEndVolume, lTemperature, sStartDateAndTime, retCode, sEndDateAndTime,dwLastErr); // 4.0.19.2170 OmerT   CR 409564

				if ((retCode == GCI_OK) && (_Module.m_server.m_cServerInfo.GetMemoProcessFlags() == FALSE))
				{
					long lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_INSERT,&info);
					if(lRtc)
					{
						retCode = GCI_FAILED_IO;
					}

					if(_Module.m_server.IsShiftChangeInProgress() && _Module.m_server.GetWetstockMaintFlag())
					{
						CShiftDelivery cShiftDelivery;
						cShiftDelivery.Write(info);
					}
				}

				// 4.0.7.501				
				if(_Module.m_server.m_cServerInfo.GetMemoProcessFlags()) //4.0.8.500
				{					
					_Module.m_server.m_cTankArray[lTankNumber-1].SetDeliveryInfo(info.cEndDeliveryDateTime ,lVolume , lStatus); //4.0.8.507
					_Module.m_server.m_cTankArray[lTankNumber-1].SetTankValidity(true);

					if(LOG_BASIC_FLOW_CONTROL)
					{		
						CString str;
						
						str.Format("Delivery information is update to Tank=%ld, Volume=%ld lStatus=%ld",lTankNumber , lVolume , lStatus);
						_LOGMSG.LogMsg(str);
					}	

					
				}
				//end add by Yaron


			}
			else
			{
				// M.L 16.7.98
				if(LOG_LOGIC_ERROR)
					_LOGMSG.LogMsg("Unexpected Delivery report , record ignored  !!! ", LOG_LEVEL_1);
			}
			
		}

		// 4.0.8.500
		if(nFlags & DELIVERY_REPORT_END_OF_LIST)
		{
			bUpdateDataBase = FALSE;
			
			if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_POS )  //4.0.9.555
			{
				if (!(_Module.m_server.dwPosRequestReport & REPORT_TYPE_DERIVERY))
					_Module.m_server.dwPosRequestReport  += REPORT_TYPE_DERIVERY; 

			}
			//4.0.7.501
			else if (_Module.m_server.m_cServerInfo.GetMemoProcessFlags())
			{
				_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_MEMO_DELIVERY_REPORT,0);
				_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,
								0,
								GCI_EVENT_MEMO_DELIVERY_REPORT );		

			}
			else
				//Add by Yaron for Ctrl Events
				_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_DELIVERY_REPORT,0);

			//3.2.0.34
			if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite())
			{
				// update flag if needed			
				if(!_Module.m_server.m_bDeliveryReportRcved)
					_Module.m_server.m_bDeliveryReportRcved = TRUE;				
			}

			//4.0.7.501
			if (_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY )
			{
				_Module.m_server.m_cServerInfo.SetWetStockFlags(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_CLEAR_DELIVERY);

				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString strLog;
					strLog.Format("CGCI::DeliveryReport2 : Set Server Info. Delivery Status: %d, Delivery Process: %d.", 
						_Module.m_server.m_cServerInfo.GetDeliveryEmulationStatus(),
						_Module.m_server.m_cServerInfo.GetDeliveryEmulationProcessType());
						
					_LOGMSG.LogMsg(strLog);
				}
								
				bUpdateDataBase = TRUE;
			}

			if (!( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY ) && 
				!( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS ))

				{
					_Module.m_server.m_cServerInfo.SetMemoProcessFlags(FALSE);
					bUpdateDataBase = TRUE;
				}

			if (bUpdateDataBase)
			{
				_Module.m_server.ClearWetStockProcess(_Module.m_server.m_cServerInfo.GetWetStockFlags());												
				//4.0.8.500				
			}
		} // end of list

	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"DeliveryReport2",
			TRUE,retCode,
			(dwLastErr!=0)? TRUE:FALSE,dwLastErr);



	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;

		str.Format("Get delivery report2(lTankNumber = %ld, Flags %d, Status %ld,Starting Volume %ld, End Volume %ld, Product %ld, Delivered volume %ld, Temperature %ld)",
		lTankNumber,nFlags,lStatus,lStartVolume,lEndVolume,lProduct,lVolume,lTemperature); 
		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::Alarm(long lCatagory, long lCode, long lFlags, BSTR sStartDateAndTime, BSTR sEndDateAndTime, long *pVal)
{
//	_LOGMSG.LogLocationIn("GCI::Alarm()",0);

#ifdef CATCH_GCI
	__try
	{
#endif
		Alarm__(lCatagory,lCode,lFlags,sStartDateAndTime,sEndDateAndTime,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::Alarm")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::Alarm()", LOG_LEVEL_0);

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::Alarm()",0);
	return S_OK;
}

void CGCI::Alarm__(long lCatagory, long lCode, long lFlags, BSTR sStartDateAndTime, BSTR sEndDateAndTime, long *pVal)
{
	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	if(retCode == GCI_OK)
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;				
			str.Format("Alarm: Catagory %d, Code %ld, lFlags %ld", 
				lCatagory,
				lCode,
				lFlags);
			_LOGMSG.LogMsg(str); 				
		}

		SZ_TAG_SYSTEMTIME	TagTime;
		ALARM_INFO			info;
		memset(&info,0,sizeof(ALARM_INFO));

		info.lIndexNumber	= _Module.m_server.NewAlarmIndex();
		info.dwActiveReadNumber = _Module.m_server.GetActiveReadNumber();
		info.dwShiftNumber = _Module.m_server.GetFormalShiftNumber();

		info.lCategory		= lCatagory;
		info.lCode			= lCode;
		info.lFlags			= lFlags;

		if (_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_ALARM )
			info.lFlags |= ALARM_REPORT_ON_DEMAND_REQUEST;
		
		int iLen = WideCharToMultiByte(CP_ACP,0,sStartDateAndTime,-1,(char *)&TagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;
		}
		else
		{
			Tag2SystemTime(&TagTime.sTime,&info.cStartDateTime);
			iLen = WideCharToMultiByte(CP_ACP,0,sEndDateAndTime,-1,(char *)&TagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
			if(iLen == 0)
			{
				dwLastErr = GetLastError();
				retCode = GCI_LAST_ERROR;
			}
			else
			{
				Tag2SystemTime(&TagTime.sTime,&info.cEndDateTime);
			}
		}
			
		if(retCode == GCI_OK)
		{
			long lRtc = _Module.m_server.IndexOperation(ALARM_FILE,REC_INSERT,&info);
			if(lRtc)
			{
				retCode = GCI_FAILED_IO;
			}

			if(lFlags & TOTALS_END_OF_LIST)
			{				
				if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_ALARM )
					_Module.m_server.m_cServerInfo.ClearAlarmFlags();
			}
		}
	}
		
	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"Alarm",
			TRUE,retCode,
			(dwLastErr!=0)? TRUE:FALSE,dwLastErr);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::CarWashStatus(long lDevice, long lSrc, long lStatus, long *pVal)
{
//	_LOGMSG.LogLocationIn("GCI::CarWashStatus()",0);

#ifdef CATCH_GCI
	__try
	{
#endif
		CarWashStatus__(lDevice,lSrc,lStatus,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::CarWashStatus")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::CarWashStatus()", LOG_LEVEL_0);

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::CarWashStatus()",0);
	return S_OK;
}

void CGCI::CarWashStatus__(long lDevice, long lSrc, long lStatus, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	if(retCode == GCI_OK)
	{	
		// update car wash status
		_Module.m_server.m_cCarWash.SetCarWashStatus((CarWashStatus_)lStatus);	//4.0.9.501
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("Car wash status change to %d" , lStatus);
			_LOGMSG.LogMsg(str);							
		}

		//4.0.19.504 - 47559
		if (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse()) // CR 444830
		{
			if (lStatus == CAR_WASH_ONLINE)
				_Module.m_server.m_cGPIEventMgr.AddGeneralSystemEvent(GPI_EVENT_CARWASH_ONLINE, 0, lDevice);
			else
				_Module.m_server.m_cGPIEventMgr.AddGeneralSystemEvent(GPI_EVENT_CARWASH_OFFLINE, 0, lDevice);
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"CarWashStatus",
			TRUE,retCode,
			FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::PriceDisplayPanelStatus( long lDevice , /*[in]*/ long lStatus, /*[in]*/ BSTR sRec ,/*[out, retval] */long *pVal)//CR 404591
{
	//	_LOGMSG.LogLocationIn("GCI::CarWashStatus()",0);

#ifdef CATCH_GCI
	__try
	{
#endif
		PriceDisplayPanelStatus__(lDevice,lStatus , sRec , pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::PriceDisplayPanelStatus")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::PriceDisplayPanelStatus()", LOG_LEVEL_0);

		exit(1);
	}
#endif
	return S_OK;
}

void CGCI::PriceDisplayPanelStatus__( long lDevice, long lPriceDisplayStatus , BSTR sFiller ,long *pVal ) /*CR 404591 */
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	if(retCode == GCI_OK)
	{	
		_Module.m_server.m_cServerInfo.SetPriceDisplayStatus(lPriceDisplayStatus);
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"PriceDisplayPanelStatus__",
			TRUE,retCode,
			FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::WashCodeResult(long lSyncNumber,long lDevice, long lResult, BSTR sData, long *pVal)
{
	//	_LOGMSG.LogLocationIn("GCI::WashCodeResult()",0);

#ifdef CATCH_GCI
	__try
	{
#endif
		WashCodeResult__(lSyncNumber,lDevice,lResult,sData,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::WashCodeResult")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::WashCodeResult()", LOG_LEVEL_0);

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::WashCodeResult()",0);
	return S_OK;
}

void CGCI::WashCodeResult__(long lSyncNumber,long lDevice, long lResult, BSTR sData, long *pVal)
{
	DWORD dwLastErr=0;
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	long lRequestType;
	CAR_WASH_INFO info;
	memset(&info,0,sizeof(CAR_WASH_INFO));

	
	CString str;
	str.Format("CGCI::WashCodeResult__ SyncNumber = %ld Device  %ld Result = %ld",
		lSyncNumber,lDevice,lResult);
	_LOGMSG.LogMsg(str);
	if(retCode == GCI_OK)
	{
		_Module.m_server.m_cCarWash.GetCurrentRequest(&info.lIndexNumber,&lRequestType);
		if ( info.lIndexNumber == 0 )
			retCode = GCI_NO_CAR_WASH_REQUEST;
	}

	if(retCode == GCI_OK)
	{
		switch (lResult)
		{
			case CW_OK_VALID_CODE: //OK //4.0.26.501 144221
			{
				TAG_CAR_WASH_DATA carWashData;
				memset(&carWashData,0,sizeof(carWashData));
  				int iLen = Bstr2Buffer(sData, (void *)&carWashData, sizeof(carWashData));
				if(iLen == 0)
				{
					dwLastErr = GetLastError();
					retCode = GCI_LAST_ERROR;
				}
				
				if(retCode == GCI_OK)
				{
					long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_READ,&info);
					if(lRtc)
					{
						if(LOG_LOGIC_ERROR)
						{
							CString str;
							str.Format("GenerateWashCode: Read Operation for Car wash Failed !!! lRtc %d",lRtc); 
							_LOGMSG.LogMsg(str, LOG_LEVEL_1);						
						}
						retCode = GCI_FAILED_IO;
					}
					else
					{
						if ( CW_TYPE_CANCEL == lRequestType )
						{
							info.lRequestStatus = CW_REQ_RESPONDED;
							//Remove it from the request list

							_Module.m_server.m_cCarWash.RemoveCancelRequest();

						}
						else
						{
							memcpy(info.sCode, carWashData.sCode, sizeof(carWashData.sCode));
							info.lProgramIndex = a2l(carWashData.sProgram, sizeof(carWashData.sProgram));  //RFUEL-2206 FUEL returns wrong Wash Program on GenerateWashCode
							info.lPrice = a2l(carWashData.sPrice, sizeof(carWashData.sPrice));
							Tag2SystemTime(&carWashData.sExpirationDate,&info.cExpirationDate);
							GetLocalTime(&info.cLastUpdate);
							//Check if type was change while waiting.
							if ( info.lRequestType & CW_TYPE_CANCEL )
							{
								//Add it to the cancel list
								_Module.m_server.m_cCarWash.AddCancelRequest(info.lIndexNumber);
								info.lRequestType = CW_TYPE_CANCEL;
								info.lRequestStatus = CW_REQ_OPEN;
								//Remove it from the request list
								_Module.m_server.m_cCarWash.RemoveRequest();

							}
							else
							{
								info.lRequestStatus = CW_REQ_RESPONDED;
								//Remove it from the request list
								_Module.m_server.m_cCarWash.RemoveRequest();

								//4.0.25.30? - 126194
								//Throw the event to terminalsrv to inform that the wash code is ready.
								if (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse())  // CR 444830
		        					_Module.m_server.m_cGPIEventMgr.AddWashCodeReady(lDevice,info.lIndexNumber,lResult);
							}
						}

						long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_UPDATE,&info);

						if(lRtc)
						{
							if(LOG_LOGIC_ERROR)
							{
								CString str;
								str.Format("GenerateWashCode: Update Operation for Car wash Failed !!! lRtc %d",lRtc); 
								_LOGMSG.LogMsg(str, LOG_LEVEL_1);						
							}
							retCode = GCI_FAILED_IO;
						}

					}
				}
				break;
			}
			default: //Failed //4.0.26.501 144221
			{
				long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_READ,&info);
				if(lRtc)
				{
					if(LOG_LOGIC_ERROR)
					{ 
						CString str;
						str.Format("GenerateWashCode: Read Operation for Car wash Failed !!! lRtc %d",lRtc); 
						_LOGMSG.LogMsg(str, LOG_LEVEL_1);						
					}
					retCode = GCI_FAILED_IO;
				}
				else
				{
					//info.bFailed = '1';//amram 4.0.11.500 
					l2a(lResult, &info.bFailed, sizeof(info.bFailed)); //4.0.26.501 144221 the new retcodes start from 4 due to backwards compatibility
					GetLocalTime(&info.cLastUpdate);
					info.lRequestStatus = CW_REQ_RESPONDED;
					if ( CW_TYPE_CANCEL == lRequestType )
					{
						_Module.m_server.m_cCarWash.RemoveCancelRequest();
					}
					else
					{
						_Module.m_server.m_cCarWash.RemoveRequest();
					}
					long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_UPDATE,&info);
					if(lRtc)
					{
						if(LOG_LOGIC_ERROR)
						{
							CString str;
							str.Format("GenerateWashCode: Update Operation for Car wash Failed !!! lRtc %d",lRtc); 
							_LOGMSG.LogMsg(str, LOG_LEVEL_1);
						}
						retCode = GCI_FAILED_IO;
					}

					//4.0.23.500 - TD 76395
					//Throw the event to terminalsrv to inform thath the wash code is ready.
					if (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse())
		        		_Module.m_server.m_cGPIEventMgr.AddWashCodeReady(lDevice,info.lIndexNumber,lResult);
				}
				break;
			}
//			default:;
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"WashCodeResult",
			TRUE,retCode,
			(dwLastErr!=0)? TRUE:FALSE,dwLastErr);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::SetPAPSwipeData(long lPumpNumber, long lFlags, BSTR sData, long *pVal)
{
	//	_LOGMSG.LogLocationIn("GCI::SetPAPSwipeData()",0);

#ifdef CATCH_GCI
	__try
	{
#endif

		if(LOG_BASIC_FLOW_CONTROL)
		{				
			_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,"SetPAPSwipeData",lFlags);
		}

		SetPAPSwipeData__(lPumpNumber,lFlags,sData,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::SetPAPSwipeData")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","SetPAPSwipeData()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif

//	_LOGMSG.LogLocationOut("GCI::SetPAPSwipeData()",0);
	return S_OK;
}

void CGCI::SetPAPSwipeData__(long lPumpNumber, long lFlags, BSTR sData, long *pVal)
{
	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();

	DWORD dwLastErr=0;
	GCIRetCode retCode = GCI_OK;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if((_Module.m_server.IsServerInRecoveryProcess() || (!_Module.m_server.m_cOLASrv->Valid())) 
			&& _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GCI_FAILED_IO;

	if(retCode == GCI_OK)
	{
		if (( _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetRes() == PAY_AT_PUMP ) ||
			(lFlags & GPSD_FOR_CURRENT_DEBIT_PAP ) ||     //4.0.2.21
			(lFlags & GPSD_FOR_STORAGE_PAP_DETAILS) ||
			(lFlags & GPSD_INSERT_EXTRA_INFO))	//4.0.9.501
		{
			PAY_AT_PUMP_INFO savedInfo; //4.0.5.0
			memset(&savedInfo,' ',sizeof(PAY_AT_PUMP_INFO)); //4.0.15.10 amit

			CARD_SALE_ALL_CHIP_EXTRA_DATA	cardDataAll3ChipExtra;	//4.0.20.503 66960
			memset(&cardDataAll3ChipExtra, ' ', sizeof(cardDataAll3ChipExtra));

			int iLen = 0;

			retCode = _Module.m_server.m_cXmlConvertorEx.GetStructCardSaleAll3ChipExtra(sData, sizeof(CARD_SALE_ALL_CHIP_EXTRA_DATA), &cardDataAll3ChipExtra, &iLen);	//4.0.20.503 66960	//4.0.22.504 69344

			if (retCode == GCI_LAST_ERROR)
			{
				dwLastErr = GetLastError();
			}

			if (retCode == GCI_OK)
			{
				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

				//4.0.20.503 66960
				memcpy(&savedInfo.CardSaleInfo.cardData, &cardDataAll3ChipExtra.cCardSaleAll3, sizeof(CARD_SALE_ALL3));//4.0.22.504 69344
				_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&savedInfo);

				//4.0.9.500
				if (lFlags & GPSD_INSERT_EXTRA_INFO)
				{					
					CPumpTransact cTmptrs;
					CString str;
					long lExtraInfoRet= 0; 
					if(!_Module.m_server.m_cPumpArray[lPumpNumber-1].GetInProgressWaitProgressTrs(&cTmptrs))
					{
						if(cTmptrs.m_lRes == PRE_PAY_AT_POS)//4.0.12.502
							retCode = GCI_NO_WAITING_TRS;
						else
						{
							//4.0.12.502
							lExtraInfoRet = _Module.m_server.TrsLinkOperation(REC_INSERT, &cTmptrs ,NULL ,&savedInfo);  																
							if(LOG_BASIC_FLOW_CONTROL)
							{								
								str.Format("SetPAPSwipeData Pump %02ld lFlags=%ld, trs=%ld, Rtc=%ld , status=%c",lPumpNumber,lFlags , cTmptrs.m_lNumber,lExtraInfoRet , savedInfo.CardSaleInfo.cardData.sTranStatus);
								_LOGMSG.LogMsg(str);
							}
							//save the pap info 
							SQLPumpSrvFile.LoadPAPInfo(lPumpNumber);
						}
					}
					else
					{
						if(LOG_BASIC_FLOW_CONTROL)
						{	
							str.Format("SetPAPSwipeData, trs not found");
							_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,str,lFlags);
						}
						// indicate error.
						lExtraInfoRet = 1;						
					}

					if (lExtraInfoRet)
						retCode = GCI_NO_WAITING_TRS;
				}

				if (slPumpArray.IsLocked())
					slPumpArray.Unlock();
			}	
		}
		else
			retCode = GCI_NO_WAITING_TRS;
	}


	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"SetPAPSwipeData",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);
	
	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGCI::GetParam3(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long *pVal)
{
	if( ((sVal == NULL) && (nFiledType == FIELD_SZ)) || 
		((lVal == NULL) && (nFiledType == FIELD_DWORD)) )
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"IGCI::GetParam3   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	
		return E_POINTER;
	}

#ifdef CATCH_GCI
	__try
	{
#endif
		GetParam3__(sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetParam3")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetParam3()", LOG_LEVEL_0);	

		exit(1);
	}
#endif


	return S_OK;
}

void CGCI::GetParam3__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long *pVal, BOOL bIniFile) //4.0.8.503
{
	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;	
	BOOL bAllocEmpty = FALSE;
	CString strEmptyName;
	long lRtc = -1;
	char szTableFieldAndVal[MAX_TABLE_NAME+MAX_TABLE_NAME+MAX_FIELD_NAME+MAX_FIELD_VAL]; 
	memset(szTableFieldAndVal,0,sizeof(szTableFieldAndVal));
	int iLen = WideCharToMultiByte(CP_ACP,0,sParamPosition,-1,szTableFieldAndVal,sizeof(szTableFieldAndVal),NULL,NULL);
	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GCI_TABLE_NAME_TOO_LONG;
	}
	else
	{
		strEmptyName  = szTableFieldAndVal;
		strEmptyName.TrimLeft();
		if(strEmptyName.IsEmpty())
			retCode = GCI_PARAM_NOT_FOUND;
	}

	SysFreeString(sParamPosition);

	char szSubjectName[MAX_TABLE_NAME];
	char szTableName[MAX_TABLE_NAME]; 
	char szSubTableName[MAX_TABLE_NAME];
	char szFieldName[MAX_FIELD_NAME]; 
	char szFieldVal[MAX_FIELD_VAL]; 

	memset(szSubjectName,0,sizeof(szSubjectName));
	memset(szSubTableName,0,sizeof(szSubTableName));
	memset(szTableName,0,sizeof(szTableName));
	memset(szFieldName,0,sizeof(szFieldName));
	memset(szFieldVal,0,sizeof(szFieldVal));


	if(retCode == CTRL_OK)
	{
		BYTE *pTmp;

		BOOL bDelimiterFound =
		_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)szTableFieldAndVal,
														(BYTE *)szSubjectName,
														sizeof(szSubjectName), 
														&pTmp);
		if(bDelimiterFound) 
		{
			bDelimiterFound = 	
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
															(BYTE *)szTableName,
															sizeof(szTableName), 
															&pTmp);
			if(bDelimiterFound) 
			{
				bDelimiterFound = 	
				_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
																(BYTE *)szSubTableName,
																sizeof(szSubTableName), 
																&pTmp);
				if (bDelimiterFound)
				{
					bDelimiterFound = 	
					_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
																	(BYTE *)szFieldName,
																	sizeof(szFieldName), 
																	&pTmp);
				}
				else //  Field Name
				{
					retCode = GCI_PARAM_NOT_FOUND;
				}

			}
			else // Sub Table Name
			{
				retCode = GCI_PARAM_NOT_FOUND;
			}
		}
		else  // Table Name
		{
			retCode = GCI_PARAM_NOT_FOUND;
		}

		if(bDelimiterFound) 
			bDelimiterFound = 	
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
															(BYTE *)szFieldVal,
															sizeof(szFieldVal), 
															&pTmp);
	}

		if(LOG_FCC_FLOW_CONTROL)
		{
			CString str;
			str.Format("TableName %.20s ,Key1 %ld ,Key2 %ld ,FieldName %.20s, FiledType %s, FieldVal %.30s",
						szTableName,lKey1,lKey2,szFieldName,(nFiledType)? "FIELD_SZ":"FIELD_DW",szFieldVal);
			_LOGMSG.LogReq(   0, // Src 
												"ICGI", 
												"GetParam3", 
												(LPCSTR)str  ); 
		}	
	


	BOOL bAllowCreate = bCreate;

	if(nFiledType == FIELD_SZ)
	{
		memset(szFieldVal,0,sizeof(szFieldVal));

		//3.0.1.61 start
		iLen = WideCharToMultiByte(CP_ACP,0,*sVal,-1,szFieldVal,sizeof(szFieldVal),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_DEFAULT_FIELD_VAL_TOO_LONG;
		}
		SysFreeString( *sVal );
		//3.0.1.61 end

		if(retCode == GCI_OK)
		{  			
			lRtc =  _Module.m_server.m_cParam.GetParam3Level(szSubjectName, szTableName,szSubTableName, lKey1, lKey2, szFieldName,nFiledType,szFieldVal,sizeof(szFieldVal),NULL,bAllowCreate ,bIniFile); 
			if(lRtc != 0) 
			{
				bAllocEmpty = TRUE;
			}
			else
			{
				CString strRetVal(szFieldVal);			
				if(strRetVal.GetLength())
				{
					( *sVal ) = strRetVal.AllocSysString(); //3.0.1.61
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

		//4.0.3.27
		if (bAllocEmpty)
		{
			lRtc = -1;
			retCode = GCI_FIELD_EMPTY;		
		}
		//4.0.3.27	
	}
	else // field DW 
	{
		DWORD dwRetVal = *lVal;

		if(retCode == GCI_OK)	
			lRtc =  _Module.m_server.m_cParam.GetParam3Level(szSubjectName, szTableName,szSubTableName, lKey1, lKey2, szFieldName,nFiledType,NULL,0,&dwRetVal,bAllowCreate, bIniFile); 

		*lVal = (long)dwRetVal;

		bAllocEmpty = TRUE;
	}


	if(bAllocEmpty)
	{
		CString strRetVal("  ");
		( *sVal ) = strRetVal.AllocSysString(); //3.0.1.61
	}

	if(lRtc != -1) // lRtc have been used - something happened.
	{
		switch(lRtc)
		{
			case 0 : retCode = GCI_OK; break;
			case 1 : retCode = GCI_FAILED_TO_OPEN_TABLE; break;
			case 3 : retCode = GCI_PARAM_NOT_FOUND_FAILED_TO_CREATE; break;
			case 4 : retCode = GCI_PARAM_NOT_FOUND; break;	
				//4.0.3.21
			default:
				retCode  = ConverHydServiceCode2GciCode((CFGDBRetCode)lRtc);
				//4.0.3.21

		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
													0, // Src
													"IGCI",
													"GetParam3",
													TRUE,retCode,
													(dwLastErr!=0)? TRUE:FALSE,dwLastErr);


	if(pVal != NULL)
			*pVal = retCode;
}


STDMETHODIMP CGCI::SetParam3(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal)
{
#ifdef CATCH_GCI
	__try
	{
#endif
		SetParam3__(sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::SetParam3")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetParam3()", LOG_LEVEL_0);

		exit(1);
	}
#endif

	
//	_LOGMSG.LogLocationOut("GCI::SetParam()",0);
	return S_OK;
}

void CGCI::SetParam3__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal ,BOOL bIniFile) //4.0.8.503
{
	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;
    CString strEmptyName;
	char szTableFieldAndVal[MAX_TABLE_NAME+MAX_TABLE_NAME+MAX_FIELD_NAME+MAX_FIELD_VAL]; 
	char szFieldVal[MAX_FIELD_VAL]; 
	memset(szTableFieldAndVal,0,sizeof(szTableFieldAndVal));

	int iLen = WideCharToMultiByte(CP_ACP,0,sParamPosition,-1,szTableFieldAndVal,sizeof(szTableFieldAndVal),NULL,NULL);
	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GCI_TABLE_NAME_TOO_LONG;
	}
	else
	{
		strEmptyName  = szTableFieldAndVal;
		strEmptyName.TrimLeft();
		if(strEmptyName.IsEmpty())
			retCode = GCI_PARAM_NOT_FOUND;
	}

	SysFreeString(sParamPosition);

	memset(szFieldVal,0,sizeof(szFieldVal));
	if(nFiledType == FIELD_SZ)
	{
		iLen = WideCharToMultiByte(CP_ACP,0,sVal,-1,szFieldVal,sizeof(szFieldVal),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_DEFAULT_FIELD_VAL_TOO_LONG;
		}
	}
 
	char szSubjectName[MAX_TABLE_NAME];
	char szTableName[MAX_TABLE_NAME]; 
	char szSubTableName[MAX_TABLE_NAME];
	char szFieldName[MAX_FIELD_NAME]; 

	memset(szSubjectName,0,sizeof(szSubjectName));
	memset(szSubTableName,0,sizeof(szSubTableName));
	memset(szTableName,0,sizeof(szTableName));
	memset(szFieldName,0,sizeof(szFieldName));
	memset(szFieldVal,0,sizeof(szFieldVal));


	if(retCode == CTRL_OK)
	{
		BYTE *pTmp;

		BOOL bDelimiterFound =
		_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)szTableFieldAndVal,
														(BYTE *)szSubjectName,
														sizeof(szSubjectName), 
														&pTmp);
		if(bDelimiterFound) 
		{
			bDelimiterFound = 	
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
															(BYTE *)szTableName,
															sizeof(szTableName), 
															&pTmp);
			if(bDelimiterFound) 
			{
				bDelimiterFound = 	
				_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
																(BYTE *)szSubTableName,
																sizeof(szSubTableName), 
																&pTmp);
				if (bDelimiterFound)
				{
					bDelimiterFound = 	
					_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
																	(BYTE *)szFieldName,
																	sizeof(szFieldName), 
																	&pTmp);
				}
				else //  Field Name
				{
					retCode = GCI_PARAM_NOT_FOUND;
				}

			}
			else // Sub Table Name
			{
				retCode = GCI_PARAM_NOT_FOUND;
			}
		}
		else  // Table Name
		{
			retCode = GCI_PARAM_NOT_FOUND;
		}
	}
	
	if(retCode == GCI_OK)
	{  
		// bCreate forse to FASLE - 
		// creating parameters via GCI is not allowed 
		long lRtc=0;
		if(nFiledType == FIELD_SZ)
		{
	  		lRtc = _Module.m_server.m_cParam.SetParam3Level(szSubjectName, szTableName,szSubTableName, lKey1, lKey2, szFieldName,nFiledType,szFieldVal,0,FALSE ,bIniFile);  //4.0.8.503
		}
		else
		{
	  		lRtc = _Module.m_server.m_cParam.SetParam3Level(szSubjectName, szTableName,szSubTableName, lKey1, lKey2, szFieldName,nFiledType,NULL,lVal,FALSE ,bIniFile);  //4.0.8.503
		}

	 	if(lRtc)
		{
			switch(lRtc)
			{
				case 1: retCode = GCI_FAILED_TO_OPEN_TABLE;break;		 
				case 2: retCode = GCI_PARAM_NOT_FOUND;break;
				//4.0.3.21
				default:
				retCode  = ConverHydServiceCode2GciCode((CFGDBRetCode) lRtc);
				//4.0.3.21
					
			}
			
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"SetParam3",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);

	
	if(pVal != NULL)
		*pVal = retCode;
}

/******************************************************************************
 Description:	Setting a process - exposed interface 
 Returns:      	
 Parameters:    long lProcessNumber, long lParam1, long lParam2, long *pVal

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			10/02/2004   11:48		Start
******************************************************************************/
STDMETHODIMP CGCI::SetProcess(long lProcessNumber, long lParam1, long lParam2, long *plRetVal)
{
#ifdef CATCH_GCI
	__try
	{
#endif
// 		char str[256];
// 		sprintf(str,"Set process from GCI process = %ld lParam1 = %ld, lParam2 = %ld", lProcessNumber, lParam1, lParam2);
// 		_LOGMSG.LogMsg(str);
		SetProcess__(lProcessNumber, lParam1, lParam2, plRetVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::SetProcess")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetProcess()", LOG_LEVEL_0);

		exit(1);
	}	
#endif 

	return S_OK;
}

/******************************************************************************
 Description:	Setting a process 
 Returns:      	
 Parameters:    long lProcessNumber, long lParam1, long lParam2, long *pVal

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			10/02/2004   11:54		Start
 AmitH          15/09/2011   00:00      TD 257858 - allow PROCESS_CHANGE_PRICES even if In Recovery Process 
******************************************************************************/

void CGCI::SetProcess__(long lProcessNumber,long lParam1,long lParam2,long * pVal)
{
	GCIRetCode lRetCode = GCI_OK;

	/******************************************************************************************************/
	//4.0.28.503 - do not verify middle of process/recovery incase of PROCESS_CHANGE_PRICES.TD 257858
	/******************************************************************************************************/
	if((_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess) &&
	   (lProcessNumber != PROCESS_CHANGE_PRICES))
	{
		lRetCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
	}

	if ((lRetCode == GCI_OK) || (PROCESS_RECONFIGURE == lProcessNumber))
	{
		_Module.m_server.m_cCommonFunction.SetProcess(lProcessNumber, lParam1, lParam2, 0, 0, (long*)&lRetCode);

		switch (lRetCode)
		{
			case GPI_FAILED_TO_OPEN_TABLE:	
				lRetCode = GCI_FAILED_TO_OPEN_TABLE;
				break;
			case GPI_PARAM_NOT_FOUND:
				lRetCode = GCI_PARAM_NOT_FOUND;
				break;
			case GPI_INVALID_PUMP_NUMBER:	
				lRetCode = GCI_INVALID_PUMP_NUMBER;
				break;
			case GPI_NOT_VALID_IN_CURRENT_MODE:	
				lRetCode = GCI_NOT_VALID_IN_CURRENT_MODE;
				break;
			case GPI_FAILED_IO:
				lRetCode = GCI_FAILED_IO;
				break;
			case GPI_DELIVERY_START_AFTER_START:
				lRetCode = GCI_DELIVERY_START_AFTER_START;
				break;
			case GPI_DELIVERY_END_AFTER_END:
				lRetCode = GCI_DELIVERY_END_AFTER_END;
				break;
			case GPI_DELIVERY_START_IN_MIDDLE_OF_END:
				lRetCode = GCI_DELIVERY_START_IN_MIDDLE_OF_END;
				break;
			case GPI_DELIVERY_END_IN_MIDDLE_OF_START:
				lRetCode = GCI_DELIVERY_END_IN_MIDDLE_OF_START;
				break;
			case GPI_OK:
				lRetCode = GCI_OK;
				break;
			default:
				lRetCode = GCI_NOT_SUPPORTED;
		}
	}

	if(lRetCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"SetProcess",
				TRUE,lRetCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = lRetCode;
}
//Udi 4.0.5.24 end

STDMETHODIMP CGCI::GetReceiptRowData(long lPumpNumber, BSTR sTrack2, short nIndex, BSTR *sReceiptRowData, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		GetReceiptRowData__(lPumpNumber, sTrack2, nIndex, sReceiptRowData, lFlags, pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetReceiptRowData")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","GetReceiptRowData()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif


	return S_OK;
}

void CGCI::GetReceiptRowData__(long lPumpNumber, BSTR sTrack2, short nIndex, BSTR *sReceiptRowData, long lFlags, long *pVal)
{

	//4.0.5.0
	PAY_AT_PUMP_INFO	PayAtPumpInfo;
	CPumpTransact		trs;
	TAG_OLA_REC5		tmpTagOlaRec;	
	TAG_OLA_REC5_EX		tagOlaRec;	
	GCIRetCode			retCode = GCI_OK;
	DWORD				dwLastErr;
	long				lRtc = 0; 
	long				lOlaRecSize = (lFlags & SWIPE_THREE_OLA_REC)?sizeof(TAG_OLA_REC5_EX):sizeof(TAG_OLA_REC3_EX);
	short				lPosIndex = 0; 
	CXMLPointers		cTmpPtr;

	char szTrack2[MAX_TABLE_NAME]; 
	memset(szTrack2,0,sizeof(szTrack2));

	//4.0.18.501
	CReceiptInfo	Info;
	CReceiptInfo	TmpInfo;

	int iLen = WideCharToMultiByte(CP_ACP,0,sTrack2,-1,szTrack2,sizeof(szTrack2),NULL,NULL);

	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode =GCI_NO_RECEIPT;
	}
	else
	{
		BOOL bFound = FALSE;
		//memcpy( Info.cReceipt1.Index.sTrack2, szTrack2, TRACK_2_DATA_SIZE);
		
		lRtc = CSQLReceiptDB::GetInstance().GetFirstReceiptRecord(Info);
		if (lRtc == OK)
		{
			if (lPumpNumber)
			{
				memset(&PayAtPumpInfo, 0, sizeof(PAY_AT_PUMP_INFO));
				_Module.m_server.ConvertReceiptToOla(TmpInfo, &trs, &PayAtPumpInfo);
				if (trs.m_lPump == lPumpNumber)
				{
					TmpInfo = Info;
					bFound = TRUE;
				}
			}
			else
			{
				TmpInfo = Info;
				bFound = TRUE;
			}
		}


		// if receipt found , it's mine that we allowed to print.(in ReceiptPrinted we delete old one)
		if (bFound)
		{		
			//TD 38654
			memset(&tmpTagOlaRec,' ',sizeof(TAG_OLA_REC5));
			memset(&tagOlaRec,' ',sizeof(TAG_OLA_REC5_EX));
			memset(&PayAtPumpInfo,0,sizeof(PAY_AT_PUMP_INFO));

			_Module.m_server.ConvertReceiptToOla(TmpInfo, &trs, &PayAtPumpInfo);


			l2a(PayAtPumpInfo.CardSaleInfo.lIndexNumber,tagOlaRec.lIndexNumber, sizeof(tagOlaRec.lIndexNumber));
			tagOlaRec.cardData	= PayAtPumpInfo.CardSaleInfo.cardData;
			tagOlaRec.extraData = PayAtPumpInfo.CardSaleInfo.extraData;
			tagOlaRec.extraData2 = PayAtPumpInfo.CardSaleExtraData2;
			trs.GetTag(&tmpTagOlaRec.tagTrs,2);

			if (lFlags & SWIPE_THREE_OLA_REC)
				tagOlaRec.extraData3 = PayAtPumpInfo.CardSaleExtraData3;

			//TD 38654, 40492
			_Module.m_server.m_cCommonFunction.ConvertFromTagOlaRec5ExToTagOlaRec5(&tagOlaRec,&tmpTagOlaRec);

			//4.0.14.500
			if (_Module.m_server.m_bUseXmlBuffer)
			{
				char sXmlBuff[MAX_XML_BUF_LEN];
				//Convert to struct
				//TD 38654, 40492
				//_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml(sXmlBuff,&tagOlaRec);
				_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml(sXmlBuff,&tmpTagOlaRec);
				CComBSTR strResult(sXmlBuff);
				*sReceiptRowData = strResult.Copy();                
			}
			else
			{
				BYTE *sTmpAllocationBuffer = new BYTE[lOlaRecSize+2];
				Str2StrNull((BYTE *)&tmpTagOlaRec,lOlaRecSize,sTmpAllocationBuffer);
				CComBSTR strResult(lOlaRecSize +2 ,(LPCSTR)sTmpAllocationBuffer); 
				delete[] sTmpAllocationBuffer; //4.0.5.0
				*sReceiptRowData = strResult.Copy();                
			}
			
		}else
			retCode = GCI_NO_RECEIPT;

	}

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		//4.0.5.12
		CString str;				
		str.Format("GetReceiptRawData(PumpNumber = %ld, Flags %ld, nIndex %d)",lPumpNumber,lFlags,nIndex); 
		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}


	*pVal = retCode;
}

//3.0.0.8
GCIRetCode CGCI::SwipeSaveData(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CARD_SALE_ALL3 *pCardData, BOOL *pbAskUserToTryAgain)
{
	GCIRetCode retCode = GCI_OK;

	CPumpStatus stat;
	stat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();




	if ((PUMPSRV_IS_MODE_MASTER) &&
		(MODE_VALID(stat.GetMode()) && _Module.m_server.m_cModeArray[stat.GetMode() - 1].PAPNotValid()) &&
		(pCardData->CardSaleAll.data.sTranType != CD_TYPE_AVI)) // 4.0.5.27		
	{
		retCode = GCI_NOT_VALID_IN_CURRENT_MODE;
	}
	else if ((PUMPSRV_IS_MODE_MASTER) &&  // 4.0.5.27
		(MODE_VALID(stat.GetMode()) && _Module.m_server.m_cModeArray[stat.GetMode() - 1].PayAVINotValid()) &&
		(pCardData->CardSaleAll.data.sTranType == CD_TYPE_AVI))
	{
		retCode = GCI_NOT_VALID_IN_CURRENT_MODE;
	}
	else if (OLA_STATE(pOla->m_byState) != OLA_NONE)  //3.2.1.44
	{
		retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
	}
	else
	{
		// Constructing and Saving PAY_AT_PUMP_INFO
		//----------------------------------------

		long lLastSeqNumber = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPSeqNumber();

		CString sSiteID = _Module.m_server.GetSiteID();
		memcpy(pCardData->extData2.sSiteId, (LPCTSTR)sSiteID, min(sSiteID.GetLength(), sizeof(pCardData->extData2.sSiteId)));
		long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_VIT_CONFLICT, lPumpNumber, lSeqNumber, pCardData);
		if (lRtc)
			retCode = GCI_FAILED_IO;
	}

	return retCode;
}

GCIRetCode CGCI::SwipeSaveDataEx(long lPumpNumber, long lSeqNumber, OLA_STAT *pOla, CXMLInterface* pCardData, BOOL *pbAskUserToTryAgain)
{
	GCIRetCode retCode = GCI_OK;

	CPumpStatus stat;
	stat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();

	if ((PUMPSRV_IS_MODE_MASTER) &&
		(MODE_VALID(stat.GetMode()) && _Module.m_server.m_cModeArray[stat.GetMode() - 1].PAPNotValid()) &&
		(pCardData->GetXmlByteField(ATT_TRAN_TYPE) != CD_TYPE_AVI)) // 4.0.5.27		
	{
		retCode = GCI_NOT_VALID_IN_CURRENT_MODE;
	}
	else if ((PUMPSRV_IS_MODE_MASTER) &&  // 4.0.5.27
		(MODE_VALID(stat.GetMode()) && _Module.m_server.m_cModeArray[stat.GetMode() - 1].PayAVINotValid()) &&
		(pCardData->GetXmlByteField(ATT_TRAN_TYPE) == CD_TYPE_AVI))
	{
		retCode = GCI_NOT_VALID_IN_CURRENT_MODE;
	}
	else if (OLA_STATE(pOla->m_byState) != OLA_NONE)  //3.2.1.44
	{
		retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
	}

	else
	{
		// Constructing and Saving PAY_AT_PUMP_INFO
		//----------------------------------------

		long lLastSeqNumber = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPSeqNumber();

		CString sSiteID = _Module.m_server.GetSiteID();
		pCardData->SetXmlField(ATT_SITE_ID, (char*)sSiteID.GetBuffer(1));

		//4.0.18.501
		CARD_SALE_ALL3 cCardSaleAll3;
		memset(&cCardSaleAll3, ' ', sizeof(cCardSaleAll3));
		char sXmlBuff[MAX_XML_BUF_LEN];
		memset(sXmlBuff, 0, sizeof(sXmlBuff));
		if (!pCardData->GetXmlString(sXmlBuff))
			_LOGMSG.LogMsg("CGCI::SwipeSaveDataEx, failed to get xml string", LOG_LEVEL_1);
		_Module.m_server.m_cXmlConvertorEx.ConvertFromXmlToStruct(sXmlBuff, &cCardSaleAll3);
		long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_VIT_CONFLICT, lPumpNumber, lSeqNumber, &cCardSaleAll3);
		if (lRtc)
			retCode = GCI_FAILED_IO;
	}

	return retCode;
}
							  
STDMETHODIMP CGCI::GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *sRetData, long *pVal)
{

#ifdef CATCH_GCI
	__try
	{
#endif
		GetCardInfo__(sAccountNumber, sTrack1, sTrack2, sShortCardName, sRetData, pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetCardInfo")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetCardInfo()", LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::GetCardInfo__(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *sRetData, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	_LOGMSG.LogMsg("Insert GetCardInfo");	

	if (_Module.m_server.CheckOLA() !=  OLA_SERVER_ON)	
	{	
		retCode = GCI_OLA_NOT_ACTIVE;
	}
	
	if(retCode == GCI_OK)
	{
		long lRtc = 0;

		lRtc = _Module.m_server.GetCardInfo(sAccountNumber, sTrack1, sTrack2, sShortCardName, sRetData);
		
	}

	_LOGMSG.LogMsg("Exit GetCardInfo");		
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"GetCardInfo",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::GetReceiptCounter(long *plCounter, long lType, long lFlags, long *pVal)
{

#ifdef CATCH_GCI
	__try
	{
#endif
		GetReceiptCounter__(plCounter, lType, lFlags, pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetReceiptCounter")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetReceiptCounter()", LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::GetReceiptCounter__(long *plCounter, long lType, long lFlags, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	if(retCode == GCI_OK)
		_Module.m_server.GetReceiptCounter(plCounter, lType, lFlags);
	
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"GetReceiptCounter",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CGCI::GetServiceFee(long lPump, long lTrsType, long lTrsNumber, BSTR stTime, long lFlags, long *pValue, long *pRetVal)
{
#ifdef CATCH_GCI
	__try
	{
#endif
		GetServiceFee__(lPump, lTrsType,lTrsNumber ,stTime, lFlags, pValue,pRetVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetServiceFee")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","GetServiceFee()",lPump,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::GetServiceFee__(long lPump, long lTrsType,long lTrsNumber, BSTR stTime, long lFlags, long *pValue, long *pRetVal)
{
	long  retCode = GCI_OK;

	_Module.m_server.GetServiceFeeInfo(lPump, lTrsType, lTrsNumber, stTime, lFlags, pValue, &retCode);

	switch (retCode) 
	{
	case SERVICE_FEE_OK:
			retCode = GCI_OK;
		break;
	case SERVICE_FEE_NOT_VALID: 
			retCode = GCI_SERVICE_FEE_NOT_VALID;
		break;
	case SERVICE_FEE_INVALID_PUMP_NUMBER:
			retCode = GCI_SERVICE_FEE_NOT_VALID;
		break;
	case SERVICE_FEE_TRS_NOT_FOUND:
			retCode = GCI_TRS_NOT_FOUND;
		break;
	case SERVICE_FEE_PARAM_NOT_FOUND:
			retCode = GCI_PARAM_NOT_FOUND;
		break;

	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"GetServiceFee",
				TRUE,retCode,
				FALSE,0);

	*pRetVal = retCode;
}

STDMETHODIMP CGCI::ReportRequest(long lParam1, long lParam2, long lFlags, BSTR sData, long *lRefNum, long *pVal)
{
#ifdef CATCH_GCI
	__try
	{
#endif
		ReportRequest__(lParam1, lParam2, lFlags, sData, lRefNum, pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::ReportRequest")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::ReportRequest()", LOG_LEVEL_0);

		exit(1);
	}
#endif

return S_OK;
}

void CGCI::ReportRequest__(long lParam1, long lParam2, long lFlags, BSTR sData, long *lRefNum, long *pVal)
{
	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;
	long lOlaRecSize = (lFlags & SWIPE_THREE_OLA_REC)?sizeof(CARD_SALE_ALL3):sizeof(CARD_SALE_ALL2);

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	CXMLInterface cXMLInterface;
	char sXmlBuff[MAX_XML_BUF_LEN];
	memset(&sXmlBuff,0,MAX_XML_BUF_LEN);
	int iLen = 0;

	//if the buffer is not as xml, convert it to xml
	if (!(_Module.m_server.m_bUseXmlBuffer))
	{

		CARD_SALE_ALL3 cardDataAll3;
		memset(&cardDataAll3,0,sizeof(CARD_SALE_ALL3));

		iLen = Bstr2Buffer(sData, (void *)&cardDataAll3,lOlaRecSize);

		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;
		}
		else
		{

			//Convert the struct to xml
			if (!(_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml(sXmlBuff,&cardDataAll3)))
			{
				retCode = GCI_FAILED_IO; 
			}
		}
	}
	else
	{
		//The buffer is already in xml format
		iLen = Bstr2Buffer(sData, (void *)&sXmlBuff,MAX_XML_BUF_LEN);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;
		}
	}

	//Init the xml interface
	if (cXMLInterface.InitDoc())
	{
		//Load the xml buffer we got to xml interface

		if (sXmlBuff[0])
		{
			if (!cXMLInterface.LoadXmlString(sXmlBuff))
			{
				retCode = GCI_FAILED_IO; 
			}
		}
		else
			retCode = GCI_FAILED_IO; 
	}
	else
	{
		retCode = GCI_FAILED_IO; 
	}

	if ( retCode == GCI_OK )
	{

		short nReport;

		//rt change
		//Temp convert to struct from xml.
		CARD_SALE_ALL3 cardDataAll3;
		memset(&cardDataAll3,' ',sizeof(cardDataAll3));
		memset(&sXmlBuff,0,sizeof(sXmlBuff));  //4.0.15.11
		cXMLInterface.GetXmlString(sXmlBuff);
		_Module.m_server.m_cXmlConvertorEx.ConvertFromXmlToStruct(sXmlBuff,&cardDataAll3);
		//rt end 

		BYTE *pTmp = (BYTE *) &cardDataAll3;
		for(int i=0;i < lOlaRecSize ; i++)
			if( pTmp[i] == 0) pTmp[i] = ' ';

		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("ReportRequest  : Buffer [%.150s]",&cardDataAll3);
			_LOGMSG.LogMsg(str);
		}

		EnterCriticalSection( &_Module.m_server.m_csReportArray );

		//get free entry in ReportArray
		nReport = GetReportFreeEntry();

		if (nReport != -1)
		{
			OLA_STAT ola;
			PAY_AT_PUMP_INFO info;
	
			memset(&ola,0,sizeof(OLA_STAT));
			memset(&info,' ',sizeof(PAY_AT_PUMP_INFO) );	
			memcpy(&info.CardSaleInfo.cardData	 ,&cardDataAll3 , lOlaRecSize);

			//update data in ReportArray
			ola.m_byState = SESSION_GENERATE_REPORT | OLA_REQUEST_TO_SEND ;
					
			_Module.m_server.m_cReportArray[nReport].SetOlaStat(&ola); 	
			_Module.m_server.m_cReportArray[nReport].SetPAPInfo(&info);
			_Module.m_server.m_cReportArray[nReport].SetRequestStatus(RS_WAIT_TO_SEND);

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("ReportRequest written to ReportArray[%d], DeviceNumber=[%.2s], OriginTrans=[%.1s]",nReport, info.CardSaleInfo.cardData.sDeviceNumber, &info.CardSaleExtraData2.sOriginTrans);
				_LOGMSG.LogMsg(str);
			}

		}
		else
		{
			if(LOG_ERROR)
			{
				CString str;
				str.Format("ReportRequest  : NO PLACE IN REPORT ARRAY !!");
				_LOGMSG.LogMsg(str);
			}
		}

		LeaveCriticalSection( &_Module.m_server.m_csReportArray );

	}

	cXMLInterface.CloseDoc();
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"ReportRequest",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);

	if(pVal != NULL)
		*pVal = retCode;

}

short CGCI::GetReportFreeEntry()
{
	short nReport = -1;
	
	for (int i=1; i<MAX_REPORTS; i++)
		if (_Module.m_server.m_cReportArray[i].GetRequestStatus() == RS_EMPTY)
		{
			nReport = i;
			break;
		}
		
	return nReport;
}

//4.0.20.503 66960
BOOL CGCI::AVIAlreadyInUse(CARD_SALE_ALL3* pCardData, long lPumpNumber)
{
	PAY_AT_PUMP_INFO Info;
	BOOL found = FALSE;

	for (int p = 0; p < _Module.m_server.GetLastValidPump(); p++)  //4.0.0.79
	{
		if (p == lPumpNumber -1)
			continue;
		if (_Module.m_server.m_cPumpArray[p].GetStat().GetRes() == PAY_AT_PUMP)
		{
			_Module.m_server.m_cPumpArray[p].GetPAPInfo(&Info);
			
			if (Info.CardSaleInfo.cardData.sTranType == CD_TYPE_AVI)
			{//4.0.20.503 66960
				if (!memcmp(Info.CardSaleInfo.cardData.sAccountNumber, pCardData->CardSaleAll.data.sAccountNumber, sizeof(Info.CardSaleInfo.cardData.sAccountNumber)))
				{//4.0.20.503 66960
					if (!memcmp(Info.CardSaleInfo.cardData.sFullVehicleId, pCardData->CardSaleAll.data.sFullVehicleId, sizeof(Info.CardSaleInfo.cardData.sFullVehicleId)))
					{
						found = TRUE;
						break;
					}
				}
			}
		}
	}

	return found;
}

STDMETHODIMP CGCI::ReceiptPrinted(BSTR sTrack2, long lIndex, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif		
		ReceiptPrinted__(sTrack2, lIndex, lFlags, pVal);
#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::ReceiptPrinted")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::ReceiptPrinted()", LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;
}

STDMETHODIMP CGCI::GetReceiptRawData2(long lPumpNumber, BSTR sTrack2, short *nIndex, BSTR *sReceiptRawData, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif		
		GetReceiptRawData2__(lPumpNumber, sTrack2, nIndex, sReceiptRawData, lFlags, pVal);
#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetReceiptRawData2")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","GetReceiptRawData2()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif
	return S_OK;
}

STDMETHODIMP CGCI::GetSingleProductInfo(long lPump, long lFlags, long *lGrade, long *pVal)
{
	
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif		
		GetSingleProductInfo__(lPump, lFlags, lGrade, pVal);
#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetSingleProductInfo")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","GetSingleProductInfo()",lPump,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif
	return S_OK;
}

//4.0.18.501
void CGCI::ReceiptPrinted__(BSTR sTrack2, long lIndex, long lFlags, long *pVal)
{
	GCIRetCode			retCode = GCI_OK;
	long				lRtc = 0; 
	long				lPosIndex = lIndex; 
	DWORD				dwLastErr;
	CReceiptInfo		ReceiptInfo;
	CPumpTransact		trs;
	int					iLen = 0;
	CString				str;
	char				szTrack2[ENCRYPTED_DATA_SIZE]; 
	auto & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
	memset(szTrack2, 0, sizeof(szTrack2));

	if(retCode == GCI_OK)
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			str.Format("GCI::ReceiptPrinted, request to update lIndex=%ld, lFlags=0x%x" ,  lIndex , lFlags);
			_LOGMSG.LogMsg(str);
		}

		if (lFlags & RECEIPT_INDEX_IS_TRS || lFlags & RECEIPT_TRACK2_HOLDS_TRS_NUM)
		{
			CString sTrsNumber(sTrack2);
			int lTrsNumber = _ttoi(sTrsNumber);
			str.Format("GCI::ReceiptPrinted, Trs %ld - incrment number of copies", lTrsNumber);
			_LOGMSG.LogMsg(str);
			lRtc = PumpSrvDatabase.ReceiptIncrementNumberOfCopies(lTrsNumber, (lFlags & RECEIPT_ORIGINAL_PRINTED) ? true : false);
		}
		else
		{	
			iLen = _Module.m_server.m_cBaseReceipt->GetReceiptIndex(szTrack2, sizeof(szTrack2), sTrack2, lFlags, lIndex); //4.0.30.0 270322 //call get card info to get the account number after we encrypt the account and this become the key to the QDX
			if (LOG_BASIC_FLOW_CONTROL)
			{
				//4.0.20.501 TD 60236		
				char szResult[MAX_FIELD_VAL] = { 0 };
				_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, szTrack2, iLen);

				CString str;
				str.Format("CGCI::ReceiptPrinted__ the track is %s", szResult);

				_LOGMSG.LogMsg(str);
			}

			if (iLen == 0)
			{
				dwLastErr = GetLastError();
				retCode = GCI_NO_RECEIPT;
			}
			else
			{
				Track2Format((unsigned char *)szTrack2);
				ReceiptInfo.Clear();
				// find the original record link to Car index.
				if (lFlags & RECEIPT_CAR_REGISTRATION_INDEX)  // 4.0.7.505
				{					
					CReceiptInfo    Info;
					CString csVehicleKey(szTrack2, _countof(szTrack2));
					auto & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
					lRtc = PumpSrvDatabase.LoadReceiptByVehicle(csVehicleKey, Info);

					if (lRtc == OK)
					{
						memset(szTrack2, '\x20', sizeof(szTrack2));
						memcpy(szTrack2, Info.m_HashedKey, min(Info.m_HashedKey.GetLength(), sizeof(szTrack2)));
					}

				}
				else
				{
					//?
				}
				
				if (lRtc == OK)
				{
					CString csHashKey(szTrack2, _countof(szTrack2)); 		//RFUEL-3093
// 					long byNumberOfCopies = ReceiptInfo.m_NumberOfCopies;
// 					
// 					str.Format("CGCI::ReceiptPrinted__ copy number %ld", byNumberOfCopies);
// 					_LOGMSG.LogMsg(str);
					
					if (lFlags & RECEIPT_ORIGINAL_PRINTED)
					{
						//PAY_AT_PUMP_INFO	PAPInfo;
						//CPumpTransact		Trs;
						//CInfoData			cInfoData;       //4.0.3.29
															 // 3.2.0.51
						//long byIndex = ReceiptInfo.m_TrsIndex;
						// 3.2.0.51

						//4.0.3.29
						//cInfoData = ReceiptInfo;
						//4.0.3.29

// 						_Module.m_server.ConvertReceiptToOla(Info, &Trs, &PAPInfo);
// 						PAPInfo.CardSaleExtraData2.sReceiptPrinted = '1';
// 						_Module.m_server.m_cBaseReceipt->ConvertOlaToReceipt(&Trs, &PAPInfo, Info, 0);//4.0.9.503

						
						CPumpSrvDatabase::GetInstance().SetReceiptPrinted(csHashKey.Trim(), ReceiptInfo.m_TrsIndex);
						//ReceiptInfo = cInfoData;
						//ReceiptInfo.m_TrsIndex = byIndex;

					}
					// 3.2.0.51
					//ReceiptInfo.m_NumberOfCopies = byNumberOfCopies + 1;
					// 3.2.0.51

					CPumpSrvDatabase::GetInstance().ReceiptIncrementNumberOfCopies(csHashKey.Trim(), (lFlags & RECEIPT_ORIGINAL_PRINTED) ? true : false);

				}
				else
					lRtc = ERR_NOT_FOUND;
			}
		}
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{	//4.0.20.501 TD-60236
		char szResult[MAX_FIELD_VAL] = {0};

		_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, szTrack2, iLen);	

		str.Format("GCI::ReceiptPrinted Data=%s, lIndex=%ld, lFlags=%ld lRtc=%d", szResult, lIndex , lFlags, lRtc);			
		_LOGMSG.LogMsg(str);
	}

	if((lRtc == ERR_NOT_FOUND)||(lRtc == Q_ERROR))
	{		
		retCode = GCI_FAILED_IO;
	}

	if (pVal != NULL)
		*pVal = retCode;
}

// RFUEL-3093
BOOL CGCI::FindTransactionNumberByRowIndex__(long lIndex, char* szTrack2, int *nOutTrnsNum)
{
	CReceiptInfo Info;
	*nOutTrnsNum = -1;
	Track2Format((unsigned char *)szTrack2);

	Info.m_HashedKey.SetString(szTrack2, sizeof(szTrack2));

	long lRtc = CSQLReceiptDB::GetInstance().GetFirstReceiptRecord(Info);
	long nPositionIndex = 0;

	while (lRtc == OK)
	{

		if (nPositionIndex == lIndex)
			break;
		lRtc = CSQLReceiptDB::GetInstance().GetNextReceiptRecord(Info.m_TrsIndex, Info);
		nPositionIndex++;
	}
	if (nPositionIndex != lIndex)
		return FALSE;
	*nOutTrnsNum = Info.m_TrsIndex;
	return TRUE;
}

void CGCI::GetSingleProductInfo__(long lPump, long lFlags, long *lGrade, long *pVal)
{
	GCIRetCode retCode = GCI_OK;

	if (PUMP_NOT_VALID(lPump))
		retCode = GCI_INVALID_PUMP_NUMBER;


	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;
		str.Format("GetSingleProductInfo, lGrade=%ld " , *lGrade); 
		_LOGMSG.LogFlagsMsg(lPump,LOG_PUMP,str,lFlags);
	}


	if(retCode == GCI_OK)
	{ 		
		long lTmpGrade  = 0;
		long lLastGrade  = 0;
		*lGrade = 0;

		for (int i =1; i <= MAX_POSITIONS_PER_PUMP; i++)
		{
			lTmpGrade = _Module.m_server.m_cPumpArray[lPump -1].NozzleToGrade(i);

			if (lTmpGrade)
			{
				if (lLastGrade)
				{
					if (lTmpGrade !=  lLastGrade)
					   retCode =GCI_PARAM_NOT_FOUND;				
				}
				else
				{
					lLastGrade	= lTmpGrade;
					retCode =GCI_OK;
				}
			}			
		}

		if(retCode == GCI_OK)
			*lGrade = lLastGrade;
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;
		str.Format("GetSingleProductInfo, lGrade=%ld, Pval=%ld ", *lGrade ,*pVal); 
		_LOGMSG.LogFlagsMsg(lPump,LOG_PUMP,str,lFlags);
	}

	if(pVal != NULL)		
		*pVal = retCode;
}

//4.0.18.501
void CGCI::GetReceiptRawData2__(long lPumpNumber, BSTR sTrack2, short *nIndex, BSTR *sReceiptRawData, long lFlags, long *pVal)
{
	PAY_AT_PUMP_INFO	PayAtPumpInfo;
	CPumpTransact		trs;
	TAG_OLA_REC5		tmpTagOlaRec;	
	TAG_OLA_REC5_EX		tagOlaRec;	
	GCIRetCode			retCode = GCI_OK;
	DWORD				dwLastErr;
	long				lRtc = 0; 
	long				lOlaRecSize = (lFlags & SWIPE_THREE_OLA_REC)?sizeof(TAG_OLA_REC5):sizeof(TAG_OLA_REC3); //4.0.5.0
	short				nPosIndex = 0; 
	short				nTmpIndex = 0;
	DWORD				dwMaxReceiptCopies;

	char				sDecryptedAccount[TRACK_2_DATA_SIZE];
	char				szAccount[TRACK_2_DATA_SIZE];
	BOOL				bWrongRec = FALSE;

	char szTrack2[MAX_TABLE_NAME]; 
	memset(szTrack2,0,sizeof(szTrack2));

	memset(sDecryptedAccount,' ',sizeof(sDecryptedAccount));
	memset(szAccount,' ',sizeof(szAccount));

	CString				str;//4.0.9.503

	CReceiptInfo	Info;
	CReceiptInfo	TmpInfo;  //3.2.0.69

	int iLen = WideCharToMultiByte(CP_ACP,0,sTrack2,-1,szTrack2,sizeof(szTrack2),NULL,NULL);

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		//4.0.20.501 TD 60236		
		char szResult[MAX_FIELD_VAL] = {0};
					
		_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, szTrack2, iLen);
		
		CString str;
		str.Format("GetReceiptRawData2, szTrack2=%s,  nIndex=%d retCode=%d ",szResult ,*nIndex ,  retCode); 
			
		_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,str,lFlags); 	
	}

	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode =GCI_NO_RECEIPT;
	}
	else
	{
		BOOL bFound = FALSE;
		Info.Clear();
		
		Track2Format((unsigned char *)szTrack2);

		//call get card info to get the account number 
		//after we encrypt the account and this become the key to the QDX 
		//first check if avi flag is on if so then he  become the key

		if(LOG_BASIC_FLOW_CONTROL) //4.0.14.790
		{//4.0.20.501 TD 60236		
			char szResult[MAX_FIELD_VAL] = {0};		
			_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, szTrack2, iLen);
	
			CString str;
			str.Format("Data=[%s]", szResult);

			_LOGMSG.LogMsg("GCI","GetReceiptRawData2",lPumpNumber,LOG_PUMP,str);
		}
		
		_Module.m_server.m_cParam.LoadParam("Receipt", "MaxReceiptCopies",&dwMaxReceiptCopies,0,TRUE);

		lRtc = _Module.m_server.IndexOperation(RECEIPT_FILE, REC_READ_FIRST, &Info);

		lRtc = CSQLReceiptDB::GetInstance().GetFirstReceiptRecord(Info);

		for (nPosIndex = 0; (lRtc == OK); nPosIndex++)
		{
			if (LOG_FCC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Pump %ld, receipt = %.37s", lPumpNumber, (LPCSTR)Info.m_HashedKey);
				_Module.m_server.m_cLogger.LogMsg(str);
			}


			if ((lRtc == OK) && !memcmp((LPCSTR)Info.m_HashedKey, szTrack2, TRACK_2_DATA_SIZE) &&
				((long)dwMaxReceiptCopies > Info.m_NumberOfCopies))
			{
				memset(&PayAtPumpInfo, 0, sizeof(PAY_AT_PUMP_INFO));
				//memset(&tagOlaRec,' ',sizeof(TAG_OLA_REC3));
				_Module.m_server.ConvertReceiptToOla(Info, &trs, &PayAtPumpInfo);

				//4.0.13.350 //4.0.14.660


				if ((PayAtPumpInfo.CardSaleExtraData2.sReceiptPrinted != '1') ||
					(RECEIPT_GET_LATEST_RECEIPT & lFlags))
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

			lRtc = CSQLReceiptDB::GetInstance().GetNextReceiptRecord(Info.m_TrsIndex, Info);
		}

		// if receipt found , it's mine that we allowed to print.(in ReceiptPrinted we delete old one)
		if (bFound)
		{			
			long  lItemIndex = 0;
			long lRtc = 0;

			memset(&tagOlaRec,' ',sizeof(TAG_OLA_REC5));
			memset(&PayAtPumpInfo,0,sizeof(PAY_AT_PUMP_INFO));

			_Module.m_server.ConvertReceiptToOla(TmpInfo, &trs, &PayAtPumpInfo);

			l2a(PayAtPumpInfo.CardSaleInfo.lIndexNumber,tagOlaRec.lIndexNumber, sizeof(tagOlaRec.lIndexNumber));
			tagOlaRec.cardData	= PayAtPumpInfo.CardSaleInfo.cardData;
			tagOlaRec.extraData = PayAtPumpInfo.CardSaleInfo.extraData;
			tagOlaRec.extraData2 = PayAtPumpInfo.CardSaleExtraData2;
			trs.GetTag(&tagOlaRec.tagTrs,2);

			if (lFlags & SWIPE_THREE_OLA_REC)
				 tagOlaRec.extraData3 = PayAtPumpInfo.CardSaleExtraData3;

			if (_Module.m_server.m_bUseXmlBuffer)
			{
				char sXmlBuff[MAX_XML_BUF_LEN];
				memset(&sXmlBuff,0,sizeof(sXmlBuff));
				_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml(sXmlBuff,&tagOlaRec);
				CComBSTR strResult(sXmlBuff);
				*sReceiptRawData = strResult.Copy();                
			}
			else
			{
				//TD 38654, 40492
				_Module.m_server.m_cCommonFunction.ConvertFromTagOlaRec5ExToTagOlaRec5(&tagOlaRec, &tmpTagOlaRec);
				BYTE *sTmpAllocationBuffer = new BYTE[lOlaRecSize+2];
				Str2StrNull((BYTE *)&tmpTagOlaRec,lOlaRecSize,sTmpAllocationBuffer);

				CComBSTR strResult(lOlaRecSize + 2,(LPCSTR)sTmpAllocationBuffer); 
				delete []sTmpAllocationBuffer; //4.0.5.0
				
				*sReceiptRawData = strResult.Copy();
	  			*nIndex = nTmpIndex;
			}

		}else
			retCode = GCI_NO_RECEIPT;

	}

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;				
		str.Format("GetReceiptRawData2(PumpNumber = %ld, Flags %ld, nIndex %d) " ,lPumpNumber,lFlags,*nIndex); 
		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}

	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGCI::DebitProcessResultCmd(long lPumpNumber, long lSessionId, long lFlags, BSTR sData, long *pVal)
{

		// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		DebitProcessResultCmd__(lPumpNumber, lSessionId, lFlags, sData, pVal);

#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::DebitProcessResultCmd")) //4.0.15.450
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::DebitProcessResultCmd()", LOG_LEVEL_0);

		exit(1);	
	}
#endif

	return S_OK;
}

void CGCI::DebitProcessResultCmd__(long lPumpNumber, long lSessionId, long lFlags, BSTR sData, long *pVal)
{
	// TODO: Add your implementation code here
	OLA_DEBIT_PROCESS_INFO  cOlaDebitProcessInfo;
	OLA_STAT				Ola;
	DWORD					dwLastErr;
	GCIRetCode				retCode = GCI_OK;

	if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;


	if(retCode == GCI_OK) 
	{
		memset(&cOlaDebitProcessInfo,' ',sizeof(OLA_DEBIT_PROCESS_INFO));

		int iLen = Bstr2Buffer(sData, (void *)&cOlaDebitProcessInfo,sizeof(OLA_DEBIT_PROCESS_INFO));
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;
		}
		else
		{
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			long  p = lPumpNumber-1;
			_Module.m_server.m_cDebitInfoArray[p].GetOlaStat(&Ola);

			Ola.m_byState = OLA_NEW_STATE(Ola.m_byState,OLA_REQUEST_TO_SEND_MORE);									

			if(LOG_BASIC_FLOW_CONTROL)
				_LOGMSG.LogOLAMsg(lPumpNumber,Ola.m_byState,Ola.m_lSessionNumber);
			_Module.m_server.m_cDebitInfoArray[lPumpNumber- 1].SetDebitInfo(&cOlaDebitProcessInfo);
			_Module.m_server.m_cDebitInfoArray[p].SetOlaStat(&Ola);  

			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}
	}

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;				
		str.Format("DebitProcessResultCmd, OlaSession=%ld  retCode=%d ",Ola.m_lSessionNumber ,  retCode); 
		_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,str,lFlags); 				
	}

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::SetTrsState(long lPumpNumber, long lTrsNumber, long lState, long lFlags, BSTR sData, long *plRetVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif

		SetTrsState__(lPumpNumber , lTrsNumber , lState , lFlags , sData , plRetVal);


#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::SetTrsState")) //4.0.15.450
	{
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","SetTrsState()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);	
	}
#endif

	return S_OK;
}

STDMETHODIMP CGCI::GetPumpInfo(long lPumpNumber, BSTR *pRec, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here

#ifdef CATCH_GCI
	__try
	{
#endif

		GetPumpInfo__(lPumpNumber, pRec, lFlags , pVal);

#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetPumpInfo")) //4.0.15.450
	{
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","GetPumpInfo()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);	
	}
#endif

	return S_OK;
}
// 3.2.0.76]

void CGCI::SetTrsState__(long lPumpNumber, long lTrsNumber, long lState, long lFlags, BSTR sData, long *plRetVal)
{
	GCIRetCode retCode = GCI_OK;
	
	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if(_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;
	else if(FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GCI_FAILED_IO;

	if(retCode == GCI_OK)
	{				
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		// Sale at BAP.
		if (lFlags & TRS_PAID_AT_POS)
		{				
		_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,"Before , UpdateAttendantSale",lFlags);
			retCode = _Module.m_server.UpdateAttendantSale(lPumpNumber, lTrsNumber, lState, lFlags, sData, plRetVal);

			_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,"after , UpdateAttendantSale",lFlags);
			
			if ((retCode == GCI_OK) && (lFlags & TRS_IGNOR_OLA))
			{
				retCode = _Module.m_server.SetAttendantTrs(lPumpNumber, lTrsNumber, lState, lFlags, sData, plRetVal);
			}
		}
		else					
			retCode = _Module.m_server.SetAttendantTrs(lPumpNumber, lTrsNumber, lState, lFlags, sData, plRetVal);							

		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;				
		str.Format("SetTrsState(PumpNumber = %ld, trans no. %ld, lState %d, flags %ld) " ,lPumpNumber, lTrsNumber , lState ,lFlags); 
		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}

	if(plRetVal)
		*plRetVal = retCode;

	return;
}
// 3.2.0.76

void CGCI::GetPumpInfo__(long lPumpNumber, BSTR *pRec, long lFlags, long *pVal)
{
	GCIRetCode retCode = GCI_OK;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	else if(_Module.m_server.IsServerInRecoveryProcess() && 
		    _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess &&
			!(lFlags & IGNORE_SERVER_IN_MIDDLE_RECOVERY)) //4.0.19.504 - TD 59460
		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	if (lFlags & STRUCT_IS_TAG_PUMP3)
	{
		//TD 38654
		TAG_PUMP3 tmpPump;
		memset(&tmpPump,' ', sizeof(TAG_PUMP3)); 
		
		TAG_PUMP3_EX pump;
		memset(&pump,' ', sizeof(TAG_PUMP3_EX)); 
		
		if(retCode == GCI_OK)
		{
			long p = lPumpNumber-1;

			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			_Module.m_server.m_cPumpArray[p].GetTag(&pump,3);

			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}
		else
		{
			if(LOG_LOGIC_ERROR)
			{
				CString sMsg;
				sMsg.Format("GetPumpInfo() - return value is %d" , retCode);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);					
			}		
		}

		BYTE sTmp[sizeof(TAG_PUMP3)+2];
		//TD 38654
		_Module.m_server.m_cCommonFunction.ConvertFromTagPump3ExToTagPump3(&pump, &tmpPump);

		Str2StrNull((BYTE *)&tmpPump,sizeof(TAG_PUMP3),sTmp);
		CComBSTR strResult(sizeof(sTmp),(LPCSTR)sTmp); 
		*pRec = strResult.Copy();
	}
	else if (lFlags & STRUCT_IS_TAG_PUMP4)
	{
		//TD 38654
		TAG_PUMP4 tmpPump;
		memset(&tmpPump,' ', sizeof(TAG_PUMP4)); 
		TAG_PUMP4_EX pump;
		memset(&pump,' ', sizeof(TAG_PUMP4_EX));
		
		if(retCode == GCI_OK)
		{
			long p = lPumpNumber-1;

			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			_Module.m_server.m_cPumpArray[p].GetTag(&pump,4);

			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}
		else
		{
			if(LOG_LOGIC_ERROR)
			{
				CString sMsg;
				sMsg.Format("GetPumpInfo");
				_LOGMSG.LogMsgWithRet(lPumpNumber,LOG_PUMP,sMsg,"IGCI",retCode);
			}
		}

		BYTE sTmp[sizeof(TAG_PUMP4)+2];
		//TD 38654
		_Module.m_server.m_cCommonFunction.ConvertFromTagPump4ExToTagPump4(&pump, &tmpPump);

		Str2StrNull((BYTE *)&tmpPump,sizeof(TAG_PUMP4),sTmp);
		CComBSTR strResult(sizeof(sTmp),(LPCSTR)sTmp); 
		*pRec = strResult.Copy();
	}
	
	if(pVal != NULL)
		*pVal = retCode;
}


/******************************************************************************
 Description:	Get Debit info 	
 Returns:      	pData - Debit buffer.
 Parameters:   	long lPumpNumber, long lFlags, BSTR *pData, long *pVal.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			21/5/2002   17:05		Start
******************************************************************************/
STDMETHODIMP CGCI::GetDebitDataInfo(long lPumpNumber, long lFlags, BSTR *pData, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		GetDebitDataInfo__(lPumpNumber, lFlags, pData, pVal);

#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetDebitDataInfo")) //4.0.15.450
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetDebitDataInfo()", LOG_LEVEL_0);

		exit(1);	
	}
#endif

	return S_OK;
}


/******************************************************************************
 Description:	Get Debit info 	
 Returns:      	pData - Debit buffer.
 Parameters:   	long lPumpNumber, long lFlags, BSTR *pData, long *pVal.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			21/5/2002   17:05		Start
******************************************************************************/
void CGCI::GetDebitDataInfo__(long lPumpNumber, long lFlags, BSTR *pData, long *pVal)
{
	GCIRetCode retCode = GCI_OK;
	OLA_DEBIT_PROCESS_INFO Info;
	memset(&Info,' ', sizeof(OLA_DEBIT_PROCESS_INFO)); 

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	if(retCode == GCI_OK)
	{
		long p = lPumpNumber-1;	
		OLA_STAT	Ola;
	
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		_Module.m_server.m_cDebitInfoArray[p].GetOlaStat(&Ola);
		
		if ((SESSION_TYPE(Ola.m_byState) == SESSION_SHIFT_DEBIT) || 
			(SESSION_TYPE(Ola.m_byState) == SESSION_INIT_DEBIT)) 
			
		{
			_Module.m_server.m_cDebitInfoArray[p].GetDebitInfo(&Info);
		}
		else
		{
			retCode = GCI_TRS_NOT_FOUND;
		}

		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}

	BYTE sTmp[sizeof(OLA_DEBIT_PROCESS_INFO)+2];
	Str2StrNull((BYTE *)&Info,sizeof(OLA_DEBIT_PROCESS_INFO),sTmp);
	CComBSTR strResult(sizeof(sTmp),(LPCSTR)sTmp); 
	*pData = strResult.Copy();
	
	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::GetReceiptRawData3(long lPumpNumber, BSTR sTrack2, short *nIndex, BSTR *sReceiptRawData, long *plFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif		
		GetReceiptRawData3__(lPumpNumber, sTrack2, nIndex, sReceiptRawData, plFlags, pVal);
#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetReceiptRawData3")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","GetReceiptRawData3()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif
	return S_OK;
}

void CGCI::GetReceiptRawData3__(long lPumpNumber, BSTR sTrack2, short *nIndex, BSTR *sReceiptRawData, long *plFlags, long *pVal)
{
	PAY_AT_PUMP_INFO	PayAtPumpInfo;
	CPumpTransact		trs;
	TAG_OLA_REC9		tmpTagOlaRec;	//4.0.5.37
	TAG_OLA_REC13		tagOlaRec;	    //4.0.27.47 
	GCIRetCode			retCode = GCI_OK;
	DWORD				dwLastErr;
	long				lRtc = 0; 
	short				nPositionIndex = 0; 
	long				nTmpIndex = 0;
	DWORD				dwMaxReceiptCopies;	
	int					iLen=0;
	CString				strMsg;	
	CReceiptInfo		Info;
	CReceiptInfo		TmpInfo;

	char				szTrack2[ENCRYPTED_DATA_SIZE];
	memset(szTrack2,0,sizeof(szTrack2));

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		strMsg.Format("GetReceiptRawData3, Pump Number = %ld ,Flags = %ld",lPumpNumber,*plFlags); 
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
	}

	if (RECEIPT_GET_LAST_MEMO_TRACK2 & *plFlags)
	{				
		if ((lPumpNumber > _Module.m_server.GetMaxPumps()) || ( lPumpNumber < 1))   //4.0.0.79
			retCode = GCI_INVALID_PUMP_NUMBER;
		else
		{
			CMemPumpInfo cMemPumpInfo;
			iLen = 1;
			
			if (_Module.m_server.m_cMemPumpArray[lPumpNumber - 1].GetMemPumpInfo(&cMemPumpInfo,0))
			{
				if (cMemPumpInfo.cCardData.sTranType == CD_TYPE_AVI)
				{
					memcpy(szTrack2 ,cMemPumpInfo.cCardData.sFullVehicleId, min(sizeof(szTrack2), sizeof(cMemPumpInfo.cCardData.sFullVehicleId)));
				}
				else 
				{
					//reprint last receipt with PCI active
					//4.0.31.25
					PAY_AT_PUMP_INFO	cPAPInfo;
					char				sReceiptIndex[ENCRYPTED_DATA_SIZE];
					
					memset(&cPAPInfo, ' ', sizeof(cPAPInfo));
					memset(sReceiptIndex, ' ', sizeof(sReceiptIndex));
					
					memcpy(&cPAPInfo.CardSaleInfo.cardData,		&cMemPumpInfo.cCardData,		min(sizeof(cPAPInfo.CardSaleInfo.cardData),		sizeof(cMemPumpInfo.cCardData)));
					memcpy(&cPAPInfo.CardSaleInfo.extraData,	&cMemPumpInfo.cCardSaleExtra,	min(sizeof(cPAPInfo.CardSaleInfo.extraData),	sizeof(cMemPumpInfo.cCardSaleExtra)));
					memcpy(&cPAPInfo.CardSaleExtraData2,		&cMemPumpInfo.cCardSaleExtra2,  min(sizeof(cPAPInfo.CardSaleExtraData2),		sizeof(cMemPumpInfo.cCardSaleExtra2)));
					 
					iLen = _Module.m_server.m_cBaseReceipt->GetReceiptIndex(sReceiptIndex, &cPAPInfo, lPumpNumber);
					
					memcpy(szTrack2, sReceiptIndex, ENCRYPTED_DATA_SIZE);  
										
				}

				if(LOG_DETAIL_FLOW_CONTROL)   
				{//4.0.20.501 TD 60236		
					char szResult[MAX_FIELD_VAL] = {0};	//***	
					_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, (char*)cMemPumpInfo.cCardData.sTrack2Data, sizeof(cMemPumpInfo.cCardData.sTrack2Data));
	
					strMsg.Format("GetReceiptRawData3, Pump Number = %ld, Track2=[%s]",lPumpNumber, szResult);

					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
				}
			}
			else
				retCode = GCI_NO_RECEIPT;							
		}	

		 strMsg.Format("GetReceiptRawData3, Get last memory receipt, retCode=%d", retCode); 
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg); 				
	}
	else
	{		
		iLen = _Module.m_server.m_cBaseReceipt->GetReceiptIndex(szTrack2, sizeof(szTrack2), sTrack2, *plFlags, lPumpNumber);	//4.0.20.501 TD-60236

		strMsg.Format("GetReceiptRawData3, Call GetCardInfo (Receipt Index Len=%ld)", iLen); //4.0.19.500 58863
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);

		if(LOG_DETAIL_FLOW_CONTROL)   
		{//4.0.20.501 TD 60236		
			char szResult[MAX_FIELD_VAL] = {0};		
			_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, szTrack2, iLen);
	
			strMsg.Format("GetReceiptRawData3, Pump Number regular  = %ld ,Track2=[%.100s]",lPumpNumber,szResult); 
			
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
		}
		
		//call get card info to get the account number 
		//after we encrypt the account and this become the key to the QDX 
		//first check if avi flag is on if so then he  become the key
	
		Track2Format((unsigned char *)szTrack2);

		if(LOG_DETAIL_FLOW_CONTROL)
		{				
			if (iLen)
			{//4.0.20.501 TD-60236
				if (RECEIPT_CAR_REGISTRATION_INDEX  & *plFlags)  // 4.0.7.500
				{
					strMsg.Format("CGCI: GetReceiptRawData3  lPumpNumber=%ld  Car registration=%s,  nIndex=%d retCode=%d ", lPumpNumber  , szTrack2,*nIndex ,  retCode); 	
				}
				else
				{
					char	szResult[MAX_FIELD_VAL] = {0};
					
					_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, szTrack2, iLen);	
					
					strMsg.Format("GetReceiptRawData3  szTrack2=[%s],  nIndex=%d retCode=%d ",szResult ,*nIndex ,  retCode); 					
				}

				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg); 				
			}
		}
	}		

	if(iLen == 0) 
	{
		dwLastErr = GetLastError();
		retCode   = GCI_NO_RECEIPT;
	}
	else
	{
		BOOL bFound = FALSE;
		Info.Clear();

		_Module.m_server.m_cParam.LoadParam("Receipt", "MaxReceiptCopies",&dwMaxReceiptCopies,0,TRUE);  //4.0.7.504

		if (RECEIPT_CAR_REGISTRATION_INDEX  & *plFlags)  // 4.0.7.500
		{
			RECEIPT_EXTRA_INDEX_INFO rec;
			memset(&rec , ' ',sizeof(RECEIPT_EXTRA_INDEX_INFO)); //4.0.21.20
			memset(rec.sKeyIndex, ' ', sizeof(rec.sKeyIndex));
			memcpy(rec.sKeyIndex ,szTrack2 , min(strlen(szTrack2), sizeof(rec.sKeyIndex)));
			
			lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_INDEX_FILE, REC_READ, &rec);
			if (!lRtc)
			{//4.0.21.20
				if(LOG_DETAIL_FLOW_CONTROL)
				{					
					strMsg.Format("GetReceiptRawData3  reading extra index file succeeded szTrack2=[%s], rec.index.track2 = [%s], rec.index.index = %s, retCode=%ld ",szTrack2  ,rec.myIndex.sTrack2, &rec.myIndex.byIndex, lRtc); 					
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg); 				
				}


				TmpInfo.m_TrsIndex = rec.myIndex.byIndex;
				TmpInfo.m_HashedKey.SetString((LPCSTR)rec.myIndex.sTrack2, _countof(rec.myIndex.sTrack2));

				if (OK == CSQLReceiptDB::GetInstance().LoadReceipt(TmpInfo.m_TrsIndex, TmpInfo))
				{
					if ((long)dwMaxReceiptCopies > TmpInfo.m_NumberOfCopies)  //4.0.7.504
					{
						nTmpIndex = TmpInfo.m_TrsIndex;
						bFound = TRUE;
						if (LOG_DETAIL_FLOW_CONTROL)
						{
							strMsg.Format("GetReceiptRawData3  reading receipt file succeeded szTrack2=[%s], retCode=%ld", szTrack2, lRtc);
							_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
						}
					}
					else
					{
						if (LOG_DETAIL_FLOW_CONTROL)
						{
							strMsg.Format("GetReceiptRawData3 max number of copies has been reached");
							_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
						}
					}
				}
				else
				{
					if(LOG_DETAIL_FLOW_CONTROL)
					{					
						strMsg.Format("GetReceiptRawData3 reading receipt file failed szTrack2=[%s], retCode=%ld ",szTrack2  ,  lRtc); 					
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg,LOG_LEVEL_1); 				
					}
				}
			}
		}
		else
		{			
			Track2Format((unsigned char *)szTrack2);
			
			Info.m_HashedKey.SetString(szTrack2, sizeof(szTrack2)); //RFUEL-172 
			lRtc = CSQLReceiptDB::GetInstance().GetFirstReceiptRecord(Info);
			int lCmpRes = 0;
			for (nPositionIndex = 0; (lRtc == OK); nPositionIndex++)
			{
				CStringA hashKey(szTrack2, sizeof szTrack2);
				if ((lRtc == OK) && (Info.m_HashedKey.Trim() == hashKey.Trim()))// RFUEL-172 
				{
					// RFUEL-3093 changes begin
					nTmpIndex = TmpInfo.m_TrsIndex;
					bFound = true;

					strMsg.Format("GetReceiptRawData3  reading receipt file szTrack2=[%s], retCode=%ld", szTrack2, lRtc);
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);

					memset(&PayAtPumpInfo, 0, sizeof(PAY_AT_PUMP_INFO));
					//memset(&tagOlaRec,' ',sizeof(TAG_OLA_REC3)); //4.0.5.0.
					_Module.m_server.ConvertReceiptToOla(Info, &trs, &PayAtPumpInfo);

					memset(&PayAtPumpInfo, 0, sizeof(PAY_AT_PUMP_INFO));
					//memset(&tagOlaRec,' ',sizeof(TAG_OLA_REC3)); //4.0.5.0.
					_Module.m_server.ConvertReceiptToOla(Info, &trs, &PayAtPumpInfo);


					if ((PayAtPumpInfo.CardSaleExtraData2.sReceiptPrinted != '1') || (RECEIPT_GET_LATEST_RECEIPT & *plFlags))
					{
						if (lPumpNumber)
						{
							if (trs.m_lPump == lPumpNumber)
							{
								nTmpIndex = nPositionIndex;
								TmpInfo = Info;
								bFound = TRUE;
							}
						}
						else
						{
							nTmpIndex = nPositionIndex;
							TmpInfo = Info;
							bFound = TRUE;

						}
					}
				}

				lRtc = CSQLReceiptDB::GetInstance().GetNextReceiptRecord(Info.m_TrsIndex, Info);
			}
		}

		// RFUEL-3093
		if (bFound)
		{
			if ((long)dwMaxReceiptCopies > TmpInfo.m_NumberOfCopies)
			{
				strMsg.Format("GetReceiptRawData3  receipt was found szTrack2=[%s]", szTrack2);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
			}
			else
			{
				CStringA sMsg;
				sMsg.Format("GetReceiptRawData3 trs %d - max number of copies has been reached!", Info.m_TrsIndex);
				_Module.m_server.m_cLogger.LogMsg(sMsg, LOG_LEVEL_0);
				bFound = FALSE;
			}
		}

		// if receipt found , it's mine that we allowed to print.(in ReceiptPrinted we delete old one)
		if (bFound)
		{			
			PS::RECEIPT_EXTRA_INFO_REC	cReceiptExtraRec;
			ITEMS_INFO	cItemInfo;	
			long  lItemIndex = 0;
			long  lRtc;			
			
			memset(&tmpTagOlaRec,' ',sizeof(tmpTagOlaRec));
			memset(&tagOlaRec,' ',sizeof(tagOlaRec));
			memset(&PayAtPumpInfo,' ' ,sizeof(PAY_AT_PUMP_INFO)); 

			//_Module.m_server.ConvertReceiptToOla(&TmpInfo,&trs,  &PayAtPumpInfo);
			_Module.m_server.ConvertReceiptToOla(TmpInfo, &trs, &PayAtPumpInfo);
			
			BOOL  bReceiptExtra =  _Module.m_server.GetOLAFlags(PayAtPumpInfo.CardSaleExtraData2.sFlags , FL_EXTRA_RECEIPT_REC);

			l2a(PayAtPumpInfo.CardSaleInfo.lIndexNumber,tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber));
			tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData	 = PayAtPumpInfo.CardSaleInfo.cardData;
			tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData  = PayAtPumpInfo.CardSaleInfo.extraData;
			tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2 = PayAtPumpInfo.CardSaleExtraData2;
			trs.GetTag(&tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs,2);
			tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData3 = PayAtPumpInfo.CardSaleExtraData3; //4.0.5.25   //4.0.5.213
			tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData4 = PayAtPumpInfo.CardSaleExtraData4; //4.0.22.510 merge from 4.0.22.70 TD 74387
			tagOlaRec.tagOlaRec12.extraData5                         = PayAtPumpInfo.CardSaleExtraData5; //4.0.27.47
			tagOlaRec.extraData6 = PayAtPumpInfo.CardSaleExtraData6;

			if (tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sTranType ==  CD_TYPE_AVI) //4.0.14.508
			{				
				memcpy(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sTrack2Data ,tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sFullVehicleId ,
					min(sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sTrack2Data),sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sFullVehicleId)));
			}

/////////////////////////////////////////////////////////////////////////
			if (bReceiptExtra)
			{
				memset(&cReceiptExtraRec, 0, sizeof(PS::RECEIPT_EXTRA_INFO_REC));
				cReceiptExtraRec.info.lIndexNumber = a2l(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sTranNbr, sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData.sTranNbr));

				lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, cReceiptExtraRec);
				//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cReceiptExtraRec);

				// Fail to read receipt exatra data
				if (lRtc)
				{
					memset(cReceiptExtraRec.info.sFiler2043, ' ', sizeof(cReceiptExtraRec.info.sFiler2043));
				}

				if(LOG_DETAIL_FLOW_CONTROL)
				{					
					strMsg.Format("GetReceiptRawData3, Trs=%ld,lRtc=%ld Receipt information was send to controller receipt extra information" , trs.m_lNumber , lRtc);
					_LOGMSG.LogMsg(trs.m_lPump,LOG_PUMP,strMsg);	
				}
			} 

			lItemIndex = a2l(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sItemsLinkIndex,sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sItemsLinkIndex));

			if (lItemIndex)
			{							
				TAG_OLA_REC12	cTagOlaRec12;			  //4.0.22.510 merge from 4.0.22.70 TD 74387
				long  lOlaRecSize = sizeof(TAG_OLA_REC6); //4.0.5.0
				BOOL  bExtarItems=  _Module.m_server.GetOLAFlags(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sFlags , FL_EXTRA_ITEM_REC);

				*plFlags = RECEIPT_FORMAT_WITH_ITEMS;				
								
				memset(&cItemInfo, ' ' , sizeof(cItemInfo));
				cItemInfo.ItemRec.lIndexNumber = lItemIndex;
				cItemInfo.ItemRec2.lIndexNumber = lItemIndex + 1;

				memset(&cTagOlaRec12,  ' ' , sizeof(cTagOlaRec12));		//4.0.22.510 merge from 4.0.22.70 TD 74387
				cTagOlaRec12.tagOlaRec11.tagOlaRec10.cardData   = tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData;
				cTagOlaRec12.tagOlaRec11.tagOlaRec10.extraData  = tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData;
				cTagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2 = tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2;
				cTagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs     = tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs;
				
				if (_Module.m_server.GetOLAFlags(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sFlags , FL_PAP_AS_THIRD_REC))
				{					
					*plFlags = RECEIPT_FORMAT_WITH_ITEMS + SWIPE_THREE_OLA_REC ;
					cTagOlaRec12.tagOlaRec11.tagOlaRec10.extraData3 = tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData3;   //4.0.22.510 merge from 4.0.22.70 TD 74387
					cTagOlaRec12.tagOlaRec11.tagOlaRec10.extraData4 = tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData4;
					cTagOlaRec12.extraData5                         = tagOlaRec.tagOlaRec12.extraData5;							  //4.0.27.47

					if (bExtarItems)
					{
						lOlaRecSize = sizeof(TAG_OLA_REC7); //4.0.5.0
						*plFlags +=RECEIPT_WITH_EXTRA_ITEM;  //4.0.5.27

						if (bReceiptExtra)
						{							
							lOlaRecSize = sizeof(TAG_OLA_REC9); //4.0.5.39
							*plFlags +=RECEIPT_WITH_RECEIPT_EXTRA_DATA;  //4.0.5.39																			
							cTagOlaRec12.tagOlaRec11.tagOlaRec10.ReceiptExtraInfo = cReceiptExtraRec.info; //4.0.21.0 68079 //4.0.22.510 merge from 4.0.22.70 TD 74387
						} 						
					}
				}
				else
					lOlaRecSize= sizeof(TAG_OLA_REC4);
				
				lRtc = _Module.m_server.IndexOperation(ITEMS_FILE,REC_READ,&cItemInfo);

				if (lRtc)
				{							
					memset(&cTagOlaRec12.tagOlaRec11.tagOlaRec10.Items ,' ', sizeof(cTagOlaRec12.tagOlaRec11.tagOlaRec10.Items));   //4.0.22.510 merge from 4.0.22.70 TD 74387
					memset(&cTagOlaRec12.tagOlaRec11.tagOlaRec10.Items2 ,' ', sizeof(cTagOlaRec12.tagOlaRec11.tagOlaRec10.Items2)); //4.0.22.70 TD 74387
					
					if(LOG_BASIC_FLOW_CONTROL)
					{							
						strMsg.Format("Fail !!!! read Index=%ld from file Items" , cItemInfo.ItemRec.lIndexNumber);
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
					}
				}
				else
				{
					CItemsFile cItemsFile;
					BAP_ALL_DATA_STRUCTS  cItemsRec;

					cItemsFile.ConstructItemsRecordToBAP(cItemInfo ,cItemsRec);
					cTagOlaRec12.tagOlaRec11.tagOlaRec10.Items  =  cItemsRec.cBAPReceiptItem;    //4.0.22.510 merge from 4.0.22.70 TD 74387
					cTagOlaRec12.tagOlaRec11.tagOlaRec10.Items2 =  cItemsRec.cBAPReceiptItem2; //4.0.22.70 TD 74387				
					if(LOG_BASIC_FLOW_CONTROL)
					{							
						strMsg.Format("read Index=%ld from file Items succeeded" , cItemInfo.ItemRec.lIndexNumber);
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
					}
				}																															

				if(LOG_BASIC_FLOW_CONTROL)
				{//4.0.21.0 68079
					long lIndexNumber = 0;
					lIndexNumber = a2l(cTagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber,sizeof(cTagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber)); //4.0.22.510 merge from 4.0.22.70 TD 74387
					strMsg.Format("GetReceiptRawData3 Process command CMD_FORMAT_RECEIPT, Read ItemIndex=%ld, lFlags=%ld, Invoice=%.10s ",lIndexNumber, *plFlags  , cTagOlaRec12.tagOlaRec11.tagOlaRec10.Items.sInvoiceNumber);
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
				}	
								
				if (_Module.m_server.m_bUseXmlBuffer)
				{
					char sXmlBuff[MAX_XML_BUF_LEN];
					memset(&sXmlBuff,0,sizeof(sXmlBuff));

					//Convert to struct
					_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml(sXmlBuff,&cTagOlaRec12); //4.0.22.510 merge from 4.0.22.70 TD 74387
					
					CComBSTR strResult(sXmlBuff);
					*sReceiptRawData = strResult.Copy();                
				}
				else
				{
					_Module.m_server.m_cCommonFunction.ConvertFromTagOlaRec12ToTagOlaRec9(&cTagOlaRec12, &tmpTagOlaRec); //4.0.22.510 merge from 4.0.22.70 TD 74387
					BYTE *sTmpAllocationBuffer = new BYTE[lOlaRecSize + 2 ]; //4.0.5.0
					
					Str2StrNull((BYTE *)&tmpTagOlaRec,lOlaRecSize,sTmpAllocationBuffer);
					CComBSTR strResult(lOlaRecSize + 2,(LPCSTR)sTmpAllocationBuffer);
									
					delete []sTmpAllocationBuffer;
					
					*sReceiptRawData = strResult.Copy();                
				}
				*nIndex = (short) nTmpIndex;
			}
			else
			{
				long  lOlaRecSize = sizeof(TAG_OLA_REC3); //4.0.5.0

				if (_Module.m_server.GetOLAFlags(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sFlags , FL_PAP_AS_THIRD_REC))
				{
					*plFlags = SWIPE_THREE_OLA_REC;  //4.0.5.25
					lOlaRecSize= sizeof(TAG_OLA_REC5_EX);

					if (bReceiptExtra)
					{
						*plFlags +=RECEIPT_WITH_RECEIPT_EXTRA_DATA;  //4.0.5.39													
						lOlaRecSize= sizeof(TAG_OLA_REC9);//4.0.15.91 - change from REC8 to REC9
						
						tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.ReceiptExtraInfo = cReceiptExtraRec.info;
					}
				}

				if (_Module.m_server.m_bUseXmlBuffer)
				{
					char sXmlBuff[MAX_XML_BUF_LEN];
					memset(&sXmlBuff,0,sizeof(sXmlBuff));

					//Convert to struct
					_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml(sXmlBuff,&tagOlaRec);
					
					CComBSTR strResult(sXmlBuff);
					*sReceiptRawData = strResult.Copy();                
				}
				else
				{
					BYTE *sTmpAllocationBuffer = new BYTE[lOlaRecSize+2];
					
					_Module.m_server.m_cCommonFunction.ConvertFromTagOlaRec12ToTagOlaRec9(&tagOlaRec.tagOlaRec12, &tmpTagOlaRec); //4.0.22.510 merge from 4.0.22.70 TD 74387
					Str2StrNull((BYTE *)&tmpTagOlaRec,lOlaRecSize,sTmpAllocationBuffer);
				
					CComBSTR strResult(lOlaRecSize +2,(LPCSTR)sTmpAllocationBuffer); 
					
					delete []sTmpAllocationBuffer; //4.0.5.0
					
					*sReceiptRawData = strResult.Copy();                
				}
				*nIndex = (short) nTmpIndex;
			}
///////////////////////////////////////////////////////////////////////////
			
			if(LOG_DETAIL_FLOW_CONTROL)
			{						
				strMsg.Format("GetReceiptRawData3, return information, SeqNum=%.5s", PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr); 
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg); 				
			}
		}
		else
			retCode = GCI_NO_RECEIPT;
	}

	if(LOG_DETAIL_FLOW_CONTROL)
	{						
		strMsg.Format("GetReceiptRawData3(PumpNumber = %ld, Flags %ld, nIndex %d) " ,lPumpNumber,*plFlags,*nIndex); 
		_LOGMSG.LogMsgWithRet(0,LOG_NONE,strMsg,"IGCI",retCode);
	}

	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CGCI::SetDebitData(long lPumpNumber, long lFlags, BSTR sData, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		SetDebitData__(lPumpNumber, lFlags, sData, pVal);

#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::SetDebitData")) //4.0.15.450
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetDebitData()", LOG_LEVEL_0);

		exit(1);	
	}
#endif

	return S_OK;
}

void CGCI::SetDebitData__(long lPumpNumber, long lFlags, BSTR sData, long *pVal)
{
	DWORD dwLastErr=0;
	GCIRetCode retCode = GCI_OK;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GCI_FAILED_IO;

	if(retCode == GCI_OK)
	{
		OLA_DEBIT_PROCESS_INFO   cOlaDebitProcessInfo;
		long p = lPumpNumber - 1;

		BYTE byTmpBuffer[sizeof(OLA_DEBIT_PROCESS_INFO)+2];
		memset(byTmpBuffer,' ',sizeof(byTmpBuffer));

		int iLen = Bstr2Buffer(sData, (void *)byTmpBuffer,sizeof(OLA_DEBIT_PROCESS_INFO));
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;
		}
		else
		{			
//			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);    
			memcpy(&cOlaDebitProcessInfo , byTmpBuffer , sizeof(OLA_DEBIT_PROCESS_INFO));
			_Module.m_server.m_cDebitInfoArray[p].SetDebitInfo(&cOlaDebitProcessInfo);	
//			LeaveCriticalSection(&_Module.m_server.m_csPumpArray);
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"SetDebitData",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);
	
	if(pVal != NULL)
		*pVal = retCode;
}

/******************************************************************************
 Description:	Tank reading informaion 	
 Returns:      	Ret code
 Parameters:   	long lTankNumber, long lProduct, short nFlags, long lStatus1, long lFuelLevel, long lWaterLevel, long lTemperature, long lVolume, long lWaterVolume, long lStatus2, long lFuelValue, long lPricePerLitre, long lTcVolume, BSTR sRec, long *pVal
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			12/8/2002   12:50		Start
******************************************************************************/

//4.0.0.78
STDMETHODIMP CGCI::TankReading3(long lTankNumber, long lProduct, short nFlags, long lStatus1, long lFuelLevel, long lWaterLevel, long lTemperature, long lVolume, long lWaterVolume, long lStatus2, long lFuelValue, long lPricePerLitre, long lTcVolume,long lUllage , BSTR sRec, long *pVal)
{
#ifdef CATCH_GCI
	__try
	{
#endif
	TankReading3__(lTankNumber, lProduct, nFlags, lStatus1, lFuelLevel, lWaterLevel, lTemperature, lVolume, lWaterVolume, lStatus2, lFuelValue, lPricePerLitre, lTcVolume,  lUllage , sRec, pVal);
#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::TankReading3")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::TankReading3()", LOG_LEVEL_0);

		exit(1);
	}
#endif
	return S_OK;
}

/******************************************************************************
 Description:	Tank reading informaion 	
 Returns:      	Ret code
 Parameters:   	long lTankNumber, long lProduct, short nFlags, long lStatus1, long lFuelLevel, long lWaterLevel, long lTemperature, long lVolume, long lWaterVolume, long lStatus2, long lFuelValue, long lPricePerLitre, long lTcVolume, BSTR sRec, long *pVal
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			12/8/2002   12:50		Start
******************************************************************************/
void CGCI::TankReading3__(long lTankNumber, long lProduct, short nFlags, long lStatus1, long lFuelLevel, long lWaterLevel, long lTemperature, long lVolume, long lWaterVolume, long lStatus2, long lFuelValue, long lPricePerLitre, long lTcVolume,  long lUllage , BSTR sRec, long *pVal)
{
	TankReading2__(lTankNumber, lProduct, nFlags, lStatus1, lFuelLevel, lWaterLevel, lTemperature, lVolume, lWaterVolume, lStatus2, lFuelValue, lPricePerLitre,  pVal, lTcVolume ,lUllage , sRec);
}
//4.0.0.78

STDMETHODIMP CGCI::SetAlarm(long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus,long lAlarmValue, long lFlags, long *pAlarmId,  BSTR sRec, long *pVal)
{
	#ifdef CATCH_GCI
	__try
	{
#endif
	SetAlarm__(lCatagory,lSubCatagory,lAlarmNumber,lDevice,lAlarmStatus,lAlarmValue,lFlags,pAlarmId,sRec,pVal);
#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::SetAlarm")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetAlarm()", LOG_LEVEL_0);

		exit(1);
	}
#endif
	return S_OK;
}

//4.0.1.40
void CGCI::SetAlarm__(long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus, long lAlarmValue, long lFlags, long *pAlarmId,  BSTR sRec, long *pVal)
{	
	char		 sMessages[MAX_ALARM_DESC_LEN];
	_bstr_t		bstrMessage(sRec);
	GCIRetCode retCode = GCI_OK;
	
	long lRetVal = 0;

	if  (bstrMessage.length())
	{
		memset(sMessages, 0, sizeof(sMessages));
		memcpy(sMessages , (const char *)bstrMessage, min (sizeof(sMessages) , bstrMessage.length()));			//4.0.25.80 TD 142857
		lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(lCatagory, lSubCatagory, lAlarmNumber, lDevice,lAlarmStatus,lAlarmValue, lFlags , sMessages);
	}
	else
	{
		lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(lCatagory, lSubCatagory, lAlarmNumber, lDevice,lAlarmStatus,lAlarmValue, lFlags);
	}

	if(LOG_DETAIL_FLOW_CONTROL)
	{	
		CString sMsg; 				
		sMsg.Format("Convert Layer Raise Alarm, lCatagory=%ld ,lSubCatagory=%ld,lAlarmNumber=%ld,lDevice=%ld,lAlarmStatus=%ld,lAlarmValue=%ld , lRetVal=%ld " ,
									   lCatagory,lSubCatagory, lAlarmNumber,lDevice,lAlarmStatus , lAlarmValue , lRetVal); 
		_LOGMSG.LogMsg(sMsg);
	}				
	
	if (lRetVal)
		retCode = GCI_FAILED_IO;

	if(pVal != NULL)
		*pVal = retCode;			
}
//4.0.1.38
/******************************************************************************
 Description:	Update PumpSrv with Cash Acceptor information
 Returns:      	GCI Retcode
 Parameters:   	long lPumpNumber, long lDevice, long lSeqNum, long lFlags, BSTR CashSaleData, long *pVal
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			21/10/2002   19:23		Start
******************************************************************************/
STDMETHODIMP CGCI::SetCashAcceptorData(long lPumpNumber, long lDevice, long lSeqNum, long lFlags, BSTR CashSaleData, long *pVal)
{
#ifdef CATCH_GCI
	__try
	{
#endif

		SetCashAcceptorData__(lPumpNumber,lDevice, lSeqNum,lFlags, CashSaleData, pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::SetCashAcceptorData")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","SetCashAcceptorData",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif
	return S_OK;
}
/******************************************************************************
 Description:	Update PumpSrv with Cash Acceptor information
 Returns:      	GCI Retcode
 Parameters:   	long lPumpNumber, long lDevice, long lSeqNum, long lFlags, BSTR CashSaleData, long *pVal
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			21/10/2002   19:23		Start
******************************************************************************/

void CGCI::SetCashAcceptorData__(long lPumpNumber, long lDevice, long lSeqNum, long lFlags, BSTR CashSaleData, long *pVal)
{
	GCIRetCode retCode = GCI_OK;
	CString sMsg;
 
	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	/***************************************************************************************/
	//4.0.25.30 - we do not need to verify middle of process/recovery - always accept bills.
	/***************************************************************************************/
// 	else if(_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)		
// 		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;

	else if(PUMP_NOT_VALID(lPumpNumber) && !(lFlags & CASH_ACCEPTORS_UNRECOGNIZED_BILL) ) //4.0.27.500
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if (CashSaleData == NULL)
		retCode = GCI_PARAM_NOT_FOUND;


	//4.0.10.50

	if(LOG_DETAIL_FLOW_CONTROL)
	{							
		sMsg.Format("SetCashAcceptorData Data received, lDevice=%ld,lSeqNum=%ld, retCode=%ld ", lDevice,lSeqNum, retCode);
		_LOGMSG.LogFlagsMsg(lPumpNumber,LOG_PUMP,sMsg,lFlags);
	}				
	

	if(retCode == GCI_OK)
	{
		long p = lPumpNumber -1;

//		EnterCriticalSection(&_Module.m_server.m_csPumpArray);				

		CPumpTransact trs;


		switch(lFlags)
		{
			case CASH_ACCEPTORS_TRANSACTION_CREATED:
			{							
				BOOL bFound = _Module.m_server.SearchForStuckTrs(lPumpNumber); //  4.0.24.770  TD 275612

				if (bFound == FALSE)
				{
					// did not find stuck trs, can insert a new cash acceptor trs
			
					retCode =  _Module.m_server.UpdateCashAcceptorData(lPumpNumber, lDevice, lSeqNum, CASH_ACCEPTORS_TRANSACTION_CREATED, CashSaleData);

					_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_CASH_ACCEPTOR_INFO,&trs); // 4.0.24.770  TD 275612

					if(LOG_DETAIL_FLOW_CONTROL)
					{							
						sMsg.Format("Create CashAcceptor Transaction  %ld RetCode =%d" ,trs.m_lNumber ,retCode);  //4.0.10.45
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
					}	

					// first 
					if (retCode == GCI_OK)
					{
						retCode = _Module.m_server.UpdateCashAcceptorData(lPumpNumber, lDevice, lSeqNum, CASH_ACCEPTORS_TRANSACTION_UPDATE_BILLS, CashSaleData);				

						if(LOG_DETAIL_FLOW_CONTROL)
						{							
							sMsg.Format("Update first bill CashAcceptor Transaction  %ld , lSeqNum=%ld RetCode =%d" , trs.m_lNumber ,lSeqNum ,retCode);  //4.0.10.45
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
						}				


						if (retCode)
						{
							if(LOG_DETAIL_FLOW_CONTROL)
							{							
								sMsg.Format("SetCashAcceptorData__ Fail in update bills information !!!!!!!" ); 
								_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
							}				
						}
					}
					else
					{				
						if(LOG_DETAIL_FLOW_CONTROL)
						{							
							sMsg.Format("Fail Create CashAcceptor Transaction RetCode =%d" ,retCode);  //4.0.10.45
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
						}				
					}
				}
				else
				{
					retCode = GCI_PUMP_ALREADY_HAVE_IN_PROGRESS_TRS;
				}

//				}

			}

			break;
			case CASH_ACCEPTORS_TRANSACTION_UPDATE_BILLS:
			case CASH_ACCEPTORS_TRANSACTION_UPDATE_FINAL:
			{
				if ((_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_CASH_ACCEPTOR_INFO,&trs)) ||
					(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_TIME_OUT,TRS_LINK_CASH_ACCEPTOR_INFO,&trs)) || //4.0.10.42
					(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS,TRS_LINK_CASH_ACCEPTOR_INFO,&trs)  ) )
					
//				if ((_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs)) ||
//				(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_TIME_OUT,TRS_LINK_PREPAY_NUMBER,&trs)) || //4.0.10.42
//				(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs)   ) ) // 4.0.28.500
				{			
					if (lFlags & CASH_ACCEPTORS_TRANSACTION_UPDATE_FINAL)
					{
						if(LOG_DETAIL_FLOW_CONTROL)
						{							
							sMsg.Format("SetCashAcceptorData get final information  for trs=%ld " , trs.m_lNumber); 
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
						}				
						retCode =_Module.m_server.UpdateCashAcceptorData(lPumpNumber, lDevice, lSeqNum, lFlags, CashSaleData);
					}
					else
					{
						retCode = _Module.m_server.UpdateCashAcceptorData(lPumpNumber, lDevice, lSeqNum, CASH_ACCEPTORS_TRANSACTION_UPDATE_BILLS, CashSaleData);				
						if(LOG_DETAIL_FLOW_CONTROL)
						{							
							sMsg.Format("SetCashAcceptorData update bills information trs=%ld " , trs.m_lNumber); 
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
						}				
					}
				}

			}
			break;
			case CASH_ACCEPTORS_TRANSACTION_CANCELED:
			{
					BOOL bTimeOut = FALSE;
				
				// SR 123824 
				//4.0.28.500 if ( _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_TIME_OUT,TRS_LINK_PREPAY_NUMBER,&trs))   //4.0.24.140 merge from 4.0.17.1260
				if (_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_TIME_OUT,TRS_LINK_CASH_ACCEPTOR_INFO,&trs)) //4.0.24.372
				{
					// check if transaction is ended.
					if ((trs.m_lResExt == RES_EXT_CASH_ACCEPTOR) && (trs.GetStatus() == WAIT_TIME_OUT))  //4.0.25.30 merge from 4.0.17.1260  123824 
					{
						retCode = _Module.m_server.UpdateCashAcceptorData(lPumpNumber, lDevice, lSeqNum, CASH_ACCEPTORS_TRANSACTION_UPDATE_FINAL, CashSaleData);				
						bTimeOut = TRUE;
					}
				}

				if (((_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs))||
					(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs))) && (!bTimeOut))  //// SR 123824 4.0.25.30 merge fro4.0.17.1260
				{
					retCode = _Module.m_server.UpdateCashAcceptorData(lPumpNumber, lDevice, lSeqNum, CASH_ACCEPTORS_TRANSACTION_CANCELED, CashSaleData);				
				}
				else
				{
					retCode= GCI_TRS_NOT_FOUND;
				}

			}
			break;
				//4.0.11.503 TD 19411
			//in case we not create trs but we need to create payin
			case CASH_ACCEPTORS_TRANSACTION_FORCE_CANCELED:
			{
				CString str;
				char byTmpBuffer[sizeof(TAG_CASH_SALE_DATA) +2];
				TAG_CASH_SALE_DATA cTagCashSaleData;
				//4.0.12.504
				//div the total by 1- so in the tra show will see the current sum
				int iLen = Bstr2Buffer(CashSaleData, (void *)byTmpBuffer,sizeof(TAG_CASH_SALE_DATA));
				memcpy(&cTagCashSaleData,(void *)byTmpBuffer,sizeof(TAG_CASH_SALE_DATA));
				long lTempTotal =  a2l(cTagCashSaleData.sCashDepositTotal,sizeof(cTagCashSaleData.sCashDepositTotal)) / 10;		
				l2a(lTempTotal,cTagCashSaleData.sCashDepositTotal,sizeof(cTagCashSaleData.sCashDepositTotal));

				BYTE sTmpCardDataString[sizeof(TAG_CASH_SALE_DATA)+2];
				Str2StrNull((BYTE*)&cTagCashSaleData, sizeof(TAG_CASH_SALE_DATA),sTmpCardDataString);
				CComBSTR sData(sizeof(sTmpCardDataString), (LPCSTR)sTmpCardDataString);
	
				str.Format("SetCashAcceptorData__ CASH_ACCEPTORS_TRANSACTION_FORCE_CANCELED Create new line and pay in with %ld",lTempTotal); 
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
				
				retCode =  _Module.m_server.UpdateCashAcceptorData(lPumpNumber, lDevice, lSeqNum, CASH_ACCEPTORS_TRANSACTION_CREATED, sData);
				// first 
				if (retCode == GCI_OK)
				{
					retCode = _Module.m_server.UpdateCashAcceptorData(lPumpNumber, lDevice, lSeqNum, CASH_ACCEPTORS_TRANSACTION_UPDATE_BILLS, sData);				
					if (retCode)
					{
						if(LOG_DETAIL_FLOW_CONTROL)
						{							
							sMsg.Format("SetCashAcceptorData__ Fail in update bills information !!!!!!!" ); 
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
						}				
					}
					else
					{
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"SetCashAcceptorData__ cancel and create pay in");
						retCode = _Module.m_server.UpdateCashAcceptorData(lPumpNumber, lDevice, lSeqNum, CASH_ACCEPTORS_TRANSACTION_CANCELED, sData);				
						retCode = _Module.m_server.UpdateCashAcceptorData(lPumpNumber, lDevice, lSeqNum, CASH_ACCEPTORS_TRANSACTION_FORCE_CANCELED, sData);//4.0.10.114

					}
				}
				else
				{				
					if(LOG_DETAIL_FLOW_CONTROL)
					{							
						sMsg.Format("Fail Create CashAcceptor Transaction retcode =%d",retCode);  //4.0.10.45
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
					}				
				}
			}
			break;
			//4.0.17.90 - TD 40564
			case CASH_ACCEPTORS_UNRECOGNIZED_BILL:
			{
				if(LOG_DETAIL_FLOW_CONTROL)
				{							
					sMsg.Format("GCI::SetCashAcceptorData update unrecognized bill  for lDevice=%ld lFlags=%ld", lDevice, lFlags); 
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
				}	
				retCode =  _Module.m_server.UpdateCashAcceptorData(lPumpNumber, lDevice, lSeqNum, CASH_ACCEPTORS_UNRECOGNIZED_BILL, CashSaleData);				
			}
			break;
		}

//		LeaveCriticalSection(&_Module.m_server.m_csPumpArray);
	}

	//4.0.10.50
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"SetCashAcceptorData",
				TRUE,retCode,
				0,0);
	
	if(pVal != NULL)
		*pVal = retCode;
	
}

STDMETHODIMP CGCI::GetCashAcceptorData(long lPumpNumber, long lDevice, long lSeqNum, long lFlags, BSTR *CashSaleData, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		GetCashAcceptorData__(lPumpNumber,lDevice, lSeqNum,lFlags, CashSaleData, pVal);
#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetCashAcceptorData")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","GetCashAcceptorData",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif
	return S_OK;
}

/******************************************************************************
 Description:	retrieved Cash acceptor information  	
 Returns:      	none
 Parameters:   	long lPumpNumber, long lDevice, long lSeqNum, long lFlags, BSTR *CashSaleData, long *pVal
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			27/10/2002   18:27		Start
******************************************************************************/
void CGCI::GetCashAcceptorData__(long lPumpNumber, long lDevice, long lSeqNum, long lFlags, BSTR *CashSaleData, long *pVal)
{
	GCIRetCode retCode = GCI_OK;
	TAG_CASH_SALE_DATA	cTagCashSaleData;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if (CashSaleData == NULL)
		retCode = GCI_PARAM_NOT_FOUND;

		
	memset(&cTagCashSaleData , ' ' , sizeof(cTagCashSaleData));
	if(retCode == GCI_OK)
	{
		EXTRA_INFO_REC	ExtraInfoRec;
		long p = lPumpNumber -1;
		long lRtc = 0;
//		EnterCriticalSection(&_Module.m_server.m_csPumpArray);				

		CPumpTransact trs;
		// Transaction was open Update bills information 
		if ((_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs)) ||
			(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs)) )
		{
			ExtraInfoRec.info.lIndexNumber = trs.m_lNumber;
			lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);

			if(!lRtc)
			{
				CCashAcceptor  cCashAcceptor; 
				cCashAcceptor.GetCashAcceptorInformation(ExtraInfoRec,cTagCashSaleData);	
			}
			else
			{
				retCode = GCI_NO_WAITING_TRS;
			}
		}
		else
		{
			retCode = GCI_NO_WAITING_TRS;			
		}


//		LeaveCriticalSection(&_Module.m_server.m_csPumpArray);				


	}

	BYTE sTmp[sizeof(TAG_CASH_SALE_DATA)+2];
	Str2StrNull((BYTE *)&cTagCashSaleData,sizeof(TAG_CASH_SALE_DATA),sTmp);

	CComBSTR strCashAcceptorData(sizeof(sTmp),(LPCSTR)sTmp); 	
	*CashSaleData = strCashAcceptorData.Copy(); 


	if(retCode)
	if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"GetCashAcceptorData",
			TRUE,retCode,
			0,0);
	
	if(pVal != NULL)
		*pVal = retCode;

}
//4.0.1.38

STDMETHODIMP CGCI::DisplayInfo(long lPumpNumber, BSTR sMsg, long lFlags, long *pRetVal)
{
	#ifdef CATCH_GCI
	__try
	{
#endif
		
		DisplayInfo__(lPumpNumber , sMsg , lFlags , pRetVal);

#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::DisplayInfo")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::DisplayInfo", LOG_LEVEL_0);	
	}
#endif

	return S_OK;
}


void CGCI::DisplayInfo__(long lPumpNumber, BSTR &sMsg, long lFlags, long *pRetVal)
{
	long lRetCode = GCI_OK;

	if(PUMPSRV_NOT_INIT)
		lRetCode = GCI_MUST_INIT_FIRST;

	else if(PUMP_NOT_VALID(lPumpNumber))
		lRetCode = GCI_INVALID_PUMP_NUMBER;
	
	if(lRetCode == GCI_OK)
	{
		BOOL bWriteSuccessful = _Module.m_server.m_cTerminalMessages.AddMessage(sMsg , lPumpNumber);

		if(!bWriteSuccessful)
			lRetCode = GCI_FAILED_IO;
	}

	if(pRetVal)
		*pRetVal = lRetCode;

	if(lRetCode)
	{
		_LOGMSG.LogResult(0,                //Source
			                                 "IGCI",           //Interface name
											 "DisplayInfo",    //Method name
											 TRUE,             //Contains return code
											 lRetCode,         //Return code
											 FALSE,            //Doesn't contain COM result
											 NULL);            //COM result (ignored)
	}

	return;
}



//4.0.1.40
STDMETHODIMP CGCI::QueryData(long lFileNum, long *lIndexNumber, BSTR sKey, long lFlags, BSTR *pRec, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		
		QueryData__(lFileNum, lIndexNumber, sKey, lFlags, pRec, pVal);

#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::QueryData")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CGCI::QueryData", LOG_LEVEL_0);	
	}
#endif	

	return S_OK;
}

void CGCI::QueryData__(long lFileNum, long *lIndexNumber, BSTR sKey, long lFlags, BSTR *pRec, long *pVal)

{
	TAG_EXTRA_FILE_INFO		cTagExtraFileInfo;
	EXTRA_INFO_REC			cExtraInfoRec;

	GCIRetCode retCode = GCI_OK;
	long lRtc = 0;

	memset(&cTagExtraFileInfo , ' ' , sizeof(cTagExtraFileInfo)); //4.0.15.11
	switch(lFileNum)
	{
		case EXTRA_INFO_FILE:
		{
			CPumpTransact			cTrs;
			
			cTrs.SetLink(TRS_LINK_EXTRA_INFO , 0);
			memset(&cExtraInfoRec , 0 , sizeof(cExtraInfoRec));			
			cExtraInfoRec.info.lIndexNumber = *lIndexNumber;

			if ((long)REC_READ  == (eFileOperation)lFlags)
			lRtc = _Module.m_server.TrsLinkOperation((eFileOperation)lFlags , &cTrs, &cExtraInfoRec);

			if (!lRtc)
			{ 
				//4.0.9.504
				for(int i=0 ; i < MAX_GRADES; i++)		
					l2a( cExtraInfoRec.info.lDiscountArray[i] ,cTagExtraFileInfo.sDiscountArray.sDiscount[i] , sizeof(cTagExtraFileInfo.sDiscountArray.sDiscount[i]));
				
				//4.0.15.10 - TD 34487
				memcpy(cTagExtraFileInfo.sPromotionNumber, cExtraInfoRec.info.sPromotionNumber, sizeof(cTagExtraFileInfo.sPromotionNumber));
				memcpy(cTagExtraFileInfo.sTotalBalance ,   cExtraInfoRec.info.sTotalBalance, sizeof(cTagExtraFileInfo.sTotalBalance));
			    cTagExtraFileInfo.sBalanceType = cExtraInfoRec.info.sBalanceType;
				cTagExtraFileInfo.sGRDiscountType =  cExtraInfoRec.info.sGRDiscountType;
				memcpy(cTagExtraFileInfo.sGRDiscountPerGallon,  cExtraInfoRec.info.sGRDiscountPerGallon, sizeof(cTagExtraFileInfo.sGRDiscountPerGallon));	

				//4.0.2.21
				memcpy(cTagExtraFileInfo.sAttendantID   , cExtraInfoRec.info.sAttendantID, sizeof(cTagExtraFileInfo.sAttendantID));
				memcpy(cTagExtraFileInfo.sAttendantName , cExtraInfoRec.info.sAttendantName, sizeof(cTagExtraFileInfo.sAttendantName));
				l2a( cExtraInfoRec.info.lAuthAmt ,cTagExtraFileInfo.sAuthAmt , sizeof(cTagExtraFileInfo.sAuthAmt));

				memcpy(cTagExtraFileInfo.sAccountNumber , cExtraInfoRec.info.sAccountNumber, sizeof(cTagExtraFileInfo.sAccountNumber));
				memcpy(cTagExtraFileInfo.sAuthNumber , cExtraInfoRec.info.sAuthNumber, sizeof(cTagExtraFileInfo.sAuthNumber));
				//4.0.2.21
				
				//4.0.3.59
				memcpy(cTagExtraFileInfo.sTrack1Data , cExtraInfoRec.info.sTrack1Data, sizeof(cTagExtraFileInfo.sTrack1Data));
				memcpy(cTagExtraFileInfo.sTrack2Data , cExtraInfoRec.info.sTrack2Data, sizeof(cTagExtraFileInfo.sTrack2Data));

				//4.0.2.35
				l2a(_Module.m_server.GetActiveShiftNumber() , (BYTE*)cTagExtraFileInfo.sActiveShiftNumber , sizeof(cTagExtraFileInfo.sActiveShiftNumber));


				//4.0.6.506
				memcpy(cTagExtraFileInfo.sCardName , cExtraInfoRec.info.sCardName, sizeof(cTagExtraFileInfo.sCardName));
				cTagExtraFileInfo.sClearanceID =  cExtraInfoRec.info.sClearanceID;
				memcpy(cTagExtraFileInfo.sOLDSession , cExtraInfoRec.info.sOLDSession, sizeof(cTagExtraFileInfo.sOLDSession));
				memcpy(cTagExtraFileInfo.sMediaMasterType , cExtraInfoRec.info.sMediaMasterType, sizeof(cTagExtraFileInfo.sMediaMasterType));
				//4.0.6.506

				//4.0.6.507
				memcpy(cTagExtraFileInfo.sExpire , cExtraInfoRec.info.sExpire, sizeof(cTagExtraFileInfo.sExpire)); // 4.0.6.507
				memcpy(cTagExtraFileInfo.sCustomerID , cExtraInfoRec.info.sCustomerID, sizeof(cTagExtraFileInfo.sCustomerID)); // 4.0.8.507

				//4.0.9.500
				memcpy(cTagExtraFileInfo.sFullVehicleId , cExtraInfoRec.info.sFullVehicleId, sizeof(cTagExtraFileInfo.sFullVehicleId));
				memcpy(cTagExtraFileInfo.sReferenceNumber , cExtraInfoRec.info.sReferenceNumber, sizeof(cTagExtraFileInfo.sReferenceNumber));
				memcpy(cTagExtraFileInfo.sCustomerName , cExtraInfoRec.info.sCustomerName, sizeof(cTagExtraFileInfo.sCustomerName));

				//4.0.9.506
				memcpy(cTagExtraFileInfo.sManagerID , cExtraInfoRec.info.sManagerID, sizeof(cTagExtraFileInfo.sManagerID));
				
				//4.0.9.503
				memcpy(cTagExtraFileInfo.sPMTReferenceNumber , cExtraInfoRec.info.sPMTReferenceNumber, sizeof(cTagExtraFileInfo.sPMTReferenceNumber));
						


				//4.0.9.180	amram
				memcpy(cTagExtraFileInfo.sLoyalId ,  cExtraInfoRec.info.sLoyalId , sizeof(cExtraInfoRec.info.sLoyalId));

				//4.0.9.180	amram		
				memcpy(cTagExtraFileInfo.sClFlags , cExtraInfoRec.info.sClFlags, sizeof(cTagExtraFileInfo.sClFlags));								

				//4.0.9.180
				memcpy(cTagExtraFileInfo.sServiceFee , cExtraInfoRec.info.sServiceFee, sizeof(cTagExtraFileInfo.sServiceFee));								

				cTagExtraFileInfo.sIgnoreServiceFee = cExtraInfoRec.info.sIgnoreServiceFee; //4.0.10.20
				cTagExtraFileInfo.sDryItemsRestriction = cExtraInfoRec.info.sDryItemsRestriction; //4.0.10.20

				//4.0.12.32
				l2a(cExtraInfoRec.info.lPreSetlimit ,  cTagExtraFileInfo.sPreSetlimit , sizeof(cTagExtraFileInfo.sPreSetlimit));
				cTagExtraFileInfo.sPreSetLimitType = cExtraInfoRec.info.sPreSetLimitType;
				
				//4.0.15.10
				memcpy(cTagExtraFileInfo.sNonDiscountTotalAmount , &cExtraInfoRec.info.sNonDiscountTotalAmount[2], sizeof(cTagExtraFileInfo.sNonDiscountTotalAmount));				

				//copy also the lControllerSeqNum
				l2a(cExtraInfoRec.info.lControllerSeqNum ,  cTagExtraFileInfo.sControllerSeqNum  , sizeof(cTagExtraFileInfo.sControllerSeqNum)); //4.0.22.506 - TD 75373 merge
										
			}

			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString sMsg;
					sMsg.Format("QueryData lIndexNumber=%ld Atendant=%.15s ,sAccountNumber=%.4s** , sAuthNumber=%.12s, lAuthAmt=%ld,Expire=%.4s, sCardName=%.8s ,sClearanceID=%c,sOLDSession=%.6s,sMediaMasterType=%.2s sCustomerID=%.10s , sClFlags=%.2s , ServiceFee=%.5s , sGRDiscountPerGallon=%.5s , sGRDiscountType=%c , sNonDiscountTotalAmount=%.8s , sPreSetLimitType = %c , sPreSetLimit = %.7s" , 
					*lIndexNumber,
					cTagExtraFileInfo.sAttendantName,
					cExtraInfoRec.info.sAccountNumber ,
					cExtraInfoRec.info.sAuthNumber,
					cExtraInfoRec.info.lAuthAmt ,
					cExtraInfoRec.info.sExpire, 
					cExtraInfoRec.info.sCardName, 
					cExtraInfoRec.info.sClearanceID,
					cExtraInfoRec.info.sOLDSession,
					cExtraInfoRec.info.sMediaMasterType,
					cTagExtraFileInfo.sCustomerID , 
					cTagExtraFileInfo.sClFlags , 
					cExtraInfoRec.info.sServiceFee,
					cTagExtraFileInfo.sGRDiscountPerGallon,     //4.0.15.10
					cTagExtraFileInfo.sGRDiscountType,			//4.0.15.10
					cTagExtraFileInfo.sNonDiscountTotalAmount,  //4.0.15.10
					cTagExtraFileInfo.sPreSetLimitType,			//4.0.15.500
					cTagExtraFileInfo.sPreSetlimit				//4.0.15.500
					);	
					_LOGMSG.LogMsg(sMsg);   //4.0.8.507
			}
			

			if((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
				retCode = GCI_TRS_NOT_FOUND;				
			else if (lRtc)
				retCode = GCI_FAILED_IO;
		}
		break;
		default:
		{
			retCode = GCI_TRS_NOT_FOUND;
		}
		break;
	}


	BYTE sTmp[sizeof(TAG_EXTRA_FILE_INFO)+2];
	Str2StrNull((BYTE *)&cTagExtraFileInfo,sizeof(TAG_EXTRA_FILE_INFO),sTmp);
	CComBSTR strResult(sizeof(sTmp),(LPCSTR)sTmp); 
	*pRec = strResult.Copy();


	if(pVal)
		*pVal = retCode;

	if(retCode)
	if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"QueryData",
			TRUE,retCode,
			TRUE,lRtc);


	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString sMsg;
		sMsg.Format("CGCI::QueryData to File=%ld Operation=%ld, key=%ld, QdxRetCode=%ld , retCode=%ld", lFileNum, lFlags, *lIndexNumber, lRtc,retCode);
		_LOGMSG.LogMsg(sMsg);
	}




}
//4.0.1.40

/******************************************************************************
 Description:	Get swipe data from payment terminal, and convert it into
                an OLA readable string for the PMT.
 Returns:      	void (real return code stored in pRet)
 Parameters:   	long lPumpNumber - Number of pump where the transaction took place.
                BSTR sCardData - data received from payment card swipe
				BSTR * sOLABuffer - OLA readable string
				long lFlags -  reserved for future use, set to 0
				long *pRet - GCI return code
 Assumptions:   sCardData is actually a CARD_SALE_ALL2 struct.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			1/1/2003   13:16		Start
******************************************************************************/

void CGCI::GetOLABuffer__(long lPumpNumber , BSTR sCardData, BSTR * sOLABuffer, long lFlags, long *pRet)
{
	long lOlaRecSize = (lFlags & SWIPE_THREE_OLA_REC)?sizeof(CARD_SALE_ALL3):sizeof(CARD_SALE_ALL2);
	GCIRetCode lRetCode = GCI_OK;
	CString sMsg;	

	long lPumpIndex = lPumpNumber - 1;

	PAY_AT_PUMP_INFO    cPayAtPumpInfo;  //4.0.5.0
	memset(&cPayAtPumpInfo , ' ' , sizeof(cPayAtPumpInfo));

	char sOutBuffer[3048]; //4.0.5.0
	memset((char*)sOutBuffer , 0 , sizeof(sOutBuffer));

	//Verify parameter validity
	if(PUMPSRV_NOT_INIT)
		lRetCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		lRetCode = GCI_INVALID_PUMP_NUMBER;

	if(lRetCode == GCI_OK)
	{
		//4.0.20.503 66960
		CARD_SALE_ALL_CHIP_EXTRA_DATA	cardDataAll3ChipExtra;	
		memset(&cardDataAll3ChipExtra, ' ', sizeof(cardDataAll3ChipExtra));

		int iLen = 0;
		lRetCode = _Module.m_server.m_cXmlConvertorEx.GetStructCardSaleAll3ChipExtra(sCardData, lOlaRecSize, &cardDataAll3ChipExtra, &iLen);	//4.0.20.503 66960

		cPayAtPumpInfo.CopyCardSaleAll3(&cardDataAll3ChipExtra.cCardSaleAll3); // coverity
		//memcpy(&cPayAtPumpInfo.CardSaleInfo.cardData, &cardDataAll3ChipExtra.cCardSaleAll3.CardSaleAll.data, lOlaRecSize);	//4.0.20.507
		
	}
	
	//Parameters are valid, continue method execution
	if(lRetCode == GCI_OK)
	{
		if (lFlags & DATA_IS_SESSION_AUTHORIZE)
		{
			char sTmpBuffer[50];
			_Module.m_server.m_cOLASrv->BuildRequest(SESSION_AUTHORIZE,
												 lPumpNumber,
												 (char*)&cPayAtPumpInfo,
												 (char*)sOutBuffer,
												 NULL, TRUE); // 4.0.6.501

			
			sprintf_s(sTmpBuffer ,sizeof(sTmpBuffer), "ShortCardName[S]=%.8s",cPayAtPumpInfo.CardSaleInfo.cardData.sCardName);
			char sOutBuffer[3048]; //4.0.5.0
			strcat_s(sOutBuffer,sizeof(sOutBuffer) ,sTmpBuffer);			

		}
		else if (lFlags & DATA_IS_SESSION_COMPLETE)
		{
			char sTmpBuffer[50];
			memset(sTmpBuffer, 0, sizeof(sTmpBuffer));
			_Module.m_server.m_cOLASrv->BuildRequest(SESSION_COMPLETE,
									 lPumpNumber,
									 (char*)&cPayAtPumpInfo,
									 (char*)sOutBuffer,
									 NULL, TRUE); // 4.0.6.501

			sMsg.Format("GetOLABuffer= sTranStatus=[%c]",cPayAtPumpInfo.CardSaleInfo.cardData.sTranStatus);
		    _LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);	
			
   			sprintf_s(sTmpBuffer,sizeof(sTmpBuffer),"ResultCode[I]=0,");
			strcat_s(sOutBuffer,sizeof(sOutBuffer) ,sTmpBuffer);			

		}
	}

	//Insert values into method's OUT parameters.  If method parameters
	//were invalid, output BSTR will be empty.
	CComBSTR bstrOutBuffer(strlen((char*)sOutBuffer) , (LPCSTR)sOutBuffer);
	*sOLABuffer = bstrOutBuffer.Copy();

	if(pRet)
		*pRet = (long)lRetCode;
	//Write standard failure message to log in case of error
	if(lRetCode)
	{
		_LOGMSG.LogResult(0,                 //Source
			                                 "GCI",             //Interface name
											 "GetOLABuffer",    //Method name
											 TRUE,              //Log return value
											 lRetCode,          //Return value
											 FALSE,             //Log COM result
											 0,                 //COM result
											 NULL);             //COM exception
	}

	return;
}

STDMETHODIMP CGCI::GetOLABuffer(long lPumpNumber, BSTR sCardData, BSTR *sOLABuffer, long lFlags, long *pRet)
{
#ifdef CATCH_GCI
	__try
	{
#endif
		GetOLABuffer__(lPumpNumber , sCardData , sOLABuffer , lFlags , pRet);

#ifdef CATCH_GPI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetOLABuffer")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetOLABuffer", LOG_LEVEL_0);	
	}
#endif

	return S_OK;
}
/******************************************************************************
 Description:	Close fuel shift
 Returns:      	GCI ret code

 Parameters:   	short nDayOfWeek,  - Set shift to  special days (1-7). 
				short nShiftNumber - Update Shift number   
				short nShiftFlags  - Shift operaion see table #001
				BSTR  sStartTime,  - Date and time to start Future shift YYYYMMDDHHMMSS.
				long *pVal	 	   - CTRL ret code 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			1/1/2003   15:06		Start
******************************************************************************/
//4.0.2.21
STDMETHODIMP CGCI::SetShift(short nDayOfWeek, short nShiftNumber, short nShiftFlags, BSTR sStartTime, long *pVal)
{
// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		
		SetShift__(nDayOfWeek,nShiftNumber, nShiftFlags, sStartTime, pVal);

#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::SetShift")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetShift", LOG_LEVEL_0);	
	}
#endif
	return S_OK;
}

/******************************************************************************
 Description:	Close fuel shift
 Returns:      	GCI ret code

 Parameters:   	short nDayOfWeek,  - Set shift to  special days (1-7). 
				short nShiftNumber - Update Shift number   
				short nShiftFlags  - Shift operaion see table #001
				BSTR  sStartTime,  - Date and time to start Future shift YYYYMMDDHHMMSS.
				long *pVal	 	   - CTRL ret code 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			1/1/2003   15:06		Start
******************************************************************************/

void CGCI::SetShift__(short nDayOfWeek, short nShiftNumber, short nShiftFlags, BSTR sStartTime, long *pVal)
{
	long lRetCode = 0;
	long lRtc = 0;

	_Module.m_server.SetShift(nDayOfWeek, nShiftNumber, nShiftFlags, sStartTime, &lRetCode , lRtc);

		switch (lRetCode)
		{
			case CTRL_FAILED_IO:	
				lRetCode = GCI_FAILED_IO;
				break;
			case CTRL_IN_MIDDLE_OF_SHIFT:
				lRetCode = GCI_SERVER_IN_MIDDLE_LOCK_SHIFT;
				break;
			case CTRL_PUMP_NOT_READY:	
				lRetCode = GCI_PUMP_NOT_READY;
				break;
			case CTRL_OPEN_PRE_PAY_TRS:	
				lRetCode = GCI_OPEN_PRE_PAY_TRS;
				break;
			case CTRL_OPEN_PAY_AT_KIOSK_TRS :	
				lRetCode = GCI_OPEN_PAY_AT_KIOSK_TRS;
				break;
			case CTRL_OPEN_PAY_AT_PUMP_TRS:	
				lRetCode = GCI_OPEN_PAY_AT_PUMP_TRS;
				break;
			case CTRL_INVALID_DAYOFWEEK:	
				lRetCode = GCI_INVALID_DAYOFWEEK;
				break;
			case CTRL_LAST_ERROR:
				lRetCode = GCI_LAST_ERROR;
				break;
			case CTRL_SHIFT_START_WITH_PUMP_NOT_IDLE://4.0.10.501
				lRetCode = GCI_SHIFT_START_WITH_PUMP_NOT_IDLE;
				break;
			case CTRL_OK:
				lRetCode = GCI_OK;
				break;

		}

	if(lRetCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"SetShift",
				TRUE,lRetCode,
				(lRtc!=0)? TRUE:FALSE,lRtc);

	if(pVal != NULL)
		*pVal = lRetCode;
}
//4.0.2.21


STDMETHODIMP CGCI::SetMaintenance(long lLock, long lParam, long *pVal)
{
	// TODO: Add your implementation code here
// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif		

		SetMaintenance__(lLock, lParam, pVal);


#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::SetMaintenance")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetMaintenance", LOG_LEVEL_0);	
	}
#endif

	return S_OK;
}
/******************************************************************************
 Description:	Maintenance  Server , Lock/unlock 	
 Returns:      	GciRetCode]
 Parameters:   	long lLock, long lParam, long *pVal

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			24/2/2003   12:55		Start
******************************************************************************/
//4.0.2.36
void CGCI::SetMaintenance__(long lLock, long lParam, long *pVal)
{
	long lRetCode;
	GCIRetCode  retCode = GCI_OK;

	if(_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
	{
		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
	}

	if (retCode == GCI_OK)
	{

		_Module.m_server.SetMaintenance(lLock,lParam,&lRetCode);


		switch (lRetCode)
		{
			case CTRL_CONTROLLER_NOT_AVAILABLE:
				retCode = GCI_CONTROLLER_NOT_AVAILABLE;
				break;
			case CTRL_SERVER_NOT_READY:
				retCode = GCI_SERVER_NOT_READY;
				break;
			case CTRL_INVALID_LOCK_OPTION:
				retCode = CGI_INVALID_LOCK_OPTION;
				break;
			case CTRL_IN_MIDDLE_OF_PROCESS:
				retCode = GCI_IN_MIDDLE_OF_PROCESS;
				break;
		}

	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"SetMaintenance",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}
//4.0.2.36

// 

/******************************************************************************
 Description:	Convert hydService return code to GCiRetCode to be generic with the return code 	
 Returns:      	GCIRetCode
 Parameters:    CFGDBRetCode code 	- hydService return code;
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			14/4/2003   9:47		Start
******************************************************************************/
GCIRetCode CGCI::ConverHydServiceCode2GciCode(CFGDBRetCode code)
{
	GCIRetCode retCode =GCI_OK;

	switch(code)
	{
		case CFGDB_OK:
			retCode = GCI_OK;
			break;		
		case CFGDB_FAILED_TO_OPEN_TABLE:			//1
			retCode = GCI_FAILED_TO_OPEN_TABLE;
			break;
		case CFGDB_PARAM_NOT_FOUND_AND_CREATED:		//2 in the get functions means param not found.
			retCode = GCI_PARAM_NOT_FOUND_AND_CREATED;
			break;
		case CFGDB_PARAM_NOT_FOUND_FAILED_TO_CREATE:	//3
			retCode = GCI_PARAM_NOT_FOUND_FAILED_TO_CREATE;
			break;
		case CFGDB_PARAM_NOT_FOUND:					//4
			retCode = GCI_PARAM_NOT_FOUND;
			break;

		case CFGDB_FIELD_EMPTY:						//9
			retCode = GCI_FIELD_EMPTY;
			break;
		case CFGDB_SERVER_NAME_TOO_LONG:			//5
		case CFGDB_TABLE_NAME_TOO_LONG:				//6
		case CFGDB_FIELD_NAME_TOO_LONG:				//7
		case CFGDB_DEFAULT_FIELD_VAL_TOO_LONG:		//8
		case CFGDB_INVALID_GROUP_NUMBER:				//10
		case CFGDB_INVALID_EVENT_GROUP_NUMBER:		//11
		case CFGDB_EXCEPTION_ERROR:					//12
			retCode = GCI_FAILED_TO_OPEN_TABLE;
			break;

	}

	return (retCode);
}

							   
STDMETHODIMP CGCI::GetCardInfo2(long lPumpNumber, long lDeviceNumber, long *lSeqNumber, long lFlags, BSTR sData, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif		
		GetCardInfo2__(lPumpNumber, lDeviceNumber, lSeqNumber, lFlags, sData, pVal);

#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetCardInfo2")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","GetCardInfo2",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	
	}
#endif
	return S_OK;
}

//4.0.3.21
void CGCI::GetCardInfo2__(long lPumpNumber, long lDeviceNumber, long *lSeqNumber, long lFlags, BSTR sData, long *pVal)
{
	long lOlaRecSize = (lFlags & SWIPE_THREE_OLA_REC)?sizeof(CARD_SALE_ALL3):sizeof(CARD_SALE_ALL2);
	DWORD dwLastErr = 0;
	OLA_STAT ola;
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GCI_FAILED_IO;
	else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
		retCode = GCI_SERVER_IN_MIDDLE_LOCK_SHIFT;   //4.0.1.40

	
	long lRtc = 0;
	BOOL bAskUserToTryAgain = FALSE;
	char sXmlBuff[MAX_XML_BUF_LEN];
	memset(&sXmlBuff,0,MAX_XML_BUF_LEN);
	int iLen = 0;
	CARD_SALE_ALL3 cardDataAll3; //4.0.20.503 66960
	memset(&cardDataAll3,0,sizeof(CARD_SALE_ALL3));

	//if the buffer is not as xml, convert it to xml
	if (!(_Module.m_server.m_bUseXmlBuffer))
	{
		iLen = Bstr2Buffer(sData, (void *)&cardDataAll3,lOlaRecSize);

		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;
		}
		//4.0.20.503 66960 else was deleted
	}
	else
	{
		//The buffer is already in xml format
		iLen = Bstr2Buffer(sData, (void *)&sXmlBuff,MAX_XML_BUF_LEN);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;
		}
		else	//4.0.20.503 66960
		{
			if (!(_Module.m_server.m_cXmlConvertorEx.ConvertFromXmlToStruct(sXmlBuff,&cardDataAll3)))
			{
				retCode = GCI_FAILED_IO;
			}
		}
	}

	CString str;
	str.Format("GetCardInfo2, iLen=%d  ,",iLen);
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);

	//4.0.20.503 66960 delete Init the xml interface
	
	if ( retCode == GCI_OK )
	{

		//rt change
		//Temp convert to struct from xml.
		//rt end 

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
//		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		_Module.m_server.m_cOLASrv->ResetConnectionAttempts();
		_Module.m_server.m_cOLASrv->ResetRetryAttemps(TRUE);

		long  p = lPumpNumber-1;
		_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);

		
		if (OLA_STATE(ola.m_byState) == OLA_NONE)
		{
			char sBuff[MAX_XML_BUF_LEN] = {0};

			//4.0.5.39
			if (lFlags & SWIPE_THREE_OLA_REC)
			{//4.0.20.503 66960
				if (!_Module.m_server.GetOLAFlags((BYTE*)cardDataAll3.extData2.sFlags ,FL_PAP_AS_THIRD_REC))
				{
					long lTmp = a2l(cardDataAll3.extData2.sFlags, sizeof(cardDataAll3.extData2.sFlags)) + FL_PAP_AS_THIRD_REC;
					l2a(lTmp, cardDataAll3.extData2.sFlags, sizeof(cardDataAll3.extData2.sFlags));
				}			
			}			

			ola.m_lSessionNumber =0;
			ola.m_byState = OLA_REQUEST_TO_SEND | SESSION_INQUIRY;			
			
			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola); 	
			//4.0.18.501
			PAY_AT_PUMP_INFO cPAPInfo;
			memset(&cPAPInfo, ' ', sizeof(cPAPInfo));
			
			//4.0.20.503 66960
			memcpy((char *)&cPAPInfo.CardSaleInfo.cardData, (char *)&cardDataAll3.CardSaleAll.data, lOlaRecSize);	//4.0.20.507

			_Module.m_server.m_cPumpArray[lPumpNumber -1].SetPAPInfo(&cPAPInfo);			
			_Module.m_server.SavePumpInfo(p+1); 
		}
		else if (OLA_STATE(ola.m_byState) == OLA_WAIT_FOR_MORE_INFO)
		{
			//4.0.18.501
			//4.0.20.503 66960
			long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_EXTRA_INFO,lPumpNumber, *lSeqNumber,&cardDataAll3);
			if(lRtc)
			{		
				// Call swipe result with summarized code relating 
				// to the requested session, and not relating to the current pOla->byStat									
				ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_SEND_END);
				if(LOG_BASIC_FLOW_CONTROL)
					_LOGMSG.LogOLAMsg(lPumpNumber,ola.m_byState,ola.m_lSessionNumber);
				retCode = GCI_FAILED_IO;
			}
			else
			{				
				// Trigger OLA process
				//--------------------									
				ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_REQUEST_TO_SEND_MORE);									
				if(LOG_BASIC_FLOW_CONTROL)
					_LOGMSG.LogOLAMsg(lPumpNumber,ola.m_byState,ola.m_lSessionNumber);				
			}

			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola); 	
			_Module.m_server.SavePumpInfo(p+1);
		}	
		else if (lFlags & SWIPE_CANCEL)
		{
			bAskUserToTryAgain = TRUE;		
	
			retCode = SwipeCancel(lPumpNumber, *lSeqNumber, &ola, &cardDataAll3, &bAskUserToTryAgain);														
			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola); 	
			_Module.m_server.SavePumpInfo(p+1); 

			if(LOG_BASIC_FLOW_CONTROL)
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Cancel GetCardinfo session");
		}
		else
		{
			if(LOG_BASIC_FLOW_CONTROL)			
			{	
				CString str;
				str.Format("GetCardInfo2, failed,due to open ola session=%ld  ,",ola.m_lSessionNumber);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
			}			
		}
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
//		LeaveCriticalSection(&_Module.m_server.m_csPumpArray);
	}


	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"GetCardInfo2",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);

	if(pVal != NULL)
		*pVal = retCode;

}

//4.0.3.21

//4.0.3.51
void CGCI::ReconfigurationEvent(long lTableId , long lSubTableId , long lSubTableIdExt , long lFlags)
{
	long lPumpNumber = 0 , lMask = 1;
	DWORD dwLogLevel = DEFAULT_LOG_LEVEL;
	CtrlRetCode lRetCode = CTRL_OK;

	// RFUEL - 2817
	bool bIsOLESRV_EPSILON = false;

	CRegTable cTable;
	CString sLogMsg("CGCI::Reconfigure2 - ") , sRetCode;

	if(!cTable.ReconfigInProgress())
	{
		cTable.SetReconfigInProgress((BYTE)TRUE);

		switch(lTableId)
		{
		case TERMINALS_TABLE:
			cTable.LoadTerminalTable(FALSE , TRUE);
			sLogMsg += "TERMINALS_TABLE (performed in CmdTX), ";
			break;
		
		case PUMPS_TABLE:
			if(lFlags & SUB_TABLE_IS_DEVICE_MAP)
			{
				for(int i = 0;i < 32;i++)
				{
					lPumpNumber = lMask & lSubTableId;

					if(lPumpNumber)
					{
						cTable.LoadDispensersTable(i + 1 , FALSE , TRUE , TRUE);
						cTable.LoadPumpsTable(i + 1 , FALSE , TRUE , TRUE);
					}

					lPumpNumber = lMask & lSubTableIdExt;

					if(lPumpNumber)
					{
						cTable.LoadDispensersTable(i + 33 , FALSE , TRUE , TRUE);
						cTable.LoadPumpsTable(i + 33 , FALSE , TRUE , TRUE);
					}

					lMask <<= 1;
				}
			}

			else
			{	
				lPumpNumber = 0;
				cTable.LoadDispensersTable(lPumpNumber , FALSE , TRUE , TRUE);
				cTable.LoadPumpsTable(lPumpNumber , FALSE , TRUE , TRUE);
			}

			sLogMsg += "PUMPS_TABLE (performed in CmdTX), ";
			
			//RFUEL-2817, Include OLASRV_EPSILON_DISCOUNT to condition for 4.0.5.39
			bIsOLESRV_EPSILON =
				(_Module.m_server.GetOLAInUse() == OLASRV_EPSILON) ||
				(_Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT);

			//4.0.5.39
			if ((_Module.m_server.m_cOLASrv != NULL) &&
				(_Module.m_server.m_cOLASrv->SupportIFSFProtocoll() || bIsOLESRV_EPSILON)) // RFUEL-1213 - Since Epsilon implementation, pumps don't work properly after Pump configuration (FCC). A reboot of Fuel or re-load of Epsilon is required.
				_Module.m_server.m_cOLASrv->LoadAllProductSoldOnPump(lPumpNumber);

			break;
		case CARWASH_TABLE:     //4.0.5.39
		case GRADES_TABLE:
			if ((_Module.m_server.m_cOLASrv != NULL) && _Module.m_server.m_cOLASrv->SupportIFSFProtocoll())
				_Module.m_server.m_cOLASrv->LoadAllProductSoldOnPump(0);
			break;
		case GENERAL_PARAM_TABLE:	//4.0.18.501 merge from 4.0.18.31
			{//4.0.18.31
				DWORD lOldShiftsToMaint = 0, lNewShiftsToMaint = 0;
				_Module.m_server.m_cParam.LoadParam("GeneralParam", "ShiftsToMaintenance",&lOldShiftsToMaint,(DWORD)0,TRUE,FALSE,0,0,TRUE);
				lNewShiftsToMaint = _Module.m_server.GetReloadQDXEveryXShifts() - _Module.m_server.GetReloadQDXShiftsCounter();
				if ((lNewShiftsToMaint != lOldShiftsToMaint))	//There was a change in the X number of shifts
				{
					_Module.m_server.SetReloadQDXShiftsCounter(0);
					_Module.m_server.m_cParam.SaveParam("GeneralParam","ShiftsToMaintenance",_Module.m_server.GetReloadQDXEveryXShifts(),FALSE,0,0,0,TRUE);//33898
					if (lOldShiftsToMaint == 1)
					{//We have to stop the alarm that was sent before 
						_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, NEXT_SHIFT_INCLUDES_RELOAD, 0, ALARM_ENDED, 0, 0);

					}
					if (_Module.m_server.GetReloadQDXEveryXShifts() - _Module.m_server.GetReloadQDXShiftsCounter() == 1)
					{//We have to start the alarm according to the new parameter
						_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, NEXT_SHIFT_INCLUDES_RELOAD, 0, ALARM_STARTED, 0, 0);
					}
				}
			}			

			break;
		default:
			break;
		}

		cTable.SetReconfigInProgress((BYTE)FALSE);
	}
	
	else
		_LOGMSG.LogMsg("ReconfigurationEvent - another reconfiguration request has already been made");
	
	sRetCode.Format("return code: %d" , lRetCode);

	sLogMsg += sRetCode;

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg);

	return;
}

STDMETHODIMP CGCI::MoveTerminalTransactionToPump(long lTerminalId, long lPumpId, long lStatus, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif		

		MoveTerminalTransactionToPump__(lTerminalId, lPumpId,lStatus,lFlags,  pVal);

#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::MoveTerminalTransactionToPump")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::MoveTerminalTransactionToPump", LOG_LEVEL_0);	
	}
#endif

	return S_OK;

}
/******************************************************************************
 Description:	Move Terminal transaction to selected Pump. 	
 Returns:      	None
 Parameters:    long lTerminalId - terminal created the source of the transaction
				long lPumpId - destination Pump 
				long lStatus - not use 
				long lFlags  - not use
				long *pVal	 - ret code
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			13/10/2003   19:01		Start
******************************************************************************/

void CGCI::MoveTerminalTransactionToPump__(long lTerminalId, long lPumpId, long lStatus, long lFlags, long *pVal)
{
	CPumpTransact trs;
	CPumpTransact NewTrs;
	BOOL bFound = FALSE;
	long retCode= 0;

	if(_Module.m_server.IsServerInRecoveryProcess() && _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;

	else if(PUMP_NOT_VALID(lPumpId))
		retCode= 0;
	
	else if ((lPumpId < 1) || (lPumpId > MAX_PUMPS_64))
		retCode = GCI_INVALID_PUMP_NUMBER;



	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;
		str.Format("MoveTerminalTransactionToPump lTerminalId=%ld, status=%ld" , lTerminalId,lStatus);
		_LOGMSG.LogMsg(lPumpId,LOG_PUMP,str);
	}

	if(retCode == GCI_OK)
	{
		//	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		bFound = _Module.m_server.m_cPumpArray[lTerminalId -1 ].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&trs);

		if (bFound)
		{	
			// Pump destination should be fee to fill the pap transaction.
			if ((
				(_Module.m_server.m_cPumpArray[lPumpId- 1].GetStat().GetRes() == NULL_RES) &&
				(_Module.m_server.m_cPumpArray[lPumpId- 1].GetStat().GetStatus() == READY) &&
				(_Module.m_server.m_cPumpArray[lPumpId- 1].GetStat().GetResExt() == NULL_RES_EXT)
				) ||
				(_Module.m_server.m_cPumpArray[lPumpId- 1].GetStat().GetRes() == PAY_AT_PUMP))
			{
				PAY_AT_PUMP_INFO	SourcePayAtPumpInfo;
				NewTrs = trs;
				//Insert Terminal transaction to selected pump.
				NewTrs.m_lPump = lPumpId;
				_Module.m_server.CentralInsertTrs(lPumpId, &NewTrs);
				trs.Clear();
				_Module.m_server.CentralUpdateTrs(lTerminalId, NewTrs.m_lNumber, &trs,FALSE);
				_Module.m_server.m_cPumpArray[lTerminalId-1].GetPAPInfo(&SourcePayAtPumpInfo);						
				_Module.m_server.m_cPumpArray[lPumpId-1].SetPAPInfo(&SourcePayAtPumpInfo);
				retCode = 0;
			}						
		}
//		LeaveCriticalSection( &_Module.m_server.m_csPumpArray );
	}
	
	
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"MoveTerminalTransactionToPump",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CGCI::PAKReadyTransaction2(long lPumpNumber, long lTrsNumber, short nNozzle, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade, long lFlags, long lTotalVal, BSTR pRec, long *pVal)
{
#ifdef CATCH_GCI
	__try
	{
#endif

	PAKReadyTransaction__(lPumpNumber,lTrsNumber, nNozzle, lVolume, lValue, nLevel, lGradePrice, nGrade,pVal , lFlags,lTotalVal, pRec);

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		char str[256];
		sprintf_s(str,256,"PAKReadyTransaction2(Pump %02ld lFlags=%ld, lVal=%ld)",lPumpNumber,lFlags,lTotalVal);
		_LOGMSG.LogMsg(str);
	}

#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::PAKReadyTransaction2")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::PAKReadyTransaction2", LOG_LEVEL_0);	
	}
#endif
	
	return S_OK;
}

STDMETHODIMP CGCI::SetPumpMode(long lPumpNumber, long lPumpMode, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif		
	SetPumpMode__(lPumpNumber, lPumpMode, lFlags, pVal);


#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::SetPumpMode")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetPumpMode", LOG_LEVEL_0);	
	}
#endif

	return S_OK;
}

void CGCI::SetPumpMode__(long lPumpNumber, long lPumpMode, long lFlags, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	if(retCode == GCI_OK)
	{
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		//		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if((CONTROLLER_IS_MODE_MASTER) || (PUMPSRV_IS_MODE_MASTER2))  //4.0.5.46
		{			
			CPumpStatus stat = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();
			stat.SetMode(lPumpMode);
			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetStat(stat);
			_Module.m_server.SavePumpInfo(lPumpNumber);

			if (lFlags & SET_PUMP_MANUAL_MODE)
			{
				CString str ("Pumps\\Pump");
				char sNumber[8] = {0};
				sprintf_s(sNumber,3,"%02d",lPumpNumber);
				str += sNumber;
			
				_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "ManualMode",lPumpMode,FALSE,_HOST.m_lGroupNum,0,lPumpNumber);//4.0.1.38
				_Module.m_server.m_lPumpsManualModeArray[lPumpNumber-1] = lPumpMode;

			}
		}
		else
		{
			retCode = GCI_NOT_SUPPORTED;
		}

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
//		LeaveCriticalSection(&_Module.m_server.m_csPumpArray);

	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"SetPumpMode",
			TRUE,retCode,
			FALSE,0);

	
	if(pVal != NULL)
		*pVal = retCode;
}

//4.0.5.24
STDMETHODIMP CGCI::GetCardInfo3(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, long lFlags, long *lSesionType , long lPumpNumber ,BSTR *sRetData, long *pVal)
{	
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif		

		GetCardInfo3__(sAccountNumber, sTrack1, sTrack2, sShortCardName, lFlags, lSesionType, lPumpNumber,sRetData, pVal);

#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetCardInfo3")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetCardInfo3", LOG_LEVEL_0);	
	}
#endif

	return S_OK;
}

void CGCI::GetCardInfo3__(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, long lFlags, long *lSesionType, long lPumpNumber, BSTR *sRetData, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString sMsg;
		sMsg.Format("Insert GetCardInfo3 with flags =%ld" ,lFlags);
		_LOGMSG.LogMsg(sMsg);
	}

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	else if((_Module.m_server.IsServerInRecoveryProcess() || (!_Module.m_server.m_cOLASrv->Valid())) 
		&& _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
	{
		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
	}

	else if (_Module.m_server.CheckOLA() !=  OLA_SERVER_ON)	
	{	
		retCode = GCI_OLA_NOT_ACTIVE;
	}

	
	if(retCode == GCI_OK)
	{
		long lRtc = 0;
		//4.0.9.506 amram
		//1.	Define new flag value for 'AllCardsData'
		//2.	When RFS send 'AllCardsData' flag,
		//      send OLA in 'ShortCardName' parameter the string 'AllCardsData'
		//      (Track2 contains the index)
		if(lFlags & GET_ALL_CARDS_DATA)
		{
			CComBSTR sTmp = "AllCardsData";
			SysFreeString(sShortCardName);
			sShortCardName = sTmp;
		}

		lRtc = _Module.m_server.GetCardInfo(sAccountNumber, sTrack1, sTrack2, sShortCardName, sRetData,lFlags, lSesionType, lPumpNumber);
		
	}

	_LOGMSG.LogMsg("Exit GetCardInfo3");		
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"GetCardInfo",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}


/******************************************************************************
 Description:	Sets connectivity status comes from CL  	
 Returns:		GCI return codes.      	
 Parameters:	BSTR sPSConnStatus - struct CL_TAG_CONNECTIVITY_INFO    	
				long lFlags - for future use.	
		
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			23/2/2004   15:00		Start
******************************************************************************/
STDMETHODIMP CGCI::SetConnectivityStatus(BSTR sPSConnStatus, long lFlags, long *pVal)
{
	#ifdef CATCH_GCI
	__try
	{
#endif		
	SetConnectivityStatus__(sPSConnStatus, lFlags, pVal);
#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::SetConnectivityStatus")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetConnectivityStatus", LOG_LEVEL_0);	
	}
#endif

	return S_OK;
}

void CGCI::SetConnectivityStatus__(BSTR sPSConnStatus, long lFlags, long *pVal)
{
	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;
	long lConnStatusRecSize = (lFlags & STRACT_IS_CONNECTIVITY_INFO2)?sizeof(CL_TAG_CONNECTIVITY_INFO2):sizeof(CL_TAG_CONNECTIVITY_INFO);

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	if(retCode == GCI_OK)
	{
		CL_TAG_CONNECTIVITY_INFO2   cCLConnInfo2;
		memset(&cCLConnInfo2,0,sizeof(cCLConnInfo2));
		
		int iLen = Bstr2Buffer(sPSConnStatus, (void *)&cCLConnInfo2,lConnStatusRecSize);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;
		}
		else
		{			
			_Module.m_server.m_cCompConnectivity.SetCLCompConnectivity(&cCLConnInfo2,lFlags);	
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"SetConnectivityStatus",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);
	
	if(pVal != NULL)
		*pVal = retCode;
}
//4.0.5.41
STDMETHODIMP CGCI::GetPAKQuery(BSTR sRestrictionData, BSTR *sOutData, long *plRecCount, long lFlags, long *plRetVal)
{
	// TODO: Add your implementation code here
	#ifdef CATCH_GCI
	__try
	{
#endif		
		GetPAKQuery__(sRestrictionData, sOutData, plRecCount, lFlags,plRetVal);
#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetPAKQuery")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetPAKQuery", LOG_LEVEL_0);	
	}
#endif
	
	return S_OK;
}
/******************************************************************************
 Description:	Query on transaction. 	
 Returns:      	None
 Parameters:   	BSTR sRestrictionData - buffer contain the restriction data. 
				BSTR *sOutData		- buffer with record data   
				long *plRecCount	- Counter of record found 
				long lFlags			- Query type
				long *plRetVal		- ret code
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			27/5/2004   14:48		Start
******************************************************************************/
void CGCI::GetPAKQuery__(BSTR sRestrictionData, BSTR *sOutData, long *plRecCount, long lFlags, long *plRetVal)
{
	GetPAKQueryRetCode eRetCode;
	GCIRetCode retCode = GCI_OK;

	eRetCode = _Module.m_server.m_cCommonFunction.GetPAKQuery(sOutData , plRecCount ,&lFlags ,sRestrictionData);

	switch(eRetCode)
	{
		case GetPakQuery_OK:  
			retCode = GCI_OK;
			break;
		case GetPakQuery_MissingData:
			retCode = GCI_LAST_ERROR;
			break;
	}

	if(LOG_LOGIC_ERROR && retCode)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"GetPAKQuery",
			TRUE,retCode,
			FALSE,0);

	if(plRetVal != NULL)
		*plRetVal = retCode;


}



/******************************************************************************
 Description:	Getting a pump's mode - exposed function
 Returns:      	
 Parameters:    long lPumpNumber, long *plPumpMode, long *plFlags, long *plMsgNumber, long *plVal

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			30/05/2004   15:33		Start
******************************************************************************/

STDMETHODIMP CGCI::GetPumpMode(long lPumpNumber, long *plPumpMode, long *plFlags, long *plMsgNumber, long *plVal)
{
	#ifdef CATCH_GCI
		__try
		{
	#endif		
		GetPumpMode__(lPumpNumber, plPumpMode, plFlags, plMsgNumber, plVal);


	#ifdef CATCH_GCI
		}

		__except(_HANDLE_EXCEPTION("GCI::GetPumpMode")) //4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("GCI::GetPumpMode", LOG_LEVEL_0);	
		}
	#endif

	return S_OK;
}

/******************************************************************************
 Description:	Getting a pump's mode  - inner function
 Returns:      	
 Parameters:    long lPumpNumber, long *plPumpMode, long *plFlags, long *plMsgNumber, long *plVal

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			30/05/2004   15:34		Start
******************************************************************************/

void CGCI::GetPumpMode__ (long lPumpNumber, long *plPumpMode, long *plFlags, long *plMsgNumber, long *plVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

//	RFUEL-2795 - Sang this validation was setting error retCode, preventing
//		system from returning the data
//	if (MODE_NOT_VALID(*plPumpMode))
//		retCode = GCI_INVALID_PUMP_MODE;
//

	if(retCode == GCI_OK)
	{
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		//		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		CPumpStatus stat = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();
		
		//Loading the mode's attributes 
		if(plPumpMode)
		   *plPumpMode = stat.GetMode(); 

		CMode *pMode = &(_Module.m_server.m_cModeArray[*plPumpMode - 1]);

		if(plFlags)
			*plFlags = pMode->GetFlags(); 
		if(plMsgNumber )
			*plMsgNumber = pMode->GetMsgNumber();
		
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
//		LeaveCriticalSection(&_Module.m_server.m_csPumpArray);
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"GetPumpMode",
			TRUE,retCode,
			FALSE,0);

	
	if(plVal != NULL)
		*plVal = retCode;
}


STDMETHODIMP CGCI::ParseOLABuffer(long lPumpNumber, BSTR *cCardSaleALL3, BSTR sOLABuffer, long lFlags, long *pRet)
{
	// TODO: Add your implementation code here
	#ifdef CATCH_GCI
		__try
		{
	#endif		
			ParseOLABuffer__(lPumpNumber, cCardSaleALL3,  sOLABuffer, lFlags, pRet);

	#ifdef CATCH_GCI
		}

		__except(_HANDLE_EXCEPTION("GCI::ParseOLABuffer")) //4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("GCI::ParseOLABuffer", LOG_LEVEL_0);	
		}
	#endif

	return S_OK;
}
/******************************************************************************
 Description:	Convert open OLA buffer to struct CARD_SALE_ALL3.	
 Returns:      	None
 Parameters:   	long lPumpNumber	- PumpNumber - 
				BSTR *cCardSaleALL3 - Return OLA struct CARD_SALE_ALL3.
				BSTR sOLABuffer		- OLA string, 
				long lFlags			- Future use.
				long *pRet
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			13/6/2004   14:12		Start
******************************************************************************/

void CGCI::ParseOLABuffer__(long lPumpNumber, BSTR *cCardSaleALL3, BSTR sOLABuffer, long lFlags, long *pRet)
{	
	GCIRetCode lRetCode = GCI_OK;
	CString sMsg;
	BYTE sTmpCardSaleData[sizeof(CARD_SALE_ALL3) +2 ]={0};
	CARD_SALE_ALL3 cTmpCardSaleAll3;
	
		
	long lPumpIndex = lPumpNumber - 1;
	
	PAY_AT_PUMP_INFO    cPayAtPumpInfo;  //4.0.5.0
	

	memset(&cPayAtPumpInfo , ' ' , sizeof(cPayAtPumpInfo));

	CString  sMyOLABuffer = sOLABuffer;
	
	//Verify parameter validity
	if(PUMPSRV_NOT_INIT)
		lRetCode = GCI_MUST_INIT_FIRST;
	
	else if(PUMP_NOT_VALID(lPumpNumber))
		lRetCode = GCI_INVALID_PUMP_NUMBER;

	else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
		lRetCode = GCI_SERVER_IN_MIDDLE_LOCK_SHIFT;   //4.0.1.40


	if ( (cCardSaleALL3 != NULL) && (lFlags & POB_APPEND_DATA))
	{
		CARD_SALE_ALL3 cardDataAll3;
		memset(&cardDataAll3,0,sizeof(CARD_SALE_ALL3));
		int iLen = Bstr2Buffer(*cCardSaleALL3, (void *)&cardDataAll3,sizeof(CARD_SALE_ALL3));

		if(iLen == 0)
		{
			DWORD dwLastErr = GetLastError();	

			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString sMsg;
				sMsg.Format("ParseOLABuffer  fail to extract in card sale data PumpNumber=%ld , iLen=%d, error=%ld" , lPumpNumber,iLen,dwLastErr);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);

			}
	
		}
		else
		{
			// use card sale data from the cilent,
			cPayAtPumpInfo.CardSaleInfo.cardData = cardDataAll3.CardSaleAll.data;
			cPayAtPumpInfo.CardSaleInfo.extraData = cardDataAll3.CardSaleAll.extData;
			cPayAtPumpInfo.CardSaleExtraData2 = cardDataAll3.extData2;
			cPayAtPumpInfo.CardSaleExtraData3= cardDataAll3.extData3;
		}
	}

	if(lRetCode == GCI_OK)
	{
		_Module.m_server.m_cOLASrv->ParseData(0,&sMyOLABuffer,&cPayAtPumpInfo,NULL,0,lPumpNumber); //4.0.16.503
	}
	
	
	// copy all part to buffer out			
	memset(sTmpCardSaleData , 0 , sizeof(sTmpCardSaleData));
	cTmpCardSaleAll3.CardSaleAll.data = cPayAtPumpInfo.CardSaleInfo.cardData;
	cTmpCardSaleAll3.CardSaleAll.extData  = cPayAtPumpInfo.CardSaleInfo.extraData;
	cTmpCardSaleAll3.extData2 = cPayAtPumpInfo.CardSaleExtraData2;
	cTmpCardSaleAll3.extData3 = cPayAtPumpInfo.CardSaleExtraData3;
	

	memcpy(sTmpCardSaleData , &cTmpCardSaleAll3, sizeof(CARD_SALE_ALL3));

	CComBSTR bstrOutBuffer(sizeof(sTmpCardSaleData) , (LPCSTR)sTmpCardSaleData);
	*cCardSaleALL3 = bstrOutBuffer.Copy();

	if(pRet)
		*pRet = (long)lRetCode;

	//Write standard failure message to log in case of error
	if(lRetCode)
	{
		_LOGMSG.LogResult(1,                 //Source
			                                 "GCI",             //Interface name
											 "ParseOLABuffer",    //Method name
											 TRUE,              //Log return value
											 lRetCode,          //Return value
											 FALSE,             //Log COM result
											 0,                 //COM result
											 NULL);             //COM exception
	}

	return;

}

STDMETHODIMP CGCI::SetVersion(BSTR sVersionList, long lFlags ,long *pVal)
{
	// TODO: Add your implementation code here
	#ifdef CATCH_GCI
		__try
		{
	#endif		
			SetVersion__(sVersionList,lFlags, pVal);
	#ifdef CATCH_GCI
		}

		__except(_HANDLE_EXCEPTION("GCI::SetVersion")) //4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("GCI::SetVersion", LOG_LEVEL_0);	
		}
	#endif

	return S_OK;
}
// 4.0.5.45
void CGCI::SetVersion__(BSTR sVersionList, long lFlags, long *pVal)
{
	GCIRetCode RetCode = GCI_OK;

	if (sVersionList)
	{		

		//4.0.14.501
		if (lFlags & SET_VERSION_BY_XML)
		{

			_Module.m_server.m_sDevicesVersions = sVersionList;

			_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_GET_VERSIONS,0);
			
			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString sMsg;
				sMsg.Format("SetVersion by Xml"); 
				_LOGMSG.LogMsg(sMsg);
			}
		}
		else
		{
			_Module.m_server.sSysVersion.Format("PumpSrv=%s", _Module.m_server.m_strPumpSrvVersion);	
			_Module.m_server.sSysVersion.Remove(' ');
			_Module.m_server.sSysVersion.Replace(',' ,'.');
			_Module.m_server.sSysVersion += ',';

			_Module.m_server.sSysVersion += sVersionList;

			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString sMsg;
				sMsg.Format("CGCI::SetVersion  %s",_Module.m_server.sSysVersion); 
				_LOGMSG.LogMsg(sMsg);
			}

		}

	}
	else
		RetCode = GCI_PARAM_NOT_FOUND;
			

	if (pVal != NULL)
		*pVal = RetCode;

	if(RetCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"SetVersion",
			TRUE,RetCode,
			FALSE,0);
	
}



STDMETHODIMP CGCI::GetStationStatus(long *lPumpMap1, long *lPumpMap2, long lFlags, long *pRetVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		GetStationStatus__(lPumpMap1 , lPumpMap2 , lFlags , pRetVal);
#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetStationSatus")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetStationSatus()", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}

//4.0.6.506
void CGCI::GetStationStatus__(long *lPumpMap1, long *lPumpMap2, long lFlags, long *pRetVal)
{
	GCIRetCode lRetCode = GCI_OK;


	if (SERVER_STATE_STARTUP)
		lRetCode = GCI_CONTROLLER_NOT_AVAILABLE;
	
	else if (SERVER_NOT_IDLE)
		lRetCode = GCI_SERVER_NOT_READY;
	
		//4.0.9.506
	*lPumpMap1 = *lPumpMap2 = 0;
	
	//No errors so far - continue.
	if(lRetCode == GCI_OK)
	{
		_Module.m_server.m_cCommonFunction.GetStationStatus(lPumpMap1, lPumpMap2, lFlags, pRetVal);		
	}

	
	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString sMsg;
		sMsg.Format("CGCI::GetStationStatus() pump bit maps - 1 to 32 %x , 33 to 64 %x , returned %d", 
			        *lPumpMap1,
					*lPumpMap2,
					lRetCode);
		
		_LOGMSG.LogMsg(sMsg);
	}	

	if(pRetVal)
		*pRetVal = lRetCode;
}

STDMETHODIMP CGCI::UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		UploadBuffer__(lTerminalId, lPumpNumber, lSessionId, lUploadType, sInBuffer, sOutBuffer, lExDataSize, sExData, pVal);
#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::UploadBuffer")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::UploadBuffer()", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}

void CGCI::UploadBuffer__(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData, long *pVal)
{
	GCIRetCode retCode = GCI_OK;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if(PUMP_NOT_VALID(lPumpNumber))
		 retCode = GCI_INVALID_PUMP_NUMBER;

	_LOGMSG.LogMsg("Insert UploadBuffer");	

	if (_Module.m_server.CheckOLA() !=  OLA_SERVER_ON)	
	{	
		retCode = GCI_OLA_NOT_ACTIVE;
	}
	
	if(retCode == GCI_OK)
	{
		long lRtc = 0;

		lRtc = _Module.m_server.UploadBuffer(lTerminalId, lPumpNumber, lSessionId, lUploadType, sInBuffer, sOutBuffer, lExDataSize, sExData);

		if (lRtc)
			retCode = GCI_LAST_ERROR;
		
	}

	_LOGMSG.LogMsg("Exit UploadBuffer");		
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"UploadBuffer",
				TRUE,retCode,
				FALSE,0);
  
	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::GetWetStockData(long lWetStockFlags, long *pVal)
{
	// TODO: Add your implementation code here	
#ifdef CATCH_GCI
	__try
	{
#endif
		GetWetStockData__(lWetStockFlags, pVal);
#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetWetStockData")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::UploadBuffer()", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}

void CGCI::GetWetStockData__(long lWetStockFlags, long *pVal)
{
	GCIRetCode retCode = GCI_OK;
	WetStockInternalRetCode InternalRetCode = WetStockInternal_OK;

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;
		str.Format("CGCI::GetWetStockDatal Flags=%ld" , lWetStockFlags);
		_LOGMSG.LogMsg(str);
	}
	
	switch(_Module.m_server.m_cCommonFunction.GetWetStockData__(lWetStockFlags, pVal))
	{
	case WetStockInternal_OK:
			retCode = GCI_OK;
		break;

	case WetStockInternal_IN_MIDDLE_OF_PROCESS:
			retCode = GCI_IN_MIDDLE_OF_PROCESS;
		break;
	default:
		_LOGMSG.LogMsg("GCI::GetWetStockData return  value out of range");
	}


	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"GetWetStockData",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::GetTankMemoryData(long lTankNumber, BSTR *pRec, long lFlags, long *lCounter, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		GetTankMemoryData__(lTankNumber, pRec, lFlags, lCounter, pVal);
#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetTankMemoryData")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetTankMemoryData()", LOG_LEVEL_0);
	}
#endif


	return S_OK;
}

void CGCI::GetTankMemoryData__(long lTankNumber, BSTR *pRec, long lFlags, long *lCounter, long *pVal)
{
	GCIRetCode retCode= GCI_OK;

	if (TANK_NOT_VALID(lTankNumber) && lTankNumber != READ_ALL_TANKS)
		retCode = GCI_INVALID_TANK_NUMBER;
	else if (lCounter == NULL) 
		retCode = GCI_INVALID_VALUES;


	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;
		str.Format("CGCI::GetTankMemoryData lTankNumber=%ld, lFlags=%ld" , lTankNumber , lFlags);
		_LOGMSG.LogMsg(str);
	}

	
	if(retCode == GCI_OK)
	{	
		GPIRetCode gpiRetCode= GPI_OK;
		_Module.m_server.m_cCommonFunction.GetTankMemoInfo(lTankNumber,pRec, lFlags, (long *)&gpiRetCode, lCounter , TANK_MEMO_WITH_DELIVERY);
		switch(gpiRetCode)
		{
			case GPI_INVALID_TANK_NUMBER:
			{
				retCode = GCI_INVALID_TANK_NUMBER;
				break;
			}
			case GPI_RECORD_NOT_FOUND:
			{
				retCode = GCI_RECORD_NOT_FOUND;
				break;
			}


		}

		
	}
	else
	{
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"GetTankInfo",
				TRUE,retCode,
				FALSE,0);
	}
	
	if(pVal != NULL)
		*pVal = retCode;	

	if ((LOG_BASIC_FLOW_CONTROL) && (retCode == GCI_OK))
	{	
		CString str;
		str.Format("CGCI::GetTankMemoryData retrieved %ld elements  " , *lCounter);
		_LOGMSG.LogMsg(str);
	}


}
/******************************************************************************
 Description:	Skip on waiting for pump total details 	
 Returns:      	
 Parameters:   	lPumpNumber - Pump number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			5/1/2005   10:01		Start
******************************************************************************/

void CGCI::ReleasePumpTotalPump(long lPumpNumber)
{
	if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite()) 
	{
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if((_Module.m_server.m_cPumpArray[lPumpNumber-1].GetLockStatus() == WAIT_RESPONSE)||
		   (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetLockStatus() == LOCKED) )
		{
			long lMisc = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetMisc();
			lMisc &= MISC_NOT_WAIT_LOCK_TIMEOUT;
			lMisc &= MISC_NOT_WAIT_RESPONSE_TIMEOUT;

			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetLockStatus(RESPONSE_RECEIVED,lMisc);
			_Module.m_server.SavePumpInfo(lPumpNumber);

		}

		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}
}

STDMETHODIMP CGCI::GetParam4(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long lFlags, long lDataBase, long *pVal)
{
	
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
		if( ((sVal == NULL) && (nFiledType == FIELD_SZ)) || 
			((lVal == NULL) && (nFiledType == FIELD_DWORD)) )
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg(
				"IGCI::GetParam4   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	
			return E_POINTER;
		}
#endif
		GetParam4__(sParamPosition, lKey1, lKey2, nFiledType, sVal, lVal, bCreate, lFlags, lDataBase, pVal);
#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetParam4")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetParam4()", LOG_LEVEL_0);
	}
#endif


	return S_OK;
}

void CGCI::GetParam4__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long lFlags, long lDataBase, long *pVal)
{
	GetParam3__(sParamPosition, lKey1, lKey2, nFiledType, sVal, lVal, bCreate,  pVal, lDataBase);
}

STDMETHODIMP CGCI::SetParam4(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long lFlags, long lDataBase, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
		if( ((sVal == NULL) && (nFiledType == FIELD_SZ)) || 
			((lVal == NULL) && (nFiledType == FIELD_DWORD)) )
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg(
				"IGCI::SetParam4   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	
			return E_POINTER;
		}
#endif
		SetParam4__(sParamPosition, lKey1, lKey2,  nFiledType, sVal, lVal, bCreate, lFlags, lDataBase, pVal);
#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::SetParam4")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetParam4()", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}

void CGCI::SetParam4__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long lFlags, long lDataBase, long *pVal)
{	
	SetParam3__(sParamPosition, lKey1, lKey2,  nFiledType, sVal, lVal, bCreate,  pVal ,lDataBase);
}

STDMETHODIMP CGCI::GetAttTrs(long lPumpNumber, long lFlags, BSTR sIndex, BSTR *sRec, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		GetAttTrs__(lPumpNumber, lFlags, sIndex, sRec, pVal);		
#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetAttTrs")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::UploadBuffer()", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}
/******************************************************************************
 Description:	Retrives transaction data for Att according to Att Id or 
				Att card.	
 Returns:      	GCI rcodes: GCI_OK - Att Trs was found for this Att.
							GCI_LAST_ERROR - Index data is wrong.
							GCI_WRONG_ATT - Att Trs was found for other Att. 
							GCI_TRS_NOT_FOUND - no Att Trs at all on this pump.						
 Parameters:	long lPumpNumber   	
				long lFlags: INDEX_IS_ATTID		0x0001
							 INDEX_IS_TRACK2	0x0002
				BSTR sIndex: AttId or Att card Track2.
				BSTR *sRec : return data - struct TAG_ATT_TRS.
				long *pVal - GCI return codes.			
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			10/3/2005   15:15		Start	
 Amram			23/06/2005	14:25       
******************************************************************************/

void CGCI::GetAttTrs__(long lPumpNumber, long lFlags, BSTR sIndex, BSTR *sRec, long *pVal)
{
	GCIRetCode retCode = GCI_OK;
	DWORD dwLastErr = 0;
	int iLen = 0;
	int iTrs = 0;
	long lNumTrs  = 0;
	BOOL bFound = FALSE;
	long lVersion = 0;
	EXTRA_INFO_REC sExtraInfoRec;
	CPumpTransact cTmpTrs;

	char szAttId[MAX_TABLE_NAME];
	memset(szAttId,' ',sizeof(szAttId));

	CComBSTR bstrTransactions;
	bstrTransactions.Empty();


	TAG_ATT_TRS_EX cTagAttTrs;
	memset(&cTagAttTrs , ' ' , sizeof(cTagAttTrs));
	
	char sTmpStr[sizeof(TAG_ATT_TRS_EX) + 2];
	memset((BYTE*)sTmpStr + sizeof(TAG_ATT_TRS_EX) , 0 , 2);

	lVersion = 2;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if(PUMP_NOT_VALID(lPumpNumber))
		 retCode = GCI_INVALID_PUMP_NUMBER;
	
	if(retCode == GCI_OK)
	{
		if (sIndex)
		{ 
			iLen = WideCharToMultiByte(CP_ACP,0,sIndex,-1,szAttId,sizeof(szAttId),NULL,NULL);
		}
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = GCI_LAST_ERROR;
		}

		if(retCode == GCI_OK)
		{
			long lRtc = 0;
			
//			EnterCriticalSection(&_Module.m_server.m_csPumpArray);

			lNumTrs = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetTrsArraySize();

			for(;iTrs < lNumTrs;iTrs++)
			{
				_Module.m_server.m_cPumpArray[lPumpNumber-1].GetTrsByIndex(iTrs , &cTmpTrs);

				if((cTmpTrs.m_lRes == PAY_AT_KIOSK) && 
				   (cTmpTrs.m_lResExt == RES_EXT_ATTENDANT) && (!cTmpTrs.m_lOnOffer) )
				{					
					sExtraInfoRec.info.lIndexNumber = cTmpTrs.m_lNumber; 
					lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE,REC_READ, &sExtraInfoRec);

					//Verify that we found the correct attendant details 
					if ( lRtc == OK)
					{
						if( ((lFlags & INDEX_IS_ATTID) && (!memcmp(sExtraInfoRec.info.sAttendantID ,szAttId , sizeof(sExtraInfoRec.info.sAttendantID)))) ||
							((lFlags & INDEX_IS_TRACK2) && (!memcmp(sExtraInfoRec.info.sTrack2Data ,szAttId , sizeof(sExtraInfoRec.info.sTrack2Data))))  )
						{
							cTmpTrs.GetTag(&cTagAttTrs.pumpTrs , lVersion);
						}
						else //the Att transaction belong to another Att
						{//4.0.20.501 TD 60236		
							char szResult[MAX_FIELD_VAL] = {0};		
							if(lFlags & INDEX_IS_ATTID)
								_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, (char*)sExtraInfoRec.info.sAttendantID, sizeof(sExtraInfoRec.info.sAttendantID));
							else
								_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, (char*)sExtraInfoRec.info.sTrack2Data, sizeof(sExtraInfoRec.info.sTrack2Data));

							char szAttIdResult[MAX_FIELD_VAL] = {0};		
							_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szAttIdResult, szAttId, sizeof(szAttId));
							
							CString sMsg;
							sMsg.Format("CGCI::GetAttTrs() No Trs for AttId:%s ,Trs found for Att:%.15s:AttId:%s", szAttIdResult, sExtraInfoRec.info.sAttendantName, szResult);
						
							_LOGMSG.LogMsg(sMsg);
							retCode = GCI_WRONG_ATT;				
						}
						
						//looking only for one Att transaction
						bFound = TRUE;
						memcpy(&cTagAttTrs.sAttendantName,sExtraInfoRec.info.sAttendantName,sizeof(cTagAttTrs.sAttendantName));
						memcpy((BYTE*)sTmpStr , &cTagAttTrs , sizeof(cTagAttTrs));

						l2a(sExtraInfoRec.info.lPreSetlimit, cTagAttTrs.sPreSetlimit, sizeof(cTagAttTrs.sPreSetlimit)); //4.0.12.32
						cTagAttTrs.sPreSetlimitType = sExtraInfoRec.info.sPreSetLimitType;//4.0.12.32

						bstrTransactions.Append((LPCSTR)sTmpStr);
				
						break;
					}
				}
			}

//			LeaveCriticalSection(&_Module.m_server.m_csPumpArray);
		}		
	}

	if(sRec)
		*sRec = bstrTransactions.Copy();
	
	if(bFound == FALSE)
	{
		CString sMsg;
		sMsg.Format("CGCI::GetAttTrs() No Trs for Att:%.10s",szAttId);
		_LOGMSG.LogMsg(sMsg);
		retCode = GCI_TRS_NOT_FOUND;
	}
		

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"GetAttTrs",
			TRUE,retCode,
			(dwLastErr!=0)? TRUE:FALSE,dwLastErr);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CGCI::PriceChangeUpdate(long lPumpNumber, long lPosition, long lFlags, long *plVal)
{
	// TODO: Add your implementation code here
	// TODO: Add your implementation code here
#ifdef CATCH_GCI
	__try
	{
#endif
		PriceChangeUpdate__( lPumpNumber,  lPosition, lFlags, plVal);
#ifdef CATCH_GCI
	}

	__except(_HANDLE_EXCEPTION("GCI::PriceChangeUpdate")) //4.0.15.460
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::PriceChangeUpdate()", LOG_LEVEL_0);
	}
#endif

	return S_OK;

}


/******************************************************************************
 Description:	Set the tank level map according to the status
				
 Returns:      	
 Parameter   : lPumpNumber - if 0 then update all pumps
							 else update only the pump number 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			22/06/2005  14:28		Start
******************************************************************************/
STDMETHODIMP CGCI::SetGradesRestriction(BSTR sTagPumpsNuzzlesRestrict, long lFlags, long *pVal)
{

#ifdef CATCH_GCI
	__try
	{
#endif
		SetGradesRestriction__(sTagPumpsNuzzlesRestrict,lFlags,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::SetPumpStatus")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetPumpStatus()", LOG_LEVEL_0);

		exit(1);
	}	
#endif

	return S_OK;

}



void CGCI::SetGradesRestriction__(BSTR sTagPumpsNuzzlesRestrict, long lFlags, long *pVal)
{
	GCIRetCode retCode = GCI_OK;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	_Module.m_server.m_cTankLevelMap.SetStatusPerTankWithPump(sTagPumpsNuzzlesRestrict,lFlags);
	if(pVal != NULL)
		*pVal = retCode;

}



/******************************************************************************
 Description:	Get list of trans according to the flags
				if PumpNumber =  0 then return trans to all pump
				plRecCount - Hold the max trans that we an return
				if we had less we will update him with the number of trans
				
 Returns:      	
 Parameter   : lPumpNumber - if 0 then update all pumps
							 else update only the pump number 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			22/06/2005  14:28		Start
******************************************************************************/
STDMETHODIMP CGCI::GetTransListQuery(long lPumpNumber, BSTR *sData, long *plRecCount, long *lFlags, long *pVal)
{
#ifdef CATCH_GPI
	__try
	{
#endif
		GetTransListQuery__(lPumpNumber, sData, plRecCount, lFlags,pVal);
#ifdef CATCH_GPI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetTransListQuery")) //4.0.15.460
	{
		_LOGMSG.LogMsg("CGCI::GetTransListQuery", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}



void CGCI::GetTransListQuery__(long lPumpNumber,BSTR *sData, long *plRecCount, long *lFlags, long *pVal)
{
	GCIRetCode retCode = GCI_OK;
	long lRetVal = GCI_OK;
	
	//_LOGMSG.LogMsg("CGCI::GetTransListQuery__ Insert Method");

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	else if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;


	_Module.m_server.m_cCommonFunction.GetTransListQuery(lPumpNumber,sData,plRecCount,lFlags);

	if(LOG_LOGIC_ERROR && lRetVal)
		_LOGMSG.LogResult(
			0, // Src
			"CGCI",
			"GetTransListQuery",
			TRUE,lRetVal,
			FALSE,0);

	if(pVal != NULL)
		*pVal = lRetVal;

}



void CGCI::PriceChangeUpdate__(long lPumpNumber, long lPosition, long lFlags, long *plVal)
{
	GCIRetCode retCode = GCI_OK;

	if(PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;
	else
	{			
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if(LOG_BASIC_FLOW_CONTROL)
		{	
			CString str;
			str.Format("Send ACK on price change , details position=%ld, flags=%x", lPosition , lFlags);
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
		}
		
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPriceChangeStatus(lPosition , FALSE);

		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}	

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"PriceChangeUpdate__",
			TRUE,retCode,
			0,0);

	if(plVal != NULL)
		*plVal = retCode;
}

STDMETHODIMP CGCI::GetCashAcceptorReport(long lTerminalId, long lFlags, BSTR *sHeader, BSTR *sRecords, long *lRecordCount, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		GetCashAcceptorReport__(lTerminalId, lFlags,sHeader, sRecords, lRecordCount, pVal);
#ifdef CATCH_GPI
	}

	__except(_HANDLE_EXCEPTION("GCI::GetCashAcceptorReport")) //4.0.15.460
	{
		_LOGMSG.LogMsg("CGCI::GetCashAcceptorReport", LOG_LEVEL_0);
	}
#endif


	return S_OK;
}

void CGCI::GetCashAcceptorReport__(long lTerminalId, long lFlags, BSTR *sHeader, BSTR *sRecords, long *lRecordCount, long *pVal)
{
	// TODO: Add your implementation code here
	GCIRetCode retCode = GCI_OK;
	long lRetVal = 0;

	 if (!_Module.m_server.IsCashAcceptorreportEnable()) //4.0.11.11
		retCode = GCI_CASH_ACCEPTOR_REPORT_NOT_ACTIVE;

	if(retCode == GCI_OK)
	{
		if ((lTerminalId >= 0 ) && (lTerminalId  <= MAX_CASH_ACCEPTOR_RECORD_SHIFT))
		{
			lRetVal = _Module.m_server.GetCashAcceptorReportXml__(lTerminalId, lFlags, sHeader, sRecords, lRecordCount, TRUE); //4.0.17.501 - TD 35765


			if ((lRetVal == GCI_OK) && (!(*lRecordCount)))
				retCode = GCI_CASH_ACCEPTOR_REPORT_NOT_FOUND;
			else
				retCode = (GCIRetCode)lRetVal; //4.0.11.115
		}  
		else
			retCode = GCI_INVALID_TERMINAL_NUMBER;
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;
		str.Format("Terminal %02ld  Requested report lFlags %ld record count %ld ,retcode =%ld", lTerminalId, lFlags, *lRecordCount , retCode);
		_LOGMSG.LogMsg(str);
	}


	if(LOG_LOGIC_ERROR && retCode)
		_LOGMSG.LogResult(
			0, // Src
			"CGCI",
			"GetCashAcceptorReport",
			TRUE,retCode,
			FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
	
}

STDMETHODIMP CGCI::SetCashAcceptorStatus(long lTerminalId, long lFlags, long lStatus, BSTR *sRec, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_GPI
	__try
	{
#endif
		SetCashAcceptorStatus__(lTerminalId, lFlags,  lStatus, sRec, pVal);
#ifdef CATCH_GPI
	}

	__except(_HANDLE_EXCEPTION("GCI::SetCashAcceptorStatus")) //4.0.15.460
	{
		_LOGMSG.LogMsg("CGCI::SetCashAcceptorStatus", LOG_LEVEL_0);
	}
#endif

	return S_OK;
}

/******************************************************************************
 Description: Update Events from cash acceptor
				
 Returns:      	
 Parameter   : lTerminalId  - IPT number
			   lStatus - IPT status
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			22/06/2005  14:28		Start
******************************************************************************/

void CGCI::SetCashAcceptorStatus__(long lTerminalId, long lFlags, long lStatus, BSTR *sRec, long *pVal)
{
	GCIRetCode retCode = GCI_OK;
	//long	lRetVal = 0;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if (!_Module.m_server.IsCashAcceptorreportEnable())
		retCode = GCI_CASH_ACCEPTOR_REPORT_NOT_ACTIVE;

	if(retCode == GCI_OK)
	{
		//4.0.11.138
		if ((lTerminalId >= 0 ) && (lTerminalId  <= MAX_CASH_ACCEPTOR_RECORD_SHIFT))
		{
			if (lStatus == SET_CASH_ACCEPTOR_REMOVAL_DRAWER)
			{
				_Module.m_server.m_cCashAcceptorFile.MarkRemovalOnterminal(lTerminalId);
			}
			else if (lStatus == SET_CASH_ACCEPTOR_REPORT_READY )
			{
				_Module.m_server.MoveCashAcceptorShift();
				_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_CASH_ACCEPTOR_REPORT_IS_READY,0);				
			}
		}
		else
			retCode = GCI_INVALID_TERMINAL_NUMBER;
	//4.0.11.138
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;
		str.Format("Terminal %02ld set cash acceptor status lFlags %ld status %ld ,retcode =%ld", lTerminalId, lFlags, lStatus , retCode);
		_LOGMSG.LogMsg(str);
	}

	
	if(LOG_LOGIC_ERROR && retCode)
		_LOGMSG.LogResult(
			0, // Src
			"CGCI",
			"GetCashAcceptorReport",
			TRUE,retCode,
			FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;

}



/******************************************************************************
 Description: New Method Agip Track3
				
 Returns:      	
 Parameter   :  
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			12/10/2005  14:28		Start
******************************************************************************/

STDMETHODIMP CGCI::GetCardInfoEx(long lClientId, BSTR sInBuffer, BSTR sInfoType, long lFlags, BSTR *sRetData, long *pVal)
{

#ifdef CATCH_GCI
	__try
	{
#endif
		GetCardInfoEx__(lClientId, sInBuffer, sInfoType, lFlags, sRetData, pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::GetCardInfoEx")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::GetCardInfoEx()", LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;

}


void CGCI::GetCardInfoEx__(long lClientId, BSTR sInBuffer, BSTR sInfoType, long lFlags, BSTR *sRetData, long *pVal)
{
	GCIRetCode retCode = GCI_OK;	

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString sMsg;
		sMsg.Format("Insert GetCardInfoEx with flags =%ld" ,lFlags);
		_LOGMSG.LogMsg(sMsg);
	}

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	//4.0.13.501
	else if((_Module.m_server.IsServerInRecoveryProcess() || (!_Module.m_server.m_cOLASrv->Valid())) 
		&& _Module.m_server.m_bSupportControllerInMiddleRecoveryProcess)
	{
		retCode = GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS;
	}

	else if (_Module.m_server.CheckOLA() !=  OLA_SERVER_ON)	
	{	
		retCode = GCI_OLA_NOT_ACTIVE;
	}
	
	if(retCode == GCI_OK)
	{
		long lRtc = 0;

		EnterCriticalSection( &_Module.m_server.m_csGetCardInfoEx ); //4.0.19.0 
		
		retCode = (GCIRetCode )_Module.m_server.GetCardInfoEx(lClientId, sInBuffer, sInfoType, lFlags, sRetData, pVal);
		
		LeaveCriticalSection( &_Module.m_server.m_csGetCardInfoEx ); //4.0.19.0 
	}

	_LOGMSG.LogMsg("Exit GetCardInfoEx__");		
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"IGCI",
				"GetCardInfoEx__",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}

//4.0.13.501
void CGCI::CheckIfPumpNeedToBeLock(long lPumpNumber)
{

	CTime cCurTime = CTime::GetCurrentTime();
	CTimeSpan ts;

	CPumpStatus stat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();

	if(_Module.m_server.m_cPumpArray[lPumpNumber - 1].AcceptedTransactions())
	{			
		CPumpTransact trs;
		int iArraySize = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsArraySize();
		
		for (int i=0; i<iArraySize; i++)
		{
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTrsByIndex (i,&trs);

			//Check if we reach UnbalancedPrPTimer, then update the trs.
			if(trs.GetStatus() == ACCEPTED)
			{
				//check if it is a PrePay trs and the timer is not 0.
				if ((trs.GetRes() == PRE_PAY_AT_POS) && (_Module.m_server.GetUnbalancedPrPTimer() > 0))
				{

					ts = cCurTime - trs.GetTime();				
					if(cCurTime > trs.GetTime() && ts.GetTotalSeconds() >= _Module.m_server.GetUnbalancedPrPTimer())
					{

						CMD_STRUCT_TRS cTRSCmd;

						//Set the trs to PAID
						cTRSCmd.lTrsNumber = trs.m_lNumber;

						_Module.m_server.SetCommand(CMD_TRS_PAID,
						SERVER_OLA_POS_NUMBER,
						lPumpNumber,
						&cTRSCmd,
						sizeof(cTRSCmd));


						if(LOG_DETAIL_FLOW_CONTROL)
						{
							CString sLogMsg;
							sLogMsg.Format("SetStatePaid -  Send cmd to controller to clear PrePay trs.  (Pump %02d, Trs %d, Status %d, link Number %d", 
										   lPumpNumber, 
										   trs.m_lNumber,
										   trs.m_lStatus,
										   trs.m_lLinkNumber );

							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogMsg); 
						}	

						//save the trs to Paid QDX
						_Module.m_server.InsertToPaidQDX(trs,trs.GetOnOffer());
					}
					else
					{
						//Lock the pump
						CMD_STRUCT_RESERVE_PUMP cmdReserveSinglePump;
						memset(&cmdReserveSinglePump, 0, sizeof(CMD_STRUCT_RESERVE_PUMP));

						//Filling the command of reserving a pump and sending it
						cmdReserveSinglePump.lRes	= PUMPSRV;
						cmdReserveSinglePump.lResExt |= PUMP_SINGLE_LOCK;

						_Module.m_server.SetCommand(CMD_RESERVE_PUMP, SERVER_POS_NUMBER, lPumpNumber, &cmdReserveSinglePump, sizeof(CMD_STRUCT_RESERVE_PUMP)); 	

						if (LOG_BASIC_FLOW_CONTROL)
						{
							CString sLogStr;
							sLogStr.Format("Sent a command to reserve pump %d\n", lPumpNumber);
							_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sLogStr); 				
						}

						//Changing the state to PUMP_WAIT_TO_BE_EXCLUSIVELY_LOCKED in the state machine
						_Module.m_server.m_cSinglePumpLock.SetPumpStateWaitLocked(lPumpNumber);
					}

				}

			}
		}
	}

}




STDMETHODIMP CGCI::TankReadingEx(long lTankNumber, long lFlags, BSTR sRec, long *pVal)
{
#ifdef CATCH_GCI
	__try
	{
#endif

		TankReadingEx__(lTankNumber,lFlags,sRec,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::TankReadingEx")) //4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::TankReadingEx()", LOG_LEVEL_0);

	}
#endif

	return S_OK;
}

//4.0.13.502
void CGCI::TankReadingEx__(long lTankNumber, long lFlags, BSTR sRec, long *pVal)
{
	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();
	GCIRetCode retCode = GCI_OK;
	BOOL bDeliveryFailed = FALSE;	

	TANK_READ_INFO info;
	memset(&info,0,sizeof(TANK_READ_INFO));

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
		
	if(retCode == GCI_OK)
	{

		_Module.m_server.m_cTankXML.Xml2TankReadingInfo(&sRec, &info);


		if(!(lFlags & TANK_READ_NOT_AVAILABLE))
		{
			if (TANK_NOT_VALID(info.lTank))
				retCode = GCI_INVALID_TANK_NUMBER;
		}

		if(retCode == GCI_OK)
		{
			if(lFlags & TANK_READ_NOT_AVAILABLE) 
			{
				CString str;
				// not available...

				if(info.lTank == 0)//4.0.2.15 All tanks
				{					
					str.Format("Tank Reading not available for All Tanks, Flags=%ld, memo=%d, command flags=0x%08x !", _Module.m_server.m_cServerInfo.GetWetStockFlags() ,_Module.m_server.m_cServerInfo.GetMemoProcessFlags()  , lFlags); //4.0.7.503 //4.0.8.500
					//If we're in the middle of a delivery process we notify the State Machine
					//of a failure, and the bDeliveryFailed variable was declared for that purpose
					bDeliveryFailed = TRUE;

					//4.0.15.120 - CR 44000
					//Set tank status to not available for all tanks
					for (long lTank=0; lTank < MAX_TANKS; lTank++)
						_Module.m_server.m_cTankArray[lTank].SetTankStatus(TANK_READ_NOT_AVAILABLE_FOR_ALL);


					if (!(lFlags & TANK_READ_END_OF_LIST))  //4.0.8.500
						lFlags +=TANK_READ_END_OF_LIST;

					if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite())
					{
						if(!_Module.m_server.m_bTankReadingRcved)
							_Module.m_server.m_bTankReadingRcved = TRUE;
					}
					//4.0.20.40
					if (_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS )
					{
						if (_Module.m_server.m_cServerInfo.GetMemoProcessFlags())
						{
								
						}
						
						//4.0.22.512 - TD 75378
						if (!(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_REPORT))
							_Module.m_server.m_cServerInfo.ClearTankReadingFlags();
					}
				}
				else
				{
					str.Format("Tank Reading not avaliable for Tank %d, Flags=%ld,memo=%d, command flags=0x%08x!",info.lTank , _Module.m_server.m_cServerInfo.GetWetStockFlags() ,_Module.m_server.m_cServerInfo.GetMemoProcessFlags() ,lFlags);

					// 4.0.15.120 - CR 44000
					_Module.m_server.m_cTankArray[info.lTank-1].SetTankStatus(TANK_READ_NOT_AVAILABLE);
				}

				if(LOG_BASIC_FLOW_CONTROL)  //4.0.8.500
				{							
					_LOGMSG.LogMsg(str);
				}	

			}
			else
			{		
				BOOL bSaveRcvedInfo = FALSE;
			


				//4.0.3.39
				if(LOG_TRS_FILE)
					_Module.m_server.m_cTrsLog.LogTankTotals(info.lTank,info.lProduct,info.lFuelVolume);

				//3.2.0.34
				if(CONTROLLER_IS_PUMP_LOCK_MASTER  || _Module.m_server.IsMeggitSite())
				{
					bSaveRcvedInfo = TRUE;
				}
				else if (!_Module.m_server.m_bTankReadingRcved) 
				{
					bSaveRcvedInfo = TRUE;
				}
				else if(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS )
				{
					bSaveRcvedInfo = TRUE;
				}

				if(bSaveRcvedInfo)
				{
					DWORD dwTmp =0;

					info.lIndexNumber	= _Module.m_server.NewTankReadIndex();
					info.nFlags			= (short)lFlags;
					if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS )
						info.nFlags |= TANK_READ_ON_DEMAND_REQUEST;

					_Module.m_server.m_cParam.LoadParam("Tanks", "VolumeMultiply",&dwTmp, (DWORD)1, FALSE);	

					// attach current active read number 
					info.dwActiveReadNumber = _Module.m_server.GetActiveReadNumber();

					// attach current shift number 
					info.dwShiftNumber	= _Module.m_server.GetFormalShiftNumber();

					// indication if we are in middle of shift process
					//4.0.23.500 - TD 77765
					info.byEndOfShiftStatus = '0'; 
					if (_Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags()) 
						info.byEndOfShiftStatus = '1';

					//4.0.25.50 - TD 108311
					if (_Module.m_server.GetSingleShiftAccumulationsActive())
					{
						SINGLE_SHIFT_ACCUM_TANK_RECORD cSingleShiftAccumTankRec;
						if (!_Module.m_server.m_cSingleShiftAccumMgr.GetTankRecord(info.lTank, cSingleShiftAccumTankRec))
						{
							info.lShiftAccumulatedDeliveryVolume = cSingleShiftAccumTankRec.lShiftAccumulatedDeliveryVolume;
							info.lShiftAccumulatedSales = cSingleShiftAccumTankRec.lShiftAccumulatedSales;
						}
					}
					//CR_475695_Start
					CString str;				
					str.Format("TankReadingEX__  info.nFlags:0X%08x",info.nFlags);
					_Module.m_server.m_cLogger.LogMsg(str);

					SetTankReadingRequestType(&info.nFlags); //4.0.24.1010 
					//CR_475695_End

					GetLocalTime(&info.cSysTime);

					int lRtc = 0;
					lRtc = SaveTankReading(&info); //443035

					if(lRtc)
						retCode = GCI_FAILED_IO;

					//4.0.1.32 start
					//Write to shift Q-Dex only if in middle of shift
					if(_Module.m_server.IsShiftChangeInProgress() && _Module.m_server.GetWetstockMaintFlag())
					{
						CShiftTanks cShiftTanks;

						lRtc = cShiftTanks.Write(info);

						if(lRtc)
							_LOGMSG.LogMsg("Failed to write to EOD qdx");
					}
					//4.0.1.32 end
					
					//**************************************************************************
					//Handle the PumpSrv delivery emulation process (does nothing if not active)
					//**************************************************************************
					if (_Module.m_server.GetSupportDeliveryEmulation() == TRUE)
					{
					    HandleDeliveryEmulationSingleTankProcess(info.lTank, info.lFuelVolume, info.lTemperature, info.lWaterVolume, info.nFlags, info.lDensity, info.lFuelWeight);//4.0.21.500 TD 66935 //4.0.28.500 - TD 250092
						HandleDeliveryEmulationProcess(info.lTank, info.lFuelVolume, info.lTemperature, info.lWaterVolume, info.nFlags); //4.0.19.151	//4.0.19.150 (TD 62818)
					}				
				}
					//add by Yaron
				else
				{
					// M.L 16.7.98
					if(LOG_LOGIC_ERROR)
						_LOGMSG.LogMsg("Unexpected tank reading , record ignored  !!! ", LOG_LEVEL_1);

				}
			

			}

			if (info.lStatus2 == TANK_READ_NOT_AVAILABLE) //443035
			{
				GetTankReadingTimeFromINIFile(&info);
			}

			if (_Module.m_server.GetMemoInterval() > 0 && info.lTank >= 1) //relocated for 443035 
			{
				_Module.m_server.m_cTankArray[info.lTank - 1].SetTankInfo(
					info.lProduct, info.lStatus, lFlags, info.lFuelLevel, info.lWaterLevel, info.lTemperature,
					info.lFuelVolume, info.lWaterVolume, info.lUllage, info.lTcVolume, info.lStatus2, info.cSysTime);
				_Module.m_server.m_cTankArray[info.lTank - 1].SetTankValidity(true);
			}

			if (lFlags & TANK_READ_END_OF_LIST) //4.0.8.500
			{	
				// 4.0.9.555
				if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_POS )
				{
					if (!(_Module.m_server.dwPosRequestReport & REPORT_TYPE_ULLAGE))
						_Module.m_server.dwPosRequestReport  += REPORT_TYPE_ULLAGE; 

				}
				//4.0.1.38 - sending the event only if writing into the qdex.
				else if(_Module.m_server.m_cServerInfo.GetMemoProcessFlags() == FALSE)
					_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_TANK_READING,0);//Add by Yaron for Ctrl Events
				else if (!(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_POS_PUMP)) //4.0.16.500
				{
					_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_MEMO_TANK_READING,0);
					_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,
									0,
									GCI_EVENT_MEMO_TANK_READING );		
				}

				//4.0.22.501 - TD 69905
				_Module.m_server.m_cServerInfo.HandleGenerateReportProcess(FALSE, lFlags);

				if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS )
				{
					_Module.m_server.m_cServerInfo.ClearTankReadingFlags();

					SQLPumpSrvFile.SaveServerInfo();
				}
				
				//4.0.5.24 Added by Udi for Delivery
				//In case the tanks were read for the purpose of delivery, we
				//instantiate the delivery flags with the appropriate values, and
				//notify the State Machine the process has succeeded 
				
				int nErrorRetrieved = 0;
				_Module.m_server.SetDeliveryFlags(DES_UNLOCK, PD_NONE, nErrorRetrieved, TRUE);
			
				if (info.lStatus == TANK_STATUS_IDLE)
				{
					// update flag if needed //3.2.0.34
					if(PUMPSRV_IS_PUMP_LOCK_MASTER  || _Module.m_server.IsMeggitSite())
					{
						if(!_Module.m_server.m_bTankReadingRcved)
							_Module.m_server.m_bTankReadingRcved = TRUE;
					}
				}
				
			} // end of list

		}
	}


	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"TankReadingEx",
			TRUE,retCode,
			FALSE,0);



	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;

		str.Format("Tank readingEx(lTankNumber = %ld, lProduct = %ld, nFlags = 0x%08x, lStatus1 = %ld, lStatus2 = %ld, lFuelLevel = %ld, lWaterLevel = %ld, lTemperature = %ld, lVolume = %ld, lWaterVolume = %ld, lDensity = %ld, lFuelWeight = %ld, lTankOnline = %ld, lPricePerLitre = %ld)",
			info.lTank,info.lProduct,lFlags,info.lStatus,info.lStatus2,info.lFuelLevel,info.lWaterLevel,info.lTemperature,info.lFuelVolume,info.lWaterVolume,info.lDensity,info.lFuelWeight,info.lTankOnline,info.lPricePerLitre);

		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}


	int nErrorRetrieved = 0;

	if(bDeliveryFailed)
	{
		_Module.m_server.SetDeliveryFlags(DES_UNLOCK, PD_NONE, nErrorRetrieved, FALSE);
	}
	
	if(pVal != NULL)
		*pVal = retCode;
}


//4.0.13.504
STDMETHODIMP CGCI::DeliveryReportEx(long lTankNumber, long lFlags, BSTR sRec, long *pVal)
{

	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif

		DeliveryReportEx__(lTankNumber,lFlags,sRec,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::DeliveryReportEx")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::DeliveryReportEx()", LOG_LEVEL_0);	

		exit(1);
	}
#endif


	return S_OK;
}

void CGCI::DeliveryReportEx__(long lTankNumber, long lFlags, BSTR sRec, long *pVal)
{

	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;
	BOOL bUpdateDataBase = FALSE;

	DELIVERY_INFO info;
	memset(&info,0,sizeof(DELIVERY_INFO));

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	
	else if(!(lFlags & DELIVERY_REPORT_NOT_AVAILABLE))
	{
		if(TANK_NOT_VALID(lTankNumber) && lTankNumber != 0) //4.0.12.517
			retCode = GCI_INVALID_TANK_NUMBER;
	}

	if(retCode == GCI_OK)
	{
		bUpdateDataBase = FALSE;

		if(lFlags & DELIVERY_REPORT_NOT_AVAILABLE)
		{
			// not available...
			CString str;			
			//4.0.2.161
			//All tanks
			if(lTankNumber == 0)
			{
				//4.0.8.500 
				str.Format("Delivery report not available for all tanks , Wet stock flags=%ld , command flag=%d" , _Module.m_server.m_cServerInfo.GetWetStockFlags() , lFlags);
				if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite() || (lFlags & DELIVERY_FORCE_ACCEPT_DATA) )  //4.0.6.502)
				{
					if(!_Module.m_server.m_bDeliveryReportRcved)
						_Module.m_server.m_bDeliveryReportRcved = TRUE;				
				}
				if (!(lFlags & DELIVERY_REPORT_END_OF_LIST))
					lFlags += DELIVERY_REPORT_END_OF_LIST;
			}
			else
				str.Format("Delivery report not available for Tank %d! ,Wet stock flags=%ld, command flag=%d",lTankNumber , _Module.m_server.m_cServerInfo.GetWetStockFlags(), lFlags); //4.0.8.500

			if(LOG_BASIC_FLOW_CONTROL)
			{		
				_LOGMSG.LogMsg(str);
			}	
			
		}
		else
		{
			BOOL bSaveRcvedInfo = FALSE;

			//3.2.0.34
			if(CONTROLLER_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite())
			{
				bSaveRcvedInfo = TRUE;
			}
			else if (lFlags & DELIVERY_FORCE_ACCEPT_DATA)  //4.0.6.507
			{
				bSaveRcvedInfo = TRUE;
			}			
			else if (!_Module.m_server.m_bDeliveryReportRcved) 
			{
				bSaveRcvedInfo = TRUE;
			}
			else if(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY )//4.0.1.38
			{
				bSaveRcvedInfo = TRUE;
			}
			else if(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_MOST_RECENT_DELIVERY ) //4.0.28.503 TD 268744
			{
				bSaveRcvedInfo = TRUE;
			}
			//4.0.12.512
			else if (lFlags & DELIVERY_REPORT_INVOICE_DATA)
			{
				bSaveRcvedInfo = TRUE;
			}			

			if(bSaveRcvedInfo)
			{

				_Module.m_server.m_cTankXML.Xml2DeliveryInfo(&sRec, &info);

				info.lIndexNumber	= _Module.m_server.NewFuelDeliveryIndex();
				info.nFlags 		= (short)lFlags;
				GetLocalTime(&info.cQdxDateTime); //4.0.23.500 - TD 72606

				if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY )
					info.nFlags |= DELIVERY_REPORT_ON_DEMAND_REQUEST;

				info.dwActiveReadNumber = _Module.m_server.GetActiveReadNumber();
				info.dwShiftNumber = _Module.m_server.GetFormalShiftNumber();

				//4.0.23.500 - TD 77765
				info.byEndOfShiftStatus = '0'; 
				if (_Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags()) 
					info.byEndOfShiftStatus = '1';

				//4.0.25.50 - TD 108311
				if (_Module.m_server.GetSingleShiftAccumulationsActive())
					_Module.m_server.m_cSingleShiftAccumMgr.AccumulateDeliveryVolume(lTankNumber, info.lDeliveredVolume);

	
				if ((retCode == GCI_OK) && (_Module.m_server.m_cServerInfo.GetMemoProcessFlags() == FALSE))
				{
					long lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_INSERT,&info);
					if(lRtc)
					{
						retCode = GCI_FAILED_IO;
					}

					if(_Module.m_server.IsShiftChangeInProgress() && _Module.m_server.GetWetstockMaintFlag())
					{
						CShiftDelivery cShiftDelivery;
						cShiftDelivery.Write(info);
					}
				}

				if(_Module.m_server.m_cServerInfo.GetMemoProcessFlags()) //4.0.8.500
				{					
					_Module.m_server.m_cTankArray[lTankNumber-1].SetDeliveryInfo(info.cEndDeliveryDateTime ,info.lDeliveredVolume , info.lStatus); //4.0.8.507
					_Module.m_server.m_cTankArray[lTankNumber-1].SetTankValidity(true);

					if(LOG_BASIC_FLOW_CONTROL)
					{		
						CString str;
						
						str.Format("Delivery information is update to Tank=%ld, Volume=%ld lStatus=%ld",info.lTank , info.lDeliveredVolume , info.lStatus);
						_LOGMSG.LogMsg(str);
					}	

					
				}
				//end add by Yaron


			}
			else
			{
				// M.L 16.7.98
				if(LOG_LOGIC_ERROR)
					_LOGMSG.LogMsg("Unexpected Delivery report , record ignored  !!! ", LOG_LEVEL_1);
			}
			
		}

		// 4.0.8.500
		if(lFlags & DELIVERY_REPORT_END_OF_LIST)
		{
			bUpdateDataBase = FALSE;
			
			if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_POS )  //4.0.9.555
			{
				if (!(_Module.m_server.dwPosRequestReport & REPORT_TYPE_DERIVERY))
					_Module.m_server.dwPosRequestReport  += REPORT_TYPE_DERIVERY; 

			}
			//4.0.7.501
			else if (_Module.m_server.m_cServerInfo.GetMemoProcessFlags())
			{
				_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_MEMO_DELIVERY_REPORT,0);
				_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,
								0,
								GCI_EVENT_MEMO_DELIVERY_REPORT );		

			}
			else
				//Add by Yaron for Ctrl Events
				_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_DELIVERY_REPORT,0);

			//3.2.0.34
			if(PUMPSRV_IS_PUMP_LOCK_MASTER || _Module.m_server.IsMeggitSite())
			{
				// update flag if needed			
				if(!_Module.m_server.m_bDeliveryReportRcved)
					_Module.m_server.m_bDeliveryReportRcved = TRUE;				
			}

			//4.0.28.502 - TD 258931 
			if (_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_MOST_RECENT_DELIVERY )
			{
				_Module.m_server.m_cServerInfo.SetWetStockFlags(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_CLEAR_MOST_RECENT_DELIVERY);
				//bUpdateDataBase = TRUE;
			}

			//4.0.7.501
			if (_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY )
			{
				_Module.m_server.m_cServerInfo.SetWetStockFlags(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_CLEAR_DELIVERY);

				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString strLog;
					strLog.Format("CGCI::DeliveryReportEx : Set Server Info. Delivery Status: %d, Delivery Process: %d.", 
						_Module.m_server.m_cServerInfo.GetDeliveryEmulationStatus(),
						_Module.m_server.m_cServerInfo.GetDeliveryEmulationProcessType());
						
					_LOGMSG.LogMsg(strLog);
				}
								
				bUpdateDataBase = TRUE;
			}

			if (!( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY ) && 
				!( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS ))

				{
					_Module.m_server.m_cServerInfo.SetMemoProcessFlags(FALSE);
					bUpdateDataBase = TRUE;
				}

			if (bUpdateDataBase)
			{
				_Module.m_server.ClearWetStockProcess(_Module.m_server.m_cServerInfo.GetWetStockFlags());												
			}
		} // end of list

	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"IGCI",
			"DeliveryReportEx",
			TRUE,retCode,
			(dwLastErr!=0)? TRUE:FALSE,dwLastErr);



	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("Get delivery report ex(lTankNumber = %ld, Flags %d, Status %ld,Starting Volume %ld, End Volume %ld, Product %ld, Delivered volume %ld, Temperature %ld)",
		info.lTank,info.nFlags,info.lStatus,info.lStartVolume,info.lEndVolume,info.lProduct,info.lDeliveredVolume,info.lTemperature); 
		_LOGMSG.LogMsgWithRet(0,LOG_NONE,str,"IGCI",retCode);
	}

	if(pVal != NULL)
		*pVal = retCode;

}




//48194
STDMETHODIMP CGCI::ReadyOfflineTRS(long lPumpNumber, long lFlags, BSTR sData, BSTR sParam1, long lParam2, long lParam3, BSTR *sRetData, long *pVal)
{
	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif

		ReadyOfflineTRS__(lPumpNumber, lFlags, sData, sParam1, lParam2, lParam3, sRetData,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::ReadyOfflineTRS")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("GCI","ReadyOfflineTRS()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}		
#endif

	return S_OK;
}

/******************************************************************************
 Description:	SetPumpStatus2 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella b			21/5/2002   17:05		Start
******************************************************************************/
//4.0.19.500 - TD 59171
STDMETHODIMP CGCI::SetPumpStatus2(long lPumpNumber, long lStatus, long lRes, long lResExt, long lFlags, short nGrade, short nPosition, BSTR sData, long *pVal)
{
	
#ifdef CATCH_GCI
	__try
	{
#endif
		SetPumpStatus__(lPumpNumber,lStatus,lRes,lResExt,pVal, lFlags, nGrade, nPosition, sData);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::SetPumpStatus2")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetPumpStatus2()", LOG_LEVEL_0);

		exit(1);
	}	
#endif

	return S_OK;
}

/******************************************************************************
 Description:	Set the misc from Conver layer (directly to PosPump).
 Returns:      	
 Parameters:   	long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			06/06/2008   17:05		Start
******************************************************************************/
STDMETHODIMP CGCI::SetPumpMisc(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal)
{
#ifdef CATCH_GCI
	__try
	{
#endif
		SetPumpMisc__(lPumpNumber, lMisc, bRemoveMisc, pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::SetPumpMisc")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetPumpMisc", LOG_LEVEL_0);

		exit(1);
	}	
#endif

	return S_OK;
}
 
void CGCI::SetPumpMisc__(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal)
{
	_Module.m_server.m_cCommonFunction.SetPumpMisc(lPumpNumber,lMisc,bRemoveMisc,pVal);
}

void CGCI::SetPumpMiscExt__(long lPumpNumber, long lMiscExt, long bRemoveMisc, long *pVal)	//4.0.20.451 65268 //4.0.21.501
{
	_Module.m_server.m_cCommonFunction.SetPumpMisc(lPumpNumber,lMiscExt,bRemoveMisc,pVal, TRUE);
}

/******************************************************************************
 Description: This method handles the new PumpSrv delivery emulation Single Tank process.
			  It collects the information from the tank reading records arrived from convert layer,
			  and calculates delivery records, if found.			
 Returns:      	
 Parameter:  

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Shelly B		23/10/2008  15:40		Start //4.0.21.500 TD 66935
Alex M          01/09/2013						4.0.25.1060  TD 377336
******************************************************************************/
void CGCI::HandleDeliveryEmulationSingleTankProcess(const long lTankNumber, const long lVolume, const long lTemperature, const long lWaterVolume, const short nFlags, long lDensity, long lWeight, long lProductLevel, long lWaterLevel) 
{
	if (_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankStatus(lTankNumber) == DES_SINGLE_TANK_WAIT_TANK_READING)
	{
		long		lTankReference                 = _Module.m_server.m_cTankArray[lTankNumber-1].GetDeliveryReference(); 
	    BOOL		bLockConnectedPumps            = _Module.m_server.m_cTankArray[lTankNumber-1].GetLockConnectedPumps();
		BYTE		byDeliveryEmulationProcessType = _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessType(lTankNumber);
		int			nErrorRetrieved = 0;
		SYSTEMTIME	cCurrTime;

		memset(&cCurrTime, 0, sizeof(SYSTEMTIME));
		GetLocalTime(&cCurrTime);

		switch(byDeliveryEmulationProcessType)
		{					
			case PD_SINGLE_TANK_START:
			{
				//save StartTime both to memory and ini file (for recovery): //4.0.21.500 - TD 69374
				_Module.m_server.m_cTankArray[lTankNumber-1].SetStartDeliveryEmulationTime(&cCurrTime);
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveStartDeliveryEmulationTime();
				
				//save volume both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetStartDeliveryEmulationVolume(lVolume);
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveStartDeliveryEmulationVolume();
				
				//save Start temperature both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetStartDeliveryEmulationTankTemperature(lTemperature);
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveStartDeliveryEmulationTankTemperature();

				//save Start water volume both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetStartDeliveryEmulationWaterVolume(lWaterVolume);
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveStartDeliveryEmulationWaterVolume();

				//save Start Density both to memory :
				_Module.m_server.m_cTankArray[lTankNumber-1].SetStartDeliveryEmulationDensity(lDensity);	//4.0.28.500 - TD 250092
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveStartDeliveryEmulationDensity();           //4.0.25.380 - TD 287958

				//save Start Weight both to memory :
				_Module.m_server.m_cTankArray[lTankNumber-1].SetStartDeliveryEmulationWeight(lWeight);		//4.0.28.500 - TD 250092
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveStartDeliveryEmulationWeight();			//4.0.25.380 - TD 287958

				// 4.0.25.1060  TD 377336 //save End ProductLevel both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetStartDeliveryEmulationProductLevel(lProductLevel);    
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveStartDeliveryEmulationProductLevel();          
				
				_Module.m_server.m_cTankArray[lTankNumber-1].SetStartDeliveryEmulationWaterLevel(lWaterLevel);    
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveStartDeliveryEmulationWaterLevel();          
			}
			break;

			case PD_SINGLE_TANK_END:
			{
				//save EndTime both to memory and ini file (for recovery): //4.0.21.500 - TD 69374
				_Module.m_server.m_cTankArray[lTankNumber-1].SetEndDeliveryEmulationTime(&cCurrTime);					
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveEndDeliveryEmulationTime();

				//save volume both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetEndDeliveryEmulationVolume(lVolume);					
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveEndDeliveryEmulationVolume();

				//save End temperature both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetEndDeliveryEmulationTankTemperature(lTemperature);
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveEndDeliveryEmulationTankTemperature();

				//save End water volume both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetEndDeliveryEmulationWaterVolume(lWaterVolume);
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveEndDeliveryEmulationWaterVolume();
								
				//save End Density both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetEndDeliveryEmulationDensity(lDensity);	//4.0.28.500 - TD 250092 // TD 253789 4.0.25.300
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveEndDeliveryEmulationDensity();         //4.0.25.380 - TD 287958

				//save End Weight both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetEndDeliveryEmulationWeight(lWeight);	//4.0.28.500 - TD 250092 // TD 253789 4.0.25.300
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveEndDeliveryEmulationWeight();          //4.0.25.380 - TD 287958

				// 4.0.25.1060  TD 377336//save End ProductLevel both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetEndDeliveryEmulationProductLevel(lProductLevel);    
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveEndDeliveryEmulationProductLevel();          
				
				_Module.m_server.m_cTankArray[lTankNumber-1].SetEndDeliveryEmulationWaterLevel(lWaterLevel);    
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveEndDeliveryEmulationWaterLevel();          
				
				long lDeliveryVolume;

				//Calculates Delivery:
				lDeliveryVolume = _Module.m_server.m_cTankArray[lTankNumber-1].CalculateDeliveryEmulation();
				if(lDeliveryVolume != 0)
				{
					//save data to delivery QDX (in case a figure found) for VP.
					_Module.m_server.m_cTankArray[lTankNumber-1].SaveDeliveryEmulationInfoInDB( nFlags, lDeliveryVolume, "SINGLE");
                    _Module.m_server.m_cTankArray[lTankNumber-1].SetFoundDelivery(TRUE);
					_Module.m_server.m_cTankArray[lTankNumber-1].SetVPNotification(TRUE);
				}

			}
			break;
		}
    
		_Module.m_server.m_cServerInfo.SetDeliveryEmulationSingleTankSuccess(lTankNumber, DESF_SINGLE_TANK_DONE);
		_Module.m_server.SetDeliverySingleTankFlags(lTankNumber, lTankReference, bLockConnectedPumps, DES_SINGLE_TANK_UNLOCK, byDeliveryEmulationProcessType, nErrorRetrieved);
		//Need to check if DES_SINGLE_TANK_UNLOCK should be sent really here, or only after EOL (and also success) *?*? 
	}
}

/******************************************************************************
 Description: New private Method. 
			  This method handles the new PumpSrv delivery emulation process.
			  It collects the information from the tank reading records arrived from convert layer,
			  and calculates delivery records , if found.
				
 Returns:      	
 Parameter   :  
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008  14:00		Merge	4.0.20.52 
 Amit H			30/05/2008  11:00		Continue - save also the start temperature
******************************************************************************/
//private
void CGCI::HandleDeliveryEmulationProcess(long lTankNumber, long lVolume, long lTemprature, long lWaterVolume, short nFlags)
{
	if (_Module.m_server.m_cServerInfo.GetDeliveryEmulationStatus() == DES_WAIT_TANK_READING)
	{
		BYTE	byDeliveryEmulationProcessType;
		byDeliveryEmulationProcessType = _Module.m_server.m_cServerInfo.GetDeliveryEmulationProcessType();

		switch(byDeliveryEmulationProcessType)
		{					
			case PD_START:
			{
				//save volume both to memory and ini file (for recovery)
				_Module.m_server.m_cTankArray[lTankNumber-1].SetStartDeliveryEmulationVolume(lVolume);
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveStartDeliveryEmulationVolume();
				
				//save Start temperature
				_Module.m_server.m_cTankArray[lTankNumber-1].SetStartDeliveryEmulationTankTemperature(lTemprature); //4.0.19.151
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveStartDeliveryEmulationTankTemperature();

				//save Start water volume both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetStartDeliveryEmulationWaterVolume(lWaterVolume); // 4.0.19.1430 TD 298855
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveStartDeliveryEmulationWaterVolume(); // 4.0.19.1430 TD 298855
			}
			break;

			case PD_END:
			{
				//save volume and end temperature both to memory and ini file (for recovery)
				_Module.m_server.m_cTankArray[lTankNumber-1].SetEndDeliveryEmulationVolume(lVolume);					
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveEndDeliveryEmulationVolume();

				//save End temperature
				_Module.m_server.m_cTankArray[lTankNumber-1].SetEndDeliveryEmulationTankTemperature(lTemprature); //4.0.19.151
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveEndDeliveryEmulationTankTemperature();

				//save End water volume both to memory and ini file (for recovery):
				_Module.m_server.m_cTankArray[lTankNumber-1].SetEndDeliveryEmulationWaterVolume(lWaterVolume); // 4.0.19.1430 TD 298855
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveEndDeliveryEmulationWaterVolume(); // 4.0.19.1430 TD 298855

				long lDeliveryVolume;

				//Calculates deliver figure.
				lDeliveryVolume = _Module.m_server.m_cTankArray[lTankNumber-1].CalculateDeliveryEmulation();
				if(lDeliveryVolume != 0) //4.0.21.500 - TD 65586
				{
					//save data to delivery QDX (in case a figure found) for VP.
					_Module.m_server.m_cTankArray[lTankNumber-1].SaveDeliveryEmulationInfoInDB( nFlags, lDeliveryVolume, "GENERAL");
					_Module.m_server.SetFoundDelivery(TRUE);
				}

                //ResetDeliveryEmulationInfo - resets only info which is saved in the QDX, 
				//and therefore can be reseted after writing to QDX:
				_Module.m_server.m_cTankArray[lTankNumber-1].ResetDeliveryEmulationInfo();
				_Module.m_server.m_cTankArray[lTankNumber-1].SaveDeliveryEmulationInfo();

			}
			break;
		}
	}
}

/******************************************************************************
 Description: New private Method. 
			  This method handles the new PumpSrv delivery emulation process.
			  It gets a transaction volume by pump and nozzle,
			  translate it to tank number,and saves the data both in memory and in ini file (for recovery)
				
 Returns:      	
 Parameter   :  
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008  14:00		Start	Merge	4.0.20.52
 Shelly         20/11/2008  15:52       Add a support for a single tank process //4.0.21.500 TD 66935
 ******************************************************************************/
void	CGCI::SaveTrsVolumeForDeliveryEmulation(long lPumpNumber, short nNozzle, long lVolume)
{
	if (_Module.m_server.GetSupportDeliveryEmulation() == TRUE)
	{
		BYTE byTank;
	    long lProcessedTank = 0;
	    BOOL SingleTankProcess = FALSE;

		byTank = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetTankPerPosition(nNozzle);

		//4.0.28.502 - TD 69518
		if (_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessType(byTank) != PD_SINGLE_TANK_NONE)
			SingleTankProcess = TRUE;

		//remove 4.0.28.502 - TD 69518
// 		if (_Module.m_server.m_cCommonFunction.IsDeliveryEmulationSingleTankInProcess(lProcessedTank))
// 			if (lProcessedTank == byTank)
// 				SingleTankProcess = TRUE;

		if ((_Module.m_server.m_cServerInfo.GetDeliveryEmulationProcessType() != PD_NONE) || 
			((SingleTankProcess) && (_Module.m_server.m_cTankArray[byTank-1].CanIncDeliveryEmulationSales() == TRUE))) //4.0.19.1260 - TD 242013
		{
			//save to memory:
			_Module.m_server.m_cTankArray[byTank-1].IncAccumulatedDeliveryEmulationSales(lVolume);
		    //save to ini: 
		    _Module.m_server.m_cTankArray[byTank-1].SaveAccumulatedDeliveryEmulationSales();
		}

	}
}


STDMETHODIMP CGCI::SetPumpMiscExt(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal) //4.0.20.451 65268 //4.0.21.501
{
#ifdef CATCH_GCI
	__try
	{
#endif
		SetPumpMiscExt__(lPumpNumber, lMisc, bRemoveMisc, pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::SetPumpMiscExt")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::SetPumpMiscExt", LOG_LEVEL_0);

		exit(1);
	}	
#endif

	return S_OK;
}

STDMETHODIMP CGCI::ReportReady(long lFlags, BSTR wszReportText, long lParam, long *pVal)
{
	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif

		ReportReady__(lFlags,wszReportText,lParam,pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::ReportReady")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::ReportReady()", LOG_LEVEL_0);	

		exit(1);
	}
#endif


	return S_OK;
}

/******************************************************************************
 Description:	CL informs PumpSrv that the wet stock report is ready
				Asynchronous process of requesting report
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		05/02/2009   18:00			Start		4.0.22.501 TD 69905
******************************************************************************/
void CGCI::ReportReady__(long lFlags, BSTR wszReportText, long lParam, long *pVal)
{
	GCIRetCode retCode = GCI_OK;

	_LOGMSG.LogMsg("CGCI::ReportReady__");
	if(LOG_BASIC_FLOW_CONTROL)
	{
		//Print report length to log
		char sXmlReport[MAX_XML_BUF_LEN] = {0};
		int iLen = Bstr2Buffer(wszReportText, (char *)sXmlReport, MAX_XML_BUF_LEN);
		if (iLen)
		{
			CString str;
			str.Format("GCI::ReportReady, Report buffer len = %ld", iLen);
			_LOGMSG.LogMsg(str);
		}		
	}	

	// Set Report text to memory			
	_Module.m_server.m_cServerInfo.SetReportBuffer(wszReportText);
	
	//Move to report ready state
	_Module.m_server.m_cServerInfo.HandleGenerateReportProcess(FALSE, 0, TRUE);


	if(pVal != NULL)
		*pVal = retCode;
}

/******************************************************************************
 Description:	CL updates PumpSrv about the car wash request result
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat		12/10/2009   09:05			Start		4.0.23.504 76432
******************************************************************************/

STDMETHODIMP CGCI::CarWashGeneralReqResponse(long lDevice, long lResult, long lFlags1, long lFlags2, BSTR sData, long *pVal)
{
	USES_CONVERSION;

#ifdef CATCH_GCI
	__try
	{
#endif

		CarWashGeneralReqResponse__(lDevice, lResult, lFlags1, lFlags2, sData, pVal);

#ifdef CATCH_GCI
	}
	__except(_HANDLE_EXCEPTION("GCI::CarWashGeneralReqResponse")) //4.0.15.450
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("GCI::CarWashGeneralReqResponse()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

	return S_OK;
}

void CGCI::CarWashGeneralReqResponse__(long lDevice, long lResult, long lFlags1, long lFlags2, BSTR sData, long *pVal) //4.0.23.504 76432
{
	if ((lFlags1 == _Module.m_server.m_cCarWashGeneralRequest.GetCurrentCarWashRequestFlags()) && _Module.m_server.m_cCarWashGeneralRequest.RequestWasSent(0)) //if we're waiting for an answer
	{
		_Module.m_server.m_cCarWashGeneralRequest.SetRequestResponse(lResult, lDevice, sData, lFlags2);
	}
}

/******************************************************************************
 Description:	Set pump test or calibration in misc ext according to the flags 
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Omer T		13/04/2010   09:05			Start	4.0.27.60	TD 231472	
 Alex M		13/11/2011   09:05			    	4.0.25.910	TD 341239
******************************************************************************/
void CGCI::SetPumpMiscExtPumpTest(long lPumpNumber, long lFlags)
{
	if(PUMP_NOT_VALID(lPumpNumber))
		return;

	long p = lPumpNumber - 1;

	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[p].GetStat();
	long lMiscExt = cPumpStatus.GetMiscExt();

	//4.0.31.??? TD 341239 turn off the misc ext
	if ((lMiscExt & MISC_EXT_PUMP_TEST) || (lMiscExt & MISC_EXT_PUMP_CALIBRATION))
	{
		_Module.m_server.m_cCommonFunction.SetPumpMisc(lPumpNumber,MISC_EXT_NOT_PUMP_TEST,TRUE,NULL,TRUE);
		_Module.m_server.m_cCommonFunction.SetPumpMisc(lPumpNumber,MISC_EXT_NOT_PUMP_CALIBRATION,TRUE,NULL,TRUE);
	}

	if(lFlags & TRS_IS_PUMP_TEST)
	{
		lMiscExt |= MISC_EXT_PUMP_TEST;
		cPumpStatus.SetMiscExt(lMiscExt);
		_Module.m_server.m_cPumpArray[p].SetStat(cPumpStatus);
	}
			
	else if(lFlags & TRS_IS_PUMP_CALIBRATION)
	{
		lMiscExt |= MISC_EXT_PUMP_CALIBRATION;
		cPumpStatus.SetMiscExt(lMiscExt);
		_Module.m_server.m_cPumpArray[p].SetStat(cPumpStatus);
	}

	  // 4.25.994 TD 338501	
} 



/******************************************************************************
 Description: set pump test/calibration flag into the trs	
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Omer T		13/04/2009   09:05			Start		4.0.27.60	TD 231472
******************************************************************************/
void CGCI::SetPumpTestToTrs(long lPumpNumber , CPumpTransact *pTrs)
{
	if(PUMP_NOT_VALID(lPumpNumber))
		return;

	long p = lPumpNumber - 1;
	
	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[p].GetStat();
	long lMiscExt = cPumpStatus.GetMiscExt();

	if(lMiscExt & MISC_EXT_PUMP_TEST)
	{
		SetPumpTestToTrsMisc(PUMP_TEST_MODE, MISC_EXT_NOT_PUMP_TEST, "PUMP_TEST_MODE", pTrs, lPumpNumber);
	}
	else if(lMiscExt & MISC_EXT_PUMP_CALIBRATION)
	{
		SetPumpTestToTrsMisc(PUMP_CALIBRATION_MODE, MISC_EXT_NOT_PUMP_CALIBRATION, "PUMP_CALIBRATION_MODE", pTrs, lPumpNumber);
	}

	 //TD 358952 - Sasol: Stuck transactions on POS related to thread safety around CPumpStatus
} 


/******************************************************************************
 Description:

   Add protection to auto authorized flow (do not send AuthorizeAndLimit to CL):
   - when there is prepay in wait_progress and status changed from Authorized to Requesting  //4.0.23.504 - TD 79647
   -  protect in case of moving from DISPENSING to REQUESTING -  special for DOMS - merge from 4.0.25.180 TD 226263, 224899;4.0.27.330	TD 268644
   The function must be called from the critical section _Module.m_server.m_csPumpArray 

  26/07/12

  Added as a parameter the current CPumpStatus (status,res,res_ext).
  When the method is being processed the CPumpStatus in the pump array (m_cPumpArray[p].GetStat)
  is already updated when the new values. This is relevant to the exceptions section at the end of the method.

 Returns:      	
                
 Parameters:    

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Alex M		25/09/2011   09:05			Start		4.0.22.800	TD 268644
 OmerT      26/07/2012                              4.0.22.931  TD 325761
 OmerT     	19/08/12								4.0.22.950  TD 331900 
 AlexM		26/08/2014								4.0.25.1130
******************************************************************************/
BOOL CGCI::IsAutoAuthorize(long lPumpNumber, CPumpStatus cOldPumpStat, long lNewStatus, long lNewRes, long lNewResExt, long nPosition)
{
	BOOL bRetVal = TRUE;
	CPumpTransact trs;
	BOOL bFound = FALSE;

	long			p = lPumpNumber-1;

	PumpStatus      eOldStatus	= cOldPumpStat.GetStatus();
	PumpReservation eOldRes	= cOldPumpStat.GetRes();
    PumpResExt		eOldResExt	= cOldPumpStat.GetResExt();

	PumpStatus      eNewStatus	= l2PumpStatus(lNewStatus);
	PumpReservation eNewRes		= l2PumpReservation(lNewRes);
    PumpResExt		eNewResExt	= l2PumpResExt(lNewResExt);
	long			lMode		= _Module.m_server.m_cPumpArray[p].GetStat().GetMode()-1;

	CString str;
	str.Format("CGCI::IsAutoAuthorize lPumpNumber =%ld ,Old: Status =%ld, Res=%ld ,ResExt=%ld ,New:Status =%ld, Res=%ld ,ResExt=%ld "
		,lPumpNumber,eOldStatus,eOldRes,eOldResExt,eNewStatus,eNewRes,eNewResExt);
	_LOGMSG.LogMsg(str);
	
	
	if((eNewRes == GCI_IGNORE_FIELD_VALUE) || (eNewRes == UNKNOWN_RES))
		eNewRes = eOldRes;	

	if((eNewResExt == GCI_IGNORE_FIELD_VALUE) || (eNewResExt == UNKNOWN_RES_EXT))
		eNewResExt = eOldResExt; // 4.0.22.930  TD 325761  // 4.0.22.1190 TD 356629
	
	if(eNewRes == PRE_PAY_AT_POS)
	{
		bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs);
		if (!bFound)
			bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs);	

		str.Format("CGCI::IsAutoAuthorize Res is PRE_PAY_AT_POS, bFound = %d, NumberOfNonClosedPrePay = %d" , bFound , _Module.m_server.m_cPumpArray[p].GetNumberOfNonClosedPrePay());
		_LOGMSG.LogMsg(str);
	} // 4.0.22.930  TD 325761


	if (eNewRes == PAY_AT_KIOSK)
	{
		bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS,TRS_NO_LINK,&trs); 

		str.Format("CGCI::IsAutoAuthorize Res is PAY_AT_KIOSK, bFound = %d", bFound);
		_LOGMSG.LogMsg(str);
	}


	bRetVal = (((_Module.m_server.m_cPumpArray[p].GetNumberOfTimeOutPrePay() == 0) || (eNewRes == PAY_AT_PUMP)) && 
	(_Module.m_server.m_cModeArray[lMode].AutoAuthorize(eNewRes,eNewResExt) || eNewResExt == RES_EXT_AUTO_AUTH ||
	(_Module.m_server.m_cPumpArray[p].IsLoyaltyAutoStartPump() && _Module.m_server.m_cModeArray[lMode].PAKAutoAuthLoyaltyOnly())) ||  //4.0.23.1500
	(_Module.m_server.m_cPumpArray[p].IsAutoAuthForLoyaltyCustomer()) );	// CR 407579

	str.Format("CGCI::IsAutoAuthorize before checking for exceptions, auto authorize result = %s" , bRetVal ? "TRUE" : "FALSE");
	_LOGMSG.LogMsg(str);

	/********************
	 These are exceptions to auto authorize:

	prepay: received REQUESTING after AUTHORIZED or DISPENSING

	pak:    received REQUESTING after DISPENSING

	pap:    received REQUESTING after AUTHORIZED

	************************/

	bRetVal &= (!((eNewRes == PRE_PAY_AT_POS) && ((bFound) || (_Module.m_server.m_cPumpArray[p].GetNumberOfNonClosedPrePay() > 0))&& (eNewStatus == REQUESTING) && ((eOldStatus == AUTHORIZED)||(eOldStatus == DISPENSING)))); 	//4.0.22.860 306658
																																											
	bRetVal &= (!((eNewRes == PAY_AT_KIOSK) && (eOldStatus == DISPENSING &&   eNewStatus == REQUESTING)  && bFound));
																																																																																	
 	bRetVal &= (!((eNewRes == PAY_AT_PUMP)  && (eOldStatus == AUTHORIZED) && (eNewStatus == REQUESTING)));  // 4.0.22.900 /4.0.22.910  TD 321585    

	bRetVal &= (!_Module.m_server.m_cCommonFunction.IsNozzleLocked(lPumpNumber, nPosition));			// 4.0.25.1130 

   return bRetVal;
}

void CGCI::ExecuateAuthorizeAndLimit(CString &strCmdName, CCmdInfo &Cmd, CThreadInfo* pInfo, HRESULT &hRes, IGCIEvents* pEvent, long lEventRetCode)
{
	CString str;
	strCmdName = "AuthorizeAndLimitPump";
	CMD_STRUCT_AUTHORIZE_LIMIT *pCmdInfo;
	pCmdInfo = (CMD_STRUCT_AUTHORIZE_LIMIT *)Cmd.GetDataPtr();

	if (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse())// CR 444830
	{
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		CPumpStatus stat = pInfo->m_pServer->m_cPumpArray[Cmd.GetParameter() - 1].GetStat();	//4.0.30.500 - TD 320068
																								// save the pump test flag in misc ext. it will be read after the trs will be created.
		SetPumpMiscExtPumpTest(Cmd.GetParameter(), pCmdInfo->lFlags); 	// 4.0.27.60	TD 231472

		SetMiscExtAuthorizeSentIndication(Cmd.GetParameter(), &stat, TRUE);					//4.0.30.500 - TD 320068

		if (_Module.m_server.GetIFSFTerminalSrvInUse() || _Module.m_server.IsFuelMobileSrvInUse())// CR 444830
		{
			AuthorizeAndLimitUpdateLimitFromTerminalSrv(stat, pCmdInfo, pInfo, Cmd);
		}

		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("AuthorizeAndLimit, Level %d, GradePrice %d, MaxValue %ld, MaxVolume %ld, Flags %x",
			pCmdInfo->nLevel,
			pCmdInfo->lGradePrice,
			pCmdInfo->lMaxValue,
			pCmdInfo->lMaxVolume,
			pCmdInfo->lFlags); //4.0.9.507

		pInfo->m_pServer->m_cLogger.LogMsg(Cmd.GetParameter(), LOG_PUMP, str);
	}


	if (pCmdInfo->lFlags & MULTIPLE_VOLUME_AND_AMOUNT_LIMIT)
	{
		hRes = AuthoirzeAndLimitWithMutipleLimits(pCmdInfo, Cmd, pInfo, pEvent, lEventRetCode);
	}
	else if ((pCmdInfo->lFlags & LIMIT_FLEET) || (pCmdInfo->lFlags & LIMIT_REEFER))//4.0.9.507  4.0.10.06				
	{
		hRes = AuthoirzeAndLimitWithFleetOrReefer(pCmdInfo, pInfo, Cmd, pEvent, lEventRetCode);
	}
	// RFUEL-2303 Missing PREPAY_WITH_LOYALTY_PRICE_PROTECTION flag. Merge from 1023 [18/10/2020 MaximB]
	else  if ((pCmdInfo->lFlags & LOYALTY_DISCOUNT_DATA) || (pCmdInfo->lFlags & PREPAY_INCLUDES_GRADES_RESTRICTIONS) || (pCmdInfo->lFlags & PREPAY_WITH_LOYALTY_PRICE_PROTECTION))//4.0.9.507 //4.0.23.270 111746 //4.0.24.60
	{
		hRes = AuthoirzeAndLimitWithLoyaltyDiscountData(pCmdInfo, Cmd, pInfo, hRes, pEvent, lEventRetCode);
	}
	else
	{
		hRes = AuthoirzeAndLimit(hRes, pEvent, Cmd, pCmdInfo, lEventRetCode);
	}
}


// 4.0.23.1130 TD 283329
bool CGCI::IsReservationPakWithLoyaltyAsPap(long lPumpNumber)
{
	PumpReservation eResStat = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetRes();
	PumpResExt ePumpResExt = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetResExt(); 

	return ((eResStat == PAY_AT_PUMP) && ((ePumpResExt == RES_EXT_PAK_LOYALTY) || (_Module.m_server.m_cPumpArray[lPumpNumber-1].IsPapToPakTrs())));//4.0.23.1501 326452
}

long CGCI::UpdatePrePayExtraInfoWithCarWashData(TAG_EXTRA_INFO & cTagExtraInfo  ,EXTRA_INFO & cExtraInfo)
{

	//4.0.21.500 - TD 14778
	//Update ExtraInfo QDEX with latest C/W data from CL
	long lRtc;
	//4.0.21.501 - TD 70420
	long lDPTProductCode = a2l(cTagExtraInfo.sDPTProductCode, sizeof(cTagExtraInfo.sDPTProductCode));
	if (lDPTProductCode > 0)
	{

		lRtc =  _Module.m_server.IndexOperation(EXTRA_INFO_FILE,REC_READ,&cExtraInfo);
		if (lRtc == OK)
		{
			memcpy(cExtraInfo.sWashCode, cTagExtraInfo.sWashCode, sizeof(cTagExtraInfo.sWashCode)); //4.0.21.501 - TD 70634


			long lCarWashProg = a2l(cTagExtraInfo.sCarWashProgram, sizeof(cTagExtraInfo.sCarWashProgram));
			l2a(lCarWashProg, &cExtraInfo.sCarWashProgram, sizeof(cExtraInfo.sCarWashProgram));

			memcpy(cExtraInfo.sWashExpire, cTagExtraInfo.sWashExpire, sizeof(cExtraInfo.sWashExpire));

			cExtraInfo.lCarWashTax = a2l(cTagExtraInfo.sCarWashTax,sizeof(cTagExtraInfo.sCarWashTax)); // 4.0.29.500 TD 287153
			cExtraInfo.lCarWashAmount = a2l(cTagExtraInfo.sCarWashAmount,sizeof(cTagExtraInfo.sCarWashAmount)); // 4.0.29.500 TD 287153
			cExtraInfo.lCarWashDiscount = a2l(cTagExtraInfo.sCarWashDiscount,sizeof(cTagExtraInfo.sCarWashDiscount)); // 4.0.29.500 TD 287153

			memcpy(cExtraInfo.sDPTProductCode, cTagExtraInfo.sDPTProductCode, sizeof(cExtraInfo.sDPTProductCode));					
			memcpy(cExtraInfo.sFullVehicleId, cTagExtraInfo.sFullVehicleId, sizeof(cExtraInfo.sFullVehicleId));		//4.0.23.500 TD 69914			
			memcpy(cExtraInfo.sANPRRefNumber, cTagExtraInfo.sANPRRefNumber, sizeof(cExtraInfo.sANPRRefNumber));					

			lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE,REC_UPDATE,&cExtraInfo);
		}
	}

	return lRtc;

}

/******************************************************************************
 Description: if IndicateAuthorizePumpSent flag is turned on, add the MiscExt MISC_EXT_AUTHORIZE_PUMP_SENT	
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shuki		17/06/2012   18:00			Start		4.0.30.500	TD 320068
******************************************************************************/
void CGCI::SetMiscExtAuthorizeSentIndication(long lPumpNumber, CPumpStatus *cPumpStatus, BOOL bAddMiscExt)
{
	if(_Module.m_server.GetIndicateAuthorizePumpSentFlag())
	{	
		long lMiscExt = cPumpStatus->GetMiscExt();
		
		if(bAddMiscExt)
			lMiscExt |= MISC_EXT_AUTHORIZE_PUMP_SENT;
		else
			lMiscExt &= MISC_EXT_NOT_AUTHORIZE_PUMP_SENT;
		
		cPumpStatus->SetMiscExt(lMiscExt);
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetStat(*cPumpStatus);
	}
}

//4.0.25.720 - TD 325958 
/******************************************************************************
 Description:  Check if PrePay was dispensed fully base on limit type

 Returns:      BOOL - indicating if PrePay is unbalanced
                
 Parameters:   CPumpTransact - current transaction
			   long - service fee
			   PRE_PAY_INFO - prepay information

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Shuki		09/07/2012					Start	4.0.25.720 - TD 325958 
******************************************************************************/
BOOL CGCI::IsPrePayUnbalanced( CPumpTransact &trs, long lServiceFee, PRE_PAY_INFO &cTmpPrePayinfo)
{
	BOOL bRetVal = FALSE;
	
	if(cTmpPrePayinfo.sPreSetLimitType == LIMIT_BY_VALUE)
	{
		if((trs.m_lRoundedValue + lServiceFee) < cTmpPrePayinfo.lMaxValue)
			bRetVal = TRUE;
	}
	else if(cTmpPrePayinfo.sPreSetLimitType == LIMIT_BY_VOLUME)
	{
		if((trs.m_lRoundedVolume      ) < cTmpPrePayinfo.lMaxVolume)
			bRetVal = TRUE;
	}

	return bRetVal; 
}


// 4.0.19.1770 TD 349356
void CGCI::UpdateTrsDetails( CPumpTransact &trs, short nNozzle, long lVolume, long lValue, short nLevel, long lGradePrice, short nFinalGrade, long p, long lPumpNumber, long lFinalTrsNumber )
{
	trs.m_nNozzle			= nNozzle;
	trs.m_lRoundedVolume	= lVolume;
	trs.m_lRawVolume		= lVolume;
	trs.m_lRoundedValue		= lValue;
	trs.m_nLevel			= nLevel;
	trs.m_lGradePrice		= lGradePrice;
	trs.m_nGrade			= nFinalGrade;
	
	// Update Shift Number - critical when doing Non locking shift.
	trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[p].GetShiftNumber();
	
	trs.UpdateTime();
	trs.m_lTankID =  _Module.m_server.m_cPumpArray[p].GetTankPerPosition(nNozzle);     
	
	if (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())
	{
		_Module.m_server.CentralUpdateTrs(lPumpNumber,lFinalTrsNumber,&trs,FALSE);	//4.0.19.820 66705 need to update values for the addItem struct to SPPS			
		
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("PakReadyTransaction, Trs %d details were updated.",trs.m_lNumber);
			_LOGMSG.LogMsg(str);
		}	
	}
}

void CGCI::SetPumpTestToTrsMisc(long lTestOrCalibMode, long lNotMisc, const char* const sMiscStr, CPumpTransact* pTrs, long lPumpNumber)
{
	long p = lPumpNumber - 1;

	// set the flag into the trs
	pTrs->setPumpTestOrCalib((BYTE)lTestOrCalibMode);
	BOOL bMiscExt = TRUE;
	BOOL bRemoveMisc = TRUE;
	long lVal = 0;
	
	_Module.m_server.m_cCommonFunction.SetPumpMisc(lPumpNumber, lNotMisc, bRemoveMisc, &lVal, bMiscExt);
	
	CString str;
	str.Format("Trs [%ld] is on [%s]", pTrs->m_lNumber, sMiscStr);
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
}


void CGCI::HandleLoyaltyPhoneLookUp(long lPumpNumber, long lSeqNumber , OLA_STAT *pOla, PAY_AT_PUMP_INFO & cInfo, CARD_SALE_ALL3* pCardData)
{
	char sMsg[MAX_MSG_LEN] = {0};
	sprintf_s(sMsg,MAX_MSG_LEN,"CGCI::HandleLoyaltyPhoneLookUp() - sEntryMethod = %c, sParamResult = %c", cInfo.CardSaleInfo.cardData.sEntryMethod, cInfo.CardSaleExtraData3.sParamResult[0]);
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
	
	// 4.0.23.1780	cInfo.CardSaleInfo.cardData.sEntryMethod = 'M';
	if ((cInfo.CardSaleInfo.cardData.sEntryMethod == 'S') &&
		(cInfo.CardSaleExtraData3.sParamResult[0] != 'N') &&	
		(cInfo.CardSaleExtraData3.sParamResult[0] != 'Y'))
	{
		long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, pOla, &cInfo, FALSE, TRUE); //4.0.20.440 68857	//4.0.21.501 //4.0.23.240
		
		//Save loyalty card number (numeric)
		if (FALSE == _Module.m_server.m_cCommonFunction.IsLoyaltytCard(lMediaMasterType))//4.0.23.390 116544
		{
			_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_UNKNOWN_ERROR);
			_Module.m_server.m_cFuelLoayaltyPS.HandleAddMemberCardResult(lPumpNumber, pOla, LOYALTY_STATE_UNKNOWN_ERROR);
			// _LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Card type is not loyalty, declined by PumpSrv");
			_LOGMSG.LogMsg("GCI", "HandleLoyaltyPhoneLookUp", lPumpNumber, LOG_PUMP, "Card type is not loyalty, declined by PumpSrv");
		}
		
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo);
		
	}
	else if(cInfo.CardSaleInfo.cardData.sEntryMethod == 'J') //4.0.23.481 126185
	{
		_Module.m_server.CancelLoyaltyTrs(lPumpNumber,pOla,&cInfo);	
	}
	else
		_Module.m_server.m_cFlowControlManager.HandleNMPPhoneLookUp(lPumpNumber, pOla, &cInfo);
}

void CGCI::CancelInTheMiddleOfAuthorize(CARD_SALE_ALL3 & cardDataAll3, long lPumpNumber, OLA_STAT & ola, BOOL & bCancelInMiddleOfAuthorize, CPumpTransact & trs, long lSeqNumber, GCIRetCode & retCode, BOOL & bMasterCancelOperation)
{
	CString str;
	long lPumpIndex = lPumpNumber - 1; // TD 425138 4.0.32.201

	// In case transaction is final has zero transaction clear the service fee charge 4.0.11.110

	if (LOG_BASIC_FLOW_CONTROL)
	{
		str.Format("PapReadyTrs - Checking Session %ld was not canceled in the middle, OLA = %x", ola.m_lSessionNumber, ola.m_byState);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
	}

	l2a(0, cardDataAll3.extData2.sServiceFee, sizeof(cardDataAll3.extData2.sServiceFee)); //4.0.21.0
	
	if(_Module.m_server.GetPAPDeclineTimeValue())
		_Module.m_server.m_cPumpArray[lPumpIndex].SetMiscCardNotAuthorized();	 //4.0.13.71	
	
	long lTmpFlags1 = a2l(cardDataAll3.extData3.sFlags1, sizeof(cardDataAll3.extData3.sFlags1));

	if( (SESSION_TYPE(ola.m_byState) == SESSION_AUTHORIZE) || (SESSION_TYPE(ola.m_byState) == SESSION_RECEIPT_AUTH) || (SESSION_TYPE(ola.m_byState) == SESSION_GET_CUSTOMER_INFO) 
		|| (OLA_STATE(ola.m_byState) == OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET))  // RFUEL 3195 - on timedout deferral message, transaction is not totally cancelled
	{
		if ((lTmpFlags1 & FL_PAP_LOYALTY_ACCEPTED) || (lTmpFlags1 & FL_PAP_PAYMENT_ACCEPTED))  //4.0.23.278?
		{
			if (LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format(">>> Force send canceltion to EFT (Session %ld), OLA = %x", 
					ola.m_lSessionNumber, 
					ola.m_byState);
				_LOGMSG.LogMsg("GCI", "CancelInTheMiddleOfAuthoirze", lPumpIndex + 1, LOG_NONE, str);
			}
		}
		else
			bCancelInMiddleOfAuthorize = TRUE;

		if(_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())
		{
			_Module.m_server.m_cFuelLoayaltyPS.EndTicket(lPumpNumber);
			if((OLA_STATE(ola.m_byState) == OLA_LOYALTY_WAIT_RESPONSE)||//it means cancel was 
				(OLA_STATE(ola.m_byState) == OLA_LOYALTY_START_TICKET)||
				(OLA_STATE(ola.m_byState) == OLA_LOYALTY_END_TICKET)||
				(OLA_STATE(ola.m_byState) == OLA_LOYALTY_WAIT_MORE_DATA) ||
				(OLA_STATE(ola.m_byState) == OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET) ||  // RFUEL 3195 - on timedout deferral message, transaction is not totally cancelled
				((OLA_STATE(ola.m_byState) == OLA_LOYALTY_SWIPE_LOYALTY) && (SESSION_TYPE(ola.m_byState) == SESSION_GET_CUSTOMER_INFO))) //4.0.19.500 LPE TD 59557
			{
				ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_NONE);								

				//Cancel ticket
			}
			_Module.m_server.m_cFlowControlManager.SetNMPTime(lPumpNumber, FALSE); //4.0.23.1270 299992 4.0.30.40
		}

#ifdef CANCEL_IN_MIDDLE
		if (ola.m_lSessionNumber) // 4.0.7.502
		{
			BOOL bSessionApproved = (lTmpFlags1 & FL_PAP_LOYALTY_ACCEPTED) || (lTmpFlags1 & FL_PAP_PAYMENT_ACCEPTED);

			if ((SESSION_TYPE(ola.m_byState) == SESSION_GET_CUSTOMER_INFO) && (!bSessionApproved))
			{
				ola.m_byState = OLA_NEW_STATE(ola.m_byState, OLA_SEND_END);

				cardDataAll3.CardSaleAll.data.sTranStatus = CD_DECLINE_USEALT;   //4.0.23.2806

				if (LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format(">>> Force send end loylaty session to EFT (Session %ld), OLA = %x , sTranStatus = %x", 
						ola.m_lSessionNumber, 
						ola.m_byState, 
						cardDataAll3.CardSaleAll.data.sTranStatus);
					_LOGMSG.LogMsg("GCI", "CancelInTheMiddleOfAuthoirze", lPumpIndex + 1, LOG_NONE, str);
				}
			}
			else
				ola.m_byState = SESSION_NEW_TYPE(ola.m_byState, SESSION_CANCEL_AUTH);
			_Module.m_server.m_cPumpArray[lPumpIndex].SetOlaStat(&ola);
		}
		else {
			// RFUEL-4351 - OLA-State was not set properly and cancellation was being
			// ignored, and finite state machine was moving to Fuel.  
			// Force the Sate to SESSION_CANCEL_AUTH
			ola.m_byState = SESSION_NEW_TYPE(ola.m_byState, SESSION_CANCEL_AUTH);
		}
		_Module.m_server.SetCancelInTheMiddleOfAuthorize(lPumpNumber,TRUE); // TD 337501  4.0.23.1580
		
#else
		ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_SEND_END);							 					
#endif
		
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format(">>> Cancel in middle of authorize (Session %ld), OLA = %x", ola.m_lSessionNumber,ola.m_byState);
			_LOGMSG.LogMsg("GCI", "CancelInTheMiddleOfAuthoirze", lPumpIndex + 1, LOG_NONE, str);
		}
		
		
		_Module.m_server.m_cPumpArray[lPumpIndex].SetOlaStat(&ola);
		_Module.m_server.SavePumpInfo(lPumpNumber);
	}
	else if(SESSION_TYPE(ola.m_byState) == SESSION_CLOCK)  //4.0.5.41
	{
		ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_SEND_END);	
		_Module.m_server.m_cPumpArray[lPumpIndex].SetOlaStat(&ola); 	
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format(">>> Cancel in middle of clock (Session %ld)", ola.m_lSessionNumber);
			_LOGMSG.LogMsg("GCI", "CancelInTheMiddleOfAuthoirze", lPumpIndex + 1, LOG_NONE, str);
		}
	}
	else if (ola.m_byState == OLA_LOYALTY_MEMBER_NOT_ACTIVE)
	{
		PAY_AT_PUMP_INFO pap;
		_Module.m_server.m_cPumpArray[lPumpIndex].GetPAPInfo(&pap);
		_Module.m_server.CancelLoyaltyTrs(lPumpNumber, &ola, &pap, FALSE);
		_Module.m_server.m_cPumpArray[lPumpIndex].SetOlaStat(&ola);
		_LOGMSG.LogMsg("GCI", "CancelInTheMiddleOfAuthoirze", lPumpNumber, LOG_NONE, "Member not active!", LOG_LEVEL_1);
	}	
	else if (IsInTheMiddleOfOlaSession(ola))
	{
		str.Format("Pump in middle of another session - Trs %d, SeqNumber %d, Ola state %x", trs.m_lNumber, lSeqNumber, ola.m_byState);
		_LOGMSG.LogMsg("GCI", "CancelInTheMiddleOfAuthoirze", lPumpNumber, LOG_NONE, str, LOG_LEVEL_1);
		retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
	}
	else if(SESSION_TYPE(ola.m_byState) == SESSION_COMPLETE)   //4.0.7.508 
	{
		str.Format("Duplicate complete session send,trs=%ld ,", trs.m_lNumber);
		_LOGMSG.LogMsg("GCI","CancelInTheMiddleOfAuthoirze", lPumpNumber, LOG_NONE, str, LOG_LEVEL_1);
		retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
	}				
	else // OLA_NONE = simply final transaction value = 0
	{
		if(PUMPSRV_IS_SLAVE)
		{
			// Cancel Operation reported by Master 
			// transaction never created.
			bMasterCancelOperation = TRUE;
		}	
	}

	_Module.m_server.ClearPapToPakFlag(lPumpNumber); // 4.0.23.2060 TD 381449
}


// 4.0.23.2070  TD 384668
void CGCI::LoyaltyEndPapTrs( long lPumpNumber, long lFlags , long lGradePrice , long lValue , long lVolume , short nGrade , PAY_AT_PUMP_INFO &cPapInfo, CPumpTransact &trs, OLA_STAT &ola , GCIRetCode & retCode )
{
	if (FALSE == _Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())
		return;

	//If we have loyalty in the system call add item and in the result we will close the ticket


	//set the status of the ola the rtaps  int the return value from the rtaps we will move the status to OLA_REQUEST_TO_SEND then call to add item
	CString str;
	long p = lPumpNumber - 1;

	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Starting Loyalty End of PAP flow");

	if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetInProgressWaitProgressTrs(&trs))	//4.0.20.480 //4.0.22.0 66977
	{
		if (LOG_BASIC_FLOW_CONTROL)
		{
			_LOGMSG.LogFlagsMsg(lPumpNumber, LOG_PUMP, "LoyaltyEndPapTrs,loyalty flow, trs not found", lFlags);
		}
		if (_Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber) == CancelAfterStartTicket)	//4.0.22.504 69344
		{
			_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber);
			_Module.m_server.m_cFuelLoayaltyPS.SetCancelTrs(lPumpNumber, NoCancel);
		}

		retCode = GCI_TRS_NOT_FOUND;
	}
	else
	{
		trs.m_lRoundedValue = lValue;
		trs.m_lRoundedVolume = lVolume;
		trs.m_nGrade = nGrade;
		trs.m_lGradePrice = lGradePrice;
		_Module.m_server.CentralUpdateTrs(lPumpNumber, trs.m_lNumber, &trs, FALSE);

		long lTmp = a2l(cPapInfo.CardSaleExtraData3.sFlags1, sizeof(cPapInfo.CardSaleExtraData3.sFlags1));

		if (LOG_BASIC_FLOW_CONTROL)  //4.0.13.202
		{
			CString str;
			str.Format("PAPReadyTransaction, Pump=%ld complete for pap trs, Loyalty Flags=0x%08x, Payment flags =0x%08x ", 
				p + 1, 
				lTmp & FL_PAP_LOYALTY_ACCEPTED, 
				lTmp & FL_PAP_PAYMENT_ACCEPTED); //4.0.23.1510 326452
			_LOGMSG.LogMsg(str);
		}

		// update PES promotion id 
		if (trs.m_nGrade)
		{
			long lGradePos = _Module.m_server.GetGradePositionRestrection(trs.m_nGrade, trs.m_lPump) - 1;
			if (lGradePos >= 0 && lGradePos < MAX_POSITIONS_PER_PUMP)
			{
				if (_Module.m_server.m_cFuelLoayaltyPS.m_dwLoyaltyEngineType == LPE)
				{
					long lPromID = 0;
					double lCentsOff = 0;

					memset(cPapInfo.CardSaleExtraData2.sPromotionNumber, ' ', sizeof(cPapInfo.CardSaleExtraData2.sPromotionNumber));
					_Module.m_server.m_cFuelLoayaltyPS.GetPromotionId(lGradePos, lPumpNumber - 1, &lPromID, &lCentsOff);

					if (lPromID > 0)
					{
						l2a(lPromID, cPapInfo.CardSaleExtraData2.sPromotionNumber, sizeof(cPapInfo.CardSaleExtraData2.sPromotionNumber));
					}

					if (LOG_DETAIL_FLOW_CONTROL)
					{
						str.Format("Update PAP promotion ID lGradePos = %ld, sPromotionNumber = %.10s",
							lGradePos,
							cPapInfo.CardSaleExtraData2.sPromotionNumber);
						_LOGMSG.LogMsg(str);
					}
				}
				else
				{	
					memset(cPapInfo.CardSaleExtraData6.sPromotionID, ' ', sizeof(cPapInfo.CardSaleExtraData6.sPromotionID));
					memcpy(cPapInfo.CardSaleExtraData6.sPromotionID, cPapInfo.CardSaleExtraData6.sGradeDiscountID[lGradePos], sizeof(cPapInfo.CardSaleExtraData6.sGradeDiscountID[lGradePos]));
					
					if (LOG_DETAIL_FLOW_CONTROL)
					{
						str.Format("Update PAP promotion ID lGradePos = %ld  GradediscountId = %.15s, sPromotionNumber = %.15s",
							lGradePos,
							cPapInfo.CardSaleExtraData6.sGradeDiscountID[lGradePos],
							cPapInfo.CardSaleExtraData6.sPromotionID);
						_LOGMSG.LogMsg(str);
					}
				}

				_Module.m_server.m_cPumpArray[p].SetPAPInfo(&cPapInfo);	
			}
			else
			{	
				str.Format("Invalid Grade position: %ld", lGradePos);
				_LOGMSG.LogMsg(str);
			}
		}
			
		if ((0 == (_Module.m_server.m_cFuelLoayaltyPS.IsCancelTrs(lPumpNumber))) || (lTmp & FL_PAP_LOYALTY_ACCEPTED))    //4.0.23.273?	//Copient 4.0.19.502 + TD 59557
		{
			_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);

			if (FALSE == _Module.m_server.m_cFuelLoayaltyPS.GetInitDllFlag() && (!(lTmp & FL_PAP_LOYALTY_ACCEPTED)))
			{
				_LOGMSG.LogMsg("CGCI", "LoyaltyPapReadyTrs", lPumpNumber, LOG_PUMP, "LoyaltyFuel is not initialized! skipping add item and moving to complete session");

				ola.m_byState = SESSION_COMPLETE | OLA_REQUEST_TO_SEND;
				_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "LoyaltyEndPapTrs  Pump=%ld starting complete process for pap");
			}
			else
			{
				ola.m_byState = SESSION_COMPLETE | OLA_LOYALTY_WAIT_RESPONSE;
				_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
				_Module.m_server.m_cFuelLoayaltyPS.SetTotalDiscount(p, 0);//Liat LPE - initialize total discount
				_Module.m_server.m_cFuelLoayaltyPS.SetPrintReceipt(p, TRUE);//TBD - Liat LPE - initialize According to user's choose 
				_Module.m_server.m_cFuelLoayaltyPS.AddItem(lPumpNumber, trs.m_lNumber, ola);//4.0.9.508
				
				cPapInfo.CardSaleExtraData4.sDeferralAccepted = '0';
				if (!_Module.m_server.m_cFuelLoayaltyPS.IsDeferralDiscountSelected(lPumpNumber))	//RFUEL-2423
				{
					cPapInfo.CardSaleExtraData4.sDeferralAccepted = '1';
				}
				_Module.m_server.m_cPumpArray[p].SetPAPInfo(&cPapInfo);
			}

			_Module.m_server.m_cPumpArray[p].GetPAPInfo(&cPapInfo); //4.0.23.390 119090	
		}
		else
		{
			// TD 476769  + TD 476762 Start [3/7/2017 MaximB]
			_Module.m_server.m_cPumpArray[p].GetPAPInfo(&cPapInfo);
			// TD 476769  + TD 476762 END [3/7/2017 MaximB]

			if (CancelBeforeStartTicket == _Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber))
			{
				if (LOG_BASIC_FLOW_CONTROL)
				{

					str.Format("LoyaltyEndPapTrs Flags1 = %ld", a2l(cPapInfo.CardSaleExtraData3.sFlags1, sizeof(cPapInfo.CardSaleExtraData3.sFlags1)));
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
				}

				if (IsReservationPakWithLoyaltyAsPap(lPumpNumber)) //4.0.23.1510 326452
				{
					_Module.m_server.m_cPumpArray[lPumpNumber - 1].ClearTrs(trs.m_lNumber);
					_Module.m_server.m_cPumpArray[lPumpNumber - 1].ClearOlaStat();
					_Module.m_server.SavePumpInfo(lPumpNumber);

					_Module.m_server.SendSwipeResult(ola.m_byState, p + 1, CD_FORCE_NOT_OK, cPapInfo.CardSaleInfo.cardData.sTranStatus, (CARD_SALE_ALL3 *)&cPapInfo.CardSaleInfo.cardData);
					if (LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("PapReadyTransaction cancel PAK detected");
						_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
					}
				}
				else
				{
					if (LOG_BASIC_FLOW_CONTROL)  //4.0.13.202
					{
						CString str;
						str.Format("PAPReadyTransaction  Pump=%ld  complete for pap trs, Loyalty Flags=0x%08x, Payment flags =0x%08x",
							p + 1, 
							lTmp & FL_PAP_LOYALTY_ACCEPTED, 
							lTmp & FL_PAP_PAYMENT_ACCEPTED); //4.0.23.1510 326452
						_LOGMSG.LogMsg(str);
					}

					if (lTmp & FL_PAP_LOYALTY_ACCEPTED)    //4.0.23.273?
					{
						ola.m_byState = SESSION_COMPLETE | OLA_LOYALTY_WAIT_RESPONSE;
						_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
						_Module.m_server.m_cFuelLoayaltyPS.SetTotalDiscount(p, 0);
						_Module.m_server.m_cFuelLoayaltyPS.SetPrintReceipt(p, TRUE);
						_Module.m_server.m_cFuelLoayaltyPS.AddItem(lPumpNumber, trs.m_lNumber, ola);//4.0.9.508
					}
					else
					{
						ola.m_byState = SESSION_COMPLETE | OLA_REQUEST_TO_SEND;
						_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
					}
				}
			}
			else
			{
				ola.m_byState = OLA_LOYALTY_CANCEL_TICKET_AFTER_START_TICKET;
				_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
			}
			_Module.m_server.m_cFuelLoayaltyPS.SetCancelTrs(lPumpNumber, NoCancel);
		}
	}
}

void CGCI::AuthorizeAndLimitUpdateLimitFromTerminalSrv( CPumpStatus &stat, CMD_STRUCT_AUTHORIZE_LIMIT * pCmdInfo, CThreadInfo* pInfo, CCmdInfo &Cmd )
{
	if (stat.GetRes() == PAY_AT_KIOSK)
	{
		//1. calculate trs limit vs. limit from the terminal (coming from TerminalSrv)
		//2. set new limit for gci
		
		long lFlagsTmp = pCmdInfo->lFlags; //4.0.29.500 - TD 273171
		long lLimitValueTmp = 0;
		pInfo->m_pServer->CalculateTrsLimit(Cmd.GetParameter(), &lFlagsTmp, &lLimitValueTmp);
		
		if (lLimitValueTmp > 0)
		{
			if (lFlagsTmp & LIMIT_BY_VALUE)
			{
				pCmdInfo->lMaxValue = lLimitValueTmp;
				pCmdInfo->lMaxVolume = 0;
			}
			else if (lFlagsTmp & LIMIT_BY_VOLUME)
			{
				pCmdInfo->lMaxVolume = lLimitValueTmp;
				pCmdInfo->lMaxValue = 0;						
			}
		}
		
		// send update to TerminalSrv
		if (lFlagsTmp & LIMIT_BY_VALUE)
			_Module.m_server.m_cGPIEventMgr.AddSetPreSetLimit(Cmd.GetParameter(),lFlagsTmp, pCmdInfo->lMaxValue);
		else if (lFlagsTmp & LIMIT_BY_VOLUME)
			_Module.m_server.m_cGPIEventMgr.AddSetPreSetLimit(Cmd.GetParameter(),lFlagsTmp, pCmdInfo->lMaxVolume);
		else if (lFlagsTmp & MULTIPLE_VOLUME_AND_AMOUNT_LIMIT)	// CR 370688
		{
			pCmdInfo->lMaxVolume = 0;
			pCmdInfo->lMaxValue = 0;
		}
	}
}


/******************************************************************************
 Description:	AuthoirzeAndLimitWithMutipleLimits

 Returns:      	void 
 Parameters:    long lPumpNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 OmerT			02/01/2014   14:00		Start	  CR 370688
******************************************************************************/
HRESULT CGCI::AuthoirzeAndLimitWithMutipleLimits( CMD_STRUCT_AUTHORIZE_LIMIT * pCmdInfo, CCmdInfo &Cmd, CThreadInfo* pInfo, IGCIEvents* pEvent, long lEventRetCode )
{
	HRESULT hRes = S_OK;
	
	TAG_DISCOUNT_MAP_AND_MULTIPLE_LIMIT  rec;
	memset(&rec, '0' , sizeof(rec));
	memcpy(rec.GradeAmountLimit, pCmdInfo->GradeAmountLimit, sizeof(pCmdInfo->GradeAmountLimit));
	memcpy(rec.GradeVolumeLimit, pCmdInfo->GradeVolumeLimit, sizeof(pCmdInfo->GradeVolumeLimit));


	BYTE sTmp[sizeof(TAG_DISCOUNT_MAP_AND_MULTIPLE_LIMIT)+2];
	Str2StrNull((BYTE *)&rec,sizeof(rec),sTmp);
	CComBSTR sLimits(sizeof(sTmp),(LPCSTR)sTmp); 
	
	hRes = pEvent->AuthorizeAndLimit3(	
		Cmd.GetParameter(),
		Cmd.GetSrc(),
		pCmdInfo->nLevel,
		pCmdInfo->lGradePrice,
		pCmdInfo->lMaxValue,
		pCmdInfo->lMaxVolume,
		pCmdInfo->lFlags,
		sLimits,
		&lEventRetCode);	
	
	return hRes;
}

HRESULT CGCI::AuthoirzeAndLimitWithFleetOrReefer( CMD_STRUCT_AUTHORIZE_LIMIT * pCmdInfo, CThreadInfo* pInfo, CCmdInfo &Cmd, IGCIEvents* pEvent, long lEventRetCode )
{
	HRESULT hRes = S_OK;
	TAG_FLEET_INFO	 rec;		  //4.0.10.130
	memset(&rec, ' ' , sizeof(rec));
	if (pCmdInfo->lFlags & LOYALTY_DISCOUNT_DATA)
	{
		for (int i=0 ; i< MAX_GRADES ; i++)
			l2a0(pCmdInfo->lDiscountArray[i], rec.sDiscount[i],  sizeof(rec.sDiscount[0]));
		
		memcpy( rec.sDiscountType , pCmdInfo->sDiscountType , sizeof(rec.sDiscountType)); // 4.0.6.502
		
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("detect fleet with discount");											
			pInfo->m_pServer->m_cLogger.LogMsg(str);							
		}
	}
	
	//4.0.11.504
	l2a(pCmdInfo->lTotalAmount, rec.sTotalAmount, sizeof(rec.sTotalAmount)); //4.0.10.130
	l2a(pCmdInfo->lPreviousFueledAmount, rec.sPreviousFueledAmount , sizeof(rec.sPreviousFueledAmount)); //4.0.10.130
	
	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("Download Discount Pump %02ld , MaxValue %ld, MaxVolume %ld, Flags 0x%08x, Grade01=%ld,Grade02=%ld,Grade03=%ld,Grade04=%ld,Grade05=%ld,Grade06=%ld,Grade07=%ld,Grade08=%ld,Grade09=%ld,Grade10=%ld , trs limit=%ld ,PreviousFueledAmount=%ld ",
			Cmd.GetParameter(),
			pCmdInfo->lMaxValue,
			pCmdInfo->lMaxVolume,
			pCmdInfo->lFlags,
			pCmdInfo->lDiscountArray[0],
			pCmdInfo->lDiscountArray[1],
			pCmdInfo->lDiscountArray[2],
			pCmdInfo->lDiscountArray[3],
			pCmdInfo->lDiscountArray[4],						
			pCmdInfo->lDiscountArray[5],
			pCmdInfo->lDiscountArray[6],
			pCmdInfo->lDiscountArray[7],
			pCmdInfo->lDiscountArray[8],
			pCmdInfo->lDiscountArray[9], 
			pCmdInfo->lTotalAmount,
			pCmdInfo->lPreviousFueledAmount
			
			);
		
		pInfo->m_pServer->m_cLogger.LogMsg(str);							
	}
	
	
	
	BYTE sTmp[sizeof(TAG_FLEET_INFO)+2];
	Str2StrNull((BYTE *)&rec,sizeof(rec),sTmp);
	CComBSTR sDiscountData(sizeof(sTmp),(LPCSTR)sTmp); 
	
	hRes = pEvent->AuthorizeAndLimit3(	
		Cmd.GetParameter(),
		Cmd.GetSrc(),
		pCmdInfo->nLevel,
		pCmdInfo->lGradePrice,
		pCmdInfo->lMaxValue,
		pCmdInfo->lMaxVolume,
		pCmdInfo->lFlags, //4.0.9.600
		sDiscountData,
		&lEventRetCode);	
	
	return hRes;
}

HRESULT CGCI::AuthoirzeAndLimitWithLoyaltyDiscountData( CMD_STRUCT_AUTHORIZE_LIMIT * pCmdInfo, CCmdInfo &Cmd, CThreadInfo* pInfo, HRESULT hRes, IGCIEvents* pEvent, long lEventRetCode )
{
	TAG_DISCOUNT_MAP_AND_LIMIT rec;	//4.0.22.506 67786	//RFS 1023 WILL NEED TO PARSE THE NEW STRUCT!!
	memset(&rec, '0' , sizeof(rec));
	
	// start 3.1.1.64
	for (int i=0 ; i< MAX_GRADES ; i++)
		l2a0(pCmdInfo->lDiscountArray[i], rec.sDiscountArray.sDiscount[i],  sizeof(rec.sDiscountArray.sDiscount[0])); //4.0.22.506 67786
	
	memcpy( rec.sDiscountArray.sDiscountType , pCmdInfo->sDiscountType , sizeof(rec.sDiscountArray.sDiscountType)); // 4.0.6.502 //4.0.22.506 67786
	l2a(pCmdInfo->lLoyaltyVolumeLimit, rec.sDiscountArray.sLoyaltyVolumeLimit, sizeof(rec.sDiscountArray.sLoyaltyVolumeLimit)); //4.0.20.450 66977 //4.0.21.501 //4.0.22.506 67786
	memcpy(rec.GradeAmountLimit, pCmdInfo->GradeAmountLimit, sizeof(rec.GradeAmountLimit)); //4.0.22.506 67786
	
	//4.0.20.380 //4.0.21.12 68431
	CString str;
	str.Format("Download Discount Pump %02ld , MaxValue %ld, MaxVolume %ld, Flags %x, discountType=%.2s, LoyaltyLimit %ld",
		Cmd.GetParameter(),
		pCmdInfo->lMaxValue,
		pCmdInfo->lMaxVolume,
		pCmdInfo->lFlags,
		pCmdInfo->sDiscountType,
		pCmdInfo->lLoyaltyVolumeLimit); //4.0.20.450 66977 //4.0.21.501
	pInfo->m_pServer->m_cLogger.LogMsg(Cmd.GetParameter(),LOG_PUMP,str);
	// end  3.1.1.64
	
	BYTE sTmp[sizeof(TAG_DISCOUNT_MAP_AND_LIMIT)+2];	//4.0.22.506 67786
	Str2StrNull((BYTE *)&rec,sizeof(rec),sTmp);
	CComBSTR sDiscountData(sizeof(sTmp),(LPCSTR)sTmp); 
	
	/*
	* RFUEL-2303
	* Removed calling to AuthorizeAndLimit2. [18/10/2020 MaximB]
	*/
	hRes = pEvent->AuthorizeAndLimit3(Cmd.GetParameter(),
		Cmd.GetSrc(),
		pCmdInfo->nLevel,
		pCmdInfo->lGradePrice,
		pCmdInfo->lMaxValue,
		pCmdInfo->lMaxVolume,
		pCmdInfo->lFlags,
		sDiscountData,
		&lEventRetCode);
		
	return hRes;
}

HRESULT CGCI::AuthoirzeAndLimit( HRESULT hRes, IGCIEvents* pEvent, CCmdInfo &Cmd, CMD_STRUCT_AUTHORIZE_LIMIT * pCmdInfo, long lEventRetCode )
{
	hRes = pEvent->AuthorizeAndLimit(	Cmd.GetParameter(),
		Cmd.GetSrc(),
		pCmdInfo->nLevel,
		pCmdInfo->lGradePrice,
		pCmdInfo->lMaxValue,
		pCmdInfo->lMaxVolume,
		(short)pCmdInfo->lFlags,
		&lEventRetCode);	return hRes;
}



/******************************************************************************
 Description: Fills the tank reading info with data recived from CL/RFS

				
 Returns:      	
 Parameter   :  
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 OmerT			24/07/14				4.0.19.2170   CR 409564
 ******************************************************************************/
void CGCI::FillTankReadingInfo( IN OUT TANK_READ_INFO &info, long lTankNumber, long lProduct, long lStatus1, short nFlags, long lFuelLevel, long lWaterLevel, long lTemperature, long lVolume, long lWaterVolume, long lStatus2, long lFuelValue, long lPricePerLitre, long lTcVolume, long lUllage )
{
	memset(&info,0,sizeof(TANK_READ_INFO));

	info.lIndexNumber	= _Module.m_server.NewTankReadIndex();
	info.lTank			= lTankNumber;
	info.lProduct		= lProduct;
	info.lStatus		= lStatus1;
	info.nFlags			= nFlags;
	
	if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_TANKS )
		info.nFlags |= TANK_READ_ON_DEMAND_REQUEST;
	
	info.lFuelLevel		= lFuelLevel;
	info.lWaterLevel	= lWaterLevel;
	info.lTemperature	= lTemperature;
	
	info.lFuelVolume	= lVolume * _Module.m_server.GetVolumeMultiply();
	
	info.lWaterVolume	= lWaterVolume; 
	info.lStatus2		= lStatus2;
	info.lFuelValue		= lFuelValue;
	info.lPricePerLitre = lPricePerLitre;
	info.lTcVolume      = lTcVolume;
	info.lUllage        = lUllage;
	info.dwActiveReadNumber = _Module.m_server.GetActiveReadNumber(); // attach current active read number 
	info.dwShiftNumber	= _Module.m_server.GetFormalShiftNumber(); // attach current shift number 
	
	GetLocalTime(&info.cSysTime);
}


/******************************************************************************
 Description: Fills the delivery report info with data recived from CL/RFS

				
 Returns:      	
 Parameter   :  
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 OmerT			24/07/14				4.0.19.2170   CR 409564
 ******************************************************************************/
void CGCI::FillDeliveryReport( IN OUT DELIVERY_INFO &info, long lTankNumber, long lProduct, long lStatus, short nFlags, long lVolume, long lStartVolume, long lEndVolume, long lTemperature, BSTR sStartDateAndTime, GCIRetCode &retCode, BSTR sEndDateAndTime,DWORD &dwLastErr )
{
	memset(&info,0,sizeof(DELIVERY_INFO));

	info.lIndexNumber	= _Module.m_server.NewFuelDeliveryIndex();
	info.lTank			= lTankNumber;
	info.lProduct		= lProduct;
	info.lStatus		= lStatus;
	info.nFlags			= nFlags;
	
	if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY )
		info.nFlags |= DELIVERY_REPORT_ON_DEMAND_REQUEST;
	
	
	info.lDeliveredVolume = lVolume ;
	info.lStartVolume	= lStartVolume ;
	info.lEndVolume		= lEndVolume ;
	info.lTemperature	= lTemperature ;
	info.dwActiveReadNumber = _Module.m_server.GetActiveReadNumber();
	info.dwShiftNumber = _Module.m_server.GetFormalShiftNumber();
	
	SZ_TAG_SYSTEMTIME	TagTime;
	int iLen = WideCharToMultiByte(CP_ACP,0,sStartDateAndTime,-1,(char *)&TagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GCI_LAST_ERROR;
	}
	else
	{
		Tag2SystemTime(&TagTime.sTime,&info.cDeliveryDateTime);

		if(sEndDateAndTime)
		{
			iLen = WideCharToMultiByte(CP_ACP,0,sEndDateAndTime,-1,(char *)&TagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
			if(iLen == 0)
			{
				//4.0.12.517
				if (!(nFlags & DELIVERY_REPORT_INVOICE_DATA))
				{
					dwLastErr = GetLastError();
					retCode = GCI_LAST_ERROR;
				}
			}
			else
			{
				Tag2SystemTime(&TagTime.sTime,&info.cEndDeliveryDateTime);
			}
		}
	}
}

void CGCI::ExecuteSwipeResult( CCmdInfo &Cmd, CString &strCmdName, CThreadInfo* pInfo, HRESULT &hRes, IGCIEvents* pEvent, long lEventRetCode )
{
	//If we work in xml mode with the convert layer - send swipe result with xml buffer
	long lPumpNumber = Cmd.GetParameter();
	strCmdName = "SwipeResult";
	CMD_STRUCT_PAP_SWIPE_RESULT3_EX *pCmdInfo;   			
	pCmdInfo = (CMD_STRUCT_PAP_SWIPE_RESULT3_EX *)Cmd.GetDataPtr(); 
	CXMLInterface cData;
	BOOL		bRet = FALSE;

	BYTE sTmp[sizeof(CARD_SALE_EXTRA_DATA4_PLUS)+2] = {0};			//4.0.19.504 - 47559 //4.0.24.60 109103

	long lOlaRecordSize = (pCmdInfo->lFlags & STRUCT_IS_CARD_ALL_DATA3 )? sizeof(CARD_SALE_ALL3): sizeof(CARD_SALE_ALL2);


	if ((LOG_BASIC_FLOW_CONTROL) && (pCmdInfo->lAnswer == CD_MORE_DATA_REQUIRED))
	{
		CString strTmp;

		strTmp.Format("CARD_SALE_ALL3: sTranNbr %ld, sTranStatus %c, PromptName=%c ",
			a2l(pCmdInfo->data.sTranNbr, sizeof(pCmdInfo->data.sTranNbr)), pCmdInfo->data.sTranStatus, pCmdInfo->extData3.sParamResult[0]); //4.0.27.45

		pInfo->m_pServer->m_cLogger.LogMsg(strTmp);
	}


	BYTE *sTmpAllocationBuffer  = new BYTE[lOlaRecordSize + sizeof(OLA_EXTRA_DATA) + 2];  //4.0.5.0				
	memset(sTmpAllocationBuffer, 0, lOlaRecordSize + sizeof(OLA_EXTRA_DATA) + 2);

	if (pCmdInfo->lFlags & STRUCT_IS_CARD_ALL_DATA3)
	{
		if (FALSE == _Module.m_server.GetIFSFTerminalSrvInUse()) //4.0.22.130 - TD 74795 
		{
			CARD_SALE_ALL3 cCardSaleAll3;
			memset(&cCardSaleAll3,' ',sizeof(cCardSaleAll3));

			cCardSaleAll3.CardSaleAll.data    = pCmdInfo->data;
			cCardSaleAll3.CardSaleAll.extData = pCmdInfo->extData;
			cCardSaleAll3.extData2            = pCmdInfo->extData2;
			cCardSaleAll3.extData3            = pCmdInfo->extData3;
			cCardSaleAll3.extData4            = pCmdInfo->extData4;
			cCardSaleAll3.extData5            = pCmdInfo->extData5;
			cCardSaleAll3.extData6            = pCmdInfo->extData6;

			//Encrypt the fields before the send.					
			if (pCmdInfo->lAnswer == CD_FORCE_NOT_OK || pCmdInfo->lAnswer == CD_FORCE_OK || pCmdInfo->lAnswer == CD_CANCEL_OK)
			{								
				pInfo->m_pServer->m_cProtectedDataHandler.EncryptOLAFields(&cCardSaleAll3);
			}
			memcpy(sTmpAllocationBuffer, &cCardSaleAll3, lOlaRecordSize);  //4.0.13.501
		}
		else
			memcpy(sTmpAllocationBuffer, &pCmdInfo->data, lOlaRecordSize);
	}
	else
	{
		CARD_SALE_ALL2 cCardSaleAll2;
		memset(&cCardSaleAll2,' ',sizeof(cCardSaleAll2));

		cCardSaleAll2.CardSaleAll.data    = pCmdInfo->data;
		cCardSaleAll2.CardSaleAll.extData = pCmdInfo->extData;
		cCardSaleAll2.extData2            = pCmdInfo->extData2;

		//Encrypt the fields before the send.					
		if (pCmdInfo->lAnswer == CD_FORCE_NOT_OK || pCmdInfo->lAnswer == CD_FORCE_OK || pCmdInfo->lAnswer == CD_CANCEL_OK)
		{								
			pInfo->m_pServer->m_cProtectedDataHandler.EncryptOLAFields(&cCardSaleAll2);
		}
		memcpy(sTmpAllocationBuffer, &cCardSaleAll2, lOlaRecordSize);  //4.0.13.501
	}

	if (FALSE == _Module.m_server.GetIFSFTerminalSrvInUse())  //4.0.22.191 //CR 444830
	{
		if(pCmdInfo->extData2.sPinPadAction  != ' ' &&
			pCmdInfo->extData2.sPinPadAction  != '0' && //4.0.15
			pCmdInfo->extData2.sPinPadAction  !=  0)
		{
			if  (STRUCT_IS_CARD_ALL_DATA3  & pCmdInfo->lFlags)
				pCmdInfo->lFlags = STRUCT_IS_CARD_SALE_EXTRA_DATA3_PLUS;
			else
				pCmdInfo->lFlags = STRUCT_IS_CARD_SALE_EXTRA_DATA2_PLUS;

			memcpy(sTmpAllocationBuffer + lOlaRecordSize, _Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byLen, 4);
			memcpy(sTmpAllocationBuffer + lOlaRecordSize + 4, _Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byData,a2l(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byLen, 4));

			lOlaRecordSize += sizeof(OLA_EXTRA_DATA);

		}												

		if (!ChrAll((char *)pCmdInfo->extData2.sClockFlags,sizeof(pCmdInfo->extData2.sClockFlags)))
		{
			if (pCmdInfo->lAnswer == CD_FORCE_NOT_OK)
				pCmdInfo->lAnswer = CD_CLOCK_NOT_OK;

			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("Attendant2 clock result is : Rtc=%ld", pCmdInfo->lAnswer);
				pInfo->m_pServer->m_cLogger.LogMsg( lPumpNumber,LOG_PUMP,str ); 
			}
		}	
		else if (pInfo->m_pServer->GetOLAFlags(pCmdInfo->extData3.sFlags1 ,FL_CANCEL_REC,sizeof(pCmdInfo->extData3.sFlags1))) 
		{
			pCmdInfo->lAnswer = CD_CANCEL_OK;
			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("Cancel PAP 2");
				pInfo->m_pServer->m_cLogger.LogMsg( lPumpNumber,LOG_PUMP,str ); 
			}
		}
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString strTmp;
		strTmp.Format("PAPswipeResult len=%ld",lOlaRecordSize);					
		pInfo->m_pServer->m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,strTmp);
	}


	BYTE sBuff[MAX_XML_BUF_LEN];
	memset(&sBuff,0,MAX_XML_BUF_LEN);

	if (_Module.m_server.m_bUseXmlBuffer) 
	{
		CARD_SALE_EXTRA_DATA4_PLUS cCardSaleExtraDataPlus;						//4.0.24.60 109103

		memset(&cCardSaleExtraDataPlus, ' ', sizeof(cCardSaleExtraDataPlus));	//4.0.24.60 109103
		memcpy(&cCardSaleExtraDataPlus,sTmpAllocationBuffer,lOlaRecordSize);

		long lReceiptIndex = a2l(cCardSaleExtraDataPlus.CardSaleAll3.CardSaleAll.data.sTranNbr, sizeof((cCardSaleExtraDataPlus.CardSaleAll3.CardSaleAll.data.sTranNbr)));
		_Module.m_server.m_cCommonFunction.CopyZoneToCardSale(lPumpNumber,lReceiptIndex,(BYTE *)cCardSaleExtraDataPlus.cReceiptExtraInfo.sFiler2043); //4.0.31.??

		if(cCardSaleExtraDataPlus.CardSaleAll3.CardSaleAll.extData.sConfirmPrompt != ' ')	//costco
		{
			cCardSaleExtraDataPlus.CardSaleAll3.extData3.sReceiptMode = 'Y';
		}
		_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml((char*)&sBuff,&cCardSaleExtraDataPlus);

		lOlaRecordSize = MAX_XML_BUF_LEN;

		char sLoyaltyMsg[MAX_FIELD_VAL];//4.0.20.361 //4.0.21.12
		memset(sLoyaltyMsg, 0, sizeof(sLoyaltyMsg));
		sprintf_s(sLoyaltyMsg,MAX_FIELD_VAL, "Authorize amount sent in SwipeResult: sVolumeLimit = %.7s,sAuthAmtH = %.5s, sAuthAmtl = %.5s, MacLen = %.5s, MacData = %.40s, sExistLoyaltyLimit = %.1s", 
			cCardSaleExtraDataPlus.CardSaleAll3.extData2.sVolumeLimit, 
			cCardSaleExtraDataPlus.CardSaleAll3.CardSaleAll.extData.sAuthAmt_Msb, 
			cCardSaleExtraDataPlus.CardSaleAll3.CardSaleAll.data.sAuthAmt, 
			cCardSaleExtraDataPlus.DebitExtraData.byLen, 
			cCardSaleExtraDataPlus.DebitExtraData.byData, 
			&(cCardSaleExtraDataPlus.CardSaleAll3.extData4.sExistLoyaltyLimit)); //4.0.31.0
		pInfo->m_pServer->m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,sLoyaltyMsg);

		Str2StrNull((BYTE *)&cCardSaleExtraDataPlus,sizeof(CARD_SALE_EXTRA_DATA4_PLUS),sTmp);		//4.0.19.504 - 47559 //4.0.23.60 109103
	}
	else
	{
		memcpy(&sBuff,sTmpAllocationBuffer,lOlaRecordSize);
	}

	CComBSTR strData(lOlaRecordSize + 2,(LPCSTR)sBuff); 

	delete []sTmpAllocationBuffer;  //4.0.5.0

	if((LOG_BASIC_FLOW_CONTROL)&&(pCmdInfo->lAnswer == CD_MORE_DATA_REQUIRED))
	{
		CString strTmp;
		strTmp.Format("PAPswipeResult bstr len = %ld", strData.Length()); //4.0.20.20	

		pInfo->m_pServer->m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, strTmp);
	}

	//4.0.19.504 - 47559
	BOOL bFireGPIevent = Cmd.GetSrc() == MOBILE_SRV_POS_NUMBER || Cmd.GetSrc() == TERMINAL_SRV_POS_NUMBER;

	if (bFireGPIevent == FALSE) // send XML to GCI
	{
		hRes = pEvent->PAPSwipeResult( Cmd.GetParameter(),
			pCmdInfo->lDeviceNumber,
			pCmdInfo->lSeqNumber,
			pCmdInfo->lAnswer,
			pCmdInfo->lFlags,							
			strData,						
			&lEventRetCode);		
	}
	else
	{
		CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp);
		_Module.m_server.m_cGPIEventMgr.AddPAPSwipeResult(Cmd.GetParameter(),
			pCmdInfo->lDeviceNumber,
			pCmdInfo->lSeqNumber,
			pCmdInfo->lAnswer,
			pCmdInfo->lFlags,
			strResult,
			Cmd.GetSrc());
	}

	if (!_Module.m_server.GetIFSFTerminalSrvInUse()) //4.0.22.191 // CR 444830
		cData.CloseDoc();


	{
		// Discount Debugging log generator
		//PS::PAY_AT_PUMP_INFO Info = {};
		//CPump& Pump = _Module.m_server.GetPump(lPumpNumber);
		//Pump.GetPAPInfo(&Info);
		//_Module.m_server.DumpPaymentCardDiscount(lPumpNumber, Info);
	}
}

void CGCI::ForceZeroComplete(long lPumpNumber)
{
	long p = lPumpNumber - 1;
	OLA_STAT ola;
	CString str;

	_Module.m_server.m_cPumpArray[p].SetInPAKLoyaltyWithComplete(TRUE);
	_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
	ola.m_lSessionNumber = 0; 
	ola.m_byCheckCount = 0;
	
	if (_Module.m_server.m_cFuelLoayaltyPS.m_dwLoyaltyEngineType == EPSILON)
	{
		ola.m_byState = OLA_REQUEST_TO_SEND | SESSION_SET_CUSTOMER_INFO;

		str.Format("ForceZeroPakCompletion flag is ON - SESSION_SET_CUSTOMER_INFO is starting");
	}
	else
	{
		ola.m_byState = OLA_REQUEST_TO_SEND | SESSION_COMPLETE;

		str.Format("ForceZeroPakCompletion flag is ON - SESSION_COMPLETE is starting");
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{
		_LOGMSG.LogOLAMsg(p + 1, ola.m_byState, ola.m_lSessionNumber);
		_LOGMSG.LogMsg("GCI", "SetPumpStatus", lPumpNumber, LOG_PUMP, str);
	}
	_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 

	//_Module.m_server.ClearPAPTrsFlags(lPumpNumber);   //4.0.23.1130// marik12345
}

void CGCI::SetPumpStatusAutoAuthoirze( CPumpStatus &pumpStat, long lPumpNumber )
{
	long p = lPumpNumber-1;
	CMD_STRUCT_AUTHORIZE_LIMIT cmdAuthorize;
	memset( &cmdAuthorize, '0', sizeof(cmdAuthorize) );						//4.0.24.90

	cmdAuthorize.nLevel =(short)_Module.m_server.m_cPumpArray[p].GetPriceLevelDefault();					
	cmdAuthorize.lGradePrice = 0;

	cmdAuthorize.lFlags = LIMIT_FILL_UP;  //4.0.9.705

	//amram add check to cash acceptor 4.0.9.513
	//if we in trs cash than we get the limit from  QDX
	if(pumpStat.GetResExt() == RES_EXT_CASH_ACCEPTOR)
	{
		CPumpTransact trs;  //4.0.10.504
		EXTRA_INFO_REC	ExtraInfoRec;
		_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_CASH_ACCEPTOR_INFO,&trs);
		memset(&ExtraInfoRec , 0 , sizeof(EXTRA_INFO_REC));
		ExtraInfoRec.info.lIndexNumber = trs.m_lNumber;
		ExtraInfoRec.info.sTotalBalance;
		long lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);
		if (!lRtc)
		{
			cmdAuthorize.lMaxVolume =  0;   //4.0.10.16
			cmdAuthorize.lMaxValue = ExtraInfoRec.info.lCashDepositTotal;
		}
	}
	else
	{
		_Module.m_server.SetSiteLimit(p,PRICE_LEVEL_DEFAULT,cmdAuthorize); // 4.0.22.850 TD 307758
	}

	cmdAuthorize.lSyncNumber = 0;	
	memset(cmdAuthorize.GradeAmountLimit, ' ', sizeof(cmdAuthorize.GradeAmountLimit));//4.0.22.506 67786

	if (!_Module.m_server.CheckForLoyaltyDiscount(lPumpNumber ,  cmdAuthorize))
	{
		//4.0.18.10
		if ((cmdAuthorize.lMaxVolume) && (pumpStat.GetRes() != PAY_AT_PUMP))	//55687
			cmdAuthorize.lFlags +=  PAK_WITH_LIMIT;
	}

	if(LOG_BASIC_FLOW_CONTROL)	//4.0.20.380 //4.0.21.12 68431
	{
		CString str;
		str.Format("SetPumpStatus: DiscountType sent to CMD_AUTHORIZE_LIMIT = %.2s", cmdAuthorize.sDiscountType);
		_LOGMSG.LogMsg("GCI","SetPumpStatus",p+1,LOG_PUMP,str);
	}			
	_Module.m_server.SetCommand(CMD_AUTHORIZE_LIMIT,SERVER_POS_NUMBER, p+1, &cmdAuthorize,sizeof(CMD_STRUCT_AUTHORIZE_LIMIT));

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("Auto Authorize Pump - Fill Up ");
		_LOGMSG.LogMsg("GCI","SetPumpStatus",p+1,LOG_PUMP,str);
	}
}

void CGCI::SetPumpStatusTerminalSrvFlow( PumpStatus lNewStatus, PumpReservation currentReservation, long p, BOOL &bAddSetPumpStatusEvent, long &lEventFlags, long &lEventParam1 )
{
	if (_Module.m_server.GetIFSFTerminalSrvInUse())		//4.0.19.504 - 47559
	{
		if ((lNewStatus == READY) && (currentReservation == PRE_PAY_AT_POS))
		{
			CPumpTransact cCurrentTrs;
			BOOL bFound = FALSE;


			if (_Module.m_server.m_cPumpArray[p].GetNextWaitingTrs(&cCurrentTrs))
				bFound = TRUE;
			if(!bFound)
			{
				if (!_Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&cCurrentTrs))
					bFound = TRUE;
			}
			if (bFound)	 
			{
				PRE_PAY_INFO info;
				if(_Module.m_server.TrsLinkOperation(REC_READ, &cCurrentTrs, &info))
				{
					bAddSetPumpStatusEvent = FALSE;		//4.0.26.503 TD 165971

					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("SetPumpStatus IFSF in use, Can't find PrePay TRS link information");
						_LOGMSG.LogMsg(p+1,LOG_PUMP,str);
					}
				}
				else
				{
					lEventFlags = (info.lFlags & (LIMIT_BY_VOLUME | LIMIT_BY_VALUE));						//4.0.26.503 TD 165971
					lEventParam1= (lEventFlags & LIMIT_BY_VOLUME) ? info.lMaxVolume : info.lMaxValue;		//4.0.26.503 TD 165971
				}
			}
		}
	}
}

void CGCI::SetPumpStatusNewRerservaionIsPumpSrv( PumpResExt lNewResExt, long lPumpNumber, PumpReservation lNewRes, CPumpStatus &pumpStat, BOOL &bChange )
{
	long p = lPumpNumber-1;
	if ((lNewResExt != RES_EXT_PRICE_CHANGE) &&  //4.0.13.501
		(UNLOCK ==  _Module.m_server.GetLockStatus() && 
		(FALSE  ==  _Module.m_server.IsLockedByMode(p+1)) && 
		(FALSE  ==  _Module.m_server.IsPumpLockedByTank(p+1)) && /*TD 66935 */
		(FALSE  ==  _Module.m_server.m_cSinglePumpLock.IsExclusivelyLocked(p+1)) &&
		(LOCKED !=  _Module.m_server.m_cPumpArray[lPumpNumber-1].GetLockStatus()) || 
		((WAIT_LOCK == _Module.m_server.GetLockStatus()) && 
		(_Module.m_server.m_cServerInfo.GetRequiredMaintenanceProcessFlags() == MP_FORCED_UNLOCK)))) // no reason  //4.0.5.30
	{
		//Check if there is a pump lock by Single Tank delivery emulation lock  //4.0.21.500 - TD 66935.
		BOOL bDeliverySingleTankActive = FALSE;
		for (int tn = 0; tn < _Module.m_server.GetLastValidTank() ; tn++)
		{
			if((_Module.m_server.m_cServerInfo.GetSingleTankLockStatus(tn+1) == SINGLE_TANK_WAIT_LOCK) ||
				(_Module.m_server.m_cServerInfo.GetSingleTankLockStatus(tn+1) == SINGLE_TANK_LOCK_STARTED))
			{	/*TD 66935 */
				bDeliverySingleTankActive = TRUE;
				break;
			}
		}
		CMD_STRUCT_RELEASE_PUMP cmdInfo;
		cmdInfo.lRes = lNewRes;
		cmdInfo.lResExt = MAINTENANCE; // 4.0.23.1590
		_Module.m_server.SetCommand(CMD_RELEASE_PUMP, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_RELEASE_PUMP));

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("Pump Reserved by PUMPSRV for no reason - Releasing Pump !");
			_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
		}
	}

	if ((_Module.m_server.GetLockResExt() & PRICE_CHANGE )  ||  //4.0.13.501 
		(lNewResExt == RES_EXT_PRICE_CHANGE) || (pumpStat.GetResExt() == RES_EXT_PRICE_CHANGE))
		pumpStat.SetRes(lNewRes,RES_EXT_PRICE_CHANGE);
	else if(_Module.m_server.GetLockResExt() & SHIFT_CHANGE)
		pumpStat.SetRes(lNewRes,RES_EXT_SHIFT_CHANGE);
	else if(_Module.m_server.GetLockResExt() & MAINTENANCE)
		pumpStat.SetRes(lNewRes,RES_EXT_MAINTENANCE);
	else
		pumpStat.SetRes(lNewRes,NULL_RES_EXT);

	bChange = TRUE;
}

void CGCI::SetPumpStatusNewRerservaionIsPrePayAtPos(short nPrePayWaiting, CPumpStatus &pumpStat, PumpReservation lNewRes, PumpResExt lNewResExt, BOOL &bChange, PumpStatus lNewStatus, long lPumpNumber, GCIRetCode &retCode )
{
	 long p = lPumpNumber-1;
	//Check if there is any dummy PrePay transaction waiting
	short nDummyPrePayWaiting = 0; 
	_Module.m_server.m_cPumpArray[p].GetDummyPrePayTrs(&nDummyPrePayWaiting);	//4.0.19.509 (62335)

	if(nPrePayWaiting || nDummyPrePayWaiting) // waiting = WAIT_RES,WAIT_AUTH,WAIT_PROGRESS 
	{
		pumpStat.SetRes(lNewRes,lNewResExt);
		bChange = TRUE;
	}
	else
	{
		PumpReservation lCurrentRes = _Module.m_server.m_cPumpArray[p].GetStat().GetRes();
		PumpStatus lCurrentStat= _Module.m_server.m_cPumpArray[p].GetStat().GetStatus();

		if( ((lCurrentStat == READY) ||(lCurrentStat == UNKNOWN_STATUS))&& 
			((lCurrentRes == NULL_RES) || (lCurrentRes == PRE_PAY_AT_POS))&&
			//in case we fail in middle of dispensing PrePay & we get SetPumpStatus(4,6) instead SetPumpStatus(4, 999) //3.0.0.6
			(!((lCurrentStat == UNKNOWN_STATUS) &&								   
			(lCurrentRes == PRE_PAY_AT_POS) && 
			(lNewStatus == DISPENSING))) && 
			// 
			(lNewStatus != COMMS_TERMINATED) &&   //3.2.0.66
			(lNewResExt != RES_EXT_CASH_ACCEPTOR)  //4.0.3.60
			) 
		{ 
			// not supposed to happen - log error !!

			if(LOG_LOGIC_ERROR)
			{
				CString str;
				str.Format("Trying to release PRE_PAY_AT_POS (none requested reservation)");
				_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
			}

			CMD_STRUCT_RELEASE_PUMP cmdInfo;
			cmdInfo.lRes = lNewRes;
			cmdInfo.lResExt = 0; // unknown
			_Module.m_server.SetCommand(CMD_RELEASE_PUMP, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_RELEASE_PUMP));											
			retCode = GCI_NO_WAITING_TRS;
		}
		else if ((lNewRes == PRE_PAY_AT_POS) && (lNewResExt == RES_EXT_CASH_ACCEPTOR))   //4.0.24.
		{
			pumpStat.SetRes(lNewRes,lNewResExt);
			bChange = TRUE;
		} // 4.0.28.500
	}
}

// 4.0.23.2450
void CGCI::SetPumpStatusNewRerservaionIsNullRes(PumpResExt lPrevResExt, long lPumpNumber, PumpReservation &lNewRes, CPumpStatus &pumpStat, PumpResExt lNewResExt, BOOL &bChange )
{
	long p = lPumpNumber -1;

	BOOL bIsForceZeroPakComplete = _Module.m_server.m_cPumpArray[p].IsForceZeroPakCompletionOn();
	BOOL bIsLoyaltyClub = _Module.m_server.m_cPumpArray[p].IsLoyaltyClub();

	CString sMsg;
	sMsg.Format("CGCI::SetPumpStatusNewRerservaionIsNullRes: IsLoyaltyClub = %d, lPrevResExt = %d, bIsForceZeroPakComplete = %d",bIsLoyaltyClub,lPrevResExt,bIsForceZeroPakComplete);
	_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,sMsg);

	if ((bIsForceZeroPakComplete) && ((bIsLoyaltyClub)) && (lPrevResExt == RES_EXT_PAK_LOYALTY) ) ///4.0.23.2441
	{
		ForceZeroComplete(lPumpNumber);
	}
	else
	{
		// M.L 13.7.98 Detecting final cancel of pending cancel trs
		CPumpTransact trs;
		if( _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_NO_LINK,&trs))
		{
			if ((_Module.m_server.m_cPumpArray[p].IsForceZeroPakCompletionOn()) &&
				(_Module.m_server.m_cPumpArray[lPumpNumber - 1].IsPapToPakTrs()) &&
				!trs.m_lRoundedValue &&
				!_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())    //4.0.1023.2570
			{
				ForceZeroComplete(lPumpNumber);
			}
			else
			{
				lNewRes = PAY_AT_KIOSK;
			}

			if(LOG_BASIC_FLOW_CONTROL & 0)
			{	
				CString str;
				str.Format("Current Res %d (ignore change to NULL_RES !)",lNewRes);
				_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
			}
		}
	}

	pumpStat.SetRes(lNewRes,lNewResExt);
	bChange = TRUE;
}

void CGCI::SetPumpStatusNewRerservaionIsExtAttendant( long lPumpNumber, PumpStatus lNewStatus, PumpReservation lNewRes, GCIRetCode &retCode, CPumpStatus &pumpStat,  short nUnpaid, PumpResExt lNewResExt, BOOL &bChange )
{
	long p = lPumpNumber-1;
	CPumpTransact trs;
	CPumpStatus stat;		//4.0.1.34
	BOOL  bSessionAuthorizeActive = FALSE;
	stat = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();
	if((lNewStatus == READY) && (lNewRes == PAY_AT_KIOSK))//4.0.12.200
	{
		OLA_STAT ola;
		memset(&ola,' ',sizeof(ola));
		_Module.m_server.m_cPumpArray[lPumpNumber -1 ].GetOlaStat(&ola);

		PAY_AT_PUMP_INFO info; 
		memset(&info,' ',sizeof(info));
		_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&info); // 	TD 343588

		if((OLA_STATE(ola.m_byState) != OLA_NONE )&&
			(SESSION_TYPE(ola.m_byState) == SESSION_CLOCK)) //4.0.14.508
			retCode = GCI_IN_MIDDLE_OF_PROCESS;

		else if ((SESSION_TYPE(ola.m_byState) == SESSION_AUTHORIZE) && //4.0.15.504
			((OLA_STATE(ola.m_byState) == OLA_SEND_END)  && 
			(info.CardSaleInfo.cardData.sTranStatus == CD_APPROVE_OK) == FALSE)) // TD 343588 4.0.22.1100
			bSessionAuthorizeActive = TRUE; 

	}

	if(retCode == GCI_OK)
	{
		if (MODE_VALID(stat.GetMode()) && _Module.m_server.m_cModeArray[stat.GetMode() - 1].PayAttendNotValid())
		{
			retCode =  GCI_NOT_VALID_IN_CURRENT_MODE;									
		}
		else if (pumpStat.GetRes() == PAY_AT_PUMP)
		{
			BOOL bFound =FALSE;
			long lNumberOfAttTrs = 0;												 //4.0.22.501 merged from 4.0.22.12 TD 73062
			_Module.m_server.m_cPumpArray[p].GetAttendTrs(0, &trs, lNumberOfAttTrs); //4.0.22.501 merged from 4.0.22.12 TD 73062

			// Sub one for the current trs. 
			if(nUnpaid >= MAX_ALLOWED_UNPAID_TRS(p) )	 //4.0.1.35													
			{
				retCode = GCI_PUMP_TRS_LIST_IS_FULL;
			}						
			else if(lNumberOfAttTrs < _Module.m_server.m_cPumpArray[p].GetMaxAttendantTrsAllowed()) //4.0.22.501 merged from 4.0.22.12 TD 73062
			{			

				if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)  //4.0.15.500
				{
					retCode = GCI_SERVER_IN_MIDDLE_LOCK_SHIFT;   
				}
				else if (bSessionAuthorizeActive)  //4.0.15.504
				{
					retCode = GCI_IN_MIDDLE_OF_PROCESS;
				}

				else
				{
					bFound = _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&trs);
					if (bFound)
					{
						trs.m_lRes = lNewRes;
						trs.m_lResExt = RES_EXT_ATTENDANT;
						trs.SetLink(TRS_LINK_ATTEND , 0);

						//4.0.22.10 - TD 71055 Ella
						PAY_AT_PUMP_INFO Info; 
						memset(&Info,' ',sizeof(Info));
						_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&Info);	
						memcpy(trs.m_sAttendantID, Info.CardSaleExtraData2.sAttendantID, sizeof(Info.CardSaleExtraData2.sAttendantID));
						memcpy(trs.m_sAttendantName, Info.CardSaleExtraData2.sAttendantName, sizeof(Info.CardSaleExtraData2.sAttendantName));
						memcpy(trs.m_sAttendantTag, Info.CardSaleExtraData5.sAttendantTag, sizeof(Info.CardSaleExtraData5.sAttendantTag));		//4.0.25.500 TD 137509

						_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);
					}
					pumpStat.SetRes(lNewRes,lNewResExt);
					bChange = TRUE;
				}

			}
			else
				retCode = GCI_PUMP_TRS_LIST_IS_FULL;							
		}
		else	//4.0.3.30
		{								
			pumpStat.SetRes(lNewRes,lNewResExt);
			bChange = TRUE;							
		}
	}
}

void CGCI::SetPumpStatusNewReservaionIsPayAtPumpOrPayAttendant( PumpReservation lNewRes, PumpStatus lNewStatus, CPumpStatus &pumpStat, PumpResExt lNewResExt, long lPumpNumber, BOOL& bChange, PumpResExt lPrevResExt )
{
	long p = lPumpNumber-1;
	pumpStat.SetRes(lNewRes,lNewResExt);
	CPumpTransact trs;
	//Find the current transaction  4.0.11.503
	if(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_NO_LINK,&trs))
	{
		if(lNewResExt == RES_EXT_LOYALTY_AVI)//4.0.11.503
		{
			trs.m_lResExt =lNewResExt;
			_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);
		}
	}
	bChange = TRUE;

	//////////////////////////////////////////////////
	if ((lNewRes == NULL_RES) && (lNewStatus == READY))
	{
		long lMisc = 0; 
		CPumpTransact trs;

		lMisc = pumpStat.GetMisc();							
		if (lMisc & MISC_CREDIT_CARD_NOT_AUTHORIZED)
		{
			if (_Module.m_server.GetPAPDeclineTimeValue() == 0)
				lMisc &= MISC_NOT_CREDIT_CARD_NOT_AUTHORIZED;

			if(lMisc & MISC_AVI_NOT_AUTHORIZED)//4.0.2.13
				lMisc &= MISC_NOT_AVI_NOT_AUTHORIZED;

			pumpStat.SetMisc(lMisc);
			_Module.m_server.m_cPumpArray[lPumpNumber -1].SetStat(pumpStat);			

		}

		if (lMisc & MISC_CREDIT_CARD_DECLINE)
		{
			if(lMisc & MISC_AVI_NOT_AUTHORIZED)	//4.0.2.13
				lMisc &= MISC_NOT_AVI_NOT_AUTHORIZED;

			pumpStat.SetMisc(lMisc);
			_Module.m_server.m_cPumpArray[lPumpNumber -1].SetStat(pumpStat);			

		}

		if( _Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_NO_LINK,&trs))	//4.0.0.77
		{															
			if(LOG_BASIC_FLOW_CONTROL)
			{	
				CString str;
				str.Format("Found WAIT PROGRESS Transaction1=%ld )",trs.m_lNumber);					//4.0.23.508
				_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);
			}

			if (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())	//4.0.23.504
			{	
				RemoveLoyaltyTrs(lPumpNumber, trs);
			}
			else if ((_Module.m_server.m_cPumpArray[p].IsForceZeroPakCompletionOn()) && ((_Module.m_server.m_cPumpArray[p].IsLoyaltyClub()) || (_Module.m_server.m_cPumpArray[lPumpNumber -1].IsPapToPakTrs()))) ///4.0.23.1130
			{
				OLA_STAT ola;

				_Module.m_server.m_cPumpArray[p].SetInPAKLoyaltyWithComplete(TRUE);
				_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
				ola.m_lSessionNumber = 0; 
				ola.m_byCheckCount = 0;
				ola.m_byState = OLA_REQUEST_TO_SEND | SESSION_COMPLETE;
				if(LOG_BASIC_FLOW_CONTROL)
				{
					_LOGMSG.LogOLAMsg(p+1, ola.m_byState, ola.m_lSessionNumber);
					CString str;
					str.Format("ForceZeroPakCompletion flag is ON - session complete is starting");
					_LOGMSG.LogMsg("GCI","SetPumpStatus",lPumpNumber,LOG_PUMP,str);

				}
				_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 

				//_Module.m_server.ClearPAPTrsFlags(lPumpNumber);   //4.0.23.1130 //marik12345

			}

			_Module.m_server.SetForceSessionCompletePak(p); // TD 278992 4.0.124.472
		}
		else
		{
			if ((_Module.m_server.m_cPumpArray[p].IsForceZeroPakCompletionOn()) && 
				((_Module.m_server.m_cPumpArray[p].IsLoyaltyClub())) && (lPrevResExt == RES_EXT_PAK_LOYALTY) ) ///4.0.23.2440
			{
				ForceZeroComplete(lPumpNumber);

			}
		} 
	}


#if SPEEDPASS_SIMULATOR
	DWORD dwResExt;
	_Module.m_server.m_cParam.LoadParam("Allied","RFID_ResExt", &dwResExt,0,FALSE,FALSE);
	if ( 4 == dwResExt )
		pumpStat.SetRes(lNewRes,RES_EXT_RFID_CT);
	if ( 5 == dwResExt )
		pumpStat.SetRes(lNewRes,RES_EXT_RFID_HT);
	if ( 11 == dwResExt)
		pumpStat.SetRes(lNewRes,RES_EXT_RFID_CT_CANCELED);
	if ( 12 == dwResExt)
		pumpStat.SetRes(lNewRes,RES_EXT_RFID_HT_CANCELED);

#endif	
}

short CGCI::ConvertLoyaltyStatusToOlaStatus( OLA_STAT * pOla, CARD_SALE_ALL3* pCardData, eFuelLoyaltyState eLoyaltyFuel, long lPumpNumber,LOYALTY_FLOW_TYPE &eCurrentFlowType)
{
	if(_Module.m_server.IsAutoReceiptCheck())
		pOla->m_byState = OLA_REQUEST_TO_SEND | SESSION_RECEIPT_AUTH;
	else if (!ChrAll((char *)pCardData->extData2.sClockFlags, sizeof(pCardData->extData2.sClockFlags)))	//4.0.20.503 66960
		pOla->m_byState = OLA_REQUEST_TO_SEND | SESSION_CLOCK;
	else
		pOla->m_byState = OLA_REQUEST_TO_SEND | SESSION_AUTHORIZE;

	short nOlaState = 0;

	eCurrentFlowType = (LOYALTY_FLOW_TYPE) _Module.m_server.GetFlowControlType();

	if (eLoyaltyFuel == PAP_Payment_Swiped)	//4.0.22.500 70767
	{
		nOlaState = OLA_LOYALTY_SWIPE_PAYMENT;
	}
	else if(eLoyaltyFuel == PAK_Loyalty_Swiped)
	{
		nOlaState = OLA_LOYALTY_SWIPE_LOYALTY;
		eCurrentFlowType = PAK_FLOW;
	}
	else if ((eLoyaltyFuel == PAK_Payment_Swiped)|| (eLoyaltyFuel == PAP_PaymentFirst_Loyalty_swiped))	//4.0.22.504 69344
	{
		pOla->m_byState = OLA_NONE;
		PAY_AT_PUMP_INFO cTmpInfo;
		_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);

		_Module.m_server.m_cFuelLoayaltyPS.CancelLoyaltyTrs(cTmpInfo,lPumpNumber,*pOla);

	}
	else if(eLoyaltyFuel == PAP_PaymentFirst_Payment_swiped) 	//Copient 55286
	{
		nOlaState = OLA_LOYALTY_SWIPE_PAYMENT;	//4.0.22.500 70767
		//Continue flow to OLA, don't change OLA state
		//If loyalty was swiped the trs will be canceled by OLA
	}
	else if (eLoyaltyFuel == PAP_Loyalty_Swiped)
	{
		nOlaState = OLA_LOYALTY_SWIPE_LOYALTY;	//4.0.22.500 70767
	}
	else if (eLoyaltyFuel == PAK_No_Card)
	{
		eCurrentFlowType = PAK_FLOW;
	}
	else if (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag())
	{
		nOlaState = 0;
	}

	return nOlaState;
}

void CGCI::PakToPapSwipe( OLA_STAT * pOla, CARD_SALE_ALL3* pCardData, long lPumpNumber, long lSeqNumber )
{
	BOOL  bFound = FALSE;
	CPumpTransact cTrs;
	pOla->m_lSessionNumber = 0; 
	pOla->m_byState = OLA_REQUEST_TO_SEND | SESSION_AUTHORIZE;

	//4.0.20.503 66960
	long lTmp = a2l(pCardData->extData2.sFlags ,sizeof(pCardData->extData2.sFlags)) + FL_DO_NOT_OPEN_TRS +FL_PAK_TO_PAP;   //4.0.3.45
	l2a(lTmp ,pCardData->extData2.sFlags ,sizeof(pCardData->extData2.sFlags)); 
	long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST, lPumpNumber, lSeqNumber, pCardData, 2, FALSE);		//4.0.20.56 //4.0.20.503 66960

	//4.0.3.47
	if (!_Module.m_server.m_cPumpArray[lPumpNumber -1 ].GetCurrentTrs(&cTrs))
		bFound = TRUE;
	else if (_Module.m_server.m_cPumpArray[lPumpNumber -1].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_ANY,&cTrs)) //4.0.5.41
		bFound = TRUE;

	if(bFound)  //4.0.6.502
	{						
		cTrs.SetLink(TRS_LINK_OLA_NUMBER,lSeqNumber);
		cTrs.m_lRes = PAY_AT_PUMP;
		_Module.m_server.CentralUpdateTrs(lPumpNumber, cTrs.m_lNumber,&cTrs,FALSE);
	}
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{											
			CString str;
			str.Format("SwipeRequest Fail to find current transaction, lSeqNumber=%ld", lSeqNumber);
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str ,LOG_LEVEL_0);
		}				

	}
}

void CGCI::ExecuteManualFirstSwipe( long lPumpNumber, CARD_SALE_ALL3* pCardData )
{
	PAY_AT_PUMP_INFO  cTmpInfo;

	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);
	memcpy(cTmpInfo.CardSaleExtraData2.sMediaMasterType, pCardData->extData2.sMediaMasterType, sizeof(cTmpInfo.CardSaleExtraData2.sMediaMasterType));
	memcpy(cTmpInfo.CardSaleInfo.extraData.sLoyalId, pCardData->CardSaleAll.extData.sLoyalId, sizeof(pCardData->CardSaleAll.extData.sLoyalId));
	memcpy(cTmpInfo.CardSaleInfo.cardData.sAccountNumber, pCardData->CardSaleAll.data.sAccountNumber, sizeof(pCardData->CardSaleAll.data.sAccountNumber));
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	CString str1;
	str1.Format("First card was entered manually");
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str1);
}

void CGCI::HandleParitalApprovalScreen( long lPumpNumber, long lSeqNumber, CARD_SALE_ALL3* pCardData, PAY_AT_PUMP_INFO &cInfo, OLA_STAT * pOla )
{
	long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);	

	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cInfo);

	if (cInfo.CardSaleExtraData3.sParamResult[0] == 'N') // continue with authorized amount
	{
		long lAuthrizedAmount = _Module.m_server.GetFirstTenderAuthorizedAmountLimit(lPumpNumber);
		ConvertLongToStrings(lAuthrizedAmount, cInfo.CardSaleInfo.cardData.sAuthAmt, sizeof(cInfo.CardSaleInfo.cardData.sAuthAmt), cInfo.CardSaleInfo.extraData.sAuthAmt_Msb, sizeof(cInfo.CardSaleInfo.extraData.sAuthAmt_Msb));

		char sMsg[MAX_MSG_LEN] = { 0 };
		sprintf_s(sMsg, "Customer chose to continue with amount authorized for the first tender = %d", lAuthrizedAmount);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg);

		if (_Module.m_server.m_cFlowControlManager.IsCarWashAmountBiggerThenAuthoirzedAmount(&cInfo, lPumpNumber, pOla, lAuthrizedAmount)) // TD 467566
		{
			_LOGMSG.LogMsg("CGCI::HandleParitalApprovalScreen - carwash amount > tender limit, canceling trs!");
			_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, &cInfo, CD_DECLINE_AMOUNT);
			return;
		}

		cInfo.CardSaleExtraData6.sPartialApproval = 'N';
		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cInfo);

		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber,pOla,&cInfo); // moving to the next step in the flow.
	}
	else if (_Module.m_server.IsCardSwiped(cInfo.CardSaleInfo.cardData.sEntryMethod))
	{
		long lMediaMasterType = HandleLoyaltyResult(lPumpNumber, pOla, &cInfo, FALSE, FALSE);

		if (lMediaMasterType == CARD_TYPE_INVALID || lMediaMasterType == CARD_TYPE_NONE)
		{
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Unrecognized card!");
			memset(cInfo.CardSaleInfo.cardData.sResultMsg,' ',sizeof(cInfo.CardSaleInfo.cardData.sResultMsg));
			_Module.m_server.CancelLoyaltyTrs(lPumpNumber,pOla,&cInfo);

		}
		else // valid Card
		{
			cInfo.CardSaleExtraData3.sParamResult[0] = ' ';
			// need force complete
			cInfo.CardSaleInfo.extraData.sForceCompleteZero = 'Y'; // 4.0.32.421

			memset(cInfo.CardSaleExtraData3.sParamResult, ' ', sizeof(cInfo.CardSaleExtraData3.sParamResult));
			memset(cInfo.CardSaleExtraData5.sParamResultExt, ' ', sizeof(cInfo.CardSaleExtraData5.sParamResultExt));

			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cInfo);

			pOla->m_byState = 	OLA_REQUEST_TO_SEND | SESSION_AUTHORIZE;
			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(pOla); 
		}
	}
	else //if(cInfo.CardSaleInfo.cardData.sEntryMethod == 'A')
	{
		pOla->m_byState = OLA_LOYALTY_START_FUELING;
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber,pOla,&cInfo); // moving to the next step in the flow.
	}
}

void CGCI::HandlePreAuthorizeScreen( long lPumpNumber, long lSeqNumber, CARD_SALE_ALL3* pCardData, PAY_AT_PUMP_INFO cInfo, OLA_STAT * pOla )
{
	long lRtc = _Module.m_server.ConstructAndSaveCardData(SWIPE_REQUEST,lPumpNumber,lSeqNumber, pCardData);	

	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cInfo);

	_Module.m_server.m_cFlowControlManager.HandlePreAuthoitzeScreen(lPumpNumber,pOla , cInfo , pCardData);
}

void CGCI::SaveMediaMasterResultToPapInfo(PAY_AT_PUMP_INFO &cTmpInfo, PAY_AT_PUMP_INFO &cGetCardPapInfoResult, long lMediaMasterType, long lPumpNumber, CARD_SALE_ALL3 * cardDataAll3)
{
	memcpy(cTmpInfo.CardSaleExtraData2.sMediaMasterType, cGetCardPapInfoResult.CardSaleExtraData2.sMediaMasterType,
		sizeof(cGetCardPapInfoResult.CardSaleExtraData2.sMediaMasterType));

	memcpy(cTmpInfo.CardSaleExtraData4.sPreAuthAmount, cGetCardPapInfoResult.CardSaleExtraData4.sPreAuthAmount,
		sizeof(cTmpInfo.CardSaleExtraData4.sPreAuthAmount));

	memcpy(cTmpInfo.CardSaleInfo.cardData.sAccountNumber, cGetCardPapInfoResult.CardSaleInfo.cardData.sAccountNumber,
		sizeof(cTmpInfo.CardSaleInfo.cardData.sAccountNumber));

	memcpy(&cTmpInfo.CardSaleExtraData3.sCardSegment, &cGetCardPapInfoResult.CardSaleExtraData3.sCardSegment,
		sizeof(cTmpInfo.CardSaleExtraData3.sCardSegment));

	memcpy(cTmpInfo.CardSaleExtraData6.sClubCardHashNumber, cGetCardPapInfoResult.CardSaleExtraData6.sClubCardHashNumber,		// 4.0.32.??? TD 405435
		sizeof(cTmpInfo.CardSaleExtraData6.sClubCardHashNumber));

	memcpy(&cTmpInfo.CardSaleInfo.cardData.sTranType, &cardDataAll3->CardSaleAll.data.sTranType, sizeof(cTmpInfo.CardSaleInfo.cardData.sTranType)); //4.0.20.450 66977
}


void CGCI::SetAutoAuthorizePump(long lPumpNumber, CPumpStatus &pumpStat)
{
	long p = lPumpNumber - 1;
	CMD_STRUCT_AUTHORIZE_LIMIT cmdAuthorize;
	memset(&cmdAuthorize, '0', sizeof(cmdAuthorize));						//4.0.24.90

	cmdAuthorize.nLevel = (short)_Module.m_server.m_cPumpArray[p].GetPriceLevelDefault();
	cmdAuthorize.lGradePrice = 0;

	cmdAuthorize.lFlags = LIMIT_FILL_UP;  //4.0.9.705

	//amram add check to cash acceptor 4.0.9.513
	//if we in trs cash than we get the limit from  QDX
	if (pumpStat.GetResExt() == RES_EXT_CASH_ACCEPTOR)
	{
		CPumpTransact trs;  //4.0.10.504
		EXTRA_INFO_REC	ExtraInfoRec;
		_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS, TRS_LINK_CASH_ACCEPTOR_INFO, &trs);
		memset(&ExtraInfoRec, 0, sizeof(EXTRA_INFO_REC));
		ExtraInfoRec.info.lIndexNumber = trs.m_lNumber;
		ExtraInfoRec.info.sTotalBalance;
		long lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);
		if (!lRtc)
		{
			cmdAuthorize.lMaxVolume = 0;   //4.0.10.16
			cmdAuthorize.lMaxValue = ExtraInfoRec.info.lCashDepositTotal;
		}
	}
	else
	{
		_Module.m_server.SetSiteLimit(p, PRICE_LEVEL_DEFAULT, cmdAuthorize); // 4.0.22.850 TD 307758
	}

	cmdAuthorize.lSyncNumber = 0;
	memset(cmdAuthorize.GradeAmountLimit, ' ', sizeof(cmdAuthorize.GradeAmountLimit));//4.0.22.506 67786

	if (!_Module.m_server.CheckForLoyaltyDiscount(lPumpNumber, cmdAuthorize))
	{
		//4.0.18.10
		if ((cmdAuthorize.lMaxVolume) && (pumpStat.GetRes() != PAY_AT_PUMP))	//55687
			cmdAuthorize.lFlags += PAK_WITH_LIMIT;
	}

	if (LOG_BASIC_FLOW_CONTROL)	//4.0.20.380 //4.0.21.12 68431
	{
		CString str;
		str.Format("SetPumpStatus: DiscountType sent to CMD_AUTHORIZE_LIMIT = %.2s", cmdAuthorize.sDiscountType);
		_LOGMSG.LogMsg("GCI", "SetAutoAuthorizePump", p + 1, LOG_PUMP, str);
	}
	_Module.m_server.SetCommand(CMD_AUTHORIZE_LIMIT, SERVER_POS_NUMBER, p + 1, &cmdAuthorize, sizeof(CMD_STRUCT_AUTHORIZE_LIMIT));

	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("Auto Authorize Pump - Fill Up ");
		_LOGMSG.LogMsg("GCI", "SetAutoAuthorizePump", p + 1, LOG_PUMP, str);
	}

}


int CGCI::SaveTankReading(TANK_READ_INFO *info)//443035
{
	// save LastTankReadingTime to INI file:
	CString sTankINIPath;
	sTankINIPath.Format("Tanks\\Tank%02ld", info->lTank);
	SZ_TAG_SYSTEMTIME	sTmpTime;
	sTmpTime.sNull = 0;
	SystemTime2Tag(&info->cSysTime, &sTmpTime.sTime);

	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)sTankINIPath), "LastTankReadingTime", (char *)&sTmpTime, TRUE, 0, 0, 0, TRUE);

	//save TankReading to QDX:
	long lRtc = 0;
	if (_Module.m_server.m_cServerInfo.GetMemoProcessFlags() == FALSE)
		lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE, REC_INSERT, info);

	if (lRtc == ERR_EXISTS) //if record exists, update it
	{
		lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE, REC_UPDATE, info);
		CString strLogMsg;
		strLogMsg.Format("TankReading already exist (Failure-CIndexFile::Insert), Updating record,Index %d, Retcode=%d", info->lIndexNumber, lRtc);
		_Module.m_server.m_cLogger.LogMsg(strLogMsg);
	}

	return lRtc;
}

void CGCI::GetTankReadingTimeFromINIFile(TANK_READ_INFO *info)//443035
{
	CString sTankINIPath;
	sTankINIPath.Format("Tanks\\Tank%02ld", info->lTank);
	SZ_TAG_SYSTEMTIME	szTempTime;
	memset(&szTempTime, 0, sizeof(SZ_TAG_SYSTEMTIME));
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTankINIPath), "LastTankReadingTime", (char *)&szTempTime, sizeof(SZ_TAG_SYSTEMTIME), (char *)&szTempTime, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	Tag2SystemTime(&szTempTime.sTime, &info->cSysTime);
}
//CR_475695_Start
void CGCI::SetPumpReadingRequestType(short *nFlags)
{
	
	BOOL bIsPriceChangeOn = ( _Module.m_server.m_cServerInfo.GetRequiredPriceChangeFlags() || 
		_Module.m_server.m_cServerInfo.GetServerReservation() == PRICE_CHANGE)  ;
	
	if (TRUE == (_Module.m_server.m_cServerInfo.GetShiftChangeStatus() == CHANGE_IN_PROGRESS))//check all cases
	{
		
		if (_Module.m_server.GetShiftEOD() == 1)	
		{
			*nFlags |= TOTALS_ON_EOD_REQUEST;
			_Module.m_server.m_cLogger.LogMsg("PumpTotals2_ by EOD_REQUEST");
		}
		else
		{
			*nFlags |= TOTALS_READ_ON_EOS_REQUEST;
			_Module.m_server.m_cLogger.LogMsg("PumpTotals2_ by EOS_REQUEST");
		}
		
	}
	else if (TRUE == bIsPriceChangeOn)//after any price change
	{
		*nFlags |= TOTALS_ON_PRICE_CHANGE_REQUEST;
		
		_Module.m_server.m_cLogger.LogMsg("PumpTotals2_ by PRICECHNG_REQUEST");
	}
	else if ((_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_POS)	|| 
		(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_POS_PUMP) )
	{
		*nFlags |= TOTALS_READ_ON_MANUAL_REQUEST;
		_Module.m_server.m_cLogger.LogMsg("PumpTotals2_ by MANUAL_REQUEST");
	}
	else
	{
		*nFlags |= TOTALS_READ_ON_SCHEDULE_CHANGE_REQUEST;
		
		_Module.m_server.m_cLogger.LogMsg("PumpTotals2_ by SCHEDULE_REQUEST");
	}
		
}




void CGCI::SetTankReadingRequestType(short *nFlags)
{
	
	BOOL bIsPriceChangeOn = ( _Module.m_server.m_cServerInfo.GetRequiredPriceChangeFlags() || 
		_Module.m_server.m_cServerInfo.GetServerReservation() == PRICE_CHANGE  );
	
	if (TRUE == (_Module.m_server.m_cServerInfo.GetShiftChangeStatus() == CHANGE_IN_PROGRESS))//check all cases
	{
		
		if (_Module.m_server.GetShiftEOD() == 1)	
		{
			*nFlags |= TANK_READ_ON_EOD_REQUEST;
			_Module.m_server.m_cLogger.LogMsg("TankReading - request type: EOD_REQUEST");
		}
		else
		{
			*nFlags |= TANK_READ_ON_EOS_REQUEST;
			_Module.m_server.m_cLogger.LogMsg("TankReading - request type: EOS_REQUEST");
		}
		
	}
	else if (TRUE == bIsPriceChangeOn)
	{
		*nFlags |= TANK_READ_ON_PRICE_CHANGE_REQUEST;
		
		_Module.m_server.m_cLogger.LogMsg("TankReading - request type: PRICECHNG_REQUEST");
	}
	else if ((_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_POS)
		|| 
		(_Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_REQUESTED_BY_POS_PUMP))
	{
		*nFlags |= TANK_READ_ON_MANUAL_REQUEST;
		
		_Module.m_server.m_cLogger.LogMsg("TankReading - request type: MANUAL_REQUEST");
	}
	else
	{
		*nFlags |= TANK_READ_ON_SCHEDULE_CHANGE_REQUEST;
		
		_Module.m_server.m_cLogger.LogMsg("TankReading - request type: SCHEDULE_REQUEST");
	}
}
//CR_475695_End
STDMETHODIMP CGCI::LockPump(long lPumpNumber, long lLock, long lFlags, long *plVal)
{
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
			_Module.m_server.m_cLogger.LogClassMsg("CGCI", "LockPump", lPumpNumber, LOG_PUMP, "", LOG_LEVEL_0);
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

void CGCI::LockPump__(long lPumpNumber, long lLock, long lFlags, long *plVal)
{
	//We instantiate the return code with GPI_OK by default, and any error
	//which will occur later will be inserted to it. If there's no error- it'll stay
	//with the retCode of GPI_OK till the function's end 
	GCIRetCode retCode = GCI_OK;
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


	sLogStr.Format("CGCI::LockPump lPumpNumber %d, lLock %d, lFlags %d", lPumpNumber, lLock, lFlags);
	_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, sLogStr);

	cPumpStatus = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();
	//Checking if there's any kind of error...
	if (lLock != MP_LOCK &&  lLock != MP_UNLOCK)
		retCode = GCI_NOT_SUPPORTED;

	else if (PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;

	if (SERVER_STATE_STARTUP)
		retCode = GCI_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = GCI_SERVER_NOT_READY;

	else if (PUMP_NOT_VALID(lPumpNumber))
		retCode = GCI_INVALID_PUMP_NUMBER;

	else if (PUMP_NOT_IN_WORKING_STATE(lPumpNumber))
		retCode = GCI_PUMP_NOT_IN_WORKING_STATE;

	else if (MP_LOCK == lLock || MP_EMERGENCY_LOCK == lLock)
	{
		//Dont let lock / unlock when we are in Emergency lock !
		if ((_Module.m_server.m_cServerInfo.GetServerReservation() & EMERGENCY_LOCK) && (MP_LOCK == lLock)) //4.0.21.501
		{
			retCode = GCI_PUMP_LOCKED;
			sLogStr.Format("Can't Lock pump while server is in EMERGENCY_LOCK !");
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, sLogStr);
		}

		else if (cPumpStatus.GetRes() == PUMPSRV)
		{
			retCode = GCI_PUMP_ALREADY_RESERVED;
		}
		else if (cPumpStatus.GetStatus() != REQUESTING)
		{
			if ((cPumpStatus.GetStatus() != READY))
				retCode = GCI_CANT_RESERVEPUMP;
			else if ((cPumpStatus.GetRes() != NULL_RES) && (cPumpStatus.GetRes() != PAY_AT_KIOSK))
			{
				retCode = GCI_CANT_RESERVEPUMP;
			}
		}
	}

	if (((_Module.m_server.m_cServerInfo.GetServerReservation() & MAINTENANCE) || (_Module.m_server.m_cServerInfo.GetServerReservation() & DATABASE_MAINTENANCE)) && (cPumpStatus.GetStatus() != REQUESTING)) //4.0.5550.1504
	{
		retCode = GCI_INVALID_PUMP_MODE;
	}

	sLogStr.Format("CGCI::LockPump passed all conditions with retCode %d", retCode);
	_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, sLogStr);

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
				_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, sLogStr);
				//Changing the state to PUMP_WAIT_TO_BE_EXCLUSIVELY_LOCKED in the state machine
				_Module.m_server.m_cSinglePumpLock.SetPumpStateWaitLocked(lPumpNumber);
			}
			else
			{
				if (!_Module.m_server.m_cSinglePumpLock.IsExclusivelyLocked(lPumpNumber))
				{
					retCode = GCI_OK;
				}
				else
				{
					//Filling the command of releasing a pump and sending it
					cmdReleaseSinglePump.lRes = PUMPSRV;
					cmdReleaseSinglePump.lResExt = IDLE;
					_Module.m_server.SetCommand(CMD_RELEASE_PUMP, SERVER_POS_NUMBER, lPumpNumber, &cmdReleaseSinglePump, sizeof(CMD_STRUCT_RELEASE_PUMP));

					sLogStr.Format("Sent a command to release pump %d\n", lPumpNumber);
					_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, sLogStr);

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
				retCode = GCI_PUMP_ALREADY_RESERVED;
			}
			else
			{
				//Filling the command of releasing a pump and sending it
				cmdReleaseSinglePump.lRes = PUMPSRV;
				cmdReleaseSinglePump.lResExt = IDLE;
				_Module.m_server.SetCommand(CMD_RELEASE_PUMP, SERVER_POS_NUMBER, lPumpNumber, &cmdReleaseSinglePump, sizeof(CMD_STRUCT_RELEASE_PUMP));

				sLogStr.Format("Sent a command to release pump %d\n", lPumpNumber);
				_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, sLogStr);

				//Changing the state to PUMP_WAIT_TO_BE_EXCLUSIVELY_UNLOCKED in the state machine
				_Module.m_server.m_cSinglePumpLock.SetPumpStateWaitUnlocked(lPumpNumber);
			}
		}

		//Leaving the pump array critical section
		if (slPumpArray.IsLocked())
			slPumpArray.Unlock();
	}

	//In case there was an error - we're logging it
	if (retCode)
		if (LOG_LOGIC_ERROR)
			_Module.m_server.m_cLogger.LogResult(
				0, // Src
				"IGCI",
				"LockPump",
				TRUE, retCode,
				FALSE, 0);

	if (plVal != NULL)
		*plVal = retCode;
}

void CGCI::SaveZeroPakTrs(long lPumpNumber, CPumpTransact &trs)
{
	if (_Module.m_server.GetSaveZeroPakTrs())
	{
		PAY_AT_PUMP_INFO	info;
		PAY_AT_PUMP_INFO	infoMem;

		memset(&infoMem, ' ', sizeof(PAY_AT_PUMP_INFO));

		if (lPumpNumber > 0)
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&infoMem);

		memset(&info, ' ', sizeof(PAY_AT_PUMP_INFO));

		info.CardSaleInfo.lIndexNumber = _Module.m_server.NewOLAIndex();
		l2a0(lPumpNumber, info.CardSaleInfo.cardData.sDeviceNumber, sizeof(info.CardSaleInfo.cardData.sDeviceNumber));
		l2a0(trs.m_lNumber, info.CardSaleInfo.cardData.sTranNbr, sizeof(info.CardSaleInfo.cardData.sTranNbr));
		l2a(trs.m_nNozzle, &info.CardSaleInfo.cardData.sNozzle, sizeof(info.CardSaleInfo.cardData.sNozzle));

		Convertl2Str2(trs.m_lRoundedValue,
			info.CardSaleInfo.extraData.sTotalAmt_Msb, sizeof(info.CardSaleInfo.extraData.sTotalAmt_Msb),
			info.CardSaleInfo.extraData.sTotalAmt, sizeof(info.CardSaleInfo.extraData.sTotalAmt));

		Convertl2Str2(trs.m_lRoundedValue,
			info.CardSaleInfo.extraData.sFuelAmt_Msb, sizeof(info.CardSaleInfo.extraData.sFuelAmt_Msb),
			info.CardSaleInfo.extraData.sFuelAmt, sizeof(info.CardSaleInfo.extraData.sFuelAmt));

		info.CardSaleInfo.cardData.sTranStatus = 'A';
		memcpy(info.CardSaleInfo.extraData.sLoyalId, infoMem.CardSaleInfo.extraData.sLoyalId, sizeof(info.CardSaleInfo.extraData.sLoyalId));



		if (LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format(" trs number  = %ld  , member=%.20s ", trs.m_lNumber, infoMem.CardSaleInfo.extraData.sLoyalId);
			_Module.m_server.m_cLogger.LogMsg("GCI", "SetPumpStatus", lPumpNumber, LOG_PUMP, str);
		}



		//Insert to Ola qdx											
		trs.SetLink(TRS_LINK_OLA_NUMBER, info.CardSaleInfo.lIndexNumber);
		trs.m_lStatus = PAID;
		long lRtc = _Module.m_server.TrsLinkOperationEx(REC_INSERT, &trs, &info);


		if (lRtc == OK)
			_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_PAP_TRS, 0);


		CString str;
		str.Format("Save zero pak trs %d", trs.m_lNumber);
		_Module.m_server.m_cLogger.LogMsg("GCI", "SetPumpStatus", lPumpNumber, LOG_PUMP, str);
	}
}

void CGCI::RemoveLoyaltyTrs(long lPumpNumber, CPumpTransact &trs)
{
	long lRtc;
	long p = lPumpNumber - 1;
	int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_INTERNAL, lPumpNumber, trs.m_lNumber, &lRtc); //4.0.23.330 115000

	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("Remove WAIT PROGRESS Transaction. Loyalty trs %d canceled by pumpsrv (return %d)", trs.m_lNumber, iRetNum);
		_LOGMSG.LogMsg("GCI", "SetPumpStatus", lPumpNumber, LOG_PUMP, str);
	}

	_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber, FALSE);

	_Module.m_server.m_cPumpArray[p].ClearOlaStat(); //4.0.23.470 125390

}

// CR 425758
void CGCI::CopyLoyaltyLockingKeyToExtraInfo(TAG_EXTRA_INFO &cTagExtraInfo, PAY_AT_PUMP_INFO &PayAtPumpInfo, long lPumpNumber)
{
	CString sLogMsg;
	if (IsBlankBuffer((char*)cTagExtraInfo.sLockingKey, sizeof(cTagExtraInfo.sLockingKey)))
	{
		if (FALSE == IsBlankBuffer((char*)PayAtPumpInfo.CardSaleExtraData5.sLockingKey, sizeof(PayAtPumpInfo.CardSaleExtraData5.sLockingKey)))
		{
			memcpy(cTagExtraInfo.sLockingKey, PayAtPumpInfo.CardSaleExtraData5.sLockingKey, sizeof(PayAtPumpInfo.CardSaleExtraData5.sLockingKey));
			CopyLoyaltyMemberInfoToExtraInfo(cTagExtraInfo, PayAtPumpInfo, lPumpNumber);
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				sLogMsg.Format("CGCI::CopyLoyaltyLockingKeyToExtraInfo, Loyalty locking key = %.40s was copied to extra info tag", cTagExtraInfo.sLockingKey);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sLogMsg);
			}
		}
	}
}

void CGCI::CopyLoyaltyMemberInfoToExtraInfo(TAG_EXTRA_INFO & cTagExtraInfo, PAY_AT_PUMP_INFO & PayAtPumpInfo, long lPumpNumber)
{
	memcpy(cTagExtraInfo.sPosId, PayAtPumpInfo.CardSaleExtraData6.sPosId,								sizeof(PayAtPumpInfo.CardSaleExtraData6.sPosId));
	memcpy(cTagExtraInfo.sLoyaltyStartDateTime, PayAtPumpInfo.CardSaleExtraData6.sLoyaltyStartDateTime, sizeof(PayAtPumpInfo.CardSaleExtraData6.sLoyaltyStartDateTime));
	memcpy(cTagExtraInfo.sStoreIDField, PayAtPumpInfo.CardSaleExtraData6.sStoreIDField,					sizeof(PayAtPumpInfo.CardSaleExtraData6.sStoreIDField));
	memcpy(cTagExtraInfo.sTicketId, PayAtPumpInfo.CardSaleExtraData6.sTicketId,							sizeof(PayAtPumpInfo.CardSaleExtraData6.sTicketId));
	memcpy(cTagExtraInfo.sTransId, PayAtPumpInfo.CardSaleExtraData6.sTransId,							sizeof(PayAtPumpInfo.CardSaleExtraData6.sTransId));
	memcpy(cTagExtraInfo.sClubType, PayAtPumpInfo.CardSaleExtraData6.sClubType,							sizeof(PayAtPumpInfo.CardSaleExtraData6.sClubType));

	CString sLogMsg;
	sLogMsg.Format("CGCI::CopyLoyaltyMemberInfoToExtraInfo, Loyalty sPosId =  %.10s, sTicketId = %.10s , sTransId = %.10s, sClubType=%.2s", 
		cTagExtraInfo.sPosId, 
		cTagExtraInfo.sTicketId, 
		cTagExtraInfo.sTransId,
		cTagExtraInfo.sClubType);
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sLogMsg);
}


void CGCI::HandleLoyaltyResultCopyLoyaltyData(PAY_AT_PUMP_INFO * pInfo, long &lMediaMasterType, long lPumpNumber)
{
	memcpy(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data, pInfo->CardSaleInfo.extraData.sLoyalId, sizeof pInfo->CardSaleInfo.extraData.sLoyalId);
	l2a(CARD_TYPE_LOYALTY, pInfo->CardSaleExtraData2.sMediaMasterType, sizeof(pInfo->CardSaleExtraData2.sMediaMasterType));
	l2a(_Module.m_server.m_dwLoyaltyClubNumber, pInfo->CardSaleExtraData6.sClubType, sizeof(pInfo->CardSaleExtraData6.sClubType));  //501164
	lMediaMasterType = CARD_TYPE_LOYALTY;
	pInfo->CardSaleInfo.cardData.sEntryMethod = 'M';
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(pInfo);
}



BOOL CGCI::IsInTheMiddleOfOlaSession(OLA_STAT &ola)
{
	if (OLA_STATE(ola.m_byState) != OLA_NONE &&
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_WAIT_FUELING) && //4.0.23.276?
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_SWIPE_PAYMENT) &&  //4.0.23.278)
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_METHOD_OF_PAYMENT) && // RFUEL-559 Wesco - PAP transaction doesn't get canceled until timeout
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_SWIPE_LOYALTY) &&	// RFUEL-577
		(OLA_STATE(ola.m_byState) != OLA_WAIT_CONTROLLER_MSG) &&	// RFUEL-621
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_ALTERNATE_ID) &&
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_ALTERNATE_ID_MANUAL_ENTER) &&
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_ALTERNATE_ID_SWIPE_LOYALTY_CARD) &&
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_ALTERNATE_ID_RETRY) &&
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_PHONE_LOOKUP) && // RFUEL-1959
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_PHONE_LOOKUP_ENTER_PHONE_NUMBER) &&  // RFUEL-1959
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_PHONE_LOOKUP_ENTER_REWARD_CARD) &&
		(OLA_STATE(ola.m_byState) != OLA_LOYALTY_WAIT_FOR_ALTERNATE_ID_RETRY_ANSWER))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CGCI::ShouldUpdatePAKExtraInfo(long lPumpNumber, long lFlags)  //RFUEL - 2463
{
	BOOL bIsActiveLoyaltyDiscount = _Module.m_server.IsActiveLoyaltyDiscount();
	BOOL  bLoyaltySrvInUseIsCOPIENT = (_Module.m_server.m_cFuelLoayaltyPS.GetLoyaltySrvInUse() == COPIENT);
	BOOL  bIsLoyaltyClub = _Module.m_server.m_cPumpArray[lPumpNumber].IsLoyaltyClub();
	BOOL  bIsPapToPakTrs = _Module.m_server.m_cPumpArray[lPumpNumber].IsPapToPakTrs();
/*	BOOL  bIsIncludesExtraInfo = (lFlags & PAK_INCLUDE_EXTRA_INFO_REC);     //RFUEL - 2463*/
	BOOL bRet =	 (((bIsActiveLoyaltyDiscount || bLoyaltySrvInUseIsCOPIENT ) &&  bIsLoyaltyClub) || 
		(/*bIsIncludesExtraInfo || RFUEL - 2463*/bIsPapToPakTrs)); //4.0.23.220 105272 BP //4.0.24.60
	return bRet;
}

void CGCI::SetPromotionId(CPumpTransact &cTrs, TAG_EXTRA_INFO &cTagExtraInfo, PAY_AT_PUMP_INFO &PayAtPumpInfo) // RFUEL-2501
{
	long lGradePos = _Module.m_server.GetGradePositionRestrection(cTrs.m_nGrade, cTrs.m_lPump) - 1; 
	if (lGradePos >= 0 && lGradePos < MAX_POSITIONS_PER_PUMP)
	{
		memset(cTagExtraInfo.sPromotionID, ' ', sizeof(cTagExtraInfo.sPromotionID));
		memcpy(cTagExtraInfo.sPromotionID, PayAtPumpInfo.CardSaleExtraData6.sGradeDiscountID[lGradePos], sizeof(PayAtPumpInfo.CardSaleExtraData6.sGradeDiscountID[lGradePos]));

		CString str;
		str.Format("SetPromotionId() - lGradePos = %ld, sPromotionID = %.15s", lGradePos, cTagExtraInfo.sPromotionID);
		_LOGMSG.LogMsg(str);
	}


}