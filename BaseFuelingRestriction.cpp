// BaseFuelingResriction.cpp: implementation of the CBaseFuelingResriction class.
//	Created //4.0.22.501 TD 70956
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseFuelingRestriction.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseFuelingRestriction::CBaseFuelingRestriction()
{
	m_dwMaxConcurrent = 0;
	memset(m_aeSupportedPumpReservation, (int)UNKNOWN_RES, sizeof(m_aeSupportedPumpReservation));
}


CBaseFuelingRestriction::~CBaseFuelingRestriction()
{

}


inline BOOL CBaseFuelingRestriction::IsActive()const
{	
	return (this->m_dwMaxConcurrent > 0);
}


inline void CBaseFuelingRestriction::SetMaxConcurrent(const DWORD dwMaxConcurrent) 
{ 
	m_dwMaxConcurrent = dwMaxConcurrent; 
}


/******************************************************************************
 Description:	Check if pump reservation type supported
 Returns:      	BOOL
 Parameters:   	PumpReservation

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/01/2009	 18:16		//4.0.22.501 TD 70956
******************************************************************************/

BOOL CBaseFuelingRestriction::IsSupportedPumpRes(const PumpReservation ePumpRes) const 
{
	BOOL bRetCode = FALSE;

	for (int i = 0; i < UNKNOWN_RES && m_aeSupportedPumpReservation[i] != UNKNOWN_RES && bRetCode == FALSE; i++)
	{
		if (m_aeSupportedPumpReservation[i] == ePumpRes)
		{
			bRetCode = TRUE;
		}
	}
		
	return bRetCode;
}