// BasicProtectData.h: interface for the CBasicProtectData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASICPROTECTDATA_H__07F9B8D6_F238_479A_A5BB_EAF0DFF52CF9__INCLUDED_)
#define AFX_BASICPROTECTDATA_H__07F9B8D6_F238_479A_A5BB_EAF0DFF52CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBasicProtectData
{
public:
	// Construction/Destruction
	CBasicProtectData();
	CBasicProtectData(const CBasicProtectData& cBasicProtectData);
	virtual ~CBasicProtectData();
	
	// Initialization Methods
	virtual void	LoadConfiguration(const BOOL bCreate);	//4.0.22.13	TD 72716
	
	// Access Methods
	inline BOOL		GetRemoveCardDataFromLog()const{ return m_bRemoveCardDataFromLog;}

	// Replace protected data
	virtual BOOL	RemoveCardInfoFromMessage(char *sIn, long lInSize, char *sOut)const;
	virtual BOOL	RemoveCardInfoFromMessage(char *sIn, long lInSize, CString &sOut)const;
	virtual BOOL	ReplaceBufferUseSize(char *pszOutBuff, const char *psBufferChangeData, const long lBufferChangeDataSize)const;
	virtual BOOL	MaskOLAString(char *pszOLAString, char *pszInBuff, char *pszOutBuff, long *plOutBuffLen)const;
	virtual void	MaskOLAFields(CXMLInterface *sData)const;
	virtual void	MaskOLAFields(CARD_SALE_ALL3 *sData)const;
	virtual void	MaskOLAFields(RECEIPT_DATA4_EX *sData)const; //4.0.27.45
	virtual void	MaskOLAFields(CARD_SALE_ALL2 *sData)const;   //4.0.27.45

	virtual BOOL	MaskAccount(char* psMaskedAccountNumberOut, const char* psOpenAccountNumber, const MaskTypeEnum eMaskType = MASK_TYPE_LOG)const;
	
	// Control Method
	inline	BOOL	IsBasicProtectActive()const{ return m_bRemoveCardDataFromLog;}
		
protected:					
							
	//Mask Methods			
	virtual BOOL	MaskOLAString(char *pszOLAString, char *pszBuff,int iSizeOFOutBuffer, long lFlags)const;
	virtual void	GetMaskLengths(size_t* piHeadFreeLen, size_t* piTailFreeLen, const size_t iOpenStrLen, const MaskTypeEnum eMaskType = MASK_TYPE_LOG)const;
	long			MaskString(char* pszMaskedStrOut, const char* pszOpenStrIn, const char chMaskChar, const size_t iHeadFreeLen = 0, const size_t iTailFreeLen = 0)const;	
	BOOL			IsProtectedParamResult( char* pszOLAString )const;

private:
	
	BOOL							m_bRemoveCardDataFromLog;
	AccountNumberMaskStandartEnum	m_eAccountMaskStandart; 
};


#endif // !defined(AFX_BASICPROTECTDATA_H__07F9B8D6_F238_479A_A5BB_EAF0DFF52CF9__INCLUDED_)
