// GAIEventsHandler.cpp: implementation of the CGAIEventsHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLAService2EventsHandler.h"
#include "XMLInterface.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


COLAService2EventsHandler* _pOLAService2EventsHandler;



HRESULT __stdcall COLAService2EventsHandler::StatusEvent(BSTR sInBuffer)
{
	BOOL			  bRetCode= TRUE;
	char			  strMsg[DEFAULT_MAX_MESSAGE_SIZE];
	CXMLInterface	  xmlInitDoc;
	CXMLPointers	  pCurrElement,pNode,pChildNode,pChildNode2;
	long lIsOnline = 0;
	long lIsOnline1 = 0;
	char			  strXML[MAX_BUF];	//10000
	char sTmp[_MAX_PATH];

	memset(sTmp,0,sizeof(sTmp));

try
{
	bRetCode = xmlInitDoc.InitDoc();
	if (bRetCode)
	{	
		int iLen = WideCharToMultiByte(CP_ACP,0,sInBuffer,-1,strXML,sizeof(strXML),NULL,NULL);
		if (iLen)
			bRetCode = xmlInitDoc.LoadXmlString(strXML);
		else
			bRetCode = FALSE;
	}
	if (bRetCode)
	{
		//xmlInitDoc.SaveToFile("C:\\xxOLAOffline.xml");
		bRetCode = xmlInitDoc.GetRootNode(&pNode);	//4.0.21.11
	}
	if (bRetCode)
	{
		bRetCode = xmlInitDoc.SelectSingleElementNode("IsOnline",&pChildNode);

		xmlInitDoc.GetChildNodeValueToByteArrWithConvert(&pChildNode, sTmp); //4.0.21.11
		lIsOnline = atol(sTmp);
	}


	if (bRetCode)
	{
		sprintf(strMsg, "OLAService2EventsHandler: received an event from OLA, IsOnline = %d",lIsOnline);
		_LOGMSG.LogMsg(strMsg, DEFAULT_LOG_LEVEL);

		if(lIsOnline)	//4.0.21.11
		{
			//4.0.21.13 68756
			_Module.m_server.SetPAPEnabledOLA(TRUE);
			if (_Module.m_server.IsPAPEnabledDSS())
			{
				_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_OLA_ONLINE);
				_LOGMSG.LogMsg("COLAService2EventsHandler::StatusEvent, GCI_EVENT_OLA_ONLINE was sent to CL"); //4.0.21.20
			}

		}
		else
		{
			//4.0.21.13 68756
			_Module.m_server.SetPAPEnabledOLA(FALSE);
			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_OLA_OFFLINE);
			_LOGMSG.LogMsg("COLAService2EventsHandler::StatusEvent, GCI_EVENT_OLA_OFFLINE was sent to CL"); //4.0.21.20
		}
	}
	else
		_LOGMSG.LogMsg("OLAService2EventsHandler: failed to parse the received event xml", DEFAULT_LOG_LEVEL);

	xmlInitDoc.CloseDoc(); 
}
catch (...)
{
		_LOGMSG.LogMsg("OLAService2EventsHandler: Exception failed to parse the received event xml", DEFAULT_LOG_LEVEL);
}

	return S_OK;

}


