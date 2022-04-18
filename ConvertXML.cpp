#include "stdafx.h"

//#include "convertXML.h"

CConvertXML::CConvertXML()
{

}

//4.0.12.501
/******************************************************************************
 Description:Insert the XML into QDX
 1.Convert to char *.
 2.the max value is 3K so we find the number charecter and insert into tmp ola
 Returns:      
 Parameters: 
				
 //4.0.10.10
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			09/01/2006				Start
******************************************************************************/
long CConvertXML::InsertXMLReceptIntoOlaTempQDX(BSTR & bstrInXML,CPumpTransact & trs,long lPumpNumber)
{
	long		lRetCode=TRUE,lNumOfRows=0,lMaxOfBytes=0,lTmpNumOfBytes=0;
	PREPAY_INFO cTmpPrePayInfo;
	XML_BUFFER  cTmpXmlBuff;
	char		strXML[MAX_XML_BUF];
	CString		str;
	long		lTmpBytes=0;

	CComBSTR bstrTmp = bstrInXML;//this convert is to know what the size of the XML.

	try
	{
	
		if(bstrTmp.Length()>MAX_XML_BUF)
		{
			str.Format("InsertXMLIntoQDX The size of the xml %ld is bigger than max size %ld trs %ld.",
				bstrTmp.Length(),MAX_XML_BUF,trs.m_lNumber);
			lRetCode = FALSE;
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
		}
		else
		{ 
			if(bstrTmp.Length()>0)
			{
				memset(&cTmpPrePayInfo,' ',sizeof(cTmpPrePayInfo));
				cTmpPrePayInfo.sXML1.lNumOfRows =0;
				cTmpPrePayInfo.sXML2.lNumOfRows =0;
				cTmpPrePayInfo.sXML3.lNumOfRows =0;
				memset(&strXML,' ',sizeof(strXML));
				memset(&cTmpXmlBuff,' ',sizeof(cTmpXmlBuff));

				lMaxOfBytes = WideCharToMultiByte(CP_ACP,0,bstrInXML,-1,strXML,sizeof(strXML),NULL,NULL);
				lNumOfRows = lMaxOfBytes/1000;
				if(lMaxOfBytes)
					lNumOfRows++;
				str.Format("InsertXMLIntoQDX The size of the xml is %ld number of rows = %ld that save in QDX . trs %ld.",lMaxOfBytes,lNumOfRows,trs.m_lNumber);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str,trs.m_lNumber);
				
				for(int i=0;i<lNumOfRows;i++)
				{
					lTmpNumOfBytes = lMaxOfBytes > QDX_XML_LEN ? QDX_XML_LEN : lMaxOfBytes;
					lMaxOfBytes -=lTmpNumOfBytes;
					cTmpXmlBuff.lIdex = trs.m_lNumber;
					cTmpXmlBuff.lNumOfRows = lTmpNumOfBytes;
					memcpy(cTmpXmlBuff.sXML,strXML+lTmpBytes,lTmpNumOfBytes);
					if(i==0)
						memcpy(&cTmpPrePayInfo.sXML1,&cTmpXmlBuff,sizeof(cTmpXmlBuff));
					else if(i==1)
						memcpy(&cTmpPrePayInfo.sXML2,&cTmpXmlBuff,sizeof(cTmpXmlBuff));
					else
						memcpy(&cTmpPrePayInfo.sXML3,&cTmpXmlBuff,sizeof(cTmpXmlBuff));
					memset(&cTmpXmlBuff,' ',sizeof(cTmpXmlBuff));
					lTmpBytes+=lTmpNumOfBytes;
					

				}
			}
			else
			{
				if(LOG_BASIC_FLOW_CONTROL)
				{
					str.Format("InsertXMLIntoQDX The size of the xml is 0. trs %ld.",trs.m_lNumber);
					_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
				}
			}
			
			lRetCode = _Module.m_server.IndexOperation(PUMP_OLA_TEMP_FILE,REC_INSERT, &trs, &cTmpPrePayInfo,1);
			if(LOG_BASIC_FLOW_CONTROL)
			{
				str.Format("InsertXMLIntoQDX Finish insert XML to Ola Temp DQX Result code = %ld trs %ld",lRetCode,trs.m_lNumber);
				_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
			}
			if(!lRetCode)
				lRetCode = TRUE;


		}
	}catch(...)
	{
		str.Format("InsertXMLIntoQDX Had UnExcpcted Error trs %ld",trs.m_lNumber);
		_LOGMSG.LogMsg(str);
	}

	return lRetCode;
}

/******************************************************************************
 Description:Insert the XML into QDX
 1.Convert to char *.
 2.the max value is 3K so we find the number charecter and insert into tmp ola
 Returns:      
 Parameters: 
				
 //4.0.10.10
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			09/01/2006				Start
******************************************************************************/
long CConvertXML::ReadXMLFromQDX(CComBSTR & bstrOutXML,CPumpTransact & trs,long lPumpNumber)
{
	long		lRetCode=0,lNumOfRows=0,lMaxOfBytes=0,lTmpNumOfBytes=0;
	PREPAY_INFO cTmpPrePayInfo;
	char		strXML[MAX_XML_BUF];
	CString		str;
	long		lTmp=0;
	CComBSTR	bstrInXML;

	try
	{
	
		memset(&cTmpPrePayInfo,' ',sizeof(cTmpPrePayInfo));
		memset(&strXML,' ',sizeof(strXML));
		lRetCode = _Module.m_server.IndexOperation(PUMP_OLA_TEMP_FILE , REC_READ, &trs ,&cTmpPrePayInfo); //4.0.10.49
		
		if(lRetCode)
		{
			lRetCode = FALSE;
			str.Format("ReadXMLFromQDX Try to read XML From QDX but Can't found trs %ld ",trs.m_lNumber);
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
		}
		else
		{
			long lTmp = cTmpPrePayInfo.sXML2.lNumOfRows+cTmpPrePayInfo.sXML1.lNumOfRows+cTmpPrePayInfo.sXML1.lNumOfRows;
			memcpy(strXML,cTmpPrePayInfo.sXML1.sXML,cTmpPrePayInfo.sXML1.lNumOfRows);// The first line
			memcpy(strXML+cTmpPrePayInfo.sXML1.lNumOfRows,cTmpPrePayInfo.sXML2.sXML,cTmpPrePayInfo.sXML2.lNumOfRows);// The 2 line
			memcpy(strXML+cTmpPrePayInfo.sXML2.lNumOfRows+cTmpPrePayInfo.sXML1.lNumOfRows,cTmpPrePayInfo.sXML3.sXML,cTmpPrePayInfo.sXML3.lNumOfRows);// The 3 line
			bstrOutXML.Empty();
			bstrOutXML.Append(strXML);
//			FILE *fp;
/*	if ( ( fp = fopen("C:\\amram.xml", "w+") ) != NULL )
	{
		fprintf(fp,"%s",strXML); 
		fclose (fp);
	}*/
			//long lLen = MultiByteToWideChar(CP_ACP,0,strXML,strlen(strXML),bstrInXML,0);	
			str.Format("ReadXMLFromQDX Finish Read XML From DQX trs %ld ",trs.m_lNumber,lTmp);
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
				
		}

	}catch(...)
	{
		str.Format("ReadXMLFromQDX Had UnExcpcted Error trs %ld",trs.m_lNumber);
		_LOGMSG.LogMsg(str);
	}
	return lRetCode;
}

	
