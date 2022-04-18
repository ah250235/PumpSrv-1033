#include "StdAfx.h"
#include "ExeModule.h"
#include "serverMain.h"

CExeModule _Module;

CExeModule::CExeModule(void)
	: m_ServerPtr(new CServerMain)
	, m_server(*m_ServerPtr)
{
}

CExeModule::~CExeModule(void)
{
}

LONG CExeModule::Unlock()
{
	LONG l = CComModule::Unlock();
	if (l == 0)
	{
#if _WIN32_WINNT >= 0x0400
		if (CoSuspendClassObjects() == S_OK)
			PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
#else
		PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
#endif
	}
	return l;
}



