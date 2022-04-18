// FleetCard.cpp: implementation of the CFleetCard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FleetCard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFleetCard::CFleetCard()
{

}

CFleetCard::~CFleetCard()
{

}

void CFleetCard::ConvertTAG2RowPreAuthDetails(TAG_PRE_AUTH_TRS & cTagRec,  PREPAY_AUTH_TRS & cRowRec)
{
	cRowRec.lGrade = a2l(cTagRec.sGrade , sizeof(cTagRec.sGrade));
	cRowRec.lLevel = a2l(cTagRec.sLevel , sizeof(cTagRec.sLevel));
	cRowRec.lGradePrices = a2l(cTagRec.sGradePrices , sizeof(cTagRec.sGradePrices));
	cRowRec.lDiscount = a2l(cTagRec.sDiscount , sizeof(cTagRec.sDiscount));
	cRowRec.lGrade = a2l(cTagRec.sGrade , sizeof(cTagRec.sGrade));
	cRowRec.lMaxValue = a2l(cTagRec.sMaxValue , sizeof(cTagRec.sMaxValue));
	cRowRec.lFlags    = a2l(cTagRec.sFlags , sizeof(cTagRec.sFlags));
	cRowRec.lTotalAmount = a2l(cTagRec.sTotalAmount ,sizeof(cTagRec.sTotalAmount));  //4.0.11.500

//	if (!(cRowRec.lFlags & PREPAY_PROMPT_DISABLE))  //4.0.10.45  4.0.10.46
//		cRowRec.lFlags += PREPAY_PROMPT_DISABLE;

	if(LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("Fleet limits Grade=%ld,Grade price=%ld Maxvalue=%ld , lFlags=%lx , discount=%ld Amount limit=%ld", cRowRec.lGrade ,cRowRec.lGradePrices ,cRowRec.lMaxValue ,cRowRec.lFlags , cRowRec.lDiscount , cRowRec.lTotalAmount); //4.0.10.45			
		_LOGMSG.LogMsg(str);
	}

}

void CFleetCard::ConvertTAG2RowSoldDetails(TAG_PRE_SOLD_TRS &cTagRec, PREPAY_SOLD_TRS &cRowRec)
{
	
	cRowRec.lGrade = a2l(cTagRec.sGrade , sizeof(cTagRec.sGrade));
	cRowRec.lLevel = a2l(cTagRec.sLevel , sizeof(cTagRec.sLevel));
	cRowRec.lGradePrice = a2l(cTagRec.sGradePrice , sizeof(cTagRec.sGradePrice));
	cRowRec.lDiscount = a2l(cTagRec.sDiscount , sizeof(cTagRec.sDiscount));
	cRowRec.lGrade = a2l(cTagRec.sGrade , sizeof(cTagRec.sGrade));
	cRowRec.lValue = a2l(cTagRec.sValue , sizeof(cTagRec.sValue));

}

void CFleetCard::ConvertRow2TAGPreAuthDetails(PREPAY_AUTH_TRS  & cRowRec, TAG_PRE_AUTH_TRS & cTagRec)
{
	l2a(cRowRec.lGrade ,cTagRec.sGrade , sizeof(cTagRec.sGrade));
	l2a(cRowRec.lLevel ,cTagRec.sLevel , sizeof(cTagRec.sLevel));
	l2a(cRowRec.lGradePrices ,cTagRec.sGradePrices , sizeof(cTagRec.sGradePrices));
	l2a(cRowRec.lDiscount ,cTagRec.sDiscount , sizeof(cTagRec.sDiscount));
	l2a(cRowRec.lGrade ,cTagRec.sGrade , sizeof(cTagRec.sGrade));
	l2a(cRowRec.lMaxValue ,cTagRec.sMaxValue , sizeof(cTagRec.sMaxValue));
	l2a(cRowRec.lFlags  , cTagRec.sFlags , sizeof(cTagRec.sFlags)); //4.0.9999.0
	l2a(cRowRec.lTotalAmount  , cTagRec.sTotalAmount , sizeof(cTagRec.sTotalAmount)); //4.0..11.500

}

void CFleetCard::ConvertRow2TAGSoldDetails(PREPAY_SOLD_TRS & cRowRec, TAG_PRE_SOLD_TRS  & cTagRec)
{
	l2a(cRowRec.lGrade ,cTagRec.sGrade , sizeof(cTagRec.sGrade));
	l2a(cRowRec.lLevel ,cTagRec.sLevel , sizeof(cTagRec.sLevel));
	l2a(cRowRec.lGradePrice ,cTagRec.sGradePrice , sizeof(cTagRec.sGradePrice));
	l2a(cRowRec.lDiscount ,cTagRec.sDiscount , sizeof(cTagRec.sDiscount));
	l2a(cRowRec.lGrade ,cTagRec.sGrade , sizeof(cTagRec.sGrade));
	l2a(cRowRec.lValue ,cTagRec.sValue , sizeof(cTagRec.sValue));
	l2a(cRowRec.lVolume ,cTagRec.sVolume , sizeof(cTagRec.sVolume));

}

void CFleetCard::ConvertRowExtraInfoRecord2TAG(EXTRA_INFO_REC1 &cUnionInfo, TAG_DIESEL_INFO &cTagInfo)
{
	memset(&cTagInfo , ' ' , sizeof(TAG_DIESEL_INFO));

	ConvertRow2TAGPreAuthDetails(cUnionInfo.info.cDieselInfo.cItem1 , cTagInfo.cItem1);
	ConvertRow2TAGPreAuthDetails(cUnionInfo.info.cDieselInfo.cItem2 , cTagInfo.cItem2);
	ConvertRow2TAGSoldDetails(cUnionInfo.info.cDieselInfo.cItemSold1,cTagInfo.cItemSold1);
	ConvertRow2TAGSoldDetails(cUnionInfo.info.cDieselInfo.cItemSold2,cTagInfo.cItemSold2);
}

BOOL CFleetCard::CheckReeferWaiting(long lPumpNumber, long lTrsNumber, short nNozzle, long lVolume, long lValue, short nLevel, long lGradePrice, short nGrade)
{
	BOOL  bRetVal = FALSE;
	long  lItemRestriction =0;
	//4.0.9.500
	OLA_STAT ola;
	long lTotalAmount = 0 ;  //4.0.10.491
	long lTotalVolume =0 ; 
	_Module.m_server.m_cPumpArray[lPumpNumber-1].GetOlaStat(&ola);

	if( OLA_STATE(ola.m_byState) == OLA_WAIT_REEFER)
	{
		
		//if (!_Module.m_server.m_cPumpArray[lPumpNumber-1].GetInConpleteTrsNumber(&lDieselTrs))
		//{
			PAY_AT_PUMP_INFO	cPayAtPumpInfo;
			//Added PAK transaction details to Waiting PAP transaction.
			CPumpTransact cTrs;
			
			if (!_Module.m_server.m_cPumpArray[lPumpNumber-1].GetTrsByNumber(lTrsNumber,&cTrs))
			{
				_Module.m_server.m_cPumpArray[lPumpNumber-1].GetPAPInfo(&cPayAtPumpInfo);  //4.0.10.49
				//4.0.9999.0
				//4.0.10.49cTrs.SetStatus(IN_PROGRESS);

				lTotalAmount = lValue + a2l(cPayAtPumpInfo.CardSaleExtraData3.sReeferTotal, sizeof(cPayAtPumpInfo.CardSaleExtraData3.sReeferTotal)); //4.0.10.49
				lTotalVolume = lVolume + a2l(cPayAtPumpInfo.CardSaleExtraData3.sReeferVolume , sizeof(cPayAtPumpInfo.CardSaleExtraData3.sReeferVolume));

				cTrs.m_lRawVolume =     lTotalVolume;
				cTrs.m_lRoundedVolume = lTotalVolume;
				cTrs.m_lRoundedValue =  lTotalAmount;


				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("Fleet totals are  value =%ld ,volume=%ld " ,lTotalAmount ,cTrs.m_lRawVolume ); //4.0.10.45			
					_LOGMSG.LogMsg(str);
				}

				
				_Module.m_server.CentralUpdateTrs(lPumpNumber, lTrsNumber, &cTrs,FALSE);

				
				l2a(lGradePrice , cPayAtPumpInfo.CardSaleInfo.extraData.SALES[3].sUnitPrice , sizeof(cPayAtPumpInfo.CardSaleInfo.extraData.SALES[3].sUnitPrice));
				l2a(lVolume ,cPayAtPumpInfo.CardSaleInfo.extraData.SALES[3].sQty ,sizeof(cPayAtPumpInfo.CardSaleInfo.extraData.SALES[3].sQty));
				l2a(nGrade, cPayAtPumpInfo.CardSaleInfo.extraData.SALES[3].sDPTProdCode , sizeof(cPayAtPumpInfo.CardSaleInfo.extraData.SALES[3].sDPTProdCode));
				l2a(lValue , cPayAtPumpInfo.CardSaleExtraData3.sReeferTotal , sizeof(cPayAtPumpInfo.CardSaleExtraData3.sReeferTotal));	
				l2a(lVolume , cPayAtPumpInfo.CardSaleExtraData3.sReeferVolume , sizeof(cPayAtPumpInfo.CardSaleExtraData3.sReeferVolume));			

				//4.0.10.491				
				Convertl2Str2( lTotalAmount ,cPayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(cPayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt_Msb) , cPayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt, sizeof(cPayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt));	  //4.0.10.491
				Convertl2Str2( lTotalAmount /10 ,cPayAtPumpInfo.CardSaleInfo.extraData.sFuelAmt_Msb,sizeof(cPayAtPumpInfo.CardSaleInfo.extraData.sTotalAmt_Msb) , cPayAtPumpInfo.CardSaleInfo.extraData.sFuelAmt, sizeof(cPayAtPumpInfo.CardSaleInfo.extraData.sFuelAmt));	  //4.0.10.491 //4.0.10.54

				
				_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cPayAtPumpInfo);

				//4.0.10.57
				lItemRestriction = a2l(cPayAtPumpInfo.CardSaleExtraData3.sItemRestriction ,sizeof(cPayAtPumpInfo.CardSaleExtraData3.sItemRestriction));
				

				 if ((lItemRestriction & ADD_CASH_BACK) || 
					  (lItemRestriction & ADD_DRY_ITEMS))
				{				
					
					// Convert PAP to PAK
					ConvertFleetPap2Pak(lPumpNumber ,cTrs, cPayAtPumpInfo);
					_Module.m_server.m_cPumpArray[lPumpNumber-1].ClearOlaStat(); //4.0.10.591
				 }
				 else
				 {
					ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_REQUEST_TO_SEND);
					_Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola); 	
				 }
				//4.0.10.57

				_Module.m_server.SavePumpInfo(lPumpNumber); // save ola stat.
				//4.0.9999.0

				bRetVal = TRUE;  //4.0.10.49
			}

		//}			
	
	}

	return bRetVal;

}

void CFleetCard::ConvertFleetPap2Pak(long &lPumpNumber, CPumpTransact &trs, PAY_AT_PUMP_INFO &cPayAtPumpInfo)
{
		CPumpTransact cTmpTrs;
	cTmpTrs = trs;

	cTmpTrs.SetLink(TRS_NO_LINK,0);
	cTmpTrs.SetOnOffer(0);
	cTmpTrs.SetStatus(UNPAID);
	cTmpTrs.m_lRes = PAY_AT_KIOSK;			

	//4.0.10.181
	// in case no fuel take place add one of the diesel grade.
	if( (!trs.m_lRoundedValue) && (!trs.m_nGrade))
	{
		long  lItemRestriction = a2l(cPayAtPumpInfo.CardSaleExtraData3.sItemRestriction ,sizeof(cPayAtPumpInfo.CardSaleExtraData3.sItemRestriction));

		if (lItemRestriction)
		{
			if (lItemRestriction & ADD_REEFER)
			{
				cTmpTrs.m_nGrade =   (short)_Module.m_server.m_cPumpArray[lPumpNumber-1].GetReeferGrade();
			}
			else
			{
				cTmpTrs.m_nGrade =   (short)_Module.m_server.m_cPumpArray[lPumpNumber-1].GetDieselGrade(); //4.0.10.180
			}
		}

	}


	_Module.m_server.CentralUpdateTrs(lPumpNumber,cTmpTrs.m_lNumber,&cTmpTrs,FALSE);

	long lRtc1 = _Module.m_server.IndexOperation(PUMP_OLA_TEMP_FILE,REC_INSERT, &cTmpTrs, &cPayAtPumpInfo);  //4.0.10.14

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("Diesel transaction was convert to PAK due to items or cash back Pumpnumber=%02ld , trs=%ld , Grade=%d,record was save Rtc=%ld" ,lPumpNumber ,cTmpTrs.m_lNumber , cTmpTrs.m_nGrade , lRtc1);
		_LOGMSG.LogMsg(str);

	}
}

void CFleetCard::ConvertFleetPap2Pak(long &lPumpNumber, CPumpTransact &trs, CXMLInterface &cPayAtPumpInfo)
{
		CPumpTransact cTmpTrs;
	cTmpTrs = trs;

	cTmpTrs.SetLink(TRS_NO_LINK,0);
	cTmpTrs.SetOnOffer(0);
	cTmpTrs.SetStatus(UNPAID);
	cTmpTrs.m_lRes = PAY_AT_KIOSK;			

	//4.0.10.181
	// in case no fuel take place add one of the diesel grade.
	if( (!trs.m_lRoundedValue) && (!trs.m_nGrade))
	{
		//long  lItemRestriction = a2l(cPayAtPumpInfo.CardSaleExtraData3.sItemRestriction ,sizeof(cPayAtPumpInfo.CardSaleExtraData3.sItemRestriction));
		long  lItemRestriction = cPayAtPumpInfo.GetXmlLongField(ATT_ITEM_RESTRICTION);

		if (lItemRestriction)
		{
			if (lItemRestriction & ADD_REEFER)
			{
				cTmpTrs.m_nGrade =   (short)_Module.m_server.m_cPumpArray[lPumpNumber-1].GetReeferGrade();
			}
			else
			{
				cTmpTrs.m_nGrade =   (short)_Module.m_server.m_cPumpArray[lPumpNumber-1].GetDieselGrade(); //4.0.10.180
			}
		}

	}


	_Module.m_server.CentralUpdateTrs(lPumpNumber,cTmpTrs.m_lNumber,&cTmpTrs,FALSE);

	long lRtc1 = _Module.m_server.IndexOperation(PUMP_OLA_TEMP_FILE,REC_INSERT, &cTmpTrs, &cPayAtPumpInfo);  //4.0.10.14

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("Diesel transaction was convert to PAK due to items or cash back Pumpnumber=%02ld , trs=%ld , Grade=%d,record was save Rtc=%ld" ,lPumpNumber ,cTmpTrs.m_lNumber , cTmpTrs.m_nGrade , lRtc1);
		_LOGMSG.LogMsg(str);

	}
}

