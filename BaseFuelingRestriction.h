// BaseFuelingRestriction.h: interface for the CBaseFuelingRestriction class.
// Created //4.0.22.501 TD 70956
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEFUELINGRESRICTION_H__E0815491_EA2F_4A83_A59D_AA03FB13C950__INCLUDED_)
#define AFX_BASEFUELINGRESRICTION_H__E0815491_EA2F_4A83_A59D_AA03FB13C950__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef enum FRRetCode{
	FRRC_APPROVED,
	FRRC_DISAPPROVED,
	FRRC_NOT_ACTIVE,
}FRRetCode;


struct RestrictionType	
{
	PumpStatus		eNewPumpStatus;
	PumpReservation	eNewPumpRes;
	PumpResExt		eNewPumpResExt;
	PumpStatus		ePrevPumpStatus;
	PumpReservation ePrevPumpRes;
	LONG			lPOSNumber;
	BOOL			bDispensed;

	RestrictionType(const LONG lPOSNumber = NO_POS_NUMBER, const PumpReservation eNewPumpRes = UNKNOWN_RES, 
							const PumpStatus eNewPumpStatus = UNKNOWN_STATUS, 
							const PumpResExt	eNewPumpResExt = UNKNOWN_RES_EXT, 
							const PumpReservation ePrevPumpRes = UNKNOWN_RES, 
							const PumpStatus ePrevPumpStatus = UNKNOWN_STATUS)
	{
		this->eNewPumpStatus	= eNewPumpStatus;
		this->eNewPumpRes		= eNewPumpRes;
		this->eNewPumpResExt	= eNewPumpResExt;
		this->ePrevPumpStatus	= ePrevPumpStatus;
		this->ePrevPumpRes		= ePrevPumpRes;		
		this->lPOSNumber		= lPOSNumber;
		this->bDispensed		= FALSE;
	}
};


class CBaseFuelingRestriction  
{
public:

	CBaseFuelingRestriction();
	virtual ~CBaseFuelingRestriction();

	virtual void			Init(BOOL bCreate) = 0;		//4.0.22.506 TD 70956

	//Configuration methods
	virtual inline BOOL		IsActive()const;
	virtual inline void		SetMaxConcurrent(const DWORD dwMaxConcurrent); 

	//Pure virtual
	virtual BOOL	IsApproveAllowed(const LONG lPumpNumber, const RestrictionType& rRestrictionType) = 0;
					
	virtual void	SetRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType) = 0;
	virtual void	ClearRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType) = 0;
	
	virtual void	CashierSignIn(){};				//4.0.22.504
	virtual void	CashierSignOut(){};				//4.0.22.504
	virtual void	ResetCashiersCounter(){};		//4.0.22.506 TD 70956

	virtual void	CleanUp() = 0;

protected:

	virtual	BOOL	IsSupportedPumpRes(const PumpReservation ePumpRes)const;
	virtual BOOL	IsAllowedSetRestriction(const RestrictionType& rRestrictionType)const = 0;
	virtual BOOL	IsAllowedClearRestriction(const RestrictionType& rRestrictionType)const = 0;


protected:
	
	DWORD			m_dwMaxConcurrent;
	PumpReservation	m_aeSupportedPumpReservation[UNKNOWN_RES];
};

#endif // !defined(AFX_BASEFUELINGRESRICTION_H__E0815491_EA2F_4A83_A59D_AA03FB13C950__INCLUDED_)
