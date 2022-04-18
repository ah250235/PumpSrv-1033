// OLAPositive_FIPAYSrv.cpp: implementation of the COLAPositive_FIPAYSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLAPositive_FIPAYSrv.h"

#include "OLA32Events.h"
extern COLA32Events * _pOLA32Events;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLAPositive_FIPAYSrv::COLAPositive_FIPAYSrv() : m_pEventUnknown(0)
{
	m_pSrvPtr = new IServicesDll;
}

COLAPositive_FIPAYSrv::~COLAPositive_FIPAYSrv()
{
	//ReleaseService(TRUE);//4.0.3.45
	//delete m_pSrvPtr;
}


void COLAPositive_FIPAYSrv::Init()
{
	HRESULT hRes;
	BOOL bEventsSupported = FALSE;

	if (!m_bOLAStopped)
	{
		EnterCriticalSection(&m_csOLASrv);

		try
		{
			if (_Module.m_server.GetOLAThreadState() == OLA_SERVER_OFF)
			{
				if (!m_bValid && m_byConnectionAttempts)
				{
					COleException *e = new COleException;
					hRes = S_OK;
					
					if (!m_pSrvPtr->CreateDispatch("Ola32.Services",e))
						hRes = COleException::Process(e);

					e->Delete();

					if(FAILED(hRes))
					{
						_com_issue_error(hRes);
						_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF;
					}

					else
						bEventsSupported = InitEventSink();

					if(!bEventsSupported)
					{
						//Use only basic interface, that is the OLA interface without events.
						_LOGMSG.LogMsg("COLAPositive_FIPAYSrv::Init() failed to init events interface");
					//	_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF;
					//	_com_issue_error(S_FALSE);
					}

					LoadRegistryParams();
					LaunchOLA();

					//4.0.2.37
					ResetConnectionAttempts();
				}    
			}
		}
		
		catch (_com_error& e)
		{
			Error(e,hRes,FALSE);
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF; //ola12345
			
			//4.0.2.37
			DecrementConnectionAttempts();
		}

		//4.0.2.37
		catch (...)
		{
			Error();
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF; //ola12345
			
			//4.0.2.37
			DecrementConnectionAttempts();
		}

		LeaveCriticalSection(&m_csOLASrv);
	}
}

BOOL COLAPositive_FIPAYSrv::InitEventSink()
{
	HRESULT hRes = S_OK;
	BOOL bRetCode = FALSE;
	CLSID clsid;
	
	//Get the interfaces unique identifier
	hRes = CLSIDFromProgID(L"Ola32.Ola32CallBack" , &clsid);

	if(SUCCEEDED(hRes))
	{
		hRes = CoCreateInstance(clsid,
					            NULL,
						        CLSCTX_ALL,
						        IID_IUnknown,
						        (void FAR * FAR *)&m_pEventUnknown);
	}

	if(SUCCEEDED(hRes))
	{
		_pOLA32Events = new COLA32Events;

		//Advise the server of the sink
		hRes = _pOLA32Events->DispEventAdvise(m_pEventUnknown);
	}

	else
		_LOGMSG.LogMsg("COLA32EventSrv::InitInterface(), failed to create instance");
	
	if(SUCCEEDED(hRes))
		bRetCode = TRUE;

	else
	{
		CString sMsg;
		sMsg.Format("COLA32EventSrv::InitInterface(), failed to advise , error %x" , hRes);
		_LOGMSG.LogMsg(sMsg);
	}
	

	return bRetCode;
}

/////////////////////////////////////////////////////////
//Card data extraction methods.
/////////////////////////////////////////////////////////
long COLAPositive_FIPAYSrv::AddParams(long lSessId , char * pInBuffer)
{
	long lRtc = 0;

	EnterCriticalSection(&m_csOLASrv);

	_bstr_t sInBuffer(pInBuffer);

	try
	{					
		m_pSrvPtr->AddParams(lSessId,sInBuffer);
	}

	catch (_com_error & e)
	{
		Error(e);
		lRtc = OLASRV_COM_ERROR;
	}
	
	catch(...)
	{
		Error();
		lRtc = OLASRV_COM_ERROR;
	}

	LeaveCriticalSection(&m_csOLASrv);

	return lRtc;
}

/////////////////////////////////////////////////////////
//Session control methods.
/////////////////////////////////////////////////////////
long COLAPositive_FIPAYSrv::StartSession(char * pInBuffer, char *pszType)
{
	_bstr_t sTransactionType(pszType);
	_bstr_t sInBuffer(pInBuffer);
	long lSessionNumber;

	EnterCriticalSection(&m_csOLASrv);

	try
	{					
		lSessionNumber = m_pSrvPtr->StartSession(SERVER_OLA_POS_NUMBER, sInBuffer,sTransactionType);
	}
	
	catch (_com_error & e)
	{
		Error(e);
		lSessionNumber = OLASRV_COM_ERROR;
	}
	
	catch (...)
	{
		Error();
		lSessionNumber = OLASRV_COM_ERROR;
	}

	LeaveCriticalSection(&m_csOLASrv);

	return lSessionNumber;
}

long COLAPositive_FIPAYSrv::QuerySession(long lSessionNumber, BSTR * pOutBuffer)
{
	long lRtc = 0;
	
	EnterCriticalSection(&m_csOLASrv);

	try
	{							
		lRtc = m_pSrvPtr->QuerySession (lSessionNumber,pOutBuffer );
	}

	catch (_com_error& e)
	{
		lRtc = OLASRV_COM_ERROR;
		Error(e);
	}
	
	catch(...)
	{
		lRtc = OLASRV_COM_ERROR;
		Error();
	}

	LeaveCriticalSection(&m_csOLASrv);

	return lRtc;
}

long COLAPositive_FIPAYSrv::EndSession(long lSessionId)
{
	long lRetCode = 0;

	EnterCriticalSection(&m_csOLASrv);

	try
	{
		m_pSrvPtr->EndSession(lSessionId);
	}

	catch (_com_error& e)
	{
		lRetCode = OLASRV_COM_ERROR;
		Error(e);
	}

	catch(...)
	{
		lRetCode = OLASRV_COM_ERROR;
		Error();
	}

	LeaveCriticalSection(&m_csOLASrv);

	return lRetCode;
}

/////////////////////////////////////////////////////////
//OLA service control methods.
/////////////////////////////////////////////////////////
void COLAPositive_FIPAYSrv::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE */)
{
	EnterCriticalSection(&m_csOLASrv);
		
	if(m_bValid)
	{
		m_bValid = FALSE;//4.0.3.45
		try
		{
			_pOLA32Events->DispEventUnadvise(m_pEventUnknown);
			delete _pOLA32Events;
			
			m_pSrvPtr->ReleaseDispatch();

			m_pEventUnknown->Release();
		}

		catch (...)
		{

		}

		if (bCloseThread)
			_Module.m_server.CloseOLAThread();  	
		else
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_WAIT_FOR_KILL;

		//_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF;  //ola12345
		//m_bValid = FALSE;//4.0.3.45	
	}
	
	LeaveCriticalSection(&m_csOLASrv);
}
