// CopientCommandExecuter.h: interface for the CCopientCommandExecute class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COPIENTCOMMANDExecute_H__A41157EC_41BA_4DC6_9D0A_73E5754BBDD0__INCLUDED_)
#define AFX_COPIENTCOMMANDExecute_H__A41157EC_41BA_4DC6_9D0A_73E5754BBDD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LoyaltyCommandExecuter.h"

class CLoyaltyCopientCommandExecuter : public CGLILoyaltyCommandExecuter  
{
public:
	CLoyaltyCopientCommandExecuter();
	virtual ~CLoyaltyCopientCommandExecuter();
	virtual BOOL Query(long lPumpNumber,PAY_AT_PUMP_INFO & pInfo);
	virtual BOOL AddMemberInfo(long lPumpNumber,long lSegmentId) { return FALSE;}
};

#endif // !defined(AFX_COPIENTCOMMANDExecute_H__A41157EC_41BA_4DC6_9D0A_73E5754BBDD0__INCLUDED_)
