// OLA32EventSrv.cpp: implementation of the COLA32EventSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLA32EventSrv.h"

#include "OLA32Events.h" 
extern 	COLA32Events *_pOLA32Events;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLA32EventSrv::COLA32EventSrv()
{

}

COLA32EventSrv::~COLA32EventSrv()
{
	//ReleaseService(TRUE);//4.0.3.45
}

void COLA32EventSrv::Init()
{
	HRESULT hRes = 0;
	BOOL bSuccessfullConnection = FALSE;

	EnterCriticalSection(&m_csOLASrv);

	//4.0.2.37 Added if statement
	if(m_byConnectionAttempts)
	{
		if(!m_pEventUnknown)
		{
			try
			{
				bSuccessfullConnection = InitInterface();
			}

			catch(_com_error &cError)
			{
				Error(cError);
			}

			catch(...)
			{
				CString sMsg("Failed to initialize - general error");
				_LOGMSG.LogMsg(sMsg , LOG_LEVEL_0);
			}
		}

		//4.0.2.37 start
		if(bSuccessfullConnection)
		{
			LoadRegistryParams();
			LaunchOLA();

			ResetConnectionAttempts();
		}

		else
		{
			_LOGMSG.LogMsg("COLA32EventSrv::Init() - failed to initialize" , LOG_LEVEL_0);
			DecrementConnectionAttempts();
		}
		//4.0.2.37 end
	}

	LeaveCriticalSection(&m_csOLASrv);

	return;
}

void COLA32EventSrv::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE*/)
{
	HRESULT hRes = 0;
	EnterCriticalSection(&m_csOLASrv);
		
	if(m_bValid)
	{
		m_bValid = FALSE;//4.0.3.45
		
		try
		{
			hRes = _pOLA32Events->DispEventUnadvise(m_pEventUnknown);
			
			if(FAILED(hRes))
			{
				CString str;
				str.Format("Failed to unadvise OLA, error %x" , hRes);
				_LOGMSG.LogMsg(str);
			}

			delete _pOLA32Events;
			_pOLA32Events = NULL;

			m_pEventUnknown->Release();
			m_pDispatch->Release();
			m_pUnknown->Release();
		}

		catch (...)
		{

		}

		if (bCloseThread)
			_Module.m_server.CloseOLAThread();  	
		else
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_WAIT_FOR_KILL;

		//m_bValid = FALSE;
	}
	
	LeaveCriticalSection(&m_csOLASrv);
}

BOOL COLA32EventSrv::InitInterface()
{
	HRESULT hRes = S_OK;
	BOOL bRetCode = FALSE;
	CLSID clsid;

	//Initialize basic IServices interface
	COLA32Srv::InitInterface();

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
	
	if(SUCCEEDED(hRes))
		bRetCode = TRUE;

	else
		_LOGMSG.LogMsg("COLA32EventSrv::InitInterface(), failed to connect");

	return bRetCode;
}