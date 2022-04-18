#include "stdafx.h"
#include "LoyaltyNMPDetails.h"


CLoyaltyNMPDetails::CLoyaltyNMPDetails()
{
	m_eNMPType = NMP_TYPE_LoyaltyCard;
	SetStateType(OLA_LOYALTY_SWIPE_LOYALTY);	//4.0.22.500
}




void CLoyaltyNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	long p = lPumpNumber-1;
	DWORD lLanguageNum = 1;
	CString sMsg;
	char sTmpStr[MAX_FIELD_VAL] = ""; //4.0.22.511 74810
	char sDefaultStr[MAX_FIELD_VAL] = "Swipe Loyalty|Yes = Manual|No  = Continue"; //4.0.22.511 74810

	if (_Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber) == CancelBeforeStartTicket) //4.0.23.330 115000 103166
	{
		sMsg.Format("CLoyaltyNMPDetails::Execute skip Swipe Loyalty card NMP, CancelTime = %ld",
			_Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber)); //4.0.23.610 133300 
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cTmpInfo, TRUE);
	}
	else
	{
		_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 
		_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

		if ((!strlen(m_sMessage)) || (ChrAll(m_sMessage, strlen(m_sMessage))))	//4.0.20.10
		{
			memcpy(m_sMessage, sDefaultStr , min(sizeof(m_sMessage), strlen(sDefaultStr)));//4.0.22.511 74810
		}
		else
		{
			char sTmpLanguagePath[MAX_FIELD_VAL] = {0};
			_Module.m_server.m_cCommonFunction.LoadDefaultLaguageString(lPumpNumber,sTmpLanguagePath,sizeof(sTmpLanguagePath));
			_Module.m_server.m_cParam.LoadParam((LPCSTR)sTmpLanguagePath, m_sMessage,(char *)sTmpStr, sizeof(sTmpStr), "",FALSE); //4.0.20.10 //4.0.20.260 67761
			if ((strlen(sTmpStr)) && (!ChrAll(sTmpStr, strlen(sTmpStr))))
			{
				memset(m_sMessage, ' ', sizeof(m_sMessage));
				memcpy(m_sMessage, sTmpStr, sizeof(m_sMessage));
			}
		}

		memcpy(cTmpInfo.CardSaleInfo.cardData.sResultMsg,m_sMessage,sizeof(cTmpInfo.CardSaleInfo.cardData.sResultMsg));
		
		l2a(m_lTimeout, cTmpInfo.CardSaleExtraData2.sMessageTimeOut, sizeof(cTmpInfo.CardSaleExtraData2.sMessageTimeOut));
		l2a(CreateGeneralNMPProperties() | ONMP_EnterAsYes | ONMP_LoyaltyCard, cTmpInfo.CardSaleExtraData4.sONMPProperties, sizeof(cTmpInfo.CardSaleExtraData4.sONMPProperties));	//4.0.20.50

		short iBitMap = CreateGeneralNMPProperties();

		l2a(iBitMap | ONMP_EnterAsYes | ONMP_LoyaltyCard, cTmpInfo.CardSaleExtraData4.sONMPProperties, sizeof(cTmpInfo.CardSaleExtraData4.sONMPProperties));	//4.0.20.50

		memset(cTmpInfo.CardSaleExtraData3.sParamResult , ' ' , sizeof(cTmpInfo.CardSaleExtraData3.sParamResult));  //4.0.23.1890

		if (iBitMap & ONMP_AlternateID)
		{
			l2a(PT_BOOL_AND_CARD_READER_AND_ALT_ID, cTmpInfo.CardSaleExtraData3.sPromptType, sizeof(cTmpInfo.CardSaleExtraData3.sPromptType));//General ONMP
		}
		else
		{
			l2a(PT_GENERIC, cTmpInfo.CardSaleExtraData3.sPromptType, sizeof(cTmpInfo.CardSaleExtraData3.sPromptType));//General ONMP
		}

		cTmpInfo.CardSaleExtraData3.sPromptMinLen[0] = '1';
		l2a(NMP_MAX_NUM_OF_OPEN_KEYS, cTmpInfo.CardSaleExtraData3.sPromptMaxLen, sizeof(cTmpInfo.CardSaleExtraData3.sPromptMaxLen));
		cTmpInfo.CardSaleExtraData3.sParamResult[0]  = '?';
		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

		CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;
		_Module.m_server.SetSwipeResultDataToCommand(cmdInfo, cTmpInfo,CD_MORE_DATA_REQUIRED);

		_Module.m_server.SetCommand(CMD_SWIPE_RESULT, 
			SERVER_POS_NUMBER, 
			lPumpNumber,
			&cmdInfo,
			sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));

		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Execute Requested Loyalty card NMP");
	}
}

void CLoyaltyNMPDetails::LoadConfiguration()
{
	CNMPDetails::LoadConfiguration();

	CString strReg;
	strReg.Format("Languages\\Language%02d", 01);
	_Module.m_server.m_cParam.LoadParam(strReg, NMP_FIELD_ALTERNATE_ID_RETRY, (char *)m_sAlternateIDRetrylMsg, sizeof(m_sAlternateIDRetrylMsg), "You Entered %s: | This number is not registered", TRUE);

	if ((0 == strlen(m_sAlternateIDRetrylMsg)) || (ChrAll(m_sAlternateIDRetrylMsg, strlen(m_sAlternateIDRetrylMsg))))
	{
		sprintf(m_sAlternateIDRetrylMsg, "This number is not registered!");
	}
}

void CLoyaltyNMPDetails::BuildAlternateIDRetryMessage(PAY_AT_PUMP_INFO cInfo, long lPumpNumber, CString & sMsg)
{
	sMsg = m_sAlternateIDRetrylMsg;
	int iResult = sMsg.Find("%s");
	if (-1 != iResult)
	{
		char tmp[100] = { 0 };

		char *p = strstr((char*)cInfo.CardSaleInfo.extraData.sLoyalId, "   ");
		DWORD dwPrintMesLen = p - (char*)cInfo.CardSaleInfo.extraData.sLoyalId;

		if (dwPrintMesLen > 0)
		{
			_snprintf(tmp, dwPrintMesLen, "%s", cInfo.CardSaleInfo.extraData.sLoyalId);
			sMsg.Replace("%s", tmp);
		}
	}
}

void CLoyaltyNMPDetails::GetMessageForTerminal(long lPumpNumber, PAY_AT_PUMP_INFO &cTmpInfo, int iSizeOfOutMsg, char * sOutMsg)
{
	if ((0 == strlen(m_sMessage)) || (ChrAll(m_sMessage, strlen(m_sMessage))))	//4.0.20.10
	{
		memset(sOutMsg, ' ', iSizeOfOutMsg);
		memcpy(m_sMessage, sOutMsg, min(sizeof(m_sMessage), strlen(sOutMsg)));	//4.0.22.504 69344
	}
	else
	{
		char sTmpStr[MAX_FIELD_VAL] = { 0 };
		char sTmpLanguagePath[MAX_FIELD_VAL] = { 0 };

		_Module.m_server.m_cCommonFunction.LoadDefaultLaguageString(lPumpNumber, sTmpLanguagePath, sizeof(sTmpLanguagePath));
		_Module.m_server.m_cParam.LoadParam((LPCSTR)sTmpLanguagePath, m_sMessage, (char *)sTmpStr, sizeof(sTmpStr), "", FALSE);
	}
}

void CLoyaltyNMPDetails::StartAlternateIDRetryFlow(PAY_AT_PUMP_INFO cInfo, long lPumpNumber, OLA_STAT * pOla)
{
	CString sMsg;
	BuildAlternateIDRetryMessage(cInfo, lPumpNumber, sMsg);

	_Module.m_server.OpenNeedMoreParam(cInfo, lPumpNumber, sMsg.GetBuffer(0), NMP_MAX_NUM_OF_OPEN_KEYS, NMP_DEFAULT_TIMEOUT, ONMP_OpenEnterCancelKeys | ONMP_OpenSoftKeys);

	pOla->m_byState = OLA_LOYALTY_WAIT_FOR_ALTERNATE_ID_RETRY_ANSWER;

	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&cInfo);
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(pOla);   //4.0.23.2805
	_Module.m_server.SavePumpInfo(lPumpNumber);
}
