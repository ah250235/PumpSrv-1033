// PCIEncodeHandler.h: interface for the CPCIEncodeHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCIENCRYPTHANDLER_H__08F4AB0D_D4E6_495A_9E71_8C3219D0485F__INCLUDED_)
#define AFX_PCIENCRYPTHANDLER_H__08F4AB0D_D4E6_495A_9E71_8C3219D0485F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "ESM.dll"	no_namespace named_guids	//4.0.20.501 TD 60236
#import "DSM.dll"	no_namespace named_guids	// TD 347994 

#include "BaseEncodeHandler.h"

class CPCIEncodeHandler : public CBaseEncodeHandler  
{
public:
	// Construction/Destruction
	CPCIEncodeHandler(const CBasicProtectData& cBasicProtectData);
	virtual ~CPCIEncodeHandler();
	
	// Initialization Methods
	EncodeComDllRetCodeEnum	Init();
							
	void					RemoveConfidencesData(PAY_AT_PUMP_INFO* pInfoInOut);	
	void					RemoveConfidencesData(CARD_SALE_ALL3* pInfoInOut);
	void					RemoveConfidencesDataFromMemory(short bySessionType, PAY_AT_PUMP_INFO* pInfoInOut);						

	// Account Methods		
	BOOL					GetDecryptedAccount(char* sEncodeAccount, char* sEncodedPart, char* sDecodedAccount);
							
	// Access Methods	
	
	// Receipt Index Methods
	long					GetReceiptIndex(char* sIndexOut, const PAY_AT_PUMP_INFO* pcPayAtPumpInfoIn, const long lPumpNumber = 0);
	long					GetReceiptIndex(char* sIndexOut, const BSTR sTrack2In, const long lFlags, const long lPumpNumber = 0);
	void					GenerateIndex(PAY_AT_PUMP_INFO* pInfo, const BOOL bStoreAccountNum = FALSE, const long lTrsNumber = 0);			//4.0.22.13	TD 72716
	void					GenerateIndex(CARD_SALE_ALL3* pcCardSaleAll3InOut, const BOOL bStoreAccountNum = FALSE, const long lTrsNumber = 0);
	
	// Control Methods
	BOOL					IsPAPEnabled(){ return TRUE; }		//4.0.22.13	TD 72716

	virtual void			ChangesResponse(CString& strAlertDescriptionOut){/*EMPTY METHOD*/}

	// Finalization Methods	
	void					CleanUp();
				
protected:
	
	// Protected Account Methods	
	void					AccountNumToParam2(PAY_AT_PUMP_INFO *pInfo);
	BOOL					MaskAccount(char* pszMaskedAccountNumberOut, const char* pszOpenAccountNumber, const MaskTypeEnum eMaskType = MASK_TYPE_LOG)const;
						
	// ConfigurationLoader overloaded Methods			//4.0.22.43	TD 72716
	virtual BOOL			IsReInitRequired(){ return FALSE; }
	virtual void			ReInit(){/*EMPTY METHOD!!*/};

private:					
	// Private Methods		

	EncodeRetCodeEnum		EncryptAccount(IN const char * pszAccountNumberIn, OUT char* pszEncryptedBuffOut, long lPumpNum);	// TD 347994 
	void					LoyalIdToParam1(PAY_AT_PUMP_INFO *pInfo);
	void					GetCustomerEncryptionKey( char* szCustomerEncryptionKey );  // TD 347994 
							
	// Private Members		
	CEncodingComDll<IEncryption>	m_cEncodingComDll;		//pointer to Encode dll
	CEncodingComDll<IDecryption>	m_cDecodingComDll;		//pointer to Decode dll		// TD 347994 
};							

#endif // !defined(AFX_PCIENCRYPTHANDLER_H__08F4AB0D_D4E6_495A_9E71_8C3219D0485F__INCLUDED_)
