// ServerInfo.cpp: implementation of the CServerInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerInfo::CServerInfo()
{
	m_bSendMemoPumpEvent = FALSE; 
}

CServerInfo::~CServerInfo()
{
}
/******************************************************************************
 Description:	change Server status 	
 Returns:       None	
 Parameters:   	LockStatus - new sever status 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   13:44		Start
******************************************************************************/

void CServerInfo::SetServerStatus(LockStatus eStatus)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);
	m_eLockStatus = eStatus; 
}
/******************************************************************************
 Description:	Get server status 	
 Returns:       LockStatus - Server status 
 Parameters:   	None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   13:44		Start
******************************************************************************/
LockStatus CServerInfo::GetServerStatus()
{
	LockStatus eTmpLockStatus; 

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	eTmpLockStatus = m_eLockStatus;

	return eTmpLockStatus;
}
/******************************************************************************
 Description:	Change server reservation 	
 Returns:       None	
 Parameters:   	lVal - PumpSrv reservation extention flags
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   13:44		Start
******************************************************************************/

void CServerInfo::SetServerReservation(long lVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);
	m_lLockResExt = lVal;
	

}
/******************************************************************************
 Description:	Get server reservation flags 	
 Returns:       None	
 Parameters:   	lVal - PumpSrv reservation extention flags
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   13:44		Start
******************************************************************************/

long CServerInfo::GetServerReservation()
{
	long lTmpVal =0 ;
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	lTmpVal = m_lLockResExt;

	

	return lTmpVal;

}
/******************************************************************************
 Description:	Change flags shift process	
 Returns:       None	
 Parameters:   	bVal - TRUE/ FALSE
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   13:44		Start
******************************************************************************/

void CServerInfo::SetRequiredShiftProcessFlags(BOOL bVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_bRequiredShiftProcess = (BYTE)bVal;

	
}
/******************************************************************************
 Description:	Get flags shift process	
 Returns:       bVal - TRUE/ FALSE	
 Parameters:   	None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   13:44		Start
******************************************************************************/
BOOL CServerInfo::GetRequiredShiftProcessFlags()
{
	BOOL bTmpVal;
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bTmpVal = m_bRequiredShiftProcess;

	

	return bTmpVal;
}
/******************************************************************************
 Description:	Change state machine of process shift.	
 Returns:      	None 
 Parameters:   	eShiftChangeState = New state.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   14:14		Start
******************************************************************************/

void CServerInfo::SetShiftChangeStatus(ChangeState eShiftChangeState)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_eShiftChangeState = eShiftChangeState;

	
}
/******************************************************************************
 Description:	Get state machine of process shift.	
 Returns:      	eShiftChangeState = New state.
 Parameters:   	None. 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   14:14		Start
******************************************************************************/
ChangeState CServerInfo::GetShiftChangeStatus()
{
	ChangeState eTmpChangeState;
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	eTmpChangeState= m_eShiftChangeState;

	

	return eTmpChangeState;
}
/******************************************************************************
 Description:	Update Price change flags, if this flags is set to TRUE then price change process will be started.    
 Returns:      	None.
 Parameters:   	bVal -	TRUE    - Start Price change process.
						FALSE	- Stop price change process
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   14:14		Start
******************************************************************************/
void CServerInfo::SetRequiredPriceChangeFlags(BOOL bVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_bRequiredPriceChange = (BYTE)bVal;

	
	
}
/******************************************************************************
 Description:	Get price change process flags

 Returns:      	TRUE	- Price change active
				FALSE   - Price change not active
 Parameters:   	None					
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   14:14		Start
******************************************************************************/
BOOL CServerInfo::GetRequiredPriceChangeFlags()
{
	BOOL bTmpVal;
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bTmpVal = m_bRequiredPriceChange; 

	

	return bTmpVal;
}
/******************************************************************************
 Description:	Set flag, to lock forecourt when price change is running
 Returns:      	None.
 Parameters:   	bVal -	TRUE    - Lock forecourt before price change done
						FALSE	-   
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   14:14		Start
******************************************************************************/

void CServerInfo::SetLockForecourtOnPriceChangeFlags(BOOL bVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_bPriceChangeLock = (BYTE)bVal;

	
	
}
/******************************************************************************
 Description:	Get flag status if need to lock forecourt on Price change
 Returns:      	None.
 Parameters:   	bVal -	TRUE    - Lock forecourt before price change done
						FALSE	-   
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   14:14		Start
******************************************************************************/
BOOL CServerInfo::GetLockForecourtOnPriceChangeFlags()
{
	BOOL bVal;
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bVal = (BYTE)m_bPriceChangeLock;

	

	return bVal;

}
/******************************************************************************
 Description:	Set flag lock forecourt on start shift
 Returns:      	None.
 Parameters:   	bVal -	TRUE    - Lock forecourt on shift process
						FALSE	- shift without lock  
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   14:14		Start
******************************************************************************/

void CServerInfo::SetShiftLockFlags(BOOL bVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_bShiftChangeLock = (BYTE)bVal;

	

}
/******************************************************************************
 Description:	Get flag required shift lock.
 Returns:      	None.
 Parameters:   	bVal -	TRUE    - Lock forecourt on shift process
						FALSE	- shift without lock  
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   14:14		Start
*****************************************************************************/
BOOL CServerInfo::GetShiftLockFlags()
{
	BOOL bVal;

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bVal =  (BYTE)m_bShiftChangeLock; 

	

	return bVal;
}
/******************************************************************************
 Description:	Start/stop Mode change process 	
 Returns:      	None
 Parameters:   	TRUE  - Start mode change process. 
				FALSE - process not active. 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

void CServerInfo::SetRequiredModeProcessFlags(BOOL bVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_bRequiredModeProcess = (BYTE)bVal;
}
/******************************************************************************
 Description:	Get Mode change process status 	
 Returns:      	None
 Parameters:   	TRUE  - Change mode process is runing
				FALSE - process not active.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

BOOL CServerInfo::GetRequiredModeProcessFlags()
{
	BOOL bVal;

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bVal =  (BYTE)m_bRequiredModeProcess; 

	

	return bVal;
}
/******************************************************************************
 Description:	Start/stop Maintenance Process	
 Returns:      	None
 Parameters:   	TRUE  - Start Maintenance Process. 
				FALSE - Maintenance Process not active. 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/
void CServerInfo::SetRequiredMaintenanceProcessFlags(BOOL bVal)
{	
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_byRequiredMaintenanceProcess = (BYTE)bVal;

	
}

//4.0.14.502
void CServerInfo::SetLockByUpsFlags(BOOL bVal)
{	
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_byLockByUps = (BYTE)bVal;

	
}

/******************************************************************************
 Description:	Get Maintenance Process flags	
 Returns:   	TRUE  - Start Maintenance Process. 
				FALSE - Maintenance Process not active. 
 
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

BOOL CServerInfo::GetRequiredMaintenanceProcessFlags()
{
	BOOL bVal = FALSE;

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bVal =  (BYTE)m_byRequiredMaintenanceProcess; 

	return bVal;

}

//4.0.14.502
BOOL CServerInfo::GetLockByUpsFlags()
{
	BOOL bVal = FALSE;

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bVal =  (BYTE)m_byLockByUps; 

	

	return bVal;

}

/******************************************************************************
 Description:	Update wet stock flags needs 
 Returns:   	None
 Parameters:    lVal  - Wet stock flags
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

//void CServerInfo::SetWetStockFlags(BYTE byVal) //4.0.9.503
void CServerInfo::SetWetStockFlags(long lVal) //4.0.9.503
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	//4.0.9.503 m_byWetStockFlags = byVal;
	m_lWetStockFlags = lVal; //4.0.9.503


	

}

/******************************************************************************
 Description:	Get Wet stock flags 
 Returns:   	lVal  - Wet stock flags
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

//4.0.9.503 BYTE CServerInfo::GetWetStockFlags()
long CServerInfo::GetWetStockFlags()
{
	long lVal = 0; //4.0.9.503

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	//byVal =  m_byWetStockFlags; //4.0.9.503
	lVal =  m_lWetStockFlags;


	

	return lVal;

}

/******************************************************************************
 Description:	Set memo process flags , which used to read wet stock data to PumpSrv memory 
 Returns:   	None
 Parameters:    byVal  - 1 - memo process is runing 
						 0 - memo process isn't runing 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

void CServerInfo::SetMemoProcessFlags(BOOL bVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_byMemoInProcess = (BYTE)bVal;

	

}

/******************************************************************************
 Description:	Get memo process flags  
 Returns:   	byVal  - 1 - active
						 0 - not active
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

BOOL CServerInfo::GetMemoProcessFlags()
{
	BOOL bVal = 0;

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bVal =  m_byMemoInProcess;

	

	return bVal;

}

/******************************************************************************
 Description:	Set debit shift process
 Returns:   	None
 Parameters:    byVal  - 1 - active
						 0 - not active
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

void CServerInfo::SetDebitShiftProcessFlags(BYTE byVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_byDebitShiftInProcess = (BYTE)byVal;

	

}

/******************************************************************************
 Description:	Get debit shift process flags
 Returns:   	byVal  - 1 - active
						 0 - not active 
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

BYTE CServerInfo::GetDebitShiftProcessFlags()
{
	BYTE byVal = 0;

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	byVal =  m_byDebitShiftInProcess;

	

	return byVal;

}

/******************************************************************************
 Description:	Set flags to start shift ignoring OLA check.  
 Returns:   	None
 Parameters:    byVal  - 1 - active
						 0 - not active
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

void CServerInfo::SetForceShiftOLAFlags(BYTE byVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_byFoceShiftOLA = (BYTE)byVal;

	

}

/******************************************************************************
 Description:	Get flags shift ignoring OLA check.
 Returns:   	byVal  - 1 - active
						 0 - not active 
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

BYTE CServerInfo::GetForceShiftOLAFlags()
{
	BYTE byVal = 0;

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	byVal =  m_byFoceShiftOLA;

	

	return byVal;

}


/******************************************************************************
 Description:	Set Delivery emulation new status.  
 Returns:   	None
 Parameters:    byVal  - Process status
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/
void CServerInfo::SetDeliveryEmulationStatus(BYTE byVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString strLog;
		strLog.Format("Set delivery emulation status new: %d, old: %d Delivery Process: %d.", 
			byVal , m_byDeliveryEmulationStatus , m_byDeliveryEmulationProcess);
		_LOGMSG.LogMsg(strLog);
	}

	m_byDeliveryEmulationStatus = (BYTE)byVal;

	

}

/******************************************************************************
 Description:	Get Delivery emulation new status.  
 Returns:   	None
 Parameters:    byVal  - Process status
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/
BYTE CServerInfo::GetDeliveryEmulationStatus()
{
	BYTE byVal = 0;

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	byVal =  m_byDeliveryEmulationStatus; //4.0.13.505

	

	return byVal;

}

/******************************************************************************
 Description:	Set flags to start shift ignoring OLA check.  
 Returns:   	None
 Parameters:    byVal  - 1 - active
						 0 - not active
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/
void CServerInfo::SetDeliveryEmulationProcessType(BYTE byVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString strLog;
		strLog.Format("Set delivery emulation process type new: %d, old: %d Delivery Status: %d.", 
			byVal, m_byDeliveryEmulationProcess, m_byDeliveryEmulationStatus);
		_LOGMSG.LogMsg(strLog);
	}

	m_byDeliveryEmulationProcess = (BYTE)byVal;

	

}

/******************************************************************************
 Description:	Get flags shift ignoring OLA check.
 Returns:   	byVal  - 1 - active
						 0 - not active 
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/1/2005   15:13		Start
******************************************************************************/

BYTE CServerInfo::GetDeliveryEmulationProcessType()
{
	BYTE byVal = 0;

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	byVal =  m_byDeliveryEmulationProcess;

	

	return byVal;

}
/******************************************************************************
 Description:	Clear tank  reading  flags 	
 Returns:      	None
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/1/2005   8:18		Start
******************************************************************************/
void CServerInfo::ClearTankReadingFlags()
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_lWetStockFlags &=  WET_STOCK_CLEAR_TANKS;  //4.0.9.503


	// Clear memo flags only if it is only tank reading
	//4.0.16.500
	if ((!(m_lWetStockFlags & WET_STOCK_DELIVERY)) && (!(m_lWetStockFlags & WET_STOCK_PUMP_TOTALS))) //4.0.14.400
	{
		if (m_lWetStockFlags & WET_STOCK_REQUESTED_BY_POS_PUMP)
			m_lWetStockFlags &=  WET_STOCK_CLEAR_REQUESTED_BY_POS_PUMP;

		m_byMemoInProcess = FALSE;	
	}
	

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString strLog;
		strLog.Format("Server Info Delivery Status: %d, Delivery Process: %d.", m_byDeliveryEmulationStatus, m_byDeliveryEmulationProcess);
		_LOGMSG.LogMsg(strLog);

	}				

	_Module.m_server.ClearWetStockProcess(m_lWetStockFlags);

	

}
/******************************************************************************
 Description:	Reset all wet stock flags 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   10:30		Start
******************************************************************************/
void CServerInfo::ClearAllWetStockFlags()
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_lWetStockFlags &=  WET_STOCK_CLEAR_All; //4.0.9.503

	
	m_byMemoInProcess = FALSE;	
	

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString strLog;
		strLog.Format("Clear All wet stock flags Status: %d, Delivery Process: %d.", m_byDeliveryEmulationStatus, m_byDeliveryEmulationProcess);
		_LOGMSG.LogMsg(strLog);

	}				

	_Module.m_server.ClearWetStockProcess(m_lWetStockFlags);

	


}
/******************************************************************************
 Description:	Clear wet stock Pump total flags 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   10:29		Start
******************************************************************************/

void CServerInfo::ClearPumpTotalsFlags()
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_lWetStockFlags &=  WET_STOCK_CLEAR_PUMP_TOTALS; //4.0.9.503

	//4.0.16.500		
	if ((!(m_lWetStockFlags & WET_STOCK_DELIVERY)) && (!(m_lWetStockFlags & WET_STOCK_TANKS)) && (!(m_lWetStockFlags & WET_STOCK_PUMP_TOTALS))) //4.0.14.400
	{
		if (m_lWetStockFlags & WET_STOCK_REQUESTED_BY_POS_PUMP)
			m_lWetStockFlags &=  WET_STOCK_CLEAR_REQUESTED_BY_POS_PUMP;

		m_byMemoInProcess = FALSE;
	}
	
	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString strLog;
		strLog.Format("Clear Pump totals flags , delivery emulation  Status: %d, Delivery Process: %d.", m_byDeliveryEmulationStatus, m_byDeliveryEmulationProcess);
		_LOGMSG.LogMsg(strLog);

	}				

	_Module.m_server.ClearWetStockProcess(m_lWetStockFlags);

	
}
/******************************************************************************
 Description:	Clear Wet stock delivery report flags 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   10:29		Start
******************************************************************************/

void CServerInfo::ClearDeliveryReportFlags()
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	//4.0.9.503 m_byWetStockFlags &=  WET_STOCK_CLEAR_DELIVERY;
	m_lWetStockFlags &=  WET_STOCK_CLEAR_DELIVERY;

	
	
	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString strLog;
		strLog.Format("Clear Delivery report flags , delivery emulation  Status: %d, Delivery Process: %d.", m_byDeliveryEmulationStatus, m_byDeliveryEmulationProcess);
		_LOGMSG.LogMsg(strLog);

	}				
	//4.0.9.503_Module.m_server.ClearWetStockProcess(m_byWetStockFlags);
	_Module.m_server.ClearWetStockProcess(m_lWetStockFlags); //4.0.9.503


	
}
/******************************************************************************
 Description:	Clear alarm flags 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   10:29		Start
******************************************************************************/

void CServerInfo::ClearAlarmFlags()
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	//4.0.9.503 m_byWetStockFlags &=  WET_STOCK_CLEAR_ALARM;
	m_lWetStockFlags &=  WET_STOCK_CLEAR_ALARM;  //4.0.9.503

	
	
	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString strLog;
		strLog.Format("Clear alarm flags , delivery emulation  Status: %d, Delivery Process: %d.", m_byDeliveryEmulationStatus, m_byDeliveryEmulationProcess);
		_LOGMSG.LogMsg(strLog);

	}				
	//4.0.9.503_Module.m_server.ClearWetStockProcess(m_byWetStockFlags);
	_Module.m_server.ClearWetStockProcess(m_lWetStockFlags);


	
}
/******************************************************************************
 Description:	Update Server information parameters in one shot
 Returns:      	
 Parameters:   	SERVER_INFO   rec - server info details
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			06/02/2005 10:20		Start
 AmitH			06/10/2008 16:00		Add support for single tank information (Delivery emulation and lock) //4.0.21.500 - TD 66935
******************************************************************************/

long CServerInfo::SetInfo(SERVER_INFO &rec)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);	
	m_eLockStatus					= rec.lockStatus;
	m_lLockResExt					= rec.lockResExt;
	m_bRequiredShiftProcess			= rec.bRequiredShiftProcess;
	m_eShiftChangeState				= rec.lShiftChangeState ;
	m_bRequiredPriceChange			= rec.bRequiredPriceChange;
	m_lPriceChangeState				= rec.lPriceChangeState ;
	m_bRequiredModeProcess			= rec.bRequiredModeProcess ;
	m_byMemoInProcess				= rec.byMemoInProcess ;
  //4.0.9.503m_byWetStockFlags		= rec.byWetStockFlags;
	m_lWetStockFlags				= rec.lWetStockFlags; //4.0.9.503
	m_byDebitShiftInProcess			= rec.byDebitShiftInProcess ;
	m_byRequiredMaintenanceProcess	= rec.byRequiredMaintenanceProcess;
	m_byLockByUps					= rec.byLockByUps; //4.0.14.502
	m_bInStopAllPumps				= rec.bInStopAllPumps; //4.0.13.80
	m_bPriceChangeLock				= rec.bPriceChangeLock;
	m_bShiftChangeLock				= rec.bShiftChangeLock; 
	m_byFoceShiftOLA				= rec.byFoceShiftOLA ;
	m_byDeliveryEmulationStatus		= rec.byDeliveryEmulationStatus;
	m_byDeliveryEmulationProcess	= rec.byDeliveryEmulationProcess ;
	m_bInPriceChangeProcess			= rec.bInPriceChangeProcess; //4.0.12.515
	m_bSendMemoPumpEvent			= rec.bSendMemoPumpEvent;
	m_bRequiredQDXMaintenance		= rec.bRequiredQDXMaintenance; //4.0.18.20

	for (int i = 1; i<= MAX_TANKS; i++ ) //4.0.21.500 - TD 66935
	{
		m_cDeliveryEmulationSingleTankServerInfo.SetDeliveryEmulationSingleTankStatus(i, rec.byDeliveryEmulationSingleTankStatus[i-1]);
        m_cDeliveryEmulationSingleTankServerInfo.SetDeliveryEmulationSingleTankProcessType(i, ProcessDeliverySingleTank(rec.byDeliveryEmulationSingleTankProcess[i-1]));
        m_cDeliveryEmulationSingleTankServerInfo.SetDeliveryEmulationSingleTankSuccess(i, rec.byDeliveryEmulationSingleTankSuccess[i-1]);
        m_cDeliveryEmulationSingleTankServerInfo.SetDeliveryEmulationSingleTankProcessStopped(i, SingleTankProcessStopped(rec.byDeliveryEmulationSingleTankProcessStopped[i-1]));
        m_cDeliveryEmulationSingleTankServerInfo.SetSingleTankLockStatus(i, SingleTankLockStatus(rec.lSingleTankLockStatus[i-1]));
	}

	//4.0.22.501 - TD 69905
	m_cReportGenerator.SetReportRequestState((ReportRequestState)a2l(&rec.byReportRequestState, sizeof(rec.byReportRequestState)), "SetInfo");       // TD 351225
	m_cReportGenerator.SetReportRequestFailureResult((ReportRequestError)a2l(&rec.byReportRequestFailureResult, sizeof(rec.byReportRequestFailureResult)), "SetInfo");  // TD 351225

	//4.0.22.503 - TD 72189
	m_cChangeProductInTankManager.SetProductInTankState(a2l(&rec.byChangeProductInTankState, sizeof(rec.byChangeProductInTankState)));	//4.0.27.20
	m_cChangeProductInTankManager.SetFailureResult(a2l(&rec.byChangeProductInTankFailureResult, sizeof(rec.byChangeProductInTankFailureResult)));

	m_byInPriceChangeWithIntegrityProcessFlags = rec.byInPriceChangeWithIntegrityProcessFlags;	//4.0.27.20
	SetPriceChangeResponseFromIntegritiyProcessFlag();

	

	return 0;
}
/******************************************************************************
 Description:	Get Server information parameters in one shot
 Returns:      	
 Parameters:   	SERVER_INFO   rec - return the server info details
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			06/02/2005 10:20		Start
 AmitH			06/10/2008 16:00		Add support for single tank information (Delivery emulation and lock) 4.0.21.500 - TD 66935
******************************************************************************/

long CServerInfo::GetInfo(SERVER_INFO &rec)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);	
	rec.lockStatus           = m_eLockStatus ;
	rec.lockResExt           = m_lLockResExt ;
	rec.bRequiredShiftProcess= m_bRequiredShiftProcess;
	rec.lShiftChangeState    = m_eShiftChangeState;
	rec.bRequiredPriceChange = m_bRequiredPriceChange;
	rec.lPriceChangeState    = m_lPriceChangeState;
	rec.bRequiredModeProcess = m_bRequiredModeProcess;
	rec.byMemoInProcess      = m_byMemoInProcess;
  //4.0.9.503rec.byWetStockFlags = m_byWetStockFlags;
	rec.lWetStockFlags = m_lWetStockFlags; //4.0.9.503

	rec.byDebitShiftInProcess       = m_byDebitShiftInProcess;
	rec.byRequiredMaintenanceProcess= m_byRequiredMaintenanceProcess;
	rec.byLockByUps                 = m_byLockByUps; //4.0.14.502
	rec.bInStopAllPumps             = m_bInStopAllPumps;
	rec.bPriceChangeLock	        = m_bPriceChangeLock;
	rec.bShiftChangeLock            = m_bShiftChangeLock;
	rec.byFoceShiftOLA              = m_byFoceShiftOLA;
	rec.byDeliveryEmulationStatus   = m_byDeliveryEmulationStatus;
	rec.byDeliveryEmulationProcess  = m_byDeliveryEmulationProcess;
	rec.bInPriceChangeProcess       = m_bInPriceChangeProcess;  //4.0.12.515
	rec.bSendMemoPumpEvent          = m_bSendMemoPumpEvent;     //4.0.15.182
	rec.bRequiredQDXMaintenance     = m_bRequiredQDXMaintenance;//4.0.18.20	

	for (int i = 1; i<= MAX_TANKS; i++ ) //4.0.21.500 - TD 66935
	{
		rec.byDeliveryEmulationSingleTankStatus[i-1] = m_cDeliveryEmulationSingleTankServerInfo.GetDeliveryEmulationSingleTankStatus(i);
		rec.byDeliveryEmulationSingleTankProcess[i-1]= m_cDeliveryEmulationSingleTankServerInfo.GetDeliveryEmulationSingleTankProcessType(i);
		rec.byDeliveryEmulationSingleTankSuccess[i-1]= m_cDeliveryEmulationSingleTankServerInfo.GetDeliveryEmulationSingleTankSuccess(i);
		rec.byDeliveryEmulationSingleTankProcessStopped[i-1] = m_cDeliveryEmulationSingleTankServerInfo.GetDeliveryEmulationSingleTankProcessStopped(i);
		rec.lSingleTankLockStatus[i-1]               = m_cDeliveryEmulationSingleTankServerInfo.GetSingleTankLockStatus(i);
	}

	//4.0.22.501 - TD 69905
	l2a(m_cReportGenerator.GetReportRequestState(), &rec.byReportRequestState, sizeof(rec.byReportRequestState));
	l2a(m_cReportGenerator.GetReportRequestFailureResult(), &rec.byReportRequestFailureResult, sizeof(rec.byReportRequestFailureResult));

	//4.0.22.503 - TD 72189
	l2a(m_cChangeProductInTankManager.GetState(), &rec.byChangeProductInTankState, sizeof(rec.byChangeProductInTankState));
	l2a(m_cChangeProductInTankManager.GetFailureResult(), &rec.byChangeProductInTankFailureResult, sizeof(rec.byChangeProductInTankFailureResult));
	
	rec.byInPriceChangeWithIntegrityProcessFlags = m_byInPriceChangeWithIntegrityProcessFlags; //4.0.27.20 147183
	
	

	return 0;
}

void CServerInfo::Lock()
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);
}

void CServerInfo::UnLock()
{
	
}

void CServerInfo::SetPriceChangeState(ChangeState lState)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_lPriceChangeState = lState;

	
}

ChangeState CServerInfo::GetPriceChangeState()
{
	ChangeState eState;
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	eState = m_lPriceChangeState;

	

	return eState;
}
/******************************************************************************
 Description:	Clear shift flags and wait to next shift process 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/1/2005   12:35		Start
******************************************************************************/

void CServerInfo::ClearShiftFlags()
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);
	m_byRequiredMaintenanceProcess = MP_NONE;
	m_byLockByUps = MP_NONE; //4.0.14.502
	m_lLockResExt = 0 ;
	m_bRequiredPriceChange =(BYTE)FALSE;
	m_bInStopAllPumps = (BYTE)FALSE; //4.0.13.80

	

}
/******************************************************************************
 Description:	Reset Devlivery emulation flags. 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   10:19		Start
******************************************************************************/
void CServerInfo::ClearDeliveryEmulationFlags()
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_byDeliveryEmulationStatus  = DES_IDLE; 
	m_byDeliveryEmulationProcess = PD_NONE;
	
	


}
/******************************************************************************
 Description:	Reset all Server info parameters 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/2/2005   10:18		Start
******************************************************************************/
void CServerInfo::ResetServerInfo()
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);	
	m_eLockStatus           = UNLOCK;
	m_lLockResExt           = 0;
	m_bRequiredShiftProcess = FALSE;
	m_eShiftChangeState     = CHANGE_DONE;
	m_bRequiredPriceChange  = 0;
	m_lPriceChangeState     = CHANGE_DONE;
	m_bRequiredModeProcess  = 0;
	
	m_byMemoInProcess= FALSE;//4.0.1.32
 // Change Parameter m_byWetStockFlags =0; //4.0.1.32 4.0.9.503
	m_lWetStockFlags               = 0; //4.0.9.503

	m_byDebitShiftInProcess        = 0; //4.0.1.32
	m_byRequiredMaintenanceProcess = 0; //4.0.1.32
	m_byLockByUps                  = 0; //4.0.14.502
	m_bInStopAllPumps              = 0; //4.0.13.80

	m_byWaitForPriceChangeAck      = 0; //4.0.9.555

	m_bInPriceChangeProcess        = 0; //4.0.12.515
	m_bRequiredQDXMaintenance      = 0; //4.0.18.20

	ClearSingleTankFlags(); //4.0.21.500 - TD 66935

	m_byInPriceChangeWithIntegrityProcessFlags = PRICE_CHANGE_WITH_INTEGRITY_NONE;  //4.0.27.20

	
}

BOOL CServerInfo::GetWaitForPriceChangeAckFlags()
{
	return m_byWaitForPriceChangeAck;	
}

void CServerInfo::SetWaitForPriceChangeAckFlags(BOOL bVal, BOOL bMarkActivePump)
{
	if (_Module.m_server.GetInformPriceChangeFlags())
	{

		CSingleLock slServerInfo(&m_csServerInfo, TRUE);	
		m_byWaitForPriceChangeAck = (char)bVal;

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString strLog;
			strLog.Format("SetWaitForPriceChangeAckFlags Active=%d", bVal);
			_LOGMSG.LogMsg(strLog);
		}

		if (bMarkActivePump)
		{
			// mark all pumps, as waiting to ACK

			// QC 501964
			long lMaxGrades = _Module.m_server.GetMaxValidGradeNumber();

			for (int j = 0; j < lMaxGrades; j++) 
			{
				if (_Module.m_server.m_GradeData.GetGradePriceChanged(j + 1))
				{
					for (int i = 0; i < _Module.m_server.GetMaxPumps(); i++)
					{
						BYTE byNozzle = _Module.m_server.m_cPumpArray[i].GetGradePosition(j + 1);
						if (byNozzle)
						{
							_Module.m_server.m_cPumpArray[i].SetPriceChangeStatus(0, TRUE);

							if (LOG_BASIC_FLOW_CONTROL)
							{
								CString strLog;
								strLog.Format("SetPriceChangeStatus() for Pump %02ld and Grade = %02ld", i + 1, j + 1);
								_Module.m_server.m_cLogger.LogMsg(strLog);
							}
						}
					}
				}
			}
		}
		
		if (bVal) 
		{
			_Module.m_server.m_cPriceChangeFlags.ResetInformPriceCounter();   //4.0.10.811	
		}
		else 
		{
			// QC 497608
			_Module.m_server.ResetPriceChangedFlags();
		}
	}
}

//4.0.12.515
void CServerInfo::SetInPriceChangeProcess(BOOL bVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_bInPriceChangeProcess = (BYTE)bVal;

	
	
}

//4.0.12.515
BOOL CServerInfo::GetInPriceChangeProcess()
{
	BOOL bVal;
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bVal = (BYTE)m_bInPriceChangeProcess;

	

	return bVal;

}

//4.0.13.80
void CServerInfo::SetInStopAllPumps(BOOL bVal)
{	
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_bInStopAllPumps = (BYTE)bVal;

	
}

//4.0.13.80
BOOL CServerInfo::GetInStopAllPumps()
{
	BOOL bVal = FALSE;

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bVal =  (BYTE)m_bInStopAllPumps; 

	

	return bVal;

}

void CServerInfo::SetSendMemoPumpEvent(const BOOL bVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_bSendMemoPumpEvent = bVal;

	
}

BOOL CServerInfo::GetSendMemoPumpEvent()
{
	BOOL bVal = FALSE;

	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bVal =  (BYTE)m_bSendMemoPumpEvent; 

	

	return bVal;

}

//4.0.18.20 - TD 33898
BOOL CServerInfo::GetRequiredQDXMaintenance()
{
	BOOL bVal = FALSE;
	
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	bVal =  m_bRequiredQDXMaintenance; 

	

	return bVal;
}

//4.0.18.20 - TD 33898
void CServerInfo::SetRequiredQDXMaintenance(BOOL bVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_bRequiredQDXMaintenance = bVal;

	
}


/******************************************************************************
 Description:	Set Single tank 's Delivery emulation new status.  
 Returns:   	None
 Parameters:    Tank number, byVal  - Process status (DES_SINGLE_TANK_IDLE,
                                                      DES_SINGLE_TANK_START_LOCK,
													  DES_SINGLE_TANK_WAIT_FOR_LOCK,
													  DES_SINGLE_TANK_WAIT_TIMEOUT,
													  DES_SINGLE_TANK_REQUEST_TANK_READING,
													  DES_SINGLE_TANK_WAIT_TANK_READING,
													  DES_SINGLE_TANK_UNLOCK)
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH			06/10/2008   15:00		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CServerInfo::SetDeliveryEmulationSingleTankStatus(const long lTankNumber, const BYTE byVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_cDeliveryEmulationSingleTankServerInfo.SetDeliveryEmulationSingleTankStatus(lTankNumber, byVal);

	

}

/******************************************************************************
 Description:	Get Single Tank's Delivery emulation new status.  
 Returns:   	Process status (DES_SINGLE_TANK_IDLE,
                                DES_SINGLE_TANK_START_LOCK,
								DES_SINGLE_TANK_WAIT_FOR_LOCK,
								DES_SINGLE_TANK_WAIT_TIMEOUT,
								DES_SINGLE_TANK_REQUEST_TANK_READING,
								DES_SINGLE_TANK_WAIT_TANK_READING,
								DES_SINGLE_TANK_UNLOCK)
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH			06/10/2008   15:00		Start 4.0.21.500 - TD 66935
******************************************************************************/
BYTE CServerInfo::GetDeliveryEmulationSingleTankStatus(const long lTankNumber)
{
	return m_cDeliveryEmulationSingleTankServerInfo.GetDeliveryEmulationSingleTankStatus(lTankNumber);
}

/******************************************************************************
 Description:	Set Single Tank's Delivery emulation new process type.  
 Returns:   	None
 Parameters:    lTankNumber, byVal  - new process type (PD_SINGLE_TANK_NONE,					
		                                                PD_SINGLE_TANK_START,					     
		                                                PD_SINGLE_TANK_END,
		                                                PD_SINGLE_TANK_FORCE_RELEASE).
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH			06/10/2008   15:00		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CServerInfo::SetDeliveryEmulationSingleTankProcessType(const long lTankNumber, const ProcessDeliverySingleTank byVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_cDeliveryEmulationSingleTankServerInfo.SetDeliveryEmulationSingleTankProcessType(lTankNumber, byVal);

	

}
/******************************************************************************
 Description:	Get Single Tank's Delivery emulation new process type .  
 Returns:   	New process type (PD_SINGLE_TANK_NONE,					
		                          PD_SINGLE_TANK_START,					     
		                          PD_SINGLE_TANK_END,
		                          PD_SINGLE_TANK_FORCE_RELEASE).
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH			06/10/2008   15:00		Start 4.0.21.500 - TD 66935
******************************************************************************/
ProcessDeliverySingleTank CServerInfo::GetDeliveryEmulationSingleTankProcessType(const long lTankNumber)
{
	return m_cDeliveryEmulationSingleTankServerInfo.GetDeliveryEmulationSingleTankProcessType(lTankNumber);
}

/******************************************************************************
 Description:	Set Single Tank's Delivery emulation new success status.  
 Returns:   	None
 Parameters:    lTankNumber, byVal  - new success status (DESF_SINGLE_TANK_NOT_DONE, 
                                                          DESF_SINGLE_TANK_TIMEOUT,
                                                          DESF_SINGLE_TANK_FAILURE,
														  DESF_SINGLE_TANK_DONE).
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH			06/10/2008   15:00		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CServerInfo::SetDeliveryEmulationSingleTankSuccess(const long lTankNumber, const BYTE byVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_cDeliveryEmulationSingleTankServerInfo.SetDeliveryEmulationSingleTankSuccess(lTankNumber, byVal);

	

}
/******************************************************************************
 Description:	Get Single Tank's Delivery emulation success status.  
 Returns:   	Success status (DESF_SINGLE_TANK_NOT_DONE, 
                                DESF_SINGLE_TANK_TIMEOUT,
                                DESF_SINGLE_TANK_FAILURE,
					            DESF_SINGLE_TANK_DONE).
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH			06/10/2008   15:00		Start 4.0.21.500 - TD 66935
******************************************************************************/
BYTE CServerInfo::GetDeliveryEmulationSingleTankSuccess(const long lTankNumber)
{
	return m_cDeliveryEmulationSingleTankServerInfo.GetDeliveryEmulationSingleTankSuccess(lTankNumber);
}

/******************************************************************************
 Description:	Set Single Tank Delivery emulation new "Process Stopped" flag value.  
               (SINGLE_TANK_PROCESS_NOT_STOPPED,
                SINGLE_TANK_PROCESS_STOPPED_BY_ABORT,
                SINGLE_TANK_PROCESS_STOPPED_BY_TIMEOUT,
                SINGLE_TANK_PROCESS_STOPPED_BY_FAILURE)

 Returns:   	None
 Parameters:    lTankNumber, byVal  - new value
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			04/11/2008   12:15		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CServerInfo::SetDeliveryEmulationSingleTankProcessStopped(const long lTankNumber, const SingleTankProcessStopped byVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_cDeliveryEmulationSingleTankServerInfo.SetDeliveryEmulationSingleTankProcessStopped(lTankNumber, byVal);

	

}
/******************************************************************************
 Description:	Get Single Tank Delivery emulation "Process Stopped" flag value
               (SINGLE_TANK_PROCESS_NOT_STOPPED,
                SINGLE_TANK_PROCESS_STOPPED_BY_ABORT,
                SINGLE_TANK_PROCESS_STOPPED_BY_TIMEOUT,
                SINGLE_TANK_PROCESS_STOPPED_BY_FAILURE). 
 Returns:   	Single Tank's "Process Stopped" flag value.
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			04/11/2008   12:15		Start 4.0.21.500 - TD 66935
******************************************************************************/
SingleTankProcessStopped CServerInfo::GetDeliveryEmulationSingleTankProcessStopped(const long lTankNumber)
{									
	return m_cDeliveryEmulationSingleTankServerInfo.GetDeliveryEmulationSingleTankProcessStopped	(lTankNumber);
}

/******************************************************************************
 Description:	Set Single Tank's new lock status.  
 Returns:   	None
 Parameters:    lTankNumber, byVal  - new lock status
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH			06/10/2008   15:00		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CServerInfo::SetSingleTankLockStatus(const long lTankNumber, const SingleTankLockStatus byVal)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_cDeliveryEmulationSingleTankServerInfo.SetSingleTankLockStatus(lTankNumber, byVal);

	

}
/******************************************************************************
 Description:	Get Single Tank's new lock status.  
 Returns:   	New lock status.
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AmitH			06/10/2008   15:00		Start 4.0.21.500 - TD 66935
******************************************************************************/
SingleTankLockStatus CServerInfo::GetSingleTankLockStatus(const long lTankNumber)
{
	return m_cDeliveryEmulationSingleTankServerInfo.GetSingleTankLockStatus(lTankNumber);
}

/******************************************************************************
 Description:	Reset single tank flags (Delivery emulation and lock).
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
AmitH			06/10/2008   15:00		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CServerInfo::ClearSingleTankFlags()
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_cDeliveryEmulationSingleTankServerInfo.InitSingleTankFlags();
	
	
}

/******************************************************************************
 Description:	Reset single tank flags of a certain tank (Delivery emulation and lock).
 Returns:       None.	
 Parameters:   	Tank Number.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Shelly B		15/10/2008   16:00		Start 4.0.21.500 - TD 66935
******************************************************************************/
void CServerInfo::ClearSingleSpecificTankFlags(const long lTankNumber)
{
	CSingleLock slServerInfo(&m_csServerInfo, TRUE);

	m_cDeliveryEmulationSingleTankServerInfo.InitSingleSpecificTankFlags(lTankNumber);
	
	
}


/******************************************************************************
 Description:	Set report request process state
 Returns:   	None
 Parameters:    BYTE byVal - Process state (REPORT_IDLE,
											REPORT_WAIT_TANK_READING,
											REPORT_SEND_REQUEST,
											REPORT_WAIT_GENERATE_REPORT,
											REPORT_READY)
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			03/02/2009   16:00		Start	4.0.22.501 - TD 69905
                24/03/2013                                   TD 351225
******************************************************************************/
void CServerInfo::SetReportRequestState(const ReportRequestState eState, const char* const callerFunction)
{
	m_cReportGenerator.SetReportRequestState(eState, callerFunction);
}


/******************************************************************************
 Description:	Get report request process state
 Returns:   	short - process state
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			03/02/2009   16:00		Start	4.0.22.501 - TD 69905
                24/03/2013                                   TD 351225
******************************************************************************/
ReportRequestState CServerInfo::GetReportRequestState()
{
	return m_cReportGenerator.GetReportRequestState();
}


/******************************************************************************
 Description:	Set report request process failure result
 Returns:   	None
 Parameters:    BYTE byVal - Failure result (0 - success, other - failure) 

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			03/02/2009   16:00		Start	4.0.22.501 - TD 69905
                24/03/2013                                   TD 351225
******************************************************************************/
void CServerInfo::SetReportRequestFailureResult(const ReportRequestError eError, const char* const callerFunction)
{
	m_cReportGenerator.SetReportRequestFailureResult(eError, callerFunction);
}


/******************************************************************************
 Description:	Get report request process failure result
 Returns:   	long - process state
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			03/02/2009   16:00		Start	4.0.22.501 - TD 69905
******************************************************************************/
long CServerInfo::GetReportRequestFailureResult()
{
	return m_cReportGenerator.GetReportRequestFailureResult();
}

/******************************************************************************
 Description:	Handle generate report process
 Returns:   	None
 Parameters:    

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			11/02/2009   11:00		Start	4.0.22.501 - TD 69905
******************************************************************************/
void CServerInfo::HandleGenerateReportProcess(BOOL bInit, long lFlags, BOOL bReportDone)
{
	m_cReportGenerator.HandleGenerateReportProcess(bInit, lFlags, bReportDone);
}

/******************************************************************************
 Description:	Set report text
 Returns:   	None
 Parameters:    

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			11/02/2009   14:50		Start	4.0.22.501 - TD 69905
******************************************************************************/
void CServerInfo::SetReportBuffer(BSTR wszReportText)
{
	m_cReportGenerator.SetReportBuffer(wszReportText);
}

/******************************************************************************
 Description:	Get report buffer
 Returns:   	None
 Parameters:    

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			11/02/2009   15:38		Start	4.0.22.501 - TD 69905
******************************************************************************/
_bstr_t CServerInfo::GetReportBuffer()
{
	return m_cReportGenerator.GetReportBuffer();
}


/******************************************************************************
 Description:	Handle change product in tank setup request
 Returns:   	None
 Parameters:    

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   10:32		Start	4.0.22.503 - TD 72189
******************************************************************************/
long CServerInfo::HandleChangeProductInTankRequest(BSTR bstrRec, BOOL bSetupOnly)
{
	return m_cChangeProductInTankManager.StartProcess(bstrRec, bSetupOnly);
}


/******************************************************************************
 Description:	Handle Change product in tank process
 Returns:   	None
 Parameters:    

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   10:32		Start	4.0.22.503 - TD 72189
******************************************************************************/
void CServerInfo::HandleChangeProductInTankProcess()
{
	m_cChangeProductInTankManager.HandleProcess();
}

/******************************************************************************
 Description:	Set change product in tank process state
 Returns:   	None
 Parameters:    BYTE byVal - Process state (CPIT_IDLE,
											CPIT_START_LOCK_FORECOURT,
											CPIT_LOCK_FORECOURT_IN_PROGRESS,
											CPIT_FORECOURT_LOCKED,
											CPIT_WAIT_CLOSE_OPEN_TRS,
											CPIT_CHANGE_SETUP_AND_SEND_TO_CL,
											CPIT_WAIT_SEND_TO_CL
											CPIT_WAIT_CL_RESPONSE,
											CPIT_HANDLE_CL_RESPONSE,
											CPIT_SEND_PRICE_CHANGE,
											CPIT_WAIT_UNLOCK_FORECOURT,	)
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   18:21		Start	4.0.22.503 - TD 72189
******************************************************************************/
void CServerInfo::SetChangeProductInTankState(const long lState)
{
	m_cChangeProductInTankManager.SetState(lState);
}


/******************************************************************************
 Description:	Get change product in tank process state
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   18:23		Start	4.0.22.503 - TD 72189
******************************************************************************/
long CServerInfo::GetChangeProductInTankState()
{
	return m_cChangeProductInTankManager.GetState();
}


/******************************************************************************
 Description:	Set change product in tank process failure result
 Returns:   	None
 Parameters:    BYTE byVal - Failure result (0 - no error, other - failure) 

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   18:25		Start	4.0.22.503 - TD 72189
******************************************************************************/
void CServerInfo::SetChangeProductInTankFailureResult(const long lError)
{
	m_cChangeProductInTankManager.SetFailureResult(lError);
}

/******************************************************************************
 Description:	Get change product in tank process failure result
 Returns:   	long - process state
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   18:26		Start	4.0.22.503 - TD 72189
******************************************************************************/
long CServerInfo::GetChangeProductInTankFailureResult()
{
	return m_cChangeProductInTankManager.GetFailureResult();

}

/******************************************************************************
 Description:	Get change product in tank setup
 Returns:   	void
 Parameters:    BSTR *bstrData
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			26/02/2009   17:16		Start	4.0.22.503 - TD 72189
******************************************************************************/
void CServerInfo::GetChangeProductInTankInfo(BSTR *pbstrData)
{
	m_cChangeProductInTankManager.GetChangeProductInTankInfo(pbstrData);

}

/******************************************************************************
 Description:	Get the xml result buffer for change product in tank buffer
 Returns:   	None
 Parameters:    

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			02/03/2009   14:17		Start	4.0.22.503 - TD 72189
******************************************************************************/
void CServerInfo::GetChangeProductInTankXmlResultBuffer(BSTR *pbstrBuff)
{
	m_cChangeProductInTankManager.GetChangeProductInTankXmlResultBuffer(pbstrBuff);
}

/******************************************************************************
 Description:	Set tank id of for change product in tank process
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   18:23		Start	4.0.22.503 - TD 72189
******************************************************************************/
void CServerInfo::SetChangeProductTankId(long lTankNumber)
{
	m_cChangeProductInTankManager.SetTankId(lTankNumber);
}

/******************************************************************************
 Description:	Set grade id of for change product in tank process
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   18:23		Start	4.0.22.503 - TD 72189
******************************************************************************/
void CServerInfo::SetChangeProductGradeId(long lGradeNumber)
{
	m_cChangeProductInTankManager.SetTankId(lGradeNumber);
}

/******************************************************************************
 Description:	Set grade id of for change product in tank process
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   18:23		Start	4.0.22.503 - TD 72189
******************************************************************************/
void CServerInfo::SetChangeProductName(char * sProductName)
{
	m_cChangeProductInTankManager.SetProductName(sProductName);
}

/******************************************************************************
 Description:	Get the flag which indicates if price change with ack per pump
				process is running
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   14:00		Start		4.0.19.820 - TD 78775
******************************************************************************/
BOOL CServerInfo::GetInPriceChangeWithAckProcessFlag()
{
	return m_bInPriceChangeWithAckPerPumpProcess;
}

/******************************************************************************
 Description:	Set the flag which indicates that price change with ack per pump
				process is running
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   14:00		Start		4.0.19.820 - TD 78775
******************************************************************************/
void CServerInfo::SetInPriceChangeWithAckProcessFlag(BOOL bInPriceChangeWithAckProcess)
{
	m_bInPriceChangeWithAckPerPumpProcess = bInPriceChangeWithAckProcess;
}

/******************************************************************************
 Description:	Start the process of price change with ack per pump
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   16:00		Start		4.0.19.820 - TD 78775
******************************************************************************/
void CServerInfo::StartPriceChangeWithAckPerPumpProcess()
{
	m_cPriceChangeWithAckPerPump.StartProcess();
}

/******************************************************************************
 Description:	Set the price change result per pump
 Returns:   	
 Parameters:    long lPumpNumber, long lErrorCode
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   16:00		Start		4.0.19.820 - TD 78775
******************************************************************************/
void CServerInfo::SetPriceChangeAckResult(long lPumpNumber, long lErrorCode)
{
	if (GetInPriceChangeWithAckProcessFlag())
		m_cPriceChangeWithAckPerPump.SetAckPerPumpProduct(lPumpNumber, lErrorCode);
}


/******************************************************************************
 Description:	Get price change with ack process failure result
 Returns:   	long - process result
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   17:40		Start	4.0.19.820 - TD 78775
******************************************************************************/
long CServerInfo::GetPriceChangeWithAckFailureResult()
{
	return m_cPriceChangeWithAckPerPump.GetFailureResult();
}

/******************************************************************************
 Description:	Get price change with ack process state
 Returns:   	long - process state
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   17:40		Start	4.0.19.820 - TD 78775
******************************************************************************/
long CServerInfo::GetPriceChangeWithAckState()
{
	return m_cPriceChangeWithAckPerPump.GetState();
}

/******************************************************************************
 Description:	Get the xml result buffer for price change with ack 
				per pump process
				
 Returns:   	None
 Parameters:    

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			21/10/2009   09:30		Start	4.0.19.820 - TD 78775
******************************************************************************/
void CServerInfo::GetPriceChangeWithAckXmlResultBuffer(BSTR *pbstrBuff)
{
	m_cPriceChangeWithAckPerPump.GetXmlResultBuffer(pbstrBuff);
}


/******************************************************************************
 Description:	Check if price change with ack results (Ack/Nack) 
				for all pumps were received from CL
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			21/10/2009   09:30		Start	
******************************************************************************/	
BOOL CServerInfo::IsPriceChangeWithAckReceivedForAllPumps()
{
	return m_cPriceChangeWithAckPerPump.IsAckReceivedForAllPumps();
}

/******************************************************************************
 Description:	Set the reference number of the price change with ack process
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			21/10/2009   11:30		Start		4.0.19.820 - TD 78775
******************************************************************************/
void CServerInfo::SetPriceChangeWithAckRefNumber(long lRefNumber)
{
	m_cPriceChangeWithAckPerPump.SetRefNumber(lRefNumber);
}

/******************************************************************************
 Description:	Handle price change with ack per pump process
 Returns:   	None
 Parameters:    

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			21/10/2009   12:02		Start	4.0.19.820 - TD 78775
******************************************************************************/
void CServerInfo::HandlePriceChangeWithAckPerPumpProcess()
{
	m_cPriceChangeWithAckPerPump.HandleProcess();
}

/******************************************************************************
 Description:	Get the flag which indicates if price change with integrity is running
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			06/11/2010   10:00		Start		4.0.26.508 147183
 Shuki		    13/02/2011				return byte //4.0.27.20 
******************************************************************************/
BYTE CServerInfo::GetInPriceChangeWithIntegrityProcessFlags()
{
	return m_byInPriceChangeWithIntegrityProcessFlags;
}

/******************************************************************************
 Description:	Set the flag which indicates that price change with integrity is running
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			06/11/2010   10:00		Start		4.0.26.508 147183
 Shuki		    13/02/2011				Set byte    //4.0.27.20
******************************************************************************/
void CServerInfo::SetInPriceChangeWithIntegrityProcessFlags(BYTE byPriceChangeWithIntegrityFlags)
{
	m_byInPriceChangeWithIntegrityProcessFlags = byPriceChangeWithIntegrityFlags;
}

/******************************************************************************
 Description:	return if we are in commit price change when integrity flag turn on 
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shuki			13/02/2011   18:00		Start		4.0.27.20 147183
******************************************************************************/
BOOL CServerInfo::IsInCommitPriceChangeWithIntegrityProcess()
{
	return (m_byInPriceChangeWithIntegrityProcessFlags & (~PRICE_CHANGE_WITH_INTEGRITY_ACTIVE & PRICE_CHANGE_WITH_INTEGRITY_GOT_COMMIT)); //(!PRICE_CHANGE_WITH_INTEGRITY_ACTIVE & (PRICE_CHANGE_WITH_INTEGRITY_PENDING_COMMIT | PRICE_CHANGE_WITH_INTEGRITY_PENDING_COMMIT_AFTER_FAILURE)));
}

/******************************************************************************
 Description:	return if we are in price change when integrity flag turn on 
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shuki			13/02/2011   18:00		Start		4.0.27.20 147183
******************************************************************************/
BOOL CServerInfo::IsInPriceChangeWithIntegrityProcess()
{
	return (m_byInPriceChangeWithIntegrityProcessFlags & PRICE_CHANGE_WITH_INTEGRITY_ACTIVE); 
}

/******************************************************************************
 Description:	return if we are pending commit integrity 
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shuki			13/02/2011   18:00		Start		4.0.27.20 147183
******************************************************************************/
BOOL CServerInfo::IsPendingCommitPriceChangeWithIntegrityProcess()
{
	return (m_byInPriceChangeWithIntegrityProcessFlags & (~PRICE_CHANGE_WITH_INTEGRITY_ACTIVE & (PRICE_CHANGE_WITH_INTEGRITY_PENDING_COMMIT | PRICE_CHANGE_WITH_INTEGRITY_PENDING_COMMIT_AFTER_FAILURE)));
}
/******************************************************************************
 Description:	Convert from server info flags to server main member
 Returns:   	
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shuki			13/02/2011   18:00		Start		4.0.27.20 147183
******************************************************************************/
void CServerInfo::SetPriceChangeResponseFromIntegritiyProcessFlag()
{
	PriceChangeResponse pcrResponce;

	pcrResponce = PRICE_CHANGE_NO_RESPONSE;

	if(m_byInPriceChangeWithIntegrityProcessFlags & (~PRICE_CHANGE_WITH_INTEGRITY_ACTIVE & PRICE_CHANGE_WITH_INTEGRITY_PENDING_COMMIT))
	{
		pcrResponce = GOT_PRICE_CHANGE_PENDING_COMMIT;
	}
	else if(m_byInPriceChangeWithIntegrityProcessFlags & (~PRICE_CHANGE_WITH_INTEGRITY_ACTIVE & PRICE_CHANGE_WITH_INTEGRITY_PENDING_COMMIT_AFTER_FAILURE))
	{
		pcrResponce = GOT_PRICE_CHANGE_PENDING_COMMIT_AFTER_FAILURE;
	}
	else if(m_byInPriceChangeWithIntegrityProcessFlags & (~PRICE_CHANGE_WITH_INTEGRITY_ACTIVE & PRICE_CHANGE_WITH_INTEGRITY_GOT_COMMIT))
	{
		pcrResponce = PRICE_CHANGE_NO_RESPONSE;
	}
	else if(m_byInPriceChangeWithIntegrityProcessFlags & PRICE_CHANGE_WITH_INTEGRITY_ACTIVE)
	{
		pcrResponce = PRICE_CHANGE_NO_RESPONSE;
	}
	else
	{
		pcrResponce = PRICE_CHANGE_NO_RESPONSE;
	}

	_Module.m_server.SetPriceChangeResponse(pcrResponce);

}