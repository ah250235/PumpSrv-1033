#include "stdafx.h"
#include <ios> 
#include <iostream>
#include <iomanip>

#include "NMPFactory.h"
#include "CStrFormat.h"

#include "phoneLookUpNmpDetalis.h"
#include "DeferralDiscountNMPDetails.h"
#include "CopientCommandExecuter.h"
#include "AuthorizeLoyaltyNMPDetails.h"
#include "AuthorizePaymentNMPDetails.h"
#include "SwipePaymentNMPDetails.h"
#include "RunQueryNMPDetails.h"
#include "FuelingNMPDetails.h"
#include "MethodOfPaymentNMPDetails.h"
#include "ManualLoyaltyNMPDetails.h"
#include "LoyaltyNMPDetails.h"
#include "PromptNMPDetails.h"
#include "TargetMessageNMPDetails.h"
#include "CarWashDiscountNMPDetails.h"
#include "ApplyCalcDiscountNMPDetail.h"

// Instance of the object
CNMPFactory CNMPFactory::inst_;


// Object construction table
// NMPType, Entry Name, OLA State number (0), a lamda function to create the new detail object
CNMPFactory::NMPLookup_Entry const CNMPFactory::build_Lookup_[] = {
	{ NMP_TYPE_CarWash, NMP_CAR_WASH_ENTRY_NAME, 0, [](void)->CNMPDetails* { return new CCarWashNMPDetails; } },
	{ NMP_TYPE_Coupon, NMP_COUPON_ENTRY_NAME, 0,[](void)->CNMPDetails* { return new CCouponsNMPDetails; } },
	{ NMP_TYPE_LoyaltyCard, NMP_LOYALTY_ENTRY_NAME,0, [](void)->CNMPDetails* { return new CLoyaltyNMPDetails; } },
	{ NMP_TYPE_MessagePrompt, NMP_PROMPT_ENTRY_NAME, OLA_LOYALTY_PROMPT_BEFORE_FUELING_BEFORE_START_TICKET,[](void)->CNMPDetails* { return new CPromptNMPDetails; } },
	{ NMP_TYPE_DeferralDiscount, NMP_DEFERRAL_ENTRY_NAME, 0,[](void)->CNMPDetails* { return new CDeferralDiscountNMPDetails; } },
	{ NMP_TYPE_PaymentCard, NMP_PAYMENT_ENTRY_NAME, 0,[](void)->CNMPDetails* { return new CSwipePaymentNMPDetails; } },
	{ NMP_TYPE_AuthorizePayment, NMP_AUTHORIZE_PAYMENT_ENTRY_NAME,0, [](void)->CNMPDetails* { return new CAuthorizePaymentNMPDetails; } },
	{ NMP_TYPE_AuthorizeLoyalty, NMP_AUTHORIZE_LOYALTY_ENTRY_NAME,0, [](void)->CNMPDetails* { return new CAuthorizeLoyaltyNMPDetails; } },
	{ NMP_TYPE_Fueling, NMP_FUELING_ENTRY_NAME, 0,[](void)->CNMPDetails* { return new CFuelingNMPDetails; } },
	{ NMP_TYPE_MethodOfPayment, NMP_METHOD_OF_PAYMENT_ENTRY_NAME,0, [](void)->CNMPDetails* { return new CMethodOfPaymentNMPDetails; } },
	{ NMP_TYPE_RunQuery, "", 0, [](void)->CNMPDetails* { return new CRunQueryNMPDetails; } },
	{ NMP_TYPE_PhoneLookUp, NMP_PHONE_LOOKUP_ENTRY_NAME,0, [](void)->CNMPDetails* { return new CPhoneLookUpNMPDetails; } },
	{ NMP_TYPE_CarWashDiscount, NMP_CAR_WASH_DISCOUNT_ENTRY_NAME, OLA_LOYALTY_CAR_WASH_DISCOUNT, [](void)->CNMPDetails* { return new CCarWashDiscountNMPDetails; } },
	{ NMP_TYPE_TargetMessage, NMP_TARGET_MESSAGE_ENTRY_NAME, OLA_LOYALTY_TARGET_MESSAGE,[](void)->CNMPDetails* { return new CTargetMessageNMPDetails; } },
	{ NMP_TYPE_ApplyCalculateDiscount, NMP_APPLY_CALCULATE_DISCOUNT_ENTRY_NAME,0, [](void)->CNMPDetails* { return new ApplyCalcDiscountNMPDetail; } },
};

//
// Create
// Search Lookup table by the NMPType
// When found, use its lamda function to create the proper detail object
// Then set required settings
//
//  This O^n operation may seems slow, but in reality
//  we have small number of items and likely executing all in L1 cache
//  the sequential array will execute much faster than hash
CNMPDetails* CNMPFactory::Create(NMP_TYPE NMPType, char *sInPath)
{
	// Hitting this assertion means new NMP Detail was added, but did not create
	//   the proper entry in build_Lookup_ table
	constexpr int table_size = _countof(build_Lookup_);
	constexpr int max_size = NMP_MAX_TYPE_OPTIONS;
	static_assert(table_size == max_size, "NMP_MAX_TYPE_OPTIONS does not match build_Lookup_ lookup table");

	char sPath[MAX_PATH] = { 0 };

	CNMPDetails* pDetailItem = nullptr;

	// Search array for match
	for (const NMPLookup_Entry& item : build_Lookup_)
	{
		if (item.NMPType == NMPType) {
			// allocate a new Detail through the lamda function
			pDetailItem = item.func_ptr_();

			// if successful creation of the object
			if (pDetailItem != nullptr) {
				// set proper NMPType code
				pDetailItem->SetNMPType((enum NMP_TYPE)NMPType);

				// if PATH specified, then merge the path
				if (item.PATH[0] != 0) {
					// InPath\PATH generation
					memcpy(sPath, sInPath, min(strlen(sInPath), MAX_PATH));
					strcat_s(sPath, MAX_PATH, "\\");
					strcat_s(sPath, MAX_PATH, item.PATH);

					pDetailItem->SetPath(sPath);
				}
				else {
					// add blank Path
					pDetailItem->SetPath(sPath);
				}

				// Set OLAState if specificed
				if (item.OLAState != 0) {
					pDetailItem->SetStateType(item.OLAState);
				}
			}

			// break out out of the loop
			break;

		} //if (item.NMPType == NMPType)
	} // for


	if (pDetailItem == nullptr) {
		LogStream LogMsg;
		LogMsg << __FUNCTION__
			<< " Failed to create a new NMP object."
			<< " NMPType=" << NMPType
			<< ends;
		
		// This could be fatal, prints at any log level
		_LOGMSG.LogMsg(LogMsg, LOG_LEVEL_1);
	}
	else {
		// debug dump shows up at log level > 5
		if (LOG_ALL_FLOW_CONTROL) {
			pDetailItem->DebugDump();
		}
	}


	return pDetailItem;
}

