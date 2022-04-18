// OLASimLinkSrv.cpp: implementation of the COLASimLinkSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLASimLinkSrv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLASimLinkSrv::COLASimLinkSrv()
{
	SetIFSFProtocoll(TRUE);
}

COLASimLinkSrv::~COLASimLinkSrv()
{

}

/////////////////////////////////////////////////////////
//OLA initialization.
/////////////////////////////////////////////////////////
/******************************************************************************
 Description:	Init the OLA server COM object
  	
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			20/07/08       		Start - TD 27377
******************************************************************************/
void COLASimLinkSrv::Init()
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
					hRes = m_pSrvPtr.CreateInstance(__uuidof(SimLinkSrv::Services)); 

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
/******************************************************************************
 Description:	Get from OLA info about the card
  	
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			20/07/08       		Start - TD 27377
******************************************************************************/
long COLASimLinkSrv::GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags,long *lSesionType, long lPumpNumber, PAY_AT_PUMP_INFO * pInPAPBuffer)	//4.0.20.52 (TD 63936)
{
	long lRtc = 0;
	//_bstr_t sRetBuf;
	CComBSTR sRetBuf;//4.0.18.501 merge from 4.0.15.285

	EnterCriticalSection(&m_csOLASrv);

	try
	{	
		
		sRetBuf.AppendBSTR(m_pSrvPtr->GetCardInfo (sAccountNumber, sTrack1, sTrack2, sShortCardName));
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


/******************************************************************************
 Description:	Add OLA params
  	
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			20/07/08       		Start - TD 27377
******************************************************************************/
long COLASimLinkSrv::AddParams(long lSessId , char * pInBuffer)
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
/******************************************************************************
 Description:	Start the OLA session
  	
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			20/07/08       		Start - TD 27377
******************************************************************************/
long COLASimLinkSrv::StartSession(char * pInBuffer, char *pszType)
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

/******************************************************************************
 Description:	Query the OLA session
  	
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			20/07/08       		Start - TD 27377
******************************************************************************/
long COLASimLinkSrv::QuerySession(long lSessionNumber, BSTR * pOutBuffer)
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
 Description:	End the OLA session
  	
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			20/07/08       		Start - TD 27377
******************************************************************************/
long COLASimLinkSrv::EndSession(long lSessionId)
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
/******************************************************************************
 Description:	Release the OLA server COM object
  	
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			20/07/08       		Start - TD 27377
******************************************************************************/
void COLASimLinkSrv::ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE*/)
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

/////////////////////////////////////////////////////////
//Card data extraction methods.
/////////////////////////////////////////////////////////
/******************************************************************************
 Description:	Get from OLA extra info about the card
  	
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			20/07/08       		Start - TD 27377
******************************************************************************/
long COLASimLinkSrv::GetCardInfoEx(long lClientID,BSTR sInBuffer,BSTR *sRetData,long lFlags )
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



void COLASimLinkSrv::AddPreAuthInformation(long lPumpNumber,long &lOffset, char *pOutBuffer , long & lOLAPumpServiceMode, long & lGradeRestrict)
{
	PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);
	long lCount = cPumpItems.lCounter;
	char sFiledName[MAX_FIELD_NAME];
	long	lPriceCash=0 , lPriceCredit=0;
	long	lNumOfItems=0;//4.0.11.504	fix this function TD 14868


	// Add item list 
	if (lPumpNumber && lCount)
	{
		for (int i= 0; i < cPumpItems.lCounter ; i++)
		{	
			// send only full information...  
			if (cPumpItems.m_cItemSoldInfo[i].lGrade)
			{
				if (cPumpItems.m_cItemSoldInfo[i].bFuelItem)
				{
					//if ((!lGradeRestrict) ||    //4.0.7.500
						//(cPumpItems.m_cItemSoldInfo[i].lGrade == lGradeRestrict))
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
				}
				else
				{
					PAY_AT_PUMP_INFO pap;
					_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&pap);
					long lQty = a2l(pap.CardSaleInfo.extraData.SALES[0].sQty,sizeof(pap.CardSaleInfo.extraData.SALES[0].sQty));
					if (lQty > 0)
					{
						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemIsFuel%d[B]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *)"N", 1);

						long lUnitPrice = a2l((BYTE *)pap.CardSaleInfo.extraData.SALES[0].sUnitPrice, sizeof(pap.CardSaleInfo.extraData.SALES[0].sUnitPrice)) *(_Module.m_server.GetCarWashScalarFactor());
						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPrice%d[F]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%ld.%03ld,", lUnitPrice / 1000, lUnitPrice % 1000);

						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemCode%d[S]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%03ld,", cPumpItems.m_cItemSoldInfo[i].lGrade);
						lNumOfItems++;  //4.0.11.504

						int i = 0;
						for (i = 0; i < 15; i++)
						{
							if (pap.CardSaleExtraData6.CarWashItem.sUpc[i] == 0x20)
								break;
						}
						char tmp[16] = { 0 };
						memcpy(tmp, pap.CardSaleExtraData6.CarWashItem.sUpc, i);
						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemUPC%d[F]", lNumOfItems);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *)tmp, sizeof(tmp));
					}
				}		
			}
		}	
		lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCount[I]",	(BYTE *)"%s=%03ld," ,lNumOfItems);
	}		
}



void COLASimLinkSrv::BuildGetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, char *pOutBuffer)
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

	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"Track2[S]", (BYTE *)"%s=%.37s,", pCardData->sClubCardTrack2Data, sizeof(pCardData->sClubCardTrack2Data));
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
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"MediaType[S]", (BYTE *)"%s=%.1c,", 'C');
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ClubCardNumber[S]", (BYTE *)"%s=%.20s,", pCardDataExtra->sLoyalId, sizeof(pCardDataExtra->sLoyalId));

	// 4.0.7.500
	long lProdCode = a2l(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode));

	//AddedPreAuthInformation(lPumpNumber, lOffset, pOutBuffer, lOLAPumpServiceMode, lProdCode);
// 	const int iEncrptSize = sizeof pCardDataExtra->sLoyalId - 4; // //RFUEL-1367
// 	CString str;
// 	str.Format("COLAEpsilon::BuildGetCustomerInfoRequest() - Pump %02ld - lTransNumber = %ld, lSeqNumber %ld, sTranType = %c, ClubCardNumber = XXXXXXXXXXXXXXXX%.4s, EntryMethod = %c",
// 		lPumpNumber,
// 		lTransNumber,
// 		lSeqNumber,
// 		pCardData->sTranType,
// 		pCardDataExtra->sLoyalId+ iEncrptSize,
// 		pCardData->sEntryMethod);
// 	_LOGMSG.LogMsg(str);
	
}


void COLASimLinkSrv::BuildSetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, CPumpTransact *pTrs, char *pOutBuffer)
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

	//// This is the Loyalty discount we are going to report, but if tender discount was applied, we subtract that out of this value
	long lGasLoyaltyDiscountPerGallon = a2l(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount, sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount));
	//const long TenderDiscount = _Module.m_server.GetTenderDiscount(*pInfo);
	//if (TenderDiscount > 0) {
	//	lGasLoyaltyDiscountPerGallon = lGasLoyaltyDiscountPerGallon - TenderDiscount;
	//}

	
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
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"UnitPrice[F]", (BYTE *)"%s=%ld.%03ld,", lGradePrice / 1000, lGradePrice % 1000);

	// RFUEL-289
	BYTE sIncludeDiscountFlags;

	sIncludeDiscountFlags = ((unsigned char)(pCardDataExtra3->sUnitPriceIncludeDiscount == '1')) ? 'Y' : 'N';
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

			lItemCount = 1;
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
			// Fixed buffer over-run
			char sSeconds[3] = { 0 }; //4.0.23.120 87260
			BYTE sTmpDate[8];
			char sTmpTime[6];

			memset(&sTmpDate, 0, sizeof(sTmpDate));
			memset(&sTmpTime, '0', sizeof(sTmpTime));
			memset(sYear, 0, sizeof(sYear));

			//If the year is bigger then 70 then it 19XX else it 20XX
			GetLocalTime(&systime);  //4.0.2.32
			if (systime.wYear >= 2000)
			{
				sprintf_s(sYear, _countof(sYear), "20%.2s", pInfo->CardSaleInfo.extraData.sPrnDateTime);
			}
			else
			{
				sprintf_s(sYear, _countof(sYear), "19%.2s", pInfo->CardSaleInfo.extraData.sPrnDateTime);
			}
			memcpy(&sTmpDate[4], sYear, 4); //4.0.2.32
			memcpy(sTmpDate, &pInfo->CardSaleInfo.extraData.sPrnDateTime[2], 4);

			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransactionDate[D]", (BYTE *)"%s=%.8s,", (BYTE *)sTmpDate, sizeof(sTmpDate));
			//hhmmss
			sprintf_s(sSeconds, _countof(sSeconds), "%02d", systime.wSecond); //4.0.23.120 87260
			memcpy(sTmpTime, &pInfo->CardSaleInfo.extraData.sPrnDateTime[6], 4);

			memcpy(&sTmpTime[4], sSeconds, 2); //4.0.23.120 87260

			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransactionTime[T]", (BYTE *)"%s=%.6s,", (BYTE *)sTmpTime, sizeof(sTmpTime));
		}
	}
}

void COLASimLinkSrv::BuildEpsilonFields(long lPumpIndex, long lTotalAmount, long &lOffset, char *pOutBuffer, PAY_AT_PUMP_INFO *pPAPInfo)
{
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TenderCount[I]", (BYTE *)"%s=%ld,", 1);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransAmount0[F]", (BYTE *)"%s=%ld.%03ld,", lTotalAmount / 1000, lTotalAmount % 1000);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ReferenceNumber0[S]", (BYTE *)"%s=%.12s,", pPAPInfo->CardSaleInfo.cardData.sReferenceNumber, sizeof(pPAPInfo->CardSaleInfo.cardData.sReferenceNumber));
}