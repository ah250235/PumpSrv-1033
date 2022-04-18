#pragma once

#include "OLAEpsilon.h"
#include "OLASimLinkSrv.h"
#include "CStrFormat.h"

class OLAPumpServiceMode;

// set 1 for testing, 0 to release
#define USE_SIMLINK 0


// in simulation mode, object derive from COLASimLinkSrv
// in release it should derive from COLAEpsilon
#if USE_SIMLINK
	class COLAEpsilonDiscount : public COLASimLinkSrv
	{
		// Create the base class type to isolate the parent class change to one location	
		typedef COLASimLinkSrv base_class;
#else
	class COLAEpsilonDiscount : public COLAEpsilon
	{
		// Create the base class type to isolate the parent class change to one location	
		typedef COLAEpsilon base_class;
#endif

public:
	COLAEpsilonDiscount() : base_class() {
		;
	}

	~COLAEpsilonDiscount() {
		;
	}

	virtual void Init() override;
	virtual long GetOLAType() override { return OLASRV_EPSILON_DISCOUNT; }
	virtual void BuildGetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, char *pOutBuffer) override;
	virtual void BuildSetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, 
		CPumpTransact *pTrs, char *pOutBuffer) override;	

	void PreSetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, 
		CPumpTransact *pTrs, char *pOutBuffer);
	void PostSetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, 
		CPumpTransact *pTrs, char *pOutBuffer);

	// This is NOT the virtual method defined by the parent class
	void AddPreAuthInformation(long lPumpNumber, PAY_AT_PUMP_INFO *pInfo, 
		long &lOffset, char *pOutBuffer, 
		const OLAPumpServiceMode& PumpServiceMode, long & lGradeRestrict)
	{
		// set this to false when not debugging, optimizer should take out the code when set to false
		const bool bDebug = LOG_ALL_FLOW_CONTROL;

		//bool bIsLoyaltyCard = IsLoyaltyCard(lPumpNumber);
		const bool bIsLoyaltyCard = IsLoyaltyCard(*pInfo);
		// if calling to add discount
		if (bIsLoyaltyCard) {

			const auto bDiscountApplying = IsDiscountApplying(lPumpNumber);
			if (bDiscountApplying) {
				if (bDebug) _LOGMSG.LogMsg("AddPreAuthInformation DEBUG FLOW - LOYALTY CARD DISCOUNT BLOCK", LOG_DETAIL_FLOW_CONTROL);
				AddedPreAuthInformation_(lPumpNumber, pInfo, lOffset, pOutBuffer, PumpServiceMode, lGradeRestrict);
			}
			else {
				if (bDebug) _LOGMSG.LogMsg("AddPreAuthInformation DEBUG FLOW NO LOYALTY CARD BLOCK1", LOG_DETAIL_FLOW_CONTROL);
			}
		}
		else {
			if (bDebug) _LOGMSG.LogMsg("AddPreAuthInformation DEBUG FLOW NO LOYALTY CARD BLOCK2", LOG_DETAIL_FLOW_CONTROL);
		}
	}

private:
	bool IsLoyaltyCard(const PAY_AT_PUMP_INFO& Info) const {
		const bool bIsLoyaltyCard = _Module.m_server.IsLoyaltycardAuthorized(Info);
		return bIsLoyaltyCard;
	}

	bool IsDiscountApplying(const int PumpNumber) const {
		const auto bDiscountApplying = _Module.m_server.IsLoyaltyDiscountApplied(PumpNumber);
		return bDiscountApplying;
	}

	void AddedPreAuthInformation_(long lPumpNumber, PAY_AT_PUMP_INFO *pInfo, long &lOffset, char *pOutBuffer, const OLAPumpServiceMode& PumpServiceMode, long & lGradeRestrict);
	void AddedPreAuthProcessFuelItem(
		long lPumpNumber, PAY_AT_PUMP_INFO *pInfo, long &lOffset, char *pOutBuffer, const OLAPumpServiceMode& PumpServiceMode, const long & lGradeRestrict,
		const long& lGrade, const ItemSoldInfo& SoldInfo, long& lNumOfItems);

	// Gets price using Service Mode, Price Level	
	long GetGradePrice(const ItemSoldInfo& SoldInfo, 
		const OLAPumpServiceMode& oServiceMode, long nPumpPriceLevel) const;
	
	// Gets price using grade, service Mode, Price Level	
	long GetGradePrice(const PumpItems& cPumpItems, long nGrade, 
		const OLAPumpServiceMode& oServiceMode, long nPumpPriceLevel) const;


	long GetGradePrice(short nGrade, 
		long& lPriceCash, long& lPriceCredit,
		const OLAPumpServiceMode& oPumpServiceMode) const;
	
	// Added for debugging
	void DebugDumpDiscountMatrix(const PAY_AT_PUMP_INFO& Info, ostream& os) {
		os << endl;
		for (int i = 0; i < MAX_GRADES; i++) {
			constexpr size_t DiscountSize = sizeof(Info.CardSaleExtraData2.sDiscountPerGrade[0].sDiscount);
			long Discount =  a2l(Info.CardSaleExtraData2.sDiscountPerGrade[i].sDiscount, DiscountSize);
			os << "DISCOUNT[" << i << "]=" << Discount << endl;
		}
	}
};

