// ShiftTanks.h: interface for the CShiftTanks class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHIFTTANKS_H__C558EB1D_A1EC_4FC3_BE4F_9781285178F3__INCLUDED_)
#define AFX_SHIFTTANKS_H__C558EB1D_A1EC_4FC3_BE4F_9781285178F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#pragma pack(1)

#include "RelativeDoubleRecord.h"
/*
typedef struct SHIFT_TANKS_REC_
{
	long m_lTankNumber;
	long m_lProduct;
	long m_lStatusGeneral;
	long m_lStatusSpecial;
	long m_lTankCapacity;
	long m_lFuelLevel;
	long m_lWaterLevel;
	long m_lFuelVolume;
	long m_lWaterVolume;
	long m_lTempertaure;
	long m_lUllage;
	long m_lTCVolume;
	long m_lManifold;

	long m_lFlags;
	long m_lShiftNumber;

	short m_nNACSCode;

	char m_sProductName[20];
	SYSTEMTIME m_sTime;
} SHIFT_TANKS_REC;

typedef struct SHIFT_TANKS_FILE_REC_
{
	SHIFT_TANKS_REC cStartOfShift;
	SHIFT_TANKS_REC cEndOfShift;
} SHIFT_TANKS_FILE_REC;

typedef struct SHIFT_TANKS_FILE_STRUCT_
{
	char sFileValidity[MAX_NAME_LENGTH];
	SHIFT_TANKS_FILE_REC cRec[MAX_TANKS];
} SHIFT_TANKS_FILE_STRUCT;
*/
class CShiftTanks : public CRelativeDoubleRecord  
{
public:
	long Read(long lTankNumber , CTank &cTank , short * pnNACSCode , long lFlags);
	long Write(CTank & cTank , short nNACSCode);
	long Write(TANK_READ_INFO &cTank);
	short GetNACSCode();
	void SetNACSCode(short nCode);
	CShiftTanks();
	virtual ~CShiftTanks();

private:
	long ConvertRecordToTankInfo(CTank & cTank);
	CRITICAL_SECTION m_csShiftTanks;
	SHIFT_TANKS_REC m_cRec;
	void ConvertTankInfoToRecord(TANK_READ_INFO &cTank);
	void ConvertTankInfoToRecord(CTank & cTank);
};

#endif // !defined(AFX_SHIFTTANKS_H__C558EB1D_A1EC_4FC3_BE4F_9781285178F3__INCLUDED_)
