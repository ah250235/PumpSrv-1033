// OLATcpBase.cpp: implementation of the COLATcpBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLATcpBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLATcpBase::COLATcpBase(OLECHAR FAR * sMethodName) : m_pCommonDispatch(0), 
                                                      m_pCommonUnknown(0)												
{
	wcscpy(m_sMethodName , sMethodName);
}

COLATcpBase::~COLATcpBase()
{
	//ReleaseService(TRUE);//4.0.3.45
}


/******************************************************************************
 Description:	Initialize interface pointers 	
 Returns:      	TRUE on success, otherwise FALSE
 Parameters:   	
 Assumptions:   COM has been initialized already in WinMain
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			17/1/2002   19:37		Start
******************************************************************************/
BOOL COLATcpBase::InitInterface()
{
	BOOL bRetCode = TRUE;
	HRESULT hCommonRes , hControlRes;
	
	hCommonRes = CoCreateInstance(CLSID_CommonSrv, 
			                      NULL, 
       				              CLSCTX_ALL, 
					              IID_IUnknown, 
					              (void FAR* FAR*)&m_pCommonUnknown);

	hControlRes = m_pCtrlPtr.CreateInstance(CLSID_ControlSrv);

	if((hCommonRes != S_OK) || (hControlRes != S_OK))
	{
		CString sMsg;
		if((hCommonRes == REGDB_E_CLASSNOTREG) || (hControlRes == REGDB_E_CLASSNOTREG))
			sMsg.Format("COLATcpBase::InitInterface() : CommonSrv or ControlSrv not registered.");

		else if((hCommonRes == CLASS_E_NOAGGREGATION) || (hControlRes == REGDB_E_CLASSNOTREG))
			sMsg.Format("COLATcpBase::InitInterface() : class does not support aggregation.");

		else
			sMsg.Format("COLATcpBase::InitInterface() : an error has occurred.");

		_LOGMSG.LogMsg(sMsg , LOG_LEVEL_0);
		
		bRetCode = FALSE;
	}
	
	else
		hCommonRes = m_pCommonUnknown->QueryInterface(IID_IDispatch, 
		                                              (void**)&m_pCommonDispatch);

	if(hCommonRes != S_OK)
	{
		CString sMsg;
		sMsg.Format("COLATcpBase::InitInterface() : failed on QueryInterface(), interface pointers reset to NULL");
		_LOGMSG.LogMsg(sMsg , LOG_LEVEL_0);

		bRetCode = FALSE;

		//4.0.2.15
		m_pCommonUnknown = m_pCommonDispatch = NULL;
	}

	return bRetCode;
}

long COLATcpBase::AnalyzeInvokeResult(OLECHAR FAR * sMethodName , HRESULT hRes, EXCEPINFO &cException , unsigned int iErrArg)
{
	long lRetCode = TCPOLA_OK;
	char sName[64];
	WideCharToMultiByte(CP_ACP,0,sMethodName,-1,sName,sizeof(sName),NULL,NULL);
	CString sMsg , sExceptionContent;
	
	sMsg.Format("COLATcpBase::%s : " , sName);

	if(SUCCEEDED(hRes))
		return lRetCode;

	switch(hRes)
	{
	case DISP_E_BADPARAMCOUNT:
		lRetCode = BAD_PARAM_COUNT;
		sMsg += "Bad param count.";
		break;

	case DISP_E_BADVARTYPE:
		lRetCode = BAD_PARAM;
		sMsg += "Bad var type.";
		break;

	case DISP_E_EXCEPTION:
		char sSource[100] , sDescription[100];
		WideCharToMultiByte(CP_ACP,0,cException.bstrSource,-1,sSource,sizeof(sSource),NULL,NULL);
		WideCharToMultiByte(CP_ACP,0,cException.bstrDescription,-1,sDescription,sizeof(sDescription),NULL,NULL);
		sExceptionContent.Format("Invoke() has thrown an exception from %s - %s" , sSource , sDescription);
		sMsg += sExceptionContent;
		//Unfortunately, numeric error codes 
		//are not supported.
		if(!strcmp(sSource , "Communicator"))
			lRetCode = RETRY;

		else
			lRetCode = GENERAL_ERROR;
		break;

	case DISP_E_MEMBERNOTFOUND:
		lRetCode = BAD_MEMBER_NAME;
		sMsg += "Member does not exist.";
		break;

	case DISP_E_NONAMEDARGS:
		lRetCode = NAMED_ARGS_NOT_SUPPORTED;
		sMsg += "Named arguments not supported.";
		break;

	case DISP_E_OVERFLOW:
		lRetCode = DISP_OVERFLOW;
		sMsg += "Overflow";
		break;

	case DISP_E_PARAMNOTFOUND:
		lRetCode = PARAM_NOT_FOUND;
		sExceptionContent.Format("The parmater indexed %d was not found" , iErrArg);
		sMsg += sExceptionContent;
		break;

	case DISP_E_TYPEMISMATCH:
		lRetCode = TYPE_MISMATCH;
		sExceptionContent.Format("The parameter indexed %d does not match declared type" , iErrArg);
		sMsg += sExceptionContent;
		break;

	case DISP_E_UNKNOWNINTERFACE:
		lRetCode = BAD_PARAM;
		sMsg += "The interface identifier passed in riid is not IID_NULL.";
		break;

	case DISP_E_PARAMNOTOPTIONAL:
		lRetCode = BAD_PARAM;
		sMsg += "A required parameter was omitted.";
		break;

	default:
		lRetCode = GENERAL_ERROR;
		sMsg += "An error has occurred.";
		break;
	}

	_LOGMSG.LogMsg(sMsg , LOG_LEVEL_0);

	return lRetCode;
}

long COLATcpBase::Call(OLECHAR FAR *sMethodName, CComVariant *cArgBuffer, int iArgBufferLen , VARIANT &cRes, EXCEPINFO& cException)
{
	DISPID DispId = 0;
	long lRetCode = 0;
	unsigned int iErrArg = 0;

	//4.0.2.15 start added if...else statement
	if(m_pCommonUnknown && m_pCommonDispatch)
	{
		EnterCriticalSection(&m_csOLASrv);
		
		//Get the desired method's DISPID.
		HRESULT hRes = m_pCommonDispatch->GetIDsOfNames(IID_NULL,
														&sMethodName,
														1,
														LOCALE_USER_DEFAULT,
														&DispId);

		if(SUCCEEDED(hRes))
		{
			//Invoke the desired method.
			DISPPARAMS params = {cArgBuffer , NULL , iArgBufferLen , 0};
			hRes = m_pCommonDispatch->Invoke(DispId,
											 IID_NULL,
											 LOCALE_SYSTEM_DEFAULT,
											 DISPATCH_METHOD,
											 &params,
											 &cRes,
											 &cException,
											 &iErrArg);

			lRetCode = AnalyzeInvokeResult(sMethodName , hRes , cException , iErrArg);
		}

		else
			lRetCode = RETRY;

		LeaveCriticalSection(&m_csOLASrv);
	}

	else
		lRetCode = RETRY;
	//4.0.2.15
	
	return lRetCode;
}

void COLATcpBase::SwitchServer()
{
	if(m_pCtrlPtr == NULL)
	{
		_LOGMSG.LogMsg("COLATcpBase::SwitchServer - Severe error, TCPCOMSrv may not be running on target machine" , LOG_LEVEL_0);
		return;
	}

	long lNumOptions = 0;
	_bstr_t sProgId("PSRemote_OLA");
	
	HRESULT hRes = 0L;

	//4.0.1.32
	BOOL bFoundNewServerAddress = FALSE , bForcedExit = FALSE;
	long lIndex = 0 , lNumberOfLoops = 0;
	_bstr_t sNewHost;

	EnterCriticalSection(&m_csOLASrv);

	lNumOptions = m_pCtrlPtr->GetHostCountForProg(sProgId);
	_bstr_t sOldHost = m_pCtrlPtr->GetHostForProg(sProgId);

	if(lNumOptions)
	{
		lIndex = m_pCtrlPtr->GetHostIndexForProg(sProgId);

		while(!bFoundNewServerAddress)
		{
			++lIndex;
			lIndex %= lNumOptions;

			//To ensure that no infinite loops are caused by
			//bad configuration, we limit the number of iterations
			++lNumberOfLoops;
			if(lNumberOfLoops > lNumOptions)
			{
				bForcedExit = TRUE;
				break;
			}

			sNewHost = m_pCtrlPtr->GetHostForProgAndIndex(sProgId , lIndex);
	
			if(sNewHost.length())
			{
				hRes = m_pCtrlPtr->SetHostForProg(sProgId , sNewHost);

				if(!hRes)
				{
					hRes = m_pCtrlPtr->SetHostIndexForProg(sProgId , lIndex);
					
					//Exit loop after new OLA server has been reset.
					bFoundNewServerAddress = TRUE;

					//Write to log in case of successful switch
					sNewHost = m_pCtrlPtr->GetHostForProg(sProgId);
					char sName[32];
					WideCharToMultiByte(CP_ACP,0,sNewHost,-1,sName,sizeof(sName),NULL,NULL);
					CString sMsg;
					sMsg.Format("COLATcpBase::SwitchServer() : new host is %s" , sName);
					_LOGMSG.LogMsg(sMsg);

					//4.0.2.161
					ResetConnectionAttempts();
				}
				
				else
					_LOGMSG.LogMsg("COLATcpBase::SwitchServer() - switch server incomplete" , LOG_LEVEL_0);

			}
		
			else
				_LOGMSG.LogMsg("COLATcpBase::SwitchServer : optional server not configured, searching for more servers." , LOG_LEVEL_0);
		}
	}
	
	else
		_LOGMSG.LogMsg("COLATcpBase::SwitchServer : no optional server found." , LOG_LEVEL_0);

	if(bForcedExit)
		_LOGMSG.LogMsg("COLATcpBase::SwitchServer : could not find any properly configured optional servers!" , LOG_LEVEL_0);
	
	LeaveCriticalSection(&m_csOLASrv);

	return;
}

//Virtual function implementations.

//Section I : methods that invoke methods from the OLA server interface.
//All these functions are built in a similar fashion : the parameters
//are packed into a variant and sent to Call().
////////////////////////////////////////////////////////////////////////////////////
//Start, end and query an OLA session.
////////////////////////////////////////////////////////////////////////////////////

long COLATcpBase::StartSession(char * pInBuffer, char *pszType)
{
	int iLen = 3;
	CComVariant * cArgs = new CComVariant[iLen];
	VARIANT cRes;
	EXCEPINFO cException;
	OLECHAR sMethodName[32];
	wcscpy(sMethodName , m_sMethodName);
	wcscat(sMethodName , L"__StartSession");

	CComBSTR sType(pszType);
	CComBSTR sInBuffer(pInBuffer);

	cArgs[0].vt = VT_BSTR;
	cArgs[0].bstrVal = sType;

	cArgs[1].vt = VT_BSTR;
	cArgs[1].bstrVal = sInBuffer;

	cArgs[2].vt = VT_I4;
	cArgs[2].lVal = 99L;

	long lRetCode = Call(sMethodName , cArgs , iLen , cRes , cException);

	if(lRetCode == RETRY)
		lRetCode = OLASRV_COM_ERROR;

	else
	{
		cRes.vt = VT_I4;
		lRetCode = cRes.lVal;
	}

	//wcscpy(m_sMethodName , L"OLASRV");
	
	//4.0.2.15 CComVariant array was not deleted!
	delete[] cArgs;

	return lRetCode;
}


long COLATcpBase::EndSession(long lSessId)
{
	int iLen = 1;
	CComVariant * cArgs = new CComVariant;
	VARIANT cRes;
	EXCEPINFO cException;
	OLECHAR sMethodName[32];
	wcscpy(sMethodName , m_sMethodName);
	wcscat(sMethodName , L"__EndSession");

	cArgs[0].vt = VT_I4;
	cArgs[0].lVal = lSessId;

	long lRetCode = Call(sMethodName , cArgs , iLen , cRes , cException);

	if(lRetCode == RETRY)
		lRetCode = OLASRV_COM_ERROR;

	else
	{
		cRes.vt = VT_I4;
		lRetCode = cRes.lVal;
	}

	//wcscpy(m_sMethodName , L"OLASRV");

	//4.0.2.15 CComVariant array was not deleted!
	delete cArgs;

	return lRetCode;
}


long COLATcpBase::QuerySession(long lSessionNumber, BSTR *pOutBuffer)
{
	int iLen = 2;
	CComVariant * cArgs = new CComVariant[iLen];
	VARIANT cRes;
	EXCEPINFO cException;
	OLECHAR sMethodName[32];
	wcscpy(sMethodName , m_sMethodName);
	wcscat(sMethodName , L"__QuerySession");

	cArgs[0].vt = VT_BSTR | VT_BYREF;
	cArgs[0].pbstrVal = pOutBuffer;

	cArgs[1].vt = VT_I4;
	cArgs[1].lVal = lSessionNumber;

	long lRetCode = Call(sMethodName , cArgs , iLen , cRes , cException);

	if(lRetCode == RETRY)
		lRetCode = OLASRV_COM_ERROR;

	else
	{
		cRes.vt = VT_I4;
		lRetCode = cRes.lVal;
	}

	//wcscpy(m_sMethodName , L"OLASRV");
	
	//4.0.2.15 CComVariant array was not deleted!
	delete[] cArgs;

	return lRetCode;
}

////////////////////////////////////////////////////////////////////////////////////
//Card data retrieval and manipulation methods.
////////////////////////////////////////////////////////////////////////////////////

long COLATcpBase::GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags ,long *lSesionType, long lPumpNumber, PAY_AT_PUMP_INFO * pInPAPBuffer)	//4.0.20.52 63936
{
	int iLen = 5;
	CComVariant * cArgs = new CComVariant[iLen];
	VARIANT cRes;
	EXCEPINFO cException;
	OLECHAR sMethodName[32];
	CComBSTR      bstrTmp(""); //4.0.3.21
	wcscpy(sMethodName , m_sMethodName);
	wcscat(sMethodName , L"__GetCArdInfo");

	//4.0.3.21
/*
	cArgs[0].vt = VT_BSTR | VT_BYREF;
	cArgs[0].pbstrVal = pRetData;

	cArgs[1].vt = VT_BSTR;
	cArgs[1].bstrVal = sShortCardName;

	cArgs[2].vt = VT_BSTR;
	cArgs[2].bstrVal = sTrack2;

	cArgs[3].vt = VT_BSTR;
	cArgs[3].bstrVal = sTrack1;

	cArgs[4].vt = VT_BSTR;
	cArgs[4].bstrVal = sAccountNumber;
*/

//4.0.3.21

	cArgs[0].vt = VT_BSTR;
	cArgs[0].bstrVal = sShortCardName;

	cArgs[1].vt = VT_BSTR;
	cArgs[1].bstrVal =sTrack2 ;

	cArgs[2].vt = VT_BSTR;
	cArgs[2].bstrVal = sTrack1;

	cArgs[3].vt = VT_BSTR;
	cArgs[3].bstrVal =sAccountNumber;

	cArgs[4].vt = VT_BSTR;
	cArgs[4].bstrVal =bstrTmp ;


	long lRetCode = Call(sMethodName , cArgs , iLen , cRes , cException);

	if(lRetCode == RETRY)
		lRetCode = OLASRV_COM_ERROR;

	else
	{
		//4.0.3.21
		//cRes.vt = VT_I4;  
		cRes.vt = VT_BSTR;
		*pRetData = cRes.bstrVal;
		
		//4.0.3.21		
		//lRetCode = cRes.lVal;
	}
	
	//4.0.2.15 CComVariant array was not deleted!
	delete[] cArgs;

	return lRetCode;  
}


long COLATcpBase::AddParams(long lSessId, char *pInBuffer)
{
	int iLen = 2;
	CComVariant * cArgs = new CComVariant[iLen];
	VARIANT cRes;
	EXCEPINFO cException;
	OLECHAR sMethodName[32];
	wcscpy(sMethodName , m_sMethodName);
	wcscat(sMethodName , L"__AddParams");

	CComBSTR sInBuffer(pInBuffer);
	
	cArgs[0].vt = VT_BSTR;
	cArgs[0].bstrVal = sInBuffer;

	cArgs[1].vt = VT_I4;
	cArgs[1].lVal = lSessId;

	long lRetCode = Call(sMethodName , cArgs , iLen , cRes , cException);

	if(lRetCode == RETRY)
		lRetCode = OLASRV_COM_ERROR;

	else
	{
		cRes.vt = VT_I4;
		lRetCode = cRes.lVal;
	}
	
	//4.0.2.15 CComVariant array was not deleted!
	delete[] cArgs;
	
	return lRetCode;  
}

//Section II : methods that do not directly invoke methods in the OLA
//server's interface.
////////////////////////////////////////////////////////////////////////////////////
//Initialize, create and destroy TcpOla service.
////////////////////////////////////////////////////////////////////////////////////
void COLATcpBase::Init()
{
	BOOL bConnectSucceeded = FALSE;

	if(!m_bOLAStopped)
	{
		EnterCriticalSection(&m_csOLASrv);

		//4.0.2.37
		if(m_byConnectionAttempts)
		{
			if(!m_pCommonUnknown || !m_pCommonDispatch)
			{
				try
				{
					bConnectSucceeded = InitInterface();
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

			//4.0.2.15 added return code check for InitInterface
			if(bConnectSucceeded)
			{
				m_bValid = TRUE;

				DWORD dwTmp;
				m_nDeclineCounter = 0;  //2.9.3.4
				_Module.m_server.m_cParam.LoadParam((char *)"GeneralParam", "OlaMaxDeclineMessToDisplay",&dwTmp,100,TRUE);
				m_lOlaMaxDeclineMessToDisplay  = (long)dwTmp;

				//Create OLA thread and wait to restart.
				_Module.m_server.CheckOLA();	

				//Restart thread.
				_Module.m_server.CheckOLA();

				//4.0.2.37
				ResetConnectionAttempts();

				if (_Module.m_server.m_lSysError == SYS_ERROR_OLA)
				{
					long lRtc = 0;
					//4.0.2.38
					if (_Module.m_server.GetAlarmSrvMode() == ALRMSRV_DIRECT_ACTIVE)
					{
						// Clear OLA error
						if (_Module.m_server.m_cAlarm.m_lAlarmSpecialGroupMap[ALARM_GROUP_OLA])
							lRtc = _Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, SOFTWARE_GENERAL_OLA_ERR,0, 0,0,0);
					}
					else  //4.0.2.38
						_Module.m_server.WriteToLogMsg(0, TRUE, 0, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, &lRtc);
				}
			}

			else
			{
				m_bValid = FALSE;
				_LOGMSG.LogMsg("COLATCPBase::Init() - did not start OLA thread, service not valid." , LOG_LEVEL_0);

				//4.0.2.37
				DecrementConnectionAttempts();
			}
		}

		LeaveCriticalSection(&m_csOLASrv);
	}
	
	return;
}


void COLATcpBase::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE */)
{
	EnterCriticalSection(&m_csOLASrv);
		
	if(m_bValid)
	{
		m_bValid = FALSE;//4.0.3.45
		try
		{
				m_pCommonDispatch->Release();
				m_pCommonUnknown->Release();

				m_pCtrlPtr.Release();
				m_pCtrlPtr.Detach();

				m_pCommonDispatch = NULL;
				m_pCommonUnknown = NULL;

				m_pCtrlPtr = NULL;
		}

		catch(_com_error &cError)
		{
			//4.0.1.32
			CatchCOMError(cError);
		}

		catch (...)
		{
			CString sMsg("TcpOla::ReleaseService() : the call to Release failed.");
			_LOGMSG.LogMsg(sMsg , LOG_LEVEL_0);
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