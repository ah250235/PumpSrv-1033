// WetStockDetails.cpp: implementation of the WetStockDetails class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WetStockDetails.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWetStockDetails::CWetStockDetails()
{
	m_lDeliveryStatus = 0;
}

CWetStockDetails::~CWetStockDetails()
{
	;
}
/******************************************************************************
 Description:	Get delivery progress status 	
 Returns:     	0 - done  1- in progress
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			31/5/2005   8:48		Start
******************************************************************************/

long CWetStockDetails::GetDeliveryStatus()
{
	return m_lDeliveryStatus;
}
/******************************************************************************
 Description:	Set delivery progress status 	
 Returns:     	
 Parameters:   	lVal:= 0 - done  1- in progress
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			31/5/2005   8:48		Start
******************************************************************************/

void CWetStockDetails::SetDeliveryStatus(long lVal)
{
	m_lDeliveryStatus =  lVal;
}
