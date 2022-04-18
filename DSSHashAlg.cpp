// DSSHashAlg.cpp: implementation of the CDSSHashAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSSHashAlg.h"


//--------------------------------------------------------------------
//					CDSSHashAlg class			  //4.0.22.40 TD 72716
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDSSHashAlg::CDSSHashAlg() : CBaseEncodingAlg("DSSHashAlgorithm, ", CLSID_Cryptography)
{}


CDSSHashAlg::CDSSHashAlg(const CString& strLogMsgPrefix)
{

}

CDSSHashAlg::~CDSSHashAlg(){}


//////////////////////////////////////////////////////////////////////
//					Main
//////////////////////////////////////////////////////////////////////

EncodeRetCodeEnum CDSSHashAlg::Algorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToHash, const long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;
	char				sHashedAccount[ENCRYPTED_DATA_SIZE];
	size_t				iHashedDataSize = 0;
	long				lPumpNumber = GetPumpNumber(pcCardSaleAll3InOut);

	memset(sHashedAccount, ' ', sizeof(sHashedAccount));
	
	eEncodeRetCode = Encode(sHashedAccount, pszAccountToHash, lPumpNumber, NULL, &iHashedDataSize);

	//Encode response
	if (eEncodeRetCode == ENCODE_OK)
	{
		SaveEncodedData(pcCardSaleAll3InOut, NULL, 0, sHashedAccount, min(sizeof(sHashedAccount), iHashedDataSize));
	}
	else
	{
		if(eEncodeRetCode == ENCODE_HASH_FAIL)
		{
			if(FALSE == m_bDeclinePAPOnDSSFail)
			{	
				char	sMaskedAccount[ACCOUNT_NUM_SIZE];
				memset(sMaskedAccount, ' ', sizeof(sMaskedAccount));

				MaskAccount(sMaskedAccount, pszAccountToHash);

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
//					DLL Entry Point to Hash Method
//////////////////////////////////////////////////////////////////////
//protected:
EncodeRetCodeEnum CDSSHashAlg::EncodeDLLEntryPoint(const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piEncryptedDataSize, size_t* piHashedDataSize, long lTrsNumber)
{
	long				lRetVal = ENCODE_DLL_OK;
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_FAIL;

	CComBSTR	cwsEncodedData(sizeof("  "), LPCSTR("  "));
	CComBSTR	cwsErrorMessage(sizeof("  "), LPCSTR("  "));
	_bstr_t		bstrDataToHash(pszDataToEncode);
	_bstr_t		bstrCertificate(m_szCertificate);

	lRetVal = CEncodingComDll<ICryptography>::PtrInstance()->CreateHash(bstrDataToHash, bstrCertificate, &cwsEncodedData.m_str, plErrorCode, &cwsErrorMessage.m_str);

	if(lRetVal == ENCODE_DLL_OK && *plErrorCode == ENCODE_OK)
	{
		long lSize = cwsEncodedData.Length();
		if (IsValidEncodedDataSize(lSize))
		{
			*piHashedDataSize = lSize;
			int iLen = Bstr2Buffer(cwsEncodedData.m_str, psEncodedData, lSize);

			eEncodeRetCode = ENCODE_OK; 

			CString strLogMsg;
			strLogMsg.Format("Hashing succeed, buffLen=%ld", lSize);
			WriteEncodingLogMsg(0, lTrsNumber, strLogMsg);
		}
		else
			eEncodeRetCode = ENCODE_STRING_ILLEGAL_SIZE; 
	}

	if(eEncodeRetCode != ENCODE_OK)
	{
		eEncodeRetCode = ENCODE_HASH_FAIL;

		Bstr2Buffer(cwsErrorMessage.m_str, szErrorMessage, cwsErrorMessage.Length());

		SendHashingFailedAlert(*plErrorCode, szErrorMessage, lTrsNumber);
	}

	return eEncodeRetCode;
}


//////////////////////////////////////////////////////////////////////
//				Control methods
//////////////////////////////////////////////////////////////////////

inline BOOL	CDSSHashAlg::IsPAPEnabled() const 
{ 
	BOOL bRetCode = ((m_dwCurrentInitState & FESS_PAP_DISABLED) ? FALSE : TRUE);

	return bRetCode;
}


inline BOOL	CDSSHashAlg::IsAlgorithmEnabled(const short nSessionType) const 
{
	BOOL bRetCode = ((nSessionType & SESSION_AUTHORIZE) ? TRUE : FALSE);

	return bRetCode;
}


//////////////////////////////////////////////////////////////////////
//			Save Encoded Data to CARD_SALE_ALL3 Methods
//////////////////////////////////////////////////////////////////////
//protected:
void CDSSHashAlg::SaveEncodedData(CARD_SALE_ALL3* pcCardSaleAll3Out, 
									   const char* psEncryptedData, const size_t iEncryptedDataSize, 
									   const char* psHashedData, const size_t iHashedDataSize, const long lTrsNumber)
{
	if(NULL != psHashedData)
 	{
		SaveEncryptedData(pcCardSaleAll3Out, psHashedData, iHashedDataSize);
		SaveHashedData(pcCardSaleAll3Out, psHashedData, iHashedDataSize);
	}
}
