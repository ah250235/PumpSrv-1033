// TargetMessageNMPDetails.h: interface for the CTargetMessageNMPDetails class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TARGETMESSAGENMPDETAILS_H__F57E17AA_4542_4208_9209_EC018A9F515E__INCLUDED_)
#define AFX_TARGETMESSAGENMPDETAILS_H__F57E17AA_4542_4208_9209_EC018A9F515E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NmpDetalis.h"

// CR 312183
class CTargetMessageNMPDetails: public CNMPDetails
{
public:
	CTargetMessageNMPDetails();
	~CTargetMessageNMPDetails() {};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
};

#endif // !defined(AFX_TARGETMESSAGENMPDETAILS_H__F57E17AA_4542_4208_9209_EC018A9F515E__INCLUDED_)
