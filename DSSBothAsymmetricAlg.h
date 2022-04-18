// DSSBothAsymmetricAlg.h: interface for the CDSSBothAsymmetricAlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSSBOTHASYMMETRICALG_H__1D9BF91E_993A_4CF2_8949_91C3787FC16B__INCLUDED_)
#define AFX_DSSBOTHASYMMETRICALG_H__1D9BF91E_993A_4CF2_8949_91C3787FC16B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSSBothAlg.h"


//--------------------------------------------------------------------
//					CDSSBothAsymmetricAlg class
//--------------------------------------------------------------------

#define INIT_MODE							255	

enum StoreModeEnum{
	STORE_MODE_HASH							= 0,
	STORE_MODE_ENCRYPT_AND_HASH				= 1,
	STORE_MODE_MASK_AND_ENCRYPT_AND_HASH	= 3,
};

enum ActualModeEnum{
	ACTUAL_MODE_FAIL						= 0,
	ACTUAL_MODE_ENCRYPT_AND_HASH			= 1,
	ACTUAL_MODE_MASK_AND_ENCRYPT_AND_HASH	= 3,
};

#define ASYMMETRIC_ENCRYPTED_DATA_MAX_SIZE	512


class CDSSBothAsymmetricAlg : public CDSSBothAlg
{
public:

	CDSSBothAsymmetricAlg();
	virtual ~CDSSBothAsymmetricAlg();

	//Main
	virtual EncodeRetCodeEnum	Algorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncode, const long lTrsNumber = 0);

	// Control Methods
	virtual inline BOOL			IsPAPEnabled() const { return TRUE;}
	virtual inline BOOL			IsAlgorithmEnabled(const short nSessionType) const;
	virtual inline BOOL			IsValidEncodedDataSize(size_t iSize) const;
	virtual inline BOOL			IsMaskingAccountRequired(const CARD_SALE_ALL3* pcCardSaleAll3InOut, const MaskTypeEnum eMaskType = MASK_TYPE_LOG)const;

	virtual void				RemoveConfidencesData(CARD_SALE_ALL3* pInfoInOut) {/* EMPTY*/;}  // 4.0.28.52 TD 264447
	virtual void				SetNoneOPTReceiptAccountMask(PAY_AT_PUMP_INFO* pcPayAtPumpInfo, const char* pszAccountNum)const;

	// Access methods
	inline BYTE					GetStoreMode() const { return m_byStoreMode;}

	// Changes control methods
	virtual void				ChangesResponse(CString& strAlertDescriptionOut);

	// Finalization Methods	
	virtual void				CleanUp();				//4.0.22.74 TD 76545

protected:

	// Initialization Methods
	virtual EncodeComDllRetCodeEnum		InitResponse(	const long	lRetCode, 
														const char* szCertificate, 
														const long	lEncodeDLLErrorCode, 
														const char* szErrorMessage); //4.0.22.85
	
	// ConfigurationLoader overloaded Methods
 	virtual void		ReInit();
	virtual void		LoadConfiguration(const BOOL bCreate = FALSE);
	virtual BOOL		IsReInitRequired();										//4.0.22.73 TD 76462

	// Save Encrypted Data to CARD_SALE_ALL3 Methods
	virtual void		SaveEncryptedData(	CARD_SALE_ALL3* pcCardSaleAll3InOut, 
											const char* psEncryptedData, const size_t iEncryptedDataSize, 
											const long lTrsNumber = 0);
	
	void				SaveAlgorithmResult(CARD_SALE_ALL3* pcCardSaleAll3InOut, const BYTE byCurrTrsActualMode)const;

	// Receipt index methods
	virtual long		GetReceiptIndex(char* psIndexOut, const char* pszInfoIn);								//TD 75688
	virtual BOOL		IsNoneOPTReceiptAccountMaskRequired(const PAY_AT_PUMP_INFO* pcPayAtPumpInfo)const;

	// Account masking methods
	virtual BOOL		RemoveCardInfoFromMessage(char *sIn, long lInSize, char *sOut)const;
	virtual void		GetMaskLengths(size_t* piHeadFreeLen, size_t* piTailFreeLen, const size_t iOpenStrLen, const MaskTypeEnum eMaskType = MASK_TYPE_LOG)const;

	// Send alerts methods
	virtual void		SendInitializationFailedAlert(const long lEncodeDLLErrorCode, const char* szErrorMessage) const;
	virtual void		SendEncryptionFailedAlert(const long lEncodeDLLErrorCode, const char* szErrorMessage, long lInvoiceNumber = 0) const;
	virtual void		SendHashingFailedAlert(const long lEncodeDLLErrorCode, const char* szErrorMessage, long lInvoiceNumber = 0) const;
	void				SendStoreModeChangedAlert(const BYTE byStoreMode) const;
	void				SendReInitAlert() const;
	void				StopInitializationFailedAlert()const;			//4.0.22.74

	// Control Methods
	void				SetStoreMode(const BYTE byStoreMode);
	void				UpdateStoreMode();
	void				UpdateActualMode(const BOOL bActionStatus);

	inline BOOL			IsStoreModeChanged(const BYTE byStoreMode)const;
	inline BOOL			IsPanRequiredProtection(const CARD_SALE_ALL3* pcCardSaleAll3InOut)const;

private:

	// Sub algorithms methods
	EncodeRetCodeEnum	ActualModeNotFailSubAlgorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncode, const long lTrsNumber);
	EncodeRetCodeEnum	HashOnlySubAlgorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncode, const long lTrsNumber);	

	void				ReInitAfterEncodingFail();
	void				SetNoneOPTReceiptAccountMask(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncode)const;

	// Serialization\DeSerialization methods
	void				BackUpLastConfig (BYTE**	pbyBackUpOut)const;
	void				RestoreLastConfig(BYTE**	pbyBackUpIn);

	// Changed control methods
	inline BOOL			IsDSSTestModeChanged(const BOOL bTestMode)const;
	inline BOOL			IsDisableEncryptChanged(const BOOL bDisableEncrypt)const;

	inline void			IncTrsNumberToProcessCount();
	inline BOOL			IsEncryptFailedOnInit(const long lEncodeDLLErrorCode)const;
	inline BOOL			IsHashFailedOnInit(const long lEncodeDLLErrorCode)const;

	long				ConvertTrsNumber2DeviceNumber(const long lTrsNumber)const;
private:

	// Configuration members
	BOOL	m_bDSSTestMode;
	BOOL	m_bDisableEncrypt;
	BYTE	m_byMaxTrsNumberToProcessOnDSSFail;

	// Control members
	BYTE	m_byStoreMode;
	BYTE	m_byActualMode;

	BYTE	m_sEncryptionKeyRef[32];
	BOOL	m_bSendChangedAlert;			//Flag for deferent between write\load new value to\from registry. 
	BYTE	m_byTrsNumberToProcessCount;
};



#endif // !defined(AFX_DSSBOTHASYMMETRICALG_H__1D9BF91E_993A_4CF2_8949_91C3787FC16B__INCLUDED_)
