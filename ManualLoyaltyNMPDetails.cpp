#include "stdafx.h"
#include "ManualLoyaltyNMPDetails.h"


CManualLoyaltyNMPDetails::CManualLoyaltyNMPDetails()
{
	SetEnableNumeric(TRUE);
	SetEnterCancel(TRUE);
	SetTimeout(NMP_DEFAULT_TIMEOUT);
	SetMessage("Please enter |Loyalty card manually");

}


void CManualLoyaltyNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	long p = lPumpNumber-1;
	DWORD lLanguageNum = 1;
	
	char sCurrentTerminalRegPath[MAX_FIELD_VAL] = "";

	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);
	
	char sRegMsg[MAX_FIELD_VAL] = {0};
	_Module.m_server.m_cCommonFunction.LoadMessageFromLanguagesReg(lPumpNumber,"LoyaltyManualPrompt",sRegMsg,sizeof(sRegMsg));
	// if there is a message in the registry - set it
	if ((strlen(sRegMsg)) && (!ChrAll(sRegMsg, strlen(sRegMsg))))
	{
		memset(m_sMessage, ' ', sizeof(m_sMessage));
		memcpy(m_sMessage, sRegMsg, sizeof(m_sMessage));
	}

	// copy the message
	memcpy(cTmpInfo.CardSaleInfo.cardData.sResultMsg,m_sMessage,sizeof(cTmpInfo.CardSaleInfo.cardData.sResultMsg));

	// copy the timeout
	l2a(m_lTimeout, cTmpInfo.CardSaleExtraData2.sMessageTimeOut, sizeof(cTmpInfo.CardSaleExtraData2.sMessageTimeOut));

	//4.0.20.420 - TD 68855 - Only if user chose manual - we need to disable the Scanner !
	l2a((CreateGeneralNMPProperties() | ONMP_LoyaltyCard) & (~ONMP_OpenScanner), cTmpInfo.CardSaleExtraData4.sONMPProperties, sizeof(cTmpInfo.CardSaleExtraData4.sONMPProperties));	//4.0.20.50

	// set the prompt type
	l2a(PT_GENERIC, cTmpInfo.CardSaleExtraData3.sPromptType, sizeof(cTmpInfo.CardSaleExtraData3.sPromptType));//General ONMP

	cTmpInfo.CardSaleExtraData3.sPromptMinLen[0] = '1';

	l2a(NMP_MAX_NUM_OF_OPEN_KEYS, cTmpInfo.CardSaleExtraData3.sPromptMaxLen, sizeof(cTmpInfo.CardSaleExtraData3.sPromptMaxLen));

	memset(cTmpInfo.CardSaleExtraData3.sParamResult , ' ' , sizeof(cTmpInfo.CardSaleExtraData3.sParamResult));  //4.0.23.1890

	// set open need more param
	cTmpInfo.CardSaleExtraData3.sParamResult[0]  = '?';

	// set the information into pump array
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;
	_Module.m_server.SetSwipeResultDataToCommand(cmdInfo, cTmpInfo,CD_MORE_DATA_REQUIRED);

	_Module.m_server.SetCommand(CMD_SWIPE_RESULT, 
		SERVER_POS_NUMBER, 
		lPumpNumber,
		&cmdInfo,
		sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Execute Requested Manual Loyalty NMP");
}
