// EventManager.h: interface for the CEventManager class.
// This class is managing all the events we are firing from.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTMANAGER_H__281D4C11_50CC_4227_A3BD_93F6773F0B72__INCLUDED_)
#define AFX_EVENTMANAGER_H__281D4C11_50CC_4227_A3BD_93F6773F0B72__INCLUDED_


#include "Queue.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct 
{
	long	m_lDataId;
	long	m_lFlags;
}CEventInfo;

class CEventManager  
{
public:
	long FireCtrlEvent();
	long AddEvent(long lDataId, long lFlags);
	CEventManager();
	virtual ~CEventManager();


protected:
	CQueue<CEventInfo, 100>		m_cEventsQueue;
};

#endif // !defined(AFX_EVENTMANAGER_H__281D4C11_50CC_4227_A3BD_93F6773F0B72__INCLUDED_)
