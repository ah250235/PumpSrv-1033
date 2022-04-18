// FuelAuthorizeLimitValidation.cpp: implementation of the FuelAuthorizeLimitValidation class.
// Created //4.0.22.501 TD 70956
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FuelingRestriction.h"

#include "ForecourtFuelingRestriction.h"
#include "POSFuelingRestriction.h"

CBaseFuelingRestriction* CFuelingRestriction::m_cFuelingRestrictions[FUELING_RESTRICTIONS_NUMBER];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFuelingRestriction::CFuelingRestriction()
{
	for (int i = 0; i < FUELING_RESTRICTIONS_NUMBER; i++)
	{
		m_cFuelingRestrictions[i] = NULL;
	}	
}


CFuelingRestriction::~CFuelingRestriction()
{
	for (int i = 0; i < FUELING_RESTRICTIONS_NUMBER; i++)
	{
		if(m_cFuelingRestrictions[i] != NULL)
			delete m_cFuelingRestrictions[i];
	}	
}


/******************************************************************************
 Description:	
 Returns:      	BOOL
 Parameters:   	PumpReservation

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CFuelingRestriction::Init(BOOL bCreate)
{
	if(NULL == m_cFuelingRestrictions[0])
	{
		m_cFuelingRestrictions[0] = new CForecourtFuelingRestriction();
		m_cFuelingRestrictions[1] = new CPOSFuelingRestriction();
		m_cFuelingRestrictions[2] = new CCashierFuelingRestriction();	//4.0.22.504
	}

	for (int i = 0; i < FUELING_RESTRICTIONS_NUMBER; i++)
	{
		if(NULL != m_cFuelingRestrictions[i])
			m_cFuelingRestrictions[i]->Init(bCreate);		 
	}
}


/******************************************************************************
 Description:	
 Returns:      	BOOL
 Parameters:   	PumpReservation

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

BOOL CFuelingRestriction::IsApproveAllowed(const LONG lPumpNumber, const LONG lPOSNumber)
{
	BOOL bRetCode = TRUE;
	RestrictionType sLocalRestrictionType(lPOSNumber);

	for (int i = 0; i < FUELING_RESTRICTIONS_NUMBER && bRetCode; i++)
	{		 
		if(m_cFuelingRestrictions[i] != NULL && m_cFuelingRestrictions[i]->IsActive())
			bRetCode = m_cFuelingRestrictions[i]->IsApproveAllowed(lPumpNumber, sLocalRestrictionType);		
	}

	return bRetCode;
}


/******************************************************************************
 Description:	
 Returns:      	BOOL
 Parameters:   	PumpReservation

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CFuelingRestriction::Restrict(const LONG lPumpNumber, const PumpReservation eNewPumpRes, const PumpStatus eNewPumpStatus, 
				const PumpResExt eNewPumpResExt, const PumpReservation ePrevPumpRes, const PumpStatus ePrevPumpStatus)
{
	RestrictionType sLocalRestrictionType(NO_POS_NUMBER, eNewPumpRes, eNewPumpStatus, eNewPumpResExt, ePrevPumpRes, ePrevPumpStatus);

	SetRestriction(lPumpNumber, sLocalRestrictionType);
	ClearRestriction(lPumpNumber, sLocalRestrictionType);
}


/******************************************************************************
 Description:	AddCashier on sign in from POS
 Returns:      	void
 Parameters:   	none

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/
void CFuelingRestriction::CashierSignIn()
{
	if(m_cFuelingRestrictions[2]->IsActive())
			m_cFuelingRestrictions[2]->CashierSignIn();
}


void CFuelingRestriction::CashierSignOut()
{
	if(m_cFuelingRestrictions[2]->IsActive())
			m_cFuelingRestrictions[2]->CashierSignOut();
}

void CFuelingRestriction::ResetCashiersCounter()
{
	if(m_cFuelingRestrictions[2]->IsActive())
			m_cFuelingRestrictions[2]->ResetCashiersCounter();
}

/******************************************************************************
 Description:	
 Returns:      	BOOL
 Parameters:   	PumpReservation

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CFuelingRestriction::CleanUp()
{
	for (int i = 0; i < FUELING_RESTRICTIONS_NUMBER; i++)
	{		 
		if(m_cFuelingRestrictions[i] != NULL && m_cFuelingRestrictions[i]->IsActive())
			m_cFuelingRestrictions[i]->CleanUp();		
	}
}

//*******************************************************************************************************************
//						PROTECTED METHODS
//*******************************************************************************************************************

/******************************************************************************
 Description:	
 Returns:      	BOOL
 Parameters:   	PumpReservation

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CFuelingRestriction::SetRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType)
{
	for (int i = 0; i < FUELING_RESTRICTIONS_NUMBER; i++)
	{		 
		if(m_cFuelingRestrictions[i] != NULL && m_cFuelingRestrictions[i]->IsActive())
			m_cFuelingRestrictions[i]->SetRestriction(lPumpNumber, rRestrictionType);		
	}
}


/******************************************************************************
 Description:	
 Returns:      	BOOL
 Parameters:   	PumpReservation

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CFuelingRestriction::ClearRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType)
{
	for (int i = 0; i < FUELING_RESTRICTIONS_NUMBER; i++)
	{		 
		if(m_cFuelingRestrictions[i] != NULL && m_cFuelingRestrictions[i]->IsActive())
			m_cFuelingRestrictions[i]->ClearRestriction(lPumpNumber, rRestrictionType);		
	}
}





