// OLAEncryptHandler.h: interface for the COLAHashAlg class.
//			Gena 22/03/2011 16:00
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLAENCRYPTHANDLER_H__427588D8_CC73_4A7A_8214_34AFF2113C0C__INCLUDED_)
#define AFX_OLAENCRYPTHANDLER_H__427588D8_CC73_4A7A_8214_34AFF2113C0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSSHashAlg.h"

class COLAHashAlg : public CDSSHashAlg
{
public:
	COLAHashAlg();
	virtual ~COLAHashAlg();

	virtual EncodeComDllRetCodeEnum		Init(BOOL bLoadConfig = TRUE);

protected:
	// General Encode Methods
	virtual EncodeRetCodeEnum	Encode(	char* psEncodedBuffOut, const char* pszOpenBuffIn, const long lPumpNumber = 0, 
										size_t* piEncryptedDataSize = NULL, size_t* piHashedDataSize = NULL, long lTrsNumber = 0);
	
	// Protected Encryption/Decryption Methods
	EncodeRetCodeEnum	EncodeDLLEntryPoint(const char* pszDataToEncode, char* psEncodedData, LONG* plErrorCode, char* szErrorMessage,
											size_t* piEncryptedDataSize, size_t* piHashedDataSize, long lTrsNumber);
};

#endif // !defined(AFX_OLAENCRYPTHANDLER_H__427588D8_CC73_4A7A_8214_34AFF2113C0C__INCLUDED_)
