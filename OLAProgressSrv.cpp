// OLAProgressSrv.cpp: implementation of the COLAProgressSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLAProgressSrv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLAProgressSrv::COLAProgressSrv()
{

}

COLAProgressSrv::~COLAProgressSrv()
{

}


void COLAProgressSrv::Init()
{
	HRESULT hRes;

	if ( m_bOLAStopped == FALSE )
	{
		EnterCriticalSection(&m_csOLASrv);

		try
		{
			if (_Module.m_server.GetOLAThreadState() == OLA_SERVER_OFF)
			{
				if (!m_bValid && m_byConnectionAttempts)
				{
					hRes = m_pSrvPtr.CreateInstance(CLSID_Service);			

					if(FAILED(hRes))
					{
						_com_issue_error(hRes);
						_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF;
					}

					else
					{			
						LoadRegistryParams();
						LaunchOLA();

						//4.0.2.37
						ResetConnectionAttempts();
					}
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


long COLAProgressSrv::QuerySession(long lSessionNumber, BSTR * pOutBuffer)
{
	long lRtc = 0;
	CString sLogMsg;

	EnterCriticalSection(&m_csOLASrv);

	sLogMsg.Format("QuerySession %ld before" , lSessionNumber );
	_LOGMSG.LogMsg(sLogMsg);

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

	sLogMsg.Format("QuerySession %ld after Rtc=%ld" , lSessionNumber ,lRtc);
	_LOGMSG.LogMsg(sLogMsg);

	LeaveCriticalSection(&m_csOLASrv);

	return lRtc;
}

long COLAProgressSrv::StartSession(char * pInBuffer, char *pszType)
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


long COLAProgressSrv::AddParams(long lSessId, char * pInBuffer)
{
	long lRtc = 0;

	EnterCriticalSection(&m_csOLASrv);

	_bstr_t sInBuffer(pInBuffer);

	try
	{					
		lRtc =	m_pSrvPtr->AddParams (lSessId,sInBuffer);
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


long COLAProgressSrv::EndSession(long lSessId)
{
	long lRtc = 0;
	CString sLogMsg;
	
	EnterCriticalSection(&m_csOLASrv);


	sLogMsg.Format("EndSession %ld before" , lSessId );
	_LOGMSG.LogMsg(sLogMsg);


	try
	{							
		lRtc = m_pSrvPtr->EndSession (lSessId);
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

	sLogMsg.Format("EndSession %ld after error=%ld " , lSessId , lRtc );
	_LOGMSG.LogMsg(sLogMsg);


	return lRtc;
}


void COLAProgressSrv::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE */)
{
	EnterCriticalSection(&m_csOLASrv);
		
	if(m_bValid)
	{
		m_bValid = FALSE;//4.0.3.45
		try
		{
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
