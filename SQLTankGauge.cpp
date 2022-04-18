#include "stdafx.h"
#include "SQLTankGauge.h"
#include "PumpSrvDatabase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CSQLTankGauge::CSQLTankGauge()
{
}


CSQLTankGauge::~CSQLTankGauge()
{
}

long CSQLTankGauge::Operation(long lOpCode, void *pRec, void *pData)
{
	CPumpSrvDatabase & PumpSrvDataBase = CPumpSrvDatabase::GetInstance();
	long lRetVal = 1;
	
	// Fixed memory leak by using unique_ptr
	//TankGauge	*cTankGauge = new TankGauge();
	std::unique_ptr<TankGauge>	cTankGauge = std::make_unique<TankGauge>();		
	
	if (pData  && pRec)
	{
		cTankGauge->pIndex = (DBTankGaugeRelativeIndex *)pData;
	}

	switch (lOpCode)
	{
	case REC_READ:
		lRetVal = PumpSrvDataBase.LoadTankGaugeRecord(*cTankGauge, cTankGauge->pIndex->lOffSet) ? OK : 1;
		break;
	case REC_UPDATE:
		lRetVal = PumpSrvDataBase.SaveTankGaugeRecord(*cTankGauge, cTankGauge->pIndex->lOffSet) ? OK : 1;
		break;
	}	

	return lRetVal;
}