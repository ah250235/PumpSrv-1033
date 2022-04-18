// Ctrl.cpp : Implementation of CCtrl
#include "stdafx.h"
#include "PumpSrv.h"
#include "Ctrl.h"

#include "ShiftTanks.h"
#include "ShiftTotals.h"
#include "ShiftDelivery.h"

#include "RegTable.h"
#include "XMLInterface.h" 
#include "SQLPumpSrvFile.h"
#include "PumpSrvDatabase.h"

/////////////////////////////////////////////////////////////////////////////
// CCtrl


void CCtrl::SetGradePrice__(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long lPrice, BSTR sTimeValid, short nFlags, long * pVal)
{
	CtrlRetCode retCode = CTRL_OK;
	DWORD dwLastErr	= 0;

	if((nGrade > MAX_GRADES_99)||(nGrade <= 0))//amram 4.0.10.501
		retCode = CTRL_INVALID_GRADE_NUMBER;

	if(retCode == CTRL_OK)
	{
		if (_Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags()||
			_Module.m_server.m_cServerInfo.GetRequiredPriceChangeFlags()||
			_Module.m_server.m_cServerInfo.GetRequiredModeProcessFlags() ||
			_Module.m_server.m_cServerInfo.GetWaitForPriceChangeAckFlags() ||      //RFUEL-696
			_Module.m_server.m_cServerInfo.GetRequiredMaintenanceProcessFlags())		
		{
			retCode = CTRL_IN_MIDDLE_OF_PROCESS;
			_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
		}
		
	}

	if(retCode == CTRL_OK)
	{
		DWORD dwTmp;
		BOOL bIgnoreGrade = FALSE; //4.0.24.70 - TD 110193
		CString str("Grades\\Grade");
		
		char sNumber[8];
		memset(sNumber,0,sizeof(sNumber));
		sprintf(sNumber,"%02d",nGrade);
		str += sNumber;


		SZ_TAG_SYSTEMTIME szTagTime;
		memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
		int iLen = WideCharToMultiByte(CP_ACP,0,sTimeValid,-1,(char *)&szTagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = CTRL_LAST_ERROR;
		}

		//4.0.27.20 - TD 215136
		if (lPrice <= 0)
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Invalid Grade price !!! Price %ld, Grade %d, Mode %d, Level %d", lPrice, nGrade, lMode, nLevel);
				_LOGMSG.LogMsg("CCtrl","SetGradePrice",lPumpNumber,LOG_PUMP,str);	
			}
			
			bIgnoreGrade = TRUE;
			if (!(nFlags & PRICE_END_OF_LIST))
				retCode = CTRL_INVALID_GRADE_NUMBER;
		}
		
		//4.0.24.60 - TD 107311
		if(retCode == CTRL_OK)
		{
			if (_Module.m_server.GetIgnoreNonValidGradesOnPriceChangeFlag())
			{
				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwTmp,(DWORD)0, FALSE);
				if (!dwTmp)
				{
					//4.0.24.70 - TD 110193
					bIgnoreGrade = TRUE;
					if (!(nFlags & PRICE_END_OF_LIST))
					{
						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;
							str.Format("Non Valid Grade - Ignoring !!! Price %ld, Grade %d, Mode %d, Level %d", lPrice, nGrade, lMode, nLevel);
							_LOGMSG.LogMsg("CCtrl","SetGradePrice",lPumpNumber,LOG_PUMP,str);	
						}
						retCode = CTRL_INVALID_GRADE_NUMBER;
					}
				}
			}
		}
		
		if(retCode ==CTRL_OK)
		{
			if (!bIgnoreGrade) //4.0.24.70 - TD 110193
			{
			
				// check if empty date ...
				if( ((szTagTime.sTime.sMonth[0] == ' ')&&(szTagTime.sTime.sMonth[1] == ' ')) ||
					((szTagTime.sTime.sMonth[0] == 0)&&(szTagTime.sTime.sMonth[1] == 0)) )
				{	// no time so set to current time :			
					
					SYSTEMTIME curSysTime;
					GetLocalTime(&curSysTime);					
					SystemTime2Tag(&curSysTime,&szTagTime.sTime);
				}

				_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "ValidEntry",(DWORD)1);
		
				
				// mode == 2 support old version 
				if((lMode == 2 ) || (lMode == FULL_SERVICE_MODE)) 
				{ 
					// full service
					if(nLevel == PRICE_LEVEL_CASH)
					{
						_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCash_FullService", &dwTmp,(DWORD)0,TRUE ,TRUE,0,HOP_DIRECT_LOCAL,TRUE); //4.0.8.503
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Old_PriceCash_FullService",dwTmp,TRUE,0,0,0,TRUE);  //4.0.8.503
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "New_PriceCash_FullService",(DWORD) lPrice,TRUE,0,0,0,TRUE); //4.0.8.503
						_Module.m_server.SetGradePriceToArray(nGrade, lPrice, FULL_CASH);
					}
					else
					{
						_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCredit_FullService", &dwTmp,(DWORD)0,TRUE,TRUE ,0,HOP_DIRECT_LOCAL,TRUE);
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Old_PriceCredit_FullService",dwTmp,TRUE,0,0,0,TRUE);
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "New_PriceCredit_FullService",(DWORD) lPrice,TRUE,0,0,0,TRUE);
						_Module.m_server.SetGradePriceToArray(nGrade, lPrice, FULL_CREDIT);
					}
				}
				else 
				{ // self service
					
					if(nLevel == PRICE_LEVEL_CASH)
					{
						_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCash_SelfService", &dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Old_PriceCash_SelfService",dwTmp, TRUE,0,0,0,TRUE);
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "New_PriceCash_SelfService",(DWORD) lPrice, TRUE,0,0,0,TRUE);
						_Module.m_server.SetGradePriceToArray(nGrade, lPrice, SELF_CASH);
					}
					else 
					{
						_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCredit_SelfService", &dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Old_PriceCredit_SelfService",dwTmp,TRUE,0,0,0,TRUE);
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "New_PriceCredit_SelfService",(DWORD) lPrice,TRUE,0,0,0,TRUE);
						_Module.m_server.SetGradePriceToArray(nGrade, lPrice, SELF_CREDIT);
					}			
				}

				long lLowestPrice = _Module.m_server.GetGradePriceFromArray(nGrade, PRICE_PROTECTION_LOWEST_PRICE);
				if ((lPrice < lLowestPrice) && 
					(lPrice > _Module.m_server.GetGradePriceMinimumLimit())) // QC 470359
				{
					_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Day_Lowest_Price", (DWORD)lPrice, TRUE, 0, 0, 0, TRUE);
					_Module.m_server.SetGradePriceToArray(nGrade, lPrice, PRICE_PROTECTION_LOWEST_PRICE);
				}

				// QC 501964
				if (dwTmp != lPrice)
				{
					_Module.m_server.m_GradeData.SetGradePriceChanged(nGrade, TRUE);
				}

				if (LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("SetGradePrice() Grade %d, Mode %d, Level %d, price %ld, LowestPrice %ld",nGrade, lMode, nLevel, lPrice, lLowestPrice);
					_LOGMSG.LogMsg("CCtrl","SetGradePrice",LOG_PUMP,lPumpNumber,str);
				}
			}

			if(nFlags & PRICE_END_OF_LIST)
			{
				_Module.m_server.m_cPriceChangeFlags.SavedFlags(szTagTime , 0); //4.0.8.501

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("SetGradePrice - End Of Price List !" );
					_LOGMSG.LogMsg("CCtrl","SetGradePrice",LOG_PUMP,lPumpNumber,str);
				}

				//Send alarm to inform start of price change.
				_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_PUMP,SUB_PRICE_CHANGE,PRICE_CHANGE_START,1,ALARM_STARTED,0,0);
			
				_Module.m_server.m_cOLASrv->LoadAllProductSoldOnPump(0); //4.0.23.504 85971 //4.0.26.500 152967

			}
		}
	}

	if (retCode != CTRL_OK)	//4.0.13.502
	{
		long lBatch = _Module.m_server.GetPriceChangeBatchNumber();
		_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_PUMP, SUB_PRICE_CHANGE, PRICE_CHANGE_WAS_NOT_APPLIED, 1, ALARM_STARTED, lBatch, 0);
	}


	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetGradePrice",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);
	
	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::SetGradePrice(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long lPrice, BSTR sTimeValid, short nFlags, long * pVal)
{
	
#ifdef CATCH_CTRL
	__try
	{
#endif

		SetGradePrice__(lPumpNumber,nNozzle,nGrade,lMode,nLevel,lPrice,sTimeValid,nFlags,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetGradePrice"))	 // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","SetGradePrice()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);
		exit(1);
	}
#endif

	return S_OK;
}

void CCtrl::GetGradePrice__(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long * lPrice, BSTR * sDateTimeValid, long * pVal)
{
	
	USES_CONVERSION;
	
	CtrlRetCode retCode = CTRL_OK;

	if((nGrade > MAX_GRADES_99)||(nGrade < 0))//amram 4.0.10.502
		retCode = CTRL_INVALID_GRADE_NUMBER;

	if(retCode == CTRL_OK)
	{

		CString str("Grades\\Grade");
		char sNumber[8];
		memset(sNumber,0,sizeof(sNumber));
		sprintf(sNumber,"%02d",nGrade);
		str += sNumber;

		SZ_TAG_SYSTEMTIME szTagTime;
		szTagTime.sNull = 0;

		_Module.m_server.m_cPriceChangeFlags.GetPriceChangeDate(szTagTime); //4.0.8.501
		
		CComBSTR sChangeTime(sizeof(szTagTime.sTime),(LPCSTR)&szTagTime.sTime); // CID_184527 Out-of-bounds access

		*sDateTimeValid = sChangeTime.Copy(); 

		DWORD dwTmp = 0;

		// mode == 2 support old version 
		if((lMode == 2 ) || (lMode == FULL_SERVICE_MODE)) 
		{ 	// full service
			
			if(nLevel == PRICE_LEVEL_CASH)
			{
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCash_FullService", &dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
			}
			else
			{
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCredit_FullService", &dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
			}
		}
		else
		{ 		
			// self service
			if(nLevel == PRICE_LEVEL_CASH)
			{
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCash_SelfService", &dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
			}
			else 
			{
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCredit_SelfService", &dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
			}			
		}

		*lPrice = dwTmp;

	}
	else if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetGradePrice",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::GetGradePrice(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long * lPrice, BSTR * sDateTimeValid, long * pVal)
{
	
#ifdef CATCH_CTRL
	__try
	{
#endif

		GetGradePrice__(lPumpNumber,nNozzle,nGrade,lMode,nLevel,lPrice,sDateTimeValid,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetGradePrice")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetGradePrice()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif	

	return S_OK;
}

//4.0.2.21
/******************************************************************************
 Description:	Close fuel shift
 Returns:      	Ctrl ret code

 Parameters:   	short nDayOfWeek,  - Set shift to  special days (1-7). 
				short nShiftNumber - Update Shift number   
				short nShiftFlags  - Shift operaion see table #001
				BSTR  sStartTime,  - Date and time to start Future shift YYYYMMDDHHMMSS.
				long *pVal	 	   - CTRL ret code 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			1/1/2003   15:06		Start
******************************************************************************/
void CCtrl::SetShift__(short nDayOfWeek, short nShiftNumber, short nShiftFlags, BSTR sStartTime, long * pVal)
{
	long lRtc = 0;
	long lRetCode = 0;	

	_Module.m_server.SetShift(nDayOfWeek, nShiftNumber, nShiftFlags, sStartTime, &lRetCode , lRtc);

	if(lRetCode)
		if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetShift",
				TRUE,lRetCode,
				(lRtc!=0)? TRUE:FALSE,lRtc);

	if(pVal != NULL)
		*pVal = lRetCode;

}
//4.0.2.21
/******************************************************************************
 Description:	Close fuel shift
 Returns:      	Ctrl ret code

 Parameters:   	short nDayOfWeek,  - Set shift to  special days (1-7). 
				short nShiftNumber - Update Shift number   
				short nShiftFlags  - Shift operaion see table #001
				BSTR  sStartTime,  - Date and time to start Future shift YYYYMMDDHHMMSS.
				long *pVal	 	   - CTRL ret code 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			1/1/2003   15:06		Start
******************************************************************************/
STDMETHODIMP CCtrl::SetShift(short nDayOfWeek, short nShiftNumber, short nShiftFlags, BSTR sStartTime, long * pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		SetShift__(nDayOfWeek,nShiftNumber,nShiftFlags,sStartTime,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetShift")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::SetShift()", LOG_LEVEL_0);	
		exit(1);
	}
#endif	

	return S_OK;
}

void CCtrl::GetShift__(short nDayOfWeek, short nShiftNumber, short * nShiftFlags, BSTR * sStartTime, long * pVal)
{
	
	USES_CONVERSION;

	CtrlRetCode retCode = CTRL_OK;

	if((nDayOfWeek > 7) || (nDayOfWeek < 1))
	{
		retCode = CTRL_INVALID_DAYOFWEEK;
	}
	

	if(retCode == CTRL_OK)
	{

	}
	else if(LOG_LOGIC_ERROR)
		_LOGMSG.LogResult(
			0, // Src
			"ICTRL",
			"GetShift",
			TRUE,retCode,
			FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;


}

STDMETHODIMP CCtrl::GetShift(short nDayOfWeek, short nShiftNumber, short * nShiftFlags, BSTR * sStartTime, long * pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetShift__(nDayOfWeek,nShiftNumber,nShiftFlags,sStartTime,pVal);
#ifdef CATCH_CTRL
	}   
	__except(_HANDLE_EXCEPTION("CCtrl::GetShift")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetShift()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

	return S_OK;
}


void CCtrl::GetPumpTotals__(short nFlags, long *lIndexNumber, long *lPump, short *nNozzle, short *nGrade, short *nTotalsFlags, BSTR *sVolume, BSTR *sValueA, BSTR *sValueB, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *pVal)
{
	
	USES_CONVERSION;

	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	TOTAL_INFO info;
	memset(&info,0,sizeof(TOTAL_INFO));

	switch(nFlags)
	{
		case GET_DIRECT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ,&info);
			break;
		case GET_FIRST_INDEX:				
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ_FIRST,&info);
			break;	
		case GET_NEXT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ_NEXT,&info);
			break;
		default :
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ_FIRST,&info);
			break;
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	
	if(retCode == CTRL_OK)
	{
		*lIndexNumber = info.lIndexNumber;
		*lPump = info.lPump;
		*nNozzle = info.nNozzle;
		*nGrade = info.nGrade;
		*nTotalsFlags = info.nFlags; 

		BYTE sTmp[sizeof(info.sVolume)+2];

		Str2StrNull((BYTE *)info.sVolume,sizeof(info.sVolume),sTmp);

		CComBSTR sVol(sizeof(sTmp),(LPCSTR)sTmp); 
		*sVolume = sVol.Copy();

		Str2StrNull((BYTE *)info.sValueA,sizeof(info.sValueA),sTmp);

		CComBSTR sValA(sizeof(sTmp),(LPCSTR)sTmp); 
		*sValueA = sValA.Copy();

		Str2StrNull((BYTE *)info.sValueB,sizeof(info.sValueB),sTmp);

		CComBSTR sValB(sizeof(sTmp),(LPCSTR)sTmp); 
		*sValueB = sValB.Copy();

		*lActiveReadNumber = (long)info.dwActiveReadNumber;
		*lShiftNumber = (long)info.dwShiftNumber;	

		SZ_TAG_SYSTEMTIME szTagTime;
		szTagTime.sNull = 0;
		SystemTime2Tag (&info.cSysTime, &szTagTime.sTime);

		CComBSTR sTime(sizeof(szTagTime.sTime), (LPCSTR)&szTagTime.sTime); // CID_185433 Out-of-bounds access
		

		*sDateTime = sTime.Copy();
	}
	
	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetPumpTotals",
					TRUE,retCode,
					(lRtc!=0)? TRUE:FALSE,lRtc);
	}

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::GetPumpTotals(short nFlags, long *lIndexNumber, long *lPump, short *nNozzle, short *nGrade, short *nTotalsFlags, BSTR *sVolume, BSTR *sValueA, BSTR *sValueB, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetPumpTotals__(nFlags,lIndexNumber,lPump,nNozzle,nGrade,nTotalsFlags,sVolume,sValueA,sValueB,lActiveReadNumber,lShiftNumber,sDateTime,pVal);
#ifdef CATCH_CTRL
	} 
	__except(_HANDLE_EXCEPTION("CCtrl::GetPumpTotals")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetPumpTotals()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	


	return S_OK;
}


void CCtrl::RemovePumpTotals__(short nFlags, long lRecIndex, long * pVal)
{

	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	TOTAL_INFO info;
	memset(&info,0,sizeof(TOTAL_INFO));
		
	switch(nFlags)
	{
		case REMOVE_FIRST_TILL_INDEX:
			retCode = CTRL_NOT_SUPPORTED;	
			break;
		case REMOVE_DIRECT_INDEX:
		default :
			info.lIndexNumber = lRecIndex;
			
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_DELETE,&info);

			if ((!lRtc) && 
				 (lRecIndex >= MAX_INDEX_TO_TURN_OVER))
			{
				long lRtc1; 
				memset(&info,0,sizeof(TOTAL_INFO));
				lRtc1 = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ_FIRST,&info);

				if (lRtc1)
				{						
					_Module.m_server.SetPumpTotalIndex(1);					
				}
			}

			
			break;		
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	else
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("Pump totals, record %d successfully removed ",lRecIndex);
			_LOGMSG.LogMsg(str);
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"RemovePumpTotals",
				TRUE,retCode,
				(lRtc!=0)? TRUE:FALSE,lRtc);

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::RemovePumpTotals(short nFlags, long lRecIndex, long * pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		RemovePumpTotals__(nFlags,lRecIndex,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::RemovePumpTotals")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::RemovePumpTotals()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	



	return S_OK;
}


void CCtrl::GetTankReading__(short nFlags, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus, short *nReadingFlags, long *lFuelLevel, long *lWaterLevel, long *lTemperature, long *lFuelVolume, long *lWaterVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *pVal)
{
	
	
	USES_CONVERSION;

	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	TANK_READ_INFO info;
	memset(&info,0,sizeof(TANK_READ_INFO));
	
	switch(nFlags)
	{
		case GET_DIRECT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ,&info);
			break;
		case GET_FIRST_INDEX:
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_FIRST,&info);
			break;
		case GET_NEXT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_NEXT,&info);
			break;
		default :
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_FIRST,&info);
			break;
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	
	if(retCode == CTRL_OK)
	{
		*lIndexNumber = info.lIndexNumber;
		*lTank = info.lTank;
		*lProduct = info.lProduct;
		*lStatus = info.lStatus;
		*nReadingFlags = info.nFlags;
		*lFuelLevel = info.lFuelLevel;
		*lWaterLevel = info.lWaterLevel;
		*lTemperature = info.lTemperature;
		*lFuelVolume = info.lFuelVolume;
		*lWaterVolume = info.lWaterVolume;
		*lActiveReadNumber = (long)info.dwActiveReadNumber; 
		*lShiftNumber = (long)info.dwShiftNumber; 

		SZ_TAG_SYSTEMTIME szTagTime;
		szTagTime.sNull = 0;

		SystemTime2Tag (&info.cSysTime, &szTagTime.sTime);
		CComBSTR sTime(sizeof(szTagTime.sTime), (LPCSTR)&szTagTime.sTime); // CID_185691 Out-of-bounds access

		*sDateTime = sTime.Copy();
	}

	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetTankReading",
					TRUE,retCode,
					(lRtc!=0)? TRUE:FALSE,lRtc);
	}

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::GetTankReading(short nFlags, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus, short *nReadingFlags, long *lFuelLevel, long *lWaterLevel, long *lTemperature, long *lFuelVolume, long *lWaterVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetTankReading__(nFlags,lIndexNumber,lTank,lProduct,lStatus,nReadingFlags,lFuelLevel,lWaterLevel,lTemperature,lFuelVolume,lWaterVolume,lActiveReadNumber,lShiftNumber,sDateTime,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetTankReading")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetTankReading()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	


	return S_OK;
}


void CCtrl::RemoveTankReading__(short nFlags, long lRecIndex, long * pVal)
{
	
	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	TANK_READ_INFO info;
	memset(&info,0,sizeof(TANK_READ_INFO));
	
	switch(nFlags)
	{
		case REMOVE_FIRST_TILL_INDEX:
			retCode = CTRL_NOT_SUPPORTED;	
			break;
		case REMOVE_DIRECT_INDEX:
			info.lIndexNumber = lRecIndex;
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_DELETE,&info);		
			break;		

		default :
			info.lIndexNumber = lRecIndex;
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_DELETE,&info);		
			break;		
	}

	if ((!lRtc) && 
		 (lRecIndex >= MAX_INDEX_TO_TURN_OVER) &&  
		 (retCode == CTRL_OK))
	{
		long lRtc1; 
		memset(&info,0,sizeof(TANK_READ_INFO));
		lRtc1 = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_FIRST,&info);

		if (lRtc1)
		{						
			_Module.m_server.SetTankReadingIndex(1);					
		}
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	else
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("Tank Reading, record %d successfully removed ",lRecIndex);
			_LOGMSG.LogMsg(str);
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"RemoveTankReading",
				TRUE,retCode,
				(lRtc!=0)? TRUE:FALSE,lRtc);

	if(pVal != NULL)
		*pVal = retCode;


}

STDMETHODIMP CCtrl::RemoveTankReading(short nFlags, long lRecIndex, long * pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		RemoveTankReading__(nFlags,lRecIndex,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::RemoveTankReading")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::RemoveTankReading()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}



void CCtrl::GetDeliveryReport__(short nFlags, long *lIndexNumber,long *lTank,long *lProduct,long *lStatus,short *nDeliveryFlags,long *lTag,long  *lDeliveredVolume,long *lActiveReadNumber,long *lShiftNumber,BSTR *sDeliveryDateTime,long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	DELIVERY_INFO info;
	memset(&info,0,sizeof(DELIVERY_INFO));
	
	switch(nFlags)
	{
		case GET_DIRECT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ,&info);
			break;
		case GET_FIRST_INDEX:
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ_FIRST,&info);
			break;
		case GET_NEXT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ_NEXT,&info);
			break;
		default :
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ_FIRST,&info);
			break;
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	
	if(retCode == CTRL_OK)
	{
		*lIndexNumber = info.lIndexNumber;
		*lTank = info.lTank;
		*lProduct = info.lProduct;
		*lStatus = info.lStatus;
		*nDeliveryFlags = info.nFlags;
		*lTag = info.lTag;
		*lDeliveredVolume = info.lDeliveredVolume;
		*lActiveReadNumber = (long)info.dwActiveReadNumber; 
		*lShiftNumber = (long)info.dwShiftNumber; 

		SZ_TAG_SYSTEMTIME szTagTime;
		szTagTime.sNull = 0;

		SystemTime2Tag (&info.cDeliveryDateTime, &szTagTime.sTime);
		CComBSTR sTime(sizeof(szTagTime.sTime), (LPCSTR)&szTagTime.sTime); // CID_183840 Out-of-bounds access

		*sDeliveryDateTime = sTime.Copy();
	}
	
	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetDeliveryReport",
					TRUE,retCode,
					(lRtc!=0)? TRUE:FALSE,lRtc);
	}

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::GetDeliveryReport(short nFlags, long *lIndexNumber,long *lTank,long *lProduct,long *lStatus,short *nDeliveryFlags,long *lTag,long  *lDeliveredVolume,long *lActiveReadNumber,long *lShiftNumber,BSTR *sDeliveryDateTime,long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetDeliveryReport__(nFlags,lIndexNumber,lTank,lProduct,lStatus,nDeliveryFlags,lTag,lDeliveredVolume,lActiveReadNumber,lShiftNumber,sDeliveryDateTime,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetDeliveryReport")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetDeliveryReport()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	



	return S_OK;
}


void CCtrl::RemoveDeliveryReport__(short nFlags, long lRecIndex, long * pVal)
{
	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	DELIVERY_INFO info;
	memset(&info,0,sizeof(DELIVERY_INFO));
	
	switch(nFlags)
	{
		case REMOVE_FIRST_TILL_INDEX:
			retCode = CTRL_NOT_SUPPORTED;	
			break;
		case REMOVE_DIRECT_INDEX:
			info.lIndexNumber = lRecIndex;
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_DELETE,&info);		
			break;		
		default :
			info.lIndexNumber = lRecIndex;
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_DELETE,&info);		
			break;		
	}

	if ((!lRtc) && 
		 (lRecIndex >= MAX_INDEX_TO_TURN_OVER) &&  
		 (retCode == CTRL_OK))
	{
		long lRtc1; 
		memset(&info,0,sizeof(FUEL_DELIVERIES_FILE));
		lRtc1 = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ_FIRST,&info);

		if (lRtc1)
		{						
			_Module.m_server.SetDeliveryReporIndex(1);					
		}
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	else
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("Delivery Report, record %d successfully removed ",lRecIndex);
			_LOGMSG.LogMsg(str);
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"RemoveDeliveryReport",
				TRUE,retCode,
				(lRtc!=0)? TRUE:FALSE,lRtc);

	if(pVal != NULL)
		*pVal = retCode;


}

STDMETHODIMP CCtrl::RemoveDeliveryReport(short nFlags, long lRecIndex, long * pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		RemoveDeliveryReport__(nFlags,lRecIndex,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::RemoveDeliveryReport")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::RemoveDeliveryReport()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

void CCtrl::SetRealTimeClock__(BSTR sDateAndTime, long * pVal)
{
	USES_CONVERSION;

	CtrlRetCode retCode = CTRL_OK;
	
	SZ_TAG_SYSTEMTIME	TagTime;
	SYSTEMTIME			sysTime;
	BOOL				bRetVal;

	if (SERVER_STATE_STARTUP)
		retCode = CTRL_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = CTRL_SERVER_NOT_READY;
	
	if(retCode == CTRL_OK)
	{
		int iLen = WideCharToMultiByte(CP_ACP,0,sDateAndTime,-1,(char *)&TagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
		if(iLen == 0 || iLen == 1)
		{
			if(LOG_DETAIL_FLOW_CONTROL)
			{
				_LOGMSG.LogMsg("SetRealTimeClock, Bad sDateAndTime");
			}			
		}
		else
		{
			Tag2SystemTime(&TagTime.sTime,&sysTime);
			bRetVal = SetLocalTime(&sysTime);
			if(bRetVal == FALSE)
			{
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString cMsg;
					cMsg.Format("SetRealTimeClock, Bad Date format:Y:%d,M:%d,D:%d,H:%d,m:%d,s:%d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
					_LOGMSG.LogMsg(cMsg);
				}			
			}
		}
		
		_Module.m_server.SetCommand(CMD_SET_REALTIME_CLOCK,0,0);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			_LOGMSG.LogMsg("CCtrl::SetRealTimeClock()");
		}
	}

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::SetRealTimeClock(BSTR sDateAndTime, long * pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		SetRealTimeClock__(sDateAndTime,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetRealTimeClock")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::SetRealTimeClock()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

void CCtrl::ControlStatus__(short nFlags, long * lTotalIndex, long * lTankReadIndex, long * lDeliveryIndex, long * lActiveShiftNumber, short * nStationStatus, long * pVal)
{

	CtrlRetCode retCode = CTRL_OK;

	
	*lTotalIndex = _Module.m_server.GetTotalIndex();
	*lTankReadIndex = _Module.m_server.GetTankReadIndex();
	*lDeliveryIndex = _Module.m_server.GetDeliveryIndex();
	*lActiveShiftNumber = _Module.m_server.GetFormalShiftNumber();
	*nStationStatus = (short)_Module.m_server.GetLockResExt();
	

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"ControlStatus",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;


}

STDMETHODIMP CCtrl::ControlStatus(short nFlags, long * lTotalIndex, long * lTankReadIndex, long * lDeliveryIndex, long * lActiveShiftNumber, short * nStationStatus, long * pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		ControlStatus__(nFlags,lTotalIndex,lTankReadIndex,lDeliveryIndex,lActiveShiftNumber,nStationStatus,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::ControlStatus")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::ControlStatus()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;	
}


void CCtrl::GetProcessStatus__(long lProcessNumber, long * lProcessStatus, long * lPumpNumberA, long * lPumpProblemA, long * lPumpNumberB, long * lPumpProblemB, long * lTankNumber, long * lTankProblem, long * lFlags, long * pVal)
{
	CtrlRetCode retCode = CTRL_OK;

	// M.L 8/5/98 Lock PumpArray in large scope 
	//-----------------------------------------
	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	*lProcessStatus = 0;
	*lPumpNumberA   = 0;
	*lPumpProblemA  = 0;
	*lPumpNumberB   = 0;
	*lPumpProblemB  = 0;						
	*lTankNumber    = 0;
	*lTankProblem   = 0;
	*lFlags         = 0;

	switch(lProcessNumber)
	{
		case PROCESS_SHIFT_CHANGE:
		{	
			if(_Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags())
			{
				*lProcessStatus =  _Module.m_server.GetLockStatus();
				BOOL bFoundA = FALSE;
			
				for (int p = 0; p < _Module.m_server.GetLastValidPump(); p++)  //4.0.0.79
				{
					long lMisc = _Module.m_server.m_cPumpArray[p].GetStat().GetMisc();
					
					if (lMisc & MISC_SHIFT_CHANGE_FLAGS)
					{
						if(!bFoundA)
						{
							*lPumpNumberA = p+1;
							*lPumpProblemA = lMisc;
							bFoundA = TRUE;
						}
						else  
						{
							*lPumpNumberB= p+1;
							*lPumpProblemB = lMisc;						
							break;
						}
					}
				}
			}
			else
			{
				*lProcessStatus = LOCK_STATUS_IRRELEVANT;
			}
			
			break;
		}

		case PROCESS_PRICE_CHANGE:
		{
			if( _Module.m_server.m_cServerInfo.GetRequiredPriceChangeFlags() || 
				_Module.m_server.m_cServerInfo.GetServerReservation() == PRICE_CHANGE  )
			{
				*lProcessStatus =  _Module.m_server.GetLockStatus();
				BOOL bFoundA = FALSE;
			
				for (int p = 0; p < _Module.m_server.GetLastValidPump(); p++)  //4.0.0.79
				{
					long lMisc =  _Module.m_server.m_cPumpArray[p].GetStat().GetMisc();

					if (lMisc & MISC_PRICE_CHANGE_FLAGS)
					{
						if(!bFoundA)
						{
							*lPumpNumberA = p+1;
							*lPumpProblemA = lMisc;
							bFoundA = TRUE;
						}
						else  
						{
							*lPumpNumberB= p+1;
							*lPumpProblemB = lMisc;						
							break;
						}
					}
				}
			}
			else
			{
				*lProcessStatus = LOCK_STATUS_IRRELEVANT;
			}
			break;
		}

		case PROCESS_MAINTENANCE:
		{
			if(_Module.m_server.m_cServerInfo.GetRequiredMaintenanceProcessFlags() ||
				(_Module.m_server.m_cServerInfo.GetServerReservation() & MAINTENANCE))  //4.0.13.500
			{
				*lProcessStatus =  _Module.m_server.GetLockStatus();
				BOOL bFoundA = FALSE;
			
				for (int p = 0; p < _Module.m_server.GetLastValidPump(); p++)  //4.0.0.79
				{
					long lMisc =  _Module.m_server.m_cPumpArray[p].GetStat().GetMisc();

					if (lMisc & MISC_MAINTENANCE_FLAGS)
					{
						if(!bFoundA)
						{
							*lPumpNumberA = p+1;
							*lPumpProblemA = lMisc;
							bFoundA = TRUE;
						}
						else  
						{
							*lPumpNumberB= p+1;
							*lPumpProblemB = lMisc;						
							break;
						}
					}
				}
			}
			else
			{
				*lProcessStatus = LOCK_STATUS_IRRELEVANT;
			}
			break;
		case PROCESS_DELIVERY_EMULATION:  //4.0.14.501
			{
				*lProcessStatus  = _Module.m_server.m_cServerInfo.GetDeliveryEmulationProcessType();
				*lFlags =  _Module.m_server.m_cServerInfo.GetDeliveryEmulationStatus();
				break;
			}
		}
		case PROCESS_GET_SHIFT_CTRL: //4.0.16.500 - TD 38127
			{
				long lShiftStatus = 0;
				SERVER_INFO sInfo;
	
				// Before getting shift status, check we are not in middle of other process
				_Module.m_server.m_cServerInfo.GetInfo(sInfo);
				
				if ( sInfo.byRequiredMaintenanceProcess || sInfo.byDeliveryEmulationStatus || sInfo.byMemoInProcess || sInfo.lWetStockFlags) 	
				{
					retCode = CTRL_IN_MIDDLE_OF_PROCESS;
				}

				if (!retCode)
				{				
					_Module.m_server.m_cCommonFunction.GetShiftProcessStatus(lFlags, &lShiftStatus);
				
					if (lShiftStatus)
						retCode = CTRL_IN_MIDDLE_OF_SHIFT;
				}

				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("CTRL Shift process: ShiftNumber = %ld, ShiftStatus = %d" , *lFlags, retCode );
					_LOGMSG.LogMsg(str); 
				}
			} 
			break;
		case PROCESS_PUMP_TOTALS:
		case PROCESS_TANK_READING:
		default:
			retCode = CTRL_NOT_SUPPORTED;
			break;
	}	

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetProcessStatus",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::GetProcessStatus(long lProcessNumber, long * lProcessStatus, long * lPumpNumberA, long * lPumpProblemA, long * lPumpNumberB, long * lPumpProblemB, long * lTankNumber, long * lTankProblem, long * lFlags, long * pVal)
{

	
#ifdef CATCH_CTRL
	__try
	{
#endif

		GetProcessStatus__(lProcessNumber,lProcessStatus,lPumpNumberA,lPumpProblemA,lPumpNumberB,lPumpProblemB, lTankNumber,lTankProblem,lFlags,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetProcessStatus")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetProcessStatus()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}


void CCtrl::SetParam__(BSTR sTableName, long lKey1, long lKey2, BSTR sFiledName, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal)
{
	// Control client not allowed to create parameters - 
	// bCreate force to FALSE internaly 

	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;
	DWORD dwLastErr = 0;

	char szTableName[MAX_TABLE_NAME]; 
	char szFieldName[MAX_FIELD_NAME]; 
	char szFieldVal[MAX_FIELD_VAL]; 

	memset(szTableName,0,sizeof(szTableName));
	int iLen = WideCharToMultiByte(CP_ACP,0,sTableName,-1,szTableName,sizeof(szTableName),NULL,NULL);
	if(iLen == 0)
		retCode = CTRL_TABLE_NAME_TOO_LONG;


	memset(szFieldName,0,sizeof(szFieldName));
	iLen = WideCharToMultiByte(CP_ACP,0,sFiledName,-1,szFieldName,sizeof(szFieldName),NULL,NULL);
	if(iLen == 0)
		retCode = CTRL_FIELD_NAME_TOO_LONG;


	memset(szFieldVal,0,sizeof(szFieldVal));
	if(nFiledType == FIELD_SZ)
	{
		iLen = WideCharToMultiByte(CP_ACP,0,sVal,-1,szFieldVal,sizeof(szFieldVal),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			if(dwLastErr == ERROR_INSUFFICIENT_BUFFER)
				retCode = CTRL_DEFAULT_FIELD_VAL_TOO_LONG;
			else 
				retCode = CTRL_LAST_ERROR;
		}
	}

	if(retCode == CTRL_OK)
	{  
		// bCreate forse to FASLE - 
		// creating parameters via CTRL is not allowed 
		long lRtc = _Module.m_server.m_cParam.SetParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,(DWORD)lVal,FALSE);
	 	if(lRtc)
		{
			switch(lRtc)
			{
				case 1: retCode = CTRL_FAILED_TO_OPEN_TABLE;break;		 
				case 2: retCode = CTRL_PARAM_NOT_FOUND;break;		 
			}
			
		}
		else // OK 
		{	
			if(_Module.m_server.GetServerState() >= SERVER_STARTUP)
			{
				CMD_STRUCT_UPDATE_PARAM cmdInfo;
				memset(&cmdInfo,0,sizeof(CMD_STRUCT_UPDATE_PARAM));
				strcpy(cmdInfo.szTableName,szTableName);			
				cmdInfo.lKey1 = lKey1;
				cmdInfo.lKey2 = lKey2;
				strcpy(cmdInfo.szFieldName,szFieldName);
				cmdInfo.nFieldType = nFiledType;
				strcpy(cmdInfo.szFieldVal,szFieldVal);
				cmdInfo.lVal = lVal;
				_Module.m_server.SetCommand(CMD_UPDATE_PARAM,SERVER_POS_NUMBER, 0,&cmdInfo,sizeof(CMD_STRUCT_UPDATE_PARAM));
				_Module.m_server.UpdateParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,lVal);
			}
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetParam",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);
				
	
	if(pVal != NULL)
		*pVal = retCode;


}

STDMETHODIMP CCtrl::SetParam(BSTR sTableName, long lKey1, long lKey2, BSTR sFiledName, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		SetParam__(sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetParam")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::SetParam()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	


	return S_OK;
}

//Eyal 4.0.0.62 This method writes to the console in case of error
//only.
void CCtrl::GetParam__(BSTR sTableName, long lKey1, long lKey2, BSTR sFiledName, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long *pVal)
{

	// Control client not allowed to create parameters - 
	// bCreate force to FALSE internaly 
	
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;
	long lRtc = -1;
	DWORD dwLastErr = 0;
	
	char szTableName[MAX_TABLE_NAME]; 
	char szFieldName[MAX_FIELD_NAME]; 
	char szFieldVal[MAX_FIELD_VAL]; 
	
	memset(szTableName,0,sizeof(szTableName));
	int iLen = WideCharToMultiByte(CP_ACP,0,sTableName,-1,szTableName,sizeof(szTableName),NULL,NULL);
	if(iLen == 0)
		retCode = CTRL_TABLE_NAME_TOO_LONG;


	memset(szFieldName,0,sizeof(szFieldName));
	
	iLen = WideCharToMultiByte(CP_ACP,0,sFiledName,-1,szFieldName,sizeof(szFieldName),NULL,NULL);
	if(iLen == 0)
		retCode = CTRL_FIELD_NAME_TOO_LONG;


	if(nFiledType == FIELD_SZ)
	{
		memset(szFieldVal,0,sizeof(szFieldVal));
		iLen = WideCharToMultiByte(CP_ACP,0,*sVal,-1,szFieldVal,sizeof(szFieldVal),NULL,NULL);
		if(iLen == 0)
		{
			DWORD dwLastErr = GetLastError();
			if(dwLastErr == ERROR_INSUFFICIENT_BUFFER)
				retCode = CTRL_DEFAULT_FIELD_VAL_TOO_LONG;
			else 
				retCode = CTRL_LAST_ERROR;
		}

		SysFreeString( *sVal );	

		if(retCode == CTRL_OK)
		{  
			lRtc =  _Module.m_server.m_cParam.GetParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,sizeof(szFieldVal),NULL,FALSE); 
			CString strRetVal(szFieldVal);
			*sVal = strRetVal.AllocSysString(); 
		}
		else
		{
			CString strRetVal("  ");
			*sVal = strRetVal.AllocSysString(); 
		}

	}
	else // field DW 
	{
		SysFreeString( *sVal );	

		DWORD dwRetVal = (DWORD)*lVal;
		if(retCode == CTRL_OK)	
			lRtc = _Module.m_server.m_cParam.GetParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,NULL,0,&dwRetVal,FALSE); 
		*lVal = (long)dwRetVal;

		CString strRetVal("  ");
		*sVal = strRetVal.AllocSysString(); 
	}

	if(lRtc != -1)
	{
		switch(lRtc)
		{
			case 0 : retCode = CTRL_OK; break;
			case 1 : retCode = CTRL_FAILED_TO_OPEN_TABLE; break;
			case 2 : retCode = CTRL_PARAM_NOT_FOUND_AND_CREATED; break;
			case 3 : retCode = CTRL_PARAM_NOT_FOUND_FAILED_TO_CREATE; break;
			case 4 : retCode = CTRL_PARAM_NOT_FOUND; break;
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetParam",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr,FALSE);


	
	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::GetParam(BSTR sTableName, long lKey1, long lKey2, BSTR sFiledName, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long *pVal)
{


	if( ((sVal == NULL) && (nFiledType == FIELD_SZ)) 
	 || ((lVal == NULL) && (nFiledType == FIELD_DWORD)) )
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"ICtrI::GetParam   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	
		return E_POINTER;
	}

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetParam__(sTableName,lKey1,lKey2,sFiledName,nFiledType,sVal,lVal,bCreate,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetParam")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetParam()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	



	return S_OK;
}

void CCtrl::SetPumpMode__(long lPumpNumber, long lMode, long * pVal)
{
	CtrlRetCode retCode = CTRL_OK;

	if(lPumpNumber !=0)
	{
		if(PUMP_NOT_VALID(lPumpNumber))
			retCode = CTRL_INVALID_PUMP_NUMBER;
		else if(PUMP_NOT_IN_WORKING_STATE(lPumpNumber))
		retCode = CTRL_PUMP_NOT_IN_WORKING_STATE;
	}

	if(retCode == CTRL_OK)
	{
		if(MODE_NOT_VALID(lMode))
			retCode = CTRL_INVALID_PUMP_MODE;
		if (FALSE == _Module.m_server.DBConnectionAvailable())
			retCode = CTRL_FAILED_IO;
	}
	
	if(retCode == CTRL_OK)
	{
		long lIndex = 0 ;
		EnterCriticalSection(&_Module.m_server.m_csModeSchemeArray);

		if(lPumpNumber)
		{
			for(int s = 2; s<= MAX_MODE_SCHEME; s++)
			{
				lIndex = _Module.m_server.GenerateModeSchemeIndex (lPumpNumber , s);				
				_Module.m_server.m_cModeSchemeArray[lIndex].SaveNotValid();
			}

			lIndex = _Module.m_server.GenerateModeSchemeIndex(lPumpNumber , 1); 			
			_Module.m_server.m_cModeSchemeArray[lIndex].SetMode(lMode);
			_Module.m_server.m_cModeSchemeArray[lIndex].SaveDefinition();

		}
		else
		{			
			for (int p = 1; p <= _Module.m_server.GetLastValidPump(); p++)  //4.0.0.79
			{
				for(int s = 2; s<= MAX_MODE_SCHEME; s++)
				{
					lIndex = _Module.m_server.GenerateModeSchemeIndex(p , s); 			
					_Module.m_server.m_cModeSchemeArray[lIndex].SaveNotValid();
				}

				lIndex = _Module.m_server.GenerateModeSchemeIndex(p , 1); 							
				_Module.m_server.m_cModeSchemeArray[lIndex].SetMode(lMode);
				_Module.m_server.m_cModeSchemeArray[lIndex ].SaveDefinition();
			}
		}
		
		LeaveCriticalSection(&_Module.m_server.m_csModeSchemeArray);	
		// Triger start mode process
		_Module.m_server.SetReqiredModeProcess();
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetPumpMode",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;


}

STDMETHODIMP CCtrl::SetPumpMode(long lPumpNumber, long lMode, long * pVal)
{
	
#ifdef CATCH_CTRL
	__try
	{
#endif

		SetPumpMode__(lPumpNumber,lMode,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetPumpMode")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","SetPumpMode()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif	

	return S_OK;	
}



void CCtrl::GetPumpMode__(long lPumpNumber, long * lMode, long * pVal)
{


	CtrlRetCode retCode = CTRL_OK;

	if(PUMP_NOT_VALID(lPumpNumber))
		retCode = CTRL_INVALID_PUMP_NUMBER;

	else if(PUMP_NOT_IN_WORKING_STATE(lPumpNumber))
		retCode = CTRL_PUMP_NOT_IN_WORKING_STATE;
	 
	if(retCode == CTRL_OK)
	{
		// M.L 8/5/98 Lock PumpArray in large scope 
		//-----------------------------------------
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if(lMode)
			*lMode = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetMode();
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetPumpMode",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::GetPumpMode(long lPumpNumber, long * lMode, long * pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetPumpMode__(lPumpNumber,lMode,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetPumpMode")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetPumpMode()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif	

	return S_OK;	
}



void CCtrl::SetPumpModeScheme__(long lPumpNumber, long lScheme, long lMode, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lFlags, long *pVal)
{
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(PUMP_NOT_VALID(lPumpNumber))
		retCode = CTRL_INVALID_PUMP_NUMBER;

	else if(MODE_NOT_VALID(lMode))
		retCode = CTRL_INVALID_PUMP_MODE;

	else if(SCHEME_NOT_VALID(lScheme))
		retCode = CTRL_INVALID_SCHEME_NUMBER;
	
	if(retCode == CTRL_OK)
	{	

		EnterCriticalSection(&_Module.m_server.m_csModeSchemeArray);

		long lIndex = _Module.m_server.GenerateModeSchemeIndex(lPumpNumber , lScheme); 			

		_Module.m_server.m_cModeSchemeArray[lIndex].SetIndex(lPumpNumber,lScheme);
		_Module.m_server.m_cModeSchemeArray[lIndex].SetMode(lMode);
		_Module.m_server.m_cModeSchemeArray[lIndex].SetFlags(lFlags);


		SZ_TAG_SYSTEMTIME szTagTime;
		memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
		int iLen = WideCharToMultiByte(CP_ACP,0,sDateTimeStart,-1,(char *)&szTagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
		if(iLen == 0)
		{			
			SYSTEMTIME curSysTime;
			GetLocalTime(&curSysTime);					
			SystemTime2Tag(&curSysTime,&szTagTime.sTime);
		}
		_Module.m_server.m_cModeSchemeArray[lIndex].SetStartTime(&szTagTime.sTime);

		memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
		iLen = WideCharToMultiByte(CP_ACP,0,sDateTimeEnd,-1,(char *)&szTagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
		if(iLen == 0)
		{			
			SYSTEMTIME curSysTime;
			GetLocalTime(&curSysTime);					
			SystemTime2Tag(&curSysTime,&szTagTime.sTime);
		}	
		_Module.m_server.m_cModeSchemeArray[lIndex].SetEndTime(&szTagTime.sTime);		
		_Module.m_server.m_cModeSchemeArray[lIndex].SaveDefinition();

		LeaveCriticalSection(&_Module.m_server.m_csModeSchemeArray);
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetPumpModeScheme",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;


}

STDMETHODIMP CCtrl::SetPumpModeScheme(long lPumpNumber, long lScheme, long lMode, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lFlags, long *pVal)
{


#ifdef CATCH_CTRL
	__try
	{
#endif

		SetPumpModeScheme__(lPumpNumber,lScheme,lMode,sDateTimeStart,sDateTimeEnd,lFlags,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetPumpModeScheme")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","SetPumpModeScheme()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif	


	return S_OK;
}


void CCtrl::GetPumpModeScheme__(long lPumpNumber, long lScheme, long *plMode, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plFlags, long *pVal)
{	
	
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(PUMP_NOT_VALID(lPumpNumber))
		retCode = CTRL_INVALID_PUMP_NUMBER;

	else if(SCHEME_NOT_VALID(lScheme))
		retCode = CTRL_INVALID_SCHEME_NUMBER;


	if(retCode == CTRL_OK)
	{																																
		long lIndex = _Module.m_server.GenerateModeSchemeIndex(lPumpNumber , lScheme); 			
		*psDateTimeStart = _Module.m_server.m_cModeSchemeArray[lIndex].GetStringStartTime().AllocSysString();
		*psDateTimeEnd   = _Module.m_server.m_cModeSchemeArray[lIndex].GetStringEndTime().AllocSysString();

		if(plMode)
			*plMode = _Module.m_server.m_cModeSchemeArray[lIndex].GetMode();

		if(plFlags)
			*plFlags = _Module.m_server.m_cModeSchemeArray[lIndex].GetFlags();
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetPumpModeScheme",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;


}

STDMETHODIMP CCtrl::GetPumpModeScheme(long lPumpNumber, long lScheme, long *plMode, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plFlags, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetPumpModeScheme__(lPumpNumber,lScheme,plMode,psDateTimeStart,psDateTimeEnd,plFlags,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetPumpModeScheme")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetPumpModeScheme()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);

		exit(1);
	}
#endif	

	
	return S_OK;
}

void CCtrl::SetModeDef__(long lMode, BSTR sName, long lFlags, long *pVal)
{

	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(MODE_NOT_VALID(lMode))
		retCode = CTRL_INVALID_PUMP_MODE;
	
	if(retCode == CTRL_OK)
	{
		char szName[MAX_FIELD_VAL]; 
		memset(szName,0,sizeof(szName));
		int iLen = WideCharToMultiByte(CP_ACP,0,sName,-1,szName,sizeof(szName),NULL,NULL);
		if(iLen == 0)
			retCode = CTRL_MODE_NAME_TOO_LONG;
		else
		{
			CMode mode(lMode);
			mode.SetFlags(lFlags);
			mode.SetName(szName);

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"Name",FIELD_SZ,szName,0,FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAKValid",FIELD_DWORD,NULL,(DWORD)mode.PAKValid(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAKAuto",FIELD_DWORD,NULL,(DWORD)mode.PAKAutoAuth(),FALSE);

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAKAutoReconcile",FIELD_DWORD,NULL,(DWORD)mode.PAKAutoReconcile(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAKAutoLoyaltyOnly",FIELD_DWORD,NULL,(DWORD)mode.PAKAutoAuthLoyaltyOnly(), FALSE);		//4.0.23.508 89149

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAPValid",FIELD_DWORD,NULL,(DWORD)mode.PAPValid(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAPAuto",FIELD_DWORD,NULL,(DWORD)mode.PAPAutoAuth(),FALSE);
			
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PayAttendValid",FIELD_DWORD,NULL,(DWORD)mode.PayAttendValid(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PayAttendAuto",FIELD_DWORD,NULL,(DWORD)mode.PayAttendAutoAuth(),FALSE);

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PrePayValid",FIELD_DWORD,NULL,(DWORD)mode.PrePayValid(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PrePayAuto",FIELD_DWORD,NULL,(DWORD)mode.PrePayAutoAuth(),FALSE);

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PayAVIValid",FIELD_DWORD,NULL,(DWORD)mode.PayAVIValid(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PayAVIAuto",FIELD_DWORD,NULL,(DWORD)mode.PayAVIAutoAuth(),FALSE);

			//4.0.3.60
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"CashAcceptorValid",FIELD_DWORD,NULL,(DWORD)mode.CashAcceptorValid(),FALSE);  //4.0.3.51
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"CashAcceptorAuto",FIELD_DWORD,NULL,(DWORD)mode.CashAcceptorAuth(),FALSE);  //4.0.3.51
			
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAPPostPayValid",FIELD_DWORD,NULL,(DWORD)mode.PAPPostPayValid(),FALSE); //4.0.29.10 - TD 264032

			_Module.m_server.m_cParam.SetParam("Modes", lMode, 0, "PAMValid", FIELD_DWORD, NULL, (DWORD)mode.PAMValid(), FALSE); // CR 444830
			_Module.m_server.m_cParam.SetParam("Modes", lMode, 0, "PAMAuto", FIELD_DWORD, NULL, (DWORD)mode.PAMAutoAuth(), FALSE); // CR 444830

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"ValidEntry",FIELD_DWORD,NULL,(DWORD)mode.ValidMode(),FALSE,_HOST.m_lGroupNum,0,lMode);
			
			_Module.m_server.m_cModeArray[lMode-1] = mode;			
		}
	}


	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetModeDef",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::SetModeDef(long lMode, BSTR sName, long lFlags, long *pVal)
{

	if(sName == NULL)
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"ICTRL::SetModeDef   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	

		return E_POINTER;
	}
	else
	{
#ifdef CATCH_CTRL
		__try
		{
#endif

			SetModeDef__(lMode,sName,lFlags,pVal);
#ifdef CATCH_CTRL
		}
		__except(_HANDLE_EXCEPTION("CCtrl::SetModeDef")) // 4.0.15.460
		{
			// we arrive here on error only !
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::SetModeDef()", LOG_LEVEL_0);	

			exit(1);
		}
#endif		
	}

	
	return S_OK;
}


/******************************************************************************
 Description:	Setting a mode definition interface - with the option to choose the related
                message number which will be presented on the terminal          
 Returns:      	
 Parameters:    long lMode, BSTR sName, long lFlags, long lMsgNumber, long *pVal

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			27/05/2004   15:48		Start
******************************************************************************/

void CCtrl::SetModeDef2__(long lMode, BSTR sRec, long lFlags, long lMsgNumber, long *pVal)
{
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;
	DWORD dwRegMsgNumber = 0;

	if(MODE_NOT_VALID(lMode))
		retCode = CTRL_INVALID_PUMP_MODE;
	
	if(retCode == CTRL_OK)
	{
		char szName[MAX_FIELD_VAL+1];
		TAG_MODEDEF2 tagModeDef;
		
		memset(szName,0,sizeof(szName));
		memset(&tagModeDef,' ',sizeof(TAG_MODEDEF2));

		BYTE byTmpBuffer[sizeof(TAG_MODEDEF2)+2];
		memset(byTmpBuffer,' ',sizeof(byTmpBuffer));
		
		int iLen = Bstr2Buffer(sRec, (void *)byTmpBuffer,sizeof(TAG_MODEDEF2));
		if(iLen == 0)
		{
			DWORD dwLastErr = GetLastError();
			retCode = CTRL_MODE_NAME_TOO_LONG;
		}
		else
		{		
			CMode mode(lMode);
			long lCreditLimit = 0;
			long lCashLimit = 0;

			memcpy(&tagModeDef , byTmpBuffer , sizeof(TAG_MODEDEF2));
			
			lCashLimit = a2l(tagModeDef.sCashLimit , sizeof(tagModeDef.sCashLimit));
			mode.SetLimit(lCashLimit,PRICE_LEVEL_CASH);
			lCreditLimit = a2l(tagModeDef.sCreditLimit , sizeof(tagModeDef.sCreditLimit));
			mode.SetLimit(lCreditLimit,PRICE_LEVEL_CREDIT);
			
			mode.SetFlags(lFlags);

			memcpy(&szName,tagModeDef.sModeName,sizeof(tagModeDef.sModeName));
			
			CString cName(szName);
			cName.TrimRight();
			cName.TrimLeft();//4.0.6.507
			mode.SetName(cName.GetBuffer(0));
			mode.SetMsgNumber(lMsgNumber);

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"Name",FIELD_SZ,cName.GetBuffer(0),0,FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAKValid",FIELD_DWORD,NULL,(DWORD)mode.PAKValid(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAKAuto",FIELD_DWORD,NULL,(DWORD)mode.PAKAutoAuth(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAKAutoReconcile",FIELD_DWORD,NULL,(DWORD)mode.PAKAutoReconcile(),FALSE); //4.0.7.504
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAKAutoLoyaltyOnly",FIELD_DWORD,NULL,(DWORD)mode.PAKAutoAuthLoyaltyOnly(), FALSE);			//4.0.23.508 89149

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAPValid",FIELD_DWORD,NULL,(DWORD)mode.PAPValid(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAPAuto",FIELD_DWORD,NULL,(DWORD)mode.PAPAutoAuth(),FALSE);
			
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PayAttendValid",FIELD_DWORD,NULL,(DWORD)mode.PayAttendValid(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PayAttendAuto",FIELD_DWORD,NULL,(DWORD)mode.PayAttendAutoAuth(),FALSE);

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PrePayValid",FIELD_DWORD,NULL,(DWORD)mode.PrePayValid(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PrePayAuto",FIELD_DWORD,NULL,(DWORD)mode.PrePayAutoAuth(),FALSE);

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PayAVIValid",FIELD_DWORD,NULL,(DWORD)mode.PayAVIValid(),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PayAVIAuto",FIELD_DWORD,NULL,(DWORD)mode.PayAVIAutoAuth(),FALSE);

			_Module.m_server.m_cParam.SetParam("Modes", lMode, 0, "PAMValid", FIELD_DWORD, NULL, (DWORD)mode.PAMValid(), FALSE); // CR 444830
			_Module.m_server.m_cParam.SetParam("Modes", lMode, 0, "PAMAuto", FIELD_DWORD, NULL, (DWORD)mode.PAMAutoAuth(), FALSE); // CR 444830

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"CashAcceptorValid",FIELD_DWORD,NULL,(DWORD)mode.CashAcceptorValid(),FALSE);  
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"CashAcceptorAuto",FIELD_DWORD,NULL,(DWORD)mode.CashAcceptorAuth(),FALSE);  

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"CashLimit",FIELD_DWORD,NULL,(DWORD)mode.GetLimit(PRICE_LEVEL_CASH),FALSE);
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"CreditLimit",FIELD_DWORD,NULL,(DWORD)mode.GetLimit(PRICE_LEVEL_CREDIT),FALSE);

			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"LimitType",FIELD_DWORD,NULL,0,FALSE);
			
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"PAPPostPayValid",FIELD_DWORD,NULL,(DWORD)mode.PAPPostPayValid(),FALSE); //4.0.29.10 - TD 264032
			
			_Module.m_server.m_cParam.SetParam("Modes",lMode,0,"ValidEntry",FIELD_DWORD,NULL,(DWORD)mode.ValidMode(),FALSE,_HOST.m_lGroupNum,0,lMode);
	
			//We'll set the IdlePromptNum parameter if the new one is different than the one in the registry
			_Module.m_server.m_cParam.GetParam("Modes", lMode, 0, "IdlePromptNum", FIELD_DWORD, NULL, 0, &dwRegMsgNumber, TRUE); 
			if((long) dwRegMsgNumber != mode.GetMsgNumber() )
				_Module.m_server.m_cParam.SetParam("Modes", lMode,0,"IdlePromptNum", FIELD_DWORD, NULL, (DWORD)mode.GetMsgNumber(), TRUE, _HOST.m_lGroupNum, 0, lMode); 

			_Module.m_server.m_cModeArray[lMode-1] = mode;	
	

			if(lFlags & MODE_DEF_MANUAL_CHANGE)			//if flag turned on
			{
				_Module.m_server.SetReqiredModeProcess();
			}

		}
	}


	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetModeDef2",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;

}

/******************************************************************************
 Description:	Setting a mode definition inner function - with the option to choose the related
                message number which will be presented on the terminal          
 Returns:      	
 Parameters:    long lMode, BSTR sName, long lFlags, long lMsgNumber, long *pVal

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			27/05/2004   15:46		Start
******************************************************************************/

STDMETHODIMP CCtrl::SetModeDef2(long lMode, BSTR sRec, long lFlags, long lMsgNumber, long *plVal)
{
	if(sRec == NULL)
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"ICTRL::SetModeDef2 Throw COM Exception : E_POINTER", LOG_LEVEL_0);	

		return E_POINTER;
	}
	else
	{
#ifdef CATCH_CTRL
		__try
		{
#endif

			SetModeDef2__(lMode, sRec, lFlags, lMsgNumber, plVal);
#ifdef CATCH_CTRL
		}
		__except(_HANDLE_EXCEPTION("CCtrl::SetModeDef2")) // 4.0.15.460
		{
			// we arrive here on error only !
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::SetModeDef2()", LOG_LEVEL_0);	

			exit(1);
		}
#endif		
	}
	return S_OK;
}


void CCtrl::GetModeDef__(long lMode, BSTR *psName, long *plFlags, long *pVal)
{
	
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(MODE_NOT_VALID(lMode))
		retCode = CTRL_INVALID_PUMP_MODE;

	CMode mode(lMode);

	if(retCode == CTRL_OK)
	{	
		mode.LoadDefinition();			
		*psName = mode.GetName().AllocSysString(); 

		if(plFlags != NULL)
			*plFlags = mode.GetFlags();
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetModeDef",
				TRUE,retCode,
				FALSE,0);
	

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::GetModeDef(long lMode, BSTR *psName, long *plFlags, long *pVal)
{

	if(psName == NULL)
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"ICTRL::GetModeDef   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	

		return E_POINTER;
	}
	else
	{
#ifdef CATCH_CTRL
		__try
		{
#endif

			GetModeDef__(lMode,psName,plFlags,pVal);
#ifdef CATCH_CTRL
		}
		__except(_HANDLE_EXCEPTION("CCtrl::GetModeDef")) // 4.0.15.460
		{
			// we arrive here on error only !
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::GetModeDef()", LOG_LEVEL_0);	

			exit(1);
		}
#endif		

	}

	
	return S_OK;

}


/******************************************************************************
 Description:	Getting a mode definition interface - with the option to get the related
                message number which will be presented on the terminal          
 Returns:      	
 Parameters:    long lMode, BSTR *psName, long *plFlags, long *plMsgNumber, long *plVal

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			27/05/2004   15:51		Start
******************************************************************************/

void CCtrl::GetModeDef2__(long lMode, BSTR *psRec, long *plFlags, long *plMsgNumber, long *plVal)
{	
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(MODE_NOT_VALID(lMode))
		retCode = CTRL_INVALID_PUMP_MODE;

	CMode mode(lMode);

	if(retCode == CTRL_OK)
	{	
		TAG_MODEDEF2 tagModeDef;
		memset(&tagModeDef,' ', sizeof(TAG_MODEDEF2)); 
		mode.LoadDefinition();
		
		mode.GetModeDefTagInfo(&tagModeDef);
		//*psRec = mode.GetName().AllocSysString(); 

		if(psRec != NULL)
		{
			BYTE sTmp[sizeof(TAG_MODEDEF2) + 2];
			Str2StrNull((BYTE *)&tagModeDef, sizeof(TAG_MODEDEF2), sTmp);
			CComBSTR strResult(sizeof(sTmp), (LPCSTR)sTmp); // CID_184855 Out-of-bounds access
			*psRec = strResult.Copy();
		}

		if(plFlags != NULL)
			*plFlags = mode.GetFlags();

		if(plMsgNumber != NULL)
			*plMsgNumber = mode.GetMsgNumber();
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetModeDef2",
				TRUE,retCode,
				FALSE,0);
	

	if(plVal != NULL)
		*plVal = retCode;

}

/******************************************************************************
 Description:	Getting a mode definition inner function - with the option to get the related
                message number which will be presented on the terminal          
 Returns:      	
 Parameters:    long lMode, BSTR *psName, long *plFlags, long *plMsgNumber, long *plVal

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			27/05/2004   15:55		Start
******************************************************************************/

STDMETHODIMP CCtrl::GetModeDef2(long lMode, BSTR *psRec, long *plFlags, long *plMsgNumber, long *plVal)
{
	if(psRec == NULL)
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"ICTRL::GetModeDef2   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	

		return E_POINTER;
	}
	else
	{
#ifdef CATCH_CTRL
		__try
		{
#endif

			GetModeDef2__(lMode, psRec, plFlags, plMsgNumber, plVal);
#ifdef CATCH_CTRL
		}
		__except(_HANDLE_EXCEPTION("CCtrl::GetModeDef2")) // 4.0.15.460
		{
			// we arrive here on error only !
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::GetModeDef2()", LOG_LEVEL_0);	

			exit(1);
		}
#endif		

	}

	return S_OK;
}


//Eyal 4.0.0.62 This method writes to the console in case of error
//only.
void CCtrl::GetParam1__(BSTR sTableAndFieldName, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, long *pVal)
{
	
	// Control client not allowed to create parameters - 
	// bCreate force to FALSE internaly 
	
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;
	CString strEmptyName;
	BOOL bAllocEmpty = FALSE;

	long lRtc = -1;
	char szTableAndFieldName[MAX_TABLE_NAME+MAX_FIELD_NAME]; 
	char szFieldVal[MAX_FIELD_VAL]; 

	memset(szTableAndFieldName,0,sizeof(szTableAndFieldName));
	int iLen = WideCharToMultiByte(CP_ACP,0,sTableAndFieldName,-1,szTableAndFieldName,sizeof(szTableAndFieldName),NULL,NULL);
	if(iLen == 0)
		retCode = CTRL_TABLE_NAME_TOO_LONG;
	else
	{
		strEmptyName  = szTableAndFieldName;
		strEmptyName.TrimLeft();
		if(strEmptyName.IsEmpty())
			retCode = CTRL_PARAM_NOT_FOUND;
	}

	char szTableName[MAX_TABLE_NAME]; 
	char szFieldName[MAX_FIELD_NAME]; 
	memset(szTableName,0,sizeof(szTableName));
	memset(szFieldName,0,sizeof(szFieldName));

	if(retCode == CTRL_OK)
	{
		BYTE *pTmp;
		pTmp = _Module.m_server.GetStringTillDelimiter((BYTE *)szTableAndFieldName,',',(BYTE *)szTableName);
		if(pTmp)
			memcpy(szFieldName,(const void *) pTmp,sizeof(szFieldName));
		else
		{
			pTmp = _Module.m_server.GetStringTillDelimiter((BYTE *)szTableAndFieldName,';',(BYTE *)szTableName);
			if(pTmp)
				memcpy(szFieldName,(const void *) pTmp,sizeof(szFieldName));
			else
				retCode = CTRL_PARAM_NOT_FOUND;
		}
		

		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("TableName %.20s ,Key1 %ld ,Key2 %ld ,FieldName %.20s, FiledType %s",
				szTableName,lKey1,lKey2,szFieldName,(nFiledType)? "FIELD_SZ":"FIELD_DW");
			_LOGMSG.LogReq(  0,
												"ICTRL", 
												"GetParam1", 
												(LPCSTR)str,
												FALSE); 
		}	
	}

	if(nFiledType == FIELD_SZ)
	{
		memset(szFieldVal,0,sizeof(szFieldVal));

		if(retCode == CTRL_OK)
		{  
			lRtc =  _Module.m_server.m_cParam.GetParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,sizeof(szFieldVal),NULL,FALSE); 
			if(lRtc != 0) 
			{
				bAllocEmpty = TRUE;
			}
			else
			{
				CString strRetVal(szFieldVal);			
				if(strRetVal.GetLength())
				{
					if(*sVal) 
						*sVal = strRetVal.AllocSysString(); 
				}
				else
				{
					bAllocEmpty = TRUE;
				}
			}			
		}
		else
		{
			bAllocEmpty = TRUE;
		}
	}
	else // field DW 
	{
		DWORD dwRetVal = 0;
		if(retCode == CTRL_OK)	
			lRtc = _Module.m_server.m_cParam.GetParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,NULL,0,&dwRetVal,FALSE); 
		*lVal = (long)dwRetVal;

		bAllocEmpty = TRUE;
	}

	if(lRtc != -1)
	{
		switch(lRtc)
		{
			case 0 : retCode = CTRL_OK; break;
			case 1 : retCode = CTRL_FAILED_TO_OPEN_TABLE; break;
			case 2 : retCode = CTRL_PARAM_NOT_FOUND_AND_CREATED; break;
			case 3 : retCode = CTRL_PARAM_NOT_FOUND_FAILED_TO_CREATE; break;
			case 4 : retCode = CTRL_PARAM_NOT_FOUND; break;
		}
	}


	if(bAllocEmpty)
	{
		CString strRetVal("  ");
		if(*sVal) 
			*sVal = strRetVal.AllocSysString(); 
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetParam1",
				TRUE,retCode,
				FALSE,0,
				FALSE);

	
	if(pVal != NULL)
		*pVal = retCode;


}

STDMETHODIMP CCtrl::GetParam1(BSTR sTableAndFieldName, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, long *pVal)
{

	if( ((sVal == NULL) && (nFiledType == FIELD_SZ)) 
	 || ((lVal == NULL) && (nFiledType == FIELD_DWORD)) )
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"ICtrI::GetParam1   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	
		return E_POINTER;
	}

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetParam1__(sTableAndFieldName,lKey1,lKey2,nFiledType,sVal,lVal,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetParam1")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetParam1()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	


	return S_OK;
}

void CCtrl::SetParam1__(BSTR sTableAndFieldName, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal)
{
	// Control client not allowed to create parameters - 
	// bCreate force to FALSE internaly 

	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;
	CString strEmptyName;
	DWORD dwLastErr = 0;	

	long lRtc = -1;
	char szTableAndFieldName[MAX_TABLE_NAME+MAX_FIELD_NAME]; 
	char szFieldVal[MAX_FIELD_VAL]; 

	memset(szTableAndFieldName,0,sizeof(szTableAndFieldName));
	int iLen = WideCharToMultiByte(CP_ACP,0,sTableAndFieldName,-1,szTableAndFieldName,sizeof(szTableAndFieldName),NULL,NULL);
	if(iLen == 0)
		retCode = CTRL_TABLE_NAME_TOO_LONG;
	else
	{
		strEmptyName  = szTableAndFieldName;
		strEmptyName.TrimLeft();
		if(strEmptyName.IsEmpty())
			retCode = CTRL_PARAM_NOT_FOUND;
	}


	char szTableName[MAX_TABLE_NAME]; 
	char szFieldName[MAX_FIELD_NAME]; 
	memset(szTableName,0,sizeof(szTableName));
	memset(szFieldName,0,sizeof(szFieldName));

	if(retCode == CTRL_OK)
	{
		BYTE *pTmp;
		pTmp = _Module.m_server.GetStringTillDelimiter((BYTE *)szTableAndFieldName,',',(BYTE *)szTableName);
		if(pTmp)
			memcpy(szFieldName,(const void *) pTmp,sizeof(szFieldName));
		else
		{
			pTmp = _Module.m_server.GetStringTillDelimiter((BYTE *)szTableAndFieldName,';',(BYTE *)szTableName);
			if(pTmp)
				memcpy(szFieldName,(const void *) pTmp,sizeof(szFieldName));
			else
				retCode = CTRL_PARAM_NOT_FOUND;
		}
		

		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("TableName %.20s ,Key1 %ld ,Key2 %ld ,FieldName %.20s, FiledType %s",
				szTableName,lKey1,lKey2,szFieldName,(nFiledType)? "FIELD_SZ":"FIELD_DW");
			_LOGMSG.LogReq(  0,
												"ICTRL", 
												"SetParam1", 
												(LPCSTR)str,
												FALSE); 
		}	
	}

	
	memset(szFieldVal,0,sizeof(szFieldVal));
	if(nFiledType == FIELD_SZ)
	{
		iLen = WideCharToMultiByte(CP_ACP,0,sVal,-1,szFieldVal,sizeof(szFieldVal),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			if(dwLastErr == ERROR_INSUFFICIENT_BUFFER)
				retCode = CTRL_DEFAULT_FIELD_VAL_TOO_LONG;
			else 
				retCode = CTRL_LAST_ERROR;
		}
	}


#if SPEEDPASS_SIMULATOR

	//Get the pump number.
	DWORD dwRFIDPump;
	_Module.m_server.m_cParam.LoadParam("Allied","RFID_Pump", &dwRFIDPump,0,FALSE,FALSE);
	long p = (( dwRFIDPump > 0 && dwRFIDPump <= 32) ? dwRFIDPump - 1 : 0);

	CPumpStatus	pumpStat = _Module.m_server.m_cPumpArray[p].GetStat();
	
	if ( !strcmp(szFieldName,"RFID_Msg") )
	{
		switch (lVal)
		{
			case PUMP_MSG_TAG_CHALLANGE_FAIL:
			case PUMP_MSG_TAG_GONE:
			case PUMP_MSG_TAG_NOT_FOUND:
			case PUMP_MSG_TAG_CHANGED:
			case PUMP_MSG_CAR_TAG_READ:
			case PUMP_MSG_HAND_TAG_READ:
			case PUMP_MSG_TAG_ERROR:
			case PUMP_MSG_STOCK_PRE_PAY: //2.9.1.3
			case PUMP_MSG_TAG_SALE_CANCELED://3.0.2.3
			case PUMP_MSG_TAG_SALE_DECLINED://3.0.2.3
				pumpStat.SetMessage(lVal);

				break;

			case PUMP_MSG_TAG_READ:
				//Determine if the tag is hand or car
				if ( pumpStat.GetResExt() == RES_EXT_RFID_CT )
					pumpStat.SetMessage(PUMP_MSG_CAR_TAG_READ);
				else if ( pumpStat.GetResExt() == RES_EXT_RFID_HT )
					pumpStat.SetMessage(PUMP_MSG_HAND_TAG_READ);
				else
					pumpStat.SetMessage(lVal);
			
				break;			

			case PUMP_NO_MSG:
				pumpStat.SetMessage(lVal);
			default:;
		}
	}

	if ( !strcmp(szFieldName,"Warning") )
	{
		switch (lVal)
		{
			case PUMP_DEVICE_NO_WARNING:
			case PUMP_DEVICE_WARNING_NON_SPECIFIC:
			case PUMP_DEVICE_WARNING_PAPER_LOW:
			case PUMP_DEVICE_WARNING_OUT_OF_PAPER:
			case PUMP_DEVICE_WARNING_PAPER_JAM:
			case PUMP_DEVICE_WARNING_CASH_REJECTED:
			case PUMP_DEVICE_WARNING_CASH_JAMED:		
			case PUMP_DEVICE_WARNING_CASH_CASSETTE_FULL:			
			case PUMP_DEVICE_WARNING_CASH_CASSETTE_REMOVED: 								
			case PUMP_DEVICE_WARNING_CARD_ENCRYPTION:
			case PUMP_DEVICE_WARNING_OFFLINE:
				pumpStat.SetWarning(lVal);

				break;
		}

	_Module.m_server.m_cPumpArray[p].SetStat(pumpStat);
}

#endif


	
	if(retCode == CTRL_OK)
	{  
		// bCreate forse to FASLE - 
		// creating parameters via CTRL is not allowed 
		long lRtc = _Module.m_server.m_cParam.SetParam( szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,(DWORD)lVal,FALSE); 			
	 	if(lRtc)
		{
			switch(lRtc)
			{
				case 1: retCode = CTRL_FAILED_TO_OPEN_TABLE;break;		 
				case 2: retCode = CTRL_PARAM_NOT_FOUND;break;		 
			}
			
		}
		else // OK 
		{	
			if(_Module.m_server.GetServerState() >= SERVER_STARTUP)
			{
				CMD_STRUCT_UPDATE_PARAM cmdInfo;
				memset(&cmdInfo,0,sizeof(CMD_STRUCT_UPDATE_PARAM));
				strcpy(cmdInfo.szTableName,szTableName);			
				cmdInfo.lKey1 = lKey1;
				cmdInfo.lKey2 = lKey2;
				strcpy(cmdInfo.szFieldName,szFieldName);			
				cmdInfo.nFieldType = nFiledType;
				strcpy(cmdInfo.szFieldVal,szFieldVal);
				cmdInfo.lVal = lVal;
				_Module.m_server.SetCommand(CMD_UPDATE_PARAM,SERVER_POS_NUMBER, 0,&cmdInfo,sizeof(CMD_STRUCT_UPDATE_PARAM));
				_Module.m_server.UpdateParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,lVal);
			}
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetParam1",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);
	
	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::SetParam1(BSTR sTableAndFieldName, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		SetParam1__(sTableAndFieldName,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetParam1")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::SetParam1()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	


	return S_OK;
}

void CCtrl::SetParam2__(BSTR sTableFieldAndVal, long lKey1, long lKey2, short nFiledType, long lVal, BOOL bCreate, long *pVal)
{
	// Control client not allowed to create parameters - 
	// bCreate force to FALSE internaly 

	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;
	CString strEmptyName;
	CString strReg;
	long lLangKey = -1;
	DWORD dwFreeEntry = 0;
	DWORD dwLangID = 0;
	int nLangIDRetVal = 0; 
	BOOL bEqualStr1 = FALSE;
	BOOL bEqualStr2 = FALSE;
	long lSetParamRetCode = -1;
	long lRtc = -1;
	char szTableFieldAndVal[MAX_TABLE_NAME+MAX_FIELD_NAME+MAX_FIELD_VAL]; 

	memset(szTableFieldAndVal,0,sizeof(szTableFieldAndVal));
	int iLen = WideCharToMultiByte(CP_ACP,0,sTableFieldAndVal,-1,szTableFieldAndVal,sizeof(szTableFieldAndVal),NULL,NULL);
	if(iLen == 0)
		retCode = CTRL_TABLE_NAME_TOO_LONG;
	else
	{
		strEmptyName  = szTableFieldAndVal;
		strEmptyName.TrimLeft();
		if(strEmptyName.IsEmpty())
			retCode = CTRL_PARAM_NOT_FOUND;
	}


	char szTableName[MAX_TABLE_NAME]; 
	char szFieldName[MAX_FIELD_NAME]; 
	char szFieldVal[MAX_FIELD_VAL]; 

	memset(szTableName,0,sizeof(szTableName));
	memset(szFieldName,0,sizeof(szFieldName));
	memset(szFieldVal,0,sizeof(szFieldVal));

	if(retCode == CTRL_OK)
	{
		BYTE *pTmp;

		BOOL bDelimiterFound =
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)szTableFieldAndVal,
															(BYTE *)szTableName,
															sizeof(szTableName), 
															&pTmp);
		if(bDelimiterFound) 
		{
			bDelimiterFound = 	
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
															(BYTE *)szFieldName,
															sizeof(szFieldName), 
															&pTmp);
		}
		else
		{
			retCode = CTRL_PARAM_NOT_FOUND;
		}

		if(bDelimiterFound) 
			bDelimiterFound = 	
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
															(BYTE *)szFieldVal,
															sizeof(szFieldVal), 
															&pTmp);

		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("TableName %.20s ,Key1 %ld ,Key2 %ld ,FieldName %.20s, FiledType %s, FieldVal %.30s",
				szTableName,lKey1,lKey2,szFieldName,(nFiledType)? "FIELD_SZ":"FIELD_DW",szFieldVal);
			_LOGMSG.LogReq(  0,
												"ICTRL", 
												"SetParam2", 
												(LPCSTR)str,
												FALSE); 
		}	
	}
	
	
	if(retCode == CTRL_OK)
	{  
		//Messages/Promotion and Messages/Idle will be now transferred to Languages/Language01
		if(strcmp(szFieldName, "Promotion") == 0)
			bEqualStr1 = TRUE;
		if(strcmp(szFieldName, "Idle") == 0)
			bEqualStr2 = TRUE;

		if( (strcmp(szTableName, "Messages") == 0) && (bEqualStr1 || bEqualStr2 ) )
		{
			strcpy(szTableName, "Languages");
			lKey1 = 1;
			lKey2 = 0;
			lSetParamRetCode = _Module.m_server.m_cParam.SetParam( szTableName, lKey1, lKey2, szFieldName, nFiledType, szFieldVal, (DWORD)lVal, TRUE); 			
		}
		else if(strcmp(szTableName, "Languages") == 0)
		{
			if(lKey2 != 0) //Language parameters are being set through a direct entrance
			{
				if( (lKey2 >= 1) && (lKey2 <= MAX_LANGUAGES) )	
				{
					lKey1 = lKey2;
					lKey2 = 0;
					lSetParamRetCode = _Module.m_server.m_cParam.SetParam( szTableName, lKey1, lKey2, szFieldName, nFiledType, szFieldVal, (DWORD)lVal, TRUE); 			
				}
				else
				{
					_LOGMSG.LogMsg("SetParam2, Trying to directly set a language  - out of range ");
					retCode = CTRL_PARAM_NOT_FOUND ;
				}
			}
			else //Finding the entrance through the LangID
			{
				for(int i = 1; i <= MAX_LANGUAGES; i++)
				{
					strReg.Format("Languages\\Language%02d" , i);
					
					nLangIDRetVal = _Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "LangID", &dwLangID, (DWORD) 0, FALSE);
					if( (nLangIDRetVal >= 1) &&(nLangIDRetVal <= 4) ) //Key not found in registry - there's a free entry in that spot
						dwFreeEntry = i;

					else if( dwLangID == (DWORD) lKey1)
					{						
						lLangKey = i;
						break;
					}
				}
				if(-1 == lLangKey)
				{
					if(0 == dwFreeEntry)
					{
						_LOGMSG.LogMsg("SetParam2, Trying to  set a language by a LangID - the language wasn't found and no vacant place was found ");
						retCode = CTRL_PARAM_NOT_FOUND ;
					}
					else
					{
						//We have a free entry, so we'll put the new language into it
						
						dwLangID = lKey1;
						lKey1 = dwFreeEntry;
						lKey2 = 0;
						//First, setting the new language with the requested new LangID and a Valid Entry
						lSetParamRetCode = _Module.m_server.m_cParam.SetParam( szTableName, lKey1, lKey2, "LangID", 0, "", dwLangID, TRUE); 			
						lSetParamRetCode = _Module.m_server.m_cParam.SetParam( szTableName, lKey1, lKey2, "ValidEntry", 0, "", 1, TRUE); 			
						lSetParamRetCode = _Module.m_server.m_cParam.SetParam( szTableName, lKey1, lKey2, "MaxMessageLineLen", 0, "", MAX_MSG_LINE_LEN, TRUE); 			

						//Then, setting the requested parameter
						lSetParamRetCode = _Module.m_server.m_cParam.SetParam( szTableName, lKey1, lKey2, szFieldName, nFiledType, szFieldVal, (DWORD)lVal, TRUE); 			
					}
				}
				else
				{
					//The language was found and  we can get its parameters

					lKey1 = lLangKey;
					lKey2 = 0;
					lSetParamRetCode = _Module.m_server.m_cParam.SetParam( szTableName, lKey1, lKey2, szFieldName, nFiledType, szFieldVal, (DWORD)lVal, TRUE); 			
				}
			}
		}
		else
		{
			// bCreate force to FALSE - 
			// creating parameters via CTRL is not allowed (in the default cases)
			lSetParamRetCode = _Module.m_server.m_cParam.SetParam( szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,(DWORD)lVal,FALSE); 			
		}
	 	if(lSetParamRetCode)
		{
			switch(lRtc)
			{
				case 1: retCode = CTRL_FAILED_TO_OPEN_TABLE;break;		 
				case 2: retCode = CTRL_PARAM_NOT_FOUND;break;		 
			}
			
		}
		else // OK 
		{	
			if(_Module.m_server.GetServerState() >= SERVER_STARTUP)
			{
				CMD_STRUCT_UPDATE_PARAM cmdInfo;
				memset(&cmdInfo,0,sizeof(CMD_STRUCT_UPDATE_PARAM));
				strcpy(cmdInfo.szTableName,szTableName);			
				cmdInfo.lKey1 = lKey1;
				cmdInfo.lKey2 = lKey2;
				strcpy(cmdInfo.szFieldName,szFieldName);			
				cmdInfo.nFieldType = nFiledType;
				strcpy(cmdInfo.szFieldVal,szFieldVal);
				cmdInfo.lVal = lVal;
				_Module.m_server.SetCommand(CMD_UPDATE_PARAM,SERVER_POS_NUMBER, 0,&cmdInfo,sizeof(CMD_STRUCT_UPDATE_PARAM));
				_Module.m_server.UpdateParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,lVal);
			}
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetParam2",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::SetParam2(BSTR sTableFieldAndVal, long lKey1, long lKey2, short nFiledType, long lVal, BOOL bCreate, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		SetParam2__(sTableFieldAndVal,lKey1,lKey2,nFiledType,lVal,bCreate,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetParam2")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::SetParam2()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

//Eyal 4.0.0.62 This method writes to the console in case of error
//only.
void CCtrl::GetParam2__(BSTR *sTableFieldAndVal, long lKey1, long lKey2, short nFiledType, long *lVal, long *pVal)
{	
	// Control client not allowed to create parameters - 
	// bCreate force to FALSE internaly 
	
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;
	CString strEmptyName;
	BOOL bAllocEmpty = FALSE;
	CString strReg;
	long lLangKey = -1;
	DWORD dwValidEntry = 0;
	DWORD dwLangID = 0;
	int nLangIDRetVal = 0; 

	long lRtc = -1;
	char szTableFieldAndVal[MAX_TABLE_NAME+MAX_FIELD_NAME+MAX_FIELD_VAL]; 
	memset(szTableFieldAndVal,0,sizeof(szTableFieldAndVal));
	int iLen = WideCharToMultiByte(CP_ACP,0,*sTableFieldAndVal,-1,szTableFieldAndVal,sizeof(szTableFieldAndVal),NULL,NULL);
	if(iLen == 0)
		retCode = CTRL_TABLE_NAME_TOO_LONG;
	else
	{
		strEmptyName  = szTableFieldAndVal;
		strEmptyName.TrimLeft();
		if(strEmptyName.IsEmpty())
			retCode = CTRL_PARAM_NOT_FOUND;
	}

	SysFreeString(*sTableFieldAndVal);


	char szTableName[MAX_TABLE_NAME]; 
	char szFieldName[MAX_FIELD_NAME]; 
	char szFieldVal[MAX_FIELD_VAL]; 

	memset(szTableName,0,sizeof(szTableName));
	memset(szFieldName,0,sizeof(szFieldName));
	memset(szFieldVal,0,sizeof(szFieldVal));


	if(retCode == CTRL_OK)
	{
		BYTE *pTmp;

		BOOL bDelimiterFound =
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)szTableFieldAndVal,
															(BYTE *)szTableName,
															sizeof(szTableName), 
															&pTmp);
		if(bDelimiterFound) 
		{
			bDelimiterFound = 	
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
															(BYTE *)szFieldName,
															sizeof(szFieldName), 
															&pTmp);
		}
		else
		{
			retCode = CTRL_PARAM_NOT_FOUND;
		}

		if(bDelimiterFound) 
			bDelimiterFound = 	
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
															(BYTE *)szFieldVal,
															sizeof(szFieldVal), 
															&pTmp);
	}


	if(nFiledType == FIELD_SZ)
	{
		if(retCode == CTRL_OK)
		{  
			//Messages/Promotion and Messages/Idle will be now transferred to Languages/Language01
			if( (strcmp(szTableName, "Messages") == 0) && ( (strcmp(szFieldName, "Promotion") == 0) || (strcmp(szFieldName, "Idle") == 0) ) )
			{
				strcpy(szTableName, "Languages");
				lKey1 = 1;
				lKey2 = 0;
				lRtc = _Module.m_server.m_cParam.GetParam( szTableName, lKey1, lKey2, szFieldName, nFiledType, szFieldVal,sizeof(szFieldVal), NULL ,FALSE); 			
			}
			else if(strcmp(szTableName, "Languages") == 0)
			{
				if(lKey2 != 0) //Language parameters are being set through a direct entrance
				{
					if( (lKey2 >= 1) && (lKey2 <= MAX_LANGUAGES) )	//Making sure the entrance is within the legal boundaries
					{
						lKey1 = lKey2;
						lKey2 = 0;
						lRtc = _Module.m_server.m_cParam.GetParam( szTableName, lKey1, lKey2, szFieldName, nFiledType, szFieldVal, sizeof(szFieldVal), NULL, FALSE); 			
					}
					else
					{
						_LOGMSG.LogMsg("GetParam2, Trying to directly get a language  - out of range ");
						retCode = CTRL_PARAM_NOT_FOUND ;
					}
				}
				else //Finding the entrance through the LangID
				{
					for(int i = 1; i <= MAX_LANGUAGES; i++)
					{
						strReg.Format("Languages\\Language%02d" , i);
						
					    _Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "LangID", &dwLangID, (DWORD) 0, FALSE);

						if( dwLangID == (DWORD) lKey1)
						{
							_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "ValidEntry", &dwValidEntry, (DWORD) 0, FALSE);
							if(1 == dwValidEntry)
							{
								//If the language was found and the entry is valid - it exists and we can
								//get its parameters
								lLangKey = i;
								break;
							}
						}
					}
					if(-1 == lLangKey)
					{
						_LOGMSG.LogMsg("GetParam2, Trying to get a language by a LangID which doesn't exist  ");
						retCode = CTRL_PARAM_NOT_FOUND ;
					}
					else
					{
						lKey1 = lLangKey;
						lKey2 = 0;
						lRtc = _Module.m_server.m_cParam.GetParam( szTableName, lKey1, lKey2, szFieldName, nFiledType, szFieldVal,sizeof(szFieldVal),NULL,FALSE); 			
					}
				}
			}
			else
			{
				lRtc =  _Module.m_server.m_cParam.GetParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,sizeof(szFieldVal),NULL,FALSE); 
			}
			if(lRtc != 0) 
			{
				bAllocEmpty = TRUE;
			}
			else
			{
				CString strRetVal(szFieldVal);			
				if(strRetVal.GetLength())
				{
					*sTableFieldAndVal = strRetVal.AllocSysString(); 
				}
				else
				{
					bAllocEmpty = TRUE;
				}
			}
		}
		else
		{
			bAllocEmpty = TRUE;
		}
	}
	else // field DW 
	{
		DWORD dwRetVal = 0;
		if(retCode == CTRL_OK)
		{
			if(strcmp(szTableName, "Languages") == 0)
			{
				if(lKey2 != 0) //Direct Entrance
				{
					if( (lKey2 >= 1) && (lKey2 <= MAX_LANGUAGES) )	
					{
						lKey1 = lKey2;
						lKey2 = 0;
						lRtc = _Module.m_server.m_cParam.GetParam( szTableName, lKey1, lKey2, szFieldName, nFiledType, NULL, 0, &dwRetVal, FALSE); 			
					}
					else
					{
						_LOGMSG.LogMsg("GetParam2, Trying to directly get a language  - out of range ");
						retCode = CTRL_PARAM_NOT_FOUND ;
					}
				}
				else //Finding the entrance through the LangID
				{
					for(int i = 1; i <= MAX_LANGUAGES; i++)
					{
						strReg.Format("Languages\\Language%02d" , i);
						
						_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "LangID", &dwLangID, (DWORD) 0, FALSE);
						if( dwLangID == (DWORD) lKey1)
						{
							_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "ValidEntry", &dwValidEntry, (DWORD) 0, FALSE);
							if(1 == dwValidEntry)
							{
								lLangKey = i;
								break;
							}
						}
					}
					if(-1 == lLangKey)
					{
						_LOGMSG.LogMsg("GetParam2, Trying to get a language by a LangID which doesn't exist ");
						retCode = CTRL_PARAM_NOT_FOUND ;
					}
					else
					{
						lKey1 = lLangKey;
						lKey2 = 0;
						lRtc = _Module.m_server.m_cParam.GetParam( szTableName, lKey1, lKey2, szFieldName, nFiledType, NULL, 0, &dwRetVal, FALSE); 			
					}
				}
			}
			else
			{
				lRtc = _Module.m_server.m_cParam.GetParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,NULL,0,&dwRetVal,FALSE); 
			}
		}

		*lVal = (long) dwRetVal;

		bAllocEmpty = TRUE;
	}

	if(lRtc != -1)
	{
		switch(lRtc)
		{
			case 0 : retCode = CTRL_OK; break;
			case 1 : retCode = CTRL_FAILED_TO_OPEN_TABLE; break;
			case 2 : retCode = CTRL_PARAM_NOT_FOUND_AND_CREATED; break;
			case 3 : retCode = CTRL_PARAM_NOT_FOUND_FAILED_TO_CREATE; break;
			case 4 : retCode = CTRL_PARAM_NOT_FOUND; break;
		}
	}


	if(bAllocEmpty)
	{
		CString strRetVal("  ");
		*sTableFieldAndVal = strRetVal.AllocSysString(); 
	}

	if(retCode)
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("ICTRL::GetParam2 TableName %.20s ,Key1 %ld ,Key2 %ld ,FieldName %.20s, FiledType %s, FieldVal %.30s, Ret Code=%d",
				szTableName,lKey1,lKey2,szFieldName,(nFiledType)? "FIELD_SZ":"FIELD_DW",szFieldVal , retCode);
			_LOGMSG.LogReq(  0,
												"ICTRL", 
												"GetParam2", 
												(LPCSTR)str,
												FALSE); 
		}	
	}

	
	if(pVal != NULL)
		*pVal = retCode;


}

STDMETHODIMP CCtrl::GetParam2(BSTR *sTableFieldAndVal, long lKey1, long lKey2, short nFiledType, long *lVal, long *pVal)
{


	if( (sTableFieldAndVal == NULL) || 
		((lVal == NULL) && (nFiledType == FIELD_DWORD)) )
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"ICtrI::GetParam2   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	
		return E_POINTER;
	}

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetParam2__(sTableFieldAndVal,lKey1,lKey2,nFiledType,lVal,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetParam2")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetParam2()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	



	return S_OK;
}

void CCtrl::GetRecord__(BSTR *sTableAndVal, long lKey1, long lKey2, long *pVal)
{

	// Control client not allowed to create parameters - 
	// bCreate force to FALSE internaly 
	
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;
	CString strEmptyName;
	BOOL bAllocEmpty = FALSE;

	long lRtc = -1;
	char szTableAndVal[MAX_RECORD_LEN]; 
	memset(szTableAndVal,0,sizeof(szTableAndVal));
	int iLen = WideCharToMultiByte(CP_ACP,0,*sTableAndVal,-1,szTableAndVal,sizeof(szTableAndVal),NULL,NULL);
	if(iLen == 0)
		retCode = CTRL_TABLE_NAME_TOO_LONG;
	else
	{
		strEmptyName  = szTableAndVal;
		strEmptyName.TrimLeft();
		if(strEmptyName.IsEmpty())
			retCode = CTRL_PARAM_NOT_FOUND;
	}

	SysFreeString(*sTableAndVal);


	char szTableName[MAX_TABLE_NAME]; 
	char szRecord[MAX_RECORD_LEN];   //YKL 03/01/00: was MAX_FIELD_NAME
	
	memset(szTableName,0,sizeof(szTableName));
	memset(szRecord,0,sizeof(szRecord));
	

	if(retCode == CTRL_OK)
	{
		BYTE *pTmp;

		BOOL bDelimiterFound =
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)szTableAndVal,
															(BYTE *)szTableName,
															sizeof(szTableName), 
															&pTmp);
	}


	if(retCode == CTRL_OK)
	{  
		long lWrittenLen = 0;
		long lNumberOfFields = 0; 
		lRtc =  _Module.m_server.m_cParam.GetRecord(szTableName, lKey1,lKey2, szRecord, sizeof(szRecord), &lWrittenLen, &lNumberOfFields);
		
		if(lRtc != 0) 
		{
			bAllocEmpty = TRUE;
		}
		else
		{

			if(LOG_FCC_FLOW_CONTROL)
			{
				CString str;
				str.Format("TableName %.20s ,Key1 %ld ,Key2 %ld, Record[%ld] (%ld fields) %.600s ",szTableName,lKey1,lKey2,lWrittenLen,lNumberOfFields,szRecord );
				_LOGMSG.LogReq(  0,
													"ICTRL", 
													"GetRecord", 
													(LPCSTR)str,
													FALSE); 
			}	

			CString strRetVal(szRecord);			
			if(strRetVal.GetLength())
					*sTableAndVal = strRetVal.AllocSysString(); 
			else
			{
				bAllocEmpty = TRUE;
			}
		}			
	}
	else
	{
		bAllocEmpty = TRUE;
	}


	if(lRtc != -1)
	{
		switch(lRtc)
		{
			case 0 : retCode = CTRL_OK; break;
			case 1 : retCode = CTRL_FAILED_TO_OPEN_TABLE; break;
			case 2 : retCode = CTRL_PARAM_NOT_FOUND_AND_CREATED; break;
			case 3 : retCode = CTRL_PARAM_NOT_FOUND_FAILED_TO_CREATE; break;
			case 4 : retCode = CTRL_PARAM_NOT_FOUND; break;
		}
	}


	if(bAllocEmpty)
	{
		CString strRetVal("  ");
		*sTableAndVal = strRetVal.AllocSysString(); 
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetRecord",
				TRUE,retCode,
				FALSE,0,FALSE);

	
	if(pVal != NULL)
		*pVal = retCode;


}


STDMETHODIMP CCtrl::GetRecord(BSTR *sTableAndVal, long lKey1, long lKey2, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetRecord__(sTableAndVal,lKey1,lKey2,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetRecord")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetRecord()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

void CCtrl::SetRecord__(BSTR sTableAndVal, long lKey1, long lKey2, long *pVal)
{
	
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;
	CString strEmptyName;
	

	long lRtc = -1;
	char szTableAndVal[MAX_RECORD_LEN]; 

	memset(szTableAndVal,0,sizeof(szTableAndVal));
	int iLen = WideCharToMultiByte(CP_ACP,0,sTableAndVal,-1,szTableAndVal,sizeof(szTableAndVal),NULL,NULL);
	if(iLen == 0)
		retCode = CTRL_TABLE_NAME_TOO_LONG;
	else
	{
		strEmptyName  = szTableAndVal;
		strEmptyName.TrimLeft();
		if(strEmptyName.IsEmpty())
			retCode = CTRL_PARAM_NOT_FOUND;
	}

	char szTableName[MAX_TABLE_NAME]; 
	char szRecord[MAX_RECORD_LEN]; 

	memset(szTableName,0,sizeof(szTableName));
	memset(szRecord,0,sizeof(szRecord));

	if(retCode == CTRL_OK)
	{
		BYTE *pTmp;

		BOOL bDelimiterFound =
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)szTableAndVal,
															(BYTE *)szTableName,
															sizeof(szTableName), 
															&pTmp);


		if(bDelimiterFound && strlen((char *)pTmp)<MAX_RECORD_LEN-1) 
         strcpy (szRecord, (char *)pTmp);
		else
		{
			retCode = CTRL_PARAM_NOT_FOUND;
		}


		if(LOG_FCC_FLOW_CONTROL)
		{
			CString str;
			str.Format("TableName %.20s ,Key1 %ld ,Key2 %ld, Record %.600s ",
				szTableName,lKey1,lKey2,szRecord);
			_LOGMSG.LogReq(  0,
												"ICTRL", 
												"SetRecord", 
												(LPCSTR)str  ); 
		}	
	}
	
	
	if(retCode == CTRL_OK)
	{  
		// bCreate forse to FASLE - 
		// creating parameters via CTRL is not allowed 
		long lRtc = _Module.m_server.m_cParam.SetRecord(szTableName,lKey1,lKey2,szRecord,FALSE); 			
	 	if(lRtc)
		{
			switch(lRtc)
			{
				case 1: retCode = CTRL_FAILED_TO_OPEN_TABLE;break;		 
				case 2: retCode = CTRL_PARAM_NOT_FOUND;break;		 
			}
			
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetRecord",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;


}

STDMETHODIMP CCtrl::SetRecord(BSTR sTableAndVal, long lKey1, long lKey2, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		SetRecord__(sTableAndVal,lKey1,lKey2, pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetRecord")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::SetRecord()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}


STDMETHODIMP CCtrl::GetTankReading2(short nFlags, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus1, short *nReadingFlags, long *lFuelLevel, long *lWaterLevel, long *lTemperature, long *lFuelVolume, long *lWaterVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *lStatus2, long *lFuelValue, long *lPricePerLitre, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetTankReading2__(nFlags,lIndexNumber,lTank,lProduct,lStatus1,nReadingFlags,lFuelLevel,lWaterLevel,lTemperature,lFuelVolume,lWaterVolume,lActiveReadNumber,lShiftNumber,sDateTime,lStatus2,lFuelValue,lPricePerLitre,pVal);

#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetTankReading2")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetTankReading2()", LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;
}

void CCtrl::GetTankReading2__(short nFlags, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus1, short *nReadingFlags, long *lFuelLevel, long *lWaterLevel, long *lTemperature, long *lFuelVolume, long *lWaterVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *lStatus2, long *lFuelValue, long *lPricePerLitre, long *pVal)
{
	
	
	USES_CONVERSION;

	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	TANK_READ_INFO info;
	memset(&info,0,sizeof(TANK_READ_INFO));
	
	switch(nFlags)
	{
		case GET_DIRECT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ,&info);
			break;
		case GET_FIRST_INDEX:
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_FIRST,&info);
			break;
		case GET_NEXT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_NEXT,&info);
			break;
		default :
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_FIRST,&info);
			break;
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	
	if(retCode == CTRL_OK)
	{
		*lIndexNumber     = info.lIndexNumber;
		*lTank            = info.lTank;
		*lProduct         = info.lProduct;
		*lStatus1         = info.lStatus;
		*nReadingFlags    = info.nFlags;
		*lFuelLevel       = info.lFuelLevel;
		*lWaterLevel      = info.lWaterLevel;
		*lTemperature     = info.lTemperature;
		*lFuelVolume      = info.lFuelVolume;
		*lWaterVolume     = info.lWaterVolume;
		*lActiveReadNumber= (long)info.dwActiveReadNumber; 
		*lShiftNumber     = (long)info.dwShiftNumber; 
		*lStatus2         = info.lStatus2;
		*lFuelValue       = info.lFuelValue;
		*lPricePerLitre   = info.lPricePerLitre;

		SZ_TAG_SYSTEMTIME szTagTime;
		szTagTime.sNull = 0;

		SystemTime2Tag (&info.cSysTime, &szTagTime.sTime);
		CComBSTR sTime(sizeof(szTagTime.sTime), (LPCSTR)&szTagTime.sTime); // CID_185190 Out-of-bounds access

		*sDateTime = sTime.Copy();
	}

	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetTankReading2",
					TRUE,retCode,
					(lRtc!=0)? TRUE:FALSE,lRtc);
	}

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::GetPumpTotals2(short nFlags, long *lIndexNumber, long *lPump, short *nNozzle, short *nGrade, short *nTotalsFlags, BSTR *sVolume, BSTR *sValueA, BSTR *sValueB, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *lPumpStatus, long *lNozzleStatus, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetPumpTotals2__(nFlags,lIndexNumber,lPump,nNozzle,nGrade,nTotalsFlags,sVolume,sValueA,sValueB,lActiveReadNumber,lShiftNumber,sDateTime,lPumpStatus,lNozzleStatus,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetPumpTotals2")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetPumpTotals2()", LOG_LEVEL_0);

		exit(1);
	}
#endif	


	return S_OK;

}

void CCtrl::GetPumpTotals2__(short nFlags, long *lIndexNumber, long *lPump, short *nNozzle, short *nGrade, short *nTotalsFlags, BSTR *sVolume, BSTR *sValueA, BSTR *sValueB, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *lPumpStatus, long *lNozzleStatus, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;	

	long lRtc = OK;
	TOTAL_INFO info;
	memset(&info,0,sizeof(TOTAL_INFO));

	switch(nFlags)
	{
		case GET_DIRECT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ,&info);
			break;
		case GET_FIRST_INDEX:
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ_FIRST,&info);
			break;	
		case GET_NEXT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ_NEXT,&info);
			break;
		default :
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ_FIRST,&info);
			break;
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	
	if(retCode == CTRL_OK)
	{
		*lIndexNumber = info.lIndexNumber;
		*lPump = info.lPump;
		*nNozzle = info.nNozzle;
		*nGrade = info.nGrade;
		*nTotalsFlags = info.nFlags; 
		*lPumpStatus = info.byPumpStatus;
		*lNozzleStatus = info.byNozzleStatus;

		//4.0.26.501 - TD 116227
		//Make extended grade conversion if needed
		if ((_Module.m_server.GetMaxGrade999()) && (info.nGrade > 0))
			_Module.m_server.GetExtGradeByGrade(info.nGrade, (long *)nGrade, info.lPump);

		
		BYTE sTmp[sizeof(info.sVolume)+2];

		Str2StrNull((BYTE *)info.sVolume,sizeof(info.sVolume),sTmp);
		CComBSTR sVol(sizeof(sTmp),(LPCSTR)sTmp); 
		*sVolume = sVol.Copy();

		Str2StrNull((BYTE *)info.sValueA,sizeof(info.sValueA),sTmp);
		CComBSTR sValA(sizeof(sTmp),(LPCSTR)sTmp); 
		*sValueA = sValA.Copy();

		Str2StrNull((BYTE *)info.sValueB,sizeof(info.sValueB),sTmp);
		CComBSTR sValB(sizeof(sTmp),(LPCSTR)sTmp); 
		*sValueB = sValB.Copy();

		*lActiveReadNumber = (long)info.dwActiveReadNumber;
		*lShiftNumber = (long)info.dwShiftNumber;	

		SZ_TAG_SYSTEMTIME szTagTime;
		szTagTime.sNull = 0;
		SystemTime2Tag (&info.cSysTime, &szTagTime.sTime);
		CComBSTR sTime(sizeof(szTagTime.sTime), (LPCSTR)&szTagTime.sTime); // CID_184040 Out-of-bounds access

		*sDateTime = sTime.Copy();
	}
	
	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetPumpTotals2",
					TRUE,retCode,
					(lRtc!=0)? TRUE:FALSE,lRtc);
	}

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::GetDeliveryReport2(short nFlags, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus, short *nDeliveryFlags, long *lTag, long *lDeliveredVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sStartDeliveryDateTime, BSTR *sEndDeliveryDateTime, long *lStartVolume, long *lEndVolume, long *lTemperature, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetDeliveryReport2__(nFlags,lIndexNumber,lTank,lProduct,lStatus,nDeliveryFlags,lTag,lDeliveredVolume,lActiveReadNumber,lShiftNumber,sStartDeliveryDateTime,sEndDeliveryDateTime,lStartVolume,lEndVolume,lTemperature,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetDeliveryReport2")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetDeliveryReport2()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

void CCtrl::GetDeliveryReport2__(short nFlags, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus, short *nDeliveryFlags, long *lTag, long *lDeliveredVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sStartDeliveryDateTime, BSTR *sEndDeliveryDateTime, long *lStartVolume, long *lEndVolume, long *lTemperature, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	DELIVERY_INFO info;
	memset(&info,0,sizeof(DELIVERY_INFO));
	
	switch(nFlags)
	{
		case GET_DIRECT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ,&info);
			break;
		case GET_FIRST_INDEX:
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ_FIRST,&info);
			break;
		case GET_NEXT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ_NEXT,&info);
			break;
		default :
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ_FIRST,&info);
			break;
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	
	if(retCode == CTRL_OK)
	{
		*lIndexNumber = info.lIndexNumber;
		*lTank = info.lTank;
		*lProduct = info.lProduct;
		*lStatus = info.lStatus;
		*nDeliveryFlags = info.nFlags;
		*lTag = info.lTag;
		*lDeliveredVolume = info.lDeliveredVolume;
		*lActiveReadNumber = (long)info.dwActiveReadNumber; 
		*lShiftNumber = (long)info.dwShiftNumber; 
		*lStartVolume = info.lStartVolume;
		*lEndVolume = info.lEndVolume;
		*lTemperature = info.lTemperature;

		SZ_TAG_SYSTEMTIME szTagTime;
		szTagTime.sNull = 0;
		SystemTime2Tag (&info.cDeliveryDateTime, &szTagTime.sTime);
		CComBSTR sStartTime(sizeof(szTagTime.sTime),(LPCSTR)&szTagTime.sTime); // CID_183958 Out-of-bounds access
		*sStartDeliveryDateTime = sStartTime.Copy();

		szTagTime.sNull = 0;
		SystemTime2Tag (&info.cEndDeliveryDateTime, &szTagTime.sTime);
		CComBSTR sEndTime(sizeof(szTagTime.sTime),(LPCSTR)&szTagTime.sTime); // CID_183958 Out-of-bounds access
		*sEndDeliveryDateTime = sEndTime.Copy();
	}
	
	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetDeliveryReport2",
					TRUE,retCode,
					(lRtc!=0)? TRUE:FALSE,lRtc);
	}

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::GetAlarm(short nFlags, long *lIndexNumber,long *plCatagory, long *plCode, long *plParameter, BSTR *psStartDateAndTime, BSTR *psEndDateAndTime,long *plActiveReadNumber, long *plShiftNumber, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetAlarm__(nFlags,lIndexNumber,plCatagory,plCode,plParameter,psStartDateAndTime,psEndDateAndTime,plActiveReadNumber, plShiftNumber,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetAlarm")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetAlarm()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

void CCtrl::GetAlarm__(short nFlags, long *lIndexNumber,long *plFlags, long *plCode, long *plParameter, BSTR *psStartDateAndTime, BSTR *psEndDateAndTime, long *plActiveReadNumber, long *plShiftNumber, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	ALARM_INFO info;
	memset(&info,0,sizeof(ALARM_INFO));
	
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
	
	switch(nFlags)
	{
		case GET_DIRECT_INDEX: 
		{
			info.lIndexNumber = *lIndexNumber;
			lRtc = (PumpSrvDatabase.LoadAlarmSrv(info.lIndexNumber, info)) ? (OK) : (1);
			break;
		}
		default: case GET_FIRST_INDEX: 
		{
			lRtc = (PumpSrvDatabase.LoadFirstAlarmSrv(info)) ? (OK) : (1);
			break;
		}
		case GET_NEXT_INDEX: 
		{
			info.lIndexNumber =  *lIndexNumber;
			lRtc = (PumpSrvDatabase.LoadNextAlarmSrv(info.lIndexNumber, info)) ? (OK) : (1);
			break;
		}
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	
	if(retCode == CTRL_OK)
	{
		*lIndexNumber = info.lIndexNumber;
		*plFlags = info.lFlags;
		*plCode = info.lCode;
		*plFlags = info.lFlags;
		*plActiveReadNumber = (long)info.dwActiveReadNumber; 
		*plShiftNumber = (long)info.dwShiftNumber; 

		SZ_TAG_SYSTEMTIME szTagTime;
		szTagTime.sNull = 0;
		SystemTime2Tag (&info.cStartDateTime, &szTagTime.sTime);
		CComBSTR sStartTime(sizeof(szTagTime.sTime), (LPCSTR)&szTagTime.sTime); // CID_183853 Out - of - bounds access
		*psStartDateAndTime = sStartTime.Copy();

		szTagTime.sNull = 0;
		SystemTime2Tag (&info.cEndDateTime, &szTagTime.sTime);
		CComBSTR sEndTime(sizeof(szTagTime.sTime), (LPCSTR)&szTagTime.sTime); // CID_183853 Out - of - bounds 
		*psEndDateAndTime = sEndTime.Copy();
	}
	
	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetAlarm",
					TRUE,
					retCode,
					(lRtc != 0) ? TRUE : FALSE,
					lRtc);
	}

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::RemoveAlarm(short nFlags, long lRecIndex, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		RemoveAlarm__(nFlags,lRecIndex,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::RemoveAlarm")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::RemoveAlarm()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	


	return S_OK;
}

void CCtrl::RemoveAlarm__(short nFlags, long lRecIndex, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	ALARM_INFO info;
	memset(&info,0,sizeof(ALARM_INFO));
		
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
		
	switch(nFlags)
	{
		case REMOVE_FIRST_TILL_INDEX:
			retCode = CTRL_NOT_SUPPORTED;	
			break;
		case REMOVE_DIRECT_INDEX:
		default :
			info.lIndexNumber = lRecIndex;
			lRtc = PumpSrvDatabase.Delete(info) ? OK : 1;
			break;		
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	else
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("Alarm, record %d successfully removed ",lRecIndex);
			_LOGMSG.LogMsg(str);
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"RemoveAlarm",
				TRUE,retCode,
				(lRtc!=0)? TRUE:FALSE,lRtc);

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::GetServiceStatus(long lService, long *lStatus, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		GetServiceStatus__(lService,lStatus,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetServiceStatus")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetServiceStatus()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

void CCtrl::GetServiceStatus__(long lService, long *lStatus, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;	

	switch(lService)
	{
		case ALL_SERVICES:
			//Since we can return status for only one service at a time, 
			//in this case ALL_SERVICES is an invalid service.
			*lStatus = CTRL_INVALID_SERVICE;
			break;

		case OLA_SERVICE:
			*lStatus = _Module.m_server.GetOLAServiceStatus();
			break;

		default:
			*lStatus = CTRL_INVALID_SERVICE;
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetServiceStatus",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::StopService(long lService, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		StopService__(lService,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::StopService")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::StopService()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	


	return S_OK;
}

void CCtrl::StopService__(long lService, long *pVal)
{
	long lRetCode = 0;
	CtrlRetCode retCode = CTRL_OK;

	switch(lService)
	{
		case ALL_SERVICES:
			//OLA
			lRetCode = _Module.m_server.StopOLAService();
			//More...
			break;

		case OLA_SERVICE:
			lRetCode = _Module.m_server.StopOLAService();
			break;

		default:
			retCode = CTRL_INVALID_SERVICE;
	}

	if ( lRetCode )
		retCode = CTRL_SERVICE_IN_MIDDLE_OF_OPERATION;

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"StopService",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::StartService(long lService, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		StartService__(lService,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::StartService")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::StartService()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	


	return S_OK;
}

void CCtrl::StartService__(long lService, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;	

	switch(lService)
	{
		case ALL_SERVICES:
			//OLA
			_Module.m_server.m_cOLASrv->StartOLA();
			//More...
			break;

		case OLA_SERVICE:
			_Module.m_server.m_cOLASrv->StartOLA();
			break;

		default:
			retCode = CTRL_INVALID_SERVICE;
	}

	
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"StartService",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::GetWetStockData(long lWetStockFlags, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		GetWetStockData__(lWetStockFlags,pVal);
		
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetWetStockData")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetWetStockData()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	
	return S_OK;
}

void CCtrl::GetWetStockData__(long lWetStockFlags, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;
	WetStockInternalRetCode InternalRetCode=WetStockInternal_OK ;

	if (SERVER_STATE_STARTUP)
		retCode = CTRL_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = CTRL_SERVER_NOT_READY;


	if(retCode == CTRL_OK)
	{
		switch(_Module.m_server.m_cCommonFunction.GetWetStockData__(lWetStockFlags, pVal))
		{
		case WetStockInternal_OK:
				retCode = CTRL_OK;
			break;

		case WetStockInternal_IN_MIDDLE_OF_PROCESS:
				retCode = CTRL_IN_MIDDLE_OF_PROCESS;
			break;
		default:
			_LOGMSG.LogMsg("CCtrl::GetWetStockData return  value out of range");
		}
	}

		
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetWetStockData",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::ReConfigure(long lConfigureFlags, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		ReConfigure__(lConfigureFlags,pVal);
		
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::ReConfigure")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::ReConfigure()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	
	return S_OK;
}

void CCtrl::ReConfigure__(long lConfigureFlags, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;	
	
	if(retCode == CTRL_OK)
	{
		if (lConfigureFlags)
		{				
			CString str("ReConfigure Flags - ");
			if (lConfigureFlags & RE_CONFIGURE_TERMINALS)
			{
				str += "Terminals, ";
			}
			if (lConfigureFlags & RE_CONFIGURE_CAR_WASH)
			{
				str += "Car Wash, ";
				_Module.m_server.LoadCarWashDefinition();
			}
			if (lConfigureFlags & RE_CONFIGURE_DISPENSERS)//4.0.1.32 start
			{
				str += "Dispensers, ";
			}
			if (lConfigureFlags & RE_CONFIGURE_AVI)
			{
				str += "AVI, ";
			}
			if (lConfigureFlags & RE_CONFIGURE_PRICE_POLES)
			{
				str += "Price Poles, ";
			}
			if (lConfigureFlags & RE_CONFIGURE_TANK_GAUGE)
			{
				str += "Tank Gauge, ";
			}//4.0.1.32 end
			_Module.m_server.SetCommand(CMD_RE_CONFIGURE,0,lConfigureFlags);

			_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_RECONFIGURE,lConfigureFlags); //4.0.14.507

			if(LOG_BASIC_FLOW_CONTROL)
			{
				str = str.Left(str.GetLength() - 2);
				_LOGMSG.LogMsg(str);
			}
		}
	}
		
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"ReConfigure",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::GetPOSInfo(BSTR *sPOSInfo, long *pVal)
{
	if(sPOSInfo == NULL)
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"ICTRL::GetPOSInfo   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	

		return E_POINTER;
	}
	else
	{
#ifdef CATCH_CTRL
		__try
		{
#endif
			GetPOSInfo__(sPOSInfo,pVal);	
#ifdef CATCH_CTRL
		}
		__except(_HANDLE_EXCEPTION("CCtrl::GetPOSInfo")) // 4.0.15.460
		{
			// we arrive here on error only !
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::GetPOSInfo()", LOG_LEVEL_0);	

			exit(1);
		}
#endif
	}
	return S_OK;
}

void CCtrl::GetPOSInfo__(BSTR *sPOSInfo, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;

	long lPOSNumber;
	CPosInfo tmpPos;
	CString strPOSInfo;
	CString strValue;
	POSITION position = _Module.m_server.m_cPosMap.GetStartPosition();

	strPOSInfo = "<POSInfo>";
	while(position)
	{
		_Module.m_server.m_cPosMap.GetNextAssoc(position,lPOSNumber,tmpPos);
			
		strPOSInfo += "<POS ID = "; //Beggining of every POS info
		strValue.Format("\"%ld\" ",lPOSNumber); //Format POS number
		strPOSInfo += strValue; //Add to the output string

		strPOSInfo += "REALPOS = "; //Beggining of every POS info
		strValue.Format("\"%ld\" ", ( tmpPos.IsRealPOS() ? 1 : 0 ));
		strPOSInfo += strValue; //Add to the output string

		strPOSInfo += "CONNECTED = "; //Beggining of every POS info
		strValue.Format("\"%ld\" ", ( tmpPos.IsPOSConnected() ? 1 : 0 ));
		strPOSInfo += strValue; //Add to the output string
		
		strPOSInfo += "/>";
		
	}
	strPOSInfo += "</POSInfo>";

	CComBSTR strResult(strPOSInfo.GetLength()+1,(LPCTSTR)strPOSInfo);


	*sPOSInfo = strResult.Copy(); 

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetPOSInfo",
				TRUE,retCode,
				FALSE,0);



	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::SetMaintenance(long lLock,long lParam,long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif
		SetMaintenance__(lLock,lParam,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetMaintenance")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::SetMaintenance()", LOG_LEVEL_0);	

		exit(1);
	}
#endif
	return S_OK;
}

void CCtrl::SetMaintenance__(long lLock,long lParam,long *pVal)
{
	long lRetCode;
	
	_Module.m_server.SetMaintenance(lLock,lParam,&lRetCode);

	if(lRetCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetMaintenance",
				TRUE,lRetCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = lRetCode;
}

STDMETHODIMP CCtrl::GetParam3(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long *pVal)
{

	if( ((sVal == NULL) && (nFiledType == FIELD_SZ)) || 
		((lVal == NULL) && (nFiledType == FIELD_DWORD)) )
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg(
			"ICtrl::GetParam3   Throw COM Exception : E_POINTER", LOG_LEVEL_0);	
		return E_POINTER;
	}

#ifdef CATCH_CTRL
	__try
	{
#endif
		GetParam3__(sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal);

#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetParam3")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("Ctrl::GetParam3()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

	return S_OK;
}

STDMETHODIMP CCtrl::SetParam3(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif
		SetParam3__(sParamPosition,lKey1,lKey2,nFiledType,sVal,lVal,bCreate,pVal);

#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetParam3")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("Ctrl::SetParam3()", LOG_LEVEL_0);

		exit(1);
	}
#endif

	return S_OK;
}

void CCtrl::SetParam3__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long *pVal ,BOOL bIniFile)
{
	DWORD dwLastErr = 0;
	CtrlRetCode retCode = CTRL_OK;	
    CString strEmptyName;
	char szTableFieldAndVal[MAX_TABLE_NAME+MAX_TABLE_NAME+MAX_FIELD_NAME+MAX_FIELD_VAL]; 
	char szFieldVal[MAX_FIELD_VAL]; 
	memset(szTableFieldAndVal,0,sizeof(szTableFieldAndVal));

	int iLen = WideCharToMultiByte(CP_ACP,0,sParamPosition,-1,szTableFieldAndVal,sizeof(szTableFieldAndVal),NULL,NULL);
	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = CTRL_TABLE_NAME_TOO_LONG;
	}
	else
	{
		strEmptyName  = szTableFieldAndVal;
		strEmptyName.TrimLeft();
		if(strEmptyName.IsEmpty())
			retCode = CTRL_PARAM_NOT_FOUND;
	}

	SysFreeString(sParamPosition);

	memset(szFieldVal,0,sizeof(szFieldVal));
	if(nFiledType == FIELD_SZ)
	{
		iLen = WideCharToMultiByte(CP_ACP,0,sVal,-1,szFieldVal,sizeof(szFieldVal),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = CTRL_DEFAULT_FIELD_VAL_TOO_LONG;
		}
	}
 
	char szSubjectName[MAX_TABLE_NAME];
	char szTableName[MAX_TABLE_NAME]; 
	char szSubTableName[MAX_TABLE_NAME];
	char szFieldName[MAX_FIELD_NAME]; 

	memset(szSubjectName,0,sizeof(szSubjectName));
	memset(szSubTableName,0,sizeof(szSubTableName));
	memset(szTableName,0,sizeof(szTableName));
	memset(szFieldName,0,sizeof(szFieldName));


	if(retCode == CTRL_OK)
	{
		BYTE *pTmp;

		BOOL bDelimiterFound =
		_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)szTableFieldAndVal,
														(BYTE *)szSubjectName,
														sizeof(szSubjectName), 
														&pTmp);
		if(bDelimiterFound) 
		{
			bDelimiterFound = 	
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
															(BYTE *)szTableName,
															sizeof(szTableName), 
															&pTmp);
			if(bDelimiterFound) 
			{
				bDelimiterFound = 	
				_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
																(BYTE *)szSubTableName,
																sizeof(szSubTableName), 
																&pTmp);
				if (bDelimiterFound)
				{
					bDelimiterFound = 	
					_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
																	(BYTE *)szFieldName,
																	sizeof(szFieldName), 
																	&pTmp);
				}
				else //  Field Name
				{
					retCode = CTRL_PARAM_NOT_FOUND;
				}

			}
			else // Sub Table Name
			{
				retCode = CTRL_PARAM_NOT_FOUND;
			}
		}
		else  // Table Name
		{
			retCode = CTRL_PARAM_NOT_FOUND;
		}
	}
	
	if(retCode == CTRL_OK)
	{  
		// bCreate forse to FASLE - 
		// creating parameters via GCI is not allowed 
		long lRtc=0;
		if(nFiledType == FIELD_SZ)
		{
	  		lRtc = _Module.m_server.m_cParam.SetParam3Level(szSubjectName, szTableName,szSubTableName, lKey1, lKey2, szFieldName,nFiledType,szFieldVal,0,FALSE ,bIniFile); 
		}
		else
		{
	  		lRtc = _Module.m_server.m_cParam.SetParam3Level(szSubjectName, szTableName,szSubTableName, lKey1, lKey2, szFieldName,nFiledType,NULL,lVal,FALSE ,bIniFile); 
		}

	 	if(lRtc)
		{
			switch(lRtc)
			{
				case 1: retCode = CTRL_FAILED_TO_OPEN_TABLE;break;		 
				case 2: retCode = CTRL_PARAM_NOT_FOUND;break;		 
			}
			
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetParam3",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);

	
	if(pVal != NULL)
		*pVal = retCode;
}

//Eyal 4.0.0.62 This method writes to the console in case of error
//only.
void CCtrl::GetParam3__(BSTR sParamPosition, long lKey1, long lKey2, short nFiledType, BSTR *sVal, long *lVal, BOOL bCreate, long *pVal , BOOL bIniFile)
{
	DWORD dwLastErr = 0;
	CtrlRetCode retCode = CTRL_OK;	
	BOOL bAllocEmpty = FALSE;
	CString strEmptyName;
	long lRtc = -1;
	char szTableFieldAndVal[MAX_TABLE_NAME+MAX_TABLE_NAME+MAX_FIELD_NAME+MAX_FIELD_VAL]; 
	memset(szTableFieldAndVal,0,sizeof(szTableFieldAndVal));
	int iLen = WideCharToMultiByte(CP_ACP,0,sParamPosition,-1,szTableFieldAndVal,sizeof(szTableFieldAndVal),NULL,NULL);
	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = CTRL_TABLE_NAME_TOO_LONG;
	}
	else
	{
		strEmptyName  = szTableFieldAndVal;
		strEmptyName.TrimLeft();
		if(strEmptyName.IsEmpty())
			retCode = CTRL_PARAM_NOT_FOUND;
	}

	SysFreeString(sParamPosition);

	char szSubjectName[MAX_TABLE_NAME];
	char szTableName[MAX_TABLE_NAME]; 
	char szSubTableName[MAX_TABLE_NAME];
	char szFieldName[MAX_FIELD_NAME]; 
	char szFieldVal[MAX_FIELD_VAL]; 

	memset(szSubjectName,0,sizeof(szSubjectName));
	memset(szSubTableName,0,sizeof(szSubTableName));
	memset(szTableName,0,sizeof(szTableName));
	memset(szFieldName,0,sizeof(szFieldName));
	memset(szFieldVal,0,sizeof(szFieldVal));


	if(retCode == CTRL_OK)
	{
		BYTE *pTmp;

		BOOL bDelimiterFound =
		_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)szTableFieldAndVal,
														(BYTE *)szSubjectName,
														sizeof(szSubjectName), 
														&pTmp);
		if(bDelimiterFound) 
		{
			bDelimiterFound = 	
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
															(BYTE *)szTableName,
															sizeof(szTableName), 
															&pTmp);
			if(bDelimiterFound) 
			{
				bDelimiterFound = 	
				_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
																(BYTE *)szSubTableName,
																sizeof(szSubTableName), 
																&pTmp);
				if (bDelimiterFound)
				{
					bDelimiterFound = 	
					_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
																	(BYTE *)szFieldName,
																	sizeof(szFieldName), 
																	&pTmp);
				}
				else //  Field Name
				{
					retCode = CTRL_PARAM_NOT_FOUND;
				}

			}
			else // Sub Table Name
			{
				retCode = CTRL_PARAM_NOT_FOUND;
			}
		}
		else  // Table Name
		{
			retCode = CTRL_PARAM_NOT_FOUND;
		}

		if(bDelimiterFound) 
			bDelimiterFound = 	
			_Module.m_server.GetStringTillDelimiterOrEnd(	(BYTE *)pTmp,
															(BYTE *)szFieldVal,
															sizeof(szFieldVal), 
															&pTmp);
	}

		if(LOG_FCC_FLOW_CONTROL)
		{
			CString str;
			str.Format("TableName %.20s ,Key1 %ld ,Key2 %ld ,FieldName %.20s, FiledType %s, FieldVal %.30s",
						szTableName,lKey1,lKey2,szFieldName,(nFiledType)? "FIELD_SZ":"FIELD_DW",szFieldVal);
			_LOGMSG.LogReq(   0, // Src 
												"ICtrl", 
												"GetParam3", 
												(LPCSTR)str,
												FALSE); 
		}	
	


	BOOL bAllowCreate = bCreate;
	BOOL bAlreadyFreedSVal = FALSE; // CID_199267 Double free

	if(nFiledType == FIELD_SZ)
	{
		memset(szFieldVal,0,sizeof(szFieldVal));

		iLen = WideCharToMultiByte(CP_ACP,0,*sVal,-1,szFieldVal,sizeof(szFieldVal),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = CTRL_DEFAULT_FIELD_VAL_TOO_LONG;
		}
		SysFreeString( *sVal );
		bAlreadyFreedSVal = TRUE; // CID_199267 Double free

		if(retCode == CTRL_OK) //DataExportSrv\SCORPIOAgent
		{  			
			if ((!strcmp(szSubjectName, "Grades")) || //4.0.15.60	
				(!strcmp(szSubjectName, "Tanks") && !strcmp(szFieldName, "AccumulatedValue")) || //4.0.15.120 - CR 44000
				(!strcmp(szSubjectName, "Tanks") && !strcmp(szFieldName, "AccumulatedVolume"))||
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "CumulateVolume"))   ||
				(!strcmp(szSubjectName, "GeneralParam") && !strcmp(szFieldName, "WetstockMemoCounter")) ||
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "PumpReadingCounter")) ||
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "TankReadingCounter"))||
				(!strcmp(szSubjectName, "GeneralParam") &&!strcmp(szFieldName, "ShiftsToMaintenance")) ||
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "BaseTotals")) ||	//4.0.19.502 47235
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "CurrentTotals")) || //4.0.19.502 47235
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "ExceptionTotals")) || //4.0.19.502 47235
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "CurrentAmountTotals")) || //4.0.31.??? TD 405462
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "BaseAmountTotals")) || //4.0.31.??? TD 405462
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "WetStockXmlFileCounter")) || //4.0.19.502 47235
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "TankSalesFileCounter")) || //4.0.19.502 47235
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "TankSalesDailyFileCounter")) || //4.0.19.502 47235
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "TankSalesDailyTmpCounter")) || //4.0.19.502 47235
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "SalesXmlFileCounter")) || 
				(!strcmp(szSubjectName, "Shifts") && !strcmp(szFieldName, "StartTime")) || //4.0.25.502
				(!strcmp(szSubjectName, "GeneralParam") &&!strcmp(szFieldName, "PriceChangeBatchNumber")) ||  //4.0.26.508 147183
				(!strcmp(szSubjectName, "TankSrv") && !strcmp(szFieldName, "SequenceNumber")) ||      //4.0.29.500 - TD 245565
				(!strcmp(szSubjectName, "GeneralParam") && !strcmp(szFieldName, "ActiveShiftNumber"))) //4.0.31.??? TD 405462



				bIniFile = TRUE;

			lRtc =  _Module.m_server.m_cParam.GetParam3Level(szSubjectName, szTableName,szSubTableName, lKey1, lKey2, szFieldName,nFiledType,szFieldVal,sizeof(szFieldVal),NULL,bAllowCreate ,bIniFile);  //4.0.8.503
			if(lRtc != 0) 
			{
				bAllocEmpty = TRUE;
			}
			else
			{
				CString strRetVal(szFieldVal);			
				if(strRetVal.GetLength())
				{
					( *sVal ) = strRetVal.AllocSysString(); //3.0.1.61
					bAlreadyFreedSVal = FALSE; // CID_199267 Double free
				}
				else
				{
					bAllocEmpty = TRUE;
				}
			}			
		}
	    else
		{
			bAllocEmpty = TRUE;
		}
	}
	else // field DW 
	{
		DWORD dwRetVal = *lVal;

		if(retCode == CTRL_OK)	
		{
			if ((!strcmp(szSubjectName, "Grades")) || //4.0.15.60	 
				(!strcmp(szSubjectName, "Tanks") && !strcmp(szFieldName, "AccumulatedValue")) ||	//4.0.15.120 - CR 44000	
				(!strcmp(szSubjectName, "Tanks") && !strcmp(szFieldName, "AccumulatedVolume"))||
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "CumulateVolume"))   ||
				(!strcmp(szSubjectName, "GeneralParam") && !strcmp(szFieldName, "WetstockMemoCounter")) ||
				(!strcmp(szSubjectName, "GeneralParam") && !strcmp(szFieldName, "TransactNumber")) ||	//4.0.23.260 111318 //4.0.24.60
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "PumpReadingCounter")) ||
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "TankReadingCounter")) ||
				(!strcmp(szSubjectName, "GeneralParam") &&!strcmp(szFieldName, "ShiftsToMaintenance")) ||
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "BaseTotals")) ||	//4.0.19.502 47235
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "CurrentTotals")) || //4.0.19.502 47235
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "ExceptionTotals")) || //4.0.19.502 47235
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "CurrentAmountTotals")) || //4.0.31.??? TD 405462
				(!strcmp(szSubjectName, "Pumps") && !strcmp(szFieldName, "BaseAmountTotals")) || //4.0.31.??? TD 405462
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "WetStockXmlFileCounter")) || //4.0.19.502 47235
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "TankSalesFileCounter")) || //4.0.19.502 47235
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "TankSalesDailyFileCounter")) || //4.0.19.502 47235
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "TankSalesDailyTmpCounter")) ||//4.0.19.502 47235
				(!strcmp(szSubjectName, "DataExportSrv") && !strcmp(szFieldName, "SalesXmlFileCounter")) ||//4.0.24.90
				(!strcmp(szSubjectName, "GeneralParam") &&!strcmp(szFieldName, "PriceChangeBatchNumber")) || //4.0.26.508 147183
				(!strcmp(szSubjectName, "TankSrv") && !strcmp(szFieldName, "SequenceNumber")) ||	//4.0.29.500 - TD 245565
				(!strcmp(szSubjectName, "GeneralParam") && !strcmp(szFieldName, "ActiveShiftNumber")))  //4.0.31.??? TD 405462
				
				
				bIniFile = TRUE;
		
			lRtc =  _Module.m_server.m_cParam.GetParam3Level(szSubjectName, szTableName,szSubTableName, lKey1, lKey2, szFieldName,nFiledType,NULL,0,&dwRetVal,bAllowCreate , bIniFile);  //4.0.8.503
		}

		*lVal = (long)dwRetVal;

		bAllocEmpty = TRUE;
	}


	if(bAllocEmpty)
	{
		CString strRetVal("  ");

		if( sVal && (FALSE == bAlreadyFreedSVal)) // CID_199267 Double free
			SysFreeString( *sVal );	//4.0.20.503
		( *sVal ) = strRetVal.AllocSysString(); //3.0.1.61
	}

	if(lRtc != -1) // lRtc have been used - something happened.
	{
		switch(lRtc)
		{
			case 0 : retCode = CTRL_OK; break;
			case 1 : retCode = CTRL_FAILED_TO_OPEN_TABLE; break;
			case 3 : retCode = CTRL_PARAM_NOT_FOUND_FAILED_TO_CREATE; break;
			case 4 : retCode = CTRL_PARAM_NOT_FOUND; break;		
		}
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
													0, // Src
													"ICTRL",
													"GetParam3",
													TRUE,retCode,
													(dwLastErr!=0)? TRUE:FALSE,dwLastErr,
													FALSE);


	if(pVal != NULL)
			*pVal = retCode;

}

STDMETHODIMP CCtrl::SetGradePrice2(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long lPrice, BSTR sTimeValid, short nFlags, long lTaxRate, long lDiscountPerUnit, long lMaxUnitForDiscount, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		if (nFlags & SEND_PRICE_DIRECT_TO_CONVER_LAYER)	 //TD 58094	
			SetGradePrice2Direct__(lPumpNumber,nNozzle,nGrade,lMode,nLevel,lPrice,sTimeValid,nFlags, lTaxRate,lDiscountPerUnit,lMaxUnitForDiscount, pVal);
		else
			SetGradePrice2__(lPumpNumber,nNozzle,nGrade,lMode,nLevel,lPrice,sTimeValid,nFlags, lTaxRate,lDiscountPerUnit,lMaxUnitForDiscount, pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetGradePrice2")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","SetGradePrice2()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif	


	return S_OK;
}

void CCtrl::SetGradePrice2__(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long lPrice, BSTR sTimeValid, short nFlags, long lTaxRate, long lDiscountPerUnit, long lMaxUnitForDiscount, long *pVal)
{

	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();


	CtrlRetCode retCode = CTRL_OK;
	DWORD dwLastErr	=0;


	//Back office should get an error, in case we are not ready to take prices: // TD 230978 4.0.27.501
	if (SERVER_NOT_IDLE && _HOST.m_lHydraActive && (nFlags & PRICE_CHANGE_INTEGRITY))
		retCode = CTRL_SERVER_NOT_READY;


	//4.0.26.501 - TD 116227
	//Make extended grade conversion if needed
	if ((nGrade > 0) && _Module.m_server.GetMaxGrade999())
		_Module.m_server.GetGradeByExtGrade((long)nGrade, (long *)&nGrade);


	if((nGrade > MAX_GRADES_99)||(nGrade <= 0))//amram 4.0.10.501
		retCode = CTRL_INVALID_GRADE_NUMBER;

	
	if (!(nFlags & PRICE_NO_PRICE_CHANGE)) 
	{
		if (retCode == CTRL_OK) 
		{
			if (_Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags()||
				_Module.m_server.m_cServerInfo.GetRequiredPriceChangeFlags()||
				_Module.m_server.m_cServerInfo.GetRequiredModeProcessFlags() ||
				_Module.m_server.m_cServerInfo.GetRequiredMaintenanceProcessFlags() ||
				_Module.m_server.m_cServerInfo.GetInPriceChangeProcess() || //4.0.12.515 		
				_Module.m_server.m_cServerInfo.GetInPriceChangeWithAckProcessFlag())  //4.0.19.820 - TD 78775
			{
				retCode = CTRL_IN_MIDDLE_OF_PROCESS;
				_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
			}
		}
	}

	if(retCode == CTRL_OK)
	{
		DWORD dwTmp;
		BOOL bIgnoreGrade = FALSE; //4.0.24.70 - TD 110193
		CString str("Grades\\Grade");
		
		char sNumber[8];
		memset(sNumber,0,sizeof(sNumber));
		sprintf(sNumber,"%02d",nGrade);
		str += sNumber;


		SZ_TAG_SYSTEMTIME szTagTime;
		memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
		int iLen = WideCharToMultiByte(CP_ACP,0,sTimeValid,-1,(char *)&szTagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = CTRL_LAST_ERROR;
		}

		//4.0.27.20 - TD 215136 
		if (lPrice <= 0)
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Invalid Grade price !!! Price %ld, Grade %d, Mode %d, Level %d", lPrice, nGrade, lMode, nLevel);
				_LOGMSG.LogMsg("CCtrl","SetGradePrice2",lPumpNumber,LOG_PUMP,str);	
			}
			
			bIgnoreGrade = TRUE;
			if (!(nFlags & PRICE_END_OF_LIST))
				retCode = CTRL_INVALID_GRADE_NUMBER;
		}		
		
		//4.0.24.60 - TD 107311
		if(retCode == CTRL_OK)
		{
			//4.0.27.31 - TD 224614
			if((nFlags & MAX_UNIT_IS_BATCH_NUM) && (!(nFlags & PRICE_CHANGE_WITH_ACK_PER_PUMP)))
			{
				_Module.m_server.m_cParam.SaveParam("GeneralParam","PriceChangeBatchNumber",lMaxUnitForDiscount,TRUE,_HOST.m_lGroupNum,0,0,TRUE); //4.0.27.43
				_Module.m_server.SetPriceChangeBatchNumber(lMaxUnitForDiscount); 
			}
			else
			{
				_Module.m_server.SetPriceChangeBatchNumber(0); 
			}
			
			if (_Module.m_server.GetIgnoreNonValidGradesOnPriceChangeFlag())
			{
				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwTmp,(DWORD)0, FALSE);
				if (!dwTmp)
				{
					//4.0.24.70 - TD 110193
					bIgnoreGrade = TRUE;
					if (!(nFlags & PRICE_END_OF_LIST))
					{
						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;
							str.Format("Non Valid Grade - Ignoring !!! Price %ld, Grade %d, Mode %d, Level %d", lPrice, nGrade, lMode, nLevel);
							_LOGMSG.LogMsg("CCtrl","SetGradePrice2",lPumpNumber,LOG_PUMP,str);	
						}
						retCode = CTRL_INVALID_GRADE_NUMBER;
					}
				}
			}
		}
		
		if(retCode ==CTRL_OK)
		{

			if (!bIgnoreGrade) //4.0.24.70 - TD 110193
			{		
				_Module.m_server.ResetGradePriceTable(nGrade,  lMode,  nLevel , nFlags); // TD 236747

				// check if empty date ...
				if( ((szTagTime.sTime.sMonth[0] == ' ')&&(szTagTime.sTime.sMonth[1] == ' ')) ||
					((szTagTime.sTime.sMonth[0] == 0)&&(szTagTime.sTime.sMonth[1] == 0)) )
				{	// no time so set to current time :			
					
					SYSTEMTIME curSysTime;
					GetLocalTime(&curSysTime);					
					SystemTime2Tag(&curSysTime,&szTagTime.sTime);
				}

				_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "ValidEntry",(DWORD)1 ,TRUE);
				_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "TaxRate",(DWORD)lTaxRate, TRUE);				
				_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "DiscountPerUnit",(DWORD)lDiscountPerUnit,TRUE);
				_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "MaxUnitForDiscount",(DWORD)lMaxUnitForDiscount ,TRUE);				

				if(nFlags & MAX_UNIT_IS_BATCH_NUM)   //4.0.23.1540
				{
					_Module.m_server.m_cParam.SaveParam("GeneralParam","PriceChangeBatchNumber",(DWORD)lMaxUnitForDiscount,TRUE,0,0,0,TRUE);
					_Module.m_server.SetPriceChangeBatchNumber(lMaxUnitForDiscount);

					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("SetGradePrice2Direct Grade %d, Mode %d, Level %d, price %ld , PriceChangeBatchNumber=%ld",nGrade, lMode, nLevel, lPrice , lMaxUnitForDiscount);
						_LOGMSG.LogMsg("CCtrl","SetGradePrice2Direct",lPumpNumber,LOG_PUMP,str);	
					}
				}

				
				// mode == 2 support old version 
				if((lMode == 2 ) || (lMode == FULL_SERVICE_MODE)) 
				{ 
					// full service
					if(nLevel == PRICE_LEVEL_CASH)
					{

						_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCash_FullService", &dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);

						if(_Module.m_server.IsRollbackOnPriceChangeFailure())					//4.0.12.515
						{
							//Save the previous grade for case of rollback.
							_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Previous_PriceCash_FullService",dwTmp,TRUE,0,0,0,TRUE);
						}


						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Old_PriceCash_FullService",dwTmp,TRUE,0,0,0,TRUE);    // 4.0.10.10
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "New_PriceCash_FullService",(DWORD) lPrice,TRUE,0,0,0,TRUE);
						_Module.m_server.SetGradePriceToArray(nGrade, lPrice, FULL_CASH);
					}
					else
					{
						_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCredit_FullService", &dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);

						if(_Module.m_server.IsRollbackOnPriceChangeFailure())					//4.0.12.515
						{
							_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Previous_PriceCredit_FullService",dwTmp,TRUE,0,0,0,TRUE);
						}

						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Old_PriceCredit_FullService",dwTmp,TRUE,0,0,0,TRUE);
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "New_PriceCredit_FullService",(DWORD) lPrice,TRUE,0,0,0,TRUE);
						_Module.m_server.SetGradePriceToArray(nGrade, lPrice, FULL_CREDIT);
					}
				}
				else 
				{ // self service
					
					if(nLevel == PRICE_LEVEL_CASH)
					{
						_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCash_SelfService", &dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);

						if(_Module.m_server.IsRollbackOnPriceChangeFailure())					//4.0.12.515
						{
							_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Previous_PriceCash_SelfService",dwTmp,TRUE,0,0,0,TRUE);
						}

						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Old_PriceCash_SelfService",dwTmp, TRUE,0,0,0,TRUE);
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "New_PriceCash_SelfService",(DWORD) lPrice, TRUE,0,0,0,TRUE);
						_Module.m_server.SetGradePriceToArray(nGrade, lPrice, SELF_CASH);
					}
					else 
					{
						_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "New_PriceCredit_SelfService", &dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);

						if(_Module.m_server.IsRollbackOnPriceChangeFailure())					//4.0.12.515
						{
							_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Previous_PriceCredit_SelfService",dwTmp,TRUE,0,0,0,TRUE);
						}

						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Old_PriceCredit_SelfService",dwTmp,TRUE,0,0,0,TRUE);
						_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "New_PriceCredit_SelfService",(DWORD) lPrice,TRUE,0,0,0,TRUE);
						_Module.m_server.SetGradePriceToArray(nGrade, lPrice, SELF_CREDIT);
					}			
				}

				long lLowestPrice = _Module.m_server.GetGradePriceFromArray(nGrade, PRICE_PROTECTION_LOWEST_PRICE);
				if ((lPrice < lLowestPrice) &&
					(lPrice > _Module.m_server.GetGradePriceMinimumLimit())) // QC 470359
				{
					_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "Day_Lowest_Price", (DWORD)lPrice, TRUE, 0, 0, 0, TRUE);
					_Module.m_server.SetGradePriceToArray(nGrade, lPrice, PRICE_PROTECTION_LOWEST_PRICE);
				}

				// QC 501964
				if (dwTmp != lPrice)
				{
					_Module.m_server.m_GradeData.SetGradePriceChanged(nGrade, TRUE);
				}

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("SetGradePrice2 Grade %d, Mode %d, Level %d, price %ld, flags %ld",nGrade, lMode, nLevel, lPrice, nFlags);
					_LOGMSG.LogMsg("CCtrl","SetGradePrice2",lPumpNumber,LOG_PUMP,str);	
				}
			}

			if((nFlags & PRICE_CHANGE_INTEGRITY) && !_Module.m_server.m_cServerInfo.IsInPriceChangeWithIntegrityProcess()) //4.0.27.20
			{
				_Module.m_server.m_cServerInfo.SetInPriceChangeWithIntegrityProcessFlags(PRICE_CHANGE_WITH_INTEGRITY_ACTIVE);

				SQLPumpSrvFile.SaveServerInfo();

			}

			if (!(nFlags & PRICE_NO_PRICE_CHANGE)) 
			{
				if (IsManualPricePolePriceChangeRequired(nFlags)) //manual integrity 4.0.26.508 147183
				{
									
					_Module.m_server.m_cServerInfo.SetInPriceChangeWithIntegrityProcessFlags(PRICE_CHANGE_WITH_INTEGRITY_PENDING_COMMIT); 
					_Module.m_server.SetPriceChangeResponse(GOT_PRICE_CHANGE_PENDING_COMMIT);	
					
					if (nFlags & PRICE_END_OF_LIST)  
					{
						_Module.m_server.m_cPriceChangeFlags.SavedFlags(szTagTime , 0); 

						SQLPumpSrvFile.SaveServerInfo();

						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;
							str.Format("SetGradePrice2  - End Of Price List ! - manual" );
							_LOGMSG.LogMsg("CCtrl","SetGradePrice2",lPumpNumber,LOG_PUMP,str);	
						}						
					}
				}
				else 
				{
					if (nFlags & PRICE_END_OF_LIST)  
					{
						if (nFlags & PRICE_CHANGE_INTEGRITY)	//auto integrity
						{
							_Module.m_server.m_cServerInfo.SetInPriceChangeWithIntegrityProcessFlags(PRICE_CHANGE_WITH_INTEGRITY_ACTIVE); 
						}
						else
						{
							_Module.m_server.m_cServerInfo.SetInPriceChangeWithIntegrityProcessFlags(PRICE_CHANGE_WITH_INTEGRITY_NONE); 
							//_Module.m_server.SetPriceChangeBatchNumber(0); // CR 405082 - Fuel Price Change Management - POS <--> BOS  Integration
						}

						_Module.m_server.ActivatePriceChangeProcess(nFlags, lMaxUnitForDiscount);

						_Module.m_server.m_cPriceChangeFlags.SavedFlags(szTagTime , 0); //4.0.8.500

						_Module.m_server.m_cOLASrv->LoadAllProductSoldOnPump(0); //

						if(LOG_BASIC_FLOW_CONTROL)
						{
							CString str;
							str.Format("SetGradePrice2  - End Of Price List ! - auto" );
							_LOGMSG.LogMsg("CCtrl","SetGradePrice2",lPumpNumber,LOG_PUMP,str);	
						}
					}
				}
			}
		}
	}


	if (retCode != CTRL_OK)	//4.0.13.502
	{
		long lBatch = _Module.m_server.GetPriceChangeBatchNumber();
		_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_PUMP, SUB_PRICE_CHANGE, PRICE_CHANGE_WAS_NOT_APPLIED, 1, ALARM_STARTED, lBatch, 0);
	}


	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetGradePrice2",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);

	
	if(pVal != NULL)
		*pVal = retCode;
}

//4.0.26.508 147183
BOOL CCtrl::IsManualPricePolePriceChangeRequired(long nFlags)
{
	BOOL bRequired = FALSE;
	if ((nFlags & PRICE_CHANGE_INTEGRITY) && (!_Module.m_server.IsActivePricePole()))
	{
		bRequired = TRUE;
	}
	return bRequired;

}

STDMETHODIMP CCtrl::SetPumpModeScheme2(long lPumpNumber, long lScheme, long lMode, BSTR sPeriodDateTimeStart, BSTR sPeriodDateTimeEnd, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lFlags, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		SetPumpModeScheme2__(lPumpNumber, lScheme, lMode, sPeriodDateTimeStart, sPeriodDateTimeEnd, sDateTimeStart, sDateTimeEnd, lFlags, pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetPumpModeScheme2")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","SetPumpModeScheme2",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

void CCtrl::SetPumpModeScheme2__(long lPumpNumber, long lScheme, long lMode, BSTR sPeriodDateTimeStart, BSTR sPeriodDateTimeEnd, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lFlags, long *pVal)
{
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(PUMP_NOT_VALID(lPumpNumber))
		retCode = CTRL_INVALID_PUMP_NUMBER;

	else if(MODE_NOT_VALID(lMode))
		retCode = CTRL_INVALID_PUMP_MODE;

	else if(SCHEME_NOT_VALID(lScheme))
		retCode = CTRL_INVALID_SCHEME_NUMBER;
	
	
	if(retCode == CTRL_OK)
	{			  
		long lIndex = _Module.m_server.GenerateModeSchemeIndex(lPumpNumber , lScheme); 			

		EnterCriticalSection(&_Module.m_server.m_csModeSchemeArray);

		
		_Module.m_server.m_cModeSchemeArray[lIndex].SetMode(lMode);
		_Module.m_server.m_cModeSchemeArray[lIndex].SetFlags(lFlags);

		SZ_TAG_SYSTEMTIME szTagTime;
		memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
		int iLen = WideCharToMultiByte(CP_ACP,0,sDateTimeStart,-1,(char *)&szTagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
		if(iLen == 0)
		{			
			SYSTEMTIME curSysTime;
			GetLocalTime(&curSysTime);					
			SystemTime2Tag(&curSysTime,&szTagTime.sTime);
		}

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("start=%.s" , szTagTime);
			_LOGMSG.LogMsg("CCtrl","SetPumpModeScheme2",lPumpNumber,LOG_PUMP,str);	
		}
		
		_Module.m_server.m_cModeSchemeArray[lIndex].SetStartTime(&szTagTime.sTime);

		memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
		iLen = WideCharToMultiByte(CP_ACP,0,sDateTimeEnd,-1,(char *)&szTagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
		if(iLen == 0)
		{			
			SYSTEMTIME curSysTime;
			GetLocalTime(&curSysTime);					
			SystemTime2Tag(&curSysTime,&szTagTime.sTime);
		}	

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("End=%.s" , szTagTime);
			_LOGMSG.LogMsg("CCtrl","SetPumpModeScheme2",lPumpNumber,LOG_PUMP,str);	
		}		
		_Module.m_server.m_cModeSchemeArray[lIndex].SetEndTime(&szTagTime.sTime);

		memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
		iLen = WideCharToMultiByte(CP_ACP,0,sPeriodDateTimeStart ,-1,(char *)&szTagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
		if(iLen == 0)
		{			
			SYSTEMTIME curSysTime;
			GetLocalTime(&curSysTime);					
			SystemTime2Tag(&curSysTime,&szTagTime.sTime);
		}	
		_Module.m_server.m_cModeSchemeArray[lIndex].SetPeriodStartTime(&szTagTime.sTime);

		memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
		iLen = WideCharToMultiByte(CP_ACP,0,sPeriodDateTimeEnd,-1,(char *)&szTagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
		if(iLen == 0)
		{			
			SYSTEMTIME curSysTime;
			GetLocalTime(&curSysTime);					
			SystemTime2Tag(&curSysTime,&szTagTime.sTime);
		}	
		_Module.m_server.m_cModeSchemeArray[lIndex].SetPeriodEndTime(&szTagTime.sTime);

		_Module.m_server.m_cModeSchemeArray[lIndex].SaveDefinition(1);
	    LeaveCriticalSection(&_Module.m_server.m_csModeSchemeArray);
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetPumpModeScheme2",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::GetPumpModeScheme2(long lPumpNumber, long lScheme, long *plMode, BSTR *psPeriodDateTimeStart, BSTR *psPeriodDateTimeEnd, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plFlags, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		GetPumpModeScheme2__(lPumpNumber, lScheme, plMode, psPeriodDateTimeStart, psPeriodDateTimeEnd, psDateTimeStart, psDateTimeEnd, plFlags, pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetPumpModeScheme2")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetPumpModeScheme2",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif	


return S_OK;
}

void CCtrl::GetPumpModeScheme2__(long lPumpNumber, long lScheme, long *plMode, BSTR *psPeriodDateTimeStart, BSTR *psPeriodDateTimeEnd, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plFlags, long *pVal)
{
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(PUMP_NOT_VALID(lPumpNumber))
		retCode = CTRL_INVALID_PUMP_NUMBER;

	else if(SCHEME_NOT_VALID(lScheme))
		retCode = CTRL_INVALID_SCHEME_NUMBER;


	if(retCode == CTRL_OK)
	{			
		long lIndex = _Module.m_server.GenerateModeSchemeIndex(lPumpNumber , lScheme); 			

		*psDateTimeStart = _Module.m_server.m_cModeSchemeArray[lIndex].GetStringStartTime().AllocSysString();	
		*psDateTimeEnd =  _Module.m_server.m_cModeSchemeArray[lIndex].GetStringEndTime().AllocSysString();	
		*psPeriodDateTimeStart =  _Module.m_server.m_cModeSchemeArray[lIndex].GetStringPeriodStartTime().AllocSysString(); 
		*psPeriodDateTimeEnd =  _Module.m_server.m_cModeSchemeArray[lIndex].GetStringPeriodEndTime().AllocSysString(); 

		if(plMode)
			*plMode =  _Module.m_server.m_cModeSchemeArray[lIndex].GetMode();

		if(plFlags)
			*plFlags =  _Module.m_server.m_cModeSchemeArray[lIndex].GetFlags();
		
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetPumpModeScheme2",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::GetServiceFee(long lPump, long lTrsType, long lTrsNumber, BSTR stTime, long lFlags, long *pValue, long *pRetVal)
{
	// TODO: Add your implementation code here
	#ifdef CATCH_CTRL
	__try
	{
#endif
		GetServiceFee__(lPump, lTrsType, lTrsNumber ,stTime, lFlags, pValue, pRetVal)	;			
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetServiceFee")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetServiceFee()",lPump,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

STDMETHODIMP CCtrl::SetServiceFee(long lFeeNumber, BSTR sName , long lFlags ,long lPakFee, long lPrePayFee, long lPapFee, long lAviFee, long lAttendFee, long *pRetVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_CTRL
	__try
	{
#endif
		SetServiceFee__(lFeeNumber, sName, lFlags ,lPakFee, lPrePayFee, lPapFee, lAviFee, lAttendFee, pRetVal);
		
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetServiceFee")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::SetServiceFee()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

void CCtrl::SetServiceFee__(long lFeeNumber, BSTR sName ,long lFlags ,long lPakFee, long lPrePayFee, long lPapFee, long lAviFee, long lAttendFee, long *pRetVal)
{
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(SERVICE_FEE_NOT_VALID(lFeeNumber))
		retCode = CTRL_INVALID_SERVICE_FEE_NUMBER;
	
	if(retCode == CTRL_OK)
	{
		char szName[MAX_FIELD_VAL]; 
		memset(szName,0,sizeof(szName));
		int iLen = WideCharToMultiByte(CP_ACP,0,sName,-1,szName,sizeof(szName),NULL,NULL);
		if(iLen == 0)
			retCode = CTRL_MODE_NAME_TOO_LONG;
		else
		{	
			CServiceFee   ServiceFee;
							
			ServiceFee.SetNumber(lFeeNumber );
			ServiceFee.SetFlagsServiceFee(lFlags);		

			_Module.m_server.m_cParam.SetParam("ServiceFee", lFeeNumber,0,"Name",FIELD_SZ,szName,0,FALSE); //3.0.3.11
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"Valid",FIELD_DWORD,NULL,(DWORD)ServiceFee.ValidServiceFee(),FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"PakService",FIELD_DWORD,NULL,(DWORD)lPakFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"PrePayService",FIELD_DWORD,NULL,(DWORD)lPrePayFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"PapService",FIELD_DWORD,NULL,(DWORD)lPapFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"AviService",FIELD_DWORD,NULL,(DWORD)lAviFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"AttendService",FIELD_DWORD,NULL,(DWORD)lAttendFee,FALSE);		


			ServiceFee.LoadDefinition();
			_Module.m_server.m_cServiceFeeArray[lFeeNumber - 1]= ServiceFee;					
		}		
	}
	 
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetServiceFee",
				TRUE,retCode,
				FALSE,0);

 
	*pRetVal = retCode;
}

void CCtrl::GetServiceFee__(long lPump, long lTrsType, long lTrsNumber ,BSTR stTime, long lFlags, long *pValue, long *pRetVal)
{
	long retCode = CTRL_OK;

	_Module.m_server.GetServiceFeeInfo(lPump, lTrsType, lTrsNumber, stTime, lFlags, pValue, &retCode);

	switch (retCode) 
	{
	case SERVICE_FEE_OK:
			retCode = CTRL_OK;
		break;
	case SERVICE_FEE_NOT_VALID: 
			retCode = CTRL_SERVICE_FEE_NOT_VALID;
		break;
	case SERVICE_FEE_INVALID_PUMP_NUMBER:
			retCode = CTRL_SERVICE_FEE_NOT_VALID;
		break;

	case SERVICE_FEE_TRS_NOT_FOUND:
			retCode = CTRL_TRS_NOT_FOUND;
		break;
	case SERVICE_FEE_PARAM_NOT_FOUND:
			retCode = CTRL_PARAM_NOT_FOUND;
		break;

	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetServiceFee",
				TRUE,retCode,
				FALSE,0);

	*pRetVal = retCode;

}

STDMETHODIMP CCtrl::SetPumpModeScheme3(long lPumpNumber, long lScheme, long lMode, BSTR sPeriodDateTimeStart, BSTR sPeriodDateTimeEnd, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lServiceFeeNo, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_CTRL
	__try
	{
#endif

		SetPumpModeScheme3__(lPumpNumber, lScheme, lMode, sPeriodDateTimeStart, sPeriodDateTimeEnd, sDateTimeStart, sDateTimeEnd, lServiceFeeNo, lFlags, pVal);
		
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetPumpModeScheme3")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","SetPumpModeScheme3()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif		

	return S_OK;
}

void CCtrl::SetPumpModeScheme3__(long lPumpNumber, long lScheme, long lMode, BSTR sPeriodDateTimeStart, BSTR sPeriodDateTimeEnd, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lServiceFeeNo, long lFlags, long *pVal)
{
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(PUMP_NOT_VALID(lPumpNumber))
		retCode = CTRL_INVALID_PUMP_NUMBER;

	else if(MODE_NOT_VALID(lMode))
		retCode = CTRL_INVALID_PUMP_MODE;

	else if(SCHEME_NOT_VALID(lScheme))
		retCode = CTRL_INVALID_SCHEME_NUMBER;
	
	
	if(retCode == CTRL_OK)
	{
		_Module.m_server.SetPumpModeScheme3(lPumpNumber, lScheme, lMode, sPeriodDateTimeStart, sPeriodDateTimeEnd, sDateTimeStart, sDateTimeEnd, lServiceFeeNo, lFlags, pVal);
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetPumpModeScheme3",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::GetPumpModeScheme3(long lPumpNumber, long lScheme, long *plMode, BSTR *psPeriodDateTimeStart, BSTR *psPeriodDateTimeEnd, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plServiceFeeNo, long *plFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_CTRL
	__try
	{
#endif
		GetPumpModeScheme3__(lPumpNumber, lScheme, plMode, psPeriodDateTimeStart, psPeriodDateTimeEnd, psDateTimeStart, psDateTimeEnd, plServiceFeeNo, plFlags, pVal);
	
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetPumpModeScheme3")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetPumpModeScheme3()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif		


	return S_OK;
}

void CCtrl::GetPumpModeScheme3__(long lPumpNumber, long lScheme, long *plMode, BSTR *psPeriodDateTimeStart, BSTR *psPeriodDateTimeEnd, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plServiceFeeNo, long *plFlags, long *pVal)
{
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(PUMP_NOT_VALID(lPumpNumber))
		retCode = CTRL_INVALID_PUMP_NUMBER;

	else if(SCHEME_NOT_VALID(lScheme))
		retCode = CTRL_INVALID_SCHEME_NUMBER;


	if(retCode == CTRL_OK)
	{			
		_Module.m_server.GetPumpModeScheme3(lPumpNumber, lScheme, plMode, psPeriodDateTimeStart, psPeriodDateTimeEnd, psDateTimeStart, psDateTimeEnd, plServiceFeeNo, plFlags, pVal);

	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetPumpModeScheme3",
				TRUE,retCode,
				FALSE,0);
	
	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CCtrl::InitEventsInterface(long lFlags, long *pClientId, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif
		InitEventInterface_(lFlags,pClientId,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::InitEventsInterface")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::InitEventsInterface()", LOG_LEVEL_0);	

		exit(1);
	}	
#endif 

	return S_OK;
}


// Function name	: CCtrl::InitEventInterface_
// Description	    : Inits the CtrlEvents interface.
// Return type		: void 
// Argument         : long lFlags - for future use.
// Argument         : long *pClientId - uniqe client id for the connected client.
// Argument         : long *pVal - function return code.
void CCtrl::InitEventInterface_(long lFlags, long *pClientId, long *pVal)
{
	ICtrlEvents *pCtrlObj = NULL;
	long		lClientId = 0;
		
	pCtrlObj =  (ICtrlEvents *) this;
	
	lClientId = _Module.m_server.GetCtrlEventClientId();
	
	_Module.m_server.AddCtrlEventClient(lClientId,pCtrlObj);

	if( pClientId != NULL )
		*pClientId = lClientId;

	if(pVal != NULL)
		*pVal = 0;
}

STDMETHODIMP CCtrl::CloseEventsInterface(long lClientId, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif
		CloseEventsInterface_(lClientId,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::CloseEventsInterface")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::CloseEventsInterface()", LOG_LEVEL_0);	

		exit(1);
	}	
#endif 

	return S_OK;
}




// Function name	: CCtrl::CloseEventsInterface_
// Description	    : Closes the CtrlEvent connection.
// Return type		: void 
// Argument         : long lClientId - client id recieved when called InitEventsInterface 
// Argument         : long *pVal
void CCtrl::CloseEventsInterface_(long lClientId, long *pVal)
{
	long lRetVal = CTRL_OK;

	lRetVal = _Module.m_server.RemoveCtrlEventPtr(lClientId);

	if(pVal != NULL)
		*pVal = lRetVal;
}

STDMETHODIMP CCtrl::SetServiceFee2(long lFeeNumber, BSTR sName,long lFlags ,long lPakFee,  long lPrePayFee, long lPapFee, long lAviFee, long lAttendFee,long lSelfPakFee, long lSelfPrePayFee,long lSelfPapFee,  long lSelfAviFee,  long lSelfAttendFee, long *pRetVal)
{
		// TODO: Add your implementation code here
#ifdef CATCH_CTRL
	__try
	{
#endif
		SetServiceFee2__(lFeeNumber, sName,lFlags ,lPakFee,lPrePayFee,lPapFee, lAviFee,lAttendFee,lSelfPakFee, lSelfPrePayFee,lSelfPapFee, lSelfAviFee,lSelfAttendFee, pRetVal);
		
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetServiceFee2__")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::SetServiceFee2__()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

void CCtrl::SetServiceFee2__(long lFeeNumber, BSTR sName,long lFlags ,long lPakFee,  long lPrePayFee, long lPapFee, long lAviFee, long lAttendFee,long lSelfPakFee, long lSelfPrePayFee,long lSelfPapFee,  long lSelfAviFee,  long lSelfAttendFee, long *pRetVal)
{
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(SERVICE_FEE_NOT_VALID(lFeeNumber))
		retCode = CTRL_INVALID_SERVICE_FEE_NUMBER;
	
	if(retCode == CTRL_OK)
	{
		char szName[MAX_FIELD_VAL]; 
		memset(szName,0,sizeof(szName));
		int iLen = WideCharToMultiByte(CP_ACP,0,sName,-1,szName,sizeof(szName),NULL,NULL);
		if(iLen == 0)
			retCode = CTRL_MODE_NAME_TOO_LONG;
		else
		{	
			CServiceFee   ServiceFee;
							
			ServiceFee.SetNumber(lFeeNumber );
			ServiceFee.SetFlagsServiceFee(lFlags);		
			_Module.m_server.m_cParam.SetParam("ServiceFee", lFeeNumber,0,"Name",FIELD_SZ,szName,0,FALSE); //3.0.3.11
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"Valid",FIELD_DWORD,NULL,(DWORD)ServiceFee.ValidServiceFee(),FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"PakService",FIELD_DWORD,NULL,(DWORD)lPakFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"PrePayService",FIELD_DWORD,NULL,(DWORD)lPrePayFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"PapService",FIELD_DWORD,NULL,(DWORD)lPapFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"AviService",FIELD_DWORD,NULL,(DWORD)lAviFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"AttendService",FIELD_DWORD,NULL,(DWORD)lAttendFee,FALSE);		
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"SelfPakService",FIELD_DWORD,NULL,(DWORD)lSelfPakFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"SelfPrePayService",FIELD_DWORD,NULL,(DWORD)lSelfPrePayFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"SelfPapService",FIELD_DWORD,NULL,(DWORD)lSelfPapFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"SelfAviService",FIELD_DWORD,NULL,(DWORD)lSelfAviFee,FALSE);
			_Module.m_server.m_cParam.SetParam("ServiceFee" ,lFeeNumber,0,"SelfAttendService",FIELD_DWORD,NULL,(DWORD)lSelfAttendFee,FALSE);		


			ServiceFee.LoadDefinition();
			_Module.m_server.m_cServiceFeeArray[lFeeNumber - 1]= ServiceFee;					
		}		
	}
	 
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetServiceFee",
				TRUE,retCode,
				FALSE,0);

 
	*pRetVal = retCode;
}



/******************************************************************************
 Description:	Transmit a TLS command request to CL 	
 Returns:      	CTRL return codes
 Parameters:   	BSTR& sCommand: an actual TLS command, for example i50301
                long lCommand:commands defined in Pump.h packed into a long
				long lTankNumber:parameter used for calls when info on a specific tank is required
				long lFlags: a. is the input in sCommand or lCommand b.output to text or binary file.
				long *lRetVal
 Assumptions:   Commands are either  commands defined in Pump.h or stored in the BSTR.
				CHECK PUMP.H FOR DEFINITIONS.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			13/3/2002   13:46		Start
******************************************************************************/


void CCtrl::TLSCommand__(BSTR& sCommand, long lCommand , long lTankNumber , long lFlags, long *lRetVal)
{
	BYTE szCommand[MAX_TLS_CMD_LEN];
	CtrlRetCode retCode = CTRL_OK;

	if (SERVER_STATE_STARTUP)
		retCode = CTRL_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = CTRL_SERVER_NOT_READY;

	if(retCode == CTRL_OK)
	{
		memset(szCommand , 0 , MAX_TLS_CMD_LEN);
		
		int iLen = WideCharToMultiByte(CP_ACP,
									   0,
									   sCommand,
									   -1,
									   (char *)&szCommand,
									   sizeof(szCommand),
									   NULL,
									   NULL);

		if(!iLen)
			retCode = CTRL_FAILED_IO;

		if(retCode == CTRL_OK)
		{
			CMD_STRUCT_TLS cmdInfo;
			memcpy(cmdInfo.sCommand , szCommand , sizeof(cmdInfo.sCommand));
			cmdInfo.lCommand = lCommand;
			cmdInfo.lTankNumber = lTankNumber;
			cmdInfo.lFlags = lFlags;

			_Module.m_server.SetCommand(CMD_TLS, 
										SERVER_POS_NUMBER, 
										0L,
										&cmdInfo,
										sizeof(CMD_STRUCT_TLS));
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("TLSCommand send with: Command:%d ,Tank Number: %d , Flags:%d" ,lCommand , lTankNumber , lFlags);
				_LOGMSG.LogMsg(str);
			}
	
		}
	}
	
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"TLSCommand",
				TRUE,retCode,
				FALSE,0);

	if(lRetVal)
		*lRetVal = retCode;

	return;
}


STDMETHODIMP CCtrl::TLSCommand(BSTR sCommand, long lCommand, long lTankNumber, long lFlags, long *plRetVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif
		TLSCommand__(sCommand , lCommand , lTankNumber , lFlags , plRetVal);
		
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::TLSCommand__")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::TLSCommand__()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	
	
	return S_OK;
}

STDMETHODIMP CCtrl::ResetQdxInfo(long lFileNum, long lPumpNum, long lFlags1, long lFlags2, long *pVal)
{
	#ifdef CATCH_CTRL
	__try
	{
#endif
		ResetQdxInfo__(lFileNum,lPumpNum,lFlags1,lFlags2,pVal);
		
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::ResetQdxInfo")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","ResetQdxInfo()",lPumpNum,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif	
	
	return S_OK;
}

/******************************************************************************
 Description:	Resets a qdx file info by setting allvalues to initial value 	
 Returns:      	void
 Parameters:   	long lFileNum - Qdx file number - see defines in DB.h
				long lPumpNum - Pump number to reset. 0 all pumps.
				long lFlags1 -  New defines in Pump.h
				long lFlags2	- for futeure use.
				long *pVal		- CtrlRetCode
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			27/3/2002   8:36		Start
******************************************************************************/

void CCtrl::ResetQdxInfo__(long lFileNum, long lPumpNum, long lFlags1, long lFlags2, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;

	
	//4.0.0.47 checking pump validity only in pums flag was send
	if(lFlags1 & RESET_PUMP_INFO)
	{
		if (PUMP_NOT_VALID(lPumpNum) && lPumpNum != RESET_ALL_PUMPS)
			retCode = CTRL_INVALID_PUMP_NUMBER;
	}
	

	if(retCode == CTRL_OK)
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("File:%d, Flags1:%d, Flags2:%d" ,lFileNum,lPumpNum,lFlags1,lFlags2);
			_LOGMSG.LogMsg("CCtrl","ResetQdxInfo",lPumpNum,LOG_PUMP,str);	
		}
		switch(lFileNum)
		{
			case PUMP_STAT_FILE:
			{
				_Module.m_server.ResetPumpStatQdxData(lPumpNum,lFlags1);				
				break;
			}
			default:
			{
				retCode = CTRL_NOT_SUPPORTED;
				break;
			}

		}
	
	}


	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"ResetQdxInfo",
				TRUE,retCode,
				FALSE,0);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::InitDebit(long lPumpGroup01, long lPumpGroup2, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_CTRL
	__try
	{
#endif

	InitDebit__(lPumpGroup01, lPumpGroup2, lFlags, pVal);		
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::InitDebit")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::InitDebit()", LOG_LEVEL_0);	

		exit(1);
	}
#endif
	return S_OK;
}

void CCtrl::InitDebit__(long lPumpGroup01, long lPumpGroup2, long lFlags, long *pVal)
{
	USES_CONVERSION;
	CtrlRetCode retCode = CTRL_OK;

	if(lPumpGroup01)
	{
		if(PUMP_NOT_VALID(lPumpGroup01))
			retCode = CTRL_INVALID_PUMP_NUMBER;
		if(PUMP_NOT_IN_WORKING_STATE(lPumpGroup01))
			retCode = CTRL_PUMP_NOT_IN_WORKING_STATE;		
	}

	// all pump
	
	if (retCode == CTRL_OK)
	{
		long lPumpNumber = lPumpGroup01; //4.0.3.58

		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		if (!lPumpGroup01) 
		{
			for (long i = 1; i <= _Module.m_server.GetLastValidPump(); i++)  //4.0.0.79
			{
				if(PUMP_NOT_VALID(i))
					retCode = CTRL_INVALID_PUMP_NUMBER;
				if(PUMP_NOT_IN_WORKING_STATE(i))
					retCode = CTRL_PUMP_NOT_IN_WORKING_STATE;
				else
					 _Module.m_server.m_cInitDebitHandler.SetStatus(i, DEBIT_SERVER_REQUEST_TO_START); //4.0.3.58
			}
		}
		else
		{
			if (_Module.m_server.m_cInitDebitHandler.GetStatus(lPumpNumber) == DEBIT_SERVER_IDLE)
			{
				_Module.m_server.m_cInitDebitHandler.SetStatus(lPumpNumber, DEBIT_SERVER_REQUEST_TO_START); //4.0.3.58
			}
			else
			{
				retCode = CTRL_IN_MIDDLE_OF_PROCESS;
				_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
			}
		}
	}

	if (pVal)
		*pVal = retCode;
}


/******************************************************************************
 Description:	Sets the controller software path and send event the 	
 Returns:      	void
 Parameters:   	BSTR sFileFullPath - Controller software full path.
				long  lFlags
				long *pVal - return value.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			9/5/2002   14:50		Start
******************************************************************************/

STDMETHODIMP CCtrl::UpdateControllerSoftware(BSTR sFileFullPath, long lFlags, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

	UpdateControllerSoftware__(sFileFullPath, lFlags, pVal);		
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::UpdateControllerSoftware")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::UpdateControllerSoftware()", LOG_LEVEL_0);	

		exit(1);
	}
#endif
	
	return S_OK;
}

void CCtrl::UpdateControllerSoftware__(BSTR sFileFullPath, long lFlags, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;
	char szFieldVal[MAX_FIELD_VAL];
	int iLen = 0;
	int iRet = 0;
	char szTableName[] = "GeneralParam";
	char szFieldName[] = "ControllerFileFullPath";
	long lKey1 = 0;
	long lKey2 = 0;
	short nFiledType = FIELD_SZ;
	long lVal = 0;
	DWORD dwLastErr	=0;
	
	if (SERVER_STATE_STARTUP)
		retCode = CTRL_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = CTRL_SERVER_NOT_READY;
	
	if(retCode == CTRL_OK)
	{
		memset(szFieldVal,0,sizeof(szFieldVal));
		iLen = WideCharToMultiByte(CP_ACP,0,sFileFullPath,-1,szFieldVal,sizeof(szFieldVal),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = CTRL_DEFAULT_FIELD_VAL_TOO_LONG;
		}
	}

	if(retCode == CTRL_OK)
	{
		long lRtc = _Module.m_server.m_cParam.SetParam(szTableName,lKey1,lKey2,szFieldName,nFiledType,szFieldVal,lVal,FALSE);
		if(lRtc)
		{
			switch(lRtc)
			{
				case 1: retCode = CTRL_FAILED_TO_OPEN_TABLE;break;		 
				case 2: retCode = CTRL_PARAM_NOT_FOUND;break;		 
			}			
		}
		else //O.K.
		{		
			CMD_STRUCT_UPDATE_PARAM cmdInfo;
			memset(&cmdInfo,0,sizeof(CMD_STRUCT_UPDATE_PARAM));
			strcpy(cmdInfo.szTableName,szTableName);			
			cmdInfo.lKey1 = lKey1;
			cmdInfo.lKey2 = lKey2;
			strcpy(cmdInfo.szFieldName,szFieldName);
			cmdInfo.nFieldType = nFiledType;
			strcpy(cmdInfo.szFieldVal,szFieldVal);
			cmdInfo.lVal = lVal;

			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,0,GCI_EVENT_UPDATE_CONTROLLER_SOFTWARE);
			
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("UpdateControllerSoftware");
				_LOGMSG.LogMsg(str);
			}
		}
	}
	
	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"UpdateControllerSoftware",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);

	if (pVal)
		*pVal = retCode;
}


STDMETHODIMP CCtrl::GetTankReading3(short nOpCode, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus1, short *nReadingFlags, long *lFuelLevel, long *lWaterLevel, long *lTemperature, long *lFuelVolume, long *lWaterVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *lStatus2, long *lFuelValue, long *lPricePerLitre, long *lTcVolume,long *lUllage, long *lFlags, BSTR *pRec, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_CTRL
	__try
	{
#endif

		GetTankReading3__(nOpCode, lIndexNumber, lTank, lProduct, lStatus1, nReadingFlags, lFuelLevel, lWaterLevel, lTemperature, lFuelVolume, lWaterVolume, lActiveReadNumber, lShiftNumber, sDateTime, lStatus2, lFuelValue, lPricePerLitre, lTcVolume, lUllage , lFlags, pRec, pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetTankReading3")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetTankReading3()", LOG_LEVEL_0);	

		exit(1);
	}
#endif
		
	return S_OK;
}

void CCtrl::GetTankReading3__(short nOpCode, long *lIndexNumber, long *lTank, long *lProduct, long *lStatus1, short *nReadingFlags, long *lFuelLevel, long *lWaterLevel, long *lTemperature, long *lFuelVolume, long *lWaterVolume, long *lActiveReadNumber, long *lShiftNumber, BSTR *sDateTime, long *lStatus2, long *lFuelValue, long *lPricePerLitre, long *lTcVolume, long *lUllage , long *lFlags, BSTR *pRec, long *pVal)
{
	USES_CONVERSION;

	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	TANK_READ_INFO info;
	memset(&info,0,sizeof(TANK_READ_INFO));
	
	switch(nOpCode)
	{
		case GET_DIRECT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ,&info);
			break;
		case GET_FIRST_INDEX:
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_FIRST,&info);
			break;
		case GET_NEXT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_NEXT,&info);
			break;
		default :
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_FIRST,&info);
			break;
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	
	if(retCode == CTRL_OK)
	{
		*lIndexNumber = info.lIndexNumber;
		*lTank = info.lTank;
		*lProduct = info.lProduct;
		*lStatus1 = info.lStatus;
		*nReadingFlags = info.nFlags;
		*lFuelLevel = info.lFuelLevel;
		*lWaterLevel = info.lWaterLevel;
		*lTemperature = info.lTemperature;
		*lFuelVolume = info.lFuelVolume;
		*lWaterVolume = info.lWaterVolume;
		*lActiveReadNumber = (long)info.dwActiveReadNumber; 
		*lShiftNumber = (long)info.dwShiftNumber; 
		*lStatus2 = info.lStatus2;
		*lFuelValue = info.lFuelValue;
		*lPricePerLitre = info.lPricePerLitre;
		*lTcVolume = info.lTcVolume;
		*lUllage = info.lUllage;


		SZ_TAG_SYSTEMTIME szTagTime;
		szTagTime.sNull = 0;

		SystemTime2Tag (&info.cSysTime, &szTagTime.sTime);
		CComBSTR sTime(sizeof(szTagTime.sTime), (LPCSTR)&szTagTime.sTime); // CID_185704 Out-of-bounds access

		*sDateTime = sTime.Copy();
	}

	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetTankReading3", //4.0.23.503
					TRUE,retCode,
					(lRtc!=0)? TRUE:FALSE,lRtc);
	}

	if(pVal != NULL)
		*pVal = retCode;
}


/******************************************************************************
 Description:	Trigger for GCI::Events to reload price poles data 	
 Returns:      	
 Parameters:   	long lFlags - for future use.
				long *pVal  - CtrlRetCodes.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			3/10/2002   17:57		Start 4.0.1.32	
******************************************************************************/
STDMETHODIMP CCtrl::ReloadPricePolesData(long lFlags, long *pVal)
{
	#ifdef CATCH_CTRL
	__try
	{
#endif

		ReloadPricePolesData__(lFlags,pVal);
		
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::ReloadPricePolesData")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::ReloadPricePolesData()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	
	return S_OK;
}



void CCtrl::ReloadPricePolesData__(long lFlags, long *pVal)
{
	USES_CONVERSION;

	CtrlRetCode retCode = CTRL_OK;	

	if (SERVER_STATE_STARTUP)
		retCode = CTRL_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = CTRL_SERVER_NOT_READY;
	
	if(retCode == CTRL_OK)
	{
		_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,0,GCI_EVENT_RELOAD_PRICE_POLES_DATA);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			_LOGMSG.LogMsg("CCtrl::ReloadPricePolesData()");
		}
	}

	if(pVal != NULL)
		*pVal = retCode;
}



/******************************************************************************
 Description:	Get number of devices per grade in forecourt. 	
 Returns:      	COM standard return code.
 Parameters:   	long lGrade - Number of the grade in common to all devices.
                long lNacsCode - Alternative grade identifier.
				long lDevice - Predefined number representing a device type
				               (cash acceptor, card reader, etc.).
				long lFlags - Reserved for future use, set to zero.
				long *pVal - Pointer to address where the total number of devices
				             configured for this grade will be stored.
				long *pRetVal - CTRL return value.
 Assumptions:   Data has already been gathered at  startup.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			6/10/2002   17:15		Start
******************************************************************************/

STDMETHODIMP CCtrl::GetPumpDevices(long lGrade, long lNacsCode, long lDevice, long lFlags, long *pVal, long *pRetVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

		GetPumpDevices__(lGrade , lNacsCode , lDevice , lFlags , pVal , pRetVal);
		
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetPumpDevices")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetPumpDevices()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

void CCtrl::GetPumpDevices__(long lGrade, long lNacsCode, long lDevice, long lFlags, long *pVal, long *pRetVal)
{
	long lRetVal = CTRL_OK , lTotalDevices = 0;
	long lGradeIndex = 0;
	long i = 0;
	
	if(!pVal)
		lRetVal = CTRL_FAILED_IO;
	
	if((lDevice >= (long)END_OF_LIST) || (lDevice < 0))
		lRetVal = CTRL_NOT_SUPPORTED;

	if(lRetVal == CTRL_OK)
	{
		if(lGrade)
		{
			lGradeIndex = lGrade - 1;
			lTotalDevices = (long)_Module.m_server.m_cDevicesPerGrade[lGradeIndex].GetNumberOfDevices((CDeviceRecordIndex)lDevice);
		}
		
		else 
		{
			if(lNacsCode)
			{
				long lNumOfMaxValidGrade = _Module.m_server.GetMaxValidGradeNumber();//amram 4.0.5.502
				for(i = 0;i < lNumOfMaxValidGrade;i++)
				{
					if(_Module.m_server.m_cDevicesPerGrade[i].GetNACSCode() == (DWORD)lNacsCode)
					{
						lGradeIndex = i;
						break;
					}
				}

				lTotalDevices = (long)_Module.m_server.m_cDevicesPerGrade[lGradeIndex].GetNumberOfDevices((CDeviceRecordIndex)lDevice);
			}
		
			//Both grade and NACS code are zero, so we return values for whole forecourt.
			else
			{
				for(i = 0;i < MAX_TERMINALS;i++)
					lTotalDevices += (long)_Module.m_server.m_cTerminalDevices[i].GetNumberOfDevices((CDeviceRecordIndex)lDevice);
			}
		}

		*pVal = lTotalDevices;
	}

	if(pRetVal)
		*pRetVal = lRetVal;

	CString sLogMsg;
	sLogMsg.Format("GetPumpDevices, called with lDevice = %d , lGrade  = %d , lNACSCode  = %d , found %d devices , returned %d" , lDevice , lGrade , lNacsCode , lTotalDevices , lRetVal);
	_LOGMSG.LogMsg(sLogMsg);

	return;
}


/******************************************************************************
 Description:	Get tank information gathered at last shift change. 	
 Returns:      	COM standard return code.
 Parameters:   	long lTankNumber - Tank number or zero for all tanks.
                BSTR * pRec - Pointer to BSTR where data will be stored
				long lFlags - Current shift info or last shift info
				              Last shift - 0x01
							  Current shift 0x02
				long *pRetVal - CTRL return value.
 Assumptions:   Data has already been gathered during shift change.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			8/10/2002   20:00		Start
******************************************************************************/

STDMETHODIMP CCtrl::GetTankInfo(long lTankNumber, BSTR *pRec, long lFlags, long *pRetVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

	GetTankInfo__(lTankNumber, pRec , lFlags , pRetVal);

#ifdef CATCH_CTRL
	}

	__except(_HANDLE_EXCEPTION("CCtrl::GetTankInfo")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetTankInfo()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

	return S_OK;
}

 void CCtrl::GetTankInfo__(long lTankNumber, BSTR *pRec, long lFlags, long *pRetVal)
{
	short nNACSCode = 0;
	long lRtc = 0;
	CTank cTmpTank;
	CShiftTanks cShiftTanks;

	long lRetVal = CTRL_OK;

	if(!lFlags)
		lRetVal = CTRL_FAILED_IO;

	if(_Module.m_server.IsShiftChangeInProgress())
	{
		lRetVal = CTRL_IN_MIDDLE_OF_PROCESS;
		_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
	}
		

	if(lRetVal == CTRL_OK)
	{
		if(lTankNumber)
		{
			lRtc = cShiftTanks.Read(lTankNumber , cTmpTank , &nNACSCode , lFlags);

			if(!lRtc)
			{
				TANK_READ_MEMO cTankInfo;
				memset((char*)&cTankInfo , ' ' , sizeof(cTankInfo));
				
				cTmpTank.GetTankTagInfo(&cTankInfo);

				BYTE sTmp[sizeof(TANK_READ_MEMO) + 2];
				Str2StrNull((unsigned char*)&cTankInfo , sizeof(TANK_READ_MEMO) , sTmp);
				CComBSTR bstrTankInfo(sizeof(sTmp) , (LPCSTR)sTmp);

				*pRec = bstrTankInfo.Copy();
			}

			else
				lRetVal = CTRL_PARAM_NOT_FOUND;
		}

		else  //All tanks
		{
			TANK_READ_MEMO cTmpTankArray[MAX_TANKS];
			memset((char*)&cTmpTankArray , ' ' , sizeof(cTmpTankArray));

			for(int i = 0;i < MAX_TANKS;i++)
			{
				lRtc = cShiftTanks.Read(i + 1 , cTmpTank , &nNACSCode , lFlags);

				if(!lRtc)
					cTmpTank.GetTankTagInfo(&(cTmpTankArray[i]));
				
				else
				{
					//If no records were found, return error
					if(!i)
						lRetVal = CTRL_PARAM_NOT_FOUND;

					break;
				}
			}

			BYTE sTmp[sizeof(cTmpTankArray) + 2];
			Str2StrNull((unsigned char*)cTmpTankArray , sizeof(cTmpTankArray) , sTmp);
			CComBSTR bstrTankInfo(sizeof(sTmp) , (LPCSTR)sTmp);

			*pRec = bstrTankInfo.Copy();
		}
	}

	if(pRetVal)
		*pRetVal = lRetVal;

	CString sLogMsg;
	sLogMsg.Format("GetTankInfo, lTankNumber = %d , QDEX read returned %d , method returned %d" , lTankNumber , lRtc , lRetVal);
	_LOGMSG.LogMsg(sLogMsg);

	return;
}

/******************************************************************************
 Description:	Get pump totals information gathered at last shift change. 	
 Returns:      	COM standard return code.
 Parameters:   	long lTankNumber - Tank number or zero for all tanks.
                BSTR * pRec - Pointer to BSTR where data will be stored
				long lFlags - Current shift info or last shift info
				              Last shift - 0x01
							  Current shift 0x02
				long *pRetVal - CTRL return value.
 Assumptions:   Data has already been gathered during shift change.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			8/10/2002   20:00		Start
******************************************************************************/

STDMETHODIMP CCtrl::GetPumpTotalsInfo(long lPumpNumber, long lFlags, BSTR *pRec, long *pRetVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

	GetPumpTotalInfo__(lPumpNumber , lFlags , pRec, pRetVal);

#ifdef CATCH_CTRL
	}

	__except(_HANDLE_EXCEPTION("CCtrl::GetPumpTotalsInfo")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetPumpTotalsInfo()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);	

		exit(1);
	}
#endif

	return S_OK;
}

void CCtrl::GetPumpTotalInfo__(long lPumpNumber, long lFlags, BSTR *pRec, long *pRetVal)
{ 
	CString str;
	long lRtc = 0;
	CShiftTotals cShiftTotals;

	BOOL bAtLeastOneNozzleConfigured = FALSE;

	long lRetVal = CTRL_OK;

	str.Format("CCtrl::GetPumpTotalInfo PumpNumber = %ld,lFlags= %ld",lPumpNumber,lFlags);
	_LOGMSG.LogMsg(str);

	if(!lFlags)
		lRetVal = CTRL_FAILED_IO;

	if(_Module.m_server.IsShiftChangeInProgress())
	{
		lRetVal = CTRL_IN_MIDDLE_OF_PROCESS;
		_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
	}
         
	if(lRetVal == CTRL_OK)
	{
		if(lFlags & MEMO_PUMP_TOTAL)
		{
			long lCounter = 0; // //4.0.15.500
			_Module.m_server.m_cCommonFunction.GetTotalPumpsInfo(lPumpNumber,&lCounter,pRec);
		}
		else
		{
			
			if(lPumpNumber)
			{
				TAG_SHIFT_TOTALS_REC cRecArray[MAX_POSITIONS_PER_PUMP];
				memset((char*)cRecArray , ' ' , sizeof(cRecArray));

				for(short nNozzle = 1;nNozzle <= MAX_POSITIONS_PER_PUMP;nNozzle++)
				{
					lRtc = cShiftTotals.Read(lPumpNumber , nNozzle , lFlags , cRecArray[nNozzle - 1]);
					
					if(!lRtc)
						bAtLeastOneNozzleConfigured = TRUE;
				}

				if(bAtLeastOneNozzleConfigured)
				{
					BYTE sTmp[sizeof(cRecArray) + 2];
					Str2StrNull((BYTE*)cRecArray , sizeof(cRecArray) , sTmp);
					CComBSTR bstrResult(sizeof(sTmp) , (LPCSTR)sTmp);

					*pRec = bstrResult.Copy();
				}

				else
					lRetVal = CTRL_PARAM_NOT_FOUND;
			}

			else  //All pumps
			{
				TAG_SHIFT_TOTALS_REC cRecArray[MAX_PUMPS_64][MAX_POSITIONS_PER_PUMP];
				memset((char*)cRecArray , ' ' , sizeof(cRecArray));

				for(long lPump = 0;lPump < MAX_PUMPS_64;lPump++)
				{
					for(short nNozzle = 0;nNozzle < MAX_POSITIONS_PER_PUMP;nNozzle++)
					{
						lRtc = cShiftTotals.Read(lPump + 1 , nNozzle + 1 , lFlags , cRecArray[lPump][nNozzle]);

						if(!lRtc)
							bAtLeastOneNozzleConfigured = TRUE;
					}
				}

				if(bAtLeastOneNozzleConfigured)
				{
					BYTE sTmp[sizeof(cRecArray) + 2];
					Str2StrNull((BYTE*)cRecArray , sizeof(cRecArray) , sTmp);
					CComBSTR bstrResult(sizeof(sTmp) , (LPCSTR)sTmp);

					*pRec = bstrResult.Copy();
				}

				else
					lRetVal = CTRL_PARAM_NOT_FOUND;
			}
		}
	}

	if(pRetVal)
		*pRetVal = lRetVal;

	CString sLogMsg;
	sLogMsg.Format("QDEX read returned %d , method returned %d" , lPumpNumber , lRtc , lRetVal);
	_LOGMSG.LogMsg("CCtrl","GetPumpTotalsInfo",lPumpNumber,LOG_PUMP,sLogMsg);	

	return;
}

/******************************************************************************
 Description:	Get tank delivery information gathered at last shift change. 	
 Returns:      	COM standard return code.
 Parameters:   	long lTankNumber - Tank number or zero for all tanks.
                BSTR * pRec - Pointer to BSTR where data will be stored
				long lFlags - Current shift info or last shift info
				              Last shift - 0x01
							  Current shift 0x02
				long *pRetVal - CTRL return value.
 Assumptions:   Data has already been gathered during shift change.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			8/10/2002   20:00		Start
******************************************************************************/

STDMETHODIMP CCtrl::GetDeliveryInfo(long lTankNumber, BSTR *pRec, long lFlags, long *pRetVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif

	GetDeliveryInfo__(lTankNumber, pRec, lFlags, pRetVal);

#ifdef CATCH_CTRL
	}

	__except(_HANDLE_EXCEPTION("CCtrl::GetDeliveryInfo")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetDeliveryInfo()", LOG_LEVEL_0);	

		exit(1);
	}
#endif

	return S_OK;
}


void CCtrl::GetDeliveryInfo__(long lTankNumber, BSTR *pRec, long lFlags, long *pRetVal)
{
	long lRtc = 0;
	CShiftDelivery cShiftDelivery;

	long lRetVal = CTRL_OK;

	if(!lFlags)
		lRetVal = CTRL_FAILED_IO;

	if(_Module.m_server.IsShiftChangeInProgress())
	{
		lRetVal = CTRL_IN_MIDDLE_OF_PROCESS;
		_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
	}

	if(lRetVal == CTRL_OK)
	{
		if(lTankNumber)
		{
			TAG_SHIFT_DELIVERY_REC cRec;
			memset((char*)&cRec , ' ' , sizeof(cRec));

			lRtc = cShiftDelivery.Read(lTankNumber , lFlags , cRec);

			if(lRtc)
				lRetVal = CTRL_PARAM_NOT_FOUND;
			
			else
			{
				BYTE sTmp[sizeof(cRec) + 2];
				Str2StrNull((BYTE*)&cRec , sizeof(cRec) , (BYTE*)sTmp);
				CComBSTR bstrResult(sizeof(sTmp) , (LPCSTR)sTmp);

				*pRec = bstrResult.Copy();
			}
		}

		else  //All tanks
		{
			TAG_SHIFT_DELIVERY_REC cRecArray[MAX_TANKS];
			memset((char*)cRecArray , ' ' , sizeof(cRecArray));

			BOOL bAtLeastOneTankConfigured = FALSE;

			for(long lTank = 0;lTank < MAX_TANKS;lTank++)
			{
				lRtc = cShiftDelivery.Read(lTank + 1 , lFlags , cRecArray[lTank]);

				if(!lRtc)
					bAtLeastOneTankConfigured = TRUE;
			}
			
			if(bAtLeastOneTankConfigured)
			{
				BYTE sTmp[sizeof(cRecArray) + 2];
				Str2StrNull((BYTE*)cRecArray , sizeof(cRecArray) , sTmp);
				CComBSTR bstrResult(sizeof(sTmp) , (LPCSTR)sTmp);

				*pRec = bstrResult.Copy();
			}

			else
				lRetVal = CTRL_PARAM_NOT_FOUND;
		}
	}

	if(pRetVal)
		*pRetVal = lRetVal;

	CString sLogMsg;
	sLogMsg.Format("GetDeliveryInfo, lTankNumber = %d , QDEX read returned %d , method returned %d" , lTankNumber , lRtc , lRetVal);
	_LOGMSG.LogMsg(sLogMsg);

	return;
}



void CCtrl::Reconfigure2__(long lTableId , long lSubTableId , long lSubTableIdExt , long lFlags , long * pRetVal)
{
	long lPumpNumber = 0 , lMask = 1;
	DWORD dwLogLevel = DEFAULT_LOG_LEVEL;
	BOOL bValidTable = TRUE;
	CtrlRetCode lRetCode = CTRL_OK;

	CRegTable cTable;
	CString sLogMsg("CCtrl::Reconfigure2, ") , sRetCode;

	CString cTableName;
	long lClientEventData = 0;

	if(!cTable.ReconfigInProgress())
	{
		cTable.SetReconfigInProgress((BYTE)TRUE);
		_Module.m_server.m_cCommonFunction.Reconfigure2(lTableId,lSubTableId,lSubTableIdExt,lFlags,pRetVal);
		cTable.SetReconfigInProgress((BYTE)FALSE);

		_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_RECONFIGURE,lTableId); //4.0.14.507
	}
	else
	{
		lRetCode = CTRL_IN_MIDDLE_OF_PROCESS;
		_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
	}

	if(pRetVal)
		*pRetVal = lRetCode;

	sRetCode.Format("return code: %d" , lRetCode);

	sLogMsg += sRetCode;

	_LOGMSG.LogMsg(sLogMsg);

	return;
}


STDMETHODIMP CCtrl::Reconfigure2(long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags, long *pRetVal)
{
#ifdef CATCH_CTRL

		__try
		{

#endif
		Reconfigure2__(lTableId , lSubTableId , lSubTableIdExt , lFlags , pRetVal);

#ifdef CATCH_CTRL
		}

		__except(_HANDLE_EXCEPTION("CCtrl::Reconfigure2")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::Reconfigure2()" , LOG_LEVEL_0);

			exit(1);
		}

#endif

	return S_OK;
}


/******************************************************************************
 Description:	Adding an offset for BOS, in some GPI returned codes, returned in
                the delivery emulation single tank process. The reason for this:

                In 1019: GPI_IN_MIDDLE_OF_QDX_MAINT = 200, so BOS expectes all returned 
				codes after it - to be larger than 200. Whereas in this version: 
				GPI_IN_MIDDLE_OF_QDX_MAINT was changed to 101. 
				
				So inner codes here will be above 100, and for BOS: an offset of 99 will 
				be added, so the outer codes will still continue to be above 200.


 Returns:      	long outer returned code
 Parameters:   	long inner returned code
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			30/11/2008   17:48		Start 4.0.21.500 TD 66935
******************************************************************************/
long CCtrl::GetGPLForDeliverySingleTank(const long lInnerRetCode)
{
	long lOutRetCode;
	long DeliverySingleTankOffset = 99;

/*	if (lInnerRetCode > 101)
		lOutRetCode = lInnerRetCode + DeliverySingleTankOffset;
	else
		lOutRetCode = lInnerRetCode;
*/
	//4.0.29.10
	switch(lInnerRetCode)
	{
		case GPI_DELIVERY_INVALID_REFERENCE_NUMBER:
		case GPI_DELIVERY_END_WITHOUT_START:
		case GPI_DELIVERY_WAITING_FOR_END:
		case GPI_DELIVERY_READING_PROCESS_DONE:
		case GPI_DELIVERY_ABORTING_IN_MIDDLE_OF_PROCESS:
		{
			lOutRetCode = lInnerRetCode + DeliverySingleTankOffset;
			break;
		}

		default:
		{
			lOutRetCode = lInnerRetCode;
			break;
		}

	}


	return lOutRetCode;
}

/******************************************************************************
 Description:	Handling Single Tank process Reguest, by activating the Single Tank process.
 Returns:      	long ret code
 Parameters:   	long lProcessType - The Single Tank process type. 
                CString sProcessString - The Single Tank process description.
                long BSTR	- XML of request info.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			03/11/2008   17:18		Start 4.0.21.500 TD 66935
******************************************************************************/
long CCtrl::HandleSingleTankReguest(const long lProcessType, const CString sProcessString, const BSTR sRec)
{
	long retCode = CTRL_OK;
	long lTankNumber = 0, lTankReference = 0;
	BOOL bLockConnectedPumps = FALSE;
	CString str;

	//In case its a PumpSrv delivery emulation process, we must be configured to work in a "Nozzle to Tank" setup:
	if (_Module.m_server.GetSupportDeliveryEmulation() == TRUE &&
		_Module.m_server.GetActiveTankConnectedPumpsTable() == FALSE)
	{				
		retCode = CTRL_FAILED_IO;
		if(LOG_BASIC_FLOW_CONTROL)
		{
			str.Format(" Pumpsrv delivery emulation single tank, CONFIGURATION NOT VALID, ret=%ld", retCode);
			str = "GeneralSystemRequest__, Cancel " + sProcessString + str;
			_LOGMSG.LogMsg(str);
		}	
	}

	else
	{
		// NO TD - First parse the xml , Second check for abort. 
		retCode = _Module.m_server.m_cCommonFunction.GetDeliveryData(sRec, lTankNumber, lTankReference, bLockConnectedPumps); 
		if(retCode == CTRL_OK)
		{
			if ((_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessStopped(lTankNumber) == SINGLE_TANK_PROCESS_STOPPED_BY_ABORT) || (_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessStopped(lTankNumber) == SINGLE_TANK_PROCESS_STOPPED_BY_TIMEOUT))
			{
				retCode = GPI_DELIVERY_ABORTING_IN_MIDDLE_OF_PROCESS;
				if(LOG_BASIC_FLOW_CONTROL)
				{
					str.Format(" Pumpsrv delivery emulation single tank, ABORT already in process, ret=%ld", retCode);
					str = "GeneralSystemRequest__, Cancel " + sProcessString + str;
					_LOGMSG.LogMsg(str);
				}
			}	
			else
			{
				str.Format(" delivery single tank request for tank = %ld, reference number = %ld", lTankNumber, lTankReference);
				str = "GeneralSystemRequest__, Begin handling " + sProcessString + str;
				_LOGMSG.LogMsg(str);

				//4.0.23.500 - TD 74964
				if (lProcessType == PD_SINGLE_TANK_CANCEL)
					retCode = HandleCancelDeliverySingleTankReguest(lTankNumber);
				else
					_Module.m_server.m_cCommonFunction.SetProcess(PROCESS_DELIVERY_SINGLE_TANK, lProcessType, lTankNumber, lTankReference, bLockConnectedPumps, &retCode);

				if (retCode == CTRL_OK)
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{
						str.Format(" delivery single tank for tank = %ld, reference number = %ld, ret = %ld",lTankNumber, lTankReference, retCode);
						str = "GeneralSystemRequest__, Got " + sProcessString + str;
						_LOGMSG.LogMsg(str);
					}
				}
			}
		}
	}

	return GetGPLForDeliverySingleTank(retCode);
}

/******************************************************************************
 Description:	Handle cancellation of delivery single tank	request
 Returns:      	Return code
 Parameters:   	long lTankNumber	- tank number
                 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			22/04/2009   11:09		Start	4.0.19.650 TD 74964
 ******************************************************************************/
long CCtrl::HandleCancelDeliverySingleTankReguest(long lTankNumber)
{
	long retCode = CTRL_OK;
	CString str;

	BYTE	byProcessType = _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessType(lTankNumber);
	BYTE	byProcessStatus = _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankStatus(lTankNumber);
	BYTE	byProcessStopped = _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessStopped(lTankNumber);

	if ((byProcessType == PD_SINGLE_TANK_END) && (byProcessStatus == DES_SINGLE_TANK_IDLE) && (byProcessStopped == SINGLE_TANK_PROCESS_NOT_STOPPED))
	{
		_Module.m_server.m_cCommonFunction.ResetDeliveryEmulationSingleTankProcessData(lTankNumber);
		if(LOG_BASIC_FLOW_CONTROL)
		{
			str.Format("CCtrl::HandleCancelDeliverySingleTankReguest, Cancel successful, ret=%ld", retCode);
			_LOGMSG.LogMsg(str);
		}
	}
	else if ((byProcessType == PD_SINGLE_TANK_NONE) && (byProcessStatus == DES_SINGLE_TANK_IDLE) && (byProcessStopped == SINGLE_TANK_PROCESS_NOT_STOPPED))
	{
		retCode = CTRL_DELIVERY_SIGLE_TANK_NOT_IN_PROCESS;
		if(LOG_BASIC_FLOW_CONTROL)
		{
			str.Format("CCtrl::HandleCancelDeliverySingleTankReguest, Pumpsrv delivery emulation single tank not in process, ret=%ld", retCode);
			_LOGMSG.LogMsg(str);
		}
	}
	else
	{
		retCode = CTRL_IN_MIDDLE_OF_DELIVERY_SIGLE_TANK;
		if(LOG_BASIC_FLOW_CONTROL)
		{
			str.Format("CCtrl::HandleCancelDeliverySingleTankReguest, Cannot cancel! PumpSrv delivery emulation single tank in process, ret=%ld", retCode);
			_LOGMSG.LogMsg(str);
		}
	}
	
	return retCode;
}

STDMETHODIMP CCtrl::GeneralSystemRequest(long lVal, BSTR sRec, long lFlags, long *plVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_CTRL

		__try
		{

#endif
			GeneralSystemRequest__(lVal, sRec, lFlags, plVal);
#ifdef CATCH_CTRL
		}

		__except(_HANDLE_EXCEPTION("CCtrl::GeneralSystemRequest")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::GeneralSystemRequest()" , LOG_LEVEL_0);

			exit(1);
		}

#endif

	
	return S_OK;
}

/******************************************************************************
 Description:	General system request process event	
 Returns:      	CtrlRetCode ret code
 Parameters:   	long lVal	- Future use
                long BSTR	- XML of request info
				long lFlags - Event request see table #150
				long *plVal - operation indication.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			06/02/2003   12:09		Start
 Shelly         08/10/2008   10:20      Add a support in GSR_START_DELIVERY_SINGLE_TANK and GSR_END_DELIVERY_SINGLE_TANK //4.0.21.500 TD 66935
 Shelly         03/11/2008   17:18		Add a support in GSR_ABORT_DELIVERY_SINGLE_TANK //4.0.21.500 TD 66935
******************************************************************************/
void CCtrl::GeneralSystemRequest__(long lVal, BSTR sRec, long lFlags, long *plVal)
{

	long retCode = CTRL_PARAM_NOT_FOUND;
	
	if (lFlags & GSR_SOFTWARE_REQUEST_VERSION)
	{
		retCode = _Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,0,GCI_EVENT_GET_VERSION);
	}

	if (lFlags & GSR_SEND_DPT_CONFIG_FILES)
	{
		retCode = _Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, lVal, GCI_EVENT_SEND_DPT_CONFIG_FILES);
	}
	
	if (lFlags & GSR_RESET_INI)
	{
		retCode = CTRL_OK;

		_Module.m_server.m_cIniFile.ResetIniParameters();

		if(LOG_BASIC_FLOW_CONTROL)
		{
			_LOGMSG.LogMsg("Set run time parameters to default value");
		}
	}

	if(lFlags & GSR_START_DELIVERY)		//4.0.13.504
	{
		retCode = CTRL_OK;

		//In case its a PumpSrv delivery emulation process, we must be configured to work in a "Nozzle to Tank" setup
		if (_Module.m_server.GetSupportDeliveryEmulation() == TRUE &&
			_Module.m_server.GetActiveTankConnectedPumpsTable() == FALSE)
		{				
			retCode = CTRL_FAILED_IO;
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("GeneralSystemRequest__, Cancel start PumpSrv delivery emulation, CONFIGURATION NOT VALID, invoice reference number=%ld, ret=%ld",lVal,retCode);
				_LOGMSG.LogMsg(str);
			}	
		}
		else
		{
			_Module.m_server.SetFoundDelivery(FALSE);
			_Module.m_server.m_cCommonFunction.SetProcess(PROCESS_DELIVERY, PD_START, 0, 0, 0, &retCode);

			if(retCode == CTRL_OK)	//4.0.19.270
			{					
				_Module.m_server.SetInvoiceRefNumber(lVal);
				_Module.m_server.m_cCommonFunction.ResetTanksReference(); //4.0.20.501 - TD 65174
				_Module.m_server.SetStartDeliveryTime();			

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("GeneralSystemRequest__, Got start delivery, invoice reference number=%ld, ret=%ld",lVal,retCode);
					_LOGMSG.LogMsg(str);
				}
			}
		}
		
	}

	if (lFlags & GSR_START_DELIVERY_SINGLE_TANK) //4.0.21.500 TD 66935
	{
		retCode = HandleSingleTankReguest(PD_SINGLE_TANK_START, "start", sRec);	
	}

	if (lFlags & GSR_ADD_DELIVERY_INVOICE_REF_NUMBER) //4.0.20.501 - TD 65174
	{
		retCode = CTRL_OK;

		//Let Invoice update only if Delivery process started successfully
		if(_Module.m_server.m_cServerInfo.GetDeliveryEmulationProcessType() != PD_NONE)
			retCode = _Module.m_server.m_cCommonFunction.ParseTanksDeliveryReferenceNumbers(sRec);		
		else
			retCode = CTRL_ADD_DELIVERY_INVOICE_REF_WITHOUT_STRAT;

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("GeneralSystemRequest__, Add delivery emulation invoice numbers, ret=%ld",retCode);
			_LOGMSG.LogMsg(str);
		}	
	}

	if (lFlags & GSR_ABORT_DELIVERY_SINGLE_TANK) //4.0.21.500 TD 66935
	{
		retCode = HandleSingleTankReguest(PD_SINGLE_TANK_FORCE_RELEASE, "abort", sRec);
	}

	if (lFlags & GSR_END_DELIVERY)
	{
		retCode = CTRL_OK;

		_Module.m_server.m_cCommonFunction.SetProcess(PROCESS_DELIVERY,PD_END,0, 0, 0, &retCode);
		_Module.m_server.SetInvoiceRefNumber(lVal);
		_Module.m_server.SetEndDeliveryTime();


		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("GeneralSystemRequest__, Got end delivery, invoice reference number=%ld, ret=%ld",lVal,retCode);
			_LOGMSG.LogMsg(str);
		}
	}

	if (lFlags & GSR_END_DELIVERY_SINGLE_TANK) //4.0.22.500 TD 66935
	{
		retCode = HandleSingleTankReguest(PD_SINGLE_TANK_END, "end", sRec);
	}

	if (lFlags & GSR_CANCEL_DELIVERY_SINGLE_TANK) //4.0.23.500 TD 74964
	{
		retCode = HandleSingleTankReguest(PD_SINGLE_TANK_CANCEL, "cancel", sRec);
	}

	//4.0.22.503 - TD 72189
	if (lFlags & GSR_CHANGE_PRODUCT_IN_TANK)
	{
		if (_Module.m_server.m_cServerInfo.GetChangeProductInTankState() != CPIT_IDLE)
			retCode = CTRL_IN_MIDDLE_CHANGE_PRODUCT_PROCESS;
		else
			retCode = _Module.m_server.m_cServerInfo.HandleChangeProductInTankRequest(sRec, FALSE);
	}

	//4.0.22.503 - TD 72189
	if (lFlags & GSR_CHANGE_PRODUCT_IN_TANK_SETUP_ONLY)
	{
		if (_Module.m_server.m_cServerInfo.GetChangeProductInTankState() != CPIT_IDLE)
			retCode = CTRL_IN_MIDDLE_CHANGE_PRODUCT_PROCESS;
		else
			retCode = _Module.m_server.m_cServerInfo.HandleChangeProductInTankRequest(sRec, TRUE);
	}

	if(plVal)
		*plVal = retCode;

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString sMsg;
		sMsg.Format("GeneralSystemRequest__, lFlags =%ld, retCode=%ld" , lFlags , retCode);
		_LOGMSG.LogMsg(sMsg);
	}


}


STDMETHODIMP CCtrl::GetTankMemoryData(long lTankNumber, BSTR *pRec, long lFlags, long *lCounter, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_CTRL

		__try
		{
#endif
			GetTankMemoryData__(lTankNumber, pRec, lFlags, lCounter  ,pVal);
#ifdef CATCH_CTRL
		}

		__except(_HANDLE_EXCEPTION("CCtrl::GetTankMemoryData")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::GetTankMemoryData()" , LOG_LEVEL_0);

			exit(1);
		}

#endif	

	return S_OK;
}
/******************************************************************************
 Description:	Get current tank details 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2003   14:00		Start
******************************************************************************/

void CCtrl::GetTankMemoryData__(long lTankNumber, BSTR *pRec, long lFlags, long *lCounter ,long *pVal)
{
	long lRetVal = CTRL_OK;

	if(!lFlags)
		lRetVal = CTRL_FAILED_IO;

	if(_Module.m_server.IsShiftChangeInProgress())
	{
		lRetVal = CTRL_IN_MIDDLE_OF_PROCESS;
		_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
	}

	if(lRetVal == CTRL_OK)
	{
		_Module.m_server.m_cCommonFunction.GetTankMemoInfo(lTankNumber,  pRec, lFlags , &lRetVal ,lCounter);		
	}

	if(lRetVal)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetTankMemoryData",
				TRUE,lRetVal,
				FALSE,0);

	if(pVal != NULL)
		*pVal = lRetVal;	
}

STDMETHODIMP CCtrl::DBTankGauge(long lOffset, BSTR *sRec, long lRecSize, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_CTRL
		__try
		{
#endif
			DBTankGauge__(lOffset, sRec,lRecSize, lFlags, pVal);
#ifdef CATCH_CTRL
		}

		__except(_HANDLE_EXCEPTION("CCtrl::DBTankGauge")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::DBTankGauge()" , LOG_LEVEL_0);

		}

#endif	
	return S_OK;
}

/******************************************************************************
 Description:	Tank gauge database operation 	
 Returns:      	CtrlRetCode
 Parameters:   	long lOffset = record offset 
				BSTR *sRec   = BSTR
				long lRecSize= BSTR size  
				long lFlags  = DataBase opetaion type (read, write)
				long *pVal   = CtrlRetCode
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			  5/2/2004   12:00		Start
 Marik			 27/06/2004  09:00      Use regular bstr.
*****************************************************************************/

void CCtrl::DBTankGauge__(long lOffset, BSTR *sRec, long lRecSize, long lFlags, long *pVal)
{

	DBTankGaugeRelativeIndex  cDBTankGaugeRelativeIndex;
	long lRtc = 0;
	CString sMsg;

	CtrlRetCode retCode = CTRL_OK;	


	if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = CTRL_FAILED_IO;
	else if ((sRec == NULL) || (lRecSize == 0))
		retCode = CTRL_RECORD_NOT_FOUND;

	if (retCode == CTRL_OK)
	{
		// 4.0.5.385
		long lTmpBufferLen =  (lRecSize * 2);
		BYTE *byBuff =  new BYTE [lTmpBufferLen +2];
		memset(byBuff , 0,lTmpBufferLen +2);
									
		cDBTankGaugeRelativeIndex.lOffSet =lOffset;
		cDBTankGaugeRelativeIndex.lSize  = lRecSize;
		memset(byBuff , 0,lTmpBufferLen + 2);


		if(LOG_FCC_FLOW_CONTROL)
		{
			
			sMsg.Format("DBTankGauge, input data lRecSize=%ld,lFlags=%ld" , lRecSize,lFlags );
			_LOGMSG.LogMsg(sMsg);

		}

		switch(lFlags)
		{
			case TGDB_READ:
			{
				
				if(LOG_FCC_FLOW_CONTROL)
				{										
					_LOGMSG.LogMsg("DBTankGauge before read DB tank");
				}
							
				lRtc = _Module.m_server.IndexOperation(DB_TANK_GAUGE_FILE, REC_READ  , byBuff, &cDBTankGaugeRelativeIndex);

				if(LOG_FCC_FLOW_CONTROL)
				{										
					_LOGMSG.LogMsg("DBTankGauge After read DB tank");					
				}

				// Check that buffer is valid as bstr
				for (int i=0; i<lRecSize;i++)
				{
					 if (byBuff[i] == 0x00)
						byBuff[i]='0';	
				}

				if(LOG_FCC_FLOW_CONTROL)
				{										
					_LOGMSG.LogMsg("DBTankGauge before free string");					
				}

				CComBSTR  sTmpBstr(lRecSize + 2  ,(LPCSTR)byBuff);

				SysFreeString(*sRec);									//4.0.18.501 merge from 4.0.15.285

				*sRec = sTmpBstr.Copy();

				sTmpBstr.Empty(); //4.0.15.285


				if(LOG_FCC_FLOW_CONTROL)
				{										
					sMsg.Format("DBTankGauge, allocate buffer in size=%d",sTmpBstr.Length()) ;
					_LOGMSG.LogMsg(sMsg);					
				}

			}
			break;			
			case TGDB_WRITE:
			{
				if (sRec)
				{
					int iLen = Bstr2Buffer(*sRec, (void *)byBuff,lRecSize);

					if(iLen == 0)
					{
						DWORD dwLastErr = GetLastError();
						retCode = CTRL_LAST_ERROR;
						lRtc = 0;
						// in case there with the bstr fill it space.
						memset(byBuff , ' ',lRecSize);


						if(LOG_BASIC_FLOW_CONTROL)
						{										
							sMsg.Format("DBTankGauge fail to parse buffer to write iLen=%d, last error=%ld" , iLen, dwLastErr) ;
							_LOGMSG.LogMsg(sMsg);					
						}

					}
					else
					{
						lRtc = _Module.m_server.IndexOperation(DB_TANK_GAUGE_FILE, REC_UPDATE , byBuff, &cDBTankGaugeRelativeIndex);	

						if(LOG_FCC_FLOW_CONTROL)
						{										
							sMsg.Format("DBTankGauge write buffer to qdx len=%ld, Rtc=%ld" ,lRecSize,lRtc) ;
							_LOGMSG.LogMsg(sMsg);					
						}
						
					}
					

					CComBSTR  sTmpBstr(lRecSize +2  ,(LPCSTR)byBuff);
					if (sRec)
						*sRec = sTmpBstr.Copy();

					if(LOG_FCC_FLOW_CONTROL)
					{																
						_LOGMSG.LogMsg("DBTankGauge send back buffer");					
					}

				}
				else
				{
					retCode = CTRL_PARAM_NOT_FOUND;
				}
			}
			break;
			default:
					retCode = CTRL_RECORD_NOT_FOUND;
				break;

		}

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("DBTankGauge, lOffset=%ld, Record size=%ld, lFlags=%ld,lRtc=%ld,retCode=%d " , lOffset , lRecSize , lFlags, lRtc , retCode);
			_LOGMSG.LogMsg(str);
		}


		if (lRtc)
			retCode = CTRL_RECORD_NOT_FOUND;

		// Free buffer 
		if (byBuff)
			delete []byBuff; 
	}


	if (pVal)
		*pVal = retCode;
	
}

STDMETHODIMP CCtrl::TankGaugeData(long lTankNumber, long lFlags, BSTR sRec, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_CTRL
		__try
		{
#endif
			TankGaugeData__(lTankNumber, lFlags, sRec, pVal);
#ifdef CATCH_CTRL
		}

		__except(_HANDLE_EXCEPTION("CCtrl::TankGaugeData")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::TankGaugeData()" , LOG_LEVEL_0);

			exit(1);
		}

#endif	

	return S_OK;
}
/******************************************************************************
 Description:	Tank gauge database operation 	
 Returns:      	CtrlRetCode
 Parameters:   	lTankNumber  - Tank number
				long lFlags  - Data type (Tankreading , delivery)
				sRec		 - Data information
				long *pVal   - CtrlRetCode
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			5/2/2004   13:00		Start
******************************************************************************/

void CCtrl::TankGaugeData__(long lTankNumber, long lFlags, BSTR sRec, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;	

	if (TANK_NOT_VALID(lTankNumber))
		retCode = CTRL_INVALID_TANK_NUMBER;
	else if (sRec == NULL)
	{
		retCode = CTRL_PARAM_NOT_FOUND;
	}

	if (retCode == CTRL_OK)
	{
		_bstr_t bstrTmp(sRec, true);

		switch(lFlags)
		{
			case TGD_TankReading:
			{
				CMD_STRUCT_TANK_GAUGE_EMULATION_READING cRec;
				memset(cRec.cTGDTankReading.sVolume , ' ' ,sizeof(cRec.cTGDTankReading.sVolume));
				cRec.lTankNumber= lTankNumber;
				cRec.lFlags = lFlags;
				memcpy(cRec.cTGDTankReading.sVolume, (const char *)bstrTmp , min(sizeof(cRec.cTGDTankReading.sVolume),bstrTmp.length()) );				//4.0.25.80 TD 142857

				_Module.m_server.SetCommand(CMD_TANK_GAUGE_EMULATION_READING, 
											SERVER_POS_NUMBER, 
											0L,
											&cRec,
											sizeof(CMD_STRUCT_TANK_GAUGE_EMULATION_READING));



			}
			break;
			case TGD_DeliveryReport:
			{
				CMD_STRUCT_TANK_GAUGE_EMULATION_DELIVERY cRec;
				memset(&cRec.cTGDDeliveryData , ' ' ,sizeof(cRec.cTGDDeliveryData));

				cRec.lTankNumber= lTankNumber;
				cRec.lFlags = lFlags;
				memcpy(&cRec.cTGDDeliveryData, (const char *)bstrTmp , min(sizeof(cRec.cTGDDeliveryData),bstrTmp.length()) );	//4.0.25.80 TD 142857

				_Module.m_server.SetCommand(CMD_TANK_GAUGE_EMULATION_DELIVERY, 
											SERVER_POS_NUMBER, 
											0L,
											&cRec,
											sizeof(CMD_STRUCT_TANK_GAUGE_EMULATION_DELIVERY));

			}

			break;
		}
			

	}

	if (pVal)
	 *pVal = retCode;		
}

STDMETHODIMP CCtrl::GetConnectivityStatus(BSTR *sPSConnStatus, long lFlags, long *pVal)
{
	#ifdef CATCH_CTRL
		__try
		{
#endif
			GetConnectivityStatus__(sPSConnStatus, lFlags, pVal);
#ifdef CATCH_CTRL
		}

		__except(_HANDLE_EXCEPTION("CCtrl::GetConnectivityStatus")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::GetConnectivityStatus()" , LOG_LEVEL_0);

			exit(1);
		}

#endif	

	return S_OK;
}
/******************************************************************************
 Description:	Retreived the components connectivity data 	
 Returns:      	CTRL return codes
 Parameters:   	BSTR *sPSConnStatus - PS_TAG_CONNECTIVITY_INFO structure.
				long lFlags - for future use.
		
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			23/2/2004   17:36		Start
******************************************************************************/
void CCtrl::GetConnectivityStatus__(BSTR *sPSConnStatus, long lFlags, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;	
	long	lRtc =0;
	
	PS_TAG_CONNECTIVITY_INFO cConnInfo;
	memset(&cConnInfo,' ', sizeof(PS_TAG_CONNECTIVITY_INFO));
	
	_Module.m_server.m_cCompConnectivity.GetCompConnectivityTagInfo(&cConnInfo);
	
	if (LOG_FCC_FLOW_CONTROL)
	{
		CString sMsg;	

		sMsg.Format( "PStoCL=%.2s,sCLtoPS=%.2s,sPStoQDEX=%.2s ,sPStoOLA=%.2s, sPStoAS=%.2s,sCLtoFC=%.2s,sCLtoGEN=%.2s,sGENtoCL%.2s, sCLtoBAP=%.2s,sBAPtoCL=%.2s\
			sCLtoTS=%.2s, sTStoCL=%.2s,	sBAPtoPMT=%.2s,	sPMTtoBAP=%.2s	,sGENtoXML=%.2s	,sRFStoQDEX=%.2s,sTStoTG=%.2s,sPMTtoBO=%.2s, sPMTtoOLA=%.2s,cPosInfo=%.42s",
			cConnInfo.sPStoCL,		
			cConnInfo.sCLtoPS,		
			cConnInfo.sPStoQDEX	,	
			cConnInfo.sPStoOLA,		
			cConnInfo.sPStoAS,		
			cConnInfo.sCLtoFC,		
			cConnInfo.sCLtoGEN,		
			cConnInfo.sGENtoCL,		
			cConnInfo.sCLtoBAP,		
			cConnInfo.sBAPtoCL,		
			cConnInfo.sCLtoTS,		
			cConnInfo.sTStoCL,		
			cConnInfo.sBAPtoPMT,		
			cConnInfo.sPMTtoBAP	,	
			cConnInfo.sGENtoXML	,	
			cConnInfo.sRFStoQDEX,	
			cConnInfo.sTStoTG,
			cConnInfo.sPMTtoBO,
			cConnInfo.sPMTtoOLA,
			cConnInfo.cPosInfo);
	_LOGMSG.LogMsg(sMsg);


	}

	constexpr  size_t struct_size = sizeof(PS_TAG_CONNECTIVITY_INFO);
	const unsigned char* pStruct = reinterpret_cast<unsigned char*>(&cConnInfo);
	*sPSConnStatus = Str2BSTR(pStruct, struct_size);

	if(pVal != NULL)
		*pVal = retCode;
}

STDMETHODIMP CCtrl::GetAccumulateTotals(long lShiftNumber ,  long lState, long lPosNumber, long lFlags,  long lVal, BSTR sStartTime, BSTR sEndTime, long *lRecordCount,   BSTR* sRec, long *pVal)
{
#ifdef CATCH_CTRL
		__try
		{
#endif
			GetAccumulateTotals__(lShiftNumber, lState,lPosNumber, lFlags, lVal,sStartTime, sEndTime, lRecordCount, sRec, pVal);
#ifdef CATCH_CTRL
		}

		__except(_HANDLE_EXCEPTION("CCtrl::GetConnectivityStatus")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::GetConnectivityStatus()" , LOG_LEVEL_0);

			exit(1);
		}

#endif	
	return S_OK;
}

void CCtrl::GetAccumulateTotals__(long lShiftNumber, long lState, long lPosNumber, long lFlags, long lVal, BSTR sStartTime, BSTR sEndTime, long *lRecordCount, BSTR *sRec, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;
	DWORD dwLastErr = 0;
	SYSTEMTIME cEndTime ,  cStartTime;
	long lRtc=0;


	
	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString sMsg;	
		sMsg.Format("Get accumulation report - lShiftNumber=%ld,lState=%ld,lPosNumber=%ld, lFlags=%ld,lVal=%ld",
			 lShiftNumber, lState, lPosNumber, lFlags, lVal);
			_LOGMSG.LogMsg(sMsg);
	}


	if (!ACCUMULATE_SHIFT_TOTAL)
		retCode = CTRL_ACCUMULATE_SHIFT_TOTAL_NOT_ACTIVE;
	else if (lFlags & GAT_BY_GRADE)
	{
		if((lVal > MAX_GRADES)||(lVal < 0))
			retCode = CTRL_INVALID_GRADE_NUMBER;
	}
	
	if (retCode == CTRL_OK)		
	{
		// if selected report by date, check date info. 
		if (lFlags & GAT_BY_DATE)
		{
			SZ_TAG_SYSTEMTIME szTagTime;
			memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
			int iLen = WideCharToMultiByte(CP_ACP,0,sStartTime,-1,(char *)&szTagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
			if(iLen == 0)
			{
				dwLastErr = GetLastError();
				retCode = CTRL_LAST_ERROR;
			}
			else
			{
				Tag2SystemTime(&szTagTime.sTime , &cStartTime);
			}
		
			if (retCode == CTRL_OK)
			{
				SZ_TAG_SYSTEMTIME szTagTime;
				memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
				int iLen = WideCharToMultiByte(CP_ACP,0,sEndTime,-1,(char *)&szTagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
				if(iLen == 0)
				{
					dwLastErr = GetLastError();
					retCode = CTRL_LAST_ERROR;
				}
				else
				{
					Tag2SystemTime(&szTagTime.sTime , &cEndTime);						
				}
			}			
		}

		if (retCode == CTRL_OK)
		{
			_Module.m_server.m_cCommonFunction.GetReport(lShiftNumber,lState,lPosNumber,lFlags,lVal ,cStartTime, cEndTime, lRecordCount, *sRec);					

			if (!(*lRecordCount))
				retCode = CTRL_RECORD_NOT_FOUND;
		}
	}

	
	if (LOG_LOGIC_ERROR)
	{
		CString sMsg;		
		sMsg.Format("Get accumulation report result - lShiftNumber=%ld,lState=%ld,lPosNumber=%ld, lFlags=%ld,lVal=%ld , total record=%ld",
			 lShiftNumber, lState, lPosNumber, lFlags, lVal, *lRecordCount);
			_LOGMSG.LogMsg(sMsg);
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetAccumulateTotals",
				TRUE,retCode,
				(lRtc!=0)? TRUE:FALSE,lRtc);
	if (pVal)
		 *pVal = retCode;		

}

STDMETHODIMP CCtrl::SetAccumulateState(long lShiftNumber, long lPosNumber,long lState, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_CTRL
		__try
		{
#endif
			SetAccumulateState__(lShiftNumber,lPosNumber, lState, lFlags, pVal);
#ifdef CATCH_CTRL
		}

		__except(_HANDLE_EXCEPTION("CCtrl::SetAccumulateState")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::SetAccumulateState()" , LOG_LEVEL_0);

			exit(1);
		}

#endif	

	return S_OK;
}
/******************************************************************************
 Description:	Signed report as used 
 Returns:      	CTRL return codes
 Parameters:   	long lShiftNumber, 
				long lState - set report  
				long lFlags - for future used, 				
		
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			02/03/2004   17:36		Start
******************************************************************************/

void CCtrl::SetAccumulateState__(long lShiftNumber, long lPosNumber, long lState, long lFlags, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;

	if (_Module.m_server.m_cCommonFunction.SetRecordStatus(lShiftNumber, lState, lPosNumber, lFlags))
		retCode = CTRL_RECORD_NOT_FOUND;

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetAccumulateState",
				TRUE,retCode,
				(0)? TRUE:FALSE,0);

	if (pVal)
		 *pVal = retCode;		

}


STDMETHODIMP CCtrl::GetGeneralData(long lRecordType, BSTR *sData, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here
#ifdef CATCH_CTRL
		__try
		{
#endif
			GetGeneralData__(lRecordType, sData, lFlags, pVal);
#ifdef CATCH_CTRL
		}
		__except(_HANDLE_EXCEPTION("CCtrl::GetGeneralData")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::GetGeneralData()" , LOG_LEVEL_0);

			exit(1);
		}
#endif	
	return S_OK;
}

void CCtrl::GetGeneralData__(long lRecordType, BSTR *sData, long lFlags, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;


	switch(lRecordType)
	{
		case PS_TYPE_SHIFT_SALES:		//4.0.19.508 
		{
			PS_SHIFT_SALES	sShiftSales;

			memset(&sShiftSales,' ',sizeof(sShiftSales));

			GetShiftSales(&sShiftSales,sData);
			break;
		}


		case PS_REC_TYPE_STARTUP:
		{
			PS_REC_STARTUP sRecStartUp;
			
			memset(&sRecStartUp,' ',sizeof(sRecStartUp));

			GetStartupRecord(&sRecStartUp,sData);

			break;
		}
		
		default:
			retCode = CTRL_RECORD_NOT_FOUND;	
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetGeneralData",
				TRUE,retCode,
				(0)? TRUE:FALSE,0);

	if (pVal)
		*pVal = retCode;	
}

//4.0.19.508 5. 
long CCtrl::GetShiftSales(PS_SHIFT_SALES *sRec,BSTR *sData)
{		
	long lRetVal = 0;
	DWORD	dwIncludeSalesFlag = 0;
	DWORD	dwActiveShiftNumber = 0;
	DWORD	dwPrevShiftLastTrsNum = 0;
	DWORD	dwTransactNumber = 0;
	
	memset(sRec, ' ', sizeof(PS_SHIFT_SALES));

	_Module.m_server.m_cParam.LoadParam("GeneralParam","ActiveShiftNumber",&dwActiveShiftNumber,0,TRUE,TRUE,_HOST.m_lGroupNum,0,TRUE); 
	l2a(dwActiveShiftNumber, sRec->sShiftNum,sizeof(sRec->sShiftNum));
	
	DWORD dwInitTrsNumber = _Module.m_server.GetFuelTransactionNumberMinValue(); //4.0.23.260 111318
	_Module.m_server.m_cParam.LoadParam("GeneralParam","TransactNumber",&dwTransactNumber,dwInitTrsNumber,TRUE,TRUE,_HOST.m_lGroupNum,0,TRUE); ////4.0.23.260 111318 was 0
	_Module.m_server.m_cParam.LoadParam("GeneralParam","PrevShiftLastTrsNum",&dwPrevShiftLastTrsNum,dwTransactNumber,TRUE,TRUE,_HOST.m_lGroupNum,0,TRUE); 

	sRec->byIncludeSalesFlag = ( dwTransactNumber - dwPrevShiftLastTrsNum != 0) ? 1 : 0;

	if(sData != NULL)
	{
		BYTE sTmp[sizeof(PS_SHIFT_SALES)+2];
		Str2StrNull((BYTE *)sRec,sizeof(PS_SHIFT_SALES),sTmp);
		CComBSTR strResult(sizeof(sTmp),(LPCSTR)sTmp); 
		*sData = strResult.Copy();

		if(LOG_BASIC_FLOW_CONTROL)
		{
			char	szDebug[MAX_STRING_LEN * 5];
			sprintf(szDebug, "%s %s","CCtrl::GetShiftSales - ", sTmp);
			_LOGMSG.LogMsg(szDebug, LOG_LEVEL_3);
		}
	}
	
	return lRetVal;
}


long CCtrl::GetStartupRecord(PS_REC_STARTUP *sRec,BSTR *sData)
{
	long lRetVal = 0;
	char tmpStr[HOME_DIR_STRING_LENGTH];
	char szPumpSrvPath[MAX_FIELD_NAME];

	memset(tmpStr, ' ' , sizeof(tmpStr));
	memset(szPumpSrvPath,' ' , sizeof(szPumpSrvPath));

	//Retrieving the current directory in order to instantiate the 
	//Home Directory with it in case it doesn't exist in the Registry
	GetCurrentDirectory(MAX_FIELD_NAME, szPumpSrvPath);

	_Module.m_server.m_cParam.LoadParam("Paths\\Long", "HomeDir", tmpStr, sizeof(tmpStr), szPumpSrvPath, FALSE);

	memcpy(sRec->sPSHomeDir, tmpStr, min(strlen(tmpStr), sizeof(sRec->sPSHomeDir)));

	//Getting the current PumpSrv version
				
	CString sVersion(_Module.m_server.m_strPumpSrvVersion);
	sVersion.Remove(' ');
	sVersion.Replace(',' ,'.');//4.0.6.509
	
	memcpy(sRec->sVersion, (LPCTSTR) sVersion,min(sizeof(sRec->sVersion),sVersion.GetLength()));
	
	CString sSiteID;
	sSiteID.Format("%07s",_Module.m_server.GetSiteID());

	memcpy(sRec->sSiteId, (LPCTSTR) sSiteID,min(sizeof(sRec->sSiteId),sSiteID.GetLength()));

	if(sData != NULL)
	{
		BYTE sTmp[sizeof(PS_REC_STARTUP)+2];
		Str2StrNull((BYTE *)sRec,sizeof(PS_REC_STARTUP),sTmp);
		CComBSTR strResult(sizeof(sTmp),(LPCSTR)sTmp); 
		*sData = strResult.Copy();
	}
	
	return lRetVal;
}

STDMETHODIMP CCtrl::SetServiceFeeMode(long lPumpNumber, long lFeeSet, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here

#ifdef CATCH_CTRL
		__try
		{
#endif
			SetServiceFeeMode__(lPumpNumber, lFeeSet,  lFlags, pVal);
#ifdef CATCH_CTRL
		}
		__except(_HANDLE_EXCEPTION("CCtrl::SetServiceFeeMode")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogClassMsg("CCtrl","SetServiceFeeMode()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);
	
		}
#endif	
	
	return S_OK;
}

void CCtrl::SetServiceFeeMode__(long lPumpNumber, long lFeeNumber, long lFlags, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;
	if(lPumpNumber !=0)
	{
		if(PUMP_NOT_VALID(lPumpNumber))
			retCode = CTRL_INVALID_PUMP_NUMBER;
	}

	if (lFeeNumber !=0)
	{
		if(SERVICE_FEE_NOT_VALID(lFeeNumber))
			retCode = CTRL_INVALID_SERVICE_FEE_NUMBER;
	}


	if (retCode == CTRL_OK)
	{
		_Module.m_server.m_cParam.SaveParam("ServiceFee", "ManualServiceFee",(DWORD)lFeeNumber,TRUE); //4.0.8.503
		_Module.m_server.SetManualServiceFee(lFeeNumber);
		
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("lFeeSet=%ld,lFlags=%ld, Ret=%d", lPumpNumber,lFeeNumber, lFlags ,retCode);
		_LOGMSG.LogMsg("CCtrl","SetServiceFeeMode",lPumpNumber,LOG_PUMP,str);
	}



	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetServiceFeeMode",
				TRUE,retCode,
				(0)? TRUE:FALSE,0);

	if (pVal)
		*pVal = retCode;	

}



STDMETHODIMP CCtrl::GetServiceFeeMode(long lPumpNumber, long *lFeeNumber, long lFlags, long *pVal)
{
	// TODO: Add your implementation code here
	

#ifdef CATCH_CTRL
		__try
		{
#endif
			GetServiceFeeMode__(lPumpNumber, lFeeNumber,  lFlags, pVal);
#ifdef CATCH_CTRL
		}
		__except(_HANDLE_EXCEPTION("CCtrl::GetServiceFeeMode")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogClassMsg("CCtrl","GetServiceFeeMode()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);
	
		}
#endif	
	

	return S_OK;
}

void CCtrl::GetServiceFeeMode__(long lPumpNumber, long *lFeeNumber, long lFlags, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;
	if(lPumpNumber !=0)
	{
		if(PUMP_NOT_VALID(lPumpNumber))
			retCode = CTRL_INVALID_PUMP_NUMBER;
	}

	if (lFeeNumber == NULL)
	{
		retCode = CTRL_PARAM_NOT_FOUND;
	}

	if (retCode == CTRL_OK)
	{		
		*lFeeNumber = _Module.m_server.GetManualServiceFee();		
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetServiceFeeMode",
				TRUE,retCode,
				(0)? TRUE:FALSE,0);

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetServiceFeeMode",
				TRUE,retCode,
				(0)? TRUE:FALSE,0);

	if (pVal)
		*pVal = retCode;	

}



/******************************************************************************
 Description:	Return XML that contain the pci param 
 Returns:      	CTRL return codes
 Parameters:   
				 
								
		
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			02/03/2004   17:36		Start
******************************************************************************/
STDMETHODIMP CCtrl::InitParam(long lFlags, BSTR *  sXmlData, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;
	BOOL bRetCode = TRUE;
	int	 i = 0;
	CXMLInterface xmlInitDoc;
	CXMLPointers  pCurrElement,pCurrElement2;
	CComBSTR	  bstrXML;

	long lPciCom = _Module.m_server.m_cProtectedDataHandler.GetPCIActiveFlag();			//4.0.22.501 TD 72716


	try
	{
		bRetCode = xmlInitDoc.InitDoc();
		xmlInitDoc.CreateElement("PARAMDATA",&pCurrElement);
		xmlInitDoc.AppendRootElement(&pCurrElement);
		xmlInitDoc.SetIntAttribute("PCICompliant",lPciCom,&pCurrElement);
		
	}
	catch(...)
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::InitParam()", LOG_LEVEL_0);
	}
	xmlInitDoc.GetXmlString(sXmlData);
	if (pVal)
		*pVal = retCode;
	xmlInitDoc.CloseDoc();
	return S_OK;
}

STDMETHODIMP CCtrl::GetPaidTrs(long lClientId, long lFlags, BSTR *pRec, long *lCountRec, long *plRetVal)
{
		// TODO: Add your implementation code here
	

#ifdef CATCH_CTRL
		__try
		{
#endif
			GetPaidTrs__(lClientId, lFlags,  pRec, lCountRec,plRetVal);
#ifdef CATCH_CTRL
		}
		__except(_HANDLE_EXCEPTION("CCtrl::GetPaidTrs")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogClassMsg("CCtrl","GetPaidTrs()",lClientId,LOG_PUMP,"",LOG_LEVEL_0);
	
		}
#endif	
	

	return S_OK;

}


void CCtrl::GetPaidTrs__(long lClientId, long lFlags, BSTR *pRec, long *lCountRec, long *plRetVal)
{
	CtrlRetCode retCode = CTRL_OK;
	

	if (retCode == CTRL_OK)
	{		
		_Module.m_server.m_cCommonFunction.GetTransListQuery(lClientId,pRec,lCountRec,&lFlags);
		
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"GetPaidTrs__",
				TRUE,retCode,
				(0)? TRUE:FALSE,0);

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetServiceFeeMode",
				TRUE,retCode,
				(0)? TRUE:FALSE,0);

	if (plRetVal)
		*plRetVal = retCode;	


}

STDMETHODIMP CCtrl::GetPumpTotalsMemoData(long lPumpNumber, long lFlags, BSTR *pRec, long *lCounter, long *pRetVal)
{
// TODO: Add your implementation code here


#ifdef CATCH_CTRL
		__try
		{
#endif
			GetPumpTotalsMemoData__(lPumpNumber, lFlags,pRec,lCounter,pRetVal);
#ifdef CATCH_CTRL
		}
		__except(_HANDLE_EXCEPTION("CCtrl::GetPumpTotalsMemoData")) // 4.0.15.460
		{
			if(LOG_ERROR)
				_LOGMSG.LogClassMsg("CCtrl","GetServiceFeeMode()",lPumpNumber,LOG_PUMP,"",LOG_LEVEL_0);
	
		}
#endif	
	

	return S_OK;
}


void CCtrl::GetPumpTotalsMemoData__(long lPumpNumber,long lFlags,BSTR * pRec,long *lCounter,long * pRetVal)
{
	CString str;
	long lRtc = 0;
	CShiftTotals cShiftTotals;


	long lRetVal = CTRL_OK;
	*lCounter=0;

	str.Format("CCtrl::GetPumpTotalsMemoData PumpNumber = %ld,lFlags= %ld",lPumpNumber,lFlags);
	_LOGMSG.LogMsg(str);

	if(_Module.m_server.IsShiftChangeInProgress())
	{
		lRetVal = CTRL_IN_MIDDLE_OF_PROCESS;
		_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
	}
         
	if(lRetVal == CTRL_OK)
	{
		_Module.m_server.m_cCommonFunction.GetTotalPumpsInfo(lPumpNumber,lCounter,pRec);
	}

	*pRetVal = lRetVal;

	
}

//4.0.12.512
STDMETHODIMP CCtrl::GetReceipt2(long lPosNumber, BSTR sTrack2, long lIndex, BSTR *sReceiptData, long lFlags, long *pVal)
{

	USES_CONVERSION; 

	if(sTrack2 == NULL)
	{
		if(LOG_ERROR)
			_LOGMSG.LogMsg2(
			"CCtrl","GetReceipt2",lPosNumber,LOG_POS,"GetReceipt2, Throw COM Exception : E_POINTER", LOG_LEVEL_0);	

		return E_POINTER;
	}
	
#ifdef CATCH_CTRL
	__try
	{
#endif
		GetReceipt2__(lPosNumber, sTrack2, lIndex, sReceiptData, lFlags, pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetReceipt2")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetReceipt2()",lPosNumber,LOG_POS,"",LOG_LEVEL_0);

		exit(1);
	}		
#endif 

	return S_OK;
}

//4.0.12.512
void CCtrl::GetReceipt2__(long lPosNumber, BSTR sTrack2, long lIndex, BSTR *sReceiptData, long lFlags, long *pVal)
{

	long lRtc = 0; 

	_Module.m_server.m_cCommonFunction.GetReceipt2(lPosNumber,sTrack2,lIndex,sReceiptData,lFlags,lPosNumber,&lRtc);

	if (lRtc)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				lPosNumber, // Src
				"IGPI", //to handle return code of CGPI that returned (not ctrl)
				"GetReceipt2",
				TRUE,lRtc,
				FALSE,0);


	if(pVal != NULL)
		*pVal = lRtc;

}



//4.0.13.503
STDMETHODIMP CCtrl::GetTankReadingEx(long lOpCode, long *lIndexNumber, long lTankNumber, long lFlags, BSTR *pRec, long *pVal)
{

#ifdef CATCH_CTRL
		__try
		{
#endif

			GetTankReadingEx__(lOpCode,lIndexNumber,lTankNumber,lFlags,pRec,pVal);
#ifdef CATCH_CTRL
		}
		__except(_HANDLE_EXCEPTION("CCtrl::GetTankReadingEx")) // 4.0.15.460
		{
			// we arrive here on error only !
			if(LOG_ERROR)
				_LOGMSG.LogMsg("CCtrl::GetTankReadingEx()", LOG_LEVEL_0);	

		}
#endif
		
	return S_OK;
}

//4.0.13.503
void CCtrl::GetTankReadingEx__(long lOpCode, long *lIndexNumber, long lTankNumber, long lFlags, BSTR *pRec, long *pVal)
{
	USES_CONVERSION;

	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	TANK_READ_INFO info;
	memset(&info,0,sizeof(TANK_READ_INFO));
	
	switch(lOpCode)
	{
		case GET_DIRECT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ,&info);
			break;
		case GET_FIRST_INDEX:
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_FIRST,&info);
			break;
		case GET_NEXT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_NEXT,&info);
			break;
		default :
			lRtc = _Module.m_server.IndexOperation(TANK_READS_FILE,REC_READ_FIRST,&info);
			break;
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}

	if(retCode == CTRL_OK)
	{

		*lIndexNumber = info.lIndexNumber;

		_Module.m_server.m_cTankXML.TankReadingInfo2Xml(&info, pRec);

	}

	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetTankReadingEx", //4.0.23.503
					TRUE,retCode,
					(lRtc!=0)? TRUE:FALSE,lRtc);
	}

	if(pVal != NULL)
		*pVal = retCode;
}


STDMETHODIMP CCtrl::GetDeliveryReportEx(long lOpCode, long *lIndexNumber, long lFlags, BSTR *pRec, long *pVal)
{


#ifdef CATCH_CTRL
	__try
	{
#endif

		GetDeliveryReportEx__(lOpCode,lIndexNumber,lFlags,pRec,pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetDeliveryReportEx")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetDeliveryReportEx()", LOG_LEVEL_0);	

		exit(1);
	}
#endif	

	return S_OK;
}

//4.0.13.504
void CCtrl::GetDeliveryReportEx__(long lOpCode, long *lIndexNumber, long lFlags, BSTR *pRec, long *pVal)
{

	CtrlRetCode retCode = CTRL_OK;	
	long lRtc = OK;
	DELIVERY_INFO info;
	memset(&info,0,sizeof(DELIVERY_INFO));
	
	switch(lOpCode)
	{
		case GET_DIRECT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ,&info);
			break;
		case GET_FIRST_INDEX:
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ_FIRST,&info);
			break;
		case GET_NEXT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ_NEXT,&info);
			break;
		default :
			lRtc = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE,REC_READ_FIRST,&info);
			break;
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	
	if(retCode == CTRL_OK)
	{
		*lIndexNumber = info.lIndexNumber; //4.0.13.508

		_Module.m_server.m_cTankXML.DeliveryInfo2Xml(&info, pRec);
	}
	
	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetDeliveryReportEx",
					TRUE,retCode,
					(lRtc!=0)? TRUE:FALSE,lRtc);
	}

	if(pVal != NULL)
		*pVal = retCode;
	
}


/******************************************************************************
 Description: This method goes all over the valid pumps	and check if the current mode
			  of the pump is manned or unmanned. Only If ALL the pumps are currently on
			  unmanned mode it will return TRUE.
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			4/6/2007   10:42		Start //4.0.15.161 TD 44867
******************************************************************************/

STDMETHODIMP CCtrl::IsUnmanned(BOOL * pbUnmanned, long *pVal)
{
	BOOL bIsUnmanned = TRUE;
	long p;
	long lMode, lTmpMode;
	long lLastPump = _Module.m_server.GetLastValidPump();
	long lRetVal = CTRL_OK;

	for(p = 0; p < lLastPump;p++)
	{
		CString str , sTable("Pumps\\Pump");
		char sNumber[8];
		sprintf(sNumber,"%02d",p+1);
		str = sTable + sNumber;
		DWORD dwPumpValid = 0;
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwPumpValid,(DWORD)1,FALSE);
		
		lMode = _Module.m_server.m_lPumpsManualModeArray[p];
			
		if (lMode) 
			lTmpMode = lMode;  
		else
			lTmpMode = _Module.m_server.m_cPumpArray[p].GetStat().GetMode();

		if (dwPumpValid)
		{
			if ((lTmpMode)  &&  (lTmpMode <= MAX_MODES))
			{
				if(!(_Module.m_server.m_cModeArray[lTmpMode-1].IsUnmannedMode()))
				{
					bIsUnmanned = FALSE;
					
					if(LOG_FCC_FLOW_CONTROL)
					{
						CString str;
						str.Format("CCtrl::IsUnmanned, Pump = %ld, Mode = %ld, bIsUnmanned = FALSE Brake the loop.",p+1,lTmpMode);
						_LOGMSG.LogMsg(str);
					}
					break;
				}
				else
				{
					if(LOG_FCC_FLOW_CONTROL)
					{
						CString str;
						str.Format("CCtrl::IsUnmanned, Pump = %ld, Mode = %ld, bIsUnmanned = TRUE ",p+1,lTmpMode);
						_LOGMSG.LogMsg(str);
					}
				}
			}
		}
		else
		{
			if(LOG_FCC_FLOW_CONTROL)
			{
				CString str;
				str.Format("CCtrl::IsUnmanned, Pump = %ld, Mode = %ld, Pump is not valid.",p+1,lTmpMode);
				_LOGMSG.LogMsg(str);
			}	
		}
	}

	if (pbUnmanned)
		*pbUnmanned = bIsUnmanned;

	if (pVal)
		*pVal = lRetVal;

	return S_OK;
}


STDMETHODIMP CCtrl::GetVersion(BSTR *sVersionList, long lFlags, long *pVal)
{
#ifdef CATCH_CTRL
	__try
	{
#endif
		GetVersion__(sVersionList, lFlags, pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetVersion")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetVersion()",0,LOG_NONE,"",LOG_LEVEL_0);	

		exit(1);
	}   		
#endif 

	return S_OK;
}

void CCtrl::GetVersion__(BSTR *sVersionList, long lFlags, long *pVal)
{		
	CtrlRetCode retCode = CTRL_OK;

	if (sVersionList != NULL)
	{
		if (_Module.m_server.m_sDevicesVersions) // 464867
			*sVersionList = _Module.m_server.m_sDevicesVersions.Copy();
		else
		{
			*sVersionList = SysAllocString(L"  ");
			_Module.m_server.m_cLogger.LogMsg("CCtrl::GetVersion - No version to send!");
		}
	}
	
	if(pVal != NULL)
		*pVal = retCode;

}


STDMETHODIMP CCtrl::SetupEx(BSTR sXmlRec, long lFlags, long *pVal) //4.0.16.500 - CR 34815
{
	#ifdef CATCH_CTRL
	__try
	{
#endif
		SetupEx__(sXmlRec, lFlags, pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::SetupEx")) // 4.0.15.460
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","SetupEx()",0,LOG_NONE,"",LOG_LEVEL_0);	

		exit(1);
	}   		
#endif 

	return S_OK;
}


/******************************************************************************
 Description:	The method gets BSTR in XML format and updates the information
				(Car wash/Grades/Taxes) in the registry
 Returns:      	CTRL return codes
 Parameters:	BSTR sXmlRec	- string in XML format, which contain data on Car wash/Grades/Taxes
				long lFlags		- flags
				long *pVal		- CTRL ret code 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			17/01/2007   17:00		Start
******************************************************************************/
void CCtrl::SetupEx__(BSTR sXmlRec, long lFlags, long *pVal) //4.0.16.500 - CR 34815
{
	CtrlRetCode retCodeCarWash	= CTRL_OK,
				retCodeTaxes	= CTRL_OK,
				retCodeGrades	= CTRL_OK,
				retCode			= CTRL_OK;
	BOOL bRetCode;
	
	char sXmlBuffer[MAX_XML_BUF_LEN];
	memset(sXmlBuffer, 0, sizeof(sXmlBuffer));

	int iLen = Bstr2Buffer(sXmlRec, (char *)sXmlBuffer, MAX_XML_BUF_LEN);
	if(iLen == 0)
	{
		retCode = CTRL_RECORD_NOT_FOUND;
	}
	else
	{
		switch(lFlags)
		{
		case SETUP_EX_FUEL_GENERAL_CONFIGURATION:											//4.0.22.45 TD 72716
			{
				if( _Module.m_server.SetConfiguratonThreadData((BYTE*)sXmlBuffer, iLen) != TRUE )
				{
					retCode = CTRL_IN_MIDDLE_OF_PROCESS;
				}
			}break;
		case 0:
			{			
				CXMLInterface	xmlDoc;
				CXMLPointers	cAllRecordsList, cRecordElement;

				try
				{
					if (xmlDoc.InitDoc())
					{
						bRetCode = xmlDoc.LoadXmlString(sXmlBuffer);

						if (bRetCode)
						{
							// Get all records
							xmlDoc.GetChildNodesList(NULL,&cAllRecordsList);
							
							// Go through the records (check Carwash/Taxes/Grades)
							while(xmlDoc.GetNextItem(cAllRecordsList, cRecordElement))
							{
								CString sName;
								xmlDoc.GetName(sName, &cRecordElement);
								
								if (!strcmp(sName, ELEMENT_CARWASH))
								{
									retCodeCarWash = _Module.m_server.m_cCommonFunction.SetCarWashInfo(&xmlDoc, &cRecordElement);
								}
								else if(!strcmp(sName, ELEMENT_TAXES))
								{
									//retCodeTaxes
								}
								else if(!strcmp(sName, ELEMENT_GRADES))
								{
									//retCodeGrades
								}
							}

							// Check return codes
							if (retCodeCarWash || retCodeTaxes || retCodeGrades)
							{						
								CString sLogMsg("SetupEx failed to set info for: ");

								if (retCodeCarWash != CTRL_OK)
								{
									retCode = retCodeCarWash;
									sLogMsg += "Car Wash, ";
								}
								if (retCodeTaxes != CTRL_OK)
								{
									retCode = retCodeTaxes;
									sLogMsg += "Taxes, ";
								}
								if (retCodeGrades != CTRL_OK)
								{
									retCode = retCodeGrades;
									sLogMsg += "Grades, ";
								}

								_LOGMSG.LogMsg(sLogMsg,LOG_LEVEL_1);

							}
						}
					}

				}
				catch(...)
				{
					_LOGMSG.LogMsg("CCtrl","SetupEx__", 0, LOG_NONE, "Unexpected Error occurred");
				}
				xmlDoc.CloseDoc();
			}
		}
	}
	

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetupEx",
				TRUE,retCode,
				FALSE,0);

	
	if(pVal != NULL)
		*pVal = retCode;

}

STDMETHODIMP CCtrl::StartQdxMaintenance(long lFileNum, long *pVal)
{
	#ifdef CATCH_CTRL
	__try
	{
#endif
		StartQdxMaintenance_(lFileNum, pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::StartQdxMaintenance")) 
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","StartQdxMaintenance()",0,LOG_NONE,"",LOG_LEVEL_0);	

		exit(1);
	}   		
#endif 

	return S_OK;
}

/******************************************************************************
 Description:	
				
 Returns:      	CTRL return codes
 Parameters:	long lFileNum		- qdxNum
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			20/09/2007   16:00		Start
******************************************************************************/
void CCtrl::StartQdxMaintenance_(long lFileNum, long *pVal) //4.0.16.500 - CR 34815
{
	//BOOL retCode = FALSE;
	//retCode = _Module.m_server.StartQdxMaintenance(lFileNum);

	//if(retCode)
	//	if(LOG_LOGIC_ERROR)
	//		_LOGMSG.LogResult(
	//			0, // Src
	//			"ICTRL",
	//			"StartQdxMaintenance",
	//			TRUE,retCode,
	//			FALSE,0);

	//
	//if(pVal != NULL)
	//	*pVal = retCode;

}
/******************************************************************************
 Description:	Set grade price with time directly to convert layer.
				PumpSrv does NOT change the grades in the ini file.
				The convert layer will do the actual price change when the given time comes.
				It is the responsibility of the Cl2PumpSrv to change PumpSrv's grades. (by unsing SetParam4)
				If the date is not valid , The current time will be taken and sent to Cl2PumpSrv.
				
 Returns:      	CTRL return codes
 Parameters:	PumpNumber (=0),Nozzle,Grade,Mode,Level,Price,Time,Flags,lTaxRate,DiscountPerUnit,MaxUnitForDiscount
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH			14/02/2008   16:00		Start - TD 58094
 AmitH			26/02/2008   10:00	    Change the method name from SetGradePriceWithTime to SetGradePrice2Direct
******************************************************************************/
void CCtrl::SetGradePrice2Direct__(long lPumpNumber, short nNozzle, short nGrade, long lMode, short nLevel, long lPrice, BSTR sTimeValid, short nFlags, long lTaxRate, long lDiscountPerUnit, long lMaxUnitForDiscount, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;
	DWORD dwLastErr	=0;

	if((nGrade > MAX_GRADES_99)||(nGrade <= 0))

	if (!(nFlags & PRICE_NO_PRICE_CHANGE)) 
	{
		if (retCode == CTRL_OK) 
		{
			if (_Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags()||
				_Module.m_server.m_cServerInfo.GetRequiredPriceChangeFlags()||
				_Module.m_server.m_cServerInfo.GetRequiredModeProcessFlags() ||
				_Module.m_server.m_cServerInfo.GetRequiredMaintenanceProcessFlags() ||
				_Module.m_server.m_cServerInfo.GetInPriceChangeProcess()) 	
			{
				retCode = CTRL_IN_MIDDLE_OF_PROCESS;
				_LOGMSG.LogMiddleOfProcessReason();
			}
		}
	}

	if(retCode == CTRL_OK)
	{
	
		SZ_TAG_SYSTEMTIME szTagTime;
		memset(&szTagTime,0,sizeof(SZ_TAG_SYSTEMTIME));
		int iLen = WideCharToMultiByte(CP_ACP,0,sTimeValid,-1,(char *)&szTagTime,sizeof(SZ_TAG_SYSTEMTIME),NULL,NULL);
		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			retCode = CTRL_LAST_ERROR;
		}
			
		if(retCode == CTRL_OK)
		{
			if (nFlags & MAX_UNIT_IS_BATCH_NUM)
			{
				_Module.m_server.m_cParam.SaveParam("GeneralParam", "PriceChangeBatchNumber", (DWORD)lMaxUnitForDiscount, TRUE, 0, 0, 0, TRUE);
				_Module.m_server.SetPriceChangeBatchNumber(lMaxUnitForDiscount);
			}

			// check if empty date ...
			if( ((szTagTime.sTime.sMonth[0] == ' ')&&(szTagTime.sTime.sMonth[1] == ' ')) ||
				((szTagTime.sTime.sMonth[0] == 0)&&(szTagTime.sTime.sMonth[1] == 0)) )
			{	// If there is empty time set to current time :			
				
				SYSTEMTIME curSysTime;
				GetLocalTime(&curSysTime);					
				SystemTime2Tag(&curSysTime,&szTagTime.sTime);
			}

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("SetGradePrice2Direct Grade %d, Mode %d, Level %d, price %ld",nGrade, lMode, nLevel, lPrice);
				_LOGMSG.LogMsg("CCtrl","SetGradePrice2Direct",lPumpNumber,LOG_PUMP,str);	
			}


			if (!(nFlags & PRICE_NO_PRICE_CHANGE)) 
			{
				if (nFlags & PRICE_END_OF_LIST) 
				{

					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("SetGradePrice2Direct - End Of Price List !" );
						_LOGMSG.LogMsg("CCtrl","SetGradePrice2Direct",lPumpNumber,LOG_PUMP,str);	
					}
				}
			}

			//Sent the price to the Q:
			
			CMD_STRUCT_GRADE_PRICE_WITH_TIME cmdInfo;
			memset(&cmdInfo,0,sizeof(CMD_STRUCT_GRADE_PRICE_WITH_TIME));
			
			
		
			cmdInfo.lPumpNumber = lPumpNumber;
			cmdInfo.nNozzle = nNozzle;
			cmdInfo.nGrade =  nGrade;
			cmdInfo.nFlags =  nFlags;
			cmdInfo.lPriceA = lPrice;
			cmdInfo.lPriceB =  0;
			memcpy(&cmdInfo.sDateTime,&szTagTime,sizeof(TAG_SYSTEMTIME));
			

			_Module.m_server.SetCommand(CMD_SET_GRADE_PRICE_WITH_TIME, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_GRADE_PRICE_WITH_TIME));
			

		}
	}	

	if (retCode != CTRL_OK)	
	{
		_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_PUMP, SUB_PRICE_CHANGE, PRICE_CHANGE_WAS_NOT_APPLIED, 1, ALARM_STARTED, 0, 0);
	}

	if(retCode)
		if(LOG_LOGIC_ERROR)
			_LOGMSG.LogResult(
				0, // Src
				"ICTRL",
				"SetGradePrice2Direct",
				TRUE,retCode,
				(dwLastErr!=0)? TRUE:FALSE,dwLastErr);


	if(pVal != NULL)
		*pVal = retCode;
}

/******************************************************************************
 Description:	Get complete picture of pumps active in forecourt.
				
 Returns:      	lPumpMap1 and  lPumpMap2 by reference represent all pumps status 
				if Not Idle = 1
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH			14/02/2008   12:00		Start 4.0.19.40
******************************************************************************/
STDMETHODIMP CCtrl::GetStationStatus(long *lPumpMap1, long *lPumpMap2, long lFlags, long *pRetVal)
{

#ifdef CATCH_CTRL
	try
	{
#endif
		GetStationStatus__(lPumpMap1 , lPumpMap2 , lFlags , pRetVal);
#ifdef CATCH_CTRL
	}
	catch(...)
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetStationStatus()",0,LOG_NONE,"",LOG_LEVEL_0);	

		exit(1);
	}   		
#endif 

	return S_OK;
}

void CCtrl::GetStationStatus__(long *lPumpMap1, long *lPumpMap2, long lFlags, long *pRetVal)
{
	CtrlRetCode retCode = CTRL_OK;


	if (SERVER_STATE_STARTUP)
		retCode = CTRL_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = CTRL_SERVER_NOT_READY;
	
	
	*lPumpMap1 = *lPumpMap2 = 0;
	
	//No errors so far - continue.
	if(retCode == CTRL_OK)
	{
		_Module.m_server.m_cCommonFunction.GetStationStatus(lPumpMap1, lPumpMap2, lFlags, pRetVal);		
	}

	
	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString sMsg;
		sMsg.Format("CCtrl::GetStationStatus() pump bit maps - 1 to 32 %x , 33 to 64 %x , returned %d", 
			        *lPumpMap1,
					*lPumpMap2,
					retCode);
		
		_LOGMSG.LogMsg(sMsg);
	}	 

	if(pRetVal) //4.0.21.502 - TD 71040
		*pRetVal = retCode;
}


/******************************************************************************
 Description:	This function is used for BOS to pull the status of the delivery 
                emulation process for a single tank. According to the received 
				TankNumber (lTankNumber): returns the process's status and information 
				for this tank, that PumpSrv has so far.
				
 Returns:      	S_OK;

 Parameters:    lTankNumber - Tank Number.
                lFlags      - For future use.
                plStatus    - Returned value of the status of the process:
				CTRL_IDLE,
                CTRL_READING_PROCESS_STARTED,
				CTRL_WAIT_FOR_CONNECTED_PUMPS_LOCK, 
				CTRL_CONNECTED_PUMPS_LOCKED,        
                CTRL_WAIT_TANK_READING,
				CTRL_READING_PROCESS_TIMEOUT,
				CTRL_READING_PROCESS_SUCCESS.
                pRec    - Returned value of the Xml with tank information.
				pRetVal - returned value of the function.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shellyb		29/09/2008   13:50		Start 4.0.21.500 TD 66935
******************************************************************************/
STDMETHODIMP CCtrl::GetDeliveryEmulationStatus(long lTankNumber, long lFlags, long *plStatus, BSTR *pRec, long *pRetVal)
{
	#ifdef CATCH_CTRL
	try
	{
#endif
		GetDeliveryEmulationStatus__(lTankNumber, lFlags, plStatus, pRec, pRetVal);
#ifdef CATCH_CTRL
	}
	catch(...)
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetDeliveryEmulationStatus()",0,LOG_NONE,"",LOG_LEVEL_0);	

		exit(1);
	}   		
#endif 

	return S_OK;
}
/******************************************************************************
 Description:	Implementation of GetDeliveryEmulationStatus.
				
 Returns:      	None;

 Parameters:    lTankNumber - Tank Number.
                lFlags      - For future use.
                plStatus    - Returned value of the status of the process:
				CTRL_IDLE,
                CTRL_READING_PROCESS_STARTED,
				CTRL_WAIT_FOR_CONNECTED_PUMPS_LOCK, 
				CTRL_CONNECTED_PUMPS_LOCKED,        
                CTRL_WAIT_TANK_READING,
				CTRL_READING_PROCESS_TIMEOUT,
				CTRL_READING_PROCESS_SUCCESS,
				CTRL_WAIT_FOR_CONNECTED_PUMPS_UNLOCK,
				CTRL_READING_PROCESS_FAILURE.

                pRec    - Xml with tank information.
				pRetVal - returned value of the function.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shellyb		06/10/2008   11:00		Start 4.0.21.500 TD 66935
******************************************************************************/
void CCtrl::GetDeliveryEmulationStatus__(long lTankNumber, long lFlags, long *plStatus, BSTR *pRec, long *pRetVal)
{
	CtrlRetCode retCode = CTRL_OK;

	// True values of retCode *?*? 
	if (SERVER_STATE_STARTUP)
		retCode = CTRL_CONTROLLER_NOT_AVAILABLE;
	else
		if (plStatus == NULL)
		{
			retCode = CTRL_FAILED_IO;
			CString sLogMsg("CCtrl::GetDeliveryEmulationStatus__: plStatus is NULL");
			 _LOGMSG.LogMsg(sLogMsg, LOG_LEVEL_1);
		}
	else
		if ((lTankNumber < 1) || (lTankNumber > _Module.m_server.GetLastValidTank()))
		{
			retCode = CTRL_INVALID_TANK_NUMBER;
			CString sLogMsg("CCtrl::GetDeliveryEmulationStatus__: Invalid Tank Number");
			 _LOGMSG.LogMsg(sLogMsg, LOG_LEVEL_1);
		}
	
	//No errors so far - continue:
	if(retCode == CTRL_OK)
	{
		_Module.m_server.m_cCommonFunction.GetDeliveryEmulationStatusSingleTank(lTankNumber, plStatus, pRec);
	}
	else
	{
		CString sMsg;
		sMsg.Format("CCtrl::GetDeliveryEmulationStatus__: Tank Number =  %ld , Process cannot start. Error = %ld", lTankNumber, retCode);
		_LOGMSG.LogMsg(sMsg);

	}

	if(pRetVal)
		*pRetVal = retCode;
}


/******************************************************************************
 Description:	The method is used by BOS for polling whether the process of
				changing product in tank is finished.
 Returns:      	CTRL return codes
 Parameters:	long lFlags			- flags
				long *plStatus		- status of the process. Possible values:
									CPIT_IDLE,							//0
									CPIT_START_LOCK_FORECOURT
									CPIT_LOCK_FORECOURT_IN_PROGRESS
									CPIT_FORECOURT_LOCKED
									CPIT_WAIT_CLOSE_OPEN_TRS
									CPIT_CHANGE_SETUP_AND_SEND_TO_CL
									CPIT_WAIT_SEND_TO_CL
									CPIT_WAIT_CL_RESPONSE
									CPIT_HANDLE_CL_RESPONSE
									CPIT_SEND_PRICE_CHANGE
									CPIT_WAIT_UNLOCK_FORECOURT

				long *plErrorCode	- non-zero value in case of failure. Possible values:
									CPIT_NO_ERROR
									CPIT_ERROR_LOCK_FORECOURT_FAILED
									CPIT_ERROR_OPEN_SALES_FOUND
									CPIT_ERROR_CL_RESPONSE_TIMEOUT
									CPIT_ERROR_CL_RESPONSE_FAILURE

				BSTR *pbstrData		- buffer in XML format will be returned in case of success
				long *plVal			- CTRL ret code 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		02/03/2009   11:30			Start		4.0.22.503 - TD 72189
******************************************************************************/
STDMETHODIMP CCtrl::GetChangeProductInTankStatus(long lFlags, long *plStatus, long *plErrorCode, BSTR *pbstrData, long *plVal)
{
	#ifdef CATCH_CTRL
	try
	{
#endif
		GetChangeProductInTankStatus__(lFlags, plStatus, plErrorCode, pbstrData, plVal);
#ifdef CATCH_CTRL
	}
	catch(...)
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetChangeProductInTankStatus()",0,LOG_NONE,"",LOG_LEVEL_0);	

		exit(1);
	}   		
#endif 

	return S_OK;
}


/******************************************************************************
 Description:	The method is used by BOS for polling whether the process of
				changing product in tank is finished.
 Returns:      	CTRL return codes
 Parameters:	long lFlags			- flags
				long *plStatus		- status of the process. Possible values:
									CPIT_IDLE,							//0
									CPIT_START_LOCK_FORECOURT
									CPIT_LOCK_FORECOURT_IN_PROGRESS
									CPIT_FORECOURT_LOCKED
									CPIT_WAIT_CLOSE_OPEN_TRS
									CPIT_CHANGE_SETUP_AND_SEND_TO_CL
									CPIT_WAIT_SEND_TO_CL
									CPIT_WAIT_CL_RESPONSE
									CPIT_HANDLE_CL_RESPONSE
									CPIT_SEND_PRICE_CHANGE
									CPIT_WAIT_UNLOCK_FORECOURT

				long *plErrorCode	- non-zero value in case of failure. Possible values:
									CPIT_NO_ERROR
									CPIT_ERROR_LOCK_FORECOURT_FAILED
									CPIT_ERROR_OPEN_SALES_FOUND
									CPIT_ERROR_CL_RESPONSE_TIMEOUT
									CPIT_ERROR_CL_RESPONSE_FAILURE

				BSTR *pbstrData		- buffer in XML format will be returned in case of success
				long *plVal			- CTRL ret code 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		02/03/2009   11:30			Start		4.0.22.503 - TD 72189
******************************************************************************/
void CCtrl::GetChangeProductInTankStatus__(long lFlags, long *plStatus, long *plErrorCode, BSTR *pbstrData, long *plVal)
{
	CtrlRetCode retCode = CTRL_OK;

	if (plStatus && plErrorCode && pbstrData)
	{
		*plStatus = _Module.m_server.m_cServerInfo.GetChangeProductInTankState();
		*plErrorCode = _Module.m_server.m_cServerInfo.GetChangeProductInTankFailureResult();

		if ((*plStatus == CPIT_IDLE) && (*plErrorCode == CPIT_NO_ERROR))
		{
			_Module.m_server.m_cServerInfo.GetChangeProductInTankXmlResultBuffer(pbstrData);
		}
		
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("CCtrl::GetChangeProductInTankStatus__, plStatus=%ld, plErrorCode=%ld", *plStatus, *plErrorCode);
			_LOGMSG.LogMsg(str);	
		}
	}
	else
		retCode = CTRL_PARAM_NOT_FOUND;

	if(plVal)
		*plVal = retCode;
}

/******************************************************************************
 Description:	The method is used by BOS for polling to check whether 
				the process is finished.
 Returns:      	CTRL return codes
 				
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		20/03/2009   16:30			Start		4.0.19.820 - TD 78775
******************************************************************************/
STDMETHODIMP CCtrl::GetProcessStatusEx(long lFlags, long lProcessType, long *plStatus, long *plErrorCode, BSTR *pbstrData, long *plVal)
{
	#ifdef CATCH_CTRL
	try
	{
#endif
		GetProcessStatusEx__(lFlags, lProcessType, plStatus, plErrorCode, pbstrData, plVal);
#ifdef CATCH_CTRL
	}
	catch(...)
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","GetProcessStatusEx()",0,LOG_NONE,"",LOG_LEVEL_0);	

		exit(1);
	}   		
#endif 

	return S_OK;
}

/******************************************************************************
 Description:	The method is used by BOS for polling to check whether 
				the process is finished.
 Returns:      	CTRL return codes
 Parameters:	long lFlags			- flags
				long lProcessType	- process type
				long *plStatus		- status of the process. Possible values:

				long *plErrorCode	- non-zero value in case of failure. Possible values:

				BSTR *pbstrData		- buffer in XML format will be returned
				
				<PriceChangeWithAck>
					<RefNumber>50895</RefNumber>
						<Pumps>
							<Pump Number="1" ResultCode="0"/>
							<Pump Number="2" ResultCode="3"/>
							<Pump Number="12" ResultCode="4"/>
						</Pumps>
				</PriceChangeWithAck>

				long *plVal			- CTRL ret code 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		20/03/2009   16:30			Start		4.0.19.820 - TD 78775
******************************************************************************/
void CCtrl::GetProcessStatusEx__(long lFlags, long lProcessType, long *plStatus, long *plErrorCode, BSTR *pbstrData, long *plVal)
{
	CtrlRetCode retCode = CTRL_OK;

	if (!plStatus || !plErrorCode || !pbstrData)
		retCode = CTRL_PARAM_NOT_FOUND;

	
	switch (lProcessType)
	{
		case PROCESS_CHANGE_PRICE_WITH_ACK_PER_PUMP:
		{
			*plStatus = _Module.m_server.m_cServerInfo.GetPriceChangeWithAckState();
			*plErrorCode = _Module.m_server.m_cServerInfo.GetPriceChangeWithAckFailureResult();

			// If the process is finished
			if (*plStatus == PCWA_IDLE)
			{
				_Module.m_server.m_cServerInfo.GetPriceChangeWithAckXmlResultBuffer(pbstrData);

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("CCtrl::GetProcessStatusEx. Status=%ld, ErrorCode=%ld", *plStatus, *plErrorCode);
					_LOGMSG.LogMsg(str);
				}
			}		
			
			break;
		}

		default:
			break;
	}

	if(plVal)
		*plVal = retCode;

}

/******************************************************************************
 Description:	Similar method to SetParam with extra parameter - flags
 Returns:      	CTRL return codes
 Parameters:	BSTR sTableName
				long lKey1
				long lKey2
				BSTR sFiledName
				short nFiledType
				BSTR sVal
				long lVal
				BOOL bCreate
				long lFlags - Flags
				long *pVal	- ctrl return code			
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		25/04/2010   12:30			Start		4.0.26.501 - TD 116227
******************************************************************************/
STDMETHODIMP CCtrl::SetParam4(BSTR sTableName, long lKey1, long lKey2, BSTR sFiledName, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long lFlags, long *pVal)
{
	#ifdef CATCH_CTRL
	try
	{
#endif
		SetParam4__(sTableName, lKey1, lKey2, sFiledName, nFiledType, sVal, lVal, bCreate, lFlags, pVal);
#ifdef CATCH_CTRL
	}
	catch(...)
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogClassMsg("CCtrl","SetParam4()",0,LOG_NONE,"",LOG_LEVEL_0);	

		exit(1);
	}   		
#endif 

	return S_OK;
}

/******************************************************************************
 Description:	Similar method to SetParam with extra parameter - flags
 Returns:      	CTRL return codes
 Parameters:	BSTR sTableName
				long lKey1
				long lKey2
				BSTR sFiledName
				short nFiledType
				BSTR sVal
				long lVal
				BOOL bCreate
				long lFlags - Flags
				long *pVal	- ctrl return code			
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		25/04/2010   12:30			Start		4.0.26.501 - TD 116227
******************************************************************************/
void CCtrl::SetParam4__(BSTR sTableName, long lKey1, long lKey2, BSTR sFiledName, short nFiledType, BSTR sVal, long lVal, BOOL bCreate, long lFlags, long *pVal)
{	
	CtrlRetCode retCode = CTRL_OK;
	
	long lGradeId = 0;

	if (lFlags & SET_PARAM_USE_EXTENDED_GRADE_ID)
	{
		// Need to convert extended grade id to grade id
		_Module.m_server.m_cExtendedGradeMap.GetGradeByExtGrade(lKey1, lGradeId);

		if (!lGradeId)
			retCode = CTRL_INVALID_GRADE_NUMBER;

	}
	else
		lGradeId = lKey1;
	
	if (!retCode)
		SetParam__(sTableName, lGradeId, lKey2,  sFiledName, nFiledType, sVal, lVal, bCreate,  pVal);


	if(pVal)
		*pVal = retCode;
}

/******************************************************************************
 Description:	The method provides information for pump totals in xml format
 Returns:      	CTRL return codes
 Parameters:	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		20/07/2010   16:15			Start		4.0.25.50 - TD 108311
******************************************************************************/
STDMETHODIMP CCtrl::GetPumpTotalsEx(long lOpCode, long *lIndexNumber, long lPumpNumber, long lNozzle, long lFlags, BSTR *pRec, long *pVal)
{
	#ifdef CATCH_CTRL
	__try
	{
#endif

		GetPumpTotalsEx__(lOpCode, lIndexNumber, lPumpNumber, lNozzle, lFlags, pRec, pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetPumpTotalsEx"))
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetPumpTotalsEx()", LOG_LEVEL_0);

		exit(1);
	}
#endif	


	return S_OK;
}

/******************************************************************************
 Description:	The method provides information for pump totals in xml format
 Returns:      	CTRL return codes
 Parameters:	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		20/07/2010   16:15			Start		4.0.25.50 - TD 108311
******************************************************************************/
void CCtrl::GetPumpTotalsEx__(long lOpCode, long *lIndexNumber, long lPumpNumber, long lNozzle, long lFlags, BSTR *pRec, long *pVal)
 {
	CtrlRetCode retCode = CTRL_OK;	

	long lRtc = OK;
	TOTAL_INFO info;
	memset(&info,0,sizeof(TOTAL_INFO));

	switch(lFlags)
	{
		case GET_DIRECT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ,&info);
			break;
		case GET_FIRST_INDEX:
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ_FIRST,&info);
			break;	
		case GET_NEXT_INDEX:
			info.lIndexNumber =  *lIndexNumber;
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ_NEXT,&info);
			break;
		default :
			lRtc = _Module.m_server.IndexOperation(PUMP_TOTALS_FILE,REC_READ_FIRST,&info);
			break;
	}

	if (lRtc != OK)
	{	
		if ((lRtc == ERR_NOT_FOUND)||(lRtc == ERR_DELETED))
			retCode = CTRL_RECORD_NOT_FOUND;
		else
			retCode = CTRL_FAILED_IO;			
	}
	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("CCtrl::GetPumpTotalsEx_ lFlags = %ld, retCode = %ld, lIndexNumber=%ld", lFlags, retCode, info.lIndexNumber);
		_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,str);
	}
		
	if(retCode == CTRL_OK)
	{

		*lIndexNumber = info.lIndexNumber;

		_Module.m_server.m_cPumpTotalsXml.PumpTotalsInfo2Xml(&info, pRec);

	}
	
	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetPumpTotalsEx",
					TRUE,retCode,
					(lRtc!=0)? TRUE:FALSE,lRtc);
	}

	if(pVal != NULL)
		*pVal = retCode;
}

/****************************************************************************************
 Description:	Get current rolling update information by pump number in xml format

	<RollingUpdateInfo>
		<MetaData RequestDate="09/05/2010" RequestTime="14:10:33" /> 
		<Pumps>
			<Pump Number="1" ShiftNumber="12">
				<Nozzle Number="3" TrsNum="540" CurrentVolume="20480" /> 
			</Pump>
			<Pump Number="4" ShiftNumber="12">
				<Nozzle Number="1" TrsNum="542" CurrentVolume="5770" /> 
			</Pump>
		</Pumps>
	</RollingUpdateInfo>

 Returns:      	CTRL return codes
 Parameters:	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		04/10/2010   16:00			Start		4.0.26.501 - TD 108311
******************************************************************************************/
STDMETHODIMP CCtrl::GetRollingUpdateInfo(long lPumpNumber, long lFlags, BSTR *pbstrData, long *pVal)
{
	#ifdef CATCH_CTRL
	__try
	{
#endif

		GetRollingUpdateInfo__(lPumpNumber, lFlags, pbstrData, pVal);
#ifdef CATCH_CTRL
	}
	__except(_HANDLE_EXCEPTION("CCtrl::GetRollingUpdateInfo"))
	{
		// we arrive here on error only !
		if(LOG_ERROR)
			_LOGMSG.LogMsg("CCtrl::GetRollingUpdateInfo()", LOG_LEVEL_0);

		exit(1);
	}
#endif	


	return S_OK;
}

/****************************************************************************************
 Description:	Get current rolling update information by pump number in xml format

	<RollingUpdateInfo>
		<MetaData RequestDate="09/05/2010" RequestTime="14:10:33" /> 
		<Pumps>
			<Pump Number="1" ShiftNumber="12">
				<Nozzle Number="3" TrsNum="540" CurrentVolume="20480" /> 
			</Pump>
			<Pump Number="4" ShiftNumber="12">
				<Nozzle Number="1" TrsNum="542" CurrentVolume="5770" /> 
			</Pump>
		</Pumps>
	</RollingUpdateInfo>

 Returns:      	CTRL return codes
 Parameters:	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		04/10/2010   16:00			Start		4.0.26.501 - TD 108311
******************************************************************************************/
void CCtrl::GetRollingUpdateInfo__(long lPumpNumber, long lFlags, BSTR *pbstrData, long *pVal)
{
	CtrlRetCode retCode = CTRL_OK;	

	retCode = _Module.m_server.m_cCommonFunction.GetRollingUpdateInfo(lPumpNumber, lFlags, pbstrData);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("lFlags = %ld, retCode = %ld", lFlags, retCode);
		_LOGMSG.LogMsg("CCtrl","GetRollingUpdateInfo__",lPumpNumber,LOG_PUMP,str);	
	}

	if((retCode)&&(retCode != CTRL_RECORD_NOT_FOUND))
	{
		if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICTRL",
					"GetRollingUpdateInfo",
					TRUE,retCode,
					FALSE,0);
	}

	if(pVal != NULL)
		*pVal = retCode;
}

///CR 442586
void CCtrl::SmartReconfigure__(long lDeviceNumberMask1, long lDeviceNumberMask2, long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags, long * pRetVal)
{
	long lPumpNumber = 0;
	DWORD dwLogLevel = DEFAULT_LOG_LEVEL;
	BOOL bValidTable = TRUE;
	CtrlRetCode lRetCode = CTRL_OK;

	CRegTable cTable;
	CString sLogMsg("SmartReconfigure started"), sRetCode;

	CString cTableName;
	long lClientEventData = 0;

	if (!cTable.ReconfigInProgress())
	{
		cTable.SetReconfigInProgress((BYTE)TRUE);
		_Module.m_server.m_cCommonFunction.SmartReconfigure(lDeviceNumberMask1, lDeviceNumberMask2, lTableId, lSubTableId, lSubTableIdExt, lFlags, pRetVal);
		cTable.SetReconfigInProgress((BYTE)FALSE);

		_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_RECONFIGURE, lTableId);
	}
	else
	{
		lRetCode = CTRL_IN_MIDDLE_OF_PROCESS;
		_Module.m_server.m_cLogger.LogMiddleOfProcessReason();
	}

	if (pRetVal)
		*pRetVal = lRetCode;

	sRetCode.Format("return code: %d", lRetCode);

	sLogMsg += sRetCode;

	_Module.m_server.m_cLogger.LogMsg(sLogMsg);

	return;
}
///END CR 442586

///CR 442586
STDMETHODIMP CCtrl::SmartReconfigure(long lDeviceNumberMask1, long lDeviceNumberMask2, long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags, long *pRetVal)
{
#ifdef CATCH_CTRL

	__try
	{

#endif
		SmartReconfigure__(lDeviceNumberMask1, lDeviceNumberMask2, lTableId, lSubTableId, lSubTableIdExt, lFlags, pRetVal);

#ifdef CATCH_CTRL
	}

	__except (_HANDLE_EXCEPTION("CCtrl::SmartReconfigure"))
	{
		if (LOG_ERROR)
			_Module.m_server.m_cLogger.LogMsg("CCtrl::SmartReconfigure()", LOG_LEVEL_0);

		exit(1);
	}

#endif

	return S_OK;
}
///END CR 442586



//4.0.25.994  TD 325383

STDMETHODIMP CCtrl::GetConfiguration(BSTR sClientName, BSTR *sConfigurationData, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif

		GetConfiguration__(sClientName, sConfigurationData, pVal);
#ifdef CATCH_CTRL
	}
	__except (_HANDLE_EXCEPTION("CCtrl::GetConfiguration"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_Module.m_server.m_cLogger.LogMsg("CCtrl::GetConfiguration()", LOG_LEVEL_0);

		exit(1);
	}
#endif	


	return S_OK;
}


//4.0.25.994  TD 325383
void CCtrl::GetConfiguration__(BSTR sClientName, BSTR * sConfigurationData, long * pVal)
{

	CtrlRetCode retCode = CTRL_OK;

	retCode = _Module.m_server.m_cCommonFunction.GetConfiguration(sClientName, sConfigurationData);

	if ((retCode) && (retCode != CTRL_RECORD_NOT_FOUND))
	{
		if (LOG_LOGIC_ERROR)
			_Module.m_server.m_cLogger.LogResult(
			0, // Src
			"ICTRL",
			"GetConfiguration",
			TRUE, retCode,
			FALSE, 0);
	}


	if (pVal != NULL)
		*pVal = retCode;
}



//4.0.25.994  TD 325383
STDMETHODIMP CCtrl::SetAlarm(long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus, long lAlarmValue, long lFlags, long *pAlarmId, BSTR sRec, long *pVal)
{

#ifdef CATCH_CTRL
	__try
	{
#endif
		SetAlarm__(lCatagory, lSubCatagory, lAlarmNumber, lDevice, lAlarmStatus, lAlarmValue, lFlags, pAlarmId, sRec, pVal);

#ifdef CATCH_CTRL
	}
	__except (_HANDLE_EXCEPTION("CCtrl::GetConfiguration"))
	{
		// we arrive here on error only !
		if (LOG_ERROR)
			_Module.m_server.m_cLogger.LogMsg("CCtrl::SetAlarm()", LOG_LEVEL_0);

		exit(1);
	}
#endif	

	return S_OK;
}


//4.0.25.994  TD 325383
void CCtrl::SetAlarm__(long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus, long lAlarmValue, long lFlags, long *pAlarmId, BSTR sRec, long *pVal)
{
	char		 sMessages[MAX_ALARM_DESC_LEN];
	_bstr_t		bstrMessage(sRec);
	CtrlRetCode retCode = CTRL_OK;

	long lRetVal = 0;

	if (bstrMessage.length())
	{
		memset(sMessages, 0, sizeof(sMessages));
		memcpy(sMessages, (const char *)bstrMessage, min(sizeof(sMessages), bstrMessage.length()));
		lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(lCatagory, lSubCatagory, lAlarmNumber, lDevice, lAlarmStatus, lAlarmValue, lFlags, sMessages);
	}
	else
	{
		lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(lCatagory, lSubCatagory, lAlarmNumber, lDevice, lAlarmStatus, lAlarmValue, lFlags);
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString sMsg;
		sMsg.Format("CTRL Raise Alarm, lCatagory=%ld ,lSubCatagory=%ld,lAlarmNumber=%ld,lDevice=%ld,lAlarmStatus=%ld,lAlarmValue=%ld , lRetVal=%ld ",
			lCatagory, lSubCatagory, lAlarmNumber, lDevice, lAlarmStatus, lAlarmValue, lRetVal);
		_Module.m_server.m_cLogger.LogMsg(sMsg);
	}

	if (lRetVal)
		retCode = CTRL_FAILED_IO;

	if (pVal != NULL)
		*pVal = retCode;
}


// 4.0.25.1130 
STDMETHODIMP CCtrl::SetTankState(long lTankNumber, long lState, long lFlags, BSTR sRec, long *pRetVal)
{

#ifdef CATCH_CTRL

	__try
	{

#endif
		SetTankState__(lTankNumber, lState, lFlags, sRec, pRetVal);
#ifdef CATCH_CTRL
	}

	__except (_HANDLE_EXCEPTION("CCtrl::SetTankState"))
	{
		if (LOG_ERROR)
			_Module.m_server.m_cLogger.LogMsg("CCtrl::SetTankState()", LOG_LEVEL_0);

		exit(1);
	}

#endif


	return S_OK;
}

void CCtrl::SetTankState__(long lTankNumber, long lState, long lFlags, BSTR sRec, long * pRetVal)
{
	CtrlRetCode retCode = CTRL_OK;

	if (SERVER_STATE_STARTUP)
		retCode = CTRL_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = CTRL_SERVER_NOT_READY;

	else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
	{
		retCode = CTRL_IN_MIDDLE_OF_PROCESS;
	}


	if (retCode == CTRL_OK)
	{
		retCode = _Module.m_server.m_cCommonFunction.SetTankState(lTankNumber, lState, lFlags, sRec);

	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_Module.m_server.m_cLogger.LogResult(
			0, // Src
			"ICTRL",
			"SetTankState",
			TRUE, retCode,
			FALSE, 0);


	*pRetVal = retCode;
}




STDMETHODIMP CCtrl::GetTankState(long lTankNumber, long lFlags, BSTR *pRec, long *pRetVal)
{

#ifdef CATCH_CTRL

	__try
	{

#endif
		GetTankState__(lTankNumber, lFlags, pRec, pRetVal);
#ifdef CATCH_CTRL
	}

	__except (_HANDLE_EXCEPTION("CCtrl::GetTankState"))
	{
		if (LOG_ERROR)
			_Module.m_server.m_cLogger.LogMsg("CCtrl::GetTankState()", LOG_LEVEL_0);

		exit(1);
	}

#endif


	return S_OK;
}


//4.0.25.1130 
void CCtrl::GetTankState__(long lTankNumber, long lFlags, BSTR *pRec, long *pRetVal)
{
	CtrlRetCode retCode = CTRL_OK;

	if (SERVER_STATE_STARTUP)
		retCode = CTRL_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		retCode = CTRL_SERVER_NOT_READY;

	else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
	{
		retCode = CTRL_IN_MIDDLE_OF_PROCESS;
	}


	if (retCode == CTRL_OK)
	{
		retCode = _Module.m_server.m_cCommonFunction.GetTankState(lTankNumber, lFlags, pRec);
	}

	if (retCode)
		if (LOG_LOGIC_ERROR)
			_Module.m_server.m_cLogger.LogResult(
			0, // Src
			"ICTRL",
			"SetTankState",
			TRUE, retCode,
			FALSE, 0);


	*pRetVal = retCode;
}

