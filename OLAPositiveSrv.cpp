// OLAPositiveSrv.cpp: implementation of the COLAPositiveSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLAPositiveSrv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLAPositiveSrv::COLAPositiveSrv()
{
	SetIFSFProtocoll(TRUE); //4.0.8.500
}

COLAPositiveSrv::~COLAPositiveSrv()
{
	//ReleaseService(TRUE);//4.0.3.45
}

/////////////////////////////////////////////////////////
//OLA initialization.
/////////////////////////////////////////////////////////
void COLAPositiveSrv::Init()
{
	HRESULT hRes;

	if (!m_bOLAStopped)
	{
		EnterCriticalSection(&m_csOLASrv);

		try
		{
			if (_Module.m_server.GetOLAThreadState() == OLA_SERVER_OFF)
			{
				if (!m_bValid && m_byConnectionAttempts)
				{
					hRes = m_pSrvPtr.CreateInstance(CLSID_Services);

					if(FAILED(hRes))
					{
						_com_issue_error(hRes);
						_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF;
					}

					else
					{
						long lPumpNumber= 0;

						m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber);  //4.0.8.40

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
		catch(...)
		{
			Error();
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF; //ola12345

			//4.0.2.37
			DecrementConnectionAttempts();
		}

		LeaveCriticalSection(&m_csOLASrv);
	}
}

/////////////////////////////////////////////////////////
//Card data extraction methods.
/////////////////////////////////////////////////////////
long COLAPositiveSrv::GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags,long *lSesionType, long lPumpNumber, PAY_AT_PUMP_INFO * pInPAPBuffer)	//4.0.20.52 (TD 63936)
{
	long lRtc = 0;
	//_bstr_t sRetBuf;
	CComBSTR sRetBuf;//4.0.18.501 merge from 4.0.15.285

	EnterCriticalSection(&m_csOLASrv);

	try
	{	
		
		sRetBuf.AppendBSTR(m_pSrvPtr->GetCardInfo ("",sAccountNumber, sTrack1, sTrack2, sShortCardName));//4.0.15.285
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
	
	if (lRtc == 0 && sRetBuf.Length() != 0) //4.0.18.501 merge from 4.0.15.285
	{
		*pRetData = sRetBuf.Copy();
	}

	return lRtc;
}

long COLAPositiveSrv::AddParams(long lSessId , char * pInBuffer)
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
long COLAPositiveSrv::StartSession(char * pInBuffer, char *pszType)
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

long COLAPositiveSrv::QuerySession(long lSessionNumber, BSTR * pOutBuffer)
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

long COLAPositiveSrv::EndSession(long lSessionId)
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
void COLAPositiveSrv::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE*/)
{
	EnterCriticalSection(&m_csOLASrv);
		
	if(m_bValid)
	{
		m_bValid = FALSE;	//4.0.3.45
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

		//_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF;  //ola12345
		//m_bValid = FALSE;	//4.0.3.45
	}
	
	LeaveCriticalSection(&m_csOLASrv);
}


long COLAPositiveSrv::UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData)
{
	long lRtc = 0;

	EnterCriticalSection(&m_csOLASrv);

	try
	{
		CString szMsg;

		PAY_AT_PUMP_INFO savedInfo; //4.0.5.0
		char sTmpSessionId[32]={0};	
		memset(&savedInfo,' ',sizeof(savedInfo));
				
		_Module.m_server.m_cPumpArray.GetAt(lPumpNumber-1).GetPAPInfo(&savedInfo);						

		sprintf_s(sTmpSessionId, _countof(sTmpSessionId),"%.2s%.4s",savedInfo.CardSaleExtraData2.sAuthorizeSessionID_MSB , savedInfo.CardSaleInfo.cardData.sAuthorizeSessionID);
		lSessionId = a2l((unsigned char *) sTmpSessionId , 6);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			szMsg.Format("call UploadBuffer to Pump=%ld, session=%ld",lPumpNumber ,lSessionId);  //4.0.6.507
			_LOGMSG.LogMsg(szMsg);
		}
	
		lRtc = m_pSrvPtr->UploadBuffer(lTerminalId, lPumpNumber, lSessionId, lUploadType, sInBuffer, sOutBuffer, lExDataSize, sExData);
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
	
	return lRtc;
}




/////////////////////////////////////////////////////////
//Card data extraction methods.
/////////////////////////////////////////////////////////
long COLAPositiveSrv::GetCardInfoEx(long lClientID,BSTR sInBuffer,BSTR *sRetData,long lFlags )
{
	long lRtc = 0;
//	_bstr_t sRetBuf;
	CComBSTR sRetBuf;	//4.0.18.501 merge from 4.0.15.285

	EnterCriticalSection(&m_csOLASrv);

	try
	{	
		
		sRetBuf.AppendBSTR(m_pSrvPtr->GetCardInfoEx(lClientID,sInBuffer,""));	//4.0.15.285
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
	
	if (lRtc == 0 && sRetBuf.Length() != 0)	//4.0.18.501 merge from 4.0.15.285
	{
		*sRetData = sRetBuf.Copy();
	}

	return lRtc;
}
