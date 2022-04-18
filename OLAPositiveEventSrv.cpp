// OLAPositiveEventSrv.cpp: implementation of the COLAPositiveEventSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLAPositiveEventSrv.h"
#include "OLAEvents.h" 
extern 	COLAEvents *_pOLAEvents;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLAPositiveEventSrv::COLAPositiveEventSrv()
{
	SetIFSFProtocoll(TRUE); //4.0.7.507
}

COLAPositiveEventSrv::~COLAPositiveEventSrv()
{
	//ReleaseService(TRUE);//4.0.3.45
}


void COLAPositiveEventSrv::Init()
{
	HRESULT hRes = 0;

	//Eyal 4.0.0.28 added to this type of OLA
	//to run even if events are unavailable
	BOOL bBasicInterfaceCreated = FALSE;

	if (m_bOLAStopped == FALSE)
	{
		EnterCriticalSection(&m_csOLASrv);

		if(!m_bValid && m_byConnectionAttempts)
		{
			try
			{
				hRes = m_pSrvPtr.CreateInstance(CLSID_Services);
							
				if(SUCCEEDED(hRes))
				{
					long  lPumpNumber = 0;	
					bBasicInterfaceCreated = TRUE;
					hRes = m_pCallBack.CreateInstance(CLSID_CallBack);
								
					if(FAILED(hRes))
						_LOGMSG.LogOLAMsg(0, 0, 0, "OLAInUse = 5 >>> ICallBack NOT FOUND !!!", 0);
				
					else
					{						
						_pOLAEvents = new COLAEvents;

						// Advise the server of the sink.
						IUnknownPtr pUnk = m_pCallBack;
						hRes = _pOLAEvents->DispEventAdvise(pUnk);
					}
					
					//4.0.7.507
					m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber);
				}
						

				if(FAILED(hRes))
				{
					//Eyal 4.0.0.28 Start
					//If we failed to create a basic connection to OLA, exit
					if(!bBasicInterfaceCreated)
					{	
						_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF;
						_com_issue_error(hRes);
					}

					else  //At least a basic connection exists
					{
						_LOGMSG.LogMsg("OLA with events: COM failure, OLA server will have no event support" , LOG_LEVEL_0);
						LoadRegistryParams();
						LaunchOLA();
						
						//4.0.2.37
						ResetConnectionAttempts();
					}
					//Eyal 4.0.0.28 End
				}

				else
				{
					LoadRegistryParams();
					LaunchOLA();

					//4.0.2.37
					ResetConnectionAttempts();
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
		}
		
		LeaveCriticalSection(&m_csOLASrv);
	}
}

void COLAPositiveEventSrv::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE*/)
{
	HRESULT hRes = 0;
	EnterCriticalSection(&m_csOLASrv);
		
	if(m_bValid)
	{
		m_bValid = FALSE;//4.0.3.45
		try
		{
			IUnknownPtr pUnk = m_pCallBack;
			hRes = _pOLAEvents->DispEventUnadvise(pUnk);
			if(FAILED(hRes))
			{
				CString str;
				str.Format("Failed to unadvise OLA, error %x" , hRes);
				_LOGMSG.LogMsg(str);
			}

			delete _pOLAEvents;
			_pOLAEvents = NULL;

			m_pCallBack.Release();
			m_pCallBack.Detach();

			m_pSrvPtr.Release();
			m_pSrvPtr.Detach();
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

