#include "stdafx.h"
#include "SwipeCardNMPDetalis.h"

CSwipeCardNMP::CSwipeCardNMP(void)
{
	m_nOlaState = OLA_LOYALTY_SWIPE_CARD;
}

/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			12/10/2011       		Start			4.0.23.1220 TD 155754
******************************************************************************/
void CSwipeCardNMP::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	OpenNeedMoreParam(cTmpInfo, lPumpNumber, m_sMessage, 1, m_lTimeout, CreateGeneralNMPProperties());
}

short CSwipeCardNMP::CreateGeneralNMPProperties()
{
	short iBitMap = 0;

	iBitMap |= ONMP_OpenCardReader;

	iBitMap |= ONMP_OpenEnterCancelKeys;

	if (m_bContacless)
		iBitMap |= ONMP_OpenContaless; // TD 460484 [7/28/2016 MaximB]

	return iBitMap;
}

