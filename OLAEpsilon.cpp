// OLAEpsilon.cpp: implementation for the COLAEpsilon class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "OLAEpsilon.h"




void COLAEpsilon::AddedPreAuthInformation(long lPumpNumber, long & lOffset, char * pOutBuffer, long & lOLAPumpServiceMode, long & lGradeRestrict)
{
	PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);
	long lCount = cPumpItems.lCounter;
	char sFiledName[MAX_FIELD_NAME];
	long	lPriceCash = 0, lPriceCredit = 0;
	long	lNumOfItems = 0;//4.0.11.504	fix this function TD 14868

	// Add item list 
	if (lPumpNumber && lCount)
	{
		for (int i = 0; i < cPumpItems.lCounter; i++)
		{
			// send only full information...  
			if (cPumpItems.m_cItemSoldInfo[i].lGrade)
			{
				if (cPumpItems.m_cItemSoldInfo[i].bFuelItem)
				{
					if ((!lGradeRestrict) ||    //4.0.7.500
						(cPumpItems.m_cItemSoldInfo[i].lGrade == lGradeRestrict))
					{
						sprintf(sFiledName, "ItemCode%d[S]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%03ld,", _Module.m_server.GetGradeCode((short)cPumpItems.m_cItemSoldInfo[i].lGrade));
						
						sprintf(sFiledName, "ItemGrade%d[I]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%03ld,", cPumpItems.m_cItemSoldInfo[i].lGrade);
						
						sprintf(sFiledName, "ItemPrice%d[F]", lNumOfItems);

						if (lOLAPumpServiceMode == FULL_SERVICE_MODE)
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%ld.%03ld,", cPumpItems.m_cItemSoldInfo[i].lPriceCreditFullService / 1000, cPumpItems.m_cItemSoldInfo[i].lPriceCreditFullService % 1000);
						else
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%ld.%03ld,", cPumpItems.m_cItemSoldInfo[i].lPriceCreditSelfService / 1000, cPumpItems.m_cItemSoldInfo[i].lPriceCreditSelfService % 1000);
						//4.0.11.502
						//Attached is the new fields that will be send to OLA in order to report low price & high price.   

						GetGradePrice((short)cPumpItems.m_cItemSoldInfo[i].lGrade, lPriceCash, lPriceCredit, lOLAPumpServiceMode);//4.0.11.122


						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%ld.%03ld,", lPriceCash / 1000, lPriceCash % 1000);//4.0.11.122
						sprintf(sFiledName, "ItemPriceHigh%d[S]", lNumOfItems);//4.0.11.50


						if (_Module.m_server.IsCurrentRegion(REGION_USA))	  //4..
							sprintf(sFiledName, "ItemPriceLow%d[F]", lNumOfItems);//4.0.1023.1170
						else
							sprintf(sFiledName, "ItemPriceLow%d[S]", lNumOfItems);//4.0.11.50


						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%ld.%03ld,", lPriceCash / 1000, lPriceCash % 1000);//4.0.11.122
						sprintf(sFiledName, "ItemPriceHigh%d[S]", lNumOfItems);//4.0.11.50


						if (_Module.m_server.IsCurrentRegion(REGION_USA))
							sprintf(sFiledName, "ItemPriceHigh%d[F]", lNumOfItems);//4.0.11.1170
						else
							sprintf(sFiledName, "ItemPriceHigh%d[S]", lNumOfItems);//4.0.11.50


						if (cPumpItems.m_cItemSoldInfo[i].lGrade)
						{
							TStaticString<STATIC_STRING_64> sGradeFullName;

							_Module.m_server.m_GradeData.GetGradeName(cPumpItems.m_cItemSoldInfo[i].lGrade, sGradeFullName);

							sprintf(sFiledName, "ItemName%d[S]", lNumOfItems);
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *)((LPCSTR)sGradeFullName), sGradeFullName.Size());
						}

						// sprintf(sFiledName,"ItemPriceLow%d[S]",lNumOfItems);//4.0.11.50
						// lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%ld.%03ld,",lPriceCash/1000,lPriceCash%1000);//4.0.11.122
						// sprintf(sFiledName,"ItemPriceHigh%d[S]",lNumOfItems);//4.0.11.50
						// lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%ld.%03ld,",lPriceCredit/1000,lPriceCredit%1000);//4.0.11.122

						sprintf(sFiledName, "ItemIsFuel%d[B]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *)"Y", 1);

						sprintf(sFiledName, "ItemType%d[S]", lNumOfItems);

						// required only for pilot   //4.0.9999.0
						if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetDieselPumpFlags())
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *)"C", 1);
						else
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *)"P", 1);
						lNumOfItems++;  //4.0.11.504
					}
				}
				else
				{
					sprintf(sFiledName, "ItemIsFuel%d[B]", lNumOfItems);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *)"N", 1);

					sprintf(sFiledName, "ItemCode%d[S]", lNumOfItems);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%03ld,", cPumpItems.m_cItemSoldInfo[i].lGrade);
					lNumOfItems++;  //4.0.11.504
				}
			}
		}

		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCount[I]", (BYTE *)"%s=%03ld,", lNumOfItems);
	}

	return;
}

COLAEpsilon::COLAEpsilon(void)
{
	
}


COLAEpsilon::~COLAEpsilon(void)
{
	
}


BOOL COLAEpsilon::InitInterface()
{
	BOOL bRetCode = FALSE;
	HRESULT hRes = S_OK;
	CLSID clsid;


	//Get the CLSID for the IServices interface.
	hRes = CLSIDFromProgID(L"EpsilonLinkSrv.Services" , &clsid);

	try 
	{
		if(SUCCEEDED(hRes))
		{
			hRes = CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IUnknown, (void FAR * FAR *)&m_pUnknown);
		}

		if(FAILED(hRes))
		{
			char sMsg[256];
			sprintf(sMsg, "COLAEpsilon::InitInterface() - failed - returned %ld", hRes);
			_LOGMSG.LogMsg(sMsg, LOG_LEVEL_0);
			_com_issue_error(hRes);

		}

	}

	catch (_com_error& e)
	{
		_LOGMSG.LogMsg("COLAEpsilon::OLA CreateInstance Failed - COM exception. " ,LOG_LEVEL_1);
		Error(e,hRes,TRUE);
	}

	catch (...)
	{
		_LOGMSG.LogMsg("COLAEpsilon::OLA CreateInstance Failed - catch (...). " ,LOG_LEVEL_1);
		Error();

	}
	
	if(SUCCEEDED(hRes))
	{
		hRes = m_pUnknown->QueryInterface(IID_IDispatch , (void**)&m_pDispatch);

		if(FAILED(hRes))
		{
			char sMsg[256];
			sprintf(sMsg, "COLAEpsilon::InitInterface() - QueryInterface failed - returned %ld", hRes);
			_LOGMSG.LogMsg(sMsg, LOG_LEVEL_0);
			_com_issue_error(hRes);
		}

		if(SUCCEEDED(hRes))
		{
			char sMsg[256];
			sprintf(sMsg, "COLAEpsilon::InitInterface() - QueryInterface Successful");
			_LOGMSG.LogMsg(sMsg, LOG_LEVEL_3);

			//Get DISPID handles to OLA session handling methods
			hRes = GetFunctionPointers();

			//if the last function call was successful, we can change the returned value to true.
			if(SUCCEEDED(hRes))
				bRetCode = TRUE;
		}
	}
	else
	{
		char sMsg[256];
		sprintf(sMsg, "COLAEpsilon::InitInterface() - CoCreateInstance returned %ld", hRes);
		_LOGMSG.LogMsg(sMsg, LOG_LEVEL_0);
	}

	return bRetCode;
}

long COLAEpsilon::StartSession(char * pInBuffer, char * pszType)
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


	try
	{
		hRes = m_pDispatch->Invoke(MethodIDs[1],
			IID_NULL,
			LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD,
			&cParams,
			&cRes,
			&cException,
			&iErrArg);

		if (SUCCEEDED(hRes))
		{
			cRes.vt = VT_I4;
			lSessionNumber = cRes.lVal;
		}
		else
		{
			if (FAILED(hRes))
			{
				CString sMsg;
				sMsg.Format("COLAEpsilon::StartSession - failed - returned %ld", hRes);
				_LOGMSG.LogMsg(sMsg.GetBuffer(0), LOG_LEVEL_0);
			}

			m_pDispatch->Release();
			m_pUnknown->Release();
			m_pDispatch = NULL;
			m_pUnknown = NULL;

			InitInterface();

			hRes = m_pDispatch->Invoke(MethodIDs[1],
				IID_NULL,
				LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD,
				&cParams,
				&cRes,
				&cException,
				&iErrArg);

			if (SUCCEEDED(hRes))
			{
				cRes.vt = VT_I4;
				lSessionNumber = cRes.lVal;

				CString sMsg;
				sMsg.Format("COLAEpsilon::StartSession - second attempt  - returned %ld", hRes, lSessionNumber);
				_LOGMSG.LogMsg(sMsg.GetBuffer(0), LOG_LEVEL_0);

			}
			else
			{
				if (FAILED(hRes))
				{
					CString sMsg;
					sMsg.Format("COLAEpsilon::StartSession - failed - returned %ld", hRes);
					_LOGMSG.LogMsg(sMsg.GetBuffer(0), LOG_LEVEL_0);
				}

				Error();  //4.0.10.49
				lSessionNumber = OLASRV_COM_ERROR;

				CString sMsg;
				sMsg.Format("COLA32Srv::StartSession() - Invoke Failed - returned %ld", hRes);
				_LOGMSG.LogMsg(sMsg.GetBuffer(0), LOG_LEVEL_1);
			}
		}
	}
	catch (_com_error& e)
	{
		_LOGMSG.LogMsg("COLA32Srv::StartSession  Failed - COM exception. ", LOG_LEVEL_1);
		Error(e, hRes, TRUE);
	}
	catch (...)
	{
		_LOGMSG.LogMsg("COLA32Srv::StartSession Failed - catch (...). ", LOG_LEVEL_1);
		Error();
	}
	LeaveCriticalSection(&m_csOLASrv);

	return lSessionNumber;
}

void COLAEpsilon::Init()
{

	BOOL bInitializationSuccessfull = FALSE;

	if(!m_bOLAStopped)
	{
		if(m_byConnectionAttempts)
		{
			char sMsg[256];
			sprintf(sMsg, "COLAEpsilon::Init() - m_byConnectionAttempts %d", m_byConnectionAttempts);
			_LOGMSG.LogMsg(sMsg, LOG_LEVEL_0);

			EnterCriticalSection(&m_csOLASrv);

			if(!m_pUnknown || !m_pDispatch)
			{
				try
				{
					bInitializationSuccessfull = InitInterface();
				}

				catch(_com_error &cError)
				{
					Error(cError);
					_LOGMSG.LogMsg("COLAEpsilon::OLA InitInterface Failed - COM exception. " ,LOG_LEVEL_1);
				}

				catch(...)
				{
					CString sMsg("COLAEpsilon::COLAEpsilon Failed to initialize - general error");
					_LOGMSG.LogMsg(sMsg , LOG_LEVEL_0);
				}
			}
			else
			{
				_LOGMSG.LogMsg("COLAEpsilon::Init() - Can't call InitInterface()", LOG_LEVEL_0);
			}

			if(bInitializationSuccessfull)
			{
				long lPumpNumber=0;
				m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber);
				LoadRegistryParams();
				LaunchOLA();
				ResetRetryAttemps();
				// Login(); // CR 424478
				_LOGMSG.LogMsg("COLAEpsilon::Init() - InitInterface Successful", LOG_LEVEL_3);
			}
			else
			{
				_LOGMSG.LogMsg("COLAEpsilon::Init - failed to initialize" , LOG_LEVEL_0); 
				DecrementConnectionAttempts();
			}

			LeaveCriticalSection(&m_csOLASrv);
		}
	}

	return;


}

HRESULT COLAEpsilon::GetFunctionPointers()
{
	OLECHAR FAR * sMethodName[20];
	HRESULT hRes = S_OK;

	sMethodName[0] = L"AddParams";
	sMethodName[1] = L"StartSession";
	sMethodName[2] = L"EndSession";
	sMethodName[3] = L"QuerySession";
	sMethodName[4] = L"CloseBatch";
	sMethodName[5] = L"IsOnLine";
	sMethodName[6] = L"IsLocked";
	sMethodName[7] = L"GetTenderAllowedDepartments";
	sMethodName[8] = L"GetTenderNotAllowedDepartments";
	sMethodName[9] = L"GetCardInfo";
	sMethodName[10] = L"GetTenderAllowedDepartmentsEX";
	sMethodName[11] = L"GetTenderNotAllowedDepartmentsEX";
	sMethodName[12] = L"DisplayItem";
	sMethodName[13] = L"DisplayItemEx";
	sMethodName[14] = L"GetCardInfoEx";
	sMethodName[15] = L"GetLSStatus";
	sMethodName[16] = L"Login";
	sMethodName[17] = L"Logout";
	sMethodName[18] = L"AddObject";
	sMethodName[19] = L"SetTransactionPhase";

	for (int i = 0 ; i < 20; i++)
	{
		//Fill the member array MethodIDs with the
		//DISPIDs of the methods stored in the sMethodName array.
		hRes = m_pDispatch->GetIDsOfNames(IID_NULL,
										  &sMethodName[i],
										  1,
										  LOCALE_USER_DEFAULT,
										  &MethodIDs[i]);

		if (FAILED(hRes)) 
		{
			char sMsg[256];
			sprintf(sMsg, "COLAEpsilon::GetFunctionPointers() - GetIDsOfNames Failed - returned %ld", hRes);
			_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);

			hRes = OLASRV_COM_ERROR;
			break;
		}
	}

	return hRes;
}



////////////////////////////////////////////////////////////////////////////////////
//Card data retrieval and manipulation methods.
////////////////////////////////////////////////////////////////////////////////////

long COLAEpsilon::GetCardInfo(BSTR sAccountNumber, BSTR bstrTrack1, BSTR bstrTrack2, BSTR bstrShortCardName, BSTR *pRetData,long lFlags ,long *lSesionType, long lPumpNumber, PAY_AT_PUMP_INFO * pInPAPBuffer)	//4.0.20.52 63936
{
	long lRetCode = 0;
	long lSessionNumber = 0;
	unsigned int iErrArg = 0;
	CComVariant pArgs[4];
	VARIANT cRes;
	EXCEPINFO cException;

	HRESULT hRes = S_OK;

	//STDMETHOD(GetCardInfo)( /*[in]*/ BSTR AccountNumber,  /*[in]*/ BSTR Track1, /*[in]*/ BSTR Track2, /*[in]*/ BSTR ShortCardName, /*[out, retval]*/ BSTR* Value);

	pArgs[0].vt = VT_BSTR;
	pArgs[0].bstrVal = bstrShortCardName;

	pArgs[1].vt = VT_BSTR;
	pArgs[1].bstrVal = bstrTrack2;

	pArgs[2].vt = VT_BSTR;
	pArgs[2].bstrVal = bstrTrack1;

	pArgs[3].vt = VT_BSTR;
	pArgs[3].bstrVal = sAccountNumber;

	DISPPARAMS cParams = { pArgs , NULL , 4 , 0 };

	EnterCriticalSection(&m_csOLASrv);




	try 
	{
		hRes = m_pDispatch->Invoke(MethodIDs[9],
		                       IID_NULL,
						       LOCALE_SYSTEM_DEFAULT,
						       DISPATCH_METHOD,
						       &cParams,
						       &cRes,
						       &cException,
						       &iErrArg);
		
		if (FAILED(hRes))
		{
			char sMsg[256];
			sprintf(sMsg, "COLAEpsilon::GetCardInfo() - Invoke Failed - returned %ld", hRes);
			_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);
		}
	}

	catch (_com_error& e)
	{
		_LOGMSG.LogMsg("COLAEpsilon::GetCardInfo Failed - COM exception. " ,LOG_LEVEL_1);
		Error(e,hRes,TRUE);
	}

	catch (...)
	{
		_LOGMSG.LogMsg("COLAEpsilon::GetCardInfo Failed - catch (...). " ,LOG_LEVEL_1);
		Error();

	}
	
	LeaveCriticalSection(&m_csOLASrv);

	*pRetData = cRes.bstrVal;
	return lRetCode;  

}



long COLAEpsilon::GetCardInfoEx(long lClientID, BSTR sInBuffer, BSTR * bstrRetDataPtr, long lFlags)
{
	long lRetCode = 0;
	unsigned int iErrArg = 0;
	CComVariant pArgs[3];
	VARIANT cRes;
	EXCEPINFO cException;

	HRESULT hRes = S_OK;

	//HRESULT GetCardInfoEx([in] long ClientId, [in] BSTR InBuffer, [in] BSTR InfoType, [out, retval] BSTR * Value );


	BSTR bstrInfoType = SysAllocString(L" ");
	pArgs[0].vt = VT_BSTR;
	pArgs[0].bstrVal = bstrInfoType;

	pArgs[1].vt = VT_BSTR;
	pArgs[1].bstrVal = sInBuffer;

	pArgs[2].vt = VT_I4;
	pArgs[2].lVal = lClientID;

	DISPPARAMS cParams = { pArgs, NULL, 3, 0 };

	EnterCriticalSection(&m_csOLASrv);

	try
	{
		hRes = m_pDispatch->Invoke(MethodIDs[14],
			IID_NULL,
			LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD,
			&cParams,
			&cRes,
			&cException,
			&iErrArg);

		if (FAILED(hRes))
		{
			char sMsg[256];
			sprintf(sMsg, "COLAEpsilon::GetCardInfoEx() - Invoke Failed - returned %ld", hRes);
			_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);

			m_pDispatch->Release();
			m_pUnknown->Release();
			m_pDispatch = NULL;
			m_pUnknown = NULL;

			InitInterface();

			hRes = m_pDispatch->Invoke(MethodIDs[14],
				IID_NULL,
				LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD,
				&cParams,
				&cRes,
				&cException,
				&iErrArg);

			if (FAILED(hRes))
			{
				char sMsg[256];
				sprintf(sMsg, "COLAEpsilon::GetCardInfoEx() - Invoke2 Failed - returned %ld", hRes);
				_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);
			}
		}
	}

	catch (_com_error& e)
	{
		_LOGMSG.LogMsg("COLAEpsilon::GetCardInfoEx Failed - COM exception. ", LOG_LEVEL_1);
		Error(e, hRes, TRUE);
	}
	catch (...)
	{
		_LOGMSG.LogMsg("COLAEpsilon::GetCardInfoEx Failed - catch (...). ", LOG_LEVEL_1);
		Error();
	}

	LeaveCriticalSection(&m_csOLASrv);

	*bstrRetDataPtr = cRes.bstrVal;

	return lRetCode;

}

void COLAEpsilon::AddPreAuthInformation(long lPumpNumber, long &lOffset, char *pOutBuffer, long & lOLAPumpServiceMode, long & lGradeRestrict)
{
	PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);
	long lCount = cPumpItems.lCounter;
	char sFiledName[MAX_FIELD_NAME];
	long	lPriceCash=0 , lPriceCredit=0;
	long	lNumOfItems=0;//4.0.11.504	fix this function TD 14868


	// Add item list 
	if (lPumpNumber && lCount)
	{
		for (int i = 0; i < cPumpItems.lCounter ; i++)
		{	
			// send only full information...  
			if (cPumpItems.m_cItemSoldInfo[i].lGrade)
			{
				if (cPumpItems.m_cItemSoldInfo[i].bFuelItem)
				{
					sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemCode%d[S]",lNumOfItems);
					lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%03ld,",		_Module.m_server.GetGradeCode((short)cPumpItems.m_cItemSoldInfo[i].lGrade));			

					sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemPrice%d[F]",lNumOfItems);

					if (lOLAPumpServiceMode  == FULL_SERVICE_MODE)
						lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%ld.%03ld,",cPumpItems.m_cItemSoldInfo[i].lPriceCreditFullService/1000,cPumpItems.m_cItemSoldInfo[i].lPriceCreditFullService%1000);				
					else
						lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%ld.%03ld,",cPumpItems.m_cItemSoldInfo[i].lPriceCreditSelfService/1000,cPumpItems.m_cItemSoldInfo[i].lPriceCreditSelfService%1000);
					//4.0.11.502
					//Attached is the new fields that will be send to OLA in order to report low price & high price.   

					GetGradePrice((short)cPumpItems.m_cItemSoldInfo[i].lGrade ,lPriceCash , lPriceCredit ,lOLAPumpServiceMode);//4.0.11.122

					if(_Module.m_server.IsCurrentRegion(REGION_USA))	
						sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemPriceLow%d[F]",lNumOfItems);//4.0.1023.1170
					else
						sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemPriceLow%d[S]",lNumOfItems);//4.0.11.50


					lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%ld.%03ld,",lPriceCash/1000,lPriceCash%1000);//4.0.11.122
					sprintf_s(sFiledName,"ItemPriceHigh%d[S]",lNumOfItems);//4.0.11.50


					if(_Module.m_server.IsCurrentRegion(REGION_USA))	
						sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemPriceHigh%d[F]",lNumOfItems);//4.0.11.1170
					else
						sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemPriceHigh%d[S]",lNumOfItems);//4.0.11.50

					lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%ld.%03ld,",lPriceCredit/1000,lPriceCredit%1000);//4.0.11.122

					sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemIsFuel%d[B]",lNumOfItems);
					lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,(BYTE *)"%s=%s,",(BYTE *)"Y",1);

					sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemType%d[S]",lNumOfItems);

					// required only for pilot   //4.0.9999.0
					if (_Module.m_server.m_cPumpArray[lPumpNumber -1].GetDieselPumpFlags())
						lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,(BYTE *)"%s=%s,",(BYTE *)"C",1);
					else
						lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,(BYTE *)"%s=%s,",(BYTE *)"P",1);
					lNumOfItems++;  //4.0.11.504

				}
				else
				{
					PAY_AT_PUMP_INFO pap;
					_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&pap);
					long lQty = a2l(pap.CardSaleInfo.extraData.SALES[0].sQty, sizeof(pap.CardSaleInfo.extraData.SALES[0].sQty));
					if (lQty > 0)
					{
						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemIsFuel%d[B]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *)"N", 1);

						long lUnitPrice = a2l((BYTE *)pap.CardSaleInfo.extraData.SALES[0].sUnitPrice, sizeof(pap.CardSaleInfo.extraData.SALES[0].sUnitPrice)) *(_Module.m_server.GetCarWashScalarFactor());
						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPrice%d[F]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%ld.%03ld,", lUnitPrice / 1000, lUnitPrice % 1000);

						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemCode%d[S]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%03ld,", cPumpItems.m_cItemSoldInfo[i].lGrade);

						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemCodeQty%d[I]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%ld,", lQty);

						long lAmountCarwash = lUnitPrice * lQty;
						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemCodeAmt%d[F]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%ld.%03ld,", lAmountCarwash / 1000, lAmountCarwash % 1000);

						int i = 0;
						for (i = 0; i < 15; i++)
						{
							if (pap.CardSaleExtraData6.CarWashItem.sUpc[i] == 0x20)
								break;
						}
						char tmp[16] = { 0 };
						memcpy(tmp, pap.CardSaleExtraData6.CarWashItem.sUpc, i);
						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemUPC%d[S]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *)tmp, sizeof(tmp));

						lNumOfItems++;
					}
				}		
			}
		}	
		lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCount[I]",	(BYTE *)"%s=%03ld," ,lNumOfItems);
	}		
}


long COLAEpsilon::Login()
{
	char tmp[500] = {0};
	long lShiftId = _Module.m_server.GetActiveShiftNumber();
	sprintf(tmp, "TerminalID[S]=99, ShiftNumber[S]=%ld", lShiftId);
	_bstr_t InBuff(tmp);

	long lRetCode = 0;
	unsigned int iErrArg = 0;
	CComVariant pArgs[2];
	VARIANT cRes;
	EXCEPINFO cException;

	HRESULT hRes = S_OK;

	pArgs[0].vt = VT_BSTR;
	pArgs[0].bstrVal = InBuff.GetBSTR();

	DISPPARAMS cParams = { pArgs , NULL , 1 , 0 };

	EnterCriticalSection(&m_csOLASrv);
	hRes = m_pDispatch->Invoke(MethodIDs[16],
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
		char sMsg[256];
		sprintf(sMsg, "COLAEpsilon::Login() - Invoke Failed - returned %ld", hRes);
		_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);

		lRetCode = OLASRV_COM_ERROR;
	}

	LeaveCriticalSection(&m_csOLASrv);

	return lRetCode;
}

long COLAEpsilon::Logout()
{
	_bstr_t InBuff("TerminalID[S]=99");

	long lRetCode = 0;
	unsigned int iErrArg = 0;
	CComVariant pArgs[2];
	VARIANT cRes;
	EXCEPINFO cException;

	HRESULT hRes = S_OK;

	pArgs[0].vt = VT_BSTR;
	pArgs[0].bstrVal = InBuff.GetBSTR();

	DISPPARAMS cParams = { pArgs , NULL , 1 , 0 };

	EnterCriticalSection(&m_csOLASrv);
	hRes = m_pDispatch->Invoke(MethodIDs[17],
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
		char sMsg[256];
		sprintf(sMsg, "COLAEpsilon::Logout() - Invoke Failed - returned %ld", hRes);
		_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);

		lRetCode = OLASRV_COM_ERROR;
	}

	LeaveCriticalSection(&m_csOLASrv);

	return lRetCode;
}


void COLAEpsilon::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE */)
{
	EnterCriticalSection(&m_csOLASrv);

	if(m_bValid)
	{
		m_bValid = FALSE;//4.0.3.45
		try
		{
			//Logout();
			m_pDispatch->Release();
			m_pUnknown->Release();

			m_pDispatch = NULL;
			m_pUnknown = NULL;
		}

		catch(_com_error &cError)
		{
			//4.0.1.32
			CatchCOMError(cError);
		}

		catch (...)
		{
			_LOGMSG.LogMsg("COLAEpsilon::ReleaseService() : the call to Release failed.", LOG_LEVEL_0);
		}
	
		if (bCloseThread)
			_Module.m_server.CloseOLAThread();  	
		else
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_WAIT_FOR_KILL;

		//m_bValid = FALSE;
	}
		
	LeaveCriticalSection(&m_csOLASrv);
}

long COLAEpsilon::GetCustomerInfo(BSTR sInBuffer, BSTR *pRetData)	//4.0.20.52 63936
{
	long lRetCode = 0;
	long lSessionNumber = 0;
	unsigned int iErrArg = 0;
	CComVariant pArgs[4];
	VARIANT cRes;
	EXCEPINFO cException;

	HRESULT hRes = S_OK;
	BSTR bstrInfoType = SysAllocString(L" ");
	pArgs[0].vt = VT_BSTR;
	pArgs[0].bstrVal = bstrInfoType;

	pArgs[1].vt = VT_BSTR;
	pArgs[1].bstrVal = sInBuffer;

	DISPPARAMS cParams = { pArgs, NULL, 3, 0 };

	EnterCriticalSection(&m_csOLASrv);

	try
	{
		hRes = m_pDispatch->Invoke(MethodIDs[20],
			IID_NULL,
			LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD,
			&cParams,
			&cRes,
			&cException,
			&iErrArg);


		if (FAILED(hRes))
		{
			char sMsg[256];
			sprintf(sMsg, "COLAEpsilon::GetCustomerInfo() - Invoke Failed - returned %ld", hRes);
			_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);
		}
	}
	catch (_com_error& e)
	{
		_LOGMSG.LogMsg("COLAEpsilon::GetCustomerInfo Failed - COM exception. ", LOG_LEVEL_1);
		Error(e, hRes, TRUE);
	}
	catch (...)
	{
		_LOGMSG.LogMsg("COLAEpsilon::GetCustomerInfo Failed - catch (...). ", LOG_LEVEL_1);
		Error();
	}

	LeaveCriticalSection(&m_csOLASrv);

	*pRetData = cRes.bstrVal;

	return lRetCode;
}

void COLAEpsilon::BuildSetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, CPumpTransact *pTrs, char *pOutBuffer)
{
	PAY_AT_PUMP_INFO *pInfo;
	CARD_SALE_DATA			*pCardData;
	CARD_SALE_EXTRA_DATA	*pCardDataExtra;
	CARD_SALE_EXTRA_DATA2_  *pCardDataExtra2;
	CARD_SALE_EXTRA_DATA3_  *pCardDataExtra3;
	CARD_SALE_EXTRA_DATA4_	*pCardDataExtra4;
	CARD_SALE_EXTRA_DATA5_	*pCardDataExtra5;
	CARD_SALE_EXTRA_DATA6_	*pCardDataExtra6;
	SYSTEMTIME systime;

	long lValue = 0, lVolume = 0, lTransactionId = 0, lItemCount = 0, lGradePrice = 0;
	short nGrade = 0;

	if (pTrs)
	{
		lValue = pTrs->m_lRoundedValue;
		lVolume = pTrs->m_lRoundedVolume;
		lTransactionId = pTrs->m_lNumber;
		nGrade = pTrs->m_nGrade;
		lGradePrice = pTrs->m_lGradePrice;
	}

	long lOffset = 0;
	pInfo = (PAY_AT_PUMP_INFO *)pInBuffer;

	pCardData = &(pInfo->CardSaleInfo.cardData);
	pCardDataExtra = &(pInfo->CardSaleInfo.extraData);
	pCardDataExtra2 = &(pInfo->CardSaleExtraData2);
	pCardDataExtra3 = &(pInfo->CardSaleExtraData3);
	pCardDataExtra4 = &(pInfo->CardSaleExtraData4);
	pCardDataExtra5 = &(pInfo->CardSaleExtraData5);
	pCardDataExtra6 = &(pInfo->CardSaleExtraData6);
	long lGradePos = _Module.m_server.GetGradePositionRestrection(nGrade, lPumpNumber);

	if (lGradePos > 0)
		lGradePos = lGradePos - 1;

	long lGasLoyaltyDiscountPerGallon = a2l(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount, sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount));

	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransVolume[F]", (BYTE *)"%s=%ld.%03ld,", lVolume / 1000, lVolume % 1000);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"OriginTrans[S]", (BYTE *)"%s=%.1s,", &(pCardDataExtra2->sOriginTrans), sizeof(pCardDataExtra2->sOriginTrans));
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"PumpNumber[S]", (BYTE *)"%s=%ld,", lPumpNumber);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"StoreNumber[S]", (BYTE *)"%s=%.4ld,", atol(_Module.m_server.GetSiteID()));
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransAmount[F]", (BYTE *)"%s=%ld.%03ld,", lValue / 1000, lValue % 1000);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"FuelTrs[I]", (BYTE *)"%s=%ld,", lTransactionId);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"GasLoyaltyDiscountPerGallon[F]", (BYTE *)"%s=%ld.%03ld,", lGasLoyaltyDiscountPerGallon / 1000, lGasLoyaltyDiscountPerGallon % 1000);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TerminalId[S]", (BYTE *)"%s=%04ld,", (long)SERVER_OLA_POS_NUMBER);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ClubcardNumber[S]", (BYTE *)"%s=%.20s,", pCardDataExtra->sLoyalId, sizeof(pCardDataExtra->sLoyalId));
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"EntryMethod[S]", (BYTE *)"%s=%.1s,", &(pCardData->sEntryMethod), sizeof(pCardData->sEntryMethod));
	//lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"MediaType[S]",					(BYTE *)"%s=%.1s,", &(pCardData->sTranType), sizeof(pCardData->sTranType));
	
	//RFUEL-672
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"HostTranId[S]", (BYTE *)"%s=%.20s,", pInfo->CardSaleExtraData6.sLoylatyRefHostTranId, sizeof(pInfo->CardSaleExtraData6.sLoylatyRefHostTranId));

	if (_Module.m_server.m_bWaitForFinalizeResponse == 1)//RFUEL-2117
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"WaitForFinalizeResponse[B]", (BYTE *)"%s=%s,", (BYTE *)"Y", 1);

	long lSeqNumber = a2l(pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr));

	if (lSeqNumber > 0)
	{
		lOffset += SetParamNoCheck(pOutBuffer + lOffset, (BYTE *)"TransNumber[I]", (BYTE *)"%s=%.5s,", pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr));
	}
	else
	{
		// Punchh
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransNumber[I]", (BYTE *)"%s=%ld,", lTransactionId);
	}

	// QC 486714
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"GRDiscountPerGallon[F]", (BYTE *)"%s=%ld.%03ld,", lGasLoyaltyDiscountPerGallon / 1000, lGasLoyaltyDiscountPerGallon % 1000);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"OldSessID[I]", (BYTE *)"%s=%.7s,", pCardDataExtra3->sLoyaltySessionID, sizeof(pCardDataExtra3->sLoyaltySessionID));

	// 	if (_Module.m_server.m_dwLoyaltySendAllTrsToLoyalty)  //501164
	if (_Module.m_server.m_dwLoyaltyClubNumber == CLUB_TYPE_PUNCHH)
	{
		if (((_Module.m_server.m_dwLoyaltySendAllTrsToLoyalty & _eSTTFP_NoneLoyalty) &&
			(ChrAll((char*)pCardDataExtra->sLoyalId, sizeof(pCardDataExtra->sLoyalId), ' '))) ||  //RFUEL-72  // None Loyalty
			(_Module.m_server.m_dwLoyaltySendAllTrsToLoyalty  & _eSTTFP_OtherLoyalty) &&
			(pCardDataExtra4->sLoyaltyEntryMethod != 'A'))											//Other loyalty 

		{
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"SendToPES[B]", (BYTE *)"%s=%.1c,", 'Y');

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("BuildSetCustomerInfoRequest() - SetPunch transaction to true SendToPES = Y");
				_LOGMSG.LogMsg("COLAEpsilon", "BuildSetCustomerInfoRequest", lPumpNumber, LOG_PUMP, str); //4.0.23.31 75234
			}
		}

		if ((pCardDataExtra4->sLoyaltyEntryMethod == 'A') ||
			(!ChrAll((char*)pCardDataExtra->sLoyalId, sizeof(pCardDataExtra->sLoyalId), ' ')))

		{
			//lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"SendToPES[B]", (BYTE *)"%s=%.1c,", 'Y');
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"LoyaltyEntryMethod[S]", (BYTE *)"%s=%.1s,", &(pInfo->CardSaleExtraData4.sLoyaltyEntryMethod), sizeof(pInfo->CardSaleExtraData4.sLoyaltyEntryMethod));

			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemPrice0[F]", (BYTE *)"%s=%ld.%03ld,", lGradePrice / 1000, lGradePrice % 1000);

			if (nGrade)
			{
				TStaticString<STATIC_STRING_64> sGradeFullName;

				_Module.m_server.m_GradeData.GetGradeName(nGrade, sGradeFullName);
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemName0[S]", (BYTE *)"%s=%s,", (BYTE *)((LPCSTR)sGradeFullName), sGradeFullName.Size());
			}
		}
	}

	// number of tenders - most likely just 1
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TenderCount[I]", (BYTE *)"%s=%ld,", 1);

	// C - Credit, D - Debit, etc.
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TenderType0[S]", (BYTE *)"%s=%.1s,", &(pInfo->CardSaleInfo.cardData.sTranType), sizeof(pInfo->CardSaleInfo.cardData.sTranType));

	if (!ChrAll((char *)pInfo->CardSaleInfo.cardData.sCardName, sizeof(pInfo->CardSaleInfo.cardData.sCardName)))
	{
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TenderSubType0[S]", (BYTE *)"%s=%.8s,", pInfo->CardSaleInfo.cardData.sCardName, sizeof(pInfo->CardSaleInfo.cardData.sCardName));
	}

	// amount tendered
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TenderAmount0[F]", (BYTE *)"%s=%ld.%03ld,", lValue / 1000, lValue % 1000);

	//lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"GradeDiscountId[S]", (BYTE *)"%s=%.12s,", pInfo->CardSaleExtraData6.sGradeDiscountID[lGradePos], sizeof(pInfo->CardSaleExtraData6.sGradeDiscountID[lGradePos]));
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"PromotionId[S]", (BYTE *)"%s=%.15s,", pInfo->CardSaleExtraData6.sGradeDiscountID[lGradePos], sizeof(pInfo->CardSaleExtraData6.sGradeDiscountID[lGradePos]));
	lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"UnitPrice[F]",		(BYTE *)"%s=%ld.%03ld,",	lGradePrice/1000 ,lGradePrice%1000);

	// RFUEL-289
	BYTE sIncludeDiscountFlags;

	sIncludeDiscountFlags =  ((unsigned char) (pCardDataExtra3->sUnitPriceIncludeDiscount == '1')) ? 'Y' : 'N';
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"UnitPriceIncludeDiscount[B]", (BYTE *) "%s=%.1s,", &sIncludeDiscountFlags, sizeof(pCardDataExtra3->sUnitPriceIncludeDiscount));
	
	if ((lVolume == 0) && (lValue == 0))
	{
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("BuildSetCustomerInfoRequest() - No fuel was taken. lItemCount = %ld", lItemCount);
			_LOGMSG.LogMsg("COLAEpsilon", "BuildSetCustomerInfoRequest", lPumpNumber, LOG_PUMP, str); //4.0.23.31 75234
		}
	}
	else
	{
		lItemCount = 1L;//There at list one item - fuel

		if (a2l(&pInfo->CardSaleInfo.extraData.sNumberOfExtraSale, sizeof(pInfo->CardSaleInfo.extraData.sNumberOfExtraSale)))
		{
			//We got car wash
			lItemCount++;
			//Check if also got discount
			if (a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sDiscount, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDiscount)))
			{
				lItemCount++;
			}
		}

		/////////////////////////////////////////////////////////////

		if (lItemCount > 1) 	//Car wash			
		{
			DWORD dwCode;
			DWORD dwCarWashLoyalApp; //3.1.1.1
			_Module.m_server.m_cParam.LoadParam((char *)"CarWash", "LoyaltyApplicable", &dwCarWashLoyalApp, 1L, FALSE); //3.1.1.1
			_Module.m_server.m_cParam.LoadParam((char *)"CarWash", "CarWashCode", &dwCode, 102L, FALSE);//102 - NACS code for car wash
			long lAmount = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice)) * 10;
			long lQty = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sQty, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sQty));
			long lDiscount = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sDiscount, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDiscount)) * 10;
			long lTax = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[2].sTax, sizeof(pInfo->CardSaleInfo.extraData.SALES[2].sTax)) * 10;        //3.2.0.9	
			long lFuelSale = lValue - lAmount + lDiscount;

			if (_Module.m_server.GetReduceCWDiscountFromCWAmount() && (lItemCount == 3))  //4.0.23.2583
			{
				CString str;
				str.Format("ReduceCWDiscountFromCWAmount is TRUE  lAmount = %ld, lDiscount = %ld , lItemCount = %ld", lAmount, lDiscount, lItemCount);
				_Module.m_server.m_cLogger.LogMsg("COLAEpsilon", "BuildSetCustomerInfoRequest", 0, LOG_NONE, str);
				lAmount = lAmount - lDiscount;
				--lItemCount;
			}

			//Fuel is alway there, and it always the first.
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCode0[S]", (BYTE *)"%s=%03ld,", _Module.m_server.GetGradeCode(nGrade));
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeAmt0[F]", (BYTE *)"%s=%ld.%03ld,", lFuelSale / 1000, lFuelSale % 1000);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeQty0[I]", (BYTE *)"%s=%ld,", 1L);//This value will be a transaction parameter as we sell items at the Pump.
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemWeight0[F]", (BYTE *)"%s=%ld.%03ld,", lVolume / 1000, lVolume % 1000);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemIsFuel0[B]", (BYTE *)"%s=%s,", (BYTE *)"Y", 1);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemTax0[F]", (BYTE *)"%s=%ld.%03ld,", lTax / 1000, lTax % 1000);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemType0[S]", (BYTE *)"%s=%s,", (BYTE *)"P", 1); //4.0.9999.0


																												 //Car Wash sale
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCode1[S]", (BYTE *)"%s=%03ld,", dwCode);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeAmt1[F]", (BYTE *)"%s=%ld.%03ld,", lAmount / 1000, lAmount % 1000);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeQty1[I]", (BYTE *)"%s=%ld,", lQty);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemWeight1[F]", (BYTE *)"%s=%ld.%03ld,", 1, 0);//always 1.000
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemIsFuel1[B]", (BYTE *)"%s=%s,", (BYTE *)"N", 1);

			lTax = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sTax, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sTax)) * 10;        	//3.2.0.69				 
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemTax1[F]", (BYTE *)"%s=%ld.%03ld,", lTax / 1000, lTax % 1000);

			long lCarWashProg = a2l(&pInfo->CardSaleInfo.extraData.sCarWashProgram, sizeof(pInfo->CardSaleInfo.extraData.sCarWashProgram));

			if (lCarWashProg)
			{
				TStaticString<STATIC_STRING_64> sCarWashProg;
				CString strCarWash;
				sCarWashProg.Clear();
				strCarWash.Format("WashPrograms\\WashProgram%02d", lCarWashProg);
				_Module.m_server.m_cParam.LoadParam((LPCSTR)strCarWash, "FullName", (char *)sCarWashProg, sCarWashProg.Size(), "", FALSE);
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemName1[F]", (BYTE *)"%s=%s,", (BYTE *)((LPCSTR)sCarWashProg), sCarWashProg.Size());
			}

			//////////////////////////////////////////////////////////////

		}
		else
		{
			long lTax = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[2].sTax, sizeof(pInfo->CardSaleInfo.extraData.SALES[2].sTax)) * 10;        //3.2.0.9					
			long lVatRate = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.sVAT_Rate, sizeof(pInfo->CardSaleInfo.extraData.sVAT_Rate));		//4.0.15.504				
																																		//Fuel is alway there, and it always the first.
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCode0[S]", (BYTE *)"%s=%03ld,", _Module.m_server.GetGradeCode(nGrade));
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeAmt0[F]", (BYTE *)"%s=%ld.%03ld,", lValue / 1000, lValue % 1000);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeQty0[I]", (BYTE *)"%s=%ld,", 1L);//This value will be a transaction parameter as we sell items at the Pump.
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemWeight0[F]", (BYTE *)"%s=%ld.%03ld,", lVolume / 1000, lVolume % 1000);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemIsFuel0[B]", (BYTE *)"%s=%s,", (BYTE *)"Y", 1);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemTax0[F]", (BYTE *)"%s=%ld.%03ld,", lTax / 1000, lTax % 1000);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemTaxRate0[F]", (BYTE *)"%s=%ld.%02ld,", lVatRate / 100, lVatRate % 100); //4.0.15.504
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE*)"ItemPrice0[F]", (BYTE*)"%s=%ld.%03ld,", lGradePrice / 1000, lGradePrice % 1000);
		}

		//Number of item
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCount[I]", (BYTE *)"%s=%ld,", lItemCount);   //4.0.23.2582

		{
			CString str;
			str.Format("COLAEpsilon::BuildSetCustomerInfoRequest() - Pump %02ld, lTransNumber = %ld, lSeqNumber %ld, sTranType = %c, ClubCardNumber = %.20s, EntryMethod = %c, OldSessID = %.6s",
				lPumpNumber,
				lTransactionId,
				lSeqNumber,
				pCardData->sTranType,
				pCardDataExtra->sLoyalId,
				pCardData->sEntryMethod,
				pCardDataExtra3->sLoyaltySessionID);
			_LOGMSG.LogMsg(str);
		}

		if (!(pInfo->CardSaleInfo.extraData.sPrnDateTime[3] == ' ') &&
			!(pInfo->CardSaleInfo.extraData.sPrnDateTime[3] == 0))
		{
			//4.0.2.33
			char sYear[5] = { 0 };
			char sSeconds[2] = { 0 }; //4.0.23.120 87260
			BYTE sTmpDate[8];
			char sTmpTime[6];

			memset(&sTmpDate, 0, sizeof(sTmpDate));
			memset(&sTmpTime, '0', sizeof(sTmpTime));
			memset(sYear, 0, sizeof(sYear));

			//If the year is bigger then 70 then it 19XX else it 20XX
			GetLocalTime(&systime);  //4.0.2.32
			if (systime.wYear >= 2000)
			{
				sprintf(sYear, "20%.2s", pInfo->CardSaleInfo.extraData.sPrnDateTime);
			}
			else
			{
				sprintf(sYear, "19%.2s", pInfo->CardSaleInfo.extraData.sPrnDateTime);
			}
			memcpy(&sTmpDate[4], sYear, 4); //4.0.2.32
			memcpy(sTmpDate, &pInfo->CardSaleInfo.extraData.sPrnDateTime[2], 4);

			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransactionDate[D]", (BYTE *)"%s=%.8s,", (BYTE *)sTmpDate, sizeof(sTmpDate));
			//hhmmss
			sprintf(sSeconds, "%02d", systime.wSecond); //4.0.23.120 87260
			memcpy(sTmpTime, &pInfo->CardSaleInfo.extraData.sPrnDateTime[6], 4);

			memcpy(&sTmpTime[4], sSeconds, 2); //4.0.23.120 87260

			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransactionTime[T]", (BYTE *)"%s=%.6s,", (BYTE *)sTmpTime, sizeof(sTmpTime));
		}
	}
}

void COLAEpsilon::BuildGetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, char *pOutBuffer)
{
	long lTransNumber = 0;
	PAY_AT_PUMP_INFO *pInfo;
	CARD_SALE_DATA			*pCardData;
	CARD_SALE_EXTRA_DATA	*pCardDataExtra;
	CARD_SALE_EXTRA_DATA2_  *pCardDataExtra2;
	CARD_SALE_EXTRA_DATA3_  *pCardDataExtra3;
	CARD_SALE_EXTRA_DATA4_	*pCardDataExtra4; //4.0.16.504
	CARD_SALE_EXTRA_DATA5_	*pCardDataExtra5; //4.0.18.501
	CARD_SALE_EXTRA_DATA6_	*pCardDataExtra6;

	long lOffset = 0;
	pInfo = (PAY_AT_PUMP_INFO *)pInBuffer;

	pCardData = &(pInfo->CardSaleInfo.cardData);
	pCardDataExtra = &(pInfo->CardSaleInfo.extraData);
	pCardDataExtra2 = &(pInfo->CardSaleExtraData2);
	pCardDataExtra3 = &(pInfo->CardSaleExtraData3);
	pCardDataExtra4 = &(pInfo->CardSaleExtraData4);	//4.0.16.504
	pCardDataExtra5 = &(pInfo->CardSaleExtraData5);	//4.0.18.501
	pCardDataExtra6 = &(pInfo->CardSaleExtraData6);

	long lOLAPumpServiceMode = 0;
	CPumpStatus cPumpStatus;
	if (!PUMP_NOT_VALID(lPumpNumber))
	{
		long lServiceMode = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetServiceMode();
		cPumpStatus = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();

		CPumpTransact trs;
		if (!_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetInProgressWaitProgressTrs(&trs))
		{
			lTransNumber = trs.m_lNumber;

			{
				CString str;
				str.Format("COLAEpsilon::BuildGetCustomerInfoRequest() - Pump %02ld - lTransNumber = %ld", lPumpNumber, lTransNumber);
				_LOGMSG.LogMsg(str);
			}
		}

		switch (lServiceMode)
		{
		case FULL_SERVICE_MODE:
			lOLAPumpServiceMode = 1;
			break;
		case SELF_SERVICE_MODE:
			lOLAPumpServiceMode = 2;
			break;
		}
	}

	if (pCardData->sEntryMethod == 'B')  //RFUEL-2399
	{
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"AccountNumber[S]", (BYTE *)"%s=%.21s,", pCardData->sAccountNumber, sizeof(pCardData->sAccountNumber));
	}
	else
	{
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"Track2[S]", (BYTE *)"%s=%.37s,", pCardData->sClubCardTrack2Data, sizeof(pCardData->sClubCardTrack2Data));
	}

	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"DeviceNumber[S]", (BYTE *)"%s=%ld,", lPumpNumber);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"EntryMethod[S]", (BYTE *)"%s=%.1s,", &(pCardData->sEntryMethod), sizeof(pCardData->sEntryMethod));
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"StoreNumber[S]", (BYTE *)"%s=%.4ld,", atol(_Module.m_server.GetSiteID())); //3.2.0.69
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TerminalId[S]", (BYTE *)"%s=%04ld,", (long)SERVER_OLA_POS_NUMBER);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"OriginTrans[S]", (BYTE *)"%s=%.1s,", &(pCardDataExtra2->sOriginTrans), sizeof(pCardDataExtra2->sOriginTrans));

	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"LoyaltyEntryMethod[S]", (BYTE *)"%s=%.1s,", &(pInfo->CardSaleExtraData4.sLoyaltyEntryMethod), sizeof(pInfo->CardSaleExtraData4.sLoyaltyEntryMethod));

	long lSeqNumber = a2l(pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr));

	if (lSeqNumber > 0)
	{
		lOffset += SetParamNoCheck(pOutBuffer + lOffset, (BYTE *)"TransNumber[I]", (BYTE *)"%s=%.5s,", pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr));
	}
	else
	{
		// Punchh
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransNumber[I]", (BYTE *)"%s=%ld,", lTransNumber);
	}

	// 494652 lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"MediaType[S]", (BYTE *)"%s=%.1s,", &(pCardData->sTranType), sizeof(pCardData->sTranType));
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"MediaType[S]",	(BYTE *)"%s=%.1c,", 'C');
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ClubCardNumber[S]", (BYTE *)"%s=%.20s,", pCardDataExtra->sLoyalId, sizeof(pCardDataExtra->sLoyalId));

	// 4.0.7.500
	long lProdCode = a2l(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode));

	AddedPreAuthInformation(lPumpNumber, lOffset, pOutBuffer, lOLAPumpServiceMode, lProdCode);

	{
		const int iEncrptSize = sizeof pCardDataExtra->sLoyalId - 4; // //RFUEL-1367
		CString str;
		str.Format("COLAEpsilon::BuildGetCustomerInfoRequest() - Pump %02ld - lTransNumber = %ld, lSeqNumber %ld, sTranType = %c, ClubCardNumber = XXXXXXXXXXXXXXXX%.4s, EntryMethod = %c",
			lPumpNumber,
			lTransNumber,
			lSeqNumber,
			pCardData->sTranType,
			pCardDataExtra->sLoyalId + iEncrptSize,
			pCardData->sEntryMethod);
		_LOGMSG.LogMsg(str);
	}
}


void COLAEpsilon::BuildEpsilonFields(long lPumpIndex, long lTotalAmount, long &lOffset, char *pOutBuffer, PAY_AT_PUMP_INFO *pPAPInfo)
{
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TenderCount[I]", (BYTE *)"%s=%ld,", 1);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransAmount0[F]", (BYTE *)"%s=%ld.%03ld,", lTotalAmount / 1000, lTotalAmount % 1000);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ReferenceNumber0[S]", (BYTE *)"%s=%.12s,", pPAPInfo->CardSaleInfo.cardData.sReferenceNumber, sizeof(pPAPInfo->CardSaleInfo.cardData.sReferenceNumber));
}

/******************************************************************************
Description:	This procedure matches the sent loyalty card info as was defined
by OLA RouteSrv to the current OLA. Only for Loyalty flow.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Liat			15/11/2009   12:12		Start  4.0.23.121 89548 //4.0.24.60 109616
******************************************************************************/
void COLAEpsilon::MatchSentLoyaltyCardInfoToOLA(PAY_AT_PUMP_INFO &pInOutPAPBuffer)
{
	switch (pInOutPAPBuffer.CardSaleInfo.cardData.sEntryMethod)
	{
		case 'S':
		{
			memcpy(pInOutPAPBuffer.CardSaleInfo.cardData.sTrack2Data, pInOutPAPBuffer.CardSaleInfo.cardData.sClubCardTrack2Data, min(sizeof(pInOutPAPBuffer.CardSaleInfo.cardData.sTrack2Data), sizeof(pInOutPAPBuffer.CardSaleInfo.cardData.sClubCardTrack2Data)));
			break;
		}
		case 'M':
		{//4.0.23.240 109616 //4.0.24.60
		 //memcpy(pInOutPAPBuffer.CardSaleInfo.cardData.sTrack2Data, pInOutPAPBuffer.CardSaleInfo.extraData.sLoyalId, min(sizeof(pInOutPAPBuffer.CardSaleInfo.cardData.sTrack2Data), sizeof(pInOutPAPBuffer.CardSaleInfo.extraData.sLoyalId)));
			break;
		}
		case 'B':
		{
			memcpy(pInOutPAPBuffer.CardSaleInfo.cardData.sTrack2Data, pInOutPAPBuffer.CardSaleExtraData2.sBarCode, min(sizeof(pInOutPAPBuffer.CardSaleInfo.cardData.sTrack2Data), sizeof(pInOutPAPBuffer.CardSaleExtraData2.sBarCode)));
			break;
		}
	}

	if (LOG_BASIC_FLOW_CONTROL)
	{
		_LOGMSG.LogMsg("Loyalty card Info was matched to OLA before GetCardInfo");
	}
}
