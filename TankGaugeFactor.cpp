// TankGaugeFactor.cpp: implementation of the CTankGaugeFactor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TankGaugeFactor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTankGaugeFactor::CTankGaugeFactor()
{
	m_lVolume = 1;   
	m_lTemperature= 1;	
	m_lFuelLevel= 1; 
}

CTankGaugeFactor::~CTankGaugeFactor()
{

}
/******************************************************************************
 Description:	return the current volume factor
 Returns:      	Volume Factor
 Parameters:   	None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			16/3/2004   11:07		Start
******************************************************************************/
long CTankGaugeFactor::GetVolumeFactor()
{
	return (m_lVolume);
}
/******************************************************************************
 Description:	return the current volume factor
 Returns:      	None
 Parameters:   	lFactor = 	Volume Factor
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			16/3/2004   11:07		Start
******************************************************************************/

void CTankGaugeFactor::SetVolumeFactor(long lFactor)
{
	if (lFactor > 0)
		m_lVolume = lFactor;
	else
		m_lVolume =1;

}
/******************************************************************************
 Description:	return the current volume factor
 Returns:      	Volume Factor
 Parameters:   	None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			16/3/2004   11:07		Start
******************************************************************************/

long CTankGaugeFactor::GetTemperatureFactor()
{
	return(m_lTemperature);
}
/******************************************************************************
 Description:	return the current volume factor
 Returns:      	None
 Parameters:   	lFactor = 	Volume Factor
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			16/3/2004   11:07		Start
******************************************************************************/

void CTankGaugeFactor::SetTemperatureFactor(long lFactor)
{
	if (lFactor > 0)
		m_lTemperature =lFactor;
	else
		m_lTemperature =1;

}
/******************************************************************************
 Description:	return the current fuel level factor
 Returns:      	Volume Factor
 Parameters:   	None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			16/3/2004   11:07		Start
******************************************************************************/
long CTankGaugeFactor::GetFuelLevelFactor()
{
	return (m_lFuelLevel);
}
/******************************************************************************
 Description:	return the current fuel level factor
 Returns:      	None
 Parameters:   	lFactor = 	fuel level
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			16/3/2004   11:07		Start
******************************************************************************/
void CTankGaugeFactor::SetFuelLevelFactor(long lFactor)
{
	if (lFactor > 0)
		m_lFuelLevel = lFactor;
	else
		m_lFuelLevel = 1;

}
