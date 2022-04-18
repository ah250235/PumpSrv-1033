// OlaR10.cpp: implementation of the COlaR10 class. R10 User Story - 71146 - add new class
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLAEps.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/******************************************************************************
 Description:	This method creates R10 EPS provider's instance.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 
 AmitH          30/06/2013   13:00      TD - R10
******************************************************************************/
void COLAEps::Init() 
{
	HRESULT hRes= S_OK;
	CLSID clsid; 
		

	if ( m_bOLAStopped == FALSE )
	{
		EnterCriticalSection(&m_csOLASrv);

		try
		{			
			if (_Module.m_server.GetOLAThreadState() == OLA_SERVER_OFF)
			{
					
				if (m_byConnectionAttempts > 0)			// to print inly relevant Log lines
				{
					CString sLogMsg;
					sLogMsg.Format("COLAEps::Init(). OLAThreadState: OLA_SERVER_OFF. OlaValid: %ld, ConnectionAttempts: %ld " , m_bValid, m_byConnectionAttempts);
					_LOGMSG.LogMsg(sLogMsg);

				}


				if (!m_bValid && m_byConnectionAttempts)
				{
				
					//Get the CLSID for the interface.
					hRes = CLSIDFromProgID(L"Retalix.Client.Forecourt.OlaAdapter.OlaComAdapter" , &clsid);

					//Create the COM object        
					hRes = CoCreateInstance(clsid , 0, CLSCTX_ALL, __uuidof(IOlaComAdapter), (void**)&m_pSrvPtr );


					if(FAILED(hRes))
					{
						_LOGMSG.LogMsg("OLA CreateInstance Failed. " ,LOG_LEVEL_1);
						_com_issue_error(hRes);

					}

					else
					{	
						_LOGMSG.LogMsg("Call LaunchOLA(). " );
						LoadRegistryParams();

						long lPumpNumber=0;
						m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber); 
						
						LaunchOLA();

						m_byInvokeOlaAppCounter = 0;
						ResetConnectionAttempts();
					}
				}    
			}
		}

		catch (_com_error& e)
		{
			_LOGMSG.LogMsg("OLA CreateInstance Failed - COM exception. " ,LOG_LEVEL_1);
			Error(e,hRes,FALSE);
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF;

			Sleep(OLA_RECOVERY_SLEEP_TIME);				
			DecrementConnectionAttempts();
		}

		catch (...)
		{
			_LOGMSG.LogMsg("OLA CreateInstance Failed - catch (...). " ,LOG_LEVEL_1);
			Error();
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF;

			Sleep(OLA_RECOVERY_SLEEP_TIME);			
			DecrementConnectionAttempts();
		}


		if ((0 == m_byConnectionAttempts) && (m_byInvokeOlaAppCounter < MAX_OLA_RETRIES))
		{
			CString strLog;

			m_byInvokeOlaAppCounter++;
			strLog.Format("COLAEps::Init() Trying to invoke Ola application. Attempt [%d] from 10.", m_byInvokeOlaAppCounter);
			_LOGMSG.LogMsg(strLog);

			// try to reload 
			InvokeOlaEpsApplication();   

			ResetConnectionAttempts();
			ResetRetryAttemps();
			Sleep(OLA_RECOVERY_SLEEP_TIME);	
		}

		LeaveCriticalSection(&m_csOLASrv);
		
//		_LOGMSG.LogMsg("COLAEps::Init() after critical section." );
	}
}

/******************************************************************************
 Description:	This method calls releases the R10 EPS provider's instance.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 
 AmitH          30/06/2013   13:00      TD - R10
******************************************************************************/
void COLAEps::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /* = FALSE */)
{
	
	EnterCriticalSection(&m_csOLASrv);
	
	if(m_bValid)
	{
		m_bValid = FALSE;	//4.0.3.45
		try
		{
			if (m_pSrvPtr != NULL)
			{
				m_pSrvPtr.Release();
				m_pSrvPtr.Detach();
				m_pSrvPtr = NULL;
				_LOGMSG.LogMsg("COLAEps terminated Successfully");
			}
		}
		catch (...) 
		{
			CString str;
			str.Format("Failed to release COLAEps"); 
			_LOGMSG.LogMsg("COLAEps","Error",0,LOG_NONE,str,LOG_LEVEL_1);
		}				
		if (bCloseThread)
			_Module.m_server.CloseOLAThread();  	
// 		else
// 			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_WAIT_FOR_KILL;

	}
	
	LeaveCriticalSection(&m_csOLASrv);
}


/******************************************************************************
 Description:	This method calls R10 EPS provider's QuerySession.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 
 AmitH          30/06/2013   13:00      TD - R10
******************************************************************************/
long COLAEps::QuerySession(long lSessionNumber, BSTR * pOutBuffer)
{
	HRESULT hRes = S_OK;
	long lResult = 0;
	CString sLogMsg;
	
	EnterCriticalSection(&m_csOLASrv);

	 _bstr_t sTransactionType(m_sOlaReqType);

	 sLogMsg.Format("COLAEps QuerySession %ld Session Type %.20s " , lSessionNumber ,m_sOlaReqType);
	_LOGMSG.LogMsg(sLogMsg);

	try
	{							
		hRes = m_pSrvPtr->QuerySession(lSessionNumber, sTransactionType, pOutBuffer, &lResult);

		if ( FAILED(hRes) )		
		{
			_LOGMSG.LogMsg("OLA QuerySession Failed. " ,LOG_LEVEL_1);
			_com_issue_error(hRes);
		}
	}
	
	catch (_com_error& e)
	{
		lResult = OLASRV_COM_ERROR;
		Error(e, hRes);
	}
	
	catch(...)
	{
		lResult = OLASRV_COM_ERROR;
		Error();
	}

	sLogMsg.Format("QuerySession %ld after Rtc=%ld", lSessionNumber, lResult);
	_LOGMSG.LogMsg(sLogMsg);

	LeaveCriticalSection(&m_csOLASrv);

	return lResult;
}

/******************************************************************************
 Description:	This method calls R10 EPS provider's StartSession.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 
 AmitH          30/06/2013   13:00      TD - R10
******************************************************************************/
long COLAEps::StartSession(char * pInBuffer, char *pszType)
{
	HRESULT hRes = S_OK; 	
    _bstr_t sTransactionType(pszType);
	_bstr_t sInBuffer(pInBuffer);
	long	lSessionNumber;
	CString	sLogMsg;		
	long lResult = 0;
	
	EnterCriticalSection(&m_csOLASrv);
	
	sLogMsg.Format("StartSession before " );		
	_LOGMSG.LogMsg(sLogMsg);	

	try
	{					
		hRes = m_pSrvPtr->StartSession(SERVER_OLA_POS_NUMBER, sInBuffer, sTransactionType, &lSessionNumber, &lResult);	

		if ( FAILED(hRes) )		
		{
			_LOGMSG.LogMsg("OLA StartSession Failed. " ,LOG_LEVEL_1);
			_com_issue_error(hRes);
		}
	} 
	
	catch (_com_error & e)
	{
		Error(e, hRes);
		lSessionNumber = OLASRV_COM_ERROR;
	}
	
	catch (...)
	{
		Error();
		lSessionNumber = OLASRV_COM_ERROR;
	}

	sLogMsg.Format("StartSession %ld after ", lSessionNumber);		
	_LOGMSG.LogMsg(sLogMsg);	

	LeaveCriticalSection(&m_csOLASrv);

	return lSessionNumber;
}

/******************************************************************************
 Description:	This method calls R10 EPS provider's AddParams.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 
 AmitH          30/06/2013   13:00      TD - R10
******************************************************************************/
long COLAEps::AddParams(long lSessionNumber, char * pInBuffer)
{
	HRESULT hRes = 0;
	CString	sLogMsg;
	long lResult = 0;

	EnterCriticalSection(&m_csOLASrv);

	_bstr_t sInBuffer(pInBuffer);
	_bstr_t sTransactionType(m_sOlaReqType);

	sLogMsg.Format("AddParams Session %ld , Session Type %.20s " , lSessionNumber ,m_sOlaReqType);
	_LOGMSG.LogMsg(sLogMsg);



	try
	{					
		hRes =	m_pSrvPtr->AddParams(lSessionNumber, sTransactionType, sInBuffer, &lResult);
	}

	catch (_com_error & e)
	{
		Error(e, hRes);
		hRes = OLASRV_COM_ERROR;
	}
	
	catch(...)
	{
		Error();
		hRes = OLASRV_COM_ERROR;
	}

	sLogMsg.Format("AddParams Session %ld after", lSessionNumber );	
	_LOGMSG.LogMsg(sLogMsg);		
	
	LeaveCriticalSection(&m_csOLASrv);

	return hRes;
}

/******************************************************************************
 Description:	This method calls R10 EPS provider's EndSession.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 
 AmitH          30/06/2013   13:00      TD - R10
******************************************************************************/
long COLAEps::EndSession(long lSessionNumber)
{
	HRESULT hRes = 0;
	long lResult = 0;
	CString sLogMsg;
	
	EnterCriticalSection(&m_csOLASrv);

	_bstr_t sTransactionType(m_sOlaReqType);

	sLogMsg.Format("COLAEps EndSession %ld, Session Type %.20s " , lSessionNumber ,m_sOlaReqType);
	_LOGMSG.LogMsg(sLogMsg);


	try
	{							
		hRes = m_pSrvPtr->EndSession (lSessionNumber, sTransactionType, &lResult);

		if ( FAILED(hRes) )		
		{
			_LOGMSG.LogMsg("OLA EndSession Failed. " ,LOG_LEVEL_1);
			_com_issue_error(hRes);
		}
	}
	
	catch (_com_error& e)
	{
		hRes = OLASRV_COM_ERROR;
		Error(e, hRes);
	}
	
	catch(...)
	{
		hRes = OLASRV_COM_ERROR;
		Error();
	}

	sLogMsg.Format("EndSession %ld after error=%ld " , lSessionNumber , hRes );
	_LOGMSG.LogMsg(sLogMsg);

	LeaveCriticalSection(&m_csOLASrv);

	return hRes;
}

/******************************************************************************
 Description:	This method calls R10 EPS provider's GetCardInfo.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 
 AmitH          30/06/2013   13:00      TD - R10
******************************************************************************/
long COLAEps::GetCardInfoEx( long lClientId,BSTR sData, BSTR *pRetData,long lFlags /* = 0 */)
{
    HRESULT hRes = 0;
	long lResult = 0;
	CString sLogMsg;

	EnterCriticalSection(&m_csOLASrv);
 
	try
	{	
		sLogMsg.Format("GetCardInfoEx before " );
		_LOGMSG.LogMsg(sLogMsg);

		hRes = m_pSrvPtr->GetCardInfo(sData, pRetData, &lResult); 

		if ( FAILED(hRes) )		
		{
			_LOGMSG.LogMsg("OLA GetCardInfoEx Failed. " ,LOG_LEVEL_1);
			_com_issue_error(hRes);
		}

	}

	catch (_com_error & e)
	{
		Error(e);		
		hRes = OLASRV_COM_ERROR;
		_Module.m_server.CloseOLAThread();
	}
	 
	catch(...)
	{
		Error();		
		hRes = OLASRV_COM_ERROR;
		_Module.m_server.CloseOLAThread();
	}

	sLogMsg.Format("GetCardInfoEx after, Rtc=%ld",hRes ); 
	_LOGMSG.LogMsg(sLogMsg);
	
	LeaveCriticalSection(&m_csOLASrv);
	
	return hRes;

}

/******************************************************************************
 Description:	This method calls R10 EPS provider's QuerySession.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 
 AmitH          30/06/2013   13:00      TD - R10
******************************************************************************/
long COLAEps::GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR* pRetData, long lFlags, long *lSesionType, long lPumpNumber, PAY_AT_PUMP_INFO* pInPAPBuffer)
{
	GCIRetCode	retCode = GCI_OK;	
	HRESULT hRes = 0;
	long lResult = 0;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;	
	else if(FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GCI_FAILED_IO;
	else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
		retCode = GCI_SERVER_IN_MIDDLE_LOCK_SHIFT;

	try
	{
		if ( retCode == GCI_OK )
		{
			ResetConnectionAttempts();
			ResetRetryAttemps(TRUE);

			//prepare to BuildRequest
			PAY_AT_PUMP_INFO	cPayAtPumpInfo;			

			memset(&cPayAtPumpInfo, ' ', sizeof(cPayAtPumpInfo));
			l2a(lPumpNumber, cPayAtPumpInfo.CardSaleInfo.cardData.sPumpNumber, sizeof(cPayAtPumpInfo.CardSaleInfo.cardData.sPumpNumber));
			
			if (sTrack1)
				WideCharToMultiByte(CP_ACP, 0, sTrack1, -1, (char*)cPayAtPumpInfo.CardSaleInfo.cardData.sTrack1Data, sizeof(cPayAtPumpInfo.CardSaleInfo.cardData.sTrack1Data), NULL, NULL);

			if (sTrack2)
				WideCharToMultiByte(CP_ACP, 0, sTrack2, -1, (char*)cPayAtPumpInfo.CardSaleInfo.cardData.sTrack2Data, sizeof(cPayAtPumpInfo.CardSaleInfo.cardData.sTrack2Data), NULL, NULL);
			
			if (sAccountNumber)
				WideCharToMultiByte(CP_ACP, 0, sAccountNumber, -1, (char*)cPayAtPumpInfo.CardSaleInfo.cardData.sAccountNumber, sizeof(cPayAtPumpInfo.CardSaleInfo.cardData.sAccountNumber), NULL, NULL);


			//special treatment for Egift - its a payment card with barcode only:
			if (lFlags & RECEIPT_TRACK2_IS_BARCODE) //4.0.27.43
			{
				cPayAtPumpInfo.CardSaleInfo.cardData.sEntryMethod = 'B';
				if (sTrack2)
				{
					//track2 argument containes the barcode, copy it for ola.
					memcpy(cPayAtPumpInfo.CardSaleExtraData2.sBarCode,cPayAtPumpInfo.CardSaleInfo.cardData.sTrack2Data,sizeof(cPayAtPumpInfo.CardSaleInfo.cardData.sTrack2Data));
				}
			}

			char sOutBuffer[3048]; 
			memset((char*)sOutBuffer , 0 , sizeof(sOutBuffer));

			BuildRequest(SESSION_AUTHORIZE, lPumpNumber, (char*)&cPayAtPumpInfo, (char*)sOutBuffer, NULL, TRUE);
		
			//Write to Log before GetCardInfo		
			_LOGMSG.LogMsg(">>> COLAEps","GetCardInfo", lPumpNumber, LOG_PUMP, "GetCardInfo Before");

			//prepare to GetCardInfo
			CComBSTR	sInBuff = sOutBuffer;		
			CComBSTR	sRetBuf;

			hRes = m_pSrvPtr->GetCardInfo(sInBuff.m_str, &sRetBuf.m_str, &lResult);	

			if ( FAILED(hRes) )		
			{
				_LOGMSG.LogMsg("OLA GetCardInfo Failed. " ,LOG_LEVEL_1);
				_com_issue_error(hRes);
		}
			
			//Copy to return data
			if (!hRes)
			{
				*pRetData = sRetBuf.Copy();
			}
			
			CString strMsg;
			strMsg.Format("GetCardInfo After, Rtc=%ld",hRes );
			_LOGMSG.LogMsg(">>> COLAEps","GetCardInfo", lPumpNumber, LOG_PUMP, strMsg);
		}
	}
	catch (_com_error & e)
	{
		Error(e, hRes);		
		hRes = OLASRV_COM_ERROR;
		_Module.m_server.CloseOLAThread();
	}
	
	catch(...)
	{
		Error();		
		hRes = OLASRV_COM_ERROR;
		_Module.m_server.CloseOLAThread();
	}

	return hRes;
}


void COLAEps::InvokeOlaEpsApplication()
{

	char	szOLAFullFileName[MAX_FILE_PATH];
	CString strLog;

	long lRetVal = RetCode_Failure;
	
	memset(szOLAFullFileName,	0, sizeof(szOLAFullFileName));	


	if (RUNNING != IsOlaAlreadyRuning())
	{

		_Module.m_server.m_cParam.LoadParam("Paths\\Long", "OlaAppFullFileName", szOLAFullFileName, sizeof(szOLAFullFileName), "C:\\Retalix\\StoreServices\\ForecourtClient\\Retalix.Client.Forecourt.Shell.exe", TRUE, FALSE);		

		strLog.Format("Ola Eps try to invoke application. AppName: [%s].", szOLAFullFileName);
		_LOGMSG.LogMsg(strLog);
		
		if (strlen(szOLAFullFileName) > 0 )
		{
			if (TRUE == _Module.m_server.m_cCommonFunction.IsFileExists(szOLAFullFileName))
			{
				
				LPTSTR szCreateProcessCmd;
				CString sCreateProcessCmd;
				
				sCreateProcessCmd.Format("%s", szOLAFullFileName);
				szCreateProcessCmd = sCreateProcessCmd.GetBuffer(0);
				
				
				STARTUPINFO SI={0};
				PROCESS_INFORMATION PI={0};
				
				try
				{
					if (TRUE == CreateProcess(0,szCreateProcessCmd,0,NULL,false,CREATE_DEFAULT_ERROR_MODE,NULL,0,&SI,&PI))
					{
						strLog.Format("Ola Eps application successfully invoked. AppName: [%s].", szOLAFullFileName);
						_LOGMSG.LogMsg(strLog);
					}
					else
					{
						DWORD dwLastError =GetLastError();
						strLog.Format("Ola Eps application FAILED to invoke.lastError=%ld AppName: [%s].", dwLastError, szOLAFullFileName);
						_LOGMSG.LogMsg(strLog, LOG_LEVEL_1);
					}
					
					
				}
				catch (...)
				{
					_LOGMSG.LogMsg("OLA InvokeOlaEpsApplication Failed - catch (...). " ,LOG_LEVEL_1);
				}
				
			}
			
		}
	}

}


long COLAEps::IsOlaAlreadyRuning()
{
	
	long lRtc = NOT_RUNNING;
	

	lRtc = _Module.m_server.m_cCompChecker.IsRunning("RetalixClientUniqueAppForecourt-Client");  
	
	if(lRtc  == RUNNING)
		_LOGMSG.LogMsg("Retalix.Client.Forecourt.Shell (OlaEps) is running.");
	else
		_LOGMSG.LogMsg("Retalix.Client.Forecourt.Shell (OlaEps) is not running.");
	
	
	return lRtc;
}
