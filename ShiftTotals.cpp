// ShiftTotals.cpp: implementation of the CShiftTotals class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShiftTotals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShiftTotals::CShiftTotals() : CRelativeDoubleRecord(SHIFT_PUMP_FILE)
{
	memset(&m_cRec , 0 , sizeof(m_cRec));
	memset(&m_cTagRec , ' ' , sizeof(m_cTagRec));
}

CShiftTotals::~CShiftTotals()
{

}

long CShiftTotals::Write(TOTAL_INFO & cRec)
{
	long lRtc = 0;
	long lPumpNumber = cRec.lPump - 1;
	short nNozzle = cRec.nNozzle - 1;

	DWORD dwOffset = offsetof(SHIFT_TOTALS_FILE_STRUCT , cRec[lPumpNumber][nNozzle]);
	WORD iBytesToWrite = sizeof(SHIFT_TOTALS_REC);

/* 4.0.1.38
	long lFlags = HOP_DIRECT_LOCAL;

	if(_HOST.m_lHydraActive)
		lFlags = HOP_DEFAULT_CONFIG;
*/

	ConvertPumpTotalsInfoToRecord(cRec);

	lRtc = CRelativeDoubleRecord::Write(dwOffset,
		                                iBytesToWrite,
										&m_cRec,
										HOP_DEFAULT_CONFIG, //4.0.1.38
										0,
										0);

	return lRtc;
}

long CShiftTotals::Read(long lPump , short nNozzle ,long lFlags , TAG_SHIFT_TOTALS_REC & cShiftTotalsRec)
{
	long lRtc = 0;
	long lPumpNumber = lPump - 1;
	short nNozzleToCheck = nNozzle - 1;

	WORD iBytesToRead = sizeof(SHIFT_TOTALS_REC);
	DWORD dwOffset = offsetof(SHIFT_TOTALS_FILE_STRUCT , cRec[lPumpNumber][nNozzleToCheck]);

	//Each record is composed of two identical halves.  The current shift is stored
	//in the second half.
	if(lFlags & GET_CURRENT_SHIFT)
		dwOffset += iBytesToRead;

/* //4.0.1.38
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

	lRtc = ConvertRecordToPumpTotalsInfo(cShiftTotalsRec);

	return lRtc;
}


void CShiftTotals::ConvertPumpTotalsInfoToRecord(TOTAL_INFO & cPumpTotals)
{
	m_cRec.byValidRecord = (BYTE)1;

	m_cRec.m_byNozzleStatus = cPumpTotals.byNozzleStatus;
	m_cRec.m_byPumpStatus = cPumpTotals.byPumpStatus;
	m_cRec.m_dwShiftNumber = cPumpTotals.dwShiftNumber;
	m_cRec.m_lIndexNumber = cPumpTotals.lIndexNumber;
	m_cRec.m_lPumpNumber = cPumpTotals.lPump;
	m_cRec.m_nFlags = cPumpTotals.nFlags;
	m_cRec.m_nGrade = cPumpTotals.nGrade;
	m_cRec.m_nNozzle = cPumpTotals.nNozzle;

	memcpy((BYTE*)m_cRec.m_sVolume , (BYTE*)cPumpTotals.sVolume , MAX_NAME_LENGTH);
	memcpy((BYTE*)m_cRec.m_sValueCash , (BYTE*)cPumpTotals.sValueA , MAX_NAME_LENGTH);
	memcpy((BYTE*)m_cRec.m_sValueCredit , (BYTE*)cPumpTotals.sValueB , MAX_NAME_LENGTH);

	char sSection[32];
	memset((char*)sSection , ' ' , sizeof(sSection));
	CString sTmpSection , sNumber;
	sTmpSection.Format("Pumps\\Pump");
	sNumber.Format("%02d" , m_cRec.m_lPumpNumber);

	sTmpSection += sNumber;
	strcpy(sSection , (LPCSTR)sTmpSection);

	_Module.m_server.m_cParam.LoadParam((char*)sSection , "ManufacturerName" , (char*)m_cRec.m_sPumpManufacture , sizeof(m_cRec.m_sPumpManufacture) , "" , FALSE , FALSE);
}

long CShiftTotals::ConvertRecordToPumpTotalsInfo(TAG_SHIFT_TOTALS_REC & cTagRec)
{
	long lRtc = OK;

	memset(&cTagRec , ' '  , sizeof(cTagRec));

	if(m_cRec.byValidRecord != 1)
		lRtc = ERR_NOT_FOUND;

	if(lRtc == OK)
	{
		l2a(m_cRec.m_byNozzleStatus , (BYTE*)cTagRec.byNozzleStatus , sizeof(cTagRec.byNozzleStatus));
		l2a(m_cRec.m_byPumpStatus , (BYTE*)cTagRec.byPumpStatus , sizeof(cTagRec.byPumpStatus));
		l2a(m_cRec.m_dwShiftNumber , (BYTE*)cTagRec.byShiftNumber , sizeof(cTagRec.byShiftNumber));
		l2a(m_cRec.m_lIndexNumber , (BYTE*)cTagRec.byIndexNumber , sizeof(cTagRec.byIndexNumber));
		l2a(m_cRec.m_lPumpNumber ,  (BYTE*)cTagRec.byPumpNumber , sizeof(cTagRec.byPumpNumber));
		l2a(m_cRec.m_nFlags ,  (BYTE*)cTagRec.byFlags , sizeof(cTagRec.byFlags));
		l2a(m_cRec.m_nGrade ,  (BYTE*)cTagRec.byGrade , sizeof(cTagRec.byGrade));
		l2a(m_cRec.m_nNACSCode ,  (BYTE*)cTagRec.byNACSCode , sizeof(cTagRec.byNACSCode));
		l2a(m_cRec.m_nNozzle ,  (BYTE*)cTagRec.byNozzle , sizeof(cTagRec.byNozzle));
		
		memcpy((char*)cTagRec.sPumpManufacture , (char*)m_cRec.m_sPumpManufacture , min(sizeof(cTagRec.sPumpManufacture) , strlen((char*)m_cRec.m_sPumpManufacture)));
		memcpy((BYTE*)cTagRec.sValueCash , (BYTE*)m_cRec.m_sValueCash , sizeof(cTagRec.sValueCash));
		memcpy((BYTE*)cTagRec.sValueCredit , (BYTE*)m_cRec.m_sValueCredit , sizeof(cTagRec.sValueCredit));
		//4.0.5.0
		memcpy((BYTE*)cTagRec.sVolume , (BYTE*)m_cRec.m_sVolume , sizeof(cTagRec.sVolume));
		
	}

	return lRtc;
}
