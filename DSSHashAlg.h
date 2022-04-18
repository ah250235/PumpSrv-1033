// DSSHashAlg.h: interface for the CDSSHashAlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSSHASHALG_H__E7EA46E0_CB1A_4C25_AA04_8AFA44A10B01__INCLUDED_)
#define AFX_DSSHASHALG_H__E7EA46E0_CB1A_4C25_AA04_8AFA44A10B01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseEncodingAlg.h"


//--------------------------------------------------------------------
//					CDSSHashAlg class
//--------------------------------------------------------------------

class CDSSHashAlg : virtual public CDSSBaseEncodingAlg
{
protected:
	//protected Constructor
	CDSSHashAlg(const CString& strLogMsgPrefix);		//4.0.27.44 TD 179125

public:

	CDSSHashAlg();
	virtual ~CDSSHashAlg();

	// Main
	virtual EncodeRetCodeEnum	Algorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToHash, const long lTrsNumber = 0);

	// Control Methods
	virtual inline BOOL			IsPAPEnabled() const;
	virtual inline BOOL			IsAlgorithmEnabled(const short nSessionType) const;

protected:

	// DLL Entry Point to Encode Methods
	virtual EncodeRetCodeEnum	EncodeDLLEntryPoint(const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piEncryptedDataSize = NULL, size_t* piHashedDataSize = NULL, long lTrsNumber = 0);

	// Save Encoded Data to CARD_SALE_ALL3 Methods
	virtual void		SaveEncodedData(CARD_SALE_ALL3* pcCardSaleAll3InOut, 
								const char* psEncryptedData, const size_t iEncryptedDataSize, 
								const char* psHashedData = NULL, const size_t iHashedDataSize = 0, 
								const long lTrsNumber = 0);
};



#endif // !defined(AFX_DSSHASHALG_H__E7EA46E0_CB1A_4C25_AA04_8AFA44A10B01__INCLUDED_)
