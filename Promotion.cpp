// Promotion.cpp: implementation of the CPromotion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Promotion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPromotion::CPromotion()
{

}

CPromotion::CPromotion(long lPromID, double fCenstOff, double fRewardAmountDeferredReward, 
						double fDeferrableRewardAmount, double fOriginalRewardField, 
						BOOL bAskDeferral, BOOL bManualDeferral, long lPluCode, 
						CString sPromDescription) : m_lPromotionId(lPromID),
					   m_fCentsOff(fCenstOff),
					   m_fRewardAmountDeferredReward(fRewardAmountDeferredReward),
					   m_fDeferrableRewardAmount(fDeferrableRewardAmount),
					   m_fOriginalRewardField(fOriginalRewardField),
					   m_bAskDeferral(bAskDeferral),
					   m_bManualDeferral(bManualDeferral),
					   m_lPluCode(lPluCode),
					   m_sPromDescription(sPromDescription)

{ 
}





CPromotion::~CPromotion()
{

}

void CPromotion::UpdateDeferralRewards()
{
	if(m_bAskDeferral > 0)
	{
		m_fRewardAmountDeferredReward = m_fCentsOff;
		m_fDeferrableRewardAmount = m_fCentsOff;
		m_fCentsOff = 0;	
	}
}

BOOL CPromotion::IsPromotionIncludeDeferralDiscount()
{
	return m_fRewardAmountDeferredReward > 0  || m_fDeferrableRewardAmount >0;	
}