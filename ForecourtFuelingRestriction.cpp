// ForecourtAuthorizeLimitValidation.cpp: implementation of the ForecourtAuthorizeLimitValidation class.
// Created //4.0.22.501 TD 69203
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ForecourtFuelingRestriction.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CForecourtFuelingRestriction::CForecourtFuelingRestriction()
{
	m_aeSupportedPumpReservation[0] = PAY_AT_PUMP;
	m_aeSupportedPumpReservation[1] = PAY_AT_KIOSK;
	//m_aeSupportedPumpReservation[2] = PRE_PAY_AT_POS;
}

CForecourtFuelingRestriction::~CForecourtFuelingRestriction()
{
	CleanUp();
}

/****************************************************************************
								PUBLIC METHODS
****************************************************************************/


void CForecourtFuelingRestriction::Init(BOOL bCreate)
{
	DWORD dwVal = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "MaxConcurrent", &dwVal, 0, bCreate); //4.0.22.504

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
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 69203
******************************************************************************/

BOOL CForecourtFuelingRestriction::IsApproveAllowed(const LONG lPumpNumber, const  RestrictionType& rRestrictionType)
{
	BOOL bRetCode = FALSE;

	if (ApprovedCondition(lPumpNumber, rRestrictionType) == FRRC_APPROVED)
	{
		bRetCode =  TRUE;
	}
	else	//4.0.23.50 78421
	{
		AddToOnHoldMaxConcurrentList(lPumpNumber);
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPumpOnHoldMaxConcurrent();//4.0.23.50 (switch lines) 78421
	}
	
	return bRetCode;
}


/******************************************************************************
 Description:	add pump to hold max concurrent list 
 Returns:      	void 
 Parameters:   	PumpNumber, RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 69203
******************************************************************************/

void CForecourtFuelingRestriction::SetRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType)
{
	if(IsAllowedSetRestriction(rRestrictionType))	
	{		
		if (GetNumberOfDispensingPumps() >= (long)m_dwMaxConcurrent)
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Max number of dispensing pumps has been reached. Pump %ld will be on hold", lPumpNumber);
				_LOGMSG.LogMsg("CForecourtFuelingRestriction", "SetRestriction", lPumpNumber, LOG_PUMP, str);
			}
			if (!(_Module.m_server.m_cPumpArray[lPumpNumber - 1].IsPumpOnHoldMaxConcurrent()))	
			{
				AddToOnHoldMaxConcurrentList(lPumpNumber);
				_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPumpOnHoldMaxConcurrent(); //4.0.23.50 78421 (switch lines)

			}
			//TBD insert into the queue
		}
	}
}


/******************************************************************************
 Description:	
 Returns:      	void
 Parameters:   	PumpNumber, RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 69203
******************************************************************************/

void CForecourtFuelingRestriction::ClearRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType)
{
	if(IsAllowedClearRestriction(rRestrictionType))
	{
		//4.0.21.500 69203 remove the misc from the current pump if it was on wait and allow next pump authorization
		if (!_Module.m_server.m_cPumpArray[lPumpNumber-1].IsPumpOnHoldMaxConcurrent())
		{
			_Module.m_server.m_cPumpArray[lPumpNumber-1].UnsetPumpOnHoldMaxConcurrent();
			long lNextPumpToAuthorize = RemoveNextPumpFromOnHoldMaxConcurrentList();
			if (lNextPumpToAuthorize > 0)
			{
				_Module.m_server.m_cPumpArray[lNextPumpToAuthorize - 1].UnsetPumpOnHoldMaxConcurrent();
				if(LOG_BASIC_FLOW_CONTROL) 
				{	
					CString str;
					str.Format("Next pump allowed to be authorized is %ld", lNextPumpToAuthorize);
					_LOGMSG.LogMsg("CForecourtFuelingRestriction", "ClearRestriction", lPumpNumber, LOG_PUMP, str);
				}
			}
		}
		else
		{
			_Module.m_server.m_cPumpArray[lPumpNumber-1].UnsetPumpOnHoldMaxConcurrent();
			RemovePumpFromOnHoldMaxConcurrentList(lPumpNumber);

		}
	}
}


/******************************************************************************
 Description:	is allowed set restriction according to value of restriction type?
 Returns:      	Allowed -> TRUE
				Not allowed -> FALSE
 Parameters:   	RestrictionType

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 69203
******************************************************************************/

BOOL CForecourtFuelingRestriction::IsAllowedSetRestriction(const RestrictionType& rRestrictionType)const
{
	BOOL bRetCode = FALSE;

	if(IsSupportedPumpRes(rRestrictionType.eNewPumpRes) && rRestrictionType.eNewPumpStatus == REQUESTING && 
		rRestrictionType.eNewPumpResExt != RES_EXT_AVI_CANCELED && rRestrictionType.eNewPumpResExt != RES_EXT_AUTO_AUTH )
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
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 69203
******************************************************************************/

BOOL CForecourtFuelingRestriction::IsAllowedClearRestriction(const RestrictionType& rRestrictionType)const
{
	BOOL bRetCode = FALSE;

	if(IsSupportedPumpRes(rRestrictionType.ePrevPumpRes) && (rRestrictionType.eNewPumpStatus == READY && rRestrictionType.eNewPumpRes == NULL_RES))
	{		
		bRetCode = TRUE;	
	}

	return bRetCode;
}
void CForecourtFuelingRestriction::CleanUp()
{
	m_cOnHoldMaxConcurrentList.RemoveAll();
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
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 69203
******************************************************************************/

FRRetCode CForecourtFuelingRestriction::ApprovedCondition(const LONG lPumpNumber, const RestrictionType& rRestrictionType)const
{
	FRRetCode eRetCode = FRRC_DISAPPROVED;

	if (!_Module.m_server.m_cPumpArray[lPumpNumber - 1].IsPumpOnHoldMaxConcurrent()) 
	{
		if (GetNumberOfDispensingPumps() >= (long)m_dwMaxConcurrent) //4.0.23.50 78421
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Max number of dispensing pumps has been reached. Pump %ld will be on hold", lPumpNumber);
				_LOGMSG.LogMsg("CForecourtFuelingRestriction", "SetRestriction", lPumpNumber, LOG_PUMP, str);
			}
			//TBD insert into the queue
		}
		else
			eRetCode = FRRC_APPROVED;

	}
		
	return eRetCode;
}




/****************************************************************************
								PRIVATE METHODS
****************************************************************************/

/******************************************************************************
 Description:	Count the pumps in status dispensing or authorized 	
 Returns:      	long
 Parameters:   	None

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			11/11/2008   10:50		Start	//4.0.21.500 69203
 Gena			27/01/2009	 18:16		Move from ServerMain //4.0.22.501 TD 69203
******************************************************************************/
long CForecourtFuelingRestriction::GetNumberOfDispensingPumps() const
{
	long lCounter = 0;
	CPumpStatus cPumpStatus;
	
	for (int i=0; i < _Module.m_server.m_cPumpArray.GetSize(); i++) //4.0.22.503
	{
		cPumpStatus = _Module.m_server.m_cPumpArray[i].GetStat();

		if ((cPumpStatus.GetStatus() == DISPENSING) || (cPumpStatus.GetStatus() == AUTHORIZED))
			lCounter++;		
	}

	return lCounter;
}


void CForecourtFuelingRestriction::AddToOnHoldMaxConcurrentList(long lNewNode)
{
	if (!(_Module.m_server.m_cPumpArray[lNewNode - 1].IsPumpOnHoldMaxConcurrent()))	//4.0.23.50 78421
	{
		m_cOnHoldMaxConcurrentList.AddHead(lNewNode);

		if(LOG_FCC_FLOW_CONTROL)
		{	
			PrintOnHoldMaxConcurrentList();
		}

	}
}


long CForecourtFuelingRestriction::RemoveNextPumpFromOnHoldMaxConcurrentList()
{
	long lPump = 0;

	if (m_cOnHoldMaxConcurrentList.IsEmpty())
		lPump = 0;
	else
	{
		lPump = m_cOnHoldMaxConcurrentList.GetTail();
		m_cOnHoldMaxConcurrentList.RemoveTail();
	}

	return lPump;
}

void CForecourtFuelingRestriction::RemovePumpFromOnHoldMaxConcurrentList(long lPumpNumber) 
{
	POSITION pPos = NULL;

	pPos = m_cOnHoldMaxConcurrentList.Find(lPumpNumber, 0);

	if (pPos)
		m_cOnHoldMaxConcurrentList.RemoveAt(pPos);
}

void CForecourtFuelingRestriction::PrintOnHoldMaxConcurrentList()	
{
	long lLen = m_cOnHoldMaxConcurrentList.GetCount(), lNextNode = 0;
	POSITION pPos;
	CString str, str1;

	if (lLen > 0)
	{
		pPos = m_cOnHoldMaxConcurrentList.GetHeadPosition();
		for (int i = 0; i < lLen; i++)
		{
			lNextNode = m_cOnHoldMaxConcurrentList.GetAt(pPos);
			str1.Format(" %d", lNextNode);
			str += str1;
			lNextNode = m_cOnHoldMaxConcurrentList.GetNext(pPos);
		}
		str1.Format("On hold max concurrent list: ");
		str1 += str;

	}
	else
	{
		str1.Format("On hold max concurrent list is empty");
	}

	_LOGMSG.LogMsg(str1);
}


