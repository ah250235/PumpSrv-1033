// ShiftTotals.h: interface for the CShiftTotals class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHIFTTOTALS_H__4A1C4CF5_7A3F_4960_BA8E_A3B61DB52BBF__INCLUDED_)
#define AFX_SHIFTTOTALS_H__4A1C4CF5_7A3F_4960_BA8E_A3B61DB52BBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#pragma pack(1)

#include "PumpClass.h"
#include "RelativeDoubleRecord.h"

/*
typedef struct
{
	BYTE byValidRecord;
	
	long m_lIndexNumber;
	long m_lPumpNumber;
	DWORD m_dwShiftNumber;

	short m_nNozzle;
	short m_nGrade;
	short m_nNACSCode;
	short m_nFlags;

	BYTE m_byPumpStatus;
	BYTE m_byNozzleStatus;

	BYTE m_sVolume[MAX_NAME_LENGTH];
	BYTE m_sValueCash[MAX_NAME_LENGTH];
	BYTE m_sValueCredit[MAX_NAME_LENGTH];
	
	char m_sPumpManufacture[MAX_NAME_LENGTH];
	
} SHIFT_TOTALS_REC;  

typedef struct
{
	BYTE byValidRecord;
	SHIFT_TOTALS_REC cStartOfShift;
	SHIFT_TOTALS_REC cEndOfShift;
} SHIFT_TOTALS_FILE_REC;

typedef struct
{
	SHIFT_TOTALS_FILE_REC cRec[MAX_PUMPS][MAX_POSITIONS_PER_PUMP];
} SHIFT_TOTALS_FILE_STRUCT;

typedef struct
{
	BYTE byNozzleStatus[STR_LONG_VAL5];
	BYTE byPumpStatus[STR_LONG_VAL5];
	BYTE byShiftNumber[STR_LONG_VAL10];
	BYTE byIndexNumber[STR_LONG_VAL10];
	BYTE byPumpNumber[STR_LONG_VAL5];
	BYTE byFlags[STR_LONG_VAL5];
	BYTE byGrade[STR_LONG_VAL5];
	BYTE byNACSCode[STR_LONG_VAL5];
	BYTE byNozzle[STR_LONG_VAL5];

	BYTE sVolume[MAX_NAME_LENGTH];
	BYTE sValueCash[MAX_NAME_LENGTH];
	BYTE sValueCredit[MAX_NAME_LENGTH];
	
	char sPumpManufacture[MAX_NAME_LENGTH];

} TAG_SHIFT_TOTALS_REC;

*/

class CShiftTotals : public CRelativeDoubleRecord
{
public:
	long Write(TOTAL_INFO & cRec);
	long Read(long lPump , short nNozzle , long lFlags , TAG_SHIFT_TOTALS_REC & cShiftTotalsRec);

	CShiftTotals();
	virtual ~CShiftTotals();

private:
	long ConvertRecordToPumpTotalsInfo(TAG_SHIFT_TOTALS_REC & cTagRec);
	void ConvertPumpTotalsInfoToRecord(TOTAL_INFO & cPumpTotals);

	SHIFT_TOTALS_REC m_cRec;
	TAG_SHIFT_TOTALS_REC m_cTagRec;
};

#endif // !defined(AFX_SHIFTTOTALS_H__4A1C4CF5_7A3F_4960_BA8E_A3B61DB52BBF__INCLUDED_)
