// GAIEventsHandler.cpp: implementation of the CGAIEventsHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GAIEventsHandler.h"
#include "AlarmConvert.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGAIEventsHandler* _pGAIEventsHandler;
//CAlarmClientModule _Module;



CGAIEventsHandler::CGAIEventsHandler()
{
	m_lEventsClientId = 0;
}

CGAIEventsHandler::~CGAIEventsHandler()
{

}

// 4.0.3.11
HRESULT __stdcall CGAIEventsHandler::UpdateAlarmMap(long lCatagory,long lSubCatagory,long lAlarmNumber,long lDevice,long lAlarmStatus,long lAlarmId,long lFlags)
{
	CPumpStatus		pumpStat;
	CAlarmConvert	cAlarmConvert;
	PumpDevice		pumpDevice = NO_DEVICE;
	long			lPumpNumber =0;
	long			lAlarmType =0;
	long			lVal = 0;
	CString cMsg;

	if(LOG_BASIC_FLOW_CONTROL)
	{		
		cMsg.Format("UpdateAlarmMap, Get Event, Cat=%ld ,SubCat=%ld, AlarmNumber%ld , lDevice=%ld, AlarmStatus=%ld, AlarmId=%ld", lCatagory,lSubCatagory,lAlarmNumber,lDevice,lAlarmStatus,lAlarmId);
		_LOGMSG.LogMsg(cMsg);
	}
	
	if (_Module.m_server.GetAlarmSrvMode() < ALRMSRV_DIRECT_ACTIVE)
	{		
		cAlarmConvert.ConvertAlarmCodeToDeviceErrorWarning(lAlarmId,lPumpNumber,lAlarmType,lVal);	
		if (cAlarmConvert.IsPumpSrvAlarm(lCatagory))
		{
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			if (!PUMP_NOT_VALID(lPumpNumber))
			{
				pumpStat = _Module.m_server.m_cPumpArray[lPumpNumber -1 ].GetStat();

				pumpDevice = (PumpDevice)lSubCatagory;
				

				switch(lAlarmType)
				{		
					case ALARM_TYPE_ERROR:
					{
						if (lPumpNumber)
						{
							if (lAlarmStatus == ALARM_STARTED)
							{								
								pumpStat.SetDeviceError(lVal,pumpDevice);
							}

							else if (lAlarmStatus == ALARM_ENDED)
								pumpStat.SetDeviceError(0,(PumpDevice)0);								
						}	
						
						/*else
						{
							//_Module.m_server.m_lSysWarning = lVal;
							_Module.m_server.m_lSysError = lVal;
						}*/

						break;
					}
					case ALARM_TYPE_WARNING :
					{
						if (lPumpNumber)
						{							
							if (lAlarmStatus == ALARM_STARTED)
								pumpStat.SetDeviceWarning(lVal,pumpDevice);
							else if (lAlarmStatus == ALARM_ENDED)
								pumpStat.SetDeviceWarning(0,(PumpDevice)0);							
						}

						/*else
						{
							_Module.m_server.m_lSysWarning = lVal;			
						}*/							
						break;
					}
					//4.0.3.14 
					// support special error message , in Allied environment
					case ALARM_TYPE_MESSAGE:
					{
						if (lAlarmStatus == ALARM_STARTED)
							pumpStat.SetMessage(lVal);
						else if (lAlarmStatus == ALARM_ENDED)
							pumpStat.SetMessage(0);
					}
					break;
					// 4.0.4.14
					default:									

						if (lPumpNumber)
						{																					
							if (lAlarmStatus == ALARM_STARTED)
							{								
								pumpStat.SetDeviceWarning(lVal,pumpDevice);
								pumpStat.SetDeviceError(lVal,pumpDevice);
							}
							else if (lAlarmStatus == ALARM_ENDED)
							{
								pumpStat.SetDeviceWarning(0,(PumpDevice)0);
								pumpStat.SetDeviceError(0,(PumpDevice)0);
							}
						}

/*						else
						{
							//Clear device/system errors
							pumpDevice = (PumpDevice)0;

							_Module.m_server.m_lSysWarning = lVal;			
							_Module.m_server.m_lSysError = lVal;
						}
						*/
						break;
						if(LOG_BASIC_FLOW_CONTROL)
						{									
							cMsg.Format("UpdateAlarmMap, Clear information Cat=%ld ,SubCat=%ld, AlarmNumber%ld , lDevice=%ld, AlarmStatus=%ld, AlarmId=%ld", lCatagory,lSubCatagory,lAlarmNumber,lDevice,lAlarmStatus,lAlarmId);
							_LOGMSG.LogMsg(cMsg);
						}
					break;
				}

				
				_Module.m_server.m_cPumpArray[lPumpNumber -1].SetStat(pumpStat);

				if(LOG_BASIC_FLOW_CONTROL)
				{		
					CString cMsg;
					
					if (lPumpNumber)
					{
						cMsg.Format("Update, Error=%ld , Warning=%ld , misc=%ld" , pumpStat.GetError() , pumpStat.GetWarning() , pumpStat.GetMisc());
					}
					else
					{
						cMsg.Format("Update System error  Error=%ld , Warning=%ld", _Module.m_server.m_lSysError, _Module.m_server.m_lSysWarning );
					}
					
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,cMsg);
				}
			}
			else if( !lPumpNumber)
			{
				switch(lAlarmType)
				{		
					case ALARM_TYPE_ERROR:
					{
						if (lAlarmStatus == ALARM_STARTED)
							_Module.m_server.m_lSysError = lVal;
						else if (lAlarmStatus == ALARM_ENDED)
							_Module.m_server.m_lSysError = 0;

						break;
					}
					case ALARM_TYPE_WARNING :
					{
						if (lAlarmStatus == ALARM_STARTED)
							_Module.m_server.m_lSysWarning = lVal;			
						else if (lAlarmStatus == ALARM_ENDED)
							_Module.m_server.m_lSysWarning = 0;			

						break;
					}
					case ALARM_TYPE_MISC:
					{
						if (lAlarmStatus == ALARM_STARTED)
						{
							_Module.m_server.m_lSysMisc = lVal;			
						}
						else if (lAlarmStatus == ALARM_ENDED)
						{
							_Module.m_server.m_lSysMisc = 0; //4.0.2.23
						}
						
						break;
					}
					default:

						_Module.m_server.m_lSysWarning = 0;			
						_Module.m_server.m_lSysError = 0;	 
						_Module.m_server.m_lSysMisc = 0; //4.0.2.23						
					break;
				}

			}

			if(LOG_BASIC_FLOW_CONTROL)
			{									
				cMsg.Format("UpdateAlarmMap, AlarmType=%ld, AlarmStatus=%ld, Value=%ld, device=%d" , lAlarmType, lAlarmStatus, lVal ,pumpDevice );
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,cMsg);
			}

			if (slPumpArray.IsLocked())
				slPumpArray.Unlock();
		}
		else
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{		
				cMsg.Format("This is not PumpSrv Category=%ld", lCatagory);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,cMsg);
			}
		}
	}
	else
	{	
		//4.0.3.11
		// do not deal with inform event , because this event used only when we handle only on Alarm per device (AlarmSrvActive=1) 
		if (!(lFlags & ALARM_EVENT_INFORM_ON_NEXT_ALARM))
			_Module.m_server.m_cAlarm.AddReceivedMessage(lCatagory,lSubCatagory,lAlarmNumber,lDevice,lAlarmStatus,lAlarmId,lFlags);
	}

	return S_OK;
}

//4.0.13.500
HRESULT __stdcall CGAIEventsHandler::UpdateAlarmMapEx(long lCatagory,long lSubCatagory,long lAlarmNumber,long lDevice,long lAlarmStatus,long lAlarmId,long lFlags,BSTR sAlarmExtraParams)
{
	long lAlarmPriority = 0;

	ALARM_EXTRA_PARAMS cStructAlarmExtraParams;
	memset(&cStructAlarmExtraParams,0,sizeof(ALARM_EXTRA_PARAMS));
	Bstr2Buffer(sAlarmExtraParams, (void *)&cStructAlarmExtraParams,sizeof(ALARM_EXTRA_PARAMS));

	lAlarmPriority = a2l(cStructAlarmExtraParams.sAlarmPriority,sizeof(cStructAlarmExtraParams.sAlarmPriority));

	//If the priority of the alarm is higher then the min alert severity - it should be sounded.
	//4.0.13.505
	if ((_Module.m_server.GetMinAlertSeverity_SoundAlert() > 0) && (lAlarmPriority >= _Module.m_server.GetMinAlertSeverity_SoundAlert()))
	{
		if (lAlarmStatus == ALARM_STARTED)
		{
			_Module.m_server.IncreaseSeverityAlarmsCount();
		}
		else if (lAlarmStatus == ALARM_ENDED)
		{
			_Module.m_server.DecreaseSeverityAlarmsCount();
		}
	}
		
	return UpdateAlarmMap(lCatagory,lSubCatagory,lAlarmNumber,lDevice,lAlarmStatus,lAlarmId,lFlags);
}


void CGAIEventsHandler::SetEventClientId(long lClientId)
{
	m_lEventsClientId = lClientId;	
}

long CGAIEventsHandler::GetEventClientId()
{
	return m_lEventsClientId;
}
