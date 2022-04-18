#include "stdafx.h"
#include "Track2Receipt.h"


long CTrack2Receipt::GetReceiptIndex(char* sIndexOut, PAY_AT_PUMP_INFO* pInfoIn, const long lPumpNumber) //RFUEL - 172
{
	PAY_AT_PUMP_INFO*	pPAPInfo;
	long				lRetIndexLen = 0;
	

	if(pInfoIn != NULL)
	{
		pPAPInfo = (PAY_AT_PUMP_INFO *)pInfoIn;
		lRetIndexLen = sizeof(pPAPInfo->CardSaleInfo.cardData.sTrack2Data);

		memcpy(sIndexOut, pPAPInfo->CardSaleInfo.cardData.sTrack2Data, lRetIndexLen);	
		
		_LOGMSG.LogClassMsg("CTrack2Receipt", "GetReceiptIndex", lPumpNumber, LOG_PUMP, "Receipt index from PAPInfo is Track2Data!");
	}

	return lRetIndexLen;
}


long CTrack2Receipt::GetReceiptIndex(char* sIndexOut, const long lIndexSize, const BSTR sTrack2In, const long lFlags, const long lPumpNumber, const long lTrsNumber)	
{
	long lRetIndexLen = 0;

	if(sTrack2In != NULL)
	{
		if(lFlags & RECEIPT_TRACK2_HOLDS_TRS_NUM)
		{
			lRetIndexLen = GetReceiptIndexByTrsNum(sIndexOut,sTrack2In); // TD 270322 //4.0.30.0
		}
		else
		{
			lRetIndexLen = Bstr2Buffer(sTrack2In, sIndexOut, lIndexSize);
		}
		
		_LOGMSG.LogClassMsg("CTrack2Receipt", "GetReceiptIndex", lPumpNumber, LOG_PUMP, "Receipt index from BSTR is Track2Data!");
	}
	
	return lRetIndexLen;
}

long CTrack2Receipt::GetReceiptIndex(char* sIndexOut, const long lIndexSize, const char* sTrack2In, const long lFlags, const long lPumpNumber)
{
	long lRetIndexLen = lIndexSize;
	
	if(sTrack2In != NULL)
	{
		memcpy(sIndexOut, sTrack2In, lIndexSize);
			
		_LOGMSG.LogClassMsg("CTrack2Receipt", "GetReceiptIndex", lPumpNumber, LOG_PUMP, "Receipt index is Track2Data!");
	}
	
	return lRetIndexLen;
}


void CTrack2Receipt::ParseCardInfo(CARD_SALE_ALL3* cTmpCardSaleData, PAY_AT_PUMP_INFO* PayAtPumpInfo)
{
	memcpy(PayAtPumpInfo->CardSaleInfo.cardData.sTrack2Data,cTmpCardSaleData->CardSaleAll.data.sTrack2Data,
		sizeof(PayAtPumpInfo->CardSaleInfo.cardData.sTrack2Data));
}

void CTrack2Receipt::ParseCardInfo(CXMLInterface* cTmpCardSaleData, CXMLInterface* PayAtPumpInfo)
{
	char sVal[MAX_FIELD_VAL];

	memset(sVal,' ',sizeof(sVal));
	
	cTmpCardSaleData->GetXmlField(ATT_TRACK2_DATA,sVal);
	PayAtPumpInfo->SetXmlFieldWithSize(ATT_TRACK2_DATA,sVal,TRACK_2_DATA_SIZE);
}


