// XmlParser.cpp: implementation of the CXmlParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XmlParser.h"
#include "FuelLoyaltyHelper.h"
#include "XMLConsts.h"
#include <sstream>



#define LOG_MSG(a,b) _Module.m_server.m_cLogger.LogClassMsg("CXmlParser", (a), -1, LOG_NONE, (b));
#define LOG_MSG_START_METHOD(a)   CString sFuncName=a;LOG_MSG(a, "start");
#define LOG_MSG_FUNCINC(a) LOG_MSG(sFuncName, a);
#define LOG_MSG_FUNCINC_RETVAL(a) CString sRet;sRet.Format("Return Value is %d",a) ;LOG_MSG(sFuncName, sRet);
#define LOG_MSG_END_METHOD   LOG_MSG(sFuncName, "end");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlParser::CXmlParser()
{

}

CXmlParser::~CXmlParser()
{

}


void CXmlParser::AddLineWithNulll(CXMLInterface *xmlInitDoc, CXMLPointers *pCurrElement, char *psAttributeName, BYTE *value, long lStrLen)
{
	unsigned int i =0;
	unsigned char  *tmpStr;
	BOOL	 bValid = TRUE;
	tmpStr = new unsigned char[lStrLen+1];
	
	tmpStr[lStrLen] = 0;
	memcpy(tmpStr,value,lStrLen);
	
	bValid =xmlInitDoc->SetCharAttribute(psAttributeName,(char *)tmpStr,pCurrElement);
	//TBC add log with failure on high log level
	delete[] tmpStr;
}


//************************************
// Method:    AddNullTerminatedNode
// FullName:  CXmlParser::AddNullTerminatedNode
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CXMLInterface * xmlInitDoc
// Parameter: CXMLPointers * pParrentElement
// Parameter: char * psAttributeName
// Parameter: BYTE * value
// Parameter: long lStrLen
// MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
//************************************
void CXmlParser::AddNullTerminatedNode(CXMLInterface *xmlInitDoc, CXMLPointers *pParrentElement, char *psAttributeName, BYTE *value, long lStrLen)
{
	CXMLPointers pCurrElement;

	char tmpStr[MAX_PATH];
	memset(tmpStr, 0, MAX_PATH);
	memcpy(tmpStr, value, min(_countof(tmpStr), lStrLen));

	CString cStr = tmpStr;
	cStr.Trim();
	const char *pTmp = (LPCTSTR)cStr;

	BOOL bValid = FALSE;
	for (int i = 0; i < lStrLen; i++)
	{
		if (tmpStr[i] != ' ')
		{
			bValid = TRUE;
			break;
		}
	}
	if (bValid)
	{
		xmlInitDoc->CreateChildElement(psAttributeName, (char*)pTmp, &pCurrElement);
		xmlInitDoc->AppendChildElement(&pCurrElement, pParrentElement);
	}

}

/******************************************************************************
 Description:  Create the xml that will send to query

 Parameters:    iNumOfElement - Num Of grade in the pumps
				sCardSaleAll  - Hold all the info about the grades				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			11/03/2009	13:30		Create xml to query for cents off //4.0.22.504
******************************************************************************/
BOOL CXmlParser::CreateQueryXML(long lPumpNumber , PumpItems * cTmpInfo,char *  szStrOutXml,PAY_AT_PUMP_INFO & pInfo, BOOL bAddCarWashBeforeFueling)
{

	BOOL bRetCode = TRUE;
	int	 i = 0, j = 0;
	CXMLInterface xmlInitDoc, xmlAddItemDoc;
	CXMLPointers  pCurrElement,pCurrElement2,pCurrElement3,pCurrElement4, pCurrElement5, pSonAddItemElement, pFatherAddItemElement;
	CComBSTR	  bstrXML;
	long		  lAmount=0;//from Price *  Quantity
	try
	{
		bRetCode = xmlInitDoc.InitDoc();
		xmlInitDoc.CreateElement("STARTTICKET",&pCurrElement);
		xmlInitDoc.AppendRootElement(&pCurrElement);
		xmlInitDoc.CreateElement("PUMPSGRADE",&pCurrElement4);
		xmlInitDoc.AppendChild(&pCurrElement4,&pCurrElement);

		//Liat LPE - verify only one item per nacs
		for(i = cTmpInfo->lCounter - 1; i > 0; i--)
		{
			for(j = i-1; j >= 0; j--)
			{
				if ((cTmpInfo->m_cItemSoldInfo[i].lNACSCode == cTmpInfo->m_cItemSoldInfo[j].lNACSCode) && (cTmpInfo->m_cItemSoldInfo[i].lAdditionalCode == cTmpInfo->m_cItemSoldInfo[j].lAdditionalCode)) //4.0.23.990 232007 232007
					cTmpInfo->m_cItemSoldInfo[i].lNACSCode = -1;
			}
		}
		
		for(i = 0; i < cTmpInfo->lCounter; i++)
		{
			if (cTmpInfo->m_cItemSoldInfo[i].lNACSCode != -1)
			{
				lAmount = _Module.m_server.GetItemPrice(lPumpNumber , cTmpInfo , i); // 4.0.23.1052 TD 270145
				xmlInitDoc.CreateElement("GRADE",&pCurrElement2);
				xmlInitDoc.AppendChild(&pCurrElement2,&pCurrElement4);
				xmlInitDoc.SetIntAttribute("GradeID",i,&pCurrElement2);

				xmlInitDoc.CreateElement("Item",&pCurrElement3);
				xmlInitDoc.AppendChild(&pCurrElement3,&pCurrElement2);
				xmlInitDoc.SetIntAttribute("AdditionalCode",cTmpInfo->m_cItemSoldInfo[i].lAdditionalCode,&pCurrElement3);
				xmlInitDoc.SetIntAttribute("Grade",cTmpInfo->m_cItemSoldInfo[i].lNACSCode,&pCurrElement3);
				
	
				//4.0.20.610 72486 //4.0.22.500 
				long lOriginUnitPrice = _Module.m_server.GetItemPrice(lPumpNumber,cTmpInfo,i); // // 4.0.23.1052 TD 270145
				ChangePrecision(lOriginUnitPrice);

				xmlInitDoc.SetAttribute("Price",lOriginUnitPrice,&pCurrElement3);		
				xmlInitDoc.SetAttribute("Quantity","1000",&pCurrElement3);
				xmlInitDoc.SetAttribute("QuantityType","2",&pCurrElement3);
				xmlInitDoc.SetAttribute("Amount",lAmount,&pCurrElement3);
				xmlInitDoc.SetAttribute("DepCode",m_lDepCode,&pCurrElement3);	//Liat Copient 

				BYTE byNozzle = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetGradePosition(cTmpInfo->m_cItemSoldInfo[i].lNACSCode);
				xmlInitDoc.SetIntAttribute("Nozzle", byNozzle, &pCurrElement3);

				CString sGradeName = _Module.m_server.GetGradeName(cTmpInfo->m_cItemSoldInfo[i].lGrade);
				xmlInitDoc.SetAttribute("Name", sGradeName.GetBuffer(), &pCurrElement3);

				if (_Module.m_server.GetDynamicAttrForGradeID() > 0)
				{
					long lGradeCode = _Module.m_server.GetGradeCodeByNacsCode(*cTmpInfo,cTmpInfo->m_cItemSoldInfo[i].lNACSCode); //4.0.23.170 102319
	 				xmlInitDoc.SetIntAttribute("GradeCode",lGradeCode,&pCurrElement3);	//4.0.23.170 102319
				}
			}
		}

		pCurrElement4.m_pMSElement= NULL;
		pCurrElement4.m_pMSNode = NULL;

		xmlInitDoc.CreateElement("Track2Loyalty",&pCurrElement4);
		xmlInitDoc.AppendChild(&pCurrElement4,&pCurrElement);

		if(!(ChrAll((char*)pInfo.CardSaleInfo.extraData.sLoyalId,sizeof(pInfo.CardSaleInfo.extraData.sLoyalId),' ')))
		{//4.0.19.500 55286

			long iFirstSpace = 0;
			for(iFirstSpace=0; iFirstSpace < sizeof(pInfo.CardSaleInfo.extraData.sLoyalId); iFirstSpace++)
			{
				if (pInfo.CardSaleInfo.extraData.sLoyalId[iFirstSpace] == ' ')
					break;
			}

			AddLineWithNulll(&xmlInitDoc,&pCurrElement4,"trackNumber",(BYTE*)pInfo.CardSaleInfo.extraData.sLoyalId,iFirstSpace);
		}
		else
		{
			long lPumpNum = a2l(pInfo.CardSaleInfo.cardData.sPumpNumber, sizeof(pInfo.CardSaleInfo.cardData.sPumpNumber));
			
			xmlInitDoc.SetAttribute("trackNumber","",&pCurrElement4);
			_LOGMSG.LogMsg("CFuelLoyaltyPS","CreatePumpsGradeXML", lPumpNum, LOG_PUMP, "There's no loyalty card to send to Loyalty Server");	//4.0.20.0
		}


		xmlInitDoc.SetAttribute("PsAllSchemeActive", "0", &pCurrElement4);
		xmlInitDoc.SetAttribute("PsDummyCrdFlg", "0", &pCurrElement4);
		xmlInitDoc.SetAttribute("lSchemeNo", "2", &pCurrElement4);

		xmlInitDoc.CreateElement("CardSegment", &pCurrElement5);
		xmlInitDoc.AppendChild(&pCurrElement5, &pCurrElement);

		AddLineWithNulll(&xmlInitDoc, &pCurrElement5, "SegmentID", &pInfo.CardSaleExtraData3.sCardSegment, sizeof(pInfo.CardSaleExtraData3.sCardSegment));

		//MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
		CXMLPointers  pCardInformationElement;

		xmlInitDoc.CreateElement("CardInformation", &pCardInformationElement);
		xmlInitDoc.AppendChild(&pCardInformationElement, &pCurrElement);

		AddNullTerminatedNode(&xmlInitDoc, &pCardInformationElement, "ClubNumber", (BYTE*)&pInfo.CardSaleExtraData6.LoyaltyCard.sClubNumber, sizeof(pInfo.CardSaleExtraData6.LoyaltyCard.sClubNumber));
		AddNullTerminatedNode(&xmlInitDoc, &pCardInformationElement, "AccountNumber", (BYTE*)&pInfo.CardSaleExtraData6.LoyaltyCard.sAccountNumber, sizeof(pInfo.CardSaleExtraData6.LoyaltyCard.sAccountNumber));
		AddNullTerminatedNode(&xmlInitDoc, &pCardInformationElement, "ProgramLongName", (BYTE*)&pInfo.CardSaleExtraData6.LoyaltyCard.sProgramLongName, sizeof(pInfo.CardSaleExtraData6.LoyaltyCard.sProgramLongName));
		AddNullTerminatedNode(&xmlInitDoc, &pCardInformationElement, "ProgramShortName", (BYTE*)&pInfo.CardSaleExtraData6.LoyaltyCard.sProgramShortName, sizeof(pInfo.CardSaleExtraData6.LoyaltyCard.sProgramShortName));
		AddNullTerminatedNode(&xmlInitDoc, &pCardInformationElement, "CardType", (BYTE*)&pInfo.CardSaleExtraData6.LoyaltyCard.sCardType, sizeof(pInfo.CardSaleExtraData6.LoyaltyCard.sCardType));

		xmlInitDoc.GetXmlString(&bstrXML);

		int iLen = Bstr2Buffer(bstrXML, szStrOutXml, MAX_BUF);
		if (iLen == 0)
			_LOGMSG.LogMsg("CFuelLoyaltyPS::CreateQueryXML - Bstr2Buffer fail!", LOG_LEVEL_1); //  4.0.23.980

		if (LOG_FCC_FLOW_CONTROL)
			xmlInitDoc.SaveToFile("c:\\LPE\\XML\\PS_StartTicket.xml");


	}
		
	
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","CreateQueryXML",0,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
	}

	xmlInitDoc.CloseDoc();
	return bRetCode;
}


//4.0.19.502 TD 55286
//Non fuel item (car wash) doesn't have the force operation
BOOL CXmlParser::CreateAddCarWashItemXML(PAY_AT_PUMP_INFO & cTmpPapInfo, char *szStrOutXml, long	lPumpNumber)
{
	BOOL bRetCode = TRUE;
	int	 i = 0;
	CXMLInterface xmlInitDoc;
	CXMLPointers  pCurrElement,pChildNode2,pCurrElement3,pChildNode,pCurrElement4;
	CComBSTR	  bstrXML;
	long lNacsCode = 0;
	long lPromID=0,lCentsOff=0;
	long lAmout=1, lPrice = 0, lQuantity = 1;
	CString strMsg;
	
	CAR_WASH_INFO info;
	memset(&info,0,sizeof(CAR_WASH_INFO));
	try
	{
		///4.0.20.420 TD - 68979 - we need to take the CCH_Product_Code from the program	
		long lProdCode = a2l(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode, sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode));
		
		lNacsCode = CFuelLoyaltyHelper::GetWashProgramNacsCodeByProgramCode(lProdCode);

		lPrice = a2l((BYTE *)&cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice,sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice));

		lQuantity = a2l((BYTE *)&cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sQty,sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sQty));

		strMsg.Format("CFuelLoyaltyPS::CreateAddNonFuelItemXML Pump = %ld, NacsCode = %ld, Price = %ld, Quantity = %ld", lPumpNumber, lNacsCode, lPrice, lQuantity);

		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
		if (lPrice <= 0)
		{	
			bRetCode = FALSE;
		}
		
		if (bRetCode)
		{
			lAmout = lPrice;	//4.0.19.508
			bRetCode = xmlInitDoc.InitDoc();
			xmlInitDoc.CreateElement("ADDITAM",&pCurrElement);
			xmlInitDoc.AppendRootElement(&pCurrElement);
			xmlInitDoc.CreateElement("PUMPSPROP",&pCurrElement3);
			xmlInitDoc.AppendChild(&pCurrElement3,&pCurrElement);
			
			xmlInitDoc.SetAttribute("PluCode",lNacsCode,&pCurrElement3);
			xmlInitDoc.SetAttribute("Grade",lNacsCode,&pCurrElement3);	//4.0.23.950 216343
			//TBD Copient
			
			xmlInitDoc.SetAttribute("Amount", lAmout, &pCurrElement3); 
			xmlInitDoc.SetAttribute("Price", lPrice, &pCurrElement3);
			xmlInitDoc.SetAttribute("Quantity", lQuantity, &pCurrElement3);
			xmlInitDoc.SetIntAttribute("QuantityType",QUANTITY_TYPE_COUNTED,&pCurrElement3);
			long lFuelItem = 0;
			xmlInitDoc.SetIntAttribute("FuelItem",lFuelItem,&pCurrElement3); //Meaning it's not a fuel item //4.0.19.502 TD 55286
		}
		
		xmlInitDoc.GetXmlString(&bstrXML);

		int iLen = Bstr2Buffer(bstrXML,szStrOutXml,MAX_BUF); //  4.0.23.980
		if(iLen == 0)
			_LOGMSG.LogMsg("CFuelLoyaltyPS::CreateAddCarWashItemXML - Bstr2Buffer fail!" , LOG_LEVEL_1);
		
		if (LOG_FCC_FLOW_CONTROL)
			xmlInitDoc.SaveToFile("c:\\LPE\\XML\\PS_AddCarWashItem.xml");
		
		
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS::CreateAddNonFuelItemXML, Unexpected error has occurred", LOG_LEVEL_1); // 4.0.23.980
		bRetCode = FALSE;
	}
	
	xmlInitDoc.CloseDoc();
	
	strMsg.Format("CFuelLoyaltyPS::CreateAddNonFuelItemXML was finished with result %s", (bRetCode == TRUE) ? "PASS":"FAILED");
	_LOGMSG.LogMsg(strMsg);
	
	return bRetCode;
}


/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			09/09/2012       		Start			4.0.23.???? TD 305114
******************************************************************************/
void CXmlParser::CreateAddMemberInfoXml(char *  szStrOutXml)
{
	CXMLInterface xmlInitDoc;
	CXMLPointers  pPromoSrvNode,pSegmentsNode,pLoyaltyInfoNode,pCardSegementNode;
	try
	{
		xmlInitDoc.InitDoc();

		xmlInitDoc.CreateElement("PromoSrvMessage",&pPromoSrvNode);
		xmlInitDoc.AppendRootElement(&pPromoSrvNode);

		xmlInitDoc.CreateElement("LoyaltyInfo",&pLoyaltyInfoNode);
		xmlInitDoc.AppendChild(&pLoyaltyInfoNode,&pPromoSrvNode);

		xmlInitDoc.CreateElement("Segments",&pSegmentsNode);
		xmlInitDoc.AppendChild(&pSegmentsNode,&pLoyaltyInfoNode);

		xmlInitDoc.CreateElement("CardSegment",&pCardSegementNode);
		xmlInitDoc.SetIntAttribute("SegmentID",300,&pCardSegementNode);
		xmlInitDoc.AppendChild(&pCardSegementNode,&pSegmentsNode);

		CComBSTR	bstrXML;
		xmlInitDoc.GetXmlString(&bstrXML);
		
		int iLen = Bstr2Buffer(bstrXML,szStrOutXml,bstrXML.Length()); 
		if(iLen == 0)
		{
			_LOGMSG.LogMsg("CFuelLoyaltyPS::CreateAddMemberCardXML - Bstr2Buffer fail!" , LOG_LEVEL_1); //  4.0.23.980
		}
		
		if (LOG_FCC_FLOW_CONTROL)
			xmlInitDoc.SaveToFile("c:\\LPE\\XML\\PS_AddMember.xml");
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","CreateAddMemberCardXML",0,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
	}
	
	
	xmlInitDoc.CloseDoc();
}


/******************************************************************************
 Description: Parse Received Target, set the text into sOutText

 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Omer			22/04/2012	 19:34		Start 312183
******************************************************************************/
BOOL CXmlParser::ParseTargetMessage(char * sTargetMessageXml, char * sOutText,int iBufferSize)
{
	BYTE    sText[MAX_TERMINAL_MESSAGE_LENGTH] ={0};
	BOOL	bRetCode = TRUE;

	CXMLInterface xmlInitDoc;

	BOOL bFoundMessage = FALSE;

	try
	{
		xmlInitDoc.InitDoc();
		xmlInitDoc.LoadXmlString(sTargetMessageXml);

		//if (LOG_FCC_FLOW_CONTROL)
		xmlInitDoc.SaveToFile("C:\\lpe\\xml\\TargetMessage.xml"); // TD 319664

		CXMLPointers pPromoSrvMsgNode,pCurrElement,pNode,pChildNode, pListNodes, pCurrPrintElement, pCurrSectionElement, pCurrDocumentElement, pListPrintNodes, pListSectionNodes, pListDocumentNodes;

		bRetCode = xmlInitDoc.GetRootNode(&pPromoSrvMsgNode);

		xmlInitDoc.SelectSingleElementNode("PrintMessages",&pPromoSrvMsgNode);

		xmlInitDoc.GetChildNodesList(&pPromoSrvMsgNode, &pListNodes);

		if (pListNodes.m_pMSListOfItems)
		{
			while (xmlInitDoc.GetNextItem(pListNodes,pCurrElement))	//PrintMessages elements
			{
				xmlInitDoc.GetChildNodesList(&pCurrElement, &pListPrintNodes);

				if (pListPrintNodes.m_pMSListOfItems)
				{
					while (xmlInitDoc.GetNextItem(pListPrintNodes,pCurrPrintElement)) //PrintMessage elements
					{
						xmlInitDoc.GetChildNodesList(&pCurrPrintElement, &pListDocumentNodes);
						if (pListDocumentNodes.m_pMSListOfItems)
						{
							while (xmlInitDoc.GetNextItem(pListDocumentNodes,pCurrDocumentElement)) //Document elements
							{
								xmlInitDoc.GetChildNodesList(&pCurrDocumentElement, &pListSectionNodes);
								if (pListSectionNodes.m_pMSListOfItems)
								{
									while (xmlInitDoc.GetNextItem(pListSectionNodes,pCurrSectionElement) ) //Section elements
									{
										bRetCode = xmlInitDoc.SelectSingleElementNode("Object", &pNode, &pCurrSectionElement); 
										if (bRetCode)
										{
											xmlInitDoc.GetChildNodesList(&pNode, &pListNodes);
											if (pListNodes.m_pMSListOfItems)
											{
												while (xmlInitDoc.GetNextItem(pListNodes,pCurrElement))	//TextAttributes elements
												{
													memset(sText, 0, sizeof(sText)); // 4.0.32.486
													xmlInitDoc.SelectSingleElementNode("Text", &pChildNode, &pCurrElement);
													xmlInitDoc.GetAttributeToByteArrWithConvert("Text",sText,&pChildNode);
													strncat(sOutText,(const char *)sText,iBufferSize);
													strncat(sOutText,(const char *)"|",iBufferSize);
													bFoundMessage = TRUE;
												}
												break; // end the loop here, after we finish the parsing the string.
											}
										}
									}	
								}
							}
						}
					}
				}
				else
				{
					bRetCode = FALSE;
				}
			}
		}
		else
			bRetCode = FALSE;
	
		CString str;
		str.Format("CXmlParser::ParseTargetMessage Parsed Target Message %s",bFoundMessage ? "successfully":"failed!",LOG_LEVEL_0);
		_Module.m_server.m_cLogger.LogMsg(str);
	}
	catch(...)
	{
		_Module.m_server.m_cLogger.LogMsg("CFuelLoyaltyPS::ParseTargetMessage, Unexpected error has occurred",LOG_LEVEL_0);
		bRetCode = FALSE;
	}

	return bFoundMessage;
}


void CXmlParser::ParseAutoStartFlag( long lPumpNumber ,long lPumpAutoStart, CXMLInterface &xmlInitDoc, CXMLPointers pPumpAutoStartNode, PAY_AT_PUMP_INFO &cTmpInfo)
{
	long lAutoStartFlag = 0;		//4.0.23.1202 89149 

	if (lPumpAutoStart)				//4.0.23.1202 89149 
	{
		lAutoStartFlag = xmlInitDoc.GetAttribute("Result",&pPumpAutoStartNode);	//1 or 0
	}

	l2a(lAutoStartFlag, &cTmpInfo.CardSaleExtraData5.sPumpAutoStart, sizeof(cTmpInfo.CardSaleExtraData5.sPumpAutoStart));
	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("CXmlParser::ParseAutoStartFlag, Auto start flag = %ld", lAutoStartFlag);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
	}
}

void CXmlParser::BuildApprovalCommunicationMessage( CXMLInterface &xmlInitDoc,long lPromID)
{
	CXMLPointers pForcePromotionElement,pCurrElement,pPromotionsNode,pPromDataNode,pChildNode,pChildNode3;
	
	//xmlInitDoc.CreateElement("ADDITAM",&pCurrElement);
	//xmlInitDoc.AppendRootElement(&pCurrElement);

	//xmlInitDoc.CreateElement("FORCE_PROP",&pForcePromotionElement);
	//xmlInitDoc.AppendChild(&pForcePromotionElement,&pCurrElement);

	xmlInitDoc.CreateElement("PromoSrvMessage",&pCurrElement);
	xmlInitDoc.AppendRootElement(&pCurrElement);

	//xmlInitDoc.AppendChild(&pCurrElement,&pForcePromotionElement);
	xmlInitDoc.CreateElement("ApprovalCommunicationMessage",&pChildNode);
	xmlInitDoc.AppendChild(&pChildNode,&pCurrElement);
	xmlInitDoc.CreateElement("Promotions",&pPromotionsNode);
	xmlInitDoc.AppendChild(&pPromotionsNode,&pChildNode);

	xmlInitDoc.CreateElement("ResponseMode",&pChildNode3);
	xmlInitDoc.AppendChild(&pChildNode3,&pPromotionsNode);
	xmlInitDoc.SetAttribute("FullRedemptionsResponse","0",&pChildNode3);
	xmlInitDoc.CreateElement("PromotionData",&pPromDataNode);
	xmlInitDoc.AppendChild(&pPromDataNode,&pChildNode3);
	xmlInitDoc.SetAttribute("PromNumber",lPromID,&pPromDataNode);
	xmlInitDoc.SetAttribute("NumberOfSaversConfirmedByCashier","1",&pPromDataNode);
	xmlInitDoc.SetAttribute("NumberOfSaversRejectByCashier","0",&pPromDataNode);
	xmlInitDoc.SetAttribute("NumberOfSaversSuspendByCashier","0",&pPromDataNode);

}

// TD 4.0.32.220 405631
BOOL CXmlParser::ParseLoyaltySummaryXml(char * szLoyaltyFile, long & lPointsEarned, long &lOpenBalance, long & lEndningBalance)
{
	CString sMsg;
	sMsg.Format("CXmlParser::ParseLoyaltySummaryXml: looking for virtual account = %d", _Module.m_server.m_cFuelLoayaltyPS.m_dwVirtualMemberAccountId);
	_LOGMSG.LogMsg(sMsg); // TD 454424

	CXMLPointers pAccounts,pAcc ,pListNodes,pAccElement;

	CXMLInterface xmlInitDoc;

	xmlInitDoc.InitDoc();

	BOOL bRetCode = xmlInitDoc.LoadFile(szLoyaltyFile);

	if(bRetCode)
		bRetCode = xmlInitDoc.SelectSingleElementNode("Accounts",&pAccounts);

	if(bRetCode)
	{
		xmlInitDoc.GetChildNodesList(&pAccounts, &pListNodes);

		if (pListNodes.m_pMSListOfItems)
		{
			while (xmlInitDoc.GetNextItem(pListNodes,pAccElement)) // "Acc"
			{
				long lId = xmlInitDoc.GetAttribute("ID", &pAccElement);

				if (lId == _Module.m_server.m_cFuelLoayaltyPS.m_dwVirtualMemberAccountId)  // TD 454424
				{ 
					BYTE sEndingBalance[50] = { 0 };
					xmlInitDoc.GetAttributeToByteArrWithConvert("EndingBalance", sEndingBalance, &pAccElement);
					long lEndBalance = a2l(sEndingBalance, sizeof(sEndingBalance));
					if (lEndBalance > 0)
					{
						lEndningBalance += lEndBalance;
						CString sMsg;
						sMsg.Format("Loyalty summary: Found account number %d with EndingBalance balance = %d, total ending balance = %d", lId, lEndBalance, lEndningBalance);
						_LOGMSG.LogMsg(sMsg);
					}
					break;
				}

			}
		}
		else 
			return FALSE;
	}

	return bRetCode;
}




/******************************************************************************
Description: Parse Received Member info, found card with status 12
			 and save to card sale data

Returns:

Parameters:

WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Omer			02/05/2016	 19:34		Start 312183
******************************************************************************/
BOOL CXmlParser::ParseGetMemberInfoMessage(char * sGetMemberInfoXml,PAY_AT_PUMP_INFO *pInfo)
{
	BOOL	bRetCode = TRUE;
	CXMLInterface xmlInitDoc;
	BOOL bFoundAccount = FALSE;
	CString str;

	try
	{
		xmlInitDoc.InitDoc();
		xmlInitDoc.LoadXmlString(sGetMemberInfoXml);

		//if (LOG_FCC_FLOW_CONTROL)
		xmlInitDoc.SaveToFile("C:\\lpe\\xml\\GetMemberInfoXml.xml"); // TD 319664

		CXMLPointers pPromoSrvMsgNode, pCustomer, pLoyaltyInfo, pMembers, pListMembersNodes, pMember, pCards, pListCardNodes, pCard;

		bRetCode = xmlInitDoc.GetRootNode(&pPromoSrvMsgNode);

		if (bRetCode)
			bRetCode = xmlInitDoc.SelectSingleElementNode("Customer", &pPromoSrvMsgNode);
		if (bRetCode)
			xmlInitDoc.SelectSingleElementNode("LoyaltyInfo", &pLoyaltyInfo, &pPromoSrvMsgNode);
		if (bRetCode)
			xmlInitDoc.SelectSingleElementNode("Members", &pMembers, &pLoyaltyInfo);

		xmlInitDoc.GetChildNodesList(&pMembers, &pListMembersNodes);

		if (pListMembersNodes.m_pMSListOfItems)
		{
			while (xmlInitDoc.GetNextItem(pListMembersNodes, pMember) && FALSE == bFoundAccount)	//MemberElement
			{
				xmlInitDoc.SelectSingleElementNode("Cards", &pCards, &pMember);

				xmlInitDoc.GetChildNodesList(&pCards, &pListCardNodes);

				if (pListCardNodes.m_pMSListOfItems)
				{
					while (xmlInitDoc.GetNextItem(pListCardNodes, pCard)) 
					{
						long lCardSatus = xmlInitDoc.GetAttribute("CardStatus", &pCard);
						if (lCardSatus == 12)
						{
							//  4.0.32.??? TD 474997
							xmlInitDoc.GetAttributeToByteArrWithConvert("Id", pInfo->CardSaleInfo.extraData.sLoyalId, &pCard, 1);
							bFoundAccount = TRUE;
							str.Format("CXmlParser::ParseGetMemberInfoMessage found member card XXXXXXXXXXXXXXXX%.4s", pInfo->CardSaleInfo.extraData.sLoyalId+16);
							_Module.m_server.m_cLogger.LogMsg(str);
							break;
						}
					}
				}
			}
		}
		else
			bRetCode = FALSE;

		
		str.Format("CXmlParser::ParseGetMemberInfoMessage Found card with status 12 %s", bFoundAccount ? "successfully" : "failed!");
		_Module.m_server.m_cLogger.LogMsg(str);
	}
	catch (...)
	{
		_Module.m_server.m_cLogger.LogMsg("CFuelLoyaltyPS::ParseGetMemberInfoMessage, Unexpected error has occurred", LOG_LEVEL_0);
		bRetCode = FALSE;
	}

	return bFoundAccount;
}


void CXmlParser::ParseStartTikcketTime(long lPumpNumber, CXMLInterface &xmlInitDoc)
{
	CXMLPointers  pNode;
	long lRetCode = xmlInitDoc.SelectSingleElementNode("StartTicketTime", &pNode);

	if (lRetCode)
	{
		SYSTEMTIME st;
		st.wYear = xmlInitDoc.GetAttribute("Year", &pNode);
		st.wMonth = xmlInitDoc.GetAttribute("Month", &pNode);
		st.wDay = xmlInitDoc.GetAttribute("Day", &pNode);
		st.wDayOfWeek = xmlInitDoc.GetAttribute("DayOfWeek", &pNode);
		st.wHour = xmlInitDoc.GetAttribute("Hour", &pNode);
		st.wMinute = xmlInitDoc.GetAttribute("Minute", &pNode);
		st.wSecond = xmlInitDoc.GetAttribute("Second", &pNode);

		CString sMsg;
		sMsg.Format("Found start ticket time: %02d-%02d-%04d %02d:%02d:%02d",
			st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);


		_Module.m_server.m_cFuelLoayaltyPS.SetStartTicketTime(lPumpNumber - 1, st);

		_LOGMSG.LogMsg("CXmlParser", "ParseStartTikcketTime", lPumpNumber, LOG_PUMP, sMsg);	//4.0.20.0		
	}





}

/******************************************************************************
Description: Parse ParseRemainingValueToTrigger

Returns:

Parameters:

WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Omer			06/07/2015	 19:34		// TD 466954
******************************************************************************/
BOOL CXmlParser::ParseRemainingValueToTrigger(IN CComBSTR bstrOutIncentiveMessage, OUT long & lRemainingValueToTrigger)
{
	BOOL	bRetCode = TRUE;
	CXMLInterface xmlInitDoc;
	CXMLPointers pPrintMessagesMode, pPromMessagesNode, pPrintMessageNode;

	try
	{
		char sIncentiveMessageXml[MAX_XML_BUF] = { 0 };
		Bstr2Buffer(bstrOutIncentiveMessage, sIncentiveMessageXml, bstrOutIncentiveMessage.Length() + 2);

		xmlInitDoc.InitDoc();
		xmlInitDoc.LoadXmlString(sIncentiveMessageXml);

		bRetCode = xmlInitDoc.SelectSingleElementNode("PrintMessages", &pPrintMessagesMode);

		if (bRetCode)
			bRetCode = xmlInitDoc.SelectSingleElementNode("PromMessages", &pPromMessagesNode, &pPrintMessagesMode);

		if (bRetCode)
			bRetCode = xmlInitDoc.SelectSingleElementNode("PrintMessage", &pPrintMessageNode, &pPromMessagesNode);

		if (bRetCode)
		{
			lRemainingValueToTrigger = xmlInitDoc.GetAttribute("RemainingValueToTrigger", &pPrintMessageNode); // TD 466954
			char sMsg[MAX_MSG_LEN] = { 0 };
			sprintf_s(sMsg, MAX_MSG_LEN, "CFuelLoyaltyPS::ParseRemainingValueToTrigger reveived remainingValueToTrigger %d", lRemainingValueToTrigger);
			_Module.m_server.m_cLogger.LogMsg(sMsg);
		}
	}
	catch (...)
	{
		_Module.m_server.m_cLogger.LogMsg("CFuelLoyaltyPS::ParseRemainingValueToTrigger, Unexpected error has occurred", LOG_LEVEL_0);
		bRetCode = FALSE;
	}

	return bRetCode;
}



/******************************************************************************
Description:	Create Message  For The Emerald Query
Returns:
Parameters:  strXMLInData - contain CARD_SALE_DATA
We check the xml and get from him all the data that we need
strXMLOutData - the xml that send to Emerald

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
MichaelPo	27/5/019				Start
******************************************************************************/
BOOL CXmlParser::CreateEmeraldFuelProductDataXML(BSTR& sXMLInData, CComBSTR& sXMLOutData, long lRequestId, long lPumpNumber, EmeraldLoyaltyRequestType eRequestType /*not in use to be deleted*/)
{
	CXMLInterface  xmlInDoc, xmlOutDoc;

	BOOL bRetCode = xmlOutDoc.InitDoc();

	CXMLPointers  pPumpsGradeElement, pItemElement, pItemDetailsElement, pCardInformationElement;

	CXMLPointers  pRootElement, pFuelProductsElement;

	bRetCode = xmlOutDoc.CreateElement("RewardServiceRequest", &pRootElement);
	bRetCode = xmlOutDoc.AppendRootElement(&pRootElement);

	bRetCode = xmlOutDoc.SetLongAttribute("RequestID", lRequestId, &pRootElement);

	bRetCode = xmlOutDoc.SetIntAttribute("PumpId", lPumpNumber, &pRootElement);

	bRetCode = xmlOutDoc.SetCharAttribute("RequestType", LOYALTY_EMERALD_QUERY_REQUEST, &pRootElement); 

	bRetCode = xmlOutDoc.CreateElement("FuelProducts", &pFuelProductsElement);
	bRetCode = xmlOutDoc.AppendChild(&pFuelProductsElement, &pRootElement);

	char tmpStr[MAX_TABLE_NAME] = { 0 };

	try
	{
		_bstr_t bstInXML(sXMLInData, TRUE);

		bRetCode = xmlInDoc.InitDoc();
		bRetCode = xmlInDoc.LoadXmlString(bstInXML);

		bRetCode = xmlInDoc.SelectSingleElementNode("PUMPSGRADE", &pPumpsGradeElement);
		xmlInDoc.GetChildNodesList(&pPumpsGradeElement, &pItemElement);

		//loop to create all the sale struct
	while (xmlInDoc.GetNextItem(pItemElement, pPumpsGradeElement))
		{
			CXMLPointers pFuelProductElement, pNozzleElement, pGradeElement, pAdditionalCodeElement, pAmountEmeraldElement, pNameElement, pMaxAllowedDiscountElement;

			bRetCode = xmlInDoc.SelectSingleElementNode("Item", &pItemDetailsElement, &pPumpsGradeElement);

			bRetCode = xmlOutDoc.CreateElement("FuelProduct", &pFuelProductElement);
			bRetCode = xmlOutDoc.AppendChild(&pFuelProductElement, &pFuelProductsElement);

			long lNozzle = xmlInDoc.GetAttribute("Nozzle", &pItemDetailsElement);
			_itoa_s((int)lNozzle, tmpStr, _countof(tmpStr), 10);
			bRetCode = xmlOutDoc.CreateChildElement("Nozzle", (char*)tmpStr, &pNozzleElement);
			bRetCode = xmlOutDoc.AppendChildElement(&pNozzleElement, &pFuelProductElement);
			/*bRetCode = xmlInDoc.CreateElement("Nozzle", &pNozzleElement);
			bRetCode = xmlOutDoc.AppendChild(&pNozzleElement, &pFuelProductElement);
			bRetCode = xmlOutDoc.SetAttribute("", (char*)tmpStr, &pNozzleElement);*/

			memset(tmpStr, 0, sizeof(tmpStr));
			long lGrade = xmlInDoc.GetAttribute("Grade", &pItemDetailsElement);
			_itoa_s((int)lGrade, tmpStr, _countof(tmpStr), 10);
			bRetCode = xmlOutDoc.CreateChildElement("Grade", (char *)tmpStr, &pGradeElement);
			bRetCode = xmlOutDoc.AppendChildElement(&pGradeElement, &pFuelProductElement);

			memset(tmpStr, 0, sizeof(tmpStr));
			xmlInDoc.GetAttributeToByteArrWithConvert("AdditionalCode", tmpStr, &pItemDetailsElement);
			bRetCode = xmlOutDoc.CreateChildElement("AdditionalCode", (char *)tmpStr, &pAdditionalCodeElement);
			bRetCode = xmlOutDoc.AppendChildElement(&pAdditionalCodeElement, &pFuelProductElement);

			memset(tmpStr, 0, sizeof(tmpStr));
			long lPrice = xmlInDoc.GetAttribute("Amount", &pItemDetailsElement);  

			stringstream ssFloat;
			ssFloat.precision(3);
			ssFloat.setf(ios::fixed, ios::floatfield);

			ssFloat.str("");
			ssFloat << (float)lPrice / (float)1000;
			string sPrintedAmount(ssFloat.str().c_str());

			bRetCode = xmlOutDoc.CreateChildElement("Price", (char *)sPrintedAmount.c_str(), &pAmountEmeraldElement);
			bRetCode = xmlOutDoc.AppendChildElement(&pAmountEmeraldElement, &pFuelProductElement);
			
			//RFUEL-3309 Get the maximal discount allowed for the grade 
			long lGradeIndx = _Module.m_server.GetGradeIndex(lGrade);
			long lMaxAllowedDiscount = _Module.m_server.GetMaximalDiscountForGrade(lGradeIndx + 1);
			ssFloat.str("");
			ssFloat << (float)lMaxAllowedDiscount / (float)1000;
			string sMaxAllowedDiscount(ssFloat.str().c_str());
			bRetCode = xmlOutDoc.CreateChildElement(XMLSTR_MaxAllowedDiscount, (char *)sMaxAllowedDiscount.c_str(), &pMaxAllowedDiscountElement);
			bRetCode = xmlOutDoc.AppendChildElement(&pMaxAllowedDiscountElement, &pFuelProductElement);
			memset(tmpStr, 0, sizeof(tmpStr));
			xmlInDoc.GetAttributeToByteArrWithConvert("Name", tmpStr, &pItemDetailsElement);
			bRetCode = xmlOutDoc.CreateChildElement("Name", (char *)tmpStr, &pNameElement);
			bRetCode = xmlOutDoc.AppendChildElement(&pNameElement, &pFuelProductElement);
		}

		bRetCode = xmlInDoc.SelectSingleElementNode("CardInformation", &pCardInformationElement);
		bRetCode = xmlOutDoc.AppendChild(&pCardInformationElement, &pRootElement);

		if (LOG_FCC_FLOW_CONTROL)
			xmlInDoc.SaveToFile("c:\\LPE\\XML\\EmeraldQuery.xml");

		xmlOutDoc.GetXmlString(&sXMLOutData);
	}
	catch (...)
	{
		_Module.m_server.m_cLogger.LogMsg("CParserXML::CreateEmeraldFuelProductData Unexpected Error Had occurred", LOG_LEVEL_1); //1.0.19.810
		bRetCode = FALSE;
	}

	xmlInDoc.CloseDoc();
	xmlOutDoc.CloseDoc();

	return bRetCode;
}

// RFUEL - 2142 - Send selected car wash to loyalty only after wash selection and apply received promotions
BOOL CXmlParser::CreateEmeraldAddCarWashItemXML( CComBSTR& sXMLInOutData, long lUnitPrice, long	lQuantity, __int64 lPluCode, CString sName)
{
	CXMLInterface  xmlOutDoc;
	CXMLPointers  pDryProductsElement, pDryProductElement, pRootElement, pAdditionalCodeElement, pPriceElement , pQuantityElement , pNameElement;
	BOOL bRetCode = xmlOutDoc.InitDoc();

	char tmpStr[MAX_TABLE_NAME] = { 0 };

	try
	{
		_bstr_t bstInXML(sXMLInOutData, TRUE);
		bRetCode = xmlOutDoc.LoadXmlString(bstInXML);


		bRetCode = xmlOutDoc.CreateElement("DryProducts", &pDryProductsElement);
		bRetCode = xmlOutDoc.GetRootToElement(&pRootElement);

		if (bRetCode)
			bRetCode = xmlOutDoc.AppendChild(&pDryProductsElement, &pRootElement);
		if (bRetCode)
		{
			bRetCode = xmlOutDoc.CreateElement("DryProduct", &pDryProductElement);
			bRetCode = xmlOutDoc.AppendChild(&pDryProductElement, &pDryProductsElement);

			_itoa_s((int)lPluCode, tmpStr, _countof(tmpStr), 10);
			bRetCode = xmlOutDoc.CreateChildElement("AdditionalCode", (char*)tmpStr, &pAdditionalCodeElement);
			bRetCode = xmlOutDoc.AppendChildElement(&pAdditionalCodeElement, &pDryProductElement);

			_itoa_s((int)lUnitPrice, tmpStr, _countof(tmpStr), 10);
			bRetCode = xmlOutDoc.CreateChildElement("Price", (char*)tmpStr, &pPriceElement);
			bRetCode = xmlOutDoc.AppendChildElement(&pPriceElement, &pDryProductElement);

			bRetCode = xmlOutDoc.CreateChildElement("Name", sName.GetBuffer(), &pNameElement);
			bRetCode = xmlOutDoc.AppendChildElement(&pNameElement, &pDryProductElement);

			_itoa_s((int)lQuantity, tmpStr, _countof(tmpStr), 10);
			bRetCode = xmlOutDoc.CreateChildElement("Quantity", (char*)tmpStr, &pQuantityElement);
			bRetCode = xmlOutDoc.AppendChildElement(&pQuantityElement, &pDryProductElement);

		}

		if (LOG_FCC_FLOW_CONTROL)
			xmlOutDoc.SaveToFile("c:\\LPE\\XML\\PS_AddCarWashItem.xml");

		xmlOutDoc.GetXmlString(&sXMLInOutData);
	}
	catch (...)
	{
		_Module.m_server.m_cLogger.LogMsg("CParserXML::CreateEmeraldAddCarWashItemXML Unexpected Error Had occurred", LOG_LEVEL_1); //1.0.19.810
		bRetCode = FALSE;
	}

	xmlOutDoc.CloseDoc();
	return bRetCode;
}


BOOL CXmlParser::CreateEmeraldCancelTicketQueryXML(long lPumpNumber, CComBSTR& sXMLOutData, PAY_AT_PUMP_INFO & pInfo, long lRequestId)
{
	CXMLInterface   xmlOutDoc;
	CXMLPointers   pCarstomerInformationElement;
	CXMLPointers  pRootElement;
	BOOL bRetCode = xmlOutDoc.InitDoc();
	CString sTransactionId;
	try
	{
		bRetCode = xmlOutDoc.CreateElement("RewardServiceRequest", &pRootElement);
		bRetCode = xmlOutDoc.AppendRootElement(&pRootElement);
		bRetCode = xmlOutDoc.SetLongAttribute("RequestID", lRequestId, &pRootElement);
		bRetCode = xmlOutDoc.SetIntAttribute("PumpId", lPumpNumber, &pRootElement);
		bRetCode = xmlOutDoc.SetCharAttribute("RequestType", LOYALTY_EMERALD_CANCEL_REQUEST, &pRootElement);
		bRetCode = CFuelLoyaltyHelper::GetTransactionDataString(pInfo, CString("sTransactionId"), sTransactionId);
		bRetCode = xmlOutDoc.SetCharAttribute("TransactionId", sTransactionId.GetBuffer(sTransactionId.GetLength()), &pRootElement);
		bRetCode = AddCustomerInformationToXML(lPumpNumber, xmlOutDoc, pRootElement);  // RFUEL - 2628 Fuel do not send Customer information in PAP / PAK transaction with loyalty

		if (LOG_FCC_FLOW_CONTROL)
			xmlOutDoc.SaveToFile("c:\\OFFICE\\PUMPSRV\\LOG\\CancelTicket.xml");

		CString str("Current transaction (EmeraldID):");
		_LOGMSG.LogMsg("CFuelLoyaltyPS CXmlParser", "CreateEmeraldCancelTicketQueryXML", 0, LOG_PUMP, str+sTransactionId, LOG_LEVEL_1); //  4.0.23.980

	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS CXmlParser", "CreateEmeraldCancelTicketQueryXML", 0, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
		bRetCode = FALSE;
	}
	xmlOutDoc.GetXmlString(&sXMLOutData);
	xmlOutDoc.CloseDoc();
	return bRetCode;
}


 BOOL CXmlParser::AddCustomerInformationToXML(long lPumpNumber, CXMLInterface  &xmlOutDataDoc, CXMLPointers   &pRootElement, BOOL bDeleteCustomerLoyaltyFile/* = TRUE*/)
 {
	 
	LOG_MSG_START_METHOD("AddCustomerInformationToXML");
 	CXMLInterface  xmlLoyaltyTmp;
 	CXMLPointers    pLoyaltyInfo;
 	BOOL bRetCode = TRUE;
 	char sPath[MAX_FILE_PATH] = { 0 };
 	try
 	{
 		BOOL bRetCode = xmlLoyaltyTmp.InitDoc();
 		if (CFuelLoyaltyHelper::GetLoyaltyCustomerFileName(sPath, "", lPumpNumber))
 			bRetCode = xmlLoyaltyTmp.LoadFile(sPath);
		if (!bRetCode)
			LOG_MSG_FUNCINC("Failed loading Customer info from file");
 		if (bRetCode)
 		{
			// RFUEL - 2628 Fuel do not send Customer information in PAP / PAK transaction with loyalty
			bRetCode = xmlLoyaltyTmp.GetRootToElement(&pLoyaltyInfo);
			if (!bRetCode)
				LOG_MSG_FUNCINC("Failed xmlLoyaltyTmp.GetRootToElement");
			if (bRetCode)
			{
				bRetCode = xmlOutDataDoc.AppendChild(&pLoyaltyInfo, &pRootElement);
				if (!bRetCode)
					LOG_MSG_FUNCINC("Failed xmlOutDataDoc.AppendChild");
				xmlLoyaltyTmp.CloseDoc();
#ifdef DEBUG
				xmlOutDataDoc.SaveToFile("c:\\office\\pumpsrv\\LoyaltyInfoIncludingCustomerInfo.xml");
#endif
				if (bRetCode)
				{
					if (bDeleteCustomerLoyaltyFile) // after reading the customer loyalty info delete the local data file
					{
						bRetCode = DeleteFile(sPath);
						if (!bRetCode)
							_LOGMSG.LogMsg("CFuelLoyaltyPS CXmlParser", "AddCustomerInformationToXML", 0, LOG_PUMP, "Failed deleting loyalty customer data file", LOG_LEVEL_1);
						if (LOG_FCC_FLOW_CONTROL && bRetCode)
							_LOGMSG.LogMsg("CFuelLoyaltyPS CXmlParser", "AddCustomerInformationToXML", 0, LOG_PUMP, "customer data file was deleted", LOG_LEVEL_1);
					}
					//}
				}
				else
					_LOGMSG.LogMsg("CFuelLoyaltyPS CXmlParser", "AddCustomerInformationToXML", 0, LOG_PUMP, "Failed loading LoyaltyCustomer from file", LOG_LEVEL_1); //  4.0.23.980
			}
		}
	}
 	catch (...)
 	{
 		_LOGMSG.LogMsg("CFuelLoyaltyPS CXmlParser", "AddCustomerInformationToXML", 0, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
 		bRetCode = FALSE;
 	}
	LOG_MSG_FUNCINC_RETVAL(bRetCode);
 	return bRetCode;
 }
 


