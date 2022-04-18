// DSSEncryptHandler.h: interface for the CDSSEncryptHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSSENCRYPTHANDLER_H__6A112EF1_8027_40FC_BF36_C01EF3CC9AB8__INCLUDED_)
#define AFX_DSSENCRYPTHANDLER_H__6A112EF1_8027_40FC_BF36_C01EF3CC9AB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


enum DataSecurityServicesEnum{
						DSS,			//0
						DPS,			//1
						GENERIC_DPS,	//2  // TD 329049
						OLA,			//3
};


#include "DSSBothAsymmetricAlg.h"
#include "DPSEncryptAlg.h"
#include "OLAHashAlg.h"

using namespace std;



class CDSSEncodeHandler : public CBaseEncodeHandler  
{
public:
	// Construction/Destruction
	CDSSEncodeHandler(const CBasicProtectData& cBasicProtectData);
	virtual ~CDSSEncodeHandler();

	// Initialization Methods
	EncodeComDllRetCodeEnum	Init();

	// General Encoding/Decoding Methods
	virtual EncodeRetCodeEnum	EncodeForTrs(const short nTrsStatus, CARD_SALE_ALL3* cCardSaleAll3InOut, const long lTrsNumber = 0);	//4.0.20.503
	virtual EncodeRetCodeEnum	EncodeForTrs(const short nTrsStatus, PAY_AT_PUMP_INFO* pcPayAtPumpInfoInOut, const long lPumpNumber = 0); //4.0.23.0 TD 76407

	// Decoding Methods
	virtual EncodeRetCodeEnum	RestoreData(PAY_AT_PUMP_INFO* pcPayAtPumpInfoInOut, const long lTrsNumber = 0);

	virtual inline BOOL			GetDecryptedAccount(char* sEncodeAccount, char* sEncodedPart, char* sDecodedAccount){ return FALSE; }
	
	// Receipt Index Methods		
	virtual long				GetReceiptIndex(char* sIndexOut, const PAY_AT_PUMP_INFO* pcPayAtPumpInfoIn, const long lPumpNumber = 0);
	virtual long				GetReceiptIndex(char* sIndexOut, const BSTR sTrack2In, const long lFlags, const long lPumpNumber = 0);
	virtual void				GenerateIndex(PAY_AT_PUMP_INFO* pInfo, const BOOL bStoreAccountNum = FALSE, const long lTrsNumber = 0);
	virtual void				GenerateIndex(CARD_SALE_ALL3* pcCardSaleAll3InOut, const BOOL bStoreAccountNum = FALSE, const long lTrsNumber = 0);
	virtual BOOL				IsLookForInternalReceiptIndex();
	virtual void				SetNoneOPTReceiptAccountMask(PAY_AT_PUMP_INFO* pcPayAtPumpInfo, const char* pszAccountNum)const;

	// DB Methods			
	virtual void				RemoveConfidencesData(PAY_AT_PUMP_INFO* pInfoInOut);
	virtual void				RemoveConfidencesData(CARD_SALE_ALL3* pInfoInOut);
		
	//4.0.24.60 TD 112773


	// Log overloaded methods
	virtual BOOL				RemoveCardInfoFromMessage(char *psIn, long lInSize, char *psOut)const;

	virtual void				ChangesResponse(CString& strAlertDescriptionOut);

	// Finalization Methods	
	virtual void				CleanUp();							//4.0.22.13	TD 72716

	// Control Methods

	virtual BOOL IsPAPEnabled();
	virtual void RemoveConfidencesDataFromMemory(short bySessionType, PAY_AT_PUMP_INFO* pInfoInOut);

protected:
	CDSSEncodeHandler();	

	virtual BOOL	MaskAccount(char* psMaskedAccountNumberOut, const char* psOpenAccountNumber, const MaskTypeEnum eMaskType = MASK_TYPE_LOG);
	
	// Protected Initialization Methods
	BOOL			LoadEncodingAlgorithm();

	// ConfigurationLoader overloaded Methods
	virtual void					LoadConfiguration(const BOOL bCreate = FALSE);		//4.0.22.13	TD 72716
	virtual BOOL					IsReInitRequired();									//4.0.22.43 TD 72716
	virtual void					ReInit();											//4.0.22.43 TD 72716

private:

	//PRIVATE:
	inline BOOL IsDSSAlgorithmChanged(const long lLastDSSAlgorithm) const { return (lLastDSSAlgorithm != m_lDSSAlgorithm);}

	inline BOOL IsPCIExcludesLoyaltyChanged(const BOOL bLastPCIExcludesLoyalty) const
	{ return CBaseEncodeHandler::IsPCIExcludesLoyaltyChanged(bLastPCIExcludesLoyalty);}

private:
	// Configurations
	long				m_lDSSAlgorithm;								//4.0.22.13	TD 72716
	long				m_lDSSServiceInUse;								//4.0.23.0 TD 76407

	// Pointer to Encoding Algorithm
	unique_ptr<CBaseEncodingAlg> m_pcBaseEncodingAlgorithm;
};



#endif // !defined(AFX_DSSENCRYPTHANDLER_H__6A112EF1_8027_40FC_BF36_C01EF3CC9AB8__INCLUDED_)
