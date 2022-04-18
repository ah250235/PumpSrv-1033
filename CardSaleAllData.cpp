#include "StdAfx.h"
#include "XMLInterface.h"   //4.0.9999


#define MAX_SALE 4
#define MAX_DISCNT 8


void AddLineWithNull(CXMLInterface *xmlInitDoc, CXMLPointers *pCurrElement, char *psAttributeName, BYTE *value, long lStrLen)
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

void EndLineWithNull(CXMLInterface * xmlInitDoc, CXMLPointers  * pCurrElement, char	 * psAttributeName, BYTE  * value, unsigned int    uStrLen)
{
	unsigned int i =0;
	unsigned char  *tmpStr;
	BOOL	 bValid = TRUE;
	tmpStr = new unsigned char[uStrLen+1];

	tmpStr[uStrLen] = 0;
	memcpy(tmpStr,value,uStrLen);
	for(i=0;i<uStrLen;i++)
	{
		if((tmpStr[i] != ' ')&&(tmpStr[i] != '0'))
		{
			bValid = FALSE;
			break;
		}
	}
	if(!bValid)
		xmlInitDoc->SetCharAttribute(psAttributeName,(char *)tmpStr,pCurrElement);

	delete[] tmpStr;


}



CCardSaleAllData::CCardSaleAllData()
{
	strcpy_s(m_StructName[0],NAME_LEN,"CARD_SALE_DATA");

	strcpy_s(m_strCName[0],NAME_LEN, "sTranStatus");
	strcpy_s(m_strCName[1],NAME_LEN, "sTranType");
	strcpy_s(m_strCName[2],NAME_LEN, "sMID");
	strcpy_s(m_strCName[3],NAME_LEN, "sPumpNumber");
	strcpy_s(m_strCName[4],NAME_LEN, "sDeviceNumber");
	strcpy_s(m_strCName[5],NAME_LEN, "sTranNbr");
	strcpy_s(m_strCName[6],NAME_LEN, "sRequestAmt");
	strcpy_s(m_strCName[7],NAME_LEN, "sSurchargeAmt");
	strcpy_s(m_strCName[8],NAME_LEN, "sTrack1Data");
	strcpy_s(m_strCName[9],NAME_LEN, "sTrack2Data");
	strcpy_s(m_strCName[10],NAME_LEN,"sStartDate");
	strcpy_s(m_strCName[11],NAME_LEN,"sCardIssue");
	strcpy_s(m_strCName[12],NAME_LEN,"sDriverId");
	strcpy_s(m_strCName[13],NAME_LEN,"sOdometer");
	strcpy_s(m_strCName[14],NAME_LEN,"sPINBlock");
	strcpy_s(m_strCName[15],NAME_LEN,"sReturnTank");
	strcpy_s(m_strCName[16],NAME_LEN,"sReserved");
	strcpy_s(m_strCName[17],NAME_LEN,"sEntryMethod");
	strcpy_s(m_strCName[18],NAME_LEN,"sIsStoreAndForward");
	strcpy_s(m_strCName[19],NAME_LEN,"sCardType");
	strcpy_s(m_strCName[20],NAME_LEN,"sFullVehicleId");
	strcpy_s(m_strCName[21],NAME_LEN,"sAuthorizeSessionID");
	strcpy_s(m_strCName[22],NAME_LEN,"sAuthAmt");
	strcpy_s(m_strCName[23],NAME_LEN,"sAuthNumber");
	strcpy_s(m_strCName[24],NAME_LEN,"sIsOnLine");
	strcpy_s(m_strCName[25],NAME_LEN,"sClearanceID");
	strcpy_s(m_strCName[26],NAME_LEN,"sAccountNumber");
	strcpy_s(m_strCName[27],NAME_LEN,"sFullCardName");
	strcpy_s(m_strCName[28],NAME_LEN,"sDateTime");
	strcpy_s(m_strCName[29],NAME_LEN,"sNozzle");
	strcpy_s(m_strCName[30],NAME_LEN,"sResultMsg");
	strcpy_s(m_strCName[31],NAME_LEN,"sResultMsg");
	strcpy_s(m_strCName[32],NAME_LEN,"sMerchantID");
	strcpy_s(m_strCName[33],NAME_LEN,"sNozzleRestriction");
	strcpy_s(m_strCName[34],NAME_LEN,"sReceiptRestriction");
	strcpy_s(m_strCName[35],NAME_LEN,"sApproval");
	strcpy_s(m_strCName[36],NAME_LEN,"sResInfoAMax8");
	strcpy_s(m_strCName[37],NAME_LEN,"sReferenceNumber");
	strcpy_s(m_strCName[38],NAME_LEN,"sCardName");
	strcpy_s(m_strCName[39],NAME_LEN,"sExpire");
	strcpy_s(m_strCName[40],NAME_LEN,"sGradeRestriction");
	strcpy_s(m_strCName[41],NAME_LEN,"sHostTerminalID");
	strcpy_s(m_strCName[42],NAME_LEN,"sGRPrompt");
	strcpy_s(m_strCName[43],NAME_LEN,"sAVIEngineHours");
	strcpy_s(m_strCName[44],NAME_LEN,"sAVISystemCustomer");
	strcpy_s(m_strCName[45],NAME_LEN,"sAVICompanyId");
	strcpy_s(m_strCName[46],NAME_LEN,"sAVIUserId");
	strcpy_s(m_strCName[47],NAME_LEN,"sAVILimit");
	strcpy_s(m_strCName[48],NAME_LEN,"sAVILimitType");
	strcpy_s(m_strCName[49],NAME_LEN,"sExtendPIN");
	strcpy_s(m_strCName[50],NAME_LEN,"sFinalRefNumber");
	strcpy_s(m_strCName[51],NAME_LEN,"sFinalAuthNumber");
	strcpy_s(m_strCName[52],NAME_LEN,"sSpare5");
	strcpy_s(m_strCName[53],NAME_LEN,"sGRVoucherNumber");
	strcpy_s(m_strCName[54],NAME_LEN,"sGRKeyCode");
	strcpy_s(m_strCName[55],NAME_LEN,"sGRDiscountPerGallon");
	strcpy_s(m_strCName[56],NAME_LEN,"sGRMaxVolume");
	strcpy_s(m_strCName[57],NAME_LEN,"sAuthTerminal");
	strcpy_s(m_strCName[58],NAME_LEN,"sAuthVersion");
	strcpy_s(m_strCName[59],NAME_LEN,"sGeneralID");
	strcpy_s(m_strCName[60],NAME_LEN,"sTransBatch");
	strcpy_s(m_strCName[61],NAME_LEN,"sTransBatchSequence");
	strcpy_s(m_strCName[62],NAME_LEN,"sClubCardTrack2Data");
	strcpy_s(m_strCName[63],NAME_LEN,"sAuthCode");
	strcpy_s(m_strCName[64],NAME_LEN,"sAppCodeElectr");

	strcpy_s(m_StructName[0],NAME_LEN,"CARD_SALE_EXTRA_DATA");

	strcpy_s(m_strCName[65],NAME_LEN, "sWashCode");
	strcpy_s(m_strCName[66],NAME_LEN, "sWashExpire");
	strcpy_s(m_strCName[67],NAME_LEN, "sPrnDateTime");
	strcpy_s(m_strCName[68],NAME_LEN, "sTotalAmt");
	strcpy_s(m_strCName[69],NAME_LEN, "sTaxAmt");
	strcpy_s(m_strCName[70],NAME_LEN, "sLoyalId");
	strcpy_s(m_strCName[71],NAME_LEN, "sLoyalScheme");
	strcpy_s(m_strCName[72],NAME_LEN, "sLoyalPoints");
	strcpy_s(m_strCName[73],NAME_LEN, "sVAT_Rate");
	strcpy_s(m_strCName[74],NAME_LEN, "sReciptStatus");
	strcpy_s(m_strCName[75],NAME_LEN,"sReserved");
	strcpy_s(m_strCName[76],NAME_LEN,"sGRTotalDiscount");
	strcpy_s(m_strCName[77],NAME_LEN,"sGRActualDiscountPerGallon");
	strcpy_s(m_strCName[78],NAME_LEN,"sNumberOfExtraSale");
	strcpy_s(m_strCName[79],NAME_LEN,"sFuelAmt");
	strcpy_s(m_strCName[80],NAME_LEN,"sCarWashProgram");
	strcpy_s(m_strCName[81],NAME_LEN,"sTotalAmt_Msb");
	strcpy_s(m_strCName[82],NAME_LEN,"sBankTermId");
	strcpy_s(m_strCName[83],NAME_LEN,"sCustomerName");
	strcpy_s(m_strCName[84],NAME_LEN,"sISOResponse");
	strcpy_s(m_strCName[85],NAME_LEN,"sSPDHCode");
	strcpy_s(m_strCName[86],NAME_LEN,"sFuelAmt_Msb");
	strcpy_s(m_strCName[87],NAME_LEN,"sLoyaltyActive");
	strcpy_s(m_strCName[88],NAME_LEN,"sForceCompleteZero");
	strcpy_s(m_strCName[89],NAME_LEN,"sAuthAmt_Msb");
	strcpy_s(m_strCName[90],NAME_LEN,"sDateTimeExt");
	strcpy_s(m_strCName[91],NAME_LEN,"sOdometerExt");
	strcpy_s(m_strCName[92],NAME_LEN,"sCompleteDate");
	strcpy_s(m_strCName[93],NAME_LEN,"sCompleteTime");
	strcpy_s(m_strCName[94],NAME_LEN,"sAttandantSession");
	strcpy_s(m_strCName[95],NAME_LEN,"sSlipFlags");
	strcpy_s(m_strCName[96],NAME_LEN,"sRedeemPoints");
	strcpy_s(m_strCName[97],NAME_LEN,"sConfirmPrompt");
	strcpy_s(m_strCName[98],NAME_LEN,"sCouponReward");

	strcpy_s(m_StructName[0],NAME_LEN,"CARD_SALE_EXTRA_DATA2");


	strcpy_s(m_strCName[99],NAME_LEN, "sSiteId");
	strcpy_s(m_strCName[100],NAME_LEN, "sUnit");
	strcpy_s(m_strCName[101],NAME_LEN, "sTripNumber");
	strcpy_s(m_strCName[102],NAME_LEN, "sCustomerReference");
	strcpy_s(m_strCName[103],NAME_LEN, "sTractorHub");
	strcpy_s(m_strCName[104],NAME_LEN, "sTrailerNumber");
	strcpy_s(m_strCName[105],NAME_LEN, "sTrailerHub");
	strcpy_s(m_strCName[106],NAME_LEN, "sOriginTrans");
	strcpy_s(m_strCName[107],NAME_LEN, "sScripUnitPrice");
	strcpy_s(m_strCName[108],NAME_LEN, "sMarginUnitPrice");
	strcpy_s(m_strCName[109],NAME_LEN,"sTotalScripAmount");
	strcpy_s(m_strCName[110],NAME_LEN,"sDeclinedLoyaltyPrompt");
	strcpy_s(m_strCName[111],NAME_LEN,"sDeclinedLoyaltyResponse");
	strcpy_s(m_strCName[112],NAME_LEN,"sAccountType");
	strcpy_s(m_strCName[113],NAME_LEN,"sCustomerMessage");
	strcpy_s(m_strCName[114],NAME_LEN,"sCouponMessage");
	strcpy_s(m_strCName[115],NAME_LEN,"ReceiptMessage");  //4.0.10.811
	strcpy_s(m_strCName[116],NAME_LEN,"sCouponId");
	strcpy_s(m_strCName[117],NAME_LEN,"sLoyaltyProgName");
	strcpy_s(m_strCName[118],NAME_LEN,"sPassCode");
	strcpy_s(m_strCName[119],NAME_LEN,"sGRDiscountType");
	strcpy_s(m_strCName[120],NAME_LEN,"sReceiptType");
	strcpy_s(m_strCName[121],NAME_LEN,"sServiceFee");
	strcpy_s(m_strCName[122],NAME_LEN,"sPrinterWidth");
	strcpy_s(m_strCName[123],NAME_LEN,"sParamNumber");
	strcpy_s(m_strCName[124],NAME_LEN,"sParam1");
	strcpy_s(m_strCName[125],NAME_LEN,"sParam2");
	strcpy_s(m_strCName[126],NAME_LEN,"sPaymentStage");
	strcpy_s(m_strCName[127],NAME_LEN,"sTextLength");
	strcpy_s(m_strCName[128],NAME_LEN,"sFormattedAccount");
	strcpy_s(m_strCName[129],NAME_LEN,"sAutoAuthorize");
	strcpy_s(m_strCName[130],NAME_LEN,"sWaitForReportFlag");
	strcpy_s(m_strCName[131],NAME_LEN,"sInvoiceNumber");
	strcpy_s(m_strCName[132],NAME_LEN,"sAuthorizationSerialNumber");
	strcpy_s(m_strCName[133],NAME_LEN,"sAuthorizerFileNumber");
	strcpy_s(m_strCName[134],NAME_LEN,"sPinPadAction");
	strcpy_s(m_strCName[135],NAME_LEN,"sPinPadResult");
	strcpy_s(m_strCName[136],NAME_LEN,"sCalculatedMAC");
	strcpy_s(m_strCName[137],NAME_LEN,"sPinSessionKey");
	strcpy_s(m_strCName[138],NAME_LEN,"sMACSessionKey");
	strcpy_s(m_strCName[139],NAME_LEN,"sLanguage");
	strcpy_s(m_strCName[140],NAME_LEN,"sPromotionNumber");
	strcpy_s(m_strCName[141],NAME_LEN,"sPriceLevel");
	strcpy_s(m_strCName[142],NAME_LEN,"sSignatureSlip");
	strcpy_s(m_strCName[143],NAME_LEN,"sReceiptPrinted");
	strcpy_s(m_strCName[144],NAME_LEN,"sResInfoAMax37");
	strcpy_s(m_strCName[145],NAME_LEN,"sGradePrice");
	strcpy_s(m_strCName[146],NAME_LEN,"sTotalDiscount");
	strcpy_s(m_strCName[147],NAME_LEN,"sVolumeLimit");
	strcpy_s(m_strCName[148],NAME_LEN,"sPriceRecalcDiscountSign");
	strcpy_s(m_strCName[149],NAME_LEN,"sTotalBalance");
	strcpy_s(m_strCName[150],NAME_LEN,"sAttendantID");
	strcpy_s(m_strCName[151],NAME_LEN,"sAttendantName");
	strcpy_s(m_strCName[152],NAME_LEN,"sIgnoreServiceFee");
	strcpy_s(m_strCName[153],NAME_LEN,"sSessionOffLine");
	strcpy_s(m_strCName[154],NAME_LEN,"sClockDateTime");
	strcpy_s(m_strCName[155],NAME_LEN,"sClockFlags");
	strcpy_s(m_strCName[156],NAME_LEN,"sItemsLinkIndex");
	strcpy_s(m_strCName[157],NAME_LEN,"sMessageTimeOut");
	strcpy_s(m_strCName[158],NAME_LEN,"sToken");
	strcpy_s(m_strCName[159],NAME_LEN,"sBarCode");
	strcpy_s(m_strCName[160],NAME_LEN,"sFlags");
	strcpy_s(m_strCName[161],NAME_LEN,"sAuthorizeSessionID_MSB");
	strcpy_s(m_strCName[162],NAME_LEN,"sCompletedSessionID_MSB");
	strcpy_s(m_strCName[163],NAME_LEN,"sZipCode");
	strcpy_s(m_strCName[164],NAME_LEN,"sTaxAmt2");
	strcpy_s(m_strCName[165],NAME_LEN,"sTaxAmt3");
	strcpy_s(m_strCName[166],NAME_LEN,"sCashDeposit1");
	strcpy_s(m_strCName[167],NAME_LEN,"sCashDeposit2");
	strcpy_s(m_strCName[168],NAME_LEN,"sCashDeposit5");
	strcpy_s(m_strCName[169],NAME_LEN,"sCashDeposit10");
	strcpy_s(m_strCName[170],NAME_LEN,"sCashDeposit20");
	strcpy_s(m_strCName[171],NAME_LEN,"sCashDeposit50");
	strcpy_s(m_strCName[172],NAME_LEN,"sCashDeposit100");
	strcpy_s(m_strCName[173],NAME_LEN,"sCashDepositTotal");
	strcpy_s(m_strCName[174],NAME_LEN,"sCompletedSessionID");
	strcpy_s(m_strCName[175],NAME_LEN,"sAuthorizerID");
	strcpy_s(m_strCName[176],NAME_LEN,"sLoyaltyTotalAmount");
	strcpy_s(m_strCName[177],NAME_LEN,"sPriceRecalcDiscount_Msb");
	strcpy_s(m_strCName[178],NAME_LEN,"sMediaMasterType");
	strcpy_s(m_strCName[179],NAME_LEN,"sDisplayOLAResultMessage");
	strcpy_s(m_strCName[180],NAME_LEN,"sChargeTo");
	strcpy_s(m_strCName[181],NAME_LEN,"sAVIFuelCode");
	strcpy_s(m_strCName[182],NAME_LEN,"sPriceRecalcDiscount");
	strcpy_s(m_strCName[183],NAME_LEN,"sRefundAmount");

	strcpy_s(m_StructName[0],NAME_LEN,"CARD_SALE_EXTRA_DATA3");


	strcpy_s(m_strCName[184],NAME_LEN, "sTotalRedeemedPoints");
	strcpy_s(m_strCName[185],NAME_LEN, "sAmountByPoints");
	strcpy_s(m_strCName[186],NAME_LEN, "sCreditAmount");
	strcpy_s(m_strCName[187],NAME_LEN, "sControllerSeqNum");
	strcpy_s(m_strCName[188],NAME_LEN, "sReSend");
	strcpy_s(m_strCName[189],NAME_LEN, "sRemoteID");
	strcpy_s(m_strCName[190],NAME_LEN, "sCarWashDiscount");
	strcpy_s(m_strCName[191],NAME_LEN, "sCarWashLimit");
	strcpy_s(m_strCName[192],NAME_LEN, "sParamResult");
	strcpy_s(m_strCName[193],NAME_LEN, "sPromptType");
	strcpy_s(m_strCName[194],NAME_LEN,"sPromptMinLen");
	strcpy_s(m_strCName[195],NAME_LEN,"sPromptMaxLen");
	strcpy_s(m_strCName[196],NAME_LEN,"sParamIndex");
	strcpy_s(m_strCName[197],NAME_LEN,"sCarwashRestriction");
	strcpy_s(m_strCName[198],NAME_LEN,"sReservedForfutureCarWash");
	strcpy_s(m_strCName[199],NAME_LEN,"sDriverId_MSB");
	strcpy_s(m_strCName[200],NAME_LEN,"sLogicShiftNumber");
	strcpy_s(m_strCName[201],NAME_LEN,"sFlags1");
	strcpy_s(m_strCName[202],NAME_LEN,"sOriginalUnitPrice");
	strcpy_s(m_strCName[203],NAME_LEN,"sNonDiscountTotalAmount");
	strcpy_s(m_strCName[204],NAME_LEN,"sOfflineTrans");
	strcpy_s(m_strCName[205],NAME_LEN,"sRequiredOlaData");
	strcpy_s(m_strCName[206],NAME_LEN,"sUnitPriceIncludeDiscount");
	strcpy_s(m_strCName[207],NAME_LEN,"sThreshold");
	strcpy_s(m_strCName[208],NAME_LEN,"sThresholdType");
	strcpy_s(m_strCName[209],NAME_LEN,"sClockRequired");
	strcpy_s(m_strCName[210],NAME_LEN,"sReceiptMode");
	strcpy_s(m_strCName[211],NAME_LEN,"sFiler995");

	strcpy_s(m_StructName[0],NAME_LEN,"SALE");

	strcpy_s(m_strCName[212],NAME_LEN,"sProdDesc");
	strcpy_s(m_strCName[213],NAME_LEN,"sDPTProdCode");
	strcpy_s(m_strCName[214],NAME_LEN,"sQty");
	strcpy_s(m_strCName[215],NAME_LEN,"sUnitPrice");
	strcpy_s(m_strCName[216],NAME_LEN,"sDiscount");
	strcpy_s(m_strCName[217],NAME_LEN,"sTax");

	strcpy_s(m_StructName[0],NAME_LEN,"DISCNT");

	strcpy_s(m_strCName[218],NAME_LEN,"sDiscount");

	strcpy_s(m_strCName[219],NAME_LEN,"sCashBacklimit");  //4.0.10.130
	strcpy_s(m_strCName[220],NAME_LEN,"sAmountLimitIncludesCashBack");  //4.0.10.130
	strcpy_s(m_strCName[221],NAME_LEN,"sDisplayCashBackAmount");  //4.0.10.130
	strcpy_s(m_strCName[222],NAME_LEN,"sAllowPartialCashBack");  //4.0.10.130

	strcpy_s(m_strCName[223],NAME_LEN,"sTruckingCompanyName");  //4.0.10.230
	strcpy_s(m_strCName[225],NAME_LEN,"sItemRestriction");  //4.0.6190.1800
	strcpy_s(m_strCName[226],NAME_LEN,"sCardSegment"); //Liat LPE
	strcpy_s(m_strCName[227],NAME_LEN,"sFormattedClubCardNumber"); // TD 327153
	strcpy_s(m_strCName[228], NAME_LEN, "sLoyaltyPointsExt"); // TD 327153
	strcpy_s(m_strCName[229], NAME_LEN, "sLoyaltyUniqueClientId");
	strcpy_s(m_strCName[230], NAME_LEN, "sLoyaltyStartTime");
	strcpy_s(m_strCName[231], NAME_LEN, "sTaxAmt4");

	strcpy_s(m_strCName[232], NAME_LEN, "sVAT_RateMSB"); //4.0.31.500  TD 404911
	strcpy_s(m_strCName[233], NAME_LEN, "sLoyaltyEntryMethod");
}



//////////////////////////////////////////////////////////////////////
// FUNCTION NAME: CreateXMLFromStruct
//
// DESCRIPTION:
// PARAMETERS:
//
// RETURNS:
//
// Who   When         What
// ---   --------     -----
// AMRAM   11/04/05     Start
// Add prompt information
//////////////////////////////////////////////////////////////////////
//4.0.9999.0
BOOL CCardSaleAllData::CreateXMLFromStruct(char *& pszXMLOut, long lOLAIndex ,CARD_SALE_ALL3 * pvStructIn, CPumpTransact & trs, void * pcRec)										  //4.0.9999.0
{
	BOOL bRetCode = TRUE;
	char	sLineID[5];
	char	sTmpOlaIndex[11];
	int i = 0;
	TRAVEL_CENTER_REC * pcTravelCenterRec;
	CXMLInterface xmlInitDoc,xmlChildDoc;
	CXMLPointers  pCurrElement, pChildNode, pChildNode3, pChildNode4, pChildNode5, pChildNode6, pChildNode7, pChildNode8, pChildNode9, pChildNode10, pChildNode11, pChildNode12, pChildNode15;

	long lPumpNumber = a2l(pvStructIn->CardSaleAll.data.sPumpNumber, sizeof(pvStructIn->CardSaleAll.data.sPumpNumber)); // 4.0.111.174 TD 319665
	long lTrsNumber = a2l(pvStructIn->CardSaleAll.data.sTranNbr, sizeof(pvStructIn->CardSaleAll.data.sTranNbr));
	try
	{
		bRetCode = xmlInitDoc.InitDoc();
		xmlInitDoc.CreateElement("CARD_SALE_ALL",&pCurrElement);
		xmlInitDoc.AppendRootElement(&pCurrElement);


		sprintf_s(sTmpOlaIndex ,"%ld" ,lOLAIndex);   //4.0.10.492
		xmlInitDoc.SetAttribute("sOLALinkId",(char *)sTmpOlaIndex,&pCurrElement); //4.0.10.492

		BYTE sVatRate[5] = {' '};
		PackFields(pvStructIn->CardSaleAll.extData.sVAT_Rate,sizeof(pvStructIn->CardSaleAll.extData.sVAT_Rate),
			&pvStructIn->extData5.sVAT_RateMSB,sizeof(pvStructIn->extData5.sVAT_RateMSB),sVatRate);

		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[0], &pvStructIn->CardSaleAll.data.sTranStatus,sizeof(pvStructIn->CardSaleAll.data.sTranStatus));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[1], &pvStructIn->CardSaleAll.data.sTranType,sizeof(pvStructIn->CardSaleAll.data.sTranType));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[2], pvStructIn->CardSaleAll.data.sMID,sizeof(pvStructIn->CardSaleAll.data.sMID));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[3],  pvStructIn->CardSaleAll.data.sPumpNumber,sizeof(pvStructIn->CardSaleAll.data.sPumpNumber));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[4], pvStructIn->CardSaleAll.data.sDeviceNumber,sizeof(pvStructIn->CardSaleAll.data.sDeviceNumber));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[5], pvStructIn->CardSaleAll.data.sTranNbr,sizeof(pvStructIn->CardSaleAll.data.sTranNbr) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[6], pvStructIn->CardSaleAll.data.sRequestAmt,sizeof(pvStructIn->CardSaleAll.data.sRequestAmt) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[7], pvStructIn->CardSaleAll.data.sSurchargeAmt,sizeof(pvStructIn->CardSaleAll.data.sSurchargeAmt) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[8], pvStructIn->CardSaleAll.data.sTrack1Data,sizeof(pvStructIn->CardSaleAll.data.sTrack1Data) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[9], pvStructIn->CardSaleAll.data.sTrack2Data,sizeof(pvStructIn->CardSaleAll.data.sTrack2Data) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[10], pvStructIn->CardSaleAll.data.sStartDate,sizeof(pvStructIn->CardSaleAll.data.sStartDate) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[11], pvStructIn->CardSaleAll.data.sCardIssue,sizeof(pvStructIn->CardSaleAll.data.sCardIssue) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[12], pvStructIn->CardSaleAll.data.sDriverId,sizeof(pvStructIn->CardSaleAll.data.sDriverId) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[13], pvStructIn->CardSaleAll.data.sOdometer,sizeof(pvStructIn->CardSaleAll.data.sOdometer) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[14], pvStructIn->CardSaleAll.data.sPINBlock,sizeof(pvStructIn->CardSaleAll.data.sPINBlock) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[15], &pvStructIn->CardSaleAll.data.sReturnTank,sizeof(pvStructIn->CardSaleAll.data.sReturnTank) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[16], pvStructIn->CardSaleAll.data.sReserved,sizeof(pvStructIn->CardSaleAll.data.sReserved) );

		if ((pvStructIn->CardSaleAll.data.sEntryMethod == ' ') || //4.0.10.50
			(pvStructIn->CardSaleAll.data.sEntryMethod == 'M')) 
		{
			pvStructIn->CardSaleAll.data.sEntryMethod = 'S';
		}


		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[17], &pvStructIn->CardSaleAll.data.sEntryMethod,sizeof(pvStructIn->CardSaleAll.data.sEntryMethod) );

		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[18], &pvStructIn->CardSaleAll.data.sIsStoreAndForward,sizeof(pvStructIn->CardSaleAll.data.sIsStoreAndForward) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[19], pvStructIn->CardSaleAll.data.sCardType,sizeof(pvStructIn->CardSaleAll.data.sCardType) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[20], pvStructIn->CardSaleAll.data.sFullVehicleId,sizeof(pvStructIn->CardSaleAll.data.sFullVehicleId) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[21], pvStructIn->CardSaleAll.data.sAuthorizeSessionID,sizeof(pvStructIn->CardSaleAll.data.sAuthorizeSessionID) );

		AddLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[22],pvStructIn->CardSaleAll.data.sAuthAmt,sizeof(pvStructIn->CardSaleAll.data.sAuthAmt) );

		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[23], pvStructIn->CardSaleAll.data.sAuthNumber,sizeof(pvStructIn->CardSaleAll.data.sAuthNumber) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[24], &pvStructIn->CardSaleAll.data.sIsOnLine,sizeof(pvStructIn->CardSaleAll.data.sIsOnLine) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[25], &pvStructIn->CardSaleAll.data.sClearanceID,sizeof(pvStructIn->CardSaleAll.data.sClearanceID) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[26], pvStructIn->CardSaleAll.data.sAccountNumber,sizeof(pvStructIn->CardSaleAll.data.sAccountNumber) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[27], pvStructIn->CardSaleAll.data.sFullCardName,sizeof(pvStructIn->CardSaleAll.data.sFullCardName) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[28], pvStructIn->CardSaleAll.data.sDateTime,sizeof(pvStructIn->CardSaleAll.data.sDateTime) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[29], &pvStructIn->CardSaleAll.data.sNozzle,sizeof(pvStructIn->CardSaleAll.data.sNozzle) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[30], pvStructIn->CardSaleAll.data.sResultMsg,sizeof(pvStructIn->CardSaleAll.data.sResultMsg) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[31], pvStructIn->CardSaleAll.data.sResultMsg,sizeof(pvStructIn->CardSaleAll.data.sResultMsg) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[32], pvStructIn->CardSaleAll.data.sMerchantID,sizeof(pvStructIn->CardSaleAll.data.sMerchantID) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[33], pvStructIn->CardSaleAll.data.sNozzleRestriction,sizeof(pvStructIn->CardSaleAll.data.sNozzleRestriction) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[34], pvStructIn->CardSaleAll.data.sReceiptRestriction,sizeof(pvStructIn->CardSaleAll.data.sReceiptRestriction) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[35], pvStructIn->CardSaleAll.data.sApproval,sizeof(pvStructIn->CardSaleAll.data.sApproval) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[36], pvStructIn->CardSaleAll.data.sResInfoAMax8,sizeof(pvStructIn->CardSaleAll.data.sResInfoAMax8) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[37], pvStructIn->CardSaleAll.data.sReferenceNumber	,sizeof(pvStructIn->CardSaleAll.data.sReferenceNumber) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[38], pvStructIn->CardSaleAll.data.sCardName,sizeof(pvStructIn->CardSaleAll.data.sCardName) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[39], pvStructIn->CardSaleAll.data.sExpire,sizeof(pvStructIn->CardSaleAll.data.sExpire) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[40], pvStructIn->CardSaleAll.data.sGradeRestriction,sizeof(pvStructIn->CardSaleAll.data.sGradeRestriction) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[41], pvStructIn->CardSaleAll.data.sHostTerminalID,sizeof(pvStructIn->CardSaleAll.data.sHostTerminalID) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[42], &pvStructIn->CardSaleAll.data.sGRPrompt,sizeof(pvStructIn->CardSaleAll.data.sGRPrompt) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[43], pvStructIn->CardSaleAll.data.sAVIEngineHours,sizeof(pvStructIn->CardSaleAll.data.sAVIEngineHours) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[44], pvStructIn->CardSaleAll.data.sAVISystemCustomer,sizeof(pvStructIn->CardSaleAll.data.sAVISystemCustomer) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[45], pvStructIn->CardSaleAll.data.sAVICompanyId,sizeof(pvStructIn->CardSaleAll.data.sAVICompanyId) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[46], pvStructIn->CardSaleAll.data.sAVIUserId,sizeof(pvStructIn->CardSaleAll.data.sAVIUserId) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[47], pvStructIn->CardSaleAll.data.sAVILimit,sizeof(pvStructIn->CardSaleAll.data.sAVILimit) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[48], &pvStructIn->CardSaleAll.data.sAVILimitType,sizeof(pvStructIn->CardSaleAll.data.sAVILimitType) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[49], pvStructIn->CardSaleAll.data.sExtendPIN,sizeof(pvStructIn->CardSaleAll.data.sExtendPIN) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[50], pvStructIn->CardSaleAll.data.sFinalRefNumber,sizeof(pvStructIn->CardSaleAll.data.sFinalRefNumber) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[51], pvStructIn->CardSaleAll.data.sFinalAuthNumber,sizeof(pvStructIn->CardSaleAll.data.sFinalAuthNumber) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[52], pvStructIn->CardSaleAll.data.sSpare5,sizeof(pvStructIn->CardSaleAll.data.sSpare5) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[53], pvStructIn->CardSaleAll.data.sGRVoucherNumber,sizeof(pvStructIn->CardSaleAll.data.sGRVoucherNumber) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[54], pvStructIn->CardSaleAll.data.sGRKeyCode,sizeof(pvStructIn->CardSaleAll.data.sGRKeyCode) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[55], pvStructIn->CardSaleAll.data.sGRDiscountPerGallon,sizeof(pvStructIn->CardSaleAll.data.sGRDiscountPerGallon) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[56], pvStructIn->CardSaleAll.data.sGRMaxVolume,sizeof(pvStructIn->CardSaleAll.data.sGRMaxVolume) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[57], pvStructIn->CardSaleAll.data.sAuthTerminal,sizeof(pvStructIn->CardSaleAll.data.sAuthTerminal) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[58], pvStructIn->CardSaleAll.data.sAuthVersion,sizeof(pvStructIn->CardSaleAll.data.sAuthVersion) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[59], pvStructIn->CardSaleAll.data.sGeneralID,sizeof(pvStructIn->CardSaleAll.data.sGeneralID) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[60], pvStructIn->CardSaleAll.data.sTransBatch,sizeof(pvStructIn->CardSaleAll.data.sTransBatch) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[61], pvStructIn->CardSaleAll.data.sTransBatchSequence,sizeof(pvStructIn->CardSaleAll.data.sTransBatchSequence) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[62], pvStructIn->CardSaleAll.data.sClubCardTrack2Data,sizeof(pvStructIn->CardSaleAll.data.sClubCardTrack2Data) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[63], pvStructIn->CardSaleAll.data.sAuthCode,sizeof(pvStructIn->CardSaleAll.data.sAuthCode) );
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[64], pvStructIn->CardSaleAll.data.sAppCodeElectr,sizeof(pvStructIn->CardSaleAll.data.sAppCodeElectr) );

		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[65],pvStructIn->CardSaleAll.extData.sWashCode,sizeof(pvStructIn->CardSaleAll.extData.sWashCode));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[66],pvStructIn->CardSaleAll.extData.sWashExpire,sizeof(pvStructIn->CardSaleAll.extData.sWashExpire));

		AddLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[67],pvStructIn->CardSaleAll.extData.sPrnDateTime,sizeof(pvStructIn->CardSaleAll.extData.sPrnDateTime));
		AddLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[68], pvStructIn->CardSaleAll.extData.sTotalAmt,sizeof(pvStructIn->CardSaleAll.extData.sTotalAmt));

		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[69],pvStructIn->CardSaleAll.extData.sTaxAmt,sizeof(pvStructIn->CardSaleAll.extData.sTaxAmt));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[70],pvStructIn->CardSaleAll.extData.sLoyalId,sizeof(pvStructIn->CardSaleAll.extData.sLoyalId));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[71],pvStructIn->CardSaleAll.extData.sLoyalScheme,sizeof(pvStructIn->CardSaleAll.extData.sLoyalScheme));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[72],pvStructIn->CardSaleAll.extData.sLoyalPoints,sizeof(pvStructIn->CardSaleAll.extData.sLoyalPoints));

		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[73],sVatRate,sizeof(sVatRate)); //4.0.31.500  TD 404911

		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[74],&pvStructIn->CardSaleAll.extData.sReciptStatus,sizeof(pvStructIn->CardSaleAll.extData.sReciptStatus));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[75],pvStructIn->CardSaleAll.extData.sReserved,sizeof(pvStructIn->CardSaleAll.extData.sReserved));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[76],pvStructIn->CardSaleAll.extData.sGRTotalDiscount,sizeof(pvStructIn->CardSaleAll.extData.sGRTotalDiscount));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[77],pvStructIn->CardSaleAll.extData.sGRActualDiscountPerGallon,sizeof(pvStructIn->CardSaleAll.extData.sGRActualDiscountPerGallon));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[78],&pvStructIn->CardSaleAll.extData.sNumberOfExtraSale,sizeof(pvStructIn->CardSaleAll.extData.sNumberOfExtraSale));
		AddLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[79],pvStructIn->CardSaleAll.extData.sFuelAmt,sizeof(pvStructIn->CardSaleAll.extData.sFuelAmt)); //4.0.10.130
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[80],&pvStructIn->CardSaleAll.extData.sCarWashProgram,sizeof(pvStructIn->CardSaleAll.extData.sCarWashProgram));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[81], pvStructIn->CardSaleAll.extData.sTotalAmt_Msb,sizeof(pvStructIn->CardSaleAll.extData.sTotalAmt_Msb));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[82],pvStructIn->CardSaleAll.extData.sBankTermId,sizeof(pvStructIn->CardSaleAll.extData.sBankTermId));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[83],pvStructIn->CardSaleAll.extData.sCustomerName,sizeof(pvStructIn->CardSaleAll.extData.sCustomerName));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[84],pvStructIn->CardSaleAll.extData.sISOResponse,sizeof(pvStructIn->CardSaleAll.extData.sISOResponse));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[85],pvStructIn->CardSaleAll.extData.sSPDHCode,sizeof(pvStructIn->CardSaleAll.extData.sSPDHCode));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[86],pvStructIn->CardSaleAll.extData.sFuelAmt_Msb,sizeof(pvStructIn->CardSaleAll.extData.sFuelAmt_Msb));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[87],&pvStructIn->CardSaleAll.extData.sLoyaltyActive,sizeof(pvStructIn->CardSaleAll.extData.sLoyaltyActive));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[88],&pvStructIn->CardSaleAll.extData.sForceCompleteZero,sizeof(pvStructIn->CardSaleAll.extData.sForceCompleteZero));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[89],pvStructIn->CardSaleAll.extData.sAuthAmt_Msb,sizeof(pvStructIn->CardSaleAll.extData.sAuthAmt_Msb));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[90],pvStructIn->CardSaleAll.extData.sDateTimeExt,sizeof(pvStructIn->CardSaleAll.extData.sDateTimeExt));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[91],pvStructIn->CardSaleAll.extData.sOdometerExt,sizeof(pvStructIn->CardSaleAll.extData.sOdometerExt));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[92],pvStructIn->CardSaleAll.extData.sCompleteDate,sizeof(pvStructIn->CardSaleAll.extData.sCompleteDate));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[93],pvStructIn->CardSaleAll.extData.sCompleteTime,sizeof(pvStructIn->CardSaleAll.extData.sCompleteTime));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[94],&pvStructIn->CardSaleAll.extData.sAttandantSession,sizeof(pvStructIn->CardSaleAll.extData.sAttandantSession));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[95],&pvStructIn->CardSaleAll.extData.sSlipFlags,sizeof(pvStructIn->CardSaleAll.extData.sSlipFlags));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[96],&pvStructIn->CardSaleAll.extData.sRedeemPoints,sizeof(pvStructIn->CardSaleAll.extData.sRedeemPoints));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[97],&pvStructIn->CardSaleAll.extData.sConfirmPrompt,sizeof(pvStructIn->CardSaleAll.extData.sConfirmPrompt));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[98],pvStructIn->CardSaleAll.extData.sCouponReward,sizeof(pvStructIn->CardSaleAll.extData.sCouponReward));

		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[99], pvStructIn->extData2.sSiteId,sizeof(pvStructIn->extData2.sSiteId));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[100], pvStructIn->extData2.sUnit,sizeof(pvStructIn->extData2.sUnit));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[101], pvStructIn->extData2.sTripNumber,sizeof(pvStructIn->extData2.sTripNumber));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[102], pvStructIn->extData2.sCustomerReference,sizeof(pvStructIn->extData2.sCustomerReference));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[103], pvStructIn->extData2.sTractorHub,sizeof(pvStructIn->extData2.sTractorHub));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[104], pvStructIn->extData2.sTrailerNumber,sizeof(pvStructIn->extData2.sTrailerNumber));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[105], pvStructIn->extData2.sTrailerHub,sizeof(pvStructIn->extData2.sTrailerHub));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[106], &pvStructIn->extData2.sOriginTrans,sizeof(pvStructIn->extData2.sOriginTrans));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[107], pvStructIn->extData2.sScripUnitPrice,sizeof(pvStructIn->extData2.sScripUnitPrice));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[108], pvStructIn->extData2.sMarginUnitPrice,sizeof(pvStructIn->extData2.sMarginUnitPrice));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[109], pvStructIn->extData2.sTotalScripAmount,sizeof(pvStructIn->extData2.sTotalScripAmount));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[110], &pvStructIn->extData2.sDeclinedLoyaltyPrompt,sizeof(pvStructIn->extData2.sDeclinedLoyaltyPrompt));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[111], &pvStructIn->extData2.sDeclinedLoyaltyResponse,sizeof(pvStructIn->extData2.sDeclinedLoyaltyResponse));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[112], &pvStructIn->extData2.sAccountType,sizeof(pvStructIn->extData2.sAccountType));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[113], pvStructIn->extData2.sCustomerMessage,sizeof(pvStructIn->extData2.sCustomerMessage));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[114], pvStructIn->extData2.sCouponMessage,sizeof(pvStructIn->extData2.sCouponMessage));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[115], pvStructIn->extData2.sReserveForCoupon,sizeof(pvStructIn->extData2.sReserveForCoupon));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[116], pvStructIn->extData2.sCouponId,sizeof(pvStructIn->extData2.sCouponId));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[117], pvStructIn->extData2.sLoyaltyProgName,sizeof(pvStructIn->extData2.sLoyaltyProgName));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[118], pvStructIn->extData2.sPassCode,sizeof(pvStructIn->extData2.sPassCode));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[119], &pvStructIn->extData2.sGRDiscountType,sizeof(pvStructIn->extData2.sGRDiscountType));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[120], pvStructIn->extData2.sReceiptType,sizeof(pvStructIn->extData2.sReceiptType));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[121], pvStructIn->extData2.sServiceFee,sizeof(pvStructIn->extData2.sServiceFee));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[122], pvStructIn->extData2.sPrinterWidth,sizeof(pvStructIn->extData2.sPrinterWidth));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[123], pvStructIn->extData2.sParamNumber,sizeof(pvStructIn->extData2.sParamNumber)); // CID_198868 Out-of-bounds access
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[124], pvStructIn->extData2.sParam1,sizeof(pvStructIn->extData2.sParam1));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[125], pvStructIn->extData2.sParam2,sizeof(pvStructIn->extData2.sParam2));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[126], &pvStructIn->extData2.sPaymentStage,sizeof(pvStructIn->extData2.sPaymentStage));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[127], pvStructIn->extData2.sTextLength,sizeof(pvStructIn->extData2.sTextLength));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[128], pvStructIn->extData2.sFormattedAccount,sizeof(pvStructIn->extData2.sFormattedAccount));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[129], &pvStructIn->extData2.sAutoAuthorize,sizeof(pvStructIn->extData2.sAutoAuthorize));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[130], &pvStructIn->extData2.sWaitForReportFlag,sizeof(pvStructIn->extData2.sWaitForReportFlag));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[131], pvStructIn->extData2.sInvoiceNumber,sizeof(pvStructIn->extData2.sInvoiceNumber));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[132], pvStructIn->extData2.sAuthorizationSerialNumber	,sizeof(pvStructIn->extData2.sAuthorizationSerialNumber));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[133], pvStructIn->extData2.sAuthorizerFileNumber,sizeof(pvStructIn->extData2.sAuthorizerFileNumber));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[134], &pvStructIn->extData2.sPinPadAction,sizeof(pvStructIn->extData2.sPinPadAction));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[135], &pvStructIn->extData2.sPinPadResult,sizeof(pvStructIn->extData2.sPinPadResult));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[136], pvStructIn->extData2.sCalculatedMAC,sizeof(pvStructIn->extData2.sCalculatedMAC));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[137], pvStructIn->extData2.sPinSessionKey	,sizeof(pvStructIn->extData2.sPinSessionKey));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[138], pvStructIn->extData2.sMACSessionKey,sizeof(pvStructIn->extData2.sMACSessionKey));


		if ((pvStructIn->extData2.sLanguage != ' ') && (pvStructIn->extData2.sLanguage  > 0))		// TD 426830
		{
			CString sStr;
			sStr.Format("%d",pvStructIn->extData2.sLanguage);
			EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[139], (BYTE *)sStr.GetBuffer(0),sizeof(sStr.GetBuffer(0)));
		}
		else
			EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[139], &pvStructIn->extData2.sLanguage,sizeof(pvStructIn->extData2.sLanguage));


		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[140], pvStructIn->extData2.sPromotionNumber,sizeof(pvStructIn->extData2.sPromotionNumber));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[141], &pvStructIn->extData2.sPriceLevel,sizeof(pvStructIn->extData2.sPriceLevel));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[142], &pvStructIn->extData2.sSignatureSlip,sizeof(pvStructIn->extData2.sSignatureSlip));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[143], &pvStructIn->extData2.sReceiptPrinted,sizeof(pvStructIn->extData2.sReceiptPrinted));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[144], pvStructIn->extData2.sResInfoAMax37,sizeof(pvStructIn->extData2.sResInfoAMax37));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[145], pvStructIn->extData2.sGradePrice,sizeof(pvStructIn->extData2.sGradePrice));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[146], pvStructIn->extData2.sTotalDiscount,sizeof(pvStructIn->extData2.sTotalDiscount));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[147], pvStructIn->extData2.sVolumeLimit,sizeof(pvStructIn->extData2.sVolumeLimit));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[148], &pvStructIn->extData2.sPriceRecalcDiscountSign,sizeof(pvStructIn->extData2.sPriceRecalcDiscountSign));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[149], pvStructIn->extData2.sTotalBalance,sizeof(pvStructIn->extData2.sTotalBalance));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[150], pvStructIn->extData2.sAttendantID,sizeof(pvStructIn->extData2.sAttendantID));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[151], pvStructIn->extData2.sAttendantName,sizeof(pvStructIn->extData2.sAttendantName));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[152], &pvStructIn->extData2.sIgnoreServiceFee,sizeof(pvStructIn->extData2.sIgnoreServiceFee));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[153], &pvStructIn->extData2.sSessionOffLine,sizeof(pvStructIn->extData2.sSessionOffLine));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[154], pvStructIn->extData2.sClockDateTime,sizeof(pvStructIn->extData2.sClockDateTime));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[155], pvStructIn->extData2.sClockFlags,sizeof(pvStructIn->extData2.sClockFlags));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[156], pvStructIn->extData2.sItemsLinkIndex,sizeof(pvStructIn->extData2.sItemsLinkIndex));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[157], pvStructIn->extData2.sMessageTimeOut,sizeof(pvStructIn->extData2.sMessageTimeOut));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[158], pvStructIn->extData2.sToken,sizeof(pvStructIn->extData2.sToken));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[159], pvStructIn->extData2.sBarCode,sizeof(pvStructIn->extData2.sBarCode));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[160], pvStructIn->extData2.sFlags,sizeof(pvStructIn->extData2.sFlags));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[161], pvStructIn->extData2.sAuthorizeSessionID_MSB,sizeof(pvStructIn->extData2.sAuthorizeSessionID_MSB));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[162], pvStructIn->extData2.sCompletedSessionID_MSB,sizeof(pvStructIn->extData2.sCompletedSessionID_MSB));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[163], pvStructIn->extData2.sZipCode,sizeof(pvStructIn->extData2.sZipCode));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[164], pvStructIn->extData2.sTaxAmt2,sizeof(pvStructIn->extData2.sTaxAmt2));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[165], pvStructIn->extData2.sTaxAmt3,sizeof(pvStructIn->extData2.sTaxAmt3));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[166], pvStructIn->extData2.sCashDeposit1,sizeof(pvStructIn->extData2.sCashDeposit1));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[167], pvStructIn->extData2.sCashDeposit2,sizeof(pvStructIn->extData2.sCashDeposit2));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[168], pvStructIn->extData2.sCashDeposit5,sizeof(pvStructIn->extData2.sCashDeposit5));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[169], pvStructIn->extData2.sCashDeposit10,sizeof(pvStructIn->extData2.sCashDeposit10));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[170], pvStructIn->extData2.sCashDeposit20,sizeof(pvStructIn->extData2.sCashDeposit20));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[171], pvStructIn->extData2.sCashDeposit50,sizeof(pvStructIn->extData2.sCashDeposit50));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[172], pvStructIn->extData2.sCashDeposit100,sizeof(pvStructIn->extData2.sCashDeposit100));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[173], pvStructIn->extData2.sCashDepositTotal,sizeof(pvStructIn->extData2.sCashDepositTotal));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[174], pvStructIn->extData2.sCompletedSessionID,sizeof(pvStructIn->extData2.sCompletedSessionID));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[175], pvStructIn->extData2.sAuthorizerID,sizeof(pvStructIn->extData2.sAuthorizerID));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[176], pvStructIn->extData2.sLoyaltyTotalAmount,sizeof(pvStructIn->extData2.sLoyaltyTotalAmount));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[177], pvStructIn->extData2.sPriceRecalcDiscount_Msb,sizeof(pvStructIn->extData2.sPriceRecalcDiscount_Msb));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[178], pvStructIn->extData2.sMediaMasterType,sizeof(pvStructIn->extData2.sMediaMasterType));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[179], &pvStructIn->extData2.sDisplayOLAResultMessage,sizeof(pvStructIn->extData2.sDisplayOLAResultMessage));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[180], pvStructIn->extData2.sChargeTo,sizeof(pvStructIn->extData2.sChargeTo));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[181], &pvStructIn->extData2.sAVIFuelCode,sizeof(pvStructIn->extData2.sAVIFuelCode));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[182], pvStructIn->extData2.sPriceRecalcDiscount,sizeof(pvStructIn->extData2.sPriceRecalcDiscount));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[183], pvStructIn->extData2.sRefundAmount,sizeof(pvStructIn->extData2.sRefundAmount));

		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[184], pvStructIn->extData3.sTotalRedeemedPoints,sizeof(pvStructIn->extData3.sTotalRedeemedPoints));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[185], pvStructIn->extData3.sAmountByPoints,sizeof(pvStructIn->extData3.sAmountByPoints));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[186], pvStructIn->extData3.sCreditAmount,sizeof(pvStructIn->extData3.sCreditAmount));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[187], pvStructIn->extData3.sControllerSeqNum,sizeof(pvStructIn->extData3.sControllerSeqNum));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[188], &pvStructIn->extData3.sReSend,sizeof(pvStructIn->extData3.sReSend));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[189], pvStructIn->extData3.sRemoteID,sizeof(pvStructIn->extData3.sRemoteID));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[190], pvStructIn->extData3.sCarWashDiscount,sizeof(pvStructIn->extData3.sCarWashDiscount));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[191], pvStructIn->extData3.sCarWashLimit,sizeof(pvStructIn->extData3.sCarWashLimit));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[192], pvStructIn->extData3.sParamResult,sizeof(pvStructIn->extData3.sParamResult));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[193], pvStructIn->extData3.sPromptType,sizeof(pvStructIn->extData3.sPromptType));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[194], pvStructIn->extData3.sPromptMinLen,sizeof(pvStructIn->extData3.sPromptMinLen));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[195], pvStructIn->extData3.sPromptMaxLen,sizeof(pvStructIn->extData3.sPromptMaxLen));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[196], pvStructIn->extData3.sParamIndex,sizeof(pvStructIn->extData3.sParamIndex));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[197],&pvStructIn->extData3.sCarwashRestriction,sizeof(pvStructIn->extData3.sCarwashRestriction));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[198], pvStructIn->extData3.sReservedForfutureCarWash,sizeof(pvStructIn->extData3.sReservedForfutureCarWash));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[199], pvStructIn->extData3.sDriverId_MSB,sizeof(pvStructIn->extData3.sDriverId_MSB));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[200], pvStructIn->extData3.sLogicShiftNumber,sizeof(pvStructIn->extData3.sLogicShiftNumber));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[201], pvStructIn->extData3.sFlags1,sizeof(pvStructIn->extData3.sFlags1));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[202], pvStructIn->extData3.sOriginalUnitPrice,sizeof(pvStructIn->extData3.sOriginalUnitPrice));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[203], pvStructIn->extData3.sNonDiscountTotalAmount,sizeof(pvStructIn->extData3.sNonDiscountTotalAmount));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[204], &pvStructIn->extData3.sOfflineTrans,sizeof(pvStructIn->extData3.sOfflineTrans));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[205], &pvStructIn->extData3.sRequiredOlaData,sizeof(pvStructIn->extData3.sRequiredOlaData));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[206], &pvStructIn->extData3.sUnitPriceIncludeDiscount,sizeof(pvStructIn->extData3.sUnitPriceIncludeDiscount));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[207], pvStructIn->extData3.sThreshold,sizeof(pvStructIn->extData3.sThreshold));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[208], &pvStructIn->extData3.sThresholdType,sizeof(pvStructIn->extData3.sThresholdType));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[209], &pvStructIn->extData3.sClockRequired,sizeof(pvStructIn->extData3.sClockRequired));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[210], &pvStructIn->extData3.sReceiptMode,sizeof(pvStructIn->extData3.sReceiptMode));
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[219], pvStructIn->extData3.sCashBacklimit,sizeof(pvStructIn->extData3.sCashBacklimit));  //4.0.10.50
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[223], pvStructIn->extData3.sTruckingCompanyName,sizeof(pvStructIn->extData3.sTruckingCompanyName));  //4.0.10.230
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[225], pvStructIn->extData3.sItemRestriction,sizeof(pvStructIn->extData3.sItemRestriction));  //TD - 39646 4.0.6190.1800
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[226], &pvStructIn->extData3.sCardSegment,sizeof(pvStructIn->extData3.sCardSegment));  //Liat LPE
		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[227], pvStructIn->extData5.sIssuerCodeTableIndex,sizeof(pvStructIn->extData5.sIssuerCodeTableIndex)); // QC 485279

		EndLineWithNull(&xmlInitDoc,&pCurrElement,m_strCName[227], pvStructIn->extData4.sFormattedClubCardNumber,sizeof(pvStructIn->extData4.sFormattedClubCardNumber)); // TD 327153
		EndLineWithNull(&xmlInitDoc, &pCurrElement, m_strCName[228], pvStructIn->extData6.sLoyaltyPointsExt, sizeof(pvStructIn->extData6.sLoyaltyPointsExt)); // TD 327153
		EndLineWithNull(&xmlInitDoc, &pCurrElement, m_strCName[231], pvStructIn->extData6.sTaxAmt4, sizeof(pvStructIn->extData6.sTaxAmt4));
		EndLineWithNull(&xmlInitDoc, &pCurrElement, m_strCName[229], pvStructIn->extData6.sCCMTaxLink, sizeof(pvStructIn->extData6.sCCMTaxLink));
		EndLineWithNull(&xmlInitDoc, &pCurrElement, m_strCName[232], &pvStructIn->extData5.sVAT_RateMSB, sizeof pvStructIn->extData5.sVAT_RateMSB); //4.0.31.500  TD 404911

		EndLineWithNull(&xmlInitDoc, &pCurrElement, m_strCName[233], &pvStructIn->extData4.sLoyaltyEntryMethod, sizeof pvStructIn->extData4.sLoyaltyEntryMethod); //4.0.31.500  TD 404911

		long lLoyaltyUniqueClient = _Module.m_server.GetLoyaltyUniqueClient() + lPumpNumber;
		CString sTmp;
		sTmp.Format("%d", lLoyaltyUniqueClient);
		EndLineWithNull(&xmlInitDoc, &pCurrElement, m_strCName[229],(BYTE *) sTmp.GetBuffer(0), sTmp.GetLength());

		AddLineWithNull(&xmlInitDoc, &pCurrElement, m_strCName[230], (unsigned char *)&pvStructIn->extData6.sLoyaltyStartTime, sizeof(pvStructIn->extData6.sLoyaltyStartTime));

		BuildLimitsToCardSaleXML(xmlInitDoc, pvStructIn, pCurrElement);

		xmlInitDoc.CreateElement("SALES",&pChildNode9);
		xmlInitDoc.AppendChild(&pChildNode9,&pCurrElement);
		if ((trs.m_lPump >0)  && (trs.m_lPump <= _Module.m_server.GetMaxPumps()))
		{
			long lGradePrice = trs.m_lGradePrice;
			long lVolume = trs.m_lRoundedVolume;
			long lValue = trs.m_lRoundedValue;


			// car was details
			long lAmountCarwash = a2l((BYTE *)&pvStructIn->CardSaleAll.extData.SALES[0].sUnitPrice,sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sUnitPrice)) * _Module.m_server.GetCarWashScalarFactor();
			long lDiscountCarwash = a2l((BYTE *)&pvStructIn->CardSaleAll.extData.SALES[0].sDiscount,sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sDiscount)) * 10;
			long lCarWashIndex = 2;

			if (_Module.m_server.m_cPumpArray[trs.m_lPump - 1].GetDieselPumpFlags())
			{
				CreateDieselSale(trs, pvStructIn,xmlInitDoc,pChildNode9, lCarWashIndex, lValue, lAmountCarwash, lDiscountCarwash, lVolume, lGradePrice);
			}
			else
			{
				AttachFuelItemToCardSaleDataXml(lValue, lAmountCarwash, lDiscountCarwash, pvStructIn, xmlInitDoc, pChildNode4, trs, lGradePrice, lVolume, pChildNode9);
			}

			AttachCarWashItemToCardSaleDataXml(lCarWashIndex, xmlInitDoc, pChildNode4, pvStructIn, lDiscountCarwash, pChildNode9);


		}
	////////////////////////////////////////////////////



	//////////////////////////////////////////////////

		TAG_PUMP_TRANSACT5 tagTrs;
		trs.GetTag(&tagTrs,2);

		xmlInitDoc.CreateElement("PUMP_TRANSACT",&pChildNode15);

		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sNumber", tagTrs.sNumber, sizeof(tagTrs.sNumber));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sNumber", tagTrs.sNumber, sizeof(tagTrs.sNumber));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sStatus", tagTrs.sStatus, sizeof(tagTrs.sStatus));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sOnOffer", tagTrs.sOnOffer, sizeof(tagTrs.sOnOffer));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sRawVolume", tagTrs.sRawVolume, sizeof(tagTrs.sRawVolume));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sRoundedVolume", tagTrs.sRoundedVolume, sizeof(tagTrs.sRoundedVolume));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sRoundedValue", tagTrs.sRoundedValue, sizeof(tagTrs.sRoundedValue));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sGradePrice", tagTrs.sGradePrice, sizeof(tagTrs.sGradePrice));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sGrade", tagTrs.sGrade, sizeof(tagTrs.sGrade));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sLevel", tagTrs.sLevel, sizeof(tagTrs.sLevel));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sPump", tagTrs.sPump, sizeof(tagTrs.sPump));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sMode", tagTrs.sMode, sizeof(tagTrs.sMode));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sRes", tagTrs.sRes, sizeof(tagTrs.sRes));

		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sResExt", tagTrs.sResExt, sizeof(tagTrs.sResExt));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sLinkNumber", tagTrs.sLinkNumber, sizeof(tagTrs.sLinkNumber));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sLinkFlags", tagTrs.sLinkFlags, sizeof(tagTrs.sLinkFlags));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sActiveReadNumber", tagTrs.sActiveReadNumber, sizeof(tagTrs.sActiveReadNumber));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sActiveShiftNumber", tagTrs.sActiveShiftNumber, sizeof(tagTrs.sActiveShiftNumber));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sMisc", tagTrs.sMisc, sizeof(tagTrs.sMisc));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sDateTime", (unsigned char *)&tagTrs.sDateTime, sizeof(tagTrs.sDateTime));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sPrePaidValue", tagTrs.sPrePaidValue, sizeof(tagTrs.sPrePaidValue));
		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sMiscH", tagTrs.sMiscH, sizeof(tagTrs.sMiscH));

		AddLineWithNull(&xmlInitDoc,&pChildNode15,"sServiceMode", tagTrs.sServiceMode, sizeof(tagTrs.sServiceMode));

		xmlInitDoc.AppendChild(&pChildNode15,&pCurrElement);

		CreateTendersData(xmlInitDoc, pCurrElement, pvStructIn);

		if (pcRec)
		{
			pcTravelCenterRec = (TRAVEL_CENTER_REC *)pcRec;
			long	lRtc = 0;
			TravelCenterData cTravelCenterData;

			// include promots
			xmlInitDoc.CreateElement("PROMPTS",&pChildNode8);
			xmlInitDoc. AppendChild(&pChildNode8,&pCurrElement);


			for (i=0;i< MAX_PROMPTS;i++)
			{
				cTravelCenterData = pcTravelCenterRec->cArrTravelCenterData[i];

				if (!ChrAll((char *)cTravelCenterData.PromptName0 ,sizeof(cTravelCenterData.PromptName0)))
				{
					sprintf_s(sLineID,"%ld",i +1);//get the line id
					xmlInitDoc.CreateElement("PROMPT",&pChildNode6);
					xmlInitDoc.SetAttribute("ID",sLineID,&pChildNode6);

					EndLineWithNull(&xmlInitDoc,&pChildNode6,"sPromptName0",	 cTravelCenterData.PromptName0,sizeof(cTravelCenterData.PromptName0));  //4.0.10.493
					AddLineWithNull(&xmlInitDoc,&pChildNode6,"sPromptDataType0", &cTravelCenterData.PromptDataType0,sizeof(cTravelCenterData.PromptDataType0));
					AddLineWithNull(&xmlInitDoc,&pChildNode6,"sPromptMaxLen0",   cTravelCenterData.PromptMaxLen0,sizeof(cTravelCenterData.PromptMaxLen0));
					AddLineWithNull(&xmlInitDoc,&pChildNode6,"sPromptMinLen0",   cTravelCenterData.PromptMinLen0,sizeof(cTravelCenterData.PromptMinLen0));
					AddLineWithNull(&xmlInitDoc,&pChildNode6,"sPromptMode0",     &cTravelCenterData.PromptMode0,sizeof(cTravelCenterData.PromptMode0));
					AddLineWithNull(&xmlInitDoc,&pChildNode6,"sPromptSource0" ,    cTravelCenterData.PromptSource0,sizeof(cTravelCenterData.PromptSource0));  //4.0.10.811
					AddLineWithNull(&xmlInitDoc,&pChildNode6,"sPromptEncrypted0", &cTravelCenterData.PromptEncrypted0,sizeof(cTravelCenterData.PromptEncrypted0));
					AddLineWithNull(&xmlInitDoc,&pChildNode6,"sPromptText0",     cTravelCenterData.PromptText0,sizeof(cTravelCenterData.PromptText0));
					AddLineWithNull(&xmlInitDoc,&pChildNode6,"sPromptResult0",   cTravelCenterData.PromptResult,sizeof(cTravelCenterData.PromptResult));
					xmlInitDoc.AppendChild(&pChildNode6,&pChildNode8);
				}

			}

			xmlInitDoc.CreateElement("DISCOUNTS",&pChildNode10);
			xmlInitDoc.AppendChild(&pChildNode10,&pCurrElement);


			for (int j = 0; j < 2; j++)
			{
				sprintf_s(sLineID, "%ld", j);//get the line id
				xmlInitDoc.CreateElement("Grade", &pChildNode11);
				xmlInitDoc.SetAttribute("ID", sLineID, &pChildNode11);

				xmlInitDoc.AppendChild(&pChildNode11, &pChildNode10);

				BYTE sTmpLine[60];
				for (i = 0; i < 5; i++)
				{
					if (pcTravelCenterRec->cItemTax[j][i].lTax)
					{
						sprintf_s(sLineID, sizeof(sLineID), "%ld", i + 1);//get the line id
						xmlInitDoc.CreateElement("DISCOUNT", &pChildNode12);
						xmlInitDoc.SetAttribute("ID", sLineID, &pChildNode12);

						sprintf_s((char*)sTmpLine, sizeof(sTmpLine), "%ld", pcTravelCenterRec->cItemTax[j][i].lTax);  //4.0.10.59

						AddLineWithNull(&xmlInitDoc, &pChildNode12, "sRate", sTmpLine, min(strlen((char*)sTmpLine), sizeof(sTmpLine)));//4.0.19.502

						memset(sTmpLine, 0, sizeof(sTmpLine));
						memcpy(sTmpLine, pcTravelCenterRec->cItemTax[j][i].sTaxDescription, min(sizeof(sTmpLine), sizeof(pcTravelCenterRec->cItemTax[j][i].sTaxDescription))); //4.0.10.54
						AddLineWithNull(&xmlInitDoc, &pChildNode12, "sDescription", sTmpLine, sizeof(pcTravelCenterRec->cItemTax[j][i].sTaxDescription));
						xmlInitDoc.AppendChild(&pChildNode12, &pChildNode11);
					}
				}
			}

		}

		// update the xml with loyalty receipt info
		char szFileName[MAX_PATH] = { 0 };
		_Module.m_server.m_cCommonFunction.GetLoyaltyPromotionsXmlFileName(lPumpNumber, lTrsNumber, szFileName); // TD 321602   4.0.111.174
		_Module.m_server.ConcatenateLoyaltyReceiptMessages(xmlInitDoc, pCurrElement, szFileName, lPumpNumber); // 4.0.111.171

		// convert the xml data structure to a string
		CComBSTR	bstrXML;
		xmlInitDoc.GetXmlString(&bstrXML); //4.0.15.287
		int iSizeOfXml = bstrXML.Length()+2;

		if (_Module.m_server.GetAddEncodingStringToVPXml())
		{
			const char szEncodingString[] = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
			int iSizeOfEncoding = sizeof(szEncodingString);
			if (iSizeOfXml > 0)
			{
				char *tmp = new char[iSizeOfXml];
				memset(tmp, 0, iSizeOfXml);
				int iLen = Bstr2Buffer(bstrXML, tmp, iSizeOfXml);
				const size_t OuputSize = iSizeOfXml + iSizeOfEncoding;
				if (NULL == pszXMLOut)
				{
					pszXMLOut = new char[iSizeOfXml + iSizeOfEncoding];
					memset(pszXMLOut, 0, iSizeOfXml);
				}
				sprintf_s(pszXMLOut, OuputSize, "%s %s", szEncodingString, tmp);

				delete[] tmp;

			}
		}
		else
		{
			if (iSizeOfXml > 0)
			{
				if (NULL == pszXMLOut)
				{
					pszXMLOut = new char[iSizeOfXml];
					memset(pszXMLOut, 0, iSizeOfXml);
				}
				int iLen = Bstr2Buffer(bstrXML, pszXMLOut, iSizeOfXml);
			}
		}

		WriteTransactionXml(xmlInitDoc);
		xmlInitDoc.CloseDoc();
	}
	catch(...)
	{
		_Module.m_server.m_cLogger.LogMsg("Execption in CCardSaleAllData::CreateXMLFromStruct!");
		return FALSE;
	}

	return TRUE;
}

/*
BOOL CCardSaleAllData::CreateXMLFormTaxPromAndCardSaleData(char * pszPromXML,char * pszCardSaleXML, char * pszTaxXML,char * pszOutXML)
{
	CComBSTR		bstrRootXML;
	CXMLInterface	xmlRootDoc,xmlChildDoc,xmlChildDoc1 ,xmlChildDoc2;
	CXMLPointers	pCurrElement,pChildNode ,pChildNode1 ,pChildNode2;

	BOOL bRetCode = TRUE;


	//Title Of Head Root
	bRetCode = xmlRootDoc.InitDoc();
	xmlRootDoc.CreateElement("PromCardSale",&pCurrElement);
	xmlRootDoc.AppendRootElement(&pCurrElement);

	//Card Sale Data XML
	xmlChildDoc.InitDoc();
	xmlChildDoc.LoadXmlString(pszPromXML);
	xmlChildDoc.GetRootNode(&pChildNode);
	xmlRootDoc.AppendChildElement(&pChildNode,&pCurrElement);

	//All Prom
	xmlChildDoc.LoadXmlString(pszCardSaleXML);
	xmlChildDoc.GetRootNode(&pChildNode);
	xmlRootDoc.AppendChildElement(&pChildNode,&pCurrElement);

	CComBSTR	bstrXML;
	bstrXML.Empty();

	xmlRootDoc.GetXmlString(&bstrXML); //4.0.15.287
	char tmp[MAX_XML_BUF_LEN];
	memset(&tmp,0x00,sizeof(tmp));

	int iLen = WideCharToMultiByte(CP_ACP,0,bstrXML,-1,tmp,sizeof(tmp),NULL,NULL);
	strcpy(pszOutXML,tmp);

	xmlChildDoc.CloseDoc();
	xmlRootDoc.CloseDoc();

	return TRUE;
}*/



void CCardSaleAllData::ParsePilotString(CString * pInStr,TravelCenterData & sTravelCenterData ,PAY_AT_PUMP_INFO *pInfo )  // 4.0.9.507
{
	BOOL bFound;
	int iLen ;
	char *pParam;
	long lRetCode = 0;

	*pInStr +=  ',';

	memset(&sTravelCenterData, ' ',sizeof(sTravelCenterData));

	bFound = _Module.m_server.m_cOLASrv->GetParam(pInStr,pInStr->Find("PromptName0[S]"),&pParam,&iLen); // HUSKY (change from Clubcard to ClubCard)
	if(bFound)
	{
		memset(sTravelCenterData.PromptName0,' ',sizeof(sTravelCenterData.PromptName0));
		if(iLen)
			memcpy(sTravelCenterData.PromptName0,pParam,min(sizeof(sTravelCenterData.PromptName0),iLen));


		memcpy(pInfo->CardSaleExtraData3.sPromptName,pParam,min(sizeof(pInfo->CardSaleExtraData3.sPromptName),iLen));

	}

	bFound = _Module.m_server.m_cOLASrv->GetParam(pInStr,pInStr->Find("PromptDataType0[S]"),&pParam,&iLen); // HUSKY (change from Clubcard to ClubCard)
	if(bFound)
	{
		sTravelCenterData.PromptDataType0 = ' ' ;
		sTravelCenterData.PromptDataType0 = pParam[0];

		switch (sTravelCenterData.PromptDataType0)		//4.0.10.11 //4.0.10.13
		{
			case 'N':pInfo->CardSaleExtraData3.sPromptType[0]  = '3'; break;
			case 'S':pInfo->CardSaleExtraData3.sPromptType[0]  = '2'; break;
			case 'T':pInfo->CardSaleExtraData3.sPromptType[0]  = '6'; break;
			case 'B':pInfo->CardSaleExtraData3.sPromptType[0]  = '1'; break;
			case 'C':pInfo->CardSaleExtraData3.sPromptType[0]  = '9'; break;
			case 'A':pInfo->CardSaleExtraData3.sPromptType[0]  = '2'; break;
		}



		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString sMsg;
			sMsg.Format("Prompt type is  %c - %c",sTravelCenterData.PromptDataType0 ,pInfo->CardSaleExtraData3.sPromptType[0] );
			_LOGMSG.LogMsg(sMsg);
		}

	}

	bFound = _Module.m_server.m_cOLASrv->GetParam(pInStr,pInStr->Find("PromptMaxLen0[I]"),&pParam,&iLen);
	if(bFound)
	{
		memset(sTravelCenterData.PromptMaxLen0,' ',sizeof(sTravelCenterData.PromptMaxLen0));
		if(iLen)
		{
			long lTmp = a2l((BYTE *)pParam,iLen);
			l2a(lTmp,sTravelCenterData.PromptMaxLen0,sizeof(sTravelCenterData.PromptMaxLen0));

			memcpy(pInfo->CardSaleExtraData3.sPromptMaxLen, sTravelCenterData.PromptMaxLen0 , sizeof(pInfo->CardSaleExtraData3.sPromptMaxLen));
		}
	}

	bFound = _Module.m_server.m_cOLASrv->GetParam(pInStr,pInStr->Find("PromptMinLen0[I]"),&pParam,&iLen);
	if(bFound)
	{
		memset(sTravelCenterData.PromptMinLen0,' ',sizeof(sTravelCenterData.PromptMinLen0));
		if(iLen)
		{
			long lTmp = a2l((BYTE *)pParam,iLen);
			l2a(lTmp,sTravelCenterData.PromptMinLen0,sizeof(sTravelCenterData.PromptMinLen0));
			memcpy(pInfo->CardSaleExtraData3.sPromptMinLen, sTravelCenterData.PromptMinLen0 , sizeof(pInfo->CardSaleExtraData3.sPromptMinLen));
		}
	}

	bFound = _Module.m_server.m_cOLASrv->GetParam(pInStr,pInStr->Find("PromptMode0[S]"),&pParam,&iLen); // HUSKY (change from Clubcard to ClubCard)
	if(bFound)
	{
		sTravelCenterData.PromptMode0 = ' ';
		sTravelCenterData.PromptMode0 = pParam[0];
		pInfo->CardSaleExtraData3.sPromptMode = pParam[0];
	}


	bFound = _Module.m_server.m_cOLASrv->GetParam(pInStr,pInStr->Find("PromptSource0[S]"),&pParam,&iLen); // HUSKY (change from Clubcard to ClubCard)
	if(bFound)
	{
		memset(sTravelCenterData.PromptSource0,' ',sizeof(sTravelCenterData.PromptSource0));
		if(iLen)
			memcpy(sTravelCenterData.PromptSource0,pParam,min(sizeof(sTravelCenterData.PromptSource0),iLen));
	}

	bFound = _Module.m_server.m_cOLASrv->GetParam(pInStr,pInStr->Find("PromptEncrypted0[B]"),&pParam,&iLen); // HUSKY (change from Clubcard to ClubCard)
	if(bFound)
	{
		sTravelCenterData.PromptEncrypted0 = ' ';

		sTravelCenterData.PromptEncrypted0 = pParam[0];
		pInfo->CardSaleExtraData3.sPromptEncrypted = pParam[0];
	}

	bFound = _Module.m_server.m_cOLASrv->GetParam(pInStr,pInStr->Find("PromptText0[S]"),&pParam,&iLen); // HUSKY (change from Clubcard to ClubCard)
	if(bFound)
	{
		memset(sTravelCenterData.PromptText0,' ',sizeof(sTravelCenterData.PromptText0));

		memset(pInfo->CardSaleExtraData2.sCouponMessage , ' ', sizeof(pInfo->CardSaleExtraData2.sCouponMessage));
		memset(pInfo->CardSaleExtraData2.sReserveForCoupon , ' ' , sizeof(pInfo->CardSaleExtraData2.sReserveForCoupon));

		if(iLen)
		{
			memcpy(sTravelCenterData.PromptText0,pParam,min(sizeof(sTravelCenterData.PromptText0),iLen));
			memcpy(pInfo->CardSaleExtraData2.sCouponMessage, pParam,min(sizeof(pInfo->CardSaleExtraData2.sCouponMessage),iLen)); //4.0.10.221-

			if (_Module.m_server.GetOLAInUse() == OLASRV_TRAVEL_CENTER)	// 4.0.23.2410
			{
				memset(pInfo->CardSaleInfo.cardData.sResultMsg,' ',sizeof(pInfo->CardSaleInfo.cardData.sResultMsg));
				memcpy(pInfo->CardSaleInfo.cardData.sResultMsg,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sResultMsg),iLen));
			}
		}
	}

	bFound = _Module.m_server.m_cOLASrv->GetParam(pInStr,pInStr->Find("OperationalPrompt0[B]"),&pParam,&iLen);
	if(bFound)
	{
		pInfo->CardSaleExtraData3.sOperationalPrompt = pParam[0];
	}

	bFound = _Module.m_server.m_cOLASrv->GetParam(pInStr,pInStr->Find("PromptTimeoutSec0[S]"),&pParam,&iLen);
	if(bFound)
	{
		memset(&pInfo->CardSaleExtraData2.sMessageTimeOut,' ',sizeof(pInfo->CardSaleExtraData2.sMessageTimeOut));
		if(iLen)
			memcpy(&pInfo->CardSaleExtraData2.sMessageTimeOut,pParam,min(sizeof(pInfo->CardSaleExtraData2.sMessageTimeOut),iLen));

	}
}


/******************************************************************************
 Description:	Create XML from One Promt
 Returns:
 Parameters:   	psTravelCenterData - contain all the prom attribute
				sOutXML		- XML that return

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			15/05/2005   17:15		Start
******************************************************************************/
/*
BOOL CCardSaleAllData::CreateXMLFromPilotStruct(TravelCenterData * psTravelCenterData,
										  char		* pszXMLOut)
{
	BOOL bRetCode = TRUE;

	CXMLInterface xmlInitDoc,xmlChildDoc;
	CXMLPointers  pCurrElement,pChildNode,pChildNode2,pChildNode3;


	bRetCode = xmlInitDoc.InitDoc();
	xmlInitDoc.CreateElement("PROMPT",&pCurrElement);
	xmlInitDoc.AppendRootElement(&pCurrElement);

	xmlInitDoc.CreateElement("PromKey",&pChildNode);

	EndLineWithNull(&xmlInitDoc,&pChildNode,"ID", psTravelCenterData->PromptName0,sizeof(psTravelCenterData->PromptName0));
	xmlInitDoc.AppendChild(&pChildNode,&pCurrElement);
	xmlInitDoc.CreateElement("Item",&pChildNode2);

	EndLineWithNull(&xmlInitDoc,&pChildNode2,m_strCName[1], &psTravelCenterData->PromptDataType0,sizeof(psTravelCenterData->PromptDataType0));
	EndLineWithNull(&xmlInitDoc,&pChildNode2,m_strCName[2], psTravelCenterData->PromptMaxLen0,sizeof(psTravelCenterData->PromptMaxLen0));
	EndLineWithNull(&xmlInitDoc,&pChildNode2,m_strCName[3], psTravelCenterData->PromptMinLen0,sizeof(psTravelCenterData->PromptMinLen0));
	EndLineWithNull(&xmlInitDoc,&pChildNode2,m_strCName[4], &psTravelCenterData->PromptMode0,sizeof(psTravelCenterData->PromptMode0));
	EndLineWithNull(&xmlInitDoc,&pChildNode2,m_strCName[5], psTravelCenterData->PromptSource0,sizeof(psTravelCenterData->PromptSource0));
	EndLineWithNull(&xmlInitDoc,&pChildNode2,m_strCName[6], &psTravelCenterData->PromptEncrypted0,sizeof(psTravelCenterData->PromptEncrypted0));
	EndLineWithNull(&xmlInitDoc,&pChildNode2,m_strCName[7], psTravelCenterData->PromptText0,sizeof(psTravelCenterData->PromptText0));

	xmlInitDoc.AppendChild(&pChildNode2,&pChildNode);


	CComBSTR	bstrXML;
	xmlInitDoc.GetXmlString(&bstrXML); //4.0.15.287
	char tmp[MAX_XML_BUF_LEN];
	memset(&tmp,0x00,sizeof(tmp));

	int iLen = WideCharToMultiByte(CP_ACP,0,bstrXML,-1,tmp,sizeof(tmp),NULL,NULL);
	strcpy_s(pszXMLOut,tmp);

	xmlInitDoc.CloseDoc();

	return bRetCode;
}
*/


/******************************************************************************
 Description:	Create List Of Promt XML
 Returns:
 Parameters:   	sArrTravelCenterData - arr of pilot data  contain all the prom attribute
				pszXMLOut		- XML that return

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			15/05/2005   17:15		Start
******************************************************************************/
/*
BOOL  CCardSaleAllData::CreateListOfPilotStruct(char * pszXMLOut,TravelCenterData  sArrTravelCenterData[],int NumOfData)
{
	int		i=0;
	BOOL    bRetCode = TRUE;
	char	sTmpXml[10000] = {0};
	CXMLInterface	xmlInitDoc,xmlChildDoc;
	CXMLPointers	pCurrElement,pChildNode,pChildNode2;


	CComBSTR	bstrXML;
	bstrXML.Empty();

	xmlInitDoc.GetXmlString(&bstrXML); //4.0.15.287
	char tmp[MAX_XML_BUF_LEN];
	memset(&tmp,0x00,sizeof(tmp));

	int iLen = WideCharToMultiByte(CP_ACP,0,bstrXML,-1,tmp,sizeof(tmp),NULL,NULL);
	strncpy_s(pszXMLOut,tmp);

	return bRetCode;
}
*/


//////////////////////////////////////////////////////////////////////
// FUNCTION NAME: CreateStructFromXML
//
// DESCRIPTION:
// PARAMETERS:
//
// RETURNS:
//
// Who   When         What
// ---   --------     -----
// AMRAM   11/04/05     Start
//////////////////////////////////////////////////////////////////////
BOOL	CCardSaleAllData::CreateStructFromXML(CARD_SALE_ALL3 * pvStructOut,void * pvTravelCenter,CPumpTransact & trs,char * pszXMLOIn)
{
	int				i = 0;
	CComBSTR		bstrXML;
	BOOL			bRetCode = TRUE;
	CXMLInterface	xmlInitDoc,xmlChildDoc;
	CXMLPointers	pItemElement,pItemElement2,pChildNode2,pChildNode,pChildNode3;
	CARD_SALE_ALL3  sCardAllData ;


	CXMLInterface   tmpXML;


	memset(&sCardAllData,' ',sizeof(CARD_SALE_ALL3));

	bRetCode = xmlInitDoc.InitDoc();
	if(bRetCode == TRUE)
		bRetCode = xmlInitDoc.LoadXmlString(pszXMLOIn);



	//save the struct to my output
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[0],sCardAllData.CardSaleAll.data.sTranStatus);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[1],sCardAllData.CardSaleAll.data.sTranType);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[2],sCardAllData.CardSaleAll.data.sMID);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[3],sCardAllData.CardSaleAll.data.sPumpNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[4],sCardAllData.CardSaleAll.data.sDeviceNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[5],sCardAllData.CardSaleAll.data.sTranNbr);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[6],sCardAllData.CardSaleAll.data.sRequestAmt);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[7],sCardAllData.CardSaleAll.data.sSurchargeAmt);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[8],sCardAllData.CardSaleAll.data.sTrack1Data);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[9],sCardAllData.CardSaleAll.data.sTrack2Data);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[10],sCardAllData.CardSaleAll.data.sStartDate);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[11],sCardAllData.CardSaleAll.data.sCardIssue);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[12],sCardAllData.CardSaleAll.data.sDriverId);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[13],sCardAllData.CardSaleAll.data.sOdometer);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[14],sCardAllData.CardSaleAll.data.sPINBlock);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[15],sCardAllData.CardSaleAll.data.sReturnTank);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[16],sCardAllData.CardSaleAll.data.sReserved);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[17],sCardAllData.CardSaleAll.data.sEntryMethod);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[18],sCardAllData.CardSaleAll.data.sIsStoreAndForward);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[19],sCardAllData.CardSaleAll.data.sCardType);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[20],sCardAllData.CardSaleAll.data.sFullVehicleId);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[21],sCardAllData.CardSaleAll.data.sAuthorizeSessionID);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[22],sCardAllData.CardSaleAll.data.sAuthAmt);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[23],sCardAllData.CardSaleAll.data.sAuthNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[24],sCardAllData.CardSaleAll.data.sIsOnLine);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[25],sCardAllData.CardSaleAll.data.sClearanceID);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[26],sCardAllData.CardSaleAll.data.sAccountNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[27],sCardAllData.CardSaleAll.data.sFullCardName);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[28],sCardAllData.CardSaleAll.data.sDateTime);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[29],sCardAllData.CardSaleAll.data.sNozzle);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[30],sCardAllData.CardSaleAll.data.sResultMsg);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[31],sCardAllData.CardSaleAll.data.sResultMsg);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[32],sCardAllData.CardSaleAll.data.sMerchantID);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[33],sCardAllData.CardSaleAll.data.sNozzleRestriction);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[34],sCardAllData.CardSaleAll.data.sReceiptRestriction);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[35],sCardAllData.CardSaleAll.data.sApproval);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[36],sCardAllData.CardSaleAll.data.sResInfoAMax8);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[37],sCardAllData.CardSaleAll.data.sReferenceNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[38],sCardAllData.CardSaleAll.data.sCardName);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[39],sCardAllData.CardSaleAll.data.sExpire);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[40],sCardAllData.CardSaleAll.data.sGradeRestriction);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[41],sCardAllData.CardSaleAll.data.sHostTerminalID);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[42],sCardAllData.CardSaleAll.data.sGRPrompt);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[43],sCardAllData.CardSaleAll.data.sAVIEngineHours);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[44],sCardAllData.CardSaleAll.data.sAVISystemCustomer);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[45],sCardAllData.CardSaleAll.data.sAVICompanyId);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[46],sCardAllData.CardSaleAll.data.sAVIUserId);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[47],sCardAllData.CardSaleAll.data.sAVILimit);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[48],sCardAllData.CardSaleAll.data.sAVILimitType);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[49],sCardAllData.CardSaleAll.data.sExtendPIN);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[50],sCardAllData.CardSaleAll.data.sFinalRefNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[51],sCardAllData.CardSaleAll.data.sFinalAuthNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[52],sCardAllData.CardSaleAll.data.sSpare5);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[53],sCardAllData.CardSaleAll.data.sGRVoucherNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[54],sCardAllData.CardSaleAll.data.sGRKeyCode);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[55],sCardAllData.CardSaleAll.data.sGRDiscountPerGallon);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[56],sCardAllData.CardSaleAll.data.sGRMaxVolume);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[57],sCardAllData.CardSaleAll.data.sAuthTerminal);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[58],sCardAllData.CardSaleAll.data.sAuthVersion);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[59],sCardAllData.CardSaleAll.data.sGeneralID);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[60],sCardAllData.CardSaleAll.data.sTransBatch);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[61],sCardAllData.CardSaleAll.data.sTransBatchSequence);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[62],sCardAllData.CardSaleAll.data.sClubCardTrack2Data);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[63],sCardAllData.CardSaleAll.data.sAuthCode);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[64],sCardAllData.CardSaleAll.data.sAppCodeElectr);

	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[65],sCardAllData.CardSaleAll.extData.sWashCode);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[66],sCardAllData.CardSaleAll.extData.sWashExpire);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[67],sCardAllData.CardSaleAll.extData.sPrnDateTime);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[68],sCardAllData.CardSaleAll.extData.sTotalAmt);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[69],sCardAllData.CardSaleAll.extData.sTaxAmt);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[70],sCardAllData.CardSaleAll.extData.sLoyalId);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[71],sCardAllData.CardSaleAll.extData.sLoyalScheme);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[72],sCardAllData.CardSaleAll.extData.sLoyalPoints);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[73],sCardAllData.CardSaleAll.extData.sVAT_Rate);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[74],sCardAllData.CardSaleAll.extData.sReciptStatus);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[75],sCardAllData.CardSaleAll.extData.sReserved);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[76],sCardAllData.CardSaleAll.extData.sGRTotalDiscount);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[77],sCardAllData.CardSaleAll.extData.sGRActualDiscountPerGallon);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[78],sCardAllData.CardSaleAll.extData.sNumberOfExtraSale);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[79],sCardAllData.CardSaleAll.extData.sFuelAmt);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[80],sCardAllData.CardSaleAll.extData.sCarWashProgram);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[81],sCardAllData.CardSaleAll.extData.sTotalAmt_Msb);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[82],sCardAllData.CardSaleAll.extData.sBankTermId);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[83],sCardAllData.CardSaleAll.extData.sCustomerName);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[84],sCardAllData.CardSaleAll.extData.sISOResponse);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[85],sCardAllData.CardSaleAll.extData.sSPDHCode);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[86],sCardAllData.CardSaleAll.extData.sFuelAmt_Msb);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[87],sCardAllData.CardSaleAll.extData.sLoyaltyActive);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[88],sCardAllData.CardSaleAll.extData.sForceCompleteZero);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[89],sCardAllData.CardSaleAll.extData.sAuthAmt_Msb);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[90],sCardAllData.CardSaleAll.extData.sDateTimeExt);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[91],sCardAllData.CardSaleAll.extData.sOdometerExt);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[92],sCardAllData.CardSaleAll.extData.sCompleteDate);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[93],sCardAllData.CardSaleAll.extData.sCompleteTime);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[94],sCardAllData.CardSaleAll.extData.sAttandantSession);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[95],sCardAllData.CardSaleAll.extData.sSlipFlags);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[96],sCardAllData.CardSaleAll.extData.sRedeemPoints);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[97],sCardAllData.CardSaleAll.extData.sConfirmPrompt);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[98],sCardAllData.CardSaleAll.extData.sCouponReward);

	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[99],sCardAllData.extData2.sSiteId);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[100],sCardAllData.extData2.sUnit);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[101],sCardAllData.extData2.sTripNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[102],sCardAllData.extData2.sCustomerReference);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[103],sCardAllData.extData2.sTractorHub);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[104],sCardAllData.extData2.sTrailerNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[105],sCardAllData.extData2.sTrailerHub);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[106],sCardAllData.extData2.sOriginTrans);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[107],sCardAllData.extData2.sScripUnitPrice);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[108],sCardAllData.extData2.sMarginUnitPrice);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[109],sCardAllData.extData2.sTotalScripAmount);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[110],sCardAllData.extData2.sDeclinedLoyaltyPrompt);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[111],sCardAllData.extData2.sDeclinedLoyaltyResponse);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[112],sCardAllData.extData2.sAccountType);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[113],sCardAllData.extData2.sCustomerMessage);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[114],sCardAllData.extData2.sCouponMessage);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[115],sCardAllData.extData2.sReserveForCoupon);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[116],sCardAllData.extData2.sCouponId);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[117],sCardAllData.extData2.sLoyaltyProgName);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[118],sCardAllData.extData2.sPassCode);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[119],sCardAllData.extData2.sGRDiscountType);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[120],sCardAllData.extData2.sReceiptType);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[121],sCardAllData.extData2.sServiceFee);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[122],sCardAllData.extData2.sPrinterWidth);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[123],sCardAllData.extData2.sParamNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[124],sCardAllData.extData2.sParam1);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[125],sCardAllData.extData2.sParam2);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[126],sCardAllData.extData2.sPaymentStage);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[127],sCardAllData.extData2.sTextLength);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[128],sCardAllData.extData2.sFormattedAccount);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[129],sCardAllData.extData2.sAutoAuthorize);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[130],sCardAllData.extData2.sWaitForReportFlag);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[131],sCardAllData.extData2.sInvoiceNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[132],sCardAllData.extData2.sAuthorizationSerialNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[133],sCardAllData.extData2.sAuthorizerFileNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[134],sCardAllData.extData2.sPinPadAction);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[135],sCardAllData.extData2.sPinPadResult);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[136],sCardAllData.extData2.sCalculatedMAC);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[137],sCardAllData.extData2.sPinSessionKey);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[138],sCardAllData.extData2.sMACSessionKey);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[139],sCardAllData.extData2.sLanguage);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[140],sCardAllData.extData2.sPromotionNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[141],sCardAllData.extData2.sPriceLevel);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[142],sCardAllData.extData2.sSignatureSlip);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[143],sCardAllData.extData2.sReceiptPrinted);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[144],sCardAllData.extData2.sResInfoAMax37);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[145],sCardAllData.extData2.sGradePrice);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[146],sCardAllData.extData2.sTotalDiscount);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[147],sCardAllData.extData2.sVolumeLimit);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[148],sCardAllData.extData2.sPriceRecalcDiscountSign);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[149],sCardAllData.extData2.sTotalBalance);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[150],sCardAllData.extData2.sAttendantID);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[151],sCardAllData.extData2.sAttendantName);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[152],sCardAllData.extData2.sIgnoreServiceFee);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[153],sCardAllData.extData2.sSessionOffLine);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[154],sCardAllData.extData2.sClockDateTime);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[155],sCardAllData.extData2.sClockFlags);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[156],sCardAllData.extData2.sItemsLinkIndex);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[157],sCardAllData.extData2.sMessageTimeOut);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[158],sCardAllData.extData2.sToken);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[159],sCardAllData.extData2.sBarCode);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[160],sCardAllData.extData2.sFlags);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[161],sCardAllData.extData2.sAuthorizeSessionID_MSB);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[162],sCardAllData.extData2.sCompletedSessionID_MSB);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[163],sCardAllData.extData2.sZipCode);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[164],sCardAllData.extData2.sTaxAmt2);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[165],sCardAllData.extData2.sTaxAmt3);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[166],sCardAllData.extData2.sCashDeposit1);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[167],sCardAllData.extData2.sCashDeposit2);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[168],sCardAllData.extData2.sCashDeposit5);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[169],sCardAllData.extData2.sCashDeposit10);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[170],sCardAllData.extData2.sCashDeposit20);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[171],sCardAllData.extData2.sCashDeposit50);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[172],sCardAllData.extData2.sCashDeposit100);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[173],sCardAllData.extData2.sCashDepositTotal);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[174],sCardAllData.extData2.sCompletedSessionID);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[175],sCardAllData.extData2.sAuthorizerID);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[176],sCardAllData.extData2.sLoyaltyTotalAmount);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[177],sCardAllData.extData2.sPriceRecalcDiscount_Msb);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[178],sCardAllData.extData2.sMediaMasterType);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[179],sCardAllData.extData2.sDisplayOLAResultMessage);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[180],sCardAllData.extData2.sChargeTo);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[181],sCardAllData.extData2.sAVIFuelCode);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[182],sCardAllData.extData2.sPriceRecalcDiscount);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[183],sCardAllData.extData2.sRefundAmount);

	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[184] ,sCardAllData.extData3.sTotalRedeemedPoints);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[185] ,sCardAllData.extData3.sAmountByPoints);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[186] ,sCardAllData.extData3.sCreditAmount);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[187] ,sCardAllData.extData3.sControllerSeqNum);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[188] ,sCardAllData.extData3.sReSend);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[189] ,sCardAllData.extData3.sRemoteID);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[190] ,sCardAllData.extData3.sCarWashDiscount);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[191] ,sCardAllData.extData3.sCarWashLimit);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[192] ,sCardAllData.extData3.sParamResult);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[193] ,sCardAllData.extData3.sPromptType);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[194],sCardAllData.extData3.sPromptMinLen);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[195],sCardAllData.extData3.sPromptMaxLen);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[196],sCardAllData.extData3.sParamIndex);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[197],sCardAllData.extData3.sCarwashRestriction);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[198],sCardAllData.extData3.sReservedForfutureCarWash);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[199],sCardAllData.extData3.sDriverId_MSB);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[200],sCardAllData.extData3.sLogicShiftNumber);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[201],sCardAllData.extData3.sFlags1);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[202],sCardAllData.extData3.sOriginalUnitPrice);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[203],sCardAllData.extData3.sNonDiscountTotalAmount);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[204],sCardAllData.extData3.sOfflineTrans);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[205],sCardAllData.extData3.sRequiredOlaData);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[206],sCardAllData.extData3.sUnitPriceIncludeDiscount);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[207],sCardAllData.extData3.sThreshold);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[208],sCardAllData.extData3.sThresholdType);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[209],sCardAllData.extData3.sClockRequired);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[210],sCardAllData.extData3.sReceiptMode);
//	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[211],sCardAllData.extData3.sFiler995);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[226],sCardAllData.extData3.sCardSegment); //Liat LPE
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[227],sCardAllData.extData4.sFormattedClubCardNumber); // TD 327153
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[228], sCardAllData.extData6.sLoyaltyPointsExt); // TD 327153
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[231], sCardAllData.extData6.sTaxAmt4);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[230], sCardAllData.extData6.sCCMTaxLink);
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[232], sCardAllData.extData5.sVAT_RateMSB); //4.0.31.500  TD 404911
	xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[233], sCardAllData.extData4.sLoyaltyEntryMethod); //4.0.31.500  TD 404911

	//Create all discnt structe
	xmlInitDoc.SelectSingleElementNode("LIMITS",&pChildNode);
	xmlInitDoc.GetChildNodesList(&pChildNode,&pItemElement);
	i=0;

	while(xmlInitDoc.GetNextItem(pItemElement,pChildNode))
	{
		xmlInitDoc.GetAttributeToByteArrWithConvert("sGrade",sCardAllData.extData3.GradeLimitsArray[i].sGradeID,&pChildNode);
		xmlInitDoc.GetAttributeToByteArrWithConvert("sAmountLimit",sCardAllData.extData3.GradeLimitsArray[i].sGradeAmountLimit,&pChildNode);
		xmlInitDoc.GetAttributeToByteArrWithConvert("sLimitType",sCardAllData.extData3.GradeLimitsArray[i].sLimitType,&pChildNode);
		xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[218],sCardAllData.extData2.sDiscountPerGrade[i].sDiscount,&pChildNode);
		i++;
	}

	TAG_PUMP_TRANSACT5	tagTrs;

	memset(&tagTrs,' ' ,sizeof(TAG_PUMP_TRANSACT5));
	BYTE sDateTime[10];
	BYTE sStartDateTime[10]; //TD 38654

	xmlInitDoc.SelectSingleElementNode("PUMP_TRANSACT",&pChildNode);
	trs.m_lNumber = xmlInitDoc.GetAttributeToLong("sNumber",&pChildNode);
	trs.m_lStatus = l2TrsStatus(xmlInitDoc.GetAttributeToLong("sStatus",&pChildNode));
	trs.m_lOnOffer = xmlInitDoc.GetAttributeToLong("sOnOffer",&pChildNode);
	trs.m_lRawVolume = xmlInitDoc.GetAttributeToLong("sRawVolume",&pChildNode);
	trs.m_lRoundedVolume = xmlInitDoc.GetAttributeToLong("sRoundedVolume",&pChildNode);
	trs.m_lRoundedValue=  xmlInitDoc.GetAttributeToLong("sRoundedValue",&pChildNode);
	trs.m_lGradePrice = xmlInitDoc.GetAttributeToLong("sGradePrice",&pChildNode);
	trs.m_nGrade = xmlInitDoc.GetAttributeToInt("sGrade",&pChildNode);
	trs.m_nLevel = xmlInitDoc.GetAttributeToInt("sLevel",&pChildNode);
	trs.m_lPump = xmlInitDoc.GetAttributeToLong("sPump",&pChildNode);
	trs.m_lMode = xmlInitDoc.GetAttributeToLong("sMode",&pChildNode);
	trs.m_lRes = l2PumpReservation(xmlInitDoc.GetAttributeToLong("sRes",&pChildNode));
	trs.m_lResExt = l2PumpResExt(xmlInitDoc.GetAttributeToLong("sResExt",&pChildNode));
	trs.m_lLinkNumber = xmlInitDoc.GetAttributeToLong("sLinkNumber",&pChildNode);
	trs.m_nLinkFlags = xmlInitDoc.GetAttributeToInt("sLinkFlags",&pChildNode);
	trs.m_lActiveReadNumber = xmlInitDoc.GetAttributeToLong("sActiveReadNumber",&pChildNode);
	trs.m_lActiveShiftNumber = xmlInitDoc.GetAttributeToLong("sActiveShiftNumber",&pChildNode);
	trs.m_lMisc = xmlInitDoc.GetAttributeToLong("sMisc",&pChildNode);

	xmlInitDoc.GetAttributeToByteArrWithConvert("sDateTime",sDateTime,&pChildNode);
	xmlInitDoc.GetAttributeToByteArrWithConvert("sPrePaidValue",tagTrs.sPrePaidValue,&pChildNode);
	xmlInitDoc.GetAttributeToByteArrWithConvert("sServiceMode",tagTrs.sServiceMode,&pChildNode);
	xmlInitDoc.GetAttributeToByteArrWithConvert("sStartDateTime",sStartDateTime,&pChildNode); //TD 38654

	bRetCode = xmlInitDoc.SelectSingleElementNode("SALES",&pChildNode);
	xmlInitDoc.GetChildNodesList(&pChildNode,&pItemElement);
	i=0;
	BYTE sTmpValue[11];
	BYTE sTmpVolume[11];
	SALE cTmpSale;

	//loop to create all the sale struct
	while(xmlInitDoc.GetNextItem(pItemElement,pChildNode))
	{
		memset(&cTmpSale , ' ', sizeof(cTmpSale));
		xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[212],cTmpSale.sProdDesc,&pChildNode, 1);
		xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[213],cTmpSale.sDPTProdCode,&pChildNode, 1);
		xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[214],cTmpSale.sQty,&pChildNode);
		xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[215],cTmpSale.sUnitPrice,&pChildNode);
		xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[216],cTmpSale.sDiscount,&pChildNode);
		xmlInitDoc.GetAttributeToByteArrWithConvert(m_strCName[217],cTmpSale.sTax,&pChildNode);

		memset(sTmpValue , 0, sizeof(sTmpValue));
		xmlInitDoc.GetAttributeToByteArrWithConvert("sValue",sTmpValue,&pChildNode);

		// it means that there is only one transaction
		if (trs.m_lRoundedValue == atol((char *)sTmpValue))
		{
			// only one transaction
			memcpy(sCardAllData.CardSaleAll.extData.SALES[0].sDPTProdCode , cTmpSale.sDPTProdCode , sizeof(cTmpSale.sDPTProdCode));
			memcpy(sCardAllData.extData2.sGradePrice , cTmpSale.sUnitPrice ,  sizeof(sCardAllData.extData2.sGradePrice));
			memcpy(sCardAllData.CardSaleAll.extData.SALES[0].sDiscount  , cTmpSale.sDiscount ,sizeof(cTmpSale.sDiscount));
		}
		else
		{

			// check if item is Reefer or diesel.
			if (trs.m_lPump)
			{
				//detect reefer
				if (_Module.m_server.m_cPumpArray[trs.m_lPump-1].GetReeferGrade() == a2l(cTmpSale.sDPTProdCode , sizeof(cTmpSale.sDPTProdCode)))
				{
					memcpy(sCardAllData.CardSaleAll.extData.SALES[3].sDPTProdCode , cTmpSale.sDPTProdCode , sizeof(cTmpSale.sDPTProdCode));

					// Transaction with both
					xmlInitDoc.GetAttributeToByteArrWithConvert("sVolume",sCardAllData.extData3.sReeferVolume,&pChildNode);
					xmlInitDoc.GetAttributeToByteArrWithConvert("sValue",sCardAllData.extData3.sReeferTotal,&pChildNode);
					memcpy(sCardAllData.CardSaleAll.extData.SALES[3].sUnitPrice  , cTmpSale.sUnitPrice ,sizeof(sCardAllData.CardSaleAll.extData.SALES[3].sUnitPrice));
					memcpy(sCardAllData.CardSaleAll.extData.SALES[3].sDiscount  , cTmpSale.sDiscount ,sizeof(sCardAllData.CardSaleAll.extData.SALES[3].sDiscount));
					memcpy(sCardAllData.CardSaleAll.extData.SALES[3].sProdDesc  , cTmpSale.sProdDesc ,sizeof(sCardAllData.CardSaleAll.extData.SALES[3].sProdDesc));


				}
				else if (_Module.m_server.m_cPumpArray[trs.m_lPump-1].GetDieselGrade() == a2l(cTmpSale.sDPTProdCode , sizeof(cTmpSale.sDPTProdCode)))
				{
					memcpy(sCardAllData.CardSaleAll.extData.SALES[0].sDPTProdCode , cTmpSale.sDPTProdCode , sizeof(sCardAllData.CardSaleAll.extData.SALES[i].sDPTProdCode));
					memcpy(sCardAllData.extData2.sGradePrice , cTmpSale.sUnitPrice ,  sizeof(sCardAllData.extData2.sGradePrice));
					memcpy(sCardAllData.CardSaleAll.extData.SALES[0].sDiscount  , cTmpSale.sDiscount ,sizeof(sCardAllData.CardSaleAll.extData.SALES[0].sDiscount));
					memcpy(sCardAllData.CardSaleAll.extData.SALES[0].sProdDesc  , cTmpSale.sProdDesc ,sizeof(sCardAllData.CardSaleAll.extData.SALES[0].sProdDesc));
				}
				else
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString sStr;
						sStr.Format("Parse XML not found item %.2s" ,cTmpSale.sDPTProdCode );
						_LOGMSG.LogMsg(sStr);
					}
				}
			}

		}


		memset(sTmpVolume , 0, sizeof(sTmpVolume));
		xmlInitDoc.GetAttributeToByteArrWithConvert("sValue",sTmpVolume,&pChildNode);

		i++;
	}

	i=0;
	int y=0;
	TRAVEL_CENTER_REC tmpTravelCenter;

	memset(&tmpTravelCenter,0,sizeof(TRAVEL_CENTER_REC));
	pvTravelCenter = (TRAVEL_CENTER_REC *)pvTravelCenter;
	bRetCode = xmlInitDoc.SelectSingleElementNode("DISCOUNTS",&pChildNode);
	if(bRetCode)
	{
		xmlInitDoc.GetChildNodesList(&pChildNode,&pItemElement);

		//loop to create all the sale struct
		while(xmlInitDoc.GetNextItem(pItemElement,pChildNode))
		{
			xmlInitDoc.GetChildNodesList(&pChildNode,&pItemElement2);
			while(xmlInitDoc.GetNextItem(pItemElement2,pChildNode3))
			{
				tmpTravelCenter.cItemTax[i][y].lTax= xmlInitDoc.GetAttributeToLong("sRate",&pChildNode3);
				xmlInitDoc.GetAttributeToByteArrWithConvert("sDescription",tmpTravelCenter.cItemTax[i][y].sTaxDescription,&pChildNode3);
				y++;
			}
			y=0;
			i++;
		}
	}
	i=0;
	TravelCenterData cTravelCenterData;
	// include promots
	bRetCode = xmlInitDoc.SelectSingleElementNode("PROMPTS",&pChildNode);

	if(bRetCode)
	{
		xmlInitDoc.GetChildNodesList(&pChildNode,&pItemElement);

		//loop to create all the sale struct
		while(xmlInitDoc.GetNextItem(pItemElement,pChildNode))
		{
			xmlInitDoc.GetAttributeToByteArrWithConvert("sPromptName0",cTravelCenterData.PromptName0,&pChildNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert("sPromptDataType0", cTravelCenterData.PromptDataType0,&pChildNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert("sPromptMaxLen0",   cTravelCenterData.PromptMaxLen0,&pChildNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert("sPromptMinLen0",   cTravelCenterData.PromptMinLen0,&pChildNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert("sPromptMode0",     cTravelCenterData.PromptMode0,&pChildNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert("sPromptSource0" ,    cTravelCenterData.PromptSource0,&pChildNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert("sPromptEncrypted0", cTravelCenterData.PromptEncrypted0,&pChildNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert("sPromptText0",     cTravelCenterData.PromptText0,&pChildNode);
			xmlInitDoc.GetAttributeToByteArrWithConvert("sPromptResult0",   cTravelCenterData.PromptResult,&pChildNode);

			tmpTravelCenter.cArrTravelCenterData[i] = cTravelCenterData;
			i++;
			memset(&cTravelCenterData,' ',sizeof(cTravelCenterData));
		}
	}

	memcpy(pvTravelCenter, &tmpTravelCenter, sizeof(TRAVEL_CENTER_REC));
	memcpy(pvStructOut, &sCardAllData, sizeof(CARD_SALE_ALL3));

	xmlInitDoc.CloseDoc();

	return 1;
}


/*
BOOL CCardSaleAllData::CreateXMLFromTaxStruct(TRAVEL_CENTER_TAX *psTravelCenterTax, char *pszXMLOut , long  &lNumber)
{
	BOOL bRetCode = TRUE;
	BYTE sTmp[11] ;

	CXMLInterface xmlInitDoc,xmlChildDoc;
	CXMLPointers  pCurrElement,pChildNode,pChildNode2,pChildNode3;


	bRetCode = xmlInitDoc.InitDoc();
	xmlInitDoc.CreateElement("Taxes",&pCurrElement);
	xmlInitDoc.AppendRootElement(&pCurrElement);

	xmlInitDoc.CreateElement("TaxKey",&pChildNode);

	memset(sTmp, 0 , sizeof(sTmp));

	sprintf_s((char *)sTmp ,sizeof(sTmp),"%02ld" ,lNumber);

	EndLineWithNull(&xmlInitDoc,&pChildNode,"ID", sTmp, min(strlen((char *)sTmp),sizeof(sTmp)));	//4.0.19.502
	xmlInitDoc.AppendChild(&pChildNode,&pCurrElement);
	xmlInitDoc.CreateElement("Item",&pChildNode2);


	memset(sTmp, 0 , sizeof(sTmp));
	l2a(psTravelCenterTax->lTax,sTmp ,sizeof(sTmp) - 1);
	EndLineWithNull(&xmlInitDoc,&pChildNode2,"TaxRate", sTmp,sizeof(sTmp));
	EndLineWithNull(&xmlInitDoc,&pChildNode2,"TaxDescription", psTravelCenterTax->sTaxDescription ,sizeof(psTravelCenterTax->sTaxDescription));

	xmlInitDoc.AppendChild(&pChildNode2,&pChildNode);


	CComBSTR	bstrXML;
	bstrXML.Empty();

	xmlInitDoc.GetXmlString(&bstrXML); //4.0.15.287
	char tmp[MAX_XML_BUF_LEN];
	memset(&tmp,0x00,sizeof(tmp));

	int iLen = WideCharToMultiByte(CP_ACP,0,bstrXML,-1,tmp,sizeof(tmp),NULL,NULL);
	strncpy_s(pszXMLOut,tmp);

	xmlInitDoc.CloseDoc();

	return bRetCode;
}
*/

/*
BOOL CCardSaleAllData::CreateListOfTAXStruct(char *pszXMLOut, TRAVEL_CENTER_TAX psTravelCenterTax[], int NumOfData)
{
	long		i=0;
	BOOL    bRetCode = TRUE;
	char	sTmpXml[10000];
	CXMLInterface	xmlInitDoc,xmlChildDoc;
	CXMLPointers	pCurrElement,pChildNode,pChildNode2;

	xmlInitDoc.InitDoc();
	xmlInitDoc.CreateElement("PromtPilot",&pCurrElement);
	xmlInitDoc.AppendRootElement(&pCurrElement);

	xmlChildDoc.InitDoc();


	for (i=1;i<=NumOfData;i++)
	{
		CreateXMLFromTaxStruct(&psTravelCenterTax[i] ,sTmpXml , i);

		xmlChildDoc.LoadXmlString(sTmpXml);
		xmlChildDoc.GetRootNode(&pChildNode);
		xmlInitDoc.AppendChildElement(&pChildNode,&pCurrElement);
		pChildNode.m_pMSElement = NULL;
		pChildNode.m_pMSNode = NULL;
	}

	CComBSTR	bstrXML;
	bstrXML.Empty();

	xmlInitDoc.GetXmlString(&bstrXML); //4.0.15.287
	char tmp[MAX_XML_BUF_LEN];
	memset(&tmp,0x00,sizeof(tmp));

	int iLen = WideCharToMultiByte(CP_ACP,0,bstrXML,-1,tmp,sizeof(tmp),NULL,NULL);
	strncpy_s(pszXMLOut,tmp);

	xmlChildDoc.CloseDoc();
	xmlInitDoc.CloseDoc();

	return bRetCode;
}
*/

// Create PAP decine XML
long CCardSaleAllData::CreateFleetPAKXML(CPumpTransact &cTrs, char *&outXML)
{
	PAY_AT_PUMP_INFO cPatAtPumpInfo;
	TRAVEL_CENTER_REC cTravelCenterRec;  //4.0.9999.
	CString sMsg;
	long	lRtc = 0;

	memset(&cPatAtPumpInfo,' ',sizeof(PAY_AT_PUMP_INFO));
	memset(&cTravelCenterRec ,' ' , sizeof(cTravelCenterRec));

	lRtc = _Module.m_server.IndexOperation(PUMP_OLA_TEMP_FILE , REC_READ, &cTrs ,&cPatAtPumpInfo);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		sMsg.Format("Fleet PAK , reading ola temp Index=%ld retCode=%ld" , cTrs.m_lNumber, lRtc );
		_LOGMSG.LogMsg(sMsg);
	}

	if (!lRtc)
	{
		cTravelCenterRec.lIndex = a2l(cPatAtPumpInfo.CardSaleInfo.cardData.sTranNbr, sizeof(cPatAtPumpInfo.CardSaleInfo.cardData.sTranNbr)); //4.0.10.49

		PS::RECEIPT_EXTRA_INFO_REC rec;
		memcpy(&rec, &cTravelCenterRec, min(sizeof(rec), sizeof(cTravelCenterRec)));
		lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, rec);
		//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cTravelCenterRec);


		if(LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("Fleet PAK , reading prompts & tax Index=%ld retCode=%ld" , cTravelCenterRec.lIndex, lRtc );
			_LOGMSG.LogMsg(sMsg);
		}

		if (lRtc)
			CreateXMLFromStruct(outXML, 0 ,(CARD_SALE_ALL3 *)&cPatAtPumpInfo.CardSaleInfo.cardData,cTrs); //4.0.10.49
		else
			CreateXMLFromStruct(outXML,0,(CARD_SALE_ALL3 *)&cPatAtPumpInfo.CardSaleInfo.cardData, cTrs, &cTravelCenterRec);

		lRtc = 0;
	}
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("Fail retrived transaction=%ld retCode=%ld" , cTrs.m_lNumber, lRtc );
			_LOGMSG.LogMsg(sMsg);
		}

	}

	return lRtc; //4.0.10.14
}

// Create PAP XML
long CCardSaleAllData::CreateFleetPAPXML(CPumpTransact &cTrs, PAY_AT_PUMP_INFO & PayAtPumpInfo ,char *&outXML)  //4.0.10.492
{
	CString sMsg;
	long	lRtc = 0;
	TRAVEL_CENTER_REC cTravelCenterRec;  //4.0.9999.0
	memset(&cTravelCenterRec ,' ' , sizeof(cTravelCenterRec));

	try
	{
		if(_Module.m_server.GetCreateFleetRecord())
		{
			cTravelCenterRec.lIndex = a2l(PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr, sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr)); //4.0.10.49

			PS::RECEIPT_EXTRA_INFO_REC rec;
			memcpy(&rec, &cTravelCenterRec, min(sizeof(rec), sizeof(cTravelCenterRec)));
			lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, rec);
			//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_READ,&cTravelCenterRec);


			if(LOG_BASIC_FLOW_CONTROL)
			{
				sMsg.Format("Fleet PAK , reading prompts & tax Index=%ld retCode=%ld" , cTravelCenterRec.lIndex, lRtc );
				_LOGMSG.LogMsg(sMsg);
			}
		}
		else
		{
			lRtc = 1; //  Not OK
		}


		if (lRtc)
			CreateXMLFromStruct(outXML, PayAtPumpInfo.CardSaleInfo.lIndexNumber,(CARD_SALE_ALL3 *)&PayAtPumpInfo.CardSaleInfo.cardData,cTrs );  //4.0.10.492
		else
			CreateXMLFromStruct(outXML,PayAtPumpInfo.CardSaleInfo.lIndexNumber ,(CARD_SALE_ALL3 *)&PayAtPumpInfo.CardSaleInfo.cardData, cTrs, &cTravelCenterRec); //4.0.10.492
	}
	catch(...)   //4.0.10.49
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("PumpNumber %02ld, Trs %ld Faile create XML for PAP transaction" ,  cTrs.m_lPump ,cTrs.m_lNumber );
			_LOGMSG.LogMsg(sMsg);
		}

	}

	return 0;
}

BOOL CCardSaleAllData::CreatePakTrsFromXML(char* sInXML, LONG& lNewTrsNumber, BOOL bCreateNewTransactionNumber /* = FALSE */) //CR 351687: Coles Enhanced post void transaction
{
	BOOL bRetCode = TRUE;
	char str[5000];
	TRAVEL_CENTER_REC cTravelCenterRec;
	PAY_AT_PUMP_INFO cPatAtPumpInfo;
	CARD_SALE_ALL3 cCardSaleAll3;
	CPumpTransact trs;
	long lRtc;
	CString sMsg;


	memset(&cPatAtPumpInfo,' ',sizeof(cPatAtPumpInfo));
	memset(&cCardSaleAll3, ' ', sizeof(cCardSaleAll3));
	memset(&str,' ',sizeof(str));
	memset(&cTravelCenterRec,' ',sizeof(cTravelCenterRec));
	memset(&trs,' ',sizeof(trs));

	CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

	try
	{
		_Module.m_server.m_cCardSaleDataXML.CreateStructFromXML(&cCardSaleAll3, &cTravelCenterRec, trs, sInXML); // CID_199605 Out-of-bounds access
		if (bCreateNewTransactionNumber)  //CR 351687: Coles Enhanced post void transaction
		{
			lNewTrsNumber = trs.m_lNumber = _Module.m_server.NewTransactNumber();
		}
		_Module.m_server.m_cCommonFunction.ConvertCardSaleAll3ToPayAtPumpInfo(cPatAtPumpInfo, cCardSaleAll3);
		lRtc = _Module.m_server.IndexOperation(PUMP_OLA_TEMP_FILE,REC_INSERT, &trs, &cPatAtPumpInfo);
		if(LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("CreatePakTrsFromXML Insert into OLA TEMP Qdx finise with result = %ld",lRtc);
			_LOGMSG.LogMsg(sMsg);
		}
		if(!(lRtc))
		{
			cTravelCenterRec.lIndex = a2l(cPatAtPumpInfo.CardSaleInfo.cardData.sTranNbr, sizeof(cPatAtPumpInfo.CardSaleInfo.cardData.sTranNbr));
			PS::RECEIPT_EXTRA_INFO_REC rec;
			memcpy(&rec, &cTravelCenterRec, min(sizeof(rec), sizeof(cTravelCenterRec)));
			lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_INSERT, rec);
			//lRtc = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_INSERT,&cTravelCenterRec);

			if(LOG_BASIC_FLOW_CONTROL)
			{
				sMsg.Format("CreatePakTrsFromXML Insert into RECEIPT_EXTRA Qdx finish with result = %ld",lRtc);
				_LOGMSG.LogMsg(sMsg);
			}
		}


		if(!(lRtc))
		{
			trs.SetLink(TRS_NO_LINK,0);  //4.0.10.310
			trs.SetOnOffer(0);
			trs.SetStatus(UNPAID);
			trs.m_lRes = PAY_AT_KIOSK;
			trs.m_lResExt = RES_EXT_FLEET;

			lRtc = _Module.m_server.CentralInsertTrs(trs.m_lPump,&trs ,TRUE);
			if(LOG_BASIC_FLOW_CONTROL)
			{
				sMsg.Format("CreatePakTrsFromXML Insert into PUMP_STAT Qdx finish with result = %ld",lRtc);
				_LOGMSG.LogMsg(sMsg);
			}
		}
	}
	catch(...)
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("CreatePakTrsFromXML  Failed create Strcut from XML");
			_LOGMSG.LogMsg(sMsg);
		}
	}

	return lRtc;
}

void CCardSaleAllData::BuildLimitsToCardSaleXML(CXMLInterface &xmlInitDoc,  CARD_SALE_ALL3 * pvStructIn, CXMLPointers & pCurrElement)
{
	CXMLPointers pTmpNode,pLimitsNode;
	char	sLineID[5];

	xmlInitDoc.CreateElement("LIMITS",&pLimitsNode);
	xmlInitDoc.AppendChild(&pLimitsNode,&pCurrElement);

	for(int i=0;i<MAX_POSITIONS_PER_PUMP;i++)
	{
		sprintf_s(sLineID,"%ld",i +1);//get the line id
		xmlInitDoc.CreateElement("LIMIT",&pTmpNode);
		xmlInitDoc.SetAttribute("ID",sLineID,&pTmpNode);

		AddLineWithNull(&xmlInitDoc,&pTmpNode,"sGrade",pvStructIn->extData3.GradeLimitsArray[i].sGradeID
			,sizeof(pvStructIn->extData3.GradeLimitsArray[i].sGradeID));

		AddLineWithNull(&xmlInitDoc,&pTmpNode,"sAmountLimit",pvStructIn->extData3.GradeLimitsArray[i].sGradeAmountLimit
			,sizeof(pvStructIn->extData3.GradeLimitsArray[i].sGradeAmountLimit));

		AddLineWithNull(&xmlInitDoc,&pTmpNode,"sLimitType",&pvStructIn->extData3.GradeLimitsArray[i].sLimitType
			,sizeof(pvStructIn->extData3.GradeLimitsArray[i].sLimitType));

		AddLineWithNull(&xmlInitDoc,&pTmpNode,m_strCName[218],pvStructIn->extData2.sDiscountPerGrade[i].sDiscount
			,sizeof(pvStructIn->extData2.sDiscountPerGrade[i].sDiscount));

		xmlInitDoc.AppendChild(&pTmpNode,&pLimitsNode);
	}
}

void CCardSaleAllData::AttachFuelTaxesToCardSaleDataXml( CXMLInterface &xmlInitDoc, CARD_SALE_ALL3 * pCardSaleAll3,CXMLPointers pCurrElement ,long lGrade)
{
	CXMLPointers pTaxesNode;
	TAX_PROPERTIES cTaxPropertis;

	long lTotalTaxAmount = 0;

	xmlInitDoc.CreateElement("TAXES",&pTaxesNode);
	xmlInitDoc.AppendChild(&pTaxesNode,&pCurrElement);

	DWORD dwGardeTaxMap = (DWORD)_Module.m_server.GetGradeTaxLink((short)lGrade);

	int iTaxNumber = 1;
	for(int iTaxIndex=0;iTaxIndex<MAX_TAXES;iTaxIndex++)
	{
		if(dwGardeTaxMap & (DWORD)(1<<iTaxIndex))
		{
			CXMLPointers pTaxNode;
			xmlInitDoc.CreateElement("TAX",&pTaxNode);

			_Module.m_server.FillTaxDetails(iTaxIndex,cTaxPropertis);

			AddLineWithNull(&xmlInitDoc,&pTaxNode,"Factor",(BYTE *)"1000",sizeof("1000"));

			AddLineWithNull(&xmlInitDoc,&pTaxNode,"PrintChar",(BYTE *)&cTaxPropertis.sTaxPrintChar,sizeof(cTaxPropertis.sTaxPrintChar));

			long lPumpNumber = a2l(pCardSaleAll3->CardSaleAll.data.sPumpNumber,sizeof(pCardSaleAll3->CardSaleAll.data.sPumpNumber));

			long lTaxAmount = _Module.m_server.GetFuelTaxAmountFormCardSaleData(lPumpNumber-1,iTaxNumber);

			lTotalTaxAmount +=lTaxAmount;

			BYTE sTaxAmount[10] = {0};
			l2a(lTaxAmount,sTaxAmount,sizeof(sTaxAmount));
			AddLineWithNull(&xmlInitDoc,&pTaxNode,"Value",sTaxAmount,sizeof(sTaxAmount));


			BYTE sTaxRate[5];
			l2a(cTaxPropertis.lTaxRate, sTaxRate, 5);
			AddLineWithNull(&xmlInitDoc, &pTaxNode, "Rate", sTaxRate, 5);


			BYTE sTaxId[5];
			l2a(cTaxPropertis.lTaxId,sTaxId,5);
			AddLineWithNull(&xmlInitDoc,&pTaxNode,"Id",sTaxId,5);

			AddLineWithNull(&xmlInitDoc,&pTaxNode,"Name",(BYTE *)cTaxPropertis.sTaxName,sizeof(cTaxPropertis.sTaxName));

			xmlInitDoc.AppendChild(&pTaxNode,&pTaxesNode);

			iTaxNumber++;
		}
	}


	BYTE sTotalTaxAmout[10] = {0};
	l2a(lTotalTaxAmount,sTotalTaxAmout,sizeof(sTotalTaxAmout));
	AddLineWithNull(&xmlInitDoc,&pTaxesNode,"sTotalTaxAmount",sTotalTaxAmout,sizeof(sTotalTaxAmout));
}

void CCardSaleAllData::AttachCarWashTaxesToCardSaleDataXml( CXMLInterface &xmlInitDoc, CARD_SALE_ALL3 * pCardSaleAll3, CXMLPointers &pCurrElement )
{
	TAX_PROPERTIES cTaxPropertis;
	CXMLPointers pTaxesNode;
	xmlInitDoc.CreateElement("TAXES",&pTaxesNode);
	xmlInitDoc.AppendChild(&pTaxesNode,&pCurrElement);

	long lDPTProdCode = a2l(&pCardSaleAll3->CardSaleAll.extData.sCarWashProgram,sizeof(pCardSaleAll3->CardSaleAll.extData.SALES[0].sDPTProdCode)); //4.0.10.230
	long lCarWashIndex = _Module.m_server.GetWashProgramIndex(lDPTProdCode);
	DWORD dWashProgramTaxLink = _Module.m_server.GetWashProgramTaxLink(lCarWashIndex);
	DWORD dWashProgramPrice =   _Module.m_server.GetWashProgramPrice(lCarWashIndex);

	for(int i=0;i<MAX_TAXES;i++)
	{
		if(dWashProgramTaxLink & (DWORD)(1<<i))
		{
			CXMLPointers pTaxNode;
			xmlInitDoc.CreateElement("TAX",&pTaxNode);

			_Module.m_server.FillTaxDetails(i,cTaxPropertis);

			AddLineWithNull(&xmlInitDoc,&pTaxNode,"Factor",(BYTE *)"1000",sizeof("1000"));

			AddLineWithNull(&xmlInitDoc,&pTaxNode,"PrintChar",(BYTE *)&cTaxPropertis.sTaxPrintChar,sizeof(cTaxPropertis.sTaxPrintChar));

			double dTaxRate  = (double)cTaxPropertis.lTaxRate/100000.0;
			long lCarWashTaxAmount = (long)(dWashProgramPrice * dTaxRate);

			BYTE sTaxAmount[10] = {0};
			l2a(lCarWashTaxAmount,sTaxAmount,sizeof(sTaxAmount));
			AddLineWithNull(&xmlInitDoc,&pTaxNode,"Value",sTaxAmount,sizeof(sTaxAmount));

			BYTE sTaxRate[5];
			l2a(cTaxPropertis.lTaxRate,sTaxRate,5);
			AddLineWithNull(&xmlInitDoc,&pTaxNode,"Rate",sTaxRate,5);

			BYTE sTaxId[5];
			l2a(cTaxPropertis.lTaxId,sTaxId,5);
			AddLineWithNull(&xmlInitDoc,&pTaxNode,"Id",sTaxId,5);

			AddLineWithNull(&xmlInitDoc,&pTaxNode,"Name",(BYTE *)cTaxPropertis.sTaxName,sizeof(cTaxPropertis.sTaxName));

			xmlInitDoc.AppendChild(&pTaxNode,&pTaxesNode);
		}
	}

	AddLineWithNull(&xmlInitDoc,&pTaxesNode,"sTotalTaxAmount",pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax,sizeof(pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax));
	xmlInitDoc.AppendChild(&pTaxesNode,&pCurrElement);
}

void CCardSaleAllData::AttachCarWashItemToCardSaleDataXml( long lCarWashSaleIndex, CXMLInterface &xmlInitDoc, CXMLPointers pChildNode4,  CARD_SALE_ALL3 * pvStructIn, long lDiscountCarwash, CXMLPointers pChildNode9 )
{
	if (a2l(&pvStructIn->CardSaleAll.extData.sCarWashProgram,sizeof(pvStructIn->CardSaleAll.extData.sCarWashProgram)) == 0)
	{
		return;
	}

	DWORD dwTmp = 0;
	char sTmpSaleBuffer[100] = {0};

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lCarWashSaleIndex);
	xmlInitDoc.CreateElement("SALE",&pChildNode4);
	xmlInitDoc.SetAttribute("ID",sTmpSaleBuffer,&pChildNode4);

	CString sWashProgram;
	sWashProgram.Format("WashPrograms\\WashProgram%02d",a2l(&pvStructIn->CardSaleAll.extData.sCarWashProgram,sizeof(pvStructIn->CardSaleAll.extData.sCarWashProgram))); //4.0.10.230
	char sFullName[MAX_FIELD_VAL] = {0};

	//BOOL bOldCarWash = FALSE;

	//if(bOldCarWash)
	//{
		//_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sWashProgram), "FullName",sFullName,(DWORD)10,FALSE,FALSE,0,HOP_DIRECT_LOCAL,TRUE);
	//}
	//else
	{
		memcpy(sFullName,pvStructIn->CardSaleAll.extData.SALES[0].sProdDesc,sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sProdDesc));
	}
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sProdDesc", (BYTE *)sFullName, sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sProdDesc));

	//if(bOldCarWash)
	//{
		//_Module.m_server.m_cParam.LoadParam((char *)"CarWash", "CarWashCode",&dwTmp,102L,FALSE); //102 - NACS code for car wash
		//sprintf_s((char *)sTmpSaleBuffer,"%ld",dwTmp);
	//}
	//else
	{
		AddLineWithNull(&xmlInitDoc,&pChildNode4,"sDPTProdCode", (BYTE *)"10", sizeof("10"));	//4.0.19.502
	}


	long lQtyCarwash = a2l((BYTE *)&pvStructIn->CardSaleAll.extData.SALES[0].sQty,sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sQty));
	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lQtyCarwash);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sQty", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	long lUnitPrice = a2l((BYTE *)&pvStructIn->CardSaleAll.extData.SALES[0].sUnitPrice,sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sUnitPrice)) *(_Module.m_server.GetCarWashScalarFactor());
	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lUnitPrice);
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sUnitPrice", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lDiscountCarwash);
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sDiscount", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",0);
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sVolume", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	long lAmountCarwash = 0;
	//if(bOldCarWash)
		 //lAmountCarwash = a2l((BYTE *)&pvStructIn->CardSaleAll.extData.SALES[0].sUnitPrice,sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sUnitPrice)) * 10;
	//else
	{
		lAmountCarwash = a2l((BYTE *)&pvStructIn->CardSaleAll.extData.SALES[0].sUnitPrice,sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sUnitPrice)) * lQtyCarwash;
	}
	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lAmountCarwash);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sValue", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	sprintf_s((char *)sTmpSaleBuffer, sizeof(sTmpSaleBuffer), "%ld", 2);
	AddLineWithNull(&xmlInitDoc, &pChildNode4, "sValueFactor", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer), sizeof(sTmpSaleBuffer)));	// RFUEL-2834


	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%.11s",pvStructIn->extData6.CarWashItem.sBarcode);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sCarWashBarcode", (unsigned char *)sTmpSaleBuffer, sizeof(pvStructIn->extData6.CarWashItem.sBarcode));

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%.2s",pvStructIn->extData6.CarWashItem.sWeatherGuarentee);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sCarWashWeatherGuarentee", (unsigned char *)sTmpSaleBuffer,sizeof(pvStructIn->extData6.CarWashItem.sWeatherGuarentee));

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%.20s",pvStructIn->extData6.CarWashItem.sSku);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sCarWashSku", (unsigned char *)sTmpSaleBuffer,sizeof(pvStructIn->extData6.CarWashItem.sSku));

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%.15s",pvStructIn->extData6.CarWashItem.sUpc);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sCarWashUpc", (unsigned char *)sTmpSaleBuffer,sizeof(pvStructIn->extData6.CarWashItem.sUpc));

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%.2s",pvStructIn->extData6.CarWashItem.sUnitsPerDay);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sCarWashUnitsPerDay", (unsigned char *)sTmpSaleBuffer,sizeof(pvStructIn->extData6.CarWashItem.sUnitsPerDay));

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%.3s",pvStructIn->extData6.CarWashItem.sVaildDays);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sCarWashValidDays", (unsigned char *)sTmpSaleBuffer,sizeof(pvStructIn->extData6.CarWashItem.sVaildDays));

	AddLineWithNull(&xmlInitDoc, &pChildNode4, "PosSequence", (unsigned char *)"1", sizeof("1"));	// TD 405456


	xmlInitDoc.AppendChild(&pChildNode4,&pChildNode9);

	AttachCarWashTaxesToCardSaleDataXml(xmlInitDoc, pvStructIn, pChildNode4);
}

void CCardSaleAllData::AttachFuelItemToCardSaleDataXml( long lValue, long lAmountCarwash, long lDiscountCarwash, CARD_SALE_ALL3 * pvStructIn,  CXMLInterface &xmlInitDoc, CXMLPointers pChildNode4,  CPumpTransact &trs, long lGradePrice, long lVolume, CXMLPointers pChildNode9 )
{
	CString sGradeTable;
	char sTmpSaleBuffer[100] = {0};
	char sShortName5[MAX_FIELD_VAL] = {0};//4.0.10.310

	//fuel  regular
	long lFuelSale = lValue + lDiscountCarwash - lAmountCarwash;  // RFUEL-2801
	long lItemCount = 1L;

	if ( a2l(&pvStructIn->CardSaleAll.extData.sNumberOfExtraSale,sizeof(pvStructIn->CardSaleAll.extData.sNumberOfExtraSale)) )
	{
		//We got car wash
		lItemCount++;
		//Check if also got discount
		if (a2l((BYTE *)&pvStructIn->CardSaleAll.extData.SALES[0].sDiscount,sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sDiscount)) )
		{
			lItemCount++;
		}
	}

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",1);//get the line id
	xmlInitDoc.CreateElement("SALE",&pChildNode4);
	xmlInitDoc.SetAttribute("ID",sTmpSaleBuffer,&pChildNode4);

	memset(sShortName5 , 0 , sizeof(sShortName5));

	sGradeTable.Format("Grades\\Grade%02d",trs.m_nGrade); //4.0.10.230
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "ShortName5",sShortName5,(DWORD)10,FALSE,FALSE,0,HOP_DIRECT_LOCAL,TRUE);

	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sProdDesc", (unsigned char *)sShortName5,sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sProdDesc));

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%d",trs.m_nGrade);//get the line id //4.0.10.811
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sDPTProdCode", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",1);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sQty", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lGradePrice);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sUnitPrice", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	AddLineWithNull(&xmlInitDoc, &pChildNode4, "sDiscount", pvStructIn->CardSaleAll.extData.sGRTotalDiscount, sizeof(pvStructIn->CardSaleAll.extData.sGRTotalDiscount));

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lVolume);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sVolume", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502


	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lFuelSale);//get the line id
	AddLineWithNull(&xmlInitDoc,&pChildNode4,"sValue", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	if (a2l(&pvStructIn->CardSaleAll.extData.sCarWashProgram, sizeof(pvStructIn->CardSaleAll.extData.sCarWashProgram)) == 0)
		AddLineWithNull(&xmlInitDoc, &pChildNode4, "PosSequence", (unsigned char *)"1", sizeof("1"));	// TD 405456
	else
		AddLineWithNull(&xmlInitDoc, &pChildNode4, "PosSequence", (unsigned char *)"2", sizeof("2"));	// TD 405456


	xmlInitDoc.AppendChild(&pChildNode4,&pChildNode9);

	AttachFuelTaxesToCardSaleDataXml(xmlInitDoc, pvStructIn, pChildNode4,trs.m_nGrade);
}

void CCardSaleAllData::WriteTransactionXml( CXMLInterface &xmlInitDoc )
{
	try
	{
		if(LOG_FCC_FLOW_CONTROL)
		{
			CString sFileNameAndPath = _Module.m_server.m_cLogger.GetDefaultLogPath() + "\\fuel_pap_trs.xml";
			xmlInitDoc.SaveToFile(sFileNameAndPath.GetBuffer());
		}
	}
	catch(...)
	{
		_LOGMSG.LogMsg("Unable to write Transaction Xml file!");
	}
}

void CCardSaleAllData::CreateDieselSale( CPumpTransact &trs, CARD_SALE_ALL3 * pvStructIn, CXMLInterface &xmlInitDoc,  CXMLPointers pChildNode9, long &lCarWashIndex, long lValue, long lAmountCarwash, long lDiscountCarwash, long &lVolume, long &lGradePrice )
{
	DWORD dwTmp = 0;     //4.0.10.310
	CString sGradeTable; //4.0.10.310
	char sShortName5[MAX_FIELD_VAL] = {0};//4.0.10.310
	char sTmpSaleBuffer[100] = {0};
	CXMLPointers pSaleNode;

	long lReeferValue = a2l(pvStructIn->extData3.sReeferTotal , sizeof(pvStructIn->extData3.sReeferTotal));
	long lReeferVolume = a2l(pvStructIn->extData3.sReeferVolume , sizeof(pvStructIn->extData3.sReeferVolume));
	long lReeferPrice = a2l(pvStructIn->CardSaleAll.extData.SALES[3].sUnitPrice, sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sUnitPrice));

	long lGrade = trs.m_nGrade;

	//Reefer sales/
	if (a2l(pvStructIn->CardSaleAll.extData.SALES[3].sDPTProdCode ,sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sDPTProdCode)))
	{

		sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",2);//get the line id
		xmlInitDoc.CreateElement("SALE",&pSaleNode);
		xmlInitDoc.SetAttribute("ID",(char *)sTmpSaleBuffer,&pSaleNode);

		if (ChrAll((char *)pvStructIn->CardSaleAll.extData.SALES[3].sProdDesc ,
			sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sProdDesc)))
		{
			if (a2l(pvStructIn->CardSaleAll.extData.SALES[3].sDPTProdCode , sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sDPTProdCode)))
			{
				memset(sShortName5 , 0 , sizeof(sShortName5));

				sGradeTable.Format("Grades\\Grade%02d",a2l(pvStructIn->CardSaleAll.extData.SALES[3].sDPTProdCode ,sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sDPTProdCode))); //4.0.10.230

				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "ShortName5",sShortName5,(DWORD)10,FALSE,FALSE,0,HOP_DIRECT_LOCAL,TRUE);

				AddLineWithNull(&xmlInitDoc,&pSaleNode,"sProdDesc", (unsigned char *)sShortName5,sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sProdDesc));
			}

		}
		else
			AddLineWithNull(&xmlInitDoc,&pSaleNode,"sProdDesc", pvStructIn->CardSaleAll.extData.SALES[3].sProdDesc,sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sProdDesc));

		AddLineWithNull(&xmlInitDoc,&pSaleNode,"sDPTProdCode", pvStructIn->CardSaleAll.extData.SALES[3].sDPTProdCode,sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sDPTProdCode));
		sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",1);//get the line id
		AddLineWithNull(&xmlInitDoc,&pSaleNode,"sQty", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

		AddLineWithNull(&xmlInitDoc,&pSaleNode,"sUnitPrice", pvStructIn->CardSaleAll.extData.SALES[3].sUnitPrice,sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sUnitPrice));
		AddLineWithNull(&xmlInitDoc,&pSaleNode,"sDiscount", pvStructIn->CardSaleAll.extData.SALES[3].sDiscount,sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sDiscount));
		sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lReeferVolume);//get the line id
		AddLineWithNull(&xmlInitDoc,&pSaleNode,"sVolume", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

		sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lReeferValue);//get the line id
		AddLineWithNull(&xmlInitDoc,&pSaleNode,"sValue", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502
		AddLineWithNull(&xmlInitDoc,&pSaleNode,"sTax", pvStructIn->CardSaleAll.extData.SALES[3].sTax,sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sTax));

		xmlInitDoc.AppendChild(&pSaleNode,&pChildNode9);

		lGrade = a2l(pvStructIn->CardSaleAll.extData.SALES[0].sDPTProdCode, sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sDPTProdCode));
		lCarWashIndex = 3;
	}


	//fuel 1
	long lFuelSale = lValue - lReeferValue - lAmountCarwash + lDiscountCarwash;  //4.0.10.310
	lVolume -= lReeferVolume;

	if (a2l(pvStructIn->CardSaleAll.extData.SALES[3].sDPTProdCode ,sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sDPTProdCode)))
		lGradePrice = a2l(pvStructIn->extData2.sGradePrice, sizeof(pvStructIn->extData2.sGradePrice)); //4.0.10.59


	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",1);//get the line id
	xmlInitDoc.CreateElement("SALE",&pSaleNode);
	xmlInitDoc.SetAttribute("ID",sTmpSaleBuffer,&pSaleNode);

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lGrade);//get the line id
	AddLineWithNull(&xmlInitDoc,&pSaleNode,"sDPTProdCode", (unsigned char *)sTmpSaleBuffer,(long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	if (ChrAll((char *)pvStructIn->CardSaleAll.extData.SALES[0].sProdDesc ,
		sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sProdDesc)))
	{
		char sShortName5[MAX_FIELD_VAL];

		if (lGrade)
		{
			memset(sShortName5 , 0 , sizeof(sShortName5));

			sGradeTable.Format("Grades\\Grade%02ld",lGrade);  //4.0.11.141

			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "ShortName5",sShortName5,(DWORD)10,FALSE,FALSE,0,HOP_DIRECT_LOCAL,TRUE);

			AddLineWithNull(&xmlInitDoc,&pSaleNode,"sProdDesc", (unsigned char *)sShortName5,sizeof(pvStructIn->CardSaleAll.extData.SALES[3].sProdDesc));
		}

	}
	else
		AddLineWithNull(&xmlInitDoc,&pSaleNode,"sProdDesc", pvStructIn->CardSaleAll.extData.SALES[0].sProdDesc,sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sProdDesc));


	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",1);//get the line id
	AddLineWithNull(&xmlInitDoc,&pSaleNode,"sQty", (unsigned char *)sTmpSaleBuffer,(long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lGradePrice);//get the line id
	AddLineWithNull(&xmlInitDoc,&pSaleNode,"sUnitPrice", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	AddLineWithNull(&xmlInitDoc,&pSaleNode,"sDiscount", pvStructIn->CardSaleAll.extData.SALES[0].sDiscount,sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sDiscount));

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lVolume);//get the line id
	AddLineWithNull(&xmlInitDoc,&pSaleNode,"sVolume", (unsigned char *)sTmpSaleBuffer, (long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502

	sprintf_s((char *)sTmpSaleBuffer,sizeof(sTmpSaleBuffer),"%ld",lFuelSale);//get the line id
	AddLineWithNull(&xmlInitDoc,&pSaleNode,"sValue", (unsigned char *)sTmpSaleBuffer,(long)min(strlen(sTmpSaleBuffer),sizeof(sTmpSaleBuffer)));	//4.0.19.502
	AddLineWithNull(&xmlInitDoc,&pSaleNode,"sTax", pvStructIn->CardSaleAll.extData.SALES[0].sTax,sizeof(pvStructIn->CardSaleAll.extData.SALES[0].sTax));

	xmlInitDoc.AppendChild(&pSaleNode,&pChildNode9);
}

BOOL CCardSaleAllData::AppendLoyaltyMessagesToXmlObject(long lPumpNumber, CXMLInterface & pRoot)
{
	CXMLInterface xmlInitDoc, xmlChildDoc, xmlChildDoc2;
	CXMLPointers  pCurrElement, pPrintMessagesNode, pPrintMessageNode, pTextNode, pReceiptPrintings, pRootChild, pAllPromNode;
	long bRetCode = 0;

	static BOOL bInit = FALSE;
	if (FALSE == bInit)
	{
		memset(m_szDeclineMsg, 0, sizeof(m_szDeclineMsg));
		char sDefaultMsg[MAX_FIELD_VAL] = "This card must be registered to earn and spend points and credits. See cashier.";
		_Module.m_server.m_cParam.LoadParam("Receipt", "NonMemberReceiptMessage", m_szDeclineMsg, sizeof(m_szDeclineMsg), sDefaultMsg, TRUE);
		bInit = TRUE;
	}


	if (_Module.m_server.IsMemberCardAccepted(lPumpNumber) == FALSE)
	{
		bRetCode = xmlInitDoc.InitDoc();
		if (pRoot.SelectSingleElementNode("AllProm//ReceiptPrinting", &pReceiptPrintings))
		{
			xmlInitDoc.CreateElement("PrintMessages", &pPrintMessagesNode);
			EndLineWithNull(&xmlInitDoc, &pPrintMessagesNode, "PrintingType", (unsigned char *)"NonMember", 9);
			xmlInitDoc.CreateElement("PrintMessage", &pPrintMessageNode);
			xmlInitDoc.AppendChild(&pPrintMessageNode, &pPrintMessagesNode);
			xmlInitDoc.CreateElement("Text", &pTextNode);
			EndLineWithNull(&xmlInitDoc, &pTextNode, "Text", (unsigned char *)m_szDeclineMsg, min(strlen(m_szDeclineMsg), sizeof(m_szDeclineMsg)));
			xmlInitDoc.AppendChild(&pTextNode, &pPrintMessageNode);

			pRoot.AppendChild(&pPrintMessagesNode, &pReceiptPrintings);
		}
		else
		{
			BOOL bRet = pRoot.SelectSingleElementNode("AllProm", &pAllPromNode);
			xmlInitDoc.CreateElement("ReceiptPrinting", &pCurrElement);
			xmlInitDoc.AppendRootElement(&pCurrElement);

			xmlInitDoc.CreateElement("PrintMessages", &pPrintMessagesNode);
			EndLineWithNull(&xmlInitDoc, &pPrintMessagesNode, "PrintingType", (unsigned char *)"NonMember", 9);
			xmlInitDoc.AppendChild(&pPrintMessagesNode, &pCurrElement);
			xmlInitDoc.CreateElement("PrintMessage", &pPrintMessageNode);
			xmlInitDoc.AppendChild(&pPrintMessageNode, &pPrintMessagesNode);
			xmlInitDoc.CreateElement("Text", &pTextNode);
			EndLineWithNull(&xmlInitDoc, &pTextNode, "Text", (unsigned char *)m_szDeclineMsg, min(strlen(m_szDeclineMsg), sizeof(m_szDeclineMsg)));
			xmlInitDoc.AppendChild(&pTextNode, &pPrintMessageNode);

			pRoot.AppendChild(&pCurrElement);
		}

		xmlInitDoc.CloseDoc();
	}

	return TRUE;
}

 void CCardSaleAllData::CreateTendersData(CXMLInterface &xmlInitDoc, CXMLPointers & pCurrElement, CARD_SALE_ALL3 * pvStructIn) //CR 404913
{
	CXMLPointers pTendersNode, pParentNode;
	char sTmpSaleBuffer[100] = { 0 };
	char sLineID[5] = { 0 };
	int iIndex = 0;

	xmlInitDoc.CreateElement("TENDERS", &pParentNode);
	xmlInitDoc.AppendChild(&pParentNode, &pCurrElement);

	long lPumpNumber = a2l(pvStructIn->CardSaleAll.data.sPumpNumber, sizeof(pvStructIn->CardSaleAll.data.sPumpNumber));
	long lPumpIndex = lPumpNumber - 1;

	CTendersCardsIterator cTenderIter = m_cTendersCard[lPumpIndex].begin(), endIterator2 = m_cTendersCard[lPumpIndex].end();
	for (iIndex; (cTenderIter != endIterator2); iIndex++)
	{
		sprintf_s(sLineID, "%ld", iIndex + 1);//get the line id
		xmlInitDoc.CreateElement("TENDER_DATA", &pTendersNode);
		xmlInitDoc.SetAttribute("ID", (char *)sLineID, &pTendersNode);

		AddLineWithNull(&xmlInitDoc, &pTendersNode, "sReferenceNumber", (BYTE *)cTenderIter->GetRefernceNumber().GetBuffer(0), cTenderIter->GetRefernceNumber().GetLength());
		AddLineWithNull(&xmlInitDoc, &pTendersNode, "sCardName", (BYTE *)cTenderIter->GetFullCardName().GetBuffer(), cTenderIter->GetFullCardName().GetLength());
		AddLineWithNull(&xmlInitDoc, &pTendersNode, "sTenderId", (BYTE *)cTenderIter->GetTenderIdStr().GetBuffer(), cTenderIter->GetTenderIdStr().GetLength());
		AddLineWithNull(&xmlInitDoc, &pTendersNode, "sMediaMasterType", (BYTE *)cTenderIter->GetMediaMasterType().GetBuffer(), cTenderIter->GetMediaMasterType().GetLength());
		AddLineWithNull(&xmlInitDoc, &pTendersNode, "sCompletedSessionId", (BYTE *)cTenderIter->GetCompletedSessionId().GetBuffer(), cTenderIter->GetCompletedSessionId().GetLength());
		AddLineWithNull(&xmlInitDoc, &pTendersNode, "sExpire", (BYTE *)cTenderIter->GetExpireDate().GetBuffer(), cTenderIter->GetExpireDate().GetLength());
		AddLineWithNull(&xmlInitDoc, &pTendersNode, "sAccountNumber", (BYTE *)cTenderIter->GetAccountNumber().GetBuffer(), cTenderIter->GetAccountNumber().GetLength());
		BYTE sMediaType = cTenderIter->GetMediaType();
		AddLineWithNull(&xmlInitDoc, &pTendersNode, "sMediaType", &sMediaType, 1);

		char sTmpBuffer[100] = { 0 };
		long lVolume = cTenderIter->GetUsedAmount();
		sprintf_s((char *)sTmpBuffer, sizeof(sTmpBuffer), "%ld", lVolume);
		AddLineWithNull(&xmlInitDoc, &pTendersNode, "sAmount", (unsigned char *)sTmpBuffer, (long)min(strlen(sTmpBuffer), sizeof(sTmpBuffer)));

		sprintf_s((char *)sTmpBuffer, sizeof(sTmpBuffer), "%ld", 1000);
		AddLineWithNull(&xmlInitDoc, &pTendersNode, "Factor", (unsigned char *)sTmpBuffer, (long)min(strlen(sTmpBuffer), sizeof(sTmpBuffer)));

		xmlInitDoc.AppendChild(&pTendersNode, &pParentNode);

		cTenderIter++;
	}

	m_cTendersCard[lPumpIndex].clear();
}