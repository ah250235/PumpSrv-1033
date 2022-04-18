// InitDebitHandler.h: interface for the CInitDebitHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITDEBITHANDLER_H__C8596B63_F89D_11D7_BBCF_00500470AD8E__INCLUDED_)
#define AFX_INITDEBITHANDLER_H__C8596B63_F89D_11D7_BBCF_00500470AD8E__INCLUDED_

#include "OLA.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInitDebitHandler    
{
public:
	long FindNextPump(long &lPumpNumber);
	void SetWorkingPump(long lPumpNumber);
	long GetWorkingPump();
	void SetStatus(long lPumpNumber,DebitSrvStatus eNewStatus);
	DebitSrvStatus GetStatus(long lPumpNumber);
	CInitDebitHandler();
	virtual ~CInitDebitHandler();
private:
	DebitSrvStatus m_byDebitInitArray[MAX_PUMPS_64];  //4.0.3.58
	long m_lWorkingPump;

};

#endif // !defined(AFX_INITDEBITHANDLER_H__C8596B63_F89D_11D7_BBCF_00500470AD8E__INCLUDED_)
