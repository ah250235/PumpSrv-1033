// DeviceRecord.cpp: implementation of the CDeviceRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeviceRecord.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeviceRecord::CDeviceRecord() : m_dwUpdateMap1(0) , m_dwUpdateMap2(0) , m_dwNACSCode(0)
{
	//Initialize array to zero
	memset((BYTE*)m_byDeviceQuantity , 0 , sizeof(m_byDeviceQuantity));
}

CDeviceRecord::~CDeviceRecord()
{

}

//lPumpNumber is 1 - 64
void CDeviceRecord::UpdateMap(unsigned long lPumpNumber)
{
	unsigned long lMask = 1;
	long lTmpMask = 0;

	lTmpMask = lMask << ((lPumpNumber - 1) % MAX_PUMPS);
		
	if(lPumpNumber <= MAX_PUMPS)
		m_dwUpdateMap1 |= lTmpMask;

	else if(lPumpNumber <= MAX_PUMPS_64)
		m_dwUpdateMap2 |= lTmpMask;

	return;
}

/******************************************************************************
 Description:	Add a blender for each pump that contains this grade 	
 Returns:      	void
 Parameters:   	none
 Assumptions:   This method must be called from the CServerMain::m_cDevicesPerGrade
                array for each grade that is a blend
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			14/10/2002   11:13		Start
******************************************************************************/
void CDeviceRecord::SetNumberOfBlenders()
{
	long lNumBlenders = 0;
	long lMask = 1;

	for(int i = 0;i < 32;i++)
	{
		lNumBlenders += (m_dwUpdateMap1 & lMask) ? 1 : 0;
		lNumBlenders += (m_dwUpdateMap2 & lMask) ? 1 : 0;

		lMask <<= 1;
	}

	m_byDeviceQuantity[BLENDERS] = (BYTE)lNumBlenders;
}

void CDeviceRecord::SetNumberOfBlenders(BYTE byNum)
{ 
	m_byDeviceQuantity[BLENDERS] = byNum;
}

void CDeviceRecord::SetNumberOfCardReaders(BYTE byNum) 
{ 
	m_byDeviceQuantity[CARD_READERS] = byNum;
}

void CDeviceRecord::SetNumberOfRFID(BYTE byNum) 
{ 
	m_byDeviceQuantity[RFIDS]= byNum; 
}

void CDeviceRecord::SetNumberOfScanners(BYTE byNum) 
{ 
	m_byDeviceQuantity[SCANNERS] = byNum; 
}

void CDeviceRecord::SetNumberOfCashAcceptors(BYTE byNum) 
{ 
	m_byDeviceQuantity[CASH_ACCEPTORS] = byNum; 
}

void CDeviceRecord::GetUpdateMaps(unsigned long * plUpdateMap1 , unsigned long * plUpdateMap2)
{
	*plUpdateMap1 = m_dwUpdateMap1;
	*plUpdateMap2 = m_dwUpdateMap2;

	return;
}

CDeviceRecord & CDeviceRecord::operator +=(CDeviceRecord &cRec)
{
	m_byDeviceQuantity[BLENDERS] += cRec.GetNumberOfBlenders();
	m_byDeviceQuantity[CARD_READERS] += cRec.GetNumberOfCardReaders();
	m_byDeviceQuantity[RFIDS] += cRec.GetNumberOfRFID();
	m_byDeviceQuantity[SCANNERS] += cRec.GetNumberOfScanners();
	m_byDeviceQuantity[CASH_ACCEPTORS] += cRec.GetNumberOfCashAcceptors();

	unsigned long lTmpMap1  = 0 , lTmpMap2 = 0;
	cRec.GetUpdateMaps(&lTmpMap1 , &lTmpMap2);

	m_dwUpdateMap1 |= lTmpMap1;
	m_dwUpdateMap2 |= lTmpMap2;

	return *this;
}

CDeviceRecord & CDeviceRecord::operator =(CDeviceRecord & cRec)
{
	m_byDeviceQuantity[BLENDERS] = cRec.GetNumberOfBlenders();
	m_byDeviceQuantity[CARD_READERS] = cRec.GetNumberOfCardReaders();
	m_byDeviceQuantity[RFIDS] = cRec.GetNumberOfRFID();
	m_byDeviceQuantity[SCANNERS] = cRec.GetNumberOfScanners();
	m_byDeviceQuantity[CASH_ACCEPTORS] = cRec.GetNumberOfCashAcceptors();

	//There is no need to copy the update maps, because the source
	//object may not have initialized them, meaning the target object
	//will lose the history of updates already made.  Another field not
	//copied is the NACSCode, again, because this field is not always initialized.
	
	return *this;
}

//Zero stands for a pump that can still update the device map.
BOOL CDeviceRecord::CanUpdate(long lSource)
{
	BOOL bRetVal = FALSE;
	long lMask = 1 << ((lSource % 32) - 1);

	if(lSource <= MAX_PUMPS)
		bRetVal = !(lMask & m_dwUpdateMap1);

	else
		bRetVal = !(lMask & m_dwUpdateMap2);

	return bRetVal;
}

BYTE CDeviceRecord::GetNumberOfDevices(CDeviceRecordIndex enumIndex)
{
	return m_byDeviceQuantity[enumIndex];
}