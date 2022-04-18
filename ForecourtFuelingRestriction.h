// ForecourtAuthorizeLimitValidation.h: interface for the ForecourtAuthorizeLimitValidation class.
// Created //4.0.22.501 TD 69203
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORECOURTAUTHORIZELIMITVALIDATION_H__BBF254AA_DCB2_4599_82C3_D78C66A8AD5F__INCLUDED_)
#define AFX_FORECOURTAUTHORIZELIMITVALIDATION_H__BBF254AA_DCB2_4599_82C3_D78C66A8AD5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseFuelingRestriction.h"

class CForecourtFuelingRestriction : public CBaseFuelingRestriction  
{
public:

	CForecourtFuelingRestriction();
	virtual ~CForecourtFuelingRestriction();
	
	virtual void	Init(BOOL bCreate);
	
	virtual BOOL	IsApproveAllowed(const LONG lPumpNumber, const  RestrictionType& rRestrictionType);

	virtual void	SetRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType);
	virtual void	ClearRestriction(const LONG lPumpNumber, const RestrictionType& rRestrictionType);

	virtual void	CleanUp();

protected:

	virtual FRRetCode	ApprovedCondition(const LONG lPumpNumber, const RestrictionType& rRestrictionType)const;
	virtual BOOL		IsAllowedSetRestriction(const RestrictionType& rRestrictionType)const;
	virtual BOOL		IsAllowedClearRestriction(const RestrictionType& rRestrictionType)const;

private:

	long	GetNumberOfDispensingPumps() const;  //4.0.23.50 78421 
	void	AddToOnHoldMaxConcurrentList(long lNewNode); 
	long	RemoveNextPumpFromOnHoldMaxConcurrentList(); 
	void	RemovePumpFromOnHoldMaxConcurrentList(long lPumpNumber); 
	void	PrintOnHoldMaxConcurrentList();
			
private:
	
	BYTE				m_byConcurrentCount;
	CList<long,long>	m_cOnHoldMaxConcurrentList;

protected:
	
};

#endif // !defined(AFX_FORECOURTAUTHORIZELIMITVALIDATION_H__BBF254AA_DCB2_4599_82C3_D78C66A8AD5F__INCLUDED_)
