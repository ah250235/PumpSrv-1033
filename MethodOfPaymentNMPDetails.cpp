#include "stdafx.h"
#include "MethodOfPaymentNMPDetails.h"

//4.0.23.20 76383
CMethodOfPaymentNMPDetails::CMethodOfPaymentNMPDetails()
{
	m_eNMPType = NMP_TYPE_MethodOfPayment;
	SetStateType(OLA_LOYALTY_METHOD_OF_PAYMENT);	
}


//4.0.23.20 76383
void CMethodOfPaymentNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola)
{
	long p = lPumpNumber - 1;
	DWORD lLanguageNum = 1;

	_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 
	_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&cTmpInfo);

	LoadMessage(lPumpNumber, "Press Yes to pay inside|or swipe payment");

	// copy the message
	memcpy(cTmpInfo.CardSaleInfo.cardData.sResultMsg,m_sMessage,sizeof(cTmpInfo.CardSaleInfo.cardData.sResultMsg));


	l2a(m_lTimeout, cTmpInfo.CardSaleExtraData2.sMessageTimeOut, sizeof(cTmpInfo.CardSaleExtraData2.sMessageTimeOut));
	
	short iBitMap = CreateGeneralNMPProperties();
	l2a((long) iBitMap, cTmpInfo.CardSaleExtraData4.sONMPProperties, sizeof(cTmpInfo.CardSaleExtraData4.sONMPProperties));	//4.0.20.50

	memset(cTmpInfo.CardSaleExtraData3.sParamResult, ' ', sizeof(cTmpInfo.CardSaleExtraData3.sParamResult));  //4.0.23.1890

	if (iBitMap & ONMP_OpenPayInsideKey) 
	{
		l2a(PT_GENERIC, cTmpInfo.CardSaleExtraData3.sPromptType, sizeof(cTmpInfo.CardSaleExtraData3.sPromptType));//General ONMP
	}
	else 
	{
		l2a(PT_BOOL_AND_CARD_READER, cTmpInfo.CardSaleExtraData3.sPromptType, sizeof(cTmpInfo.CardSaleExtraData3.sPromptType));//General ONMP
	}
	
	cTmpInfo.CardSaleExtraData3.sPromptMinLen[0] = '1';
	cTmpInfo.CardSaleExtraData4.sLoyaltyEntryMethod = ' '; //  [19/10/2020 MaximB]
	l2a(NMP_MAX_NUM_OF_OPEN_KEYS, cTmpInfo.CardSaleExtraData3.sPromptMaxLen, sizeof(cTmpInfo.CardSaleExtraData3.sPromptMaxLen));
	
	// RFUEL-3740
	if (GetUseNMPOption())
	{
		cTmpInfo.CardSaleInfo.cardData.sTrack2Data[0] = '?';
	}
	else
		cTmpInfo.CardSaleExtraData3.sParamResult[0]  = '?';
	
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;
	_Module.m_server.SetSwipeResultDataToCommand(cmdInfo, cTmpInfo, CD_MORE_DATA_REQUIRED);

	_Module.m_server.SetCommand(CMD_SWIPE_RESULT, 
		SERVER_POS_NUMBER, 
		lPumpNumber,
		&cmdInfo,
		sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));

	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "Execute Requested Method of payment NMP");
}
