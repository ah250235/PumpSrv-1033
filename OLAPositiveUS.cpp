// OLAPositiveUS.cpp: implementation of the COLAPositiveUS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLAPositiveUS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLAPositiveUS::COLAPositiveUS()
{

}

COLAPositiveUS::~COLAPositiveUS()
{

}

/******************************************************************************
 Description:	This procedure was copied from olaRouteSrv 
				in order to call GetCardInfoEx instead of GetCardInfo 
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			25/10/2009   11:30		Start  4.0.23.90 85955 //4.0.23.504
******************************************************************************/

long COLAPositiveUS::GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData, long lFlags, long *lSesionType, long lPumpNumber, PAY_AT_PUMP_INFO * pInPAPBuffer)	//4.0.20.52 63936
{
	long lOlaRecSize = (lFlags & SWIPE_THREE_OLA_REC)?sizeof(CARD_SALE_ALL3):sizeof(CARD_SALE_ALL2);
	DWORD dwLastErr = 0;
	GCIRetCode retCode = GCI_OK;	

	if(PUMPSRV_NOT_INIT)
		retCode = GCI_MUST_INIT_FIRST;
	else if (FALSE == _Module.m_server.DBConnectionAvailable())
		retCode = GCI_FAILED_IO;
	else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
		retCode = GCI_SERVER_IN_MIDDLE_LOCK_SHIFT;   //4.0.1.40

	long lRtc = 0;
	BOOL bAskUserToTryAgain = FALSE;

	EnterCriticalSection(&m_csOLASrv); //4.0.22.509 75755

	try	//4.0.22.509 75755
	{
		if ( retCode == GCI_OK )
		{
			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
//			EnterCriticalSection( &_Module.m_server.m_csPumpArray ); //4.0.22.509 75755

			_Module.m_server.m_cOLASrv->ResetConnectionAttempts();
			_Module.m_server.m_cOLASrv->ResetRetryAttemps(TRUE);

			PAY_AT_PUMP_INFO	info;
				
			if (pInPAPBuffer == NULL)	//4.0.20.52 63936
			{
				_Module.m_server.m_cPumpArray.GetAt(lPumpNumber-1).GetPAPInfo(&info);				   
			}
			else
				info = *pInPAPBuffer;
			
			
			if (sAccountNumber)
			{
				_bstr_t sTmpAccount(sAccountNumber, true);
				if (sTmpAccount.length())
					memcpy(info.CardSaleInfo.cardData.sAccountNumber , (char *)sTmpAccount, min(sTmpAccount.length() ,sizeof(info.CardSaleInfo.cardData.sAccountNumber)));
				else
					memset(info.CardSaleInfo.cardData.sAccountNumber , ' ', sizeof(info.CardSaleInfo.cardData.sAccountNumber));

			}
			else
			{
				memset(info.CardSaleInfo.cardData.sAccountNumber , ' ', sizeof(info.CardSaleInfo.cardData.sAccountNumber));
			}

			if (sTrack1)
			{
				_bstr_t sTmpTrack1(sTrack1, true);
				if (sTmpTrack1.length())
					memcpy(info.CardSaleInfo.cardData.sTrack1Data , (char *)sTmpTrack1, min(sTmpTrack1.length() ,sizeof(info.CardSaleInfo.cardData.sTrack1Data)));
				else
					memset(info.CardSaleInfo.cardData.sTrack1Data , ' ', sizeof(info.CardSaleInfo.cardData.sTrack1Data));
			}
			else
			{
				memset(info.CardSaleInfo.cardData.sTrack1Data , ' ', sizeof(info.CardSaleInfo.cardData.sTrack1Data));
			}

			if (sTrack2)
			{
				_bstr_t sTmpTrack2(sTrack2, true);
				if (sTmpTrack2.length())
					memcpy(info.CardSaleInfo.cardData.sTrack2Data , (char *)sTmpTrack2, min(sTmpTrack2.length() ,sizeof(info.CardSaleInfo.cardData.sTrack2Data)));
				else
					memset(info.CardSaleInfo.cardData.sTrack2Data , ' ', sizeof(info.CardSaleInfo.cardData.sTrack2Data));
			}
			else
			{
				memset(info.CardSaleInfo.cardData.sTrack2Data , ' ', sizeof(info.CardSaleInfo.cardData.sTrack2Data));

			}

			PAY_AT_PUMP_INFO    cPayAtPumpInfo;  //4.0.5.0

			memset(&cPayAtPumpInfo , ' ' , sizeof(cPayAtPumpInfo));

			char sOutBuffer[3048]; //4.0.5.0
			memset((char*)sOutBuffer , 0 , sizeof(sOutBuffer));

			_Module.m_server.m_cOLASrv->BuildRequest(SESSION_AUTHORIZE,
												 lPumpNumber,
												 (char*)&info,
												 (char*)sOutBuffer,
												 NULL, TRUE); // 4.0.6.501	


			_bstr_t InBuff(sOutBuffer);
			CComBSTR  sRetBuf ;

 		//	long lRtc  = m_pSrvPtr->GetCardInfo(0, InBuff ,(unsigned short **) &sRetBuf);
			long lRtc  = GetCardInfoEx(99, InBuff , &sRetBuf, 0);

			CString sOutstr, strNew;
			sOutstr.Format("%s",sOutBuffer);
			_Module.m_server.m_cProtectedDataHandler.RemoveDataFromLog(sOutstr,strNew);	//4.0.22.501 TD 72716


			CString str;
			str.Format(">>> COLAPositiveUS:GetCardInfo ");
			_LOGMSG.LogOLABufferMsg(lPumpNumber,LOG_PUMP,str,(LPCTSTR)strNew);

			if (!lRtc)
			{
				*pRetData = sRetBuf.Copy();
			}


			// M.L 8/5/98 Lock PumpArray in large scope 
			//-----------------------------------------
//			LeaveCriticalSection(&_Module.m_server.m_csPumpArray); //4.0.22.509 75755
	
		}
	}
	catch (_com_error & e) //4.0.22.509 75755
	{
		Error(e);		
		lRtc = OLASRV_COM_ERROR;
		_Module.m_server.CloseOLAThread();
	}
	
	catch(...) //4.0.22.509 75755
	{
		Error();		
		lRtc = OLASRV_COM_ERROR;
		_Module.m_server.CloseOLAThread();
	}

	LeaveCriticalSection(&m_csOLASrv);	//4.0.22.509 75755

	return retCode;

}

/******************************************************************************
 Description:	This procedure matches the sent loyalty card info as was defined 
				by OLA RouteSrv to the current OLA. Only for Loyalty flow.
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			15/11/2009   12:12		Start  4.0.23.507 89548
******************************************************************************/
void COLAPositiveUS::MatchSentLoyaltyCardInfoToOLA(PAY_AT_PUMP_INFO &pInOutPAPBuffer)  
{
	switch (pInOutPAPBuffer.CardSaleInfo.cardData.sEntryMethod)
	{
	case 'S':
		{
			memcpy(pInOutPAPBuffer.CardSaleInfo.cardData.sTrack2Data, pInOutPAPBuffer.CardSaleInfo.cardData.sClubCardTrack2Data, min(sizeof(pInOutPAPBuffer.CardSaleInfo.cardData.sTrack2Data), sizeof(pInOutPAPBuffer.CardSaleInfo.cardData.sClubCardTrack2Data)));
			break;
		}
	case 'M':
		{
			memcpy(pInOutPAPBuffer.CardSaleInfo.cardData.sTrack2Data, pInOutPAPBuffer.CardSaleInfo.extraData.sLoyalId, min(sizeof(pInOutPAPBuffer.CardSaleInfo.cardData.sTrack2Data), sizeof(pInOutPAPBuffer.CardSaleInfo.extraData.sLoyalId)));
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


/******************************************************************************
 Description:	This procedure matches the received loyalty card info as was defined 
				by OLA RouteSrv to the current OLA. Only for Loyalty flow.
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			15/11/2009   12:12		Start  4.0.23.507 89548
******************************************************************************/
void COLAPositiveUS::MatchReceivedLoyaltyCardInfoToOLA(CARD_SALE_ALL3 &pInOutPAPBuffer)  
{
	//Always return the result in ClubCardNumber[S]
/*	memcpy(pInOutPAPBuffer.CardSaleAll.extData.sLoyalId, pInOutPAPBuffer.CardSaleAll.data.sAccountNumber, min(sizeof(pInOutPAPBuffer.CardSaleAll.extData.sLoyalId), sizeof(pInOutPAPBuffer.CardSaleAll.data.sAccountNumber)));
	memset(pInOutPAPBuffer.CardSaleAll.data.sAccountNumber, ' ', sizeof(pInOutPAPBuffer.CardSaleAll.data.sAccountNumber));
	if (LOG_BASIC_FLOW_CONTROL)
	{
		_LOGMSG.LogMsg("Loyalty card Info was matched to OLA after GetCardInfo");
	}4.0.23.190 104610 4.0.24.60*/
}
