// CFGEventsHandler.h: interface for the CCFGEventsHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CFGEVENTSHANDLER_H__CC817CB0_C102_49D8_B299_2E27E3DF9028__INCLUDED_)
#define AFX_CFGEVENTSHANDLER_H__CC817CB0_C102_49D8_B299_2E27E3DF9028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCFGEventsHandler: 
			public IDispEventImpl <
			0,
			CCFGEventsHandler,
			&DIID__ICFGEvents,
			&LIBID_HYDSERVICELib,
			1,
			0>,
			
			public CComObjectRootEx<CComMultiThreadModel> 
{
public:
	void SetEventClientId(long lClientId);
	long GetEventClientId();
	HRESULT __stdcall CFGDBNewData(BSTR sServerPath ,BSTR sTableName ,BSTR sKeyName,long lFlags,long lClientEventData);
					  
	BEGIN_SINK_MAP(CCFGEventsHandler)
		SINK_ENTRY_EX(0, DIID__ICFGEvents,1,CFGDBNewData)
	END_SINK_MAP()

	CCFGEventsHandler();
	virtual ~CCFGEventsHandler();

private:
	long m_lEventsClientId;
};

#endif // !defined(AFX_CFGEVENTSHANDLER_H__CC817CB0_C102_49D8_B299_2E27E3DF9028__INCLUDED_)
