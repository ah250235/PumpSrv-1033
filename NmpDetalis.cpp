
#include "NmpDetalis.h"
#include "StdAfx.h"
#include <iomanip>
#include "CStrFormat.h"

////////////////////////////////////////////////////////////////////
/////////////////////////  CNMPDetails /////////////////////////////
////////////////////////////////////////////////////////////////////
 
CNMPDetails::CNMPDetails():	m_bValidEntry(FALSE), m_nOlaState(0), m_lRepeat(0),m_lScreenNumber(0),
							m_lCurrentLoopNum(1), m_bCardReader(FALSE), m_bEnterCancel(FALSE),
							m_bYesNo(FALSE), m_bEnableScanner(FALSE), m_bEnableNumeric(FALSE),
							m_lTimeout(NMP_DEFAULT_TIMEOUT), m_eNMPTime(NMP_TIME_BeforeFuelingBeforeStartTicket), 
							m_eNMPType(NMP_TYPE_MessagePrompt), m_iNextFlowState(0), m_bPayInside(FALSE), 
							m_bOpenSoftKeys(FALSE), m_bAlternateID(FALSE), m_bContacless(FALSE) // TD 460484 [7/28/2016 MaximB]
{
	memset (m_sMessage, ' ', sizeof(m_sMessage));
	memset (m_sPath, 0, sizeof(m_sPath));
}

/******************************************************************************
 Description: Combining the NMP properties into a bit map. 
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			02/04/08   15:38		Start 4.0.19.508
******************************************************************************/

short CNMPDetails::CreateGeneralNMPProperties()
{
	short iBitMap = 0;
	
	if (m_bCardReader)
		iBitMap |= ONMP_OpenCardReader;

	if (m_bEnableScanner)
		iBitMap |= ONMP_OpenScanner;

	if (m_bEnableNumeric)
		iBitMap |= ONMP_OpenNumericKeys;

	if (m_bEnterCancel)
		iBitMap |= ONMP_OpenEnterCancelKeys;

	if (m_bYesNo)
		iBitMap |= ONMP_OpenYesNoKeys;

	if (m_bPayInside)
		iBitMap |= ONMP_OpenPayInsideKey;

	if (m_bContacless)	// TD 460484 [7/28/2016 MaximB]
		iBitMap |= ONMP_OpenContaless;

	if (m_bOpenSoftKeys)			// UDF - Epsilon
		iBitMap |= ONMP_OpenSoftKeys;

	if (m_bAlternateID)				// UDF - Epsilon
		iBitMap |= ONMP_AlternateID;

	return iBitMap;
}

void CNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola)
{
	long p = lPumpNumber-1;
	
	_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	LoadMessage(lPumpNumber,"Loyalty Flow");

	// set the message
	memcpy(cTmpInfo.CardSaleInfo.cardData.sResultMsg,m_sMessage,sizeof(cTmpInfo.CardSaleInfo.cardData.sResultMsg));

	// set the timeot
	l2a(m_lTimeout, cTmpInfo.CardSaleExtraData2.sMessageTimeOut, sizeof(cTmpInfo.CardSaleExtraData2.sMessageTimeOut));

	// set general Properties
	l2a(CreateGeneralNMPProperties(), cTmpInfo.CardSaleExtraData4.sONMPProperties, sizeof(cTmpInfo.CardSaleExtraData4.sONMPProperties));

	l2a(PT_GENERIC, cTmpInfo.CardSaleExtraData3.sPromptType, sizeof(cTmpInfo.CardSaleExtraData3.sPromptType));//General ONMP
	cTmpInfo.CardSaleExtraData3.sPromptMinLen[0] = '1';
	l2a(NMP_MAX_NUM_OF_OPEN_KEYS, cTmpInfo.CardSaleExtraData3.sPromptMaxLen, sizeof(cTmpInfo.CardSaleExtraData3.sPromptMaxLen));
	cTmpInfo.CardSaleExtraData3.sParamResult[0]  = '?';

	CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;
	_Module.m_server.SetSwipeResultDataToCommand(cmdInfo, cTmpInfo,CD_MORE_DATA_REQUIRED);
	_Module.m_server.SetCommand(CMD_SWIPE_RESULT, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"CFuelLoyaltyPS::Execute Requested General NMP");
}


/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
void CNMPDetails::LoadConfiguration()
{
	DWORD dwValid = 0;
	DWORD dwTmp = 0;

	if (ChrAll(m_sPath, sizeof(m_sPath) - 1, ' ')) //4.0.23.960 
		return;
	
	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_VALID_ENTRY,&dwValid,(DWORD)0 ,FALSE);	//4.0.22.504
	m_bValidEntry = dwValid;
	
	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_REPEAT,&dwTmp,0,FALSE);	
	m_lRepeat = dwTmp;
	
	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_CARD_READER,&dwTmp,0,FALSE);
	m_bCardReader = dwTmp;

	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_CONTACTLESS, &dwTmp, 0, FALSE);		// TD 460484 [7/28/2016 MaximB]
	m_bContacless = dwTmp;

	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_ENTER_CANCEL,&dwTmp,0,FALSE);
	m_bEnterCancel = dwTmp;
		
		
	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_YES_NO,&dwTmp,0,FALSE);
	m_bYesNo = dwTmp;
		
		
	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_PAY_INSIDE,&dwTmp,0,FALSE); //4.0.23.860 212541
	m_bPayInside = dwTmp;
			
	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_SCANNER,&dwTmp,0,FALSE);
	m_bEnableScanner = dwTmp;
		
		
	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_NUMERIC,&dwTmp,0,FALSE);
	m_bEnableNumeric = dwTmp;
		
		
	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_TIMEOUT,&dwTmp,NMP_DEFAULT_TIMEOUT,FALSE);
	if (dwTmp)
		m_lTimeout = dwTmp;
	else
		m_lTimeout = NMP_DEFAULT_TIMEOUT;
	
	char sTmpStr[MAX_FIELD_VAL];
	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_MESSAGE, (char *)sTmpStr, sizeof(sTmpStr), "", FALSE);	//4.0.20.10
	strcpy_s(m_sMessage, sizeof(m_sMessage), sTmpStr);
	
	// UDF - Epsilon
	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_OPEN_SOFT_KEYS, &dwTmp, 0, FALSE);
	m_bOpenSoftKeys = dwTmp;

	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_FIELD_ALTERNATE_ID, &dwTmp, 0, FALSE);
	m_bAlternateID = dwTmp;

	_Module.m_server.m_cParam.LoadParam(m_sPath, NMP_USE_NMP_OPTION, &dwTmp, 0, FALSE);
	m_bUseNMPOption = dwTmp;
}

void CNMPDetails::SetNmpDetails(short nNmpDetails)
{
	m_bCardReader = m_bEnableScanner = m_bEnableNumeric = m_bEnterCancel = m_bYesNo = m_bPayInside = m_bContacless = FALSE;

	if (nNmpDetails & ONMP_OpenCardReader)
		m_bCardReader = TRUE;

	if (nNmpDetails & ONMP_OpenContaless)	// TD 460484 [7/28/2016 MaximB]
		m_bContacless = TRUE;

	if (nNmpDetails & ONMP_OpenScanner)
		m_bEnableScanner = TRUE;

	if (nNmpDetails & ONMP_OpenNumericKeys)
		m_bEnableNumeric = TRUE;

	if (nNmpDetails & ONMP_OpenEnterCancelKeys)
		m_bEnterCancel = TRUE;

	if (nNmpDetails & ONMP_OpenYesNoKeys)
		m_bYesNo = TRUE;

	if (nNmpDetails & ONMP_OpenPayInsideKey)
		m_bPayInside = TRUE;
}

void CNMPDetails::LoadMessage(long lPumpNumber , CString sDefaultMessage)
{
	if ( (strlen(m_sMessage) == 0) || (ChrAll(m_sMessage, strlen(m_sMessage))))	//4.0.20.10
	{
		memcpy(m_sMessage, sDefaultMessage.GetBuffer() , min(sizeof(m_sMessage), sDefaultMessage.GetLength()));//4.0.22.511 74810
	}
	else
	{
		char sTmpStr[MAX_FIELD_VAL] = {0};
		char sTmpLanguagePath[MAX_FIELD_VAL] = {0};
		_Module.m_server.m_cCommonFunction.LoadDefaultLaguageString(lPumpNumber,sTmpLanguagePath,sizeof(sTmpLanguagePath));
		_Module.m_server.m_cParam.LoadParam((LPCSTR)sTmpLanguagePath, m_sMessage,(char *)sTmpStr, sizeof(sTmpStr), "" ,FALSE); //4.0.20.10 //4.0.20.260 67761

		if ((strlen(sTmpStr)) && (!ChrAll(sTmpStr, strlen(sTmpStr))))
		{
			memset(m_sMessage, 0 , sizeof(m_sMessage));
			memcpy(m_sMessage, sTmpStr, sizeof(m_sMessage));
		}
	}
}

/******************************************************************************
 Description: Sets open need more param info and pap swipe result command	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			12/10/2011       		Start			4.0.23.1220 TD 155754
******************************************************************************/
void CNMPDetails::OpenNeedMoreParam(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, char * strMsg, int iMaxNumOfOpenKeys, long lTimeOut, short nONMPProperties, PromptType ePromptType /*= PT_GENERIC*/)
{
	// copy the message
	memcpy(cTmpInfo.CardSaleInfo.cardData.sResultMsg,strMsg,sizeof(cTmpInfo.CardSaleInfo.cardData.sResultMsg));

	// copy the timeout
	l2a(lTimeOut, cTmpInfo.CardSaleExtraData2.sMessageTimeOut, sizeof(cTmpInfo.CardSaleExtraData2.sMessageTimeOut));

	// Set General Properties
	l2a(nONMPProperties, cTmpInfo.CardSaleExtraData4.sONMPProperties, sizeof(cTmpInfo.CardSaleExtraData4.sONMPProperties));

	// Set the prompt type
	l2a(ePromptType, cTmpInfo.CardSaleExtraData3.sPromptType, sizeof(cTmpInfo.CardSaleExtraData3.sPromptType));

	// Set minimum prompt length
	cTmpInfo.CardSaleExtraData3.sPromptMinLen[0] = '1';
	
	// Set maximum prompt length
	l2a(iMaxNumOfOpenKeys, cTmpInfo.CardSaleExtraData3.sPromptMaxLen, sizeof(cTmpInfo.CardSaleExtraData3.sPromptMaxLen));

	// removes "old" data that were sent to PumpSrv
	memset(cTmpInfo.CardSaleExtraData3.sParamResult , ' ' , sizeof(cTmpInfo.CardSaleExtraData3.sParamResult));  //4.0.23.1890
	memset(cTmpInfo.CardSaleExtraData5.sParamResultExt, ' ', sizeof(cTmpInfo.CardSaleExtraData5.sParamResultExt));

	// set open need more param
	cTmpInfo.CardSaleExtraData3.sParamResult[0]  = '?';

	// set screen number
	l2a(m_lScreenNumber,cTmpInfo.CardSaleExtraData5.sScreenMsgNumber, sizeof(cTmpInfo.CardSaleExtraData5.sScreenMsgNumber));

	// set the information into pump array
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;
	_Module.m_server.SetSwipeResultDataToCommand(cmdInfo,cTmpInfo,CD_MORE_DATA_REQUIRED);

	_Module.m_server.SetCommand(CMD_SWIPE_RESULT, 
		SERVER_POS_NUMBER, 
		lPumpNumber,
		&cmdInfo,
		sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));
}



void CNMPDetails::DebugDump() const {
	
	LogStream LogMsg;

	// add more for your debugging need
	LogMsg  << " NMPType=" << m_eNMPType 
			<< " NMPTime=" << m_eNMPTime
			<< " PATH=" << m_sPath
		    << " Valid=" << (m_bValidEntry ? 1 : 0)
			<< std::hex << std::uppercase
			<< std::setw(2)
		    << " OLAState=0x" << m_nOlaState
			<< std::dec
			<< " Repeat=" << m_lRepeat
		    << " CurrLoop=" << m_lCurrentLoopNum
			<< " CardReader=" << (m_bCardReader ? 1: 0)
			<< " Enter/Cancel=" << (m_bEnterCancel ? 1 : 0)
			<< " Y/N=" << (m_bYesNo ? 1 : 0)		
			<< " Scanner=" << (m_bEnableScanner ? 1 : 0)
			<< " Num=" << (m_bEnableNumeric ? 1 : 0)
			<< " TimeOut=" << m_lTimeout
			<< " PayInside=" <<(m_bPayInside ? 1 : 0)
			<< " OpenSoftKeys=" << (m_bOpenSoftKeys ? 1 : 0)
			<< " Alt_ID=" << (m_bAlternateID ? 1 : 0)
			<< " ContacLess=" << (m_bContacless ? 1 : 0)
			<< " ScreenNo=" << m_lScreenNumber		
			<< " NextFlowState=" << m_iNextFlowState
			<< " msg=" << m_sMessage
			<< std::ends;

	_LOGMSG.LogMsg(LogMsg);
}


CCouponsNMPDetails::CCouponsNMPDetails()
{
	m_eNMPType = NMP_TYPE_Coupon;
	SetStateType(OLA_LOYALTY_COUPONS_BEFORE_FUELING_BEFORE_START_TICKET);	
}
