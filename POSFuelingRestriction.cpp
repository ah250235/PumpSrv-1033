// POSFuelingRestriction.cpp: implementation of the CPOSFuelingRestriction class.
// Created //4.0.22.501 TD 70956
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "POSFuelingRestriction.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPOSFuelingRestriction::CPOSFuelingRestriction()
{
	m_aeSupportedPumpReservation[0] = PAY_AT_PUMP;
	m_aeSupportedPumpReservation[1] = PAY_AT_KIOSK;
	//m_aeSupportedPumpReservation[2] = PRE_PAY_AT_POS;
}

CPOSFuelingRestriction::~CPOSFuelingRestriction()
{
	CleanUp();
}


/****************************************************************************
								PUBLIC METHODS
****************************************************************************/


void CPOSFuelingRestriction::Init(BOOL bCreate)
{
	DWORD dwVal = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "MaxPOSConcurrent", &dwVal, 0, bCreate); //4.0.22.506

	SetMaxConcurrent(dwVal);
}

/******************************************************************************
 Description:	//FROM GPI.cpp AuthorizePump__()
				if approve is allowed -> Add Pump To Restricted List and Add POS To Approved List
 Returns:      	if approve is allowed -> TRUE
				else ------------------> FALSE
 Parameters:   	PumpNumber, RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

BOOL CPOSFuelingRestriction::IsApproveAllowed(const LONG lPumpNumber, const  RestrictionType& rRestrictionType)	
{
	BOOL			bRetCode = FALSE;

	if (ApprovedCondition(lPumpNumber, rRestrictionType) == FRRC_APPROVED)
	{
		HandleApproveAllowed(lPumpNumber, rRestrictionType);

		bRetCode =  TRUE;
	}
	
	return bRetCode;
}


/******************************************************************************
 Description:	Add Pump To Restricted List 
 Returns:      	void 
 Parameters:   	PumpNumber, RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CPOSFuelingRestriction::SetRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType)
{
 	if(IsAllowedSetRestriction(rRestrictionType))
		AddPumpToRestrictedList(lPumpNumber, rRestrictionType);

	if(IsAllowedSetApprove(rRestrictionType))
		SetApprove(lPumpNumber, rRestrictionType);
}


/******************************************************************************
 Description:	clear approve and remove pump from restricted list
 Returns:      	void
 Parameters:   	PumpReservation, RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CPOSFuelingRestriction::ClearRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType)
{
	if(IsAllowedClearRestriction(rRestrictionType))
	{
		if(IsAllowedClearApprove(lPumpNumber))
			ClearApprove(lPumpNumber);

		RemovePumpFromRestrictedList(lPumpNumber);				
	}
}


/******************************************************************************
 Description:	Clean all maps
 Returns:      	void
 Parameters:   	NONE

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CPOSFuelingRestriction::CleanUp()
{
	if(!m_cApprovedPOSCountsList.IsEmpty())
		m_cApprovedPOSCountsList.RemoveAll();

	if(!m_cRestrictedPumpsList.IsEmpty())
		m_cRestrictedPumpsList.RemoveAll();
}


/****************************************************************************
								PROTECTED METHODS
****************************************************************************/

/******************************************************************************
 Description:	Approved Condition
 Returns:      	if allowed approve -> FRRC_APPROVED
				else -> FRRC_DISAPPROVED
 Parameters:   	PumpNumber, RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

FRRetCode CPOSFuelingRestriction::ApprovedCondition(const LONG lPumpNumber, const RestrictionType& rRestrictionType)const
{
	FRRetCode	eRetCode = FRRC_APPROVED;

	if (rRestrictionType.lPOSNumber != NO_POS_NUMBER)
	{
		BYTE	byConcurrentCount = 0;

		if(m_cApprovedPOSCountsList.Lookup(rRestrictionType.lPOSNumber, byConcurrentCount))
		{
			if(byConcurrentCount >= m_dwMaxConcurrent)
			{
				eRetCode = FRRC_DISAPPROVED;				
			}
		}
	}
				
	CString sDebugMsg;

	if(eRetCode == FRRC_APPROVED)
	{		
		if(LOG_FCC_FLOW_CONTROL)
		{			
			sDebugMsg.Format("Pump allowed to be authorized. Max number of dispensing pumps = %ld per POS.", m_dwMaxConcurrent);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sDebugMsg);
		}
	}
	else
	{				
		if(LOG_BASIC_FLOW_CONTROL) 
		{
			sDebugMsg.Format("Max number of dispensing pumps = %ld per POS has been reached. Pump will be not authorized", m_dwMaxConcurrent);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sDebugMsg, LOG_LEVEL_1);
		}
	}
	
	return eRetCode;
}


void CPOSFuelingRestriction::HandleApproveAllowed(const LONG lPumpNumber, const  RestrictionType& rRestrictionType)
{
	AddPumpToRestrictedList(lPumpNumber, rRestrictionType);
	AddPOSToApprovedList(rRestrictionType.lPOSNumber);
}


/******************************************************************************
 Description:	is allowed set restriction according to value of restriction type?
 Returns:      	Allowed -> TRUE
				Not allowed -> FALSE
 Parameters:   	RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

BOOL CPOSFuelingRestriction::IsAllowedSetRestriction(const RestrictionType& rRestrictionType)const
{
	BOOL bRetCode = FALSE;

	if( IsSupportedPumpRes(rRestrictionType.eNewPumpRes) && rRestrictionType.ePrevPumpStatus == REQUESTING && 
		rRestrictionType.eNewPumpResExt != RES_EXT_AVI_CANCELED && rRestrictionType.eNewPumpResExt != RES_EXT_AUTO_AUTH)
	{		
		bRetCode = TRUE;	
	}

	return bRetCode;
}


/******************************************************************************
 Description:	Is allowed clear restriction according to value of restriction type?
 Returns:      	Allowed -> TRUE
				Not allowed -> FALSE
 Parameters:   	RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

BOOL CPOSFuelingRestriction::IsAllowedClearRestriction(const RestrictionType& rRestrictionType)const
{
	BOOL bRetCode = FALSE;

	if(IsSupportedPumpRes(rRestrictionType.ePrevPumpRes) && (rRestrictionType.eNewPumpStatus == READY && rRestrictionType.eNewPumpRes == NULL_RES))
	{		
		bRetCode = TRUE;	
	}

	return bRetCode;
}


BOOL CPOSFuelingRestriction::IsAllowedClearApprove(const LONG lPumpNumber)const
{
	BOOL			bRetCode = FALSE;
	RestrictionType sLocalRestrictionType;

	if(m_cRestrictedPumpsList.Lookup(lPumpNumber, sLocalRestrictionType))
	{
		bRetCode = sLocalRestrictionType.bDispensed;								//4.0.23.20 TD 76619
	}

	return bRetCode;	
}


/******************************************************************************
 Description:	is allowed set approve according to value of restriction type?
 
   Returns:      	Allowed -> TRUE
				Not allowed -> FALSE	
 
 Parameters:   	RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/
BOOL CPOSFuelingRestriction::IsAllowedSetApprove(const RestrictionType& rRestrictionType)const
{
	BOOL bRetCode = FALSE;

	if(IsSupportedPumpRes(rRestrictionType.ePrevPumpRes) && rRestrictionType.ePrevPumpStatus == REQUESTING && 
							(rRestrictionType.eNewPumpStatus == DISPENSING || rRestrictionType.eNewPumpStatus == AUTHORIZED) )
	{		
		bRetCode = TRUE;	
	}

	return bRetCode;
}


/******************************************************************************
 Description:	add pos to approved list and if pump exist in restricted list -> increment pos count
 Returns:      	void
 Parameters:   	PumpNumber, RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CPOSFuelingRestriction::SetApprove(const LONG lPumpNumber, const RestrictionType& rRestrictionType)
{
	AddPOSToApprovedList(rRestrictionType.lPOSNumber);

	RestrictionType sLocalRestrictionType;

	if(m_cRestrictedPumpsList.Lookup(lPumpNumber, sLocalRestrictionType))		//Get posNumber from RestrictedPumpsList
	{
		sLocalRestrictionType.bDispensed = TRUE;								//4.0.23.20 TD 76619
		m_cRestrictedPumpsList.SetAt(lPumpNumber, sLocalRestrictionType);

		IncApprovedCount(sLocalRestrictionType.lPOSNumber);
	}
}


/******************************************************************************
 Description:	decrement pos count.
 Returns:      	void
 Parameters:   	PumpNumber

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CPOSFuelingRestriction::ClearApprove(const LONG lPumpNumber)
{	
	RestrictionType sLocalRestrictionType;

	if(m_cRestrictedPumpsList.Lookup(lPumpNumber, sLocalRestrictionType))	//Get posNumber from RestrictedPumpsList
	{	
		DecApprovedCount(sLocalRestrictionType.lPOSNumber);	
	}
}


/****************************************************************************
								PRIVATE METHODS
****************************************************************************/

//**********************
//RestrictedList methods
//**********************

/******************************************************************************
 Description:	add pump to restricted list.
 Returns:      	void
 Parameters:   	PumpNumber, RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/
void CPOSFuelingRestriction::AddPumpToRestrictedList(const LONG lPumpNumber, const RestrictionType& rRestrictionType)
{
	RestrictionType sLocalRestrictionType;
	
	if (!m_cRestrictedPumpsList.Lookup(lPumpNumber, sLocalRestrictionType))
	{
		memcpy(&sLocalRestrictionType, &rRestrictionType, sizeof(RestrictionType));
		m_cRestrictedPumpsList.SetAt(lPumpNumber, sLocalRestrictionType);
	}
	else
	{
		if (sLocalRestrictionType.lPOSNumber == NO_POS_NUMBER && rRestrictionType.lPOSNumber != NO_POS_NUMBER)		
		{
			sLocalRestrictionType.lPOSNumber = rRestrictionType.lPOSNumber;
			m_cRestrictedPumpsList.SetAt(lPumpNumber, sLocalRestrictionType);				//update pos number from NO_POS_NUMBER to legal number
		}
	}
}


/******************************************************************************
 Description:	remove pump from restricted list.
 Returns:      	void
 Parameters:   	PumpNumber

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CPOSFuelingRestriction::RemovePumpFromRestrictedList(const LONG lPumpNumber)
{
	RestrictionType sLocalRestrictionType;

	if(m_cRestrictedPumpsList.GetCount() > 0 && m_cRestrictedPumpsList.Lookup(lPumpNumber, sLocalRestrictionType))
	{
		m_cRestrictedPumpsList.RemoveKey(lPumpNumber);
	}
}

//********************
//ApprovedList methods
//********************

/******************************************************************************
 Description:	add pos to approved list.
 Returns:      	void
 Parameters:   	POSNumber

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CPOSFuelingRestriction::AddPOSToApprovedList(const LONG lPOSNumber)	
{
	if (lPOSNumber != NO_POS_NUMBER)
	{	
		LONG	lLocalPOSNumber = lPOSNumber;
		BYTE	byConcurrentCount = 0;

		if(!m_cApprovedPOSCountsList.Lookup(lLocalPOSNumber, byConcurrentCount))
		{
			m_cApprovedPOSCountsList.SetAt(lLocalPOSNumber, byConcurrentCount);
		}
	}
}


/******************************************************************************
 Description:	increment approved pos count.
 Returns:      	void
 Parameters:   	POSNumber

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CPOSFuelingRestriction::IncApprovedCount(const LONG lPOSNumber)
{
	if (lPOSNumber != NO_POS_NUMBER)
	{
		LONG	lLocalPOSNumber = lPOSNumber;
		BYTE	byConcurrentCount;

		if(m_cApprovedPOSCountsList.Lookup(lLocalPOSNumber, byConcurrentCount))
		{
			m_cApprovedPOSCountsList.SetAt(lLocalPOSNumber, ++byConcurrentCount);
		}
	}
}


/******************************************************************************
 Description:	decrement approved pos count.
 Returns:      	void
 Parameters:   	POSNumber

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

void CPOSFuelingRestriction::DecApprovedCount(const LONG lPOSNumber)
{
	if (lPOSNumber != NO_POS_NUMBER)
	{
		LONG	lLocalPOSNumber= lPOSNumber;
		BYTE	byConcurrentCount;

		if(m_cApprovedPOSCountsList.Lookup(lLocalPOSNumber, byConcurrentCount))
		{
			if(byConcurrentCount > 0)
				m_cApprovedPOSCountsList.SetAt(lLocalPOSNumber, --byConcurrentCount);
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////////
//				CCashierFuelingRestriction class implementation		4.0.22.504
/////////////////////////////////////////////////////////////////////////////////////


CCashierFuelingRestriction::CCashierFuelingRestriction()
{	
	m_lSignInCashiersCount = 0;
	m_lMaxApprovedPumps = 0;
	m_lApprovedPumpsCount = 0;
}


void CCashierFuelingRestriction::Init(BOOL bCreate)
{
	DWORD dwVal = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "MaxCashierConcurrent", &dwVal, 0, bCreate); //4.0.22.506
	SetMaxConcurrent(dwVal);

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "SignInCashiersCounter", (DWORD*)&m_lSignInCashiersCount, 0, TRUE, bCreate, 0, HOP_DIRECT_LOCAL, TRUE);	//4.0.22.506
	SetMaxApprovedPumps(m_lSignInCashiersCount * m_dwMaxConcurrent);
}


inline void CCashierFuelingRestriction::CashierSignIn()
{
	SetMaxApprovedPumps(++m_lSignInCashiersCount * m_dwMaxConcurrent);
	SaveCashiersCounter();	//4.0.22.506 TD 70956
}	//4.0.22.504
	

inline void	CCashierFuelingRestriction::CashierSignOut()
{
	if(m_lSignInCashiersCount > 0)
	{
		SetMaxApprovedPumps(--m_lSignInCashiersCount * m_dwMaxConcurrent);
		SaveCashiersCounter();	//4.0.22.506 TD 70956
	}
}	//4.0.22.504


//4.0.22.506 TD 70956
inline void	CCashierFuelingRestriction::ResetCashiersCounter()
{	
	m_lMaxApprovedPumps = m_lSignInCashiersCount = 0;
	SaveCashiersCounter();	
}	


/****************************************************************************
								PROTECTED METHODS
****************************************************************************/

/******************************************************************************
 Description:	Approved Condition
 Returns:      	if allowed approve -> FRRC_APPROVED
				else -> FRRC_DISAPPROVED
 Parameters:   	PumpNumber, RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			24/03/2009	 10:16		//4.0.22.501 TD 70956 4.0.22.504
******************************************************************************/

FRRetCode CCashierFuelingRestriction::ApprovedCondition(const LONG lPumpNumber, const RestrictionType& rRestrictionType)const
{
	FRRetCode	eRetCode = FRRC_APPROVED;

	if (rRestrictionType.lPOSNumber != NO_POS_NUMBER)
	{		
		if(m_lApprovedPumpsCount >= m_lMaxApprovedPumps)
		{
			eRetCode = FRRC_DISAPPROVED;				
		}
	}
				
	CString sDebugMsg;

	if(eRetCode == FRRC_APPROVED)
	{		
		if(LOG_BASIC_FLOW_CONTROL/*LOG_FCC_FLOW_CONTROL*/)
		{			
			sDebugMsg.Format("Pump allowed to be authorized. Number of dispensing pumps = %ld.", m_lApprovedPumpsCount);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sDebugMsg);
		}
	}
	else
	{				
		if(LOG_BASIC_FLOW_CONTROL) 
		{
			sDebugMsg.Format("Max number of dispensing pumps = %ld has been reached with %ld dispensing pumps. Pump will be not authorized", m_lMaxApprovedPumps, m_lApprovedPumpsCount);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sDebugMsg, LOG_LEVEL_1);
		}
	}
	
	return eRetCode;
}

//4.0.23.507 TD 89166
void CCashierFuelingRestriction::SetMaxApprovedPumps(long lMaxApprovedPumps)
{
	m_lMaxApprovedPumps = lMaxApprovedPumps;
}


//4.0.23.500 TD 78089
void CCashierFuelingRestriction::HandleApproveAllowed(const LONG lPumpNumber, const  RestrictionType& rRestrictionType)
{
	AddPumpToRestrictedList(lPumpNumber, rRestrictionType);
	m_lApprovedPumpsCount = GetRestrictedListSize();
}


//4.0.23.500 TD 78089
void CCashierFuelingRestriction::RemovePumpFromRestrictedList(const LONG lPumpNumber)
{
	CPOSFuelingRestriction::RemovePumpFromRestrictedList(lPumpNumber);
	m_lApprovedPumpsCount = GetRestrictedListSize();	
}


void CCashierFuelingRestriction::SaveCashiersCounter()const
{
	_Module.m_server.m_cParam.SaveParam("GeneralParam", "SignInCashiersCounter", (DWORD)m_lSignInCashiersCount, TRUE, _HOST.m_lGroupNum, 0, 0, TRUE); //4.0.22.506 TD 70956 //4.0.26.503 TD 158047
}

