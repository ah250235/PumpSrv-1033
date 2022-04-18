// FuelEncrypt.cpp: implementation of the CFuelEncrypt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FuelEncrypt.h"
#include "ExeModule.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFuelEncrypt::CFuelEncrypt()
{
	
}

CFuelEncrypt::~CFuelEncrypt()
{

}

/****************************************************************************************************
 Description:	Masked a string with '*' 	
 Returns:      	long - ENCRYPT_OK or ENCRYPT_BUFF_TOO_SMALL if parameters are bigger then buffer len.
 Parameters:   	char *pszBuffToMask			 - Buffer to mask.
				long lNumberOfCharsFromStart - Number of chars to mask from buffer beggin.
				long lNumberOfCharsFromEnd	 - Number of chars to mask from buffer end.
 
 WHO			WHEN                    WHAT
------------------------------------------------------------------------------------------------------
 Yaron			24/1/2002   10:55		Start
******************************************************************************/
long CFuelEncrypt::MaskBuffer(char *pszBuffToMask, long lNumberOfCharsFromStart, long lNumberOfCharsFromEnd)
{
	long lRetVal = ENCRYPT_OK;
	int i,j;
	long lBuffLen = strlen(pszBuffToMask);

	if( lBuffLen < lNumberOfCharsFromStart || lBuffLen < lNumberOfCharsFromEnd)
		lRetVal = ENCRYPT_BUFF_TOO_SMALL;
	else
	{
		for( i = 0 ; i < lNumberOfCharsFromStart ; i++ )
			pszBuffToMask[i] = '*';

		for( j = 1 ; j <= lNumberOfCharsFromEnd ; j++ )
			pszBuffToMask[lBuffLen-j] = '*';
	}

	return lRetVal;
}

// Function name	: CFuelEncrypt::Encrypt
// Description	    : Encrypt a given buffer.
// Return type		: long - ENCRYPT_OK=0,  ENCRYPT_FAIL=1
// Argument         : char *pszOpenBuff		- Data to encrypt.
// Argument         : char *pszEncryBuff	- Encrypt result.
long CFuelEncrypt::Encrypt(char *pszOpenBuff, char *pszEncryBuff)
{
	long lRetVal = ENCRYPT_OK;

	lRetVal = YEncode((unsigned char*)pszOpenBuff,(unsigned char*)pszEncryBuff);

	return lRetVal;
}

long CFuelEncrypt::Encrypt(const CString & sSrcBuff, CStringA & sEncryptBuff)
{
	long lRetVal = ENCRYPT_OK;

	lRetVal = YEncode(sSrcBuff, sEncryptBuff);

	return lRetVal;
}



// Function name	: CFuelEncrypt::Decrypt
// Description	    : Decrypt a given buffer
// Return type		: long - ENCRYPT_OK=0,  ENCRYPT_FAIL=1
// Argument         : char *pszOpenBuff		- Data to encrypt.
// Argument         : char *pszEncryBuff	- Encrypt result.
long CFuelEncrypt::Decrypt(char *pszOpenBuff, char *pszEncryBuff)
{
	long	lRetVal = ENCRYPT_OK;
	int		iSize = 0;
	
	lRetVal = YDecode((unsigned char*)pszOpenBuff,(unsigned char*)pszEncryBuff);

	return lRetVal;
}


// Function name	: CFuelEncrypt::ReplaceTrack2
// Description	    : Replaces the track2 with encrypted data.
// Return type		: long				 - ENCRYPT return codes.
// Argument         : char *pszInBuff	 - In buffer with original track2
// Argument         : char *pszOutBuff	 - Out buff with the replacment.
// Argument         : long* plOutBuffLen - In buffer len replaced by the out buffer len.
long CFuelEncrypt::ReplaceTrack2(char *pszInBuff, char *pszOutBuff, long* plOutBuffLen)
{
	long lRetVal = ENCRYPT_OK;
	char* szTmpBuff;
	char  szTrack2[40];
	char  szEncryptTrack2[150];
	long lBuffBegin = 0;
	long lCurrBuffLen = 0;
	long lEncryptTrack2Len = 0;
	long lEndOfTrack2Len = 0;
	long lEndOfBuff = 0;
	long lInBuffLen = strlen(pszInBuff);

	szTmpBuff = strstr(pszInBuff,"Track2");

	if(szTmpBuff)
	{
		char *strStartOfTrack2 = strstr(szTmpBuff,"=");
		char *strEndOfTrack2 = strstr(szTmpBuff,",");
		long lCardNumberLength = (long)strEndOfTrack2 - (long)strStartOfTrack2 - 1;
		memset(szTrack2,0,sizeof(szTrack2));
		memset(szEncryptTrack2,0,sizeof(szEncryptTrack2));
		memcpy(szTrack2,strStartOfTrack2+1,lCardNumberLength);

		if (1/*!_Module.m_server.m_cLogger.GetFlagsRemoveCardDataFromLog()*/)  //4.0.6.505
			lRetVal = Encrypt(szTrack2,szEncryptTrack2);
		else
		{
			lRetVal = ENCRYPT_OK, 
			sprintf(szEncryptTrack2, "%ld" ,lCardNumberLength);
		}

		if( lRetVal == ENCRYPT_OK)
		{
			lBuffBegin = (long)strStartOfTrack2 - (long)pszInBuff + 1;
			if(lBuffBegin > *plOutBuffLen)
				lRetVal = ENCRYPT_BUFF_TOO_SMALL;
			else
			{
				memcpy(pszOutBuff,pszInBuff,lBuffBegin);
				lCurrBuffLen += lBuffBegin;
				lEncryptTrack2Len = strlen(szEncryptTrack2);
				if(lCurrBuffLen + lEncryptTrack2Len > *plOutBuffLen)
					lRetVal = ENCRYPT_BUFF_TOO_SMALL;
				else
				{
					strcat(pszOutBuff,szEncryptTrack2);
					lCurrBuffLen += lEncryptTrack2Len;
					//copying the rest of the data
					lEndOfTrack2Len = strlen(strEndOfTrack2);
					lEndOfBuff = min(lEndOfTrack2Len ,(*plOutBuffLen-lCurrBuffLen))-1;
					if(lEndOfBuff + lCurrBuffLen == *plOutBuffLen)
						lEndOfBuff--;
					strncat(pszOutBuff,strEndOfTrack2,lEndOfBuff);
					lCurrBuffLen += lEndOfBuff;
					*plOutBuffLen = lCurrBuffLen;
				}
				
			}
		}
	}
	else
		lRetVal = ENCRYPT_NO_TRACK2;
		
	return lRetVal;
}


long CFuelEncrypt::YEncode(unsigned char *open_string,unsigned char *encrypted_string)
{
	unsigned long src_len;
	unsigned long index = 0, index2 = 0;
	unsigned char c = 0, c2 = 0, out = 0;
	unsigned char dest_str[1000];

	src_len = max (strlen((const char*)open_string),MIN_PASS_SIZE);

	if (src_len > sizeof(dest_str)/4)
	return ENCRYPT_FAIL; // len too big

	while (index < src_len )
	{
		c2 = 0;
		c= open_string[index++];
		Encrypt(&c, &c2);
		dest_str[index2++] = c ;
		dest_str[index2++] = c2 ;
	}

	unpak(encrypted_string, dest_str, index2 * 2);

	encrypted_string[ index2*2 ] = '\0';

	//   printf ("encoded to: %s \n",encripted_string);
	return(0); //OK
}

long CFuelEncrypt::YEncode(const CString & open_string, CString & encrypted_string)
{
	unsigned long src_len;
	unsigned long index = 0, index2 = 0;
	unsigned char c = 0, c2 = 0, out = 0;
	unsigned char dest_str[1000];

	src_len = max(open_string.GetLength(), MIN_PASS_SIZE);

	if (src_len > sizeof(dest_str) / 4)
		return ENCRYPT_FAIL; // len too big

	while (index < src_len)
	{
		c2 = 0;
		c = open_string[index++];
		Encrypt(&c, &c2);
		dest_str[index2++] = c;
		dest_str[index2++] = c2;
	}

	unpak(encrypted_string, dest_str, index2 * 2);


	//   printf ("encoded to: %s \n",encripted_string);
	return(0); //OK
}


long CFuelEncrypt::YDecode(unsigned char *open_string,unsigned char *encripted_string)
{
	unsigned long index = 0, index2 = 0;
	unsigned char c = 0, c2 = 0, out = 0;
	unsigned char pak_str[1000];

	unsigned long ascii_len = strlen((char*)encripted_string);

	if (ascii_len > 2*sizeof(pak_str))
	return ENCRYPT_FAIL; // len too big

	pak( encripted_string, pak_str, ascii_len);

	while (index < ascii_len / 2 )
	{
		c = pak_str[index++];
		c2 = pak_str[index++];
		Decrypt(&c, c2);
		open_string[index2++] = c ;
	}
	open_string[index2++] = '\0';
	//   printf ("decoded to: %s \n",open_string);
	return(0); //OK
}


// This function takes two parameters.  First, is the character to be
// encrypted.  The second parameter should be 0.  The original character
// is spread throughout the two parameters.  This function returns nothing.
// This function mixes the original character with a random character.
// So, the letter 'A' would be encrypted differently every time, making
// for hard decryption.  This method will keep MOST users from breaking
// into your data.  Example:

// Encrypt( &Original_Character,  &Second_Character);


void CFuelEncrypt::Encrypt(unsigned char *c,unsigned char *c2)
{
	int r = 0;  // Used for random number.

	int r_2 = 0;  // Second random number.

	r = rand() % 256;  // Chooses a random number between 0 and 256

	r_2 = rand() % 256;

	// Places the odd bits from *c into *c2 and the even bits from the random
	// number into *c2

	*c2  =  ( ( *c & ODD_BIT_MASK  ) | ( r & EVEN_BIT_MASK ) );

	// Places the odd bits from the random number in *c, and keeps the original
	// even bits.

	*c   =  ( ( *c & EVEN_BIT_MASK ) | ( r_2 & ODD_BIT_MASK  ) );
}


// This function takes two characters that were encrypted by encrypt() and
// takes the even bits from *c, and takes the odd bits from c2.  It stores
// the decrypted character in *c.  Both *c and c2 must be unsigned characters.
// The first parameter must be a pointer to a character, or an address of one.
// Decrypt returns nothing. Example:

// Decrypt( &First_character, Second_character);



void CFuelEncrypt::Decrypt(unsigned char *c,unsigned char c2)
{
	*c  =  (  ( *c & EVEN_BIT_MASK ) | ( c2 & ODD_BIT_MASK )  );
}

/*
	pak
	===
	pack buffer,add 7 to codes > 39H

*/

void CFuelEncrypt::pak(unsigned char * u_str,unsigned char * p_str,int n)
{
	short int i;
	char uh,ul;
	for (i = 0 ; i < n/2 ; i++)
	{
	   if ((uh=*u_str++) >= 0x41 )
		  uh -= 7;   /* convert letters to asci */
	   if ((ul=*u_str++) >= 0x41 )
		  ul -= 7;   /* convert letters to asci */
	   *p_str++ = (uh & 0xf) << 4 | ul & 0xf;
	}
}

/*
	unpak
	=====
	unpack buffer ,sub 7 from codes >41H

*/
void CFuelEncrypt::unpak(unsigned char *u_str,unsigned char *p_str,int n)
{
	short int i;
	for (i=0; i < n ;i++,u_str++)
	{
		*u_str = (i & 1) ? (*p_str++) & 0xf | 0x30
			: (*p_str >> 4 ) & 0xf | 0x30 ;
		if (*u_str >= 0x3a )
			*u_str  += 7;   /* convert letters to asci */
	}
}

void CFuelEncrypt::unpak(CStringA & u_str, unsigned char *p_str, int n)
{
	short int i;
	u_str.Empty();

	unsigned char Byte;

	for (i = 0; i < n; ++i)
	{
		Byte = (i & 1) ? (*p_str++) & 0xf | 0x30 : (*p_str >> 4) & 0xf | 0x30;
		if (Byte >= 0x3a)
			Byte += 7;   /* convert letters to asci */

		u_str += (char)Byte;
	}
}

/******************************************************************************
 Description:	Encrypt data buffer 	
 Returns:      	
 Parameters:   	sBufferChangedata -		buffer for encrypt
				lBufferChangedataSize - buffer encrypt size
				pszOutBuff			  - encrypted buffer	
				plOutBuffLen           - encrypted buffer size	


 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			9/12/2003   15:21		Start
******************************************************************************/

long CFuelEncrypt::ReplaceBuffer(const CStringA & sBufferData, CStringA & sEncryptedData)
{
	long lRetVal = ENCRYPT_OK;

	//amram
	long lFlagRemoveCard = 0;// _Module.m_server.m_cLogger.GetFlagsRemoveCardDataFromLog();//4.0.9.503

	if (lFlagRemoveCard==0)
	{
		lRetVal = Encrypt(sBufferData, sEncryptedData);
	}

	if(lFlagRemoveCard==0)
	{
		sEncryptedData.Format("%ld" , sBufferData.GetLength());
	}

	if( lRetVal != ENCRYPT_OK)
		lRetVal = ENCRYPT_NO_TRACK2;

	return lRetVal;
}

long CFuelEncrypt::ReplaceOLAString(char *pszOLAString, char *pszInBuff, char *pszOutBuff, long *plOutBuffLen ,bool bMaskAccount ) //4.0.6190.3000
{
	long lRetVal = ENCRYPT_OK;
	char* szTmpBuff;
	CStringA  sData;
	CStringA  sEncryptData;
	long lBuffBegin = 0;
	long lCurrBuffLen = 0;
	long lEncryptTrack2Len = 0;
	long lEndLen = 0;
	long lEndOfBuff = 0;
	long lInBuffLen = strlen(pszInBuff);

	szTmpBuff = strstr(pszInBuff, pszOLAString);
 
	if(szTmpBuff)
	{
		char *strStart = strstr(szTmpBuff,"=");
		char *strEnd = strstr(szTmpBuff,",");
		long lCardNumberLength = (long)strEnd - (long)strStart - 1;

		if (lCardNumberLength > 0)
		{
			sData.SetString(strStart+1, lCardNumberLength);
		}
		else
			lRetVal = ENCRYPT_STRING_NOT_FOUND;
		

		//amram
		long lFlagRemoveCard = 1;//_Module.m_server.m_cLogger.GetFlagsRemoveCardDataFromLog();//4.0.9.503

		if(lFlagRemoveCard == 0)
		{
			sEncryptData.Format("%ld" , lCardNumberLength);
		}
		else
		{//in this case we have to check if we under account
		//if so we have to encrypt (XXXXXX......XXXX).

			//4.0.10.301
			if (lCardNumberLength > 0)
			{
				if(!(strcmp("AccountNumber[S]=", pszOLAString)))
				{
					lRetVal = EncryptAccount(sData, sEncryptData, bMaskAccount);  //4.0.6190.3000
					lRetVal = ENCRYPT_OK;
				}

				if(!(strcmp("ClubCardTrack2[S]=", pszOLAString)))
				{
					lRetVal = EncryptAccount(sData, sEncryptData, bMaskAccount);	//4.0.6190.3000
					lRetVal = ENCRYPT_OK;
				}
			}
			else
				lRetVal = ENCRYPT_STRING_NOT_FOUND;
		}

		if( lRetVal == ENCRYPT_OK)
		{
			lBuffBegin = (long)strStart - (long)pszInBuff + 1;

			if(lBuffBegin > *plOutBuffLen)
			{
				//CString sMsg;
				//sMsg.Format("888888 lBuffBegin=%ld , ",lBuffBegin, *plOutBuffLen);
				//_Module.m_server.m_cLogger.LogMsg(sMsg);

				lRetVal = ENCRYPT_BUFF_TOO_SMALL;
			}
			else
			{
				memcpy(pszOutBuff, pszInBuff, lBuffBegin);
				lCurrBuffLen += lBuffBegin;
				lEncryptTrack2Len = sEncryptData.GetLength();

				if(lCurrBuffLen + lEncryptTrack2Len > *plOutBuffLen)
				{
					lRetVal = ENCRYPT_BUFF_TOO_SMALL;
				}
				else
				{
					//strcat(pszOutBuff,szEncryptData);
					//lCurrBuffLen += lEncryptTrack2Len;
					//4.0.10.132
					memcpy(pszOutBuff+lCurrBuffLen, (LPCSTR) sEncryptData, min(lEncryptTrack2Len, sEncryptData.GetLength()));//4.0.11.503
					lCurrBuffLen += lEncryptTrack2Len;//4.0.10.
					
					//copying the rest of the data
					lEndLen = strlen(strEnd);
					lEndOfBuff = min(lEndLen ,(*plOutBuffLen-lCurrBuffLen));

					if(lEndOfBuff + lCurrBuffLen == *plOutBuffLen)
						lEndOfBuff--;

					memcpy(pszOutBuff+lCurrBuffLen, strEnd, lEndOfBuff);//4.0.10.132
					//strncat(pszOutBuff,strEnd,lEndOfBuff);4.0.10.132
					lCurrBuffLen += lEndOfBuff;
					*plOutBuffLen = lCurrBuffLen;
				}
			}
		}
	}
	else
		lRetVal = ENCRYPT_STRING_NOT_FOUND;
  
	return lRetVal;
}

/******************************************************************************
  Description:	Clear card info from given string 	
  Returns:      succeeded
  Parameters:  	char *sIn - string source
				long lInSize - string source size , 
				char *sOut   - string destination 
  
  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
  Marik			6/9/2004   10:04		Start
 ******************************************************************************/
 
//4.0.6.505
//4.0.10.132
long CFuelEncrypt::RemoveCardInfoFromMessage(char *sIn, long lInSize, char *sOut, bool bMaskAccount)  //4.0.6190.3000
{
	long lRetVal = ENCRYPT_OK;
	char *szTmpMsg =  new char[lInSize +1];	
	char szTmpIn[8000]; //4.0.9490.0
	strcpy(szTmpIn,sIn);

	long lOutBuffLen = lInSize;

	if (lInSize)
	{
		//4.0.11.80memset(szTmpMsg , ' ' , sizeof(lInSize +50));
		memset(szTmpMsg , ' ' , lInSize + 1);  //4.0.11.81

		if (1 /*_Module.m_server.m_cLogger.GetFlagsRemoveCardDataFromLog()*/)  //4.0.6.505
		{
			ReplaceOLAString("AccountNumber[S]=", szTmpIn ,szTmpMsg,&lOutBuffLen , bMaskAccount);
			ZeroBuff(szTmpIn, szTmpMsg);
			ReplaceOLAString("Track2[S]=", szTmpIn ,szTmpMsg,&lOutBuffLen, bMaskAccount);
			ZeroBuff(szTmpIn, szTmpMsg);
			ReplaceOLAString("Track1[S]=", szTmpIn ,szTmpMsg,&lOutBuffLen, bMaskAccount);
			ZeroBuff(szTmpIn, szTmpMsg);
			ReplaceOLAString("ClubCardTrack2[S]=", szTmpIn ,szTmpMsg, &lOutBuffLen, bMaskAccount);  //4.0.10.47
			ZeroBuff(szTmpIn, szTmpMsg);
			ReplaceOLAString("ClubCardNumber[S]=", szTmpIn ,szTmpMsg, &lOutBuffLen, bMaskAccount);//4.0.10.47			
			ZeroBuff(szTmpIn, szTmpMsg);
			ReplaceOLAString("PIN[S]=", szTmpIn ,szTmpMsg,&lOutBuffLen, bMaskAccount);//4.0.12490.2600
			ZeroBuff(szTmpIn, szTmpMsg);
			ReplaceOLAString("ExtendPIN[S]=", szTmpIn ,szTmpMsg, &lOutBuffLen, bMaskAccount);//4.0.12490.2600			
			ZeroBuff(szTmpIn, szTmpMsg);

			memcpy(sOut, szTmpIn, lInSize);
		}
		else
			memcpy(sOut, szTmpIn, lInSize);
	}
	
	if (szTmpMsg)
		delete []szTmpMsg;
	
	return lRetVal;
}


void CFuelEncrypt::ZeroBuff(char * sInBuf,char * sOutBuf)
{
	memset(sInBuf, ' ', strlen(sInBuf));
	memcpy(sInBuf, sOutBuf, strlen(sInBuf));
}


/******************************************************************************
  Description:	Encrypt the account with the next format
				first six tav will come from the account
				4 XXXX
				last four tav from account
				nnnnnnXXXXnnnn
  Returns:      succeeded
  Parameters:  	
  
  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
  Amram			25/5/2005   10:04		Start
 ******************************************************************************/
 //4.0.10.300
long CFuelEncrypt::EncryptAccount(CStringA & sAccountNumber, CStringA & sEncryptionKey, bool bMask)
{
	char szEncrypt[80];
	char szAcountNumStart[PCI_COMPLIENT_MASK_NUM_START+1]	= {0};
	char szAcountNumEnd[PCI_COMPLIENT_MASK_NUM_END+1]		= {0};
	char szMask[PCI_COMPLIENT_DIGIT_NUM_TO_MASK+1]			= {0};

	memset(szEncrypt, ' ', sizeof(szEncrypt));
	CString str;
	long lLen = 0;
	

	if(!(ChrAll(sAccountNumber, sAccountNumber.GetLength(),' ')))
	{
		if (_Module.m_server.GetOLAInUse() == OLASRV_TRAVEL_CENTER ) //4.0.6190.3801
		{
			//4.0.6190.3801
			memset(szAcountNumStart,PCI_COMPLIENT_MASK_CHAR,PCI_COMPLIENT_MASK_NUM_START);
		}
		else
		{
			memcpy(szAcountNumStart,sAccountNumber,PCI_COMPLIENT_MASK_NUM_START);
		}

		const char * pStrEnd = strtok((LPSTR) (LPCSTR) sAccountNumber," ");
		lLen = strlen(pStrEnd);
		if(lLen<10)
		{
			strcpy_s(szEncrypt, _countof(szEncrypt), "XXXXXX");
			sEncryptionKey.SetString(szEncrypt, strlen(szEncrypt));
		}
		else
		{
			memcpy(szAcountNumEnd,pStrEnd+strlen(pStrEnd)-PCI_COMPLIENT_MASK_NUM_END,PCI_COMPLIENT_MASK_NUM_END);
			memset(szMask,PCI_COMPLIENT_MASK_CHAR,PCI_COMPLIENT_DIGIT_NUM_TO_MASK);
			if (_Module.m_server.GetOLAInUse() == OLASRV_TRAVEL_CENTER ) //4.0.6190.3801
			{	//We need 16 chars in this case
				sprintf_s(szEncrypt, _countof(szEncrypt), "XX%s%s%s",szAcountNumStart,szMask,szAcountNumEnd);

			}
			else
				sprintf_s(szEncrypt, _countof(szEncrypt), "%s%s%s",szAcountNumStart,szMask,szAcountNumEnd);

			sEncryptionKey.SetString(szEncrypt, strlen(szEncrypt));
		}

		CString sMsg;
		sMsg.Format("EncryptAccount() - The New Encrypt Account= %s ",szEncrypt);
		_Module.m_server.m_cLogger.LogMsg(sMsg);
	}
	else
	{
		str.Format("EncryptAccount() - The account that return from GetCardInfo is empty");
		_Module.m_server.m_cLogger.LogMsg(str);
	}

	return 0 ;	
}
	/*int		i=0;
	long	lLen = 16;
	i=lLen;
	strcpy(sAccount,"");
	strcpy(sAccount,"4271540300037768");//Marik - problem  with ola hard code 
	while(!isdigit(sAccount[i]))
		i--;
	long	lAccountLen = lLen - i-1;
	memset(bTrack2 , ' ' ,TRACK_2_DATA_SIZE);
	bTrack2[TRACK_2_DATA_SIZE] = 0;


	if(lLen>=10)
	{
		memcpy(bTrack2+TRACK_2_DATA_SIZE-LAST_STR,sAccount+lLen-LAST_STR-lAccountLen,LAST_STR);//last four tav
		memcpy(bTrack2+TRACK_2_DATA_SIZE-lLen,sAccount,FIRST_STR);//first six tav
	}
	else//just copy
	{
	}

	int		i=0;
	long	lLen = 21;
	i=lLen;
	while(sAccount[lLen] == ' ')
		i--;
	long	lAccountLen = lLen - i;
	memset(bTrack2 , ' ' ,TRACK_2_DATA_SIZE);
	bTrack2[TRACK_2_DATA_SIZE] = 0;


	if(lAccountLen>=10)
	{
		memcpy(bTrack2+TRACK_2_DATA_SIZE-LAST_STR,sAccount+lLen-LAST_STR,LAST_STR);//last four tav
		memcpy(bTrack2+TRACK_2_DATA_SIZE-lAccountLen,sAccount+i,FIRST_STR);//first six tav
	}
	else//just copy
	{
	}*/


long CFuelEncrypt::EncryptAccountDLL(char *sBufferChangedata, long lBufferChangedataSize, char *pszOutBuff, long lMaxOutBuffSize, long *plOutBuffLen)
{
	long lRetVal = ENCRYPT_OK;
	char  *szBufferData = new  char[lBufferChangedataSize +2]; 
	char  szEncryptData[MAX_FIELD_VAL * 2] ={0};
	long lBuffBegin = 0;
	long lCurrBuffLen = 0;
	long lEndOfBuff = 0;

	//clear buffer 
	memset(szBufferData , 0 , lBufferChangedataSize +2 );	
	memcpy(szBufferData,sBufferChangedata,lBufferChangedataSize);

	//amram
	lRetVal = Encrypt(szBufferData,szEncryptData);
	

	if( lRetVal == ENCRYPT_OK)
	{
		strncpy(pszOutBuff , szEncryptData , min(strlen(szEncryptData), sizeof(szEncryptData)));
	}
	else
		lRetVal = ENCRYPT_NO_TRACK2;


	delete []szBufferData;
		
	return lRetVal;
}


void CFuelEncrypt::DecryptAccountDLL(char *sBufferChangedata, char *pszOutBuff)
{
	Decrypt(sBufferChangedata,pszOutBuff);
}


//4.0.10490.999
BOOL CFuelEncrypt::Track2ToAccount(char *sTrack2)
{
	BOOL bRet = FALSE;
	char* sAccountEndPlace;
	long lAccountEnd = 0; 
	char szAccountBeforeEncrypt[TRACK_2_DATA_SIZE];
	
	try
	{
		memset(szAccountBeforeEncrypt,' ',sizeof(szAccountBeforeEncrypt));
		
		if (sTrack2)
		{
			//Find the end of the account number in the track2 string
			sAccountEndPlace = strstr(sTrack2,"=");
			
			//If we found the delimiter
			if (sAccountEndPlace)
			{
				lAccountEnd = sAccountEndPlace - sTrack2; 
			}
			
			if (lAccountEnd > 0)
			{
				memcpy(szAccountBeforeEncrypt,sTrack2,lAccountEnd);			
				bRet = TRUE;
			}
			
			memcpy(sTrack2,szAccountBeforeEncrypt,TRACK_2_DATA_SIZE);
		}
	}
	
	catch(...)
	{
		_Module.m_server.m_cLogger.LogMsg("Problem in Track2ToAccount",LOG_LEVEL_0);
	}
	
	return bRet;
}


long CFuelEncrypt::EncryptReceiptAccount(char *sAccountNumber, char *bTrack2, bool bMask)
{
	char szEncrypt[80];
	char szAcountNumStart[PCI_COMPLIENT_MASK_NUM_START+1]	= {0};
	char szAcountNumEnd[PCI_COMPLIENT_MASK_NUM_END+1]		= {0};
	char szMask[81]			= {0};
	
	memset(szEncrypt,' ',sizeof(szEncrypt));
	CString str;
	long lLen =0 ;
	
	
	if(!(ChrAll(sAccountNumber,21,' ')))
	{
		memcpy(szAcountNumStart,sAccountNumber, PCI_COMPLIENT_MASK_NUM_START);
		char * pStrEnd = strtok((char*)sAccountNumber," ");
		lLen = strlen(pStrEnd);
		if(lLen <= 10)
		{
			sprintf_s(szEncrypt, _countof(szEncrypt), "******");
			memcpy(bTrack2,szEncrypt,strlen(szEncrypt));
		}
		else
		{	
			memcpy(szAcountNumEnd,pStrEnd+strlen(pStrEnd)-PCI_COMPLIENT_MASK_NUM_END,PCI_COMPLIENT_MASK_NUM_END);
			
			//memset(szMask,PCI_COMPLIENT_MASK_CHAR,PCI_COMPLIENT_DIGIT_NUM_TO_MASK);
			
			memset(szMask,PCI_COMPLIENT_MASK_CHAR, (lLen - 10));
			//We need 16 chars in this case
			sprintf_s(szEncrypt, _countof(szEncrypt), "%s%s%s",szAcountNumStart,szMask,szAcountNumEnd);
			
			memcpy(bTrack2, szEncrypt, strlen(szEncrypt));
		}
		CString sMsg;
		sMsg.Format("EncryptReceiptAccount() - The New Encrypt Account= %s ",szEncrypt);
		_Module.m_server.m_cLogger.LogMsg(sMsg);
		//		}		
	}
	else
	{
		str.Format("EncryptReceiptAccount() - The account that return from GetCardInfo is empty");
		_Module.m_server.m_cLogger.LogMsg(str);
	}
	
	return 0;	
}