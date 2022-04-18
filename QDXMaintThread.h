// QDXMaintThread.h: interface for the CQDXMaintThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QDXMAINTTHREAD_H__71B19F4C_A49A_4D09_BDB3_E5461E211103__INCLUDED_)
#define AFX_QDXMAINTTHREAD_H__71B19F4C_A49A_4D09_BDB3_E5461E211103__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"


class CQDXMaintThread : public CThread  
{
public:
	CQDXMaintThread();
	virtual ~CQDXMaintThread();

	virtual void ThreadRoutine(void * pData = NULL);
	CThreadRetCode StartQDXMaintThread(long lFileNum = 0, BOOL bUploadOnly=FALSE);
	CThreadRetCode Init();
	CThreadRetCode CleanUp();
	CThreadRetCode StartMaintAll(BOOL bUploadOnly = FALSE);
	
private:
	BOOL	m_bCreated;
	BOOL	m_bActive;
	long	m_lFileNum;
	BOOL	m_bUploadOnly;
	//CList<long,long>	m_cRetryList;
	const long m_lTimeout;

};

#endif // !defined(AFX_QDXMAINTTHREAD_H__71B19F4C_A49A_4D09_BDB3_E5461E211103__INCLUDED_)
