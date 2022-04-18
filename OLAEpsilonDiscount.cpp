#include "StdAfx.h"
#include "OLAEpsilonDiscount.h"
#include <iomanip>
#include "CStrFormat.h"
#include "FixedPointValue.h"
#include "OLAParam.h"
#include "OLAPumpServiceMode.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



void COLAEpsilonDiscount::Init() {
	base_class::Init();
#if USE_SIMLINK
	LogStream LogMsg;
	LogMsg << __FUNCTION__ << "Initializing with SimLink" << ends;
	_LOGMSG.LogMsg(LogMsg, LOG_LEVEL_0);
#else
	LogStream LogMsg;
	LogMsg << __FUNCTION__ << "Initializing with Epsilon" << ends;
	_LOGMSG.LogMsg(LogMsg);
#endif
}




// size of pOutBuffer is MAX_OLA_BUFFER
void COLAEpsilonDiscount::BuildGetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, char *pOutBuffer)
{
	long lTransNumber = 0;
	long lOffset = 0;	

	PAY_AT_PUMP_INFO *pInfo = reinterpret_cast<PAY_AT_PUMP_INFO*>(pInBuffer);
	CARD_SALE_DATA			*pCardData = &(pInfo->CardSaleInfo.cardData);
	CARD_SALE_EXTRA_DATA	*pCardDataExtra = &(pInfo->CardSaleInfo.extraData);
	CARD_SALE_EXTRA_DATA2_  *pCardDataExtra2 = &(pInfo->CardSaleExtraData2);

	OLAPumpServiceMode	oPumpServiceMode;

	if (!PUMP_NOT_VALID(lPumpNumber))
	{
		CPump& Pump = _Module.m_server.GetPump(lPumpNumber);
		CPumpStatus cPumpStatus = Pump.GetStat();
		long lPumpServiceMode = cPumpStatus.GetServiceMode();

		// Set Pump Service Mode
		//  Instead of passing long, we pass reference to this object
		oPumpServiceMode.SetServiceMode(lPumpServiceMode);

		CPumpTransact trs;
		if (!Pump.GetInProgressWaitProgressTrs(&trs))
		{
			lTransNumber = trs.m_lNumber;

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				LogStream LogMsg;
				LogMsg << __FUNCTION__
					<< " - Pump " << lPumpNumber
					<< " - lTransNumber = " << lTransNumber
					<< ends;
				_LOGMSG.LogMsg(LogMsg);
			}
		}
	}

	if (pCardData->sEntryMethod == 'B')  //RFUEL-2399
	{
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"AccountNumber[S]", (BYTE *)"%s=%.21s,", pCardData->sAccountNumber, sizeof(pCardData->sAccountNumber));
	}
	else
	{
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"Track2[S]", (BYTE *)"%s=%.37s,", pCardData->sClubCardTrack2Data, sizeof(pCardData->sClubCardTrack2Data));

		// Hard coded for testing
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"SendToPES[B]", (BYTE *)"%s=%.1c,", 'Y');
	}

	
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"DeviceNumber[S]", (BYTE *)"%s=%ld,", lPumpNumber);
	
	// If Track2 provided, backend server developer requested we do not send EntryMethod
	const bool bIsEmptyTrack2 = ChrAll(pCardData->sClubCardTrack2Data, sizeof(pCardData->sClubCardTrack2Data), ' ') == TRUE ? true : false;
	if (!bIsEmptyTrack2)
	{
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"EntryMethod[S]", (BYTE *)"%s=%.1s,", &(pCardData->sEntryMethod), sizeof(pCardData->sEntryMethod));		
	}

	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"StoreNumber[S]", (BYTE *)"%s=%.4ld,", atol(_Module.m_server.GetSiteID())); //3.2.0.69
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TerminalId[S]", (BYTE *)"%s=%04ld,", (long)SERVER_OLA_POS_NUMBER);
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"OriginTrans[S]", (BYTE *)"%s=%.1s,", &(pCardDataExtra2->sOriginTrans), sizeof(pCardDataExtra2->sOriginTrans));

	// if LoyaltyEntryMethod is not set
	if (pInfo->CardSaleExtraData4.sLoyaltyEntryMethod == ' ') {		
		long lMediaMasterType = _Module.m_server.m_cFlowControlManager.GetMediaMasterType(lPumpNumber);
		if (lMediaMasterType == CARD_TYPE_LOYALTY) {
			pInfo->CardSaleExtraData4.sLoyaltyEntryMethod = 'S';
		}
		else if (lMediaMasterType == CARD_TYPE_LOYALTY_AND_CREDIT) {
			pInfo->CardSaleExtraData4.sLoyaltyEntryMethod = 'B';
		}
	}

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


	//  bypass the existing call to pass pInfo
	//AddedPreAuthInformation(lPumpNumber, lOffset, pOutBuffer, lOLAPumpServiceMode, lProdCode);
	AddPreAuthInformation(lPumpNumber, pInfo, lOffset, pOutBuffer, 
		oPumpServiceMode, lProdCode);



	if (LOG_DETAIL_FLOW_CONTROL)
	{
		const int iEncrptSize = sizeof pCardDataExtra->sLoyalId - 4; // //RFUEL-1367
		CString str;
		str.Format("%s - Pump %02ld - lTransNumber = %ld, lSeqNumber %ld, sTranType = %c, ClubCardNumber = XXXXXXXXXXXXXXXX%.4s, EntryMethod = %c",
			__FUNCTION__,
			lPumpNumber,
			lTransNumber,
			lSeqNumber,
			pCardData->sTranType,
			pCardDataExtra->sLoyalId + iEncrptSize,
			pCardData->sEntryMethod);
		_LOGMSG.LogMsg(str);
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		LogStream LogMsg;
		DebugDumpDiscountMatrix(*pInfo, LogMsg);
		LogMsg << ends;

		_LOGMSG.LogMsg(LogMsg);
	}

	//{
	//	//Debugging line - reads back all parameters into an object
	//	//keep this commented out
		//const long outputLen = strnlen(pOutBuffer, MAX_OLA_BUFFER);
		//OLAParamReader OLAParamValidator(pOutBuffer, outputLen);		
		//LogStream LogMsg;
		//LogMsg << "GET_CUST FINAL BUFFER : " 
		//	<< OLAParamValidator << ends;
		//_LOGMSG.LogMsg(LogMsg, LOG_LEVEL_1);
	//}
}

//
//  Pulls price information from ItemSoldInfo structure
//	based on ServiceMode (Full/Self) and PriceLevel (Cash/Credit)
//
long COLAEpsilonDiscount::GetGradePrice(const ItemSoldInfo& SoldInfo, const OLAPumpServiceMode& oPumpServiceMode, long PriceLevel) const
{
	long lPrice = 0;

	if (oPumpServiceMode.IsFullService()) {
		lPrice = (PriceLevel == PRICE_LEVEL_CREDIT) ?
			SoldInfo.lPriceCreditFullService :
			SoldInfo.lPriceCashFullService;
	}
	else if (oPumpServiceMode.IsSelfService()) {
		lPrice = (PriceLevel == PRICE_LEVEL_CREDIT) ?
			SoldInfo.lPriceCreditSelfService :
			SoldInfo.lPriceCashSelfService;
	}
	else {
		// Debug Dump, uncomment for debugging 
		LogStream szLogmsg;
		szLogmsg << __FUNCTION__
			<< "- Unable to find Price"
			<< " Grade=" << SoldInfo.lGrade
			<< " ServiceMode=" << (oPumpServiceMode.IsFullService() ? "FULL" : "SELF")
			<< " PriceLevel=" << PriceLevel
			<< std::ends;
		_LOGMSG.LogMsg(szLogmsg, LOG_LEVEL_1);
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		// Debug log, 
		// writes out pricing structure
		LogStream szLogmsg;
		szLogmsg << __FUNCTION__
			<< SoldInfo
			<< " OLAServiceMode=" << (oPumpServiceMode.IsFullService() ? "FULL" : "SELF")
			<< " PriceLevel=" << ((PriceLevel == PRICE_LEVEL_CREDIT) ? "CREDIT" : "CASH")
			<< " Selected Price=" << lPrice
			<< std::ends;
		_LOGMSG.LogMsg(szLogmsg);
	}

	return lPrice;
}


long COLAEpsilonDiscount::GetGradePrice(short nGrade, long& lPriceCash, long& lPriceCredit,
	const OLAPumpServiceMode& oPumpServiceMode) const
{
	lPriceCredit = 0;
	lPriceCash = 0;

	if (GRADE_NOT_VALID(nGrade))
		return 1;

	if (nGrade > 0)
	{
		DWORD dwTmp;
		CString sGradeTable;

		sGradeTable.Format("Grades\\Grade%02d", nGrade);
		//4.0.8.503

		if (oPumpServiceMode.IsFullService())
		{
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCredit_FullService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			lPriceCredit = (long)dwTmp;
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCash_FullService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			lPriceCash = (long)dwTmp;
		}
		else if (oPumpServiceMode.IsSelfService())
		{
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCredit_SelfService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			lPriceCredit = (long)dwTmp;
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCash_SelfService", &dwTmp, (DWORD)0, FALSE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			lPriceCash = (long)dwTmp;
		}
		//4.0.8.503
	}

	return 0;

}

long COLAEpsilonDiscount::GetGradePrice(const PumpItems& cPumpItems, 
	long nGrade, 
	const OLAPumpServiceMode& oPumpServiceMode, long nPumpPriceLevel) const {
	long lPrice = 0l;

	// walk through all ItemSoldInfo
	for (int ItemPos = 0; ItemPos < cPumpItems.lCounter; ItemPos++) {
		const ItemSoldInfo& SoldInfo = cPumpItems.m_cItemSoldInfo[ItemPos];

		//{ 
		//	// Debugging log 
		//	// Dump all SoldInfo in memory as we process
		//	LogStream LogMsg;
		//	LogMsg << __FUNCTION__ << ""
		//		<< "ItemSoldInfoDump - "
		//		<< " ItemPos=" << ItemPos
		//		<< SoldInfo
		//		<< ends;
		//	_LOGMSG.LogMsg(LogMsg);
		//}

		// Find item by Grade
		if (SoldInfo.lGrade == nGrade) {
			
			lPrice = GetGradePrice(SoldInfo, oPumpServiceMode, nPumpPriceLevel);
			break;
		}
	}

	return lPrice;
}


// Refactored from AddedPreAuthInformation_
void COLAEpsilonDiscount::AddedPreAuthProcessFuelItem(
	long lPumpNumber, 
	PAY_AT_PUMP_INFO *pInfo, 
	long &lOffset, 
	char *pOutBuffer, 
	const OLAPumpServiceMode& oPumpServiceMode,
	const long & lGradeRestrict,
	const long& lGrade, 
	const ItemSoldInfo& SoldInfo,  
	long& lNumOfItems)
{
	CARD_SALE_DATA			*pCardData = &(pInfo->CardSaleInfo.cardData);	
	CARD_SALE_EXTRA_DATA2_  *pCardDataExtra2 = &(pInfo->CardSaleExtraData2);

	char	sFiledName[MAX_FIELD_NAME] = {};

	auto& Pump = _Module.m_server.GetPump(lPumpNumber);

	// Print original unit price 
	if (SoldInfo.bFuelItem)
	{
		if ((!lGradeRestrict) ||    //4.0.7.500
			(lGrade == lGradeRestrict))
		{
			// Debugging log 
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				LogStream LogMsg;
				LogMsg << __FUNCTION__
					<< " ItemSoldInfo - "
					<< SoldInfo
					<< ends;
				_LOGMSG.LogMsg(LogMsg);
			}

			sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemCode%d[S]", lNumOfItems);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%03ld,", _Module.m_server.GetGradeCode((short)SoldInfo.lGrade));

			long ItemPrice = 0;
			long ItemPricePES = 0;

			long nPumpLevel = (short)(Pump.GetPriceLevelDefault());
			if (pCardDataExtra2->sPriceLevel != ' ') {
				nPumpLevel = a2l( (const unsigned char*)pCardDataExtra2->sPriceLevel, sizeof(pCardDataExtra2->sPriceLevel));
			}

			ItemPricePES = GetGradePrice(SoldInfo, oPumpServiceMode, nPumpLevel);
			mathext::Fixed_Point_Value FixedItemPricePES(ItemPricePES);

			sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPricePES%d[F]", lNumOfItems);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%ld.%03ld,", 
				FixedItemPricePES.GetDollars(), 
				FixedItemPricePES.GetCents());

			//// Debugging log right after writing ItemPricePESX
			//if (LOG_DETAIL_FLOW_CONTROL)
			//{
			//	LogStream LogMsg;
			//	LogMsg << __FUNCTION__
			//		<< " - Pump " << std::setfill('0') << std::setw(2) << lPumpNumber
			//		<< " FIELD: " << sFiledName
			//		<< " VALUE: " << FixedItemPricePES
			//		<< ends;
			//	_LOGMSG.LogMsg(LogMsg);
			//}

			//RFUEL - 2817
			// Get payment a.k.a Tender Discount
			const long lGRDiscount = _Module.m_server.GetPaymentDiscount(*pInfo);
			ItemPrice = ItemPricePES - lGRDiscount;
			sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPrice%d[F]", lNumOfItems);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *) "%s=%ld.%03ld,", ItemPrice / 1000, ItemPrice % 1000);

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				LogStream LogMsg;
				LogMsg << __FUNCTION__
					<< " - Pump " << std::setfill('0') << std::setw(2) << lPumpNumber
					<< " PaymentDiscount=" << lGRDiscount
					<< " ItemPrice=" << ItemPrice
					<< " ItemPricePES=" << ItemPricePES << ends;
				_LOGMSG.LogMsg(LogMsg);
			}			
			long	lPriceCash = 0;
			long	lPriceCredit = 0;
			
			//4.0.11.502
			//Attached is the new fields that will be send to OLA in order to report low price & high price.   
			GetGradePrice((short)lGrade, lPriceCash, lPriceCredit, oPumpServiceMode);//4.0.11.122


			const bool bAnyDiscount = (lGRDiscount > 0L);

			// RFUEL-2817, remove discount from the prices
			if (bAnyDiscount) {
				// calculate total discount, this assumes Discount type is DISCOUNT_PG
				const long lTotalDiscount = lGRDiscount;


				// take discount out of the prices
				lPriceCash = lPriceCash - lTotalDiscount;
				lPriceCredit = lPriceCredit - lTotalDiscount;

				if (LOG_DETAIL_FLOW_CONTROL)
				{
					// Debug string
					LogStream LogMsg;
					LogMsg << __FUNCTION__
						<< " - Pump " << std::setfill('0') << std::setw(2) << lPumpNumber
						<< " - Discount = " << lTotalDiscount
						<< " - Adjusted Cash Price = " << lPriceCash
						<< " - Adjusted Credit Price = " << lPriceCredit
						<< ends;
					_LOGMSG.LogMsg(LogMsg);
				}
			}


			sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPriceLow%d[S]", lNumOfItems);//4.0.11.50
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *) "%s=%ld.%03ld,", lPriceCash / 1000, lPriceCash % 1000);//4.0.11.122
			sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPriceHigh%d[S]", lNumOfItems);//4.0.11.50
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%ld.%03ld,", lPriceCredit / 1000, lPriceCredit % 1000);//4.0.11.122


			sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemIsFuel%d[B]", lNumOfItems);
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *) "Y", 1);

			sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemType%d[S]", lNumOfItems);
			// required only for pilot   //4.0.9999.0
			if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetDieselPumpFlags())
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *) "C", 1);
			else
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *) "P", 1);

			lNumOfItems++;  //4.0.11.504
		}
	}
	else //if (!cPumpItems.m_cItemSoldInfo[i].bFuelItem)
	{
		sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemIsFuel%d[B]", lNumOfItems);
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%s,", (BYTE *)"N", 1);

		sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemCode%d[S]", lNumOfItems);
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%03ld,", SoldInfo.lGrade);

		lNumOfItems++;  //4.0.11.504
	}
}


// This function gets called from GET_ for purpose of Authentication of the payment card
void COLAEpsilonDiscount::AddedPreAuthInformation_(long lPumpNumber, PAY_AT_PUMP_INFO *pInfo, long &lOffset, char *pOutBuffer, 
	const OLAPumpServiceMode& oPumpServiceMode, long & lGradeRestrict) {
	// Copy&pasted from Epsilon AddedPreAuthInformation on 3/4/2021
	PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);	
	const long	lPumpItemsCount = cPumpItems.lCounter;
	long	lNumOfItems = 0;//4.0.11.504	fix this function TD 14868


	
	// Add item list, logic refactored to
	// reduce the code nesting
	bool bClearToProcess = (lPumpNumber && lPumpItemsCount);
	if (!bClearToProcess)
		return;

	// go through all Pump Items (Items sold structure)
	for (int i = 0; i < lPumpItemsCount; i++)
	{
		const ItemSoldInfo& SoldInfo = cPumpItems.m_cItemSoldInfo[i];

		// if invalid grade, then continue on with the next data
		const auto lGrade = SoldInfo.lGrade;
		if (lGrade == 0) continue;

		AddedPreAuthProcessFuelItem(lPumpNumber, pInfo, lOffset, pOutBuffer, 
			oPumpServiceMode, lGradeRestrict, lGrade,
			SoldInfo, lNumOfItems);		
	}

	
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCount[I]", (BYTE *)"%s=%03ld,", lNumOfItems);

	//RFUEL-3939
	// added more data
	// AdjustmentCount[I]=xxx
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"AdjustmentCount[I]", (BYTE *)"%s=%d,", 1);

	// Get payment a.k.a Tender Discount
	// location of this discount value changes due to RFS only taking one discount
	// AdjustmentValue0[F]=xx.xxx
	const long lTenderDiscount = _Module.m_server.GetPaymentDiscount(*pInfo);	
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"AdjustmentValue0[F]", (BYTE *)"%s=%ld.%03ld,", lTenderDiscount / 1000, lTenderDiscount % 1000);

	//  Added by request to make the PES Linksever happy
	//ItemWeight0[F]=0
	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemWeight0[F]", (BYTE *)"%s=%ld,", 0);	

//  Only used for POS, Not used in this case
//	lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemIsFuel0[B]", (BYTE *)"%s=%s,", (BYTE *)"Y", 1);

}


void COLAEpsilonDiscount::BuildSetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, CPumpTransact *pTrs, char *pOutBuffer) {

	try {
		PreSetCustomerInfoRequest(lPumpNumber, pInBuffer, pTrs, pOutBuffer);

		// call base class Set Customer Info request
		base_class::BuildSetCustomerInfoRequest(lPumpNumber, pInBuffer, pTrs, pOutBuffer);

		PostSetCustomerInfoRequest(lPumpNumber, pInBuffer, pTrs, pOutBuffer);



		//Debugging Logs - reads back all parameters into an object
		//Dump all the fields reporting for SET_CUST, leave this commented out as 
		//it contains sensitive data
		//  Must be commented out in production code
		//const long outputLen = strnlen(pOutBuffer, MAX_OLA_BUFFER);
		//OLAParamReader OLAParamValidator(pOutBuffer, outputLen);		
		//char buffer[MAX_OLA_BUFFER] = { 0 };
		//MemoryStream LogMsg(buffer, sizeof(buffer));
		//LogMsg << "SET_CUST FINAL BUFFER : "
		//		<< OLAParamValidator << ends;
		//_LOGMSG.LogMsg(LogMsg, LOG_LEVEL_1);
	}
	catch (...) {
		// catch all exception 
		LogStream LogMsg;
		LogMsg << __FUNCTION__
			<< " - Pump=" << std::setfill('0') << std::setw(2) << lPumpNumber
			<< " EXCEPTION thrown"
			<< ends;
		_LOGMSG.LogMsg(LogMsg, LOG_LEVEL_0);
	}
}

// Called BEFORE caliing parent classes SetCustomerInfoRequest
// refactored to separate method
void COLAEpsilonDiscount::PreSetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, CPumpTransact *pTrs, char *pOutBuffer)
{
	PAY_AT_PUMP_INFO *pInfo = reinterpret_cast<PAY_AT_PUMP_INFO*>(pInBuffer);

	// Pump identified by the PumpNumber
	auto& Pump = _Module.m_server.GetPump(lPumpNumber);

	//  Take out the tender discount from lTotalDiscountPerGallon
	//  RFS was taking either Loyalty discount or Tender Discount, but not both.
	//  We implemented workaround th add tender discount to LoyaltyDiscount to show the
	//	correct dollar amount on the nozzle, now we need to take the value out of the total 
	//  to find loyalty discount that we report to VP

	// find the Grade Position to get to the Loyalty Discount for the grade
	short nGrade = 0;
	if (pTrs) { nGrade = pTrs->m_nGrade; }
	long lGradePos = _Module.m_server.GetGradePositionRestrection(nGrade, lPumpNumber);
	if (lGradePos > 0) lGradePos = lGradePos - 1;
	
	// this time, the structure sDiscount is sume of LoyaltyDiscount + TenderDiscount (if any)
	long lTotalDiscountPerGallon = a2l(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount, 
		sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount));

	// if we have tender discount as part of the lTotalDiscountPerGallon
	const long lTenderDiscount = _Module.m_server.GetTenderDiscount(*pInfo);
	long lLoyaltyDiscount = 0l;
	if (lTenderDiscount > 0) 
	{
		// Take out the tender discount due to tender add operation
		lLoyaltyDiscount = lTotalDiscountPerGallon - lTenderDiscount;

		// update the matrix value with just the loyalty discount
		l2a(lLoyaltyDiscount, pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount,
			sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount));

		// Store the total discounts to this location for VP
		l2a(lTotalDiscountPerGallon, pInfo->CardSaleInfo.cardData.sGRDiscountPerGallon, 
			sizeof(pInfo->CardSaleInfo.cardData.sGRDiscountPerGallon));

		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString sLogMsg;
			sLogMsg.Format("%s DISCOUNT:  sGRDiscountPerGallon=%d", __FUNCTION__, lTotalDiscountPerGallon);
			_LOGMSG.LogMsg(sLogMsg);
		}
	}
	else 
	{
		// RFUEL - 4260
		// if with no tender discount, we still need to store the value back into proper structure		
		//lLoyaltyDiscount = lTotalDiscountPerGallon;

		// not necessary if tender Discount is 0
		//l2a(lLoyaltyDiscount, pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount,
		//	sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount));

		// Store the total discounts to this location for VP
		l2a(lTotalDiscountPerGallon, pInfo->CardSaleInfo.cardData.sGRDiscountPerGallon, 
			sizeof(pInfo->CardSaleInfo.cardData.sGRDiscountPerGallon));

		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString sLogMsg;
			sLogMsg.Format("%s DISCOUNT:  sGRDiscountPerGallon=%d", __FUNCTION__, lTotalDiscountPerGallon);
			_LOGMSG.LogMsg(sLogMsg);
		}
	}

	// zero out tender discount in the struct, was told that this is not needed,
	//  we keep this commented until the other team says otherwise
	//BYTE* pTenderDiscount = &pInfo->CardSaleExtraData6.sTenderDiscount[0];
	//std::fill(pTenderDiscount, pTenderDiscount + DISCOUNT_SIZE, ' ');
	//

	Pump.SetPAPInfo(pInfo);

	//if (LOG_DETAIL_FLOW_CONTROL)
	//{
	//	LogStream LogMsg;
	//	LogMsg << __FUNCTION__
	//		<< " - Pump=" << std::setfill('0') << std::setw(2) << lPumpNumber
	//		<< " TenderDiscount=" << lTenderDiscount
	//		<< " LoyaltyDiscount=" << lLoyaltyDiscount
	//		<< " TotalDiscount=" << lTotalDiscountPerGallon
	//		<< ends;
	//	_LOGMSG.LogMsg(LogMsg, LOG_LEVEL_0);

	//}
}

//  Used for adding logic after calling the Parent classes SetCustomerInfoRequest
//   The amount of code after process grew and eventually it was refactored into this method
//
void COLAEpsilonDiscount::PostSetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, CPumpTransact *pTrs, char *pOutBuffer)
{
	PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);

	PAY_AT_PUMP_INFO *pInfo = reinterpret_cast<PAY_AT_PUMP_INFO*>(pInBuffer);
	auto& Pump = _Module.m_server.GetPump(lPumpNumber);

	string FieldName;

	// find the tail end of the output data	
	const long lOffset = strnlen(pOutBuffer, MAX_OLA_BUFFER);

	// calculate how much space is available
	const size_t RemainingBufferSize = MAX_OLA_BUFFER - lOffset;

	// pass nullptr to not to disturb existing data
	// Use this object to build the parameter, then copy over the data
	// to the final array using OLAWriter
	OLAParamBuilder OLAParams(nullptr, 0);

	//RFUEL-3939
	// added more data
	// AdjustmentCount[I]=1
	OLAParams.Add("AdjustmentCount[I]", "1");


	// Get payment a.k.a Tender Discount
	// AdjustmentValue0[F]=x.ccc
	const long lAdjustmentValue = _Module.m_server.GetTenderDiscount(*pInfo);
	FieldName = "AdjustmentValue0[F]";
	OLAParams.AddPrice(FieldName, lAdjustmentValue);

	long lGRDiscountPerGallon = 0l;

	// PromotionTotalAmount[F]=xxx.xxx
	try {
		// find the Grade Position to get to the Loyalty Discount for the grade
		short nGrade = 0;
		if (pTrs) { nGrade = pTrs->m_nGrade; }
		long lGradePos = _Module.m_server.GetGradePositionRestrection(nGrade, lPumpNumber);
		if (lGradePos > 0) lGradePos = lGradePos - 1;

		// at this time, both loyalty discount and tender discount should already been separated
		// no calculation is necessary other than just pulling the value from the structure
		constexpr size_t LoyaltyDiscountSize = sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount);
		const long lLoyaltyDiscount =
			a2l(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount, LoyaltyDiscountSize);
		lGRDiscountPerGallon = lAdjustmentValue + lLoyaltyDiscount;

		if (LOG_DETAIL_FLOW_CONTROL)
		{	
			LogStream LogMsg;
			LogMsg << "DISCOUNT: Loyalty=" << lLoyaltyDiscount
				<< " Tender=" << lAdjustmentValue
				<< " GRDiscountPerGallon=" << lGRDiscountPerGallon << ends;
			_LOGMSG.LogMsg(LogMsg);
		}

		//if (LOG_DETAIL_FLOW_CONTROL)
		//{	
		//	LogStream LogMsg;
		//	DebugDumpDiscountMatrix(*pInfo, LogMsg);
		//	LogMsg << endl;
		//	LogMsg << " GradePos=" << lGradePos << " CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount=" << lLoyaltyDiscount;
		//	LogMsg << ends;

		//	_LOGMSG.LogMsg(LogMsg);
		//}


		//
		// pull the original unit price
		//
		auto& Pump = _Module.m_server.GetPump(lPumpNumber);
		auto& cPumpStatus = Pump.GetStat();

		const OLAPumpServiceMode oServiceMode(cPumpStatus.GetServiceMode());

		long nPumpPriceLevel =
			pTrs ? pTrs->m_nLevel : 0l;
		long ItemPricePES = GetGradePrice(cPumpItems, nGrade, oServiceMode, nPumpPriceLevel);

		// ItemPricePES0[F]=UnitPrice
		FieldName = OLAParams.Format("ItemPricePES%d[F]", 0);
		OLAParams.AddPrice(FieldName, ItemPricePES);

		
		// PromotionTotalAmount[F]=xxx.xxx
		// Get the Transaction volume
		const long lVolume = 
			pTrs ? pTrs->m_lRoundedVolume : 0l;

		using namespace mathext;

		// lValue and lDiscount are both fixed value representation of the value
		Fixed_Point_Value Fixed_Vol(lVolume);
		Fixed_Point_Value Fixed_Loyalty_discount(lLoyaltyDiscount);

		// multiply the two numbers and round to nearest cent
		Fixed_Point_Value Fixed_PromoTotalAmt = Fixed_Vol * Fixed_Loyalty_discount;
		Fixed_PromoTotalAmt.RoundToCents();

		
		FieldName = "PromotionTotalAmount[F]";
		OLAParams.AddPrice(FieldName, Fixed_PromoTotalAmt);

		if (LOG_DETAIL_FLOW_CONTROL)
		{
			LogStream LogMsg;
			LogMsg << __FUNCTION__
				<< " - Pump=" << std::setfill('0') << std::setw(2) << lPumpNumber
				<< " UnitPrice=" << ItemPricePES
				<< " AdjustementValue(TenderDiscount)=" << lAdjustmentValue
				<< " LoyaltyDiscount=" << lLoyaltyDiscount
				<< " Volume=" << lVolume
				<< " PromotionTotalAmount=" << Fixed_PromoTotalAmt
				<< ends;
			_LOGMSG.LogMsg(LogMsg);
		}


		if (LOG_DETAIL_FLOW_CONTROL)
		{
			LogStream LogMsg;			
			LogMsg << "Final Discount Matrix" << endl;
			LogMsg << "GradeIndex= " << lGradePos;
			DebugDumpDiscountMatrix(*pInfo, LogMsg);
			LogMsg << ends;

			_LOGMSG.LogMsg(LogMsg);
		}
	}
	catch (...) {
		LogStream LogMsg;
		LogMsg << __FUNCTION__
			<< " - Pump=" << std::setfill('0') << std::setw(2) << lPumpNumber
			<< " EXCEPTION thrown while constructing PromotionTotalAmount and ItemPricePES"
			<< ends;
		_LOGMSG.LogMsg(LogMsg, LOG_LEVEL_0);
	}

	// Write Parameters to the buffer
	OLAParamWriter OLAWriter(pOutBuffer + lOffset, RemainingBufferSize);
	OLAWriter << OLAParams;

	// Store the total discounts to this location for VP
	constexpr size_t lGRDiscountPerGalSize = sizeof(pInfo->CardSaleInfo.cardData.sGRDiscountPerGallon);
	l2a0(lGRDiscountPerGallon, pInfo->CardSaleInfo.cardData.sGRDiscountPerGallon,
		lGRDiscountPerGalSize);
	if (LOG_DETAIL_FLOW_CONTROL)
	{
		LogStream LogMsg;
		LogMsg << "DISCOUNT: Updating PumpPAPInfo GRDiscountPerGallon=" 
			<< lGRDiscountPerGallon << ends;
		_LOGMSG.LogMsg(LogMsg);
	}

	Pump.SetPAPInfo(pInfo);
	
	// Clear the flag
	_Module.m_server.SetLoyaltyDiscountApplied(lPumpNumber, ' ');

	_Module.m_server.SavePumpInfo(lPumpNumber);

}