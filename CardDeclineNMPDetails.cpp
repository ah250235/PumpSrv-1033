#include "stdafx.h"
#include "CardDeclineNMPDetails.h"


CCardDeclineNMPDetails::CCardDeclineNMPDetails(void)
{
	m_nOlaState = OLA_LOYALTY_DECLINE;
}


CCardDeclineNMPDetails::~CCardDeclineNMPDetails(void)
{
}

void CCardDeclineNMPDetails::Execute( PAY_AT_PUMP_INFO & cInfo, long lPumpNumber, OLA_STAT & ola )
{
	long p = lPumpNumber-1;

	_Module.m_server.m_cFuelLoayaltyPS.ClearTimer(p);

	ola.m_byState = OLA_LOYALTY_DECLINE;
	cInfo.CardSaleInfo.cardData.sTranStatus = CD_DECLINE_BADCARD;

	//_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
	//_Module.m_server.m_cPumpArray[p].SetPAPInfo(&cInfo);

	CARD_SALE_ALL3		cCardSaleAll3;
	memset(&cCardSaleAll3, ' ', sizeof(cCardSaleAll3));
	_Module.m_server.m_cCommonFunction.ConvertPayAtPumpInfoToCardSaleAll3(cCardSaleAll3, cInfo);

	CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;
	_Module.m_server.SetSwipeResultDataToCommand(cmdInfo, cInfo,CD_FORCE_NOT_OK);

	_Module.m_server.SendSwipeResult(SESSION_AUTHORIZE,lPumpNumber, CD_NOT_OK,CD_DECLINE_MODE, &cCardSaleAll3);	//4.0.20.503	66960

	/*
	_Module.m_server.SetCommand(CMD_SWIPE_RESULT, 
		SERVER_POS_NUMBER, 
		lPumpNumber,
		&cmdInfo,
		sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));
		*/

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Card Declined!");
}

