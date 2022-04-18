// ComChecker.h: interface for the CCompChecker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMCHECKER_H__2140FAE1_BB3F_4327_AE99_D87E2C87F9CE__INCLUDED_)
#define AFX_COMCHECKER_H__2140FAE1_BB3F_4327_AE99_D87E2C87F9CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>
#include <vdmdbg.h>

#define MAX_LOOPS 20

//Return codes table:
#define OK							0
#define RUNNING						1
#define NOT_RUNNING					2
#define MORE_THEN_ONE_INSTANCE		3
#define DOREG_ALREADY_DONE			4
#define CANNOT_CREATE_FILE_MAPPING	5
#define CANNOT_CLOSE_HANDLE			6
#define GENERIC_ERROR				7
#define CANNOT_MAP_VIEW_OF_FILE		8


// for NT process list
#define  NT_PROC_PATH_LEN  128

typedef struct
{
   long  lProcID;
   char  szProcPath[NT_PROC_PATH_LEN];
   char  nu[64];
} NT_PROC_INFO;


typedef BOOL(WINAPI *lpf_ENUM_PROCESSES)( DWORD *, DWORD cb, DWORD * );
typedef BOOL(WINAPI *lpf_ENUM_PROCESS_MODULES)( HANDLE, HMODULE *, DWORD, LPDWORD );
typedef DWORD(WINAPI *lpf_GET_MODULE_FILENAME_EX)( HANDLE, HMODULE, LPTSTR, DWORD );
typedef INT (WINAPI *lpf_VDM_ENUM_TASK_WOW_EX)( DWORD, TASKENUMPROCEX  fp, LPARAM );

class CCompChecker  
{
	HANDLE m_hShareMemo;
	HANDLE m_hMutex;
public:
	BOOL IsAlreadyRunning(char* pszMutexName);
	BOOL IsAlreadyRunningWithoutCreateMutex(char *pszMutexName);
	CCompChecker(char *sId);
	virtual ~CCompChecker();
	long DoReg(char *sId);
	int DoUnReg();
	long IsRunning(char *sId);
	bool KillProcess(LPCSTR lpstrProgName);
	
private:
	BOOL LoadDllLibs(HINSTANCE *phPSAPILib, HINSTANCE *phVDMDBGLib);
	NT_PROC_INFO *NTProcList(int *pNum);
	long GetProgPID(LPCSTR lpstrProgName,long &lCounter);
	BOOL GetProcAddr(HINSTANCE hPSAPILib, 
                 HINSTANCE hVDMDBGLib,
                 lpf_ENUM_PROCESSES *lpfEnumProcesses, 
                 lpf_ENUM_PROCESS_MODULES *lpfEnumProcessModules,
                 lpf_GET_MODULE_FILENAME_EX *lpfGetModuleFileNameEx, 
                 lpf_VDM_ENUM_TASK_WOW_EX *lpfVDMEnumTaskWOWEx);
};

#endif // !defined(AFX_COMCHECKER_H__2140FAE1_BB3F_4327_AE99_D87E2C87F9CE__INCLUDED_)
