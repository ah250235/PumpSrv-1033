// CashAcceptor.cpp: implementation of the CCashAcceptor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CashAcceptor.h"
#include  "CashAcceptorFile.h"
#include "PumpSrvDatabase.h"
#include "SQLReceiptDB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCashAcceptor::CCashAcceptor()
{

}

CCashAcceptor::~CCashAcceptor()
{

}

void CCashAcceptor::UpdatebillsInformation(long lPumpNumber, long lDevice, long lSeqNum,  EXTRA_INFO_REC &ExtraInfoRec, TAG_CASH_SALE_DATA &cTagCashSaleData)
{
	ExtraInfoRec.info.lFlag  = TRS_LINK_EXTRA_INFO + TRS_LINK_PREPAY_NUMBER +TRS_LINK_CASH_ACCEPTOR_INFO;						
	ExtraInfoRec.info.byCashDeposit1   =  (BYTE)a2l(cTagCashSaleData.sCashDeposit1,sizeof(cTagCashSaleData.sCashDeposit1));				
	ExtraInfoRec.info.byCashDeposit2   =  (BYTE)a2l(cTagCashSaleData.sCashDeposit2,sizeof(cTagCashSaleData.sCashDeposit2));			
	ExtraInfoRec.info.byCashDeposit5   =  (BYTE)a2l(cTagCashSaleData.sCashDeposit5,sizeof(cTagCashSaleData.sCashDeposit5));			
	ExtraInfoRec.info.byCashDeposit10  =  (BYTE)a2l(cTagCashSaleData.sCashDeposit10,sizeof(cTagCashSaleData.sCashDeposit10));		
	ExtraInfoRec.info.byCashDeposit20  =  (BYTE)a2l(cTagCashSaleData.sCashDeposit20,sizeof(cTagCashSaleData.sCashDeposit20));		
	ExtraInfoRec.info.byCashDeposit50  =  (BYTE)a2l(cTagCashSaleData.sCashDeposit50,sizeof(cTagCashSaleData.sCashDeposit50));		
	ExtraInfoRec.info.byCashDeposit100 =  (BYTE)a2l(cTagCashSaleData.sCashDeposit100,sizeof(cTagCashSaleData.sCashDeposit100));		
	ExtraInfoRec.info.lCashDepositTotal=  a2l(cTagCashSaleData.sCashDepositTotal,sizeof(cTagCashSaleData.sCashDepositTotal)) * 10;		
	ExtraInfoRec.info.lRefundAmount=  a2l(cTagCashSaleData.sRefundAmount,sizeof(cTagCashSaleData.sRefundAmount));		
	memcpy(ExtraInfoRec.info.sInvoiceNumber ,cTagCashSaleData.sInvoiceNumber ,min(sizeof(cTagCashSaleData.sInvoiceNumber) ,sizeof(ExtraInfoRec.info.sInvoiceNumber))); //4.0.11.14
	memcpy(ExtraInfoRec.info.sTransactionDateTime,cTagCashSaleData.sTransactionDateTime , sizeof(ExtraInfoRec.info.sTransactionDateTime)); //4.0.11.14
	ExtraInfoRec.info.lPumpNumber = lPumpNumber; //4.0.11.14
}

void CCashAcceptor::UpdateFinalInformation(EXTRA_INFO_REC &ExtraInfoRec, TAG_CASH_SALE_DATA &cTagCashSaleData)
{
	memcpy(ExtraInfoRec.info.sTransactionDateTime,cTagCashSaleData.sTransactionDateTime , sizeof(ExtraInfoRec.info.sTransactionDateTime));
	ExtraInfoRec.info.lTransactionAmount = a2l(cTagCashSaleData.sTransactionAmount, sizeof(cTagCashSaleData.sTransactionAmount));
	ExtraInfoRec.info.lRefundAmount =a2l(cTagCashSaleData.sRefundAmount, sizeof(cTagCashSaleData.sRefundAmount));	
	ExtraInfoRec.info.sCarWashProgram = cTagCashSaleData.sCarWashProgram;

	memcpy(ExtraInfoRec.info.sWashCode, cTagCashSaleData.sWashCode, sizeof(cTagCashSaleData.sWashCode)); //4.0.21.501 - TD 70634
	memcpy(ExtraInfoRec.info.sWashExpire, cTagCashSaleData.sWashExpire, sizeof(ExtraInfoRec.info.sWashExpire));	

	ExtraInfoRec.info.lCarWashTax =a2l(cTagCashSaleData.sCarWashTax, sizeof(cTagCashSaleData.sCarWashTax));
	ExtraInfoRec.info.lCarWashAmount =a2l(cTagCashSaleData.sCarWashAmount, sizeof(cTagCashSaleData.sCarWashAmount));
	ExtraInfoRec.info.lCarWashDiscount = a2l(cTagCashSaleData.sCarWashDiscount,sizeof(cTagCashSaleData.sCarWashDiscount)); //4.0.11.90

	ExtraInfoRec.info.lServiceType =cTagCashSaleData.sServiceType;
	ExtraInfoRec.info.lSalesTax= a2l(cTagCashSaleData.sSalesTax, sizeof(cTagCashSaleData.sSalesTax));	
	ExtraInfoRec.info.lDiscount= a2l(cTagCashSaleData.sDiscount, sizeof(cTagCashSaleData.sDiscount));	
	
	memcpy(ExtraInfoRec.info.sBarCode ,cTagCashSaleData.sBarCode , sizeof(ExtraInfoRec.info.sBarCode));;

	if(LOG_BASIC_FLOW_CONTROL)  //4.0.11.90
	{
		CString str;
		str.Format("Pump - %02ld Cash Acceptor Trs =%ld with Carwash code %.6s , CarwashProgram = %c" , ExtraInfoRec.info.lPumpNumber ,ExtraInfoRec.info.lIndexNumber , ExtraInfoRec.info.sWashCode , ExtraInfoRec.info.sCarWashProgram);
		_Module.m_server.m_cLogger.LogMsg(str);
	}
}

void CCashAcceptor::GetCashAcceptorInformation(EXTRA_INFO_REC &ExtraInfoRec, TAG_CASH_SALE_DATA &cTagCashSaleData)
{	
	l2a((long)ExtraInfoRec.info.byCashDeposit1,cTagCashSaleData.sCashDeposit1 ,  sizeof(cTagCashSaleData.sCashDeposit1));
	l2a((long)ExtraInfoRec.info.byCashDeposit2,cTagCashSaleData.sCashDeposit2 ,  sizeof(cTagCashSaleData.sCashDeposit2));
	l2a((long)ExtraInfoRec.info.byCashDeposit5,cTagCashSaleData.sCashDeposit5 ,  sizeof(cTagCashSaleData.sCashDeposit5));
	l2a((long)ExtraInfoRec.info.byCashDeposit10,cTagCashSaleData.sCashDeposit10 ,  sizeof(cTagCashSaleData.sCashDeposit10));
	l2a((long)ExtraInfoRec.info.byCashDeposit20,cTagCashSaleData.sCashDeposit20 ,  sizeof(cTagCashSaleData.sCashDeposit20));
	l2a((long)ExtraInfoRec.info.byCashDeposit50,cTagCashSaleData.sCashDeposit50 ,  sizeof(cTagCashSaleData.sCashDeposit50));
	l2a((long)ExtraInfoRec.info.byCashDeposit100,cTagCashSaleData.sCashDeposit100 ,  sizeof(cTagCashSaleData.sCashDeposit100));
	
	l2a(ExtraInfoRec.info.lCashDepositTotal, cTagCashSaleData.sCashDepositTotal,sizeof(cTagCashSaleData.sCashDepositTotal));

	memcpy(cTagCashSaleData.sTransactionDateTime ,ExtraInfoRec.info.sTransactionDateTime, sizeof(cTagCashSaleData.sTransactionDateTime));

	l2a(ExtraInfoRec.info.lTransactionAmount ,cTagCashSaleData.sTransactionAmount, sizeof(cTagCashSaleData.sTransactionAmount));
	l2a(ExtraInfoRec.info.lRefundAmount ,cTagCashSaleData.sRefundAmount, sizeof(cTagCashSaleData.sRefundAmount));	
	cTagCashSaleData.sCarWashProgram = ExtraInfoRec.info.sCarWashProgram;

	memcpy(cTagCashSaleData.sWashCode, ExtraInfoRec.info.sWashCode, sizeof(ExtraInfoRec.info.sWashCode)); //4.0.21.501 - TD 70634

	memcpy( cTagCashSaleData.sWashExpire, ExtraInfoRec.info.sWashExpire,sizeof( cTagCashSaleData.sWashExpire));	
	ExtraInfoRec.info.lCarWashTax =a2l(cTagCashSaleData.sCarWashTax, sizeof(cTagCashSaleData.sCarWashTax));
	ExtraInfoRec.info.lCarWashAmount =a2l(cTagCashSaleData.sCarWashAmount, sizeof(cTagCashSaleData.sCarWashAmount));
    ExtraInfoRec.info.lServiceType =cTagCashSaleData.sServiceType;
	ExtraInfoRec.info.lSalesTax= a2l(cTagCashSaleData.sSalesTax, sizeof(cTagCashSaleData.sSalesTax));	
	ExtraInfoRec.info.lDiscount= a2l(cTagCashSaleData.sDiscount, sizeof(cTagCashSaleData.sDiscount));	
	memcpy(ExtraInfoRec.info.sBarCode ,cTagCashSaleData.sBarCode , sizeof(ExtraInfoRec.info.sBarCode));

}

/******************************************************************************
 Description:	Build OLA record form buffers 	
 Returns:      	Save operation code 
 Parameters:	CPumpTransact   trs , EXTRA_INFO_REC	 ExtraInfoRec  	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			31/10/2002   14:34		Start
******************************************************************************/
long CCashAcceptor::SendDataToOLA(CPumpTransact  & trs , EXTRA_INFO_REC	& ExtraInfoRec ,BOOL bPreAuthDetailsFlags, BOOL bUpdateUnrecognizedBill) //4.0.17.90 - CR 40564
{
	PAY_AT_PUMP_INFO	PayAtPumpInfo;	
	CPumpTransact		cTmpTrs;
	long lRtc = 0;

	memset(&PayAtPumpInfo , ' ',sizeof(PAY_AT_PUMP_INFO));
	
	CreateOLARec(PayAtPumpInfo, trs, ExtraInfoRec ,bPreAuthDetailsFlags, bUpdateUnrecognizedBill);
	
	cTmpTrs =trs;	

	PayAtPumpInfo.CardSaleInfo.lIndexNumber = _Module.m_server.NewOLAIndex();

	ExtraInfoRec.info.lIndexNumber = PayAtPumpInfo.CardSaleInfo.lIndexNumber;
		 
	cTmpTrs.SetLink(TRS_LINK_OLA_NUMBER, PayAtPumpInfo.CardSaleInfo.lIndexNumber);		

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("New OLA Index Number - %ld (Trs %ld, Value %ld, Volume %ld) ",PayAtPumpInfo.CardSaleInfo.lIndexNumber,cTmpTrs.m_lNumber,cTmpTrs.m_lRoundedValue,cTmpTrs.m_lRoundedVolume); 
		_LOGMSG.LogMsg(str);
	}						

	lRtc = _Module.m_server.TrsLinkOperation(REC_INSERT,&cTmpTrs, &PayAtPumpInfo);	
	// insert info to receipt file
	if (lRtc)
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("CCashAcceptor::SendDataToOLA Fail Update OLA Index Number - %ld (Trs %ld, Value %ld, Volume %ld) ",PayAtPumpInfo.CardSaleInfo.lIndexNumber,trs.m_lNumber,trs.m_lRoundedValue,trs.m_lRoundedVolume); 
			_LOGMSG.LogMsg(str);
		}						

	}
		
	return lRtc;
}

void CCashAcceptor::CreateOLARec(PAY_AT_PUMP_INFO &PayAtPumpInfo, CPumpTransact &trs, EXTRA_INFO_REC & ExtraInfoRec , BOOL bPreAuthDetailsFlags, BOOL bUpdateUnrecognizedBill) //4.0.17.90 - CR 40564
{	
	SYSTEMTIME		curSysTime;
	GetLocalTime(&curSysTime);	
	char sTmpDateAndTime[20]={0};

	sprintf_s(sTmpDateAndTime, _countof(sTmpDateAndTime),"%02d%02d%02d%02d%02d%02d" , ((curSysTime.wYear >= 2000 )? (curSysTime.wYear - 2000) : (curSysTime.wYear - 1900)) ,curSysTime.wMonth,curSysTime.wDay, curSysTime.wHour , curSysTime.wMinute ,curSysTime.wSecond) ;
	memcpy(PayAtPumpInfo.CardSaleInfo.cardData.sDateTime ,sTmpDateAndTime , 10);
	memcpy(PayAtPumpInfo.CardSaleInfo.extraData.sDateTimeExt ,&sTmpDateAndTime[10] , 2);

	if (bUpdateUnrecognizedBill)	//4.0.17.90 - CR 40564
	{
		PayAtPumpInfo.CardSaleExtraData3.sUnrecognizedBillFound = '1';
		l2a(ExtraInfoRec.info.lSeqNumber, PayAtPumpInfo.CardSaleInfo.cardData.sReferenceNumber, sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sReferenceNumber)); //4.0.18.0
	}

	l2a((long)ExtraInfoRec.info.byCashDeposit1,PayAtPumpInfo.CardSaleExtraData2.sCashDeposit1, sizeof(PayAtPumpInfo.CardSaleExtraData2.sCashDeposit1)); 
	l2a((long)ExtraInfoRec.info.byCashDeposit2,PayAtPumpInfo.CardSaleExtraData2.sCashDeposit2, sizeof(PayAtPumpInfo.CardSaleExtraData2.sCashDeposit2)); 
	l2a((long)ExtraInfoRec.info.byCashDeposit5,PayAtPumpInfo.CardSaleExtraData2.sCashDeposit5, sizeof(PayAtPumpInfo.CardSaleExtraData2.sCashDeposit5)); 
	l2a((long)ExtraInfoRec.info.byCashDeposit10, PayAtPumpInfo.CardSaleExtraData2.sCashDeposit10, sizeof(PayAtPumpInfo.CardSaleExtraData2.sCashDeposit10)); 
	l2a((long)ExtraInfoRec.info.byCashDeposit20,PayAtPumpInfo.CardSaleExtraData2.sCashDeposit20, sizeof(PayAtPumpInfo.CardSaleExtraData2.sCashDeposit20)); 
	l2a((long)ExtraInfoRec.info.byCashDeposit50,PayAtPumpInfo.CardSaleExtraData2.sCashDeposit50, sizeof(PayAtPumpInfo.CardSaleExtraData2.sCashDeposit50)); 
	l2a((long)ExtraInfoRec.info.byCashDeposit100,PayAtPumpInfo.CardSaleExtraData2.sCashDeposit100, sizeof(PayAtPumpInfo.CardSaleExtraData2.sCashDeposit100)); 	
	l2a(ExtraInfoRec.info.lRefundAmount,PayAtPumpInfo.CardSaleExtraData2.sRefundAmount , sizeof(PayAtPumpInfo.CardSaleExtraData2.sRefundAmount)); 

	if (bUpdateUnrecognizedBill) //Merge 4.0.17.450
		l2a(ExtraInfoRec.info.lCashDepositTotal, PayAtPumpInfo.CardSaleExtraData2.sCashDepositTotal,sizeof(PayAtPumpInfo.CardSaleExtraData2.sCashDepositTotal)); 
	else
		l2a(ExtraInfoRec.info.lCashDepositTotal /10,PayAtPumpInfo.CardSaleExtraData2.sCashDepositTotal,sizeof(PayAtPumpInfo.CardSaleExtraData2.sCashDepositTotal)); 

	l2a(ExtraInfoRec.info.lSeqNumber,PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr , sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr));  //4.0.10.44
	PayAtPumpInfo.CardSaleInfo.lIndexNumber = ExtraInfoRec.info.lIndexNumber; //4.0.17.191

	l2a(ExtraInfoRec.info.lDevice , PayAtPumpInfo.CardSaleInfo.cardData.sDeviceNumber , sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sDeviceNumber)); //4.0.10.20

	if (bPreAuthDetailsFlags)
	{		
		if (bUpdateUnrecognizedBill) //Merge 4.0.17.450
		{
			Convertl2Str2((ExtraInfoRec.info.lCashDepositTotal)  ,
				PayAtPumpInfo.CardSaleInfo.extraData.sAuthAmt_Msb,sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sAuthAmt_Msb) , 
				PayAtPumpInfo.CardSaleInfo.cardData.sAuthAmt, sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sAuthAmt));
		}
		else
		{
			Convertl2Str2((ExtraInfoRec.info.lCashDepositTotal /10)  ,
				PayAtPumpInfo.CardSaleInfo.extraData.sAuthAmt_Msb,sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sAuthAmt_Msb) , 
				PayAtPumpInfo.CardSaleInfo.cardData.sAuthAmt, sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sAuthAmt));
		}
	}
	else
	{
		Convertl2Str2(trs.m_lRoundedValue , //4.0.10.16//4.0.10.251
			PayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt_Msb) , 
				PayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt, sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt));		


		long lFuelAmt = trs.m_lRoundedValue/10 - ExtraInfoRec.info.lCarWashAmount; //4.0.21.500 - TD 14778
		
		Convertl2Str2(lFuelAmt,  //4.0.21.500 - TD 14778
			PayAtPumpInfo.CardSaleInfo.extraData.sFuelAmt_Msb,sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sFuelAmt_Msb) , 
			PayAtPumpInfo.CardSaleInfo.extraData.sFuelAmt, sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sFuelAmt));		
			

	}

	if (ChrAll((char *)ExtraInfoRec.info.sTrack2Data,TRACK_2_DATA_SIZE,' '))	//4.0.18.504 Merge from 4.0.17.330 
	{
		memcpy(PayAtPumpInfo.CardSaleInfo.cardData.sTrack2Data,ExtraInfoRec.info.sInvoiceNumber, sizeof(ExtraInfoRec.info.sInvoiceNumber));
	}

	if (ChrAll((char *)ExtraInfoRec.info.sAccountNumber,ACCOUNT_NUM_SIZE,' '))	//4.0.18.504 Merge from 4.0.17.330 
	{
		memcpy(PayAtPumpInfo.CardSaleInfo.cardData.sAccountNumber,ExtraInfoRec.info.sInvoiceNumber, sizeof(ExtraInfoRec.info.sInvoiceNumber));
		_LOGMSG.LogMsg("CCashAcceptor::CreateOLARec Account number is empty. Invoice number was copied.");
	}
	

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		if (bUpdateUnrecognizedBill) //Merge 4.0.17.450
		{
			str.Format("CCashAcceptor::CreateOLARec device %ld preauth=%.5s, fueled=%.5s Total trs=%ld, Refund=%ld, bills  one=%d , two=%d, five=%d , ten=%d, twenty=%d , fifty=%d , hundred=%d date=%s"  ,
				ExtraInfoRec.info.lDevice ,
				PayAtPumpInfo.CardSaleInfo.cardData.sAuthAmt,
				PayAtPumpInfo.CardSaleInfo.extraData.sFuelAmt,
				ExtraInfoRec.info.lCashDepositTotal,
				ExtraInfoRec.info.lRefundAmount , ExtraInfoRec.info.byCashDeposit1 ,ExtraInfoRec.info.byCashDeposit2 ,ExtraInfoRec.info.byCashDeposit5,ExtraInfoRec.info.byCashDeposit10 ,ExtraInfoRec.info.byCashDeposit20 , ExtraInfoRec.info.byCashDeposit50 ,ExtraInfoRec.info.byCashDeposit100 ,sTmpDateAndTime);
		}
		else
		{
			str.Format("CCashAcceptor::CreateOLARec device %ld preauth=%.5s, fueled=%.5s Total trs=%ld, Refund=%ld, bills  one=%d , two=%d, five=%d , ten=%d, twenty=%d , fifty=%d , hundred=%d date=%s"  ,
				ExtraInfoRec.info.lDevice ,
				PayAtPumpInfo.CardSaleInfo.cardData.sAuthAmt,
				PayAtPumpInfo.CardSaleInfo.extraData.sFuelAmt,
				ExtraInfoRec.info.lCashDepositTotal /10,
				ExtraInfoRec.info.lRefundAmount , ExtraInfoRec.info.byCashDeposit1 ,ExtraInfoRec.info.byCashDeposit2 ,ExtraInfoRec.info.byCashDeposit5,ExtraInfoRec.info.byCashDeposit10 ,ExtraInfoRec.info.byCashDeposit20 , ExtraInfoRec.info.byCashDeposit50 ,ExtraInfoRec.info.byCashDeposit100 ,sTmpDateAndTime);
		}
		_LOGMSG.LogMsg(str);
	}						

	memcpy(&PayAtPumpInfo.CardSaleExtraData2.sInvoiceNumber[3], ExtraInfoRec.info.sInvoiceNumber , sizeof(ExtraInfoRec.info.sInvoiceNumber)); //4.0.11.14
	l2a(ExtraInfoRec.info.lPumpNumber , PayAtPumpInfo.CardSaleInfo.cardData.sPumpNumber , sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sPumpNumber)); //4.0.11.14

	memcpy(PayAtPumpInfo.CardSaleInfo.extraData.sWashCode, ExtraInfoRec.info.sWashCode, sizeof(ExtraInfoRec.info.sWashCode)); //4.0.21.501 - TD 70634
	PayAtPumpInfo.CardSaleInfo.extraData.sCarWashProgram = ExtraInfoRec.info.sCarWashProgram;
	memcpy(PayAtPumpInfo.CardSaleInfo.extraData.sWashExpire , ExtraInfoRec.info.sWashExpire , sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sWashExpire));

	l2a(1,PayAtPumpInfo.CardSaleInfo.extraData.SALES[0].sQty, sizeof(PayAtPumpInfo.CardSaleInfo.extraData.SALES[0].sQty));
	l2a(ExtraInfoRec.info.lCarWashAmount /*/10*/ ,PayAtPumpInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice , sizeof(PayAtPumpInfo.CardSaleInfo.extraData.SALES[0].sUnitPrice));
	
	memcpy(PayAtPumpInfo.CardSaleInfo.extraData.SALES[0].sDPTProdCode, ExtraInfoRec.info.sDPTProductCode, sizeof(ExtraInfoRec.info.sDPTProductCode)); //4.0.21.500 - TD 14778
	
	l2a(ExtraInfoRec.info.lCarWashDiscount,PayAtPumpInfo.CardSaleInfo.extraData.SALES[0].sDiscount , sizeof(PayAtPumpInfo.CardSaleInfo.extraData.SALES[0].sDiscount));
	l2a(ExtraInfoRec.info.lCarWashTax,PayAtPumpInfo.CardSaleInfo.extraData.SALES[0].sTax , sizeof(PayAtPumpInfo.CardSaleInfo.extraData.SALES[0].sTax));
	
	PayAtPumpInfo.CardSaleInfo.cardData.sTranType = CD_TYPE_CASH_ACCEPTOR;
	
	//4.0.24.140 127928 start
	for(int i=0 ; i< MAX_GRADES; i++)
	{
		l2a(ExtraInfoRec.info.lDiscountArray[i], PayAtPumpInfo.CardSaleExtraData2.sDiscountPerGrade[i].sDiscount , sizeof(PayAtPumpInfo.CardSaleExtraData2.sDiscountPerGrade[i].sDiscount));  //4.0.20.141			 
	}
	memcpy(PayAtPumpInfo.CardSaleExtraData2.sPromotionNumber, ExtraInfoRec.info.sPromotionNumber , sizeof(ExtraInfoRec.info.sPromotionNumber));
	memcpy(PayAtPumpInfo.CardSaleInfo.extraData.sLoyalId , ExtraInfoRec.info.sLoyalId , sizeof(ExtraInfoRec.info.sLoyalId));
	memcpy(PayAtPumpInfo.CardSaleExtraData4.sLoyaltyOriginalUnitPrice, ExtraInfoRec.info.sOriginalUnitPrice, sizeof(ExtraInfoRec.info.sOriginalUnitPrice));
	memcpy(PayAtPumpInfo.CardSaleExtraData3.sNonDiscountTotalAmount, ExtraInfoRec.info.sNonDiscountTotalAmount, sizeof(ExtraInfoRec.info.sNonDiscountTotalAmount));
	memcpy(&ExtraInfoRec.info.sDeferralAccepted ,  &PayAtPumpInfo.CardSaleExtraData4.sDeferralAccepted, sizeof(PayAtPumpInfo.CardSaleExtraData4.sDeferralAccepted));

	for (int i = 0; i < MAX_POSITIONS_PER_PUMP; i++)
	{
		memcpy(PayAtPumpInfo.CardSaleExtraData3.GradeLimitsArray[i].sGradeID, ExtraInfoRec.info.GradeAmountLimit[i].sGradeID, min(sizeof ExtraInfoRec.info.GradeAmountLimit[i].sGradeID, sizeof PayAtPumpInfo.CardSaleExtraData3.GradeLimitsArray[i].sGradeID));
		l2a(ExtraInfoRec.info.GradeAmountLimit[i].lGradeLimit, PayAtPumpInfo.CardSaleExtraData3.GradeLimitsArray[i].sGradeAmountLimit, sizeof ExtraInfoRec.info.GradeAmountLimit[i].lGradeLimit);
		PayAtPumpInfo.CardSaleExtraData3.GradeLimitsArray[i].sLimitType = '1';
	}
	
	//memcpy(PayAtPumpInfo.CardSaleExtraData3.GradeLimitsArray, ExtraInfoRec.info.GradeAmountLimit, sizeof(PayAtPumpInfo.CardSaleExtraData3.GradeLimitsArray));
	//4.0.24.140 127928 end
}

long CCashAcceptor::SendDataToOLA(CPumpTransact &trs , BOOL bPreAuthDetailsFlags)
{	
	CPumpTransact   cTmpTrs;
	EXTRA_INFO_REC  ExtraInfoRec;
	long			lRtc;
	long			lRtcCode =0; 

	memset(&ExtraInfoRec, 0, sizeof(EXTRA_INFO_REC)); //4.0.19.504

	cTmpTrs.m_lNumber = trs.m_lNumber ;
	cTmpTrs.SetLink(TRS_LINK_EXTRA_INFO,0);		
	ExtraInfoRec.info.lIndexNumber = trs.m_lNumber;

	lRtc = _Module.m_server.TrsLinkOperation(REC_READ,&cTmpTrs, &ExtraInfoRec);	
	// insert info to receipt file
	if (lRtc == OK)
	{
		lRtcCode = SendDataToOLA(trs, ExtraInfoRec,bPreAuthDetailsFlags); //4.0.10.30
	}
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("CCashAcceptor::SendDataToOLA Fail Reading ExtraInfo index=%ld" , trs.m_lNumber); 
			_LOGMSG.LogMsg(str);
		}						
	}

	return (lRtcCode) ;


}
/******************************************************************************
 Description:	Close cash acceptor transaction  	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			19/7/2005   17:36		Start
******************************************************************************/

long CCashAcceptor::FinalizeTrs(CPumpTransact &trs)
{
	long lRtc; 
	CString sMsg;

	if(LOG_BASIC_FLOW_CONTROL)
	{
		sMsg.Format("Pump number =%ld Finalize Cash acceptor trs=%ld"  ,trs.m_lPump, trs.m_lNumber); 
		_LOGMSG.LogMsg(trs.m_lPump,LOG_PUMP,sMsg);
	}						

	SendDataToOLA(trs);	
	// Clear trs

	if (lRtc = _Module.m_server.m_cCommonFunction.RemovePrePaidTrs__(trs.m_lPump ,trs.m_lNumber))
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("PumpNumber = %ld ,Fail delete cash acceptor record  trs=%ld ,error=%ld "  , trs.m_lPump ,trs.m_lNumber ,lRtc); 
			_LOGMSG.LogMsg(sMsg);
		}						

	}

	return 0;
}
/******************************************************************************
 Description:	Create receipt row data form cash acceptor details 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			19/7/2005   17:37		Start
******************************************************************************/

long CCashAcceptor::GenerateReceiptRowData(CPumpTransact &trs)
{
	CPumpTransact   cTmpTrs;
	EXTRA_INFO_REC  ExtraInfoRec;
	long			lRtc;
	long			lRtcCode =0; 
	CString sMsg;
	RECEIPT_REC_EX_DATA cExData;

	cTmpTrs.m_lNumber = trs.m_lNumber ;
	cTmpTrs.SetLink(TRS_LINK_EXTRA_INFO,0);		
	ExtraInfoRec.info.lIndexNumber = trs.m_lNumber;
	memset(&cExData,0,sizeof(cExData));

	lRtc = _Module.m_server.TrsLinkOperation(REC_READ,&cTmpTrs, &ExtraInfoRec);	
	// insert info to receipt file
	if (lRtc == OK)
	{
		PAY_AT_PUMP_INFO	PayAtPumpInfo;	
		CXMLInterface		PayAtPumpInfoXml;	
		long lRtc = 0;
		BYTE byIndex=0; 

		memset(&PayAtPumpInfo , ' ',sizeof(PAY_AT_PUMP_INFO));
		
		CreateOLARec(PayAtPumpInfo, trs, ExtraInfoRec);

		Convertl2Str2((ExtraInfoRec.info.lCashDepositTotal)  ,  //4.0.13.503
			PayAtPumpInfo.CardSaleInfo.extraData.sAuthAmt_Msb,sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sAuthAmt_Msb) , 
			PayAtPumpInfo.CardSaleInfo.cardData.sAuthAmt, sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sAuthAmt));		

		CReceiptInfo ReceiptInfo;
		_Module.m_server.m_cBaseReceipt->ConvertOlaToReceipt(&trs, &PayAtPumpInfo, ReceiptInfo);//4.0.9.503
		CSQLReceiptDB::GetInstance().UpdateReceipt(ReceiptInfo);

		cExData.byIndex = byIndex;
		
		if (LOG_BASIC_FLOW_CONTROL)
		{//4.0.20.501 TD 60236		
			char szResult[MAX_FIELD_VAL] = {0};		
			_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, (char*)PayAtPumpInfo.CardSaleInfo.cardData.sTrack2Data, sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sTrack2Data));
	
			CString str1;
			str1.Format("CCashAcceptor::GenerateReceiptRowData, Insert receipt track2 = %s, index = %ld", szResult, cExData.byIndex);
		
			_LOGMSG.LogMsg(str1);
		}
		

		if (lRtc)
		{
			if (LOG_BASIC_FLOW_CONTROL)
			{
				sMsg.Format("Fail insert cash acceptor  details into receipt qdx index =%ld" , trs.m_lNumber); 
				_LOGMSG.LogMsg(sMsg);
			}
		}
		else
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{
				sMsg.Format("PumpNumber %02ld Create receipt for cash acceptor  trs=%ld Index=%d, seq=%.5s" ,trs.m_lPump, trs.m_lNumber ,byIndex ,PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr ); //4.0.10.52
				_LOGMSG.LogMsg(trs.m_lPump,LOG_PUMP,sMsg);
			}
			
			if (trs.m_lPump)  //4.0.10.25
			{

				_Module.m_server.m_cMemPumpArray[trs.m_lPump - 1].SetMemPumpInfo(&PayAtPumpInfo ,&trs , NULL ,byIndex); //4.0.0.6 //4.0.30.0
			}
		}
	}
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("CCashAcceptor::Generate Row receipt Fail Reading ExtraInfo index=%ld" , trs.m_lNumber); 
			_LOGMSG.LogMsg(sMsg);
		}						
	}

	return lRtc;
}



/******************************************************************************
 Description:	Create receipt row data from cash acceptor unrecognized bill details 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			05/09/2007   15:00		Start
 Gena			09/12/2007		15:15	4.0.18.504 Merge from 4.0.17.330
******************************************************************************/

long CCashAcceptor::GenerateReceiptRowDataUnrecognizedBill(CPumpTransact &trs, 	EXTRA_INFO_REC &ExtraInfoRec)
{
	long			lRtc = 0;
	long			lRtcCode =0; 
	CString sMsg;
	RECEIPT_REC_EX_DATA cExData;
	memset(&cExData,0,sizeof(cExData));
	PAY_AT_PUMP_INFO	PayAtPumpInfo;	
	BYTE byIndex=0; 

	memset(&PayAtPumpInfo , ' ',sizeof(PAY_AT_PUMP_INFO));
	
	CreateOLARec(PayAtPumpInfo, trs, ExtraInfoRec);

	Convertl2Str2((ExtraInfoRec.info.lCashDepositTotal)  ,  //4.0.13.503
		PayAtPumpInfo.CardSaleInfo.extraData.sAuthAmt_Msb,sizeof(PayAtPumpInfo.CardSaleInfo.extraData.sAuthAmt_Msb) , 
		PayAtPumpInfo.CardSaleInfo.cardData.sAuthAmt, sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sAuthAmt));		

	cExData.byIndex = byIndex;

	CReceiptInfo ReceiptInfo;
	_Module.m_server.m_cBaseReceipt->ConvertOlaToReceipt(&trs, &PayAtPumpInfo, ReceiptInfo);//4.0.9.503
	long lRtc1 = CSQLReceiptDB::GetInstance().UpdateReceipt(ReceiptInfo);

	if (lRtc)
	{
		if (LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("Fail insert cash acceptor unrecognized bill details into receipt qdx"); 
			_LOGMSG.LogMsg(sMsg);
		}
	}
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			sMsg.Format("PumpNumber %02ld Create receipt for cash acceptor unrecognized bill seq=%.5s" ,ExtraInfoRec.info.lPumpNumber, PayAtPumpInfo.CardSaleInfo.cardData.sTranNbr ); //4.0.10.52 //Merge 4.0.17.450
			_LOGMSG.LogMsg(sMsg);
		}
	}

	return lRtc;
}


void CCashAcceptor::GenerateCashAcceptorPayIn(long lPumpNumber, CPumpTransact & trs , long lFlags)
{
	EXTRA_INFO_REC	ExtraInfoRec;
	CASH_ACCEPTOR_REC cCashAcceptorRec;

	memset(&ExtraInfoRec , 0 , sizeof(EXTRA_INFO_REC));
	ExtraInfoRec.info.lIndexNumber = trs.m_lNumber;	
	ExtraInfoRec.info.sTotalBalance;

	long p = lPumpNumber-1; //4.0.21.500 69415
	long lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);

	if (!lRtc)
	{
		if ((trs.m_nLinkFlags & TRS_LINK_CASH_ACCEPTOR_INFO) && (ExtraInfoRec.info.sPayInCreate != '1') && (ExtraInfoRec.info.lCashDepositTotal > 0)) //4.0.24.280 160508 4.0.26.505
		{
			SendDataToOLA(trs , TRUE);
			ExtraInfoRec.info.sPayInCreate = '1';
			lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_UPDATE, &ExtraInfoRec);

			if(LOG_BASIC_FLOW_CONTROL)
			{	
				CString str;
				str.Format("Pump %02ld, Add PayIn record trs %ld ",lPumpNumber,trs.m_lNumber);
				_LOGMSG.LogMsg(str);
			}

			
			memset(&cCashAcceptorRec , 0, sizeof(cCashAcceptorRec));

			cCashAcceptorRec.lCounter =  ExtraInfoRec.info.byCashDeposit1 + 
										 ExtraInfoRec.info.byCashDeposit2 + 
										 ExtraInfoRec.info.byCashDeposit5 + 
										 ExtraInfoRec.info.byCashDeposit10 +
										 ExtraInfoRec.info.byCashDeposit20 + 
										 ExtraInfoRec.info.byCashDeposit50 + 
										 ExtraInfoRec.info.byCashDeposit100 ;


			cCashAcceptorRec.lAmount = ExtraInfoRec.info.lCashDepositTotal;


			lRtc = _Module.m_server.m_cCashAcceptorFile.UpdateBillsCurrent(ExtraInfoRec.info.lDevice , cCashAcceptorRec);

		}
		else
		{
			if (lFlags & CASH_ACCEPTORS_TRANSACTION_CANCELED) //4.0.21.500 69415
			{
				PRE_PAY_INFO	cTmpPrePayinfo;
				CPumpTransact	cTempTrs = trs;
				if(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_CASH_ACCEPTOR_INFO,&trs))  
				{
					lRtc = _Module.m_server.TrsLinkOperation(REC_READ,&cTempTrs,&cTmpPrePayinfo);
					if (!lRtc)	
					{
						trs.m_lStatus = PAID;
						lRtc = _Module.m_server.TrsLinkOperation(REC_UPDATE,&trs,&cTmpPrePayinfo);
						if (!lRtc)
						{
							_Module.m_server.CentralUpdateTrs(lPumpNumber,trs.m_lNumber,&trs,FALSE);	
			
							if (ExtraInfoRec.info.lCashDepositTotal > 0) //4.0.24.280 160508 //4.0.26.505
							{

								_Module.m_server.m_cPumpArray[p].IncreaseUnBalancePrePayCounter(); //The number of unbalanced is increased in setPumpStatus

								if(LOG_BASIC_FLOW_CONTROL)
								{	
									CString str;
									str.Format("CCashAcceptor::GenerateCashAcceptorPayIn, trs %ld was closed as unbalanced",trs.m_lNumber);
									_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
								}
							}
							else
							{
								if(LOG_BASIC_FLOW_CONTROL)
								{	
									CString str;
									str.Format("CCashAcceptor::GenerateCashAcceptorPayIn, trs %ld was closed as 0 deposit",trs.m_lNumber);
									_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
								}

							}

							
						}
					}
				}
				else
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{	
						CString str;
						str.Format("CCashAcceptor::GenerateCashAcceptorPayIn, no cash acceptor trs in status WAIT_PROGRESS");
						_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
					}
				}
			}
		}
	}
}


GCIRetCode CCashAcceptor::CashAcceptorUpdateData(long &lPumpNumber, TAG_CASH_SALE_DATA &cTagCashSaleData, CPumpTransact & trs)
{
	GCIRetCode retCode = GCI_OK;
	PRE_PAY_REC		PrePayRec;	
	EXTRA_INFO_REC	ExtraInfoRec;
	long lRtc;		
	long p = lPumpNumber -1;
	CString sMsg;
	CASH_ACCEPTOR_REC cCashAcceptorRec; //4.0.27.500
	
	if(LOG_DETAIL_FLOW_CONTROL)   //4.0.11.90
	{							
		sMsg.Format("Pump - %02ld total Deposit=%.7s, " , lPumpNumber ,cTagCashSaleData.sCashDepositTotal);
		_LOGMSG.LogMsg(sMsg);
	}

	

	if ((_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs)) ||
		(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(IN_PROGRESS,TRS_LINK_PREPAY_NUMBER,&trs)) || 
		(_Module.m_server.m_cPumpArray[p].GetTrsByStatus(WAIT_TIME_OUT,TRS_LINK_PREPAY_NUMBER,&trs)))  //4.0.10.31
	{

		ExtraInfoRec.info.lIndexNumber = trs.m_lNumber;
		lRtc= _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_READ, &ExtraInfoRec);

		if (!lRtc)
		{
			if (ExtraInfoRec.info.sPayInCreate != '1')   //merge from 4.0.24.312 //4.0.27.20 217493
			{
				if(LOG_DETAIL_FLOW_CONTROL)   
				{							
					sMsg.Format("Pump - %02ld Create Force Pay In", lPumpNumber);
					_LOGMSG.LogMsg(sMsg);
				}
				// Force send PAYin incase we did not send it from some reason.
				SendDataToOLA(trs , TRUE);
				ExtraInfoRec.info.sPayInCreate = '1';

				//4.0.24.370 TD 236571 Begin - also update bills in cash acc qdx:  //4.0.28.500
				memset(&cCashAcceptorRec , 0, sizeof(cCashAcceptorRec));
				
				cCashAcceptorRec.lCounter =  ExtraInfoRec.info.byCashDeposit1 + 
					ExtraInfoRec.info.byCashDeposit2 + 
					ExtraInfoRec.info.byCashDeposit5 + 
					ExtraInfoRec.info.byCashDeposit10 +
					ExtraInfoRec.info.byCashDeposit20 + 
					ExtraInfoRec.info.byCashDeposit50 + 
					ExtraInfoRec.info.byCashDeposit100 ;
				
				cCashAcceptorRec.lAmount = ExtraInfoRec.info.lCashDepositTotal;
				
				lRtc = _Module.m_server.m_cCashAcceptorFile.UpdateBillsCurrent(ExtraInfoRec.info.lDevice , cCashAcceptorRec);
				//4.0.28.500 End
			}
				
			UpdateFinalInformation(ExtraInfoRec ,cTagCashSaleData) ;
				
			lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE, REC_UPDATE, &ExtraInfoRec);
			
			if (lRtc)
			{
				if(LOG_DETAIL_FLOW_CONTROL)
				{							
					sMsg.Format("UpdateCashAcceptorData Fail to update Extrainfo with final information trs =%ld ",trs.m_lNumber ); 
					_LOGMSG.LogMsg(sMsg);
				}

				retCode = GCI_TRS_NOT_FOUND;
			}


		}
		else
		{
			if(LOG_DETAIL_FLOW_CONTROL)
			{							
				sMsg.Format("Pump - 02ld UpdateCashAcceptorData Final/Cancel Fail to locate Cash acceptor record in ExtraInfo,  bills Index =%ld  ", lPumpNumber ,trs.m_lNumber ); 
				_LOGMSG.LogMsg(sMsg);
			}
			retCode = GCI_TRS_NOT_FOUND;
		}
		
	}
	else
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{							
			sMsg.Format("Pump - %02ld Not found cash acceptor trs" , lPumpNumber);
			_LOGMSG.LogMsg(sMsg);
		}

		retCode = GCI_TRS_NOT_FOUND;
	}

	return retCode;

}
