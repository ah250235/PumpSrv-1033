#include "stdafx.h"
#include "CStrFormat.h"
#include "ApplyCalcDiscountNMPDetail.h"

// RFUEL-2817
ApplyCalcDiscountNMPDetail::ApplyCalcDiscountNMPDetail() :
	CNMPDetails()
{
	m_eNMPType = NMP_TYPE_ApplyCalculateDiscount;

	SetStateType(OLA_LOYALTY_APPLY_PAYMENT_DISCOUNT);
}



//  At this State, the user 
//  1. Cancelled transaction
//  2. Swiped Loyalty Card, followed by Payment card OR
//  3. Skip the discount card and swiped the Payment card.  
//  The Authorize state should've eliminated invalid card swiped or all varients involved in wrong card.
void ApplyCalcDiscountNMPDetail::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	char LogBuffer[512] = {};

	CFlowControlManager& FlowCtrlMan = _Module.m_server.GetFlowControlManager();

	if ((_Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber) == CancelBeforeStartTicket) ||
		(FlowCtrlMan.GetPumpSrvAddMemberCardRes(lPumpNumber) > 0))
	{
		MemoryStream LogMsg(LogBuffer, sizeof(LogBuffer));
		LogMsg << __FUNCTION__ << " - skip Authorize Loyalty card NMP, "
			<< "CancelTime = " << _Module.m_server.m_cFuelLoayaltyPS.GetCancelTrsTime(lPumpNumber)
			<< ", PumpSrvAddMemberRes = "
			<< FlowCtrlMan.GetPumpSrvAddMemberCardRes(lPumpNumber)
			<< ends;
		_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, LogBuffer);

		FlowCtrlMan.HandleNMPSessions(lPumpNumber, &ola, &cTmpInfo, TRUE);
	}
	else
	{
		auto& Pump = _Module.m_server.GetPump(lPumpNumber);

		const bool bLoyaltySwiped = _Module.m_server.IsLoyaltycardAuthorized(cTmpInfo);;

		// if loyalty card was swiped, then trigger Get Customer Info to add Member data
		if (bLoyaltySwiped) {
			
			Pump.SetOlaStat(OLA_LOYALTY_APPLY_PAYMENT_DISCOUNT | SESSION_GET_CUSTOMER_INFO);					
			Pump.SetPAPInfo(&cTmpInfo);
			
			MemoryStream LogMsg(LogBuffer, sizeof(LogBuffer));
			LogMsg << __FUNCTION__
				<< " - Requested Authorize loyalty card NMP"
				<< ends;
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, LogBuffer);
		}
		else {
			// Loyalty card Not scanned and likely this is the final stage of Finite State Machine
			// start Fueling with Payment card only (assuming that when we get to this point, payment card is Authorized)		
			Pump.SetOlaStat(OLA_LOYALTY_RUN_QUERY | SESSION_AUTHORIZE);
			Pump.SetPAPInfo(&cTmpInfo);

			MemoryStream LogMsg(LogBuffer, sizeof(LogBuffer));
			LogMsg << __FUNCTION__
				<< " - RUN_QUERY"
				<< ends;
			_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, LogBuffer);
		}
	}
}
