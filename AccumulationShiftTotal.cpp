// AccumulationShiftTotal.cpp: implementation of the CAccumulationShiftTotal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AccumulationShiftTotal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAccumulationShiftTotal::CAccumulationShiftTotal():CRelativeFile(ACCUMULATION_SHIFT_TOTAL_FILE)
{
	InitializeCriticalSection(&m_csAccumulationShift);
}

CAccumulationShiftTotal::~CAccumulationShiftTotal()
{
	DeleteCriticalSection(&m_csAccumulationShift); 	
}
/******************************************************************************
 Description: Add final transaction to accumulation data	 	
 Returns:     None	
 Parameters:  cTrs - Transaction details	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			24/2/2004   12:55		Start
******************************************************************************/

long CAccumulationShiftTotal::AddAccumulatedTrs(CPumpTransact cTrs , BOOL bPosVoid , long lCarWash) //4.0.13.501
{	
	CString strTrs;
	AccumulationShiftTotalTransactionType eTrsType = AST_TRS_PAK;


	EnterCriticalSection(&m_csAccumulationShift);
	
	switch(cTrs.m_lRes)
	{
		case PAY_AT_PUMP:				
		{
			if (cTrs.m_lResExt == RES_EXT_AVI)
			{
				strTrs += "AVI           ";				
				eTrsType =  AST_TRS_AVI;
			}										
			else
			{
				strTrs += "PAY_AT_PUMP   ";
				eTrsType =  AST_TRS_PAP;
			}							
		}
					
		break;
		case PAY_AT_KIOSK:
		{
			if (cTrs.m_lResExt == RES_EXT_ATTENDANT)  
			{
				strTrs += "ATTENDANT     ";		
				eTrsType =  AST_TRS_ATTENDANT;
			}
			else if (cTrs.m_lResExt == RES_EXT_PAK_AUTO_RECONCILE)  
			{
				strTrs += "PAK RECONCILE ";		
				
				eTrsType =  ATS_TRS_PAK_AUTO_RECONCILE;
			}

			else 
			{												
				strTrs += "PAK			 ";		
				eTrsType =  AST_TRS_PAK;				
			}			
		}
		break;
		case PRE_PAY_AT_POS:
		{
			if (cTrs.m_lResExt == RES_EXT_CASH_ACCEPTOR)
			{
				strTrs += "CASH ACCEPTOR ";		
				eTrsType =  AST_TRS_ATTENDANT;								
			}
			else
			{
				eTrsType =  AST_TRS_PREPAY;				
				strTrs += "PRE_PAY_AT_POS";				
			}			 
		}			
		break;
		case NULL_RES:
		default:
				strTrs += "PAK			 ";		
				eTrsType =  AST_TRS_PAK;							
			break;

	}

	cTrs.m_lRoundedValue -=lCarWash;  //4.0.8.501
	if ((cTrs.m_lRoundedValue) || (cTrs.m_lRoundedVolume))
		UpdateRowData(cTrs, eTrsType , bPosVoid);


	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString strTrs1;

		strTrs1.Format("Accumulation shift total update current details, Grade %02d, Volume %06d, Value %06d,Negative=%d",
			cTrs.m_nGrade, 
			cTrs.m_lRoundedVolume, 
			cTrs.m_lRoundedValue, 
			 bPosVoid);
		_LOGMSG.LogMsg(cTrs.m_lPump,LOG_PUMP,strTrs1);

	}


	LeaveCriticalSection(&m_csAccumulationShift);
	return 0;

}
/******************************************************************************
 Description: Save final transaction into the database
 Returns:     None	
 Parameters:  cTrs		- Transaction details	
			  eTrsType  - Transaction type
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			24/2/2004   12:55		Start
******************************************************************************/

long CAccumulationShiftTotal::UpdateRowData(CPumpTransact & cTrs, AccumulationShiftTotalTransactionType  & eTrsType , BOOL & bPosVoid)
{
	GradeAccumulateDetails cGradeAccumulateDetails;
	CString sMsg;
	long lRtc=0;
	// find the offset in file,
	
	m_csFile.Lock();

	DWORD dwOffset =  GenerateIndex(cTrs.m_lActiveShiftNumber , eTrsType);
	

	lRtc = CRelativeFile::Read(dwOffset,
						(WORD)sizeof(GradeAccumulateDetails),
						&cGradeAccumulateDetails,HOP_DIRECT_LOCAL,0,0);


	if(lRtc)
	{
		if(LOG_ERROR)
		{
			sMsg.Format("CAccumulationShiftTotal::UpdateRowData, Read  trs=%ld,error %d, (indx_num %d, offset %d, length %d)",cTrs.m_lNumber,lRtc,q_param.indx_num,q_param.write_part_offset,q_param.write_part_length);
			_LOGMSG.LogMsg(cTrs.m_lPump,LOG_PUMP,sMsg,LOG_LEVEL_0);
		}
	}
	else
	{
		AddTotal(cTrs, cGradeAccumulateDetails, eTrsType ,bPosVoid);

		lRtc = CRelativeFile::Write(	dwOffset,
							(WORD)sizeof(GradeAccumulateDetails),
							&cGradeAccumulateDetails, HOP_DEFAULT_CONFIG,0,0,0,0,0);

		if(LOG_DETAIL_FLOW_CONTROL)
		{			
			sMsg.Format("Update Accumulation record , write  trs=%ld error %d, (indx_num %d, offset %d, length %d)",cTrs.m_lNumber,lRtc,q_param.indx_num,q_param.write_part_offset,q_param.write_part_length);
			_LOGMSG.LogMsg(cTrs.m_lPump,LOG_PUMP,sMsg);
		}

	}

	m_csFile.Unlock();

	return 0;
}
/******************************************************************************
 Description: Save final transaction into the database
 Returns:     None	
 Parameters:  cTrs		- Transaction details	
			  eTrsType  - Transaction type
			  cGradeAccumulateDetails - accumulate row data.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			24/2/2004   12:55		Start
******************************************************************************/

long CAccumulationShiftTotal::AddTotal(CPumpTransact &cTrs, GradeAccumulateDetails &cGradeAccumulateDetails, AccumulationShiftTotalTransactionType & eTrsType , BOOL & bPosVoid)
{
	long lGrade =  cTrs.m_nGrade;

	// check record signature 
	if (cGradeAccumulateDetails.sUsed != AST_RECORD_IN_USED)
	{
		
		memset(&cGradeAccumulateDetails ,0, sizeof(GradeAccumulateDetails));
		SignedRecordAsUsed(cGradeAccumulateDetails, cTrs.m_lActiveShiftNumber);
		
		cGradeAccumulateDetails.lTransactionType = eTrsType;
		ClearShiftdata(cTrs.m_lActiveShiftNumber); 
	}

	if (!lGrade)
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("CAccumulationShiftTotal::AddTotal, no Grade=%ld , shift=%ld, value=%ld" , 
						cTrs.m_nGrade,
						cTrs.m_lActiveShiftNumber,
						cTrs.m_lRoundedValue);
						_LOGMSG.LogMsg(cTrs.m_lPump,LOG_PUMP,str);
		}

	}

	// subtraction negative transaction 
	if (bPosVoid)
	{
		// Accumulate data
		cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lGrade].lCounter--;
		cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lGrade].lTotalVale   -=   cTrs.m_lRoundedValue;
		cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lGrade].lTotalVolume -= cTrs.m_lRoundedVolume;

	}
	else
	{
		// Accumulate data
		cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lGrade].lCounter++;
		cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lGrade].lTotalVale   +=   cTrs.m_lRoundedValue;
		cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lGrade].lTotalVolume += cTrs.m_lRoundedVolume;

	}


	return 0;
}

/******************************************************************************
 Description: Check process activity
 Returns:     Process activity  0- no 1-yes   
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			24/2/2004   12:55		Start
******************************************************************************/

BOOL CAccumulationShiftTotal::IsProcessActive()
{
	return m_bActiveAccumulationProcess;
}
/******************************************************************************
 Description: Check process activity
 Returns:     None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			24/2/2004   12:55		Start
******************************************************************************/
void CAccumulationShiftTotal::SetProcessActive(BOOL bVal)
{
	m_bActiveAccumulationProcess = bVal;
}

long CAccumulationShiftTotal::GetReport(long lShiftNumber, long lState, long lPosNumber, long lFlags, long lVal, SYSTEMTIME  & cStartTime, SYSTEMTIME  & cEndTime, long *lRecordCount, BSTR  & sRec)
{
	GradeAccumulateDetails cGradeAccumulateDetails;
	TransactionAccumulationTotals cTransactionAccumulationTotals;
	long lShiftId = 0; 
	long lRetVal = 0 ;
	long lIndex = 0 ; 
	long lTmpFlgas = lFlags & 0xffff0000;

	// Clear all data. 
	memset(&cGradeAccumulateDetails , 0 , sizeof(cGradeAccumulateDetails));
	(*lRecordCount) =0;
	
	
	switch (lTmpFlgas)
	{
		case GAT_BY_DATE:
		{
			lRetVal = GetReportByDate(lPosNumber ,lFlags,lVal,cStartTime, cEndTime, lRecordCount, sRec);
		}
		break;
		case GAT_LAST_REPORT:
		{
			lRetVal =  GetLastReport(lPosNumber,lShiftNumber, lFlags,lVal,lRecordCount, sRec);
		}
		break;
		case GAT_ALL_REPORT:
		{
			lRetVal =  GetAllReport(lPosNumber ,lShiftNumber, lFlags, lVal,*lRecordCount, sRec);
		}
		break;

		case GAT_CURRENT_REPORTS:
		{
			lShiftId = _Module.m_server.GetActiveShiftNumber();

			if (lFlags & GAT_BY_GRADE)
				lRetVal = GetRowDataByGrade(lPosNumber,lShiftId, 0 ,lFlags, lVal, lRecordCount, cGradeAccumulateDetails , sRec);
			else
				lRetVal = GetRowDataByTransaction(lPosNumber,lShiftId, 0,lFlags ,lVal, lRecordCount, cTransactionAccumulationTotals , sRec);			
		}
		break;

		default:

			break;
	}


	return 0;
}

long CAccumulationShiftTotal::GetRowDataByGrade(long lPosNumber,long lShiftNumber, long lIndex, long & lFlags,long lVal, long *lRecordCount, GradeAccumulateDetails   & cGradeAccumulateDetailsCounter, BSTR & sRec)
{
	GradeAccumulateDetails cGradeAccumulateDetails;
	long lIndexNumber = 0;
	long lRetVal = 0;	
	long lLoopIndex =0; 

	long lTrsFlags  = AST_BIT_MAP_ALL_TRS_SELECT ;	

	// transaction restriction
	if (lFlags & AST_BIT_MAP_ALL_TRS_SELECT) 
	{
		lTrsFlags = lFlags & AST_BIT_MAP_ALL_TRS_SELECT;
		lIndexNumber=1; // 4.0.5.39
		
	}
	
	while (!lRetVal) 
	{		
		if (lTrsFlags & 1)
		{
			lRetVal = Read(lShiftNumber, lIndexNumber,cGradeAccumulateDetails);						

			// found validity record 
			if ((!lRetVal)  &&  
				(IsRecordValidToRead(cGradeAccumulateDetails ,lShiftNumber, lPosNumber)))
			{
				// select only grade. 
				if (lVal)
				{
					cGradeAccumulateDetailsCounter.cArrayGradeAccumulateTotals[lVal].lCounter		+= cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lVal].lCounter;
					cGradeAccumulateDetailsCounter.cArrayGradeAccumulateTotals[lVal].lTotalVale		+= cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lVal].lTotalVale;
					cGradeAccumulateDetailsCounter.cArrayGradeAccumulateTotals[lVal].lTotalVolume	+= cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lVal].lTotalVolume;
				}
				else
				{
					
					for (lLoopIndex=0 ; lLoopIndex < AST_MAX_GRADES;lLoopIndex++)
					{						
						cGradeAccumulateDetailsCounter.cArrayGradeAccumulateTotals[lLoopIndex].lCounter		+= cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lLoopIndex].lCounter;
						cGradeAccumulateDetailsCounter.cArrayGradeAccumulateTotals[lLoopIndex].lTotalVale		+= cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lLoopIndex].lTotalVale;
						cGradeAccumulateDetailsCounter.cArrayGradeAccumulateTotals[lLoopIndex].lTotalVolume	+= cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lLoopIndex].lTotalVolume;
					}
				}


				(*lRecordCount)++;
				
			}
			// problem in reading qdx
			else if (lRetVal)
			{
				break;
			}
			// end of list 
			else if (cGradeAccumulateDetails.lShiftNumber != lShiftNumber)
			{

				break;	
			}

		}

		lIndexNumber++;

		// move bit right to next transaction
		lTrsFlags = lTrsFlags >> 1;

		// no more restriction need
		if (!lTrsFlags)
			break;

	}

	if (*lRecordCount)
	{
		BYTE sTmpStr[sizeof(TAG_ACCUMULATE_TOTALS) +2];
		
		TAG_ACCUMULATE_TOTALS cTagAccumulateTotals;
		CComBSTR bstrTmp;
		(*lRecordCount) = 0;

		for (lLoopIndex=0 ; lLoopIndex < AST_MAX_GRADES;lLoopIndex++)
		{
			if (cGradeAccumulateDetailsCounter.cArrayGradeAccumulateTotals[lLoopIndex].lCounter)
			{

				memset(sTmpStr , 0, sizeof(sTmpStr));
				memset(&cTagAccumulateTotals , ' ', sizeof(cTagAccumulateTotals));

				l2a(lLoopIndex ,cTagAccumulateTotals.sGradeId, sizeof(cTagAccumulateTotals.sGradeId));
				l2a(lShiftNumber , cTagAccumulateTotals.sShiftNumber , sizeof(cTagAccumulateTotals.sShiftNumber));

				lTrsFlags = lFlags & AST_BIT_MAP_ALL_TRS_SELECT;			
				l2a(lTrsFlags ,cTagAccumulateTotals.sTransactionType , sizeof(cTagAccumulateTotals.sTransactionType));					
				l2a(cGradeAccumulateDetailsCounter.cArrayGradeAccumulateTotals[lLoopIndex].lCounter ,    cTagAccumulateTotals.sCounter , sizeof(cTagAccumulateTotals.sCounter));
				l2a(cGradeAccumulateDetailsCounter.cArrayGradeAccumulateTotals[lLoopIndex].lTotalVale,   cTagAccumulateTotals.sVal , sizeof(cTagAccumulateTotals.sVal)); 
				l2a(cGradeAccumulateDetailsCounter.cArrayGradeAccumulateTotals[lLoopIndex].lTotalVolume	,cTagAccumulateTotals.sVolume, sizeof(cTagAccumulateTotals.sVolume));				
				SystemTime2Tag(&cGradeAccumulateDetails.cTime, &cTagAccumulateTotals.sDateTime);				
						
				memcpy((BYTE*)sTmpStr , &cTagAccumulateTotals , sizeof(cTagAccumulateTotals));
				bstrTmp.Append((LPCSTR)sTmpStr);				
				(*lRecordCount)++;
			}
		}

		sRec = bstrTmp.Copy();
	}

	return 0;
}

long CAccumulationShiftTotal::Read(long lShiftNumber, long lIndexNumber, GradeAccumulateDetails &cGradeAccumulateDetails)
{
	long lRtc = 0 ;

	m_csFile.Lock();

	DWORD dwOffset =  GenerateIndex(lShiftNumber, lIndexNumber);

	lRtc = CRelativeFile::Read(dwOffset,
						(WORD)sizeof(GradeAccumulateDetails),
						&cGradeAccumulateDetails,HOP_DIRECT_LOCAL,0,0);
	if(lRtc)
	{
		if(LOG_ERROR)
		{
			CString str;
			str.Format("CAccumulationShiftTotal::Read, error ,error %d, (indx_num %d, offset %d, length %d)",lRtc,q_param.indx_num,q_param.write_part_offset,q_param.write_part_length);
			_LOGMSG.LogMsg(str,LOG_LEVEL_0);
		}
	}

	m_csFile.Unlock();		


	return lRtc;
}
/******************************************************************************
 Description:	Get sales report by transaction for single shift 	
 Returns:      	
 Parameters:   	
		long lShiftNumber,  - shift number 
		long lIndex,		- Transaction Type 
		long lVal, 
		long *lRecordCount  - Total record found 
		TransactionAccumulationTotals & cTransactionAccumulationTotals, 
		BSTR & sRec         - Bstr buffer output.  
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			26/2/2004   10:29		Start
******************************************************************************/

long CAccumulationShiftTotal::GetRowDataByTransaction(long lPosNumber,long lShiftNumber, long lIndex, long & lFlags,long lVal, long *lRecordCount, TransactionAccumulationTotals &cTransactionAccumulationTotals, BSTR & sRec)
{
	GradeAccumulateDetails cGradeAccumulateDetails;
	DWORD dwOffset =0;
	long lIndexNumber = 0;
	long lRetVal = 0;	
	long lLoopIndex =0; 
	long lTrsFlags  = AST_BIT_MAP_ALL_TRS_SELECT ;

	memset(&cTransactionAccumulationTotals , 0, sizeof(cTransactionAccumulationTotals));
	

	// transaction restriction
	if (lFlags & AST_BIT_MAP_ALL_TRS_SELECT) 
	{
		lTrsFlags = lFlags & AST_BIT_MAP_ALL_TRS_SELECT;
		lIndexNumber++;
	}

	// This strcut hold the accumulation data 	
	while (!lRetVal) 
	{						
		if (lTrsFlags & 1)
		{			
			dwOffset =  GenerateIndex(lShiftNumber, lIndexNumber);

			lRetVal = Read(lShiftNumber,lIndexNumber ,cGradeAccumulateDetails);

			if ((!lRetVal) &&   //4.0.5.39
				(IsRecordValidToRead(cGradeAccumulateDetails,lShiftNumber, lPosNumber)))
							  
			{
				for (lLoopIndex=0 ; lLoopIndex < AST_MAX_GRADES;lLoopIndex++)
				{
					cTransactionAccumulationTotals.cGradeAccumulateTotals[lIndexNumber].lCounter		+= cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lLoopIndex].lCounter ;
					cTransactionAccumulationTotals.cGradeAccumulateTotals[lIndexNumber].lTotalVale		+= cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lLoopIndex].lTotalVale ;
					cTransactionAccumulationTotals.cGradeAccumulateTotals[lIndexNumber].lTotalVolume	+= cGradeAccumulateDetails.cArrayGradeAccumulateTotals[lLoopIndex].lTotalVolume ;
				}

				(*lRecordCount)++;			
				
			}
			// problem in reading qdx
			else if (lRetVal)
			{
				break;
			}
			// end of list 
			else if (cGradeAccumulateDetails.lShiftNumber != lShiftNumber)
			{
				break;	
			}
			
		}

		lIndexNumber++;
		// move bit right to next transaction
		lTrsFlags = lTrsFlags >> 1;

		// no more restriction need
		if (!lTrsFlags)
			break;
	}

	if (*lRecordCount)
	{
		BYTE sTmpStr[sizeof(TAG_ACCUMULATE_TOTALS) +2];
		
		TAG_ACCUMULATE_TOTALS cTagAccumulateTotals;
		CComBSTR bstrTmp = NULL;
		(*lRecordCount) = 0;
		
		for (int i=0; i< AST_MAX_TRANSACTION_TYPE; i++)
		{
			if ((cTransactionAccumulationTotals.cGradeAccumulateTotals[i].lTotalVale) || 
				(cTransactionAccumulationTotals.cGradeAccumulateTotals[i].lTotalVolume))
			{
				memset(sTmpStr , 0, sizeof(sTmpStr));
				memset(&cTagAccumulateTotals , ' ', sizeof(cTagAccumulateTotals));
				l2a(99 ,cTagAccumulateTotals.sGradeId, sizeof(cTagAccumulateTotals.sGradeId));
				l2a(lShiftNumber , cTagAccumulateTotals.sShiftNumber , sizeof(cTagAccumulateTotals.sShiftNumber));
				l2a(i ,cTagAccumulateTotals.sTransactionType , sizeof(cTagAccumulateTotals.sTransactionType));
				l2a(cTransactionAccumulationTotals.cGradeAccumulateTotals[i].lCounter ,    cTagAccumulateTotals.sCounter , sizeof(cTagAccumulateTotals.sCounter));
				l2a(cTransactionAccumulationTotals.cGradeAccumulateTotals[i]. lTotalVolume,   cTagAccumulateTotals.sVolume , sizeof(cTagAccumulateTotals.sVal)); 
				l2a(cTransactionAccumulationTotals.cGradeAccumulateTotals[i].lTotalVale	,cTagAccumulateTotals.sVal, sizeof(cTagAccumulateTotals.sVolume));				
				SystemTime2Tag(&cGradeAccumulateDetails.cTime, &cTagAccumulateTotals.sDateTime);				
						
				memcpy((BYTE*)sTmpStr , &cTagAccumulateTotals , sizeof(cTagAccumulateTotals));
				bstrTmp.Append((LPCSTR)sTmpStr);
				(*lRecordCount)++ ;
				
			}
		}		

		sRec = bstrTmp.Copy();
	}

	return 0;
}

/******************************************************************************
 Description: Get sale report between to period of time 
 Returns:      	
 Parameters: 
	long lFlags, 
	SYSTEMTIME & sStartTime -  start date report
	SYSTEMTIME  & cEndTime  -  end date report 
	long *lRecordCount		- total record  
	BSTR  & sRec			- buffer result 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			26/2/2004   10:05		Start
******************************************************************************/

long CAccumulationShiftTotal::GetReportByDate(long lPosNumber,long lFlags,long  & lVal, SYSTEMTIME & sStartTime, SYSTEMTIME  & cEndTime, long *lRecordCount, BSTR  & sRec)
{
	TransactionAccumulationTotals cTransactionAccumulationTotals;
	GradeAccumulateDetails  cGradeAccumulateDetails;	
	long		lRetVal = 0;
	long		lpResultStart ,lpResultEnd  ,lResultType =0 ,lStartRtc, lEndRtc ;
	CComBSTR	bstrComTmp;
	BSTR		bstrTmp= NULL;
	long		lCounter = 0 ;

	*lRecordCount = 0;

	
	 for (long l= 1;l<= AST_MAX_SHIFT_ACTIVE_IN_FILE;l++)
	 {		 
		lRetVal = Read(l,0, cGradeAccumulateDetails);
		
		if (!lRetVal && (cGradeAccumulateDetails.sUsed == AST_RECORD_IN_USED) && (!cGradeAccumulateDetails.lRecordStatus))
		{				
			lStartRtc = CompareSystemTime(&sStartTime, &cGradeAccumulateDetails.cTime,&lpResultStart, INTERVAL_IN_SECONDS);
			lEndRtc = CompareSystemTime(&cGradeAccumulateDetails.cTime, &cEndTime, &lpResultEnd, INTERVAL_IN_SECONDS);

			//  start bigger then current and currect bigger then end period
			if ((lStartRtc == 1) || (lEndRtc == 1))
			{
				// faild	
			}
			else
			{
				 bstrTmp= NULL;

				if (lFlags & GAT_BY_GRADE)
					lRetVal = GetRowDataByGrade(lPosNumber,cGradeAccumulateDetails.lShiftNumber, 0 ,lFlags,lVal, &lCounter, cGradeAccumulateDetails , bstrTmp);
				else
					lRetVal = GetRowDataByTransaction(lPosNumber,cGradeAccumulateDetails.lShiftNumber, 0, lFlags,0, &lCounter, cTransactionAccumulationTotals , bstrTmp);			
			
				if (lCounter)
				{
					*lRecordCount += lCounter;
					bstrComTmp.Append(bstrTmp);
				}

				if ( bstrTmp)
					SysFreeString(bstrTmp);									
			}								
		}			
	 }
		
	 // Copy struct to buffer.
	if (*lRecordCount)
	{
		sRec = bstrComTmp.Copy();
	}

	return 0;
}

/******************************************************************************
 Description:	Get latest sales report 	
 Returns:      	
 Parameters:   	long lShiftNumber, 
				long lFlags, 
				lVal - Grade restriction
				long *lRecordCount - Record found counter  
				BSTR  & sRec , 
				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			26/2/2004   11:29		Start
******************************************************************************/
long CAccumulationShiftTotal::GetLastReport(long lPosNumber,long lShiftNumber, long lFlags, long lVal, long *lRecordCount, BSTR  & sRec)
{
	long lQdxRtc , lRetVal =0;;  
	GradeAccumulateDetails	cGradeAccumulateDetails;
	TransactionAccumulationTotals  cTransactionAccumulationTotals;

	long lShiftId = _Module.m_server.GetActiveShiftNumber();
		
	for (int i=0; i < (AST_MAX_SHIFT_ACTIVE_IN_FILE - 1); i++)
	{
		lShiftId--;
		if (lShiftId < 0)
			lShiftId = 9;

		lQdxRtc = Read(lShiftId,0, cGradeAccumulateDetails);
		// check record validity
		if (!lQdxRtc && (cGradeAccumulateDetails.sUsed == AST_RECORD_IN_USED) && (!cGradeAccumulateDetails.lRecordStatus))
		{
			if (lFlags & GAT_BY_GRADE)
				lRetVal = GetRowDataByGrade(lPosNumber,lShiftId, 0, lFlags ,lVal, lRecordCount, cGradeAccumulateDetails , sRec);
			else
				lRetVal = GetRowDataByTransaction(lPosNumber,lShiftId, 0, lFlags ,0, lRecordCount, cTransactionAccumulationTotals , sRec);			

			//latet record was found, end search. 
			break;			

		}
	}

	return 0;
}

/******************************************************************************
 Description:	Clear shift buffer and prepared it to new shift. 	
 Returns:      	
 Parameters:   lShiftNumber - shift number to be delete 	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			26/2/2004   10:44		Start
******************************************************************************/

void CAccumulationShiftTotal::ClearShiftdata(long lShiftNumber)
{
	GradeAccumulateDetails cGradeAccumulateDetails;
	long lRtc = 0;

	memset(&cGradeAccumulateDetails , 0, sizeof(GradeAccumulateDetails));

	for (int i=0; i< AST_MAX_TRANSACTION_TYPE ; i++)
	{
		SignedRecordAsUsed(cGradeAccumulateDetails, lShiftNumber);
		cGradeAccumulateDetails.lTransactionType = i;

		lRtc = Write(lShiftNumber, i, cGradeAccumulateDetails);

		if(lRtc)
		{
			if(LOG_ERROR)
			{
				CString str;
				str.Format("CAccumulationShiftTotal::ClearShiftdata for shift=%ld, trs=%d , error ,error %d, (indx_num %d, offset %d, length %d)",lShiftNumber, i , lRtc,q_param.indx_num,q_param.write_part_offset,q_param.write_part_length);
				_LOGMSG.LogMsg(str,LOG_LEVEL_0);
			}
		}
	}
}

long CAccumulationShiftTotal::Write(long lShiftNumber, long lIndexNumber, GradeAccumulateDetails &cGradeAccumulateDetails)
{

	long lRtc = 0 ;

	m_csFile.Lock();


	DWORD dwOffset =  GenerateIndex(lShiftNumber, lIndexNumber);

	lRtc = CRelativeFile::Write(	dwOffset,
						(WORD)sizeof(GradeAccumulateDetails),
						&cGradeAccumulateDetails, HOP_DEFAULT_CONFIG,0,0,0,0,0);
	if(lRtc)
	{
		if(LOG_ERROR)
		{
			CString str;
			str.Format("CAccumulationShiftTotal::write, error ,error %d, (indx_num %d, offset %d, length %d)",lRtc,q_param.indx_num,q_param.write_part_offset,q_param.write_part_length);
			_LOGMSG.LogMsg(str,LOG_LEVEL_0);
		}
	}

	m_csFile.Unlock();		


	return lRtc;

}
/******************************************************************************
 Description:	Get All open report except for the  current shift 	
 Returns:      	
 Parameters:   	(long lShiftNumber, long lFlags, long & lRecordCount, BSTR &sRec)
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			26/2/2004   12:04		Start
******************************************************************************/
long CAccumulationShiftTotal::GetAllReport(long lPosNumber,long lShiftNumber, long lFlags, long & lVal,long & lRecordCount, BSTR &sRec)
{
	long lQdxRtc , lRetVal =0;
	GradeAccumulateDetails	cGradeAccumulateDetails;
	TransactionAccumulationTotals  cTransactionAccumulationTotals;
	CComBSTR bstrCom = NULL; 
	BSTR     bstrTmp = NULL;
	long     lLocalCounter = 0;
	long	 lShiftId = _Module.m_server.GetActiveShiftNumber();

	lRecordCount =0; 
	memset(&cGradeAccumulateDetails , 0 , sizeof(cGradeAccumulateDetails));
	memset(&cTransactionAccumulationTotals, 0, sizeof(cTransactionAccumulationTotals));


	if (lShiftNumber)
	{

		if (lFlags & GAT_BY_GRADE)
			lRetVal = GetRowDataByGrade(lPosNumber,lShiftNumber, 0 ,lFlags,lVal, &lLocalCounter, cGradeAccumulateDetails , bstrTmp);
		else
			lRetVal = GetRowDataByTransaction(lPosNumber,lShiftNumber, 0, lFlags,0, &lLocalCounter, cTransactionAccumulationTotals , bstrTmp);			

		//4.0.5.30
		if (lLocalCounter)
		{
			lRecordCount += lLocalCounter;
			bstrCom.Append(bstrTmp);

			if (bstrTmp)
				SysFreeString(bstrTmp);

		}
	}
	else
	{
		for (int i=0; i < (AST_MAX_SHIFT_ACTIVE_IN_FILE - 1); i++)
		{
			lShiftId--;
			if (lShiftId < 0)
				lShiftId = 9;

			lQdxRtc = Read(lShiftId,0, cGradeAccumulateDetails);
			// check record validity
			if (!lQdxRtc && (cGradeAccumulateDetails.sUsed == AST_RECORD_IN_USED) && (!cGradeAccumulateDetails.lRecordStatus))
			{
				if (lFlags & GAT_BY_GRADE)
					lRetVal = GetRowDataByGrade(lPosNumber,lShiftId, 0 ,lFlags,0, &lLocalCounter, cGradeAccumulateDetails , bstrTmp);
				else
					lRetVal = GetRowDataByTransaction(lPosNumber,lShiftId, 0, lFlags,0, &lLocalCounter, cTransactionAccumulationTotals , bstrTmp);			

				if (lLocalCounter)
				{
					lRecordCount += lLocalCounter;
					bstrCom.Append(bstrTmp);
					if (bstrTmp)
						SysFreeString(bstrTmp);

					bstrTmp = NULL;
				}
				
				lLocalCounter = 0;
			}
		}

	}
		
	if (lRecordCount)
	{
		sRec = bstrCom.Copy();	// 4.0.5.30	  
	}
		
	return 0;
}

/******************************************************************************
 Description:	Sign record as process.
 Returns:      	1- failed , 0 - succeded .
 Parameters:   	long lShiftNumber, long lState, long lPosNumber, long lFlags
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			26/2/2004   12:04		Start
******************************************************************************/
long CAccumulationShiftTotal::SetRecordStatus(long lShiftNumber, long lState, long lPosNumber, long lFlags)
{	
	GradeAccumulateDetails cGradeAccumulateDetails;
	CString sMsg;
	long lRtc = 0;
	long lRetCode = 1; 
	long lDataBaseShift=0;
	BOOL bUpdate =FALSE;
	
	EnterCriticalSection(&m_csAccumulationShift);
	memset(&cGradeAccumulateDetails , 0, sizeof(GradeAccumulateDetails));

	lRtc = Read(lShiftNumber, 0,  cGradeAccumulateDetails);

	if (!lRtc)
	{
		lDataBaseShift = cGradeAccumulateDetails.lShiftNumber;
		switch(lState)
		{
			case ARS_ON_OFFER:
			{
				if (!cGradeAccumulateDetails.lRecordStatus) 
				{				
					lState = ARS_ON_OFFER;
					bUpdate = TRUE; 
				}
			}
			break;
			case ARS_LOCKED:
			{
				if (((cGradeAccumulateDetails.lRecordStatus == ARS_ON_OFFER) && (cGradeAccumulateDetails.lPosNumber == lPosNumber)) ||
					(cGradeAccumulateDetails.lRecordStatus == ARS_NON_ON_OFFER))				
				{
					bUpdate = TRUE; 
					lState = ARS_LOCKED;
				}
			}
			break;
			case ARS_FORCE_RELEASE:
			{
				bUpdate = TRUE; 
				lState = ARS_NON_ON_OFFER;
			}
		}

		if (bUpdate)
		{
			for (int i=0; i< AST_MAX_TRANSACTION_TYPE ; i++)
			{
				lRtc = Read(lShiftNumber, i,  cGradeAccumulateDetails);

				cGradeAccumulateDetails.lRecordStatus  = lState;
				cGradeAccumulateDetails.lPosNumber = lPosNumber;			

				lRtc = Write(lShiftNumber, i, cGradeAccumulateDetails);

				if(lRtc)
				{
					if(LOG_ERROR)
					{						
						sMsg.Format("CAccumulationShiftTotal::SetRecordStatus for shift=%ld, trs=%d , error ,error %d, (indx_num %d, offset %d, length %d)",lShiftNumber, i , lRtc,q_param.indx_num,q_param.write_part_offset,q_param.write_part_length);
						_LOGMSG.LogMsg(sMsg,LOG_LEVEL_0);
					}
				}
			}
			// update succeeded	
			lRetCode = 0;
		}
				
	}
	else
	{
		if(LOG_ERROR)
		{
			sMsg.Format("CAccumulationShiftTotal::SetRecordStatus1  for shift=%ld,  error ,error %d, (indx_num %d, offset %d, length %d)",lShiftNumber, lRtc,q_param.indx_num,q_param.write_part_offset,q_param.write_part_length);
			_LOGMSG.LogMsg(sMsg,LOG_LEVEL_0);
		}

	}
	
	LeaveCriticalSection(&m_csAccumulationShift);


	if(LOG_DETAIL_FLOW_CONTROL)
	{
		sMsg.Format("CAccumulationShiftTotal::SetRecordStatus lShiftNumber=%ld,lPosNumber=%ld,lState=%ld ,lFlags=%ld, DatabaseShift=%ld,lDataBaseShift=%ld,ResultCode=%d", 
				lShiftNumber,lPosNumber, lState, lFlags, bUpdate, bUpdate);
		_LOGMSG.LogMsg(sMsg);
	}


	return (lRetCode);
}

long CAccumulationShiftTotal::GenerateIndex(long lShift, long lTrsIndex)
{
	DWORD dwOffset= 0;  

	dwOffset=  		     
	// one block
	((lShift % AST_MAX_SHIFT_ACTIVE_IN_FILE)) * (AST_MAX_TRANSACTION_TYPE * sizeof(GradeAccumulateDetails)) +
	(lTrsIndex *  sizeof(GradeAccumulateDetails));

	return ((dwOffset > 0)?dwOffset: 0); 
}

long CAccumulationShiftTotal::SignedRecordAsUsed(GradeAccumulateDetails &cGradeAccumulateDetails,long lShiftNumber)
{
	SYSTEMTIME  cSystemtime;   

	GetLocalTime(&cSystemtime);

	memset(&cGradeAccumulateDetails , 0, sizeof(cGradeAccumulateDetails));
	cGradeAccumulateDetails.sUsed = AST_RECORD_IN_USED;
	cGradeAccumulateDetails.lShiftNumber = lShiftNumber;
	cGradeAccumulateDetails.cTime   = cSystemtime;

	return 0;
}
/******************************************************************************
 Description:	Verify that the record reading is valid to be process 	
 Returns:      	
 Parameters:   TRUE - Valid, FALSE 	not valid
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			13/5/2004   13:58		Start
******************************************************************************/

BOOL CAccumulationShiftTotal::IsRecordValidToRead(GradeAccumulateDetails &cGradeAccumulateDetails, long lShiftNumber, long lPosNumber)
{
	if ((cGradeAccumulateDetails.lShiftNumber == lShiftNumber) && 
		(
		(!cGradeAccumulateDetails.lRecordStatus) ||
		((cGradeAccumulateDetails.lRecordStatus !=ARS_LOCKED) && (cGradeAccumulateDetails.lPosNumber == lPosNumber))
		))
		return TRUE;
	else
		return FALSE;
			
}
