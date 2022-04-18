#include "stdafx.h"
#include "PumpSrvDatabaseLogger.h"
#include "Logger.h"

CPumpSrvDatabaseLogger::CPumpSrvDatabaseLogger()
{
	m_ThreadQueue.reset(new CThreadedQueue<CFuelErrorInfo, CPumpSrvDatabaseLogger>(*this, &CPumpSrvDatabaseLogger::ProcessError));
}


CPumpSrvDatabaseLogger::~CPumpSrvDatabaseLogger()
{
}

void CPumpSrvDatabaseLogger::PostError(const CFuelErrorInfo & FuelErrorInfo)
{
	m_ThreadQueue->PushObject(FuelErrorInfo);
}

void CPumpSrvDatabaseLogger::ProcessError(const CFuelErrorInfo & FuelErrorInfo)
{
	CStringA str;
	str.Format("%s Error '%s'", __FUNCTION__, FuelErrorInfo.GetErrorText());

	_Module.m_server.m_cLogger.LogMsg(str);
}

