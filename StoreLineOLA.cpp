// StoreLineOLA.cpp: implementation of the CStoreLineOLA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StoreLineOLA.h"
#include "XMLInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStoreLineOLA::CStoreLineOLA()
{

}

CStoreLineOLA::~CStoreLineOLA()
{

}

BOOL CStoreLineOLA::CreateOLAEvent()
{
	BOOL bRes = TRUE;
	_pOLAService2EventsHandler = NULL;
		
	_pOLAService2EventsHandler = new COLAService2EventsHandler;
	// Advise the server of the sink.
	IUnknownPtr pUnk = m_pSrvPtr;
	HRESULT hRes = _pOLAService2EventsHandler->DispEventAdvise(pUnk);
	if ( FAILED(hRes) )
	{			
		_pOLAService2EventsHandler = NULL;
		bRes = FALSE;
	}
	return bRes;
}


BOOL CStoreLineOLA::CreateInitXML(char * strOutXml)
{
	CXMLInterface xmlInitDoc;
	CXMLPointers  pCurrElement,pChildNode2,pCurrElement3,pChildNode,pCurrElement4;
	BOOL bRetCode = TRUE;
	CComBSTR	  bstrXML;

	BOOL bIsMaster = _Module.m_server.IsMasterComputer();
	bRetCode = xmlInitDoc.InitDoc();
	if (bRetCode)
		bRetCode = xmlInitDoc.CreateElement("OLAInit",&pCurrElement);
	if (bRetCode)
		bRetCode = xmlInitDoc.AppendRootElement(&pCurrElement);
	if (bRetCode)
		bRetCode = xmlInitDoc.CreateElement("Data",&pCurrElement3);
	if (bRetCode)
		bRetCode = xmlInitDoc.AppendChild(&pCurrElement3,&pCurrElement);
	if (bRetCode)
	{
		if (bIsMaster)
			bRetCode = xmlInitDoc.SetAttribute("IsMaster","1",&pCurrElement3);
		else
			bRetCode = xmlInitDoc.SetAttribute("IsMaster","0",&pCurrElement3);
	}

 	xmlInitDoc.GetXmlString(&bstrXML);
	char tmp[MAX_BUF];
	int iLen = WideCharToMultiByte(CP_ACP,0,bstrXML,-1,tmp,sizeof(tmp),NULL,NULL);
	strcpy(strOutXml,tmp);
	xmlInitDoc.CloseDoc();
	return bRetCode;


}

void CStoreLineOLA::Init()
{
	long lRtc=0;
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
					hRes = m_pSrvPtr.CreateInstance(/*OLASTORELINELib::CLSID_OLAService*/__uuidof(OLASTORELINELib::OLAService));			
					if(SUCCEEDED(hRes))
					{
						if (!CreateOLAEvent())
						{
							_LOGMSG.LogMsg("OLA with events: COM failure, OLA server will have no event support" , LOG_LEVEL_0);
							LoadRegistryParams();
							LaunchOLA();
							ResetConnectionAttempts();

						}
						else
						{
							LoadRegistryParams();
							LaunchOLA();
							
							_Module.m_server.m_cProtectedDataHandler.SetRemoveProhibitedData(TRUE);	//4.0.20.501 TD 60236
							
							ResetConnectionAttempts();
						}
					}
					else
					{
						_LOGMSG.LogOLAMsg(0, 0, 0, "OLAInUse = 12 >>> Service2 NOT FOUND !!!", 0);
					}
				}    
			}
		}

		catch (_com_error& e)
		{
			Error(e,hRes,FALSE);
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF; //ola12345
			DecrementConnectionAttempts();
		}

		catch (...)
		{
			Error();
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF; //ola12345
			DecrementConnectionAttempts();
		}

		LeaveCriticalSection(&m_csOLASrv);
	}

	if(m_pSrvPtr)
	{
		EnterCriticalSection(&m_csOLASrv);
 
		try
		{	
				char		strXML[MAX_BUF];
				if (CreateInitXML(strXML))
				{
					_bstr_t bstrXML(strXML);
					lRtc = m_pSrvPtr->Initialize(bstrXML);
				}
				else
					_LOGMSG.LogOLAMsg(0, 0, 0, "OLAInUse = 12 >>> Can't create init2 xml !!!", 0);

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
	}
}

void CStoreLineOLA::HandleStatusEvent(BSTR sInBuffer)
{
	//TBD
}

/*void CStoreLineOLA::ReleaseService(BOOL bCloseThread)
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
}*/

void CStoreLineOLA::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE*/)
{

	HRESULT hRes = 0;
	EnterCriticalSection(&m_csOLASrv); 
		
	if(m_bValid)
	{
		m_bValid = FALSE;//4.0.3.45
		
				
		try
		{
			m_pSrvPtr->Finilize();

			
			IUnknownPtr pUnk = m_pSrvPtr;
			hRes = _pOLAService2EventsHandler->DispEventUnadvise(pUnk);
			
			if(FAILED(hRes))
			{
				CString str;
				str.Format("Failed to unadvise OLA, error %x" , hRes);
				_LOGMSG.LogMsg(str);
			}

			delete _pOLAService2EventsHandler;
			_pOLAService2EventsHandler = NULL;

			m_pSrvPtr.Release();
		
		}

		catch (...)
		{

		}

		if (bCloseThread)
			_Module.m_server.CloseOLAThread();  	
		else
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_WAIT_FOR_KILL;

		//m_bValid = FALSE;
	}
	
	LeaveCriticalSection(&m_csOLASrv);
}
long CStoreLineOLA::EndSession(long lSessId)
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
long CStoreLineOLA::QuerySession(long lSessionNumber, BSTR * pOutBuffer)
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
long CStoreLineOLA::StartSession(char * pInBuffer, char *pszType)
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

//4.0.18.501 merge from 1018
void CStoreLineOLA::ParseNeedMoreParamData(CString *pInStr,PAY_AT_PUMP_INFO *pInfo, CPumpTransact *pTrs, long *lRetCode)
{
	BOOL bFound;
	int iLen ;
	char *pParam;

	CString str;
	if(LOG_BASIC_FLOW_CONTROL)
		str = ">>> OLA: Need more params :";

	int iFound;
	iFound =  pInStr->Find("CarRegistration[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		//4.0.6.501
		memset(pInfo->CardSaleInfo.cardData.sFullVehicleId , ' ' , sizeof(pInfo->CardSaleInfo.cardData.sFullVehicleId));
		pInfo->CardSaleInfo.cardData.sFullVehicleId[0] = '?';
		*lRetCode += REQUIRED_CAR_REGISTRATION;
		if(LOG_BASIC_FLOW_CONTROL)
			str += "CarRegistration";
	}

	iFound = pInStr->Find("Odometer[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "Odometer";
		}
		
		//4.0.6.501
		memset(pInfo->CardSaleInfo.cardData.sOdometer, ' ', sizeof(pInfo->CardSaleInfo.cardData.sOdometer));
		memset(pInfo->CardSaleInfo.extraData.sOdometerExt, ' ', sizeof(pInfo->CardSaleInfo.extraData.sOdometerExt));

		pInfo->CardSaleInfo.cardData.sOdometer[0] = '?';
		*lRetCode += REQUIRED_ODOMETER;
	}

	iFound = pInStr->Find("PIN[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		//4.0.2.15 Start 
		//Added explicit call to SysFreeString.  This is slower,
		//but we can be certain that all resources are freed.
		BSTR bstrTmpInStr = pInStr->AllocSysString();

		//Create a _bstr_t object that allocated a copy
		//of bstrTmpInStr.  This copy is freed in the _bstr_t's
		//d'tor.
		_bstr_t sEncryptionKeysBuffer(bstrTmpInStr , true); 

		//Free tmp string.  There is a separate copy of this BSTR
		//in sEncryptionKeysBuffer
		SysFreeString(bstrTmpInStr);
		//4.0.2.15 end

		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";

			str += "PIN";
		}
		pInfo->CardSaleInfo.cardData.sPINBlock[0] = '?';
		*lRetCode += REQUIRED_PIN;
		_Module.m_server.UpdateEncryptionKeys(0,sEncryptionKeysBuffer);  // 2.9.1.2
	}

	iFound = pInStr->Find("GeneralID[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";

			str += "GeneralID";
		}
		memset(pInfo->CardSaleInfo.cardData.sGeneralID , ' ' , sizeof(pInfo->CardSaleInfo.cardData.sGeneralID));
		pInfo->CardSaleInfo.cardData.sGeneralID[0] = '?';
		*lRetCode += REQUIRED_GENERAL_ID;
	}

	iFound = pInStr->Find("ReturnTank[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";

			str += "ReturnTank";
		}
		pInfo->CardSaleInfo.cardData.sReturnTank = '?';
		*lRetCode += REQUIRED_RETURN_TANK;
	}

	iFound = pInStr->Find("DriverID[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "DriverID";
		}
		// 4.0.6.501
		memset(pInfo->CardSaleInfo.cardData.sDriverId , ' ' , sizeof(pInfo->CardSaleInfo.cardData.sDriverId));
		memset(pInfo->CardSaleExtraData3.sDriverId_MSB , ' ' , sizeof(pInfo->CardSaleExtraData3.sDriverId_MSB));	//4.0.23.370 117473 //4.0.24.90
		memset(pInfo->CardSaleExtraData4.sDriverId_MSB2 , ' ' , sizeof(pInfo->CardSaleExtraData4.sDriverId_MSB2));  //4.0.23.370 117473 //4.0.24.90
		pInfo->CardSaleInfo.cardData.sDriverId[0] = '?';		
		*lRetCode += REQUIRED_DRIVER_ID;
	}

	//iFound = pInStr->Find("Track2[S]");
	//if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))

	iFound = pInStr->Find("Track2[S]");
	//4.0.14.30 if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))  //4.0.14.30
	if((iFound != -1) && (GetParamLenEx("Track2[S]",pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "Track2";
		}

		pInfo->CardSaleInfo.cardData.sTrack2Data[0] = '?';		
		*lRetCode += REQUIRED_PAYMENT_CARD;
	}

	iFound = pInStr->Find("PromptRewards[B]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "PromptRewards";
		}
		pInfo->CardSaleInfo.cardData.sGRPrompt = '?';
		*lRetCode += REQUIRED_PROMPT_REWARDS;
	}
	
	iFound = pInStr->Find("GRVoucherNumber[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "GRVoucherNumber";
		}
		pInfo->CardSaleInfo.cardData.sGRVoucherNumber[0] = '?';
		*lRetCode += REQUIRED_VOUCHER_NUMBER;
	}

	iFound = pInStr->Find("GRKeyCode[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "GRKeyCode";
		}
		pInfo->CardSaleInfo.cardData.sGRKeyCode[0] = '?';
		*lRetCode += REQUIRED_KEY_CODE;
	}

	//4.0.15.502
	iFound = pInStr->Find("PONumber[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "PONumber";
		}
		pInfo->CardSaleExtraData3.sPONumber[0] = '?';
		*lRetCode += REQUIRED_PO_NUMBER_PARAM;
	}

/////////////////////////////
//		iFound = pInStr->Find("MediaType[S]");
//		if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
//		{
//				if(LOG_BASIC_FLOW_CONTROL)
///				{
//					if(lRetCode)
//						str += ", ";
//					str += "MediaType";
//				}
//				pInfo->CardSaleInfo.cardData.sTranType = '?';
//
//				CString stmp;
//				stmp.Format("need more parem [%c]", pInfo->CardSaleInfo.cardData.sTranType);
//				_LOGMSG.LogMsg(stmp);
//				_LOGMSG.LogMsg("Test");
//				
//				lRetCode += REQUIRED_MEDIA_TYPE;
//		}
//
////////////////////

	bFound = GetParam(pInStr,pInStr->Find("MediaType[S]"),&pParam,&iLen);
	if(bFound) 
	{
		if(iLen == 1)
		{
			pInfo->CardSaleInfo.cardData.sTranType = pParam[0];

		}
		else if(iLen == 0)
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{
				if(*lRetCode)
					str += ", ";
				str += "MediaType";
			}
			*lRetCode += REQUIRED_MEDIA_TYPE;
		}


	}

	//start  husky new promte

	iFound = pInStr->Find("Unit[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "Unit";
		}
		memset(pInfo->CardSaleExtraData2.sUnit , ' ', sizeof(pInfo->CardSaleExtraData2.sUnit)); //4.0.6.501
		pInfo->CardSaleExtraData2.sUnit[0] = '?';
		*lRetCode += REQUIRED_UNIT;
	}

	iFound = pInStr->Find("TripNumber[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "TripNumber";
		}
		memset(pInfo->CardSaleExtraData2.sTripNumber ,' ', sizeof(pInfo->CardSaleExtraData2.sTripNumber)); //4.0.6.501
		pInfo->CardSaleExtraData2.sTripNumber[0] = '?';
		*lRetCode += REQUIRED_TRIP_NUMBER;
	}


	iFound = pInStr->Find("CustomerReference[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "CustomerReference";
		}
		memset(pInfo->CardSaleExtraData2.sCustomerReference , ' ', sizeof(pInfo->CardSaleExtraData2.sCustomerReference)); //4.0.6.501
		pInfo->CardSaleExtraData2.sCustomerReference[0] = '?';
		*lRetCode += REQUIRED_CUSTOMER_REFERENCE;
	}


	iFound = pInStr->Find("TractorHub[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "TractorHub";
		}
		memset(pInfo->CardSaleExtraData2.sTractorHub , ' ',sizeof(pInfo->CardSaleExtraData2.sTractorHub));
		pInfo->CardSaleExtraData2.sTractorHub[0] = '?';
		*lRetCode += REQUIRED_TRACTOR_HUB;
	}

	iFound = pInStr->Find("TrailerNumber[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "TrailerNumber";
		}
		memset(pInfo->CardSaleExtraData2.sTrailerNumber , ' ' , sizeof(pInfo->CardSaleExtraData2.sTrailerNumber)); //4.0.6.501
		pInfo->CardSaleExtraData2.sTrailerNumber[0] = '?';
		*lRetCode += REQUIRED_TRAILER_NUMBER;
	}

	iFound = pInStr->Find("TrailerHub[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "TrailerHub";
		}
		memset(pInfo->CardSaleExtraData2.sTrailerHub ,' ', sizeof(pInfo->CardSaleExtraData2.sTrailerHub)); //4.0.6.501
		pInfo->CardSaleExtraData2.sTrailerHub[0] = '?';
		*lRetCode += REQUIRED_TRAILER_HUB;
	}

	iFound = pInStr->Find("TransAmount[F]"); //3.1.1.1
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "TransAmount";
		}
		pInfo->CardSaleInfo.extraData.sTotalAmt[0] = '?';
		*lRetCode += REQUIRED_TRANS_AMOUNT;

	
	}

//////////////////////////////////////

	iFound = pInStr->Find("DeclinedLoyaltyInvalid[B]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "DeclinedLoyaltyInvalid";
		}

		pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_LOYALTY_INVALID;
		*lRetCode += REQUIRED_DECLINE_LOYALTY_PROMTE;
	}


	iFound = pInStr->Find("DeclinedLoyaltyExpired[B]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "DeclinedLoyaltyExpired";
		}

		pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_LOYALTY_EXPIRED;
		*lRetCode += REQUIRED_DECLINE_LOYALTY_PROMTE;		
	}


	iFound = pInStr->Find("DeclinedLoyaltyWithPayCard[B]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "DeclinedLoyaltyWithPayCard";
		}

		pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_LOYALTY_WITH_PAYCARD;
		*lRetCode += REQUIRED_DECLINE_LOYALTY_PROMTE;		
	}

	//4.0.0.86
	iFound = pInStr->Find("DeclinePaymentRetry[B]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "DeclinePaymentRetry";
		}

		pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_PAYMENT_CARD;
		
		*lRetCode += REQUIRED_DECLINE_LOYALTY_PROMTE;		
	}
	//4.0.0.86
	
	
	iFound = pInStr->Find("PassCode[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "PassCode";
		}

		memset(pInfo->CardSaleExtraData2.sPassCode , ' ' , sizeof(pInfo->CardSaleExtraData2.sPassCode));
		pInfo->CardSaleExtraData2.sPassCode[0] = '?';
		*lRetCode += REQUIRED_PASS_CODE;		
	}

	//3.1.1.1
	iFound = pInStr->Find("PinPadAction[I]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "PinPadAction";
		}
		*lRetCode += REQUIRED_PINPAD_ACTION;
	}


	// 4.0.1.1
	iFound = pInStr->Find("ChargeTo[I]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "ChargeTo";
		}
		memset(pInfo->CardSaleExtraData2.sChargeTo , ' ' , sizeof(pInfo->CardSaleExtraData2.sChargeTo));
		pInfo->CardSaleExtraData2.sChargeTo[0]= '?';
		*lRetCode += REQUIRED_CHARGE_TO;
	}
	// 4.0.1.1

	// 4.0.1.31
	iFound = pInStr->Find("ZipCode[I]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "Zip Code";
		}
		memset(pInfo->CardSaleExtraData2.sZipCode, ' ' ,sizeof(pInfo->CardSaleExtraData2.sZipCode));
		pInfo->CardSaleExtraData2.sZipCode[0]= '?';
		*lRetCode += REQUIRED_ZIP_CODE;
	}
	// 4.0.1.31

	//46512
	iFound = pInStr->Find("ExtendedZipCode[I]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "Extended Zip Code";
		}
		memset(pInfo->CardSaleExtraData4.sExtendedZipCode, ' ' ,sizeof(pInfo->CardSaleExtraData4.sExtendedZipCode));
		pInfo->CardSaleExtraData4.sExtendedZipCode[0]= '?';
		*lRetCode += REQUIRED_ZIP_CODE;
	}

	//54295
	iFound = pInStr->Find("PumpTestPassword[I]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "Pump test password";
		}
		memset(pInfo->CardSaleExtraData4.sPumpTestPassword, ' ' ,sizeof(pInfo->CardSaleExtraData4.sPumpTestPassword));
		pInfo->CardSaleExtraData4.sPumpTestPassword[0]= '?';
//		*lRetCode += REQUIRED_PUMP_TEST_PWD;
	}


	//4.0.3.47
	iFound = pInStr->Find("RedeemPoints[B]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "RedeemPoints";
		}
		pInfo->CardSaleInfo.extraData.sRedeemPoints = '?' ;			
		*lRetCode += REQUIRED_REDEED_POINT_ACTION;		
	}
	
	//4.0.3.47

	//4.0.3.54

	iFound = pInStr->Find("ConfirmPrompt[B]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "ConfirmPrompt";
		}
		pInfo->CardSaleInfo.extraData.sConfirmPrompt = '?' ;			
		*lRetCode += REQUIRED_CONFIRM_PROMPT_ACTION;		
	}


	//4.0.5.26
	iFound = pInStr->Find("ClubCardTrack2[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "ClubCardTrack2";
		}

		pInfo->CardSaleInfo.cardData.sClubCardTrack2Data[0] = '?';		
		*lRetCode += REQUIRED_PAYMENT_LOYALTY_CARD;
	}

	//4.0.5.26

	//4.0.5.39
	iFound = pInStr->Find("ParamResult[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "ParamResult";
		}
		
		memset(pInfo->CardSaleExtraData3.sParamResult, ' ', sizeof(pInfo->CardSaleExtraData3.sParamResult));		//4.0.28.502 255727
		memset(pInfo->CardSaleExtraData5.sParamResultExt, ' ', sizeof(pInfo->CardSaleExtraData5.sParamResultExt));	//4.0.28.502 255727

		pInfo->CardSaleExtraData3.sParamResult[0] = '?';		
		*lRetCode += REQUIRED_PARAM_RESULT_DATA;
	}
	//4.0.5.39
	// 4.0.5.41
	iFound = pInStr->Find("RemoteID[I]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "RemoteID";
		}

		pInfo->CardSaleExtraData3.sRemoteID[0] = '?';		
		*lRetCode += REQUIRED_REMOTE_ID;
	}	

	// 4.0.5.42
	iFound = pInStr->Find("LogicShiftNumber[I]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "LogicShiftNumber";
		}

		pInfo->CardSaleExtraData3.sLogicShiftNumber[0] = '?';		
		*lRetCode += REQUIRED_LOGIC_SHIFT_NUMBER;
	}	


	iFound = pInStr->Find("AuthorizationNumber[S]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)
				str += ", ";
			str += "AuthorizationNumber";
		}

		pInfo->CardSaleInfo.cardData.sAuthNumber[0] = '?';		
		*lRetCode += REQUIRED_AUTHORIZATION_NUMBER;
	}	

	//4.0.9.507
	iFound = pInStr->Find("PromptName0[S]");
	//4.0.9999.9 if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		//PAY_AT_PUMP_INFO	sTmpPAPInfo;

		//memcpy(&sTmpPAPInfo,pInfo , sizeof(PAY_AT_PUMP_INFO));
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)	
				str += ", ";
			str += "PromptName0";
		}

		
		pInfo->CardSaleExtraData3.sParamResult[0] = '?';		
		_Module.m_server.SaveAndParsePromptInfo(pInStr, pTrs ,pInfo); //4.0.9999.0

		*lRetCode += REQUIRED_PARAM_RESULT_DATA;
	}	


	iFound = pInStr->Find("CombinedCardSelect[S]");
	
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)	
				str += ", ";
			str += "CombinedCardSelect";
		}

		
		pInfo->CardSaleExtraData3.sCombinedCardSelect = '?';		

		*lRetCode += REQUIRED_CnP_PARAM;
	}	

	iFound = pInStr->Find("CVMResult[I]");
	
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)	
				str += ", ";
			str += "CVMResult";
		}

		
		pInfo->CardSaleExtraData3.sCVMResult[0] = '?';		
		*lRetCode += REQUIRED_CnP_PARAM;
	}	

	iFound = pInStr->Find("TermResponseCode[I]");
	if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			if(*lRetCode)	
				str += ", ";
			str += "TermResponseCode";
		}

		
		pInfo->CardSaleExtraData3.sTermResponseCode[0] = '?';		
		*lRetCode += REQUIRED_CnP_PARAM;
	}	

	
	//4.0.9.507
	
	
	//End  husky new promte
	if(LOG_BASIC_FLOW_CONTROL)
		_LOGMSG.LogMsg("COLAStoreLine","ParseNeedMoreParam",0,LOG_NONE,str);
}


long CStoreLineOLA::AddParams(long lSessId, char * pInBuffer)
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

long CStoreLineOLA::GetCardInfoEx( long lClientId,BSTR sData, BSTR *pRetData,long lFlags /* = 0 */)
{
	long lRtc = 0;
	CString sLogMsg;

	EnterCriticalSection(&m_csOLASrv);
 
	try
	{	
		
		sLogMsg.Format("GetCardInfoEx before " );
		_LOGMSG.LogMsg(sLogMsg);

		lRtc = m_pSrvPtr->GetCardInfo(sData,pRetData); 
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
	
	sLogMsg.Format("GetCardInfoEx After ,Rtc=%ld",lRtc );	//4.0.18.501
	_LOGMSG.LogMsg(sLogMsg);

	return lRtc;


}

/******************************************************************************
 Description:	Call Ola's GeCardInfoEx method.
				
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			07/11/2007   10:54		Start 4.0.18.21
******************************************************************************/
long CStoreLineOLA::GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags,long *lSesionType, long lPumpNumber, PAY_AT_PUMP_INFO * pInPAPBuffer) //4.0.20.52 63936
{
	GCIRetCode retCode = GCI_OK;
	CString sLogMsg;

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GCI_FAILED_IO;
	else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
		retCode = GCI_SERVER_IN_MIDDLE_LOCK_SHIFT;
		
	if ( retCode == GCI_OK )
	{
		CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

		long lRtc = 0;
		CARD_SALE_ALL3 cardDataAll3;
		memset(&cardDataAll3, ' ', sizeof(CARD_SALE_ALL3));
		PAY_AT_PUMP_INFO info;
		
		info.CardSaleInfo.cardData   = cardDataAll3.CardSaleAll.data;
		info.CardSaleInfo.extraData  = cardDataAll3.CardSaleAll.extData;
		info.CardSaleExtraData2      = cardDataAll3.extData2;	
		info.CardSaleExtraData3      = cardDataAll3.extData3;
		info.CardSaleExtraData4      = cardDataAll3.extData4;
		
		if (sAccountNumber)
		{
			_bstr_t sTmpAccount(sAccountNumber, true);
			memcpy(info.CardSaleInfo.cardData.sAccountNumber , (const char *)sTmpAccount, min(sTmpAccount.length() ,sizeof(info.CardSaleInfo.cardData.sAccountNumber)));	//4.0.25.80 TD 142857
		}

		if (sTrack1)
		{
			_bstr_t sTmpTrack1(sTrack1, true);
			memcpy(info.CardSaleInfo.cardData.sTrack1Data , (const char *)sTmpTrack1, min(sTmpTrack1.length() ,sizeof(info.CardSaleInfo.cardData.sTrack1Data)));			//4.0.25.80 TD 142857

		}

		if (sTrack2)
		{
			CString sMsg;
			_bstr_t sTmpTrack2(sTrack2, true);
			memcpy(info.CardSaleInfo.cardData.sTrack2Data , (const char *)sTmpTrack2, min(sTmpTrack2.length() ,sizeof(info.CardSaleInfo.cardData.sTrack2Data)));			//4.0.25.80 TD 142857
		}				


		char sOutBuffer[3048]; 
		memset((char*)sOutBuffer , 0 , sizeof(sOutBuffer));

		_Module.m_server.m_cOLASrv->BuildRequest(SESSION_AUTHORIZE,
											 lPumpNumber,
											 (char*)&info,
											 (char*)sOutBuffer,
											 NULL, TRUE); 


		_bstr_t InBuff(sOutBuffer);
		CComBSTR  sRetBuf ;

		try
		{
			sLogMsg.Format("StoreLineOLA::GetCardInfo before " );
			_LOGMSG.LogMsg(sLogMsg);

			long lRtc  = m_pSrvPtr->GetCardInfo(InBuff ,&sRetBuf);

			if (!lRtc)
			{
				*pRetData = sRetBuf.Copy();
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
	
		sLogMsg.Format("StoreLineOLA::GetCardInfo - After Len=%ld, Rtc=%ld",sRetBuf.Length(), lRtc );
		_LOGMSG.LogMsg(sLogMsg);
	}

	return retCode;
}


// Gena 4.0.27.44 TD 179125
long CStoreLineOLA::GetHashedPAN(const long lPumpNumber, const char* pszOpenBuffIn, char* psHashedBuffOut, size_t* piHashedDataSize)
{
	long lRtc = 0;

	EnterCriticalSection(&m_csOLASrv);

	_bstr_t		cwsOpenBuffIn(pszOpenBuffIn);
	CComBSTR	cwsHashedBuff(L"");

	try
	{					
		lRtc =	m_pSrvPtr->GetHashedPan(lPumpNumber, cwsOpenBuffIn, &cwsHashedBuff.m_str);

		if(lRtc == OK)
		{
			long lSize = cwsHashedBuff.Length(); 

			if (lSize > ENCRYPTED_DATA_SIZE || lSize < 1)
				lRtc = ENCODE_STRING_ILLEGAL_SIZE; 
			else
				*piHashedDataSize = Bstr2Buffer(cwsHashedBuff.m_str, psHashedBuffOut, lSize);	
		}
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