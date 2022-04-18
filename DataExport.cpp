// DataExport.cpp: implementation of the CDataExport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataExport.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataExportThread::CDataExportThread(): m_lOpCode(DATAEXPORT_OPCODE_NONE),
	CThread(FALSE , 100 , TRUE)   //4.0.6.502

{
	m_pExport = NULL;
	m_bActive = FALSE;
}

CDataExportThread::~CDataExportThread()
{

}


long CDataExportThread::Restore_COM()
{
	CDataExportRetCode eRtc;
	CloseConnectionDataExport();
	eRtc = ConnectDataExport();

	return eRtc;
}

long CDataExportThread::WriteComError(_com_error &e)
{
	_LOGMSG.LogResult(
				0, // Src
				"CDataExportThread",
				"WriteComError",
				FALSE,0,
				TRUE,e.Error(),e);
	return 0;
}


/******************************************************************************
 Description:	Creating the Log string from the CDataExport return code	
 Returns:      	CString 
 Parameters:   	CDataExportRetCode cDataExportRetCode
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi			12/5/2004   13:51		Start
******************************************************************************/

CString CDataExportThread::CreateDataExportLog(CDataExportRetCode cDataExportRetCode)
{
	CString strLog;
	switch(cDataExportRetCode)
	{
			case DATA_EXPORT_OK:
				strLog.Format("Export Data passed successfully");
				break;
			case DATA_EXPORT_FAIL_CREATE_INTERFACE:	
				strLog.Format("Failed to create Data Export Interface");
				break;
			case DATA_EXPORT_COM_ERROR :	
				strLog.Format("Data Export COM Error");
				break;
			case DATA_EXPORT_NOT_CONNECTED :
				strLog.Format("Data Export not connected");
				break;
			case DATA_EXPORT_CUSTOMIZE_ERROR :
				strLog.Format("Data Export customization error");
				break;
			case DATA_EXPORT_LOG_UNCUSTOMIZED :	
				strLog.Format("Data Export uncustomized while server is connected!");
				break;
			default:
				strLog.Format("Data Export unknown error");

	}

	return(strLog);
}

/******************************************************************************
 Description:	The actual actions performed in the DataExport thread - connecting 
	            to it and notifying it of the relevant data
 Returns:      	
 Parameters:   	void *pData
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi			16/5/2004   14:58		Change:4.0.6.501
******************************************************************************/
void CDataExportThread::ThreadRoutine(void *pData)
{
	BOOL bShouldConnect = TRUE;
	CString strLog;
	CString strRetCode;
	CDataExportRetCode DataExpRetCode = DATA_EXPORT_OK;
	CThreadRetCode eThreadRetCode = THREAD_OK;
	
	//Varify connection to the Data Export
	DataExpRetCode = ConnectDataExport();
	
	if(DATA_EXPORT_OK == DataExpRetCode)
	{
		switch(m_lOpCode)
		{
			case DATAEXPORT_OPCODE_INIT_INT:
			{
				//Notifying the Data Export server of the data relevant to it					
				DataExpRetCode = InitDataExportInt();
				strLog.Format("CDataExportThread::ThreadRoutine:Opcode:%d",m_lOpCode);
				break;
			}
			default:
				strLog.Format("CDataExportThread::ThreadRoutine:Unknown Opcode");				
		}
		_LOGMSG.LogMsg(strLog);
		
	}
	else
	{
		strLog.Format("CDataExportThread::ThreadRoutine: connecting to DataExport failed. The reason: ");
		strRetCode = CreateDataExportLog(DataExpRetCode);
		strLog += strRetCode;
		_LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
	}

	eThreadRetCode = StopThread();
	if(eThreadRetCode != THREAD_OK)
		_LOGMSG.LogMsg("CDataExportThread::ThreadRoutine: Error in stopping the DataExport thread. Return Code: %d", eThreadRetCode);					

}


/******************************************************************************
 Description:	Starting the data export thread (waking it up from the suspened mode)
 Returns:      	CThreadRetCode
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi			16/5/2004   14:59		Change:4.0.6.501
******************************************************************************/

CThreadRetCode CDataExportThread::DataExportSrvInit(BOOL bActive)
{
	CThreadRetCode threadRetCode = THREAD_OK; 

	if(m_bActive == FALSE)
	{
		m_bActive = bActive;//4.0.6.507	
		//Starting the DataExport thread in case it's not started and it's
		//required to do so according to the registry
		threadRetCode = CreateThread();
		if( threadRetCode == THREAD_OK )
		{
			_LOGMSG.LogMsg("Data export thread was created");
		}
		else
		{
			_LOGMSG.LogMsg("Data export thread wasn't created");
			threadRetCode = THREAD_NOT_CREATED;
		}
	}


	return threadRetCode;
}

/******************************************************************************
 Description:	Closing the connection to the DataExport and killing its thread 
 Returns:      	CThreadRetCode
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi			16/5/2004   14:59		Start
******************************************************************************/

CThreadRetCode CDataExportThread::DataExportCleanUp(BOOL bDisconnFromPS)
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	if(m_bActive)
	{
		//4.0.7.502 - check if need to release pumpsrv connectiob
		if(bDisconnFromPS)
		{
			DisconnectFromPumpSrv();			
		}
		//Closing the connection to DataExport 
		CloseConnectionDataExport(); 
		//Killing the DataExport thread
		eThreadRetCode = CThread::KillThread();
		if(THREAD_OK == eThreadRetCode)
			_LOGMSG.LogMsg("DataExportThread terminated successfully");
		else
			_LOGMSG.LogMsg("DataExportThread not terminated successfully");
		
		m_bActive = FALSE;//4.0.7.502
	}

	return(eThreadRetCode);

}

/******************************************************************************
 Description:	Sets the operation code 	
 Returns:      	void
 Parameters:   	long - OpCode 
		
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			15/7/2004   8:54		Start
******************************************************************************/
void CDataExportThread::SetDataExportOpCode(long lOpCode)
{
	m_lOpCode = lOpCode;
}

CDataExportRetCode CDataExportThread::ConnectDataExport()
{
	CDataExportRetCode lRetCode = DATA_EXPORT_OK;
	HRESULT hRes = 0;

	if(m_bActive)
	{
		if(m_pExport == NULL)
		{
			try
			{			
				hRes = m_pExport.CreateInstance(CLSID_Export);

				if(SUCCEEDED(hRes))		
				{
					lRetCode = DATA_EXPORT_OK;				
					_LOGMSG.LogMsg("successfully connected to DataExportSrv");

				}
				else
				{
					m_pExport = NULL; // also release the object
					lRetCode = DATA_EXPORT_FAIL_CREATE_INTERFACE;
					_LOGMSG.LogMsg("Fail connect to DataExportSrv");
				}

			}
			catch (_com_error& e)
			{
				WriteComError(e);
				m_pExport = NULL; // also release the object
				lRetCode = DATA_EXPORT_FAIL_CREATE_INTERFACE;
			}
			catch(...)
			{
				if (LOG_LOGIC_ERROR )
				{
					CString sMsg;
					sMsg.Format("CDataExportThread::ConnectDataExport");
					_LOGMSG.LogMsg(sMsg,LOG_LOGIC_ERROR);			
				}
				m_pExport = NULL; // also release the object
				lRetCode = DATA_EXPORT_FAIL_CREATE_INTERFACE;
			}
		}
	}
	return lRetCode;
}

CDataExportRetCode CDataExportThread::CloseConnectionDataExport()
{
	CDataExportRetCode lRetCode = DATA_EXPORT_OK;
	
	if(m_pExport != NULL)
	{
		try
		{
			m_pExport.Release();
			m_pExport.Detach();
			m_pExport = NULL;
			_LOGMSG.LogMsg("CDataExportThread:: Closed the connection to DataExport");
		}
		catch (_com_error& e)  
		{
			WriteComError(e);
			m_pExport = NULL; 
			lRetCode = DATA_EXPORT_COM_ERROR;		
		}
		catch(...)
		{
			m_pExport = NULL;
			lRetCode = DATA_EXPORT_COM_ERROR;
		}
	}
	
	return lRetCode;
}

long CDataExportThread::NotifySystemStartup()
{
	CDataExportRetCode lRetCode = DATA_EXPORT_OK;

	if(m_bActive)//4.0.6.509
	{
		//Connecting to DataExport
		ConnectDataExport();

		SetDataExportOpCode(DATAEXPORT_OPCODE_INIT_INT);

		StartThread();
	}
	
	return lRetCode;
}

CDataExportRetCode CDataExportThread::InitDataExportInt()
{
	CDataExportRetCode DataExpRetCode = DATA_EXPORT_OK;
	long lRetVal = 0, lFlags = 0;
	CString strLog;
	CString strRetCode;
	
	if(m_pExport != NULL)
	{
		try
		{
			//Here to add the call to the interface
			CString cMsg;

			//4.0.24.90 106227
			if (_Module.m_server.GetSingleCoreActive())
			{
				lFlags |= DATAEXPORT_USES_SINGLE_CORE;
			}
			lRetVal = m_pExport->InitInterface(CLIENT_PUMPSRV,lFlags);//4.0.24.90 106227
			//rtt -> add here!!! TD 31281  
			cMsg.Format("CDataExportThread:: InitDataExportInt return:%d",lRetVal);
			_LOGMSG.LogMsg(cMsg);
		}
		catch (_com_error& e)  
		{
			WriteComError(e);
			m_pExport = NULL; 
			Restore_COM();
			DataExpRetCode = DATA_EXPORT_COM_ERROR;		
		}
		catch(...)
		{
			m_pExport = NULL;
			DataExpRetCode = DATA_EXPORT_COM_ERROR;
			Restore_COM();
		}
	}
	strLog.Format("CDataExportThread::ThreadRoutine: InitDataExportInt() result: ");
	strRetCode = CreateDataExportLog(DataExpRetCode);
	strLog += strRetCode;

	_LOGMSG.LogMsg(strLog);
	
	return DataExpRetCode;
}

BOOL CDataExportThread::IsActive()
{
	return m_bActive;
}

long CDataExportThread::DisconnectFromPumpSrv()
{
	CDataExportRetCode DataExpRetCode = DATA_EXPORT_OK;
	long lRetVal = 0;
	CString strLog;
	CString strRetCode;

	if(m_pExport != NULL)
	{
		try
		{
			//Here to add the call to the interface
			CString cMsg;
			lRetVal = m_pExport->ConnectionToPumpSrv(DISCONNECT_FROM_PUMPSRV);
			cMsg.Format("CDataExportThread:: ConnectionToPumpSrv return:%d",lRetVal);
			_LOGMSG.LogMsg(cMsg);
		}
		catch (_com_error& e)  
		{
			WriteComError(e);
			m_pExport = NULL; 
			Restore_COM();
			DataExpRetCode = DATA_EXPORT_COM_ERROR;		
		}
		catch(...)
		{
			m_pExport = NULL;
			DataExpRetCode = DATA_EXPORT_COM_ERROR;
			Restore_COM();
		}
	}
	
	return DataExpRetCode;
}
