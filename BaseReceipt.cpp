#include "StdAfx.h"
#include "BaseReceipt.h"
#include "Convert.h"

CBaseReceipt::CBaseReceipt()
{

}


CBaseReceipt::~CBaseReceipt()
{

}


/******************************************************************************
 Description:ConvertOlaToReceipt - Convert and set the data in the
			 Receipt File
 Returns:      	
 Parameters:	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			23/05/2005   14:34		Start
 Liat			17/06/2008	 14:36		Update CR 52440 PCI
 Ella			15/09/2010	 16:30		4.0.26.0 - TD 69677
 AlexM			04/11/2013	 14:38		Update TD 347994 
******************************************************************************/
long CBaseReceipt::ConvertOlaToReceipt(CPumpTransact *pTrs, void *pInfo, RECEIPT_INFO *pReceipt ,short lOlaState, const long lPumpNumber,long lFlags)
{
	PAY_AT_PUMP_INFO	*pPAPInfo;
	BYTE				sTmpTrack2[ENCRYPTED_DATA_SIZE]; 
	long				lSizeofVar =0;//if avi then the size is diff from track2;
	long				lRetCode = 0;
	long				p = pTrs->m_lPump -1 ;	//4.0.30.0 270322

	if(pInfo)
	{
		pPAPInfo = (PAY_AT_PUMP_INFO *)pInfo;	

		PAY_AT_PUMP_INFO pPAPInfoToReceipt;
		memset(&pPAPInfoToReceipt,0,sizeof(PAY_AT_PUMP_INFO));
		pPAPInfoToReceipt = *pPAPInfo;

		const long lOLABufSize = sizeof(pPAPInfo->CardSaleExtraData2);	//995
		const long lReceiptBufSize = sizeof(pReceipt->cReceipt6.BufferRec6);	//922
		const long lSizeDiffOlaReceipt = lOLABufSize - lReceiptBufSize;	//73

		if (!(lFlags & DO_NOT_RESET_RECEIPT_BUFFER)) //4.0.26.0 - TD 69677
			memset(pReceipt , 0 , sizeof(RECEIPT_INFO));

		memset(sTmpTrack2 ,' ' , sizeof(sTmpTrack2));
		if (_Module.m_server.m_cProtectedDataHandler.GetPCIActiveFlag() == PCI_FLAG_ON)
		{
			_Module.m_server.m_cProtectedDataHandler.RemoveConfidencesData(&pPAPInfoToReceipt);
		}
		Track2Format(pPAPInfoToReceipt.CardSaleInfo.cardData.sTrack2Data);	

		if (lFlags & TRS_NUMBER_IN_RECEIPT_INDEX) // RFUEL-1917
		{
			sprintf_s((char*)sTmpTrack2,sizeof sTmpTrack2, "%ld", pTrs->m_lNumber);
			lSizeofVar = sizeof(sTmpTrack2);
			_Module.m_server.m_cLogger.LogMsg("ConvertOlaToReceipt, Mobile Transaction number!");
		}
		else  if (pTrs->m_lResExt == RES_EXT_MOBILE)
		{
			sprintf_s((char*)sTmpTrack2, sizeof sTmpTrack2, "%0*ld", 10, pTrs->m_lNumber);
			lSizeofVar = sizeof(sTmpTrack2);
			_Module.m_server.m_cLogger.LogMsg("ConvertOlaToReceipt, Receipt index is mobile transaction number)");
		}
		else if (pPAPInfoToReceipt.CardSaleInfo.cardData.sTranType == CD_TYPE_AVI)
		{
			memcpy(sTmpTrack2, pPAPInfoToReceipt.CardSaleInfo.cardData.sFullVehicleId, sizeof(pPAPInfoToReceipt.CardSaleInfo.cardData.sFullVehicleId));
			lSizeofVar = sizeof(pPAPInfoToReceipt.CardSaleInfo.cardData.sFullVehicleId);
			_LOGMSG.LogMsg("ConvertOlaToReceipt, Car registration Transaction 1 !");	
		}
		else
		{
			lSizeofVar = GetReceiptIndex((char*)sTmpTrack2, &pPAPInfoToReceipt, lPumpNumber);
		}
		

		SetNoneOPTReceiptAccountMask(&pPAPInfoToReceipt, (char*)pPAPInfoToReceipt.CardSaleInfo.cardData.sAccountNumber);		//4.0.27.501 TD 233308

		memcpy(pReceipt->cReceipt1.BufferRec1 , (char *) &pPAPInfoToReceipt.CardSaleInfo, sizeof(pPAPInfo->CardSaleInfo));	//900
		memcpy(pReceipt->cReceipt2.BufferRec2 ,(char *) &pPAPInfoToReceipt.CardSaleExtraData2, sizeof(pReceipt->cReceipt2.BufferRec2));	//922
		memcpy(pReceipt->cReceipt3.BufferRec3 ,(char *) &pPAPInfoToReceipt.CardSaleExtraData3, sizeof(pReceipt->cReceipt3.BufferRec3));	//922
		memcpy(pReceipt->cReceipt4.BufferRec4 ,(char *) &pPAPInfoToReceipt.CardSaleExtraData4, sizeof(pReceipt->cReceipt4.BufferRec4));	//922
		memcpy(pReceipt->cReceipt5.BufferRec5 ,(char *) &pPAPInfoToReceipt.CardSaleExtraData5, min(sizeof(pReceipt->cReceipt5.BufferRec5), sizeof(pPAPInfoToReceipt.CardSaleExtraData5)));	//785 out of 922 //4.0.22.13	TD 72716	filer =922 -785 =137
		memcpy(pReceipt->cReceipt7.BufferRec7 ,(char *) &pPAPInfoToReceipt.CardSaleExtraData6, sizeof(pReceipt->cReceipt7.BufferRec7));	//922


		//All remainders are saved in cReceipt6.sbuffer	//4.0.22.13	TD 72716
		memcpy(pReceipt->cReceipt6.BufferRec6 ,(char *)&pPAPInfoToReceipt.CardSaleExtraData2 + lReceiptBufSize , lSizeDiffOlaReceipt);	//last 73 bytes of CardSaleExtraData2
		memcpy(pReceipt->cReceipt6.BufferRec6 + lSizeDiffOlaReceipt, (char *)&pPAPInfoToReceipt.CardSaleExtraData3 + lReceiptBufSize , lSizeDiffOlaReceipt);	//last 73 bytes of CardSaleExtraData3
		memcpy(pReceipt->cReceipt6.BufferRec6 + 2*lSizeDiffOlaReceipt, (char *)&pPAPInfoToReceipt.CardSaleExtraData4 + lReceiptBufSize , lSizeDiffOlaReceipt);	//last 73 bytes of CardSaleExtraData4
		memcpy(pReceipt->cReceipt6.BufferRec6 + 3*lSizeDiffOlaReceipt, pTrs , sizeof(CPumpTransact)); //76 //200	

		memset(pReceipt->cReceipt1.Index.sTrack2, ' ', sizeof(pReceipt->cReceipt1.Index.sTrack2));
		memcpy(pReceipt->cReceipt1.Index.sTrack2, sTmpTrack2, lSizeofVar);

		//cReceipt2
		memset(pReceipt->cReceipt2.Index.sTrack2, ' ', sizeof(pReceipt->cReceipt2.Index.sTrack2));
		memcpy(pReceipt->cReceipt2.Index.sTrack2, sTmpTrack2, lSizeofVar);

		//cReceipt3
		memset(pReceipt->cReceipt3.Index.sTrack2, ' ', sizeof(pReceipt->cReceipt3.Index.sTrack2));
		memcpy(pReceipt->cReceipt3.Index.sTrack2, sTmpTrack2, lSizeofVar);

		//cReceipt4
		memset(pReceipt->cReceipt4.Index.sTrack2, ' ', sizeof(pReceipt->cReceipt4.Index.sTrack2));
		memcpy(pReceipt->cReceipt4.Index.sTrack2, sTmpTrack2, lSizeofVar);

		//cReceipt5
		memset(pReceipt->cReceipt5.Index.sTrack2, ' ', sizeof(pReceipt->cReceipt5.Index.sTrack2));
		memcpy(pReceipt->cReceipt5.Index.sTrack2, sTmpTrack2, lSizeofVar);

		//cReceipt6
		memset(pReceipt->cReceipt6.Index.sTrack2, ' ', sizeof(pReceipt->cReceipt6.Index.sTrack2));	//4.0.22.13	TD 72716
		memcpy(pReceipt->cReceipt6.Index.sTrack2, sTmpTrack2, lSizeofVar);

		//cReceipt7
		memset(pReceipt->cReceipt7.Index.sTrack2, ' ', sizeof(pReceipt->cReceipt7.Index.sTrack2));	//4.0.22.13	TD 72716
		memcpy(pReceipt->cReceipt7.Index.sTrack2, sTmpTrack2, lSizeofVar);
		

		_LOGMSG.LogMsg("ConvertOlaToReceipt, Finish ConvertOlaToReceipt successfully");
	
		_Module.m_server.m_cMemPumpArray[p].SetMemPumpInfo( (PAY_AT_PUMP_INFO *) pInfo , pTrs , pReceipt); // TD 270322 //4.0.30.0 
	}
	else
	{
		_LOGMSG.LogMsg("ConvertOlaToReceipt, Failed ConvertOlaToReceipt!");
		lRetCode = 1;
	}

	return lRetCode; 
}


/******************************************************************************
 Description:GetReceiptIndexByTrsNum
			 Look for the receipt index in MemPump according to trs number
	
 Returns:    lRetIndexLen - length of the index  	
 Parameters: sIndexOut - out index
			 bstrTrsNumIn - in trs num 270322

 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 OmerT			31/01/2012   14:34		TD //4.0.30.0 // TD 270322 
******************************************************************************/
long CBaseReceipt::GetReceiptIndexByTrsNum(char* sIndexOut , const BSTR bstrTrsNumIn)
{
	long lRetIndexLen = 0;

	long  lRequiredSize = WideCharToMultiByte(CP_ACP, 0,  bstrTrsNumIn, -1, NULL, 0, NULL, NULL);
	char * szTrsNumber = new char[lRequiredSize];
	memset(szTrsNumber,0,lRequiredSize);
	WideCharToMultiByte(CP_ACP, 0, bstrTrsNumIn, -1, szTrsNumber, lRequiredSize, NULL, NULL);

	long lTrsNumber = a2l((unsigned char *)szTrsNumber,lRequiredSize);
	lRetIndexLen = _Module.m_server.m_cCommonFunction.LookForInternalReceiptIndex(sIndexOut, lTrsNumber);

	delete [] szTrsNumber;

	return lRetIndexLen;
}

long CBaseReceipt::ConvertOlaToReceipt(CPumpTransact *pTrs, PAY_AT_PUMP_INFO *pPAPInfo, CReceiptInfo & ReceiptInfo, short lOlaState)
{

	try
	{
		PAY_AT_PUMP_INFO	tmpInfo;//for the encrypt account from the dll
		CStringA			sTmpTrack2;
		long				lSizeofVar = 0;//if avi then the size is diff from track2;

		ReceiptInfo.Clear();
		memcpy(&tmpInfo, pPAPInfo, sizeof(PAY_AT_PUMP_INFO));

		if (_Module.m_server.m_cProtectedDataHandler.GetPCIActiveFlag() == PCI_FLAG_ON)
		{
			_Module.m_server.m_cProtectedDataHandler.RemoveConfidencesData(pPAPInfo); // RFUEL-1347
		}
		Track2Format(pPAPInfo->CardSaleInfo.cardData.sTrack2Data);

		//here we have to call encrypt dll and insert the result 
		//into track1 track2  extrafield

		//call to encrypt function according  to the class
		if (pPAPInfo->CardSaleInfo.cardData.sTranType == CD_TYPE_AVI)
		{
			sTmpTrack2.SetString((LPCSTR)tmpInfo.CardSaleInfo.cardData.sFullVehicleId, sizeof(tmpInfo.CardSaleInfo.cardData.sFullVehicleId));
			lSizeofVar = sizeof(tmpInfo.CardSaleInfo.cardData.sFullVehicleId);
			_Module.m_server.m_cLogger.LogMsg("Car registration Transaction 1 !");
		}
		else
		{
			sTmpTrack2.SetString((LPCSTR)tmpInfo.CardSaleInfo.cardData.sAccountNumber, sizeof(tmpInfo.CardSaleInfo.cardData.sAccountNumber));
			lSizeofVar = sizeof(tmpInfo.CardSaleInfo.cardData.sTrack2Data);
		}


		ReceiptInfo.m_HashedKey = sTmpTrack2;
		ReceiptInfo.m_PAPInfo = *pPAPInfo;

		


		if (pTrs)
		{
			ReceiptInfo.UpdateTrsNumber(pTrs->m_lNumber);
			ReceiptInfo.m_PumpTransact = *pTrs;
			ReceiptInfo.UpdatePumpNumber();

			bool bTimeConvert = ReceiptInfo.m_PumpTransact.m_cTime.GetAsSystemTime(ReceiptInfo.m_TrsTime); // RFUEL-1169
			if (false == bTimeConvert)
				GetSystemTime(&ReceiptInfo.m_TrsTime);

		}

		CString sMsg;
		sMsg.Format("CBaseReceipt::ConvertOlaToReceipt Finish ConvertOlaToReceipt successfully: TrsIndex = %d , HashedKey = %s", ReceiptInfo.m_TrsIndex, ReceiptInfo.m_HashedKey);
		long lPumpNumber = a2l(tmpInfo.CardSaleInfo.cardData.sPumpNumber, sizeof tmpInfo.CardSaleInfo.cardData.sPumpNumber);
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, sMsg);


	}
	catch (...)
	{
		_Module.m_server.m_cLogger.LogMsg("CBaseReceipt::ConvertOlaToReceipt - Error!", LOG_LEVEL_1);
		return 1;
	}


	return OK;
}