// OLAEvents.h: interface for the COLAEvents class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLAEVENTS_H__DC0F7B97_5D9E_11D5_AC0B_000102191E6D__INCLUDED_)
#define AFX_OLAEVENTS_H__DC0F7B97_5D9E_11D5_AC0B_000102191E6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "OLA/OlaSrv.exe" auto_rename no_namespace named_guids

//OLA events
#define OLA_EVENT_REPORT_READY  1
#define OLA_EVENT_DISPLAY_MSG	5


class COLAEvents : public IDispEventImpl<
		0,
		COLAEvents,
		&DIID_ICallBackEvents,
		&LIBID_OlaSrv,
		1,
		0>,
		
		public CComObjectRootEx<CComMultiThreadModel>
 
{
public:

	HRESULT __stdcall OlaEvent ( long EventId, long TerminalId, BSTR sParams );

	BEGIN_SINK_MAP(COLAEvents)
		SINK_ENTRY_EX(0, DIID_ICallBackEvents, 1, OlaEvent)
	END_SINK_MAP()
};

#endif // !defined(AFX_OLAEVENTS_H__DC0F7B97_5D9E_11D5_AC0B_000102191E6D__INCLUDED_)
