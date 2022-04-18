// DSSEncryptAlg.h: interface for the CDSSEncryptAlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSSENCRYPTALG_H__1DCBC877_87DC_4721_BF77_5DDDE11497BA__INCLUDED_)
#define AFX_DSSENCRYPTALG_H__1DCBC877_87DC_4721_BF77_5DDDE11497BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseEncodingAlg.h"



//--------------------------------------------------------------------
//					CDSSEncryptAlg class
//--------------------------------------------------------------------

class CDSSEncryptAlg : virtual public CDSSBaseEncodingAlg
{
public:
	CDSSEncryptAlg();
	virtual ~CDSSEncryptAlg();	

	// Main
	virtual EncodeRetCodeEnum	Algorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncrypt, const long lTrsNumber = 0);

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

#endif // !defined(AFX_DSSENCRYPTALG_H__1DCBC877_87DC_4721_BF77_5DDDE11497BA__INCLUDED_)
