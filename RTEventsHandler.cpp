// RTEventsHandler.cpp: implementation of the CRTEventsHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RTEventsHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRTEventsHandler* _pRTEventHandler;

CRTEventsHandler::CRTEventsHandler()
{
	m_lEventsClientId = 0;
}

CRTEventsHandler::~CRTEventsHandler()
{

}

HRESULT __stdcall CRTEventsHandler::RTDBNewData(long lFileNum ,long lFlags,long lClientEventData)
{
	CString cMsg;
	BYTE byPumpFileIndexNum = _Module.m_server.GetPumpFileIndexNum();
	
	if(LOG_DETAIL_FLOW_CONTROL)
	{
		cMsg.Format("CRTEventsHandler::RTDBNewData: File number %d was updated",lFileNum);
		_LOGMSG.LogMsg(cMsg);
	}
	
	if(byPumpFileIndexNum == lFileNum)
	{
		_Module.m_server.LoadPumpFileInfo(lClientEventData);//4.0.0.45		
	}
			
	return S_OK;
}

void CRTEventsHandler::SetEventClientId(long lClientId)
{
	m_lEventsClientId = lClientId;	
}

long CRTEventsHandler::GetEventClientId()
{
	return m_lEventsClientId;
}
