// CommonFunction.cpp: implementation of the CCommonFunction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FleetCard.h"
#include "CommonFunction.h"
#include "SQLPumpSrvFile.h"
#include "PumpSrvDatabase.h"
#include "SQLReceiptDB.h"
#define MAX_MSG_LEN     4096

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommonFunction::CCommonFunction()
{

}

CCommonFunction::~CCommonFunction()
{

}
/******************************************************************************
 Description:	Get current tank details from memory.	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2003   13:35		Start
 Marik			10/11/2004	 10:00      Add new flags to function that define different  memeory tank structur. 
******************************************************************************/
void CCommonFunction::GetTankMemoInfo(long lTankNumber, BSTR *pRec, long lFlags, long *pVal , long *lRecordCount, long lVersion)
{
	CString sMsg;
	long lRecordSize = sizeof(TANK_READ_MEMO);
	
	
	if ((lVersion & TRM_TYPE_INCLUDE_DELIVERY_DATA) || (lVersion & TRM_TYPE_WETSTOCK_REPORT)) //4.0.22.503 - TD 69905
		lRecordSize= sizeof(TANK_READ_MEMO_2);

	if (lFlags & GTI_DELIVERY_NO_HISTORY_INFO) //4.0.9.555		
		_Module.m_server.GetWetStockData__(WSM_BY_DEMAND_MEMO ,WET_STOCK_MOST_RECENT_DELIVERY | WET_STOCK_REQUESTED_BY_POS);
	else if (lFlags & GTI_ULLAGE_READING_INFO) //4.0.9.555		
		_Module.m_server.GetWetStockData__(WSM_BY_DEMAND_MEMO ,WET_STOCK_TANKS | WET_STOCK_REQUESTED_BY_POS);	
	 else  if(lFlags & REFRESH_TANK_INFO) //4.0.1.32 - Read tank on client demand for updated data.						
		_Module.m_server.GetWetStockData__(WSM_BY_MEMO, WET_STOCK_REQUESTED_BY_POS_PUMP); //4.0.16.500


	*lRecordCount = 0;
	
	if ( lTankNumber == READ_ALL_TANKS )
	{
		long lNumTanks = 0;
		//lFlags == 1 means client is configured for 32 tanks.
		//lFlags == 0 means client is configured for 16 tanks.
		if(lFlags & SUPPORT_32_TANKS)
			lNumTanks = MAX_TANKS;
		else
			lNumTanks = OLD_MAX_TANKS;
		
		int iCounter = 0;
		int iNumOfCopiedChars;
		int iArrLength = lRecordSize * lNumTanks + 2;
		BYTE * sTmp = new BYTE[iArrLength];

		memset(sTmp , 0 , iArrLength);
		
		//setting the first two positions for the number to valid tanks number
		memcpy(sTmp,"00",2);
		iNumOfCopiedChars = 2;

		BYTE *sTmpTankMemo  = new BYTE[lRecordSize];

		memset(sTmpTankMemo ,' ' ,lRecordSize);

		BOOL bIncludeTankInReport = TRUE;

		// process details only if we successfully allocate memeory
		if ((sTmp) && (*sTmpTankMemo))
		{
			for ( long tn = 0 ; tn < lNumTanks ; tn++ )
			{
				//checking if this tank is valid
				if ( _Module.m_server.m_cTankArray[tn].IsTankValid() )
				{
					memset(sTmpTankMemo , ' ' , lRecordSize);
					_Module.m_server.m_cTankArray[tn].GetTankTagInfo(sTmpTankMemo,lVersion);					

					//4.0.22.503 - TD 69905
					bIncludeTankInReport = CheckIncludeTankInReport(sTmpTankMemo);

					if (bIncludeTankInReport)
					{
						iCounter++;

						memcpy(sTmp+iNumOfCopiedChars,sTmpTankMemo,lRecordSize);
						iNumOfCopiedChars += lRecordSize;
					}
				}
			}
			//checking if at least one tank was valid
			if ( iCounter == 0 )
			{
				*pVal = GPI_TANK_NOT_VALID;
				if(LOG_LOGIC_ERROR)
					_LOGMSG.LogResult(
						0, // Src
						"IGPI",
						"GetTankInfo",
						TRUE,*pVal,
						FALSE,0);
			}
			else
			{
				// tank record counter.
				*lRecordCount = iCounter; 
				//setting the two first positions with number of tanks.
				i2a(iCounter,sTmp,2);

				CComBSTR strResult(iArrLength , (LPCSTR)sTmp); 
				*pRec = strResult.Copy();
			}

		}
		else
		{			
			sMsg.Format("GetTankMemoInfo  allocation failed !!!!!!! for all tanks");
			_LOGMSG.LogMsg(sMsg , LOG_LOGIC_ERROR);
			*pVal = GPI_RECORD_NOT_FOUND;
		}

		if (sTmp)
			delete[] sTmp;

		if (sTmpTankMemo)
			delete []sTmpTankMemo;
	}//end if lTankNumber == READ_ALL_TANKS
	else
	{
		BYTE *sTmpTankMemo  = new BYTE[lRecordSize];
		memset(sTmpTankMemo , ' ',lRecordSize) ;

		if (sTmpTankMemo)
		{
			long tn = lTankNumber-1;

			//checking if tank is valid
			if ( _Module.m_server.m_cTankArray[tn].IsTankValid() )
			{
				_Module.m_server.m_cTankArray[tn].GetTankTagInfo(sTmpTankMemo , lVersion);
				*lRecordCount = 1;
			}			
			else
			{
				*pVal = GPI_TANK_NOT_VALID;
				if(LOG_LOGIC_ERROR)
					_LOGMSG.LogResult(
						0, // Src
						"IGPI",
						"GetTankInfo",
						TRUE,*pVal,
						FALSE,0);
			}

			BYTE  *sTmp = new BYTE[lRecordSize+2];  
			Str2StrNull((BYTE *)sTmpTankMemo,lRecordSize ,sTmp);

			CComBSTR strResult(lRecordSize,(LPCSTR)sTmp); 
			*pRec = strResult.Copy();

			if (sTmp)
				delete []sTmp;
		}
		else
		{
			sMsg.Format("GetTankMemoInfo  allocation failed !!!!!!! for all tanks");
			_LOGMSG.LogMsg(sMsg , LOG_LOGIC_ERROR);				
			*pVal = GPI_RECORD_NOT_FOUND;
		}

		delete []sTmpTankMemo;
		
	}	
}


/******************************************************************************
 Description:	Check whether this tank need to be included in wetstock report
 Returns:      	BOOL
 Parameters:   	

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			16/03/2009   14:00		Start		4.0.22.503 - TD 69905
******************************************************************************/
BOOL CCommonFunction::CheckIncludeTankInReport(BYTE *pBuff)
{
	TANK_READ_MEMO_2 cTankReadMemo;
	memset(&cTankReadMemo,' ',sizeof(TANK_READ_MEMO_2));

	BOOL bIncludeTankInReport = TRUE;

// 	if (_Module.m_server.m_cServerInfo.GetReportRequestState() == REPORT_SEND_REQUEST)		//disabled for 443035
// 	{
// 		memcpy(&cTankReadMemo, (void *)pBuff, sizeof(TANK_READ_MEMO_2));
// 		
// 		SYSTEMTIME sDateTime;
// 		Tag2SystemTime(&(cTankReadMemo.sDateTime), &sDateTime);
// 		CTime cTankReadingTime(sDateTime);
// 
// 		SYSTEMTIME cStartReportProcessSysTime = _Module.m_server.m_cServerInfo.GetReportStartTime();
// 		CTime cStartReportProcessTime(cStartReportProcessSysTime); 
// 		
// 		// if the actual tank reading time smaller than the start report process
// 		if ((cTankReadingTime < cStartReportProcessTime) && !_Module.m_server.GetWetStockReportIncludeOldDataFlag())
// 		{
// 			bIncludeTankInReport = FALSE;
// 		}
// 	}

	return bIncludeTankInReport;
}

/******************************************************************************
 Description:	Runs the reconfigure process 	
 Returns:      	
 Parameters:   	

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			1/12/2003   14:39		Start
******************************************************************************/
void CCommonFunction::Reconfigure2(long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags, long *pRetVal,BOOL bSendEvent)
{
	CMD_STRUCT_RECONFIGURATION_EVENT cParamStruct;
	
	cParamStruct.lTableId = lTableId;
	cParamStruct.lSubTableId = lSubTableId;
	cParamStruct.lSubTableIdExt = lSubTableIdExt;
	cParamStruct.lFlags = lFlags;
	
	
	if (cParamStruct.lFlags & FLAG_RECONFIGURE_IN_EOS)
	{
		_Module.m_server.RegisterEOSReconfigureEvent(&cParamStruct);
	}
	else
	{
		_Module.m_server.UnRegisterEOSReconfigureEvent(&cParamStruct); // need to check
		_Reconfigure2(lTableId, lSubTableId, lSubTableIdExt, lFlags, pRetVal, bSendEvent);		
	}
	
	
}
void CCommonFunction::_Reconfigure2(long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags, long *pRetVal,BOOL bSendEvent)
{
	long lPumpNumber = 0 , lMask = 1;
	DWORD dwLogLevel = DEFAULT_LOG_LEVEL;
	BOOL bValidTable = TRUE;
	CtrlRetCode lRetCode = CTRL_OK;

	CRegTable cTable;
	CString sLogMsg("CCommonFunction::Reconfigure2 - ") , sRetCode;

	CString cTableName;
	long lClientEventData = 0;

	
	CMD_STRUCT_RECONFIGURATION_EVENT cParamStruct;
	
	cParamStruct.lTableId = lTableId;
	cParamStruct.lSubTableId = lSubTableId;
	cParamStruct.lSubTableIdExt = lSubTableIdExt;
	cParamStruct.lFlags = lFlags;

	switch(lTableId)
	{
	case GENERAL_PARAM_TABLE:
		if(!cTable.LoadGeneralParamSubTable(lSubTableId , FALSE , TRUE))  //4.0.2.36
		{
			lRetCode = CTRL_IN_MIDDLE_OF_PROCESS;
			sLogMsg += "GENERAL_PARAM_TABLE (failed OLA reconfiguration), ";
		}
		else
		{
			cTableName = "GeneralParam";
			lClientEventData = lSubTableId;
			sLogMsg += "GENERAL_PARAM_TABLE, ";
		}
			
		break;

	case SHIFTS_TABLE:
		cTable.LoadShiftsTable(FALSE , TRUE , TRUE);
		sLogMsg += "SHIFTS_TABLE, ";
		cTableName = "Shifts\\Day1\\Shift1";
		break;
	
	case DATABASE_TABLE:
		cTable.LoadDatabaseTable(FALSE , TRUE , TRUE);
		sLogMsg += "DATABASE_TABLE, ";
		cTableName = "Database";
		break;
	case LOYALTY_TABLE:	// 4.0.23.2812
		cTable.LoadFuelLoyalty(FALSE, TRUE);
		sLogMsg += "LOYALTY_TABLE, ";
		cTableName = "Loyalty";
		break;
	case TERMINALS_TABLE:
		sLogMsg += "TERMINALS_TABLE (delegated to CmdTX), ";
		cTableName = "Terminals\\Terminal01";
		break;

	case CARWASH_TABLE:
		_Module.m_server.m_cCarWash.LoadCarWashDefinition();
		sLogMsg += "CARWASH_TABLE, ";
		cTableName = "CarWash";
		break;

	case GRADES_TABLE:
		cTable.LoadGradesTable(FALSE , TRUE , TRUE);
		sLogMsg += "GRADES_TABLE, ";
		cTableName = "Grades\\Grade01";
		break;

	case IPT_TABLE:
		cTable.LoadIPTTable(FALSE , TRUE);
		sLogMsg += "IPT_TABLE, ";
		cTableName = "IPT\\IPT01";
		break;

	case MESSAGES_TABLE:
		sLogMsg += "MESSAGES_TABLE - not support";     //4.0.6.506
		cTableName = "Messages";
		break;

	//Mode reconfiguration supported by PumpSrv without a call
	//to Reconfigure2, so we just write to the log.
	case MODES_TABLE:
		sLogMsg += "MODES_TABLE";
		bSendEvent = FALSE; //no need to send via the reconfigure	
		break;

	case PRICE_POLES_TABLE:
		cTable.LoadPricePolesTable(FALSE , TRUE);
		sLogMsg += "PRICE_POLES_TABLE, ";
		cTableName = "PricePoles\\PricePole01";
		break;

	case PURE_PRODUCTS_TABLE:
		cTable.LoadPureProductsTable(FALSE , TRUE , TRUE);
		sLogMsg += "PURE_PRODUCTS_TABLE, ";
		cTableName = "PureProducts\\PureProduct01";
		break;

	case RECEIPT_TABLE:
		cTable.LoadReceiptTable(FALSE , TRUE , TRUE);
		sLogMsg += "RECEIPT_TABLE, ";
		cTableName = "Receipt";
		break;

	case SCREEN_LAYOUT_TABLE:
		cTable.LoadScreenLayoutsTable(FALSE , TRUE , TRUE);
		sLogMsg += "SCREEN_LAYOUT_TABLE, ";
		cTableName = "ScreenLayouts\\ScreenLayout01";
		break;

	case SERVICE_FEE_TABLE:
		cTable.LoadServiceFeeTable(FALSE , TRUE , TRUE);
		sLogMsg += "SERVICE_FEE_TABLE, ";
		cTableName = "ServiceFee\\ServiceFee01";
		break;

	case TANKS_TABLE:
		cTable.LoadTanksTable(FALSE , TRUE , TRUE);
		sLogMsg += "TANKS_TABLE, ";
		cTableName = "Tanks\\Tank01";
		break;

	case TAXES_TABLE:
		cTable.LoadTaxesTable(FALSE , TRUE);
		sLogMsg += "TAXES_TABLE, ";
		cTableName = "Taxes\\Tax01";
		break;

	case WASH_PROGRAMS_TABLE:
		sLogMsg += "WASH_PROGRAMS_TABLE (not supported), ";
		lRetCode = CTRL_NOT_SUPPORTED;
		cTableName = "WashPrograms\\WashProgram01";
		break;

	case ALLIED_TABLE:
		sLogMsg += "ALLIED_TABLE, ";
		cTableName = "Allied";
		break;

	case RFS_TABLE:
		cTable.LoadRFSSubTable(lSubTableId , FALSE , TRUE);
		sLogMsg += "RFS_TABLE, ";
		cTableName = "RFS";
		lClientEventData = lSubTableId; 
		break;

	case MEGGITT_TABLE:  //4.0.2.36
		sLogMsg += "MEGGITT_TABLE ";
		cTableName = "Meggitt";
		break;

	case DOMS_TABLE:
	{
		cTable.LoadDOMSSubTable(lSubTableId , FALSE , TRUE);
		cTableName = "Doms";
		break;
	}

	case POSTEC_TABLE:					//4.0.21.21 TD 69287
	{
		sLogMsg += "POSTEC_TABLE, ";
		cTableName = "Postec";
	}break;
	
	case PUMPS_TABLE:
		_Module.m_server.m_cOLASrv->LoadAllProductSoldOnPump(0); // RFUEL-1213 - Since Epsilon implementation, pumps don't work properly after Pump configuration (FCC). A reboot of Fuel or re-load of Epsilon is required.
		sLogMsg += "PUMPS_TABLE (delegated to CmdTX), ";
		cTableName = "Pumps\\Pump01";			
		break;

	case TANK_SRV_TABLE:		//CR 414103
		sLogMsg += "TANK_SRV_TABLE (delegated to CmdTX), ";
		cTableName = "TankSrv";
		break;

	default:
		bValidTable = FALSE;
		sLogMsg += "Invalid table ID, ";
		dwLogLevel = LOG_LEVEL_0;
		lRetCode = CTRL_FAILED_IO;  //4.0.2.36
		break;
	}

	if(bValidTable)
	{
		_Module.m_server.SetCommand(CMD_RECONFIGURE_EVENT,
			SERVER_POS_NUMBER,
			0,
			&cParamStruct,
			sizeof(cParamStruct));
		
		//4.0.5.11
		if(bSendEvent == TRUE && _HOST.m_lHydraActive)
		{
			DWORD dwParamCRC = 0;
			_PARAM.LoadParam(cTableName.GetBuffer(0),"ParamCRC",&dwParamCRC,0,TRUE,FALSE);
			_PARAM.SaveParam(cTableName.GetBuffer(0),"ParamCRC",dwParamCRC,TRUE,_HOST.m_lGroupNum,0,lClientEventData);
		}
	}

	if(pRetVal)
		*pRetVal = lRetCode;
	
	return;
}
/******************************************************************************
 Description:	This function checks if a card type is a payment card.

 Returns:      	TRUE - a payment card.
                FALSE - not a payment card.

 Parameters:    lCardType - card type.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		10/12/2008   18:15		Start 4.0.21.501 TD 67443
******************************************************************************/
BOOL CCommonFunction::IsPaymentCard(const long lCardType)
{
	BOOL bRet = FALSE;
 
    switch(lCardType)
	{
	case CARD_TYPE_LOYALTY_AND_CREDIT:
    case CARD_TYPE_CREDIT:
    case CARD_TYPE_PREPAID_CASH:
    case CARD_TYPE_FLEET:
    case CARD_TYPE_FLEETCARD:
    case CARD_TYPE_CASH:
    case CARD_TYPE_DEBIT:
    case CARD_TYPE_CUSTOMER:
    case CARD_TYPE_TEST_CARDS:
	case CARD_TYPE_COMBO:
		{
			bRet = TRUE;
		}
	}
 
 return bRet;
}

BOOL CCommonFunction::IsLoyaltytCard(const long lCardType)
{
	return ((lCardType == CARD_TYPE_LOYALTY) || (lCardType == CARD_TYPE_LOYALTY_AND_CREDIT));
}


/******************************************************************************
 Description:	This function checks if there is a delivery enulation single tank in 
                process, for at least one tank.

 Returns:      	TRUE - there is a process / FALSE - there isn't.
 Parameters:    lTankNumber - returned value, of the tank which is in process.
                If no tan in process: lTankNumber = 0;  

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		26/10/2008   11:02		Start 4.0.21.500 TD 66935
******************************************************************************/
BOOL CCommonFunction::IsDeliveryEmulationSingleTankInProcess(long & lTankNumber)
{
	BOOL bRes = FALSE;
	lTankNumber = 0;

	for (int lTankIndex = 1; lTankIndex <= _Module.m_server.GetLastValidTank() ; lTankIndex++)
	{
		if (_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessType(lTankIndex) != PD_SINGLE_TANK_NONE)
		{
			lTankNumber = lTankIndex;
			bRes = TRUE;
			break;
		}
	}

	return bRes;
}

/******************************************************************************
 Description:	This function checks if there is a delivery enulation single tank in 
                reading process, for at least one tank.

 Returns:      	TRUE - there is a process / FALSE - there isn't.
 Parameters:    lTankNumber - returned value, of the tank which is in process.
                If no tan in process: lTankNumber = 0;

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		26/10/2008   11:02		Start 4.0.21.500 TD 66935
******************************************************************************/
BOOL CCommonFunction::IsDeliveryEmulationSingleTankInReadingProcess(long & lTankNumber) const // 4.0.28.501 TD 254219
{
	BOOL bRes = FALSE;
	lTankNumber = 0;

	for (int lTankIndex = 1; lTankIndex <= _Module.m_server.GetLastValidTank() ; lTankIndex++)
	{
		if (_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankStatus(lTankIndex) != DES_SINGLE_TANK_IDLE)
		{
			lTankNumber = lTankIndex;
			bRes        = TRUE;
			break;
		}
	}

	return bRes;
}

/******************************************************************************
 Description:	This function checks if there is a delivery enulation single tank in process,
                for lTankReference.

 Returns:      	TRUE - there is a process / FALSE - there isn't.
 Parameters:    Tank Number.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		16/10/2008   18:48		Start 4.0.21.500 TD 66935
******************************************************************************/
BOOL CCommonFunction::DeliveryEmulationSingleTankInProcess(const long lTankNumber)
{
	BOOL DeliveryEmulationSingleTankInProcess = FALSE;

	if ((_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankStatus(lTankNumber) != DES_SINGLE_TANK_IDLE) || (_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessType(lTankNumber) != PD_SINGLE_TANK_NONE) || (_Module.m_server.m_cServerInfo.GetSingleTankLockStatus(lTankNumber) != SINGLE_TANK_UNLOCKED))
		DeliveryEmulationSingleTankInProcess = TRUE;
	return DeliveryEmulationSingleTankInProcess;

}

/******************************************************************************
 Description:	This function checks if lTankReference is valid.

                lTankReference is valid either if we don't have any DeliveryEmulationSingleTank
				in process for this tank at all, or we have or we have a DeliveryEmulationSingleTank
				in process for this tank and this reference.

                lTankReference is not valid - whenever we have a DeliveryEmulationSingleTank
				in process for this tank, but with another reference.

 Returns:      	Valid / Not Valid.
 Parameters:    Tank Number, Tank Reference.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		16/10/2008   18:48		Start 4.0.21.500 TD 66935
******************************************************************************/
BOOL CCommonFunction::IsValidReferenceNumber(const long lTankNumber,const long lTankReference)
{
	BOOL bValidNumber = TRUE;
	long lReference;

	if (DeliveryEmulationSingleTankInProcess(lTankNumber))
	{
		lReference = _Module.m_server.m_cTankArray[lTankNumber-1].GetDeliveryReference();
	    if (lReference != lTankReference)
		{
			bValidNumber = FALSE;
			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString strLog;
			    strLog.Format("CCommonFunction::IsValidReferenceNumber: Tank Reference %d is invalid. There is a delivery process on Tank Number %d, with Reference %d.", 
			    lTankReference, lTankNumber, lReference);
			    _LOGMSG.LogMsg(strLog);							
			}
		}
	}
	
	return bValidNumber;
}

/******************************************************************************
 Description:	Setting the delivery emulation for single tank process.
                Setting the Process state machine with DES_SINGLE_TANK_START_LOCK as its state,
				and either PD_SINGLE_TANK_START or PD_SINGLE_TANK_END, as its Delivery process
				flag - depends on whether we're beginning a start delivery process or an ending 
				delivery process. 

 Returns:      	None.
 Parameters:    Tank Number, Tank Reference, whether to lock connected pumps,
                Process Command, Returned Value.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		08/10/2008   11:34		Start 4.0.21.500 TD 66935
******************************************************************************/
void CCommonFunction::SetProcessDeliveryEmulationSingleTank(long lTankNumber, long lTankReference, BOOL bLockConnectedPumps,long lProcessCommand, long *pVal)
{
	int nErrorRetreived = 0;
	long lReadingStatus = _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankStatus(lTankNumber);
	long lSuccessType   = _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankSuccess(lTankNumber);

	//Checking the db connection:
	if (FALSE == _Module.m_server.DBConnectionAvailable())
	{
		*pVal = GPI_FAILED_IO;
	}
	else
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString strLog;
			strLog.Format("CCommonFunction::SetProcessDeliveryEmulationSingleTank: Tank Number: %d, Delivery Status: %d, Delivery Process: %d.", 
			lTankNumber, _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankStatus(lTankNumber), _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessType(lTankNumber));
			_LOGMSG.LogMsg(strLog);							
		}

		//Check if the same tank is in process with another reference:
		if (IsValidReferenceNumber(lTankNumber, lTankReference))
		{
			//If not: check if tank is in reading process with the same reference (different in ABORT):
			if (lProcessCommand == PD_SINGLE_TANK_FORCE_RELEASE)
			{
				if (((lReadingStatus == DES_SINGLE_TANK_UNLOCK) || (lReadingStatus == DES_SINGLE_TANK_IDLE)) && (lSuccessType != DESF_SINGLE_TANK_TIMEOUT))
				//Process already finished (Assumption: no ABORT will be sent before process was even started!!)
				//If unlock will be changed to be only after last reading: need to decide what to do if readings
				//of this specific tank was already done (especially at end). *?*?
				{
					*pVal = GPI_DELIVERY_READING_PROCESS_DONE;
				}
				else
				{
					_Module.m_server.SetDeliverySingleTankFlags(lTankNumber, lTankReference, bLockConnectedPumps, DES_START_LOCK, (BYTE) lProcessCommand, nErrorRetreived);
				    *pVal = nErrorRetreived;
				}

			}
			else //lProcessCommand != PD_SINGLE_TANK_FORCE_RELEASE
			{
				if (lReadingStatus == DES_SINGLE_TANK_IDLE)
				//tank is not in reading process with the same reference => try to start this process:
				{
					_Module.m_server.SetDeliverySingleTankFlags(lTankNumber, lTankReference, bLockConnectedPumps, DES_START_LOCK, (BYTE) lProcessCommand, nErrorRetreived);
				    *pVal = nErrorRetreived;
				}
				else //tank is in reading process with the same reference, and command is not ABORT - update error code:
				{
					*pVal = CheckDeliverySingleTankProcessState(lTankNumber, lProcessCommand);
				}

			}
		}
		else
			*pVal = GPI_DELIVERY_INVALID_REFERENCE_NUMBER;
	}
}

/******************************************************************************
 Description:	A common function for setting a process
 Returns:      	
 Parameters:    long lProcessNumber, long lParam1, long lParam2, long *pVal

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi F			09/02/2004   09:53		Start
 Shelly B       08/10/2008   11:34      Add a support in new process PROCESS_DELIVERY_SINGLE_TANK, 
                                        and add lParam3, lParam4 (4.0.21.500 TD 66935)
******************************************************************************/
void CCommonFunction::SetProcess(long lProcessNumber, long lParam1, long lParam2, long lParam3, long lParam4, long *pVal)
{

	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();


	switch(lProcessNumber)
	{
		case PROCESS_LOCK:
		//==============================================================
		{				
			long lRtc = _PARAM.SetParam( "GeneralParam", 0,0,"ProcessLockRequest",FIELD_DWORD,NULL,(lParam1 ? 1:0), FALSE); 			
	 		if(lRtc)
			{
				switch(lRtc)
				{
					case 1: *pVal = GPI_FAILED_TO_OPEN_TABLE;break;		 
					case 2: *pVal = GPI_PARAM_NOT_FOUND;break;		 
				}
				
			}
			else // OK 
			{	
				if(_Module.m_server.GetServerState() >= SERVER_STARTUP)
				{
					CMD_STRUCT_UPDATE_PARAM cmdInfo;
					memset(&cmdInfo,0,sizeof(CMD_STRUCT_UPDATE_PARAM));
					strcpy(cmdInfo.szTableName,"GeneralParam");			
					cmdInfo.lKey1 = 0;
					cmdInfo.lKey2 = 0;
					strcpy(cmdInfo.szFieldName,"ProcessLockRequest");			
					cmdInfo.nFieldType = FIELD_DWORD;						
					cmdInfo.lVal = (lParam1 ? 1:0);
					_Module.m_server.SetCommand(CMD_UPDATE_PARAM,SERVER_POS_NUMBER, 0,&cmdInfo,sizeof(CMD_STRUCT_UPDATE_PARAM));
				}
			}
		}		
		break;

		case PROCESS_HELP:
		//==============================================================
		{
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			CPumpStatus		pumpStat;
			pumpStat = _Module.m_server.m_cPumpArray[lParam1-1].GetStat();
			long lMisc = pumpStat.GetMisc();
			pumpStat.SetMisc(lMisc & MISC_NOT_HELP_REQUEST);
			_Module.m_server.m_cPumpArray[lParam1-1].SetStat(pumpStat);
			_Module.m_server.SavePumpInfo(lParam1);

			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}
		break;

		case PROCESS_REMOVE_MSG:
		//==============================================================
		{
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			CPumpStatus	pumpStat = _Module.m_server.m_cPumpArray[lParam1-1].GetStat();
			pumpStat.SetMessage(PUMP_NO_MSG);
			_Module.m_server.m_cPumpArray[lParam1-1].SetStat(pumpStat);
			_Module.m_server.SavePumpInfo(lParam1);

			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}
		break;

		case PROCESS_REMOVE_EVENT: //3.0.3.11
		//==============================================================
		{
			long lPOSNumber;
			CPosInfo tmpPos;
			POSITION position = _Module.m_server.m_cPosMap.GetStartPosition();

			while(position)
			{
				_Module.m_server.m_cPosMap.GetNextAssoc(position,lPOSNumber,tmpPos);
				if (lPOSNumber == lParam1)
				{
					//reset m_bOLAEvent for this POSNumber !!
					if(_Module.m_server.m_cPosMap.Lookup(lPOSNumber,tmpPos))
					{
						tmpPos.ResetOLAEvent();
						_Module.m_server.m_cPosMap.SetAt(lPOSNumber,tmpPos);
					}
					break;
				}
			}

		}
		break;
		case PROCESS_REMOVE_LATEST_PREPAY: //3.1.1.4
		{
			long lPumpNumber  = lParam1; 

			if(PUMP_NOT_VALID(lPumpNumber))
				*pVal = GPI_INVALID_PUMP_NUMBER;
			else
			{
				_Module.m_server.m_cPumpArray[lPumpNumber -1].SetLatestPrePayPosNumber(0);
				_Module.m_server.m_cPumpArray[lPumpNumber -1].SetLatestPrePaySyncNumber(0);
			}
		
		}
		break;
		//3.2.0.32
		case PROCESS_UPDATE_LOYALTY_ACTIVITY:
		{
			long lRtc = _PARAM.SetParam( "GeneralParam", 0,0,"ActiveLoyaltyDiscount",FIELD_DWORD,NULL,(lParam1 ? 1:0), FALSE); 			
	 		if(lRtc)
			{
				switch(lRtc)
				{
					case 1: *pVal = GPI_FAILED_TO_OPEN_TABLE;break;		 
					case 2: *pVal = GPI_PARAM_NOT_FOUND;break;		 
				}
				
			}
			else
			{					
				_Module.m_server.SetLoyaltyActivity(lParam1);

				if(_Module.m_server.GetServerState() >= SERVER_STARTUP)
				{
					CMD_STRUCT_UPDATE_PARAM cmdInfo;
					memset(&cmdInfo,0,sizeof(CMD_STRUCT_UPDATE_PARAM));
					strcpy(cmdInfo.szTableName,"GeneralParam");			
					cmdInfo.lKey1 = 0;
					cmdInfo.lKey2 = 0;
					strcpy(cmdInfo.szFieldName,"ActiveLoyaltyDiscount");			
					cmdInfo.nFieldType = FIELD_DWORD;						
					cmdInfo.lVal = (lParam1 ? 1:0);
					_Module.m_server.SetCommand(CMD_UPDATE_PARAM,SERVER_POS_NUMBER, 0,&cmdInfo,sizeof(CMD_STRUCT_UPDATE_PARAM));
				}
			}
		}
		break;

		case PROCESS_DELIVERY: 
		{
			//Setting the Delivery Process state machine with DES_START_LOCK as its
			//state and either PD_START or PD_END as its Delivery process flag - depends
			//on whether we're beginning a start delivery process or an end delivery process. 
			//All that refers to the regular situation where the delivery process wasn't aborted
			//in the middle. But when it was, we set the state machine to the previous state according
			//to the state machine rules and proceed from where we had stopped in the PumpSrv's previous session
		
			int nErrorRetreived = 0;
			
			//Checking the db connection
			if (FALSE == _Module.m_server.DBConnectionAvailable())
			{
				*pVal = GPI_FAILED_IO;
			}
			else
			{
				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString strLog;
					strLog.Format("CCommonFunction::SetProcess: Set Server Info. Delivery Status: %d, Delivery Process: %d.", 
						_Module.m_server.m_cServerInfo.GetDeliveryEmulationStatus(), _Module.m_server.m_cServerInfo.GetDeliveryEmulationProcessType());
					_LOGMSG.LogMsg(strLog);							
				}


				switch(_Module.m_server.m_cServerInfo.GetDeliveryEmulationStatus())  //4.0.8.500
				{
					case DES_IDLE:

						if(!_Module.m_server.SetDeliveryFlags(DES_START_LOCK, (BYTE) lParam1, nErrorRetreived))
						{
								if( GPI_DELIVERY_START_AFTER_START == nErrorRetreived)
									*pVal = GPI_DELIVERY_START_AFTER_START;
								else if ( GPI_DELIVERY_END_AFTER_END == nErrorRetreived)
									*pVal = GPI_DELIVERY_END_AFTER_END;
								else
									*pVal = GPI_NOT_VALID_IN_CURRENT_MODE; 
						}
						break;
					
					case DES_START_LOCK:
						//Checking if the current Server Info's byDeliveryEmulationProcess parameter fits the 
						//Process parameter passed to the function in the lParam1 parameter

						if (!CheckDeliveryProcessState(lParam1, lParam2, pVal))
						{
							if(PD_START == _Module.m_server.m_cServerInfo.GetDeliveryEmulationProcessType())
								_Module.m_server.m_cServerInfo.SetDeliveryEmulationProcessType(PD_NONE);
								
							_Module.m_server.m_cServerInfo.SetDeliveryEmulationStatus(DES_IDLE);
							
							if(LOG_DETAIL_FLOW_CONTROL)
							{
								_LOGMSG.LogMsg("Continuing delivery process from the DES_START_LOCK status");
							}
							
							
							if(!_Module.m_server.SetDeliveryFlags(DES_START_LOCK, (BYTE) lParam1, nErrorRetreived))
							{
								if( GPI_DELIVERY_START_AFTER_START == nErrorRetreived)
									*pVal = GPI_DELIVERY_START_AFTER_START;
								else if ( GPI_DELIVERY_END_AFTER_END == nErrorRetreived)
									*pVal = GPI_DELIVERY_END_AFTER_END;
								else
									*pVal = GPI_NOT_VALID_IN_CURRENT_MODE; 
							}
						}
						break;
					
					case DES_WAIT_FOR_LOCK: 
						//Checking if the current Server Info's byDeliveryEmulationProcess parameter fits the 
						//Process parameter passed to the function in the lParam1 parameter
						if (!CheckDeliveryProcessState(lParam1, lParam2, pVal))
						{
							_Module.m_server.m_cServerInfo.SetDeliveryEmulationStatus(DES_START_LOCK);

							if(LOG_DETAIL_FLOW_CONTROL)
							{
								_LOGMSG.LogMsg("Continuing delivery process from the DES_WAIT_FOR_LOCK status");
							}

							if(!_Module.m_server.SetDeliveryFlags(DES_WAIT_FOR_LOCK, (BYTE) lParam1, nErrorRetreived))
								*pVal = GPI_NOT_VALID_IN_CURRENT_MODE; 
						}
						break;
					
					case DES_WAIT_TIMEOUT: 

						//Checking if the current Server Info's byDeliveryEmulationProcess parameter fits the 
						//Process parameter passed to the function in the lParam1 parameter

						if (!CheckDeliveryProcessState(lParam1, lParam2, pVal))
						{
							_Module.m_server.m_cServerInfo.SetDeliveryEmulationStatus(DES_WAIT_FOR_LOCK);
							
							if(LOG_DETAIL_FLOW_CONTROL)
							{
								_LOGMSG.LogMsg("Continuing delivery process from the DES_WAIT_FOR_LOCK status");
							}
							
							if(!_Module.m_server.SetDeliveryFlags(DES_WAIT_TIMEOUT, (BYTE) lParam1, nErrorRetreived))
								*pVal = GPI_NOT_VALID_IN_CURRENT_MODE; 
						}
						break;
					
					case DES_REQUEST_TANK_READING:  //4.0.15.501
						{
							if (!CheckDeliveryProcessState(lParam1, lParam2, pVal))
							{
								_Module.m_server.m_cServerInfo.SetDeliveryEmulationStatus(DES_WAIT_TANK_READING);

								if(LOG_DETAIL_FLOW_CONTROL)
								{
									_LOGMSG.LogMsg("Continuing delivery process from the DES_WAIT_TANK_READING status");
								}

								if(!_Module.m_server.SetDeliveryFlags(DES_WAIT_FOR_LOCK, (BYTE) lParam1, nErrorRetreived))
									*pVal = GPI_NOT_VALID_IN_CURRENT_MODE; 
							}

						}
						break;

					case DES_WAIT_TANK_READING: 
						//Checking if the current Server Info's byDeliveryEmulationProcess parameter fits the 
						//Process parameter passed to the function in the lParam1 parameter
						if (!CheckDeliveryProcessState(lParam1, lParam2, pVal))
						{
							_Module.m_server.m_cServerInfo.SetDeliveryEmulationStatus(DES_WAIT_FOR_LOCK);
							
							if(LOG_DETAIL_FLOW_CONTROL)
							{
								_LOGMSG.LogMsg("Continuing delivery process from the DES_WAIT_FOR_LOCK status");
							}

							if(!_Module.m_server.SetDeliveryFlags(DES_WAIT_TIMEOUT, (BYTE) lParam1, nErrorRetreived))
								*pVal = GPI_NOT_VALID_IN_CURRENT_MODE;
						}
						break;
					
					case DES_UNLOCK: 
						//Checking if the current Server Info's byDeliveryEmulationProcess parameter fits the 
						//Process parameter passed to the function in the lParam1 parameter
						if (!CheckDeliveryProcessState(lParam1, lParam2, pVal))
						{			
							_Module.m_server.m_cServerInfo.SetDeliveryEmulationStatus(DES_WAIT_TANK_READING);
							
							if(LOG_DETAIL_FLOW_CONTROL)
							{
								_LOGMSG.LogMsg("Continuing delivery process from the DES_UNLOCK status");
							}

							if(!_Module.m_server.SetDeliveryFlags(DES_UNLOCK, (BYTE) lParam1, nErrorRetreived))
								*pVal = GPI_NOT_VALID_IN_CURRENT_MODE; 
						}
						break;
					
					default:
						*pVal = GPI_NOT_VALID_IN_CURRENT_MODE;
				}
			}
		}
		break;

		case PROCESS_DELIVERY_SINGLE_TANK: 
		{
			SetProcessDeliveryEmulationSingleTank(lParam2, lParam3, lParam4, lParam1, pVal);
			break;
		}

		case PROCESS_PRINTER_STATUS: 
		{
			static BOOL bPrinterOnlineReported = FALSE; 

			if(_Module.m_server.m_bControllerManagePrinterStatus)
			{
				CPosInfo tmpPos;
				long lPosNumber = lParam1;
				PrinterStatus lIsConnected = (PrinterStatus) lParam2;
				//Looking for the POS correspondent to the POS number requested  
				if(_Module.m_server.m_cPosMap.Lookup(lPosNumber, tmpPos) )
				{
					//Setting the POS with the printer status requested
					tmpPos.SetPrinterStatus(lIsConnected);
					_Module.m_server.m_cPosMap.SetAt(lPosNumber, tmpPos);

					//Checking if at least one printer is connected to a POS or not, and sending
					//the appropriate event according to it
					if(_Module.m_server.IsPrinterConnected() )
					{
						if(!bPrinterOnlineReported)
						{
							_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_PRINTER_ONLINE);
							bPrinterOnlineReported = TRUE;
						}
					}
					else
					{
						if(bPrinterOnlineReported == TRUE)
						{
							_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_PRINTER_OFFLINE);
							bPrinterOnlineReported = FALSE;
						}
					}
				}
				else
				{
					_LOGMSG.LogMsg("CCommonFunction::SetProcess: PROCESS_PRINTER_STATUS case - POS requested isn't valid");
					*pVal = GPI_INVALID_POS_NUMBER;
				}
			}
			else
			{
				_LOGMSG.LogMsg("CCommonFunction::SetProcess: PROCESS_PRINTER_STATUS case - Printers Status isn't managed");
				*pVal = GPI_NOT_VALID_IN_CURRENT_MODE; 
			}

			break;
		}
		case PROCESS_CLEAR_ULLAGE_REPORT:	 //	9 //4.0.9.555
		{
			_Module.m_server.dwPosRequestReport &= CLEAR_REPORT_TYPE_ULLAGE;  
			break;
		}
		case PROCESS_CLEAR_DELIVERY_REPORT :  //4.0.9.555
		{
			_Module.m_server.dwPosRequestReport &= CLEAR_REPORT_TYPE_DERIVERY;  
			break;
		}
		case PROCESS_DECLINE_PAP://4.0.12.502
		{
			_LOGMSG.LogMsg("SetProcess Found PROCESS_DECLINE_PAP");
			_Module.m_server.UpdateDeclinePAP(lParam1,lParam2);
			break;
		}
		case PROCESS_TERMINAL_SW_RESET://4.0.10.1000
		{
		if(lParam1>0)
			{
			
				if (_Module.m_server.m_cPumpArray[lParam1-1].GetDieselPumpFlags())
				{
					_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, lParam1, GCI_EVENT_TERMINAL_SW_RESET);
					CString str;
					if(LOG_BASIC_FLOW_CONTROL)
					{
						str.Format("SetProcess Found PROCESS_TERMINAL_SW_RESET for pump  %ld",lParam1);
						_LOGMSG.LogMsg(str);
					}
						
				}
				else
				{
					CString str;
					if(LOG_BASIC_FLOW_CONTROL)
					{
						str.Format("SetProcess Found PROCESS_TERMINAL_SW_RESET But the pump is not diesel pump %ld",lParam1);
						_LOGMSG.LogMsg(str);
					}
					*pVal = GPI_INVALID_PUMP_NUMBER;
				}
			}
			else
			{
				_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_TERMINAL_SW_RESET);
				if(LOG_BASIC_FLOW_CONTROL)
					_LOGMSG.LogMsg("SetProcess Found PROCESS_TERMINAL_SW_RESET send reset event for all Pumps");
			}

			break;
		}

		case PROCESS_CHANGE_PRICES:
		{
			switch (lParam1)
			{
				case PROCESS_PRICE_CHANGE_FAILURE:
				{

					if (LOG_DETAIL_FLOW_CONTROL)		
					{
						CString sMsg;
						sMsg.Format("Got Failure in price change process. lParam2 = %ld",lParam2);
						_LOGMSG.LogMsg(sMsg,LOG_LEVEL_1);
					}

					//Init the device num of the failure price change
					if (lParam2 & PRICE_CHANGE_COMMIT_PUMPS_ONLY)
					{
 						_Module.m_server.SetPriceChangeResponse(GOT_PRICE_CHANGE_PENDING_COMMIT_AFTER_FAILURE);	
						_Module.m_server.m_cServerInfo.SetInPriceChangeWithIntegrityProcessFlags(PRICE_CHANGE_WITH_INTEGRITY_PENDING_COMMIT_AFTER_FAILURE); //4.0.27.20
					}
					else
					{
						//If we still in the original price change - only set the flag to do failure of process in the end of the price change process.  
						if (_Module.m_server.IsInOriginalPriceChangeProcess())
						{
 							_Module.m_server.SetPriceChangeResponse(GOT_PRICE_CHANGE_FAILURE);								
						}
						else
						{
							_Module.m_server.PriceChangeFailure();
						}
					}
					
					SQLPumpSrvFile.SaveServerInfo();

					
					break;
				}

				case PROCESS_PRICE_CHANGE_SUCCESS:
				{

					if (LOG_DETAIL_FLOW_CONTROL)		
					{
						CString sMsg;
						sMsg.Format("Got Success in price change process");
						_LOGMSG.LogMsg(sMsg);
					}

					_Module.m_server.m_cEventManager.AddEvent(CTRL_PRICE_CHANGE_PUMPS_SUMMARY,0);   //4.0.32.??? CR 405462

					//If we still in the original price change - only set the flag to do success of process in the end of the price change process.  
					if (_Module.m_server.IsInOriginalPriceChangeProcess())
					{
						_Module.m_server.SetPriceChangeResponse(GOT_PRICE_CHANGE_SUCCESS);								
					}
					else
					{
						_Module.m_server.PriceChangeSuccess();
					}
					
					_Module.m_server.m_cServerInfo.SetInPriceChangeWithIntegrityProcessFlags(PRICE_CHANGE_WITH_INTEGRITY_GOT_COMMIT);
					
					break;
				}
				case PROCESS_PRICE_CHANGE_NO_CHANGE:
				{

					if (LOG_DETAIL_FLOW_CONTROL)
					{
						CString sMsg;
						sMsg.Format("Price change process no change Event");
						_LOGMSG.LogMsg(sMsg);
					}
					//Inot the device num of the succedd price change
					_Module.m_server.SetPriceChangeResponseDeviceNum(lParam2);
					_Module.m_server.PriceChangeNoChangeEvent();
					break;
				}

				case PROCESS_PRICE_CHANGE_COMMIT: //4.0.26.508 147183
				{
					long lCurrentResponse = _Module.m_server.GetPriceChangeResponse();
					CString sMsg;
					sMsg.Format("Got price change commit process, lParam2 = %ld, current price change response = %ld", lParam2, lCurrentResponse); 
					_LOGMSG.LogMsg(sMsg);

				
					if ((lCurrentResponse == GOT_PRICE_CHANGE_PENDING_COMMIT) || (lCurrentResponse == GOT_PRICE_CHANGE_PENDING_COMMIT_AFTER_FAILURE)) 
					{
						if(lCurrentResponse == GOT_PRICE_CHANGE_PENDING_COMMIT_AFTER_FAILURE)	//4.0.27.30 TD 222751
							_Module.m_server.PriceChangePendingCommitAfterFailure();

						if (lParam2 & PRICE_CHANGE_COMMIT_PUMPS_ONLY)
						{
							_Module.m_server.SetPriceChangeResponse(PRICE_CHANGE_NO_RESPONSE);							
							_Module.m_server.PriceChangeCommit();

							SQLPumpSrvFile.SaveServerInfo();


						}
					}
					else
					{
						//return error to POS - if we already got one commit, or not in a relevant status to receive a commit.
						//TD 236805 - 4.0.27.502
						*pVal = GPI_NOT_VALID_IN_CURRENT_MODE;
					}					

					break;
				}
				
				case PROCESS_PRICE_CHANGE_COMPLETE: //4.0.26.508 147183
				{
					CString sMsg;
					sMsg.Format("Got price change complete process");
					_LOGMSG.LogMsg(sMsg);
					
					_Module.m_server.SetPriceChangePOSMsg(_Module.m_server.GetPriceChangeResponse()); //in order to show pop up in POS
					_Module.m_server.SetPriceChangeResponse(PRICE_CHANGE_NO_RESPONSE);	 
					_Module.m_server.m_cServerInfo.SetInPriceChangeWithIntegrityProcessFlags(PRICE_CHANGE_WITH_INTEGRITY_NONE); 
				
					SQLPumpSrvFile.SaveServerInfo();

					break;
				}
				
				case PROCESS_PRICE_CHANGE_MSG_ACK:
				{
					_Module.m_server.SetPriceChangePOSMsg(0);

					break;
				}
			}
			break;
		}

		case PROCESS_TERMINAL_SW_DOWNLOAD:
		{
			CString sMsg;
			sMsg.Format("Got terminal download process");
			_LOGMSG.LogMsg(sMsg);

			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, lParam1, GCI_EVENT_TERMINAL_SW_DOWNLOAD);

			break;
		}

		case PROCESS_CLOSE_BATCH:		//4.0.16.502 - TD 35765
		{
			_LOGMSG.LogMsg("Got close batch process");

			*pVal = ClearCashAcceptorData(lParam1, lParam2);

			break; //4.0.17.501
		}
		
		case PROCESS_OLA_CLOSE_BATCH:		//4.0.19.502 - CR 58096
		{
			CString sMsg;
			sMsg.Format("Got OLA close batch process");
			_LOGMSG.LogMsg(sMsg);

			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_OLA_CLOSE_BATCH);
			break;
		}

		//4.0.22.500
		case PROCESS_FORCE_RELEASE_TRS:
		{
			CString sMsg;
			sMsg.Format("Got Force Release trs process");
			_LOGMSG.LogMsg(sMsg);

			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, lParam1, GCI_EVENT_FORCE_RELEASE_TRS);

			break;
		}

		case PROCESS_PUMP_RESET:		//4.0.19.502 - CR 58649
		{
			// On call from CL lParam2 is flag

			CString sMsg;
			sMsg.Format("Got Pump Reset process, pump = %ld, flags = %ld", lParam1, lParam2);
			_LOGMSG.LogMsg(sMsg);

			_Module.m_server.ResetPumpStatQdxData(lParam1, lParam2 | RESET_PUMP_INFO);			//4.0.23.508 TD 89049

			_Module.m_server.ResetExtraInfoQdxData();											//4.0.23.510 no TD

			break;
		}
		
		//4.0.22.503 - TD 72189
		case PROCESS_CHANGE_PRODUCT_IN_TANK:
		{
			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString sMsg;
				sMsg.Format("Change product in tank process - CL Response arrived. Response=%ld", lParam1);
				_LOGMSG.LogMsg(sMsg);
			}

			// Change new state of the process to continue
			_Module.m_server.m_cServerInfo.SetChangeProductInTankState(CPIT_HANDLE_CL_RESPONSE);
			if (lParam1)
				_Module.m_server.m_cServerInfo.SetChangeProductInTankFailureResult(CPIT_ERROR_CL_RESPONSE_FAILURE); // Failure

			break;
		}
		
		//4.0.19.820 - TD 78775
		case PROCESS_PRICE_CHANGE_WITH_ACK_PER_PUMP:
		{
			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString sMsg;
				sMsg.Format("SetProcess. Price change with Ack per pump process. Pump=%ld, Error Code=%ld", lParam1, lParam2);
				_LOGMSG.LogMsg(sMsg);
			}
			_Module.m_server.m_cServerInfo.SetPriceChangeAckResult(lParam1, lParam2);
			break;
		}
		
		//4.0.22.504 - TD 70956
		case PROCESS_ADD_CASHIER:
		{
			if(LOG_DETAIL_FLOW_CONTROL)
				_LOGMSG.LogMsg("Got SetProcess Cashier Sign In");

			_Module.m_server.m_cFuelingRestriction.CashierSignIn();
			break;
		}

		//4.0.22.504 - TD 70956
		case PROCESS_REMOVE_CASHIER:
		{
			if(LOG_DETAIL_FLOW_CONTROL)
				_LOGMSG.LogMsg("Got SetProcess Cashier Sign Out");

			_Module.m_server.m_cFuelingRestriction.CashierSignOut();
			break;
		}

		//4.0.22.504 - TD 70956
		case PROCESS_RESET_CASHIER:
		{
			if(LOG_DETAIL_FLOW_CONTROL)
				_LOGMSG.LogMsg("Got SetProcess Reset Cashiers Counter");

			_Module.m_server.m_cFuelingRestriction.ResetCashiersCounter();
			break;
		}

        // 4.0.28.502 - TD 256429
		case PROCESS_TERMINAL_LOGON:
		{
			_LOGMSG.LogMsg("Got terminal logon process");
			_Module.m_server.ProcessTerminalCmd( TERMINAL_CMD_LOGIN, lParam1);
			break;
		}

		case PROCESS_TERMINAL_HW_RESET:     // 4.0.31.64
		{

			CString sMsg;
			sMsg.Format("Got terminal reset process, terminal = %ld", lParam1);
			_Module.m_server.m_cLogger.LogMsg(sMsg);

			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, lParam1, GCI_EVENT_TERMINAL_HW_RESET);

			break;
		}

        case PROCESS_OLA_ONLINE:     //4.0.31.?? - R10 User Story - 92780
		{		
			_LOGMSG.LogMsg("Got Ola online process.");				
			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_OLA_ONLINE);				
			break;
		}

		case PROCESS_OLA_OFFLINE:     //4.0.31.?? - R10 User Story - 92780
		{
			
			_LOGMSG.LogMsg("Got Ola offline process.");		
			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_OLA_OFFLINE);
			break;
		}

		case PROCESS_RECONFIGURE:
			{
				CString sMsg;			
				sMsg.Format("Received EOS Reconfigure event for Table ID:%d, Table Name:%s",GetBitPosition (lParam1), _Module.m_server.ReconfigureTableIdToString(lParam1));
				_Module.m_server.m_cLogger.LogMsg(sMsg);
				
				_Module.m_server.UnRegisterEOSReconfigureEvent (lParam1);
			}
		break;

		case PROCESS_NOZZLE_UP:
		{
			if (LOG_FCC_FLOW_CONTROL)
				_Module.m_server.m_cLogger.LogMsg("Got SetProcess NOZZLE UP.");

			SetPumpNozzleNotification(lParam1, lParam2);  // PumpNumber, lFlagIsOn

			break;
		}

		default : 
		//==============================================================
			*pVal = GPI_INVALID_PROCESS_NUMBER;
		break;
	}

}


/******************************************************************************
 Description:	Close Batch and clean cash acceptor data
 Returns:      	
 Parameters:  
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			13/3/2007   12:00		Start		4.0.16.502 - TD 35765
******************************************************************************/
long CCommonFunction::ClearCashAcceptorData(long lParam1, long lParam2)
{
	GPIRetCode retCode = GPI_OK;

	if (!_Module.m_server.IsCashAcceptorreportEnable())
		retCode = GPI_CASH_ACCEPTOR_REPORT_NOT_ACTIVE;

	_Module.m_server.MoveCashAcceptorShift();
	_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_CASH_ACCEPTOR_REPORT_IS_READY,0);				

	return retCode;
}


long CCommonFunction::GetReport(long lShiftNumber, long lState, long lPosNumber, long lFlags, long lVal, SYSTEMTIME &cStartTime, SYSTEMTIME &cEndTime, long *lRecordCount, BSTR &sRec)
{
	long lRtc; 
	lRtc = _Module.m_server.m_cAccumulationShiftTotal.GetReport(lShiftNumber,lState,lPosNumber,lFlags,lVal ,cStartTime, cEndTime, lRecordCount, sRec);

	return lRtc;

}

long CCommonFunction::SetRecordStatus(long lShiftNumber, long lState, long lPosNumber, long lFlags)
{
	long lRtc; 

	lRtc = _Module.m_server.m_cAccumulationShiftTotal.SetRecordStatus(lShiftNumber, lState, lPosNumber, lFlags);
	return lRtc;
}
/******************************************************************************
 Description:	Generate row data information  of transaction  PAK /PrePay / Attendant 	
 Returns:      	
 Parameters:  BSTR *sData      - out buffer of report 
              long *plRecCount - total record found 
			  long lFlags 
			  BSTR sRestrictionData -  restriction data  	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			25/5/2004   9:58		Start
******************************************************************************/
GetPAKQueryRetCode CCommonFunction::GetPAKQuery(BSTR *sData, long *plRecCount, long *lFlags, BSTR sRestrictionData , long lPumpNumber) //4.0.9.507
{
	long lTrsCount = 0;
	long lNumTrs  = 0;
	long lVersion = 0;
	GetPAKQueryRetCode eRetVal = GetPakQuery_OK ; 
	int iPump = 0 , iTrs = 0;
	long lLastValidPump = 0;
	BOOL bUnBalancePrepay = FALSE;
	BOOL bWaitPrepay = FALSE;
	BOOL bAuthorizedPrepay = FALSE; // TD 38806 - 4.0.16.501
	BOOL bInProgressPrePay = FALSE;

	lLastValidPump = _Module.m_server.GetLastValidPump();//4.0.3.37

	CComBSTR bstrTransactions;
	bstrTransactions.Empty();

	CPumpTransact cTmpTrs;

	//TD 38654
	TAG_PUMP_TRANSACT5 cTagPumpTrs;
	memset(&cTagPumpTrs , ' ' , sizeof(cTagPumpTrs));

	char sTmpStr[sizeof(TAG_PUMP_TRANSACT2) + 2];
	memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT2) , 0 , 2);

	lVersion = 2;

	if(eRetVal == GetPakQuery_OK)
	{
		if( *lFlags & PAK_QUERY_UNPAID_PAK )
		{
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			//We may make the version depend on the lFlags parameter
			//in the future

			if (lPumpNumber)		//4.0.9.507
				iPump =  lPumpNumber-1;				//?.??.?.?? - TD 335186
			
			for(;iPump < lLastValidPump; iPump++)
			{
				if ((lPumpNumber) && (iPump >= lPumpNumber))		//4.0.9.507 //?.??.?.?? - TD 335186
					break;

				lNumTrs = _Module.m_server.m_cPumpArray[iPump].GetTrsArraySize();

				for(;iTrs < lNumTrs;iTrs++)
				{
					_Module.m_server.m_cPumpArray[iPump].GetTrsByIndex(iTrs , &cTmpTrs);
#ifdef DEBUG		
					CString str; str.Format("1. transaction %ld ,sMaxDiscountAllowed %ld", cTmpTrs.m_lNumber, cTmpTrs.m_sMaxDiscountAllowed); if (LOG_ALL_FLOW_CONTROL) _LOGMSG.LogMsg(str);
#endif
					//RFUEL-3309 Get the maximal discount allowed for the grade 
					SetMaximalAllowedDiscountToPumpTransaction(&cTmpTrs);
					if((cTmpTrs.m_lRes == PAY_AT_KIOSK) && 
					   (cTmpTrs.GetStatus() == UNPAID) && (!cTmpTrs.m_lOnOffer) )//4.0.3.46
					{
						
						cTmpTrs.GetTag(&cTagPumpTrs , lVersion);
						// TD 38654 - Convert the tag from TAG_PUMP_TRANSACT5 back to TAG_PUMP_TRANSACT2:
						TAG_PUMP_TRANSACT2	tmpTrs2;
						memset(&tmpTrs2,' ', sizeof(TAG_PUMP_TRANSACT2)); 
						ConvertFromTrs5ToTrs2(&cTagPumpTrs,&tmpTrs2);
						lTrsCount++;

						memcpy((BYTE*)sTmpStr , &tmpTrs2 , sizeof(tmpTrs2));

						bstrTransactions.Append((LPCSTR)sTmpStr);
						
						//TD 38654
						memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT2) , 0 , 2);
					}
				}

				//Reset trs index
				iTrs = 0;
			}

			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}
		//4.0.3.45 - check if need to read from prepay qdex
		if ((*lFlags & PAK_QUERY_UNBALANCE_PREPAY) || (*lFlags & PAK_QUERY_WAIT_PREPAY) || (*lFlags & PAK_QUERY_AUTH_PREPAY) || (*lFlags & PAK_QUERY_IN_PROGRESS_PREPAY))
		{
			long lRtc = 0;
			PRE_PAY_INFO info;
			lRtc = _Module.m_server.IndexOperation(PUMP_PRE_PAY_FILE,REC_READ_FIRST,&cTmpTrs,&info);
			
			while (!lRtc)
			{
				bUnBalancePrepay = FALSE;
				bWaitPrepay = FALSE;
				bAuthorizedPrepay = FALSE;
				bInProgressPrePay = FALSE;

				if ((info.lPumpNumber == lPumpNumber) || (!lPumpNumber))		//4.0.9.507
				{
					if( _Module.m_server.IsUnbalancedPrepay(&cTmpTrs,&info) && (*lFlags & PAK_QUERY_UNBALANCE_PREPAY) )
						bUnBalancePrepay = TRUE;
					if ( (cTmpTrs.m_lStatus == WAIT_AUTH || cTmpTrs.m_lStatus == WAIT_TIME_OUT || cTmpTrs.m_lStatus == WAIT_RES ) && (*lFlags & PAK_QUERY_WAIT_PREPAY) )
						bWaitPrepay = TRUE;	
					
					// TD 38806 - 4.0.16.501
					if( (cTmpTrs.m_lStatus == WAIT_PROGRESS) && (*lFlags & PAK_QUERY_AUTH_PREPAY) )
						bAuthorizedPrepay = TRUE;

					if ((cTmpTrs.m_lStatus == IN_PROGRESS) && (*lFlags & PAK_QUERY_IN_PROGRESS_PREPAY))
						bInProgressPrePay = TRUE;

					if(bUnBalancePrepay || bWaitPrepay || bAuthorizedPrepay || bInProgressPrePay) //4.0.16.501
					{
						cTmpTrs.GetTag(&cTagPumpTrs , lVersion, info.lMaxValue);
						
						// TD 38654 - Convert the tag from TAG_PUMP_TRANSACT5 back to TAG_PUMP_TRANSACT2:
						TAG_PUMP_TRANSACT2	tmpTrs2;
						memset(&tmpTrs2,' ', sizeof(TAG_PUMP_TRANSACT2)); 
						ConvertFromTrs5ToTrs2(&cTagPumpTrs,&tmpTrs2);

						lTrsCount++;

						memcpy((BYTE*)sTmpStr , &tmpTrs2 , sizeof(tmpTrs2));

						bstrTransactions.Append((LPCSTR)sTmpStr);
						
						memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT2) , 0 , 2);	//TD 38654
					}
				}
				lRtc = _Module.m_server.IndexOperation(PUMP_PRE_PAY_FILE,REC_READ_NEXT,&cTmpTrs,&info);
			}
		}
		
	}

	if (*lFlags & PAK_QUERY_FOR_ATTENDANT)	//4.0.5.42
	{
		EXTRA_INFO_REC sExtraInfoRec;
		char szTrack2[MAX_TABLE_NAME]; 
		char szTmpKey[MAX_TABLE_NAME]; 
		memset(szTrack2,' ',sizeof(szTrack2));
		memset(szTmpKey,' ',sizeof(szTmpKey));
		DWORD dwLastErr=0;
		int iLen = 0;

		if (sRestrictionData)
		{
			 iLen = WideCharToMultiByte(CP_ACP,0,sRestrictionData,-1,szTrack2,sizeof(szTrack2),NULL,NULL);
		}

		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			eRetVal = GetPakQuery_MissingData;
		}			

		if(eRetVal == GetPakQuery_OK)
		{
			long lRtc = 0;
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			//We may make the version depend on the lFlags parameter
			//in the future
			
			iPump =  lPumpNumber; // .4.0.9.507

			for(;iPump < lLastValidPump; iPump++)
			{
				lNumTrs = _Module.m_server.m_cPumpArray[iPump].GetTrsArraySize();

				if ((lPumpNumber) && (iPump > lPumpNumber))		//4.0.9.507
					break;

				for(;iTrs < lNumTrs;iTrs++)
				{
					_Module.m_server.m_cPumpArray[iPump].GetTrsByIndex(iTrs , &cTmpTrs);
#ifdef DEBUG		
					CString str; str.Format("2. transaction %ld ,sMaxDiscountAllowed %ld", cTmpTrs.m_lNumber, cTmpTrs.m_sMaxDiscountAllowed); if (LOG_ALL_FLOW_CONTROL) _LOGMSG.LogMsg(str);
#endif
					//RFUEL-3309 Get the maximal discount allowed for the grade 
					SetMaximalAllowedDiscountToPumpTransaction(&cTmpTrs);
					if((cTmpTrs.m_lRes == PAY_AT_KIOSK) && 
					   (cTmpTrs.m_lResExt == RES_EXT_ATTENDANT) && (!cTmpTrs.m_lOnOffer) )
					{					
						sExtraInfoRec.info.lIndexNumber = cTmpTrs.m_lNumber; 
						lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE,REC_READ, &sExtraInfoRec);
						 
						//Verify that we found the correct attendant details 
						if (!lRtc) 
						{
							if(*lFlags & PAK_QUERY_FOR_ATTENDANT_BY_ID)
								memcpy(szTmpKey,sExtraInfoRec.info.sAttendantID,sizeof(sExtraInfoRec.info.sAttendantID));
							else
								memcpy(szTmpKey,sExtraInfoRec.info.sTrack2Data,sizeof(sExtraInfoRec.info.sTrack2Data));

							if(!memcmp(szTmpKey ,szTrack2 , sizeof(sExtraInfoRec.info.sTrack2Data)))
							{
								cTmpTrs.GetTag(&cTagPumpTrs , lVersion);

								// TD 38654 - Convert the tag from TAG_PUMP_TRANSACT5 back to TAG_PUMP_TRANSACT2:
								TAG_PUMP_TRANSACT2	tmpTrs2;
								memset(&tmpTrs2,' ', sizeof(TAG_PUMP_TRANSACT2)); 
								ConvertFromTrs5ToTrs2(&cTagPumpTrs,&tmpTrs2);
								
								lTrsCount++;

								memcpy((BYTE*)sTmpStr , &tmpTrs2 , sizeof(tmpTrs2));

								bstrTransactions.Append((LPCSTR)sTmpStr);
								
								memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT2) , 0 , 2);	// TD 38654
							}
						}
					}
				}

				//Reset trs index
				iTrs = 0;
			}

			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}		
	}

	if(plRecCount)
		*plRecCount = lTrsCount;

	if(sData)
		*sData = bstrTransactions.Copy();

	return eRetVal;
}


/******************************************************************************
 Description:	Get station  status report  	
 Returns:      	None
 Parameters:   	long *lPumpMap1 - Map bit of pump 1  - 32
				long *lPumpMap2 - Map bit of pump 33 - 64
				long lFlags
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			8/9/2004   16:31		Start
******************************************************************************/
void CCommonFunction::GetStationStatus(long *lPumpMap1, long *lPumpMap2, long lFlags, long *pRetVal)
{
	//4.0.22.504 - TD 72710
	if (lFlags & STATION_STATUS_OFFLINE_PUMPS)
	{
		GetStationStatusPumpOffline(lPumpMap1, lPumpMap2);		
	}
	else
	{		
		if (lFlags & STATION_STATUS_UNPAID_TRS_ON_PUMPS)	
		{//4.0.29.501 284709
			GetStationStatusUnpaidTrsOnPump(lPumpMap1, lPumpMap2); 
		}
		else
			GetStationStatusPumpNotIdle(lPumpMap1, lPumpMap2);		
	}
}

/******************************************************************************
 Description:	Get station  status for pumps not idle  	
 Returns:      	None
 Parameters:   	long *lPumpMap1 - Map bit of pump 1  - 32
				long *lPumpMap2 - Map bit of pump 33 - 64
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			31/03/2009   15:30		Start	4.0.22.504 - TD 72710
******************************************************************************/
void CCommonFunction::GetStationStatusPumpNotIdle(long *lPumpMap1, long *lPumpMap2)
{
	unsigned long lBitMask = 1;

	int iMapLength = 32; //sizeof(long) * 8;
	
	*lPumpMap1 = *lPumpMap2 = 0;

	short nPumps = (short)_Module.m_server.GetMaxPumps() - 1;

	//The loop does not include 0, because the least
	//significant bit does not need to be shifted.
	for(short i = nPumps;i > 0;i--)
	{
		if(i < iMapLength)
		{
			if(!IsPumpIdle(i))
				*lPumpMap1 = *lPumpMap1 | lBitMask;

			*lPumpMap1 = *lPumpMap1 << 1;
		}

		else
		{
			if(!IsPumpIdle(i))
				*lPumpMap2 = *lPumpMap2 | lBitMask;
			
			if(i != iMapLength)
				*lPumpMap2 = *lPumpMap2 << 1;
		}
	}

	if(!IsPumpIdle(0))
		*lPumpMap1 = *lPumpMap1 | lBitMask;
}

/******************************************************************************
 Description:	Get station  status for pumps with unpaid trs  	
 Returns:      	None
 Parameters:   	long *lPumpMap1 - Map bit of pump 1  - 32
				long *lPumpMap2 - Map bit of pump 33 - 64
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			19/12/2011   12:33		Start	4.0.29.501 - TD 284709
******************************************************************************/
void CCommonFunction::GetStationStatusUnpaidTrsOnPump(long *lPumpMap1, long *lPumpMap2)
{
	unsigned long lBitMask = 1;

	int iMapLength = MAX_PUMPS; //sizeof(long) * 8;
	
	*lPumpMap1 = *lPumpMap2 = 0;

	short nPumps = (short)_Module.m_server.GetMaxPumps() - 1;
	short nWaiting = 0 , nFree = 0 , nUnpaid = 0;

	//The loop does not include 0, because the least
	//significant bit does not need to be shifted.
	for(short i = nPumps;i > 0;i--)
	{
		nWaiting = 0; 
		nFree = 0; 
		nUnpaid = 0;
		_Module.m_server.m_cPumpArray[i].GetTrsListCount(&nWaiting, &nFree, &nUnpaid);

		if(i < iMapLength)
		{

			if(nUnpaid)
				*lPumpMap1 = *lPumpMap1 | lBitMask;

			*lPumpMap1 = *lPumpMap1 << 1;
		}

		else
		{
			if(nUnpaid)
				*lPumpMap2 = *lPumpMap2 | lBitMask;
			
			if(i != iMapLength)
				*lPumpMap2 = *lPumpMap2 << 1;
		}
	}

	_Module.m_server.m_cPumpArray[0].GetTrsListCount(&nWaiting, &nFree, &nUnpaid);
	if(nUnpaid)
		*lPumpMap1 = *lPumpMap1 | lBitMask;

}

/******************************************************************************
 Description:	Get station  status for offline pumps
 Returns:      	None
 Parameters:   	long *lPumpMap1 - Map bit of pump 1  - 32
				long *lPumpMap2 - Map bit of pump 33 - 64
				 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			31/03/2009   15:30		Start	4.0.22.504 - TD 72710
******************************************************************************/
void CCommonFunction::GetStationStatusPumpOffline(long *lPumpMap1, long *lPumpMap2)
{
	unsigned long lBitMask = 1;

	int iMapLength = 32;
	
	*lPumpMap1 = *lPumpMap2 = 0;

	long lPumps = _Module.m_server.GetMaxPumps();

	for(long i = lPumps;i > 1;i--)
	{
		if(i < iMapLength)
		{
			// In case pump is valid, then check if it's offline
			// Not valid pump will not be marked as offline
			if ((_Module.m_server.m_cPumpArray[i-1].GetStat().GetStatus() != UNKNOWN_STATUS) &&
				(_Module.m_server.PumpNotInWorkingState(i)))
				*lPumpMap1 = *lPumpMap1 | lBitMask;

			*lPumpMap1 = *lPumpMap1 << 1;
		}

		else
		{
			if ((_Module.m_server.m_cPumpArray[i-1].GetStat().GetStatus() != UNKNOWN_STATUS) &&
				(_Module.m_server.PumpNotInWorkingState(i)))
				*lPumpMap2 = *lPumpMap2 | lBitMask;
			
			if(i != iMapLength)
				*lPumpMap2 = *lPumpMap2 << 1;
		}
	}

	if ((_Module.m_server.m_cPumpArray[1].GetStat().GetStatus() != UNKNOWN_STATUS) &&
		(_Module.m_server.PumpNotInWorkingState(1)))
		*lPumpMap1 = *lPumpMap1 | lBitMask;
}

/******************************************************************************
 Description:	Return the delivery emulation status for a single tank  	
 Returns:      	None
 Parameters:   	lTankNumber - The tank number
				plStatus    - Returned value of the status
				pRec        - Returned value of the Xml with tank information
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			06/10/2008   12:00		Start 4.0.21.500 TD 66935
******************************************************************************/
long CCommonFunction::GetStatusSingleTank(long lTankNumber)
{
	long retCode        = CTRL_IDLE;
	long lSuccessType   = _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankSuccess(lTankNumber);
	long lProcessStatus = _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankStatus(lTankNumber);
	
	if (lSuccessType == DESF_SINGLE_TANK_DONE)
		retCode = CTRL_READING_PROCESS_SUCCESS;
	else
		if (lSuccessType == DESF_SINGLE_TANK_TIMEOUT)
			retCode = CTRL_READING_PROCESS_TIMEOUT;
		else
			if (lSuccessType == DESF_SINGLE_TANK_FAILURE)
				retCode = CTRL_READING_PROCESS_FAILURE;
			else
				switch (lProcessStatus)
			{
                case DES_SINGLE_TANK_START_LOCK:
					{
						retCode = CTRL_READING_PROCESS_STARTED;
						break;
					}
				
				case DES_SINGLE_TANK_WAIT_FOR_LOCK:
					{ 
						retCode = CTRL_WAIT_FOR_CONNECTED_PUMPS_LOCK;
				        break;
					}
				
				case DES_SINGLE_TANK_WAIT_TIMEOUT:
					{
						retCode = CTRL_CONNECTED_PUMPS_LOCKED;
						break;
					}
				
				case DES_SINGLE_TANK_REQUEST_TANK_READING:
					{
						retCode = CTRL_WAIT_TANK_READING;
						break;
					}
				
				case DES_SINGLE_TANK_WAIT_TANK_READING:
					{
						retCode = CTRL_WAIT_TANK_READING;
						break;
					}
				
				case DES_SINGLE_TANK_UNLOCK:
					{
						retCode = CTRL_WAIT_FOR_CONNECTED_PUMPS_UNLOCK;
						break;
					}
			}

	return retCode;

}

/******************************************************************************
 Description:	Return the delivery emulation status for a single tank  	
 Returns:      	None
 Parameters:   	lTankNumber - The tank number
				plStatus    - Returned value of the status
				pRec        - Returned value of the Xml with tank information
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			06/10/2008   12:00		Start 4.0.21.500 TD 66935
******************************************************************************/
void CCommonFunction::BulidDeliverySingleTankXML(long lTankNumber, long lStatus, BSTR *pRec)
{
	BOOL bAddReadings = FALSE;
	BOOL bAddDeliveryIndication = FALSE;

	long lProcessType   = _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessType(lTankNumber);
	long lProcessStatus = _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankStatus(lTankNumber);

	//Build basic XML (Number, Reference);
	if (lStatus == CTRL_READING_PROCESS_SUCCESS) //reading process ended
	{
		bAddReadings = TRUE; //Add reading XML
		
		//If we are in the success of END:
		if (((lProcessType == PD_SINGLE_TANK_END) && (lProcessStatus != DES_SINGLE_TANK_IDLE)) || (lProcessType == PD_SINGLE_TANK_NONE))
			bAddDeliveryIndication = TRUE; //Add "Delivery" element
	}
	_Module.m_server.m_cTankXML.TankReadingInfoDeliveryXml(&_Module.m_server.m_cTankArray[lTankNumber-1], pRec, bAddReadings, bAddDeliveryIndication);
}

/******************************************************************************
 Description:	Reset the Delivery Emulation Single Tank Process Data 
                (all the data relevant for the process)  	
 Returns:      	None
 Parameters:   	lTankNumber - The tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			15/10/2008   17:20		Start 4.0.21.500 TD 66935
******************************************************************************/
void CCommonFunction::ResetDeliveryEmulationSingleTankProcessData(const long lTankNumber)
{
	_Module.m_server.m_cServerInfo.ClearSingleSpecificTankFlags(lTankNumber);
	_Module.m_server.m_cTankArray[lTankNumber-1].ResetDeliveryEmulationSingleTank();

}

/******************************************************************************
 Description:	Reset the Delivery Emulation Single Tank Reading Process Out Data 
                (all the data relevant for the reading process, which is external =>
				used for notify external applications about the reading data).

 Returns:      	None
 Parameters:   	lTankNumber - The tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			04/11/2008   14:35		Start 4.0.21.500 TD 66935
******************************************************************************/
void CCommonFunction::ResetDeliveryEmulationSingleTankReadingOutData(const long lTankNumber)
{
	_Module.m_server.m_cServerInfo.SetDeliveryEmulationSingleTankSuccess(lTankNumber, DESF_SINGLE_TANK_NOT_DONE);
	_Module.m_server.m_cServerInfo.SetDeliveryEmulationSingleTankProcessStopped(lTankNumber, SINGLE_TANK_PROCESS_NOT_STOPPED);
	_Module.m_server.m_cTankArray[lTankNumber-1].ResetFoundDelivery();
}

/******************************************************************************
 Description:	Reset the Delivery Emulation Single Tank Reading Process In Data 
                (all the data relevant for the reading process, which is internal)  	
 Returns:      	None
 Parameters:   	lTankNumber - The tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			15/10/2008   17:20		Start 4.0.21.500 TD 66935
******************************************************************************/
void CCommonFunction::ResetDeliveryEmulationSingleTankReadingInData(const long lTankNumber)
{
	_Module.m_server.m_cServerInfo.SetDeliveryEmulationSingleTankStatus(lTankNumber, DES_SINGLE_TANK_IDLE);
	_Module.m_server.m_cServerInfo.SetSingleTankLockStatus(lTankNumber, SINGLE_TANK_UNLOCKED);
	_Module.m_server.m_cTankArray[lTankNumber-1].ResetLockConnectedPumps();
}

/******************************************************************************
 Description:	Return the delivery emulation status for a single tank  	
 Returns:      	None
 Parameters:   	lTankNumber - The tank number
				plStatus    - Returned value of the status
				pRec        - Returned value of the Xml with tank information
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			06/10/2008   12:00		Start 4.0.21.500 TD 66935
******************************************************************************/
void CCommonFunction::GetDeliveryEmulationStatusSingleTank(long lTankNumber, long *plStatus, BSTR *pRec)
{
	*plStatus = GetStatusSingleTank(lTankNumber);

	BulidDeliverySingleTankXML(lTankNumber, *plStatus, pRec);
}

/******************************************************************************
 Description:	Ascertain whether a pump is idle 	
 Returns:      	bool
 Parameters:   	short nPump
 Assumptions:   nPump is a valid pump number.  Must be protected by
			    a critical section, mutex or semaphore.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			17/12/2001   12:31		Start
 Eyal           3.2.0.74 1/5/02         Revised function to check for 
                                        open transactions
******************************************************************************/
BOOL CCommonFunction::IsPumpIdle(short nPump)
{
	BOOL bRetCode = FALSE;
	PumpStatus ePumpStat;
	PumpReservation eResStat;
	PumpResExt ePumpResExt;  //4.0.8.505
	CPumpTransact cTrs;

	ePumpStat = _Module.m_server.m_cPumpArray[nPump].GetStat().GetStatus();
	eResStat = _Module.m_server.m_cPumpArray[nPump].GetStat().GetRes();
	ePumpResExt = _Module.m_server.m_cPumpArray[nPump].GetStat().GetResExt(); //4.0.8.505
	
	//An idle pump is either ready with no waiting transactions,
	//ready and reserved for PumpSrv (locked) , 
	//or simply a pump which has not been configured.

	//The pump is ready, which means that we cannot be dispensing
	//or requesting.
	if(ePumpStat == READY)
	{
		//If the pump is ready and reserved for PumpSrv,
		//it is considered locked.
		if(eResStat == PUMPSRV)
			bRetCode = TRUE;

		else
		{
			//The pump is not being used for anything,
			//but unpaid transactions may still exist.
			if ((eResStat == NULL_RES) && (ePumpResExt == NULL_RES_EXT))   //4.0.8.505
			{
				short nWaiting = 0 , nFree = 0 , nUnpaid = 0;
				_Module.m_server.m_cPumpArray[nPump].GetTrsListCount(&nWaiting,
																	 &nFree,
																	 &nUnpaid);

				//If there are no waiting pre pay transactions
				//and no unpaid transactions, the pump is idle.
				if(!nWaiting && !nUnpaid)
				   bRetCode = TRUE;
			}
		}
	}

	else
	{
		if((ePumpStat == UNKNOWN_STATUS) ||
		   (ePumpStat == COMMS_TERMINATED) || 
		   (ePumpStat == ERROR_TERMINATED) ||
		   (ePumpStat == OUT_OF_SERVICE))
			bRetCode = TRUE;
	}

	return bRetCode;
}

BOOL CCommonFunction::IsClubCardHashNumberPresent(const long lPumpNumber)
{
	PAY_AT_PUMP_INFO cInfo;
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cInfo);
	if (cInfo.CardSaleExtraData6.sClubCardHashNumber[0] != ' ' && cInfo.CardSaleExtraData6.sClubCardHashNumber[0] != 0)
		return TRUE;
	return FALSE;
}



/******************************************************************************
 Description:	Get wet stock  data   	
 Returns:      	Operation ret code.
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			9/11/2004   14:28		Start
******************************************************************************/

long CCommonFunction::GetWetStockData__(long lWetStockFlags, long *pVal)
{
	WetStockInternalRetCode retCode = WetStockInternal_OK;; 

	if(retCode == WetStockInternal_OK)
	{
		    
		if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() || 
			 _Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags()||
			 _Module.m_server.m_cServerInfo.GetMemoProcessFlags())				 
		{
			retCode = WetStockInternal_IN_MIDDLE_OF_PROCESS;
			_LOGMSG.LogMiddleOfProcessReason();//4.0.2.31
		}
	}

	if(retCode == WetStockInternal_OK)
	{
		if (lWetStockFlags)
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{	
				CString str("GetWetStockData Flags - ");
				if (lWetStockFlags & WET_STOCK_PUMP_TOTALS)
					str += "Pump Toatls, ";
				if (lWetStockFlags & WET_STOCK_TANKS)
					str += "Tank Reading, ";
				if (lWetStockFlags & WET_STOCK_DELIVERY)
					str += "Delivery Report, ";
				if (lWetStockFlags & WET_STOCK_ALARM)
					str += "Alarm, ";
				if (lWetStockFlags & WET_STOCK_PUMP_CFG)
					str += "Pump Configuration, ";
				if (lWetStockFlags & WET_STOCK_TANK_CFG)
					str += "Tank Configuration, ";
				if (lWetStockFlags & WET_STOCK_MOST_RECENT_DELIVERY)
					str += "recent delivery, ";
				if (lWetStockFlags & WET_STOCK_DO_MEMO_PROCESS)
					str += "memo process, ";


				str = str.Left(str.GetLength() - 2);
				_LOGMSG.LogMsg(str);
			}
			

			if(lWetStockFlags & WET_STOCK_DO_MEMO_PROCESS)   // 4.0.7.501
				_Module.m_server.GetWetStockData__(WSM_BY_DEMAND_MEMO , lWetStockFlags);			
			else
			{
				_Module.m_server.SetCommand(CMD_WET_STOCK_DATA,0,lWetStockFlags);

				// Save the requested flags, and by using WET_STOCK_ALL make sure that
				// no unused flag will be use. (otherwise, shift process of another "on demand"
				// process, will never take place).
				
				if(lWetStockFlags & WET_STOCK_MOST_RECENT_DELIVERY) //4.0.28.502 - TD 258931 
					_Module.m_server.m_cServerInfo.SetWetStockFlags(((BYTE)lWetStockFlags & WET_STOCK_ALL) | WET_STOCK_MOST_RECENT_DELIVERY); // 4.0.27.500 TD 109624 only from FCC
				else
					_Module.m_server.m_cServerInfo.SetWetStockFlags((BYTE)lWetStockFlags & WET_STOCK_ALL); // TD 109624

				
				
				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString strLog;
					strLog.Format("CCtrl::GetWetStockData: Set Server Info. Delivery Status: %d, Delivery Process: %d.", 
						_Module.m_server.m_cServerInfo.GetDeliveryEmulationStatus(), _Module.m_server.m_cServerInfo.GetDeliveryEmulationProcessType());
					_LOGMSG.LogMsg(strLog);
				}

			}

			
		}
	}
	
	return retCode;
}

/******************************************************************************
 Description:	This function returns the suitable return code, according to the Single Tank 
                Delivery Process's state, for lTankNumber.

 Returns:      	GPI_OK (no error) - when getting START during START, or END during END.
                GPI_DELIVERY_START_IN_MIDDLE_OF_END - when getting START during END.
                GPI_DELIVERY_END_IN_MIDDLE_OF_START - when getting END during START.

 Parameters:    Tank Number, Process Command.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		19/10/2008   12:41		Start 4.0.21.500 TD 66935
******************************************************************************/
long CCommonFunction::CheckDeliverySingleTankProcessState(const long lTankNumber, const long lProcessCommand)
{
    long lProcessType = _Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessType(lTankNumber);
	long lRtc = GPI_OK; //Check that this is the correct return code for all the other 
	//combinations values of lProcessCommand and lProcessType *?*?
	
	if ((lProcessCommand == PD_SINGLE_TANK_START) && (lProcessType == PD_SINGLE_TANK_END))
		lRtc = GPI_DELIVERY_START_IN_MIDDLE_OF_END;
		
	else
		if ((lProcessCommand == PD_SINGLE_TANK_END) && (lProcessType == PD_SINGLE_TANK_START))
			lRtc = GPI_DELIVERY_END_IN_MIDDLE_OF_START;
	
	return lRtc;
}

long CCommonFunction::CheckDeliveryProcessState(long &lParam1, long &lParam2 , long *pVal)
{	
	long lRtc = GPI_OK;

	if( (PD_START == _Module.m_server.m_cServerInfo.GetDeliveryEmulationProcessType()) && (lParam1 != PD_START) )
		lRtc = GPI_DELIVERY_END_IN_MIDDLE_OF_START;	
	else if ( (PD_END ==  _Module.m_server.m_cServerInfo.GetDeliveryEmulationProcessType()) && (lParam1 != PD_END) )
		lRtc = GPI_DELIVERY_START_IN_MIDDLE_OF_END;

	*pVal = lRtc;
	
	return lRtc;
}



/******************************************************************************
 Description:	Check if we can change the mode according to the flag	
				This function will call only if two flags are on 
 Parameters:   	long nPump
 Assumptions:   
 1.	On mode change (Single pump or all pumps) if 'ModeChangeNoOpenPrp' change mode only if relevant pump / pumps has no open PrP.
2.	On mode change (Single pump or all pumps) if 'ModeChangeOnIdle' change mode only if relevant pump / pumps are in IDLE status.
3.	In case set mode is declined (pre pay exist or pump not idle) PumpSrv will return new FTI error code to POSPump

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			21/06/2005   12:31		Start
 
******************************************************************************/
BOOL CCommonFunction::IsPumpIdleForModeChange(long nPump)
{
	BOOL bRetCode = FALSE;
	PumpStatus ePumpStat;
	PumpReservation eResStat;
	PumpResExt ePumpResExt;  //4.0.8.505
	CPumpTransact cTrs;

	ePumpStat = _Module.m_server.m_cPumpArray[nPump].GetStat().GetStatus();
	eResStat = _Module.m_server.m_cPumpArray[nPump].GetStat().GetRes();
	ePumpResExt = _Module.m_server.m_cPumpArray[nPump].GetStat().GetResExt(); //4.0.8.505
	
	//An idle pump is either ready with no waiting transactions,
	//ready and reserved for pumpsrv (locked) , 
	//or simply a pump which has not been configured.

	//The pump is ready, which means that we cannot be dispensing
	//or requesting.
	if(ePumpStat == READY)
	{
		//If the pump is ready and reserved for PumpSrv,
		//it is considered locked.
		if(eResStat == PUMPSRV)
			bRetCode = TRUE;

		else
		{
			//The pump is not being used for anything,
			//but unpaid transactions may still exist.
			if ((eResStat == NULL_RES) && (ePumpResExt == NULL_RES_EXT))   //4.0.8.505
			{
				short nWaiting = 0 , nFree = 0 , nUnpaid = 0;
				_Module.m_server.m_cPumpArray[nPump].GetTrsListCount(&nWaiting,
																	 &nFree,
																	 &nUnpaid);

				//If there are no waiting pre pay transactions
				//and no unpaid transactions, the pump is idle.
				if(!nWaiting && !nUnpaid)
				   bRetCode = TRUE;
				else
				{
					if(nWaiting>0)
						bRetCode = FALSE;
					else
						bRetCode = _Module.m_server.m_cPumpArray[nPump].CheckIfAllUnpaidArePAK();
					
				
				}
							
			}
		}
	}

	else
	{
		if((ePumpStat == UNKNOWN_STATUS) ||
		   (ePumpStat == COMMS_TERMINATED) || 
		   (ePumpStat == ERROR_TERMINATED) ||
		   (ePumpStat == OUT_OF_SERVICE))
			bRetCode = TRUE;
	}

	return bRetCode;
}





/******************************************************************************
 Description:	Generate row data information  of transaction  
 Returns:      	
 Parameters:  lPumpNumber -  if 0 then return  trans in all pumps 
			  *plRecCount -  get max number of trans that we can return
							 return the number of trans that we actully return
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			25/5/2004   9:58		Start
******************************************************************************/
GetPAKQueryRetCode CCommonFunction::GetTransListQuery(long lPumpNumber,BSTR *sData, long *plRecCount, long *lFlags)//4.0.9.506 //amram
{
	long lTrsCount = 0;
	long lNumTrs  = 0;
	long lVersion = 0;
	long lRtc = 0;
	GetPAKQueryRetCode eRetVal = GetPakQuery_OK ; 
	int iPump = 0 , iTrs = 0;
	long lLastValidPump = 0;
	CString sMsg;
	
	EXTRA_INFO_REC sExtraInfoRec;

	long lMaxOfTransToReturn = *plRecCount;
	long      lFlagIfForce =  0;//4.0.11.511
	PAID_TRS_EX cPaidTrsArr[MAX_PAID_TRS];

	long	  lNumOfPaidTrs=0;//4.0.11.511
	char	  sKey[20];
	long	  lGradeOla =0 ;
	
	PAID_TRS_EX  cPaidTrs;

	CTime cCurTime = CTime::GetCurrentTime();	//4.0.14.501
	CTime cTrsTime;
	CTimeSpan ts;

	long p = lPumpNumber-1;
	lLastValidPump = _Module.m_server.GetLastValidPump();//4.0.3.37

	CComBSTR bstrTransactions;
	CComBSTR bstrLastTrs;//hold for each loop the last trs
	bstrTransactions.Empty();
	bstrLastTrs.Empty();

	CPumpTransact cTmpTrs;

	TAG_PUMP_TRANSACT5 cTagPumpTrs;
	memset(&cTagPumpTrs , ' ' , sizeof(cTagPumpTrs));

	char sTmpStr[sizeof(TAG_PUMP_TRANSACT5) + 2];
	memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT5) , 0 , 2);


	if(LOG_BASIC_FLOW_CONTROL)
	{
		
		sMsg.Format("GetTransListQuery Insert PumpNumber = %ld Flags =  %ld RecCount = %ld",lPumpNumber,*lFlags,*plRecCount );
		_LOGMSG.LogMsg(sMsg);
	}

	lVersion = 2;
 
	if(eRetVal == GetPakQuery_OK)
	{
		if((*lFlags & PAK_QUERY_UNPAID_PAK) && (lPumpNumber > POS_NUMBER_PAID_QDX))
		{
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			//We may make the version depend on the lFlags parameter
			//in the future
			
			for(;iPump < lLastValidPump; iPump++)
			{				
				if(lPumpNumber)//if we have a valid pump
				{
					if(p == iPump)
					{
						lNumTrs = _Module.m_server.m_cPumpArray[iPump].GetTrsArraySize();
					}
					else
						continue;//move to next pump
				}
				else
					lNumTrs = _Module.m_server.m_cPumpArray[iPump].GetTrsArraySize();

				for(;iTrs < lNumTrs;iTrs++)
				{					
					_Module.m_server.m_cPumpArray[iPump].GetTrsByIndex(iTrs , &cTmpTrs);

					//check if flag on if so we will ignore the onOffer state
					//else we will not take trans that on offer AGIP
					if(*lFlags & TRS_IGNORE_OFFER)
						lFlagIfForce = FALSE;
					else
						lFlagIfForce = cTmpTrs.m_lOnOffer;
					if((cTmpTrs.m_lRes == PAY_AT_KIOSK) && 
					   (cTmpTrs.GetStatus() == UNPAID) && (!lFlagIfForce) )//4.0.3.46
					{
						//check the flags
						if(*lFlags & TRS_LOYALTY_CARD) 
						{
							//we have to check if  loyalty id exists
							//if so we will not return the trans.
							sExtraInfoRec.info.lIndexNumber = cTmpTrs.m_lNumber; 
							lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE,REC_READ, &sExtraInfoRec);
							//Verify if we have loyalty id  
							if(lRtc == OK)
							{
								if(!(ChrAll((char*)sExtraInfoRec.info.sLoyalId,sizeof(sExtraInfoRec.info.sLoyalId),' ')))
									continue;
							}						
						}

						cTmpTrs.GetTag(&cTagPumpTrs , lVersion);
						cTmpTrs.m_nGrade = (short)_Module.m_server.m_cDevicesPerGrade[cTmpTrs.m_nGrade-1].GetNACSCode();
						l2a((long)cTmpTrs.m_nGrade,cTagPumpTrs.sGrade,sizeof(cTagPumpTrs.sGrade));
						lTrsCount++;

						if(lTrsCount>lMaxOfTransToReturn)
						{
							if(LOG_BASIC_FLOW_CONTROL)	
							{
								sMsg.Format("GetTransListQuery Can't add new trans to buffer over limit MaxTrs=%ld , Current count =%ld ",lMaxOfTransToReturn,lTrsCount);
							    _LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);								
							}

							continue;
						}

						memcpy((BYTE*)sTmpStr , &cTagPumpTrs , sizeof(cTagPumpTrs));

						bstrTransactions.Append((LPCSTR)sTmpStr);
						memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT5) , 0 , 2);
					}
				}

				//Reset trs index
				iTrs = 0;
			}
			
			sMsg.Format("CCommonFunction::GetTransListQuery found unpaid %ld trs ",lTrsCount);
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);

			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}

		if(( *lFlags & PAID_TRS_FOR_PUMP )&&(lTrsCount<lMaxOfTransToReturn))//return all Paid Trs in Pump 
			//if we got from GCI/GPI than we got here
			//if we got from GPI than we have to add 100 to pumpNumber
			{
				CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

				long lRtc = 0;
				long lTmpPump = 0;
				memset(&cPaidTrs,' ',sizeof(cPaidTrs));
				memset(&cPaidTrsArr,' ',sizeof(cPaidTrsArr));				

				memset(&sKey,' ',sizeof(sKey));
 				_Module.m_server.SetPumpNumberToQDX(lPumpNumber,sKey);//4.0.11.510
				
				if(*lFlags & TRS_IGNORE_OFFER)
					_LOGMSG.LogMsg("CCommonFunction::GetTransListQuery TRS_IGNORE_OFFER is on");
				memcpy(cPaidTrsArr[0].sIndex,sKey,3);
				lRtc = _Module.m_server.IndexOperation(PAID_TRS_FILE,REC_READ_FIRST,&cPaidTrsArr[0]);

				if(lRtc)
				{
					if(LOG_BASIC_FLOW_CONTROL)	
					{
						sMsg.Format("GetTransListQuery Not found trs in QDX");
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
					}
				}
				else
				{				
					lTmpPump = a2l(cPaidTrsArr[lNumOfPaidTrs].sIndex,3);
					if(lTmpPump == lPumpNumber)//if we have at list one row
					{
						lNumOfPaidTrs++;
						while(!(lRtc))
						{
							if((cPaidTrs.bOnOffer == ' ')||(*lFlags & TRS_IGNORE_OFFER))
							{
							}
							else
							{
								CString str;
								str.Format("CCommonFunction::GetTransListQuery trs number %ld already in use by another user",
									a2l(cPaidTrs.cTagPump.sNumber,sizeof(cPaidTrs.cTagPump.sNumber)));
								_LOGMSG.LogMsg(str);
								
							}
							//copy the key
							memcpy(cPaidTrsArr[lNumOfPaidTrs].sIndex,cPaidTrsArr[lNumOfPaidTrs-1].sIndex,sizeof(cPaidTrsArr[lNumOfPaidTrs-1].sIndex));
							lRtc = _Module.m_server.IndexOperation(PAID_TRS_FILE,REC_READ_NEXT,&cPaidTrsArr[lNumOfPaidTrs]);
							if(lRtc)//4.0.11.515
								break;
							lTmpPump = a2l(cPaidTrsArr[lNumOfPaidTrs].sIndex,3);
							if(lTmpPump != lPumpNumber)//if we have at list one row
								break;
		 					lNumOfPaidTrs++;
						}
					}
				}

				if (slPumpArray.IsLocked())
					slPumpArray.Unlock();
			} 
		}
		
	lNumOfPaidTrs--;//4.0.11.511

	for(int i=lNumOfPaidTrs;i>=0;i--)
	{//4.0.11.511

		//Check if the transaction is still at the time before the timeout   		//4.0.14.501
		if (_Module.m_server.GetLoyaltyTransactionTimeout() && 
			a2l(cPaidTrsArr[i].cTagPump.sPump,sizeof(cPaidTrsArr[i].cTagPump.sPump)) > POS_NUMBER_PAID_QDX)
		{
			CTime cTrsTime(
				a2l(cPaidTrsArr[i].cTagPump.sDateTime.sYear,sizeof(cPaidTrsArr[i].cTagPump.sDateTime.sYear)),
				a2l(cPaidTrsArr[i].cTagPump.sDateTime.sMonth,sizeof(cPaidTrsArr[i].cTagPump.sDateTime.sMonth)), 
				a2l(cPaidTrsArr[i].cTagPump.sDateTime.sDay,sizeof(cPaidTrsArr[i].cTagPump.sDateTime.sDay)),
				a2l(cPaidTrsArr[i].cTagPump.sDateTime.sHour,sizeof(cPaidTrsArr[i].cTagPump.sDateTime.sHour)),
				a2l(cPaidTrsArr[i].cTagPump.sDateTime.sMinute,sizeof(cPaidTrsArr[i].cTagPump.sDateTime.sMinute)),
				a2l(cPaidTrsArr[i].cTagPump.sDateTime.sSecond,sizeof(cPaidTrsArr[i].cTagPump.sDateTime.sSecond)));

			ts = cCurTime - cTrsTime;								
			if((cCurTime > cTrsTime) && (ts.GetTotalMinutes() > _Module.m_server.GetLoyaltyTransactionTimeout()))
			{
				//Don't add the transaction to the list
				continue;
			}
		}

		lGradeOla = a2l(cPaidTrsArr[i].cTagPump.sGrade,sizeof(cPaidTrsArr[i].cTagPump.sGrade));
		lGradeOla = (short)_Module.m_server.m_cDevicesPerGrade[lGradeOla-1].GetNACSCode();
		l2a(lGradeOla,cPaidTrsArr[i].cTagPump.sGrade,sizeof(cPaidTrsArr[i].cTagPump.sGrade));
			

		l2a(cPaidTrsArr[i].lServiceMode  ,
			cPaidTrsArr[i].cTagPump.sServiceMode, 
			sizeof(cPaidTrsArr[i].cTagPump.sServiceMode)); //4.0.14.504

		memcpy((BYTE*)sTmpStr , &cPaidTrsArr[i].cTagPump , sizeof(cPaidTrsArr[i].cTagPump));
		bstrTransactions.Append((LPCSTR)sTmpStr);

		memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT5) , 0 , 2);

		lTrsCount++;//4.0.12.10
		sMsg.Format("CCommonFunction::GetTransListQuery insert trs number %ld to buffer",a2l(cPaidTrsArr[i].cTagPump.sNumber,sizeof(cPaidTrsArr[i].cTagPump.sNumber)));
		_LOGMSG.LogMsg(sMsg);
		
		if(lTrsCount>=lMaxOfTransToReturn)
		{
			if(LOG_BASIC_FLOW_CONTROL)	
			{
				sMsg.Format("GetTransListQuery Can't add new trans to buffer over limit Max count=%ld , Trs count=%ld", lMaxOfTransToReturn , lTrsCount);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
			}
			break;
		}

	}
	
	if(plRecCount)
		*plRecCount = lTrsCount;

	if((sData)&&(lTrsCount>0))
	{
		if(*lFlags & PAID_TRS_ONE_TRS)//only one trs
		{//4.0.12.10

			BOOL bAddTrs = TRUE;
			//Check if the transaction is still at the time before the timeout   
			if (_Module.m_server.GetLoyaltyTransactionTimeout() && 
				a2l(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sPump,sizeof(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sPump)) > POS_NUMBER_PAID_QDX)
			{
				CTime cTrsTime(
					a2l(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sYear,sizeof(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sYear)),
					a2l(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sMonth,sizeof(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sMonth)), 
					a2l(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sDay,sizeof(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sDay)),
					a2l(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sHour,sizeof(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sHour)),
					a2l(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sMinute,sizeof(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sMinute)),
					a2l(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sSecond,sizeof(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sDateTime.sSecond)));

				ts = cCurTime - cTrsTime;								
				if((cCurTime > cTrsTime) && (ts.GetTotalMinutes() > _Module.m_server.GetLoyaltyTransactionTimeout()))
				{
					//Don't add the transaction to the list
					BOOL bAddTrs = FALSE;
				}
			}

			bstrLastTrs.Empty();
			if (bAddTrs)
			{
				lGradeOla = a2l(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sGrade,sizeof(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sGrade));
				lGradeOla = (short)_Module.m_server.m_cDevicesPerGrade[lGradeOla-1].GetNACSCode();
				l2a(lGradeOla,cPaidTrsArr[lNumOfPaidTrs].cTagPump.sGrade,sizeof(cPaidTrsArr[lNumOfPaidTrs].cTagPump.sGrade));
				memcpy((BYTE*)sTmpStr , &cPaidTrsArr[lNumOfPaidTrs].cTagPump , sizeof(cPaidTrsArr[lNumOfPaidTrs].cTagPump));
				bstrLastTrs.Append((LPCSTR)sTmpStr);
				*sData = bstrLastTrs.Copy();
				sMsg.Format("CCommonFunction::GetTransListQuery return one trs PAID_TRS_ONE_TRS");
				_LOGMSG.LogMsg(sMsg);
			}

		}
		else
			*sData = bstrTransactions.Copy();
	}

	if(LOG_BASIC_FLOW_CONTROL)	
	{
		sMsg.Format("GetTransListQuery return %ld trs ",lTrsCount);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
	}

	return eRetVal;

}


/******************************************************************************
 Description:	Remove PrePay transaction from Database 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			19/7/2005   17:03		Start
******************************************************************************/

long CCommonFunction::RemovePrePaidTrs__(long & lPumpNumber, long & lSyncNumber)
{
	long lRtc = 0 , lServiceFeeRetCode = 0 , lServiceFeeValue = 0;
	long lTrsTotal = 0;

	CString str;

	PRE_PAY_INFO cTmpPrePayinfo;	
	CPumpTransact trs;

	trs.SetLink(TRS_LINK_PREPAY_NUMBER, lSyncNumber);
	
	lRtc = _Module.m_server.TrsLinkOperation(REC_READ,&trs,&cTmpPrePayinfo);

	if (!PUMP_NOT_VALID(lPumpNumber))  
	{
		//If the service fee was not charged at the POS, we have to add it here
		//in order to see if the trs was really unbalanced or if just the service
		//fee is missing.
		if(!(cTmpPrePayinfo.lFlags & SERVICE_FEE_CHARGED_BY_POS))  //4.0.9.507
		{
			_Module.m_server.GetServiceFeeInfo(lPumpNumber,
											   SERVICE_TYPE_PREPAY,
											   0,
											   NULL,
											   GET_FEE_BY_TRANSACTION_TYPE,
											   &lServiceFeeValue,
											   &lServiceFeeRetCode);

			if(LOG_BASIC_FLOW_CONTROL)
			{
				str.Format("CGPI::RemovePrePaidTrs - adding service fee %d to trs #%d with value %d", lServiceFeeValue, trs.m_lNumber, trs.m_lRoundedValue);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
			}
		}
	}
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			str.Format("CGPI::RemovePrePaidTrs  with no valid pump=%ld", lPumpNumber);
			_LOGMSG.LogMsg(str); 				
		}

	}
		
	lTrsTotal = trs.m_lRoundedValue + lServiceFeeValue;


	if ( (trs.GetLinkFlags() & TRS_LINK_EXTRA_INFO) && (cTmpPrePayinfo.lFlags & PREPAY_WITH_XML_BUFF) )
	{
		EXTRA_INFO_REC ExtraInfoRec;
		memset(&ExtraInfoRec , 0, sizeof(EXTRA_INFO_REC));

		ExtraInfoRec.info.lIndexNumber = trs.m_lNumber;
		lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);

		if (!lRtc)
		{
			//Delete record.
			lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_DELETE, &ExtraInfoRec);

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Remove extra info record. found extra info data for trs number %ld, record removed status %ld  ",trs.m_lNumber, lRtc);
			}
		}
			
		if(lRtc == ERR_DELETED) lRtc = OK; //4.0.29.10 - TD 271775
	}
		
	if ( ( (!lRtc) && trs.m_lPump ) && 
			( (_Module.m_server.GetFinishedPrepayIndication() == TRUE) || (cTmpPrePayinfo.lMaxValue != lTrsTotal)) )	//56040 4.0.19.508	//4.0.24.60 TD 112315
	{
		_Module.m_server.m_cPumpArray[trs.m_lPump - 1].DecreaseUnBalancePrePayCounter();
	}
	

	if(trs.m_lMisc & MISC_DECLINE_PRP)	//4.0.12.502
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("RemovePrePaidTrs__ found MISC_DECLINE_PAP need to Decreas number of decline PRP trs number %ld ",trs.m_lNumber);
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].DecreaseDeclinePrePayCounter();

		}
	}

	lRtc = _Module.m_server.TrsLinkOperation(REC_DELETE,&trs,NULL);

	if (_Module.m_server.m_cFuelLoayaltyPS.GetFuelLoyaltyFlag() && _Module.m_server.GetIFSFTerminalSrvInUse()) //4.0.19.710 66705 //4.0.23.504
	{
		//IFSF EPS - Clean the zones from the extra data qdx
		PS::RECEIPT_EXTRA_INFO_REC  cReceiptExtraRec;
		memset(&cReceiptExtraRec, ' ', sizeof(PS::RECEIPT_EXTRA_INFO_REC));
		cReceiptExtraRec.info.lIndexNumber = trs.m_lLinkNumber;

		long lQdxRetCode = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_DELETE, cReceiptExtraRec);
		//long lQdxRetCode = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_DELETE,&cReceiptExtraRec);

		CString sMsg; //4.0.19.810 66705

		if (lQdxRetCode)
		{
			sMsg.Format("RemovePrePaidTrs__, failed to clear Index=%ld from ReceipteExtraData.qdx , error=%ld", cReceiptExtraRec.info.lIndexNumber, lQdxRetCode);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg, LOG_LEVEL_1); //4.0.19.810 66705
		}
		else
		{
			sMsg.Format("RemovePrePaidTrs__ successfully  cleared Index=%ld from ReceipteExtraData.qdx", cReceiptExtraRec.info.lIndexNumber);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg); //4.0.19.810 66705
		}
	}
	return lRtc;
}


void CCommonFunction::GetTotalPumpsInfo(long lPumpNumber,long * lCount,BSTR * pRec)
{
	int i=0,NumOfTotals=0,lFlags=0;
	TAG_MEMO_TOTAL_PUMPS_INFO tagTotalInfo;
	CString str;

	CComBSTR bstrTransactions;
	char sTmpStr[sizeof(TAG_MEMO_TOTAL_PUMPS_INFO) + 2];
	memset(&tagTotalInfo,' ',sizeof(TAG_MEMO_TOTAL_PUMPS_INFO));
	memset((BYTE*)sTmpStr + sizeof(TAG_MEMO_TOTAL_PUMPS_INFO) , 0 , 2);

	//get the data only for one pump
	if(lPumpNumber>0)
	{
		if(_Module.m_server.m_cTotalPumpInfo.GetTotalData(lPumpNumber,lFlags,tagTotalInfo))
		{
			memcpy((BYTE*)sTmpStr , &tagTotalInfo , sizeof(tagTotalInfo));
			bstrTransactions.Append((LPCSTR)sTmpStr);
			memset((BYTE*)sTmpStr + sizeof(TAG_MEMO_TOTAL_PUMPS_INFO) , 0 , 2);
			NumOfTotals++;
		
		}
	}
	else
	{
		for(i=1;i<=MAX_PUMPS_64;i++) //4.0.15.502
		{
			
			if(_Module.m_server.m_cTotalPumpInfo.GetTotalData(i,lFlags,tagTotalInfo))
			{
				memcpy((BYTE*)sTmpStr , &tagTotalInfo , sizeof(tagTotalInfo));
				bstrTransactions.Append((LPCSTR)sTmpStr);
				memset((BYTE*)sTmpStr + sizeof(TAG_MEMO_TOTAL_PUMPS_INFO) , 0 , 2);
				NumOfTotals++;
			}
		}
	}
	if(LOG_BASIC_FLOW_CONTROL)
	{
		str.Format("GetTotalPumpsInfo return %ld of pump total",NumOfTotals);;
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
	}
	*pRec = bstrTransactions.Copy();
	bstrTransactions.Empty(); //4.0.18.501 merge from 4.0.15.285
	*lCount=NumOfTotals ;

}

//4.0.12.512
void CCommonFunction::GetReceipt2(long lPosNumber, BSTR sTrack2, long lIndex, BSTR *sReceiptData, long lFlags,long lFromPosNumber, long *pVal)
{
	GPIRetCode	retCode = GPI_OK;
	long		lRtc = 0; 
	short		lPosIndex = 0; 
	DWORD		dwLastErr;
	int			iLen = 0;
	long		lPumpNumber = 0;	
	long		lWaitTimeout = ((_Module.m_server.GetIFSFTerminalSrvInUse()) ? MAX_TERMINAL_SRV_WAIT : MAX_DEFAULT_WAIT_TIMEOUT); //4.0.29.500 - TD 275511
	

	if (lPosNumber == MOBILE_SRV_POS_NUMBER)
		lWaitTimeout = 20000;


	BYTE sInvoiceNumber[10];
	memset(sInvoiceNumber, ' ', sizeof(sInvoiceNumber));

	char szTrack2[ENCRYPTED_DATA_SIZE]; 
	memset(szTrack2, 0, sizeof(szTrack2));	

	TAG_OLA_REC13	tagOlaRec;   //4.0.27.46

	if ((lPosNumber == MOBILE_SRV_POS_NUMBER) || (!_Module.m_server.GetIFSFTerminalSrvInUse())) //4.0.22.250 - TD 100854
		iLen = _Module.m_server.m_cBaseReceipt->GetReceiptIndex(szTrack2, sizeof(szTrack2), sTrack2, lFlags, 0, lIndex);	//4.0.20.502 (2)
	else 
		iLen = 1;

	if(iLen == 0)
	{
		dwLastErr = GetLastError();
		retCode = GPI_NO_RECEIPT;
	}
	else
	{	
		Track2Format((unsigned char *) szTrack2); //TD 142653 4.0.25.501

		BOOL lFound = RECEIPT_NOT_FOUND;

		CMD_STRUCT_FORMAT_RECEIPT cmdInfo;
		memset(&cmdInfo.sTrack2, ' ', sizeof(cmdInfo.sTrack2)); //4.0.20.53
		

		if ((lPosNumber == MOBILE_SRV_POS_NUMBER) || (!_Module.m_server.GetIFSFTerminalSrvInUse())) //4.0.19.504 - 47559
		{
			if (RECEIPT_CAR_REGISTRATION_INDEX  & lFlags)  // 4.0.7.500
			{
				RECEIPT_EXTRA_INDEX_INFO rec;
				memset(&rec , 0,sizeof(RECEIPT_EXTRA_INDEX_INFO));
				memset(&rec.sKeyIndex , ' ', sizeof(rec.sKeyIndex));

				iLen = min(strlen(szTrack2),sizeof(szTrack2));	//4.0.19.502

				memcpy(rec.sKeyIndex ,szTrack2 , min(iLen, sizeof(rec.sKeyIndex)));
				
				lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_INDEX_FILE, REC_READ, &rec);
				if (!lRtc)
				{
					CReceiptInfo	    receiptTmp; 
					PAY_AT_PUMP_INFO PayAtPumpInfo;
					receiptTmp.m_TrsIndex = rec.myIndex.byIndex;
					receiptTmp.m_HashedKey.SetString((LPCSTR)rec.myIndex.sTrack2, _countof(rec.myIndex.sTrack2));
		

					CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
					if (OK == CSQLReceiptDB::GetInstance().LoadReceipt(receiptTmp.m_TrsIndex, receiptTmp))
					{
						CPumpTransact trs;

						_Module.m_server.ConvertReceiptToOla(receiptTmp,&trs,&PayAtPumpInfo);	
						memset(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber ,' ' , sizeof(tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber));
						tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData	 = PayAtPumpInfo.CardSaleInfo.cardData;
						tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData  = PayAtPumpInfo.CardSaleInfo.extraData;
						tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2 = PayAtPumpInfo.CardSaleExtraData2;
						trs.GetTag(&tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs, 2);
						tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData3 = PayAtPumpInfo.CardSaleExtraData3;
						tagOlaRec.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData4 = PayAtPumpInfo.CardSaleExtraData4;							//4.0.22.61	TD 75605
						tagOlaRec.tagOlaRec12.extraData5             = PayAtPumpInfo.CardSaleExtraData5;
						tagOlaRec.extraData6                         = PayAtPumpInfo.CardSaleExtraData6;

						cmdInfo.lIndex = rec.myIndex.byIndex;
						memcpy(cmdInfo.sTrack2 , rec.myIndex.sTrack2,sizeof(cmdInfo.sTrack2));
						cmdInfo.lFlags = lFlags;
						cmdInfo.lPosNumber = lFromPosNumber; //4.0.12.512
						lPosNumber = lFromPosNumber; //4.0.12.512
						lFound  = RECEIPT_FOUND;
					}
				}
			}
			else			// 4.0.7.500
			{
				CStringA cTrack2(szTrack2);
				lFound = _Module.m_server.BuildRequestReceiptInfo(cmdInfo, cTrack2, lIndex, lFlags, lPosNumber, &tagOlaRec);
			}
		}
		else  //4.0.19.504 - 47559
		{
			if (lPosNumber)
			{
				if (lIndex > 0)
				{
					sprintf((char *)sInvoiceNumber, "%05ld", lIndex);
					lFound  = RECEIPT_FOUND;
				}
			}
			else
			{
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;  //4.0.5.12
					str.Format("Request Pump %ld not valid ",lPosNumber);;
					_LOGMSG.LogMsg(lPosNumber,LOG_PUMP,str);
				}
			}
		}

		if (lFound == RECEIPT_FOUND)
		{
			if (_Module.m_server.GetIFSFTerminalSrvInUse() && ((lPosNumber != MOBILE_SRV_POS_NUMBER)))   //4.0.19.504 - 47559
			{
				//Get the receipt from the TerminalSrv
				_Module.m_server.m_cGPIEventMgr.AddRequest4FormatReceiptData(lPumpNumber , lFromPosNumber , sInvoiceNumber); //lInvoiceNumber
				lPosNumber = lFromPosNumber; //4.0.29.500 - TD 275511
			}
			else
			{			
				if(LOG_DETAIL_FLOW_CONTROL)
				{						
					char szResult[MAX_FIELD_VAL]={0};
					
					_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, (char*)szTrack2, iLen); //4.0.20.501 TD-60236
					
					CString	str;  
					if (RECEIPT_CAR_REGISTRATION_INDEX  & lFlags)  // 4.0.7.500
					{
						str.Format("for car registration=[%s] lPos=%ld, lIndex=%d, retCode=%d", szResult, lPosNumber, lIndex, retCode); 
					}
					else
					{
						str.Format("Send Receipt to be format for lPos=%ld, szTrack2=[%s], lIndex=%d retCode=%d", lPosNumber, szResult, lIndex, retCode); 
					}

					_LOGMSG.LogMsg(lFromPosNumber,LOG_POS,str);
				}							
				_Module.m_server.SetCommand(CMD_FORMAT_RECEIPT, SERVER_POS_NUMBER ,lPosNumber ,&cmdInfo,sizeof(CMD_STRUCT_FORMAT_RECEIPT));						
			}

			//Get handle Pos receipt 
			CSingleLock slPosMap( &(_Module.m_server.m_csPosMap));	
			slPosMap.Lock(INFINITE);
			CPosInfo tmpPos;

			_Module.m_server.m_cPosMap.Lookup(lPosNumber,tmpPos);
			slPosMap.Unlock();

			// if we get the event in 1 second we get receipt
			if (tmpPos.WaitForSingleObjectReceipt(lWaitTimeout)) //4.0.29.500 - TD 275511
			{
				CComBSTR strResult;
				//Get handle Pos receipt 
				CSingleLock slPosMap( &(_Module.m_server.m_csPosMap));	
				slPosMap.Lock(INFINITE);
				CPosInfo tmpPos;

				////////////////////////////////////////////////////////////////////////////////////////
				if(LOG_BASIC_FLOW_CONTROL)
				{					
					char szResult[MAX_FIELD_VAL] = {0};
					
					_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, (char*)szTrack2, iLen); //4.0.20.501 TD-60236
					
					CString	str;
					if (RECEIPT_CAR_REGISTRATION_INDEX  & lFlags)  // 4.0.7.500
					{
						str.Format("Received formated receipt for registration lPos=%ld, lIndex=%d, retCode=%d", lPosNumber  ,lIndex ,  retCode); 						str.Format("Received formated receipt for registration lPos %ld, lIndex %d, retCode %d", lPosNumber, lIndex, retCode);
					}
					else
					{
						str.Format("Received Format receipt lPos=%ld, Index=%ld, retCode=%d,", lPosNumber, lIndex, retCode); 						
					}

					_LOGMSG.LogMsg(lFromPosNumber,LOG_POS,str);
				}			
				/////////////////////////////////////////////////////////////////////////////////////////

				_Module.m_server.m_cPosMap.Lookup(lPosNumber,tmpPos);

				strResult = tmpPos.GetReceiptText();
				*sReceiptData = strResult.Copy();
				
				slPosMap.Unlock();
			}
			else
				retCode = GPI_NO_RECEIPT;			
		}
		else 
		{
			if(lFound == TOO_MANY_RECEIPTS)
				retCode = GPI_CANNOT_PRINT_RECEIPT;
			else
				retCode = GPI_NO_RECEIPT;
		}
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString	str; 
		if (iLen)
		{
			char szResult[MAX_FIELD_VAL]={0};
					
			_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, (char*)szTrack2, iLen); //4.0.20.501 TD-60236
			 
			if (RECEIPT_CAR_REGISTRATION_INDEX  & lFlags)  // 4.0.7.500
			{
				str.Format("by Car registration Data=[%s], lIndex=%ld, retCode=%d, lFlags=%ld", szResult, lIndex, retCode, lFlags);
			}
			else
			{
				str.Format("Data=[%s], lIndex=%ld, retCode=%d, lFlags=%ld", szResult, lIndex, retCode, lFlags);
			}
		}
		else
			str.Format("GetReceipt2, lIndex=%ld, retCode=%d, ilen=%d, lFlags=%ld", lIndex, retCode, iLen, lFlags);

		_LOGMSG.LogMsg(lFromPosNumber,LOG_POS,str);
	}			

	if(pVal != NULL)
		*pVal = retCode;
}


//4.0.16.500 - TD 38127
/******************************************************************************
 Description:	Get shift process status
 Returns:      	None
 Parameters:   	long *lShiftNumber - shift number
				long *lShiftStatus - shift process status (in middle of process or not)				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			2/01/2007   12:00		Start
******************************************************************************/
void CCommonFunction::GetShiftProcessStatus(long *lShiftNumber, long *lShiftStatus)
{
	if (lShiftNumber != NULL)
		*lShiftNumber = _Module.m_server.GetActiveShiftNumber();
	
	if(lShiftStatus != NULL)
		*lShiftStatus = _Module.m_server.IsShiftChangeInProgress();
}


//4.0.16.500
/******************************************************************************
 Description:	Set wash programs data
 Returns:      	None
 Parameters:   	CXMLInterface *pXmlDoc - pointer to XML document to parse
				CXMLPointers *pXmlRecord - pointer to specific record in the XML document
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			18/01/2007   12:00		Start
******************************************************************************/
CtrlRetCode CCommonFunction::SetWashProgramsData(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord)
{
	CtrlRetCode		retCode = CTRL_OK;
	long			lRtc = 0;
	CXMLPointers	cProgramsElement, cProgramsList, cProgramElement;
	CXMLPointers	cProgramAttributeList, cProgramAttribute;
	char szProgramId [MAX_ID_LEN] = {0};
	long lProgramId = 0;

	memset(szProgramId, 0, sizeof(szProgramId));
	if (!pXmlDoc || !pXmlRecord)
	{
		retCode = CTRL_RECORD_NOT_FOUND;
	}
	else
	{
		if (pXmlDoc->SelectSingleElementNode(SECTION_CARWASH_PROGRAMS, &cProgramsElement,pXmlRecord))
		{
			pXmlDoc->GetChildNodesList(SECTION_SINGLE_CARWASH_PROGRAM, &cProgramsElement, &cProgramsList);

			// Make all car wash programs not valid
			for (lProgramId = 1; lProgramId <= MAX_CAR_WASH_PROGRAM; lProgramId++)
				lRtc = _PARAM.SetParam("WashPrograms", lProgramId, 0, "ValidEntry", FIELD_DWORD, NULL, FALSE, FALSE); 

			// Go through all car wash programs			
			while(pXmlDoc->GetNextItem(cProgramsList, cProgramElement))
			{
				pXmlDoc->GetAttributeToByteArrWithConvert(ATT_CW_PROGRAM_ID, szProgramId, &cProgramElement);
				
				// Check if program id is valid
				lProgramId = a2l((unsigned char *)szProgramId, sizeof(szProgramId));
				if (lProgramId <= 0 || lProgramId > MAX_CAR_WASH_PROGRAM)
				{
					retCode = CTRL_INVALID_CAR_WASH_NUMBER;
					break;
				}
				else
				{
					pXmlDoc->GetChildNodesList(&cProgramElement, &cProgramAttributeList);
					while(pXmlDoc->GetNextItem(cProgramAttributeList, cProgramAttribute))
					{
						// Get car wash program's attributes
						char szProgramAttrName [MAX_NAME_LEN] = {0};
						char szProgramAttrVal [MAX_NAME_LEN] = {0};
						
						pXmlDoc->GetName(szProgramAttrName, &cProgramAttribute);
						if (!strcmp(szProgramAttrName, ATT_CW_PROGRAM_NAME))
						{
							pXmlDoc->GetChildNodeValueToByteArrWithConvert(&cProgramAttribute, szProgramAttrVal);
							lRtc = _PARAM.SetParam("WashPrograms", lProgramId, 0, "FullName",FIELD_SZ,szProgramAttrVal,0,FALSE); 
						}
						else if (!strcmp(szProgramAttrName, ATT_CW_PROGRAM_PRICE))
						{
							long lPrice = 0;						
							pXmlDoc->GetChildNodeValueToByteArrWithConvert(&cProgramAttribute, szProgramAttrVal);
							lPrice = a2l((unsigned char *)szProgramAttrVal, sizeof(szProgramAttrVal));
							lRtc = _PARAM.SetParam("WashPrograms", lProgramId, 0, "Price",FIELD_DWORD, NULL, lPrice, FALSE); 
						}
						else if (!strcmp(szProgramAttrName, ATT_CW_PROGRAM_PRODUCT_CODE))
						{
							pXmlDoc->GetChildNodeValueToByteArrWithConvert(&cProgramAttribute, szProgramAttrVal);
							lRtc = _PARAM.SetParam("WashPrograms", lProgramId, 0, "CCH_Product_Code", FIELD_SZ, szProgramAttrVal, 0, FALSE); 
						}
					}
					lRtc = _PARAM.SetParam("WashPrograms", lProgramId, 0, "ValidEntry", FIELD_DWORD, NULL, TRUE, FALSE); 
				}
			
			}
		}
		else
			retCode = CTRL_RECORD_NOT_FOUND;
	}

	return retCode;
	
}

/******************************************************************************
 Description:	Set car wash information
 Returns:      	None
 Parameters:   	CXMLInterface *pXmlDoc - pointer to XML document to parse
				CXMLPointers *pXmlRecord - pointer to specific record in the XML document
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			22/01/2007   12:00		Start
******************************************************************************/
CtrlRetCode CCommonFunction::SetCarWashInfo(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord)
{
	CtrlRetCode		retCode = CTRL_OK;
	long lRetCode = 0;

	// Set wash programs section
	retCode = SetWashProgramsData(pXmlDoc, pXmlRecord);

	// Set car wash general section
	if (!retCode) 
		retCode = SetCarWashGeneralData(pXmlDoc, pXmlRecord);
		
	// Set car wash device section
	if (!retCode) 
		retCode = SetCarWashDeviceData(pXmlDoc, pXmlRecord);


	if (retCode) 
	{
		_LOGMSG.LogMsg("Failed to set car wash info data!");
	}
	else
	{
		//Load the requests and the cancels lists
		_Module.m_server.m_cCarWash.InitCarWashLists();

		Reconfigure2(CARWASH_TABLE, 0, 0, 0, &lRetCode, FALSE);
		if (lRetCode)
		{
			_LOGMSG.LogMsg("Failed to send Reconfigure event");
		}
	}
	
	return retCode;
}

/******************************************************************************
 Description:	Set car wash general data section
 Returns:      	None
 Parameters:   	CXMLInterface *pXmlDoc - pointer to XML document to parse
				CXMLPointers *pXmlRecord - pointer to specific record in the XML document
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			22/01/2007   12:00		Start
******************************************************************************/
CtrlRetCode CCommonFunction::SetCarWashGeneralData(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord)
{
	CtrlRetCode		retCode = CTRL_OK;

	return retCode;
}

/******************************************************************************
 Description:	Set car wash device data section
 Returns:      	None
 Parameters:   	CXMLInterface *pXmlDoc - pointer to XML document to parse
				CXMLPointers *pXmlRecord - pointer to specific record in the XML document
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			22/01/2007   12:00		Start
******************************************************************************/
CtrlRetCode CCommonFunction::SetCarWashDeviceData(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord)
{
	CtrlRetCode		retCode = CTRL_OK;

	return retCode;
}

/******************************************************************************
 Description:	Reset Tanks' document reference numbers (memory + INI file) => set them to zero
 Returns:      	None
 Parameters:   	None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			17/08/2008   18:14		Merge //4.0.20.501 - TD 65174
******************************************************************************/
void CCommonFunction::ResetTanksReference()
{
	if (_Module.m_server.GetSupportDeliveryEmulation() == TRUE)
	  for(long i = 0; i < MAX_TANKS; i++)
	    _Module.m_server.m_cTankArray[i].ResetDeliveryReference();

}

/******************************************************************************
 Description:	Set Tanks Delivery Reference Numbers from an XML given from BOS
                to the INI file.
 Returns:      	CTRL return codes
 Parameters:   	CXMLInterface *pXmlDoc - pointer to XML document to parse
				CXMLPointers *pXmlRecord - pointer to specific record in the XML document
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			12/08/2008   16:41		Merge //4.0.20.501 - TD 65174
******************************************************************************/
CtrlRetCode CCommonFunction::SetTanksData(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord)
{
	CtrlRetCode  retCode = CTRL_OK;
	CXMLPointers cTankElement, cTanksList, cTankAttributeList, cTankAttribute;
	long lTankNumber, lTankReference = 0;

	if (!pXmlDoc || !pXmlRecord)
	{
		retCode = CTRL_RECORD_NOT_FOUND;
	}
	else
	{
		char sName [MAX_NAME_LEN] = {0};
		(*pXmlDoc).GetName(sName, pXmlRecord);
		if (!strcmp(sName, ELEMENT_TANKS))
		{
			//Get all tanks records:
			pXmlDoc->GetChildNodesList(pXmlRecord, &cTanksList);

			// Go through all tanks:			
			while(pXmlDoc->GetNextItem(cTanksList, cTankElement))
			{
				
				//Get each tank's attributes:
				pXmlDoc->GetChildNodesList(&cTankElement, &cTankAttributeList);

				//Go through all attributes:
				while(pXmlDoc->GetNextItem(cTankAttributeList, cTankAttribute))
				{
					char szTankAttrName[MAX_NAME_LEN] = {0};
					char szTankAttrVal[MAX_NAME_LEN] = {0};
						
					pXmlDoc->GetName(szTankAttrName, &cTankAttribute);
					if (!strcmp(szTankAttrName, ATT_TANK_NUMBER))
					{
						pXmlDoc->GetChildNodeValueToByteArrWithConvert(&cTankAttribute, szTankAttrVal);
						lTankNumber = a2l((unsigned char *)szTankAttrVal, sizeof(szTankAttrVal));
					}
					else if (!strcmp(szTankAttrName, ATT_TANK_REFERENCE))
					{						
						pXmlDoc->GetChildNodeValueToByteArrWithConvert(&cTankAttribute, szTankAttrVal);
						lTankReference = a2l((unsigned char *)szTankAttrVal, sizeof(szTankAttrVal));

       					if (lTankNumber >= 1 && lTankNumber <= _Module.m_server.GetLastValidTank())
						{
						   ////Save DeliveryReference both to memory and ini file (for recovery):
                           _Module.m_server.m_cTankArray[lTankNumber-1].SetDeliveryReference(lTankReference);
				           _Module.m_server.m_cTankArray[lTankNumber-1].SaveDeliveryReference();
						}
						else
						{
                           CString sLogMsg("CCommonFunction, SetTanksData: Invalid Tank Number");
					       _LOGMSG.LogMsg(sLogMsg, LOG_LEVEL_1);
						}
					}
				}
			}
		}
		else
			retCode = CTRL_RECORD_NOT_FOUND;
	}
	
	return retCode;
}
		
/******************************************************************************
 Description:	Parse Tanks Delivery Reference Numbers from an XML given from BOS,
                and update information in the INI file. 
 Returns:      	CTRL return codes
 Parameters:   	BSTR sXmlRec - string in an XML format, which contains data on Tanks Delivery Reference Numbers
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			12/08/2008   15:49		Merge //4.0.20.501 - TD 65174
******************************************************************************/
CtrlRetCode CCommonFunction::ParseTanksDeliveryReferenceNumbers(BSTR sXmlRec)
{
	CtrlRetCode retCode = CTRL_OK;
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
		CXMLInterface  xmlDoc;
		CXMLPointers   cAllRecordsList, cRecordElement;

		try
		{
			if (xmlDoc.InitDoc())
			{
				bRetCode = xmlDoc.LoadXmlString(sXmlBuffer);

				if (bRetCode)
				{
					// Get all records under the root (should be only one: "Tanks")
					xmlDoc.GetChildNodesList(NULL,&cAllRecordsList);
					
					// Go to the "Tanks" element:
					xmlDoc.GetNextItem(cAllRecordsList, cRecordElement);
					retCode = _Module.m_server.m_cCommonFunction.SetTanksData(&xmlDoc, &cRecordElement);

				   // Check return code:
				   if (retCode)
				   {						
				    	CString sLogMsg("CCommonFunction: ParseTanksDeliveryReferenceNumbers failed to set info");
					   _LOGMSG.LogMsg(sLogMsg, LOG_LEVEL_1);

				   }
				}
			}

		}
		catch(...)
		{
			_LOGMSG.LogMsg("CCommonFunction","ParseTanksDeliveryReferenceNumbers", 0, LOG_NONE, "Unexpected Error occurred");
		}
		xmlDoc.CloseDoc();
	}

	return retCode;
}

/******************************************************************************
 Description:	Get Tank's Delivery data from an XML given from BOS
 Returns:      	CTRL return codes
 Parameters:   	CXMLInterface *pXmlDoc   - pointer to XML document to parse
				CXMLPointers *pXmlRecord - pointer to specific record in the XML document
				lTankNumber, lTankReference, bLockConnectedPumps - the returned data
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			08/10/2008   10:20		Start //4.0.21.500 - TD 66935
******************************************************************************/
CtrlRetCode CCommonFunction::GetTankData(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord, long &lTankNumber, long &lTankReference, BOOL &bLockConnectedPumps)
{
	CtrlRetCode  retCode = CTRL_RECORD_NOT_FOUND;
	CXMLPointers cTankElement, cTanksList, cTankAttributeList, cTankAttribute;
	long lLockConnectedPumps = 0;
	bool bFoundTankNumber = FALSE, bFoundTankReference = FALSE, bFoundLockConnectedPumps = FALSE; 

	if (pXmlDoc && pXmlRecord)
	{
		char sName [MAX_NAME_LEN] = {0};
		(*pXmlDoc).GetName(sName, pXmlRecord);
		if (!strcmp(sName, ELEMENT_TANKS))
		{
			//Get all tanks records (should be only one. Others will be ignored):
			pXmlDoc->GetChildNodesList(pXmlRecord, &cTanksList);

			// Go through all tanks:			
			if (pXmlDoc->GetNextItem(cTanksList, cTankElement))
			{
				
				//Get the tank's attributes:
				pXmlDoc->GetChildNodesList(&cTankElement, &cTankAttributeList);

				//Go through all attributes (should be three):
				while(pXmlDoc->GetNextItem(cTankAttributeList, cTankAttribute))
				{
					char szTankAttrName[MAX_NAME_LEN] = {0};
					char szTankAttrVal[MAX_NAME_LEN] = {0};
						
					pXmlDoc->GetName(szTankAttrName, &cTankAttribute);
					if (!strcmp(szTankAttrName, ATT_TANK_NUMBER))
					{
						pXmlDoc->GetChildNodeValueToByteArrWithConvert(&cTankAttribute, szTankAttrVal);
						lTankNumber= a2l((unsigned char *)szTankAttrVal, sizeof(szTankAttrVal));
						if ((lTankNumber >= 1) && (lTankNumber <= _Module.m_server.GetLastValidTank()))
							bFoundTankNumber = TRUE;
						else
						{
                           CString sLogMsg("CCommonFunction::GetTankData: Invalid Tank Number");
					       _LOGMSG.LogMsg(sLogMsg, LOG_LEVEL_1);
						}
					}
					else if (!strcmp(szTankAttrName, ATT_TANK_REFERENCE))
					{						
						pXmlDoc->GetChildNodeValueToByteArrWithConvert(&cTankAttribute, szTankAttrVal);
						lTankReference = a2l((unsigned char *)szTankAttrVal, sizeof(szTankAttrVal));
						if (lTankReference != 0)
							bFoundTankReference = TRUE;
						else
						{
                           CString sLogMsg("CCommonFunction::GetTankData: Invalid Reference Number");
					       _LOGMSG.LogMsg(sLogMsg, LOG_LEVEL_1);
						}
					}
					else if (!strcmp(szTankAttrName, ATT_TANK_LOCK_PUMPS))
					{						
						pXmlDoc->GetChildNodeValueToByteArrWithConvert(&cTankAttribute, szTankAttrVal);
						lLockConnectedPumps = a2l((unsigned char *)szTankAttrVal, sizeof(szTankAttrVal));
						bFoundLockConnectedPumps = TRUE;
						if (lLockConnectedPumps == 1)
							bLockConnectedPumps = TRUE;
						else
							bLockConnectedPumps = FALSE;
					}
				}
			}
		}
	}
	if (bFoundTankNumber && bFoundTankReference && bFoundLockConnectedPumps)
		retCode = CTRL_OK;
	else
	{
		CString sLogMsg("CCommonFunction::GetTankData failed to get info. Not all record exist or valid.");
		_LOGMSG.LogMsg(sLogMsg, LOG_LEVEL_1);
		sLogMsg.Format("bFoundTankNumber = %s , bFoundTankReference = %s , bFoundLockConnectedPumps = %s",bFoundTankNumber ? "TRUE":"FALSE",bFoundTankReference ? "TRUE":"FALSE",bFoundLockConnectedPumps ? "TRUE":"FALSE");
		_LOGMSG.LogMsg(sLogMsg, LOG_LEVEL_1); // TD 258646	4.0.25.302
	}
	return retCode;
}

/******************************************************************************
 Description:	Parse Tank Number, Delivery Reference Number and LockConnectedPumpds, from an XML given from BOS,
 Returns:      	CTRL return codes
 Parameters:   	BSTR sXmlRec - string in an XML format, which contains data.
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			08/10/2008   10:20		Start //4.0.21.500 - TD 66935
******************************************************************************/
CtrlRetCode CCommonFunction::GetDeliveryData(BSTR sXmlRec, long &lTankNumber, long &lTankReference, BOOL &bLockConnectedPumps)
{
	CtrlRetCode retCode = CTRL_RECORD_NOT_FOUND;
	BOOL bRetCode;
	
	char sXmlBuffer[MAX_XML_BUF_LEN];
	memset(sXmlBuffer, 0, sizeof(sXmlBuffer));

	int iLen = Bstr2Buffer(sXmlRec, (char *)sXmlBuffer, MAX_XML_BUF_LEN);
	if(iLen > MIN_DELIVERY_XML_LEN)
	{
		CXMLInterface  xmlDoc;
		CXMLPointers   cAllRecordsList, cRecordElement;

		try
		{
			if (xmlDoc.InitDoc())
			{
				bRetCode = xmlDoc.LoadXmlString(sXmlBuffer);

				// print the xml
				_LOGMSG.LogMsg(sXmlBuffer); // TD 258646		4.0.25.302

				if (bRetCode)
				{
					// Get all records under the root (should be only one: "Tanks")
					xmlDoc.GetChildNodesList(NULL,&cAllRecordsList);
					
					// Go to the "Tanks" element:
					xmlDoc.GetNextItem(cAllRecordsList, cRecordElement);
					retCode = _Module.m_server.m_cCommonFunction.GetTankData(&xmlDoc, &cRecordElement, lTankNumber, lTankReference, bLockConnectedPumps);

				   // Check return code:
				   if (retCode)
				   {						
				    	CString sLogMsg("CCommonFunction: GetDeliveryData failed to get info");
					   _LOGMSG.LogMsg(sLogMsg, LOG_LEVEL_1);

				   }
				}
			}

		}
		catch(...)
		{
			_LOGMSG.LogMsg("CCommonFunction","GetDeliveryData", 0, LOG_NONE, "Unexpected Error occurred");
		}
		xmlDoc.CloseDoc();
	}
	return retCode;
}

/******************************************************************************
 Description:	Convert data of TAG_PUMP_TRANSACT5 (new struct) to TAG_PUMP_TRANSACT2 (old struct)
 Returns:      	None
 Parameters:   	TAG_PUMP_TRANSACT5 *structIn - pointer to new TAG_PUMP_TRANSACT5 struct
				TAG_PUMP_TRANSACT2 *structOut - pointer to old TAG_PUMP_TRANSACT2 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			29/04/2007   14:00		Start
******************************************************************************/
void CCommonFunction::ConvertFromTrs5ToTrs2(TAG_PUMP_TRANSACT5 *structIn, TAG_PUMP_TRANSACT2 *structOut)
{
	memcpy(structOut->sActiveReadNumber,structIn->sActiveReadNumber,(sizeof(structIn->sActiveReadNumber) < sizeof(structOut->sActiveReadNumber))?sizeof(structIn->sActiveReadNumber):sizeof(structOut->sActiveReadNumber));
	memcpy(structOut->sActiveShiftNumber,structIn->sActiveShiftNumber,sizeof(structIn->sActiveShiftNumber));
	memcpy(structOut->sDateTime.sDay,structIn->sDateTime.sDay,sizeof(structIn->sDateTime.sDay));
	memcpy(structOut->sDateTime.sHour,structIn->sDateTime.sHour,sizeof(structIn->sDateTime.sHour));
	memcpy(structOut->sDateTime.sMinute,structIn->sDateTime.sMinute,sizeof(structIn->sDateTime.sMinute));
	memcpy(structOut->sDateTime.sMonth,structIn->sDateTime.sMonth,sizeof(structIn->sDateTime.sMonth));
	memcpy(structOut->sDateTime.sSecond,structIn->sDateTime.sSecond,sizeof(structIn->sDateTime.sSecond));
	memcpy(structOut->sDateTime.sYear,structIn->sDateTime.sYear,sizeof(structIn->sDateTime.sYear));
	memcpy(structOut->sGrade,structIn->sGrade,sizeof(structIn->sGrade));
	memcpy(structOut->sGrade2,structIn->sGrade2,sizeof(structIn->sGrade2));
	memcpy(structOut->sGradePrice,structIn->sGradePrice,sizeof(structIn->sGradePrice));
	memcpy(structOut->sLevel,structIn->sLevel,sizeof(structIn->sLevel));
	memcpy(structOut->sLinkFlags,structIn->sLinkFlags,sizeof(structIn->sLinkFlags));
	memcpy(structOut->sLinkNumber,structIn->sLinkNumber,sizeof(structIn->sLinkNumber));
	memcpy(structOut->sMisc,structIn->sMisc,sizeof(structIn->sMisc));
	memcpy(structOut->sMiscH,structIn->sMiscH,sizeof(structIn->sMiscH));
	memcpy(structOut->sMode,structIn->sMode,sizeof(structIn->sMode));
	memcpy(structOut->sNozzle,structIn->sNozzle,sizeof(structIn->sNozzle));
	memcpy(structOut->sNumber,structIn->sNumber,sizeof(structIn->sNumber));
	memcpy(&structOut->sOffers2Pos,&structIn->sOffers2Pos,sizeof(structIn->sOffers2Pos));
	memcpy(structOut->sOnOffer,structIn->sOnOffer,sizeof(structIn->sOnOffer));
	memcpy(structOut->sPrePaidValue,structIn->sPrePaidValue,sizeof(structIn->sPrePaidValue));
	memcpy(structOut->sPump,structIn->sPump,sizeof(structIn->sPump));
	memcpy(structOut->sRawVolume,structIn->sRawVolume,sizeof(structIn->sRawVolume));
	memcpy(structOut->sRes,structIn->sRes,sizeof(structIn->sRes));
	memcpy(structOut->sResExt,structIn->sResExt,sizeof(structIn->sResExt));
	memcpy(structOut->sRoundedValue,structIn->sRoundedValue,sizeof(structIn->sRoundedValue));
	memcpy(structOut->sRoundedVolume,structIn->sRoundedVolume,sizeof(structIn->sRoundedVolume));
	memcpy(structOut->sServiceMode,structIn->sServiceMode,sizeof(structIn->sServiceMode));
	memcpy(structOut->sStatus,structIn->sStatus,sizeof(structIn->sStatus));

};

/******************************************************************************
 Description:	Convert data of TAG_PUMP2_EX (new struct) to TAG_PUMP2 (old struct)
 Returns:      	None
 Parameters:   	TAG_PUMP2_EX *structIn - pointer to new TAG_PUMP2_EX struct
				TAG_PUMP2 *structOut - pointer to old TAG_PUMP2 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			29/04/2007   15:30		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagPump2ExToTagPump2(TAG_PUMP2_EX *structIn, TAG_PUMP2 *structOut)
{
	memcpy(&structOut->pumpStat,&structIn->pumpStat,sizeof(structIn->pumpStat));
	memcpy(structOut->sNumber ,structIn->sNumber,sizeof(structIn->sNumber));
	memcpy(structOut->sNumberOfTrs ,structIn->sNumberOfTrs,sizeof(structIn->sNumberOfTrs));
	memcpy(structOut->sPumpMenufacture ,structIn->sPumpMenufacture,sizeof(structIn->sPumpMenufacture));
	memcpy(structOut->sType ,structIn->sType,sizeof(structIn->sType));
	for (int i=0; i<MAX_TRS_IN_LIST; i++ )
		ConvertFromTrs5ToTrs2(&(structIn->pumpTrs[i]), &(structOut->pumpTrs[i]));

};


/******************************************************************************
 Description:	Convert data of TAG_PUMP3_EX (new struct) to TAG_PUMP3 (old struct)
 Returns:      	None
 Parameters:   	TAG_PUMP3_EX *structIn - pointer to new TAG_PUMP3_EX struct
				TAG_PUMP3 *structOut - pointer to old TAG_PUMP3 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			29/04/2007   15:30		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagPump3ExToTagPump3(TAG_PUMP3_EX *structIn, TAG_PUMP3 *structOut)
{
	memcpy(&structOut->pumpStat ,&structIn->pumpStat,sizeof(structIn->pumpStat));
	for (int i=0; i<MAX_TRS_IN_LIST; i++ )
		ConvertFromTrs5ToTrs2(&structIn->pumpTrs[i], &structOut->pumpTrs[i]);
	memcpy(structOut->sFullVehicleId ,structIn->sFullVehicleId,sizeof(structIn->sFullVehicleId));
	memcpy(structOut->sNumber ,structIn->sNumber,sizeof(structIn->sNumber));
	memcpy(structOut->sNumberOfTrs ,structIn->sNumberOfTrs,sizeof(structIn->sNumberOfTrs));
	memcpy(structOut->sPumpMenufacture ,structIn->sPumpMenufacture,sizeof(structIn->sPumpMenufacture));
	memcpy(structOut->sType ,structIn->sType,sizeof(structIn->sType));
};

/******************************************************************************
 Description:	Convert data of TAG_PUMP4_EX (new struct) to TAG_PUMP4 (old struct)
 Returns:      	None
 Parameters:   	TAG_PUMP4_EX *structIn - pointer to new TAG_PUMP4_EX struct
				TAG_PUMP4 *structOut - pointer to old TAG_PUMP4 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			29/04/2007   15:30		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagPump4ExToTagPump4(TAG_PUMP4_EX *structIn, TAG_PUMP4 *structOut)
{
	memcpy(&structOut->pumpStat ,&structIn->pumpStat,sizeof(structIn->pumpStat));
	for (int i=0; i<MAX_TRS_IN_LIST; i++ )
		ConvertFromTrs5ToTrs2(&structIn->pumpTrs[i], &structOut->pumpTrs[i]);
	memcpy(structOut->sFullVehicleId ,structIn->sFullVehicleId,sizeof(structIn->sFullVehicleId));
	memcpy(&structOut->sNoParallelPakPrePayflg ,&structIn->sNoParallelPakPrePayflg,sizeof(structIn->sNoParallelPakPrePayflg));
	memcpy(structOut->sNumber ,structIn->sNumber,sizeof(structIn->sNumber));
	memcpy(structOut->sNumberOfTrs ,structIn->sNumberOfTrs,sizeof(structIn->sNumberOfTrs));
	memcpy(&structOut->sPumpAsTerminal ,&structIn->sPumpAsTerminal,sizeof(structIn->sPumpAsTerminal));
	memcpy(structOut->sPumpMenufacture ,structIn->sPumpMenufacture,sizeof(structIn->sPumpMenufacture));
	memcpy(structOut->sType ,structIn->sType,sizeof(structIn->sType));
};

/******************************************************************************
 Description:	Convert data of TAG_PUMP5_EX (new struct) to TAG_PUMP5 (old struct)
 Returns:      	None
 Parameters:   	TAG_PUMP5_EX *structIn - pointer to new TAG_PUMP5_EX struct
				TAG_PUMP5 *structOut - pointer to old TAG_PUMP5 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			01/05/2007   17:00		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagPump5ExToTagPump5(TAG_PUMP5_EX *structIn, TAG_PUMP5 *structOut)
{
	ConvertFromTagPump4ExToTagPump4(&structIn->cTagPump4,  &structOut->cTagPump4);
	memcpy(structOut->sTerminalMsg ,structIn->sTerminalMsg,sizeof(structIn->sTerminalMsg));
};

/******************************************************************************
 Description:	Convert data of TAG_PUMP6_EX (new struct) to TAG_PUMP6 (old struct)
 Returns:      	None
 Parameters:   	TAG_PUMP6_EX *structIn - pointer to new TAG_PUMP6_EX struct
				TAG_PUMP6 *structOut - pointer to old TAG_PUMP6 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			01/05/2007   17:00		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagPump6ExToTagPump6(TAG_PUMP6_EX *structIn, TAG_PUMP6 *structOut)
{
	ConvertFromTagPump4ExToTagPump4(&structIn->cTagPump4,  &structOut->cTagPump4);
	memcpy(&structOut->sIsPumpDiesel ,&structIn->sIsPumpDiesel,sizeof(structIn->sIsPumpDiesel));
	memcpy(structOut->sShortCardNameTrsArr ,structIn->sShortCardNameTrsArr,sizeof(structIn->sShortCardNameTrsArr));
};


/******************************************************************************
 Description:	Convert data of TAG_PUMP7_EX (new struct) to TAG_PUMP7 (old struct)
 Returns:      	None
 Parameters:   	TAG_PUMP7_EX *structIn - pointer to new TAG_PUMP7_EX struct
				TAG_PUMP7 *structOut - pointer to old TAG_PUMP7 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			01/05/2007   17:00		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagPump7ExToTagPump7(TAG_PUMP7_EX *structIn, TAG_PUMP7 *structOut)
{
	ConvertFromTagPump4ExToTagPump4(&structIn->cTagPump4,  &structOut->cTagPump4);
	memcpy(&structOut->sDieselPump ,&structIn->sDieselPump,sizeof(structIn->sDieselPump));
	memcpy(structOut->sShortCardNameTrsArr ,structIn->sShortCardNameTrsArr,sizeof(structIn->sShortCardNameTrsArr));
	memcpy(structOut->sTerminalMsg ,structIn->sTerminalMsg,sizeof(structIn->sTerminalMsg));

};

/******************************************************************************
 Description:	Convert data of TAG_PUMP8_EX (new struct) to TAG_PUMP8 (old struct)
 Returns:      	None
 Parameters:   	TAG_PUMP8_EX *structIn - pointer to new TAG_PUMP8_EX struct
				TAG_PUMP8 *structOut - pointer to old TAG_PUMP8 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			01/05/2007   17:00		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagPump8ExToTagPump8(TAG_PUMP8_EX *structIn, TAG_PUMP8 *structOut)
{
	ConvertFromTagPump4ExToTagPump4(&structIn->cTagPump4,  &structOut->cTagPump4);
	memcpy(&structOut->sIsPumpDiesel ,&structIn->sIsPumpDiesel,sizeof(structIn->sIsPumpDiesel));
	memcpy(structOut->sShortCardNameTrsArr ,structIn->sShortCardNameTrsArr,sizeof(structIn->sShortCardNameTrsArr));
	memcpy(structOut->sTerminalMsg ,structIn->sTerminalMsg,sizeof(structIn->sTerminalMsg));
	memcpy(structOut->sPreSetLimit ,structIn->sPreSetLimit,sizeof(structIn->sPreSetLimit));
	memcpy(structOut->sPreSetLimitType ,structIn->sPreSetLimitType,sizeof(structIn->sPreSetLimitType));

};
/******************************************************************************
 Description:	Convert data of TAG_PRE_PAY_REC2_EX (new struct) to TAG_PRE_PAY_REC2 (old struct)
 Returns:      	None
 Parameters:   	TAG_PRE_PAY_REC2_EX *structIn - pointer to new TAG_PRE_PAY_REC2_EX struct
				TAG_PRE_PAY_REC2 *structOut - pointer to old TAG_PRE_PAY_REC2 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			01/05/2007   15:30		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagPrepay2ExToTagPrepay2(TAG_PRE_PAY_REC2_EX *structIn, TAG_PRE_PAY_REC2 *structOut)
{
	memcpy(structOut->sFiler ,structIn->sFiler,sizeof(structIn->sFiler));
	memcpy(structOut->sPosOffers ,structIn->sPosOffers,sizeof(structIn->sPosOffers));
	memcpy(structOut->sTotalReward ,structIn->sTotalReward,sizeof(structIn->sTotalReward));
	memcpy(&structOut->tagInfo ,&structIn->tagInfo,sizeof(structIn->tagInfo));
	ConvertFromTrs5ToTrs2(&structIn->tagTrs, &structOut->tagTrs);

};

/******************************************************************************
 Description:	Convert data of TAG_PRE_PAY_REC_EX (new struct) to TAG_PRE_PAY_REC (old struct)
 Returns:      	None
 Parameters:   	TAG_PRE_PAY_REC_EX *structIn - pointer to new TAG_PRE_PAY_REC_EX struct
				TAG_PRE_PAY_REC *structOut - pointer to old TAG_PRE_PAY_REC struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			01/05/2007   15:30		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagPrepayExToTagPrepay(TAG_PRE_PAY_REC_EX *structIn, TAG_PRE_PAY_REC *structOut)
{
	memcpy(&structOut->tagInfo ,&structIn->tagInfo,sizeof(structIn->tagInfo));
	ConvertFromTrs5ToTrs2(&structIn->tagTrs, &structOut->tagTrs);
};


/******************************************************************************
 Description:	Convert data of TAG_OLA_REC2_EX (new struct) to TAG_OLA_REC2 (old struct)
 Returns:      	None
 Parameters:   	TAG_OLA_REC2_EX *structIn - pointer to new TAG_OLA_REC2_EX struct
				TAG_OLA_REC2 *structOut - pointer to old TAG_OLA_REC2 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			01/05/2007   15:50		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagOlaRec2ExToTagOlaRec2(TAG_OLA_REC2_EX *structIn, TAG_OLA_REC2 *structOut)
{
	memcpy(&structOut->cardData ,&structIn->cardData,sizeof(structIn->cardData));
	memcpy(&structOut->extraData ,&structIn->extraData,sizeof(structIn->extraData));
	memcpy(structOut->lIndexNumber ,structIn->lIndexNumber,sizeof(structIn->lIndexNumber));
	ConvertFromTrs5ToTrs2(&structIn->tagTrs, &structOut->tagTrs);
};

/******************************************************************************
 Description:	Convert data of TAG_OLA_REC5_EX (new struct) to TAG_OLA_REC5 (old struct)
 Returns:      	None
 Parameters:   	TAG_OLA_REC5_EX *structIn - pointer to new TAG_OLA_REC5_EX struct
				TAG_OLA_REC5 *structOut - pointer to old TAG_OLA_REC5 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			02/05/2007   12:00		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagOlaRec5ExToTagOlaRec5(TAG_OLA_REC5_EX *structIn, TAG_OLA_REC5 *structOut)
{
	memcpy(&structOut->cardData ,&structIn->cardData,sizeof(structIn->cardData));
	memcpy(&structOut->extraData ,&structIn->extraData,sizeof(structIn->extraData));
	memcpy(&structOut->extraData2 ,&structIn->extraData2,sizeof(structIn->extraData2));
	memcpy(&structOut->extraData3 ,&structIn->extraData3,sizeof(structIn->extraData3)); //4.0.17.502
	memcpy(structOut->lIndexNumber ,structIn->lIndexNumber,sizeof(structIn->lIndexNumber));
	ConvertFromTrs5ToTrs2(&structIn->tagTrs, &structOut->tagTrs);
};

/******************************************************************************
 Description:	Convert data of TAG_OLA_REC9_EX (new struct) to TAG_OLA_REC9 (old struct)
 Returns:      	None
 Parameters:   	TAG_OLA_REC9_EX *structIn - pointer to new TAG_OLA_REC9_EX struct
				TAG_OLA_REC9 *structOut - pointer to old TAG_OLA_REC9 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			02/05/2007   12:30		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagOlaRec9ExToTagOlaRec9(TAG_OLA_REC9_EX *structIn, TAG_OLA_REC9 *structOut)
{
	memcpy(&structOut->cardData ,&structIn->cardData,sizeof(structIn->cardData));
	memcpy(&structOut->extraData ,&structIn->extraData,sizeof(structIn->extraData));
	memcpy(&structOut->extraData2 ,&structIn->extraData2,sizeof(structIn->extraData2));
	memcpy(&structOut->extraData3 ,&structIn->extraData3,sizeof(structIn->extraData3));
	memcpy(structOut->lIndexNumber ,structIn->lIndexNumber,sizeof(structIn->lIndexNumber));
	memcpy(&structOut->Items ,&structIn->Items,sizeof(structIn->Items));
	memcpy(&structOut->Items2 ,&structIn->Items2,sizeof(structIn->Items2));
	memcpy(&structOut->ReceiptExtraInfo ,&structIn->ReceiptExtraInfo,sizeof(structIn->ReceiptExtraInfo));
	ConvertFromTrs5ToTrs2(&structIn->tagTrs, &structOut->tagTrs);
};

/******************************************************************************
 Description:	Convert data of TAG_OLA_REC12 (new struct) to TAG_OLA_REC9 (old struct)
 Returns:      	None
 Parameters:   	TAG_OLA_REC12 *structIn - pointer to new TAG_OLA_REC12 struct
				TAG_OLA_REC9 *structOut - pointer to old TAG_OLA_REC9 struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH			10/06/2009   12:30		Start - for backword compitability
******************************************************************************/

//4.0.27.47
void CCommonFunction::ConvertFromTagOlaRec12ToTagOlaRec9(TAG_OLA_REC12 *structIn, TAG_OLA_REC9 *structOut)
{
	memcpy(&structOut->cardData ,&structIn->tagOlaRec11.tagOlaRec10.cardData,sizeof(structIn->tagOlaRec11.tagOlaRec10.cardData));
	memcpy(&structOut->extraData ,&structIn->tagOlaRec11.tagOlaRec10.extraData,sizeof(structIn->tagOlaRec11.tagOlaRec10.extraData));
	memcpy(&structOut->extraData2 ,&structIn->tagOlaRec11.tagOlaRec10.extraData2,sizeof(structIn->tagOlaRec11.tagOlaRec10.extraData2));
	memcpy(&structOut->extraData3 ,&structIn->tagOlaRec11.tagOlaRec10.extraData3,sizeof(structIn->tagOlaRec11.tagOlaRec10.extraData3));
	memcpy(structOut->lIndexNumber ,structIn->tagOlaRec11.tagOlaRec10.lIndexNumber,sizeof(structIn->tagOlaRec11.tagOlaRec10.lIndexNumber));
	memcpy(&structOut->Items ,&structIn->tagOlaRec11.tagOlaRec10.Items,sizeof(structIn->tagOlaRec11.tagOlaRec10.Items));
	memcpy(&structOut->Items2 ,&structIn->tagOlaRec11.tagOlaRec10.Items2,sizeof(structIn->tagOlaRec11.tagOlaRec10.Items2));
	memcpy(&structOut->ReceiptExtraInfo ,&structIn->tagOlaRec11.tagOlaRec10.ReceiptExtraInfo,sizeof(structIn->tagOlaRec11.tagOlaRec10.ReceiptExtraInfo));
	ConvertFromTrs5ToTrs2(&structIn->tagOlaRec11.tagOlaRec10.tagTrs, &structOut->tagTrs);
};
/******************************************************************************
 Description:	Convert data of TAG_OLA_MEM_REC_EX (new struct) to TAG_OLA_MEM_REC (old struct)
 Returns:      	None
 Parameters:   	TAG_OLA_MEM_REC_EX *structIn - pointer to new TAG_OLA_MEM_REC_EX struct
				TAG_OLA_MEM_REC *structOut - pointer to old TAG_OLA_MEM_REC struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			01/05/2007   15:50		Start
******************************************************************************/

void CCommonFunction::ConvertFromTagOlaMemRecExToTagOlaMemRec(TAG_OLA_MEM_REC_EX *structIn, TAG_OLA_MEM_REC *structOut)
{
	memcpy(&structOut->cardData ,&structIn->cardData,sizeof(structIn->cardData));
	memcpy(structOut->sAuthAmount ,structIn->sAuthAmount,sizeof(structIn->sAuthAmount));
	memcpy(structOut->sFuelTotal ,structIn->sFuelTotal,sizeof(structIn->sFuelTotal));
	memcpy(structOut->sTotalAmount ,structIn->sTotalAmount,sizeof(structIn->sTotalAmount));
	ConvertFromTrs5ToTrs2(&structIn->tagTrs, &structOut->tagTrs2);
};

/******************************************************************************
 Description:	Generate row data information  of transaction  PAK /PrePay / Attendant 	
				this method wraps the new TAG_PUMP_TRANSACT5 struct instead of TAG_PUMP_TRANSACT2
 Returns:      	
 Parameters:  BSTR *sData      - out buffer of report 
              long *plRecCount - total record found 
			  long lFlags 
			  BSTR sRestrictionData -  restriction data  	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			09/05/2007   9:58		Start for TD 38654
******************************************************************************/
GetPAKQueryRetCode CCommonFunction::GetPAKQuery2(BSTR *sData, long *plRecCount, long *lFlags, BSTR sRestrictionData , long lPumpNumber) //4.0.9.507
{
	long lTrsCount = 0;
	long lNumTrs  = 0;
	long lVersion = 0;
	GetPAKQueryRetCode eRetVal = GetPakQuery_OK ; 
	int iPump = 0 , iTrs = 0;
	long lLastValidPump = 0;
	BOOL bUnBalancePrepay = FALSE;
	BOOL bWaitPrepay = FALSE;
	BOOL bAuthorizedPrepay = FALSE; // TD 38806 - 4.0.16.501
	BOOL bInProgressPrePay = FALSE;

	lLastValidPump = _Module.m_server.GetLastValidPump();//4.0.3.37

	CComBSTR bstrTransactions;
	bstrTransactions.Empty();

	CPumpTransact cTmpTrs;

	TAG_PUMP_TRANSACT5 cTagPumpTrs;
	memset(&cTagPumpTrs , ' ' , sizeof(cTagPumpTrs));

	char sTmpStr[sizeof(TAG_PUMP_TRANSACT5) + 2];
	memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT5) , 0 , 2);


	lVersion = 2;

	if(eRetVal == GetPakQuery_OK)
	{
		if( *lFlags & PAK_QUERY_UNPAID_PAK )
		{
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);
			
			//We may make the version depend on the lFlags parameter
			//in the future

			if (lPumpNumber)		//4.0.9.507
				iPump =  lPumpNumber-1;							//?.??.?.?? - TD 335186
			
			for(;iPump < lLastValidPump; iPump++)
			{
				if ((lPumpNumber) && (iPump >= lPumpNumber))		//4.0.9.507 //?.??.?.?? - TD 335186
					break;

				lNumTrs = _Module.m_server.m_cPumpArray[iPump].GetTrsArraySize();

				for(;iTrs < lNumTrs;iTrs++)
				{
					_Module.m_server.m_cPumpArray[iPump].GetTrsByIndex(iTrs , &cTmpTrs);
#ifdef DEBUG		
					CString str; str.Format("3. transaction %ld ,sMaxDiscountAllowed %ld", cTmpTrs.m_lNumber, cTmpTrs.m_sMaxDiscountAllowed); if (LOG_ALL_FLOW_CONTROL) _LOGMSG.LogMsg(str);
#endif
					//RFUEL-3309 Get the maximal discount allowed for the grade 
					SetMaximalAllowedDiscountToPumpTransaction(&cTmpTrs);
					if((cTmpTrs.m_lRes == PAY_AT_KIOSK) && 
					   (cTmpTrs.GetStatus() == UNPAID)) //4.0.22.503 69173 && (!cTmpTrs.m_lOnOffer) )//4.0.3.46
					{
						if ((*lFlags & PAK_QUERY_UNPAID_ON_OFFER_PAK) || (!cTmpTrs.m_lOnOffer)) //4.0.22.503 69173
						{
							cTmpTrs.GetTag(&cTagPumpTrs , lVersion);
							
							ConvertToExtendedGradeForPAKQuery(lPumpNumber, cTagPumpTrs.sGrade, sizeof(cTagPumpTrs.sGrade), *lFlags); //4.0.28.500 - TD 224805

							lTrsCount++;

							memcpy((BYTE*)sTmpStr , &cTagPumpTrs , sizeof(cTagPumpTrs));

							bstrTransactions.Append((LPCSTR)sTmpStr);
						
							memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT5) , 0 , 2);
						}
					}
				}

				//Reset trs index
				iTrs = 0;
			}

			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}

		if( *lFlags & PAK_QUERY_UNPAID_PAK_BY_PUMP )		//4.0.20.501 - TD 65908
		{
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			if (lPumpNumber > 0 && lPumpNumber <= lLastValidPump)
			{
				iPump =  lPumpNumber - 1;

				lNumTrs = _Module.m_server.m_cPumpArray[iPump].GetTrsArraySize();

				for(;iTrs < lNumTrs;iTrs++)
				{
					BOOL bGetTrs = TRUE;
					_Module.m_server.m_cPumpArray[iPump].GetTrsByIndex(iTrs , &cTmpTrs);
#ifdef DEBUG		
					CString str; str.Format("4. transaction %ld ,sMaxDiscountAllowed %ld", cTmpTrs.m_lNumber, cTmpTrs.m_sMaxDiscountAllowed); if (LOG_ALL_FLOW_CONTROL) _LOGMSG.LogMsg(str);
#endif
					//RFUEL-3309 Get the maximal discount allowed for the grade 
					SetMaximalAllowedDiscountToPumpTransaction(&cTmpTrs);
					if((cTmpTrs.m_lRes == PAY_AT_KIOSK) && 
					   (cTmpTrs.GetStatus() == UNPAID) && (!cTmpTrs.m_lOnOffer) )//4.0.3.46
					{
						
						cTmpTrs.GetTag(&cTagPumpTrs , lVersion);
						
						//4.0.26.501 - TD 116227
						//Make extended grade conversion if needed
						/*if (_Module.m_server.GetMaxGrade999() && !(*lFlags & PAK_QUERY_IGNORE_EXTENDED_GRADE_ID_CONVERSION))
							_Module.m_server.GetExtGradeByGrade(cTagPumpTrs.sGrade, sizeof(cTagPumpTrs.sGrade), lPumpNumber);*/
						ConvertToExtendedGradeForPAKQuery(lPumpNumber, cTagPumpTrs.sGrade, sizeof(cTagPumpTrs.sGrade), *lFlags); //4.0.28.500 - TD 224805
						
						//4.0.22.507 - TD 72675
						if( *lFlags & PAK_QUERY_UNPAID_PAK_BY_PUMP_NOZZLE )
						{
							if(plRecCount && *plRecCount > 0 && *plRecCount <= MAX_POSITIONS_PER_PUMP)
							{
								long lNozzle = a2l(cTagPumpTrs.sNozzle, sizeof(cTagPumpTrs.sNozzle));
								if (lNozzle != *plRecCount)
									bGetTrs = FALSE;
							}
						}

						if (bGetTrs)
						{
							lTrsCount++;

							memcpy((BYTE*)sTmpStr , &cTagPumpTrs , sizeof(cTagPumpTrs));

							bstrTransactions.Append((LPCSTR)sTmpStr);
						
							memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT5) , 0 , 2);
						}
					}
				}

				//Reset trs index
				iTrs = 0;
			}
			else
			{
				eRetVal = GetPakQuery_MissingData;
			}

			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}
		//4.0.3.45 - check if need to read from prepay qdex
		if( (*lFlags & PAK_QUERY_UNBALANCE_PREPAY) || (*lFlags & PAK_QUERY_WAIT_PREPAY) || (*lFlags & PAK_QUERY_AUTH_PREPAY) || (*lFlags & PAK_QUERY_IN_PROGRESS_PREPAY))
		{
			long lRtc = 0;
			PRE_PAY_INFO info;			
			lRtc = _Module.m_server.IndexOperation(PUMP_PRE_PAY_FILE,REC_READ_FIRST,&cTmpTrs,&info);
			
			while (!lRtc)
			{
				bUnBalancePrepay = FALSE;
				bWaitPrepay = FALSE;
				bAuthorizedPrepay = FALSE;
				bInProgressPrePay = FALSE;
#ifdef DEBUG		
				CString str; str.Format("5. transaction %ld ,sMaxDiscountAllowed %ld", cTmpTrs.m_lNumber, cTmpTrs.m_sMaxDiscountAllowed); if (LOG_ALL_FLOW_CONTROL) _LOGMSG.LogMsg(str);
#endif
				//RFUEL-3309 Get the maximal discount allowed for the grade 
				SetMaximalAllowedDiscountToPumpTransaction(&cTmpTrs);
				if ((info.lPumpNumber == lPumpNumber) || (!lPumpNumber))		//4.0.9.507
				{
					if( _Module.m_server.IsUnbalancedPrepay(&cTmpTrs,&info) && (*lFlags & PAK_QUERY_UNBALANCE_PREPAY) )
						bUnBalancePrepay = TRUE;
					if ( (cTmpTrs.m_lStatus == WAIT_AUTH || cTmpTrs.m_lStatus == WAIT_TIME_OUT || cTmpTrs.m_lStatus == WAIT_RES ) && (*lFlags & PAK_QUERY_WAIT_PREPAY) )
						bWaitPrepay = TRUE;	
										
					if( (cTmpTrs.m_lStatus == WAIT_PROGRESS) && (*lFlags & PAK_QUERY_AUTH_PREPAY) )// TD 38806 - 4.0.16.501
						bAuthorizedPrepay = TRUE;

					if ((cTmpTrs.m_lStatus == IN_PROGRESS) && (*lFlags & PAK_QUERY_IN_PROGRESS_PREPAY))
						bInProgressPrePay = TRUE;

					if(bUnBalancePrepay || bWaitPrepay || bAuthorizedPrepay || bInProgressPrePay) //4.0.16.501
					{
						cTmpTrs.GetTag(&cTagPumpTrs , lVersion, info.lMaxValue, 0, 0, info.lPurchaseID);	//4.0.26.508 7.
						
						ConvertToExtendedGradeForPAKQuery(lPumpNumber, cTagPumpTrs.sGrade, sizeof(cTagPumpTrs.sGrade), *lFlags); //4.0.28.500 - TD 224805

						lTrsCount++;

						memcpy((BYTE*)sTmpStr , &cTagPumpTrs , sizeof(cTagPumpTrs));

						bstrTransactions.Append((LPCSTR)sTmpStr);

						memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT5) , 0 , 2);

					}
				}
				
				lRtc = _Module.m_server.IndexOperation(PUMP_PRE_PAY_FILE,REC_READ_NEXT,&cTmpTrs,&info);
			}
		}
	}

	if (*lFlags & PAK_QUERY_FOR_ATTENDANT)	//4.0.5.42
	{
		EXTRA_INFO_REC sExtraInfoRec;
		char szTrack2[MAX_TABLE_NAME]; 
		char szTmpKey[MAX_TABLE_NAME]; 
		memset(szTrack2,' ',sizeof(szTrack2));
		memset(szTmpKey,' ',sizeof(szTmpKey));
		DWORD dwLastErr=0;
		int iLen = 0;

		if (sRestrictionData)
		{
			 iLen = WideCharToMultiByte(CP_ACP,0,sRestrictionData,-1,szTrack2,sizeof(szTrack2),NULL,NULL);
		}

		if(iLen == 0)
		{
			dwLastErr = GetLastError();
			eRetVal = GetPakQuery_MissingData;
		}			

		if(eRetVal == GetPakQuery_OK)
		{
			long lRtc = 0;

			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);
			
			//We may make the version depend on the lFlags parameter
			//in the future
			
			iPump =  lPumpNumber; // .4.0.9.507

			for(;iPump < lLastValidPump; iPump++)
			{
				lNumTrs = _Module.m_server.m_cPumpArray[iPump].GetTrsArraySize();

				if ((lPumpNumber) && (iPump > lPumpNumber))		//4.0.9.507
					break;

				for(;iTrs < lNumTrs;iTrs++)
				{
					_Module.m_server.m_cPumpArray[iPump].GetTrsByIndex(iTrs , &cTmpTrs);
#ifdef DEBUG		
					CString str; str.Format("6. transaction %ld ,sMaxDiscountAllowed %ld", cTmpTrs.m_lNumber, cTmpTrs.m_sMaxDiscountAllowed); if (LOG_ALL_FLOW_CONTROL) _LOGMSG.LogMsg(str);
#endif
					//RFUEL-3309 Get the maximal discount allowed for the grade 
					SetMaximalAllowedDiscountToPumpTransaction(&cTmpTrs);
					if((cTmpTrs.m_lRes == PAY_AT_KIOSK) && 
					   (cTmpTrs.m_lResExt == RES_EXT_ATTENDANT) && (!cTmpTrs.m_lOnOffer) )
					{					
						sExtraInfoRec.info.lIndexNumber = cTmpTrs.m_lNumber; 
						lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE,REC_READ, &sExtraInfoRec);
						 
						//Verify that we found the correct attendant details 
						if (!lRtc) 
						{
							if(*lFlags & PAK_QUERY_FOR_ATTENDANT_BY_ID)
								memcpy(szTmpKey,sExtraInfoRec.info.sAttendantID,sizeof(sExtraInfoRec.info.sAttendantID));
							else
								memcpy(szTmpKey,sExtraInfoRec.info.sTrack2Data,sizeof(sExtraInfoRec.info.sTrack2Data));

							if(!memcmp(szTmpKey ,szTrack2 , sizeof(sExtraInfoRec.info.sTrack2Data)))
							{
								cTmpTrs.GetTag(&cTagPumpTrs , lVersion);

								lTrsCount++;

								memcpy((BYTE*)sTmpStr , &cTagPumpTrs , sizeof(cTagPumpTrs));

								bstrTransactions.Append((LPCSTR)sTmpStr);

								memset((BYTE*)sTmpStr + sizeof(TAG_PUMP_TRANSACT5) , 0 , 2);
							}
						}
					}
				}

				//Reset trs index
				iTrs = 0;
			}

			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}		
	}

	if(plRecCount)
		*plRecCount = lTrsCount;

	if(sData)
		*sData = bstrTransactions.Copy();

#ifdef DEBUG
	if (LOG_ALL_FLOW_CONTROL)  
	{
		CString str1(bstrTransactions);
		CString str = "GetPAKQuery2 returns Buf[" + str1+"]";
		_LOGMSG.LogMsg(str);
	}
#endif // DEBUG
	return eRetVal;
}

/******************************************************************************
 Description:	Set the pump Misc directly to Pump array and to the PumpStat Qdx
 Returns:      	
 Parameters:   
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH 			06/03/2008   15:30		Start
******************************************************************************/
void CCommonFunction::SetPumpMisc(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal, BOOL bMiscExt)	//4.0.21.501
{
	CPumpStatus		pumpStat;
	GPIRetCode retCode = GPI_OK;

	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();
	long lTmpMisc;
	if (bMiscExt)	//4.0.20.451 65268 //4.0.21.501
		lTmpMisc = pumpStat.GetMiscExt();
	else
		lTmpMisc = pumpStat.GetMisc();
	
	if (bRemoveMisc)
		lTmpMisc &= lMisc;
	else
		lTmpMisc |= lMisc;

	if (bMiscExt)	//4.0.20.451 65268	//4.0.21.501
		pumpStat.SetMiscExt(lTmpMisc);
	else
		pumpStat.SetMisc(lTmpMisc);
	
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetStat(pumpStat);
	_Module.m_server.SavePumpInfo(lPumpNumber);

	if (slPumpArray.IsLocked())
		slPumpArray.Unlock();

	if(pVal != NULL)
		*pVal = retCode;
}


void CCommonFunction::GetTransactionList4IFSFTerminal(long &lPumpNumber, long lPosNumber, long lFlag, char * szTrack2, long lenTrack2)
{
	_Module.m_server.m_cGPIEventMgr.AddRequestGetTransactionList(lPumpNumber , lPosNumber, lFlag, szTrack2, lenTrack2); // CID_199328 Out-of-bounds access
	//Get handle Pos receipt 
	CSingleLock slPosMap( &(_Module.m_server.m_csPosMap));	
	slPosMap.Lock(INFINITE);
	CPosInfo tmpPos;
	
	_Module.m_server.m_cPosMap.Lookup(lPosNumber,tmpPos);
	slPosMap.Unlock();

	// if we get the event in 1 second we get receipt
	if (tmpPos.WaitForSingleObjectReceipt(MAX_TERMINAL_SRV_WAIT))
	{
		CComBSTR strResult;
		//Get handle Pos receipt 
		CSingleLock slPosMap( &(_Module.m_server.m_csPosMap));	
		slPosMap.Lock(INFINITE);
		CPosInfo tmpPos;

		////////////////////////////////////////////////////////////////////////////////////////
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;  //4.0.5.12
			char szResult[MAX_FIELD_VAL]={0};
			long lOutBuffLen = 0;
			
		}			
		/////////////////////////////////////////////////////////////////////////////////////////

		
		slPosMap.Unlock();
	}

}

//4.0.19.504
BOOL CCommonFunction::DeleteAllLoyaltyXmls()
{
	CString str;
	BOOL bRet = TRUE;
	
	char szDirName[MAX_FILE_PATH] = {0};
	_PARAM.LoadParam("FuelLoyalty\\SysParam","LoyaltyDataPath",szDirName,sizeof(szDirName),"c:\\Office\\pumpSrv\\log",1);
	
	//Create the delete command with the loyalty file name
	char szLoyaltySummaryDeleteCommand[MAX_PATH];
	sprintf(szLoyaltySummaryDeleteCommand, "del LoyaltySummary*.xml"); 

	char szLoyaltyPromoationsDeleteCommand[MAX_PATH];
	sprintf(szLoyaltyPromoationsDeleteCommand, "del LoyaltyPromotions*.xml");


	if(SetCurrentDirectory(szDirName))
	{
		// Run the delete command
		system(szLoyaltySummaryDeleteCommand);
		system(szLoyaltyPromoationsDeleteCommand);
		str.Format("CCommonFunction::DeleteAllLoyaltyXmls command %s ran successfully in directory %s.", szLoyaltySummaryDeleteCommand, szDirName); 
		_LOGMSG.LogMsg(str);
		
	}
	else
	{
		str.Format("CCommonFunction::DeleteAllLoyaltyXmls Failed to set directory to %s.", szDirName); 
		_LOGMSG.LogMsg(str);
		bRet = FALSE;
	}
	return bRet;
}

//4.0.19.504
BOOL CCommonFunction::DeleteLoyaltyXmslFile(long lPumpNumber, long lTrsNum)
{

	char szFileName[MAX_FILE_PATH] = { 0 };
	GetLoyaltySummaryXmlFileName(lPumpNumber, lTrsNum, szFileName); // TD 321602   4.0.111.174

	CString str;
	BOOL bRet = TRUE;
	int iDelRes = DeleteFile(szFileName);	//4.0.19.508
	if(!iDelRes)
	{
		DWORD dwLastError = GetLastError();
		str.Format("CCommonFunction::DeleteLoyaltyXmlFile, Failed to Delete %s , Error Code = %d",szFileName,dwLastError);
		_LOGMSG.LogMsg(str, LOG_LEVEL_0);
		bRet = FALSE;
	}
	else
	{
		str.Format("CCommonFunction::DeleteLoyaltyXmlFile delete file %s returned val = %ld", szFileName, iDelRes);
		_LOGMSG.LogMsg(str);
	}

	return bRet;
}

// TD 321602   4.0.111.174 
BOOL CCommonFunction::GetLoyaltySummaryXmlFileName(long lPumpNumber , long lTrsNum , char * szOutXmlFileName)  // TD 321602
{
	BOOL bRet = TRUE;
	
	char sDirName[MAX_FILE_PATH] = {0};
	sprintf(sDirName,"%s",_Module.m_server.GetLoyaltyDataPath()); 
	
	//Create the delete command with the loyalty file name
	char sFileName[MAX_PATH] = {0};
	sprintf(sFileName, "\\LoyaltySummary_%ld_%ld.xml", ( _Module.m_server.GetLoyaltyUniqueClient() + lPumpNumber), lTrsNum); //4.0.19.508
	strcat(sDirName, sFileName);

	memcpy((char *)szOutXmlFileName, (char *) sDirName,min(sizeof(sDirName),MAX_FILE_PATH));
	
	return bRet;
}

BOOL CCommonFunction::GetLoyaltyPromotionsXmlFileName(long lPumpNumber , long lTrsNum , char * szOutXmlFileName)  // TD 321602
{
	BOOL bRet = TRUE;

	char sDirName[MAX_FILE_PATH] = {0};
	sprintf(sDirName,"%s",_Module.m_server.GetLoyaltyDataPath()); 

	//Create the delete command with the loyalty file name
	char sFileName[MAX_PATH] = {0};
	sprintf(sFileName, "\\LoyaltyPromotions_%ld_%ld.xml", ( _Module.m_server.GetLoyaltyUniqueClient() + lPumpNumber), lTrsNum); //4.0.19.508
	strcat(sDirName, sFileName);

	memcpy((char *)szOutXmlFileName, (char *) sDirName,min(sizeof(sDirName),MAX_FILE_PATH));

	return bRet;
}


/******************************************************************************	
 Description:	Get Account Number By Track2
 Returns:      	lRetLen, 0 - Exception 
						 >0 - OK
 Parameters:	char* sOutAccountNum	-	out buffer to report
				BSTR sTrack2			-	Track2			
				long lFlags
				long lPumpNumber

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			07/07/2008   13:58		Start for 4.0.18.330 TD 60203 
 Gena			10/08/2008	 12:13		Merge to TD - 60236 
 Gena			11/11/2008	 11:11		Add lPumpNumber	to parameters
 AmitH          25/05/2010   12:00      Add bDoNotUseXmlBuffer so no xml conversion will take place in GetCardInfo. 
******************************************************************************/
long CCommonFunction::GetAccountNumByTrack2(char* sOutAccountNum, const BSTR sTrack2, const long lFlags, const long lPumpNumber) 
{
	long	lRetLen = 0;

	//First check if OLA is on before GetCardInfo //4.0.25.0
	//===========================================
	if (_Module.m_server.m_cOLASrv->Valid() && OLA_BY_PUMPSRV)	
	{	
		CString				str;
		BYTE				sStrVal[sizeof(CARD_SALE_ALL3)]; 
		CARD_SALE_ALL3		cTmpCardSaleData; 
		CComBSTR			sAccountNumber("");
		CComBSTR			sShortCardNumber("");
		CComBSTR			sTrack1("");
		CComBSTR			sRetData;
		long				lRetVal	= OK;
		long				lSessionType = 0 ;
		BOOL                bDoNotUseXmlBuffer = TRUE; //4.0.25.0 - we want to work internally without xml

		lRetVal = _Module.m_server.GetCardInfo(sAccountNumber,
											   sTrack1,
											   sTrack2,
											   sShortCardNumber,
											   &sRetData,
											   lFlags,
											   &lSessionType,
											   lPumpNumber, 
											   bDoNotUseXmlBuffer); //4.0.25.0 
		
		if(lRetVal == OK)
		{	
			long lLen = Bstr2Buffer(sRetData, sStrVal ,sizeof(sStrVal)); //4.0.25.0
			
			if (!lLen)			
				_LOGMSG.LogMsg("GetAccountNumByTrack2, Bstr2Buffer failed",LOG_LEVEL_1); //4.0.25.0

			else //OK
			{
				//copy to structure: //4.0.25.0
				memset(&cTmpCardSaleData, ' ', sizeof(cTmpCardSaleData));
				memcpy(&cTmpCardSaleData, sStrVal, min(sizeof(cTmpCardSaleData),lLen));
			
				lRetLen = sizeof(cTmpCardSaleData.CardSaleAll.data.sAccountNumber);
				memcpy(sOutAccountNum, cTmpCardSaleData.CardSaleAll.data.sAccountNumber, lRetLen);
			}

			
			if (*sOutAccountNum < '0' ||  *sOutAccountNum > '9')
			{
				str.Format("GetAccountNumByTrack2, illegal account number=%.21s!",sOutAccountNum); //4.0.25.0
				_LOGMSG.LogMsg(str, LOG_LEVEL_1);
				
				lRetLen = 0; 
			}
			else 
			{//4.0.20.501 TD 60236		
				char szResult[MAX_FIELD_VAL] = {0};		
				_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, sOutAccountNum, lRetLen);

				str.Format("GetAccountNumByTrack2, convert Track2 to AccountNum = %s ", szResult); 
				
				_LOGMSG.LogMsg(str);
			}
		}
	}

	return	lRetLen;
}

//4.0.20.507
void CCommonFunction::ConvertCardSaleAll3ToPayAtPumpInfo(PAY_AT_PUMP_INFO& cPayAtPumpInfoOut,const CARD_SALE_ALL3& cCardSaleAll3In)
{
	memcpy(&cPayAtPumpInfoOut.CardSaleInfo.cardData, &cCardSaleAll3In.CardSaleAll.data, sizeof(cCardSaleAll3In.CardSaleAll.data));
	memcpy(&cPayAtPumpInfoOut.CardSaleInfo.extraData, &cCardSaleAll3In.CardSaleAll.extData, sizeof(cCardSaleAll3In.CardSaleAll.extData));

	memcpy(&cPayAtPumpInfoOut.CardSaleExtraData2, &cCardSaleAll3In.extData2, sizeof(cCardSaleAll3In.extData2));
	memcpy(&cPayAtPumpInfoOut.CardSaleExtraData3, &cCardSaleAll3In.extData3, sizeof(cCardSaleAll3In.extData3));
	memcpy(&cPayAtPumpInfoOut.CardSaleExtraData4, &cCardSaleAll3In.extData4, sizeof(cCardSaleAll3In.extData4));
	memcpy(&cPayAtPumpInfoOut.CardSaleExtraData5, &cCardSaleAll3In.extData5, sizeof(cCardSaleAll3In.extData5));
	memcpy(&cPayAtPumpInfoOut.CardSaleExtraData6, &cCardSaleAll3In.extData6, sizeof(cCardSaleAll3In.extData6));
}

/******************************************************************************
 Description:	Function check if a time out had passed.

 Returns:     Passed / Not passed 	
 Parameters:  SYSTEMTIME tTimeVal - holds the time's value 
              long lTimeOutVal    - holds the timeOut value (in milliseconds).
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		06/11/2008   13:50		Start 4.0.21.500 TD 66935
******************************************************************************/
BOOL CCommonFunction::IsTimeOutPassed(const SYSTEMTIME tTimeVal, const long lTimeOutVal)
{
	BOOL bRetVal = FALSE;

	SYSTEMTIME	tZeroTime;
	memset(&tZeroTime, 0, sizeof(SYSTEMTIME));

	if (memcmp(&tZeroTime, &tTimeVal, sizeof(SYSTEMTIME)) && GetInterval(tTimeVal) > lTimeOutVal)
	{
		bRetVal = TRUE;
	}

	return bRetVal;

}

void CCommonFunction::ConvertPayAtPumpInfoToCardSaleAll3(CARD_SALE_ALL3&  cCardSaleAll3In, const PAY_AT_PUMP_INFO& cPayAtPumpInfoOut)
{
	memcpy(&cCardSaleAll3In.CardSaleAll.data, &cPayAtPumpInfoOut.CardSaleInfo.cardData, sizeof(cPayAtPumpInfoOut.CardSaleInfo.cardData));
	memcpy(&cCardSaleAll3In.CardSaleAll.extData, &cPayAtPumpInfoOut.CardSaleInfo.extraData, sizeof(cPayAtPumpInfoOut.CardSaleInfo.extraData));

	memcpy(&cCardSaleAll3In.extData2, &cPayAtPumpInfoOut.CardSaleExtraData2, sizeof(cPayAtPumpInfoOut.CardSaleExtraData2));
	memcpy(&cCardSaleAll3In.extData3, &cPayAtPumpInfoOut.CardSaleExtraData3, sizeof(cPayAtPumpInfoOut.CardSaleExtraData3));
	memcpy(&cCardSaleAll3In.extData4, &cPayAtPumpInfoOut.CardSaleExtraData4, sizeof(cPayAtPumpInfoOut.CardSaleExtraData4));
	memcpy(&cCardSaleAll3In.extData5, &cPayAtPumpInfoOut.CardSaleExtraData5, sizeof(cPayAtPumpInfoOut.CardSaleExtraData5));
	memcpy(&cCardSaleAll3In.extData6, &cPayAtPumpInfoOut.CardSaleExtraData6, sizeof(cPayAtPumpInfoOut.CardSaleExtraData6));
}

/******************************************************************************
 Description:	Function check if Pump is on Unmanned mode or not.

 Returns:     TRUE if pump is on unmanned mode 	
 Parameters:  lPumpNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH		13/05/2009   13:50		Start 4.0.22.506 TD 74605
******************************************************************************/
BOOL CCommonFunction::IsPumpOnUnmannedMode(const long lPumpNumber)
{
	BOOL bIsUnmanned = TRUE;
	long p = lPumpNumber - 1;
	long lMode, lTmpMode;
	
	lMode = _Module.m_server.m_lPumpsManualModeArray[p];
			
	if (lMode) 
		lTmpMode = lMode;  
	else
		lTmpMode = _Module.m_server.m_cPumpArray[p].GetStat().GetMode();

	if(!(_Module.m_server.m_cModeArray[lTmpMode-1].IsUnmannedMode()))
	{
		bIsUnmanned = FALSE;			
	}
	return (bIsUnmanned);	
}


//4.0.23.500 TD 69914
//*******		TO GET NEW TAG REC SIZE :		*********
//1. Change the iMaxQEIFlags to next number 
//2. Add to adwQEIFlag the new flag
//3. Add to alTagRecSize the new size
//*******										*********

long CCommonFunction::GetTagRecSize(const short nFlags)
{
	long			lTagRecSize = sizeof(TAG_DISCOUNT_LOYALTY_INFO);

	const size_t	iMaxQEIFlag = 5; //4.0.23.504 66705
	DWORD			adwQEIFlag[iMaxQEIFlag]	=	{	QEI_STRUCT_IS_LOYALTY_INFO2,																 
													QEI_STRUCT_IS_LOYALTY_INFO3, 
													QEI_STRUCT_IS_ANPR_INFO_PLUS,
													QEI_STRUCT_IS_LOYALTY_INFO4,	//4.0.23.504 66705
													QEI_STRUCT_IS_PUMP_TEST_INFO  //4.0.25.1140 TD 412765
												};
	
	long			alTagRecSize[iMaxQEIFlag]	=	{	sizeof(TAG_DISCOUNT_LOYALTY_INFO2),															
														sizeof(TAG_DISCOUNT_LOYALTY_INFO3), 
														sizeof(TAG_ANPR_INFO_PLUS),
														sizeof(TAG_DISCOUNT_LOYALTY_INFO4), //4.0.23.504 66705
														sizeof(TAG_PUMP_TEST_INFO) //4.0.25.1140 TD 412765
													};												

	for (int i = 0; i < iMaxQEIFlag; i++)
	{
		if (nFlags & adwQEIFlag[i])
		{
			lTagRecSize = alTagRecSize[i];
			i = iMaxQEIFlag;						//Stop loop
		}
	}

	return lTagRecSize;
}

//4.0.23.500 TD 69914
long CCommonFunction::GetExtraInfoRecFromDB(EXTRA_INFO_REC* pExtraInfo, const short nFlags, const long* plIndexNumber, CString *pExtraInfoXml)
{
	long			lRetCode = 1;
	CPumpTransact	cPumpTransact;
	short			nTmpFlags = nFlags & 0x00ff;

	const size_t	iMaxQEIIndex = 3;
	BYTE			abyQEIIndex[iMaxQEIIndex]	=	{	QEI_GET_DIRECT_INDEX,
														QEI_GET_FIRST_INDEX,
														QEI_GET_NEXT_INDEX
													};
	
	eFileOperation	abyFileOp[iMaxQEIIndex]		=	{	REC_READ, 
														REC_READ_FIRST,	
														REC_READ_NEXT 
													};

	cPumpTransact.SetLink(TRS_LINK_PREPAY_NUMBER_AND_LOYALTY | TRS_LINK_EXTRA_INFO, *plIndexNumber);
	
	for (int i = 0; i < iMaxQEIIndex; i++)
	{
		if (nTmpFlags == abyQEIIndex[i])
		{
			if( nTmpFlags != QEI_GET_FIRST_INDEX )
				pExtraInfo->info.lIndexNumber = *plIndexNumber;

			lRetCode = _Module.m_server.TrsLinkOperation(abyFileOp[i], &cPumpTransact, pExtraInfo, pExtraInfoXml);

			i = iMaxQEIIndex;					//Stop loop
		}
	}

	return lRetCode;
}


//4.0.23.500 TD 69914
void CCommonFunction::ConvertExtraInfoRecord2Tag(TAG_DIESEL_SOLD_INFO* psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber, const long lPosNumber)
{
	EXTRA_INFO_REC1			cUnionInfo;
	TAG_DIESEL_INFO			cTagInfo;	
	CFleetCard				cFleetCard;

	memset(psTagInfo, ' ', sizeof(TAG_DIESEL_SOLD_INFO));
	memcpy(&cUnionInfo, &rsExtraInfo, sizeof(EXTRA_INFO_REC1));

	cFleetCard.ConvertRowExtraInfoRecord2TAG(cUnionInfo, cTagInfo);

	psTagInfo->cItemSold1 = cTagInfo.cItemSold1;
	psTagInfo->cItemSold2 = cTagInfo.cItemSold2;

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;				
		str.Format("CGPI:QueryExtraInfoData return fleet information amount= %.10s, amount2=%.10s",
												psTagInfo->cItemSold1.sValue, psTagInfo->cItemSold1.sValue);

		_LOGMSG.LogMsg(lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
	}
}


//4.0.23.500 TD 69914
void CCommonFunction::ConvertExtraInfoRecord2Tag(TAG_DISCOUNT_LOYALTY_INFO* psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber, const long lPosNumber)
{
	long lPumpNum = lPumpNumber;
	
	memcpy(psTagInfo->sLoyalId, rsExtraInfo.info.sLoyalId, sizeof(psTagInfo->sLoyalId));

	if(_Module.m_server.m_bControllerSupportNozzleRestrction)
	{
		long lTmpGrade=0;
		BOOL bValidPump = TRUE;

		if (PUMP_NOT_VALID(lPumpNumber))
		{
			if (!PUMP_NOT_VALID(rsExtraInfo.info.lPumpNumber))	
				lPumpNum = rsExtraInfo.info.lPumpNumber;
			else
				bValidPump = FALSE;
		}
		
		if (bValidPump)
		{
			for(int i= 0;i <= MAX_POSITIONS_PER_PUMP ; i++) 
			{
				if (rsExtraInfo.info.lDiscountArray[i])
				{
					lTmpGrade = _Module.m_server.m_cPumpArray[lPumpNum - 1].NozzleToGrade(i + 1);

					if ((lTmpGrade > 0 ) && (lTmpGrade <= MAX_GRADES))
					{
						l2a(rsExtraInfo.info.lDiscountArray[i], psTagInfo->sDiscountArray.sDiscount[lTmpGrade - 1], sizeof(psTagInfo->sDiscountArray.sDiscount[lTmpGrade]));			
					}
				}
			}
		}
	}
	else
	{
		for(int i=0;i<MAX_GRADES ; i++)
			l2a(rsExtraInfo.info.lDiscountArray[i] , psTagInfo->sDiscountArray.sDiscount[i], sizeof(psTagInfo->sDiscountArray.sDiscount[i]));	
	}

	memcpy(psTagInfo->sPromotionNumber, rsExtraInfo.info.sPromotionNumber, sizeof(psTagInfo->sPromotionNumber));
	memcpy(psTagInfo->sAttendantID , rsExtraInfo.info.sAttendantID, sizeof(psTagInfo->sAttendantID));
	l2a(rsExtraInfo.info.lPreSetlimit, psTagInfo->sPreSetLimit, sizeof(psTagInfo->sPreSetLimit));				// 4.0.15.500 - CR 19675
	psTagInfo->sPreSetLimitType = rsExtraInfo.info.sPreSetLimitType;

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;				
		str.Format("CGPI:QueryExtraInfoData exist with those parameters lPumpNumber=%ld, Discounts = %.6s, %.6s, %.6s, %.6s, %.6s, %.6s, %.6s, %.6s, %.6s, %.6s, %.6s, %.6s, %.6s, %.6s, %.6s, %.6s,",
					 lPumpNumber, psTagInfo->sDiscountArray.sDiscount[0], psTagInfo->sDiscountArray.sDiscount[1], psTagInfo->sDiscountArray.sDiscount[2], 
					 psTagInfo->sDiscountArray.sDiscount[3], psTagInfo->sDiscountArray.sDiscount[4], psTagInfo->sDiscountArray.sDiscount[5],
					 psTagInfo->sDiscountArray.sDiscount[6], psTagInfo->sDiscountArray.sDiscount[7], psTagInfo->sDiscountArray.sDiscount[8], 
					 psTagInfo->sDiscountArray.sDiscount[9], psTagInfo->sDiscountArray.sDiscount[10], psTagInfo->sDiscountArray.sDiscount[11], 
					 psTagInfo->sDiscountArray.sDiscount[12], psTagInfo->sDiscountArray.sDiscount[13], psTagInfo->sDiscountArray.sDiscount[14], 
					 psTagInfo->sDiscountArray.sDiscount[15]
					);								//4.0.22.504 73878
		_LOGMSG.LogMsg(lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str); 				
	}			
}


//4.0.23.500 TD 69914
void CCommonFunction::ConvertExtraInfoRecord2Tag(TAG_DISCOUNT_LOYALTY_INFO2* psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber, const long lPosNumber)
{
	ConvertExtraInfoRecord2Tag((TAG_DISCOUNT_LOYALTY_INFO*)psTagInfo, rsExtraInfo, lPumpNumber, lPosNumber);

	memcpy(psTagInfo->sCustomerName,	rsExtraInfo.info.sCustomerName,	sizeof(psTagInfo->sCustomerName));
	memcpy(psTagInfo->sTotalBalance,	rsExtraInfo.info.sTotalBalance,	sizeof(psTagInfo->sTotalBalance));				
	memcpy(psTagInfo->sHostTransId,		rsExtraInfo.info.sHostTransId,		sizeof(psTagInfo->sHostTransId)); 					

	Unpak((BYTE*)psTagInfo->sLAM, (BYTE*)rsExtraInfo.info.sLAM, sizeof(psTagInfo->sLAM));
	
	if ((rsExtraInfo.info.sLAMFlags == '1') ||(rsExtraInfo.info.sLAMFlags == '2'))
	{
		psTagInfo->sLAMFlags = rsExtraInfo.info.sLAMFlags;
		memset(psTagInfo->sLAM, ' ', sizeof(psTagInfo->sLAM));
	}
	else
		psTagInfo->sLAMFlags = '0';

	memcpy(&psTagInfo->sCarWashProgram,	&rsExtraInfo.info.sCarWashProgram,	sizeof(rsExtraInfo.info.sCarWashProgram));	//TD 411428
	memcpy(psTagInfo->sWashCode,	rsExtraInfo.info.sWashCode,	sizeof(rsExtraInfo.info.sWashCode));				//4.0.21.501 - TD 70634
	memcpy(psTagInfo->sWashExpire,	rsExtraInfo.info.sWashExpire,	sizeof(psTagInfo->sWashExpire));
	l2a(rsExtraInfo.info.lCarWashTax,		psTagInfo->sCarWashTax,			sizeof(psTagInfo->sCarWashTax));
	l2a(rsExtraInfo.info.lCarWashAmount*10,	psTagInfo->sCarWashAmount,		sizeof(psTagInfo->sCarWashAmount)); //TD 411428
	l2a(rsExtraInfo.info.lCarWashDiscount, psTagInfo->sCarWashDiscount,	sizeof(psTagInfo->sCarWashDiscount)); 
	
	if(LOG_FCC_FLOW_CONTROL)
	{
		CString str;				
		str.Format("CGPI:QueryExtraInfoData sCustomerName=%.20s, TotalBalace=%.8s, sHostTransId=%.8s, lWashCode=%.6s, Car wash amount=%ld , info.info.sOLDSession=%.6s",
			psTagInfo->sCustomerName, psTagInfo->sTotalBalance, psTagInfo->sHostTransId, rsExtraInfo.info.sWashCode, rsExtraInfo.info.lCarWashAmount, rsExtraInfo.info.sOLDSession); //marikdebug //4.0.23.220 105272 BP //4.0.24.60
		_LOGMSG.LogMsg(str); 				
					
		str.Format("CGPI:QueryExtraInfoData LAM::[%.50s], flags=%c", 
			psTagInfo->sLAM, psTagInfo->sLAMFlags);
		_LOGMSG.LogMsg(lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str); 				
	}	
}


//4.0.23.500 TD 69914
void CCommonFunction::ConvertExtraInfoRecord2Tag(TAG_DISCOUNT_LOYALTY_INFO3* psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber, const long lPosNumber)
{
	ConvertExtraInfoRecord2Tag((TAG_DISCOUNT_LOYALTY_INFO2*)psTagInfo, rsExtraInfo, lPumpNumber, lPosNumber);

	long lNonDiscountTotalAmount = a2l((BYTE*)rsExtraInfo.info.sNonDiscountTotalAmount, sizeof(rsExtraInfo.info.sNonDiscountTotalAmount));					//66871 4.0.20.141
	long lTotalDiscount = lNonDiscountTotalAmount - rsExtraInfo.info.lTransactionAmount;	

	l2a(lTotalDiscount, psTagInfo->sTotalDiscount, sizeof(psTagInfo->sTotalDiscount));
	memcpy(psTagInfo->sOriginalUnitPrice, rsExtraInfo.info.sOriginalUnitPrice, sizeof(psTagInfo->sOriginalUnitPrice));

	memcpy(&psTagInfo->discountAttrsInfo, &rsExtraInfo.info.discountAttrsInfo, sizeof(psTagInfo->discountAttrsInfo));
	memcpy(&psTagInfo->sDeferralAccepted, &rsExtraInfo.info.sDeferralAccepted, sizeof(psTagInfo->sDeferralAccepted));						//4.0.22.504 69344
	memcpy(&psTagInfo->sDiscountAttrExt, &rsExtraInfo.info.sDiscountAttrExt, sizeof(psTagInfo->sDiscountAttrExt));							//4.0.22.509 38162
	memcpy(&psTagInfo->sOLDSession, &rsExtraInfo.info.sOLDSession,sizeof(psTagInfo->sOLDSession));  //marikDebug //4.0.23.220 105272 BP //4.0.24.60
}

//4.0.23.504 66705
void CCommonFunction::ConvertExtraInfoRecord2Tag(TAG_DISCOUNT_LOYALTY_INFO4* psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber, const long lPosNumber)
{
	ConvertExtraInfoRecord2Tag((TAG_DISCOUNT_LOYALTY_INFO3*)psTagInfo, rsExtraInfo, lPumpNumber, lPosNumber);
	ConvertExtraInfoRecord2Tag((TAG_ANPR_INFO_PLUS*)&psTagInfo->cANPRInfo, rsExtraInfo, lPumpNumber); 

	PS::RECEIPT_EXTRA_INFO_REC cReceiptExtraRec;
	memset(&cReceiptExtraRec, 0, sizeof(PS::RECEIPT_EXTRA_INFO_REC));
	cReceiptExtraRec.info.lIndexNumber = rsExtraInfo.info.lSeqNumber;  //4.0.19.800 trs.m_lLinkNumber;
	memset(psTagInfo->cReceiptExtraInfo.sFiler2043, ' ', sizeof(psTagInfo->cReceiptExtraInfo.sFiler2043));

	long lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, cReceiptExtraRec);

	// Fail to read receipt extra data
	if (lRtc)
	{
		memset(cReceiptExtraRec.info.sFiler2043, ' ', sizeof(cReceiptExtraRec.info.sFiler2043));
	}
	else
	{
		memcpy((char *)psTagInfo->cReceiptExtraInfo.sFiler2043, (char *)cReceiptExtraRec.info.sFiler2043, sizeof(psTagInfo->cReceiptExtraInfo.sFiler2043));
	}

	//4.0.23.509 97360
	memcpy(psTagInfo->sPostDispensingVolumeHigh,	rsExtraInfo.info.sPostDispensingVolumeHigh, sizeof(psTagInfo->sPostDispensingVolumeHigh));
	memcpy(psTagInfo->sPostDispensingVolumeLow,		rsExtraInfo.info.sPostDispensingVolumeLow,	sizeof(psTagInfo->sPostDispensingVolumeLow));
	memcpy(psTagInfo->sPreDispensingVolumeHigh,		rsExtraInfo.info.sPreDispensingVolumeHigh,	sizeof(psTagInfo->sPreDispensingVolumeHigh));
	memcpy(psTagInfo->sPreDispensingVolumeLow,		rsExtraInfo.info.sPreDispensingVolumeLow,	sizeof(psTagInfo->sPreDispensingVolumeLow));
	memcpy(psTagInfo->sLockingKey,					rsExtraInfo.info.sLockingKey,				sizeof(psTagInfo->sLockingKey)); // CR 425758

	memcpy(psTagInfo->sStoreIDField,				rsExtraInfo.info.sStoreIDField,				sizeof(psTagInfo->sStoreIDField));
	memcpy(psTagInfo->sPosId,						rsExtraInfo.info.sPosId,					sizeof(psTagInfo->sPosId));
	memcpy(psTagInfo->sTransId,						rsExtraInfo.info.sTransId,					sizeof(psTagInfo->sTransId));
	memcpy(&psTagInfo->sLoyaltyStartDateTime,		&rsExtraInfo.info.sLoyaltyStartDateTime,	sizeof(psTagInfo->sLoyaltyStartDateTime));
	memcpy(psTagInfo->sTicketId,					rsExtraInfo.info.sTicketId,					sizeof(psTagInfo->sTicketId));

	memcpy(psTagInfo->sClubType,					rsExtraInfo.info.sClubType,					sizeof(psTagInfo->sClubType));  // CR 501164
	memcpy(psTagInfo->sPromotionID,					rsExtraInfo.info.sPromotionID,				sizeof(psTagInfo->sPromotionID));  // RFUEL-155
	//RFUEL-3309 Get the maximal discount allowed for the grade 
	memcpy(psTagInfo->cDiscountLoyaltyInfo3.sMaxDiscountAllowed, rsExtraInfo.info.sMaxDiscountAllowed, sizeof(psTagInfo->cDiscountLoyaltyInfo3.sMaxDiscountAllowed));

	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("CCommonFunction::ConvertExtraInfoRecord2Tag() - sClubType = %.2s, ClubType = %.2s, sPromotionID = %.15s", 
			psTagInfo->sClubType, 
			rsExtraInfo.info.sClubType, 
			rsExtraInfo.info.sPromotionID);
		_LOGMSG.LogMsg(lPosNumber, LOG_POS, lPumpNumber, LOG_PUMP, str);
	}
}

//4.0.23.500 TD 69914
void CCommonFunction::ConvertExtraInfoRecord2Tag(TAG_ANPR_INFO_PLUS* psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber)
{
	memcpy(&psTagInfo->sANPRRefNumber, &rsExtraInfo.info.sANPRRefNumber, sizeof(psTagInfo->sANPRRefNumber));
	memcpy(&psTagInfo->sFullVehicleId, &rsExtraInfo.info.sFullVehicleId, sizeof(psTagInfo->sFullVehicleId));
	memcpy(&psTagInfo->cVRNDateTime, &rsExtraInfo.info.cVRNDateTime, sizeof(psTagInfo->cVRNDateTime));							//4.0.26.505 157073
	memcpy(&psTagInfo->sANPRTrsStatus, &rsExtraInfo.info.sANPRTrsStatus, sizeof(psTagInfo->sANPRTrsStatus));					//4.0.26.505 157073
	memcpy(&psTagInfo->sTrsFlowRate, &rsExtraInfo.info.sTrsFlowRate, sizeof(psTagInfo->sTrsFlowRate));							//4.0.27.40 TD 165585
	memcpy(&psTagInfo->sAverageFlowRate, &rsExtraInfo.info.sAverageFlowRate, sizeof(psTagInfo->sAverageFlowRate));				//4.0.27.40 TD 165585
	psTagInfo->sRatesFromDevice = rsExtraInfo.info.sRatesFromDevice;															//4.0.27.40 TD 165585				
}

/******************************************************************************
 Description:	Function Convert From TagOlaRec12 To PAPInfo /4.0.27.47

 Returns:     	
 Parameters:	pTagOlaRecIn  -> source
				pPAPInfoOut -> destination
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		12/04/2010   13:50		Start 4.0.24.71 No TD  
******************************************************************************/

void CCommonFunction::ConvertFromTagOlaRecToPAPInfo( PAY_AT_PUMP_INFO* pPAPInfoOut, const TAG_OLA_REC12* pTagOlaRecIn )
{
	if ( pPAPInfoOut != NULL )
	{
		memset(pPAPInfoOut, ' ', sizeof(PAY_AT_PUMP_INFO));

		pPAPInfoOut->CardSaleInfo.cardData	= pTagOlaRecIn->tagOlaRec11.tagOlaRec10.cardData;
		pPAPInfoOut->CardSaleInfo.extraData	= pTagOlaRecIn->tagOlaRec11.tagOlaRec10.extraData;
		pPAPInfoOut->CardSaleExtraData2		= pTagOlaRecIn->tagOlaRec11.tagOlaRec10.extraData2;
		pPAPInfoOut->CardSaleExtraData3		= pTagOlaRecIn->tagOlaRec11.tagOlaRec10.extraData3;
		pPAPInfoOut->CardSaleExtraData4		= pTagOlaRecIn->tagOlaRec11.tagOlaRec10.extraData4; 	
		pPAPInfoOut->CardSaleExtraData5		= pTagOlaRecIn->extraData5; //4.0.27.47
	}
}


/******************************************************************************
 Description:	Function Convert From TagOlaRec12 To Items //4.0.27.47

 Returns:     	0	-> OK
				-1	-> Fail
 Parameters:	pTagOlaRecIn  -> source
				pItemsInfoOut -> destination
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		12/04/2010   13:50		Start 4.0.24.71 No TD
******************************************************************************/

long CCommonFunction::ConvertFromTagOlaRecToItems( ITEMS_INFO* pItemsInfoOut, const TAG_OLA_REC12* pTagOlaRecIn )
{
	long	lRetCode = -1;	

	long lItemsNumber = a2l( (unsigned char*)pTagOlaRecIn->tagOlaRec11.tagOlaRec10.Items.sItemsNumber, sizeof(pTagOlaRecIn->tagOlaRec11.tagOlaRec10.Items.sItemsNumber) );

	if (lItemsNumber > 0)
	{	
		BAP_ALL_DATA_STRUCTS	cItemRec; 

		memset(&cItemRec , ' ' , sizeof(cItemRec));
		memset(pItemsInfoOut , ' ' , sizeof(ITEMS_INFO));

		long	lItemIndex = _Module.m_server.NewItemsIndex();
		l2a(lItemIndex, (unsigned char*)pTagOlaRecIn->tagOlaRec11.tagOlaRec10.extraData2.sItemsLinkIndex, sizeof(pTagOlaRecIn->tagOlaRec11.tagOlaRec10.extraData2.sItemsLinkIndex));					

		cItemRec.cBAPReceiptItem	= pTagOlaRecIn->tagOlaRec11.tagOlaRec10.Items;
		cItemRec.cBAPReceiptItem2	= pTagOlaRecIn->tagOlaRec11.tagOlaRec10.Items2;

		CItemsFile				cItemsFile;
		cItemsFile.SplitBAPRecorToItems(lItemIndex, cItemRec, *pItemsInfoOut);	
		
		lRetCode = OK;
	}

	return lRetCode;
}


/******************************************************************************
 Description:	Function Convert From TagOlaRec12 To Zones //4.0.27.47

 Returns:     	0	-> OK
				-1	-> Fail

 Parameters:	pTagOlaRecIn  -> source
				pZonesInfoOut -> destination
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		12/04/2010   13:50		Start 4.0.24.71 No TD
******************************************************************************/
long CCommonFunction::ConvertFromTagOlaRecToZones(PS::RECEIPT_EXTRA_INFO_REC* pZonesInfoOut, const TAG_OLA_REC12* pTagOlaRecIn, long lTrsNumber)
{
	long lRetCode = -1;

	if (!ChrAll((char *)pTagOlaRecIn->tagOlaRec11.tagOlaRec10.ReceiptExtraInfo.sFiler2043, sizeof(pTagOlaRecIn->tagOlaRec11.tagOlaRec10.ReceiptExtraInfo.sFiler2043)))
	{
		memset(pZonesInfoOut, ' ', sizeof(PS::RECEIPT_EXTRA_INFO_REC));

		pZonesInfoOut->info.lIndexNumber = lTrsNumber;
		memcpy(pZonesInfoOut->info.sFiler2043, pTagOlaRecIn->tagOlaRec11.tagOlaRec10.ReceiptExtraInfo.sFiler2043, sizeof(pZonesInfoOut->info.sFiler2043));

		lRetCode = OK;
	}

	return lRetCode;
}


/******************************************************************************
 Description:	Function Convert From TagOlaRec12 To TAG_DISCOUNT_LOYALTY_INFO4 //4.0.27.47

 Returns:     	none

 Parameters:	pTagOlaRecIn  -> source
				pTagDiscountLoyaltyInfoOut -> destination
				lPumpNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		12/04/2010   13:50		Start 4.0.24.71 No TD
******************************************************************************/
void CCommonFunction::ConvertFromTagOlaRecToTagDiscountLoyalty(TAG_DISCOUNT_LOYALTY_INFO4* pTagDiscountLoyaltyInfoOut, const TAG_OLA_REC13* pTagOlaRecIn, long lPumpNumber )
{
	memcpy( pTagDiscountLoyaltyInfoOut->cDiscountLoyaltyInfo3.sPreSetLimit,			(BYTE*) pTagOlaRecIn->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData3.sPreSetlimit,			sizeof(pTagOlaRecIn->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData3.sPreSetlimit) );
	memcpy( &pTagDiscountLoyaltyInfoOut->cDiscountLoyaltyInfo3.sDiscountType[1],	(BYTE*) &pTagOlaRecIn->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sGRDiscountType,		sizeof(pTagOlaRecIn->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sGRDiscountType) );			//4.0.24.76
	memcpy( pTagDiscountLoyaltyInfoOut->cDiscountLoyaltyInfo3.sPromotionNumber,		(BYTE*) pTagOlaRecIn->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sPromotionNumber,		sizeof(pTagOlaRecIn->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sPromotionNumber) );
	memcpy( pTagDiscountLoyaltyInfoOut->cDiscountLoyaltyInfo3.sLoyalId,				(BYTE*) pTagOlaRecIn->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData.sLoyalId,				sizeof(pTagOlaRecIn->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData.sLoyalId) );
	
	// RFUEL-155
	memcpy(pTagDiscountLoyaltyInfoOut->sPromotionID,								(BYTE*)pTagOlaRecIn->extraData6.sPromotionID,												sizeof(pTagOlaRecIn->extraData6.sPromotionID));

	// in case we support nozzle restriction convert Grade discount to nozzle discount
	if(_Module.m_server.m_bControllerSupportNozzleRestrction)
	{
		CString strLogMsg("ConvertFromTagOlaRecToTagDiscountLoyalty, Work with position");

		TAG_DISCOUNT_MAP	sDiscountPerGrade;

		ConvertDiscountArrayToDiscountMap( &sDiscountPerGrade, pTagOlaRecIn->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2.sDiscountPerGrade );
		ConvertGradeDiscountToNozzleDiscount( lPumpNumber, &(pTagDiscountLoyaltyInfoOut->cDiscountLoyaltyInfo3.sDiscountArray), &sDiscountPerGrade );

		strLogMsg.Format("ConvertFromTagOlaRecToTagDiscountLoyalty, Saved discounts Array 16X6 = [%.96s]", pTagDiscountLoyaltyInfoOut->cDiscountLoyaltyInfo3.sDiscountArray.sDiscount); 
		_LOGMSG.LogMsg(strLogMsg);
	}		

	memcpy( pTagDiscountLoyaltyInfoOut->cDiscountLoyaltyInfo3.sDiscountArray.sLoyaltyVolumeLimit,	(BYTE*) pTagOlaRecIn->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData4.sLoyaltyVolumeLimit,	sizeof(pTagOlaRecIn->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData4.sLoyaltyVolumeLimit) );	//4.0.24.91

	//4.0.24.140 127928 //4.0.124.251
	//start arranging grades limits according to positions
	ArrangeGradesLimitsByPosition(lPumpNumber, pTagDiscountLoyaltyInfoOut->GradeAmountLimit);

}


void CCommonFunction::ConvertDiscountArrayToDiscountMap( TAG_DISCOUNT_MAP* pDiscountMapOut, const DISCNT* pDiscountArrIn )
{
	memset( pDiscountMapOut, ' ', sizeof(TAG_DISCOUNT_MAP) );
	CString strLogMsg;
	for (int i = 0; i < MAX_GRADES; i++)  //RFUEL-3443 SN - On prepay pump 6 coupon discount are not updated on fuel
	{									  //discounts on grades beyond 6 were not applied (source of bug: Emerald) 
		memcpy( pDiscountMapOut->sDiscount[i], &pDiscountArrIn[i], min( sizeof(pDiscountMapOut->sDiscount[i]), sizeof(pDiscountArrIn[i]) ) );
#ifdef DEBUG
		strLogMsg.Format("i is [%ld]", i );
		_LOGMSG.LogMsg(strLogMsg);
#endif
	}
}

//4.0.24.140 127928 //4.0.124.251
void CCommonFunction::ArrangeGradesLimitsByPosition( long lPumpNumber, GRADE_LIMITS* pGradeLimitsArray )
{
	GRADE_LIMITS		sTmpGradeLimitsArray[MAX_POSITIONS_PER_PUMP]; 
	BYTE				byNozzle = 0;
	BOOL				bLimitExists = FALSE;
	memset (sTmpGradeLimitsArray, ' ', sizeof(sTmpGradeLimitsArray));
	if (!pGradeLimitsArray)
		return;

	for (int i = 0; i < MAX_POSITIONS_PER_PUMP; i++)
	{
		long lPosition;
		if (pGradeLimitsArray[i].sGradeID > 0)
		{
			short iGradeID = (short)a2l(pGradeLimitsArray[i].sGradeID, sizeof(pGradeLimitsArray[i].sGradeID));
			byNozzle = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetGradePosition(iGradeID, &lPosition);
			if ((byNozzle > 0) && (byNozzle < MAX_POSITIONS_PER_PUMP))
			{
				bLimitExists = TRUE;
				memcpy(sTmpGradeLimitsArray[byNozzle - 1].sGradeID, pGradeLimitsArray[i].sGradeID, sizeof(sTmpGradeLimitsArray[byNozzle - 1].sGradeID));
				memcpy(sTmpGradeLimitsArray[byNozzle - 1].sGradeAmountLimit, pGradeLimitsArray[i].sGradeAmountLimit, sizeof(sTmpGradeLimitsArray[byNozzle - 1].sGradeAmountLimit));
				memcpy(&sTmpGradeLimitsArray[byNozzle - 1].sLimitType, &pGradeLimitsArray[i].sLimitType, sizeof(sTmpGradeLimitsArray[byNozzle - 1].sLimitType));
				CString str;
				str.Format("Found grade limit GradeID = %.02s, AmountLimit = %.09s, LimitType = %c", sTmpGradeLimitsArray[byNozzle - 1].sGradeID,
																									 sTmpGradeLimitsArray[byNozzle - 1].sGradeAmountLimit,
																									 sTmpGradeLimitsArray[byNozzle - 1].sLimitType);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
			}

		}

	}
	if (bLimitExists)
		memcpy(pGradeLimitsArray, sTmpGradeLimitsArray, sizeof(pGradeLimitsArray));

}

/******************************************************************************
 Description:	Function Convert From GradeDiscount array To NozzleDiscount array

 Returns:     	none

 Parameters:	pGradeDiscountIn  -> source
				pNozzleDiscountOut -> destination
				lPumpNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		12/04/2010   13:50		Start 4.0.24.71 No TD
******************************************************************************/
void CCommonFunction::ConvertGradeDiscountToNozzleDiscount( long lPumpNumber, TAG_DISCOUNT_MAP* pNozzleDiscountOut, const TAG_DISCOUNT_MAP* pGradeDiscountIn)
{
	long				lNumOfPosition = 0;
	long				lDiscount=0;
	CString				strLogMsg;
		
	memset( pNozzleDiscountOut, ' ', sizeof(TAG_DISCOUNT_MAP) ); 
	
	for (int i = 1; i <= MAX_GRADES; i++)
	{
		lDiscount = a2l( (BYTE*)pGradeDiscountIn->sDiscount[i - 1], sizeof(pGradeDiscountIn->sDiscount[i - 1]) ); 
		lNumOfPosition = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetNumOfPositionForGrade( (short) i );		//The number of positions holding the same grade.
#ifdef DEBUG
		CString str;
		str.Format("ConvertGradeDiscountToNozzleDiscount:  i = %ld  lDiscount = %ld, lNumOfPosition = %ld , lPumpNumber = %ld   ", i, lDiscount, lNumOfPosition, lPumpNumber);
		_LOGMSG.LogMsg(1, LOG_PUMP, str);
#endif
		if( (lNumOfPosition > 0) && (lDiscount != 0) )
		{						
			if(LOG_BASIC_FLOW_CONTROL)
			{			
				strLogMsg.Format("ConvertGradeDiscountToNozzleDiscount, Prepay discount is for grade number %ld discount = %ld", i, lDiscount);
				_LOGMSG.LogMsg(strLogMsg);
			}

			long lLastIndex = 0;

			for(int count = 0; count < lNumOfPosition; count++)
			{
				BYTE byNozzle = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetGradePosition(i, &lLastIndex);

				if (byNozzle)
				{	
					if(LOG_BASIC_FLOW_CONTROL)
					{
						strLogMsg.Format( "ConvertGradeDiscountToNozzleDiscount, Set discount %ld to nozzle %ld", lDiscount, byNozzle );
						_LOGMSG.LogMsg( strLogMsg );
					}

					memcpy( pNozzleDiscountOut->sDiscount[byNozzle - 1], pGradeDiscountIn->sDiscount[i - 1], sizeof(pNozzleDiscountOut->sDiscount[byNozzle - 1]) );
				}
			}

			lDiscount = 0;
		}			
	}

	// memcpy( pNozzleDiscountOut, pGradeDiscountIn, sizeof(pNozzleDiscountOut) ); // TD 264119  4.0.28.502
}


/******************************************************************************
 Description:	Function Update OLA Flags

 Returns:     	none

 Parameters:	pPAPInfoInOut  -> source
				lNewFlag -> destination				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		12/04/2010   13:50		Start 4.0.24.71 No TD
******************************************************************************/
void CCommonFunction::UpdateOLAFlags(PAY_AT_PUMP_INFO* pPAPInfoInOut, long lNewFlags)
{
	if (pPAPInfoInOut != NULL)
	{
		if ( !_Module.m_server.GetOLAFlags(pPAPInfoInOut->CardSaleExtraData2.sFlags, lNewFlags) )
		{			
			long lUpdatedFlags = a2l( pPAPInfoInOut->CardSaleExtraData2.sFlags, sizeof(pPAPInfoInOut->CardSaleExtraData2.sFlags) );
			lUpdatedFlags += lNewFlags;
			l2a( lUpdatedFlags, pPAPInfoInOut->CardSaleExtraData2.sFlags, sizeof(pPAPInfoInOut->CardSaleExtraData2.sFlags) );
		}
	}
}


/******************************************************************************
 Description:	Function Calculate Loyalty Volume Limit

 Returns:     	none

 Parameters:	pTagDiscountLoyaltyInfoInOut
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		12/04/2010   13:50		Start 4.0.24.71 No TD
******************************************************************************/
void CCommonFunction::CalcLoyaltyVolumeLimit(TAG_DISCOUNT_LOYALTY_INFO4* pTagDiscountLoyaltyInfoInOut)
{
	if (pTagDiscountLoyaltyInfoInOut != NULL)
	{	
		BYTE*	pDefaultLoyaltyVolumeLimit = pTagDiscountLoyaltyInfoInOut->cDiscountLoyaltyInfo3.sLoyaltyVolumeLimit;
		size_t	nDefaultLoyaltyVolumeLimitSize = sizeof( pTagDiscountLoyaltyInfoInOut->cDiscountLoyaltyInfo3.sLoyaltyVolumeLimit );

		BYTE*	pDiscountLoyaltyVolumeLimit = pTagDiscountLoyaltyInfoInOut->cDiscountLoyaltyInfo3.sDiscountArray.sLoyaltyVolumeLimit;
		size_t	nDiscountLoyaltyVolumeLimitSize = sizeof( pTagDiscountLoyaltyInfoInOut->cDiscountLoyaltyInfo3.sDiscountArray.sLoyaltyVolumeLimit );

		long		lValue = 0;
		const long	lDefaultLoyaltyVolumeLimit = a2l( pDefaultLoyaltyVolumeLimit, nDefaultLoyaltyVolumeLimitSize ); 

		lValue = (lDefaultLoyaltyVolumeLimit == 0) ? a2l(pDiscountLoyaltyVolumeLimit, nDiscountLoyaltyVolumeLimitSize) : RoundDouble(lDefaultLoyaltyVolumeLimit, 1000);
		
		l2a( lValue, pDefaultLoyaltyVolumeLimit, nDefaultLoyaltyVolumeLimitSize ); 

		CString strLogMsg;			
		strLogMsg.Format("CalcLoyaltyVolumeLimit, Calculated GallonLimit=%ld, Default GallonLimit=%ld.", lValue, lDefaultLoyaltyVolumeLimit); 
		_LOGMSG.LogMsg(strLogMsg);
	}			
}


/******************************************************************************
 Description:	Function Set Lowes Between OLA and Loyalty Limits

 Returns:     	none

 Parameters:	pLoyaltyInfoInOut  
				pPrePayInfoInOut				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		12/04/2010   13:50		Start 4.0.24.71 No TD
******************************************************************************/
void CCommonFunction::SetLowesBetweenOLAandLoyaltyLimits( long lPumpNumber, TAG_DISCOUNT_LOYALTY_INFO4* pLoyaltyInfoInOut, PRE_PAY_INFO* pPrePayInfoInOut )
{
	long lLoyaltyLimit = a2l((BYTE*)pLoyaltyInfoInOut->cDiscountLoyaltyInfo3.sLoyaltyVolumeLimit, sizeof(pLoyaltyInfoInOut->cDiscountLoyaltyInfo3.sLoyaltyVolumeLimit));
	
	if (lLoyaltyLimit != 0)
	{
		CString strLogMsg;

		if (pPrePayInfoInOut->lPreSetLimit > lLoyaltyLimit)
		{
			pPrePayInfoInOut->lPreSetLimit = lLoyaltyLimit;			
				
			strLogMsg.Format("SetLowesBetweenOLAandLoyaltyLimits, Preset volume limit is taken from loyalty volume limit %ld", lLoyaltyLimit);												
		}
		else	//loyalty volume limit is higher than ola volume limit so we can ignore it
		{
			lLoyaltyLimit = 0;
			l2a(lLoyaltyLimit, pLoyaltyInfoInOut->cDiscountLoyaltyInfo3.sLoyaltyVolumeLimit, sizeof(pLoyaltyInfoInOut->cDiscountLoyaltyInfo3.sLoyaltyVolumeLimit));

			strLogMsg.Format("SetLowesBetweenOLAandLoyaltyLimits, Loyalty volume limit %ld is higher than preset volume limit %ld. Loyalty limit will be ignored", lLoyaltyLimit, pPrePayInfoInOut->lPreSetLimit);																
		}

		if(LOG_BASIC_FLOW_CONTROL)
		{
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strLogMsg);
		}
	}
}


/******************************************************************************
 Description:	Function Generate Link Flags for PrePay Transaction by General authorise and limit flags

 Returns:     	none

 Parameters:	lFlags				- General authorise and limit flags
				lTrsLinkFlagsOut	- generated Link Flags (Out)
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		15/04/2010   14:50		Start 4.0.24.73 No TD
******************************************************************************/
void CCommonFunction::GeneratePrePayTrsLinkFlags( const long lFlags, short* lTrsLinkFlagsOut )
{	
	*lTrsLinkFlagsOut = TRS_LINK_PREPAY_NUMBER;

	if(lFlags & PREPAY_WITH_XML_BUFF)
	{	
		*lTrsLinkFlagsOut |= TRS_LINK_XML_BUFF; 
	}
	else if (lFlags & LOYALTY_DISCOUNT_DATA)   
	{
		*lTrsLinkFlagsOut |= TRS_LINK_PREPAY_NUMBER_AND_LOYALTY;
	}
	else if (lFlags & PREPAY_WITH_XML_BUFF_WITH_LOYALTY_DISCOUNT_DATA)		
	{				
		*lTrsLinkFlagsOut |= (TRS_LINK_XML_BUFF | TRS_LINK_PREPAY_NUMBER_AND_LOYALTY); 	   
	}
	else if (lFlags & LIMIT_FLEET) 
	{
		*lTrsLinkFlagsOut |= (TRS_LINK_EXTRA_INFO | TRS_LINK_FLEET);
	}
}


/******************************************************************************
 Description:  	Execute File 
 Returns:      	
 Parameters:   char* szFilePath, char* szFileAction, char* szExtraParameters, long lConsolShow = SW_HIDE	 
               	
 WHO                   WHEN                    WHAT
-------------------------------------------------------------------------------
 Shahaf				12/5/2010					Start
 Gena				26/08/2010				Merge from FCC 1025	
******************************************************************************/

DWORD CCommonFunction::ExecuteFile (char* szFilePath, char* szFileAction, char* szExtraParameters, long lConsolShow)
{
	DWORD				dwRetVal = (DWORD)RetCode_Success;
	SHELLEXECUTEINFO	sShellExecuteInfo = {0}; 

	sShellExecuteInfo.cbSize = sizeof( sShellExecuteInfo ); 
	sShellExecuteInfo.lpVerb = szFileAction; 
	sShellExecuteInfo.lpFile = szFilePath; 	
	sShellExecuteInfo.nShow	= lConsolShow; 
	sShellExecuteInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	sShellExecuteInfo.lpParameters = szExtraParameters; 

	try
	{
		ShellExecuteEx( &sShellExecuteInfo );   // you should check for an error here 

		while( TRUE ) 
		{ 
			DWORD nStatus = MsgWaitForMultipleObjects( 1, &sShellExecuteInfo.hProcess, FALSE, INFINITE, QS_ALLINPUT );  // drop through on user activity  

			if ( nStatus == WAIT_OBJECT_0 ) 
			{  // done: the program has ended 
				break; 
			} 
			
			MSG msg;     // else process some messages while waiting... 
			
			while( PeekMessage(&msg,NULL,0,0,PM_REMOVE) )
			{ 
				DispatchMessage( &msg ); 
			} 
		}  // launched process has exited 

		GetExitCodeProcess( sShellExecuteInfo.hProcess, &dwRetVal );  // ERRORLEVEL value
	}
	catch (...)
	{
		dwRetVal = RetCode_Critical_Failure;
	}

	return dwRetVal;
}


/******************************************************************************
 Description:	Find ReceiptIndex in m_cMemPumpArray by TrsNumber

 Returns:     	ReceiptIndex length

 Parameters:	

 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		17/10/2010   19:52		Start 4.0.26.500 TD TD 144219
******************************************************************************/
long CCommonFunction::LookForInternalReceiptIndex(char* psIndexOut, const long lTrsNumber)
{
	BYTE	szTrsNumber[STR_LONG_VAL5 + 1] = {0};
	long	lRetIndexLen = 0;

	l2a(lTrsNumber, szTrsNumber, sizeof(szTrsNumber) - 1 );	

	for ( int i = 0; i < _Module.m_server.m_cMemPumpArray.GetSize() && lRetIndexLen == 0; i++)
	{	
		lRetIndexLen = _Module.m_server.m_cMemPumpArray[i].LookForInternalReceiptIndex(psIndexOut, lTrsNumber); // TD 300280
	}

	return lRetIndexLen;			
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
CtrlRetCode CCommonFunction::GetRollingUpdateInfo(long lPumpNumber, long lFlags, BSTR *pbstrData)
{
	CtrlRetCode lRetCode = CTRL_OK;

	CXMLInterface cXmlInterface;
	CXMLPointers cXmlRollingInfoElement, cXmlMetaDataElement, cXmlChildElement, cXmlPumpsElement, cXmlPumpElement;
	SYSTEMTIME cSysTime;
	char szStr[STR_20] = {0};
	long lLastValidPump = _Module.m_server.GetLastValidPump();
	BOOL bRollingFound = FALSE;

	//Check if pump is dispensing
	if (!lPumpNumber)
	{
		for (int p = 0; p < lLastValidPump && (!bRollingFound); p++)
		{
			if (_Module.m_server.m_cPumpArray[p].GetStat().GetStatus() == DISPENSING)
				bRollingFound = TRUE;
		}
	}
	else if ((lPumpNumber > 0) && (lPumpNumber <= lLastValidPump))
	{
		if (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetStatus() == DISPENSING)
			bRollingFound = TRUE;
	}
	else
		lRetCode = CTRL_INVALID_PUMP_NUMBER;
	

	if (!lRetCode && bRollingFound)
	{
		// Fill current date and time
		GetLocalTime(&cSysTime); 

		if(cXmlInterface.InitDoc() == TRUE)
		{
			// Create XML root
			cXmlInterface.CreateElement("RollingUpdateInfo",&cXmlRollingInfoElement);
			cXmlInterface.AppendRootElement(&cXmlRollingInfoElement);

			// Create XML MetaData
			cXmlInterface.CreateElement("MetaData", &cXmlMetaDataElement) ;
			cXmlInterface.AppendChild(&cXmlMetaDataElement, &cXmlRollingInfoElement);

			// Create RequestDate attribute
			sprintf(szStr, "%02d/%02d/%04d",cSysTime.wDay, cSysTime.wMonth, cSysTime.wYear);
			cXmlInterface.SetCharAttribute("RequestDate", szStr, &cXmlMetaDataElement);
			
			// Create RequestTime attribute
			sprintf(szStr, "%02d:%02d:%02d",cSysTime.wHour, cSysTime.wMinute, cSysTime.wSecond);
			cXmlInterface.SetCharAttribute("RequestTime", szStr, &cXmlMetaDataElement);
			
			// Create Pumps node
			cXmlInterface.CreateElement("Pumps", &cXmlPumpsElement) ;
			cXmlInterface.AppendChild(&cXmlPumpsElement, &cXmlRollingInfoElement);

			if (lPumpNumber == 0)
			{
				//Info required for all pumps
				for (int p = 1; p <= lLastValidPump; p++)
				{
					if (_Module.m_server.m_cPumpArray[p-1].GetStat().GetStatus() == DISPENSING)
						CreateRollingUpdatePumpInfo(p, &cXmlInterface, &cXmlPumpsElement);
				}
			}
			else
			{
				//Info required for a specific pump
				CreateRollingUpdatePumpInfo(lPumpNumber, &cXmlInterface, &cXmlPumpsElement);
			}

			if (LOG_FCC_FLOW_CONTROL)
				cXmlInterface.SaveToFile("C:\\Office\\PumpSrv\\Log\\Rolling.xml");
			
			cXmlInterface.GetXmlString(pbstrData);
			
			cXmlInterface.CloseDoc();
		}
	}
	else
		lRetCode = CTRL_RECORD_NOT_FOUND;


	return lRetCode;
}


/******************************************************************************
 Description:	Build rolling update xml info per pump

 Returns:      	None
 Parameters:	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		04/10/2010   16:00			Start		4.0.26.501 - TD 108311
*******************************************************************************/
void CCommonFunction::CreateRollingUpdatePumpInfo(long lPumpNumber, CXMLInterface *pXmlInterface, CXMLPointers *pXmlPumpsElement)
{
	CXMLPointers cXmlPumpElement, cXmlNozzleElement, cXmlTempElement;

	if (pXmlInterface != NULL && pXmlPumpsElement != NULL)
	{
		char szStr[STR_20] = {0};

		pXmlInterface->CreateElement("Pump", &cXmlPumpElement) ;
		pXmlInterface->AppendChild(&cXmlPumpElement, pXmlPumpsElement);

		// Create pump attributes
		pXmlInterface->SetLongAttribute("Number",lPumpNumber, &cXmlPumpElement);
		
		long lShiftNumber = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetShiftNumber();
		pXmlInterface->SetLongAttribute("ShiftNumber", lShiftNumber, &cXmlPumpElement);
		
		pXmlInterface->CreateElement("Nozzle", &cXmlNozzleElement) ;
		pXmlInterface->AppendChild(&cXmlNozzleElement, &cXmlPumpElement);


		CPumpTransact cTrs;
		for (int iTrs=0; (iTrs < MAX_TRS_IN_LIST) && (cTrs.m_lStatus != IN_PROGRESS); iTrs++)
		{		
			_Module.m_server.m_cPumpArray[lPumpNumber-1].GetTrsByIndex(iTrs, &cTrs);
			if (cTrs.m_lStatus == IN_PROGRESS)
			{
				short nPosition = (short)_Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetPosition();
				if (!nPosition && cTrs.m_nGrade)
					nPosition = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetGradePosition(cTrs.m_nGrade); 

				pXmlInterface->SetLongAttribute("Number", nPosition, &cXmlNozzleElement);
				pXmlInterface->SetLongAttribute("TrsNumber", cTrs.m_lNumber, &cXmlNozzleElement);
				pXmlInterface->SetLongAttribute("CurrentVolume", cTrs.m_lRoundedVolume, &cXmlNozzleElement);

			}
		}
	}
}

/******************************************************************************
 Description:	Check if 999 grade is turn on and update the grade id to the extended grade id

 Returns:     None 	
 Parameters:  long lPump		- Pump number 
              char *sGrade		- the grade id, will be updated to the extended id
			  long lSizeOfGrade - size of grade id
			  long lFlags		- flags recived in GetPAKQuery 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shuki Y		27/06/2011   17:30		Start 4.0.28.500 - TD 224805
******************************************************************************/
void CCommonFunction::ConvertToExtendedGradeForPAKQuery(const long lPump, unsigned char *sGrade, const long lSizeOfGrade, const long lFlags)
{
	long lExtendedGradeId = 0;
	
	if (_Module.m_server.GetMaxGrade999() && !(lFlags & PAK_QUERY_IGNORE_EXTENDED_GRADE_ID_CONVERSION))
	{
		if(LOG_BASIC_FLOW_CONTROL)
			_LOGMSG.LogClassMsg("CCommonFunction","ConvertToExtendedGrade",lPump,LOG_PUMP,"");
		
		_Module.m_server.m_cExtendedGradeMap.GetExtGradeByGrade(sGrade, lSizeOfGrade, lExtendedGradeId, lPump);
		
		if (lExtendedGradeId > 0)
			l2a(lExtendedGradeId, sGrade, lSizeOfGrade);
	}
}

/*********************************************************************************************
Description:	IsIntheMiddleOfProc() - Returns true in the system is in the middle of process.
Returns:      	BOOL - TRUE if the system is in the middle of process.
Parameters:    None

  WHO			WHEN                    WHAT
  -------------------------------------------------------------------------------
  OmerT			26/07/2011   15:00		Start		4.0.28.501 -  TD 254219
**********************************************************************************************/
BOOL CCommonFunction::IsIntheMiddleOfProc() const
{
	SERVER_INFO sInfo;
	_Module.m_server.m_cServerInfo.GetInfo(sInfo);
	long lTankNumber;
	
	return(sInfo.bRequiredShiftProcess || sInfo.bRequiredPriceChange || sInfo.bRequiredModeProcess || 
		sInfo.byRequiredMaintenanceProcess	 || sInfo.lWetStockFlags || sInfo.byMemoInProcess ||
		sInfo.byDeliveryEmulationStatus || sInfo.bInPriceChangeProcess || 
		(_Module.m_server.m_cCommonFunction.IsDeliveryEmulationSingleTankInProcess(lTankNumber)) ||
		   (_Module.m_server.m_cServerInfo.GetChangeProductInTankState() != CPIT_IDLE));
} // TD 254219	4.0.28.501



/*******************************************************************
 Method:         BuildTerminalCmdRequest
 Description:     fills OLA buffer to Terminal request
 Parameter: 

WHO		        WHEN                WHAT
/-----------------------------------------------------------------
Alex			15/08/2011   		Start   TD 256429
*******************************************************************/
void CCommonFunction::BuildTerminalCmdRequest(TerminalCmdCode eTerminalCmdCode, long lTerminalId, char * pInBuffer)
{
	long lOffset =0;
	CString cstrSiteID = _Module.m_server.GetSiteID();
	BYTE *szSiteID = (BYTE *)cstrSiteID.GetBuffer(cstrSiteID.GetLength());
	
	lOffset += _Module.m_server.m_cOLASrv->SetParam(pInBuffer+lOffset,(BYTE *)"TerminalId[S]",	(BYTE *)"%s=%02ld,",	(long)SERVER_OLA_POS_NUMBER); //4.0.28.503 - TD 256429
	lOffset += _Module.m_server.m_cOLASrv->SetParam(pInBuffer+lOffset,(BYTE *)"PinPadId[I]",	(BYTE *)"%s=%ld,",	lTerminalId);
	lOffset += _Module.m_server.m_cOLASrv->SetParam(pInBuffer+lOffset,(BYTE *)"PinPadCmd[I]",	(BYTE *)"%s=%ld,",	eTerminalCmdCode);
	lOffset += _Module.m_server.m_cOLASrv->SetParam(pInBuffer+lOffset,(BYTE *)"StoreID[I]",	(BYTE *)"%s=%.10s,", szSiteID, strlen((char*)szSiteID)); //4.0.28.503 - TD 256429
	lOffset += _Module.m_server.m_cOLASrv->SetParam(pInBuffer+lOffset,(BYTE *)"PumpNumber[I]",	(BYTE *)"%s=%ld,",	lTerminalId);	//4.0.29.10 - TD 256429
}


/*******************************************************************
 Method:         HandleTerminalCmd
 Description:    executes the commands intended for the Terminal
 Parameter: 

WHO		        WHEN                WHAT
/-----------------------------------------------------------------
Alex			15/08/2011   		Start   TD 256429
*******************************************************************/
long CCommonFunction::HandleTerminalCmd(TerminalCmdCode eTerminalCmdCode, long lTerminalId)
{
	long lRtc = 0;
	
	
	switch(TerminalCmdCode(eTerminalCmdCode))	
	{
	case TERMINAL_CMD_LOGIN:
		lRtc = ProcessTerminallogOn(lTerminalId); 
		break;
		
	case TERMINAL_CMD_UNKNOWN:
		break;
		
	}
	
	
	return lRtc;
}


/*******************************************************************
 Method:         ProcessTerminallogOn
 Description:    indicate to OLA that Terminal ready to start initialization process
 Parameter: 

WHO		        WHEN                WHAT
/-----------------------------------------------------------------
Alex			15/08/2011   		Start   TD 256429
*******************************************************************/
long CCommonFunction::ProcessTerminallogOn(long lTerminalId)
{
	long lRtc = 0;
	
	try
	{	
		
		//  buffer to OLA request
		memset(_Module.m_server.m_cOLASrv->m_sOlaReqBuff,0,sizeof(_Module.m_server.m_cOLASrv->m_sOlaReqBuff));
		memset(_Module.m_server.m_cOLASrv->m_sOlaReqType,0,sizeof(_Module.m_server.m_cOLASrv->m_sOlaReqType));
		
		_Module.m_server.m_cOLASrv->BuildSessionType(SESSION_LOGON, _Module.m_server.m_cOLASrv->m_sOlaReqType);
        BuildTerminalCmdRequest( TERMINAL_CMD_LOGIN, lTerminalId, _Module.m_server.m_cOLASrv->m_sOlaReqBuff);   
		
		
		lRtc = _Module.m_server.m_cOLASrv->StartSession(_Module.m_server.m_cOLASrv->m_sOlaReqBuff, _Module.m_server.m_cOLASrv->m_sOlaReqType);					
		
		if (lRtc != OLASRV_COM_ERROR)
		{
			CString str;
			str.Format("ProcessTerminallogOn,  Successfully send Logon for Terminal: %d, lRtc = %ld", lTerminalId, lRtc); 
			_LOGMSG.LogMsg(str);
			
		}
	}
	
	
	catch (_com_error & e)
	{
		_Module.m_server.m_cOLASrv->Error(e);		
		lRtc = OLASRV_COM_ERROR;
	}
	
	
	return lRtc;
}



/******************************************************************************
Description: Loads a message from the registry according to the deault language
that is set.
Parameters:  lPumpNumber  - Pump number to search in the registry
sMsgPathKey  - Key to search in the registry (registry maps from key to a message)
e.g "PhoneLookUp"
sMsgOut      - Output of the method, the searched message.


WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
Omer			12/10/2011       		Start			4.0.23.1220 TD 155754
******************************************************************************/
void CCommonFunction::LoadMessageFromLanguagesReg(int lPumpNumber, const char * const sMsgPathKey, char * sMsgOut, int iSizeOfMsg, char *pszDefaultVal)
{
	DWORD lLanguageNum = 1;

	CString sTerminalRegPath;
	sTerminalRegPath.Format("Terminals\\Terminal%02d", lPumpNumber);
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTerminalRegPath), "DefaultLanguage", &lLanguageNum, (DWORD)1, FALSE);

	CString sLanguagePath;
	sLanguagePath.Format("Languages\\Language%02d\\", lLanguageNum);
	_Module.m_server.m_cParam.LoadParam((LPCSTR)sLanguagePath, sMsgPathKey, (char *)sMsgOut, iSizeOfMsg, pszDefaultVal, FALSE);
}


void CCommonFunction::CopyZoneToCardSale(long lPumpNumber,  long lReceiptIndex , BYTE*sFiler2043 /*CARD_SALE_EXTRA_DATA4_PLUS & cCardSaleExtraDataPlus*/ )
{
	PS::RECEIPT_EXTRA_INFO_REC cReceiptExtraRec;
	memset(&cReceiptExtraRec, 0, sizeof(PS::RECEIPT_EXTRA_INFO_REC));
	cReceiptExtraRec.info.lIndexNumber = lReceiptIndex;

	long lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, cReceiptExtraRec);
	//long lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cReceiptExtraRec);

	// Fail to read receipt extra data
	if (lRtc)
	{
		memset(sFiler2043, ' ', sizeof(cReceiptExtraRec.info.sFiler2043));
	}
	else
	{
		memcpy(sFiler2043, (BYTE *)cReceiptExtraRec.info.sFiler2043, sizeof(cReceiptExtraRec.info.sFiler2043));
	}

	char sMsg[MAX_MSG_LEN];
	sprintf(sMsg, "CGCI::CopyZoneToCardSale - Zone copied %s", lRtc ? "FAILED" : "SUCCEEDED");
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg);
}


void CCommonFunction::HandleMiscCardNotAuthorized(long lPumpNumber, PAY_AT_PUMP_INFO* pInfo /* = NULL */, CPumpTransact* pTrs /* = NULL */)
{
	long p = lPumpNumber - 1;
	
	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[p].GetStat();
	long lMisc = pumpStat.GetMisc();
	
	lMisc |= MISC_CREDIT_CARD_NOT_AUTHORIZED;
	if (_Module.m_server.GetPAPDeclineTimeValue())
	{
		lMisc |= MISC_CREDIT_CARD_DECLINE;
	}
	
	if (pInfo)
	{
		if (_Module.m_server.GetOLAFlags(pInfo->CardSaleExtraData2.sFlags , FL_PAK_TO_PAP))
		{
			lMisc |= MISC_PAK_TO_PAP_FAILED;
			pInfo->CardSaleInfo.cardData.sTranStatus = CD_TYPE_PAK_TO_PAP_DECLINE;
			_Module.m_server.m_cMemPumpArray[p].SetMemPumpInfo(pInfo ,pTrs);
		}
	}
	
	pumpStat.SetMisc(lMisc);
	_Module.m_server.m_cPumpArray[p].SetStat(pumpStat);
	
	if (slPumpArray.IsLocked())
		slPumpArray.Unlock();
}

void CCommonFunction::ClearDeclinedMiscs(long lPumpNumber, const char* const pCallingMethodName)
{
	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	CPumpStatus pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();
	long lMisc = pumpStat.GetMisc();
	
	if (lMisc & MISC_CREDIT_CARD_DECLINE)
	{
		lMisc &= MISC_NOT_CREDIT_CARD_DECLINE;
		ResetMiscCardNotAuthorized(lMisc);
		
		CString sLogStr;
		sLogStr.Format("%s: Reset misc of MISC_CREDIT_CARD_DECLINE\n", pCallingMethodName);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP, sLogStr); 
		
		pumpStat.SetMisc(lMisc);
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetStat(pumpStat);
	}
	
	if (slPumpArray.IsLocked())
		slPumpArray.Unlock();
}

void CCommonFunction::ResetMiscCardNotAuthorized(long& lMisc)
{
	if (lMisc & MISC_CREDIT_CARD_NOT_AUTHORIZED) 
	{
		lMisc &= MISC_NOT_CREDIT_CARD_NOT_AUTHORIZED;
	}
}

//4.0.20.507
void CCommonFunction::CardSaleAll3ToPayAtPumpInfo(PAY_AT_PUMP_INFO& cPayAtPumpInfoOut,const CARD_SALE_ALL3& cCardSaleAll3In)
{
	memcpy(&cPayAtPumpInfoOut.CardSaleInfo.cardData, &cCardSaleAll3In.CardSaleAll.data, sizeof(cCardSaleAll3In.CardSaleAll.data));
	memcpy(&cPayAtPumpInfoOut.CardSaleInfo.extraData, &cCardSaleAll3In.CardSaleAll.extData, sizeof(cCardSaleAll3In.CardSaleAll.extData));

	memcpy(&cPayAtPumpInfoOut.CardSaleExtraData2, &cCardSaleAll3In.extData2, sizeof(cCardSaleAll3In.extData2));
	memcpy(&cPayAtPumpInfoOut.CardSaleExtraData3, &cCardSaleAll3In.extData3, sizeof(cCardSaleAll3In.extData3));
	memcpy(&cPayAtPumpInfoOut.CardSaleExtraData4, &cCardSaleAll3In.extData4, sizeof(cCardSaleAll3In.extData4));
	memcpy(&cPayAtPumpInfoOut.CardSaleExtraData5, &cCardSaleAll3In.extData5, sizeof(cCardSaleAll3In.extData5));
	memcpy(&cPayAtPumpInfoOut.CardSaleExtraData6, &cCardSaleAll3In.extData6, sizeof(cCardSaleAll3In.extData6));
}


void CCommonFunction::PayAtPumpInfoToCardSaleAll3(CARD_SALE_ALL3&  cCardSaleAll3In, const PAY_AT_PUMP_INFO& cPayAtPumpInfoOut)
{
	memcpy(&cCardSaleAll3In.CardSaleAll.data, &cPayAtPumpInfoOut.CardSaleInfo.cardData, sizeof(cPayAtPumpInfoOut.CardSaleInfo.cardData));
	memcpy(&cCardSaleAll3In.CardSaleAll.extData, &cPayAtPumpInfoOut.CardSaleInfo.extraData, sizeof(cPayAtPumpInfoOut.CardSaleInfo.extraData));

	memcpy(&cCardSaleAll3In.extData2, &cPayAtPumpInfoOut.CardSaleExtraData2, sizeof(cPayAtPumpInfoOut.CardSaleExtraData2));
	memcpy(&cCardSaleAll3In.extData3, &cPayAtPumpInfoOut.CardSaleExtraData3, sizeof(cPayAtPumpInfoOut.CardSaleExtraData3));
	memcpy(&cCardSaleAll3In.extData4, &cPayAtPumpInfoOut.CardSaleExtraData4, sizeof(cPayAtPumpInfoOut.CardSaleExtraData4));
	memcpy(&cCardSaleAll3In.extData5, &cPayAtPumpInfoOut.CardSaleExtraData5, sizeof(cPayAtPumpInfoOut.CardSaleExtraData5));
	memcpy(&cCardSaleAll3In.extData6, &cPayAtPumpInfoOut.CardSaleExtraData6, sizeof(cPayAtPumpInfoOut.CardSaleExtraData6));
}

void CCommonFunction::LoadDefaultLaguageString(long lPumpNumber , char * sOutStr , int iOutStrSize)
{
	
	
	//char sCurrentTerminalRegPath[MAX_FIELD_VAL] = {0};
	//sprintf(sCurrentTerminalRegPath, "Terminals\\Terminal%02d" , lPumpNumber);
	//_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sCurrentTerminalRegPath), "DefaultLanguage",	&lLanguageNum, (DWORD)1 , FALSE);

	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&info);
	int iLanguageId = info.CardSaleExtraData2.sLanguage;

	int iLanguageIndexInRegistry = 1;
	for(int iLanguageIndex = 0;iLanguageIndex<MAX_LANGUAGES;iLanguageIndex++)
	{
		if(iLanguageId == _Module.m_server.m_languagesId[iLanguageIndex])
		{
			iLanguageIndexInRegistry = iLanguageIndex+1;
			break;
		}
	}

	char sTmpLanguagePath[MAX_FIELD_VAL] = {0};
	sprintf_s(sTmpLanguagePath, MAX_FIELD_VAL,"Languages\\Language%02d", iLanguageIndexInRegistry);

	memcpy(sOutStr,sTmpLanguagePath , min(iOutStrSize,sizeof(sTmpLanguagePath)));
}


//  R10 User Story - 92780 - Fuel P@P - OLA recovery.
BOOL CCommonFunction::IsFileExists( const CString& sFileName )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;	
	DWORD dwError = 0;
	BOOL bFileFound =FALSE;
	
	//Find the first file in the directory:
	hFind = FindFirstFile(sFileName ,&FindFileData);
	
	
	if (hFind == INVALID_HANDLE_VALUE)
	{
		
	}
	else
	{	 
		bFileFound =TRUE;
		FindClose(hFind);
	}
	
	return bFileFound;
}



void CCommonFunction::SetPumpMiscExt(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal)
{
	CPumpStatus		pumpStat;
	pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();
	long lTmpMisc = pumpStat.GetMiscExt();

	if (bRemoveMisc)
		lTmpMisc &= lMisc;
	else
		lTmpMisc |= lMisc;

	pumpStat.SetMiscExt(lTmpMisc);

	_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetStat(pumpStat);
	_Module.m_server.SavePumpInfo(lPumpNumber);
}


void CCommonFunction::SetProcessExt(long lProcessNumber, long lParam1, BSTR sInData, BSTR * sOutData, long *pVal)
{

	*pVal = GPI_OK;

	switch (lProcessNumber)
	{
	case PROCESS_HELP:
		//==============================================================
	{
	}
	break;

	case PROCESS_EXT_SET_CASHIER_INFO:
	{
		long lPumpNumber = lParam1;
		// nothing ToDo 
	}
	break;


	default:
		*pVal = GPI_INVALID_PROCESS_NUMBER;
		break;

	}
}


///CR 442586

void CCommonFunction::SmartReconfigure(long lDeviceNumberMask1, long lDeviceNumberMask2, long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags, long *pRetVal, BOOL bSendEvent)
{
	CMD_STRUCT_SMART_RECONFIGURATION_EVENT cParamStruct;

	cParamStruct.lDeviceNumberMask1 = lDeviceNumberMask1;
	cParamStruct.lDeviceNumberMask2 = lDeviceNumberMask2;
	cParamStruct.lTableId = lTableId;
	cParamStruct.lSubTableId = lSubTableId;
	cParamStruct.lSubTableIdExt = lSubTableIdExt;
	cParamStruct.lFlags = lFlags;

	_SmartReconfigure(lDeviceNumberMask1, lDeviceNumberMask2, lTableId, lSubTableId, lSubTableIdExt, lFlags, pRetVal, bSendEvent);
}

////CR 442586

void CCommonFunction::_SmartReconfigure(long lDeviceNumberMask1, long lDeviceNumberMask2, long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags, long *pRetVal, BOOL bSendEvent)
{
	DWORD dwLogLevel = DEFAULT_LOG_LEVEL;
	BOOL bValidTable = TRUE;
	CtrlRetCode lRetCode = CTRL_OK;

	CRegTable cTable;
	CString sLogMsg("CCtrl::SmartReconfigure - "), sRetCode;

	CString cTableName;
	long lClientEventData = 0;


	CMD_STRUCT_SMART_RECONFIGURATION_EVENT cParamStruct;

	cParamStruct.lDeviceNumberMask1 = lDeviceNumberMask1;
	cParamStruct.lDeviceNumberMask2 = lDeviceNumberMask2;
	cParamStruct.lTableId = lTableId;
	cParamStruct.lSubTableId = lSubTableId;
	cParamStruct.lSubTableIdExt = lSubTableIdExt;
	cParamStruct.lFlags = lFlags;

	switch (lTableId)
	{
	case GENERAL_PARAM_TABLE:
		if (!cTable.LoadGeneralParamSubTable(lSubTableId, FALSE, TRUE))  //4.0.2.36
		{
			lRetCode = CTRL_IN_MIDDLE_OF_PROCESS;
			sLogMsg += "GENERAL_PARAM_TABLE (failed OLA reconfiguration), ";
		}
		else
		{
			cTableName = "GeneralParam";
			lClientEventData = lSubTableId;
			sLogMsg += "GENERAL_PARAM_TABLE, ";
		}

		break;

	case SHIFTS_TABLE:
		cTable.LoadShiftsTable(FALSE, TRUE, TRUE);
		sLogMsg += "SHIFTS_TABLE, ";
		cTableName = "Shifts\\Day1\\Shift1";
		break;

	case DATABASE_TABLE:
		cTable.LoadDatabaseTable(FALSE, TRUE, TRUE);
		sLogMsg += "DATABASE_TABLE, ";
		cTableName = "Database";
		break;

	case TERMINALS_TABLE:
		sLogMsg += "TERMINALS_TABLE (delegated to CmdTX), ";
		cTableName = "Terminals\\Terminal01";
		break;

	case CARWASH_TABLE:
		_Module.m_server.m_cCarWash.LoadCarWashDefinition();
		sLogMsg += "CARWASH_TABLE, ";
		cTableName = "CarWash";
		break;

	case GRADES_TABLE:
		cTable.LoadGradesTable(FALSE, TRUE, TRUE);
		sLogMsg += "GRADES_TABLE, ";
		cTableName = "Grades\\Grade01";
		break;

	case IPT_TABLE:
		cTable.LoadIPTTable(FALSE, TRUE);
		sLogMsg += "IPT_TABLE, ";
		cTableName = "IPT\\IPT01";
		break;

	case MESSAGES_TABLE:
		sLogMsg += "MESSAGES_TABLE - not support";     //4.0.6.506
		cTableName = "Messages";
		break;

		//Mode reconfiguration supported by PumpSrv without a call
		//to Reconfigure2, so we just write to the log.
	case MODES_TABLE:
		sLogMsg += "MODES_TABLE";
		bSendEvent = FALSE; //no need to send via the reconfigure	
		break;

	case PRICE_POLES_TABLE:
		cTable.LoadPricePolesTable(FALSE, TRUE);
		sLogMsg += "PRICE_POLES_TABLE, ";
		cTableName = "PricePoles\\PricePole01";
		break;

	case PURE_PRODUCTS_TABLE:
		cTable.LoadPureProductsTable(FALSE, TRUE, TRUE);
		sLogMsg += "PURE_PRODUCTS_TABLE, ";
		cTableName = "PureProducts\\PureProduct01";
		break;

	case RECEIPT_TABLE:
		cTable.LoadReceiptTable(FALSE, TRUE, TRUE);
		sLogMsg += "RECEIPT_TABLE, ";
		cTableName = "Receipt";
		break;

	case SCREEN_LAYOUT_TABLE:
		cTable.LoadScreenLayoutsTable(FALSE, TRUE, TRUE);
		sLogMsg += "SCREEN_LAYOUT_TABLE, ";
		cTableName = "ScreenLayouts\\ScreenLayout01";
		break;

	case SERVICE_FEE_TABLE:
		cTable.LoadServiceFeeTable(FALSE, TRUE, TRUE);
		sLogMsg += "SERVICE_FEE_TABLE, ";
		cTableName = "ServiceFee\\ServiceFee01";
		break;

	case TANKS_TABLE:
		cTable.LoadTanksTable(FALSE, TRUE, TRUE);
		sLogMsg += "TANKS_TABLE, ";
		cTableName = "Tanks\\Tank01";
		break;

	case TAXES_TABLE:
		cTable.LoadTaxesTable(FALSE, TRUE);
		sLogMsg += "TAXES_TABLE, ";
		cTableName = "Taxes\\Tax01";
		break;

	case WASH_PROGRAMS_TABLE:
		sLogMsg += "WASH_PROGRAMS_TABLE (not supported), ";
		lRetCode = CTRL_NOT_SUPPORTED;
		cTableName = "WashPrograms\\WashProgram01";
		break;

	case ALLIED_TABLE:
		sLogMsg += "ALLIED_TABLE, ";
		cTableName = "Allied";
		break;

	case RFS_TABLE:
		cTable.LoadRFSSubTable(lSubTableId, FALSE, TRUE);
		sLogMsg += "RFS_TABLE, ";
		cTableName = "RFS";
		lClientEventData = lSubTableId;
		break;

	case MEGGITT_TABLE:  //4.0.2.36
		sLogMsg += "MEGGITT_TABLE ";
		cTableName = "Meggitt";
		break;

	case DOMS_TABLE:
	{
		cTable.LoadDOMSSubTable(lSubTableId, FALSE, TRUE);
		cTableName = "Doms";
		break;
	}

	case POSTEC_TABLE:					//4.0.21.21 TD 69287
	{
		sLogMsg += "POSTEC_TABLE, ";
		cTableName = "Postec";
	}break;

	case PUMPS_TABLE:
		sLogMsg += "PUMPS_TABLE (delegated to CmdTX), ";
		cTableName = "Pumps\\Pump01";
		break;

	case TANK_SRV_TABLE:		//CR 414103
		sLogMsg += "TANK_SRV_TABLE (delegated to CmdTX), ";
		cTableName = "TankSrv";
		break;

	default:
		bValidTable = FALSE;
		sLogMsg += "Invalid table ID, ";
		dwLogLevel = LOG_LEVEL_0;
		lRetCode = CTRL_FAILED_IO;  //4.0.2.36
		break;
	}

	if (bValidTable)
	{
		_Module.m_server.SetCommand(CMD_SMART_RECONFIGURE_EVENT,
			SERVER_POS_NUMBER,
			0,
			&cParamStruct,
			sizeof(cParamStruct));

		//4.0.5.11
		if (bSendEvent == TRUE && _HOST.m_lHydraActive)
		{
			DWORD dwParamCRC = 0;
			_Module.m_server.m_cParam.LoadParam(cTableName.GetBuffer(0), "ParamCRC", &dwParamCRC, 0, TRUE, FALSE);
			_Module.m_server.m_cParam.SaveParam(cTableName.GetBuffer(0), "ParamCRC", dwParamCRC, TRUE, _HOST.m_lGroupNum, 0, lClientEventData);
		}
	}

	if (pRetVal)
		*pRetVal = lRetCode;

	return;
}


//4.0.25.1140 TD 412765
void CCommonFunction::ConvertExtraInfoRecord2Tag(TAG_PUMP_TEST_INFO* psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber)
{
	memcpy(&psTagInfo->sUnLockSerNumPumpTest, &rsExtraInfo.info.sReferenceNumber, sizeof(psTagInfo->sUnLockSerNumPumpTest));
	if (LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("CGPI:QueryExtraInfoData UnLockSerNumPumpTest=[%.20s]", psTagInfo->sUnLockSerNumPumpTest);
		_Module.m_server.m_cLogger.LogMsg(str);

	}
}


/******************************************************************************
Description:	Parse Connected pumps from given delivery xml string

Returns:      	true if parse successful
Parameters:	sInXml - xml in string format
BOOL & bConnectedPumps - returned connected pumps flag

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Alex		08/02/2013   11:00			Start		4.0.25.994  TD 325383
*******************************************************************************/
CtrlRetCode CCommonFunction::GetConfiguration(BSTR sClientName, BSTR *sConfigurationData)
{

	CtrlRetCode  retCode = CTRL_RECORD_NOT_FOUND;

	CXMLInterface cXMLInterface;

	char sDirName[MAX_FILE_PATH] = { 0 };
	BYTE	szFullFileName[MAX_PATH] = { 0 };


	DWORD	 dwAttrs = 0x0;

	_Module.m_server.m_cParam.LoadParam("General", "ConfigurationPath", sDirName, sizeof(sDirName), "c:\\Office\\pumpSrv\\Data\\Configuration", 0);


	dwAttrs = GetFileAttributes(sDirName);
	if ((dwAttrs & FILE_ATTRIBUTE_DIRECTORY) && (dwAttrs != INVALID_FILE_SIZE))
	{
		memcpy(szFullFileName, sDirName, strlen((const char*)sDirName));
		strcat((char*)szFullFileName, "\\");
		strcat((char*)szFullFileName, "FuelConfiguration.xml");

		dwAttrs = GetFileAttributes(LPTSTR(szFullFileName));
		if ((dwAttrs & FILE_ATTRIBUTE_ARCHIVE) && (dwAttrs != INVALID_FILE_SIZE))
		{
			cXMLInterface.InitDoc();

			BOOL bIsLoadXml = cXMLInterface.LoadFile((char*)szFullFileName);
			if (TRUE == bIsLoadXml)
			{

				bIsLoadXml = cXMLInterface.GetXmlString(sConfigurationData);
				retCode = CTRL_OK;

			}

			cXMLInterface.CloseDoc();

		}

	}


	return retCode;
}


//4.0.25.1130 

CtrlRetCode CCommonFunction::SetTankState(long lTankNumber, long lState, long lFlags, BSTR sRec)
{
	CtrlRetCode  retCode = CTRL_FAILED_IO;

	try
	{
		if (lTankNumber == READ_ALL_TANKS)
		{
			SetAllTanksState(lState);
			retCode = CTRL_OK;
		}
		else
		{
			if (TANK_NOT_VALID(lTankNumber))
			{
				retCode = CTRL_INVALID_TANK_NUMBER;
			}
			else
			{
				SetSingleTankState(lTankNumber, lState);
				retCode = CTRL_OK;
			}

		}


	}
	catch (...)
	{
		_Module.m_server.m_cLogger.LogMsg("CCommonFunction", "SetTankState", 0, LOG_NONE, "Unexpected Error occurred");
	}


	return retCode;

}

void CCommonFunction::SetAllTanksState(long lState)
{
	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString sMsg;
		sMsg.Format(" Got SetTankState for ALL tanks: New TankState=%ld", lState);
		_Module.m_server.m_cLogger.LogMsg(sMsg);
	}

	for (long ind = 0; ind < MAX_TANKS; ind++)
	{
		if (_Module.m_server.m_cTankArray[ind].IsTankValid())
		{
			_Module.m_server.m_cTankArray[ind].SetTankState(lState);
		}
	}
}

// 4.0.25.1130 
void CCommonFunction::SetSingleTankState(long lTankNumber, long lState)
{
	long lOldState = 0;

	lOldState = _Module.m_server.m_cTankArray[lTankNumber - 1].GetTankState();
	if (_Module.m_server.m_cTankArray[lTankNumber - 1].GetTankState() != lState)
	{
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString sMsg;
			sMsg.Format("SetTankState: State changed TankNumber=%ld, TankState=%ld, OldState=%ld", lTankNumber, lState, lOldState);
			_Module.m_server.m_cLogger.LogMsg(sMsg);
		}
		_Module.m_server.m_cTankArray[lTankNumber - 1].SetTankState(lState);

	}
}


BOOL CCommonFunction::GetGradePositionInfo(long lPumpNumber, long lPosition, TAG_GRADE_POSITION_INFO &cTagGradePositionInfo)
{
	BOOL bVal = FALSE;
	long lGrade, lTankNumber, lPositionStatus;

	lGrade = _Module.m_server.m_cPumpArray[lPumpNumber - 1].NozzleToGrade(lPosition);

	if (lGrade)
	{
		CString sTable;
		sTable.Format("Grades\\Grade%.02ld", lGrade);
		char	szGrade[MAX_FIELD_VAL] = { 0 };

		memset(&cTagGradePositionInfo, ' ', sizeof(cTagGradePositionInfo));
		l2a(lGrade, cTagGradePositionInfo.sGradeNumber, sizeof(cTagGradePositionInfo.sGradeNumber));
		memset(szGrade, ' ', sizeof(szGrade));
		_Module.m_server.m_cParam.LoadParam((LPCSTR)sTable, "FullName", (char *)szGrade, sizeof(szGrade), "Unleaded", FALSE);
		memcpy(cTagGradePositionInfo.sGradeName, szGrade, min(sizeof(cTagGradePositionInfo.sGradeName), strlen(szGrade)));


		lTankNumber = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTankPerPosition(lPosition);

		lPositionStatus = 0;
		if (lTankNumber > 0)
			lPositionStatus = _Module.m_server.m_cTankArray[lTankNumber - 1].GetTankState();

		l2a(lPositionStatus, cTagGradePositionInfo.sPositionStatus, sizeof(cTagGradePositionInfo.sPositionStatus));

		bVal = TRUE;
	}
	else
	{
		memset(&cTagGradePositionInfo, ' ', sizeof(cTagGradePositionInfo));
		l2a(0, cTagGradePositionInfo.sGradeNumber, sizeof(cTagGradePositionInfo.sGradeNumber));
		bVal = TRUE;
	}


	return bVal;
}


/******************************************************************************
Description:  Check if Nozzle was locked

Returns:      BOOL - indicating if Nozzle is locked

Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
AlexM		14/08/2014					Start	4.0.25.1130
******************************************************************************/
BOOL CCommonFunction::IsNozzleLocked(long lPumpNumber, long nPosition)
{
	BOOL bRetVal = FALSE;

	if (nPosition > 0)
	{
		long lTankNumber = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetTankPerPosition(nPosition);
		if (lTankNumber > 0)
			bRetVal = (_Module.m_server.m_cTankArray[lTankNumber - 1].IsTankBlockedByUser());
	}

	return bRetVal;

}

//4.0.25.1130
CtrlRetCode CCommonFunction::GetTankState(long lTankNumber, long lFlags, BSTR * pRec)
{

	CtrlRetCode  retCode = CTRL_FAILED_IO;

	try
	{
		if (lTankNumber == READ_ALL_TANKS)
		{
			_Module.m_server.m_cTankXML.AllTanksStateXml(pRec);
			retCode = CTRL_OK;
		}
		else
		{
			if (TANK_NOT_VALID(lTankNumber))
				retCode = CTRL_INVALID_TANK_NUMBER;
			else
			{
				_Module.m_server.m_cTankXML.TankStateXml(&_Module.m_server.m_cTankArray[lTankNumber - 1], pRec);
				retCode = CTRL_OK;
			}
		}

	}
	catch (...)
	{
		_Module.m_server.m_cLogger.LogMsg("CCommonFunction", "GetTankState", 0, LOG_NONE, "Unexpected Error occurred");
	}

	return retCode;

}

/******************************************************************************
Description:	Parse Sequence  Number from an XML given from BOS

Returns:
Parameters:   	BSTR sXmlRec - string in an XML format, which contains data
<PumpCalibrationDetails UnLockSerNumPumpTest="12343"></PumpCalibrationDetails>

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
4.0.25.1140 TD 412765

******************************************************************************/
BOOL CCommonFunction::SetPumpCalibrationDetails(long lPumpNumber, BSTR sXmlRec)
{

	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();


	BOOL bRetCode = FALSE;

	char sXmlBuffer[MAX_XML_BUF_LEN];
	char szStr[MAX_FIELD_VAL] = { 0 };
	_variant_t varValue;


	memset(sXmlBuffer, 0, sizeof(sXmlBuffer));

	int iLen = Bstr2Buffer(sXmlRec, (char *)sXmlBuffer, MAX_XML_BUF_LEN);

	if (iLen != 0)
	{
		CXMLInterface  xmlDoc;
		CXMLPointers   cRootElement, cAllRecordsList, cRecordElement;

		try
		{
			if (xmlDoc.InitDoc())
			{
				bRetCode = xmlDoc.LoadXmlString(sXmlBuffer);

				if (bRetCode)
				{
					bRetCode = xmlDoc.GetRootNode(&cRootElement);

					varValue = xmlDoc.GetAttribute("UnLockSerNumPumpTest");

					if (varValue.bstrVal)
					{
						memcpy(szStr, (const char*)((_bstr_t)varValue), sizeof(varValue));
						iLen = strlen(szStr);

						if (iLen)
						{
							PAY_AT_PUMP_INFO info;
							_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&info);
							memset(info.CardSaleInfo.cardData.sReferenceNumber, ' ', sizeof(info.CardSaleInfo.cardData.sReferenceNumber));
							memcpy(info.CardSaleInfo.cardData.sReferenceNumber, szStr, min(sizeof(info.CardSaleInfo.cardData.sReferenceNumber), iLen));

							_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&info);

							SQLPumpSrvFile.LoadPAPInfo(lPumpNumber);



							if (LOG_DETAIL_FLOW_CONTROL)
							{
								CString strLog;
								strLog.Format("CCommonFunction::SetPumpCalibrationDetails: Set UnLockSerNumPumpTest:[%s].", szStr);
								_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, strLog);
							}
						}
					}
				}
			}
		}
		catch (...)
		{
			_Module.m_server.m_cLogger.LogMsg("CCommonFunction", "SetPumpCalibrationDetails", 0, LOG_NONE, "Unexpected Error occurred");
			bRetCode = FALSE;
		}
		xmlDoc.CloseDoc();
	}


	if (!bRetCode)
	{
		CString sLogMsg;
		sLogMsg.Format("CCommonFunction: SetPumpCalibrationDetails failed to set info. XmlBuffer:[%40s]", sXmlBuffer);
		_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, sLogMsg, LOG_LEVEL_1);

	}


	return bRetCode;
}



void CCommonFunction::SetPumpNozzleNotification(long lPumpNumber, long lFlagIsOn)
{
	CPumpStatus		pumpStat;
	pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();


	// 	if(LOG_BASIC_FLOW_CONTROL)
	// 	{
	// 		CString str;
	// 		str.Format("SetPumpNozzleNotification lFlagIsOn = %ld.", lFlagIsOn);
	// 		_Module.m_server.m_cLogger.LogMsg("GCI","SetPumpNozzleNotification",lPumpNumber,LOG_PUMP,str);
	// 
	// 	}

	if (pumpStat.GetResExt() == RES_EXT_MOBILE)
	{

		if (LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("Set Misc Mobile Pump Nozzle Up. Misc = %ld.", lFlagIsOn);
			_Module.m_server.m_cLogger.LogMsg("GCI", "SetPumpNozzleNotification", lPumpNumber, LOG_PUMP, str);

		}

		if (lFlagIsOn == 1)
		{
			SetPumpMiscExt(lPumpNumber, MISC_EXT_MOBILE_REQUEST_NOTIFICATION, FALSE, NULL);

		}

		if (lFlagIsOn == 0)
		{
			SetPumpMiscExt(lPumpNumber, MISC_EXT_NOT_MOBILE_REQUEST_NOTIFICATION, TRUE, NULL);
		}

	}


}


void CCommonFunction::SetMaximalAllowedDiscountToPumpTransaction(CPumpTransact *pTrs) 	//RFUEL-3309 Get the maximal discount allowed for the grade 
{
	long lGradePrice = 0;
	long lMaxAllowedDiscount = _Module.m_server.GetMaximalDiscountForGrade(pTrs->m_nGrade,LOWEST_PRICE,&lGradePrice);
	long lMaxAllowedDiscountCurrent = a2l(pTrs->m_sMaxDiscountAllowed, sizeof(pTrs->m_sMaxDiscountAllowed));
	l2a(lMaxAllowedDiscount, pTrs->m_sMaxDiscountAllowed, sizeof(pTrs->m_sMaxDiscountAllowed));
#ifdef DEBUG
	if (LOG_ALL_FLOW_CONTROL)
	{
		lMaxAllowedDiscount = a2l(pTrs->m_sMaxDiscountAllowed, sizeof(pTrs->m_sMaxDiscountAllowed));
		CString str;str.Format("CCommonFunction::SetMaximalAllowedDiscountToPumpTransaction transaction %ld ,sMaxDiscountAllowed %ld , lGradePrice %ld", pTrs->m_lNumber, lMaxAllowedDiscount, lGradePrice);
		_LOGMSG.LogMsg(str);
	}
#endif // DEBUG
}
