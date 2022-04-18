// RegTable.cpp: implementation of the CRegTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegTable.h"
#include "StaticString.h"
#include <cmath>
#include "SQLPumpSrvFile.h"
#include "PumpSrvDatabase.h"
#include "ProductVersion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BYTE CRegTable::m_byReconfigInProgress = 0;

CRegTable::CRegTable()
{
	
}

CRegTable::~CRegTable()
{
	
}

//////////////////////////////////////////////////////////////////////
//Pumps and dispensers
//////////////////////////////////////////////////////////////////////

void CRegTable::LoadDispensersTable(long lPumpNumber , BOOL bCreate , BOOL bCompare , BOOL bRun)
{
	int i = 1 , iLastPump = MAX_PUMPS_64;
	DWORD dwTmp = 0 , dwVal = 0;
	TStaticString<STATIC_STRING_64> sTmpVal;
		
	//4.0.1.32 Start
	BYTE byNumBlenders = 0 , byNumRFID = 0;
	long lLinkedTerminalIndex = 0;
	//4.0.1.32 End

	SYSTEMTIME curSysTime;
	SZ_TAG_SYSTEMTIME szTagTime;

	CString sLogMsg , str , sTable("Pumps\\Pump") , tmpStr;
	char sNumber[8];

	//4.0.3.1
	if(bCompare)
	{
		//If we haven't already switched to STARTUP, switch
		//now
		if(!SERVER_STATE_STARTUP)
			_Module.m_server.SetServerState(SERVER_STARTUP);
	}

	_Module.m_server.m_cParam.LoadParam("Pumps", "VolumeMultiply", &dwTmp, (DWORD)1,	bCreate);
		
	_Module.m_server.m_cParam.LoadParam("Pumps" , "TerminalBaseNumber" , &dwTmp , (DWORD)32 , bCreate); //4.0.22.503

	_Module.m_server.m_cParam.LoadParam("Pumps" , "Support64Pumps" , &dwTmp , (DWORD)0 , bCreate);

	//4.0.5.21
	//_Module.m_server.m_cParam.LoadParam("Pumps" , "TerminalBaseNumber" , &dwTmp , (DWORD)32 , bCreate);  //4.0.22.503

	if(dwTmp)
	{
		_Module.m_server.m_lMaxPumps = MAX_PUMPS_64;  //set max pumps to 64
		_LOGMSG.LogMsg("PumpSrv initialized with 64 pumps");
	}	
	else
	{
		_Module.m_server.m_lMaxPumps = MAX_PUMPS;  //set max pumps to 32
		_LOGMSG.LogMsg("PumpSrv initialized with 32 pumps");
	}

	if(!bRun)
	{
		_Module.m_server.m_lMaxPumps = MAX_PUMPS_64;
		_LOGMSG.LogMsg("PumpSrv is being registered with 64 pump support");
	}

	if(bCreate)
	{
		int iPumpArraySize = _Module.m_server.m_lMaxPumps;
		int iGrowBy = -1;

		//If the pump array is currently 32 pumps long, set the 
		//array to grow by another 32 pumps if necessary
		if(_Module.m_server.m_lMaxPumps == MAX_PUMPS)
			iGrowBy = MAX_PUMPS;
		
		_Module.m_server.m_cMemPumpArray.SetSize(iPumpArraySize , iGrowBy);
		_Module.m_server.m_cPumpArray.SetSize(iPumpArraySize , iGrowBy);

		for (size_t PumpIndex = 0; PumpIndex < iPumpArraySize; ++PumpIndex)
		{
			_Module.m_server.m_cPumpArray[PumpIndex].SetNumber(PumpIndex + 1);
		}
	}
	
	//Set loop to run for just one pump
	if(lPumpNumber && !bCreate)
		i = iLastPump = lPumpNumber;
	
	for(;i <= iLastPump; i++)
	{
		memset(sNumber,0,sizeof(sNumber));

		sprintf(sNumber,"%02d",i);
		str = sTable + sNumber;

		if(bCreate)
		{
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ManufacturerName",sTmpVal, sTmpVal.Size() ,"Tokheim", bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "Brand",sTmpVal, sTmpVal.Size() ,"T", bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "Model",sTmpVal, sTmpVal.Size() ," ", bCreate);
			
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "BlenderType",&dwTmp,(DWORD)0,	bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ServiceMode",&dwTmp,(DWORD)SELF_SERVICE_MODE, bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PriceLevelSelect",&dwTmp,(DWORD)PRICE_LEVEL_ALLOW_DEFAULT, bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "SingleHose",&dwTmp,(DWORD)0,bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "StartButton",&dwTmp,(DWORD)0,bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "SupportOfflineTrs",&dwTmp,(DWORD)0,bCreate); // TD 348622


			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "BaudRate",&dwTmp,(DWORD)9600,bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "DeviceAddress",&dwTmp,(DWORD)i,bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "CommPort",&dwTmp,(DWORD)1,bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ParamCRC",&dwTmp,(DWORD)0,bCreate);

			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ReportNozzleOut",&dwTmp,(DWORD)0,bCreate); //2.9.4.0
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PumpVolumePrecision",&dwTmp,(DWORD)3,bCreate);

			//4.0.16.500 - TD 37522
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValueScaleFactor",&dwTmp,(DWORD)2,bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PPGScaleFactor",&dwTmp,(DWORD)3,bCreate);

			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "AutoPayOff",sTmpVal,sTmpVal.Size(),"A", bCreate);
			//4.0.3.61
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "SaleStacking",&dwTmp,(DWORD)0,bCreate);  

			//4.0.8.500
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "UsePumpKeypadPreset",&dwTmp,(DWORD)0,bCreate);
			
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "SatellitePump",&dwTmp,(DWORD)0,bCreate); //4.0.8.505
			
			//4.0.9.508
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PumpPricePrecision",&dwTmp,(DWORD)3,bCreate); //4.0.9.508
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ExtraOfflineTimeout",&dwTmp,(DWORD)0,bCreate); //4.0.9.508
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "UseTDM",&dwTmp,(DWORD)0,bCreate); //4.0.9.508
			//_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ResetPumpTransactionDisplayOnPriceChange",&dwTmp,(DWORD)0,bCreate); //4.0.9.508

			
			//4.0.15.501 - TD 35841
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "SlowDownVolume",&dwTmp,(DWORD)600,bCreate);
			
			//4.0.16.500 - TD 10718 (For Pilot version)
			// Removed the "Station..." from these keys names - this is a pump configuration //4.0.23.504 85954
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "Sales_Display_Mode",&dwTmp,(DWORD)0,bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "Sales_Display_Delay",&dwTmp,(DWORD)30,bCreate);
			
			//4.0.15.440 - TD 57732
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "DisablePulsesMechanism",&dwTmp,(DWORD)0,bCreate); //4.0.15.440
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "TransactionDeviation",&dwTmp,(DWORD)1,bCreate); //4.0.15.440

			// 4.0.28.500
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "VaporRecoverySupport",&dwTmp,(DWORD)0,bCreate);  // TD 243282 4.0.28.500
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PollingInterval",&dwTmp,(DWORD)0,bCreate);		//4.0.28.320
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "HardwareVersion", sTmpVal, sTmpVal.Size(), "", bCreate);	// CR 472673 

			for(int m = 1; m<= MAX_MODE_SCHEME; m++)
			{
				tmpStr = str;

				memset(sNumber,0,sizeof(sNumber));
				sprintf(sNumber,"%1d",m);
				
				tmpStr += "\\ModeScheme";
				tmpStr += sNumber;
				
				_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "ModeNumber",&dwTmp,(DWORD)1,bCreate);

				if(m==1)
					_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "ValidEntry",&dwTmp,(DWORD)1,bCreate);
				
				else
					_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "ValidEntry",&dwTmp,(DWORD)0,bCreate);
				
				_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "AutoDaily",&dwTmp,(DWORD)1,bCreate);
				_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "AutoWeekly",&dwTmp,(DWORD)0,bCreate);
				_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "AutoYearly",&dwTmp,(DWORD)0,bCreate);

				GetLocalTime(&curSysTime);		
				
				szTagTime.sNull = 0;
				SystemTime2Tag(&curSysTime,&szTagTime.sTime);
				_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "StartTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME) ,(char *)&szTagTime,bCreate);
				_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "EndTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME) ,(char *)&szTagTime,bCreate);

				memcpy(&szTagTime , "2000 1 1 0 0 0" , sizeof(szTagTime) -1);
				szTagTime.sNull = 0;

				_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "PeriodStartTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME) ,(char *)&szTagTime,bCreate);
				_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "PeriodEndTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME) ,(char *)&szTagTime,bCreate);

				_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "ServiceFeeSet",&dwTmp,(DWORD)0,bCreate);  
				
				_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "ServiceMode",&dwTmp,(DWORD)0,bCreate);
			}
		}
		
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "TerminalNumber",&dwTmp,(DWORD)i,bCreate);

		if(dwTmp && (dwTmp <= MAX_TERMINALS))
			lLinkedTerminalIndex = dwTmp - 1;
		
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "AntennaNumber",&dwTmp,(DWORD)0,bCreate);

		if(dwTmp)
			byNumRFID = 1;

		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ManualMode",&dwTmp,(DWORD)0,bCreate);

		_Module.m_server.m_lPumpsManualModeArray[i-1] = dwTmp;

		if((i == 1) || (i == 2))
		{
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwTmp,(DWORD)1,bCreate);
		}
		else
		{
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwTmp,(DWORD)0,bCreate);
		}

		//Eyal 4.0.0.79
		if(dwTmp)  //if this pump is valid
		{	
			//4.0.0.81
			//It is possible for a mistake in registry config to cause
			//m_lLastValidpump > m_lMaxPumps.  This will lead to a system
			//crash.
			if(i <= _Module.m_server.m_lMaxPumps)
				_Module.m_server.m_lLastValidPump = i;

			else
			{
				sLogMsg.Format("Pump configured, but max pumps supported is %d" , _Module.m_server.m_lMaxPumps);
				_LOGMSG.LogMsg(i,LOG_PUMP,sLogMsg);
			}

			//4.0.1.32
			_Module.m_server.m_cTerminalDevices[lLinkedTerminalIndex].SetNumberOfBlenders(byNumBlenders);
			_Module.m_server.m_cTerminalDevices[lLinkedTerminalIndex].SetNumberOfRFID(byNumRFID);
		}

		for(int p = 1; p<= MAX_POSITIONS_PER_PUMP; p++)
		{
			tmpStr = str;
			
			memset(sNumber,0,sizeof(sNumber));
			sprintf(sNumber,"%ld",p);
			
			tmpStr += "\\Position";
			tmpStr += sNumber;

			_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "Grade1", &dwVal, p,bCreate);
			
			/////////////////////////////////////////////////////////
			//DO NOT MODIFY dwVal PAST THIS POINT!!!!!!!!!!!!!!!!!!!!
			/////////////////////////////////////////////////////////

			//Set ValidEntry to pumps 1,2 and to positions 1,2,3
			if( (p == 1) || (p == 2) || (p == 3) ) //Positions 1,2,3
			{
				if ( (i == 1) || (i == 2) ) //Pumps 1,2
				{
					_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "ValidEntry",&dwTmp,(DWORD)1,bCreate);
				}
				else
				{	
					_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "ValidEntry",&dwTmp,(DWORD)0,bCreate);
				}
			}
			else
			{
				_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "ValidEntry",&dwTmp,(DWORD)0,bCreate);
			}

			//4.0.1.32 start
			//If this is a valid entry AND 
			//if a valid grade number has been configured 
			if(dwTmp && dwVal && (dwVal <= MAX_GRADES_99))//amram 4.0.10.502
			{
				//Check to see whether we have already been updated by this pump
				//(this can happen if the same grade has been configured
				//for more than one nozzle.
				if(_Module.m_server.m_cDevicesPerGrade[dwVal - 1].CanUpdate(i))
				{
					_Module.m_server.m_cDevicesPerGrade[dwVal - 1] += _Module.m_server.m_cTerminalDevices[lLinkedTerminalIndex];
					_Module.m_server.m_cDevicesPerGrade[dwVal - 1].UpdateMap(i);
				}
			}
			//4.0.1.32 end

			//YOU MAY MODIFY dwVal PAST THIS POINT

			//4.0.13.502
			dwVal = 0;
			_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "Address", &dwVal, 0,bCreate);

			//4.0.3.49 4.0.3.60
			//4.0.15.120 - CR 44000
			dwVal = 0;
			//_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "CumulateVolume", &dwVal, 0,bCreate);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "CumulateVolume",&dwVal,(DWORD)0,bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);

			//4.0.19.502 47235
			dwVal = 0;
			char sVal[MAX_STRING_LEN]; //4.0.15.40  TD 59209
			_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "BaseTotals",(char*)&sVal,MAX_STRING_LEN,"0",bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);

					 
			dwVal = 0;
			_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "ExceptionTotals",(char*)sVal,MAX_STRING_LEN,"0",bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
			dwVal = 0;
			_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "CurrentTotals",(char*)sVal,MAX_STRING_LEN,"0",bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);

			//4.0.31.??? TD 405462
			dwVal = 0;
			_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "CurrentAmountTotals",(char*)sVal,MAX_STRING_LEN,"0",bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
			dwVal = 0;
			_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "BaseAmountTotals",(char*)sVal,MAX_STRING_LEN,"0",bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);


			//4.0.14.501
			dwVal = 0;
			_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "TankNumber1", &dwVal, 0,bCreate);

			dwVal = 0;
			_Module.m_server.m_cParam.LoadParam((LPCSTR)tmpStr, "TankNumber2", &dwVal, 0,bCreate);

		}

		byNumRFID = byNumBlenders = 0;

		//Pump number 0 stands for all pumps
		if(!lPumpNumber && !bRun)
			Sleep(REGISTRATION_SLEEP_TIME);
	}
	
	//if(bCompare) send event to CL
	
	sLogMsg.Format("LoadDispensersTable - last valid pump detected is %d" , _Module.m_server.m_lLastValidPump);
	_LOGMSG.LogMsg(sLogMsg);

	return;
}


void CRegTable::LoadPumpsTable(long lPumpNumber , BOOL bCreate , BOOL bCompare , BOOL bRun)
{

	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();

	CPumpStatus stat;
	long lMisc = 0 , lMode = 1 , p = 0 , lTmpMode =0; //4.0.3.55

	long lLastPump = _Module.m_server.m_lMaxPumps;

	CString str;
	short n1 = 0 , n2 = 0 , n3 = 0;

	//4.0.3.1
	if(bCompare)
	{
		//If we haven't already switched to STARTUP, switch
		//now
		if(!SERVER_STATE_STARTUP)
			_Module.m_server.SetServerState(SERVER_STARTUP);
	}

	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
	size_t RealTimeInfoCount = 0;
	long lValid = (PumpSrvDatabase.GetRealTimeInfoCount(RealTimeInfoCount, __FUNCTION__, __LINE__) && (RealTimeInfoCount == _Module.m_server.m_lMaxPumps)) ? OK : Q_ERROR; // RFUEL-1711


	if(lValid == OK)
	{
 		if(LOG_BASIC_FLOW_CONTROL)
			_LOGMSG.LogMsg("Pump File is valid, reloading data");

		// RFUEL-3028 - Pulled out for debugging
		//Load server and shift info
		const bool bServerInfo = SQLPumpSrvFile.LoadServerInfo() > 0;
		const bool bShiftInfo = SQLPumpSrvFile.LoadShiftInfo() > 0;
	
		//4.0.8.500if(_Module.m_server.m_cInfo.lockStatus == UNLOCK)
		if(_Module.m_server.m_cServerInfo.GetServerStatus()== UNLOCK)
		{
			if(_Module.m_server.m_cCurrentShift.dwFormalShiftNumber != _Module.m_server.m_cCurrentShift.dwActiveShiftNumber)
			{
				_Module.m_server.m_cCurrentShift.dwFormalShiftNumber = _Module.m_server.m_cCurrentShift.dwActiveShiftNumber;
				SQLPumpSrvFile.SaveShiftInfo();

			}
		}	
		
		if(lPumpNumber && !bCreate)
		{
			p = lPumpNumber - 1;
			lLastPump = lPumpNumber;
		}

		for(;p < lLastPump;p++)
		{
			//Load info from PumpStat.qdx
			SQLPumpSrvFile.LoadInfo(p + 1);
			
			SQLPumpSrvFile.LoadPAPInfo(p+1); // RFUEL-229 Recovery of PAP transaction

			//load initial state for prepay state machine 
 			_Module.m_server.m_lAutoState[p] = PUMP_NONE_FIRST;
				
			CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

			stat = _Module.m_server.m_cPumpArray[p].GetStat();
			lMisc = stat.GetMisc();
			_Module.m_server.m_lPrePayReTryReservation[p] = TIME_BETWEEN_RETRYING_RESERVED_PRE_PAID_PUMP;  //4.0.24.351	//4.0.124.355 //4.0.27.60 231255
			
			//Check if the mode is manual and set the misc accordingly
			lMode = _Module.m_server.m_lPumpsManualModeArray[p];
			
 			if (lMode)
				lMisc |= MISC_MANUAL_MODE;
			else
				lMisc &= MISC_NOT_MANUAL_MODE;

			stat.SetMisc(lMisc);

			stat.SetStatus(UNKNOWN_STATUS);

			_Module.m_server.m_cPumpArray[p].SetStat(stat);

			 //TD 358952 - Sasol: Stuck transactions on POS related to thread safety around CPumpStatus

			
			//Load registry data:
			_Module.m_server.m_cPumpArray[p].LoadDefinition();

			if (_Module.m_server.m_cServerInfo.GetRequiredShiftProcessFlags())	  //4.0.7.504
			{
				str.Format("Recovery process , pump #%02d with shift #%d , formal shift %d status=%d",
				   p + 1,
				   _Module.m_server.m_cPumpArray[p].GetShiftNumber(),
				   _Module.m_server.m_cCurrentShift.dwActiveShiftNumber,
				   _Module.m_server.m_cPumpArray[p].GetLockStatus());

				_LOGMSG.LogMsg(p+1,LOG_PUMP,str);
			}			
			//4.0.2.24
			else  if((DWORD)_Module.m_server.m_cPumpArray[p].GetShiftNumber() !=
			   _Module.m_server.m_cCurrentShift.dwActiveShiftNumber)
			{
				
				str.Format("Found pump #%02d with shift #%d instead of %d", p + 1,
																		    _Module.m_server.m_cPumpArray[p].GetShiftNumber(),
																		    _Module.m_server.m_cCurrentShift.dwActiveShiftNumber);

				_LOGMSG.LogMsg(p+1,LOG_PUMP,str);

				_Module.m_server.m_cPumpArray[p].SetShiftNumber(_Module.m_server.m_cCurrentShift.dwActiveShiftNumber);
			}

			if(LOG_BASIC_FLOW_CONTROL)
			{		
				_Module.m_server.m_cPumpArray[p].GetTrsListCount(&n1,&n2,&n3);
				str.Format("Status %d, Res %d, ResExt %d, LockState %d, Shift %d, Waiting %d, Free %d, Unpaid %d, ",
							_Module.m_server.m_cPumpArray[p].GetStat().GetStatus(),
							_Module.m_server.m_cPumpArray[p].GetStat().GetRes(),
							_Module.m_server.m_cPumpArray[p].GetStat().GetResExt(),
							_Module.m_server.m_cPumpArray[p].GetLockStatus(),
							_Module.m_server.m_cPumpArray[p].GetShiftNumber(),
							n1,n2,n3);
				_LOGMSG.LogMsg(p+1,LOG_PUMP,str);
			}

			if(_Module.m_server.m_cPumpArray[p].GetLockStatus() == WAIT_LOCK)
				_Module.m_server.m_cPumpArray[p].SetLockStatus(STARTED);

			if(_Module.m_server.m_cPumpArray[p].GetLockStatus() == WAIT_RESPONSE)
				_Module.m_server.m_cPumpArray[p].SetLockStatus(LOCKED);

			_Module.m_server.SavePumpInfo(p+1);
		}	
	}
	else // failed on validity check
	{
		if (LOG_BASIC_FLOW_CONTROL) 
		{
			str.Format("Failed on Pump File Validity Check ! %ld, RealTimeInfoCount = %ld", lValid, RealTimeInfoCount);
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, str);
		}

		//4.0.7.506
		// Clear Server info and Pumps data
		_Module.m_server.ResetPSServerInfo();

		_Module.m_server.ResetPSShiftInfo();

		lValid = 1; // since we are re-initializing info for all pumps, we should write it to the db
		for (long p = 1; p <= _Module.m_server.m_lMaxPumps; p++)   //4.0.11290.2600
			_Module.m_server.ResetPSPumpInfo(lValid, p);
	}
}

void CRegTable::LoadFuellingPoints(long lNumber , BOOL bCreate , BOOL bCompare , BOOL bRun)
{
	LoadDispensersTable(lNumber , bCreate , bCompare , bRun);
	LoadPumpsTable(lNumber , bCreate , bCompare , bRun);
	
	return;
}

//////////////////////////////////////////////////////////////////////
//Mode schemes
//////////////////////////////////////////////////////////////////////

void CRegTable::LoadModeSchemeTable(BOOL bRun)
{
	long lIndex = 0;

	int iArraySize = _Module.m_server.m_lMaxPumps * MAX_MODE_SCHEME;
	int iGrowBy = -1;

	if(_Module.m_server.m_lMaxPumps == MAX_PUMPS)
		iGrowBy = MAX_PUMPS;

	_Module.m_server.m_cModeSchemeArray.SetSize(iArraySize , iGrowBy);

	for(int p = 1;p <= _Module.m_server.m_lMaxPumps;p++)
	{	
		for(int n = 1;n <= MAX_MODE_SCHEME;n++)		
		{						
			lIndex = _Module.m_server.GenerateModeSchemeIndex(p  , n );
			_Module.m_server.m_cModeSchemeArray[lIndex].SetIndex(p  , n  );
			_Module.m_server.m_cModeSchemeArray[lIndex].LoadDefinition(1);
		}
		
		if (!bRun)
			Sleep(REGISTRATION_SLEEP_TIME);
	}

	return;
}

void CRegTable::LoadWashProgramsTable(BOOL bCreate , BOOL bCompare , BOOL bRun)
{
	TStaticString<STATIC_STRING_64> sTmpStr;
	for(int iIndex=1;iIndex<=MAX_WASH_PROGRAMAS;iIndex++)
	{
		CString sWashProgram;
		sWashProgram.Format("WashPrograms\\WashProgram%02d",iIndex);   //RFUEL-2206

		_PARAM.LoadParam((LPCTSTR)sWashProgram.GetBuffer(0), "MapTaxLink",&_Module.m_server.m_WashProgramsData[iIndex].m_dwTaxLink,0,FALSE);

		_PARAM.LoadParam((LPCTSTR)sWashProgram.GetBuffer(0), "Price",&_Module.m_server.m_WashProgramsData[iIndex].m_dwPrice,0,FALSE);

		_PARAM.LoadParam((LPCTSTR)sWashProgram.GetBuffer(0), "DisplayOrder",&_Module.m_server.m_WashProgramsData[iIndex].m_dwOrder,0,FALSE);

		_PARAM.LoadParam((LPCTSTR)sWashProgram, "CCH_Product_Code", sTmpStr, sizeof(sTmpStr), "0", FALSE);
		_Module.m_server.m_WashProgramsData[iIndex].m_sProduct_Code = sTmpStr;
		
		_PARAM.LoadParam((char *)((LPCTSTR)sWashProgram), "FullName", (char *)(char *)sTmpStr, sTmpStr.Size() ,"Wash",FALSE);
		_Module.m_server.m_WashProgramsData[iIndex].m_sFullName = sTmpStr;	


	}
}
void CRegTable::LoadGradesTable(BOOL bCreate , BOOL bCompare , BOOL bRun)
{
	BOOL bBlenderUpdated = FALSE;

	DWORD dwTmp = 0, dwIsValid = 0;
	
	TStaticString<STATIC_STRING_64> sTmpStr;
	
	CString str , sTable("Grades\\Grade"),sTableMaxGrade("Grades");
	//amram 4.0.5550.1000 check if we have 99 grades
	_PARAM.LoadParam(sTableMaxGrade, "MaxGrades99",&dwTmp,(DWORD)0,bCreate);
	_Module.m_server.SetMaxGrade99(dwTmp);
	
	//4.0.26.501 - TD 116227
	_Module.m_server.m_cExtendedGradeMap.Empty(); //Empty the extended grades map before filling it
	_PARAM.LoadParam(sTableMaxGrade, "MaxGrades999",&dwTmp,(DWORD)0,bCreate);
	_Module.m_server.SetMaxGrade999(dwTmp);

	if(dwTmp)
		_LOGMSG.LogMsg("PumpSrv is working with 999 grades");

	char sNumber[8];
	
	_Module.m_server.SetMaxValidGradeNumber(0);//amram
	for(int i = 1; i <= MAX_GRADES_99; i++)
	{
		memset(sNumber,0,sizeof(sNumber));

		sprintf(sNumber,"%02d",i);
		str = sTable + sNumber;
			
		_PARAM.LoadParam((LPCSTR)str, "ValidEntry",&dwTmp,(DWORD)0,bCreate);
		
		dwIsValid = dwTmp;
		if(dwIsValid) //4.0.5.502 set the max valid grade 
		{
			_Module.m_server.SetMaxValidGradeNumber(i);
		}
		
		_PARAM.LoadParam((LPCSTR)str, "ConvertCode",&dwTmp,(DWORD)0,bCreate);
		_PARAM.LoadParam((LPCSTR)str, "ConvertCode2",&dwTmp,(DWORD)0,bCreate);

		_PARAM.LoadParam((LPCSTR)str, "Code",&dwTmp,(DWORD)i,bCreate);
		_Module.m_server.m_cDevicesPerGrade[i - 1].SetNACSCode(dwTmp);
		_Module.m_server.m_GradesRegistryData[i - 1].m_dwGradesCode = dwTmp;

		_PARAM.LoadParam((LPCSTR)str, "MapTaxLink",&dwTmp,(DWORD)0,bCreate);  
		_Module.m_server.m_GradesRegistryData[i - 1].m_dwTaxLink = dwTmp;

		_PARAM.LoadParam((LPCTSTR)str,"TypeOfMeasure",&dwTmp,0,TRUE); // TD 363041
		_Module.m_server.m_GradesRegistryData[i - 1].m_bTypeOfMeasure =  (dwTmp) ? TRUE : FALSE;

		_PARAM.LoadParam((LPCSTR)str, "PureProductA",&dwTmp,(DWORD)1,bCreate);
		_PARAM.LoadParam((LPCSTR)str, "PureProductB",&dwTmp,(DWORD)0,bCreate);
		_PARAM.LoadParam((LPCSTR)str, "BlendRatioA",&dwTmp,(DWORD)100,bCreate);
	
		//4.0.1.32
		//If this grade is blendable, each pump containing this grade
		//must have a blender.  We set the number of blenders
		//according to the number of pumps that have this grade.
		if((dwTmp != 100) && dwIsValid)
			_Module.m_server.m_cDevicesPerGrade[i - 1].SetNumberOfBlenders();
			
		_PARAM.LoadParam((LPCSTR)str, "TaxRate",&dwTmp,(DWORD)0,bCreate);
		_PARAM.LoadParam((LPCSTR)str, "DiscountPerUnit",&dwTmp,(DWORD)0,bCreate);
		_PARAM.LoadParam((LPCSTR)str, "MaxUnitForDiscount",&dwTmp,(DWORD)0,bCreate);

		//4.0.3.61
		_PARAM.LoadParam((LPCSTR)str, "MinUnitForDiscount",&dwTmp,(DWORD)0,bCreate);
		_PARAM.LoadParam((LPCSTR)str, "DiscountType",&dwTmp,(DWORD)0,bCreate);
		//4.0.3.61
		_PARAM.LoadParam((LPCSTR)str, "LoyaltyApplicable",&dwTmp,(DWORD)1,bCreate); //3.1.1.1
		_PARAM.LoadParam((LPCSTR)str, "CarWashDiscountApplicable",&dwTmp,(DWORD)1,bCreate); //4.0.14.1540 70776 //4.0.22.0

		if(i == 1)			
		{
			sTmpStr.Clear();
			_PARAM.LoadParam((LPCSTR)str, "FullName", (char *)sTmpStr, sTmpStr.Size() ,"Unleaded",bCreate);
			_Module.m_server.m_GradesRegistryData[i - 1].m_sFullName = sTmpStr;
			
			_Module.m_server.m_GradeData.SetGradeName(i, sTmpStr);
			
			sTmpStr.Clear();
			_PARAM.LoadParam((LPCSTR)str, "ShortName3", (char *)sTmpStr, sTmpStr.Size() ," 87",bCreate);
			
			sTmpStr.Clear();
			_PARAM.LoadParam((LPCSTR)str, "ShortName5", (char *)sTmpStr, sTmpStr.Size() ,"Unled",bCreate);

			//4.0.20.54 
			sTmpStr.Clear();
			_PARAM.LoadParam((LPCSTR)str, "AdditionalCode", (char *)sTmpStr, sTmpStr.Size() ," 87",bCreate);
			_Module.m_server.m_GradesRegistryData[i - 1].m_sAdditionalCode = sTmpStr;
		}
		else
		{
			sTmpStr.Clear();
			_PARAM.LoadParam((LPCSTR)str, "FullName", (char *)sTmpStr, sTmpStr.Size() ,"",bCreate);
			_Module.m_server.m_GradesRegistryData[i - 1].m_sFullName = sTmpStr;
			
			_Module.m_server.m_GradeData.SetGradeName(i, sTmpStr);
			
			sTmpStr.Clear();
			_PARAM.LoadParam((LPCSTR)str, "ShortName3", (char *)sTmpStr,sTmpStr.Size() ,"",bCreate);
			
			sTmpStr.Clear();
			_PARAM.LoadParam((LPCSTR)str, "ShortName5", (char *)sTmpStr, sTmpStr.Size() ,"",bCreate);

			//4.0.20.54 
			sTmpStr.Clear();
			_PARAM.LoadParam((LPCSTR)str, "AdditionalCode", (char *)sTmpStr, sTmpStr.Size() ," 87",bCreate);
			_Module.m_server.m_GradesRegistryData[i - 1].m_sAdditionalCode = sTmpStr;
		}

		//4.0.8.503
		_PARAM.LoadParam((LPCSTR)str, "Old_PriceCash_SelfService",&dwTmp,(DWORD)100,bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
		_PARAM.LoadParam((LPCSTR)str, "Old_PriceCash_FullService",&dwTmp,(DWORD)100,bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
		_PARAM.LoadParam((LPCSTR)str, "Old_PriceCredit_SelfService",&dwTmp,(DWORD)100,bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
		_PARAM.LoadParam((LPCSTR)str, "Old_PriceCredit_FullService",&dwTmp,(DWORD)100,bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
		_PARAM.LoadParam((LPCSTR)str, "New_PriceCash_SelfService",&dwTmp,(DWORD)100,bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
		_Module.m_server.SetGradePriceToArray(i, dwTmp, SELF_CASH);
		_PARAM.LoadParam((LPCSTR)str, "New_PriceCash_FullService",&dwTmp,(DWORD)100,bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
		_Module.m_server.SetGradePriceToArray(i, dwTmp, FULL_CASH);
		_PARAM.LoadParam((LPCSTR)str, "New_PriceCredit_SelfService",&dwTmp,(DWORD)100,bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
		_Module.m_server.SetGradePriceToArray(i, dwTmp, SELF_CREDIT);
		_PARAM.LoadParam((LPCSTR)str, "New_PriceCredit_FullService",&dwTmp,(DWORD)100,bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
		_Module.m_server.SetGradePriceToArray(i, dwTmp, FULL_CREDIT);
		_PARAM.LoadParam((LPCSTR)str, "Prev_Shift_Price",&dwTmp,(DWORD)100,bCreate,TRUE,0,HOP_DIRECT_LOCAL,TRUE);		 //4.0.32.??? TD 405462

		_PARAM.LoadParam((LPCSTR)str, "Day_Lowest_Price", &dwTmp, (DWORD)9999, bCreate, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
		_Module.m_server.SetGradePriceToArray(i, dwTmp, PRICE_PROTECTION_LOWEST_PRICE);

		//_Module.m_server.m_lGradePriceArray;

		//4.0.8.503
		
		_PARAM.LoadParam((LPCSTR)str, "RewardCentsPerUnit",&dwTmp,(DWORD)0,bCreate);													
		_PARAM.LoadParam((LPCSTR)str, "RewardDays",&dwTmp,(DWORD)0,bCreate);		
		
		

		//4.0.5.0
		_PARAM.LoadParam((LPCSTR)str, "MinUnitForCoupon",&dwTmp,(DWORD)0,bCreate);  
		_PARAM.LoadParam((LPCSTR)str, "MaxUnitForCoupon",&dwTmp,(DWORD)0,bCreate);  

		
		_PARAM.LoadParam((LPCSTR)str, "ParamCRC",&dwTmp,(DWORD)0,bCreate);

		//4.0.26.501 - TD 116227
		_PARAM.LoadParam((LPCSTR)str, "ExtendedGradeId",&dwTmp,(DWORD)i,bCreate);
		if (dwIsValid && _Module.m_server.GetMaxGrade999())
		{
			_Module.m_server.m_cExtendedGradeMap.Insert(i, dwTmp);
		}
		
		if (bRun == FALSE)
			Sleep(REGISTRATION_SLEEP_TIME);
	}
	
	return;
}

void CRegTable::LoadPureProductsTable(BOOL bCreate , BOOL bCompare , BOOL bRun)
{
	//None of these parameters are relevant during run-time, so we exit
	//the method if in the middle of run time update (indicated by bCreate == FALSE
	//and bCompare == TRUE)
	if(bCreate)  
	{
		DWORD dwTmp = 0;
		TStaticString<STATIC_STRING_64> sTmpStr;
		
		CString str , sTable("PureProducts\\PureProduct");
		char sNumber[8];

		for(int i = 1; i <= MAX_PURE_PRODUCTS; i++)
		{
			memset(sNumber,0,sizeof(sNumber));
			sprintf(sNumber,"%02d",i);
			str = sTable + sNumber;
				
			_PARAM.LoadParam((LPCSTR)str, "ValidEntry",&dwTmp,(DWORD)0,TRUE);
			
			_PARAM.LoadParam((LPCSTR)str, "Code",&dwTmp,(DWORD)i,TRUE);
			_PARAM.LoadParam((LPCSTR)str, "Blendable",&dwTmp,(DWORD)0,TRUE);
			
			_PARAM.LoadParam((LPCSTR)str, "TermalExpansion",&dwTmp,(DWORD)0,TRUE);
			
			_PARAM.LoadParam((LPCSTR)str, "ParamCRC",&dwTmp,(DWORD)0,TRUE);

			_PARAM.LoadParam((LPCSTR)str, "Name", (char *)sTmpStr, sTmpStr.Size(),"",TRUE);
			
			//4.0.1.0
			if (bRun == FALSE)
				Sleep(REGISTRATION_SLEEP_TIME);
		}
	}
	
	return;
}

//////////////////////////////////////////////////////////////////////
//GeneralParam table and sub tables
//////////////////////////////////////////////////////////////////////

void CRegTable::LoadPumpSrvInternalParams()
{
	DWORD dwTmpVal = 0;
	TStaticString<STATIC_STRING_64> sTmpStr;

	CString sTable("GeneralParam") , sParam;

	SYSTEMTIME curSysTime;
	SZ_TAG_SYSTEMTIME szTagTime;
	
	GetLocalTime(&curSysTime);		
	szTagTime.sNull = 0;
	SystemTime2Tag(&curSysTime,&szTagTime.sTime);

	sParam = "Version";

	CString sVersion = PUMPSRV_VERSION;
	if(_PARAM.SaveParam("GeneralParam", "Version" , sVersion.GetBuffer(0)))
		_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,sTmpStr,sTmpStr.Size(), sVersion.GetBuffer(0),TRUE);

	sParam = "TransactNumber";
	DWORD dwInitTrsNumber = _Module.m_server.GetFuelTransactionNumberMinValue(); //4.0.23.260 111318
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_dwTransactNumber),(DWORD)dwInitTrsNumber,TRUE,TRUE,_HOST.m_lGroupNum,HOP_DIRECT_LOCAL,TRUE ); //4.0.9.512					 

	sParam = "ActiveReadNumber";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_dwActiveReadNumber),(DWORD)1,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE );

	sParam = "ActiveShiftNumber";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_cCurrentShift.dwActiveShiftNumber),(DWORD)1,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE); //4.0.8.503

	sParam = "InMaintenanceQdxNum";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_dwInMaintenanceQdxNum),(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE); //33898

	sParam = "PumpTotalIndex";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_dwTotalIndex),(DWORD)1,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE); //4.0.8.503

	sParam = "TankReadIndex";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_dwTankReadIndex),(DWORD)1,TRUE);

	sParam = "FuelDeliveryIndex";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_dwDeliveryIndex),(DWORD)1,TRUE , TRUE,0,HOP_DIRECT_LOCAL,TRUE); //4.0.8.503

	sParam = "AlarmIndex";	
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_dwAlarmIndex),(DWORD)1,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE); //4.0.8.503

	sParam = "CarWashIndex";	
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_dwCarWashIndex),(DWORD)1,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE); //4.0.8.503
	
	sParam = "ItemsIndex";	
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_dwItemsIndex),(DWORD)1,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);

	sParam = "OLAIndex";	
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_dwOLAIndex), 1,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);

	sParam = "WetStockDone";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)1,TRUE , TRUE,0,HOP_DIRECT_LOCAL,TRUE); //4.0.8.503

	sParam = "OlaMaxDeclineMessToDisplay";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)100,TRUE);

	sParam = "CreateFleetRecord";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)0,TRUE);
	_Module.m_server.m_lCreateFleetRecord = dwTmpVal;

	//4.0.15.151
	sParam = "WetstockMemoCounter";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)0,TRUE , TRUE,0,HOP_DIRECT_LOCAL,TRUE); 

 	sParam = "SignInCashiersCounter";		//4.0.22.506 TD 70956
	_PARAM.LoadParam((LPCSTR)sTable, (LPCSTR)sParam, &(dwTmpVal), (DWORD)0, TRUE , TRUE, 0, HOP_DIRECT_LOCAL, TRUE); 

	//load the counter of the receipt //4.0.15.330
	sParam = "Counter";
	_PARAM.LoadParam((LPCSTR)"Receipt\\Receipt00", (LPCSTR)sParam, &(dwTmpVal), (DWORD)1, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE ); //4.0.15.440
	_Module.m_server.m_lReceipt00Counter = (long) dwTmpVal ;

	_Module.m_server.m_cPriceChangeFlags.LoadFlags();  //4.0.8.501
	//4.0.8.501
	
	sParam = "ProcessLockRequest";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)0,TRUE);

	sParam = "ProcessLockStatus";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)0,TRUE);

	sParam = "GradesCRC";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)0,TRUE);

	sParam = "PumpsCRC";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)0,TRUE);

	sParam = "PureProductsCRC";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)0,TRUE);

	sParam = "TanksCRC";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)0,TRUE);

	sParam = "CarWashCRC";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)0,TRUE);
	
	//4.0.10.1000
	sParam = "OLAPresentCardName";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)0,TRUE);
	_Module.m_server.m_bPresentCardName = dwTmpVal;

	//4.0.20.52
	sParam = "PrevShiftLastTrsNum";
	DWORD	dwTmp = 0x00000000;
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&dwTmp,_Module.m_server.m_dwTransactNumber,TRUE,TRUE,_HOST.m_lGroupNum,HOP_DIRECT_LOCAL,TRUE ); 				 

	//4.0.20.52 (62818)
	_PARAM.LoadParam("GeneralParam", "StartDeliveryTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME) , (char *)&szTagTime, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	_PARAM.LoadParam("GeneralParam", "EndDeliveryTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME) , (char *)&szTagTime, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

	//4.0.20.54 46943
	sParam = "OfflineTrsInShift"; 
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(_Module.m_server.m_dwOfflineTrsInShift),(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);

	//4.0.22.503 - TD 72189
	sParam = "ChangeProductGradeId";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&dwTmpVal,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
	_Module.m_server.m_cServerInfo.SetChangeProductGradeId(dwTmpVal);

	sParam = "ChangeProductTankId";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&dwTmpVal,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
	_Module.m_server.m_cServerInfo.SetChangeProductTankId(dwTmpVal);

	sParam = "ChangeProductName";
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,sTmpStr,sTmpStr.Size(),"",TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
	_Module.m_server.m_cServerInfo.SetChangeProductName(sTmpStr);

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "WaitForFinalizeResponse", &dwTmpVal, 0, TRUE);//RFUEL-2117 
	_Module.m_server.m_bWaitForFinalizeResponse = dwTmpVal;

	dwTmpVal = 1;
	sParam = "SingleCoreActive";  //4.0.24.90 - Support working with a single core TD 106227
	_PARAM.LoadParam((LPCSTR)sTable,(LPCSTR)sParam,&(dwTmpVal),(DWORD)1/*1 = Active*/,TRUE,TRUE,0,HOP_DIRECT_LOCAL);
	_Module.m_server.SetSingleCoreActive(dwTmpVal);
	
	// TD 371850 (Merges from TD 356420)
	if (TRUE == _Module.m_server.m_cProtectedDataHandler.GetPCIActiveFlag() || ExceptionLogLevelNone == _LOGMSG.GetExceptionLogLevel())
	{
		CExceptionHandler::ExceptionHandler().SetExceptionLogLevel(ExceptionLogLevelNone);
	}
	else
	{
		CExceptionHandler::ExceptionHandler().SetExceptionLogLevel(ExceptionLogLevelHigh);
	}
	
	return;
}

BOOL CRegTable::LoadOLATable(BOOL bCreate , BOOL bCompare)
{
	BOOL bNoErrorsSoFar = TRUE;
	DWORD dwTmpVal = 0 , dwIsOlaRemote = 0;
	TStaticString<STATIC_STRING_8> sTmpStr;
	CString sTable("GeneralParam") , sParam;

	DWORD dwDefaultOlaSrv = 0;

	if(bCompare)
	{
		//4.0.2.37 - make sure no reconfiguration attempts are made in middle
		//of transactions
		if(_Module.m_server.GetOLAServiceStatus() == SERVICE_IN_MIDDLE_OF_OPERATION)
		{
			_LOGMSG.LogMsg("LoadOLATable exiting without doing nothing because OLA is in middle of session");
			bNoErrorsSoFar = FALSE;
		}

		else
			_Module.m_server.SetServerState(SERVER_STARTUP);
	}


	if(bNoErrorsSoFar)
	{
		sParam = "Region";
		_PARAM.LoadParam(sTable,sParam,sTmpStr,sTmpStr.Size(),"USA",bCreate);

		if(!memcmp(sTmpStr,"USA",3))
		{
			_Module.m_server.m_dwRegion = REGION_USA;
			dwDefaultOlaSrv = OLASRV_POSITIVE;
		}
		else if(!memcmp(sTmpStr,"UK",2))
		{
			_Module.m_server.m_dwRegion = REGION_UK;
			dwDefaultOlaSrv = OLASRV_PROGRESS;
		}
		else if(!memcmp(sTmpStr,"ISR",3))
		{
			_Module.m_server.m_dwRegion = REGION_ISRAEL;
			dwDefaultOlaSrv = OLASRV_POSITIVE;
		}
		//4.0.12.506
		else if(!memcmp(sTmpStr,"EUR",3))
		{
			_Module.m_server.m_dwRegion = REGION_EUROPE;
		}
		//4.0.12.506
		else if(!memcmp(sTmpStr,"AUS",3))
		{
			_Module.m_server.m_dwRegion = REGION_AUSTRALIA;
		}

		_PARAM.LoadParam(sTable,"OLASrvInUse",&dwTmpVal, dwDefaultOlaSrv ,bCreate);
		_PARAM.LoadParam(sTable,"IsOlaRemote",&dwIsOlaRemote, 0 ,bCreate);
		if(dwIsOlaRemote)
			dwTmpVal |= IS_OLA_REMOTE;

		//If we are creating the parameter, or loading its value from the
		//registry during PumpSrv initialization, we initialize the 
		//relevant parameter in PumpSrv
		if(bCreate || (!bCreate && !bCompare))
			_Module.m_server.m_dwOLASrvInUse = dwTmpVal;

		//Changing the region may have altered the OLA server in use, or the OLA
		//srv in use may have been changed by the user.
		if(bCompare)
		{
			if(_Module.m_server.m_dwOLASrvInUse != dwTmpVal)
				_Module.m_server.SwitchServer__(dwTmpVal);  //4.0.2.37
		}

		_PARAM.LoadParam(sTable,"OLALogDeclineReq",&_Module.m_server.m_dwOLALogDeclineReq, 0 ,bCreate);

		_PARAM.LoadParam("GeneralParam","OLADeclineMessage",&dwTmpVal,0,TRUE);

		_PARAM.LoadParam(sTable,"OLAByController",&_Module.m_server.m_dwOLAByController, 0 ,bCreate);

		_PARAM.LoadParam(sTable,"OLAAutoReceiptCheck",&_Module.m_server.m_dwAutoReceiptCheck, 0,bCreate);

		_PARAM.LoadParam(sTable,"OLAAutoForce",&_Module.m_server.m_dwAutoForce, 1 ,bCreate);

		//4.0.3.55
		_PARAM.LoadParam(sTable, "OlaWaitingMessTimeOut",&_Module.m_server.dwOlaWaitingMessTimeOut,(DWORD)8000,TRUE);
		
		//The parameters OLAResponseTimeOut and OLACheckingInterval scale, was second. 
		//since we change the scale to be millisecond, we need to update an old parameter in case it wasn't done manually. 
		//We assume that if the values are too small then it wasn't update, and we do it here automatically.
		_PARAM.LoadParam(sTable,"OLAResponseTimeOut",&_Module.m_server.m_dwOLAResponseTimeOut, 35 ,bCreate);
		if (_Module.m_server.m_dwOLAResponseTimeOut < 1000)
		{
			_Module.m_server.m_dwOLAResponseTimeOut *= 1000;
			_PARAM.SaveParam("GeneralParam","OLAResponseTimeOut",_Module.m_server.m_dwOLAResponseTimeOut);
		}

		_PARAM.LoadParam(sTable,"OLACheckingInterval",&_Module.m_server.m_dwOLACheckingInterval, 1000 ,bCreate);
		if (_Module.m_server.m_dwOLACheckingInterval < 10)
		{
			if (_Module.m_server.m_dwOLACheckingInterval <= 0)
				_Module.m_server.m_dwOLACheckingInterval  = 1;

			_Module.m_server.m_dwOLACheckingInterval *= 1000;
			_PARAM.SaveParam("GeneralParam","OLACheckingInterval",_Module.m_server.m_dwOLACheckingInterval);
		}
	}

	//4.0.2.36
	_PARAM.LoadParam(sTable,"SaveDebitZeroCompleteData",&_Module.m_server.m_dwSaveDebitZeroCompleteData, 0 ,bCreate);
	_PARAM.LoadParam(sTable, "SaveZeroPakTrs", &_Module.m_server.m_dwSaveZeroPakTrs, 0, bCreate);

	return bNoErrorsSoFar;
}


/******************************************************************************
 Description:	Forecourt lock parameter that not need to reconfiguration 	
 Returns:      	None
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			23/9/2003   10:58		Start
******************************************************************************/
void CRegTable::LoadForecourtLockConfiguration()
{
	DWORD dwTmp = 0;

	_PARAM.LoadParam("GeneralParam","ShiftLock",&dwTmp,1,TRUE);
	_PARAM.LoadParam("GeneralParam","ShiftEOD",&dwTmp,1,TRUE); // CR 475695
	_PARAM.LoadParam("GeneralParam", "PriceChangeLock",&dwTmp,(DWORD)1,TRUE);
	
	//4.0.3.21
	_PARAM.LoadParam("GeneralParam" , "LockOnStopAll" , &dwTmp, (DWORD)0 , TRUE);
	
	//4.0.11.502
	_PARAM.LoadParam("GeneralParam","InformPriceChangeTimeOut(Sec)",&dwTmp, (DWORD)0,TRUE);	 
    _Module.m_server.m_cPriceChangeFlags.SetInformPriceChangeTimer(dwTmp);

	//4.0.9.512 
	_PARAM.LoadParam("GeneralParam","InformPriceChange",&dwTmp,0,TRUE);	 
    _Module.m_server.SetInformPriceChangeFlags(dwTmp);

	_PARAM.LoadParam("GeneralParam", "ModeChangeNoOpenPrp",&dwTmp,(DWORD)0,TRUE);//4.0.9.506 //amram
	_Module.m_server.m_lModeChangeNoOpenPrp =  dwTmp;  
	_PARAM.LoadParam("GeneralParam", "ModeChangeOnIdle",&dwTmp,(DWORD)0,TRUE);//4.0.9.506 //amram
	_Module.m_server.m_lModeChangeOnIdle    =  dwTmp;
	_PARAM.LoadParam("GeneralParam", "ResetPumpTransactionDisplayOnPriceChange",&dwTmp,(DWORD)0,TRUE);//4.0.9.506 //amram

	_PARAM.LoadParam("GeneralParam", "PriceChangeLockEx",&dwTmp,(DWORD)0,TRUE);//4.0.9.510 //amram
	_Module.m_server.SetPriceChangeLockEx(dwTmp);
	
	_PARAM.LoadParam("GeneralParam" , "EOSMaintenanceLockForecourt" , &dwTmp, (DWORD)0 , TRUE);//4.0.10.501 TD 14801
}

void CRegTable::LoadForecourtLimitSettings(BOOL bCreate , BOOL bCompare)
{
	DWORD dwVal =0;
	if(bCompare)
		_Module.m_server.SetServerState(SERVER_STARTUP);

	_PARAM.LoadParam("GeneralParam","StationLimitType",&_Module.m_server.m_dwStationLimitType, STATION_LIMIT_BY_VALUE ,bCreate);
	
	_PARAM.LoadParam("GeneralParam","VolumeLimit",&_Module.m_server.m_dwVolumeLimit, 50 ,bCreate);
	
	_PARAM.LoadParam("GeneralParam","CashLimit",&_Module.m_server.m_dwCashLimit, 50 ,bCreate);
	
	_PARAM.LoadParam("GeneralParam","CreditLimit",&_Module.m_server.m_dwCreditLimit, 35 ,bCreate);

//	_PARAM.LoadParam("GeneralParam", "UseServiceFee",(DWORD*)&_Module.m_server.m_bUseServiceFee,(DWORD)0,bCreate);  //4.0.6.505

	_PARAM.LoadParam("GeneralParam", "ForceCreditLimitCheck",&_Module.m_server.m_dwForceCreditLimitCheck,(DWORD)0,bCreate);
	
	//4.0.3.32	
	dwVal = 0;
	_PARAM.LoadParam("GeneralParam","PrePayLimit",&dwVal, 0 ,bCreate);
	_Module.m_server.SetPrePayLimit(dwVal);
	//4.0.3.32

	//4.0.3.44
	dwVal = 0;
	_PARAM.LoadParam("GeneralParam","ForcePreSetToLimitSite",&_Module.m_server.m_dwForcePreSetToSiteLimit, 0 ,bCreate);

	//4.0.3.44
	
	//4.0.7.500
	_PARAM.LoadParam("GeneralParam","RunningValueThreshold",&_Module.m_server.m_dwRunningValueThreshold, 0 ,bCreate);
	//TD 37244
	_PARAM.LoadParam("GeneralParam","EnhancedRollingUpdate",&_Module.m_server.m_dwEnhancedRollingUpdate, 0 ,bCreate);

	// CR 438993
	_PARAM.LoadParam("GeneralParam", "GradePriceMinimumLimit", &dwVal, 0, bCreate);
	_Module.m_server.SetGradePriceMinimumLimit(dwVal);

	_PARAM.LoadParam("GeneralParam","ContactlessCreditLimit",&_Module.m_server.m_dwContactlessCreditLimit, 150 ,bCreate); //  [1/18/2015 bm250159] CR 274401
	
	_PARAM.LoadParam("GeneralParam","ContactlessDebitLimit",&_Module.m_server.m_dwContactlessDebitLimit, 100 ,bCreate);   //  [1/18/2015 bm250159] CR 274401
	_Module.m_server.m_cFuelingRestriction.Init(bCreate);	//4.0.22.501 TD 70956

	return;
}

void CRegTable::LoadWetStockConfiguration(BOOL bCreate , BOOL bCompare)
{
	DWORD dwTmp = 0;
	TStaticString<STATIC_STRING_8> sTmpStr;

	if(bCompare)
		_Module.m_server.SetServerState(SERVER_STARTUP);

	// Wet Stock mode - Get wet stock data: 
	//(0) not at all. 
	//(1) every X minute. 
	//(2) at time hh:mm.
	_PARAM.LoadParam("GeneralParam","WetStockMode",&_Module.m_server.m_dwWetStockMode, 0,bCreate);

	// Wet Stock interval (stored in seconds in the registry), 
	//in case m_dwWetStockMode == 1.
	//The value is translated to milliseconds.
	_PARAM.LoadParam("GeneralParam","WetStockInterval",&dwTmp , 60 ,bCreate);
	//_Module.m_server.m_dwWetStockInterval = dwTmp * 1000;  //Translate to milliseconds //4.0.5.39
	_Module.m_server.m_dwWetStockInterval = dwTmp;  //Translate to milliseconds 

	//Wet Stock flags, to use with the automatic process
	_PARAM.LoadParam("GeneralParam", "WetStockTime", (char *)sTmpStr,sTmpStr.Size(), "0000",bCreate ,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
	
	//Wet Stock flags, to use with the automatic process
	_PARAM.LoadParam("GeneralParam", "WetStockFlags",&dwTmp,(DWORD)0, bCreate);

	//Check to see whether we use EOD QDX files.
	_PARAM.LoadParam("GeneralParam", "WetstockMaint",&_Module.m_server.m_dwWetStockMaint,(DWORD)0,bCreate);

	// Wet Stock clear time interval (in milliseconds) default = 10000(10 seconds)
	_PARAM.LoadParam("GeneralParam","WetStockClearTime",&_Module.m_server.m_dwWetStockClearTime,60000 ,bCreate);

	// Wet Stock timeout (maximum time for state) (in milliseconds) default = 900000(900 seconds = 15 minutes)
	_PARAM.LoadParam("GeneralParam","WetStockStateTimeout",&_Module.m_server.m_dwWetStockStateTimeout, 900000 ,bCreate); //TD 351225 - Wetstock report does not print after wetstock process had arrived into a bad state
	
	//4.0.7.501
	_PARAM.LoadParam("GeneralParam","WetstockMemoFlags",&dwTmp , 0 ,bCreate);
	_Module.m_server.m_dwWetStockMemoFlags = dwTmp;  //Translate to milliseconds 

	//4.0.22.503 - TD 69905	
	_PARAM.LoadParam("GeneralParam","WetStockReportIncludeOldData",&_Module.m_server.m_dwWetStockReportIncludeOldData, (DWORD)1 ,bCreate);

	return;
}

void CRegTable::LoadTimers(BOOL bCreate , BOOL bCompare)
{
	DWORD dTmp;
	if(bCompare)
		_Module.m_server.SetServerState(SERVER_STARTUP);
 
	//Internal pumpsrv parameter, can be reconfigured manually
	_PARAM.LoadParam("GeneralParam","WatchdogInterval(mSec)",&_Module.m_server.m_dwWatchdogInterval, 5000 ,TRUE);

	_PARAM.LoadParam("GeneralParam","PrePayAuthorizedTimeOut",&_Module.m_server.m_dwPrePayAuthorizedTimeOut, 300 ,bCreate);

	_PARAM.LoadParam("GeneralParam","PAKAutoReconciledTimer(sec)" ,&dTmp, 300 ,bCreate);//4.0.9.506 //amram
	_Module.m_server.SetPAKAutoReconcileTimer(dTmp);

	
	_PARAM.LoadParam("GeneralParam","MemoInterval",&_Module.m_server.m_dwMemoInterval,0 ,bCreate);
	if (_Module.m_server.m_dwMemoInterval && _Module.m_server.m_dwMemoInterval < MIN_MEMO_INTERVAL_TIMER) //4.0.15.200
	{
		_Module.m_server.m_dwMemoInterval = MIN_MEMO_INTERVAL_TIMER;
	}
	
	_PARAM.LoadParam("GeneralParam" , "NozzleOutTimeout(Sec)" , &_Module.m_server.m_dwNozzleLeftOut , (DWORD)0 , bCreate);
	
	_PARAM.LoadParam("GeneralParam", "SuppressedDriveoffTimeout", &_Module.m_server.m_dwSuppressedDriveoffTimeout,(DWORD)0,bCreate);
	
	_PARAM.LoadParam("GeneralParam", "AviTrsTimeOut",&_Module.m_server.m_dwAviTrsTimeOut,(DWORD)60,bCreate);

	_PARAM.LoadParam("GeneralParam", "PAPDeclineTimer(Sec)",&_Module.m_server.m_dwPAPDeclineTimeValue,(DWORD)0,bCreate); //4.0.12.501

	_PARAM.LoadParam("GeneralParam", "UnbalancedPrPTimer(Sec)",&_Module.m_server.m_dwUnbalancedPrPTimer,(DWORD)0,bCreate); //4.0.13.500

	//4.0.5550.1504
	_PARAM.LoadParam("GeneralParam", "DataBaseMaintenanceTimeout(Sec)",&_Module.m_server.m_dwDataBaseMaintenanceTimeout,(DWORD)300,bCreate);

	//4.0.14.501
	_PARAM.LoadParam("GeneralParam", "LoyaltyTransactionTimeout(Min)",&_Module.m_server.m_dwLoyaltyTransactionTimeout,(DWORD)0,bCreate);

	_PARAM.LoadParam("GeneralParam", "LoyaltyTransactionCompleteTimeout(Sec)",&_Module.m_server.m_dwLoyaltyTransactionCompleteTimeout,(DWORD)120,bCreate); //367311

	_PARAM.LoadParam("GeneralParam", "SecondTrsAuthTimeout(Sec)",&_Module.m_server.m_dwSecondTrsAuthTimeout,(DWORD)0,bCreate);//4.0.15.501
	
	_PARAM.LoadParam("GeneralParam", "CheckAndReserveTimeout",&_Module.m_server.m_dwCheckAndReserveTimeOut,(DWORD)120,bCreate);//4.0.17.20 - TD 26384

	//4.0.21.500 TD 66935
	_PARAM.LoadParam("GeneralParam","PumpLockTimeOut(Sec)",&_Module.m_server.m_dwPumpLockTimeOut, 0 ,TRUE);
	
	//4.0.22.503 - TD 72189
	_PARAM.LoadParam("GeneralParam","CloseOpenSalesTimeOut",&_Module.m_server.m_dwCloseOpenSalesTimeOut, 0 ,TRUE);

	//4.0.19.820 - TD 78775
	_PARAM.LoadParam("GeneralParam","PriceChangeWithAckPerPumpTimeout_sec",&_Module.m_server.m_dwPriceChangeWithAckPerPumpTimeout, 180 ,TRUE);
	
	return;
}


void CRegTable::LoadAVIParameters(BOOL bCreate , BOOL bCompare)
{
	DWORD dwTmp = 0;

	if(bCompare)
		_Module.m_server.SetServerState(SERVER_STARTUP);

	_PARAM.LoadParam("GeneralParam", "AllowParallelAVIUsage",(LPDWORD)&_Module.m_server.m_bAllowParallelAVIUsage,(DWORD)1,bCreate);
	_PARAM.LoadParam("GeneralParam", "PaidAllAviTrs",&dwTmp,(DWORD)0,bCreate);

	//If the field CheckVITConflict exists and is set to 1, copy this value
	//into the new field, CheckAVIConflict.  Otherwise (the value is zero or
	//the entry does not exist), mimic old behaviour by creating CheckAVIConflict
	//and setting to zero.
	if(!_PARAM.LoadParam("GeneralParam","CheckVITConflict",&dwTmp, 0, FALSE)  && dwTmp)
	{
		_PARAM.LoadParam("GeneralParam","CheckAVIConflict",&_Module.m_server.m_dwCheckVITConflict , dwTmp , bCreate);
		_PARAM.SaveParam("GeneralParam","CheckVITConflict",(DWORD)0,FALSE);
	}

	else
		_PARAM.LoadParam("GeneralParam","CheckAVIConflict",&_Module.m_server.m_dwCheckVITConflict , 1000 , bCreate);
	//4.0.0.55 (3.2.1.35) Eyal end

	//3.2.1.36
	//m_cParam.LoadParam("GeneralParam","MaxVolumeForVITConflict",&m_dwMaxVolumeForVITConflict, 1000, TRUE);

	if(!_PARAM.LoadParam("GeneralParam","MaxVolumeForVITConflict",&dwTmp, 0, FALSE)  && dwTmp)
	{
		_PARAM.LoadParam("GeneralParam","MaxVolumeForAVIConflict",&_Module.m_server.m_dwMaxVolumeForVITConflict , dwTmp , bCreate);
		_PARAM.SaveParam("GeneralParam","MaxVolumeForVITConflict",(DWORD)0,FALSE);
	}
	else
		_PARAM.LoadParam("GeneralParam","MaxVolumeForAVIConflict",&_Module.m_server.m_dwMaxVolumeForVITConflict , 1000 , bCreate);
	
	return;
}

void CRegTable::LoadMiscellaneousParameters(BOOL bCreate, BOOL bCompare)
{
	DWORD dwTmp = 0;
	TStaticString<STATIC_STRING_64> sTmpStr;

	if(bCompare)
		_Module.m_server.SetServerState(SERVER_STARTUP);

	_PARAM.LoadParam("GeneralParam", "ClearTrsOnIdlePump",&_Module.m_server.m_dwClearTrsOnIdlePump,(DWORD)0,bCreate);

	_PARAM.LoadParam("GeneralParam", "AutoTimeCheck",&dwTmp,(DWORD)0,bCreate);
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "OLAEpsilonSupport", &dwTmp, (DWORD) 0, bCreate);
	_Module.m_server.m_bOLAEpsilonSupport = dwTmp ? TRUE : FALSE;

	// QC 498288
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PinPadCapabilities", &dwTmp, (DWORD)1, bCreate);

	// QC 503092
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "MaxAlternateIDInputData", &dwTmp, (DWORD)10, bCreate);
	_Module.m_server.m_lMaxAlternateIDInputData = dwTmp;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "AddRollingUpdateDataToLog", &dwTmp, 0, TRUE);  // RFuel-745
	_Module.m_server.m_dwAddRollingUpdateDataToLog = dwTmp;

	//4.0.3.49
	_Module.m_server.SetAutoTimeChecker((long)dwTmp);
	//4.0.3.49

	_PARAM.LoadParam("GeneralParam","ControllerFileFullPath",sTmpStr,sTmpStr.Size()," ",bCreate);
	
	_PARAM.LoadParam("GeneralParam","AlternateServerAutoSwitch",&_Module.m_server.m_dwAutoSwitch, 0 ,bCreate);
	
	_PARAM.LoadParam("GeneralParam", "StationLevelEncryption",&_Module.m_server.m_dwStationLevelEncryption,(DWORD)1,bCreate);
    
	_PARAM.LoadParam("GeneralParam", "MaxMemCreditDisplay",&_Module.m_server.m_dwMaxMemCreditDisplay,(DWORD)3,bCreate);

	_PARAM.LoadParam("GeneralParam","AllowAuthOnNonRequest",&_Module.m_server.m_dwAllowAuthOnNonRequest, 0 ,bCreate);

	_PARAM.LoadParam("GeneralParam","NozzleLevelLogic",&_Module.m_server.m_dwNozzleLevelLogic, 0 ,bCreate);
	
	_PARAM.LoadParam("GeneralParam","CreditLimitByUser",&_Module.m_server.m_dwCreditLimitByUser, 0 ,bCreate);
	
	_PARAM.LoadParam("GeneralParam","DebitLimitByUser", &_Module.m_server.m_dwDebitLimitByUser, 0 ,bCreate);

	_PARAM.LoadParam("GeneralParam","SiteID",sTmpStr,sTmpStr.Size(),"",bCreate);
	_Module.m_server.m_sSiteID = sTmpStr;

	_PARAM.LoadParam("GeneralParam","EncryptionKey",sTmpStr,sTmpStr.Size(),"Non Valid Key",bCreate);

	_PARAM.LoadParam("GeneralParam", "DebitSingleSession",&_Module.m_server.m_dwDebitSingleSession,(DWORD)0,TRUE);

	_PARAM.LoadParam("GeneralParam", "SetWaitingPrePayToTimeOut",&_Module.m_server.m_dwSetWaitingPrePayToTimeOutFlags,(DWORD)0,TRUE);

	//4.0.3.54
	_PARAM.LoadParam("GeneralParam","LockPumpByMode",(DWORD*)&(_Module.m_server.m_dwLockPumpByMode),(DWORD)0,TRUE);
	
	//4.0.8.500
	_PARAM.LoadParam("GeneralParam","ShiftClearStuckTrs",&dwTmp,(DWORD)0,TRUE);
	_Module.m_server.m_cShiftProcess.SetFlagClearStuckTrs((dwTmp)?TRUE:FALSE);
	
	 //4.0.9.501
	_PARAM.LoadParam("GeneralParam","NoParallelPAK_PrP",&dwTmp, 0, TRUE);
	_Module.m_server.SetNoParallelPakPrePayFlag(dwTmp);
	_PARAM.LoadParam("GeneralParam", "ModeChangeNoOpenPrp",&dwTmp,(DWORD)0,TRUE);//4.0.9.506 //amram
	_Module.m_server.m_lModeChangeNoOpenPrp =  dwTmp;  
	_PARAM.LoadParam("GeneralParam", "ModeChangeOnIdle",&dwTmp,(DWORD)0,TRUE);//4.0.9.506 //amram
	_Module.m_server.m_lModeChangeOnIdle    =  dwTmp;
	//4.0.10.502
	_PARAM.LoadParam("GeneralParam", "OLAReportSoldItemOnPump",&dwTmp,(DWORD)0,TRUE);//4.0.9.506 //amram
	_Module.m_server.SetOLAReportSoldItemOnPump(dwTmp);//4.0.10.502

	//49339
	_PARAM.LoadParam("GeneralParam", "SupportNACSRestrictions",&dwTmp,(DWORD)0,TRUE);//49339
	_Module.m_server.SetSupportNACSRestriction(dwTmp);	//49339

	//Roni Start(version 4.0.9.800)
	_PARAM.LoadParam("GeneralParam", "SysOnManualMode",(DWORD*)&_Module.m_server.m_dwSysOnManualModes ,(DWORD)0,bCreate);  //4.0.6.505	
	//Roni End
	/*@@@*/

	//4.0.12.515
	_PARAM.LoadParam("GeneralParam", "RollbackOnPriceChangeFailure",&_Module.m_server.m_dwRollbackOnPriceChangeFailure,(DWORD)0,TRUE);

	//4.0.14.501
	_PARAM.LoadParam("GeneralParam", "LoyaltyTransactionCount",&_Module.m_server.m_dwLoyaltyTransactionCount ,(DWORD)4,TRUE);

	//4.0.15.120 - CR 44000
	_PARAM.LoadParam("GeneralParam", "SendZeroValuesWhenOffline",&_Module.m_server.m_dwSendZeroValuesWhenOffline, 0 ,TRUE);

	//4.0.18.20 - CR 33898	//4.0.18.501
	_PARAM.LoadParam("GeneralParam", "ReloadQDXEveryXShifts",&_Module.m_server.m_dwReloadQDXEveryXShifts,(DWORD)0,TRUE);

	//4.0.23.507
	dwTmp = 0;
	_PARAM.LoadParam("GeneralParam", "ShiftsToMaintenance", &dwTmp, (DWORD)_Module.m_server.m_dwReloadQDXEveryXShifts, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE); 

	_PARAM.LoadParam("GeneralParam","ActiveReOrganizeReceiptQdx",&dwTmp, 1 ,TRUE);  //4.0.18.501 merge from 4.0.15.291
	if (dwTmp)
		_Module.m_server.m_bActiveReOrganizeReceiptQdx = TRUE;
	else
		_Module.m_server.m_bActiveReOrganizeReceiptQdx = FALSE;

	_PARAM.LoadParam("GeneralParam", "SupportDeliveryEmulation", &dwTmp, 0, TRUE);  //4.0.20.52 (62818)
	_Module.m_server.m_bSupportDeliveryEmulation = (dwTmp==1) ? TRUE : FALSE;

	_PARAM.LoadParam("GeneralParam", "AllowMultiPosOwnersOfPumpTransactions",&_Module.m_server.m_dwAllowMultiPosOwnersOfPumpTrs,(DWORD)0,TRUE);	//4.0.23.501 78146

	//4.0.24.60 - TD 107311
	_PARAM.LoadParam("GeneralParam", "IgnoreNonValidGradesOnPriceChange", &dwTmp, 0, TRUE);	
	_Module.m_server.m_bIgnoreNonValidGradesOnPriceChange = (dwTmp==1) ? TRUE : FALSE;

	_PARAM.LoadParam("GeneralParam", "FuelTransactionNumberMinValue",&dwTmp ,(DWORD)1,bCreate);	////4.0.23.260 111318	4.0.23.261
	_Module.m_server.SetFuelTransactionNumberMinValue(dwTmp);

	_PARAM.LoadParam("GeneralParam", "FuelTransactionNumberMaxValue",&dwTmp ,(DWORD)99999,bCreate);	////4.0.23.260 111318	4.0.23.261
	_Module.m_server.SetFuelTransactionNumberMaxValue(dwTmp);

	_PARAM.LoadParam("GeneralParam", "AddEncodingStringToVPXml", &dwTmp, 0, TRUE);					//4.0.24.90 TD 106227
	_Module.m_server.SetAddEncodingStringToVPXml( dwTmp );

	//4.0.25.50 - TD 108311
	_PARAM.LoadParam("GeneralParam", "SingleShiftAccumulationsActive", &dwTmp, 0, TRUE);
	_Module.m_server.SetSingleShiftAccumulationsActive( dwTmp );

	_PARAM.LoadParam("GeneralParam", "RollingAsRemaining" , (LPDWORD)& _Module.m_server.m_bRollingAsRemaining,0,TRUE); // 4.0.25.600   CR 296433

	// TD 339257
	_PARAM.LoadParam("GeneralParam","PayPalActive",(unsigned long *)&_Module.m_server.m_bPayPalActive,(DWORD)0,TRUE);

	//4.0.26.501 TD 144220
	SZ_TAG_SYSTEMTIME	szTagTime;

	memset(&szTagTime, 0, sizeof(szTagTime));
	szTagTime.sNull = 0;

	_PARAM.LoadParam("GeneralParam", "ReloadQDXEveryXDays", &dwTmp, (DWORD)0, TRUE);
	_PARAM.LoadParam("GeneralParam", "ReloadQDXLastTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME), "", TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	_Module.m_server.SetReloadQdxLastTime(&szTagTime);

	//4.0.27.41 TD 165585
	_PARAM.LoadParam("GeneralParam", "FlowRateActive", &dwTmp, 0, TRUE);
	_Module.m_server.SetFlowRateActive( dwTmp );		
	
	_PARAM.LoadParam("GeneralParam", "IndicateAuthorizePumpSent", &_Module.m_server.m_dwIndicateAuthorizePumpSent, (DWORD)0, TRUE);  //4.0.30.500 - TD 320068

	_PARAM.LoadParam("GeneralParam" , "TwoTendersInTransaction" , &dwTmp, (DWORD)0 , TRUE);//4.0.10.501 TD 14801
	_Module.m_server.m_bTwoTendersInTransaction = (BOOL)dwTmp;

	_PARAM.LoadParam("GeneralParam", "MaxShiftNumber", &_Module.m_server.m_dwMaxShiftNumber, (DWORD)99999, TRUE);	//TD 417989

	// CR 407579
	_PARAM.LoadParam("GeneralParam", "AutomaticAuthForLoyaltyCustomer",	&dwTmp, (DWORD)0, TRUE);
	_Module.m_server.m_bAutomaticAuthForLoyaltyCustomer = dwTmp;
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "ReduceCWDiscountFromCWAmount",		&dwTmp, (DWORD)0, TRUE);
	_Module.m_server.m_bReduceCWDiscountFromCWAmount = dwTmp;

	return;
}

void CRegTable::LoadActivesTable(BOOL bCreate , BOOL bCompare)
{
	DWORD dwTmp = 0;

	if(bCompare)
		_Module.m_server.SetServerState(SERVER_STARTUP);
	
	//The following parameters are simply queried during runtime
	_Module.m_server.m_cParam.LoadParam("GeneralParam","HotPumping",&_Module.m_server.m_dwHotPumping, 0, bCreate);
	_Module.m_server.m_cParam.LoadParam("GeneralParam" , "PrePayEnabled" , &_Module.m_server.m_dwPrePayEnabled , 1 , bCreate);
	
	ReloadHydra(bCreate , bCompare);
	ReloadAlarmSrv(bCreate , bCompare);
	ReloadPostVoid(bCreate , bCompare);
	ReloadCashAcceptorActive(bCreate , bCompare);//4.0.12.502
	ReloadVP(bCreate , bCompare);
	ReloadDataExportSrv(bCreate , bCompare); //4.0.5.39
	ReloadIFSFTerminalSrv(bCreate , bCompare); //4.0.12.503
	ReloadFuelMobileSrv(bCreate, bCompare);// CR 444830
	
	//relevant only for convert layers
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "Buy@PumpActive",&dwTmp,(DWORD)0,bCreate);
	
	//4.0.2.38  moved here from LoadPumpSrvInternalParams()
	_Module.m_server.m_cParam.LoadParam("GeneralParam","ActiveLoyaltyDiscount",(DWORD*)&(_Module.m_server.m_bActiveLoyaltyDiscount),(DWORD)0,bCreate);

	//4.0.5.26
	//  active accumulation shift  
	_Module.m_server.m_cParam.LoadParam("GeneralParam","AccumulationShiftActive",&dwTmp,0 ,bCreate);

	_Module.m_server.m_cParam.LoadParam("GeneralParam","CashAcceptorReportActive",&_Module.m_server.m_dwCashAcceptorReportActive ,0 ,bCreate);

	_Module.m_server.m_cAccumulationShiftTotal.SetProcessActive((BOOL)dwTmp);

	//4.0.13.500
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "MinAlertSeverity_SoundAlert",&_Module.m_server.m_dwMinAlertSeverity_SoundAlert,(DWORD)0,bCreate);  //4.0.10.504
	
	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString sLogMsg;
		sLogMsg.Format("Minimal Alert Severity sound is %ld",_Module.m_server.m_dwMinAlertSeverity_SoundAlert);
		_LOGMSG.LogMsg(sLogMsg);
	}
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "UseServiceFee",(DWORD*)&_Module.m_server.m_bUseServiceFee,(DWORD)0,bCreate);  //4.0.6.505

	_Module.m_server.m_cParam.LoadParam("GeneralParam","FuelLoyaltyFlag",&dwTmp,0 ,bCreate);//4.0.9.508
	_Module.m_server.m_cFuelLoayaltyPS.SetFuelLoyaltyFlag((BOOL)dwTmp);//4.0.9.508
	
	
	
	//amram 4.0.9.506
	//get the param that handle the tank level 

//	_Module.m_server.m_cParam.LoadParam("GeneralParam", "TankMinVolumeLimit",&dwTmp,(DWORD)0,bCreate);  //4.0.29.500
//	_Module.m_server.m_cTankLevelMap.SetTankMinVolumeLimit(dwTmp);
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "ProtectTrsLimit",&dwTmp,(DWORD)0,bCreate);  //4.0.9.506
	_Module.m_server.m_cTankLevelMap.SetProtectTrsLimit(dwTmp);

	//4.0.13.505
	_Module.m_server.m_cParam.LoadParam("GeneralParam","InvoiceRefNumber",&dwTmp,(DWORD)0,bCreate);
	_Module.m_server.SetInvoiceRefNumber((long)dwTmp);

	//4.0.13.512
	_Module.m_server.m_cParam.LoadParam("GeneralParam","SimTb",&dwTmp,(DWORD)0,bCreate);

	//4.0.15.504 - TD 38453
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PrintZeroTrsReceipts",&_Module.m_server.m_dwPrintZeroTrsReceipts,(DWORD)0,bCreate);

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "FinishedPrepayIndication",&dwTmp ,(DWORD)0,bCreate);	////56040 4.0.19.508	
	_Module.m_server.SetFinishedPrepayIndication(dwTmp);	////56040 4.0.19.508	

	//4.0.20.501 - TD 60203	
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "ReceiptLookupMethod", &_Module.m_server.m_dwReceiptLookupMethod, (DWORD)0, bCreate);		

	//4.0.24.60 TD 105272
	//4.0.20.1280 - CR 360293 - Electonic Wallet+ Manual Loyalty  	
	_Module.m_server.m_cParam.LoadParam("GeneralParam","ManualLoyaltyOnIdle",&_Module.m_server.m_dwManualLoyalty,(DWORD)0 ,bCreate);
	_Module.m_server.m_cParam.LoadParam("GeneralParam","ManualPayment",		 &dwTmp,(DWORD)0 ,bCreate);
	_Module.m_server.m_bManualPayment = (BOOL)dwTmp;

		// 4.0.23.1050  TD 233242
	_Module.m_server.m_cParam.LoadParam("GeneralParam","EnableAutoReconcileProcess", (DWORD*) &_Module.m_server.m_bAutoReconcileProcess, 0 ,bCreate);

	//4.0.26.508 TD 146575
	_Module.m_server.m_cSiteConfigurationExporter.LoadConfiguration();			

	return;
}

void CRegTable::LoadLogTable(BOOL bCreate , BOOL bCompare)
{
	DWORD dwTmp = 0;
	TStaticString<STATIC_STRING_64> sTmpStr;
	//Since all log parameters are handled in CLogger,
	//this method becomes relevant only if we are performing
	//a runtime correction
	if(bCompare)
	{
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "LogPercent",&dwTmp,(DWORD)0,bCreate);
		_LOGMSG.ResetLogPercentage(dwTmp);
		
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "WarningPercent",&dwTmp,(DWORD)0,bCreate);
		_LOGMSG.ResetWarningLevel(dwTmp);
		
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "LogLevel",&dwTmp,(DWORD)0,bCreate);
		_LOGMSG.SetLogLevel(dwTmp);

		_Module.m_server.m_cParam.LoadParam("Paths\\Long", "LogDir",sTmpStr,sTmpStr.Size(),"",bCreate);
		_LOGMSG.ResetLogPath((char*)sTmpStr);
	}

	//4.0.3.39
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "TrsLogActive",&dwTmp,(DWORD)0,bCreate);
	_Module.m_server.m_cTrsLog.SetActiveParam(dwTmp);
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "GetPaidPAK",&_Module.m_server.m_cSQLPaidTrsFile.m_lGetPaidPAK,(DWORD)0,bCreate);//4.0.11.503
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "GetPaidPAP",&_Module.m_server.m_cSQLPaidTrsFile.m_lGetPaidPAP,(DWORD)0,bCreate);//4.0.11.503
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "GetPaidAVI",&_Module.m_server.m_cSQLPaidTrsFile.m_lGetPaidAVI,(DWORD)0,bCreate);//4.0.11.503
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "GetPaidAtt",&_Module.m_server.m_cSQLPaidTrsFile.m_lGetPaidAtt,(DWORD)0,bCreate);//4.0.11.503
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "GetPaidPRP",&_Module.m_server.m_cSQLPaidTrsFile.m_lGetPaidPRP,(DWORD)0,bCreate);//4.0.11.503
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "MaxNumOfPaidTrs",&_Module.m_server.m_cSQLPaidTrsFile.m_lMaxNumOfPaidTrs,(DWORD)12,bCreate);//4.0.11.503
		
	ULONG lTmpValue = 0;
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "ExceptionHandlerLogLevel",&lTmpValue,(DWORD)0,bCreate);//4.0.23.1800

	return;
}

//////////////////////////////////////////////////////////////////////
//Sub table loading methods
//////////////////////////////////////////////////////////////////////

BOOL CRegTable::LoadGeneralParamSubTable(long lSubTable , BOOL bCreate , BOOL bCompare)
{
	BOOL bRetCode = TRUE;

	if(lSubTable & GENERAL_PARAM_TABLE_OLA)
		bRetCode = LoadOLATable(bCreate , bCompare);
		
	if(lSubTable & GENERAL_PARAM_TABLE_FORECOURT_LIMITS)
		LoadForecourtLimitSettings(bCreate , bCompare);
	
	if(lSubTable & GENERAL_PARAM_TABLE_ACTIVES)
		LoadActivesTable(bCreate , bCompare);
		
	if(lSubTable & GENERAL_PARAM_TABLE_WETSTOCK)
		LoadWetStockConfiguration(bCreate , bCompare);
		
	if(lSubTable & GENERAL_PARAM_TABLE_LOGS)
		LoadLogTable(bCreate , bCompare);
		
	if(lSubTable & GENERAL_PARAM_TABLE_TIMERS)
		LoadTimers(bCreate , bCompare);
		
	if(lSubTable & GENERAL_PARAM_TABLE_AVI_TRS)
		LoadAVIParameters(bCreate , bCompare);
		
	if(lSubTable & GENERAL_PARAM_TABLE_GENERALS)
		LoadMiscellaneousParameters(bCreate , bCompare);

	if(lSubTable & GENERAL_PARAM_TABLE_LOCK_CONFIGURATION)  //4.0.10.811
		LoadForecourtLockConfiguration();

	if(lSubTable & GENERAL_PARAM_TABLE_ENCODING)			//4.0.22.501	TD 72716
		LoadEncodingTable(bCreate);	
	
	if(lSubTable & GENERAL_PARAM_TABLE_REAL_TIME)			//4.0.22.501	TD 72716
		LoadRealTimeTable(bCreate);

	return bRetCode;
}

//Note that the AVI parameters can be loaded from several options.
//This is done to ensure backwards compatibility with old versions of FCC.
void CRegTable::LoadRFSSubTable(long lSubTable , BOOL bCreate , BOOL bCompare)
{
	CString sLogMsg("LoadRFSSubTable - ") , sTable;

	if(lSubTable & RFS_TABLE_GENERAL)
	{
		LoadAVIParameters(bCreate , bCompare);
		sLogMsg += "RFS_TABLE_GENERAL";
	}

	if(lSubTable & RFS_TABLE_AVI)
	{
		LoadAVIParameters(bCreate , bCompare);
		sLogMsg += "RFS_TABLE_AVI";
	}
		
	if(lSubTable & RFS_TABLE_KEYBOARDS)
		sLogMsg += "RFS_TABLE_KEYBOARDS";
		
	if(lSubTable & RFS_TABLE_LANGUAGES)
		sLogMsg += "RFS_TABLE_LANGUAGES";
	
	_LOGMSG.LogMsg(sLogMsg);
}


//////////////////////////////////////////////////////////////////////
//Protected methods
//////////////////////////////////////////////////////////////////////

void CRegTable::ReloadVP(BOOL bCreate , BOOL bCompare)
{
	DWORD dwTmp = 0;
	DWORD dwVal = 0;
	

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "VpActive",&dwTmp,(DWORD)0,bCreate);
	// 4.0.29.500 TD 264908
 	_Module.m_server.m_cParam.LoadParam("GeneralParam","VPRecoveryRetries",&dwVal,(DWORD)0,TRUE); 
	_Module.m_server.m_lVpRecoveryMaxRetries = dwVal;
	if(bCompare)
	{
		//VPActive turned on
		if(dwTmp && !_Module.m_server.m_dwVpActive)
			_Module.m_server.CreateVPThread();
			
		//VPActive turned off
		else if(!dwTmp && _Module.m_server.m_dwVpActive)
		{
			_Module.m_server.DisconnectFromVp();
			_Module.m_server.DeleteVPThreadInfo();
		}
	}

	_Module.m_server.m_dwVpActive = dwTmp;
}

void CRegTable::ReloadHydra(BOOL bCreate, BOOL bCompare)
{
	DWORD dwTmp = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "HydraActive", &dwTmp, (DWORD)0, bCreate);

	_Module.m_server.SetRedundancyByCL(dwTmp);					//4.0.23.508

	if ( _Module.m_server.IsRedundancyByCL() == TRUE )			//4.0.23.508 TD 89049
	{
		dwTmp = HYD_NOT_ACTIVE;

		if(LOG_BASIC_FLOW_CONTROL)		
			_LOGMSG.LogMsg("ReloadHydra, PumpSrv is running without HydService with REDUNDANCY by CL");
	}

	if(bCompare)
	{
		if(dwTmp && !_Module.m_server.m_dwHydraActive)
		{
			_Module.m_server.ConnectToHydService();
		}

		else if(!dwTmp && _Module.m_server.m_dwHydraActive)
		{
			_Module.m_server.m_cHydra.CloseConnectionToHydra();
		}
	}

	_Module.m_server.m_dwHydraActive = dwTmp;
}

void CRegTable::ReloadAlarmSrv(BOOL bCreate , BOOL bCompare)
{
	DWORD dwTmp = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "AlarmSrvActive",&dwTmp,(DWORD)0,bCreate);
	if(bCompare)
	{
		if(dwTmp && !_Module.m_server.m_dwAlarmSrvActive)
		{
			_Module.m_server.ConnectToAlarmSrv();
		}

		else if(!dwTmp && _Module.m_server.m_dwAlarmSrvActive)
		{
			_Module.m_server.CloseConnectionToAlarmSrv();
		}
	}

	_Module.m_server.m_dwAlarmSrvActive = dwTmp;

	return;
}

void CRegTable::ReloadPostVoid(BOOL bCreate , BOOL bCompare)
{
	DWORD dwTmp = 0 , dwPrevValue = (DWORD)_Module.m_server.GetPosVoidFlags();

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PostVoidActive",&dwTmp,(DWORD)0,bCreate);

	_Module.m_server.SetPosVoidFlags((dwTmp?1:0));

	if(bCompare)
	{
		if((dwPrevValue?1:0) != (_Module.m_server.GetPosVoidFlags()?1:0))
			ReloadDefinitionForAllPumps();
	}

	return;
}

//4.0.12.502
void CRegTable::ReloadCashAcceptorActive(BOOL bCreate , BOOL bCompare)
{
	DWORD dwTmp = 0 , dwPrevValue = (DWORD)_Module.m_server.GetCashAcceptorActive();

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "CashAcceptorValid",&dwTmp,(DWORD)0,bCreate);

	_Module.m_server.SetCashAcceptorValid((dwTmp?1:0));

	return;
}

//4.0.12.503
void CRegTable::ReloadIFSFTerminalSrv(BOOL bCreate , BOOL bCompare)
{
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "IFSFTerminalSrvInUse",&_Module.m_server.m_dwIFSFInUse,(DWORD)0,bCreate);
	
	if(bCompare)
	{
		//If we want to load IFSFTerminalSrv and it is not already loaded. 
		if(_Module.m_server.m_dwIFSFInUse && _Module.m_server.m_pIFSFTerminalSrvCtrl == NULL) 
		{
			_Module.m_server.ConnectToIFSFTerminalSrv();
		}
		else
		{
			_Module.m_server.CloseConnectToIFSFTerminalSrv();
		}
	}

	return;	
}


//////////////////////////////////////////////////////////////////////
//Private methods
//////////////////////////////////////////////////////////////////////


// CR 444830
void CRegTable::ReloadFuelMobileSrv(BOOL bCreate, BOOL bCompare)
{
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "FuelMobileSrvInUse", &_Module.m_server.m_dwFuelMobileSrvInUse, (DWORD)0, bCreate);

	if (bCompare)
	{
		//If we want to load FuelMobileSrv and it is not already loaded. 
		if (_Module.m_server.m_dwFuelMobileSrvInUse && _Module.m_server.m_pFuelMobileSrvCtrl == NULL)
		{
			_Module.m_server.ConnectToFuelMobileSrv();
		}
		else
		{
			_Module.m_server.CloseConnectToFuelMobileSrv();
		}
	}

	return;
}


void CRegTable::ReloadDefinitionForAllPumps()
{
	for(int i = 0;i < _Module.m_server.GetLastValidPump();i++)
		_Module.m_server.m_cPumpArray[i].LoadDefinition();
}



void CRegTable::LoadShiftsTable(BOOL bCreate , BOOL bCompare , BOOL bRun)
{
	CString str;
	DWORD dwTmp = 0;
	SYSTEMTIME curSysTime;
	SZ_TAG_SYSTEMTIME szTagTime;
	
	memset(&curSysTime , 0 , sizeof(curSysTime));
	GetLocalTime(&curSysTime);


	memset(&szTagTime , 0 , sizeof(szTagTime));
	szTagTime.sNull = 0;
	SystemTime2Tag(&curSysTime,&szTagTime.sTime);

	_Module.m_server.m_cParam.LoadParam("GeneralParam","ShiftMode",&_Module.m_server.m_dwShiftMode, SHIFT_MODE_SINGLE,bCreate);

	if(bCreate)
	{
		for(int i = 1; i<9; i++)
		{
			for(int s=1; s<7; s++)
			{
				str.Format("Shifts\\Day%d\\Shift%d" , i , s);
				
				if(s==1)
					_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwTmp,(DWORD)1,bCreate);
				
				else
					_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwTmp,(DWORD)0,bCreate);
				
				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "StartTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME) ,(char *)&szTagTime,bCreate ,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ReadPumpTotals",&dwTmp,(DWORD)0,bCreate);
				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ReadTanks",&dwTmp,(DWORD)0,bCreate);

				// 2.9.2.1 start
				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "LockForeCourt",&dwTmp,(DWORD)0,bCreate);

				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ForceShiftOLA",&dwTmp,(DWORD)0,bCreate);
				
				// 2.9.2.1 end 	

				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ShiftLock",&dwTmp,(DWORD)0,bCreate); //4.0.5.39 
				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ShiftEOD",&dwTmp,(DWORD)0,bCreate); // CR 475695
				
				//4.0.3.60
				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ReconcileTankSrv", &dwTmp , (DWORD)0 , bCreate);
				
				//4.0.10.505
				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ShiftLockDuringProcess", &dwTmp , (DWORD)0 , bCreate);

				//4.0.5550.1504
				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "DataBaseMaintenance", &dwTmp , (DWORD)0 , bCreate);

				//4.0.26.508 TD 146575
				_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ExportSiteConfig", &dwTmp , (DWORD)0 , bCreate);

				if (i==8)
					break;
			}

			//4.0.1.0
			if (bRun == FALSE)
				Sleep(REGISTRATION_SLEEP_TIME);
		}
	}

	return;
}

void CRegTable::LoadReceiptTable(BOOL bCreate , BOOL bCompare , BOOL bRun)
{
	//None of these parameters are relevant during run-time, so we exit
	//the method if in the middle of run time update (indicated by bCreate == FALSE
	//and bCompare == TRUE)
	if(bCreate)
	{
		DWORD dwVal = 0;

		char sTmpStr[MAX_FIELD_VAL];
		memset(sTmpStr , 0 , sizeof(sTmpStr));

		_Module.m_server.m_cParam.LoadParam("Receipt", "HeaderCount",&dwVal,1,TRUE);	
		_Module.m_server.m_cParam.LoadParam("Receipt", "ReceiptRecordTimeout",&dwVal,0,TRUE);    //Default - 0 (if the parameter set to zero, then all the process should be shut down).
		_Module.m_server.m_cParam.LoadParam("Receipt", "MaxReceiptCopies",&dwVal,0,TRUE);        //If it set to 0 then there is no limit

		//4.0.8.501 
		// set activity receipt extra index feature.
		_Module.m_server.m_cParam.LoadParam("Receipt", "ActiveReceiptExtraIndex",(DWORD*)&dwVal,(DWORD)0,bCreate);  //4.0.6.505	
		_Module.m_server.m_cReceiptExtraIndexFile.SetReceiptExtraIndexFlags(dwVal);


		CString sEntryName;

		for(int h = 1; h < 10 ; h++)
		{
			sEntryName.Format("Header%1d" , h);
			
			if(h == 1) 
				_Module.m_server.m_cParam.LoadParam("Receipt", (LPCSTR)sEntryName, (char *)sTmpStr, sizeof(sTmpStr) ,(char *)"Welcome !!!",TRUE);
			
			else
			{
				ZeroMemory((char *)sTmpStr, sizeof(sTmpStr));
				_Module.m_server.m_cParam.LoadParam("Receipt",(LPCSTR)sEntryName, (char *)sTmpStr, sizeof(sTmpStr) ,(char *)"",TRUE);
			}
		}

		_Module.m_server.m_cParam.LoadParam("Receipt", "TrailerCount",&dwVal,1,TRUE);
		
		for(int t = 1; t < 10 ; t++)
		{
			sEntryName.Format("Trailer%1d" , t);
			
			if(t == 1)
				_Module.m_server.m_cParam.LoadParam("Receipt",(LPCSTR)sEntryName, (char *)sTmpStr, sizeof(sTmpStr) ,(char *)"Thank You !!!",TRUE);
			
			else
			{
				ZeroMemory((char *)sTmpStr, sizeof(sTmpStr));
				_Module.m_server.m_cParam.LoadParam("Receipt",(LPCSTR)sEntryName, (char *)sTmpStr, sizeof(sTmpStr) ,(char *)"",TRUE);
			}
		}
		
		_Module.m_server.m_cParam.LoadParam("Receipt", "HotPumpingCount",&dwVal,1,TRUE);
		
		//4.0.0.47
		_Module.m_server.m_cParam.LoadParam("Receipt", "MaxReceiptLineLen",&dwVal,20,TRUE);	

		_Module.m_server.m_cParam.LoadParam("Receipt", "MinReceiptLines",&dwVal,32,TRUE); //4.0.6.506
		
		for(int t = 1; t < 4 ; t++)
		{
			sEntryName.Format("HotPumping%1d" , t);
			
			if(t == 1)
				_Module.m_server.m_cParam.LoadParam("Receipt",(LPCSTR)sEntryName, (char *)sTmpStr, sizeof(sTmpStr) ,(char *)"Please pay cashier",TRUE);
			
			else
			{
				ZeroMemory((char *)sTmpStr, sizeof(sTmpStr));
				_Module.m_server.m_cParam.LoadParam("Receipt",(LPCSTR)sEntryName, (char *)sTmpStr, sizeof(sTmpStr) ,(char *)"",TRUE);
			}
		}

		_Module.m_server.m_cParam.LoadParam("Receipt", "ExtraDataCount", &dwVal, 0, TRUE);					//4.0.23.500 TD 75951
		
		_Module.m_server.m_cParam.LoadParam("Receipt", "PrintOnlyPrepayBalancedReceipt", &dwVal, 0, TRUE);	//4.0.23.501 - TD 75947

		_Module.m_server.m_cParam.LoadParam("Receipt", "PrintReceiptOnCardDenial", &dwVal, 0, TRUE);

		_Module.m_server.SetPrintReceiptOnDecline(dwVal);

		if (bRun == FALSE)								//4.0.1.0
			Sleep(REGISTRATION_SLEEP_TIME);

	}

	return;
}

void CRegTable::LoadMessagesTable(BOOL bCreate , BOOL bCompare  , BOOL bRun)
{
	//None of these parameters are relevant during run-time, so we exit
	//the method if in the middle of run time update (indicated by bCreate == FALSE
	//and bCompare == TRUE)
	if(bCreate)
	{
		DWORD dwVal = 0;
		char sTmpStr[MAX_FIELD_VAL];
		
		memset(sTmpStr , 0 , sizeof(sTmpStr));

		_Module.m_server.m_cParam.LoadParam("Messages", "ScreenBasedMessagest",&dwVal,0,TRUE);

		memset(sTmpStr , 0 , sizeof(sTmpStr));
		_Module.m_server.m_cParam.LoadParam("Messages", "Idle"							, sTmpStr, sizeof(sTmpStr) ,"     Insert Card             or            Lift Handle to         Pay Inside    ",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "Promotion"						, sTmpStr, sizeof(sTmpStr) ,"Welcome to our smart fueling point",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "IdleScreen"						, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "PromotionScreen"				, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "Odometer"						, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "OdometerScreen"					, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "VehicleId"						, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "VehicleIdScreen"				, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "PIN"							, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "PINScreen"						, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "ReturnTank"						, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "ReturnTankScreen"				, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "DriverId"						, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "DriverIdScreen"					, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "GasRewardPrompt"				, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "GasRewardPromptScreen"			, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "GasRewardVoucherNumber"			, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "GasRewardVoucherNumberScreen"	, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "GeneralId"						, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "GeneralIdScreen"				, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "GasRewardKeyCode"				, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "GasRewardKeyCodeScreen"			, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "Track2"							, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "Track2Screen"					, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "LoyaltyExpired"					, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "LoyaltyExpiredScreen"			, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "LoyaltyInvalid"					, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "LoyaltyInvalidScreen"			, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "LoyaltyNotMatchPayment"			, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "LoyaltyNotMatchPaymentScreen"	, sTmpStr, sizeof(sTmpStr) ,"",TRUE);

		_Module.m_server.m_cParam.LoadParam("Messages", "Unit"							, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "UnitScreen"						, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "TripNumber"						, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "TripNumberScreen"				, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "CustomerReference"				, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "CustomerReferenceScreen"		, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "TractorHub"						, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "TractorHubScreen"				, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "TrailerNumber"					, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "TrailerNumberScreen"			, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "TrailerHub"						, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "TrailerHubScreen"				, sTmpStr, sizeof(sTmpStr) ,"",TRUE);

		// start 3.1.1.61
		_Module.m_server.m_cParam.LoadParam("Messages", "LoyaltySwipePrompt"						, sTmpStr, sizeof(sTmpStr) ,"Swipe Pref Card or YES = Key-in#",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "LoyaltyManualPrompt"				, sTmpStr, sizeof(sTmpStr) ,"Key # and ENTER",TRUE);	
		// end 3.1.1.61

		//3.2.0.31
		_Module.m_server.m_cParam.LoadParam("Messages", "IdleIPT"				, sTmpStr, sizeof(sTmpStr) ," Welcome to Husky  please swipe your card",TRUE);	
		//3.2.0.31

		//4.0.0.47
		_Module.m_server.m_cParam.LoadParam("Messages", "MaxMessageLineLen",&dwVal,20,TRUE);	
		//4.0.0.47	
		
		//4.0.1.1
		_Module.m_server.m_cParam.LoadParam("Messages", "ChargeTo"					, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam("Messages", "ChargeToScreen"				, sTmpStr, sizeof(sTmpStr) ,"",TRUE);
		//4.0.1.1

		//4.0.22.500 70766
		_Module.m_server.m_cParam.LoadParam("Messages", "MaxCouponsEntered",(char *)sTmpStr, sizeof(sTmpStr), (char *)SCREEN_MAX_COUPONS_STR,TRUE);
	}

	return;
}

void CRegTable::LoadScreenLayoutsTable(BOOL bCreate, BOOL bCompare , BOOL bRun)
{
	//None of these parameters are relevant during run-time, so we exit
	//the method if in the middle of run time update (indicated by bCreate == FALSE
	//and bCompare == TRUE)
	if(bCreate)
	{
		DWORD dwTmp = 0;
	
		char sTmpStr[MAX_FIELD_VAL];
		memset(sTmpStr , 0 , sizeof(sTmpStr));

		CString str , sTable("ScreenLayouts\\ScreenLayout");
		char sSoftKey[] = "SoftKeyXX";

		char sNumber[8];

		for(int i = 1; i <= MAX_SCREEN_LAYOUT; i++)
		{
			memset(sNumber , 0 , sizeof(sNumber));
			sprintf((char*)sNumber ,"%02d" , i);
			
			str = sTable + sNumber;

			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ValidEntry", &dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ScreenName", sTmpStr, sizeof(sTmpStr), sNumber, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "DefaultKeys", sTmpStr, sizeof(sTmpStr), "GHIJ", TRUE);

			for (int iKey = 1 ; iKey <= 10 ; iKey++)
			{
				sprintf(sSoftKey, "SoftKey%02d", iKey);
				ZeroMemory((char *)sTmpStr, sizeof(sTmpStr));
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), sSoftKey, (char *)sTmpStr, sizeof(sTmpStr) ,"",TRUE);
				//2.9.3.5
				sprintf(sSoftKey, "Numeric%d", iKey - 1);
				ZeroMemory((char *)sTmpStr, sizeof(sTmpStr));
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), sSoftKey, (char *)sTmpStr, sizeof(sTmpStr) ,"",TRUE);
			}

			//4.0.1.0
			if (bRun == FALSE)
				Sleep(REGISTRATION_SLEEP_TIME);
		}
	}

	return;
}

void CRegTable::LoadModesTable(BOOL bCreate, BOOL bCompare , BOOL bRun)
{
	DWORD dwTmp = 0;

	TStaticString<STATIC_STRING_8> strModeName;
	TStaticString<STATIC_STRING_64> sTmpStr;
	
	CString str;

	for(int i = 1; i <= MAX_MODES; i++)
	{
		str.Format("Modes\\Mode%02d" , i);
		sprintf((char*)strModeName , "Mode%02d" , i);		
		
		if(i==1)	
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwTmp,(DWORD)1,TRUE);
		
		else
			_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ValidEntry",&dwTmp,(DWORD)0,TRUE);
		
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "Name",(char *)sTmpStr, sTmpStr.Size(), (char *)strModeName,TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PAKValid",&dwTmp,(DWORD)1,TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PAKAuto",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PAKAutoReconcile",&dwTmp,(DWORD)0,TRUE);		//4.0.7.504		
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PAKAutoLoyaltyOnly",&dwTmp,(DWORD)0,TRUE);	//4.0.23.508 89149
		
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PAPValid",&dwTmp,(DWORD)1,TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PAPAuto",&dwTmp,(DWORD)1,TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PrePayValid",&dwTmp,(DWORD)1,TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PrePayAuto",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PayAttendValid",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PayAttendAuto",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PayAVIValid",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PayAVIAuto",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PAMValid", &dwTmp, (DWORD)0, TRUE);// CR 444830
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PAMAuto", &dwTmp, (DWORD)0, TRUE);// CR 444830
		_Module.m_server.m_cParam.LoadParam((LPCTSTR)str, "CashAcceptorValid",&dwTmp,(DWORD)0,TRUE); //4.0.3.51
		_Module.m_server.m_cParam.LoadParam((LPCTSTR)str, "CashAcceptorAuto",&dwTmp,(DWORD)0,TRUE); //4.0.3.51
		_Module.m_server.m_cParam.LoadParam((LPCTSTR)str, "IdlePromptNum",&dwTmp,(DWORD)1,TRUE); //4.0.5.41
		_Module.m_server.m_cParam.LoadParam((LPCTSTR)str, "CashLimit",&dwTmp,(DWORD)0,TRUE); //4.0.6.506
		_Module.m_server.m_cParam.LoadParam((LPCTSTR)str, "CreditLimit",&dwTmp,(DWORD)0,TRUE); //4.0.6.506
		_Module.m_server.m_cParam.LoadParam((LPCTSTR)str, "PAPPostPayValid",&dwTmp,(DWORD)0,TRUE); //4.0.29.500 - TD 264032

			
		/*@@@*/
		//Roni Start(version 4.0.9.800)
		_Module.m_server.m_cParam.LoadParam((LPCTSTR)str, "PriceLevel",&dwTmp,(DWORD)0,TRUE); 
		_Module.m_server.m_cParam.LoadParam((LPCTSTR)str, "ModeService",&dwTmp,(DWORD)0,TRUE); 
		//Roni End
		/*@@@*/
		
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "UnmannedMode",&dwTmp,(DWORD)0,TRUE); //4.0.14.503		

		//4.0.1.0
		if (bRun == FALSE)
			Sleep(REGISTRATION_SLEEP_TIME);
	}
}

void CRegTable::LoadLanguagesTable(BOOL bCreate, BOOL bCompare , BOOL bRun)
{
 	DWORD dwTmp = 0;
 
 	TStaticString<STATIC_STRING_8> strLangName;
 	TStaticString<STATIC_STRING_256> sTmpStr;
 	
 	CString strReg;
 
 	for(int i = 1; i <= MAX_LANGUAGES; i++)
 	{
 		strReg.Format("Languages\\Language%02d" , i);
 		strcpy((char*)strLangName , "");
 	
 		if(i == 1)
 			_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "ValidEntry",		&dwTmp,(DWORD)1,TRUE);
 		else
 			_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "ValidEntry",		&dwTmp,(DWORD)0,TRUE);
 
 		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg,		"LanguageName",			(char *)sTmpStr, sTmpStr.Size(), (char *)strLangName,TRUE);
 		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg,		"LangID",				&_Module.m_server.m_languagesId[i-1], (DWORD) 0,TRUE);
 		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg,		"MaxMessageLineLen",	&dwTmp,(DWORD) MAX_MSG_LINE_LEN, TRUE);
 
// 		//4.0.22.500 70766
 		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg,		"MaxCouponsEntered",	(char *)sTmpStr, sTmpStr.Size(), (char *)SCREEN_MAX_COUPONS_STR,TRUE);
 
 		// CR 433449 - Moved these messages from the "FuelLoyalty\\SysParam" entry
  		if (_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "PhoneNotFound", (char *)sTmpStr, sizeof(sTmpStr), "", FALSE))
  		{
  			_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "PhoneNotFound", (char *)sTmpStr, sizeof(sTmpStr), "Phone Number Not Found | continue?", FALSE);
  			_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "PhoneNotFound", (char *)sTmpStr, sizeof(sTmpStr), sTmpStr, TRUE);
  		}
  
   		if (_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "MutipleMemberFound", (char *)sTmpStr, sizeof(sTmpStr), "", FALSE))
  		{
   			_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "MutipleMemberFound", (char *)sTmpStr, sizeof(sTmpStr), "See Cashier to continue loyalty promotion  | continue with out promotion ?", FALSE);
   			_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "MutipleMemberFound", (char *)sTmpStr, sizeof(sTmpStr), sTmpStr, TRUE);
  		}
 
 		if (_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "ServerDown", (char *)sTmpStr, sizeof(sTmpStr), "", FALSE))
 		{
 			_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "ServerDown", (char *)sTmpStr, sizeof(sTmpStr), "Details Not Found | continue?", FALSE);
 			_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "ServerDown", (char *)sTmpStr, sizeof(sTmpStr), sTmpStr, TRUE);
 		}
 
 		if (_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "LoyaltyDeclineMsg", (char *)sTmpStr, sizeof(sTmpStr), "", FALSE))
 		{
 			_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "LoyaltyDeclineMsg", (char *)sTmpStr, sizeof(sTmpStr), "Loyalty card |was declined, |continue?", FALSE);
 			_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "LoyaltyDeclineMsg", (char *)sTmpStr, sizeof(sTmpStr), sTmpStr, TRUE);
 		}
 
 		if (_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "ServerDeclineMsg", (char *)sTmpStr, sizeof(sTmpStr), "", FALSE))
 		{
 			_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "ServerDeclineMsg", (char *)sTmpStr, sizeof(sTmpStr), "No connection |to loyalty server, |continue?", FALSE);
 			_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "ServerDeclineMsg", (char *)sTmpStr, sizeof(sTmpStr), sTmpStr, TRUE);
 		}
 
 		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "EnterPhoneNumber", (char *)sTmpStr, sizeof(sTmpStr), "Enter Phone Number", TRUE);
 		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "EnterRewardCardNumber", (char *)sTmpStr, sizeof(sTmpStr), "Enter Reward Card Number", TRUE);
 		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "DefaultMessage", (char *)sTmpStr, sizeof(sTmpStr), "Do you want to continue?", TRUE);
		
		//_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "EnterAlternateID", (char *)sTmpStr, sizeof(sTmpStr), "Enter Alternate ID", TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "InvalidMember", (char *)sTmpStr, sizeof(sTmpStr), "Invalid member", TRUE); // CR 472487
		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "LoyaltyTransactionCanceled", (char *)sTmpStr, sizeof(sTmpStr), "Transaction canceled", TRUE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, NMP_FIELD_ALTERNATE_ID_RETRY, (char *)sTmpStr, sizeof(sTmpStr), "You Entered %s: | This number is not registered", TRUE);
 		
 		if (bRun == FALSE)
 			Sleep(REGISTRATION_SLEEP_TIME);
 	}
}


void CRegTable::LoadServiceFeeTable(BOOL bCreate, BOOL bCompare , BOOL bRun)
{
	DWORD dwTmp = 0;
		
	TStaticString<STATIC_STRING_16> strModeName;
	TStaticString<STATIC_STRING_64> sTmpStr;
	
	_Module.m_server.m_cParam.LoadParam("ServiceFee", "MinFullingToChargeServiceFee",(DWORD *)&(_Module.m_server.m_lMinFullingToChargeServiceFee),(DWORD)0,TRUE);

	_Module.m_server.m_cParam.LoadParam("ServiceFee", "ManualServiceFee",&dwTmp,(DWORD)0,TRUE);	
	_Module.m_server.SetManualServiceFee(dwTmp); //4.0.8.508

	//4.0.8.508
	if(LOG_BASIC_FLOW_CONTROL)
	{
		if (dwTmp)
		{
			CString sLogMsg;
			sLogMsg.Format("Manual service fee set = %ld",dwTmp);
			_LOGMSG.LogMsg(sLogMsg);
		}
	}
	//4.0.8.508

	CString str;

	for(int i = 1; i <= MAX_SERVICE_FEE; i++)
	{		
		str.Format("ServiceFee\\ServiceFee%02d" , i);
		sprintf((char*)strModeName , "ServiceFee%02d" , i);
		
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Valid",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Name",(char *)sTmpStr, sTmpStr.Size(),(char *)strModeName,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PakService",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PapService",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PrePayService",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "AviService",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "AttendService",&dwTmp,(DWORD)0,TRUE);		
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "SelfPakService",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "SelfPapService",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "SelfPrePayService",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "SelfAviService",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "SelfAttendService",&dwTmp,(DWORD)0,TRUE);			
		
		//4.0.1.0
		if (bRun == FALSE)
			Sleep(REGISTRATION_SLEEP_TIME);
	}

	return;
}


void CRegTable::LoadTanksTable(BOOL bCreate , BOOL bCompare , BOOL bRun)
{
	//None of these parameters are relevant during run-time, so we exit
	//the method if in the middle of run time update (indicated by bCreate == FALSE
	//and bCompare == TRUE)
	if(bCreate)
	{
		DWORD dwTmp = 0;
		DWORD dwTmp1 = 0;
		CString str;
		
		_Module.m_server.m_cParam.LoadParam("Tanks", "TanksUnitsSetup",&dwTmp,(DWORD)1,TRUE);

		_Module.m_server.m_cParam.LoadParam("Tanks", "LevelFactor",&dwTmp, (DWORD)1,	TRUE);//4.0.3.36
		
		_Module.m_server.m_cParam.LoadParam("Tanks", "TemperatureFactor",&dwTmp, (DWORD)1,	TRUE);//4.0.3.51

		_Module.m_server.m_cParam.LoadParam("Tanks", "EmulationDeliveryTimeOut",&_Module.m_server.m_dwEmulationDeliveryTimeOut, (DWORD) 120, TRUE);//4.0.5.24

		//  4.0.5.32   //4.0.5.34
		_Module.m_server.m_cParam.LoadParam("Tanks", "VolumeMultiply",&_Module.m_server.m_dwVolumeMultiply, (DWORD)1,	TRUE);

		_Module.m_server.m_cParam.LoadParam("Tanks", "ActiveTankConnectedPumpsTable",&dwTmp, (DWORD)1, TRUE);	//4.0.20.52 (62818)
		_Module.m_server.m_bActiveTankConnectedPumpsTable = (dwTmp==0) ? TRUE : FALSE;						
		
		for(int i = 1; i <= MAX_TANKS; i++)
		{
			str.Format("Tanks\\Tank%02d" , i);
			
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ValidEntry",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PureProduct",&dwTmp,(DWORD)1,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "MinDelivery",&dwTmp,(DWORD)150,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PumpLinkMap",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "LogicTanks",&dwTmp,(DWORD)0,TRUE);

			//add by Yaron for Tank configuration
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "FullHeightVolume",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankDiameter",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankTilt",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "StickHeightOffset",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankDeliveryDelay",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankLowLevelLimit",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankHighLevelLimit",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankOverfillLevel",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankHighWaterLevel",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankSuddenLossLimit",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankLeakAlarmLimit",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankHighWaterWarningLimit",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankMaxVolumeLimit",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankDeliveryRequired",&dwTmp,(DWORD)0,TRUE);

			//4.0.3.60						
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankType",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CommPort",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "DeviceAddress",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "DeviceSubAddress",&dwTmp,(DWORD)0,TRUE);
			//4.0.3.60

			//The CRC field
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ParamCRC",&dwTmp,(DWORD)0,TRUE);

			//Eyal 4.0.0.79 64 pump support
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PumpLinkMap2",&dwTmp,(DWORD)0,TRUE);

			//4.0.13.501
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "LeakThresholdVolume",&dwTmp,(DWORD)0,TRUE);

			//4.0.22.1400
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "MaxPumpFlowRate",&dwTmp,(DWORD)0,TRUE);


			//4.0.13.508
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "MaximumVolumeWarning",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankHeight",&dwTmp,(DWORD)0,TRUE);

			//4.0.15.120 - CR 44000
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "AccumulatedValue",&dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "AccumulatedVolume",&dwTmp,(DWORD)0,TRUE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);

			//4.0.20.52 (62818)	{	
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "StartDeliveryVolume", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "EndDeliveryVolume", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "EndDeliveryTemperature", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "AccumulatedDeliverySales", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "StartDeliveryTemperature", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			//4.0.20.52 (62818)	}

			//4.0.21.500 - TD 66935
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "StartDeliveryWaterVolume", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "EndDeliveryWaterVolume", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

			//4.0.20.500 - TD 64928
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankMinVolumeLimit", &dwTmp,(DWORD)0, TRUE);

			//4.0.20.501 - TD 65174
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "DeliveryReference", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

			//4.0.21.500 - TD 66935
            _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "FoundDelivery", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

			//4.0.21.500 - TD 66935
            _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "LockConnectedPumps", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
			
			if (bRun == FALSE)
				Sleep(REGISTRATION_SLEEP_TIME);

			//4.0.21.500 - TD 66935:

			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ValidEntry",&dwTmp,(DWORD)0,FALSE);

			if(dwTmp)
				_Module.m_server.m_lLastValidTank = i;
		}
	}

	return;
}

void CRegTable::LoadDatabaseTable(BOOL bCreate , BOOL bCompare , BOOL bRun)
{
	DWORD dwTmp = 0;

	//4.0.24.70 TD 114988
	const size_t	iDataBaseSize = 24;
	const char		szDataBaseRoot [MAX_NAME_LEN] = "Database";
	const char		aszDataBaseName[iDataBaseSize][MAX_NAME_LEN] = {	"PrePay",
																		"OLA",
																		"PumpStat",
																		"PumpTotals",
																		"TankRead",
																		"Delivery",
																		"Alarm",
																		"CarWash",
																		"Receipt",
																		"RFS",
																		"ExtraInfo",
																		"Items",
																		"ShiftTankTotals",
																		"ShiftPumpTotals",
																		"ShiftDeliveryTotals",
																		"Ash2000",
																		"DBTankGauge",
																		"AccumulationShiftTotal",
																		"ReceiptExtraData",
																		"ReceiptExtraIndex",
																		"OLATemporary",
																		"CashAcceptorFile",
																		"PaidTrs",
																		"SingleShiftAccumulations"
																};

	const DWORD adwDataBaseNumber[iDataBaseSize] = {	PUMP_PRE_PAY_FILE,			
														PUMP_OLA_FILE,				
														PUMP_STAT_FILE,				
														PUMP_TOTALS_FILE,			
														TANK_READS_FILE,				
														FUEL_DELIVERIES_FILE,		
														ALARM_FILE,					
														CAR_WASH_FILE,				
														RECEIPT_FILE,				
														RFS_FILE,		   			
														EXTRA_INFO_FILE,				
														ITEMS_FILE,																							
														SHIFT_TANKS_FILE,			
														SHIFT_PUMP_FILE,				
														SHIFT_DELIVERY_FILE,			
														ASH2000_FILE,				
														DB_TANK_GAUGE_FILE,			
														ACCUMULATION_SHIFT_TOTAL_FILE,
														RECEIPT_EXTRA_DATA_FILE,		
														RECEIPT_EXTRA_INDEX_FILE,    
														PUMP_OLA_TEMP_FILE,			
														CASH_ACCEPTOR_FILE,			
														PAID_TRS_FILE,
														SINGLE_SHIFT_ACCUMULATIONS_FILE
													};
													
	
	for (int i = 0; i < iDataBaseSize; i++, dwTmp = 0)
	{
		_Module.m_server.m_cParam.LoadParam(szDataBaseRoot, aszDataBaseName[i], &dwTmp, adwDataBaseNumber[i], TRUE);		
		dwTmp = 0;

		CString strParamPath = szDataBaseRoot;
		strParamPath += "\\";
		strParamPath += aszDataBaseName[i];
		
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)strParamPath), "UseBackUp", &dwTmp, 0, TRUE);
	}


	return;
}


void CRegTable::LoadPathsTable(BOOL bCreate , BOOL bCompare)
{
	char szPumpSrvPath[MAX_FIELD_NAME];
	char tmpStr[MAX_FILE_PATH];

	//4.0.0.83
	char sTmpDefault[MAX_FILE_PATH];

	DWORD dwRes = 0;
	int iRes = 0;

	memset(szPumpSrvPath,0,MAX_FIELD_NAME);
	memset(tmpStr,0,sizeof(tmpStr));
	memset(sTmpDefault,0,sizeof(sTmpDefault));

	dwRes = GetCurrentDirectory(MAX_FIELD_NAME,szPumpSrvPath);

	if(dwRes)
	{
		iRes = _Module.m_server.m_cParam.LoadParam("Paths\\Long", "HomeDir",tmpStr,sizeof(tmpStr),szPumpSrvPath,TRUE);
		
		//4.0.0.83 - if the parameter was found, use the value read from 
		//the registry, otherwise create.
		if(!iRes)
		{
			//4.0.2.251
			memcpy((char*)sTmpDefault , (char*)szPumpSrvPath , sizeof(sTmpDefault));
			strcat(sTmpDefault,"\\Data");  //4.0.0.85

			iRes = _Module.m_server.m_cParam.LoadParam("Paths\\Long", "DataDir",tmpStr,sizeof(tmpStr),sTmpDefault,TRUE);  //4.0.2.271
		}
		else
		{
			memcpy((char*)sTmpDefault , (char*)szPumpSrvPath , sizeof(sTmpDefault));
			strcat(sTmpDefault,"\\Data");  //4.0.0.85

			iRes = _Module.m_server.m_cParam.LoadParam("Paths\\Long", "DataDir",tmpStr,sizeof(tmpStr),sTmpDefault,TRUE);
		}

		//Exception handler: log path	4.0.19.502 merge from 4.0.15.450	
		char szExceptionPath[MAX_FILE_PATH];
		memset(szExceptionPath,0x00,sizeof(szExceptionPath));
		_Module.m_server.m_cParam.LoadParam("Paths\\Long", "LogDir",tmpStr,sizeof(tmpStr), szExceptionPath,FALSE);
		strcpy(szExceptionPath,tmpStr);
		strcat((char *)szExceptionPath,"\\log\\PUMPSRV");	
		CExceptionHandler::ExceptionHandler().SetLoggingPath(szExceptionPath);

		//33898
		iRes = _Module.m_server.m_cParam.LoadParam("Paths\\Long", "MaintQDXDir",tmpStr,sizeof(tmpStr),szPumpSrvPath,TRUE); //TD 60205 4.0.20.500

		//4.0.13.503 
		//Always should be after taking DataDir! the defauld is the value in DataDir.
		iRes = _Module.m_server.m_cParam.LoadParam("Paths\\Long", "FirmwareDir",tmpStr,sizeof(tmpStr), tmpStr,TRUE);

		//4.0.11.504
		iRes = _Module.m_server.m_cParam.LoadParam("Paths\\Long", "TransmitDir",tmpStr,sizeof(tmpStr), sTmpDefault,TRUE);
		
		iRes = _Module.m_server.m_cParam.LoadParam("Paths\\Long", "LogDir",tmpStr,sizeof(tmpStr), szPumpSrvPath,TRUE);
						  

		iRes = _Module.m_server.m_cParam.LoadParam("Paths\\Short", "HomeDir",tmpStr,sizeof(tmpStr),szPumpSrvPath,TRUE);
		iRes = _Module.m_server.m_cParam.LoadParam("Paths\\Short", "LogDir",tmpStr,sizeof(tmpStr),szPumpSrvPath,TRUE);

		//TD 60205 4.0.20.500
		memset(sTmpDefault,0,sizeof(sTmpDefault));
		memcpy((char*)sTmpDefault , (char*)szPumpSrvPath , sizeof(sTmpDefault));
		strcat(sTmpDefault,"\\Qdx\\Drv32"); 
		iRes = _Module.m_server.m_cParam.LoadParam("Paths\\Long", "QdxEngineDir",tmpStr,sizeof(tmpStr),sTmpDefault,TRUE);

		//TD 60205 4.0.20.501
		memset(sTmpDefault,0,sizeof(sTmpDefault));
		memcpy((char*)sTmpDefault , (char*)szPumpSrvPath , sizeof(sTmpDefault));
		strcat(sTmpDefault,"\\Qdx"); 
		iRes = _Module.m_server.m_cParam.LoadParam("Paths\\Long", "QdxFilesDir",tmpStr,sizeof(tmpStr),sTmpDefault,TRUE);

		//4.0.0.86	
		memset(sTmpDefault,0,sizeof(sTmpDefault));
		memcpy((char*)sTmpDefault , (char*)szPumpSrvPath , sizeof(sTmpDefault));
		strcat(sTmpDefault,"\\FuelPriceChange.xml");
		_Module.m_server.m_cParam.LoadParam("Paths\\Long","XML_PriceChangeFile",tmpStr,sizeof(tmpStr),sTmpDefault,TRUE);
		//4.0.7.500
		memset(sTmpDefault,0,sizeof(sTmpDefault));
		memcpy((char*)sTmpDefault , (char*)szPumpSrvPath , sizeof(sTmpDefault));
		strcat(sTmpDefault,"\\Data\\Image");
		_Module.m_server.m_cParam.LoadParam("Paths\\Long","TerminalImagesPath",tmpStr,sizeof(tmpStr),sTmpDefault,TRUE);

		//4.0.3.39		
		_Module.m_server.m_cParam.LoadParam("Paths\\Long","PSTransactionDir",tmpStr,sizeof(tmpStr),szPumpSrvPath,TRUE);	
		//4.0.12.509
		_Module.m_server.m_cParam.LoadParam("Paths\\Long","TLS_CaptureFile",tmpStr,sizeof(tmpStr),szPumpSrvPath,TRUE);	
		
		//4.0.19.504 - 47559
		_Module.m_server.m_cParam.LoadParam("Paths\\Long","OfflineTrs",tmpStr,sizeof(tmpStr),"C:\\FIPAYEPS\\Offline",TRUE);

		//Configuration Data Files Path
		char szDefaultConfigDataFilesPath[MAX_PATH] = {0};				//4.0.23.504 TD 78942
		memcpy( szDefaultConfigDataFilesPath, szPumpSrvPath, min( sizeof(szDefaultConfigDataFilesPath), sizeof(szPumpSrvPath) ) );
		strcat( szDefaultConfigDataFilesPath, "\\ConfigData");
		_Module.m_server.m_cParam.LoadParam("Paths\\Long", "ConfigThreadDataFilePath", tmpStr, sizeof(tmpStr), szDefaultConfigDataFilesPath, TRUE);

		//Export Site Configuration XML File Path 4.0.26.508 TD 146575
		char szDefaultExportSiteConfigFile[MAX_PATH] = {0};				
		memcpy( szDefaultExportSiteConfigFile, szPumpSrvPath, min( sizeof(szDefaultExportSiteConfigFile), sizeof(szPumpSrvPath) ) );
		strcat( szDefaultExportSiteConfigFile, "\\Data\\SiteConfiguration.xml");
		_Module.m_server.m_cParam.LoadParam("Paths\\Long", "ExportSiteConfigFile", tmpStr, sizeof(tmpStr), szDefaultExportSiteConfigFile, TRUE);

		// R10 User Story - 92780 -  Fuel P@P - Ola Recovery.
		_Module.m_server.m_cParam.LoadParam("Paths\\Long","OlaAppFullFileName",tmpStr,sizeof(tmpStr),"",TRUE);

		//Export Data Files Path
		char szDefaultExportDataPath[MAX_PATH] = {0};	
		memcpy( szDefaultExportDataPath, szPumpSrvPath, min( sizeof(szDefaultExportDataPath), sizeof(szPumpSrvPath) ) );
		strcat( szDefaultExportDataPath, "\\ExportData");
		_Module.m_server.m_cParam.LoadParam("Paths\\Long", "ExportDataPath", tmpStr, sizeof(tmpStr), szDefaultExportDataPath, TRUE);
	}
	else
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			dwRes = GetLastError();
			str.Format("CreatePathsTable:Fail in GetCurrentDirectory(), Error: %d,",dwRes);
			_LOGMSG.LogMsg(str);
		}	
	}
}


void CRegTable::LoadPricePolesTable(BOOL bCreate , BOOL bCompare)
{
	//None of these parameters are relevant during run-time, so we exit
	//the method if in the middle of run time update (indicated by bCreate == FALSE
	//and bCompare == TRUE)
// 	if(bCreate) //4.0.26.511 147183
// 	{
		DWORD dwTmp = 0;

		char sTmpStr[MAX_FIELD_VAL];
		memset(sTmpStr,0,sizeof(sTmpStr));
		CString str , sPanel , tmpStr;

		// define PricePoles genral param		
		_Module.m_server.m_cParam.LoadParam((char *)"PricePoles", "PricePolesBaseNumber",&dwTmp,(DWORD)0,bCreate);
		
		long lNumOfPricePoleDisplays = 0;	//4.0.26.508 147183

		for(int i = 1; i <= MAX_PRICE_DISPLAYS; i++)
		{
			str.Format("PricePoles\\PricePole%02d" , i);
			
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"Type", sTmpStr, sizeof(sTmpStr) ,"FutureMedia",bCreate);

			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CommPort",&dwTmp,(DWORD)0,bCreate);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "DeviceAddress",&dwTmp,(DWORD)0,bCreate);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ValidEntry",&dwTmp,(DWORD)0,bCreate);

			if (dwTmp > 0)	//4.0.26.508 147183
			{
				++lNumOfPricePoleDisplays;
			}

			
			//4.0.5.27
			dwTmp = 0;			
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "IPAddress" , sTmpStr , sizeof(sTmpStr), "" , bCreate);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Simulator",&dwTmp,(DWORD)0,bCreate);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "BaudRate",&dwTmp,(DWORD)0,bCreate);  //


			//4.0.12.515
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "FailPriceChangeWhenOffline", &dwTmp, (DWORD)0, bCreate);

			//4.0.13.31
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "DivisionFactor", &dwTmp, (DWORD)10, bCreate);
			

			for(int p = 1; p<= MAX_PANELS_PER_DEVICE; p++)
			{
				sPanel.Format("\\Panel%02d" , p);

				tmpStr = str + sPanel;
				
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)tmpStr), "Grade",&dwTmp,(DWORD)p,bCreate);
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)tmpStr), "PriceLevel",&dwTmp,(DWORD)0,bCreate);
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)tmpStr), "ServiceMode",&dwTmp,(DWORD)0,bCreate);
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)tmpStr), "Discount",&dwTmp,(DWORD)0,TRUE); // CR 393826
				_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)tmpStr), "ValidEntry",&dwTmp,(DWORD)0,bCreate);			
			}
		}
		_Module.m_server.SetNumOfPricePoleDisplays(lNumOfPricePoleDisplays);
// 	}
	
	return;
}

void CRegTable::LoadANPRTable(BOOL bCreate , BOOL bCompare) //4.0.26.505 157073
{
	//None of these parameters are relevant during run-time, so we exit
	//the method if in the middle of run time update (indicated by bCreate == FALSE
	//and bCompare == TRUE)
	if(bCreate)
	{
		DWORD dwTmp = 0;

		TStaticString<STATIC_STRING_64> sTmpStr;
		CString str , sPanel , tmpStr;

		// define PricePoles genral param		
		_Module.m_server.m_cParam.LoadParam((char *)"ANPR", "ANPROn",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.SetANPRState(dwTmp);
		_Module.m_server.m_cParam.LoadParam((char *)"ANPR", "AuthorizeOnFailed",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)"ANPR", "UnblockOnIDLE",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)"ANPR", "IPAddress", sTmpStr, sTmpStr.Size() ,"",TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)"ANPR","Port",&dwTmp,0,1,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)"ANPR", "Type", sTmpStr, sTmpStr.Size() ,"",TRUE);
	}
	
	return;
}

void CRegTable::LoadTaxesTable(BOOL bCreate , BOOL bCompare)
{
	//None of these parameters are relevant during run-time, so we exit
	//the method if in the middle of run time update (indicated by bCreate == FALSE
	//and bCompare == TRUE)
	//if(FALSE ==bCreate)
		//return;
	
	BOOL bTaxExists = FALSE,bCreateNewTable=FALSE;
	long lNewTaxesInserted = 0 ,lInLoopCounter = 0,lReferredTax=0,lMaskTax = 0;
	CString sTax, sTaxDefName,str , strGrade;
	DWORD dwNewTax = 0, dwValidEntry = 0 , dwTmp = 0 , dwInfo = 0;
	TStaticString<STATIC_STRING_64> sTmpStr;

	_Module.m_server.m_cParam.LoadParam("Taxes\\GeneralParam", "TaxRegulation",&dwTmp,(DWORD)0,TRUE);
	
	if (_Module.m_server.m_cParam.LoadParam("Taxes\\Tax01", "TaxID",&dwTmp,(DWORD)0,FALSE))
		bCreateNewTable = TRUE;
		
	for(int i = 0; i < MAX_TAXES; i++)
	{
		str.Format("Taxes\\Tax%02d" , i+1);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"ValidEntry",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cTaxMap[i].bValid = dwTmp;

		//The values which should be inserted regardless of whether a new table should be created are handled
		//here. Additional parameters should also be inserted here
			
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"TaxName", _Module.m_server.m_cTaxMap[i].sTaxName , sizeof(_Module.m_server.m_cTaxMap[i].sTaxName) ,"",TRUE);

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TaxID",&_Module.m_server.m_cTaxMap[i].lTaxId,(DWORD)i,TRUE);

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TaxTypeFlag", &_Module.m_server.m_cTaxMap[i].bIsTaxTypeCPL, (DWORD)0, TRUE); //4.0.3.41 
		
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Included", &_Module.m_server.m_cTaxMap[i].bIsIncluded, (DWORD)1, TRUE); //4.0.3.41 						
		
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TaxRate", &_Module.m_server.m_cTaxMap[i].lTaxRate, (DWORD)1, TRUE); //4.0.3.41 						
		
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"TaxPrintChar", sTmpStr , 64 ,"",TRUE);

		

		memcpy(&_Module.m_server.m_cTaxMap[i].sTaxPrintChar,sTmpStr,1);
			
		if (bCreateNewTable)
		{
			dwInfo = 0;

			strGrade.Format("Grades\\Grade%02d" , i);
						
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)strGrade), "TaxRate",&dwInfo,(DWORD)0,FALSE);
				
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)strGrade), "ValidEntry",&dwValidEntry,(DWORD)0,FALSE);
				
			bTaxExists = FALSE;
				
			if((dwInfo != 0) && (1 == dwValidEntry) ) //A new tax has been found 				
			{
				//Searching the taxes which have been inserted so far in order to find out whether
				//the tax found in the grade already exists 
				for(lInLoopCounter = 0; lInLoopCounter < lNewTaxesInserted; lInLoopCounter++)
				{
					sTax.Format("Taxes\\Tax%02d" , lInLoopCounter + 1);
					_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTax), "TaxRate", &dwNewTax,(DWORD) 0, FALSE);
					if(dwInfo == dwNewTax)
					{
						//The tax already exists and the grade should point to it
						lReferredTax = lInLoopCounter + 1;
						bTaxExists = TRUE;
						break;
					}
				}
				if(!bTaxExists )
				{
					//The tax doesn't exist yet - we'll insert it here
					lReferredTax = lNewTaxesInserted + 1;
					sTax.Format("Taxes\\Tax%02d" , lNewTaxesInserted + 1);
					_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sTax), "TaxRate",&dwTmp,(DWORD)dwInfo,TRUE);
					lNewTaxesInserted++;

					sTaxDefName.Format("Tax%02d", lNewTaxesInserted);
					_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)sTax), "TaxName", ((char*) (LPCTSTR) sTaxDefName));
				}

				//Saving the new pointed tax to the grade's MapTaxLink parameter - using a mask
				lMaskTax = (long) pow((double)2, lReferredTax - 1);
				_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)strGrade), "MapTaxLink", lMaskTax );
			}
			//4.0.5.26 end - Udi
		}
		else
		{
			//A new table wasn't created
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TaxRate",&dwTmp,(DWORD)0,TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ValidEntry",&dwTmp,(DWORD)0,TRUE);
		}
	}
	//4.0.5.26 start - Udi
	if(bCreateNewTable)
	{
		//Inserting 1 into the ValidEntry parameter of the valid taxes
		for(int i = 1; i <= lNewTaxesInserted; i++)
		{
			str.Format("Taxes\\Tax%02d" , i);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ValidEntry",&dwTmp,(DWORD) 1,TRUE);
		}

		for(int i = lNewTaxesInserted + 1; i <= MAX_TAXES; i++)
		{
			str.Format("Taxes\\Tax%02d" , i);
			//Inserting 0 into the ValidEntry parameter of the invalid taxes
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ValidEntry",&dwTmp,(DWORD) 0, TRUE);
			//Inserting 0 into the TaxRate parameter of the invalid taxes
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TaxRate", &dwTmp, (DWORD)0, TRUE);
		}
	}
	//4.0.5.26 end - Udi

	_Module.m_server.CreateTaxCalcator();
	
}


void CRegTable::LoadIPTTable(BOOL bCreate , BOOL bCompare)
{
	//None of these parameters are relevant during run-time, so we exit
	//the method if in the middle of run time update (indicated by bCreate == FALSE
	//and bCompare == TRUE)
	if(bCreate)
	{
		DWORD dwTmp = 0;

		CString str;
		TStaticString<STATIC_STRING_64> sTmpStr;
		
		for(int i = 0; i < MAX_IPT; i++)
		{
			str.Format("IPT\\IPT%02d" , i + 1);
			
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "DeviceAddress",			&dwTmp, (DWORD)(i + 1),	TRUE);

			//changed value to set to i / 4 + 2
			//so that terminals 1 - 4 will be at port number 2
			//terminals 5 - 8 will be at 3, etc.
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CommPort",				&dwTmp, (DWORD)((i / 4) + 2),	TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Simulator",				&dwTmp, (DWORD)0,	TRUE);


			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CardReader",			&dwTmp, (DWORD)1,	TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CashAcceptor",			&dwTmp, (DWORD)0,	TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "KeyboardType",			sTmpStr, sTmpStr.Size() ,"1", TRUE);

			//In use by RFS
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "EncryptionKey",			sTmpStr, sTmpStr.Size() , "Non Valid Key", TRUE);

			//In use by Allied CL
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PINEncryption",			&dwTmp, (DWORD)1,	TRUE);
			
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ValidEntry",				&dwTmp, (DWORD)0,	TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Type" , sTmpStr , sTmpStr.Size() , "PT3000" , TRUE);
			

			//The CRC field 
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ParamCRC",&dwTmp,(DWORD)0,TRUE);
		
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "OfflineTimeOut(mSec)" , &dwTmp , (DWORD)40 , TRUE);

			//GraphicDevice
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "GraphicDevice",	&dwTmp, (DWORD)1 , TRUE);

			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PumpGroup01",	&dwTmp, (DWORD)0,	TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PumpGroup02",	&dwTmp, (DWORD)0,	TRUE);
			
			//4.0.3.29
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CashAcceptorMaxAmount",	&dwTmp, (DWORD)0,	TRUE);
			//4.0.3.29
		}
	}

	return;
}

void CRegTable::LoadTerminalTable(BOOL bCreate , BOOL bCompare)
{
	DWORD dwTmp = 0;

	CString str;
	TStaticString<STATIC_STRING_64> sTmpStr;
	
	BYTE byCardReaders = 0 , byCashAcceptors = 0;

	for(int i = 0; i < MAX_TERMINALS; i++)
	{
		str.Format("Terminals\\Terminal%02d" , i + 1);
		
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "DeviceAddress",			&dwTmp, (DWORD)(i + 1),	TRUE);

		//changed value to set to i / 4 + 2
		//so that terminals 1 - 4 will be at port number 2
		//terminals 5 - 8 will be at 3, etc.
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CommPort",				&dwTmp, (DWORD)((i / 4) + 2),	TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Simulator",				&dwTmp, (DWORD)0,	TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Non_Pump",				&dwTmp, (DWORD)0,	TRUE);//4.0.12.501
		 

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CardReader",			&dwTmp, (DWORD)1,	TRUE);

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ContactlessReader",	&dwTmp, (DWORD)0,	TRUE); //4.0.31.??

		//4.0.32.?? CR407355 Support non-concatenated   messages for SPOT M3 2 wire connection , Rinatk
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "SupportMessagesOneByOne",	&dwTmp, (DWORD)0,	TRUE); //4.0.31.??
		//4.0.1.32
		if(dwTmp)
			byCardReaders = (BYTE)1;

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CashAcceptor",			&dwTmp, (DWORD)0,	TRUE);

		//4.0.1.32
		if(dwTmp)
			byCashAcceptors = (BYTE)1;

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "KeyboardType",			sTmpStr, sTmpStr.Size() ,"1", TRUE);

		//In use by RFS
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "EncryptionKey",			sTmpStr, sTmpStr.Size(), "Non Valid Key", TRUE);

		//In use by Allied CL
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PINEncryption",			&dwTmp, (DWORD)1,	TRUE);
		
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ValidEntry",				&dwTmp, (DWORD)0,	TRUE);

		//4.0.1.32
		if(dwTmp)
		{
			_Module.m_server.m_cTerminalDevices[i].SetNumberOfCardReaders(byCardReaders);
			_Module.m_server.m_cTerminalDevices[i].SetNumberOfCashAcceptors(byCashAcceptors);
		}

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Type" , sTmpStr , sTmpStr.Size() , "PT3000" , TRUE);

		//The CRC field 
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ParamCRC",&dwTmp,(DWORD)0,TRUE);
	
		//GraphicDevice
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "GraphicDevice",	&dwTmp, (DWORD)1 , TRUE);

		//End Eyal 3.1.1.62
		
		//3.2.0.69
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "OriginTrans",	sTmpStr, sTmpStr.Size(), "P", TRUE);
		//3.2.0.69

		//4.0.1.35
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "BarcodeScanner",	&dwTmp, (DWORD)0 , TRUE);
		//4.0.1.35

		//4.0.2.15
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "IPAddress" , sTmpStr , sTmpStr.Size() , "" , TRUE);
				
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "BaudRate" , &dwTmp , (DWORD)9600 , TRUE);

		//4.0.3.39
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PumpGroup01",	&dwTmp, (DWORD)0 , TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PumpGroup02",	&dwTmp, (DWORD)0 , TRUE);
		//4.0.3.39

		//4.0.3.60
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "NotesLinkMap",	&dwTmp, (DWORD)0 , TRUE);


		//4.0.1.32 - prepare variables for next iteration of loop.
		byCashAcceptors = byCardReaders = 0;

		//4.0.5.35
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "DefaultLanguage",	&dwTmp, (DWORD)1 , TRUE);		

		//4.0.5.36
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "SignInToTerminal",	&dwTmp, (DWORD)0 , TRUE);			
		
		//4.0.5.38
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"PresetMode", &dwTmp,(DWORD)0,TRUE); 	

		//4.0.13.501
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"ReceiptPaddingBlanks", &dwTmp,(DWORD)0,TRUE); 	

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"TerminalSupportOnScreenSoftkeys", &dwTmp,(DWORD)0,TRUE); 	
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"GraphicsColdStart", &dwTmp,(DWORD)0,TRUE); 	
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"EmvTablesColdStart", &dwTmp,(DWORD)0,TRUE); 	
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"EmvSupport", &dwTmp,(DWORD)0,TRUE); 	

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"KeyboardPciVer2",	&dwTmp,(DWORD)0,TRUE); 	
		
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"OtiGcmVer2",		&dwTmp,(DWORD)0,TRUE); 	
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"PCDFileColdStart",	&dwTmp,(DWORD)0,TRUE); 
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "SpotResourcesColdStart", &dwTmp, (DWORD)0, TRUE); // TD 467646  [25/12/2016 MaximB]
	}

	return;
}

//4.0.3.60
void CRegTable::LoadBankNoteTable(BOOL bCreate, BOOL bCompare, BOOL bRun)
{
	CString str;
	DWORD dwTmp=0;
	
	if(bCreate)
	{
		for(int i = 1; i <= MAX_BANK_NOTE_LEVEL; i++)
		{
			str.Format("BankNotes\\BankNote%02d" , i);
			
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "NoteCurrencyCode",			&dwTmp, 0,	TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "NoteValue",				&dwTmp, 0,	TRUE);
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ValidEntry",				&dwTmp, 0,	TRUE);
		}
	}
}


void CRegTable::LoadDOMSSubTable(long lSubTable, BOOL bCreate, BOOL bCompare)
{
	CString sLogMsg("CRegTable::LoadDOMSSubTable - ") , sTable;


	if(lSubTable & DOMS_TABLE_GENERAL_PARAM)
	{
		sLogMsg += "DOMS_TABLE_GENERAL";
	}
	if(lSubTable & DOMS_TABLE_DECIMAL_POINT)                     
	{
		sLogMsg += "DOMS_TABLE_DECIMAL_POINT";
	}
	if(lSubTable & DOMS_TABLE_FUELLING_MODE_GROUPS)              
	{
		sLogMsg += "DOMS_TABLE_FUELLING_MODE_GROUPS";
	}
	if(lSubTable & DOMS_TABLE_LANGUAGES)							
	{
		sLogMsg += "DOMS_TABLE_LANGUAGES";
	}
	if(lSubTable & DOMS_TABLE_FUELLING_MODES)					
	{
		sLogMsg += "DOMS_TABLE_FUELLING_MODES";
	}
	if(lSubTable & DOMS_TABLE_IPT_EXTRA_INFOS)					
	{
		sLogMsg += "DOMS_TABLE_IPT_EXTRA_INFOS";
	}
	if(lSubTable & DOMS_TABLE_SERVICE_MODES)					
	{
		sLogMsg += "DOMS_TABLE_SERVICE_MODES";
	}
	if(lSubTable & DOMS_TABLE_TRANSACTION_TYPES)				
	{
		sLogMsg += "DOMS_TABLE_TRANSACTION_TYPES";		
	}
	_LOGMSG.LogMsg(sLogMsg);	
}

/******************************************************************************
 Description:	Converts registry table name to  table id 	
 Returns:      	long - Table id or 0 if failed to convert.
 Parameters:   	char* pszTableName
		
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			1/12/2003   14:35		Start
******************************************************************************/
long CRegTable::ConvertTableNameToTableId(char *pszTableName)
{
	long lTableId = 0;

	if( !strcmp(pszTableName, "CarWash") )
		lTableId = CARWASH_TABLE;
	else if( !strcmp(pszTableName, "Database") ) 
		lTableId = DATABASE_TABLE;
	else if( strstr(pszTableName, "Grades") ) 
		lTableId = GRADES_TABLE;
	else if( strstr(pszTableName, "IPT") ) 
		lTableId = IPT_TABLE;
	else if( !strcmp(pszTableName, "Messages") ) 
		lTableId = MESSAGES_TABLE;
	else if( strstr(pszTableName, "PricePoles") ) 
		lTableId = PRICE_POLES_TABLE;
	else if( strstr(pszTableName, "ANPR") ) //4.0.26.505 157073
		lTableId = ANPR_TABLE;
	else if( strstr(pszTableName, "Pumps") ) 
		lTableId = PUMPS_TABLE;
	else if( strstr(pszTableName, "PureProducts") ) 
		lTableId = PURE_PRODUCTS_TABLE;
	else if( !strcmp(pszTableName, "Receipt") ) 
		lTableId = RECEIPT_TABLE;
	else if( !strcmp(pszTableName, "RFS") ) 
		lTableId = RFS_TABLE;
	else if( strstr(pszTableName, "ScreenLayouts") ) 
		lTableId = SCREEN_LAYOUT_TABLE;
	else if( strstr(pszTableName, "ServiceFee") ) 
		lTableId = SERVICE_FEE_TABLE;
	else if( strstr(pszTableName, "Shifts") ) 
		lTableId = SHIFTS_TABLE;
	else if( strstr(pszTableName, "Tanks") ) 
		lTableId = TANKS_TABLE;
	else if( strstr(pszTableName, "Taxes") ) 
		lTableId = TAXES_TABLE;
	else if( strstr(pszTableName, "Terminals") ) 
		lTableId = TERMINALS_TABLE;
	else if( strstr(pszTableName, "WashPrograms") ) 
		lTableId = WASH_PROGRAMS_TABLE;
	else if( !strcmp(pszTableName, "Allied") ) 
		lTableId = ALLIED_TABLE;
	else if( !strcmp(pszTableName, "GeneralParam") ) 
		lTableId = GENERAL_PARAM_TABLE;
	else if( !strcmp(pszTableName, "Meggitt") ) 
		lTableId = MEGGITT_TABLE;
	else if( !strcmp(pszTableName, "Doms") ) 
		lTableId = DOMS_TABLE;
	else if( !strcmp(pszTableName, "Languages") ) 
		lTableId = LANGUAGES_TABLE;

	return lTableId;
}

void CRegTable::ReloadDataExportSrv(BOOL bCreate, BOOL bCompare)
{
	DWORD dwTmp = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "DataExportInUse",&dwTmp,(DWORD)0,bCreate);
	
	if(bCompare)
	{
		if(dwTmp)
		{
			_Module.m_server.ConnectToDataExport();
		}
		else
		{
			_Module.m_server.DataExportCleanUp(TRUE);
		}
	}

	return;
}


void CRegTable::LoadFuelLoyalty(BOOL bCreate, BOOL bCompare)
{
	DWORD dwTmp = 0;
	TStaticString<STATIC_STRING_64> sTmpStr;

	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","DataBaseType",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","RoundingOption",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","CheckoutBank",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","LogLevel",&dwTmp,3,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","WeightType",&dwTmp,75,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","PromoConflictMngActive",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","LogAddItemInfoMsg",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","LogGetDiscountMsg",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","LogAddCouponMsg",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","LogAddDiscountMsg",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","LogAddTenderMsg",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","LogInitMsgMsg",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","SupportQueryItem",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","PromDllUseBinInterface",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","CouponNeedTriggeredProm",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","AddMemberCardMsg",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","GetTriggeredDiscountsOnly",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","XMLLogFileName",sTmpStr,sTmpStr.Size(),"PromSrv.xml",bCreate);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","LogFileName",sTmpStr,sTmpStr.Size(),"PromSrv.log",bCreate);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","TargetMsgTemplate",sTmpStr,sTmpStr.Size(),"VOUCHR",bCreate);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","PromotionTemplate",sTmpStr,sTmpStr.Size(),"PROMSG",bCreate);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","VirtualPosID",&dwTmp,0,TRUE);	//Hyvee 4.0.19.502
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","PSAuthorizePaymentBeforeStartTicket",&dwTmp,0,TRUE);	//Copient 55286 4.0.19.502
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","LoyaltyUniqueClient",&_Module.m_server.m_dwLoyaltyUniqueClient,0,TRUE);	//Hyvee 4.0.19.502 // TD 321602   4.0.111.174 
	_Module.m_server.m_cParam.LoadParam("Paths\\Long", "HomeDir",sTmpStr,sTmpStr.Size(),"c:\\Office\\pumpSrv\\log",bCreate); //4.0.19.506
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","LoyaltyDataPath",sTmpStr,sTmpStr.Size(),sTmpStr,bCreate);
	_Module.m_server.m_sLoyaltyDataPath = sTmpStr; // TD 321602   4.0.111.174 
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","DepCode",&dwTmp,0,TRUE);	//Hyvee 4.0.19.503 + Copient 55286


	_Module.m_server.m_cFuelLoayaltyPS.SetDepCode(dwTmp);	//4.0.19.505
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "LoyaltyDeclineMsg", sTmpStr,sTmpStr.Size(), "Loyalty card |was declined, |continue?", bCreate);	//4.0.19.506
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "ServerDeclineMsg", sTmpStr,sTmpStr.Size(), "No connection |to loyalty server, |continue?", bCreate);	//4.0.20.53
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "InvalidMember", sTmpStr, sTmpStr.Size(), "Invalid member", bCreate);	//CR 472487
	
	//4.0.23.120 88350 start //4.0.23.508
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","CashierID", &dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","StoreLanguage", &dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","PrintMsgDest", &dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","PrintDevice", &dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","DecimalPoints",&dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","XmlSupportedLanguage", &dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","TrainingMode", &dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","RefundTicket", &dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","DepCode", &dwTmp, 0, TRUE);	//Hyvee 4.0.19.503 + Copient 55286
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","DefaultLanguageId", &dwTmp, 32, TRUE);	//Hyvee 4.0.19.503 + Copient 55286
	
	//4.0.23.120 88350 end
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","DynamicAttrForGradeID", &dwTmp, 0, TRUE); //4.0.23.170 102319
	_Module.m_server.SetDynamicAttrForGradeID(dwTmp);//4.0.23.170 102319

	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "UnlockLoyaltyMember", &_Module.m_server.m_cFuelLoayaltyPS.m_dwUnlockLoyaltyMember, 0, TRUE);
	//_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "AutoSelectFirstActiveMember", &_Module.m_server.m_cFuelLoayaltyPS.m_dwAutoSelctFirstActiveMember, 0, TRUE);//4.0.23.2670 CR 472489 

	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam","PumpAutoStartSegmentID", &dwTmp, 2, TRUE);	//4.0.23.508 89149

	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Trigger","GetImmediateDiscounts",&_Module.m_server.m_cFuelLoayaltyPS.m_dwImmediateDiscounts,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Trigger","GetOrderDiscounts", &dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Trigger","GetPointsDiscounts",&dwTmp, 0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Trigger","GetChargeInfo", &dwTmp, 1, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Trigger","GroupBy", &dwTmp, 1, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Trigger","GetLoyaltyRequiredDiscounts", &dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Trigger","GetNoneLoyaltyRequiredDiscounts",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Trigger","GetTriggeredDiscountsOnly",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Trigger","GetTargetDiscounts",&dwTmp,0,TRUE);	//4.0.20.40 63629


	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\AddItem","GetImmediateDiscounts",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\AddItem","GetOrderDiscounts",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\AddItem","GetPointsDiscounts",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\AddItem","GetChargeInfo",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\AddItem","GroupBy",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\AddItem","GetLoyaltyRequiredDiscounts",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\AddItem","GetNoneLoyaltyRequiredDiscounts",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\AddItem","GetTriggeredDiscountsOnly",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\AddItem","GetTargetDiscounts",&dwTmp,0,TRUE); //4.0.20.40 63629

	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Query","GetImmediateDiscounts",&dwTmp,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Query","GetOrderDiscounts",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Query","GetPointsDiscounts",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Query","GetChargeInfo",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Query","GroupBy",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Query","GetLoyaltyRequiredDiscounts",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Query","GetNoneLoyaltyRequiredDiscounts",&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Query","GetTriggeredDiscountsOnly",&dwTmp,1,TRUE);

	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Server","port",&dwTmp,12000,1,TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Server","ip",sTmpStr,sTmpStr.Size(),"10.168.77.2",bCreate);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Server","UsingComInterface",&dwTmp,0,1,TRUE); //4.0.19.505
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Server", "LoyaltyEngineType", &_Module.m_server.m_cFuelLoayaltyPS.m_dwLoyaltyEngineType, 0, 1, TRUE); //4.0.19.505
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\Server", "LoyaltyInterfaceType", &_Module.m_server.m_cFuelLoayaltyPS.m_dwLoyaltyInterfaceType, 0, 1, TRUE);

	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "ReceiptInfoFlags", &dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "ReceiptInfoEnableFlag", &dwTmp, 1, TRUE);

	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "GetMessagesForTerminal", &dwTmp, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "GetLoyaltyVirtualAccountsOpenBalance", &dwTmp, 0, TRUE);  // TD 454424
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "VirtualMemberAccountId", &_Module.m_server.m_cFuelLoayaltyPS.m_dwVirtualMemberAccountId, 1, TRUE);  // TD 454424

	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "CheckMemberStatus", &_Module.m_server.m_cFuelLoayaltyPS.m_dwCheckMemberStatus, 0, TRUE);
	_Module.m_server.m_cParam.LoadParam("FuelLoyalty\\SysParam", "MinLoyaltyUnitPrice", &dwTmp, 1, TRUE);  //RFUEL-3309,RFUEL-3194 Minimum Rollback Price After Discount
	_Module.m_server.m_cFuelLoayaltyPS.SetMinLoyaltyUnitPrice(dwTmp*10); // Configuration is sat by cents while grade prices are 10 cent/10 

	char lpePath[100];
	_Module.m_server.m_cParam.LoadParam("Paths\\Long", "XML_LpeMemberRestrictionsFile", lpePath, sizeof(lpePath), "C:\\Office\\PumpSrv\\Data\\LpeMemberRestrictions.xml", TRUE);
	CXMLInterface xmlInitDoc;
	try
	{
		CXMLPointers pLpeMemberRestriction, pMembersRestrictions, pCardsRestrictions, pMembersListRestrictions, pCardsListRestrictions, pMember, pCard;

		BOOL bRetCode = xmlInitDoc.InitDoc();
		if (bRetCode)
			bRetCode = xmlInitDoc.LoadFile(lpePath);
		if (bRetCode)
			bRetCode = xmlInitDoc.GetRootNode(&pLpeMemberRestriction);
		if (bRetCode)
			bRetCode = xmlInitDoc.SelectSingleElementNode("MembersRestrictions", &pMembersRestrictions, &pLpeMemberRestriction);

		xmlInitDoc.GetChildNodesList(&pMembersRestrictions, &pMembersListRestrictions);
		if (pMembersListRestrictions.m_pMSListOfItems)
		{
			while (xmlInitDoc.GetNextItem(pMembersListRestrictions, pMember))
			{
				DWORD dwRestrictionID = xmlInitDoc.GetAttributeToInt("RestrictionID", &pMember);
				BOOL bIsActiveStatus = xmlInitDoc.GetAttributeToInt("IsActiveStatus", &pMember);
				// CString str = xmlInitDoc.GetStringAttribute("RestrictionDescription",&pMember);
				_Module.m_server.m_cFuelLoayaltyPS.m_dwFuelLoyaltyMembersMap[dwRestrictionID] = bIsActiveStatus;
			}
		}

		bRetCode = xmlInitDoc.GetRootNode(&pLpeMemberRestriction);
		if (bRetCode)
			bRetCode = xmlInitDoc.SelectSingleElementNode("CardsRestrictions", &pCardsRestrictions, &pLpeMemberRestriction);

		xmlInitDoc.GetChildNodesList(&pCardsRestrictions, &pCardsListRestrictions);
		if (pCardsListRestrictions.m_pMSListOfItems)
		{
			while (xmlInitDoc.GetNextItem(pCardsListRestrictions, pCard))
			{
				DWORD dwRestrictionID = xmlInitDoc.GetAttributeToInt("RestrictionID", &pCard);
				BOOL bIsActiveStatus = xmlInitDoc.GetAttributeToInt("IsActiveStatus", &pCard);
				// CString str = xmlInitDoc.GetStringAttribute("RestrictionDescription",&pCard);
				_Module.m_server.m_cFuelLoayaltyPS.m_dwFuelLoyaltyCardsMap[dwRestrictionID] = bIsActiveStatus;
			}
		}
	}
	catch (...)
	{
		_LOGMSG.LogMsg("CRegTable::LoadFuelLoyalty Unexpected error has occurred", LOG_LEVEL_5);
	}
	xmlInitDoc.CloseDoc();
}

//4.0.19.508 //4.0.20.10	//4.0.22.500 70767	//4.0.22.504
void CRegTable::LoadFuelFlowControl(BOOL bCreate , BOOL bCompare, BOOL bReconfigure)
{
	char sPath[2*MAX_PATH] = {0};
	DWORD dwTmp = 0;

	_Module.m_server.m_cParam.LoadParam("FlowControl", "FlowControlActive",			&dwTmp,											(DWORD)0 ,			TRUE);	//4.0.22.511 74810

	_Module.m_server.m_cParam.LoadParam("FlowControl", "SortedLoyaltyMessges",		&_Module.m_server.m_dwSortedLoyaltyMessages,	(DWORD)0 ,			TRUE);

	_Module.m_server.m_cParam.LoadParam("FlowControl", "FlowControlType",			&_Module.m_server.m_dwFlowControlType,			(DWORD)PAP_FLOW ,	TRUE);	//4.0.22.511 74810

	_Module.m_server.m_cParam.LoadParam("FlowControl", "ClubType",					&_Module.m_server.m_dwLoyaltyClubNumber,			0,				TRUE);	//
	_Module.m_server.m_cParam.LoadParam("FlowControl", "LoyaltyClubNumberRetry",	&_Module.m_server.m_dwLoyaltyClubNumberRetry,		1,				TRUE);
	_Module.m_server.m_cParam.LoadParam("FlowControl", "SendAllTrsToLoyalty",		&_Module.m_server.m_dwLoyaltySendAllTrsToLoyalty,	0,				TRUE);

	////PAP
	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAP_PATH, NMP_CAR_WASH_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_CARWASH_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAP_PATH, NMP_COUPON_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_COUPON_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAP_PATH, NMP_LOYALTY_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_LOYALTY_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAP_PATH, NMP_PROMPT_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_PROMPT_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAP_PATH, NMP_DEFERRAL_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_DEFERRAL_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAP_PATH, NMP_PAYMENT_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_PAYMENT_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAP_PATH, NMP_AUTHORIZE_PAYMENT_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_AUTHORIZE_PAYMENT_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAP_PATH, NMP_AUTHORIZE_LOYALTY_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_AUTHORIZE_LOYALTY_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAP_PATH, NMP_FUELING_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_FUELING_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAP_PATH, NMP_METHOD_OF_PAYMENT_ENTRY_NAME);	//4.0.23.20 76383
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_METHOD_OF_PAYMENT_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAP_PATH, NMP_PHONE_LOOKUP_ENTRY_NAME);	//4.0.23.1220 155754
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_PHONE_LOOKUP_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s", NMP_FLOW_CONTROL_PAP_PATH, NMP_ALTERNATE_ID_RETRY_NAME);	// CR 312183
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_LOYALTY_DEFAULT_SETUP);

	/// RFUEL-2817
	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s", NMP_FLOW_CONTROL_PAP_PATH, NMP_APPLY_CALCULATE_DISCOUNT_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_LOYALTY_DEFAULT_SETUP);



	////PAK
	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAK_PATH, NMP_COUPON_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_COUPON_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAK_PATH, NMP_LOYALTY_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_LOYALTY_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAK_PATH, NMP_DEFERRAL_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_DEFERRAL_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s",NMP_FLOW_CONTROL_PAK_PATH, NMP_AUTHORIZE_LOYALTY_ENTRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_AUTHORIZE_LOYALTY_DEFAULT_SETUP);

	memset(sPath, 0, sizeof(sPath));
	sprintf(sPath, "%s\\%s", NMP_FLOW_CONTROL_PAK_PATH, NMP_ALTERNATE_ID_RETRY_NAME);
	LoadFuelFlowControlForNMP(sPath, bCreate, bCompare, NMP_LOYALTY_DEFAULT_SETUP);

	if (bReconfigure)
		_Module.m_server.m_cFlowControlManager.Init(TRUE);
}

//4.0.19.508 //4.0.20.10
void CRegTable::LoadFuelFlowControlForNMP(char *sPath, BOOL bCreate , BOOL bCompare, long lSetUp)
{	
	DWORD dwTmp = 0;
	// TStaticString<STATIC_STRING_64> sTmpStr;
	char sTmpStr[MAX_FIELD_VAL];

	_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_ORDER,&dwTmp,0,TRUE);
	_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_VALID_ENTRY,&dwTmp,(DWORD)0 ,TRUE);
	_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_REPEAT,&dwTmp,1,TRUE);	//4.0.20.40

	if (lSetUp & ONMP_OpenCardReader)
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_CARD_READER,&dwTmp,1,TRUE);
	else
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_CARD_READER,&dwTmp,0,TRUE);
	
	if (lSetUp & ONMP_OpenEnterCancelKeys)
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_ENTER_CANCEL,&dwTmp,1,TRUE);
	else
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_ENTER_CANCEL,&dwTmp,0,TRUE);
	
	if (lSetUp & ONMP_OpenYesNoKeys)
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_YES_NO,&dwTmp,1,TRUE);
	else
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_YES_NO,&dwTmp,0,TRUE);

	if (lSetUp & ONMP_OpenPayInsideKey)	//4.0.23.860 212541
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_PAY_INSIDE,&dwTmp,1,TRUE);
	else
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_PAY_INSIDE,&dwTmp,0,TRUE);

	if (lSetUp & ONMP_OpenScanner)
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_SCANNER,&dwTmp,1,TRUE);
	else
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_SCANNER,&dwTmp,0,TRUE);
	
	if (lSetUp & ONMP_OpenNumericKeys)
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_NUMERIC,&dwTmp,1,TRUE);
	else
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_NUMERIC,&dwTmp,0,TRUE);

	_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_TIMEOUT,&dwTmp,NMP_DEFAULT_TIMEOUT,TRUE);
	_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_MESSAGE, (char *)sTmpStr, sizeof(sTmpStr), "", TRUE);	//4.0.20.10

	// UDF - Epsilon
	if (lSetUp & ONMP_OpenSoftKeys)
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_OPEN_SOFT_KEYS, &dwTmp, 1, TRUE);
	else
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_OPEN_SOFT_KEYS, &dwTmp, 0, TRUE);

	if (lSetUp & ONMP_AlternateID)
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_ALTERNATE_ID, &dwTmp, 1, TRUE);
	else
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_ALTERNATE_ID, &dwTmp, 0, TRUE);

	if (lSetUp & ONMP_UseNMPOption)
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_USE_NMP_OPTION, &dwTmp, 1, TRUE);
	else
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_USE_NMP_OPTION, &dwTmp, 0, TRUE);
}

//4.0.14.502
void CRegTable::LoadMultiSync(BOOL bCreate , BOOL bCompare)
{
	HKEY			hKey;
	long			lRetVal = 0;	

	DWORD			dwVal;
	DWORD			dwValLength;
	dwValLength = sizeof(DWORD);

	//The key is not under PumpSrv
	CString strPathName((const unsigned char*)"Software\\Retalix\\MultiSync\\GeneralParam");

	// Open Key
	lRetVal = RegOpenKeyEx(  
			HKEY_LOCAL_MACHINE, // handle of an open key 
			(LPCTSTR)strPathName,  // address of name of subkey to open
			0,   // reserved  REGSAM samDesired, 
			KEY_WRITE|KEY_READ,// security access mask
			&hKey);   // address of handle to open key); 
			
	if(lRetVal == ERROR_SUCCESS) // success to open the key.
	{		
		lRetVal = RegQueryValueEx(  hKey,
									"IsMaster",
									0,
									NULL,
									(unsigned char *)&dwVal,
									&dwValLength);

		_Module.m_server.m_bIsMasterComputer = (BOOL)dwVal;
	}

	RegCloseKey(hKey);
}

//4.0.20.501 - TD 65908
void CRegTable::LoadTerminalSrvParams(BOOL bCreate)
{
	DWORD dwTmp = 0;		
	_Module.m_server.m_cParam.LoadParam("TerminalSrv\\GeneralParam", "PAKConfirmationTimeout(sec)", &dwTmp, (DWORD)DEFAULT_PAK_CONFIRMATION_TIMEOUT, bCreate);

	return;
}


//4.0.22.501	TD 72716
void CRegTable::LoadEncodingTable(const BOOL bCreate, const BOOL bCompare)const		
{
	if (TRUE == bCreate)		//call from InitServer()
	{
		//Create:
		//	1. General Encoding parameters PCICompliant, DSSActive
		//	2. Basic Encoding parameters RemoveCardDataFromLog, PCIAccountMaskStandard
		_Module.m_server.m_cProtectedDataHandler.LoadConfiguration(bCreate);

		char szTmpStr[MAX_PATH_NAME];
		memset(szTmpStr, 0, sizeof(szTmpStr));

		DWORD dwTmp = 0;
		
		//Create Base Encode parameters
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "PCIExcludesLoyalty",	  &dwTmp, (DWORD)0, bCreate);
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "RemoveProhibitedData",	  &dwTmp, (DWORD)0, bCreate);
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "PCIStrictness",		  &dwTmp, (DWORD)0, bCreate);

		//Create DSS Encode parameters
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "DSSAlgorithm",					  &dwTmp, (DWORD)0, bCreate);
		if (ENCODE_ALGORITHM_ASYMMETRIC_ENCRYPTION_AND_HASH == dwTmp)	//TD 75741 
		{			
			_Module.m_server.m_cParam.SaveParam("GeneralParam", "PCIAccountMaskStandard", (DWORD)ANMS_HEAD0_TAIL4, bCreate);		
		}

		_Module.m_server.m_cParam.LoadParam("GeneralParam", "DSSServiceInUse",				  &dwTmp, (DWORD)0, bCreate);		//4.0.22.511
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "DeclinePAPOnDSSFail",			  &dwTmp, (DWORD)0, bCreate);
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "DisablePAPOnDSSFail",			  &dwTmp, (DWORD)0, bCreate);	
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "DSSTestMode",					  &dwTmp, (DWORD)0, bCreate);
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "DisableEncrypt",				  &dwTmp, (DWORD)0, bCreate);	
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "MaxTrsNumberToProcessOnDSSFail", &dwTmp, (DWORD)0, bCreate);	

		_Module.m_server.m_cParam.LoadParam("Paths\\Long", "DPSConfigFile",	szTmpStr,	sizeof(szTmpStr),	" ", bCreate);	
	}
	else	//call from Reconfigure2()
		_Module.m_server.m_cProtectedDataHandler.ReLoad();

	return;
}

void CRegTable::LoadRealTimeTable(BOOL bCreate)
{
	DWORD dwVal = 0;
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PriceProtectionActive", &dwVal, 0, TRUE);
	_Module.m_server.m_lPriceProtectionActive = dwVal;

	char sParamValue[MAX_FILE_PATH] = { 0 };
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PriceProtectionStart", sParamValue, sizeof(sParamValue), "1111", TRUE);
	_Module.m_server.m_lPriceProtectionStart = a2l((unsigned char*)sParamValue, strlen(sParamValue));

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PriceProtectionEnd", sParamValue, sizeof(sParamValue), "1111", TRUE);
	_Module.m_server.m_lPriceProtectionEnd = a2l((unsigned char*)sParamValue, strlen(sParamValue));

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PriceProtectionLevel", &dwVal, (DWORD)0, TRUE);
	_Module.m_server.m_lPriceProtectionLevel = dwVal;

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString str;
		str.Format("LoadRealTimeTable() - PriceProtectionActive %ld, PriceProtectionStart %ld, PriceProtectionEnd %ld, PriceProtectionLevel %ld", _Module.m_server.m_lPriceProtectionActive,
			_Module.m_server.m_lPriceProtectionStart,
			_Module.m_server.m_lPriceProtectionEnd,
			_Module.m_server.m_lPriceProtectionLevel);
		_Module.m_server.m_cLogger.LogMsg(str);
	}
}