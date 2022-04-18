// PCIEncodeHandler.cpp: implementation of the CPCIEncodeHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PCIEncodeHandler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CPCIEncodeHandler::CPCIEncodeHandler(const CBasicProtectData& cBasicProtectData) : 
		CBaseEncodeHandler(cBasicProtectData),
		m_cEncodingComDll(CLSID_Encryption),
		m_cDecodingComDll(CLSID_Decryption)		// TD 347994 
{

}

CPCIEncodeHandler::~CPCIEncodeHandler()
{

}

//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////
/******************************************************************************
 Description:	Encode PCI data
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			12/10/2007   11:34		Start	
 Gena			17/08/2008	 13:34		Change class name
 Gena			24/06/2009	 18:45		ReMark
******************************************************************************/

// void CPCIEncodeHandler::PCIEncryptData(BYTE *data , long lSize)
// {
// 	for (int i=0 ; i < lSize ; i++)
// 	{
// 		data[i] += i; 		
// 	}
// }

/******************************************************************************
 Description:	Decrypt PCI data
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			12/10/2007   11:34		Start	
 Gena			17/08/2008	 13:34		Change class name
 Gena			24/06/2009	 18:45		ReMark
******************************************************************************/

// void CPCIEncodeHandler::PCIDecryptData(BYTE *data , long lSize)
// {
// 	for (int i=0 ; i < lSize ; i++)
// 	{
// 		data[i] -= i; 		
// 	}
// } 

/*		//4.0.20.501 TD 60236		NOT IN USE !!!
long CPCIEncodeHandler::YEncode(unsigned char *open_string,unsigned char *encripted_string)
{
   unsigned long src_len;
   unsigned long index=0, index2=0;
   unsigned char c=0, c2=0, out=0;
   unsigned char dest_str[1000];
  
   src_len = max (strlen((char*)open_string),MIN_PASS_SIZE);
   
   if (src_len > sizeof(dest_str)/4)
   	return ENCODE_FAIL; // len too big
    
   while (index < src_len )
   {
      c2=0;
      c= open_string[index++];
      Encryption(&c, &c2);
      dest_str[index2++] = c ;
      dest_str[index2++] = c2 ;
   }
    
   unpak( encripted_string, dest_str, index2 * 2);
    
   encripted_string[ index2*2 ] = '\0';
    
//   printf ("encoded to: %s \n",encripted_string);
   return(OK); 
   
    
}


long CPCIEncodeHandler::YDecode(unsigned char *open_string,unsigned char *encripted_string)
{
   unsigned long index=0, index2=0;
   unsigned char c=0, c2=0, out=0;
   unsigned char pak_str[1000];
   
   unsigned long ascii_len = strlen((char*)encripted_string);
   
   if (ascii_len > 2*sizeof(pak_str))
   	return ENCODE_FAIL; // len too big
      
   pak( encripted_string, pak_str, ascii_len);
  
   while (index < ascii_len / 2 )
   {
      c= pak_str[index++];
      c2= pak_str[index++];
      Decryption(&c, c2);
      open_string[index2++] = c ;
   }
   open_string[index2++] = '\0';
//   printf ("decoded to: %s \n",open_string);
   return(OK); 
  
}	



// This function takes two parameters.  First, is the character to be
// encrypted.  The second parameter should be 0.  The original character
// is spread throughout the two parameters.  This function returns nothing.
// This function mixes the original character with a random character.
// So, the letter 'A' would be encrypted differently every time, making
// for hard decryption.  This method will keep MOST users from breaking
// into your data.  Example:

// Encrypt( &Original_Character,  &Second_Character);

void CPCIEncodeHandler::Encryption(unsigned char *c,unsigned char *c2)
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
void CPCIEncodeHandler::Decryption(unsigned char *c,unsigned char c2)
{

  *c  =  (  ( *c & EVEN_BIT_MASK ) | ( c2 & ODD_BIT_MASK )  );

}
*/
/*
	pak
	===
	pack buffer,add 7 to codes > 39H

*/
/*
void CPCIEncodeHandler::pak(unsigned char * u_str,unsigned char * p_str,int n)
{
	short int i;
	char uh,ul;

	for (i=0 ; i < n/2 ; i++)
	{
	   if ((uh=*u_str++) >= 0x41 )
		  uh -= 7;   // convert leters to asci 
	   if ((ul=*u_str++) >= 0x41 )
		  ul -= 7;   // convert leters to asci 
	   *p_str++ = (uh & 0xf) << 4 | ul & 0xf;
	}
}
*/
/*
	unpak
	=====
	unpack buffer ,sub 7 from codes >41H

*/
/*
void CPCIEncodeHandler::unpak(unsigned char *u_str,unsigned char *p_str,int n)
{
	short int i;

	for (i=0; i < n ;i++,u_str++)
	{
	       *u_str = (i & 1) ? (*p_str++) & 0xf | 0x30
				  : (*p_str >> 4 ) & 0xf | 0x30 ;
	       if (*u_str >= 0x3a )
		  *u_str  += 7;   // convert leters to asci 
	}
}
*/

/************************************************************************/
/*						Protected Methods								*/
/************************************************************************/

// Function name	: CPCIEncodeHandler::Encrypt
// Description	    : Encrypt a given buffer.		NOT IN USE!!!
// Return type		: long - ENCODE_OK=0,  ENCODE_FAIL=1
// Argument         : char *pszOpenBuff		- Data to encrypt.
// Argument         : char *pszEncryBuff	- Encrypt result.
// EncodeRetCodeEnum CPCIEncodeHandler::Encrypt(char* pszEncryptedBuffOut, const char* pszOpenBuffIn, const long lPumpNumber)
// {	
// 	EncodeRetCodeEnum eEncodeRetCode = ENCODE_FAIL;
// /*	
// 	long	lRetVal = ENCODE_OK;
// 	
// 	lRetVal = YEncode((unsigned char*)pszOpenBuffIn, (unsigned char*)pszEncryptedBuffOut);
// 	
// 	if(lRetVal != ENCODE_OK)
// 		bRetCode = FALSE;
// */
// 	return eEncodeRetCode;
// }



// Function name	: CPCIEncodeHandler::Decrypt
// Description	    : Decrypt a given buffer. 	NOT IN USE!!!
// Return type		: long - ENCODE_OK=0,  ENCODE_FAIL=1
// Argument         : char *pszOpenBuff		- Data to encrypt.
// Argument         : char *pszEncryBuff	- Encrypt result.
// EncodeRetCodeEnum CPCIEncodeHandler::Decrypt(char* pszOpenBuffOut, const char* pszDecryptBuffIn)
// {
// 	EncodeRetCodeEnum eEncodeRetCode = ENCODE_FAIL;
// /*	
// 	long	lRetVal = ENCODE_OK;
// 	
// 	lRetVal = YDecode((unsigned char*)pszOpenBuffOut,(unsigned char*)pszDecryptBuffIn);
// 	
// 	if(lRetVal != ENCODE_OK)
// 		bRetCode = FALSE;
// */
// 	return eEncodeRetCode;
// }



/************************************************************************/
/*                    Public Methods                                    */
/************************************************************************/


/************************************************************************/
/*                    Initialization Methods                            */
/************************************************************************/

/******************************************************************************
 Description:	Initialize
 Returns:      	ENCODE_DLL_OK - ok, else - fail
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Change class name	4.0.20.501 TD 60236
 AlexM			04/11/2013	 13:36		Added Decoding Dll	4.0.23.2100 TD 347994
******************************************************************************/
	
EncodeComDllRetCodeEnum CPCIEncodeHandler::Init()
{
	LoadConfiguration(FALSE);

	m_cDecodingComDll.Init();   
	return m_cEncodingComDll.Init();
}


/************************************************************************/
/*             DB Methods												*/
/************************************************************************/

/******************************************************************************
 Description:	Encode all Card sale
 Returns:      	ENCODE_OK - ok
 Parameters:   	cPAPInfoInOut - PAP struct to Encode
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			12/10/2007   11:34		Start	
 Gena			17/08/2008	 13:34		Change class name 4.0.20.501 TD 60236
 Gena			24/06/2009	 18:45		ReMark
******************************************************************************/

/*EncodeRetCodeEnum CPCIEncodeHandler::EncodeForDB(PAY_AT_PUMP_INFO& cPAPInfoInOut)	
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;

	PCIEncryptData(cPAPInfoInOut.CardSaleInfo.cardData.sAccountNumber , sizeof(cPAPInfoInOut.CardSaleInfo.cardData.sAccountNumber));
	PCIEncryptData(cPAPInfoInOut.CardSaleInfo.cardData.sTrack1Data , sizeof(cPAPInfoInOut.CardSaleInfo.cardData.sTrack1Data));
	PCIEncryptData(cPAPInfoInOut.CardSaleInfo.cardData.sTrack2Data , sizeof(cPAPInfoInOut.CardSaleInfo.cardData.sTrack1Data));
	PCIEncryptData(cPAPInfoInOut.CardSaleInfo.extraData.sLoyalId , sizeof(cPAPInfoInOut.CardSaleInfo.extraData.sLoyalId));
	PCIEncryptData(cPAPInfoInOut.CardSaleInfo.cardData.sClubCardTrack2Data , sizeof(cPAPInfoInOut.CardSaleInfo.cardData.sClubCardTrack2Data));
	PCIEncryptData(cPAPInfoInOut.CardSaleExtraData3.sTrack3Data , sizeof(cPAPInfoInOut.CardSaleExtraData3.sTrack3Data));
	PCIEncryptData(cPAPInfoInOut.CardSaleExtraData2.sResInfoAMax37 , sizeof(cPAPInfoInOut.CardSaleExtraData2.sResInfoAMax37));	
	PCIEncryptData(cPAPInfoInOut.CardSaleExtraData2.sParam1 , sizeof(cPAPInfoInOut.CardSaleExtraData2.sParam1));
	PCIEncryptData(cPAPInfoInOut.CardSaleExtraData2.sParam2 , sizeof(cPAPInfoInOut.CardSaleExtraData2.sParam2));

	return eEncodeRetCode;
}*/


/******************************************************************************
 Description:	Decode all Card sale
 Returns:      	ENCODE_OK - ok
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			12/10/2007   11:34		Start
 Gena			17/08/2008	 13:34		Change class name 4.0.20.501 TD 60236
 Gena			24/06/2009	 18:45		ReMark
******************************************************************************/

/*EncodeRetCodeEnum CPCIEncodeHandler::DecodeForDB(PAY_AT_PUMP_INFO& cPAPInfoInOut)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;

	PCIDecryptData(cPAPInfoInOut.CardSaleInfo.cardData.sAccountNumber , sizeof(cPAPInfoInOut.CardSaleInfo.cardData.sAccountNumber));
	PCIDecryptData(cPAPInfoInOut.CardSaleInfo.cardData.sTrack1Data , sizeof(cPAPInfoInOut.CardSaleInfo.cardData.sTrack1Data));
	PCIDecryptData(cPAPInfoInOut.CardSaleInfo.cardData.sTrack2Data , sizeof(cPAPInfoInOut.CardSaleInfo.cardData.sTrack1Data));
	PCIDecryptData(cPAPInfoInOut.CardSaleInfo.extraData.sLoyalId , sizeof(cPAPInfoInOut.CardSaleInfo.extraData.sLoyalId));
	PCIDecryptData(cPAPInfoInOut.CardSaleInfo.cardData.sClubCardTrack2Data , sizeof(cPAPInfoInOut.CardSaleInfo.cardData.sClubCardTrack2Data));
	PCIDecryptData(cPAPInfoInOut.CardSaleExtraData3.sTrack3Data , sizeof(cPAPInfoInOut.CardSaleExtraData3.sTrack3Data));
	PCIDecryptData(cPAPInfoInOut.CardSaleExtraData2.sResInfoAMax37 , sizeof(cPAPInfoInOut.CardSaleExtraData2.sResInfoAMax37));
	PCIDecryptData(cPAPInfoInOut.CardSaleExtraData2.sParam1 , sizeof(cPAPInfoInOut.CardSaleExtraData2.sParam1));
	PCIDecryptData(cPAPInfoInOut.CardSaleExtraData2.sParam2 , sizeof(cPAPInfoInOut.CardSaleExtraData2.sParam2));

	return eEncodeRetCode;
}*/


/************************************************************************/
/*               Account Methods                                        */
/************************************************************************/

/******************************************************************************
 Description:	Encrypt Account Number with ComDLL
 Returns:      	ENCODE_OK - ok, else - fail
 Parameters:   	pszEncryptedBuffOut - out encrypted buff
				pszOpenBufferIn - buff to encrypt
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Change class name 4.0.20.501 TD 60236 
 AlexM			04/11/2013	 13:36		Update			  4.0.23.2100 TD 347994
******************************************************************************/
EncodeRetCodeEnum CPCIEncodeHandler::EncryptAccount(IN const char* pszAccountNumberIn, OUT char* pszEncryptedBuffOut, long lPumpNumber)
{
    EncodeComDllRetCodeEnum	eEncodeDLLRetCode = ENCODE_DLL_OK;
	EncodeRetCodeEnum			eEncodeRetCode = ENCODE_OK;
	long						lRetVal = 0;	
		
	if(m_cEncodingComDll.PtrInstance() != NULL)
	{
		try
		{				
			CComBSTR sAccount(pszAccountNumberIn);
			CComBSTR sEncryptAccount;

			lRetVal = m_cEncodingComDll.PtrInstance()->EncryptAccountNumber(sAccount.m_str, &sEncryptAccount.m_str);
			if(lRetVal != ENCODE_OK)
			{
				if (LOG_LOGIC_ERROR)				
				{
					CString sErrMsg;
					sErrMsg.Format("EncryptionAccount failed! RetCode=%ld",lRetVal);
					_LOGMSG.LogClassMsg("CPCIEncodeHandler", "EncryptAccount", lPumpNumber, LOG_PUMP, sErrMsg);
				}				
			}
			else
			{
				Bstr2Buffer(sEncryptAccount,pszEncryptedBuffOut,sEncryptAccount.Length()); // 4.0.23.1280    TD 310220
				CString sMsg;
				sMsg.Format("EncryptionAccount successfully, retBuf:[%.20s]",pszEncryptedBuffOut);//4.0.11.503
				_LOGMSG.LogClassMsg("CPCIEncodeHandler", "EncryptAccount", lPumpNumber, LOG_PUMP, sMsg);				
			}
		}
		catch (_com_error& e)  
		{
			m_cEncodingComDll.WriteComError(e);			
			eEncodeDLLRetCode = m_cEncodingComDll.RestoreCOM();
			
		}
		catch(...)
		{
			if (LOG_LOGIC_ERROR )
			{
				_LOGMSG.LogClassMsg("CPCIEncodeHandler", "EncryptAccount", lPumpNumber, LOG_PUMP, "EncryptionAccount Failed!",LOG_LEVEL_1);			
			}

			eEncodeDLLRetCode = m_cEncodingComDll.RestoreCOM();
		}
	}
	else
	{
		eEncodeDLLRetCode = m_cEncodingComDll.RestoreCOM();
		memset(pszEncryptedBuffOut,' ',strlen(pszEncryptedBuffOut));
		
		eEncodeRetCode = ENCODE_FAIL;
	}

	if (eEncodeDLLRetCode != ENCODE_DLL_OK)
		eEncodeRetCode = ENCODE_FAIL;
	
	return eEncodeRetCode;	
}


/******************************************************************************
 Description:	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Change class name 4.0.20.501 TD 60236
******************************************************************************/
//protected::
void CPCIEncodeHandler::AccountNumToParam2(PAY_AT_PUMP_INFO *pInfo)
{
	char* sValEndByBlank;
	long lValLen;
	long lAccountNumberSize = sizeof(pInfo->CardSaleInfo.cardData.sAccountNumber);
	long lAccountLen =0;
	
	try
	{
		char szTmpAccountNumber[sizeof(pInfo->CardSaleInfo.cardData.sAccountNumber)];

		memcpy(szTmpAccountNumber,pInfo->CardSaleInfo.cardData.sAccountNumber,lAccountNumberSize);

		//find the account len.
		sValEndByBlank = strstr(szTmpAccountNumber," "); 
		if (sValEndByBlank)
		{
			lValLen = sValEndByBlank - szTmpAccountNumber; 
		}
		//Just to make sure.
		if (lValLen > lAccountNumberSize)
		{
			lValLen = lAccountNumberSize;
		}
		
		lValLen = min(strlen(szTmpAccountNumber),sizeof(szTmpAccountNumber));
	
		for( lAccountLen=0;lAccountLen < lValLen;lAccountLen++)
		{
			if (szTmpAccountNumber[lAccountLen] == ' ')
				break;			
		}

		lValLen = lAccountLen;
		
		//Only if there is encryption.
		if (lValLen > 10)
		{
			//copy the encrypt part to param2
			memcpy(pInfo->CardSaleExtraData2.sParam2,szTmpAccountNumber + 6,lValLen - 10);
		}
		else if (lValLen > 6)
		{
			memcpy(pInfo->CardSaleExtraData2.sParam2,szTmpAccountNumber + 6,lValLen - 6);
		}
	}
	catch(...)
	{
		_LOGMSG.LogClassMsg("CPCIEncodeHandler", "AccountNumToParam2", 0, LOG_NONE, "Problem in AccountNumToParam2",LOG_LEVEL_0);
	}

}


//Mask head->0 - tail->4
//RFUEL-172
BOOL CPCIEncodeHandler::MaskAccount(char* bTrack2, const char* sAccountNumber, const MaskTypeEnum eMaskType)const
{
	BOOL	bRetCode = TRUE;
	char	szEncrypt[ACCOUNT_NUM_SIZE];
	char	szAcountNumStart[PCI_COMPLIENT_MASK_NUM_START + 1] = { 0 };
	char	szAcountNumEnd[PCI_COMPLIENT_MASK_NUM_END + 1] = { 0 };
	char	szMask[MAX_FIELD_VAL] = { 0 };


	memset(szEncrypt, ' ', sizeof(szEncrypt));

	long	lLen = 0;
	long	lEndLen = 0;
	long	lMaskLen = 0;
	long	lStartLen = 0;

	char	sTmpAccount[ACCOUNT_NUM_SIZE];
	memset(sTmpAccount, ' ', ACCOUNT_NUM_SIZE);
	memcpy(sTmpAccount, sAccountNumber, ACCOUNT_NUM_SIZE);

	if ((!(ChrAll(sTmpAccount, ACCOUNT_NUM_SIZE, ' '))) && (sAccountNumber[0]))
	{
		CString s(sTmpAccount, ACCOUNT_NUM_SIZE);
		lLen = s.Find(' ');
		if (lLen == -1) // did not found ' '
			lLen = ACCOUNT_NUM_SIZE;

		lEndLen = min(lLen,strlen(sTmpAccount), sizeof(sTmpAccount)); //In case the string ends with NULL and not with " ".

		if ((lEndLen && lEndLen < lLen) || !lLen)
		{
			lLen = lEndLen;
		}

		if (lStartLen > lLen)
		{
			lStartLen = lLen;
		}

		if(lStartLen == PCI_COMPLIENT_MASK_NUM_START)
			memcpy(szAcountNumStart,sTmpAccount,lStartLen);			
		else
			memset(szAcountNumStart, PCI_COMPLIENT_MASK_CHAR, PCI_COMPLIENT_MASK_NUM_START);

		if(lLen<=6)
		{
			if(lLen > 0)
			{
				sprintf(szEncrypt,"%s",szAcountNumStart);
				memcpy(bTrack2,szEncrypt, min(strlen(szEncrypt),sizeof(szEncrypt)));	
			}
		}
		else if(lLen<=10)
		{
			//4.0.12.170
			lMaskLen = lLen - 6;
			if (lMaskLen > 0)
			{
				memset(szMask,PCI_COMPLIENT_MASK_CHAR,lMaskLen);
			}

			sprintf(szEncrypt, "%s%s",szAcountNumStart,szMask);
			memcpy(bTrack2,szEncrypt, min(strlen(szEncrypt),sizeof(szEncrypt)));	
		}
		else
		{		
			memcpy(szAcountNumEnd, sTmpAccount + lEndLen - PCI_COMPLIENT_MASK_NUM_END, PCI_COMPLIENT_MASK_NUM_END);

			lMaskLen = lLen - 10;
			if (lMaskLen > 0)
			{
				memset(szMask,PCI_COMPLIENT_MASK_CHAR,lMaskLen);
			}

			sprintf(szEncrypt, "%s%s%s",szAcountNumStart,szMask,szAcountNumEnd);
			memcpy(bTrack2,szEncrypt, min(strlen(szEncrypt),sizeof(szEncrypt)));	
		}

		if(LOG_FCC_FLOW_CONTROL)
		{			
			CString sMsg;
			sMsg.Format("MaskAccount - The New Mask Account= %s ",szEncrypt);
			_LOGMSG.LogMsg(sMsg);
		}
	}

	return bRetCode;
}


/******************************************************************************
 Description:	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Change class name 4.0.20.501 TD 60236
 AlexM			04/11/2013	 13:36		added DSM	  4.0.23.2100 TD 347994 
******************************************************************************/
BOOL CPCIEncodeHandler::GetDecryptedAccount(char* sEncryptAccount,char* sEncryptedPart,char* sDecryptedAccount)
{
	EncodeComDllRetCodeEnum	eEncodeDLLRetCode = ENCODE_DLL_OK;
	EncodeRetCodeEnum			eEncodeRetCode = ENCODE_OK;
	long lRetVal = 0;

	if(m_cDecodingComDll.PtrInstance() != NULL)
	{
		try
		{	
			
			
			
			char szDecryptedAccount[MAX_FIELD_VAL]={0};
			
			CComBSTR sAccountNumber("L  ");
			CComBSTR bstrEncryptAccount(sEncryptAccount);
			
			
			char szCustomerEncryptionKey[MAX_ENCRYPT_LEN]={0};
			
			GetCustomerEncryptionKey((char*)szCustomerEncryptionKey);
			
			CComBSTR sCustomerEncryptionKey(szCustomerEncryptionKey);
			
			// This GUID is validated by DSM.dll during each decryption call
			CComBSTR sAppIdentiferGUID ("{36FBDF63-2BBC-11D2-BD8B-004005614539}");
			
			// Encryption module Identifier
			CComBSTR sEncryptionModuleId ("{8BAB5D39-1FD1-474E-BC2E-69441AFCF67E}");
			
			
			
			lRetVal = m_cDecodingComDll.PtrInstance()->DecryptAccountNumber(bstrEncryptAccount.m_str, sCustomerEncryptionKey.m_str, sEncryptionModuleId.m_str, sAppIdentiferGUID.m_str, &sAccountNumber.m_str);
			
			if(lRetVal != ENCODE_OK)
			{
				if (LOG_LOGIC_ERROR)				
				{
					CString sErrMsg;
					sErrMsg.Format("DecryptionAccount failed! RetCode=%ld",lRetVal);
					_Module.m_server.m_cLogger.LogClassMsg("CPCIEncodeHandler", "DecryptAccount", 0, LOG_PUMP, sErrMsg);
				}				
			}
			else
				
				
				
				
			{
				Bstr2Buffer(sAccountNumber, sDecryptedAccount, sAccountNumber.Length()); 
				
				
				
				CString			  sMsg;
				sMsg.Format("DecryptionAccount successfully, inBuf:%.20s", sEncryptAccount);
				_Module.m_server.m_cLogger.LogClassMsg("CPCIEncodeHandler", "sEncryptAccount", 0, LOG_PUMP, sMsg);				
				
			}
		}
		catch (_com_error& e)  
		{
			m_cDecodingComDll.WriteComError(e);			
			eEncodeDLLRetCode = m_cDecodingComDll.RestoreCOM();
			
		}
		catch(...)
		{
			if (LOG_LOGIC_ERROR )
			{
				_Module.m_server.m_cLogger.LogClassMsg("CPCIEncodeHandler", "DeryptAccount", 0, LOG_PUMP, "EncryptionAccount Failed!",LOG_LEVEL_1);			
			}
			
			eEncodeDLLRetCode = m_cDecodingComDll.RestoreCOM();
		}
	}
	else
	{
		eEncodeDLLRetCode = m_cDecodingComDll.RestoreCOM();
		memset(sDecryptedAccount,' ',strlen(sDecryptedAccount));
		
		eEncodeRetCode = ENCODE_FAIL;
	}
	
	if (eEncodeDLLRetCode != ENCODE_DLL_OK)
		eEncodeRetCode = ENCODE_FAIL;
	
	return eEncodeRetCode;	
}




/************************************************************************/
/*                     LoyalId Methods                                   */
/************************************************************************/

/******************************************************************************
 Description:	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Change class name 4.0.20.501 TD 60236
******************************************************************************/
void CPCIEncodeHandler::LoyalIdToParam1(PAY_AT_PUMP_INFO *pInfo)
{
	char* sValEndByBlank;
	long lValLen;
	long lAccountNumberSize = sizeof(pInfo->CardSaleInfo.extraData.sLoyalId);

	try
	{
		char szTmpAccountNumber[sizeof(pInfo->CardSaleInfo.extraData.sLoyalId)];

		memcpy(szTmpAccountNumber,pInfo->CardSaleInfo.extraData.sLoyalId,lAccountNumberSize);

		//find the account len.
		sValEndByBlank = strstr(szTmpAccountNumber," "); 
		if (sValEndByBlank)
		{
			lValLen = sValEndByBlank - szTmpAccountNumber; 
		}
		//Just to make sure.
		if (lValLen > lAccountNumberSize)
		{
			lValLen = lAccountNumberSize;
		}

		//Only if there is encryption.
		if (lValLen > 10)
		{
			//copy the encrypt part to param2
			memcpy(pInfo->CardSaleExtraData2.sParam1,szTmpAccountNumber + 6,lValLen - 10);
		}
		else if (lValLen > 6)
		{
			memcpy(pInfo->CardSaleExtraData2.sParam1,szTmpAccountNumber + 6,lValLen - 6);
		}

	}
	catch(...)
	{
		_LOGMSG.LogClassMsg("CPCIEncodeHandler", "LoyalIdToParam1", 0, LOG_NONE, "Problem in LoyalIdToParam1",LOG_LEVEL_0);
	}

}



/************************************************************************/
/*                     Access Methods                                   */
/************************************************************************/

/******************************************************************************
 Description:	Get Receipt Index from PAY_AT_PUMP_INFO
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Start 4.0.20.501 TD 60236
 AlexM			04/11/2013	 13:36		add DSM	  4.0.23.2100 TD 347994 
 OmerT          27/08/2020              RFUEL-1916 
******************************************************************************/
long CPCIEncodeHandler::GetReceiptIndex(char* sIndexOUT, const PAY_AT_PUMP_INFO* pcPayAtPumpInfoIn, const long lPumpNumber)
{
	long lRetIndexLen = 0;
	
	if(pcPayAtPumpInfoIn != NULL)	
	{
		
		lRetIndexLen = sizeof(pcPayAtPumpInfoIn->CardSaleInfo.cardData.sAccountNumber);	
		memset(sIndexOUT, ' ', lRetIndexLen);

		
		CString sAccountNumber(pcPayAtPumpInfoIn->CardSaleInfo.cardData.sAccountNumber);
		sAccountNumber = sAccountNumber.Left(sizeof pcPayAtPumpInfoIn->CardSaleInfo.cardData.sAccountNumber);
		sAccountNumber.Trim();
		if (sAccountNumber.GetLength() <= TRS_NUMBER_LENGTH) // Account number is trs number, no need to mask
		{
			memcpy(sIndexOUT, (char*)pcPayAtPumpInfoIn->CardSaleInfo.cardData.sAccountNumber, sizeof pcPayAtPumpInfoIn->CardSaleInfo.cardData.sAccountNumber);
		}
		else
		{
			MaskAccount(sIndexOUT, (char*)pcPayAtPumpInfoIn->CardSaleInfo.cardData.sAccountNumber);
		}

	
		_LOGMSG.LogClassMsg("CPCIEncodeHandler", "GetReceiptIndex", lPumpNumber, LOG_PUMP, "Receipt index from PAPInfo is PCIAccountNumber!");
	}

	return lRetIndexLen;
}

/******************************************************************************
 Description:	Get Receipt Index from BSTR
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Start 4.0.20.501 TD 60236
 OmerT          18/07/2019   14:26      RFUEL-172 start
******************************************************************************/
long CPCIEncodeHandler::GetReceiptIndex(char* sIndexOut, const BSTR sTrack2In, const long lFlags, const long lPumpNumber)	
{
	long lRetIndexLen = 0;
	if (	(lFlags & RECEIPT_BY_ACCOUNT_NUM)		|| (lFlags & RECEIPT_INDEX_IS_RECEPT_KEY) || 
			(lFlags & RECEIPT_UNRECOGNIZED_BILL)	|| (lFlags & RECEIPT_CAR_REGISTRATION_INDEX) || 
			(lFlags & RECEIPT_INDEX_IS_TRS)			/*|| (lFlags & RECEIPT_GET_LATEST_RECEIPT)*/ )		//4.0.24.90 TD 120918
	{
		long  lRequiredSize = WideCharToMultiByte(CP_ACP, 0, sTrack2In, -1, NULL, 0, NULL, NULL);
		lRetIndexLen = WideCharToMultiByte(CP_ACP, 0, sTrack2In, -1, sIndexOut, lRequiredSize, NULL, NULL);
	}
	else
	{
		char sAccountNumber[ACCOUNT_NUM_SIZE];
		memset(sAccountNumber, ' ', ACCOUNT_NUM_SIZE);
		lRetIndexLen = _Module.m_server.m_cCommonFunction.GetAccountNumByTrack2(sAccountNumber, sTrack2In, lFlags);
		MaskAccount(sIndexOut, sAccountNumber);
	}
		
	

	CString strLogMsg;
	strLogMsg.Format("Get Receipt index from BSTR, RetIndexLen=%ld.", lRetIndexLen);
	_LOGMSG.LogClassMsg("CPCIEncodeHandler", "GetReceiptIndex", lPumpNumber, LOG_PUMP, strLogMsg);
	
	return lRetIndexLen;
}

/******************************************************************************
 Description:	get PAY_AT_PUMP_INFO and encrypt the data
					  the new account encrypt will insert into
					  track1,track2,extrafield
					  after it we will encrypt the account with our 
					  encrytpion 	
 Parameters:	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			26/05/2005   14:34		Start
 AlexM			04/11/2013	 13:36		add DSM	  4.0.23.2100 TD 347994 
******************************************************************************/
void	CPCIEncodeHandler::GenerateIndex(PAY_AT_PUMP_INFO* pInfo, const BOOL bStoreAccountNum, const long lTrsNumber)
{	
	PAY_AT_PUMP_INFO  *pPAPInfo;
	EncodeRetCodeEnum lEncodeRetVal = ENCODE_OK;
	BYTE			  szAccountNumber[ACCOUNT_NUM_SIZE+2] = {0};
	BYTE			  sEncryptAccountDll[MAX_FIELD_VAL] = {0}; 
	
	CString			  sMsg;
	
	if(pInfo)
		pPAPInfo = (PAY_AT_PUMP_INFO *)pInfo;
	
	try
	{
		long lPumpNumber = a2l((BYTE*)pPAPInfo->CardSaleInfo.cardData.sPumpNumber, sizeof(pPAPInfo->CardSaleInfo.cardData.sPumpNumber));
		
		if(TRUE == bStoreAccountNum)		//4.0.22.13	TD 72716
			AccountNumToParam2(pPAPInfo);
		
		memset(&sEncryptAccountDll,' ',sizeof(sEncryptAccountDll));
		memcpy(&szAccountNumber,&pPAPInfo->CardSaleInfo.cardData.sAccountNumber,sizeof(pPAPInfo->CardSaleInfo.cardData.sAccountNumber));
		
		
		lEncodeRetVal = EncryptAccount((char *) szAccountNumber,(char*)sEncryptAccountDll,lPumpNumber);
		
		if(ENCODE_OK == lEncodeRetVal)	
			sMsg.Format("CProtectedReceipt::GenerateIndex Encrypt Account success");
		else
			sMsg.Format("CProtectedReceipt::GenerateIndex Encrypt Account Failed to create new key"); 	
		_Module.m_server.m_cLogger.LogMsg(sMsg);
		
		// copy the encrypt account
		memcpy(pPAPInfo->CardSaleExtraData4.sEncryptedAccount,sEncryptAccountDll,sizeof(pPAPInfo->CardSaleExtraData4.sEncryptedAccount)); // 4.0.23.1280    TD 310220
		
		memset(pPAPInfo->CardSaleInfo.cardData.sTrack1Data,' ',sizeof(pPAPInfo->CardSaleInfo.cardData.sTrack1Data));
		memset(pPAPInfo->CardSaleInfo.cardData.sTrack2Data,' ',sizeof(pPAPInfo->CardSaleInfo.cardData.sTrack2Data));
		memset(pPAPInfo->CardSaleInfo.cardData.sClubCardTrack2Data,' ',sizeof(pPAPInfo->CardSaleInfo.cardData.sClubCardTrack2Data)); //4.0.12.171
		
		memset(pPAPInfo->CardSaleExtraData2.sResInfoAMax37,		' ', sizeof(pPAPInfo->CardSaleExtraData2.sResInfoAMax37)); //4.0.14.1020  //4.0.23.120 TD 85948
		memset(pPAPInfo->CardSaleInfo.cardData.sPINBlock,		' ', sizeof(pPAPInfo->CardSaleInfo.cardData.sPINBlock)); //4.0.14.681
		memset(pPAPInfo->CardSaleExtraData3.sParamResult,		' ', sizeof(pPAPInfo->CardSaleExtraData3.sParamResult)); //4.0.23.1591
		memset(pPAPInfo->CardSaleInfo.extraData.sCustomerName,	' ', sizeof(pPAPInfo->CardSaleInfo.extraData.sCustomerName)); //4.0.23.264?
	}
	catch(...)
	{
		sMsg.Format("CProtectedReceipt::GenerateIndex Had UnExcpcted Error",LOG_LEVEL_1);
		_Module.m_server.m_cLogger.LogMsg(sMsg);
	}	
}

//Gena			01/07/2010	21:00		 //4.0.25.30 
void	CPCIEncodeHandler::GenerateIndex(CARD_SALE_ALL3* pcCardSaleAll3InOut, const BOOL bStoreAccountNum, const long lTrsNumber)
{
	if (pcCardSaleAll3InOut != NULL)
	{
		CARD_SALE_ALL3		cCardSaleAll3;
		PAY_AT_PUMP_INFO	cPAPInfo;
		
		memset(&cCardSaleAll3, ' ', sizeof(cCardSaleAll3));
		memset(&cPAPInfo, ' ', sizeof(cPAPInfo));

		memcpy(&cCardSaleAll3, pcCardSaleAll3InOut, sizeof(cCardSaleAll3));
		_Module.m_server.m_cCommonFunction.ConvertCardSaleAll3ToPayAtPumpInfo(cPAPInfo, cCardSaleAll3);

		GenerateIndex(&cPAPInfo, bStoreAccountNum, lTrsNumber);
		
		_Module.m_server.m_cCommonFunction.ConvertPayAtPumpInfoToCardSaleAll3(cCardSaleAll3, cPAPInfo);
		memcpy( pcCardSaleAll3InOut, &cCardSaleAll3, sizeof(cCardSaleAll3) );
	}	
}

/************************************************************************/
/*                 Finalization Methods                                 */
/************************************************************************/

void CPCIEncodeHandler::CleanUp()
{
	m_cEncodingComDll.CleanUp();
	m_cDecodingComDll.CleanUp();
}


/******************************************************************************
 Description:	Remove prohibited feilds before save to DB
 Returns:      	void
 Parameters:   	source data structure
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			20/01/2009	 13:34		Start	//4.0.21.60 TD 72216
 AlexM			04/11/2013	 13:36		implement DSM	    4.0.23.2100 TD 347994 
******************************************************************************/
void	CPCIEncodeHandler::RemoveConfidencesData(PAY_AT_PUMP_INFO* pInfoInOut)
{
	CBaseEncodeHandler::RemoveConfidencesData(pInfoInOut);
	
	char	sMaskedAccount[ACCOUNT_NUM_SIZE];
	BYTE	szEncryptAccountDll[MAX_FIELD_VAL + 2] = {0}; 
	BYTE	szOrgAccount[MAX_FIELD_VAL + 2] = {0};
	
	memset(sMaskedAccount, ' ', sizeof(sMaskedAccount));
	AccountNumToParam2(pInfoInOut); 
	
	//save the original account to encrypt by the dll
	memcpy(&szOrgAccount,&pInfoInOut->CardSaleInfo.cardData.sAccountNumber,sizeof(pInfoInOut->CardSaleInfo.cardData.sAccountNumber));
	
	if (((!ChrAll((char*)szOrgAccount,ACCOUNT_NUM_SIZE,' ')) && (pInfoInOut->CardSaleInfo.cardData.sAccountNumber[0])) 
		&& (((ChrAll((char*)pInfoInOut->CardSaleExtraData4.sEncryptedAccount,ACCOUNT_NUM_SIZE,' '))) && (pInfoInOut->CardSaleExtraData4.sEncryptedAccount[0]))) 
		
	{
		long lPumpNumber = a2l((BYTE*)pInfoInOut->CardSaleInfo.cardData.sPumpNumber, sizeof(pInfoInOut->CardSaleInfo.cardData.sPumpNumber));
		EncodeRetCodeEnum lRetVal = EncryptAccount((char *)szOrgAccount,(char *)szEncryptAccountDll,lPumpNumber);
		
		if(ENCODE_OK == lRetVal)
		{
			memcpy(pInfoInOut->CardSaleExtraData4.sEncryptedAccount, szEncryptAccountDll, sizeof(pInfoInOut->CardSaleExtraData4.sEncryptedAccount)); 
		}
	}
	
	
	MaskAccount(sMaskedAccount, (char*)pInfoInOut->CardSaleInfo.cardData.sAccountNumber);	
	memcpy(pInfoInOut->CardSaleInfo.cardData.sAccountNumber, sMaskedAccount, sizeof(sMaskedAccount));
	
	memset(pInfoInOut->CardSaleInfo.cardData.sClubCardTrack2Data, ' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sClubCardTrack2Data));
	
	if(LOG_BASIC_FLOW_CONTROL)
	{			
		_Module.m_server.m_cLogger.LogMsg("CPCIEncodeHandler","RemoveConfidencesData", 0, LOG_NONE, "RemoveConfidencesData from PAY_AT_PUMP_INFO");
	}	
	
}


/******************************************************************************
 Description:	Remove prohibited feilds before save to DB
 Returns:      	void
 Parameters:   	source data structure
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			20/01/2009	 13:34		Start	//4.0.21.60 TD 72216
******************************************************************************/
void	CPCIEncodeHandler::RemoveConfidencesData(CARD_SALE_ALL3* pInfoInOut)
{	

	CBaseEncodeHandler::RemoveConfidencesData(pInfoInOut);

	char	sMaskedAccount[ACCOUNT_NUM_SIZE];
	memset(sMaskedAccount, ' ', sizeof(sMaskedAccount));

	MaskAccount(sMaskedAccount, (char*)pInfoInOut->CardSaleAll.data.sAccountNumber);	

	memcpy(pInfoInOut->CardSaleAll.data.sAccountNumber, sMaskedAccount, sizeof(sMaskedAccount));

	memset(pInfoInOut->CardSaleAll.data.sClubCardTrack2Data, ' ', sizeof(pInfoInOut->CardSaleAll.data.sClubCardTrack2Data));
    memset(pInfoInOut->CardSaleAll.data.sPINBlock,		' ', sizeof(pInfoInOut->CardSaleAll.data.sPINBlock));
	memset(pInfoInOut->extData3.sParamResult, ' ', sizeof(pInfoInOut->extData3.sParamResult)); //TD 276656 4.0.124.471
	
	if(LOG_BASIC_FLOW_CONTROL)
	{			
		_LOGMSG.LogClassMsg("CPCIEncodeHandler","RemoveConfidencesData", 0, LOG_NONE, "RemoveConfidencesData from CARD_SALE_ALL3");
	}
}


/******************************************************************************
 Description:	Remove prohibited fields from sending it to OLA. 
 Returns:      	void
 Parameters:   	source data structure
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			30/10/2010	 13:34		Start	//4.0.23.780 4.0.26.500 TD 156842 
******************************************************************************/

void	CPCIEncodeHandler::RemoveConfidencesDataFromMemory(short bySessionType, PAY_AT_PUMP_INFO* pInfoInOut)
{
	if ( SESSION_TYPE(bySessionType) == SESSION_COMPLETE && pInfoInOut->CardSaleExtraData4.byIsRemoveProhibitedData == 'Y')
	{
		memset(pInfoInOut->CardSaleInfo.cardData.sTrack1Data,	' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sTrack1Data));
		memset(pInfoInOut->CardSaleInfo.cardData.sTrack2Data,	' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sTrack2Data));


    	memset(pInfoInOut->CardSaleInfo.cardData.sPINBlock,		' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sPINBlock));
		memset(pInfoInOut->CardSaleExtraData2.sResInfoAMax37 ,	' ', sizeof(pInfoInOut->CardSaleExtraData2.sResInfoAMax37));
		memset(pInfoInOut->CardSaleInfo.cardData.sExtendPIN,	' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sExtendPIN));	 // //4.0.23.1591 TD 338379				
		memset(pInfoInOut->CardSaleExtraData3.sParamResult,		' ', sizeof(pInfoInOut->CardSaleExtraData3.sParamResult)); // //4.0.23.1591	TD 338379 
		memset(pInfoInOut->CardSaleInfo.extraData.sCustomerName, ' ', sizeof(pInfoInOut->CardSaleInfo.extraData.sCustomerName)); //4.0.23.264?
		
		_LOGMSG.LogClassMsg("CPCIEncodeHandler","CPCIEncodeHandler", 0, LOG_NONE, "Confidences data removed from the memory!");
	}
}


//4.0.23.2100 TD 347994 
void CPCIEncodeHandler::GetCustomerEncryptionKey( char* szCustomerEncryptionKey )
{
	HKEY			hKey;
	long			lRetVal = 0;	
	
	DWORD			dwValLength;
	dwValLength = MAX_FIELD_VAL;
	
	//The key is not under PumpSrv
	CString strPathName((const unsigned char*)"Software\\Retalix\\ESM");
	
	try
	{
		// Open Key
		lRetVal = RegOpenKeyEx(  
			HKEY_LOCAL_MACHINE, // handle of an open key 
			(LPCTSTR)strPathName,  // address of name of subkey to open
			0,   // reserved  REGSAM samDesired, 
			KEY_WRITE|KEY_READ,// security access mask
			&hKey);   // address of handle to open key); 
		
		if(lRetVal == ERROR_SUCCESS) // success to open the key.
		{		
			lRetVal = RegQueryValueEx(
				hKey,
				"CEK",
				0,
				NULL,
				(unsigned char *)szCustomerEncryptionKey,
				&dwValLength);
		}
		
		if(lRetVal != ERROR_SUCCESS)
		{
			memset(szCustomerEncryptionKey,0,dwValLength);
		}
		
		RegCloseKey(hKey);
	}
	
	catch(...)
	{
		memset(szCustomerEncryptionKey,0,dwValLength);
		if (LOG_LOGIC_ERROR )
		{
			_Module.m_server.m_cLogger.LogClassMsg("CPCIEncodeHandler", "GetCustomerEncryptionKey", 0, LOG_PUMP, "GetCustomerEncryptionKey Failed!",LOG_LEVEL_1);			
		}
	}
	
}

