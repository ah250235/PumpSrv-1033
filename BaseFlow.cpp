#include "stdafx.h"
#include "BaseFlow.h"
#include "phoneLookUpNmpDetalis.h"
#include "DeferralDiscountNMPDetails.h"
#include "CopientCommandExecuter.h"
#include "AuthorizeLoyaltyNMPDetails.h"
#include "AuthorizePaymentNMPDetails.h"
#include "SwipePaymentNMPDetails.h"
#include "RunQueryNMPDetails.h"
#include "FuelingNMPDetails.h"
#include "MethodOfPaymentNMPDetails.h"
#include "ManualLoyaltyNMPDetails.h"
#include "LoyaltyNMPDetails.h"
#include "PromptNMPDetails.h"
#include "TargetMessageNMPDetails.h"
#include "CarWashDiscountNMPDetails.h"
#include "ApplyCalcDiscountNMPDetail.h"

#include "NMPFactory.h"

/*************************************************/
/*********          CBaseFlow          ***********/
/*************************************************/

/******************************************************************************
 Description: Delete previous saved NMP configuration.
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			02/04/08   15:38		Start 4.0.19.508
 Liat			26/05/09	11:03		Update 4.0.22.508 74810
******************************************************************************/

void CBaseFlow::ClearOldSetup()
{
	try
	{
		for (int j = 0; j < NMP_MAX_TYPE_OPTIONS; j++)
		{
			if (m_psNMPRegistryEntries[j])
			{
				delete m_psNMPRegistryEntries[j];
				m_psNMPRegistryEntries[j] = NULL;
				m_psOrderNMP[j] = NULL;
			}
		}

		// cannot call Log as this object and logger are both owned by CServerMain,
		//  construction and destruction order is not guaranteed so log may not be properly constructed
		//  by the time this code runs
		//CString  strMsg;
		//strMsg.Format("ClearOldSetup, flow order was cleared");
		//_LOGMSG.LogMsg(strMsg);	
	}
	catch (CException* e)
	{
			char sErrorMsg[256] = { 0 };
			char sMsg[512] = { 0 };
			e->GetErrorMessage(sMsg, sizeof(sMsg));
			sprintf_s(sMsg, sizeof(sMsg), "EXCEPTION : CBaseFlow, ClearOldSetup, %s", sErrorMsg);
			_LOGMSG.LogMsg(sMsg);
	}
}

/******************************************************************************
 Description: Delete previous saved NMP configuration.
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			18/02/09   18:00		Start 4.0.22.504 69344
 Liat			26/05/09	11:05		Update 4.0.22.508 74810
******************************************************************************/

void	CBaseFlow::ClearFlowOrder()
{
	for (int j = 0 ; j <NMP_MAX_TYPE_OPTIONS; j++)
	{
		m_psOrderNMP[j] = NULL;
	}

	// The object gets created from registration process 
	// may not have _logger properly initialized
	//CString  strMsg;
	//strMsg.Format("ClearFlowOrder, flow order was cleared");
	//_LOGMSG.LogMsg(strMsg);
}


BOOL	CBaseFlow::Init(BOOL bReconfigure)
{
	if (!bReconfigure) {
		// Reconfigure also clear out Flow Order
		ClearOldSetup();
	}
	else {
		ClearFlowOrder();
	}

	return TRUE;
}

short CBaseFlow::GetNextNMPSession(long lPumpNumber, PAY_AT_PUMP_INFO * pInfo, long lCurrentState, BOOL bSkipRepeatOption, CNMPDetails ** psConfigDetails, BOOL bStartFueling)
{
	// set this to true to get detailed debug log
	// The most of the debugging line would show at log level 5
	const bool bDetailDebug = LOG_ALL_FLOW_CONTROL;

	short lNextState = 0;
	CString sMsg;

	// Epsilon
	if ((lCurrentState == OLA_LOYALTY_START_NMP_BEFORE_FUELING_AFTER_START_TICKET) ||
		((bStartFueling == TRUE) && (lCurrentState == OLA_LOYALTY_SWIPE_LOYALTY)))
	{
		lCurrentState = OLA_LOYALTY_AUTHORIZE_LOYALTY;
	}
	else if ((lCurrentState == OLA_LOYALTY_ALTERNATE_ID_MANUAL_ENTER) ||
		((lCurrentState == OLA_LOYALTY_ALTERNATE_ID) || 
		(lCurrentState == OLA_LOYALTY_ALTERNATE_ID_SWIPE_LOYALTY_CARD) || 
		(lCurrentState == OLA_LOYALTY_SWIPE_LOYALTY)))
	{
		lCurrentState = OLA_LOYALTY_SWIPE_LOYALTY;
	}

	if (m_psOrderNMP[0] == NULL)
	{
		lNextState = GetNextStateAfterFlowControl();
		sMsg.Format("FindNextNMPSession() -  GetNextStateAfterFlowControl() returned %s", _LOGMSG.GetStateString(lNextState));
		_LOGMSG.LogMsg(sMsg);
	}
	else if (lCurrentState == 0) //4.0.23.860 212541
	{
		lNextState = m_psOrderNMP[0]->GetState();
		*psConfigDetails = (m_psOrderNMP[0]);

		sMsg.Format("FindNextNMPSession() -  CurrentState 0. GetStateType() returned %s", _LOGMSG.GetStateString(lNextState));
		_LOGMSG.LogMsg(sMsg);
	}
	else if ((lCurrentState & SESSION_GET_CUSTOMER_INFO) &&
		(lCurrentState & OLA_LOYALTY_SWIPE_LOYALTY))
	{
		long lLoyaltyPromptRetry = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetLoyaltyPromptRetry();

		if (lLoyaltyPromptRetry > 1)
		{
			lCurrentState = OLA_LOYALTY_AUTHORIZE_LOYALTY;
		}
		else
		{
			lCurrentState = OLA_LOYALTY_SWIPE_LOYALTY;

			lLoyaltyPromptRetry++;

			_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetLoyaltyPromptRetry(lLoyaltyPromptRetry);
		}

		sMsg.Format("GetNextNMPSession() - lPumpNumber %ld - lLoyaltyPromptRetry %ld", lPumpNumber, lLoyaltyPromptRetry);
		_LOGMSG.LogMsg(sMsg);
	}





	for (int j = 0; (j < NMP_MAX_TYPE_OPTIONS) && (m_psOrderNMP[j] != NULL); j++)
	{
		// Find the current State
		if (m_psOrderNMP[j]->GetState() == lCurrentState)
		{
			if (bDetailDebug)
			{
				// RFUEL-2817 - Added Debugging Log Message
				sMsg.Format("GetNextNMPSession() - Found currentStateNum=%d", j);
				_LOGMSG.LogMsg(sMsg);
			}

			if ((m_psOrderNMP[j]->GetRepeat() > 1) && (!bSkipRepeatOption))	//4.0.19.508
			{
				if (m_psOrderNMP[j]->GetCurrentLoopNum() < m_psOrderNMP[j]->GetRepeat())
				{
					m_psOrderNMP[j]->SetCurrentLoopNum(m_psOrderNMP[j]->GetCurrentLoopNum() + 1);
					lNextState = m_psOrderNMP[j]->GetState();
					*psConfigDetails = (m_psOrderNMP[j]);

					sMsg.Format("FindNextNMPSession() - GetRepeat %ld, CurrentLoopNum %ld. GetStateType() returned %ld", m_psOrderNMP[j]->GetRepeat(), m_psOrderNMP[j]->GetCurrentLoopNum(), lNextState);
					_LOGMSG.LogMsg(sMsg);

					break;
				}
				else
				{
					m_psOrderNMP[j]->SetCurrentLoopNum(1);	//4.0.19.508
				}
			}

			//checking the next state to move to
			CNMPDetails* pNextDetail = m_psOrderNMP[j + 1];
			if (bDetailDebug)
			{
				// RFUEL-2817 - Added Debugging Log Message
				sMsg.Format("GetNextNMPSession() - Found NextState=%d", j+1);
				_LOGMSG.LogMsg(sMsg);
				if (pNextDetail == NULL)
				{
					sMsg.Format("GetNextNMPSession() - SessionDetailobj NULL");
					_LOGMSG.LogMsg(sMsg);
				}
			}

			if ((j + 1 < NMP_MAX_TYPE_OPTIONS) && (pNextDetail))
			{
				m_psOrderNMP[j]->SetCurrentLoopNum(1); //4.0.19.508
				if (bDetailDebug)
				{
					sMsg.Format("GetNextNMPSession() - Pulling Next State from the Session Detail obj");
					_LOGMSG.LogMsg(sMsg);
				}
				// Gets next state
				lNextState = pNextDetail->GetState();

				// Gets next state detail
				*psConfigDetails = pNextDetail;
				break;
			}
			else
			{
				if (bDetailDebug)
				{
					sMsg.Format("GetNextNMPSession() - calling GetNextStateAfterFlowControl()");
					_LOGMSG.LogMsg(sMsg);
				}

				lNextState = GetNextStateAfterFlowControl();
				break;
			}
		}
	}

	if (bDetailDebug)
	{
		// RFUEL-2817 - Added Debugging Log Message
		const CString StateTo = _LOGMSG.GetStateString(lNextState);
		const CString StateFrom = _LOGMSG.GetStateString(lCurrentState);
		sMsg.Format("GetNextNMPSession() - Exit CurrentState=%s NextState=%s", StateFrom, StateTo);
		_LOGMSG.LogMsg(sMsg);
	}

	return lNextState;
}


//4.0.22.508 start 74810
CBaseFlow::CBaseFlow():bCarWashAfterFueling(FALSE), bPresentDeferralDiscountMsg(FALSE),m_bPhoneConfigured(FALSE) // 4.0.23.1220 155754
{
	for (int j = 0 ; j < NMP_MAX_TYPE_OPTIONS; j++)
	{
		m_psNMPRegistryEntries[j] = NULL;
		m_psOrderNMP[j] = NULL;
	}

	//
	// This is a object Fasico going on between CBaseFlow Object(static to CServerMain)
	// and the m_cLogger.  Unfortuantly in C++ you cannot define order of static variable creation/destruction
	// and m_cLogger gets cleaned up BEFORE CBaseFlow object, which makes it impossible to 
	// call the server m_cLogger object from the constructor or destructor
	//  Either the logger has to be allocated in heap or CBaseFlow object must be allocated in heap
	//  to control the cleaning order and neither of them are easy change
	//
	//CString str = "CBaseFlow::ctor, flow order was cleared";
	//_LOGMSG.LogMsg(str);
}

CBaseFlow::~CBaseFlow()
{
	// prevent throwing exception out of destructor for any reason
	try {
		ClearOldSetup();
	} catch(...) {}
}



/******************************************************************************
 Description: Load the NMP entries and set the constant values according to their 
	point of time in the flow.
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			02/04/08   15:38		Start 4.0.19.508
 Liat			24/05/09   10:22		Update 4.0.22.508 74810
******************************************************************************/
void	CBaseFlow::LoadNMPConfiguration(BOOL bReconfigure, char *sInPath)	
{
	// this function gets alled for PAK and PAP, called twice
	// LoadNMPRegistryEntries is virtual function that builds the order array
	char sPath[MAX_PATH] = {0};
	try
	{
		if (!bReconfigure)	//4.0.22.504 69344	 for reconfigure don't create the objects, only read the data from the registry
		{
			for (int iNMP = 0; iNMP<NMP_MAX_TYPE_OPTIONS; iNMP++)
			{
//  The old construction
//				memset(sPath, 0, sizeof(sPath));
//				memcpy(sPath, sInPath, min(strlen(sInPath), MAX_PATH));
//				CNMPDetails* pNMPDetails = CreateNMPDetail(iNMP, sPath); //4.0.22.504	69344   //Pumpsrv crushes on stop via Fcc since this pointer was defined 				
//				if (pNMPDetails)
//				{
//					pNMPDetails->SetNMPType((enum NMP_TYPE) iNMP);
//					pNMPDetails->SetPath(sPath);		//4.0.22.504	69344
//					m_psNMPRegistryEntries[iNMP] = pNMPDetails;
//				}

//  new construction
				CNMPDetails* pNMPDetails = CNMPFactory::Inst().Create((enum NMP_TYPE)iNMP, sInPath);

				if (pNMPDetails)
				{					
					m_psNMPRegistryEntries[iNMP] = pNMPDetails;
				}
			}
		}
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","LoadNMPConfiguration",0,LOG_PUMP,"Unexpected Error Had occurred",LOG_LEVEL_1);
	}
	//}
	LoadNMPRegistryEntries();	

	if (LOG_ALL_FLOW_CONTROL)
	{
		// Debugging Message
		_LOGMSG.LogMsg("CBaseFlow::LoadNMPConfiguration Exit");
		DumpOrderNMNP();
	}
}









/******************************************************************************
 Description: Build the NMP array according to the flow type (pak/pap).
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			02/04/08   15:38		Start 4.0.19.508
 Liat			24/05/09	10:43		Update 4.0.22.508 74810
******************************************************************************/

void CBaseFlow::LoadNMPRegistryEntries()	//4.0.22.500
{
	DWORD dwValid = 0;
	DWORD dwTmp = 0;
	DWORD dwOrder = 0, dwFuelingOrder = 0;	//4.0.22.500
	char sPath[MAX_PATH] = {0};
	CString strMsg;

	char* szNMPNames[NMP_MAX_TYPE_OPTIONS] = {
		"Car Wash", 
		"Coupon", 
		"Swipe Loyalty", 
		"Prompt", 
		"Deferral Discount", 
		"Swipe Payment", 
		"Authorize Payment", 
		"Authorize Loyalty", 
		"Fueling", 
		"Method of payment", 
		"Run Query", 
		"Phone Lookup"
		"Car Wash Discount", 
		"Alternate ID", 
		"Apply Tender Discount"
	}; //4.0.23.860 212541 4.0.23.1220 TD 155754  RFUEL-2817, added Apply Tender Discount, reorder Phone Lookuup

	// Existing code had missing items/out of index entries for szNMPNames
	// Added this so that when future developer adds new item, assertion should trigger
	// so that he/she also need to set szNMPNames properly.  This is used for debugging log entries	
	constexpr int CountOfNames = _countof(szNMPNames);
	constexpr int NMPCount = NMP_MAX_TYPE_OPTIONS;
	static_assert(NMPCount == CountOfNames, "NMP_MAX_TYPE_OPTIONS does not match szNMPNames lookup table");

	//"ApplyCalculateDiscount"
	if (LOG_DETAIL_FLOW_CONTROL)
	{
		_LOGMSG.LogMsg("Loading need more param configuration from the registry.");
	}

	for (int j = 0 ; j < NMP_MAX_TYPE_OPTIONS; j++)
	{
			if (dwOrder = GetNMPOrder(m_psNMPRegistryEntries[j]->GetPath()))
			{
				if (LOG_DETAIL_FLOW_CONTROL)
				{
					strMsg.Format("Found requested NMP type %d = %s, order = %ld", j, szNMPNames[j], dwOrder);	//4.0.22.500
					_LOGMSG.LogMsg(strMsg);
				}
				m_psOrderNMP[dwOrder - 1] = m_psNMPRegistryEntries[j];	//Order starts from 1, so we need to decrease it when saving to the array

				if(m_psOrderNMP[dwOrder - 1]->GetState() == OLA_LOYALTY_PHONE_LOOKUP)
					m_bPhoneConfigured = TRUE; // 4.0.23.1220 TD 155754 
			}

			m_psNMPRegistryEntries[j]->LoadConfiguration(); // 4.0.23.???? TD 29523
	}

	//update car wash time
	//4.0.22.500 start
	if (m_psNMPRegistryEntries[NMP_TYPE_CarWash]->GetValidEntry())
	{
		dwOrder = GetNMPOrder(m_psNMPRegistryEntries[NMP_TYPE_CarWash]->GetPath());
		dwFuelingOrder = GetNMPOrder(m_psNMPRegistryEntries[NMP_TYPE_Fueling]->GetPath());	//fueling always has to be valid in the flow.

		if (dwOrder > dwFuelingOrder)
			bCarWashAfterFueling = TRUE;
		else
			bCarWashAfterFueling = FALSE;
	}
	else	//4.0.19.508
		bCarWashAfterFueling = FALSE;

	UpdateQueryOrderInTheFlow();  //4.0.23.860 212541
};

void CBaseFlow::UpdateQueryOrderInTheFlow()	//4.0.23.860 212541
{
	DWORD dwTmp = 0;
	DWORD dwOrder = 0, dwFuelingOrder = 0;	//4.0.22.500
	if (m_psNMPRegistryEntries[NMP_TYPE_DeferralDiscount]->GetValidEntry())
	{
		//Put the query before the deferral if exists
		dwOrder = GetNMPOrder(m_psNMPRegistryEntries[NMP_TYPE_DeferralDiscount]->GetPath());
		for (int i = NMP_MAX_TYPE_OPTIONS - 1; i >= (int)dwOrder; i--)
		{
			m_psOrderNMP[i] = m_psOrderNMP[i - 1];
		}

		if (dwOrder >= 1) // CID_198687 Out-of-bounds write
			m_psOrderNMP[dwOrder - 1] = m_psNMPRegistryEntries[NMP_TYPE_RunQuery];

	}
	else
	{
		//if no deferral exists in the flow- call query before fueling
		dwFuelingOrder = GetNMPOrder(m_psNMPRegistryEntries[NMP_TYPE_Fueling]->GetPath());	//fueling always has to be valid in the flow.
		m_psOrderNMP[dwFuelingOrder] = m_psNMPRegistryEntries[NMP_TYPE_Fueling];
		m_psOrderNMP[dwFuelingOrder - 1] = m_psNMPRegistryEntries[NMP_TYPE_RunQuery];
	}

}



DWORD	CBaseFlow::GetNMPOrder(char * sPath)	//4.0.22.508 74810
{
	DWORD dwValid = 0;
	DWORD dwOrder = 0;
	CString strMsg;

	if ( !ChrAll(sPath, MAX_PATH, 0) ) // NO TD 4.0.27.500
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_VALID_ENTRY,&dwValid,0,FALSE);	//4.0.22.504

	if (dwValid)	//4.0.19.509
	{
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_ORDER,&dwOrder,0,TRUE);
		if (dwOrder > NMP_MAX_TYPE_OPTIONS)	//4.0.22.500
		{
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				strMsg.Format("GetNMPOrder Invalid order for NMP, Order = %ld", dwOrder);
				_LOGMSG.LogMsg(strMsg);
			}
			return 0;
		}
	}

	return dwOrder;
}

//4.0.22.508 74810
void	CBaseFlow::HandleNMPSessions(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo, BOOL bSkipRepeatOption, BOOL bStartFueling /* = FALSE */)
{
	if(pOla->m_byState == OLA_LOYALTY_START_NMP_BEFORE_FUELING_BEFORE_START_TICKET)
	{
		UpdateCarWashTime(*pInfo, lPumpNumber);
	}

	HandleNMPWaitingSessions(lPumpNumber, pOla, pInfo, bSkipRepeatOption, bStartFueling);
}

void CBaseFlow::HandleNMPRegularSessions(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo)
{
	long p = lPumpNumber - 1;
	short iCurrentState = 0;
	CNMPDetails * psConfigDetails = NULL;
	

	_Module.m_server.m_cPumpArray[p].GetOlaStat(pOla);
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "HandleNMPRegularSessions() - Looking for current NMP function");

	iCurrentState = FindCurrentNMPSession(pOla->m_byState, &psConfigDetails);

	if (psConfigDetails)
	{
		CString sMsg;
		sMsg.Format("HandleNMPRegularSessions, Calling need more param type = %d", psConfigDetails->GetNMPType()); //4.0.22.500
		_LOGMSG.LogMsg(sMsg);
		psConfigDetails->Execute(*pInfo, lPumpNumber, *pOla);
	}
	else
		_LOGMSG.LogMsg("HandleNMPRegularSessions, Failed to find need more param function to run.");
	
}


/******************************************************************************
 Description: Finding the next NMP session or the next flow state to run and run 
	its Execute method.. 
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			02/04/08   15:38		Start 4.0.19.508
 Liat			26/05/09	10:14		Update 4.0.22.508 74810
******************************************************************************/
void	CBaseFlow::ExecuteNMPRegularSessions(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo)
{
	CString sMsg;
	long p = lPumpNumber -1;
	short iCurrentState = 0;
	CNMPDetails * pNmpDetails = NULL;

	_Module.m_server.m_cPumpArray[p].GetOlaStat(pOla);

	iCurrentState = FindCurrentNMPSession(pOla->m_byState, &pNmpDetails);

	if (pNmpDetails)   
	{
		CString sMsg;
		sMsg.Format("HandleNMPRegularSessions, Calling need more param type = %d", pNmpDetails->GetNMPType()); //4.0.22.500
		_LOGMSG.LogMsg(sMsg);
		pNmpDetails->Execute(*pInfo, lPumpNumber, *pOla);
	}
	else
	{
		_LOGMSG.LogMsg("HandleNMPRegularSessions, Failed to find need more param function to run.",LOG_LEVEL_1);
	}
}

//4.0.22.508 74810
void CBaseFlow::HandleNMPWaitingSessions(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo, BOOL bSkipRepeatOption, BOOL bStartFueling /* = FALSE */)
{	
	const long PumpIndex = lPumpNumber -1;
	short iNextState = 0;
	CNMPDetails * psConfigDetails = NULL;
	CString sMsg;

	_Module.m_server.m_cPumpArray[PumpIndex].GetOlaStat(pOla);
	CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[PumpIndex].GetStat();

	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Looking for next NMP function");

	const short CurrentState = pOla->m_byState;
	iNextState = GetNextNMPSession(lPumpNumber, pInfo, CurrentState, bSkipRepeatOption, &psConfigDetails, bStartFueling);	//4.0.22.504	69344

	if (iNextState)   
	{
		_Module.m_server.m_cFuelLoayaltyPS.PrintLoyaltyStateToLog(lPumpNumber , pOla->m_byState , iNextState);
		pOla->m_byState = iNextState;
		_Module.m_server.m_cPumpArray[PumpIndex].SetOlaStat(pOla);

		if(IsNeedToSkipDeferralStep(iNextState, lPumpNumber))
		{
			SkipDeferralStep(psConfigDetails, iNextState, pInfo, pOla, bSkipRepeatOption, lPumpNumber);
		}

		if (psConfigDetails)
		{
			psConfigDetails->Execute(*pInfo, lPumpNumber, *pOla);
			//ExecuteNMPRegularSessions(lPumpNumber, pOla, pInfo);	//4.0.22.504	69344
		}
		else
			_Module.m_server.m_cPumpArray[PumpIndex].SetPAPInfo(pInfo);	//4.0.19.508
	}
	else
	{
		sMsg.Format("CBaseFlow::HandleNMPWaitingSessions(), Failed to find next need more param state. Last known state = %s", 
			_LOGMSG.GetStateString(pOla->m_byState));

		_LOGMSG.LogMsg(sMsg);

	}
}

/******************************************************************************
 Description: Returns the next state the flow should continue with after the flow control states. 
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			27/05/09   14:53		Start 4.0.22.508 74810
******************************************************************************/
short	CBaseFlow::GetNextStateAfterFlowControl() //4.0.23.860 212541
{
	short iNextState = 0;
	if (m_psOrderNMP[0] == NULL)
	{
		iNextState = OLA_LOYALTY_START_FUELING;
	}
	else
	{
		iNextState = OLA_LOYALTY_END_TICKET;
	}
	return iNextState;

}



/******************************************************************************
 Description: Finding the next NMP session or the next flow state to run. 
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			02/04/08   15:38		Start 4.0.19.508
 Liat			25/05/09	9:53		Update 4.0.22.508 74810
******************************************************************************/

//4.0.22.508 74810
short	CBaseFlow::FindCurrentNMPSession(long lCurrentState, CNMPDetails ** psConfigDetails)
{//4.0.22.500
	short iCurrentState = 0;
	for (int j = 0 ; (j < NMP_MAX_TYPE_OPTIONS) && (m_psOrderNMP[j] != NULL); j++)
	{
		if (m_psOrderNMP[j]->GetState() == lCurrentState)
		{
				iCurrentState = m_psOrderNMP[j]->GetState();
				*psConfigDetails = (m_psOrderNMP[j]);
				break;
				//return iCurrentState;
		}
	}
	return iCurrentState;
}


//4.0.22.508 74810
void	CBaseFlow::UpdateCarWashTime(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber)	
{
	if (bCarWashAfterFueling)
	{
		//4.0.20.10
		if (a2l(cTmpInfo.CardSaleInfo.extraData.sWashCode, sizeof(cTmpInfo.CardSaleInfo.extraData.sWashCode)))
		{
			l2a(0, cTmpInfo.CardSaleExtraData4.sCarWashOption, sizeof(cTmpInfo.CardSaleExtraData4.sCarWashOption));
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "NO Car wash after fueling");
		}
		else
		{
			l2a(CAR_WASH_AFTER_FUELING, cTmpInfo.CardSaleExtraData4.sCarWashOption, sizeof(cTmpInfo.CardSaleExtraData4.sCarWashOption));
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Car wash will be after fueling");

		}
	}
	else //4.0.20.10
	{
		l2a(0, cTmpInfo.CardSaleExtraData4.sCarWashOption, sizeof(cTmpInfo.CardSaleExtraData4.sCarWashOption));
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"NO Car wash after fueling");
	}
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);
}

//4.0.20.54 64528 //4.0.22.508 74810
char*	CBaseFlow::GetDeferralDiscountMsg()	
{//4.0.22.500
	return m_psNMPRegistryEntries[NMP_TYPE_DeferralDiscount]->GetMessage();
}


BOOL	CBaseFlow::IsNMPStateBeforeStartTicket(long lState)	//4.0.22.500	//4.0.22.504 69344
{
	CNMPDetails * psConfigDetails = NULL;
	BOOL bRes = FALSE;
	if (lState)
	{
		bRes = FALSE;

	}
	else
	{
		bRes = TRUE;
	}

	return bRes;

}
BOOL CBaseFlow::IsNMPStateAfterStartTicket(long lState)	//4.0.22.500
{
	CNMPDetails * psConfigDetails = NULL;
	BOOL bRes = FALSE;
	if (lState)
	{
		long lCurrentState = FindCurrentNMPSession(lState, &psConfigDetails);

		if (psConfigDetails)
		{
			bRes = TRUE;
		}
		else
		{
			bRes = FALSE;
		}
	}
	else
	{
		bRes = FALSE;
	}

	return bRes;
}

/******************************************************************************
 Description: Finding the next step in the flow after starting the loyalty ticket 
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			08/02/09	16:00		Start 4.0.22.504	69344
 Liat			26/05/09	9:47		Update 4.0.22.508 74810
******************************************************************************/

void CBaseFlow::StartAfterStartTicketFlowControl(long lPumpNumber, OLA_STAT * pOla, long lStartTicketRes)	//4.0.22.504	69344
{
	CString str;
	OLA_STAT ola;
	PAY_AT_PUMP_INFO cInfo;
	long p = lPumpNumber-1;

	if (!pOla)
		pOla = &ola;

	CPump& oPump = _Module.m_server.GetPump(lPumpNumber);

	oPump.GetOlaStat(pOla);
	oPump.GetPAPInfo(&cInfo);

	if (lStartTicketRes)	//Start ticket failed
	{
		StartTicketFail(lPumpNumber,pOla);
	}
	else
	{
		short iStartOlaNMPState = _Module.m_server.m_cFlowControlManager.GetStartOlaNMPState(lPumpNumber); // TD 359309
		if (iStartOlaNMPState != 0)
		{
			pOla->m_byState = iStartOlaNMPState;
			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(pOla);
			_Module.m_server.SavePumpInfo(lPumpNumber);
			str.Format("StartAfterStartTicketFlowControl, first state after start ticket is ");
			str += _LOGMSG.GetStateString(pOla->m_byState); 
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
			HandleNMPSessions(lPumpNumber, pOla, &cInfo, FALSE);
		}
		else
		{
			CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();

			if (m_psOrderNMP[0])
			{
				pOla->m_byState = m_psOrderNMP[0]->GetState();
				str.Format("StartAfterStartTicketFlowControl, first flow state is "); 
				str +=_LOGMSG.GetStateString(pOla->m_byState); 
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
			}
			else
			{
				pOla->m_byState = m_psNMPRegistryEntries[NMP_TYPE_Fueling]->GetState();
				str.Format("StartAfterStartTicketFlowControl, missing first flow entry. Fueling will be the next step."); 
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
			}

			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(pOla);
			_Module.m_server.SavePumpInfo(lPumpNumber);
			ExecuteNMPRegularSessions(lPumpNumber, pOla, &cInfo);
		}
	}
}

//4.0.22.508 74810
void CBaseFlow::StartAfterQueryFlowControl(long lPumpNumber, OLA_STAT & ola, PAY_AT_PUMP_INFO & cTmpInfo)	//4.0.22.504	69344
{
	//We should start fueling now
	CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();
	long p = lPumpNumber - 1;
	BOOL bCheckDeferral = TRUE;

	if ((ola.m_byState == OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET) || ((ola.m_byState == OLA_LOYALTY_WAIT_FUELING) && ((_Module.m_server.m_cPumpArray[lPumpNumber-1].IsPapToPakTrs()) || (cPumpStatus.GetRes() == PAY_AT_KIOSK) || (cPumpStatus.GetResExt() == RES_EXT_PAK_LOYALTY)))) //4.0.23.860 212541
		bCheckDeferral = FALSE;	//deferral was already checked

	//4.0.23.860 212541
	if (bCheckDeferral)
	{
		ola.m_byState = OLA_LOYALTY_RUN_QUERY;
		_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
	}

	_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cTmpInfo, TRUE);
}

//4.0.22.508 74810
void CBaseFlow::StartAfterAddMemberFlowControl(long lPumpNumber, OLA_STAT * pOla, long lMemberRes)	//4.0.22.504	69344
{
	long p = lPumpNumber-1;
	CString sMsg;
	OLA_STAT ola;
	PAY_AT_PUMP_INFO cTmpInfo;

	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);//TD 349768

	try
	{
		if (!pOla)
		{
			pOla = &ola;
			_Module.m_server.m_cPumpArray[lPumpNumber-1].GetOlaStat(pOla);
		}
		sMsg.Format("StartAfterAddMemberFlowControl: ola state = 0x%x", pOla->m_byState);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg);
		_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);
		
		//TD 349768 - if OLA_NONE - means transaction was canceled ==> do nothing
		if((_Module.m_server.m_cProtectedDataHandler.EncodeForTrs(OLA_LOYALTY_AUTHORIZE_LOYALTY, &cTmpInfo) == TRUE) && FALSE == _Module.m_server.IsAfterOrInTheMiddleCancel(pOla))	 // TD 356872
		{
			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);		//4.0.23.20
	
			if ((lMemberRes) && (lMemberRes != LOYALTY_STATE_NO_LOYALTY))	//4.0.22.504 69344 //4.0.23.390 116544
			{
				LoyaltyDecline(lPumpNumber , lMemberRes, pOla, cTmpInfo);
			}
			else if(_Module.m_server.m_cFlowControlManager.GetPumpSrvAddMemberCardRes(lPumpNumber) > 0 && _Module.m_server.m_cFlowControlManager.GetPumpSrvAddMemberCardRes(lPumpNumber) != LOYALTY_STATE_UNAVALIABLE_ERROR)//4.0.23.390 116544
			{
				// Continue the flow from swipe loyalty
				
				if(m_bPhoneConfigured)
					pOla->m_byState = OLA_LOYALTY_PHONE_LOOKUP;
				else
					pOla->m_byState = OLA_LOYALTY_SWIPE_LOYALTY; // 4.0.23.1220 TD 155754 
	
				_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(pOla);
				_Module.m_server.SavePumpInfo(lPumpNumber);

				sMsg.Format("CFuelLoyaltyPS::StartAfterAddMemberFlowControl, Start NMP after Swipe loyalty before fueling, AddMember result = %d", _Module.m_server.m_cFlowControlManager.GetPumpSrvAddMemberCardRes(lPumpNumber)); //4.0.22.509 76060
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
				
				HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, TRUE);
			}
			else
			{
				//Continue the flow from authorize loyalty
				pOla->m_byState = OLA_LOYALTY_AUTHORIZE_LOYALTY;
				_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(pOla);
				_Module.m_server.SavePumpInfo(lPumpNumber);

				sMsg.Format("CFuelLoyaltyPS::StartAfterAddMemberFlowControl, Start NMP after AddMember before fueling, AddMember result = %d", lMemberRes); //4.0.22.509 76060
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);

				HandleNMPSessions(lPumpNumber, pOla, &cTmpInfo, TRUE);
			}
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","CreatePumpsInitXML",0,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
	}
}


// 4.0.23.1220 155754
void CBaseFlow::LoadLoyaltyErrorMsg(LoyaltyState_ eLoyaltyState ,  char * sOutMsg , int iLen , long &lScreenNumber) const 
{
	CString sKey = "";
	CString sDefaultMsg = "";
	switch(eLoyaltyState)
	{
		case LOYALTY_STATE_PHONE_NUMBER_NOT_FOUND:
		{
			sKey = "PhoneNotFound";
			sDefaultMsg = "Phone Number Not Found | continue?";
			break;
		}
		case LOYALTY_STATE_MULTIPLE_MEMBER_FOUND:
		{
			sKey = "MutipleMemberFound";
			sDefaultMsg = "See Cashier to continue loyalty promotion  | continue with out promotion ?";
			break;
		}
		case LOYALTY_STATE_LOYALTY_SERVER_DOWN:
		{
			sKey = "ServerDown";
			sDefaultMsg = "Details Not Found | continue?";
			break;
		}
		case LOYALTY_STATE_PUNCHH_ERROR:
		{
			sKey = "PhoneNotFound";
			sDefaultMsg = "Phone Number Not Found | continue?"; //test.....
			break;
		}
		case LOYALTY_STATE_EMERALD_SERVER_DOWN:
		{
			sKey = "ServerDown";
			sDefaultMsg = "Details Not Found | continue?"; 
			break;
		}
		default:
		{
			sKey = "LoyaltyDeclineMsg";
			sDefaultMsg = "Loyalty card |was declined, |continue??";
			lScreenNumber = SCREEN_LOYALTY_CARD_DECLINED;
		}
	}
	// CID_198275 String not null terminated
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", (LPCSTR) (sKey.GetBuffer(0)), sOutMsg, iLen, (LPCSTR) (sDefaultMsg.GetBuffer(0)), TRUE);
}

void CBaseFlow::LoyaltyDecline( long lPumpNumber , long lMemberRes, OLA_STAT * pOla, PAY_AT_PUMP_INFO cTmpInfo ) 
{
	// set the prompt details
	CPromptNMPDetails cPromptNMP;
	
	// load the error message
	char sMessage[MAX_FIELD_VAL] = { 0 };
	long lScreenNumber = 0;
	LoadLoyaltyErrorMsg((LoyaltyState_)lMemberRes, sMessage, sizeof(sMessage), lScreenNumber);
	cPromptNMP.SetMessage(sMessage);

	if (lScreenNumber != 0)
		cPromptNMP.SetScreenNumber(lScreenNumber);

	cPromptNMP.SetStateType(OLA_WAIT_CONTROLLER_MSG);
	// pOla->m_byState = OLA_WAIT_CONTROLLER_MSG;

	pOla->m_byState = OLA_NEW_STATE(pOla->m_byState, OLA_WAIT_CONTROLLER_MSG);
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);
	_Module.m_server.SavePumpInfo(lPumpNumber);
	//RFUEL-2673 PAP with loyalty card - Flow for Scan Member card when Loyalty is Offline is wrong
	cPromptNMP.Execute(cTmpInfo, lPumpNumber, *pOla, PT_GENERIC, 1, ONMP_OpenYesNoKeys | ONMP_NoAsCancel /*| ONMP_OpenCardReader*/); // TD 456792 QC 461948

	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetOlaStat(pOla);

	CString sMsg;
	sMsg.Format("CBaseFlow::LoyaltyDecline() Pump %ld - Screen number %d was prompted, AddMember result = %d, OLA state = %s",
		lPumpNumber,
		SCREEN_LOYALTY_CARD_DECLINED, 
		lMemberRes,
		_LOGMSG.GetStateString(pOla->m_byState));
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
}


void CBaseFlow::StartTicketFail( long lPumpNumber ,OLA_STAT *pOla )
{
	_Module.m_server.m_cFuelLoayaltyPS.ClearTimer(lPumpNumber - 1);

	PAY_AT_PUMP_INFO cInfo;
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cInfo);

	CString str;
	long p = lPumpNumber - 1;
	CPromptNMPDetails cPromptNMP;
	cPromptNMP.SetScreenNumber(SCREEN_LOYALTY_OFFLINE);
	cPromptNMP.SetStateType(OLA_LOYALTY_WAIT_CONTROLLER_MSG_START_TICKET_FAIL);
	pOla->m_byState = OLA_LOYALTY_WAIT_CONTROLLER_MSG_START_TICKET_FAIL;

	//char sMessage[MAX_FIELD_VAL] = {0};
	//_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","ServerDeclineMsg",sMessage,sizeof(sMessage),"No connection |to loyalty server, |continue?",1);
	//cPromptNMP.SetMessage(sMessage);

	_Module.m_server.m_cPumpArray[p].SetOlaStat(pOla);
	cPromptNMP.Execute(cInfo, lPumpNumber, *pOla);

	str.Format("CBaseFlow::StartAfterStartTicketFlowControl, Loyalty declined message was prompted for pump %d", lPumpNumber);
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
}

void CBaseFlow::SkipDeferralStep(CNMPDetails * &psConfigDetails, short &iNextState, PAY_AT_PUMP_INFO * pInfo, OLA_STAT * pOla, BOOL bSkipRepeatOption, long lPumpNumber)
{
	//in case of no deferral discount, the message should not be presented - move to next state
	psConfigDetails = NULL;
	iNextState = GetNextNMPSession(lPumpNumber, pInfo, pOla->m_byState, bSkipRepeatOption, &psConfigDetails);	//4.0.22.504	69344

	if (iNextState)
	{
		_Module.m_server.m_cFuelLoayaltyPS.PrintLoyaltyStateToLog(lPumpNumber , pOla->m_byState , iNextState);
		pOla->m_byState = iNextState;
		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(pOla);
	}
	else
	{
		CString sMsg; 
		sMsg.Format("CBaseFlow::HandleNMPWaitingSessions() - Last known state = %s", _LOGMSG.GetStateString(pOla->m_byState));
		_LOGMSG.LogMsg(sMsg);
	}
}

BOOL CBaseFlow::IsNeedToSkipDeferralStep( short iNextState, long lPumpNumber )
{
	return (((iNextState == OLA_LOYALTY_DEFERRAL_AFTER_FUELING_BEFORE_ADD_ITEM) || 
		(iNextState == OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET) || 
		(iNextState == OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_BEFORE_START_TICKET)) && 
		(_Module.m_server.m_cFuelLoayaltyPS.GetDeferralDiscountForPump(lPumpNumber) == 0));	//4.0.22.504 69344
}

//long CBaseFlow::GetDeferralState()
//{
//	for (int j = 0; (j < NMP_MAX_TYPE_OPTIONS) && (m_psOrderNMP[j] != NULL); j++)
//	{
//		if (m_psOrderNMP[j]->GetStateType() == OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET)
//		{
//			return m_psOrderNMP[j - 1]->GetStateType();
//		}
//
//	}
//
//	return 0;
//}

//short GetNextNMPSession(PAY_AT_PUMP_INFO *pInfo, long lCurrentState, BOOL bSkipRepeatOption, CNMPDetails ** psConfigDetails, BOOL bStartFueling = FALSE) 
//{
//	short lNextState = 0;
//	CString sMsg;
//
//	if ((lCurrentState == OLA_LOYALTY_START_NMP_BEFORE_FUELING_AFTER_START_TICKET) ||
//		((bStartFueling == TRUE) && (lCurrentState == OLA_LOYALTY_SWIPE_LOYALTY)))	// Epsilon
//		lCurrentState = OLA_LOYALTY_AUTHORIZE_LOYALTY;
//
//	if (m_psOrderNMP[0] == NULL)
//	{
//		lNextState = GetNextStateAfterFlowControl();
//		sMsg.Format("FindNextNMPSession() -  GetNextStateAfterFlowControl() returned %s", _LOGMSG.GetStateString(lNextState));
//		_LOGMSG.LogMsg(sMsg);
//	}
//	else if (lCurrentState == 0) //4.0.23.860 212541
//	{
//		lNextState = m_psOrderNMP[0]->GetStateType();
//		*psConfigDetails = (m_psOrderNMP[0]);
//
//		sMsg.Format("FindNextNMPSession() -  CurrentState 0. GetStateType() returned %s", _LOGMSG.GetStateString(lNextState));
//		_LOGMSG.LogMsg(sMsg);
//	}
//
//	for (int j = 0; (j < NMP_MAX_TYPE_OPTIONS) && (m_psOrderNMP[j] != NULL); j++)
//	{
//		if (m_psOrderNMP[j]->GetStateType() == lCurrentState)
//		{
//			if ((m_psOrderNMP[j]->GetRepeat() > 1) && (!bSkipRepeatOption))	//4.0.19.508
//			{
//				if (m_psOrderNMP[j]->GetCurrentLoopNum() < m_psOrderNMP[j]->GetRepeat())
//				{
//					m_psOrderNMP[j]->SetCurrentLoopNum(m_psOrderNMP[j]->GetCurrentLoopNum() + 1);
//					lNextState = m_psOrderNMP[j]->GetStateType();
//					*psConfigDetails = (m_psOrderNMP[j]);
//
//					sMsg.Format("FindNextNMPSession() - GetRepeat %ld, CurrentLoopNum %ld. GetStateType() returned %ld", m_psOrderNMP[j]->GetRepeat(), m_psOrderNMP[j]->GetCurrentLoopNum(), lNextState);
//					_LOGMSG.LogMsg(sMsg);
//
//					break;
//				}
//				else
//				{
//					m_psOrderNMP[j]->SetCurrentLoopNum(1);	//4.0.19.508
//				}
//			}
//			//checking the next state to move to
//			if ((j + 1 < NMP_MAX_TYPE_OPTIONS) && (m_psOrderNMP[j + 1]))
//			{
//				m_psOrderNMP[j]->SetCurrentLoopNum(1); //4.0.19.508
//				lNextState = m_psOrderNMP[j + 1]->GetStateType();
//				*psConfigDetails = (m_psOrderNMP[j + 1]);
//				break;
//			}
//			else
//			{
//				lNextState = GetNextStateAfterFlowControl();
//				break;
//			}
//		}
//	}
//
//	return lNextState;
//}


// This function is going to be Obsolete, copied to a function to preserve the logic
//CNMPDetails* CBaseFlow::CreateNMPDetail(int iNMP, char sPath[MAX_PATH])
//{
//	CNMPDetails* pNMPDetails = NULL;
//
//	switch (iNMP)
//	{
//	case NMP_TYPE_MethodOfPayment:		//4.0.23.20 76383
//	{
//		pNMPDetails = new (CMethodOfPaymentNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new MethodOfPayment NMP class.", LOG_LEVEL_1);
//			break;
//		}
//
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_METHOD_OF_PAYMENT_ENTRY_NAME);
//
//	}
//	break;
//	case NMP_TYPE_PaymentCard:
//	{
//		pNMPDetails = new (CSwipePaymentNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new Payment NMP class.", LOG_LEVEL_1);
//
//			break;
//		}
//
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_PAYMENT_ENTRY_NAME);
//
//	}
//	break;
//	case NMP_TYPE_AuthorizePayment:
//	{
//		pNMPDetails = new (CAuthorizePaymentNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new AuthorizePayment NMP class.", LOG_LEVEL_1);
//
//			break;
//		}
//
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_AUTHORIZE_PAYMENT_ENTRY_NAME);
//
//	}
//	break;
//	case NMP_TYPE_AuthorizeLoyalty:
//	{
//		pNMPDetails = new (CAuthorizeLoyaltyNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new AuthorizeLoyalty NMP class.", LOG_LEVEL_1);
//
//			break;
//		}
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_AUTHORIZE_LOYALTY_ENTRY_NAME);
//
//	}
//	break;
//	case NMP_TYPE_Fueling:
//	{
//		pNMPDetails = new (CFuelingNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new Fueling NMP class.", LOG_LEVEL_1);
//
//			break;
//		}
//
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_FUELING_ENTRY_NAME);
//
//	}
//	break;
//	case NMP_TYPE_RunQuery:	//4.0.23.860 212541
//	{
//		pNMPDetails = new (CRunQueryNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new Query NMP class.");
//
//			break;
//		}
//
//		memset(sPath, 0, sizeof(sPath)); // NO TD 4.0.27.500 - fixes Memory overrun
//
//	}
//	break;
//	case NMP_TYPE_CarWash:
//	{
//		pNMPDetails = new (CCarWashNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new car wash NMP class.", LOG_LEVEL_1);
//
//			break;
//		}
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_CAR_WASH_ENTRY_NAME);
//	}
//	break;
//	case NMP_TYPE_CarWashDiscount:	//4.0.23.950 216343
//	{
//		pNMPDetails = new (CCarWashDiscountNMPDetails);
//		if (NULL == pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new car wash discount NMP class.", LOG_LEVEL_1);
//
//			break;
//		}
//
//		pNMPDetails->SetStateType(OLA_LOYALTY_CAR_WASH_DISCOUNT);
//
//		strcat(sPath, "\\");
//		strcat(sPath, NMP_CAR_WASH_DISCOUNT_ENTRY_NAME);
//	}
//	break;
//	case NMP_TYPE_LoyaltyCard:
//	{
//		pNMPDetails = new (CLoyaltyNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new Loyalty NMP class.", LOG_LEVEL_1);
//
//			break;
//		}
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_LOYALTY_ENTRY_NAME);
//	}
//	break;
//	case NMP_TYPE_Coupon:
//	{
//		pNMPDetails = new (CCouponsNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new Coupons NMP class.", LOG_LEVEL_1);
//
//			break;
//
//		}
//
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_COUPON_ENTRY_NAME);
//	}
//	break;
//	case NMP_TYPE_MessagePrompt:
//	{
//		pNMPDetails = new (CPromptNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new prompt NMP class.", LOG_LEVEL_1);
//			break;
//		}
//
//		pNMPDetails->SetStateType(OLA_LOYALTY_PROMPT_BEFORE_FUELING_BEFORE_START_TICKET);
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_PROMPT_ENTRY_NAME);
//	}
//	break;
//	case NMP_TYPE_DeferralDiscount:	//4.0.20.54 64528
//	{
//		pNMPDetails = new (CDeferralDiscountNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new deferral discount.", LOG_LEVEL_1);
//
//			break;
//		}
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_DEFERRAL_ENTRY_NAME);
//	}
//	break;
//	case NMP_TYPE_PhoneLookUp:	// 4.0.23.1220 155754
//	{
//		pNMPDetails = new (CPhoneLookUpNMPDetails);
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a new phone lookup NMP class.", LOG_LEVEL_1);
//			break;
//		}
//
//		pNMPDetails->SetStateType(OLA_LOYALTY_PHONE_LOOKUP);
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_PHONE_LOOKUP_ENTRY_NAME);
//	}
//	break;
//	case NMP_TYPE_TargetMessage:  // CR 312183
//	{
//		pNMPDetails = new CTargetMessageNMPDetails();
//
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_Module.m_server.m_cLogger.LogMsg("LoadNMPConfiguration Failed to create a Target Message.");
//			break;
//
//		}
//
//		pNMPDetails->SetStateType(OLA_LOYALTY_TARGET_MESSAGE);
//		strcat(sPath, "\\");
//		strcat(sPath, NMP_TARGET_MESSAGE_ENTRY_NAME);
//	}
//	break;
//
//
//	// RFUEL-2817
//	case NMP_TYPE_ApplyCalculateDiscount:
//	{
//		pNMPDetails = new ApplyCalcDiscountNMPDetail();
//		if (!pNMPDetails)
//		{
//			if (LOG_DETAIL_FLOW_CONTROL)
//				_LOGMSG.LogMsg("LoadNMPConfiguration Failed to create a ApplyCalcDiscountNMPDetail NMP class.", LOG_LEVEL_1);
//			break;
//		}
//
//		strcat_s(sPath, MAX_PATH, "\\");
//		strcat_s(sPath, MAX_PATH, NMP_APPLY_CALCULATE_DISCOUNT_ENTRY_NAME);
//	}
//	break;
//
//	}
//
//	return pNMPDetails;
//}


void CBaseFlow::DumpOrderNMNP() const {
	CString sMsg;
	for (int j = 0; j < NMP_MAX_TYPE_OPTIONS; j++) {
		if (m_psOrderNMP[j] == NULL) {
			sMsg.Format("%s - m_psOrderNMP[%d] = NULL", __FUNCTION__, j);
			_LOGMSG.LogMsg(sMsg);
		}
		else {
			sMsg.Format("%s - m_psOrderNMP[%d] = NMPType[%d] PATH=[%s]",
				__FUNCTION__,
				j,
				(int)m_psOrderNMP[j]->GetNMPType(),
				m_psOrderNMP[j]->GetPath());

			_LOGMSG.LogMsg(sMsg);
		}
	}
}


