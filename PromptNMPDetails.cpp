#include "stdafx.h"
#include "PromptNMPDetails.h"


CPromptNMPDetails::CPromptNMPDetails()
{
	m_eNMPType = NMP_TYPE_MessagePrompt;
	
}

void CPromptNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola,PromptType ePromptType /*= PT_GENERIC*/,INT iMaxNumOfOpenKeys /*= NMP_MAX_NUM_OF_OPEN_KEYS*/,short nONMPProperties /*= ONMP_OpenYesNoKeys | ONMP_NoAsCancel*/)
{
	long p = lPumpNumber-1;

	_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 

	GetMessageForTerminal(lPumpNumber,cTmpInfo);

	memcpy(cTmpInfo.CardSaleInfo.cardData.sResultMsg,m_sMessage,sizeof(cTmpInfo.CardSaleInfo.cardData.sResultMsg));

	l2a(m_lTimeout, cTmpInfo.CardSaleExtraData2.sMessageTimeOut, sizeof(cTmpInfo.CardSaleExtraData2.sMessageTimeOut));

	l2a(nONMPProperties, cTmpInfo.CardSaleExtraData4.sONMPProperties, sizeof(cTmpInfo.CardSaleExtraData4.sONMPProperties));
	

	memset(cTmpInfo.CardSaleExtraData3.sParamResult , ' ' , sizeof(cTmpInfo.CardSaleExtraData3.sParamResult));  //4.0.23.1890

	l2a(ePromptType, cTmpInfo.CardSaleExtraData3.sPromptType, sizeof(cTmpInfo.CardSaleExtraData3.sPromptType));//General ONMP
	cTmpInfo.CardSaleExtraData3.sPromptMinLen[0] = '1';

	l2a(iMaxNumOfOpenKeys, cTmpInfo.CardSaleExtraData3.sPromptMaxLen, sizeof(cTmpInfo.CardSaleExtraData3.sPromptMaxLen));
	cTmpInfo.CardSaleExtraData3.sParamResult[0]  = '?';

	// set screen number
	l2a(m_lScreenNumber, cTmpInfo.CardSaleExtraData5.sScreenMsgNumber, sizeof(cTmpInfo.CardSaleExtraData5.sScreenMsgNumber));

	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;
	_Module.m_server.SetSwipeResultDataToCommand(cmdInfo, cTmpInfo,CD_MORE_DATA_REQUIRED);

	_Module.m_server.SetCommand(CMD_SWIPE_RESULT, 
		SERVER_POS_NUMBER, 
		lPumpNumber,
		&cmdInfo,
		sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Execute Requested Prompt NMP");
}

void CPromptNMPDetails::GetMessageForTerminal( long lPumpNumber ,PAY_AT_PUMP_INFO & cTmpInfo)
{
	// in case m_sMessage in NULL or empty load the default string	
	if ( (strlen(m_sMessage) == 0) || (ChrAll(m_sMessage, strlen(m_sMessage))))	//4.0.20.10
	{
		char sDefaultStr[MAX_FIELD_VAL] = "Do you want to continue?"; //4.0.22.511 74810
		memcpy(m_sMessage, sDefaultStr , min(sizeof(m_sMessage), strlen(sDefaultStr)));//4.0.22.511 74810
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

