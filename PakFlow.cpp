#include "stdafx.h"
#include "PakFlow.h"



CPakFlow::CPakFlow()
{
}

CPakFlow* CPakFlow::getInstance()
{
	if (!m_bInstanceFlag)
	{
		m_pInstance = new CPakFlow();
		m_bInstanceFlag = TRUE;
	}
	return m_pInstance;
}
/******************************************************************************
 Description: Starting the flow control. Setting the relevant state
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			09/02/09	12:22		Start 4.0.22.504	69344
 Liat			26/05/09	14:16		Update 4.0.22.508 74810
******************************************************************************/
void	CPakFlow::StartFlowControl(long lPumpNumber, short iStartOlaNMPState, OLA_STAT & ola)	//4.0.22.504	69344
{
	if (FALSE == PUMP_NOT_VALID(lPumpNumber))
	{
		//if iStartOlaNMPState == 0 it means start from the first entry according to the flow.
		_Module.m_server.m_cFlowControlManager.SetStartOlaNMPState(lPumpNumber,iStartOlaNMPState); // TD 359309
		_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, LOYALTY_STATE_COMPLETE);//4.0.23.390 116544
		ola.m_byState = OLA_LOYALTY_START_TICKET | SESSION_AUTHORIZE;	//Start ticket is always the first step.
		_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola);
		_Module.m_server.SavePumpInfo(lPumpNumber);
	}
	else
	{	
		_LOGMSG.LogClassMsg("CPakFlow", "StartFlowControl", lPumpNumber, LOG_PUMP, "Invalid pump number!");
	}
}

void CPakFlow::DeleteFlow()
{
	if(m_pInstance != 0)
		delete m_pInstance;

	m_bInstanceFlag = FALSE;

}

short	CPakFlow::GetNextStateAfterFlowControl() //4.0.23.860 212541
{
	return (OLA_LOYALTY_START_FUELING);
}


void	CPakFlow::UpdateQueryOrderInTheFlow()	//4.0.23.860 212541
{
	DWORD dwTmp = 0;
	DWORD dwOrder = 0, dwAuthLoyaltyOrder = 0;	//4.0.22.500
	if (m_psNMPRegistryEntries[NMP_TYPE_DeferralDiscount]->GetValidEntry())
	{
		//Put the query before the deferral if exists
		dwOrder = GetNMPOrder(m_psNMPRegistryEntries[NMP_TYPE_DeferralDiscount]->GetPath());
		for (int i = NMP_MAX_TYPE_OPTIONS - 1; i >= (int)dwOrder; i--)
		{
			m_psOrderNMP[i] = m_psOrderNMP[i - 1];
		}
		if (dwOrder - 1 >= 0 && dwOrder - 1 < NMP_MAX_TYPE_OPTIONS) m_psOrderNMP[dwOrder - 1] = m_psNMPRegistryEntries[NMP_TYPE_RunQuery];
		//deferral is the last order, fueling must follow dwOrder
		if (dwOrder + 1 >= 0 && dwOrder + 1 < NMP_MAX_TYPE_OPTIONS) m_psOrderNMP[dwOrder + 1] = m_psNMPRegistryEntries[NMP_TYPE_Fueling];

	}
	else if (m_psOrderNMP[0] == NULL)
	{
		m_psOrderNMP[0] = m_psNMPRegistryEntries[NMP_TYPE_RunQuery];
		m_psOrderNMP[1] = m_psNMPRegistryEntries[NMP_TYPE_Fueling];

	}
	else 
	{
		//if no deferral exists in the flow- call query before fueling
		dwAuthLoyaltyOrder = GetNMPOrder(m_psNMPRegistryEntries[NMP_TYPE_AuthorizeLoyalty]->GetPath());	
		m_psOrderNMP[dwAuthLoyaltyOrder] = m_psNMPRegistryEntries[NMP_TYPE_RunQuery];
		m_psOrderNMP[dwAuthLoyaltyOrder + 1] = m_psNMPRegistryEntries[NMP_TYPE_Fueling];

	}

}

void CPakFlow::LoadNMPRegistryEntries()	//4.0.22.508 74810
{
	DWORD dwValid = 0;
	DWORD dwTmp = 0;
	DWORD dwOrder = 0, dwFuelingOrder = 0;	//4.0.22.500
	char sPath[MAX_PATH] = {0};
	CString strMsg;

	char* szNMPNames[NMP_MAX_TYPE_OPTIONS] = {"Car Wash", "Coupon", "Swipe Loyalty", "Prompt", "Deferral Discount", "Swipe Payment", 
		"Authorize Payment", "Authorize Loyalty", "Fueling",   "Method Of Payment",    "Run Query"   , "Alternate ID (phonelookup)" , "Car Wash Discount","14"}; 

	

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		_LOGMSG.LogMsg("Loading need more param configuration from the registry.");
	}

	for (int iNmpType=0 ; iNmpType< NMP_MAX_TYPE_OPTIONS; iNmpType++)
	{
		if (dwOrder = GetNMPOrder(m_psNMPRegistryEntries[iNmpType]->GetPath()))
		{
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				strMsg.Format("Found requested NMP type %d = %s, order = %ld", iNmpType, szNMPNames[iNmpType], dwOrder);	//4.0.22.500
				_LOGMSG.LogMsg(strMsg);
			}
			m_psOrderNMP[dwOrder - 1] = m_psNMPRegistryEntries[iNmpType];	//Order starts from 1, so we need to decrease it when saving to the array
		}
		if ((iNmpType == NMP_TYPE_Coupon) || (iNmpType == NMP_TYPE_LoyaltyCard) || (iNmpType == NMP_TYPE_DeferralDiscount) || (iNmpType == NMP_TYPE_AuthorizeLoyalty))//4.0.22.504
		{
			m_psNMPRegistryEntries[iNmpType]->LoadConfiguration(); // 4.0.23.???? TD 295237
		}
	}

	//update car wash time
	//4.0.22.500 start
	if (m_psNMPRegistryEntries[NMP_TYPE_CarWash]->GetValidEntry())
	{
		dwOrder = GetNMPOrder(m_psNMPRegistryEntries[NMP_TYPE_CarWash]->GetPath());
		dwFuelingOrder = GetNMPOrder(m_psNMPRegistryEntries[NMP_TYPE_Fueling]->GetPath());	//fueling always has to be valid in the flow.

		if (dwOrder > dwFuelingOrder)
			bCarWashAfterFueling = TRUE;
		else
			bCarWashAfterFueling = FALSE;
	}
	else	//4.0.19.508
		bCarWashAfterFueling = FALSE;
	UpdateQueryOrderInTheFlow();  //4.0.23.860 212541

}

BOOL	CPakFlow::Init(BOOL bReconfigure)
{
	BOOL bRetVal = TRUE;

	try
	{
		CBaseFlow::Init(bReconfigure);
		LoadNMPConfiguration(bReconfigure, NULL);	


	}
	catch (...)
	{
		_LOGMSG.LogMsg("Init Pap, Unexpected error has occurred");
		bRetVal = FALSE;
	}
	return bRetVal;

}

void CPakFlow::LoadNMPConfiguration(BOOL bReconfigure, char *sPath)	//4.0.22.508 74810
{
	CBaseFlow::LoadNMPConfiguration(bReconfigure, NMP_FLOW_CONTROL_PAK_PATH);
}