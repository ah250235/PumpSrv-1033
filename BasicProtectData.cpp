// BasicProtectData.cpp: implementation of the CBasicProtectData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BasicProtectData.h"


/******************************************************************************
//						CBasicProtectData
******************************************************************************/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBasicProtectData::CBasicProtectData()
{
	m_bRemoveCardDataFromLog = FALSE;
	m_eAccountMaskStandart = ANMS_HEAD0_TAIL4;
}


CBasicProtectData::CBasicProtectData(const CBasicProtectData& cBasicProtectData)
{
	m_bRemoveCardDataFromLog = cBasicProtectData.m_bRemoveCardDataFromLog;
	m_eAccountMaskStandart = cBasicProtectData.m_eAccountMaskStandart;
}


CBasicProtectData::~CBasicProtectData() 
{

}

/******************************************************************************
  Description:	Clear card info from given string 	
  Returns:      succeeded
  Parameters:  	char *sIn - string source
				long lInSize - string source size , 
				char *sOut   - string destination 
  
  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
  Marik			6/9/2004   10:04		Start
  Liat			07/07/2008	11:53		Change class name 4.0.20.501 TD-60236
  Gena			17/08/2008	19:00		Move from CProtectedDataHendler //4.0.22.13	TD 72716
 ******************************************************************************/
BOOL CBasicProtectData::RemoveCardInfoFromMessage(char *sIn, long lInSize, char *sOut)const
{
	BOOL	bRetCode = TRUE;
	long	lSize=lInSize;

	if (lInSize < 200)
		lSize = 5000;

	try
	{
		int iSizeOfTmpBuffer = lSize * 2 + 1;
		char *szTmpIn =  new char[iSizeOfTmpBuffer];

		memset(szTmpIn , 0 ,iSizeOfTmpBuffer);
		strcpy(szTmpIn,sIn);

		long lFlag = ENCRYPT_DATA;

		if (lInSize)
		{
			//char* sTmpStr = "ResultCode[S]=000,ResultMessage[S]=Approved,Approval[S]=9999,ReferenceNumber[S]=9999,FullCardName[S]=VISA,MerchantID[S]=26007586001,AuthorizationDate[D]=05262011,AuthorizationTime[T]=154800,ReceiptMessage[S]=APPROVED  990495{NL},SVCBalance[F]=0.00,SystemID[I]=1,IsOnline[B]=Y,StoreSession[B]=N,AuthorizationNumber[S]=9999,SessId[I]=129,DeclineCode[S]=,AccountNumber[S]=00020001FA1000000012793BA6956315C9759801FBB99B0ABD,ExpirationDate[E]=201304,RedeemedPoints[I]=0,RedeemedAmount[F]=0.00,ClubCardPoints[I]=0,LoyaltyAmount[F]=0.00,ReceiptMessageEx1[S]=,ReceiptMessageEx2[S]=,FormattedAcct[S]=XXXXXXXXXXXX0001,IsSVC[B]=N,AmountLimit[F]=51,ShortCardName[S]=VI,ChargeTo[I]=0,SignatureRequired[B]=T,MediaMasterType[I]=0,IsFSAPmnt[B]=N,RemoveProhibitedData[B]=Y,,";
			MaskOLAString("AccountNumber[S]=",	szTmpIn, iSizeOfTmpBuffer, lFlag);
			MaskOLAString("Track2[S]=",			szTmpIn, iSizeOfTmpBuffer, lFlag);
			MaskOLAString("Track1[S]=",			szTmpIn, iSizeOfTmpBuffer, lFlag);
			MaskOLAString("ClubCardTrack2[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.10.47
			MaskOLAString("ClubCardNumber[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.10.47			
			MaskOLAString("Data2MAC[S]=",		szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.13.140			
			MaskOLAString("PIN[S]=",			szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.17.20			
			MaskOLAString("ExtendPIN[S]=",		szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.17.20	
			MaskOLAString("BarCode[S]=",		szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.22.66		TD 76185
			MaskOLAString("CustomerName[S]=",	szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.23.500	TD 77826

			//If 'PromptType' = PT_CARD_READER or PT_BOOL_AND_CARD_READER -> remove the Track1 and Track2 from 'ParamResult' 

			MaskOLAString("AccountNumber0[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag); // QC 499737
			MaskOLAString("Data2Verify[S]=",	szTmpIn, iSizeOfTmpBuffer, lFlag);	// 4.0.23.360	
			MaskOLAString("CalculatedMAC[S]=",	szTmpIn, iSizeOfTmpBuffer, lFlag); // 4.0.23.360
			MaskOLAString("ParamResult[S]=",	szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.24.90 TD 119728
			MaskOLAString(",Track2[S]=",		szTmpIn, iSizeOfTmpBuffer, lFlag);  //4.0.28.500
			MaskOLAString(",Track1[S]=",		szTmpIn, iSizeOfTmpBuffer, lFlag);  //4.0.29.501 
			MaskOLAString("EMVExtendedBuffer[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag); // NO TD - Remove sensitive from log
			

			memcpy(sOut ,szTmpIn,lInSize);
		}
		
		if (szTmpIn)
			delete []szTmpIn;
	}
	catch(...)
	{
		CString sMsg;
		sMsg.Format("LogRemoveCardInfoFromMessage, GenerateIndex Had UnExcpcted Error", LOG_LEVEL_1);
		_LOGMSG.LogMsg(sMsg,LOG_LEVEL_0);

		bRetCode = FALSE;
	}

	return bRetCode;
}

BOOL CBasicProtectData::RemoveCardInfoFromMessage(char *sIn, long lInSize, CString & sOut)const
{
	BOOL	bRetCode = TRUE;
	long	lSize = lInSize;

	if (lInSize < 200)
		lSize = 5000;

	try
	{
		int iSizeOfTmpBuffer = lSize * 2 + 1;
		char *szTmpIn = new char[iSizeOfTmpBuffer];

		memset(szTmpIn, 0, iSizeOfTmpBuffer);
		strcpy(szTmpIn, sIn);

		long lFlag = ENCRYPT_DATA;

		if (lInSize)
		{
			//char* sTmpStr = "ResultCode[S]=000,ResultMessage[S]=Approved,Approval[S]=9999,ReferenceNumber[S]=9999,FullCardName[S]=VISA,MerchantID[S]=26007586001,AuthorizationDate[D]=05262011,AuthorizationTime[T]=154800,ReceiptMessage[S]=APPROVED  990495{NL},SVCBalance[F]=0.00,SystemID[I]=1,IsOnline[B]=Y,StoreSession[B]=N,AuthorizationNumber[S]=9999,SessId[I]=129,DeclineCode[S]=,AccountNumber[S]=00020001FA1000000012793BA6956315C9759801FBB99B0ABD,ExpirationDate[E]=201304,RedeemedPoints[I]=0,RedeemedAmount[F]=0.00,ClubCardPoints[I]=0,LoyaltyAmount[F]=0.00,ReceiptMessageEx1[S]=,ReceiptMessageEx2[S]=,FormattedAcct[S]=XXXXXXXXXXXX0001,IsSVC[B]=N,AmountLimit[F]=51,ShortCardName[S]=VI,ChargeTo[I]=0,SignatureRequired[B]=T,MediaMasterType[I]=0,IsFSAPmnt[B]=N,RemoveProhibitedData[B]=Y,,";
			MaskOLAString("AccountNumber[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);
			MaskOLAString("Track2[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);
			MaskOLAString("Track1[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);
			MaskOLAString("ClubCardTrack2[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.10.47
			MaskOLAString("ClubCardNumber[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.10.47			
			MaskOLAString("Data2MAC[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.13.140			
			MaskOLAString("PIN[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.17.20			
			MaskOLAString("ExtendPIN[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.17.20	
			MaskOLAString("BarCode[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.22.66		TD 76185
			MaskOLAString("CustomerName[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.23.500	TD 77826

			//If 'PromptType' = PT_CARD_READER or PT_BOOL_AND_CARD_READER -> remove the Track1 and Track2 from 'ParamResult' 

			MaskOLAString("AccountNumber0[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag); // QC 499737
			MaskOLAString("Data2Verify[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);	// 4.0.23.360	
			MaskOLAString("CalculatedMAC[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag); // 4.0.23.360
			MaskOLAString("ParamResult[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);	//4.0.24.90 TD 119728
			MaskOLAString(",Track2[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);  //4.0.28.500
			MaskOLAString(",Track1[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag);  //4.0.29.501 
			MaskOLAString("EMVExtendedBuffer[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag); // NO TD - Remove sensitive from log
			MaskOLAString("PINEncryptionKey[S]=", szTmpIn, iSizeOfTmpBuffer, lFlag); // NO TD - Remove sensitive from log
			


			sOut = szTmpIn;
		}

		if (szTmpIn)
			delete[]szTmpIn;
	}
	catch (...)
	{
		CString sMsg;
		sMsg.Format("LogRemoveCardInfoFromMessage, GenerateIndex Had UnExcpcted Error", LOG_LEVEL_1);
		_LOGMSG.LogMsg(sMsg, LOG_LEVEL_0);

		bRetCode = FALSE;
	}

	return bRetCode;
}

/******************************************************************************
  Description:	prepare the OLA string to be printed to the log (encrypt values)   	
  Returns:      succeeded
  Parameters:  	char *pszOLAString - field to encrypt 
				char *pszBuff      - OLA buffer, 
				long lFlags        - flags 
  
  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
  Ravit 		6/12/2005   15:04		Start
  Liat			07/07/2008	11:53		Change class name 4.0.20.501 TD-60236
  Gena			17/08/2008	19:00		Move from CProtectedDataHendler //4.0.22.13	TD 72716
 ******************************************************************************/
//private:
BOOL CBasicProtectData::MaskOLAString(char *pszOLAString, char *pszBuff, int iSizeOFOutBuffer, long lFlags) const
{	
	BOOL	bRetCode = TRUE;
	char*	sStrPlace = NULL;
	char*	sValPlace= NULL;
	char*	sValEnd= NULL;
	char*	sValEndByBlank= NULL;
	long	lValLen;
	long	lValEndPlace;
	char	sValLen[4] = {0};
	char*	sEndStr= NULL;

	char	szTmpIn[MAX_OLA_BUFFER_LEN];

	memset(szTmpIn,0,sizeof(szTmpIn));
	strcpy(szTmpIn,pszBuff);
	memset(sValLen,' ',sizeof(sValLen));

	if (lFlags == ENCRYPT_DATA)
	{
		long lStartPlace = 0;
	
		sStrPlace = strstr(szTmpIn, pszOLAString);

		if (sStrPlace != NULL)
		{
			sValPlace = sStrPlace + strlen(pszOLAString);

			//check where the value ends, start with the start place of the value.
			if (!(strcmp("Track1[S]=", pszOLAString))) // QC 496962
			{
				//check where the value ends, start with the start place of the value.
				sValEnd = strstr(sStrPlace + 10, "]=");

				while (sValEnd[0] != ',')
				{
					sValEnd--;
				}
			}
			else
			{
				if (sStrPlace[0] == ',')  //4.0.24.840
					sStrPlace++;

				//check where the value ends, start with the start place of the value.
				sValEnd = strstr(sStrPlace, ",");
			}

			if(!(strcmp("AccountNumber[S]=", pszOLAString)) || 
				!(strcmp("ClubCardNumber[S]=", pszOLAString)) || 
				!(strcmp("FormattedAcct[S]=", pszOLAString)) ||
				!(strcmp("AccountNumber0[S]=", pszOLAString)))
			{
				sValEndByBlank = strstr(sStrPlace," "); 

				if (sValEndByBlank && sValEndByBlank < sValEnd)
					sValEnd = sValEndByBlank; 
				
				lValLen = sValEnd - sValPlace;  

				char sMaskedAccount[ACCOUNT_NUM_SIZE] = {0};
				char sAccountToMask[ACCOUNT_NUM_SIZE] = {0};

				memcpy(sAccountToMask, sValPlace, min(lValLen,ACCOUNT_NUM_SIZE));
				MaskAccount(sMaskedAccount, sAccountToMask);

				memcpy(sValPlace, sMaskedAccount, min(strlen(sMaskedAccount), ACCOUNT_NUM_SIZE));
			}
			else
			{
				BOOL bMaskParam = TRUE;

// 				if ( strcmp( "ParamResult[S]=", pszOLAString ) == 0 )				//4.0.24.90 TD 119728 
// 				{
// 					bMaskParam = IsProtectedParamResult( pszOLAString );
// 				}

				if ( bMaskParam == TRUE )
				{
					lValLen = sValEnd - sValPlace;  

					if (!ChrAll(sValPlace, lValLen, ' '))
					{
						lValEndPlace = sValEnd - szTmpIn;  
						_itoa(lValLen, sValLen, 10);

						//remove all NULL's from the string before insert it to the buffer.
						for(int i = 0; i < sizeof(sValLen); i++)
						{
							if (sValLen[i] == 0)
							{
								sValLen[i] = ' ';
							}
						}

						sEndStr = szTmpIn + min(strlen(szTmpIn),sizeof(szTmpIn));	//4.0.19.502

						if (lValLen > 3)
						{
							//copy the new val (the len of the old val)
							memcpy(sValPlace, sValLen, 3);

							if (lValEndPlace > 0 && (long)min(strlen(szTmpIn),sizeof(szTmpIn)) > lValEndPlace)
							{
								memcpy(sValPlace + 3,sValPlace + lValLen,min(strlen(szTmpIn),sizeof(szTmpIn)) - lValEndPlace);
							}

							//remove the chars that left in the end of the string.
							memset(sEndStr - lValLen + 3,0,lValLen - 3);
						}
					}
				}
			}
		}
		else
		{
			bRetCode = FALSE;
		}
	}

	strncpy(pszBuff, szTmpIn, min(iSizeOFOutBuffer, MAX_OLA_BUFFER_LEN));
	
	return bRetCode;
}



/******************************************************************************
 Description:	If RemoveCardDataFromLog is ON -> Replace Buffer Data with length	
 Returns:      	
 Parameters:   	sBufferChangeData -		buffer for Replace
				lBufferChangeDataSize - size of buffer for Replace
				pszOutBuff			  - Replaced buffer	
				 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/02/2009	12:36		Move from CProtectedDataHendler //4.0.22.13	TD 72716 //4.0.24.501 TD 75230
******************************************************************************/
BOOL CBasicProtectData::ReplaceBufferUseSize(char *pszOutBuff, const char *psBufferChangeData, const long lBufferChangeDataSize)const
{
	BOOL	bRetCode = TRUE;

	if(lBufferChangeDataSize > 0)
	{		
		char	szChangedData[STR_LONG_VAL10 + 1] = {0};

		sprintf(szChangedData, "%ld", lBufferChangeDataSize);				

		strcpy(pszOutBuff, szChangedData);
	}
	else
	{
		strcpy(pszOutBuff, " ");
		bRetCode = FALSE;	
	}
	
	return bRetCode;
}


/******************************************************************************
  Description:		
  Returns:      
  Parameters:  	
  
  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
  Gena			19/02/2009	13:00		Move from CProtectedDataHendler //4.0.22.13	TD 72716
 ******************************************************************************/
BOOL CBasicProtectData::MaskOLAString(char *pszOLAString, char *pszInBuff, char *pszOutBuff, long *plOutBuffLen) const
{
	BOOL	bRetCode = FALSE;
	char*	szTmpBuff;
	char	szData[MAX_FIELD_VAL * 2] ={0};
	char	szEncodeData[MAX_FIELD_VAL * 2]={0};
	long	lBuffBegin = 0;
	long	lCurrBuffLen = 0;
	long	lEncodeTrack2Len = 0;
	long	lEndLen = 0;
	long	lEndOfBuff = 0;
	long	lInBuffLen = strlen(pszInBuff);

	szTmpBuff = strstr(pszInBuff, pszOLAString);

	if(szTmpBuff)
	{
		char *strStart = strstr(szTmpBuff, "=");
		char *strEnd = strstr(szTmpBuff, ",");
		long lCardNumberLength = (long)strEnd - (long)strStart - 1;
		
		memset(szData, 0, sizeof(szData));
		memset(szEncodeData, 0, sizeof(szEncodeData));
		memcpy(szData, strStart + 1, min(sizeof(szData),lCardNumberLength));
				
		strcpy(szEncodeData, ""); //new case
		
		//in this case we have to check if we under account
		//if so we have to encrypt (XXXXXX......XXXX).
		if(!(strcmp("AccountNumber[S]=", pszOLAString)) ||
			!(strcmp("ClubCardTrack2[S]=", pszOLAString)))
		{
			bRetCode = MaskAccount(szEncodeData, szData);					
		}
		if(!(strcmp("Track2[S]=", pszOLAString)))
		{
			if (szData[strlen("Track2[S]=")] != ' ')
			{
				_ltoa(TRACK_2_DATA_SIZE, szData, 10);					
			}
		}				

		if( bRetCode == TRUE)
		{
			lBuffBegin = (long)strStart - (long)pszInBuff + 1;
			if(lBuffBegin > *plOutBuffLen)
				bRetCode = FALSE;
			else
			{
				memcpy(pszOutBuff,pszInBuff,lBuffBegin);
				lCurrBuffLen += lBuffBegin;
				lEncodeTrack2Len = min(strlen(szEncodeData),sizeof(szEncodeData));	
				if(lCurrBuffLen + lEncodeTrack2Len > *plOutBuffLen)
				{
					bRetCode = FALSE;
				}
				else
				{
					memcpy(pszOutBuff+lCurrBuffLen,szEncodeData,lEncodeTrack2Len);
					lCurrBuffLen += lEncodeTrack2Len;
					//copying the rest of the data
					lEndLen = strlen(strEnd);
					lEndOfBuff = min(lEndLen ,(*plOutBuffLen-lCurrBuffLen))-1;
					if(lEndOfBuff + lCurrBuffLen == *plOutBuffLen)
						lEndOfBuff--;
					memcpy(pszOutBuff+lCurrBuffLen,strEnd,lEndOfBuff);
					lCurrBuffLen += lEndOfBuff;
					*plOutBuffLen = lCurrBuffLen;
				}				
			}
		}
	}
	else
	{		
		memcpy(pszOutBuff,pszInBuff,strlen(pszInBuff));
	}
		
	return bRetCode;
}


/******************************************************************************
  Description:	Encrypt fields - AccountNumber, Track1, Track2, 
				from CARD_SALE_DATA struct.   	


  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
  Ravit 		25/04/2006   17:00		Start
  Liat			07/07/2008	 11:55		Change class name 4.0.20.501 TD-60236
  Gena			17/08/2008	19:00		Move from CProtectedDataHendler //4.0.22.13	TD 72716
******************************************************************************/

void CBasicProtectData::MaskOLAFields(CXMLInterface *sData)const
{
	char szBeforeEncrypt[ACCOUNT_NUM_SIZE];
	char szAfterEncrypt[ACCOUNT_NUM_SIZE];

	memset(szBeforeEncrypt,' ',sizeof(szBeforeEncrypt));
	memset(szAfterEncrypt,' ',sizeof(szAfterEncrypt));

	sData->GetXmlField(ATT_ACCOUNT_NUMBER,szBeforeEncrypt);
	
	if (!(ChrAll((char*)szBeforeEncrypt,sizeof(szBeforeEncrypt))))
	{
		MaskAccount((char*)szAfterEncrypt, (char*)&szBeforeEncrypt); 
		sData->SetXmlFieldWithSize(ATT_ACCOUNT_NUMBER,(char*)&szAfterEncrypt,ACCOUNT_NUM_SIZE);
	}
}


void CBasicProtectData::MaskOLAFields(CARD_SALE_ALL3 *sData)const
{
	char sAfterEncrypt[TRACK_2_DATA_SIZE];
	
	memset(sAfterEncrypt,' ',sizeof(sAfterEncrypt));
		
	MaskAccount((char*)sAfterEncrypt, (char*)sData->CardSaleAll.data.sAccountNumber); //4.0.20.501 TD-60236

	memcpy(sData->CardSaleAll.data.sAccountNumber, sAfterEncrypt, sizeof(sData->CardSaleAll.data.sAccountNumber));
}

void CBasicProtectData::MaskOLAFields(RECEIPT_DATA4_EX *sData)const  //4.0.27.45
{
	char sAfterEncrypt[TRACK_2_DATA_SIZE];
	
	memset(sAfterEncrypt,' ',sizeof(sAfterEncrypt));
		
	MaskAccount((char*)sAfterEncrypt, (char*)sData->CardSaleData.sAccountNumber); 

	memcpy(sData->CardSaleData.sAccountNumber, sAfterEncrypt, sizeof(sData->CardSaleData.sAccountNumber));
}

void CBasicProtectData::MaskOLAFields(CARD_SALE_ALL2 *sData)const  //4.0.27.45
{
	char sAfterEncrypt[TRACK_2_DATA_SIZE];
	
	memset(sAfterEncrypt,' ',sizeof(sAfterEncrypt));
		
	MaskAccount((char*)sAfterEncrypt, (char*)sData->CardSaleAll.data.sAccountNumber); 

	memcpy(sData->CardSaleAll.data.sAccountNumber, sAfterEncrypt, sizeof(sData->CardSaleAll.data.sAccountNumber));
}

void CBasicProtectData::GetMaskLengths(size_t* piHeadFreeLen, size_t* piTailFreeLen, const size_t iOpenStrLen, const MaskTypeEnum eMaskType)const
{	
	switch(m_eAccountMaskStandart)
	{
	case ANMS_HEAD6_TAIL4:
		{
			*piHeadFreeLen = PCI_COMPLIENT_MASK_NUM_START;
			*piTailFreeLen = PCI_COMPLIENT_MASK_NUM_END;
		}break;
	case ANMS_HEAD0_TAIL4:
		{
			*piHeadFreeLen = 0;
			*piTailFreeLen = PCI_COMPLIENT_MASK_NUM_END;
		}break;
	case ANMS_16_DIGIT_RULE:
		{				
			*piHeadFreeLen = (iOpenStrLen >= 16) ? PCI_COMPLIENT_MASK_NUM_START : 0;
			*piTailFreeLen = PCI_COMPLIENT_MASK_NUM_END;
		}break;
	default:
		{
			//case ANMS_HEAD0_TAIL4:
			*piHeadFreeLen = 0;
			*piTailFreeLen = PCI_COMPLIENT_MASK_NUM_END;
		}break;
	}
}


BOOL CBasicProtectData::MaskAccount(char* psMaskedAccountNumberOut, const char* psOpenAccountNumber, const MaskTypeEnum eMaskType)const
{
	BOOL bRetCode = TRUE;

	if( (psMaskedAccountNumberOut != NULL) && (psOpenAccountNumber != NULL) )
	{
		char	szInBackUp[ACCOUNT_NUM_SIZE+1] = {0};
		char	szOutBackUp[ACCOUNT_NUM_SIZE+1] = {0};

		memcpy(szInBackUp, psOpenAccountNumber, ACCOUNT_NUM_SIZE);

		if(!ChrAll(szInBackUp, ACCOUNT_NUM_SIZE, ' ') && (0 != *psOpenAccountNumber))
		{				
			memcpy(szOutBackUp, psMaskedAccountNumberOut, ACCOUNT_NUM_SIZE);
			int i=ACCOUNT_NUM_SIZE-1;
			for(i; i>0 && (' ' == szInBackUp[i]); i--);
			
			if(i>0)
				szInBackUp[i+1] = '\0';

			size_t iHeadFreeLen, iTailFreeLen;
			iHeadFreeLen = iTailFreeLen = 0;

			GetMaskLengths(&iHeadFreeLen, &iTailFreeLen, strlen(szInBackUp), eMaskType);
			
			MaskString(szOutBackUp, szInBackUp, PCI_COMPLIENT_MASK_CHAR, iHeadFreeLen, iTailFreeLen);

			memcpy(psMaskedAccountNumberOut, szOutBackUp, min(strlen(szOutBackUp), ACCOUNT_NUM_SIZE));
		}
	}

	return bRetCode;
}

/******************************************************************************
  Description:	Masking string	
  Returns:      0  - success -> pszMaskedStrOut will contain masked string
				-1 - fail	 -> pszMaskedStrOut will contain NULL

  Parameters:  	pszMaskedStrOut	-> Masked string [OUT]
				pszOpenStrIn	-> Open string to mask [IN]
				chMaskChar		-> Character setting 
				iHeadFreeLen	-> First iHeadMaskLen characters will be free
				iTailFreeLen	-> Last iTailMaskLen characters will be free
  
  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
  Gena			16/02/2009	10:00		Start //4.0.22.13	TD 72716
 ******************************************************************************/
long CBasicProtectData::MaskString(char* pszMaskedStrOut, const char* pszOpenStrIn, const char chMaskChar, const size_t iHeadFreeLen, const size_t iTailFreeLen)const
{
	long lRetCode = -1;

	if( (pszMaskedStrOut != NULL) && (pszOpenStrIn != NULL) )
	{		
		char szLocalDest[MAX_FIELD_VAL] = {0};

		try
		{
			strcpy(szLocalDest, pszOpenStrIn);

			size_t iMaskLength = (strlen(pszOpenStrIn) - (iHeadFreeLen + iTailFreeLen));

			_strnset(szLocalDest + iHeadFreeLen, chMaskChar, iMaskLength);				
	
			strcpy(pszMaskedStrOut, szLocalDest);

			lRetCode = OK;
		}
		catch (...)
		{
			pszMaskedStrOut = NULL;

			CString strLogMsg;
			strLogMsg.Format("MaskString FAILED! HeadFreeLen = %ld, TailFreeLen = %ld OpenStr = [%s]", iHeadFreeLen, iTailFreeLen, szLocalDest);
			_LOGMSG.LogMsg(strLogMsg, LOG_LEVEL_1);			
		}
	} 

	return lRetCode;
}


void CBasicProtectData::LoadConfiguration(const BOOL bCreate)
{
	DWORD dwTmp = 0;

	//***************************
	//**** If region is USA - Force remove card data from log !! //4.0.22.66
	//***************************
	if(_Module.m_server.IsCurrentRegion(REGION_USA) == TRUE)						//4.0.23.508 
	{		
		dwTmp = 1;
		_Module.m_server.m_cParam.SaveParam("GeneralParam", "RemoveCardDataFromLog", dwTmp, dwTmp, bCreate);	
	}
	//*************************** //4.0.22.66
	else
	{
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "RemoveCardDataFromLog", &dwTmp, (DWORD)0, bCreate);
	}

	m_bRemoveCardDataFromLog = (0 == dwTmp) ? FALSE : TRUE;
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PCIAccountMaskStandard", &dwTmp, (DWORD)0, bCreate);
	m_eAccountMaskStandart = (AccountNumberMaskStandartEnum)dwTmp;
}


//4.0.24.90 TD 119728 
BOOL CBasicProtectData::IsProtectedParamResult( char* pszOLAString ) const
{
	BOOL	bRetVal = FALSE;
	
	//Find 'PromptType' param in the pszOLAString
	const char	szParamName[] = "PromptType[I]=";
	char*		pStartParamName = NULL;
	
	pStartParamName = strstr( pszOLAString, szParamName);
	
	//Get param value
	if ( pStartParamName != NULL )
	{		
		const char*		pStartParamValue = pStartParamName + strlen( szParamName );
		const char*		pEndParamValue = strstr( pStartParamValue, "," );
		const size_t	nParamValueLen = pEndParamValue - pStartParamValue;

		if ( nParamValueLen != 0)
		{
			const long lParamValue = a2l( (unsigned char *)pStartParamValue, nParamValueLen);

			bRetVal = ( lParamValue == PT_CARD_READER || lParamValue == PT_BOOL_AND_CARD_READER );
		}
	}

	return bRetVal;
}