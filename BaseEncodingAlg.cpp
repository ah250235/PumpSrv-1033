// BaseEncodingAlg.cpp: implementation of the CBaseEncodingAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseEncodingAlg.h"

//--------------------------------------------------------------------
//				class CDSSInitThread
//--------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDSSInitThread::CDSSInitThread () : CThread(FALSE, 1000 , TRUE)   
{
	DWORD dwTimeout = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "DSSInitRetryInterval", &dwTimeout, (DWORD)60, TRUE);
	dwTimeout *= 1000;		//second

	CThread::SetSleepDuration(dwTimeout);

	m_bCreated = FALSE;
	m_bActive = FALSE;
}


CDSSInitThread::~CDSSInitThread () 
{

}


CThreadRetCode CDSSInitThread::Init()
{
	CThreadRetCode eThreadRetCode = THREAD_OK; 

	if(m_bCreated == FALSE)
	{		
		eThreadRetCode = CThread::CreateThread();
		if( eThreadRetCode == THREAD_OK )
		{
			m_bCreated = TRUE;
			_LOGMSG.LogClassMsg("DSSInitThread", "Init", 0, LOG_NONE, "Thread was created"); 
		}
		else
		{
			_LOGMSG.LogClassMsg("DSSInitThread", "Init", 0, LOG_NONE, "Thread fail created");
			eThreadRetCode = THREAD_NOT_CREATED;
		}
	}

	return eThreadRetCode;
}


void CDSSInitThread::ThreadRoutine(void* pData)
{
	EncodeComDllRetCodeEnum eEncodeDLLRetCode = ENCODE_DLL_OK;

	eEncodeDLLRetCode = _Module.m_server.m_cProtectedDataHandler.Init();

	if (eEncodeDLLRetCode == ENCODE_DLL_OK)
	{			
		StopDSSInitThread();
		_LOGMSG.LogClassMsg("DSSInitThread", "ThreadRoutine", 0, LOG_NONE, "Re-Init the EncodeHandler successfully");
	}
}


CThreadRetCode CDSSInitThread::StartDSSInitThread ()
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	if (m_bActive)
		eThreadRetCode = THREAD_ALREADY_RUNNING;

	if (!eThreadRetCode)
	{
		if (m_bCreated && !m_bActive)
		{
			if ((eThreadRetCode = CThread::StartThread()) == THREAD_OK)
			{
				m_bActive = TRUE;
				_LOGMSG.LogClassMsg("DSSInitThread", "StartCDSSInitThread", 0, LOG_NONE, "*** Tread Started successfully");
			}
			else
			{
				_LOGMSG.LogClassMsg("DSSInitThread", "StartCDSSInitThread", 0, LOG_NONE, "*** Tread Failed to start!!!", LOG_LEVEL_1);
			}
		}
		else
			eThreadRetCode = THREAD_NOT_CREATED;
	}

	return eThreadRetCode;
}

CThreadRetCode CDSSInitThread::StopDSSInitThread()
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	//Stop the thread
	eThreadRetCode = CThread::StopThread();

	if(THREAD_OK == eThreadRetCode)
	{
		m_bActive = FALSE;		
		_LOGMSG.LogMsg("DSSInitThread Stoped successfully");
	}
	else
	{
		_LOGMSG.LogMsg("DSSInitThread not Stoped successfully");
		eThreadRetCode = THREAD_FAILED_TO_TERMINATE;
	}	

	return(eThreadRetCode);
}

CThreadRetCode CDSSInitThread::KillDSSInitThread()
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	if(m_bActive)
	{
		m_bActive = m_bCreated = FALSE;

		//Killing the thread
		eThreadRetCode = CThread::KillThread();

		if(THREAD_OK == eThreadRetCode)
			_LOGMSG.LogMsg("DSSInitThread terminated successfully");
		else
			_LOGMSG.LogMsg("DSSInitThread not terminated successfully");	
	}
	
	return(eThreadRetCode);
}


//--------------------------------------------------------------------
//					CBaseEncodingAlg class
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
//					Construction/Destruction
//////////////////////////////////////////////////////////////////////


CBaseEncodingAlg::CBaseEncodingAlg(const CString& strLogMsgPrefix, const GUID& guidName) : 
		m_strLogMsgPrefix(strLogMsgPrefix)
{
	LoadEncodingComDll(guidName);

	m_dwCurrentInitState = FESS_ALLOWED_ALL;
	m_bHashEnabled = TRUE;
	memset(m_szCertificate, 0x00, sizeof(m_szCertificate));			//4.0.22.85

	m_bLookForInternalReceiptIndex = FALSE;
}

//protected
CBaseEncodingAlg::CBaseEncodingAlg()
{
	m_pEncodingComDll = NULL;
}


CBaseEncodingAlg::~CBaseEncodingAlg()
{

}


//private:
void CBaseEncodingAlg::LoadEncodingComDll(const GUID& guidName)
{

	if(guidName == CLSID_Cryptography)
	{
		m_pEncodingComDll = new CEncodingComDll<ICryptography>(guidName);			//DSS.dll

		WriteEncodingLogMsg("Create interface to DSS.dll");
	}
	else if(guidName == __uuidof(COMInt)) 
	{
		m_pEncodingComDll = new CEncodingComDll<_COMInt>(guidName);					//Retalix.DPS.COM.tlb
		WriteEncodingLogMsg("Create interface to Retalix.DPS.COM.tlb");
	}
	else if(guidName == __uuidof(COMIntV2)) //  4.0.31.80 TD 373856
	{
		m_pEncodingComDll = new CEncodingComDll<_COMIntV2>(guidName);				//Retalix.DPS.COM.V2.tlb
		WriteEncodingLogMsg("Create interface to Retalix.DPS.COM.V2.tlb");
	}
	else
	{
		m_pEncodingComDll = NULL;
		WriteEncodingLogMsg("Invalid GUID!!", LOG_LEVEL_1);
	}
}


//protected:
void CBaseEncodingAlg::LoadConfiguration(const BOOL bCreate)
{
	CBasicProtectData::LoadConfiguration(bCreate);

	DWORD dwTmp = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "DeclinePAPOnDSSFail", &dwTmp, (DWORD)0, bCreate);
	m_bDeclinePAPOnDSSFail = (0 == dwTmp) ? FALSE : TRUE;
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "DisablePAPOnDSSFail", &dwTmp, (DWORD)0, bCreate);	
	m_bDisablePAPOnDSSFail = (0 == dwTmp) ? FALSE : TRUE;
}


BOOL CBaseEncodingAlg::IsReInitRequired()
{
	return FALSE;
}


void  CBaseEncodingAlg::ReInit()
{
	CloseSession();

	Sleep(1000);
	
	Init(FALSE);
}


void  CBaseEncodingAlg::RemoveConfidencesData(CARD_SALE_ALL3* pInfoInOut)
{
	//memset(pInfoInOut->CardSaleAll.data.sPINBlock,   ' ', sizeof(pInfoInOut->CardSaleAll.data.sPINBlock));   

	CString strLogMsg;
	strLogMsg.Format("CBaseEncodingAlg::RemoveConfidencesData -%.16s", pInfoInOut->CardSaleAll.data.sPINBlock );
	WriteEncodingLogMsg(1, strLogMsg);

}


//////////////////////////////////////////////////////////////////////
//					Receipt Index Methods                             
//////////////////////////////////////////////////////////////////////

/******************************************************************************
 Description:	Get Receipt Index (key for receipt QDX) from PAY_AT_PUMP_INFO struct
 Returns:      	lRetIndexLen - Length of index
 Parameters:   	sIndexOUT - index
				pInfoIn	- PAY_AT_PUMP_INFO struct
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			17/08/2008	 13:34		Start 4.0.20.501 TD-60236 72716
******************************************************************************/
long CBaseEncodingAlg::GetReceiptIndex(char* psIndexOUT, const PAY_AT_PUMP_INFO* pcPayAtPumpInfoIn, const long lPumpNumber) const
{
	long lRetIndexLen = 0;

	if(pcPayAtPumpInfoIn != NULL)	
	{	
		memset(psIndexOUT, ' ', ENCRYPTED_DATA_SIZE);	

		//If DSSFailed (either hash or encrypt) - we save the transaction number as the key of the receipt qdx
		if (pcPayAtPumpInfoIn->CardSaleExtraData4.sDSSFailed == 1)
		{			
			lRetIndexLen = sizeof(pcPayAtPumpInfoIn->CardSaleInfo.cardData.sTranNbr);
			memcpy(psIndexOUT, pcPayAtPumpInfoIn->CardSaleInfo.cardData.sTranNbr, lRetIndexLen);

			WriteEncodingLogMsg(lPumpNumber, "GetReceiptIndex, DSSFailed=1, Get receipt index from sTranNbr!");
		}
		else
		{
			//return the Hash or encrypt string as the key of the receipt qdx
			lRetIndexLen = sizeof(pcPayAtPumpInfoIn->CardSaleExtraData4.sEncryptedAccount);
			memcpy(psIndexOUT, pcPayAtPumpInfoIn->CardSaleExtraData4.sEncryptedAccount, lRetIndexLen);

			CString strLogMsg;
			strLogMsg.Format("GetReceiptIndex, DSSFailed=0, Get receipt index from sEncryptedAccount=[%.100s], RetIndexLen=%ld", psIndexOUT, lRetIndexLen);
			WriteEncodingLogMsg(lPumpNumber, strLogMsg);
		}
	}
	
	return lRetIndexLen;
}


inline BOOL	CBaseEncodingAlg::IsValidEncodedDataSize(size_t iSize) const
{ 
	BOOL bRetCode = (iSize > ENCRYPTED_DATA_SIZE || iSize < 1) ? FALSE : TRUE; 

	return bRetCode;
}


//protected:
inline BOOL CBaseEncodingAlg::IsFieldNotEmpty(const char* pbyFeild, const size_t iFeildLen)const
{
	return ( *pbyFeild == 0 || ChrAll((char*)pbyFeild, iFeildLen, ' ') == TRUE ) ? FALSE : TRUE;
}

/******************************************************************************
 Description:	This method send events to CL:
				1) remove the reprint option from the terminal
				2) remove the P@P option from the Terminal.

				or both

				//implementation for CDSSEncryptAlg,CDSSHashAlg,CDSSEncryptAndHshAlg
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			17/08/2008	 13:34		Start 4.0.20.501 TD-60236
******************************************************************************/
//protected:
void CBaseEncodingAlg::SendSystemStateToCL()
{				
	// Disabled reprint
	if( (m_dwCurrentInitState & FESS_REPRINT_DISABLED) )
	{
		_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_DISABLE_REPRINT);
		WriteEncodingLogMsg("SendSystemStateToCL, GCI_EVENT_DISABLE_REPRINT was sent to CL"); //4.0.21.20
	}
	
	// Disabled PAP
	if ( (m_dwCurrentInitState & FESS_PAP_DISABLED) )
	{
		_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_OLA_OFFLINE);
		WriteEncodingLogMsg("SendSystemStateToCL, GCI_EVENT_OLA_OFFLINE was sent to CL"); //4.0.21.20
	}

	// Enabled PAP
	if ( (m_dwCurrentInitState & FESS_PAP_ENABLED) )
	{
		if (_Module.m_server.IsPAPEnabledOLA()) //4.0.21.13 68756
		{
			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_OLA_ONLINE);
			WriteEncodingLogMsg("SendSystemStateToCL, GCI_EVENT_OLA_ONLINE was sent to CL"); //4.0.21.20
		}
	}
}


//////////////////////////////////////////////////////////////////////
//					Encoding\Decoding Methods
//////////////////////////////////////////////////////////////////////
//protected:
EncodeRetCodeEnum CBaseEncodingAlg::Encode(char* psEncodedBuffOut, const char* pszOpenBuffIn, const long lPumpNumber, 
										   size_t* piEncryptedDataSize, size_t* piHashedDataSize, long lTrsNumber)
{
	EncodeComDllRetCodeEnum		eEncodeDLLRetCode = ENCODE_DLL_OK;
	EncodeRetCodeEnum			eEncodeRetCode = ENCODE_FAIL;
	long						lErrorCode = OK;

	if(m_pEncodingComDll->IsPtrNULL() == FALSE)
	{
		if((m_dwCurrentInitState & FESS_PAP_ENABLED))
		{			
			try
			{
				char		szErrorMessage[MAX_FIELD_VAL] = {0};
				
				eEncodeRetCode = EncodeDLLEntryPoint(pszOpenBuffIn, psEncodedBuffOut, &lErrorCode, szErrorMessage, piEncryptedDataSize, piHashedDataSize, lTrsNumber);

				if(eEncodeRetCode != ENCODE_OK)
				{
					if (LOG_LOGIC_ERROR)				
					{
						CString strLogMsg;
						strLogMsg.Format("Encoding failed! DSSRetErrorCode=%ld, RetCode=%ld, DSSRetErrorMessage=[%s]", lErrorCode, eEncodeRetCode, szErrorMessage);							

						WriteEncodingLogMsg(lPumpNumber, lTrsNumber, strLogMsg, LOG_LEVEL_1);		
					}
				}
			}
			catch(...)
			{
				if (LOG_LOGIC_ERROR )
				{				
					WriteEncodingLogMsg(lPumpNumber, lTrsNumber, "Encoding Failed!", LOG_LEVEL_1);			
				}
				
				eEncodeDLLRetCode = m_pEncodingComDll->RestoreCOM();
				eEncodeRetCode = ENCODE_FAIL; 
			}
		}
	}
	else	//4.0.20.506
		WriteEncodingLogMsg(lPumpNumber, lTrsNumber, "EncodingComDll not init! Encoding Failed!", LOG_LEVEL_1);	

	if (eEncodeDLLRetCode!=ENCODE_DLL_OK || eEncodeRetCode!=ENCODE_OK)
		psEncodedBuffOut[0] = '\0';

	return eEncodeRetCode;
}



//////////////////////////////////////////////////////////////////////
//					Save Encoding Data Methods
//////////////////////////////////////////////////////////////////////
//protected:
void CBaseEncodingAlg::SaveEncryptedData(CARD_SALE_ALL3* pcCardSaleAll3Out, 
										 const char* psEncryptedData, const size_t iEncryptedDataSize, const long lTrsNumber)
{
	memcpy(pcCardSaleAll3Out->extData4.sEncryptedAccount2, psEncryptedData, min(iEncryptedDataSize, sizeof(pcCardSaleAll3Out->extData4.sEncryptedAccount2)));
}


//protected:
void CBaseEncodingAlg::SaveHashedData(CARD_SALE_ALL3* pcCardSaleAll3Out, const char* psHashedData, const size_t iHashedDataSize)
{
	memcpy(pcCardSaleAll3Out->extData4.sEncryptedAccount, psHashedData, min(iHashedDataSize, sizeof(pcCardSaleAll3Out->extData4.sEncryptedAccount)));
}


void CBaseEncodingAlg::SaveHashedData(CARD_SALE_ALL3* pcCardSaleAll3Out, const long lData, const size_t iHashedDataSize)
{
	BYTE* psData = NULL;
	psData = new BYTE[iHashedDataSize];

	l2a(lData, psData, iHashedDataSize);

	memcpy(pcCardSaleAll3Out->extData4.sEncryptedAccount, (char*)psData, min(iHashedDataSize, sizeof(pcCardSaleAll3Out->extData4.sEncryptedAccount)));

	delete [] psData;
}


//protected:
void CBaseEncodingAlg::SaveEncryptedData2(CARD_SALE_ALL3* pcCardSaleAll3Out, 
										 const char* psEncryptedData, const size_t iEncryptedDataSize, const long lTrsNumber)
{
	memcpy(pcCardSaleAll3Out->extData5.sEncryptedAccount3, psEncryptedData, min(iEncryptedDataSize, sizeof(pcCardSaleAll3Out->extData5.sEncryptedAccount3)));
}


inline long	CBaseEncodingAlg::GetPumpNumber(const CARD_SALE_ALL3* pcCardSaleAll3In)const
{
	return a2l((BYTE*)pcCardSaleAll3In->CardSaleAll.data.sPumpNumber, sizeof(pcCardSaleAll3In->CardSaleAll.data.sPumpNumber));
}


//////////////////////////////////////////////////////////////////////
//					Log Message Methods
//////////////////////////////////////////////////////////////////////
//protected:
void CBaseEncodingAlg::WriteEncodingLogMsg(const long lPumpNumber, const long lTrsNumber, const CString& strLogMsg, const long lLogLevel ) const
{
	CString strTrsLogMsgPostfix;

	strTrsLogMsgPostfix.Format(", TrsNum=%ld", lTrsNumber);

	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, m_strLogMsgPrefix + strLogMsg + strTrsLogMsgPostfix, lLogLevel);
}


//protected:
void CBaseEncodingAlg::WriteEncodingLogMsg(const long lPumpNumber, const CString& strLogMsg, const long lLogLevel)const
{
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, m_strLogMsgPrefix + strLogMsg, lLogLevel);
}


//protected:
void CBaseEncodingAlg::WriteEncodingLogMsg(const CString& strLogMsg, const long lLogLevel)const
{
	_LOGMSG.LogMsg(m_strLogMsgPrefix + strLogMsg, lLogLevel);
}



//////////////////////////////////////////////////////////////////////
//					Finalization Methods 
//////////////////////////////////////////////////////////////////////

void CBaseEncodingAlg::CleanUp()
{
	if(m_cDSSInitThread.IsDSSInitThreadActive())
		m_cDSSInitThread.KillDSSInitThread();

	if (NULL != m_pEncodingComDll)
		m_pEncodingComDll->CleanUp();

	memset(m_szCertificate, 0x00, sizeof(m_szCertificate)); //4.0.22.90
}


//--------------------------------------------------------------------
//					CDSSBaseEncodingAlg class
//--------------------------------------------------------------------

CDSSBaseEncodingAlg::CDSSBaseEncodingAlg(const CString& strLogMsgPrefix) 
										
{
	m_strLogMsgPrefix = strLogMsgPrefix;
	m_pEncodingComDll = NULL;
	m_dwCurrentInitState = FESS_ALLOWED_ALL;
	m_bHashEnabled = TRUE;
	m_bLookForInternalReceiptIndex = FALSE;

	memset(m_szCertificate, 0x00, sizeof(m_szCertificate));			
}


CDSSBaseEncodingAlg::CDSSBaseEncodingAlg() : CBaseEncodingAlg("DSSEncodingAlgorithm, ", CLSID_Cryptography)
{
	
}


CDSSBaseEncodingAlg::~CDSSBaseEncodingAlg()
{

}


//////////////////////////////////////////////////////////////////////
//					Initialization Methods							
//////////////////////////////////////////////////////////////////////

EncodeComDllRetCodeEnum	CDSSBaseEncodingAlg::Init(BOOL bLoadConfig)
{
    EncodeComDllRetCodeEnum eEncodeDLLRetCode = ENCODE_DLL_OK;
	
	if(TRUE == bLoadConfig)
		LoadConfiguration();

	eEncodeDLLRetCode = m_pEncodingComDll->Init();

	if(eEncodeDLLRetCode == ENCODE_DLL_OK)
	{
		long		lEncodeDLLErrorCode = ENCODE_DLL_OK;
		long		lRetCode = OK;
		
		char        sTemp[MAX_TABLE_NAME];            
		memset(sTemp, ' ', sizeof(sTemp));								//4.0.22.90
	
		CComBSTR	cwsCertificate(sizeof(sTemp),	(LPCSTR)sTemp);
		CComBSTR	cwsErrorMessage(sizeof(sTemp),	(LPCSTR)sTemp);
		
		char		szCertificate[MAX_FIELD_VAL] = {0};
		char		szErrorMessage[MAX_FIELD_VAL] = {0};

		try
		{
			lRetCode = CEncodingComDll<ICryptography>::PtrInstance()->InitializeSession("{455634A2-1A48-11D1-B8A5-0000E8D0CBF8}",	/*App ID*/
 																						"PumpSrv.exe",								/*User name*/
 																						"aRTis400%",								/*Password*/
																						&cwsCertificate.m_str,						/*Certificate OUT*/
																						&lEncodeDLLErrorCode,
																						&cwsErrorMessage.m_str);

			int iLen = Bstr2Buffer(cwsCertificate, szCertificate, sizeof(szCertificate) - 1);
			
			if(iLen == 0)
				memset(szCertificate, 0x00, sizeof(szCertificate));

			iLen = Bstr2Buffer(cwsErrorMessage, szErrorMessage, sizeof(szErrorMessage) - 1);
			
			if(iLen == 0)
				memset(szErrorMessage, 0x00, sizeof(szErrorMessage));

			eEncodeDLLRetCode = InitResponse(lRetCode, szCertificate, lEncodeDLLErrorCode, szErrorMessage);
		}
		catch (...)
		{
			DWORD dwLastError = GetLastError();

			CString strLogMsg;
			strLogMsg.Format("Initialize Failed!!! Error=%ld", dwLastError);

			WriteEncodingLogMsg(strLogMsg, LOG_LEVEL_1);				
		}
	}

	return eEncodeDLLRetCode;
}


//implementation for CDSSEncryptAlg,CDSSHashAlg,CDSSBothAlg
//protected:
EncodeComDllRetCodeEnum	CDSSBaseEncodingAlg::InitResponse(	const long	lRetCode, 
															const char* szCertificate, 
															const long	lEncodeDLLErrorCode, 
															const char* szErrorMessage)
{
	EncodeComDllRetCodeEnum	eEncodeDLLRetCode = ENCODE_DLL_OK;
	CString					strLogMsg;
	long					lLogLevel = LOG_LEVEL_1;

 	if (lRetCode == OK)
	{
		if( lEncodeDLLErrorCode == ENCODE_DLL_OK )
		{
			const size_t iCertificateLen = strlen(szCertificate);

			if (iCertificateLen > 0)																
			{
				memset(m_szCertificate, 0x00,			sizeof(m_szCertificate));						//4.0.22.90
				memcpy(m_szCertificate, szCertificate,	min(sizeof(m_szCertificate), iCertificateLen));													

							
				m_dwCurrentInitState = (DWORD)FESS_PAP_ENABLED;
				SendSystemStateToCL();

				strLogMsg = "Initialize Succeed!"; 
				lLogLevel = DEFAULT_LOG_LEVEL;
			}
			else
			{
				DWORD dwLastError = GetLastError();

 				strLogMsg.Format("Initialize Failed!!! Certificate Len=%ld, Error=%ld", iCertificateLen, dwLastError);

				eEncodeDLLRetCode = ENCODE_DLL_FAIL_INITIALIZATON;
			}			
		}
		else if( lEncodeDLLErrorCode == -10 )
		{
			//Send CL event to remove the reprint from the terminal !
			m_bHashEnabled = FALSE;

			m_dwCurrentInitState = (DWORD) FESS_REPRINT_DISABLED;
			SendSystemStateToCL();
			
			strLogMsg = "Initialize, DISABLE Hashing!!!";
		}
	}
	else
	{
		eEncodeDLLRetCode = ENCODE_DLL_FAIL_INITIALIZATON;

		if (LOG_LOGIC_ERROR)				
		{
			strLogMsg.Format("Initialize Failed! RetCode=%ld, EncodeDLLErrorCode = %ld, ErrorMessage=[%s]", lRetCode, lEncodeDLLErrorCode, szErrorMessage);
		}
	}			
	
	WriteEncodingLogMsg(strLogMsg, lLogLevel);

	if(eEncodeDLLRetCode != ENCODE_DLL_OK)
	{		
		if(m_cDSSInitThread.IsDSSInitThreadActive() == FALSE)	
		{
			_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, DSS_INIT_FAIL, 0, ALARM_STARTED, 0, 0);

			// Set System State
			if(m_bDisablePAPOnDSSFail)
				m_dwCurrentInitState |= FESS_PAP_DISABLED;

			m_dwCurrentInitState |= FESS_REPRINT_DISABLED;

			SendSystemStateToCL();
				
			if(eEncodeDLLRetCode != -10)	
			{
				// Start re-attempt init
				m_cDSSInitThread.Init();
				m_cDSSInitThread.StartDSSInitThread();
			}
		}
	}

	return eEncodeDLLRetCode;
}


long CDSSBaseEncodingAlg::GetReceiptIndex(char* psIndexOut, const char* pszInfoIn)			//4.0.23.0 TD 76407
{
	long lRetIndexLen = 0;

	lRetIndexLen = Encode(psIndexOut, pszInfoIn);	

	if(LOG_BASIC_FLOW_CONTROL)
	{		
		CString strLogMsg;
		strLogMsg.Format("GetReceiptIndex, after encode ReceiptIndex=[%.100s]", psIndexOut);
		WriteEncodingLogMsg(strLogMsg);
	}

	return lRetIndexLen;
}


//4.0.23.20 TD 77536
long CDSSBaseEncodingAlg::GetReceiptIndex(char* psIndexOut, const BSTR wsTrack2In, const long lFlags, const long lPumpNumber)
{
	EncodeRetCodeEnum		eEncodeRetCode = ENCODE_OK;
	char					sEncodedField[ENCRYPTED_DATA_SIZE];
	char					szAccountToEncode[ENCRYPTED_DATA_SIZE];
	long					lRetIndexLen = 0;

	memset(sEncodedField, ' ', sizeof(sEncodedField));
	memset(szAccountToEncode, 0, sizeof(szAccountToEncode));

	if ( (lFlags & RECEIPT_BY_ACCOUNT_NUM) || (lFlags & RECEIPT_INDEX_IS_RECEPT_KEY) || 
			(lFlags & RECEIPT_UNRECOGNIZED_BILL) || (lFlags & RECEIPT_CAR_REGISTRATION_INDEX))
	{
		long  lRequiredSize = WideCharToMultiByte(CP_ACP, 0, wsTrack2In, -1, NULL, 0, NULL, NULL);
		lRetIndexLen = WideCharToMultiByte(CP_ACP, 0, wsTrack2In, -1, szAccountToEncode, lRequiredSize, NULL, NULL);
	}
	else
		lRetIndexLen = _Module.m_server.m_cCommonFunction.GetAccountNumByTrack2(szAccountToEncode, wsTrack2In, lFlags, lPumpNumber);	

	if(lRetIndexLen != 0)
	{
		if( !(lFlags & RECEIPT_INDEX_IS_RECEPT_KEY))	//4.0.21.500 TD 70106
		{
			ChangeSpaceCharToNull((BYTE*)szAccountToEncode, sizeof(szAccountToEncode));	//4.0.22.41 TD 74458
			eEncodeRetCode = (EncodeRetCodeEnum)GetReceiptIndex(sEncodedField, szAccountToEncode);
		}
		else	
		{
			memcpy(sEncodedField, szAccountToEncode, lRetIndexLen);
			
			CString sTmp;	
			sTmp.Format("GetReceiptIndex, Copy receipt index without Encoding! [%.100s]", sEncodedField);
			WriteEncodingLogMsg(lPumpNumber, sTmp);			
		}

		if (eEncodeRetCode == ENCODE_OK)
		{
			lRetIndexLen = sizeof(sEncodedField);
			memcpy(psIndexOut, sEncodedField, lRetIndexLen);
		}
		else
			lRetIndexLen = 0;
	}
	else
	{
		WriteEncodingLogMsg(lPumpNumber, "GetReceiptIndex, GetAccountNum Falied", LOG_LEVEL_1);
	}	
	
	return lRetIndexLen;
}


EncodeComDllRetCodeEnum	 CDSSBaseEncodingAlg::CloseSession()
{
	EncodeComDllRetCodeEnum eEncodeDLLRetCode = ENCODE_DLL_OK;

	if (CEncodingComDll<ICryptography>::PtrInstance() != NULL)
	{	
		try
		{
			long		lEncodeDLLErrorCode = ENCODE_DLL_OK;
			long		lRetCode = OK;

			char        sTemp[MAX_TABLE_NAME];            
			memset(sTemp, ' ', sizeof(sTemp));								//4.0.22.90
		
			_bstr_t		bstrCertificate(m_szCertificate);
			CComBSTR	cwsErrorMessage(sizeof(sTemp), (LPCSTR)sTemp);

			lRetCode = CEncodingComDll<ICryptography>::PtrInstance()->CloseSession(	bstrCertificate,						/*Certificate OUT*/
																					&lEncodeDLLErrorCode,
																					&cwsErrorMessage.m_str);

			CString strLogMsg;
			long	lLogLevel = DEFAULT_LOG_LEVEL;

			if (OK != lRetCode)
			{	
				char		szErrorMessage[MAX_FIELD_VAL] = {0};

				int iLen = Bstr2Buffer(cwsErrorMessage, szErrorMessage, sizeof(szErrorMessage) - 1); //4.0.22.90

				strLogMsg.Format("CloseSession Failed!!! GeneralRetCode=%ld, ExtendedErrorCode=%ld, ErrorMessage=[%s]", lRetCode, lEncodeDLLErrorCode, szErrorMessage);
				
				lLogLevel = LOG_LEVEL_1;
				
				eEncodeDLLRetCode = ENCODE_DLL_CLOSE_SESSION_FAIL;
			}
			else
			{		
				strLogMsg.Format("CloseSession Succeed!");
			}

			WriteEncodingLogMsg(strLogMsg, lLogLevel);
		}
		catch (...)
		{
			DWORD dwLastError = GetLastError();

			CString strLogMsg;
			strLogMsg.Format("CloseSession Failed!!! Error=%ld", dwLastError);

			WriteEncodingLogMsg(strLogMsg, LOG_LEVEL_1);				

			eEncodeDLLRetCode = ENCODE_DLL_CLOSE_SESSION_FAIL;
		}
	}
	else
		eEncodeDLLRetCode = ENCODE_DLL_NOT_CONNECTED;

	return eEncodeDLLRetCode;
}


//--------------------------------------------------------------------
//					CDPSBaseEncodingAlg class
//--------------------------------------------------------------------


CDPSBaseEncodingAlg::CDPSBaseEncodingAlg() : CBaseEncodingAlg("DPSEncodingAlgorithm, ", __uuidof(COMInt))
{
	memset(m_szConfigFileName, 0x00, sizeof(m_szConfigFileName));
}


CDPSBaseEncodingAlg::~CDPSBaseEncodingAlg()
{

}


inline BOOL CDPSBaseEncodingAlg::IsConfigFileNameCorrupted()
{
	BOOL bRetCode = (strlen(m_szConfigFileName) < MIN_PASS_SIZE) ? TRUE : FALSE;

	return bRetCode;
}


//////////////////////////////////////////////////////////////////////
//					Initialization Methods							
//////////////////////////////////////////////////////////////////////


void CDPSBaseEncodingAlg::LoadConfiguration(const BOOL bCreate)
{
	CBaseEncodingAlg::LoadConfiguration(bCreate);

	memset(m_szConfigFileName, 0x00, sizeof(m_szConfigFileName));

	_Module.m_server.m_cParam.LoadParam("Paths\\Long", "DPSConfigFile", m_szConfigFileName, sizeof(m_szConfigFileName), " ", bCreate);	

	if (IsConfigFileNameCorrupted() == TRUE)
	{
		WriteEncodingLogMsg("DPSConfigFile is corrupted!", LOG_LEVEL_1);
		memset(m_szConfigFileName, 0x00, sizeof(m_szConfigFileName));
	}	
}


EncodeComDllRetCodeEnum	CDPSBaseEncodingAlg::Init(BOOL bLoadConfig)
{
    EncodeComDllRetCodeEnum eEncodeDLLRetCode	= ENCODE_DLL_OK;
	long					lEncodeDLLErrorCode = ENCODE_DLL_UNKNOWN_ERROR;
	long					lRetCode = -1;
	char					szErrorMessage[MAX_FIELD_VAL] = {0};
	char					szCertificate[MAX_FIELD_VAL] = {0};
	
	if(TRUE == bLoadConfig)
		LoadConfiguration();

	if (IsConfigFileNameCorrupted() == TRUE)
	{
		eEncodeDLLRetCode = ENCODE_DLL_CUSTOMIZE_ERROR;
	}

	if(eEncodeDLLRetCode == ENCODE_DLL_OK)
	{
		if(m_pEncodingComDll->IsPtrNULL() == TRUE)
			eEncodeDLLRetCode = m_pEncodingComDll->Init();

		if( ENCODE_DLL_OK == eEncodeDLLRetCode )
		{		
			try
			{				
				_bstr_t		bstrConfigFileName(m_szConfigFileName);
				char        sTemp[MAX_TABLE_NAME] = {' '};            
				CComBSTR	cwsErrorMessage(sizeof(sTemp),	(LPCSTR)sTemp);
				
				CEncodingComDll<_COMInt>::PtrInstance()->InitializeAPI(	bstrConfigFileName,
																		&lEncodeDLLErrorCode, 
																		&cwsErrorMessage.m_str, 
																		&lRetCode);
				if( OK != lRetCode )
				{
					if (LOG_LOGIC_ERROR)				
					{
						int iLen = Bstr2Buffer(cwsErrorMessage, szErrorMessage, sizeof(szErrorMessage) - 1);
			
						if(iLen == 0)
							memset(szErrorMessage, 0x00, sizeof(szErrorMessage));

						CString strErrMsg;
						strErrMsg.Format("InitializeAPI Failed! RetCode=%ld, EncodeDLLErrorCode = %ld, ErrorMessage=[%s]", lRetCode, lEncodeDLLErrorCode, szErrorMessage);
						
						WriteEncodingLogMsg(strErrMsg, LOG_LEVEL_1);
					}
				}
				else
				{				
					WriteEncodingLogMsg("InitializeAPI Succeed!");

					/*CComBSTR	cwsAppID("{455634A2-1A48-11D1-B8A5-0000E8D0CBF8}");
					CComBSTR	cwsUserName("PumpSrv.exe");
					CComBSTR	cwsPassword("aRTis400%");	*/
					CComBSTR	cwsCertificate(sizeof(m_szCertificate),(LPCSTR)m_szCertificate);

					_bstr_t		bstrAppID("RetalixApplicationTest");				//Test to be rem! 4.0.23.20
					_bstr_t		bstrEmptyForTest(" ");								//Test to be rem! 4.0.23.20

					lEncodeDLLErrorCode = lRetCode = OK;

					cwsErrorMessage.Empty();
					cwsErrorMessage = "  ";

					CEncodingComDll<_COMInt>::PtrInstance()->InitializeSession(	bstrAppID,								/*App ID*/
 																				bstrEmptyForTest,							/*User name*/
 																				bstrEmptyForTest,							/*Password*/
																				&cwsCertificate.m_str,						/*Certificate OUT*/
																				&lEncodeDLLErrorCode,
																				&cwsErrorMessage.m_str,
																				&lRetCode );									

					int iLen = Bstr2Buffer(cwsCertificate, szCertificate, sizeof(szCertificate) - 1);
			
					if(iLen == 0)
						memset(szCertificate, 0x00, sizeof(szCertificate));

					iLen = Bstr2Buffer(cwsErrorMessage, szErrorMessage, sizeof(szErrorMessage) - 1);
			
					if(iLen == 0)
						memset(szErrorMessage, 0x00, sizeof(szErrorMessage));
				}
			}
			catch (...)
			{
				lEncodeDLLErrorCode = lRetCode = ENCODE_FAIL;
				memset(szErrorMessage, 0x00, sizeof(szErrorMessage));
				memset(szCertificate, 0x00, sizeof(szCertificate));

				DWORD dwLastError = GetLastError();

				CString strLogMsg;
				strLogMsg.Format("Initialize Failed!!! Error=%ld", dwLastError);

				WriteEncodingLogMsg(strLogMsg, LOG_LEVEL_1);				
			}
		}
	}
	else
	{
		CString strLogMsg;
		strLogMsg.Format("Initialize Canceled!!! Error=%ld", eEncodeDLLRetCode);

		WriteEncodingLogMsg(strLogMsg, LOG_LEVEL_1);	
	}

	eEncodeDLLRetCode = InitResponse(lRetCode, szCertificate, lEncodeDLLErrorCode, szErrorMessage);		//4.0.23.508 TD 93615

	return eEncodeDLLRetCode;
}


//implementation for CDSSEncryptAlg,CDSSHashAlg,CDSSBothAlg
//protected:
EncodeComDllRetCodeEnum	CDPSBaseEncodingAlg::InitResponse(	const long	lRetCode, 
															const char* szCertificate, 
															const long	lEncodeDLLErrorCode, 
															const char* szErrorMessage)
{
	EncodeComDllRetCodeEnum	eEncodeDLLRetCode = ENCODE_DLL_OK;
	CString					strLogMsg;
	long					lLogLevel = LOG_LEVEL_1;

 	if (lRetCode == OK && lEncodeDLLErrorCode == ENCODE_DLL_OK)
	{
		const size_t iCertificateLen = strlen(szCertificate);

		if (iCertificateLen > 0)																
		{
			memset(m_szCertificate, 0x00,			sizeof(m_szCertificate));						//4.0.22.90
			memcpy(m_szCertificate, szCertificate,	min(sizeof(m_szCertificate), iCertificateLen));	
			
			m_dwCurrentInitState = (DWORD)FESS_PAP_ENABLED;

			strLogMsg = "Initialize Succeed!";
			lLogLevel = DEFAULT_LOG_LEVEL;
		}
		else
		{
			DWORD dwLastError = GetLastError();
			
 			strLogMsg.Format("Initialize Failed!!! Certificate Len=%ld, Error=%ld", iCertificateLen, dwLastError);

			eEncodeDLLRetCode = ENCODE_DLL_FAIL_INITIALIZATON;
		}
	}
	else
	{
		eEncodeDLLRetCode = ENCODE_DLL_FAIL_INITIALIZATON;
		
		if (LOG_LOGIC_ERROR)				
		{
			strLogMsg.Format("Initialize Failed! RetCode=%ld, EncodeDLLErrorCode = %ld, ErrorMessage=[%s]", lRetCode, lEncodeDLLErrorCode, szErrorMessage);
		}
	}			
	
	WriteEncodingLogMsg(strLogMsg, lLogLevel);

	if(eEncodeDLLRetCode != ENCODE_DLL_OK)
	{		
		SendInitializationFailedAlert(lEncodeDLLErrorCode, szErrorMessage);

		// Set System State
		if(m_bDisablePAPOnDSSFail)
			m_dwCurrentInitState |= FESS_PAP_DISABLED;		
	}

	return eEncodeDLLRetCode;
}



long CDPSBaseEncodingAlg::GetReceiptIndex(char* psIndexOut, const char* pszInfoIn)			//4.0.23.0 TD 76407
{
	long lRetIndexLen = 0;

	lRetIndexLen = ( MaskAccount(psIndexOut, pszInfoIn) == TRUE ) ? OK : -1;	

	if(LOG_BASIC_FLOW_CONTROL)
	{		
		CString strLogMsg;
		strLogMsg.Format("GetReceiptIndex, after MaskAccount ReceiptIndex=[%.100s]", psIndexOut);
		WriteEncodingLogMsg(strLogMsg);
	}

	return lRetIndexLen;
}


//4.0.23.20 TD 77627
long CDPSBaseEncodingAlg::GetReceiptIndex(char* psIndexOut, const PAY_AT_PUMP_INFO* pcPayAtPumpInfoIn, const long lPumpNumber) const
{
	long lRetIndexLen = 0;

	if(pcPayAtPumpInfoIn != NULL)	
	{	
		const size_t		iReceiptKeySize = sizeof(pcPayAtPumpInfoIn->CardSaleExtraData4.sEncryptedAccount);
		
		if (IsFieldNotEmpty((char*)pcPayAtPumpInfoIn->CardSaleExtraData4.sEncryptedAccount, iReceiptKeySize) == FALSE)
		{
			const size_t	iAccountNumSize = sizeof(pcPayAtPumpInfoIn->CardSaleInfo.cardData.sAccountNumber);
			char			szAccountNum[iAccountNumSize + 1] = {0};

			memcpy(szAccountNum, (char*)pcPayAtPumpInfoIn->CardSaleInfo.cardData.sAccountNumber, iAccountNumSize);
			lRetIndexLen = ( MaskAccount(psIndexOut, szAccountNum) == TRUE ) ? iAccountNumSize : -1; //4.0.29.500 - TD 276913
		}
		else
		{
			//return the key of the receipt qdx
			lRetIndexLen = iReceiptKeySize;
			memcpy(psIndexOut, pcPayAtPumpInfoIn->CardSaleExtraData4.sEncryptedAccount, lRetIndexLen);

			CString		strLogMsg;
			strLogMsg.Format("GetReceiptIndex, Get receipt index from sEncryptedAccount=[%.100s], RetIndexLen=%ld", psIndexOut, lRetIndexLen);
			WriteEncodingLogMsg(lPumpNumber, strLogMsg);
		}
	}
	
	return lRetIndexLen;
}


//4.0.23.20 TD 77536
long CDPSBaseEncodingAlg::GetReceiptIndex(char* psIndexOut, const BSTR wsTrack2In, const long lFlags, const long lPumpNumber)
{
	EncodeRetCodeEnum		eEncodeRetCode = ENCODE_OK;
	char					sEncodedField[ENCRYPTED_DATA_SIZE];
	char					szAccountToEncode[ENCRYPTED_DATA_SIZE];
	long					lRetIndexLen = 0;

	memset(sEncodedField, ' ', sizeof(sEncodedField));
	memset(szAccountToEncode, 0, sizeof(szAccountToEncode));

	if ( (lFlags & RECEIPT_BY_ACCOUNT_NUM) || (lFlags & RECEIPT_INDEX_IS_RECEPT_KEY) || 
			(lFlags & RECEIPT_UNRECOGNIZED_BILL) || (lFlags & RECEIPT_CAR_REGISTRATION_INDEX) || (lFlags & RECEIPT_INDEX_IS_TRS))
	{
		long  lRequiredSize = WideCharToMultiByte(CP_ACP, 0, wsTrack2In, -1, NULL, 0, NULL, NULL);
		lRetIndexLen = WideCharToMultiByte(CP_ACP, 0, wsTrack2In, -1, szAccountToEncode, lRequiredSize, NULL, NULL);
	}
	else
		lRetIndexLen = _Module.m_server.m_cCommonFunction.GetAccountNumByTrack2(szAccountToEncode, wsTrack2In, lFlags, lPumpNumber);	

	if(lRetIndexLen != 0)
	{
		if( !(lFlags & RECEIPT_INDEX_IS_RECEPT_KEY) )	
		{
			ChangeSpaceCharToNull((BYTE*)szAccountToEncode, sizeof(szAccountToEncode));	
			eEncodeRetCode = (EncodeRetCodeEnum)GetReceiptIndex(sEncodedField, szAccountToEncode);
		}
		else	
		{
			memcpy(sEncodedField, szAccountToEncode, lRetIndexLen);
			
			CString sTmp;	
			sTmp.Format("GetReceiptIndex, Copy receipt index without Encoding! [%.100s]", sEncodedField);
			WriteEncodingLogMsg(lPumpNumber, sTmp);			
		}

		if (eEncodeRetCode == ENCODE_OK)
		{
			lRetIndexLen = sizeof(sEncodedField);
			memcpy(psIndexOut, sEncodedField, lRetIndexLen);
		}
		else
			lRetIndexLen = 0;
	}
	else
	{
		WriteEncodingLogMsg(lPumpNumber, "GetReceiptIndex, GetAccountNumByTrack2 Falied", LOG_LEVEL_1);
	}	
	
	return lRetIndexLen;
}


EncodeComDllRetCodeEnum	 CDPSBaseEncodingAlg::CloseSession()
{
	EncodeComDllRetCodeEnum eEncodeDLLRetCode = ENCODE_DLL_OK;

	if (m_pEncodingComDll->IsPtrNULL() == FALSE)
	{	
		try
		{
			long		lEncodeDLLErrorCode = ENCODE_DLL_OK;
			long		lRetCode = OK;

			char        sTemp[MAX_TABLE_NAME];            
			memset(sTemp, ' ', sizeof(sTemp));								//4.0.22.90
		
			_bstr_t		bstrCertificate(m_szCertificate);
			CComBSTR	cwsErrorMessage(sizeof(sTemp),	(LPCSTR)sTemp);

			CEncodingComDll<_COMInt>::PtrInstance()->CloseSession(	bstrCertificate,						/*Certificate IN*/
																	&lEncodeDLLErrorCode,
																	&cwsErrorMessage.m_str,
																	&lRetCode);
		
			CString strLogMsg;
			long	lLogLevel = DEFAULT_LOG_LEVEL;

			if (OK != lRetCode)
			{
				char		szErrorMessage[MAX_FIELD_VAL] = {0};

				int iLen = Bstr2Buffer(cwsErrorMessage, szErrorMessage, sizeof(szErrorMessage) - 1);
			
				strLogMsg.Format("CloseSession Failed!!! GeneralRetCode=%ld, ExtendedErrorCode=%ld, ErrorMessage=[%s]", lRetCode, lEncodeDLLErrorCode, szErrorMessage);
				lLogLevel = LOG_LEVEL_1;
				
				eEncodeDLLRetCode = ENCODE_DLL_CLOSE_SESSION_FAIL;
			}
			else
			{		
				strLogMsg.Format("CloseSession Succeed!");
			}

			WriteEncodingLogMsg(strLogMsg, lLogLevel);
		}
		catch (...)
		{
			DWORD dwLastError = GetLastError();

			CString strLogMsg;
			strLogMsg.Format("CloseSession Failed!!! Error=%ld", dwLastError);

			WriteEncodingLogMsg(strLogMsg, LOG_LEVEL_1);				

			eEncodeDLLRetCode = ENCODE_DLL_CLOSE_SESSION_FAIL;
		}
	}
	else
		eEncodeDLLRetCode = ENCODE_DLL_NOT_CONNECTED;

	return eEncodeDLLRetCode;
}