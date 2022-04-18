
#include "StdAfx.h"
#include "DeferralDiscountNMPDetails.h"


/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
														//4.0.20.54 64528	
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
CDeferralDiscountNMPDetails::CDeferralDiscountNMPDetails()
{
	m_eNMPType = NMP_TYPE_DeferralDiscount;

	for(int i = 0; i < MAX_PUMPS_64; i++)
	{
		m_lNumOfManualDeferralRetries[i] = 0;	
	}

	SetStateType(OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET);	
}



/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
														//4.0.20.54 64528	
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
void CDeferralDiscountNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	char szTerminalMsg[MAX_FIELD_VAL] = {0};
	GetMessageForTerminal(lPumpNumber, cTmpInfo,sizeof(szTerminalMsg),szTerminalMsg);

	OpenNeedMoreParam(cTmpInfo,lPumpNumber,szTerminalMsg,NMP_MAX_NUM_OF_OPEN_KEYS,m_lTimeout,ONMP_OpenYesNoKeys);

	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Execute Requested Deferral discount NMP");
}

/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
void CDeferralDiscountNMPDetails::HandleDeferralAnswer(long lPumpNumber , OLA_STAT *pOla)
{
	long lPumpIndex = lPumpNumber -1;

	PAY_AT_PUMP_INFO cInfo;
				
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cInfo);

	// get the customer answer
	BOOL bUseDiscountAnswer = cInfo.CardSaleExtraData3.sParamResult[0] == 'Y';
	
	if (bUseDiscountAnswer )
	{	
		//Add the deferral discount to cents off
		if(_Module.m_server.m_cFuelLoayaltyPS.IsManualDeferral(lPumpIndex)) // 4.0.23.2350  TD 403075
		{
			StartManualDeferralFlow(cInfo, lPumpNumber, pOla);	
		}
		else
		{	
			_Module.m_server.m_cFuelLoayaltyPS.UpdateDeferralDiscountPerGrade(lPumpNumber,cInfo);

			_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo, TRUE); //4.0.23.860 212541
		}
	}
	else
	{
		_Module.m_server.m_cFuelLoayaltyPS.UpdateCentsOffPerGrade(lPumpNumber,cInfo);

		l2a(0, cInfo.CardSaleExtraData2.sVolumeLimit, sizeof(cInfo.CardSaleExtraData2.sVolumeLimit)); //4.0.20.620
		
		cInfo.CardSaleExtraData4.sExistLoyaltyLimit = ' ';
		
		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cInfo);
		
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, &cInfo, TRUE); //4.0.23.860 212541		
	}	
}



/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
void CDeferralDiscountNMPDetails::HandleManualDeferral(long lPumpNumber, OLA_STAT *pOla ,PAY_AT_PUMP_INFO *pInfo)
{

	long lManualDeferralDiscount = GetDeferralValueFromCardSale(pInfo);

	long lMaxDeferral = _Module.m_server.m_cFuelLoayaltyPS.GetDeferralDiscountForPump(lPumpNumber);
	
	CString sMsg;
	sMsg.Format("CDeferralDiscountNMPDetails::HandleManualDeferal - customer choose %ld deferral , Max deferral = %ld ",lManualDeferralDiscount,lMaxDeferral);
	_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,sMsg);

	BOOL bValidDeferralValue  = ((0 < lManualDeferralDiscount) && (lManualDeferralDiscount <= lMaxDeferral));
	
	if(FALSE == bValidDeferralValue && (m_lNumOfManualDeferralRetries[lPumpNumber-1]++ < 3))
	{
		TreatMaxDefrrralExceed(lPumpNumber,lManualDeferralDiscount,lMaxDeferral,pOla,pInfo);
	}
	else
	{
		ContiuneFlowAfterManualDeferral(lPumpNumber, lManualDeferralDiscount ,bValidDeferralValue,pInfo, pOla);
	}
}

/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
void CDeferralDiscountNMPDetails::TreatMaxDefrrralExceed(long lPumpNumber ,long lDeferralDiscount,long lMaxDeferral , OLA_STAT *pOla ,PAY_AT_PUMP_INFO *pInfo )
{
	CString sMsg;
	sMsg.Format("CDeferralDiscountNMPDetails::TreatMaxDefrrralExceed - Mex Deferral %ld exceeded!",lMaxDeferral);
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);

	OpenNeedMoreParam(*pInfo ,lPumpNumber,m_sManualDeferralMsg,NMP_MAX_NUM_OF_OPEN_KEYS,NMP_DEFAULT_TIMEOUT,ONMP_OpenNumericKeys | ONMP_OpenEnterCancelKeys);

	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(pInfo);
	
}

/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
long CDeferralDiscountNMPDetails::GetDeferralValueFromCardSale( PAY_AT_PUMP_INFO * pInfo )
{
	int iLen = StrNonSpaceLen( (char *)  pInfo->CardSaleExtraData3.sParamResult);
	
	BYTE * sDeferralReward = new BYTE[iLen+1];
	
	memcpy(sDeferralReward,pInfo->CardSaleExtraData3.sParamResult,iLen);
	
	sDeferralReward[iLen] = 0;
	
	long lDeferralDiscount = a2l(sDeferralReward,iLen);
	
	delete [] sDeferralReward;

	return lDeferralDiscount;
}

/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
void CDeferralDiscountNMPDetails::ContiuneFlowAfterManualDeferral( long lPumpNumber, long lDeferralDiscount, BOOL bValidDeferralValue , PAY_AT_PUMP_INFO * pInfo, OLA_STAT * pOla )
{
	// reset the counter for next trs
	m_lNumOfManualDeferralRetries[lPumpNumber-1] = 0;
		
	if(bValidDeferralValue)
	{
		_Module.m_server.m_cFuelLoayaltyPS.SetdeferralDiscount(lPumpNumber,lDeferralDiscount);
		
		_Module.m_server.m_cFuelLoayaltyPS.UpdateDeferralDiscountPerGrade(lPumpNumber,*pInfo);

		// set the ctor state again
		pOla->m_byState = m_nOlaState;
		
		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(pOla); 
		
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, pOla, pInfo, TRUE);

	}
	else
	{
		_Module.m_server.CancelLoyaltyTrs(lPumpNumber,pOla,pInfo);

		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(pOla);
	}



}

/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
void CDeferralDiscountNMPDetails::StartManualDeferralFlow( PAY_AT_PUMP_INFO cInfo, long lPumpNumber, OLA_STAT * pOla )
{
	OpenNeedMoreParam(cInfo ,lPumpNumber,m_sManualDeferralMsg,NMP_MAX_NUM_OF_OPEN_KEYS,NMP_DEFAULT_TIMEOUT,ONMP_OpenNumericKeys | ONMP_OpenEnterCancelKeys);
	
	pOla->m_byState = OLA_LOYALTY_WAIT_FOR_DEFERAL_ANSWER;
	
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cInfo);
}


/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
void CDeferralDiscountNMPDetails::LoadConfiguration()
{
	CNMPDetails::LoadConfiguration();

	_PARAM.LoadParam(m_sPath, NMP_FIELD_MANUAL_DEFERRAL, (char *)m_sManualDeferralMsg, sizeof(m_sManualDeferralMsg), "", FALSE);	

	if ((0 == strlen(m_sManualDeferralMsg)) || (ChrAll(m_sManualDeferralMsg, strlen(m_sManualDeferralMsg))))	
	{
		sprintf_s(m_sManualDeferralMsg,"Please Enter Deferral Value");
	}


}

/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
void CDeferralDiscountNMPDetails::GetMessageForTerminal( long lPumpNumber, PAY_AT_PUMP_INFO &cTmpInfo , int iSizeOfOutMsg,char * szOutTerminalMsg)
{
	float fDiscount = RoundDouble(_Module.m_server.m_cFuelLoayaltyPS.GetDeferralValueToDisplayOnPump(lPumpNumber),1);  //fix deferral discount display precision  (round to two digits after decimal point)

	if ((0 == strlen(m_sMessage)) || (ChrAll(m_sMessage, strlen(m_sMessage))))	//4.0.20.10
	{
		memset(szOutTerminalMsg, ' ', iSizeOfOutMsg);
		sprintf_s(szOutTerminalMsg, iSizeOfOutMsg,"You have $%.3lf / gal |in GAS REWARDS points, |Use it?", fDiscount/100);	//4.0.20.260 67761
		memcpy(m_sMessage, szOutTerminalMsg, min(sizeof(m_sMessage), strlen(szOutTerminalMsg)));	//4.0.22.504 69344
	}
	else
	{
		char sTmpStr[MAX_FIELD_VAL] = {0};
		char sTmpLanguagePath[MAX_FIELD_VAL] = {0};

		_Module.m_server.m_cCommonFunction.LoadDefaultLaguageString(lPumpNumber,sTmpLanguagePath,sizeof(sTmpLanguagePath));
		_PARAM.LoadParam((LPCSTR)sTmpLanguagePath, m_sMessage,(char *)sTmpStr, sizeof(sTmpStr), "",FALSE); //4.0.20.10 //4.0.20.260 67761 Don't create the key if it doesn't exist.

		if ((strlen(sTmpStr)) && (!ChrAll(sTmpStr, strlen(sTmpStr))))
		{	
			long lValueToDisplay = (long)fDiscount/100;
			sprintf_s(szOutTerminalMsg, iSizeOfOutMsg,sTmpStr, lValueToDisplay);
		}
		else
		{
			//Add the deferral discount to the current message
			char sTemp[MAX_FIELD_VAL] = {0};
			sprintf_s(sTemp,MAX_FIELD_VAL, m_sMessage, fDiscount/100);
			memcpy(szOutTerminalMsg, sTemp, min((int)strlen(sTemp), iSizeOfOutMsg));
		}
	}
	
	
}



