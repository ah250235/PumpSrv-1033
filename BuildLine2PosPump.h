// BuildLine2PosPump.h: interface for the CBuildLine2PosPump class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUILDLINE2POSPUMP_H__74EA5830_7DDD_460A_9850_DEC8A1E8DF38__INCLUDED_)
#define AFX_BUILDLINE2POSPUMP_H__74EA5830_7DDD_460A_9850_DEC8A1E8DF38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "StaticString.h"

//#define MAX_TEMPLATE_LINE 256

typedef struct
{
	BYTE	sCardName[8];
	BYTE 	sTruckingCompanyName[40];
	BYTE	sAccountNumber[21];
	BYTE	sFullVehicleId[FULL_VEHICLE_ID];
	BYTE    sAttendantID[STR_LONG_VAL10];
	BYTE    sAttendantName[STR_15];	

}PosPumpCardsInfo;

class CBuildLine2PosPump  
{
public:
	void GetFormattedLine(CPumpTransact &trs, char *sLine, long lLen);
	long m_lTempleteLen;
	void Init(long lLen = 1);
	long GetParamData(long & lPos, long &lSize, LPCSTR p, char *sLine) const;
	long ParseLableString(LPCSTR strFormat, char *strOut) const;
	void Generate(char * sBufferTmpOut, long lLen) const;
	char m_sTemplateLine[MAX_FIELD_VAL];
	CBuildLine2PosPump();
	CBuildLine2PosPump(const PosPumpCardsInfo & CardData);
	void Clear();
	
	virtual ~CBuildLine2PosPump();
private:
	PosPumpCardsInfo m_CardData;

};

#endif // !defined(AFX_BUILDLINE2POSPUMP_H__74EA5830_7DDD_460A_9850_DEC8A1E8DF38__INCLUDED_)
