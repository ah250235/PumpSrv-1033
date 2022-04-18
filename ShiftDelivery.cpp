// ShiftDelivery.cpp: implementation of the CShiftDelivery class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShiftDelivery.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShiftDelivery::CShiftDelivery() : CRelativeDoubleRecord(SHIFT_DELIVERY_FILE)
{

}

CShiftDelivery::~CShiftDelivery()
{

}

long CShiftDelivery::Read(long lTank , long lFlags , TAG_SHIFT_DELIVERY_REC & cDeliveryRec)
{
	long lRtc = 0;
	long lTankNumber = lTank - 1;

	WORD iBytesToRead = sizeof(SHIFT_DELIVERY_REC);
	DWORD dwOffset = offsetof(SHIFT_DELIVERY_FILE_STRUCT , cRec[lTankNumber]);

	//Each record is composed of two identical halves.  The current shift is stored
	//in the second half.
	if(lFlags & GET_CURRENT_SHIFT)
		dwOffset += iBytesToRead;

/*	  //4.0.1.38
	long lHydFlags = HOP_DIRECT_LOCAL;

	if(_HOST.m_lHydraActive)
		lHydFlags = HOP_DEFAULT_CONFIG;
*/
	
	CRelativeDoubleRecord::Read(dwOffset,
								iBytesToRead,
								&m_cRec,
								HOP_DIRECT_LOCAL, //4.0.1.38
								0,
								0);

	lRtc = ConvertRecordToDeliveryInfo(cDeliveryRec);

	return lRtc;
}

long CShiftDelivery::Write(DELIVERY_INFO & cDelivery)
{
	long lRtc = 0;

	long lTankNumber =  cDelivery.lTank - 1;

	DWORD dwOffset = offsetof(SHIFT_DELIVERY_FILE_STRUCT , cRec[lTankNumber]);
	WORD iBytesToWrite = sizeof(SHIFT_DELIVERY_REC);

/*	4.0.1.38
	long lFlags = HOP_DIRECT_LOCAL;

	if(_HOST.m_lHydraActive)
		lFlags = HOP_DEFAULT_CONFIG;
*/

	ConvertDeliveryInfoToRecord(cDelivery);
	
	lRtc = CRelativeDoubleRecord::Write(dwOffset,
		                                iBytesToWrite,
										&m_cRec,
										HOP_DEFAULT_CONFIG, //4.0.1.38
										0,
										0);

	return lRtc;
}


long CShiftDelivery::ConvertRecordToDeliveryInfo(TAG_SHIFT_DELIVERY_REC & cDeliveryRec)
{
	long lRtc = OK;

	memset(&cDeliveryRec , ' ' , sizeof(cDeliveryRec));

	if(m_cRec.byValidRecord != 1)
		lRtc = ERR_NOT_FOUND;

	if(lRtc == OK)
	{
		l2a(m_cRec.cDeliveryData.dwShiftNumber , (BYTE*)cDeliveryRec.byShiftNumber , sizeof(cDeliveryRec.byShiftNumber));
		l2a(m_cRec.cDeliveryData.lDeliveredVolume , (BYTE*)cDeliveryRec.byDeliveredVolume , sizeof(cDeliveryRec.byDeliveredVolume));
		l2a(m_cRec.cDeliveryData.lEndVolume , (BYTE*)cDeliveryRec.byEndVolume , sizeof(cDeliveryRec.byEndVolume));
		l2a(m_cRec.cDeliveryData.lIndexNumber , (BYTE*)cDeliveryRec.byIndexNumber , sizeof(cDeliveryRec.byIndexNumber));
		l2a(m_cRec.cDeliveryData.lProduct , (BYTE*)cDeliveryRec.byProduct , sizeof(cDeliveryRec.byProduct));
		l2a(m_cRec.cDeliveryData.lStartVolume , (BYTE*)cDeliveryRec.byStartVolume , sizeof(cDeliveryRec.byStartVolume));
		l2a(m_cRec.cDeliveryData.lStatus , (BYTE*)cDeliveryRec.byStatus , sizeof(cDeliveryRec.byStatus));
		l2a(m_cRec.cDeliveryData.lTank , (BYTE*)cDeliveryRec.byTank , sizeof(cDeliveryRec.byTank));
		l2a(m_cRec.cDeliveryData.lTag , (BYTE*)cDeliveryRec.byTag , sizeof(cDeliveryRec.byTag));
		l2a(m_cRec.cDeliveryData.lTemperature , (BYTE*)cDeliveryRec.byTemperature , sizeof(cDeliveryRec.byTemperature));
		l2a(m_cRec.cDeliveryData.nFlags , (BYTE*)cDeliveryRec.byFlags , sizeof(cDeliveryRec.byFlags));
	}

	return lRtc;
}

void CShiftDelivery::ConvertDeliveryInfoToRecord(DELIVERY_INFO & cDelivery)
{
	m_cRec.byValidRecord = (BYTE)1;
	
	memcpy(&m_cRec.cDeliveryData , &cDelivery , sizeof(cDelivery));

	return;
}