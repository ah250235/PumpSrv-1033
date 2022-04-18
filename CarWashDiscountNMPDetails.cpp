#include "stdafx.h"
#include "CarWashDiscountNMPDetails.h"



//4.0.23.950 216343
CCarWashDiscountNMPDetails::CCarWashDiscountNMPDetails()
{
	m_eNMPType = NMP_TYPE_CarWashDiscount;
}

void CCarWashDiscountNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	long p = lPumpNumber - 1;

	_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 
	ola.m_byState = OLA_LOYALTY_CAR_WASH_DISCOUNT | SESSION_AUTHORIZE;
	memset(cTmpInfo.CardSaleInfo.cardData.sResultMsg, ' ', sizeof(cTmpInfo.CardSaleInfo.cardData.sResultMsg)); 

	_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"RunQuery for carwash");
}

//4.0.22.500 end

CCarWashNMPDetails::CCarWashNMPDetails()
{
	m_eNMPType = NMP_TYPE_CarWash;
	SetStateType(OLA_LOYALTY_CAR_WASH);	
}

/******************************************************************************
 Description: This is a regular car wash NMP session sent from PumpSrv. 
 Parameters:    PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			02/04/08   15:38		Start
******************************************************************************/

void CCarWashNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	long p = lPumpNumber-1;

	_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	memcpy(cTmpInfo.CardSaleInfo.cardData.sResultMsg, m_sMessage, sizeof(cTmpInfo.CardSaleInfo.cardData.sResultMsg));

	l2a(m_lTimeout, cTmpInfo.CardSaleExtraData2.sMessageTimeOut, sizeof(cTmpInfo.CardSaleExtraData2.sMessageTimeOut));
	l2a(CreateGeneralNMPProperties(), cTmpInfo.CardSaleExtraData4.sONMPProperties, sizeof(cTmpInfo.CardSaleExtraData4.sONMPProperties));
	cTmpInfo.CardSaleInfo.extraData.sCarWashProgram = '?';

	// RFUEL 2142 -  Set the prompt type to trigger Emerald flow (Carwash sale by FlowControl) on RFS
	l2a(PT_CAR_WASH, cTmpInfo.CardSaleExtraData3.sPromptType, sizeof(cTmpInfo.CardSaleExtraData3.sPromptType));


	CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;
	_Module.m_server.SetSwipeResultDataToCommand(cmdInfo, cTmpInfo,CD_MORE_DATA_REQUIRED);

	_Module.m_server.SetCommand(CMD_SWIPE_RESULT, 
		SERVER_POS_NUMBER, 
		lPumpNumber,
		&cmdInfo,
		sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"CCarWashNMPDetails::Execute Requested Car Wash");
}
