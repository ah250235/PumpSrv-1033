// EventManager.cpp: implementation of the CEventManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EventManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventManager::CEventManager()
{

}

CEventManager::~CEventManager()
{

}


// Function name	: CEventManager::AddEvent
// Description	    : Adds an event to the events queue.
// Return type		: long 
// Argument         : long lDataId - event data id.
// Argument         : long lFlags - for future use.
long CEventManager::AddEvent(long lDataId, long lFlags)
{
	long lRetVal = 0;
	CEventInfo	cEventInfoData;

	//adding avents to the queue only if there are connected event clients.
	if(_Module.m_server.m_CtrlEventsObjMap.IsEmpty()== FALSE)
	{
		cEventInfoData.m_lDataId = lDataId;
		cEventInfoData.m_lFlags = lFlags;

		//entering the message into the update cmd queue.
		lRetVal = m_cEventsQueue.Set(&cEventInfoData);

		// 4.0.2.27 _Module.m_server.StartEventThread();
	}
	if(lRetVal == QUEUE_FULL)
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("Event Queue is Full.Event %d not added",cEventInfoData.m_lDataId); 
			_LOGMSG.LogMsg(str);
		}
	}
	return lRetVal;
}


// Function name	: CEventManager::FireCtrlEvent
// Description	    : Getting the event from the events queue and 
//					  fires it according to connected clients in the
//					  m_server.m_CtrlEventsObjMap.
// Return type		: long 
long CEventManager::FireCtrlEvent()
{
	long	lRetVal = 0;
	HRESULT	hr;
	long	lKey;
	int		iNumOfElements = 0;
	POSITION pos = _Module.m_server.m_CtrlEventsObjMap.GetStartPosition();
	CCtrl* pCtrlObj = NULL;
	ICtrlEvents *pCtrlEventObj = NULL;
	CEventInfo	cEventInfoData;
	
	if(m_cEventsQueue.Empty())
		lRetVal = QUEUE_EMPTY;
	else
	{
		lRetVal = m_cEventsQueue.GetHead(&cEventInfoData);

		if( lRetVal == QUEUE_NO_ERROR )
		{
			iNumOfElements = _Module.m_server.m_CtrlEventsObjMap.GetCount();
			
			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("Event %d Fired, %d Clients are connected",cEventInfoData.m_lDataId,iNumOfElements); 
				_LOGMSG.LogMsg(str);
			}
			
			while (pos != NULL)
			{
				_Module.m_server.m_CtrlEventsObjMap.GetNextAssoc(pos, lKey, pCtrlEventObj );
				if (pCtrlEventObj)
				{
					try
					{
						pCtrlObj = (CCtrl*)pCtrlEventObj;

						hr = pCtrlObj->Fire_NewPumpSrvData(cEventInfoData.m_lDataId,cEventInfoData.m_lFlags);
						if(hr)
						{
							if(LOG_BASIC_FLOW_CONTROL)
							{
								CString str;
								str.Format("Fire_NewPumpSrvData HRESULT: %d",hr); 
								_LOGMSG.LogMsg(str);
							}
						}
						
					}
					catch(...)
					{
						if(LOG_ERROR)
							_LOGMSG.LogMsg("Fire_NewPumpSrvData, Com error", LOG_LEVEL_0);
					}
				}//end of if (pCtrlObj)
			}//end of while
			//removing the event info from the queue.
			m_cEventsQueue.Remove(&cEventInfoData);
		}//end of if( lRetVal == QUEUE_NO_ERROR )
	}//end of else QUEUE_EMPTY;

	return lRetVal;
}
