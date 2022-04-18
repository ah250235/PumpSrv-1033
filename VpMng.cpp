// VpMng.cpp: implementation of the CVpMng class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "VpMng.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVpMng::CVpMng()
{
	//m_hRes = CoInitialize(NULL);
	IServersPtr pIServersPtr = NULL;
}

CVpMng::~CVpMng()
{
	/*if(m_hRes)
		CoUninitialize();*/
}

VpStatus CVpMng::Disconnect()
{
	VpStatus enmRetVal = VP_OK;
	HRESULT hRes = 0;
	
	if(pIServersPtr != NULL)
	{	
		try
		{
			hRes = pIServersPtr->Stop();					
			hRes = pIServersPtr->StopInterface();
			
			if( (pIServersPtr != NULL) )
			{
				pIServersPtr.Release();
				pIServersPtr.Detach();
			}		
			
			if ( FAILED(hRes) )
			{
				enmRetVal = VP_FAIL_STOP;
			}
		}
		catch (...)
		{
			pIServersPtr = NULL;
			enmRetVal = VP_FAIL_STOP;
		}		
	}
	return enmRetVal;
}

VpStatus CVpMng::Connect(long lPumpSrvVersion)
{
	VpStatus enmRetVal = VP_OK;
	BOOL bIsVpStarted = FALSE;
	HRESULT hRes;

	try
	{
		if (pIServersPtr ==	NULL)	//4.0.24.60 Starting
		{
			hRes = pIServersPtr.CreateInstance(CLSID_Servers);
			if ( FAILED(hRes) )
			{
				enmRetVal = VP_FAIL_START;
				pIServersPtr = NULL;
			}
			if (pIServersPtr!= NULL)
			{
				if(LOG_BASIC_FLOW_CONTROL)
				{	
					_LOGMSG.LogMsg("Connecting to VP");			
				}
				pIServersPtr->VersionControl(lPumpSrvVersion);					
				bIsVpStarted = pIServersPtr->Start();
				if(bIsVpStarted == FALSE)
					enmRetVal = VP_FAIL_START;
			//	Sleep(60000);
			
			}
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg("VP","Error",0,LOG_NONE,"Failed to connect VP ",LOG_LEVEL_0);											
		pIServersPtr = NULL;
		enmRetVal = VP_FAIL_START;
	}		
	if (pIServersPtr!= NULL)	//Polling 
	{
		try
		{
			pIServersPtr->VersionControl(lPumpSrvVersion);					
			_Module.m_server.SetVpIsGone(FALSE);			
		}
		catch (_com_error & e)
		{
			CString str;
			str.Format("Failed to access VP : %.60s", e.ErrorMessage()); 

			_LOGMSG.LogMsg("VP","Error",0,LOG_NONE,str,LOG_LEVEL_0);											
			enmRetVal = VP_FAIL_START;
		}
		
		catch(...)
		{
			_LOGMSG.LogMsg("VP","Error",0,LOG_NONE,"Failed to access VP : Unspecified Error ",LOG_LEVEL_0);											
			enmRetVal = VP_FAIL_START;
		}
	}
	
	return enmRetVal;
}

