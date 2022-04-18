// DSSBothAlg.h: interface for the CDSSBothAlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSSBOTHALG_H__43A9CA04_F118_4B59_B1AE_F5D9EB808D8E__INCLUDED_)
#define AFX_DSSBOTHALG_H__43A9CA04_F118_4B59_B1AE_F5D9EB808D8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSSHashAlg.h"
#include "DSSEncryptAlg.h"

class CDSSBothAlg : public CDSSEncryptAlg, 
					public CDSSHashAlg
{
public:

	CDSSBothAlg();
	virtual ~CDSSBothAlg();

	//Main
	virtual EncodeRetCodeEnum	Algorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* szAccountToEncode, const long lTrsNumber = 0);

	//Receipt Methods
	virtual long				GetReceiptIndex(char* psIndexOut, const char* pszInfoIn);

	// Control Methods
	virtual inline BOOL			IsPAPEnabled() const;
	virtual inline BOOL			IsAlgorithmEnabled(const short nSessionType) const;

protected:

	//DLL Entry Point to Encode Methods
	virtual EncodeRetCodeEnum	EncodeDLLEntryPoint( const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piEncryptedDataSize = NULL, size_t* piHashedDataSize = NULL, long lTrsNumber = 0);

	// Save Encoded Data to CARD_SALE_ALL3 Methods
	virtual void				SaveEncodedData(CARD_SALE_ALL3* pcCardSaleAll3InOut, 
										const char* psEncryptedData, const size_t iEncryptedDataSize, 
										const char* psHashedData = NULL, const size_t iHashedDataSize = 0, 
										const long lTrsNumber = 0);
};


#endif // !defined(AFX_DSSBOTHALG_H__43A9CA04_F118_4B59_B1AE_F5D9EB808D8E__INCLUDED_)
