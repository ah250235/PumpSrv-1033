// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__455634A5_1A48_11D1_B8A5_0000E8D0CBF8__INCLUDED_)
#define AFX_STDAFX_H__455634A5_1A48_11D1_B8A5_0000E8D0CBF8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define STRICT
#define _WIN32_WINNT 0x0501
#define _ATL_APARTMENT_THREADED
#define USE_XML_PRICE_CHANGE	0  //4.0.0.86

// ML - START - Extra definition need for MFC support.
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxtempl.h>
#endif // _AFX_NO_AFXCMN_SUPPORT
// ML - END

#import "TcpComCln.exe" no_namespace named_guids

#include <atlbase.h>
#include <list>
#include <map>
#include <memory>
#include <vector>
#include "PumpSrv.h"
#include "PumpClass.h"
#include "CommonStruct.h"
#include "ServerMain.h"	
#include "Convert.h"
#include "HydraDef.h"
#include "AlarmClass.h"
#include "ExceptionHandler.h" //4.0.19.502
#include "StaticString.h"


//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
class CExeModule : public CComModule
{
public:
	CServerMain				m_server;
	LONG					Unlock();
	DWORD					dwThreadID;
	HWND					m_hWnd;
};
extern CExeModule _Module;
#include <atlcom.h>
#include "RTEventsHandler.h"
#include "GLIEventsHandler.h"
#include "CFGEventsHandler.h"
#include "GAIEventsHandler.h"   //4.0.0.79
#include "RTIEventsHandler.h"//4.0.9.508
#include "OLAService2EventsHandler.h"	//TD 42728
#include "Ctrl.h"
#include "ScreenDef.h"


extern CRTEventsHandler* _pRTEventHandler;
extern CCFGEventsHandler* _pCFGEventHandler;
extern CGAIEventsHandler* _pGAIEventsHandler;  //4.0.0.79
extern CRTIEventsHandler* _pRTIEventsHandler;//4.0.9.508
extern CGLIEventsHandler* _pGLIEventsHandler;
extern COLAService2EventsHandler* _pOLAService2EventsHandler;	//TD 42728

#define _HOST			_Module.m_server.m_cHydra
#define _ALARM_SRV		_Module.m_server.m_cAlarm
#define _HANDLE_EXCEPTION(x) CExceptionHandler::ExceptionHandler().HandleException(GetExceptionInformation(),_T(x)) //4.0.19.502
#define _PARAM _Module.m_server.m_cParam

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__455634A5_1A48_11D1_B8A5_0000E8D0CBF8__INCLUDED)
