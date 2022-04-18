#include "stdafx.h"
#include "AltrenateIDNMPDetails.h"


CAlternateIDNMPDetails::CAlternateIDNMPDetails()
{
	SetEnableNumeric(TRUE);
	SetEnterCancel(TRUE);
	SetTimeout(NMP_DEFAULT_TIMEOUT);
	SetMessage("Enter Alternate ID");
	SetStateType(OLA_LOYALTY_ALTERNATE_ID);
	SetAlternateID(TRUE);
	SetOpenSoftKeys(TRUE);
}

void CAlternateIDNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola)
{
	long p = lPumpNumber - 1;
	DWORD lLanguageNum = 1;

	char sCurrentTerminalRegPath[MAX_FIELD_VAL] = "";

	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&cTmpInfo);

	char sRegMsg[MAX_FIELD_VAL] = { 0 };
	if (ChrAll(m_sMessage, sizeof(m_sMessage), ' '))
	{
		_Module.m_server.m_cCommonFunction.LoadMessageFromLanguagesReg(lPumpNumber, "EnterAlternateID", sRegMsg, sizeof(sRegMsg), "Enter Alternate ID");
	}
	// if there is a message in the registry - set it
	if ((strlen(sRegMsg)) && (!ChrAll(sRegMsg, strlen(sRegMsg))))
	{
		memset(m_sMessage, ' ', sizeof(m_sMessage));
		memcpy(m_sMessage, sRegMsg, sizeof(m_sMessage));
	}

	// copy the message
	memcpy(cTmpInfo.CardSaleInfo.cardData.sResultMsg, m_sMessage, min(sizeof(m_sMessage), sizeof(cTmpInfo.CardSaleInfo.cardData.sResultMsg))); //4.0.20.40 

																																			   // copy the timeout
	l2a(m_lTimeout, cTmpInfo.CardSaleExtraData2.sMessageTimeOut, sizeof(cTmpInfo.CardSaleExtraData2.sMessageTimeOut));

	// in PT_BOOL_AND_CARD_READER_AND_ALT_ID - the need more param sONMPProperties is set in RFS/CL
	l2a(CreateGeneralNMPProperties(), cTmpInfo.CardSaleExtraData4.sONMPProperties, sizeof(cTmpInfo.CardSaleExtraData4.sONMPProperties));	//4.0.20.50

																																																										// set the prompt type
	l2a(PT_BOOL_AND_CARD_READER_AND_ALT_ID, cTmpInfo.CardSaleExtraData3.sPromptType, sizeof(cTmpInfo.CardSaleExtraData3.sPromptType));//General ONMP

	cTmpInfo.CardSaleExtraData3.sPromptMinLen[0] = '1';

	l2a(NMP_MAX_NUM_OF_OPEN_KEYS, cTmpInfo.CardSaleExtraData3.sPromptMaxLen, sizeof(cTmpInfo.CardSaleExtraData3.sPromptMaxLen));

	memset(cTmpInfo.CardSaleExtraData3.sParamResult, ' ', sizeof(cTmpInfo.CardSaleExtraData3.sParamResult));  //4.0.23.1890

																											  // QC 503093
	cTmpInfo.CardSaleExtraData3.sPromptMode = ' ';

	// set open need more param
	cTmpInfo.CardSaleExtraData3.sParamResult[0] = '?';

	cTmpInfo.CardSaleExtraData3.sPromptMinLen[0] = '1';

	l2a(NMP_MAX_NUM_OF_OPEN_KEYS, cTmpInfo.CardSaleExtraData3.sPromptMaxLen, sizeof(cTmpInfo.CardSaleExtraData3.sPromptMaxLen));

	cTmpInfo.CardSaleExtraData3.sParamResult[0] = '?';

	// set the information into pump array
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&cTmpInfo);

	CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;
	_Module.m_server.SetSwipeResultDataToCommand(cmdInfo, cTmpInfo, CD_MORE_DATA_REQUIRED);


	_Module.m_server.SetCommand(CMD_SWIPE_RESULT,
		SERVER_POS_NUMBER,
		lPumpNumber,
		&cmdInfo,
		sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));


	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Execute Requested Alternate ID NMP");


}


CAlternateIDRetryNMPDetails::CAlternateIDRetryNMPDetails()
{
	SetEnableNumeric(TRUE);
	SetEnterCancel(TRUE);
	SetOpenSoftKeys(TRUE);


	SetTimeout(NMP_DEFAULT_TIMEOUT);
	SetStateType(OLA_LOYALTY_WAIT_FOR_ALTERNATE_ID_RETRY_ANSWER);
	SetAlternateID(TRUE);
	
}

void CAlternateIDRetryNMPDetails::Execute(PAY_AT_PUMP_INFO & cInfo, long lPumpNumber, OLA_STAT & ola)
{
	_Module.m_server.OpenNeedMoreParam(cInfo, lPumpNumber, cInfo.CardSaleInfo.cardData.sResultMsg, _Module.m_server.m_lMaxAlternateIDInputData, m_lTimeout, CreateGeneralNMPProperties(), SCREEN_ENTER_ALTERNATE_ID_RETRY, PT_NON_MASK_NUM);

	ola.m_byState = OLA_LOYALTY_WAIT_FOR_ALTERNATE_ID_RETRY_ANSWER;

	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&cInfo);
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(&ola);   //4.0.23.2805
	_Module.m_server.SavePumpInfo(lPumpNumber);

	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Execute Alternate ID Retry NMP");
}