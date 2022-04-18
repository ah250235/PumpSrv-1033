// DSSBothAsymmetricAlg.cpp: implementation of the CDSSBothAsymmetricAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSSBothAsymmetricAlg.h"


//--------------------------------------------------------------------
//					CDSSBothAsymmetricAlg class	  //4.0.22.40 TD 72716
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDSSBothAsymmetricAlg::CDSSBothAsymmetricAlg() : 
						CBaseEncodingAlg("DSSEncodingAlgorithm, ", CLSID_Cryptography)
{
	m_dwCurrentInitState = FESS_PAP_ENABLED;
	m_byStoreMode = m_byActualMode = INIT_MODE;
	m_byMaxTrsNumberToProcessOnDSSFail = 0;
	m_bDSSTestMode = m_bDisableEncrypt = FALSE;
	m_bSendChangedAlert = TRUE;
	m_byTrsNumberToProcessCount = 0;
	memset(m_sEncryptionKeyRef, ' ', sizeof(m_sEncryptionKeyRef));
	m_bLookForInternalReceiptIndex = TRUE;					//4.0.26.500 TD 144219
}

						
CDSSBothAsymmetricAlg::~CDSSBothAsymmetricAlg()
{

}


/******************************************************************************
 Description:	Response to Initialize session

 Returns:      	ENCODE_DLL_FAIL_INITIALIZATON
				ENCODE_DLL_OK

 Parameters:   	Initialize session return arguments
				const long lRetCode
				const CComBSTR& cwsCertificate
				const long lEncodeDLLErrorCode
				const CComBSTR& cwsErrorMessage

 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			23/03/2009	 13:34		Start 4.0.22.40 TD-72716
******************************************************************************/
//protected:
EncodeComDllRetCodeEnum	CDSSBothAsymmetricAlg::InitResponse(const long	lRetCode, 
															const char* szCertificate, 
															const long	lEncodeDLLErrorCode, 
															const char* szErrorMessage)
{
	EncodeComDllRetCodeEnum	eEncodeDLLRetCode = ENCODE_DLL_OK;

 	if (lRetCode == OK)
	{
		const size_t iCertificateLen = strlen(szCertificate);

		if (iCertificateLen > 0)																
		{
			memset(m_szCertificate, 0x00,			sizeof(m_szCertificate));						//4.0.22.90
			memcpy(m_szCertificate, szCertificate,	min(sizeof(m_szCertificate), iCertificateLen));	

			if(strlen(szErrorMessage) > 0)
			{
				memset(m_sEncryptionKeyRef, ' ',			sizeof(m_sEncryptionKeyRef));												//4.0.22.90
				memcpy(m_sEncryptionKeyRef, szErrorMessage, min(sizeof(m_sEncryptionKeyRef), strlen(szErrorMessage)));					//4.0.22.90
			}

			if( lEncodeDLLErrorCode != ENCODE_DLL_OK )
			{
				if( ( (STORE_MODE_HASH == m_byStoreMode) && (IsHashFailedOnInit(lEncodeDLLErrorCode) == TRUE)) ||							//4.0.22.73 TD 76455
					( ((STORE_MODE_ENCRYPT_AND_HASH == m_byStoreMode) || (STORE_MODE_MASK_AND_ENCRYPT_AND_HASH == m_byStoreMode)) && 
						(IsEncryptFailedOnInit(lEncodeDLLErrorCode) == TRUE)) )															//4.0.22.92 TD 77501
				{			
					eEncodeDLLRetCode = ENCODE_DLL_FAIL_INITIALIZATON;
				}
			}
		}
		else
		{
			DWORD dwLastError = GetLastError();

			CString strErrMsg;
 			strErrMsg.Format("Initialize Failed!!! Certificate Len=%ld, Error=%ld", iCertificateLen, dwLastError);

			WriteEncodingLogMsg(strErrMsg, LOG_LEVEL_1);

			eEncodeDLLRetCode = ENCODE_DLL_FAIL_INITIALIZATON;
		}	
	}
	else
	{
		eEncodeDLLRetCode = ENCODE_DLL_FAIL_INITIALIZATON;
	}			
	
	if(eEncodeDLLRetCode != ENCODE_DLL_OK)
	{
		UpdateActualMode(FALSE);

		if (LOG_LOGIC_ERROR)				
		{			
			CString strErrMsg;
			strErrMsg.Format("Initialize Failed! RetCode=%ld, EncodeDLLErrorCode=%ld, StoreMode=%ld, ErrorMessage=[%s]", 
																				lRetCode, lEncodeDLLErrorCode, m_byStoreMode, szErrorMessage);
			
			WriteEncodingLogMsg(strErrMsg, LOG_LEVEL_1);
		}

		SendInitializationFailedAlert(lEncodeDLLErrorCode, szErrorMessage);
	}
	else //if (eEncodeDLLRetCode == ENCODE_DLL_OK)
	{
		UpdateActualMode(TRUE);
		
		if (LOG_DETAIL_FLOW_CONTROL)				
		{
			CString strLogMsg;
			strLogMsg.Format("Initialize Succeed! EncodeDLLErrorCode=%ld, StoreMode=%ld, ErrorMessage=[%s]", 
																					lEncodeDLLErrorCode, m_byStoreMode, szErrorMessage);
			WriteEncodingLogMsg(strLogMsg); 
		}

		if ( ((STORE_MODE_ENCRYPT_AND_HASH == m_byStoreMode) || (STORE_MODE_MASK_AND_ENCRYPT_AND_HASH == m_byStoreMode)) 
				&& IsHashFailedOnInit(lEncodeDLLErrorCode) == TRUE )														//4.0.22.110	TD 77501
		{
			SendInitializationFailedAlert(lEncodeDLLErrorCode, szErrorMessage);
		}
		else
			StopInitializationFailedAlert();		//4.0.22.74 
	}

	m_byTrsNumberToProcessCount = 0;			//4.0.22.92	TD 77461

	return eEncodeDLLRetCode;
}


inline void	CDSSBothAsymmetricAlg::IncTrsNumberToProcessCount()
{ 
	m_byTrsNumberToProcessCount++; 
}


//4.0.22.110	TD 77501
inline BOOL CDSSBothAsymmetricAlg::IsEncryptFailedOnInit(const long lEncodeDLLErrorCode)const
{
	BOOL bRetCode = (-9 <= lEncodeDLLErrorCode && 0 > lEncodeDLLErrorCode) ? TRUE : FALSE;

	return bRetCode;
}


//4.0.22.110	TD 77501
inline BOOL CDSSBothAsymmetricAlg::IsHashFailedOnInit(const long lEncodeDLLErrorCode)const
{
	BOOL bRetCode = (-9 > lEncodeDLLErrorCode && 0 > lEncodeDLLErrorCode) ? TRUE : FALSE;

	return bRetCode;
}


//////////////////////////////////////////////////////////////////////
//				ConfigurationLoader overloaded Method
//////////////////////////////////////////////////////////////////////
//protected:
void CDSSBothAsymmetricAlg::LoadConfiguration(const BOOL bCreate)
{
	DWORD dwTmp = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "DSSTestMode", &dwTmp, (DWORD)0, bCreate);
	m_bDSSTestMode = (0 == dwTmp) ? FALSE : TRUE;
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "DisableEncrypt", &dwTmp, (DWORD)0, bCreate);	
	m_bDisableEncrypt = (0 == dwTmp) ? FALSE : TRUE;

	UpdateStoreMode();

	UpdateActualMode(TRUE);

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "MaxTrsNumberToProcessOnDSSFail", &dwTmp, (DWORD)0, bCreate);	
	m_byMaxTrsNumberToProcessOnDSSFail = (BYTE)dwTmp;
}


//protected:
void CDSSBothAsymmetricAlg::ReInit()
{
	memset(m_sEncryptionKeyRef, ' ', sizeof(m_sEncryptionKeyRef)); //4.0.22.91

	CBaseEncodingAlg::ReInit();
	
	
}


//4.0.22.73 TD 76462
BOOL CDSSBothAsymmetricAlg::IsReInitRequired()	
{
	BOOL bRetCode = FALSE;
	BYTE byStoreMode = m_byStoreMode;

	LoadConfiguration();

	bRetCode = IsStoreModeChanged(byStoreMode);

	return bRetCode;
}

//////////////////////////////////////////////////////////////////////
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
 Gena			23/03/2009	 13:34		Start 4.0.22.40 TD-72716
******************************************************************************/

EncodeRetCodeEnum CDSSBothAsymmetricAlg::Algorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncode, const long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;

	//If sPanRequiredProtection is False, - no need to perform algorithm.
	//we simply copy the sFormattedAccount to sEncryptedAccount2.

	if(IsPanRequiredProtection(pcCardSaleAll3InOut) == TRUE)
	{	
		ReInitAfterEncodingFail();
		
		if(STORE_MODE_HASH == m_byStoreMode)
		{
			HashOnlySubAlgorithm(pcCardSaleAll3InOut, pszAccountToEncode, lTrsNumber);
		}
		else if(ACTUAL_MODE_FAIL != m_byActualMode)
		{			
			eEncodeRetCode = ActualModeNotFailSubAlgorithm(pcCardSaleAll3InOut, pszAccountToEncode, lTrsNumber);
		}
		
		if(ACTUAL_MODE_FAIL == m_byActualMode && STORE_MODE_HASH != m_byStoreMode) 
		{

			HashOnlySubAlgorithm(pcCardSaleAll3InOut, pszAccountToEncode, lTrsNumber);	//IF ENCRYPT FAILED DO HASH HERE!

			IncTrsNumberToProcessCount();
		}
	}
	else if(IsPanRequiredProtection(pcCardSaleAll3InOut) == FALSE)
	{
		HashOnlySubAlgorithm(pcCardSaleAll3InOut, pszAccountToEncode, lTrsNumber);
	}

	// Set EncryptedAccount2 with MaskedAccount or sFormattedAccount - to be sent later to VP/Extranse Receipt.
	SetNoneOPTReceiptAccountMask(pcCardSaleAll3InOut, pszAccountToEncode);		
	
	SaveAlgorithmResult(pcCardSaleAll3InOut, m_byActualMode);

	return eEncodeRetCode;
}


/******************************************************************************
 Description:	Save Result of Asymmetric Algorithm
 Returns:      	void
 Parameters:   	pcCardSaleAll3InOut
				byCurrTrsActualMode
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			29/04/09	 23:58		Start 4.0.22.45 TD-72716
******************************************************************************/
//private:
void CDSSBothAsymmetricAlg::SaveAlgorithmResult(CARD_SALE_ALL3* pcCardSaleAll3InOut, const BYTE byCurrTrsActualMode)const
{
	//Set actual mode
	l2a(byCurrTrsActualMode, &pcCardSaleAll3InOut->extData4.sActualMode, sizeof(pcCardSaleAll3InOut->extData4.sActualMode));

	// Set Store Mode
	l2a(m_byStoreMode, &pcCardSaleAll3InOut->extData4.sStoreMode, sizeof(pcCardSaleAll3InOut->extData4.sStoreMode));

	//Set EncryptionKeyRef
	memcpy(pcCardSaleAll3InOut->extData4.sEncryptionKeyRef, m_sEncryptionKeyRef, sizeof(m_sEncryptionKeyRef));
}


/******************************************************************************
 Description:	Sub Asymmetric Algorithm in case ActualMode Not Fail
 Returns:      	ENCODE_OK
				ENCODE_ENCRYPTION_FAIL

 Parameters:   	pcCardSaleAll3InOut
				pszAccountToEncode
				lTrsNumber
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			29/04/09	 22:51		Start 4.0.22.45 TD-72716
******************************************************************************/
//private:
EncodeRetCodeEnum CDSSBothAsymmetricAlg::ActualModeNotFailSubAlgorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncode, const long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;
	long				lPumpNumber = GetPumpNumber(pcCardSaleAll3InOut);
	char				sEncodedData[ASYMMETRIC_ENCRYPTED_DATA_MAX_SIZE + ENCRYPTED_DATA_SIZE] = {' '};
	size_t				iEncryptedDataSize = 0, iHashedDataSize = 0;
	long				lInvoiceNumber = 0;

	lInvoiceNumber = a2l(pcCardSaleAll3InOut->extData2.sInvoiceNumber, sizeof(pcCardSaleAll3InOut->extData2.sInvoiceNumber)); 			//4.0.22.121	TD 78302

	memset(sEncodedData, ' ', sizeof(sEncodedData));

	//DO BOTH HASH AND ENCRYPT
	eEncodeRetCode = Encode(sEncodedData, pszAccountToEncode, lPumpNumber, &iEncryptedDataSize, &iHashedDataSize, lInvoiceNumber);		//4.0.22.121	TD 78302

	if (eEncodeRetCode == ENCODE_OK)
	{
		if (LOG_DETAIL_FLOW_CONTROL)				
		{
			CString strLogMsg;
			strLogMsg.Format("Encrypt AND Hash succeed,StoreMode = %ld, ActualMode = %ld, EncryptedbuffLen=%ld, HashedbuffLen=%ld",
							m_byStoreMode, m_byActualMode, iEncryptedDataSize, iHashedDataSize);
			WriteEncodingLogMsg(lPumpNumber, lTrsNumber, strLogMsg);
		}

		SaveEncodedData(pcCardSaleAll3InOut, sEncodedData, iEncryptedDataSize, sEncodedData + iEncryptedDataSize, iHashedDataSize, lTrsNumber);
	}
	else if(eEncodeRetCode == ENCODE_ENCRYPTION_FAIL)	//IF ENCRYPTION FAILED NO HASH!
	{
		m_byActualMode = ACTUAL_MODE_FAIL;

		if (LOG_DETAIL_FLOW_CONTROL)				
		{
			CString strLogMsg;
			strLogMsg.Format("Encrypt failed, StoreMode = %ld, ActualMode = %ld",
							m_byStoreMode, m_byActualMode);
			WriteEncodingLogMsg(lPumpNumber, lTrsNumber, strLogMsg);
		}
	}
	else if(eEncodeRetCode == ENCODE_HASH_FAIL)
	{
		const size_t iMaxTrsNumLen = 5;			//MAX_TRANSACT_NUMBER	

		SaveEncryptedData(pcCardSaleAll3InOut, sEncodedData, min(iEncryptedDataSize, ASYMMETRIC_ENCRYPTED_DATA_MAX_SIZE), lTrsNumber);
		SaveHashedData(pcCardSaleAll3InOut, lTrsNumber, iMaxTrsNumLen);

		if (LOG_DETAIL_FLOW_CONTROL)				
		{
			CString strLogMsg;
			strLogMsg.Format("Encrypt succeed AND Hash failed, StoreMode = %ld, ActualMode = %ld, ReceiptIndex=[%.100s]",
							m_byStoreMode, m_byActualMode, pcCardSaleAll3InOut->extData4.sEncryptedAccount);
			WriteEncodingLogMsg(lPumpNumber, lTrsNumber, strLogMsg);
		}

		eEncodeRetCode = ENCODE_OK;
	}

	return eEncodeRetCode;
}


/******************************************************************************
 Description:	Sub Asymmetric Algorithm in case HashOnly
 Returns:      	ENCODE_OK
				ENCODE_ENCRYPTION_FAIL

 Parameters:   	pcCardSaleAll3InOut
				pszAccountToEncode
				lTrsNumber
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			29/04/09	 22:51		Start 4.0.22.45 TD-72716
******************************************************************************/
//private:
EncodeRetCodeEnum CDSSBothAsymmetricAlg::HashOnlySubAlgorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncode, const long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;
	long				lPumpNumber = GetPumpNumber(pcCardSaleAll3InOut);
	char				sEncodedData[ENCRYPTED_DATA_SIZE] = {' '};
	size_t				iHashedDataSize = 0;
	long				lInvoiceNumber = 0;

	lInvoiceNumber = a2l(pcCardSaleAll3InOut->extData2.sInvoiceNumber, sizeof(pcCardSaleAll3InOut->extData2.sInvoiceNumber)); 		//4.0.22.121	TD 78302

	eEncodeRetCode = Encode(sEncodedData, pszAccountToEncode, lPumpNumber, NULL, &iHashedDataSize, lInvoiceNumber);					//4.0.22.121	TD 78302
	
	if (eEncodeRetCode == ENCODE_OK)
	{
		SaveHashedData(pcCardSaleAll3InOut, sEncodedData, iHashedDataSize);

		if (LOG_DETAIL_FLOW_CONTROL)				
		{
			CString strLogMsg;
			strLogMsg.Format("Hash succeed, StoreMode=%ld, ActualMode=%ld, HashedDataSize=%ld, ReceiptIndex=[%.100s]", 
				m_byStoreMode, m_byActualMode, iHashedDataSize, pcCardSaleAll3InOut->extData4.sEncryptedAccount);
			
			WriteEncodingLogMsg(lPumpNumber, lTrsNumber, strLogMsg);
		}
	}

	//In any case, when HASH fails - we save the transaction number as the receipt key. Done for Extrnse to take the Receipt later. 
	if(eEncodeRetCode == ENCODE_HASH_FAIL)
	{
		const size_t iMaxTrsNumLen = 5;			//MAX_TRANSACT_NUMBER	
		
		SaveHashedData(pcCardSaleAll3InOut, lTrsNumber, iMaxTrsNumLen);

		if (LOG_DETAIL_FLOW_CONTROL)				
		{
			CString strLogMsg;
			strLogMsg.Format("Hash failed, StoreMode=%ld, ActualMode=%ld, ReceiptIndex=[%.100s]", 
				m_byStoreMode, m_byActualMode, pcCardSaleAll3InOut->extData4.sEncryptedAccount);
			
			WriteEncodingLogMsg(lPumpNumber, lTrsNumber, strLogMsg);
		}

		eEncodeRetCode = ENCODE_OK;
	}

	return eEncodeRetCode;
}


/******************************************************************************
 Description:	ReInit After Encoding Fail
 Returns:      	void
 Parameters:   	none
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			29/04/09	 22:22		Start 4.0.22.45 TD-72716
******************************************************************************/
//private:
void CDSSBothAsymmetricAlg::ReInitAfterEncodingFail()
{
	//We allow a number of transactions (set by parameter to be with exposed data before calling Re init to DSS.
	if(0 != m_byMaxTrsNumberToProcessOnDSSFail)
	{
		if(m_byTrsNumberToProcessCount >= m_byMaxTrsNumberToProcessOnDSSFail)
		{
			ReInit();
			SendReInitAlert();

			m_byTrsNumberToProcessCount = 0;

			if (LOG_BASIC_FLOW_CONTROL)				
			{
				CString strLogMsg;
				strLogMsg.Format("ReInit and Reset TrsNumberToProcessCount, StoreMode = %ld, ActualMode = %ld", m_byStoreMode, m_byActualMode);
				WriteEncodingLogMsg(strLogMsg);
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Control Methods
//////////////////////////////////////////////////////////////////////
inline BOOL	CDSSBothAsymmetricAlg::IsAlgorithmEnabled(const short nSessionType) const 
{ 
	BOOL bRetCode = ((nSessionType & SESSION_COMPLETE) ? TRUE : FALSE);

	return bRetCode;
}


inline BOOL	CDSSBothAsymmetricAlg::IsValidEncodedDataSize(size_t iSize) const
{ 
	BOOL bRetCode = (iSize > ASYMMETRIC_ENCRYPTED_DATA_MAX_SIZE || iSize < 1) ? FALSE : TRUE; 

	return bRetCode;
}


//////////////////////////////////////////////////////////////////////
// Changes control methods
//////////////////////////////////////////////////////////////////////
/******************************************************************************
 Description:	
 Returns:      	void
 Parameters:   	strAlertDescriptionOut
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena									Start 4.0.22.45 TD-72716
******************************************************************************/
void CDSSBothAsymmetricAlg::ChangesResponse(CString& strAlertDescriptionOut)
{
	BYTE*			pbyBackUp = NULL;
	const size_t	iParameterNumber = 2;
	const char		szParameterNames[iParameterNumber][MAX_NAME_LEN] = {"DSSTestMode,", 
																		"DisableEncrypt,",
																		};

	//Save last configuration
	BackUpLastConfig(&pbyBackUp);

	//Load configuration from registry
	m_bSendChangedAlert = FALSE;
	LoadConfiguration();
	m_bSendChangedAlert = TRUE;

	//Check if was changes and update Alert Description string
	if(IsDSSTestModeChanged( pbyBackUp[0] ))
	{
		strAlertDescriptionOut += szParameterNames[0];
	}

	if(IsDisableEncryptChanged( pbyBackUp[1] ))
	{
		strAlertDescriptionOut += szParameterNames[1];
	}

	//return the last configuration to members
	RestoreLastConfig(&pbyBackUp);
}


//////////////////////////////////////////////////////////////////////
//					Save Encoding Data Methods
//////////////////////////////////////////////////////////////////////

/******************************************************************************
 Description:	Save Encrypted Data to Ash2000 Qdx
 Returns:      	void
 Parameters:   	pcCardSaleAll3InOut
				psEncryptedData
				iEncryptedDataSize
				lTrsNumber
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena									Start 4.0.22.45 TD-72716
******************************************************************************/
//protected:
void CDSSBothAsymmetricAlg::SaveEncryptedData(CARD_SALE_ALL3* pcCardSaleAll3InOut, 
										 const char* psEncryptedData, const size_t iEncryptedDataSize, const long lTrsNumber)
{
	long			lRtc = 0;
	ASH2000_REC		cASH2000_REC;

	//init ASH2000_REC
	memset(&cASH2000_REC, 0, sizeof(ASH2000_REC));
	memset(cASH2000_REC.sData, ' ', sizeof(cASH2000_REC.sData));

	//set ASH2000_REC
	memcpy(cASH2000_REC.sData, psEncryptedData, min(iEncryptedDataSize ,sizeof(cASH2000_REC.sData)) );	
	cASH2000_REC.lIndexNumber = lTrsNumber;

	//set CardSaleAll3 Flags to ASH2000
	size_t	iFlagsSize = sizeof(pcCardSaleAll3InOut->extData2.sFlags);
	long	lFlags = a2l(pcCardSaleAll3InOut->extData2.sFlags, iFlagsSize);

	if (!(lFlags & FL_CONNECTED_RECORD_IN_ASH2000_FILE))
		lFlags |= FL_CONNECTED_RECORD_IN_ASH2000_FILE;

	l2a(lFlags, pcCardSaleAll3InOut->extData2.sFlags, iFlagsSize); 

	//save to ASH2000 file
	CSQLAsh2000	cSQLAsh2000File;
	lRtc = (cSQLAsh2000File.Operation(REC_READ, &cASH2000_REC)) ? cSQLAsh2000File.Operation(REC_INSERT, &cASH2000_REC) : cSQLAsh2000File.Operation(REC_UPDATE, &cASH2000_REC);

	if (LOG_DETAIL_FLOW_CONTROL)				
	{
		long	lPumpNumber = GetPumpNumber(pcCardSaleAll3InOut);
		CString strLogMsg;

		strLogMsg.Format("SaveEncryptedData to ASH2000, lIndexNumber=%ld, lRtc=%ld", cASH2000_REC.lIndexNumber, lRtc);
		WriteEncodingLogMsg(lPumpNumber, lTrsNumber, strLogMsg);
	}
}


//////////////////////////////////////////////////////////////////////
//			Receipt Index Methods		
//////////////////////////////////////////////////////////////////////
long CDSSBothAsymmetricAlg::GetReceiptIndex(char* psIndexOut, const char* pszInfoIn)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;
	size_t				iHashedDataSize = 0;

	eEncodeRetCode = Encode(psIndexOut, pszInfoIn, 0, NULL, &iHashedDataSize);
	
	//Encode response
	if (eEncodeRetCode == ENCODE_HASH_FAIL)
	{	//TD 75761 
		WriteEncodingLogMsg("GetReceiptIndex, Hash Failed, No Receipt index!");
	}
	else
		WriteEncodingLogMsg("GetReceiptIndex, Receipt index is HashedAccount!");

	return eEncodeRetCode;
}


/******************************************************************************
 Description:	NoneOPTReceipt Account Masking Is Required
 Returns:      	TRUE\FALSE
 Parameters:   	pInfoIn	- PAY_AT_PUMP_INFO struct
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			17/05/2011	 09:41		Start .4.0.27.501 TD 233308
******************************************************************************/
BOOL CDSSBothAsymmetricAlg::IsNoneOPTReceiptAccountMaskRequired(const PAY_AT_PUMP_INFO* pcPayAtPumpInfo)const
{
	return ((IsFieldNotEmpty((char*)&pcPayAtPumpInfo->CardSaleExtraData4.sEncryptedAccount2, sizeof(pcPayAtPumpInfo->CardSaleExtraData4.sEncryptedAccount2)) == TRUE) ? FALSE : TRUE);
}


/******************************************************************************
 Description:	Mask Account number for None OPT Receipt (EJ) 
 Returns:      	void
 Parameters:   	pcPayAtPumpInfo
				pszAccountToEncode

 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			17/05/2011	 11:41		Start 4.0.27.501 TD 233308
******************************************************************************/
void CDSSBothAsymmetricAlg::SetNoneOPTReceiptAccountMask(PAY_AT_PUMP_INFO* pcPayAtPumpInfo, const char* pszAccountNum)const
{
	if (IsNoneOPTReceiptAccountMaskRequired(pcPayAtPumpInfo) == TRUE)
	{
		CARD_SALE_ALL3 cCardSaleAll3;
		memset(&cCardSaleAll3, ' ', sizeof(cCardSaleAll3));

		_Module.m_server.m_cCommonFunction.ConvertPayAtPumpInfoToCardSaleAll3(cCardSaleAll3, *pcPayAtPumpInfo);

		SetNoneOPTReceiptAccountMask(&cCardSaleAll3, pszAccountNum);	

		_Module.m_server.m_cCommonFunction.ConvertCardSaleAll3ToPayAtPumpInfo(*pcPayAtPumpInfo, cCardSaleAll3);
	}
}



//////////////////////////////////////////////////////////////////////
//					Mask Account Methods
//////////////////////////////////////////////////////////////////////
//protected:
void  CDSSBothAsymmetricAlg::GetMaskLengths(size_t* piHeadFreeLen, size_t* piTailFreeLen, const size_t iOpenStrLen, const MaskTypeEnum eMaskType)const
{
	switch(eMaskType)
	{
	case  MASK_TYPE_LOG:
		{	//ANMS_HEAD0_TAIL4
			*piHeadFreeLen = 0;
			*piTailFreeLen = PCI_COMPLIENT_MASK_NUM_END;
		}break;
	case  MASK_TYPE_DB:
		{	//ANMS_16_DIGIT_RULE
			*piHeadFreeLen = (iOpenStrLen >= 16) ? PCI_COMPLIENT_MASK_NUM_START : 0;
			*piTailFreeLen = PCI_COMPLIENT_MASK_NUM_END;
		}
	}
}


//4.0.22.45 TD 72716
//protected:
inline BOOL CDSSBothAsymmetricAlg::IsMaskingAccountRequired(const CARD_SALE_ALL3* pcCardSaleAll3InOut, const MaskTypeEnum eMaskType)const
{
	BOOL bRetCode = ( (eMaskType == MASK_TYPE_LOG || ACTUAL_MODE_MASK_AND_ENCRYPT_AND_HASH == m_byActualMode) 
						&& IsPanRequiredProtection(pcCardSaleAll3InOut) == TRUE ) ? TRUE : FALSE;	
	
	return bRetCode;
}


/******************************************************************************
  Description:	Clear card info from given string 	
  Returns:      succeeded
  Parameters:  	char *sIn - string source
				long lInSize - string source size , 
				char *sOut   - string destination 
  
  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
 Gena			24/06/2009	11:34		//4.0.22.74 TD 76537
 ******************************************************************************/
BOOL CDSSBothAsymmetricAlg::RemoveCardInfoFromMessage(char *sIn, long lInSize, char *sOut)const
{
	BOOL	bRetCode = TRUE;
	long	lSize=lInSize;

	if(lInSize<200)
		lSize = 5000;

	CBasicProtectData::RemoveCardInfoFromMessage(sIn, lInSize, sOut);

	try
	{
		int iSizeOfBuffer = lSize*2 +1;
		char *szTmpIn = new char[iSizeOfBuffer];

		memset(szTmpIn , 0 ,iSizeOfBuffer);
		strcpy(szTmpIn, sOut);

		long lFlag = ENCRYPT_DATA;

		if (lInSize)
		{
			MaskOLAString("Zone[S]=", szTmpIn,iSizeOfBuffer, lFlag); //4.0.22.71
			MaskOLAString("FormattedAcct[S]=", szTmpIn,iSizeOfBuffer, lFlag); //4.0.22.74
			
			memcpy(sOut, szTmpIn, lInSize);
		}
		
		if (szTmpIn)
			delete []szTmpIn;
	}
	catch(...)
	{
		CString sMsg;
		sMsg.Format("RemoveCardInfoFromMessage, GenerateIndex Had UnExcpcted Error", LOG_LEVEL_1);
		WriteEncodingLogMsg(sMsg, LOG_LEVEL_0);

		bRetCode = FALSE;
	}

	return bRetCode;
}


//////////////////////////////////////////////////////////////////////
//							Send alerts methods
//////////////////////////////////////////////////////////////////////
//protected:
void CDSSBothAsymmetricAlg::SendInitializationFailedAlert(const long lEncodeDLLErrorCode, const char* szErrorMessage) const
{
	long lAlarmValue = 0;
	char szAlarmDescription[MAX_ALARM_DESC_LEN] = {0};

	lAlarmValue = (STORE_MODE_HASH == m_byStoreMode && lEncodeDLLErrorCode <= -10) ? -10 : lEncodeDLLErrorCode;
	memcpy(szAlarmDescription, szErrorMessage, min(sizeof(szAlarmDescription), strlen(szErrorMessage)));

	if (LOG_BASIC_FLOW_CONTROL)				
	{
		CString strLogMsg;
		strLogMsg.Format("SendInitializationFailedAlert, AlarmValue=%ld, AlarmDescription=[%s]", lAlarmValue, szAlarmDescription);
		WriteEncodingLogMsg(strLogMsg);
	}

	_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, DSS_INIT_FAIL, 0, ALARM_STARTED, lAlarmValue, 0, szAlarmDescription);	
}


/******************************************************************************
  Description:	Convert transaction number (max 99999) to device number (max 999)
				device number will contain last three digits of transaction number
  Returns:      device number (last three digits of transaction number)
  Parameters:  	long lTrsNumber - transaction number 
  
  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
 Gena			10/08/2009	16:09		//4.0.22.120 TD 78096
 AmitH			20/07/2010  22:00       //4.0.25.50  TD 136037 - use 2 last digits of trs number instead of three
																 so we use 99 instead of 999
 ******************************************************************************/
//private
long CDSSBothAsymmetricAlg::ConvertTrsNumber2DeviceNumber(const long lTrsNumber) const
{
//	const long	lDeviceNumberDivisor = MAX_DEVICE_NUMBER + 1;
	const long	lDeviceNumberDivisor = MAX_SUBCATEGORY_NUMBER + 1; //99 + 1
	long		lRetDeviceNumber = (lTrsNumber/lDeviceNumberDivisor > 0) ? (lTrsNumber%lDeviceNumberDivisor) : lTrsNumber;

	return lRetDeviceNumber;
}

//	lTrsNumber is InvoiceNumber
//protected:
void CDSSBothAsymmetricAlg::SendEncryptionFailedAlert(const long lEncodeDLLErrorCode, const char* szErrorMessage, long lInvoiceNumber) const
{
	long lAlarmValue = 0;
	long lDeviceNumber = 0;
	char szAlarmDescription[MAX_ALARM_DESC_LEN] = {0};

	//4.0.22.510 TD 75693 - If there is no Transaction number - do not send the alarm
	if (lInvoiceNumber != 0)
	{		
		lAlarmValue = lEncodeDLLErrorCode;
		lDeviceNumber = ConvertTrsNumber2DeviceNumber(lInvoiceNumber);							//4.0.22.120 TD 78096

		sprintf(szAlarmDescription, "%s | InvoiceNumber=%ld", szErrorMessage, lInvoiceNumber);				//4.0.22.121	TD 78302
		//memcpy(szAlarmDescription, szErrorMessage, min(sizeof(szAlarmDescription), strlen(szErrorMessage)));

		if (LOG_BASIC_FLOW_CONTROL)				
		{
			CString strLogMsg;
			strLogMsg.Format("SendEncryptionFailedAlert, InvoiceNumber=%ld, AlarmValue=%ld AlarmDescription=[%s]", lInvoiceNumber, lAlarmValue, szAlarmDescription);
			WriteEncodingLogMsg(strLogMsg);
		}

		_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, DSS_ENCRYPTION_FAIL, lDeviceNumber, ALARM_STARTED, lAlarmValue, 0, szAlarmDescription);	
	}
}


//protected:
void CDSSBothAsymmetricAlg::SendHashingFailedAlert(const long lEncodeDLLErrorCode, const char* szErrorMessage, long lInvoiceNumber) const
{
	long lAlarmValue = 0;
	long lDeviceNumber = 0;
	char szAlarmDescription[MAX_ALARM_DESC_LEN] = {0};

	//4.0.22.510 TD 75693 - If there is no Transaction number - do not send the alarm
	if (lInvoiceNumber != 0)
	{		
		lAlarmValue = lEncodeDLLErrorCode;
		lDeviceNumber = ConvertTrsNumber2DeviceNumber(lInvoiceNumber);							//4.0.22.120 TD 78096
		
		sprintf(szAlarmDescription, "%s | InvoiceNumber=%ld", szErrorMessage, lInvoiceNumber);				//4.0.22.121	TD 78302
		//memcpy(szAlarmDescription, szErrorMessage, min(sizeof(szAlarmDescription), strlen(szErrorMessage)));

		if (LOG_BASIC_FLOW_CONTROL)				
		{
			CString strLogMsg;
			strLogMsg.Format("SendHashingFailedAlert, InvoiceNumber=%ld, AlarmValue=%ld AlarmDescription=[%s]", lInvoiceNumber, lAlarmValue, szAlarmDescription);
			WriteEncodingLogMsg(strLogMsg);
		}

		_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, DSS_HASHING_FAIL, lDeviceNumber, ALARM_STARTED, lAlarmValue, 0, szAlarmDescription);	//TD 75922 4.0.22.65
	}
}

//protected:
void CDSSBothAsymmetricAlg::SendStoreModeChangedAlert(const BYTE byStoreMode) const
{
	long lAlarmValue	= (long)m_byStoreMode;
	long lDeviceNumber	= (long)byStoreMode;

	if(LOG_BASIC_FLOW_CONTROL)
	{		
		CString strLogMsg;
		strLogMsg.Format("SendStoreModeChangedAlert Old StoreMode=%ld New StoreMode=%ld", m_byStoreMode, byStoreMode);
		WriteEncodingLogMsg(strLogMsg);
	}

	_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, DSS_STORE_MODE_CHANGED, lDeviceNumber, ALARM_STARTED, lAlarmValue, 0);	
}


//protected:
void CDSSBothAsymmetricAlg::SendReInitAlert() const
{
	LONG lAlarmValue = m_byMaxTrsNumberToProcessOnDSSFail;

	_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, DSS_REINIT_CALLED, 0, ALARM_STARTED, lAlarmValue, 0);	
}


//4.0.22.74
void CDSSBothAsymmetricAlg::StopInitializationFailedAlert()const
{
	if (LOG_BASIC_FLOW_CONTROL)				
	{
		WriteEncodingLogMsg("Send Stop InitializationFailedAlert");
	}
	
//	_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, DSS_INIT_FAIL, MAX_DEVICE_NUMBER, ALARM_ENDED, 0, 0);	//TD 76536 
	_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, DSS_INIT_FAIL, 0, ALARM_ENDED, 0, 0);	//4.0.25.50 TD 136037 devie number = zero
}


/******************************************************************************
 Description:	Mask Account number for None OPT Receipt (EJ) 
 Returns:      	void
 Parameters:   	pcCardSaleAll3InOut
				pszAccountToEncode
				 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena									Start 4.0.22.45 TD-72716
******************************************************************************/
void CDSSBothAsymmetricAlg::SetNoneOPTReceiptAccountMask(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncode)const
{	
	CString strLogMsg;
	char	szLogMsgIdentifyire[MAX_NAME_LENGTH] = {0};
	char	sMaskedAccount[ACCOUNT_NUM_SIZE];
	
	memset(sMaskedAccount, ' ', sizeof(sMaskedAccount));
	strLogMsg.Empty();

	if (IsPanRequiredProtection(pcCardSaleAll3InOut) == TRUE)
	{
		MaskAccount(sMaskedAccount, pszAccountToEncode, MASK_TYPE_DB);

		strcpy(szLogMsgIdentifyire, "MaskedAccount");
	}
	else if (IsPanRequiredProtection(pcCardSaleAll3InOut) == FALSE)
	{
		memcpy(sMaskedAccount, pcCardSaleAll3InOut->extData2.sFormattedAccount, min(sizeof(sMaskedAccount), sizeof(pcCardSaleAll3InOut->extData2.sFormattedAccount)) );
		strcpy(szLogMsgIdentifyire, "FormattedAccount");
	}

	memcpy(pcCardSaleAll3InOut->extData4.sEncryptedAccount2, sMaskedAccount, min(sizeof(pcCardSaleAll3InOut->extData4.sEncryptedAccount2), sizeof(sMaskedAccount)) );

	if(0 != szLogMsgIdentifyire[0])
	{
		strLogMsg.Format("EncryptedAccount2 is [%s], PanRequiredProtection = %c, StoreMode = %ld, ActualMode = %ld", 
						szLogMsgIdentifyire, pcCardSaleAll3InOut->extData4.sPanRequiredProtection, m_byStoreMode, m_byActualMode);

		WriteEncodingLogMsg( GetPumpNumber(pcCardSaleAll3InOut), strLogMsg );
	}
}


//////////////////////////////////////////////////////////////////////
//							Control methods
//////////////////////////////////////////////////////////////////////
inline BOOL	CDSSBothAsymmetricAlg::IsStoreModeChanged(const BYTE byStoreMode)const
{
	BOOL bRetCode = (m_byStoreMode != byStoreMode && m_byStoreMode != INIT_MODE) ? TRUE : FALSE;

	return bRetCode;
}


inline BOOL	CDSSBothAsymmetricAlg::IsPanRequiredProtection(const CARD_SALE_ALL3* pcCardSaleAll3InOut)const
{
	CARD_SALE_ALL3	cCardSaleAll3;

	memcpy(&cCardSaleAll3, pcCardSaleAll3InOut, sizeof(cCardSaleAll3));

	BOOL bRetCode = (	cCardSaleAll3.extData4.sPanRequiredProtection == 'Y' ||
						cCardSaleAll3.extData4.sPanRequiredProtection == 'y') ? TRUE : FALSE;

	return bRetCode;
}


void  CDSSBothAsymmetricAlg::SetStoreMode(const BYTE byStoreMode)
{
	if (m_bSendChangedAlert == TRUE && IsStoreModeChanged(byStoreMode) == TRUE)
	{
		SendStoreModeChangedAlert(byStoreMode);
	}

	m_byStoreMode = byStoreMode;
}


void CDSSBothAsymmetricAlg::UpdateStoreMode()
{
	if (TRUE == m_bDisableEncrypt)
		SetStoreMode(STORE_MODE_HASH);
	else
		SetStoreMode((TRUE == m_bDSSTestMode) ? STORE_MODE_ENCRYPT_AND_HASH : STORE_MODE_MASK_AND_ENCRYPT_AND_HASH);
}


void CDSSBothAsymmetricAlg::UpdateActualMode(const BOOL bActionStatus)
{
	m_byActualMode = (FALSE == bActionStatus) ? ACTUAL_MODE_FAIL : m_byStoreMode ;
}


/******************************************************************************
 Description:	DSSTestMode was Changed?
 Returns:      	TRUE - changed
				FALSE - not changed

 Parameters:   	bDSSTestMode - DSSTestMode before change
				 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//28/04/09 12:15		Start 4.0.22.45 TD-72716
******************************************************************************/
//private:
inline BOOL CDSSBothAsymmetricAlg::IsDSSTestModeChanged(const BOOL bDSSTestMode)const
{
	BOOL bRetCode = (m_bDSSTestMode != bDSSTestMode) ? TRUE : FALSE;

	return bRetCode;
}


/******************************************************************************
 Description:	DisableEncrypt was Changed?
 Returns:      	TRUE - changed
				FALSE - not changed

 Parameters:   	bDisableEncrypt - DisableEncrypt before change
				 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//28/04/09 12:15		Start 4.0.22.45 TD-72716
******************************************************************************/
//private:
inline BOOL CDSSBothAsymmetricAlg::IsDisableEncryptChanged(const BOOL bDisableEncrypt)const
{
	BOOL bRetCode = (m_bDisableEncrypt != bDisableEncrypt) ? TRUE : FALSE;

	return bRetCode;
}


/******************************************************************************
 Description:	Serialization the configuration members onto a array
 Returns:      	void
 Parameters:   	pbyBackUpOut - target array
				 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//28/04/09 12:56		Start 4.0.22.45 TD-72716
******************************************************************************/
//private:
void CDSSBothAsymmetricAlg::BackUpLastConfig(BYTE**	pbyBackUpOut)const
{
	if (NULL == *pbyBackUpOut)
	{
		const size_t iNumOfAributes	= 5;
		const size_t iArrSize		= iNumOfAributes*sizeof(BYTE);

		*pbyBackUpOut = new BYTE[iArrSize];
		memset(*pbyBackUpOut, 0, iArrSize);

		int i = 0;

		// Cfg
		(*pbyBackUpOut)[i++]	= (BYTE)m_bDSSTestMode;
		(*pbyBackUpOut)[i++]	= (BYTE)m_bDisableEncrypt;
		(*pbyBackUpOut)[i++]	= m_byMaxTrsNumberToProcessOnDSSFail;

		// Ctrl
		(*pbyBackUpOut)[i++] = m_byStoreMode;
		(*pbyBackUpOut)[i++] = m_byActualMode;
	}
}


/******************************************************************************
 Description:	Deserialization the configuration members from a array
 Returns:      	void
 Parameters:   	pbyBackUpOut - source array
				 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//28/04/09 12:57		Start 4.0.22.45 TD-72716
******************************************************************************/
//private:
void CDSSBothAsymmetricAlg::RestoreLastConfig(BYTE** pbyBackUpIn)
{
	if (NULL != *pbyBackUpIn)
	{
		int i = 0;

		// Cfg
		m_bDSSTestMode						= (BOOL)(*pbyBackUpIn)[i++];
		m_bDisableEncrypt					= (BOOL)(*pbyBackUpIn)[i++];
		m_byMaxTrsNumberToProcessOnDSSFail	= (*pbyBackUpIn)[i++];

		// Ctrl
		m_byStoreMode						= (*pbyBackUpIn)[i++];
		m_byActualMode						= (*pbyBackUpIn)[i++];

		delete [] *pbyBackUpIn;
		*pbyBackUpIn = NULL;
	}
}


//4.0.22.74 TD 76545 
void  CDSSBothAsymmetricAlg::CleanUp()
{
	CBaseEncodingAlg::CleanUp();
}