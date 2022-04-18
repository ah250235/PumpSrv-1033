#include "StdAfx.h"
#include "FuelLoyaltyPSR.h"
#include "XMLInterface.h"
#include "PakFlow.h"
#include "PapFlow.h"
#include "DynamicPapFlow.h"
#include "phoneLookUpNmpDetalis.h"
#include "DeferralDiscountNMPDetails.h"
#include "CopientCommandExecuter.h"
#include "AuthorizeLoyaltyNMPDetails.h"
#include "AuthorizePaymentNMPDetails.h"
#include "SwipePaymentNMPDetails.h"
#include "RunQueryNMPDetails.h"
#include "FuelingNMPDetails.h"
#include "MethodOfPaymentNMPDetails.h"
#include "ManualLoyaltyNMPDetails.h"
#include "LoyaltyNMPDetails.h"
#include "PumpSrvDatabase.h"
#include "PumpSrvDatabaseLogger.h"
#include "SQLPumpSrvFile.h"
#include "SQLAlarmFile.h"
#include "SinglePumpLock.h"
#include "EmeraldLoyaltyCommandExecuter.h"
#include "FuelLoyaltyHelper.h"
#include "XMLConsts.h"

#define  CARWASH_PLU 1234
#define  MAX_BARCODES 5

//Copient 55286
CFuelLoyaltyPS::CFuelLoyaltyPS() :m_lFuelLoyaltyFlag(FALSE), m_lInitDllFlag(FALSE), m_bLoyaltyFlowPaymentFirst(FALSE), m_dwLoyaltyEngineType(LPE)
, m_lMinLoyaltyUnitPrice(0)
{
	memset(m_eaCancelTrs, 0, sizeof(m_eaCancelTrs));	//4.0.19.502 Copient + Rtaps 59557

	memset(m_lSegmentId,0,sizeof(m_lSegmentId));

	memset(m_lPosSeqenceNumber, 0, sizeof(m_lPosSeqenceNumber)); // TD 405456
	
	m_cLoyaltyCommandExecuter = NULL;
	 
}

CFuelLoyaltyPS::~CFuelLoyaltyPS()
{
	delete m_cLoyaltyCommandExecuter;
	m_TicketPromotionlist.clear();
	
}


void CFuelLoyaltyPS::CreateLoyaltyInterface()
{
	const CString sServers[7] = { "LPE", "COPIENT", "SPPS", "LPE_SA","EPSILON", "EMERALD", "EPSILON DISCOUNT"}; //4.0.23.140 93617

	if (m_dwLoyaltyEngineType > sizeof(sServers))
		m_dwLoyaltyEngineType = LPE;

	CString str;
	str.Format("CreateLoyaltyInterface: Server %.10s in use. Interface type = %ld ", sServers[m_dwLoyaltyEngineType], m_dwLoyaltyInterfaceType);
	_LOGMSG.LogMsg(str);


	if (m_cLoyaltyCommandExecuter)
		delete m_cLoyaltyCommandExecuter;

	if (m_dwLoyaltyEngineType == COPIENT)
	{
		m_cLoyaltyCommandExecuter = new CLoyaltyCopientCommandExecuter();
	}
	else if (m_dwLoyaltyEngineType == LPE)
	{
		if (m_dwLoyaltyInterfaceType == 0)
			m_cLoyaltyCommandExecuter = new CRTILoyaltyCommandExecuter();
		else
			m_cLoyaltyCommandExecuter = new CGLILoyaltyCommandExecuter();
	}
	else if (m_dwLoyaltyEngineType == EPSILON)
	{
		m_lInitDllFlag = TRUE;
		m_cLoyaltyCommandExecuter = new CEpsilonLoyaltyCommandExecuter();
	}
	else if (m_dwLoyaltyEngineType == LPE_SA)
	{
		m_cLoyaltyCommandExecuter = new CRTIStandAloneLoyaltyCommandExecuter();
	}
	else if (m_dwLoyaltyEngineType == EMERALD)
	{
		m_cLoyaltyCommandExecuter = new CEmeraldLoyaltyCommandExecuter();
	}
	else if (m_dwLoyaltyEngineType == EPSILON_DISCOUNT) {
		// RFUEL_2817
		m_lInitDllFlag = TRUE;
		m_cLoyaltyCommandExecuter = new CEpsilonLoyaltyCommandExecuter();
	}
}



/******************************************************************************
 Description:   Get pump number and create xml that contain the pumps
 Returns:		XML with pumps format

 Parameters:    lPumpNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			17/5/2005   15:38		Start
******************************************************************************/

BOOL CFuelLoyaltyPS::CreatePumpsInitXML(long lPumpNumber, char	* strOutXml , int iMaxSizeOfXml)
{
	BOOL bRetCode = TRUE;
	int	 i = 0;
	CXMLInterface xmlInitDoc;
	CXMLPointers  pCurrElement,pCurrElement2,pCurrElement3,pCurrElement4, pCurrElement5;
	CComBSTR	  bstrXML;
	

	try
	{
		bRetCode = xmlInitDoc.InitDoc();
		xmlInitDoc.CreateElement(XMLSTR_PUMPSALL,&pCurrElement);
		xmlInitDoc.AppendRootElement(&pCurrElement);
		xmlInitDoc.CreateElement(XMLSTR_PUMPS,&pCurrElement4);
		xmlInitDoc.AppendChild(&pCurrElement4,&pCurrElement);
		for(i=0;i<lPumpNumber;i++)
		{
			xmlInitDoc.CreateElement(XMLSTR_Pump,&pCurrElement2);
			xmlInitDoc.AppendChild(&pCurrElement2,&pCurrElement4);
			xmlInitDoc.SetIntAttribute(XMLSTR_PumpID,i,&pCurrElement2);

			xmlInitDoc.CreateElement(XMLSTR_Item,&pCurrElement3);
			xmlInitDoc.AppendChild(&pCurrElement3,&pCurrElement2);
			xmlInitDoc.SetIntAttribute(XMLSTR_PumpNumber,i+1,&pCurrElement3);

		}
		xmlInitDoc.SetLongAttribute("LogLevel",_LOGMSG.GetLogLevel(),&pCurrElement4);

	
	
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","CreatePumpsInitXML",0,LOG_PUMP,"Unexpected Error Had occurred",LOG_LEVEL_1);
	}
	xmlInitDoc.GetXmlString(&bstrXML);
	char tmp[MAX_BUF];
	int iLen = WideCharToMultiByte(CP_ACP,0,bstrXML,-1,tmp,sizeof(tmp),NULL,NULL);

	strcpy_s(strOutXml,iMaxSizeOfXml,tmp);

	if (LOG_FCC_FLOW_CONTROL)
		xmlInitDoc.SaveToFile("c:\\LPE\\XML\\PS_Pumps.xml");

	xmlInitDoc.CloseDoc();

	return bRetCode;
}




/******************************************************************************
 Description:   Get pump number and create xml that contain the pumps
 Returns:		After it call to loyalty fuel dll 

 Parameters:    lPumpNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			17/5/2005   15:38		Start
******************************************************************************/

BOOL CFuelLoyaltyPS::InitFuelLoyaltyInterface(long lNumberOfPumps)
{
	char		strMsg[MAX_FIELD_VAL];	
	BOOL		bRetCode = TRUE; //4.0.19.506 55286
	CComBSTR	bstrPump;
	char		sXML[MAX_BUF] = {0}; //  4.0.23.980

	try
	{		
		bRetCode =  m_cLoyaltyCommandExecuter->Init();

		if(TRUE == bRetCode)
		{    
			bRetCode = CreatePumpsInitXML(lNumberOfPumps,sXML,sizeof(sXML));
			WideCharToMultiByte(CP_ACP,0,bstrPump,-1,sXML,sizeof(sXML),NULL,NULL); //  4.0.23.980
			bRetCode = m_cLoyaltyCommandExecuter->InitPumps(sXML,TRUE);
			if (!bRetCode) //Success 4.0.19.506 55286
			//4.0.23.650
			{
				PumpItems	cPumpItemsGrades;
				cPumpItemsGrades.lCounter = 0;
				for (long lPumpNumber= 0; lPumpNumber < MAX_PUMPS_64; lPumpNumber ++) //4.0.23.650 136174
				{
					_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber,cPumpItemsGrades);
					FillInPromotionsArray(&cPumpItemsGrades, lPumpNumber);		
				}
				bRetCode = TRUE;
			}
		}
		else
		{
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				sprintf_s(strMsg, _countof(strMsg),"InitInterface failed");
				_LOGMSG.LogMsg("CFuelLoyaltyPS","InitFuelLoyaltyPump",0,LOG_NONE,strMsg);
			}
			bRetCode = FALSE;
		}
		
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","InitFuelLoyaltyPump",0,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
		bRetCode = FALSE;
	}
	if(bRetCode )
		m_lInitDllFlag = TRUE;
	return bRetCode;
}



void AddLineWithNulll(CXMLInterface *xmlInitDoc, CXMLPointers *pCurrElement, char *psAttributeName, BYTE *value, long lStrLen)
{
	unsigned int i =0;
	unsigned char  *tmpStr;
	BOOL	 bValid = TRUE;
	tmpStr = new unsigned char[lStrLen+1];

	tmpStr[lStrLen] = 0;
	memcpy(tmpStr,value,lStrLen);

	xmlInitDoc->SetCharAttribute(psAttributeName,(char *)tmpStr,pCurrElement);

	delete[] tmpStr;

}

void CFuelLoyaltyPS::FillInPromotionsArray(PumpItems * cTmpInfo, long lPumpNum) //4.0.23.50 136174
{
	int	 i = 0, j = 0;
	long		  lTmpOriginUnitPrice = 0;  //4.0.23.641
	for(i=cTmpInfo->lCounter - 1; i>0; i--)
	{
		for(j=i-1; j>=0; j--)
		{
			if (cTmpInfo->m_cItemSoldInfo[i].lNACSCode == cTmpInfo->m_cItemSoldInfo[j].lNACSCode)
				cTmpInfo->m_cItemSoldInfo[i].lNACSCode = -1;
		}
	}
	long lCurrentIndex = 0;
	for(i=0;i<cTmpInfo->lCounter;i++)
	{
		if (cTmpInfo->m_cItemSoldInfo[i].lNACSCode != -1)
		{

			long lOriginUnitPrice = GetItemPrice(lPumpNum , cTmpInfo , i); // 4.0.23.1052 TD 270145

			//4.0.20.610 72486 //4.0.22.500
			lTmpOriginUnitPrice = lOriginUnitPrice;   //4.0.23.641
			long lGradeCode = _Module.m_server.GetGradeCodeByNacsCode(*cTmpInfo,cTmpInfo->m_cItemSoldInfo[i].lNACSCode); 
			//4.0.25.50 start 136174
			m_Promotion[lPumpNum - 1].sPromID[lCurrentIndex].lGradeNumber = lGradeCode;				
			m_Promotion[lPumpNum - 1].sPromID[lCurrentIndex].lOriginalUnitPrice = lTmpOriginUnitPrice ; //4.0.23.641
			lCurrentIndex++; //4.0.23.641

			//4.0.25.50 end 136174

		}
	}
}

/******************************************************************************
 Description:  Create the xml that will send to create ticket

 Parameters:    iNumOfElement - Num Of grade in the pumps
				sCardSaleAll  - Hold all the info about the grades				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			17/5/2005   15:38		Start
 Liat			25/2/2008	20:27		Add CW item xml to start ticket
******************************************************************************/
//4.0.19.500 55286
BOOL CFuelLoyaltyPS::CreatePumpsGradeXML(long lPumpNumber,PumpItems * cTmpInfo,CComBSTR & bstrOutXml,PAY_AT_PUMP_INFO & pInfo, BOOL bAddCarWashBeforeFueling)
{
	BOOL bRetCode = TRUE;
	int	 i = 0, j = 0;
	CXMLInterface xmlInitDoc, xmlAddItemDoc;
	CXMLPointers  pCurrElement,pCurrElement2,pCurrElement3,pTrack2Loyalty, pCardSegment,pLanguage;
	long		  lAmount=0;
	try
	{
		bRetCode = xmlInitDoc.InitDoc();
		xmlInitDoc.CreateElement(XMLSTR_STARTTICKET,&pCurrElement);
		xmlInitDoc.AppendRootElement(&pCurrElement);
		xmlInitDoc.CreateElement(XMLSTR_PUMPSGRADE,&pTrack2Loyalty);
		xmlInitDoc.AppendChild(&pTrack2Loyalty,&pCurrElement);
		long lPumpNum = a2l(pInfo.CardSaleInfo.cardData.sPumpNumber, sizeof(pInfo.CardSaleInfo.cardData.sPumpNumber)); //4.0.25.50 4.0.23.640
	
		FillInPromotionsArray(cTmpInfo, lPumpNum);		//4.0.25.50 136174
		
		long lCurrentIndex = 0; //4.0.25.50 4.0.23.640
		for(i=0;i<cTmpInfo->lCounter;i++)
		{
			if (cTmpInfo->m_cItemSoldInfo[i].lNACSCode != -1)
			{
				lAmount = GetItemPrice(lPumpNum , cTmpInfo , i); // 4.0.23.1052 TD 270145
				xmlInitDoc.CreateElement(XMLSTR_GRADE,&pCurrElement2);
				xmlInitDoc.AppendChild(&pCurrElement2,&pTrack2Loyalty);
				xmlInitDoc.SetIntAttribute(XMLSTR_GradeID,i,&pCurrElement2);
				xmlInitDoc.CreateElement(XMLSTR_Item,&pCurrElement3);
				xmlInitDoc.AppendChild(&pCurrElement3,&pCurrElement2);
				xmlInitDoc.SetInt64Attribute(XMLSTR_AdditionalCode,cTmpInfo->m_cItemSoldInfo[i].lAdditionalCode,&pCurrElement3);
				xmlInitDoc.SetIntAttribute(XMLSTR_Grade,cTmpInfo->m_cItemSoldInfo[i].lNACSCode,&pCurrElement3);
	
				// 4.0.23.640 136174 4.0.20.610 72486 //4.0.22.500 
				long lOriginUnitPrice = GetItemPrice(lPumpNum , cTmpInfo , i); // 4.0.23.1052 TD 270145
				ChangePrecision(lOriginUnitPrice);
				xmlInitDoc.SetAttribute(XMLSTR_Price,lOriginUnitPrice,&pCurrElement3);
				xmlInitDoc.SetAttribute(XMLSTR_Quantity,"1000",&pCurrElement3);
				xmlInitDoc.SetAttribute(XMLSTR_QuantityType,"2",&pCurrElement3);
				xmlInitDoc.SetAttribute(XMLSTR_Amount,lAmount,&pCurrElement3);
				xmlInitDoc.SetAttribute(XMLSTR_DepCode,GetDepCode(),&pCurrElement3);	//Liat Copient 
				long lGradeCode = _Module.m_server.GetGradeCodeByNacsCode(*cTmpInfo,cTmpInfo->m_cItemSoldInfo[i].lNACSCode); 
				if (_Module.m_server.GetDynamicAttrForGradeID() > 0)//4.0.23.170 102319
				{
					xmlInitDoc.SetIntAttribute(XMLSTR_GradeCode,lGradeCode,&pCurrElement3);//4.0.23.170 102319
				}
			}			
		}

		SetLoyatyTrack2(pInfo, xmlInitDoc, pCurrElement);

		SetCardSegment(pInfo, xmlInitDoc, pCurrElement);

		SetLanguageId(pInfo, xmlInitDoc, pCurrElement);
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","CreatePumpsGradeXML",lPumpNumber,LOG_PUMP,"Unexpected Error Had occurred",LOG_LEVEL_1);
		bRetCode = FALSE;
	}

	xmlInitDoc.GetXmlString(&bstrOutXml);

	if (LOG_FCC_FLOW_CONTROL)
		xmlInitDoc.SaveToFile("c:\\LPE\\XML\\PS_StartTicket.xml");

	xmlInitDoc.CloseDoc();

	return bRetCode;
}

/******************************************************************************
 Description:  Create the xml that will send to query

 Parameters:    iNumOfElement - Num Of grade in the pumps
				sCardSaleAll  - Hold all the info about the grades				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			11/03/2009	13:30		Create xml to query for cents off //4.0.22.504
******************************************************************************/
BOOL CFuelLoyaltyPS::CreateQueryXML(long lPumpNumber ,PumpItems * cTmpInfo,char *  strOutXml,PAY_AT_PUMP_INFO & pInfo, BOOL bAddCarWashBeforeFueling)
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
		xmlInitDoc.CreateElement(XMLSTR_STARTTICKET,&pCurrElement);
		xmlInitDoc.AppendRootElement(&pCurrElement);
		xmlInitDoc.CreateElement(XMLSTR_PUMPSGRADE,&pCurrElement4);
		xmlInitDoc.AppendChild(&pCurrElement4,&pCurrElement);

		for(i=cTmpInfo->lCounter - 1; i>0; i--)
		{
			for(j=i-1; j>=0; j--)
			{
				if (cTmpInfo->m_cItemSoldInfo[i].lNACSCode == cTmpInfo->m_cItemSoldInfo[j].lNACSCode)
					cTmpInfo->m_cItemSoldInfo[i].lNACSCode = -1;
			}
		}
	
		for(i=0;i<cTmpInfo->lCounter;i++)
		{
			if (cTmpInfo->m_cItemSoldInfo[i].lNACSCode != -1)
			{
				lAmount = GetItemPrice(lPumpNumber , cTmpInfo , i); // 4.0.23.1052 TD 270145
				xmlInitDoc.CreateElement(XMLSTR_GRADE,&pCurrElement2);
				xmlInitDoc.AppendChild(&pCurrElement2,&pCurrElement4);
				xmlInitDoc.SetIntAttribute(XMLSTR_GradeID,i,&pCurrElement2);
				xmlInitDoc.CreateElement(XMLSTR_Item,&pCurrElement3);
				xmlInitDoc.AppendChild(&pCurrElement3,&pCurrElement2);
				xmlInitDoc.SetInt64Attribute(XMLSTR_AdditionalCode, cTmpInfo->m_cItemSoldInfo[i].lAdditionalCode, &pCurrElement3);
				xmlInitDoc.SetIntAttribute("Grade",cTmpInfo->m_cItemSoldInfo[i].lNACSCode,&pCurrElement3);

				long lOriginUnitPrice = GetItemPrice(lPumpNumber,cTmpInfo,i); // // 4.0.23.1052 TD 270145
				ChangePrecision(lOriginUnitPrice);
				xmlInitDoc.SetAttribute(XMLSTR_Price,lOriginUnitPrice,&pCurrElement3);
				xmlInitDoc.SetAttribute(XMLSTR_Quantity,"1000",&pCurrElement3);
				xmlInitDoc.SetAttribute(XMLSTR_QuantityType,"2",&pCurrElement3);
				xmlInitDoc.SetAttribute(XMLSTR_Amount,lAmount,&pCurrElement3);
				xmlInitDoc.SetAttribute(XMLSTR_DepCode,GetDepCode(),&pCurrElement3);	//Liat Copient 
				if (_Module.m_server.GetDynamicAttrForGradeID() > 0)
				{
					long lGradeCode = _Module.m_server.GetGradeCodeByNacsCode(*cTmpInfo,cTmpInfo->m_cItemSoldInfo[i].lNACSCode); //4.0.23.170 102319
	 				xmlInitDoc.SetIntAttribute(XMLSTR_GradeCode,lGradeCode,&pCurrElement3);	//4.0.23.170 102319
				}
			}
						
		}

		SetLoyatyTrack2(pInfo, xmlInitDoc, pCurrElement);		 // 4.0.32.??? TD 405435

		SetCardSegment(pInfo, xmlInitDoc, pCurrElement);		 // 4.0.32.??? TD 405435

	}
	catch(...)
	{
	}
	xmlInitDoc.GetXmlString(&bstrXML);
	char tmp[MAX_BUF];
	int iLen = WideCharToMultiByte(CP_ACP,0,bstrXML,-1,tmp,sizeof(tmp),NULL,NULL);
	strcpy(strOutXml,tmp);

	if (LOG_FCC_FLOW_CONTROL)
		xmlInitDoc.SaveToFile("c:\\LPE\\XML\\PS_StartTicket.xml");
	xmlInitDoc.CloseDoc();
	return bRetCode;
}
/******************************************************************************
 Description:  Create the xml that will send to create ticket

 Parameters:    iNumOfElement - Num Of grade in the pumps
				sCardSaleAll  - Hold all the info about the grades				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			17/5/2005   15:38		Start
 Liat			25/2/2008	20:27		Add CW item xml to start ticket
******************************************************************************/
//4.0.19.500 55286
BOOL CFuelLoyaltyPS::CreateAddMemberCardXML(PAY_AT_PUMP_INFO & pInfo, char *  strOutXml)
{
	BOOL bRetCode = TRUE;
	CXMLInterface xmlInitDoc; 
	CXMLPointers  pCurrElement; 
	CComBSTR	  bstrXML;
	try
	{
		bRetCode = xmlInitDoc.InitDoc();
		xmlInitDoc.CreateElement(XMLSTR_STARTTICKET,&pCurrElement);
		xmlInitDoc.AppendRootElement(&pCurrElement);

		SetLoyatyTrack2(pInfo, xmlInitDoc, pCurrElement);	 // 4.0.32.??? TD 405435

		SetCardSegment(pInfo, xmlInitDoc, pCurrElement);	 // 4.0.32.??? TD 405435

	}
	catch(...)
	{
		long lPumpNumber = a2l(pInfo.CardSaleInfo.cardData.sPumpNumber,sizeof(pInfo.CardSaleInfo.cardData.sPumpNumber));
		_LOGMSG.LogMsg("CFuelLoyaltyPS","CreateAddMemberCardXML",lPumpNumber,LOG_PUMP,"Unexpected Error Had occurred",LOG_LEVEL_1);
	}
	xmlInitDoc.GetXmlString(&bstrXML);
	char tmp[MAX_BUF];
	int iLen = WideCharToMultiByte(CP_ACP,0,bstrXML,-1,tmp,sizeof(tmp),NULL,NULL);
	strcpy(strOutXml,tmp);

	if (LOG_FCC_FLOW_CONTROL)
		xmlInitDoc.SaveToFile("c:\\LPE\\XML\\PS_AddMember.xml");

	xmlInitDoc.CloseDoc();

	return bRetCode;
}


/******************************************************************************
 Description:  Call to Start Ticket with 

 Parameters:    lPumpNumber -   PumpNumber
				cTmpInfo  -		Hold CardSaleData with all the grades				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			17/5/2005   15:38		Start
******************************************************************************/

BOOL CFuelLoyaltyPS::StartTicket(long lPumpNumber, PAY_AT_PUMP_INFO & pInfo)	//4.0.19.500 55286							 
{
	CString		strMsg;
	CComBSTR	bstrAddItemXML;
	BOOL		bRetCode = TRUE;	//4.0.22.504?
	PumpItems	cPumpItemsGrades;
	long		lPumpIndex = lPumpNumber - 1 ;
	BYTE		sTrsNumber[6]; // 4.0.23.1310  TD 312695
	BOOL		bAddCarWashBeforeFueling = FALSE;
	CPumpTransact trs;	//4.0.19.508

	if(FALSE == _Module.m_server.m_cFuelLoayaltyPS.GetInitDllFlag())//we have to init
	{
		bRetCode = FALSE;	//4.0.22.504?
		_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"StartTicket wasn't called after init failed.",LOG_LEVEL_1);
	}

	m_lPosSeqenceNumber[lPumpIndex] = 0; // TD 405456

	if (bRetCode)
	{
		char sVal[MAX_FIELD_VAL]; 
		memset(sVal,' ',sizeof(sVal));
		
		CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[lPumpIndex].GetStat();
		if (( cPumpStatus.GetRes() == PAY_AT_KIOSK ) || (cPumpStatus.GetResExt() == RES_EXT_PAK_LOYALTY))	//4.0.19.506
		{
			trs.m_lNumber	= _Module.m_server.NewTransactNumber();
			trs.m_lActiveReadNumber	= _Module.m_server.GetActiveReadNumber();
			if (cPumpStatus.GetResExt() == RES_EXT_PAK_LOYALTY) // RFS PAK //4.0.20.20
				trs.SetLink(TRS_LINK_OLA_NUMBER, _Module.m_server.m_cPumpArray[lPumpIndex].GetPAPSeqNumber());
			else	//Allied PAK
				trs.SetLink(TRS_NO_LINK,0);	
			trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[lPumpIndex].GetShiftNumber();
			trs.SetStatus(WAIT_PROGRESS);	
			trs.m_lPump = lPumpIndex+1;
			_Module.m_server.CentralInsertTrs(lPumpIndex+1,&trs);
			l2a(trs.m_lNumber,sTrsNumber, sizeof(sTrsNumber));	
			
		}
		else
		{
			// 4.0.23.1310  TD 312695
			memset(sTrsNumber , 0 , sizeof(sTrsNumber));
			memcpy(sTrsNumber,pInfo.CardSaleInfo.cardData.sTranNbr,min(sizeof(pInfo.CardSaleInfo.cardData.sTranNbr), sizeof(sTrsNumber)));	//4.0.19.500 5528
		}

		try
		{
			cPumpItemsGrades.lCounter = 0;
			_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber,cPumpItemsGrades);
			if (a2l(pInfo.CardSaleInfo.extraData.sWashCode, sizeof(pInfo.CardSaleInfo.extraData.sWashCode)))	
				bAddCarWashBeforeFueling = TRUE;

			CComBSTR	bstrPumpGradeXml;
			bRetCode =  CreatePumpsGradeXML(lPumpNumber,&cPumpItemsGrades,bstrPumpGradeXml,pInfo, bAddCarWashBeforeFueling);

			if(FALSE == bRetCode)
			{
				_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"Failed Create Grade XML ",LOG_LEVEL_1);	
			}
			else
			{
				_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"call StartTicket");
				long lTrsNumber = a2l(sTrsNumber,strlen((char*)sTrsNumber));
				_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"Start Session with Loyalty server");
				(_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber))->UpdateCarWashTime(pInfo, lPumpNumber);	//4.0.20.10

				if (bAddCarWashBeforeFueling)
				{
					bRetCode =  CreateAddCarWashItemXML(pInfo, bstrAddItemXML, lPumpNumber);
					if (bRetCode)
					{
						bRetCode = m_cLoyaltyCommandExecuter->StartTicketWithAddItem(bstrPumpGradeXml.m_str,lPumpNumber,lTrsNumber,0, bstrAddItemXML.m_str);
						_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"Start ticket with car wash item.");
		
					}
					else
					{
						bRetCode = m_cLoyaltyCommandExecuter->StartTicket(bstrPumpGradeXml.m_str, lPumpNumber, lTrsNumber, 0);
						_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"Start ticket without car wash item.");
					}
				}
				else
				{
					bRetCode = m_cLoyaltyCommandExecuter->StartTicket(bstrPumpGradeXml.m_str, lPumpNumber, lTrsNumber, 0);
					_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"Start ticket without car wash item.");

				}
				strMsg.Format("CFuelLoyaltyPS::StartTicket PumpNumber = %ld, TrsNumber = %ld", lPumpNumber, lTrsNumber);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
			}
		}
		catch(...)
		{
			_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"Unexpected Error Had occurred",LOG_LEVEL_1);
			bRetCode = FALSE;
		}
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP, "Start Ticket was finished");
		}
	}

	_Module.m_server.SetCancelInTheMiddleOfAuthorize(lPumpNumber,FALSE); // TD 337501  4.0.23.1580

	return bRetCode;

}


/******************************************************************************
 Description:  Query for cents off

 Parameters:    lPumpNumber -   PumpNumber
				cTmpInfo  -		Hold CardSaleData with all the grades				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			15/02/2009   17:45		Start
 OmerT          24/09/2013   Refactor
******************************************************************************/
BOOL CFuelLoyaltyPS::RunQuery(long lPumpNumber,PAY_AT_PUMP_INFO & pInfo)	//4.0.22.504
{
	if(FALSE == GetInitDllFlag())
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","RunQuery",lPumpNumber,LOG_PUMP,"RunQuery wasn't called after init failed.");
		return FALSE;
	}

	return	m_cLoyaltyCommandExecuter->Query(lPumpNumber,pInfo);
}



/******************************************************************************
 Description: Return grade that fits the additional code
 Parameters:    pPumpItem -   Hold all the grade,nace codes and additional codes

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			21/01/2010   13:38		Start //4.0.23.170 102319
******************************************************************************/

long	CFuelLoyaltyPS::GetGradeCodeByAdditionalCode(PumpItems & pPumpItem,INT64 lAdditionalCode)
{
	int i=0;
	long lGradeCode = 0;

	while(i<pPumpItem.lCounter)
	{
		if(pPumpItem.m_cItemSoldInfo[i].lAdditionalCode == lAdditionalCode)
		{
			lGradeCode = pPumpItem.m_cItemSoldInfo[i].lGrade;
			break;
		}
		i++;			

	}
	return lGradeCode;
}

/******************************************************************************
 Description: Update the limit according to the volume limit we got from Loyalty Server
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			21/9/2008   15:38		Start	//4.0.20.300
******************************************************************************/
void CFuelLoyaltyPS::UpdatePumpWithLoyaltyVolumeLimit(PAY_AT_PUMP_INFO &pInfo, long lPumpNumber)
{
	long lExistAuthAmt = a2l(pInfo.CardSaleExtraData2.sVolumeLimit,sizeof(pInfo.CardSaleExtraData2.sVolumeLimit)); //3.2.0.73
	long lVolumeLimit = a2l(pInfo.CardSaleExtraData4.sLoyaltyVolumeLimit, sizeof(pInfo.CardSaleExtraData4.sLoyaltyVolumeLimit));
	unsigned char sTmpValue[11];		
	long lExistVolumeLimit = 0;

	if ((lVolumeLimit) && ((lExistAuthAmt >= lVolumeLimit) || (lExistAuthAmt == 0)))	//The lowest is selected
	{
		memset(sTmpValue , 0, sizeof(sTmpValue));
		l2a(lVolumeLimit, pInfo.CardSaleExtraData2.sVolumeLimit , sizeof(pInfo.CardSaleExtraData2.sVolumeLimit));
		lExistVolumeLimit = 1;
		//l2a(1, &(pInfo.CardSaleExtraData4.sExistLoyaltyLimit), sizeof(pInfo.CardSaleExtraData4.sExistLoyaltyLimit));	//4.0.20.60 64734
	}
	else
	{
		lExistVolumeLimit = 0;
		l2a(lExistVolumeLimit, pInfo.CardSaleExtraData4.sLoyaltyVolumeLimit, sizeof(pInfo.CardSaleExtraData4.sLoyaltyVolumeLimit)); //4.0.20.450 66977
	}

	l2a(lExistVolumeLimit, &(pInfo.CardSaleExtraData4.sExistLoyaltyLimit), sizeof(pInfo.CardSaleExtraData4.sExistLoyaltyLimit));	//4.0.20.60 64734

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("CFuelLoyaltyPS::UpdatePumpWithLoyaltyVolumeLimit, Authorize amount after receiving data from loyalty server is:: sVolumeLimit=%.7s, sAuthAmtH=%.5s, sAuthAmtl=%.5s, sExistLoyaltyLimit = %.1s" ,pInfo.CardSaleExtraData2.sVolumeLimit , pInfo.CardSaleInfo.extraData.sAuthAmt_Msb ,pInfo.CardSaleInfo.cardData.sAuthAmt, &(pInfo.CardSaleExtraData4.sExistLoyaltyLimit));
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
	}
}

/******************************************************************************
 Description: After we get event from rtaps for start ticket, we have to continue the flow according to the flow control
 Parameters:  pump number

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			17/5/2005   15:38		Start
******************************************************************************/

BOOL CFuelLoyaltyPS::HandleStartTicketResult(long lPumpNumber, CXMLInterface & xmlInitDoc)
{
	CString str;
	if (PUMP_NOT_VALID(lPumpNumber))
	{
		str.Format("The PumpNumber that return from Loyalty server start ticket is invalid %ld",lPumpNumber);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);

	}
	else
	{
		CXmlParser parser;
		parser.ParseStartTikcketTime(lPumpNumber, xmlInitDoc);


		_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->StartAfterStartTicketFlowControl(lPumpNumber, NULL);//4.0.22.508 74810
	}
	return TRUE;
}

BOOL CFuelLoyaltyPS::HandleAddMemberCardResult(long lPumpNumber, OLA_STAT *pOla, long lResult)	//4.0.22.504	69344 //4.0.23.390 116544 //4.0.24.90
{

	_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->StartAfterAddMemberFlowControl(lPumpNumber, NULL, lResult); //4.0.22.508 74810
	return TRUE;
}

int CFuelLoyaltyPS::GetGradeIndexInPumpPromotionsArray(long lPumpNumber, long lGradeCode) //4.0.23.640 136174 //4.0.25.50
{
	int i = 0;
	for (i = 0; i < MAX_POSITIONS_PER_PUMP; i ++)
	{
		if (m_Promotion[lPumpNumber - 1].sPromID[i].lGradeNumber == lGradeCode) 
			break;
	
	}
	if (i == MAX_POSITIONS_PER_PUMP)	//index wasn't found
	{
		CString str;
		str.Format("No valid index for GradeCode %ld", lGradeCode);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);

	}
	return i;
}

long CFuelLoyaltyPS::GetSavedOriginalUnitPrice(long lPumpNumber, long lGradeCode) //4.0.23.640 136174 //4.0.25.50
{
	int i = 0;
	long lOriginalUnitPrice = 0;
	for (i = 0; i < MAX_POSITIONS_PER_PUMP; i ++)
	{
		if (m_Promotion[lPumpNumber - 1].sPromID[i].lGradeNumber == lGradeCode)
			break;
	
	}
	if (i == MAX_POSITIONS_PER_PUMP)	//index wasn't found
	{
		CString str;
		str.Format("No Saved Original Unit Price for GradeCode %ld", lGradeCode);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);

	}
	else
	{
		lOriginalUnitPrice = m_Promotion[lPumpNumber - 1].sPromID[i].lOriginalUnitPrice;
	}
	return lOriginalUnitPrice;
}
/******************************************************************************
 Description: After we get event from rtaps we have to parse the  XML
			  That return from the rtaps and send the result to ConvertLayer
 Parameters:    strGradesXML -   XML that contain all the discount per grade on the pump

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			17/5/2005   15:38		Start
******************************************************************************/

long CFuelLoyaltyPS::ParsePromotionsFromRtaps(BSTR strGradesXML)
{
	long			  lRetCode= TRUE;
	CString			  strMsg;
	CXMLInterface	  xmlInitDoc;
	CXMLPointers	  pCurrElement,pNode,pChildNode2,pChildNode1,pMemberCardNode, pAdditionalCodeNode, pPumpAutoStartNode;	//4.0.23.1202 89149 //4.0.23.960 232007
	long			  lPumpNumber=0;	//4.0.20.54 64528	
	long			  p = 0,lMasterM=0, lUseAdditionalCode = 0;	//4.0.23.960 232007
	BOOL			  bNeedToLeaveCS = FALSE;
	long			  lRewardType=0, lMemberRetCode = 0, lMemberRes = 0,  lAskDeferral = 0, lOriginalRewardField = 0, lPumpAutoStart = 0 ;	//4.0.22.504 69344 //4.0.23.1202 89149
	char			  strXML[MAX_BUF] = {0};	//  4.0.23.980
	BOOL			  bCarwashResponse = FALSE; 
	BOOL			  bActiveCustomer = TRUE; // CR 472487
	long			  lResCode = 0;

	OLA_STAT		  ola;
	memset(&ola,0,sizeof(OLA_STAT));
	PAY_AT_PUMP_INFO  cTmpInfo;
	memset(&cTmpInfo,' ' ,sizeof(PAY_AT_PUMP_INFO));

	int iLen = Bstr2Buffer(strGradesXML,strXML,sizeof(strXML));
	try
	{
		BOOL	bVaildXml = xmlInitDoc.InitDoc();
		bVaildXml = xmlInitDoc.LoadXmlString(strXML);

		if( bVaildXml == FALSE )
		{
			CString sStr(strXML);
			strMsg.Format("Invalid xml returned from loyalty server! Length of buffer result is:[%d]", iLen);
			if ( sStr.Find(LOYALTY_EMERALD_QUERY_SENT_SUCCEEDED_STRING) >= 0 )  //iLen < 56)  //(this is not a complete valid response XML, it might be a "query sent" confirmation..)
			{
				lRetCode = QRY_CONFIRMATION;
				_LOGMSG.LogClassMsg("CFuelLoyaltyPS", "ParseGradesFromRtaps", 0, LOG_NONE, strXML);
			}
			else
			{
				lRetCode = FALSE;
				_LOGMSG.LogClassMsg("CFuelLoyaltyPS", "ParseGradesFromRtaps", 0, LOG_NONE, strMsg.GetBuffer());
			}
		}
		else
		{
			if (LOG_FCC_FLOW_CONTROL)
				xmlInitDoc.SaveToFile("C:\\LPE\\XML\\GradesFromRtaps.xml");

			lRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_ALLProm,&pNode);

			if(SetPumpNumberReceivedFromLPE(lPumpNumber,p,xmlInitDoc,pNode)) // 4.0.23.2390  TD 406362
			{
				lPumpAutoStart = xmlInitDoc.SelectSingleElementNode(XMLSTR_PumpAutoStart,&pPumpAutoStartNode);		//4.0.23.1202 89149
				lMemberRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_AddMemberCardRes,&pMemberCardNode);
				lUseAdditionalCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_UseAdditionalCode,&pAdditionalCodeNode);
				lPumpNumber = xmlInitDoc.GetAttribute(XMLSTR_PumpNumber,&pNode);
				lResCode = xmlInitDoc.GetAttribute(XMLSTR_ResultCode, &pNode);
				_Module.m_server.m_cFlowControlManager.SaveEmeraldLoyaltyErrorCode(lResCode);  // RFUEL - 520  save result code from Emerald

				ClearTimer(p); // 4.0.23.2390  TD 406362

				//_Module.m_server.m_cPumpArray[p].m_cCritsec.Enter(); //TD 350969: Member card did not print on receipt
				bNeedToLeaveCS = TRUE;
				_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
				if ((ola.m_byState != OLA_NEW_STATE(SESSION_AUTHORIZE, OLA_LOYALTY_WAIT_RESPONSE)) //4.0.23.790 157216 
					&& (ola.m_byState != OLA_NEW_STATE(SESSION_AUTHORIZE, OLA_LOYALTY_WAIT_QUERY_CW_RESPONSE)) //4.0.23.950 216343
					&& (ola.m_byState != OLA_NEW_STATE(SESSION_AUTHORIZE, OLA_LOYALTY_WAIT_QUERY_RESPONSE)))
				{
					CString str;
					str.Format("Invalid state (%ld) for parsing grades from loyalty server. Parsing will be ignored", ola.m_byState);
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP,str);
					lRetCode = FALSE;
				}

				if (lRetCode)
				{
					if (OLA_STATE(ola.m_byState) == OLA_LOYALTY_WAIT_QUERY_CW_RESPONSE)
					{
						bCarwashResponse = TRUE;
					}
					_Module.m_server.m_cPumpArray[p].GetPAPInfo(&cTmpInfo);
					_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseGradesFromRtaps",lPumpNumber,LOG_PUMP,"Parse grades discount from Loyalty server");
					
					memset(&cTmpInfo.CardSaleExtraData6.sTransactionId, ' ', sizeof(cTmpInfo.CardSaleExtraData6.sTransactionId));
					xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_TransactionId, cTmpInfo.CardSaleExtraData6.sTransactionId, &pNode);
					SetVolumeLimitToLoyaltyMin(lPumpNumber, xmlInitDoc, cTmpInfo);  // RFUEL-2445 Minimum Reward limit should be applied irrespective of dispensed fuel product.

					ParseCustomerData(lPumpNumber, strXML, cTmpInfo, bActiveCustomer); // CR 472487
					
					_Module.m_server.m_cXmlParser.ParseAutoStartFlag(lPumpNumber,lPumpAutoStart, xmlInitDoc, pPumpAutoStartNode, cTmpInfo);
					
					if (bActiveCustomer) // CR 472487
					{
						ParsePromotions(lPumpNumber, xmlInitDoc, pNode, lUseAdditionalCode, pAdditionalCodeNode, cTmpInfo);
						ParseCarWashPromotions(lPumpNumber, xmlInitDoc, pNode, lUseAdditionalCode, pAdditionalCodeNode, cTmpInfo);
						// RFUEL-2729 SpartanNash - Select 'No' @ Car wash prompt, will get a second car wash prompt
						// If carwash is not set to "after fueling" on flow control, we don't want any carwash prompt after loyalty server reply 
						if (!_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsCarWashAfterFueling())
							cTmpInfo.CardSaleExtraData3.sCarwashRestriction = '0';   // prevent car wash prompting after query response.
					}
				}
			}
		}
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseGradesFromRtaps",0,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
		lRetCode = FALSE;
	}
	
	try
	{
		if ((lPumpNumber > 0) && (lRetCode == TRUE)) //334724 4.0.23.1610
		{
			ParseCustomerData(lPumpNumber, strXML, cTmpInfo, bActiveCustomer); // CR 472487
			_Module.m_server.m_cPumpArray[p].SetPAPInfo(&cTmpInfo);
			_Module.m_server.m_cFlowControlManager.SetEmeraldLoyaltyOnlineFlag(TRUE);
			if (bActiveCustomer) // CR 472487
				_Module.m_server.m_cFlowControlManager.StartAfterQueryFlowControl(lPumpNumber, ola, cTmpInfo);
			else
			{
				_Module.m_server.m_cFlowControlManager.SendNeedMoreParamSessionMemberInactive(lPumpNumber, ola, cTmpInfo);
			}
		}
		else 
		{
			if (ola.m_byState == OLA_NEW_STATE(SESSION_COMPLETE, OLA_REQUEST_TO_SEND))  // 461321 - Stuck transaction.
			{
				if (LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("CFuelLoyaltyPS::ParsePromotionsFromRtaps, pump = %ld , Continue with cancel Flow", lPumpNumber);
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
				}

			}
			else
				ParsePromotionsError(lPumpNumber); // 4.0.23.2390  TD 40636
		}
		
		_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseGradesFromRtaps",0,LOG_NONE,"Finish successfully ");
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS::ParseGradesFromRtaps, Unexpected error has occurred");
		lRetCode = FALSE;
	}

	if (bNeedToLeaveCS) //TD 350969: Member card did not print on receipt
	{
		bNeedToLeaveCS = FALSE;
	}

	if (LOG_FCC_FLOW_CONTROL)
	{
		strMsg.Format("CFuelLoyaltyPS::ParsePromotionsFromRtaps finished with result = %s", lRetCode ? "TRUE" : "FALSE");
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
	}

	return lRetCode;
}


void CFuelLoyaltyPS::StartTicketResult(BSTR strGradesXML)
{
	ParsePromotionsFromRtaps(strGradesXML);

}
//4.0.22.504 69344
void CFuelLoyaltyPS::QueryResult(BSTR strGradesXML)
{
	long lRetCode = ParsePromotionsFromRtaps(strGradesXML);
	
	CString str;
	str.Format("CFuelLoyaltyPS::QueryResult was called by loyalty lRetCode of parsing is [%ld]", lRetCode);
	_LOGMSG.LogMsg(0, LOG_PUMP, str);
}

//4.0.22.504 69344
void CFuelLoyaltyPS::GeneralResult(BSTR strResult)
{
	
	BOOL			  bRetCode= TRUE;
	char			  strXML[MAX_BUF];	//10000
	CXMLInterface	  xmlInitDoc;
	CXMLPointers	  pCurrElement,pNode,pChildNode;
	long lPumpNumber = 0, p = 0, lResult = 0;
	char			sMethodName[MAX_FIELD_VAL] = {0};
	
	int iLen = WideCharToMultiByte(CP_ACP,0,strResult,-1,strXML,sizeof(strXML),NULL,NULL);
	
	try
	{
		long	lRetCode = xmlInitDoc.InitDoc();
		lRetCode = xmlInitDoc.LoadXmlString(strXML);
		
		if(lRetCode != 0)
		{ 
			if (LOG_FCC_FLOW_CONTROL)
				xmlInitDoc.SaveToFile("C:\\LPE\\XML\\GeneralResult.xml"); //Liat LPE
			
			lRetCode = xmlInitDoc.SelectSingleElementNode("LoyaltyResult",&pNode);
			lPumpNumber = xmlInitDoc.GetAttribute("PumpNumber",&pNode);
			p = lPumpNumber - 1;
			if(p<0)
			{
				CString str;
				str.Format("CFuelLoyaltyPS::GeneralResult, The PumpNumber that return from Loyalty server is invalid %ld",lPumpNumber);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
			}
			else 
			{
				if(FALSE == _Module.m_server.GetCancelInTheMiddleOfAuthorize(lPumpNumber)) // 4.0.23.2100 TD 385971
				{
					_Module.m_server.m_cFuelLoayaltyPS.ClearTimer(p);
					
					xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_Method, sMethodName, &pNode);
					lResult = xmlInitDoc.GetAttribute(XMLSTR_Result, &pNode);
					if (!strcmp(sMethodName, START_TICKET_METHOD))
					{
						HandleStartTicketResult(lPumpNumber, xmlInitDoc);
					}
					else if (!strcmp(sMethodName, ADD_MEMBER_CARD_METHOD))
					{
						HandleAddMemberCardResult(lPumpNumber, NULL, lResult);//4.0.23.390 116544 //4.0.24.90
					}
					else if(!strcmp(sMethodName, END_TICKET_METHOD)) // 4.0.23.2280. 305114
					{
						HandleEndTicketResult(lPumpNumber);
					}
					else if(!strcmp(sMethodName, ADD_MEMBER_INFO_METHOD)) // 4.0.23.2280. 305114
					{
						HandleAddMemberInfoResult(lPumpNumber);
					}
				}
				else
				{	
					_Module.m_server.SetCancelInTheMiddleOfAuthorize(lPumpNumber,FALSE);
					_LOGMSG.LogMsg("CRTIEventsHandler","GeneralResult",lPumpNumber,LOG_PUMP,"Cancel in the middle of authorize! Reject General Result");
				}

			}

		}
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","GeneralResult",0,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
		bRetCode = FALSE;
	}
}



/******************************************************************************
 Description: Add member card to the ticket 
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			11/02/09	17:08		Start 
******************************************************************************/

BOOL CFuelLoyaltyPS::AddMemberCard(long lPumpNumber,PAY_AT_PUMP_INFO & pInfo)
{
	long		p = lPumpNumber - 1;
	char		strMsg[MAX_FIELD_VAL] = {0};
	char		strXML[MAX_BUF];
	BOOL		bRetCode = TRUE;
	CComBSTR	sXML;

	if(!_Module.m_server.m_cFuelLoayaltyPS.GetInitDllFlag())//we have to init
	{
		bRetCode = FALSE;	//4.0.22.504 69344
		_LOGMSG.LogMsg("CFuelLoyaltyPS","AddMemberCard",lPumpNumber,LOG_PUMP,"AddMemberCard wasn't called after init failed.",LOG_LEVEL_1);
	}

	try
	{
		if (bRetCode)
		{
			bRetCode =  CreateAddMemberCardXML(pInfo, strXML);
			sXML = strXML;

			bRetCode = m_cLoyaltyCommandExecuter->AddMemberCard(sXML.m_str, lPumpNumber,0, pInfo);
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				sprintf_s(strMsg, _countof(strMsg),"CFuelLoyaltyPS::AddMemberCard, AddMemberCard was sent, pump = %ld",lPumpNumber);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
			}

		}
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","AddMemberCard",lPumpNumber,LOG_PUMP,"Unexpected Error Had occurred",LOG_LEVEL_1);
	}

	if (LOG_FCC_FLOW_CONTROL)
	{
		CString strMsg;
		strMsg.Format("AddMemberCard finished with result = %s", bRetCode ? "TRUE" : "FALSE");
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
	}

	return bRetCode;
}

long CFuelLoyaltyPS::EndTicket(long lPumpNumber, BOOL bInitLoyalID)
{
	long		p = lPumpNumber - 1;
	char		strMsg[MAX_FIELD_VAL] = { 0 };
	long		lRetCode = 0;
	try
	{
		lRetCode = m_cLoyaltyCommandExecuter->EndTicket(lPumpNumber, 0); //  RFUEL-2184 - return the end ticket result
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			sprintf_s(strMsg, _countof(strMsg), "CFuelLoyaltyPS::EndTicket, End ticket was sent, pump = %ld", lPumpNumber);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
		}
		_Module.m_server.m_cFuelLoayaltyPS.InitSavedLoyaltyInfo(lPumpNumber); //4.0.20.360 68429
		if (bInitLoyalID)	//4.0.20.51 63930
		{
			//4.0.18.15
			PAY_AT_PUMP_INFO PayAtPumpInfo;
			memset(&PayAtPumpInfo, 0, sizeof(PayAtPumpInfo));
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&PayAtPumpInfo);
			memset(&PayAtPumpInfo.CardSaleInfo.cardData.sAccountNumber, ' ', sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sAccountNumber)); //4.0.18.15
			memset(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId, ' ', sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId));
			memset(PayAtPumpInfo.CardSaleExtraData6.sClubCardHashNumber, ' ', sizeof(PayAtPumpInfo.CardSaleExtraData6.sClubCardHashNumber));	 // 4.0.32.??? TD 405435
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&PayAtPumpInfo);
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				sprintf_s(strMsg, _countof(strMsg), "CFuelLoyaltyPS::EndTicket, Loyalty ID was initialized, pump = %ld", lPumpNumber);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
			}
		}

		SetAddMemberInfo(lPumpNumber, 0); // TD 305114
	}
	catch (...)
	{
		lRetCode = 0;
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "EndTicket", lPumpNumber, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1);
	}

	return lRetCode;
}

/******************************************************************************
 Description: Canceling the loyalty ticket 
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			18/07/07	16:00		Start 
******************************************************************************/

void CFuelLoyaltyPS::CancelTicket(long lPumpNumber, BOOL bPAKAddItem, BOOL bInitLoyalID)	//4.0.28.500 231113
{
	long		p = lPumpNumber - 1;
	long		lRetCode = 0;
	BOOL		bRetCode = FALSE;
	PumpItems	cPumpItemsGrades;
	long		lFullServicePrice = 0, lSelfServicePrice = 0, lTotalNoDiscount = 0, lPromID = 0,  lFlags = 0;
	double		fCentsOff = 0;
	PAY_AT_PUMP_INFO  cTmpInfo;
	CPumpTransact trs;

	try
	{
		if ((bPAKAddItem) && (!IsCancelTrs(lPumpNumber)))	//4.0.22.504 69344 if StartTicket failed,  AddItem should not be sent in PAK
		{
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"CFuelLoyaltyPS::CancelTicket Cancel ticket was sent with AddItem data");
			}

			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);
			{
				lFlags = CANCEL_FROM_PAK;

				_Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&trs);

				_Module.m_server.m_cPumpArray[p].GetPAPInfo(&cTmpInfo);
			}
			

			_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber,cPumpItemsGrades);

			CalculateLoyaltyValues(cTmpInfo, lPumpNumber, trs);//4.0.20.300
			
			if (!a2l(cTmpInfo.CardSaleExtraData3.sOriginalUnitPrice, sizeof(cTmpInfo.CardSaleExtraData3.sOriginalUnitPrice))) //4.0.20.56
				l2a(lSelfServicePrice, cTmpInfo.CardSaleExtraData3.sOriginalUnitPrice, sizeof(cTmpInfo.CardSaleExtraData3.sOriginalUnitPrice));

			GetPromotionId(trs.m_nGrade, lPumpNumber, &lPromID, &fCentsOff);
			l2a(lPromID, cTmpInfo.CardSaleExtraData2.sPromotionNumber, sizeof(cTmpInfo.CardSaleExtraData2.sPromotionNumber));
			_Module.m_server.m_cPumpArray[p].SetPAPInfo(&cTmpInfo);

			lRetCode = m_cLoyaltyCommandExecuter->CancelTicketWithAddItem(lPumpNumber,lFlags/*, bstrXml.m_str */);

		}
		else
		{
			lRetCode = m_cLoyaltyCommandExecuter->CancelTicket(lPumpNumber,lFlags);
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				_LOGMSG.LogClassMsg("CFuelLoyaltyPS","CancelTicket",lPumpNumber,LOG_NONE,"Cancel ticket was sent");
			}
		}
		_Module.m_server.m_cFuelLoayaltyPS.InitSavedLoyaltyInfo(lPumpNumber); //4.0.20.360 68429
		//4.0.18.15
		if (bInitLoyalID)	//4.0.23.991 231113 4.0.28.500
		{
			PAY_AT_PUMP_INFO PayAtPumpInfo;
			memset(&PayAtPumpInfo,0,sizeof(PayAtPumpInfo));
			_Module.m_server.m_cPumpArray[lPumpNumber -1].GetPAPInfo(&PayAtPumpInfo);
			memset(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId,' ',sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId));
			memset(PayAtPumpInfo.CardSaleExtraData6.sClubCardHashNumber,' ',sizeof(PayAtPumpInfo.CardSaleExtraData6.sClubCardHashNumber));	 // 4.0.32.??? TD 405435
			_Module.m_server.m_cPumpArray[lPumpNumber -1].SetPAPInfo(&PayAtPumpInfo);
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				_LOGMSG.LogClassMsg("CFuelLoyaltyPS","CancelTicket",lPumpNumber,LOG_NONE,"CFuelLoyaltyPS::CancelTicket, Loyalty ID was initialized");
			}
		}

		SetAddMemberInfo(lPumpNumber,0); // TD 305114
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","CancelTicket",lPumpNumber,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1);	
	}
}

BOOL CFuelLoyaltyPS::AddItem(long lPumpNumber,long lTrsNum, OLA_STAT & ola)
{
	BOOL		bRetCode = TRUE;	//4.0.22.504 69344
	_bstr_t		bstrXML;

	if (!_Module.m_server.m_cFuelLoayaltyPS.GetInitDllFlag())//we have to init
	{
		bRetCode = FALSE;
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "AddItem", lPumpNumber, LOG_PUMP, "AddItem wasn't called after init failed.");
	}

	try
	{
		if (bRetCode)
		{
			
			bRetCode =  m_cLoyaltyCommandExecuter->AddItem(bstrXML, lPumpNumber, 0, ola, lTrsNum);
		}
	}
	catch(...)
	{
		bRetCode = FALSE;
	}
	
	if(LOG_FCC_FLOW_CONTROL)
	{
		CString strMsg;
		strMsg.Format("AddItem finished with result = %s", bRetCode ? "TRUE" : "FALSE");
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
	}
	
	return bRetCode;

}
/******************************************************************************
 Description: Calculate discounts from loyalty server
 Parameters:    

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			28/09/08	16:00		Start //4.0.20.300 67854
******************************************************************************/
void CFuelLoyaltyPS::CalculateLoyaltyValues(PAY_AT_PUMP_INFO & cTmpPapInfo, long lPumpNumber, CPumpTransact & trs)
{
	long lOriginalUnitPrice = 0, lNonDiscountTotalAmount = 0, lTotalAmountWithDiscount = 0, lFullServicePrice = 0, lSelfServicePrice = 0; 
	long lServiceMode = 0, lPriceLevel = 0, lTotalDiscount = 0;
	long p = lPumpNumber - 1;
	char strMsg[MAX_FIELD_VAL];

	lTotalAmountWithDiscount = ConvertStringsToLong(cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt,sizeof(cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt),cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt_Msb));
	if(GetLoyaltySrvInUse() == COPIENT)	//RFS Copient
	{
		lOriginalUnitPrice = a2l(cTmpPapInfo.CardSaleExtraData4.sLoyaltyOriginalUnitPrice, sizeof(cTmpPapInfo.CardSaleExtraData4.sLoyaltyOriginalUnitPrice));
		lNonDiscountTotalAmount = a2l(cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount
				,sizeof(cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount));
		lNonDiscountTotalAmount *= 10;
		l2a(lNonDiscountTotalAmount, cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount
				,sizeof(cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount));
		lNonDiscountTotalAmount += (a2l((BYTE *)&cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice,sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice)) * (_Module.m_server.GetCarWashScalarFactor()));

		lTotalDiscount = a2l(cTmpPapInfo.CardSaleExtraData2.sTotalDiscount, sizeof(cTmpPapInfo.CardSaleExtraData2.sTotalDiscount));	
		//sTotalDiscount is calculated by RFS
	}
	else	//Allied LPE
	{	
		lOriginalUnitPrice = GetSavedOriginalUnitPrice(lPumpNumber, trs.m_nGrade);//4.0.23.640 136174

		//4.0.23.371 117151 start
		lTotalDiscount = a2l(cTmpPapInfo.CardSaleInfo.extraData.sGRTotalDiscount, sizeof(cTmpPapInfo.CardSaleInfo.extraData.sGRTotalDiscount));
		if (lTotalDiscount)
		{
			lNonDiscountTotalAmount = lTotalAmountWithDiscount + lTotalDiscount;
			memset(strMsg, 0, sizeof(strMsg)); //4.0.20.56
			sprintf_s(strMsg, _countof(strMsg), "CFuelLoyaltyPS::CalculateLoyaltyValues (from CL) %ld (TotalNoDiscount) = %ld (TotalWithDiscount) + %ld (TotalDiscount) , service mode = %ld",lNonDiscountTotalAmount, lTotalAmountWithDiscount, lTotalDiscount, lServiceMode);
			lNonDiscountTotalAmount -= a2l((BYTE *)&cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice,sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice));
			//The totalAmtBeforeDiscount is saved without the CW price.
			l2a(lNonDiscountTotalAmount, cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount, sizeof(cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount));
		}
		else
		{// CL didn't calculate the total discount (for example in PAK), so pumpSrv has to calculate it.

			lNonDiscountTotalAmount = (lOriginalUnitPrice *trs.m_lRoundedVolume/1000) /10 *10;// + 5) /10 * 10;//Need to cut the 3rd decimal digit
			l2a(lNonDiscountTotalAmount, cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount, sizeof(cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount));
			memset(strMsg, 0, sizeof(strMsg)); //4.0.20.56
			sprintf_s(strMsg, _countof(strMsg), "CFuelLoyaltyPS::CalculateLoyaltyValues %ld (TotalNoDiscount) = [%ld (unit price) * %ld (RoundedVolume)] / 1000, service mode = %ld",lNonDiscountTotalAmount, lOriginalUnitPrice, trs.m_lRoundedVolume, lServiceMode);
			lNonDiscountTotalAmount += (a2l((BYTE *)&cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice,sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice)) * (_Module.m_server.GetCarWashScalarFactor()));

			if (lNonDiscountTotalAmount -lTotalAmountWithDiscount <= 10)
			{
				lNonDiscountTotalAmount = lTotalAmountWithDiscount;
			}
			lTotalDiscount = (lNonDiscountTotalAmount -lTotalAmountWithDiscount); //3 decimal point
		}

		//4.0.23.371 117151 end

		l2a(lOriginalUnitPrice, cTmpPapInfo.CardSaleExtraData3.sOriginalUnitPrice, sizeof(cTmpPapInfo.CardSaleExtraData3.sOriginalUnitPrice));
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP, strMsg);


		if (lTotalDiscount > 0)  
		{//if CL didn't calculate the total cents off discount, pumpSrv has to calculate and save it.
			l2a(lTotalDiscount, cTmpPapInfo.CardSaleExtraData2.sTotalDiscount, sizeof(cTmpPapInfo.CardSaleExtraData2.sTotalDiscount));	
			l2a(0, cTmpPapInfo.CardSaleInfo.extraData.sGRTotalDiscount, sizeof(cTmpPapInfo.CardSaleInfo.extraData.sGRTotalDiscount)); //4.0.25.501 144101
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetPAPInfo(&cTmpPapInfo);
		}
	}
	memset(strMsg, 0, sizeof(strMsg)); //4.0.20.56
	sprintf_s(strMsg, _countof(strMsg), "Transaction total amount (including dry items): TotalWithDiscount = [%d], TotalDiscount = [%d], TotalNoDiscount = [%d]",	
		lTotalAmountWithDiscount, lTotalDiscount, lNonDiscountTotalAmount);
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP, strMsg);

}





BOOL CFuelLoyaltyPS::CreateAddItemXML(PAY_AT_PUMP_INFO & cTmpPapInfo,
									  CComBSTR			& bstrOutXml,
									  PumpItems 	   & cTmpInfo,
									  CPumpTransact    & trs,
									  long				lPumpNumber)
{
	BOOL bRetCode = TRUE;
	int	 i = 0;
	CXMLInterface xmlInitDoc; //4.0.23.950 216343
	CXMLPointers  pRootElement,pPumpsPropElement,pPumpsPropElement2,pChildNode;	//4.0.22.504 69344 //4.0.23.170 102319

	long lNacsCode = GetNacsCodeByGradeCode(cTmpInfo,trs.m_nGrade);
	INT64 lAdditionalCode = GetAdditionalCodeByGradeCode(cTmpInfo,trs.m_nGrade); //4.0.23.170 102319
	long lAmout=0;
	BYTE sTrack2[TRACK_2_DATA_SIZE] = {0};
	CString sMsg;
	long lPumpIndex = lPumpNumber-1;

	try
	{
		lAmout = 0;
		bRetCode = xmlInitDoc.InitDoc();
		xmlInitDoc.CreateElement("ADDITAM",&pRootElement);
		xmlInitDoc.AppendRootElement(&pRootElement);
		xmlInitDoc.CreateElement("PUMPSPROP",&pPumpsPropElement);
		xmlInitDoc.AppendChild(&pPumpsPropElement,&pRootElement);
	 
		xmlInitDoc.SetAttribute("PluCode",lNacsCode,&pPumpsPropElement);	//4.0.19.500 TD 55286
		xmlInitDoc.SetInt64Attribute("AdditionalCode",lAdditionalCode,&pPumpsPropElement);	//4.0.23.170 102319
		if (_Module.m_server.GetDynamicAttrForGradeID() > 0)
			xmlInitDoc.SetAttribute("GradeID", trs.m_nGrade, &pPumpsPropElement);	//4.0.23.170 102319
	
		long lOriginUnitPrice = 0;
		if(GetLoyaltySrvInUse() == COPIENT)	//RFS Copient
		{
			lOriginUnitPrice = a2l(cTmpPapInfo.CardSaleExtraData4.sLoyaltyOriginalUnitPrice	//64635	4.0.20.54
				,sizeof(cTmpPapInfo.CardSaleExtraData4.sLoyaltyOriginalUnitPrice));
		}
		else	//Allied
		{
			lOriginUnitPrice = GetSavedOriginalUnitPrice(lPumpNumber, trs.m_nGrade);//4.0.23.640 136174
		}
		
		//4.0.20.56 64904
		ChangePrecision(lOriginUnitPrice);
		xmlInitDoc.SetAttribute("Price",(long)(lOriginUnitPrice),&pPumpsPropElement);//16-5-07 LPE
		xmlInitDoc.SetAttribute("discount","0",&pPumpsPropElement);
		xmlInitDoc.SetAttribute("Quantity",trs.m_lRoundedVolume,&pPumpsPropElement);
		xmlInitDoc.SetIntAttribute("QuantityType",QUANTITY_TYPE_WEIGHT,&pPumpsPropElement);
		xmlInitDoc.SetIntAttribute("FuelItem",1,&pPumpsPropElement); //Meaning it's a fuel item //4.0.19.500 TD 55286
		xmlInitDoc.SetAttribute("DepCode",_Module.m_server.m_cXmlParser.GetDepCode(),&pPumpsPropElement);	//Liat Copient
		xmlInitDoc.SetAttribute("TrsNum",trs.m_lNumber,&pPumpsPropElement);
		xmlInitDoc.SetAttribute("PosSequence", ++m_lPosSeqenceNumber[lPumpNumber-1], &pPumpsPropElement); // TD 405456
		

		variant_t vReceiptAttributeData = _Module.m_server.m_cFuelLoayaltyPS.GetPrintReceipt(lPumpNumber-1) ? "1":"0";
		xmlInitDoc.SetAttribute("PrintReceipt",vReceiptAttributeData,&pPumpsPropElement);
		
		variant_t vDeferredAcceptedAttributeData = IsDeferralDiscountSelected(lPumpNumber) ? "1":"0";
		xmlInitDoc.SetAttribute("DeferredAccepted",vDeferredAcceptedAttributeData,&pPumpsPropElement);

		memcpy(sTrack2,cTmpPapInfo.CardSaleInfo.cardData.sAccountNumber, sizeof(cTmpPapInfo.CardSaleInfo.cardData.sAccountNumber));
		AddLineWithNulll(&xmlInitDoc,&pPumpsPropElement,"Track2Number",(BYTE*)sTrack2,sizeof(sTrack2));

		AddTotalTenderAmount(lPumpNumber, cTmpPapInfo, xmlInitDoc, pPumpsPropElement,pRootElement,lNacsCode,&trs); // TD 404465

		ForceFuelItem(lNacsCode,xmlInitDoc, pPumpsPropElement, pRootElement, trs, lPumpNumber, cTmpPapInfo, lOriginUnitPrice, pChildNode);

		PrintForceDiscounts(lPumpNumber,cTmpPapInfo);
		
		if (LOG_FCC_FLOW_CONTROL)
			xmlInitDoc.SaveToFile("c:\\LPE\\XML\\PS_AddItem.xml");
		
		xmlInitDoc.GetXmlString(&bstrOutXml);
		
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS::CreateAddItemXML, Unexpected error has occurred", LOG_LEVEL_1); //  4.0.23.980
	}
	
	xmlInitDoc.CloseDoc();
	
	return TRUE;
}
//4.0.20.50
long CFuelLoyaltyPS::GetTenderNumber(PAY_AT_PUMP_INFO & cTmpPapInfo, BOOL bFirstTender)
{
	long lTenderNum = 0;

	if (_Module.m_server.GetTwoTendersInTransaction()) // TD 404465
	{
		if (bFirstTender)
			return a2l(cTmpPapInfo.CardSaleExtraData6.PaymentCard1.sTenderId, sizeof(cTmpPapInfo.CardSaleExtraData6.PaymentCard1.sTenderId));
		else
			return a2l(cTmpPapInfo.CardSaleExtraData6.PaymentCard2.sTenderId, sizeof(cTmpPapInfo.CardSaleExtraData6.PaymentCard2.sTenderId));
	}

	switch (cTmpPapInfo.CardSaleInfo.cardData.sTranType)
	{
	case CD_TYPE_CREDIT:
	{
		lTenderNum = 3;
	}
	break;
	case CD_TYPE_DEBIT:
	{
		lTenderNum = 11;
	}
	break;
	case CD_TYPE_LOYALTY:
	{
		lTenderNum = 25;
	}
	break;
	case CD_TYPE_CASH:
	{
		lTenderNum = 1;
	}
	break;
	default:
	{
		lTenderNum = 0;
	}
	break;
	}

	return lTenderNum;
}


//4.0.19.502 TD 55286
//Non fuel item (car wash) doesn't have the force operation
BOOL CFuelLoyaltyPS::CreateAddCarWashItemXML(PAY_AT_PUMP_INFO & cTmpPapInfo, CComBSTR &bstrAddItemXML, long	lPumpNumber)
{
	BOOL bRetCode = TRUE;
	CXMLInterface xmlInitDoc;
	CXMLPointers  pCurrElement,pPumpElement;
	long lAmout = 1, lUnitPrice = 0, lQuantity = 1;
	__int64 lPluCode = 0;
	CString strMsg;

	try
	{
		GetCarWashDetalisFromCardSaleData(lPumpNumber, lPluCode, cTmpPapInfo, lUnitPrice, lQuantity);

		ChangePrecision(lUnitPrice);

		if (lUnitPrice <= 0)
			bRetCode = FALSE;

		if (bRetCode)
		{
			lAmout = lUnitPrice * lQuantity;
			bRetCode = xmlInitDoc.InitDoc();
			xmlInitDoc.CreateElement("ADDITAM",&pCurrElement);
			xmlInitDoc.AppendRootElement(&pCurrElement);
			xmlInitDoc.CreateElement("PUMPSPROP",&pPumpElement);
			xmlInitDoc.AppendChild(&pPumpElement,&pCurrElement);
		
			xmlInitDoc.SetInt64Attribute("PluCode",lPluCode,&pPumpElement);
			xmlInitDoc.SetAttribute("Amount", lAmout, &pPumpElement); 
			xmlInitDoc.SetAttribute("Price", lUnitPrice, &pPumpElement);
			xmlInitDoc.SetAttribute("Quantity", lQuantity, &pPumpElement);
			xmlInitDoc.SetIntAttribute("QuantityType",QUANTITY_TYPE_COUNTED,&pPumpElement);
			xmlInitDoc.SetIntAttribute("FuelItem",0,&pPumpElement); //Meaning it's not a fuel item //4.0.19.502 TD 55286
			xmlInitDoc.SetIntAttribute("PosSequence", ++m_lPosSeqenceNumber[lPumpNumber-1], &pPumpElement); // TD 405456
		}
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS::CreateAddNonFuelItemXML, Unexpected error has occurred");
		bRetCode = FALSE;
	}

	xmlInitDoc.GetXmlString(&bstrAddItemXML);

	if (LOG_FCC_FLOW_CONTROL)
		xmlInitDoc.SaveToFile("c:\\LPE\\XML\\PS_AddCarWashItem.xml");

	xmlInitDoc.CloseDoc();

	strMsg.Format("CFuelLoyaltyPS::CreateAddNonFuelItemXML was finished with result %ld", (bRetCode == TRUE)? 1:0);
	_LOGMSG.LogMsg(strMsg);

	return bRetCode;
}

/******************************************************************************
 Description: Get the trigger prom and parse 
			  
 Parameters:    strGradesXML -   XML that contain all the promotions

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			17/5/2005   15:38		Start
******************************************************************************/

void CFuelLoyaltyPS::GetTriggerPromResult(BSTR strTriggerPromXML, long lPumpNumber)
{
	ParseTriggerPromFromRtaps(strTriggerPromXML, lPumpNumber);
}

BOOL CFuelLoyaltyPS::ParseTriggerPromFromRtaps(BSTR bstrTriggerPromXML , long lPumpNumber)	
{
	BOOL			  bRetCode= TRUE;
	CXMLInterface	  xmlInitDoc;
	CXMLPointers	  pCurrElement,pNode,pPromoSrvElement,pChildNode2, pChildAccount, pChildAccount1, pDiscountNode;
	OLA_STAT		  ola;
	long			  lMemAccount=0,lTotalReward=0,p = 0,lPromNumber=0;	
	int i = 0;
	PAY_AT_PUMP_INFO  cTmpInfo; 
	char * sReceiptText = NULL;
	long lTrsNumber = 0, lReceiptExtraRecIndex = 0; //4.0.19.800 //4.0.23.504 66705
	memset(&ola,0,sizeof(OLA_STAT));
	memset(&cTmpInfo,' ',sizeof(PAY_AT_PUMP_INFO));

	CComBSTR bstrComTriggerPromotions(bstrTriggerPromXML);
	char *strXML = new char[bstrComTriggerPromotions.Length()+2];
	int iLen = Bstr2Buffer(bstrComTriggerPromotions,strXML,bstrComTriggerPromotions.Length()+2);
	try
	{
		long	lRetCode = xmlInitDoc.InitDoc();
		lRetCode = xmlInitDoc.LoadXmlString(strXML);

		//xmlInitDoc.LoadFile()
			
		if(lRetCode != 0)
		{
			

			bRetCode = xmlInitDoc.SelectSingleElementNode("AllProm",&pNode);
			if (bRetCode)
			{
				if (lPumpNumber<0)
					lPumpNumber = xmlInitDoc.GetAttribute("PumpNumber",&pNode);
				p = lPumpNumber - 1;

				_Module.m_server.m_cFuelLoayaltyPS.ClearTimer(p);

				lTrsNumber = xmlInitDoc.GetAttribute("TrsNumber",&pNode);	//4.0.19.800 //4.0.23.504 66705

				if(lTrsNumber == 0)
				{
					CPumpTransact trs;
					_Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&trs);
					lTrsNumber = trs.m_lNumber;
				}
				_Module.m_server.m_cPumpArray[p].GetPAPInfo(&cTmpInfo);

				bRetCode = SaveTriggerPromoationsXml(lPumpNumber,cTmpInfo, xmlInitDoc);

				bRetCode = xmlInitDoc.SelectSingleElementNode("PromoSrvMessage",&pPromoSrvElement,&pNode);

				if (bRetCode)
				{
					//ParseLPEBarcodes(lPumpNumber,xmlInitDoc, pPromoSrvElement, cTmpInfo);

					ParseTotalDiscountFromTriggerXml(lPumpNumber,lTotalReward,xmlInitDoc, pPromoSrvElement,lTrsNumber, cTmpInfo);

					if (_Module.m_server.m_cFuelLoayaltyPS.GetPrintReceipt(p) || (_Module.m_server.GetIFSFTerminalSrvInUse())) //4.0.19.800 //4.0.23.504 66705
						ParseReceiptPrintingFromRtaps(strXML, &sReceiptText);

					UpdateTriggerDiscountWithExtraInfoQdx(lPumpNumber,lTrsNumber, lReceiptExtraRecIndex,lTotalReward, cTmpInfo);

					SaveLoyaltyPrinitingsToReceipt(sReceiptText, lReceiptExtraRecIndex, cTmpInfo);

					ParseTerminalMessagesFromRtaps(strXML, cTmpInfo);

				}
				else
				{
					// no discount
					l2a(0,cTmpInfo.CardSaleInfo.extraData.sGRTotalDiscount,sizeof(cTmpInfo.CardSaleInfo.extraData.sGRTotalDiscount));
				}


				_Module.m_server.m_cPumpArray[p].SetPAPInfo(&cTmpInfo);
			}
			else
			{
				_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseTriggerPromFromRtaps",lPumpNumber,LOG_PUMP,"Missing data from Loyalty server");
			}

			//change the status to end ticket
			_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 

			if (_Module.m_server.GetIFSFTerminalSrvInUse())	//4.0.19.800 //4.0.23.504 66705
			{
				_Module.m_server.m_cPumpArray[p].ClearOlaStat();
			}
			else
			{
				ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_LOYALTY_END_TICKET);
			}

			SetLoyaltyPointsToCardSale(lPumpNumber, lTrsNumber,cTmpInfo); // 4.0.32.220 TD 405631

			_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
			_Module.m_server.m_cPumpArray[p].SetPAPInfo(&cTmpInfo);
		}
		else
		{
			_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseTriggerPromFromRtaps",lPumpNumber,LOG_PUMP,"No data return from Loyalty server");
		}
	}
	catch(...)
	{
		if (lPumpNumber >= 0)
		{
			_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
			ola.m_byState = OLA_NEW_STATE(ola.m_byState, OLA_LOYALTY_END_TICKET);
			_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
			_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseTriggerPromFromRtaps", lPumpNumber, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1);
		}
		bRetCode = FALSE;
	}

	if(strXML)
		delete [] strXML;

	xmlInitDoc.CloseDoc();

	if (_Module.m_server.GetIFSFTerminalSrvInUse())	// 4.0.19.1750 TD  349356
	{
		CMD_STRUCT_SET_PROMOTIONS_EVENT cmdInfo;
		cmdInfo.lPumpNumber = lPumpNumber;
		_Module.m_server.m_cGPIEventMgr.AddEvent(CMD_SET_PROMOTIONS_EVENT, SERVER_POS_NUMBER, lPumpNumber, &cmdInfo, sizeof(cmdInfo));		//4.0.19.1230	
	}



	_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseTriggerPromFromRtaps",lPumpNumber,LOG_PUMP,"finish successfully");
	return bRetCode;
}

/******************************************************************************
 Description: Return Nacs Code
 Parameters:    pPumpItem -   Hold all the grade,nace codes

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			17/5/2005   15:38		Start
******************************************************************************/

long	CFuelLoyaltyPS::GetNacsCodeByGradeCode(PumpItems & pPumpItem,long lGradeCode)
{
	int i=0;
	long lNacsCode = 0;

	while(i<pPumpItem.lCounter)
	{
		if(pPumpItem.m_cItemSoldInfo[i].lGrade == lGradeCode)
		{
			lNacsCode = pPumpItem.m_cItemSoldInfo[i].lNACSCode;
			break;
		}
		i++;			

	}
	return lNacsCode;
}

/******************************************************************************
 Description: Return Additional Code
 Parameters:    pPumpItem -   Hold all the grade,nace codes

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			21/1/2010   13:38		Start //4.0.24.60 103546
******************************************************************************/

INT64	CFuelLoyaltyPS::GetAdditionalCodeByGradeCode(PumpItems & pPumpItem, long lGradeCode)
{
	int i=0;
	INT64 lAdditionalCode = 0;

	while(i<pPumpItem.lCounter)
	{
		if(pPumpItem.m_cItemSoldInfo[i].lGrade == lGradeCode)
		{
			lAdditionalCode = pPumpItem.m_cItemSoldInfo[i].lAdditionalCode;
			break;
		}
		i++;			

	}
	return lAdditionalCode;
}
void	CFuelLoyaltyPS::GetPricesByGradeCode(long lPumpNumber,long lGradeCode, long *lFullServicePrice, long *lSelfServicePrice)
{
	int i=0;
	long lNacsCode = 0;
	*lFullServicePrice = 0;
	*lSelfServicePrice = 0;
	PumpItems  cPumpItemsGrades;

	_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber,cPumpItemsGrades);

	
	while(i<cPumpItemsGrades.lCounter)
	{
		if(cPumpItemsGrades.m_cItemSoldInfo[i].lGrade == lGradeCode)
		{
			*lFullServicePrice = cPumpItemsGrades.m_cItemSoldInfo[i].lPriceCreditFullService;
			*lSelfServicePrice = cPumpItemsGrades.m_cItemSoldInfo[i].lPriceCreditSelfService;
			break;
		}
		i++;			
		
	}

}


void CFuelLoyaltyPS::GetPromotionId(long	lGradeCode,
									long	lPumpIndex,
									long *  lPromId,
									double *  lCentsOff)
{
	for(int i = 0; i < m_Promotion[lPumpIndex].lNumOfPromotionID; i++)
	{
		if(m_Promotion[lPumpIndex].sPromID[i].lGradeNumber == lGradeCode)
		{
			*lPromId = m_Promotion[lPumpIndex].sPromID[i].lPromID;
			if (m_Promotion[lPumpIndex].sPromID[i].bUseDeferral == TRUE)	//4.0.20.54 64528
				*lCentsOff = m_Promotion[lPumpIndex].sPromID[i].fDeferralTotalCentsOff;
			else
				*lCentsOff = m_Promotion[lPumpIndex].sPromID[i].fTotalCentsOff;
		}
	}
}

long CFuelLoyaltyPS::GetNumberOfPromotions(long lPumpIndex,long lGrdaeCode)
{
	for(int i = 0; i < m_Promotion[lPumpIndex].lNumOfPromotionID; i++)
	{
		if(m_Promotion[lPumpIndex].sPromID[i].lGradeNumber == lGrdaeCode)
		{
			return m_Promotion[lPumpIndex].sPromID[i].cPromotionsList.size();
		}
	}
	
	return 0;
}

void CFuelLoyaltyPS::InitSavedLoyaltyInfo(long lPumpNumber) //4.0.20.360 68429
{
	_LOGMSG.LogMsg("CFuelLoyaltyPS", "InitSavedLoyaltyInfo", 0, LOG_PUMP, "Reset all loyalty originated CensOff,carwash promotions", LOG_LEVEL_3);
	long iPumpIndex = lPumpNumber - 1;	//4.0.20.360 68429
	m_Promotion[iPumpIndex].lNumOfPromotionID = 0;
	m_Promotion[iPumpIndex].lPumpNumber = 0;
	m_Promotion[iPumpIndex].lRemainingValueToTrigger = 0;
	for (int iPositionIndex = 0; iPositionIndex < MAX_POSITIONS_PER_PUMP; iPositionIndex++)
	{
		m_Promotion[iPumpIndex].sPromID[iPositionIndex].fTotalCentsOff = 0;
		m_Promotion[iPumpIndex].sPromID[iPositionIndex].lGradeNumber = 0;
		m_Promotion[iPumpIndex].sPromID[iPositionIndex].lPromID = 0;
		m_Promotion[iPumpIndex].sPromID[iPositionIndex].fDeferrableRewardAmount = 0;	//4.0.20.54 64528
		m_Promotion[iPumpIndex].sPromID[iPositionIndex].fDeferralTotalCentsOff = 0; //4.0.20.54 64528
		m_Promotion[iPumpIndex].sPromID[iPositionIndex].bUseDeferral = FALSE; //4.0.20.54 64528
		m_Promotion[iPumpIndex].sPromID[iPositionIndex].fValueToDisplay = 0; //4.0.22.504 69344
		m_Promotion[iPumpIndex].sPromID[iPositionIndex].lOriginalUnitPrice = 0; //4.0.23.650
		m_Promotion[iPumpIndex].sPromID[iPositionIndex].cPromotionsList.clear();
	}
	m_TicketPromotionlist.clear(); // delete all carwash promotions
}


void CFuelLoyaltyPS::ReleaseFuelLoyalty()
{
	if(m_lInitDllFlag)
	{
		if(m_cLoyaltyCommandExecuter)
		{
			m_cLoyaltyCommandExecuter->ReleaseLoyaltyInterface();
			_LOGMSG.LogMsg("CFuelLoyaltyPS","ReleseFuelLoyalty",0,LOG_NONE,"Finish successfully release loyalty fuel.");	
			
			m_lInitDllFlag = FALSE;
		}
	}
}







void CFuelLoyaltyPS::PAKResult(PAY_AT_PUMP_INFO & cTmpInfo,
							   long		lPumpNumber,
							   OLA_STAT &  ola,
							   BOOL bCheckDeferralDiscountFirst)	//4.0.20.54 64528
{
	try
	{
		long p = lPumpNumber-1;
		CString sMsg;
		if ((bCheckDeferralDiscountFirst) && (_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsDeferralDiscountBeforeFuelingPresented())) //4.0.22.508 74810
		{
			//Read ONMP from the registry
			CDeferralDiscountNMPDetails cDeferralDiscount;
			char sMessage[MAX_FIELD_VAL] = {0};
			strcpy_s(sMessage, _countof(sMessage), _Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->GetDeferralDiscountMsg()); //4.0.22.508 74810
			cDeferralDiscount.SetMessage(sMessage);
			ola.m_byState = OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET;
			_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
			cDeferralDiscount.Execute(cTmpInfo, lPumpNumber,ola);
			sMsg.Format("CFuelLoyaltyPS::PAKResult, Loyalty deferral message was prompted for pump %d", lPumpNumber);
			_LOGMSG.LogMsg(sMsg);
		}
		else
		{
			long lTmp = a2l(cTmpInfo.CardSaleExtraData2.sFlags ,sizeof(cTmpInfo.CardSaleExtraData2.sFlags)) + FL_DO_NOT_OPEN_TRS ;   
			
			
			_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 
			ola.m_byState = OLA_NONE;
			
			
			//4.0.14.500
			CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;   //4.0.27.45  - do not work with xml 
			memset(&cmdInfo,' ',sizeof(cmdInfo));
			cmdInfo.lDeviceNumber = '0'; 
			cmdInfo.lSeqNumber = a2l(cTmpInfo.CardSaleInfo.cardData.sTranNbr,sizeof(cTmpInfo.CardSaleInfo.cardData.sTranNbr));	
			cmdInfo.lAnswer =  CD_OK;	
			cmdInfo.lFlags = STRUCT_IS_CARD_ALL_DATA3;
			
		
			memset(&cmdInfo.data,0,sizeof(cmdInfo.data));
			cmdInfo.data     = cTmpInfo.CardSaleInfo.cardData;
			cmdInfo.extData  = cTmpInfo.CardSaleInfo.extraData;
			cmdInfo.extData2 = cTmpInfo.CardSaleExtraData2;
			cmdInfo.extData3 = cTmpInfo.CardSaleExtraData3;
			cmdInfo.extData4 = cTmpInfo.CardSaleExtraData4;
			cmdInfo.extData5 = cTmpInfo.CardSaleExtraData5;
			cmdInfo.extData6 = cTmpInfo.CardSaleExtraData6;

			_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);		
			_Module.m_server.SavePumpInfo(lPumpNumber);
			
			
			_Module.m_server.SetCommand(CMD_SWIPE_RESULT, 
				SERVER_POS_NUMBER, 
				lPumpNumber,
				&cmdInfo,
				sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));
			
			_LOGMSG.LogMsg("CFuelLoyaltyPS","PAKResult",lPumpNumber,LOG_PUMP,"PAKResult Returned CD_OK");
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","PAKResult",0,LOG_NONE,"PAKResult Unexpected Error !!!");
		//		m_pRTI = NULL;	
	}
}

void CFuelLoyaltyPS::CancelLoyaltyTrs(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola, BYTE sTranStatus /* = CD_FORCE_COMPLETION_ZERO*/)
{
	
	long p = lPumpNumber-1;
	long lTmp = a2l(cTmpInfo.CardSaleExtraData2.sFlags ,sizeof(cTmpInfo.CardSaleExtraData2.sFlags)) + FL_DO_NOT_OPEN_TRS ;   
	cTmpInfo.CardSaleInfo.cardData.sTranStatus = sTranStatus;
	
	_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola); 
	ola.m_byState = OLA_NONE;
	_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);
	
	
	//4.0.14.500
	CMD_STRUCT_PAP_SWIPE_RESULT3_EX cmdInfo;   //4.0.27.45  - do not work with xml
	memset(&cmdInfo,' ',sizeof(cmdInfo));
	cmdInfo.lDeviceNumber = '0'; 
	cmdInfo.lSeqNumber = a2l(cTmpInfo.CardSaleInfo.cardData.sTranNbr,sizeof(cTmpInfo.CardSaleInfo.cardData.sTranNbr));	
	cmdInfo.lAnswer =  CD_NOT_OK;	
	cmdInfo.lFlags = STRUCT_IS_CARD_ALL_DATA3;
	
	memset(&cmdInfo.data,0,sizeof(cmdInfo.data));
	cmdInfo.data     = cTmpInfo.CardSaleInfo.cardData;
	cmdInfo.extData  = cTmpInfo.CardSaleInfo.extraData;
	cmdInfo.extData2 = cTmpInfo.CardSaleExtraData2;
	cmdInfo.extData3 = cTmpInfo.CardSaleExtraData3;
	cmdInfo.extData4 = cTmpInfo.CardSaleExtraData4;
	cmdInfo.extData5 = cTmpInfo.CardSaleExtraData5;
	cmdInfo.extData6 = cTmpInfo.CardSaleExtraData6;
	
	
	_Module.m_server.SetCommand(CMD_SWIPE_RESULT, 
		SERVER_POS_NUMBER, 
		lPumpNumber,
		&cmdInfo,
		sizeof(CMD_STRUCT_PAP_SWIPE_RESULT3_EX));
	_LOGMSG.LogMsg("CFuelLoyaltyPS","CancelLoyaltyTrs",lPumpNumber,LOG_PUMP,"Requested Cancel loyalty trs");
	
}

void CFuelLoyaltyPS::SimulStartOlaSession(long lPumpNumber,OLA_STAT & ola)
{
	long p = lPumpNumber-1;
	ola.m_lSessionNumber = 0; 
	ola.m_byState = OLA_REQUEST_TO_SEND | SESSION_AUTHORIZE;
	_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
	_LOGMSG.LogMsg("CFuelLoyaltyPS","SimulStartOlaSession",lPumpNumber,LOG_PUMP,"Start session with ola Card Type credit");
}

//55286 Copient
void CFuelLoyaltyPS::SimulStartFuelingSession(long lPumpNumber,OLA_STAT & ola, CARD_SALE_ALL3 * pcCardSale)
{
	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);//349003

	long p = lPumpNumber - 1;

	_Module.m_server.m_cPumpArray[p].ClearOlaStat();
	ola.m_byState = OLA_LOYALTY_WAIT_FUELING; //4.0.20.611 72527	//4.0.22.500
	_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);

	l2a(PT_GENERIC, pcCardSale->extData3.sPromptType, sizeof(pcCardSale->extData3.sPromptType)); // RFUEL-1149

	_Module.m_server.SavePumpInfo(lPumpNumber);
	_Module.m_server.SendSwipeResult(ola.m_byState, lPumpNumber, CD_OK,CD_APPROVE_OK, pcCardSale);

	 //349003
}

BOOL CFuelLoyaltyPS::ParseCustomerData(long lPumpNumber, char * sCustXml, PAY_AT_PUMP_INFO & cTmpInfo, BOOL & bActiveMember)
{
	CXMLInterface xmlInitDoc, xmlLoyaltyCustomerDoc;
	CXMLPointers  pCurrElement, pCustomerNode, pNode, pPromoSrvNode, pCustomerPropNode, 
					pLoyaltyInfoNode, pMembers, pListMembersNodes, pMember, pCards, 
					pListCardNodes, pCard,pChildNode,pChildNode2, pTmpNode;
	
	CString sMsg;
	long lData = 0;
	BOOL	bRetCode = FALSE;
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "CFuelLoyaltyPS::ParseCustomerData   start");
	try
	{
		bRetCode = xmlInitDoc.InitDoc();
		if (bRetCode)
			bRetCode = xmlInitDoc.LoadXmlString(sCustXml);

		if (bRetCode)
			bRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_CustomerProp, &pCustomerPropNode);
		if (bRetCode)
			bRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_PromoSrvMessage, &pPromoSrvNode, &pCustomerPropNode);
		if (bRetCode)
			bRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_Customer, &pCustomerNode, &pPromoSrvNode);
		else
			bRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_Customer, &pCustomerNode, &pCustomerPropNode);

		if (bRetCode)
		{
			lData = xmlInitDoc.GetAttribute("ClientID", &pCustomerNode); // TD 398329
			if (lData) //4.0.23.1231 306140
			{
				l2a(lData, cTmpInfo.CardSaleExtraData3.sCustomerID, sizeof(cTmpInfo.CardSaleExtraData3.sCustomerID));
			}
			
			xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_StoreID, cTmpInfo.CardSaleExtraData6.sStoreIDField, &pCustomerNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_PosID, cTmpInfo.CardSaleExtraData6.sPosId, &pCustomerNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_TransID, cTmpInfo.CardSaleExtraData6.sTransId, &pCustomerNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_StartDateTime, cTmpInfo.CardSaleExtraData6.sLoyaltyStartDateTime, &pCustomerNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_TicketIdentifier, cTmpInfo.CardSaleExtraData6.sTicketId, &pCustomerNode);

			bRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_LoyaltyInfo, &pLoyaltyInfoNode, &pCustomerNode);

			if (bRetCode) 
			{
				xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_LockingKey, cTmpInfo.CardSaleExtraData5.sLockingKey, &pLoyaltyInfoNode);  // CR 425758
				if (LOG_DETAIL_FLOW_CONTROL)
				{
					CString sLogMsg;
					sLogMsg.Format("CFuelLoyaltyPS::ParseCustomerData, Loyalty locking key = %.40s StoreID = %.24s PosID = %.10s TransID = %.10s was copied to extra info tag", 
						cTmpInfo.CardSaleExtraData5.sLockingKey, 
						cTmpInfo.CardSaleExtraData6.sStoreIDField, 
						cTmpInfo.CardSaleExtraData6.sPosId, 
						cTmpInfo.CardSaleExtraData6.sTransId);
					_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sLogMsg);
				}
			}
		}
		else // MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
		{
			bRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_CustomerInformation, &pChildNode);
			if (bRetCode)
			{
				CXMLPointers pValueNode;
				CString sLogMsg;
				
				//Attach LoyaltyCustomer xml
				bRetCode = xmlLoyaltyCustomerDoc.InitDoc();
				if (bRetCode)
				{
					/*bRetCode = xmlLoyaltyCustomerDoc.CreateElement("CustomerInformation", &pTmpNode);

					if (bRetCode)
					{
						xmlLoyaltyCustomerDoc.AppendRootElement(&pTmpNode);
						xmlLoyaltyCustomerDoc.AppendChild(&pChildNode, &pTmpNode);
					}*/
				}
				xmlLoyaltyCustomerDoc.AppendRootElement(&pChildNode);

				CXMLInterface xmlTmpLoyalty;
				CXmlParser xmlParse;
				xmlTmpLoyalty.InitDoc();
				xmlTmpLoyalty.AppendRootElement(&pChildNode);

				char sPath[MAX_FILE_PATH] = { 0 };
				if (CFuelLoyaltyHelper::GetLoyaltyCustomerFileName(sPath, "", lPumpNumber))
				{
					xmlTmpLoyalty.SaveToFile(sPath);
#ifdef DEBUG
					// print Customerinfo to log  RFUEL -  2628
					char stmmp1[MAX_BUF] = { 0 };
					xmlTmpLoyalty.GetXmlString(stmmp1);
					_LOGMSG.LogMsg(stmmp1, LOG_LEVEL_3);
#endif
				}
				xmlLoyaltyCustomerDoc.CloseDoc();
				xmlTmpLoyalty.CloseDoc();

			}
		}

		if (bRetCode && m_dwCheckMemberStatus)
		{
			bActiveMember = IsMemberActive(lPumpNumber, xmlInitDoc, pLoyaltyInfoNode, cTmpInfo);
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS::ParseCustomerData Unexpected error has occurred", LOG_LEVEL_1); //  4.0.23.980
	}

	xmlInitDoc.CloseDoc();

	sMsg.Format("was finished with bActiveMember == %s", (bActiveMember == TRUE) ? "TRUE" : "FALSE");
	_LOGMSG.LogClassMsg("CFuelLoyaltyPS", "ParseCustomerData", lPumpNumber, LOG_PUMP, sMsg);

	sMsg.Format("was finished with result %s", (bRetCode == TRUE) ? "PASS" : "FAILED");
	_LOGMSG.LogClassMsg("CFuelLoyaltyPS", "ParseCustomerData", lPumpNumber, LOG_PUMP, sMsg);

	return bRetCode;
}

BOOL CFuelLoyaltyPS::IsMemberActive(long lPumpNumber, CXMLInterface & xmlInitDoc, CXMLPointers pLoyaltyInfoNode, PAY_AT_PUMP_INFO & cTmpInfo)
{
	CXMLPointers pMembers, pListMembersNodes, pMember ,pCards, pListCardNodes, pCard;
	CString sMsg;
	BOOL bActiveMember = FALSE;
	
	xmlInitDoc.SelectSingleElementNode(XMLSTR_Members, &pMembers, &pLoyaltyInfoNode);
	xmlInitDoc.GetChildNodesList(&pMembers, &pListMembersNodes);

	if (LOG_FCC_FLOW_CONTROL)
	{
		const int iEncrptSize = sizeof cTmpInfo.CardSaleInfo.extraData.sLoyalId - 4; //RFUEL-1367
		sMsg.Format("CFuelLoyaltyPS::ParseCustomerData, loyaltyId = XXXXXXXXXXXXXXXX%.4s", cTmpInfo.CardSaleInfo.extraData.sLoyalId+ iEncrptSize);
		_LOGMSG.LogMsg(sMsg, LOG_LEVEL_5);
	}

	const int iLenCardId = sizeof(cTmpInfo.CardSaleInfo.extraData.sLoyalId);

	if (pListMembersNodes.m_pMSListOfItems)
	{
		while (xmlInitDoc.GetNextItem(pListMembersNodes, pMember) &&
			(FALSE == bActiveMember))
		{
			int iStatusOfMember = xmlInitDoc.GetAttributeToInt(XMLSTR_Status, &pMember);
			BOOL bMemberIsActive = CheckMemberRestriction(iStatusOfMember);

			if (bMemberIsActive)
			{
				xmlInitDoc.SelectSingleElementNode(XMLSTR_Cards, &pCards, &pMember);
				xmlInitDoc.GetChildNodesList(&pCards, &pListCardNodes);
				if (pListCardNodes.m_pMSListOfItems)
				{
					while (xmlInitDoc.GetNextItem(pListCardNodes, pCard))
					{
						CString sId = xmlInitDoc.GetStringAttribute(XMLSTR_Id, &pCard);
						BOOL bSameCardID = (memcmp(sId.GetBuffer(0), cTmpInfo.CardSaleInfo.extraData.sLoyalId, min(sId.GetLength(), iLenCardId)) == 0);
						if (bSameCardID == FALSE)
							continue;

						int iCardStatus = xmlInitDoc.GetAttributeToInt(XMLSTR_CardStatus, &pCard);
						BOOL bCardIsActive = CheckCardRestriction(iCardStatus);
						if (bCardIsActive)
						{
							bActiveMember = TRUE;
							break;
						}
					}
				}
			}
		}
	}
	else
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, "CFuelLoyaltyPS::ParseCustomerData No customer data was found.");			
	
	return bActiveMember;
}

//if we got here it's mean you had error from startTicket session
//so we continue with regular flow of trs.
void CFuelLoyaltyPS::GetErrorFromRtapsAndContinue(BSTR sXMLError)
{
	char	 strXML[MAX_BUF];
	PAY_AT_PUMP_INFO  cTmpInfo;
	OLA_STAT		  ola;
	CXMLInterface	  xmlInitDoc;
	CXMLPointers	  pCurrElement,pNode;
	long			  lRetCode,lPumpNumber,p,lErrorCode = 0; //4.0.23.510 127468 //4.0.25.30
	char			  strMethod[100];
	char			  strMsg[MAX_FIELD_VAL];


	memset(&ola,0,sizeof(OLA_STAT));
	memset(&strMethod,0,sizeof(strMethod));
	memset(&cTmpInfo,' ' ,sizeof(PAY_AT_PUMP_INFO));

	lRetCode = xmlInitDoc.InitDoc();
	int iLen = WideCharToMultiByte(CP_ACP,0,sXMLError,-1,strXML,sizeof(strXML),NULL,NULL);
	lRetCode = xmlInitDoc.LoadXmlString(strXML);

	if(lRetCode != 0)
	{
		lPumpNumber = xmlInitDoc.GetAttribute("PumpNumber");
		p = lPumpNumber - 1;
		if(p<0)
		{
			_LOGMSG.LogMsg("CFuelLoyaltyPS","GetErrorFromRtapsAndContinue",lPumpNumber,LOG_PUMP,"The PumpNumber that return from Loyalty server is invalid");
		}
		else
		{
			xmlInitDoc.SelectSingleElementNode(XMLSTR_ErrorLine,&pNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_Error,strMethod,&pNode);
			lErrorCode = xmlInitDoc.GetAttribute(XMLSTR_ErrorCode , &pNode);//4.0.23.510 127468 //4.0.25.30
			sprintf_s(strMsg, _countof(strMsg),"Error happend in method %s",strMethod);
			_LOGMSG.LogMsg("CFuelLoyaltyPS","GetErrorFromRtapsAndContinue",lPumpNumber,LOG_PUMP,strMsg);
			_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->SetLoyaltyServerOnline(FALSE);
			
			//RFUEL-520 Emerald Loyalty offline
			if (lErrorCode == LOYALTY_STATE_EMERALD_SERVER_DOWN)
			{
				_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);	
				_Module.m_server.m_cFlowControlManager.SetEmeraldLoyaltyOnlineFlag(FALSE);
				_Module.m_server.m_cFlowControlManager.SaveEmeraldLoyaltyErrorCode(LOYALTY_STATE_EMERALD_SERVER_DOWN);  //RFUEL 797
				_Module.m_server.m_cFlowControlManager.StartAfterAddMemberFlowControl(lPumpNumber, &ola, LOYALTY_STATE_EMERALD_SERVER_DOWN);
				sprintf_s(strMsg, _countof(strMsg), "Emerald Loyalty server is offline");
				_LOGMSG.LogMsg("CFuelLoyaltyPS", "GetErrorFromRtapsAndContinue", lPumpNumber, LOG_PUMP, strMsg);
			}
			else
			{
				_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
			_Module.m_server.m_cPumpArray[p].GetPAPInfo(&cTmpInfo);
			l2a(2,&cTmpInfo.CardSaleExtraData4.loyaltyOnOff,sizeof(cTmpInfo.CardSaleExtraData4.loyaltyOnOff));
			_Module.m_server.m_cPumpArray[p].SetPAPInfo(&cTmpInfo);

			if (lErrorCode != LOYALTY_STATE_TIMEOUT) //4.0.23.510 127468 //4.0.25.30
			{
				SetCancelTrs(lPumpNumber, CancelBeforeStartTicket);	//Copient + RTAPS 59557
				CancelTicket(lPumpNumber);
			}
			_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->SetLoyaltyServerOnline(FALSE);
			_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->StartAfterStartTicketFlowControl(lPumpNumber, &ola, lErrorCode);//Start ticket failed //4.0.22.508 74810 //4.0.23.510 127468 //4.0.25.30
			}
		}
	}

	xmlInitDoc.CloseDoc();	


}


void CFuelLoyaltyPS::GetErrorAfterFuelAndContinue(BSTR sXMLError)
{
	char	 strXML[MAX_BUF];
	PAY_AT_PUMP_INFO  cTmpInfo;
	OLA_STAT		  ola;
	CXMLInterface	  xmlInitDoc;
	CXMLPointers	  pCurrElement,pNode;
	long			  lRetCode,lPumpNumber,p;	//,lMasterM;
	char			  strMethod[100];
	char			  strMsg[MAX_FIELD_VAL];


	memset(&ola,0,sizeof(OLA_STAT));
	memset(&strMethod,0,sizeof(strMethod));
	memset(&cTmpInfo,' ' ,sizeof(PAY_AT_PUMP_INFO));

	lRetCode = xmlInitDoc.InitDoc();
	int iLen = WideCharToMultiByte(CP_ACP,0,sXMLError,-1,strXML,sizeof(strXML),NULL,NULL);
	lRetCode = xmlInitDoc.LoadXmlString(strXML);

	if(lRetCode != 0)
	{ 
		//lRetCode = xmlInitDoc.SelectSingleElementNode("ALLProm",&pNode);
		lPumpNumber = xmlInitDoc.GetAttribute(XMLSTR_PumpNumber);
		p = lPumpNumber - 1;
		if(p<0)
		{
			_LOGMSG.LogMsg("CFuelLoyaltyPS","GetErrorFromRtapsAndContinue",lPumpNumber,LOG_PUMP,"The PumpNumber that return from Loyalty server is invalid");
		}
		else
		{
			xmlInitDoc.SelectSingleElementNode(XMLSTR_ErrorLine,&pNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_Error,strMethod,&pNode);
			sprintf_s(strMsg, _countof(strMsg),"Error happend in method %s",strMethod);
			_LOGMSG.LogMsg("CFuelLoyaltyPS","GetErrorFromRtapsAndContinue",lPumpNumber,LOG_PUMP,strMsg);
			_Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);
			_Module.m_server.m_cPumpArray[p].GetPAPInfo(&cTmpInfo);
			l2a(2,&cTmpInfo.CardSaleExtraData4.loyaltyOnOff,sizeof(cTmpInfo.CardSaleExtraData4.loyaltyOnOff));
			_Module.m_server.m_cPumpArray[p].SetPAPInfo(&cTmpInfo);

			ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_LOYALTY_CANCEL_TICKET_AFTER_START_TICKET);
			_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 
		}
	}
	xmlInitDoc.CloseDoc();

}

void CFuelLoyaltyPS::SetTotalDiscount(long lPumpIdx, long lVal)
{

	if ((lVal < 0) || (lPumpIdx > MAX_PUMPS_64 - 1) || (lPumpIdx < 0))
	{
		char strMsg[MAX_FIELD_VAL];
		sprintf_s(strMsg,MAX_FIELD_VAL,"Error happend in method SetTotalDiscount: invalid parameters");

		_LOGMSG.LogMsg("CFuelLoyaltyPS","SetTotalDiscount",lPumpIdx,LOG_PUMP,strMsg);
	}
	else
		lTotalDiscount[lPumpIdx] = lVal;

};

long CFuelLoyaltyPS::GetTotalDiscount(long lPumpIdx)
{

	if ((lPumpIdx > MAX_PUMPS_64 - 1) || (lPumpIdx < 0))
	{
		char strMsg[MAX_FIELD_VAL];
		sprintf_s(strMsg,MAX_FIELD_VAL,"Error happend in method GetTotalDiscount: invalid parameters");

		_LOGMSG.LogMsg("CFuelLoyaltyPS","GetTotalDiscount",lPumpIdx,LOG_PUMP,strMsg);
		return -1;

	}
	else
		return lTotalDiscount[lPumpIdx];
};

/******************************************************************************
 Description:	Parse receipt xml - build next zone for CL
 Returns:      	 
 Parameters:   	CXMLInterface * pInXML
				CXMLPointers * pXMLReceiptText
				char *  strOutText 
				long lZoneNum

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			20/05/2007	12:30		Start
******************************************************************************/
BOOL CFuelLoyaltyPS::ParseReceiptTextXml(CXMLInterface * pInXML, CXMLPointers * pSecetionElement, char *  strOutText, long lZoneNum)
{
	CXMLPointers	  pCurrElement,pNode,pChildNode, pListNodes;
	BOOL bRet = TRUE, bAlignCenter = FALSE, bDoubleWidth = FALSE, bDoubleHight = FALSE;	//, bBold = FALSE;
	BYTE sAlign[MAX_STRING_LEN];
	BYTE sText[MAX_TERMINAL_MESSAGE_LENGTH];
	char *sTempBuff = new char[MAX_BUF];
	memset(sTempBuff, 0, MAX_BUF);
	long lBold = 0, lCount = 0;

	try
	{
		bRet = pInXML->SelectSingleElementNode(XMLSTR_Object, &pNode, pSecetionElement);
		if (bRet)
		{
			pInXML->GetChildNodesList(&pNode, &pListNodes);
			if (pListNodes.m_pMSListOfItems)
			{
				while (pInXML->GetNextItem(pListNodes,pCurrElement))	//TextAttributes elements
				{
					bRet = pInXML->SelectSingleElementNode(XMLSTR_Text, &pChildNode, &pCurrElement);
					if (bRet)
					{
						pInXML->GetAttributeToByteArrWithConvert(XMLSTR_Align,sAlign,&pChildNode);
						if (!strcmp("Center",(char *)sAlign))
							bAlignCenter = TRUE;

						memset(&sText, '\0', MAX_TERMINAL_MESSAGE_LENGTH);
						pInXML->GetAttributeToByteArrWithConvert(XMLSTR_Text,sText,&pChildNode);

						if (lCount)
							strcat_s(sTempBuff,MAX_BUF,"|");	//New row
						if (lBold)
							strcat_s(sTempBuff,MAX_BUF,"{BOLD_START}");
						if (bDoubleWidth)
							strcat_s(sTempBuff,MAX_BUF,"{DW_START}");
						if (bDoubleHight)
							strcat_s(sTempBuff,MAX_BUF,"{DH_START}");
						if (bAlignCenter)
							strcat_s(sTempBuff,MAX_BUF,"^");

						strcat_s(sTempBuff,MAX_BUF,(char *)sText);
						if (bDoubleHight)
							strcat_s(sTempBuff,MAX_BUF,"{DH_STOP}");	//4.0.22.506
						if (bDoubleWidth)
							strcat_s(sTempBuff,MAX_BUF,"{DW_STOP}");
						if (lBold)
							strcat_s(sTempBuff,MAX_BUF,"{BOLD_STOP}");		
					}
					else
					{
						_LOGMSG.LogMsg("CFuelLoyaltyPS::ParseReceiptTextXml - No Text element found");
					}
					lCount++;
				}
				strcat_s(strOutText,MAX_BUF,sTempBuff);
			}
		}
		if (sTempBuff)	//4.0.20.731	//4.0.22.506
		{
			delete [] sTempBuff;
			sTempBuff = NULL;
		}

	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseReceiptTextXml",0,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1);	
		bRet = FALSE;
	}

	if(lCount == 0)
		return FALSE;

	return bRet;	
}

BOOL CFuelLoyaltyPS::ParseTargetMessageTextXml(CXMLInterface * pInXML, CXMLPointers * pSecetionElement, char *  strOutText)
{
	CXMLPointers	  pCurrElement, pNode, pChildNode, pListNodes;
	BOOL bRet = FALSE;
	BYTE sText[MAX_TERMINAL_MESSAGE_LENGTH];
	char *sTempBuff = new char[MAX_BUF];
	memset(sTempBuff, 0, MAX_BUF);
	

	try
	{
		bRet = pInXML->SelectSingleElementNode(XMLSTR_Object, &pNode, pSecetionElement);
		if (bRet)
		{
			pInXML->GetChildNodesList(&pNode, &pListNodes);
			if (pListNodes.m_pMSListOfItems)
			{
				while (pInXML->GetNextItem(pListNodes, pCurrElement))	//TextAttributes elements
				{
					bRet = pInXML->SelectSingleElementNode(XMLSTR_Text, &pChildNode, &pCurrElement);
					if (bRet)
					{
						memset(&sText, '\0', MAX_TERMINAL_MESSAGE_LENGTH);
						pInXML->GetAttributeToByteArrWithConvert(XMLSTR_Text, sText, &pChildNode);
						strcat_s(strOutText, MAX_BUF, (const char*)sText);
						strcat_s(strOutText, MAX_BUF, "|");
						bRet = TRUE;
					}
					else
					{
						_LOGMSG.LogMsg("CFuelLoyaltyPS::ParseTargetMessageTextXml - No Text element found");
					}
					
				}
				
			}
		}
		if (sTempBuff)	//4.0.20.731	//4.0.22.506
		{
			delete[] sTempBuff;
			sTempBuff = NULL;
		}

	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseTargetMessageTextXml", 0, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1);
		bRet = FALSE;
	}


	return bRet;
}

BOOL CFuelLoyaltyPS::ParseCommunicationRewardTextXml(CXMLInterface * pInXML, CXMLPointers * pSecetionElement, char *  strOutText)
{
	CXMLPointers	  pCurrElement, pNode, pChildNode, pListNodes;
	BOOL bRet = FALSE;
	BYTE sText[MAX_TERMINAL_MESSAGE_LENGTH];
	char *sTempBuff = new char[MAX_BUF];
	memset(sTempBuff, 0, MAX_BUF);


	try
	{
		bRet = pInXML->SelectSingleElementNode(XMLSTR_Object, &pNode, pSecetionElement);
		if (bRet)
		{
			pInXML->GetChildNodesList(&pNode, &pListNodes);
			if (pListNodes.m_pMSListOfItems)
			{
				while (pInXML->GetNextItem(pListNodes, pCurrElement))	//TextAttributes elements
				{
					bRet = pInXML->SelectSingleElementNode(XMLSTR_Text, &pChildNode, &pCurrElement);
					if (bRet)
					{
						memset(&sText, '\0', MAX_TERMINAL_MESSAGE_LENGTH);
						pInXML->GetAttributeToByteArrWithConvert(XMLSTR_Text, sText, &pChildNode);
						strcat_s(strOutText, MAX_BUF, (const char*)sText);
						strcat_s(strOutText, MAX_BUF, "|");
						bRet = TRUE;
					}
					else
					{
						_LOGMSG.LogMsg("CFuelLoyaltyPS::ParseCommunicationRewardTextXml - No Text element found");
					}

				}

			}
		}
		if (sTempBuff)	//4.0.20.731	//4.0.22.506
		{
			delete[] sTempBuff;
			sTempBuff = NULL;
		}

	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseCommunicationRewardTextXml", 0, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1);
		bRet = FALSE;
	}


	return bRet;
}

BOOL CFuelLoyaltyPS::ParseVocher(CXMLInterface * pInXML, CXMLPointers * pSecetionElement, char *  strOutText, long lZoneNum)
{
	CXMLPointers	  pCurrElement,pObjectElement;;
	BYTE sBarCode[40] = {0};
	try
	{
		long lId = pInXML->GetAttribute(XMLSTR_Id,pSecetionElement);
		if(lId == 1 || lId == 3)
			return ParseReceiptTextXml(pInXML,pSecetionElement,strOutText,lZoneNum);
		else if(lId != 2)
			return FALSE;

		BOOL bRet = pInXML->SelectSingleElementNode(XMLSTR_Object, &pObjectElement, pSecetionElement); 
		if (bRet)
		{
			pInXML->GetAttributeToByteArrWithConvert(XMLSTR_Id,sBarCode,&pObjectElement);
			CString sTmp;
			sTmp.Format("{Br,0,0,0,17,0,%s}",sBarCode);
			strcat_s(strOutText,MAX_BUF,sTmp.GetBuffer(0));
		}
		else
			return FALSE;
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseVocher",0,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1);	
		return FALSE;
	}

	return TRUE;	
}

/******************************************************************************
 Description:	Parse receipt printing from rtaps - build zones for CL
 Returns:      	 
 Parameters:   	BSTR strReceiptPrintingXML
				char ** sOutZoneText
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			20/05/2007	12:30		Start
******************************************************************************/


BOOL CFuelLoyaltyPS::ParseReceiptPrintingFromRtaps(char * strReceiptPrintingXML, char ** sOutZoneText)
 {
	BOOL			  bRetCode= TRUE, bFirstFullZone = TRUE;
	CXMLInterface	  xmlInitDoc;
	CXMLPointers	  pPrintMessagesElement,pAllPromNode,pReceiptPrintingNode, pListNodes, pCurrPrintElement, pCurrSectionElement, pCurrDocumentElement, pListPrintNodes, pListSectionNodes, pListDocumentNodes;
	int iZoneIndex = 1,iNumberOfZones = 1;
	long	p = 0,lPumpNumber=0,lPromNumber=0;

	char *	sAllText = new char[MAX_BUF];
	memset(sAllText, 0, sizeof(sAllText));	//4.0.20.731	//4.0.22.506

	PAY_AT_PUMP_INFO  cTmpInfo; 
	memset(&cTmpInfo,' ',sizeof(PAY_AT_PUMP_INFO));
	OLA_STAT		  ola;
	memset(&ola,0,sizeof(OLA_STAT));

	try
	{
		long	lRetCode = xmlInitDoc.InitDoc();
		lRetCode = xmlInitDoc.LoadXmlString(strReceiptPrintingXML);
			
		if(lRetCode != 0)
		{
			bRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_ALLProm,&pAllPromNode);
			if (bRetCode)
			{
				lPumpNumber = xmlInitDoc.GetAttribute(XMLSTR_PumpNumber,&pAllPromNode);
				p = lPumpNumber - 1;

				if (LOG_FCC_FLOW_CONTROL)
				{
					char strFileName[HOME_DIR_STRING_LENGTH];
					sprintf_s(strFileName, _countof(strFileName),"C:\\LPE\\XML\\PS_ReceiptPrintingFromRtaps_%ld.xml",GetTotalDiscount(p));
					xmlInitDoc.SaveToFile(strFileName);
				}
				bRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_ReceiptPrinting,&pReceiptPrintingNode,&pAllPromNode);

				if (bRetCode)
				{
					if (_Module.m_server.GetIFSFTerminalSrvInUse())	//4.0.19.710 66705 //4.0.23.504 66705
					{
						ParseSppsPromotions(xmlInitDoc, pReceiptPrintingNode, pListNodes,sAllText);
					}
					else
					{
						xmlInitDoc.GetChildNodesList(&pReceiptPrintingNode, &pListNodes);
						if (pListNodes.m_pMSListOfItems)
						{
							while (xmlInitDoc.GetNextItem(pListNodes,pPrintMessagesElement))	//PrintMessages elements
							{
								long lExistsReceiptPrinting = 0;
								char sReceiptText[MAX_BUF] = {0};
								BYTE sPrintingType[100] = {0};
								xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_PrintingType,sPrintingType,&pPrintMessagesElement);

								SetZoneIndexAccordingToLoyaltyMessageType(xmlInitDoc, pPrintMessagesElement, iNumberOfZones, iZoneIndex);
								sprintf_s(sReceiptText,MAX_BUF,"<B%ld>", iZoneIndex);

								xmlInitDoc.GetChildNodesList(&pPrintMessagesElement, &pListPrintNodes);
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
													while (xmlInitDoc.GetNextItem(pListSectionNodes,pCurrSectionElement) && (iNumberOfZones < 20)) //Section elements
													{
														if(IsVoucher((const char*)sPrintingType))
														{
															if(ParseVocher(&xmlInitDoc, &pCurrSectionElement, sReceiptText,iZoneIndex))
															{
																strcat_s(sReceiptText,MAX_BUF,"|");
																lExistsReceiptPrinting ++;
															}
														}
														else if(ParseReceiptTextXml(&xmlInitDoc, &pCurrSectionElement, sReceiptText,iZoneIndex))
														{
															strcat_s(sReceiptText,MAX_BUF,"|");
															lExistsReceiptPrinting ++;
														}
													}
												}
											}
										}
									}
								}

								char sClosingZone[MAX_STRING_LEN];
								sprintf_s(sClosingZone,MAX_STRING_LEN,"<\\B%ld>",iZoneIndex);
								strcat_s(sReceiptText,MAX_BUF,sClosingZone);

								if (bFirstFullZone)
								{
									strncpy(sAllText,sReceiptText,MAX_BUF);
									bFirstFullZone = FALSE;
								}
								else
								{
									if (lExistsReceiptPrinting > 0) //4.0.23.260 110902 //4.0.24.60
										strncat(sAllText,sReceiptText,MAX_BUF);
								}
								if (lExistsReceiptPrinting >0) //4.0.23.260 110902 //4.0.24.60
									iNumberOfZones++;


							} // end of print messages while 
						}
						else
						{
							_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseReceiptPrintingFromRtaps",lPumpNumber,LOG_PUMP,"No receipt printings received from Loyalty server");
						}
					}
				}
				else
				{
					_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseReceiptPrintingFromRtaps",lPumpNumber,LOG_PUMP,"No receipt printings received from Loyalty server");

				}
			}
			else
				_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseReceiptPrintingFromRtaps",lPumpNumber,LOG_PUMP,"Missing data from Loyalty server");

		}else
			_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseTriggerPromFromRtaps",lPumpNumber,LOG_PUMP,"No data return from Loyalty server");
		
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseTriggerPromFromRtaps",lPumpNumber,LOG_PUMP,"Unexpected Error Had occurred ");
		bRetCode = FALSE;
	}

	_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseTriggerPromFromRtaps",lPumpNumber,LOG_PUMP,"finish successfully");
	
	if (strlen(sAllText))
		*sOutZoneText = sAllText;
	else
	{
		*sOutZoneText = NULL;
		if (sAllText)//4.0.20.731	//4.0.22.506
		{
			delete [] sAllText;
			sAllText = NULL;
		}
	}

	CString cStrMsg;
	cStrMsg.Format("CFuelLoyaltyPS::ParseTriggerPromFromRtaps created receipt zones: %.1000s ...",*sOutZoneText);
	_LOGMSG.LogMsg(cStrMsg);

	xmlInitDoc.CloseDoc();
	return bRetCode;
}

BOOL CFuelLoyaltyPS::IsOLAInLoyaltyNMPState(long lPumpNumber)
{
	return (IsOLAInLoyaltyNMPStateAfterStartTicket(lPumpNumber) || IsOLAInLoyaltyNMPStateBeforeStartTicket(lPumpNumber));
}


//4.0.20.730 74977
void CFuelLoyaltyPS::SetCancelTrs(long lPumpNumber, LoyaltyCancelTime eVal)
{
	m_eaCancelTrs[lPumpNumber - 1] = eVal;
	CString cStrMsg;
	cStrMsg.Format("CFuelLoyaltyPS::SetCancelTrs LoyaltyCancelTime = %d",eVal);
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,cStrMsg);

	
}

//For the use of cancel trs. In those states CancelTicket needs to be called
BOOL CFuelLoyaltyPS::IsOLAInLoyaltyNMPStateAfterStartTicket(long lPumpNumber)
{
	OLA_STAT ola;
	CPumpTransact trsLoyalty;	//4.0.20.620 72527	//4.0.22.500
	_Module.m_server.m_cPumpArray[lPumpNumber- 1].GetOlaStat(&ola); 
	if ((ola.m_byState == (OLA_LOYALTY_START_TICKET | SESSION_AUTHORIZE)) ||
		(ola.m_byState == (OLA_LOYALTY_WAIT_RESPONSE | SESSION_AUTHORIZE)) ||
		(ola.m_byState == (OLA_LOYALTY_WAIT_RESPONSE | SESSION_COMPLETE)) ||
		(ola.m_byState == OLA_LOYALTY_WAIT_PHONE_LOOKUP_RESPONSE)|| //4.0.23.1661 349768
		(ola.m_byState == (OLA_LOYALTY_RUN_QUERY | SESSION_AUTHORIZE)) || //4.0.23.20
		(ola.m_byState == (OLA_LOYALTY_WAIT_ADD_MEMBER_INFO_RESPONSE | SESSION_AUTHORIZE)) || // 4.0.23.2280. 305114
		(ola.m_byState == (OLA_LOYALTY_WAIT_GET_MEMBER_INFO_RESPONSE | SESSION_AUTHORIZE)) || // 4.0.23.2280. 305114
		(ola.m_byState == (OLA_LOYALTY_ADD_MEMBER_CARD | SESSION_AUTHORIZE)) || //4.0.23.20
		//(ola.m_byState == (OLA_LOYALTY_APPLY_PAYMENT_DISCOUNT | SESSION_AUTHORIZE)) ||
		(ola.m_byState == OLA_WAIT_CONTROLLER_MSG)||
		(ola.m_byState == OLA_LOYALTY_WAIT_FUELING) ||	//4.0.20.611 72527	//4.0.22.500
		(ola.m_byState ==  OLA_LOYALTY_START_NMP_BEFORE_FUELING_AFTER_START_TICKET)||				
		(ola.m_byState ==  OLA_LOYALTY_CW_BEFORE_FUELING_AFTER_START_TICKET		)||			
		(ola.m_byState ==  OLA_LOYALTY_COUPONS_BEFORE_FUELING_AFTER_START_TICKET	)||			
		(ola.m_byState ==  OLA_LOYALTY_MEMBER_CARD_BEFORE_FUELING_AFTER_START_TICKET	)||		
		(ola.m_byState ==  OLA_LOYALTY_PROMPT_BEFORE_FUELING_AFTER_START_TICKET	)||			
		(ola.m_byState ==  OLA_LOYALTY_START_NMP_AFTER_FUELING_BEFORE_ADD_ITEM	)||				
		(ola.m_byState ==  OLA_LOYALTY_CW_AFTER_FUELING_BEFORE_ADD_ITEM		)||				
		(ola.m_byState ==  OLA_LOYALTY_COUPONS_AFTER_FUELING_BEFORE_ADD_ITEM		)||			
		(ola.m_byState ==  OLA_LOYALTY_MEMBER_CARD_AFTER_FUELING_BEFORE_ADD_ITEM	)||			
		(ola.m_byState ==  OLA_LOYALTY_PROMPT_AFTER_FUELING_BEFORE_ADD_ITEM	) ||
		(ola.m_byState ==  OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET) ||	//4.0.20.61	65210
		(ola.m_byState ==  OLA_LOYALTY_WAIT_FOR_DEFERAL_ANSWER) || // 4.0.23.???? TD 295237
		(_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsNMPStateAfterStartTicket(ola.m_byState)) ||//4.0.22.500 //4.0.22.508 74810
		(ola.m_byState ==  OLA_NONE) )//4.0.23.504 85970 && (!_Module.m_server.m_cPumpArray[lPumpNumber-1].GetInProgressWaitProgressTrs(&trsLoyalty))) ||	//4.0.20.620 72527	//4.0.22.500
	{
		return TRUE;
	}
	return FALSE;
}

//For the use of cancel trs. In those states CancelTicket does not need to be called
BOOL CFuelLoyaltyPS::IsOLAInLoyaltyNMPStateBeforeStartTicket(long lPumpNumber)
{
	OLA_STAT ola;
	CPumpTransact trsLoyalty;	//4.0.20.620 72527	//4.0.22.500
	_Module.m_server.m_cPumpArray[lPumpNumber- 1].GetOlaStat(&ola); 
	if ((ola.m_byState == OLA_LOYALTY_WAIT_MORE_DATA) ||
		//(ola.m_byStat ==  OLA_NONE)||		//Liat copient 4.0.20.450 66977 68857
		//4.0.23.504 85970((ola.m_byStat ==  OLA_NONE) && (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetInProgressWaitProgressTrs(&trsLoyalty))) || //4.0.20.620 72527	//4.0.22.500
		(ola.m_byState ==  OLA_LOYALTY_START_NMP_BEFORE_FUELING_BEFORE_START_TICKET	)||		//Liat copient
		(ola.m_byState ==  OLA_LOYALTY_CW_BEFORE_FUELING_BEFORE_START_TICKET		)||			//Liat copient
		(ola.m_byState ==  OLA_LOYALTY_COUPONS_BEFORE_FUELING_BEFORE_START_TICKET		)||		//Liat copient
		(ola.m_byState ==  OLA_LOYALTY_MEMBER_CARD_BEFORE_FUELING_BEFORE_START_TICKET	)||		//Liat copient
		(ola.m_byState ==  OLA_LOYALTY_PROMPT_BEFORE_FUELING_BEFORE_START_TICKET	) ||			//Liat copient
		(ola.m_byState ==  OLA_LOYALTY_WAIT_CONTROLLER_MSG_START_TICKET_FAIL	) ||			//4.0.22.504
		(_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsNMPStateBeforeStartTicket(ola.m_byState)))//4.0.22.500 //4.0.22.508 74810
	{
		return TRUE;
	}
	return FALSE;
}

//4.0.22.504 69344 // 4.0.23.1052 TD 270124 
double CFuelLoyaltyPS::GetDeferralValueToDisplayOnPump(long lPumpNumber)	
{
	// return the first value to display
	double fValue = 0;

	for(int iProm=0; iProm < MAX_PROM_PER_PUMP; iProm++)
	{
		fValue = m_Promotion[lPumpNumber - 1].sPromID[iProm].fValueToDisplay;
		if (fValue == 0)
		{
			if (m_Promotion[lPumpNumber - 1].sPromID[iProm].bUseDeferral == TRUE)
			{
				fValue = m_Promotion[lPumpNumber - 1].sPromID[iProm].fDeferrableRewardAmount;
			}
		}

		if(fValue != 0)
			break;
			
	}

	return fValue;
}
//4.0.20.54 start 64528
//4.0.22.504 69344 // 4.0.23.1052 TD 270124
double CFuelLoyaltyPS::GetDeferralDiscountForPump(long lPumpNumber)	
{
	double fDeferralDiscount =0;

	for(int iProm=0; iProm < MAX_PROM_PER_PUMP; iProm++)
	{
		fDeferralDiscount = m_Promotion[lPumpNumber - 1].sPromID[iProm].fDeferrableRewardAmount;

		if (fDeferralDiscount)
			break;
	}

	return fDeferralDiscount;
}

//4.0.22.504 69344 // 4.0.23.1052 TD 270124
BOOL CFuelLoyaltyPS::IsDeferralDiscountSelected(long lPumpNumber)
{
	BOOL bUseDeferral;
	
	for(int iProm = 0; iProm < MAX_POSITIONS_PER_PUMP; iProm++)
	{
		bUseDeferral = m_Promotion[lPumpNumber - 1].sPromID[iProm].bUseDeferral;
		
		if (bUseDeferral)
			return TRUE;			
	}
	
	return FALSE;
}
void CFuelLoyaltyPS::UpdateDeferralDiscountPerGrade(long lPumpNumber, PAY_AT_PUMP_INFO & cTmpInfo)
{
	long p = lPumpNumber - 1;
	for (int i = 0 ; i < MAX_PROM_PER_PUMP; i++)
	{
		if (m_Promotion[p].sPromID[i].fDeferrableRewardAmount > 0)
		{
			m_Promotion[p].sPromID[i].bUseDeferral = TRUE;
			_Module.m_server.UpdateDiscountPerGrade(m_Promotion[p].sPromID[i].lGradeNumber, lPumpNumber, m_Promotion[p].sPromID[i].fDeferralTotalCentsOff*10, cTmpInfo);
			UpdatePumpWithLoyaltyVolumeLimit(cTmpInfo, lPumpNumber);	//4.0.20.300
		}
	}
}
//4.0.20.54 end

//4.0.19.503 Copient + Rtaps end 59557

/******************************************************************************
 Description:	Gets the item price according to the configured service mode
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Omer			17/10/2011	 19:34		Start 4.0.23.1052 TD 270145
******************************************************************************/
long CFuelLoyaltyPS::GetItemPrice(long lPumpNumber , PumpItems * pInfo , int iItem)
{
	long lUnitPrice = 0;

	long lServiceMode = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetModeService();	//the default is self service = 0

	if (lServiceMode == 0)	//SelfService
		lUnitPrice = pInfo->m_cItemSoldInfo[iItem].lPriceCreditSelfService;
	else
		lUnitPrice = pInfo->m_cItemSoldInfo[iItem].lPriceCreditFullService;
	
	return lUnitPrice;
}

/******************************************************************************
 Description:	Runs phone lookup with loyalty fuel.
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			12/10/2011       		Start			4.0.23.1220 TD 155754
******************************************************************************/
BOOL CFuelLoyaltyPS::RunPhoneLookUp(long lPumpNumber, BYTE * sPhoneNumber)
{
	CComBSTR bstrXMLPhoneNumber( (char * ) sPhoneNumber);
	
	m_cLoyaltyCommandExecuter->PhoneLookUp( bstrXMLPhoneNumber.m_str , lPumpNumber);

	char sMsg[MAX_MSG_LEN];
	sprintf_s(sMsg, _countof(sMsg),"CFuelLoyaltyPS::RunPhoneLookUp - Run Lookup for for phone number %s.",sPhoneNumber);
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);

	return TRUE;
}

/******************************************************************************
 Description: Handles result club card received from RTAPS
			  Move to next state (authorize loyalty) and continue flow from there
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			12/10/2011       		Start			4.0.23.1220 TD 155754
*****************************************************************************/
void CFuelLoyaltyPS::PhoneLookUpResult(long lPumpNumber , int iRetCode , BYTE * strClubCard)
{
	OLA_STAT ola;
	memset(&ola,0,sizeof(OLA_STAT));
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetOlaStat(&ola);

	PAY_AT_PUMP_INFO cInfo;
	memset(&cInfo,' ' ,sizeof(PAY_AT_PUMP_INFO));
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cInfo);

	long lMemberRes = 0;
	switch(iRetCode)
	{
		case _PhoneNumberFound:
		{
			memcpy(cInfo.CardSaleInfo.extraData.sLoyalId,strClubCard,sizeof(cInfo.CardSaleInfo.extraData.sLoyalId));

			char sMsg[MAX_MSG_LEN];
			sprintf_s(sMsg, _countof(sMsg),"CFuelLoyaltyPS::PhoneLookUpResult - Phone Number Found, Received clubcard number %s.",strClubCard);
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);

			ola.m_byState = OLA_LOYALTY_PHONE_LOOKUP;
			_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola); 
			
			_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cInfo, TRUE);

			break;
		}
		
		case _PhoneNumberNotFound:
		{
			CString sMsg;
			sMsg.Format("CFuelLoyaltyPS::PhoneLookUpResult - Phone Number NOT Found, Received clubcard number %s.", strClubCard);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg);

			ola.m_byState = OLA_LOYALTY_PHONE_LOOKUP;
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].SetOlaStat(&ola);

			cInfo.CardSaleInfo.cardData.sEntryMethod = ' ';
			_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cInfo, FALSE);

			lMemberRes = LOYALTY_STATE_PHONE_NUMBER_NOT_FOUND;
			break;
		}
		case _MultipleMembersFound:
			lMemberRes = LOYALTY_STATE_MULTIPLE_MEMBER_FOUND;
			break;
		case _LoyaltyHostOffline:
			lMemberRes = LOYALTY_STATE_LOYALTY_SERVER_DOWN;
			break;
	}

	if ((iRetCode != _PhoneNumberFound) &&
		(iRetCode != _PhoneNumberNotFound))
	{
		_Module.m_server.m_cFlowControlManager.SetPumpSrvAddMemberCardRes(lPumpNumber, lMemberRes);
		_Module.m_server.m_cFlowControlManager.StartAfterAddMemberFlowControl(lPumpNumber,&ola,lMemberRes);

	}
}




void CFuelLoyaltyPS::SetdeferralDiscount(long lPumpNumber,double fDeferralDiscount)	
{
	int iPumpIndex = lPumpNumber - 1;
	
	for(int iProm = 0; iProm < MAX_POSITIONS_PER_PUMP; iProm++)
	{	
		if((m_Promotion[iPumpIndex].sPromID[iProm].fDeferrableRewardAmount > 0) && fDeferralDiscount < (m_Promotion[iPumpIndex].sPromID[iProm].fDeferrableRewardAmount))
		{
			m_Promotion[iPumpIndex].sPromID[iProm].fDeferrableRewardAmount = fDeferralDiscount;
		}
		
		if( (m_Promotion[iPumpIndex].sPromID[iProm].fDeferralTotalCentsOff > 0) && fDeferralDiscount < (m_Promotion[iPumpIndex].sPromID[iProm].fDeferralTotalCentsOff))
		{
			m_Promotion[iPumpIndex].sPromID[iProm].fDeferralTotalCentsOff = fDeferralDiscount;
		}
		
		for (PromotionIterator it = m_Promotion[iPumpIndex].sPromID[iProm].cPromotionsList.begin(); it != m_Promotion[iPumpIndex].sPromID[iProm].cPromotionsList.end(); ++it)
		{	
			if(it->GetDeferrableRewardAmount() > 0)
				it->SetRewardAmountDeferredReward(fDeferralDiscount);
		}
	}
}

/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			09/09/2012       		Start			4.0.23.???? TD 295237
******************************************************************************/
void CFlowControlManager::HandleManualDeferal(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo)
{
	CNMPDetails * cNmpObj = GetPumpFlow(lPumpNumber)->GetNMPObject(NMP_TYPE_DeferralDiscount);
	
	CDeferralDiscountNMPDetails* cDeferalNMPObject = dynamic_cast<CDeferralDiscountNMPDetails*>(cNmpObj);
	
	if(cDeferalNMPObject != NULL)
	{
		cDeferalNMPObject->HandleManualDeferral(lPumpNumber,pOla,pInfo);
	}
}

void CFuelLoyaltyPS::ClearTimer(long lPumpIndex)
{
	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[lPumpIndex].GetPAPInfo(&info);
	
	info.CardSaleExtraData5.sLoyalyTimerCode = '0';
	
	memset(&info.CardSaleExtraData5.sLoyalyTime,'0',sizeof(info.CardSaleExtraData5.sLoyalyTime));
	
	_Module.m_server.m_cPumpArray[lPumpIndex].SetPAPInfo(&info);
	
}


void CFuelLoyaltyPS::SetStartTicketTime(IN long lPumpIndex, IN SYSTEMTIME st)
{

	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();

	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[lPumpIndex].GetPAPInfo(&info);

	TAG_SYSTEMTIME pTagTime;
	SystemTime2Tag(&st, &pTagTime);
	memcpy((void *)&info.CardSaleExtraData6.sLoyaltyStartTime, &pTagTime, sizeof(info.CardSaleExtraData6.sLoyaltyStartTime));

	_Module.m_server.m_cPumpArray[lPumpIndex].SetPAPInfo(&info);


	SQLPumpSrvFile.SavePAPInfo(lPumpIndex + 1, __FUNCTION__, __LINE__);


}

void CFuelLoyaltyPS::SetTimer(IN long lPumpIndex, IN long lCode)
{

	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();

	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[lPumpIndex].GetPAPInfo(&info);

	l2a(lCode,&info.CardSaleExtraData5.sLoyalyTimerCode,1);

	SYSTEMTIME st;
	GetLocalTime(&st);	
	TAG_SYSTEMTIME pTagTime;
	SystemTime2Tag(&st,&pTagTime);
	
	memcpy((void *)&info.CardSaleExtraData5.sLoyalyTime,&pTagTime,sizeof(info.CardSaleExtraData5.sLoyalyTime));

	_Module.m_server.m_cPumpArray[lPumpIndex].SetPAPInfo(&info);


	SQLPumpSrvFile.SavePAPInfo(lPumpIndex + 1, __FUNCTION__, __LINE__); //OmerT - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling

}

void CFuelLoyaltyPS::GetTimer(IN long lPumpIndex,OUT long & lCode , OUT CTime & cTime)
{
	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[lPumpIndex].GetPAPInfo(&info);

	SYSTEMTIME st;
	TAG_SYSTEMTIME pTagTime;
	memcpy(&pTagTime,&info.CardSaleExtraData5.sLoyalyTime,sizeof(info.CardSaleExtraData5.sLoyalyTime));
	Tag2SystemTime(&pTagTime,&st);

	cTime = st;
	lCode = a2l(&info.CardSaleExtraData5.sLoyalyTimerCode,sizeof(info.CardSaleExtraData5.sLoyalyTimerCode));
	
}



// 4.0.23.2100
CString CFuelLoyaltyPS::GetStringTimeoutCode(long lCode)
{
	switch(lCode)
	{
	case ADD_ITEM_CHECK_TIMEOUT:
		return "AddItem";
	case END_TICKET_CHECK_TIMEOUT:
		return "EndTicket";
	case START_TICKET_CHECK_TIMEOUT:
		return "StartTicket";
	case ADD_MEMBER_CARD_CHECK_TIMEOUT:
		return "AddMember";
	case ADD_ADD_MEMBER_INFO_CHECK_TIMEOUT:
		return "AddMemberInfo";
	case QUERY_CHECK_TIMEOUT: // TD 406362  4.0.23.2390
			return "Query";
	case ADD_CARWASH_ITEM_CHECK_TIMEOUT:
			return "AddCarWashItem";
	case ADD_GET_MEMBER_INFO_CHECK_TIMEOUT:
		return "GetMemberInfo";
	default:
		return "";
	}
}

/******************************************************************************
 Description: Convert from integer state to string state and prints it into the log	
 Returns:      	

 Parameters: iCurrState - current loyalty state
			 iNextState - next loylaty state
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			12/10/2011       		Start			4.0.23.1220 TD 155754
******************************************************************************/
void CFuelLoyaltyPS::PrintLoyaltyStateToLog(long lPumpNumber , int iCurrState  , int iNextState)
{
	CString sLogMsg("Loyalty State Machine, Changing From [");
	const CString StateFrom = _LOGMSG.GetStateString(iCurrState);
	const CString StateTo = _LOGMSG.GetStateString(iNextState);
	sLogMsg = sLogMsg + StateFrom + "] to [" + StateTo + "]";

	/*
	for(int i = 0; i < 2; i++)
	{
		// get the string for current state
		int iState = iCurrState;

		// get the string the next state
		if(i == 1)
		{
			iState = iNextState;
			sLogMsg += "] TO [";
			
		}
		sLogMsg += _LOGMSG.GetStateString(iState);
	}	
	sLogMsg += "]";
	*/
	_LOGMSG.LogMsg(lPumpNumber , LOG_PUMP ,  sLogMsg);
}

/******************************************************************************
 Description: Handle Member Info event from loayltFuel (move to query)
			  
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			23/03/2014       		Start			4.0.23.2280 305114
*****************************************************************************/
BOOL CFuelLoyaltyPS::HandleAddMemberInfoResult(long lPumpNumber)
{
	long p = lPumpNumber -1;
	OLA_STAT ola;

	SetAddMemberInfo(lPumpNumber,0);
	
	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);
	memset(&ola,0,sizeof(OLA_STAT));
	
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetOlaStat(&ola);

	PrintLoyaltyStateToLog(lPumpNumber , ola.m_byState , OLA_LOYALTY_RUN_QUERY);

	ola.m_byState = OLA_LOYALTY_RUN_QUERY | SESSION_AUTHORIZE;

	_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);

	

	return TRUE;
}



/******************************************************************************
 Description: ParsePromotions received from RTAPS
			  
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			23/03/2014       		Start			4.0.23.2280 305114
*****************************************************************************/
void CFuelLoyaltyPS::ParsePromotions( long lPumpNumber,CXMLInterface &xmlInitDoc, CXMLPointers pRootNode, 
									  long lUseAdditionalCode, CXMLPointers pAdditionalCodeNode ,PAY_AT_PUMP_INFO  &cTmpInfo)
{
	int				iPositionIndex = 0;
	double			fRewardAmountDeferredReward = 0,
					fDeferrableRewardAmount = 0,
					fCenstOff = 0,
					fOriginalRewardField;
	long			p = lPumpNumber - 1,
					lPromID = 0,
					lAskDeferral = 0,
					lManualDeferral = 0,
					lNacsCode = 0,
					lGradeCode = 0,
					lRetCode = 0;
	CString			sPromDescription = "";
	PumpItems		cPumpItemsGrades;
	CXMLPointers    pChildNode, pCurrElement, pChildNode2;
	CString strMsg;
	strMsg.Format("ParsePromotions %d ", lPumpNumber);
	_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParsePromotions", 0, LOG_NONE, strMsg);


	m_Promotion[p].lPumpNumber = lPumpNumber;
	m_Promotion[p].lNumOfPromotionID = 0;
	_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber, cPumpItemsGrades);

	xmlInitDoc.GetChildNodesList(&pRootNode, &pChildNode);
	if (pChildNode.m_pMSListOfItems)
	{
		BOOL bCheckVolumeLimit = FALSE;	//4.0.20.300

		while (xmlInitDoc.GetNextItem(pChildNode, pCurrElement))
		{
			fCenstOff = xmlInitDoc.GetAttribute(XMLSTR_RewardAmount, &pCurrElement);
			fRewardAmountDeferredReward = xmlInitDoc.GetAttribute(XMLSTR_RewardAmountDeferredReward, &pCurrElement);
			fDeferrableRewardAmount = xmlInitDoc.GetAttribute(XMLSTR_DeferrableRewardAmount, &pCurrElement);
			lPromID = xmlInitDoc.GetAttribute(XMLSTR_PromNumber, &pCurrElement);
			lAskDeferral = xmlInitDoc.GetAttribute(XMLSTR_AskDeferral, &pCurrElement);	//4.0.22.504 69344 for LPE only
			fOriginalRewardField = xmlInitDoc.GetAttribute(XMLSTR_OriginalRewardField, &pCurrElement);	//4.0.22.504 69344 for LPE only
			lManualDeferral = xmlInitDoc.GetAttribute(XMLSTR_ManualEntry, &pCurrElement);	//4.0.22.504 69344 for LPE only

																						// MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling 
			sPromDescription = xmlInitDoc.GetAttribute(XMLSTR_Description, &pCurrElement);//Nac's Code or Additional code

			if (fCenstOff > 0)
				bCheckVolumeLimit = TRUE;

			lRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_AttachedItem, &pChildNode2, &pCurrElement);
			if (lRetCode)  //  RFUEL-2639 PAP - Incorrect Promotions being applied on items with fuel.  Don't create triggered promotion if no item is attached. 
			{
				//4.0.23.640 136174
				lNacsCode = xmlInitDoc.GetAttribute(XMLSTR_PluCode, &pChildNode2);//Nac's Code or Additional code

				lGradeCode = _Module.m_server.GetGradeCodeByNacsCode(cPumpItemsGrades, lNacsCode);
				if (lUseAdditionalCode)	//4.0.23.960 232007
				{
					INT64 lAdditionalCodeRes = xmlInitDoc.GetAttribute(XMLSTR_Result, &pAdditionalCodeNode);
					if (lAdditionalCodeRes)
					{
						lGradeCode = GetGradeCodeByAdditionalCode(cPumpItemsGrades, lNacsCode);
					}
				}

				iPositionIndex = GetGradeIndexInPumpPromotionsArray(lPumpNumber, lGradeCode);

				CPromotion cPromotion(lPromID, fCenstOff, fRewardAmountDeferredReward, fDeferrableRewardAmount, fOriginalRewardField, lAskDeferral, lManualDeferral, lNacsCode, sPromDescription);

				if (_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsDeferralDiscountInFlow()) //4.0.22.508 74810
				{
					cPromotion.UpdateDeferralRewards();
				}

				if (lNacsCode > 0)
					SavePromotions(lPumpNumber, iPositionIndex, cTmpInfo, lGradeCode, cPromotion, lNacsCode, sPromDescription);
			}
		}


		// TODO: Save customer data to pap info (card sale data 6)
		if (bCheckVolumeLimit)	//4.0.20.300
		{
			UpdatePumpWithLoyaltyVolumeLimit(cTmpInfo, lPumpNumber);
			_Module.m_server.m_cPumpArray[p].SetPAPInfo(&cTmpInfo);
		}

		if (m_dwLoyaltyEngineType == EMERALD)  // Add flag for Auto authorize loyalty only
		{
			long lPumpAutoStart = 1;
			l2a(lPumpAutoStart, &cTmpInfo.CardSaleExtraData5.sPumpAutoStart, sizeof(cTmpInfo.CardSaleExtraData5.sPumpAutoStart));
		}
	}
	else
	{
		CString strMsg;
		strMsg.Format("No cents off at pump %d ", lPumpNumber);
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParsePromotions", 0, LOG_NONE, strMsg);
	}
}

/******************************************************************************
 Description: SavePromotions received from RTAPS
			  
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			23/03/2014       		Start			4.0.23.2280 305114
*****************************************************************************/
void CFuelLoyaltyPS::SavePromotions(long lPumpNumber, int iPositionIndex, PAY_AT_PUMP_INFO &cTmpInfo, long lGradeCode, CPromotion cPromotion, long lPluCode, CString sPromDescription)
{
	long p = lPumpNumber - 1;
	char			  strMsg[MAX_FIELD_VAL];

	sprintf_s(strMsg, _countof(strMsg), "SavePromotions input: pump [%ld], lGradeCode [%d], iPositionIndex [%ld], lCenstOff [%f] ,lAskDeferral [%ld] , lPromID [%ld]  ,lRewardAmountDeferredReward [%f] ,fDeferrableRewardAmount [%f] ,lOriginalRewardField [%f]   PLU[%ld]",
		lPumpNumber,
		lGradeCode,
		iPositionIndex,
		cPromotion.GetCentsOff(),
		cPromotion.GetAskDeferral(),
		cPromotion.GetID(),
		cPromotion.GetRewardAmountDeferredReward(),
		cPromotion.GetDeferrableRewardAmount(),
		cPromotion.GetOriginalRewardField(), lPluCode);

	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);

	CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[p].GetStat();


	if (_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsDeferralDiscountInFlow()) //4.0.22.508 74810
	{
		m_Promotion[p].sPromID[iPositionIndex].fValueToDisplay = cPromotion.GetOriginalRewardField(); //4.0.22.504 69344
	}

	m_Promotion[p].sPromID[iPositionIndex].lPluCode = lPluCode; // MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
	m_Promotion[p].sPromID[iPositionIndex].sPromotionDescription = sPromDescription; // MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling

	m_Promotion[p].sPromID[iPositionIndex].cPromotionsList.push_back(cPromotion);

	m_Promotion[p].sPromID[iPositionIndex].lPromID = cPromotion.GetID();//save the prom id
	m_Promotion[p].lNumOfPromotionID++;

	m_Promotion[p].sPromID[iPositionIndex].fTotalCentsOff += cPromotion.GetCentsOff();//save the centOff
	m_Promotion[p].sPromID[iPositionIndex].fDeferrableRewardAmount += cPromotion.GetDeferrableRewardAmount();//save the deferral discount only.
	m_Promotion[p].sPromID[iPositionIndex].fDeferralTotalCentsOff += cPromotion.GetRewardAmountDeferredReward();//save the total discount in case of deferral. 

	sprintf_s(strMsg, _countof(strMsg), "SavePromotions: m_Promotion[%ld].sPromID[%ld]: fCentsOff [%f] , lDeferralDiscount [%f] , fDeferralTotalCentsOff [%f]", p, iPositionIndex, m_Promotion[p].sPromID[iPositionIndex].fTotalCentsOff, m_Promotion[p].sPromID[iPositionIndex].fDeferrableRewardAmount, m_Promotion[p].sPromID[iPositionIndex].fDeferralTotalCentsOff);
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);

	m_Promotion[p].sPromID[iPositionIndex].bUseDeferral = FALSE; //initialize deferral selection.
	m_Promotion[p].sPromID[iPositionIndex].lGradeNumber = lGradeCode;//save the grade number 

	l2a(CENTS_OFF_TYPE, &cTmpInfo.CardSaleExtraData2.sGRDiscountType, sizeof(cTmpInfo.CardSaleExtraData2.sGRDiscountType));

	long lCenstOff = abs(cPromotion.GetCentsOff() * 10);

	_Module.m_server.UpdateDiscountPerGrade(lGradeCode, lPumpNumber, lCenstOff, cTmpInfo);
}


long CFuelLoyaltyPS::CalclateDiscountForForceXml( long lPumpNumber,PAY_AT_PUMP_INFO &cPapInfo, long lPromotionDiscount, long lPromotionID,CPumpTransact &cTrs )
{
	long lTotalDiscount = 0;
	
	if(GetNumberOfPromotions(lPumpNumber-1,cTrs.m_nGrade) == 1)
	{
		if (a2l(cPapInfo.CardSaleExtraData3.sCarWashDiscount, sizeof(cPapInfo.CardSaleExtraData3.sCarWashDiscount)) == 0)	//we don't have carwash cents off //4.0.23.950 216343
		{
			//4.0.20.300
			lTotalDiscount = a2l(cPapInfo.CardSaleExtraData2.sTotalDiscount, sizeof(cPapInfo.CardSaleExtraData2.sTotalDiscount));
		}
		else
		{
			lTotalDiscount = a2l(cPapInfo.CardSaleExtraData5.sGRTotalDiscountFuelItem, sizeof(cPapInfo.CardSaleExtraData5.sGRTotalDiscountFuelItem));
		}
		

		float fTotalDiscount = (float)lTotalDiscount;
		fTotalDiscount /= 10;
		fTotalDiscount += 0.5;
		long lTotalDiscountToXml = (long)fTotalDiscount;
		
		return lTotalDiscountToXml;
	}
	else
	{
		float fTotalDiscount =  (float)(lPromotionDiscount  * cTrs.m_lRoundedVolume);
		fTotalDiscount /= 1000;
		fTotalDiscount += 0.5;
		long lTotalDiscountToXml = (long)fTotalDiscount;
		
		CString sMsg;
		sMsg.Format("GradeId [%d] PromotionId [%ld] , Applied total discount [%d]",cTrs.m_nGrade,lPromotionID, lTotalDiscountToXml);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP, sMsg);
		
		return  lTotalDiscountToXml;
	}
}

void CFuelLoyaltyPS::ForceFuelItem( long lNacsCode,CXMLInterface &xmlInitDoc, CXMLPointers pPumpPromotionElement,CXMLPointers pRootElement, CPumpTransact &trs, long lPumpNumber, PAY_AT_PUMP_INFO &cTmpPapInfo, long lOriginUnitPrice, CXMLPointers pChildNode)
{
	try
	{
		CXmlParser xmlParser;
		CXMLPointers pForcePromotionElement;
		long lPumpIndex = lPumpNumber - 1, lPromID = 0;
		double fCentsOff;
		GetPromotionId(trs.m_nGrade,lPumpNumber-1,&lPromID,&fCentsOff);

		//4.0.20.420 - do not add the car wash price (TD - 67763 failed qa part)
		long lNonDiscountTotalAmount = a2l(cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount,sizeof(cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount));

		long lDiscountTotalAmount = ConvertStringsToLong(cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt,sizeof(cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt),cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt_Msb));
		
		if((lNacsCode != 0) && (lPromID != 0) && (abs(lNonDiscountTotalAmount - lDiscountTotalAmount) >= 10))//it means that we have to do force
		{	
			CXMLInterface xmlChildDoc;
			char	strForceXML[MAX_BUF] = {0};
			
			xmlInitDoc.SetAttribute(XMLSTR_GetImmediateDiscounts,"0",&pPumpPromotionElement);	//4.0.22.504 69344
			xmlInitDoc.CreateElement(XMLSTR_FORCE_PROP,&pForcePromotionElement);
			xmlInitDoc.AppendChild(&pForcePromotionElement,&pRootElement);
			xmlInitDoc.CreateElement(XMLSTR_PromoSrvMessage,&pRootElement);
			xmlInitDoc.AppendChild(&pRootElement,&pForcePromotionElement);
			
			PromotionList promotionList = GetPromotionsList(lPumpIndex,trs.m_nGrade);
			for (PromotionIterator itPromotion = promotionList.begin(); itPromotion != promotionList.end(); ++itPromotion)
			{	
				if(FALSE == IsDeferralDiscountSelected(lPumpNumber) && itPromotion->IsPromotionIncludeDeferralDiscount())
				{
					continue;
				}
				
				CreateForceXmlFuelItem(lPumpNumber, cTmpPapInfo, trs, lOriginUnitPrice, lNacsCode, itPromotion,strForceXML);
				
				xmlChildDoc.InitDoc();
				xmlChildDoc.LoadXmlString(strForceXML);
				
				xmlChildDoc.SelectSingleElementNode(XMLSTR_ResponseInfo, &pChildNode);
				xmlInitDoc.AppendChild(&pChildNode, &pRootElement);
				xmlChildDoc.CloseDoc();
			}
			
			if (a2l(cTmpPapInfo.CardSaleExtraData3.sCarWashDiscount, sizeof(cTmpPapInfo.CardSaleExtraData3.sCarWashDiscount)))
			{
				CreateForceXmlCarWashItem(lPumpNumber, cTmpPapInfo, lNacsCode, strForceXML);
				memset(strForceXML, 0, sizeof(strForceXML));

				xmlChildDoc.InitDoc();
				xmlChildDoc.LoadXmlString(strForceXML);
				xmlChildDoc.SelectSingleElementNode(XMLSTR_ResponseInfo, &pChildNode);
				xmlInitDoc.AppendChild(&pChildNode, &pRootElement);
				xmlChildDoc.CloseDoc();
			}
			
			xmlChildDoc.CloseDoc();
		}
		else if ((fCentsOff) && (IsDeferralDiscountSelected(lPumpNumber)))	//4.0.22.504 69344
		{
			xmlInitDoc.SetAttribute(XMLSTR_GetImmediateDiscounts,"1",&pPumpPromotionElement);

			xmlParser.BuildApprovalCommunicationMessage(xmlInitDoc,lPromID);

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString sMsg;
				sMsg.Format("Response approval message is required for cents off %f", fCentsOff); 
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
			}
		}
		else
		{
			xmlInitDoc.SetAttribute(XMLSTR_GetImmediateDiscounts,"0",&pPumpPromotionElement);
		}
	}
	catch (...)
	{	
		DWORD dwLastError = GetLastError();
		
		CString strLogMsg;
		strLogMsg.Format("CFuelLoyaltyPS::ForceFuelItem Expection! Error=%ld", dwLastError);
		
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strLogMsg,LOG_LEVEL_1);			
	}
}


void CFuelLoyaltyPS::PrintForceDiscounts(long lPumpNumber ,PAY_AT_PUMP_INFO &cPapInfo)
{
	long lTotalAmountWithoutDiscount = a2l(cPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount ,sizeof(cPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount));
	
	long lTotalAmountAfterDiscount = ConvertStringsToLong(cPapInfo.CardSaleInfo.extraData.sTotalAmt,sizeof(cPapInfo.CardSaleInfo.extraData.sTotalAmt),cPapInfo.CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(cPapInfo.CardSaleInfo.extraData.sTotalAmt_Msb));
	
	long lTotalDiscount = 0;
	if (a2l(cPapInfo.CardSaleExtraData3.sCarWashDiscount, sizeof(cPapInfo.CardSaleExtraData3.sCarWashDiscount)) == 0)	//we don't have carwash cents off //4.0.23.950 216343
	{
		//4.0.20.300
		lTotalDiscount = a2l(cPapInfo.CardSaleExtraData2.sTotalDiscount, sizeof(cPapInfo.CardSaleExtraData2.sTotalDiscount));
	}
	else
	{
		lTotalDiscount = a2l(cPapInfo.CardSaleExtraData5.sGRTotalDiscountFuelItem, sizeof(cPapInfo.CardSaleExtraData5.sGRTotalDiscountFuelItem));
	}
	
	CString sMsg;
	sMsg.Format("TotalDiscount = [%ld], lTotalAmountWithoutDiscount = [%ld], lTotalAmountAfterDiscount = [%ld]",lTotalDiscount, lTotalAmountWithoutDiscount, lTotalAmountAfterDiscount);
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP, sMsg);
}

PromotionList CFuelLoyaltyPS::GetPromotionsList(long lPumpIndex,long lGradeCode)
{
	for(int i = 0; i < m_Promotion[lPumpIndex].lNumOfPromotionID; i++)
	{
		if(m_Promotion[lPumpIndex].sPromID[i].lGradeNumber == lGradeCode)
		{
			return m_Promotion[lPumpIndex].sPromID[i].cPromotionsList;
		}
	}
	
	return (PromotionList)
		NULL;
}

long CFuelLoyaltyPS::CreateForceXmlFuelItem(long lPumpNumber, PAY_AT_PUMP_INFO &cPapInfo, CPumpTransact & cTrs, long lOriginUnitPrice, long lPluCode, PromotionIterator & itPromotion, char * szOutXml)
{
	CXMLInterface xmlInitDoc;
	CXMLPointers  pCurrElement,pChildNode2,pCurrElement3,pChildNode,pCurrElement4, pPromotionsNode, pChildNode3, pPromDataNode, pGradeIDNode, pGradeIDNode2;	//4.0.22.504 69344 //4.0.23.170 102319
	long lPromID = itPromotion->GetID();
	long lPromotionDiscount = itPromotion->GetTotalDiscount();
	CString sMsg;
	CComBSTR	  bstrXML;
	
	try
	{
		xmlInitDoc.InitDoc();
		xmlInitDoc.CreateElement(XMLSTR_PromoSrvMessage,&pCurrElement);
		xmlInitDoc.AppendRootElement(&pCurrElement);
		
		xmlInitDoc.CreateElement(XMLSTR_ResponseInfo,&pChildNode);
		xmlInitDoc.AppendChild(&pChildNode,&pCurrElement);
		
		// 4.0.23.170 start 102319
		if (_Module.m_server.GetDynamicAttrForGradeID())
		{
			xmlInitDoc.CreateElement(XMLSTR_Attributes,&pGradeIDNode);
			xmlInitDoc.AppendChild(&pGradeIDNode,&pChildNode);
			xmlInitDoc.CreateElement(XMLSTR_Attr,&pGradeIDNode2);
			xmlInitDoc.AppendChild(&pGradeIDNode2,&pGradeIDNode);
			xmlInitDoc.SetIntAttribute(XMLSTR_ID,_Module.m_server.GetDynamicAttrForGradeID(), &pGradeIDNode2);
			xmlInitDoc.SetAttribute(XMLSTR_Value,cTrs.m_nGrade, &pGradeIDNode2);
			
		}
		// 4.0.23.170 end 102319
		
		xmlInitDoc.SetAttribute(XMLSTR_PromNumber,lPromID,&pChildNode);
		xmlInitDoc.SetAttribute(XMLSTR_ApprovalType,"4",&pChildNode);
		xmlInitDoc.CreateElement(XMLSTR_ApprovedItems,&pChildNode2);
		xmlInitDoc.AppendChild(&pChildNode2,&pChildNode);
		xmlInitDoc.CreateElement(XMLSTR_ApprovedItem,&pChildNode);
		xmlInitDoc.AppendChild(&pChildNode,&pChildNode2);
		xmlInitDoc.SetAttribute(XMLSTR_PluCode,lPluCode,&pChildNode);
		xmlInitDoc.SetAttribute(XMLSTR_DepCode,_Module.m_server.m_cXmlParser.GetDepCode(),&pChildNode);	//Liat Copient was 90 HyVee = 4
		xmlInitDoc.SetAttribute(XMLSTR_Quantity,cTrs.m_lRoundedVolume,&pChildNode);
		xmlInitDoc.SetAttribute(XMLSTR_QuantityType,"2",&pChildNode);
		
		
		long lTotalDiscountToXml = CalclateDiscountForForceXml(lPumpNumber,cPapInfo, lPromotionDiscount, lPromID,cTrs);
		
		xmlInitDoc.SetDoubleAttributeWithConvert(XMLSTR_RewardAmount,lTotalDiscountToXml,&pChildNode);
		
		xmlInitDoc.SetDoubleAttributeWithConvert(XMLSTR_Price,lOriginUnitPrice,&pChildNode);	//4.0.20.56 64904
		
		long lPriceAfterDiscountToXml = CalclateUnitPriceAfterDiscountForForceXml(cTrs);
		
		xmlInitDoc.SetDoubleAttributeWithConvert(XMLSTR_UnitPriceAfterDiscount,lPriceAfterDiscountToXml,&pChildNode);
		
		xmlInitDoc.SetIntAttribute(XMLSTR_GetImmediateDiscounts,m_dwImmediateDiscounts,&pChildNode); //Hyvee 4.0.20.54 64636	
		
		
		xmlInitDoc.GetXmlString(&bstrXML);
		
		int iLen = Bstr2Buffer(bstrXML , szOutXml , MAX_BUF);
		if(iLen == 0)
			_LOGMSG.LogMsg("CFuelLoyaltyPS::CreateForceXmlFuelItem - Bstr2Buffer fail!" , LOG_LEVEL_1);
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS::CreateForceXmlFuelItem, Unexpected error has occurred", LOG_LEVEL_1); //  4.0.23.980
	}
	
	xmlInitDoc.CloseDoc();
	
	return TRUE;
}

long CFuelLoyaltyPS::CreateForceXmlCarWashItem(long lPumpNumber, PAY_AT_PUMP_INFO &cPapInfo, long lPluCode, char * szOutXml)
{
	CXMLInterface xmlInitDoc;
	CXMLPointers  pCurrElement,pChildNode2,pCurrElement3,pChildNode,pCurrElement4, pPromotionsNode, pChildNode3, pPromDataNode, pGradeIDNode, pGradeIDNode2;	//4.0.22.504 69344 //4.0.23.170 102319

	CString sMsg;
	CComBSTR	  bstrXML;
	long lAmout = 1, lPrice = 0, lQuantity = 1, lNacsCode = 0;
	CString strMsg;
	BOOL bRetCode = TRUE;
	
	try
	{
		long lProdCode = a2l(cPapInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode, sizeof(cPapInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode));
		
		CString str ("WashPrograms\\WashProgram");
		char sTmpStr[MAX_FIELD_VAL];
		ZeroMemory((char *)sTmpStr, sizeof(sTmpStr));
		char sNumber[8];
		memset(sNumber,0,sizeof(sNumber));
		sprintf_s(sNumber, _countof(sNumber),"%02d",lProdCode);
		str += sNumber;
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CCH_Product_Code",sTmpStr,sizeof(sTmpStr),sNumber,TRUE);
		lNacsCode = a2l((BYTE *)&sTmpStr, sizeof(sTmpStr));
		
		//	lNacsCode = a2l(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode, sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode));
	
		lPrice = a2l((BYTE *)&cPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice,sizeof(cPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice));
		lQuantity = a2l((BYTE *)&cPapInfo.CardSaleInfo.extraData.SALES[0].sQty,sizeof(cPapInfo.CardSaleInfo.extraData.SALES[0].sQty));
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
			xmlInitDoc.CreateElement(XMLSTR_PromoSrvMessage,&pCurrElement);
			xmlInitDoc.AppendRootElement(&pCurrElement);
			
			xmlInitDoc.CreateElement(XMLSTR_ResponseInfo,&pChildNode);
			xmlInitDoc.AppendChild(&pChildNode,&pCurrElement);
			long lPromID = a2l(cPapInfo.CardSaleExtraData3.sPromotionNumber2, sizeof(cPapInfo.CardSaleExtraData3.sPromotionNumber2));
			xmlInitDoc.SetAttribute(XMLSTR_PromNumber,lPromID,&pChildNode);
			xmlInitDoc.SetAttribute(XMLSTR_ApprovalType,"4",&pChildNode);
			xmlInitDoc.CreateElement(XMLSTR_ApprovedItems,&pChildNode2);
			xmlInitDoc.AppendChild(&pChildNode2,&pChildNode);
			xmlInitDoc.CreateElement(XMLSTR_ApprovedItem,&pChildNode);
			xmlInitDoc.AppendChild(&pChildNode,&pChildNode2);
			xmlInitDoc.SetAttribute(XMLSTR_PluCode,lPluCode,&pChildNode);	//Should be the plu of the fuel item, not the cw
			xmlInitDoc.SetAttribute(XMLSTR_DepCode,_Module.m_server.m_cXmlParser.GetDepCode(),&pChildNode);	//Liat Copient was 90 HyVee = 4
			xmlInitDoc.SetAttribute(XMLSTR_Quantity, lQuantity, &pChildNode);
			xmlInitDoc.SetIntAttribute(XMLSTR_QuantityType,QUANTITY_TYPE_COUNTED,&pChildNode);
			xmlInitDoc.SetAttribute(XMLSTR_Amount, lAmout, &pChildNode); 
			xmlInitDoc.SetAttribute(XMLSTR_Price, lPrice, &pChildNode);
		}
		
		
		long lTotalDiscount = 0;
		lTotalDiscount = a2l(cPapInfo.CardSaleExtraData5.sGRTotalDiscountCarWash, sizeof(cPapInfo.CardSaleExtraData5.sGRTotalDiscountCarWash));
		float fTotalDiscount = (float)lTotalDiscount;
		fTotalDiscount /= 10;
		fTotalDiscount += 0.5;
		long lTotalDiscountToXml = (long)fTotalDiscount;
		
		xmlInitDoc.SetDoubleAttributeWithConvert(XMLSTR_RewardAmount,lTotalDiscountToXml,&pChildNode);
		long lNonDiscountTotalAmount = a2l(cPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount
			,sizeof(cPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount)) ;
		long lDiscountTotalAmount = ConvertStringsToLong(cPapInfo.CardSaleInfo.extraData.sTotalAmt,sizeof(cPapInfo.CardSaleInfo.extraData.sTotalAmt),cPapInfo.CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(cPapInfo.CardSaleInfo.extraData.sTotalAmt_Msb));
		
		
		sMsg.Format("CFuelLoyaltyPS::CreateForceXmlCarWashItem %ld (TotalDiscount) = [%ld (NonDiscountTotalAmount) - %ld (DiscountTotalAmount)]",lTotalDiscount, lNonDiscountTotalAmount, lDiscountTotalAmount);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP, sMsg);
		
		xmlInitDoc.SetIntAttribute(XMLSTR_GetImmediateDiscounts,m_dwImmediateDiscounts,&pChildNode);
		
		xmlInitDoc.GetXmlString(&bstrXML);
		
		int iLen = Bstr2Buffer(bstrXML,szOutXml,MAX_BUF); //  4.0.23.980
		if(iLen == 0)
			_LOGMSG.LogMsg("CFuelLoyaltyPS::CreateForceXmlCarWashItem - Bstr2Buffer fail!" , LOG_LEVEL_1);
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS::CreateForceXmlCarWashItem, Unexpected error has occurred", LOG_LEVEL_1); //  4.0.23.980
		bRetCode = FALSE;
	}
	
	xmlInitDoc.CloseDoc();
	
	return bRetCode;
}

// 4.0.23.2340  CR 305114
long CFuelLoyaltyPS::CalclateUnitPriceAfterDiscountForForceXml( CPumpTransact &cTrs )
{
	long lUnitPriceAfterDiscount = cTrs.m_lGradePrice;
	float fUnitPriceAfterDiscount = (float)lUnitPriceAfterDiscount;
	
	fUnitPriceAfterDiscount /= 10;
	fUnitPriceAfterDiscount += 0.5;
	
	return (long)fUnitPriceAfterDiscount;
}






// 4.0.23.2350  TD 403075
BOOL CFuelLoyaltyPS::IsManualDeferral(long lPumpIndex)
{
	for(int i = 0; i < MAX_POSITIONS_PER_PUMP; i++)
	{
		for (PromotionIterator itPromotion = m_Promotion[lPumpIndex].sPromID[i].cPromotionsList.begin(); 
		itPromotion != m_Promotion[lPumpIndex].sPromID[i].cPromotionsList.end(); ++itPromotion)
		{	
			if(itPromotion->IsManualDeferral())
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}


BOOL CFuelLoyaltyPS::HandleEndTicketResult(long lPumpNumber) //367311	
{
	long p = lPumpNumber - 1;
	OLA_STAT ola;
	
	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);
	memset(&ola,0,sizeof(OLA_STAT));
	
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetOlaStat(&ola);
	if (OLA_STATE(ola.m_byState) == OLA_LOYALTY_WAIT_END_TICKET_RESPONSE)
	{
		ola.m_byState = SESSION_COMPLETE | OLA_REQUEST_TO_SEND;
		_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
	}
	else // TD 463378
	{
		CString str;
		str.Format("HandleEndTicketResult - Unexpected ola state!! Ola state = %s", _LOGMSG.GetStateString(ola.m_byState));
		_LOGMSG.LogMsg("GCI", "PAPReadyTransaction", lPumpNumber, LOG_PUMP, str, LOG_LEVEL_1);

		if (ola.m_byState == (SESSION_COMPLETE | OLA_LOYALTY_WAIT_RESPONSE)) // was in the add item
		{
			ola.m_byState = SESSION_COMPLETE | OLA_REQUEST_TO_SEND;
			_Module.m_server.m_cPumpArray[p].SetOlaStat(&ola);
		}
	}
	return TRUE;
}


/******************************************************************************
 Description: Sets Pump Number recived from loyalty fuel

 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 OmerT		11/06/2014	 19:34		// 4.0.23.2390  TD 406362
******************************************************************************/
BOOL CFuelLoyaltyPS::SetPumpNumberReceivedFromLPE( long &lPumpNumber,  long & p ,CXMLInterface & xmlInitDoc, CXMLPointers & pNode)
{
	if(lPumpNumber > 0 && lPumpNumber < MAX_PUMPS_64)
	{
		p = lPumpNumber - 1;
		return TRUE;
	}

	lPumpNumber = xmlInitDoc.GetAttribute(XMLSTR_PumpNumber,&pNode);

	if(lPumpNumber > 0 && lPumpNumber < MAX_PUMPS_64)
	{
		p = lPumpNumber - 1;
		return TRUE;
	}
	

	_LOGMSG.LogMsg("The pump number that return from Loyalty server is invalid",LOG_LEVEL_0);
	return FALSE;

}

/******************************************************************************
Description: Parse Received target message, set the text into sOutText

Returns:

Parameters:

WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Omer			22/04/2012	 19:34		Start 312183
******************************************************************************/
BOOL CFuelLoyaltyPS::ParseTargetMessage(char * sTargetMessageXml, char * sOutText)
{
	BYTE    sText[MAX_TERMINAL_MESSAGE_LENGTH] = { 0 };
	BOOL	bRetCode = TRUE;

	CXMLInterface xmlInitDoc;

	try
	{
		xmlInitDoc.InitDoc();
		xmlInitDoc.LoadXmlString(sTargetMessageXml);

		if (LOG_FCC_FLOW_CONTROL)
			xmlInitDoc.SaveToFile("C:\\lpe\\xml\\TargetMessage.xml"); // TD 319664

		CXMLPointers pPromoSrvMsgNode, pCurrElement, pNode, pChildNode, pListNodes, pCurrPrintElement, pCurrSectionElement, pCurrDocumentElement, pListPrintNodes, pListSectionNodes, pListDocumentNodes;

		bRetCode = xmlInitDoc.GetRootNode(&pPromoSrvMsgNode);

		xmlInitDoc.SelectSingleElementNode(XMLSTR_PrintMessages, &pPromoSrvMsgNode);

		xmlInitDoc.GetChildNodesList(&pPromoSrvMsgNode, &pListNodes);

		if (pListNodes.m_pMSListOfItems)
		{
			while (xmlInitDoc.GetNextItem(pListNodes, pCurrElement))	//PrintMessages elements
			{
				xmlInitDoc.GetChildNodesList(&pCurrElement, &pListPrintNodes);

				if (pListPrintNodes.m_pMSListOfItems)
				{
					while (xmlInitDoc.GetNextItem(pListPrintNodes, pCurrPrintElement)) //PrintMessage elements
					{
						xmlInitDoc.GetChildNodesList(&pCurrPrintElement, &pListDocumentNodes);
						if (pListDocumentNodes.m_pMSListOfItems)
						{
							while (xmlInitDoc.GetNextItem(pListDocumentNodes, pCurrDocumentElement)) //Document elements
							{
								xmlInitDoc.GetChildNodesList(&pCurrDocumentElement, &pListSectionNodes);
								if (pListSectionNodes.m_pMSListOfItems)
								{
									while (xmlInitDoc.GetNextItem(pListSectionNodes, pCurrSectionElement)) //Section elements
									{
										bRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_Object, &pNode, &pCurrSectionElement);
										if (bRetCode)
										{
											xmlInitDoc.GetChildNodesList(&pNode, &pListNodes);
											if (pListNodes.m_pMSListOfItems)
											{
												while (xmlInitDoc.GetNextItem(pListNodes, pCurrElement))	//TextAttributes elements
												{
													xmlInitDoc.SelectSingleElementNode(XMLSTR_Text, &pChildNode, &pCurrElement);
													xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_Text, sText, &pChildNode);
													strcat(sOutText, (const char *)sText);
													strcat(sOutText, (const char *)"\n");
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
					bRetCode = FALSE;
			}
		}
		else
			bRetCode = FALSE;

		CString str;
		str.Format("CFuelLoyaltyPS::ParseTargetMessage Parsed Target Message %s", bRetCode ? "successfully" : "failed!", LOG_LEVEL_0);
		_Module.m_server.m_cLogger.LogMsg(str);
	}
	catch (...)
	{
		_Module.m_server.m_cLogger.LogMsg("CFuelLoyaltyPS::ParseTargetMessage, Unexpected error has occurred", LOG_LEVEL_0);
		bRetCode = FALSE;
	}

	return bRetCode;
}

void CFuelLoyaltyPS::ParseSppsPromotions( CXMLInterface &xmlInitDoc, CXMLPointers pChildNode, CXMLPointers &pListNodes,char *&sAllText )
{
	/**********************************************************/
	//For IFSF EPS - build the zone to include the promotion text for receipt.
	/**********************************************************/
	CXMLPointers pCurrElement;
	char sText[MAX_TERMINAL_MESSAGE_LENGTH] = {0}; //4.0.23.504 66705
	BYTE sCurrText[MAX_DESC_LEN] = {0}; //4.0.19.710 66705 (100) //4.0.23.504 66705
	xmlInitDoc.GetChildNodesList(&pChildNode, &pListNodes);
	if (pListNodes.m_pMSListOfItems)
	{
		while (xmlInitDoc.GetNextItem(pListNodes,pCurrElement))	//TextLine elements
		{
			memset(sCurrText, 0, sizeof(sCurrText));
			xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_Text,sCurrText,&pCurrElement);
			strncat_s(sText, _countof(sText), (char *)sCurrText,MAX_DESC_LEN);
			strncat_s(sText, _countof(sText), "|",sizeof("|"));
		}
		strncpy(sAllText,sText,MAX_TERMINAL_MESSAGE_LENGTH);
	}
}

BOOL CFuelLoyaltyPS::AddCarWashItem(long lPumpNumber, long lCarWashSaleStatus)
{
	BOOL		bRetCode = TRUE;
	long		p = lPumpNumber - 1;
	PAY_AT_PUMP_INFO  cTmpInfo;
	CComBSTR	bstrCarWashItemXML;

	if(FALSE == _Module.m_server.m_cFuelLoayaltyPS.GetInitDllFlag())//we have to init
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","AddCarWashItem",lPumpNumber,LOG_PUMP,"AddCarWashItem called after init failed.",LOG_LEVEL_1);
		bRetCode = FALSE;
	}		

	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	_Module.m_server.m_cPumpArray[p].GetPAPInfo(&cTmpInfo);	

	try
	{
		if(lCarWashSaleStatus == eGetPromotion)
		{
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Adding car wash item and getting promotion");
			bRetCode = CreateAddCarWashItemXML(cTmpInfo, bstrCarWashItemXML, lPumpNumber);
			if (bRetCode)
			{
				_Module.m_server.m_cFuelLoayaltyPS.SetTimer(p, ADD_CARWASH_ITEM_CHECK_TIMEOUT);
			}
		}
		else if(lCarWashSaleStatus == eCustomerAccpectedPromotion)
		{
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Car wash promotion accepted, sending response to LPE");
			bRetCode = CreatePromotionAccpectedXML(cTmpInfo, bstrCarWashItemXML, lPumpNumber);
		}

		if(bRetCode)
			HRESULT hr = m_cLoyaltyCommandExecuter->AddCarWashItem(lPumpNumber,lCarWashSaleStatus,bstrCarWashItemXML.m_str);
		
	}
	catch(_com_error & e )
	{
		bRetCode = FALSE;
		_LOGMSG.LogResult(0, "CFuelLoyaltyPS","AddItem",FALSE,0,TRUE,e.Error(),e);
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","AddCarWashItem",lPumpNumber,LOG_PUMP, "Unexpected Error Had occurred");
		bRetCode = FALSE;
	}


	

	return bRetCode;
}



/******************************************************************************
 Description: Parse Received Target Message and set pap swipe result command to CL

 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Omer			22/04/2012	 19:34		Start 312183
******************************************************************************/
void CFuelLoyaltyPS::TargetMessageResult(long lPumpNumber , char * sInTargetMsg)
{
	OLA_STAT ola;
	memset(&ola,0,sizeof(OLA_STAT));
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetOlaStat(&ola);

	PAY_AT_PUMP_INFO cInfo;
	memset(&cInfo,' ' ,sizeof(PAY_AT_PUMP_INFO));
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cInfo);

	// build the message to display
	char sOutTargetMsg[MAX_TERMINAL_MESSAGE_LENGTH] = {0};
	if(_Module.m_server.m_cXmlParser.ParseTargetMessage(sInTargetMsg,sOutTargetMsg,sizeof(sOutTargetMsg)))
	{
		// instead of opening a need more param session send the message in pump authorize
		cInfo.CardSaleExtraData2.sDisplayOLAResultMessage = '5';
		memset(cInfo.CardSaleExtraData2.sCustomerMessage, ' ' , sizeof(cInfo.CardSaleExtraData2.sCustomerMessage));
 		memcpy((char *)cInfo.CardSaleExtraData2.sCustomerMessage, (char *)sOutTargetMsg, min(sizeof(cInfo.CardSaleExtraData2.sCustomerMessage), strlen(sOutTargetMsg)));
	}
	else
	{	
		cInfo.CardSaleExtraData2.sDisplayOLAResultMessage = ' ';
		memset(cInfo.CardSaleExtraData2.sCustomerMessage, ' ', sizeof(cInfo.CardSaleExtraData2.sCustomerMessage));
	}

	ola.m_byState =	OLA_LOYALTY_TARGET_MESSAGE;
	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola); 

	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cInfo);
	
	_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cInfo, TRUE);

}

/******************************************************************************
Description: Get Target Message from RTAPS and move OLA state machine

Returns:

Parameters:

WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Omer			22/04/2012	 19:34		Start 312183
******************************************************************************/
BOOL CFuelLoyaltyPS::GetTargetMessage(long lPumpNumber)
{
	m_cLoyaltyCommandExecuter->GetTargetMessage(lPumpNumber);

	return TRUE;
}

//************************************
// Method:    GetCarWashNacsCode
// FullName:  CFuelLoyaltyPS::GetCarWashNacsCode
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: long lProdCode
// Parameter: long & lNacsCode
//************************************
void CFuelLoyaltyPS::GetCarWashNacsCode( long lProdCode, long &lNacsCode )
{
	///4.0.20.420 TD - 68979 - we need to take the CCH_Product_Code from the program
	CString str ("WashPrograms\\WashProgram");
	char sTmpStr[MAX_FIELD_VAL];
	ZeroMemory((char *)sTmpStr, sizeof(sTmpStr));
	char sNumber[8];
	memset(sNumber,0,sizeof(sNumber));
	sprintf_s(sNumber, _countof(sNumber),"%02d",lProdCode);
	str += sNumber;
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CCH_Product_Code",sTmpStr,sizeof(sTmpStr),sNumber,TRUE);
	lNacsCode = a2l((BYTE *)&sTmpStr, sizeof(sTmpStr));
}


//************************************
// Method:    AddItemResult
// FullName:  CFuelLoyaltyPS::AddItemResult
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: long lPumpNumber
// Parameter: char * sItemPromotionXML
//************************************
BOOL CFuelLoyaltyPS::AddItemResult(long lPumpNumber,char * sItemPromotionXML)
{
	long lPumpIndex = lPumpNumber-1;
	if(GetTimerCode(lPumpIndex) == ADD_CARWASH_ITEM_CHECK_TIMEOUT)
		ClearTimer(lPumpIndex);

	OLA_STAT ola;
	memset(&ola,0,sizeof(OLA_STAT));
	_Module.m_server.m_cPumpArray[lPumpIndex].GetOlaStat(&ola);

	PAY_AT_PUMP_INFO cInfo;
	memset(&cInfo,' ' ,sizeof(PAY_AT_PUMP_INFO));
	_Module.m_server.m_cPumpArray[lPumpIndex].GetPAPInfo(&cInfo);

	char sOutIncetiveMsg[MAX_TERMINAL_MESSAGE_LENGTH] = {0};
	if(sItemPromotionXML != NULL)
	{
		_Module.m_server.m_cLogger.LogMsg("CFuelLoyaltyPS::AddItemResult parsing carwash promotion");

		m_Promotion[lPumpIndex].lRemainingValueToTrigger = 0;
		BOOL bRetCode = _Module.m_server.m_cXmlParser.ParseRemainingValueToTrigger(sItemPromotionXML, m_Promotion[lPumpIndex].lRemainingValueToTrigger); // TD 466954

		if(_Module.m_server.m_cXmlParser.ParseTargetMessage(sItemPromotionXML,sOutIncetiveMsg,sizeof(sOutIncetiveMsg)))
		{
			ola.m_byState =	OLA_LOYALTY_CARWASH_PROMOTION;

			_Module.m_server.m_cPumpArray[lPumpIndex].SetOlaStat(&ola);

			_Module.m_server.m_cPumpArray[lPumpIndex].SetPAPInfo(&cInfo);

			CNMPDetails cNmpObject;

			cNmpObject.SetScreenNumber(SCREEN_CARWASH_PROMOTION);


			cNmpObject.OpenNeedMoreParam(cInfo ,lPumpNumber,sOutIncetiveMsg,1,NMP_DEFAULT_TIMEOUT,ONMP_OpenYesNoKeys);
		}
		else
		{
			_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cInfo);
		}
	}
	else
	{
		_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cInfo);
		
	}

	return TRUE;
}

//************************************
// Method:    CreatePromotionAccpectedXML
// FullName:  CFuelLoyaltyPS::CreatePromotionAccpectedXML
// Access:    private 
// Returns:   BOOL
// Qualifier:
// Parameter: PAY_AT_PUMP_INFO & cTmpPapInfo
// Parameter: CComBSTR & bstrAddItemXml
// Parameter: long lPumpNumber
//************************************
BOOL CFuelLoyaltyPS::CreatePromotionAccpectedXML( PAY_AT_PUMP_INFO & cTmpPapInfo, CComBSTR& bstrAddItemXml, long lPumpNumber )
{
	CXmlParser xmlParser;
	CXMLInterface xmlInitDoc;
	xmlInitDoc.InitDoc();

	LONG lPromID = CARWASH_PLU;
	xmlParser.BuildApprovalCommunicationMessage(xmlInitDoc,lPromID);

	xmlInitDoc.GetXmlString(&bstrAddItemXml);


	return TRUE;
}

//************************************
// Method:    PromotionResult
// FullName:  CFuelLoyaltyPS::PromotionResult
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: long lPumpNumber
// Parameter: long lEarnValue
// Parameter: BSTR bstrItemPromotion
//************************************
BOOL CFuelLoyaltyPS::PromotionResult( long lPumpNumber,long lEarnValue, BSTR bstrItemPromotion)
{
	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&info);

	ClearTimer(lPumpNumber);

	OLA_STAT ola;
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetOlaStat(&ola);

	long lCarWashQuantity = a2l(info.CardSaleInfo.extraData.SALES[0].sQty,sizeof(info.CardSaleInfo.extraData.SALES[0].sQty));
	memset(info.CardSaleInfo.extraData.SALES[0].sQty,' ',sizeof(info.CardSaleInfo.extraData.SALES[0].sQty));
	l2a(lCarWashQuantity+lEarnValue,info.CardSaleInfo.extraData.SALES[0].sQty,sizeof(info.CardSaleInfo.extraData.SALES[0].sQty));

	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&info);

	_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &info);

	return TRUE;
}

//************************************
// Method:    GetCarWashDetalisFromCardSaleData
// FullName:  CFuelLoyaltyPS::GetCarWashDetalisFromCardSaleData
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: long lPumpNumber
// Parameter: long & lProdCode
// Parameter: PAY_AT_PUMP_INFO & cTmpPapInfo
// Parameter: long & lPrice
// Parameter: long & lQuantity
// Parameter: long & lNacsCode
//************************************
void CFuelLoyaltyPS::GetCarWashDetalisFromCardSaleData( long lPumpNumber , __int64 &i64PluCode, PAY_AT_PUMP_INFO &cTmpPapInfo, long &lUnitPrice, long &lQuantity)
{
	//lProduceCode = a2l(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode, sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode));
	lUnitPrice = a2l((BYTE *)&cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice,sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice));
	lQuantity = a2l((BYTE *)&cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sQty,sizeof(cTmpPapInfo.CardSaleInfo.extraData.SALES[0].sQty));

	if(_Module.m_server.GetCarWashDeviceType().Compare("CCM") == 0) // 4.0.32.460
		i64PluCode = a2i64(cTmpPapInfo.CardSaleExtraData6.CarWashItem.sUpc, sizeof(cTmpPapInfo.CardSaleExtraData6.CarWashItem.sUpc));
	else
		i64PluCode = _Module.m_server.GetCarWashPluCode();
		//GetCarWashNacsCode(lProduceCode, lPluCode);

	CString strMsg;
	strMsg.Format("GetCarWashDetalisFromCardSaleData Pump = %ld, PluCode = %llu, Price = %ld, Quantity = %ld, barcode = %.11s", lPumpNumber, i64PluCode, lUnitPrice, lQuantity,cTmpPapInfo.CardSaleExtraData6.CarWashItem.sBarcode);
	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
}

// 4.0.32.160 TD 404754
//************************************
// Method:    SaveLoyaltyPrinitingsToReceipt
// FullName:  CFuelLoyaltyPS::SaveLoyaltyPrinitingsToReceipt
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: char * & sReceiptText
// Parameter: long lReceiptExtraRecIndex
// Parameter: PAY_AT_PUMP_INFO & cTmpInfo
//************************************
void CFuelLoyaltyPS::SaveLoyaltyPrinitingsToReceipt(char *&sReceiptText, long lReceiptExtraRecIndex, PAY_AT_PUMP_INFO &cTmpInfo)
{
	if (sReceiptText)
	{
		long lRtc = 0;
		int iTextSize = strlen(sReceiptText);
		PS::RECEIPT_EXTRA_INFO_REC	rec;
		memset(&rec, ' ', sizeof(PS::RECEIPT_EXTRA_INFO_REC));
		rec.info.lIndexNumber = lReceiptExtraRecIndex; //4.0.19.800 //4.0.23.504 66705
		memset(rec.info.sFiler2043, ' ', sizeof(rec.info.sFiler2043));
		memcpy(rec.info.sFiler2043, sReceiptText, min(sizeof(rec.info.sFiler2043), iTextSize));
		lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_INSERT, rec);

		if (lRtc == 0)
			_LOGMSG.LogMsg("CFuelLoyaltyPS::ParseTriggerPromFromRtaps zones were copied to RECEIPT_EXTRA_REC and saved to RECEIPT_EXTRA_DATA_FILE qdx");
		else
		{
			_LOGMSG.LogMsg("CFuelLoyaltyPS::ParseTriggerPromFromRtaps zones already exists, going to update with new ones");
			PS::RECEIPT_EXTRA_INFO_REC	oldRec;
			memset(&oldRec, ' ', sizeof(PS::RECEIPT_EXTRA_INFO_REC));
			oldRec.info.lIndexNumber = lReceiptExtraRecIndex; //4.0.19.800 //4.0.23.504 66705
			lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, oldRec);
			if (lRtc == 0)
			{
				CString sTmp((CHAR*)&oldRec.info.sFiler2043);
				int iLenOfOldRecored = sTmp.ReverseFind('>') + 1;
				memcpy(rec.info.sFiler2043 + iTextSize + 1, oldRec.info.sFiler2043, min(sizeof(rec.info.sFiler2043) - (iTextSize + 1), iLenOfOldRecored));
				lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_UPDATE, rec);

				_LOGMSG.LogMsg("CFuelLoyaltyPS::ParseTriggerPromFromRtaps zones updated succsfully.");

			}
		}

		if (!(_Module.m_server.GetIFSFTerminalSrvInUse()))	//4.0.19.800 //4.0.23.504 66705
		{
			long lTmp = a2l(cTmpInfo.CardSaleExtraData2.sFlags, sizeof(cTmpInfo.CardSaleExtraData2.sFlags));

			lTmp |= FL_EXTRA_RECEIPT_REC;
			l2a(lTmp, cTmpInfo.CardSaleExtraData2.sFlags, sizeof(cTmpInfo.CardSaleExtraData2.sFlags));
		}
		if (sReceiptText)
		{
			delete[] sReceiptText;
			sReceiptText = NULL;
		}
	}
}

void CFuelLoyaltyPS::SetLoyatyTrack2( PAY_AT_PUMP_INFO &pInfo, CXMLInterface &xmlInitDoc, CXMLPointers &pCurrElement )
{
	CXMLPointers pTrack2Loyalty;
	pTrack2Loyalty.m_pMSElement= NULL;
	pTrack2Loyalty.m_pMSNode = NULL;

	xmlInitDoc.CreateElement(XMLSTR_Track2Loyalty,&pTrack2Loyalty);
	xmlInitDoc.AppendChild(&pTrack2Loyalty,&pCurrElement);

	if(FALSE == (ChrAll((char*)pInfo.CardSaleExtraData6.sClubCardHashNumber,sizeof(pInfo.CardSaleExtraData6.sClubCardHashNumber),' ')))	
		SetCardHashNumberToTrackNumber(pInfo, xmlInitDoc, pTrack2Loyalty);
	else
		SetLoyalIdToTrackNumber(pInfo, xmlInitDoc, pTrack2Loyalty);

	AddLineWithNulll(&xmlInitDoc, &pTrack2Loyalty, "LockingKey", (BYTE*)pInfo.CardSaleExtraData5.sLockingKey, sizeof(pInfo.CardSaleExtraData5.sLockingKey));	// CR 425758

	xmlInitDoc.SetAttribute(XMLSTR_PsAllSchemeActive,"0",&pTrack2Loyalty);
	xmlInitDoc.SetAttribute(XMLSTR_PsDummyCrdFlg,"0",&pTrack2Loyalty);
	xmlInitDoc.SetAttribute(XMLSTR_lSchemeNo,"2",&pTrack2Loyalty);
}

void CFuelLoyaltyPS::SetLanguageId( PAY_AT_PUMP_INFO &pInfo, CXMLInterface &xmlInitDoc, CXMLPointers &pCurrElement )
{
	CXMLPointers pLanguage;
	pLanguage.m_pMSElement= NULL;
	pLanguage.m_pMSNode = NULL;

	if(pInfo.CardSaleExtraData2.sLanguage != ' ')
	{
		CString tmp;
		tmp.Format("%d",pInfo.CardSaleExtraData2.sLanguage);
		xmlInitDoc.CreateElement(XMLSTR_Language,&pLanguage);
		xmlInitDoc.SetAttribute(XMLSTR_LanguageId,tmp.GetBuffer(0),&pLanguage);
		xmlInitDoc.AppendChild(&pLanguage,&pCurrElement);
	}
}


void CFuelLoyaltyPS::SetCardSegment( PAY_AT_PUMP_INFO &pInfo, CXMLInterface &xmlInitDoc, CXMLPointers &pCurrElement )
{
	long lPumpNum = a2l(pInfo.CardSaleInfo.cardData.sPumpNumber, sizeof(pInfo.CardSaleInfo.cardData.sPumpNumber));
	CXMLPointers pCardSegment;

	pCardSegment.m_pMSElement= NULL;
	pCardSegment.m_pMSNode = NULL;

	CPumpTransact trs;
	int iFoundTrs = 0;
	if (iFoundTrs == _Module.m_server.m_cPumpArray[lPumpNum - 1].GetInProgressWaitProgressTrs(&trs)) //0 - found
		xmlInitDoc.SetAttribute(XMLSTR_TrsNum, trs.m_lNumber, &pCurrElement);

	if(pInfo.CardSaleExtraData3.sCardSegment != ' ')
	{
		xmlInitDoc.CreateElement(XMLSTR_CardSegment,&pCardSegment);
		xmlInitDoc.AppendChild(&pCardSegment,&pCurrElement);
		AddLineWithNulll(&xmlInitDoc,&pCardSegment,XMLSTR_SegmentID, &pInfo.CardSaleExtraData3.sCardSegment,sizeof(pInfo.CardSaleExtraData3.sCardSegment)); 
	}
}



/******************************************************************************
 Description:  AddMemberInfo

 Parameters:    
							

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 OmerT          24/09/2013   
******************************************************************************/
BOOL CFuelLoyaltyPS::AddMemberInfo( long lPumpNumber )
{
	return m_cLoyaltyCommandExecuter->AddMemberInfo(lPumpNumber,m_lSegmentId[lPumpNumber]);	
}

void CFuelLoyaltyPS::SetAddMemberInfo( long lPumpNumber,long lVal)
{
	if(m_lSegmentId[lPumpNumber-1] || lVal)
	{
		CString sMsg;
		sMsg.Format("FuelLoyaltyPS::SetAddMemberInfo current semgnet = %ld , new segment = %ld", m_lSegmentId[lPumpNumber-1],lVal);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
	}

	m_lSegmentId[lPumpNumber-1] = lVal;
}

// TD 305114
BOOL CFuelLoyaltyPS::IsNeedToAddMemberInfo(long lPumpNumber)
{
	return m_lSegmentId[lPumpNumber-1] > 0;
}
// 4.0.32.??? TD 405435
void CFuelLoyaltyPS::SetCardHashNumberToTrackNumber( PAY_AT_PUMP_INFO & pInfo, CXMLInterface & xmlInitDoc, CXMLPointers pTrack2Loyalty )
{
	if(!(ChrAll((char*)pInfo.CardSaleExtraData6.sClubCardHashNumber,sizeof(pInfo.CardSaleExtraData6.sClubCardHashNumber),' ')))
	{
		long iFirstSpace = 0;
		for(iFirstSpace=0; iFirstSpace < sizeof(pInfo.CardSaleExtraData6.sClubCardHashNumber); iFirstSpace++)
		{
			if (pInfo.CardSaleExtraData6.sClubCardHashNumber[iFirstSpace] == ' ')
				break;
		}

		AddLineWithNulll(&xmlInitDoc,&pTrack2Loyalty, XMLSTR_trackNumber,(BYTE*)pInfo.CardSaleExtraData6.sClubCardHashNumber,iFirstSpace);
	}
	else
	{
		xmlInitDoc.SetAttribute(XMLSTR_trackNumber,"",&pTrack2Loyalty);
		long lPumpNum = a2l(pInfo.CardSaleInfo.cardData.sPumpNumber, sizeof(pInfo.CardSaleInfo.cardData.sPumpNumber));
		_LOGMSG.LogMsg("CFuelLoyaltyPS","SetCardHashNumberToTrackNumber", lPumpNum, LOG_PUMP, "There's no loyalty card to send to Loyalty Server");	//4.0.20.0
	}
}

void CFuelLoyaltyPS::SetLoyalIdToTrackNumber( PAY_AT_PUMP_INFO &pInfo, CXMLInterface &xmlInitDoc, CXMLPointers pTrack2Loyalty )
{
	if(!(ChrAll((char*)pInfo.CardSaleInfo.extraData.sLoyalId,sizeof(pInfo.CardSaleInfo.extraData.sLoyalId),' ')))
	{
		long iFirstSpace = 0;
		for(iFirstSpace=0; iFirstSpace < sizeof(pInfo.CardSaleInfo.extraData.sLoyalId); iFirstSpace++)
		{
			if (pInfo.CardSaleInfo.extraData.sLoyalId[iFirstSpace] == ' ')
				break;
		}

		AddLineWithNulll(&xmlInitDoc,&pTrack2Loyalty, XMLSTR_trackNumber,(BYTE*)pInfo.CardSaleInfo.extraData.sLoyalId,iFirstSpace);
	}
	else
	{
		xmlInitDoc.SetAttribute(XMLSTR_trackNumber,"",&pTrack2Loyalty);
		long lPumpNum = a2l(pInfo.CardSaleInfo.cardData.sPumpNumber, sizeof(pInfo.CardSaleInfo.cardData.sPumpNumber));
		_LOGMSG.LogMsg("CFuelLoyaltyPS","SetLoyalIdToTrackNumber", lPumpNum, LOG_PUMP, "There's no loyalty card to send to Loyalty Server");	//4.0.20.0
	}
}

void CFuelLoyaltyPS::SetParam( long lPumpNumber )
{
	// TD 456223
	long lTrsNumber = 0;
	bstr_t bstr(L"  ");

	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&info);
	m_cLoyaltyCommandExecuter->SetParam(lPumpNumber, lTrsNumber, info.CardSaleExtraData2.sLanguage, bstr);
	
}


// 4.0.32.220 TD 405631
void CFuelLoyaltyPS::SetLoyaltyPointsToCardSale( long lPumpNumber, long lTrsNumber , PAY_AT_PUMP_INFO &cTmpInfo )
{
	char szLoyaltyFile[MAX_FILE_PATH] = {0};
	_Module.m_server.m_cCommonFunction.GetLoyaltySummaryXmlFileName(lPumpNumber,lTrsNumber,szLoyaltyFile);
	
	long lOpenBalance = 0, lPointsEarned = 0, lEndningBalance = 0;
	
	CXmlParser cXmlParser;
	BOOL bRetCode = cXmlParser.ParseLoyaltySummaryXml(szLoyaltyFile, lPointsEarned, lOpenBalance, lEndningBalance);


	if (!bRetCode)
	{
		memset(cTmpInfo.CardSaleInfo.extraData.sLoyalPoints, ' ', sizeof(cTmpInfo.CardSaleInfo.extraData.sLoyalPoints));
		memset(cTmpInfo.CardSaleExtraData6.sLoyaltyPointsExt, ' ', sizeof(cTmpInfo.CardSaleExtraData6.sLoyaltyPointsExt));
	}
	else
	{
		Convertl2Str2WithBlanks(lEndningBalance, cTmpInfo.CardSaleExtraData6.sLoyaltyPointsExt, sizeof(cTmpInfo.CardSaleExtraData6.sLoyaltyPointsExt),
			cTmpInfo.CardSaleInfo.extraData.sLoyalPoints, sizeof(cTmpInfo.CardSaleInfo.extraData.sLoyalPoints));
	}	
}

void CFuelLoyaltyPS::ParseTotalDiscountFromTriggerXml(long lPumpNumber,long &lTotalReward, CXMLInterface &xmlInitDoc, CXMLPointers &pPromoSrvElement, long lTrsNumber, PAY_AT_PUMP_INFO &cTmpInfo )
{
	CString strMsg;
	long lRewordDest=0, lPrevTotalReward=0, lAmountToPay=0,lPumpIndex = lPumpNumber-1;
	lPrevTotalReward = GetTotalDiscount(lPumpIndex);

	lTotalReward = xmlInitDoc.GetAttribute(XMLSTR_TotalRewardAmount,&pPromoSrvElement);
	lTotalReward *= 10;	//4.0.20.60 64904

	if (lTotalReward < 0)	//4.0.20.11 63123
	{
		strMsg.Format("CFuelLoyaltyPS::ParseTriggerPromFromRtaps Total ticket discount is negative -> will be zero, pump = %ld", lPumpNumber);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
		lTotalReward = 0;
	}
	else
	{
		strMsg.Format("ParseTriggerPromFromRtaps Total Reward Amount for trs %ld is = [%ld], previous discount = [%ld], Total Discount = [%ld]",
			lTrsNumber,lTotalReward,lPrevTotalReward,lTotalReward + lPrevTotalReward);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);

	}

	// update totalReward with the previews total reward.
	lTotalReward += lPrevTotalReward;

	//No card sale data for IFSF EPS
	if (FALSE == (_Module.m_server.GetIFSFTerminalSrvInUse()))	//4.0.19.800 //4.0.23.504 66705
	{
		SetTotalDiscount(lPumpIndex,lTotalReward);
		long lFuelAmt = ConvertStringsToLong(cTmpInfo.CardSaleInfo.extraData.sTotalAmt,sizeof(cTmpInfo.CardSaleInfo.extraData.sTotalAmt),cTmpInfo.CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(cTmpInfo.CardSaleInfo.extraData.sTotalAmt_Msb)); //4.0.20.57 //4.0.19.710 66705

		if (lFuelAmt >= lTotalReward)
		{
			lAmountToPay = lFuelAmt - lTotalReward;
		}
		else
		{
			lTotalReward = lFuelAmt;
			strMsg.Format("CFuelLoyaltyPS::ParseTriggerPromFromRtaps Additional total ticket discount is more than fuel amount- total amount is zero, pump = %ld", lPumpNumber);
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
		}

		l2a(lTotalReward,cTmpInfo.CardSaleInfo.extraData.sGRTotalDiscount,sizeof(cTmpInfo.CardSaleInfo.extraData.sGRTotalDiscount));	//4.0.20.56
		Convertl2Str2(lAmountToPay  ,cTmpInfo.CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(cTmpInfo.CardSaleInfo.extraData.sTotalAmt_Msb) , cTmpInfo.CardSaleInfo.extraData.sTotalAmt, sizeof(cTmpInfo.CardSaleInfo.extraData.sTotalAmt));	

		strMsg.Format("Current discount from Loyalty Server: %ld, Current total amount to pay: %ld", lTotalReward, lAmountToPay); //4.0.20.0
		_LOGMSG.LogMsg("CFuelLoyaltyPS","ParseTriggerPromFromRtaps",lPumpNumber,LOG_PUMP,strMsg);
	}

	CXMLPointers pTriggeredPromElements, pCurrPromElement, pListNodes;
	BOOL bRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_TriggeredPromElements,&pTriggeredPromElements,&pPromoSrvElement);
	if(xmlInitDoc.HasChildNodes(&pTriggeredPromElements))
	{
		xmlInitDoc.GetChildNodesList(&pTriggeredPromElements, &pListNodes);
		//if (pTriggeredPromElements.m_pMSListOfItems)
		{
			BOOL bFoundTriggerredProm = FALSE;
			while (xmlInitDoc.GetNextItem(pListNodes, pCurrPromElement))
			{
				long lAmount = (long)(xmlInitDoc.GetAttribute(XMLSTR_Amount, &pCurrPromElement)) * 10;
				long lRewardType = xmlInitDoc.GetAttribute(XMLSTR_RewardType, &pCurrPromElement);
				BOOL bTenderProm = xmlInitDoc.GetAttribute(XMLSTR_TenderSpendCondition, &pCurrPromElement);
				long lRewardCPL = xmlInitDoc.GetAttribute(XMLSTR_OriginalRewardField, &pCurrPromElement);
				double  dDecimalRewardAmount = xmlInitDoc.GetAttributeToDoubleWithConvert(XMLSTR_DecimalRewardAmount, &pCurrPromElement);
				long    lDecimalRewardAmount = RoundDouble(dDecimalRewardAmount, 1000);
				if (bTenderProm)
				{
					BYTE sTmp[100] = { 0 };
					xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_Description, sTmp, &pCurrPromElement);
					memcpy(cTmpInfo.CardSaleExtraData6.sTenderPromotionDescription, sTmp, min(sizeof(sTmp), sizeof(cTmpInfo.CardSaleExtraData6.sTenderPromotionDescription)));

					long lCurrentTenderDiscount = a2l(cTmpInfo.CardSaleExtraData6.sTenderDiscount, sizeof(cTmpInfo.CardSaleExtraData6.sTenderDiscount)); // TD 467180
					l2a(lAmount + lCurrentTenderDiscount, cTmpInfo.CardSaleExtraData6.sTenderDiscount, sizeof(cTmpInfo.CardSaleExtraData6.sTenderDiscount));

					l2a(lRewardCPL, cTmpInfo.CardSaleExtraData6.sTenderRewardCPL, sizeof(cTmpInfo.CardSaleExtraData6.sTenderRewardCPL));

					strMsg.Format("Found Tender promotion %.20s with discount = [%d], CPL Rate is = [%d],", sTmp, lAmount, lRewardCPL);
					_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseTriggerPromFromRtaps", lPumpNumber, LOG_PUMP, strMsg);
				}
				else if (lRewardType == 9)
				{
					BYTE sTmp[100] = { 0 };
					xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_Description, sTmp, &pCurrPromElement);
					memcpy(cTmpInfo.CardSaleExtraData6.CarWashItem.sPromotionDescription, sTmp, min(sizeof(sTmp), sizeof(cTmpInfo.CardSaleExtraData6.CarWashItem.sPromotionDescription)));

					long lCurrentCarWashDiscount = a2l(cTmpInfo.CardSaleExtraData6.CarWashItem.sTotalDiscount, sizeof(cTmpInfo.CardSaleExtraData6.CarWashItem.sTotalDiscount));// TD 467180
					l2a(lAmount + lCurrentCarWashDiscount, cTmpInfo.CardSaleExtraData6.CarWashItem.sTotalDiscount, sizeof(cTmpInfo.CardSaleExtraData6.CarWashItem.sTotalDiscount));

					l2a(lRewardCPL, cTmpInfo.CardSaleExtraData6.sCWRewardCPL, sizeof(cTmpInfo.CardSaleExtraData6.sCWRewardCPL));

					strMsg.Format("Found CPL promotion %.20s with discount = [%d], CPL Rate is = [%d], ", sTmp, lAmount, lRewardCPL);
					_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseTriggerPromFromRtaps", lPumpNumber, LOG_PUMP, strMsg);
				}
			}
		}
	}

}

void CFuelLoyaltyPS::UpdateTriggerDiscountWithExtraInfoQdx(long lPumpNumber,long lTrsNumber, long &lReceiptExtraRecIndex,long lTotalReward, PAY_AT_PUMP_INFO &cTmpInfo )
{
	EXTRA_INFO_REC	ExtraInfoRec; //4.0.19.800 //4.0.23.504 66705
	ExtraInfoRec.ClearRecord(); // RFUEL-3708

 //4.0.23.504 66705

	long lExtraInfoRtc = 0;

	// For IFSF EPS promotion - we insert the discount into extra info. Will be later taken by TerminalSrv to be reported to EPS.
	if (_Module.m_server.GetIFSFTerminalSrvInUse())	//4.0.19.800 //4.0.23.504 66705
	{
		ExtraInfoRec.info.lIndexNumber = lTrsNumber;
		lExtraInfoRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);
		CString str;
		if (!lExtraInfoRtc)
		{
			lReceiptExtraRecIndex = ExtraInfoRec.info.lSeqNumber;
			str.Format("ParseTriggerProm, Extra info rec found when IFSFTerminalSrvInUse, SeqNum = %ld", lReceiptExtraRecIndex);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
			ExtraInfoRec.info.lTransactionAmount -= lTotalReward;
			lExtraInfoRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_UPDATE, &ExtraInfoRec); //Update extra info record

		}
		else
		{
			str.Format("ParseTriggerProm, No extra info rec found when IFSFTerminalSrvInUse");
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
		}
	}
	else
	{
		lReceiptExtraRecIndex = a2l(cTmpInfo.CardSaleInfo.cardData.sTranNbr, sizeof(cTmpInfo.CardSaleInfo.cardData.sTranNbr));
	}
}

long CFuelLoyaltyPS::GetTimerCode( IN long lPumpIndex )
{
	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[lPumpIndex].GetPAPInfo(&info);

	return a2l(&info.CardSaleExtraData5.sLoyalyTimerCode,sizeof(info.CardSaleExtraData5.sLoyalyTimerCode));
}

void CFuelLoyaltyPS::ParseTerminalMessagesFromRtaps(char * strXML, PAY_AT_PUMP_INFO &cTempInfo)
{
	BOOL			  bRetCode = TRUE, bFirstFullZone = TRUE;
	CXMLInterface	  xmlInitDoc;
	CXMLPointers	  pPrintMessagesElement, pAllPromNode, pTargetMessagesNode, pListNodes, pCurrPrintElement, pCurrSectionElement, pCurrDocumentElement, pListPrintNodes, pListSectionNodes, pListDocumentNodes;
	long p = 0, lPumpNumber = 0;
	
	try
	{
		long	lRetCode = xmlInitDoc.InitDoc();
		lRetCode = xmlInitDoc.LoadXmlString(strXML);

		if (lRetCode != 0)
		{
			bRetCode = xmlInitDoc.SelectSingleElementNode("AllProm"  /*Noam - Need to check this seems like the right string should be ALLProm not AllProm*/ , &pAllPromNode);
			if (bRetCode)
			{
				lPumpNumber = xmlInitDoc.GetAttribute(XMLSTR_PumpNumber, &pAllPromNode);
				p = lPumpNumber - 1;

				
				bRetCode = xmlInitDoc.SelectSingleElementNode(XMLSTR_TerminalMessages, &pTargetMessagesNode, &pAllPromNode);

				if (bRetCode)
				{
					xmlInitDoc.GetChildNodesList(&pTargetMessagesNode, &pListNodes);
					if (pListNodes.m_pMSListOfItems)
					{
						while (xmlInitDoc.GetNextItem(pListNodes, pPrintMessagesElement))	//PrintMessages elements
						{
							long lExistsReceiptPrinting = 0;
							char sTargetMessageText[MAX_BUF] = { 0 };
							BYTE sPrintingType[100] = { 0 };
							xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_PrintingType, sPrintingType, &pPrintMessagesElement);

							
							xmlInitDoc.GetChildNodesList(&pPrintMessagesElement, &pListPrintNodes);
							if (pListPrintNodes.m_pMSListOfItems)
							{
								while (xmlInitDoc.GetNextItem(pListPrintNodes, pCurrPrintElement)) //PrintMessage elements
								{
									xmlInitDoc.GetChildNodesList(&pCurrPrintElement, &pListDocumentNodes);
									if (pListDocumentNodes.m_pMSListOfItems)
									{
										while (xmlInitDoc.GetNextItem(pListDocumentNodes, pCurrDocumentElement)) //Document elements
										{
											xmlInitDoc.GetChildNodesList(&pCurrDocumentElement, &pListSectionNodes);
											if (pListSectionNodes.m_pMSListOfItems)
											{
												while (xmlInitDoc.GetNextItem(pListSectionNodes, pCurrSectionElement)) //Section elements
												{
													if (IsTargetMessage((const char*)sPrintingType))
													{
														
														if (ParseTargetMessageTextXml(&xmlInitDoc, &pCurrSectionElement, sTargetMessageText))
														{
															memcpy(cTempInfo.CardSaleExtraData2.sReserveForCoupon, sTargetMessageText, sizeof(cTempInfo.CardSaleExtraData2.sReserveForCoupon));
														}
													}
													else if (IsCommunicationReward((const char*)sPrintingType))
													{
														if (ParseCommunicationRewardTextXml(&xmlInitDoc, &pCurrSectionElement, sTargetMessageText))
														{
															memcpy(cTempInfo.CardSaleExtraData6.sCommunicationReward, sTargetMessageText, sizeof(cTempInfo.CardSaleExtraData6.sCommunicationReward));
														}
														
													}
												}
											}
										}
									}
								}
							}

							

						} // end of print messages while 
					}
						else
						{
							_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseTerminalTargetMessagesFromRtaps", lPumpNumber, LOG_PUMP, "No target messagee received from Loyalty server");
						}
					
				}
				else
				{
					_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseTerminalTargetMessagesFromRtaps", lPumpNumber, LOG_PUMP, "No target message received from Loyalty server");

				}
			}
			else
				_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseTerminalTargetMessagesFromRtaps", lPumpNumber, LOG_PUMP, "Missing data from Loyalty server");

		}
		else
			_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseTriggerPromFromRtaps", lPumpNumber, LOG_PUMP, "No data return from Loyalty server");

	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseTriggerPromFromRtaps", lPumpNumber, LOG_PUMP, "Unexpected Error Had occurred ");
		bRetCode = FALSE;
	}

	_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseTriggerPromFromRtaps", lPumpNumber, LOG_PUMP, "finish successfully");

	xmlInitDoc.CloseDoc();
	
}

BOOL CFuelLoyaltyPS::CheckMemberRestriction(const int & iMemberStatus)
{
	auto it = m_dwFuelLoyaltyMembersMap.find(iMemberStatus);
	if (it != m_dwFuelLoyaltyMembersMap.end())
	{
		return m_dwFuelLoyaltyMembersMap.find(iMemberStatus)->second;
	}
	return FALSE;
}

BOOL CFuelLoyaltyPS::CheckCardRestriction(const int &iCardStatus)
{
	auto it = m_dwFuelLoyaltyCardsMap.find(iCardStatus);
	if (it != m_dwFuelLoyaltyCardsMap.end())
	{
		return m_dwFuelLoyaltyCardsMap.find(iCardStatus)->second;
	}
	return FALSE;
}


void CFuelLoyaltyPS::SetZoneIndexAccordingToLoyaltyMessageType(CXMLInterface &xmlInitDoc, CXMLPointers pPrintMessagesElement, int iDefaultIndex, OUT int &iZoneIndex)
{
	if(_Module.m_server.IsSortedLoyaltyMessages())
	{
		BYTE sPrintingType[100] = {0};
		xmlInitDoc.GetAttributeToByteArrWithConvert(XMLSTR_PrintingType,sPrintingType,&pPrintMessagesElement);

		if(IsRewardSimulation((const char*)sPrintingType))
		{
			iZoneIndex = 1;
		}
		else if(IsLoyaltySummary((const char*)sPrintingType))
		{
			iZoneIndex = 2; 
		}
		else if(IsVoucher((const char*)sPrintingType))
		{
			iZoneIndex = 3;
		}
		else if(IsTargetMessage((const char*)sPrintingType))
		{
			iZoneIndex = 4;
		}
		else if(IsIncentiveMessage((const char*)sPrintingType))
		{
			iZoneIndex = 5;
		}	
		else 
			iZoneIndex = iDefaultIndex;

	}
	else 
		iZoneIndex = iDefaultIndex;
}

void CFuelLoyaltyPS::AddTotalTenderAmount(long lPumpNumber, PAY_AT_PUMP_INFO & cTmpPapInfo, CXMLInterface &xmlInitDoc, CXMLPointers & pPumpsPropElement, CXMLPointers  &pRootElement, long lNacsCode, CPumpTransact *pTrs)
{
	long lTotalAmountWithoutDiscount = a2l(cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount, sizeof(cTmpPapInfo.CardSaleExtraData3.sNonDiscountTotalAmount));

	long lTotalAmountAfterDiscount = ConvertStringsToLong(cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt, sizeof(cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt), cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt_Msb, sizeof(cTmpPapInfo.CardSaleInfo.extraData.sTotalAmt_Msb));

	long lPumpIndex = lPumpNumber - 1;

	DOUBLE dFirstCardTenderAmount = 0, dSecondCardTenderAmount = 0;

	_Module.m_server.CalculateTendersTotalAmount(lPumpNumber, lTotalAmountAfterDiscount, dFirstCardTenderAmount, dSecondCardTenderAmount);

	float fAmount = (float)lTotalAmountWithoutDiscount;
	long lAmtToXml = FloatToLong(fAmount);
	xmlInitDoc.SetAttribute(XMLSTR_Amount, lAmtToXml, &pPumpsPropElement);

	float fDiscountTotalAmt;
	if (_Module.m_server.GetTwoTendersInTransaction())
		fDiscountTotalAmt = (float)dFirstCardTenderAmount;
	else
		fDiscountTotalAmt = (float)lTotalAmountAfterDiscount;

	long lTenderNum = GetTenderNumber(cTmpPapInfo);
	xmlInitDoc.SetAttribute(XMLSTR_TenderNum, lTenderNum, &pPumpsPropElement);
	long lTotalAmount = FloatToLong(fDiscountTotalAmt);
	xmlInitDoc.SetAttribute("TotalAmtAfterDiscount", lTotalAmount, &pPumpsPropElement);  //4.0.23.460 124743

	CString sMsg;
	sMsg.Format("Tender number = %ld, Tender Amount = %d", lTenderNum, lTotalAmount);
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg);

	if (_Module.m_server.GetTwoTendersInTransaction()) // TD 404465
	{
		lTenderNum = GetTenderNumber(cTmpPapInfo, FALSE);
		AddTenderToXml(lPumpNumber, xmlInitDoc, pRootElement, lTenderNum, dSecondCardTenderAmount);
	}


}


void CFuelLoyaltyPS::AddTenderToXml( long lPumpNumber ,CXMLInterface &xmlInitDoc, CXMLPointers & pRootElement, long lTenderNum, DOUBLE dTenderAmount )
{
	if(dTenderAmount > 0)
	{
		CXMLPointers pPumpsPropElement;
		xmlInitDoc.CreateElement(XMLSTR_PUMPSPROP,&pPumpsPropElement);
		xmlInitDoc.AppendChild(&pPumpsPropElement,&pRootElement);
		xmlInitDoc.SetAttribute(XMLSTR_TenderNum,lTenderNum,&pPumpsPropElement);

		float fDiscountTotalAmt = (float)dTenderAmount;
		long lTotalAmount = FloatToLong(fDiscountTotalAmt);
		xmlInitDoc.SetAttribute(XMLSTR_TotalAmtAfterDiscount, lTotalAmount ,&pPumpsPropElement);


		CString sMsg;
		sMsg.Format("Tender number = %ld, Tender amount = %d",lTenderNum,lTotalAmount);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
	}
}

BOOL CFuelLoyaltyPS::SaveTriggerPromoationsXml(long lPumpNumber, PAY_AT_PUMP_INFO &cTmpInfo, CXMLInterface &xmlInitDoc)
{
	long p = lPumpNumber - 1;
	//Save receipt printing on the disk for VP //4.0.??.?? 291216
	long lTrsNum = a2l(cTmpInfo.CardSaleInfo.cardData.sTranNbr, sizeof(cTmpInfo.CardSaleInfo.cardData.sTranNbr));
	char szFileName[MAX_PATH] = { 0 };
	_Module.m_server.m_cCommonFunction.GetLoyaltyPromotionsXmlFileName(lPumpNumber, lTrsNum, szFileName); // TD 321602   4.0.111.174 
	BOOL bRetCode = xmlInitDoc.SaveToFile(szFileName);
	if (bRetCode)
	{
		CString strMsg;
		strMsg.Format("Save Loyalty Promotions, Saved xml to file %s successfully", szFileName);
		_Module.m_server.m_cLogger.LogMsg(strMsg);
	}
	else
	{
		CString strMsg;
		strMsg.Format("SaveLoyaltyReceiptInfo, Failed to save xml to file %s", szFileName);
		_Module.m_server.m_cLogger.LogMsg(strMsg, LOG_LEVEL_0);
	}

	if (LOG_FCC_FLOW_CONTROL)
	{
		char strFileName[HOME_DIR_STRING_LENGTH];
		sprintf_s(strFileName, _countof(strFileName), "C:\\LPE\\XML\\PS_TriggerPromFromRtaps_%ld.xml", GetTotalDiscount(p));
		xmlInitDoc.SaveToFile(strFileName);
	}				
	
	return bRetCode;
}


/******************************************************************************
Description:  GetMemebrInfo from LPE

Parameters:


WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
OmerT       02/05/2016				CR 405435
******************************************************************************/
BOOL CFuelLoyaltyPS::GetMemberInfo(long lPumpNumber)
{
	long		p = lPumpNumber - 1;
	char		strMsg[MAX_FIELD_VAL] = { 0 };
	char		strXML[MAX_BUF];
	BOOL		bRetCode = TRUE;
	
	PAY_AT_PUMP_INFO	papInfo;
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&papInfo);

	if (!_Module.m_server.m_cFuelLoayaltyPS.GetInitDllFlag())//we have to init
	{
		bRetCode = FALSE;	//4.0.22.504 69344
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "GetMemberInfo", lPumpNumber, LOG_PUMP, "GetMemberInfo wasn't called after init failed.", LOG_LEVEL_1);
	}
	try
	{
		if (bRetCode)
		{
			bRetCode = CreateAddMemberCardXML(papInfo, strXML);
			CComBSTR	sXML(strXML);
			bRetCode = m_cLoyaltyCommandExecuter->GetMemberInfo(lPumpNumber, sXML.m_str);
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				sprintf_s(strMsg, _countof(strMsg), "CFuelLoyaltyPS::GetMemberInfo, GetMemberInfo was sent, pump = %ld", lPumpNumber);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
			}
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "GetMemberInfo", lPumpNumber, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1);
	}

	SetTimer(p, ADD_GET_MEMBER_INFO_CHECK_TIMEOUT);

	return bRetCode;
}

/******************************************************************************
Description: Handle GetMemberInfo event from loayltFuel

Returns:

Parameters:

WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
Omer	   02/05/2016       		Start			// CR 405435
*****************************************************************************/
BOOL CFuelLoyaltyPS::HandleGetMemberInfoResult(long lPumpNumber,char * sMemberInfoResonseXml)
{
	ClearTimer(lPumpNumber-1);

	PAY_AT_PUMP_INFO cInfo;
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cInfo);

	OLA_STAT ola;
	_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetOlaStat(&ola);

	if (sMemberInfoResonseXml)
		_Module.m_server.m_cXmlParser.ParseGetMemberInfoMessage(sMemberInfoResonseXml, &cInfo);

	ola.m_byState = OLA_LOYALTY_AUTHORIZE_LOYALTY;

	_Module.m_server.m_cFlowControlManager.HandleNMPSessions(lPumpNumber, &ola, &cInfo, TRUE);

	return TRUE;
}




/******************************************************************************
Description: Handles Error in ParsePromotionsFromRtaps

Returns:

Parameters:

WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
OmerT		11/06/2014	 19:34		// 4.0.23.2390  TD 406362
******************************************************************************/
void CFuelLoyaltyPS::ParsePromotionsError(long lPumpNumber)
{
	if (lPumpNumber > 0 && lPumpNumber < MAX_PUMPS_64)
	{
		_LOGMSG.LogClassMsg("CFuelLoyaltyPS", "ParsePromotionsError", lPumpNumber, LOG_PUMP, "Continue the flow after query!");

		OLA_STAT		  ola;
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetOlaStat(&ola);

		PAY_AT_PUMP_INFO  cTmpInfo;
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);

		if(ola.m_byState != OLA_NONE) // RFUEL-2184
			_Module.m_server.m_cFlowControlManager.StartAfterQueryFlowControl(lPumpNumber, ola, cTmpInfo);
		else
		{
			_LOGMSG.LogClassMsg("CFuelLoyaltyPS", "ParsePromotionsError", 0, LOG_PUMP, "OLA state is OLA_NONE. Ignore and continue from IDLE "); // RFUEL-2184
		}
	}
	else
	{
		_LOGMSG.LogClassMsg("CFuelLoyaltyPS", "ParsePromotionsError", 0, LOG_NONE, "Invalid PumpNumber!");
	}
}

BOOL CFuelLoyaltyPS::GetInitDllFlag()
{
	return m_lInitDllFlag;
}

BOOL CFuelLoyaltyPS::CreateAddItemEmeraldXML(CComBSTR& bstrOutXml, PumpItems& cItems, long lPumpNumber, CPumpTransact trs, double fTotalDiscount)
{
	return m_cLoyaltyCommandExecuter->CreateAddItemXML(bstrOutXml, cItems, lPumpNumber, trs, fTotalDiscount);
}

void CFuelLoyaltyPS::ParseCarWashPromotions(long lPumpNumber, CXMLInterface &xmlInDoc, CXMLPointers pRootNode, long lUseAdditionalCode, CXMLPointers pAdditionalCodeNode, PAY_AT_PUMP_INFO &papInfo)
{
	CXMLPointers    pChildNode, pAllProm, pCurrElement, pSinglePromotionNode, pCarWashPromoPrice, pCarWashPromoID, pCarWashPromoName, pCarWashPluCode;
	char sTmp[MAX_FIELD_VAL] = { 0 };
	CString sMsg;
#ifdef DEBUG
	xmlInDoc.SaveToFile("c:\\LPE\\XML\\LookingForCarWash.xml"); //noam tbd
#endif

	_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseCarWashPromotions", 0, LOG_PUMP, "enter"); 
	try
	{
		BOOL lRet = xmlInDoc.SelectSingleElementNode(XMLSTR_ALLProm, &pAllProm);
		if (lRet)
			lRet = xmlInDoc.SelectSingleElementNode(XMLSTR_PromoSrvMessageCarWash, &pSinglePromotionNode, &pAllProm);
		if (lRet)
		{
			long lDryProductPluCode = xmlInDoc.GetAttributeToLong(XMLSTR_CarWashPluCode, &pSinglePromotionNode);
			long lProdCode = a2l(papInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode, sizeof(papInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode));
			int i64PluCode = CFuelLoyaltyHelper::GetWashProgramNacsCodeByProgramCode(lProdCode);
			if (lDryProductPluCode == i64PluCode)
			{
				CTicketPromotionPtr pTicketPromotionPtr(new CTicketPromotion); // create a new CTicketPromotion
				pTicketPromotionPtr->SetProductPlu(lDryProductPluCode);
				pTicketPromotionPtr->AddField(XMLSTR_TicketPromotionType, XMLSTR_CarWashPromotion);  //update ticketpromotion
    
				memset(sTmp, 0, MAX_FIELD_VAL);
				long lCarWashUnitPrice = a2l((BYTE *)&papInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice, sizeof(papInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice));
				xmlInDoc.GetAttributeToByteArrWithConvert(XMLSTR_CarWashPromoDiscount,sTmp, &pSinglePromotionNode);
				pTicketPromotionPtr->AddField(XMLSTR_CarWashPromoDiscount, sTmp);  //update ticketpromotion
				long lCarWashDiscount = atol(sTmp) ;
				
				l2a(lCarWashUnitPrice, papInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice, sizeof(papInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice));
				if (lCarWashDiscount < lCarWashUnitPrice)  //don't set a carwash promotion if it's bigger than unit price...
				{
					lCarWashUnitPrice = lCarWashUnitPrice - lCarWashDiscount;
					// RFUEL - 3846	(Carwash discount was not applied at all.)
					l2a(lCarWashUnitPrice, papInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice, sizeof(papInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice));
					//RFUEL-2693  PAP carwash promotion not Showing on receipt
					l2a(lCarWashDiscount, papInfo.CardSaleExtraData6.CarWashItem.sTotalDiscount, sizeof(papInfo.CardSaleExtraData6.CarWashItem.sTotalDiscount));
				}
				else
				{
					CString sMsg;
					sMsg.Format("Error.. Received a carwash promotion with value greater than carwash unit price.  lCarWashUnitPrice = [%d], carwash promotion = [%d]", lCarWashUnitPrice, lCarWashDiscount);
					_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseCarWashPromotions", 0, LOG_PUMP, sMsg.GetBuffer(), LOG_LEVEL_1); //  4.0.23.980

				}

				//get promotion ID
				memset(sTmp, 0, sizeof(sTmp));
				xmlInDoc.GetAttributeToByteArrWithConvert(XMLSTR_CarWashPromoID, sTmp, &pSinglePromotionNode);
				pTicketPromotionPtr->AddField(XMLSTR_CarWashPromoID, sTmp); //update ticketpromotion


				//get promotion name
				memset(sTmp, 0, sizeof(sTmp)); 
				xmlInDoc.GetAttributeToByteArrWithConvert(XMLSTR_CarWashPromoName, sTmp, &pSinglePromotionNode);
				pTicketPromotionPtr->AddField(XMLSTR_CarWashPromoName, sTmp); //update ticketpromotion
				CString sName = sTmp;


				sMsg.Format("lCarWashUnitPrice = [%d], i64PluCode = [%d], Name = [%s]",lCarWashUnitPrice, i64PluCode, sName);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sTmp);

				m_TicketPromotionlist.AddPromotion(pTicketPromotionPtr);
				
			}
		}
		else
		{
			CString strMsg;
			strMsg.Format("No carwash promotion at pump %d ", lPumpNumber);
			_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseGradesFromRtaps", 0, LOG_NONE, strMsg);
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseCarWashPromotions", 0, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
	}
	_LOGMSG.LogMsg("CFuelLoyaltyPS", "ParseCarWashPromotions", 0, LOG_PUMP, "leave");
}


// RFUEL-2445 Minimum Reward limit should be applied irrespective of dispensed fuel product.
void CFuelLoyaltyPS::SetVolumeLimitToLoyaltyMin(long lPumpNumber, CXMLInterface &xmlInitDoc, PAY_AT_PUMP_INFO &cTmpInfo)  
{
	CXMLPointers pSingleVolumeLimitNode, pVolumeLimitNodes, pAllNodes, pRootNode;
	BOOL bElementRetrieved = FALSE, bFoundLimit = FALSE;
	long	dVolumeLimit = 0;
	CString str;

	if (LOG_DETAIL_FLOW_CONTROL) 
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "SetVolumeLimitToLoyaltyMin", 0, LOG_PUMP, "Enter",  LOG_LEVEL_3);

	try
	{
		memset(&(cTmpInfo.CardSaleExtraData4.sExistLoyaltyLimit), ' ', sizeof(cTmpInfo.CardSaleExtraData4.sExistLoyaltyLimit));	//4.0.20.300
		bElementRetrieved = xmlInitDoc.GetRootNode(&pRootNode);
		if (bElementRetrieved)
		{
			bElementRetrieved = xmlInitDoc.selectNodes(XMLSTR_VolumeLimitRes, &pVolumeLimitNodes,NULL);
		}
		if (bElementRetrieved && pVolumeLimitNodes.m_pMSListOfItems)
		{  
			// iterate all VolumeLimitRes nodes and get the lowest Limit attribute value, to set it as transaction loyalty volume limit
			while (xmlInitDoc.GetNextItem(pVolumeLimitNodes, pSingleVolumeLimitNode))
			{
				//55286 Copient 4.0.19.502 limit can exist only when discounts are the same for all grades
				double dCurrentVolumeLimit = xmlInitDoc.GetAttribute(XMLSTR_Limit, &pSingleVolumeLimitNode);
				if (dCurrentVolumeLimit)	//4.0.20.300
				{
					long lValue = RoundDouble(dCurrentVolumeLimit, 1000);
					if (lValue >= 0 && lValue < dVolumeLimit)  // Keep only the lowest Volume limit..
						dVolumeLimit = lValue;
					if (!bFoundLimit) // first catch set value
						dVolumeLimit = lValue;
					bFoundLimit = TRUE;
				}
			}

		}
		if (bFoundLimit) // For backwards code compatibility , we don't set this volume limit unless found such a value on loyalty response
		{
			l2a(dVolumeLimit, cTmpInfo.CardSaleExtraData4.sLoyaltyVolumeLimit, sizeof(cTmpInfo.CardSaleExtraData4.sLoyaltyVolumeLimit));
			str.Format("CFuelLoyaltyPS::SetVolumeLimitToLoyaltyMin, VolumeLimit is set to Loyalty minimum [%ld]", dVolumeLimit);
		}
		else
		{
			str.Format("CFuelLoyaltyPS::SetVolumeLimitToLoyaltyMin, Authorize amount stays the same after receiving data from loyalty server pump = %ld", lPumpNumber);
		}
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, str);
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "SetVolumeLimitToLoyaltyMin", 0, LOG_PUMP, "Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
	}

	if (LOG_DETAIL_FLOW_CONTROL)
		_LOGMSG.LogMsg("CFuelLoyaltyPS", "SetVolumeLimitToLoyaltyMin", 0, LOG_PUMP, "leave", LOG_LEVEL_3);
}



void  CFuelLoyaltyPS::SetMinLoyaltyUnitPrice(long val)
{
	m_lMinLoyaltyUnitPrice = val; //RFUEL-3309,RFUEL-3194 limit minimal unit price after loyalty discounts are applied
}

long  CFuelLoyaltyPS::GetMinLoyaltyUnitPrice() 
{
	return m_lMinLoyaltyUnitPrice;  //RFUEL-3309,RFUEL-3194 limit minimal unit price after loyalty discounts are applied
}



void CFuelLoyaltyPS::UpdateCentsOffPerGrade(long lPumpNumber, PAY_AT_PUMP_INFO & cTmpInfo)
{
	long p = lPumpNumber - 1;

	for (int i = 0; i < MAX_POSITIONS_PER_PUMP; i++)
	{
		long fTotalCentsOff = (m_Promotion[p].sPromID[i].fTotalCentsOff) * 10;
		if (fTotalCentsOff > 0)
		{
			CString sMsg;
			sMsg.Format("UpdateCentsOffPerGrade : m_PromotionArr[%ld].sPromIDArr[%ld]: CentsOff = %f", p, i, fTotalCentsOff);
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg);


			_Module.m_server.UpdateDiscountPerGrade(m_Promotion[p].sPromID[i].lGradeNumber, lPumpNumber, fTotalCentsOff, cTmpInfo);

			UpdatePumpWithLoyaltyVolumeLimit(cTmpInfo, lPumpNumber);	//4.0.20.300
		}
	}
}
