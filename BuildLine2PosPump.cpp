// BuildLine2PosPump.cpp: implementation of the CBuildLine2PosPump class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BuildLine2PosPump.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuildLine2PosPump::CBuildLine2PosPump():m_lTempleteLen(0)
{
	Clear();		
}

CBuildLine2PosPump::CBuildLine2PosPump(const PosPumpCardsInfo & CardData )
{
	m_CardData = CardData;
	Init();
	//strcpy(m_sTemplateLine , "[1,8,__sCardName] [10,12,__sTruckingCompanyName] [35,12,__sAccountNumber~5]");  //debug.
//	strcpy(m_sTemplateLine , "[1,8,__sCardName] [10,8,__sTruckingCompanyName] [19,24,__sAccountNumber~4][26,5,Truk=] [32,10,__sFullVehicleId]");  //debug.
	
}

CBuildLine2PosPump::~CBuildLine2PosPump()
{

}

void CBuildLine2PosPump::Clear()
{
	memset(&m_CardData , ' ' , sizeof(m_CardData));
	memset(m_sTemplateLine , ' ' , sizeof(m_sTemplateLine));
}


/******************************************************************************
 Description: Merge row data and template line
 Returns:     None
 Parameters:  sBufferTmpOut -  return Formated line. 
			  lLen -buffer length

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			01/30/2008   08:01		Start
******************************************************************************/

void CBuildLine2PosPump::Generate(char  *sBufferTmpOut , long lLen)const
{	
	for (int i = 0; i < m_lTempleteLen ;i++)
	{
		switch(m_sTemplateLine[i])
		{
		case ' ':
			break;

		case '[':
			ParseLableString(&m_sTemplateLine[i] , sBufferTmpOut);
			break;

		case ']':
			break;

		}
	}

	for(int i = lLen-1; i >= 0 ; i--)
	{
		if (sBufferTmpOut[i] != ' ' )
		{
			sBufferTmpOut[i] = 0;
			break;
		}
	}
}


/******************************************************************************
 Description: Parse template line  
 Returns:     0
 Parameters:  strFormat  - template line.
			  strOut     - retrun Text line 	


 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			01/30/2008   08:01		Start
******************************************************************************/

long CBuildLine2PosPump::ParseLableString(LPCSTR strFormat, char  * strOut) const
{
	LPCSTR p = strFormat;
	LPCSTR n = NULL;
	long lPos = 0;
	long lSize = 0;
	p++;

	if (p)
	{
		n = strchr(p, ',');

		lPos = a2l((unsigned char *)p , n - p) ; //
		
		n++;
		p = strchr(n, ',');

		if (p)
		{
			lSize = a2l((unsigned char *)n , p - n);
			p++;
			
			if (!memcmp(p , "__" ,2))  // param
			{
				p += 2;
				GetParamData(lPos,lSize, p ,strOut);
			}
			else 
			{
				// string found.
				GetParamData(lPos,lSize, p ,strOut);
			}
		}
	}
 
	return 0;
}

/******************************************************************************
 Description: Retrieved data according to template field
 Returns:     0
 Parameters:  long lPos		  - String offset		
			  long lSize	  - String size.	 
			  char *p,        - Template line 
			  char *sLine	  - Return Formated line


 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			01/30/2008   08:01		Start
******************************************************************************/

long CBuildLine2PosPump::GetParamData(long &lPos, long &lSize, LPCSTR p, char *sLine) const
{
	char sTmpbuff[MAX_FIELD_VAL ] = {0};
	long lFuncOps = 0;
	long lFiledSize = 0;
	BOOL bParam =  TRUE;

	if (p)
	{
		if (!memcmp(p ,	"sCardName" , strlen("sCardName")))
		{
			if (ChrAll(sLine + lPos - 1, lSize, ' '))
				memcpy(sTmpbuff , m_CardData.sCardName ,min(lSize, sizeof(m_CardData.sCardName)));

			lFiledSize = sizeof(m_CardData.sCardName);
			lFuncOps  = strlen("sCardName");
		}
		else if (!memcmp(p ,	"sTruckingCompanyName" , strlen("sTruckingCompanyName")))
		{
			if (ChrAll(sLine + lPos - 1, lSize, ' '))
				memcpy(sTmpbuff , m_CardData.sTruckingCompanyName ,min(lSize, sizeof(m_CardData.sTruckingCompanyName)));

			lFiledSize = sizeof(m_CardData.sTruckingCompanyName);
			lFuncOps  = strlen("sTruckingCompanyName");
		}
		else if (!memcmp(p ,	"sFullVehicleId" ,  strlen((char *)"sFullVehicleId")))
		{
 			if (ChrAll(sLine + lPos - 1, lSize, ' '))
				memcpy(sTmpbuff , m_CardData.sFullVehicleId ,min(lSize, sizeof(m_CardData.sFullVehicleId)));
				//memcpy(sLine + lPos - 1, m_CardData.sAccountNumber , min(lSize, sizeof(m_CardData.sAccountNumber)));

			lFiledSize = sizeof(m_CardData.sFullVehicleId);
			lFuncOps  = strlen("sFullVehicleId");
		}		
		else if (!memcmp(p ,	"sAccountNumber" ,  strlen((char *)"sAccountNumber")))
		{
 			if (ChrAll(sLine + lPos - 1, lSize, ' '))
				memcpy(sTmpbuff , m_CardData.sAccountNumber ,min(lSize, sizeof(m_CardData.sAccountNumber)));
				//memcpy(sLine + lPos - 1, m_CardData.sAccountNumber , min(lSize, sizeof(m_CardData.sAccountNumber)));

			lFiledSize = sizeof(m_CardData.sAccountNumber);  
			lFuncOps  = strlen("sAccountNumber");
		}
		else if (!memcmp(p ,	"sAttendantID" ,  strlen((char *)"sAttendantID"))) //4.0.22.10 TD 71055
		{
 			if (ChrAll(sLine + lPos - 1, lSize, ' '))
				memcpy(sTmpbuff , m_CardData.sAttendantID ,min(lSize, sizeof(m_CardData.sAttendantID)));

			lFiledSize = sizeof(m_CardData.sAttendantID);  
			lFuncOps  = strlen("sAttendantID");
		}		
		else if (!memcmp(p ,	"sAttendantName" ,  strlen((char *)"sAttendantName"))) //4.0.22.11 TD 71055
		{
 			if (ChrAll(sLine + lPos - 1, lSize, ' '))
				memcpy(sTmpbuff , m_CardData.sAttendantName ,min(lSize, sizeof(m_CardData.sAttendantName)));

			lFiledSize = sizeof(m_CardData.sAttendantName);  
			lFuncOps  = strlen("sAttendantName");
		}
		else  
		{
			LPCSTR pLable = NULL;
			bParam = FALSE;

			pLable = strchr(p, ']');

			if (ChrAll(sLine + lPos - 1, lSize, ' ') && pLable)
			{

				lFiledSize = pLable  - p;
				lFuncOps  = 0;

 				if (ChrAll(sLine + lPos - 1, lSize, ' ') && (lFiledSize > 0 ))
					memcpy(sTmpbuff , p , pLable  - p);
			}
		}

		if (ChrAll(sLine + lPos - 1, lSize, ' '))
		{
			if ((bParam) && (p[lFuncOps] == '~'))
			{
				long lLastChar = a2l((unsigned char *)&p[lFuncOps + 1] , 1);
				int m = min(lSize, lFiledSize) - 1 ;
				for (m; m >=0 ;m--)
				{
					if (sTmpbuff[m] != ' ')
					{
						break;
					}
				}

				if (m > lLastChar)
				{
					memcpy(sLine + lPos - 1, &sTmpbuff[m -  lLastChar + 1] , lLastChar);	 		 
				}
				else
				{
					memcpy(sLine + lPos - 1, sTmpbuff , lLastChar);			
				}
			}
			else
				memcpy(sLine + lPos - 1, sTmpbuff , min(lSize, lFiledSize));
		}
	}

	return 0;
}


/******************************************************************************
 Description: initialize buffer
 Returns:     0
 Parameters:  lLen - language

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			01/30/2008   08:01		Start
******************************************************************************/

void CBuildLine2PosPump::Init(long lLen)
{
	TStaticString<STATIC_STRING_8> strLangName;
	//TStaticString<STATIC_STRING_64> sTmpStr;
	
	long lFieldSize = 0;
	
	CString strReg;
	char  sTmpStr[MAX_FIELD_VAL]= {0};
	memset(sTmpStr ,  0 , sizeof(sTmpStr));

	m_lTempleteLen  = 0 ;


	strReg.Format("Languages\\Language%02d" ,lLen);
	strcpy_s(strLangName, sizeof(strLangName), "");
	
	//_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "POSPumpOpenStr",(char *)((LPCSTR)sTmpStr), MAX_FIELD_VAL, (char *)strLangName,TRUE);
	_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "POSPumpOpenStr",(char *)sTmpStr, 256, (char *)strLangName,TRUE);

	//Example POSPumpOpenStr = [1,15,sAttendantName]
	
	 lFieldSize = strlen(sTmpStr);
	

	if ((lFieldSize) && (lFieldSize <= MAX_FIELD_VAL))
	{
		memset(m_sTemplateLine , ' ' , sizeof(m_sTemplateLine));
		memcpy(m_sTemplateLine , sTmpStr , lFieldSize);
		m_lTempleteLen  = lFieldSize;

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;

			str.Format("Loaded templete message  %ld  - %s-" , lLen ,sTmpStr);
			_LOGMSG.LogMsg(str);
		}		
	}
}


void CBuildLine2PosPump::GetFormattedLine(CPumpTransact & trs, char *sLine, long lLen)
{
	char sTmpBuff[MAX_FIELD_VAL];
	BOOL bFound =  FALSE;  
	CPumpTransact cTmpTrs = trs;
	long lRtc = 0;

	memset(sTmpBuff , ' ' , sizeof(sTmpBuff));
	memset(&m_CardData , ' ' , sizeof(m_CardData));
	memset(sLine , ' ' , lLen);
	

	//4.0.22.10 - TD 71055
	//Copy relevant data from CPumpTransact
	if (!ChrAll((char *)cTmpTrs.m_sAttendantName , sizeof(cTmpTrs.m_sAttendantName)))
	{			
		memcpy(m_CardData.sAttendantName ,cTmpTrs.m_sAttendantName , sizeof(cTmpTrs.m_sAttendantName));
		bFound = TRUE;
	}
	
	if (!ChrAll((char *)cTmpTrs.m_sFullVehicleId , sizeof(cTmpTrs.m_sFullVehicleId)))
	{			
		memcpy(m_CardData.sFullVehicleId, cTmpTrs.m_sFullVehicleId , sizeof(cTmpTrs.m_sFullVehicleId));
		bFound = TRUE;
	}
	

	if ((cTmpTrs.m_lResExt == RES_EXT_FLEET) && (cTmpTrs.m_lRes == PRE_PAY_AT_POS))
	{
		EXTRA_INFO_REC1  cExtraInfoRec;
		BOOL	bBalanceTrs =FALSE;

		cExtraInfoRec.info.cDieselInfo.lIndexNumber = cTmpTrs.m_lNumber;
		lRtc = _Module.m_server.IndexOperation(EXTRA_INFO_FILE ,REC_READ, &cExtraInfoRec);

		if (!lRtc)	
		{
			long lLenTmp = 0;

			lLenTmp = 	min(lLen , sizeof(cExtraInfoRec.info.cDieselInfo.sFormatedLine));
			memcpy(sLine , cExtraInfoRec.info.cDieselInfo.sFormatedLine , lLenTmp);
			//bFound = TRUE;
		}
	}
	else if ((cTmpTrs.m_lRes == PAY_AT_KIOSK) && (cTmpTrs.m_lResExt == RES_EXT_FLEET)) //4.0.8190.0
	{
		PAY_AT_PUMP_INFO cPatAtPumpInfo;   // PAK - with fleet.		

		lRtc = _Module.m_server.IndexOperation(PUMP_OLA_TEMP_FILE , REC_READ, &cTmpTrs ,&cPatAtPumpInfo);

		if (!lRtc)	
		{
			memcpy(m_CardData.sAccountNumber ,cPatAtPumpInfo.CardSaleInfo.cardData.sAccountNumber , sizeof(m_CardData.sAccountNumber));
			memcpy(m_CardData.sCardName ,cPatAtPumpInfo.CardSaleInfo.cardData.sCardName , sizeof(m_CardData.sCardName));
			memcpy(m_CardData.sFullVehicleId ,cPatAtPumpInfo.CardSaleInfo.cardData.sFullVehicleId , sizeof(m_CardData.sFullVehicleId));
			memcpy(m_CardData.sTruckingCompanyName ,cPatAtPumpInfo.CardSaleExtraData3.sTruckingCompanyName , sizeof(m_CardData.sTruckingCompanyName));
			bFound = TRUE;
		}
	}
	else if (
		     (cTmpTrs.m_lRes == PAY_AT_PUMP) && 
			 (cTmpTrs.m_lResExt == RES_EXT_FLEET) && 
			 (cTmpTrs.m_lStatus != PAID)) 
	{
			PAY_AT_PUMP_INFO info; 
				
			memset(&info, ' ' , sizeof(info));
			if (cTmpTrs.m_lPump)   
				_Module.m_server.m_cPumpArray[cTmpTrs.m_lPump - 1].GetPAPInfo(&info);

			memcpy(m_CardData.sAccountNumber ,info.CardSaleInfo.cardData.sAccountNumber , sizeof(m_CardData.sAccountNumber));
			memcpy(m_CardData.sCardName ,info.CardSaleInfo.cardData.sCardName , sizeof(m_CardData.sCardName));
			memcpy(m_CardData.sFullVehicleId ,info.CardSaleInfo.cardData.sFullVehicleId , sizeof(m_CardData.sFullVehicleId));
			memcpy(m_CardData.sTruckingCompanyName ,info.CardSaleExtraData3.sTruckingCompanyName , sizeof(m_CardData.sTruckingCompanyName));
			bFound = TRUE;
	}
	
	if (bFound)
	{
		Generate(sTmpBuff , sizeof(sTmpBuff));
		if (strlen(sTmpBuff))
			memcpy(sLine, sTmpBuff, min((long)strlen(sTmpBuff), lLen));

	}
}
