#include "stdafx.h"
#include "FlowControlManager.h"
#include "PapFlow.h"
#include "PakFlow.h"
#include "ManualLoyaltyNMPDetails.h"
#include "DynamicPapFlow.h"
#include "PreAuthorizeScreen.h"
#include "DeferralDiscountNMPDetails.h"
#include "LoyaltyNMPDetails.h"

BOOL CPakFlow::m_bInstanceFlag = FALSE;
CPakFlow* CPakFlow::m_pInstance = NULL;

BOOL CPapFlow::m_bInstanceFlag = FALSE;
CPapFlow* CPapFlow::m_pInstance = NULL;



CFlowControlManager::CFlowControlManager():m_bFuelFlowControlFlag(TRUE), m_bInitFlag(FALSE), m_bEmeraldLoyaltyOnline(TRUE)
{//4.0.22.504	69344

	memset(m_NMPstartTime, 0, sizeof(m_NMPstartTime));
	memset(m_iStartOlaNMPState, 0, sizeof(m_iStartOlaNMPState)); // TD 359309

	for (int i = 0; i < MAX_PUMPS_64; i++)
	{
		m_cPumpFlowObject[i] = NULL;
		m_lMediaMasterType[i] = 0;
	}	
	
}

CFlowControlManager::~CFlowControlManager()
{
	// Prevent destructor to throw an exception to get out
	try {
		DeleteAllExistingFlows();	//4.0.22.508 74810
	} catch(...) {}
}

void CFlowControlManager::InitAllExistingFlows(BOOL bReconfigure) //4.0.22.508 74810
{
	CPakFlow::getInstance()->Init(bReconfigure);
	CPapFlow::getInstance()->Init(bReconfigure);
}


void	CFlowControlManager::DeleteAllExistingFlows() //4.0.22.508 74810
{
	if(_Module.m_server.GetFlowControlType() == DYNAMIC_PAP_FLOW)
	{
		for (int i = 0; i < MAX_PUMPS_64; i++)
		{
			delete m_cPumpFlowObject[i];
			m_cPumpFlowObject[i] = NULL;
		}
	}
	else
	{
		for (int i = 0; i < MAX_PUMPS_64; i++)
		{
			m_cPumpFlowObject[i] = NULL;
		}

		CPapFlow::getInstance()->DeleteFlow();
		CPakFlow::getInstance()->DeleteFlow();
	}	
}


/******************************************************************************
 Description: Load all NMP configuration from the registry.
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			02/04/08   15:38		Start 4.0.19.508
 Liat			26/05/09	11:53		Update 4.0.22.508 74810
******************************************************************************/
BOOL	CFlowControlManager::Init(BOOL bReconfigure)
{
	BOOL bRetVal = TRUE;
	DWORD dwTmp = 0;
	try
	{
		if(_Module.m_server.GetFlowControlType() != DYNAMIC_PAP_FLOW)
		{
			_LOGMSG.LogMsg("CFlowControlManager::Init, Loading Need More Param configuration");
			InitAllExistingFlows(bReconfigure);
		}

		for (int i = 0; i < MAX_PUMPS_64; i++)
		{
			if(_Module.m_server.GetFlowControlType() == DYNAMIC_PAP_FLOW)
				m_cPumpFlowObject[i] = new CDynamicPapFlow(i+1);
			else
				m_cPumpFlowObject[i] = CPapFlow::getInstance();
	
		}
		SetInitFlag(TRUE);
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFlowControlManager::Init, Unexpected error has occurred");
		bRetVal = FALSE;
	}
	return bRetVal;
}

//4.0.22.508 74810
void	CFlowControlManager::HandleNMPSessions(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo, BOOL bSkipRepeatOption /* = TRUE */, BOOL bStartFueling /* = FALSE */)
{
	CBaseFlow* pFlow = GetPumpFlow(lPumpNumber);
	if (pFlow) {
		pFlow->HandleNMPSessions(lPumpNumber, pOla, pInfo, bSkipRepeatOption, bStartFueling);
	}
}

//4.0.22.508 start 74810
void	CFlowControlManager::StartAfterQueryFlowControl(long lPumpNumber, OLA_STAT & ola, PAY_AT_PUMP_INFO & cTmpInfo)
{
	CBaseFlow* pFlow = GetPumpFlow(lPumpNumber);
	if (pFlow) {
		pFlow->StartAfterQueryFlowControl(lPumpNumber, ola, cTmpInfo);
	}
}	
void	CFlowControlManager::StartAfterAddMemberFlowControl(long lPumpNumber, OLA_STAT * ola, long lMemberRes)
{
	CBaseFlow* pFlow = GetPumpFlow(lPumpNumber);
	if (pFlow) {
		pFlow->StartAfterAddMemberFlowControl(lPumpNumber, ola, lMemberRes);
	}
}	
void	CFlowControlManager::StartAfterStartTicketFlowControl(long lPumpNumber, OLA_STAT * pOla, long lStartTicketRes)
{
	CBaseFlow* pFlow = GetPumpFlow(lPumpNumber);
	if (pFlow) {
		pFlow->StartAfterStartTicketFlowControl(lPumpNumber, pOla, lStartTicketRes);
	}
}	

//4.0.22.508 end 74810

long	CFlowControlManager::GetPumpSrvAddMemberCardRes(long lPumpNumber) //4.0.23.390 116544 //4.0.24.90 
{
	return m_lPumpSrvMemberCardRes[lPumpNumber-1];
}	

void CFlowControlManager::SetPumpSrvAddMemberCardRes(long lPumpNumber, long lRes) //4.0.23.390 116544 //4.0.24.90
{
	CString sMsg;
	sMsg.Format("SetPumpSrvAddMemberCardRes, Res = %ld", lRes);	//4.0.23.610 133300
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
	
	m_lPumpSrvMemberCardRes[lPumpNumber-1] = lRes;
}

void CFlowControlManager::HandleNMPManualLoyalty(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo)
{
	CManualLoyaltyNMPDetails cManualLoyaltyNMP;

	cManualLoyaltyNMP.Execute(*pInfo, lPumpNumber, *pOla);

	// RFUEL-647
	if (_Module.m_server.GetFlowControlType() == DYNAMIC_PAP_FLOW)
	{
		CFlowControlManager::DeleteAllExistingFlows(); //4.0.22.508 74810
	}
}


/******************************************************************************
 Description: Handling deferral discounts. 
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			22/06/08	15:27		Start 4.0.20.54
 OmerT          27/08/12    19:30       TD 295237
******************************************************************************/

void	CFlowControlManager::HandleNMPDeferralDiscount(long lPumpNumber, OLA_STAT *pOla)
{
	CNMPDetails * cNmpObj = GetPumpFlow(lPumpNumber)->GetNMPObject(NMP_TYPE_DeferralDiscount);

	CDeferralDiscountNMPDetails* cDeferalNMPObject = dynamic_cast<CDeferralDiscountNMPDetails*>(cNmpObj);

	if (cDeferalNMPObject != NULL)
	{
		cDeferalNMPObject->HandleDeferralAnswer(lPumpNumber, pOla);
	}
}

void	CFlowControlManager::SetPumpFlow(long lPumpNumber, LOYALTY_FLOW_TYPE eType) //4.0.23.20 76383
{
	switch (eType)
	{
		case PAK_FLOW:
		{
			m_cPumpFlowObject[lPumpNumber - 1] = CPakFlow::getInstance();
		}
		break;
		case DYNAMIC_PAP_FLOW:
		{
			;
				//m_cPumpFlowObject[lPumpNumber - 1] = CDynamicPapFlow::getInstance();
		}
		break;	
		
		default: //PAP
		{
			m_cPumpFlowObject[lPumpNumber - 1] = CPapFlow::getInstance();
		}
	}
}


/******************************************************************************
 Description: Handling Method of payment selection (Pap to Pak). 
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			13/07/09	15:58		Start 4.0.23.20 76383
******************************************************************************/
void	CFlowControlManager::HandleNMPMethodOfPayment(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo, BOOL bPapToPak, BOOL bPaymentSwiped)
{
	CString str;
	if (bPapToPak)
	{

		CPumpTransact TmpTrs;
		long lSwipeLoyaltyOrder = 0, lAuthLoyaltyOrder = 0, lMethodOfPaymentOrder = 0;

		if (!_Module.m_server.m_cPumpArray[lPumpNumber -1].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&TmpTrs))
		{
			TmpTrs.m_lNumber	= _Module.m_server.NewTransactNumber();
			TmpTrs.m_lActiveReadNumber	= _Module.m_server.GetActiveReadNumber();
			TmpTrs.SetLink(TRS_LINK_OLA_NUMBER,  _Module.m_server.m_cPumpArray[lPumpNumber -1].GetPAPSeqNumber());	
			TmpTrs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[lPumpNumber -1].GetShiftNumber();
			TmpTrs.SetStatus(WAIT_PROGRESS);	
			TmpTrs.m_lPump = lPumpNumber;
			_Module.m_server.CentralInsertTrs(lPumpNumber,&TmpTrs);
		}

		long lTmp =  a2l(pInfo->CardSaleExtraData3.sFlags1,sizeof(pInfo->CardSaleExtraData3.sFlags1)) + FL_PAP_TO_PAK;
		l2a(lTmp ,pInfo->CardSaleExtraData3.sFlags1, sizeof(pInfo->CardSaleExtraData3.sFlags1));
		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(pInfo);
		if (_Module.m_server.GetFlowControlType() != DYNAMIC_PAP_FLOW)
		{
			SetPumpFlow(lPumpNumber, PAK_FLOW);
			lSwipeLoyaltyOrder = CPapFlow::getInstance()->GetNMPOrder(NMP_TYPE_LoyaltyCard);
			lAuthLoyaltyOrder = CPapFlow::getInstance()->GetNMPOrder(NMP_TYPE_AuthorizeLoyalty);
			lMethodOfPaymentOrder = CPapFlow::getInstance()->GetNMPOrder(NMP_TYPE_MethodOfPayment);
			if (lMethodOfPaymentOrder - lAuthLoyaltyOrder == 1)
			{
				if (CPakFlow::getInstance()->GetNMPOrder(NMP_TYPE_AuthorizeLoyalty) > 0)
				{
					pOla->m_byState = OLA_LOYALTY_AUTHORIZE_LOYALTY;	//Continue PAK flow from authorizing loyalty.
					str.Format("User selected PAK flow-> Changing flow from PAP to PAK, continue from authorizing loyalty state");
				}
				else
				{
					pOla->m_byState = 0;
					str.Format("User selected PAK flow-> Changing flow from PAP to PAK, no auth loyalty in the PAK flow, continue from starting state");
				}
			}
			else if (lMethodOfPaymentOrder - lSwipeLoyaltyOrder == 1)
			{
				if (CPakFlow::getInstance()->GetNMPOrder(NMP_TYPE_LoyaltyCard) > 0)
				{
					pOla->m_byState = OLA_LOYALTY_SWIPE_LOYALTY;	//Continue PAK flow from swiping loyalty.
					str.Format("User selected PAK flow-> Changing flow from PAP to PAK, continue from swiping loyalty state");
				}
				else
				{
					pOla->m_byState = 0;
					str.Format("User selected PAK flow-> Changing flow from PAP to PAK, no swipe loyalty in the PAK flow, continue from starting state");
				}
			}
			else	//4.0.23.860 212541
			{
				if (CPakFlow::getInstance()->GetNMPOrder(NMP_TYPE_DeferralDiscount) > 0)
				{
					pOla->m_byState = OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET;	//Continue PAK flow from deferral.
					str.Format("User selected PAK flow-> Changing flow from PAP to PAK, continue from deferral discount state");
				}
				else
				{
					pOla->m_byState = OLA_LOYALTY_RUN_QUERY;
					str.Format("User selected PAK flow-> Changing flow from PAP to PAK, no deferral in the PAK flow, continue from run query state");
				}
			}
		}
		else
			pOla->m_byState = OLA_LOYALTY_METHOD_OF_PAYMENT;

		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);
		_Module.m_server.SavePumpInfo(lPumpNumber);

		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
	}
	else if (bPaymentSwiped)
	{
		pOla->m_byState = OLA_LOYALTY_SWIPE_PAYMENT;	//Continue pap flow from swiping payment.
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);
		_Module.m_server.SavePumpInfo(lPumpNumber);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "User selected PAP flow, payment was swiped");
	}
	else
	{
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "User selected PAP flow, no payment was swiped");
	}
}



/******************************************************************************
 Description: Initialize the flow running on a pump and start the flow control. 
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			26/05/09	15:27		Start 4.0.22.508 74810
******************************************************************************/
void CFlowControlManager::StartFlowControl(long lPumpNumber, short iStartOlaNMPState, OLA_STAT & ola, LOYALTY_FLOW_TYPE eLoyaltyFlowType) 
{
	CString str;
	SetPumpFlow(lPumpNumber, eLoyaltyFlowType); //4.0.23.20 76383

	if (m_cPumpFlowObject[lPumpNumber - 1] !=NULL)
	{
		m_cPumpFlowObject[lPumpNumber - 1]->StartFlowControl(lPumpNumber, iStartOlaNMPState, ola);
		str.Format("Starting flow control, first state = %s", _LOGMSG.GetStateString(iStartOlaNMPState));
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
	}
	else
	{
		str.Format("Failed to start flow control, first state = %s", _LOGMSG.GetStateString(iStartOlaNMPState));
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);

	}

}

void CFlowControlManager::SetMediaMasterType(long lPumpNumber,long lMediaMaster)
{
	m_lMediaMasterType[lPumpNumber-1] = lMediaMaster;
}

void CFlowControlManager::HandleNMPAlternateIdSwipeLoyaltyCard(long lPumpNumber, OLA_STAT * pOla, PAY_AT_PUMP_INFO* cInfo) 
{
	long lMediaMasterType = _Module.m_server.m_cFlowControlManager.GetMediaMasterType(lPumpNumber);
	
	if ((lMediaMasterType != CARD_TYPE_LOYALTY) && (lMediaMasterType != CARD_TYPE_LOYALTY_AND_CREDIT))//4.0.23.390 116544
	{
		pOla->m_byState = OLA_LOYALTY_INVALID_INPUT; // TD 480995
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, 
			"HandleNMPAlternateIdSwipeLoyaltyCard: Card type is not loyalty, moving back to OLA_LOYALTY_ALTERNATE_ID_SWIPE_LOYALTY_CARD");
	}
	else
		_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_COMPLETE);


	_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, cInfo, TRUE);
}

void CFlowControlManager::HandleNMPAlternateIDManualEntry(long lPumpNumber, OLA_STAT * pOla, PAY_AT_PUMP_INFO* cInfo) 
{
	_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_COMPLETE);
	_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, cInfo, TRUE);
}

void CFlowControlManager::HandlePunchhNMPAlternateIDRetry(long lPumpNumber, OLA_STAT * pOla, PAY_AT_PUMP_INFO * cInfo)
{
	CNMPDetails * cNmpObj = GetPumpFlow(lPumpNumber)->GetNMPObject(NMP_TYPE_LoyaltyCard);

	CLoyaltyNMPDetails* cLoyaltyNMPObject = dynamic_cast<CLoyaltyNMPDetails*>(cNmpObj);

	if (cLoyaltyNMPObject != NULL)
	{
		cInfo->CardSaleInfo.cardData.sTranStatus = ' ';
		cLoyaltyNMPObject->StartAlternateIDRetryFlow(*cInfo, lPumpNumber, pOla);
	}
}

void CFlowControlManager::HandlePreAuthoitzeScreen( long lPumpNumber,OLA_STAT *pOla, PAY_AT_PUMP_INFO & cInfo, CARD_SALE_ALL3* pCardData )
{
	// other amoount
	if (cInfo.CardSaleExtraData3.sParamResult[0] == 'O')
	{
		CPreAuthorizeScreen nmp;
		OLA_STAT * pOla = new OLA_STAT;
		nmp.SetScreenNumber(SCREEN_PREAUTHORIZE_SCREEN_CARWASH);
		nmp.Execute(cInfo , lPumpNumber , *pOla);
	}

	else
	HandleNMPSessions(lPumpNumber, pOla, &cInfo, TRUE);
}

void CFlowControlManager::HandleNMPCarWashSale( long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo)
{
	if(pInfo->CardSaleExtraData3.sParamResult[0] == 'N')
	{
		_LOGMSG.LogMsg("CFlowControlManager::HandleNMPCarWashSale - car wash was not sold, moving to the next step.");
		HandleNMPSessions(lPumpNumber,pOla,pInfo);
	}

	else if( _Module.m_server.m_cFuelLoayaltyPS.AddCarWashItem(lPumpNumber,eGetPromotion) == FALSE)
	{
		HandleNMPSessions(lPumpNumber,pOla,pInfo); // moving to the next step in the flow.
	}
	// else getting an event back from loyaltyFuel.	
}



void CFlowControlManager::HandleNMPCarWashPromotion( long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo )
{
	if(pInfo->CardSaleExtraData3.sParamResult[0] == 'Y')
	{
		if (IsCarWashAmountBiggerThenPreAuthorizeAmount(pInfo, lPumpNumber, pOla))
		{
			_LOGMSG.LogMsg("CFlowControlManager::HandleNMPCarWashPromotion - carwash amount > user limit, canceling trs!");
			_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, pInfo, CD_DECLINE_AMOUNT);
			return;
		}
			
		BOOL bReturnCode = _Module.m_server.m_cFuelLoayaltyPS.AddCarWashItem(lPumpNumber,eCustomerAccpectedPromotion);	
		if(bReturnCode == FALSE)
		{
			_LOGMSG.LogMsg("CFlowControlManager::HandleNMPCarWashSale - Failed adding car wash item, moving to the next step.");
			HandleNMPSessions(lPumpNumber,pOla,pInfo); // moving to the next step in the flow.
		}
		// else getting an event back from loyaltyFuel.
	}
	else
	{
		_LOGMSG.LogMsg("CFlowControlManager::HandleNMPCarWashSale - customer declined car wash promotion, moving to the next step.");
		HandleNMPSessions(lPumpNumber,pOla,pInfo);
	}
}

/******************************************************************************
 Description: Handles the menu:
			  Swipe Reward Card
			  Select 1 to enter phone #
			  Select 2 to enter reward card #
			  NO to continue without reward card  

 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			12/10/2011       		Start			4.0.23.1220 TD 155754
******************************************************************************/
void CFlowControlManager::HandleNMPPhoneLookUp(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo)
{
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "CFlowControlManager::HandleNMPPhoneLookUp");

	char sRegMsg[MAX_FIELD_VAL] = { 0 };

	CNMPDetails cNmpObject;

	if ((pInfo->CardSaleExtraData3.sParamResult[0] == '1') ||
		(pInfo->CardSaleExtraData3.sParamResult[0] == 'Y'))	// 4.0.23.2590
	{
		_Module.m_server.m_cCommonFunction.LoadMessageFromLanguagesReg(lPumpNumber, "EnterPhoneNumber", sRegMsg, sizeof(sRegMsg), "Enter Phone Number");
		cNmpObject.SetScreenNumber(SCREEN_ENTER_PHONE_NUMBER);
		cNmpObject.OpenNeedMoreParam(*pInfo, lPumpNumber, sRegMsg, NMP_MAX_NUM_OF_OPEN_KEYS, NMP_DEFAULT_TIMEOUT, ONMP_OpenNumericKeys | ONMP_OpenEnterCancelKeys);

		pOla->m_byState = OLA_LOYALTY_PHONE_LOOKUP_ENTER_PHONE_NUMBER;
	}
	else if (pInfo->CardSaleExtraData3.sParamResult[0] == '2')
	{
		_Module.m_server.m_cCommonFunction.LoadMessageFromLanguagesReg(lPumpNumber, "EnterRewardCardNumber", sRegMsg, sizeof(sRegMsg), "Enter Reward Card Number");
		cNmpObject.SetScreenNumber(SCREEN_ENTER_REWARD_CARD_NUMBER);
		cNmpObject.OpenNeedMoreParam(*pInfo, lPumpNumber, sRegMsg, NMP_MAX_NUM_OF_OPEN_KEYS, NMP_DEFAULT_TIMEOUT, ONMP_OpenNumericKeys | ONMP_OpenEnterCancelKeys);

		pOla->m_byState = OLA_LOYALTY_PHONE_LOOKUP_ENTER_REWARD_CARD;
	}
	else if (pInfo->CardSaleExtraData3.sParamResult[0] == 'N')
	{
		SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_NO_LOYALTY);
		StartAfterAddMemberFlowControl(lPumpNumber, pOla, LOYALTY_STATE_NO_LOYALTY); //4.0.23.1220 TD 155754
	}
}

/******************************************************************************
 Description: 	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			12/10/2011       		Start			4.0.23.1220 TD 155754
******************************************************************************/
void CFlowControlManager::HandleNMPGetPhoneNumber(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo)
{
	int iPhoneLength = StrNonSpaceLen( (char *) pInfo->CardSaleExtraData3.sParamResult);

	BYTE * sPhoneNumber = new BYTE[iPhoneLength+1];

	memcpy(sPhoneNumber,pInfo->CardSaleExtraData3.sParamResult,iPhoneLength);

	sPhoneNumber[iPhoneLength] = 0;
		
	_Module.m_server.m_cFuelLoayaltyPS.RunPhoneLookUp(lPumpNumber , sPhoneNumber);

	delete [] sPhoneNumber;

	pOla->m_byState = OLA_LOYALTY_WAIT_PHONE_LOOKUP_RESPONSE;
}


/******************************************************************************
 Description:	continue flow as if we received reward card from loyalty fuel
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			12/10/2011       		Start			4.0.23.1220 TD 155754
******************************************************************************/
void CFlowControlManager::HandleNMPGetRewardCard(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo)
{
	int iStrLength = StrNonSpaceLen( (char *)  pInfo->CardSaleExtraData3.sParamResult);

	BYTE * sRewardCard = new BYTE[iStrLength+1];

	memcpy(sRewardCard,pInfo->CardSaleExtraData3.sParamResult,iStrLength);

	sRewardCard[iStrLength] = 0;

	CString sMsg;
	sMsg.Format("CFlowControlManager::HandleNMPGetRewardCard - Reward card %s entered.",sRewardCard);
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
	
	_Module.m_server.m_cFuelLoayaltyPS.PhoneLookUpResult(lPumpNumber ,_PhoneNumberFound , sRewardCard );

	delete [] sRewardCard;

	// get an updated pointer to ola and returns it to the caller
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetOlaStat(pOla);
}

/******************************************************************************
Description:	Check if car wash amount exceed pre-authoirze amount limit
Returns:

Parameters:

WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
Omer			25/12/2016       	Start			TD 466954
******************************************************************************/
BOOL CFlowControlManager::IsCarWashAmountBiggerThenPreAuthorizeAmount(PAY_AT_PUMP_INFO * pInfo, long lPumpNumber, OLA_STAT * pOla)
{
	long lUnitPrice = a2l((BYTE *)pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice));
	long lQuantity = a2l((BYTE *)pInfo->CardSaleInfo.extraData.SALES[0].sQty, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sQty)) + _Module.m_server.m_cFuelLoayaltyPS.GetRemainingBalanceToTrigger(lPumpNumber - 1);
	long lCarwashAmount = lUnitPrice * lQuantity;

	long lPreAuthorizeAmount = _Module.m_server.GetRequstedAuthorizeAmount(lPumpNumber);

	if (lPreAuthorizeAmount - lCarwashAmount >= 500 /* The minimum allowed limit is 50 cents */)
		return FALSE;

	return TRUE;
}

/******************************************************************************
Description:	Check if car wash amount exceed authorized amount
Returns:

Parameters:

WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
Omer			25/12/2016       		Start		TD 467566	
******************************************************************************/
BOOL CFlowControlManager::IsCarWashAmountBiggerThenAuthoirzedAmount(PAY_AT_PUMP_INFO * pInfo, long lPumpNumber, OLA_STAT * pOla, long lAuthrizedAmount) // TD 467566
{
	long lUnitPrice = a2l((BYTE *)pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice));
	long lQuantity = a2l((BYTE *)pInfo->CardSaleInfo.extraData.SALES[0].sQty, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sQty)); // remaining balance to trigger already included. 
	long lCarwashAmount = lUnitPrice * lQuantity;

	if (lAuthrizedAmount * 10 - lCarwashAmount >= 500 /* The minimum allowed limit is 50 cents */)
		return FALSE;

	return TRUE;
}

void CFlowControlManager::HandleNMPAlternateID(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo)
{
	long p = lPumpNumber - 1;
	bool bEntryHandled = false;

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		char strMsg[MAX_FIELD_VAL] = { 0 };
		sprintf_s(strMsg, "CFlowControlManager::HandleNMPAlternateID() - EntryMethod = %c, ParamResult = %c", 
			pInfo->CardSaleInfo.cardData.sEntryMethod, 
			pInfo->CardSaleExtraData3.sParamResult[0]);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
	}

	if (pInfo->CardSaleInfo.cardData.sEntryMethod == 'S')
	{
		long lMediaMasterType = _Module.m_server.m_cFlowControlManager.GetMediaMasterType(lPumpNumber);

		if ((lMediaMasterType != CARD_TYPE_LOYALTY) && (lMediaMasterType != CARD_TYPE_LOYALTY_AND_CREDIT))//4.0.23.390 116544
		{
			pOla->m_byState = OLA_LOYALTY_INVALID_INPUT; // TD 480995
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, 
				"HandleNMPAlternateID: Card type is not loyalty, switch state back to OLA_LOYALTY_ALTERNATE_ID");
		}
		else 
		{
			_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_COMPLETE);
		}

		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, pInfo, TRUE);
		bEntryHandled = true;
	}

	if (pInfo->CardSaleExtraData3.sParamResult[0] == 'Y') //4.0.23.481 126185
	{
		pOla->m_byState = OLA_LOYALTY_ALTERNATE_ID_SWIPE_LOYALTY_CARD;
		int iMaxNumOfOpenKeys = 1;
		_Module.m_server.OpenNeedMoreParam(*pInfo, lPumpNumber, "Please swipe loyalty", iMaxNumOfOpenKeys, NMP_DEFAULT_TIMEOUT, ONMP_OpenCardReader | ONMP_OpenEnterCancelKeys); // RFUEL 1146
		bEntryHandled = true;
	}
	else if (pInfo->CardSaleExtraData3.sParamResult[0] == 'N') //4.0.23.481 126185
	{
		pOla->m_byState = OLA_LOYALTY_START_FUELING;
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, pInfo, TRUE);
		bEntryHandled = true;
	}
	else if (pInfo->CardSaleExtraData3.sParamResult[0] == 'A') //4.0.23.481 126185
	{
		char sRegMsg[MAX_FIELD_VAL] = { 0 };
		_Module.m_server.m_cCommonFunction.LoadMessageFromLanguagesReg(lPumpNumber, "EnterAlternateID", sRegMsg, sizeof(sRegMsg), "Please Enter Alternate ID");

		pOla->m_byState = OLA_LOYALTY_ALTERNATE_ID_MANUAL_ENTER;
		_Module.m_server.OpenNeedMoreParam(*pInfo, lPumpNumber, sRegMsg, _Module.m_server.m_lMaxAlternateIDInputData, NMP_DEFAULT_TIMEOUT, ONMP_OpenNumericKeys | ONMP_OpenEnterCancelKeys, SCREEN_ENTER_ALTERNATE_ID,PT_GENERIC);
		bEntryHandled = true;
	}
	else if (pInfo->CardSaleExtraData3.sParamResult[0] == 'P') //4.0.23.481 126185
	{
		pInfo->CardSaleExtraData4.sLoyaltyEntryMethod = ' ';
		memset(pInfo->CardSaleExtraData6.sClubType, ' ', sizeof(pInfo->CardSaleExtraData6.sClubType));  //marik1969 
		CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();

		if (cPumpStatus.GetRes() == PAY_AT_KIOSK)
		{
			CPumpTransact trs;
			long lRtc;

			if (!_Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&trs)) // Found
			{
				_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, pInfo);
				int iRetNum = _Module.m_server.WideCancelTrs(SOURCE_INTERNAL, lPumpNumber, trs.m_lNumber, &lRtc);
				if (LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("Trs %d canceled by PumpSrv (return %d)", trs.m_lNumber, iRetNum);
					_LOGMSG.LogMsg("GCI", "SetPumpStatus", lPumpNumber, LOG_NONE, str);
				}
			}
		}
		else
		{
			// RFUEL-406
			_Module.m_server.m_cFlowControlManager.SetStartOlaNMPState(lPumpNumber, OLA_LOYALTY_AUTHORIZE_PAYMENT);
			_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_COMPLETE);
			pOla->m_byState = OLA_LOYALTY_START_TICKET | SESSION_AUTHORIZE;
		}

		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);

		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "HandleNMPAlternateID: Back button pressed, back to Swipe Loyally Screen");
		bEntryHandled = true;
	}

	if (!bEntryHandled)
	{
		pOla->m_byState = OLA_LOYALTY_INVALID_INPUT;
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP,
			"HandleNMPAlternateID: Unhandled input, switch state back to OLA_LOYALTY_ALTERNATE_ID");
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, pInfo, TRUE);
	}

	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(pInfo); //marik19699
}

/******************************************************************************
Description: SendNeedMoreParamSessionMemberInactive

Returns:

Parameters:

WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
OmerT		26/06/2017	 19:34		   CR 472487
******************************************************************************/
void CFlowControlManager::SendNeedMoreParamSessionMemberInactive(long lPumpNumber, OLA_STAT & ola, PAY_AT_PUMP_INFO & cTmpInfo)
{

	long lPumpIndex = lPumpNumber - 1;
	memset(&ola, 0, sizeof(OLA_STAT));

	EnterCriticalSection(_Module.m_server.m_csPumpArray);

	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetOlaStat(&ola);
	ola.m_byState = OLA_LOYALTY_MEMBER_NOT_ACTIVE;
	_Module.m_server.m_cPumpArray[lPumpIndex].SetOlaStat(&ola);

	LeaveCriticalSection(_Module.m_server.m_csPumpArray);

	char sMessage[MAX_FIELD_VAL] = { 0 };
	_Module.m_server.m_cCommonFunction.LoadMessageFromLanguagesReg(lPumpNumber, "InvalidMember", sMessage, sizeof(sMessage), "Invalid member");
	_Module.m_server.OpenNeedMoreParam(cTmpInfo, lPumpNumber, sMessage, 1, NMP_DEFAULT_TIMEOUT, ONMP_OpenEnterCancelKeys | ONMP_EnterAsYes,0,PT_BOOL);
}

/******************************************************************************
Description: HandleNeedMoreParamInactiveMemberResult

Returns:

Parameters:

WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
OmerT		    26/06/2017	 19:34		   CR 472487
******************************************************************************/
void CFlowControlManager::HandleNeedMoreParamInactiveMemberResult(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo)
{
	if ((pInfo->CardSaleExtraData3.sParamResult[0] == 'Y'))
	{
		memset(pInfo->CardSaleInfo.extraData.sLoyalId, ' ', sizeof(pInfo->CardSaleInfo.extraData.sLoyalId));
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(pInfo);
		_Module.m_server.m_cFlowControlManager.StartAfterQueryFlowControl(lPumpNumber, *pOla, *pInfo);
	}
	else // 'N' or Cancel
	{
		memset(pInfo->CardSaleInfo.cardData.sResultMsg, ' ', sizeof(pInfo->CardSaleInfo.cardData.sResultMsg));
		_Module.m_server.CancelLoyaltyTrs(lPumpNumber, pOla, pInfo);
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);
	}
}