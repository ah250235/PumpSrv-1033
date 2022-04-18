// ShiftTanks.cpp: implementation of the CShiftTanks class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShiftTanks.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShiftTanks::CShiftTanks() : CRelativeDoubleRecord(SHIFT_TANKS_FILE)
{
	InitializeCriticalSection(&m_csShiftTanks);
	memset((char*)&m_cRec , 0 , sizeof(SHIFT_TANKS_REC));
}

CShiftTanks::~CShiftTanks()
{
	DeleteCriticalSection(&m_csShiftTanks);
}

void CShiftTanks::ConvertTankInfoToRecord(CTank &cTank)
{
	m_cRec.m_lFlags         = cTank.m_lFlags;
	m_cRec.m_lFuelLevel     = cTank.m_lFuelLavel;
	m_cRec.m_lFuelVolume    = cTank.m_lFuelVolume;
	m_cRec.m_lManifold      = cTank.m_lManifold;
	m_cRec.m_lProduct       = cTank.m_lProduct;
	m_cRec.m_lShiftNumber   = cTank.m_lShiftNumber;
	m_cRec.m_lStatusGeneral = cTank.m_lStatusGeneral;
	m_cRec.m_lStatusSpecial = cTank.m_lStatusSpecial;
	m_cRec.m_lTankCapacity  = cTank.m_lTankCapacity;
	m_cRec.m_lTankNumber    = cTank.m_lTankNumber;
	m_cRec.m_lTCVolume      = cTank.m_lTCVolume;
	m_cRec.m_lTempertaure   = cTank.m_lTemperature;
	m_cRec.m_lUllage        = cTank.m_lUllage;
	m_cRec.m_lWaterLevel    = cTank.m_lWaterLavel;
	m_cRec.m_lWaterVolume   = cTank.m_lWaterVolume;
	m_cRec.m_nNACSCode      = cTank.m_nNACSCode;
	
	memcpy((char*)m_cRec.m_sProductName , (LPCSTR)cTank.m_cProductName , sizeof(m_cRec.m_sProductName));

	cTank.m_cTime.GetAsSystemTime(m_cRec.m_sTime);

	m_cRec.byValidRecord = (BYTE)1;

	return;
}


void CShiftTanks::ConvertTankInfoToRecord(TANK_READ_INFO &cTank)
{
	m_cRec.m_lFlags         = (long)cTank.nFlags;
	m_cRec.m_lFuelLevel     = cTank.lFuelLevel;
	m_cRec.m_lFuelVolume    = cTank.lFuelVolume;
	m_cRec.m_lProduct       = cTank.lProduct;
	m_cRec.m_lStatusGeneral = cTank.lStatus;
	m_cRec.m_lStatusSpecial = cTank.lStatus2;
	m_cRec.m_lTankNumber    = cTank.lTank;
	m_cRec.m_lTCVolume      = cTank.lTcVolume;
	m_cRec.m_lTempertaure   = cTank.lTemperature;
	m_cRec.m_lUllage        = cTank.lUllage;
	m_cRec.m_lWaterLevel    = cTank.lWaterLevel;
	m_cRec.m_lWaterVolume   = cTank.lWaterVolume;
	
	//Get shift number
	m_cRec.m_lShiftNumber = _Module.m_server.GetFormalShiftNumber();
	
	//Get product name from registry
	CString str("PureProducts\\PureProduct");
	char sNumber[8];
	memset(sNumber,0,sizeof(sNumber));
	sprintf(sNumber,"%02d",m_cRec.m_lProduct);
	str += sNumber;
		
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Name", (char *)m_cRec.m_sProductName , sizeof(m_cRec.m_sProductName),"",TRUE);
	
	//Get product NACS code
	//First, we get grade consisting of only this pure product
	DWORD dwTmp;
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Code", &dwTmp ,0,FALSE , FALSE);

	//If such a grade exists, we get its NACS code
	if(dwTmp)
	{
		str = "Grades\\Grade";
		memset(sNumber,0,sizeof(sNumber));

		sprintf(sNumber,"%02d",dwTmp);
		str += sNumber;

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str) , "Code" , &dwTmp , 0 , FALSE , FALSE);

		m_cRec.m_nNACSCode = (short)dwTmp;
	}

	//getting manifold from registry
	str = "Tanks\\Tank";
	memset(sNumber,0,sizeof(sNumber));

	sprintf(sNumber,"%02d",m_cRec.m_lTankNumber);
	str += sNumber;
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "LogicTanks",&dwTmp,(DWORD)0,TRUE);
	
	//getting tank capacity from registry
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "FullHeightVolume",&dwTmp,(DWORD)0,TRUE);
	
	GetSystemTime(&m_cRec.m_sTime);

	m_cRec.byValidRecord = (BYTE)1;
	
	return;
}

long CShiftTanks::ConvertRecordToTankInfo(CTank &cTank)
{
	long lRtc = OK;
	CString sTmpSection , sNum;

	sTmpSection.Format("Tanks\\Tank");
	sNum.Format("%02d" , m_cRec.m_lTankNumber);
	sTmpSection += sNum;

	char sSection[64];

	strcpy((char*)sSection , (LPCSTR)sTmpSection);

	//Load validity from registry, but do not write this to log.
	_Module.m_server.m_cParam.LoadParam(sSection , "ValidEntry" , (DWORD*)(&cTank.m_lTankValid) , 0 , FALSE , FALSE);

	//If nonsense info has been read from the q-dex, the following call
	//to CTime's c'tor will fail on an ASSERT call, causing PumpSrv to
	//crash.  In order to avoid this, we check tank validity
	//to be sure info is correct.
	if(cTank.m_lTankValid && (m_cRec.byValidRecord == 1))
	{
		CTime cRecTime(m_cRec.m_sTime);
		cTank.m_cTime = cRecTime;
	}

	//If the tank is invalid, tank info may include nonsense info, so
	//we erase it all, just to be in the safe side.
	else
	{
		memset(&m_cRec , 0 , sizeof(m_cRec));
		lRtc = ERR_NOT_FOUND;
	}

	cTank.m_lFlags         = m_cRec.m_lFlags;
	cTank.m_lFuelLavel     = m_cRec.m_lFuelLevel;
	cTank.m_lFuelVolume    = m_cRec.m_lFuelVolume;
	cTank.m_lManifold      = m_cRec.m_lManifold;
	cTank.m_lProduct       = m_cRec.m_lProduct;
	cTank.m_lShiftNumber   = m_cRec.m_lShiftNumber;
	cTank.m_lStatusGeneral = m_cRec.m_lStatusGeneral;
	cTank.m_lStatusSpecial = m_cRec.m_lStatusSpecial;
	cTank.m_lTankCapacity  = m_cRec.m_lTankCapacity;
	cTank.m_lTankNumber    = m_cRec.m_lTankNumber;
	cTank.m_lTCVolume      = m_cRec.m_lTCVolume;
	cTank.m_lTemperature   = m_cRec.m_lTempertaure;
	cTank.m_lUllage        = m_cRec.m_lUllage;
	cTank.m_lWaterLavel    = m_cRec.m_lWaterLevel;
	cTank.m_lWaterVolume   = m_cRec.m_lWaterVolume;
	cTank.m_nNACSCode      = m_cRec.m_nNACSCode;

	cTank.m_cProductName = (LPCSTR)m_cRec.m_sProductName;

	return lRtc;
}

void CShiftTanks::SetNACSCode(short nCode)
{
	m_cRec.m_nNACSCode = nCode;

	return;
}

short CShiftTanks::GetNACSCode()
{
	return m_cRec.m_nNACSCode;
}

long CShiftTanks::Write(CTank &cTank, short nNACSCode)
{
	long lRtc = OK;

	long lTankNumber = cTank.m_lTankNumber;
	long lProductNumber = cTank.m_lProduct;

/* 4.0.1.38
	long lFlags = HOP_DIRECT_LOCAL;

	if(_HOST.m_lHydraActive)
		lFlags = HOP_DEFAULT_CONFIG;
*/

	ConvertTankInfoToRecord(cTank);
	SetNACSCode(nNACSCode);

	lRtc = CRelativeDoubleRecord::Write(offsetof(SHIFT_TANKS_FILE_STRUCT , cRec[lTankNumber - 1]),
										sizeof(SHIFT_TANKS_REC),
										&m_cRec,
										HOP_DEFAULT_CONFIG,   //4.0.1.38
										0,
										0);
	
	return lRtc;
}

long CShiftTanks::Write(TANK_READ_INFO &cTank)
{
	long lRtc = OK;

	long lTankNumber = cTank.lTank;
	long lProductNumber = cTank.lProduct;
/* //4.0.1.38

	long lFlags = HOP_DIRECT_LOCAL;

	if(_HOST.m_lHydraActive)
		lFlags = HOP_DEFAULT_CONFIG;
*/

	ConvertTankInfoToRecord(cTank);
	
	lRtc = CRelativeDoubleRecord::Write(offsetof(SHIFT_TANKS_FILE_STRUCT , cRec[lTankNumber - 1]),
										sizeof(SHIFT_TANKS_REC),
										&m_cRec,
										HOP_DEFAULT_CONFIG,
										0,
										0);
	
	return lRtc;
}

long CShiftTanks::Read(long lTankNumber , CTank &cTank , short *pnNACSCode , long lFlags)
{
	long lRtc = 0;
	
	WORD iBytesToRead = sizeof(SHIFT_TANKS_REC);
	DWORD dwOffset = offsetof(SHIFT_TANKS_FILE_STRUCT , cRec[lTankNumber - 1]);

	//Each record is composed of two identical halves.  The current shift is stored
	//in the second half.
	if(lFlags & GET_CURRENT_SHIFT)
		dwOffset += iBytesToRead;

/*   //4.0.1.38
	long lHydFlags = HOP_DIRECT_LOCAL;

	if(_HOST.m_lHydraActive)
		lHydFlags = HOP_DEFAULT_CONFIG;
*/

	CRelativeDoubleRecord::Read(dwOffset,
					            iBytesToRead,
					            &m_cRec,
					            HOP_DIRECT_LOCAL,   // 4.0.1.38
					            0,
					            0);
	
	lRtc = ConvertRecordToTankInfo(cTank);
	
	return lRtc;
}
