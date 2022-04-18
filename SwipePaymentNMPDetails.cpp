#include "stdafx.h"
#include "SwipePaymentNMPDetails.h"


//4.0.22.500 start
CSwipePaymentNMPDetails::CSwipePaymentNMPDetails()
{
	m_eNMPType = NMP_TYPE_PaymentCard;
	SetStateType(OLA_LOYALTY_SWIPE_PAYMENT);	

}

// TD 339257
short CSwipePaymentNMPDetails::CreateGeneralNMPProperties()
{
	short iBitMap = CNMPDetails::CreateGeneralNMPProperties();

	if(_Module.m_server.IsPayPalActive())
		iBitMap |= ONMP_YesAsPayPal;
	iBitMap |= ONMP_OpenCardReader;

	return iBitMap;
}


//4.0.22.500 start
void CSwipePaymentNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	long p = lPumpNumber - 1;
	DWORD lLanguageNum = 1;

	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	LoadMessage(lPumpNumber,"Swipe Payment card");

	memcpy(cTmpInfo.CardSaleInfo.cardData.sResultMsg,m_sMessage,sizeof(cTmpInfo.CardSaleInfo.cardData.sResultMsg));

	l2a(m_lTimeout, cTmpInfo.CardSaleExtraData2.sMessageTimeOut, sizeof(cTmpInfo.CardSaleExtraData2.sMessageTimeOut));
	l2a(CreateGeneralNMPProperties() , cTmpInfo.CardSaleExtraData4.sONMPProperties, sizeof(cTmpInfo.CardSaleExtraData4.sONMPProperties));	//4.0.20.50

	memset(cTmpInfo.CardSaleExtraData3.sParamResult , ' ' , sizeof(cTmpInfo.CardSaleExtraData3.sParamResult));  //4.0.23.1890

	l2a(PT_GENERIC, cTmpInfo.CardSaleExtraData3.sPromptType, sizeof(cTmpInfo.CardSaleExtraData3.sPromptType));//General ONMP
	cTmpInfo.CardSaleExtraData3.sPromptMinLen[0] = '1';
	l2a(NMP_MAX_NUM_OF_OPEN_KEYS, cTmpInfo.CardSaleExtraData3.sPromptMaxLen, sizeof(cTmpInfo.CardSaleExtraData3.sPromptMaxLen));
	
	// RFUEL-3740
	if (GetUseNMPOption())
	{
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Execute Requested Swipe Payment card NMP - track2");
		cTmpInfo.CardSaleInfo.cardData.sTrack2Data[0] = '?';
	}
	else
		cTmpInfo.CardSaleExtraData3.sParamResult[0] = '?';

	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;
	_Module.m_server.SetSwipeResultDataToCommand(cmdInfo, cTmpInfo,CD_MORE_DATA_REQUIRED);

	_Module.m_server.SetCommand(CMD_SWIPE_RESULT, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Execute Requested Swipe Payment card NMP");
}
