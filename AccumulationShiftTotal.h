// AccumulationShiftTotal.h: interface for the CAccumulationShiftTotal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCUMULATIONSHIFTTOTAL_H__E5EBA158_B58E_4273_96E6_445F53B1B03E__INCLUDED_)
#define AFX_ACCUMULATIONSHIFTTOTAL_H__E5EBA158_B58E_4273_96E6_445F53B1B03E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DB.h"



// General definition 
#define AST_MAX_TRANSACTION_TYPE     10  // 4.0.7.508
#define AST_MAX_SHIFT_ACTIVE_IN_FILE 10

//#define AST_BIT_MAP_ALL_TRS_SELECT    0x3f

#define AST_BIT_MAP_ALL_TRS_SELECT    0xFF



// general defind 
#define AST_RECORD_IN_USED				'1'	
#define ACCUMULATE_SHIFT_TOTAL		(_Module.m_server.m_cAccumulationShiftTotal.IsProcessActive()) 

typedef struct
{
	long		lShiftNumber;		
	SYSTEMTIME	cTime;									 // 221 - 236 					
	GradeAccumulateTotals 	 cGradeAccumulateTotals[AST_MAX_TRANSACTION_TYPE];
}TransactionAccumulationTotals; 

GradeAccumulateTotals   cArrayGradeAccumulateTotals[AST_MAX_GRADES];  // 17  - 220 -  12 * 17 = 204

enum AccumulateRecordStatus
{
	ARS_NON_ON_OFFER = 0,
	ARS_ON_OFFER= 1,
	ARS_LOCKED= 2,
	ARS_FORCE_RELEASE=3,
};



class CAccumulationShiftTotal : public CRelativeFile  
{
public:
	long SignedRecordAsUsed(GradeAccumulateDetails &cGradeAccumulateDetails, long lShiftNumber);
	long SetRecordStatus(long lShiftNumber, long lState, long lPosNumber, long lFlags);
	
	void ClearShiftdata(long lShiftNumber);
	void SetProcessActive(BOOL bVal);
	BOOL IsProcessActive();
	long AddAccumulatedTrs (CPumpTransact cTrs , BOOL bPosVoid= FALSE ,long lCarWash=0); //4.0.8.340
	long GetReport(long lShiftNumber, long lState, long lPosNumber, long lFlags, long lVal, SYSTEMTIME & cStartTime, SYSTEMTIME & cEndTime, long *lRecordCount, BSTR  & sRec);
	CAccumulationShiftTotal();
	virtual ~CAccumulationShiftTotal();
private:
	BOOL IsRecordValidToRead(GradeAccumulateDetails  & cGradeAccumulateDetails , long lShiftNumber, long  lPosNumber);
	long GenerateIndex(long lShift , long lTrsIndex);
	long GetAllReport(long lPosNumber,long lShiftNumber, long lFlags,long & lVal ,long & lRecordCount, BSTR &sRec);
	long Write(long lShiftNumber, long lIndexNumber, GradeAccumulateDetails &cGradeAccumulateDetails);
	long GetLastReport(long lPosNumber,long lShiftNumber, long lFlags, long lVal, long * lRecordCount, BSTR  & sRec);
	long GetReportByDate(long lPosNumber,long lFlags, long  & lVal ,SYSTEMTIME & cStartTime, SYSTEMTIME & cEndTime, long *lRecordCount, BSTR  & sRec);
	long GetRowDataByTransaction(long lPosNumber,long lShiftNumber, long lIndex,long & lFlags, long lVal, long *lRecordCount, TransactionAccumulationTotals   & cTransactionAccumulationTotals , BSTR & sRec);
	long Read(long lShiftNumber, long lIndexNumber, GradeAccumulateDetails &cGradeAccumulateDetails);
	long GetRowDataByGrade(long lPosNumber,long lShiftNumber, long lIndex,long & lFlags ,long lVal, long *lRecordCount, GradeAccumulateDetails & cGradeAccumulateDetailsCounter , BSTR & sRec);	
	BOOL m_bActiveAccumulationProcess;
	long AddTotal(CPumpTransact & cTrs ,GradeAccumulateDetails & cGradeAccumulateDetails, AccumulationShiftTotalTransactionType & eTrsType , BOOL & bPosVoid);
	long UpdateRowData(CPumpTransact & cTrs , AccumulationShiftTotalTransactionType & eTrsType, BOOL & bPosVoid);
	CRITICAL_SECTION m_csAccumulationShift;
};

#endif // !defined(AFX_ACCUMULATIONSHIFTTOTAL_H__E5EBA158_B58E_4273_96E6_445F53B1B03E__INCLUDED_)
