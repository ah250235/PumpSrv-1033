// OLARouteSrv.cpp: implementation of the COLARouteSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLARouteSrv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLARouteSrv::COLARouteSrv()
{

}

COLARouteSrv::~COLARouteSrv()
{
	//ReleaseService(TRUE);
}


void COLARouteSrv::Init()
{
	HRESULT hRes;

	if ( m_bOLAStopped == FALSE )
	{
		EnterCriticalSection(&m_csOLASrv);

		try
		{
			if (_Module.m_server.GetOLAThreadState() == OLA_SERVER_OFF)
			{
				long lPumpNumber =0;
				if (!m_bValid && m_byConnectionAttempts)
				{
					hRes = m_pSrvPtr.CreateInstance(CLSID_GCA);
					
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
						m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber); //4.0.19.Marik 4.0.19.500
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


long COLARouteSrv::AddParams(long lSessId, char * pInBuffer)
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

void COLARouteSrv::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE */)
{
	EnterCriticalSection(&m_csOLASrv);
		
	if(m_bValid)
	{
		m_bValid = FALSE;//4.0.3.45
		try
		{
			m_pSrvPtr.Release();
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

long COLARouteSrv::StartSession(char * pInBuffer, char *pszType)
{
	_bstr_t sTransactionType(pszType);
	_bstr_t sInBuffer(pInBuffer);
	long lSessionNumber;
	
	EnterCriticalSection(&m_csOLASrv);

	try
	{					
		//4.0.0.39 
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


long COLARouteSrv::EndSession(long lSessId)
{
	long lRtc = 0;

	EnterCriticalSection(&m_csOLASrv);

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
	
	return lRtc;
}


long COLARouteSrv::QuerySession(long lSessionNumber, BSTR * pOutBuffer)
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
/******************************************************************************
 Description:	This procedure convert direct request to GetCardinfo to session, 
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			8/2/2004   10:20		Start
******************************************************************************/
				   
long COLARouteSrv::GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData, long lFlags, long *lSesionType, long lPumpNumber, PAY_AT_PUMP_INFO * pInPAPBuffer)	//4.0.20.52 63936
{
	long lOlaRecSize = (lFlags & SWIPE_THREE_OLA_REC)?sizeof(CARD_SALE_ALL3):sizeof(CARD_SALE_ALL2);
	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;	
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GCI_FAILED_IO;
	else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
		retCode = GCI_SERVER_IN_MIDDLE_LOCK_SHIFT;   //4.0.1.40

	long lRtc = 0;
	BOOL bAskUserToTryAgain = FALSE;

	EnterCriticalSection(&m_csOLASrv); //4.0.22.509 75755

	try	//4.0.22.509 75755
	{
		if ( retCode == GCI_OK )
		{
			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
//			EnterCriticalSection( &_Module.m_server.m_csPumpArray ); //4.0.22.509 75755

			_Module.m_server.m_cOLASrv->ResetConnectionAttempts();
			_Module.m_server.m_cOLASrv->ResetRetryAttemps(TRUE);

			PAY_AT_PUMP_INFO	info;
				
			if (pInPAPBuffer == NULL)	//4.0.20.52 63936
				_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&info);
			else
				info = *pInPAPBuffer;
			
			
			if (sAccountNumber)
			{
				_bstr_t sTmpAccount(sAccountNumber, true);
				if (sTmpAccount.length())
					memcpy(info.CardSaleInfo.cardData.sAccountNumber , (const char *)sTmpAccount, min(sTmpAccount.length() ,sizeof(info.CardSaleInfo.cardData.sAccountNumber)));	//4.0.22.473 TD 142857	
				else
					memset(info.CardSaleInfo.cardData.sAccountNumber , ' ', sizeof(info.CardSaleInfo.cardData.sAccountNumber));

			}
			else
			{
				memset(info.CardSaleInfo.cardData.sAccountNumber , ' ', sizeof(info.CardSaleInfo.cardData.sAccountNumber));
			}

			if (sTrack1)
			{
				_bstr_t sTmpTrack1(sTrack1, true);
				if (sTmpTrack1.length())
					memcpy(info.CardSaleInfo.cardData.sTrack1Data , (const char *)sTmpTrack1, min(sTmpTrack1.length() ,sizeof(info.CardSaleInfo.cardData.sTrack1Data)));			//4.0.22.473 TD 142857	
				else
					memset(info.CardSaleInfo.cardData.sTrack1Data , ' ', sizeof(info.CardSaleInfo.cardData.sTrack1Data));
			}
			else
			{
				memset(info.CardSaleInfo.cardData.sTrack1Data , ' ', sizeof(info.CardSaleInfo.cardData.sTrack1Data));
			}

			if (sTrack2)
			{
				_bstr_t sTmpTrack2(sTrack2, true);
				if (sTmpTrack2.length())
					memcpy(info.CardSaleInfo.cardData.sTrack2Data , (const char *)sTmpTrack2, min(sTmpTrack2.length() ,sizeof(info.CardSaleInfo.cardData.sTrack2Data)));			//4.0.22.473 TD 142857	
				else
					memset(info.CardSaleInfo.cardData.sTrack2Data , ' ', sizeof(info.CardSaleInfo.cardData.sTrack2Data));
			}
			else
			{
				memset(info.CardSaleInfo.cardData.sTrack2Data , ' ', sizeof(info.CardSaleInfo.cardData.sTrack2Data));

			}

			PAY_AT_PUMP_INFO    cPayAtPumpInfo;  //4.0.5.0

			memset(&cPayAtPumpInfo , ' ' , sizeof(cPayAtPumpInfo));

			char sOutBuffer[3048]; //4.0.5.0
			memset((char*)sOutBuffer , 0 , sizeof(sOutBuffer));

			_Module.m_server.m_cOLASrv->BuildRequest(SESSION_AUTHORIZE,
												 lPumpNumber,
												 (char*)&info,
												 (char*)sOutBuffer,
												 NULL, TRUE); // 4.0.6.501	


			_bstr_t InBuff(sOutBuffer);
			CComBSTR  sRetBuf ;

 			long lRtc  = m_pSrvPtr->GetCardInfo(InBuff ,&sRetBuf);

			CString sOutstr, strNew;
			sOutstr.Format("%s",sOutBuffer);
			_Module.m_server.m_cProtectedDataHandler.RemoveDataFromLog(sOutstr,strNew);	//4.0.22.501 TD 72716


			CString str;
			str.Format(">>> OLARouteSrv:GetCardInfo ");
			_LOGMSG.LogOLABufferMsg(lPumpNumber,LOG_PUMP,str,(LPCTSTR)strNew);

			if (!lRtc)
			{
				*pRetData = sRetBuf.Copy();
			}


			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
//			LeaveCriticalSection(&_Module.m_server.m_csPumpArray); //4.0.22.509 75755
	
		}
	}
	catch (_com_error & e) //4.0.22.509 75755
	{
		Error(e);		
		lRtc = OLASRV_COM_ERROR;
		_Module.m_server.CloseOLAThread();
	}
	
	catch(...) //4.0.22.509 75755
	{
		Error();		
		lRtc = OLASRV_COM_ERROR;
		_Module.m_server.CloseOLAThread();
	}

	LeaveCriticalSection(&m_csOLASrv);	//4.0.22.509 75755

	return retCode;

}
