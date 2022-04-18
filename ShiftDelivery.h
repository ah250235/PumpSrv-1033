// ShiftDelivery.h: interface for the CShiftDelivery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHIFTDELIVERY_H__04913DBD_89C5_4792_B787_564038B094F6__INCLUDED_)
#define AFX_SHIFTDELIVERY_H__04913DBD_89C5_4792_B787_564038B094F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PumpClass.h"
#include "RelativeDoubleRecord.h"

/*
typedef struct
{
	BYTE byValidRecord;
	DELIVERY_INFO cDeliveryData;
} SHIFT_DELIVERY_REC;

typedef struct
{
	BYTE byValidRecord;
	SHIFT_DELIVERY_REC cStartOfShift;
	SHIFT_DELIVERY_REC cEndOfShift;
} SHIFT_DELIVERY_FILE_REC;

typedef struct
{
	SHIFT_DELIVERY_FILE_REC cRec[MAX_TANKS];
} SHIFT_DELIVERY_FILE_STRUCT;

typedef struct
{
	BYTE			    byIndexNumber[STR_LONG_VAL10];		 
	BYTE				byTank[STR_LONG_VAL10];				 
	BYTE				byProduct[STR_LONG_VAL10];			 
	BYTE				byStatus[STR_LONG_VAL10];			 
	BYTE				byFlags[STR_LONG_VAL5];				 
	BYTE				byTag[STR_LONG_VAL10];				 
	BYTE				byDeliveredVolume[STR_LONG_VAL10];	 
	BYTE				byShiftNumber[STR_LONG_VAL10];		 
	TAG_SYSTEMTIME		cDeliveryDateTime;	 
	TAG_SYSTEMTIME		cEndDeliveryDateTime;
	BYTE				byStartVolume[STR_LONG_VAL10];		 
	BYTE				byEndVolume[STR_LONG_VAL10];			 
	BYTE				byTemperature[STR_LONG_VAL10];		 

} TAG_SHIFT_DELIVERY_REC;
*/

class CShiftDelivery : public CRelativeDoubleRecord  
{
public:
	CShiftDelivery();
	virtual ~CShiftDelivery();

	long Read(long lTank , long lFlags , TAG_SHIFT_DELIVERY_REC & cDeliveryRec);
	long Write(DELIVERY_INFO & cDelivery);

private:
	long ConvertRecordToDeliveryInfo(TAG_SHIFT_DELIVERY_REC & cDeliveryRec);
	void ConvertDeliveryInfoToRecord(DELIVERY_INFO & cDelivery);

	SHIFT_DELIVERY_REC m_cRec;
	TAG_SHIFT_DELIVERY_REC m_cTagRec;
};

#endif // !defined(AFX_SHIFTDELIVERY_H__04913DBD_89C5_4792_B787_564038B094F6__INCLUDED_)
