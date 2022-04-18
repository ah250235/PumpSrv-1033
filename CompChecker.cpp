// ComChecker.cpp: implementation of the CCompChecker class.
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CompChecker.h"

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
CCompChecker::CCompChecker(char *sId)
{
	m_hShareMemo = NULL;
	m_hMutex = NULL;//Yaron
	DoReg(sId);
}

//////////////////////////////////////////////////////////////////////
// Destruction
//////////////////////////////////////////////////////////////////////
CCompChecker::~CCompChecker()
{
	DoUnReg();
	//Yaron
	if(m_hMutex)
		CloseHandle(m_hMutex);
}


//////////////////////////////////////////////////////////////////////
// FUNCTION NAME: DoReg
//
// DESCRIPTION: Get a component name and register it by create FileMapping with this string. 
//
// PARAMETERS: sId[In]  - The component name to register (null terminated)
//
// RETURNS:  See return codes table.
//
// REMARKS: 1. If sId already matches the name of an existing event, semaphore, mutex or waitable
//          timer, then try MAX_LOOPS to define another sequence FileMapping (ie: _1_sId) 
//			2. In this FileMapping init a long size to be 0. every time this FileMapping is being 
//          entered, raise this counter in 1.
//
// Who   When         What
// ---   --------     -----
// SIV   28/01/02     Start
//////////////////////////////////////////////////////////////////////
long CCompChecker::DoReg(char *sId)
{
	long lRetVal;
	char *sComId;
	long *lAlreadyExist;
	HANDLE  hTmp;

	if (!m_hShareMemo)
	{
		lRetVal= OK;
		int iComIdLen = strlen(sId) +5;
		sComId = new char[iComIdLen];
		strncpy_s(sComId, iComIdLen, sId, strlen(sId)+1);

		hTmp = CreateFileMapping((HANDLE)0xFFFFFFFF,	// File handle
										  NULL,					// Security attributes
										  PAGE_READWRITE,		// Protection
										  0,					// Size - high 32 bits
										  1<<2,				// Size - low 32 bits
										  sComId);			// Name

		lRetVal = GetLastError ();

		// Can't creat file-mapping object
		if (!hTmp)
			// m_sComId matches the name of an existing event, semaphore, mutex, waitable timer,
			// or job, the function fails  
			if ((lRetVal==ERROR_INVALID_HANDLE) )
				//chnge the handle name and try again
				for(int i=0 ; i<MAX_LOOPS ; i++)
				{
					sprintf_s(sComId, iComIdLen, "_%d_%s",i,sId);
					hTmp = CreateFileMapping((HANDLE)0xFFFFFFFF,	// File handle
													NULL,					// Security attributes
													PAGE_READWRITE,			// Protection
													0,						// Size - high 32 bits
													1<<2,					// Size - low 32 bits
													sComId);				// Name
					lRetVal = GetLastError ();
					if (lRetVal == S_OK)
						break;
				}	

		//we got a file-mapping object handle
		if ((lRetVal == S_OK)||(lRetVal==ERROR_ALREADY_EXISTS))
		{
			lAlreadyExist = (long *)MapViewOfFile(hTmp,        // File mapping object
												FILE_MAP_ALL_ACCESS, // Read/Write
												0,                   // Offset - high 32 bits
												0,                   // Offset - low 32 bits
												0);                  // Map the whole thing
			if (lAlreadyExist)
			{
				if (lRetVal==ERROR_ALREADY_EXISTS)
					*lAlreadyExist =*lAlreadyExist + 1;
				else
					*lAlreadyExist = 0;
			
				m_hShareMemo= hTmp;
	
				UnmapViewOfFile(lAlreadyExist);
			}
			lRetVal = OK;
		}
		else
			lRetVal = CANNOT_CREATE_FILE_MAPPING;

		if (sComId)
			delete []sComId;
	}
	else
		lRetVal = DOREG_ALREADY_DONE;
	
	return lRetVal;	
}


//////////////////////////////////////////////////////////////////////
// FUNCTION NAME: DoUnReg
//
// DESCRIPTION: unregister a component by close its FileMapping.
//
// PARAMETERS: None
//
// RETURNS:  See return codes table.
//
// Who   When         What
// ---   --------     -----
// SIV   28/01/02     Start
//////////////////////////////////////////////////////////////////////
int CCompChecker::DoUnReg()
{
	BOOL bIsClosed;
	long *lCounter;

	if (m_hShareMemo)
	{
		lCounter = (long *)MapViewOfFile(m_hShareMemo,           // File mapping object
											FILE_MAP_ALL_ACCESS, // Read/Write
											0,                   // Offset - high 32 bits
											0,                   // Offset - low 32 bits
											0);                  // Map the whole thing
		if (lCounter)
		{
			if (*lCounter>0)
				*lCounter = *lCounter - 1;

			UnmapViewOfFile(lCounter);
		}

		bIsClosed = CloseHandle(m_hShareMemo);
		m_hShareMemo = NULL;
	}
		
	return bIsClosed ? OK : CANNOT_CLOSE_HANDLE ;
}

//////////////////////////////////////////////////////////////////////
// FUNCTION NAME: IsRunning
//
// DESCRIPTION: check if a component is running on the system. 
//
// PARAMETERS: sId[In]  - The component name (null terminated)
//
// RETURNS:  See return codes table.
//
// REMARKS: 1. first try to look for the relevant FileMapping. if not found, then try to 
//          find out if running with the function GetProgPID
//
// Who   When         What
// ---   --------     -----
// SIV   28/01/02     Start
//////////////////////////////////////////////////////////////////////
long CCompChecker::IsRunning(char *sId)
{
	BOOL bIsClosed;
	HANDLE  hComponent;
	long *lCounter;
	long lRetVal = OK;
	char *sComId;
	int i = 0; // CID_185863 Uninitialized scalar variable
	long lProcessCounter = 0;
	int iComIdLen = strlen(sId) +5;
	sComId = new char[iComIdLen];
	strncpy_s(sComId,iComIdLen, sId, strlen(sId)+1);

	hComponent = OpenFileMapping (FILE_MAP_ALL_ACCESS,true,sComId);
	
	if (!hComponent)
	{
		for(i = 0 ; i < MAX_LOOPS; i++)
		{
			sprintf_s(sComId, iComIdLen, "_%d_%s",i,sId);
			hComponent = OpenFileMapping (FILE_MAP_ALL_ACCESS,true,sComId);
			if (hComponent)
				break;
		}
	}
	
	if  ((i == MAX_LOOPS) && (!hComponent))
		lRetVal = NOT_RUNNING;
	else
		if (hComponent)
		{
			lCounter = (long *)MapViewOfFile(hComponent,        // File mapping object
											FILE_MAP_ALL_ACCESS, // Read/Write
											0,                   // Offset - high 32 bits
											0,                   // Offset - low 32 bits
											0);                  // Map the whole thing
			if (lCounter)
			{
				if (*lCounter == 0)
					lRetVal = RUNNING;
				else
					lRetVal = MORE_THEN_ONE_INSTANCE;

				UnmapViewOfFile(lCounter);
			}
			else
				lRetVal = CANNOT_MAP_VIEW_OF_FILE;
			bIsClosed = CloseHandle(hComponent);
		}
		else
			lRetVal = GENERIC_ERROR;

	// if the lRetVal shows that it isn't running then
	// there are 2 options 1)it is realy not running 2)there is no shared memory
	// so we retry checking in a different way
	if (!((lRetVal == RUNNING)||(lRetVal ==MORE_THEN_ONE_INSTANCE)))
	{
		if (sComId)
			delete []sComId;

		if (strlen(sId) > 6)
		{
			iComIdLen = 7;
			sComId = new char[iComIdLen];  // we will try to look in the processes 
							   // list according to 6 first signs
			strncpy_s(sComId, iComIdLen, sId , 6);
			sComId[6]=0;
		}
		else
		{
			iComIdLen = strlen(sId)+1;
			sComId = new char[iComIdLen];
			strncpy_s(sComId,iComIdLen, sId,strlen(sId)+1);
		}
	
		lRetVal = GetProgPID(sComId,lProcessCounter);

		if (lRetVal!=-1)
			if (lProcessCounter>1)
				lRetVal = MORE_THEN_ONE_INSTANCE;
			else
				lRetVal = RUNNING;
		else
			lRetVal = NOT_RUNNING;	
	}

	
	if (sComId)
		delete []sComId;

	return lRetVal;
}


/***************************************************************************
 DESCRIPTION: Get list of processes running on system

 PARAMETERS :  pNum  - will retun number of processes found

 RETURNS    :  list of processes. NULL if faild

Who   When     What
---   -------- ----
YKL   22/03/99 Start
*****************************************************************************/
NT_PROC_INFO *CCompChecker::NTProcList(int *pNum)
{
   HINSTANCE      hPSAPILib ;
   HINSTANCE      hVDMDBGLib ;
   LPDWORD        lpdwPIDs ;
   DWORD          dwSize, dwSize2, dwIndex ;
   HMODULE        hMod ;
   HANDLE         hProcess ;
   char           szFileName[ MAX_PATH ] ;
   NT_PROC_INFO   *pProcList=NULL,  // Process list
                  *pProc;           // current process
   // PSAPI Function Pointers.
   // ------------------------
   lpf_ENUM_PROCESSES         lpfEnumProcesses;
   lpf_ENUM_PROCESS_MODULES   lpfEnumProcessModules;
   lpf_GET_MODULE_FILENAME_EX lpfGetModuleFileNameEx;
   lpf_VDM_ENUM_TASK_WOW_EX   lpfVDMEnumTaskWOWEx;

   *pNum = 0;

   // Dynamic Load of DLL's
   if ( !LoadDllLibs(&hPSAPILib, &hVDMDBGLib) )
      return(0);
            
   // Get procedure addresses.
   // ------------------------
   if ( !GetProcAddr(hPSAPILib, hVDMDBGLib,
                     &lpfEnumProcesses, &lpfEnumProcessModules,
                     &lpfGetModuleFileNameEx, &lpfVDMEnumTaskWOWEx) )
   {
      FreeLibrary( hPSAPILib ) ;
      FreeLibrary( hVDMDBGLib ) ;
      return 0;
   }


   // Call the PSAPI function EnumProcesses to get all of the
   // ProcID's currently in the system.
   // NOTE: In the documentation, the third parameter of
   // EnumProcesses is named cbNeeded, which implies that you
   // can call the function once to find out how much space to
   // allocate for a buffer and again to fill the buffer.
   // This is not the case. The cbNeeded parameter returns
   // the number of PIDs returned, so if your buffer size is
   // zero cbNeeded returns zero.
   // NOTE: The "HeapAlloc" loop here ensures that we
   // actually allocate a buffer large enough for all the
   // PIDs in the system.

   dwSize2 = 256 * sizeof( DWORD ) ;
   lpdwPIDs = NULL ;
   do
   {
      if( lpdwPIDs )
      {
         HeapFree( GetProcessHeap(), 0, lpdwPIDs ) ;
         dwSize2 *= 2 ;
      }

      lpdwPIDs = (unsigned long *)HeapAlloc( GetProcessHeap(), 0, dwSize2 );
      if( lpdwPIDs == NULL )
      {
         FreeLibrary( hPSAPILib ) ;
         FreeLibrary( hVDMDBGLib ) ;
         return 0;
      }
      
      if( !lpfEnumProcesses( lpdwPIDs, dwSize2, &dwSize ) )
      {
         HeapFree( GetProcessHeap(), 0, lpdwPIDs ) ;
         FreeLibrary( hPSAPILib ) ;
         FreeLibrary( hVDMDBGLib ) ;
         return 0;
      }
   }while( dwSize == dwSize2 ) ;

   // How many ProcID's did we get?
   dwSize /= sizeof( DWORD ) ;
   
   if ( dwSize )
      pProcList = (NT_PROC_INFO *)calloc(sizeof(NT_PROC_INFO),dwSize);

   // Loop through each ProcID.
   for( dwIndex = 0 ; dwIndex < dwSize ; dwIndex++ )
   {
      memset (szFileName, 0, sizeof(szFileName));
      pProc = pProcList+dwIndex; // point to current process in table
      
      pProc->lProcID = lpdwPIDs[dwIndex];

      // Open the process (if we can... security does not
      // permit every process in the system).
      hProcess = OpenProcess(
         PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
         FALSE, lpdwPIDs[ dwIndex ] ) ;
      if( hProcess != NULL )
      {
         // Here we call EnumProcessModules to get only the
         // first module in the process this is important,
         // because this will be the .EXE module for which we
         // will retrieve the full path name in a second.
         if( lpfEnumProcessModules( hProcess, &hMod,
            sizeof( hMod ), &dwSize2 ) )
         {
            // Get Full pathname:
            if( !lpfGetModuleFileNameEx( hProcess, hMod,
               szFileName, sizeof( szFileName ) ) )
            {
               szFileName[0] = 0 ;
            }
         }
         CloseHandle( hProcess ) ;
      }
      // Regardless of OpenProcess success or failure, we
      // still call the enum func with the ProcID.
      pProc->lProcID = lpdwPIDs[dwIndex];
      strncpy_s(pProc->szProcPath, _countof(pProc->szProcPath), szFileName, NT_PROC_PATH_LEN-1);
   }

   HeapFree( GetProcessHeap(), 0, lpdwPIDs ) ;
   FreeLibrary( hVDMDBGLib ) ;

   (*pNum) = dwSize;
   return pProcList;         
}


/***************************************************************************
 DESCRIPTION: Check if program is running

 PARAMETERS :  lpstrProgName  - program name to look for
			   lCounter [out] - if lPID!=-1 then lCounter will count how many 
                                times the process is running	

 RETURNS    :  process id of the program. -1 if not found

Who   When     What
---   -------- ----
YKL   22/03/99 Start

SIV	  24/01/02 add lCounter. 
*****************************************************************************/
long CCompChecker::GetProgPID(LPCSTR lpstrProgName,long &lCounter)
{
   NT_PROC_INFO      *pProc;        // will hold processes list
   int               iNumProc=0;    // number of process in list
   int               i;             // index
   long              lPID=-1;        // return value
   char              *p;

   lCounter = 0;
   pProc = NTProcList(&iNumProc);
   if ( !pProc )
      return -1;

   for (i=0; i<iNumProc ; i++)
   {
      // point to program name
      if ( !(p=strrchr(pProc[i].szProcPath, '\\')) )
         p = pProc[i].szProcPath;
      else
         p++;

      if ( !_strnicmp(p, lpstrProgName,strlen(lpstrProgName)) )
	  {
		  lCounter++;
		  lPID = pProc[i].lProcID;
	  }
         
   }

   free (pProc);

   return lPID;
}

/***************************************************************************
 DESCRIPTION: Connect to DLL's required for getting list

 PARAMETERS :  phPSAPILib     - pointer to PSAPI instance
               phVDNDBGLib    - pointer to VDMDBG instance

 RETURNS    : TRUE   - success

Who   When     What
---   -------- ----
YKL   22/03/99 Start
*****************************************************************************/
BOOL CCompChecker::LoadDllLibs(HINSTANCE *phPSAPILib, HINSTANCE *phVDMDBGLib)
{
   *phPSAPILib = LoadLibraryA( "PSAPI.DLL" ) ;
   if( (*phPSAPILib) == NULL )
      return FALSE ;

   *phVDMDBGLib = LoadLibraryA( "VDMDBG.DLL" ) ;
   if( (*phVDMDBGLib) == NULL )
      return FALSE ;
      
   return(TRUE);
}   

/***************************************************************************
 DESCRIPTION: Get addresses of functions in DLL's

 PARAMETERS :

 RETURNS    : 

Who   When     What
---   -------- ----
YKL   22/03/99 Start
*****************************************************************************/
BOOL CCompChecker::GetProcAddr(HINSTANCE hPSAPILib, 
                 HINSTANCE hVDMDBGLib,
                 lpf_ENUM_PROCESSES *lpfEnumProcesses, 
                 lpf_ENUM_PROCESS_MODULES *lpfEnumProcessModules,
                 lpf_GET_MODULE_FILENAME_EX *lpfGetModuleFileNameEx, 
                 lpf_VDM_ENUM_TASK_WOW_EX *lpfVDMEnumTaskWOWEx)
{
   *lpfEnumProcesses       = (lpf_ENUM_PROCESSES)GetProcAddress( hPSAPILib, "EnumProcesses" ) ;
   *lpfEnumProcessModules  = (lpf_ENUM_PROCESS_MODULES)GetProcAddress( hPSAPILib,"EnumProcessModules" ) ;
   *lpfGetModuleFileNameEx =(lpf_GET_MODULE_FILENAME_EX)GetProcAddress( hPSAPILib,"GetModuleFileNameExA" ) ;
   *lpfVDMEnumTaskWOWEx    =(lpf_VDM_ENUM_TASK_WOW_EX)GetProcAddress( hVDMDBGLib, "VDMEnumTaskWOWEx" );

   return ( (*lpfEnumProcesses)        &&
            (*lpfEnumProcessModules)   &&
            (*lpfGetModuleFileNameEx)  &&
            (*lpfVDMEnumTaskWOWEx)
            );
}

/***************************************************************************
 DESCRIPTION:	Get name of program and kills it

 PARAMETERS :	Program name. can be only part of the name 
				(e.g. "BuyAtP" instead of "BuyAtPump")

 RETURNS    :   true if succees killing, else false

Who		When		What
---		--------	----
Sivan   23/01/03	Start
*****************************************************************************/
bool CCompChecker::KillProcess(LPCSTR lpstrProgName)
{
	bool bRet=false;
	long lCounter=0;
	HANDLE hProcess;

	// first time need to get the number of programs with this name that run now.
	long dwProcId= GetProgPID(lpstrProgName,lCounter);

	while ((lCounter > 0) && (dwProcId != -1))
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcId);

		if ( hProcess )
			if (TerminateProcess(hProcess,0)) 
			{
				if (lCounter == 1)
				{
					// we succeed kill it.
					 bRet=  true; 
					 break;
				}
				else
					// we have another one so we get it and kill it
					dwProcId= GetProgPID(lpstrProgName,lCounter);
			}
			else
				break;
		else
			break;
	}

	 return bRet;
}
/******************************************************************************
 Description:	Checks is the components already running. The method creates a 
				unique mutex for the component. If another nstance exist the creation
				of this mutex will fail.
 Returns:      	BOOL - TRUE - Already running, FALSE - not running
 Parameters:	char *pszMutexName - component unique mutex name.
				The name should be 'CompName' + Mutex, e.g. PumpSrvMutex.
				
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			28/1/2004   10:37		Start
******************************************************************************/
BOOL CCompChecker::IsAlreadyRunning(char *pszMutexName)
{
	BOOL bIsAlreadyRunning = FALSE;	
	
	m_hMutex = CreateMutex (NULL, FALSE, pszMutexName);
    if ((m_hMutex != NULL) && (GetLastError () == ERROR_ALREADY_EXISTS)) 
    {
        CloseHandle (m_hMutex);
		m_hMutex = NULL;
        bIsAlreadyRunning = TRUE ;
    }
	
	return bIsAlreadyRunning;
}


/******************************************************************************
 Description:	Checks is the components already running. The method try to open a 
				unique mutex for the component. If another instance exist the open of this mutex will fail.
 Returns:      	BOOL - TRUE - Already running, FALSE - not running
 Parameters:	char *pszMutexName - component unique mutex name.
				The name should be 'CompName' + Mutex, e.g. PumpSrvMutex.
				
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			08/06/2010   10:48		Start				//1.0.25.20 TD 100291 merge from PumpSrv 1026
******************************************************************************/
BOOL CCompChecker::IsAlreadyRunningWithoutCreateMutex(char *pszMutexName)
{
	BOOL bIsAlreadyRunning = FALSE;
	DWORD dwGLE = 0;
		
	m_hMutex = OpenMutex (NULL, FALSE, pszMutexName);
	dwGLE = GetLastError ();

	if ((m_hMutex != NULL))
	{
		ReleaseMutex(m_hMutex);
		CloseHandle (m_hMutex);
		m_hMutex = NULL;		
	}

	if (dwGLE == ERROR_ACCESS_DENIED)
		bIsAlreadyRunning = TRUE;
	
	return bIsAlreadyRunning;
}
