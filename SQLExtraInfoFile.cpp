#include "stdafx.h"
#include "SQLExtraInfoFile.h"
#include "PumpSrvDatabase.h"
#include "PumpClass.h"

CSQLExtraInfoFile::CSQLExtraInfoFile()
{
}


CSQLExtraInfoFile::~CSQLExtraInfoFile()
{
}


long CSQLExtraInfoFile::InsertPrePayRecord(CPumpTransact *pTrs, void *pData)
{
	EXTRA_INFO_REC  cExtraInfoRec;
	TAG_DISCOUNT_LOYALTY_INFO4 *pRec;	//4.0.20.450	66977 //4.0.25.503 144328
	long lRtc = 0;
	CString str;
	CString sExtraInfo;
	if (pTrs->GetLinkFlags() & TRS_LINK_CASH_ACCEPTOR_INFO)
	{
		if (pData)
		{
			lRtc = Operation(REC_INSERT, pData, sExtraInfo);
		}
		else
		{
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				str.Format("CSQLExtraInfoFile InsertPrePayRecord  fail for Index=%ld, lFlags=%ld,lRtc=%ld", pTrs->m_lNumber, pTrs->GetLinkFlags(), lRtc);
				_LOGMSG.LogMsg(str);
			}
		}
	}
	else if (pTrs->GetLinkFlags() & TRS_LINK_FLEET)   //4.0.9.507
	{
		EXTRA_INFO_	*TmpRec = (EXTRA_INFO_ *)pData;
		TmpRec->cDieselInfo.lFlag |= TRS_LINK_FLEET;

		TmpRec->cDieselInfo.lIndexNumber = pTrs->m_lNumber;
		memcpy(&cExtraInfoRec, TmpRec, min(sizeof(EXTRA_INFO_), sizeof(cExtraInfoRec)));
		lRtc = Operation(REC_INSERT, &cExtraInfoRec, sExtraInfo);
	}
	else
	{
		pRec = (TAG_DISCOUNT_LOYALTY_INFO4 *)pData; //4.0.20.450 66977 //4.0.21.501 //4.0.25.503 144328

		cExtraInfoRec.ClearRecord(); // RFUEL-3708
		cExtraInfoRec.info.lAuthAmt = 0;

		if (pData != NULL)
		{
			cExtraInfoRec.info.lIndexNumber = pTrs->m_lNumber;
			memcpy(cExtraInfoRec.info.sLoyalId, pRec->cDiscountLoyaltyInfo3.sLoyalId, sizeof(cExtraInfoRec.info.sLoyalId));
			memcpy(cExtraInfoRec.info.sDiscountType, pRec->cDiscountLoyaltyInfo3.sDiscountType, sizeof(cExtraInfoRec.info.sDiscountType));  //4.0.6.502

			for (int i = 0; i < MAX_GRADES; i++)
				cExtraInfoRec.info.lDiscountArray[i] = a2l(pRec->cDiscountLoyaltyInfo3.sDiscountArray.sDiscount[i], STR_LONG_VAL6);

			// 3.1.1.66
			memcpy(cExtraInfoRec.info.sPromotionNumber, pRec->cDiscountLoyaltyInfo3.sPromotionNumber, sizeof(cExtraInfoRec.info.sPromotionNumber));
			cExtraInfoRec.info.lFlag |= TRS_LINK_PREPAY_NUMBER_AND_LOYALTY;

			//4.0.20.450 66977 //4.0.21.501
			memcpy(&cExtraInfoRec.info.sLoyaltyVolumeLimit, &(pRec->cDiscountLoyaltyInfo3.sLoyaltyVolumeLimit), sizeof(cExtraInfoRec.info.sLoyaltyVolumeLimit));

			memcpy(&cExtraInfoRec.info.sDeferralAccepted, &pRec->cDiscountLoyaltyInfo3.sDeferralAccepted, sizeof(cExtraInfoRec.info.sDeferralAccepted));//4.0.22.505 69344

			memcpy(cExtraInfoRec.info.GradeAmountLimit, pRec->GradeAmountLimit, sizeof(cExtraInfoRec.info.GradeAmountLimit)); //4.0.23.720 111746 144328 //4.0.25.503 144328

			lRtc = Operation(REC_INSERT, &cExtraInfoRec, sExtraInfo);
		}
		else
			lRtc = 1;
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		str.Format("CSQLExtraInfoFile InsertPrePayRecord Index=%ld, lFlags=%ld,lRtc=%ld", pTrs->m_lNumber, pTrs->GetLinkFlags(), lRtc);
		_LOGMSG.LogMsg(str);
	}



	return lRtc;
}

long CSQLExtraInfoFile::Operation(eFileOperation OpCode, void * pInfo ,CString &sExtraInfoXml)
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	long lRetVal = OK;
	EXTRA_INFO			*pExtraInfo = NULL;
	PS::PUMPSRV_EXTRA_INFO_REC rec;

	if (pInfo != NULL)//4.0.12.502 
		pExtraInfo = (EXTRA_INFO *)pInfo;

	memset(&rec, 0, sizeof(PS::PUMPSRV_EXTRA_INFO_REC));
	
	switch (OpCode)
	{
	case REC_INSERT:
	case REC_UPDATE:
		rec.info = *(PUMPSRV_EXTRA_INFO*)pExtraInfo;
		rec.info.lIndexNumber = pExtraInfo->lIndexNumber;
		lRetVal = PumpSrvDataBase.SaveExtraInfo(rec, sExtraInfoXml, rec.info.lIndexNumber) ? OK : 1;
		break;

	case REC_READ:
		rec.info.lIndexNumber = pExtraInfo->lIndexNumber;
		lRetVal = PumpSrvDataBase.LoadExtraInfo(rec, sExtraInfoXml, rec.info.lIndexNumber) ? OK : 1;
		if (lRetVal == OK)
		{
			if (pExtraInfo)
				*(PUMPSRV_EXTRA_INFO*)pExtraInfo = rec.info;
			
		}
		break;
	case REC_DELETE:
		rec.info.lIndexNumber = pExtraInfo->lIndexNumber;
		lRetVal = PumpSrvDataBase.DeleteExtraInfo(rec.info.lIndexNumber) ? OK : 1;
		break;

	case REC_READ_FIRST:
		lRetVal = PumpSrvDataBase.LoadFirstExtraInfo(rec, sExtraInfoXml) ? OK : 1;
		if (lRetVal == OK)
		{
			rec.info.lIndexNumber = rec.info.lIndexNumber;
			if (pExtraInfo)
				*(PUMPSRV_EXTRA_INFO*)pExtraInfo = rec.info;
		}
		break;

	case REC_READ_NEXT:

		rec.info = *(PUMPSRV_EXTRA_INFO*)pExtraInfo;
		rec.info.lIndexNumber = pExtraInfo->lIndexNumber;
		lRetVal = PumpSrvDataBase.LoadNext(rec.info.lIndexNumber ,rec, sExtraInfoXml) ? OK : 1;
		if (lRetVal == OK)
		{
			if (pExtraInfo)
				*(PUMPSRV_EXTRA_INFO*)pExtraInfo = rec.info;
		}
		break;

	case REC_DELETE_TILL_INDEX:
		break;
	}
	
	return lRetVal;
}

long CSQLExtraInfoFile::InsertPayAtKioskRecord(CPumpTransact *pTrs, void *pData)
{
	EXTRA_INFO_REC  cExtraInfoRec;
	PAY_AT_PUMP_INFO *pRec;
	CString str;
	long lRtc = 0;
	BOOL bUpdate = FALSE;
	CString sExtraInfoXml;

	pRec = (PAY_AT_PUMP_INFO *)pData;

	//clear record garbage  //4.0.5.32
	cExtraInfoRec.ClearRecord(); // RFUEL-3708
	cExtraInfoRec.info.lAuthAmt = 0;

	if (pData != NULL)
	{
		cExtraInfoRec.info.lIndexNumber = pTrs->m_lNumber;
		lRtc = Operation(REC_READ, &cExtraInfoRec, sExtraInfoXml);

		if (lRtc)
		{
			// Record not found, continue as usual. //4.0.5.32
			cExtraInfoRec.ClearRecord(); // RFUEL-3708
			cExtraInfoRec.info.lAuthAmt = 0;
			cExtraInfoRec.info.lIndexNumber = pTrs->m_lNumber;
		}
		else
		{
			bUpdate = TRUE;
		}

		memcpy(cExtraInfoRec.info.sLoyalId, pRec->CardSaleInfo.extraData.sLoyalId, sizeof(cExtraInfoRec.info.sLoyalId));
		for (int i = 0; i < MAX_GRADES; i++)
			cExtraInfoRec.info.lDiscountArray[i] = a2l(pRec->CardSaleExtraData2.sDiscountPerGrade[i].sDiscount, sizeof(pRec->CardSaleExtraData2.sDiscountPerGrade[i].sDiscount));  //3.2.1.0			 
																																												   //START 3.1.1.6
		memcpy(cExtraInfoRec.info.sPromotionNumber, pRec->CardSaleExtraData2.sPromotionNumber, sizeof(cExtraInfoRec.info.sPromotionNumber));

		// 4.0.5.26
		memcpy(cExtraInfoRec.info.sCustomerName, pRec->CardSaleInfo.extraData.sCustomerName, sizeof(cExtraInfoRec.info.sCustomerName));
		memcpy(cExtraInfoRec.info.sTotalBalance, pRec->CardSaleExtraData2.sTotalBalance, sizeof(cExtraInfoRec.info.sTotalBalance));
		cExtraInfoRec.info.sBalanceType = ' ';
		memcpy(cExtraInfoRec.info.sHostTransId, pRec->CardSaleInfo.cardData.sResInfoAMax8, sizeof(cExtraInfoRec.info.sHostTransId));

		cExtraInfoRec.info.lControllerSeqNum = a2l(pRec->CardSaleExtraData3.sControllerSeqNum, sizeof(pRec->CardSaleExtraData3.sControllerSeqNum));

		cExtraInfoRec.info.lPreSetlimit = a2l(pRec->CardSaleExtraData3.sPreSetlimit, sizeof(pRec->CardSaleExtraData3.sPreSetlimit));
		cExtraInfoRec.info.sPreSetLimitType = pRec->CardSaleExtraData3.sPreSetlimitType;

		//4.0.22.504 69344
		memcpy(cExtraInfoRec.info.sNonDiscountTotalAmount, pRec->CardSaleExtraData3.sNonDiscountTotalAmount, sizeof(pRec->CardSaleExtraData3.sNonDiscountTotalAmount));
		memcpy(&cExtraInfoRec.info.sDeferralAccepted, &pRec->CardSaleExtraData4.sDeferralAccepted, sizeof(pRec->CardSaleExtraData4.sDeferralAccepted));
		memcpy(cExtraInfoRec.info.sOLDSession, pRec->CardSaleExtraData2.sAuthorizeSessionID_MSB, sizeof(pRec->CardSaleExtraData2.sAuthorizeSessionID_MSB));				//4.0.23.220 105272 BP //4.0.24.60
		memcpy(&cExtraInfoRec.info.sOLDSession[2], pRec->CardSaleInfo.cardData.sAuthorizeSessionID, sizeof(pRec->CardSaleInfo.cardData.sAuthorizeSessionID));				//marikdebug //4.0.23.220 105272 BP //4.0.24.60


		if (LOG_DETAIL_FLOW_CONTROL)
		{
			str.Format("CSQLExtraInfoFile InsertPayAtKioskRecord Data=%.20s , TotalBalanc=%.8s, sHostTransId=%.8s, seqNumber=%ld, lDiscountArray = [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld], [%ld] , %.6s ",
				cExtraInfoRec.info.sCustomerName,
				cExtraInfoRec.info.sTotalBalance,
				cExtraInfoRec.info.sHostTransId,
				cExtraInfoRec.info.lControllerSeqNum,
				cExtraInfoRec.info.lDiscountArray[0], cExtraInfoRec.info.lDiscountArray[1], cExtraInfoRec.info.lDiscountArray[2], cExtraInfoRec.info.lDiscountArray[3],
				cExtraInfoRec.info.lDiscountArray[4], cExtraInfoRec.info.lDiscountArray[5], cExtraInfoRec.info.lDiscountArray[6], cExtraInfoRec.info.lDiscountArray[7],
				cExtraInfoRec.info.lDiscountArray[8], cExtraInfoRec.info.lDiscountArray[9], cExtraInfoRec.info.lDiscountArray[10], cExtraInfoRec.info.lDiscountArray[11],
				cExtraInfoRec.info.lDiscountArray[12], cExtraInfoRec.info.lDiscountArray[13], cExtraInfoRec.info.lDiscountArray[14], cExtraInfoRec.info.lDiscountArray[15], cExtraInfoRec.info.sOLDSession);	//4.0.22.504 73878 //4.0.24.60
			_LOGMSG.LogMsg(str);
		}

		//3.2.0.76
		if (pTrs->m_lResExt == RES_EXT_ATTENDANT)
		{
			//4.0.2.21
			memcpy(cExtraInfoRec.info.sFullVehicleId, pRec->CardSaleInfo.cardData.sFullVehicleId, sizeof(cExtraInfoRec.info.sFullVehicleId));
			memcpy(cExtraInfoRec.info.sAttendantName, pRec->CardSaleExtraData2.sAttendantName, sizeof(cExtraInfoRec.info.sAttendantName));
			memcpy(cExtraInfoRec.info.sAttendantID, pRec->CardSaleExtraData2.sAttendantID, sizeof(cExtraInfoRec.info.sAttendantID));

			memcpy(cExtraInfoRec.info.sAccountNumber, pRec->CardSaleInfo.cardData.sAccountNumber, sizeof(cExtraInfoRec.info.sAccountNumber));
			memcpy(cExtraInfoRec.info.sAuthNumber, pRec->CardSaleInfo.cardData.sAuthNumber, sizeof(cExtraInfoRec.info.sAuthNumber));
			cExtraInfoRec.info.lAuthAmt = Convert2Str2l(pRec->CardSaleInfo.cardData.sAuthAmt, sizeof(pRec->CardSaleInfo.cardData.sAuthAmt), pRec->CardSaleInfo.extraData.sAuthAmt_Msb, sizeof(pRec->CardSaleInfo.extraData.sAuthAmt_Msb)); //3.2.0.73

																																																										   // 4.0.3.59
			memcpy(cExtraInfoRec.info.sTrack1Data, pRec->CardSaleInfo.cardData.sTrack1Data, sizeof(cExtraInfoRec.info.sTrack1Data));
			memcpy(cExtraInfoRec.info.sTrack2Data, pRec->CardSaleInfo.cardData.sTrack2Data, sizeof(cExtraInfoRec.info.sTrack2Data));
			// 4.0.3.59

			//4.0.6.506
			cExtraInfoRec.info.sClearanceID = pRec->CardSaleInfo.cardData.sClearanceID;
			memcpy(cExtraInfoRec.info.sCardName, pRec->CardSaleInfo.cardData.sCardName, sizeof(pRec->CardSaleInfo.cardData.sCardName));
			memcpy(cExtraInfoRec.info.sOLDSession, pRec->CardSaleExtraData2.sAuthorizeSessionID_MSB, sizeof(pRec->CardSaleExtraData2.sAuthorizeSessionID_MSB));
			memcpy(&cExtraInfoRec.info.sOLDSession[2], pRec->CardSaleInfo.cardData.sAuthorizeSessionID, sizeof(pRec->CardSaleInfo.cardData.sAuthorizeSessionID));
			memcpy(cExtraInfoRec.info.sMediaMasterType, pRec->CardSaleExtraData2.sMediaMasterType, sizeof(pRec->CardSaleExtraData2.sMediaMasterType));

			//4.0.6.507
			memcpy(cExtraInfoRec.info.sExpire, pRec->CardSaleInfo.cardData.sExpire, sizeof(pRec->CardSaleInfo.cardData.sExpire));

			memcpy(cExtraInfoRec.info.sCustomerID, pRec->CardSaleExtraData3.sCustomerID, sizeof(pRec->CardSaleExtraData3.sCustomerID));
			//4.0.9.500				
			memcpy(cExtraInfoRec.info.sReferenceNumber, pRec->CardSaleInfo.cardData.sReferenceNumber, sizeof(pRec->CardSaleInfo.cardData.sReferenceNumber));

			cExtraInfoRec.info.sGRDiscountType = pRec->CardSaleExtraData2.sGRDiscountType;
			memcpy(cExtraInfoRec.info.sGRDiscountPerGallon, pRec->CardSaleInfo.cardData.sGRDiscountPerGallon, sizeof(cExtraInfoRec.info.sGRDiscountPerGallon));

			//4.0.9.508			
			memcpy(cExtraInfoRec.info.sPMTReferenceNumber, pRec->CardSaleExtraData3.sPMTReferenceNumber, sizeof(cExtraInfoRec.info.sPMTReferenceNumber));
			memcpy(cExtraInfoRec.info.sManagerID, pRec->CardSaleExtraData3.sManagerID, sizeof(cExtraInfoRec.info.sManagerID));
			memcpy(cExtraInfoRec.info.sClFlags, pRec->CardSaleExtraData3.sClFlags, sizeof(cExtraInfoRec.info.sClFlags));

			memcpy(cExtraInfoRec.info.sServiceFee, pRec->CardSaleExtraData2.sServiceFee, sizeof(pRec->CardSaleExtraData2.sServiceFee));

			cExtraInfoRec.info.sIgnoreServiceFee = pRec->CardSaleExtraData2.sIgnoreServiceFee;  //4.0.10.504
			cExtraInfoRec.info.sDryItemsRestriction = pRec->CardSaleExtraData3.sDryItemsRestriction;  //4.0.10.504

																									  // 4.0.15.10

			if (LOG_DETAIL_FLOW_CONTROL)
			{
				str.Format("CSQLExtraInfoFile InsertPayAtKioskRecord Data=%.21s , pRec->CardSaleInfo.cardData.sAuthNumber=%.12s, lAuthAmt=%ld,expire=%.4s ,sCustomerID=%.10s , Cl2Flags=%.2s , sNonDiscountTotalAmount=%.10s",
					pRec->CardSaleInfo.cardData.sAccountNumber,
					pRec->CardSaleInfo.cardData.sAuthNumber,
					cExtraInfoRec.info.lAuthAmt,
					cExtraInfoRec.info.sExpire,
					cExtraInfoRec.info.sCustomerID,
					cExtraInfoRec.info.sClFlags,
					cExtraInfoRec.info.sNonDiscountTotalAmount);
				_LOGMSG.LogMsg(str);   //4.0.8.507
			}
		}
		else
			cExtraInfoRec.info.lFlag |= TRS_LINK_PAK_AND_LOYALTY;

		if (bUpdate)
			lRtc = Operation(REC_UPDATE, &cExtraInfoRec, sExtraInfoXml);
		else
			lRtc = Operation(REC_INSERT, &cExtraInfoRec, sExtraInfoXml);
	}
	else
	{
		lRtc = 1;
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		str.Format("(PAK) Insert new Record to ExtraInfo file , Index=%ld lRtc=%ld, Update=%d,sAttendantID=%.10s ", pTrs->m_lNumber, lRtc, bUpdate, cExtraInfoRec.info.sAttendantID);
		_LOGMSG.LogMsg(str);
	}

	return lRtc;
}

long CSQLExtraInfoFile::DeleteRecord(CPumpTransact *pTrs)
{
	EXTRA_INFO_REC  cExtraInfoRec;
	long lRtc = 0;
	CString str;
	memset(&cExtraInfoRec, 0, sizeof(EXTRA_INFO_REC));
	cExtraInfoRec.info.lIndexNumber = pTrs->m_lNumber;
	CString sExtraInfoXml;

	lRtc = Operation(REC_DELETE, &cExtraInfoRec, sExtraInfoXml);

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		str.Format("CSQLExtraInfoFile::DeleteRecord Index=%ld lRtc=%ld", pTrs->m_lNumber, lRtc); //4.0.23.500
		_LOGMSG.LogMsg(str);
	}

	return lRtc;
}
