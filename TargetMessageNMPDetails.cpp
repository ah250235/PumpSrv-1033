// TargetMessageNMPDetails.cpp: implementation of the CTargetMessageNMPDetails class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TargetMessageNMPDetails.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTargetMessageNMPDetails::CTargetMessageNMPDetails()
{
	m_eNMPType = NMP_TYPE_TargetMessage;
}

/******************************************************************************
 Description: Get Target Message from RTAPS and move OLA state machine

 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Omer			22/04/2012	 19:34		Start 312183
******************************************************************************/
void CTargetMessageNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola)
{	
	_Module.m_server.m_cFuelLoayaltyPS.GetTargetMessage(lPumpNumber);

	ola.m_byState = OLA_LOYALTY_WAIT_TARGET_MESSAGE_RESPONSE;
}

