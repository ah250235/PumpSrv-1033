// AlarmSrvPS.cpp: implementation of the CAlarmSrvPS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlarmSrvPS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlarmSrvPS::CAlarmSrvPS()
{
	InitializeCriticalSection(&m_csAlarmArray);	
}

CAlarmSrvPS::~CAlarmSrvPS()
{
	DeleteCriticalSection(&m_csAlarmArray);	
}

long CAlarmSrvPS::WriteComError(_com_error &e)
{
	long lRetVal = 0;
	long lAlarmId = 0;

	_LOGMSG.LogResult(
				0, // Src
				"CAlarmSrvPs",
				"WriteComError",
				FALSE,0,
				TRUE,e.Error(),e);


	return lRetVal;

}

long CAlarmSrvPS::SendAlarmToAlarmSrv(CAlarmSrvQueueData & cAlarmSrvQueueData)
{
	GCIRetCode retCode  = GCI_OK;
	int iTry = 0;
	int iBigTry =1;
	BOOL bCOMSent = FALSE;

	long lRetVal = 0;
	long lAlarmId = 0;	

	long lAlarmSrvMode = _Module.m_server.GetAlarmSrvMode();

	if(lAlarmSrvMode)
	{
		CAlarmSrvSendInfo  *pInfo;
		pInfo = (CAlarmSrvSendInfo *)cAlarmSrvQueueData.GetDataPtr();
					
		while ( !bCOMSent )
		{
			BSTR sRec = NULL;					//.4.0.27.500

			if(LOG_BASIC_FLOW_CONTROL)
			{	
				CString sMsg;
				sMsg.Format("SendAlarmToAlarmSrv, attempt retry=%ld  to send lCategory=%ld, subCategory=%ld,lAlarmNumber=%ld,lDevice=%ld,lAlarmValue=%ld",
									  iBigTry,
									   pInfo->lCatagory ,
									   pInfo->lSubCatagory,
									   pInfo->lAlarmNumber, 
									   pInfo->lDevice,
									   pInfo->lAlarmValue);									   									 
				_LOGMSG.LogMsg(sMsg,LOG_LEVEL_3);
			}
			
			try
			{			
				CComBSTR bstrTmp((LPCSTR)pInfo->sMessages);
		
				
				lRetVal = m_pGAI->SetAlarm(pInfo->lCatagory ,
										   pInfo->lSubCatagory,
										   pInfo->lAlarmNumber, 
										   pInfo->lDevice,
										   cAlarmSrvQueueData.GetStatus(),
										   pInfo->lAlarmValue,									   
										   pInfo->lFlags,
										   &lAlarmId ,bstrTmp.m_str);

				bCOMSent = TRUE;
	
			}
			catch(_com_error & e )
			{
				retCode = GCI_ALARMSRV_NOT_ACTIVE;
				
				_LOGMSG.LogResult(
					0, // Src
					"IGCI",
					"SetAlarm",
					FALSE,0,
					TRUE,e.Error(),e);

				Sleep( 10);
				_ALARM_SRV.Restore_COM();
				iBigTry++;				
			}	
			catch(...)
			{
				retCode = GCI_ALARMSRV_NOT_ACTIVE;
				
				if(LOG_LOGIC_ERROR)
					_LOGMSG.LogResult(
						0, // Src
						"IGCI",
						"SetAlarm",
						TRUE,0,
						(lRetVal!=0)? TRUE:FALSE,lRetVal);

				Sleep( 10);
				_ALARM_SRV.Restore_COM();
				iBigTry++;
			}
			
			if (iBigTry >= MAX_COM_ATTEMPTS)
			{
				bCOMSent = TRUE; //4.0.21.500
			}

			if (sRec != NULL)				//4.0.27.500
			{
				SysFreeString(sRec);
				sRec = NULL;
			}
		}
	}
	else
	{
		retCode = GCI_ALARMSRV_NOT_ACTIVE;		
	}
		
	return retCode;	
}

/******************************************************************************
 Description:	Cretate Alarm message to send to AlarmSrv 	
 Returns:      	
 Parameters:   	long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus, long lFlags
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			13/11/2002   17:15		Start
******************************************************************************/

long CAlarmSrvPS::AddSendMessage(long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus,long lAlarmValue,long lFlags ,char *sMessages)
{
	CAlarmSrvSendInfo cAlarmSrvSendInfo;
	CAlarmSrvQueueData cAlarmSrvQueueData;
	long lRetVal = 0;

	EnterCriticalSection(&m_csAlarmArray);

	cAlarmSrvQueueData.SetCmd(SEND_TO_ALARM_SRV);
	cAlarmSrvQueueData.SetStatus((AlarmStatus)lAlarmStatus);

	cAlarmSrvSendInfo.lCatagory = lCatagory;
	cAlarmSrvSendInfo.lSubCatagory = lSubCatagory;
	cAlarmSrvSendInfo.lAlarmNumber = lAlarmNumber;
	cAlarmSrvSendInfo.lDevice = lDevice;
	cAlarmSrvSendInfo.lFlags = lFlags;
	cAlarmSrvSendInfo.lAlarmValue = lAlarmValue;

	memset(cAlarmSrvSendInfo.sMessages , 0 , sizeof(cAlarmSrvSendInfo.sMessages));
	if (sMessages)
	{
		memcpy(cAlarmSrvSendInfo.sMessages ,sMessages, MAX_ALARM_DESC_LEN);
	}

	cAlarmSrvQueueData.SetCmdData((char*)&cAlarmSrvSendInfo , sizeof(cAlarmSrvSendInfo));
	lRetVal = m_cAlarmSrvQueue.Set(&cAlarmSrvQueueData);

	
	if(lRetVal == QUEUE_FULL)           // 4.0.29.501	TD 290189
	{
		CString str;
		str.Format("Alarms Queue is Full.Error alarm lCategory=%ld, subCategory=%ld,lAlarmNumber=%ld,lDevice=%ld, status=%ld not added",
			lCatagory ,
			lSubCatagory,
			lAlarmNumber, 
			lDevice,
			lAlarmStatus,
			lRetVal); 
		
		_LOGMSG.LogMsg(str, LOG_LEVEL_3);				
		
	}

	LeaveCriticalSection(&m_csAlarmArray);

	return lRetVal;
}


/******************************************************************************
 Description:	creat received message.from the event messages
 Returns:      	
 Parameters:   	long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus, long lFlags
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			13/11/2002   17:15		Start
******************************************************************************/

void CAlarmSrvPS::AddReceivedMessage(long lCatagory,long lSubCatagory,long lAlarmNumber,long lDevice,long lAlarmStatus,long lAlarmId, long lFlags)
{
	CAlarmSrvReceivedEventInfo cAlarmSrvReceivedEventInfo;
	CAlarmSrvQueueData cAlarmSrvQueueData;
	long lRetVal = 0;

	memset(&cAlarmSrvReceivedEventInfo , 0, sizeof(cAlarmSrvReceivedEventInfo));

	cAlarmSrvQueueData.SetCmd(RECEIVED_TO_ALARM_SRV);
	cAlarmSrvQueueData.SetStatus((AlarmStatus)lAlarmStatus);	

	cAlarmSrvReceivedEventInfo.lCatagory = lCatagory;
	cAlarmSrvReceivedEventInfo.lSubCatagory = lSubCatagory;
	cAlarmSrvReceivedEventInfo.lAlarmNumber = lAlarmNumber;
	cAlarmSrvReceivedEventInfo.lDevice = lDevice;
	cAlarmSrvReceivedEventInfo.lAlarmId = lAlarmId;
	cAlarmSrvReceivedEventInfo.lFlags = lFlags; //4.0.3.11 
	
	cAlarmSrvQueueData.SetCmdData((char *)&cAlarmSrvReceivedEventInfo,sizeof(cAlarmSrvReceivedEventInfo));
	lRetVal = m_cAlarmReceivedQueue.Set(&cAlarmSrvQueueData); //4.0.3.15

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString sStr = "";
		sStr.Format("AddReceivedMessage, lCategory=%ld, subCategory=%ld,lAlarmNumber=%ld,lDevice=%ld, status=%ld, lRetVal=%ld",
							   lCatagory,
							   lSubCatagory,
							   lAlarmNumber,
							   lDevice,
							   lAlarmStatus,
							   lRetVal);             // 4.0.29.501	TD 290189
		_LOGMSG.LogMsg(sStr.GetBuffer(0), LOG_LEVEL_3);			
	}
}




/******************************************************************************
 Description:	Add alarm to list  	
 Returns:      	None
 Parameters:   	cAlarmSrvBuffer - struct with all alarm inforamtion
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			29/1/2003   16:13		Start
******************************************************************************/
void CAlarmSrvPS::AddAlarmNumerator(CAlarmSrvQueueData & cAlarmSrvQueueData)
{
	CAlarmSrvReceivedEventInfo *pInfo;
	pInfo = (CAlarmSrvReceivedEventInfo *)cAlarmSrvQueueData.GetDataPtr();	
	CString sStr = "";
	
	if (IsPumpSrvGroup(pInfo->lCatagory))
	{		
		long lGroupId=0;
		EnterCriticalSection(&m_csAlarmArray);	
					
		// System alarm check for special groups.
		lGroupId = CheckSpecialAlarmGroup(pInfo);
		switch(lGroupId)
		{
			case ALARM_GROUP_OLA:
			{
				m_lAlarmSpecialGroupMap[ALARM_GROUP_OLA]++;
			}
			break;
			case ALARM_GROUP_TANK:
			{				
				// check exist  any  Tank alarm 
				m_lAlarmSpecialGroupMap[ALARM_GROUP_TANK]++;

				// put Tank alarm in the special department.
				if ((pInfo->lDevice < MAX_ALARM_TANKS) && (pInfo->lDevice >= 0)) // CID_185558 Out-of-bounds write
					m_lAlarmTankMap[pInfo->lDevice]++;
				else
				{
					m_lAlarmTankMap[pInfo->lDevice]++;
					if(LOG_BASIC_FLOW_CONTROL)
					{
						sStr.Format("CAlarmSrvPS::AddAlarmNumerator Alarm =%ld system !!!!!", pInfo->lDevice);
						_LOGMSG.LogMsg(sStr.GetBuffer(0), LOG_LEVEL_3);
					}
				}
			}
			break;
			case ALARM_GROUP_PUMP:
			{
				if ((pInfo->lDevice < MAX_ALARM_PUMP) && (pInfo->lDevice >= 0)) // CID_184878 Out-of-bounds write
					m_lAlarmPumpMap[pInfo->lDevice]++;
				else
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{
						sStr.Format("CAlarmSrvPS::AddAlarmNumerator Pump =%ld not valid!!!!!", pInfo->lDevice);
						_LOGMSG.LogMsg(sStr.GetBuffer(0), LOG_LEVEL_3);
					}
				}
			}
			break;
			case ALARM_GROUP_GENERAL_SYSTEM:
			{
				m_lAlarmPumpMap[0]++;

				 if ((pInfo->lAlarmNumber == SOFTWARE_DISK_FULL_WARNING) && 
					 (pInfo->lSubCatagory == SUB_PUMPSRV))
				 {
						_Module.m_server.m_lDiskWarning = SYS_ERROR_DISK_WARNING;
				 }
				 else if ((pInfo->lAlarmNumber == SOFTWARE_DISK_FULL_WARNING) && 
					 (pInfo->lSubCatagory == SOFTWARE_DISK_FULL_ERR))
				 {
						_Module.m_server.m_lDiskWarning = SYS_ERROR_DISK_FULL;
				 }

			}
			default:
				break;
		}		

		if(LOG_BASIC_FLOW_CONTROL)
		{
			sStr.Format("AddAlarmNumerator to group=%ld, SeverityAlarmsCount=%ld", lGroupId,_Module.m_server.GetSeverityAlarmsCount());
			_LOGMSG.LogMsg(sStr.GetBuffer(0), LOG_LEVEL_3);			
		}

		LeaveCriticalSection(&m_csAlarmArray);
	}
}

long CAlarmSrvPS::DecreaseAlarmNumerator(CAlarmSrvQueueData & cAlarmSrvQueueData)
{
	CAlarmSrvReceivedEventInfo *pInfo;
	pInfo = (CAlarmSrvReceivedEventInfo *)cAlarmSrvQueueData.GetDataPtr();
	CString sStr = "";

	if (IsPumpSrvGroup(pInfo->lCatagory))
	{		
		long lGroupId = 0;
		EnterCriticalSection(&m_csAlarmArray);	
					
		// System alarm check for special groups.

		lGroupId = CheckSpecialAlarmGroup(pInfo);

		switch(lGroupId)
		{
			case ALARM_GROUP_OLA:
			{
				m_lAlarmSpecialGroupMap[ALARM_GROUP_OLA]--;

				if (m_lAlarmSpecialGroupMap[ALARM_GROUP_OLA] < 0 )
					m_lAlarmSpecialGroupMap[ALARM_GROUP_OLA] = 0;

			}
			break;
			case ALARM_GROUP_TANK:
			{
				long lDevice = pInfo->lDevice;
				m_lAlarmSpecialGroupMap[ALARM_GROUP_TANK]--;

				if (m_lAlarmSpecialGroupMap[ALARM_GROUP_TANK] < 0)
					m_lAlarmSpecialGroupMap[ALARM_GROUP_TANK]  = 0;

				if ((lDevice < MAX_ALARM_TANKS) && (lDevice >= 0)) // CID_183994 Out-of-bounds write
				{
					m_lAlarmTankMap[lDevice]--;

					if (m_lAlarmTankMap[lDevice] < 0)
						m_lAlarmTankMap[lDevice] = 0;

				}					
				else
				{
					
					if (LOG_BASIC_FLOW_CONTROL)
					{
						sStr.Format("DecreaseAlarmNumerator, error Alarm =%ld system !!!!!", pInfo->lDevice);
						_LOGMSG.LogMsg(sStr.GetBuffer(0), LOG_LEVEL_3);			
					}
				}
			}
			break;
			case ALARM_GROUP_PUMP:
			{
				long lDevice = pInfo->lDevice;
				if (( lDevice < MAX_ALARM_PUMP) && (lDevice >= 0)) // CID_185964 Out-of-bounds write
				{
					m_lAlarmPumpMap[pInfo->lDevice]--;

					if (m_lAlarmPumpMap[lDevice] < 0)
						m_lAlarmPumpMap[lDevice] = 0;
				}
				else
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{
						sStr.Format("DecreaseAlarmNumerator Pump =%ld not valid!!!!!", pInfo->lDevice);
						_LOGMSG.LogMsg(sStr.GetBuffer(0), LOG_LEVEL_3);			
					}
				}
			}
			break;
			case ALARM_GROUP_GENERAL_SYSTEM:
			{
				m_lAlarmPumpMap[0]--;

				if (m_lAlarmPumpMap[0] < 0)
					m_lAlarmPumpMap[0] = 0;

				 if ((pInfo->lAlarmNumber == SOFTWARE_DISK_FULL_WARNING) && 
					 (pInfo->lSubCatagory == SUB_PUMPSRV))
				 {
						_Module.m_server.m_lDiskWarning = 0;
				 }
				 else if ((pInfo->lAlarmNumber == SOFTWARE_DISK_FULL_WARNING) && 
					 (pInfo->lSubCatagory == SOFTWARE_DISK_FULL_ERR))
				 {
						_Module.m_server.m_lDiskWarning =0;
				 }

			}
			default:
				break;
		}		

		if(LOG_BASIC_FLOW_CONTROL)
		{
			sStr.Format("DecreaseAlarmNumerator to group=%ld, SeverityAlarmsCount=%ld", lGroupId,_Module.m_server.GetSeverityAlarmsCount());
			_LOGMSG.LogMsg(sStr.GetBuffer(0), LOG_LEVEL_3);			
		}

		LeaveCriticalSection(&m_csAlarmArray);			
	}

	return 0;
}

/******************************************************************************
 Description:	Checking exist alarm in system 	
 Returns:      	yes/no
 Parameters:   	lPump - Device number 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			29/1/2003   16:09		Start
******************************************************************************/
long CAlarmSrvPS::IsAnyAlarmPump(long lPump) const
{
	long lRetVal = 0 ;


	if (lPump)
	{
		if (m_lAlarmPumpMap[lPump])
		{
			lRetVal = 1;
		}
	}
	else 
	{
		for(int i = 1; i < MAX_ALARM_PUMP; i++)
		{
			if (m_lAlarmPumpMap[i])
			{
				lRetVal = 1;
				break;
			}
		}
	}
	return lRetVal;
}

/******************************************************************************
 Description:	Find special Alarm Group 	
 Returns:      	Group number
 Parameters:   	CAlarmSrvBuffer &cAlarmSrvBuffer
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			29/1/2003   18:33		Start
******************************************************************************/
long CAlarmSrvPS::CheckSpecialAlarmGroup(CAlarmSrvReceivedEventInfo *cAlarmSrvReceivedEventInfo)
{

	long lAlarmGroup = ALARM_GROUP_NOT_FOUND;

	if ((cAlarmSrvReceivedEventInfo->lCatagory == CATEGORY_SOFTWARE) && 
		(cAlarmSrvReceivedEventInfo->lSubCatagory == SUB_PUMPSRV) &&
		(cAlarmSrvReceivedEventInfo->lAlarmNumber == SOFTWARE_GENERAL_OLA_ERR))
	{
		lAlarmGroup = ALARM_GROUP_OLA;
	}	
	else if (cAlarmSrvReceivedEventInfo->lCatagory == CATEGORY_TANK)
	{
		lAlarmGroup =  ALARM_GROUP_TANK;
	}
	else if (cAlarmSrvReceivedEventInfo->lCatagory == CATEGORY_PUMP) 
	{
		lAlarmGroup =  ALARM_GROUP_PUMP;
	}
	else if(IsPumpSrvGroup(cAlarmSrvReceivedEventInfo->lCatagory))
	{
		lAlarmGroup =  ALARM_GROUP_GENERAL_SYSTEM;
	}
		
	return lAlarmGroup;
}
/******************************************************************************
 Description:	Check is PumpSrv category 	
 Returns:      	TRUE/FALSE
 Parameters:   	lCategory = Alarm category
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			25/2/2003   10:14		Start
******************************************************************************/

BOOL CAlarmSrvPS::IsPumpSrvGroup(long lCategory) const
{
	BOOL bRet = FALSE;

	if ((CATEGORY_TANK == lCategory) ||		
		(CATEGORY_PUMP == lCategory)	||	
		(CATEGORY_SOFTWARE == lCategory)	||
		(CATEGORY_DEVICES == lCategory)	||
		(CATEGORY_CONTROLLER == lCategory))
	bRet = TRUE;


	return bRet;
}

BOOL CAlarmSrvPS::HandleComException(char *szMsgFuncName, int &iRetryCount, int &iBigRetryCount , _com_error e )
{
	BOOL bRetCode = FALSE;
	CString sMsg;

	iRetryCount++;	
	
	if (iRetryCount >= MAX_COM_ATTEMPTS)
	{
		Sleep(10000);
		iRetryCount = 0;
		iBigRetryCount++;
		if (iBigRetryCount >= MAX_COM_ATTEMPTS)
		{
			bRetCode = TRUE;
		}
	}
	return bRetCode;
}

long CAlarmSrvPS::PumpStatusAlarm(long lPumpNumber,PumpStatus lCurrStatus, PumpStatus lNewStatus)
{
	long lRetVal = 0;
	
	if(lCurrStatus != lNewStatus)
	{
		if(lNewStatus == OUT_OF_SERVICE)
		{
			//Setting on the 'out of service' alarm
			AddSendMessage(CATEGORY_PUMP,SUB_TERMINAL_DISPENSER,PUMP_OUT_OF_SERVICE,lPumpNumber,ALARM_ON,lPumpNumber,0);
		}
		//No TD - CL can also send COMMS_TERMINATED as pump state.
		else if( ((lCurrStatus == OUT_OF_SERVICE) || (lCurrStatus == COMMS_TERMINATED)) && 
			     (lNewStatus != UNKNOWN_STATUS || lNewStatus != COMMS_TERMINATED ) )
		{
			//Setting off the 'out of service' alarm
			AddSendMessage(CATEGORY_PUMP,SUB_TERMINAL_DISPENSER,PUMP_OUT_OF_SERVICE,lPumpNumber,ALARM_OFF,0,0);
		}
	}

	return lRetVal;
}

long CAlarmSrvPS::ServerStateAlarm(ServerState lCurrState, ServerState lNewState)
{
	long lRetVal = 0;

	if(lCurrState != lNewState)
	{
		if(lCurrState == SERVER_IDLE && lNewState == SERVER_STARTUP)
		{
			//Setting on the 'no controller' alarm
			AddSendMessage(CATEGORY_SOFTWARE,SUB_PUMPSRV,SOFTWARE_NO_CONTROLLER,0,ALARM_ON,0,0);	//4.0.23.0 TD 76536 //129043
		}
		else if( lCurrState == SERVER_STARTUP && lNewState == SERVER_IDLE)
		{
			//Setting off the 'no controller' alarm and the 'no convert layer'
			AddSendMessage(CATEGORY_SOFTWARE,SUB_PUMPSRV,SOFTWARE_NO_CONTROLLER,0,ALARM_OFF,0,0);	//4.0.23.0 TD 76536
			AddSendMessage(CATEGORY_SOFTWARE,SUB_PUMPSRV,SOFTWARE_NO_CONVERT_LAYER,0,ALARM_OFF,0,0);//4.0.23.0 TD 76536
		}
		else if(lCurrState == SERVER_IDLE && lNewState == SERVER_NO_CONVERT)
		{
			//Setting on the 'no convert layer' alarm
			AddSendMessage(CATEGORY_SOFTWARE,SUB_PUMPSRV,SOFTWARE_NO_CONVERT_LAYER,0,ALARM_ON,0,0); //4.0.23.0 TD 76536
		}
	}
	return lRetVal;
}


//////////////////////////////////////////////////////////////////////
// CAlarmCmdData Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlarmSrvQueueData::CAlarmSrvQueueData()
{
	memset(m_sData, 0, sizeof(m_sData));
}

CAlarmSrvQueueData::~CAlarmSrvQueueData()
{

}

AlarmSrvCmdType CAlarmSrvQueueData::GetCommand()
{
	return m_enmAlarmSrvCmdType;
}

AlarmStatus CAlarmSrvQueueData::GetStatus()
{
	return m_enmAlarmStatus;
}

char * CAlarmSrvQueueData::GetDataPtr()
{
	return (m_sData);
}

void CAlarmSrvQueueData::SetStatus(AlarmStatus enmAlarmStatus)
{
	m_enmAlarmStatus = enmAlarmStatus;
}

void CAlarmSrvQueueData::SetCmd(AlarmSrvCmdType enmCmdType)
{
	m_enmAlarmSrvCmdType = enmCmdType;  
}

void CAlarmSrvQueueData::SetCmdData(char *sBuff, long lSize)
{
	memcpy(m_sData, sBuff, min(lSize, sizeof(m_sData)));
}
