// FleetCard.h: interface for the CFleetCard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLEETCARD_H__AE3685E0_B256_45BE_BF8B_A5371E87F601__INCLUDED_)
#define AFX_FLEETCARD_H__AE3685E0_B256_45BE_BF8B_A5371E87F601__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFleetCard  
{
public:
	void ConvertFleetPap2Pak(long & lPumpNumber ,CPumpTransact &trs, PAY_AT_PUMP_INFO & cPayAtPumpInfo);
	void ConvertFleetPap2Pak(long & lPumpNumber ,CPumpTransact &trs, CXMLInterface & cPayAtPumpInfo);
	BOOL CheckReeferWaiting(long lPumpNumber, long lTrsNumber,  short nNozzle,long lVolume,long lValue,short nLevel,long lGradePrice,  short nGrade);
	void ConvertRowExtraInfoRecord2TAG(EXTRA_INFO_REC1	&    cUnionInfo  , TAG_DIESEL_INFO  &cTagINfo);
	void ConvertRow2TAGPreAuthDetails( PREPAY_AUTH_TRS & cRowRec, TAG_PRE_AUTH_TRS & cTagRec);
	void ConvertRow2TAGSoldDetails(PREPAY_SOLD_TRS  & cRowRec, TAG_PRE_SOLD_TRS & cTagRec);
	void ConvertTAG2RowPreAuthDetails(TAG_PRE_AUTH_TRS &  cTagRec,  PREPAY_AUTH_TRS & cRowRec);
	void ConvertTAG2RowSoldDetails(TAG_PRE_SOLD_TRS  & cTagRec, PREPAY_SOLD_TRS &  cRowRec);
	CFleetCard();
	virtual ~CFleetCard();

};

#endif // !defined(AFX_FLEETCARD_H__AE3685E0_B256_45BE_BF8B_A5371E87F601__INCLUDED_)
