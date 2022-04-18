// ConfigurationThread.cpp: implementation of the CConfigurationThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigurationThread.h"


//--------------------------------------------------------------------
//				class CConfigurationLoader		//4.0.22.43 TD 72716
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigurationLoader::CConfigurationLoader()
{

}

CConfigurationLoader::~CConfigurationLoader()
{

}


//////////////////////////////////////////////////////////////////////
//					Main method
//////////////////////////////////////////////////////////////////////
void CConfigurationLoader::ReLoad()
{
	if(IsReInitRequired() == TRUE)
	{
		ReInit();
	}
	else
	{
		CConfigurationLoader* pcParent = NULL;

		pcParent = GetChild();
		if(NULL != pcParent)
			pcParent->ReLoad();	
	}
}


//---------------------------------------------------------------------
//						class CTimer  
//---------------------------------------------------------------------
//4.0.22.45 TD 72716
//04/05/09 11:23

CTimer::CTimer()
{
	ResetWaitTime();
}

void CTimer::InitWaitTime()
{ 
	if(TRUE == m_bTimerNotActive)
	{
		GetLocalTime(&m_tStartWaitTime); 
		m_bTimerNotActive = FALSE;
	}
}


BOOL CTimer::IsWaitTimePassed(const long lTimeOutVal)
{ 
	return _Module.m_server.m_cCommonFunction.IsTimeOutPassed(m_tStartWaitTime, lTimeOutVal);
}


void CTimer::ResetWaitTime()
{ 
	m_bTimerNotActive = TRUE;
	memset(&m_tStartWaitTime, 0, sizeof(SYSTEMTIME));
}


//--------------------------------------------------------------------
//				class CFileMonitor 		//4.0.23.504 TD 78942
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileMonitor::CFileMonitor()
					
{
	m_hDirHandle = m_hFileHandle = INVALID_HANDLE_VALUE;
	m_pDirNotifyInfo = NULL;
}


CFileMonitor::~CFileMonitor()
{}


//////////////////////////////////////////////////////////////////////
//						Initialization methods
//////////////////////////////////////////////////////////////////////
void CFileMonitor::Init(const BYTE* szDirectoryPath, const HANDLE hNotifyEvent)
{
	InitDirInfo(szDirectoryPath, hNotifyEvent);
	InitFileInfo();	
}


//private:
void CFileMonitor::InitDirInfo(const BYTE* szDirectoryPath, const HANDLE hNotifyEvent)
{	
	// Init m_szDirPath
	if ( NULL != szDirectoryPath )
	{
		memset( m_szDirPath, 0, sizeof(m_szDirPath) );	
		memcpy( m_szDirPath, szDirectoryPath, min( sizeof(m_szDirPath), strlen((const char *)szDirectoryPath) ) );

		CreateMonitorDir();
		OpenMonitorDir();
	}

	// Init m_sDirOverlap
	memset( &m_sDirOverlap, 0, sizeof(m_sDirOverlap) );
	if ( IsValidHandle( hNotifyEvent ) == TRUE )
	{
		m_sDirOverlap.hEvent = hNotifyEvent;
	}

	// Init Dir info
	memset( m_szDirInfoBuff, 0, sizeof(m_szDirInfoBuff) );	
	m_dwDirInfoLength	= 0;
	m_pDirNotifyInfo	= ( FILE_NOTIFY_INFORMATION* ) m_szDirInfoBuff;	
}


//private:
BOOL CFileMonitor::CreateMonitorDir() const
{
	BOOL	bRetCode = TRUE;
	CString	strLogMsg;

	bRetCode = CreateDirectory( (const char*)m_szDirPath, NULL );

	// LOG
	CString		strSuccessfulLogMsg;
	strSuccessfulLogMsg.Format( "Directory [%s] created.", m_szDirPath );
	
	WriteLogMsg( bRetCode, strSuccessfulLogMsg, "");

	return bRetCode;
}


//private:
BOOL CFileMonitor::OpenMonitorDir() 
{
	BOOL	bRetCode = TRUE;
	CString	strLogMsg;

	m_hDirHandle = CreateFile(	(const char*)m_szDirPath,								// pointer to the file name,directoryName for the directory to be monitored
								FILE_LIST_DIRECTORY,										// access (read/write) mode
								FILE_SHARE_READ|FILE_SHARE_DELETE,							// share mode
								NULL,														// security descriptor
								OPEN_EXISTING,												// how to create
								FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,			// file attributes
								NULL														// file with attributes to copy
						);

	// LOG
	CString		strSuccessfulLogMsg;
	CString		strFailedMsg;

	strSuccessfulLogMsg.Format( "Directory [%s] opened.", m_szDirPath );
	strFailedMsg.Format( "Unable to open directory [%s]!", m_szDirPath );
	
	WriteLogMsg( IsValidHandle(m_hDirHandle), strSuccessfulLogMsg, strFailedMsg );

	return bRetCode;
}


//private:
void CFileMonitor::InitFileInfo()
{
	if ( IsValidHandle( m_hFileHandle ) == TRUE )
		m_hFileHandle	= INVALID_HANDLE_VALUE;	

	// create find specification
	memset( m_szFindSpec, 0, sizeof(m_szFindSpec) );	
	memcpy( m_szFindSpec, m_szDirPath, min( sizeof(m_szDirPath), sizeof(m_szFindSpec) ) );
	strcat( (char*)m_szFindSpec, "\\*");						// all
	
	// init find file info
	memset( &m_sFindFileInfo, 0, sizeof(m_sFindFileInfo) );
}



//////////////////////////////////////////////////////////////////////
//						FileData methods
//////////////////////////////////////////////////////////////////////

BOOL CFileMonitor::GetFileData(BYTE** pszFileData, size_t* piDataLength)
{
	BOOL	bRetCode = FALSE;

	if ( NULL != piDataLength )
	{	
		CXMLInterface cXMLInterface;

		cXMLInterface.InitDoc();

		bRetCode = ReadFromFile( cXMLInterface );

		if( TRUE == bRetCode )
		{
			char szXmlString[ENLARGE_XML_BUF] = { 0 };

			bRetCode = cXMLInterface.GetXmlString( szXmlString );
			
			if ( bRetCode == TRUE )
			{
				*piDataLength = strlen( szXmlString ) + 1;

				if ( NULL == *pszFileData )
				{
					*pszFileData = new BYTE[ *piDataLength ];
				}

				memcpy( *pszFileData, szXmlString, *piDataLength);
			}
		}

		cXMLInterface.CloseDoc();
	}

	return bRetCode;
}

//////////////////////////////////////////////////////////////////////
//						File Methods
//////////////////////////////////////////////////////////////////////
BOOL CFileMonitor::IsNewFileAdded() 
{
	BOOL bRetCode = FALSE;
		
	bRetCode = ( m_pDirNotifyInfo->Action == FILE_ACTION_ADDED ) ? TRUE : FALSE;

	return bRetCode;
}


void CFileMonitor::LookForFile()
{
	ReadDirectoryChangesW(	m_hDirHandle, 
							m_szDirInfoBuff,	
							sizeof(m_szDirInfoBuff), 
							FALSE, 
							FILE_NOTIFY_CHANGE_FILE_NAME, 
							&m_dwDirInfoLength, 
							&m_sDirOverlap, 
							NULL	);
}


BOOL CFileMonitor::FindFile()
{
	BOOL bRetCode = FALSE;

	if ( IsValidHandle( m_hFileHandle ) == TRUE )
	{
		bRetCode = FindNextFile( m_hFileHandle, &m_sFindFileInfo );
	}
	else
	{
		m_hFileHandle = FindFirstFile( (LPCSTR)m_szFindSpec, &m_sFindFileInfo );

		if ( IsValidHandle( m_hFileHandle ) == TRUE )
		{			
			if ( memcmp( m_sFindFileInfo.cFileName, ".", 1 ) == OK )
			{
				bRetCode = FindNextFile( m_hFileHandle, &m_sFindFileInfo );

				if ( memcmp( m_sFindFileInfo.cFileName, "..", 2 ) == OK )
					bRetCode = FindNextFile( m_hFileHandle, &m_sFindFileInfo );
			}
		}
		else
			bRetCode = FALSE;
	}

	if ( FALSE == bRetCode )
	{
		ClearFileInfo();		
	}

	// LOG
	CString		strSuccessfulLogMsg;
	strSuccessfulLogMsg.Format( "Found new file [%s].", m_sFindFileInfo.cFileName );
		
	WriteLogMsg( bRetCode, strSuccessfulLogMsg, "");

	return bRetCode;
}
		

void CFileMonitor::DeleteCurrFile()
{
	BOOL		bRetVal = FALSE;
	BYTE		szFullFileName[MAX_PATH] = {0};

	GenerateFullFileName( szFullFileName );

	ClearReadOnlyAttr( szFullFileName );

	bRetVal = DeleteFile( (LPCTSTR)szFullFileName );
	
	// LOG
	CString		strSuccessfulLogMsg;
	CString		strFailedMsg;

	strSuccessfulLogMsg.Format( "File [%s] deleted.", szFullFileName );
	strFailedMsg.Format( "File [%s] wasn't deleted!", szFullFileName );
	
	WriteLogMsg( bRetVal, strSuccessfulLogMsg, strFailedMsg);	
	
	ClearFileInfo();
}

//CR 407467
void CFileMonitor::MoveCurrFile()
{
	BOOL		bRetVal = FILE_ALREADY_EXISTS;
	BYTE		szFullFileName[MAX_PATH] = {0};
	BYTE		szNewFullFileName[MAX_PATH] = {0};
	
	GenerateFullFileName( szFullFileName );
	GenerateNewFullFileName( szNewFullFileName );
	ClearReadOnlyAttr( szFullFileName );

	while (bRetVal == FILE_ALREADY_EXISTS)
	{
		bRetVal = MoveFile( (LPCTSTR)szFullFileName , (LPCTSTR)szNewFullFileName );
		bRetVal = GetLastError();
		strcat((char*)szNewFullFileName,"_");
	}
	// LOG
	CString		strSuccessfulLogMsg;
	CString		strFailedMsg;

	strSuccessfulLogMsg.Format( "File [%s] moved.", szFullFileName );
	strFailedMsg.Format( "File [%s] wasn't moved!", szFullFileName );

	WriteLogMsg( bRetVal, strSuccessfulLogMsg, strFailedMsg);	

	ClearFileInfo();
}

//private
void CFileMonitor::ClearReadOnlyAttr(BYTE* szFullFileName) const
{
	DWORD		dwAttrs = 0x0, 
				dwClearReadOnly = ~FILE_ATTRIBUTE_READONLY;

	// set file attribute 'Read only' to 0
	dwAttrs = GetFileAttributes( (LPCSTR)szFullFileName ); 
    if ( dwAttrs & FILE_ATTRIBUTE_READONLY ) 
    { 
        SetFileAttributes( (LPCSTR)szFullFileName, dwAttrs & dwClearReadOnly ); 
    } 
}


//private
BOOL CFileMonitor::ReadFromFile(CXMLInterface& cXMLInterface) const
{
	BOOL	bRetCode = FALSE;
	BYTE	szFullFileName[MAX_PATH] = {0};

	GenerateFullFileName( szFullFileName );
	bRetCode = cXMLInterface.LoadFile( (char*)szFullFileName );	

	return bRetCode;
}


//private
void CFileMonitor::GenerateFullFileName(BYTE* szFullFileName) const
{
	memcpy(szFullFileName, m_szDirPath, strlen( (const char*)m_szDirPath ) );
	strcat((char*)szFullFileName, "\\");
	strcat((char*)szFullFileName, (const char*)m_sFindFileInfo.cFileName);
}

void CFileMonitor::GenerateNewFullFileName(BYTE* szFullFileName) const
{
	//build the path to build new folder
	char tmpStr[100];
	char szPumpSrvPath[MAX_FIELD_NAME];
	_Module.m_server.m_cParam.LoadParam("Paths\\Long", "LogDir",tmpStr,sizeof(tmpStr), szPumpSrvPath,TRUE);
	char path[MAX_FIELD_VAL] = "\\Log\\Unprocessed Files";
	memcpy(szFullFileName, tmpStr, strlen( (const char*)tmpStr ) );
	strcat((char*)szFullFileName, (const char*)path);
	
	CreateDirectory((char*)szFullFileName,NULL);
	
	//attach the file name to the path
	strcat((char*)szFullFileName, "\\");
	strcat((char*)szFullFileName, (const char*)m_sFindFileInfo.cFileName);
}



//////////////////////////////////////////////////////////////////////
//						LOG methods
//////////////////////////////////////////////////////////////////////
void CFileMonitor::WriteLogMsg(BOOL bSuccessMsg, const CString& strSuccessfulMsg, const CString& strFailedMsg, long lFailedLogLevel) const
{
	if (LOG_BASIC_FLOW_CONTROL)
	{
		BOOL		bEmptyMsg = FALSE;
		CString		strLogMsg("CFileMonitor, ");
		long		lLogLevel = DEFAULT_LOG_LEVEL;

		if ( TRUE == bSuccessMsg )
		{
			bEmptyMsg = strSuccessfulMsg.IsEmpty();
			strLogMsg += strSuccessfulMsg;
		}
		else 
		{
			bEmptyMsg = strFailedMsg.IsEmpty();
			strLogMsg += strFailedMsg;

			if ( lFailedLogLevel != DEFAULT_LOG_LEVEL)
			{
				CString strGLE;
				DWORD	dwLastErr = GetLastError();

				strGLE.Format(" GLE = %ld.", dwLastErr);

				strLogMsg	+= strGLE;			

				lLogLevel = lFailedLogLevel;
			}
		}

		if ( FALSE == bEmptyMsg )
			_LOGMSG.LogMsg( strLogMsg,  lLogLevel);
	}
}


//////////////////////////////////////////////////////////////////////
//						Cleanup Methods
//////////////////////////////////////////////////////////////////////
void CFileMonitor::CleanUp()
{
	ClearDirInfo();
	ClearFileInfo();
}


//private:
void CFileMonitor::ClearDirInfo()
{
	if( IsValidHandle(m_hDirHandle) == TRUE )
	{
		CloseHandle( m_hDirHandle );
		m_hDirHandle = INVALID_HANDLE_VALUE;
	}

	memset( &m_sDirOverlap, 0, sizeof(m_sDirOverlap) );
	memset( m_szDirInfoBuff, 0, sizeof(m_szDirInfoBuff) );	

	m_pDirNotifyInfo	= NULL;
	m_dwDirInfoLength	= 0;
}


//private:
void CFileMonitor::ClearFileInfo()
{
	if( IsValidHandle(m_hFileHandle) == TRUE )
	{
		FindClose( m_hFileHandle );
		m_hFileHandle = INVALID_HANDLE_VALUE;
	}

	memset( &m_sFindFileInfo, 0, sizeof( m_sFindFileInfo ) );
}



//--------------------------------------------------------------------
//				class CMultipleEvents 		//4.0.23.504 TD 78942
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultipleEvents::CMultipleEvents() :	m_pahEvents(NULL),
										m_iMaxEventIndex(0),
										m_bFlagSignaled(FALSE)
{	
}


CMultipleEvents::~CMultipleEvents()
{
	CleanUp();
}


/******************************************************************************
 Description:	init events array by size
 Returns:      	void
 Parameters:   	iEventsNum	- events array size
				bManualReset - property for all events, by default = FALSE
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			04/10/2009	 09:11		Start //4.0.23.504 TD 78942
******************************************************************************/
void CMultipleEvents::Init(const size_t iEventsNum, const BOOL bManualReset)
{
	if ( NULL == m_pahEvents )
	{
		m_pahEvents = new HANDLE[iEventsNum];
		m_iMaxEventIndex = iEventsNum;
	
		for (int i = 0; i < iEventsNum; i++)
		{
			m_pahEvents[i] = INVALID_HANDLE_VALUE;
			m_pahEvents[i] = CreateEvent(NULL, bManualReset, FALSE, NULL);		
		}
	}
}


/******************************************************************************
 Description:	Wait for some event
 Returns:      	event index was signaled
 Parameters:   	lMilliseconds - waiting time out
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			04/10/2009	 09:21		Start //4.0.23.504 TD 78942
******************************************************************************/
long CMultipleEvents::WaitForSome(const long lMilliseconds)
{
	long lRetEventIndex = -1;

	lRetEventIndex = WaitForMultipleObjects( m_iMaxEventIndex, m_pahEvents, FALSE, lMilliseconds);

	if ( lRetEventIndex >= 0 && lRetEventIndex < m_iMaxEventIndex)
	{
		m_bFlagSignaled = TRUE;
	}

	return lRetEventIndex;
}


HANDLE CMultipleEvents::GetEvent(const long lIndex)
{
	return m_pahEvents[lIndex];
}


inline BOOL	 CMultipleEvents::EventSignaled() const
{
	return m_bFlagSignaled;
}


inline void	 CMultipleEvents::ClearSignaled()
{
	m_bFlagSignaled = FALSE;
}

/******************************************************************************
 Description:	Set event by index
 Returns:      	success - TRUE
				fail - FALSE
 Parameters:   	lIndexForWait - event index for set
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			04/10/2009	 09:29		Start //4.0.23.504 TD 78942
******************************************************************************/
BOOL CMultipleEvents::SetOne(const long lIndexForWait)
{
	BOOL bRetCode = FALSE;

	if ( NULL != m_pahEvents && IsValidHandle( m_pahEvents[lIndexForWait] ) )
		bRetCode = SetEvent( m_pahEvents[lIndexForWait] );

	return bRetCode;
}

/******************************************************************************
 Description:	Clean up 
 Returns:      	void
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			04/10/2009	 09:34		Start //4.0.23.504 TD 78942
******************************************************************************/
void CMultipleEvents::CleanUp()
{

	if (NULL != m_pahEvents)
	{
		for (int i = 0; i < m_iMaxEventIndex; i++)
		{
			if ( IsValidHandle( m_pahEvents[i] ) )
			{
				CloseHandle( m_pahEvents[i] );
			}
		}

		delete [] m_pahEvents;
		m_pahEvents = NULL;
	}

	m_iMaxEventIndex = 0;
}


//--------------------------------------------------------------------
//				class CConfigurationThread		//4.0.22.43 TD 72716
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigurationThread::CConfigurationThread() : CThread(FALSE, THREAD_SLEEP_DURATION, FALSE, THREAD_PRIORITY_BELOW_NORMAL),
												m_dwWaitForDataInterval(INFINITE),
												m_bThreadAlive(FALSE),
												m_iThreadDataLength(0),
												m_pszThreadData(NULL)
{	
	InitializeCriticalSection(&m_csThreadData);
}


CConfigurationThread::~CConfigurationThread()
{
	DeleteCriticalSection(&m_csThreadData);
}


/******************************************************************************
 Description:	Load ConfigurationThread Configuration
 Returns:      	void
 Parameters:   	bCreate - TRUE create if not exist
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/05/2009	 13:34		Start //4.0.22.45 TD 72716
******************************************************************************/
void CConfigurationThread::LoadConfiguration(const BOOL bCreate)
{
	// Active
	DWORD dwThreadAlive = 0;
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "ConfigThreadAlive", &dwThreadAlive, (DWORD)0, bCreate);			// 0 - THREAD NOT ACTIVE
	m_bThreadAlive = (dwThreadAlive) ? TRUE : FALSE;

	// Thread Timeout
	CThread::SetSleepDuration( THREAD_SLEEP_DURATION );		

	// ThreadPriority
	DWORD dwThreadPriority= 0;
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "ConfigThreadPriority", &dwThreadPriority, (DWORD)THREAD_PRIORITY_BELOW_NORMAL, bCreate);
	CThread::SetThreadPriority(dwThreadPriority);

	// Lock TimeOut
	DWORD dwLockTimeOut = 0;
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "ConfigThreadLockTimeOut(Min)", &dwLockTimeOut, (DWORD)DEFAULT_LOCK_TIMEOUT, bCreate);
	m_dwLockTimeOut = MINUTES_TO_MILISECONDS(dwLockTimeOut);

	// Look for data file
	DWORD dwLookForDataFile = 0;
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "ConfigThreadLookForDataFile", &dwLookForDataFile, (DWORD)0, bCreate);			// 0 - DON'T LOOK FOR DATA FILE
	m_bLookForDataFile = (dwLookForDataFile) ? TRUE : FALSE;

	// Data file path
	char szDataFilePath[MAX_FILE_PATH] = {0};
	_Module.m_server.m_cParam.LoadParam("Paths\\Long", "ConfigThreadDataFilePath", szDataFilePath, sizeof(szDataFilePath), "", bCreate);
	memcpy( m_szDataFilePath, szDataFilePath, min(sizeof(m_szDataFilePath), sizeof(szDataFilePath)) );

}


/******************************************************************************
 Description:	Initialize ConfigurationThread
 Returns:      	void
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/05/2009	 13:34		Start //4.0.22.45 TD 72716
******************************************************************************/
CThreadRetCode CConfigurationThread::Init()
{
	CThreadRetCode eThreadRetCode = THREAD_OK; 

	if( IsThreadAlive() == TRUE )
	{		
		eThreadRetCode = CThread::CreateThread();
		if( eThreadRetCode == THREAD_OK )
		{			
			if ( IsDataFileInUse() == TRUE )			//4.0.23.504 TD 78942
			{
				m_cMultipleEvents.Init( CTEI_MAX_INDEX, FALSE );
				m_cFileMonitor.Init( m_szDataFilePath, m_cMultipleEvents.GetEvent( CTEI_FILE ) );			
			}

			m_cStateMachine.Init();

			_LOGMSG.LogMsg("ConfigurationThread, Thread was created"); 
		}
		else
		{
			_LOGMSG.LogMsg("ConfigurationThread, Thread wasn't created");
		}
	}
	else
	{
		_LOGMSG.LogMsg("ConfigurationThread, Thread wasn't created, ThreadActive is 0.");
		eThreadRetCode = THREAD_NOT_CREATED;
	}

	return eThreadRetCode;
}


/******************************************************************************
 Description:	Main method ThreadRoutine
 Returns:      	void
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/05/2009	 13:34		Start //4.0.22.45 TD 72716
******************************************************************************/
void CConfigurationThread::ThreadRoutine(void* pData)
{
	m_cStateMachine.Operation(this);
}


/******************************************************************************
 Description:	Apply ThreadData to the registry
 Returns:      	success - TRUE
				fail - FALSE
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/05/2009	 13:34		Start //4.0.22.45 TD 72716
******************************************************************************/
BOOL CConfigurationThread::ApplyData()
{
	BOOL bRetCode = FALSE;

	EnterCriticalSection(&m_csThreadData);

	bRetCode = XMLData2RegTable();		//set XML to regisry	
		
	SaveConfigurationData();

	ClearThreadData();

	LeaveCriticalSection(&m_csThreadData);

	return bRetCode;
}


/******************************************************************************
 Description:	Lock Forecourt
 Returns:      	void
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/05/2009	 13:34		Start //4.0.22.45 TD 72716
******************************************************************************/
void CConfigurationThread::LockForecourt() const
{
	if (IsLockForecourtComplete() == FALSE)
	{
		_Module.m_server.m_cServerInfo.SetRequiredMaintenanceProcessFlags(MP_LOCK);
	}
}


/******************************************************************************
 Description:	UnLock Forecourt
 Returns:      	void
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/05/2009	 13:34		Start //4.0.22.45 TD 72716
******************************************************************************/
void CConfigurationThread::UnLockForecourt() const
{
	if (IsUnLockForecourtComplete() == FALSE)
	{
		_Module.m_server.m_cServerInfo.SetRequiredMaintenanceProcessFlags(MP_UNLOCK);
	}
}


/******************************************************************************
 Description:	Force UnLock Forecourt
 Returns:      	void
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/05/2009	 13:34		Start //4.0.22.45 TD 72716
******************************************************************************/
void  CConfigurationThread::ForceUnLockForecourt() const
{
	if (IsUnLockForecourtComplete() == FALSE)
	{
		_Module.m_server.m_cServerInfo.SetRequiredMaintenanceProcessFlags(MP_FORCED_UNLOCK);
	}
}


/******************************************************************************
 Description:	Reconfigure
 Returns:      	void
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/05/2009	 13:34		Start //4.0.22.45 TD 72716
******************************************************************************/
void CConfigurationThread::Reconfigure() 
{
	long	lRetVal		= OK;

	while ( m_cConfigData.IsReconfigDataEmpty() == FALSE )
	{
		long	lTableId		= 0;
		long	lSubTableId		= 0;
		long	lSubTableIdExt	= 0;
		long	lFlags			= 0;
		BOOL	bBackUp			= FALSE;

		m_cConfigData.GetReconfigData(&lTableId, &lSubTableId, &lSubTableIdExt, &lFlags, &bBackUp);
		_Module.m_server.m_cCommonFunction.Reconfigure2(lTableId, lSubTableId, lSubTableIdExt, lFlags, &lRetVal, bBackUp);		//4.0.22.72 TD 76419
	}	
}


/******************************************************************************
 Description:	IsLockForecourtComplete
 Returns:      	TRUE - Forecourt LOCKED
				FALSE - Forecourt NOT LOCKED
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/05/2009	 11:56		Start //4.0.22.45 TD 72716
******************************************************************************/
BOOL CConfigurationThread::IsLockForecourtComplete()const
{
	BOOL bRetCode = (	_Module.m_server.GetServerState() != SERVER_IDLE ||
						_Module.m_server.m_cServerInfo.GetServerStatus() == LOCKED ) ? TRUE : FALSE;

	return bRetCode;
}


/******************************************************************************
 Description:	IsUnLockForecourtComplete
 Returns:      	TRUE - Forecourt UNLOCKED
				FALSE - Forecourt NOT UNLOCKED
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/05/2009	 11:56		Start //4.0.22.45 TD 72716
******************************************************************************/
BOOL CConfigurationThread::IsUnLockForecourtComplete()const
{
	BOOL bRetCode = (	_Module.m_server.GetServerState() != SERVER_IDLE ||
						_Module.m_server.m_cServerInfo.GetServerStatus() == UNLOCK ) ? TRUE : FALSE;

	return bRetCode;
}


/******************************************************************************
 Description:	IsReconfigureComplete
 Returns:      	TRUE - Reconfigure Complete
				FALSE - Reconfigure NOT Complete
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			01/05/2009	 11:56		Start //4.0.22.45 TD 72716
******************************************************************************/
BOOL CConfigurationThread::IsReconfigureComplete()const
{
	BOOL bRetCode = ( TRUE ) ? TRUE : FALSE;			////4.0.22.60 TD 72716 TBD

	return bRetCode;
}


/******************************************************************************
 Description:	CONVERT THREAD DATA STRING TO XML AND SAVE TO REGISTRY
 Returns:      	TRUE - SUCCESS
				FALSE - FAILED
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//17/04/09	11:33		Start //4.0.22.45 TD 72716
******************************************************************************/
//protected:
BOOL CConfigurationThread::XMLData2RegTable()
{
	BOOL			bRetCode = TRUE;

	CString			strLogMsg;
	long			lLogLevel = LOG_LEVEL_1;

	CXMLInterface	xmlDoc;
	CXMLPointers	cAllRecordsList, cActiveRecordElement;

	try
	{		
		if (xmlDoc.InitDoc())
		{
			if(NULL != m_pszThreadData)
			{
				bRetCode = xmlDoc.LoadXmlString((char*)m_pszThreadData);
			}
			else
			{
				bRetCode = FALSE;				
			}

			if (bRetCode)
			{
				lLogLevel = DEFAULT_LOG_LEVEL;
				strLogMsg.Format("ConfigurationThread, XML loaded successfully, DataLength=%ld", m_iThreadDataLength);

				bRetCode = m_cConfigData.SaveData2RegTable(xmlDoc);
			}
			else
				strLogMsg = "ConfigurationThread, Failed to load XML, m_pszData=NULL";
		}
	}
	catch(...)
	{
		bRetCode = FALSE;
		
		strLogMsg.Format("ConfigurationThread, XMLData2RegTable is Failed, DataLength=%ld", m_iThreadDataLength);		
	}

	_LOGMSG.LogMsg(strLogMsg, lLogLevel);

#ifdef _DEBUG
	xmlDoc.SaveToFile("C:\\Office\\PumpSrv\\Log\\ConfigurationThreadData.xml");
#endif

	xmlDoc.CloseDoc();

	return bRetCode;
}


inline DWORD CConfigurationThread::GetLockTimeOut()const
{
	return m_dwLockTimeOut;
}


//////////////////////////////////////////////////////////////////////
//						Data Control Methods
//////////////////////////////////////////////////////////////////////

/******************************************************************************
 Description:	IS ThreadData Empty
 Returns:      	TRUE - ThreadData Empty
				FALSE - ThreadData NOT Empty
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//17/04/09	15:33		Start //4.0.22.45 TD 72716
******************************************************************************/
inline BOOL	CConfigurationThread::IsEmptyThreadData()const 
{
	BOOL bRetCode = ( m_iThreadDataLength <= 0 || NULL == m_pszThreadData ) ? TRUE : FALSE;		//add check for new file if exist set thread data  

	return bRetCode;
}


inline BOOL	CConfigurationThread::IsDataFileInUse()const
{
	return m_bLookForDataFile;	
}


/******************************************************************************
 Description:	Set ThreadData AND RASE WaitForData EVENT 
				CALL FROM CTRL::SETUPEX__

 Returns:      	TRUE - SUCCESS
				FALSE - FAILED
 Parameters:   	pszDataIn - STRING TO SET
				iDataLength - STRING LENGTH
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//18/04/09	13:01		Start //4.0.22.45 TD 72716
******************************************************************************/
BOOL CConfigurationThread::SetThreadData(const BYTE* pszDataIn, const size_t iDataLength, const ConfigurationThreadDataSource eDataSource)
{
	BOOL			bRetCode = TRUE;
	CString			strLogMsg;
	long			lLogLevel = LOG_LEVEL_1;

	if (FALSE == m_bThreadAlive)
	{
		bRetCode = FALSE;
		strLogMsg = "ConfigurationThread, Thread Not In Use, Cancel to set Thread Data!";
	}
	else if (m_eThreadState == THREAD_STATE_NOT_CREATED) 
	{
		bRetCode = FALSE;
		strLogMsg = "ConfigurationThread, Thread Not Created, Cancel to set Thread Data!";
	}
	else if (IsEmptyThreadData() == FALSE)
	{
		bRetCode = FALSE;
		strLogMsg = "ConfigurationThread, Thread Data Not Empty, Cancel to set Thread Data!";
	}
	else if (m_cStateMachine.IsInMiddleOfProcess() == TRUE)
	{
		bRetCode = FALSE;
		strLogMsg = "ConfigurationThread, In Middle Of Process, Cancel to set Thread Data!";
	}

	EnterCriticalSection(&m_csThreadData);

	if (TRUE == bRetCode)
	{
		try
		{
			if ( eDataSource == CTDS_FILE )
			{
				bRetCode = m_cFileMonitor.GetFileData( &m_pszThreadData, &m_iThreadDataLength);
			}
			else
			{
				m_iThreadDataLength = iDataLength + 1;

				m_pszThreadData = new BYTE[m_iThreadDataLength];
				memset(m_pszThreadData, 0, m_iThreadDataLength);
				memcpy(m_pszThreadData, pszDataIn, iDataLength);

				bRetCode = TRUE;
			}
		}
		catch(...)
		{
			bRetCode = FALSE;			
		}

		if ( TRUE == bRetCode )
		{
			strLogMsg.Format("ConfigurationThread, Successfully set new Thread Data! DataLength=%ld", m_iThreadDataLength);
			lLogLevel = DEFAULT_LOG_LEVEL;
		}
		else
		{
			strLogMsg.Format("ConfigurationThread, Failed to set Thread Data! DataLength=%ld", m_iThreadDataLength);
			m_cFileMonitor.MoveCurrFile();
			_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE , SUB_PUMPSRV ,	FAILED_TO_PROCESS_FUEL_CONFIGURATION_XML,0,ALARM_STARTED,0,0);//CR 407467
		}
	}

	LeaveCriticalSection(&m_csThreadData);

	_LOGMSG.LogMsg(strLogMsg, lLogLevel);

	if ( bRetCode == TRUE )
		DataToProcess(eDataSource);

	return bRetCode;
}


CConfigurationThread::
ActivationMode  CConfigurationThread::GetActivationMode() const
{ 
	return m_cConfigData.GetActivationMode(); 
}


BOOL CConfigurationThread::GetBackUp() const	//4.0.22.72  TD 76419
{
	return m_cConfigData.GetBackUp();
}


//////////////////////////////////////////////////////////////////////
//					State Machine Access methods
//////////////////////////////////////////////////////////////////////
inline void  CConfigurationThread::SetStateMachine(const State eState)
{ 
	m_cStateMachine.SetCurrentState(eState); 
}


/******************************************************************************
 Description:	Clear ThreadData
 Returns:      	VOID
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//18/04/09	13:01		Start //4.0.22.45 TD 72716
******************************************************************************/
void CConfigurationThread::ClearThreadData()
{
	if ( NULL != m_pszThreadData && 0 != m_iThreadDataLength ) 
	{
		delete [] m_pszThreadData;
		m_pszThreadData = NULL;
		m_iThreadDataLength = 0;
	}
}



//////////////////////////////////////////////////////////////////////
//						Thread Control Methods
//////////////////////////////////////////////////////////////////////
CThreadRetCode CConfigurationThread::StartThread()
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	eThreadRetCode = CThread::StartThread();

	switch (eThreadRetCode)
	{
	case THREAD_OK:
		{
			_LOGMSG.LogMsg("ConfigurationThread, *** Thread Started successfully");
		}break;
	default:
		{
			_LOGMSG.LogMsg("ConfigurationThread, *** Thread WASN'T started!!!", LOG_LEVEL_1);
		}break;
	}

	return eThreadRetCode;
}


CThreadRetCode CConfigurationThread::SetSleepDuration(const DWORD dwSleepDuration)
{
	CThreadRetCode	eThreadRetCode = THREAD_OK;
	DWORD			dwCurrSleepDuration = GetSleepDuration();

	if (dwCurrSleepDuration != dwSleepDuration)
	{
		CThread::StopThread();

		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString strLogMsg;
			strLogMsg.Format("ConfigurationThread, SetSleepDuration from %ldMsec to %ldMsec", dwCurrSleepDuration, dwSleepDuration);
			_LOGMSG.LogMsg(strLogMsg);
		}

		eThreadRetCode = CThread::SetSleepDuration(dwSleepDuration);
		
		CThread::StartThread();
	}

	return(eThreadRetCode);
}


CThreadRetCode CConfigurationThread::StopThread()
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	//Stop the thread
	eThreadRetCode = CThread::StopThread();

	if(THREAD_OK == eThreadRetCode)
	{
		_LOGMSG.LogMsg("ConfigurationThread, Stoped successfully");
	}
	else
	{
		_LOGMSG.LogMsg("ConfigurationThread, WASN'T Stoped successfully");
	}	

	return(eThreadRetCode);
}


CThreadRetCode CConfigurationThread::KillThread()
{
	CThreadRetCode eThreadRetCode = THREAD_OK;

	//Killing the thread
	eThreadRetCode = CThread::KillThread();

	if(THREAD_OK == eThreadRetCode || THREAD_TERMINATED_SUCCESSFULLY == eThreadRetCode)
	{
		eThreadRetCode = THREAD_OK;
		_LOGMSG.LogMsg("ConfigurationThread, successfully Terminated");
	}
	else
	{
		CString strLogErr;
		strLogErr.Format("ConfigurationThread, WASN'T terminated. ThreadRetCode = %ld", eThreadRetCode);
		_LOGMSG.LogMsg(strLogErr);	
	}
	
	return(eThreadRetCode);
}


//////////////////////////////////////////////////////////////////////
//					New Data methods
//////////////////////////////////////////////////////////////////////
inline void CConfigurationThread::DataToProcess(const ConfigurationThreadDataSource eDataSource)/*SetWaitForDataEvent()*/
{ 
	if ( IsDataFileInUse() == TRUE )
	{
		if ( eDataSource == CTDS_CTRL_INTERFACE )
		{
			m_cMultipleEvents.SetOne(CTEI_CTRL_INTERFACE); 
		}		
	}
	else
	{
		StartThread();
	}
}


void CConfigurationThread::WaitForData()		
{ 
	if ( IsDataFileInUse() == TRUE )			//4.0.23.504 TD 78942
	{
		//Before wating for the 2 events - we look for existing files on directory.
		//If exist, process them first.
		
		m_cFileMonitor.LookForFile();		

		m_cMultipleEvents.WaitForSome( INFINITE );
	}
	else
	{
		StopThread();
	}
}


/******************************************************************************
 Description:	set thread data with xml file data
 Returns:      	VOID
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//30/09/09	10:52		Start	//4.0.23.504 TD 78942
******************************************************************************/
//private
BOOL CConfigurationThread::HandleNewFile()
{
	BOOL	bRetCode = TRUE;

	if ( m_cMultipleEvents.EventSignaled() == TRUE )
	{
		bRetCode = m_cFileMonitor.IsNewFileAdded();
		m_cMultipleEvents.ClearSignaled();

		Sleep(1000);
	}

	if ( TRUE == bRetCode )
	{
		bRetCode = m_cFileMonitor.FindFile();

		if ( TRUE == bRetCode )
		{
			bRetCode = SetThreadData( NULL, 0, CTDS_FILE);

			if ( TRUE == bRetCode)
			{
				m_cFileMonitor.DeleteCurrFile();
			}
		}
	}

	return bRetCode;
}


/******************************************************************************
 Description:	CleanUp
 Returns:      	VOID
 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//18/04/09	12:32		Start //4.0.22.45 TD 72716
******************************************************************************/
void CConfigurationThread::CleanUp()
{
	if ( TRUE == m_bThreadAlive )
	{
		EnterCriticalSection(&m_csThreadData);

		ClearThreadData();
		
		if ( IsDataFileInUse() == TRUE )
		{
			m_cMultipleEvents.SetOne( CTEI_CLEANUP );

			m_cFileMonitor.CleanUp();
			m_cMultipleEvents.CleanUp();
		}

		KillThread();
		
		LeaveCriticalSection(&m_csThreadData);	
	}
}


void CConfigurationThread::SaveConfigurationData()
{
	BOOL			bRetCode = FALSE;
	CString			strLogMsg;
	CXMLInterface	xmlDoc;

	char tmpStr[MAX_PATH];
	char sConfigurationXMLFile[MAX_FILE_PATH];

	memset(tmpStr, 0, sizeof(tmpStr));
	memset(sConfigurationXMLFile, 0,sizeof(sConfigurationXMLFile));


	_Module.m_server.m_cParam.LoadParam("Paths\\Long", "LogDir",tmpStr,sizeof(tmpStr), sConfigurationXMLFile,FALSE);
	strcpy(sConfigurationXMLFile,tmpStr);
	strcat((char *)sConfigurationXMLFile,"\\Log\\ConfigurationData.xml");	


	EnterCriticalSection(&m_csThreadData);	


	try	
	{
		if (xmlDoc.InitDoc())
		{
			if ( NULL != m_pszThreadData && 0 != m_iThreadDataLength ) 
			{
				bRetCode = xmlDoc.LoadXmlString((char*)m_pszThreadData);
			}

			if (bRetCode)
			{

				xmlDoc.SaveToFile(sConfigurationXMLFile);

				strLogMsg.Format("ConfigurationThread, SaveConfigurationData Successfully. File=[%s]", sConfigurationXMLFile);
				_Module.m_server.m_cLogger.LogMsg(strLogMsg, LOG_LEVEL_1);
			}
		}

	}
	catch (...)
	{
		_Module.m_server.m_cLogger.LogMsg("ConfigurationThread, Failed on SaveConfigurationData.", LOG_LEVEL_1);

	}

	LeaveCriticalSection(&m_csThreadData);

	xmlDoc.CloseDoc();

}


void CConfigurationThread::SetForecourtLocked( BOOL bServerLockState )
{
	_Module.m_server.m_cParam.SaveParam("GeneralParam", "LockedByConfigurationThread",(DWORD)bServerLockState, TRUE,0,0,0,TRUE);	
}

BOOL CConfigurationThread::IsForecourtLockedByConfigurationThread()
{

	DWORD dwLockedByConfigurationThread = FALSE;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "LockedByConfigurationThread",&dwLockedByConfigurationThread,(DWORD)0,TRUE ,TRUE,0,HOP_DIRECT_LOCAL,TRUE);	//	4.0.28.340  TD 419618 

	return BOOL(dwLockedByConfigurationThread);
}


//---------------------------------------------------------------------
//							CConfigData class methods
//---------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfigurationThread::CConfigData::CConfigData()
{
	ClearReconfigData();
	memset(&m_cConfigDataStruct, 0, sizeof(m_cConfigDataStruct));
}


/******************************************************************************
 Description:	Get ActivationMode from m_cConfigDataStruct 
 Returns:      	CTAM_RECONFIGURE_WITH_LOCK	= 0
				CTAM_RECONFIGURE_IMMEDIATE	= 1
				CTAM_RECONFIGURE_ON_NEXT_STARTUP = 2

 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//18/04/09	12:32		Start //4.0.22.45 TD 72716
******************************************************************************/
CConfigurationThread::
ActivationMode	CConfigurationThread::CConfigData::GetActivationMode()const
{
	ActivationMode	eActivationMode = CTAM_RECONFIGURE_IMMEDIATE;
	const size_t	iActivationModeNum = 3;
	const char		szActivationMode[iActivationModeNum][MAX_NAME_LENGTH] = {	"Lock",
																				"Immediate",
																				"OnNextStartUp",
																			};
	
	for (int i=0; i<iActivationModeNum; i++)
	{
			if(strncmp((char*)m_cConfigDataStruct.cMetaData.szActivationMode, szActivationMode[i], 
						min(sizeof(m_cConfigDataStruct.cMetaData.szActivationMode), strlen(szActivationMode[i]))) == 0 )
			{
				eActivationMode = (ActivationMode)i;
				i = iActivationModeNum;
			}
	}

	return eActivationMode;
}


/******************************************************************************
 Description:	Get BackUp from m_cConfigDataStruct 
 Returns:      	TRUE
				FALSE

 Parameters:   	
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//17/06/09	10:59		Start //4.0.22.72  TD 76419
******************************************************************************/
inline BOOL	CConfigurationThread::CConfigData::GetBackUp() const
{
	return m_cConfigDataStruct.cMetaData.bBackUp;
}


/******************************************************************************
 Description:	Save XML Data TO RegTable through m_cConfigDataStruct
 Returns:      	void
 Parameters:   	cXMLInterface - XML Data TO save
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//22/04/09	11:00		Start //4.0.22.45 TD 72716
******************************************************************************/
BOOL CConfigurationThread::CConfigData::SaveData2RegTable(CXMLInterface& cXMLInterface)
{
	BOOL			bRetCode	= TRUE;	
	BOOL			bRetVal		= TRUE;
	CXMLPointers	cAllRecordsList;

	bRetCode = SetMetaData(&cAllRecordsList, cXMLInterface);
	ClearReconfigData();

	while( TRUE == bRetCode && TRUE == bRetVal )
	{	
		try
		{
			bRetVal = SetConfiguration(cXMLInterface, cAllRecordsList);
		}
		catch (...)
		{
			bRetCode = FALSE;
		}		
	}

	return bRetCode;
}


//////////////////////////////////////////////////////////////////////
//					Set methods
//////////////////////////////////////////////////////////////////////

/******************************************************************************
 Description:	Set XML MetaData node attributes TO m_cConfigDataStruct.cMetaData
 Returns:      	TRUE - success
				FALSE - fail

 Parameters:   	cXMLInterface - XML Data TO set
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//22/04/09	11:30		Start //4.0.22.45 TD 72716
******************************************************************************/
//protected:
BOOL CConfigurationThread::CConfigData::SetMetaData(CXMLPointers* pAllRecordsList, CXMLInterface& cXMLInterface)
{
	BOOL			bRetCode	= TRUE;
	META_DATA*		pMetaData	= NULL;
	CXMLPointers	cMetaDataRecordsList;
		
	pMetaData = &m_cConfigDataStruct.cMetaData;
	memset(pMetaData, 0, sizeof(META_DATA));

	cXMLInterface.GetChildNodesList(NULL, pAllRecordsList);									//Get all nodes
	bRetCode = cXMLInterface.GetNextItem(*pAllRecordsList, cMetaDataRecordsList);			//Move to "MetaData" node

	if(TRUE == bRetCode)
	{
		try
		{
			char	szBackUp[MAX_NAME_LENGTH] = {0};
			
			cXMLInterface.GetAttributeToByteArrWithConvert("BackUp", szBackUp, sizeof(szBackUp) - 1, &cMetaDataRecordsList, 1);
			
			if ( strlen( (const char*)szBackUp ) == 0 )
				bRetCode = FALSE;
			else
				pMetaData->bBackUp = (strcmp((char*)szBackUp, "TRUE") == 0 || strcmp((char*)szBackUp, "true") == 0) ? TRUE : FALSE;				

			if ( TRUE == bRetCode )
			{
				cXMLInterface.GetAttributeToByteArrWithConvert("Source",			pMetaData->szSource,			&cMetaDataRecordsList);
				cXMLInterface.GetAttributeToByteArrWithConvert("Date",				pMetaData->szDate,				&cMetaDataRecordsList);
				cXMLInterface.GetAttributeToByteArrWithConvert("Time",				pMetaData->szTime,				&cMetaDataRecordsList);
				cXMLInterface.GetAttributeToByteArrWithConvert("ActivationMode",	pMetaData->szActivationMode,	&cMetaDataRecordsList);
			}
		}
		catch (...)
		{
			bRetCode = FALSE;
		}
	}

	return bRetCode;
}


/******************************************************************************
 Description:	Set XML Configuration node attributes TO m_cConfigDataStruct.cConfiguration
 Returns:      	TRUE - success
				FALSE - fail

 Parameters:   	cXMLInterface - XML Data TO set
				cActiveRecordsList
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//22/04/09	17:44		Start //4.0.22.45 TD 72716
******************************************************************************/
//protected:
BOOL CConfigurationThread::CConfigData::SetConfiguration(CXMLInterface& cXMLInterface, CXMLPointers& cActiveRecordsList)
{
	BOOL				bRetCode		= TRUE;
	CONFIGURATION*		pConfiguration	= NULL;
	CXMLPointers		cConfigurationRecordsList, 
						cChildRecordsList;
	DWORD				dwFullDataDownload = FALSE;
	
	pConfiguration = &m_cConfigDataStruct.cConfiguration;
	memset(pConfiguration, 0, sizeof(DATA));

 	bRetCode = cXMLInterface.GetNextItem(cActiveRecordsList, cConfigurationRecordsList);			//Move to ("Configuration") node
	
	if(TRUE == bRetCode)
	{
		try
		{
			cXMLInterface.GetAttributeToByteArrWithConvert("Source", pConfiguration->szSource, &cConfigurationRecordsList);
		
			cXMLInterface.GetChildNodesList(&cConfigurationRecordsList, &cChildRecordsList);						//Get all child nodes of "Configuration"

			dwFullDataDownload	= cXMLInterface.GetAttributeToLong("FullDataUpdate", &cConfigurationRecordsList);			//cr 407467

			
		}
		catch (...)
		{
			bRetCode = FALSE;
		}

		if ( TRUE == bRetCode )
			bRetCode = SetData(cXMLInterface, cChildRecordsList,dwFullDataDownload);
	}

	return bRetCode;
}


/******************************************************************************
 Description:	Set XML Data node attributes TO m_cConfigDataStruct.cConfiguration.cData
 Returns:      	TRUE - success
				FALSE - fail

 Parameters:   	cXMLInterface - XML Data TO set
				cActiveRecordsList
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//23/04/09 13:44		Start //4.0.22.45 TD 72716
******************************************************************************/
//private:
BOOL CConfigurationThread::CConfigData::SetData(CXMLInterface& cXMLInterface, CXMLPointers& cActiveRecordsList , BOOL bFullDataDownload)
{
	BOOL			bRetCode = TRUE;	
	DATA*			pData = NULL;
	CXMLPointers	cDataRecordsList, 
					cChildRecordsList;
	
	pData = &m_cConfigDataStruct.cConfiguration.cData;
	memset(pData, 0, sizeof(DATA));
	while( TRUE == bRetCode )//CR 407467
	{
		bRetCode = cXMLInterface.GetNextItem(cActiveRecordsList, cDataRecordsList);							//Move to ("Data") node
	
		if( TRUE == bRetCode )
		{	
			try
			{
				cXMLInterface.GetAttributeToByteArrWithConvert("TableName", pData->szTableName, &cDataRecordsList);
				pData->dwTableId		= cXMLInterface.GetAttributeToLong("TableId", &cDataRecordsList);			//4.0.22.74 3
				pData->dwSubTableId		= cXMLInterface.GetAttributeToLong("SubTableId", &cDataRecordsList);		//4.0.22.74 3
				pData->dwFlags			= cXMLInterface.GetAttributeToLong("Flags", &cDataRecordsList);				//4.0.22.74 3
				pData->dwSubTableIdExt	= cXMLInterface.GetAttributeToLong("SubTableIdExt", &cDataRecordsList);			//4.0.22.74 3
				
				cXMLInterface.GetChildNodesList(&cDataRecordsList, &cChildRecordsList);									//Get all child nodes of "Data"
			}
			catch (...)
			{
				bRetCode = FALSE;
			}

			if ( TRUE == bRetCode )
			{
				if(TRUE == bFullDataDownload)//cr 407467
				{
					ResetAllValidEntries();
					bFullDataDownload = FALSE;//the reset should be called once for every xml file received
				}
				SetReconfigData();
				bRetCode = SetParameters(cXMLInterface, cChildRecordsList);
			}
		}
	}
	return bRetCode;
}


BOOL CConfigurationThread::CConfigData::IsReconfigDataEmpty() 
{
	BOOL bRetCode = TRUE;
	
	bRetCode = m_cReconfigDataQueue.Empty();

	return bRetCode;
}


void CConfigurationThread::CConfigData::GetReconfigData(long* lTableId, long* lSubTableId, long* lSubTableIdExt, long* lFlags, BOOL* bBackUp)
{
	DATA	cData;
	memset( &cData, 0, sizeof(cData) );

	m_cReconfigDataQueue.GetHead( &cData );

	*lTableId		= cData.dwTableId;
	*lSubTableId	= cData.dwSubTableId;
	*lSubTableIdExt	= cData.dwSubTableIdExt;
	*lFlags			= cData.dwFlags;
	*bBackUp		= m_cConfigDataStruct.cMetaData.bBackUp;

	m_cReconfigDataQueue.Remove( &cData );
}


void CConfigurationThread::CConfigData::SetReconfigData()
{
	DATA	cReconfigData;

	memset( &cReconfigData, 0, sizeof(cReconfigData));
	memcpy( &cReconfigData, &m_cConfigDataStruct.cConfiguration.cData, sizeof(cReconfigData));

	m_cReconfigDataQueue.Set( &cReconfigData );
}


void CConfigurationThread::CConfigData::ClearReconfigData()
{
	m_cReconfigDataQueue.Clean();
}


/******************************************************************************
 Description:	Set XML Parameters node attributes TO m_cConfigDataStruct.cConfiguration.cData.cParameters
 Returns:      	TRUE - success
				FALSE -fail

 Parameters:   	cXMLInterface - XML Data TO set
				cActiveRecordsList
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//23/04/09 12:44		Start //4.0.22.45 TD 72716
******************************************************************************/
//private:
BOOL CConfigurationThread::CConfigData::SetParameters(CXMLInterface& cXMLInterface, CXMLPointers& cActiveRecordsList)
{
	BOOL			bRetCode	= TRUE;
	BOOL			bRetVal		= TRUE;
	LONG			lFlags		= HOP_DIRECT_LOCAL;
	CXMLPointers	cParametersRecordsList, 
					cChildRecordsList;

	if ( m_cConfigDataStruct.cMetaData.bBackUp == TRUE ) 
		lFlags = HOP_DEFAULT_CONFIG;

	bRetCode = cXMLInterface.GetNextItem(cActiveRecordsList, cParametersRecordsList);			//Get next ("Parameters") node

	if ( TRUE == bRetCode )
	{
		try
		{
			cXMLInterface.GetChildNodesList(&cParametersRecordsList, &cChildRecordsList);			//Get "Param" nodes list

			bRetVal = SetParam(cXMLInterface, cChildRecordsList); 
		}
		catch (...)
		{
			bRetCode = FALSE;
		}

		while ( TRUE == bRetVal && TRUE == bRetCode)
		{		
			SaveParam(lFlags);

			bRetVal = SetParam(cXMLInterface, cChildRecordsList); 		
		}
	}

	return bRetCode;
}


/******************************************************************************
 Description:	Set XML Param node attributes TO m_cConfigDataStruct.cConfiguration.cData.cParameters.cParam
 Returns:      	TRUE - success
				FALSE - fail

 Parameters:   	cXMLInterface - XML Data TO set
				cActiveRecordsList
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//22/04/09 17:44		Start //4.0.22.45 TD 72716
******************************************************************************/
//private:
BOOL CConfigurationThread::CConfigData::SetParam(CXMLInterface& cXMLInterface, CXMLPointers& cActiveRecordsList)
{
	BOOL			bRetCode	= TRUE;
	BOOL			bRetVal		= TRUE;
	PARAM*			pParam		= NULL;			
	CXMLPointers	cActiveRecordItem;
	
	pParam = &m_cConfigDataStruct.cConfiguration.cData.cParameters.cParam;			
	memset(pParam, 0, sizeof(PARAM));

	bRetCode = cXMLInterface.GetNextItem(cActiveRecordsList, cActiveRecordItem);

	if (TRUE == bRetCode)
	{
		try
		{
			cXMLInterface.GetAttributeToByteArrWithConvert("Name", pParam->szValueName, &cActiveRecordItem);
			pParam->byType = cXMLInterface.GetAttribute("Type", &cActiveRecordItem);

			if ( REG_SZ == pParam->byType )
			{
				cXMLInterface.GetAttributeToByteArrWithConvert("Value", pParam->szValue, &cActiveRecordItem);
			}
			else
			{
				pParam->dwValue = cXMLInterface.GetAttributeToLong("Value", &cActiveRecordItem);
			}			
		}
		catch (...)
		{
			bRetCode = FALSE;			
		}
	}

	return bRetCode;
}



/******************************************************************************
 Description:	Save m_cConfigDataStruct.cConfiguration.cData.cParameters.cParam attributes TO registry 
 Returns:      	void
 Parameters:   	lFlags -	HOP_DIRECT_LOCAL,
							HOP_DEFAULT_CONFIG
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//22/04/09 19:44		Start //4.0.22.45 TD 72716
******************************************************************************/
//private:
void CConfigurationThread::CConfigData::SaveParam(const long lFlags) 
{
	long		lRetCode = OK;
	PARAM*		pParam	= NULL;			
	DATA*		pData	= NULL;

	pParam = &m_cConfigDataStruct.cConfiguration.cData.cParameters.cParam;			
	pData  = &m_cConfigDataStruct.cConfiguration.cData;	

	if ( pParam->byType == REG_SZ) 
	{
		 lRetCode = _Module.m_server.m_cParam.SaveParam((char*)pData->szTableName, (char*)pParam->szValueName, (char*)pParam->szValue, FALSE, 0, lFlags);
	}
	else 
	{
		 lRetCode = _Module.m_server.m_cParam.SaveParam((char*)pData->szTableName, (char*)pParam->szValueName, pParam->dwValue, FALSE, 0, lFlags);
	}
}

void CConfigurationThread::CConfigData::ResetAllValidEntries()
{
	long lRetCode = 0;
	DWORD dwValue = 0;
	
	char sTemp[MAX_FIELD_VAL]={};
	const char*sVal=NULL;
	
	memcpy(sTemp , (char*)&m_cConfigDataStruct.cConfiguration.cData.szTableName,sizeof(m_cConfigDataStruct.cConfiguration.cData.szTableName));
	sVal = strstr(sTemp,"0");
	
	for(long lIndex = 1; !lRetCode ; lIndex++)
	{
		(lIndex < 10) ? sprintf_s((char*)sVal , 5,"0%ld" , lIndex) : sprintf_s((char*)sVal , 5,"%ld" , lIndex);
		
		lRetCode = _Module.m_server.m_cParam.SaveParam(sTemp, "ValidEntry", dwValue, FALSE, 0, 1);		
	}
}


//---------------------------------------------------------------------
//		CWaitDataState class	//4.0.22.45 TD 72716	//25/04/09 10:44
//---------------------------------------------------------------------

/******************************************************************************
 Description:	IDLE state. 
				if got new data go to the next state (CTS_APPLY_DATA)
				if not wait for event 
							
 Returns:      	void
 Parameters:   	ConfigurationThread

 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//22/04/09 19:44		Start //4.0.22.45 TD 72716
******************************************************************************/
void CConfigurationThread::CWaitForDataState::Operation(CConfigurationThread* cConfigurationThread)
{
	if( cConfigurationThread->IsEmptyThreadData() == FALSE || cConfigurationThread->HandleNewFile() == TRUE )	//Thread data is not empty
	{
		cConfigurationThread->SetStateMachine(CTS_APPLY_DATA);
	}
	else
	{
		cConfigurationThread->WaitForData();					//4.0.23.504 TD 78942
	}
}


//---------------------------------------------------------------------
//		CApplyDataState class	//4.0.22.45 TD 72716	//25/04/09 10:44
//---------------------------------------------------------------------
void CConfigurationThread::CApplyDataState::Operation(CConfigurationThread* cConfigurationThread)
{
	if( cConfigurationThread->ApplyData() == TRUE )		// Success
	{		
		ActivationMode eActivationMode = cConfigurationThread->GetActivationMode();

		switch(eActivationMode)
		{
		case CTAM_RECONFIGURE_WITH_LOCK:
			{
				cConfigurationThread->SetStateMachine(CTS_START_LOCK);
			}break;
		case CTAM_RECONFIGURE_IMMEDIATE:
			{
				cConfigurationThread->SetStateMachine(CTS_START_RECONFIGURE);
			}break;
		case CTAM_RECONFIGURE_ON_NEXT_STARTUP:
			{
				cConfigurationThread->SetStateMachine(CTS_WAIT_FOR_DATA);
			}break;
		default:
			{
				cConfigurationThread->SetStateMachine(CTS_START_RECONFIGURE);
				_LOGMSG.LogMsg("ConfigurationThread, Activation Mode is wrong!", LOG_LEVEL_1);			
			}break;
		}
	}
	else					// Fail Apply data. reset state machine
	{
		cConfigurationThread->SetStateMachine( CTS_WAIT_FOR_DATA );
		_LOGMSG.LogMsg("ConfigurationThread, Apply data was failed!", LOG_LEVEL_1);	
		_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE , SUB_PUMPSRV ,	FAILED_TO_PROCESS_FUEL_CONFIGURATION_XML,0,ALARM_STARTED,0,0);//CR 407467
	}
}


//---------------------------------------------------------------------
//		CStartLockState class	//4.0.22.45 TD 72716	//26/04/09 16:10
//---------------------------------------------------------------------
void CConfigurationThread::CStartLockState::Operation(CConfigurationThread* cConfigurationThread)
{
	cConfigurationThread->LockForecourt();
	cConfigurationThread->SetStateMachine(CTS_WAIT_FOR_LOCK);
}


//---------------------------------------------------------------------
//		CWaitForLockState class		//4.0.22.45 TD 72716	//26/04/09 16:21
//---------------------------------------------------------------------
void CConfigurationThread::CWaitForLockState::Operation(CConfigurationThread* cConfigurationThread)
{
	InitWaitTime();

	if(cConfigurationThread->IsLockForecourtComplete() == TRUE)
	{
		ResetWaitTime();
		cConfigurationThread->SetStateMachine(CTS_START_RECONFIGURE);
	}
	else if ( IsWaitTimePassed(cConfigurationThread->GetLockTimeOut()) == TRUE )					//4.0.22.60 TD 72716
	{
		ResetWaitTime();
		cConfigurationThread->SetStateMachine(CTS_START_FORCE_UNLOCK);
	}
}


//--------------------------------------------------------------------
//		CStartForceUnlockState class 	//4.0.22.45 TD 72716	//10/05/09 11:49
//--------------------------------------------------------------------
void CConfigurationThread::CStartForceUnlockState::Operation(CConfigurationThread* cConfigurationThread)
{
	cConfigurationThread->ForceUnLockForecourt();
	cConfigurationThread->SetStateMachine(CTS_WAIT_FOR_FORCE_UNLOCK);
}


//--------------------------------------------------------------------
//		CWaitForForceUnlockState class 	//4.0.22.45 TD 72716	//10/05/09 11:49
//--------------------------------------------------------------------
void CConfigurationThread::CWaitForForceUnlockState::Operation(CConfigurationThread* cConfigurationThread)
{
	if(cConfigurationThread->IsUnLockForecourtComplete() == TRUE) 
	{
		cConfigurationThread->SetStateMachine(CTS_START_RECONFIGURE);
	}
}


//---------------------------------------------------------------------
//		CStartReconfigureState class	//4.0.22.45 TD 72716	//26/04/09 16:26
//---------------------------------------------------------------------
void CConfigurationThread::CStartReconfigureState::Operation(CConfigurationThread* cConfigurationThread)
{
	cConfigurationThread->Reconfigure();
	cConfigurationThread->SetStateMachine(CTS_WAIT_FOR_RECONFIGURE);
}


//---------------------------------------------------------------------
//		CWaitForReconfigureState class	//4.0.22.45 TD 72716	//26/04/09 16:26
//---------------------------------------------------------------------
void CConfigurationThread::CWaitForReconfigureState::Operation(CConfigurationThread* cConfigurationThread)
{
	InitWaitTime();

	if ( cConfigurationThread->IsReconfigureComplete() == TRUE || IsWaitTimePassed(WAIT_FOR_RECONFIGURE_TIMEOUT) == TRUE)		//TBD
	{
		ActivationMode eActivationMode = cConfigurationThread->GetActivationMode();

		switch(eActivationMode)
		{
		case CTAM_RECONFIGURE_WITH_LOCK:
			{
				cConfigurationThread->SetStateMachine(CTS_START_UNLOCK);				
			}break;
		case CTAM_RECONFIGURE_IMMEDIATE:
			{
				cConfigurationThread->SetStateMachine(CTS_WAIT_FOR_DATA);
			}break;	
		default:
			{
				cConfigurationThread->SetStateMachine(CTS_WAIT_FOR_DATA);
				_LOGMSG.LogMsg("ConfigurationThread, Activation Mode is wrong!", LOG_LEVEL_1);			
			}break;
		}

		ResetWaitTime();
	}
}


//---------------------------------------------------------------------
//		CStartUnlockState class		//4.0.22.45 TD 72716	//26/04/09 16:46
//---------------------------------------------------------------------
void CConfigurationThread::CStartUnlockState::Operation(CConfigurationThread* cConfigurationThread)
{
	cConfigurationThread->UnLockForecourt();						
	cConfigurationThread->SetStateMachine(CTS_WAIT_FOR_UNLOCK);
}


//---------------------------------------------------------------------
//		CWaitForUnlockState class	//4.0.22.45 TD 72716	//26/04/09 16:46
//---------------------------------------------------------------------
void CConfigurationThread::CWaitForUnlockState::Operation(CConfigurationThread* cConfigurationThread)
{
	if(cConfigurationThread->IsUnLockForecourtComplete() == TRUE) 
	{
		cConfigurationThread->SetStateMachine(CTS_WAIT_FOR_DATA);
	}
}


//---------------------------------------------------------------------
//							CStateMachine class
//---------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction		//4.0.22.45 TD 72716	//25/04/09 10:33
//////////////////////////////////////////////////////////////////////
CConfigurationThread::CStateMachine::CStateMachine()
{
	for(int i=0; i<CTS_STATE_SIZE; i++)
	{
		m_cStates[i] = NULL;
	}
}


CConfigurationThread::CStateMachine::~CStateMachine()
{
	for(int i=0; i<CTS_STATE_SIZE; i++)
	{
		if(NULL == m_cStates[i])
			delete m_cStates[i];
	}
}


/******************************************************************************
 Description:	StateMachine Initialization
 Returns:      	void
 Parameters:   	none
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//25/04/09 10:44		Start //4.0.22.45 TD 72716
******************************************************************************/
void CConfigurationThread::CStateMachine::Init()
{
	m_eCurrState = CTS_WAIT_FOR_DATA;

	m_cStates[CTS_WAIT_FOR_DATA]			= new CWaitForDataState;
	m_cStates[CTS_APPLY_DATA]				= new CApplyDataState;
	m_cStates[CTS_START_LOCK]				= new CStartLockState;
	m_cStates[CTS_WAIT_FOR_LOCK]			= new CWaitForLockState;
	m_cStates[CTS_START_RECONFIGURE]		= new CStartReconfigureState;
	m_cStates[CTS_WAIT_FOR_RECONFIGURE]		= new CWaitForReconfigureState;
	m_cStates[CTS_START_UNLOCK]				= new CStartUnlockState;
	m_cStates[CTS_WAIT_FOR_UNLOCK]			= new CWaitForUnlockState;
	m_cStates[CTS_START_FORCE_UNLOCK]		= new CStartForceUnlockState;
	m_cStates[CTS_WAIT_FOR_FORCE_UNLOCK]	= new CWaitForForceUnlockState;
}


/******************************************************************************
 Description:	1.Write Log Msg 
				2.set Current state

 Returns:      	void
 Parameters:   	eState -	CTS_WAIT_FOR_DATA,		
							CTS_APPLY_DATA,			
							CTS_START_LOCK,			
							CTS_WAIT_FOR_LOCK,		
							CTS_START_RECONFIGURE,	
							CTS_WAIT_FOR_RECONFIGURE,
							CTS_START_UNLOCK,		
							CTS_WAIT_FOR_UNLOCK,	
							CTS_START_FORCE_UNLOCK,	
							CTS_WAIT_FOR_FORCE_UNLOCK,

 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//25/04/09 10:41		Start //4.0.22.45 TD 72716
******************************************************************************/
inline void CConfigurationThread::CStateMachine::SetCurrentState(const State eState)
{
	StateChangedLogMsg(eState);

	m_eCurrState = eState;
}


/******************************************************************************
 Description:	StateMachine Initialization
 Returns:      	void
 Parameters:   	none
 
 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//26/04/09 22:41		Start //4.0.22.45 TD 72716
******************************************************************************/
inline BOOL CConfigurationThread::CStateMachine::IsInMiddleOfProcess() const
{
	BOOL bRetCode = (CTS_WAIT_FOR_DATA == m_eCurrState) ? FALSE : TRUE;

	return bRetCode;
}


void CConfigurationThread::CStateMachine::Operation(CConfigurationThread* cConfigurationThread)
{
	if (NULL != m_cStates[m_eCurrState])
	{
		__try			
		{
			m_cStates[m_eCurrState]->Operation(cConfigurationThread);
		}
		__except(_HANDLE_EXCEPTION("CConfigurationThread::CStateMachine::Operation"))
		{
			_Module.m_server.m_cLogger.LogMsg("CConfigurationThread::CStateMachine::Operation is Failed", LOG_LEVEL_1);
			_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, CONFIGURATION_THREAD_FAIL, 0, ALARM_STARTED, 0, 0);
			cConfigurationThread->SetStateMachine(CTS_WAIT_FOR_RECONFIGURE);  // Force UNLOCK Forecourt operation
			cConfigurationThread->SetForecourtLocked(FALSE);
		}
	}
}


/******************************************************************************
 Description:	Log Msg Format after state change
 Returns:      	void
 Parameters:   	eNewState - CTS_WAIT_FOR_DATA,		
							CTS_APPLY_DATA,			
							CTS_START_LOCK,			
							CTS_WAIT_FOR_LOCK,		
							CTS_START_RECONFIGURE,	
							CTS_WAIT_FOR_RECONFIGURE,
							CTS_START_UNLOCK,		
							CTS_WAIT_FOR_UNLOCK,	
							CTS_START_FORCE_UNLOCK,	
							CTS_WAIT_FOR_FORCE_UNLOCK,

 WHO			WHEN                    WHAT
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Gena			//25/04/09 10:41		Start //4.0.22.45 TD 72716
******************************************************************************/
//private:
void CConfigurationThread::CStateMachine::StateChangedLogMsg(const State eNewState)const
{
	BYTE szStateNames[CTS_STATE_SIZE][MAX_NAME_LEN] = {	{"CTS_WAIT_FOR_DATA"},
														{"CTS_APPLY_DATA"},			
														{"CTS_START_LOCK"},
														{"CTS_WAIT_FOR_LOCK"},
														{"CTS_START_RECONFIGURE"},
														{"CTS_WAIT_FOR_RECONFIGURE"},
														{"CTS_START_UNLOCK"},
														{"CTS_WAIT_FOR_UNLOCK"},
														{"CTS_START_FORCE_UNLOCK"},
														{"CTS_WAIT_FOR_FORCE_UNLOCK"}
													};
	
	CString strLogMsg;
	strLogMsg.Format("ConfigurationThread, Changed the StateMachine status from %s to %s", szStateNames[m_eCurrState], szStateNames[eNewState]);
	_LOGMSG.LogMsg(strLogMsg);
}
