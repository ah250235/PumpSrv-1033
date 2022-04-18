#include "stdafx.h"
#include "EmeraldLoyaltyCommandExecuter.h"
#include "FuelLoyaltyHelper.h"
#include "XMLConsts.h"


CEmeraldLoyaltyCommandExecuter::CEmeraldLoyaltyCommandExecuter()
{
}


CEmeraldLoyaltyCommandExecuter::~CEmeraldLoyaltyCommandExecuter()
{
}

long CEmeraldLoyaltyCommandExecuter::StartTicket(_bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag)
{
	if (LOG_DETAIL_FLOW_CONTROL)
	{
		_Module.m_server.m_cLogger.LogClassMsg("CEmeraldLoyaltyCommandExecuter", "StartTicket", lPumpNumber, LOG_PUMP, "Start");
	}
	return FALSE;
}

long CEmeraldLoyaltyCommandExecuter::AddMemberCard(_bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lFlags, PAY_AT_PUMP_INFO & pInfo)
{
	return FALSE;
}

long CEmeraldLoyaltyCommandExecuter::CancelTicket(long lPumpNumber, long lFlag)
{
	PAY_AT_PUMP_INFO  cTmpInfo;
	BOOL		bRetCode = TRUE;
	CComBSTR	bstrOutData;
	CString sTransactionId, strMsg;
	

	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cTmpInfo);
	try
	{

		//RFUEL - 2271 Loyalty Cancel request from Fuel to Emerald  
		bRetCode = CFuelLoyaltyHelper::GetTransactionDataString(cTmpInfo, CString("sTransactionId"), sTransactionId);
		if (sTransactionId.IsEmpty() || !bRetCode) // first check if we have a valid query that we should cancel, otherwise don't send cancelTicket  to Emerald      
		{
			bRetCode = FALSE;
			strMsg = " No transactionId received from Emerald. Cancel ticket query will not be sent to Emerald.";
			_Module.m_server.m_cLogger.LogClassMsg("CEmeraldLoyaltyCommandExecuter", "CancelTicket", lPumpNumber, LOG_PUMP, strMsg.GetBuffer());
		}
		else
		{
			long lTrsNumber = 0;
			CFuelLoyaltyHelper::GetTransactionDatalong(cTmpInfo, "sTranNbr", lTrsNumber);
			//Prepare XML for sending query 
			bRetCode = _Module.m_server.m_cXmlParser.CreateEmeraldCancelTicketQueryXML(lPumpNumber, bstrOutData, cTmpInfo, lTrsNumber);
			HRESULT hResult = (HRESULT)(m_pRTI->CancelTicket2(bstrOutData.m_str, lPumpNumber, 0));
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				strMsg.Format("CancelTicket call to m_pRTI->CancelTicket2 is finished with result:%d", hResult);
				_Module.m_server.m_cLogger.LogClassMsg("CEmeraldLoyaltyCommandExecuter", "CancelTicket", lPumpNumber, LOG_PUMP, strMsg.GetBuffer());
			}
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CEmeraldLoyaltyCommandExecuter", "CancelTicket", lPumpNumber, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1);
		bRetCode = FALSE;
	}
	return bRetCode;
}


BOOL CEmeraldLoyaltyCommandExecuter::Query(long lPumpNumber, PAY_AT_PUMP_INFO & pInfo)
{
	BOOL		bRetCode = TRUE;	//4.0.22.504
	PumpItems	cPumpItemsGrades;
	long		p = lPumpNumber - 1;
	BOOL		bAddCarWashBeforeFueling = FALSE;
	char		strXML[MAX_BUF] = { 0 }; //  4.0.23.980
	long		lTrsNumber = 0;
	CComBSTR	bstrOutData, bstrTmp;
	BSTR		sXMLInData = NULL;

	try
	{
		CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[p].GetStat();
		cPumpItemsGrades.lCounter = 0;
		_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber, cPumpItemsGrades);
		if (a2l(pInfo.CardSaleInfo.extraData.sWashCode, sizeof(pInfo.CardSaleInfo.extraData.sWashCode)))
			bAddCarWashBeforeFueling = TRUE;

		////////////   Prepare XML for sending query   ////////////////////////////
		CFuelLoyaltyHelper::GetTransactionDatalong(pInfo, "sTranNbr", lTrsNumber);
		bRetCode = _Module.m_server.m_cXmlParser.CreateQueryXML(lPumpNumber, &cPumpItemsGrades, strXML, pInfo, bAddCarWashBeforeFueling); //4.0.22.504 
		bstrTmp.Append(strXML);
		sXMLInData = bstrTmp.Copy();
		bRetCode = _Module.m_server.m_cXmlParser.CreateEmeraldFuelProductDataXML(sXMLInData, bstrOutData, lTrsNumber, lPumpNumber, LOYALTY_QUERY_REQUEST);
		bRetCode = AddCarWashItemToXml(pInfo,bstrOutData);  //RFUEL - 2142  Add carwash item to query XML which is to be sent to Emerald to get the valid promotions..
		//////////////////////////////////////////////////////////////////////////
		if (bRetCode)
		{    ///    Send query xml  ///
			(_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber))->UpdateCarWashTime(pInfo, lPumpNumber);	//4.0.20.10
			_Module.m_server.m_cFuelLoayaltyPS.SetTimer(p, QUERY_CHECK_TIMEOUT); // // TD 406362  4.0.23.2390
#ifdef DEBUG
			CXMLInterface dbgXml;
			dbgXml.InitDoc();
			CString str("C:\\LPE\\XML\\LoyaltyQeury.xml");
			if (dbgXml.LoadFile(str))
			{
				dbgXml.GetXmlString(&bstrOutData);
				str += "  !!!!!!!!!!!!!!!!!!!! CParserXML:: DebugAnalyze Load Query From Local File  !!!!!!!!!!!!!!!!!!!!!!!!!";
			}
			else
			{
				str = +"CEmeraldLoyaltyCommandExecuter:: Failed loading file a debug file and will use the regular response buffer received from Emerald.";
				_Module.m_server.m_cLogger.LogMsg(-1, LOG_PUMP, str);
			}
			dbgXml.CloseDoc();
#endif
			bRetCode = m_pRTI->Query(bstrOutData.m_str, lPumpNumber, 0) == S_OK;
			CString strMsg;
			strMsg.Format("CEmeraldLoyaltyCommandExecuter::Query PumpNumber = %ld, TrsNumber = %ld, Query result = %d", lPumpNumber, lTrsNumber, bRetCode);
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
		}
		else
		{
			_Module.m_server.m_cLogger.LogMsg("CEmeraldLoyaltyCommandExecuter", "RunQuery", lPumpNumber, LOG_PUMP, "Failed Create Query XML");
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CEmeraldLoyaltyCommandExecuter", "RunQuery", lPumpNumber, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
		bRetCode = FALSE;
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		_Module.m_server.m_cLogger.LogClassMsg("CEmeraldLoyaltyCommandExecuter", "RunQuery", lPumpNumber, LOG_PUMP, "Query was finished");
	}
	return bRetCode;
}

long CEmeraldLoyaltyCommandExecuter::AddItem(_bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, OLA_STAT & ola, long lTrsNum)
{
	BOOL bRetCode = TRUE;
	CXMLInterface xmlInitDoc;
	long lPumpIndex = lPumpNumber - 1;

	PAY_AT_PUMP_INFO  cPapInfo;
	_Module.m_server.m_cPumpArray[lPumpIndex].GetPAPInfo(&cPapInfo);

	PumpItems	cPumpItemsGrades;
	_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber, cPumpItemsGrades);

	CPumpTransact trs;

	EnterCriticalSection(_Module.m_server.m_csPumpArray);
	{
		if (_Module.m_server.m_cPumpArray[lPumpIndex].GetTrsByNumber(lTrsNum, &trs))
		{
			CString sMsg;
			sMsg.Format("CEmeraldLoyaltyCommandExecuter::AddItem Error! Couldn't find trs %d!", lTrsNum);
			_LOGMSG.LogMsg(sMsg, LOG_LEVEL_1);
		}  // 4.0.19.1790  TD 349356
	}
	LeaveCriticalSection(_Module.m_server.m_csPumpArray);

	_Module.m_server.m_cFuelLoayaltyPS.CalculateLoyaltyValues(cPapInfo, lPumpNumber, trs);//4.0.20.300
	long lTotalDiscount = a2l(cPapInfo.CardSaleExtraData2.sTotalDiscount, sizeof cPapInfo.CardSaleExtraData2.sTotalDiscount);

	CComBSTR sXml;
	bRetCode = CreateAddItemXML(sXml, cPumpItemsGrades, lPumpNumber, trs, lTotalDiscount);

	try
	{
		bRetCode = xmlInitDoc.InitDoc();
		bstr_t bstrXMLStuctData(sXml, TRUE);
		bRetCode = xmlInitDoc.LoadXmlString(bstrXMLStuctData);
		char sPath[MAX_FILE_PATH] = { 0 };
		strcat_s((char *)sPath, MAX_FILE_PATH, _Module.m_server.GetLoyaltyDataPath());

		if (!strlen(sPath))
			strcat_s((char *)sPath, MAX_FILE_PATH, "c:\\Office\\PumpSrv\\Data");

		char sFileName[MAX_FILE_PATH] = { 0 };
		sprintf_s(sFileName, MAX_FILE_PATH, "LoyaltySummary_%ld_%ld.xml", lPumpNumber, lTrsNum);
		strcat_s((char *)sPath, MAX_FILE_PATH, "\\");
		strcat_s((char *)sPath, MAX_FILE_PATH, (char *)sFileName);

		char sLogMsg[MAX_MSG_LEN]{ 0 };
		sprintf_s(sLogMsg, "Saving loyalty summary %s", sPath);
		_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, sLogMsg);

		xmlInitDoc.SaveToFile(sPath);
	}
	catch (...)
	{
		_Module.m_server.m_cLogger.LogMsg("CEmeraldLoyaltyCommandExecuter::AddItem Unexpected Error Had occurred", LOG_LEVEL_1); // 1.0.23.980
		bRetCode = FALSE;
	}

	xmlInitDoc.CloseDoc();

	// skip loyalty response for addItem
	ola.m_byState = OLA_NEW_STATE(ola.m_byState, OLA_LOYALTY_END_TICKET); //SESSION_COMPLETE | OLA_REQUEST_TO_SEND;
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "CEmeraldLoyaltyCommandExecuter::AddItem, Emerald flow - move to complete seesion");

	return bRetCode;
}

long CEmeraldLoyaltyCommandExecuter::EndTicket(long lPumpNumber, long lFlag)
{
	long lPumpIndex = lPumpNumber - 1;
	char sPath[MAX_FILE_PATH] = { 0 };
	// RFUEL-2065 - no need to set or clear the end ticket timer.
	//delete loyalty customer file after end ticket
	if (CFuelLoyaltyHelper::GetLoyaltyCustomerFileName(sPath, "", lPumpNumber))
	{
		if (DeleteFile(sPath))
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "CEmeraldLoyaltyCommandExecuter::EndTicket   CustomerInfo file was Delete");
		else
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "CEmeraldLoyaltyCommandExecuter::EndTicket   CustomerInfo file Deletetion failed !!");
	}
	// 
	// 	// move to complete
	// 	OLA_STAT ola;
	// 	_Module.m_server.m_cPumpArray[lPumpIndex].GetOlaStat(&ola);
	// 	ola.m_byState = OLA_NEW_STATE(ola.m_byState, OLA_LOYALTY_END_TICKET);
	// 	_Module.m_server.m_cPumpArray[lPumpIndex].SetOlaStat(&ola);
	// 	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "CEmeraldLoyaltyCommandExecuter::EndTicket - starting complete process for pap");


	return 0;
}



// MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
BOOL CEmeraldLoyaltyCommandExecuter::CreateAddItemXML(CComBSTR& bstrOutXml, PumpItems& cItems, long lPumpNumber, CPumpTransact trs, long lTotalDiscount)
{
	BOOL bRet = TRUE;
	CString strMsg;
	long lPumpIndex = lPumpNumber - 1;
	CXMLInterface xmlInitDoc, cXmlCustomer;
	CXMLPointers  pLoyaltyInfomration, pAllPromElement, pPromoSrvElement, pPromoSrvElements, pTriggerPromElement, pTriggeredPromElements, pTriggeredPromoyionElement, pCustomerId, pTmpNode, pRoot, pItem, pItemList, pCustomerInfo;

	try
	{
		long bRet = xmlInitDoc.InitDoc();
		if (bRet)
			bRet = xmlInitDoc.CreateElement(XMLSTR_LoyaltyInformation, &pLoyaltyInfomration);
		if (bRet)
			bRet = xmlInitDoc.AppendRootElement(&pLoyaltyInfomration);
		if (bRet)
		{
			bRet = xmlInitDoc.CreateElement(XMLSTR_TriggerProm, &pTriggerPromElement);
			xmlInitDoc.AppendChild(&pTriggerPromElement, &pLoyaltyInfomration);
		}
		if (bRet)
			bRet = xmlInitDoc.CreateElement("AllProm", &pAllPromElement);

		if (bRet)
		{
			xmlInitDoc.AppendChild(&pAllPromElement, &pTriggerPromElement);
			xmlInitDoc.SetAttribute(XMLSTR_PumpNumber, lPumpNumber, &pAllPromElement);

			bRet = xmlInitDoc.CreateElement(XMLSTR_PromoSrvMessage, &pPromoSrvElement);
			if (bRet)
				xmlInitDoc.AppendChild(&pPromoSrvElement, &pAllPromElement);

			bRet = xmlInitDoc.CreateElement(XMLSTR_TriggeredPromElements, &pTriggeredPromElements);
			if (bRet)
				xmlInitDoc.AppendChild(&pTriggeredPromElements, &pPromoSrvElement);

			long lCount = 0; double fAmount = 0;
			long lGradePlu = _Module.m_server.m_cFuelLoayaltyPS.GetNacsCodeByGradeCode(cItems, trs.m_nGrade);
			INT64 lAdditionalCode = _Module.m_server.m_cFuelLoayaltyPS.GetAdditionalCodeByGradeCode(cItems, trs.m_nGrade);  //  RFUEL-2651 In FuelTransactionData Fuel sends the grade in the PLUcode while Emerald needs the AdditionalCode.

			//if (LOG_FCC_FLOW_CONTROL)
			{
				strMsg.Format("CEmeraldLoyaltyCommandExecuter::CreateAddItemXML    Getting triggered promotions for grade plu [%d]", lGradePlu);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg.GetBuffer());
			}
			for (int iPos = 0; iPos < MAX_POSITIONS_PER_PUMP; iPos++)
			{
				double fTotalCentsOff = _Module.m_server.m_cFuelLoayaltyPS.GetTotalCentsOff(lPumpIndex, iPos);
				PromotionList promotionList = _Module.m_server.m_cFuelLoayaltyPS.GetPromotionsListByPosition(lPumpIndex, iPos);
				for each (CPromotion cPromo in promotionList)
				{
					long lPluCode = cPromo.GetPluCode();
					if (lPluCode == lGradePlu)
					{
						bRet = xmlInitDoc.CreateElement(XMLSTR_TriggeredPromElement, &pTriggeredPromoyionElement);
						if (bRet)
							bRet = xmlInitDoc.AppendChild(&pTriggeredPromoyionElement, &pTriggeredPromElements);
						if (bRet)
						{
							if (_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsDeferralDiscountInFlow() && cPromo.GetAskDeferral())   //RFUEL-1732  // RFUEL-2393

								fAmount = cPromo.GetRewardAmountDeferredReward();
							else
								fAmount = cPromo.GetCentsOff();  //RFUEL-2306
							long lAmount = fAmount * 10;  //RFUEL-3914 support Emerald interface (as they cannot have decimal point on amount value) changed to long and multiplied by 10
							xmlInitDoc.SetAttribute(XMLSTR_Amount, lAmount, &pTriggeredPromoyionElement);
							xmlInitDoc.SetAttribute(XMLSTR_PluCode, lPluCode, &pTriggeredPromoyionElement);
							xmlInitDoc.SetAttribute(XMLSTR_AdditionalCode, lAdditionalCode, &pTriggeredPromoyionElement);  // RFUEL-2651  In FuelTransactionData Fuel sends the grade in the PLUcode while Emerald needs the AdditionalCode.
							xmlInitDoc.SetAttribute(XMLSTR_Description, cPromo.GetsPromDescription().GetBuffer(), &pTriggeredPromoyionElement);
							xmlInitDoc.SetAttribute(XMLSTR_PromNumber, cPromo.GetID(), &pTriggeredPromoyionElement);

							if (cPromo.IsPromotionIncludeDeferralDiscount()) 
							{
								 xmlInitDoc.SetAttribute(XMLSTR_DeferrablePromotion, "1", &pTriggeredPromoyionElement); //YES
							}
							else
							{
								xmlInitDoc.SetAttribute(XMLSTR_DeferrablePromotion, "0", &pTriggeredPromoyionElement);//NO
							}
					//		if (LOG_FCC_FLOW_CONTROL)
							{
								strMsg.Format("CEmeraldLoyaltyCommandExecuter::CreateAddItemXML   Added triggered promotion id[%d] for grade plu [%d]", cPromo.GetID(),lGradePlu);
								_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg.GetBuffer());
							}

						}
						lCount++;
					}
				}
			}
			//add carwash promotions

			AddTicketPromotionsToXML(lPumpNumber,xmlInitDoc, pTriggeredPromElements, lCount, lTotalDiscount);

			if (lCount > 0)
			{
				xmlInitDoc.SetAttribute(XMLSTR_Count, lCount, &pPromoSrvElement);
				lTotalDiscount = lTotalDiscount / 10; // RFUEL-2306 - cut last 0 
				xmlInitDoc.SetAttribute(XMLSTR_TotalRewardAmount, lTotalDiscount, &pPromoSrvElement);
			}
		}
		_Module.m_server.m_cXmlParser.AddCustomerInformationToXML(lPumpNumber, xmlInitDoc, pLoyaltyInfomration,FALSE);   // RFUEL - 2628 Fuel do not send Customer information in PAP / PAK transaction with loyalty
    
		if (LOG_FCC_FLOW_CONTROL)
				xmlInitDoc.SaveToFile("c:\\OFFICE\\PUMPSRV\\LOG\\PS_AddItem.xml");
		xmlInitDoc.GetXmlString(&bstrOutXml);
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CEmeraldLoyaltyCommandExecuter::CreateAddItemXML, Unexpected error has occurred", LOG_LEVEL_1); //  4.0.23.980
	}

	xmlInitDoc.CloseDoc();

	return bRet;
}


//RFUEL - 2142

void CEmeraldLoyaltyCommandExecuter::GetCarWashDetalisFromCardSaleData(__int64 &i64PluCode, PAY_AT_PUMP_INFO &cTmpPapInfo, long &lUnitPrice, long &lQuantity, CString &sName)
{
	lUnitPrice = a2l((BYTE *)&cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice, sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice)) * 10;  //RFUEL-2514 Incorrect Carwash Price in RewardServiceRequest message sent to Emerald
	lQuantity = a2l((BYTE *)&cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sQty, sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sQty));
	long lProdCode = a2l(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode, sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode));
	// this line should contain the tax...  cTmpPapInfo.CardSaleInfo.extData.SALES[0].sTax
	i64PluCode = CFuelLoyaltyHelper::GetWashProgramNacsCodeByProgramCode(lProdCode);
	sName = CFuelLoyaltyHelper::GetNameOfCarWashProgramByID(cTmpPapInfo, lProdCode);
	CString strMsg;
	strMsg.Format("GetCarWashDetalisFromCardSaleData PluCode = %llu, Price = %ld, Quantity = %ld, barcode = %.11s , program = %s",  i64PluCode, lUnitPrice, lQuantity,  cTmpPapInfo.CardSaleExtraData6.CarWashItem.sBarcode , sName);
	_LOGMSG.LogMsg(0, LOG_PUMP, strMsg);
}


//Add carwash item to query XML which is to be sent to Emerald to get the valid promotions..
BOOL CEmeraldLoyaltyCommandExecuter::AddCarWashItemToXml(PAY_AT_PUMP_INFO &cTmpPapInfo, CComBSTR & sXMLOutData)
{
	long lUnitPrice, lQuantity;
	__int64 i64PluCode;
	CString sName;
	BOOL bRetCode = FALSE;
	GetCarWashDetalisFromCardSaleData(i64PluCode, cTmpPapInfo, lUnitPrice, lQuantity, sName);
	bRetCode = _Module.m_server.m_cXmlParser.CreateEmeraldAddCarWashItemXML(sXMLOutData, lUnitPrice, lQuantity, i64PluCode, sName);
	return bRetCode;
}


void CEmeraldLoyaltyCommandExecuter::AddTicketPromotionsToXML(long lPumpNumber, CXMLInterface &xmlInitDoc, CXMLPointers & pTriggeredPromElements, long	&lCount, long  &lTotalDiscount) // add carwash to xml returned to Emerald
{
	CString strMsg;

	CTicketPromotionlist promotionList = _Module.m_server.m_cFuelLoayaltyPS.GetTicketPromotionlist();
	CXMLPointers  pTriggeredPromoyionElement;
	BOOL bRet = FALSE;
	try
	{
		for each (CTicketPromotionPtr pTicketPromotionPtr in promotionList)
		{
			long lPluCode = pTicketPromotionPtr->GetPluCode();
			{
				bRet = xmlInitDoc.CreateElement(XMLSTR_TriggeredPromElement, &pTriggeredPromoyionElement);
				if (bRet)
					bRet = xmlInitDoc.AppendChild(&pTriggeredPromoyionElement, &pTriggeredPromElements);
				if (bRet)
				{   
					xmlInitDoc.SetAttribute(XMLSTR_TicketPromotionType, pTicketPromotionPtr->GetFieldValue(XMLSTR_TicketPromotionType).GetBuffer(), &pTriggeredPromoyionElement);
					// RFUEL - 3846 Discount should be multiplied by scalefactor.
					long lAmount = _Module.m_server.GetCarWashScalarFactor() * atol(pTicketPromotionPtr->GetFieldValue(XMLSTR_CarWashPromoDiscount).GetBuffer());   //RFUEL-2586   Incorrect TotalRewardAmount when a carwash item is bought along with fuelling and an extra TriggeredPromElement element
					lTotalDiscount += lAmount; // Accumulate the discounts
					xmlInitDoc.SetAttribute(XMLSTR_Amount, pTicketPromotionPtr->GetFieldValue(XMLSTR_CarWashPromoDiscount).GetBuffer(), &pTriggeredPromoyionElement);
					xmlInitDoc.SetAttribute(XMLSTR_PluCode, lPluCode, &pTriggeredPromoyionElement);
					xmlInitDoc.SetAttribute(XMLSTR_Description, pTicketPromotionPtr->GetFieldValue(XMLSTR_CarWashPromoName).GetBuffer(), &pTriggeredPromoyionElement);
					CString sCarWashPromoId = pTicketPromotionPtr->GetFieldValue(XMLSTR_CarWashPromoID).GetBuffer();
					xmlInitDoc.SetAttribute(XMLSTR_PromNumber, sCarWashPromoId.GetBuffer(), &pTriggeredPromoyionElement);
					strMsg.Format("AddTicketPromotionsToXML Amount of carwashpromo= %ld, sCarWashPromoId = %s", lAmount, sCarWashPromoId);
					_LOGMSG.LogMsg(0, LOG_PUMP, strMsg);

				}
				lCount++;
			}
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CEmeraldLoyaltyCommandExecuter", "AddTicketPromotionsToXML", lPumpNumber, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1);
	}

}


