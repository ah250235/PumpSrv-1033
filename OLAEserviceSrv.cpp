// OLAEserviceSrv.cpp: implementation of the COLAEservice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLAEserviceSrv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLAEserviceSrv::COLAEserviceSrv()
{

}

COLAEserviceSrv::~COLAEserviceSrv()
{

}


/////////////////////////////////////////////////////////
//OLA initialization.
/////////////////////////////////////////////////////////
/******************************************************************************
 Description:	Init the OLA server COM object
  	
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AlexM			17/07/11       		Start - TD 212899
******************************************************************************/
void  COLAEserviceSrv::Init()
{
   
	HRESULT hRes = S_OK;
	
	try
    {
        HRESULT hr = S_OK; 
	
		CLSID clsid; 
		
		//Get the CLSID for the interface.

        hRes = CLSIDFromProgID(L"SPEftAdapterLaunch.Service.1" , &clsid);
	
        //Create the COM object
        
        hr = CoCreateInstance(clsid , 0, CLSCTX_ALL, __uuidof(ISPEftAdapterLinkServerPtr), (void**)&m_pSrvPtr );
        if( FAILED(hr) )
            _com_issue_error(hr);
		else
			
		{
           _LOGMSG.LogMsg("Succesfully initialized SPEftAdapterLinkServerPtr");
 		    long lPumpNumber= 0;
 		   m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber);  

		   LoadRegistryParams();
		   LaunchOLA();

           ResetConnectionAttempts();
		}



    }
    catch( _com_error e )
    {
		CString str;
		str.Format("Failed to access SPEftAdapterLinkServerPtr: hRes %x : %.50s",hRes, e.ErrorMessage()); 
		_LOGMSG.LogMsg("SPEftAdapterPosServer","Error",0,LOG_NONE,str,LOG_LEVEL_1);
    }
 } 


 
/////////////////////////////////////////////////////////
//Card data extraction methods.
/////////////////////////////////////////////////////////
long COLAEserviceSrv::GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags,long *lSesionType, long lPumpNumber, PAY_AT_PUMP_INFO * pInPAPBuffer)	
{
	long lRtc = OLASRV_NOT_SUPPORTED;
    // not implemented in Eservices Server mode
   _LOGMSG.LogMsg("GetCardInfo did not implemented in Eservices Server mode");
	return lRtc;
}

long COLAEserviceSrv::AddParams(long lSessId , char * pInBuffer)
{
	long lRtc = 0;

	EnterCriticalSection(&m_csOLASrv);

	_bstr_t sInBuffer(pInBuffer);

	try
	{					
		lRtc =	m_pSrvPtr->AddParams(lSessId,sInBuffer);
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
long COLAEserviceSrv::StartSession(char * pInBuffer, char *pszType)
{
	_bstr_t sTransactionType(pszType);
	_bstr_t sInBuffer(pInBuffer);
	long lSessionNumber = 0;

	EnterCriticalSection(&m_csOLASrv);

	try
	{

    	lSessionNumber = m_pSrvPtr->StartSession(SERVER_OLA_POS_NUMBER , sInBuffer,sTransactionType);
        

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

long COLAEserviceSrv::QuerySession(long lSessionNumber, BSTR * pOutBuffer)
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

long COLAEserviceSrv::EndSession(long lSessionId)
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
void COLAEserviceSrv::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE*/)
{
	EnterCriticalSection(&m_csOLASrv);
		
	if(m_bValid)
	{
		m_bValid = FALSE;	
		try
		{
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

	}
	
	LeaveCriticalSection(&m_csOLASrv);
}


long COLAEserviceSrv::UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData)
{
	long lRtc = OLASRV_NOT_SUPPORTED;
    // not implemented in Eservices Server mode
    _LOGMSG.LogMsg("GetCardInfo did not implemented in Eservices Server mode");
	return lRtc;	
}




/////////////////////////////////////////////////////////
//Card data extraction methods.
/////////////////////////////////////////////////////////
long COLAEserviceSrv::GetCardInfoEx(long lClientID,BSTR sInBuffer,BSTR *sRetData,long lFlags )
{
	long lRtc = 0;
	CComBSTR sRetBuf;

	EnterCriticalSection(&m_csOLASrv);

	try
	{	
		
		sRetBuf.AppendBSTR(m_pSrvPtr->GetCardInfoEx(lClientID,sInBuffer,""));
	}

	catch (_com_error & e)
	{
		Error(e);		
		lRtc = OLASRV_COM_ERROR;
		_Module.m_server.CloseOLAThread();
	}
	
	catch(...)
	{
		Error();		
		lRtc = OLASRV_COM_ERROR;
		_Module.m_server.CloseOLAThread();
	}

	LeaveCriticalSection(&m_csOLASrv);
	
	if (lRtc == 0 && sRetBuf.Length() != 0)	
	{
		*sRetData = sRetBuf.Copy();
	}

	return lRtc;
}
