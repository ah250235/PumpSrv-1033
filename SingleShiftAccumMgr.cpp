// SingleShiftAccumMgr.cpp: implementation of the CSingleShiftAccumMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SingleShiftAccumMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSingleShiftAccumMgr::CSingleShiftAccumMgr()
{
	for (long lTankNumber = 1; lTankNumber <= MAX_TANKS; lTankNumber++)
	{
		memset(&m_cSingleShiftAccumTankArray[lTankNumber-1], 0x00, sizeof(m_cSingleShiftAccumTankArray[lTankNumber-1]));
	}
	
	for(long lPump = 1;lPump <= MAX_PUMPS_64;lPump++)
	{
		for(short nPosition = 1; nPosition <= MAX_POSITIONS_PER_PUMP; nPosition++)
		{
			memset(&m_cSingleShiftAccumPumpPositionArray[lPump-1][nPosition-1], 0x00, sizeof(m_cSingleShiftAccumPumpPositionArray[lPump-1][nPosition-1]));
		}
	}
}

CSingleShiftAccumMgr::~CSingleShiftAccumMgr()
{

}


/******************************************************************************
 Description: Calculate the Qdex index by tank number 
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			04/08/2010   15:00		Start
******************************************************************************/
long CSingleShiftAccumMgr::GetDbIndexByTank(const long lTankNumber)
{
	long lIndex = 0;
	lIndex = ((lTankNumber + 1) * sizeof(SINGLE_SHIFT_ACCUM_TANK_RECORD));
	return(lIndex);
}

/******************************************************************************
 Description: Calculate the Qdex index by Pump and Position
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			04/08/2010   15:00		Start
******************************************************************************/
long CSingleShiftAccumMgr::GetDbIndexByPumpPosition(const long lPump,const short nPosition)
{
	long lIndex = 0;
	lIndex = SINGLE_SHIFT_ACCUM_FIRST_PUMP_POSITION_RECORD + (( lPump * MAX_POSITIONS_PER_PUMP + nPosition ) * sizeof(SINGLE_SHIFT_ACCUM_TANK_RECORD) );
	return(lIndex);
}

/******************************************************************************
 Description: Reset the single shift accumulation tank records in Qdex 
 Returns:     
 Parameters:   Tank number.
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				04/08/2010   15:00		Start
******************************************************************************/
void CSingleShiftAccumMgr::ResetTankRecordInDB(const long lTankNum)
{
	memset(&m_cSingleShiftAccumTankArray[lTankNum-1], 0x00, sizeof(SINGLE_SHIFT_ACCUM_TANK_RECORD));
	
	long lRtc = SaveTankRecordInDB(lTankNum);
	if (lRtc) //error!
	{
		CString sMsg;
		sMsg.Format("ResetTankRecordInDB() failed to save to Qdex!!! Retval = %ld",lRtc);
		_LOGMSG.LogMsg(sMsg);
	}
}

/******************************************************************************
 Description: Reset the single shift accumulation pump position records in Qdex 
 Returns:     
 Parameters:   Tank number.
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				04/08/2010   15:00		Start
******************************************************************************/
void CSingleShiftAccumMgr::ResetPumpPositionRecordInDB(const long lPump,const short nPosition)
{
	memset(&m_cSingleShiftAccumPumpPositionArray[lPump-1][nPosition-1], 0x00, sizeof(SINGLE_SHIFT_ACCUM_PUMP_POSITION_RECORD));
	
	long lRtc = SavePumpPositionRecordInDB(lPump, nPosition);
	if (lRtc) //error!
	{
		CString sMsg;
		sMsg.Format("ResetPumpPositionRecordInDB() failed to save to Qdex!!! Retval = %ld",lRtc);
		_LOGMSG.LogMsg(lPump, LOG_PUMP, sMsg);
	}
}


/******************************************************************************
 Description:	Saves a single shift accumulation tank record into Qdx by index 	
 Returns:       0 if ok.  	
 Parameters:  	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				04/08/2010   15:30		Start
******************************************************************************/
long CSingleShiftAccumMgr::SaveTankRecordInDB(const long lTankNum)
{
	long lRtc = RetCode_Success;
	SingleShiftAccumRelativeIndex  cDBRelativeIndex;

	cDBRelativeIndex.lOffSet = GetDbIndexByTank(lTankNum);
	cDBRelativeIndex.lSize  = sizeof(SINGLE_SHIFT_ACCUM_TANK_RECORD);

	lRtc = _Module.m_server.IndexOperation(SINGLE_SHIFT_ACCUMULATIONS_FILE, REC_UPDATE, (void *)&m_cSingleShiftAccumTankArray[lTankNum-1], &cDBRelativeIndex);	

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString sMsg;
		sMsg.Format("DBSingleShiftAccum write tank rec buffer to qdx len=%ld, Rtc=%ld" ,cDBRelativeIndex.lSize,lRtc) ;
		_LOGMSG.LogMsg(sMsg);					
	}

	return(lRtc);
}

/******************************************************************************
 Description:	Saves a single shift accumulation pump position record 
				into Qdx by index 	
 Returns:       0 if ok.  	
 Parameters:  	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				04/08/2010   16:00		Start
******************************************************************************/
long CSingleShiftAccumMgr::SavePumpPositionRecordInDB(const long lPump, const short nPosition)
{
	long lRtc = RetCode_Success;
	SingleShiftAccumRelativeIndex  cDBRelativeIndex;

	cDBRelativeIndex.lOffSet = GetDbIndexByPumpPosition(lPump, nPosition);
	cDBRelativeIndex.lSize  = sizeof(SINGLE_SHIFT_ACCUM_PUMP_POSITION_RECORD);
	
	lRtc = _Module.m_server.IndexOperation(SINGLE_SHIFT_ACCUMULATIONS_FILE, REC_UPDATE , (void *)&m_cSingleShiftAccumPumpPositionArray[lPump-1][nPosition-1], &cDBRelativeIndex);

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString sMsg;
		sMsg.Format("DBSingleShiftAccum write pump position buffer to qdx len=%ld, Rtc=%ld" ,cDBRelativeIndex.lSize,lRtc) ;
		_LOGMSG.LogMsg(lPump, LOG_PUMP, sMsg);
	}

	return(lRtc);
}

/******************************************************************************
 Description: Loads a single shift accumulation tank record	from Qdx by index	
 Returns:     0 if ok , the struct by ref;
 Parameters:   	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				04/08/2010   16:00		Start
******************************************************************************/
long CSingleShiftAccumMgr::LoadTankRecordFromDB(const long lTankNum)
{
	long lRtc = RetCode_Success;

	DBTankGaugeRelativeIndex  cDBRelativeIndex;
	
	memset(&m_cSingleShiftAccumTankArray[lTankNum-1] ,0x00 , sizeof(SINGLE_SHIFT_ACCUM_TANK_RECORD));

	cDBRelativeIndex.lOffSet = GetDbIndexByTank(lTankNum);
	cDBRelativeIndex.lSize  = sizeof(SINGLE_SHIFT_ACCUM_TANK_RECORD);
	
	lRtc = _Module.m_server.IndexOperation(SINGLE_SHIFT_ACCUMULATIONS_FILE, REC_READ, &m_cSingleShiftAccumTankArray[lTankNum-1], &cDBRelativeIndex);

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString sMsg;
		sMsg.Format("DBSingleShiftAccum load tank buffer from qdx len=%ld, Rtc=%ld" ,cDBRelativeIndex.lSize,lRtc) ;
		_LOGMSG.LogMsg(sMsg);					
	}

	return(lRtc);
}

/******************************************************************************
 Description: Loads a single shift accumulation tank record	from Qdx by index	
 Returns:     0 if ok , the struct by ref;
 Parameters:   	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				04/08/2010   16:00		Start
******************************************************************************/
long CSingleShiftAccumMgr::LoadPumpPositionRecordFromDB(const long lPump, const short nPosition)
{
	long lRtc = RetCode_Success;

	DBTankGaugeRelativeIndex  cDBRelativeIndex;
	
	memset(&m_cSingleShiftAccumPumpPositionArray[lPump-1][nPosition-1] ,0x00 , sizeof(SINGLE_SHIFT_ACCUM_PUMP_POSITION_RECORD));

	cDBRelativeIndex.lOffSet = GetDbIndexByPumpPosition(lPump, nPosition);
	cDBRelativeIndex.lSize  = sizeof(SINGLE_SHIFT_ACCUM_PUMP_POSITION_RECORD);
	
	lRtc = _Module.m_server.IndexOperation(SINGLE_SHIFT_ACCUMULATIONS_FILE, REC_READ, &m_cSingleShiftAccumPumpPositionArray[lPump-1][nPosition-1], &cDBRelativeIndex);

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString sMsg;
		sMsg.Format("DBSingleShiftAccum load pump position buffer from qdx len=%ld, Rtc=%ld" ,cDBRelativeIndex.lSize,lRtc) ;
		_LOGMSG.LogMsg(lPump, LOG_PUMP, sMsg);
	}
	
	return(lRtc);
}

/******************************************************************************
 Description: Reset all records - tanks and pumps
 Returns:     
 Parameters:   	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				09/08/2010   12:00		Start
******************************************************************************/
void CSingleShiftAccumMgr::ResetAllRecordsInDB()
{
	for (long lTankNumber = 1; lTankNumber <= _Module.m_server.GetLastValidTank(); lTankNumber++)
	{
		ResetTankRecordInDB(lTankNumber);
	}
	
	for(long lPump = 1;lPump <= _Module.m_server.GetLastValidPump(); lPump++)
	{
		for(short nPosition = 1; nPosition <= MAX_POSITIONS_PER_PUMP; nPosition++)
		{
			ResetPumpPositionRecordInDB(lPump, nPosition);
		}
	}

}


/******************************************************************************
 Description: Load all tanks info from single shift accumulation Qdex
 Returns:     
 Parameters:   	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				10/08/2010   12:00		Start
******************************************************************************/
void CSingleShiftAccumMgr::LoadTanksFromDB()
{
	for (long lTankNumber = 1; lTankNumber <= _Module.m_server.GetLastValidTank(); lTankNumber++)
		LoadTankRecordFromDB(lTankNumber);
}


/******************************************************************************
 Description: Load all pumps info from single shift accumulation Qdex
 Returns:     
 Parameters:   	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				10/08/2010   12:00		Start
******************************************************************************/
void CSingleShiftAccumMgr::LoadPumpsFromDB()
{
	for(long lPump = 1;lPump <= _Module.m_server.GetLastValidPump(); lPump++)
	{
		for (short nPosition = 1; nPosition <= MAX_POSITIONS_PER_PUMP; nPosition++)
		{
			LoadPumpPositionRecordFromDB(lPump, nPosition);
		}
	}
}


/******************************************************************************
 Description: Get single shift accumulation tank record
 Returns:     
 Parameters:   	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				10/08/2010   12:00		Start
******************************************************************************/
long CSingleShiftAccumMgr::GetTankRecord(const long lTankNum, SINGLE_SHIFT_ACCUM_TANK_RECORD &cTankRecord)
{
	long lRtc = RetCode_Success;

	if (lTankNum >= 1 && lTankNum <= _Module.m_server.GetLastValidTank())
		memcpy((void *)&cTankRecord, &m_cSingleShiftAccumTankArray[lTankNum-1], sizeof(cTankRecord));
	else
		lRtc = RetCode_Failure;

	return lRtc;
}


/******************************************************************************
 Description: Get single shift accumulation pump position record
 Returns:     
 Parameters:   	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				10/08/2010   12:00		Start
******************************************************************************/
long CSingleShiftAccumMgr::GetPumpPositionRecord(const long lPump,const short nPosition, SINGLE_SHIFT_ACCUM_PUMP_POSITION_RECORD &cPumpPositionRecord)
{
	long lRtc = RetCode_Success;

	if ((lPump >= 1 && lPump <= _Module.m_server.GetLastValidPump()) && (nPosition >= 1 && nPosition <= MAX_POSITIONS_PER_PUMP))
		memcpy((void *)&cPumpPositionRecord, &m_cSingleShiftAccumPumpPositionArray[lPump-1][nPosition-1], sizeof(cPumpPositionRecord));
	else
		lRtc = RetCode_Failure;

	return lRtc;
}


/******************************************************************************
 Description: Accumulate delivery volume
 Returns:     
 Parameters:   	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				10/08/2010   12:00		Start
******************************************************************************/
long CSingleShiftAccumMgr::AccumulateDeliveryVolume(const long lTankNum, const long lDeliveryVolume)
{
	long lRtc = RetCode_Success;

	if (lTankNum >= 1 && lTankNum <= _Module.m_server.GetLastValidTank())
	{
		m_cSingleShiftAccumTankArray[lTankNum-1].lShiftAccumulatedDeliveryVolume += lDeliveryVolume;
		SaveTankRecordInDB(lTankNum);

		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString sMsg;
			sMsg.Format("CSingleShiftAccumMgr::AccumulateDeliveryVolume lTankNum=%ld, lDeliveryVolume=%d, Accum delivery volume=%ld" ,lTankNum, lDeliveryVolume, m_cSingleShiftAccumTankArray[lTankNum-1].lShiftAccumulatedDeliveryVolume) ;
			_LOGMSG.LogMsg(sMsg);					
		}
	}
	else
		lRtc = RetCode_Failure;
	
	return lRtc;
}

/******************************************************************************
 Description: Accumulate sales for tank and pump/position
 Returns:     
 Parameters:   	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				24/08/2010   12:00		Start
******************************************************************************/
long CSingleShiftAccumMgr::AccumulateSales(const long lPump,const short nPosition, const long lSaleAmount)
{
	long lRtc = RetCode_Success;
	long lTank = 0;

	if ((lPump >= 1 && lPump <= _Module.m_server.GetLastValidPump()) && (nPosition >= 1 && nPosition <= MAX_POSITIONS_PER_PUMP))
	{
		m_cSingleShiftAccumPumpPositionArray[lPump-1][nPosition-1].lShiftAccumulatedSales += lSaleAmount;
		SavePumpPositionRecordInDB(lPump, nPosition);

		lTank = _Module.m_server.m_cPumpArray[lPump-1].GetTankPerPosition(nPosition);
		if (lTank >= 1 && lTank <= _Module.m_server.GetLastValidTank())
		{
			m_cSingleShiftAccumTankArray[lTank-1].lShiftAccumulatedSales += lSaleAmount;
			SaveTankRecordInDB(lTank);
		}
		else
			lRtc = RetCode_Failure;


		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString sMsg;
			sMsg.Format("CSingleShiftAccumMgr::AccumulateSales Pump=%ld, Position=%d, Tank=%ld, SaleAmount=%ld, Accum pump sales=%ld, Accum tank sales=%ld" ,lPump, nPosition, lTank, lSaleAmount, m_cSingleShiftAccumPumpPositionArray[lPump-1][nPosition-1].lShiftAccumulatedSales, m_cSingleShiftAccumTankArray[lTank-1].lShiftAccumulatedSales) ;
			_LOGMSG.LogMsg(lPump, LOG_PUMP, sMsg);					
		}
	}
	else
		lRtc = RetCode_Failure;

	return lRtc;
}

/******************************************************************************
 Description: Accumulate pump test sales
 Returns:     
 Parameters:   	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				24/08/2010   15:00		Start
******************************************************************************/
long CSingleShiftAccumMgr::AccumulatePumpTest(const long lPump,const short nPosition, const long lPumpTestSaleAmount)
{
	long lRtc = RetCode_Success;

	if ((lPump >= 1 && lPump <= _Module.m_server.GetLastValidPump()) && (nPosition >= 1 && nPosition <= MAX_POSITIONS_PER_PUMP))
	{
		m_cSingleShiftAccumPumpPositionArray[lPump-1][nPosition-1].lShiftAccumulatedPumpTest += lPumpTestSaleAmount;
		SavePumpPositionRecordInDB(lPump, nPosition);

		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString sMsg;
			sMsg.Format("CSingleShiftAccumMgr::AccumulatePumpTest Pump=%ld, Position=%d, PumpTestSaleAmount=%ld, Accum pump test=%ld" ,lPump, nPosition, lPumpTestSaleAmount, m_cSingleShiftAccumPumpPositionArray[lPump-1][nPosition-1].lShiftAccumulatedPumpTest);
			_LOGMSG.LogMsg(lPump, LOG_PUMP, sMsg);					
		}
	}
	else
		lRtc = RetCode_Failure;

	return lRtc;
}

/******************************************************************************
 Description: Load all all data from single shift accumulation Qdex
 Returns:     
 Parameters:   	
 
 WHO				WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella				25/08/2010   12:00		Start
******************************************************************************/
void CSingleShiftAccumMgr::LoadDataFromDB()
{
	LoadTanksFromDB();
	LoadPumpsFromDB();
}