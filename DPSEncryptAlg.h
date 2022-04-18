// DPSEncryptAlg.h: interface for the CDPSEncryptAlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DPSENCRYPTALG_H__11C3718A_CD9D_487F_9AFB_229CFB5A892D__INCLUDED_)
#define AFX_DPSENCRYPTALG_H__11C3718A_CD9D_487F_9AFB_229CFB5A892D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "BaseEncodingAlg.h"


class CDPSEncryptAlg : public CDPSBaseEncodingAlg  
{
public:
	CDPSEncryptAlg();
	virtual ~CDPSEncryptAlg();

	// ConfigurationLoader overloaded Methods
	virtual void				LoadConfiguration(const BOOL bCreate = FALSE);

	// Main
	virtual EncodeRetCodeEnum	Algorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncrypt, const long lTrsNumber = 0);

	// Decrypt
	virtual EncodeRetCodeEnum	RestoreData(CARD_SALE_ALL3* pcCardSaleAll3InOut, const long lTrsNumber = 0);

	// Control Methods
	virtual inline BOOL			IsPAPEnabled() const;
	virtual inline BOOL			IsAlgorithmEnabled(const short nSessionType) const;
	virtual	BOOL				IsRestoreDataEnabled(CARD_SALE_ALL3* const pcCardSaleAll3In) const; 						//4.0.23.20 TD 76407

	virtual void				RemoveConfidencesDataFromMemory(short bySessionType, PAY_AT_PUMP_INFO* pInfoInOut);

protected:	
	// Decode Main Logic
	virtual EncodeRetCodeEnum	Decode(	char* psDecodedBuffOut, const char* pszEncodedBuffIn, const long lPumpNumber = 0, 
										size_t* piDecodedDataSize = NULL, const long lTrsNumber = 0);

	// DLL Entry Point to Encode Methods
	virtual EncodeRetCodeEnum	EncodeDLLEntryPoint(const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piEncryptedDataSize = NULL, size_t* piHashedDataSize = NULL, long lTrsNumber = 0);

	// DLL Entry Point to Decode Methods
	virtual EncodeRetCodeEnum	DecodeDLLEntryPoint(const char* pszEncodedData, char* psDecodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piDecodedDataSize, long lTrsNumber);

	// Encode
	virtual EncodeRetCodeEnum	EncodePAN(CARD_SALE_ALL3* pcCardSaleAll3InOut);
	virtual EncodeRetCodeEnum	EncodeLoyaltyAN(CARD_SALE_ALL3* pcCardSaleAll3InOut);

	// Decode
	virtual EncodeRetCodeEnum	DecodePAN(CARD_SALE_ALL3* pcCardSaleAll3InOut);
	virtual EncodeRetCodeEnum	DecodeLoyaltyAN(CARD_SALE_ALL3* pcCardSaleAll3InOut);

	// Save encoded data overloaded methods
	virtual void		SaveEncodedData(CARD_SALE_ALL3* pcCardSaleAll3InOut, 
									const char* psEncryptedData, const size_t iEncryptedDataSize, 
									const char* psHashedData = NULL, const size_t iHashedDataSize = 0, 
									const long lTrsNumber = 0){};
	
	virtual void	SaveHashedData(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* psHashedData, const size_t iHashedDataSize);

	// Control methods
	virtual void		PANnCpy(char* szPAN, const size_t iPANSize, CARD_SALE_ALL3* const pcCardSaleAll3In)const;
	virtual void		LoyaltyANnCpy(char* szLoyaltyAN, const size_t iLoyaltyANSize, CARD_SALE_ALL3* const pcCardSaleAll3In)const;
	
	virtual void		PANnCpy(CARD_SALE_ALL3* pcCardSaleAll3In, const size_t iPANSize, const char* sPAN)const;
	virtual void		LoyaltyANnCpy(CARD_SALE_ALL3* pcCardSaleAll3In, const size_t iLoyaltyANSize, const char* sLoyaltyAN)const;

	virtual size_t		EncryptedPANnCpy(char* szEncryptedPAN, const size_t iEncryptedPANSize, CARD_SALE_ALL3* const pcCardSaleAll3In)const;
	virtual size_t		EncryptedLoyaltyANnCpy(char* szEncryptedLoyaltyAN, const size_t iEncryptedLoyaltyANSize, CARD_SALE_ALL3* const pcCardSaleAll3In)const;

	virtual inline BOOL IsPANEncoded(const CARD_SALE_ALL3* pcCardSaleAll3) const;
	virtual inline BOOL IsLoyaltyANEncoded(const CARD_SALE_ALL3* pcCardSaleAll3) const;

	virtual BOOL		IsRestorePANEnabled(CARD_SALE_ALL3* const pcCardSaleAll3In) const;
	virtual BOOL		IsRestoreLoyaltyANEnabled(CARD_SALE_ALL3* const pcCardSaleAll3In) const;

};

class CSADPSEncryptAlg : public CDPSEncryptAlg // TD 329049
{
public :

	CSADPSEncryptAlg(); 
	virtual ~CSADPSEncryptAlg();

	virtual void	LoadConfiguration(const BOOL bCreate = FALSE);
	virtual EncodeComDllRetCodeEnum Init(BOOL bLoadConfig);
	virtual EncodeRetCodeEnum DecodeDLLEntryPoint(const char* pszEncodedData, char* psDecodedData, LONG* plErrorCode, char* szErrorMessage,
		size_t* piDecodedDataSize, long lTrsNumber);
	virtual EncodeRetCodeEnum EncodeDLLEntryPoint(const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
		size_t* piEncryptedDataSize, size_t* piHashedDataSize, long lTrsNumber);
	virtual EncodeComDllRetCodeEnum	CloseSession();
};

#endif // !defined(AFX_DPSENCRYPTALG_H__11C3718A_CD9D_487F_9AFB_229CFB5A892D__INCLUDED_)
