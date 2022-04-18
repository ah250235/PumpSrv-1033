// FuelingRestriction.h: interface for the FuelAuthorizeLimitValidation class.
// Created //4.0.22.501 TD 70956
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUELINGRESTRICTION_H__59F5EB15_95ED_4A5C_9246_303B046BBD4E__INCLUDED_)
#define AFX_FUELINGRESTRICTION_H__59F5EB15_95ED_4A5C_9246_303B046BBD4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseFuelingRestriction.h"

#define FUELING_RESTRICTIONS_NUMBER 3	//4.0.22.504

class CFuelingRestriction  
{
public:
	CFuelingRestriction();
	virtual ~CFuelingRestriction();

	void	Init(BOOL bCreate);
	
	BOOL	IsApproveAllowed(const LONG lPumpNumber, const LONG lPOSNumber);
	void	Restrict(const LONG lPumpNumber, const PumpReservation eNewPumpRes, const PumpStatus eNewPumpStatus, 
						const PumpResExt eNewPumpResExt, const PumpReservation ePrevPumpRes, const PumpStatus ePrevPumpStatus);
	
	void	CashierSignIn();
	void	CashierSignOut();
	void	ResetCashiersCounter();					//4022506 TD 70956

	void	CleanUp();

protected:

	void	SetRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType);
	void	ClearRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType);

private:

	static CBaseFuelingRestriction* m_cFuelingRestrictions[FUELING_RESTRICTIONS_NUMBER];
};


#endif // !defined(AFX_FUELINGRESTRICTION_H__59F5EB15_95ED_4A5C_9246_303B046BBD4E__INCLUDED_)
