// Param.cpp: implementation of the CParam class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PumpSrv.h"
#include "Param.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParam::CParam()
{
	InitializeCriticalSection(&m_csParamDB);
}

CParam::~CParam()
{
	DeleteCriticalSection(&m_csParamDB);
}


// 0 - OK
// 1 - Cant Open Key, Param set to default 
// 2 - Not Found, Param created with default value
// 3 - Not Found, Cant create new param , Param set to default
// 4 - Not Found, Didn't create, according to request
int CParam::LoadParam(const char *pszSection,const char *pszName, char *pszVal, DWORD dwLen , char *pszDefaultVal,BOOL bCreate,BOOL bWriteToLog,long lGroupNum,long lFlags ,BOOL bIniFiled)  //4.0.8.503
{
	int		iRetVal = OK;

	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csParamDB);
	
	if(pszVal != pszDefaultVal)
	{
		memset(pszVal, 0, dwLen);
		sprintf(pszVal, "%s", pszDefaultVal);
	}

	if (bIniFiled)   
	{			
		_Module.m_server.m_cIniFile.ReadParam((char *)pszSection, (char *)pszName, pszVal, dwLen);
	}
	else
	{
		DWORD dwDefaultSize = strlen(pszDefaultVal) + 1;
		iRetVal = ReadParam((char *)pszSection, (char *)pszName, REG_SZ, (unsigned char*)pszVal, dwLen, (unsigned char*)pszDefaultVal, dwDefaultSize, bCreate, bWriteToLog);
	}

	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csParamDB);

	return (iRetVal);
}


// 0 - OK
// 1 - Cant Open Key, Param set to default 
// 2 - NOt Found, Param created with default value
// 3 - NOt Found, Cant create new param , Param set to default
// 4 - NOt Found, Didn't create, accoording to request
int CParam::LoadParam(const char *pszSection, const char *pszName, DWORD *pVal, DWORD dwDefaultVal, BOOL bCreate, BOOL bWriteToLog, long lGroupNum, long lFlags, BOOL bIniFiled)
{
	ParamRetCodeEnum	eRetCode = PARAM_OK;

	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csParamDB);
	
	*pVal = dwDefaultVal;

	if (bIniFiled)   
	{			
		_Module.m_server.m_cIniFile.ReadParam((char *)pszSection, (char *)pszName, pVal, 0);
	}
	else
	{
		eRetCode = ReadParam((char *)pszSection, (char *)pszName, REG_DWORD, (unsigned char*)pVal, sizeof(DWORD), (unsigned char*)&dwDefaultVal, sizeof(DWORD), bCreate, bWriteToLog);
	}

	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csParamDB);

	return (eRetCode);
}

// return TRUE if the entry is valid
BOOL CParam::CheckValidEntry(char *pszSection,long lFlags)
{
	HKEY			hKey;
	long			lRetVal =0;
	DWORD			dwValLength;
	DWORD			dwVal;
	BOOL			bValidEntry = FALSE;
	BOOL			bWorkLocal = TRUE;
	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csParamDB);
	

	dwValLength = sizeof(DWORD);
	
	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pCFGDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
	{
		CString strPathName((const unsigned char*)"Software\\PointOfSale\\PumpSrv");
		CString cSection(pszSection);
		CString cName("ValidEntry");
		BSTR    bstrServerPath = strPathName.AllocSysString();
		BSTR	bstrSection = cSection.AllocSysString();
		BSTR	bstrName = cName.AllocSysString();
		long	lVal = 0;
		
		bWorkLocal = FALSE;
		try
		{
			lRetVal = _HOST.m_pCFGDB->LoadNumParam(bstrServerPath,bstrSection,bstrName,
													FALSE,0,0,0,0,0,&lVal);
		}
		catch(_com_error & e )
		{
			_LOGMSG.LogResult(
				0, // Src
				"ICFGDB",
				"LoadNumParam",
				FALSE,0,
				TRUE,e.Error(),e);

			lRetVal = 3;
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();
		}
		catch(...)
		{
			if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICFGDB",
					"LoadNumParam",
					TRUE,0,
					(lRetVal!=0)? TRUE:FALSE,lRetVal);

			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();			
		}
		
		//checking the result
		if ( lRetVal != ERROR_SUCCESS )
		{
			bValidEntry = FALSE;
		}
		else
		{
			if(lVal == 1)
				bValidEntry = TRUE;
			else
				bValidEntry = FALSE;
		}
		//freeing all the bstrs
		if(bstrServerPath)
			SysFreeString(bstrServerPath);
		if(bstrSection)
			SysFreeString(bstrSection);
		if(bstrName)
			SysFreeString(bstrName);
	
	}
	if(bWorkLocal)
	{
		CString strPathName((const unsigned char*)"Software\\PointOfSale\\PumpSrv");
		if(pszSection)
		{
			strPathName += "\\";
			strPathName += pszSection;
		}
			

		// Open Key
		lRetVal = RegOpenKeyEx(  
						HKEY_LOCAL_MACHINE, // handle of an open key 
						(LPCTSTR)strPathName,  // address of name of subkey to open
						0,   // reserved  REGSAM samDesired, 
						KEY_WRITE|KEY_READ,// security access mask
						&hKey);   // address of handle to open key); 


		if(lRetVal == ERROR_SUCCESS)
		{
			lRetVal = RegQueryValueEx(
					hKey,
					"ValidEntry",
					0,
					NULL,
					(unsigned char *)&dwVal,
					&dwValLength);

			if(lRetVal != ERROR_SUCCESS)
			{			
				bValidEntry = FALSE;
			}	
			else if( dwVal == 1)
			{
				bValidEntry = TRUE;
			}
			else
			{
				bValidEntry = FALSE;
			}
		}	
		else
		{
			bValidEntry = FALSE;
		}

		if((LOG_BASIC_FLOW_CONTROL)&&(!bValidEntry))
		{
			CString str;
			str.Format("Entry - %s, not valid  !",pszSection);

			//Eyal 4.0.0.62 changed log writing so that there is no
			//output to the console.
			_LOGMSG.LogMsg("Param","CheckValidEntry",0,LOG_NONE,str , DEFAULT_LOG_LEVEL , FALSE);
		}

		RegCloseKey(hKey);
	}

	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csParamDB);


	return (bValidEntry);
}


// 0 - OK
// 1 - Cant Open Open Key.
// 2 - Cant Find field
int CParam::SaveParam(char *pszSection, char *pszName, DWORD dwVal, BOOL bCreate, long lGroupNum, long lFlags, long lClientEventData, BOOL bIniFiled)	//4.0.8.503
{
	int	iRetCode = 0;

	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csParamDB);
	
	// Save to local
	if (bIniFiled)							//.??? TD 72545
	{
		_Module.m_server.m_cIniFile.WriteParam((char *)pszSection, (char *)pszName, &dwVal, 0);
	}
	else
	{
		iRetCode = WriteParam((char *)pszSection, (char *)pszName, REG_DWORD, (unsigned char*)&dwVal, sizeof(dwVal), bCreate);
	}

	// Save to remote only through Hydra 
	PassParamToHydra((char *)pszSection, (char *)pszName, dwVal, NULL, bCreate, lGroupNum, lFlags, lClientEventData, bIniFiled);
	
	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csParamDB);

	return (iRetCode);
}


// 0 - OK
// 1 - Cant Open Open Key.
// 2 - Cant Find field
int CParam::SaveParam(char *pszSection, char *pszName, char *pszVal, BOOL bCreate, long lGroupNum, long lFlags, long lClientEventData, BOOL bIniFiled)
{
	int	iRetCode = 0;
	
	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csParamDB);

	// Save to local
	 if (bIniFiled)							//.??? TD 72545
	{
		_Module.m_server.m_cIniFile.WriteParam((char *)pszSection, (char *)pszName, pszVal, 0);
	}
	else
	{
		iRetCode = WriteParam((char *)pszSection, (char *)pszName, REG_SZ, (unsigned char*)pszVal, strlen(pszVal), bCreate);
	}

	// Save to remote only through Hydra 
	PassParamToHydra((char *)pszSection, (char *)pszName, 0, pszVal, bCreate, lGroupNum, lFlags, lClientEventData, bIniFiled);
	
	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csParamDB);

	return (iRetCode);
}


// 0 - OK
// 1 - Cant Open Key, Param set to default 
// 2 - NOt Found, Param created with default value
// 3 - NOt Found, Cant create new param , Param set to default
// 4 - NOt Found, Didn't create, accoording to request
LONG CParam::GetParam(char *szTableName, long lKey1, long lKey2, char *szFieldName, short nFiledType,  char *szFieldVal, long lBufferLen, DWORD *pVal ,BOOL bCreate)
{

	CString sTableName(szTableName);
	CString strRecordName;


	if(sTableName == "Paths")
	{
		if(lKey1 == 1)
		{
			strRecordName = "Paths\\Long";			
		}
		if(lKey1 == 2)
		{
			strRecordName = "Paths\\Short";
		}
	}		
	else if(sTableName == "ModeScheme")
	{
		strRecordName = "Pumps\\Pump";
		
		char sKey1[3];
		memset(sKey1,0,sizeof(sKey1));

		sprintf(sKey1,"%02d",lKey1);
		strRecordName += sKey1;

		strRecordName += "\\ModeScheme";

		char sKey2[3];
		memset(sKey2,0,sizeof(sKey2));

		sprintf(sKey2,"%1d",lKey2);
		strRecordName += sKey2;		
	}
	else if(sTableName == "Position")
	{
		strRecordName = "Pumps\\Pump";
		
		char sKey1[3];
		memset(sKey1,0,sizeof(sKey1));

		sprintf(sKey1,"%02d",lKey1);
		strRecordName += sKey1;

		strRecordName += "\\Position";

		char sKey2[3];
		memset(sKey2,0,sizeof(sKey2));

		sprintf(sKey2,"%1d",lKey2);
		strRecordName += sKey2;		

	}
	else if (sTableName == "Day")
	{
		strRecordName = "Shifts\\Day";
		
		char sKey1[3];
		memset(sKey1,0,sizeof(sKey1));

		sprintf(sKey1,"%1d",lKey1);
		strRecordName += sKey1;

		strRecordName += "\\Shift";

		
		char sKey2[3];
		memset(sKey2,0,sizeof(sKey2));

		sprintf(sKey2,"%1d",lKey2);
		strRecordName += sKey2;
	}
	else if (sTableName == "Taxes")
	{
		strRecordName = sTableName;
		
		if(lKey1)
		{
			strRecordName += "\\";
			int i = sTableName.ReverseFind('e');
			int len = sTableName.GetLength(); 
			if(i == (len-2))
				sTableName.Delete(i,2);

			strRecordName += sTableName; 
			
			char sKey1[3];
			memset(sKey1,0,sizeof(sKey1));

			sprintf(sKey1,"%02d",lKey1);
			strRecordName += sKey1;
		}		

	}
	else if (sTableName == "Database")
	{
		switch(lKey1)
		{
			case PUMP_PRE_PAY_FILE:
				strRecordName = "Database\\PrePay";
				break;
			case PUMP_OLA_FILE:
				strRecordName = "Database\\OLA";			
				break;
			case PUMP_STAT_FILE:
				strRecordName = "Database\\PumpStat";
				break;
			case PUMP_TOTLAS_FILE:
				strRecordName = "Database\\PumpTotals";
				break;
			case TANK_READS_FILE:
				strRecordName = "Database\\TankRead";
				break;
			case FUEL_DELIVERIES_FILE:
				strRecordName = "Database\\Delivery";
				break;
			case ALARM_FILE:
				strRecordName = "Database\\Alarm";
				break;
			case CAR_WASH_FILE:
				strRecordName = "Database\\CarWash";
				break;
			case RECEIPT_FILE:
				strRecordName = "Database\\Receipt";
				break;
			case RFS_FILE:
				strRecordName = "Database\\RFS";
				break;
			case EXTRA_INFO_FILE:
				strRecordName = "Database\\ExtraInfo";
				break;
			case ITEMS_FILE:
				strRecordName = "Database\\Items";
				break;
//4.0.1.38
			case SHIFT_TANKS_FILE:
				strRecordName = "Database\\ShiftTankTotals";
				break;

			case SHIFT_PUMP_FILE :
				strRecordName = "Database\\ShiftPumpTotals";
				break;

			case SHIFT_DELIVERY_FILE:
				strRecordName = "Database\\ShiftDeliveryTotals";
				break;				
//4.0.1.38
			case ASH2000_FILE:    //4.0.3.47
				strRecordName = "Database\\Ash2000";
				break;
			case DB_TANK_GAUGE_FILE:    //4.0.5.24  
				strRecordName = "Database\\DBTankGauge";
			break;
			case ACCUMULATION_SHIFT_TOTAL_FILE:
				strRecordName = "Database\\AccumulationShiftTotal";
				break;

			case RECEIPT_EXTRA_INDEX_FILE:
				strRecordName = "Database\\ReceiptExtraIndex";
				break;
			case PUMP_OLA_TEMP_FILE:  //4.0.9.509
				strRecordName = "Database\\OLATemporary";
				break;
			default :
				strRecordName = "Database";				
				break;
		}
	}
	else if (sTableName == "FuelLoyalty")
	{
		switch(lKey1)
		{
			case FUEL_LOYALTY_SYS_PARAM_FILE:
				strRecordName = "FuelLoyalty\\SysParam";
				break;
			case FUEL_LOYALTY_SERVER_FILE:
				strRecordName = "FuelLoyalty\\Server";
				break;
			case FUEL_LOYALTY_TRIGGER_PROM_FILE:
				strRecordName = "FuelLoyalty\\Trigger";
				break;
			case FUEL_LOYALTY_QUERY_PROM_FILE:
				strRecordName = "FuelLoyalty\\Query";
				break;
			case FUEL_LOYALTY_ADD_ITEM_PROM_FILE:
				strRecordName = "FuelLoyalty\\AddItem";
				break;
				default :
				strRecordName = "FuelLoyalty";				
				break;
		}
	}
	else if (sTableName == "FlowControl")
	{
		strRecordName = "FlowControl";		//4.0.23.0 74810		
	
	}	
	else
	{
		strRecordName = sTableName;

		if(lKey1)
		{
			strRecordName += "\\";
			int i = sTableName.ReverseFind('s');
			int len = sTableName.GetLength(); 
			if(i == (len-1))
				sTableName.Delete(i,1);

			strRecordName += sTableName; 
			
			char sKey1[3];
			memset(sKey1,0,sizeof(sKey1));

			sprintf(sKey1,"%02d",lKey1);
			strRecordName += sKey1;
		}		
	}

	int iRtc;
	BOOL bWriteToLog = TRUE;
	if(sTableName == "Paths")
		bWriteToLog = FALSE;

	if(nFiledType == FIELD_SZ)
	{

		iRtc = LoadParam( (char *)((LPCTSTR)	strRecordName),
						szFieldName,
						szFieldVal,    
				(DWORD) lBufferLen,
						szFieldVal,
						bCreate,
						bWriteToLog);
	}
	else
	{
	  iRtc = LoadParam( (char *)((LPCTSTR)	strRecordName),
						szFieldName,
						pVal,    
						*pVal,
						bCreate,
	  					bWriteToLog);
	}

	return (long)iRtc;
}


// 0 - OK
// 1 - Cant Open Open Key.
LONG CParam::SetParam(char *szTableName, long lKey1, long lKey2, char *szFieldName, short nFiledType,  char *szFieldVal, DWORD dwVal ,BOOL bCreate,long lGroupNum,long lFlags,long lClientEventData)
{

	CString sTableName(szTableName);
	CString strRecordName;

	if(sTableName == "ModeScheme")
	{
		strRecordName = "Pumps\\Pump";
		
		char sKey1[3];
		memset(sKey1,0,sizeof(sKey1));

		sprintf(sKey1,"%02d",lKey1);
		strRecordName += sKey1;

		strRecordName += "\\ModeScheme";

		char sKey2[3];
		memset(sKey2,0,sizeof(sKey2));

		sprintf(sKey2,"%1d",lKey2);
		strRecordName += sKey2;		

	}
	else if(sTableName == "Position")
	{
		strRecordName = "Pumps\\Pump";
		
		char sKey1[3];
		memset(sKey1,0,sizeof(sKey1));

		sprintf(sKey1,"%02d",lKey1);
		strRecordName += sKey1;

		strRecordName += "\\Position";

		char sKey2[3];
		memset(sKey2,0,sizeof(sKey2));

		sprintf(sKey2,"%1d",lKey2);
		strRecordName += sKey2;		

	}
	else if (sTableName == "Day")
	{
		strRecordName = "Shifts\\Day";
		
		char sKey1[3];
		memset(sKey1,0,sizeof(sKey1));

		sprintf(sKey1,"%1d",lKey1);
		strRecordName += sKey1;

		strRecordName += "\\Shift";

		char sKey2[3];
		memset(sKey2,0,sizeof(sKey2));

		sprintf(sKey2,"%1d",lKey2);
		strRecordName += sKey2;
	}
	//4.0.16.500
	else if (sTableName == "Taxes")
	{
		strRecordName = sTableName;
		if(lKey1)
		{
			strRecordName += "\\";
			int len = sTableName.GetLength(); 
			int i = sTableName.Find("es", len-2);			
			if(i > 0)
				sTableName.Delete(i,2);

			strRecordName += sTableName; 
			
			char sKey1[3];
			memset(sKey1,0,sizeof(sKey1));

			sprintf(sKey1,"%02d",lKey1);
			strRecordName += sKey1;
		}
	}
	else
	{
		strRecordName = sTableName;
		if(lKey1)
		{
			strRecordName += "\\";
			int i = sTableName.ReverseFind('s');
			int len = sTableName.GetLength(); 
			if(i == (len-1))
				sTableName.Delete(i,1);

			strRecordName += sTableName; 
			
			char sKey1[3];
			memset(sKey1,0,sizeof(sKey1));

			sprintf(sKey1,"%02d",lKey1);
			strRecordName += sKey1;
		}		
	}

	int iRtc;
	if(nFiledType == FIELD_SZ)
	{
	  iRtc = SaveParam((char *)((LPCTSTR)	strRecordName),
						szFieldName,
						szFieldVal,
						bCreate,lGroupNum,lFlags,lClientEventData);//4.0.1.38
	}
	else
	{
	  iRtc = SaveParam( (char *)((LPCTSTR)	strRecordName),
						szFieldName,
						dwVal,
						bCreate,lGroupNum,lFlags,lClientEventData);//4.0.1.38
		
	}

	return ((long)iRtc);
}

BOOL CParam::IsPumpSrvTable(char *szTableName)
{
	CString sTableName(szTableName);
	BOOL bIsTable = FALSE;

	if(sTableName == "Database")
	{
		bIsTable = TRUE;
	}
	else if(sTableName == "GeneralParam")
	{
		bIsTable = TRUE;
	}
	else if(sTableName == "Grades")
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "Messages")
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "Modes")
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "Pumps")
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "ModeScheme")
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "Position")
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "Day")
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "PureProducts")
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "Receipt")
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "Tanks")
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "ServiceFee")
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "Taxes")//4.0.1.32
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "BankNotes")//4.0.3.60
	{
		bIsTable = TRUE;
	}
	else if (sTableName == "Languages")//4.0.5.42
	{
		bIsTable = TRUE;
	}

	return bIsTable;
}

//!!!!!YKL 01/12/99
/***************************************************************************
DESCRIPTION: Get full record as a string from PumpSrv.
             String is formated as follows:
             Field1Name[Type]=Value,|Field2Name[Type]=Value,|Field3Name[Type]=Value
             Type can be:
               S  - string
               I  - DWROD
.
PARAMETERS :   szTableName       - pumpsrv table to read from
               lKey1             - primary key
               lKey2             - secondary key
               szBuf             - buffer to store data in
               lBufferLen        - input buffer size
               lWrittenLen       - will hold number of bytes written to buffer
               lNumberOfFields   - Number of fields in output buffer
               bWriteToLog       - write output record to log file
.
RETURNS    :   0 - OK
               1 - Cant Open Key
               2 - Record NOt Found
               3 - Buffer too small
.
Who   When     What
---   -------- ----
YKL   07/12/99 Start
*****************************************************************************/
LONG CParam::GetRecord(char *szTableName, long lKey1, long lKey2, char *szBuff, long lBufferLen, long *lWrittenLen, long *lNumberOfFields, BOOL bWriteToLog,long lFlags)
{
	CString  sTableName(szTableName);
	CString  strRecordName;
	CString  strRecValues;  // will hold all values found in registry key
	long     lRc=0;         // return code
	BOOL			bWorkLocal = TRUE;

	strRecordName = GetRecordName(szTableName,lKey1,lKey2);  

	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csParamDB);

	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pCFGDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL))
	{
		CString cTmpStr;
		BSTR	bstrVal = cTmpStr.AllocSysString();
		
		CString strPathName((const unsigned char*)"Software\\PointOfSale\\PumpSrv");
		strPathName += "\\";
		strPathName += strRecordName;
		
		BSTR    bstrServerPath = strPathName.AllocSysString();
		
		bWorkLocal = FALSE;
		try
		{	
			lRc = _HOST.m_pCFGDB->GetRecord(bstrServerPath,0,0,&bstrVal,lWrittenLen,lNumberOfFields);
		}
		catch(_com_error & e )
		{
			_LOGMSG.LogResult(
				0, // Src
				"ICFGDB",
				"GetRecord",
				FALSE,0,
				TRUE,e.Error(),e);

			lRc = 1;
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();
		}
		catch(...)
		{
			if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICFGDB",
					"GetRecord",
					TRUE,0,
					(lRc!=0)? TRUE:FALSE,lRc);

			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();			
		}

		//checking the result
		if ( lRc == OK )
		{
			_bstr_t tmpBstr( bstrVal);
			int len = tmpBstr.length();
			memset(szBuff,0,lBufferLen);
			memcpy( szBuff, (char *)tmpBstr, len);
		}
		
		//freeing all the bstrs
		if(bstrVal)
			SysFreeString(bstrVal);
		if(bstrServerPath)
			SysFreeString(bstrServerPath);
			
		CString str;
		switch(lRc)
		{
		case 1:
			str.Format("CParam::GetRecord:Cant Open Key %s",szTableName);					
			break;
		case 2:
			str.Format("CParam::GetRecord:Record %s not found in registry",szTableName);
			break;
		case 3:
			str.Format("CParam::GetRecord:Buffer too small");
			break;
		}
		if(bWriteToLog && lRc)
				_LOGMSG.LogMsg("Param","GetRecord",0,LOG_NONE,str);
	}
	if(bWorkLocal)
	{
		CString strPathName((const unsigned char*)"Software\\PointOfSale\\PumpSrv");

		strPathName += "\\";
		strPathName += strRecordName;

	   // create string will all values in registry key
	   lRc = GetRecValues((char *)(LPCSTR)strPathName, &strRecValues,lNumberOfFields,bWriteToLog);

	   if ( strRecValues.GetLength() > lBufferLen-1) 
	   {
		  lRc = 3;
		  if(bWriteToLog)
			   _LOGMSG.LogMsg("Param","GetRecord",0,LOG_NONE,"Record Buffer too small");
	   }
	   else 
	   {
		  strcpy(szBuff, strRecValues);
	//      if(bWriteToLog)
	// 		_LOGMSG.LogMsg(strRecValues);
	   }
	}
	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csParamDB);


  return lRc;

}
//!!!!!YKL 01/12/99

//!!!!!YKL 01/12/99
/***************************************************************************
DESCRIPTION: Write a full record to registry
             String is formated as follows:
             Field1Name[Type]=Value,|Field2Name[Type]=Value,|Field3Name[Type]=Value
             Type can be:
               S  - string
               I  - DWROD
.
PARAMETERS :   szTableName - table name
               lKey1       - Primary key
               lKey2       - Secondary key
               szBuff      - string holding record.
               bCreate     - create value in registry if does not exit
               bWriteToLog - write to log file
.
RETURNS    :   0 - OK
.
Who   When     What
---   -------- ----
YKL   21/12/99 Start
*****************************************************************************/
LONG CParam::SetRecord(char *szTableName, long lKey1, long lKey2, char *szBuff, BOOL bCreate, BOOL bWriteToLog,long lGroupNum,long lFlags)
{

	CString  sTableName(szTableName);   // table name as cstring
	CString  strSection;                // registry key
	CString  strRecValues;              // will hold all values found in registry key
	long     lRc=0;                     // return code
	CString  strValueName;              // field name
	long     lValueType;                // value type (REG_SZ / REG_DWORD)
	DWORD    dwValue;                   // field value (if dword)
	CString  strValue;                  // field value (if string)
	char     *szStart = szBuff;         // points to next field to extract
	BOOL	 bWorkLocal = TRUE;
	long	 lLocalCheckRet = FIELD_NOT_EXISTS;
	BOOL	 bSubTableIsEncode = FALSE;							//4.0.22.43 TD 72716

	strSection = GetRecordName(szTableName,lKey1,lKey2);  

	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csParamDB);


	if (bSubTableIsEncode == FALSE && strlen(szStart) && strstr(szStart, "PCICompliant") != NULL)	//4.0.22.44 TD 72716
	{
		bSubTableIsEncode = TRUE;
	}

	//4.0.0.47 start
	//checking if this parameter should be local only and Hydra is active.
	if(_HOST.m_lHydraActive && lFlags != HOP_DIRECT_LOCAL)
	{
		lLocalCheckRet = IsLocalOnlyParameter(strSection.GetBuffer(0),NULL,szBuff,SET_RECORD,bCreate);
		if(lLocalCheckRet == FIELD_EXISTS)
			lFlags = HOP_DIRECT_LOCAL;
	}
	//4.0.0.47 end
	
	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pCFGDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
	{
		CString strPathName((const unsigned char*)"Software\\PointOfSale\\PumpSrv");
		
		BSTR bstrServerPath = strPathName.AllocSysString();

		BSTR bstrSection = strSection.AllocSysString();

		_bstr_t	bstrtVal(szBuff);
		BSTR bstrVal = bstrtVal.copy();
		
		bWorkLocal = FALSE;
		try
		{
			lRc = _HOST.m_pCFGDB->SetRecord(bstrServerPath,bstrSection,bstrVal,bCreate,lFlags,0,lGroupNum,0);
		}
		catch(_com_error & e )
		{
			_LOGMSG.LogResult(
				0, // Src
				"ICFGDB",
				"SetRecord",
				FALSE,0,
				TRUE,e.Error(),e);

			lRc = 1;
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();
		}
		catch(...)
		{
			if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"ICFGDB",
					"SetRecord",
					TRUE,0,
					(lRc!=0)? TRUE:FALSE,lRc);

			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();			
		}

		
		//freeing all the bstrs
		if(bstrServerPath)
			SysFreeString(bstrServerPath);
		if(bstrSection)
			SysFreeString(bstrSection);
		if(bstrVal)
			SysFreeString(bstrVal);	
	}
	if(bWorkLocal)
	{

	   // scan buffer and get one field at a time. Stop when finished or if error found.
		while ( !lRc && strlen(szStart) && !(lRc=GetFieldValue(szStart, &szStart, &strValueName, &lValueType, &strValue, &dwValue)) ) 
		{
			if ( lValueType==REG_SZ) 
			{
				 lRc = SaveParam((char *)(LPCSTR)strSection,(char *)(LPCSTR)strValueName,(char *)(LPCSTR)strValue);
			}
			else 
			{
				 lRc = SaveParam((char *)(LPCSTR)strSection,(char *)(LPCSTR)strValueName,dwValue);
			}

			if (lRc == 0)
			{
				if(_Module.m_server.GetServerState() >= SERVER_STARTUP)
				{
					CMD_STRUCT_UPDATE_PARAM cmdInfo;
					memset(&cmdInfo,0,sizeof(CMD_STRUCT_UPDATE_PARAM));
					strcpy(cmdInfo.szTableName,szTableName);			
					cmdInfo.lKey1 = lKey1;
					cmdInfo.lKey2 = lKey2;
					strcpy(cmdInfo.szFieldName,(char *)(LPCSTR)strValueName);			
					if(lValueType==REG_SZ) 
						cmdInfo.nFieldType = FIELD_SZ;
					else
						cmdInfo.nFieldType = FIELD_DWORD;

					strcpy(cmdInfo.szFieldVal,(char *)(LPCSTR)strValue);
					cmdInfo.lVal = dwValue;
					_Module.m_server.SetCommand(CMD_UPDATE_PARAM,SERVER_POS_NUMBER, 0,&cmdInfo,sizeof(CMD_STRUCT_UPDATE_PARAM));
					_Module.m_server.UpdateParam(szTableName,lKey1,lKey2,cmdInfo.szFieldName,cmdInfo.nFieldType,cmdInfo.szFieldVal,dwValue);
				}
			}

		}
	}
	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csParamDB);


	if (TRUE == bSubTableIsEncode)					//4.0.22.43 TD 72716
	{
		_Module.m_server.m_cProtectedDataHandler.ChangesResponse();
	}


  return lRc;
}
//!!!!!YKL 01/12/99

//!!!!!YKL 01/12/99
/***************************************************************************
DESCRIPTION: Build a registry key name based on table name and key
.
PARAMETERS :   szTableName - table name (Pumps, Modes etc.)
               lKey1       - primary key
               lKey2       - secondary key
.
RETURNS    :   Key name (from below HKLM\Software\PointOfSale\PumpSrv)
.
Who   When     What
---   -------- ----
YKL   01/12/99 Start
*****************************************************************************/
CString CParam::GetRecordName(char *szTableName, long lKey1, long lKey2)
{
   CString strRecordName;
   CString sTableName(szTableName);

	if(sTableName == "Paths")
	{
		if(lKey1 == 1)
		{
			strRecordName = "Paths\\Long";			
		}
		if(lKey1 == 2)
		{
			strRecordName = "Paths\\Short";
		}
	}		
	else if(sTableName == "ModeScheme")
	{
		strRecordName = "Pumps\\Pump";
		
		char sKey1[3];
		memset(sKey1,0,sizeof(sKey1));

		sprintf(sKey1,"%02d",lKey1);
		strRecordName += sKey1;

		strRecordName += "\\ModeScheme";

		char sKey2[3];
		memset(sKey2,0,sizeof(sKey2));

		sprintf(sKey2,"%1d",lKey2);
		strRecordName += sKey2;		
	}
	else if(sTableName == "Position")
	{
		strRecordName = "Pumps\\Pump";
		
		char sKey1[3];
		memset(sKey1,0,sizeof(sKey1));

		sprintf(sKey1,"%02d",lKey1);
		strRecordName += sKey1;

		strRecordName += "\\Position";

		char sKey2[3];
		memset(sKey2,0,sizeof(sKey2));

		sprintf(sKey2,"%1d",lKey2);
		strRecordName += sKey2;		

	} 
	else if (sTableName == "Day")
	{
		strRecordName = "Shifts\\Day";
		
		char sKey1[3];
		memset(sKey1,0,sizeof(sKey1));

		sprintf(sKey1,"%1d",lKey1);
		strRecordName += sKey1;

		strRecordName += "\\Shift";

		char sKey2[3];
		memset(sKey2,0,sizeof(sKey2));

		sprintf(sKey2,"%1d",lKey2);
		strRecordName += sKey2;
	}
	else if (sTableName == "Taxes")
	{
		strRecordName = sTableName;

		if(lKey1)
		{
			strRecordName += "\\";
			int i = sTableName.ReverseFind('e');
			int len = sTableName.GetLength(); 
			if(i == (len-2))
				sTableName.Delete(i,2);

			strRecordName += sTableName; 
			
			char sKey1[3];
			memset(sKey1,0,sizeof(sKey1));

			sprintf(sKey1,"%02d",lKey1);
			strRecordName += sKey1;
		}		

	}	
	//4.0.2.23
	else if (sTableName == "PricePoles")
	{
		strRecordName = "PricePoles\\PricePole";
		
		char sKey1[3];
		memset(sKey1,0,sizeof(sKey1));		
		sprintf(sKey1,"%02d",lKey1);
		strRecordName += sKey1;

		if (lKey2)
		{
			strRecordName += "\\Panel";

			char sKey2[3];
			memset(sKey2,0,sizeof(sKey2));

			sprintf(sKey2,"%02d",lKey2);
			strRecordName += sKey2;

		}
	}
	//4.0.2.23
	else
	{
		strRecordName = sTableName;

		// Generic single key handling 

		if(lKey1)
		{
			strRecordName += "\\";
			int i = sTableName.ReverseFind('s');
			int len = sTableName.GetLength(); 
			if(i == (len-1))
				sTableName.Delete(i,1);
			
			//4.0.3.61
			int iPath = sTableName.ReverseFind('\\');

			if (iPath > 0)
				sTableName.Delete(0,iPath +1);
			

			strRecordName += sTableName; 
			
			char sKey1[3];
			memset(sKey1,0,sizeof(sKey1));

			sprintf(sKey1,"%02d",lKey1);
			strRecordName += sKey1;
		}		
	}

   return strRecordName;
}
//!!!!!YKL 01/12/99

//!!!!!YKL 01/12/99
/***************************************************************************
DESCRIPTION: Scan all values in registry key. Create a string that is made 
             of all values found in that key.
             String is formated as follows:
             Field1Name[Type]=Value,|Field2Name[Type]=Value,|Field3Name[Type]=Value
             Type can be:
               S  - string
               I  - DWROD
.
PARAMETERS :   strPathName - Registry key
.
RETURNS    :   0  - ok
               1  - Can't open registry key
               2  - Unknown data type
.
Who   When     What
---   -------- ----
YKL   01/12/99 Start
*****************************************************************************/
long CParam::GetRecValues(char *strPathName, CString *pstrVals, long *lNumFields, BOOL bWriteToLog)
{
   HKEY     hKey;          // registry handle
   long     lRetVal;
   long     lRc=0;
   DWORD    dwIndex=0;     // for registry scanning API
   char     sValueName[512];  // field name
   DWORD    dwValueSize=sizeof(sValueName);
   DWORD    dwType;
   unsigned char  strData[1024] = {0};	//4.0.19.0 55366
   DWORD dwDataSize=sizeof(strData);
   char     sTmp[50];

   *pstrVals = "";
   *lNumFields=0;

	// Open Key
	lRetVal = RegOpenKeyEx(  
					HKEY_LOCAL_MACHINE, // handle of an open key 
					(LPCTSTR)strPathName,  // address of name of subkey to open
					0,   // reserved  REGSAM samDesired, 
					KEY_WRITE|KEY_READ,// security access mask
					&hKey);   // address of handle to open key); 

   if(lRetVal != ERROR_SUCCESS) // success to open 
      return 1;

   // Get 1st value.
   if ( ::RegEnumValue(hKey,dwIndex, sValueName, &dwValueSize,NULL, &dwType,strData,&dwDataSize) == ERROR_SUCCESS) 
   {
      // scan key until all values read
      do 
      {
         if ( (*lNumFields) ) // not 1st value? Add delimiter
            *pstrVals += REC_VALUE_DELIM;

         *pstrVals += (LPSTR)sValueName;
         *pstrVals += "[";
         if ( dwType==REG_SZ) // string
         {
            *pstrVals += "S]=";
            *pstrVals += strData;
         }
         else if ( dwType==REG_DWORD) // dword
         {
            sprintf(sTmp, "%ld", *((long *)strData));
            *pstrVals += "I]=";
            *pstrVals += sTmp;
         }
		 else if ( dwType==REG_BINARY)//4.0.3.23
		 {
			dwIndex++;			
			continue;
		 }
         else   // Unknown type
         {
            return 2;
         }

         dwValueSize=sizeof(sValueName);
         dwDataSize=sizeof(strData);
         dwIndex++;
         (*lNumFields)++;
		 memset(&strData, 0, sizeof(strData));	//4.0.19.0 //55366
      } while (::RegEnumValue(hKey,dwIndex, sValueName, &dwValueSize,NULL, &dwType,strData,&dwDataSize) != ERROR_NO_MORE_ITEMS);
   }


   RegCloseKey(hKey) ;

   return lRc;
}

//!!!!!YKL 01/12/99
/***************************************************************************
DESCRIPTION: Extract one field from a long string of values
.
PARAMETERS :   szBuff         - buffer to extract from
               pszNextBuff    - will get the start pointer of next field
               pstrValueName  - will get field name 
               plValueType    - will get the field type (string / dword)
               pstrValue      - pointer to buffer to hold string value
               pdwValue       - pointer to dword value
.
RETURNS    :   0 - OK
.
Who   When     What
---   -------- ----
YKL   22/12/99 Start
*****************************************************************************/
long CParam::GetFieldValue(char *szBuff, char **pszNextBuff, CString *pstrValueName, long *plValueType, CString *pstrValue, DWORD *pdwValue)
{
   char *pLeftBrace;
   char *pRightBrace;
   char *pValueType;
   char *pValue; 
   char *pValueEnd;
   CString strValue;

   // ----------------------
   // Find field type braces
   // ----------------------
   // find delimiter to field type (left [)
   pLeftBrace = strchr(szBuff, '[');
   if ( !pLeftBrace )
      return 1;

   // find delimiter to field type (left [)
   pRightBrace = strchr(pLeftBrace, ']');
   if ( !pRightBrace )
      return 2;

   if ( pRightBrace - pLeftBrace != 2 )
      return 3;
   // ----------------------

   // -----------------------
   // Find field type 
   // -----------------------
   pValueType = pLeftBrace+1;
   if ( (*pValueType) == 'S' ) 
      *plValueType = REG_SZ;
   else if ( (*pValueType) == 'I' ) 
      *plValueType = REG_DWORD;
   else
      return 4;
   // -----------------------

   // -----------------------
   // Find field name 
   // -----------------------
   pstrValueName->Format("%.*s", pLeftBrace - szBuff, szBuff);
   // -----------------------

   // ------------------------------
   // Find field value 
   // ------------------------------
   if ( *(pRightBrace+1) != '=' )
      return 5;

   pValue = pRightBrace+2;

   //4.0.13.508   pValueEnd = strstr(pValue, REC_VALUE_DELIM);
   if (strstr(pValue,REC_VALUE_DELIM_LANGUAGES))
   {
		pValueEnd = strstr(pValue, REC_VALUE_DELIM_LANGUAGES);
   }
   else
   {
		pValueEnd = strstr(pValue, REC_VALUE_DELIM);
   }

   if ( !pValueEnd ) 
      pValueEnd = pValue+strlen(pValue);

   strValue.Format("%.*s", pValueEnd-pValue, pValue);

   // string or dword?
   if ( (*plValueType) == REG_SZ ) 
      *pstrValue = strValue;
   else
      *pdwValue = atol(strValue);
   // ------------------------------

   // create pointer for next field
   if ( (*pValueEnd) ) 
      *pszNextBuff = pValueEnd+strlen(REC_VALUE_DELIM);
   else
      *pszNextBuff = pValueEnd;
   
   return 0;
}
//!!!!!YKL 01/12/99

LONG CParam::GetParam3Level(char *szSubjectName, char *szTableName, char *szSubTableName, long lKey1, long lKey2, char *szFieldName, short nFiledType, char *szFieldVal, long lBufferLen, DWORD *pVal, BOOL bCreate, BOOL bIniFile)
{
	CString strRecordName(szSubjectName);
	CString strTableName(szTableName);
	int iRtc=4 ,i=0;

	BOOL bWriteToLog = TRUE;
	char sKey1[3];
	char sKey2[3];
	memset(sKey1,0,sizeof(sKey1));
	memset(sKey2,0,sizeof(sKey2));
    
    if (szTableName[0])
	{
		if (szSubjectName[0])
		{
			strRecordName+="\\";  // Search For Find('s')
		}
		strRecordName+=szTableName;
		if (((i = strTableName.ReverseFind('s')) > 0) && (i == (strTableName.GetLength() - 1)))	//4.0.19.505
		{
		 strRecordName += "\\";
	     int len = strTableName.GetLength(); 
		 if(i == (len-1))
			 strTableName.Delete(i,1);
		 strRecordName += strTableName; 
		 sprintf(sKey1,"%02d",lKey1);
		 strRecordName += sKey1;
		}
	}
    
    if (szSubTableName[0])
	{
     if (szTableName[0])
	 {
		 strRecordName+="\\"; 
	 }
	 strRecordName += szSubTableName;
	 sprintf(sKey2,"%02d",lKey2);
	 strRecordName += sKey2;
	}
     
	if (strRecordName.GetLength())
	{
		if(nFiledType == FIELD_SZ)
		{
			iRtc = LoadParam( (char *)((LPCTSTR) strRecordName),
												 szFieldName,
												 szFieldVal,    
										 (DWORD) lBufferLen,
												 szFieldVal,
    											 bCreate,
												 bWriteToLog,
												 0,HOP_DIRECT_LOCAL,bIniFile);//4.0.1.40
		}
		else
		{
			iRtc = LoadParam( (char *)((LPCTSTR)	strRecordName),
													szFieldName,
													pVal,    
													*pVal,
													bCreate,
													bWriteToLog,
													0,HOP_DIRECT_LOCAL,bIniFile);//4.0.1.40 //4.0.8.503
		}
	}
	return (long)iRtc;
}
// 0 - OK
// 1 - Cant Open Open Key.
LONG CParam::SetParam3Level(char *szSubjectName, char *szTableName, char *szSubTableName, long lKey1, long lKey2, char *szFieldName, short nFiledType, char *szFieldVal, DWORD dwVal, BOOL bCreate, BOOL bIniFile)
{
	CString strRecordName(szSubjectName);
	CString strTableName(szTableName);
	int iRtc=0 ,i=0;
	BOOL bWriteToLog = TRUE;
	char sKey1[3];
	char sKey2[3];
	memset(sKey1,0,sizeof(sKey1));
	memset(sKey2,0,sizeof(sKey2));

    if (szTableName[0])
	{
		if (szSubjectName[0])
		{
			strRecordName+="\\"; 
		}
		strRecordName+=szTableName;
		if ((i = strTableName.ReverseFind('s')) > 0 )
		{
		 strRecordName += "\\";
	     int len = strTableName.GetLength(); 
		 if(i == (len-1))
			 strTableName.Delete(i,1);
		 strRecordName += strTableName; 
		 sprintf(sKey1,"%02d",lKey1);
		 strRecordName += sKey1;
		}
	}
    
    if (szSubTableName[0])
	{
     if (szTableName[0])
	 {
		 strRecordName+="\\"; 
	 }
	 strRecordName += szSubTableName;
	 sprintf(sKey2,"%02d",lKey2);
	 strRecordName += sKey2;
	}
     
	if (strRecordName.GetLength())
	{
		if(nFiledType == FIELD_SZ)
		{
		  iRtc = SaveParam((char *)((LPCTSTR)	strRecordName),
												szFieldName,
												szFieldVal,
												bCreate,0,0,0,TRUE);  //4.0.8.503
		}
		else
		{
		  iRtc = SaveParam( (char *)((LPCTSTR)	strRecordName),
												szFieldName,
												dwVal,
												bCreate,0,0,0,TRUE); //4.0.8.503
			
		}
	}
	return ((long)iRtc);
}

int CParam::DeleteParam(char *pszSection, char *pszName, long *pVal)
{
	HKEY			hKey;
	long			lRetVal;

	int				iRetNum = 0;

	DWORD   dwRtn, dwSubKeyLength;
	LPTSTR  pSubKey = NULL;
	TCHAR   szSubKey[256]; // (256) this should be dynamic.
	 int lSize =strlen(pszName);
     
	// -------------------------
	// Critical Section - Start
	// -------------------------
	EnterCriticalSection(&m_csParamDB);

	CString strPathName((const unsigned char*)"Software\\PointOfSale\\PumpSrv");
	if(pszSection)
	{
		strPathName += "\\";
		strPathName += pszSection;
	}
	
     lRetVal=RegOpenKeyEx(HKEY_LOCAL_MACHINE,strPathName,
			 0, KEY_ALL_ACCESS, &hKey );
	

	if(lRetVal == ERROR_SUCCESS) // success to open or create the key.
	{		

		 long lCount = 0;

	     while (lRetVal == ERROR_SUCCESS )
        {
           dwSubKeyLength = 256;

           lRetVal=RegEnumValue(
                          hKey,
                          lCount,       // always index zero
                          szSubKey,
                          &dwSubKeyLength,
                          NULL,
                          NULL,
                          NULL,
                          NULL
                        );

           if(lRetVal == ERROR_NO_MORE_ITEMS)
           {              
              break;
           }
           else if(lRetVal == ERROR_SUCCESS)
		   {			   
			   if ((!strcmp(pszName , szSubKey)) && 
				    ((long)dwSubKeyLength == lSize))
			   {
					dwRtn = RegDeleteValue(hKey, pszName);
					break;
			   }

		   }              
		   lCount++;
        }

		if(LOG_LOGIC_ERROR)
		{
			CString str;
			str.Format("DELETE %s %s, value  ! (lRetVal = %ld) ",pszSection,pszName,lRetVal);			
			_LOGMSG.LogMsg("Param","DeleteParam",0,LOG_NONE,str, LOG_LEVEL_1);
		}
	}	
	else
	{
		iRetNum = 1; //Cant Open Key, Param set to default
		if(LOG_LOGIC_ERROR)
		{
			CString str;
			str.Format("Failed to Delete %s %s, value  ! (lRetVal = %ld) ",pszSection,pszName,lRetVal);			
			_LOGMSG.LogMsg("Param","DeleteParam",0,LOG_NONE,str, LOG_LEVEL_1);
		}
	}
	
	
	// -------------------------
	// Critical Section - End
	// -------------------------
	LeaveCriticalSection(&m_csParamDB);

	return iRetNum;
}

/*.??? TD 72545NOT IN USE
DWORD CParam::RegDeleteKeyNT(HKEY hStartKey, LPTSTR pKeyName)
{

      DWORD   dwRtn, dwSubKeyLength;
      LPTSTR  pSubKey = NULL;
      TCHAR   szSubKey[256]; // (256) this should be dynamic.
      HKEY    hKey;

      // Do not allow NULL or empty key name
      if ( pKeyName &&  lstrlen(pKeyName))
      {
         if( (dwRtn=RegOpenKeyEx(hStartKey,pKeyName,
            0, KEY_ALL_ACCESS  , &hKey )) == ERROR_SUCCESS)
         {
			 long lCount = 0;

	          while (dwRtn == ERROR_SUCCESS )
            {
               dwSubKeyLength = 256;

               dwRtn=RegEnumValue(
                              hKey,
                              lCount,       // always index zero
                              szSubKey,
                              &dwSubKeyLength,
                              NULL,
                              NULL,
                              NULL,
                              NULL
                            );

               if(dwRtn == ERROR_NO_MORE_ITEMS)
               {
                  dwRtn = RegDeleteValue(hStartKey, pKeyName);
                  break;
               }
               else if(dwRtn == ERROR_SUCCESS)
                  dwRtn=RegDeleteKeyNT(hKey, szSubKey);

			   lCount++;
            }
            RegCloseKey(hKey);
            // Do not save return code because error
            // has already occurred
         }
      }
      else
         dwRtn = ERROR_BADKEY;

      return dwRtn;
   
}
*/
//4.0.0.47 start
/******************************************************************************
 Description:	Checks if this parametr should bw update only in the
				local computer and not sent to other computer.
 Returns:		long -	FIELD_NOT_EXISTS = 0
						FIELD_EXISTS	 = 1
						FIELD_REMOVED	 = 2	   	
 Parameters:   	char *szTableName - Table name.
				char *szFieldName - Field name.
				char* szBuff - SetRecord Buffer.
				long lFuncType - SAVE_PARAM		1
								 SET_RECORD		2
				BOOL bCreate - Create the field if not exists.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			9/5/2002   10:02		Start
******************************************************************************/

long CParam::IsLocalOnlyParameter(char* szTableName, char* szFieldName, char* szBuff, long lFuncType,BOOL bCreate)
{
	long lRetVal = FIELD_NOT_EXISTS;
	int iRet = 0;
	
	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"MemoInterval",lFuncType,bCreate);
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;
		
	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"Station_ComPort",lFuncType,bCreate);
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;

	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"Communication_ComPort",lFuncType,bCreate);
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;

	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"LogDir",lFuncType,bCreate);
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;

	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"LogLevel",lFuncType,bCreate);
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;

	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"ControllerFileFullPath",lFuncType,bCreate);
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;
	
	//4.0.5.11 checks if IPAddress
	char *pdest;

	if(szBuff)
	{
		pdest = strstr(szBuff,"IPAddress");
		if(pdest == NULL)
		{
			//4.0.1.38 start - for RFS and TankSrv support.
			lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"CommPort",lFuncType,bCreate);
			if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
				return lRetVal;
		}
	}
	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"DataCommPort",lFuncType,bCreate);
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;

	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"ReconCommPort",lFuncType,bCreate);
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;
	//4.0.1.38 end - for RFS and TankSrv support.
	
	//4.0.2.13 start checks the "UseBackUp" for DataBase\OLA
	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"UseBackUp",lFuncType,bCreate);
	if(lRetVal == FIELD_EXISTS)
	{
		iRet = strcmp(szTableName,"Database\\OLA");
		if( iRet != 0 )//string not identical.
			lRetVal = FIELD_NOT_EXISTS;			
	}
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;
	//4.0.2.13 end
	
	//4.0.2.15
	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"IsOlaRemote",lFuncType,bCreate);
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;
	
	//4.0.3.58
	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"PSTransactionDir",lFuncType,bCreate);
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;
	//4.0.12.509
	lRetVal = CheckPumpSrvParam(szTableName,szFieldName,szBuff,"TLS_CaptureFile",lFuncType,bCreate);
	if(lFuncType == SAVE_PARAM && lRetVal == FIELD_EXISTS)
		return lRetVal;
	
	return lRetVal;
}

/******************************************************************************
 Description:	Checks if one of PumpSrv Special parameters are send to 
				be saved.Those special parameters are for local saving only. 	
 Returns:      	long -	FIELD_NOT_EXISTS = 0
						FIELD_EXISTS	 = 1
						FIELD_REMOVED	 = 2											
 Parameters:   	char *szTableName - Table name.
				char *szFieldName - Field name.
				char* szBuff - SetRecord Buffer.
				char* szPSSpecialFieldName - PumpSrv special field name.
				long lFuncType - SAVE_PARAM		1
								 SET_RECORD		2
				BOOL bCreate - Create the field if not exists.
			
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			13/5/2002   16:28		Start
******************************************************************************/
long CParam::CheckPumpSrvParam(char* szTableName,char* szFieldName, char* szBuff, char* szPSSpecialFieldName, long lFuncType,BOOL bCreate)
{
	long lRetVal = FIELD_NOT_EXISTS;
	int iRet = 0;

	switch(lFuncType)
	{
		case SAVE_PARAM:
		{
			iRet = strcmp(szFieldName,szPSSpecialFieldName);
			if( iRet == 0 )//string are dentical.
				lRetVal = FIELD_EXISTS;
			break;
		}
		case SET_RECORD:
		{
			lRetVal = RemoveAndSaveParam(szTableName,szBuff,szPSSpecialFieldName,bCreate);
			break;
		}
		default:
		{
			lRetVal = FIELD_NOT_EXISTS;
		}
	}

	return lRetVal;
}

/******************************************************************************
 Description:	Removes from the SetRecord buffer the special fields.
				Saved those field with HOP_DIRECT_LOCAL and realloc the buffer without
				the special field so PumpSrv can continue.
 Returns:      	long -	FIELD_NOT_EXISTS = 0
						FIELD_EXISTS	 = 1
						FIELD_REMOVED	 = 2											
 Parameters:   	char *szTableName - Table name.
				char* szBuff - SetRecord Buffer.
				char* szPSSpecialFieldName - PumpSrv special field name.
				BOOL bCreate - Create the field if not exists.
			
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			13/5/2002   16:28		Start
******************************************************************************/
long CParam::RemoveAndSaveParam(char* szTableName,char *szBuff, char *szPSSpecialFieldName,BOOL bCreate)
{
	long lRetVal = FIELD_NOT_EXISTS;
	int iFieldindex = 0;
	int iDelimiterIndex = 0;
	CString  strValueName;              
	long     lValueType = 0;                
	DWORD    dwValue = 0;                  
	CString  strValue;      
	CString sBuff(szBuff);
	CString sFieldName(szPSSpecialFieldName);
	CString sTmpField;
	int iNumOfChars = 0;
	long lRet = 0;


	iFieldindex = sBuff.Find(sFieldName);
	if( iFieldindex == -1 )//not exists
		lRetVal = FIELD_NOT_EXISTS;
	else
	{
		//exists
		iDelimiterIndex = sBuff.Find(REC_VALUE_DELIM,iFieldindex);
		if(iDelimiterIndex)
		{            
			iNumOfChars = iDelimiterIndex - iFieldindex;
			sTmpField = sBuff.Mid(iFieldindex,iNumOfChars+2);//with the delimiter
			lRet = GetFieldValue(sTmpField.GetBuffer(0), &szPSSpecialFieldName, &strValueName, &lValueType, &strValue, &dwValue);
			if(!lRet)
			{			
				if ( lValueType==REG_SZ) 
				{
					 lRet = SaveParam(szTableName,(char *)(LPCSTR)strValueName,(char *)(LPCSTR)strValue,TRUE,0,HOP_DIRECT_LOCAL);
				}
				else 
				{
					 lRet = SaveParam(szTableName,(char *)(LPCSTR)strValueName,dwValue,TRUE,0,HOP_DIRECT_LOCAL);
				}
			}
			sBuff.Delete(iFieldindex,iNumOfChars+2);
			//cleaning the buffer and copying the new buffer.
			memset(szBuff,0,sizeof(szBuff));
			strcpy(szBuff,sBuff.GetBuffer(sBuff.GetLength()));
			lRetVal = FIELD_REMOVED;
		}
	}

	return lRetVal;
}
//4.0.0.47 end


/*.??? TD 72545 NOT IN USE
long CParam::GetRegistryValues(char * sKeyName , char * sValName , DWORD *pdwNumericMember, BOOL bCreateInRegistry, BOOL bCompare)
{
	long lRetVal = 0;
	DWORD dwTmpVal = 0;

	if(pdwNumericMember)
	{
		LoadParam(sKeyName,
		    	  sValName,
				  &dwTmpVal,
				  *pdwNumericMember,    //Default value
				  bCreateInRegistry);

		if(bCompare)
		{
			if(dwTmpVal != *pdwNumericMember)
				lRetVal = -1;     //value has changed
		}

		*pdwNumericMember = dwTmpVal;
		
	}

	else
		lRetVal = 1;

	return lRetVal;
}


long CParam::GetRegistryValues(char * sKeyName , char * sValName , char * sStringMember, BOOL bCreateInRegistry, BOOL bCompare)
{
	long lRetVal = 0;
	
	char sTmpVal[64];
	memset((char*)sTmpVal , 0 , sizeof(sTmpVal));

	if(sStringMember)
	{
		LoadParam(sKeyName,
		          sValName,
				  (char*)sTmpVal,
				  (DWORD)sizeof(sTmpVal),
				  sStringMember,
				  bCreateInRegistry);
							
		if(bCompare)
		{
			if(strcmp(sStringMember , (char*)sTmpVal))
				lRetVal = -1;     //value has changed
		}

		strcpy(sStringMember , (const char*)sTmpVal);
	}
	else
		lRetVal = 1;

	return lRetVal;
}*/


//.??? TD 72545
long CParam::WriteParam(char* pszSection, char* pszFieldName, DWORD dwRegType, const unsigned char* pVal, DWORD dwValSize, BOOL bCreate)
{
	long			lRetVal = ERROR_SUCCESS;
	int				iRetCode = OK;
	HKEY			hKey;
	
	lRetVal = OpenKey(&hKey, pszSection, bCreate);
		
	if(lRetVal == ERROR_SUCCESS)
	{	
		if(bCreate == FALSE)
		{
			// Check first if field available ...
			lRetVal = IsFieldAvailable(&hKey, pszFieldName, dwRegType); 
		}

		if(lRetVal == ERROR_SUCCESS)
		{
			// Write
			lRetVal = RegSetValueEx(	hKey, 
										pszFieldName, 
										0, 
										dwRegType, 
										pVal, 
										dwValSize);					
		}
		
		if(lRetVal != ERROR_SUCCESS)
		{
			iRetCode = 2;
		}	
	}	
	else
	{
		iRetCode = 1;

		CString strLogMsg;
		strLogMsg.Format("Failed to save [%s]", pszFieldName);
		_LOGMSG.LogMsg("Param", "SaveParam", 0, LOG_NONE, strLogMsg);
	}
	
	RegCloseKey(hKey);

	return iRetCode;
}


//.??? TD 72545
ParamRetCodeEnum CParam::ReadParam(char* pszSection, char* pszFieldName, DWORD dwRegType, unsigned char* pOutVal, DWORD dwOutValSize, const unsigned char* pDefaultVal, DWORD dwDefaultValSize, BOOL bCreate, BOOL bWriteToLog)
{
	ParamRetCodeEnum	eRetCode = PARAM_OK;
	long				lRetVal = ERROR_SUCCESS;
	HKEY				hKey;
	BOOL				bIsCreated = FALSE;
	
	lRetVal = OpenKey(&hKey, pszSection, bCreate, &bIsCreated);

	if(lRetVal == ERROR_SUCCESS)
	{
		if(bIsCreated == TRUE)
		{
			lRetVal = RegSetValueEx(	hKey,
										pszFieldName,
										0,
										dwRegType,
										(const unsigned char *)pOutVal,
										dwDefaultValSize);

			eRetCode = (lRetVal == ERROR_SUCCESS) ? PARAM_NOT_FOUND_CREATED_WITH_DEFAULT : PARAM_NOT_FOUND_FAILED_TO_CREATE;
		}
		else
		{
			// Read
			lRetVal = RegQueryValueEx(	hKey,
										pszFieldName,
										0,
										NULL,
										(unsigned char *)pOutVal,
										&dwOutValSize);

			if(lRetVal != ERROR_SUCCESS)
			{				 
				if(pOutVal != pDefaultVal)
				{
					if (dwRegType == REG_DWORD)
					{
						*pOutVal = *pDefaultVal;
					}
					else if (dwRegType == REG_SZ)
					{
						memset(pOutVal, 0, dwOutValSize);
						sprintf((char*)pOutVal, "%s", pDefaultVal);
					}
				}
				
				if(bCreate)
				{
					lRetVal = RegSetValueEx(	hKey,
												pszFieldName,
												0,
												dwRegType,
												(const unsigned char *)pOutVal,
												dwDefaultValSize);

					eRetCode = (lRetVal == ERROR_SUCCESS) ? PARAM_NOT_FOUND_CREATED_WITH_DEFAULT : PARAM_NOT_FOUND_FAILED_TO_CREATE;

					if(bWriteToLog == TRUE && LOG_BASIC_FLOW_CONTROL)
					{
						CString strLogMsg;
						strLogMsg.Format("Parameter [%s] not found in registry, ", pszFieldName);				

						strLogMsg += (lRetVal == ERROR_SUCCESS) ? "New one created!" : "Failed to create new one!";
						
						_LOGMSG.LogMsg("Param", "LoadParam", 0, LOG_NONE, strLogMsg);
					}

				}
				else // NOt Found, Dont create, accoording to request
				{
					eRetCode = PARAM_NOT_FOUND_NOT_OPENED_NOT_CREATED;				// NOt Found, Didn't create, accoording to request

					if(bWriteToLog == TRUE && LOG_BASIC_FLOW_CONTROL)
					{
						CString strLogMsg;
						strLogMsg.Format("Parameter [%s] not found in registry!", pszFieldName);
						
						_LOGMSG.LogMsg("Param", "LoadParam", 0, LOG_NONE, strLogMsg);
					}
				}
			}			
		}
	}	
	else
	{
		eRetCode = PARAM_NOT_OPENED_SET_WITH_DEFAULT;						//Cant Open Key, Param set to default

		if(bWriteToLog ==TRUE && LOG_LOGIC_ERROR)
		{
			CString strLogMsg;
			strLogMsg.Format("Failed to load [%s] [%s], value set to default! (lRetVal = %ld)", pszSection, pszFieldName, lRetVal);
			_LOGMSG.LogMsg("Param", "LoadParam", 0, LOG_NONE, strLogMsg, LOG_LEVEL_1);
		}
	}

	RegCloseKey(hKey);	

	return eRetCode;
}


//.??? TD 72545
long CParam::PassParamToHydra(char* pszSection, char* pszFieldName, DWORD dwVal, char* pszVal, BOOL bCreate, long lGroupNum, long lFlags, long lClientEventData, BOOL bIniFiled)
{
	long	lRetVal		= ERROR_SUCCESS;
	int		iRetCode	= OK;
	long	lHydFlags	= HOP_HYDRA_REMOTE_ONLY;

	//checking if this parameter should be local only and Hydra is active.
	if(_HOST.m_lHydraActive && lFlags != HOP_DIRECT_LOCAL)								
	{
		long	lLocalCheckRet = FIELD_NOT_EXISTS;

		lLocalCheckRet = IsLocalOnlyParameter(pszSection, pszFieldName, NULL, SAVE_PARAM, bCreate);

		if(lLocalCheckRet == FIELD_EXISTS)
			lFlags = HOP_DIRECT_LOCAL;
	}

	if ( _HOST.m_lHydraActive && !(lFlags & HOP_DIRECT_LOCAL) && (_HOST.m_pCFGDB != NULL) && _HOST.CheckConnectionStatus() )
	{
		CString strPathName((const unsigned char*)"Software\\PointOfSale\\PumpSrv");
		CString strSection(pszSection);
		CString strName(pszFieldName);

		BSTR    bstrServerPath;					
		BSTR	bstrSection;
		BSTR	bstrName;
		BSTR	bstrVal;
		
		char	szInterfaceName[MAX_STRING_LEN] = {0};

		try
		{			
			if (bIniFiled)						
			{
				lHydFlags |= HOP_CFG_INI_FILE;
				strPathName = _Module.m_server.m_cIniFile.GetFilePath();						//The IniFile path 
				strName	+= (pszVal == NULL) ? "[I]" : "[S]";
			}

			bstrServerPath	= strPathName.AllocSysString();
			bstrSection		= strSection.AllocSysString();
			bstrName		= strName.AllocSysString();

			if (pszVal == NULL)					//Pass dword value to Hydra
			{
				strcpy( szInterfaceName, "SaveNumParam");

				lRetVal = _HOST.m_pCFGDB->SaveNumParam(	bstrServerPath, bstrSection, bstrName, dwVal, bCreate, lHydFlags, 0, lGroupNum, lClientEventData);
			}
			else								//Pass string value to Hydra
			{
				strcpy( szInterfaceName, "SaveStrParam");

				CString cVal(pszVal);
				bstrVal		= cVal.AllocSysString();

				lRetVal = _HOST.m_pCFGDB->SaveStrParam(	bstrServerPath, bstrSection, bstrName, bstrVal, bCreate, lHydFlags,	0, lGroupNum, lClientEventData);
			}
		}
		catch(_com_error & e )
		{
			_LOGMSG.LogResult( 0, "ICFGDB", szInterfaceName, FALSE, 0, TRUE, e.Error(), e);

			lRetVal = 1;

			_HOST.TryToRestoreHydConn();			//trying to restore the com
		}
		catch(...)
		{
			if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(	0, "ICFGDB", szInterfaceName, TRUE, 0, (lRetVal != 0) ? TRUE : FALSE, lRetVal);
			
			_HOST.TryToRestoreHydConn();			//trying to restore the com
		}
		
		//freeing all the bstrs
		if(bstrServerPath)
			SysFreeString(bstrServerPath);

		if(bstrSection)
			SysFreeString(bstrSection);

		if(bstrName)
			SysFreeString(bstrName);

		if(bstrVal)
			SysFreeString(bstrVal);

		//debug information
		if ( lRetVal )
		{
			CString str;
			str.Format("Failed to save %s", pszFieldName);
			_LOGMSG.LogMsg("Param", "SaveParam", 0, LOG_NONE, str);
		}

		//setting the return value 
		iRetCode = lRetVal;
	}

	return iRetCode;
}


//.??? TD 72545
long CParam::IsFieldAvailable(const HKEY* phKey, const char* szFieldName, const DWORD dwRegType)
{
	long	lRetVal		= ERROR_SUCCESS;

	DWORD	dwTmpVal	= 0;
	char	szTmpVal[MAX_FIELD_VAL] = {0};
	
	DWORD			dwTmpValSize = -1;
	unsigned char*	pTmpVal	= NULL;

	
	switch (dwRegType)
	{
	case REG_DWORD:
		{
			dwTmpValSize = sizeof(dwTmpVal);
			pTmpVal = (unsigned char*)&dwTmpVal;				
		}break;
	case REG_SZ:
		{
			dwTmpValSize = sizeof(szTmpVal);
			pTmpVal = (unsigned char*)szTmpVal;					
		}break;
	}

	
	DWORD	dwLocalRegType = dwRegType;

	lRetVal = RegQueryValueEx(	*phKey,							// handle to key to query
								szFieldName,					// address of name of value to query
								0,
								&dwLocalRegType,				// address of buffer for value type
								pTmpVal,						// address of data buffer
								&dwTmpValSize);					// address of data buffer size);		

	if(dwRegType != dwLocalRegType)
		lRetVal = 1;
	
	return lRetVal;
}			


//.??? TD 72545 Create / Open Key
long CParam::OpenKey(HKEY* phKey, const char* szSectionName, const BOOL bCreate, BOOL* bIsCreated)
{
	long lRetVal = ERROR_SUCCESS;

	CString	strPathName(REGISTRY_ROOT_PATH_NAME);

	if(szSectionName != NULL)
	{
		strPathName += "\\";
		strPathName += szSectionName;
	}

	if(bCreate == TRUE)
	{
		DWORD dwDisposition = 0;
		// Create / Open Key
		lRetVal = RegCreateKeyEx(	HKEY_LOCAL_MACHINE,			// handle of an open key 
									(LPCTSTR)strPathName,		// address of subkey name 
									0,							// reserved 
									NULL,
									0,
									KEY_WRITE | KEY_READ,
									NULL,
									phKey,
									&dwDisposition);
		
		if (bIsCreated != NULL)
		{
			if (lRetVal == ERROR_SUCCESS)
			{
				*bIsCreated = (dwDisposition == REG_CREATED_NEW_KEY) ? TRUE : FALSE;
			}
		}
		
	}
	else
	{
		// Open Key
		lRetVal = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,				// handle of an open key 
								(LPCTSTR)strPathName,			// address of name of subkey to open
								0,								// reserved  REGSAM samDesired, 
								KEY_WRITE | KEY_READ,			// security access mask
								phKey);							// address of handle to open key); 
	}

	return lRetVal;
}

