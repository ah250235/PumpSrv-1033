#include "stdafx.h"
#include "CarWashSaleNMPDetalis.h"


CCarWashSaleNMPDetails::CCarWashSaleNMPDetails(void)
{
	m_nOlaState = OLA_LOYALTY_CARWASH_SALE;
}


CCarWashSaleNMPDetails::~CCarWashSaleNMPDetails(void)
{
}

void CCarWashSaleNMPDetails::Execute( PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola )
{
	// set language id
	_Module.m_server.m_cFuelLoayaltyPS.SetParam(lPumpNumber);

	OpenNeedMoreParam(cTmpInfo ,lPumpNumber,m_sMessage,NMP_MAX_NUM_OF_OPEN_KEYS,NMP_DEFAULT_TIMEOUT, ONMP_OpenYesNoKeys | ONMP_YesAsEnter,PT_CAR_WASH);
}