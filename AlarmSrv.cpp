// AlarmSrv.cpp: implementation of the CAlarmSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlarmSrv.h"


enum CAlarmSrvRetCode {

					CALARM_SRV_OK,							//0
					CALARM_SRV_FAIL_CREATE_GAI_CONN,		//1					
					CALARM_SRV_COM_ERROR,					//2
					CALARM_SRV_FAIL_CREATE_GAIEVENT_CONN,	//3
					CALARM_SRV_NOT_CONNECTED,			    //4
					CALARM_SRV_CREATE_GAIEVENT_CONN,		//5
};



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlarmSrv::CAlarmSrv()
{
	m_pGAI = NULL;
	m_lAlarmSrvActive = 0;
}

CAlarmSrv::~CAlarmSrv()
{

}

long CAlarmSrv::CreateConnectToAlarmSrv()
{
	long lRetCode = 0;
	HRESULT hRes = 0;

	try
	{			
		hRes = m_pGAI.CreateInstance(CLSID_GAI);

		if(SUCCEEDED(hRes))		
		{
			lRetCode = CALARM_SRV_OK;		
		}
		else
		{
			m_pGAI = NULL; // also release the object
			lRetCode = CALARM_SRV_FAIL_CREATE_GAI_CONN;
		}

	}
	catch (_com_error& e)
	{
		WriteComError(e);
		m_pGAI = NULL; // also release the object
		lRetCode = CALARM_SRV_FAIL_CREATE_GAI_CONN;		
	}
	catch(...)
	{
		m_pGAI = NULL; // also release the object //4.0.21.501
		lRetCode = CALARM_SRV_FAIL_CREATE_GAI_CONN;	

		if (LOG_LOGIC_ERROR )
		{
			CString sMsg;
			sMsg.Format("CAlarmSrv::CreateConnectToAlarmSrv()");
			_LOGMSG.LogMsg(sMsg,LOG_LOGIC_ERROR);			
		}
	}
	
	if (!lRetCode)
		lRetCode = CreateConnectionToGAIEvents();
	
	if(lRetCode)
		_Module.m_server.m_cCompConnectivity.SetAlarmSrvConn(FALSE);
	else
		_Module.m_server.m_cCompConnectivity.SetAlarmSrvConn(TRUE);

	return lRetCode;
}

long CAlarmSrv::CloseConnectToAlarmSrv()
{
	long lRetCode = CALARM_SRV_OK;
	HRESULT  hr = 0;
	
	if(m_pGAI != NULL)
	{
		try
		{
			//checking if Event class was created
			if(_pGAIEventsHandler)
			{
				long lClientId = _pGAIEventsHandler->GetEventClientId();
				m_pGAI->CloseEventsInterface(lClientId);
				IUnknownPtr pUnk = m_pGAI;
				_pGAIEventsHandler->DispEventUnadvise(pUnk);
				delete _pGAIEventsHandler;		
				_pGAIEventsHandler = NULL;
			}
			
			m_pGAI.Release();
			m_pGAI.Detach();
			m_pGAI = NULL;
		}
		catch (_com_error& e)  //4.0.2.26
		{
			WriteComError(e);
			m_pGAI = NULL; // also release the object
			lRetCode = CALARM_SRV_COM_ERROR;		
		}
		catch(...)
		{
			m_pGAI = NULL;
			lRetCode = CALARM_SRV_COM_ERROR;
		}
	}
	
	// Clear all Buffer.//4.0.3.16
	memset(m_lAlarmPumpMap , 0 ,sizeof(m_lAlarmPumpMap));
	memset(m_lAlarmSpecialGroupMap ,0,sizeof(m_lAlarmSpecialGroupMap));
	memset(m_lAlarmTankMap , 0, sizeof(m_lAlarmTankMap));

	_Module.m_server.ResetSeverityAlarmsCount(); //4.0.13.500

	_Module.m_server.m_cAlarm.m_cAlarmReceivedQueue.Clean();  //4.0.3.15
	_Module.m_server.m_cAlarm.m_cAlarmSrvQueue.Clean();  //4.0.3.15

	//4.0.5.26 - when closing the connection, setting to FALSE
	_Module.m_server.m_cCompConnectivity.SetAlarmSrvConn(FALSE);

	return lRetCode;
}

long CAlarmSrv::Restore_COM()
{
	long lRetCode = 0;

	CloseConnectToAlarmSrv();
	lRetCode = CreateConnectToAlarmSrv();
	
	return lRetCode;
}

long CAlarmSrv::CreateConnectionToGAIEvents()
{
	long lRetCode = CALARM_SRV_OK;
	HRESULT hRes = 0;
	long lClientId = 0;


	CString sMsg;
	sMsg.Format("CreateConnectionToGAIEvents - Start");
	_LOGMSG.LogMsg(sMsg);			


	try
	{
		// Clear all Buffer.
		memset(m_lAlarmPumpMap , 0 ,sizeof(m_lAlarmPumpMap));
		memset(m_lAlarmSpecialGroupMap ,0,sizeof(m_lAlarmSpecialGroupMap));
		memset(m_lAlarmTankMap , 0, sizeof(m_lAlarmTankMap));
	
		_Module.m_server.ResetSeverityAlarmsCount(); //4.0.13.500

		_Module.m_server.m_cAlarm.m_cAlarmReceivedQueue.Clean();  //4.0.3.15
	
		//setting to NULL befor starting.
		_pGAIEventsHandler = NULL;
		
		_pGAIEventsHandler = new CGAIEventsHandler;
		// Advise the server of the sink.
		IUnknownPtr pUnk = m_pGAI;
		hRes = _pGAIEventsHandler->DispEventAdvise(pUnk);
		if ( FAILED(hRes) )
		{			
			CString sMsg;
			sMsg.Format("CreateConnectionToGAIEvents FAILED=%ld", hRes);
			_LOGMSG.LogMsg(sMsg);			

			lRetCode = CALARM_SRV_FAIL_CREATE_GAIEVENT_CONN;	
			_pGAIEventsHandler = NULL;
		}
		else
		{
			//setting the event client id
			_pGAIEventsHandler->SetEventClientId(lClientId);

			CString sMsg;
			sMsg.Format("CreateConnectionToGAIEvents Success");
			_LOGMSG.LogMsg(sMsg);			

		}
		lRetCode = m_pGAI->InitEventsInterface(1 ,&lClientId);
	}
	catch (_com_error& e)
	{
		WriteComError(e);
		_pGAIEventsHandler = NULL;
		lRetCode = CALARM_SRV_CREATE_GAIEVENT_CONN;		
	}
	catch(...)  //4.0.2.26
	{			
		_pGAIEventsHandler = NULL; //4.0.21.501
		lRetCode = CALARM_SRV_CREATE_GAIEVENT_CONN;	

		CString sMsg;
		sMsg.Format("CAlarmSrv::CreateConnectionToGAIEvents()");
		_LOGMSG.LogMsg(sMsg,LOG_LOGIC_ERROR);			
		
	}


	return lRetCode;
}


long CAlarmSrv::InitAlarmSrv(long lAlarmSrvActive)
{
	long lRetCode = 0;
	
	lRetCode = CreateConnectToAlarmSrv();

	CString sMsg;
	sMsg.Format("InitAlarmSrv lRetCode=%ld ", lRetCode);
	_LOGMSG.LogMsg(sMsg);			

	m_lAlarmSrvActive= lAlarmSrvActive;

	return lRetCode;

}

/******************************************************************************
 Description:	Init the alarms counters  	
 Returns:      	None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ravit 			29/12/2005   12:13		Start
******************************************************************************/
long CAlarmSrv::InitAlarmsNumerators()
{
	
	long lTotalAlarms = 0;
	long lRetVal;
	long lRetCode = CALARM_SRV_OK;
	long lDevice;
	long lFlags = GABC_ONLY_DISPLAY_MESSAGES;
	CComBSTR  bstrElement = NULL;
	bstrElement.Empty(); //4.0.15.287

	// Clear all Buffer.//4.0.3.16
	memset(m_lAlarmPumpMap , 0 ,sizeof(m_lAlarmPumpMap));
	memset(m_lAlarmSpecialGroupMap ,0,sizeof(m_lAlarmSpecialGroupMap));
	memset(m_lAlarmTankMap , 0, sizeof(m_lAlarmTankMap));
	
	_Module.m_server.ResetSeverityAlarmsCount(); //4.0.13.500

	lRetVal = m_pGAI->GetAlarmById(301013000,lFlags,&bstrElement);

	//found OLA alarm
	if (!lRetVal)
	{
		m_lAlarmSpecialGroupMap[ALARM_GROUP_OLA]++;
	}

	for (lDevice=1; lDevice<MAX_ALARM_TANKS; lDevice++)
	{
		lRetVal = m_pGAI->GetAlarmByCategory(CATEGORY_TANK,0,lDevice,lFlags,&lTotalAlarms,&bstrElement);

		if (lTotalAlarms > 0)
		{

			m_lAlarmSpecialGroupMap[ALARM_GROUP_TANK] += lTotalAlarms;
			m_lAlarmTankMap[lDevice] += lTotalAlarms;
			lTotalAlarms = 0;
		}
	}

	for (lDevice=1; lDevice<MAX_ALARM_PUMP; lDevice++)
	{
		lRetVal = m_pGAI->GetAlarmByCategory(CATEGORY_PUMP,0,lDevice,lFlags,&lTotalAlarms,&bstrElement);

		if (lTotalAlarms > 0)
		{
			m_lAlarmPumpMap[lDevice] += lTotalAlarms;
			lTotalAlarms = 0;
		}
	}

	lRetVal = m_pGAI->GetAlarmByCategory(CATEGORY_SOFTWARE,0,lDevice,lFlags,&lTotalAlarms,&bstrElement);

	m_lAlarmPumpMap[0] += lTotalAlarms;
	lTotalAlarms = 0;

	lRetVal = m_pGAI->GetAlarmByCategory(CATEGORY_DEVICES,0,lDevice,lFlags,&lTotalAlarms,&bstrElement);


	m_lAlarmPumpMap[0] += lTotalAlarms;
	lTotalAlarms = 0;

	lRetVal = m_pGAI->GetAlarmByCategory(CATEGORY_CONTROLLER,0,lDevice,lFlags,&lTotalAlarms,&bstrElement);

	m_lAlarmPumpMap[0] += lTotalAlarms;
	lTotalAlarms = 0;

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;
		str.Format("Init Alarms counters, Tank counter=%ld",m_lAlarmSpecialGroupMap[ALARM_GROUP_TANK]);
		_LOGMSG.LogMsg(str);			

	}

	return lRetCode;

}


