// ProtectedReceipt.cpp: implementation of the ProtectedReceipt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProtectedReceipt.h"

/******************************************************************************
 Description:	Get Receipt Index (AccountNum/TrsNum) through ProtectedDataHandler
 Returns:      	Receipt Index Length
 Parameters:	sIndexOut	- OUT 
				pInfoIn		- PAP IN
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		10/08/2008   14:34			Start 4.0.20.501 TD 60236
 OmerT      18/07/2019   14:24          RFUEL-172
******************************************************************************/
long CProtectedReceipt::GetReceiptIndex(char* sIndexOut, PAY_AT_PUMP_INFO* pInfoIn, const long lPumpNumber)
{	
	long	lRetIndexLen = 0;
	
	if(pInfoIn != NULL)
	{
		PAY_AT_PUMP_INFO*	pPAPInfo;
		pPAPInfo = (PAY_AT_PUMP_INFO*)pInfoIn;

		lRetIndexLen = _Module.m_server.m_cProtectedDataHandler.GetReceiptIndex((char*)sIndexOut, pPAPInfo, lPumpNumber);
	}

	return lRetIndexLen;
}

/******************************************************************************
 Description:	Get Receipt Index (AccountNum/TrsNum) By Track2 through ProtectedDataHandler
 Returns:      	Receipt Index Length
 Parameters:	sIndexOut	- OUT 
				sTrack2In	- Track2 IN
				lIndexSize	- NOT IN USE
				lFlags		- pass to GetCardInfo()
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		10/08/2008   14:34			Start 4.0.20.501 TD 60236
******************************************************************************/
long CProtectedReceipt::GetReceiptIndex(char* sIndexOut, const long lIndexSize /*lIndexSize not in use*/, const BSTR sTrack2In, const long lFlags, const long lPumpNumber, const long lTrsNumber)	
{
	long lRetIndexLen = 0;

	if (sTrack2In != NULL)
	{
		if(lFlags & RECEIPT_TRACK2_HOLDS_TRS_NUM)
			lRetIndexLen = GetReceiptIndexByTrsNum(sIndexOut,sTrack2In); // TD 270322
		else
			lRetIndexLen = _Module.m_server.m_cProtectedDataHandler.GetReceiptIndex((char*)sIndexOut , sTrack2In, lFlags, lPumpNumber, lTrsNumber);
	}	
		
	return lRetIndexLen;
}


/******************************************************************************
 Description:	Get Receipt Index (AccountNum/TrsNum) By Track2 through ProtectedDataHandler
 Returns:      	Receipt Index Length
 Parameters:	sIndexOut	- OUT 
				sTrack2In	- Track2 IN
				lIndexSize	- NOT IN USE
				lFlags		- pass to GetCardInfo()
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		10/08/2008   14:34			Start 4.0.20.501 TD 60236
******************************************************************************/
long CProtectedReceipt::GetReceiptIndex(char* sIndexOut, const long lIndexSize /*lIndexSize not in use*/, const char* sTrack2In, const long lFlags, const long lPumpNumber)	
{	
	long		lRetIndexLen = 0;

	if (sTrack2In != NULL)
	{
		CComBSTR	bstrTmp(sTrack2In);
		BSTR		bstrTrack2 = NULL;

		bstrTrack2 = bstrTmp.Copy();
		
		lRetIndexLen = _Module.m_server.m_cProtectedDataHandler.GetReceiptIndex((char*)sIndexOut, bstrTrack2, lFlags, lPumpNumber);
			
		if(bstrTrack2 != NULL)
			SysFreeString(bstrTrack2);
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
void CProtectedReceipt::ParseCardInfo(CARD_SALE_ALL3 * cTmpCardSaleData, PAY_AT_PUMP_INFO *	PayAtPumpInfo)
{
	memcpy(&PayAtPumpInfo->CardSaleInfo.cardData.sAccountNumber,cTmpCardSaleData->CardSaleAll.data.sAccountNumber,
			sizeof(PayAtPumpInfo->CardSaleInfo.cardData.sAccountNumber));
}

void CProtectedReceipt::ParseCardInfo(CXMLInterface * cTmpCardSaleData, CXMLInterface *	PayAtPumpInfo)
{
	char sVal[MAX_FIELD_VAL];
	memset(sVal,0,sizeof(sVal));
	
	cTmpCardSaleData->GetXmlField(ATT_ACCOUNT_NUMBER,sVal);
	PayAtPumpInfo->SetXmlFieldWithSize(ATT_ACCOUNT_NUMBER,sVal,ACCOUNT_NUM_SIZE);
}


//4.0.27.501 TD 233308
void CProtectedReceipt::SetNoneOPTReceiptAccountMask(PAY_AT_PUMP_INFO* pcPayAtPumpInfo, const char* pszAccountNum)const
{
	_Module.m_server.m_cProtectedDataHandler.SetNoneOPTReceiptAccountMask(pcPayAtPumpInfo, pszAccountNum);
}