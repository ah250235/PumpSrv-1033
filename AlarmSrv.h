// AlarmSrv.h: interface for the CAlarmSrv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMSRV_H__E43B8BDF_A211_11D6_BB70_00500470AD8E__INCLUDED_)
#define AFX_ALARMSRV_H__E43B8BDF_A211_11D6_BB70_00500470AD8E__INCLUDED_

#import "AlarmSrv.tlb"  no_namespace named_guids

// general definition.
#define MAX_ALARM_CATEGORY		SUB_TERMINAL_ENCRYPTION
#define MAX_ALARM_PUMP			MAX_PUMPS_64+1		// add one category for system alarm.	
#define MAX_ALARM_TANKS			MAX_TANKS  + 1		// add one category for system alarm. //4.0.2.37	

#define MAX_SPECIAL_CATEGORY    3		// OLA , TANK , System
#define SYSTEM_CATEGORY         0


// Alarm Special group.
#define MAX_ALARM_SPECIAL_GROUP    2

#define ALARM_GROUP_OLA		       0
#define ALARM_GROUP_TANK		   1
#define ALARM_GROUP_PUMP		   2
#define ALARM_GROUP_GENERAL_SYSTEM 3

#define ALARM_GROUP_NOT_FOUND      99


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAlarmSrv  
{
public:
	long m_lAlarmSrvActive;
	long InitAlarmSrv(long lAlarmSrvActive);
	long CreateConnectionToGAIEvents();	
	virtual long WriteComError(_com_error & e ) =0;
	long CloseConnectToAlarmSrv();
	long CreateConnectToAlarmSrv();
	long InitAlarmsNumerators(); //4.0.12.501

	CAlarmSrv();
	virtual ~CAlarmSrv();
	long Restore_COM();
	IGAIPtr   m_pGAI;

	long m_lAlarmPumpMap[MAX_ALARM_PUMP];
	long m_lAlarmTankMap[MAX_ALARM_TANKS];
	long m_lAlarmSpecialGroupMap[MAX_ALARM_SPECIAL_GROUP];  //4.0.2.37


protected:	
	
};

#endif // !defined(AFX_ALARMSRV_H__E43B8BDF_A211_11D6_BB70_00500470AD8E__INCLUDED_)
