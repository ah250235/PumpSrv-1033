// HydraPS.cpp: implementation of the CHydraPS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HydraPS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHydraPS::CHydraPS()
{
	m_lConnState = 0;
	m_lUpdateState = 0;
	m_lHydErrors = 0;
}

CHydraPS::~CHydraPS()
{

}

long CHydraPS::WriteComError(_com_error &e)
{
	long lRetVal = 0;

	_LOGMSG.LogResult(
				0, // Src
				"CHydraPS",
				"WriteComError",
				FALSE,0,
				TRUE,e.Error(),e);

	return lRetVal;

}

long CHydraPS::GetHydSrvInfo()
{
	long lRetVal = CHYDRA_OK;
	BSTR bstrVal = SysAllocString(L" ");
	
	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pCTRL != NULL  )
	{
		try
		{
			lRetVal = _HOST.m_pCTRL->GetServerInfo(&bstrVal,0);
			if(lRetVal == 0)
			{
				TAG_SERVER_INFO cTagInfo;
				memset(&cTagInfo,0,sizeof(TAG_SERVER_INFO));

				//4.0.2.15
				//_bstr_t tmp(bstrVal);
				
				CString cTmp(bstrVal);
				memcpy(&cTagInfo,cTmp,sizeof(TAG_SERVER_INFO));
				m_lConnState = atol((char*)cTagInfo.sConnState);
				m_lHydErrors = atol((char*)cTagInfo.sErrors);
				m_lUpdateState = atol((char*)cTagInfo.sUpdateDataState);
			}
			else
			{
				m_lConnState = 0;
				m_lUpdateState = 0;
			}
		}
		catch(_com_error & e )
		{
			m_lConnState = 0;
			m_lUpdateState = 0;
			lRetVal = CHYDRA_COM_ERROR;
			WriteComError(e);
		}
	}
	else
	{
		m_lConnState = 0;
		m_lUpdateState = 0;
		lRetVal = CHYDRA_NOT_CONNECTED;
	}

	
	return lRetVal;
}

/******************************************************************************
 Description:	Refreshes HydService info and returns connection to 
				other computer state.
 Returns:      	long - connection state. 0 - no connection.
									     1 - connection establish.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			22/4/2002   16:20		Start
******************************************************************************/

BOOL CHydraPS::IsHydSrvConnected()
{
	long lRetVal = GetHydSrvInfo();

	return m_lConnState;
}

/******************************************************************************
 Description:	Refreshes HydService info and returns if update complete 	
 Returns:      	long - update state. 0 - In update process.
									 1 - Process complete.
   	
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			22/4/2002   16:23		Start
******************************************************************************/

BOOL CHydraPS::IsUpdateComplete()
{
	long lRetVal = GetHydSrvInfo();

	return m_lUpdateState; 
}

/******************************************************************************
 Description:	Refreshes HydService info and returns errors value 	
 Returns:      	long - errors value. 0 - no errors. 
				Or combination of the follow:
				HYDRA_NOT_SYNC_ERROR_MISC		0x00000001
				HYDRA_QDX_FILES_ERROR_MISC 		0x00000002
				HYDRA_NO_QDX_ACTIVE_MISC	 	0x00000004
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			22/4/2002   16:25		Start
******************************************************************************/

long CHydraPS::GetHydServiceErrors()
{
	long lRetVal = GetHydSrvInfo();
	
	return m_lHydErrors;
}
