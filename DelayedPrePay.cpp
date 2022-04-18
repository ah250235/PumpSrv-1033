// DelayedPrePay.cpp: implementation of the CDelayedPrePay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DelayedPrePay.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDelayedPrePay::CDelayedPrePay()
{

}

CDelayedPrePay::~CDelayedPrePay()
{

}
/******************************************************************************
 Description:	Generate pre pay transaction from PAP information 
 Returns:      	operation success/fail 
 Parameters:   	long lPumpNumber ,long & lMaxValue
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			22/1/2003   15:34		Start
******************************************************************************/

long CDelayedPrePay::CreateNewPrePay(long lPumpNumber, PAY_AT_PUMP_INFO *pInfo)
{
	long lRtc= 0;	
	CPumpTransact trs;
	PRE_PAY_REC		PrePayRec;	
	EXTRA_INFO_REC	ExtraInfoRec;
	CString sMsg;

	if(_Module.m_server.m_cPumpArray[lPumpNumber -1 ].GetTrsByStatus(WAIT_PROGRESS,TRS_LINK_OLA_NUMBER,&trs))
	{  
		//4.0.2.23
		long lMaxValue =  ConvertStringsToLong(pInfo->CardSaleInfo.cardData.sAuthAmt,sizeof(pInfo->CardSaleInfo.cardData.sAuthAmt),pInfo->CardSaleInfo.extraData.sAuthAmt_Msb,sizeof(pInfo->CardSaleInfo.extraData.sAuthAmt_Msb));
		long lPosTrsNumber = a2l(pInfo->CardSaleExtraData2.sCustomerReference ,sizeof(pInfo->CardSaleExtraData2.sCustomerReference));

		lMaxValue *= 10;
		memset(&PrePayRec , 0, sizeof(PrePayRec));
		// found								
		trs.SetOnOffer(SERVER_OLA_POS_NUMBER);				
		PrePayRec.info.lPOSNumber		= SERVER_OLA_POS_NUMBER;					
		PrePayRec.info.lSyncNumber	= trs.m_lNumber;
		trs.UpdateTime();
		trs.SetLink(TRS_LINK_PREPAY_NUMBER ,PrePayRec.info.lSyncNumber);		
		trs.SetStatus(WAIT_RES);			
		trs.m_lActiveShiftNumber = _Module.m_server.GetActiveShiftNumber();
		trs.m_lPump = lPumpNumber;			
		PrePayRec.info.lPumpNumber	=	lPumpNumber;
		PrePayRec.info.lMaxValue =lMaxValue;
		PrePayRec.info.lMaxVolume = lPosTrsNumber; 
		PrePayRec.info.lPOSNumber = GetPosNumber();

 		_Module.m_server.CentralUpdateTrs(lPumpNumber, trs.m_lNumber,&trs ,FALSE);
		memset(&ExtraInfoRec, 0, sizeof(ExtraInfoRec));
		ExtraInfoRec.info.lIndexNumber = trs.m_lNumber;
		PrePayRec.info.lFlags = LIMIT_BY_VALUE; //4.0.9.507
	
		if(lRtc = _Module.m_server.TrsLinkOperation(REC_INSERT,&trs,&PrePayRec,&ExtraInfoRec))
		{
			lRtc =1;
		}

 		if(LOG_BASIC_FLOW_CONTROL)
		{		
			CString sMsg;
			sMsg.Format("MaxValue=%ld , lPosTrsNumber=%ld , lPosNumber=%ld" ,lMaxValue , PrePayRec.info.lPOSNumber);
			_LOGMSG.LogMsg("CDelayedPrePay","CreateNewPrePay",lPumpNumber,LOG_PUMP,sMsg);
		}
	}
	else
	{
		lRtc=1;  
	}

	return lRtc;
}
//4.0.2.31
/******************************************************************************
 Description:	Find the first Active Pos 	
 Returns:      	Connected Pos 
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			18/2/2003   13:05		Start
******************************************************************************/
long CDelayedPrePay::GetPosNumber()
{
	CPosInfo tmpPos;
	POSITION position = _Module.m_server.m_cPosMap.GetStartPosition();
	long lPOSNumber = 1;
	long lTmpPOSNumber = 0;

	while(position)
	{
		_Module.m_server.m_cPosMap.GetNextAssoc(position,lTmpPOSNumber,tmpPos);
			
		if (tmpPos.IsRealPOS() && tmpPos.IsPOSConnected())
		{
			lPOSNumber = lTmpPOSNumber;
			break;
		}		
	}

	return lPOSNumber;
}
//4.0.2.31