#include "stdafx.h"
#include "PapFlow.h"

CPapFlow::CPapFlow()
{

}

CPapFlow::~CPapFlow()
{

}

CPapFlow* CPapFlow::getInstance()
{
	if (!m_bInstanceFlag)
	{
		m_pInstance = new CPapFlow();
		m_bInstanceFlag = TRUE;
	}
	return m_pInstance;

}


BOOL	CPapFlow::Init(BOOL bReconfigure)
{
	BOOL bRetVal = TRUE;

	try
	{
		this->CBaseFlow::Init(bReconfigure);
		LoadNMPConfiguration(bReconfigure, NULL);	
		if (_Module.m_server.GetFlowControlType() != DYNAMIC_PAP_FLOW)
		{
			DWORD dwLoyaltyOrder = GetNMPOrder(m_psNMPRegistryEntries[NMP_TYPE_LoyaltyCard]->GetPath());
			DWORD dwPaymentOrder = GetNMPOrder(m_psNMPRegistryEntries[NMP_TYPE_PaymentCard]->GetPath());	
	
			if ((dwPaymentOrder < dwLoyaltyOrder) || (dwLoyaltyOrder == 0))
			{
				_Module.m_server.m_cFuelLoayaltyPS.SetLoyaltyFlowPaymentFirst(TRUE);
				_LOGMSG.LogMsg("Init Pap, Swipe payment before loyalty.");
			}
			else
			{
				_Module.m_server.m_cFuelLoayaltyPS.SetLoyaltyFlowPaymentFirst(FALSE);
				_LOGMSG.LogMsg("Init Pap, Swipe loyalty before payment.");
			}
		}

	}
	catch (...)
	{
		_LOGMSG.LogMsg("Init Pap, Unexpected error has occurred");
		bRetVal = FALSE;
	}
	return bRetVal;

}


/******************************************************************************
 Description: Starting the flow control. Setting the relevant state
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			08/02/09	9:00		Start 4.0.22.504	69344
 Liat			26/05/09	12:13		Update 4.0.22.508 74810
******************************************************************************/
void CPapFlow::StartFlowControl(long lPumpNumber, short iStartOlaNMPState, OLA_STAT & ola)	//4.0.22.504	69344
{
	if (FALSE == _Module.m_server.PumpNotValid(lPumpNumber))
	{
		//if iStartOlaNMPState == 0 it means start from the first entry according to the flow.
		_Module.m_server.m_cFlowControlManager.SetStartOlaNMPState(lPumpNumber,iStartOlaNMPState); // TD 359309
		_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_COMPLETE);//4.0.23.390 116544
		if (_Module.m_server.m_cFuelLoayaltyPS.IsCopientFlowControlInUse())
		{
			ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_LOYALTY_WAIT_RESPONSE);
			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola);
			StartAfterStartTicketFlowControl(lPumpNumber, &ola);
		}
		else
		{
			ola.m_byState = OLA_LOYALTY_START_TICKET | SESSION_AUTHORIZE;	//Start ticket is always the first step.
			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola);
			_Module.m_server.SavePumpInfo(lPumpNumber);
		}
	}
	else
	{
		_LOGMSG.LogClassMsg("CPapFlow", "StartFlowControl", lPumpNumber, LOG_PUMP, "Invalid pump number!");
	}
}




void CPapFlow::DeleteFlow()
{
	if (m_pInstance != 0) {
		delete m_pInstance;
		m_pInstance = NULL;
	}

	m_bInstanceFlag = FALSE;

}



void CPapFlow::LoadNMPConfiguration(BOOL bReconfigure, char *sPath)	//4.0.22.508 74810
{
	CBaseFlow::LoadNMPConfiguration(bReconfigure, NMP_FLOW_CONTROL_PAP_PATH);
}