// POSFuelingRestriction.h: interface for the CPOSFuelingRestriction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSFUELINGRESTRICTION_H__E1C84B92_F071_4C5F_A690_B48B9F72E697__INCLUDED_)
#define AFX_POSFUELINGRESTRICTION_H__E1C84B92_F071_4C5F_A690_B48B9F72E697__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseFuelingRestriction.h"


class CPOSFuelingRestriction : public CBaseFuelingRestriction
{
public:
	CPOSFuelingRestriction();
	virtual ~CPOSFuelingRestriction();

	virtual void	Init(BOOL bCreate);
	
	virtual BOOL	IsApproveAllowed(const LONG lPumpNumber, const  RestrictionType& rRestrictionType);
					
	virtual void	SetRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType);
	virtual void	ClearRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType);

	virtual void	CleanUp();

protected:

	virtual FRRetCode	ApprovedCondition(const LONG lPumpNumber, const RestrictionType& rRestrictionType)const;

	virtual BOOL		IsAllowedSetRestriction(const RestrictionType& rRestrictionType)const;
	virtual BOOL		IsAllowedClearRestriction(const RestrictionType& rRestrictionType)const;

	virtual void		HandleApproveAllowed(const LONG lPumpNumber, const  RestrictionType& rRestrictionType);

	virtual void		SetApprove(const LONG lPumpNumber, const RestrictionType& rRestrictionType);
	virtual void		ClearApprove(const LONG lPumpNumber);

	virtual BOOL		IsAllowedSetApprove(const RestrictionType& rRestrictionType)const;
	virtual BOOL		IsAllowedClearApprove(const LONG lPumpNumber)const;

	virtual void		AddPumpToRestrictedList(const LONG lPumpNumber, const RestrictionType& rRestrictionType);
	virtual void		RemovePumpFromRestrictedList(const LONG lPumpNumber);

	inline  LONG		GetRestrictedListSize(){return m_cRestrictedPumpsList.GetCount();}

private:	
	
	virtual void AddPOSToApprovedList(const LONG lPOSNumber);	
	virtual void IncApprovedCount(const LONG lPOSNumber);
	virtual void DecApprovedCount(const LONG lPOSNumber);


private:
	CMap<LONG, LONG, RestrictionType, RestrictionType&>	m_cRestrictedPumpsList;
	CMap<LONG, LONG, BYTE, BYTE&>						m_cApprovedPOSCountsList;
};


//4.0.22.504
class CCashierFuelingRestriction : public CPOSFuelingRestriction
{
public:
	
	CCashierFuelingRestriction();
	virtual ~CCashierFuelingRestriction(){};

	virtual void		Init(BOOL bCreate);

	virtual inline void	CashierSignIn();
	virtual inline void	CashierSignOut();
	virtual inline void	ResetCashiersCounter();					//4.0.22.506 TD 70956

	virtual void		SetRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType){/* */ };		//4.0.23.500 TD 78089

protected:

 	virtual FRRetCode	ApprovedCondition(const LONG lPumpNumber, const RestrictionType& rRestrictionType)const;

	virtual void		HandleApproveAllowed(const LONG lPumpNumber, const  RestrictionType& rRestrictionType);

	virtual BOOL		IsAllowedSetApprove(const RestrictionType& rRestrictionType)const{return FALSE;}
	virtual BOOL		IsAllowedClearApprove(const LONG lPumpNumber)const{return FALSE;}

	virtual void		RemovePumpFromRestrictedList(const LONG lPumpNumber);
	
	void				SaveCashiersCounter()const;
	void				SetMaxApprovedPumps(long lMaxApprovedPumps);

private:

	LONG	m_lMaxApprovedPumps;
	LONG	m_lSignInCashiersCount;

	LONG	m_lApprovedPumpsCount;
};

#endif // !defined(AFX_POSFUELINGRESTRICTION_H__E1C84B92_F071_4C5F_A690_B48B9F72E697__INCLUDED_)
