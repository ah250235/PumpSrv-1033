// Hydra.cpp: implementation of the CHydra class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Hydra.h"
#include "Convert.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHydra::CHydra()
{
	m_lHydraActive = 0;
	m_lInterfacesFlag = 0;
	m_lGroupNum = 0;
	m_lHydStatus = HYD_NOT_ACTIVE;
	m_lRecoveryTime = 0;
	m_pRTDB = NULL;
	m_pCFGDB = NULL;
	m_pCTRL = NULL;
	memset(m_szServerPath,0,MAX_SERVER_NAME);
	memset(m_szServerIniFilePath,0,MAX_SERVER_NAME); //4.0.12.506
	memset(m_szClientName,0,MAX_CLIENT_NAME);

	memset(m_byDummyArray,' ',2048); //4.0.19.502
}


CHydra::~CHydra()
{
	
}


long CHydra::InitHydra(long lHydraActive,char* pszServerPath,char* pszServerIniFilePath,long lInterfacesFlags,long lGroupNum,char* pszClientName,long lRecoveryTime)
{
	long lRetCode = CHYDRA_OK;

	m_lHydraActive = lHydraActive;
	m_lInterfacesFlag = lInterfacesFlags;
	m_lGroupNum = lGroupNum;
	m_lRecoveryTime = lRecoveryTime;

	//4.0.12.506
	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("Init Hydra connection, lHydraActive=%ld,pszServerPath=%s,pszServerIniFilePath=%s,lInterfacesFlags=%ld,lGroupNum=%ld,pszClientName=%s,lRecoveryTime=%ld",lHydraActive,pszServerPath,pszServerIniFilePath,lInterfacesFlags,lGroupNum,pszClientName,lRecoveryTime); 
		_LOGMSG.LogMsg(str);
	}

	if(pszServerPath)
	{
		if (strlen(pszServerPath) > MAX_SERVER_NAME)
			lRetCode = CHYDRA_SERVER_NAME_TOO_LONG; //
		else
			strcpy(m_szServerPath,pszServerPath);
	}

	//4.0.12.506
	if(pszServerIniFilePath)
	{
		if (strlen(pszServerIniFilePath) > MAX_SERVER_NAME)
			lRetCode = CHYDRA_SERVER_NAME_TOO_LONG; //
		else
			//strcpy(m_szServerPath,pszServerIniFilePath);
			strcpy(m_szServerIniFilePath,pszServerIniFilePath); //4.0.15.150
	}

	if(pszClientName)
	{
		if (strlen(pszClientName) > MAX_CLIENT_NAME)
			lRetCode = CHYDRA_SERVER_NAME_TOO_LONG; //
		else
			strcpy(m_szClientName,pszClientName);
	}

	if (m_lHydraActive > 0)
	{
		if(m_lInterfacesFlag & HYDRA_CFGDB_INT)
			lRetCode = CreateConnectionToCFG();
		
		if(m_lInterfacesFlag & HYDRA_RTDB_INT)
			lRetCode = CreateConnectionToRT();
		
		if(m_lInterfacesFlag & HYDRA_CTRL_INT)
			lRetCode = CreateConnectionToCTL();
		
		if(m_lInterfacesFlag & HYDRA_RTEVENTS_INT)
			lRetCode = CreateConnectionToRTEvents();
		
		if(m_lInterfacesFlag & HYDRA_CFGEVENTS_INT)
			lRetCode = CreateConnectionToCFGEvents();
	}
	if(lRetCode == CHYDRA_OK)
		m_lHydStatus = HYD_ACTIVE;
	
	if(LOG_DETAIL_FLOW_CONTROL)
	{
		_LOGMSG.LogMsg("Init Hydra connection, END"); //4.0.19.501
	}

	return lRetCode;
}


long CHydra::CloseConnectionToHydra()
{
	long lRetCode = CHYDRA_OK;
	
	CloseConnectionToCFG();
	CloseConnectionToRT();
	CloseConnectionToCTL();
	
	return lRetCode;
}

long CHydra::CloseConnectionToRT()
{
	long lRetCode = CHYDRA_OK;
	HRESULT  hr = 0;
	
	if(m_pRTDB != NULL)
	{
		try
		{
			//checking if Event class was created
			if(_pRTEventHandler)
			{
				long lClientId = _pRTEventHandler->GetEventClientId();
				m_pRTDB->CloseRTEventsInterface(lClientId,m_lGroupNum);
				IUnknownPtr pUnk = m_pRTDB;
				_pRTEventHandler->DispEventUnadvise(pUnk);
				delete _pRTEventHandler;
		
				_pRTEventHandler = NULL;
			}
			
			m_pRTDB.Release();
			m_pRTDB.Detach();
			m_pRTDB = NULL;
		}
		catch(...)
		{
			m_pRTDB = NULL;
			lRetCode = CHYDRA_COM_ERROR;
			_LOGMSG.LogMsg("CHydra::CloseConnectionToRT ,Unexpected error has happened",LOG_LEVEL_1);//4.0.19.502
		}
	}
	return lRetCode;
}

long CHydra::CloseConnectionToCFG()
{
	long lRetCode = HYDRA_OK;
	HRESULT  hr = 0;
	
	if(m_pCFGDB != NULL)
	{
		try 
		{
			//checking if Event class was created
			if(_pCFGEventHandler)
			{
				long lClientId = _pCFGEventHandler->GetEventClientId();
				m_pCFGDB->CloseCFGEventsInterface(lClientId,m_lGroupNum);
				IUnknownPtr pUnk = m_pCFGDB;
				_pCFGEventHandler->DispEventUnadvise(pUnk);
				delete _pCFGEventHandler;
		
				_pCFGEventHandler = NULL;
			}
			m_pCFGDB.Release();
			m_pCFGDB.Detach();
			m_pCFGDB = NULL;
		}
		catch(...)
		{
			lRetCode = CHYDRA_COM_ERROR;
			m_pCFGDB = NULL;
			_LOGMSG.LogMsg("CHydra::CloseConnectionToCFG ,Unexpected error has happened",LOG_LEVEL_1);//4.0.19.502
		}
	}		
	return lRetCode;
}

long CHydra::CloseConnectionToCTL()
{
	long lRetCode = HYDRA_OK;
	HRESULT  hr = 0;

	if(m_pCTRL != NULL)
	{
		try 
		{
			m_pCTRL.Release();
			m_pCTRL.Detach();
			m_pCTRL = NULL;
		}
		catch(...)
		{
			lRetCode = CHYDRA_COM_ERROR;
			m_pCTRL = NULL;
			_LOGMSG.LogMsg("CHydra::CloseConnectionToCTL ,Unexpected error has happened",LOG_LEVEL_1);//4.0.19.502
		}
	}
	return lRetCode;
}

long CHydra::Restore_COM(HRESULT hResFailure, long lComType)
{
	long lRetCode = 0;

	switch(lComType)
	{
		case RTDB_TYPE:
		{
			if(hResFailure == 0x800706bf)
				m_pRTDB = NULL;

			if(m_pRTDB)
				m_pRTDB = NULL;

			lRetCode = CreateConnectionToRT();
		}
		case CFGDB_TYPE:
		{
			if(hResFailure == 0x800706bf)
				m_pCFGDB = NULL;

			if(m_pCFGDB)
				m_pCFGDB = NULL;

			lRetCode = CreateConnectionToCFG();
		}
		case CTRL_TYPE:
		{
			if(hResFailure == 0x800706bf)
				m_pCTRL = NULL;

			if(m_pCTRL)
				m_pCTRL = NULL;

			lRetCode = CreateConnectionToCTL();

		}
	}
	
	return lRetCode;
}


long CHydra::CreateConnectionToRT()
{
	long lRetVal = CHYDRA_OK;
	HRESULT hRes = 0;

	try
	{			
		hRes = m_pRTDB.CreateInstance(CLSID_RT);

		if(SUCCEEDED(hRes))		
		{
			lRetVal = CHYDRA_OK;
			
			CString str; //4.0.19.501	
			str.Format("CHydra::CreateConnectionToRT m_pRTDB = %ld",m_pRTDB);
			_LOGMSG.LogMsg(str);
		}
		else
		{
			CString str;	
			str.Format("CHydra::CreateConnectionToRT fail m_pRTDB = %ld",m_pRTDB);
			_LOGMSG.LogMsg(str);

			m_pRTDB = NULL; // also release the object
			lRetVal = CHYDRA_FAIL_CREATE_RTDB_CONN;
		}

	}
	catch (_com_error& e)
	{
		WriteComError(e);
		m_pRTDB = NULL; // also release the object
		lRetVal = CHYDRA_FAIL_CREATE_RTDB_CONN;		
	}

	return lRetVal;
}

long CHydra::CreateConnectionToCFG()
{
	long lRetVal = CHYDRA_OK;
	HRESULT hRes = 0;

	try
	{			
		hRes = m_pCFGDB.CreateInstance(CLSID_CFG);

		if(SUCCEEDED(hRes))		
		{
			lRetVal = CHYDRA_OK;
			
			//4.0.19.501
			CString str;	
			str.Format("CHydra::CreateConnectionToCFG m_pCFGDB = %ld",m_pCFGDB);
			_LOGMSG.LogMsg(str);
		}
		else
		{
			//4.0.19.501
			CString str;	
			str.Format("CHydra::CreateConnectionToRT fail m_pRTDB = %ld",m_pRTDB);
			_LOGMSG.LogMsg(str);
			
			m_pCFGDB = NULL; // also release the object
			lRetVal = CHYDRA_FAIL_CREATE_CFGDB_CONN;
		}

	}
	catch (_com_error& e)
	{
		WriteComError(e);
		m_pCFGDB = NULL; // also release the object
		lRetVal = CHYDRA_FAIL_CREATE_CFGDB_CONN;
	}
	return lRetVal;
}


long CHydra::CreateConnectionToCTL()
{
	long lRetVal = CHYDRA_OK;
	HRESULT hRes = 0;

	try
	{			
		hRes = m_pCTRL.CreateInstance(CLSID_CTL);

		if(SUCCEEDED(hRes))		
		{
			lRetVal = CHYDRA_OK;	
			
			//4.0.19.501
			CString str;	
			str.Format("CHydra::CreateConnectionToCTL m_pCTRL = %ld",m_pCTRL);
			_LOGMSG.LogMsg(str);
		}
		else
		{
			CString str;	
			str.Format("CHydra::CreateConnectionToCTL fail m_pCTRL = %ld",m_pCTRL);
			_LOGMSG.LogMsg(str);
			
			m_pCTRL = NULL; // also release the object
			lRetVal = CHYDRA_FAIL_CREATE_CTRL_CONN;
		}
	}
	catch (_com_error& e)
	{
		WriteComError(e);
		m_pCTRL = NULL; // also release the object
		lRetVal = CHYDRA_FAIL_CREATE_CTRL_CONN;		
	}

	return lRetVal;
}

long CHydra::CreateConnectionToCFGEvents()
{
	long lRetVal = CHYDRA_OK;
	HRESULT hRes = 0;
	long lEventsClientId;

	CComBSTR sClientName(m_szClientName);

	try
	{
		lRetVal = m_pCFGDB->InitCFGEventsInterface(0,m_lGroupNum,sClientName.m_str,&lEventsClientId);
		
		if(lRetVal)
		{
			lRetVal = CHYDRA_FAIL_CREATE_CFGEVENT_CONN;	
		}
		else
		{
			//setting to NULL befor starting.
			_pCFGEventHandler = NULL;
			
			_pCFGEventHandler = new CCFGEventsHandler;
			// Advise the server of the sink.
			IUnknownPtr pUnk = m_pCFGDB;
			hRes = _pCFGEventHandler->DispEventAdvise(pUnk);
			if ( FAILED(hRes) )
			{
				lRetVal = CHYDRA_FAIL_CREATE_CFGEVENT_CONN;	
				_pCFGEventHandler = NULL;
			}
			else
			{
				//setting the event client id
				_pCFGEventHandler->SetEventClientId(lEventsClientId);
			}
		}
	}
	catch (_com_error& e)
	{
		WriteComError(e);
		_pCFGEventHandler = NULL;
		lRetVal = CHYDRA_FAIL_CREATE_CFGEVENT_CONN;		
	}

	return lRetVal;
}

long CHydra::CreateConnectionToRTEvents()
{
	long lRetVal = CHYDRA_OK;
	HRESULT hRes = 0;
	long lEventsClientId;

	CComBSTR   sClientName(m_szClientName);

	try
	{
		lRetVal = m_pRTDB->InitRTEventsInterface(0,m_lGroupNum,sClientName.m_str,&lEventsClientId);
		
		if(lRetVal)
		{
			lRetVal = CHYDRA_FAIL_CREATE_RTEVENT_CONN;	
		}
		else
		{
			_pRTEventHandler = new CRTEventsHandler;
			// Advise the server of the sink.
			IUnknownPtr pUnk = m_pRTDB;
			hRes = _pRTEventHandler->DispEventAdvise(pUnk);
			if ( FAILED(hRes) )
			{
				lRetVal = CHYDRA_FAIL_CREATE_RTEVENT_CONN;	
				_pRTEventHandler = NULL;
			}
			else
			{
				//setting the event client id
				_pRTEventHandler->SetEventClientId(lEventsClientId);
			}
		}
	}
	catch (_com_error& e)
	{
		WriteComError(e);
		_pRTEventHandler = NULL;
		lRetVal = CHYDRA_FAIL_CREATE_RTEVENT_CONN;		
	}

	return lRetVal;
}

void CHydra::SetHydStatus(long lStatus)
{
	m_lHydStatus = lStatus;
}

void CHydra::SetLastTime(SYSTEMTIME cSysTime)
{
	m_cLastSysTime = cSysTime;
}

/******************************************************************************
 Description:	Checks if connection to HydService is active 	
 Returns:      	BOOL - TRUE if active FALSE if not active.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			11/2/2002   14:35		Start
******************************************************************************/

BOOL CHydra::CheckConnectionStatus()
{
	BOOL bRetVal = FALSE;
	long lRetVal = 0;
	long lResult = 0;
	long lTimeDiff = 0;

	if(m_lHydStatus == HYD_ACTIVE)
		bRetVal = TRUE;
	else
	{
		if(m_lHydStatus == HYD_NOT_ACTIVE)
		{
			SYSTEMTIME cCurrSysTime;
			GetLocalTime(&cCurrSysTime);

			lResult = CompareSystemTime(&cCurrSysTime,&m_cLastSysTime,&lTimeDiff,INTERVAL_IN_MINUTES);
			if( lResult == 1 && lTimeDiff > m_lRecoveryTime )
			{
				lRetVal = TryToRestoreHydConn();
				if(lRetVal == CHYDRA_NOT_CONNECTED)
					bRetVal = FALSE;		
			}
		}
		else
		{
			lRetVal = TryToRestoreHydConn();
			if(lRetVal == CHYDRA_NOT_CONNECTED)
					bRetVal = FALSE;
		}
	}

	return bRetVal;
}
long CHydra::TryToRestoreHydConn(HRESULT hRes)
{
	long lRetVal = 0;
	CString  str1;
	try	//4.0.19.502
	{
		CString str;
		
		//4.0.19.502
		//we are about to write the dummy buffer, make sure it is NULL termianted, it might include garbadge
		_HOST.m_byDummyArray[500] = '\0'; 
		str1.Format("CHydra::TryToRestoreHydConn m_byDummyArray = %s", _HOST.m_byDummyArray); //4.0.19.502
		_LOGMSG.LogMsg(str1); 
		
		//before recreating connection closing old connection.		
		if (hRes != 0xC0000005) //4.0.20.56
		{
			CloseConnectionToHydra();
			_LOGMSG.LogMsg("CHydra::TryToRestoreHydConn",LOG_LEVEL_1); 
		}
		
		lRetVal = InitHydra(m_lHydraActive,m_szServerPath,m_szServerIniFilePath,m_lInterfacesFlag,m_lGroupNum,m_szClientName,m_lRecoveryTime); //4.0.12.506 add ServerIniFilePath	
		
		if(lRetVal != CHYDRA_OK)
		{
			if(m_lHydStatus == HYD_ACTIVE)
				m_lHydStatus = HYD_CONN_FAIL1;
			else if(m_lHydStatus == HYD_CONN_FAIL1)
				m_lHydStatus = HYD_CONN_FAIL2;
			else if(m_lHydStatus == HYD_CONN_FAIL2)
				m_lHydStatus = HYD_CONN_FAIL3;
			else if(m_lHydStatus == HYD_CONN_FAIL3)
				m_lHydStatus = HYD_NOT_ACTIVE;
			else if(m_lHydStatus == HYD_NOT_ACTIVE)
				SetLocalTime(&m_cLastSysTime);

			lRetVal = CHYDRA_NOT_CONNECTED;
		}
	}
	catch(...)	//4.0.19.502
	{
		_LOGMSG.LogMsg("CHydra::TryToRestoreHydConn, Unexpected error has occured",LOG_LEVEL_1);
	}
	
	return lRetVal;
}