// ShiftProcess.h: interface for the CShiftProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHIFTPROCESS_H__B914343D_B9D2_4367_9AAF_14FAA1B820B4__INCLUDED_)
#define AFX_SHIFTPROCESS_H__B914343D_B9D2_4367_9AAF_14FAA1B820B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CShiftProcess  
{
public:
	void SetFlagClearStuckTrs(BOOL bVal);
	BOOL GetFlagClearStuckTrs();
	void CheckStuckTransaction();
	CShiftProcess();
	virtual ~CShiftProcess();

private:
	BOOL m_bClearStuckTrs;
};

#endif // !defined(AFX_SHIFTPROCESS_H__B914343D_B9D2_4367_9AAF_14FAA1B820B4__INCLUDED_)
