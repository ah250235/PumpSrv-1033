// Tank.cpp: implementation of the CTank class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PumpSrv.h"
#include "Tank.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//******************************************************************************
//******************************************************************************

//				CTank class

//******************************************************************************
//******************************************************************************

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTank::CTank()
{
	m_lTankValid   = false;
	m_lTankNumber  = -1;
	m_cProductName = "";
	m_cTime        = CTime::GetCurrentTime();

	m_lFuelLavel     = 0;
	m_lFuelVolume    = 0;	
	m_lManifold      = 0;
	m_lProduct       = 0;
	m_lShiftNumber   = 0;
	m_lStatusGeneral = 0;
	m_lStatusSpecial = 0;
	m_lTankCapacity  = 0;
	m_lTCVolume      = 0;
	m_lTemperature   = 0;
	m_lUllage        = 0;
	m_lWaterLavel    = 0;
	m_lWaterVolume   = 0;
	m_nNACSCode      = 0;

	//4.0.2.30
	m_lLowTankLevelLimit = 0;
	m_lHighTankLevelLimit= 0;
	
	//4.0.3.36
	m_lLevelFactor = 0;
	m_lTemperatureFactor = 0; //4.0.3.51
	
	//4.0.7.501
	m_cMemoDeliveryInfo.m_cDeliveryTime = CTime::GetCurrentTime();
	m_cMemoDeliveryInfo.m_lDeliveryVolume = 0;

	m_lPumpsLinkedMap1 = 0; //TD 66935
	m_lPumpsLinkedMap2 = 0; //TD 66935
	m_eTankState	   = TANK_STATE_READY;	// 4.0.25.1130
}

CTank::CTank(const CTank & cTank , BOOL bOnlyTankInfo)
{
	m_cProductName = cTank.m_cProductName;
	m_cTime        = cTank.m_cTime;

	m_lFlags        = cTank.m_lFlags;
	m_lFuelLavel    = cTank.m_lFuelLavel;
	m_lFuelVolume   = cTank.m_lFuelVolume;	
	m_lManifold     = cTank.m_lManifold;
	m_lProduct      = cTank.m_lProduct;
	m_lShiftNumber  = cTank.m_lShiftNumber;
	m_lStatusGeneral= cTank.m_lStatusGeneral;
	m_lStatusSpecial= cTank.m_lStatusSpecial;
	m_lTankCapacity = cTank.m_lTankCapacity;
	m_lTankNumber   = cTank.m_lTankNumber;
	m_lTankValid    = cTank.m_lTankValid;
	m_lTCVolume     = cTank.m_lTCVolume;
	m_lTemperature  = cTank.m_lTemperature;
	m_lUllage       = cTank.m_lUllage;
	m_lWaterLavel   = cTank.m_lWaterLavel;
	m_lWaterVolume  = cTank.m_lWaterVolume;
	m_nNACSCode     = cTank.m_nNACSCode;

	//4.0.2.30
	m_lLowTankLevelLimit = cTank.m_lLowTankLevelLimit;
	m_lHighTankLevelLimit= cTank.m_lHighTankLevelLimit;

	//4.0.3.36
	m_lLevelFactor       = cTank.m_lLevelFactor;
	m_lTemperatureFactor = cTank.m_lTemperatureFactor; //4.0.3.51
}


CTank::~CTank()
{

}

void CTank::SetTankInfo(long lProduct,long lStatusGeneral,long lFlags,
						long lFuelLevel,long lWaterLavel,long lTemp,
						long lFuelVol,long lWaterVol,long lUllage,
						long lTCVol, long lStatusSpecial, SYSTEMTIME cTime)
{
	int iRetval;
	DWORD dwTmp;
	CString  cName;
	char sTmpStr[MAX_FIELD_VAL];
	
	//STARTING CRITICAL SECTION
	m_csTank.Lock();
	
	m_lProduct       = lProduct; 
	m_lStatusGeneral = lStatusGeneral;
	m_lFlags         = lFlags;
	m_lFuelLavel     = lFuelLevel;
	m_lWaterLavel    = lWaterLavel;
	m_lTemperature   = lTemp;
	m_lFuelVolume    = lFuelVol;
	m_lWaterVolume   = lWaterVol;
	m_lUllage        = lUllage;
	m_lTCVolume      = lTCVol;
	m_lStatusSpecial = lStatusSpecial;
	
	//getting shift number
	m_lShiftNumber = _Module.m_server.GetFormalShiftNumber();
	//getting product name from registry
	CString str("PureProducts\\PureProduct");
	char sNumber[8];
	memset(sNumber,0,sizeof(sNumber));
	sprintf(sNumber,"%02d",lProduct);
	str += sNumber;
		
	ZeroMemory((char *)sTmpStr, sizeof(sTmpStr));
	iRetval = _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Name", (char *)sTmpStr, sizeof(sTmpStr),"",TRUE);
	
	//if fail reading putting an empty string.
	if ( iRetval != 0 )
		cName = "";
	else
		cName = sTmpStr;
	m_cProductName = cName;

	//Get product NACS code
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Code", &dwTmp ,0,FALSE);
	m_nNACSCode = (short)dwTmp;

	//getting manifold from registry
	str = "Tanks\\Tank";
	memset(sNumber,0,sizeof(sNumber));

	sprintf(sNumber,"%02d",m_lTankNumber);
	str += sNumber;
	iRetval = _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "LogicTanks",&dwTmp,(DWORD)0,TRUE);
	if ( iRetval != 0 )
		m_lManifold = 0;
	else
		m_lManifold = dwTmp;
	
	//getting tank capacity from registry
	iRetval = _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "FullHeightVolume",&dwTmp,(DWORD)0,TRUE);
	if ( iRetval != 0 )
		m_lTankCapacity = 0;
	else
		m_lTankCapacity = dwTmp;

	//4.0.2.30 Start
	//Get high level limit and low level limit
	iRetval = _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankLowLevelLimit",&dwTmp,(DWORD)0,TRUE);
	if ( iRetval != 0 )
		m_lLowTankLevelLimit = 0;
	else
		m_lLowTankLevelLimit = dwTmp;

	iRetval = _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TankHighLevelLimit",&dwTmp,(DWORD)0,TRUE);
	if ( iRetval != 0 )
		m_lHighTankLevelLimit = 0;
	else
		m_lHighTankLevelLimit = dwTmp;
	//4.0.2.30 End

	
	//4.0.1.32
	_Module.m_server.m_cParam.LoadParam("Tanks", "TanksUnitsSetup",&dwTmp,(DWORD)1,TRUE);
	m_lVolumeFactor =dwTmp;
	//4.0.1.32

	//4.0.3.36
	_Module.m_server.m_cParam.LoadParam("Tanks", "LevelFactor",&dwTmp, (DWORD)1,	TRUE);
	m_lLevelFactor =dwTmp;

	_Module.m_server.m_cParam.LoadParam("Tanks", "TemperatureFactor",&dwTmp, (DWORD)1,	TRUE);//4.0.3.51

	m_lTemperatureFactor =dwTmp;

	//setting current time.
	//	m_cTime = CTime::GetCurrentTime(); //disabled for 443035

	//443035 keep existing TankReading time:
	CTime TempTime(cTime);
	memcpy(&m_cTime, &TempTime, sizeof(m_cTime));


	m_csTank.Unlock();
	//ENDING CRITICAL SECTION
}


// Function name	: CTank::GetTankTagInfo
// Description	    : Sets into pBuff all tank information.
// Return type		: void 
// Argument         : void *pBuff - buffer that has tank information
void CTank::GetTankTagInfo(void *pBuff , long lVersion)   //4.0.7.500
{
	SYSTEMTIME sysTime;
	TANK_READ_MEMO_2  cTankReadMemo2; 

	//TANK_READ_MEMO *pTagTank;  //4.0.7.501
	//pTagTank = (TANK_READ_MEMO *)pBuff; //4.0.7.501

	//STARTING CRITICAL SECTION
	m_csTank.Lock();

	memset(&cTankReadMemo2 , ' ' , sizeof(TANK_READ_MEMO_2));
	
	// Build Tank product name.
	memcpy(	cTankReadMemo2.byProductName,
			(LPCTSTR)m_cProductName,
			min(sizeof(cTankReadMemo2.byProductName), m_cProductName.GetLength()) );
	
	l2a(m_lTankValid, cTankReadMemo2.byTankValid, sizeof(cTankReadMemo2.byTankValid));
	l2a(m_lTankNumber, cTankReadMemo2.byTankNumber, sizeof(cTankReadMemo2.byTankNumber));
	l2a(m_lProduct, cTankReadMemo2.byProduct, sizeof(cTankReadMemo2.byProduct));
	l2a(m_lStatusGeneral, cTankReadMemo2.byStatusGeneral,	sizeof(cTankReadMemo2.byStatusGeneral));
	l2a(m_lFlags, cTankReadMemo2.nFlags, sizeof(cTankReadMemo2.nFlags));
	l2a(m_lFuelLavel, cTankReadMemo2.byFuelLevel, sizeof(cTankReadMemo2.byFuelLevel));
	l2a(m_lWaterLavel, cTankReadMemo2.byWaterLevel, sizeof(cTankReadMemo2.byWaterLevel));
	l2a(m_lTemperature, cTankReadMemo2.byTemperature, sizeof(cTankReadMemo2.byTemperature));
	l2a(m_lFuelVolume, cTankReadMemo2.byFuelVolume, sizeof(cTankReadMemo2.byFuelVolume));
	l2a(m_lWaterVolume, cTankReadMemo2.byWaterVolume, sizeof(cTankReadMemo2.byWaterVolume));
	l2a(m_lUllage, cTankReadMemo2.byUllage, sizeof(cTankReadMemo2.byUllage));
	l2a(m_lTCVolume, cTankReadMemo2.byTCVolume, sizeof(cTankReadMemo2.byTCVolume));
	l2a(m_lShiftNumber, cTankReadMemo2.byShiftNumber, sizeof(cTankReadMemo2.byShiftNumber));
	l2a(m_lStatusSpecial, cTankReadMemo2.byStatusSpecial, sizeof(cTankReadMemo2.byStatusSpecial));
	l2a(m_lManifold, cTankReadMemo2.byManifold, sizeof(cTankReadMemo2.byManifold));
	l2a(m_lTankCapacity, cTankReadMemo2.byTankCapacity, sizeof(cTankReadMemo2.byTankCapacity));
	
	//4.0.2.36
	if ((_Module.m_server.GetAlarmSrvMode() == ALRMSRV_DIRECT_ACTIVE) && (m_lTankNumber > 0))
	{		
		if ((m_lTankNumber > 0) && (m_lTankNumber <= MAX_TANKS))
		{
			if (_Module.m_server.m_cAlarm.m_lAlarmTankMap[m_lTankNumber])
				cTankReadMemo2.byTankAlarmFromAlarmSrv = '1';
			else
				cTankReadMemo2.byTankAlarmFromAlarmSrv = ' ';		
		}
	}
	//4.0.2.36


	//4.0.1.32
	l2a(m_lVolumeFactor,cTankReadMemo2.byVolumeFactor,sizeof(cTankReadMemo2.byVolumeFactor));
	//4.0.1.32

	//4.0.2.30 start
	l2a(m_lLowTankLevelLimit , cTankReadMemo2.byTankLowLevelLimit , sizeof(cTankReadMemo2.byTankLowLevelLimit));
	l2a(m_lHighTankLevelLimit , cTankReadMemo2.byTankHighLevelLimit , sizeof(cTankReadMemo2.byTankHighLevelLimit));
	//4.0.2.30 end

	//4.0.3.36
	l2a(m_lLevelFactor , cTankReadMemo2.byLevelFactor , sizeof(cTankReadMemo2.byLevelFactor));

	//4.0.3.51
	l2a(m_lTemperatureFactor , cTankReadMemo2.byTemperatureFactor , sizeof(cTankReadMemo2.byTemperatureFactor));

	//setting time info
	m_cTime.GetAsSystemTime(sysTime);
	SystemTime2Tag(&sysTime,&(cTankReadMemo2.sDateTime));

	// add delivery information
	if ((lVersion & TRM_TYPE_INCLUDE_DELIVERY_DATA) || (lVersion & TRM_TYPE_WETSTOCK_REPORT)) //4.0.22.503 - TD 69905
	{			
		l2a(m_cMemoDeliveryInfo.m_lDeliveryVolume , cTankReadMemo2.byDeliveryVolume  , sizeof(cTankReadMemo2.byDeliveryVolume ));
		m_cMemoDeliveryInfo.m_cDeliveryTime.GetAsSystemTime(sysTime);
		SystemTime2Tag(&sysTime,&(cTankReadMemo2.sDeliveryDateTime));

		if (m_cMemoDeliveryInfo.m_lStatus) //4.0.9.503
			cTankReadMemo2.sDeliveryInProgress = '1';
			
		memcpy(pBuff , &cTankReadMemo2 ,  sizeof(TANK_READ_MEMO_2));
	}
	else
	{
		memcpy(pBuff , &cTankReadMemo2 ,  sizeof(TANK_READ_MEMO));
	}
		

	m_csTank.Unlock();
	//ENDING CRITICAL SECTION
} //4.0.7.500

bool CTank::IsTankValid()
{
	return (m_lTankValid);
}

void CTank::SetTankNumber(long lTankNumber)
{
	m_lTankNumber = lTankNumber;
}

void CTank::SetTankValidity(bool bValid)
{
	m_lTankValid = bValid;
}

void CTank::SetPumpsLinkedMap1(const long lPumpNumber) //TD 66935
{
	m_lPumpsLinkedMap1 |= 1<<(lPumpNumber-1); //Turn on the bit for the lPumpNumber, without changing the other bits
}

void CTank::SetPumpsLinkedMap2(const long lPumpNumber) //TD 66935
{
	m_lPumpsLinkedMap2 |= 1<<(lPumpNumber-1); //Turn on the bit for the lPumpNumber, without changing the other bits
}

long CTank::GetPumpsLinkedMap1() //TD 66935
{
	return (m_lPumpsLinkedMap1);
}

long CTank::GetPumpsLinkedMap2() //TD 66935
{
	return (m_lPumpsLinkedMap2);
}

/******************************************************************************
 Description:	Update memo delivery data 	
 Returns:      	
 Parameters:   cCurrentTime - delivery time
					lVolume - delivery volume. 
	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			9/11/2004   14:57		Start
******************************************************************************/

void CTank::SetDeliveryInfo(SYSTEMTIME & cCurrentTime, long lVolume, long lStatus)
{
	CTime cTmpTime(cCurrentTime);	
	m_cMemoDeliveryInfo.m_lDeliveryVolume= lVolume;
	m_cMemoDeliveryInfo.m_cDeliveryTime  = cTmpTime;
	m_cMemoDeliveryInfo.m_lStatus        = lStatus;	
}

void CTank::ResetDevilveryStatus()
{
	m_cMemoDeliveryInfo.m_lStatus  = 0;  //4.0.9.503
}
void CTank::SetTankStatus(long lStatus)
{
	m_lStatusGeneral = lStatus;
}

//******************************************************************************
//******************************************************************************

//				CDeliveryEmulationInfo class

//******************************************************************************
//******************************************************************************
CDeliveryEmulationInfo::CDeliveryEmulationInfo() //4.0.21.500 - TD 66935
{
	memset(&m_tWaitForLockTime, 0, sizeof(SYSTEMTIME));	//4.0.21.500
	memset(&m_tWaitForReadTime, 0, sizeof(SYSTEMTIME));	//4.0.21.500
	memset(&m_tEndLockTime, 0, sizeof(SYSTEMTIME));	    //4.0.21.500 TD G
	m_bWasUnlockCommandSet = FALSE;
	m_bCanIncDeliveryEmulationSales = FALSE;			//4.0.19.1260 - TD 242013
	m_lStartDeliveryEmulationDensity = 0;				//4.0.28.500 - TD 250092
	m_lEndDeliveryEmulationDensity = 0;					//4.0.28.500 - TD 250092
	m_lStartDeliveryEmulationWeight = 0;				//4.0.28.500 - TD 250092
	m_lEndDeliveryEmulationWeight = 0;					//4.0.28.500 - TD 250092
	m_lStartDeliveryEmulationProductLevel = 0;		// 4.0.25.1060  TD 377336
	m_lEndDeliveryEmulationProductLevel = 0;		// 4.0.25.1060  TD 377336
	m_lStartDeliveryEmulationWaterLevel = 0;		// 4.0.25.1060  TD 377336
	m_lEndDeliveryEmulationWaterLevel = 0;			// 4.0.25.1060  TD 377336
}


/******************************************************************************
 Description:	Save Start Delivery Emulation Volume to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008   11:31		Merge	//4.0.20.52 (62818)
AlexM           22/12/2011   14:46      updated //4.0.25.380 TD 264908
******************************************************************************/
void CDeliveryEmulationInfo::SaveStartDeliveryEmulationVolume(long lTankNumber)
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryVolume", (DWORD)m_lStartDeliveryEmulationVolume, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

/******************************************************************************
 Description:	Save End Delivery Emulation Volume to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008   11:31		Merge	//4.0.20.52 (62818)
 AlexM          22/12/2011   14:46      updated //4.0.25.380  TD 287958
******************************************************************************/
void CDeliveryEmulationInfo::SaveEndDeliveryEmulationVolume(long lTankNumber)
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryVolume", (DWORD)m_lEndDeliveryEmulationVolume, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

void CDeliveryEmulationInfo::SetStartDeliveryEmulationTime(SYSTEMTIME* cTime)
{
	memcpy(&m_cStartDeliveryEmulationTime, cTime, sizeof(SYSTEMTIME)); 
}	

/******************************************************************************
 Description:	Save Start Delivery Emulation Time to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			06/11/2008   16:38		Start	//4.0.21.500 (69374)
 AlexM          22/12/2011   14:46      updated      //4.0.25.380  TD 287958
******************************************************************************/
void CDeliveryEmulationInfo::SaveStartDeliveryEmulationTime(long lTankNumber)
{	
	SZ_TAG_SYSTEMTIME	szTagTime;

	szTagTime.sNull = 0;
	SystemTime2Tag(&m_cStartDeliveryEmulationTime, &szTagTime.sTime);

	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryTime", (char *)&szTagTime.sTime, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

void CDeliveryEmulationInfo::SetEndDeliveryEmulationTime(SYSTEMTIME* cTime)
{
	memcpy(&m_cEndDeliveryEmulationTime, cTime, sizeof(SYSTEMTIME)); 
}	
/******************************************************************************
 Description:	Save Start Delivery Emulation Time to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			06/11/2008   16:38		Start	//4.0.21.500 (69374)
 AlexM          22/12/2011   14:46      updated      //4.0.25.380  TD 287958
******************************************************************************/
void CDeliveryEmulationInfo::SaveEndDeliveryEmulationTime(long lTankNumber)
{
	SZ_TAG_SYSTEMTIME	szTagTime;

	szTagTime.sNull = 0;
	SystemTime2Tag(&m_cEndDeliveryEmulationTime, &szTagTime.sTime);

	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryTime", (char *)&szTagTime.sTime, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

/******************************************************************************
 Description:	Save Accumulated Delivery Emulation Sales to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008   11:31		Merge	//4.0.20.52 (62818)
 AlexM          22/12/2011   14:46      updated      //4.0.25.380  TD 287958
******************************************************************************/
void CDeliveryEmulationInfo::SaveAccumulatedDeliveryEmulationSales(long lTankNumber)
{	
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "AccumulatedDeliverySales", (DWORD)m_lAccumulatedDeliveryEmulationSales, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

/******************************************************************************
 Description:	Save Start Delivery Emulation Tank Temperature to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008   11:31		Start	//4.0.19.150 (62818)
 AlexM          22/12/2011   14:46      updated      //4.0.25.380  TD 287958
******************************************************************************/
void CDeliveryEmulationInfo::SaveStartDeliveryEmulationTankTemperature(long lTankNumber)
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryTemperature", (DWORD)m_lStartDeliveryEmulationTankTemperature, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

/******************************************************************************
 Description:	Save End Delivery Emulation Tank Temperature to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008   11:31		Start	//4.0.19.150 (62818)
 AlexM          22/12/2011   14:46      updated      //4.0.25.380  TD 287958
******************************************************************************/
void CDeliveryEmulationInfo::SaveEndDeliveryEmulationTankTemperature(long lTankNumber)
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryTemperature", (DWORD)m_lEndDeliveryEmulationTankTemperature, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

/******************************************************************************
 Description:	Save Start Delivery Emulation Water Volume to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			12/10/2008   12:08		Start	//4.0.21.500 TD 66935
 AlexM          22/12/2011   14:46      updated      //4.0.25.380  TD 287958
******************************************************************************/
void CDeliveryEmulationInfo::SaveStartDeliveryEmulationWaterVolume(long lTankNumber)
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryWaterVolume", (DWORD)m_lStartDeliveryEmulationWaterVolume, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

/******************************************************************************
 Description:	Save End Delivery Emulation Water Volume to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			12/10/2008   12:08		Start	//4.0.21.500 TD 66935
 AlexM          22/12/2011   14:46      updated      //4.0.25.380  TD 287958
******************************************************************************/
void CDeliveryEmulationInfo::SaveEndDeliveryEmulationWaterVolume(long lTankNumber)
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryWaterVolume", (DWORD)m_lEndDeliveryEmulationWaterVolume, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

/******************************************************************************
 Description:	Reset Start and End Water Volume field, both in memory, and in ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			12/10/2008   12:08		Start	//4.0.21.500 TD 66935
******************************************************************************/
void CDeliveryEmulationInfo::ResetDeliveryEmulationWaterVolume(long lTankNumber)
{ 
  ResetDeliveryEmulationWaterVolumeMember();                     
  SaveStartDeliveryEmulationWaterVolume(lTankNumber);
  SaveEndDeliveryEmulationWaterVolume(lTankNumber);
} 

/******************************************************************************
 Description:	Save LockConnectedPumps field to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			07/10/2008   16:08		Start	//4.0.21.500 TD 66935
 AlexM          22/12/2011   14:46      updated      //4.0.25.380  TD 287958
******************************************************************************/
void CDeliveryEmulationInfo::SaveLockConnectedPumps(long lTankNumber)
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "LockConnectedPumps", (DWORD)m_bLockConnectedPumps, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

/******************************************************************************
 Description:	Reset LockConnectedPumps field, both in memory, and in ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			07/10/2008   16:08		Start	//4.0.21.500 TD 66935
******************************************************************************/
void CDeliveryEmulationInfo::ResetLockConnectedPumps(long lTankNumber)
{ 
  ResetLockConnectedPumpsMember();     //Reset LockConnectedPumps field in memory
  SaveLockConnectedPumps(lTankNumber); //Reset LockConnectedPumps field in ini file
}

/******************************************************************************
 Description:	Save FoundDelivery field to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			07/10/2008   16:08		Start	//4.0.21.500 TD 66935
 AlexM          22/12/2011   14:46      updated      //4.0.25.380  TD 287958
******************************************************************************/
void CDeliveryEmulationInfo::SaveFoundDelivery(long lTankNumber)
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "FoundDelivery", (DWORD)m_bFoundDelivery, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

/******************************************************************************
 Description:	Reset FoundDelivery field, both in memory, and in ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			07/10/2008   16:08		Start	//4.0.21.500 TD 66935
******************************************************************************/
void CDeliveryEmulationInfo::ResetFoundDelivery(long lTankNumber)
{ 
  ResetFoundDeliveryMember();     //Reset FoundDelivery field in memory
  SaveFoundDelivery(lTankNumber); //Reset FoundDelivery field in ini file
}

/******************************************************************************
 Description:	Save Delivery Reference field to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			12/08/2008   14:46		Start	//4.0.20.501 (merge from 4.0.19.300) - TD 65174
 AlexM          05/10/2011   14:46              //4.0.29.0    - TD 264908
******************************************************************************/
void CDeliveryEmulationInfo::SaveDeliveryReference(long lTankNumber)
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "DeliveryReference", (DWORD)m_lDeliveryReference, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);
}

/******************************************************************************
 Description:	Reset Delivery Reference field, both in memory, and in ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			17/08/2008   17:22		Merge	//4.0.20.501 - TD 65174
******************************************************************************/
void CDeliveryEmulationInfo::ResetDeliveryReference(long lTankNumber)
{ 
  ResetDeliveryReferenceMember();     //Reset Delivery Reference field in memory
  SaveDeliveryReference(lTankNumber); //Reset Delivery Reference field in ini file
}

/******************************************************************************
 Description:	Reset all fields needed for DeliveryEmulationSingleTank proccess, both in memory, and in ini file
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly			12/10/2008   15:43		Start	//4.0.21.500 TD 66935
******************************************************************************/
void CDeliveryEmulationInfo::ResetDeliveryEmulationSingleTank(long lTankNumber)
{	
	ResetDeliveryReference(lTankNumber);
	ResetLockConnectedPumps(lTankNumber);
	ResetFoundDelivery(lTankNumber);
	ResetDeliveryEmulationWaterVolume(lTankNumber);

	SetVPNotification(FALSE);

	ResetDeliveryEmulationInfo();
	SaveDeliveryEmulationInfo(lTankNumber);

	TurnOffIncDeliveryEmulationSales(); //4.0.19.1260 - TD 242013
}
 

/******************************************************************************
 Description:	Save Delivery Emulation Info to ini file (only info which needs to be saved in the QDX)
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008   11:31		Merge	//4.0.20.52 (62818)
******************************************************************************/
void CDeliveryEmulationInfo::SaveDeliveryEmulationInfo(long lTankNumber)
{
	SaveStartDeliveryEmulationVolume(lTankNumber);
	SaveEndDeliveryEmulationVolume(lTankNumber);
	SaveAccumulatedDeliveryEmulationSales(lTankNumber);
	SaveEndDeliveryEmulationTankTemperature(lTankNumber);
	SaveStartDeliveryEmulationTankTemperature(lTankNumber);
	SaveStartDeliveryEmulationDensity(lTankNumber);   //4.0.25.380 - TD 287958
	SaveStartDeliveryEmulationWeight(lTankNumber);	  //4.0.25.380 - TD 287958	
	SaveEndDeliveryEmulationDensity(lTankNumber);   //4.0.25.380 - TD 287958
	SaveEndDeliveryEmulationWeight(lTankNumber);	  //4.0.25.380 - TD 287958	
	SaveStartDeliveryEmulationProductLevel(lTankNumber);  // 4.0.25.1060  TD 377336
	SaveStartDeliveryEmulationWaterLevel(lTankNumber);  // 4.0.25.1060  TD 377336
	SaveEndDeliveryEmulationProductLevel(lTankNumber);  // 4.0.25.1060  TD 377336
	SaveEndDeliveryEmulationWaterLevel(lTankNumber);  // 4.0.25.1060  TD 377336
}


/******************************************************************************
 Description:	Calculate Delivery Emulation
 Returns:      	Calculation result
 Parameters:    none	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008   11:31		Merge	//4.0.20.52 (62818)
******************************************************************************/
long CDeliveryEmulationInfo::CalculateDeliveryEmulation()
{
	long lRetVal = 0;

	lRetVal = m_lEndDeliveryEmulationVolume - m_lStartDeliveryEmulationVolume + m_lAccumulatedDeliveryEmulationSales;

	CString str;

	str.Format("Calculate Delivery Emulation: Delivery volume = %ld. Calculated by: EndDeliveryEmulationVolume (= %ld) - StartDeliveryEmulationVolume (= %ld) + AccumulatedDeliveryEmulationSales (= %ld)", lRetVal, m_lEndDeliveryEmulationVolume, m_lStartDeliveryEmulationVolume, m_lAccumulatedDeliveryEmulationSales);

    _LOGMSG.LogMsg(str);

	return	lRetVal;
}

/******************************************************************************
 Description:	Save Delivery Emulation Info In DB (only info which needs to be saved in the QDX)
 Returns:      	DB retVal
 Parameters:    Flags, lVolume - DeliveredVolume, 	cDeliveryType - "SINGLE" or "GENERAL"
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008   11:31		Merge	//4.0.20.52 (62818)
******************************************************************************/
long CTank::SaveDeliveryEmulationInfoInDB(short nFlags, long lVolume, CString szDeliveryType)
{
	long				retCode = 0;
	SZ_TAG_SYSTEMTIME	TagTime;
	DELIVERY_INFO		info;

	memset(&info, 0, sizeof(DELIVERY_INFO));

	info.lIndexNumber	= _Module.m_server.NewFuelDeliveryIndex();
	info.lTank			= m_lTankNumber;
	info.lProduct		= m_lProduct;
	info.lStatus		= m_lStatusGeneral;
	info.nFlags			= nFlags;
	
	if ( _Module.m_server.m_cServerInfo.GetWetStockFlags() & WET_STOCK_DELIVERY )
		info.nFlags |= DELIVERY_REPORT_ON_DEMAND_REQUEST;

	info.lDeliveredVolume   = lVolume ;
	info.lStartVolume	    = m_cDeliveryEmulationInfo.GetStartDeliveryEmulationVolume();
	info.lEndVolume		    = m_cDeliveryEmulationInfo.GetEndDeliveryEmulationVolume();
	info.lTemperature	    = m_cDeliveryEmulationInfo.GetEndDeliveryEmulationTankTemperature();
	info.lStartTemperature	= m_cDeliveryEmulationInfo.GetStartDeliveryEmulationTankTemperature();
	info.dwActiveReadNumber = _Module.m_server.GetActiveReadNumber();
	info.dwShiftNumber      = _Module.m_server.GetFormalShiftNumber();
	info.lStartDensity = m_cDeliveryEmulationInfo.GetStartDeliveryEmulationDensity(); // TD 253789  4.0.25.300
	info.lDensity = m_cDeliveryEmulationInfo.GetEndDeliveryEmulationDensity();		  // TD 253789  4.0.25.300
	info.lStartWeight = m_cDeliveryEmulationInfo.GetStartDeliveryEmulationWeight();   // TD 253789  4.0.25.300
	info.lTag =	   m_cDeliveryEmulationInfo.GetEndDeliveryEmulationWeight();          // TD 258652  4.0.25.302
	info.lWeight = info.lTag - info.lStartWeight;       // TD 253789  4.0.25.300      // TD 258608  4.0.25.303
	info.lStartingWater     = m_cDeliveryEmulationInfo.GetStartDeliveryEmulationWaterVolume(); // 4.0.19.1430 TD 298855
	info.lEndingWater       = m_cDeliveryEmulationInfo.GetEndDeliveryEmulationWaterVolume();   // 4.0.19.1430 TD 298855
	info.lStartingHeight  = m_cDeliveryEmulationInfo.GetStartDeliveryEmulationProductLevel();  // 4.0.25.1060  TD 377336
	info.lEndingHeight  = m_cDeliveryEmulationInfo.GetEndDeliveryEmulationProductLevel();      // 4.0.25.1060  TD 377336
	info.lStartWaterLevel  = m_cDeliveryEmulationInfo.GetStartDeliveryEmulationWaterLevel();   // 4.0.25.1060  TD 377336
	info.lEndWaterLevel  = m_cDeliveryEmulationInfo.GetEndDeliveryEmulationWaterLevel();       // 4.0.25.1060  TD 377336

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("CTank::SaveDeliveryEmulationInfoInDB: lDeliveredVolume=%d, lStartVolume=%d, lEndVolume=%d, lTemperature=%d, lStartTemperature=%d, dwActiveReadNumber=%d, dwShiftNumber=%d, lStartingWater=%d, lEndingWater=%d, lStartWeight=%d, lEndWeight=%d, lWeight=%d" ,
			info.lDeliveredVolume,info.lStartVolume,info.lEndVolume,info.lTemperature,info.lStartTemperature,info.dwActiveReadNumber,info.dwShiftNumber,info.lStartingWater,info.lEndingWater,info.lStartWeight,info.lTag,info.lWeight);
		   
		_LOGMSG.LogMsg(str);	
	}

	if(szDeliveryType == "GENERAL")
	{		
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "StartDeliveryTime",(char *)&TagTime,sizeof(SZ_TAG_SYSTEMTIME),"",FALSE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
		Tag2SystemTime(&TagTime.sTime, &info.cDeliveryDateTime);
		
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "EndDeliveryTime",(char *)&TagTime,sizeof(SZ_TAG_SYSTEMTIME),"",FALSE,TRUE,0,HOP_DIRECT_LOCAL,TRUE);
		Tag2SystemTime(&TagTime.sTime,&info.cEndDeliveryDateTime);
	}
	else if(szDeliveryType == "SINGLE") //4.0.21.500 TD G
	{
		memcpy(&info.cDeliveryDateTime, &m_cDeliveryEmulationInfo.GetStartDeliveryEmulationTime(), sizeof(SYSTEMTIME));
		memcpy(&info.cEndDeliveryDateTime, &m_cDeliveryEmulationInfo.GetEndDeliveryEmulationTime(), sizeof(SYSTEMTIME));		
	}
		
	retCode = _Module.m_server.IndexOperation(FUEL_DELIVERIES_FILE, REC_INSERT, &info);
	
	if(_Module.m_server.m_cServerInfo.GetMemoProcessFlags()) 
	{					
		SetDeliveryInfo(info.cEndDeliveryDateTime, lVolume , m_lStatusGeneral); 
		SetTankValidity(true);

		if(LOG_BASIC_FLOW_CONTROL)
		{		
			CString str;
			
			str.Format("Delivery emulation information is update to Tank=%ld, Volume=%ld lStatus=%ld", m_lTankNumber , lVolume , m_lStatusGeneral);
			_LOGMSG.LogMsg(str);
		}		
	}

	return	retCode;
}

/******************************************************************************
 Description:	Calculate Delivery Emulation records for this tank
 Returns:      	The volume of the delivery otherwise zero.
 Parameters:    	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008   11:31		Merge	//4.0.20.52 (62818) 
 AmitH			04/06/2008   10:30		continue	//4.0.19.160 (multiply he min delivery by 1000)
******************************************************************************/
long CTank::CalculateDeliveryEmulation()
{
	long	lRetVolume = 0;
	long	lMinDeliveryVolume = 150;	//to be move to CTank member	!!!
	CString	str;

	//to be move to init tank !!!
	str.Format("Tanks\\Tank%02ld" , m_lTankNumber);
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "MinDelivery", (DWORD*)&lMinDeliveryVolume, (DWORD)lMinDeliveryVolume, FALSE);

	lRetVolume = m_cDeliveryEmulationInfo.CalculateDeliveryEmulation();

		
	//if ((lRetVolume < (lMinDeliveryVolume * 1000)) && (lRetVolume >= 0)) //4.0.19.160
	if (abs(lRetVolume) < (lMinDeliveryVolume * 1000)) //4.0.19.281 - TD 65586
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{		
			CString str;
			str.Format("Delivery Emulation: Ignoring Delivery Report For Tank number %ld. Delivery Volume = %ld Is Smaller Than MinDelivery Parameter = %ld.",
					          m_lTankNumber,
							  lRetVolume,
							  lMinDeliveryVolume*1000);

			_LOGMSG.LogMsg(str);

			lRetVolume = 0;
		}	
	}
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{		
			CString str;
			
			long lEndTemprature = m_cDeliveryEmulationInfo.GetEndDeliveryEmulationTankTemperature();

			_LOGMSG.LogMsg("*************************");
			str.Format("Delivery Emulation: Found Delivery for Tank=%ld, DeliveryVolume=%ld ,EndTemprature=%ld", m_lTankNumber, lRetVolume, lEndTemprature);
			_LOGMSG.LogMsg(str);
			_LOGMSG.LogMsg("*************************");
		}	
	}

	//4.0.19.281 - TD 65586
	//if (lRetVolume < 0) //4.0.19.160
	//{
	//	lRetVolume = 0;
	//}
	return	lRetVolume;
}

/******************************************************************************
 Description:	Load Delivery Emulation Info from ini file
 Returns:      	void
 Parameters:    Tank number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			29/05/2008   11:31		Start	//4.0.20.52 (62818)
 Shelly         12/08/2008   15:18      Add a support for DeliveryReference //4.0.20.501 - TD 65174 
 Shelly         12/10/2008   12:18      Add a support for more fields       //4.0.21.500 TD 66935 
******************************************************************************/	
void CDeliveryEmulationInfo::LoadDeliveryEmulationInfo(long lTankNumber)
{
	m_szTankINIPath.Format("Tanks\\Tank%02ld" , lTankNumber);
	
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryVolume", (DWORD*)&m_lStartDeliveryEmulationVolume, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryVolume", (DWORD*)&m_lEndDeliveryEmulationVolume, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryTemperature", (DWORD*)&m_lStartDeliveryEmulationTankTemperature, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryTemperature", (DWORD*)&m_lEndDeliveryEmulationTankTemperature, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

	//4.0.21.500 TD 66935

	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryWaterVolume", (DWORD*)&m_lStartDeliveryEmulationWaterVolume, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryWaterVolume", (DWORD*)&m_lEndDeliveryEmulationWaterVolume, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "AccumulatedDeliverySales", (DWORD*)&m_lAccumulatedDeliveryEmulationSales, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	
	 //4.0.20.501 (merge from 4.0.19.300) - TD 65174
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "DeliveryReference", (DWORD*)&m_lDeliveryReference, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

	//4.0.21.500 TD 66935
    _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "FoundDelivery", (DWORD*)&m_bFoundDelivery, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

	//4.0.21.500 TD 66935
    _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "LockConnectedPumps", (DWORD*)&m_bLockConnectedPumps, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

	//4.0.21.500
	SZ_TAG_SYSTEMTIME	szTagTime;		
	memset(&szTagTime, 0, sizeof(SZ_TAG_SYSTEMTIME));

	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME), (char *)&szTagTime, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	Tag2SystemTime(&szTagTime.sTime, &m_cStartDeliveryEmulationTime);

	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME), (char *)&szTagTime, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	Tag2SystemTime(&szTagTime.sTime, &m_cEndDeliveryEmulationTime);

	//4.0.29.501
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryDensity", (DWORD*)&m_lStartDeliveryEmulationDensity, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryDensity", (DWORD*)&m_lEndDeliveryEmulationDensity, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryWeight", (DWORD*)&m_lStartDeliveryEmulationWeight, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryWeight", (DWORD*)&m_lEndDeliveryEmulationWeight, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);

	// 4.0.25.1060  TD 377336
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryProductLevel", (DWORD*)&m_lStartDeliveryEmulationProductLevel, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryProductLevel", (DWORD*)&m_lEndDeliveryEmulationProductLevel, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryWaterLevel", (DWORD*)&m_lStartDeliveryEmulationWaterLevel, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryWaterLevel", (DWORD*)&m_lEndDeliveryEmulationWaterLevel, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
}

/******************************************************************************
 Description: Checks if reading TimeOut is passed	
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			27/10/2008   12:31		Start	  //4.0.21.500 TD G
 Shelly         06/11/2008   14:00      Use General Common function
******************************************************************************/	
BOOL CDeliveryEmulationInfo::IsTimeOutBeforeReading()
{
	long lEmulationDeliveryTimeOutMs = (long)(_Module.m_server.GetEmulationDeliveryTimeOut() * MILLISECONDS_IN_A_SECOND);

	return _Module.m_server.m_cCommonFunction.IsTimeOutPassed(m_tEndLockTime, lEmulationDeliveryTimeOutMs);
}

/******************************************************************************
 Description: Checks if reading TimeOut is passed	
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		06/11/2008   13:50		Start 4.0.21.500 TD 66935 
******************************************************************************/	
BOOL CDeliveryEmulationInfo::IsTimeOutForLocking(const long lTankNumber)
{
	BOOL bRetVal = FALSE;
	long lPumpLockTimeOut = (long)(_Module.m_server.GetPumpLockTimeOut() * MILLISECONDS_IN_A_SECOND);

	if (lPumpLockTimeOut)
	{
		bRetVal = _Module.m_server.m_cCommonFunction.IsTimeOutPassed(m_tWaitForLockTime, lPumpLockTimeOut);
	    if (bRetVal)
		{
			CString str;

			str.Format("Delivery Emulation Single Tank: Locking TimeOut for Tank=%ld had been passed", lTankNumber);
			_LOGMSG.LogMsg(str);

			//Inner indication:
			_Module.m_server.m_cServerInfo.SetDeliveryEmulationSingleTankProcessStopped(lTankNumber, SINGLE_TANK_PROCESS_STOPPED_BY_TIMEOUT);
			//Indication for BOS:
			_Module.m_server.m_cServerInfo.SetDeliveryEmulationSingleTankSuccess(lTankNumber, DESF_SINGLE_TANK_TIMEOUT);
		}
	}
	
	return bRetVal;
}

/******************************************************************************
 Description: Checks if reading TimeOut is passed	
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		06/11/2008   13:50		Start 4.0.21.500 TD 66935 
******************************************************************************/	
BOOL CDeliveryEmulationInfo::IsTimeOutForReading(const long lTankNumber)
{
	BOOL bRetVal = FALSE;
	long lWetStockClearTime = _Module.m_server.GetWetStockClearTime();

    bRetVal = _Module.m_server.m_cCommonFunction.IsTimeOutPassed(m_tWaitForReadTime, lWetStockClearTime);
	if (bRetVal)
	{
		CString str;

		str.Format("Delivery Emulation Single Tank: Reading TimeOut for Tank=%ld had been passed", lTankNumber);
		_LOGMSG.LogMsg(str);

		//Inner indication:
		_Module.m_server.m_cServerInfo.SetDeliveryEmulationSingleTankProcessStopped(lTankNumber, SINGLE_TANK_PROCESS_STOPPED_BY_TIMEOUT);
		//Indication for BOS:
		_Module.m_server.m_cServerInfo.SetDeliveryEmulationSingleTankSuccess(lTankNumber, DESF_SINGLE_TANK_TIMEOUT);
	}

	return bRetVal;
}


/******************************************************************************
 Description: Update the flag whether increase 
			  delivery emulation single tank sales can be done
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella		16/06/2011   12:00			Start 4.0.19.1260 - TD 242013
******************************************************************************/	
void CDeliveryEmulationInfo::UpdateCanIncDeliveryEmulationSales(const long lTankNumber)
{
	if (((_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessType(lTankNumber) == PD_SINGLE_TANK_START) &&
		(_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankStatus(lTankNumber) >= DES_SINGLE_TANK_WAIT_TIMEOUT)) ||
		((_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankProcessType(lTankNumber) == PD_SINGLE_TANK_END) &&
		(_Module.m_server.m_cServerInfo.GetDeliveryEmulationSingleTankStatus(lTankNumber) < DES_SINGLE_TANK_WAIT_TIMEOUT)))

			m_bCanIncDeliveryEmulationSales = TRUE;

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		CString strLog;
		strLog.Format("Delivery Emulation Single Tank:	CanIncDeliveryEmulationSales = %d", m_bCanIncDeliveryEmulationSales);
		_LOGMSG.LogMsg(strLog);
	}
}/******************************************************************************
 Description:	Save Start Delivery Emulation Density to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AlexM			22/12/2011   11:31		Start	//4.0.25.380 TD 264908
******************************************************************************/
void CDeliveryEmulationInfo::SaveStartDeliveryEmulationDensity( long lTankNumber )
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryDensity", (DWORD)m_lStartDeliveryEmulationDensity, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);	
}


/******************************************************************************
 Description:	Save End Delivery Emulation Density to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AlexM			22/12/2011   11:31		Start	//4.0.25.380 TD 264908
******************************************************************************/
void CDeliveryEmulationInfo::SaveEndDeliveryEmulationDensity( long lTankNumber )
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryDensity", (DWORD)m_lEndDeliveryEmulationDensity, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);		
}

/******************************************************************************
 Description:	Save Start Delivery Emulation Weight to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AlexM			22/12/2011   11:31		Start	//4.0.25.380 TD 264908
******************************************************************************/
void CDeliveryEmulationInfo::SaveStartDeliveryEmulationWeight( long lTankNumber )
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryWeight", (DWORD)m_lStartDeliveryEmulationWeight, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);			
}


/******************************************************************************
 Description:	Save End Delivery Emulation Weight to ini file	
 Returns:      	void
 Parameters:    Tank Number.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 AlexM			22/12/2011   11:31		Start	//4.0.25.380 TD 264908
******************************************************************************/
void CDeliveryEmulationInfo::SaveEndDeliveryEmulationWeight( long lTankNumber )
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryWeight", (DWORD)m_lEndDeliveryEmulationWeight, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);				
}

// 4.0.25.1060  TD 377336
void CDeliveryEmulationInfo::SaveStartDeliveryEmulationProductLevel( long lTankNumber )
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryProductLevel", (DWORD)m_lStartDeliveryEmulationProductLevel, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);				
}


void CDeliveryEmulationInfo::SaveEndDeliveryEmulationProductLevel( long lTankNumber )
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryProductLevel", (DWORD)m_lEndDeliveryEmulationProductLevel, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);				
}


void CDeliveryEmulationInfo::SaveStartDeliveryEmulationWaterLevel( long lTankNumber )
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "StartDeliveryWaterLevel", (DWORD)m_lStartDeliveryEmulationWaterLevel, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);				
}


void CDeliveryEmulationInfo::SaveEndDeliveryEmulationWaterLevel( long lTankNumber )
{
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)m_szTankINIPath), "EndDeliveryWaterLevel", (DWORD)m_lEndDeliveryEmulationWaterLevel, TRUE, _HOST.m_lGroupNum, 0, lTankNumber, TRUE);				
}


BOOL CTank::IsTankBlockedByUser()
{
	return (GetTankState() == TANK_STATE_BLOCKED_BY_USER);
}


CString CTank::GeTankStateDesc()
{

	CString str;

	switch (m_eTankState)
	{
	case TANK_STATE_READY:
		str.Format("READY");
		break;
	case TANK_STATE_BLOCKED_BY_USER:
		str.Format("BLOCKED_BY_USER");
		break;
	default:
		str.Format("UnNone State");
		break;
	}

	return str;
}
