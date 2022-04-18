#include "stdafx.h"
#include "DynamicPapFlow.h"
#include "RunQueryNMPDetails.h"
#include "FuelingNMPDetails.h"
#include "PromptNMPDetails.h"
#include "CarWashSaleNMPDetalis.h"
#include "PreAuthorizeScreen.h"
#include "AuthorizeLoyaltyNMPDetails.h"
#include "ParitalApproveNMPDetalis.h"
#include "CardDeclineNMPDetails.h"
#include "GetMemberInfoNmpDetalis.h"
#include "SwipePaymentNMPDetails.h"
#include "MethodOfPaymentNMPDetails.h"
#include "AltrenateIDNMPDetails.h"


BOOL CDynamicPapFlow::Init(BOOL bReconfigure )
{
	m_CurrentNmpStateObject = new CSwipeCardNMP();

	m_cTreeFlowChart.Init();

	m_lLoyaltySwiped = FALSE;

	m_bLoyaltyServerOnline = TRUE;

	return TRUE;
}

CDynamicPapFlow::CDynamicPapFlow(long lPumpNumber) : m_lPumpNumber(lPumpNumber)
{
	m_CurrentNmpStateObject = NULL;

	Init();
}

CDynamicPapFlow::~CDynamicPapFlow()
{
	if(m_CurrentNmpStateObject)
		delete m_CurrentNmpStateObject;
}

short CDynamicPapFlow::GetNextNMPSession(long lPumpNumber, PAY_AT_PUMP_INFO *pInfo, long lCurrentState, BOOL bSkipRepeatOption, CNMPDetails ** psConfigDetails, BOOL bStartFueling /* = FALSE */)
{	
	long lMediaMasterType = _Module.m_server.m_cFlowControlManager.GetMediaMasterType(m_lPumpNumber);

	BOOL bVaildInput = m_cTreeFlowChart.IsValidInput(lMediaMasterType);
	
	if((lCurrentState == OLA_LOYALTY_SWIPE_CARD || lCurrentState == OLA_LOYALTY_INVALID_INPUT) && bVaildInput == FALSE)
	{
		CString sMsg("This card is not acceptable|");
		sMsg += m_CurrentNmpStateObject->GetMessage();
		m_CurrentNmpStateObject->SetMessage(sMsg.GetBuffer(0));
	}
	else
	{
		while (MoveToTheNextState(lCurrentState));
	}

	*psConfigDetails = m_CurrentNmpStateObject;

	if(m_CurrentNmpStateObject)
		return m_CurrentNmpStateObject->GetState();
	else 
		return 0;
}

short CDynamicPapFlow::FindCurrentNMPSession(long lCurrentState, CNMPDetails ** psConfigDetails )
{
	*psConfigDetails = m_CurrentNmpStateObject;
	return m_CurrentNmpStateObject->GetState();
}

void CDynamicPapFlow::StartFlowControl(long lPumpNumber, short iStartOlaNMPState, OLA_STAT & ola )
{
	if(IsPumpNumberNotValid(lPumpNumber))
		return;

	ClearLastFlow();

	Init();

	StartTicket(lPumpNumber, iStartOlaNMPState, ola);
}

BOOL CDynamicPapFlow::MoveToTheNextState(long lCurrentState)
{
	BOOL bLoop = FALSE;
	long lMediaMasterType = _Module.m_server.m_cFlowControlManager.GetMediaMasterType(m_lPumpNumber);

	BOOL bLoyalytyCard = (_Module.m_server.m_cCommonFunction.IsLoyaltytCard(lMediaMasterType) || lMediaMasterType == CARD_TYPE_COMBO);

	BOOL bPaymentCard = (_Module.m_server.m_cCommonFunction.IsPaymentCard(lMediaMasterType) ||(lMediaMasterType == CARD_TYPE_INVALID) || (lMediaMasterType == CARD_TYPE_NONE));	 // RFUEL-381

	CString strMsg;
	strMsg.Format("CDynamicPapFlow::MoveToTheNextState lCurrentState = 0x%x , lMediaMasterType = %ld , bLoyalytyCard = %ld , bPaymentCard = %ld"
		, lCurrentState
		, lMediaMasterType
		, bLoyalytyCard
		, bPaymentCard);
	_LOGMSG.LogMsg(strMsg);

	if ((FALSE == IsVaildCard(lMediaMasterType) && (lMediaMasterType != CARD_TYPE_INVALID)))
	{
		delete m_CurrentNmpStateObject;
		m_CurrentNmpStateObject = new CCardDeclineNMPDetails();
		return bLoop;
	}
	else if (bPaymentCard && lCurrentState == OLA_LOYALTY_SWIPE_PAYMENT)
	{
		delete m_CurrentNmpStateObject;
		m_CurrentNmpStateObject = new CAuthorizeCardNMP();
	}
	else if ((lCurrentState == OLA_LOYALTY_ALTERNATE_ID_MANUAL_ENTER) ||
		(bLoyalytyCard && ((lCurrentState == OLA_LOYALTY_ALTERNATE_ID) || (lCurrentState == OLA_LOYALTY_ALTERNATE_ID_SWIPE_LOYALTY_CARD) || (lCurrentState == OLA_LOYALTY_SWIPE_LOYALTY))))
	{
		delete m_CurrentNmpStateObject;
		m_CurrentNmpStateObject = new CAuthorizeCardNMP();
	}
	else if (bLoyalytyCard && lCurrentState == OLA_LOYALTY_SWIPE_PAYMENT)
	{
		delete m_CurrentNmpStateObject;
		m_CurrentNmpStateObject = new CSwipePaymentNMPDetails();
	}
	else if (lCurrentState == OLA_LOYALTY_START_FUELING)
	{
		delete m_CurrentNmpStateObject;
		m_CurrentNmpStateObject = new CFuelingNMPDetails();
	}

	else
	{
		TERMINAL_CONFIGURATION cTerminalConfiguration;
		short nOlaState = m_cTreeFlowChart.GetNextState(m_lPumpNumber, lMediaMasterType, cTerminalConfiguration);

		if (nOlaState == OLA_LOYALTY_METHOD_OF_PAYMENT)
		{
			delete m_CurrentNmpStateObject;
			m_CurrentNmpStateObject = new CMethodOfPaymentNMPDetails();
		}
		else if (nOlaState == OLA_LOYALTY_START_FUELING)
		{
			delete m_CurrentNmpStateObject;
			m_CurrentNmpStateObject = new CFuelingNMPDetails();
		}
		else if (nOlaState == OLA_LOYALTY_ALTERNATE_ID)
		{
			delete m_CurrentNmpStateObject;
			m_CurrentNmpStateObject = new CAlternateIDNMPDetails();
		}
		else if (nOlaState == OLA_LOYALTY_SWIPE_PAYMENT)
		{
			delete m_CurrentNmpStateObject;
			m_CurrentNmpStateObject = new CSwipePaymentNMPDetails();
		}

		SetTerminalConfiguration(cTerminalConfiguration);
	}
	return bLoop;
}


void CDynamicPapFlow::SetTerminalConfiguration(TERMINAL_CONFIGURATION cTerminalConfiguration)
{
	m_CurrentNmpStateObject->SetMessage(cTerminalConfiguration.sMessage.GetBuffer());
	m_CurrentNmpStateObject->SetNmpDetails(cTerminalConfiguration.nSettings);   
	m_CurrentNmpStateObject->SetScreenNumber(cTerminalConfiguration.lScreenNumber); 
}


void CDynamicPapFlow::ClearLastFlow()
{
	if(m_CurrentNmpStateObject)
		delete m_CurrentNmpStateObject;
}

void CDynamicPapFlow::StartTicket(long lPumpNumber, short iStartOlaNMPState, OLA_STAT &ola )
{
	_Module.m_server.m_cFlowControlManager.SetStartOlaNMPState(lPumpNumber, iStartOlaNMPState); // TD 359309
	_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_COMPLETE);//4.0.23.390 116544

	ola.m_byState = OLA_LOYALTY_START_TICKET | SESSION_AUTHORIZE;	//Start ticket is always the first step.
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(&ola);
	_Module.m_server.SavePumpInfo(lPumpNumber);
}



BOOL CDynamicPapFlow::IsPumpNumberNotValid(long lPumpNumber)
{
	if (_Module.m_server.PumpNotValid(lPumpNumber))
	{
		_LOGMSG.LogClassMsg("CDynamicPapFlow", "StartFlowControl", lPumpNumber, LOG_PUMP, "Invalid pump number!");
		return TRUE;
	}

	return FALSE;
}

void CDynamicPapFlow::StartTicketFail( long lPumpNumber ,OLA_STAT *pOla )
{
	_Module.m_server.m_cFuelLoayaltyPS.ClearTimer(lPumpNumber - 1);

	PAY_AT_PUMP_INFO cInfo;
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cInfo);

	CString str;
	long p = lPumpNumber - 1;
	CPromptNMPDetails cPromptNMP;
	//cPromptNMP.SetScreenNumber(SCREEN_LOYALTY_OFFLINE);

	cPromptNMP.SetStateType(OLA_LOYALTY_WAIT_CONTROLLER_MSG_START_TICKET_FAIL);

	pOla->m_byState = OLA_LOYALTY_WAIT_CONTROLLER_MSG_START_TICKET_FAIL;
	_Module.m_server.m_cPumpArray[p].SetOlaStat(pOla);
	cPromptNMP.Execute(cInfo, lPumpNumber, *pOla, PT_GENERIC, 1);
}


BOOL CDynamicPapFlow::IsVaildCard( long lMediaMasterType )
{
	return (lMediaMasterType != CARD_TYPE_INVALID);
}

