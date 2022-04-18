#pragma once

#include "PumpSrvDatabase.h"
#include "ThreadedQueue.h"

class CPumpSrvDatabaseLogger : public CPumpSrvDatabaseCallback
{
public:
	CPumpSrvDatabaseLogger();
	virtual ~CPumpSrvDatabaseLogger();

public:
	virtual void	PostError(const CFuelErrorInfo & FuelErrorInfo);
	virtual	void	ProcessError(const CFuelErrorInfo & FuelErrorInfo);

public:
	static CPumpSrvDatabaseLogger & GetInstance(void)
	{
		static CPumpSrvDatabaseLogger m_Callback;

		return m_Callback;
	}

protected:
	std::unique_ptr<CThreadedQueue<CFuelErrorInfo, CPumpSrvDatabaseLogger>>	m_ThreadQueue;
};


