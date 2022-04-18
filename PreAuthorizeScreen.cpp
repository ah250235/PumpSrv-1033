#include "stdafx.h"
#include "PreAuthorizeScreen.h"


CPreAuthorizeScreen::CPreAuthorizeScreen(void)
{
	m_nOlaState = OLA_LOYALTY_DISPLAY_PRE_AUTHOIRZE_SCREEN;
}


CPreAuthorizeScreen::~CPreAuthorizeScreen(void)
{
}

void CPreAuthorizeScreen::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	long p = lPumpNumber-1;

	_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 

	CString sTmp("");
	SetMessage(sTmp.GetBuffer(0));

	OpenNeedMoreParam(cTmpInfo, lPumpNumber, m_sMessage, 1, NMP_DEFAULT_TIMEOUT, CreateGeneralNMPProperties());
	
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "CPreAuthorizeScreen::Execute");
}

short CPreAuthorizeScreen::CreateGeneralNMPProperties()
{
	short iBitMap = 0;

	iBitMap |= ONMP_OpenYesNoKeys;

	iBitMap |= ONMP_YesAsEnter;

	iBitMap |= ONMP_NoAsCancel;

	iBitMap |= ONMP_OtherAmount;

	return iBitMap;
}

