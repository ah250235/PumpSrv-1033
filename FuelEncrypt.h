// FuelEncrypt.h: interface for the CFuelEncrypt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUELENCRYPT_H__4208C588_6728_4D52_BFC5_8B596EA7741A__INCLUDED_)
#define AFX_FUELENCRYPT_H__4208C588_6728_4D52_BFC5_8B596EA7741A__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFAULT_BLOCK_SIZE 16

#define MIN_PASS_SIZE   5
#define EVEN_BIT_MASK 0xAA // 10101010
#define ODD_BIT_MASK 0x55  // 01010101


enum EncryptErrors{
		ENCRYPT_OK,
		ENCRYPT_FAIL,
		ENCRYPT_NO_TRACK2,
		ENCRYPT_BUFF_TOO_SMALL,
		ENCRYPT_STRING_NOT_FOUND,   //4.0.5.13
};

class CFuelEncrypt  
{
public:
	long EncryptReceiptAccount(char * sAccountNumber,char * bTrack2 ,  bool bMask = false);
	BOOL Track2ToAccount(char *sTrack2);
	long RemoveCardInfoFromMessage(char *sIn, long lInSize, char *sOut,bool bMaskAccount=false); //4.0.6190.3001
	long ReplaceOLAString(char *pszOLAString, char *pszInBuff, char *pszOutBuff, long* plOutBuffLen, bool bMaskAccount=false); //4.0.6190.3000
	long ReplaceBuffer(const CStringA & sBufferData, CStringA & sEncryptedData);
	long MaskBuffer(char* pszBuffToMask, long lNumberOfCharsFromStart, long lNumberOfCharsFromEnd);
	long ReplaceTrack2(char* pszInBuff, char* pszOutBuff, long* plOutBuffLen);
	long Decrypt(char *pszOpenBuff, char *pszEncryBuff);
	long Encrypt(char* pszOpenBuff, char* pszEncryBuff);
	long Encrypt(const CString & sSrcBuff, CStringA & sEncryptBuff);
	long EncryptAccount(CStringA & sAccount, CStringA & sEncryptionKey, bool bMask = false);//4.0.9.503 //4.0.6190.3000
	long EncryptAccountDLL(char *sBufferChangedata, long lBufferChangedataSize, char *pszOutBuff, long lMaxOutBuffSize, long *plOutBuffLen);//4.0.9.503
	void DecryptAccountDLL(char *sBufferChangedata, char *pszOutBuff);//4.0.9.503
	void ZeroBuff(char * sInBuf,char * sOutBuf);//4.0.10.132


	CFuelEncrypt();
	virtual ~CFuelEncrypt();

private:
	long		YEncode(unsigned char *open_string, unsigned char *encrypted_string);
	long		YEncode(const CString & open_string, CString & encrypted_string);
	long		YDecode(unsigned char *open_string,unsigned char *encripted_string);
	void		Encrypt(unsigned char *c,unsigned char *c2);
	void		Decrypt(unsigned char *c,unsigned char c2);
	void		pak(unsigned char * u_str,unsigned char * p_str,int n);
	void		unpak(unsigned char *u_str, unsigned char *p_str, int n);
	void		unpak(CStringA & u_str, unsigned char *p_str, int n);
};

#endif // !defined(AFX_FUELENCRYPT_H__4208C588_6728_4D52_BFC5_8B596EA7741A__INCLUDED_)
