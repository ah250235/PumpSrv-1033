// BaseEncodeHandler.h: interface for the CBaseEncodeHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEENCRYPTHANDLER_H__36F2E879_36F7_4C5E_A660_6F310A5180CA__INCLUDED_)
#define AFX_BASEENCRYPTHANDLER_H__36F2E879_36F7_4C5E_A660_6F310A5180CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MIN_PASS_SIZE   5
#define EVEN_BIT_MASK	0xAA	// 10101010
#define ODD_BIT_MASK	0x55	// 01010101
#define PCI_FLAG_ON		1		//4.0.22.13	TD 72716
#define PCI_FLAG_OFF	0		//4.0.22.13	TD 72716

enum EncodeRetCodeEnum{
		ENCODE_OK,							//0	
		ENCODE_FAIL,						//1
		ENCODE_NO_TRACK2,					//2
		ENCODE_BUFF_TOO_SMALL,				//3
		ENCODE_STRING_NOT_FOUND,			//4
		ENCODE_STRING_ILLEGAL_SIZE,			//5
		ENCODE_ENCRYPTION_FAIL,				//6
		ENCODE_HASH_FAIL,					//7
		ENCODE_DECODING_FAIL,				//8
};

enum EncodeComDllRetCodeEnum {

		ENCODE_DLL_OK,							//0
		ENCODE_DLL_FAIL_CREATE_INTERFACE,		//1
		ENCODE_DLL_COM_ERROR,					//2					
		ENCODE_DLL_NOT_CONNECTED,			    //3
		ENCODE_DLL_CUSTOMIZE_ERROR,				//4
		ENCODE_DLL_LOG_UNCUSTOMIZED,			//5
		ENCODE_DLL_FAIL_INITIALIZATON,			//6
		ENCODE_DLL_UNKNOWN_ERROR,				//7
		ENCODE_DLL_CLOSE_SESSION_FAIL,				//8
};

enum FuelEncodeAlgorithmEnum{
		ENCODE_ALGORITHM_NONE = -1,							//-1
		ENCODE_ALGORITHM_ENCRYPTION,						//0
		ENCODE_ALGORITHM_HASH,								//1
		ENCODE_ALGORITHM_ENCRYPTION_AND_HASH,				//2
		ENCODE_ALGORITHM_ASYMMETRIC_ENCRYPTION_AND_HASH,	//3
};

enum AccountNumberMaskStandartEnum{
		ANMS_HEAD6_TAIL4,
		ANMS_HEAD0_TAIL4,
		ANMS_16_DIGIT_RULE,
};

//Fuel Encode System States
#define FESS_ALLOWED_ALL				0x00000000
#define FESS_INIT_ENCRYPT_FAILED		0x00000001
#define FESS_INIT_HASH_FAILED			0x00000002
#define FESS_PAP_DISABLED				0x00000004
#define FESS_REPRINT_DISABLED			0x00000008
#define FESS_PAP_ENABLED				0x00000010

//Masking type (DB/Log)
enum MaskTypeEnum{
		MASK_TYPE_LOG,		//0
		MASK_TYPE_DB,		//1
};

#include "BasicProtectData.h"


//*************************************************************************************
//								CEncodingComDll class
//*************************************************************************************


class CBaseEncodingComDll 
{

public:	
	CBaseEncodingComDll(const GUID& guidName);	
	virtual ~CBaseEncodingComDll();

	EncodeComDllRetCodeEnum	Init();
	
	void					WriteComError(_com_error &e) const;
	
	EncodeComDllRetCodeEnum RestoreCOM();

	EncodeComDllRetCodeEnum CleanUp();

	virtual BOOL			IsPtrNULL() = 0;

protected:

	virtual EncodeComDllRetCodeEnum ConnectEncodingComDll() = 0;

	virtual EncodeComDllRetCodeEnum CloseConnectionEncodingComDll() = 0;

protected:
 	CBaseEncodingComDll();

	const GUID			GUID_NAME;
};


template<class T>
class CEncodingComDll : public CBaseEncodingComDll
{
public:

	CEncodingComDll(const GUID& guidName) : CBaseEncodingComDll(guidName){}	

	virtual BOOL IsPtrNULL()
	{
		BOOL bRetCode = (CEncodingComDll<T>::PtrInstance() == NULL) ? TRUE : FALSE;

		return bRetCode;
	}

	//Singletone Access to dll
	static CComPtr<T>& PtrInstance()
	{		
		static CComPtr<T> s_pEncodePtr = NULL;			//pointer to dll
		
		return s_pEncodePtr;
	}

protected:

	EncodeComDllRetCodeEnum ConnectEncodingComDll()
	{
		EncodeComDllRetCodeEnum eEncodeDLLRetCode = ENCODE_DLL_OK;
		
		if(IsPtrNULL() == TRUE)
		{
			try
			{		
				HRESULT hRes = 0;
				hRes = CEncodingComDll<T>::PtrInstance().CoCreateInstance(GUID_NAME);	

				if(SUCCEEDED(hRes))		
				{				
					_Module.m_server.m_cLogger.LogMsg("ConnectEncodingComDll, Successfully connected to EncodeDll/DecodeDLL");      // TD 347994 
				}
				else
				{
					CEncodingComDll<T>::PtrInstance() = NULL; // also release the object
					eEncodeDLLRetCode = ENCODE_DLL_FAIL_CREATE_INTERFACE;

					CString str;
					str.Format("ConnectEncodingComDll, Fail connect to EncodeDll! Res=%ld", hRes);
					_Module.m_server.m_cLogger.LogMsg(str, LOG_LEVEL_1);
				}

			}
			catch (_com_error& e)
			{
				WriteComError(e);
				CEncodingComDll<T>::PtrInstance() = NULL; // also release the object
				eEncodeDLLRetCode = ENCODE_DLL_FAIL_CREATE_INTERFACE;
			}
			catch(...)
			{
				if (LOG_LOGIC_ERROR)				
					_Module.m_server.m_cLogger.LogMsg("ConnectEncodingComDll, Unknown error! Fail connect to EncodeDll", LOG_LEVEL_1);			
				
				CEncodingComDll<T>::PtrInstance() = NULL; // also release the object
				eEncodeDLLRetCode = ENCODE_DLL_FAIL_CREATE_INTERFACE;
			}
		}
		
		return eEncodeDLLRetCode;
	}


	EncodeComDllRetCodeEnum CloseConnectionEncodingComDll()
	{
		EncodeComDllRetCodeEnum eEncodeDLLRetCode = ENCODE_DLL_OK;
		
		if(IsPtrNULL() != TRUE)
		{
			try
			{
				CEncodingComDll<T>::PtrInstance().Release();
				CEncodingComDll<T>::PtrInstance().Detach();
				CEncodingComDll<T>::PtrInstance() = NULL;

				_Module.m_server.m_cLogger.LogMsg("CloseConnectionEncodingComDll, Closed the connection to EncodeDll/DecodeDLL");  // TD 347994 
			}
			catch (_com_error& e)  
			{
				WriteComError(e);
				CEncodingComDll<T>::PtrInstance() = NULL; 
				eEncodeDLLRetCode = ENCODE_DLL_COM_ERROR;		
			}
			catch(...)
			{
				CEncodingComDll<T>::PtrInstance() = NULL;
				eEncodeDLLRetCode = ENCODE_DLL_COM_ERROR;

				_Module.m_server.m_cLogger.LogMsg("CloseConnectionEncodingComDll, Failed to closed the connection to EncodeDll", LOG_LEVEL_1);
			}
		}
		
		return eEncodeDLLRetCode;
	}

};


//*************************************************************************************
//								CBaseEncodeHandler class
//*************************************************************************************

class CBaseEncodeHandler : virtual public CBasicProtectData, public CConfigurationLoader
{
public:
	// Construction/Destruction
	CBaseEncodeHandler(const CBasicProtectData& cBasicProtectData);
	virtual ~CBaseEncodeHandler();

	// Initialization Methods
	virtual EncodeComDllRetCodeEnum	Init() = 0;

	// DB Methods
	virtual void					RemoveConfidencesData(PAY_AT_PUMP_INFO* pInfoInOut);
	virtual void					RemoveConfidencesData(CARD_SALE_ALL3* pInfoInOut);
		
	virtual void					RemoveConfidencesDataFromMemory(short bySessionType, PAY_AT_PUMP_INFO* pInfoInOut) = 0;		//4.0.24.60 TD 112773
									
	// Account Methods				
	virtual BOOL					GetDecryptedAccount(char* sEncodeAccount, char* sEncodedPart, char* sDecodedAccount) = 0;
									
	// General Encoding\Decoding Methods
	virtual EncodeRetCodeEnum		EncodeForTrs(const short nTrsStatus, CARD_SALE_ALL3* pcCardSaleAll3InOut, const long lPumpNumber = 0) {return ENCODE_OK;/*Not use in PCI!!!!!!!!!!*/};	//4.0.20.503
	virtual EncodeRetCodeEnum		EncodeForTrs(const short nTrsStatus, PAY_AT_PUMP_INFO* pcPayAtPumpInfoInOut, const long lPumpNumber = 0){return ENCODE_OK;/*Not use in PCI!!!!!!!!!!*/};	//4.0.23.0 TD 76407
	
	// Decoding Methods
	virtual EncodeRetCodeEnum		RestoreData(PAY_AT_PUMP_INFO* pcPayAtPumpInfoInOut, const long lTrsNumber = 0){return ENCODE_OK;/*Not use in PCI!!!!!!!!!!*/};	//4.0.23.20

	// Receipt Index Methods		
	virtual long					GetReceiptIndex(char* sIndexOut, const PAY_AT_PUMP_INFO* pcPayAtPumpInfoIn, const long lPumpNumber = 0) = 0;
	virtual long					GetReceiptIndex(char* sIndexOut, const BSTR sTrack2In, const long lFlags, const long lPumpNumber = 0) = 0;		
	virtual void					GenerateIndex(PAY_AT_PUMP_INFO* pInfo, const BOOL bStoreAccountNum = FALSE, const long lTrsNumber = 0) = 0;
	virtual void					GenerateIndex(CARD_SALE_ALL3* pcCardSaleAll3InOut, const BOOL bStoreAccountNum  = FALSE, const long lTrsNumber = 0) = 0;		//4.0.25.30 
	
	// Access configuration Methods	
	inline virtual BOOL				GetExcludesLoyalty(){ return m_bExcludesLoyalty; }
	inline virtual void				SetExcludesLoyalty(const BOOL bExcludesLoyalty){ m_bExcludesLoyalty = bExcludesLoyalty; }
									
	inline virtual BOOL				GetRemoveProhibitedData(){ return m_bRemoveProhibitedData; }
	inline virtual void				SetRemoveProhibitedData(const BOOL bVal){ m_bRemoveProhibitedData = bVal; }

	// Control methods
	virtual BOOL					IsPAPEnabled() = 0; //4.0.22.13	TD 72716

	virtual void					SetNoneOPTReceiptAccountMask(PAY_AT_PUMP_INFO* pcPayAtPumpInfo, const char* pszAccountNum)const{/*EMPTY METHO*/};		//4.0.27.501 TD 233308
	
	virtual void					ChangesResponse(CString& strAlertDescriptionOut) = 0;
	virtual BOOL					IsLookForInternalReceiptIndex() { return FALSE; }

	// Finalization Methods			
	virtual void					CleanUp() = 0;	//4.0.22.13	TD 72716

	virtual inline BOOL				IsPCIExcludesLoyaltyChanged(const BOOL bLastPCIExludesLoyalty)const;
protected:

	// Configuration Loader overloaded methods
	virtual void					LoadConfiguration(const BOOL bCreate = FALSE);	//4.0.22.13	TD 72716

protected:
	
	// Configuration Members
	BOOL							m_bRemoveProhibitedData;		//4.0.22.13	TD 72716
	BOOL							m_bExcludesLoyalty;				//4.0.22.13	TD 72716
};





#endif // !defined(AFX_BASEENCRYPTHANDLER_H__36F2E879_36F7_4C5E_A660_6F310A5180CA__INCLUDED_)
