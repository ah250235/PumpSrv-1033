// DSSBothAlg.cpp: implementation of the CDSSBothAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSSBothAlg.h"


//--------------------------------------------------------------------
//					CDSSBothAlg class			  //4.0.22.40 TD 72716
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDSSBothAlg::CDSSBothAlg() : CBaseEncodingAlg("DSSEncodingAlgorithm, ", CLSID_Cryptography)
{}

CDSSBothAlg::~CDSSBothAlg(){}


//////////////////////////////////////////////////////////////////////
//						Main
//////////////////////////////////////////////////////////////////////
EncodeRetCodeEnum CDSSBothAlg::Algorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncode, const long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;
	char				sEncodedData[ENCRYPTED_DATA_SIZE*2];
	size_t				iEncryptedDataSize = 0, iHashedDataSize = 0;
	long				lPumpNumber = GetPumpNumber(pcCardSaleAll3InOut);

	memset(sEncodedData, ' ', sizeof(sEncodedData));
	
	eEncodeRetCode = Encode(sEncodedData, pszAccountToEncode, lPumpNumber, &iEncryptedDataSize, &iHashedDataSize);

	//Encode response
	if (eEncodeRetCode == ENCODE_OK)
	{
		SaveEncodedData(pcCardSaleAll3InOut, sEncodedData, iEncryptedDataSize, sEncodedData + iEncryptedDataSize, iHashedDataSize);
	}
	else
	{
		char	sMaskedAccount[ACCOUNT_NUM_SIZE];
		memset(sMaskedAccount, ' ', sizeof(sMaskedAccount));

		MaskAccount(sMaskedAccount, pszAccountToEncode);

		if(eEncodeRetCode == ENCODE_HASH_FAIL)
		{
			SaveHashedData(pcCardSaleAll3InOut, sMaskedAccount, sizeof(sMaskedAccount));

			//mark the failure:
			pcCardSaleAll3InOut->extData4.sDSSFailed = 1;

			eEncodeRetCode = ENCODE_OK;

			WriteEncodingLogMsg(lPumpNumber, lTrsNumber, "HASH_FAIL Set DSSFailed to 1. Receipt index is MaskedAccount!");
		}
		else if(eEncodeRetCode == ENCODE_ENCRYPTION_FAIL)
		{
			if(! m_bDeclinePAPOnDSSFail)
			{
				SaveEncodedData(pcCardSaleAll3InOut, sMaskedAccount, sizeof(sMaskedAccount));

				//mark the failure:
				pcCardSaleAll3InOut->extData4.sDSSFailed = 1;

				eEncodeRetCode = ENCODE_OK;
				
				WriteEncodingLogMsg(lPumpNumber, lTrsNumber, "ENCRYPTION_FAIL Set DSSFailed to 1. Receipt index is MaskedAccount!");
			}
			else
			{
				//Do nothing - The method will return bad return code - it will be treated in the caller.
			}
		}
	}

	return eEncodeRetCode;
}


//////////////////////////////////////////////////////////////////////
//			Receipt Index Methods
//////////////////////////////////////////////////////////////////////
long CDSSBothAlg::GetReceiptIndex(char* psIndexOut, const char* pszInfoIn)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;
	size_t				iHashedDataSize = 0;

	eEncodeRetCode = Encode(psIndexOut, pszInfoIn, 0, NULL, &iHashedDataSize);
	
	//Encode response
	if (eEncodeRetCode == ENCODE_HASH_FAIL)
	{
		MaskAccount(psIndexOut, pszInfoIn);

		eEncodeRetCode = ENCODE_OK;

		WriteEncodingLogMsg("GetReceiptIndex, HASH_FAIL, Receipt index is MaskedAccount!");
	}
	else
		WriteEncodingLogMsg("GetReceiptIndex, Receipt index is HashedAccount!");

	return eEncodeRetCode;
}

//////////////////////////////////////////////////////////////////////
//				DLL Entry Point to Hash and Encrypt Methods
//////////////////////////////////////////////////////////////////////
//protected:
EncodeRetCodeEnum CDSSBothAlg::EncodeDLLEntryPoint(const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piEncryptedDataSize, size_t* piHashedDataSize, long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCodeEnum = ENCODE_OK;
	CString				strCompleteEncodedData;
	
	strCompleteEncodedData.Empty();

	if(piEncryptedDataSize != NULL)
	{		
		eEncodeRetCodeEnum = CDSSEncryptAlg::EncodeDLLEntryPoint(	pszDataToEncode, psEncodedData, plErrorCode, szErrorMessage, 
																	piEncryptedDataSize, piHashedDataSize, lTrsNumber);
		
		if (eEncodeRetCodeEnum == ENCODE_OK)
		{
			CString	strLocalEncodedData(psEncodedData, *piEncryptedDataSize);
			strCompleteEncodedData = strLocalEncodedData;
		}
	}
		
	if (eEncodeRetCodeEnum == ENCODE_OK)
	{
		if(piHashedDataSize != NULL)
		{	
			eEncodeRetCodeEnum = CDSSHashAlg::EncodeDLLEntryPoint(	pszDataToEncode, psEncodedData, plErrorCode, szErrorMessage, 
																	piEncryptedDataSize, piHashedDataSize, lTrsNumber);

			if (eEncodeRetCodeEnum == ENCODE_OK)
			{
				CString	strLocalEncodedData(psEncodedData, *piHashedDataSize);
				strCompleteEncodedData += strLocalEncodedData;
			}
		}
	}

	if (strCompleteEncodedData.IsEmpty() != TRUE)
	{
		memcpy(psEncodedData, (LPCTSTR)strCompleteEncodedData, strCompleteEncodedData.GetLength());
	}

	return eEncodeRetCodeEnum;
}


//////////////////////////////////////////////////////////////////////
//					Control methods
//////////////////////////////////////////////////////////////////////
inline BOOL	CDSSBothAlg::IsPAPEnabled() const 
{ 
	BOOL bRetCode = ((m_dwCurrentInitState & FESS_PAP_DISABLED) ? FALSE : TRUE);

	return bRetCode;
}


inline BOOL CDSSBothAlg::IsAlgorithmEnabled(const short nSessionType) const 
{
	BOOL bRetCode = ((nSessionType & SESSION_AUTHORIZE) ? TRUE : FALSE);

	return bRetCode;
}


//////////////////////////////////////////////////////////////////////
//				Save Encoded Data to CARD_SALE_ALL3 Methods
//////////////////////////////////////////////////////////////////////
//protected:
void CDSSBothAlg::SaveEncodedData(CARD_SALE_ALL3* pcCardSaleAll3Out, 
									   const char* psEncryptedData, const size_t iEncryptedDataSize, 
									   const char* psHashedData, const size_t iHashedDataSize, const long lNumber)
{
	if(NULL != psEncryptedData && NULL != psHashedData)
	{
		SaveEncryptedData(pcCardSaleAll3Out, psEncryptedData, iEncryptedDataSize, lNumber);
		SaveHashedData(pcCardSaleAll3Out, psHashedData, iHashedDataSize);
	}
}

