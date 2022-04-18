// OLA32Events.h: interface for the COLA32Events class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLA32EVENTS_H__48035A7A_66AB_4B2E_A9F8_CC0A482B52F5__INCLUDED_)
#define AFX_OLA32EVENTS_H__48035A7A_66AB_4B2E_A9F8_CC0A482B52F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "OLA/Ola32.exe" no_namespace named_guids

//OLA events
#define OLA_EVENT_REPORT_READY      1
#define UPDATE_SHIFT_DEBIT_PARAM	2
#define END_SHIFT_DEBIT_PARAM		3
#define INIT_DEBIT_EVENT_PARAM		4
#define OLA_EVENT_DISPLAY_MSG		5

class COLA32Events : public IDispEventImpl<
		0,
		COLA32Events,
		&DIID_IOla32CallBackEvents,
		&LIBID_Ola32,
		1,
		0>,
		
		public CComObjectRootEx<CComMultiThreadModel>
 
{
public:

	HRESULT __stdcall OlaEvent ( long EventId, long TerminalId, BSTR sParams );

	BEGIN_SINK_MAP(COLA32Events)
		SINK_ENTRY_EX(0, DIID_IOla32CallBackEvents, 1, OlaEvent)
	END_SINK_MAP()
};

#endif // !defined(AFX_OLA32EVENTS_H__48035A7A_66AB_4B2E_A9F8_CC0A482B52F5__INCLUDED_)
