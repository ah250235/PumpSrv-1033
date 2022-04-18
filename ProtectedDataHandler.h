// ProtectedDataHandler.h: interface for the CProtectedDataHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUELENCRYPT_H__4208C588_6728_4D52_BFC5_8B596EA7741A__INCLUDED_)
#define AFX_FUELENCRYPT_H__4208C588_6728_4D52_BFC5_8B596EA7741A__INCLUDED_




#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "PCIEncodeHandler.h"
#include "DSSEncodeHandler.h"


/*********************************************************
	class CProtectedDataHandler *Encoding manager*		 * 
**********************************************************/

class CProtectedDataHandler : public CConfigurationLoader
{
public:
	// Construction/Destruction
	CProtectedDataHandler();
	virtual ~CProtectedDataHandler();

	// Initialization Methods
	EncodeComDllRetCodeEnum	Init();

	// ConfigurationLoader overloaded Methods
	virtual void			LoadConfiguration(const BOOL bCreate = FALSE);	//4.0.22.13	TD 72716

	// DB Methods
	void					RemoveConfidencesData(PAY_AT_PUMP_INFO* pInfoInOut)const;
	void					RemoveConfidencesData(CARD_SALE_ALL3* pInfoInOut)const;
	
	void					RemoveConfidencesDataFromMemory(short bySessionType, PAY_AT_PUMP_INFO* pInfoInOut)const;
						
	// Log Methods		
	void					RemoveDataFromLog(CString & strOutBuff,CString & strNew)const;
	BOOL					LogReplaceBufferUseSize(char *pszOutBuff, const char *psBufferChangeData, const long lBufferChangeDataSize)const;
						
	// OLA Methods		
	BOOL					EncryptOLAString(char *pszOLAString, char *pszInBuff, char *pszOutBuff, long* plOutBuffLen)const;
	BOOL					EncryptOLAFields(CXMLInterface *sData)const; 
	BOOL					EncryptOLAFields(CARD_SALE_ALL3 *sData)const; 
	BOOL					EncryptOLAFields(RECEIPT_DATA4_EX *sData)const; 
	BOOL					EncryptOLAFields(CARD_SALE_ALL2 *sData)const; 
						
	// Account Methods	
	BOOL					GetDecryptedAccount(char* sEncryptAccount,char* sEncryptedPart,char* sDecryptedAccount); 
	
	// General Encoding\Decoding Methods
	BOOL					EncodeForTrs(const short nTrsStatus, CARD_SALE_ALL3* pcCardSaleAll3InOut);							//4.0.20.503
	BOOL					EncodeForTrs(const short nTrsStatus, PAY_AT_PUMP_INFO *pcPAPInfoInOut);							//4.0.23.0 TD 76407
	
	// Decoding Methods
	BOOL					RestoreData(PAY_AT_PUMP_INFO* pcPayAtPumpInfoInOut, const long lTrsNumber = 0);	//4.0.23.20
	
	// Receipt Index Methods		
	long					GetReceiptIndex(char* sIndexOut, const PAY_AT_PUMP_INFO *pPAPInfoIn, const long lPumpNumber);
	long					GetReceiptIndex(char* sIndexOut, const BSTR sTrack2In, const long lFlags, const long lPumpNumber, const long lTrsNumber = 0);	
 	void					GenerateIndex(PAY_AT_PUMP_INFO* pInfo, const BOOL bStoreAccountNum = FALSE, const long lTrsNumber = 0);
	void					GenerateIndex(CARD_SALE_ALL3* pcCardSaleAll3InOut, const BOOL bStoreAccountNum = FALSE, const long lTrsNumber = 0);

	// Control Methods
	BOOL					IsNeedToRemoveCardDataFromLog()const;
	inline BOOL				IsEncodeActive()const;
	BOOL					IsPAPEnabled();	//4.0.21.13 68756

	void					SetNoneOPTReceiptAccountMask(PAY_AT_PUMP_INFO* pcPayAtPumpInfo, const char* pszAccountNum)const;

	// Access configurations Methods		
	BOOL					GetExcludesLoyalty()const;								//4.0.22.13	TD 72716
	inline BOOL				GetPCIActiveFlag()const{ return m_bPCIActiveFlag; }		//4.0.22.13	TD 72716
	BOOL					GetRemoveProhibitedData()const;							//4.0.22.13	TD 72716
	void					SetRemoveProhibitedData(const BOOL bVal);				//4.0.22.13	TD 72716
	
	void					ChangesResponse();
	void					SendChangesAlert(CString& strAlertDescriptionOut);
	
	// Finalization Methods	
	virtual void			CleanUp();

protected:

	// ConfigurationLoader overloaded Methods
	virtual BOOL							IsReInitRequired();
	virtual void							ReInit();
	inline virtual CConfigurationLoader*	GetChild()const;

private:

	inline BOOL				IsDSSActiveFlagChanged(const BOOL bLastDSSActiveFlag)const;
	inline BOOL				IsPCIActiveFlagChanged(const BOOL bLastPCIActiveFlag)const;

	// Configuration Members
	BOOL					m_bDSSActiveFlag;				//4.0.22.13	TD 72716
	BOOL					m_bPCIActiveFlag;				//4.0.22.13	TD 72716

	//
	CBasicProtectData		m_cBasicProtectData;			//4.0.22.13	TD 72716
	CBaseEncodeHandler*		m_pcBaseEncodeHandler;		
};


#endif // !defined(AFX_FUELENCRYPT_H__4208C588_6728_4D52_BFC5_8B596EA7741A__INCLUDED_)
