// OLAEncryptHandler.cpp: implementation of the COLAHashAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLAHashAlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLAHashAlg::COLAHashAlg() : CDSSHashAlg("OLAHashAlg, ")
{

}

COLAHashAlg::~COLAHashAlg()
{

}


/******************************************************************************
 Description:	Initialize the OLAEncryptHandler
 Returns:      	ENCRYPT_DLL_OK - OK
				
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			22/03/2011	 16:04		Start 4.0.27.44 TD 179125
******************************************************************************/

EncodeComDllRetCodeEnum	COLAHashAlg::Init(BOOL bLoadConfig)
{
	m_dwCurrentInitState = FESS_PAP_ENABLED;
	SendSystemStateToCL();

	WriteEncodingLogMsg("Initialize Succeed!"); 
	
	return ENCODE_DLL_OK;
}



//////////////////////////////////////////////////////////////////////
//					Encoding\Decoding Methods
//////////////////////////////////////////////////////////////////////
//protected:
EncodeRetCodeEnum COLAHashAlg::Encode(char* psEncodedBuffOut, const char* pszOpenBuffIn, const long lPumpNumber, 
										   size_t* piEncryptedDataSize, size_t* piHashedDataSize, long lTrsNumber)
{
	EncodeComDllRetCodeEnum		eEncodeDLLRetCode = ENCODE_DLL_OK;
	EncodeRetCodeEnum			eEncodeRetCode = ENCODE_FAIL;
	long						lErrorCode = OK;

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
					strLogMsg.Format("Hashing failed! RetCode=%ld.", eEncodeRetCode);							
					
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
			
			eEncodeRetCode = ENCODE_FAIL; 
		}
	}

	if (eEncodeDLLRetCode!=ENCODE_DLL_OK || eEncodeRetCode!=ENCODE_OK)
		psEncodedBuffOut[0] = '\0';

	return eEncodeRetCode;
}

//////////////////////////////////////////////////////////////////////
//					DLL Entry Point to Hash Method
//////////////////////////////////////////////////////////////////////

/******************************************************************************
 Description:	Hashing
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			22/03/2011	 16:24		Start 4.0.27.44  merge from 1021 TD 179125
******************************************************************************/

//protected:
EncodeRetCodeEnum COLAHashAlg::EncodeDLLEntryPoint(const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
													size_t* piEncryptedDataSize, size_t* piHashedDataSize, long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_FAIL;

	eEncodeRetCode = (EncodeRetCodeEnum)_Module.m_server.m_cOLASrv->GetHashedPAN(lTrsNumber, pszDataToEncode, psEncodedData, piHashedDataSize);

	if(eEncodeRetCode != ENCODE_OK)
	{
		*plErrorCode = eEncodeRetCode;

		eEncodeRetCode = ENCODE_HASH_FAIL;
		SendHashingFailedAlert(*plErrorCode, szErrorMessage, lTrsNumber);
	}
	else
	{
		CString strLogMsg;
		strLogMsg.Format("Hashing successed, retBuf:[%.100s]", psEncodedData);
		WriteEncodingLogMsg(0, lTrsNumber, strLogMsg);
	}

	return eEncodeRetCode;
}

