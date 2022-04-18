#include "StdAfx.h"
#include "AccountReceipt.h"


/******************************************************************************
 Description:	Get Receipt Index (AccountNum) from PAPInfo
 Returns:      	Receipt Index Length
 Parameters:	sIndexOut	- OUT 
				pInfoIn		- PAP IN
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		10/08/2008   14:34			Start 4.0.20.501 TD-60236
 OmerT      18/07/2019   14:24          RFUEL-172
******************************************************************************/
long CAccountReceipt::GetReceiptIndex(char* sIndexOut, PAY_AT_PUMP_INFO* pInfoIn, const long lPumpNumber)
{
	PAY_AT_PUMP_INFO*	pPAPInfo;
	long				lRetIndexLen = 0;

	if(pInfoIn != NULL)
	{
		pPAPInfo = (PAY_AT_PUMP_INFO*)pInfoIn;
		lRetIndexLen = sizeof(pPAPInfo->CardSaleInfo.cardData.sAccountNumber);

		memcpy(sIndexOut, pPAPInfo->CardSaleInfo.cardData.sAccountNumber, lRetIndexLen);	
		
		_LOGMSG.LogClassMsg("CAccountReceipt", "GetReceiptIndex", lPumpNumber, LOG_PUMP, "Receipt index from PAPInfo is AccountNumber!");
	}

	return lRetIndexLen;
}


/******************************************************************************
 Description:	Get Receipt Index (AccountNum) By Track2 through GetCardInfo()
 Returns:      	Receipt Index Length
 Parameters:	sIndexOut	- OUT 
				sTrack2In	- Track2 IN
				lIndexSize	- NOT IN USE
				lFlags		- pass to GetCardInfo()
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		10/08/2008   14:34			Start 4.0.20.501 TD-60236
******************************************************************************/
long CAccountReceipt::GetReceiptIndex(char* sIndexOut, const long lIndexSize, const BSTR sTrack2In, const long lFlags, const long lPumpNumber, const long lTrsNumber)	
{
	long lRetIndexLen = 0;

	if(sTrack2In != NULL)
	{		
		if ( (lFlags & RECEIPT_BY_ACCOUNT_NUM) || (lFlags & RECEIPT_INDEX_IS_RECEPT_KEY) || 
			(lFlags & RECEIPT_UNRECOGNIZED_BILL) || (lFlags & RECEIPT_CAR_REGISTRATION_INDEX))
		{
			lRetIndexLen = WideCharToMultiByte(CP_ACP, 0, sTrack2In, -1, sIndexOut, lIndexSize, NULL, NULL);
		}
		else if(lFlags & RECEIPT_TRACK2_HOLDS_TRS_NUM)
		{
			lRetIndexLen = GetReceiptIndexByTrsNum(sIndexOut,sTrack2In); // TD 270322 //4.0.30.0
		}

		else
		{
			lRetIndexLen = _Module.m_server.m_cCommonFunction.GetAccountNumByTrack2(sIndexOut, sTrack2In, lFlags);			
		}
					
		_LOGMSG.LogClassMsg("CAccountReceipt", "GetReceiptIndex", lPumpNumber, LOG_PUMP, "Receipt index from BSTR is AccountNumber!");
	}
	
	return lRetIndexLen;
}

/******************************************************************************
 Description:	Get Receipt Index (AccountNum) By Track2 through GetCardInfo()
 Returns:      	Receipt Index Length
 Parameters:	sIndexOut	- OUT 
				sTrack2In	- Track2 IN
				lIndexSize	- NOT IN USE
				lFlags		- pass to GetCardInfo()
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		10/08/2008   14:34			Start 4.0.20.501 TD-60236
******************************************************************************/
long CAccountReceipt::GetReceiptIndex(char* sIndexOut, const long lIndexSize, const char* sTrack2In, const long lFlags, const long lPumpNumber)
{	
	long		lRetIndexLen = 0;

	if(sTrack2In != NULL)
	{	
		if ( (lFlags & RECEIPT_BY_ACCOUNT_NUM) || (lFlags & RECEIPT_INDEX_IS_RECEPT_KEY) || 
			(lFlags & RECEIPT_UNRECOGNIZED_BILL) || (lFlags & RECEIPT_CAR_REGISTRATION_INDEX))
			memcpy(sIndexOut, sTrack2In, lIndexSize);			
		else
		{
			CComBSTR	bstrTmp(sTrack2In);
			BSTR		bstrTrack2 = NULL;

			bstrTrack2 = bstrTmp.Copy();

			lRetIndexLen = _Module.m_server.m_cCommonFunction.GetAccountNumByTrack2(sIndexOut, bstrTrack2, lFlags);

			if (bstrTrack2 != NULL)
				SysFreeString(bstrTrack2);
		}
		
		_LOGMSG.LogClassMsg("CAccountReceipt", "GetReceiptIndex", lPumpNumber, LOG_PUMP, "Receipt index is AccountNumber!");	
	}

	return lRetIndexLen;
}



/******************************************************************************
 Description:After we call GetCardInfo we have to check if we have account number
			or we have to work on attend
 Returns:      	
 Parameters:cTmpCardSaleData - the info that return from GetCardInfo
			PayAtPumpInfo out param that will hold the currect account and track2	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			30/05/2005   14:34		Start
******************************************************************************/
void CAccountReceipt::ParseCardInfo(CARD_SALE_ALL3 * cTmpCardSaleData, PAY_AT_PUMP_INFO *	PayAtPumpInfo)
{
	memcpy(&PayAtPumpInfo->CardSaleInfo.cardData.sAccountNumber,cTmpCardSaleData->CardSaleAll.data.sAccountNumber,
			sizeof(PayAtPumpInfo->CardSaleInfo.cardData.sAccountNumber));
}

void CAccountReceipt::ParseCardInfo(CXMLInterface * cTmpCardSaleData, CXMLInterface *	PayAtPumpInfo)
{
	char sVal[MAX_FIELD_VAL];
	memset(sVal,0,sizeof(sVal));
	
	cTmpCardSaleData->GetXmlField(ATT_ACCOUNT_NUMBER,sVal);
	PayAtPumpInfo->SetXmlFieldWithSize(ATT_ACCOUNT_NUMBER,sVal,ACCOUNT_NUM_SIZE);
}
