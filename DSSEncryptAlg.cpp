// DSSEncryptAlg.cpp: implementation of the CDSSEncryptAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSSEncryptAlg.h"


//--------------------------------------------------------------------
//					CDSSEncryptAlg class
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
//				Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDSSEncryptAlg::CDSSEncryptAlg() : CBaseEncodingAlg("DSSEncodingAlgorithm, ", CLSID_Cryptography)
{

}


CDSSEncryptAlg::~CDSSEncryptAlg(){}


//////////////////////////////////////////////////////////////////////
//				Main 
//////////////////////////////////////////////////////////////////////

EncodeRetCodeEnum CDSSEncryptAlg::Algorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncrypt, const long lTrsNumber) 
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;
	char				sEncryptedAccount[ENCRYPTED_DATA_SIZE];	
	size_t				iEncryptedDataSize = 0;
	long				lPumpNumber = GetPumpNumber(pcCardSaleAll3InOut);

	memset(sEncryptedAccount, ' ', sizeof(sEncryptedAccount));
	
	eEncodeRetCode = Encode(sEncryptedAccount, pszAccountToEncrypt, lPumpNumber, &iEncryptedDataSize);

	//Encode response
	if (eEncodeRetCode == ENCODE_OK)
	{
		SaveEncodedData(pcCardSaleAll3InOut, sEncryptedAccount, min(sizeof(sEncryptedAccount), iEncryptedDataSize));
	}				
	else
	{
		if(eEncodeRetCode == ENCODE_ENCRYPTION_FAIL)
		{
			if(FALSE == m_bDeclinePAPOnDSSFail)
			{	
				char	sMaskedAccount[ACCOUNT_NUM_SIZE];
				memset(sMaskedAccount, ' ', sizeof(sMaskedAccount));

				MaskAccount(sMaskedAccount, pszAccountToEncrypt, MASK_TYPE_DB);

				SaveEncodedData(pcCardSaleAll3InOut, sMaskedAccount, sizeof(sMaskedAccount));
	
				//mark the failure:
				pcCardSaleAll3InOut->extData4.sDSSFailed = 1;

				eEncodeRetCode = ENCODE_OK;
				
				WriteEncodingLogMsg(lPumpNumber, lTrsNumber, "Set DSSFailed to 1. Receipt index is MaskedAccount!");
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
//					DLL Entry Point to Encrypt Method
//////////////////////////////////////////////////////////////////////
//protected:
EncodeRetCodeEnum CDSSEncryptAlg::EncodeDLLEntryPoint(const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piEncryptedDataSize, size_t* piHashedDataSize, long lTrsNumber)
{
	long				lRetVal = ENCODE_DLL_OK;
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_FAIL;

	CComBSTR	cwsEncodedData(sizeof("  "), LPCSTR("  "));
	CComBSTR	cwsErrorMessage(sizeof("  "), LPCSTR("  "));
	_bstr_t		bstrDataToEncrypt(pszDataToEncode);
	_bstr_t		bstrCertificate(m_szCertificate);

	lRetVal = CEncodingComDll<ICryptography>::PtrInstance()->Encrypt(bstrDataToEncrypt, bstrCertificate, &cwsEncodedData.m_str, plErrorCode, &cwsErrorMessage.m_str);

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


//////////////////////////////////////////////////////////////////////
//					Control Methods
//////////////////////////////////////////////////////////////////////


inline BOOL	CDSSEncryptAlg::IsPAPEnabled() const 
{ 
	 BOOL bRetCode = ((m_dwCurrentInitState & FESS_PAP_DISABLED) ? FALSE : TRUE);

	 return bRetCode;
}


inline BOOL	CDSSEncryptAlg::IsAlgorithmEnabled(const short nSessionType) const 
{
	 BOOL bRetCode = ((nSessionType & SESSION_AUTHORIZE) ? TRUE : FALSE);

	 return bRetCode;
}


//////////////////////////////////////////////////////////////////////
//				Save Encoded Data to CARD_SALE_ALL3 Methods
//////////////////////////////////////////////////////////////////////
//protected:

void CDSSEncryptAlg::SaveEncodedData(CARD_SALE_ALL3* pcCardSaleAll3Out, 
									   const char* psEncryptedData, const size_t iEncryptedDataSize, 
									   const char* psHashedData, const size_t iHashedDataSize, const long lTrsNumber)
{
	if(NULL != psEncryptedData)
	{
		SaveEncryptedData(pcCardSaleAll3Out, psEncryptedData, iEncryptedDataSize);
		SaveHashedData(pcCardSaleAll3Out, psEncryptedData, iEncryptedDataSize);
 	}
}

