// OLAPositiveIFSF.cpp: implementation of the COLAPositiveIFSF class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLA.h"
#include "OLAPositiveIFSF.h"
#include <math.h>
#include <afx.h>
#include "OLAEvents.h" 
extern 	COLAEvents *_pOLAEvents;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLAPositiveIFSF::COLAPositiveIFSF()
{
	 SetIFSFProtocoll(TRUE);
	 SetPreAuthIFSF(TRUE);
}

COLAPositiveIFSF::~COLAPositiveIFSF()
{

}
/////////////////////////////////////////////////////////
//OLA initialization.
/////////////////////////////////////////////////////////
void COLAPositiveIFSF::Init()
{

	HRESULT hRes = 0;

	BOOL bBasicInterfaceCreated = FALSE;

	if (m_bOLAStopped == FALSE)
	{
		EnterCriticalSection(&m_csOLASrv);

		if(!m_bValid && m_byConnectionAttempts)
		{
			try
			{
				hRes = m_pSrvPtr.CreateInstance(CLSID_Services);
							
				if(SUCCEEDED(hRes))
				{
					bBasicInterfaceCreated = TRUE;
					hRes = m_pCallBack.CreateInstance(CLSID_CallBack);
								
					if(FAILED(hRes))
						_LOGMSG.LogOLAMsg(0, 0, 0, "OLAInUse = 8 >>> ICallBack NOT FOUND !!!", 0);
				
					else
					{
						long  lPumpNumber = 0;						
						_pOLAEvents = new COLAEvents;

						// Advise the server of the sink.
						IUnknownPtr pUnk = m_pCallBack;
						hRes = _pOLAEvents->DispEventAdvise(pUnk);
						m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber);
					}		
				}
						

				if(FAILED(hRes))
				{
					//Eyal 4.0.0.28 Start
					//If we failed to create a basic connection to OLA, exit
					if(!bBasicInterfaceCreated)
					{	
						_Module.m_server.m_enmOlaThreadStatus = OLA_SERVER_OFF;
						_com_issue_error(hRes);
					}

					else  //At least a basic connection exists
					{
						
						_LOGMSG.LogMsg("OLA with events: COM failure, OLA server will have no event support" , LOG_LEVEL_0);
						LoadRegistryParams();
						LaunchOLA();
											
						ResetConnectionAttempts();						

					}
					
				}

				else
				{
					
					LoadRegistryParams();
					LaunchOLA();
					
					ResetConnectionAttempts();					
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
		}
		
		LeaveCriticalSection(&m_csOLASrv);
	}
/*
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
						long  lPumpNumber = 0;
						LoadRegistryParams();
						LaunchOLA();

						//4.0.2.37
						ResetConnectionAttempts();
						m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber);
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
*/
}

/******************************************************************************
 Description:	Add item list of product that sold on specific Pump. 	
 Returns:      	None
 Parameters:   	long lPumpNumber  - 
				long lOffset,   - pointer to position for next item to add
				char *pOutBuffer  - OLA string buffer  to add the item lits
				long lOLAPumpServiceMode - price level
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			22/4/2004   18:07		Start
******************************************************************************/
void COLAPositiveIFSF::AddPreAuthInformation(long lPumpNumber ,long & lOffset,char *pOutBuffer , long & lOLAPumpServiceMode, long & lGradeRestrict)
{
	PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);
	long lCount = cPumpItems.lCounter;
	long  lNumOfItems=0;//4.0.12.501	fix this function TD 14868,22629
	char sFiledName[MAX_FIELD_NAME]; 

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
					if ((!lGradeRestrict) ||    //4.0.7.500
						(cPumpItems.m_cItemSoldInfo[i].lGrade == lGradeRestrict))
					{
						sprintf(sFiledName,"ItemCode%d[S]",lNumOfItems);
						lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%03ld,",		_Module.m_server.GetGradeCode((short)cPumpItems.m_cItemSoldInfo[i].lGrade));			
						
						sprintf(sFiledName,"ItemPrice%d[F]",lNumOfItems);

						if (lOLAPumpServiceMode  == FULL_SERVICE_MODE)
							lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%ld.%03ld,",cPumpItems.m_cItemSoldInfo[i].lPriceCreditFullService/1000,cPumpItems.m_cItemSoldInfo[i].lPriceCreditFullService%1000);				
						else
							lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%ld.%03ld,",cPumpItems.m_cItemSoldInfo[i].lPriceCreditSelfService/1000,cPumpItems.m_cItemSoldInfo[i].lPriceCreditSelfService%1000);

						sprintf(sFiledName,"ItemIsFuel%d[B]",lNumOfItems);
						lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,(BYTE *)"%s=%s,",(BYTE *)"Y",1);
						lNumOfItems++;
					}

				}
				else
				{
					sprintf(sFiledName,"ItemIsFuel%d[B]",lNumOfItems);
					lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,(BYTE *)"%s=%s,",(BYTE *)"N",1);

					sprintf(sFiledName,"ItemCode%d[S]",lNumOfItems);
					lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%03ld,",	cPumpItems.m_cItemSoldInfo[i].lGrade);			
					lNumOfItems++;

				}	

			}
		}	
		
		lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCount[I]",	(BYTE *)"%s=%03ld," ,lNumOfItems);
	}	
}

/******************************************************************************
 Description:	check for approved item and discount witch got from OLA 	
 Returns:      	None
 Parameters:   	long		lPumpNumber - Pump number.   
				CString		pInStr      - OLA open string  
				PAY_AT_PUMP_INFO pInfo  - PumpSrv internal buffer

 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			25/4/2004   8:53		Start
******************************************************************************/
/*
void COLAPositiveIFSF::ParseItemProduct(long &lPumpNumber, CString * pInStr, PAY_AT_PUMP_INFO *pInfo)
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
				lNACSCode = a2l((BYTE *)pParam,iLen);
				// convert NACS item to index
				if (ConvertNACSItemsToIndex(lPumpNumber,lNACSCode,lIndex))
				{			
					lVal = 0;

					// Grade product number
					if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
					{
						lGradeRestriction += (long)pow(2,(cPumpItems.m_cItemSoldInfo[lIndex].lGrade -1));	
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
								pInfo->CardSaleExtraData2.sDiscountPerGrade[(cPumpItems.m_cItemSoldInfo[lIndex].lGrade) -1 ].sDiscount , sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[(cPumpItems.m_cItemSoldInfo[lIndex].lGrade) - 1].sDiscount));							
							
						}
						else  // Carwash item for now there is only one 
						{
							l2a(lVal,
								pInfo->CardSaleExtraData3.sCarWashDiscount,min(sizeof(pInfo->CardSaleExtraData3.sCarWashDiscount) , iLen));															
						
						}
					} 

/*					
					sprintf(sFiledName,"ItemAmountLimit%d[F]",i);

					bFound = GetParam(pInStr,pInStr->Find("sFiledName"),&pParam,&iLen);

					if ((bFound) && (iLen))
					{														
						double x;
						x = atof(pParam);
						lVal = RoundDouble(x , 100);

						if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
						{
							l2a(lVal,
								pInfo->CardSaleExtraData3.cGradeLimitArray[(cPumpItems.m_cItemSoldInfo[lIndex].lGrade -1)].sGradeRestrictionLimit , sizeof(pInfo->CardSaleExtraData3.cGradeLimitArray[(cPumpItems.m_cItemSoldInfo[lIndex].lGrade) -1].sGradeRestrictionLimit));														

						}
						else  // Carwash item for now there is only one 
						{
							l2a(lVal,
								pInfo->CardSaleExtraData3.sCarWashLimit,min(sizeof(pInfo->CardSaleExtraData3.sCarWashLimit) , iLen));															

							lCarWashRestriction = 1;
						}
					}
*/
/*
					if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
					{
						if(LOG_BASIC_FLOW_CONTROL)
						{
							sMsg.Format("Parse OLA IFSF data pump%ld,code=%d, discount=%.5s" ,lPumpNumber,  lNACSCode, pInfo->CardSaleExtraData2.sDiscountPerGrade[cPumpItems.m_cItemSoldInfo[lIndex].lGrade -1].sDiscount);
							_LOGMSG.LogMsg(sMsg);
						}
					}
					
				}
				else
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{
						sMsg.Format("Item Not found in lits of IFSF pump%ld,code=%d" ,lPumpNumber,  lNACSCode);
						_LOGMSG.LogMsg(sMsg);
					}
				}
				 
			}
		}

		// update grade restriction map
		l2a(lGradeRestriction , pInfo->CardSaleInfo.cardData.sGradeRestriction , sizeof(pInfo->CardSaleInfo.cardData.sGradeRestriction));
		// update carwash restriction map
		l2a(lCarWashRestriction ,&pInfo->CardSaleExtraData3.sCarwashRestriction, sizeof(pInfo->CardSaleExtraData3.sCarwashRestriction));

		if(LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("OLA restrict Pump=%ld with the following details Grade map=%ld,Car wash=%ld",lPumpNumber, lGradeRestriction, lCarWashRestriction);
			_LOGMSG.LogMsg(sMsg);
		}		
	}

}
*/
/******************************************************************************
 Description:	Find the NACS code in the item lits and return the item index 	
 Returns:      	TRUE/FALSE  ; - TURE - found the nacs code in list
 Parameters:   	long lPumpNumber,
				long lNACSCode 
				long lIndex -  NACS code location in product array.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			22/4/2004   18:51		Start
******************************************************************************/
/*
BOOL COLAPositiveIFSF::ConvertNACSItemsToIndex(long & lPumpNumber,long & lNACSCode, long &lIndex)
{
	PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);
	BOOL bRet = FALSE;

	for (int i=0 ; (i < MAX_ITEMS) && ( i < cPumpItems.lCounter); i++)
	{
		if (cPumpItems.m_cItemSoldInfo[i].lNACSCode == lNACSCode)
		{
			lIndex = i;
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}
*/
/*
void COLAPositiveIFSF::LoadAllProductSoldOnPump(long lPumpNumber)
{	
	m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber);
}
*/

/******************************************************************************
 Description:	Performs a Login/Logout to the pump in case it's needed 	
 Returns:      	
 Parameters:   	PumpLoginOp ePumpLoginOp,long lPumpNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi			28/4/2004   18:07		Start
******************************************************************************/

long COLAPositiveIFSF::Login(PumpLoginOp ePumpLoginOp, long lPumpNumber)
{
	long lRetval;
	CString strOlaMsg;
	CString strLog;
	
	lRetval = 0;
	//Building the CString containing the data intended for the OLA Login/Logout, and
	//then building the BSTR which will be sent to the OLA from it 
	strOlaMsg.Format("PumpNumber[I]=%ld,TerminalID[S]=%ld,", lPumpNumber, PUMPSRV_REQUEST);  

	CComBSTR strData(strOlaMsg); 

	switch(ePumpLoginOp)
	{
		case PUMP_LOGIN: //Logging in
			try
			{
				lRetval = m_pSrvPtr->OpenShift(&strData);
			}
			catch (_com_error & e)
			{
				Error(e);
			}

			catch (...)
			{
				Error();
			}
			if(0 == lRetval)
			{
				strLog.Format("COLAPositiveIFSF::Login: Performed Login successfully for pump %d" ,lPumpNumber);
				_LOGMSG.LogMsg(strLog);
			}
			break;

		case PUMP_LOGOUT: //Logging out
			// After talk with yossi b.  he said that no need to send the closeshift.
			//lRetval = m_pSrvPtr->CloseShift(&strData);   //4.0.10.14  21/08/05 
/*			
			try
			{
				lRetval = m_pSrvPtr->CloseShift(&strData);
			}
			catch (_com_error & e)
			{
				Error(e);
			}

			catch (...)
			{
				Error();
			}

			if(0 == lRetval)
			{
				strLog.Format("COLAPositiveIFSF::Login: Performed Logout successfully for pump %d", lPumpNumber);
				_LOGMSG.LogMsg(strLog);
			}
			*/
			break;
		default:
			//Invalid request
			strLog.Format("COLAPositiveIFSF::Login: Login request invalid");
			_LOGMSG.LogMsg(strLog);
			lRetval = -1;
	}
	return(lRetval);
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
void COLAPositiveIFSF::MatchSentLoyaltyCardInfoToOLA(PAY_AT_PUMP_INFO &pInOutPAPBuffer)  
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