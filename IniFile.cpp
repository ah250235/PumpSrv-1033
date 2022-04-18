// IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile()
{
	SetFileName();
}

CIniFile::~CIniFile()
{

}
/******************************************************************************
 Description: Generate Run time parameters file name include his path 	 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			28/2/2005   16:11		Start
******************************************************************************/

void CIniFile::SetFileName()
{
	BYTE szFilePath[MAX_FILE_PATH] = {0};
	BYTE sTmpDefault[MAX_FILE_PATH]={0};	
	DWORD dwRes;

	memset(szFilePath , 0 ,sizeof(szFilePath));
	sprintf((char *)szFilePath, "c:\\");

	memset(sTmpDefault,0,MAX_FIELD_NAME);
	
	dwRes = GetCurrentDirectory(MAX_FIELD_NAME,(char *)sTmpDefault);

	
	strcat((char *)sTmpDefault,"\\Data");  //4.0.0.85

	_Module.m_server.m_cParam.LoadParam("Paths\\Long", "DataDir",(char *)szFilePath,sizeof(szFilePath),(char *)sTmpDefault,TRUE);

	m_sFileName.Format("%s\\%s" ,szFilePath , INI_FILE_NAME);
	
}
/******************************************************************************
 Description:	Write string to ini file 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			28/2/2005   17:59		Start
******************************************************************************/

long CIniFile::WriteParam(char *sSection, char *sFieldName, char *sVal, long lFlags, BOOL bCreate)
{
	char sTmpFiledName [MAX_FIELD_VAL * 4];
	sprintf(sTmpFiledName,"%s%s", sFieldName , "[S]");

	long lLen = WritePrivateProfileString(sSection,sTmpFiledName,sVal,m_sFileName);

	return lLen;
}

/******************************************************************************
 Description:	Read string from INI file 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			28/2/2005   17:59		Start
******************************************************************************/

long CIniFile::ReadParam(char *sSection, char *sFieldName, char *sVal, long lLen ,long lFlags, BOOL bCreate)
{		
	char sTmpFiledName [MAX_FIELD_VAL * 4];
	sprintf(sTmpFiledName,"%s%s", sFieldName , "[S]");
	char sTmpVal [MAX_FIELD_VAL * 4];
	char *sTmpDefult = "\\\\";
	memset(sTmpVal , 0 , sizeof(sTmpVal));
		
	
	long lLenBuff = GetPrivateProfileString((LPCSTR)sSection,sTmpFiledName,(LPCSTR)sTmpDefult,sTmpVal,lLen,m_sFileName);

	if (bCreate)
	{
		if ((!lLenBuff) || (!lstrcmp(sTmpVal ,sTmpDefult))) 
		{
			CString sMsg;
			DWORD dwLastErr = GetLastError();

			sMsg.Format("Parameter %s not found in RunTime file (Error %ld). Param created with default value:%s", sFieldName, dwLastErr, sVal);
			_LOGMSG.LogMsg(sMsg);
			lLen = WriteParam(sSection, sFieldName, sVal, lFlags,bCreate);		
		}
		else
			strcpy(sVal ,sTmpVal);
	}
	else
	{
		strcpy(sVal ,sTmpVal);
	}
	


	return lLenBuff;
}
/******************************************************************************
 Description:	write DWORD to ini file 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			28/2/2005   17:59		Start
******************************************************************************/

long CIniFile::WriteParam(char *sSection, char *sFieldName, DWORD *dwVal, long lFlags, BOOL bCreate)
{
	char sVal[MAX_FIELD_VAL];
	char sTmpFiledName [MAX_FIELD_VAL * 4];
	sprintf(sTmpFiledName,"%s%s", sFieldName , "[I]");

	memset(sVal , 0, sizeof(sVal));

	sprintf(sVal , "%ld",*dwVal);
	long lLen = WritePrivateProfileString(sSection,sTmpFiledName,sVal,m_sFileName);		

	return lLen;
}
/******************************************************************************
 Description:	Read DWORD from ini file 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			28/2/2005   17:59		Start
******************************************************************************/

long CIniFile::ReadParam(char *sSection, char *sFieldName, DWORD *dwVal, long lFlags, BOOL bCreate)
{
	char  sVal[MAX_FIELD_VAL];
	memset(sVal , 0, sizeof(sVal));
	char sTmpFiledName [MAX_FIELD_VAL * 4];
	sprintf(sTmpFiledName,"%s%s", sFieldName , "[I]");
	

	//long lLen = GetPrivateProfileString((LPCSTR)sSection,sFieldName,(LPCSTR)sVal,sVal ,MAX_FIELD_VAL,m_sFileName);
	long lLen = GetPrivateProfileString((LPCSTR)sSection, sTmpFiledName,(LPCSTR) sVal, sVal, MAX_FIELD_VAL, m_sFileName);

	if (lLen)
	{
		*dwVal = a2l((unsigned char *)sVal,min(strlen(sVal), STR_LONG_VAL10));		

	}
	else
	{
		DWORD dwLastErr = GetLastError();
		if (bCreate)
			//lLen = WriteParam(sSection, sFieldName, dwVal, lFlags,bCreate);
			lLen = WriteParam(sSection, sFieldName, dwVal, lFlags, bCreate);

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString sMsg;
			sMsg.Format("Parameter %s not found in RunTime file (Error %ld). Len = %ld", sFieldName, dwLastErr, lLen);
			_LOGMSG.LogMsg(sMsg);
		}
	}

	return lLen;
}

void CIniFile::ResetIniParameters()
{

	DWORD dwVal = 1;
	_Module.m_server.m_cParam.SaveParam("GeneralParam","ActiveReadNumber",dwVal ,TRUE,0,0,0,TRUE);
	_Module.m_server.m_cParam.SaveParam("GeneralParam","ActiveShiftNumber",dwVal,TRUE,_HOST.m_lGroupNum,0,0,TRUE);

	DWORD dwInitTrsNumber = _Module.m_server.GetFuelTransactionNumberMinValue(); //4.0.23.260 111318
	_Module.m_server.m_cParam.SaveParam("GeneralParam","TransactNumber",dwInitTrsNumber , TRUE,TRUE,0,0,TRUE); 
	_Module.m_server.m_cParam.SaveParam("GeneralParam","ShiftsToMaintenance",dwVal,TRUE,TRUE,0,0,TRUE);
	_Module.m_server.m_cParam.SaveParam("GeneralParam","InMaintenanceQdxNum",dwVal,TRUE,TRUE,0,0,TRUE);


	_Module.m_server.m_cParam.SaveParam("GeneralParam","OLAIndex",dwVal,TRUE,0,0,0,TRUE); //4.0.8.503

	_Module.m_server.m_cParam.SaveParam("GeneralParam","PumpTotalIndex",dwVal,TRUE,0,0,0,TRUE); //4.0.8.503

	_Module.m_server.m_cParam.SaveParam("GeneralParam","TankReadIndex",dwVal,TRUE,0,0,0,TRUE); //4.0.8.503
	_Module.m_server.m_cParam.SaveParam("GeneralParam","FuelDeliveryIndex",dwVal,TRUE,0,0,0,TRUE); //4.0.8.503
	_Module.m_server.m_cParam.SaveParam("GeneralParam","AlarmIndex",dwVal,TRUE,0,0,0,TRUE); //4.0.8.503
	_Module.m_server.m_cParam.SaveParam("GeneralParam","CarWashIndex",dwVal,TRUE,0,0,0,TRUE); //4.0.8.503
	_Module.m_server.m_cParam.SaveParam("GeneralParam","ItemsIndex ",dwVal,TRUE,0,0,0,TRUE); //4.0.8.503
	
	_Module.m_server.m_cParam.SaveParam("GeneralParam","PrevShiftLastTrsNum",dwVal,TRUE,0,0,0,TRUE); //4.0.20.52
	_Module.m_server.m_cParam.SaveParam("GeneralParam", "SignInCashiersCounter", (DWORD)0, TRUE, 0, 0, 0, TRUE); //4.0.22.506 TD 70956
	_Module.m_server.m_cParam.SaveParam("GeneralParam", "PriceChangeBatchNumber", (DWORD)0, TRUE, 0, 0, 0, TRUE); //4.0.27.43

	for(int i=1; i <= MAX_PUMPS_64; i++)  
	{
		CString str("Pumps\\Pump");
		char sNumber[8];
		memset(sNumber,0,sizeof(sNumber));
		sprintf(sNumber,"%02d",i);
		str += sNumber;				
		_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str),"UnbalancePrePayCounter",(DWORD)0,FALSE,_HOST.m_lGroupNum,0,i,TRUE);//4.0.3.27 //4.0.8.505
		_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str),"PrpDeclienNumber",(DWORD)0,FALSE,_HOST.m_lGroupNum,0,i,TRUE);//4.0.12.502
	}

	
}
