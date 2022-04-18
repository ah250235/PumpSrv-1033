#pragma once
#include "Thread.h"

#define SLEEP_DURATION 60000

enum eFuelMobileSrvStatus
{
	FuelMobileSrv_OFF,
	FuelMobileSrv_OK,
};

class CFuelMobileSrvThread :
	public CThread
{
public:
	CFuelMobileSrvThread();
	~CFuelMobileSrvThread();

	virtual void ThreadRoutine(void * pData = NULL);
	CThreadRetCode StartThread();
	CThreadRetCode Init();
	CThreadRetCode CleanUp();

private:
	BOOL	m_bCreated;
	BOOL	m_bActive;
};

