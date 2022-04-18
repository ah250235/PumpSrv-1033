
#include "StdAfx.h"
#include "OLAProgressSrvEx.h"

//4.0.12.505

void COLAProgressSrvEx::Init()
{
	long lRtc=0;
	HRESULT hRes;
	
	_LOGMSG.LogMsg("COLAProgressSrvEx::Init() ");

	if ( m_bOLAStopped == FALSE )
	{
		_LOGMSG.LogMsg("COLAProgressSrvEx::Init() before critical section." );

		EnterCriticalSection(&m_csOLASrv);

		try
		{			
			if (_Module.m_server.GetOLAThreadState() == OLA_SERVER_OFF)
			{
				_LOGMSG.LogMsg("COLAProgressSrvEx::Init() OLA_SERVER_OFF." );

				if (!m_bValid && m_byConnectionAttempts)
				{
					_LOGMSG.LogMsg("Call OLA CreateInstance before. ");
					hRes = m_pSrvPtr.CreateInstance(CLSID_OLAService);	
					_LOGMSG.LogMsg("Call OLA CreateInstance after. ");


					if(FAILED(hRes))
					{
						_LOGMSG.LogMsg("OLA CreateInstance Failed. " ,LOG_LEVEL_1);
						_com_issue_error(hRes);
						_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF;
					}

					else
					{	
						_LOGMSG.LogMsg("Call LaunchOLA(). " );
						LoadRegistryParams();

						_Module.m_server.m_cProtectedDataHandler.SetRemoveProhibitedData(TRUE);	//4.0.22.501 TD 72716

						long lPumpNumber=0;
						m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber);  //4.0.27.32
						
						LaunchOLA();

						//4.0.2.37
						ResetConnectionAttempts();
					}
				}    
			}
		}

		catch (_com_error& e)
		{
			_LOGMSG.LogMsg("OLA CreateInstance Failed - COM exception. " ,LOG_LEVEL_1);
			Error(e,hRes,FALSE);
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF; //ola12345

			//4.0.2.37
			DecrementConnectionAttempts();
		}

		//4.0.2.37
		catch (...)
		{
			_LOGMSG.LogMsg("OLA CreateInstance Failed - catch (...). " ,LOG_LEVEL_1);
			Error();
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF; //ola12345

			//4.0.2.37
			DecrementConnectionAttempts();
		}

		LeaveCriticalSection(&m_csOLASrv);
		
		_LOGMSG.LogMsg("COLAProgressSrvEx::Init() after critical section." );
	}


}




long COLAProgressSrvEx::GetCardInfoEx( long lClientId,BSTR sData, BSTR *pRetData,long lFlags /* = 0 */)
{
	long lRtc = 0;
	CString sLogMsg;

	EnterCriticalSection(&m_csOLASrv);
 
	try
	{	
		
		sLogMsg.Format("GetCardInfoEx before " );
		_LOGMSG.LogMsg(sLogMsg);

		lRtc = m_pSrvPtr->GetCardInfo(sData,pRetData); //4.0.14.503
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

	sLogMsg.Format("GetCardInfoEx after, Rtc=%ld",lRtc ); 
	_LOGMSG.LogMsg(sLogMsg);
	
	LeaveCriticalSection(&m_csOLASrv);
	
	return lRtc;

}


void COLAProgressSrvEx::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /* = FALSE */)
{
	
	long lRtc=0;
	CString	sLogMsg;
	
	if(m_pSrvPtr)
	{
		EnterCriticalSection(&m_csOLASrv);
		sLogMsg.Format("ReleaseService before " );		//4.0.20.56
		_LOGMSG.LogMsg(sLogMsg);		//4.0.20.56
  
		if(m_bValid)
		{
			m_bValid = FALSE;//4.0.3.45
			try
			{
				_Module.m_server.m_cLogger.LogMsg("OLAProgressSrvEx::ReleaseService() - call Release method.");
				m_pSrvPtr.Release(); //4.0.14.430
				if( bIsShutDown == FALSE )
					Sleep(2000);

			//	m_pSrvPtr.Detach();
			}

			catch (...)
			{
				_Module.m_server.m_cLogger.LogMsg("OLAProgressSrvEx::ReleaseService() - call Release failed." , LOG_LEVEL_1);
			}

			if (bCloseThread)
				_Module.m_server.CloseOLAThread();  	
			
			else
				_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_WAIT_FOR_KILL;
		}
	
	sLogMsg.Format("ReleaseService after" );		//4.0.20.56
	_LOGMSG.LogMsg(sLogMsg);		//4.0.20.56

	LeaveCriticalSection(&m_csOLASrv);

	}
}

long COLAProgressSrvEx::QuerySession(long lSessionNumber, BSTR * pOutBuffer)
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


long COLAProgressSrvEx::StartSession(char * pInBuffer, char *pszType)
{
	_bstr_t sTransactionType(pszType);
	_bstr_t sInBuffer(pInBuffer);
	long	lSessionNumber;
	CString	sLogMsg;
	
	EnterCriticalSection(&m_csOLASrv);
	
	sLogMsg.Format("StartSession before " );		//4.0.20.56
	_LOGMSG.LogMsg(sLogMsg);		//4.0.20.56

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

	sLogMsg.Format("StartSession %ld after ", lSessionNumber);		//4.0.20.56
	_LOGMSG.LogMsg(sLogMsg);		//4.0.20.56

	LeaveCriticalSection(&m_csOLASrv);

	return lSessionNumber;
}


long COLAProgressSrvEx::AddParams(long lSessId, char * pInBuffer)
{
	long lRtc = 0;
	CString	sLogMsg;

	EnterCriticalSection(&m_csOLASrv);

	sLogMsg.Format("AddParams Session %ld before", lSessId);		//4.0.20.56
	_LOGMSG.LogMsg(sLogMsg);		//4.0.20.56

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

	sLogMsg.Format("AddParams Session %ld after", lSessId );		//4.0.20.56
	_LOGMSG.LogMsg(sLogMsg);		//4.0.20.56
	
	LeaveCriticalSection(&m_csOLASrv);

	return lRtc;
}


long COLAProgressSrvEx::EndSession(long lSessId)
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

/******************************************************************************
 Description:	Call OLA's GetCardInfo
 Returns:      	0 - OK, else - Fail
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			11/11/2008	 13:34		Start 4.0.21.21 TD-69389
******************************************************************************/
long COLAProgressSrvEx::GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR* pRetData, long lFlags, long *lSesionType, long lPumpNumber, PAY_AT_PUMP_INFO* pInPAPBuffer)
{
	GCIRetCode	retCode = GCI_OK;	
	long		lRtc = 0;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
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
			//Debug Msg _LOGMSG.LogOLABufferMsg(lPumpNumber, LOG_PUMP, ">>> COLAProgressSrvEx:GetCardInfo ", (LPCTSTR)sOutstr);
			_LOGMSG.LogMsg(">>> COLAProgressSrvEx","GetCardInfo", lPumpNumber, LOG_PUMP, "GetCardInfo Before");

			//prepare to GetCardInfo
			CComBSTR	sInBuff = sOutBuffer;		
			CComBSTR	sRetBuf;

			lRtc = m_pSrvPtr->GetCardInfo(sInBuff.m_str, &sRetBuf.m_str);	
			
			//Copy to return data
			if (!lRtc)
			{
				*pRetData = sRetBuf.Copy();
			}
			
			CString strMsg;
			strMsg.Format("GetCardInfo After, Rtc=%ld",lRtc );
			_LOGMSG.LogMsg(">>> COLAProgressSrvEx","GetCardInfo", lPumpNumber, LOG_PUMP, strMsg);
		}
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

	return retCode;
}