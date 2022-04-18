// RTEventsHandler.h: interface for the CRTEventsHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTEVENTSHANDLER_H__542FB78B_9EC9_43AF_B779_7BF1E7F6A940__INCLUDED_)
#define AFX_RTEVENTSHANDLER_H__542FB78B_9EC9_43AF_B779_7BF1E7F6A940__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRTEventsHandler: 
			public IDispEventImpl <
			0,
			CRTEventsHandler,
			&DIID__IRTEvents,
			&LIBID_HYDSERVICELib,
			1,
			0>,
			
			public CComObjectRootEx<CComMultiThreadModel> 
{
public:
	void SetEventClientId(long lClientId);
	long GetEventClientId();
	HRESULT __stdcall RTDBNewData(long lFileNum ,long lFlags,long lClientEventData);
	
	BEGIN_SINK_MAP(CRTEventsHandler)
		SINK_ENTRY_EX(0, DIID__IRTEvents,1,RTDBNewData)
	END_SINK_MAP()  

public:
	CRTEventsHandler();
	virtual ~CRTEventsHandler();
	
private:
	long m_lEventsClientId;
};

#endif // !defined(AFX_RTEVENTSHANDLER_H__542FB78B_9EC9_43AF_B779_7BF1E7F6A940__INCLUDED_)
