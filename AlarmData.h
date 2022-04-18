// AlarmData.h: interface for the AlarmData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMDATA_H__3D4A3653_B27E_11D6_BB71_00500470AD8E__INCLUDED_)
#define AFX_ALARMDATA_H__3D4A3653_B27E_11D6_BB71_00500470AD8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AlarmData  
{
public:
	void SetAlarmWarningFlags(long lFlags);
	void SetAlarmErrorFlags(long lFlags);
	long GetAlarmErrorFlags();
	void SetWarningID(long lWarning);
	long GetWarningId();
	void SetErrorId(long lError);
	long GetErrorId();
	AlarmData();
	virtual ~AlarmData();

private:
	long m_lWarningId;
	long m_lErrorID;
	long m_AlarmErrorFlags;
	long m_AlarmWarningFlags;
};

#endif // !defined(AFX_ALARMDATA_H__3D4A3653_B27E_11D6_BB71_00500470AD8E__INCLUDED_)
