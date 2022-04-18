#include "stdafx.h"
#include "FuelingNMPDetails.h"



CFuelingNMPDetails::CFuelingNMPDetails()
{
	m_eNMPType = NMP_TYPE_Fueling;
	SetStateType(OLA_LOYALTY_START_FUELING);
}

//4.0.22.504 69344
void CFuelingNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{//4.0.23.860 212541

	CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[lPumpNumber-1].GetStat();
	long p = lPumpNumber - 1;
	BOOL bCheckDeferral = TRUE;


	if (( cPumpStatus.GetRes() == PAY_AT_KIOSK ) || (cPumpStatus.GetResExt() == RES_EXT_PAK_LOYALTY) || (_Module.m_server.m_cPumpArray[lPumpNumber-1].IsPapToPakTrs()))	//4.0.22.504 PAK	69344 //4.0.23.860 212541
	{
		if (_Module.m_server.m_cModeArray[cPumpStatus.GetMode() - 1].PAKAutoAuthLoyaltyOnly() && (cTmpInfo.CardSaleExtraData5.sPumpAutoStart != '1') && _Module.m_server.m_cFuelLoayaltyPS.m_dwLoyaltyEngineType != EMERALD) ///*PumpStartOnly and seg wasn't received*/ 4.0.23.1210
		{
			CString sMsg;
			sMsg.Format("Pump Start segment wasn't received- transaction is canceled, sPumpAutoStart = %c", cTmpInfo.CardSaleExtraData5.sPumpAutoStart);
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);	
			_Module.m_server.m_cFuelLoayaltyPS.CancelTicket(lPumpNumber);
			_Module.m_server.m_cFuelLoayaltyPS.CancelLoyaltyTrs(cTmpInfo,lPumpNumber, ola);
		}
		else
			_Module.m_server.m_cFuelLoayaltyPS.PAKResult(cTmpInfo, lPumpNumber, ola);	
	}
	else
	{
		_Module.m_server.m_cFuelLoayaltyPS.SimulStartFuelingSession(lPumpNumber, ola, (CARD_SALE_ALL3 *)&cTmpInfo.CardSaleInfo.cardData);
	}

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Execute Requested Fueling NMP");
}
//4.0.22.500 end
