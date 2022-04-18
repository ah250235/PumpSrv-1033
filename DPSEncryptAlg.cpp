// DPSEncryptAlg.cpp: implementation of the CDPSEncryptAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DPSEncryptAlg.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDPSEncryptAlg::CDPSEncryptAlg() : CBaseEncodingAlg("DPSEncodingAlgorithm, ", __uuidof(COMInt))
									
{
}

CDPSEncryptAlg::~CDPSEncryptAlg()
{

}


//////////////////////////////////////////////////////////////////////
//				ConfigurationLoader overloaded Method
//////////////////////////////////////////////////////////////////////
//protected:
void CDPSEncryptAlg::LoadConfiguration(const BOOL bCreate)
{
	CDPSBaseEncodingAlg::LoadConfiguration(bCreate);

	m_bDeclinePAPOnDSSFail = TRUE;
	m_bDisablePAPOnDSSFail = TRUE;
}


/////////////////////////////////////////////////////////////////////
//				Main 
//////////////////////////////////////////////////////////////////////
/******************************************************************************
 Description:	Encode account number and save to CARD_SALE_ALL3 by Asymmetric Algorithm
 Returns:      	EncodeRetCodeEnum 
 Parameters:   	pcCardSaleAll3InOut - 
				pszAccountToEncode	- 
				lTrsNumber - trs number from DoAutoOLASendEnd
								default from GCI::Swipe
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/07/2009	 19:34		Start 4.0.23.0 TD-76407
******************************************************************************/
EncodeRetCodeEnum CDPSEncryptAlg::Algorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncrypt/*Not in use*/, const long lTrsNumber) 
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;

	if (IsPANEncoded(pcCardSaleAll3InOut) == FALSE)
		eEncodeRetCode = EncodePAN(pcCardSaleAll3InOut);

	if (eEncodeRetCode == ENCODE_OK)
	{
		if (IsLoyaltyANEncoded(pcCardSaleAll3InOut) == FALSE)
			eEncodeRetCode = EncodeLoyaltyAN(pcCardSaleAll3InOut);
	}

	//Encode response
	if (eEncodeRetCode != ENCODE_OK)
	{

	}				

	return eEncodeRetCode;
}

//4.0.23.20 TD 76407
EncodeRetCodeEnum  CDPSEncryptAlg::RestoreData(CARD_SALE_ALL3* pcCardSaleAll3InOut, const long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;

	eEncodeRetCode = DecodePAN(pcCardSaleAll3InOut);

	if (eEncodeRetCode == ENCODE_BUFF_TOO_SMALL)					//4.0.24.60 TD 106586
	{
		eEncodeRetCode = ENCODE_OK;

		if (LOG_LOGIC_ERROR)				
		{
			WriteEncodingLogMsg(0, lTrsNumber, "PAN is empty!");		
		}
	}

	if (eEncodeRetCode == ENCODE_OK)
	{
		eEncodeRetCode = DecodeLoyaltyAN(pcCardSaleAll3InOut);

		if (eEncodeRetCode == ENCODE_BUFF_TOO_SMALL)					//4.0.24.60 TD 106586
		{
			eEncodeRetCode = ENCODE_OK;

			if (LOG_LOGIC_ERROR)				
			{
				WriteEncodingLogMsg(0, lTrsNumber, "LoyaltyAN is empty!");		
			}
		}
	}

	//Encode response
	if (eEncodeRetCode != ENCODE_OK)
	{
		
	}				

	return eEncodeRetCode;
}


/*HRESULT _stdcall Decrypt ([in] BSTR EncryptedData, [in] long CryptoAlgorithm, [in] BSTR Certificate, [in, out] BSTR * DecryptedData, 
[in, out] long * ErrorCode, [in, out] BSTR * ErrorMessage, [out, retval] long * Value);*/


EncodeRetCodeEnum CDPSEncryptAlg::Decode(	char* psDecodedBuffOut, const char* pszEncodedBuffIn, const long lPumpNumber, 
										size_t* piDecodedDataSize, const long lTrsNumber)
{
	EncodeComDllRetCodeEnum		eEncodeDLLRetCode = ENCODE_DLL_OK;
	EncodeRetCodeEnum			eEncodeRetCode = ENCODE_FAIL;
	long						lErrorCode = OK;

	if(m_pEncodingComDll->IsPtrNULL() == FALSE)
	{
		try
		{
			char		szErrorMessage[MAX_FIELD_VAL] = {0};
			
			eEncodeRetCode = DecodeDLLEntryPoint(pszEncodedBuffIn, psDecodedBuffOut, &lErrorCode, szErrorMessage, piDecodedDataSize, lTrsNumber);

			if(eEncodeRetCode != ENCODE_OK)
			{
				if (LOG_LOGIC_ERROR)				
				{
					CString strLogMsg;
					strLogMsg.Format("Decoding failed! DPSRetErrorCode=%ld, RetCode=%ld, DPSRetErrorMessage=[%s]", lErrorCode, eEncodeRetCode, szErrorMessage);							

					WriteEncodingLogMsg(lPumpNumber, lTrsNumber, strLogMsg, LOG_LEVEL_1);		
				}
			}
		}
		catch(...)
		{
			if (LOG_LOGIC_ERROR )
			{				
				WriteEncodingLogMsg(lPumpNumber, lTrsNumber, "Decoding Failed!", LOG_LEVEL_1);			
			}
			
			eEncodeDLLRetCode = m_pEncodingComDll->RestoreCOM();
			eEncodeRetCode = ENCODE_FAIL; 
		}
	}
	else	
		WriteEncodingLogMsg(lPumpNumber, lTrsNumber, "EncodingComDll not init! Decoding Failed!", LOG_LEVEL_1);	

	if (eEncodeDLLRetCode!=ENCODE_DLL_OK || eEncodeRetCode!=ENCODE_OK)
		psDecodedBuffOut[0] = '\0';

	return eEncodeRetCode;
}


//////////////////////////////////////////////////////////////////////
//					DLL Entry Point to Encrypt\Decrypt Method
//////////////////////////////////////////////////////////////////////
EncodeRetCodeEnum CDPSEncryptAlg::EncodeDLLEntryPoint(const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piEncryptedDataSize, size_t* piHashedDataSize, long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_FAIL;
	long				lRetVal = ENCODE_DLL_OK;

	CComBSTR	cwsEncodedData(sizeof("  "), LPCSTR("  "));
	CComBSTR	cwsErrorMessage(sizeof("  "), LPCSTR("  "));
	_bstr_t		bstrDataToEncrypt(pszDataToEncode);
	_bstr_t		bstrCertificate(m_szCertificate);

	CEncodingComDll<_COMInt>::PtrInstance()->Encrypt(bstrDataToEncrypt, (long)0, bstrCertificate, &cwsEncodedData.m_str, plErrorCode, &cwsErrorMessage.m_str, &lRetVal);
		
	if(lRetVal == ENCODE_DLL_OK && *plErrorCode == ENCODE_OK)
	{
		long lSize = cwsEncodedData.Length();
		if (IsValidEncodedDataSize(lSize))
		{
			*piEncryptedDataSize = lSize;
			int iLen = Bstr2Buffer(cwsEncodedData.m_str, psEncodedData, lSize);

			eEncodeRetCode = ENCODE_OK; 

			CString strLogMsg;
			strLogMsg.Format("Encrypt succeed, buffLen=%ld", lSize);
			WriteEncodingLogMsg(0, lTrsNumber, strLogMsg);
		}
		else
			eEncodeRetCode = ENCODE_STRING_ILLEGAL_SIZE; 
	}
			
	if(eEncodeRetCode != ENCODE_OK)
	{
		eEncodeRetCode = ENCODE_ENCRYPTION_FAIL;

		Bstr2Buffer(cwsErrorMessage.m_str, szErrorMessage, cwsErrorMessage.Length());

		SendEncryptionFailedAlert(*plErrorCode, szErrorMessage, lTrsNumber);
	}

	return eEncodeRetCode;
}


			/*HRESULT _stdcall Decrypt ([in] BSTR EncryptedData, [in] long CryptoAlgorithm, [in] BSTR Certificate, [in, out] BSTR * DecryptedData, 
			[in, out] long * ErrorCode, [in, out] BSTR * ErrorMessage, [out, retval] long * Value);*/
//4.0.23.20	TD 76407
EncodeRetCodeEnum CDPSEncryptAlg::DecodeDLLEntryPoint(const char* pszEncodedData, char* psDecodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piDecodedDataSize, long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_FAIL;
	long				lRetVal = ENCODE_DLL_OK;

	CComBSTR	cwsDecodedData(sizeof("  "), LPCSTR("  "));
	CComBSTR	cwsErrorMessage(sizeof("  "), LPCSTR("  "));
	_bstr_t		bstrEncodedData(pszEncodedData);
	_bstr_t		bstrCertificate(m_szCertificate);

	CEncodingComDll<_COMInt>::PtrInstance()->Decrypt(bstrEncodedData, (long)0, bstrCertificate, &cwsDecodedData.m_str, plErrorCode, &cwsErrorMessage.m_str, &lRetVal);
		
	if(lRetVal == ENCODE_DLL_OK && *plErrorCode == ENCODE_OK)
	{
		long lSize = cwsDecodedData.Length();
		if (IsValidEncodedDataSize(lSize))
		{
			*piDecodedDataSize = lSize;
			int iLen = Bstr2Buffer(cwsDecodedData.m_str, psDecodedData, lSize);

			eEncodeRetCode = ENCODE_OK; 

			CString strLogMsg;
			strLogMsg.Format("Decrypt succeed, buffLen=%ld", lSize);
			WriteEncodingLogMsg(0, lTrsNumber, strLogMsg);
		}
		else
			eEncodeRetCode = ENCODE_STRING_ILLEGAL_SIZE; 
	}
			
	if(eEncodeRetCode != ENCODE_OK)
	{
		eEncodeRetCode = ENCODE_DECODING_FAIL;

		Bstr2Buffer(cwsErrorMessage.m_str, szErrorMessage, cwsErrorMessage.Length());
	}

	return eEncodeRetCode;
}


//////////////////////////////////////////////////////////////////////
//				Get Data from CARD_SALE_ALL3 Methods
//////////////////////////////////////////////////////////////////////
//protected:
void CDPSEncryptAlg::PANnCpy(char* szPAN, const size_t iPANSize, CARD_SALE_ALL3* const pcCardSaleAll3In)const
{
	memcpy(szPAN, pcCardSaleAll3In->CardSaleAll.data.sAccountNumber, min(iPANSize, sizeof(pcCardSaleAll3In->CardSaleAll.data.sAccountNumber)));

	ChangeSpaceCharToNull((BYTE*)szPAN, iPANSize);		
}


//protected:
void CDPSEncryptAlg::LoyaltyANnCpy(char* szLoyaltyAN, const size_t iLoyaltyANSize, CARD_SALE_ALL3* const pcCardSaleAll3In)const
{
	memcpy(szLoyaltyAN, pcCardSaleAll3In->CardSaleAll.extData.sLoyalId, min(iLoyaltyANSize, sizeof(pcCardSaleAll3In->CardSaleAll.extData.sLoyalId)));

	ChangeSpaceCharToNull((BYTE*)szLoyaltyAN, iLoyaltyANSize);		
}


//////////////////////////////////////////////////////////////////////
//				Set Data To CARD_SALE_ALL3 Methods
//////////////////////////////////////////////////////////////////////
//protected:
void CDPSEncryptAlg::PANnCpy(CARD_SALE_ALL3* pcCardSaleAll3In, const size_t iPANSize, const char* sPAN)const
{
	const size_t	iAccountNumberSize = sizeof(pcCardSaleAll3In->CardSaleAll.data.sAccountNumber);

	memset(pcCardSaleAll3In->CardSaleAll.data.sAccountNumber, ' ',	iAccountNumberSize);
	memcpy(pcCardSaleAll3In->CardSaleAll.data.sAccountNumber, sPAN, min(iPANSize, iAccountNumberSize));
}


//protected:
void CDPSEncryptAlg::LoyaltyANnCpy(CARD_SALE_ALL3* pcCardSaleAll3In, const size_t iLoyaltyANSize, const char* sLoyaltyAN)const
{
	const size_t	iLoyalIdSize = sizeof(pcCardSaleAll3In->CardSaleAll.extData.sLoyalId);

	memset(pcCardSaleAll3In->CardSaleAll.extData.sLoyalId, ' ',	iLoyalIdSize);
	memcpy(pcCardSaleAll3In->CardSaleAll.extData.sLoyalId, sLoyaltyAN, min(iLoyaltyANSize, iLoyalIdSize));
}


//protected:
// same as strncpy
size_t CDPSEncryptAlg::EncryptedPANnCpy(char* szEncryptedPAN, const size_t iEncryptedPANSize, CARD_SALE_ALL3* const pcCardSaleAll3In)const
{
	size_t			iRetLength = 0;
	const size_t	iEncryptedAccount2Size = sizeof(pcCardSaleAll3In->extData4.sEncryptedAccount2);

	memcpy(szEncryptedPAN, pcCardSaleAll3In->extData4.sEncryptedAccount2, min(iEncryptedPANSize, iEncryptedAccount2Size));
	
	ChangeSpaceCharToNull((BYTE*)szEncryptedPAN, iEncryptedPANSize);
	
	iRetLength = strlen(szEncryptedPAN);				//4.0.24.60 TD 106586

	return iRetLength;									//4.0.24.60 TD 106586
}


//protected:
// same as strncpy
size_t CDPSEncryptAlg::EncryptedLoyaltyANnCpy(char* szEncryptedLoyaltyAN, const size_t iEncryptedLoyaltyANSize, CARD_SALE_ALL3* const pcCardSaleAll3In)const
{
	size_t			iRetLength = 0;
	const size_t	iEncryptedAccount3Size = sizeof(pcCardSaleAll3In->extData5.sEncryptedAccount3);

	memcpy(szEncryptedLoyaltyAN, pcCardSaleAll3In->extData5.sEncryptedAccount3, min(iEncryptedLoyaltyANSize, iEncryptedAccount3Size));
	
	ChangeSpaceCharToNull((BYTE*)szEncryptedLoyaltyAN, iEncryptedLoyaltyANSize);
	
	iRetLength = strlen(szEncryptedLoyaltyAN);			//4.0.24.60 TD 106586

	return iRetLength;									//4.0.24.60 TD 106586
}


//////////////////////////////////////////////////////////////////////
//				Encode Data Methods
//////////////////////////////////////////////////////////////////////
//protected:
EncodeRetCodeEnum CDPSEncryptAlg::EncodePAN(CARD_SALE_ALL3* pcCardSaleAll3InOut)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;
	char				sEncryptedAccount[ENCRYPTED_DATA_SIZE];	
	size_t				iEncryptedDataSize = 0;
	long				lPumpNumber = GetPumpNumber(pcCardSaleAll3InOut);
	char				szPAN[ENCRYPTED_DATA_SIZE];

	memset(sEncryptedAccount, ' ', sizeof(sEncryptedAccount));
	memset(szPAN,			 0x00, sizeof(szPAN));

	PANnCpy(szPAN, sizeof(szPAN), pcCardSaleAll3InOut);

	if(IsFieldNotEmpty(szPAN, sizeof(szPAN)) == TRUE)
	{
		eEncodeRetCode = Encode(sEncryptedAccount, szPAN, lPumpNumber, &iEncryptedDataSize);

		if (eEncodeRetCode == ENCODE_OK)
		{
			SaveEncryptedData(pcCardSaleAll3InOut, sEncryptedAccount, iEncryptedDataSize);
			SaveHashedData(pcCardSaleAll3InOut, szPAN, iEncryptedDataSize);
		}
	}

	return eEncodeRetCode;
}


//protected:
EncodeRetCodeEnum CDPSEncryptAlg::EncodeLoyaltyAN(CARD_SALE_ALL3* pcCardSaleAll3InOut)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;

	char				sEncryptedAccount[ENCRYPTED_DATA_SIZE];	
	size_t				iEncryptedDataSize = 0;
	long				lPumpNumber = GetPumpNumber(pcCardSaleAll3InOut);
	char				szLoyaltyAN[ENCRYPTED_DATA_SIZE];

	memset(sEncryptedAccount, ' ', sizeof(sEncryptedAccount));
	memset(szLoyaltyAN,		 0x00, sizeof(szLoyaltyAN));

	LoyaltyANnCpy(szLoyaltyAN, sizeof(szLoyaltyAN), pcCardSaleAll3InOut);

	if(IsFieldNotEmpty(szLoyaltyAN, sizeof(szLoyaltyAN)) == TRUE)
	{
		eEncodeRetCode = Encode(sEncryptedAccount, szLoyaltyAN, lPumpNumber, &iEncryptedDataSize);

		if (eEncodeRetCode == ENCODE_OK)
		{
			SaveEncryptedData2(pcCardSaleAll3InOut, sEncryptedAccount, iEncryptedDataSize);
		}
	}

	return eEncodeRetCode;
}



//////////////////////////////////////////////////////////////////////
//				Decode Data Methods
//////////////////////////////////////////////////////////////////////
//protected:
EncodeRetCodeEnum CDPSEncryptAlg::DecodePAN(CARD_SALE_ALL3* pcCardSaleAll3InOut)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;

	char				szEncryptedPAN[ENCRYPTED_DATA_SIZE];
	char				sDecryptedPAN[ENCRYPTED_DATA_SIZE];	
	long				lPumpNumber = GetPumpNumber(pcCardSaleAll3InOut);
	size_t				iDecryptedDataSize = 0;
	const size_t		iEncryptedPANSize = sizeof(szEncryptedPAN);

	memset(szEncryptedPAN, 0x00, iEncryptedPANSize);
	memset(sDecryptedPAN,   ' ', sizeof(sDecryptedPAN));

	EncryptedPANnCpy(szEncryptedPAN, iEncryptedPANSize, pcCardSaleAll3InOut);

	if(IsFieldNotEmpty(szEncryptedPAN, sizeof(szEncryptedPAN)) == TRUE)					//4.0.24.60 TD 106586
	{
		eEncodeRetCode = Decode(sDecryptedPAN, szEncryptedPAN, lPumpNumber, &iDecryptedDataSize);

		if (eEncodeRetCode == ENCODE_OK)
		{
			PANnCpy(pcCardSaleAll3InOut, iDecryptedDataSize, sDecryptedPAN);
		}
	}
	else
		eEncodeRetCode = ENCODE_BUFF_TOO_SMALL;			//4.0.24.60 TD 106586

	return eEncodeRetCode;
}


//protected:
EncodeRetCodeEnum CDPSEncryptAlg::DecodeLoyaltyAN(CARD_SALE_ALL3* pcCardSaleAll3InOut)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;

	char				szEncryptedLoyaltyAN [ENCRYPTED_DATA_SIZE];
	char				sDecryptedLoyaltyAN [ENCRYPTED_DATA_SIZE];	
	long				lPumpNumber = GetPumpNumber(pcCardSaleAll3InOut);
	size_t				iDecryptedDataSize = 0;
	const size_t		iEncryptedLoyaltyANSize = sizeof(szEncryptedLoyaltyAN);

	memset(szEncryptedLoyaltyAN, 0x00, iEncryptedLoyaltyANSize);
	memset(sDecryptedLoyaltyAN, ' ', sizeof(sDecryptedLoyaltyAN));

	EncryptedLoyaltyANnCpy(szEncryptedLoyaltyAN, iEncryptedLoyaltyANSize, pcCardSaleAll3InOut);
	
	if(IsFieldNotEmpty(szEncryptedLoyaltyAN, sizeof(szEncryptedLoyaltyAN)) == TRUE)					//4.0.24.60 TD 106586		
	{
		eEncodeRetCode = Decode(sDecryptedLoyaltyAN, szEncryptedLoyaltyAN, lPumpNumber, &iDecryptedDataSize);

		if (eEncodeRetCode == ENCODE_OK)
		{
			LoyaltyANnCpy(pcCardSaleAll3InOut, iDecryptedDataSize, sDecryptedLoyaltyAN);
		}
	}
	else
		eEncodeRetCode = ENCODE_BUFF_TOO_SMALL;														//4.0.24.60 TD 106586

	return eEncodeRetCode;
}

//////////////////////////////////////////////////////////////////////
//				Save Encoded Data to CARD_SALE_ALL3 Methods
//////////////////////////////////////////////////////////////////////
void  CDPSEncryptAlg::SaveHashedData(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* psHashedData, const size_t iHashedDataSize)
{
	char	sMaskedAccount[ACCOUNT_NUM_SIZE];

	memset(sMaskedAccount, ' ', sizeof(sMaskedAccount));

	MaskAccount(sMaskedAccount, psHashedData, MASK_TYPE_DB);

	CBaseEncodingAlg::SaveHashedData(pcCardSaleAll3InOut, sMaskedAccount, sizeof(sMaskedAccount));
}


//////////////////////////////////////////////////////////////////////
//					Control Methods
//////////////////////////////////////////////////////////////////////

inline BOOL CDPSEncryptAlg::IsPANEncoded(const CARD_SALE_ALL3* pcCardSaleAll3) const
{
	BOOL bRetCode = FALSE;

	bRetCode = (IsFieldNotEmpty((char*)pcCardSaleAll3->extData4.sEncryptedAccount2, sizeof(pcCardSaleAll3->extData4.sEncryptedAccount2))) ? TRUE : FALSE;

	return bRetCode;
}


inline BOOL CDPSEncryptAlg::IsLoyaltyANEncoded(const CARD_SALE_ALL3* pcCardSaleAll3) const
{
	BOOL bRetCode = FALSE;

	bRetCode = (IsFieldNotEmpty((char*)pcCardSaleAll3->extData5.sEncryptedAccount3, sizeof(pcCardSaleAll3->extData5.sEncryptedAccount3))) ? TRUE : FALSE;

	return bRetCode;
}


inline BOOL	CDPSEncryptAlg::IsPAPEnabled() const 
{ 
	 BOOL bRetCode = ((m_dwCurrentInitState & FESS_PAP_DISABLED) ? FALSE : TRUE);

	 return bRetCode;
}


inline BOOL	CDPSEncryptAlg::IsAlgorithmEnabled(const short nSessionType) const 
{
	 BOOL bRetCode = (((nSessionType & OLA_LOYALTY_AUTHORIZE_PAYMENT || nSessionType & OLA_LOYALTY_AUTHORIZE_LOYALTY)) ? TRUE : FALSE);

	 return bRetCode;
}


BOOL CDPSEncryptAlg::IsRestoreDataEnabled(CARD_SALE_ALL3* const pcCardSaleAll3In) const
{
	BOOL bRetCode = FALSE;
	
	bRetCode = (IsRestorePANEnabled(pcCardSaleAll3In) == FALSE) ? IsRestoreLoyaltyANEnabled(pcCardSaleAll3In) : TRUE;

	return bRetCode;
}


BOOL CDPSEncryptAlg::IsRestorePANEnabled(CARD_SALE_ALL3* const pcCardSaleAll3In) const
{
	BOOL			bRetCode = FALSE;

	char			szPAN[ENCRYPTED_DATA_SIZE];
	const size_t	iPANSize = sizeof(szPAN);

	memset(szPAN, 0, iPANSize);
	PANnCpy(szPAN, iPANSize, pcCardSaleAll3In);

	bRetCode = (IsFieldNotEmpty(szPAN, min(strlen(szPAN), iPANSize)) == FALSE && IsPANEncoded(pcCardSaleAll3In) == TRUE) ? TRUE : FALSE;

	return bRetCode;
}


BOOL CDPSEncryptAlg::IsRestoreLoyaltyANEnabled(CARD_SALE_ALL3* const pcCardSaleAll3In) const
{
	BOOL			bRetCode = FALSE;

	char			szLoyaltyAN[ENCRYPTED_DATA_SIZE];
	const size_t	iLoyaltyANSize = sizeof(szLoyaltyAN);

	memset(szLoyaltyAN, 0, iLoyaltyANSize);
	LoyaltyANnCpy(szLoyaltyAN, iLoyaltyANSize, pcCardSaleAll3In);

	bRetCode = (IsFieldNotEmpty(szLoyaltyAN, min(strlen(szLoyaltyAN), iLoyaltyANSize)) == FALSE && IsLoyaltyANEncoded(pcCardSaleAll3In) == TRUE) ? TRUE : FALSE;

	return bRetCode;
}


//4.0.24.60 TD 112773
void CDPSEncryptAlg::RemoveConfidencesDataFromMemory(short bySessionType, PAY_AT_PUMP_INFO* pInfoInOut)
{
	if ( SESSION_TYPE(bySessionType) == SESSION_COMPLETE && pInfoInOut->CardSaleExtraData4.byIsRemoveProhibitedData == 'Y')
	{
		memset(pInfoInOut->CardSaleInfo.cardData.sTrack1Data,		' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sTrack1Data));
		memset(pInfoInOut->CardSaleInfo.cardData.sTrack2Data,		' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sTrack2Data));
		//memset(pInfoInOut->CardSaleInfo.cardData.sAccountNumber,	' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sAccountNumber)); //4.0.23.1520
	    //memset(pInfoInOut->CardSaleInfo.cardData.sPINBlock,			' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sPINBlock));
		//memset(pInfoInOut->CardSaleExtraData2.sResInfoAMax37 ,		' ', sizeof(pInfoInOut->CardSaleExtraData2.sResInfoAMax37)); //TD 276656 4.0.124.471
		memset(pInfoInOut->CardSaleExtraData3.sParamResult,		' ', sizeof(pInfoInOut->CardSaleExtraData3.sParamResult)); //TD 276656 4.0.124.471

		WriteEncodingLogMsg("Confidences data removed from the memory!111");
	}
}


// TD 329049
CSADPSEncryptAlg::CSADPSEncryptAlg() : CBaseEncodingAlg("CSADPSEncryptAlgorithm, ", __uuidof(COMIntV2)) //  4.0.31.80 TD 373856					
{

}
// TD 329049
CSADPSEncryptAlg::~CSADPSEncryptAlg()
{
}

// TD 329049
void CSADPSEncryptAlg::LoadConfiguration(const BOOL bCreate)
{
	CBaseEncodingAlg::LoadConfiguration(bCreate);

	memset(m_szConfigFileName, 0, sizeof(m_szConfigFileName));
		
	//_Module.m_server.m_cParam.LoadParam("Software\\Retalix\\Retalix DPS","Path", (char*)m_szConfigFileName,sizeof(m_szConfigFileName),"",TRUE);

	_Module.m_server.m_cParam.LoadParam("Paths\\Long", "DPSConfigFile", m_szConfigFileName, sizeof(m_szConfigFileName), " ", bCreate);	

	if (IsConfigFileNameCorrupted() == TRUE)
	{
		WriteEncodingLogMsg("DPSConfigFile is corrupted!", LOG_LEVEL_1);
		memset(m_szConfigFileName, 0x00, sizeof(m_szConfigFileName));
	}
}

EncodeComDllRetCodeEnum	CSADPSEncryptAlg::Init(BOOL bLoadConfig) //  4.0.31.80 TD 373856
{
    EncodeComDllRetCodeEnum eEncodeDLLRetCode	= ENCODE_DLL_OK;
	long					lEncodeDLLErrorCode = ENCODE_DLL_UNKNOWN_ERROR;
	long					lRetCode = -1;
	char					szErrorMessage[MAX_FIELD_VAL] = {0};
	char					szCertificate[MAX_FIELD_VAL] = {0};
	
	if(TRUE == bLoadConfig)
		LoadConfiguration();

	if (IsConfigFileNameCorrupted() == TRUE)
	{
		eEncodeDLLRetCode = ENCODE_DLL_CUSTOMIZE_ERROR;
	}

	if(eEncodeDLLRetCode == ENCODE_DLL_OK)
	{
		if(m_pEncodingComDll->IsPtrNULL() == TRUE)
			eEncodeDLLRetCode = m_pEncodingComDll->Init();

		if( ENCODE_DLL_OK == eEncodeDLLRetCode )
		{		
			try
			{				
				_bstr_t		bstrConfigFileName(m_szConfigFileName);
				char        sTemp[MAX_TABLE_NAME] = {' '};            
				CComBSTR	cwsErrorMessage(sizeof(sTemp),	(LPCSTR)sTemp);
	
				CEncodingComDll<_COMIntV2>::PtrInstance()->InitializeAPI(	bstrConfigFileName,	&lEncodeDLLErrorCode, 	&cwsErrorMessage.m_str,&lRetCode);
				if( OK != lRetCode )
				{
					if (LOG_LOGIC_ERROR)				
					{
						int iLen = Bstr2Buffer(cwsErrorMessage, szErrorMessage, sizeof(szErrorMessage) - 1);
			
						if(iLen == 0)
							memset(szErrorMessage, 0x00, sizeof(szErrorMessage));

						CString strErrMsg;
						strErrMsg.Format("InitializeAPI Failed! RetCode=%ld, EncodeDLLErrorCode = %ld, ErrorMessage=[%s]", lRetCode, lEncodeDLLErrorCode, szErrorMessage);
						
						WriteEncodingLogMsg(strErrMsg, LOG_LEVEL_1);
					}
				}
				else
				{				
					WriteEncodingLogMsg("InitializeAPI Succeed!");

					CComBSTR	cwsCertificate(sizeof(m_szCertificate),(LPCSTR)m_szCertificate);

					_bstr_t		bstrAppID("RetalixApplicationTest");				//Test to be rem! 4.0.23.20
					_bstr_t		bstrEmptyForTest(" ");								//Test to be rem! 4.0.23.20

					lEncodeDLLErrorCode = lRetCode = OK;

					cwsErrorMessage.Empty();
					cwsErrorMessage = "  ";

					CEncodingComDll<_COMIntV2>::PtrInstance()->InitializeSession(	bstrAppID,								/*App ID*/
 																				bstrEmptyForTest,							/*User name*/
 																				bstrEmptyForTest,							/*Password*/
																				&cwsCertificate.m_str,						/*Certificate OUT*/
																				&lEncodeDLLErrorCode,
																				&cwsErrorMessage.m_str,
																				&lRetCode );									

					int iLen = Bstr2Buffer(cwsCertificate, szCertificate, sizeof(szCertificate) - 1);
			
					if(iLen == 0)
						memset(szCertificate, 0x00, sizeof(szCertificate));

					iLen = Bstr2Buffer(cwsErrorMessage, szErrorMessage, sizeof(szErrorMessage) - 1);
			
					if(iLen == 0)
						memset(szErrorMessage, 0x00, sizeof(szErrorMessage));
				}
			}
			catch (...)
			{
				lEncodeDLLErrorCode = lRetCode = ENCODE_FAIL;
				memset(szErrorMessage, 0x00, sizeof(szErrorMessage));
				memset(szCertificate, 0x00, sizeof(szCertificate));

				DWORD dwLastError = GetLastError();

				CString strLogMsg;
				strLogMsg.Format("Initialize Failed!!! Error=%ld", dwLastError);

				WriteEncodingLogMsg(strLogMsg, LOG_LEVEL_1);				
			}
		}
	}
	else
	{
		CString strLogMsg;
		strLogMsg.Format("Initialize Canceled!!! Error=%ld", eEncodeDLLRetCode);

		WriteEncodingLogMsg(strLogMsg, LOG_LEVEL_1);	
	}

	eEncodeDLLRetCode = InitResponse(lRetCode, szCertificate, lEncodeDLLErrorCode, szErrorMessage);		//4.0.23.508 TD 93615

	return eEncodeDLLRetCode;
}

EncodeComDllRetCodeEnum	 CSADPSEncryptAlg::CloseSession()
{
	EncodeComDllRetCodeEnum eEncodeDLLRetCode = ENCODE_DLL_OK;

	if (m_pEncodingComDll->IsPtrNULL() == FALSE)
	{	
		try
		{
			long		lEncodeDLLErrorCode = ENCODE_DLL_OK;
			long		lRetCode = OK;

			char        sTemp[MAX_TABLE_NAME];            
			memset(sTemp, ' ', sizeof(sTemp));								//4.0.22.90
		
			_bstr_t		bstrCertificate(m_szCertificate);
			CComBSTR	cwsErrorMessage(sizeof(sTemp),	(LPCSTR)sTemp);

			CEncodingComDll<_COMIntV2>::PtrInstance()->CloseSession(	bstrCertificate,						/*Certificate IN*/
																	&lEncodeDLLErrorCode,
																	&cwsErrorMessage.m_str,
																	&lRetCode);
		
			CString strLogMsg;
			long	lLogLevel = DEFAULT_LOG_LEVEL;

			if (OK != lRetCode)
			{
				char		szErrorMessage[MAX_FIELD_VAL] = {0};

				int iLen = Bstr2Buffer(cwsErrorMessage, szErrorMessage, sizeof(szErrorMessage) - 1);
			
				strLogMsg.Format("CloseSession Failed!!! GeneralRetCode=%ld, ExtendedErrorCode=%ld, ErrorMessage=[%s]", lRetCode, lEncodeDLLErrorCode, szErrorMessage);
				lLogLevel = LOG_LEVEL_1;
				
				eEncodeDLLRetCode = ENCODE_DLL_CLOSE_SESSION_FAIL;
			}
			else
			{		
				strLogMsg.Format("CloseSession Succeed!");
			}
			
			WriteEncodingLogMsg(strLogMsg, lLogLevel);
		}
		catch (...)
		{
			DWORD dwLastError = GetLastError();
			
			CString strLogMsg;
			strLogMsg.Format("CloseSession Failed!!! Error=%ld", dwLastError);
			
			WriteEncodingLogMsg(strLogMsg, LOG_LEVEL_1);				
			
			eEncodeDLLRetCode = ENCODE_DLL_CLOSE_SESSION_FAIL;
		}
	}
	else
		eEncodeDLLRetCode = ENCODE_DLL_NOT_CONNECTED;
	
	return eEncodeDLLRetCode;
}


EncodeRetCodeEnum CSADPSEncryptAlg::EncodeDLLEntryPoint(const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piEncryptedDataSize, size_t* piHashedDataSize, long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_FAIL;
	long				lRetVal = ENCODE_DLL_OK;

	CComBSTR	cwsEncodedData(sizeof("  "), LPCSTR("  "));
	CComBSTR	cwsErrorMessage(sizeof("  "), LPCSTR("  "));
	_bstr_t		bstrDataToEncrypt(pszDataToEncode);
	_bstr_t		bstrCertificate(m_szCertificate);

	CEncodingComDll<_COMIntV2>::PtrInstance()->Encrypt(bstrDataToEncrypt, (long)0, bstrCertificate, &cwsEncodedData.m_str, plErrorCode, &cwsErrorMessage.m_str, &lRetVal);
		
	if(lRetVal == ENCODE_DLL_OK && *plErrorCode == ENCODE_OK)
	{
		long lSize = cwsEncodedData.Length();
		if (IsValidEncodedDataSize(lSize))
		{
			*piEncryptedDataSize = lSize;
			int iLen = Bstr2Buffer(cwsEncodedData.m_str, psEncodedData, lSize);

			eEncodeRetCode = ENCODE_OK; 

			CString strLogMsg;
			strLogMsg.Format("Encrypt succeed, buffLen=%ld", lSize);
			WriteEncodingLogMsg(0, lTrsNumber, strLogMsg);
		}
		else
			eEncodeRetCode = ENCODE_STRING_ILLEGAL_SIZE; 
	}
			
	if(eEncodeRetCode != ENCODE_OK)
	{
		eEncodeRetCode = ENCODE_ENCRYPTION_FAIL;

		Bstr2Buffer(cwsErrorMessage.m_str, szErrorMessage, cwsErrorMessage.Length());

		SendEncryptionFailedAlert(*plErrorCode, szErrorMessage, lTrsNumber);
	}

	return eEncodeRetCode;
}

EncodeRetCodeEnum CSADPSEncryptAlg::DecodeDLLEntryPoint(const char* pszEncodedData, char* psDecodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piDecodedDataSize, long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_FAIL;
	long				lRetVal = ENCODE_DLL_OK;

	CComBSTR	cwsDecodedData(sizeof("  "), LPCSTR("  "));
	CComBSTR	cwsErrorMessage(sizeof("  "), LPCSTR("  "));
	_bstr_t		bstrEncodedData(pszEncodedData);
	_bstr_t		bstrCertificate(m_szCertificate);

	CEncodingComDll<_COMIntV2>::PtrInstance()->Decrypt(bstrEncodedData, (long)0, bstrCertificate, &cwsDecodedData.m_str, plErrorCode, &cwsErrorMessage.m_str, &lRetVal);
		
	if(lRetVal == ENCODE_DLL_OK && *plErrorCode == ENCODE_OK)
	{
		long lSize = cwsDecodedData.Length();
		if (IsValidEncodedDataSize(lSize))
		{
			*piDecodedDataSize = lSize;
			int iLen = Bstr2Buffer(cwsDecodedData.m_str, psDecodedData, lSize);

			eEncodeRetCode = ENCODE_OK; 

			CString strLogMsg;
			strLogMsg.Format("Decrypt succeed, buffLen=%ld", lSize);
			WriteEncodingLogMsg(0, lTrsNumber, strLogMsg);
		}
		else
			eEncodeRetCode = ENCODE_STRING_ILLEGAL_SIZE; 
	}
			
	if(eEncodeRetCode != ENCODE_OK)
	{
		eEncodeRetCode = ENCODE_DECODING_FAIL;

		Bstr2Buffer(cwsErrorMessage.m_str, szErrorMessage, cwsErrorMessage.Length());
	}

	return eEncodeRetCode;
}