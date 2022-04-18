// OlaTravelCenter.cpp: implementation of the COLATravelCenter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLATravelCenter.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLATravelCenter::COLATravelCenter(): m_pUnknown(0) , m_pDispatch(0)
{
	SetIFSFProtocoll(TRUE);
	SetPreAuthIFSF(TRUE);	
	SetEndSessionEx(TRUE);

	
}

COLATravelCenter::~COLATravelCenter()
{

}

long COLATravelCenter::SessionPriceChange(char *pInStr , long * lOffset)
{
	long lPriceCash , lPriceCredit ;
	long lGradeNacs;
	long lCounter = 0 ;
	long lServiceMode = FULL_SERVICE_MODE;
	char sTmpBuffer[50];
	
	for ( long i = 0 ;i < MAX_GRADES ; i++)
	{
		if (!GetGradePrice(i + 1 ,lPriceCash , lPriceCredit ,lServiceMode))
		{
				
			lGradeNacs = _Module.m_server.GetGradeCode(i+ 1);

			sprintf(sTmpBuffer ,"ItemCode%ld[S]" , lCounter);			
			*lOffset += SetParam(pInStr+ *lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%ld",	lGradeNacs);
			
			sprintf(sTmpBuffer ,"ItemPriceLow%ld[S]" , lCounter);
			*lOffset += SetParam(pInStr+*lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%ld.%03ld,",	lPriceCash/1000,lPriceCash%1000);			

			sprintf(sTmpBuffer ,"ItemPriceHigh%ld[S]" , lCounter);
			*lOffset += SetParam(pInStr+*lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%ld.%03ld,",	lPriceCredit/1000,lPriceCredit%1000);		

			sprintf(sTmpBuffer ,"ItemPriceType%ld[S]" , lCounter);
			*lOffset += SetParam(pInStr+*lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%.1s,",	'F');

			lCounter++	;	
			lServiceMode =SELF_SERVICE_MODE;
			GetGradePrice(i + 1 ,lPriceCash , lPriceCredit ,lServiceMode);

			*lOffset += SetParam(pInStr+*lOffset,(BYTE *)"ItemCodeX[S]",	(BYTE *)"%s=%ld",	lGradeNacs);			
			*lOffset += SetParam(pInStr+*lOffset,(BYTE *)"ItemPriceLowX[F]",	(BYTE *)"%s=%ld.%03ld,",	lPriceCash/1000,lPriceCash%1000);			
			*lOffset += SetParam(pInStr+*lOffset,(BYTE *)"ItemPriceHighX[F]",	(BYTE *)"%s=%ld.%03ld,",	lPriceCredit/1000,lPriceCredit%1000);		
			*lOffset += SetParam(pInStr+*lOffset,(BYTE *)"ItemPriceTypeX[S]",	(BYTE *)"%s=%.1s,",	'S');
			lCounter++	;	
		}
	}

	*lOffset += SetParam(pInStr+*lOffset,(BYTE *)"ItemCount[I]",		(BYTE *)"%s=%ld,",		lCounter);
	return 0;
}

void COLATravelCenter::Init()
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
					CString sMsg("COLATravelCenter Failed to initialize - general error");
					_LOGMSG.LogMsg(sMsg , LOG_LEVEL_0);
				}
			}

			//4.0.2.37 start
			if(bInitializationSuccessfull)
			{
				long lPumpNumber=0;
				m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber);
				LoadRegistryParams();
				LaunchOLA();

				ResetRetryAttemps();
			}

			else
			{
				_LOGMSG.LogMsg("COLATravelCenter::Init() - failed to initialize" , LOG_LEVEL_0); //4.0.9999.0
				DecrementConnectionAttempts();
			}
			//4.0.2.37 end

			LeaveCriticalSection(&m_csOLASrv);
		}
	}

	return;

}

long COLATravelCenter::AddParams(long lSessId, char *pInBuffer)
{
	long lRtc = 0;
	long lRetCode = 0;
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
	hRes = m_pDispatch->Invoke(MethodIDs[4],
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
		_LOGMSG.LogMsg("COLATravelCenter::EndSession() - Invoke has failed");
		Error();   //4.0.10.506
		lRetCode = OLASRV_COM_ERROR;

	}



	LeaveCriticalSection(&m_csOLASrv);

	return lRtc;
}

long COLATravelCenter::EndSession(long lSessId)
{
	long lRetCode = 0;
	unsigned int iErrArg = 0;
	CComVariant pArgs[1];
	VARIANT cRes;
	EXCEPINFO cException;

	HRESULT hRes = S_OK;

	pArgs[0].vt = VT_I4;
	pArgs[0].lVal = lSessId;

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
		_LOGMSG.LogMsg("COLATravelCenter::EndSession() - Invoke has failed");
		Error();   //4.0.10.506
		lRetCode = OLASRV_COM_ERROR;

	}


	LeaveCriticalSection(&m_csOLASrv);

	return lRetCode;
}

long COLATravelCenter::QuerySession(long lSessionNumber, BSTR *pOutBuffer)
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
		lRetCode = cRes.lVal;
	}


	else
	{
		_LOGMSG.LogMsg("QuerySession::QuerySession, Invoke has failed");
		Error();   //4.0.10.506
		lRetCode = OLASRV_COM_ERROR;

	}

		

	LeaveCriticalSection(&m_csOLASrv);

	return lRetCode;

}

void COLATravelCenter::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE */)
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
			CString sMsg("COLATravelCenter::ReleaseService() : the call to Release failed.");
			_LOGMSG.LogMsg(sMsg , LOG_LEVEL_0);
		}
	
		if (bCloseThread)
			_Module.m_server.CloseOLAThread();  	
		else
			_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_WAIT_FOR_KILL;

		//m_bValid = FALSE;
	}
		
	LeaveCriticalSection(&m_csOLASrv);
}

long COLATravelCenter::EndSessionEx(long lSessID ,long lSessionResult)	
{
	long lRetCode = 0;
	unsigned int iErrArg = 0;
	CComVariant pArgs[2];
	VARIANT cRes;
	EXCEPINFO cException;

	HRESULT hRes = S_OK;

	pArgs[0].vt = VT_I4;
	pArgs[0].lVal = lSessionResult;

	pArgs[1].vt = VT_I4;
	pArgs[1].lVal = lSessID;

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
		_LOGMSG.LogMsg("EndSessionEx::EndSession() - Invoke has failed");

	LeaveCriticalSection(&m_csOLASrv);

	return lRetCode;
}

HRESULT COLATravelCenter::GetFunctionPointers()
{
	OLECHAR FAR * sMethodName[MOSAIC_NUMBER_OF_IMPLEMENTED_METHODS];
	HRESULT hRes = S_OK;

	sMethodName[0] = L"StartSession";
	sMethodName[1] = L"QuerySession";
	sMethodName[2] = L"EndSession";
	sMethodName[3] = L"EndSessionEx";
	sMethodName[4] = L"AddParams";
	

	for (int i=0 ; i < 5 ; i++)
	{
		//Fill the member array MethodIDs with the
		//DISPIDs of the methods stored in the sMethodName array.
		hRes = m_pDispatch->GetIDsOfNames(IID_NULL,
										  &sMethodName[i],
										  1,
										  LOCALE_USER_DEFAULT,
										  &MethodIDs[i]);
		if(FAILED(hRes))
			break;
	}

	return hRes;
}

BOOL COLATravelCenter::InitInterface()
{
	BOOL bRetCode = FALSE;
	HRESULT hRes = S_OK;
	CLSID clsid;

	//Get the CLSID for the IServices interface.
	hRes = CLSIDFromProgID(L"MosaicComSrv.Services.1" , &clsid);

	if(SUCCEEDED(hRes))
	{
		hRes = CoCreateInstance(clsid,
			                    NULL,
								CLSCTX_ALL,
								IID_IUnknown,
								(void FAR * FAR *)&m_pUnknown);
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
		_LOGMSG.LogMsg("COLATravelCenter::InitInterface(), failed to acquire interface" , LOG_LEVEL_0);
		
	return bRetCode;
}

long COLATravelCenter::StartSession(char *pInBuffer, char *pszType)
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
		lSessionNumber = cRes.lVal;
	}
	else
	{
		Error();  //4.0.10.49
		lSessionNumber = OLASRV_COM_ERROR;
		_LOGMSG.LogMsg("COLATravelCenter::StartSession, Invoke has failed");

	}

	LeaveCriticalSection(&m_csOLASrv);

	return lSessionNumber;
}
//4.0.9999.0

void COLATravelCenter::ParseItemProduct(long &lPumpNumber, CString *pInStr, PAY_AT_PUMP_INFO *pInfo ,CPumpTransact *pTrs) //4.0.9999.0
{
	CString  sMsg;
	PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);
	long	lNACSCode;
	long	lIndex;
	BOOL	bFound= FALSE;
	int		iLen ;
	char	*pParam;
	short	nItemCounter = 0;
	char	sFiledName[MAX_FIELD_NAME]; 
	long	lGradeRestriction = 0;
	long	lCarWashRestriction = 0;
	long    lVal=0;
	BYTE	byNozzle = 0;
	BYTE	byNozzleRestriction=0;


//4.0.10.14

	pInfo->CardSaleExtraData3.sPositionRestrictionActive = '1';  //4.0.10.49

	bFound = GetParam(pInStr,pInStr->Find("ItemRestriction[I]"),&pParam,&iLen);

	if(bFound) 
	{
		memset(pInfo->CardSaleExtraData3.sItemRestriction , ' ' , sizeof(pInfo->CardSaleExtraData3.sItemRestriction));
		if(iLen)
  			memcpy(pInfo->CardSaleExtraData3.sItemRestriction,pParam,min(sizeof(pInfo->CardSaleExtraData3.sItemRestriction),iLen));

	}

	//4.0.10.14

	// check if exist any item product list	
	bFound = GetParam(pInStr,pInStr->Find("ItemCount[I]"),&pParam,&iLen);

	if(bFound) 
	{	
		nItemCounter = a2i((BYTE *)pParam,iLen);
		for(long i=0;i<nItemCounter;i++)
		{
			sprintf(sFiledName,"ItemCode%d[S]",i);

			// find NACS item code, 
			bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);

			if (bFound)
			{
				byNozzle=0; 
				lNACSCode = a2l((BYTE *)pParam,iLen);
				// convert NACS item to index
				if (ConvertNACSItemsToIndex(lPumpNumber,lNACSCode,lIndex))
				{			
					lVal = 0;

					// Grade product number
					if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
					{
						byNozzle = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetGradePosition((unsigned char)cPumpItems.m_cItemSoldInfo[lIndex].lGrade);
						l2a(cPumpItems.m_cItemSoldInfo[lIndex].lGrade ,pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeID , sizeof(pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeID));

						if ( byNozzle ) //If there is nozzle with that grade,
							byNozzleRestriction |= 1<<(byNozzle-1); //add it to the bit field.
						else
						{
							byNozzle = 1;

							if(LOG_BASIC_FLOW_CONTROL)
							{
								sMsg.Format("Item Not found in lits of Travel center ola pump%ld,code=%d" ,lPumpNumber,  lNACSCode);
								_LOGMSG.LogMsg("PumpNumber %02ld ,Grade not found in nozzle list set nozzle to 1");
							}
						}

						//lGradeRestriction += (long)pow(2,(cPumpItems.m_cItemSoldInfo[lIndex].lGrade -1));	
					}
					else
					{
						lCarWashRestriction = 1;
					}


					sprintf(sFiledName,"ItemDiscount%d[F]",i);
					bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);

					if ((bFound) && (iLen))
					{														
						double x;
						x = atof(pParam);
						lVal = RoundDouble(x , 1000);
						
						// Grade product discount
						if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
						{							

							l2a(lVal,
								pInfo->CardSaleExtraData2.sDiscountPerGrade[byNozzle -1 ].sDiscount , sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[byNozzle - 1].sDiscount));							
							//	pInfo->CardSaleExtraData2.sDiscountPerGrade[(cPumpItems.m_cItemSoldInfo[lIndex].lGrade) -1 ].sDiscount , sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[(cPumpItems.m_cItemSoldInfo[lIndex].lGrade) - 1].sDiscount));							
							
							
						}
						else  // Carwash item for now there is only one 
						{
							l2a(lVal,
								pInfo->CardSaleExtraData3.sCarWashDiscount,min(sizeof(pInfo->CardSaleExtraData3.sCarWashDiscount) , iLen));															
						}
					} 

					//4.0.999.9
					sprintf(sFiledName,"ItemAmountLimit%d[F]",i);

					bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);
					
					if ((bFound) && (iLen))
					{														
						double x;
						x = atof(pParam);
						lVal = RoundDouble(x , 1000);   //4.0.10.20

						if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
						{
							l2a(lVal,
								pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeAmountLimit , sizeof(pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeAmountLimit));
								//pInfo->CardSaleExtraData3.sGradeAmountLimit[i] , sizeof(pInfo->CardSaleExtraData3.sGradeAmountLimit[i]));														

							pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sLimitType= '0'; //4.0.10.59

							if (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetDieselPumpFlags())
							{
								
								if (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetReeferGrade() == cPumpItems.m_cItemSoldInfo[lIndex].lGrade)
									l2a(lVal ,pInfo->CardSaleExtraData3.sReeferLimit , sizeof(pInfo->CardSaleExtraData3.sReeferLimit));
								else
									l2a(lVal ,pInfo->CardSaleExtraData3.sDieselLimit , sizeof(pInfo->CardSaleExtraData3.sDieselLimit));
							}
						}
					}

					sprintf(sFiledName,"ItemVolumeLimit%d[F]",i);

					bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);

					if ((bFound) && (iLen))
					{														
						double x;
						x = atof(pParam);
						lVal = RoundDouble(x , 1000); //4.0.10.54

						if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
						{
							l2a(lVal,
								pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeAmountLimit , sizeof(pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeAmountLimit));	
								//pInfo->CardSaleExtraData3.sGradeAmountLimit[i] , sizeof(pInfo->CardSaleExtraData3.sGradeAmountLimit[i]));														

							pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sLimitType= '1';
							if (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetDieselPumpFlags())
							{
								
								if (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetReeferGrade() == cPumpItems.m_cItemSoldInfo[lIndex].lGrade)
									l2a(lVal ,pInfo->CardSaleExtraData3.sReeferLimit , sizeof(pInfo->CardSaleExtraData3.sReeferLimit));
								else
									l2a(lVal ,pInfo->CardSaleExtraData3.sDieselLimit , sizeof(pInfo->CardSaleExtraData3.sDieselLimit));
							}
						}
					}

					//4.0.999.9
					sprintf(sFiledName,"ItemDiscountCount%d[I]",i); //4.0.10.54
					bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);


					//4.0.19.500
					//if ((bFound) && (iLen))
					if ((bFound) && (iLen)  && (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)) 
					{
						CString sMsg;
						long lDiscountCount = a2l((BYTE *)pParam ,iLen);

						//4.0.19.500
						//for (int i=0  ; (i< MAX_ITEMS_PROMPTS_DISCOUNT) && (i <  lDiscountCount) ;i++)  //4.0.10.54
						if ( i< MAX_ITEMS_PROMPTS_DISCOUNT) //4.0.7390.1700
						{
							TRAVEL_CENTER_REC cTravelCenterRec; //4.0.9999.0
							BOOL bCreate = FALSE;
							
							sMsg.Format("detect ItemDiscountCount for item %d" , i);
							_LOGMSG.LogMsg(sMsg);
							
							memset(cTravelCenterRec.cItemTax ,0,sizeof(cTravelCenterRec.cItemTax));
							
												
							//cTravelCenterRec.lIndex = pTrs->m_lNumber;  //4.0.10.49
							cTravelCenterRec.lIndex = a2l(pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr)); //4.0.10.49

							PS::RECEIPT_EXTRA_INFO_REC rec;
							memcpy(&rec, &cTravelCenterRec, min(sizeof(rec), sizeof(cTravelCenterRec)));
							long lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, rec);
							//long lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cTravelCenterRec);

							if (lRtc)
							{
								memset(&cTravelCenterRec , 0, sizeof(cTravelCenterRec));
								bCreate = TRUE;
							}


							//4.0.10.54
							for (long j=0;j<MAX_PROMPTS_DISCOUNT;j++)
							{
								sprintf(sFiledName,"ItemDiscount%d_%d[F]",i , j); //4.0.19
								bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);
								if ((bFound) && (iLen))
								{
									double x;
									x = atof(pParam);
									lVal = RoundDouble(x , 1000); //4.0.15.501
									cTravelCenterRec.cItemTax[i][j].lTax = lVal;														
								}

								sprintf(sFiledName,"ItemDiscountText%d_%d[S]",i , j);  //4.0.10.54
								bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);
								if ((bFound) && (iLen))
								{
									memcpy(cTravelCenterRec.cItemTax[i][j].sTaxDescription , pParam,min(sizeof(cTravelCenterRec.cItemTax[i][j].sTaxDescription),iLen));														
								}
							}

							// update data base with the new data

							cTravelCenterRec.lIndex = a2l(pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr));

							if (bCreate)   //4.0.10.351
							{
								long lTmp = 0;

								lTmp = a2l(pInfo->CardSaleExtraData2.sFlags ,sizeof(pInfo->CardSaleExtraData2.sFlags));

								if (!(lTmp & FL_EXTRA_RECEIPT_REC))
								{
									lTmp |= FL_EXTRA_RECEIPT_REC;
									l2a(lTmp ,pInfo->CardSaleExtraData2.sFlags ,sizeof(pInfo->CardSaleExtraData2.sFlags)); 
								}


								PS::RECEIPT_EXTRA_INFO_REC rec;
								memcpy(&rec, &cTravelCenterRec, min(sizeof(rec), sizeof(cTravelCenterRec)));
								lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_INSERT, rec);
								//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_INSERT,&cTravelCenterRec);

							}
							else
							{
								PS::RECEIPT_EXTRA_INFO_REC rec;
								memcpy(&rec, &cTravelCenterRec, min(sizeof(rec), sizeof(cTravelCenterRec)));
								lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_UPDATE, rec);
								//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_UPDATE,&cTravelCenterRec);
							}

							if (lRtc)
							{
								sMsg.Format("Attepmt to update zone file has failed Index=%ld , status create=%d , Retcode=%ld", cTravelCenterRec.lIndex, bCreate, lRtc );
								_LOGMSG.LogMsg(sMsg);
							}
						}
					}
					if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
					{
						if(LOG_BASIC_FLOW_CONTROL)
						{
							sMsg.Format("Parse OLA travel center data pump %ld, nozzle=%d ,code=%d, discount=%.5s Amount limit=%.9s Grade =%.2s",	lPumpNumber,
																																					byNozzle,
																																					lNACSCode,
																																					pInfo->CardSaleExtraData2.sDiscountPerGrade[byNozzle -1].sDiscount,
																																					pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeAmountLimit, 
																																					pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeID);
							_Module.m_server.m_cLogger.LogMsg(sMsg);
						}
					}
				}
				else
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{
						sMsg.Format("Item Not found in lits of Travel center ola pump%ld,code=%d" ,lPumpNumber,  lNACSCode);
						_LOGMSG.LogMsg(sMsg);
					}
				}
				 
			}
		}

		// update nozzle restriction map
		i2a(byNozzleRestriction,pInfo->CardSaleInfo.cardData.sNozzleRestriction,sizeof(pInfo->CardSaleInfo.cardData.sNozzleRestriction));
		// update carwash restriction map
		l2a(lCarWashRestriction ,&pInfo->CardSaleExtraData3.sCarwashRestriction, sizeof(pInfo->CardSaleExtraData3.sCarwashRestriction));

		if(LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("OLA restrict Pump=%ld with the following details position map=%ld,Car wash=%ld",lPumpNumber, byNozzleRestriction, lCarWashRestriction);
			_LOGMSG.LogMsg(sMsg);
		}		
	}
}


/*
void COLATravelCenter::ParseItemProduct(long &lPumpNumber, CString *pInStr, CXMLInterface *pInfo ,CPumpTransact *pTrs) //4.0.9999.0
{
	CString  sMsg;
	PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);
	long	lNACSCode;
	long	lIndex;
	BOOL	bFound= FALSE;
	int		iLen ;
	char	*pParam;
	short	nItemCounter = 0;
	char	sFiledName[MAX_FIELD_NAME]; 
	long	lGradeRestriction = 0;
	long	lCarWashRestriction = 0;
	long    lVal=0;
	BYTE	byNozzle = 0;
	BYTE	byNozzleRestriction=0;


//4.0.10.14


	pInfo->SetXmlByteField(ATT_POSITION_RESTRICTION_ACTIVE,'1');
	//pInfo->CardSaleExtraData3.sPositionRestrictionActive = '1';  //4.0.10.49

	bFound = GetParam(pInStr,pInStr->Find("ItemRestriction[I]"),&pParam,&iLen);

	if(bFound) 
	{
		//memset(pInfo->CardSaleExtraData3.sItemRestriction , ' ' , sizeof(pInfo->CardSaleExtraData3.sItemRestriction));
		if(iLen)
			pInfo->SetXmlFieldWithSize(ATT_ITEM_RESTRICTION,pParam,iLen);
  			//memcpy(pInfo->CardSaleExtraData3.sItemRestriction,pParam,min(sizeof(pInfo->CardSaleExtraData3.sItemRestriction),iLen));

	}

	//4.0.10.14

	// check if exist any item product list	
	bFound = GetParam(pInStr,pInStr->Find("ItemCount[I]"),&pParam,&iLen);

	if(bFound) 
	{	
		nItemCounter = a2i((BYTE *)pParam,iLen);
		for(long i=0;i<nItemCounter;i++)
		{
			sprintf(sFiledName,"ItemCode%d[S]",i);

			// find NACS item code, 
			bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);

			if (bFound)
			{
				byNozzle=0; 
				lNACSCode = a2l((BYTE *)pParam,iLen);
				// convert NACS item to index
				if (ConvertNACSItemsToIndex(lPumpNumber,lNACSCode,lIndex))
				{			
					lVal = 0;

					// Grade product number
					if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
					{
						byNozzle = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetGradePosition((unsigned char)cPumpItems.m_cItemSoldInfo[lIndex].lGrade);

						pInfo->SetXmlLongFieldByAttribute(ELEMENT_LIMIT,ATT_ID,byNozzle - 1 ,ATT_GRADE_ID,cPumpItems.m_cItemSoldInfo[lIndex].lGrade);
						//l2a(cPumpItems.m_cItemSoldInfo[lIndex].lGrade ,pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeID , sizeof(pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeID));

						if ( byNozzle ) //If there is nozzle with that grade,
							byNozzleRestriction |= 1<<(byNozzle-1); //add it to the bit field.
						else
						{
							byNozzle = 1;

							if(LOG_BASIC_FLOW_CONTROL)
							{
								sMsg.Format("Item Not found in lits of Travel center ola pump%ld,code=%d" ,lPumpNumber,  lNACSCode);
								_LOGMSG.LogMsg("PumpNumber %02ld ,Grade not found in nozzle list set nozzle to 1");
							}
						}

						//lGradeRestriction += (long)pow(2,(cPumpItems.m_cItemSoldInfo[lIndex].lGrade -1));	
					}
					else
					{
						lCarWashRestriction = 1;
					}


					sprintf(sFiledName,"ItemDiscount%d[F]",i);
					bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);

					if ((bFound) && (iLen))
					{														
						double x;
						x = atof(pParam);
						lVal = RoundDouble(x , 1000);
						
						// Grade product discount
						if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
						{							

							pInfo->SetXmlLongFieldByAttribute(ELEMENT_DISCOUNT,ATT_ID,byNozzle -1,ATT_DISCOUNT,lVal);
							//l2a(lVal,
							//	pInfo->CardSaleExtraData2.sDiscountPerGrade[byNozzle -1 ].sDiscount , sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[byNozzle - 1].sDiscount));							
							
							
						}
						else  // Carwash item for now there is only one 
						{
							pInfo->SetXmlLongField(ATT_CAR_WASH_DISCOUNT,lVal);
							//l2a(lVal,
							//	pInfo->CardSaleExtraData3.sCarWashDiscount,min(sizeof(pInfo->CardSaleExtraData3.sCarWashDiscount) , iLen));															
						}
					} 

					//4.0.999.9
					sprintf(sFiledName,"ItemAmountLimit%d[F]",i);

					bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);

					if ((bFound) && (iLen))
					{														
						double x;
						x = atof(pParam);
						lVal = RoundDouble(x , 1000);   //4.0.10.20

						if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
						{

							pInfo->SetXmlLongFieldByAttribute(ELEMENT_LIMIT,ATT_ID,byNozzle - 1 ,ATT_GRADE_AMOUNT_LIMIT,lVal);
							//l2a(lVal,
							//	pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeAmountLimit , sizeof(pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeAmountLimit));														

							pInfo->SetXmlByteFieldByAttribute(ELEMENT_LIMIT,ATT_ID,byNozzle - 1 ,ATT_LIMIT_TYPE,'0');
							//pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sLimitType= '0'; //4.0.10.59

							if (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetDieselPumpFlags())
							{
								
								if (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetReeferGrade() == cPumpItems.m_cItemSoldInfo[lIndex].lGrade)

									pInfo->SetXmlLongField(ATT_REEFER_LIMIT,lVal);
									//l2a(lVal ,pInfo->CardSaleExtraData3.sReeferLimit , sizeof(pInfo->CardSaleExtraData3.sReeferLimit));
								else
									pInfo->SetXmlLongField(ATT_DIESEL_LIMIT,lVal);
									//l2a(lVal ,pInfo->CardSaleExtraData3.sDieselLimit , sizeof(pInfo->CardSaleExtraData3.sDieselLimit));
							}
						}
					}


//4.0.10.20
					sprintf(sFiledName,"ItemVolumeLimit%d[F]",i);

					bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);

					if ((bFound) && (iLen))
					{														
						double x;
						x = atof(pParam);
						lVal = RoundDouble(x , 1000); //4.0.10.54

						if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
						{

							pInfo->SetXmlLongFieldByAttribute(ELEMENT_LIMIT,ATT_ID,byNozzle - 1 ,ATT_GRADE_AMOUNT_LIMIT,lVal);
							//l2a(lVal,
							//	pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeAmountLimit , sizeof(pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeAmountLimit));														

							pInfo->SetXmlByteFieldByAttribute(ELEMENT_LIMIT,ATT_ID,byNozzle - 1 ,ATT_LIMIT_TYPE,'1');
							//pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sLimitType= '1';
							if (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetDieselPumpFlags())
							{
								
								if (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetReeferGrade() == cPumpItems.m_cItemSoldInfo[lIndex].lGrade)
									pInfo->SetXmlLongField(ATT_REEFER_LIMIT,lVal);
									//l2a(lVal ,pInfo->CardSaleExtraData3.sReeferLimit , sizeof(pInfo->CardSaleExtraData3.sReeferLimit));
								else
									pInfo->SetXmlLongField(ATT_DIESEL_LIMIT,lVal);
									//l2a(lVal ,pInfo->CardSaleExtraData3.sDieselLimit , sizeof(pInfo->CardSaleExtraData3.sDieselLimit));
							}
						}
					}


//4.0.10.20

					//4.0.999.9
					sprintf(sFiledName,"ItemDiscountCount%d[I]",i); //4.0.10.54
					bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);


					if ((bFound) && (iLen))
					{
						CString sMsg;
						long lDiscountCount = a2l((BYTE *)pParam ,iLen);

						for (int i=0  ; (i< MAX_ITEMS_PROMPTS_DISCOUNT) && (i <  lDiscountCount) ;i++)  //4.0.10.54
						{
							TRAVEL_CENTER_REC cTravelCenterRec; //4.0.9999.0
							BOOL bCreate = FALSE;
							
							sMsg.Format("detect ItemDiscountCount for item %d" , i);
							_LOGMSG.LogMsg(sMsg);
							
							memset(cTravelCenterRec.cItemTax ,0,sizeof(cTravelCenterRec.cItemTax));
							
							cTravelCenterRec.lIndex = pInfo->GetXmlLongField(ATT_TRAN_NBR);
							//cTravelCenterRec.lIndex = a2l(pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr)); //4.0.10.49

							long lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cTravelCenterRec);


							if (lRtc)
							{
								memset(&cTravelCenterRec , 0, sizeof(cTravelCenterRec));
								bCreate = TRUE;
							}


							//4.0.10.54
							for (long j=0;j<MAX_PROMPTS_DISCOUNT;j++)
							{
								sprintf(sFiledName,"ItemDiscount%d_%d[S]",i , j); //4.0.19
								bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);
								if ((bFound) && (iLen))
								{
									double x;
									x = atof(pParam);
									lVal = RoundDouble(x , 100);
									cTravelCenterRec.cItemTax[i][j].lTax = lVal;

									//	l2a(lVal,
									//		pInfo->CardSaleExtraData3.sGradeAmountLimit[i] , sizeof(pInfo->CardSaleExtraData3.sGradeAmountLimit[i]));														
								}

								sprintf(sFiledName,"ItemDiscountText%d_%d[S]",i , j);  //4.0.10.54
								bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);
								if ((bFound) && (iLen))
								{
									memcpy(cTravelCenterRec.cItemTax[i][j].sTaxDescription , pParam,min(sizeof(cTravelCenterRec.cItemTax[i][j].sTaxDescription),iLen));
									//	l2a(lVal,
									//		pInfo->CardSaleExtraData3.sGradeAmountLimit[i] , sizeof(pInfo->CardSaleExtraData3.sGradeAmountLimit[i]));														
								}

							}

							// update data base with the new data

							cTravelCenterRec.lIndex = pInfo->GetXmlLongField(ATT_TRAN_NBR);
							//cTravelCenterRec.lIndex = a2l(pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr));

							if (bCreate)   //4.0.10.351
							{
								long lTmp = 0;

								lTmp = pInfo->GetXmlLongField(ATT_FLAGS);
								//lTmp = a2l(pInfo->CardSaleExtraData2.sFlags ,sizeof(pInfo->CardSaleExtraData2.sFlags));

								if (!(lTmp & FL_EXTRA_RECEIPT_REC))
								{
									lTmp |= FL_EXTRA_RECEIPT_REC;
									pInfo->SetXmlLongField(ATT_FLAGS,lTmp);
									//l2a(lTmp ,pInfo->CardSaleExtraData2.sFlags ,sizeof(pInfo->CardSaleExtraData2.sFlags)); 
								}


								lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_INSERT,&cTravelCenterRec);

							}
							else
								lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_UPDATE,&cTravelCenterRec);


							if (lRtc)
							{
								sMsg.Format("Attepmt to update zone file has failed Index=%ld , status create=%d , Retcode=%ld", cTravelCenterRec.lIndex, bCreate, lRtc );
								_LOGMSG.LogMsg(sMsg);
							}
						}
					}
					//4.0.999.0

					if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
					{
						if(LOG_BASIC_FLOW_CONTROL)
						{
							sMsg.Format("Parse OLA travel center data pump %ld, nozzle=%d ,code=%d, discount=%.5s Amount limit=%.9s Grade =%.2s" ,lPumpNumber,  byNozzle ,lNACSCode, pInfo->CardSaleExtraData2.sDiscountPerGrade[byNozzle -1].sDiscount, pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeAmountLimit ,pInfo->CardSaleExtraData3.GradeLimitsArray[byNozzle -1].sGradeID);
							_LOGMSG.LogMsg(sMsg);
						}
					}
					
					//Convertl2Str2(2000,pInfo->CardSaleInfo.extraData.sAuthAmt_Msb,sizeof(pInfo->CardSaleInfo.extraData.sAuthAmt_Msb) , pInfo->CardSaleInfo.cardData.sAuthAmt, sizeof(pInfo->CardSaleInfo.cardData.sAuthAmt));	
				}
				else
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{
						sMsg.Format("Item Not found in lits of Travel center ola pump%ld,code=%d" ,lPumpNumber,  lNACSCode);
						_LOGMSG.LogMsg(sMsg);
					}
				}
				 
			}
		}

		// update nozzle restriction map
		pInfo->SetXmlLongField(ATT_NOZZLE_RESTRICTION,byNozzleRestriction);
		//i2a(byNozzleRestriction,pInfo->CardSaleInfo.cardData.sNozzleRestriction,sizeof(pInfo->CardSaleInfo.cardData.sNozzleRestriction));
		// update carwash restriction map
		pInfo->SetXmlLongField(ATT_CAR_WASH_RESTRICTION,lCarWashRestriction);
		//l2a(lCarWashRestriction ,&pInfo->CardSaleExtraData3.sCarwashRestriction, sizeof(pInfo->CardSaleExtraData3.sCarwashRestriction));


		if(LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("OLA restrict Pump=%ld with the following details position map=%ld,Car wash=%ld",lPumpNumber, byNozzleRestriction, lCarWashRestriction);
			_LOGMSG.LogMsg(sMsg);
		}		
	}
}
*/
void COLATravelCenter::SendAllPrompts(long &lOffset, char *pOutBuffer, PAY_AT_PUMP_INFO *pInfo)
{
	TRAVEL_CENTER_REC cTravelCenterRec; //4.0.9999.0
	char sPromptName0Empty[MAX_PROM_NAME];

	char sTmpPromptName[MAX_PROM_NAME +2 ];
	char sTmpPromptResult[MAX_PROM_RESULT +2 ];
	char sBufferTmp[MAX_PROM_NAME + 2];
	long lPromptCount =0;


	memset(sPromptName0Empty , 0  ,sizeof(sPromptName0Empty));
	cTravelCenterRec.lIndex =  a2l(pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr));


	PS::RECEIPT_EXTRA_INFO_REC rec;
	memcpy(&rec, &cTravelCenterRec, min(sizeof(rec), sizeof(cTravelCenterRec)));
	long lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, rec);
	//long lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cTravelCenterRec);
	if (lRtc)
	{	
		CString strTmp;   //4.  //4.0.5.40 
		strTmp.Format("SendAllPrompts information Index=%ld , Rtc=%ld" , cTravelCenterRec.lIndex ,lRtc ); 																	
		_LOGMSG.LogMsg(strTmp);								
	}
	else
	{
		char sPromptName0Empty[MAX_PROM_NAME]; 
	
		for (long i=0 ;i < MAX_PROMPTS ; i++)
		{

		//	if(LOG_DETAIL_FLOW_CONTROL)
		//	{
		//		CString str;
		//		str.Format("Prompt i=%ld prompts=[%.20s]",i,cTravelCenterRec.cArrTravelCenterData[i].PromptName0 );
		//		_LOGMSG.LogMsg(str);
		//	}


			if (			
				(!ChrAll((char *)cTravelCenterRec.cArrTravelCenterData[i].PromptName0 , sizeof(cTravelCenterRec.cArrTravelCenterData[i].PromptName0))) && 
				(!ChrAll((char *)cTravelCenterRec.cArrTravelCenterData[i].PromptName0 , sizeof(cTravelCenterRec.cArrTravelCenterData[i].PromptName0),0)))
			{
				memset(sPromptName0Empty , 0  ,sizeof(sPromptName0Empty));

				memset(sBufferTmp , 0, sizeof(sBufferTmp));
				
				memset(sTmpPromptResult , 0 , sizeof(sTmpPromptResult));

				memcpy(sTmpPromptResult , cTravelCenterRec.cArrTravelCenterData[i].PromptResult , sizeof(cTravelCenterRec.cArrTravelCenterData[i].PromptResult));
				
				memcpy(sBufferTmp, cTravelCenterRec.cArrTravelCenterData[i].PromptName0 ,MAX_PROM_NAME);
				TrimRightSpace(sBufferTmp ,MAX_PROM_NAME);

				if (cTravelCenterRec.cArrTravelCenterData[i].PromptDataType0 == 'N')						
					sprintf(sTmpPromptName ,"%s[I]", sBufferTmp);
				else
					sprintf(sTmpPromptName ,"%s[S]", sBufferTmp);

				lOffset += SetParamNoCheck(pOutBuffer+lOffset,(BYTE *)sTmpPromptName,(BYTE *)"%s=%.25s,", cTravelCenterRec.cArrTravelCenterData[i].PromptResult,sizeof(cTravelCenterRec.cArrTravelCenterData[i].PromptResult));
				lPromptCount++;

			}
		}				
	}


	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("Send count=%ld prompts to ola", lPromptCount);
		_LOGMSG.LogMsg(str);
	}

}


/******************************************************************************
Description:Add prompts to ola buffers.
Returns:
Parameters:

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Marik			18/05/2005   19:13		Start
******************************************************************************/
void COLATravelCenter::AddPromptsRespons(long &lOffset, char *pOutBuffer, PAY_AT_PUMP_INFO *pInfo)
{
	TRAVEL_CENTER_REC cTravelCenterRec; //4.0.9999.0
	TravelCenterData cTravelCenterData;
	char sPromptName0Empty[MAX_PROM_NAME];

	memset(sPromptName0Empty, 0, sizeof(sPromptName0Empty));

	cTravelCenterRec.lIndex = a2l(pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr));
	PS::RECEIPT_EXTRA_INFO_REC rec;
	memcpy(&rec, &cTravelCenterRec, min(sizeof(rec), sizeof(cTravelCenterRec)));
	long lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, rec);
	//long lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cTravelCenterRec);
	if (lRtc)
	{
		CString strTmp;   //4.  //4.0.5.40 
		strTmp.Format("Fail read receipt information Index=%ld , Rtc=%ld", cTravelCenterRec.lIndex, lRtc);
		_LOGMSG.LogMsg("COLABase", "AddPromptRespons", 0, LOG_NONE, strTmp);
	}
	else
	{
		char sPromptName0Empty[MAX_PROM_NAME];
		BOOL bFound = FALSE;

		memset(sPromptName0Empty, 0, sizeof(sPromptName0Empty));

		for (long i = 0; i < MAX_PROMPTS; i++)
		{
			// Overwrite exist prompts
			if (cTravelCenterRec.cArrTravelCenterData[i].PromptResult[0] == '?')
			{
				cTravelCenterData = cTravelCenterRec.cArrTravelCenterData[i];
				memcpy(cTravelCenterRec.cArrTravelCenterData[i].PromptResult, pInfo->CardSaleExtraData3.sParamResult, min(sizeof(pInfo->CardSaleExtraData3.sParamResult), sizeof(cTravelCenterRec.cArrTravelCenterData[i].PromptResult)));

				// clear temprorey prompts
				if (pInfo->CardSaleExtraData3.sOperationalPrompt == 'Y')
					memset(&cTravelCenterRec.cArrTravelCenterData[i], 0x00, sizeof(TravelCenterData));

				PS::RECEIPT_EXTRA_INFO_REC rec;
				memcpy(&rec, &cTravelCenterRec, min(sizeof(rec), sizeof(cTravelCenterRec)));
				long lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_UPDATE, rec);
				//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_UPDATE,&cTravelCenterRec);


				bFound = TRUE;
				break;
			}
		}

		// return the answer to mosaic link.
		if (bFound)
		{
			char sTmpPrompts[50];
			char stmp[MAX_PROM_NAME + 2];

			memset(stmp, 0, sizeof(stmp));

			CString sSpacePromptName;


			//4.0.9999.0
			memcpy(stmp, cTravelCenterData.PromptName0, MAX_PROM_NAME);

			for (int i = MAX_PROM_NAME - 1; i >= 0; i--)
			{
				if (stmp[i] != ' ')
				{
					stmp[i + 1] = 0;
					break;

				}
			}


			if (cTravelCenterData.PromptDataType0 == 'N')
				sprintf_s(sTmpPrompts, 50, "%s[I]", stmp);
			else
				sprintf_s(sTmpPrompts, 50, "%s[S]", stmp);
			lOffset += SetParamNoCheck(pOutBuffer + lOffset, (BYTE *)sTmpPrompts, (BYTE *)"%s=%.113s,", pInfo->CardSaleExtraData3.sParamResult, sizeof(pInfo->CardSaleExtraData3.sParamResult));

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("Prompt Name send to mosaic is  promptsname=%s , PromptType=%c, data=%.113s , Prompt Cl[%c]", sTmpPrompts, cTravelCenterData.PromptDataType0, pInfo->CardSaleExtraData3.sParamResult, pInfo->CardSaleExtraData3.sPromptType);
				_LOGMSG.LogMsg("COLATravelCenter", "AddPromptRespons", 0, LOG_NONE, str);
			}
		}
	}
}