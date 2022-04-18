// ChangeProductInTank.cpp: implementation of the CChangeProductInTank class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChangeProductInTank.h"
#include "SQLPumpSrvFile.h"

/******************************************************************************/
/*******************	CChangeProductInTank class		***********************/
/******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChangeProductInTankSetup::CChangeProductInTankSetup()
{
	m_lTankNumber = 0;
	m_lGradeNumber = 0;
	m_lOldGradeNumber = 0; //4.0.23.500
	memset(m_szProductName, 0, sizeof(m_szProductName));
	m_bIsNewProduct = FALSE;
	m_lExtGradeNumber = 0; //4.0.26.501 - TD 116227

	//4.0.23.501 - TD 76330
	//4.0.25.30 - TD 127181
	ClearNewGradeDetails();	
}

CChangeProductInTankSetup::~CChangeProductInTankSetup()
{

}


/******************************************************************************
 Description:	Clear new grade details structure
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			07/06/2010   17:00		Start		4.0.25.30 - TD 127181
******************************************************************************/
void CChangeProductInTankSetup::ClearNewGradeDetails()
{
	memset(&m_cNewGradeDetails.byGradeFullName, 0, sizeof(m_cNewGradeDetails.byGradeFullName));
	memset(&m_cNewGradeDetails.byGradeShortName3, 0, sizeof(m_cNewGradeDetails.byGradeShortName3));
	memset(&m_cNewGradeDetails.byGradeShortName5, 0, sizeof(m_cNewGradeDetails.byGradeShortName5));
	memset(&m_cNewGradeDetails.byAdditionalProductCode, 0, sizeof(m_cNewGradeDetails.byAdditionalProductCode));
	m_cNewGradeDetails.lPriceCashSelf = 0;
	m_cNewGradeDetails.lPriceCashFull = 0;
	m_cNewGradeDetails.lPriceCreditSelf = 0;
	m_cNewGradeDetails.lPriceCreditFull = 0;
}


/******************************************************************************
 Description:	Handling request for changing product in tank
 Returns:      	long ret code
 Parameters:   	BSTR sRec - request data in xml format
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			23/02/2009   10:45		Start	
******************************************************************************/
long CChangeProductInTankSetup::HandleRequest(BSTR sRec)
{
	long lRetCode = CTRL_OK;
	CString str;
	
	if (SERVER_STATE_STARTUP)
		lRetCode = CTRL_CONTROLLER_NOT_AVAILABLE;

	else if (SERVER_NOT_IDLE)
		lRetCode = CTRL_SERVER_NOT_READY;

	else
	{
		long lTank = 0;
		if (_Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags()||
			_Module.m_server.m_cServerInfo.GetRequiredPriceChangeFlags()||
			_Module.m_server.m_cServerInfo.GetRequiredModeProcessFlags() ||
			_Module.m_server.m_cServerInfo.GetRequiredMaintenanceProcessFlags() ||
			_Module.m_server.m_cServerInfo.GetDeliveryEmulationStatus() ||
			_Module.m_server.m_cCommonFunction.IsDeliveryEmulationSingleTankInReadingProcess(lTank))
		{
			lRetCode = CTRL_IN_MIDDLE_OF_PROCESS;
			_LOGMSG.LogMiddleOfProcessReason();
		}
	}
	
	if (lRetCode == CTRL_OK)
	{
		//Nozzle to tank setup must be configured
		if (_Module.m_server.GetActiveTankConnectedPumpsTable() == FALSE)
		{				
			lRetCode = CTRL_FAILED_IO;
			if(LOG_BASIC_FLOW_CONTROL)
			{
				str.Format("Nozzle to tank must be configured, CONFIGURATION NOT VALID, ret=%ld", lRetCode);
				_LOGMSG.LogMsg(str);
			}	
		}
		else
		{
			// Remove 4.0.23.501 - TD 78465
			//Check offline pumps
//			for (int p = 0; p < _Module.m_server.GetLastValidPump(); p++) //loop on all valid pumps
//			{
//				if ( (!_Module.m_server.m_cCommonFunction.IsPumpIdle(p)) || 
//					(_Module.m_server.PumpNotInWorkingState(p+1)) )
//				{
//					lRetCode = CTRL_PUMP_NOT_IN_WORKING_STATE;
//					break;
//				}
//			}

			if (lRetCode == CTRL_OK)
			{
				//4.0.25.30 - TD 127181
				ClearNewGradeDetails();

				lRetCode = ParseData(sRec);
				
				if(!lRetCode)
				{
					if (LOG_BASIC_FLOW_CONTROL)
					{
						str.Format("Change product in tank record was found. TankId = %ld, GradeId = %ld, OldGradeId = %ld", m_lTankNumber, m_lGradeNumber, m_lOldGradeNumber);
						_LOGMSG.LogMsg(str);
					}

					//4.0.23.501 - TD 78465
					lRetCode = CheckOfflinePumps(); 
					if (lRetCode == CTRL_OK)
					{	
						lRetCode = CheckValidTankGrade();
						//4.0.23.501 - TD 76330
						if (lRetCode == -1) // if grade does not exist create new one
						{
							if (LOG_BASIC_FLOW_CONTROL)
							{
								_LOGMSG.LogMsg("CChangeProductInTankSetup::HandleRequest, Grade does not exist, create new one");
							}

							m_bIsNewProduct = TRUE;
							lRetCode = CreateNewProduct(sRec);
						}
						else
							m_bIsNewProduct = FALSE;

						// Save new setup request in ini file
						if (lRetCode == CTRL_OK)
						{
							_Module.m_server.m_cParam.SaveParam("GeneralParam","ChangeProductGradeId",m_lGradeNumber,TRUE,0,0,0,TRUE);
							_Module.m_server.m_cParam.SaveParam("GeneralParam","ChangeProductTankId",m_lTankNumber,TRUE,0,0,0,TRUE);
							_Module.m_server.m_cParam.SaveParam("GeneralParam","ChangeProductName",m_szProductName,TRUE,0,0,0,TRUE);
							_Module.m_server.m_cParam.SaveParam("GeneralParam","ChangeProductOldGradeId",m_lOldGradeNumber,TRUE,0,0,0,TRUE); //4.0.23.500
						}
					}
				}
			}
		}
	}

	return lRetCode;
}


/******************************************************************************
 Description:	Handling request for changing product in tank setup only, no process
 Returns:      	long ret code
 Parameters:   	BSTR sRec - request data in xml format
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			24/02/2009   11:00		Start	
******************************************************************************/
long CChangeProductInTankSetup::HandleRequestSetupOnly(BSTR sRec)
{
	long lRetCode = CTRL_OK;
	
	lRetCode = ParseData(sRec);

	if (lRetCode == CTRL_OK)
	{
		lRetCode = CheckValidTankGrade();
		if (lRetCode == CTRL_OK)
		{
			ChangeSetup();
		}
	}

	return lRetCode;
}


/******************************************************************************
 Description:	Parse the xml sent by BOS and retrieve the information 
				of changing product in tank
 Returns:      	CTRL return codes
 Parameters:   	BSTR sXmlRec - string in an XML format, which contains the data
				
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			23/02/2009   13:00		Start	
******************************************************************************/
long CChangeProductInTankSetup::ParseData(BSTR sXmlRec)
{
	long lRetCode = CTRL_RECORD_NOT_FOUND;
	BOOL bRetCode;
	
	char sXmlBuffer[MAX_XML_BUF_LEN] = {0};
	char sTankId[STR_LONG_CODE] = {0};
	char sGradeId[STR_LONG_CODE] = {0};
	char sProductName[GRADE_NAME] = {0};
	char sOldGradeId[STR_LONG_CODE] = {0}; //4.0.23.500
	
	int iLen = Bstr2Buffer(sXmlRec, (char *)sXmlBuffer, MAX_XML_BUF_LEN);
	if(iLen > 0)
	{
		CXMLInterface  xmlDoc;
		CXMLPointers   cTankIdElement, cGradeIdElement, cProductNameElement;

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;				
			str.Format("CChangeProductInTankSetup::ParseData. XML = %s", sXmlBuffer); 
			_LOGMSG.LogMsg(str); 
		}
		
		try
		{
			if (xmlDoc.InitDoc())
			{
				bRetCode = xmlDoc.LoadXmlString(sXmlBuffer);

				if (bRetCode)
				{
					xmlDoc.SelectSingleElementNode("TankId",&cTankIdElement);
					xmlDoc.GetChildNodeValueToByteArrWithConvert(&cTankIdElement, sTankId, STR_LONG_CODE);

					xmlDoc.SelectSingleElementNode("GradeId",&cGradeIdElement);
					xmlDoc.GetChildNodeValueToByteArrWithConvert(&cGradeIdElement, sGradeId, STR_LONG_CODE);

					xmlDoc.SelectSingleElementNode("ProductName",&cProductNameElement);
					xmlDoc.GetChildNodeValueToByteArrWithConvert(&cProductNameElement, sProductName, GRADE_NAME);

					//4.0.19.651
					xmlDoc.SelectSingleElementNode("OldGradeId",&cGradeIdElement);
					xmlDoc.GetChildNodeValueToByteArrWithConvert(&cGradeIdElement, sOldGradeId, STR_LONG_CODE);
					
					m_lTankNumber = a2l((BYTE *)sTankId, strlen(sTankId));
					m_lGradeNumber = a2l((BYTE *)sGradeId, strlen(sGradeId));
					strcpy(m_szProductName, sProductName);
					m_lOldGradeNumber = a2l((BYTE *)sOldGradeId, strlen(sOldGradeId)); //4.0.23.500

					lRetCode = CTRL_OK;

					//4.0.26.501 - TD 116227
					//Make extended grade conversion if needed
					if ((_Module.m_server.GetMaxGrade999()) && (m_lGradeNumber > 0))
					{
						m_lExtGradeNumber = m_lGradeNumber;
						if (!_Module.m_server.GetGradeByExtGrade(m_lGradeNumber, &m_lGradeNumber))
						{
							//Find first available grade
							FindNonValidGrade(m_lGradeNumber);
							if (!m_lGradeNumber)
								lRetCode = CTRL_INVALID_GRADE_NUMBER;
						}
						
						if (m_lGradeNumber)
							_Module.m_server.GetGradeByExtGrade(m_lOldGradeNumber, &m_lOldGradeNumber);
					}
				}
			}
		}
		catch(...)
		{
			_LOGMSG.LogMsg("CChangeProductInTankSetup","ParseData", 0, LOG_NONE, "Unexpected Error occurred");
		}
		xmlDoc.CloseDoc();
	}
	return lRetCode;
}

/******************************************************************************
 Description:	Find the first non valid grade
 Returns:      	void
 Parameters:   	long &lGradeId
				

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			29/04/2010   16:00		Start		4.0.26.501 - TD 116227
******************************************************************************/
void CChangeProductInTankSetup::FindNonValidGrade(long &lGradeId)
{
	char sNumber[8];
	CString str , sTable("Grades\\Grade");
	DWORD dwIsValid = 0;

	for(long lGrade = 1; lGrade <= MAX_GRADES_99; lGrade++)
	{
		memset(sNumber,0,sizeof(sNumber));
		sprintf(sNumber,"%02d", lGrade);
		str = sTable + sNumber;
			
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwIsValid,(DWORD)0,FALSE);
		
		if(!dwIsValid) 
		{
			lGradeId = lGrade;
			break;
		}
	}
}

/******************************************************************************
 Description:	Check whether tank and grade information is valid

 Returns:      	CTRL return codes
 Parameters:   	long lTankNumber - tank number
				long lGradeNumber - grade number
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			23/02/2009   15:20		Start	
******************************************************************************/
long CChangeProductInTankSetup::CheckValidTankGrade()
{
	long lRetCode = CTRL_OK;
	CString strGrade, strPureProduct, strTank;
	DWORD dwPureProduct = 0, dwPureProductValidEntry = 0;

	if(GRADE_NOT_VALID(m_lGradeNumber))
		lRetCode = CTRL_INVALID_GRADE_NUMBER;
	
	if (TANK_NOT_VALID(m_lTankNumber))
		lRetCode = CTRL_INVALID_TANK_NUMBER;

	if (!m_szProductName[0])
		lRetCode = CTRL_PARAM_NOT_FOUND;

	if (lRetCode == CTRL_OK)
	{
		strTank.Format("Tanks\\Tank%02d", m_lTankNumber);
		if ( _Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strTank) )
		{
			strGrade.Format("Grades\\Grade%02d" , m_lGradeNumber);
			if ( _Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strGrade) )
			{
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)strGrade), "PureProductA",&dwPureProduct,(DWORD)0,FALSE);
				if (dwPureProduct > 0)
				{
					strPureProduct.Format("PureProducts\\PureProduct%02d", dwPureProduct);
					_Module.m_server.m_cParam.LoadParam((LPCSTR)strPureProduct, "ValidEntry", &dwPureProductValidEntry, (DWORD)0, TRUE);
					if ( !_Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strPureProduct) )				
						lRetCode = CTRL_INVALID_GRADE_NUMBER;
				}
			}
			else
			{
				lRetCode = -1; //4.0.23.501 - TD 76330
			}
		}
		else
			lRetCode = CTRL_INVALID_TANK_NUMBER;
	}

	return lRetCode;
}

/******************************************************************************
 Description:	Create new product (grade and pure product if does not exist)

 Returns:      	CTRL return code
 Parameters:   	
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			27/08/2009   12:00		Start		4.0.23.501 - TD 76330
******************************************************************************/
long CChangeProductInTankSetup::CreateNewProduct(BSTR sXmlRec)
{
	long lRetCode = CTRL_OK;
	short nPProduct;
	BOOL bPureProductFound = FALSE;
	
	lRetCode = ParseNewProductData(sXmlRec);
	if (lRetCode == CTRL_OK) // in case of success
	{
		//4.0.24.70 - TD 111683
		// Check if pure product with the same name already exists
		nPProduct = CheckPureProductExist();

		if (nPProduct == -1)
		{
			// Find unusable pure product
			nPProduct = FindUnusablePureProduct();
			
			if (nPProduct >= 1 && nPProduct <= MAX_GRADES)
			{
				UpdateNewPureProduct(nPProduct);
				bPureProductFound = TRUE;
			}			
		}
		else
		{
			bPureProductFound = TRUE;
		}

		if (bPureProductFound)
		{
			// Create new grade
			UpdateNewGrade(nPProduct);

			//4.0.23.504 - TD 75226
			UpdateMaxValidGradeNumber();
		}

		if (!bPureProductFound)
			lRetCode = CTRL_RECORD_NOT_FOUND;
	}

	return lRetCode;
}

/******************************************************************************
 Description:	Checks if pure product with the requested name already exists,
				if exists return its id.
 Returns:      	long - pure product id
 Parameters:   	None
				
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			09/03/2010   12:00		Start		4.0.24.70 - TD 111683
 AmitH          17/03/2010   12:00		Start		4.0.24.70 - TD 113020
******************************************************************************/
short CChangeProductInTankSetup::CheckPureProductExist()
{
	short nPProduct = -1;
	CString strPureProduct;
	char szPPName[MAX_NAME_LENGTH] = {0};
	BOOL bPureProductFound = FALSE;

	for(int i = 0; (i < MAX_GRADES) && (!bPureProductFound); i++) //TD 113020 -- loop got into product 17...
	{
		strPureProduct.Format("PureProducts\\PureProduct%02d", i+1);
		if ( _Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strPureProduct)) //4.0.24.73 - TD 117927
		{
			_Module.m_server.m_cParam.LoadParam((LPCTSTR)strPureProduct, "Name", (char *)szPPName, sizeof(szPPName),"", FALSE, FALSE);

			if (!strcmp(szPPName, (char *)m_cNewGradeDetails.byGradeFullName))
			{
				bPureProductFound = TRUE;
				nPProduct = i + 1; //TD 113020
			}
		}
	}

	return nPProduct;
}


/******************************************************************************
 Description:	In case there is no available non-valid pure product to use, 
				let's find a valid product which is unusable
 Returns:      	short - return available product index
 Parameters:   	None
				
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			31/08/2009   11:00		Start		4.0.23.501 - TD 76330
******************************************************************************/
short CChangeProductInTankSetup::FindUnusablePureProduct()
{
	short nPProduct, nGrade, nPump, nNozzle;
	BOOL bPureProductFound = FALSE;
	CString strPump, strGrade, strPureProduct, strNozzle;

	// Find a place for new pure product
	for(nPProduct = 1; (nPProduct <= MAX_GRADES) && (!bPureProductFound); nPProduct++)
	{
		strPureProduct.Format("PureProducts\\PureProduct%02d", nPProduct);
		
		if (!( _Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strPureProduct) ))
		{
			bPureProductFound = TRUE;
		}
	}

	// In case there is no available non-valid pure product, let's look for a valid pure product which is not connected to any grade 
	// and then the grade which is not connected to any pump
	if (!bPureProductFound)
	{
		BOOL bGradeFound = TRUE;
		for(nPProduct = 1; (nPProduct <= MAX_GRADES) && (bGradeFound); nPProduct++)
		{
			bGradeFound = FALSE;
			strPureProduct.Format("PureProducts\\PureProduct%02d", nPProduct);

			for(nGrade = 1; (nGrade <= MAX_GRADES_99) && (!bGradeFound); nGrade++)
			{
				strGrade.Format("Grades\\Grade%02d", nGrade);
				
				if ( _Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strGrade) )
				{
					DWORD dwPureProduct;
					_Module.m_server.m_cParam.LoadParam((LPCTSTR)strGrade, "PureProductA",&dwPureProduct,(DWORD)0,FALSE);
					if ((DWORD)nPProduct == dwPureProduct)
					{
						for(nPump = 1; (nPump <= _Module.m_server.GetLastValidPump()) && (!bGradeFound); nPump++)
						{
							strPump.Format("Pumps\\Pump%02d", nPump);
							if ( _Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strPump) )
							{	
								for(nNozzle = 1; (nNozzle <= MAX_POSITIONS_PER_PUMP) && (!bGradeFound); nNozzle++)
								{
									DWORD dwGrade;
									strNozzle.Format("Pumps\\Pump%02d\\Position%d", nPump, nNozzle);
									if ( _Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strNozzle) )
									{
										_Module.m_server.m_cParam.LoadParam((LPCTSTR)strNozzle, "Grade1",&dwGrade,(DWORD)0,FALSE);
										if ((DWORD)nGrade == dwGrade)
										{
											bGradeFound = TRUE;
										}
									}
								}	
							}
						}
					}
				}
			}
		}
	}

	if (nPProduct >= 1 && nPProduct <= MAX_GRADES + 1) //4.0.25.30 - TD 126154
		nPProduct--;

	return nPProduct;		
}

/******************************************************************************
 Description:	Update all fields for the new grade

 Returns:      	void
 Parameters:   	short nPureProduct, short nGrade
				
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			01/09/2009   10:00		Start		4.0.23.501 - TD 76330
******************************************************************************/
void CChangeProductInTankSetup::UpdateNewPureProduct(short nPureProduct)
{
	CString strPureProduct;
	strPureProduct.Format("PureProducts\\PureProduct%02d", nPureProduct);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strPureProduct), "ValidEntry",(DWORD)1 ,TRUE);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strPureProduct), "Blendable",(DWORD)0 ,TRUE);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strPureProduct), "Code",(DWORD)nPureProduct ,TRUE);				
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strPureProduct), "Name", (char *)m_szProductName, TRUE);
}

/******************************************************************************
 Description:	Update all fields for the new grade

 Returns:      	void
 Parameters:   	short nPureProduct, short nGrade
				
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			01/09/2009   10:00		Start		4.0.23.501 - TD 76330
******************************************************************************/
void CChangeProductInTankSetup::UpdateNewGrade(short nPureProduct)
{
	CString strGrade;
	strGrade.Format("Grades\\Grade%02d", m_lGradeNumber);

	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "ValidEntry",(DWORD)1 ,TRUE);
	
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "FullName", (char *)m_cNewGradeDetails.byGradeFullName, TRUE);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "ShortName3", (char *)m_cNewGradeDetails.byGradeShortName3, TRUE);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "ShortName5", (char *)m_cNewGradeDetails.byGradeShortName5, TRUE);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "AdditionalCode", (char *)m_cNewGradeDetails.byAdditionalProductCode, TRUE);
	
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "Code",(DWORD)m_lGradeNumber, TRUE);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "PureProductA",(DWORD)nPureProduct, TRUE);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "PureProductB",(DWORD)0, TRUE);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "BlendRatioA",(DWORD)100, TRUE);
	
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "New_PriceCash_FullService",(DWORD)m_cNewGradeDetails.lPriceCashFull,TRUE,0,0,0,TRUE);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "New_PriceCredit_FullService",(DWORD) m_cNewGradeDetails.lPriceCreditFull,TRUE,0,0,0,TRUE);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "New_PriceCash_SelfService",(DWORD) m_cNewGradeDetails.lPriceCashSelf, TRUE,0,0,0,TRUE);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "New_PriceCredit_SelfService",(DWORD) m_cNewGradeDetails.lPriceCreditSelf,TRUE,0,0,0,TRUE);

	//4.0.26.501 - TD 116227
	_Module.m_server.m_cExtendedGradeMap.Insert(m_lGradeNumber, m_lExtGradeNumber);
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "ExtendedGradeId",(DWORD)m_lExtGradeNumber, TRUE);
}


/******************************************************************************
 Description:	Parse the data of the new product in xml
 Returns:      	CTRL return codes
 Parameters:   	BSTR sXmlRec - string in an XML format, which contains the data
				
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			27/08/2009   16:00		Start		4.0.23.501 - TD 76330
******************************************************************************/
long CChangeProductInTankSetup::ParseNewProductData(BSTR sXmlRec)
{
	long lRetCode = CTRL_RECORD_NOT_FOUND;
	BOOL bRetCode;

	char sXmlBuffer[MAX_XML_BUF_LEN] = {0};

	int iLen = Bstr2Buffer(sXmlRec, (char *)sXmlBuffer, MAX_XML_BUF_LEN);
	if(iLen > 0)
	{
		CXMLInterface  xmlDoc;
		CXMLPointers   cSetupDetailsNode, cGradeInfoNode, cPricesNode, cGradeData, cPriceNode;

		try
		{
			if (xmlDoc.InitDoc())
			{
				bRetCode = xmlDoc.LoadXmlString(sXmlBuffer);
				
				if (bRetCode)				
				{
					char sPrice[STR_LONG_VAL6];
					bRetCode = xmlDoc.SelectSingleElementNode("SetupDetails",&cSetupDetailsNode);
					if (bRetCode)
					{
						xmlDoc.SelectSingleElementNode("GradeInfo",&cGradeInfoNode, &cSetupDetailsNode);
						
						xmlDoc.SelectSingleElementNode("GradeFullName", &cGradeData, &cGradeInfoNode);
						xmlDoc.GetChildNodeValueToByteArrWithConvert(&cGradeData, m_cNewGradeDetails.byGradeFullName);

						xmlDoc.SelectSingleElementNode("GradeShortName3", &cGradeData, &cGradeInfoNode);
						xmlDoc.GetChildNodeValueToByteArrWithConvert(&cGradeData, m_cNewGradeDetails.byGradeShortName3);

						xmlDoc.SelectSingleElementNode("GradeShortName5", &cGradeData, &cGradeInfoNode);
						xmlDoc.GetChildNodeValueToByteArrWithConvert(&cGradeData, m_cNewGradeDetails.byGradeShortName5);

						xmlDoc.SelectSingleElementNode("AdditionalProductCode", &cGradeData, &cGradeInfoNode);
						xmlDoc.GetChildNodeValueToByteArrWithConvert(&cGradeData, m_cNewGradeDetails.byAdditionalProductCode);

						xmlDoc.SelectSingleElementNode("Prices", &cPricesNode, &cGradeInfoNode);

						xmlDoc.SelectSingleElementNode("PriceCashSelf", &cPriceNode, &cPricesNode);
						xmlDoc.GetChildNodeValueToByteArrWithConvert(&cPriceNode, sPrice);
						m_cNewGradeDetails.lPriceCashSelf = a2l((BYTE *)sPrice, strlen(sPrice));
						
						xmlDoc.SelectSingleElementNode("PriceCashFull", &cPriceNode, &cPricesNode);
						xmlDoc.GetChildNodeValueToByteArrWithConvert(&cPriceNode, sPrice);
						m_cNewGradeDetails.lPriceCashFull = a2l((BYTE *)sPrice, strlen(sPrice));
						
						xmlDoc.SelectSingleElementNode("PriceCreditSelf", &cPriceNode, &cPricesNode);
						xmlDoc.GetChildNodeValueToByteArrWithConvert(&cPriceNode, sPrice);
						m_cNewGradeDetails.lPriceCreditSelf = a2l((BYTE *)sPrice, strlen(sPrice));
						
						xmlDoc.SelectSingleElementNode("PriceCreditFull", &cPriceNode, &cPricesNode);
						xmlDoc.GetChildNodeValueToByteArrWithConvert(&cPriceNode, sPrice);
						m_cNewGradeDetails.lPriceCreditFull = a2l((BYTE *)sPrice, strlen(sPrice));

						lRetCode = CTRL_OK;						
					}
				}
			}
		}
		catch(...)
		{
			_LOGMSG.LogMsg("CChangeProductInTankSetup","ParseNewProductData", 0, LOG_NONE, "Unexpected Error occurred");
		}
		xmlDoc.CloseDoc();
	}
	return lRetCode;
}


/******************************************************************************
 Description:	Change the registry setup of product in tank

 Returns:      	CTRL return codes
 Parameters:   	long lTankNumber - tank number
				long lGradeNumber - grade number
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			24/02/2009   14:15		Start	
******************************************************************************/
void CChangeProductInTankSetup::ChangeSetup()
{
	CString strGrade, strPureProduct, strTank, strPump;
	DWORD dwPureProduct = 0;

	// Get grade pure product - assume that no blending
	strGrade.Format("Grades\\Grade%02d" , m_lGradeNumber);
	_Module.m_server.m_cParam.LoadParam((LPCTSTR)strGrade, "PureProductA",&dwPureProduct,(DWORD)0,FALSE);
	
	// Change pure product name
	strPureProduct.Format("PureProducts\\PureProduct%02d", dwPureProduct);
	_Module.m_server.m_cParam.SaveParam((char *)(LPCSTR)strPureProduct, "Name", (char *)m_szProductName);

	// Change pure product for requested tank
	strTank.Format("Tanks\\Tank%02d", m_lTankNumber);
	_Module.m_server.m_cParam.SaveParam((char *)(LPCSTR)strTank, "PureProduct", dwPureProduct); //4.0.23.20 - TD 77424
	
	
	// Change nozzle to tank setup under each pump which contain that specific grade (nozzle)
	for(long lPump = 1;lPump <= _Module.m_server.GetLastValidPump();lPump++)
	{
		strPump.Format("Pumps\\Pump%02d" , lPump);

		for(int nPosition = 1; nPosition<= MAX_POSITIONS_PER_PUMP; nPosition++)
		{
			char sNumber [STR_LONG_CODE] = {0};
			CString strPosition = strPump;
			DWORD dwTankNum = 0;
			
			sprintf(sNumber,"%ld",nPosition);
			strPosition += "\\Position";
			strPosition += sNumber;

			if ( _Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strPosition) )
			{
				_Module.m_server.m_cParam.LoadParam((LPCTSTR)strPosition, "TankNumber1", &dwTankNum, (DWORD)0,FALSE);
				if ((long)dwTankNum == m_lTankNumber)
				{
					_Module.m_server.m_cParam.SaveParam((char *)(LPCTSTR)strPosition, "Grade1", (DWORD)m_lGradeNumber);
				}
			}
		} 
	}
}


/******************************************************************************
 Description: Get change product in tank info in BSTR
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			26/02/2009   15:00		Start	
******************************************************************************/	
void CChangeProductInTankSetup::GetChangeProductInTankInfo(BSTR *pbstrData)
{
	CHANGE_PRODUCT_IN_TANK_SETUP cInfo;
	BYTE sBuff[sizeof(CHANGE_PRODUCT_IN_TANK_SETUP)+2];

	memset(&cInfo, ' ', sizeof(cInfo));

	l2a(m_lTankNumber, cInfo.byTankNumber, sizeof(cInfo.byTankNumber));
	l2a(m_lGradeNumber, cInfo.byGradeNumber, sizeof(cInfo.byGradeNumber));
	memcpy(cInfo.byProductName, m_szProductName, sizeof(m_szProductName));
	l2a(m_lOldGradeNumber, cInfo.byOldGradeNumber, sizeof(cInfo.byOldGradeNumber)); //4.0.23.500
	cInfo.byNewProductFlag = m_bIsNewProduct + '0'; //4.0.23.501 - TD 76330

	
	Str2StrNull((BYTE *)&cInfo,sizeof(CHANGE_PRODUCT_IN_TANK_SETUP),sBuff);	
	CComBSTR strResult(sizeof(sBuff),(LPCSTR)sBuff); 
	*pbstrData = strResult.Copy();
}



/******************************************************************************/
/*****************		CChangeProductInTankManager			*******************/
/******************************************************************************/



/******************************************************************************
 Description:	Start the process of changing product in tank

 Returns:      	CTRL return codes
 Parameters:   	BSTR bstrRec	-	buffer in XML format containing the new product setup
				BOOL bSetupOnly -	TRUE if the request is only to change the setup, 
									FALSE in case the request is for the full process
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			24/02/2009   14:15		Start	
******************************************************************************/
long CChangeProductInTankManager::StartProcess(BSTR bstrRec, BOOL bSetupOnly)
{
	long lRetCode = CTRL_OK;

	cChangeProductInTankProcess.SetFailureResult(CPIT_NO_ERROR);
	cChangeProductInTankProcess.ClearResultBufferXml();

	if (bSetupOnly == TRUE)
	{
		lRetCode = cChangeProductInTankSetup.HandleRequestSetupOnly(bstrRec);
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("CChangeProductInTankManager::StartProcess - Handle request SETUP ONLY, lRetCode=%ld", lRetCode);
			_LOGMSG.LogMsg(str);
		}
	}
	else
	{
		lRetCode = cChangeProductInTankSetup.HandleRequest(bstrRec);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("CChangeProductInTankManager::StartProcess - Handle PROCESS request, lRetCode=%ld", lRetCode);
			_LOGMSG.LogMsg(str);
		}
		
		if (lRetCode == CTRL_OK)
		{
			cChangeProductInTankProcess.SetState(CPIT_START_LOCK_FORECOURT);

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString	str;
				str.Format("Change product in tank process state=%ld (CPIT_START_LOCK_FORECOURT)", cChangeProductInTankProcess.GetState());
				_LOGMSG.LogMsg(str);
			}
		}
	}

	return lRetCode;
}

/******************************************************************************
 Description:	Clear result buffer xml

 Returns:      	
 Parameters:   	
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			04/03/2009   11:01		Start	
******************************************************************************/
void CChangeProductInTankProcess::ClearResultBufferXml()
{
	memset(&m_szResultBufferXml, 0, sizeof(m_szResultBufferXml));
}

/******************************************************************************
 Description:	Handle the process and all its states

 Returns:      	
 Parameters:   	
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   14:02		Start	
******************************************************************************/
void CChangeProductInTankManager::HandleProcess()
{
	cChangeProductInTankProcess.HandleProcess(cChangeProductInTankSetup);
}



/******************************************************************************/
/*******************	CChangeProductInTankProcess class	*******************/
/******************************************************************************/


CChangeProductInTankProcess::CChangeProductInTankProcess()
{
	InitializeCriticalSection(&m_csChangeProductInTank);
	
	m_eState = CPIT_IDLE;
	m_eFailureResult = CPIT_NO_ERROR;

	memset(&m_tWaitCloseOpenSalesTime, 0, sizeof(SYSTEMTIME));
	memset(&m_tWaitCLResponseTime, 0, sizeof(SYSTEMTIME));
	
	memset(m_szResultBufferXml, 0, sizeof(m_szResultBufferXml));
}

CChangeProductInTankProcess::~CChangeProductInTankProcess()
{
	DeleteCriticalSection(&m_csChangeProductInTank);
}


/******************************************************************************
 Description:	Set change product in tank process state
 Returns:   	None
 Parameters:    BYTE byVal - Process state (CPIT_IDLE,
											CPIT_START_LOCK_FORECOURT,
											CPIT_LOCK_FORECOURT_IN_PROGRESS,
											CPIT_FORECOURT_LOCKED,
											CPIT_WAIT_CLOSE_OPEN_TRS,
											CPIT_CHANGE_SETUP_AND_SEND_TO_CL,
											CPIT_WAIT_SEND_TO_CL,
											CPIT_WAIT_CL_RESPONSE,
											CPIT_HANDLE_CL_RESPONSE,
											CPIT_SEND_PRICE_CHANGE,
											CPIT_WAIT_UNLOCK_FORECOURT,	)
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   18:21		Start
******************************************************************************/
void CChangeProductInTankProcess::SetState(const long lState)
{

	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();


	EnterCriticalSection(&m_csChangeProductInTank);

	m_eState = (ChangeProductInTankState)lState;

	SQLPumpSrvFile.SaveServerInfo();

	LeaveCriticalSection(&m_csChangeProductInTank);	
}

/******************************************************************************
 Description:	Set change product in tank process state
 Returns:   	None
 Parameters:    const long lState - process state
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shuki 			25/02/2011   18:23		Start	//4.0.27.20 TD 147183
******************************************************************************/
void CChangeProductInTankProcess::SetProductInTankState(const long lState)
{
	EnterCriticalSection(&m_csChangeProductInTank);

	m_eState = (ChangeProductInTankState)lState;

	LeaveCriticalSection(&m_csChangeProductInTank);	
}

/******************************************************************************
 Description:	Get change product in tank process state
 Returns:   	short - process state
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   18:23		Start	
******************************************************************************/
long CChangeProductInTankProcess::GetState()
{
	return (long)m_eState;
}


/******************************************************************************
 Description:	Set change product in tank process failure result
 Returns:   	None
 Parameters:    BYTE byVal - Failure result (0 - no error, other - failure) 

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   18:25		Start	
******************************************************************************/
void CChangeProductInTankProcess::SetFailureResult(const long lError)
{
	EnterCriticalSection(&m_csChangeProductInTank);

	m_eFailureResult = (ChangeProductInTankError)lError;
	
	LeaveCriticalSection(&m_csChangeProductInTank);	
}


/******************************************************************************
 Description:	Get change product in tank process failure result
 Returns:   	long - process state
 Parameters:    None
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   18:26		Start	
******************************************************************************/
long CChangeProductInTankProcess::GetFailureResult()
{
	return (long)m_eFailureResult;

}


/******************************************************************************
 Description:	Handle all process states

 Returns:      	
 Parameters:   	

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			24/02/2009   14:15		Start	
******************************************************************************/
void CChangeProductInTankProcess::HandleProcess(CChangeProductInTankSetup &cSetup)
{
	CString strLog;

	switch(m_eState)
	{
		case CPIT_IDLE:
		{			
			break;
		}
		case CPIT_START_LOCK_FORECOURT:
		{
			HandleStartLockState();
			break;
		}
		case CPIT_LOCK_FORECOURT_IN_PROGRESS:
		{
			if(_Module.m_server.m_cServerInfo.GetServerStatus() == LOCKED)
			{
				SetState(CPIT_FORECOURT_LOCKED);

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("Change product in tank process state=%ld (CPIT_FORECOURT_LOCKED)", m_eState);
					_LOGMSG.LogMsg(str);
				}
			}
			break;
		}
		case CPIT_FORECOURT_LOCKED:
		{
			HandleForecourtLockedState();			
			break;
		}
		case CPIT_WAIT_CLOSE_OPEN_TRS:
		{ 
			HandleWaitCloseOpenSalesState();
			break;
		}
		case CPIT_CHANGE_SETUP_AND_SEND_TO_CL:
		{
			HandleChangeSetupAndSendToCLState(cSetup);
			break;
		}
		case CPIT_WAIT_SEND_TO_CL:
		{
			//do nothing
			break;
		}
		case CPIT_WAIT_CL_RESPONSE:
		{
			HandleWaitCLResponseState();
			break;
		}
		case CPIT_HANDLE_CL_RESPONSE:
		{
			HandleCLResponseState();
			break;
		}
		case CPIT_SEND_PRICE_CHANGE:
		{
			HandleSendPriceChangeState(cSetup);
			break;
		}
		case CPIT_WAIT_UNLOCK_FORECOURT:
		{
			if(_Module.m_server.m_cServerInfo.GetServerStatus() == UNLOCK)
			{
				SetState(CPIT_IDLE);

				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString	str;
					str.Format("PROCESS FINISHED. Change product in tank process state=%ld (CPIT_IDLE), error=%ld", m_eState, m_eFailureResult);
					_LOGMSG.LogMsg(str);
				}
			}
			break;
		}
		
		default:
			SetState(CPIT_IDLE);
			strLog.Format("CChangeProductInTankProcess::HandleProcess - Invalid state.");
			_LOGMSG.LogMsg(strLog);
			
			break;
	}
}

/******************************************************************************
 Description:	Handle the state of lock forecourt

 Returns:      	
 Parameters:   	

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/02/2009   20:15		Start	
******************************************************************************/
void CChangeProductInTankProcess::HandleStartLockState()
{
	long lRetVal = 0;
	_Module.m_server.SetMaintenance(MP_LOCK, 0, &lRetVal);

	// Move to IDLE and set failure result to error
	if (!lRetVal)
	{
		SetState(CPIT_LOCK_FORECOURT_IN_PROGRESS);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("Change product in tank process state = %ld (CPIT_LOCK_FORECOURT_IN_PROGRESS)", m_eState);
			_LOGMSG.LogMsg(str);
		}
	}
	else
	{
		SetState(CPIT_IDLE);
		SetFailureResult(CPIT_ERROR_LOCK_FORECOURT_FAILED);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("Change product in tank process state=%ld (CPIT_IDLE), error=%ld (CPIT_ERROR_LOCK_FORECOURT_FAILED)", m_eState, m_eFailureResult);
			_LOGMSG.LogMsg(str);
		}
	}
		
}

/******************************************************************************
 Description:	Check if open sales exist

 Returns:		BOOL - TRUE if exist, FALSE if no open sales 	
 Parameters:   	

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			26/02/2009   11:15		Start	
******************************************************************************/
BOOL CChangeProductInTankProcess::CheckOpenSales()
{
	BSTR bstrOpenTrs;
	long lFlags = 0, lTrsCount;
	BOOL bOpenSalesFound = FALSE;

	// Get open transactions
	lFlags = PAK_QUERY_UNPAID_PAK | PAK_QUERY_UNBALANCE_PREPAY | PAK_QUERY_WAIT_PREPAY | PAK_QUERY_AUTH_PREPAY;
	
	_Module.m_server.m_cCommonFunction.GetPAKQuery(&bstrOpenTrs, &lTrsCount, &lFlags);
	if (lTrsCount > 0)
	{
		bOpenSalesFound = TRUE; // found
	}
	else
	{
		// Check also if OLA qdex is empty
		PAY_AT_PUMP_INFO	info;
		CPumpTransact		trs;

		trs.SetLink(TRS_LINK_OLA_NUMBER, 0);
		memset(&info,' ',sizeof(info));  //4.0.15.11
		long lRtc = _Module.m_server.TrsLinkOperation(REC_READ_FIRST,&trs,&info);
		
		if(lRtc == OK) 
		{
			bOpenSalesFound = TRUE; // found
		}
	}

	return bOpenSalesFound;
}

/******************************************************************************
 Description: Checks if there was timeout for closing open sales
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			26/02/2009   13:50		Start	
******************************************************************************/	
BOOL CChangeProductInTankProcess::IsTimeOutWaitCloseOpenSales()
{
	long lCloseOpenSalesTime = _Module.m_server.GetCloseOpenSalesTimeout();
	return _Module.m_server.m_cCommonFunction.IsTimeOutPassed(m_tWaitCloseOpenSalesTime, lCloseOpenSalesTime);
}


/******************************************************************************
 Description: Handle forecourt locked state, check open sales
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			26/02/2009   13:50		Start	
******************************************************************************/	
void CChangeProductInTankProcess::HandleForecourtLockedState()
{
	if (CheckOpenSales() == TRUE)
	{
		ResetWaitCloseOpenSalesTime();
		InitWaitCloseOpenSalesTime();

		SetState(CPIT_WAIT_CLOSE_OPEN_TRS);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("Change product in tank process state=%ld (CPIT_WAIT_CLOSE_OPEN_TRS)", m_eState);
			_LOGMSG.LogMsg(str);
		}
	}
	else
	{
		// Change setup
		SetState(CPIT_CHANGE_SETUP_AND_SEND_TO_CL);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("Change product in tank process state=%ld (CPIT_CHANGE_SETUP_AND_SEND_TO_CL)", m_eState);
			_LOGMSG.LogMsg(str);
		}
	}
}

/******************************************************************************
 Description: Handle wait for closing open sales
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			26/02/2009   13:50		Start	
******************************************************************************/	
void CChangeProductInTankProcess::HandleWaitCloseOpenSalesState()
{
	//Start timeout to check if sales were closed
	BOOL bRetVal = FALSE;
	bRetVal = IsTimeOutWaitCloseOpenSales();

	if (bRetVal)
	{
		long lRetVal;

		//Unlock forecourt and return error to BOS
		_Module.m_server.SetMaintenance(MP_UNLOCK, 0, &lRetVal);

		SetState(CPIT_WAIT_UNLOCK_FORECOURT);
		SetFailureResult(CPIT_ERROR_OPEN_SALES_FOUND);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("Change product in tank process state=%ld (CPIT_WAIT_UNLOCK_FORECOURT), error=%ld (CPIT_ERROR_OPEN_SALES_FOUND)", m_eState, m_eFailureResult);
			_LOGMSG.LogMsg(str);
		}
	}
	else
	{
		if (CheckOpenSales() == FALSE)
		{
			SetState(CPIT_CHANGE_SETUP_AND_SEND_TO_CL);

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString	str;
				str.Format("Change product in tank process state=%ld (CPIT_CHANGE_SETUP_AND_SEND_TO_CL)", m_eState);
				_LOGMSG.LogMsg(str);
			}
		}
	}
}


/******************************************************************************
 Description: Handle change new setup state and send request to CL
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			26/02/2009   15:00		Start	
******************************************************************************/	
void CChangeProductInTankProcess::HandleChangeSetupAndSendToCLState(CChangeProductInTankSetup &cSetup)
{
	// Change setup internally
	cSetup.ChangeSetup();

	//When changing configuration, update tank level map per pump in the memory
	_Module.m_server.m_cTankLevelMap.LoadTankMapPerPump(); //4.0.23.504 - TD 73758

	// Send to change request to CL
	_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT,0,GCI_EVENT_CHANGE_PRODUCT_IN_TANK);

	ResetWaitCLResponseTime();
	InitWaitCLResponseTime();

	// Send reconfigure to IFSFTerminalSrv in case its active
	if (_Module.m_server.GetIFSFTerminalSrvInUse())
		_Module.m_server.m_cGPIEventMgr.AddReconfigure(GRADES_TABLE, 0, 0, 0);

	SetState(CPIT_WAIT_SEND_TO_CL);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString	str;
		str.Format("Change product in tank process state=%ld (CPIT_WAIT_SEND_TO_CL)", m_eState);
		_LOGMSG.LogMsg(str);
	}
}


/******************************************************************************
 Description: Handle wait Convert Layer response state
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			01/03/2009   14:00		Start	
******************************************************************************/	
void CChangeProductInTankProcess::HandleWaitCLResponseState()
{
	//Start timeout to check if CL responded
	BOOL bRetVal = FALSE;

	if (m_tWaitCLResponseTime.wDay == 0 && m_tWaitCLResponseTime.wHour == 0)
		InitWaitCLResponseTime();

	bRetVal = IsTimeOutWaitCLResponse();

	if (bRetVal)
	{
		long lRetVal;

		//Unlock forecourt and return error to BOS
		_Module.m_server.SetMaintenance(MP_UNLOCK, 0, &lRetVal);

		SetState(CPIT_WAIT_UNLOCK_FORECOURT);
		SetFailureResult(CPIT_ERROR_CL_RESPONSE_TIMEOUT);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("Change product in tank process state=%ld (CPIT_WAIT_UNLOCK_FORECOURT), error=%ld (CPIT_ERROR_CL_RESPONSE_TIMEOUT)", m_eState, m_eFailureResult);
			_LOGMSG.LogMsg(str);
		}
	}
}


/******************************************************************************
 Description: Checks if there was timeout for CL response
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			01/03/2009   14:31		Start	
******************************************************************************/	
BOOL CChangeProductInTankProcess::IsTimeOutWaitCLResponse()
{
	return _Module.m_server.m_cCommonFunction.IsTimeOutPassed(m_tWaitCLResponseTime, TIME_OUT_WAIT_CL_RESPONSE);
}


/******************************************************************************
 Description: Handle the state of CL response
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			01/03/2009   17:24		Start	
******************************************************************************/	
void CChangeProductInTankProcess::HandleCLResponseState()
{
	if (m_eFailureResult == CPIT_ERROR_CL_RESPONSE_FAILURE)
	{
		long lRetVal;

		//Unlock forecourt and return error to BOS
		_Module.m_server.SetMaintenance(MP_UNLOCK, 0, &lRetVal);

		SetState(CPIT_WAIT_UNLOCK_FORECOURT);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("Change product in tank process state=%ld (CPIT_WAIT_UNLOCK_FORECOURT), error=%ld (CPIT_ERROR_CL_RESPONSE_FAILURE)", m_eState, m_eFailureResult);
			_LOGMSG.LogMsg(str);
		}
	}
	else
	{
		SetState(CPIT_SEND_PRICE_CHANGE);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("Change product in tank process state=%ld (CPIT_SEND_PRICE_CHANGE)", m_eState);
			_LOGMSG.LogMsg(str);
		}
	}
}


/******************************************************************************
 Description: Handle start price change state of the process
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			01/03/2009   17:54		Start	
******************************************************************************/	
void CChangeProductInTankProcess::HandleSendPriceChangeState(CChangeProductInTankSetup &cSetup)
{
	long lRetVal;
	
	//set offline pumps before sending grade price to CL
	if (!SetOfflinePumpsXml(cSetup))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("Failed to save offline/online pumps");
			_LOGMSG.LogMsg(str);
		}
	}
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("Change product in tank process, result buffer XML -> %s", m_szResultBufferXml);
			_LOGMSG.LogMsg(str);
		}
	}
	
	_Module.m_server.SendGradePrice(0); //4.0.23.500 - TD 77843 //cSetup.GetGradeNumber()

	//Unlock forecourt 
	_Module.m_server.SetMaintenance(MP_UNLOCK, 0, &lRetVal);

	SetState(CPIT_WAIT_UNLOCK_FORECOURT);
	SetFailureResult(CPIT_NO_ERROR);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString	str;
		str.Format("Change product in tank process state=%ld (CPIT_WAIT_UNLOCK_FORECOURT), error=%ld (CPIT_NO_ERROR)", m_eState, m_eFailureResult);
		_LOGMSG.LogMsg(str);
	}
}


/******************************************************************************
 Description: Get the result buffer of change product in tank process
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			02/03/2009   14:22		Start	
******************************************************************************/	
void CChangeProductInTankProcess::GetResultBuffer(BSTR *pbstrBuffer)
{
	CComBSTR strResult(min(sizeof(m_szResultBufferXml), strlen(m_szResultBufferXml)+1),(LPCSTR)m_szResultBufferXml); 
	*pbstrBuffer = strResult.Copy();
}


/******************************************************************************
 Description: Prepare xml data for offline pumps. This buffer will be returned
			  to BOS if the process succeeded
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			02/03/2009   14:22		Start	
******************************************************************************/	
BOOL CChangeProductInTankProcess::SetOfflinePumpsXml(CChangeProductInTankSetup &cSetup)
{
	CXMLInterface	xmlInitDoc;
	CXMLPointers	cRootNode, cOnlineNode, cPumpNode, cChangedDataNode, cDataNode;
	char szTmp[MAX_NAME_LENGTH] = {0};
	long lLength = 0;

	CComBSTR bstrStr; 
	bstrStr.Empty();
	
	BOOL bRetCode = TRUE;
	
	bRetCode = xmlInitDoc.InitDoc();

	if(bRetCode)
	{
		xmlInitDoc.CreateElement("ChangeProductInTank", &cRootNode);
		xmlInitDoc.AppendRootElement(&cRootNode);
		
		xmlInitDoc.CreateElement("Online", &cOnlineNode);
		xmlInitDoc.AppendChild(&cOnlineNode, &cRootNode);	

		
		//Check Offline pumps before price change
		for (int p = 1; p <= _Module.m_server.GetLastValidPump(); p++) //loop on all valid pumps
		{
			char szPumpStr[7] = {0};
			sprintf(szPumpStr, "Pump%02d", p);

			if (_Module.m_server.PumpNotInWorkingState(p))
				xmlInitDoc.CreateChildElement(szPumpStr, (char*)"0", &cPumpNode);
			else
				xmlInitDoc.CreateChildElement(szPumpStr, (char*)"1", &cPumpNode);

			
			xmlInitDoc.AppendChildElement(&cPumpNode, &cOnlineNode);
		}

		// Create changed data section
		xmlInitDoc.CreateElement("ChangedData", &cChangedDataNode);
		xmlInitDoc.AppendChild(&cChangedDataNode, &cRootNode);	

		sprintf((char *)szTmp,"%ld", cSetup.GetTankId());
		//l2a(cSetup.GetTankId(), (BYTE*)szTmp, sizeof(szTmp));
		xmlInitDoc.CreateChildElement("TankId", (char*)szTmp, &cDataNode);
		xmlInitDoc.AppendChildElement(&cDataNode, &cChangedDataNode);

		long lGradeId = cSetup.GetGradeNumber();
		//4.0.26.501 - TD 116227
		//Make extended grade conversion if needed
		if ((_Module.m_server.GetMaxGrade999()))
			_Module.m_server.GetExtGradeByGrade(lGradeId, &lGradeId);


		sprintf((char *)szTmp,"%ld", lGradeId);
		//l2a(cSetup.GetGradeNumber(), (BYTE*)szTmp, sizeof(szTmp));
		xmlInitDoc.CreateChildElement("GradeId", (char*)szTmp, &cDataNode);
		xmlInitDoc.AppendChildElement(&cDataNode, &cChangedDataNode);

		xmlInitDoc.CreateChildElement("ProductName", cSetup.GetProductName(), &cDataNode);
		xmlInitDoc.AppendChildElement(&cDataNode, &cChangedDataNode);
		
		//bRetCode = xmlInitDoc.GetXmlString(m_szResultBufferXml);
		bRetCode = xmlInitDoc.GetXmlString(&bstrStr);
		if(bRetCode)
		{
			lLength = Bstr2Buffer(bstrStr.m_str, m_szResultBufferXml, bstrStr.Length()+MAX_NAME_LENGTH);
			if (lLength > 0) 
				bRetCode = TRUE;
			else
				bRetCode = FALSE;
		}
	}

	return bRetCode;
}

/******************************************************************************
 Description:	Check if the relevant pumps which are connected to the 
				specified tank are offline or not

 Returns:      	BOOL
 Parameters:   	
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			24/08/2009   15:00		Start		4.0.23.501 - TD 78465
******************************************************************************/
long CChangeProductInTankSetup::CheckOfflinePumps()
{
	long lRetCode = CTRL_OK;
	CString strGrade, strPump;
	DWORD dwPureProduct = 0;

	// Go over all pumps and check if the relevant are offline, if so return error
	for(long lPump = 1; (lPump <= _Module.m_server.GetLastValidPump()) && (!lRetCode); lPump++)
	{
		strPump.Format("Pumps\\Pump%02d" , lPump);
		if ( _Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strPump) ) //4.0.23.504 - TD 73759
		{
			for(int nPosition = 1; nPosition<= MAX_POSITIONS_PER_PUMP; nPosition++)
			{
				char sNumber [STR_LONG_CODE] = {0};
				CString strPosition = strPump;
				DWORD dwTankNum = 0;
				
				sprintf(sNumber,"%ld",nPosition);
				strPosition += "\\Position";
				strPosition += sNumber;

				if ( _Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strPosition) )
				{
					_Module.m_server.m_cParam.LoadParam((LPCTSTR)strPosition, "TankNumber1", &dwTankNum, (DWORD)0,FALSE);
					if ((long)dwTankNum == m_lTankNumber)
					{
						if ( (!_Module.m_server.m_cCommonFunction.IsPumpIdle(lPump-1)) || 
						(_Module.m_server.PumpNotInWorkingState(lPump)) )
						{
							lRetCode = CTRL_PUMP_NOT_IN_WORKING_STATE;
							break;
						}
						
					}
				}
			}
		}
	}
	return lRetCode;
}

/******************************************************************************
 Description:	After adding new grade to the system, update the max valid 
				grade number

 Returns:      	void
 Parameters:   	
 
  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			24/09/2009   10:00		Start		4.0.19.800 - TD 75226
******************************************************************************/
void CChangeProductInTankSetup::UpdateMaxValidGradeNumber()
{
	char sNumber[8];
	CString str , sTable("Grades\\Grade");
	DWORD dwTmp = 0, dwIsValid = 0;

	_Module.m_server.SetMaxValidGradeNumber(0);
	for(int i = 1; i <= MAX_GRADES_99; i++)
	{
		memset(sNumber,0,sizeof(sNumber));

		sprintf(sNumber,"%02d",i);
		str = sTable + sNumber;
			
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwTmp,(DWORD)0,FALSE);
		
		dwIsValid = dwTmp;
		if(dwIsValid) 
		{
			_Module.m_server.SetMaxValidGradeNumber(i);
		}
	}
}