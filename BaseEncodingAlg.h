// BaseEncodingAlg.h: interface for the CBaseEncodingAlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEENCODINGALG_H__38B88195_4AD9_40F4_9277_431C3890D34A__INCLUDED_)
#define AFX_BASEENCODINGALG_H__38B88195_4AD9_40F4_9277_431C3890D34A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"
#include "BaseEncodeHandler.h"


//--------------------------------------------------------------------
//	 CDSSInitThread  class
//--------------------------------------------------------------------

class CDSSInitThread : public CThread  
{
public:
	// Construction/Destruction
	CDSSInitThread ();
	virtual ~CDSSInitThread ();
	
	// Initialization Methods
	CThreadRetCode		Init();

	// Control Methods
	CThreadRetCode		StartDSSInitThread();
	CThreadRetCode		StopDSSInitThread();

	// Main
	void				ThreadRoutine(void * pData = NULL);	

	// Access Methods
	inline BOOL			IsDSSInitThreadActive(){return m_bActive;};	//for sampling from CRDSSHandler

	// Finalization Methods
	CThreadRetCode		KillDSSInitThread();

private:
	// Members
	BOOL	m_bCreated;
	BOOL	m_bActive;

	DWORD	m_dwCurrentInitState;
};


//--------------------------------------------------------------------
//					CBaseEncodingAlg class
//--------------------------------------------------------------------


// DSS
#import "DSS.dll"  no_namespace named_guids	//4.0.20.501 TD-60236

// DPS
#import "Retalix.DPS.COM.V1.tlb" raw_interfaces_only no_namespace //  4.0.31.80 TD 373856
#import "Retalix.DPS.COM.tlb" raw_interfaces_only rename("_COMInt","_COMIntV2") rename("COMInt","COMIntV2") no_namespace //  4.0.31.80 TD 373856


class CBaseEncodingAlg : virtual public CBasicProtectData, public CConfigurationLoader
{
public:
	CBaseEncodingAlg(const CString& strLogMsgPrefix, const GUID& guidName);
	virtual ~CBaseEncodingAlg();

	// Initialization Methods
	virtual EncodeComDllRetCodeEnum		Init(BOOL bLoadConfig = TRUE) = 0;

	// ConfigurationLoader overloaded Methods
	virtual void				LoadConfiguration(const BOOL bCreate = FALSE);
	virtual BOOL				IsReInitRequired();									//4.0.22.73 TD 76462
	virtual void				ReInit();											//4.0.22.73 TD 76462

	virtual void				RemoveConfidencesDataFromMemory(short bySessionType, PAY_AT_PUMP_INFO* pInfoInOut){/*EMPTY METHOD*/};		//4.0.24.60 TD 112773
	virtual void				RemoveConfidencesData(CARD_SALE_ALL3* pInfoInOut);

	// Main Method
	virtual EncodeRetCodeEnum	Algorithm(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* pszAccountToEncode, const long lTrsNumber = 0) = 0;

	// Decrypt
	virtual EncodeRetCodeEnum	RestoreData(CARD_SALE_ALL3* pcCardSaleAll3InOut, const long lTrsNumber = 0){ return ENCODE_OK; }

	// Control Methods
	virtual inline BOOL			IsPAPEnabled() const = 0;
	virtual inline BOOL			IsAlgorithmEnabled(const short nSessionType) const = 0;
	virtual inline BOOL			IsValidEncodedDataSize(size_t iSize) const;
	virtual inline BOOL			IsFieldNotEmpty(const char* pbyFeild, const size_t iFeildLen)const;
	virtual	BOOL				IsRestoreDataEnabled(CARD_SALE_ALL3* const pcCardSaleAll3In) const { return FALSE; }						//4.0.23.20
	virtual inline BOOL			IsLookForInternalReceiptIndex()const { return m_bLookForInternalReceiptIndex; }					//4.0.26.500 TD 144219
	virtual void				SetNoneOPTReceiptAccountMask(PAY_AT_PUMP_INFO* pcPayAtPumpInfo, const char* pszAccountNum)const{/*EMPTY METHO*/};

	// Access Methods
	inline DWORD				GetCurrentInitState(){ return m_dwCurrentInitState; }

	//Receipt Methods
	virtual long				GetReceiptIndex(char* psIndexOut, const PAY_AT_PUMP_INFO* pcPayAtPumpInfoIn, const long lPumpNumber = 0)const;
	virtual long				GetReceiptIndex(char* psIndexOut, const char* pszInfoIn) = 0;							//4.0.23.0 TD 76407
	virtual long				GetReceiptIndex(char* psIndexOut, const BSTR sTrack2In, const long lFlags, const long lPumpNumber = 0) = 0;

	// Masking control methods
	virtual inline BOOL			IsMaskingAccountRequired(const CARD_SALE_ALL3* pcCardSaleAll3InOut, const MaskTypeEnum eMaskType = MASK_TYPE_LOG)const
								{ return TRUE; }


	// Log Message Methods
	virtual void				WriteEncodingLogMsg(const long lPumpNumber, const long lTrsNumber, const CString& strLogMsg, const long lLogLevel = DEFAULT_LOG_LEVEL) const;
	virtual void				WriteEncodingLogMsg(const long lPumpNumber, const CString& strLogMsg, const long lLogLevel = DEFAULT_LOG_LEVEL) const;
	virtual void				WriteEncodingLogMsg(const CString& strLogMsg, const long lLogLevel = DEFAULT_LOG_LEVEL) const;
	
	// Changes control methods
	virtual void				ChangesResponse(CString& strAlertDescriptionOut){/*DEFAULT EMPTY METHOD*/};

	// Finalization Methods	
	virtual EncodeComDllRetCodeEnum		CloseSession() = 0;
	virtual void						CleanUp();
	
protected:
	CBaseEncodingAlg();
	
	// Initialization Methods
	virtual EncodeComDllRetCodeEnum	InitResponse(	const long	lRetCode, 
													const char* szCertificate, 
													const long	lEncodeDLLErrorCode, 
													const char* szErrorMessage) = 0;
	// General Encode Methods
	virtual EncodeRetCodeEnum	Encode(	char* psEncodedBuffOut, const char* pszOpenBuffIn, const long lPumpNumber = 0, 
										size_t* piEncryptedDataSize = NULL, size_t* piHashedDataSize = NULL, long lTrsNumber = 0);
	
	//DLL Entry Point to Encode Methods
	virtual EncodeRetCodeEnum	EncodeDLLEntryPoint(const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piEncryptedDataSize = NULL, size_t* piHashedDataSize = NULL, long lTrsNumber = 0) = 0;

	// Save Encoded Data to CARD_SALE_ALL3 Methods
	virtual void				SaveEncodedData(CARD_SALE_ALL3* pcCardSaleAll3InOut, 
										const char* psEncryptedData, const size_t iEncryptedDataSize, 
										const char* psHashedData = NULL, const size_t iHashedDataSize = 0, 
										const long lTrsNumber = 0) = 0;

	virtual void				SaveEncryptedData(CARD_SALE_ALL3* pcCardSaleAll3InOut, 
										const char* psEncryptedData, const size_t iEncryptedDataSize, 
										const long lTrsNumber = 0);

	virtual void				SaveHashedData(CARD_SALE_ALL3* pcCardSaleAll3InOut, const char* psHashedData, const size_t iHashedDataSize);
	virtual void				SaveHashedData(CARD_SALE_ALL3* pcCardSaleAll3Out, const long lData, const size_t iHashedDataSize);

	virtual void				SaveEncryptedData2(CARD_SALE_ALL3* pcCardSaleAll3InOut, 
										const char* psEncryptedData, const size_t iEncryptedDataSize, 
										const long lTrsNumber = 0);

	// Send alerts methods
	virtual void				SendInitializationFailedAlert(const long lEncodeDLLErrorCode, const char* szErrorMessage) const{/*EMPTY METHOD TO OVERLOAD*/};
	virtual void				SendEncryptionFailedAlert(const long lEncodeDLLErrorCode, const char* szErrorMessage, long lTrsNumber = 0) const{/*EMPTY METHOD TO OVERLOAD*/};
	virtual void				SendHashingFailedAlert(const long lEncodeDLLErrorCode, const char* szErrorMessage, long lTrsNumber = 0) const{/*EMPTY METHOD TO OVERLOAD*/};

	// System control methods
	void						SendSystemStateToCL();

	// General control methods
	virtual inline long			GetPumpNumber(const CARD_SALE_ALL3* pcCardSaleAll3In) const;

private:
	
	void						LoadEncodingComDll(const GUID& guidName);

protected:

	CString							m_strLogMsgPrefix;	

	// Configuration members
	BOOL							m_bDisablePAPOnDSSFail;	
	BOOL							m_bDeclinePAPOnDSSFail;	

	// Control members
	BOOL							m_bHashEnabled;
	DWORD							m_dwCurrentInitState;
	char							m_szCertificate[MAX_FIELD_VAL];	

	BOOL							m_bLookForInternalReceiptIndex;
									
	CDSSInitThread					m_cDSSInitThread; 

	CBaseEncodingComDll*			m_pEncodingComDll;		//pointer to Encode dll
};



//--------------------------------------------------------------------
//					CDSSBaseEncodingAlg class		//4.0.23.0 TD 76407
//--------------------------------------------------------------------

class CDSSBaseEncodingAlg : virtual public CBaseEncodingAlg
{
protected:
	CDSSBaseEncodingAlg(const CString& strLogMsgPrefix);

public:
	CDSSBaseEncodingAlg();
	~CDSSBaseEncodingAlg();

	// Initialization Methods
	virtual EncodeComDllRetCodeEnum		Init(BOOL bLoadConfig = TRUE);

	//ReceiptMethods
	virtual long				GetReceiptIndex(char* psIndexOut, const char* pszInfoIn);								
	virtual long				GetReceiptIndex(char* psIndexOut, const BSTR sTrack2In, const long lFlags, const long lPumpNumber = 0);

	// Finalization Methods	
	virtual EncodeComDllRetCodeEnum		CloseSession();

protected:

	// Initialization Methods
	virtual EncodeComDllRetCodeEnum	InitResponse(	const long	lRetCode, 
													const char* szCertificate, 
													const long	lEncodeDLLErrorCode, 
													const char* szErrorMessage);
};


//--------------------------------------------------------------------
//					CDPSBaseEncodingAlg class		//4.0.23.0 TD 76407
//--------------------------------------------------------------------
class CDPSBaseEncodingAlg : virtual public CBaseEncodingAlg
{
public:
	CDPSBaseEncodingAlg();
	~CDPSBaseEncodingAlg();

	// Initialization Methods
	virtual EncodeComDllRetCodeEnum		Init(BOOL bLoadConfig = TRUE);

	virtual void						LoadConfiguration(const BOOL bCreate = FALSE);

	//ReceiptMethods
	virtual long				GetReceiptIndex(char* psIndexOut, const PAY_AT_PUMP_INFO* pcPayAtPumpInfoIn, const long lPumpNumber = 0)const;	//4.0.23.20	TD 77627
	virtual long				GetReceiptIndex(char* psIndexOut, const char* pszInfoIn);								
	virtual long				GetReceiptIndex(char* psIndexOut, const BSTR wsTrack2In, const long lFlags, const long lPumpNumber = 0);

	// Finalization Methods	
	virtual EncodeComDllRetCodeEnum		CloseSession();
	
protected:

	// Initialization Methods
	virtual EncodeComDllRetCodeEnum	InitResponse(	const long	lRetCode, 
													const char* szCertificate, 
													const long	lEncodeDLLErrorCode, 
													const char* szErrorMessage);
	inline BOOL		IsConfigFileNameCorrupted();

protected:

	char	m_szConfigFileName[MAX_PATH_NAME];	
};


#endif // !defined(AFX_BASEENCODINGALG_H__38B88195_4AD9_40F4_9277_431C3890D34A__INCLUDED_)
