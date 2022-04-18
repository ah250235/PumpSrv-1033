// PumpSrv.cpp : Implementation of WinMain


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f PumpSrvps.mk in the project directory.


#include "stdafx.h"
#include "resource.h"
#include "initguid.h"
#include "PumpSrv_i.c"
#include "GPI.h"
#include "GCI.h"
#include "Ctrl.h"
#include <process.h>
#include "SQLItemsInfoTest.h"
#include "PumpSrvDatabase.h"

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

CExeModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_GPI, CGPI)
	OBJECT_ENTRY(CLSID_GCI, CGCI)
	OBJECT_ENTRY(CLSID_Ctrl, CCtrl)
END_OBJECT_MAP()

void ClosePumpSrvDatabase(void)
{
	CPumpSrvDatabase & PumpSrvDatabase = CPumpSrvDatabase::GetInstance();
	PumpSrvDatabase.Close();
}

void HandleAtExit(void)
{
	ClosePumpSrvDatabase();
}

LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
	while (*p1 != NULL)
	{
		LPCTSTR p = p2;
		while (*p != NULL)
		{
			if (*p1 == *p++)
				return p1+1;
		}
		p1++;
	}
	return NULL;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int InitMainWndClass(HINSTANCE hInst)
{
	WNDCLASS wcl;
	wcl.hInstance = hInst;
	wcl.lpszClassName = "PumpSrv";
	wcl.lpfnWndProc = WndProc;
	wcl.style = 0;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.lpszMenuName = NULL;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	return RegisterClass(&wcl);
}

//4.0.1.38
inline void AllowAccess()
{
	HRESULT hRes = S_OK;

	hRes = CoInitializeSecurity(NULL,
			     		        -1,
								NULL,
								NULL,
								RPC_C_AUTHN_NONE,
								RPC_C_IMP_LEVEL_IMPERSONATE,
								NULL,
								EOAC_NONE,
								NULL);

	//4.0.1.40
	//_ASSERTE(SUCCEEDED(hRes));
	
	return;
}

/////////////////////////////////////////////////////////////////////////////

extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, 
	HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nShowCmd*/)
{
	lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT
//	HRESULT hRes = CoInitialize(NULL);
//  If you are running on NT 4.0 or higher you can use the following call
//	instead to make the EXE free threaded.
//  This means that calls come in on a random RPC thread
 	HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	_ASSERTE(SUCCEEDED(hRes));

	//4.0.1.38
	AllowAccess();

	_Module.Init(ObjectMap, hInstance);
	_Module.dwThreadID = GetCurrentThreadId();
	TCHAR szTokens[] = _T("-/");
	
	int nRet = 0;
	BOOL bRun = TRUE;
	LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
	

	while (lpszToken != NULL)
	{
		if (lstrcmpi(lpszToken, _T("UnregServer"))==0)
		{
			_Module.UpdateRegistryFromResource(IDR_PumpSrv, FALSE);
			nRet = _Module.UnregisterServer();
			bRun = FALSE;
			break;
		}
		if (lstrcmpi(lpszToken, _T("RegServer"))==0)
		{
			_Module.UpdateRegistryFromResource(IDR_PumpSrv, TRUE);
			nRet = _Module.RegisterServer(TRUE);
			bRun = FALSE;
			break;
		}
		lpszToken = FindOneOf(lpszToken, szTokens);
	}

	//4.0.0.21 start
	BOOL bInitServerSuccessfully = _Module.m_server.InitServer(bRun);
	//4.0.0.21 end
	//CSQLItemsInfoTest test;
	//test.TestManager();
	if (bRun && bInitServerSuccessfully)
	{

		hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, 
			REGCLS_MULTIPLEUSE);
		_ASSERTE(SUCCEEDED(hRes));

	//	int iProcessId = _getpid() ;

		 if (!InitMainWndClass(hInstance))
		  return -1;

		_Module.m_hWnd = CreateWindow("PumpSrv", "", WS_OVERLAPPEDWINDOW,
             CW_USEDEFAULT,
             CW_USEDEFAULT,
             CW_USEDEFAULT,
             CW_USEDEFAULT,
             HWND_DESKTOP,
             NULL,
             hInstance,
             NULL);

		ShowWindow(_Module.m_hWnd , SW_HIDE);
		UpdateWindow(_Module.m_hWnd);

		try
		{
			MSG msg;
			memset(&msg, 0, sizeof(msg));

			do
			{
				Sleep(250);

				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

			}	while (msg.message != WM_QUIT);
		}
		catch(...)
		{
			if(LOG_ERROR)
				_LOGMSG.LogMsg("_tWinMain(), catch exception ", LOG_LEVEL_0);	
		}
	}


	//4.0.3.20 start
	_Module.m_server.ServerCleanUp(bRun);   //4.0.5.30

	//ClosePumpSrvDatabase();

	_Module.RevokeClassObjects();

	//4.0.3.30 end
	CoUninitialize();
	return nRet;
}
