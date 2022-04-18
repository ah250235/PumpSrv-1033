// Promotion.h: interface for the CPromotion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROMOTION_H__35970DFF_EB38_489D_8208_160FEDF5DD84__INCLUDED_)
#define AFX_PROMOTION_H__35970DFF_EB38_489D_8208_160FEDF5DD84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPromotion  
{
public:
	CPromotion();	
	CPromotion(long lPromID, double fCenstOff, double fRewardAmountDeferredReward, double fDeferrableRewardAmount, double fOriginalRewardField, BOOL bAskDeferral, BOOL bManualDeferral, long lPluCode, CString sPromDescription);
	virtual ~CPromotion();

	double GetOriginalRewardField() const { return m_fOriginalRewardField; }
	double GetCentsOff() const { return m_fCentsOff; }
	double GetRewardAmountDeferredReward() const { return m_fRewardAmountDeferredReward; }
	long GetID() const {return m_lPromotionId; }
	BOOL GetAskDeferral() const { return m_bAskDeferral; }
	double GetDeferrableRewardAmount() const { return m_fDeferrableRewardAmount; }
	double GetTotalDiscount() const {return m_fRewardAmountDeferredReward + m_fCentsOff;}
	void   SetRewardAmountDeferredReward(double val) { m_fRewardAmountDeferredReward = val; }
	void UpdateDeferralRewards();

	BOOL IsPromotionIncludeDeferralDiscount();
	BOOL IsManualDeferral() const { return m_bManualDeferral; }

	CString GetsPromDescription() const { return m_sPromDescription; }
	long	GetPluCode() const { return m_lPluCode; }
	void	SetPluCode(long lPluCode)  {  m_lPluCode = lPluCode; }

private:
	long	m_lPromotionId;//hold the prom id	
	double	m_fCentsOff;//hold the censt off
	double	m_fDeferrableRewardAmount; //4.0.20.54 64528
	double	m_fRewardAmountDeferredReward;
	BOOL	m_bAskDeferral;
	double	m_fOriginalRewardField;
	BOOL    m_bManualDeferral;
	long    m_lPluCode;
	CString m_sPromDescription;
};

#endif // !defined(AFX_PROMOTION_H__35970DFF_EB38_489D_8208_160FEDF5DD84__INCLUDED_)
