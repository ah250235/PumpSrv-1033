// OLABase.cpp: implementation of the COLABase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLABase.h"
#include "math.h"
#include "CardSaleAllData.h"
#include <list>
#include <string>
#include "SQLPumpSrvFile.h"
#include "CStrFormat.h"


#define  OLA_START_ZONE_INDEX 10


LPCSTR TransAmountField(bool bSessionComplete)
{
/*	if (bSessionComplete && _Module.m_server.m_bOLAEpsilonSupport)
	{*/
		return "TransAmount[F]";
	/*}
	else
	{
		return "TransAmount[F]";
	}*/
}


LPCTSTR sDepName[] = { //change this back to sDepName
   "G01",
   "G02",
   "G03",
   "G04",
   "G05",   
   "G06",
   "G07",
   "G08",
   "G09",
   "G10",
   "G11",
   "G12",
   "G13",
   "G14",
   "G15",
   "G16",
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLABase::COLABase():m_bUseSessionEnd(0)
{
    InitializeCriticalSection(&m_csOLASrv);
    InitializeCriticalSection(&m_csRetry);//4.0.3.38
    m_bValid = FALSE;
    m_bOLAStopped = FALSE;	 
    m_nDeclineCounter = 0;    //2.9.3.4
    memset(&m_lQuerySessionResult , 0 , sizeof(m_lQuerySessionResult)); //4.0.9999.0
    m_lSessionResult=0;
    //ola12345
    _Module.m_server.m_enmOlaThreadStatus =  OLA_SERVER_OFF;
    _Module.m_server.InitOLAThreadInfo();
    _Module.m_server.ConstructOLAThreadInfo();

    //4.0.2.37 OLARetries no longer read from registry
    
    m_byConnectionAttempts = m_byRetryAttemps = MAX_OLA_RETRIES;

    m_bNoMoreConnectionRetries = m_bNoMoreRetries = FALSE;
    m_bSupportIFSF = FALSE;		// 4.0.5.39
    m_bPreAuthIFSF = FALSE;		// 4.0.7.507
    m_pbsrOlaBuffer = NULL;		// 4.0.25.80 TD 142857

    m_bAddPreAuthInfo = FALSE;	// 4.0.27.501 TD 235737
}

COLABase::~COLABase()
{
	// Fixed Kernal Object leak
	DeleteCriticalSection(&m_csRetry);
	DeleteCriticalSection(&m_csOLASrv);
}


/////////////////////////////////////////////////////////

long COLABase::BuildRequest(short byOlaStat, long lPumpNumber, char * pInBuffer, char *pOutBuffer,CPumpTransact *pTrs ,BOOL bBuildBufferToClient)
{
    PAY_AT_PUMP_INFO *pInfo;
    CARD_SALE_DATA			*pCardData;
    CARD_SALE_EXTRA_DATA	*pCardDataExtra;
    CARD_SALE_EXTRA_DATA2_  *pCardDataExtra2;
    CARD_SALE_EXTRA_DATA3_  *pCardDataExtra3;
    CARD_SALE_EXTRA_DATA4_	*pCardDataExtra4; //4.0.16.504
    CARD_SALE_EXTRA_DATA5_	*pCardDataExtra5; //4.0.18.501
	CARD_SALE_EXTRA_DATA6_	*pCardDataExtra6;

    BYTE sTmpDate[8];
    BYTE sTmpTime[6];
    BYTE sTmpCustomerData[12] = {0}; //4.0.22.511 74797
    BYTE sTmpUserID[12] = {0}; //4.0.22.511 74797
    SYSTEMTIME systime;	
    long lGRTotalDiscount = 0, lGasLoyaltyDiscountPerGallon = 0;	//4.0.14.1540 69922 //4.0.22.0
    long lTmp = 0;
    BYTE sIncludeDiscountFlags, sForceZeroPAKCompletion;	//4.0.14.1390 67387 //4.0.21.0
    BYTE sTmpParamResult[118] = {0};

    long lPumpIndex = lPumpNumber-1;

    pInfo = (PAY_AT_PUMP_INFO *)pInBuffer;

    pCardData		= &(pInfo->CardSaleInfo.cardData);
    pCardDataExtra	= &(pInfo->CardSaleInfo.extraData);
    pCardDataExtra2	= &(pInfo->CardSaleExtraData2);
    pCardDataExtra3 = &(pInfo->CardSaleExtraData3);
    pCardDataExtra4 = &(pInfo->CardSaleExtraData4);	//4.0.16.504
    pCardDataExtra5 = &(pInfo->CardSaleExtraData5);	//4.0.18.501
	pCardDataExtra6 = &(pInfo->CardSaleExtraData6);

    long lOLAPumpServiceMode = 0;
    CPumpStatus cPumpStatus; //4.0.14.1390 67387
    if(!PUMP_NOT_VALID(lPumpNumber))	//4.0.19.500 58863 
    {
        long lServiceMode = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat().GetServiceMode();
        cPumpStatus = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat(); //4.0.14.1390 67387 //4.0.21.0

        switch(	lServiceMode )
        {
            case FULL_SERVICE_MODE:
                lOLAPumpServiceMode = 1;
                break;
            case SELF_SERVICE_MODE:
                lOLAPumpServiceMode = 2;
                break;
        }
    }

    long lValue = 0, lVolume =0, lGradePrice = 0, lTotalAmount = 0, lServiceFee = 0,lTransactionId = 0;  
    short nGrade = 0;

    _Module.m_server.m_cProtectedDataHandler.RemoveConfidencesDataFromMemory(SESSION_TYPE(byOlaStat), pInfo);		//4.0.24.60 TD 112773
    
    if( (SESSION_TYPE(byOlaStat) == SESSION_COMPLETE) ||
        (SESSION_TYPE(byOlaStat) == SESSION_CANCEL)   ||
        (SESSION_TYPE(byOlaStat) == SESSION_SALE)	  ||
        (SESSION_TYPE(byOlaStat) == SESSION_CAPTURE)) //5.0.0.55

    {
        if(pTrs)
        {
            lValue = pTrs->m_lRoundedValue;
            lVolume = pTrs->m_lRoundedVolume;
            lGradePrice = pTrs->m_lGradePrice;
            nGrade = pTrs->m_nGrade;
            if(_Module.m_server.m_cPumpArray[lPumpNumber - 1].IsForceZeroPakCompletionOn())
                lTransactionId = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPapToPakTrsNum(); // TD 256101
            else
                lTransactionId = pTrs->m_lNumber;  //4.0.14.502
        }
        
        lGRTotalDiscount = a2l(pInfo->CardSaleInfo.extraData.sGRTotalDiscount,sizeof(pInfo->CardSaleInfo.extraData.sGRTotalDiscount));
        //4.0.14.1540 69922 start //4.0.22.0
        long lGradePos = _Module.m_server.GetGradePositionRestrection(nGrade, lPumpNumber);
        if (lGradePos > 0) lGradePos--;  //4.0.23.1411
        lGasLoyaltyDiscountPerGallon = a2l(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount,sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount));
        //4.0.14.1540 69922 end //4.0.22.0

        lTotalAmount = ConvertStringsToLong(pInfo->CardSaleInfo.extraData.sTotalAmt,sizeof(pInfo->CardSaleInfo.extraData.sTotalAmt),pInfo->CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(pInfo->CardSaleInfo.extraData.sTotalAmt_Msb));

        if ((lTotalAmount == 0) && lValue)
            lTotalAmount =  lValue; 

        if(_Module.m_server.GetTwoTendersInTransaction() && lValue == 0)
        {
            lTotalAmount = 0;
            ConvertLongToStrings(lTotalAmount,pInfo->CardSaleInfo.cardData.sAuthAmt,sizeof(pInfo->CardSaleInfo.cardData.sAuthAmt),pInfo->CardSaleInfo.extraData.sAuthAmt_Msb,sizeof(pInfo->CardSaleInfo.extraData.sAuthAmt_Msb));
        }

        lServiceFee = a2l(pCardDataExtra2->sServiceFee,sizeof(pCardDataExtra2->sServiceFee));

        if(LOG_DETAIL_FLOW_CONTROL)
        {
            CString str;
            long lTmpAmountTotal = ConvertStringsToLong(pInfo->CardSaleInfo.extraData.sTotalAmt,sizeof(pInfo->CardSaleInfo.extraData.sTotalAmt),pInfo->CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(pInfo->CardSaleInfo.extraData.sTotalAmt_Msb));
            str.Format(">>> Complete : TotalAmount %ld (lValue = %ld, lGRTotalDiscount = %ld) (pInfo->extraData.sTotalAmt = %ld, GasLoyaltyDiscountPerGallon = %ld)",lTotalAmount,lValue,lGRTotalDiscount * 10,lTmpAmountTotal, lGasLoyaltyDiscountPerGallon);	//4.0.14.1540 69922 //4.0.22.0

            _LOGMSG.LogMsg("COLABase","BuildRequest",0,LOG_NONE,str);
        }
    }
    else
    {
        /*
        if (_Module.m_server.IsContactlessCard(lPumpNumber))
        {
            lTotalAmount = Convert2Str2l(pInfo->CardSaleInfo.extraData.sTotalAmt,sizeof(pInfo->CardSaleInfo.extraData.sTotalAmt),pInfo->CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(pInfo->CardSaleInfo.extraData.sTotalAmt_Msb));
            
            // CR 434089 2015/05/26 JackD
            long lSiteCLessLimit = lTotalAmount;

            if(LOG_DETAIL_FLOW_CONTROL)
            {
                CString str;			
                str.Format("ContactlessCard TotalAmount=%d, SiteCLessLimit=%d", lTotalAmount, lSiteCLessLimit);
                _Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest",0,LOG_NONE,str);
            }

            Convertl2Str2(lTotalAmount, pCardDataExtra->sAuthAmt_Msb,sizeof(pCardDataExtra->sAuthAmt_Msb) , 
                                        pCardData->sAuthAmt, sizeof(pCardData->sAuthAmt));	
            Convertl2Str2(lTotalAmount, pCardDataExtra->sTotalAmt_Msb,sizeof(pCardDataExtra->sTotalAmt_Msb) , 
                                        pCardDataExtra->sTotalAmt, sizeof(pCardDataExtra->sTotalAmt));	

        }
        else*/ 
        if ((	(_Module.m_server.GetTypeLimitByUser(DEBIT_LIMIT_BY_USER)) && (pCardData->sTranType ==CD_TYPE_DEBIT )) ||
           ((_Module.m_server.GetTypeLimitByUser(DEBIT_LIMIT_BY_USER)) && _Module.m_server.IsInteracCard(lPumpNumber)) ||																											  																											  
            ((_Module.m_server.GetTypeLimitByUser(CREDIT_LIMIT_BY_USER)) && (pCardData->sTranType ==CD_TYPE_CREDIT ))  	)
        {
            lTotalAmount = ConvertStringsToLong(pInfo->CardSaleInfo.extraData.sTotalAmt,sizeof(pInfo->CardSaleInfo.extraData.sTotalAmt),pInfo->CardSaleInfo.extraData.sTotalAmt_Msb,sizeof(pInfo->CardSaleInfo.extraData.sTotalAmt_Msb));
            
            if(LOG_DETAIL_FLOW_CONTROL)
            {
                CString str;			
                str.Format("Debit Limit = %ld", lTotalAmount);
                _LOGMSG.LogMsg("COLABase", "BuildRequest", 0, LOG_NONE, str);
            }

            if (pCardData->sTranType == CD_TYPE_DEBIT )
            {
                long lTmpPreAuth = ConvertStringsToLong(pCardData->sAuthAmt,sizeof(pCardData->sAuthAmt),pCardDataExtra->sAuthAmt_Msb   ,sizeof(pCardDataExtra->sAuthAmt_Msb));

                if (lTotalAmount)  //4.0.17.501
                {
                    Convertl2Str2(lTotalAmount /10 ,pCardDataExtra->sAuthAmt_Msb,sizeof(pCardDataExtra->sAuthAmt_Msb) , pCardData->sAuthAmt, sizeof(pCardData->sAuthAmt));	
                }

                if(LOG_DETAIL_FLOW_CONTROL)
                {
                    CString str;
                
                    str.Format("Debit Limit = %ld, sAuthAmt = %ld", lTotalAmount, lTmpPreAuth);
                    _LOGMSG.LogMsg("COLABase", "BuildRequest", 0, LOG_NONE, str);
                }
            }
        }
        else
        {
            long lDeviceLimit = 0;
            // this information already written in CardSale construct by function - 
            // ConstructAndSaveCardData , to save convertin (a2l,l2a) , 
            // take the original value

            lDeviceLimit = ConvertStringsToLong(pCardData->sAuthAmt,sizeof(pCardData->sAuthAmt),pCardDataExtra->sAuthAmt_Msb   ,sizeof(pCardDataExtra->sAuthAmt_Msb));				// 3.2.1.73 

            if (lDeviceLimit)
            {
                if(LOG_DETAIL_FLOW_CONTROL)
                {
                    CString str;
                    str.Format(">>> COLASrv::BuildRequest Send Device limit = %ld", lDeviceLimit);
                    _LOGMSG.LogMsg("COLABase", "BuildRequest", 0, LOG_NONE, str);
                }

                lDeviceLimit *= 10;					//4.0.1.40
                lValue = lDeviceLimit;
            } //3.2.1.73
			else
				lValue = _Module.m_server.GetRequstedAuthorizeAmount(lPumpNumber);

            if (_Module.m_server.IsNeedCreditLimitCheck())		//3.2.1.23 //3.2.1.32
            {
                long lTmpValue = 0;  //4.0.2.23
                long lDeviceLimit =0;  //3.2.1.73
                lDeviceLimit = ConvertStringsToLong(pCardData->sAuthAmt,sizeof(pCardData->sAuthAmt),pCardDataExtra->sAuthAmt_Msb   ,sizeof(pCardDataExtra->sAuthAmt_Msb));
                
                lDeviceLimit *= 10;					

                if ((lDeviceLimit > lValue) || (!lDeviceLimit))  //3.2.1.41
                {
                    lTmpValue = lValue / 10;
                    unsigned char sTmpValue[11] = {0};							
                    l2a0((lTmpValue), sTmpValue , 10);			//4.0.2.32
                    memcpy(pCardData->sAuthAmt , &sTmpValue[5] ,sizeof(pCardData->sAuthAmt));
                    memcpy(pCardDataExtra->sAuthAmt_Msb , sTmpValue ,sizeof(pCardDataExtra->sAuthAmt_Msb));		
                }
				else
					lValue = lDeviceLimit;
            }

			lTotalAmount = lValue;			  //3.2.1.23 3.2.1.32
        }
    }

    char sExpire[6];
    memset(sExpire,0,sizeof(sExpire));
    //Check if we got an Expire date or empty string
    if ( !(pCardData->sExpire[0] == ' ') && !(pCardData->sExpire[0] == 0) )
    {
        char sYear[3];
        //If the year is bigger then 70 then it 19XX else it 20XX
        memset(sYear,0,sizeof(sYear));
        memcpy(sYear,pCardData->sExpire,2);

        // Adir - TD 260513 - Y2K is behind us...
// 		if (atoi(sYear) > 70 )
// 			memcpy(sExpire,"19",2);
// 		else

        memcpy(sExpire,"20",2);
        memcpy(&sExpire[2],pCardData->sExpire,4);
    }


#ifdef NEW_OLA_STRUCT

    long lTrsFee = a2l(pCardData->sSurchargeAmt,sizeof(pCardData->sSurchargeAmt));
    long lOffset =0;
    CString str("Grades\\Grade");
    char sNumber[8];
    memset(sNumber,0,sizeof(sNumber));
    sprintf_s(sNumber,3,"%02d",nGrade);
    str += sNumber;

    if( ((SESSION_TYPE(byOlaStat) == SESSION_INIT_DEBIT) || (SESSION_TYPE(byOlaStat) == SESSION_REPORT_PRICE_CHANGE))  &&   //4.0.9.510
        (OLA_STATE(byOlaStat) == OLA_REQUEST_TO_SEND) )
    {	
        //3.2.0.69
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PumpNumber[I]",	(BYTE *)"%s=%ld,",		lPumpNumber);
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TerminalId[S]",	(BYTE *)"%s=%04ld,",	(long)SERVER_OLA_POS_NUMBER);
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"StoreNumber[S]",(BYTE *)"%s=%.4ld,",	atol(_Module.m_server.GetSiteID())); //3.2.0.69
        //3.2.0.69
    
        if ((SESSION_TYPE(byOlaStat)) == SESSION_REPORT_PRICE_CHANGE )
        {
            AddPriceInformation(lOffset, pOutBuffer);		
        }
    }
    else
    {
        long lMediaMasterType =0, lCardSegment = 0; //4.0.5.24
        long lTrsFee = a2l(pCardData->sSurchargeAmt,sizeof(pCardData->sSurchargeAmt));
        
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			LogStream sLog;
			sLog << "-Auth=" << ((SESSION_TYPE(byOlaStat) == SESSION_AUTHORIZE) ? "Y" : "N")
				<< " ReqSend=" << ((OLA_STATE(byOlaStat) == OLA_REQUEST_TO_SEND) ? "Y" : "N")
				<< " SendMore=" << ((OLA_STATE(byOlaStat) == OLA_REQUEST_TO_SEND_MORE) ? "Y" : "N")
				<< " WaitForMore=" << ((OLA_STATE(byOlaStat) == OLA_WAIT_FOR_MORE_INFO) ? "Y" : "N")
				<< std::endl;

			_LOGMSG.LogMsg(sLog);
		}


        if ((SESSION_TYPE(byOlaStat) == SESSION_AUTHORIZE) && ((OLA_STATE(byOlaStat) == OLA_REQUEST_TO_SEND) || (OLA_STATE(byOlaStat) == OLA_REQUEST_TO_SEND_MORE) || (OLA_STATE(byOlaStat) == OLA_WAIT_FOR_MORE_INFO)))
        {
			if (!ChrAll((char *)pCardDataExtra4->dallasKeysInfo.sBlockedListBuffer, sizeof(pCardDataExtra4->dallasKeysInfo.sBlockedListBuffer))) {
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"EMVExtendedBuffer[S]", (BYTE *)"%s=%.512s,", pCardDataExtra4->dallasKeysInfo.sBlockedListBuffer, sizeof(pCardDataExtra4->dallasKeysInfo.sBlockedListBuffer));
			}
			else {
				_LOGMSG.LogMsg("sBlockedListBuffer empty");
			}
        }

		//RFUEL-2817
        if (_Module.m_server.GetOLAInUse() == OLASRV_EPSILON || 
			_Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT)
        {
            if (pTrs)
                lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransNumber[I]", (BYTE *)"%s=%ld,", pTrs->m_lNumber);
        }
        else
        {
            lOffset += SetParamNoCheck(pOutBuffer + lOffset, (BYTE *)"TransNumber[I]", (BYTE *)"%s=%.5s,", pCardData->sTranNbr, sizeof(pCardData->sTranNbr));
        }

        long lTmpShiftId = _Module.m_server.GetActiveShiftNumber();
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ShiftNumber[S]", (BYTE *)"%s=%ld,", lTmpShiftId);

        // TD 387091
        BYTE sTmpAuthNumber[sizeof(pCardData->sAuthNumber) + sizeof(pInfo->CardSaleExtraData5.sAuthNumberEx)];
        memset(sTmpAuthNumber, ' ', sizeof(sTmpAuthNumber));

        PackFields(pInfo->CardSaleExtraData5.sAuthNumberEx, sizeof(pInfo->CardSaleExtraData5.sAuthNumberEx), pCardData->sAuthNumber, sizeof(pCardData->sAuthNumber), sTmpAuthNumber);	
        if (!ChrAll( (char *)sTmpAuthNumber, sizeof(sTmpAuthNumber)) )
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AuthorizationNumber[S]",  (BYTE *)"%s=%.17s,", sTmpAuthNumber, sizeof(sTmpAuthNumber) );			
        }

        lOffset += SetParamNoCheck(pOutBuffer+lOffset,(BYTE *)"MediaType[S]",	(BYTE *)"%s=%.1s,",			&(pCardData->sTranType),sizeof(pCardData->sTranType));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PumpNumber[I]",		(BYTE *)"%s=%ld,",			lPumpNumber);
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ModeOfService[I]",	(BYTE *)"%s=%ld,",			lOLAPumpServiceMode);

        BuildTransAmount(lPumpNumber, lTotalAmount, byOlaStat,pInfo,lOffset, pOutBuffer);


		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "PinPadCapabilities[I]", (BYTE *)"%s=%.3s,", pInfo->CardSaleExtraData5.sPinPadCapabilities, sizeof(pInfo->CardSaleExtraData5.sPinPadCapabilities));
		

        lOffset += SetParam(pOutBuffer + lOffset,(BYTE *)"TransTaxAmount[F]",	(BYTE *) "%s=%ld.00,",		0L);
        lOffset += SetParam(pOutBuffer + lOffset,(BYTE *)"TransactionFee[F]",	(BYTE *) "%s=%ld.%03ld,",	lTrsFee / 1000, lTrsFee % 1000);
        		
        lOffset += SetParamNoCheck(pOutBuffer+lOffset,(BYTE *)"Track1[S]",	(BYTE *) "%s=%.76s,",		pCardData->sTrack1Data, sizeof(pCardData->sTrack1Data));
        lOffset += SetParamNoCheck(pOutBuffer+lOffset,(BYTE *)"Track2[S]",	(BYTE *) "%s=%.37s,",		pCardData->sTrack2Data, sizeof(pCardData->sTrack2Data));
        lOffset += SetParamNoCheck(pOutBuffer+lOffset,(BYTE *)"Track3[S]",	(BYTE *) "%s=%.104s,",		pCardDataExtra3->sTrack3Data, sizeof(pCardDataExtra3->sTrack3Data));//4.0.11.500

        // TD 347994 
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"AccountNumber[S]", (BYTE *)"%s=%.21s,", pCardData->sAccountNumber, sizeof(pCardData->sAccountNumber));

        //TD 317615 removed NON PCI log writing.
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TerminalId[S]",		(BYTE *)"%s=%04ld,",		(long)SERVER_OLA_POS_NUMBER);
        
        // TD 389050
        long lOdometer = GetOdometerFromCardSale(pCardDataExtra4, pCardDataExtra, pCardData);
        if(lOdometer)
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"Odometer[S]",(BYTE *)"%s=%d,",lOdometer);

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CarRegistration[S]",(BYTE *)"%s=%.16s,",		pCardData->sFullVehicleId,sizeof(pCardData->sFullVehicleId));

        //4.0.22.511 74797 start		
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"Job[S]",			(BYTE *)"%s=%.12s,",		pCardDataExtra4->sJob,sizeof(pCardDataExtra4->sJob));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"Department[S]",		(BYTE *)"%s=%.12s,",		pCardDataExtra4->sDepartment,sizeof(pCardDataExtra4->sDepartment));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"License[S]",		(BYTE *)"%s=%.10s,",		pCardDataExtra4->sLicense,sizeof(pCardDataExtra4->sLicense)); //4.0.23.500 74797

        memcpy(sTmpCustomerData, pCardDataExtra4->sCustomerReferenceExt, sizeof(pCardDataExtra4->sCustomerReferenceExt));
        memcpy(&sTmpCustomerData[2], pCardDataExtra2->sCustomerReference, sizeof(pCardDataExtra2->sCustomerReference));

        if (!ChrAll((char*)pCardDataExtra4->sCustomerReferenceExt, sizeof(pCardDataExtra4->sCustomerReferenceExt)))
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CustomerData[S]",	(BYTE *)"%s=%.12s,",		sTmpCustomerData, sizeof(sTmpCustomerData));
        }	
        else
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CustomerData[S]",	(BYTE *)"%s=%.10s,",		&sTmpCustomerData[2], sizeof(pCardDataExtra2->sCustomerReference));
        }

        memcpy(sTmpUserID, pCardDataExtra4->sCustomerIDExt, sizeof(pCardDataExtra4->sCustomerIDExt));
        memcpy(&sTmpUserID[2], pCardDataExtra3->sCustomerID, sizeof(pCardDataExtra3->sCustomerID));
        if (!ChrAll((char*)pCardDataExtra4->sCustomerIDExt, sizeof(pCardDataExtra4->sCustomerIDExt)))
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"UserID[I]",	(BYTE *)"%s=%.12s,",	sTmpUserID, sizeof(sTmpUserID));
        }
        else
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"UserID[I]",	(BYTE *)"%s=%.10s,",	&sTmpUserID[2], sizeof(pCardDataExtra3->sCustomerID));
        }

        //4.0.22.511 74797 end		

        //4.0.23.370 117473 start //4.0.24.90
        BYTE sTmpDriverId[10] = {0};
        memcpy(sTmpDriverId , pCardDataExtra4->sDriverId_MSB2 , sizeof(pCardDataExtra4->sDriverId_MSB2));	
        memcpy(&sTmpDriverId[2] , pCardDataExtra3->sDriverId_MSB , sizeof(pCardDataExtra3->sDriverId_MSB));
        memcpy(&sTmpDriverId[4] , pCardData->sDriverId , sizeof(pCardData->sDriverId));
        if (!ChrAll((char*)pCardDataExtra4->sDriverId_MSB2, sizeof(pCardDataExtra4->sDriverId_MSB2)))
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"DriverID[S]",		(BYTE *)"%s=%.10s,",			sTmpDriverId, sizeof(sTmpDriverId)); 
        }
        else if (!ChrAll((char*)pCardDataExtra3->sDriverId_MSB, sizeof(pCardDataExtra3->sDriverId_MSB)))
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"DriverID[S]",		(BYTE *)"%s=%.8s,",			&sTmpDriverId[2], 8); 
        }
        else
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"DriverID[S]",		(BYTE *)"%s=%.6s,",			&sTmpDriverId[4], 6); 
        }
        //4.0.23.370 117473 end
    
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PIN[S]",			(BYTE *)"%s=%.16s,",		pCardData->sPINBlock,sizeof(pCardData->sPINBlock));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ExtendPIN[S]",		(BYTE *)"%s=%.20s,",		pCardData->sExtendPIN,sizeof(pCardData->sExtendPIN));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ReturnTank[I]",		(BYTE *)"%s=%ld,",			a2l(&(pCardData->sReturnTank),sizeof(pCardData->sReturnTank)));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"GeneralID[S]",		(BYTE *)"%s=%.6s,",			pCardData->sGeneralID,sizeof(pCardData->sGeneralID));

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PromptRewards[B]",	(BYTE *)"%s=%.1s,",			&(pCardData->sGRPrompt),sizeof(pCardData->sGRPrompt));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"GRVoucherNumber[S]",(BYTE *)"%s=%.10s,",		pCardData->sGRVoucherNumber,sizeof(pCardData->sGRVoucherNumber));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"GRKeyCode[S]",		(BYTE *)"%s=%.4s,",			pCardData->sGRKeyCode,sizeof(pCardData->sGRKeyCode));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"MerchantID[S]",		(BYTE *)"%s=%.10s,",		pCardData->sMerchantID,sizeof(pCardData->sMerchantID));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"HostTerminalID[S]",	(BYTE *)"%s=%.2s,",			pCardData->sHostTerminalID,sizeof(pCardData->sHostTerminalID));


        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"StoreNumber[S]",	(BYTE *)"%s=%.10s,",		pCardDataExtra2->sSiteId,sizeof(pCardDataExtra2->sSiteId));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"Unit[S]",			(BYTE *)"%s=%.10s,",		pCardDataExtra2->sUnit,sizeof(pCardDataExtra2->sUnit));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TripNumber[S]",		(BYTE *)"%s=%.10s,",		pCardDataExtra2->sTripNumber,sizeof(pCardDataExtra2->sTripNumber));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CustomerReference[S]",	(BYTE *)"%s=%.10s,",	pCardDataExtra2->sCustomerReference,sizeof(pCardDataExtra2->sCustomerReference));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TractorHub[S]",		(BYTE *)"%s=%.10s,",		pCardDataExtra2->sTractorHub,sizeof(pCardDataExtra2->sTractorHub));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TrailerNumber[S]",	(BYTE *)"%s=%.10s,",		pCardDataExtra2->sTrailerNumber,sizeof(pCardDataExtra2->sTrailerNumber));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TrailerHub[S]",		(BYTE *)"%s=%.10s,",		pCardDataExtra2->sTrailerHub,sizeof(pCardDataExtra2->sTrailerHub));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"OriginTrans[S]",	(BYTE *)"%s=%.1s,",			&(pCardDataExtra2->sOriginTrans),sizeof(pCardDataExtra2->sOriginTrans));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"DeviceNumber[S]",	(BYTE *)"%s=%.2s,",			pCardData->sDeviceNumber,sizeof(pCardData->sDeviceNumber));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PaymentStage[S]",	(BYTE *)"%s=%.1s,",			&(pCardDataExtra2->sPaymentStage),sizeof(pCardDataExtra2->sPaymentStage));

		//RFUEL-2817
		if ((_Module.m_server.GetOLAInUse() == OLASRV_EPSILON || _Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT) &&
			(SESSION_TYPE(byOlaStat) == SESSION_COMPLETE))
		{
			_Module.m_server.m_cLogger.LogMsg("COLABase::BuildRequest(OLASRV_EPSILON) - Skip sending the ClubCardNumber and ClubCardTrack2 to OLA");
		}
		else 
		{
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ClubCardNumber[S]", (BYTE *)"%s=%.20s,", pCardDataExtra->sLoyalId, sizeof(pCardDataExtra->sLoyalId));
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ClubCardTrack2[S]", (BYTE *)"%s=%.37s,", pCardData->sClubCardTrack2Data, sizeof(pCardData->sClubCardTrack2Data));
		}

        //4.0.20.51 63781	4.0.20.620 72559 for DEBUG only	//4.0.22.500
// 		CString str8;
// 		str8.Format("COLABase::BuildRequest, ClubCardNumber = %.20s, and ClubCardTrack2 = %.40s, were sent to OLA*****", pCardDataExtra->sLoyalId, pCardData->sClubCardTrack2Data);
// 		_LOGMSG.LogMsg(str8);
        
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ServiceFee[F]",		(BYTE *)"%s=%ld.%02ld,",	lServiceFee/100,lServiceFee%100);

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"FuelTrs[I]",		(BYTE *)"%s=%ld,",			lTransactionId); //4.0.14.502

        //42728	start //4.0.18.501
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"IFD[S]",(BYTE *)"%s=%.8s,",	pCardDataExtra4->sIFD,sizeof(pCardDataExtra4->sIFD));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ControllerRestriction[S]",(BYTE *)"%s=%.4s,", pCardDataExtra3->sControllerRestriction  ,sizeof(pCardDataExtra3->sControllerRestriction));
        //42728 end
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"DF[S]",(BYTE *)"%s=%.32s,",	pCardDataExtra5->sDedicatedFileName,sizeof(pCardDataExtra5->sDedicatedFileName)); //4.0.31.0 274401

        long lNoMatchingAID = 0;
        if (pCardDataExtra5->sNoMatchingAID == '1')
            lNoMatchingAID = 1;

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"NoMatchingAID[I]",(BYTE *)"%s=%ld,",	lNoMatchingAID); //4.0.31.0 AID
        
        if (pCardDataExtra5->sChipMalfunc !=' ' )
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ChipMalfunc[B]",	(BYTE *)"%s=%.1s,",	&(pCardDataExtra5->sChipMalfunc),sizeof(pCardDataExtra5->sChipMalfunc));

        if (pCardDataExtra5->sUserCancel !=' ' )
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"UserCancel[B]",	(BYTE *)"%s=%.1s,",		&(pCardDataExtra5->sUserCancel),sizeof(pCardDataExtra5->sUserCancel));
        
        //3.1.1.1
        if (pCardDataExtra2->sPinPadAction != ' ')
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PinPadResult[I]",	(BYTE *)"%s=%ld,",		a2l(&(pCardDataExtra2->sPinPadResult),sizeof(pCardDataExtra2->sPinPadResult)));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CalculatedMAC[S]",	(BYTE *)"%s=%.8s,",		(unsigned char *)pCardDataExtra2->sCalculatedMAC ,sizeof(pCardDataExtra2->sCalculatedMAC));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PinPadAction[I]",	(BYTE *)"%s=%ld,",		a2l(&(pCardDataExtra2->sPinPadAction),sizeof(pCardDataExtra2->sPinPadAction)));
            pCardDataExtra2->sPinPadAction = ' ';

        } //husky April
        else if( SESSION_TYPE(byOlaStat) == SESSION_COMPLETE ) //send MAC on session Complete
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CalculatedMAC[S]",	(BYTE *)"%s=%.8s,",		(unsigned char *)pCardDataExtra2->sCalculatedMAC ,sizeof(pCardDataExtra2->sCalculatedMAC));

        // Send Item list to OLA.
        //4.0.7.507
        if(IsAddPreAuthInfo(byOlaStat))
        {
            // 4.0.7.500
            long lProdCode = a2l(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode,sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode));

            AddedPreAuthInformation(lPumpNumber , lOffset, pOutBuffer , lOLAPumpServiceMode, lProdCode);	

			if (pCardDataExtra6->sQuickChipTrs == 'Y')
			{
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"QuickChipTrs[B]", (BYTE *)"%s=%s,", (BYTE *)"Y", 1);				
			}
        }
        else if((SESSION_TYPE(byOlaStat) == SESSION_AUTHORIZE) || (SESSION_TYPE(byOlaStat) == SESSION_SALE))   //4.0.1.32
        {
            short nProdCode = a2i(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode,sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode));
        
            if ((nProdCode > 0) && 
				(_Module.m_server.GetOLAReportSoldItemOnPump() == 0))
            {
                long lPriceCash = 0;   //4.0.2.38
                long lPriceCredit = 0;   //4.0.2.38

                lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCode0[S]",	(BYTE *)"%s=%03ld,", _Module.m_server.GetGradeCode(a2i(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode))));
                lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCount[I]",	(BYTE *)"%s=%03ld,", 1);
                lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemIsFuel0[B]", (BYTE *)"%s=%s,", (BYTE *)"Y", 1);

                //4.0.2.38
                GetGradePrice(nProdCode, lPriceCash, lPriceCredit, lOLAPumpServiceMode);				
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"GradePrice1[F]", (BYTE *)"%s=%ld.%03ld,", lPriceCash / 1000, lPriceCash % 1000);
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"GradePrice2[F]", (BYTE *)"%s=%ld.%03ld,", lPriceCredit / 1000, lPriceCredit % 1000);
                //4.0.2.38					
            }

			if (pCardDataExtra6->sQuickChipTrs == 'Y')
			{
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"QuickChipTrs[B]", (BYTE *)"%s=%s,", (BYTE *)"Y", 1);
			}

            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AVIFuelCode[I]",	(BYTE *)"%s=%.1s,",			&(pInfo->CardSaleExtraData2.sAVIFuelCode),sizeof(pInfo->CardSaleExtraData2.sAVIFuelCode));				//3.2.0.34 
        }
             
        // Convert layer will pass char 'Y' - to continue without ask another card , 'N' -  another track2 , ' ' - empty
        switch (pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt)
        {
            case DECLINE_LOYALTY_INVALID:
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"DeclinedLoyaltyInvalid[B]",	(BYTE *)"%s=%.1s,",			&(pInfo->CardSaleExtraData2.sDeclinedLoyaltyResponse),sizeof(pInfo->CardSaleExtraData2.sDeclinedLoyaltyResponse));																
                break;											
            case DECLINE_LOYALTY_EXPIRED:
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"DeclinedLoyaltyExpired[B]",	(BYTE *)"%s=%.1s,",			&(pInfo->CardSaleExtraData2.sDeclinedLoyaltyResponse),sizeof(pInfo->CardSaleExtraData2.sDeclinedLoyaltyResponse));
                break;
            case DECLINE_LOYALTY_WITH_PAYCARD:
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"DeclinedLoyaltyWithPayCard[B]",	(BYTE *)"%s=%.1s,",		&(pInfo->CardSaleExtraData2.sDeclinedLoyaltyResponse),sizeof(pInfo->CardSaleExtraData2.sDeclinedLoyaltyResponse));
                break;
            case DECLINE_PAYMENT_CARD:  //4.0.0.86
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"DeclinePaymentRetry[B]",	(BYTE *)"%s=%.1s,",		&(pInfo->CardSaleExtraData2.sDeclinedLoyaltyResponse),sizeof(pInfo->CardSaleExtraData2.sDeclinedLoyaltyResponse));
                break; //4.0.0.86
        }

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PassCode[S]",		(BYTE *)"%s=%.16s,",		pCardDataExtra2->sPassCode,sizeof(pCardDataExtra2->sPassCode));

		#ifdef DEBUG
                CString str;
                str.Format(">>> sDeclinedLoyaltyPrompt=%c  sDeclinedLoyaltyResponse=%c" ,pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt , pInfo->CardSaleExtraData2.sDeclinedLoyaltyResponse);
                _LOGMSG.LogMsg("COLABase","BuildRequest",0,LOG_NONE,str);
		#endif // DEBUG
            
        //only buffer
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"AccountType[S]",			(BYTE *)"%s=%.1s,",		&(pInfo->CardSaleExtraData2.sAccountType), sizeof(pInfo->CardSaleExtraData2.sAccountType));
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ExpirationDate[E]",		(BYTE *)"%s=%.6s,",		(BYTE *)sExpire, sizeof(sExpire));
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ChargeTo[I]",				(BYTE *)"%s=%.9s,",		pCardDataExtra2->sChargeTo, sizeof(pCardDataExtra2->sChargeTo));			//4.0.3.31				
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ZipCode[I]",				(BYTE *)"%s=%.5s,",		pCardDataExtra2->sZipCode, sizeof(pCardDataExtra2->sZipCode));			//4.0.1.31 //4.0.5.19
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"PumpTestPassword[I]",		(BYTE *)"%s=%.4s,",		pCardDataExtra4->sPumpTestPassword, sizeof(pCardDataExtra4->sPumpTestPassword));		//4.0.19.502 - merge from 4.0.19.33
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ExtendedZipCode[I]",		(BYTE *)"%s=%.9s,",		pCardDataExtra4->sExtendedZipCode, sizeof(pCardDataExtra4->sExtendedZipCode));//46512

		BOOL bSendBarCode = TRUE; // RFUEL-1734
#if _ENCOR_DEBUG
		if(_Module.m_server.GetOLAInUse() == OLASRV_POSWARE)
			bSendBarCode = FALSE;
#endif // _ENCOR_DEBUG

		if(bSendBarCode)
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"BarCode[S]",				(BYTE *)"%s=%.40s,",	pCardDataExtra2->sBarCode, sizeof(pCardDataExtra2->sBarCode));//4.0.1.38
        
        long lTmpPreAuthAmount = a2l(pInfo->CardSaleExtraData4.sPreAuthAmount,sizeof(pInfo->CardSaleExtraData4.sPreAuthAmount));//4.0.19.505
        if(lTmpPreAuthAmount)
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PreAuthAmount[F]",		(BYTE *)"%s=%ld.%03ld,",	lTmpPreAuthAmount/1000,lTmpPreAuthAmount%1000);

        //4.0.19.508 - merge from 4.0.18.290
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"IssActOnLinel[S]",	(BYTE *)"%s=%.10s,",		pCardDataExtra3->sIssuerActiondesOnLinel,sizeof(pCardDataExtra3->sIssuerActiondesOnLinel));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"IssActDefault[S]",	(BYTE *)"%s=%.10s,",		pCardDataExtra3->sIssuerActiondesDefault,sizeof(pCardDataExtra3->sIssuerActiondesDefault));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"IssActDenial[S]",	(BYTE *)"%s=%.10s,",		pCardDataExtra3->sIssuerActiondesDenial,sizeof(pCardDataExtra3->sIssuerActiondesDenial));
 
        //4.0.20.20 merge from 4.0.18.300
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TransactionStatusMode[I]",	(BYTE *)"%s=%.4s,", pInfo->CardSaleExtraData3.sTransactionStatusMode, sizeof(pInfo->CardSaleExtraData3.sTransactionStatusMode));

        //4.0.20.20 - in case there is a CvmList info waiting to be sent to OLA
        if(!PUMP_NOT_VALID(lPumpNumber))
        {
            if (!ChrAll((char *)_Module.m_server.m_cCvmListInfo[lPumpNumber-1].byLen, sizeof(_Module.m_server.m_cCvmListInfo[lPumpNumber-1].byLen)))
            {
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CVMList[S]",	(BYTE *)"%s=%.512s,", _Module.m_server.m_cCvmListInfo[lPumpNumber-1].byData, sizeof(_Module.m_server.m_cCvmListInfo[lPumpNumber-1].byData));	
            }
        }
        
        //4.0.27.30  TD 157086 - Arval - start
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "ChipLocalPromptsMandated[S]",(BYTE *)"%s=%.3s,",pInfo->CardSaleExtraData5.sChipLocalPromptsMandated,sizeof(pInfo->CardSaleExtraData5.sChipLocalPromptsMandated));
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "ChipLocalValidationMandated[S]",(BYTE *)"%s=%.3s,",pInfo->CardSaleExtraData5.sChipLocalValidationMandated,sizeof(pInfo->CardSaleExtraData5.sChipLocalValidationMandated));
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "ChipCarRegistration[S]",(BYTE *)"%s=%.14s,",pInfo->CardSaleExtraData5.sChipCarRegistration,sizeof(pInfo->CardSaleExtraData5.sChipCarRegistration));
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "ChipItemCode[S]",(BYTE *)"%s=%.76s,",pInfo->CardSaleExtraData5.sChipItemCode,sizeof(pInfo->CardSaleExtraData5.sChipItemCode));
        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "ChipFillVolume[S]",(BYTE *)"%s=%.6s,",pInfo->CardSaleExtraData5.sChipFillVolume,sizeof(pInfo->CardSaleExtraData5.sChipFillVolume));
        //4.0.27.30  TD 157086 - Arval - end
        
		lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "TenderCount[I]", (BYTE *) "%s=%s,", (BYTE *) "1", 1);

        if( (SESSION_TYPE(byOlaStat) == SESSION_COMPLETE) ||
            (SESSION_TYPE(byOlaStat) == SESSION_CANCEL)   ||
            (SESSION_TYPE(byOlaStat) == SESSION_SALE)	  ||
            (SESSION_TYPE(byOlaStat) == SESSION_CAPTURE)) //5.0.0.55
        {		
            //4.0.20.20 - delete the internal CvmList Info only in complete session.
            if(!PUMP_NOT_VALID(lPumpNumber))
                memset(&_Module.m_server.m_cCvmListInfo[lPumpNumber-1], ' ', sizeof(_Module.m_server.m_cCvmListInfo[lPumpNumber-1])); 

            long lGRActualDiscount = a2l(pInfo->CardSaleInfo.extraData.sGRActualDiscountPerGallon,sizeof(pInfo->CardSaleInfo.extraData.sGRActualDiscountPerGallon));
            long lGRDiscount = a2l(pCardData->sGRDiscountPerGallon, sizeof(pCardData->sGRDiscountPerGallon));
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString sLogMsg;
				sLogMsg.Format("%s DISCOUNT:  sGRDiscountPerGallon=%d", __FUNCTION__, lGRDiscount);
				_LOGMSG.LogMsg(sLogMsg);
			}
			long lGRMaxVolume = a2l(pCardData->sGRMaxVolume,sizeof(pCardData->sGRMaxVolume));
            long lPreAuthorizedAmount = ConvertStringsToLong(pCardData->sAuthAmt,sizeof(pCardData->sAuthAmt),pCardDataExtra->sAuthAmt_Msb   ,sizeof(pCardDataExtra->sAuthAmt_Msb)); //3.2.0.73
            long lPercentOffGallon = a2l(pCardDataExtra5->sPercentOffDiscount, sizeof(pCardDataExtra5->sPercentOffDiscount)); //4.0.23.500 75234

            long lGRDiscountType = a2l(&(pCardDataExtra2->sGRDiscountType),sizeof(pCardDataExtra2->sGRDiscountType));
            unsigned long lGradeTax;
            unsigned long lFuelLoyalApp;  //3.1.1.1	
            long lLoyaltyTotalAmount = 0; //3.1.1.1			

            if (nGrade)
            {
                CString strTableGrade("Grades\\Grade");  //4.0.25.501 TD 114999
                char sNumber[8] = {0};
                sprintf_s(sNumber,3,"%02d",nGrade);
                strTableGrade += sNumber;

                _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)strTableGrade), "TaxRate",&lGradeTax,(DWORD)0,FALSE);
                _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)strTableGrade), "LoyaltyApplicable",&lFuelLoyalApp,(DWORD)1,FALSE); //3.1.1.1
            }

            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PreAuthResponseCode[I]",(BYTE *)"%s=%.2s,",	pInfo->CardSaleExtraData5.sPreAuthResponseCode,sizeof(pInfo->CardSaleExtraData5.sPreAuthResponseCode)); //4.0.27.60
    
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"FuelAmount[F]",		(BYTE *)"%s=%ld.%03ld,",	lValue/1000,lValue%1000);
            
            if (!ChrAll((char *)pInfo->CardSaleExtraData2.sAuthorizeSessionID_MSB, sizeof(pInfo->CardSaleExtraData2.sAuthorizeSessionID_MSB)))
            {
                BYTE sTmpSessionId[7];
                sprintf_s((char*)sTmpSessionId , sizeof(sTmpSessionId),"%.2s%.4s",pInfo->CardSaleExtraData2.sAuthorizeSessionID_MSB, pCardData->sAuthorizeSessionID );
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"OldSessID[I]",		(BYTE *)"%s=%.6s,",			sTmpSessionId, 6);
            }
            else						
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"OldSessID[I]",		(BYTE *)"%s=%.4s,",			pCardData->sAuthorizeSessionID,sizeof(pCardData->sAuthorizeSessionID));

            //4.0.3.57	//4.0.1.38
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CompletedSessID[I]",(BYTE *)"%s=%.4s,",			pInfo->CardSaleExtraData2.sCompletedSessionID,sizeof(pInfo->CardSaleExtraData2.sCompletedSessionID));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"Grade[I]",			(BYTE *)"%s=%ld,",			(long)nGrade);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"UnitPrice[F]",		(BYTE *)"%s=%ld.%03ld,",	lGradePrice/1000 ,lGradePrice%1000);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TransVolume[F]",	(BYTE *)"%s=%ld.%03ld,",	lVolume/1000,lVolume%1000);

//			if (_Module.m_server.GetOLAInUse() == OLASRV_EPSILON == FALSE)
			lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ReferenceNumber[S]",(BYTE *)"%s=%.12s,",		pCardData->sReferenceNumber	,sizeof(pCardData->sReferenceNumber));

            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"Approval[S]",		(BYTE *)"%s=%.12s,",		pCardData->sApproval,sizeof(pCardData->sApproval) );
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AuthTerminal[S]",	(BYTE *)"%s=%.7s,",			pCardData->sAuthTerminal,sizeof(pCardData->sAuthTerminal) );
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AuthVersion[S]",	(BYTE *)"%s=%.7s,",			pCardData->sAuthVersion,sizeof(pCardData->sAuthVersion) );
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"IsOnLine[B]",		(BYTE *)"%s=%.1s,",			&(pCardData->sIsOnLine),sizeof(pCardData->sIsOnLine));

            if(FALSE == _Module.m_server.GetTwoTendersInTransaction())
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TransTaxPercent[F]",(BYTE *)"%s=%ld.%03ld,",	lGradeTax/1000,lGradeTax%1000); // 3.0.4.1

            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"FuelNetAmt[F]",		(BYTE *)"%s=%ld.%03ld,",	(lValue-lGRTotalDiscount)/1000,(lValue-lGRTotalDiscount)%1000); // 3.0.4.1

            //*** R10 User Story 80969 start ***/
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"Nozzle[I]",		    (BYTE *)"%s=%.1s,",		    &(pCardData->sNozzle),sizeof(pCardData->sNozzle));										//R10 User Story - 76018 
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TransFlowRate[I]",	(BYTE *)"%s=%.4s,",		    pInfo->CardSaleExtraData5.sTrsFlowRate,sizeof(pInfo->CardSaleExtraData5.sTrsFlowRate)); //R10 User Story - 80969 
        
            // 4.0.32.671 - TD 464219
            if (pTrs)
            {
                TAG_PUMP_TRANSACT5 pumpTrs5;
                memset(&pumpTrs5, 0x00, sizeof(pumpTrs5));
                pTrs->Convert2PumpTrsStruct(&pumpTrs5);
                BYTE sTmp1[16];
                BYTE sTmp2[16];
                memset(sTmp1, 0, sizeof(sTmp1));
                memset(sTmp2, 0, sizeof(sTmp2));
                sprintf_s((char*)sTmp1, sizeof(sTmp1), "%.2s%.2s%.2s%.2s%.2s%.4s", pumpTrs5.sStartFuelingDateTime.sHour, pumpTrs5.sStartFuelingDateTime.sMinute, pumpTrs5.sStartFuelingDateTime.sSecond, pumpTrs5.sStartFuelingDateTime.sDay, pumpTrs5.sStartFuelingDateTime.sMonth, pumpTrs5.sStartFuelingDateTime.sYear);
                sprintf_s((char*)sTmp2, sizeof(sTmp2), "%.2s%.2s%.2s%.2s%.2s%.4s", pumpTrs5.sDateTime.sHour, pumpTrs5.sDateTime.sMinute, pumpTrs5.sDateTime.sSecond, pumpTrs5.sDateTime.sDay, pumpTrs5.sDateTime.sMonth, pumpTrs5.sDateTime.sYear);

                lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"StartFuelingDateTime[D]", (BYTE *)"%s=%.16s,", sTmp1, 16);               // YYYYMMMDDHHMMSS
                lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"EndFuelingDateTime[D]", (BYTE *)"%s=%.16s,", sTmp2, 16);               // YYYYMMMDDHHMMSS
            }

            //*** R10 User Story 80969 end ***/			
            if (pInfo->CardSaleExtraData2.sPriceLevel != ' ')	//3.2.0.34
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PriceLevel[I]",		(BYTE *)"%s=%.1s,",			&(pInfo->CardSaleExtraData2.sPriceLevel),sizeof(pInfo->CardSaleExtraData2.sPriceLevel));
            
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AmountLimit[F]",	(BYTE *)"%s=%ld.%02ld,",	lPreAuthorizedAmount/100,lPreAuthorizedAmount%100);//4.0.1.38

			long lItemCount = 1L;

			if (!PUMP_NOT_VALID(lPumpNumber))	//4.0.19.500 58863 
			{
				//RFUEL-2817
				if ((nGrade) && !((_Module.m_server.GetOLAInUse() == OLASRV_EPSILON || _Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT) && (lVolume == 0) && (lValue == 0)))
				{
					long lTmpGrade = nGrade;
					AddedCompletePriceInformation(lPumpNumber, lOffset, pOutBuffer, lOLAPumpServiceMode, lTmpGrade, lGradePrice);
				}
				else
				{
					lItemCount = 0;
				}
			}

			//Check for more item, other then fuel.
			if (a2l(&pInfo->CardSaleInfo.extraData.sNumberOfExtraSale, sizeof(pInfo->CardSaleInfo.extraData.sNumberOfExtraSale)))
			{
				//We got car wash
				lItemCount++;
				//Check if also got discount
				if (a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sDiscount, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDiscount)))
				{
					lItemCount++;
				}
			}

			//4.0.10.506
			if (a2l(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode, sizeof(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode)))  //4.0.10.491
			{
				lItemCount++;
			}

			//Number of item
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCount[I]", (BYTE *)"%s=%ld,", lItemCount);

			if (!PUMP_NOT_VALID(lPumpNumber))	//4.0.19.500 58863
			{
				if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetDieselPumpFlags())
				{
					if ((OLA_STATE(byOlaStat) == OLA_REQUEST_TO_SEND) &&   //4.0.10.811
						(SESSION_TYPE(byOlaStat) == SESSION_COMPLETE))
					{
						//4.0.10.49
						//Reefer sales/
						long lReeferValue = a2l(pInfo->CardSaleExtraData3.sReeferTotal, sizeof(pInfo->CardSaleExtraData3.sReeferTotal));
						long lReeferVolume = a2l(pInfo->CardSaleExtraData3.sReeferVolume, sizeof(pInfo->CardSaleExtraData3.sReeferVolume));
						long lReeferPrice = a2l(pInfo->CardSaleInfo.extraData.SALES[3].sUnitPrice, sizeof(pInfo->CardSaleInfo.extraData.SALES[3].sUnitPrice));

						if (LOG_DETAIL_FLOW_CONTROL)
						{
							CString str;
							str.Format("Report reefer details value = %ld, volume = %ld, price = %ld, Grade = %ld", lReeferValue, lReeferVolume, lReeferPrice, a2l(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode, sizeof(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode)));
							_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", 0, LOG_NONE, str);
						}

						DWORD dwCode;
						DWORD dwCarWashLoyalApp; //3.1.1.1
						_Module.m_server.m_cParam.LoadParam((char *)"CarWash", "LoyaltyApplicable", &dwCarWashLoyalApp, 1L, FALSE); //3.1.1.1
						_Module.m_server.m_cParam.LoadParam((char *)"CarWash", "CarWashCode", &dwCode, 102L, FALSE);//102 - NACS code for car wash
						long lAmount = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice)) * 10;
						long lQty = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sQty, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sQty));
						long lDiscount = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sDiscount, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDiscount)) * 10;
						long lTax = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[2].sTax, sizeof(pInfo->CardSaleInfo.extraData.SALES[2].sTax)) * 10;        //3.2.0.9	

						if (a2l(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode, sizeof(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode)))
						{
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCode1[S]", (BYTE *)"%s=%03ld,", _Module.m_server.GetGradeCode((short)a2l(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode, sizeof(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode))));
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeAmt1[F]", (BYTE *)"%s=%ld.%03ld,", lReeferValue / 1000, lReeferValue % 1000);   //4.0.10.49
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeQty1[I]", (BYTE *)"%s=%ld,", 1L);//This value will be a transaction parameter as we sell items at the Pump.
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemWeight1[F]", (BYTE *)"%s=%ld.%03ld,", lReeferVolume / 1000, lReeferVolume % 1000);
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemIsFuel1[B]", (BYTE *)"%s=%s,", (BYTE *)"Y", 1);
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemPrice1[F]", (BYTE *)"%s=%ld.%03ld,", lReeferPrice / 1000, lReeferPrice % 1000);
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemType1[S]", (BYTE *)"%s=%s,", (BYTE *)"C", 1);
						}

						long lFuelSale = lValue - lReeferValue;
						lVolume -= lReeferVolume;

						if (lItemCount == 2)   //4.0.10.49
						{
							lGradePrice = a2l(pInfo->CardSaleExtraData2.sGradePrice, sizeof(pInfo->CardSaleExtraData2.sGradePrice));
							nGrade = a2i(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode));
						}

						if (LOG_DETAIL_FLOW_CONTROL)
						{
							CString str;
							str.Format("Report diesel details value = %ld, volume = %ld, price = %ld, Grade = %d", lFuelSale, lVolume, lGradePrice, nGrade);
							_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", 0, LOG_NONE, str);
						}

						//Fuel is alway there, and it always the first.
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCode0[S]", (BYTE *)"%s=%03ld,", _Module.m_server.GetGradeCode(nGrade));
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeAmt0[F]", (BYTE *)"%s=%ld.%03ld,", lFuelSale / 1000, lFuelSale % 1000);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeQty0[I]", (BYTE *)"%s=%ld,", 1L);//This value will be a transaction parameter as we sell items at the Pump.
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemWeight0[F]", (BYTE *)"%s=%ld.%03ld,", lVolume / 1000, lVolume % 1000);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemIsFuel0[B]", (BYTE *)"%s=%s,", (BYTE *)"Y", 1);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemPrice0[F]", (BYTE *)"%s=%ld.%03ld,", lGradePrice / 1000, lGradePrice % 1000); //4.0.10.49
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemType0[S]", (BYTE *)"%s=%s,", (BYTE *)"C", 1);
					}	//4.0.9.507 
				}
				else if (lItemCount > 1) 	//Car wash			
				{
					DWORD dwCode;
					DWORD dwCarWashLoyalApp; //3.1.1.1
					_Module.m_server.m_cParam.LoadParam((char *)"CarWash", "LoyaltyApplicable", &dwCarWashLoyalApp, 1L, FALSE); //3.1.1.1
					_Module.m_server.m_cParam.LoadParam((char *)"CarWash", "CarWashCode", &dwCode, 102L, FALSE);//102 - NACS code for car wash
					long lAmount = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice)) * 10;
					long lQty = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sQty, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sQty));
					long lDiscount = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sDiscount, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDiscount)) * 10;
					long lTax = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[2].sTax, sizeof(pInfo->CardSaleInfo.extraData.SALES[2].sTax)) * 10;        //3.2.0.9	
					long lFuelSale = lValue - lAmount + lDiscount;

					if (_Module.m_server.GetReduceCWDiscountFromCWAmount() && (lItemCount == 3))  //4.0.23.2583
					{
						CString str;
						str.Format("ReduceCWDiscountFromCWAmount is TRUE  lAmount = %ld, lDiscount = %ld , lItemCount = %ld", lAmount, lDiscount, lItemCount);
						_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", 0, LOG_NONE, str);
						lAmount = lAmount - lDiscount;
						--lItemCount;
					}

					//Fuel is alway there, and it always the first.
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCode0[S]", (BYTE *)"%s=%03ld,", _Module.m_server.GetGradeCode(nGrade));
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeAmt0[F]", (BYTE *)"%s=%ld.%03ld,", lFuelSale / 1000, lFuelSale % 1000);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeQty0[I]", (BYTE *)"%s=%ld,", 1L);//This value will be a transaction parameter as we sell items at the Pump.
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemWeight0[F]", (BYTE *)"%s=%ld.%03ld,", lVolume / 1000, lVolume % 1000);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemIsFuel0[B]", (BYTE *)"%s=%s,", (BYTE *)"Y", 1);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemTax0[F]", (BYTE *)"%s=%ld.%03ld,", lTax / 1000, lTax % 1000);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemType0[S]", (BYTE *)"%s=%s,", (BYTE *)"P", 1); //4.0.9999.0

																														 //3.1.1.1
					if (lFuelLoyalApp)
					{
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemLoyalAmt0[F]", (BYTE *)"%s=%ld.%03ld,", lFuelSale / 1000, lFuelSale % 1000);
						lLoyaltyTotalAmount += lFuelSale;
					}

					//Car Wash sale
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCode1[S]", (BYTE *)"%s=%03ld,", dwCode);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeAmt1[F]", (BYTE *)"%s=%ld.%03ld,", lAmount / 1000, lAmount % 1000);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeQty1[I]", (BYTE *)"%s=%ld,", lQty);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemWeight1[F]", (BYTE *)"%s=%ld.%03ld,", 1, 0);//always 1.000
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemIsFuel1[B]", (BYTE *)"%s=%s,", (BYTE *)"N", 1);

					lTax = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sTax, sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sTax)) * 10;        	//3.2.0.69				 
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemTax1[F]", (BYTE *)"%s=%ld.%03ld,", lTax / 1000, lTax % 1000);

					if (dwCarWashLoyalApp)					//4.0.0.737
					{
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemLoyalAmt1[F]", (BYTE *)"%s=%ld.%03ld,", lAmount / 1000, lAmount % 1000);
						lLoyaltyTotalAmount += lAmount;
					}

					//Car Wash Discount
					if (lItemCount == 3)
					{
						_Module.m_server.m_cParam.LoadParam((char *)"CarWash", "DiscountCode", &dwCode, 900L, FALSE);//900 - NACS code for discount1

						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCode2[S]", (BYTE *)"%s=%03ld,", dwCode);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeAmt2[F]", (BYTE *)"%s=%ld.%03ld,", lDiscount / 1000, lDiscount % 1000);
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeQty2[I]", (BYTE *)"%s=%ld,", 1);//always 1
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemWeight2[F]", (BYTE *)"%s=%ld.%03ld,", 1, 0);//always 1.000
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemIsFuel2[B]", (BYTE *)"%s=%s,", (BYTE *)"N", 1);
						lTax = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[1].sTax, sizeof(pInfo->CardSaleInfo.extraData.SALES[1].sTax)) * 10;
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemTax2[F]", (BYTE *)"%s=%ld.%03ld,", lTax / 1000, lTax % 1000);

						if (dwCarWashLoyalApp)						//3.1.1.1
						{
							lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemLoyalAmt2[F]", (BYTE *)"%s=%ld.%03ld,", lDiscount / 1000, lDiscount % 1000);
							lLoyaltyTotalAmount -= lDiscount;
						}

					}
				}	// Defect 480448 //RFUEL-2817
				else if ((_Module.m_server.GetOLAInUse() == OLASRV_EPSILON|| _Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT) &&
					(lVolume == 0) && (lValue == 0))
				{
					lItemCount = 0;

					if (LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("No fuel was taken. lItemCount = %ld", lItemCount);
						_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", lPumpNumber, LOG_PUMP, str); //4.0.23.31 75234
					}
				}
				else
				{
					long lTax = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[2].sTax, sizeof(pInfo->CardSaleInfo.extraData.SALES[2].sTax)) * 10;        //3.2.0.9					
					long lVatRate = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.sVAT_Rate, sizeof(pInfo->CardSaleInfo.extraData.sVAT_Rate));		//4.0.15.504				
																																				//Fuel is alway there, and it always the first.
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCode0[S]", (BYTE *)"%s=%03ld,", _Module.m_server.GetGradeCode(nGrade));
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeAmt0[F]", (BYTE *)"%s=%ld.%03ld,", lValue / 1000, lValue % 1000);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeQty0[I]", (BYTE *)"%s=%ld,", 1L);//This value will be a transaction parameter as we sell items at the Pump.
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemWeight0[F]", (BYTE *)"%s=%ld.%03ld,", lVolume / 1000, lVolume % 1000);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemIsFuel0[B]", (BYTE *)"%s=%s,", (BYTE *)"Y", 1);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemTax0[F]", (BYTE *)"%s=%ld.%03ld,", lTax / 1000, lTax % 1000);
					lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemTaxRate0[F]", (BYTE *)"%s=%ld.%02ld,", lVatRate / 100, lVatRate % 100); //4.0.15.504

					if (lFuelLoyalApp)					//3.1.1.1
					{
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemLoyalAmt0[F]", (BYTE *)"%s=%ld.%03ld,", lValue / 1000, lValue % 1000);
						lLoyaltyTotalAmount += lValue;
					}
				}
				//End of the dedicate code for Fuel+Car wash+Discount.
			}

			//Number of item
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCount[I]", (BYTE *)"%s=%ld,", lItemCount);   //4.0.23.2582

            if (lLoyaltyTotalAmount)			//3.1.1.1
            {
                //4.0.2.162
                l2a(lLoyaltyTotalAmount,pCardDataExtra2->sLoyaltyTotalAmount , sizeof(pCardDataExtra2->sLoyaltyTotalAmount));
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"LoyaltyTransAmount[F]",	(BYTE *)"%s=%ld.%03ld,",lLoyaltyTotalAmount/1000,lLoyaltyTotalAmount%1000);
            }
        
            if(lGRDiscount || lPercentOffGallon) //4.0.23.500 75234
            {
				_Module.m_server.AdjustPriceProtectionDiscount(lGRDiscount, pTrs);
				lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"GRDiscountPerGallon[F]",		(BYTE *)"%s=%ld.%03ld,",	lGRDiscount/1000,lGRDiscount%1000);
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"GRActualDiscountPerGallon[F]",	(BYTE *)"%s=%ld.%03ld,",	lGRActualDiscount/1000,lGRActualDiscount%1000);
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"GRMaxVolume[F]",				(BYTE *)"%s=%ld.%03ld,",	lGRMaxVolume/1000,lGRMaxVolume%1000);
                //4.0.23.20 75234 start
                long lTotalTicketDiscount = a2l(pCardDataExtra5->sFuelTotalTicketDiscount, sizeof(pCardDataExtra5->sFuelTotalTicketDiscount));	
                if (lTotalTicketDiscount > 0)
                {
                    lGRTotalDiscount = 0;
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TotalTicketDiscount[F]",			(BYTE *)"%s=%ld.%03ld,",	lTotalTicketDiscount/1000,lTotalTicketDiscount%1000);

                    if(LOG_DETAIL_FLOW_CONTROL)
                    {
                        CString str;
                        str.Format("Total ticket discount found = %ld" , lTotalTicketDiscount);
                        _LOGMSG.LogMsg("COLABase","BuildRequest",lPumpNumber,LOG_PUMP,str); //4.0.23.500 75234
                    }
                }
                //4.0.23.20 75234 end
                lOffset += SetParam(pOutBuffer+lOffset, (BYTE *)"GRTotalDiscount[F]",				(BYTE *)"%s=%ld.%03ld,",	lGRTotalDiscount / 1000, lGRTotalDiscount % 1000);
                lOffset += SetParam(pOutBuffer+lOffset, (BYTE *)"GRDiscountType[I]",					(BYTE *)"%s=%ld,",			lGRDiscountType);
                lOffset += SetParam(pOutBuffer+lOffset, (BYTE *)"GasLoyaltyDiscountPerGallon[F]",	(BYTE *)"%s=%ld.%03ld,", lGRDiscount / 1000, lGRDiscount % 1000); //4.0.14.1540 69922 //4.0.22.0
            }			
            
            //yymmdd -> mmddyyyy	
            if ( !(pInfo->CardSaleInfo.extraData.sPrnDateTime[3] == ' ') && 
                 !(pInfo->CardSaleInfo.extraData.sPrnDateTime[3] == 0))
            {				
                //4.0.2.33
                char sYear[5] = {0};
                char sSeconds[3] = {0}; //4.0.23.504 87260
                BYTE sTmpDate[8];
                char sTmpTime[6];

                memset(&sTmpDate , 0 , sizeof(sTmpDate));					
                memset(&sTmpTime , '0' , sizeof(sTmpTime));
                memset(sYear , 0 , sizeof(sYear));

                //If the year is bigger then 70 then it 19XX else it 20XX
                GetLocalTime(&systime);  //4.0.2.32
                if (systime.wYear >= 2000)
                {
                    sprintf_s(sYear,5,"20%.2s",pInfo->CardSaleInfo.extraData.sPrnDateTime);
                }
                else
                {
                    sprintf_s(sYear,5,"19%.2s",pInfo->CardSaleInfo.extraData.sPrnDateTime);
                }
                memcpy(&sTmpDate[4] ,sYear, 4); //4.0.2.32
                memcpy(sTmpDate,&pInfo->CardSaleInfo.extraData.sPrnDateTime[2],4);			

                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TransactionDate[D]", (BYTE *)"%s=%.8s,",(BYTE *)sTmpDate ,sizeof(sTmpDate));
                //hhmmss
                sprintf_s(sSeconds,sizeof(sSeconds),"%02d", systime.wSecond); //4.0.23.504 87260
                memcpy(sTmpTime , &pInfo->CardSaleInfo.extraData.sPrnDateTime[6], 4 );

                memcpy(&sTmpTime[4], sSeconds, 2); //4.0.23.504 87260

                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TransactionTime[T]", (BYTE *)"%s=%.6s,",(BYTE *)sTmpTime ,sizeof(sTmpTime));
                
            }		

            if ( (pInfo->CardSaleInfo.extraData.sCompleteDate[0] == ' ') && 
                 !(pInfo->CardSaleInfo.extraData.sCompleteDate[0] == 0))		//4.0.3.24			
            {
                SYSTEMTIME systime;				
                memset(&sTmpDate, '0', sizeof(sTmpDate));					
                memset(&sTmpTime, '0', sizeof(sTmpTime));
                GetLocalTime(&systime);
                char sTmp[11];

                memset(sTmp, 0, sizeof(sTmp));

                sprintf_s(sTmp, 11, "%02d%02d%02d", systime.wHour, systime.wMinute, systime.wSecond);
                memcpy(pInfo->CardSaleInfo.extraData.sCompleteTime ,sTmp ,sizeof(pInfo->CardSaleInfo.extraData.sCompleteTime));

                memset(sTmp, 0, sizeof(sTmp));
                sprintf_s(sTmp, 11, "%02d%02d%04d", systime.wMonth, systime.wDay, systime.wYear);			

                lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "CompleteDate[D]", (BYTE *) "%s=%.8s,", (BYTE *)sTmp, 8); // MMDDYY
                memcpy(pInfo->CardSaleInfo.extraData.sCompleteDate, sTmp, sizeof(pInfo->CardSaleInfo.extraData.sCompleteDate));
            }

            //3.2.1.312
            lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "CompleteTime[T]", (BYTE *)"%s=%.6s,", 
								(BYTE *) pInfo->CardSaleInfo.extraData.sCompleteTime, 6); // HHMMSS		
            lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "CompleteDate[D]", (BYTE *) "%s=%.8s,", 
								(BYTE *)pInfo->CardSaleInfo.extraData.sCompleteDate ,8); // MMDDYY

            //4.0.3.47				
            lTmp = a2l(pInfo->CardSaleExtraData2.sTotalBalance,sizeof(pInfo->CardSaleExtraData2.sTotalBalance));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"RemainingBalance[F]",	(BYTE *)"%s=%ld.%02ld,",lTmp/100,lTmp%100);			

            if (pInfo->CardSaleExtraData4.byIsRemoveProhibitedData == 'Y')							//4.0.24.60 TD 112773
                lOffset += SetParam(pOutBuffer+lOffset, (BYTE *)"SystemID[I]",		(BYTE *)"%s=%ld,  ", a2l(pInfo->CardSaleExtraData3.sAuthorizerSystemID, sizeof(pInfo->CardSaleExtraData3.sAuthorizerSystemID)) ); 
            
            // 4.0.5.26
            if (!ChrAll((char *)pInfo->CardSaleInfo.cardData.sCardName, sizeof(pInfo->CardSaleInfo.cardData.sCardName)))
            {
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ShortCardName[S]",	(BYTE *)"%s=%.8s,",			pInfo->CardSaleInfo.cardData.sCardName,sizeof(pInfo->CardSaleInfo.cardData.sCardName) );

                if(LOG_DETAIL_FLOW_CONTROL)
                {
                    CString str;						
                    str.Format("ShortCardName=%.8s" , pInfo->CardSaleInfo.cardData.sCardName);
                    _LOGMSG.LogMsg("COLABase","BuildRequest",0,LOG_NONE,str);
                }
            }
            
            //4.0.22.509 74387
            if (!ChrAll((char *)pInfo->CardSaleExtraData4.sLoyaltyName, sizeof(pInfo->CardSaleExtraData4.sLoyaltyName)))
            {
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"LoyaltyName[S]",	(BYTE *)"%s=%.20s,", pInfo->CardSaleExtraData4.sLoyaltyName, sizeof(pInfo->CardSaleExtraData4.sLoyaltyName) );

                if(LOG_DETAIL_FLOW_CONTROL)
                {
                    CString str;						
                    str.Format("LoyaltyName=%.20s" , pInfo->CardSaleExtraData4.sLoyaltyName);
                    _LOGMSG.LogMsg("COLABase","BuildRequest",0,LOG_NONE,str);
                }
            }
            
            //42728 start
            if (!ChrAll((char *)pInfo->CardSaleExtraData4.sFormattedClubCardNumber, sizeof(pInfo->CardSaleExtraData4.sFormattedClubCardNumber)))
            {
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"FormattedClubCardNumber[S]",	(BYTE *)"%s=%.20s,", pInfo->CardSaleExtraData4.sFormattedClubCardNumber, sizeof(pInfo->CardSaleExtraData4.sFormattedClubCardNumber));

            }
            if (!ChrAll((char *)pInfo->CardSaleInfo.cardData.sFullCardName, sizeof(pInfo->CardSaleInfo.cardData.sFullCardName)))
            {
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"FullCardName[S]",	(BYTE *)"%s=%.20s,", pInfo->CardSaleInfo.cardData.sFullCardName, sizeof(pInfo->CardSaleInfo.cardData.sFullCardName) );

                if(LOG_DETAIL_FLOW_CONTROL)
                {
                    CString str;						
                    str.Format("FullCardName=%.20s" , pInfo->CardSaleInfo.cardData.sFullCardName);
                    _LOGMSG.LogMsg("COLABase","BuildRequest",0,LOG_NONE,str);
                }
            }
            
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"MediaNumber[I]",(BYTE *)"%s=%.5s,",	pInfo->CardSaleExtraData4.sMediaNumber , sizeof(pInfo->CardSaleExtraData4.sMediaNumber));
            //42728 end

            
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"RestrictPoints[B]",(BYTE *)"%s=%.1s,",		&(pCardDataExtra5->sRestrictPoints),sizeof(pCardDataExtra5->sRestrictPoints));
            
                

            //4.0.24.90 118124
            if ( (OLA_STATE(byOlaStat) == OLA_REQUEST_TO_SEND) &&   
                (SESSION_TYPE(byOlaStat) == SESSION_COMPLETE))
            {
                if (_Module.m_server.GetOLAFlags(pInfo->CardSaleExtraData3.sFlags1, FL_OLA_EXTRA_DATA, sizeof(pInfo->CardSaleExtraData3.sFlags1))) //4.0.24.90 118124
                {
                    ASH2000_REC			sAsh2000Rec;
                    long lQdxRetVal =0;
                    memset(&sAsh2000Rec, ' ', sizeof(sAsh2000Rec));
                    sAsh2000Rec.lIndexNumber = lTransactionId;						
                                        
                    if (lQdxRetVal = _Module.m_server.IndexOperation(ASH2000_FILE , REC_READ,&sAsh2000Rec ))//4.0.3.49
                    {
                        if(LOG_BASIC_FLOW_CONTROL)
                        {
                            DWORD dwThreadId = GetCurrentThreadId();
                            CString tmpStr;
                            tmpStr.Format("BuildRequest, failed=%ld to read record %ld from file Ash 2000 info" ,   lQdxRetVal  , lTransactionId);
                            _LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, tmpStr);
                        }
                    }
                    else 
                    {
                        
                        if(LOG_BASIC_FLOW_CONTROL)
                        {
                            CString tmpStr;
                            tmpStr.Format("BuildRequest, Found ash 2000 record trs=%ld"  , lTransactionId );
                            _LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,tmpStr);
                        }
                        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"OLAExtraData[S]",	(BYTE *)"%s=%.1019s,", sAsh2000Rec.sData, sizeof(sAsh2000Rec.sData) );
                    }
                }
            }

            if(LOG_DETAIL_FLOW_CONTROL)
            {
                CString str;						
                str.Format("CompleteDate[D]=%.8s,CompleteTime[T]=%.6s", pInfo->CardSaleInfo.extraData.sCompleteDate,pInfo->CardSaleInfo.extraData.sCompleteTime);
                _LOGMSG.LogMsg("COLABase","BuildRequest",lPumpNumber,LOG_PUMP,str);
            }

            //4.0.28.500
            if (!ChrAll((char *)pInfo->CardSaleExtraData5.sEncryptData, sizeof(pInfo->CardSaleExtraData5.sEncryptData)))
            {
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"EncryptData[S]",	(BYTE *)"%s=%.100s,", pInfo->CardSaleExtraData5.sEncryptData, sizeof(pInfo->CardSaleExtraData5.sEncryptData));   
            }

			BuildEpsilonFields(lPumpIndex, lTotalAmount, lOffset, pOutBuffer, pInfo);

        }
        else if( SESSION_TYPE(byOlaStat) == SESSION_INQUIRY)
        {
            if (!ChrAll((char *)pInfo->CardSaleExtraData2.sAuthorizeSessionID_MSB, sizeof(pInfo->CardSaleExtraData2.sAuthorizeSessionID_MSB)))
            {
                BYTE sTmpSessionId[7];
                sprintf_s((char*)sTmpSessionId , 7,"%.2s%.4s",pInfo->CardSaleExtraData2.sAuthorizeSessionID_MSB, pCardData->sAuthorizeSessionID );
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"OldSessID[I]",		(BYTE *)"%s=%.6s,",			sTmpSessionId, 6);
            }
            else						
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"OldSessID[I]",		(BYTE *)"%s=%.4s,",			pCardData->sAuthorizeSessionID,sizeof(pCardData->sAuthorizeSessionID));
        }

        //4.0.5.24.  //4.0.5.32
        lMediaMasterType = a2l(pInfo->CardSaleExtraData2.sMediaMasterType , sizeof(pInfo->CardSaleExtraData2.sMediaMasterType));
        if (lMediaMasterType)
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"MediaMasterType[I]",(BYTE *)"%s=%ld,",lMediaMasterType);

#if _ENCOR_DEBUG // RFUEL-1734
		else if (SESSION_TYPE(byOlaStat) == SESSION_COMPLETE && _Module.m_server.GetOLAInUse() == OLASRV_POSWARE)
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"MediaMasterType[I]", (BYTE *)"%s=%ld,", CARD_TYPE_CREDIT);
#endif
        
        lCardSegment = a2l(&pInfo->CardSaleExtraData3.sCardSegment , sizeof(pInfo->CardSaleExtraData3.sCardSegment));			//Liat LPE

        if (lCardSegment)
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CardSegment[I]",(BYTE *)"%s=%ld,",lCardSegment);						

        //3.2.0.74
        if (pInfo->CardSaleInfo.cardData.sDateTime[1] != ' ')  	//Ella Husky EMV ?
        {			
            memset(sTmpDate , '0' , sizeof(sTmpDate));					
            memset(sTmpTime , '0' , sizeof(sTmpTime));
            GetLocalTime(&systime);

            l2a((long)systime.wYear, &sTmpDate[4] , 4);	
            
            if (_Module.m_server.GetIFSFTerminalSrvInUse())	//4.0.26.501 TD 154354
            {
                //Take freash Data from local time:
                l2a0((long)systime.wMonth, sTmpDate , 2);	
                l2a0((long)systime.wDay, &sTmpDate[2] , 2);	
                l2a0((long)systime.wHour, sTmpTime , 2);		
                l2a0((long)systime.wMinute, &sTmpTime[2] , 2);	

                //Copy to card sale data:
                memcpy(&pInfo->CardSaleInfo.cardData.sDateTime[6] , sTmpTime , 4);  //time
                memcpy(&pInfo->CardSaleInfo.cardData.sDateTime[2] , sTmpDate , 4);  //date
                memcpy(pInfo->CardSaleInfo.cardData.sDateTime , &sTmpDate[6] , 2);  //date
            }
            else  //4.0.26.501 TD 154354
            {	//Take Data As is
                memcpy(&sTmpDate[6], &pInfo->CardSaleInfo.cardData.sDateTime, 2);			//if pInfo->CardSaleInfo.cardData.sDateTime == {"08........"} 
                memcpy(sTmpDate,&pInfo->CardSaleInfo.cardData.sDateTime[2],4);				//then sTmpDate = {"00002008"}		//4.0.17.1060 TD 72868 
                memcpy(sTmpTime , &pInfo->CardSaleInfo.cardData.sDateTime[6] , 4);			//then sTmpDate = {"00002008"}		//4.0.17.1060 TD 72868 
                memcpy(&sTmpTime[4] , pInfo->CardSaleInfo.extraData.sDateTimeExt , 2);  						
            }

            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AuthorizationDate[D]",					(BYTE *)"%s=%.8s,",(BYTE *)sTmpDate ,sizeof(sTmpDate)); // MMDDYY
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AuthorizationTime[T]",					(BYTE *)"%s=%.4s,",(BYTE *)sTmpTime , 4); // MMDDYY

            if(LOG_DETAIL_FLOW_CONTROL)
            {
                CString str;	//4.0.3.36 - change the debug from "Complete" to "Authorization"
                str.Format(">>> Authorization Date=%.8s Time=%.6s" , sTmpDate, sTmpTime ); 
                if (lPumpNumber)
                    _LOGMSG.LogMsg("COLABase","BuildRequest",0,LOG_NONE,str);
                else
                    _LOGMSG.LogMsg("COLABase","BuildRequest",lPumpNumber,LOG_PUMP,str);
            }

            
        }
    //3.2.0.74		// 3.2.0.9
        memset(&sTmpTime , '0' , sizeof(sTmpTime));
        GetLocalTime(&systime);			

        l2a0((long)systime.wHour , sTmpDate, 2);
        l2a0((long)systime.wMinute , &sTmpDate[2], 2);
        l2a0((long)systime.wSecond, &sTmpDate[4], 2);
        
        //4.0.2.21
        long lPriceRecalcDiscount = ConvertStringsToLong(pInfo->CardSaleExtraData2.sPriceRecalcDiscount,
                                           sizeof(pInfo->CardSaleExtraData2.sPriceRecalcDiscount),
                                           pInfo->CardSaleExtraData2.sPriceRecalcDiscount_Msb,
                                           sizeof(pInfo->CardSaleExtraData2.sPriceRecalcDiscount_Msb));
        
        
        if (pInfo->CardSaleExtraData2.sPriceRecalcDiscountSign == '1')
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PriceRecalcDiscount[F]",		(BYTE *)"%s=-%ld.%03ld,", lPriceRecalcDiscount/1000,lPriceRecalcDiscount%1000);		
        else
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PriceRecalcDiscount[F]",		(BYTE *)"%s=%ld.%03ld,", lPriceRecalcDiscount/1000,lPriceRecalcDiscount%1000);		
        
        //4.0.3.21
        lTmp = a2l(pInfo->CardSaleExtraData2.sVolumeLimit, sizeof(pInfo->CardSaleExtraData2.sVolumeLimit));

        if (lTmp)
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"VolumeLimit[F]",		(BYTE *)"%s=%ld.%03ld,", lTmp/1000,lTmp%1000);		
        //4.0.3.21

        //4.0.18.504
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"LoyaltyEntryMethod[S]",	(BYTE *)"%s=%.1s,",		&(pCardDataExtra4->sLoyaltyEntryMethod) ,sizeof(pCardDataExtra4->sLoyaltyEntryMethod));

        //4.0.19.502
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TotalEftNetworkTime[I]",	(BYTE *)"%s=%.2s,",		pCardDataExtra4->sTotalEftNetworkTime ,sizeof(pCardDataExtra4->sTotalEftNetworkTime));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CustomerEftWaitTime[I]",	(BYTE *)"%s=%.2s,",		pCardDataExtra4->sCustomerEftWaitTime ,sizeof(pCardDataExtra4->sCustomerEftWaitTime));

        //4.0.19.0
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ClubcardScheme[I]",	(BYTE *)"%s=%.4s,",		pCardDataExtra->sLoyalScheme ,sizeof(pCardDataExtra->sLoyalScheme));
        
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"SignatureVarified[S]=N,");
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ManualAuthObtained[S]=N,");

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AVIEngineHours[I]",		(BYTE *)"%s=%.6s,",		pCardData->sAVIEngineHours,sizeof(pCardData->sAVIEngineHours));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AVISystemCustomer[I]",	(BYTE *)"%s=%.4s,",		pCardData->sAVISystemCustomer,sizeof(pCardData->sAVISystemCustomer));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AVICompanyId[I]",		(BYTE *)"%s=%.6s,",		pCardData->sAVICompanyId,sizeof(pCardData->sAVICompanyId));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AVIUserId[I]",			(BYTE *)"%s=%.6s,",		pCardData->sAVIUserId,sizeof(pCardData->sAVIUserId));

        //25853 4.0.18.503 
        if (!ChrAll((char *)pCardDataExtra4->sExtendedAVISpecialCodes, sizeof(pCardDataExtra4->sExtendedAVISpecialCodes)))
        {
            //4.0.24.60
            char cTmpAVISpecialCodes[25];
            memset(cTmpAVISpecialCodes, ' ', sizeof(cTmpAVISpecialCodes));
            memcpy(cTmpAVISpecialCodes,pCardData->sAVISpecialCodes,sizeof(pCardData->sAVISpecialCodes));
            memcpy(cTmpAVISpecialCodes+sizeof(pCardData->sAVISpecialCodes),pCardDataExtra4->sExtendedAVISpecialCodes,sizeof(pCardDataExtra4->sExtendedAVISpecialCodes));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AVISpecialCodes[S]",	(BYTE *)"%s=%.25s,",(BYTE *)cTmpAVISpecialCodes,sizeof(cTmpAVISpecialCodes));
        }
        else
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AVISpecialCodes[S]",	(BYTE *)"%s=%.8s,",		pCardData->sAVISpecialCodes,sizeof(pCardData->sAVISpecialCodes));
        }

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ResInfoAMax8[S]",		(BYTE *)"%s=%.8s,",		pCardData->sResInfoAMax8,sizeof(pCardData->sResInfoAMax8));

        //4.0.0.63		
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ClockFlags[I]",		(BYTE *)"%s=%.6s,",			pCardDataExtra2->sClockFlags,sizeof(pCardDataExtra2->sClockFlags));		
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"EmployeeName[S]",	(BYTE *)"%s=%.15s,",		pCardDataExtra2->sAttendantName,sizeof(pCardDataExtra2->sAttendantName));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"EmployeeId[I]",	(BYTE *)"%s=%.10s,",		pCardDataExtra2->sAttendantID,sizeof(pCardDataExtra2->sAttendantID));
        //4.0.0.63

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"IgnoreServiceFee[B]",(BYTE *)"%s=%.1s,",		&(pCardDataExtra2->sIgnoreServiceFee),sizeof(pCardDataExtra2->sIgnoreServiceFee));

        //3.2.0.76
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AuthTerminal[S]",	(BYTE *)"%s=%.7s,",			pCardData->sAuthTerminal,sizeof(pCardData->sAuthTerminal) ); //4.0.15.501
        
        //4.0.16.504
        lOffset += SetParam(pOutBuffer+lOffset, (BYTE *)"AVIBlockListPointer[S]", (BYTE *)"%s=%.4s,", pCardDataExtra4->dallasKeysInfo.sBlockListPointer, sizeof(pCardDataExtra4->dallasKeysInfo.sBlockListPointer));
        lOffset += SetParam(pOutBuffer+lOffset, (BYTE *)"AVIBlockedKeysRequest[B]", (BYTE *)"%s=%.1s,", &(pCardDataExtra4->dallasKeysInfo.sBlockedKeysRequest), sizeof(pCardDataExtra4->dallasKeysInfo.sBlockedKeysRequest));
            
        if( SESSION_TYPE(byOlaStat) == SESSION_GENERATE_REPORT )
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ReceiptType[I]",	(BYTE *)"%s=%.3s,",		pCardDataExtra2->sReceiptType ,sizeof(pCardDataExtra2->sReceiptType));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PrinterWidth[I]",	(BYTE *)"%s=%.3s,",		pCardDataExtra2->sPrinterWidth ,sizeof(pCardDataExtra2->sPrinterWidth));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ParamNumber[I]",	(BYTE *)"%s=%.2s,",		pCardDataExtra2->sParamNumber ,sizeof(pCardDataExtra2->sParamNumber));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"Param1[S]",			(BYTE *)"%s=%.37s,",	pCardDataExtra2->sParam1 ,sizeof(pCardDataExtra2->sParam1));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"Param2[S]",			(BYTE *)"%s=%.37s,",	pCardDataExtra2->sParam2 ,sizeof(pCardDataExtra2->sParam2));
        }
        if (SESSION_TYPE(byOlaStat) == SESSION_COMPLETE) //TD 70459  4.0.22.500
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ReceiptType[I]",	(BYTE *)"%s=%.3s,",		pCardDataExtra2->sReceiptType ,sizeof(pCardDataExtra2->sReceiptType));

			if ((pCardData->sEntryMethod == ' ') || (pCardData->sEntryMethod == 'M'))
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"EntryMethod[S]=S,");
			else
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"EntryMethod[S]", (BYTE *)"%s=%.1s,", &(pCardData->sEntryMethod), sizeof(pCardData->sEntryMethod));
		}
		else
		{
			if (pCardData->sEntryMethod == ' ')
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"EntryMethod[S]=S,");
			else
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"EntryMethod[S]", (BYTE *)"%s=%.1s,", &(pCardData->sEntryMethod), sizeof(pCardData->sEntryMethod));
        }

        //4.0.22.506 TD 74605
        if(!PUMP_NOT_VALID(lPumpNumber))
        {
            BOOL bIsUnmanned = _Module.m_server.m_cCommonFunction.IsPumpOnUnmannedMode(lPumpNumber);
            BYTE sInUnmanned = 'N';
            if(bIsUnmanned)
                sInUnmanned = 'Y';
            lOffset += SetParam(pOutBuffer+lOffset, (BYTE *)"UnattendedMode[B]", (BYTE *)"%s=%.1s,", &(sInUnmanned), sizeof(sInUnmanned));						
        }
        
        if (pInfo->CardSaleExtraData2.sLanguage != ' ')
        {
            long lTmp = 0 ;

            if (pInfo->CardSaleExtraData2.sLanguage)
                lTmp= pInfo->CardSaleExtraData2.sLanguage;
            else
                lTmp = LANG_ENGLISH;

            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"LangID[I]",	(BYTE *)"%s=%ld,",	lTmp);			
        } //4.0.24.70 115660
                
        if (pInfo->CardSaleExtraData2.sSessionOffLine != ' ')
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"SessionOffLine[B]",		(BYTE *)"%s=%.1s,",			&(pCardDataExtra2->sSessionOffLine),sizeof(pCardDataExtra2->sSessionOffLine));
        
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AuthorizerID[I]",	(BYTE *)"%s=%.5s,",		pCardDataExtra2->sAuthorizerID ,sizeof(pCardDataExtra2->sAuthorizerID));		//4.0.2.15
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"RedeemPoints[B]",	(BYTE *)"%s=%.1s,",		&(pCardDataExtra->sRedeemPoints) ,sizeof(pCardDataExtra->sRedeemPoints));		//4.0.3.47
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ConfirmPrompt[B]",	(BYTE *)"%s=%.1s,",		&(pCardDataExtra->sConfirmPrompt) ,sizeof(pCardDataExtra->sConfirmPrompt));		//4.0.3.54
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TotalRedeemedPoints[I]",	(BYTE *)"%s=%.6s,",		pCardDataExtra3->sTotalRedeemedPoints ,sizeof(pCardDataExtra3->sTotalRedeemedPoints));		//4.0.5.21

        lTmp = a2l(pCardDataExtra3->sAmountByPoints, sizeof(pCardDataExtra3->sAmountByPoints));

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AmountByPoints[F]",		(BYTE *)"%s=%ld.%03ld,", lTmp/100,lTmp%100);		

        //4.0.14.503
        lTmp = a2l(pCardDataExtra3->sAmountByPoints2, sizeof(pCardDataExtra3->sAmountByPoints2)); //4.0.14.33
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AmountByPoints2[F]",	(BYTE *)"%s=%ld.%03ld,", lTmp/100,lTmp%100);//4.0.14.33		

        lTmp = a2l(pCardDataExtra3->sCreditAmount, sizeof(pCardDataExtra3->sCreditAmount));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CreditAmount[F]",		(BYTE *)"%s=%ld.%03ld,", lTmp/100,lTmp%100);		

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"BankTermId[S]",	(BYTE *)"%s=%.8s,",		pCardDataExtra->sBankTermId,sizeof(pCardDataExtra->sBankTermId));

        //4.0.5.39
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PromptType[I]",	(BYTE *)"%s=%.2s,",		pCardDataExtra3->sPromptType ,sizeof(pCardDataExtra3->sPromptType));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PromptMinLen[I]",	(BYTE *)"%s=%.2s,",		pCardDataExtra3->sPromptMinLen ,sizeof(pCardDataExtra3->sPromptMinLen));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PromptMaxLen[I]",	(BYTE *)"%s=%.3s,",		pCardDataExtra3->sPromptMaxLen ,sizeof(pCardDataExtra3->sPromptMaxLen));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ParamIndex[I]",	(BYTE *)"%s=%.2s,",		pCardDataExtra3->sParamIndex ,sizeof(pCardDataExtra3->sParamIndex));
        
        if (!ChrAll((char*)&pCardDataExtra5->sParamResultExt, sizeof(pCardDataExtra5->sParamResultExt), ' '))				//4.0.28.502 255727			
        {
            memcpy(sTmpParamResult , &pCardDataExtra3->sParamResult , sizeof(pCardDataExtra3->sParamResult));	
            memcpy(&sTmpParamResult[113] , pCardDataExtra5->sParamResultExt , sizeof(pCardDataExtra5->sParamResultExt));	
            
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ParamResult[S]",	(BYTE *)"%s=%.118s,",		sTmpParamResult,sizeof(sTmpParamResult));
        }
        else
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ParamResult[S]",	(BYTE *)"%s=%.113s,",		pCardDataExtra3->sParamResult,sizeof(pCardDataExtra3->sParamResult));	
        }		
        
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"RemoteID[I]",	(BYTE *)"%s=%.6s,",		pCardDataExtra3->sRemoteID ,sizeof(pCardDataExtra3->sRemoteID));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"LogicShiftNumber[I]",	(BYTE *)"%s=%.2s,",		pCardDataExtra3->sLogicShiftNumber ,sizeof(pCardDataExtra3->sLogicShiftNumber));
        
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"KeyboardMask[I]",	(BYTE *)"%s=%.1s,",		&(pCardDataExtra3->sKeyboardMask) ,sizeof(pCardDataExtra3->sKeyboardMask));//4.0.16.70
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"OfflineTrans[B]",	(BYTE *)"%s=%.1s,",		&(pCardDataExtra3->sOfflineTrans) ,sizeof(pCardDataExtra3->sOfflineTrans));		//4.0.6.502
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"DryItemsRestriction[I]",	(BYTE *)"%s=%.1s,",		&(pCardDataExtra3->sDryItemsRestriction) ,sizeof(pCardDataExtra3->sDryItemsRestriction));		//4.0.10.70	
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CustomerValidationType[I]",	(BYTE *)"%s=%.1s,",		&(pInfo->CardSaleExtraData3.sCustomerValidationType) ,sizeof(pInfo->CardSaleExtraData3.sCustomerValidationType));

        //4.0.12.505 Start
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"MagCapabilities[S]",	(BYTE *)"%s=%.6s,",		pCardDataExtra3->sMagCapabilities ,sizeof(pCardDataExtra3->sMagCapabilities)); //4.0.15.500
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"IssueNumber[I]",	(BYTE *)"%s=%.2s,",		pCardDataExtra3->sIssueNumber ,sizeof(pCardDataExtra3->sIssueNumber));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TerminalType[S]",	(BYTE *)"%s=%.2s,",		pCardDataExtra3->sTerminalType ,sizeof(pCardDataExtra3->sTerminalType));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"STerminalCountryCode[I]",	(BYTE *)"%s=%.3s,",		pCardDataExtra3->sTerminalCountryCode ,sizeof(pCardDataExtra3->sTerminalCountryCode));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TransactionCurrencyCode[I]",	(BYTE *)"%s=%.3s,",		pCardDataExtra3->sTransactionCurrencyCode ,sizeof(pCardDataExtra3->sTransactionCurrencyCode));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AIP[I]",	(BYTE *)"%s=%.4s,",		pCardDataExtra3->sAIP ,sizeof(pCardDataExtra3->sAIP));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ATC[I]",	(BYTE *)"%s=%.4s,",		pCardDataExtra3->sATC ,sizeof(pCardDataExtra3->sATC));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TVR[I]",	(BYTE *)"%s=%.10s,",		pCardDataExtra3->sTVR ,sizeof(pCardDataExtra3->sTVR));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AID[S]",	(BYTE *)"%s=%.32s,",		pCardDataExtra3->sAID ,sizeof(pCardDataExtra3->sAID));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AVN[I]",	(BYTE *)"%s=%.4s,",		pCardDataExtra3->sAVN ,sizeof(pCardDataExtra3->sAVN));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PINEncryptionKey[S]",	(BYTE *)"%s=%.16s,",		pCardDataExtra2->sPinSessionKey ,sizeof(pCardDataExtra2->sPinSessionKey));  //4.0.13.510
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"IssueDate[E]",	(BYTE *)"%s=%.10s,",		pCardDataExtra3->sIssueDate ,sizeof(pCardDataExtra3->sIssueDate));

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CryptogramInfoData[I]",	(BYTE *)"%s=%.2s,",		pCardDataExtra3->sCryptogramInfoData ,sizeof(pCardDataExtra3->sCryptogramInfoData));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"IssuerApplicationData[I]",	(BYTE *)"%s=%.64s,",		pCardDataExtra3->sIssuerApplicationData ,sizeof(pCardDataExtra3->sIssuerApplicationData)); //4.0.15.500
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CVMResult[I]",	(BYTE *)"%s=%.6s,",		pCardDataExtra3->sCVMResult ,sizeof(pCardDataExtra3->sCVMResult));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TermResponseCode[I]",	(BYTE *)"%s=%.3s,",		pCardDataExtra3->sTermResponseCode ,sizeof(pCardDataExtra3->sTermResponseCode));  
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CombinedCardSelect[S]",	(BYTE *)"%s=%.1s,",		&pCardDataExtra3->sCombinedCardSelect ,sizeof(pCardDataExtra3->sCombinedCardSelect));
        
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"LoyaltySessionID[I]",	(BYTE *)"%s=%.7s,",		pCardDataExtra3->sLoyaltySessionID ,sizeof(pCardDataExtra3->sLoyaltySessionID));  //4.0.13.502
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ReasonOnLineCode[I]",	(BYTE *)"%s=%.2s,",		pCardDataExtra3->sReasonOnLineCode ,sizeof(pCardDataExtra3->sReasonOnLineCode));  //4.0.13.509
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CardHolderVerifyMethod[S]",	(BYTE *)"%s=%.3s,",		pCardDataExtra3->sCardHolderVerifyMethod ,sizeof(pCardDataExtra3->sCardHolderVerifyMethod));  //4.0.15.500
        //4.0.13.504
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TimeCardInserted[S]",	(BYTE *)"%s=%.9s,",		pCardDataExtra3->sTimeCardInserted ,sizeof(pCardDataExtra3->sTimeCardInserted));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TimeTRSRequired[S]",	(BYTE *)"%s=%.9s,",		pCardDataExtra3->sTimeTRSRequired ,sizeof(pCardDataExtra3->sTimeTRSRequired));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TimeTRSResponse[S]",	(BYTE *)"%s=%.9s,",		pCardDataExtra3->sTimeTRSResponse ,sizeof(pCardDataExtra3->sTimeTRSResponse));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TimeHCResponse[S]",		(BYTE *)"%s=%.9s,",		pCardDataExtra3->sTimeHCResponse ,sizeof(pCardDataExtra3->sTimeHCResponse));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TimePINRequest[S]",		(BYTE *)"%s=%.9s,",		pCardDataExtra3->sTimePINRequest ,sizeof(pCardDataExtra3->sTimePINRequest));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TimeOLARequest[S]",		(BYTE *)"%s=%.9s,",		pCardDataExtra3->sTimeOLARequest ,sizeof(pCardDataExtra3->sTimeOLARequest));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TimeOLAResult[S]",		(BYTE *)"%s=%.9s,",		pCardDataExtra3->sTimeOLAResult ,sizeof(pCardDataExtra3->sTimeOLAResult));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TimeTNXDetails[S]",		(BYTE *)"%s=%.9s,",		pCardDataExtra3->sTimeTNXDetails ,sizeof(pCardDataExtra3->sTimeTNXDetails));

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"HostOnLineID[I]",		(BYTE *)"%s=%.4s,",		pCardDataExtra3->sHostOnLineID ,sizeof(pCardDataExtra3->sHostOnLineID));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"AuthorizationSerialNumber[S]",		(BYTE *)"%s=%.3s,",		pCardDataExtra2->sAuthorizationSerialNumber ,sizeof(pCardDataExtra2->sAuthorizationSerialNumber));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CardType[I]",		(BYTE *)"%s=%.2s,",		pCardData->sCardType ,sizeof(pCardData->sCardType));  //4.0.13.511
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PONumber[S]",	(BYTE *)"%s=%.15s,", pCardDataExtra3->sPONumber ,sizeof(pCardDataExtra3->sPONumber));		//4.0.15.502
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TransactionType[S]",	(BYTE *)"%s=%.2s,", pCardDataExtra4->sTransactionType ,sizeof(pCardDataExtra4->sTransactionType)); 		//4.0.20.20

		str.Format("CardTypeThirdParty[S]= size= %ld, %.32s", _countof(pCardDataExtra6->sCardTypeThirdParty), pCardDataExtra6->sCardTypeThirdParty);
		_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", 0, LOG_NONE, str);

		if (!ChrAll((char*)pCardDataExtra6->sCardTypeThirdParty, sizeof(pCardDataExtra6->sCardTypeThirdParty)))
		{
			CString csTmp((LPCSTR)pCardDataExtra6->sCardTypeThirdParty, sizeof(pCardDataExtra6->sCardTypeThirdParty));
			csTmp.TrimRight();

			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"CardTypeThirdParty[S]", (BYTE *)"%s=%s,", (BYTE *)(LPCSTR)csTmp, csTmp.GetLength());
		}

		//		str.Format("IssuerScriptResult[S]=%.80s", _countof(pCardDataExtra6->sIssuerScriptResult), pCardDataExtra6->sIssuerScriptResult);
		//	_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", 0, LOG_NONE, str);

		if (!ChrAll((char*)pCardDataExtra6->sIssuerScriptResult, sizeof(pCardDataExtra6->sIssuerScriptResult)))
		{
			CString csTmp((LPCSTR)pCardDataExtra6->sIssuerScriptResult, sizeof(pCardDataExtra6->sIssuerScriptResult));
			csTmp.TrimRight();

			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"IssuerScriptResult[S]", (BYTE *)"%s=%s,", (BYTE *)(LPCSTR)csTmp, csTmp.GetLength());
		}

		if (!ChrAll((char*)pCardDataExtra6->sAppVersionNumberICC, sizeof(pCardDataExtra6->sAppVersionNumberICC)))
		{
			CString csTmp((LPCSTR)pCardDataExtra6->sAppVersionNumberICC, sizeof(pCardDataExtra6->sAppVersionNumberICC));
			csTmp.TrimRight();

			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"AppVersionNumberICC[I]", (BYTE *)"%s=%s,", (BYTE *)(LPCSTR)csTmp, csTmp.GetLength());
		}

		// QC 481403 //RFUEL-2817
		if (_Module.m_server.GetOLAInUse() == OLASRV_EPSILON || _Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT)
		{
			if (!ChrAll((char *)pInfo->CardSaleExtraData3.sTruckingCompanyName, sizeof(pInfo->CardSaleExtraData3.sTruckingCompanyName)))
			{
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TruckingCompanyName[S]", (BYTE *)"%s=%.20s,", pInfo->CardSaleExtraData3.sTruckingCompanyName, sizeof(pInfo->CardSaleExtraData3.sTruckingCompanyName));
			}

			if (!ChrAll((char *)pInfo->CardSaleInfo.cardData.sFullCardName, sizeof(pInfo->CardSaleInfo.cardData.sFullCardName)))
			{
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"FullCardName[S]", (BYTE *)"%s=%.20s,", pInfo->CardSaleInfo.cardData.sFullCardName, sizeof(pInfo->CardSaleInfo.cardData.sFullCardName));

				if (LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("FullCardName=%.20s", pInfo->CardSaleInfo.cardData.sFullCardName);
					_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", 0, LOG_NONE, str);
				}
			}
			else // in the case the FullCardName doesn't present -> must be fill value from TruckingCompanyName
			{
				CString str;
				str.Format("FullCardName doesn't present using TruckingCompanyName to filling tag=%.20s", pInfo->CardSaleExtraData3.sTruckingCompanyName);
				_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", std::stoi(std::string(pInfo->CardSaleInfo.cardData.sPumpNumber, pInfo->CardSaleInfo.cardData.sPumpNumber + sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber)).c_str()), LOG_PUMP, str);

				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"FullCardName[S]", (BYTE *)"%s=%.20s,", pInfo->CardSaleExtraData3.sTruckingCompanyName, sizeof(pInfo->CardSaleExtraData3.sTruckingCompanyName));
			}

			if (!ChrAll((char *)pInfo->CardSaleExtraData5.sIssuerCodeTableIndex, sizeof(pInfo->CardSaleExtraData5.sIssuerCodeTableIndex)))
			{
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"CardTableIndex[S]", (BYTE *)"%s=%.2s,", pInfo->CardSaleExtraData5.sIssuerCodeTableIndex, sizeof(pInfo->CardSaleExtraData5.sIssuerCodeTableIndex));

				if (LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("CardTableIndex=%.2s", pInfo->CardSaleInfo.cardData.sCardName);
					_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", 0, LOG_NONE, str);
				}
			}
			else
			{

				BYTE strDefaultValue[2] = {'0', '0'};
				CString str;
				str.Format("CardTableIndex use default value=%.2s", strDefaultValue);
				_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", std::stoi(std::string(pInfo->CardSaleInfo.cardData.sPumpNumber, pInfo->CardSaleInfo.cardData.sPumpNumber + sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber)).c_str()), LOG_PUMP, str);
				lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "CardTableIndex[S]", (BYTE *) "%s=%.2s,", strDefaultValue, sizeof(strDefaultValue));
			}
		}
		else
		{
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TruckingCompanyName[S]", (BYTE *)"%s=%.40s,", pInfo->CardSaleExtraData3.sTruckingCompanyName, sizeof(pInfo->CardSaleExtraData3.sTruckingCompanyName));	// 4.0.23.2410
		}


		//		if (!ChrAll((char*)pCardDataExtra6->sPinPadCapabilities, sizeof(pCardDataExtra6->sPinPadCapabilities)))
		{
			CString csLogMessage;

			csLogMessage.Format("COLABase::BuildRequest - PinPadCapabilities[I]=%.3s", pInfo->CardSaleExtraData6.sPinPadCapabilities);
			_Module.m_server.m_cLogger.LogMsg(csLogMessage);

			CString csTmp((LPCSTR)pCardDataExtra6->sPinPadCapabilities, sizeof(pCardDataExtra6->sPinPadCapabilities));
			csTmp.TrimRight();

			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "PinPadCapabilities[I]", (BYTE *) "%s=%s,", (BYTE *) (LPCSTR) csTmp, csTmp.GetLength());
		}

		if (!ChrAll((char*) pCardDataExtra6->sCardRiskManagementDataObjectList2, sizeof(pCardDataExtra6->sCardRiskManagementDataObjectList2)))
		{
			CString csTmp((LPCSTR)pCardDataExtra6->sCardRiskManagementDataObjectList2, sizeof(pCardDataExtra6->sCardRiskManagementDataObjectList2));
			csTmp.TrimRight();

			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) "CardRiskManagementDataObjectList2[I]", (BYTE *) "%s=%s,", (BYTE *) (LPCSTR) csTmp, csTmp.GetLength());
		}

        if (bBuildBufferToClient) 			//4.0.6.501
        {
            //4.0.6.503
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ClearanceID[I]",	(BYTE *)"%s=%.1s,",		&pCardData->sClearanceID ,sizeof(pCardData->sClearanceID));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CouponMessage[S]",	(BYTE *)"%s=%.80s,",		pInfo->CardSaleExtraData2.sCouponMessage,sizeof(pInfo->CardSaleExtraData2.sCouponMessage));		

            if(LOG_DETAIL_FLOW_CONTROL)
            {
                CString str;
                str.Format(">>> buffer CouponMessage %.80s",pInfo->CardSaleExtraData2.sCouponMessage); 
                _LOGMSG.LogMsg("COLABase","BuildRequest",0,LOG_NONE,str);
            }
        }

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"CustomerID[I]",	(BYTE *)"%s=%.10s,", pCardDataExtra3->sCustomerID, sizeof(pCardDataExtra3->sCustomerID));		//4.0.8.507
        
        sIncludeDiscountFlags =  (unsigned char )(pCardDataExtra3->sUnitPriceIncludeDiscount == '1' )? 'Y' : 'N';

        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"UnitPriceIncludeDiscount[B]", (BYTE *) "%s=%.1s,", &sIncludeDiscountFlags, sizeof(pCardDataExtra3->sUnitPriceIncludeDiscount));
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PreviousMediaMasterType[I]", (BYTE *) "%s=%.2s,", pInfo->CardSaleExtraData3.sPreviousMediaMasterType, sizeof(pCardDataExtra3->sPreviousMediaMasterType));

        //4.0.14.1390 67387 //4.0.21.0
        if((SESSION_TYPE(byOlaStat) == SESSION_COMPLETE) && (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetInPAKLoyaltyWithComplete()))
        {
            sForceZeroPAKCompletion = 'Y';
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"PAKwithLoyalty[B]",(BYTE *)"%s=%.1s,", &sForceZeroPAKCompletion, sizeof(pCardDataExtra4->sForceZeroPAKCompletion));
        }

        // TD473974 [8/5/2017 MaximB]
        if (SESSION_TYPE(byOlaStat) == SESSION_COMPLETE) 
        {
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ZeroCompleteStatus[I]",(BYTE *)"%s=%.3s,", pCardDataExtra4->sZeroCompleteStatus,sizeof(	pCardDataExtra4->sZeroCompleteStatus));
            if(LOG_DETAIL_FLOW_CONTROL)
            {
                CString str;						
                str.Format("ZeroCompleteStatus[I]=%.3s", pCardDataExtra4->sZeroCompleteStatus);
                _LOGMSG.LogMsg("COLABase","BuildRequest", lPumpNumber, LOG_PUMP, str);
            }

            
        } 

        //4.0.9.507
		if (pCardDataExtra2->sDisplayOLAResultMessage != ' ')
		{
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("Clean DisplayOLAResultMessage");
				_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", 0, LOG_NONE, str);
			}

			pCardDataExtra2->sDisplayOLAResultMessage = ' ';
		}

		if (!ChrAll((char *)pCardDataExtra5->sScreenMsgNumber, sizeof(pCardDataExtra5->sScreenMsgNumber)))
		{
			memset(pCardDataExtra5->sScreenMsgNumber, ' ', sizeof(pCardDataExtra5->sScreenMsgNumber));
		}

		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("Prompt Name is required = %c ", pCardDataExtra3->sWaitPromptNeedMoreParam);
			_LOGMSG.LogMsg("COLABase", "BuildRequest", lPumpNumber, LOG_PUMP, str);
		}
        // this section should moved to Mosaic ola.....		
        if (pCardDataExtra3->sWaitPromptNeedMoreParam  == 'Y')  // wait to approperate pilot ola
        {
            pCardDataExtra3->sWaitPromptNeedMoreParam = ' ';  //4.0.10.502
            AddPromptsRespons(lOffset, pOutBuffer ,pInfo);
        }
        //4.0.9.507

        if (( SESSION_TYPE(byOlaStat) == SESSION_COMPLETE ) && (OLA_STATE(byOlaStat) == OLA_REQUEST_TO_SEND) )
                SendAllPrompts(lOffset, pOutBuffer, pInfo);   //4.0.10.50
                        

        if(LOG_DETAIL_FLOW_CONTROL)
        {
            CString str;
            str.Format(">>> buffer size %ld", lOffset); 
            _LOGMSG.LogMsg("COLABase","BuildRequest", 0, LOG_NONE, str);
        }

    }
#endif 

#ifndef NEW_OLA_STRUCT
	CString csFormatString =
		"TransNumber[I]=%.5s,"
		"PumpNumber[I]=%ld,"
		"PumpMode[I]=%ld,"
		"TransAmountField(SESSION_TYPE(byOlaStat) == SESSION_COMPLETE) + " = %ld. % 03ld, "
		"TransTaxAmount[F] = %ld.00,"
		"Track1[S] = %.76s,"
		"Track2[S] = %.37s,"
		"Track3[S] = %.104s,"
		"TerminalId[S] = % 04d,"
		"Odometer[S] = %.6s,"
		"CarRegistration[S] = %.16s,"
		"PIN[S] = %.16s,"
		"ReturnTank[I] = %ld,"
		"DriverID[S] = %.6s,"
		"OldSessID[I] = %.4s,"
		"Grade[I] = %d,"
		"UnitPrice[F] = %ld. % 03ld,"
		"TransVolume[F] = %ld. % 03ld,"
		"ReferenceNumber[S] = %.12s,"
		"AuthorizationNumber[S] = %.12s,"
		"ExpirationDate[E] = %.6s,"
		"Approval[S] = %.12s,"
		"IsStored[B] = %c, ";

	sprintf(pCardData->sTranNbr,
            lPumpNumber,
            lPumpMode,
            lValue/1000,lValue%1000,
            SURCHARGE_AMOUNT_FOR_PRE_AUTH,
            pCardData->sTrack1Data,
            pCardData->sTrack2Data,
            pCardData->sTrack3Data,
            SERVER_OLA_POS_NUMBER,
            pCardData->sOdometer, 
            pCardData->sFullVehicleId,
            pCardData->sPINBlock,
            a2l(&(pCardData->sReturnTank),sizeof(pCardData->sReturnTank)),
            pCardData->sDriverId,
            pCardData->sAuthorizeSessionID,
            nGrade,
            lGradePrice/1000 ,lGradePrice%1000 ,
            lVolume/1000,lVolume%1000,
            pCardData->sReferenceNumber,
            pCardData->sAuthNumber,
            sExpire,
            pCardData->sApproval,
            pCardData->sIsStoreAndForward);

#endif 
    return 0;
}

long COLABase::SetParam(char *pBuff,BYTE *pParamName)
{
    long lCopiedLen = 0;

    if(pParamName[0] != 0)
        lCopiedLen = (long)sprintf(pBuff,"%s",pParamName) ; 
    
    return lCopiedLen;
}

long COLABase::SetParam(char *pBuff,BYTE *pParamName,BYTE *pParamFormat,BYTE *pParamVal,long lParamLen)
{
    long lCopiedLen = 0;
    BOOL bEmpty = TRUE;
    
    for(long i = 0;i < lParamLen; i++)
    {
        if(pParamVal[i] != ' ')
            bEmpty = FALSE;
    }

    if(!bEmpty)
    {
        if((((BYTE *)pParamVal)[0] != 0)&&(pParamName[0] != 0)&&(pParamFormat[0] != 0))
            lCopiedLen = (long)sprintf(pBuff,(const char *)pParamFormat,pParamName,pParamVal) ; 
    }

    return lCopiedLen;
}

long COLABase::SetParamNoCheck(char *pBuff,BYTE *pParamName,BYTE *pParamFormat,BYTE *pParamVal,long lParamLen)
{
    long lCopiedLen = 0;

    if((((BYTE *)pParamVal)[0] != 0)&&(pParamName[0] != 0)&&(pParamFormat[0] != 0))
        lCopiedLen = (long)sprintf(pBuff,(const char *)pParamFormat,pParamName,pParamVal) ; 
    
    return lCopiedLen;
}


long COLABase::SetParam(char *pBuff,BYTE *pParamName,BYTE *pParamFormat,long lVal)
{
    long lCopiedLen = 0;
    
    if((pParamName[0] != 0)&&(pParamFormat[0] != 0))
        lCopiedLen = (long)sprintf(pBuff,(const char *)pParamFormat,pParamName,lVal) ; 
    
    return lCopiedLen;
}

long COLABase::SetParam(char *pBuff,BYTE *pParamName,BYTE *pParamFormat,long lVal1, long lVal2)
{
    long lCopiedLen = 0;
    
    if((pParamName[0] != 0)&&(pParamFormat[0] != 0))
        lCopiedLen = (long)sprintf(pBuff,(const char *)pParamFormat,pParamName,lVal1,lVal2) ; 
    
    return lCopiedLen;
}

long COLABase::SetParam(char *pBuff,BYTE *pParamName1,long lItemCount,BYTE *pParamName2,BYTE *pParamFormat,long lVal)
{
    long lCopiedLen = 0;
    
    char *pParamName = new char[strlen((const char *)pParamName1)+strlen((const char *)pParamName2)+10];
    if ( pParamName )
    {
        sprintf(pParamName,"%s%ld%s",pParamName1,lItemCount,pParamName2);
        if((pParamName[0] != 0)&&(pParamFormat[0] != 0))
            lCopiedLen = (long)sprintf(pBuff,(const char *)pParamFormat,pParamName,lVal) ; 

        //4.0.2.15 changed to delete[]
        delete[] pParamName;
    }

    return lCopiedLen;
}

long COLABase::SetParam(char *pBuff,BYTE *pParamName1,long lItemCount,BYTE *pParamName2,BYTE *pParamFormat,long lVal1, long lVal2)
{
    long lCopiedLen = 0;

    char *pParamName = new char[strlen((const char *)pParamName1)+strlen((const char *)pParamName2)+10];
    if ( pParamName )
    {
        sprintf(pParamName,"%s%ld%s",pParamName1,lItemCount,pParamName2);
        if((pParamName[0] != 0)&&(pParamFormat[0] != 0))
            lCopiedLen = (long)sprintf(pBuff,(const char *)pParamFormat,pParamName,lVal1,lVal2) ; 

        //4.0.2.15 changed to delete[]
        delete[] pParamName;
    }

    return lCopiedLen;
}


long COLABase::BuildSessionType(short byOlaStat, char *sReqType)  //4.0.9.507
{
    long lRtc = 0;

    
    switch(SESSION_TYPE(byOlaStat))
    {
        case OLA_NONE:
            lRtc =1; 
            break;
        case SESSION_AUTHORIZE:
            memcpy(sReqType,"Authorize",9);
            break;
        case SESSION_GET_RECEIPT:
        case SESSION_RECEIPT_AUTH:
            memcpy(sReqType,"ReceiptInfo",11);
            break;
        case SESSION_COMPLETE:
        case SESSION_CANCEL:
            memcpy(sReqType,"Complete",8);
            break;
        case SESSION_RECEIPT_UPDATE:
            memcpy(sReqType,"ReceiptUpdate",13);
            break;
        case SESSION_SALE: //3.0.0.8
            memcpy(sReqType,"Sale",4);
            break;
        case SESSION_GENERATE_REPORT:
            memcpy(sReqType,"Gen_Report",10);
            break;
        case SESSION_GET_REPORT:
            memcpy(sReqType,"Get_Report",10);
            break;
        //husky april start
        case SESSION_INIT_DEBIT:   //3.2.0.9
            memcpy(sReqType,"InitDebit",9);		
            break;
        case SESSION_SHIFT_DEBIT: //3.2.0.9
            memcpy(sReqType,"DebitShift",11);
            break;
        case SESSION_CAPTURE:
            memcpy(sReqType,"CAPTURE",7);  //5.0.0.55
            break;
        case SESSION_CLOCK:
            memcpy(sReqType,"CLOCK",7);  //5.0.0.55
            break;
        case SESSION_INQUIRY:
            memcpy(sReqType,"INQUIRY",7); //4.0.3.21
            break;
        case SESSION_REPORT_PRICE_CHANGE:
            memcpy(sReqType,"PRICE CHANGE",12); //4.0.9.511
            break;
        case SESSION_LOGON:
            memcpy(sReqType,"LOGIN",5); //4.0.28.503 - TD 256429
            break;
		case SESSION_GET_CUSTOMER_INFO:
			memcpy(sReqType, "GET_CUST_INFO", 13);
			break;
		case SESSION_SET_CUSTOMER_INFO:
			memcpy(sReqType, "SET_CUST_INFO", 13);
			break;

    }



    return lRtc;
}

BOOL COLABase::CanPrintLogMessages()
{
    BOOL bRet = TRUE;

    if (m_nDeclineCounter > m_lOlaMaxDeclineMessToDisplay)
        bRet = FALSE;
        
    else
    {
        if (m_nDeclineCounter == m_lOlaMaxDeclineMessToDisplay)
            _LOGMSG.LogMsg("COLABase","CanPrintLogMessages",0,LOG_NONE,"Stop Display Ola Messages !!!!");			
    
        m_nDeclineCounter++;
    }

    return (bRet);
}

BOOL COLABase::GetParam(CString *pInStr,int iPos,char **ppParam,int *pLen, char *pFindStr)
{
    if(iPos == -1)
        return FALSE; // not found

    else
    {
        const char *pTmp;
        pTmp = (LPCTSTR)(*pInStr);
        BOOL bFound = (iPos == 0);
        
        if (pFindStr)
        {
            while (!bFound)
            {
                if ( (pTmp[iPos - 1] == ',') ||
                     (pTmp[iPos - 1] == ';') ||
                     (pTmp[iPos - 1] == ' ') )
                     bFound = TRUE;
                else
                {
                    iPos = pInStr->Find(pFindStr,iPos + 1);
                    if(iPos == -1)
                        return FALSE; // not found
                }

            }
        }

        CString tmpStr(pTmp+iPos);
        int iStartPos = tmpStr.Find('=');
        
        if(iStartPos == -1) 
            return FALSE; // not found

        iStartPos += iPos;
        iStartPos ++;

        while((pTmp[iStartPos] == ' ')&&(iStartPos < pInStr->GetLength()) )
                iStartPos++;

        int iEndPos = tmpStr.Find(',');
        if(iEndPos == -1) 
        {
            iEndPos = tmpStr.Find(';'); // line delimiter
        
            if(iEndPos == -1) 
                return FALSE; // not found
        }

        iEndPos += iPos;
        *ppParam = (char *)(pTmp + iStartPos);

        if(iEndPos > iStartPos)
            *pLen = iEndPos -  iStartPos;
        
        else
            *pLen = 0;
    }
    
    return TRUE;
}


long COLABase::GetParamLen(CString *pInStr,int iPos)
{
    // 4.0.5.33
    BOOL bFound =FALSE;
    if(iPos == -1)
        return -1; // not found
        //return 0; // not found 4.0.5.39
    
    else
    {
        const char *pTmp;
        pTmp = (LPCTSTR)(*pInStr);
        CString tmpStr(pTmp+iPos);

        // check that we found the exact string.
        if (iPos == 0)
        {
            bFound = TRUE;

        }
        else if ( (pTmp[iPos - 1] == ',') ||
                  (pTmp[iPos - 1] == ';') ||
                 (pTmp[iPos - 1] == ' ') )
             bFound = TRUE;
        else
            bFound = FALSE;

            
        if (bFound)
        {
            int iStartPos = tmpStr.Find('=');
            
            if(iStartPos == -1) 				
                return 0; // not found

            iStartPos += iPos;
            iStartPos ++;

            while((pTmp[iStartPos] == ' ')&&(iStartPos < pInStr->GetLength()) )
                    iStartPos++;

            int iEndPos = tmpStr.Find(',');
            if(iEndPos == -1) 
            {
                iEndPos = tmpStr.Find(';'); // line delimiter

                if(iEndPos == -1) 
                    return -1; //4.0.5.39
                    //return 0; // not found
            }

            iEndPos += iPos;

            if(iEndPos > iStartPos)
                return (iEndPos -  iStartPos);			
            else				
                return 0;

        }
        else
            return -1; //4.0.5.39
    }

    
    /*long lRtc = 0;

    if(iPos == -1)
        lRtc =  0; // not found
    
    else
    {
        const char *pTmp;
        pTmp = (LPCTSTR)(*pInStr);
        CString tmpStr(pTmp+iPos);


        if (iPos == 0)
            lRtc  = 0; // matching to required str   
        else
        {
            if ( (pTmp[iPos - 1] == ',') ||
                 (pTmp[iPos - 1] == ';') ||
                 (pTmp[iPos - 1] == ' ') )
                     lRtc = 0;				  	
                else
                    lRtc = 1;  // not matching  				
        }
    }


    return lRtc;

  */


/*
        int iStartPos = tmpStr.Find('=');
        
        if(iStartPos == -1) 
            return 0; // not found

        iStartPos += iPos;
        iStartPos ++;

        while((pTmp[iStartPos] == ' ')&&(iStartPos < pInStr->GetLength()) )
                iStartPos++;

        int iEndPos = tmpStr.Find(',');
        if(iEndPos == -1) 
        {
            iEndPos = tmpStr.Find(';'); // line delimiter

            if(iEndPos == -1) 
                return 0; // not found
        }

        iEndPos += iPos;

        if(iEndPos > iStartPos)
            return (iEndPos -  iStartPos);
        
        else
            return 0;
    }
    */
}


BOOL COLABase::IsDisplayMessageValid()
{
    BOOL bRet = TRUE;

    if (m_nDeclineCounter > MAX_DECLINE_MESSAGES)
        bRet = FALSE;
    
    return (bRet);
}

BOOL COLABase::OLAStopped()
{
    return m_bOLAStopped;
}

/*
void COLABase::StopOLA()
{
    m_bOLAStopped = TRUE;
    ReleaseService(TRUE);
}
*/
//4.0.5.0
void COLABase::ParseCardData(BSTR sCardBuf, CARD_SALE_ALL3 *pCardData,long lPumpNumber)  //4.0.0.75
{
    BOOL bFound;
    int iLen ;
    char *pParam;
    long lRetCode = 0;
    CString sInStr;

	iLen = Bstr2Buffer(sCardBuf, sInStr);
	if (iLen <= 0)
	{
		_LOGMSG.LogMsg("ParseCardData Receive empty buffer from EPS!");
	}

	sInStr.Insert(iLen, ',');

    if(LOG_BASIC_FLOW_CONTROL) //RFUEL-1367
    {
		CString protectedDataStr;
		_Module.m_server.m_cProtectedDataHandler.RemoveDataFromLog(sInStr, protectedDataStr);
		_LOGMSG.LogOLABufferMsg(lPumpNumber, LOG_PUMP, ">>> GetCardInfo, Recived Buffer:", protectedDataStr);
		
    }

    bFound = GetParam(&sInStr,sInStr.Find("ResultCode[I]"),&pParam,&iLen);
    if(bFound) 
    {
        char sRes[4];
        memset(sRes,0,sizeof(sRes));
        memcpy(sRes,pParam,min(sizeof(sRes),iLen));
        switch(a2l((unsigned char *)sRes,sizeof(sRes)))
        {
            case CARDSRV_Approved: 
            case CARDSRV_NeedMoreParam:
            {
                pCardData->CardSaleAll.data.sTranStatus = CD_APPROVE_OK;
                break;
            }
            default: 
            {
                pCardData->CardSaleAll.data.sTranStatus = CD_DECLINE_NOGOOD;

                if(LOG_ERROR)
                {
                    CString str;
                    str.Format("ResultCode[I]=%ld" ,a2l((unsigned char *)sRes,sizeof(sRes)));
                    _LOGMSG.LogMsg("COLABase","ParseCardData",0,LOG_NONE,str);
                }
                
                break;
            }
        }
    }
    else
    {
        //4.0.20.52 63936
        bFound = GetParam(&sInStr,sInStr.Find("ResultCode[S]"),&pParam,&iLen);

        if(bFound) 
        {
            char sRes[4];
            memset(sRes,0,sizeof(sRes));
            memcpy(sRes,pParam,min(sizeof(sRes),iLen));
            switch(a2l((unsigned char *)sRes,sizeof(sRes)))
            {
                case CARDSRV_Approved: 
                case CARDSRV_NeedMoreParam:
                {
                    pCardData->CardSaleAll.data.sTranStatus = CD_APPROVE_OK;
                    break;
                }
                default: 
                {
                    pCardData->CardSaleAll.data.sTranStatus = CD_DECLINE_NOGOOD;

                    if(LOG_ERROR)
                    {
                        CString str;
                        str.Format("ResultCode[S]=%ld" ,a2l((unsigned char *)sRes,sizeof(sRes)));
                        _LOGMSG.LogMsg("COLABase","ParseCardData",0,LOG_NONE,str);
                    }
                    
                    break;
                }
            }
        }
        else
        {
            pCardData->CardSaleAll.data.sTranStatus = CD_DECLINE_ERROR;
        }
    }

    bFound = GetParam(&sInStr,sInStr.Find("ExpirationDate[E]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pCardData->CardSaleAll.data.sExpire,' ',sizeof(pCardData->CardSaleAll.data.sExpire));
        if(iLen)
            memcpy(pCardData->CardSaleAll.data.sExpire,pParam+2,4);	
    }
   
    bFound = GetParam(&sInStr,sInStr.Find("AccountNumber[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pCardData->CardSaleAll.data.sAccountNumber, ' ', sizeof(pCardData->CardSaleAll.data.sAccountNumber));
        if(iLen)
            memcpy(pCardData->CardSaleAll.data.sAccountNumber,pParam,min(sizeof(pCardData->CardSaleAll.data.sAccountNumber),iLen));
    }
    
    bFound = GetParam(&sInStr,sInStr.Find("ShortCardName[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pCardData->CardSaleAll.data.sCardName,' ',sizeof(pCardData->CardSaleAll.data.sCardName));
        if(iLen)
        {
            memcpy(pCardData->CardSaleAll.data.sCardName,pParam,min(sizeof(pCardData->CardSaleAll.data.sCardName),iLen));
        }
    }

    bFound = GetParam(&sInStr,sInStr.Find("CardType[I]"),&pParam,&iLen,"CardType[I]");
    if(bFound) 
    {
        memset(pCardData->CardSaleAll.data.sCardType	,' ',sizeof(pCardData->CardSaleAll.data.sCardType));
        if(iLen)
        {
            long lTmp = a2l((BYTE *)pParam,iLen); 
            l2a(lTmp,pCardData->CardSaleAll.data.sCardType,sizeof(pCardData->CardSaleAll.data.sCardType));
        }
    }
    
    //4.0.0.75							   
    bFound = GetParam(&sInStr,sInStr.Find("EmployeeName[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pCardData->extData2.sAttendantName,' ',sizeof(pCardData->extData2.sAttendantName));
        if(iLen)
            memcpy(pCardData->extData2.sAttendantName,pParam,min(sizeof(pCardData->extData2.sAttendantName),iLen));
    }

    bFound = GetParam(&sInStr,sInStr.Find("EmployeeId[I]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pCardData->extData2.sAttendantID,' ',sizeof(pCardData->extData2.sAttendantID));
        if(iLen)
            memcpy(pCardData->extData2.sAttendantID,pParam,min(sizeof(pCardData->extData2.sAttendantID),iLen));
    }


    //4.0.25.501 TD 137509						   
    bFound = GetParam(&sInStr, sInStr.Find("EmployeeCard[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pCardData->extData5.sAttendantTag, ' ', sizeof(pCardData->extData5.sAttendantTag));
        if(iLen)
            memcpy(pCardData->extData5.sAttendantTag, pParam, min(sizeof(pCardData->extData5.sAttendantTag),iLen));
    }

    //4.0.2.31
    //4.0.5.1
    long lMediaMasterType = 0;
    bFound = GetParam(&sInStr,sInStr.Find("MediaMasterType[I]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pCardData->extData2.sMediaMasterType,' ',sizeof(pCardData->extData2.sMediaMasterType));
        if(iLen)
            memcpy(pCardData->extData2.sMediaMasterType,pParam,min(sizeof(pCardData->extData2.sMediaMasterType),iLen));

        lMediaMasterType = a2l(pCardData->extData2.sMediaMasterType,sizeof(pCardData->extData2.sMediaMasterType));
        if (lMediaMasterType == CARD_TYPE_ATTENDANT)
            pCardData->CardSaleAll.extData.sAttandantSession = '1';

    }


        //Liat LPE
    bFound = GetParam(&sInStr,sInStr.Find("CardSegment[I]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(&pCardData->extData3.sCardSegment,' ',sizeof(pCardData->extData3.sCardSegment));
        if(iLen)
            memcpy(&pCardData->extData3.sCardSegment,pParam,min(sizeof(pCardData->extData3.sCardSegment),iLen));
    }


    //4.0.2.32

    
    //4.0.3.21
    bFound = GetParam(&sInStr,sInStr.Find("MediaType[S]"),&pParam,&iLen);
    if(bFound) 
    {
        pCardData->CardSaleAll.data.sTranType = ' ';
        if(iLen)
            memcpy(&pCardData->CardSaleAll.data.sTranType,pParam,1);
    }
    //4.0.3.21

    //4.0.4.12
    bFound = GetParam(&sInStr,sInStr.Find("ClearanceID[I]"),&pParam,&iLen);
    if(bFound) 
    {
        pCardData->CardSaleAll.data.sClearanceID = ' ';
        if(iLen)
            memcpy(&pCardData->CardSaleAll.data.sClearanceID,pParam,1);

    }
    //4.0.4.12

        //4.0.9.500
    
    bFound = GetParam(&sInStr,sInStr.Find("DoSessionAuth[B]"),&pParam,&iLen);
    if(bFound) 
    {	
        pCardData->extData3.sDoSessionAuth = ' ';			

        if(iLen)
            pCardData->extData3.sDoSessionAuth = pParam[0];
                
    }

    bFound = GetParam(&sInStr,sInStr.Find("ClubCardNumber[S]"),&pParam,&iLen); // 4.0.20.50
    if(bFound) 
    {
        memset(pCardData->CardSaleAll.extData.sLoyalId,' ',sizeof(pCardData->CardSaleAll.extData.sLoyalId));
        if(iLen)
            memcpy(pCardData->CardSaleAll.extData.sLoyalId,pParam,min(sizeof(pCardData->CardSaleAll.extData.sLoyalId),iLen));
    }

    bFound = GetParam(&sInStr,sInStr.Find("ClubCardNumberHash[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pCardData->extData6.sClubCardHashNumber,' ',sizeof(pCardData->extData6.sClubCardHashNumber));
        if(iLen)
            memcpy(pCardData->extData6.sClubCardHashNumber,pParam,min(sizeof(pCardData->extData6.sClubCardHashNumber),iLen));
    } // 4.0.32.??? TD 405435

    bFound = GetParam(&sInStr,sInStr.Find("TenderID[I]"),&pParam,&iLen);
    
    if(bFound) 
    {
        if(_Module.m_server.m_cCommonFunction.IsPaymentCard(lMediaMasterType))
        {
            long lNumberOfTendersAuthorized =  _Module.m_server.GetNumberOfTendersAuthotized(lPumpNumber-1);
            
            memset(pCardData->extData6.PaymentCard1.sTenderId + (lNumberOfTendersAuthorized * sizeof(PAYMENT_CARD_DETAILS)),' ',sizeof(pCardData->extData6.PaymentCard1.sTenderId));
            if(iLen)
                memcpy(pCardData->extData6.PaymentCard1.sTenderId + (lNumberOfTendersAuthorized * sizeof(PAYMENT_CARD_DETAILS)),pParam,min(sizeof(pCardData->extData6.PaymentCard1.sTenderId),iLen));
        }
    }


    if(lMediaMasterType != CARD_TYPE_LOYALTY)
    {
        bFound = GetParam(&sInStr, sInStr.Find("LocalSegment[I]"), &pParam, &iLen);	
        if(bFound) 
        {	
            long lLocalSegment = a2l((unsigned char *) pParam,sizeof(pParam));
            _Module.m_server.m_cFuelLoayaltyPS.SetAddMemberInfo(lPumpNumber,lLocalSegment);
        } // 4.0.23.2280 TD 305114	
    }

    bFound = GetParam(&sInStr,sInStr.Find("LangID[I]"),&pParam,&iLen);
    if(bFound && iLen) 
    {	
        long lNewLanguageId = a2l((BYTE *)pParam,iLen);

        //_Module.m_server.m_cFuelLoayaltyPS.SetLanguageIdForUpdate(lPumpNumber,lNewLanguageId,pCardData); // 4.0.32.220 TD 409107

        memset(&pCardData->extData2.sLanguage ,' ',sizeof(pCardData->extData2.sLanguage));

        pCardData->extData2.sLanguage = (BYTE)lNewLanguageId; //4.0.24.71 115660

        if(LOG_DETAIL_FLOW_CONTROL)
        {
            CString str;
            str.Format("Received language=%ld" ,lNewLanguageId);
            _LOGMSG.LogMsg("COLABase","ParseCardData",0,LOG_NONE,str);
        }	
    }	

    bFound = GetParam(&sInStr,sInStr.Find("AmountLimit[F]"),&pParam,&iLen);
    if (bFound)
    {
        if (iLen)
        {
                double dAmount = atof(pParam);
                long lAmountLimit = RoundDouble(dAmount, 100);
                if (_Module.m_server.IsNeedCreditLimitCheck())
                {
                    long lPumpNumber = a2l(pCardData->CardSaleAll.data.sPumpNumber, sizeof(pCardData->CardSaleAll.data.sPumpNumber));
                    long lValue = _Module.m_server.GetLimit(PRICE_LEVEL_CREDIT, lPumpNumber);
                    long lDeviceLimit = 0;

                    lValue /= 10;  //4.0.1.40

                    lDeviceLimit = lAmountLimit;

                    if ((lDeviceLimit > lValue))
                    {
                        lAmountLimit = lValue;
                    }
                    else
                        lAmountLimit = lDeviceLimit;
                }

                if ((pCardData->CardSaleAll.data.sEntryMethod == CD_TYPE_FLEET) || (pCardData->CardSaleAll.data.sEntryMethod == CD_TYPE_SPEED_PASS))
                { 
                    _LOGMSG.LogClassMsg("COLABase", "ParseCardData(GetCardInfo)", 0, LOG_NONE, "Tap transaction, ignoring AmountLimit!");

                }
                else
                {
                    BYTE sTmpValue[6] = { 0 };
                    l2a0(lAmountLimit, sTmpValue, sizeof(sTmpValue));
                    memcpy(pCardData->extData4.sPreAuthAmount, sTmpValue, sizeof(pCardData->extData4.sPreAuthAmount));
                }


            

        }	
    }
}

__int64 COLABase::ParseData(long lState, CString *pInStr,PAY_AT_PUMP_INFO *pInfo, CPumpTransact *pTrs , short byOlaStat, long lPumpNum) //4.0.9.507 //4.0.16.503 //4.0.22.511 74797
{
    CString str;
    BOOL bFound;
    int iLen ;
    char *pParam;
    __int64 lRetCode = 0;

    *pInStr +=  ',';
	
//  _LOGMSG.LogMsg("ParseData Logging incoming data from EPS");
//  _LOGMSG.LogMsg(pInStr->GetBuffer(0));

    bFound = GetParam(pInStr,pInStr->Find("ResultCode[S]"),&pParam,&iLen);
    if(bFound) 
    {
        char sRes[4];
        memset(sRes,0,sizeof(sRes));
        memcpy(sRes,pParam,min(sizeof(sRes),iLen));
        switch(a2l((unsigned char *)sRes,sizeof(sRes)))
        {
            case OLASRV_Approved: //     Approved
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_APPROVE_OK;
                break;
            }
            case OLASRV_BatchOK: //      Successful receipt of batch totals        
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_APPROVE_BACTCHOK;
                break;
            }
            case OLASRV_Decline_FailedSelfCheck: //      Failed to pass self check or denied by the HOST system
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_INVALID;
                break;
            }
            case OLASRV_Decline_CardExpired: //      Card expired
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_EXPIRE;
                break;
            }
            case OLASRV_Decline: //      Transaction has been declined
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_DECLINED;				
                break;
            }
            case OLASRV_Decline_DebitNetworkDown: //      Debit network is down
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_NONET;
                break;
            }
            case OLASRV_Decline_InvalidStateCode: //      State code invalid
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_NOSTATE;
                break;
            }
            case OLASRV_Decline_BadFormat: //      Transaction data is invalid or formated incorrectly by the site
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_BADFMT;
                break;
            }
            case OLASRV_Decline_NoHost: //      Host is experiencing system problem
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_NOHOST;
                break;
            }
            case OLASRV_Decline_UseAlt: //      Use alternate authorization method
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_USEALT;
                break;
            }
            case OLASRV_Decline_CallCenter: //      Call center for voice authorization
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_CALL;
                break;
            }
            case OLASRV_Decline_PINError: //      Excessive debit PIN errors
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_TRYPIN;
                break;
            }
            case OLASRV_Decline_VoidNoTarget: //      Void has no target on host system
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_VOID;
                break;
            }
            case OLASRV_Decline_NoMoney: //      Insufficient funds
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_NOMONEY;
                break;
            }
            case OLASRV_Decline_AmountMismatch: //      Void amount does not match the target transaction on the host
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_AMTERR;
                break;
            }
            case OLASRV_Decline_AccountMismatch: //      Void account number does not match the target transaction on the                             host.
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_ACTERR;
                break;
            }
            case OLASRV_Decline_InvalidDriverID: //      Invalid driver number
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_DRIVER;
                break;
            }
            case OLASRV_Decline_InvalidVeicleID: //      Invalid veicle number
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_VEHICLE;
                break;
            }
            case OLASRV_Decline_InvalidMerchantID: //      Merchant ID error
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_MERCHANT;
                break;
            }
            case OLASRV_Decline_InvalidPIN: //      PIN error
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_PIN;
                break;
            }
            case OLASRV_Decline_BatchOutOfBalance: //      Batch was closed out of balance
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_OUTBAL;
                break;
            }
            case OLASRV_Decline_RetriveCard: //      Transaction is declined and the card should retrieved from the customer
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_GETCARD;
                break;
            }
            case OLASRV_Decline_Transaction: //      Transaction is declined
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_NOGOOD;
                break;
            }
            case OLASRV_Decline_DateTime: //      Date/Time error
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_DATETIME;
                break;
            }
            case OLASRV_Decline_CardType: //      Card not accepted by the merchant
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_BADCARD;
                break;
            }

            case OLASRV_Receipt_PrintedBefore:
            case OLASRV_Receipt_NoReceipt:
            case OLASRV_Receipt_OldReceipt:
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_ERROR;
                break;
            }

            case OLASRV_Decline_DailyLimitExceeded: // Daily limit exceeded
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_DAILYLIMIT;
                break;
            }

            case OLASRV_Decline_GR:
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_GR;
                break;
            }

            case OLASRV_Decline_NoMember:
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_NO_MEMBER;				
                break;
            }			
            case OLASRV_Decline_Transaction2:	//4.0.2.38
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_DECLINE_TRANSACTION_2;				
                break;
            }				
            case OLASRV_Decline_NoMoney2:	//4.0.2.38
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_NOMONEY_2;				
                break;
            }
            case OLASRV_Decline_balaceAmount:
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_AMOUNT;				
                break;
            }
            case OLASRV_Decline_NotInValidRang:  //4.0.0.78
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_NOT_VALID_RANG;				
                break;
            }
            case OLASRV_Decline_PanMismatchCard:  // 4.0.31.270 FSD 403522 [23/7/2014 MaximB]
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_PAN_MISMATCH_CARD;				
                break;
            }
            case OLASRV_Decline_ContactLessError:  //  [16/6/2015 MaximB]
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_CONTACTLESS_ERROR;				
                break;
            }

            case OLASRV_Decline_Transaction_Not_Completed: // 4.0.3.2
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_TRANSACTION_NOT_COMPLETED;
            }
            break;
            case OLASRV_Decline_Transaction_Not_Approved:  //4.0.3.2
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_TRANSACTION_NOT_APPROVED;
            }
            break;
            case OLASRV_Decline_payment_Need_clear_OLA_buffer_except_loyalty:  //4.0.14.505
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_PAYMENT_NEED_CLEAR_OLA_BUFFER_EXCEPT_LOYALTY;
            }
            break;
                
            //3.2.0.73
            case OLASRV_Decline_Error: //      Unknown error
            case OLASRV_Decline_BadSwipe:
            case OLASRV_Decline_CardNotValid:
            case OLASRV_Decline_AmountOutsideRange:
            case OLASRV_Decline_UnknownPromptId:
            case OLASRV_Decline_InvalidProdId:
            case OLASRV_Decline_RestrictedAccountNumber:
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_ERROR;
                break;
            }
            //4.0.22.0 70991 :
            case CARDSRV_Decline_Card_Blocked:
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_CARD_BLOCKED;
                break;
            }
			case OLASRV_Offline_Host:
			{
				pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_CARD_BLOCKED;
				break;
			}
			case Loyalty_Unavailable:
			{
				pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_INVALID_LOYALTY_CARD;
				break;
			}
            default: 
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_RESPONSE_EXPECTED;
                break;
            }
        }
    }
    else
    {
        pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_ERROR;
    }

    // Retrive transaction information 
    // Used only in cases of "SESSION_GET_RECEIPT" session or "SESSION_RECEIPT_AUTH"
    if( pTrs ) 
    {
        bFound = GetParam(pInStr,pInStr->Find("TransVolume[F]"),&pParam,&iLen);
        if(bFound) 
        {
            if(iLen)
            {
                double x;
                x = atof(pParam);
                x *= 1000;				
                pTrs->m_lRoundedVolume = (long)x;
                pTrs->m_lRawVolume = (long)x;
            }
            else
            {
                pTrs->m_lRoundedVolume = 0;
                pTrs->m_lRawVolume = 0;
            }		
        }														 

        bFound = GetParam(pInStr,pInStr->Find("Grade[I]"),&pParam,&iLen);
        if(bFound) 
        {
            if(iLen)
            {
                pTrs->m_nGrade = a2i((BYTE *)pParam,iLen);
            }
            else
            {
                pTrs->m_nGrade = 0;
            }		
        }														 

        bFound = GetParam(pInStr,pInStr->Find("UnitPrice[F]"),&pParam,&iLen);
        if(bFound) 
        {
            if(iLen)
            {
                double x;
                x = atof(pParam);
                pTrs->m_lGradePrice = RoundDouble(x , 1000);
            }
            else
            {
                pTrs->m_lGradePrice = 0;
            }		
        }		 

        bFound = GetParam(pInStr,pInStr->Find("TransAmount[F]"),&pParam,&iLen);
        if(bFound) 
        {
            if(iLen)
            {
                double x;
                x = atof(pParam);
                
                pTrs->m_lRoundedValue = RoundDouble(x , 1000);	//4.0.3.61				
            }
            else
            {
                pTrs->m_lRoundedValue = 0;
            }		
        }
    }

    WORD wGradeRestriction = 0xffff; // start with everything allowed.
    bFound = GetParam(pInStr,pInStr->Find("ValidDepList[S]"),&pParam,&iLen);
    if(bFound) 
    {
        BYTE byNozzle;
        BYTE byNozzleRestriction = 0;
        wGradeRestriction = 0;
        if(iLen)
        {
            long lPumpNumber = a2l(pInfo->CardSaleInfo.cardData.sPumpNumber,sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));
            CString sAllDepList(pParam,iLen);
            for(int i = 0; i < MAX_DEP_IN_LIST; i++)
            {	
                if(sAllDepList.Find(sDepName[i]) != -1)
                {
                    wGradeRestriction += 1<<i;
                    if ( _Module.m_server.NozzleLevelLogic() )
                    {
                        //Get the positon of grade number i+1, at pump number pInfo->cardData.sPumpNumber
                        byNozzle = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetGradePosition(i+1);
                        if ( byNozzle ) //If there is nozzle with that grade,
                            byNozzleRestriction |= 1<<(byNozzle-1); //add it to the bit field.
                    }
                }
            }
        }

        i2a(wGradeRestriction,pInfo->CardSaleInfo.cardData.sGradeRestriction,sizeof(pInfo->CardSaleInfo.cardData.sGradeRestriction));
        if ( _Module.m_server.NozzleLevelLogic() )
            i2a(byNozzleRestriction,pInfo->CardSaleInfo.cardData.sNozzleRestriction,sizeof(pInfo->CardSaleInfo.cardData.sNozzleRestriction));
    }

    bFound = GetParam(pInStr,pInStr->Find("InValidDepList[S]"),&pParam,&iLen);
    if(bFound) 
    {
        BYTE byNozzle;
        BYTE byNozzleRestriction = 0;
        wGradeRestriction = 0xffff;
        if(iLen)
        {
            long lPumpNumber = a2l(pInfo->CardSaleInfo.cardData.sPumpNumber,sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));
            CString sAllDepList(pParam,iLen);			
            for(int i = 0; i < MAX_DEP_IN_LIST; i++)
            {	
                if(sAllDepList.Find(sDepName[i]) != -1)
                {
                    wGradeRestriction -= 1<<i;
                }
            }
            if ( _Module.m_server.NozzleLevelLogic() )
            {
                for(int i = 0; i < MAX_DEP_IN_LIST; i++)
                {	
                    if(wGradeRestriction & (1 << i))
                    {
                        //Get the positon of grade number i+1, at pump number pInfo->cardData.sPumpNumber.
                        byNozzle = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetGradePosition(i+1);
                        if ( byNozzle ) //If there is nozzle with that grade,
                            byNozzleRestriction -= 1<<(byNozzle-1); //remove it from the bit field.
                    }
                }
            }
        }

        i2a(wGradeRestriction,pInfo->CardSaleInfo.cardData.sGradeRestriction,sizeof(pInfo->CardSaleInfo.cardData.sGradeRestriction));
        if ( _Module.m_server.NozzleLevelLogic() )
            i2a(byNozzleRestriction,pInfo->CardSaleInfo.cardData.sNozzleRestriction,sizeof(pInfo->CardSaleInfo.cardData.sNozzleRestriction));
    }
    
    bFound = GetParam(pInStr,pInStr->Find("PrintReceiptTime[B]"),&pParam,&iLen);
    if(bFound) 
    {
        if(iLen)
        {
            long lReceiptRestriction = a2l(pInfo->CardSaleInfo.cardData.sReceiptRestriction,sizeof(pInfo->CardSaleInfo.cardData.sReceiptRestriction));
            //3.1.1.1
            if ((char)*pParam == 'N')
                lReceiptRestriction |= PRINT_RECEIPT_TIME;
            
            l2a(lReceiptRestriction,pInfo->CardSaleInfo.cardData.sReceiptRestriction,sizeof(pInfo->CardSaleInfo.cardData.sReceiptRestriction));
        }
    }
    
    bFound = GetParam(pInStr,pInStr->Find("PrintReceiptPrice[B]"),&pParam,&iLen);
    if(bFound) 
    {
        if(iLen)
        {
            long lReceiptRestriction = a2l(pInfo->CardSaleInfo.cardData.sReceiptRestriction,sizeof(pInfo->CardSaleInfo.cardData.sReceiptRestriction));
            //3.1.1.1
            if ((char)*pParam == 'N')
                lReceiptRestriction |= PRINT_RECEIPT_PRICE;

            l2a(lReceiptRestriction,pInfo->CardSaleInfo.cardData.sReceiptRestriction,sizeof(pInfo->CardSaleInfo.cardData.sReceiptRestriction));
        }
    }
    
    bFound = GetParam(pInStr,pInStr->Find("CardType[I]"),&pParam,&iLen,"CardType[I]");
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.cardData.sCardType,' ',sizeof(pInfo->CardSaleInfo.cardData.sCardType));
        if(iLen)
        {
            long lTmp = a2l((BYTE *)pParam,iLen); 
            l2a(lTmp,pInfo->CardSaleInfo.cardData.sCardType,sizeof(pInfo->CardSaleInfo.cardData.sCardType));
        }
    }

    bFound = GetParam(pInStr,pInStr->Find("CardIssue[I]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.cardData.sCardIssue,' ',sizeof(pInfo->CardSaleInfo.cardData.sCardIssue));
        if(iLen)
        {
            long lTmp = a2l((BYTE *)pParam,iLen); 
            l2a(lTmp,pInfo->CardSaleInfo.cardData.sCardIssue,sizeof(pInfo->CardSaleInfo.cardData.sCardIssue));
        }
    }

    //4.0.23.500
    bFound = GetParam(pInStr,pInStr->Find("DoNotPrintExpirationDate[B]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData3.sDoNotPrintExpirationDate,' ',sizeof(pInfo->CardSaleExtraData3.sDoNotPrintExpirationDate));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData3.sDoNotPrintExpirationDate,pParam,min(sizeof(pInfo->CardSaleExtraData3.sDoNotPrintExpirationDate),iLen));

        switch(pInfo->CardSaleExtraData3.sDoNotPrintExpirationDate)	//4.0.21.500 changing ola parameter from 1/0 to Y/N
        {
        case 'Y':
                pInfo->CardSaleExtraData3.sDoNotPrintExpirationDate = '1';
            break;
        case 'N':
                pInfo->CardSaleExtraData3.sDoNotPrintExpirationDate = '0';
            break;
        }
    }


    bFound = GetParam(pInStr,pInStr->Find("StartDate[E]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.cardData.sStartDate,' ',sizeof(pInfo->CardSaleInfo.cardData.sStartDate));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sStartDate,pParam+2,4);	
    }

    bFound = GetParam(pInStr,pInStr->Find("ReceiptPrinted[I]"),&pParam,&iLen);
    if(bFound) 
    {
        if(iLen)		
            pInfo->CardSaleInfo.extraData.sReciptStatus = *pParam;
        else
            pInfo->CardSaleInfo.extraData.sReciptStatus = '0';
    }														 

    bFound = GetParam(pInStr,pInStr->Find("ClubcardPoints[I]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.extraData.sLoyalPoints, ' ', sizeof(pInfo->CardSaleInfo.extraData.sLoyalPoints));
        memset(pInfo->CardSaleExtraData6.sLoyaltyPointsExt, ' ', sizeof(pInfo->CardSaleExtraData6.sLoyaltyPointsExt));
        if (iLen)
        {
            long lTmp = a2l((BYTE *)pParam, iLen);
            Convertl2Str2(lTmp, pInfo->CardSaleExtraData6.sLoyaltyPointsExt, sizeof(pInfo->CardSaleExtraData6.sLoyaltyPointsExt),
                pInfo->CardSaleInfo.extraData.sLoyalPoints, sizeof(pInfo->CardSaleInfo.extraData.sLoyalPoints));
        }
    }

    bFound = GetParam(pInStr,pInStr->Find("ClubcardScheme[I]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.extraData.sLoyalScheme,' ',sizeof(pInfo->CardSaleInfo.extraData.sLoyalScheme));
        if(iLen)
        {
            long lTmp = a2l((BYTE *)pParam,iLen); 
            l2a(lTmp,pInfo->CardSaleInfo.extraData.sLoyalScheme,sizeof(pInfo->CardSaleInfo.extraData.sLoyalScheme));			
        }
    }

    bFound = GetParam(pInStr,pInStr->Find("ClubCardNumber[S]"),&pParam,&iLen); // HUSKY (change from Clubcard to ClubCard)
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.extraData.sLoyalId,' ',sizeof(pInfo->CardSaleInfo.extraData.sLoyalId));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.extraData.sLoyalId,pParam,min(sizeof(pInfo->CardSaleInfo.extraData.sLoyalId),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("VatRate[I]"),&pParam,&iLen);
    if(bFound) 
    {
		BYTE sTmpVat[sizeof(pInfo->CardSaleInfo.extraData.sVAT_Rate)+sizeof(pInfo->CardSaleExtraData5.sVAT_RateMSB)] = {' '}; //4.0.31.500  TD 404911
        if(iLen)
        {
            memcpy(sTmpVat,pParam,min(sizeof(sTmpVat),iLen));
            UnpackFields(sTmpVat,pInfo->CardSaleInfo.extraData.sVAT_Rate,sizeof(pInfo->CardSaleInfo.extraData.sVAT_Rate),
                &pInfo->CardSaleExtraData5.sVAT_RateMSB,sizeof(pInfo->CardSaleExtraData5.sVAT_RateMSB));
        }
    }	

    // TD 387091
    bFound = GetParam(pInStr,pInStr->Find("AuthorizationNumber[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        BYTE sTmpAuthNumber[sizeof(pInfo->CardSaleInfo.cardData.sAuthNumber)+sizeof(pInfo->CardSaleExtraData5.sAuthNumberEx)];
        memset(sTmpAuthNumber,' ',sizeof(sTmpAuthNumber));

        if(iLen)
        {
            memcpy(sTmpAuthNumber,pParam,min(sizeof(sTmpAuthNumber),iLen));
            UnpackFields(sTmpAuthNumber,pInfo->CardSaleExtraData5.sAuthNumberEx,sizeof(pInfo->CardSaleExtraData5.sAuthNumberEx),
                pInfo->CardSaleInfo.cardData.sAuthNumber,sizeof(pInfo->CardSaleInfo.cardData.sAuthNumber));
        }
    }

    bFound = GetParam(pInStr,pInStr->Find("MerchantID[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleInfo.cardData.sMerchantID,' ',sizeof(pInfo->CardSaleInfo.cardData.sMerchantID));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sMerchantID,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sMerchantID),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("HostTerminalID[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleInfo.cardData.sHostTerminalID,' ',sizeof(pInfo->CardSaleInfo.cardData.sHostTerminalID));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sHostTerminalID,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sHostTerminalID),iLen));
    }
    
    bFound = GetParam(pInStr,pInStr->Find("ReferenceNumber[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleInfo.cardData.sReferenceNumber,' ',sizeof(pInfo->CardSaleInfo.cardData.sReferenceNumber));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sReferenceNumber,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sReferenceNumber),iLen));
    }
    
    bFound = GetParam(pInStr,pInStr->Find("IsStored[B]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleInfo.cardData.sIsStoreAndForward,' ',sizeof(pInfo->CardSaleInfo.cardData.sIsStoreAndForward));
        if(iLen)
            memcpy(&pInfo->CardSaleInfo.cardData.sIsStoreAndForward,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sIsStoreAndForward),iLen));			
    }

    bFound = GetParam(pInStr,pInStr->Find("IsOnLine[B]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleInfo.cardData.sIsOnLine,' ',sizeof(pInfo->CardSaleInfo.cardData.sIsOnLine));
        if(iLen)
            memcpy(&pInfo->CardSaleInfo.cardData.sIsOnLine,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sIsOnLine),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("ResultMessage[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleInfo.cardData.sResultMsg,' ',sizeof(pInfo->CardSaleInfo.cardData.sResultMsg));
        if(iLen)
        {
            memcpy(pInfo->CardSaleInfo.cardData.sResultMsg,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sResultMsg),iLen));
        }
    }

    bFound = GetParam(pInStr,pInStr->Find("Approval[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleInfo.cardData.sApproval,' ',sizeof(pInfo->CardSaleInfo.cardData.sApproval));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sApproval,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sApproval),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("FinalRefNumber[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleInfo.cardData.sFinalRefNumber,' ',sizeof(pInfo->CardSaleInfo.cardData.sFinalRefNumber));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sFinalRefNumber,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sFinalRefNumber),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("FinalAuthNumber[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleInfo.cardData.sFinalAuthNumber,' ',sizeof(pInfo->CardSaleInfo.cardData.sFinalAuthNumber));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sFinalAuthNumber,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sFinalAuthNumber),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("ShortCardName[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.cardData.sCardName,' ',sizeof(pInfo->CardSaleInfo.cardData.sCardName));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sCardName,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sCardName),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("FullCardName[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.cardData.sFullCardName,' ',sizeof(pInfo->CardSaleInfo.cardData.sFullCardName));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sFullCardName,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sFullCardName),iLen));
    }

    //4.0.22.509 74387
    bFound = GetParam(pInStr,pInStr->Find("LoyaltyName[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData4.sLoyaltyName,' ',sizeof(pInfo->CardSaleExtraData4.sLoyaltyName));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.sLoyaltyName,pParam,min(sizeof(pInfo->CardSaleExtraData4.sLoyaltyName),iLen));
    }
    
    //4.0.24.90 118124
    bFound = GetParam(pInStr,pInStr->Find("OLAExtraData[S]"),&pParam,&iLen);
    if(bFound) 
    {
        CAsh2000File  cAsh2000File;
        long lPumpNumber = a2l(pInfo->CardSaleInfo.cardData.sPumpNumber,sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));

        if(!PUMP_NOT_VALID(lPumpNumber)) // TD 41144 - 4.0.15.72
        {
            if (!cAsh2000File.InsertData(lPumpNumber ,pParam ,iLen,pTrs))
            {
                long lTmp = a2l(pInfo->CardSaleExtraData3.sFlags1,sizeof(pInfo->CardSaleExtraData3.sFlags1)) + FL_OLA_EXTRA_DATA;
                l2a(lTmp ,pInfo->CardSaleExtraData3.sFlags1, sizeof(pInfo->CardSaleExtraData3.sFlags1));

            }
        }
        else
        {
            if(LOG_BASIC_FLOW_CONTROL) // 4.0.15.72
            {
                CString str;
                str.Format("Pump number is not valid! Pump=%ld", lPumpNumber);
                _LOGMSG.LogMsg("COLABase","ParseData",0,LOG_NONE,str);
            }
        }
    }

    // HUSKY (start)

    bFound = GetParam(pInStr,pInStr->Find("BankTermId[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.extraData.sBankTermId,' ',sizeof(pInfo->CardSaleInfo.extraData.sBankTermId));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.extraData.sBankTermId,pParam,min(sizeof(pInfo->CardSaleInfo.extraData.sBankTermId),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("CustomerName[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.extraData.sCustomerName,' ',sizeof(pInfo->CardSaleInfo.extraData.sCustomerName));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.extraData.sCustomerName,pParam,min(sizeof(pInfo->CardSaleInfo.extraData.sCustomerName),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("ISOResponse[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.extraData.sISOResponse,' ',sizeof(pInfo->CardSaleInfo.extraData.sISOResponse));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.extraData.sISOResponse,pParam,min(sizeof(pInfo->CardSaleInfo.extraData.sISOResponse),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("SPDHCode[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.extraData.sSPDHCode,' ',sizeof(pInfo->CardSaleInfo.extraData.sSPDHCode));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.extraData.sSPDHCode,pParam,min(sizeof(pInfo->CardSaleInfo.extraData.sSPDHCode),iLen));
    }


    //4.0.16.504
    bFound = GetParam(pInStr,pInStr->Find("AVIBlockedListBuffer[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData4.dallasKeysInfo.sBlockedListBuffer,' ',sizeof(pInfo->CardSaleExtraData4.dallasKeysInfo.sBlockedListBuffer));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.dallasKeysInfo.sBlockedListBuffer,pParam,min(sizeof(pInfo->CardSaleExtraData4.dallasKeysInfo.sBlockedListBuffer),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("AVIBlockedListBufferLength[I]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(&pInfo->CardSaleExtraData4.dallasKeysInfo.sBlockedListBufferLength,' ',sizeof(pInfo->CardSaleExtraData4.dallasKeysInfo.sBlockedListBufferLength));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData4.dallasKeysInfo.sBlockedListBufferLength,pParam,min(sizeof(pInfo->CardSaleExtraData4.dallasKeysInfo.sBlockedListBufferLength),iLen));
    }

    //4.0.17.501 - CR 38162
    bFound = GetParam(pInStr,pInStr->Find("LoyaltyType[I]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData4.discountAttrsInfo.sLoyaltyType,' ',sizeof(pInfo->CardSaleExtraData4.discountAttrsInfo.sLoyaltyType));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.discountAttrsInfo.sLoyaltyType,pParam,min(sizeof(pInfo->CardSaleExtraData4.discountAttrsInfo.sLoyaltyType),iLen));
    }

    //4.0.17.501 - CR 38162
    // search for ItemDiscountID, if found then try to parse the data 
    bFound = GetParam(pInStr,pInStr->Find("ItemDiscountId"),&pParam,&iLen); 

    if(bFound)
    {
        memset(pInfo->CardSaleExtraData4.discountAttrsInfo.DiscountIDs ,' ' , sizeof(pInfo->CardSaleExtraData4.discountAttrsInfo.DiscountIDs));
        memset(pInfo->CardSaleExtraData4.sDiscountAttrExt.sDiscountTypes ,' ' , sizeof(pInfo->CardSaleExtraData4.sDiscountAttrExt.sDiscountTypes));
        long lIndex =0;
        
        for(int i = 0; i < 4; i++)	//4.0.22.509 38162
        {	
            CString sItemDiscountId;
            sItemDiscountId.Format("ItemDiscountId%d[I]=", i);  
            bFound = GetParam(pInStr,pInStr->Find(sItemDiscountId),&pParam,&iLen); 

            if((bFound) && iLen)
            {
                long lVal = a2l((BYTE *)pParam,iLen);

                l2a(lVal,
                    pInfo->CardSaleExtraData4.discountAttrsInfo.DiscountIDs[i].sDiscountID,
                    min(sizeof(pInfo->CardSaleExtraData4.discountAttrsInfo.DiscountIDs[i].sDiscountID),iLen)); //4.0.22.509 38162
 
                
                if(LOG_DETAIL_FLOW_CONTROL)
                {	
                    CString str;
                    str.Format("ItemDiscountId =%d, indexArray=%ld" ,lVal, i); //4.0.22.509 38162
                    _LOGMSG.LogMsg("COLABase","ParseData",0,LOG_NONE,str);
                }
            }

            CString sItemDiscountType;
            sItemDiscountType.Format("ItemDiscountType%d[I]=", i);  
            bFound = GetParam(pInStr,pInStr->Find(sItemDiscountType),&pParam,&iLen); 

            if((bFound) && iLen)
            {
                long lVal = a2l((BYTE *)pParam,iLen);

                l2a(lVal,
                    pInfo->CardSaleExtraData4.sDiscountAttrExt.sDiscountTypes[i].sItemDiscountType,
                    min(sizeof(pInfo->CardSaleExtraData4.sDiscountAttrExt.sDiscountTypes[i].sItemDiscountType),iLen)); //4.0.22.509 38162
 
                
                if(LOG_DETAIL_FLOW_CONTROL)
                {	
                    CString str;
                    str.Format("ItemDiscountType =%d, indexArray=%ld" ,lVal, i); //4.0.22.509 38162
                    _LOGMSG.LogMsg("COLABase","ParseData",0,LOG_NONE,str);
                }

            }
        }
    }

    //3.1.1.1
    bFound = GetParam(pInStr,pInStr->Find("LangID[I]"),&pParam,&iLen);
    if(bFound && iLen) 
    {	
        long lNewLanguageId = a2l((BYTE *)pParam,iLen);

        memset(&pInfo->CardSaleExtraData2.sLanguage ,' ',sizeof(pInfo->CardSaleExtraData2.sLanguage));

        pInfo->CardSaleExtraData2.sLanguage = (BYTE)lNewLanguageId; //4.0.24.71 115660

        if(LOG_DETAIL_FLOW_CONTROL)
        {
            CString str;
            str.Format("Received language=%ld" ,lNewLanguageId);
            _LOGMSG.LogMsg("COLABase","ParseCardData",0,LOG_NONE,str);
        }	
    }	
    
    //4.0.20.20 - start
    pInfo->CardSaleExtraData4.sExtraChipInfo = ' ';
    long lPumpNumber = a2l(pInfo->CardSaleInfo.cardData.sPumpNumber,sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));
    if(!PUMP_NOT_VALID(lPumpNumber))
        memset(&_Module.m_server.m_cExtraChipInfo[lPumpNumber-1], ' ', sizeof(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1]));
    long lPumpIndex = lPumpNumber-1;
    
    bFound = GetParam(pInStr,pInStr->Find("IssuerScriptData[S]"),&pParam,&iLen);
    if(bFound)
    {
        long lPumpNumber = a2l(pInfo->CardSaleInfo.cardData.sPumpNumber,sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));

        if(!PUMP_NOT_VALID(lPumpNumber))
        {
            memset(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData.byLen, ' ', sizeof(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData.byLen));
            memset(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData.byData, ' ', sizeof(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData.byData));	
            
            if(iLen)
            {
                pInfo->CardSaleExtraData4.sExtraChipInfo = '1';
                l2a0(iLen,_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData.byLen, sizeof(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData.byLen));
                memcpy(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData.byData, pParam, min(sizeof(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData.byData),iLen));

                if(LOG_DETAIL_FLOW_CONTROL)
                {
                    CString str;
                    str.Format("String IssuerScriptData[S] was found ,Len=%d", iLen);
                    _LOGMSG.LogMsg("COLABase","BuildRequest",lPumpNumber,LOG_PUMP,str);
                }
            }
        }		
    }
    //for future use
    bFound = GetParam(pInStr,pInStr->Find("IssuerScriptData2[S]"),&pParam,&iLen);
    if(bFound)
    {
        long lPumpNumber = a2l(pInfo->CardSaleInfo.cardData.sPumpNumber,sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));
        if(!PUMP_NOT_VALID(lPumpNumber))
        {
            memset(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData2.byLen, ' ', sizeof(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData2.byLen));
            memset(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData2.byData, ' ', sizeof(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData2.byData));			
            if(iLen)
            {
                pInfo->CardSaleExtraData4.sExtraChipInfo = '1';
                l2a0(iLen,_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData2.byLen, sizeof(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData2.byLen));
                memcpy(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData2.byData, pParam, min(sizeof(_Module.m_server.m_cExtraChipInfo[lPumpNumber-1].byIssuerScriptData2.byData),iLen));

                if(LOG_DETAIL_FLOW_CONTROL)
                {
                    CString str;
                    str.Format("String IssuerScriptData2[S] was found ,Len=%d", iLen);
                    _LOGMSG.LogMsg("COLABase","BuildRequest",lPumpNumber,LOG_PUMP,str);
                }
            }
        }		
    }

    //4.0.20.20 - end

    bFound = GetParam(pInStr,pInStr->Find("PinPadAction[I]"),&pParam,&iLen);
    // 4.0.2.15
    if(bFound) 
    {
        memset(&(pInfo->CardSaleExtraData2.sPinPadAction),' ',sizeof(pInfo->CardSaleExtraData2.sPinPadAction));
        if(iLen)
        {
            long lPumpNumber = a2l(pInfo->CardSaleInfo.cardData.sPumpNumber,sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));
            long lTmp = a2l((BYTE *)pParam,iLen);  //4.0.14.492
            l2a(lTmp,&(pInfo->CardSaleExtraData2.sPinPadAction),sizeof(pInfo->CardSaleExtraData2.sPinPadAction));
            
            bFound = GetParam(pInStr,pInStr->Find("Data2MAC[S]"),&pParam,&iLen);

            if (!bFound)
                bFound = GetParam(pInStr,pInStr->Find("Data2Verify[S]"),&pParam,&iLen);

            if(bFound) 
            {					
                if(( SESSION_TYPE(byOlaStat) == SESSION_SHIFT_DEBIT) ||	
                    (SESSION_TYPE(byOlaStat) == SESSION_INIT_DEBIT))	//4.0.0.53
                {
                    BYTE sLen[5];

                    if(iLen)														
                        l2a0(iLen,sLen, 4);
                    else
                        l2a0(0,sLen, 4);
                    
                    _Module.m_server.m_cDebitInfoArray[lPumpNumber-1].UpdateData( sLen ,(unsigned char *)pParam);
                }
                else 
                {
                    memset(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byLen, ' ', sizeof(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byLen));
                    memset(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byData, ' ', sizeof(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byData));
                    
                    if(iLen)
                    {
                        l2a0(iLen,_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byLen, sizeof(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byLen));
                        memcpy(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byData, pParam, min(sizeof(_Module.m_server.m_cOLAExtraArray[lPumpNumber-1].byData),iLen));
                    }
                }
            }
        }
    }

    //4.0.2.15

    bFound = GetParam(pInStr,pInStr->Find("CalculatedMAC[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sCalculatedMAC,' ',sizeof(pInfo->CardSaleExtraData2.sCalculatedMAC));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sCalculatedMAC,pParam,min(sizeof(pInfo->CardSaleExtraData2.sCalculatedMAC),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("PINEncryptionKey[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sPinSessionKey,' ',sizeof(pInfo->CardSaleExtraData2.sPinSessionKey));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sPinSessionKey,pParam,min(sizeof(pInfo->CardSaleExtraData2.sPinSessionKey),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("MACEncryptionKey[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sMACSessionKey,' ',sizeof(pInfo->CardSaleExtraData2.sMACSessionKey));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sMACSessionKey,pParam,min(sizeof(pInfo->CardSaleExtraData2.sMACSessionKey),iLen));
    }

// HUSKY (end)

    bFound = GetParam(pInStr,pInStr->Find("ExpirationDate[E]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.cardData.sExpire,' ',sizeof(pInfo->CardSaleInfo.cardData.sExpire));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sExpire,pParam+2,4);	
    }

    /*bFound = GetParam(pInStr,pInStr->Find("AccountNumber[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.cardData.sAccountNumber, ' ', sizeof(pInfo->CardSaleInfo.cardData.sAccountNumber));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sAccountNumber,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sAccountNumber),iLen));
    }*/


    bFound = GetParam(pInStr,pInStr->Find("CarRegistration[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.cardData.sFullVehicleId, ' ', sizeof(pInfo->CardSaleInfo.cardData.sFullVehicleId));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sFullVehicleId,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sFullVehicleId),iLen));
    }

    //4.0.22.511 74797 start
    bFound = GetParam(pInStr,pInStr->Find("Job[S]"),&pParam,&iLen);	
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData4.sJob, ' ', sizeof(pInfo->CardSaleExtraData4.sJob));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.sJob,pParam,min(sizeof(pInfo->CardSaleExtraData4.sJob),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("Department[S]"),&pParam,&iLen);	
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData4.sDepartment, ' ', sizeof(pInfo->CardSaleExtraData4.sDepartment));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.sDepartment,pParam,min(sizeof(pInfo->CardSaleExtraData4.sDepartment),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("License[S]"),&pParam,&iLen);	
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData4.sLicense, ' ', sizeof(pInfo->CardSaleExtraData4.sLicense));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.sLicense,pParam,min(sizeof(pInfo->CardSaleExtraData4.sLicense),iLen));
    }


    bFound = GetParam(pInStr,pInStr->Find("CustomerData[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sCustomerReference, ' ', sizeof(pInfo->CardSaleExtraData2.sCustomerReference));
        memset(pInfo->CardSaleExtraData4.sCustomerReferenceExt, ' ', sizeof(pInfo->CardSaleExtraData4.sCustomerReferenceExt));	

        if(iLen)
        {
            if (iLen > 10) 
            {
                memcpy(pInfo->CardSaleExtraData2.sCustomerReference, &pParam[iLen - 10] ,  sizeof(pInfo->CardSaleExtraData2.sCustomerReference));
                memcpy(pInfo->CardSaleExtraData4.sCustomerReferenceExt, pParam, sizeof(pInfo->CardSaleExtraData4.sCustomerReferenceExt));			 
            }
            else
            {
                memcpy(pInfo->CardSaleExtraData2.sCustomerReference,pParam,min(sizeof(pInfo->CardSaleExtraData2.sCustomerReference),iLen));			 

            }
        }
    }

    bFound = GetParam(pInStr,pInStr->Find("UserID[I]"),&pParam,&iLen);
    if(bFound) 
    {
        //4.0.0.43		
        memset(pInfo->CardSaleExtraData3.sCustomerID, ' ', sizeof(pInfo->CardSaleExtraData3.sCustomerID));
        memset(pInfo->CardSaleExtraData4.sCustomerIDExt, ' ', sizeof(pInfo->CardSaleExtraData4.sCustomerIDExt));	

        if(iLen)
        {
            if (iLen > 10) 
            {
                memcpy(pInfo->CardSaleExtraData3.sCustomerID, &pParam[iLen - 10] ,  sizeof(pInfo->CardSaleExtraData3.sCustomerID));
                memcpy(pInfo->CardSaleExtraData4.sCustomerIDExt, pParam, sizeof(pInfo->CardSaleExtraData4.sCustomerIDExt));			 
            }
            else
            {
                memcpy(pInfo->CardSaleExtraData3.sCustomerID,pParam,min(sizeof(pInfo->CardSaleExtraData3.sCustomerID),iLen));			 

            }
        }
    }

    //4.0.22.511 74797 end

    bFound = GetParam(pInStr,pInStr->Find("Odometer[S]"),&pParam,&iLen);
    if(bFound) 
    {
        //4.0.0.43		
        memset(pInfo->CardSaleInfo.cardData.sOdometer, ' ', sizeof(pInfo->CardSaleInfo.cardData.sOdometer));
        memset(pInfo->CardSaleInfo.extraData.sOdometerExt, ' ', sizeof(pInfo->CardSaleInfo.extraData.sOdometerExt));
        memset(&pInfo->CardSaleExtraData4.sOdometerExt2, ' ', sizeof(pInfo->CardSaleExtraData4.sOdometerExt2));	//4.0.22.511 74797

        if(iLen)
        {
            if (iLen > 8) //4.0.22.511 74797
            {
                memcpy(pInfo->CardSaleInfo.cardData.sOdometer, &pParam[iLen - 6] ,  sizeof(pInfo->CardSaleInfo.cardData.sOdometer));
                memcpy(pInfo->CardSaleInfo.extraData.sOdometerExt, &pParam[iLen - 8],sizeof(pInfo->CardSaleInfo.extraData.sOdometerExt));			 
                memcpy(&pInfo->CardSaleExtraData4.sOdometerExt2, pParam, sizeof(pInfo->CardSaleExtraData4.sOdometerExt2));			 
            }
            else if (iLen > 6)	//4.0.2.23
            {	
                int iPosition =  ((iLen - 6) == 1)? 1:0;

                memcpy(pInfo->CardSaleInfo.cardData.sOdometer,&pParam[iLen - 6] ,  sizeof(pInfo->CardSaleInfo.cardData.sOdometer));
                memcpy(&pInfo->CardSaleInfo.extraData.sOdometerExt[iPosition] ,pParam,min(sizeof(pInfo->CardSaleInfo.extraData.sOdometerExt),iLen - 6));			 
            }
            else			
                memcpy(pInfo->CardSaleInfo.cardData.sOdometer,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sOdometer),iLen));			 
        }
    }

    bFound = GetParam(pInStr,pInStr->Find("ResInfoAMax8[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleInfo.cardData.sResInfoAMax8,' ',sizeof(pInfo->CardSaleInfo.cardData.sResInfoAMax8));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sResInfoAMax8,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sResInfoAMax8),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("AuthTerminal[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleInfo.cardData.sAuthTerminal,' ',sizeof(pInfo->CardSaleInfo.cardData.sAuthTerminal));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sAuthTerminal,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sAuthTerminal),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("AuthVersion[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleInfo.cardData.sAuthVersion,' ',sizeof(pInfo->CardSaleInfo.cardData.sAuthVersion));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sAuthVersion,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sAuthVersion),iLen));
    }

    //4.0.2.15 added 5th parameter to make sure we find PIN[S] and not
    //ExtendPIN[S]
    bFound = GetParam(pInStr,pInStr->Find("PIN[S]"),&pParam,&iLen , "PIN[S]");
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.cardData.sPINBlock, ' ', sizeof(pInfo->CardSaleInfo.cardData.sPINBlock));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sPINBlock,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sPINBlock),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("ReturnTank[S]"),&pParam,&iLen);
    if(bFound) 
    {
        if(iLen)
            pInfo->CardSaleInfo.cardData.sReturnTank = *pParam;			
        else
            pInfo->CardSaleInfo.cardData.sReturnTank = ' ';
    }

    bFound = GetParam(pInStr,pInStr->Find("TransBatch[I]"),&pParam,&iLen);
    if(bFound) 
    {
        if(iLen)
        {
            long lTmp = a2l((BYTE *)pParam,iLen); 
            memset(pInfo->CardSaleInfo.cardData.sTransBatch,' ',sizeof(pInfo->CardSaleInfo.cardData.sTransBatch));
            l2a(lTmp,pInfo->CardSaleInfo.cardData.sTransBatch,sizeof(pInfo->CardSaleInfo.cardData.sTransBatch));
        }
    }

    bFound = GetParam(pInStr,pInStr->Find("TransSequence[I]"),&pParam,&iLen);
    if(bFound) 
    {
        if(iLen)
        {
            long lTmp = a2l((BYTE *)pParam,iLen); 
            memset(pInfo->CardSaleInfo.cardData.sTransBatchSequence,' ',sizeof(pInfo->CardSaleInfo.cardData.sTransBatchSequence));
            l2a(lTmp,pInfo->CardSaleInfo.cardData.sTransBatchSequence,sizeof(pInfo->CardSaleInfo.cardData.sTransBatchSequence));
        }
    }

    bFound = GetParam(pInStr,pInStr->Find("AuthorizationDate[D]"),&pParam,&iLen); // MMDDYYYY
    if(bFound) 
    {
        //sDateTime format is YYMMDDhhmm
        memset(pInfo->CardSaleInfo.cardData.sDateTime, ' ', sizeof(pInfo->CardSaleInfo.cardData.sDateTime)-4);
        if( iLen >= 2 )
        {
            memcpy(pInfo->CardSaleInfo.cardData.sDateTime	,pParam + (iLen-2),2);//4.0.3.36
            memcpy(pInfo->CardSaleInfo.cardData.sDateTime + 2,pParam    ,2);
            memcpy(pInfo->CardSaleInfo.cardData.sDateTime + 4,pParam + 2,2);
        }
    }

    bFound = GetParam(pInStr,pInStr->Find("AuthorizationTime[T]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.cardData.sDateTime+6, ' ', sizeof(pInfo->CardSaleInfo.cardData.sDateTime)-6);
        memset(pInfo->CardSaleInfo.extraData.sDateTimeExt, ' ', sizeof(pInfo->CardSaleInfo.extraData.sDateTimeExt)); //4.0.22.504 4.0.22.505
        
        if(iLen)  //3.2.0.74
        {
            memcpy(pInfo->CardSaleInfo.cardData.sDateTime+6,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sDateTime)-6,iLen));
            if (iLen > 4)	//4.0.22.504 4.0.22.505
                memcpy(pInfo->CardSaleInfo.extraData.sDateTimeExt , pParam + 4 , min(sizeof(pInfo->CardSaleInfo.extraData.sDateTimeExt), (iLen-4)));
        }  
    }
    
    bFound = GetParam(pInStr,pInStr->Find("TransactionFee[F]"),&pParam,&iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);

            long lVal = RoundDouble(x , 1000);
            
            l2a(lVal,pInfo->CardSaleInfo.cardData.sSurchargeAmt,min(sizeof(pInfo->CardSaleInfo.cardData.sSurchargeAmt),iLen));
        }
        else
        {
            l2a(0L,pInfo->CardSaleInfo.cardData.sSurchargeAmt,sizeof(pInfo->CardSaleInfo.cardData.sSurchargeAmt));
        }		
    }

    long lAmountLimit = 0;
    ParseAmountLimit(pInStr, pParam,pInfo);

	// RFUEL-4328 - PAP- Tender discount is not applying on the transaction
	// We don't want to accept this value from the backend if OLA is OLASRV_EPSILON_DISCOUNT and
	// in SESSION_COMPLETE as EPSILON passes incorrect value back to the PumpSrv
	const bool bDontParseGRDiscountPerGallon =
		(SESSION_TYPE(byOlaStat) == SESSION_COMPLETE) &&
		(_Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT);

	if (bDontParseGRDiscountPerGallon)
	{
		_LOGMSG.LogMsg("COLABase - Skipping GRDiscountPerGallon[F] Parsing");
	}
	else
	{
		bFound = GetParam(pInStr, pInStr->Find("GRDiscountPerGallon[F]"), &pParam, &iLen);
		if (bFound)
		{
			DWORD dwDiscountByVolume = 0;

			if (iLen)
			{
				double x;
				x = atof(pParam);

				DWORD dwVal = RoundDouble(x, 1000);			//4.0.3.61

				dwDiscountByVolume = ((dwDiscountByVolume < dwVal) ? dwVal : dwDiscountByVolume);
			}
			else
			{
				dwDiscountByVolume = 0;
			}

			l2a0(dwDiscountByVolume, pInfo->CardSaleInfo.cardData.sGRDiscountPerGallon, sizeof(pInfo->CardSaleInfo.cardData.sGRDiscountPerGallon));//4.0.0.31
			if (LOG_FCC_FLOW_CONTROL)	//4.0.23.500 75234
			{
				CString str;
				str.Format(">>> GRDiscountPerGallon=%d  ", dwDiscountByVolume);
				_LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
			}
		}
	}

	
		
	




    bFound = GetParam(pInStr,pInStr->Find("PercentOffDiscount[F]"), &pParam,&iLen); //4.0.23.20 75234
    if(bFound) 
    {
        DWORD dwDiscountByPercent = 0;

        if(iLen)
        {
            double x;
            x = atof(pParam);  

            DWORD dwVal = RoundDouble(x, 100);			

            dwDiscountByPercent = ((dwDiscountByPercent < dwVal) ? dwVal : dwDiscountByPercent);
        }

        l2a0(dwDiscountByPercent,pInfo->CardSaleExtraData5.sPercentOffDiscount, sizeof(pInfo->CardSaleExtraData5.sPercentOffDiscount));
        if(LOG_FCC_FLOW_CONTROL)	//4.0.23.500 75234
        {
            CString str;
            str.Format(">>> PercentOffDiscount=%d  ", dwDiscountByPercent);
            _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
        }
    }
    
    BOOL bCheckTypeAccordingToDiscount = FALSE;
    long lDiscountType = DISCOUNT_NONE;

    bFound = GetParam(pInStr, pInStr->Find("GRDiscountType[I]"), &pParam,&iLen); //4.0.23.20 75234
    if(bFound) 
    {
        memset(&(pInfo->CardSaleExtraData2.sGRDiscountType), ' ', sizeof(pInfo->CardSaleExtraData2.sGRDiscountType));
        if(iLen)
        {
            long lTmp = a2l((BYTE *) pParam,iLen); 
            l2a(lTmp, &(pInfo->CardSaleExtraData2.sGRDiscountType), sizeof(pInfo->CardSaleExtraData2.sGRDiscountType));
            if ((lTmp < 1) || (lTmp > 5)) //4.0.23.20 75234
            {
                bCheckTypeAccordingToDiscount = TRUE;
            }
        }
    }
    else 
    {
        lDiscountType = a2l(&(pInfo->CardSaleExtraData2.sGRDiscountType), sizeof(pInfo->CardSaleExtraData2.sGRDiscountType)); //4.0.27.20 TD 219520
        if (lDiscountType == DISCOUNT_NONE) //4.0.27.20
            bCheckTypeAccordingToDiscount = TRUE; 

    }
    
    if (bCheckTypeAccordingToDiscount)	//4.0.23.20 75234
    {
        long lType = DISCOUNT_NONE;
        if (a2l(pInfo->CardSaleInfo.cardData.sGRDiscountPerGallon, sizeof(pInfo->CardSaleInfo.cardData.sGRDiscountPerGallon)) > 0)
        {
            lType = DISCOUNT_CENTS_PG;
        }
        else if (a2l(pInfo->CardSaleExtraData5.sPercentOffDiscount, sizeof(pInfo->CardSaleExtraData5.sPercentOffDiscount)) > 0)
        {
            lType = DISCOUNT_PERCENT_PG;
        }
        l2a(lType, &(pInfo->CardSaleExtraData2.sGRDiscountType), sizeof(pInfo->CardSaleExtraData2.sGRDiscountType));
    }

    bFound = GetParam(pInStr, pInStr->Find("GRMaxVolume[F]"), &pParam,&iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);

            long lVal = RoundDouble(x, 1000);			//4.0.3.61			

            l2a(lVal, pInfo->CardSaleInfo.cardData.sGRMaxVolume, min(sizeof(pInfo->CardSaleInfo.cardData.sGRMaxVolume),iLen));
        }
        else
        {
            l2a(0L, pInfo->CardSaleInfo.cardData.sGRMaxVolume, sizeof(pInfo->CardSaleInfo.cardData.sGRMaxVolume));
        }		
    }

    bFound = GetParam(pInStr, pInStr->Find("ScripUnitPrice[F]"), &pParam,&iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);
            
            long lVal = RoundDouble(x, 100000);

            l2a(lVal, pInfo->CardSaleExtraData2.sScripUnitPrice, min(sizeof(pInfo->CardSaleExtraData2.sScripUnitPrice),iLen));
        }
        else
        {
            l2a(0L, pInfo->CardSaleExtraData2.sScripUnitPrice, sizeof(pInfo->CardSaleExtraData2.sScripUnitPrice));
        }		
    }

    bFound = GetParam(pInStr, pInStr->Find("MarginUnitPrice[F]"), &pParam,&iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);
            long lVal = RoundDouble(x, 100000);

            l2a(lVal, pInfo->CardSaleExtraData2.sMarginUnitPrice, min(sizeof(pInfo->CardSaleExtraData2.sMarginUnitPrice),iLen));
        }
        else
        {
            l2a(0L, pInfo->CardSaleExtraData2.sMarginUnitPrice, sizeof(pInfo->CardSaleExtraData2.sMarginUnitPrice));
        }		
    }

    bFound = GetParam(pInStr, pInStr->Find("TotalScripAmount[F]"), &pParam,&iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);

            long lVal = RoundDouble(x , 1000);

            l2a(lVal,pInfo->CardSaleExtraData2.sTotalScripAmount, min(sizeof(pInfo->CardSaleExtraData2.sTotalScripAmount), iLen));
        }
        else
        {
            l2a(0L, pInfo->CardSaleExtraData2.sTotalScripAmount, sizeof(pInfo->CardSaleExtraData2.sTotalScripAmount));
        }		
    }

     // New husky parameter
    bFound = GetParam(pInStr,pInStr->Find("Unit[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sUnit, ' ', sizeof(pInfo->CardSaleExtraData2.sUnit));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sUnit, pParam, min(sizeof(pInfo->CardSaleExtraData2.sUnit),iLen));
    }

    //start  husky new promte
    bFound = GetParam(pInStr,pInStr->Find("TripNumber[S]"), &pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sTripNumber, ' ', sizeof(pInfo->CardSaleExtraData2.sTripNumber));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sTripNumber, pParam, min(sizeof(pInfo->CardSaleExtraData2.sTripNumber),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("CustomerReference[S]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sCustomerReference, ' ', sizeof(pInfo->CardSaleExtraData2.sCustomerReference));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sCustomerReference,pParam,min(sizeof(pInfo->CardSaleExtraData2.sCustomerReference),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TractorHub[S]"), &pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sTractorHub, ' ', sizeof(pInfo->CardSaleExtraData2.sTractorHub));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sTractorHub, pParam, min(sizeof(pInfo->CardSaleExtraData2.sTractorHub), iLen));
    }

    bFound = GetParam(pInStr, pInStr->Find("TrailerNumber[S]"), &pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sTrailerNumber, ' ', sizeof(pInfo->CardSaleExtraData2.sTrailerNumber));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sTrailerNumber, pParam, min(sizeof(pInfo->CardSaleExtraData2.sTrailerNumber), iLen));
    }

    bFound = GetParam(pInStr, pInStr->Find("TrailerHub[S]"), &pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sTrailerHub, ' ', sizeof(pInfo->CardSaleExtraData2.sTrailerHub));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sTrailerHub, pParam, min(sizeof(pInfo->CardSaleExtraData2.sTrailerHub), iLen));
    }

    // end  new promte

    //4.0.20.650 71868 start //4.0.22.511
    bFound = GetParam(pInStr,pInStr->Find("HashedData[S]"), &pParam, &iLen); 
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData5.sHashedData, ' ', sizeof(pInfo->CardSaleExtraData5.sHashedData));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.sHashedData, pParam, min(sizeof(pInfo->CardSaleExtraData5.sHashedData), iLen));
    }

    bFound = GetParam(pInStr, pInStr->Find("EncryptData[S]"), &pParam,&iLen); 
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData5.sEncryptData, ' ', sizeof(pInfo->CardSaleExtraData5.sEncryptData));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.sEncryptData, pParam, min(sizeof(pInfo->CardSaleExtraData5.sEncryptData), iLen));
    }
    //4.0.20.650 71868 end


    //4.0.27.43 - Egift start
    bFound = GetParam(pInStr, pInStr->Find("CompleteRemainingBalance[I]"), &pParam, &iLen); //4.0.27.44
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData5.sCompleteRemainingBalance,' ', sizeof(pInfo->CardSaleExtraData5.sCompleteRemainingBalance));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.sCompleteRemainingBalance, pParam, min(sizeof(pInfo->CardSaleExtraData5.sCompleteRemainingBalance),iLen));
    }
    bFound = GetParam(pInStr,pInStr->Find("OverFillAmount[I]"), &pParam, &iLen); 
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData5.sOverFillAmount, ' ', sizeof(pInfo->CardSaleExtraData5.sOverFillAmount)); //4.0.27.44 
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.sOverFillAmount, pParam, min(sizeof(pInfo->CardSaleExtraData5.sOverFillAmount),iLen));
    
    }
    bFound = GetParam(pInStr, pInStr->Find("CompleteResponseCode[I]"), &pParam, &iLen); 
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData5.sCompleteResponseCode, ' ', sizeof(pInfo->CardSaleExtraData5.sCompleteResponseCode));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.sCompleteResponseCode, pParam, min(sizeof(pInfo->CardSaleExtraData5.sCompleteResponseCode), iLen));
    }
    bFound = GetParam(pInStr, pInStr->Find("PreAuthResponseCode[I]"), &pParam, &iLen); //4.0.27.44 
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData5.sPreAuthResponseCode,' ', sizeof(pInfo->CardSaleExtraData5.sPreAuthResponseCode));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.sPreAuthResponseCode, pParam, min(sizeof(pInfo->CardSaleExtraData5.sPreAuthResponseCode), iLen));
    }

    //reCalc tax amount in case there is overfill:
    _Module.m_server.UpdateTaxAmountWithoutOverfill(pInfo); //TD 231498 4.0.27.500

    //4.0.27.43 - Egift end

    //4.0.27.43 - Arval
    bFound = GetParam(pInStr, pInStr->Find("ValidationResult[I]"), &pParam,&iLen); 
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData5.sValidationResult, ' ', sizeof(pInfo->CardSaleExtraData5.sValidationResult));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.sValidationResult, pParam, min(sizeof(pInfo->CardSaleExtraData5.sValidationResult),iLen));
    }
    //4.0.27.43 - Arval
    bFound = GetParam(pInStr,pInStr->Find("OlaRequestAmount[F]"), &pParam, &iLen); 
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData5.sOlaRequestAmount, ' ', sizeof(pInfo->CardSaleExtraData5.sOlaRequestAmount));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.sOlaRequestAmount, pParam, min(sizeof(pInfo->CardSaleExtraData5.sOlaRequestAmount),iLen));
    }

    
/////////////////////////////////////////////////////////////////////////////////////////
    pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_LOYALTY_NON;
    // Set buffer promte to empty
    //pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_LOYALTY_NON;

    bFound = GetParam(pInStr, pInStr->Find("DeclinedLoyaltyInvalid[B]"), &pParam, &iLen);
    if(bFound) 
    {
        pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_LOYALTY_INVALID;
    }

    bFound = GetParam(pInStr, pInStr->Find("DeclinedLoyaltyExpired[B]"), &pParam, &iLen);
    if(bFound) 
    {
        pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_LOYALTY_EXPIRED;
    }

    bFound = GetParam(pInStr, pInStr->Find("DeclinedLoyaltyWithPayCard[B]"), &pParam, &iLen);
    if(bFound) 
    {
        pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_LOYALTY_WITH_PAYCARD;
    }

    //4.0.0.86
    bFound = GetParam(pInStr, pInStr->Find("DeclinePaymentRetry[B]"), &pParam, &iLen);
    if(bFound) 
    {
        pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_PAYMENT_CARD;
    }

    //4.0.5.21
    bFound = GetParam(pInStr, pInStr->Find("TotalRedeemedPoints[I]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sTotalRedeemedPoints, ' ', sizeof(pInfo->CardSaleExtraData3.sTotalRedeemedPoints));
        if(iLen)
        {
            long lTmp = a2l((BYTE *)pParam, iLen); 
            l2a(lTmp, pInfo->CardSaleExtraData3.sTotalRedeemedPoints, sizeof(pInfo->CardSaleExtraData3.sTotalRedeemedPoints));			
        }
    }	

    bFound = GetParam(pInStr, pInStr->Find("AmountByPoints[F]"), &pParam, &iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);
            long lVal = RoundDouble(x, 100);
            l2a(lVal,
                pInfo->CardSaleExtraData3.sAmountByPoints,
                min(sizeof(pInfo->CardSaleExtraData3.sAmountByPoints),iLen));
        }
        else
        {
            l2a(0L, pInfo->CardSaleExtraData3.sAmountByPoints, sizeof(pInfo->CardSaleExtraData3.sAmountByPoints));
        }		
    }

    //4.0.14.503
    bFound = GetParam(pInStr, pInStr->Find("AmountByPoints2[F]"), &pParam, &iLen); //4.0.15.501
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);
            long lVal = RoundDouble(x, 100);
            l2a(lVal,
                pInfo->CardSaleExtraData3.sAmountByPoints2,
                min(sizeof(pInfo->CardSaleExtraData3.sAmountByPoints2),iLen));
        }
        else
        {
            l2a(0L, pInfo->CardSaleExtraData3.sAmountByPoints2, sizeof(pInfo->CardSaleExtraData3.sAmountByPoints2));
        }		
    }

    //4.0.14.503
    bFound = GetParam(pInStr,pInStr->Find("CreditAmount[F]"), &pParam, &iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);
            long lVal = RoundDouble(x, 100);
            l2a(lVal, pInfo->CardSaleExtraData3.sCreditAmount, min(sizeof(pInfo->CardSaleExtraData3.sCreditAmount), iLen));
        }
        else
        {
            l2a(0L, pInfo->CardSaleExtraData3.sCreditAmount, sizeof(pInfo->CardSaleExtraData3.sCreditAmount));
        }		
    }

#ifdef DEBUG
	str.Format(">>> sDeclinedLoyaltyPrompt=%c  sDeclinedLoyaltyResponse=%c", pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt, pInfo->CardSaleExtraData2.sDeclinedLoyaltyResponse);
	_LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
#endif // DEBUG

/////////////////////////////////////////////////////////////////////////////////////////////
    bFound = GetParam(pInStr,pInStr->Find("CustomerMessage[S]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sCustomerMessage, ' ', sizeof(pInfo->CardSaleExtraData2.sCustomerMessage));

        if(iLen)
        {
            memcpy((char *) pInfo->CardSaleExtraData2.sCustomerMessage, pParam, min(sizeof(pInfo->CardSaleExtraData2.sCustomerMessage), iLen));
            i2a(iLen, pInfo->CardSaleExtraData2.sTextLength, sizeof(pInfo->CardSaleExtraData2.sTextLength));
        }
    }

    bFound = GetParam(pInStr, pInStr->Find("CouponMessage[S]"), &pParam, &iLen);
    if(bFound) 
    {

        memset(pInfo->CardSaleExtraData2.sCouponMessage , ' ' , sizeof(pInfo->CardSaleExtraData2.sCouponMessage));

        if(iLen)
            memcpy((char *) pInfo->CardSaleExtraData2.sCouponMessage, pParam, min(sizeof(pInfo->CardSaleExtraData2.sCouponMessage), iLen));
    }		
    
    // 3.2.0.69
    bFound = GetParam(pInStr, pInStr->Find("InstantWinMessage[S]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sReserveForCoupon, ' ', sizeof(pInfo->CardSaleExtraData2.sReserveForCoupon));

        if(iLen)
            memcpy((char *) pInfo->CardSaleExtraData2.sReserveForCoupon, pParam, min(sizeof(pInfo->CardSaleExtraData2.sReserveForCoupon), iLen));
    }

    bFound = GetParam(pInStr, pInStr->Find("CouponId[S]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sCouponId, ' ', sizeof(pInfo->CardSaleExtraData2.sCouponId));

        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sCouponId, pParam, min(sizeof(pInfo->CardSaleExtraData2.sCouponId), iLen));
    }

    //3.2.0.69
    bFound = GetParam(pInStr, pInStr->Find("InstantWinSerialNum[S]"), &pParam, &iLen);
    if(bFound) 
    {

        memset(pInfo->CardSaleExtraData2.sCouponId, ' ', sizeof(pInfo->CardSaleExtraData2.sCouponId));

        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sCouponId, pParam, min(sizeof(pInfo->CardSaleExtraData2.sCouponId), iLen));
    }

    bFound = GetParam(pInStr, pInStr->Find("LoyaltyProgId[S]"), &pParam, &iLen);
    if(bFound) 
    {

        memset(pInfo->CardSaleExtraData2.sLoyaltyProgName, ' ', sizeof(pInfo->CardSaleExtraData2.sLoyaltyProgName));

        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sLoyaltyProgName, pParam, min(sizeof(pInfo->CardSaleExtraData2.sLoyaltyProgName), iLen));
    }

    bFound = GetParam(pInStr, pInStr->Find("PassCode[S]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sPassCode, ' ', sizeof(pInfo->CardSaleExtraData2.sPassCode));

        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sPassCode, pParam, min(sizeof(pInfo->CardSaleExtraData2.sPassCode), iLen));
    }

    //End  husky 

    bFound = GetParam(pInStr, pInStr->Find("ReceiptType[I]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData2.sReceiptType, ' ', sizeof(pInfo->CardSaleExtraData2.sReceiptType));
        if(iLen)
        {
            long lTmp = a2l((BYTE *) pParam, iLen); 
            l2a(lTmp, pInfo->CardSaleExtraData2.sReceiptType, sizeof(pInfo->CardSaleExtraData2.sReceiptType));
        }
    }

    bFound = GetParam(pInStr, pInStr->Find("DeviceNumber[S]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleInfo.cardData.sDeviceNumber, ' ', sizeof(pInfo->CardSaleInfo.cardData.sDeviceNumber));

        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sDeviceNumber, pParam, min(sizeof(pInfo->CardSaleInfo.cardData.sDeviceNumber), iLen));
    }

    bFound = GetParam(pInStr, pInStr->Find("OriginTrans[S]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(&(pInfo->CardSaleExtraData2.sOriginTrans), ' ', sizeof(pInfo->CardSaleExtraData2.sOriginTrans));

        if(iLen)
            memcpy(&(pInfo->CardSaleExtraData2.sOriginTrans), pParam, min(sizeof(pInfo->CardSaleExtraData2.sOriginTrans), iLen));
    }

    //3.0.3.11
    bFound = GetParam(pInStr,pInStr->Find("AutoAuthorize[S]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(&(pInfo->CardSaleExtraData2.sAutoAuthorize), ' ', sizeof(pInfo->CardSaleExtraData2.sAutoAuthorize));

        if(iLen)
            memcpy(&(pInfo->CardSaleExtraData2.sAutoAuthorize), pParam, min(sizeof(pInfo->CardSaleExtraData2.sAutoAuthorize), iLen));
    }

    //3.0.4.1
    bFound = GetParam(pInStr, pInStr->Find("WaitForReportFlag[B]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData2.sWaitForReportFlag, ' ', sizeof(pInfo->CardSaleExtraData2.sWaitForReportFlag));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData2.sWaitForReportFlag, pParam, min(sizeof(pInfo->CardSaleExtraData2.sWaitForReportFlag), iLen));			
    }

    //3.0.9.21 
    bFound = GetParam(pInStr, pInStr->Find("AuthorizerPOSSerialNumber[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData2.sAuthorizationSerialNumber, ' ', sizeof(pInfo->CardSaleExtraData2.sAuthorizationSerialNumber));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData2.sAuthorizationSerialNumber, pParam, min(sizeof(pInfo->CardSaleExtraData2.sAuthorizationSerialNumber), iLen));			
    }

    bFound = GetParam(pInStr, pInStr->Find("AuthorizerFileNumebr[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData2.sAuthorizerFileNumber, ' ', sizeof(pInfo->CardSaleExtraData2.sAuthorizerFileNumber));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData2.sAuthorizerFileNumber, pParam, min(sizeof(pInfo->CardSaleExtraData2.sAuthorizerFileNumber), iLen));			
    }

    ///////////Marik12345///////////////////////////////////////////
    // search for one occur of the discount, if found then try to parse the data 
    bFound = GetParam(pInStr, pInStr->Find("GradeDiscount"), &pParam, &iLen); //3.1.1.5

    if(bFound)
    {
        memset(pInfo->CardSaleExtraData2.sDiscountPerGrade, ' ', sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade));
        long lGradePos = 0; //4.0.10.504
        long lPumpNumber =  a2l(pInfo->CardSaleInfo.cardData.sPumpNumber,sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));//4.0.10.504
        for(int i = 0; i < MAX_GRADES; i++)
        {	
            CString sGradeDiscount;
            sGradeDiscount.Format("GradeDiscount%02d[F]=", i + 1);  
            bFound = GetParam(pInStr, pInStr->Find(sGradeDiscount), &pParam, &iLen); 
            if((bFound) && iLen)
            {
                long lDiscount = RoundDouble(atof(pParam), 1000);
				lGradePos = _Module.m_server.GetGradePositionRestrection(i + 1, lPumpNumber); //4.0.23.1360 //RFUEL-2055

                //4.0.23.1360 TD 316024 start
                if  ((lGradePos > 0)  && (lGradePos <= MAX_GRADES))
                {
                    lGradePos -= 1; 
                    l2a0(lDiscount,pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount,min(sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[lGradePos].sDiscount),iLen));//4.0.10.504//3.1.1.65

                    if(LOG_DETAIL_FLOW_CONTROL)
                    {
                        
                        CString str;
                            str.Format("GradeDiscount for Grade %d, Total = %ld, GradePos = %ld", i + 1, lDiscount, lGradePos);
                        _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
                    }
                }
                else
                {
                    if(LOG_DETAIL_FLOW_CONTROL)
                    {
                        CString str;
                        str.Format("Not Valid position Grade = %d, Total = %ld", i + 1);
                        _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
                    }
                }
                //4.0.23.1360 TD 316024 end

				///////////////////////////
				CString GradeDiscountId;
				GradeDiscountId.Format("GradeDiscountId%02d[S]=", i + 1);
				bFound = GetParam(pInStr, pInStr->Find(GradeDiscountId), &pParam, &iLen);

				if ((bFound) && iLen)
				{
					memset(pInfo->CardSaleExtraData6.sGradeDiscountID[lGradePos], ' ', sizeof(pInfo->CardSaleExtraData6.sGradeDiscountID[lGradePos]));
					memcpy(pInfo->CardSaleExtraData6.sGradeDiscountID[lGradePos], pParam, min(sizeof(pInfo->CardSaleExtraData6.sGradeDiscountID[lGradePos]), iLen));

					if (LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("ParseData() - GradeDiscountId = %.15s, Position = %ld, value = %d", pInfo->CardSaleExtraData6.sGradeDiscountID[lGradePos], lGradePos + 1,lDiscount);
						_Module.m_server.m_cLogger.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
					}
				}

				//pInfo->CardSaleExtraData3.sGradeAmountLimit[i] , sizeof(pInfo->CardSaleExtraData3.sGradeAmountLimit[i]));														

				/////////////////////////
            }		
        }
    }

    //3.1.1.65
    bFound = GetParam(pInStr, pInStr->Find("PromotionNumber[S]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(&pInfo->CardSaleExtraData2.sPromotionNumber, ' ', sizeof(pInfo->CardSaleExtraData2.sPromotionNumber));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData2.sPromotionNumber, pParam, min(sizeof(pInfo->CardSaleExtraData2.sPromotionNumber), iLen));			
    }

    //4.0.14.503
    bFound = GetParam(pInStr, pInStr->Find("PromotionNumber2[S]"), &pParam, &iLen);
    if(bFound) 
    {
        memset(&pInfo->CardSaleExtraData3.sPromotionNumber2, ' ', sizeof(pInfo->CardSaleExtraData3.sPromotionNumber2));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData3.sPromotionNumber2, pParam, min(sizeof(pInfo->CardSaleExtraData3.sPromotionNumber2 ),iLen));			
    }

    //3.1.1.66
    bFound = GetParam(pInStr, pInStr->Find("SignatureSlip[B]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData2.sSignatureSlip, ' ', sizeof(pInfo->CardSaleExtraData2.sSignatureSlip));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData2.sSignatureSlip, pParam, min(sizeof(pInfo->CardSaleExtraData2.sSignatureSlip), iLen));			

        //3.2.0.11
        switch(pInfo->CardSaleExtraData2.sSignatureSlip)
        {
        case 'Y':
                pInfo->CardSaleExtraData2.sSignatureSlip = '1';
            break;
        case 'N':
                pInfo->CardSaleExtraData2.sSignatureSlip = '0';
            break;
        }
    }
    
     //1 = cash, 2 = credit
    bFound = GetParam(pInStr, pInStr->Find("PriceLevel[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData2.sPriceLevel, ' ', sizeof(pInfo->CardSaleExtraData2.sPriceLevel));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData2.sPriceLevel, pParam, min(sizeof(pInfo->CardSaleExtraData2.sPriceLevel), iLen));			
    }

    //3.2.0.51
    bFound = GetParam(pInStr, pInStr->Find("ReceiptMessage[S]"), &pParam, &iLen);
    if(bFound) 
    {			
        memset(pInfo->CardSaleExtraData2.sReserveForCoupon, ' ', sizeof(pInfo->CardSaleExtraData2.sReserveForCoupon));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sReserveForCoupon, pParam, min(sizeof(pInfo->CardSaleExtraData2.sReserveForCoupon), iLen));
    }

	ParseZoneFromOlaBuffer(lPumpNumber, pInStr, pParam, iLen, byOlaStat, pInfo);  // 4.0.23.266?

   // 3.2.0.64
    bFound = GetParam(pInStr, pInStr->Find("ForceCompletion[B]"), &pParam, &iLen);
    if(bFound) 
    {		
        memset(&pInfo->CardSaleInfo.extraData.sForceCompleteZero, ' ', sizeof(pInfo->CardSaleInfo.extraData.sForceCompleteZero));
        if(iLen)
            memcpy(&pInfo->CardSaleInfo.extraData.sForceCompleteZero, pParam, min(sizeof(pInfo->CardSaleInfo.extraData.sForceCompleteZero), iLen));			
        
        if ((pInfo->CardSaleInfo.extraData.sForceCompleteZero == 'Y') && (pInfo->CardSaleInfo.cardData.sTranStatus != CD_APPROVE_OK)) //4.0.22.506 75407
        {
            pInfo->CardSaleInfo.cardData.sTranStatus = CD_FORCE_COMPLETION_ZERO;
        }
    }

    //4.0.14.1390 67387 //4.0.21.0
    bFound = GetParam(pInStr, pInStr->Find("ForceZeroPAKCompletion[B]"), &pParam, &iLen);
    if(bFound) 
    {		
        memset(&pInfo->CardSaleExtraData4.sForceZeroPAKCompletion, ' ', sizeof(pInfo->CardSaleExtraData4.sForceZeroPAKCompletion));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData4.sForceZeroPAKCompletion, pParam, min(sizeof(pInfo->CardSaleExtraData4.sForceZeroPAKCompletion), iLen));			
    }
    
    //4.0.21.10 64119
    bFound = GetParam(pInStr, pInStr->Find("CancelAborts[B]"), &pParam, &iLen);
    if(bFound) 
    {		
        memset(&pInfo->CardSaleExtraData4.sCancelAborts, ' ', sizeof(pInfo->CardSaleExtraData4.sCancelAborts));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData4.sCancelAborts, pParam, min(sizeof(pInfo->CardSaleExtraData4.sCancelAborts), iLen));			
    }

    //4.0.22.506 67786
    bFound = GetParam(pInStr, pInStr->Find("PapToPak[B]"), &pParam, &iLen);
    if(bFound) 
    {		
        if(iLen)
        {
            if (pParam[0] == '1')
            {
                long lTmp = a2l(pInfo->CardSaleExtraData3.sFlags1, sizeof(pInfo->CardSaleExtraData3.sFlags1)) + FL_PAP_TO_PAK;
                l2a(lTmp, pInfo->CardSaleExtraData3.sFlags1, sizeof(pInfo->CardSaleExtraData3.sFlags1));
            }
        }
    }
    

    //3.2.0.76
    bFound = GetParam(pInStr, pInStr->Find("IgnoreServiceFee[B]"), &pParam, &iLen);
    if(bFound) 
    {		
        pInfo->CardSaleExtraData2.sIgnoreServiceFee = ' ';		
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData2.sIgnoreServiceFee, pParam, min(sizeof(pInfo->CardSaleExtraData2.sIgnoreServiceFee), iLen));					
    }
    
	if ((SESSION_TYPE(byOlaStat) != SESSION_GET_CUSTOMER_INFO) && 
		(SESSION_TYPE(byOlaStat) != SESSION_SET_CUSTOMER_INFO))	// RFUEL-633
	{
		//3.2.1.51
		bFound = GetParam(pInStr, pInStr->Find("MediaType[S]"), &pParam, &iLen);
		if (bFound)
		{
			pInfo->CardSaleInfo.cardData.sTranType = ' ';
			if (iLen)
				memcpy(&pInfo->CardSaleInfo.cardData.sTranType, pParam, 1);
		}
	}

    //3.2.0.69
    bFound = GetParam(pInStr, pInStr->Find("VolumeLimit[F]"), &pParam, &iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);

            long lVal = RoundDouble(x, 1000);

            l2a(lVal, pInfo->CardSaleExtraData2.sVolumeLimit, sizeof(pInfo->CardSaleExtraData2.sVolumeLimit));//3.1.1.65	// 4.0.3.21

            long lMediaMasterType = _Module.m_server.m_cFlowControlManager.GetMediaMasterType(lPumpNumber);

            if ((lMediaMasterType == CARD_TYPE_LOYALTY)  ||     //2.5.33.320
				  (SESSION_TYPE(byOlaStat) == SESSION_GET_CUSTOMER_INFO))
			{
                l2a(lVal, pInfo->CardSaleExtraData4.sLoyaltyVolumeLimit, sizeof(pInfo->CardSaleExtraData4.sLoyaltyVolumeLimit));
                l2a(1, &(pInfo->CardSaleExtraData4.sExistLoyaltyLimit), sizeof(pInfo->CardSaleExtraData4.sExistLoyaltyLimit));
            }
        }
        else
        {
            l2a(0L, pInfo->CardSaleExtraData2.sVolumeLimit, sizeof(pInfo->CardSaleExtraData2.sVolumeLimit));
        }		
    }

    //4.0.0.66
    bFound = GetParam(pInStr, pInStr->Find("TimeOut[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData2.sMessageTimeOut, ' ',sizeof(pInfo->CardSaleExtraData2.sMessageTimeOut));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData2.sMessageTimeOut, pParam, min(sizeof(pInfo->CardSaleExtraData2.sMessageTimeOut), iLen));			
        
    }

    //5.0.0.55
    bFound = GetParam(pInStr, pInStr->Find("ClockSrvTime[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData2.sClockDateTime, ' ', sizeof(pInfo->CardSaleExtraData2.sClockDateTime));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sClockDateTime, pParam, min(sizeof(pInfo->CardSaleExtraData2.sClockDateTime), iLen));
    }

    //4.0.0.63
    bFound = GetParam(pInStr, pInStr->Find("EmployeeName[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData2.sAttendantName, ' ', sizeof(pInfo->CardSaleExtraData2.sAttendantName));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sAttendantName, pParam, min(sizeof(pInfo->CardSaleExtraData2.sAttendantName), iLen));
    }

    bFound = GetParam(pInStr, pInStr->Find("EmployeeId[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData2.sAttendantID, ' ', sizeof(pInfo->CardSaleExtraData2.sAttendantID));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sAttendantID, pParam, min(sizeof(pInfo->CardSaleExtraData2.sAttendantID), iLen));
    }

    //4.0.25.501 TD 137509
    bFound = GetParam(pInStr, pInStr->Find("EmployeeCard[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData5.sAttendantTag, ' ', sizeof(pInfo->CardSaleExtraData5.sAttendantTag));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.sAttendantTag, pParam, min(sizeof(pInfo->CardSaleExtraData5.sAttendantTag), iLen));
    }

    //4.0.26.501 TD 143919
    bFound = GetParam(pInStr, pInStr->Find("OfflineForceAcc[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData5.sOfflineForceAcc, ' ', sizeof(pInfo->CardSaleExtraData5.sOfflineForceAcc));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.sOfflineForceAcc, pParam, min(sizeof(pInfo->CardSaleExtraData5.sOfflineForceAcc), iLen));
    }
    //no TD 4.0.26.507
    bFound = GetParam(pInStr, pInStr->Find("RestrictPoints[B]"), &pParam, &iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData5.sRestrictPoints = ' ';
        if(iLen)
            pInfo->CardSaleExtraData5.sRestrictPoints = pParam[0];
    }
    
    
/////////////////////////////////////////
    //4.0.1.0
    bFound = GetParam(pInStr,pInStr->Find("RemainingBalance[F]"), &pParam, &iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);

            long lVal = RoundDouble(x, 100); 			//4.0.3.61

            l2a0(lVal, pInfo->CardSaleExtraData2.sTotalBalance, sizeof(pInfo->CardSaleExtraData2.sTotalBalance));			//4.0.4.41
        }
        else
        {
            l2a0(0L, pInfo->CardSaleExtraData2.sTotalBalance, sizeof(pInfo->CardSaleExtraData2.sTotalBalance));
        }		
    }

    //4.0.1.31
    bFound = GetParam(pInStr, pInStr->Find("ZipCode[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData2.sZipCode, ' ', sizeof(pInfo->CardSaleExtraData2.sZipCode));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sZipCode, pParam, min(sizeof(pInfo->CardSaleExtraData2.sZipCode), iLen));
    }

    //46512
    bFound = GetParam(pInStr, pInStr->Find("ExtendedZipCode[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData4.sExtendedZipCode, ' ', sizeof(pInfo->CardSaleExtraData4.sExtendedZipCode));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.sExtendedZipCode, pParam, min(sizeof(pInfo->CardSaleExtraData4.sExtendedZipCode), iLen));
    }

    //4.0.1.32	 4.0.1.391
    bFound = GetParam(pInStr, pInStr->Find("FormattedAcct[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData2.sFormattedAccount, ' ', sizeof(pInfo->CardSaleExtraData2.sFormattedAccount));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sFormattedAccount, pParam, min(sizeof(pInfo->CardSaleExtraData2.sFormattedAccount), iLen));
    }

    //42728 start
    bFound = GetParam(pInStr, pInStr->Find("FormattedClubCardNumber[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData4.sFormattedClubCardNumber, ' ', sizeof(pInfo->CardSaleExtraData4.sFormattedClubCardNumber));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.sFormattedClubCardNumber, pParam, min(sizeof(pInfo->CardSaleExtraData4.sFormattedClubCardNumber), iLen));
    }


	const bool bDontParseLoyaltyEntryMethod =
		(SESSION_TYPE(byOlaStat) == SESSION_COMPLETE) &&
		(_Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT);

	if (bDontParseLoyaltyEntryMethod) 
	{
		CString str = "Skip Parsing LoyaltyEntryMethod[S]";
		_LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
	}
	else 
	{

		//55466 4.0.18.507
		bFound = GetParam(pInStr, pInStr->Find("LoyaltyEntryMethod[S]"), &pParam, &iLen);
		if (bFound)
		{
			memset(&pInfo->CardSaleExtraData4.sLoyaltyEntryMethod, ' ', sizeof(pInfo->CardSaleExtraData4.sLoyaltyEntryMethod));
			if (iLen)
				memcpy(&pInfo->CardSaleExtraData4.sLoyaltyEntryMethod, pParam, min(sizeof(pInfo->CardSaleExtraData4.sLoyaltyEntryMethod), iLen));
			CString str;
			str.Format("LoyaltyEntryMethod[S]=%c passed from EPS",
				(char)pInfo->CardSaleExtraData4.sLoyaltyEntryMethod);
		}
		else {
			CString str = "LoyaltyEntryMethod[S] not passed from EPS";
			_LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
		}
	}


    //59424 4.0.19.502
    bFound = GetParam(pInStr, pInStr->Find("TotalEftNetworkTime[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData4.sTotalEftNetworkTime, ' ', sizeof(pInfo->CardSaleExtraData4.sTotalEftNetworkTime));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.sTotalEftNetworkTime, pParam, min(sizeof(pInfo->CardSaleExtraData4.sTotalEftNetworkTime), iLen));
    }

    //59424 4.0.19.502
    bFound = GetParam(pInStr, pInStr->Find("CustomerEftWaitTime[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData4.sCustomerEftWaitTime, ' ', sizeof(pInfo->CardSaleExtraData4.sCustomerEftWaitTime));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.sCustomerEftWaitTime, pParam, min(sizeof(pInfo->CardSaleExtraData4.sCustomerEftWaitTime), iLen));
    }
    
    bFound = GetParam(pInStr, pInStr->Find("MediaNumber[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData4.sMediaNumber, ' ', sizeof(pInfo->CardSaleExtraData4.sMediaNumber));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.sMediaNumber, pParam, min(sizeof(pInfo->CardSaleExtraData4.sMediaNumber), iLen));
    }

    //60638 4.0.19.502
    bFound = GetParam(pInStr, pInStr->Find("HotCard[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData4.sHotCard, ' ', sizeof(pInfo->CardSaleExtraData4.sHotCard));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData4.sHotCard, pParam, min(sizeof(pInfo->CardSaleExtraData4.sHotCard), iLen));
    }

    //4.0.11.501
    long lIdx=0;
    if(CheckIfStrIsReal("MediaMasterType[I]", *pInStr, &lIdx))
    {
		if (lIdx >= 0)
		{
			bFound = GetParam(pInStr, pInStr->Find("MediaMasterType[I]", lIdx), &pParam, &iLen);
			if (bFound)
			{
				memset(pInfo->CardSaleExtraData2.sMediaMasterType, ' ', sizeof(pInfo->CardSaleExtraData2.sMediaMasterType));
				if (iLen)
					memcpy(pInfo->CardSaleExtraData2.sMediaMasterType, pParam, min(sizeof(pInfo->CardSaleExtraData2.sMediaMasterType), iLen));

				if (a2l(pInfo->CardSaleExtraData2.sMediaMasterType, sizeof(pInfo->CardSaleExtraData2.sMediaMasterType)) == CARD_TYPE_ATTENDANT)	//4.0.3.25
					pInfo->CardSaleInfo.extraData.sAttandantSession = '1';

				//Triple card
				long lMediaMasterType2 = a2l(pInfo->CardSaleExtraData2.sMediaMasterType, sizeof(pInfo->CardSaleExtraData2.sMediaMasterType));
				_Module.m_server.CheckTripleAFlow(lPumpNumber, lMediaMasterType2); // TD 327153
			}
		}
    }

	long lMediaMasterType2 = a2l(pInfo->CardSaleExtraData2.sMediaMasterType, sizeof(pInfo->CardSaleExtraData2.sMediaMasterType));
	bFound = GetParam(pInStr, pInStr->Find("AccountNumber[S]"), &pParam, &iLen);
	if (bFound && lMediaMasterType2 != CARD_TYPE_LOYALTY) // TD 476566
	{
		memset(pInfo->CardSaleInfo.cardData.sAccountNumber, ' ', sizeof(pInfo->CardSaleInfo.cardData.sAccountNumber));
		if (iLen)
			memcpy(pInfo->CardSaleInfo.cardData.sAccountNumber, pParam, min(sizeof(pInfo->CardSaleInfo.cardData.sAccountNumber), iLen));
	}
    
    //4.0.3.2.25
    bFound = GetParam(pInStr,pInStr->Find("DisplayResultMessage[I]"),&pParam,&iLen);
    if(bFound) 
    {	

        pInfo->CardSaleExtraData2.sDisplayOLAResultMessage = ' ';		
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData2.sDisplayOLAResultMessage, pParam, min(sizeof(pInfo->CardSaleExtraData2.sDisplayOLAResultMessage), iLen));
    }

    //4.0.3.26
    bFound = GetParam(pInStr, pInStr->Find("Ash2000Data[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        CAsh2000File  cAsh2000File;
        long lPumpNumber = a2l(pInfo->CardSaleInfo.cardData.sPumpNumber, sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));
                
        if(PUMP_NOT_VALID(lPumpNumber)) // 4.0.16.503
        {
            if(!PUMP_NOT_VALID(lPumpNum))
            {
                lPumpNumber = lPumpNum;
                l2a(lPumpNumber,pInfo->CardSaleInfo.cardData.sPumpNumber, sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));

                if(LOG_BASIC_FLOW_CONTROL)
                {
                    CString str;
                    str.Format("Pump number is taken from the interface (Ash2000Data[S]) Pump=%ld", lPumpNumber);
                    _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
                }
            }
        }

        if(!PUMP_NOT_VALID(lPumpNumber)) // TD 41144 - 4.0.15.72
        {
            if (!cAsh2000File.InsertData(lPumpNumber, pParam, iLen, pTrs))
            {
                long lTmp = a2l(pInfo->CardSaleExtraData2.sFlags, sizeof(pInfo->CardSaleExtraData2.sFlags));

                if (!(lTmp & FL_CONNECTED_RECORD_IN_ASH2000_FILE))
                    lTmp |= FL_CONNECTED_RECORD_IN_ASH2000_FILE;
                l2a(lTmp, pInfo->CardSaleExtraData2.sFlags, sizeof(pInfo->CardSaleExtraData2.sFlags)); 
            }
        }
        else
        {
            if(LOG_BASIC_FLOW_CONTROL) // 4.0.15.72
            {
                CString str;
                str.Format("Pump number is not valid! Pump=%ld", lPumpNumber);
                _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
            }
        }
    }	

/////////////////////////////////////////////////
    
    CheckSlipFlags(pInStr, pInfo->CardSaleInfo.extraData.sSlipFlags);	//4.0.3. 49   //4.0.3,53
    
    //4.0.3.47
    bFound = GetParam(pInStr, pInStr->Find("RedeemPoints[B]"), &pParam, &iLen);
    if(bFound) 
    {				
        if(iLen)
            pInfo->CardSaleInfo.extraData.sRedeemPoints = pParam[0];
        else
            pInfo->CardSaleInfo.extraData.sRedeemPoints = ' ';			
    }

    //4.0.3.47
    bFound = GetParam(pInStr, pInStr->Find("ConfirmPrompt[B]"), &pParam, &iLen);
    if(bFound) 
    {			
        if(iLen)
            pInfo->CardSaleInfo.extraData.sConfirmPrompt = pParam[0];
        else
            pInfo->CardSaleInfo.extraData.sConfirmPrompt = ' ';			
    }
    
    
    //4.0.5.12	
    bFound = GetParam(pInStr, pInStr->Find("ClearanceID[I]"), &pParam, &iLen);
    if(bFound) 
    {
        pInfo->CardSaleInfo.cardData.sClearanceID = ' ';
        if(iLen)
            memcpy(&pInfo->CardSaleInfo.cardData.sClearanceID, pParam, 1);

    }	

    //4.0.5.36	
    bFound = GetParam(pInStr, pInStr->Find("ReSend[B]"), &pParam, &iLen);
    if(bFound) 
    {
        pInfo->CardSaleExtraData3.sReSend = ' ';
        if(iLen)
            pInfo->CardSaleExtraData3.sReSend = pParam[0];			

    }
    
    long old_report = _Module.m_server.GetOLAReportSoldItemOnPump();
    // 4.0.5.39
    if ( ( SupportIFSFProtocoll() && (lState == OLASRV_PROCESS_FINISHED) && 
        ((SESSION_TYPE(byOlaStat) == SESSION_AUTHORIZE)|| (SESSION_TYPE(byOlaStat) == SESSION_INQUIRY)))|| //4.0.12.460
        (_Module.m_server.GetOLAReportSoldItemOnPump()))//4.0.10.502
    {
        long lPumpNumber = a2l(pInfo->CardSaleInfo.cardData.sPumpNumber, sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));  //4.0.15.501

        // 4.0.16.503
        if(PUMP_NOT_VALID(lPumpNumber)) 
        {
            if(!PUMP_NOT_VALID(lPumpNum))
            {
                lPumpNumber = lPumpNum;
                l2a(lPumpNumber, pInfo->CardSaleInfo.cardData.sPumpNumber, sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));

                if(LOG_BASIC_FLOW_CONTROL)
                {
                    CString str;
                    str.Format("Pump number is taken from the interface () Pump=%ld", lPumpNumber);
                    _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
                }
            }
        }

        if(!PUMP_NOT_VALID(lPumpNumber)) // 4.0.15.72
        {
            ParseItemProduct(lPumpNumber, pInStr, pInfo, pTrs); //4.0.9999.0
        }
        else
        {
            if(LOG_BASIC_FLOW_CONTROL) // 4.0.15.72
            {
                CString str;
                str.Format("Pump number is not valid! Pump = %ld", lPumpNumber);
                _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
            }
        }
    }	

    bFound = GetParam(pInStr, pInStr->Find("PromptType[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sPromptType, ' ', sizeof(pInfo->CardSaleExtraData3.sPromptType));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sPromptType, pParam, min(sizeof(pInfo->CardSaleExtraData3.sPromptType), iLen));
    }

    bFound = GetParam(pInStr, pInStr->Find("PromptMinLen[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sPromptMinLen, ' ', sizeof(pInfo->CardSaleExtraData3.sPromptMinLen));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sPromptMinLen, pParam, min(sizeof(pInfo->CardSaleExtraData3.sPromptMinLen), iLen));
    }

    //4.0.16.70
    bFound = GetParam(pInStr, pInStr->Find("KeyboardMask[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData3.sKeyboardMask = ' ';			

        if(iLen)
            pInfo->CardSaleExtraData3.sKeyboardMask = pParam[0];
    }

    //Liat LPE
    bFound = GetParam(pInStr,pInStr->Find("CardSegment[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData3.sCardSegment = ' ';			

        if(iLen)
            pInfo->CardSaleExtraData3.sCardSegment = pParam[0];
    }

    bFound = GetParam(pInStr, pInStr->Find("PromptMaxLen[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sPromptMaxLen, ' ', sizeof(pInfo->CardSaleExtraData3.sPromptMaxLen));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sPromptMaxLen, pParam, min(sizeof(pInfo->CardSaleExtraData3.sPromptMaxLen), iLen));
    }

    bFound = GetParam(pInStr, pInStr->Find("ParamIndex[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sParamIndex, ' ', sizeof(pInfo->CardSaleExtraData3.sParamIndex));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sParamIndex, pParam, min(sizeof(pInfo->CardSaleExtraData3.sParamIndex), iLen));
    }	

    bFound = GetParam(pInStr, pInStr->Find("ParamResult[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sParamResult,' ',sizeof(pInfo->CardSaleExtraData3.sParamResult));
        memset(pInfo->CardSaleExtraData5.sParamResultExt,' ',sizeof(pInfo->CardSaleExtraData5.sParamResultExt));	//4.0.28.502 255727

        if(iLen)
        {
            if (iLen > sizeof(pInfo->CardSaleExtraData3.sParamResult))												//4.0.28.502 255727
            {
                memcpy(pInfo->CardSaleExtraData3.sParamResult, pParam, sizeof(pInfo->CardSaleExtraData3.sParamResult));
                memcpy(pInfo->CardSaleExtraData5.sParamResultExt, &pParam[sizeof(pInfo->CardSaleExtraData3.sParamResult)], sizeof(pInfo->CardSaleExtraData5.sParamResultExt));
            }
            else
                memcpy(pInfo->CardSaleExtraData3.sParamResult, pParam, min(sizeof(pInfo->CardSaleExtraData3.sParamResult), iLen));
        }
    }

    //4.0.5.41
    bFound = GetParam(pInStr, pInStr->Find("RemoteID[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sRemoteID, ' ', sizeof(pInfo->CardSaleExtraData3.sRemoteID));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sRemoteID, pParam, min(sizeof(pInfo->CardSaleExtraData3.sRemoteID), iLen));
    }

    // 4.0.5.42
    bFound = GetParam(pInStr, pInStr->Find("LogicShiftNumber[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sLogicShiftNumber,' ',sizeof(pInfo->CardSaleExtraData3.sLogicShiftNumber));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sLogicShiftNumber, pParam, min(sizeof(pInfo->CardSaleExtraData3.sLogicShiftNumber), iLen));
    }

    //4.0.6.502
    bFound = GetParam(pInStr, pInStr->Find("OfflineTrans[B]"), &pParam, &iLen);
    if(bFound) 
    {						
        if(iLen)
            pInfo->CardSaleExtraData3.sOfflineTrans = pParam[0];
        else
            pInfo->CardSaleExtraData3.sOfflineTrans = ' ';			
    }

    //4.0.7.500
    bFound = GetParam(pInStr, pInStr->Find("RequiredOlaData[B]"), &pParam, &iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData3.sRequiredOlaData = ' ';			

        if(iLen)
            pInfo->CardSaleExtraData3.sRequiredOlaData = pParam[0];
                
    }
    
    // 4.0.7.501
    bFound = GetParam(pInStr, pInStr->Find("Threshold[F]"), &pParam, &iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);
            long lVal = RoundDouble(x, 1000); 
            l2a0(lVal, pInfo->CardSaleExtraData3.sThreshold, sizeof(pInfo->CardSaleExtraData3.sThreshold));
        }
        else
        {
            l2a0(0L, pInfo->CardSaleExtraData3.sThreshold, sizeof(pInfo->CardSaleExtraData3.sThreshold));
        }		
    }

    bFound = GetParam(pInStr, pInStr->Find("ThresholdType[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData3.sThresholdType = ' ';
        pInfo->CardSaleExtraData3.sThresholdType = pParam[0];

        if(LOG_DETAIL_FLOW_CONTROL)
        {
            CString str;
            
            str.Format("Threshold = %.6s- type = %c", pInfo->CardSaleExtraData3.sThreshold, pInfo->CardSaleExtraData3.sThresholdType);
            _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
        }

    }
                                           
    bFound = GetParam(pInStr, pInStr->Find("GRTotalDiscount[F]"), &pParam, &iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);
            long lVal = RoundDouble(x, 1000); 
            l2a0(lVal, pInfo->CardSaleInfo.extraData.sGRTotalDiscount, sizeof(pInfo->CardSaleInfo.extraData.sGRTotalDiscount));
        }
        else
        {
            l2a0(0L, pInfo->CardSaleInfo.extraData.sGRTotalDiscount, sizeof(pInfo->CardSaleInfo.extraData.sGRTotalDiscount));
        }		
    }
    //4.0.7.501
    
    //4.0.7.503
    bFound = GetParam(pInStr, pInStr->Find("ClockRequired[B]"), &pParam, &iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData3.sClockRequired = ' ';			

        if(iLen)
            pInfo->CardSaleExtraData3.sClockRequired = pParam[0];
                
    }

    //4.0.8.502
    bFound = GetParam(pInStr, pInStr->Find("ReceiptMode[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData3.sReceiptMode = ' ';			

        if(iLen)
            pInfo->CardSaleExtraData3.sReceiptMode = pParam[0];
        
        if(LOG_DETAIL_FLOW_CONTROL)
        {
            CString str;
            
            str.Format("ReceiptMode  is : [%c]", pInfo->CardSaleExtraData3.sReceiptMode);
            _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
        }
        
                
    }
    
    //4.0.8.507	
    bFound = GetParam(pInStr,pInStr->Find("CustomerID[I]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sCustomerID, ' ', sizeof(pInfo->CardSaleExtraData3.sCustomerID));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sCustomerID, pParam, min(sizeof(pInfo->CardSaleExtraData3.sCustomerID), iLen));
    }

    //4.0.9.500
    bFound = GetParam(pInStr, pInStr->Find("BarCode[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData2.sBarCode,' ',sizeof(pInfo->CardSaleExtraData2.sBarCode));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData2.sBarCode, pParam, min(sizeof(pInfo->CardSaleExtraData2.sBarCode), iLen));
    }

    //4.0.9.500
    bFound = GetParam(pInStr,pInStr->Find("DoSessionAuth[B]"),&pParam,&iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData3.sDoSessionAuth = ' ';			

        if(iLen)
            pInfo->CardSaleExtraData3.sDoSessionAuth = pParam[0];
                
    }

    //4.0.9.505
    bFound = GetParam(pInStr,pInStr->Find("GRVoucherNumber[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleInfo.cardData.sGRVoucherNumber, ' ', sizeof(pInfo->CardSaleInfo.cardData.sGRVoucherNumber));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sGRVoucherNumber, pParam, min(sizeof(pInfo->CardSaleInfo.cardData.sGRVoucherNumber), iLen));
    }

    //4.0.9.601
    bFound = GetParam(pInStr, pInStr->Find("ReeferLimit[F]"), &pParam, &iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);
            long lVal = RoundDouble(x, 1000); 
            l2a0(lVal, pInfo->CardSaleExtraData3.sReeferLimit , sizeof(pInfo->CardSaleExtraData3.sReeferLimit));
        }
        else
        {
            l2a0(0L, pInfo->CardSaleExtraData3.sReeferLimit, sizeof(pInfo->CardSaleExtraData3.sReeferLimit));
        }
        
        if(LOG_DETAIL_FLOW_CONTROL)//
        {			
            CString str;
            str.Format("Reefer limit %.10s", pInfo->CardSaleExtraData3.sReeferLimit);
            _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
        }

    }

    bFound = GetParam(pInStr, pInStr->Find("DieselLimit[F]"), &pParam, &iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);
            long lVal = RoundDouble(x, 1000); 
            l2a0(lVal, pInfo->CardSaleExtraData3.sDieselLimit, sizeof(pInfo->CardSaleExtraData3.sDieselLimit));
        }
        else
        {
            l2a0(0L, pInfo->CardSaleExtraData3.sDieselLimit, sizeof(pInfo->CardSaleExtraData3.sDieselLimit));
        }	
        
        if(LOG_DETAIL_FLOW_CONTROL)//
        {			
            CString str;
            str.Format("Diesel limit %.10s", pInfo->CardSaleExtraData3.sDieselLimit);
            _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
        }

    }

    bFound = GetParam(pInStr, pInStr->Find("CashBackLimit[F]"), &pParam, &iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);
            long lVal = RoundDouble(x, 1000);
            l2a0(lVal, pInfo->CardSaleExtraData3.sCashBacklimit, sizeof(pInfo->CardSaleExtraData3.sCashBacklimit));
        }
        else
        {
            l2a0(0L, pInfo->CardSaleExtraData3.sCashBacklimit, sizeof(pInfo->CardSaleExtraData3.sCashBacklimit));
        }	
        
        if(LOG_DETAIL_FLOW_CONTROL)//
        {			
            CString str;
            str.Format("CashBack limit %.10s", pInfo->CardSaleExtraData3.sCashBacklimit);
            _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
        }
    }

    bFound = GetParam(pInStr, pInStr->Find("ItemRestriction[I]"), &pParam, &iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sItemRestriction, ' ', sizeof(pInfo->CardSaleExtraData3.sItemRestriction));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sItemRestriction, pParam, min(sizeof(pInfo->CardSaleExtraData3.sItemRestriction), iLen));
            
        if(LOG_DETAIL_FLOW_CONTROL)//
        {			
            CString str;
            str.Format("sAddItems limit %.3s", pInfo->CardSaleExtraData3.sItemRestriction);
            _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
        }
    }

    bFound = GetParam(pInStr, pInStr->Find("ServiceFee[F]"), &pParam, &iLen); //4.0.9.512
    if(bFound) 
    {
        if(iLen)
        {
            double x;
            x = atof(pParam);

            long lVal = RoundDouble(x, 100);  //4.0.10.504
            l2a0(lVal, pInfo->CardSaleExtraData2.sServiceFee, sizeof(pInfo->CardSaleExtraData2.sServiceFee));
        }
        else
        {
            l2a0(0L, pInfo->CardSaleExtraData2.sServiceFee, sizeof(pInfo->CardSaleExtraData2.sServiceFee));
        }	
        
        if(LOG_DETAIL_FLOW_CONTROL)//
        {
            long lPumpNumber = a2l(pInfo->CardSaleInfo.cardData.sPumpNumber, sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));
            if(!PUMP_NOT_VALID(lPumpNumber))
            {
                CString str;
                str.Format("Service fee charge %.5s", pInfo->CardSaleExtraData2.sServiceFee);
                _LOGMSG.LogMsg("COLABase", "ParseData", lPumpNumber, LOG_PUMP, str);
            }
            else
            {
                CString str;
                str.Format("Service fee charge %.5s", pInfo->CardSaleExtraData2.sServiceFee);
                _LOGMSG.LogMsg("COLABase", "ParseData", 0, LOG_NONE, str);
            }			
        }
    }

    //4.0.10.130
    bFound = GetParam(pInStr, pInStr->Find("AmountLimitIncludesCashBack[B]"), &pParam, &iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData3.sAmountLimitIncludesCashBack = ' ';			

        if(iLen)
            pInfo->CardSaleExtraData3.sAmountLimitIncludesCashBack = pParam[0];				
    }

    bFound = GetParam(pInStr,pInStr->Find("AllowPartialCashBack[B]"),&pParam,&iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData3.sAllowPartialCashBack = ' ';			

        if(iLen)
            pInfo->CardSaleExtraData3.sAllowPartialCashBack = pParam[0];				
    }

    bFound = GetParam(pInStr,pInStr->Find("DisplayCashBackAmount[B]"),&pParam,&iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData3.sDisplayCashBackAmount = ' ';			

        if(iLen)
            pInfo->CardSaleExtraData3.sDisplayCashBackAmount = pParam[0];				
    }

    //4.0.10.130
    bFound = GetParam(pInStr,pInStr->Find("DryItemsRestriction[I]"),&pParam,&iLen); // 4.0.10.504
    if(bFound) 
    {		
        pInfo->CardSaleExtraData3.sDryItemsRestriction = ' ';

        if(iLen)
            pInfo->CardSaleExtraData3.sDryItemsRestriction = pParam[0];						
    }

    bFound = GetParam(pInStr,pInStr->Find("CustomerValidationType[I]"),&pParam,&iLen); // 4.0.11.505
    if(bFound) 
    {		
        pInfo->CardSaleExtraData3.sCustomerValidationType = ' ';

        if(iLen)
            pInfo->CardSaleExtraData3.sCustomerValidationType = pParam[0];						
    }	

    //4.0.11.505
    bFound = GetParam(pInStr,pInStr->Find("OLABatchId[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sOLABatchId,' ',sizeof(pInfo->CardSaleExtraData3.sOLABatchId));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sOLABatchId ,pParam,min(sizeof(pInfo->CardSaleExtraData3.sOLABatchId),iLen));
            
        if(LOG_DETAIL_FLOW_CONTROL)//
        {			
            CString str;
            str.Format("sOLABatchId  %.7s" ,pInfo->CardSaleExtraData3.sOLABatchId);
            _LOGMSG.LogMsg(str);
        }
    }

    bFound = GetParam(pInStr,pInStr->Find("TruckingCompanyName[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sTruckingCompanyName,' ',sizeof(pInfo->CardSaleExtraData3.sTruckingCompanyName));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTruckingCompanyName,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTruckingCompanyName),iLen));
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //***** 4.0.12.505 Start
    bFound = GetParam(pInStr,pInStr->Find("MagCapabilities[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sMagCapabilities,' ',sizeof(pInfo->CardSaleExtraData3.sMagCapabilities));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sMagCapabilities,pParam,min(sizeof(pInfo->CardSaleExtraData3.sMagCapabilities),iLen));
    }
    
    bFound = GetParam(pInStr,pInStr->Find("IssueNumber[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sIssueNumber,' ',sizeof(pInfo->CardSaleExtraData3.sIssueNumber));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sIssueNumber,pParam,min(sizeof(pInfo->CardSaleExtraData3.sIssueNumber),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("ScreenMsgNumber[I]"),&pParam,&iLen);//4.0.30.40
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData5.sScreenMsgNumber,' ',sizeof(pInfo->CardSaleExtraData5.sScreenMsgNumber));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.sScreenMsgNumber,pParam,min(sizeof(pInfo->CardSaleExtraData5.sScreenMsgNumber),iLen));
    }
    else // TD 443183
    {
        memset(pInfo->CardSaleExtraData5.sScreenMsgNumber,' ',sizeof(pInfo->CardSaleExtraData5.sScreenMsgNumber));
    }

    bFound = GetParam(pInStr,pInStr->Find("Club_ID[I]"),&pParam,&iLen); // 4.0.22.980 TD 332329
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData5.byClubId,' ',sizeof(pInfo->CardSaleExtraData5.byClubId));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData5.byClubId,pParam,min(sizeof(pInfo->CardSaleExtraData5.byClubId),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TerminalType[S]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sTerminalType,' ',sizeof(pInfo->CardSaleExtraData3.sTerminalType));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTerminalType,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTerminalType),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("STerminalCountryCode[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sTerminalCountryCode,' ',sizeof(pInfo->CardSaleExtraData3.sTerminalCountryCode));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTerminalCountryCode,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTerminalCountryCode),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TransactionCurrencyCode[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sTransactionCurrencyCode,' ',sizeof(pInfo->CardSaleExtraData3.sTransactionCurrencyCode));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTransactionCurrencyCode,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTransactionCurrencyCode),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("AIP[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sAIP,' ',sizeof(pInfo->CardSaleExtraData3.sAIP));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sAIP,pParam,min(sizeof(pInfo->CardSaleExtraData3.sAIP),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("ATC[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sATC,' ',sizeof(pInfo->CardSaleExtraData3.sATC));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sATC,pParam,min(sizeof(pInfo->CardSaleExtraData3.sATC),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TVR[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sTVR,' ',sizeof(pInfo->CardSaleExtraData3.sTVR));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTVR,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTVR),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("AID[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sAID,' ',sizeof(pInfo->CardSaleExtraData3.sAID));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sAID,pParam,min(sizeof(pInfo->CardSaleExtraData3.sAID),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("AVN[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sAVN,' ',sizeof(pInfo->CardSaleExtraData3.sAVN));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sAVN,pParam,min(sizeof(pInfo->CardSaleExtraData3.sAVN),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("CryptogramInfoData[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sCryptogramInfoData,' ',sizeof(pInfo->CardSaleExtraData3.sCryptogramInfoData));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sCryptogramInfoData,pParam,min(sizeof(pInfo->CardSaleExtraData3.sCryptogramInfoData),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("IssuerApplicationData[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sIssuerApplicationData,' ',sizeof(pInfo->CardSaleExtraData3.sIssuerApplicationData));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sIssuerApplicationData,pParam,min(sizeof(pInfo->CardSaleExtraData3.sIssuerApplicationData),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("CVMResult[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sCVMResult,' ',sizeof(pInfo->CardSaleExtraData3.sCVMResult));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sCVMResult,pParam,min(sizeof(pInfo->CardSaleExtraData3.sCVMResult),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TermResponseCode[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sTermResponseCode,' ',sizeof(pInfo->CardSaleExtraData3.sTermResponseCode));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTermResponseCode,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTermResponseCode),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("ScreenPrompt[I]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sScreenPrompt,' ',sizeof(pInfo->CardSaleExtraData3.sScreenPrompt));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sScreenPrompt,pParam,min(sizeof(pInfo->CardSaleExtraData3.sScreenPrompt),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("AuthCode[S]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleInfo.cardData.sAuthCode,' ',sizeof(pInfo->CardSaleInfo.cardData.sAuthCode));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sAuthCode,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sAuthCode),iLen));
    }	

    bFound = GetParam(pInStr,pInStr->Find("LoyaltySessionID[I]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sLoyaltySessionID,' ',sizeof(pInfo->CardSaleExtraData3.sLoyaltySessionID));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sLoyaltySessionID,pParam,min(sizeof(pInfo->CardSaleExtraData3.sLoyaltySessionID),iLen));
    }
    
    //4.0.13.504
    bFound = GetParam(pInStr,pInStr->Find("TimeCardInserted[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sTimeCardInserted,' ',sizeof(pInfo->CardSaleExtraData3.sTimeCardInserted));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTimeCardInserted,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTimeCardInserted),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TimeTRSRequired[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sTimeTRSRequired,' ',sizeof(pInfo->CardSaleExtraData3.sTimeTRSRequired));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTimeTRSRequired,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTimeTRSRequired),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TimeTRSResponse[S]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleExtraData3.sTimeTRSResponse,' ',sizeof(pInfo->CardSaleExtraData3.sTimeTRSResponse));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTimeTRSResponse,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTimeTRSResponse),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TimeHCResponse[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sTimeHCResponse,' ',sizeof(pInfo->CardSaleExtraData3.sTimeHCResponse));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTimeHCResponse,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTimeHCResponse),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TimePINRequest[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sTimePINRequest,' ',sizeof(pInfo->CardSaleExtraData3.sTimePINRequest));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTimePINRequest,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTimePINRequest),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TimeOLARequest[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sTimeOLARequest,' ',sizeof(pInfo->CardSaleExtraData3.sTimeOLARequest));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTimeOLARequest,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTimeOLARequest),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TimeOLAResult[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sTimeOLAResult,' ',sizeof(pInfo->CardSaleExtraData3.sTimeOLAResult));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTimeOLAResult,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTimeOLAResult),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("TimeTNXDetails[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sTimeTNXDetails,' ',sizeof(pInfo->CardSaleExtraData3.sTimeTNXDetails));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sTimeTNXDetails,pParam,min(sizeof(pInfo->CardSaleExtraData3.sTimeTNXDetails),iLen));
    }

    //4.0.13.504
    bFound = GetParam(pInStr,pInStr->Find("HostOnLineID[I]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sHostOnLineID,' ',sizeof(pInfo->CardSaleExtraData3.sHostOnLineID));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sHostOnLineID,pParam,min(sizeof(pInfo->CardSaleExtraData3.sHostOnLineID),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("ReasonOnLineCode[I]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sReasonOnLineCode,' ',sizeof(pInfo->CardSaleExtraData3.sReasonOnLineCode));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sReasonOnLineCode,pParam,min(sizeof(pInfo->CardSaleExtraData3.sReasonOnLineCode),iLen));
    }

    //4.0.13.510
    bFound = GetParam(pInStr,pInStr->Find("IssueDate[E]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(pInfo->CardSaleExtraData3.sIssueDate,' ',sizeof(pInfo->CardSaleExtraData3.sIssueDate));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sIssueDate,pParam,min(sizeof(pInfo->CardSaleExtraData3.sIssueDate),iLen));
    }

    
    bFound = GetParam(pInStr,pInStr->Find("ClubCardTrack2[S]"),&pParam,&iLen);
    if(bFound) 
    {		
        memset(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data,' ',sizeof(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sClubCardTrack2Data),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("IssuerAuthenticationData[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData3.sIssuerAuthData,' ',sizeof(pInfo->CardSaleExtraData3.sIssuerAuthData));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData3.sIssuerAuthData,pParam,min(sizeof(pInfo->CardSaleExtraData3.sIssuerAuthData),iLen));
    }
    
    bFound = GetParam(pInStr,pInStr->Find("SponsorNumber[S]"),&pParam,&iLen); //4.0.19.503
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData4.sSponsorNum,' ',sizeof(pInfo->CardSaleExtraData4.sSponsorNum));
        if(iLen)
            memcpy(pInfo->CardSaleExtraData4.sSponsorNum,pParam,min(sizeof(pInfo->CardSaleExtraData4.sSponsorNum),iLen));
    }
    
    //4.0.14.503
    bFound = GetParam(pInStr,pInStr->Find("OLAQualifySpend0[I]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData3.sOLAQualifySpend0,' ',sizeof(pInfo->CardSaleExtraData3.sOLAQualifySpend0));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData3.sOLAQualifySpend0,pParam,min(sizeof(pInfo->CardSaleExtraData3.sOLAQualifySpend0),iLen));			
    }


    bFound = GetParam(pInStr,pInStr->Find("OLAQualifySpend1[I]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData3.sOLAQualifySpend1,' ',sizeof(pInfo->CardSaleExtraData3.sOLAQualifySpend1));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData3.sOLAQualifySpend1,pParam,min(sizeof(pInfo->CardSaleExtraData3.sOLAQualifySpend1),iLen));			
    }
    //4.0.14.503


//***** 4.0.12.505 End

    //4.0.15.500
    bFound = GetParam(pInStr,pInStr->Find("OverfillAdjusted[B]"),&pParam,&iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData3.sOverfillAdjusted = ' ';			

        if(iLen)
            pInfo->CardSaleExtraData3.sOverfillAdjusted = pParam[0];
                
    }

    //4.0.20.501 (4.0.17.670) {
    bFound = GetParam(pInStr,pInStr->Find("RemoveProhibitedData[B]"),&pParam,&iLen);
    if(bFound) 
    {	
        pInfo->CardSaleExtraData4.byIsRemoveProhibitedData = ' ';					//4.0.24.60 TD 106586

        if(iLen)
            pInfo->CardSaleExtraData4.byIsRemoveProhibitedData = pParam[0];			//4.0.24.60 TD 106586
    }


    bFound = GetParam(pInStr,pInStr->Find("SystemID[I]"),&pParam,&iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData3.sAuthorizerSystemID,' ',sizeof(pInfo->CardSaleExtraData3.sAuthorizerSystemID));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData3.sAuthorizerSystemID,pParam,min(sizeof(pInfo->CardSaleExtraData3.sAuthorizerSystemID),iLen));			
    }
    //4.0.20.501 (4.0.17.670)}

    bFound = GetParam(pInStr, pInStr->Find("StaffFlag[S]"), &pParam, &iLen);
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData4.sStaffFlag, ' ', sizeof(pInfo->CardSaleExtraData4.sStaffFlag));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData4.sStaffFlag, pParam, min(sizeof(pInfo->CardSaleExtraData4.sStaffFlag), iLen));			
    }

    bFound = GetParam(pInStr, pInStr->Find("CardRangeNo[S]"), &pParam, &iLen); //4.0.22.504
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData4.sCardRangeNo, ' ', sizeof(pInfo->CardSaleExtraData4.sCardRangeNo));
        memset(&pInfo->CardSaleExtraData4.sCardRangeNoExt, ' ', sizeof(pInfo->CardSaleExtraData4.sCardRangeNoExt));
        if(iLen > 3)
        {
            memcpy(&pInfo->CardSaleExtraData4.sCardRangeNo, &pParam[0] , sizeof(pInfo->CardSaleExtraData4.sCardRangeNo));
            memcpy(&pInfo->CardSaleExtraData4.sCardRangeNoExt, &pParam[iLen -1], sizeof(pInfo->CardSaleExtraData4.sCardRangeNoExt));
        }
        else		
            memcpy(&pInfo->CardSaleExtraData4.sCardRangeNo, pParam, min(sizeof(pInfo->CardSaleExtraData4.sCardRangeNo), iLen));	
        
    } // TD 228117

    bFound = GetParam(pInStr, pInStr->Find("PanRequiredProtection[B]"), &pParam, &iLen);							//4.0.22.504 TD 72716
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData4.sPanRequiredProtection, ' ', sizeof(pInfo->CardSaleExtraData4.sPanRequiredProtection));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData4.sPanRequiredProtection, pParam, min(sizeof(pInfo->CardSaleExtraData4.sPanRequiredProtection), iLen));			
    }

    bFound = GetParam(pInStr, pInStr->Find("Track2Exist[B]"), &pParam, &iLen);	//4.0.22.40 TD 72716
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData4.sTrack2Exist, ' ', sizeof(pInfo->CardSaleExtraData4.sTrack2Exist));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData4.sTrack2Exist, pParam, min(sizeof(pInfo->CardSaleExtraData4.sTrack2Exist), iLen));			
    }

    
    bFound = GetParam(pInStr, pInStr->Find("MaskAccountNumber[B]"), &pParam, &iLen);	
    if(bFound) 
    {	
        memset(&pInfo->CardSaleExtraData3.sMaskAccountNumber, ' ', sizeof(pInfo->CardSaleExtraData4.sTrack2Exist));
        if(iLen)
            memcpy(&pInfo->CardSaleExtraData3.sMaskAccountNumber, pParam, min(sizeof(pInfo->CardSaleExtraData4.sTrack2Exist), iLen));			
    } // TD 332389

	if (SESSION_TYPE(byOlaStat) == SESSION_GET_CUSTOMER_INFO)  //RFUEL-672 (RFUEL-3317)
	{
		bFound = GetParam(pInStr, pInStr->Find("HostTranId[S]"), &pParam, &iLen);
		if (bFound)
		{
			memset(pInfo->CardSaleExtraData6.sLoylatyRefHostTranId, ' ', sizeof(pInfo->CardSaleExtraData6.sLoylatyRefHostTranId));
			if (iLen)
				memcpy(&pInfo->CardSaleExtraData6.sLoylatyRefHostTranId, pParam, min(sizeof(pInfo->CardSaleExtraData6.sLoylatyRefHostTranId), iLen));
		}
	}

	if ((SESSION_TYPE(byOlaStat) == SESSION_AUTHORIZE) || //4.0.23.2651
		(SESSION_TYPE(byOlaStat) == SESSION_GET_CUSTOMER_INFO)) // QC 485864
	{
		// CR 407579
		bFound = GetParam(pInStr, pInStr->Find("LoyaltyAutoArm[B]"), &pParam, &iLen);
		if (bFound)
		{
			memset(&pInfo->CardSaleExtraData5.sLoyaltyAutoArm, ' ', sizeof(pInfo->CardSaleExtraData5.sLoyaltyAutoArm));
			if (iLen)
				memcpy(&pInfo->CardSaleExtraData5.sLoyaltyAutoArm, pParam, min(sizeof(pInfo->CardSaleExtraData5.sLoyaltyAutoArm), iLen));

			switch (pInfo->CardSaleExtraData5.sLoyaltyAutoArm)
			{
			case 'Y':
				pInfo->CardSaleExtraData5.sLoyaltyAutoArm = '1';
				break;
			case 'N':
				pInfo->CardSaleExtraData5.sLoyaltyAutoArm = '0';
				break;
			}

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("sLoyaltyAutoArm %c", pInfo->CardSaleExtraData5.sLoyaltyAutoArm);
				_Module.m_server.m_cLogger.LogMsg(str);
			}

			if (pInfo->CardSaleExtraData5.sLoyaltyAutoArm == '1')	// QC 452777
			{
				if (_Module.m_server.BuildPriceProtectionDiscounts(lPumpNumber, pInfo) == TRUE)
				{
					pInfo->CardSaleExtraData5.sPriceProtectionUsed = '1';

					if (LOG_DETAIL_FLOW_CONTROL)
					{
						CString str;
						str.Format("COLABase::ParseData() - sPriceProtectionUsed %c", pInfo->CardSaleExtraData5.sPriceProtectionUsed);
						_Module.m_server.m_cLogger.LogMsg(str);
					}
				}
			}
		}
	}


    //4.0.28.500 start
    if (pInfo->CardSaleExtraData4.sTrack2Exist == 'Y')
    {
        short iFound = 0;
        
        
        if (pInStr->Find(",Track1[S]") !=  -1)
        {
            bFound = GetParam(pInStr,pInStr->Find(",Track1[S]") +1,&pParam,&iLen);
            if(bFound) 
            {		
                memset(pInfo->CardSaleInfo.cardData.sTrack1Data,' ',sizeof(pInfo->CardSaleInfo.cardData.sTrack1Data));
                if(iLen)
                    memcpy(pInfo->CardSaleInfo.cardData.sTrack1Data,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sTrack1Data),iLen));										
            }
            
        }

        if (pInStr->Find(",Track2[S]") != -1)
        {
            bFound = GetParam(pInStr,pInStr->Find(",Track2[S]") +1 ,&pParam,&iLen);
            if(bFound) 
            {		
                
                memset(pInfo->CardSaleInfo.cardData.sTrack2Data,' ',sizeof(pInfo->CardSaleInfo.cardData.sTrack2Data));
                if(iLen)
                    memcpy(pInfo->CardSaleInfo.cardData.sTrack2Data,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sTrack2Data),iLen));							
            }
            
        }
        
    } //4.0.28.500 end

    bFound = GetParam(pInStr, pInStr->Find("TrsAuthorizeTimeOut[I]"), &pParam, &iLen);
    if(bFound && iLen) 
    {	
        long lTimeOut = a2l((unsigned char *) pParam,sizeof(pParam));
        _Module.m_server.SetAutoReconcileTimeout(lPumpNum , (short)lTimeOut , PAY_AT_PUMP);
    } // TD 233242 4.0.23.1050

    bFound = GetParam(pInStr, pInStr->Find("AuthorisationResponseCode[S]"), &pParam, &iLen);   //4.0
    if (bFound)
    {
        memset(pInfo->CardSaleExtraData5.sAuthorisationResponseCode, ' ', sizeof(pInfo->CardSaleExtraData5.sAuthorisationResponseCode));
        if (iLen)
            memcpy(pInfo->CardSaleExtraData5.sAuthorisationResponseCode, pParam, min(sizeof(pInfo->CardSaleExtraData5.sAuthorisationResponseCode), iLen));
        CString str;
        str.Format("AuthorisationResponseCode[S]= %.2s", pInfo->CardSaleExtraData5.sAuthorisationResponseCode);
        _Module.m_server.m_cLogger.LogMsg(str);

    }
	
	bFound = GetParam(pInStr,pInStr->Find("ClubCardNumberHash[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(pInfo->CardSaleExtraData6.sClubCardHashNumber,' ',sizeof(pInfo->CardSaleExtraData6.sClubCardHashNumber));
		if (iLen)
		  memcpy(pInfo->CardSaleExtraData6.sClubCardHashNumber, pParam, min(sizeof(pInfo->CardSaleExtraData6.sClubCardHashNumber), iLen));		
	           
    } // 4.0.32.??? TD 405435

    
    long lCentsPerLiter = 0;
    bFound = GetParam(pInStr, pInStr->Find("AmountOffPerUnit[F]"), &pParam, &iLen);	//4.0.22.40 TD 72716
    if(bFound) 
    {	
        double dAmountOffPerUnit;
        dAmountOffPerUnit = atof(pParam);

        lCentsPerLiter = RoundDouble(dAmountOffPerUnit , 1000);
    }

    long lCentsPerLitreVolumeLimit = 0;
    bFound = GetParam(pInStr,pInStr->Find("AmountOffVolumeLimit[F]"),&pParam,&iLen);
    if(bFound && iLen) 
    {
        lCentsPerLitreVolumeLimit = RoundDouble(atof(pParam) , 1000);	
    }

    bFound = GetParam(pInStr,pInStr->Find("PartialApprovalFlag[B]"),&pParam,&iLen);
    if(bFound)
    {
        pInfo->CardSaleExtraData6.sPartialApproval = 'N';
    }

    long lRemaningBalance = 0;
    bFound = GetParam(pInStr,pInStr->Find("BalanceDue[F]"),&pParam,&iLen);
    if(bFound && iLen) 
    {
        lRemaningBalance = RoundDouble(atof(pParam),100);
        l2a0(lRemaningBalance, pInfo->CardSaleExtraData6.sRemainingBalance , 10);
    }

    if (lState == OLASRV_NEED_MORE_PARAMS)
    {
        ParseNeedMoreParamData(pInStr,pInfo,pTrs,&lRetCode);	//4.0.18.12	//4.0.18.501
    }
    
    long lMediaMasterType = a2l(pInfo->CardSaleExtraData2.sMediaMasterType,sizeof(pInfo->CardSaleExtraData2.sMediaMasterType));

    if (_Module.m_server.GetTwoTendersInTransaction() && _Module.m_server.m_cCommonFunction.IsPaymentCard(lMediaMasterType) && (pInfo->CardSaleInfo.cardData.sTranStatus == CD_APPROVE_OK) && (SESSION_TYPE(byOlaStat) == SESSION_AUTHORIZE) && lState != OLASRV_NEED_MORE_PARAMS) // TD 404910 
    {
        _Module.m_server.SaveTenderData(lPumpNumber, pInfo);		//CR 404913 
    
        if(lRemaningBalance > 0 && _Module.m_server.GetNumberOfTendersAuthotized(lPumpIndex) == 2)
        {
            // can't authorize three tenders
            pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_DECLINED;
        }
        else if(_Module.m_server.GetNumberOfTendersAuthotized(lPumpIndex) == 1)
        {
            if(lRemaningBalance > 0)
            {
                long lAuthrizedAmount = ConvertStringsToLong(pInfo->CardSaleInfo.cardData.sAuthAmt,sizeof(pInfo->CardSaleInfo.cardData.sAuthAmt),pInfo->CardSaleInfo.extraData.sAuthAmt_Msb,sizeof(pInfo->CardSaleInfo.extraData.sAuthAmt_Msb));				
                lAuthrizedAmount += lRemaningBalance;
                ConvertLongToStrings(lAuthrizedAmount,pInfo->CardSaleInfo.cardData.sAuthAmt,sizeof(pInfo->CardSaleInfo.cardData.sAuthAmt),pInfo->CardSaleInfo.extraData.sAuthAmt_Msb,sizeof(pInfo->CardSaleInfo.extraData.sAuthAmt_Msb));

                str.Format("Authorized amount updated to include remaining balance, current authorized amount = $%d" ,lAuthrizedAmount/100);
                _LOGMSG.LogClassMsg("COLABase","ParseData",0,LOG_NONE,str);
            }
        }
        else if(_Module.m_server.GetNumberOfTendersAuthotized(lPumpIndex) == 2)
        {
            long lFirstAuthorizedAmount = _Module.m_server.GetFirstTenderAuthorizedAmountLimit(lPumpNumber); 
            long lRemaningBalance = _Module.m_server.GetRemainingBalance(lPumpNumber);
            long lSecondAuthorizedAmount = _Module.m_server.GetSecondTenderAuthorizedAmountLimit(lPumpNumber);

            str.Format("First tender authorized amount  = $%d, remaining balance = $%d , second tender authorized amount = $%d" ,lFirstAuthorizedAmount/100,lRemaningBalance/100,lSecondAuthorizedAmount/100);
            _LOGMSG.LogClassMsg("COLABase","ParseData",0,LOG_NONE,str);

            long lAuthrizedAmount = lFirstAuthorizedAmount + lSecondAuthorizedAmount;
            ConvertLongToStrings(lAuthrizedAmount,pInfo->CardSaleInfo.cardData.sAuthAmt,sizeof(pInfo->CardSaleInfo.cardData.sAuthAmt),pInfo->CardSaleInfo.extraData.sAuthAmt_Msb,sizeof(pInfo->CardSaleInfo.extraData.sAuthAmt_Msb));
        }
    }
    
    return lRetCode;	
}

void COLABase::ParseZoneFromOlaBuffer(long lPumpNumber, CString * pInStr, char * pParam, int iLen, short byOlaStat, PAY_AT_PUMP_INFO * pInfo)
{
    BOOL bFound = GetParam(pInStr, pInStr->Find("Zone[S]"), &pParam, &iLen);
    if (bFound && 
		((SESSION_TYPE(byOlaStat) != SESSION_AUTHORIZE) || (_Module.m_server.GetPrintReceiptOnDecline() > 0)))
    {
        SaveZone(pInfo, pParam, iLen);
    }

    // used to set put ReceiptMessageN/ReceiptHeaderN into either:
    //     payment (SESSION_AUTHORIZE/SESSION_COMPLETE):  index <B1>  through <B9>
    //     others including loyalty:                      index <B10> through <B18>
    // note that this does not work if there are more than ReceiptMessage0[S] and there also exist any ReceiptMessageHeaderN[S] fields
    for (int i = 0; i <OLA_START_ZONE_INDEX - 1; i++)
    {
        CString sField;
        sField.Format("ReceiptMessage%d[S]", i);

        BOOL bReceiptMessage = GetParam(pInStr, pInStr->Find(sField), &pParam, &iLen);
        if (bReceiptMessage)
        {
            int iZoneIndex = i + 1; // <B1>
            // use the offset whenever this is not a payment session
            if (!((SESSION_TYPE(byOlaStat) == SESSION_AUTHORIZE) ||
                  (SESSION_TYPE(byOlaStat) == SESSION_COMPLETE)))
            {
                iZoneIndex += OLA_START_ZONE_INDEX - 1; // <B10>
            }

            SaveZone(pInfo, pParam, iLen, iZoneIndex);

			sField.Format("ReceiptMessageHeader%d[S]", i);   //4.0.33.17 

			bReceiptMessage = GetParam(pInStr, pInStr->Find(sField), &pParam, &iLen);

			if (bReceiptMessage)
			{
				SaveZone(pInfo, pParam, iLen, iZoneIndex + 1); // <B2> or <B11>
			}
        }
    }
}

// this will remove the <BN>...<\BN> portion of the sFiler2043 string where N==iZoneIndex, or leave the buffer untouched if not found
// returns the length of the zone information in the buffer, whether or not anything was removed
int COLABase::RemoveZone(RECEIPT_EXTRA_INFO_REC& Info, int iZoneIndex)
{
    CString sFiler((CHAR*)&Info.info.sFiler2043, sizeof(Info.info.sFiler2043));
    CString sZoneStart;
    sZoneStart.Format("<B%d>", iZoneIndex);
    CString sZoneEnd;
    sZoneEnd.Format("<\\B%d>", iZoneIndex);

    sFiler.Trim();
    int iFilerLength = sFiler.GetLength();

    if (LOG_DETAIL_FLOW_CONTROL)
    {
        CString strTmp;   //4.  //4.0.5.40 
        strTmp.Format("RemoveZone %d enter size %d, buffer=%s", iZoneIndex, iFilerLength, sFiler.GetBuffer());
        _LOGMSG.LogMsg(strTmp);
    }


    // find the start of the zone
    int iStartPos = sFiler.Find(sZoneStart);
    if (iStartPos == -1)
    {
        return iFilerLength;
    }

    // find the end of the zone, at the last marker, just in case there are multiples (which there should never be)
    int iEndPos = -1;
    int iNextPos = iStartPos + sZoneStart.GetLength();
    while ((iNextPos + sZoneEnd.GetLength()) <= sFiler.GetLength()) 
    {
        iNextPos = sFiler.Find(sZoneEnd, iNextPos);
        if (iNextPos == -1)
            break;
        iNextPos += sZoneEnd.GetLength();
        iEndPos = iNextPos;
    }
    
    if (iEndPos == -1)
    {
        CString strTmp;   //4.  //4.0.5.40 
        strTmp.Format("RemoveZone error - found start of zone %d at %d, but no end zone marker", iZoneIndex, iStartPos);
        _LOGMSG.LogMsg(strTmp);
        return iFilerLength;
    }
    // remove the zone
    sFiler.Delete(iStartPos, iEndPos - iStartPos);
    memset(&Info.info.sFiler2043, ' ', sizeof(Info.info.sFiler2043));
    iFilerLength = min(sizeof(Info.info.sFiler2043), sFiler.GetLength());
    memcpy(&Info.info.sFiler2043, sFiler.GetBuffer(), iFilerLength);

    if (LOG_DETAIL_FLOW_CONTROL)
    {
        CString strTmp;   //4.  //4.0.5.40 
        strTmp.Format("RemoveZone %d exit size %d, buffer=%s", iZoneIndex, iFilerLength, sFiler.GetBuffer());
        _LOGMSG.LogMsg(strTmp);
    }

    return iFilerLength;
}

// either inserts or updates iLen bytes of pParam's data into zone iZoneIndex of pInfo's sFiler2043, and either inserts or updates the ReceiptExtraInfoDB at pInfo's sTranNbr index
void COLABase::SaveZone( PAY_AT_PUMP_INFO * pInfo, char * pParam, int iLen, int iZoneIndex)
{
    // sanity check the input
    if ((iLen < 0) || (iLen > sizeof(PS::RECEIPT_EXTRA_INFO_REC)))
    {
        CString strTmp;   //4.  //4.0.5.40 
            strTmp.Format("SaveZone error iLen=%ld is invalid", iLen);
            _LOGMSG.LogMsg(strTmp);
            return;
    }
    PS::RECEIPT_EXTRA_INFO_REC rec;
    PS::RECEIPT_EXTRA_INFO_REC recExist;
    long lRtc = 0;

    // put as much of the new zone information as will fit into rec
    memset(&rec, ' ', sizeof(PS::RECEIPT_EXTRA_INFO_REC));
    memcpy(rec.info.sFiler2043, pParam, min(sizeof(rec.info.sFiler2043), iLen));
    // initialize our record index
    rec.info.lIndexNumber = a2l(pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr));

    // see whether the DB has this record
    memset(&recExist, ' ', sizeof(PS::RECEIPT_EXTRA_INFO_REC));
    recExist.info.lIndexNumber = a2l(pInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pInfo->CardSaleInfo.cardData.sTranNbr));

    lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_READ, recExist);

    if (!lRtc)
    {
        if (LOG_DETAIL_FLOW_CONTROL)
        {
            CString strTmp;   //4.  //4.0.5.40 
            strTmp.Format("SaveZone found existing Index=%ld", recExist.info.lIndexNumber);
            _LOGMSG.LogMsg(strTmp);
        }
        // remove existing zone and get how big remains
        int iRemainingLength = RemoveZone(recExist, iZoneIndex);
        
        // and place the zone index markers around as much as will fit in the remaining space
        UpdateZoneIndex(rec, iLen, iZoneIndex, sizeof(rec.info.sFiler2043) - iRemainingLength);

        // and add back the other zones at the end
        memcpy(rec.info.sFiler2043 + iLen, recExist.info.sFiler2043, min((sizeof(rec.info.sFiler2043) - iLen), iRemainingLength));
 
        // update total data size for later masking
        iLen += iRemainingLength;

        // update the db
        lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_UPDATE, rec);
        if (lRtc)
        {
            CString strTmp;   //4.  //4.0.5.40 
            strTmp.Format("Fail update extra receipt information Index=%ld , Rtc=%ld",
                rec.info.lIndexNumber,
                lRtc);
            _LOGMSG.LogMsg(strTmp);
        }
        else
        {
            if (LOG_DETAIL_FLOW_CONTROL)
            {
                CString strTmp;   //4.  //4.0.5.40 
                strTmp.Format("SaveZone updated extra receipt information Index=%ld ZoneIndex %d iLen=%d buffer=%.*s",
                    rec.info.lIndexNumber, iZoneIndex, iLen, iLen, rec.info.sFiler2043);
                _LOGMSG.LogMsg(strTmp);
            }
        }
    }
    else
    {
        // place the zone index markers around as much as will fit
        UpdateZoneIndex(rec, iLen, iZoneIndex, sizeof(rec.info.sFiler2043));
        
        // insert new record
        lRtc = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_INSERT, rec);
        if (lRtc)
        {
            CString strTmp;   //4.  //4.0.5.40 
            strTmp.Format("Fail insert extra receipt information Index=%ld , Rtc=%ld",
                rec.info.lIndexNumber,
                lRtc);
            _LOGMSG.LogMsg(strTmp);
        }
        else
        {
            if (LOG_DETAIL_FLOW_CONTROL)
            {
                CString strTmp;   //4.  //4.0.5.40 
                strTmp.Format("SaveZone inserted extra receipt information Index=%ld ZoneIndex %d iLen=%d buffer=%.*s",
                    rec.info.lIndexNumber, iZoneIndex, iLen, iLen, rec.info.sFiler2043);
                _LOGMSG.LogMsg(strTmp);
            }
        }
    }

    if (!lRtc)
    {
        // remember that we just stored receipt extra info
        long lTmp = a2l(pInfo->CardSaleExtraData2.sFlags, sizeof(pInfo->CardSaleExtraData2.sFlags));
        lTmp |= FL_EXTRA_RECEIPT_REC;
        l2a(lTmp, pInfo->CardSaleExtraData2.sFlags, sizeof(pInfo->CardSaleExtraData2.sFlags));
    }

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		char szResult[STATIC_STRING_2048] = { 0 };           //4.0.128.280
		_Module.m_server.m_cProtectedDataHandler.LogReplaceBufferUseSize(szResult, pParam, iLen);

		str.Format("Found Zone=[%s],iLen=%d lRtc=%ld", szResult, iLen, lRtc);

		_Module.m_server.m_cLogger.LogOLABufferMsg("SaveZone", str, LOG_LEVEL_3);  //4.0.128.280
	}
}

// places the zone index markers around the sFiler2043 data after truncating so that it all takes no more than iMaxLen bytes
// there had better only be a single, unmarkered parameter in the sFiler2043 data!
void COLABase::UpdateZoneIndex(PS::RECEIPT_EXTRA_INFO_REC &rec, int &iLen, int iZoneIndex, int iMaxLen)
{
    CString sZoneStart;
    sZoneStart.Format("<B%d>", iZoneIndex);
    CString sZoneEnd;
    sZoneEnd.Format("<\\B%d>", iZoneIndex);

    // leave room for the markers
    iMaxLen -= (sZoneStart.GetLength() + sZoneEnd.GetLength());
    if (iMaxLen <= 0)
    {
        // no room for this zone
        iLen = 0;
        if (LOG_DETAIL_FLOW_CONTROL)
        {
            CString strTmp;   //4.  //4.0.5.40 
            strTmp.Format("UpdateZoneIndex %d exit iMaxLen=%d", iMaxLen);
            _LOGMSG.LogMsg(strTmp);
        }
    }
    else
    {
        // keep however much data we can, after leaving room for the markers
        CString sTmp((char*)rec.info.sFiler2043, min(iLen, iMaxLen));
        CString sFinal = sZoneStart + sTmp + sZoneEnd;
        memcpy(rec.info.sFiler2043, sFinal.GetBuffer(), min(sizeof(rec.info.sFiler2043), sFinal.GetLength()));
        iLen = sFinal.GetLength();
        if (LOG_DETAIL_FLOW_CONTROL)
        {
            CString strTmp;   //4.  //4.0.5.40 
            strTmp.Format("UpdateZoneIndex %d exit size %d, buffer=%s", iZoneIndex, iLen, sFinal.GetBuffer());
            _LOGMSG.LogMsg(strTmp);
        }
    }
}

//4.0.18.12
void COLABase::ParseNeedMoreParamData(CString *pInStr,PAY_AT_PUMP_INFO *pInfo, CPumpTransact *pTrs, __int64 *lRetCode) //4.0.22.511 74797
{
    BOOL bFound;
    int iLen ;
    char *pParam;

    CString str;
    if(LOG_BASIC_FLOW_CONTROL)
        str = ">>> OLA: Need more params :";

    int iFound;
    iFound =  pInStr->Find("CarRegistration[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        //4.0.6.501
        memset(pInfo->CardSaleInfo.cardData.sFullVehicleId , ' ' , sizeof(pInfo->CardSaleInfo.cardData.sFullVehicleId));
        pInfo->CardSaleInfo.cardData.sFullVehicleId[0] = '?';
        *lRetCode += REQUIRED_CAR_REGISTRATION;
        if(LOG_BASIC_FLOW_CONTROL)
            str += "CarRegistration";
    }

    iFound = pInStr->Find("Odometer[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "Odometer";
        }
        
        //4.0.6.501
        memset(pInfo->CardSaleInfo.cardData.sOdometer, ' ', sizeof(pInfo->CardSaleInfo.cardData.sOdometer));
        memset(pInfo->CardSaleInfo.extraData.sOdometerExt, ' ', sizeof(pInfo->CardSaleInfo.extraData.sOdometerExt));
        memset(&pInfo->CardSaleExtraData4.sOdometerExt2, ' ', sizeof(pInfo->CardSaleExtraData4.sOdometerExt2));

        pInfo->CardSaleInfo.cardData.sOdometer[0] = '?';
		pInfo->CardSaleExtraData3.sParamResult[0] = ' ';  //RFUEL - 2682 Scanning loyalty and trying to pay with personal Fleet that requires input does not work
        *lRetCode += REQUIRED_ODOMETER;
    }

    //4.0.22.511 74797 start
    iFound = pInStr->Find("Job[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "Job";
        }
        
        memset(pInfo->CardSaleExtraData4.sJob, ' ', sizeof(pInfo->CardSaleExtraData4.sJob));

        pInfo->CardSaleExtraData4.sJob[0] = '?';
        *lRetCode += REQUIRED_JOB;
    }


        iFound = pInStr->Find("AccountType[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "AccountType";
        }
        
        memset(&pInfo->CardSaleExtraData2.sAccountType, ' ', sizeof(pInfo->CardSaleExtraData2.sAccountType));

        pInfo->CardSaleExtraData2.sAccountType = '?';
        *lRetCode += REQUIRED_ACCOUNT_TYPE;
    }


    iFound = pInStr->Find("Department[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "Department";
        }
        
        memset(pInfo->CardSaleExtraData4.sDepartment, ' ', sizeof(pInfo->CardSaleExtraData4.sDepartment));

        pInfo->CardSaleExtraData4.sDepartment[0] = '?';
        *lRetCode += REQUIRED_DEPARTMENT;
    }

    iFound = pInStr->Find("License[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "License";
        }
        
        memset(pInfo->CardSaleExtraData4.sLicense, ' ', sizeof(pInfo->CardSaleExtraData4.sLicense));

        pInfo->CardSaleExtraData4.sLicense[0] = '?';
        *lRetCode += REQUIRED_LICENSE;
    }

    iFound = pInStr->Find("CustomerData[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "CustomerData";
        }
        
        //4.0.6.501
        memset(pInfo->CardSaleExtraData2.sCustomerReference, ' ', sizeof(pInfo->CardSaleExtraData2.sCustomerReference));
        memset(pInfo->CardSaleExtraData4.sCustomerReferenceExt, ' ', sizeof(pInfo->CardSaleExtraData4.sCustomerReferenceExt));

        pInfo->CardSaleExtraData2.sCustomerReference[0] = '?';
        *lRetCode += REQUIRED_CUSTOMER_REFERENCE;
    }

    iFound = pInStr->Find("UserID[I]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "UserID";
        }
        
        //4.0.6.501
        memset(pInfo->CardSaleExtraData3.sCustomerID, ' ', sizeof(pInfo->CardSaleExtraData3.sCustomerID));
        memset(pInfo->CardSaleExtraData4.sCustomerIDExt, ' ', sizeof(pInfo->CardSaleExtraData4.sCustomerIDExt));

        pInfo->CardSaleExtraData3.sCustomerID[0] = '?';
        *lRetCode += REQUIRED_USER_ID;
    }
    //4.0.22.511 74797 end

    iFound = pInStr->Find("PIN[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        //4.0.2.15 Start 
        //Added explicit call to SysFreeString.  This is slower,
        //but we can be certain that all resources are freed.
        BSTR bstrTmpInStr = pInStr->AllocSysString();

        //Create a _bstr_t object that allocated a copy
        //of bstrTmpInStr.  This copy is freed in the _bstr_t's
        //d'tor.
        _bstr_t sEncryptionKeysBuffer(bstrTmpInStr , true); 

        //Free tmp string.  There is a separate copy of this BSTR
        //in sEncryptionKeysBuffer
        SysFreeString(bstrTmpInStr);
        //4.0.2.15 end

        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";

            str += "PIN";
        }
        pInfo->CardSaleInfo.cardData.sPINBlock[0] = '?';
        *lRetCode += REQUIRED_PIN;
        _Module.m_server.UpdateEncryptionKeys(0,sEncryptionKeysBuffer);  // 2.9.1.2
    }

    iFound = pInStr->Find("GeneralID[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";

            str += "GeneralID";
        }
        memset(pInfo->CardSaleInfo.cardData.sGeneralID , ' ' , sizeof(pInfo->CardSaleInfo.cardData.sGeneralID));
        pInfo->CardSaleInfo.cardData.sGeneralID[0] = '?';
        *lRetCode += REQUIRED_GENERAL_ID;
    }

    iFound = pInStr->Find("ReturnTank[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";

            str += "ReturnTank";
        }
        pInfo->CardSaleInfo.cardData.sReturnTank = '?';
        *lRetCode += REQUIRED_RETURN_TANK;
    }

    iFound = pInStr->Find("DriverID[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "DriverID";
        }
        // 4.0.6.501
        memset(pInfo->CardSaleInfo.cardData.sDriverId , ' ' , sizeof(pInfo->CardSaleInfo.cardData.sDriverId));
        memset(pInfo->CardSaleExtraData3.sDriverId_MSB , ' ' , sizeof(pInfo->CardSaleExtraData3.sDriverId_MSB)); //4.0.23.370 117473 //4.0.24.90
        memset(pInfo->CardSaleExtraData4.sDriverId_MSB2 , ' ' , sizeof(pInfo->CardSaleExtraData4.sDriverId_MSB2)); //4.0.23.370 117473 //4.0.24.90
        pInfo->CardSaleInfo.cardData.sDriverId[0] = '?';		
        *lRetCode += REQUIRED_DRIVER_ID;
    }

    //iFound = pInStr->Find("Track2[S]");
    //if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))

    iFound = pInStr->Find("Track2[S]");
    //4.0.14.30 if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))  //4.0.14.30
    if((iFound != -1) && (GetParamLenEx("Track2[S]",pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "Track2";
        }

        pInfo->CardSaleInfo.cardData.sTrack2Data[0] = '?';		
        *lRetCode += REQUIRED_PAYMENT_CARD;
    }

    iFound = pInStr->Find("PromptRewards[B]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "PromptRewards";
        }
        pInfo->CardSaleInfo.cardData.sGRPrompt = '?';
        *lRetCode += REQUIRED_PROMPT_REWARDS;
    }
    
    iFound = pInStr->Find("GRVoucherNumber[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "GRVoucherNumber";
        }
        pInfo->CardSaleInfo.cardData.sGRVoucherNumber[0] = '?';
        *lRetCode += REQUIRED_VOUCHER_NUMBER;
    }

    iFound = pInStr->Find("GRKeyCode[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "GRKeyCode";
        }
        pInfo->CardSaleInfo.cardData.sGRKeyCode[0] = '?';
        *lRetCode += REQUIRED_KEY_CODE;
    }

    //4.0.15.502
    iFound = pInStr->Find("PONumber[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "PONumber";
        }
        pInfo->CardSaleExtraData3.sPONumber[0] = '?';
        *lRetCode += REQUIRED_PO_NUMBER_PARAM;
    }

/////////////////////////////
//		iFound = pInStr->Find("MediaType[S]");
//		if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
//		{
//				if(LOG_BASIC_FLOW_CONTROL)
///				{
//					if(lRetCode)
//						str += ", ";
//					str += "MediaType";
//				}
//				pInfo->CardSaleInfo.cardData.sTranType = '?';
//
//				CString stmp;
//				stmp.Format("need more parem [%c]", pInfo->CardSaleInfo.cardData.sTranType);
//				_LOGMSG.LogMsg(stmp);
//				_LOGMSG.LogMsg("Test");
//				
//				lRetCode += REQUIRED_MEDIA_TYPE;
//		}
//
////////////////////

    bFound = GetParam(pInStr,pInStr->Find("MediaType[S]"),&pParam,&iLen);
    if((bFound) && ((GetParamLen(pInStr,iFound) == 0) || (pParam[0] >= '1' && pParam[0] <= '9'))) // Costco EMV 4.0.31.0 274401
    {
        if(iLen == 1)
        {
            if(LOG_BASIC_FLOW_CONTROL)
            {
                if(*lRetCode)
                    str += ", ";
                str += "MediaType";
            }
            pInfo->CardSaleInfo.cardData.sTranType = pParam[0];
            *lRetCode += REQUIRED_MEDIA_TYPE;
        }


    }

    //start  husky new promte

    iFound = pInStr->Find("Unit[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "Unit";
        }
        memset(pInfo->CardSaleExtraData2.sUnit , ' ', sizeof(pInfo->CardSaleExtraData2.sUnit)); //4.0.6.501
        pInfo->CardSaleExtraData2.sUnit[0] = '?';
        *lRetCode += REQUIRED_UNIT;
    }

    iFound = pInStr->Find("TripNumber[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "TripNumber";
        }
        memset(pInfo->CardSaleExtraData2.sTripNumber ,' ', sizeof(pInfo->CardSaleExtraData2.sTripNumber)); //4.0.6.501
        pInfo->CardSaleExtraData2.sTripNumber[0] = '?';
        *lRetCode += REQUIRED_TRIP_NUMBER;
    }


    iFound = pInStr->Find("CustomerReference[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "CustomerReference";
        }
        memset(pInfo->CardSaleExtraData2.sCustomerReference , ' ', sizeof(pInfo->CardSaleExtraData2.sCustomerReference)); //4.0.6.501
        pInfo->CardSaleExtraData2.sCustomerReference[0] = '?';
        *lRetCode += REQUIRED_CUSTOMER_REFERENCE;
    }


    iFound = pInStr->Find("TractorHub[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "TractorHub";
        }
        memset(pInfo->CardSaleExtraData2.sTractorHub , ' ',sizeof(pInfo->CardSaleExtraData2.sTractorHub));
        pInfo->CardSaleExtraData2.sTractorHub[0] = '?';
        *lRetCode += REQUIRED_TRACTOR_HUB;
    }

    iFound = pInStr->Find("TrailerNumber[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "TrailerNumber";
        }
        memset(pInfo->CardSaleExtraData2.sTrailerNumber , ' ' , sizeof(pInfo->CardSaleExtraData2.sTrailerNumber)); //4.0.6.501
        pInfo->CardSaleExtraData2.sTrailerNumber[0] = '?';
        *lRetCode += REQUIRED_TRAILER_NUMBER;
    }

    iFound = pInStr->Find("TrailerHub[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "TrailerHub";
        }
        memset(pInfo->CardSaleExtraData2.sTrailerHub ,' ', sizeof(pInfo->CardSaleExtraData2.sTrailerHub)); //4.0.6.501
        pInfo->CardSaleExtraData2.sTrailerHub[0] = '?';
        *lRetCode += REQUIRED_TRAILER_HUB;
    }

    iFound = pInStr->Find("TransAmount[F]"); //3.1.1.1
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "TransAmount";
        }
        pInfo->CardSaleInfo.extraData.sTotalAmt[0] = '?';
        *lRetCode += REQUIRED_TRANS_AMOUNT;
    }

//////////////////////////////////////

    iFound = pInStr->Find("DeclinedLoyaltyInvalid[B]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "DeclinedLoyaltyInvalid";
        }

        pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_LOYALTY_INVALID;
        *lRetCode += REQUIRED_DECLINE_LOYALTY_PROMTE;
    }


    iFound = pInStr->Find("DeclinedLoyaltyExpired[B]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "DeclinedLoyaltyExpired";
        }

        pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_LOYALTY_EXPIRED;
        *lRetCode += REQUIRED_DECLINE_LOYALTY_PROMTE;		
    }


    iFound = pInStr->Find("DeclinedLoyaltyWithPayCard[B]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "DeclinedLoyaltyWithPayCard";
        }

        pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_LOYALTY_WITH_PAYCARD;
        *lRetCode += REQUIRED_DECLINE_LOYALTY_PROMTE;		
    }

    //4.0.0.86
    iFound = pInStr->Find("DeclinePaymentRetry[B]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "DeclinePaymentRetry";
        }

        pInfo->CardSaleExtraData2.sDeclinedLoyaltyPrompt = DECLINE_PAYMENT_CARD;
        
        *lRetCode += REQUIRED_DECLINE_LOYALTY_PROMTE;		
    }
    //4.0.0.86
    
    
    iFound = pInStr->Find("PassCode[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "PassCode";
        }

        memset(pInfo->CardSaleExtraData2.sPassCode , ' ' , sizeof(pInfo->CardSaleExtraData2.sPassCode));
        pInfo->CardSaleExtraData2.sPassCode[0] = '?';
        *lRetCode += REQUIRED_PASS_CODE;		
    }

    //3.1.1.1
    iFound = pInStr->Find("PinPadAction[I]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "PinPadAction";
        }
        *lRetCode += REQUIRED_PINPAD_ACTION;
    }


    // 4.0.1.1
    iFound = pInStr->Find("ChargeTo[I]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "ChargeTo";
        }
        memset(pInfo->CardSaleExtraData2.sChargeTo , ' ' , sizeof(pInfo->CardSaleExtraData2.sChargeTo));
        pInfo->CardSaleExtraData2.sChargeTo[0]= '?';
        *lRetCode += REQUIRED_CHARGE_TO;
    }
    // 4.0.1.1

    // 4.0.1.31
    iFound = pInStr->Find("ZipCode[I]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "Zip Code";
        }
        memset(pInfo->CardSaleExtraData2.sZipCode, ' ' ,sizeof(pInfo->CardSaleExtraData2.sZipCode));
        pInfo->CardSaleExtraData2.sZipCode[0]= '?';
        *lRetCode += REQUIRED_ZIP_CODE;
    }
    // 4.0.1.31

    //46512
    iFound = pInStr->Find("ExtendedZipCode[I]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "Extended Zip Code";
        }
        memset(pInfo->CardSaleExtraData4.sExtendedZipCode, ' ' ,sizeof(pInfo->CardSaleExtraData4.sExtendedZipCode));
        pInfo->CardSaleExtraData4.sExtendedZipCode[0]= '?';
        *lRetCode += REQUIRED_ZIP_CODE;
    }

    //54295
    iFound = pInStr->Find("PumpTestPassword[I]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "Pump test password";
        }
        memset(pInfo->CardSaleExtraData4.sPumpTestPassword, ' ' ,sizeof(pInfo->CardSaleExtraData4.sPumpTestPassword));
        pInfo->CardSaleExtraData4.sPumpTestPassword[0]= '?';
        //*lRetCode += REQUIRED_PUMP_TEST_PWD;
    }


    //4.0.3.47
    iFound = pInStr->Find("RedeemPoints[B]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "RedeemPoints";
        }
        pInfo->CardSaleInfo.extraData.sRedeemPoints = '?' ;			
        *lRetCode += REQUIRED_REDEED_POINT_ACTION;		
    }
    
    //4.0.3.47

    //4.0.3.54

    iFound = pInStr->Find("ConfirmPrompt[B]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))		
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "ConfirmPrompt";
        }
        pInfo->CardSaleInfo.extraData.sConfirmPrompt = '?' ;			
        *lRetCode += REQUIRED_CONFIRM_PROMPT_ACTION;		
    }


    //4.0.5.26
    iFound = pInStr->Find("ClubCardTrack2[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "ClubCardTrack2";
        }

        pInfo->CardSaleInfo.cardData.sClubCardTrack2Data[0] = '?';		
        *lRetCode += REQUIRED_PAYMENT_LOYALTY_CARD;
    }

    //4.0.5.26

    //4.0.5.39
    iFound = pInStr->Find("ParamResult[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "ParamResult";
        }

        memset(pInfo->CardSaleExtraData3.sParamResult, ' ', sizeof(pInfo->CardSaleExtraData3.sParamResult));		//4.0.28.502 255727
        memset(pInfo->CardSaleExtraData5.sParamResultExt, ' ', sizeof(pInfo->CardSaleExtraData5.sParamResultExt));	//4.0.28.502 255727

        pInfo->CardSaleExtraData3.sParamResult[0] = '?';		
        *lRetCode += REQUIRED_PARAM_RESULT_DATA;
    }
    //4.0.5.39
    // 4.0.5.41
    iFound = pInStr->Find("RemoteID[I]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "RemoteID";
        }

        pInfo->CardSaleExtraData3.sRemoteID[0] = '?';		
        *lRetCode += REQUIRED_REMOTE_ID;
    }	

    // 4.0.5.42
    iFound = pInStr->Find("LogicShiftNumber[I]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "LogicShiftNumber";
        }

        pInfo->CardSaleExtraData3.sLogicShiftNumber[0] = '?';		
        *lRetCode += REQUIRED_LOGIC_SHIFT_NUMBER;
    }	


    iFound = pInStr->Find("AuthorizationNumber[S]");
    if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)
                str += ", ";
            str += "AuthorizationNumber";
        }

        pInfo->CardSaleInfo.cardData.sAuthNumber[0] = '?';		
        *lRetCode += REQUIRED_AUTHORIZATION_NUMBER;
    }	

    //4.0.9.507
    iFound = pInStr->Find("PromptName0[S]");
    //4.0.9999.9 if((iFound != -1) && (GetParamLen(pInStr,iFound) == 0))
    if(iFound != -1)
    {
        //PAY_AT_PUMP_INFO	sTmpPAPInfo;

        //memcpy(&sTmpPAPInfo,pInfo , sizeof(PAY_AT_PUMP_INFO));
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)	
                str += ", ";
            str += "PromptName0";
        }

        
        pInfo->CardSaleExtraData3.sParamResult[0] = '?';		
       // _Module.m_server.SaveAndParsePromptInfo(pInStr, pTrs ,pInfo); //4.0.9999.0 

        *lRetCode += REQUIRED_PARAM_RESULT_DATA;
    }	


    iFound = pInStr->Find("CombinedCardSelect[S]");
    
    if(iFound != -1)
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)	
                str += ", ";
            str += "CombinedCardSelect";
        }

        
        pInfo->CardSaleExtraData3.sCombinedCardSelect = '?';		

        *lRetCode += REQUIRED_CnP_PARAM;
    }	

    iFound = pInStr->Find("CVMResult[I]");
    
    if(iFound != -1)
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)	
                str += ", ";
            str += "CVMResult";
        }

        
        pInfo->CardSaleExtraData3.sCVMResult[0] = '?';		
        *lRetCode += REQUIRED_CnP_PARAM;
    }	

    iFound = pInStr->Find("TermResponseCode[I]");
    
    if(iFound != -1)
    {
        if(LOG_BASIC_FLOW_CONTROL)
        {
            if(*lRetCode)	
                str += ", ";
            str += "TermResponseCode";
        }

        
        pInfo->CardSaleExtraData3.sTermResponseCode[0] = '?';		
        *lRetCode += REQUIRED_CnP_PARAM;
    }

	bFound = GetParam(pInStr, pInStr->Find("MoreEMVData[S]"), &pParam, &iLen);
	if (bFound)
	{
		memset(pInfo->CardSaleExtraData5.sExtraEMVData, ' ', sizeof(pInfo->CardSaleExtraData5.sExtraEMVData));
		pInfo->CardSaleExtraData5.ExtraEMVDataReq = ' ';
		if (iLen)
		{
			int  iLenData;
			char *pParamLen;
			bFound = GetParam(pInStr, pInStr->Find("MoreEMVLen[I]"), &pParamLen, &iLenData);

			if (bFound)
			{
				int iLength = a2l((PBYTE)pParamLen, iLenData);
				iLenData = min(iLenData, iLen);
				if (iLenData > sizeof(pInfo->CardSaleExtraData5.sExtraEMVData))
				{
					CString strWarning;
					strWarning.Format("MoreEMVData len more that exceed");
					_Module.m_server.m_cLogger.LogMsg("COLABase", "ParseNeedMoreParam", 0, LOG_NONE, strWarning);
				}
				else
				{
					if (LOG_BASIC_FLOW_CONTROL)
					{
						if (*lRetCode)
							str += ", ";
						str += "MoreEMVData";
					}

					pInfo->CardSaleExtraData5.ExtraEMVDataReq = '?';
					memcpy(pInfo->CardSaleExtraData5.sExtraEMVData, pParam, iLenData);
					*lRetCode += REQUIRED_MORE_EMV_DATA;
				}
			}
		}
	}

    iFound = pInStr->Find("CalculatedMAC[S]");
    if (iFound != -1)
    {
        if (LOG_BASIC_FLOW_CONTROL)
        {
            if (*lRetCode)
                str += ", ";
            str += "CalculatedMAC";
        }

        *lRetCode += REQUIRED_CnP_PARAM;
    }

    if (*lRetCode & REQUIRED_CnP_PARAM || *lRetCode & REQUIRED_ACCOUNT_TYPE) // TD 470156
        pInfo->CardSaleExtraData3.sParamResult[0] = ' ';

    //End husky new promt
    if(LOG_BASIC_FLOW_CONTROL)
        _LOGMSG.LogMsg("COLABase","ParseNeedMoreParam",0,LOG_NONE,str);
}

void COLABase::ParseReportData(CString *pInStr, PAY_AT_PUMP_INFO *pInfo, char *pText, long *plLen)
//void COLABase::ParseReportData(CString *pInStr, CXMLInterface *pInfo, char *pText, long *plLen)
{
    BOOL bFound;
    int iLen ;
    char *pParam;
    char sVal[MAX_FIELD_VAL]; 

    bFound = GetParam(pInStr,pInStr->Find("ResultCode[S]"),&pParam,&iLen);

    if(bFound) 
    {
        char sRes[4];
        memset(sRes,0,sizeof(sRes));
        memcpy(sRes,pParam,min(sizeof(sRes),iLen));
        switch(a2l((unsigned char *)sRes,sizeof(sRes)))
        {
            case CARDSRV_Approved: 
            case CARDSRV_NeedMoreParam:
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_APPROVE_OK;
                //pInfo->SetXmlByteField(ATT_TRAN_STATUS,CD_APPROVE_OK); 4.0.18.501
                break;
            }
            default: 
            {
                pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_NOGOOD;
                //pInfo->SetXmlByteField(ATT_TRAN_STATUS,CD_DECLINE_NOGOOD);

                if(LOG_ERROR)
                {
                    CString str;
                    str.Format("ResultCode[I]=%ld" ,a2l((unsigned char *)sRes,sizeof(sRes)));
                    _LOGMSG.LogMsg("COLABase","ParseReportData",0,LOG_NONE,str);
                }
                
                break;
            }
        }
    }
    else
    {
        pInfo->CardSaleInfo.cardData.sTranStatus = CD_DECLINE_ERROR;
        //pInfo->SetXmlByteField(ATT_TRAN_STATUS,CD_DECLINE_ERROR);
    }


    bFound = GetParam(pInStr,pInStr->Find("CustomerMessage[S]"),&pParam,&iLen);

    if(bFound) 
        if(iLen)
        {
            memcpy(pText,pParam,min(REPORT_TEXT_LENGTH,iLen));
            *plLen = (long)iLen;
        }

    bFound = GetParam(pInStr,pInStr->Find("OriginTrans[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(sVal,0,sizeof(sVal));
        if(iLen)
            memcpy(&(pInfo->CardSaleExtraData2.sOriginTrans),pParam,min(sizeof(pInfo->CardSaleExtraData2.sOriginTrans),iLen));
    }

    bFound = GetParam(pInStr,pInStr->Find("DeviceNumber[S]"),&pParam,&iLen);
    if(bFound) 
    {
        memset(sVal,0,sizeof(sVal));
        if(iLen)
            memcpy(pInfo->CardSaleInfo.cardData.sDeviceNumber,pParam,min(sizeof(pInfo->CardSaleInfo.cardData.sDeviceNumber),iLen));
    }
}


BOOL COLABase::Valid()
{
    return(m_bValid);	
}

//4.0.2.15 start
BYTE COLABase::DecrementRetryAttemps()
{
    EnterCriticalSection(&m_csRetry);//4.0.3.38

    BYTE byRetVal = m_byRetryAttemps;

    //if counter greater than 0, decrement.
    //4.0.2.37 OLARetries no longer read from registry
    if(m_byRetryAttemps > 0)
        m_byRetryAttemps--;
    
    else if(m_byRetryAttemps < 0)
        m_byRetryAttemps = 0;

    if(!m_byRetryAttemps)
        _LOGMSG.LogMsg("COLABase","DecrementRetryAttemps",0,LOG_NONE,"no more retries left!" , LOG_LEVEL_0);

    LeaveCriticalSection(&m_csRetry);//4.0.3.38
    
    return byRetVal;
}

BYTE COLABase::DecrementConnectionAttempts()
{
    EnterCriticalSection(&m_csRetry);//4.0.3.38

    BYTE byRetVal = m_byConnectionAttempts;

    //if counter greater than 0, decrement.
    //4.0.2.37 OLARetries no longer read from registry
    if(m_byConnectionAttempts > 0)
        m_byConnectionAttempts--;
    
    else if(m_byConnectionAttempts < 0)
        m_byConnectionAttempts = 0;

    if(!m_byConnectionAttempts)
        _LOGMSG.LogMsg("COLABase","DecrementConnectionAttempts",0,LOG_NONE,"no more connection retries left!" , LOG_LEVEL_0);

    LeaveCriticalSection(&m_csRetry);//4.0.3.38
    
    return byRetVal;
}

void COLABase::ResetRetryAttemps(BOOL bResetOnlyWhenZero)
{
    //4.0.2.161 start added if statement and critical section call

    //4.0.2.33
    BOOL bReset = FALSE;

    EnterCriticalSection(&m_csRetry);//4.0.3.38

    //4.0.2.33
    if((bResetOnlyWhenZero) && (m_byRetryAttemps == 0))
        bReset = TRUE;

    else if((!bResetOnlyWhenZero) && (m_byRetryAttemps < MAX_OLA_RETRIES))
        bReset = TRUE;
    
    if(bReset)
    {
        m_byRetryAttemps = MAX_OLA_RETRIES;

        //4.0.2.37
        //OLARetries no longer read from registry

        //4.0.2.162
        m_bNoMoreRetries = FALSE;

        _LOGMSG.LogMsg("COLABase","ResetRetryAttemps",0,LOG_NONE,"reset retry attempts" , LOG_LEVEL_0);
    }

    LeaveCriticalSection(&m_csRetry);//4.0.3.38

    //4.0.2.161 end
    
    return;
}
//4.0.2.15 end


void COLABase::ResetConnectionAttempts(BOOL bResetOnlyWhenZero)
{
    //4.0.2.161 start added if statement and critical section call

    //4.0.2.33
    BOOL bReset = FALSE;

    EnterCriticalSection(&m_csRetry);//4.0.3.38

    //4.0.2.33
    if((bResetOnlyWhenZero) && (m_byConnectionAttempts == 0))
        bReset = TRUE;

    else if((!bResetOnlyWhenZero) && (m_byConnectionAttempts < MAX_OLA_RETRIES))
        bReset = TRUE;
    
    if(bReset)
    {
        m_byConnectionAttempts = MAX_OLA_RETRIES;

        //4.0.2.37
        //OLARetries no longer read from registry

        //4.0.2.162
        m_bNoMoreConnectionRetries = FALSE;

        _LOGMSG.LogMsg("COLABase","ResetConnectionAttempts",0,LOG_NONE,"reset connection retry attempts" , LOG_LEVEL_0);
    }

    LeaveCriticalSection(&m_csRetry);//4.0.3.38

    //4.0.2.161 end
    
    return;
}
//4.0.2.15 end


void COLABase::LaunchOLA()
{
    m_bValid = TRUE;
    long lRtc = 0;				
    _LOGMSG.LogMsg("COLABase","LaunchOLA",0,LOG_NONE,"OLA server successfully invoked");

    //OLA12345
    //Create Ola Thread and wait to restart
    _Module.m_server.CheckOLA();	
    
    // Restart thread
    _Module.m_server.CheckOLA();

    //4.0.2.38
    if (_Module.m_server.GetAlarmSrvMode() == ALRMSRV_DIRECT_ACTIVE)
    {
        // Clear OLA error
        if (_Module.m_server.m_cAlarm.m_lAlarmSpecialGroupMap[ALARM_GROUP_OLA])
            lRtc = _Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, SOFTWARE_GENERAL_OLA_ERR,0, 0,0,0);
    }//4.0.2.38
    else if (_Module.m_server.m_lSysError == SYS_ERROR_OLA)
    {	
        _Module.m_server.WriteToLogMsg(0, TRUE, 0, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 0, &lRtc);
    }

    //4.0.28.503 - TD 256429
    //Send general event to RFS when OLA is up
    _Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_OLA_ONLINE);

    return;
}

void COLABase::LoadRegistryParams()
{
    //Start 2.9.3.4
    DWORD dwTmp;

    m_nDeclineCounter = 0;  //2.9.3.4
    _Module.m_server.m_cParam.LoadParam((char *)"GeneralParam", "OlaMaxDeclineMessToDisplay",&dwTmp,100,TRUE);
    m_lOlaMaxDeclineMessToDisplay  = (long)dwTmp;
    // end 2.9.3.4

    return;
}

BOOL COLABase::RecreateService()
{
    //4.0.2.15 end
    if(!m_byRetryAttemps)
        m_bValid = FALSE;
    //4.0.2.15 end

    else if(!m_bValid)	
        Init();
    //Note that the method called is the method
    //of the derived class, not the base class.
    
    return(m_bValid);	
}

void COLABase::StartOLA()
{
    m_bOLAStopped = FALSE;
    RecreateService();
    //Note that the method called is the method
    //of the derived class, not the base class.
}

void COLABase::StopOLA()
{
    m_bOLAStopped = TRUE;
    ReleaseService(TRUE);
    //Note that the method called is the method
    //of the derived class, not the base class.
}

/////////////////////////////////////////////////////////
//Error handling methods.
/////////////////////////////////////////////////////////
void COLABase::Error(BOOL bWriteToLog)
{
    if(bWriteToLog)
        _LOGMSG.LogMsg("COLABase","Error",0,LOG_NONE,"Failed to access OLASrv : Unspecified Error ",LOG_LEVEL_0);											

    ReleaseService(FALSE);
    //Note that the method called is the method
    //of the derived class, not the base class.
}

void COLABase::Error(_com_error & e,BOOL bWriteToLog)
{
    if(bWriteToLog)
    {
        CString str;
        str.Format("Failed to access OLASrv : %.60s", e.ErrorMessage()); 

        _LOGMSG.LogMsg("COLABase","Error",0,LOG_NONE,str,LOG_LEVEL_0);											
    }

    ReleaseService(FALSE);
    //Note that the method called is the method
    //of the derived class, not the base class.
}

void COLABase::Error(_com_error & e, HRESULT & hRes,BOOL bWriteToLog)
{
    if(bWriteToLog)
    {
        CString str;
        str.Format("Failed to access OLASrv : hRes %x : %.50s",hRes, e.ErrorMessage()); 
        _LOGMSG.LogMsg("COLABase","Error",0,LOG_NONE,str,LOG_LEVEL_0);											
    }

    ReleaseService(FALSE);
    //Note that the method called is the method
    //of the derived class, not the base class.
}

void COLABase::CatchCOMError(_com_error cError)
{
    CString sErrorMsg;
    sErrorMsg.Format("The following COM execption has been thrown: %.50s" , cError.ErrorMessage());

    _LOGMSG.LogMsg("COLABase","CatchCOMError",0,LOG_NONE,sErrorMsg , LOG_LEVEL_0);

    return;
}
/******************************************************************************
 Description:	Get Credit price value . 	
 Returns:      	Success/Failed
 Parameters:   	short nGrade
                long  lPriceCash    - Cash price.
                long  lPriceCredit  - Credit price. 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			13/3/2003   13:04		Start
******************************************************************************/
//4.0.2.38 //4.0.3.21
long COLABase::GetGradePrice(short nGrade ,long & lPriceCash , long & lPriceCredit , long & lServiceMode)
{
     lPriceCredit = 0;
     lPriceCash = 0;

    if(GRADE_NOT_VALID(nGrade))
        return 1;

    if (nGrade > 0 )
    {
        DWORD dwTmp;
        CString sGradeTable;

        sGradeTable.Format("Grades\\Grade%02d",nGrade);
        //4.0.8.503

        if (lServiceMode == FULL_SERVICE_MODE)
        {
            _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCredit_FullService",&dwTmp ,(DWORD)0,FALSE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
            lPriceCredit =(long)dwTmp;
            _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCash_FullService",&dwTmp ,(DWORD)0,FALSE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);		
            lPriceCash = (long)dwTmp;

        }
        else
        {
            _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCredit_SelfService",&dwTmp ,(DWORD)0,FALSE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
            lPriceCredit =(long)dwTmp;
            _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCash_SelfService",&dwTmp ,(DWORD)0,FALSE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);		
            lPriceCash = (long)dwTmp;
        }		
        //4.0.8.503
    }

    return 0;
}
//4.0.3.49
/******************************************************************************
 Description:	Check for slip flag 	
 Returns:       None 	
 Parameters:   	CString *pInStr    - OLA string 
                BYTE    sSlipFlags - pumpSrv OLA field. 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			30/7/2003   14:30		Start
******************************************************************************/

void COLABase::CheckSlipFlags(CString *pInStr, BYTE &sSlipFlags)
{
    char *pParam;
    char sTmpSlipFlags = sSlipFlags;
    BOOL bFound;
    int iLen; 

    if (sTmpSlipFlags == ' ')
        sTmpSlipFlags =0;
        
    
    bFound = GetParam(pInStr,pInStr->Find("NoOdometerSlip[B]"),&pParam,&iLen);
    
    if(bFound) 
    {			
        if(iLen && (pParam[0] =='Y'))
        {
            sTmpSlipFlags += IGNORE_ODOMETER;
        }
        else
            sTmpSlipFlags &= NOT_IGNORE_ODOMETER;			
    }

    bFound = GetParam(pInStr,pInStr->Find("NoCarRegistrationSlip[B]"),&pParam,&iLen);
    if(bFound) 
    {			
        if(iLen && (pParam[0] =='Y'))
        {
            sTmpSlipFlags += IGNORE_CAR_REG;
        }
        else
            sTmpSlipFlags &=NOT_IGNORE_CAR_REG;
    }

    if (sTmpSlipFlags)
        sSlipFlags = sTmpSlipFlags;
    else
        sSlipFlags = ' ';
    

}

void COLABase::ReasetCounter()
{
    m_nDeclineCounter = 0;
}

/******************************************************************************
 Description:	return status is supporting  IFSF 	
 Returns:      	0- not support/ 1- support 
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			21/4/2004   19:13		Start
******************************************************************************/

BOOL COLABase::SupportIFSFProtocoll()
{
    return (m_bSupportIFSF);
}

void COLABase::SetIFSFProtocoll(BOOL bVal)
{
    m_bSupportIFSF =  bVal;
}

void COLABase::AddedPreAuthInformation(long lPumpNumber,long & lOffset, char *pOutBuffer , long & lOLAPumpServiceMode, long & lGradeRestrict)
{
    PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);
    long lCount = cPumpItems.lCounter;
    char sFiledName[MAX_FIELD_NAME];
    long	lPriceCash=0 , lPriceCredit=0;
    long	lNumOfItems=0;//4.0.11.504	fix this function TD 14868
    

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
                    
                        sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemCode%d[S]",lNumOfItems);
                        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%03ld,",		_Module.m_server.GetGradeCode((short)cPumpItems.m_cItemSoldInfo[i].lGrade));			
                        
                        sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemPrice%d[F]",lNumOfItems);

                        if (lOLAPumpServiceMode  == FULL_SERVICE_MODE)
                            lOffset += SetParam(pOutBuffer+lOffset, (BYTE *)sFiledName, (BYTE *) "%s=%ld.%03ld,", cPumpItems.m_cItemSoldInfo[i].lPriceCreditFullService / 1000, cPumpItems.m_cItemSoldInfo[i].lPriceCreditFullService % 1000);				
                        else
                            lOffset += SetParam(pOutBuffer+lOffset, (BYTE *)sFiledName, (BYTE *) "%s=%ld.%03ld,", cPumpItems.m_cItemSoldInfo[i].lPriceCreditSelfService / 1000, cPumpItems.m_cItemSoldInfo[i].lPriceCreditSelfService % 1000);
                        //4.0.11.502
                        //Attached is the new fields that will be send to OLA in order to report low price & high price.   
						
						GetGradePrice((short) cPumpItems.m_cItemSoldInfo[i].lGrade, lPriceCash, lPriceCredit, lOLAPumpServiceMode);//4.0.11.122
						
						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPriceLow%d[S]", lNumOfItems);//4.0.11.50
						lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *) "%s=%ld.%03ld,", lPriceCash / 1000, lPriceCash % 1000);//4.0.11.122
						sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPriceHigh%d[S]", lNumOfItems);//4.0.11.50
                        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sFiledName, (BYTE *)"%s=%ld.%03ld,", lPriceCredit / 1000, lPriceCredit % 1000);//4.0.11.122

                        sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemIsFuel%d[B]",lNumOfItems);
                        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,(BYTE *)"%s=%s,",(BYTE *) "Y",1);

                        sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemType%d[S]",lNumOfItems);

                        // required only for pilot   //4.0.9999.0
                        if (_Module.m_server.m_cPumpArray[lPumpNumber -1].GetDieselPumpFlags())
                            lOffset += SetParam(pOutBuffer+lOffset, (BYTE *) sFiledName, (BYTE *)"%s=%s,",(BYTE *) "C",1);
                        else
                            lOffset += SetParam(pOutBuffer+lOffset, (BYTE *) sFiledName, (BYTE *)"%s=%s,",(BYTE *) "P",1);
                        lNumOfItems++;  //4.0.11.504

                    }

                }
                else
                {
                    sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemIsFuel%d[B]",lNumOfItems);
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,(BYTE *)"%s=%s,",(BYTE *)"N",1);

                    sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemCode%d[S]",lNumOfItems);
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%03ld,",	cPumpItems.m_cItemSoldInfo[i].lGrade);			
                    lNumOfItems++;  //4.0.11.504
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

void COLABase::ParseItemProduct(long &lPumpNumber, CString *pInStr, PAY_AT_PUMP_INFO  *pInfo, CPumpTransact *pTrs) //4.0.9999.0
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
        bFound = GetParam(pInStr,pInStr->Find("RestrictionItemRequired[I]"),&pParam,&iLen);
        if (bFound)
        {
            long lItemType =  a2l((BYTE *)pParam,iLen);
            l2a(lItemType,&pInfo->CardSaleExtraData3.sRestrictionItemRequired,iLen);
        }
        for(long i=0;i<nItemCounter;i++)
        {
           if(ParsePositionGrades(lPumpNumber,pInStr,pInfo,i))
           {

           }
           else 
           {
           
                sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemCode%d[S]",i);

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
                            lGradeRestriction |= (long)pow((double)2,(double)(cPumpItems.m_cItemSoldInfo[lIndex].lGrade -1));	
                        }
                        else
                        {
                            lCarWashRestriction = 1;
                        }

                        sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemDiscount%d[F]",i);
                        bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);

                        if ((bFound) && (iLen))
                        {
                            long lGradePos =0;//4.0.10.504
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


                        if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
                        {
                            if(LOG_BASIC_FLOW_CONTROL)
                            {
                                sMsg.Format("Parse OLA IFSF data pump%ld,code=%d, discount=%.5s" ,lPumpNumber,  lNACSCode, pInfo->CardSaleExtraData2.sDiscountPerGrade[cPumpItems.m_cItemSoldInfo[lIndex].lGrade -1].sDiscount);
                                _LOGMSG.LogMsg("COLABase","ParseItemProduct",lPumpNumber,LOG_PUMP,sMsg);
                            }
                        }
                        
                    }
                    else
                    {
                        if(LOG_BASIC_FLOW_CONTROL)
                        {
                            sMsg.Format("Item Not found in lits of IFSF pump%ld,code=%d" ,lPumpNumber,  lNACSCode);
                            _LOGMSG.LogMsg("COLABase","ParseItemProduct",lPumpNumber,LOG_PUMP,sMsg);
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
                _LOGMSG.LogMsg("COLABase","ParseItemProduct",lPumpNumber,LOG_PUMP,sMsg);
            }
        }
    }
}


/*
void COLABase::ParseItemProduct(long &lPumpNumber, CString *pInStr, CXMLInterface  *pInfo, CPumpTransact *pTrs) //4.0.9999.0
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
        bFound = GetParam(pInStr,pInStr->Find("RestrictionItemRequired[I]"),&pParam,&iLen);
        if (bFound)
        {
            long lItemType =  a2l((BYTE *)pParam,iLen);
            pInfo->SetXmlLongField(ATT_RESTRICTION_ITEM_REQUIRED,lItemType);
            //l2a(lItemType,&pInfo->CardSaleExtraData3.sRestrictionItemRequired,iLen);
        }
        for(long i=0;i<nItemCounter;i++)
        {
           if(ParsePositionGrades(lPumpNumber,pInStr,pInfo,i))
           {

           }
           else 
           {
           
                sprintf_s(sFiledName,"ItemCode%d[S]",i);

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

                        sprintf_s(sFiledName,"ItemDiscount%d[F]",i);
                        bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);

                        if ((bFound) && (iLen))
                        {
                            long lGradePos =0;//4.0.10.504
                            double x;
                            x = atof(pParam);
                            lVal = RoundDouble(x , 1000);
                            
                            // Grade product discount
                            if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
                            {
                                
                                pInfo->SetXmlLongFieldByAttribute(ELEMENT_DISCOUNT,ATT_ID,(cPumpItems.m_cItemSoldInfo[lIndex].lGrade) -1 ,ATT_DISCOUNT,lVal);
                                //l2a(lVal,
                                //	pInfo->CardSaleExtraData2.sDiscountPerGrade[(cPumpItems.m_cItemSoldInfo[lIndex].lGrade) -1 ].sDiscount , sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[(cPumpItems.m_cItemSoldInfo[lIndex].lGrade) - 1].sDiscount));							
                                
                            }
                            else  // Carwash item for now there is only one 
                            {
                                pInfo->SetXmlLongField(ATT_CAR_WASH_DISCOUNT,lVal);
                                //l2a(lVal,
                                //	pInfo->CardSaleExtraData3.sCarWashDiscount,min(sizeof(pInfo->CardSaleExtraData3.sCarWashDiscount) , iLen));															
                            
                            }
                        } 


                        if (cPumpItems.m_cItemSoldInfo[lIndex].bFuelItem)
                        {
                            if(LOG_BASIC_FLOW_CONTROL)
                            {
                                sMsg.Format("Parse OLA IFSF data pump%ld,code=%d, discount=%ld" ,lPumpNumber,  lNACSCode, pInfo->GetXmlLongFieldByAttribute(ELEMENT_DISCOUNT,ATT_ID,(cPumpItems.m_cItemSoldInfo[lIndex].lGrade) -1 ,ATT_DISCOUNT,lVal));
                                _LOGMSG.LogMsg("COLABase","ParseItemProduct",lPumpNumber,LOG_PUMP,sMsg);
                            }
                        }
                        
                    }
                    else
                    {
                        if(LOG_BASIC_FLOW_CONTROL)
                        {
                            sMsg.Format("Item Not found in lits of IFSF pump%ld,code=%d" ,lPumpNumber,  lNACSCode);
                            _LOGMSG.LogMsg("COLABase","ParseItemProduct",lPumpNumber,LOG_PUMP,sMsg);
                        }
                    }
                     
                }
            }

            // update grade restriction map
            pInfo->SetXmlLongField(ATT_GRADE_RESTRICTION,lGradeRestriction);
            //l2a(lGradeRestriction , pInfo->CardSaleInfo.cardData.sGradeRestriction , sizeof(pInfo->CardSaleInfo.cardData.sGradeRestriction));

            // update carwash restriction map
            pInfo->SetXmlLongField(ATT_CAR_WASH_RESTRICTION,lCarWashRestriction);
            //l2a(lCarWashRestriction ,&pInfo->CardSaleExtraData3.sCarwashRestriction, sizeof(pInfo->CardSaleExtraData3.sCarwashRestriction));

            if(LOG_BASIC_FLOW_CONTROL)
            {
                sMsg.Format("OLA restrict Pump=%ld with the following details Grade map=%ld,Car wash=%ld",lPumpNumber, lGradeRestriction, lCarWashRestriction);
                _LOGMSG.LogMsg("COLABase","ParseItemProduct",lPumpNumber,LOG_PUMP,sMsg);
            }
        }
    }
}
*/
void COLABase::LoadAllProductSoldOnPump(long lPumpNumber)
{
    m_cForecourtProductList.LoadAllProductSoldOnPump(lPumpNumber);
}
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
BOOL COLABase::ConvertNACSItemsToIndex(long &lPumpNumber, long &lNACSCode, long &lIndex)
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
/******************************************************************************
 Description:	Set status for item list in PreAuth session
 Returns:      	0- not support/ 1- support 
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			21/4/2004   19:13		Start
******************************************************************************/

void COLABase::SetPreAuthIFSF(BOOL bVal)
{
    m_bPreAuthIFSF = bVal;
}
/******************************************************************************
 Description:	Get status if need to send item list in PreAuth session
 Returns:      	0- not support/ 1- support 
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			21/4/2004   19:13		Start
******************************************************************************/
BOOL COLABase::GetPreAuthIFSF()
{
    return m_bPreAuthIFSF;
}



/******************************************************************************
 Description:Return all Grades that hold in pump 	
 Returns: grades arr to the XML Rtaps     	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			18/05/2005   19:13		Start
******************************************************************************/
void COLABase::GetProductForSinglePump(long & lPumpNumber,PumpItems & cRec )
{
    m_cForecourtProductList.GetAllProductsForSinglePump(lPumpNumber,cRec);
}

void COLABase::SetEndSessionEx(BOOL bVal)
{
    m_bUseSessionEnd = bVal;
}

BOOL COLABase::GetEndSessionEx()
{
    return m_bUseSessionEnd;
}

long COLABase::EndSessionEx(long lSessID, long SessionResult)
{

    return 0;	
}

void COLABase::AddPriceInformation(long &lOffset, char *pInStr)
{
    long lPriceCash , lPriceCredit ;
    long lGradeNacs;
    long lCounter = 0 ;
    long lServiceMode = FULL_SERVICE_MODE;
    char sTmpBuffer[50];  ///4.0.10.50
    
    for ( short nGradeIndex = 0 ;nGradeIndex < MAX_GRADES ; nGradeIndex++)
    {
        if (!GetGradePrice(nGradeIndex + 1 ,lPriceCash , lPriceCredit ,lServiceMode))
        {
            lGradeNacs = _Module.m_server.GetGradeCode(nGradeIndex+1);

            sprintf_s(sTmpBuffer ,50,"ItemCode%ld[S]" , lCounter);			
            lOffset += SetParam(pInStr+ lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%ld,",	lGradeNacs);
            
            sprintf_s(sTmpBuffer,50,"ItemPriceLow%ld[S]" , lCounter);
            lOffset += SetParam(pInStr+lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%ld.%03ld,",	lPriceCash/1000,lPriceCash%1000);			

            sprintf_s(sTmpBuffer,50,"ItemPriceHigh%ld[S]" , lCounter);
            lOffset += SetParam(pInStr+lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%ld.%03ld,",	lPriceCredit/1000,lPriceCredit%1000);		

            sprintf_s(sTmpBuffer,50,"ItemPriceType%ld[S]" , lCounter);
            lOffset += SetParam(pInStr+lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%.1c,",	'F');

            lCounter++	;	
            lServiceMode =SELF_SERVICE_MODE;
            GetGradePrice(nGradeIndex + 1 ,lPriceCash , lPriceCredit ,lServiceMode);


            sprintf_s(sTmpBuffer,50,"ItemCodeX%ld[S]" , lCounter);			
            lOffset += SetParam(pInStr+ lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%ld,",	lGradeNacs);
            
            sprintf_s(sTmpBuffer,50,"ItemPriceLowX%ld[S]" , lCounter);
            lOffset += SetParam(pInStr+lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%ld.%03ld,",	lPriceCash/1000,lPriceCash%1000);			

            sprintf_s(sTmpBuffer,50,"ItemPriceHighX%ld[S]" , lCounter);
            lOffset += SetParam(pInStr+lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%ld.%03ld,",	lPriceCredit/1000,lPriceCredit%1000);		

            sprintf_s(sTmpBuffer,50,"ItemPriceTypeX%ld[S]" , lCounter);
            lOffset += SetParam(pInStr+lOffset,(BYTE *)sTmpBuffer,	(BYTE *)"%s=%.1c,",	'S');

            lCounter++	;	
        }
    }
//4.0.10.50
    lOffset += SetParam(pInStr+lOffset,(BYTE *)",ItemCount[I]",		(BYTE *)"%s=%ld,",		lCounter);
}

/******************************************************************************
 Description:In case we have to parse the grades and insert into position array
             we parse the data from ola .
             4.0.10.502
 Returns: TURE if m_bControllerSupportNozzleRestrction is on 
          and we use position.
          FALSE if we use restection .
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			17/08/2005   19:13		Start
******************************************************************************/
BOOL COLABase::ParsePositionGrades(long &lPumpNumber, CString *pInStr, PAY_AT_PUMP_INFO *pInfo,int iCountPos)
{
    CString  sMsg;
    PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);
    long	lNACSCode;
    //long	lIndex;
    BOOL	bFound= FALSE;
    int		iLen ;
    char	*pParam;
    short	nItemCounter = 0,nTmpPosition =0;
    char	sFiledName[MAX_FIELD_NAME]; 
    long	lGradePosition = 0,lGradeNumber = 0,lNumOfPosition=0;  //4.0.11.513
    BOOL	bRetCode = FALSE;
    long    lVal=0,idx=0;
    double  x =0;

    //if we use position
    if(_Module.m_server.m_bControllerSupportNozzleRestrction)
    {
        bRetCode = TRUE; //4.0.11.161
        //In case value is set '1' , Cl2PumpSrv will authorize the Pump as 
        //define in the position restriction
        
        // find NACS item code, 
        
        sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemCode%d[S]",iCountPos);

        // find NACS item code, 
        bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);
        if (bFound)
        {
            memset(pInfo->CardSaleExtraData4.discountAttrsInfo.DiscountIDs[iCountPos].sDiscountGrade, ' ', sizeof(pInfo->CardSaleExtraData4.discountAttrsInfo.DiscountIDs[iCountPos].sDiscountGrade)); //4.0.22.509 38162
            lNACSCode = a2l((BYTE *)pParam,iLen);
            // convert NACS item to index
            for(int i = 0; i < MAX_ITEMS; i++)
            {
                if (cPumpItems.m_cItemSoldInfo[i].lNACSCode == lNACSCode)
                {
                    lVal = 0;
                    // Grade product number
                    if (cPumpItems.m_cItemSoldInfo[i].bFuelItem)
                    {
                        lGradeNumber += cPumpItems.m_cItemSoldInfo[i].lGrade;  ////4.0.11.513	

                        CString sItemDiscountId; 
                        sItemDiscountId.Format("ItemDiscountId%d[I]=", iCountPos);  
                        BOOL bFound2 = GetParam(pInStr,pInStr->Find(sItemDiscountId),&pParam,&iLen); 

                        if (bFound2)
                        {
                            l2a(lGradeNumber,pInfo->CardSaleExtraData4.discountAttrsInfo.DiscountIDs[iCountPos].sDiscountGrade ,sizeof(pInfo->CardSaleExtraData4.discountAttrsInfo.DiscountIDs[iCountPos].sDiscountGrade)); //4.0.22.509 38162
                            sMsg.Format("COLABase::ParsePositionGrades Discount attr, grade %ld was updated for index %ld",lGradeNumber,iCountPos);
                            _LOGMSG.LogMsg(sMsg);
                        }
                        
                        lNumOfPosition = _Module.m_server.m_cPumpArray[lPumpNumber -1].GetNumOfPositionForGrade((short)lGradeNumber); //4.0.11.513
                        sMsg.Format("COLABase::ParsePositionGrades num of position is  %ld for grade %ld",lNumOfPosition,lNACSCode);
                        _LOGMSG.LogMsg(sMsg);
                        idx=0;
                        for(int count = 0; count < lNumOfPosition; count++)
                        {
                            nTmpPosition = _Module.m_server.m_cPumpArray[lPumpNumber -1].GetGradePosition((short)lGradeNumber,&idx); //4.0.11.513
                            if((nTmpPosition <= 0)||(nTmpPosition > MAX_GRADES))
                            {
                            }
                            else
                            {
                                sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemVolumeLimit%d[F]",iCountPos);
                                //get the limit 
                                bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);

                                if (nTmpPosition - 1 >= 0 && nTmpPosition - 1 < MAX_POSITIONS_PER_PUMP)
                                {
                                    if(!(bFound))
                                    {
                                        pInfo->CardSaleExtraData3.GradeLimitsArray[nTmpPosition-1].sLimitType = '0';
                                        sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemAmountLimit%d[F]",iCountPos);
                                        bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);
                                    }
                                    else
                                    {
                                        pInfo->CardSaleExtraData3.GradeLimitsArray[nTmpPosition-1].sLimitType = '1';
                                    }

                                    //insert the item type and grade number to  struct
                                    l2a(lGradeNumber,pInfo->CardSaleExtraData3.GradeLimitsArray[nTmpPosition-1].sGradeID,sizeof(pInfo->CardSaleExtraData3.GradeLimitsArray[nTmpPosition-1].sGradeID)); //4.0.11.513

                                    long lGradeLimit = 0;
                                    if(bFound)
                                    {
                                        x = atof(pParam);
                                        lGradeLimit = RoundDouble(x , 1000);
                                        l2a(lGradeLimit,pInfo->CardSaleExtraData3.GradeLimitsArray[nTmpPosition-1].sGradeAmountLimit, (sizeof(pInfo->CardSaleExtraData3.GradeLimitsArray[nTmpPosition-1].sGradeAmountLimit)));
                                    }
                                    else if(!bFound && !lGradeLimit)
                                    {
                                        //Clean all data for this grade - there is no limit 4.0.27.20 TD 219520
                                        pInfo->CardSaleExtraData3.GradeLimitsArray[nTmpPosition-1].sLimitType = ' ';
                                        memset(&pInfo->CardSaleExtraData3.GradeLimitsArray[nTmpPosition-1].sGradeID, ' ', sizeof(pInfo->CardSaleExtraData3.GradeLimitsArray[nTmpPosition-1].sGradeID));
                                        memset(&pInfo->CardSaleExtraData3.GradeLimitsArray[nTmpPosition-1].sGradeAmountLimit, ' ', sizeof(pInfo->CardSaleExtraData3.GradeLimitsArray[nTmpPosition-1].sGradeAmountLimit));
                                        bRetCode = FALSE;//Haimk		TD 465190 - Suncor Grade restriction not enforce.
                                    }
                                }

                                if ((nTmpPosition <= 0) || (nTmpPosition > MAX_GRADES))
                                {
                                    sMsg.Format("Pos=%ld",nTmpPosition);
                                    _LOGMSG.LogMsg(sMsg);
                                }
                                else
                                {									
                                    sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemDiscount%d[F]",iCountPos);
                                    bFound = GetParam(pInStr,pInStr->Find(sFiledName),&pParam,&iLen);
                                    if ((bFound) && (iLen))
                                    {														
                                        x = atof(pParam);
                                        sMsg.Format("Parse OLA ParsePositionGrades data pump%ld,code=%d, discount=%lf" ,lPumpNumber, lNACSCode, x);
                                        _LOGMSG.LogMsg(sMsg);

                                        lVal = RoundDouble(x , 1000);
                                        // Grade product discount
                                        if (cPumpItems.m_cItemSoldInfo[i].bFuelItem)
                                        {							
                                            l2a(lVal,pInfo->CardSaleExtraData2.sDiscountPerGrade[nTmpPosition- 1].sDiscount,
                                                sizeof(pInfo->CardSaleExtraData2.sDiscountPerGrade[nTmpPosition- 1].sDiscount));							
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                        bRetCode = FALSE; //TD 60450 4.0.19.504

                }
                // work always with position restriction  //4.0.15.500
                //else
                //{
                //	bRetCode = FALSE;						
                //}
            }
        }
    }
    return bRetCode;
}

void COLABase::TrimRightSpace(char *sbuffer, long lSize)
{
    for (int i=lSize -1 ; i >=0 ; i--)
    {
        if (sbuffer[i] != ' ')
        {
            sbuffer[i+1] = 0;
            break;
            
        }
    }

}
BOOL COLABase::CheckIfStrIsReal(char * pszStr,CString sInStr,long * lIdx)
{
   long lIfExist=0,lLen;
   char cFirstTav,cLastTav;
   BOOL bRetVal = FALSE;
   try
   {
       while(!bRetVal)
       {

           lIfExist = sInStr.Find(pszStr,lIfExist);
           if(lIfExist>=0)
           {
               lLen  = strlen(pszStr);
               cLastTav =  sInStr.GetAt(lIfExist+lLen);
               if(lIfExist == 0)
                   cFirstTav  =  ' ';
               else
                   cFirstTav  =  sInStr.GetAt(lIfExist-1);
               if((cFirstTav == ',')||(cFirstTav == ' '))
                   if((cLastTav == ',')||(cLastTav == ' ')||(cLastTav == '='))
                       bRetVal = TRUE;
                   else
                   {
                       bRetVal = FALSE;
                       lIfExist += lLen;
                   }
                else
                {
                    bRetVal = FALSE;
                    lIfExist += lLen;
                }
                *lIdx = lIfExist;
           }
           else
               bRetVal = TRUE;
       }

   }
   catch(...)
   {
      // we arrive here on error only !
        if(LOG_ERROR)
            _LOGMSG.LogClassMsg("COLABase","CheckIfStrIsReal",0,LOG_NONE,"",LOG_LEVEL_0);
    
   }
   
   return bRetVal;
   
}
/******************************************************************************
 Description: Start Session Inquire to collect details on special card
 Returns:     	BSTR CardSaleData.
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			16/11/2005   19:13		Start
******************************************************************************/

long COLABase::GetCardInfoSession(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData, long lFlags, long *lSesionType, long lPumpNumber)
{
    long lOlaRecSize = (lFlags & SWIPE_THREE_OLA_REC)?sizeof(CARD_SALE_ALL3):sizeof(CARD_SALE_ALL2);
    DWORD dwLastErr = 0;
    OLA_STAT ola;
    GCIRetCode retCode = GCI_OK;	

    if(PUMPSRV_NOT_INIT)
        retCode = GCI_MUST_INIT_FIRST;
    
    if(PUMP_NOT_VALID(lPumpNumber))
        retCode = GCI_INVALID_PUMP_NUMBER;

	if (FALSE == _Module.m_server.DBConnectionAvailable())
        retCode = GCI_FAILED_IO;
    else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
    {
        retCode = GCI_SERVER_IN_MIDDLE_LOCK_SHIFT;   //4.0.1.40
    }

    long lRtc = 0;
    BOOL bAskUserToTryAgain = FALSE;
        
    if ( retCode == GCI_OK )
    {
        // M.L 8/5/98 Lock PumpArray in large scope 
        //-----------------------------------------
        //EnterCriticalSection( &_Module.m_server.m_csPumpArray );

        _Module.m_server.m_cOLASrv->ResetConnectionAttempts();
        _Module.m_server.m_cOLASrv->ResetRetryAttemps(TRUE);

        long  p = lPumpNumber-1;
        _Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);

        
        if (OLA_STATE(ola.m_byState) == OLA_NONE)
        {
            PAY_AT_PUMP_INFO	info;	//4.0.18.501
            
            if (lFlags & GET_CARD_INFO_SESSION)  //4.0.11.504
            {
                PAY_AT_PUMP_INFO	infoTmp;
                char sVal[MAX_FIELD_VAL];
                long lTmpPumpNumber = 0;

                memset(&infoTmp , ' ' ,sizeof(infoTmp));
                _Module.m_server.m_cPumpArray[lPumpNumber -1].GetPAPInfo(&infoTmp);

                memcpy(info.CardSaleInfo.cardData.sAuthorizeSessionID , infoTmp.CardSaleInfo.cardData.sAuthorizeSessionID , sizeof(info.CardSaleInfo.cardData.sAuthorizeSessionID));
                memcpy(info.CardSaleExtraData2.sAuthorizeSessionID_MSB, infoTmp.CardSaleExtraData2.sAuthorizeSessionID_MSB , sizeof(info.CardSaleExtraData2.sAuthorizeSessionID_MSB));
                memcpy(info.CardSaleInfo.cardData.sTranNbr , infoTmp.CardSaleInfo.cardData.sTranNbr, sizeof(info.CardSaleInfo.cardData.sTranNbr));
                memcpy(info.CardSaleExtraData2.sFlags, infoTmp.CardSaleExtraData2.sFlags , sizeof(info.CardSaleExtraData2.sFlags));

                lTmpPumpNumber = atol((char*)infoTmp.CardSaleInfo.cardData.sPumpNumber);

                if (!lTmpPumpNumber)
                {
                    sprintf_s(sVal,MAX_FIELD_VAL,"%ld" ,lPumpNumber);
                    //info.SetXmlField(ATT_FLAGS,(char*)sVal);
                    memcpy(info.CardSaleInfo.cardData.sPumpNumber, sVal, sizeof(info.CardSaleInfo.cardData.sPumpNumber));
                }
        

                if(LOG_DETAIL_FLOW_CONTROL)
                {
                    CString str;
                
                    str.Format("Pump - %02ld GetCardinfo force add old session %ld", lPumpNumber ,info.CardSaleInfo.cardData.sAuthorizeSessionID);
                    //str.Format("Pump - %02ld GetCardinfo force add old session %ld", lPumpNumber ,info.GetXmlLongField(ATT_AUTHORIZE_SESSION_ID));
                    
                    _LOGMSG.LogMsg(str);
                }

            }
                


            if (sAccountNumber)
            {
                _bstr_t sTmpAccount(sAccountNumber, true);
                //info.SetXmlFieldWithSize(ATT_ACCOUNT_NUMBER,(char *)sTmpAccount,ACCOUNT_NUM_SIZE);
                memcpy(info.CardSaleInfo.cardData.sAccountNumber , (const char *)sTmpAccount, min(sTmpAccount.length() ,sizeof(info.CardSaleInfo.cardData.sAccountNumber)));	// 4.0.25.80 TD 142857	
            }

            if (sTrack1)
            {
                _bstr_t sTmpTrack1(sTrack1, true);
                //info.SetXmlFieldWithSize(ATT_TRACK1_DATA,(char *)sTmpTrack1,TRACK_1_DATA_SIZE);
                memcpy(info.CardSaleInfo.cardData.sTrack1Data , (const char *)sTmpTrack1, min(sTmpTrack1.length() ,sizeof(info.CardSaleInfo.cardData.sTrack1Data)));			// 4.0.25.80 TD 142857	
            }

            if (sTrack2)
            {
                CString sMsg;
                _bstr_t sTmpTrack2(sTrack2, true);
                //info.SetXmlFieldWithSize(ATT_TRACK2_DATA,(char *)sTmpTrack2,TRACK_2_DATA_SIZE);
                memcpy(info.CardSaleInfo.cardData.sTrack2Data , (const char *)sTmpTrack2, min(sTmpTrack2.length() ,sizeof(info.CardSaleInfo.cardData.sTrack2Data)));			// 4.0.25.80 TD 142857	
            }				


            if (lFlags & SWIPE_THREE_OLA_REC)
            {

                char sFlagsVal[MAX_FIELD_VAL]; 
                memset(sFlagsVal,0,sizeof(sFlagsVal));
                //info.GetXmlField((char*)ATT_FLAGS,(char*)sFlagsVal);
                
                if (!_Module.m_server.GetOLAFlags((BYTE*)info.CardSaleExtraData2.sFlags ,FL_PAP_AS_THIRD_REC))
				{
                    long lTmp = a2l(info.CardSaleExtraData2.sFlags ,sizeof(info.CardSaleExtraData2.sFlags))  + FL_PAP_AS_THIRD_REC;
                    l2a(lTmp ,info.CardSaleExtraData2.sFlags ,sizeof(info.CardSaleExtraData2.sFlags)); 
                }			

            }

            
            ola.m_lSessionNumber =0;
            ola.m_byState = OLA_REQUEST_TO_SEND | SESSION_INQUIRY;			
            
            _Module.m_server.m_cPumpArray[lPumpNumber-1].SetOlaStat(&ola); 	
            _Module.m_server.m_cPumpArray[lPumpNumber -1].SetPAPInfo(&info);			
            _Module.m_server.SavePumpInfo(p+1); 

            *lSesionType = ST_SESSION;

            CString str;
            str.Format("COLABase::GetCardInfoSession OLA_REQUEST_TO_SEND|SESSION_INQUIRY ST_SESSION SessionNumber=%ld", ola.m_lSessionNumber);
            _LOGMSG.LogMsg(str);
        }
        else
        {
            retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
            _LOGMSG.LogMsg("COLABase::GetCardInfoSession GCI_PAP_REQUEST_OUT_OF_SYNC");
        }
    }


    return retCode;
}

//4.0.13.501
long COLABase::GetCardInfoSessionEx(long lClientID, BSTR sInBuffer, BSTR *sRetData, long lFlags)
{
    long				lOlaRecSize = (lFlags & SWIPE_THREE_OLA_REC)?sizeof(CARD_SALE_ALL3):sizeof(CARD_SALE_ALL2);
    CARD_SALE_ALL3		cCardSaleALL;
    PAY_AT_PUMP_INFO	cInfo;
    OLA_STAT			ola;
    GCIRetCode			retCode = GCI_OK;	
    long				lPumpNumber; 
    long				lTmpRetCode;
    long				lRtc = 0;

	{
		CString str;
		str.Format("COLABase::GetCardInfoSessionEx() - lClientID %ld", lClientID);
		_Module.m_server.m_cLogger.LogMsg(str);
	}

    memset(&cCardSaleALL,' ', sizeof(cCardSaleALL));
    memset(&cInfo,' ', sizeof(cInfo));

    if(PUMPSRV_NOT_INIT)
        retCode = GCI_MUST_INIT_FIRST;
    
    if(FALSE == _Module.m_server.DBConnectionAvailable())
        retCode = GCI_FAILED_IO;
    else if (IS_SERVER_IN_MIDDLE_LOCK_SHIFT)
    {
        retCode = GCI_SERVER_IN_MIDDLE_LOCK_SHIFT;   //4.0.1.40
    }

    //4.0.21.14 69073 start
    CARD_SALE_ALL_CHIP_EXTRA_DATA	cardDataAll3ChipExtra;	
    memset(&cardDataAll3ChipExtra, ' ', sizeof(cardDataAll3ChipExtra));

    int iLen = 0;

    lTmpRetCode = _Module.m_server.m_cXmlConvertorEx.GetStructCardSaleAll3ChipExtra(sInBuffer, lOlaRecSize, &cardDataAll3ChipExtra, &iLen);	//4.0.20.503 66960
    memcpy(&cCardSaleALL, &cardDataAll3ChipExtra.cCardSaleAll3, sizeof(cardDataAll3ChipExtra.cCardSaleAll3));

    if(lTmpRetCode)	//4.0.21.15 69073
        retCode = GCI_FAILED_IO;
    else
    {
        lPumpNumber = a2l(cCardSaleALL.CardSaleAll.data.sPumpNumber,sizeof(cCardSaleALL.CardSaleAll.data.sPumpNumber));
        
        if(PUMP_NOT_VALID(lPumpNumber))
            retCode = GCI_INVALID_PUMP_NUMBER;
    }	
        
    if ( retCode == GCI_OK )
    {
        // M.L 8/5/98 Lock PumpArray in large scope 
        //-----------------------------------------
        //EnterCriticalSection( &_Module.m_server.m_csPumpArray );

        _Module.m_server.m_cOLASrv->ResetConnectionAttempts();
        _Module.m_server.m_cOLASrv->ResetRetryAttemps(TRUE);

        long  p = lPumpNumber-1;
        _Module.m_server.m_cPumpArray[p].GetOlaStat(&ola);

        if (OLA_STATE(ola.m_byState) == OLA_NONE)
        {
            _Module.m_server.m_cCommonFunction.ConvertCardSaleAll3ToPayAtPumpInfo(cInfo, cCardSaleALL);	//4.0.20.507

            if (lFlags & SWIPE_THREE_OLA_REC)
            {
                if (!_Module.m_server.GetOLAFlags((BYTE*)cInfo.CardSaleExtraData2.sFlags ,FL_PAP_AS_THIRD_REC))
                {//4.0.20.503 66960
                    long lTmp = a2l(cInfo.CardSaleExtraData2.sFlags, sizeof(cInfo.CardSaleExtraData2.sFlags)) + FL_PAP_AS_THIRD_REC;
                    l2a(lTmp, cInfo.CardSaleExtraData2.sFlags, sizeof(cInfo.CardSaleExtraData2.sFlags));
                }			
            }

            ola.m_lSessionNumber = 0;
            ola.m_byState = OLA_REQUEST_TO_SEND | SESSION_INQUIRY;			
            
            _Module.m_server.m_cPumpArray[p].SetOlaStat(&ola); 	

            _Module.m_server.m_cPumpArray[p].SetPAPInfo(&cInfo);		//4.0.20.503	66960
            _Module.m_server.SavePumpInfo(p+1); 

            //The session will return later, asynchrony.
            retCode = GCI_CREATE_OLA_SESSION;

            CString str;
            str.Format("COLABase::GetCardInfoSessionEx GCI_CREATE_OLA_SESSION SessionNumber=%ld", ola.m_lSessionNumber);
            _LOGMSG.LogMsg(str);
        }
        else
        {
            retCode = GCI_PAP_REQUEST_OUT_OF_SYNC;
            _LOGMSG.LogMsg("COLABase::GetCardInfoSessionEx GCI_PAP_REQUEST_OUT_OF_SYNC");
        }
    }

    return retCode;
}

/******************************************************************************
 Description: Send original price in complete session 
 Returns:     	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			16/08/2006   19:13		Start
 marik			07/06/2011				Use the grade  price report from transaction level	
******************************************************************************/
void COLABase::AddedCompletePriceInformation(long lPumpNumber, long &lOffset, char *pOutBuffer, long &lOLAPumpServiceMode, long &lGradeRestrict ,long &lGrade  )   //4.0.28.500 - TD 238829
{
    PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);
    long lCount = cPumpItems.lCounter;
    char sFiledName[MAX_FIELD_NAME]; 
    long lNumOfItems = 0;//4.0.11.98	fix this function TD 14868
    long lPriceCash = 0 , lPriceCredit =0 ;//4.0.11.121

    // Add item list 
    if (lPumpNumber && lCount)
    {
        for (int i = 0; i < cPumpItems.lCounter; i++)
        {	
            // send only full information...  
            if (cPumpItems.m_cItemSoldInfo[i].lGrade)
            {
                if (cPumpItems.m_cItemSoldInfo[i].bFuelItem)
                {
                    if ((!lGradeRestrict) ||    //4.0.7.500
                        (cPumpItems.m_cItemSoldInfo[i].lGrade == lGradeRestrict))
                    {
                    
                        sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemCode%d[S]", lNumOfItems);
                        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *) sFiledName,	(BYTE *)"%s=%03ld,",		_Module.m_server.GetGradeCode((short)cPumpItems.m_cItemSoldInfo[i].lGrade));			
                        
                        sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemPrice%d[F]", lNumOfItems);

                        //4.0.28.500 - TD 238829
//						if (lOLAPumpServiceMode  == FULL_SERVICE_MODE)
//							lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%ld.%03ld,",cPumpItems.m_cItemSoldInfo[i].lPriceCreditFullService/1000,cPumpItems.m_cItemSoldInfo[i].lPriceCreditFullService%1000);				
//						else
//							lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%ld.%03ld,",cPumpItems.m_cItemSoldInfo[i].lPriceCreditSelfService/1000,cPumpItems.m_cItemSoldInfo[i].lPriceCreditSelfService%1000);

                        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%ld.%03ld,",lGrade / 1000,lGrade % 1000);  //4.0.28.500 - TD 238829

                        //4.0.11.502
                        //Attached is the new fields that will be send to OLA in order to report low price & high price.   
                        
						if (_Module.m_server.GetPriceProtectionActive() == PRICE_PROTECTION_ACTIVE_INSIDE_TIME_FRAME)
						{
							lPriceCash = _Module.m_server.GetGradePriceFromArray(cPumpItems.m_cItemSoldInfo[i].lGrade, PRICE_PROTECTION_LOWEST_PRICE);
						}
						else 
						{
							GetGradePrice((short) cPumpItems.m_cItemSoldInfo[i].lGrade ,lPriceCash, lPriceCredit, lOLAPumpServiceMode);//4.0.11.122
						}
						
						if (_Module.m_server.IsCurrentRegion(REGION_USA)) 
						{
							sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPriceLow%d[F]", lNumOfItems);//4.0.11.50
						}
						else 
						{
							sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPriceLow%d[S]", lNumOfItems);//4.0.11.50
						}
                        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) sFiledName, (BYTE *)"%s=%ld.%03ld,", lPriceCash / 1000, lPriceCash % 1000);//4.0.11.122
						
						if (_Module.m_server.IsCurrentRegion(REGION_USA))
						{
							sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPriceHigh%d[F]", lNumOfItems);//4.0.11.50
						}
						else
						{
							sprintf_s(sFiledName, MAX_FIELD_NAME, "ItemPriceLow%d[S]", lNumOfItems);//4.0.11.50
						}
                        lOffset += SetParam(pOutBuffer + lOffset, (BYTE *) sFiledName, (BYTE *)"%s=%ld.%03ld,", lPriceCredit / 1000, lPriceCredit % 1000);//4.0.11.122

                        sprintf_s(sFiledName,MAX_FIELD_NAME, "ItemIsFuel%d[B]", lNumOfItems);
                        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,(BYTE *)"%s=%s,",(BYTE *)"Y",1);

                        sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemType%d[S]",lNumOfItems);

                        // required only for pilot   //4.0.9999.0
                        if (_Module.m_server.m_cPumpArray[lPumpNumber -1].GetDieselPumpFlags())
                            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,(BYTE *)"%s=%s,",(BYTE *)"C",1);
                        else
                            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,(BYTE *)"%s=%s,",(BYTE *)"P",1);

                        lNumOfItems++;  //4.0.11.100

                    }

                }
                else
                {
                    sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemIsFuel%d[B]",lNumOfItems);
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,(BYTE *)"%s=%s,",(BYTE *)"N",1);

                    sprintf_s(sFiledName,MAX_FIELD_NAME,"ItemCode%d[S]",lNumOfItems);
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sFiledName,	(BYTE *)"%s=%03ld,",	cPumpItems.m_cItemSoldInfo[i].lGrade);			
                    lNumOfItems++;  //4.0.11.100

                }		
            }
        }	
        
    }		
   return;
}
/******************************************************************************
 Description: Detect exact ola string in buffre in case  string appers more then once in buffer, 
              example :ClubCardTrack2[S] &  Track2[S].

 Returns:     0 - Found string  , -1 .Not found
 Parameters:   sFiledName - field name , 
               pInStr - string buffer 
               iPos - field poistion found. 

 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			01/08/2006   19:13		Start
******************************************************************************/

long COLABase::GetParamLenEx(char *sFiledName, CString *pInStr, int iPos)
{
    int iFound=iPos;
    int iFoundTmp=0;
    long lFoundExactParam  = GetParamLen(pInStr,iFound);

    if (lFoundExactParam )  //4.0.14.200
    {
        iFound++;
        do
        {
            // not found exact filed check for next fields. 
            iFoundTmp = pInStr->Find(sFiledName,iFound);

            //Reach to end buffer without find the exact string
            if (iFoundTmp < 0)
                break;

            lFoundExactParam = GetParamLen(pInStr,iFoundTmp);

            iFound =iFoundTmp +1 ;
            if (lFoundExactParam == 0)
                break;

        }
        while(iFound != -1) ;
    }

    return lFoundExactParam;
}

/*******************************************************************
 Method:    IsAddPreAuthInfo
 Description: Returns true if preAuthorize flags are needed
              In the 4th case we send the flags in case OLA requested to send more params.
  FullName:  COLABase::IsAddPreAuthInfo
  Access:    public 
  Returns:   BOOL
  Qualifier: const
  Parameter: short sByOlaStat

WHO			WHEN                    WHAT
/-----------------------------------------------------------------
 Omer			08/05/2011   19:13		Start
*******************************************************************/

BOOL COLABase::IsAddPreAuthInfo(short sByOlaStat) const
{
    return ( 
        (m_bPreAuthIFSF   && (SESSION_TYPE(sByOlaStat) == SESSION_AUTHORIZE) && (OLA_STATE(sByOlaStat) == OLA_REQUEST_TO_SEND)) ||
        (_Module.m_server.GetOLAReportSoldItemOnPump() && (SESSION_TYPE(sByOlaStat) == SESSION_AUTHORIZE) && (OLA_STATE(sByOlaStat) == OLA_REQUEST_TO_SEND)) || //4.0.10.502
        (_Module.m_server.GetSupportNACSRestriction()  && (SESSION_TYPE(sByOlaStat) == SESSION_AUTHORIZE) && (OLA_STATE(sByOlaStat) == OLA_REQUEST_TO_SEND)) ||	//49339
        (_Module.m_server.GetOLAReportSoldItemOnPump() && (SESSION_TYPE(sByOlaStat) == SESSION_AUTHORIZE) && (OLA_STATE(sByOlaStat) == OLA_REQUEST_TO_SEND_MORE) && m_bAddPreAuthInfo));

        
}   // TD 235737 4.0.27.501



/******************************************************************************
 Description:	Find the Grade code in the item lits and return the item index 	
 Returns:      	TRUE/FALSE  ; - TURE - found the nacs code in list
 Parameters:   	long lPumpNumber,
                long lGrade 
                long lIndex -  NACS code location in product array.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			23/03/2012   8:33		Start	//4.0.23.1250
******************************************************************************/  
BOOL COLABase::ConvertGradeItemsToIndex(long &lPumpNumber, long &lGrade, long &lIndex)
{
    PumpItems cPumpItems = m_cForecourtProductList.operator =(lPumpNumber);
    BOOL bRet = FALSE;
    
    for (int i=0 ; (i < MAX_ITEMS) && ( i < cPumpItems.lCounter); i++)
    {
        if (cPumpItems.m_cItemSoldInfo[i].lGrade == lGrade)
        {
            lIndex = i;
            bRet = TRUE;
            break;
        }
    }
    
    return bRet;
}

long COLABase::GetItemPrice(long lPumpNumber, PumpItems *pInfo, int iItem) //4.0.23.1250
{
    long lUnitPrice = 0;
    
    long lServiceMode = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetModeService();	//the default is self service = 0
    
    
    if (lServiceMode == 0)	//SelfService
        lUnitPrice = pInfo->m_cItemSoldInfo[iItem].lPriceCreditSelfService;
    else
        lUnitPrice = pInfo->m_cItemSoldInfo[iItem].lPriceCreditFullService;
    
    return lUnitPrice;
}


/******************************************************************************
 Description:	Build the Odometer from card sale data and returns it as output
 Returns:      	long - the odometer in long format
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 OmerT			05/12/13		TD 389050
******************************************************************************/ 
long COLABase::GetOdometerFromCardSale( CARD_SALE_EXTRA_DATA4_ * pCardDataExtra4, CARD_SALE_EXTRA_DATA * pCardDataExtra, CARD_SALE_DATA * pCardData )
{
    BYTE sTmpOdometer[9];
    memset(sTmpOdometer,' ' ,sizeof(sTmpOdometer));	
    memcpy(sTmpOdometer , &pCardDataExtra4->sOdometerExt2 , sizeof(pCardDataExtra4->sOdometerExt2));	//4.0.22.511 74797
    memcpy(&sTmpOdometer[1] , pCardDataExtra->sOdometerExt , sizeof(pCardDataExtra->sOdometerExt));
    memcpy(&sTmpOdometer[3] , pCardData->sOdometer , sizeof(pCardData->sOdometer));
    
    return a2l(sTmpOdometer,sizeof(sTmpOdometer));
    
}


void COLABase::Destruct()
{
    CString str;
    str.Format("Start COLABase::Destruct");
    _Module.m_server.m_cLogger.LogMsg(str);
    //SR 362184: PumpSrv freeze on COLABase destructor
    
	//if ( m_pbsrOlaBuffer != NULL )		// 4.0.25.80 TD 142857	
    //{
    //    SysFreeString(m_pbsrOlaBuffer);
    //    m_pbsrOlaBuffer = NULL;
    //}
    

    //ola12345
    _Module.m_server.DeleteOLAThreadInfo();

    str.Format("Delete CriticalSection details m_csOLASrv: LockCount:%d, RecursionCount:%d,  ",m_csOLASrv.LockCount, m_csOLASrv.RecursionCount);
    _Module.m_server.m_cLogger.LogMsg(str);
    
    str.Format("Delete CriticalSection details m_csRetry: LockCount:%d, RecursionCount:%d,  ",m_csOLASrv.LockCount, m_csRetry.RecursionCount);
    _Module.m_server.m_cLogger.LogMsg(str);

    DeleteCriticalSection(&m_csOLASrv);
    DeleteCriticalSection(&m_csRetry);//4.0.3.38

    str.Format("End COLABase::Destruct");
    _Module.m_server.m_cLogger.LogMsg(str);
}

void COLABase::ParseAmountLimit(CString * pInStr, char * pParam,PAY_AT_PUMP_INFO *pInfo)
{
    int iLen;
    BOOL bFound = GetParam(pInStr,pInStr->Find("AmountLimit[F]"),&pParam,&iLen);
    if(bFound) 
    {
        if(iLen)
        {
            double x = atof(pParam);
            //In Israel we do not use the decimal point there for all the 5 characeters are for Integer number.
            //therefore there is no need to multiply by 1000 to get the 3 decimal.
            //2.9.2.2 
            long lAmountLimit =  RoundDouble(x , 100);				//4.0.3.61

            if (_Module.m_server.IsNeedCreditLimitCheck())			//3.2.1.71
            {
                long lPumpNumber = a2l(pInfo->CardSaleInfo.cardData.sPumpNumber,sizeof(pInfo->CardSaleInfo.cardData.sPumpNumber));
                long lValue = _Module.m_server.GetLimit(PRICE_LEVEL_CREDIT,lPumpNumber);
                long lDeviceLimit =0;

                lValue/=10;  //4.0.1.40

                lDeviceLimit = lAmountLimit;

                if ((lDeviceLimit > lValue))
                {
                    lAmountLimit = lValue;
                }
                else
                    lAmountLimit = lDeviceLimit;				
            }

            BYTE sTmpValue[11] = {0};
            l2a0(lAmountLimit, sTmpValue , 10);
            memcpy(pInfo->CardSaleInfo.cardData.sAuthAmt , &sTmpValue[5] ,sizeof(pInfo->CardSaleInfo.cardData.sAuthAmt));
            memcpy(pInfo->CardSaleInfo.extraData.sAuthAmt_Msb , sTmpValue ,sizeof(pInfo->CardSaleInfo.extraData.sAuthAmt_Msb));

            if(LOG_DETAIL_FLOW_CONTROL)
            {
                CString str;
                str.Format("Authorize amount is:: =auth=%ld, sAuthAmtH=%.5s, sAuthAmtl=%.5s" ,lAmountLimit , pInfo->CardSaleInfo.extraData.sAuthAmt_Msb ,pInfo->CardSaleInfo.cardData.sAuthAmt);
                _LOGMSG.LogMsg("COLABase","ParseData",0,LOG_NONE,str);
            }
        }
        else
        {
            l2a(0L,pInfo->CardSaleInfo.cardData.sAuthAmt,sizeof(pInfo->CardSaleInfo.cardData.sAuthAmt));
            l2a(0L,pInfo->CardSaleInfo.extraData.sAuthAmt_Msb,sizeof(pInfo->CardSaleInfo.extraData.sAuthAmt_Msb));//3.2.0.73			
        }		
    }
}

void COLABase::BuildServiceMode( long lPumpIndex, long & lOffset, char * pOutBuffer )
{
    CString sServiceMode;
    long lServiceMode= _Module.m_server.m_cPumpArray[lPumpIndex].GetStat().GetServiceMode();
    if(lServiceMode == FULL_SERVICE_MODE)
        sServiceMode = "S";
    else // lServiceMode SELF_SERVICE_MODE
        sServiceMode = "F";

    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemServiceMode[S]",(BYTE *)"%s=%.1s,",(BYTE *)sServiceMode.GetBuffer(0),1);	
}
void COLABase::BuildItems(long lPumpIndex, CPumpTransact *pTrs, long & lOffset, char * pOutBuffer)
{
    if (pTrs == NULL) // 4.0.32.671  - TD 464219
    {
        _LOGMSG.LogClassMsg("COLABase", "BuildItems", 0, LOG_NONE, "NO transaction, skipping build items!");
        return;
    }

    long lItemCount = 0;

    long lTransTaxAmount = 0;

    lItemCount += BuildFuelItem(lPumpIndex, pTrs, lOffset, pOutBuffer, lTransTaxAmount);

    lItemCount += BuildCarWashItem(lPumpIndex, lOffset, pOutBuffer, lTransTaxAmount);

    lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCount[I]", (BYTE *)"%s=%ld,", lItemCount);

    lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"TransTaxAmount[F]", (BYTE *)"%s=%ld.%03ld,", lTransTaxAmount / 1000, lTransTaxAmount % 1000);
}

long COLABase::BuildFuelItem(long lPumpIndex, CPumpTransact *pTrs, long & lOffset, char * pOutBuffer, long &lTransTotalTax)
{
    if (pTrs == NULL) // 4.0.32.671  - TD 464219
        return 0;

    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemDesc0[S]",(BYTE *)"%s=%.20s,",(BYTE *)
        _Module.m_server.GetGradeName(pTrs->m_nGrade).GetBuffer(0),_Module.m_server.GetGradeName(pTrs->m_nGrade).GetLength());

    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCode0[S]",(BYTE *)"%s=%03ld,",_Module.m_server.GetGradeCode(pTrs->m_nGrade));

    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeAmt0[F]",(BYTE *)"%s=%ld.%03ld,",pTrs->m_lRoundedValue/1000,pTrs->m_lRoundedValue%1000);  //4.0.32.427

    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemPrice0[F]",(BYTE *)"%s=%ld.%03ld,",	pTrs->m_lGradePrice/1000 ,pTrs->m_lGradePrice%1000);

    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemWeight0[F]",(BYTE *)"%s=%ld.%03ld,",pTrs->m_lRoundedVolume/1000,pTrs->m_lRoundedVolume%1000);

    DWORD dwGardeTaxMap = _Module.m_server.GetGradeTaxLink(pTrs->m_nGrade);

    int iTaxNumber = 0;
    for(int iTaxIndex = 0; iTaxIndex < MAX_TAXES; iTaxIndex++)
    {
        if(dwGardeTaxMap & (DWORD)(1<<iTaxIndex))
        {
            TAX_PROPERTIES cTaxPropertis;
            _Module.m_server.FillTaxDetails(iTaxIndex, cTaxPropertis);

            long lTaxAmount = _Module.m_server.GetFuelTaxAmountFormCardSaleData(lPumpIndex,iTaxNumber + 1);

            CString sTaxString;
            sTaxString.Format("ItemTax0_%d[F]",iTaxNumber);

            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sTaxString.GetBuffer(0),(BYTE *)"%s=%ld.%03ld,",lTaxAmount/1000,lTaxAmount%1000);

            sTaxString.Format("ItemTaxName0_%d[S]",iTaxNumber);

            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)sTaxString.GetBuffer(0),(BYTE *)"%s=%.16s,",(BYTE *)cTaxPropertis.sTaxName,sizeof(cTaxPropertis.sTaxName));

            sTaxString.Format("ItemTaxID0_%d[S]", iTaxNumber);
            char sStr[MAX_NAME_LENGTH] = { 0 };
            sprintf_s(sStr, MAX_NAME_LENGTH, "%ld", cTaxPropertis.lTaxId);
            lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sTaxString.GetBuffer(0), (BYTE *)"%s=%s,", (BYTE*)sStr, strlen(sStr));

            lTransTotalTax += lTaxAmount;

            iTaxNumber++;
        }
    }

    if(iTaxNumber)
    {
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemTaxCount0[I]",(BYTE *)"%s=%ld,",iTaxNumber);
    }

    if (pTrs->m_lRoundedValue)
        return 1;
    else 
        return 0;
}

long COLABase::BuildCarWashItem( long lPumpIndex, long & lOffset, char * pOutBuffer, long &lTransTotalTax )
{
    PAY_AT_PUMP_INFO pap;
    _Module.m_server.m_cPumpArray[lPumpIndex].GetPAPInfo(&pap);

    long lQtyCarwash = a2l((BYTE *)pap.CardSaleInfo.extraData.SALES[0].sQty,sizeof(pap.CardSaleInfo.extraData.SALES[0].sQty));

    long lUnitPrice = a2l((BYTE *)pap.CardSaleInfo.extraData.SALES[0].sUnitPrice,sizeof(pap.CardSaleInfo.extraData.SALES[0].sUnitPrice)) *(_Module.m_server.GetCarWashScalarFactor());

    long lAmountCarwash = lUnitPrice * lQtyCarwash;

    if (lAmountCarwash == 0)
        return 0; 
    
    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemDesc1[S]",(BYTE *)"%s=%.5s,",(BYTE *)pap.CardSaleInfo.extraData.SALES[0].sProdDesc,sizeof(pap.CardSaleInfo.extraData.SALES[0].sProdDesc));

    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCode1[S]",(BYTE *)"%s=%.03ld,",(BYTE *)"102",3);

    lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)"ItemCodeQty1[I]", (BYTE *)"%s=%ld,", lQtyCarwash);

    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeAmt1[F]",(BYTE *)"%s=%ld.%03ld,",lAmountCarwash/1000,lAmountCarwash%1000);  //4.0.32.427

    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemPrice1[F]",(BYTE *)"%s=%ld.%03ld,",	lUnitPrice/1000 ,lUnitPrice%1000);

    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemWeight1[F]",(BYTE *)"%s=%02ld,",lQtyCarwash);

    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemUPC1[S]",(BYTE *)"%s=%.15s,",(BYTE *)pap.CardSaleExtraData6.CarWashItem.sUpc,sizeof(pap.CardSaleExtraData6.CarWashItem.sUpc));

    long lDPTProdCode = a2l(&pap.CardSaleInfo.extraData.sCarWashProgram, sizeof(pap.CardSaleInfo.extraData.sCarWashProgram)); //4.0.10.230
    long lCarWashIndex = _Module.m_server.GetWashProgramIndex(lDPTProdCode);
    DWORD dWashProgramTaxLink = _Module.m_server.GetWashProgramTaxLink(lCarWashIndex);

    int iTaxIndex = 0;
    int iTaxNumber = 1;
    for (; iTaxNumber <= 2; iTaxNumber++)
    {
        long lTaxAmount = _Module.m_server.GetCarWashTaxFromCardSaleData(lPumpIndex, iTaxNumber);

        if (lTaxAmount > 0)
        {
            CString sTaxString;
            sTaxString.Format("ItemTax1_%d[F]", iTaxNumber);

            lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sTaxString.GetBuffer(0), (BYTE *)"%s=%ld.%03ld,", lTaxAmount / 1000, lTaxAmount % 1000);

            sTaxString.Format("ItemTaxName1_%d[F]", iTaxNumber);

            if (iTaxNumber == 1)
                lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sTaxString.GetBuffer(0), (BYTE *)"%s=%.10s,", (BYTE *)pap.CardSaleExtraData6.CarWashItem.sTaxName1, sizeof(pap.CardSaleExtraData6.CarWashItem.sTaxName1));
            else
            {
                lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sTaxString.GetBuffer(0), (BYTE *)"%s=%.10s,", (BYTE *)pap.CardSaleExtraData6.CarWashItem.sTaxName2, sizeof(pap.CardSaleExtraData6.CarWashItem.sTaxName2));
                iTaxIndex++;
            }
            for (; iTaxIndex < MAX_TAXES; iTaxIndex++)
            {
                if (dWashProgramTaxLink & (DWORD)(1 << iTaxIndex))
                    break;
            }

            TAX_PROPERTIES cTaxPropertis;
            _Module.m_server.FillTaxDetails(iTaxIndex, cTaxPropertis);
            sTaxString.Format("ItemTaxID1_%d[S]", iTaxNumber);

            char sStr[MAX_NAME_LENGTH] = { 0 };
            sprintf_s(sStr, MAX_NAME_LENGTH, "%ld", cTaxPropertis.lTaxId);
            lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)sTaxString.GetBuffer(0), (BYTE *)"%s=%s,", (BYTE*)sStr, strlen(sStr));

            lTransTotalTax += lTaxAmount;
        }
    }

    if(iTaxNumber > 1)
    {
        lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemTaxCount1[I]",(BYTE *)"%s=%ld,",iTaxNumber);
    }

    return 1;
}

void COLABase::BuildTransAmount(long lPumpNumber,long lTotalAmount,short byOlaStat,PAY_AT_PUMP_INFO * pInfo , IN OUT long & lOffset, OUT char * pOutBuffer)
{
    long lPumpIndex = lPumpNumber-1;
    CString str;

	if (_Module.m_server.GetTwoTendersInTransaction()) // 4.0.32.790
    {
        if (SESSION_TYPE(byOlaStat) & SESSION_AUTHORIZE)
        {
            long lNumberOfTendersAuthrozied = _Module.m_server.GetNumberOfTendersAuthotized(lPumpIndex);
            if(lNumberOfTendersAuthrozied == 0) // first tender
            {
                long lAmountToAuthorize = _Module.m_server.GetRequstedAuthorizeAmount(lPumpNumber);
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TransAmount[F]",	(BYTE *)"%s=%ld.%03ld," ,lAmountToAuthorize/1000,lAmountToAuthorize%1000);

                str.Format("Authorizing first tender: TransAmount[F]=%d.%03d", lAmountToAuthorize / 1000, lAmountToAuthorize % 1000);
                _LOGMSG.LogClassMsg("COLABase","BuildTransAmount",0,LOG_NONE,str);
            }
            else if(lNumberOfTendersAuthrozied == 1) // second tender
            {
                long lReamingBalance = _Module.m_server.GetRemainingBalance(lPumpNumber)*10;
                if(lReamingBalance)
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"TransAmount[F]",	(BYTE *)"%s=%ld.%03ld,",lReamingBalance/1000,lReamingBalance%1000);

                str.Format("Authorizing second Tender: TransAmount[F]=%d.%03d",lReamingBalance / 1000, lReamingBalance % 1000);
                _LOGMSG.LogClassMsg("COLABase","BuildTransAmount",0,LOG_NONE,str);
            }	
        }
    }
    else
    {
		
		bool bSessionComplete = (SESSION_TYPE(byOlaStat) == SESSION_COMPLETE);

		if ((SESSION_TYPE(byOlaStat) == SESSION_COMPLETE) && (!lTotalAmount))
		{
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)TransAmountField(bSessionComplete), (BYTE *)"%s=%ld.%03ld,", lTotalAmount / 1000, lTotalAmount % 1000);
		}
		else if (lTotalAmount)
		{
			lOffset += SetParam(pOutBuffer + lOffset, (BYTE *)TransAmountField(bSessionComplete), (BYTE *)"%s=%ld.%03ld,", lTotalAmount / 1000, lTotalAmount % 1000);
		}

    }
}

void COLABase::AddItemsToOla( PAY_AT_PUMP_INFO * pInfo, long &lOffset, char * pOutBuffer, long lPumpNumber, short byOlaStat, long lValue, long &lVolume, long &lGradePrice, short &nGrade, unsigned long lFuelLoyalApp, long &lLoyaltyTotalAmount )
{
    //Currently the only item we sale at the pump is car wash.
    //The way the list of the item is build here is far from being general, the way it's
    //currently work there is always at least one item with is fuel. there could be
    //two more items at that list 1.car wash 2. discount.
    //If we will start sale more items at the pump, this part of the code must be
    //changed to support it.

    //Check for more item, other then fuel.
    long lItemCount = 1L;//There at list one item - fuel
    if ( a2l(&pInfo->CardSaleInfo.extraData.sNumberOfExtraSale,sizeof(pInfo->CardSaleInfo.extraData.sNumberOfExtraSale)) )
    {
        //We got car wash
        lItemCount++;
        //Check if also got discount
        if (a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sDiscount,sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDiscount)) )
        {
            lItemCount++;
        }
    }

    //4.0.10.506
    if (a2l(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode ,sizeof(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode)))  //4.0.10.491
    {
        lItemCount++;
    }

    //Number of item
    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCount[I]",	(BYTE *)"%s=%ld,",lItemCount);

    if(FALSE == PUMP_NOT_VALID(lPumpNumber))	//4.0.19.500 58863
    {			
        if (_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetDieselPumpFlags())
        {
            if ( (OLA_STATE(byOlaStat) == OLA_REQUEST_TO_SEND) && (SESSION_TYPE(byOlaStat) == SESSION_COMPLETE))
            {
                //4.0.10.49
                //Reefer sales/
                long lReeferValue = a2l(pInfo->CardSaleExtraData3.sReeferTotal , sizeof(pInfo->CardSaleExtraData3.sReeferTotal));
                long lReeferVolume = a2l(pInfo->CardSaleExtraData3.sReeferVolume , sizeof(pInfo->CardSaleExtraData3.sReeferVolume));
                long lReeferPrice = a2l(pInfo->CardSaleInfo.extraData.SALES[3].sUnitPrice, sizeof(pInfo->CardSaleInfo.extraData.SALES[3].sUnitPrice));

                if(LOG_DETAIL_FLOW_CONTROL)
                {
                    CString str;
                    str.Format("Report reefer details value=%ld ,volume=%ld ,price=%ld , Grade=%ld" , lReeferValue,lReeferVolume ,lReeferPrice , a2l(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode ,sizeof(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode)));
                    _LOGMSG.LogMsg("COLABase","BuildRequest",0,LOG_NONE,str);
                }

                long lAmount = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice,sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice)) * (_Module.m_server.GetCarWashScalarFactor());
                long lQty = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sQty,sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sQty));
                long lDiscount = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sDiscount,sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDiscount)) * 10;
                long lTax  = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[2].sTax,sizeof(pInfo->CardSaleInfo.extraData.SALES[2].sTax)) * 10;        //3.2.0.9	

                if (a2l(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode ,sizeof(pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode)))
                {
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCode1[S]",		(BYTE *)"%s=%03ld,",	_Module.m_server.GetGradeCode((short)a2l( pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode , sizeof( pInfo->CardSaleInfo.extraData.SALES[3].sDPTProdCode))));
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeAmt1[F]",	(BYTE *)"%s=%ld.%03ld,",lReeferValue/1000,lReeferValue%1000);   //4.0.10.49
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeQty1[I]",	(BYTE *)"%s=%ld,",1L);//This value will be a transaction parameter as we sell items at the Pump.
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemWeight1[F]",	(BYTE *)"%s=%ld.%03ld,",lReeferVolume/1000,lReeferVolume%1000);
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemIsFuel1[B]",	(BYTE *)"%s=%s,",(BYTE *)"Y",1);
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemPrice1[F]",		(BYTE *)"%s=%ld.%03ld,",lReeferPrice/1000,lReeferPrice%1000);
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemType1[S]",(BYTE *)"%s=%s,",(BYTE *)"C",1);
                }

                long lFuelSale = lValue - lReeferValue;
                lVolume -= lReeferVolume;

                if (lItemCount == 2)   //4.0.10.49
                {
                    lGradePrice = a2l(pInfo->CardSaleExtraData2.sGradePrice, sizeof(pInfo->CardSaleExtraData2.sGradePrice));
                    nGrade = a2i(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode ,sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDPTProdCode));  
                }

                if(LOG_DETAIL_FLOW_CONTROL)
                {
                    CString str;
                    str.Format("Report diesel  details value=%ld ,volume=%ld ,price=%ld, Grade=%d" , lFuelSale,lVolume ,lGradePrice ,nGrade);
                    _LOGMSG.LogMsg("COLABase","BuildRequest",0,LOG_NONE,str);
                }

                //Fuel is alway there, and it always the first.
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCode0[S]",		(BYTE *)"%s=%03ld,",	_Module.m_server.GetGradeCode(nGrade));
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeAmt0[F]",	(BYTE *)"%s=%ld.%03ld,",lFuelSale/1000,lFuelSale%1000);
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeQty0[I]",	(BYTE *)"%s=%ld,",1L);//This value will be a transaction parameter as we sell items at the Pump.
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemWeight0[F]",	(BYTE *)"%s=%ld.%03ld,",lVolume/1000,lVolume%1000);
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemIsFuel0[B]",	(BYTE *)"%s=%s,",(BYTE *)"Y",1);
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemPrice0[F]",		(BYTE *)"%s=%ld.%03ld,",lGradePrice/1000,lGradePrice%1000); //4.0.10.49
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemType0[S]",(BYTE *)"%s=%s,",(BYTE *)"C",1);
            }	//4.0.9.507 
        }
        else if ( lItemCount > 1 ) 	//Car wash			
        {
            long lAmount = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice,sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sUnitPrice)) * (_Module.m_server.GetCarWashScalarFactor());
            long lQty = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sQty,sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sQty));
            long lDiscount = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sDiscount,sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sDiscount)) * 10;
            long lTax  = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[2].sTax,sizeof(pInfo->CardSaleInfo.extraData.SALES[2].sTax)) * 10;        //3.2.0.9	
            long lFuelSale = lValue - lAmount + lDiscount;

            //Fuel is alway there, and it always the first.
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCode0[S]",		(BYTE *)"%s=%03ld,",	_Module.m_server.GetGradeCode(nGrade));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeAmt0[F]",	(BYTE *)"%s=%ld.%03ld,",lFuelSale/1000,lFuelSale%1000);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeQty0[I]",	(BYTE *)"%s=%ld,",1L);//This value will be a transaction parameter as we sell items at the Pump.
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemWeight0[F]",	(BYTE *)"%s=%ld.%03ld,",lVolume/1000,lVolume%1000);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemIsFuel0[B]",	(BYTE *)"%s=%s,",(BYTE *)"Y",1);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemTax0[F]",		(BYTE *)"%s=%ld.%03ld,",lTax/1000,lTax%1000);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemType0[S]",(BYTE *)"%s=%s,",(BYTE *)"P",1); //4.0.9999.0

            //3.1.1.1
            if (lFuelLoyalApp)
            {
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemLoyalAmt0[F]",	(BYTE *)"%s=%ld.%03ld,",lFuelSale/1000,lFuelSale%1000);
                lLoyaltyTotalAmount += lFuelSale;
            }

            //Car Wash sale
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCode1[S]",		(BYTE *)"%s=%03ld,", _Module.m_server.GetCarWashPluCode()); 
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeAmt1[F]",	(BYTE *)"%s=%ld.%03ld,",lAmount/1000,lAmount%1000);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeQty1[I]",	(BYTE *)"%s=%ld,",lQty);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemWeight1[F]",	(BYTE *)"%s=%ld.%03ld,",1,0);//always 1.000
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemIsFuel1[B]",	(BYTE *)"%s=%s,",(BYTE *)"N",1);

            lTax  =a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[0].sTax,sizeof(pInfo->CardSaleInfo.extraData.SALES[0].sTax)) * 10;        	//3.2.0.69				 
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemTax1[F]",		(BYTE *)"%s=%ld.%03ld,",lTax/1000,lTax%1000);

            if(_Module.m_server.GetCarWashLoyaltyApplicable())					//4.0.0.737
            {
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemLoyalAmt1[F]",	(BYTE *)"%s=%ld.%03ld,",lAmount/1000,lAmount%1000);
                lLoyaltyTotalAmount += lAmount;
            }

            //Car Wash Discount
            if ( lItemCount == 3 )
            {
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCode2[S]",		(BYTE *)"%s=%03ld,", _Module.m_server.GetCarWashDiscountCode()); 
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeAmt2[F]",	(BYTE *)"%s=%ld.%03ld,",lDiscount/1000,lDiscount%1000);
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeQty2[I]",	(BYTE *)"%s=%ld,",1);//always 1
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemWeight2[F]",	(BYTE *)"%s=%ld.%03ld,",1,0);//always 1.000
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemIsFuel2[B]",	(BYTE *)"%s=%s,",(BYTE *)"N",1);
                lTax  =a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[1].sTax,sizeof(pInfo->CardSaleInfo.extraData.SALES[1].sTax)) * 10;        	
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemTax2[F]",		(BYTE *)"%s=%ld.%03ld,",lTax / 1000,lTax % 1000);

                if(_Module.m_server.GetCarWashLoyaltyApplicable())						//3.1.1.1
                {
                    lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemLoyalAmt2[F]", (BYTE *) "%s=%ld.%03ld,",lDiscount / 1000, lDiscount % 1000);
                    lLoyaltyTotalAmount -= lDiscount;
                }
            }
        } //RFUEL-2817
		else if ((_Module.m_server.GetOLAInUse() == OLASRV_EPSILON || _Module.m_server.GetOLAInUse() == OLASRV_EPSILON_DISCOUNT) &&
				(lVolume == 0) &&
				(lValue == 0)) // Defect 480448
		{
			lItemCount = 0;

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("No fuel was taken. lItemCount = %ld", lItemCount);
				_Module.m_server.m_cLogger.LogMsg("COLABase", "BuildRequest", lPumpNumber, LOG_PUMP, str); //4.0.23.31 75234
			}
		}
        else
        {
            long lTax  = a2l((BYTE *)&pInfo->CardSaleInfo.extraData.SALES[2].sTax,sizeof(pInfo->CardSaleInfo.extraData.SALES[2].sTax)) * 10;        //3.2.0.9

            //4.0.31.500  TD 404911
            BYTE sTmpVat[sizeof(pInfo->CardSaleInfo.extraData.sVAT_Rate)+sizeof(pInfo->CardSaleExtraData5.sVAT_RateMSB)] = {' '};
            PackFields(pInfo->CardSaleInfo.extraData.sVAT_Rate, sizeof(pInfo->CardSaleInfo.extraData.sVAT_Rate), &pInfo->CardSaleExtraData5.sVAT_RateMSB, sizeof(pInfo->CardSaleExtraData5.sVAT_RateMSB), sTmpVat);	
            long lVatRate = a2l(sTmpVat, sizeof(sTmpVat));

            //Fuel is alway there, and it always the first.
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCode0[S]",		(BYTE *)"%s=%03ld,",	_Module.m_server.GetGradeCode(nGrade));
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeAmt0[F]",	(BYTE *)"%s=%ld.%03ld,",lValue/1000,lValue%1000);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemCodeQty0[I]",	(BYTE *)"%s=%ld,",1L);//This value will be a transaction parameter as we sell items at the Pump.
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemWeight0[F]",	(BYTE *)"%s=%ld.%03ld,",lVolume/1000,lVolume%1000);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemIsFuel0[B]",	(BYTE *)"%s=%s,",(BYTE *)"Y",1);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemTax0[F]",		(BYTE *)"%s=%ld.%03ld,",lTax/1000,lTax%1000);
            lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemTaxRate0[F]",	(BYTE *)"%s=%ld.%02ld,",lVatRate/100,lVatRate%100); //4.0.15.504

            if (lFuelLoyalApp)					//3.1.1.1
            {
                lOffset += SetParam(pOutBuffer+lOffset,(BYTE *)"ItemLoyalAmt0[F]",	(BYTE *)"%s=%ld.%03ld,",lValue/1000,lValue%1000);
                lLoyaltyTotalAmount += lValue;
            }
        }
        //End of the dedicate code for Fuel+Car wash+Discount.
    }
}
