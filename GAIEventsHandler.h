// GAIEventsHandler.h: interface for the CCAIEventsHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAIEVENTSHANDLER_H__542FB78B_9EC9_43AF_B779_7BF1E7F6A940__INCLUDED_)
#define AFX_GAIEVENTSHANDLER_H__542FB78B_9EC9_43AF_B779_7BF1E7F6A940__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlarmDef.h"

class CGAIEventsHandler: 
			public IDispEventImpl <
			0,
			CGAIEventsHandler,
			&DIID__IGAIEvents,
			&LIBID_ALARMSRVLib,
			1,
			0>,
			
			public CComObjectRootEx<CComMultiThreadModel> 
{
public:
	void SetEventClientId(long lClientId);
	long GetEventClientId();
	HRESULT __stdcall UpdateAlarmMap(long lCatagory,long lSubCatagory,long lAlarmNumber,long lDevice,long lAlarmStatus,long lAlarmId, long lFlags); //4.0.2.31
	HRESULT __stdcall UpdateAlarmMapEx(long lCatagory,long lSubCatagory,long lAlarmNumber,long lDevice,long lAlarmStatus,long lAlarmId, long lFlags, BSTR sAlarmExtraParams); //4.0.13.500
	
	
	BEGIN_SINK_MAP(CGAIEventsHandler)		
		SINK_ENTRY_EX(0, DIID__IGAIEvents,1,UpdateAlarmMap)
		SINK_ENTRY_EX(0, DIID__IGAIEvents,2,UpdateAlarmMapEx) 
	END_SINK_MAP()  

public:
	CGAIEventsHandler();
	virtual ~CGAIEventsHandler();
	
private:
	long m_lEventsClientId;
};

#endif // !defined(AFX_GAIEVENTSHANDLER_H__542FB78B_9EC9_43AF_B779_7BF1E7F6A940__INCLUDED_)
