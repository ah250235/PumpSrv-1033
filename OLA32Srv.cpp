// OLAPositiveSrv.cpp: implementation of the COLAPositiveSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLA32Srv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLA32Srv::COLA32Srv() : m_pUnknown(0) , m_pDispatch(0)
{
	memset(MethodIDs , 0 , NUMBER_OF_IMPLEMENTED_METHODS * sizeof(DISPID));
}

COLA32Srv::~COLA32Srv()
{
	//ReleaseService(TRUE);//4.0.3.45
}

/////////////////////////////////////////////////////////
//OLA initialization.
/////////////////////////////////////////////////////////
void COLA32Srv::Init()
{
	BOOL bInitializationSuccessfull = FALSE;

	if(!m_bOLAStopped)
	{
		//4.0.2.37 Added m_byRetryAttempts
		if(m_byConnectionAttempts)
		{
			EnterCriticalSection(&m_csOLASrv);

			if(!m_pUnknown || !m_pDispatch)
			{
				try
				{
					//4.0.2.37 added return code check and subsequent
					//if statements
					bInitializationSuccessfull = InitInterface();
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
			if(bInitializationSuccessfull)
			{
				LoadRegistryParams();
				LaunchOLA();

				ResetRetryAttemps();
			}

			else
			{
				_LOGMSG.LogMsg("COLA32Srv::Init() - failed to initialize" , LOG_LEVEL_0);
				DecrementConnectionAttempts();
			}
			//4.0.2.37 end

			LeaveCriticalSection(&m_csOLASrv);
		}
	}

	return;
}

/******************************************************************************
 Description:	Dynamically create instances of IServices interface 	
 Returns:      	TRUE on success, otherwise FALSE
 Parameters:   	
 Assumptions:   Called within a CRITICAL SECTION
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			21/4/2002   16:00		Start
******************************************************************************/

BOOL COLA32Srv::InitInterface()
{
	BOOL bRetCode = FALSE;
	HRESULT hRes = S_OK;
	CLSID clsid;

	//Get the CLSID for the IServices interface.
	hRes = CLSIDFromProgID(L"Ola32.Services" , &clsid);

	if(SUCCEEDED(hRes))
	{
		hRes = CoCreateInstance(clsid,
			                    NULL,
								CLSCTX_ALL,
								IID_IUnknown,
								(void FAR * FAR *)&m_pUnknown);
	}
	else
	{
		CString sMsg;
		sMsg.Format("COLA32Srv::InitInterface() - CoCreateInstance Failed - returned %ld", hRes);
		_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);
	}

	if(SUCCEEDED(hRes))
	{
		hRes = m_pUnknown->QueryInterface(IID_IDispatch , (void**)&m_pDispatch);

		if(SUCCEEDED(hRes))
		{
			//Get DISPID handles to OLA session handling methods
			hRes = GetFunctionPointers();

			//if the last function call was successful,
			//we can change the returned value to true.
			if(SUCCEEDED(hRes))
				bRetCode = TRUE;
		}
	}

	if(FAILED(hRes))
		_LOGMSG.LogMsg("COLA32Srv::InitInterface(), failed to acquire interface" , LOG_LEVEL_0);
		
	return bRetCode;
}

/******************************************************************************
 Description:	Get DISPIDs for supported methods 	
 Returns:      	Microsoft COM return value
 Parameters:   	
 Assumptions:   Called within a CRITICAL SECTION
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			21/4/2002   15:52		Start
******************************************************************************/

HRESULT COLA32Srv::GetFunctionPointers()
{
	OLECHAR FAR * sMethodName[NUMBER_OF_IMPLEMENTED_METHODS];
	HRESULT hRes = S_OK;

	sMethodName[0] = L"AddParam";
	sMethodName[1] = L"StartSession";
	sMethodName[2] = L"EndSession";
	sMethodName[3] = L"QuerySession";

	//Fill the member array MethodIDs with the
	//DISPIDs of the methods stored in the sMethodName array.
	hRes = m_pDispatch->GetIDsOfNames(IID_NULL,
		                              sMethodName,
							          NUMBER_OF_IMPLEMENTED_METHODS,
									  LOCALE_USER_DEFAULT,
							          MethodIDs);
	
	if (FAILED(hRes))
	{

		CString sMsg;
		sMsg.Format("COLA32Srv::GetFunctionPointers() - GetIDsOfNames Failed - returned %ld", hRes);
		_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);
	}

	return hRes;
}

/////////////////////////////////////////////////////////
//Card data extraction methods.
/////////////////////////////////////////////////////////

long COLA32Srv::AddParams(long lSessId , char * pInBuffer)
{
	long lRtc = 0;
	unsigned int iErrArg = 0;
	CComVariant pArgs[2];
	VARIANT cRes;
	EXCEPINFO cException;

	HRESULT hRes = S_OK;

	CComBSTR sInBuffer(pInBuffer);
	
	pArgs[0].vt = VT_BSTR;
	pArgs[0].bstrVal = sInBuffer;

	pArgs[1].vt = VT_I4;
	pArgs[1].lVal = lSessId;

	DISPPARAMS cParams = { pArgs , NULL , 2 , 0 };

	EnterCriticalSection(&m_csOLASrv);
	hRes = m_pDispatch->Invoke(MethodIDs[0],
				               IID_NULL,
				        	   LOCALE_SYSTEM_DEFAULT,
							   DISPATCH_METHOD,
							   &cParams,
							   &cRes,
							   &cException,
							   &iErrArg);

	if(SUCCEEDED(hRes))
	{
		cRes.vt = VT_I4;
		lRtc = cRes.lVal;
	}
	else
	{
		CString sMsg;
		sMsg.Format("COLA32Srv::AddParams() - Invoke Failed - returned %ld", hRes);
		_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);
	}

	LeaveCriticalSection(&m_csOLASrv);

	return lRtc;
}

/////////////////////////////////////////////////////////
//Session control methods.
/////////////////////////////////////////////////////////
long COLA32Srv::StartSession(char * pInBuffer, char *pszType)
{
	long lSessionNumber = 0;
	unsigned int iErrArg = 0;
	CComVariant pArgs[3];
	VARIANT cRes;
	EXCEPINFO cException;

	HRESULT hRes = S_OK;

	CComBSTR sType(pszType);
	CComBSTR sInBuffer(pInBuffer);

	pArgs[0].vt = VT_BSTR;
	pArgs[0].bstrVal = sType;

	pArgs[1].vt = VT_BSTR;
	pArgs[1].bstrVal = sInBuffer;

	pArgs[2].vt = VT_I4;
	pArgs[2].lVal = 99L;

	DISPPARAMS cParams = { pArgs , NULL , 3 , 0 };

	EnterCriticalSection(&m_csOLASrv);
	hRes = m_pDispatch->Invoke(MethodIDs[1],
		                       IID_NULL,
						       LOCALE_SYSTEM_DEFAULT,
						       DISPATCH_METHOD,
						       &cParams,
						       &cRes,
						       &cException,
						       &iErrArg);

	if(SUCCEEDED(hRes))
	{
		cRes.vt = VT_I4;
		lSessionNumber = cRes.lVal;
	}
	else
	{
		Error();  //4.0.10.49
		lSessionNumber = OLASRV_COM_ERROR;

		CString sMsg;
		sMsg.Format("COLA32Srv::StartSession() - Invoke Failed - returned %ld", hRes);
		_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);
	}

	LeaveCriticalSection(&m_csOLASrv);

	return lSessionNumber;
}

long COLA32Srv::QuerySession(long lSessionNumber, BSTR * pOutBuffer)
{
	long lRetCode = 0;
	unsigned int iErrArg = 0;
	CComVariant pArgs[2];
	VARIANT cRes;
	EXCEPINFO cException;

	HRESULT hRes = S_OK;

	pArgs[0].vt = VT_BSTR | VT_BYREF;
	pArgs[0].pbstrVal = pOutBuffer;

	pArgs[1].vt = VT_I4;
	pArgs[1].lVal = lSessionNumber;

	DISPPARAMS cParams = { pArgs , NULL , 2 , 0 };

	EnterCriticalSection(&m_csOLASrv);
	hRes = m_pDispatch->Invoke(MethodIDs[3],
				               IID_NULL,
							   LOCALE_SYSTEM_DEFAULT,
							   DISPATCH_METHOD,
							   &cParams,
							   &cRes,
							   &cException,
							   &iErrArg);

	if(SUCCEEDED(hRes))
	{
		cRes.vt = VT_I4;
		lRetCode = cRes.lVal;
	}
	else
	{
		CString sMsg;
		sMsg.Format("COLA32Srv::QuerySession() - Invoke Failed - returned %ld", hRes);

		_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);

		//TD 464407
		Error();
		
		if (hRes == 0x800706BE)
		{
			_Module.m_server.SendEventKillOla();
		}

		// 		if (hRes == 0x8007007e)
		// 		{
		// 			lSessionNumber = OLASRV_COM_ERROR_OLA_EXCEPTION;
		// 		}
		// 		else
		lSessionNumber = OLASRV_COM_ERROR;
	}

	LeaveCriticalSection(&m_csOLASrv);

	return lRetCode;
}

long COLA32Srv::EndSession(long lSessionId)
{
	long lRetCode = 0;
	unsigned int iErrArg = 0;
	CComVariant pArgs[1];
	VARIANT cRes;
	EXCEPINFO cException;

	HRESULT hRes = S_OK;

	pArgs[0].vt = VT_I4;
	pArgs[0].lVal = lSessionId;

	DISPPARAMS cParams = { pArgs , NULL , 1 , 0 };

	EnterCriticalSection(&m_csOLASrv);
	hRes = m_pDispatch->Invoke(MethodIDs[2],
		                       IID_NULL,
							   LOCALE_SYSTEM_DEFAULT,
							   DISPATCH_METHOD,
						       &cParams,
						       &cRes,
						       &cException,
						       &iErrArg);

	if(SUCCEEDED(hRes))
	{
		cRes.vt = VT_I4;
		lRetCode = cRes.lVal;
	}
	else
	{
		CString sMsg;
		sMsg.Format("COLA32Srv::EndSession() - Invoke Failed - returned %ld", hRes);

		_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);

		Error();
		lRetCode = OLASRV_COM_ERROR;
	}

	LeaveCriticalSection(&m_csOLASrv);

	return lRetCode;
}

/////////////////////////////////////////////////////////
//OLA service control methods.
/////////////////////////////////////////////////////////
void COLA32Srv::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE */)
{
	EnterCriticalSection(&m_csOLASrv);

	if(m_bValid)
	{
		m_bValid = FALSE;//4.0.3.45
		try
		{
			m_pDispatch->Release();
			m_pUnknown->Release();
		}

		catch(_com_error &cError)
		{
			//4.0.1.32
			CatchCOMError(cError);
		}

		catch (...)
		{
			_Module.m_server.m_cLogger.LogMsg("COLA32Srv::ReleaseService() - the call to Release failed.", LOG_LEVEL_0);
		}
	
		if (bCloseThread)
			_Module.m_server.CloseOLAThread();  	
		else
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_WAIT_FOR_KILL;

		//m_bValid = FALSE;
	}
		
	LeaveCriticalSection(&m_csOLASrv);
}

