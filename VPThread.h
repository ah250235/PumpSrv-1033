// VPThread.h: interface for the CVPThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VPTHREAD_H__DBF31599_EFE8_4F08_94F4_792ACE1F315D__INCLUDED_)
#define AFX_VPTHREAD_H__DBF31599_EFE8_4F08_94F4_792ACE1F315D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"
#define SLEEP_DURATION 60000

//4.0.24.60
class CVPThread : public CThread  
{
public:
	CVPThread();
	virtual ~CVPThread();

	virtual void ThreadRoutine(void * pData = NULL);
	CThreadRetCode StartVPThread();
	CThreadRetCode Init();
	CThreadRetCode CleanUp();
	CThreadRetCode StartMaintAll(BOOL bUploadOnly = FALSE);
	void           ResetVpRecoveryCounter();
	
private:
	BOOL	m_bCreated;
	BOOL	m_bActive;
// 4.0.29.500 TD 264908 start
	long    m_lVpRecoveryMaxRetries;
	long    m_lVpRecoveryCounter;

	void    InitVpRecoveryCounter();
	void    CheckVpRecoveryState();
    BOOL	IsVpFailedRecovery();
	void    TryToVpRecovery();
// 4.0.29.500 TD 264908 end

};

#endif // !defined(AFX_VPTHREAD_H__DBF31599_EFE8_4F08_94F4_792ACE1F315D__INCLUDED_)
