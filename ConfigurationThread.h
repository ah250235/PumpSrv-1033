// ConfigurationThread.h: interface for the CConfigurationThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGURATIONTHREAD1_H__2A9DAAC4_65E2_430E_8571_22C87E7AA7D0__INCLUDED_)
#define AFX_CONFIGURATIONTHREAD1_H__2A9DAAC4_65E2_430E_8571_22C87E7AA7D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "Logger.h"

#define MAX_FILE_PATH 200
#define MINUTES_TO_MILISECONDS(minutes) minutes*SECONDS_IN_MINUTE*MILLISECONDS_IN_A_SECOND	//convert minutes to miliseconds

//--------------------------------------------------------------------
//				CConfigurationLoader class //4.0.22.43 TD 72716
//--------------------------------------------------------------------
class CConfigurationLoader
{
public:
	CConfigurationLoader();
	virtual ~CConfigurationLoader();

	// Main Method
	void			ReLoad();	//DONT OVERLOAD THIS METHOD!

protected:

	virtual void	ReInit() = 0;
	virtual void 	LoadConfiguration(const BOOL bCreate) = 0;
	virtual BOOL	IsReInitRequired() = 0;

	virtual CConfigurationLoader*	GetChild()const { return NULL; };

private:
	
};


//--------------------------------------------------------------------
//				CTimer class //4.0.22.43 TD 72716
//--------------------------------------------------------------------
class CTimer
{
public:
	CTimer();
	
	virtual void	InitWaitTime();
	BOOL			IsWaitTimePassed(const long lTimeOutVal);
	void			ResetWaitTime();

private:
	BOOL			m_bTimerNotActive;
	SYSTEMTIME		m_tStartWaitTime;
};


//--------------------------------------------------------------------
//					Configuration Data Structures
//					"BUILT AS FuelConfiguration XML"
//--------------------------------------------------------------------

// FuelConfiguration XML FOR EXAMPLE !!!!!

// <FuelConfiguration>
//    <MetaData Source="Tesco BOS" Date="02/03/2008" Time="18:21:09" ActivationMode="Lock" BackUp="FALSE"><!--ActivationMode - Can be OnNextStartUp, or IMMEDIATE or Lock--></MetaData>
//    <Configuration Source="Tesco BOS">
//       <Data TableName="GeneralParam" TableId="0x00100000" SubTableId="0x00000200" SubTableIdExt="0" Flags="0">
//          <Parameters>
//             <Param Name="DisableEncrypt" Value="0" Type="0"><!--Store Mode = DisableEncrypt & DSSTestMode--></Param>
//             <Param Name="DSSTestMode" Value="0" Type="0"/>
//             <Param Name="PCIExcludesLoyalty" Value="0" Type="0"/>
//             <Param Name="PCICompliant" Value="1" Type="0"/>
//             <Param Name="DSSActive" Value="1" Type="0"/>
//             <Param Name="DSSAlgorithm" Value="3" Type="0 "/>
//          </Parameters>
//       </Data>
//    </Configuration>
// </FuelConfiguration>


typedef struct 
{
	BYTE	szDate[MAX_DATE_LENGTH];
	BYTE	szTime[MAX_TIME_LENGTH];
	BYTE	szActivationMode[MAX_NAME_LENGTH];
	BOOL	bBackUp;
	BYTE	szSource[MAX_FIELD_VAL];
}META_DATA;

typedef struct 
{
	BYTE	szValueName[MAX_FIELD_NAME];
	BYTE	szValue[MAX_FIELD_VAL];
	DWORD	dwValue;
	BYTE	byType;
}PARAM;

typedef struct 
{
	PARAM cParam;
}PARAMETERS;

typedef struct 
{
	BYTE		szTableName[MAX_TABLE_NAME];
	DWORD		dwTableId;
	DWORD		dwSubTableId;
	DWORD		dwSubTableIdExt;
	DWORD		dwFlags;

	PARAMETERS	cParameters;		
}DATA;

typedef struct 
{
	BYTE	szSource[MAX_FIELD_VAL];		//NOT IN USE
	DATA	cData;
}CONFIGURATION;		

typedef struct 
{	
	META_DATA		cMetaData;
	CONFIGURATION	cConfiguration;
}CONFIG_DATA;


//-------------------------------------------------------------------- 
//		CTDS -> Configuration Thread Data Source
//		CTDS_CTRL_INTERFACE = 0
//		CTDS_FILE = 1
enum ConfigurationThreadDataSource{	
									CTDS_CTRL_INTERFACE,		//0
									CTDS_FILE,					//1
									
};	


//-------------------------------------------------------------------- 
//		CTEI -> Configuration Thread Event Index
//		CTEI_CTRL_INTERFACE = 0
//		CTEI_FILE = 1
enum ConfigurationThreadEventIndex{
									CTEI_CLEANUP,				//0
									CTEI_CTRL_INTERFACE,		//1
									CTEI_FILE,					//2

//ADD NEW INDEXES BEFORE THIS LINE!!!!!!!
									CTEI_MAX_INDEX
};


//--------------------------------------------------------------------
//				CFileMonitor class	//4.0.23.504 TD 78942
//--------------------------------------------------------------------

#define MAX_BUFFER  4096
#define IsValidHandle(hHandle) (INVALID_HANDLE_VALUE!=hHandle) ? TRUE : FALSE
#define FILE_ALREADY_EXISTS	183

class CFileMonitor
{
public:
	// Constructors
	CFileMonitor();
	~CFileMonitor();

	// Init methods
	void	Init(const BYTE* szDirectoryPath, const HANDLE hNotifyEvent);

	// Monitoring methods
	void	LookForFile();

	// File data methods	
	BOOL		GetFileData(BYTE** pszFileData, size_t* piDataLength);

	// File methods
	BOOL		IsNewFileAdded();
	BOOL		FindFile();
	void		DeleteCurrFile();	
	void		MoveCurrFile();

	// Cleanup methods
	void	CleanUp();

private:
	// Init methods
	void	InitDirInfo(const BYTE* szDirectoryPath, const HANDLE hNotifyEvent);
	void	InitFileInfo();

	BOOL	CreateMonitorDir() const;
	BOOL	OpenMonitorDir();

	// Clear methods
	void	ClearFileInfo();
	void	ClearDirInfo();

	// File methods
	void	GenerateFullFileName(BYTE* szFullFileName) const;
	BOOL	ReadFromFile(CXMLInterface& cXMLInterface) const;
	void	ClearReadOnlyAttr(BYTE* szFullFileName) const;

	// Log method
	void	WriteLogMsg(BOOL bSuccessMsg, const CString& strSuccessfulMsg, const CString& strFailedMsg, long lFailedLogLevel = LOG_LEVEL_1) const;
	void GenerateNewFullFileName(BYTE* szFullFileName) const;

private:
	//Dir Info
	HANDLE						m_hDirHandle;
	BYTE						m_szDirPath[MAX_PATH];
	OVERLAPPED					m_sDirOverlap;
	FILE_NOTIFY_INFORMATION*	m_pDirNotifyInfo;
	BYTE						m_szDirInfoBuff[MAX_BUFFER];
	DWORD						m_dwDirInfoLength;


	//File Info
	HANDLE				m_hFileHandle;
	BYTE				m_szFindSpec[MAX_PATH];
	WIN32_FIND_DATA		m_sFindFileInfo;
};



//--------------------------------------------------------------------
//				CMultipleEvents class	//4.0.23.504 TD 78942
//--------------------------------------------------------------------

class CMultipleEvents
{
public:
	// Constructors
	CMultipleEvents();
	~CMultipleEvents();

	void	Init(const size_t iEventsNum, const BOOL bManualReset = FALSE);

	long	WaitForSome(const long lMilliseconds);
	BOOL	WaitForOne(const long lMilliseconds, const long lIndexForWait);		// TBDIN
	BOOL	WaitForAll(const long lMilliseconds);								// TBDIN

	HANDLE	GetEvent(const long lIndex);

	inline BOOL		EventSignaled() const;
	inline void		ClearSignaled();

	BOOL	SetOne(const long lIndexForWait);
	void	SetAll();															// TBDIN
	
	void	CleanUp();

private:
	HANDLE*		m_pahEvents;
	size_t		m_iMaxEventIndex;

	BOOL		m_bFlagSignaled;
};


//--------------------------------------------------------------------
//				CConfigurationThread class	//4.0.22.43 TD 72716
//--------------------------------------------------------------------

#include "Thread.h"

class CConfigurationThread : public CThread  
{
public:

// CTS - > Configuration Thread State options
enum State{	CTS_WAIT_FOR_DATA,									//0		//IDLE
			CTS_APPLY_DATA,									//1
			CTS_START_LOCK,										//2
			CTS_WAIT_FOR_LOCK,									//3
			CTS_START_RECONFIGURE,								//4
			CTS_WAIT_FOR_RECONFIGURE,							//5
			CTS_START_UNLOCK,									//6
			CTS_WAIT_FOR_UNLOCK,								//7
			CTS_START_FORCE_UNLOCK,								//8
			CTS_WAIT_FOR_FORCE_UNLOCK,							//9

//ADD NEW STATES BEFORE THIS LINE!!!!!!!
			CTS_STATE_SIZE
			};


// CTAM  - > Configuration Thread Activation Mode options
enum ActivationMode{	CTAM_RECONFIGURE_WITH_LOCK,
						CTAM_RECONFIGURE_IMMEDIATE,
						CTAM_RECONFIGURE_ON_NEXT_STARTUP
					};

public:

	CConfigurationThread();
	virtual ~CConfigurationThread();

	// Initialization Methods
	CThreadRetCode		Init();
	void				LoadConfiguration(const BOOL bCreate);

	// Thread Control Methods
	virtual CThreadRetCode		StartThread();
	virtual CThreadRetCode		SetSleepDuration(const DWORD dwSleepDuration);
	virtual CThreadRetCode		StopThread();
	virtual CThreadRetCode		KillThread();

	// Main
	virtual void		ThreadRoutine(void* pData = NULL);	

	// Data Methods
	void				DataToProcess(const ConfigurationThreadDataSource eDataSource);			//4.0.23.504 TD 78942
	void				WaitForData();
	BOOL				HandleNewFile();														//4.0.23.504 TD 78942

	// Config Data Access Methods
	inline ActivationMode	GetActivationMode()const;
	inline BOOL				GetBackUp()const;													//4.0.22.72 TD 76419

	// State Machine Access methods
	inline void				SetStateMachine(const State eState);

	// Access methods 
	inline DWORD		GetLockTimeOut()const;
	inline BOOL			IsThreadAlive()const{ return m_bThreadAlive;}

	// Forecourt methods
	void				LockForecourt()const;
	void				UnLockForecourt()const;
	void				ForceUnLockForecourt()const;
	void				Reconfigure();

	BOOL				IsLockForecourtComplete()const;
	BOOL				IsUnLockForecourtComplete()const;
	BOOL				IsReconfigureComplete()const;
	
	void SetForecourtLocked( BOOL bServerLockState );
	BOOL IsForecourtLockedByConfigurationThread();

	// Thread Data methods
	BOOL				ApplyData();
	BOOL				SetThreadData(const BYTE* pszDataIn, const size_t iDataLength, const ConfigurationThreadDataSource eDataSource = CTDS_CTRL_INTERFACE);
	inline BOOL			IsEmptyThreadData()const;

	inline BOOL			IsDataFileInUse()const;

	void				SaveConfigurationData();	
		
	// Finalization Methods
	void				CleanUp();								

protected:

	// Config Data Methods
	BOOL				XMLData2RegTable();	

	// Thread Data methods
	void				ClearThreadData();

private:

//-----------------------------------------------------------------------
//					CConfigData	class
//-----------------------------------------------------------------------
	class	CConfigData
	{	
		
	public:

		CConfigData();
		~CConfigData(){};

		BOOL				SaveData2RegTable(CXMLInterface& cXMLInterface);

		ActivationMode		GetActivationMode()const;
		inline BOOL			GetBackUp()const;												//4.0.22.72 TD 76419

		// Reconfig data
		BOOL			IsReconfigDataEmpty() ;
		void			GetReconfigData(long* lTableId, long* lSubTableId, long* lSubTableIdExt, long* lFlags, BOOL* bBackUp);

	protected:

		// Set methods
		BOOL			SetMetaData(CXMLPointers* pAllRecordsList, CXMLInterface& cXMLInterface);
		BOOL			SetConfiguration(CXMLInterface& cXMLInterface, CXMLPointers& cActiveRecordsList);

	private:

		// Set methods
		BOOL			SetData(CXMLInterface& cXMLInterface, CXMLPointers& cActiveRecordsList, BOOL bFullDataDownload);
		BOOL			SetParameters(CXMLInterface& cXMLInterface, CXMLPointers& cActiveRecordsList);
		BOOL			SetParam(CXMLInterface& cXMLInterface, CXMLPointers& cActiveRecordsList);

		// Reconfig data
		void			SetReconfigData();
		void			ClearReconfigData();

		// Save param to registry
		void			SaveParam(const long lFlags);
		void ResetAllValidEntries();

	private:

#define MAX_CONFIGURATION_NUM 100
		CQueue<DATA, MAX_CONFIGURATION_NUM>		m_cReconfigDataQueue;
		CONFIG_DATA								m_cConfigDataStruct;
	};

//--------------------------------------------------------------------
//				CBaseState class 	//4.0.22.45 TD 72716
//--------------------------------------------------------------------

	class CBaseState
	{		
	public:
		virtual void	Operation(CConfigurationThread* cConfigurationThread) = 0;
	};

//--------------------------------------------------------------------
//				CWaitDataState class 	//4.0.22.45 TD 72716
//--------------------------------------------------------------------

	class CWaitForDataState : public CBaseState
	{
	public:
		virtual void	Operation(CConfigurationThread* cConfigurationThread);
	};


//--------------------------------------------------------------------
//				CApplyDataState class 	//4.0.22.45 TD 72716
//--------------------------------------------------------------------

	class CApplyDataState : public CBaseState
	{
	public:
		virtual void	Operation(CConfigurationThread* cConfigurationThread);
	};


//--------------------------------------------------------------------
//				CStartLockState class 	//4.0.22.45 TD 72716	//26/04/09 16:09
//--------------------------------------------------------------------

	class CStartLockState : public CBaseState
	{
	public:
		virtual void	Operation(CConfigurationThread* cConfigurationThread);
	};


//--------------------------------------------------------------------
//				CWaitForLockState class 	//4.0.22.45 TD 72716	//26/04/09 16:21
//--------------------------------------------------------------------

	class CWaitForLockState : public CBaseState, public CTimer
	{
	public:
		virtual void	Operation(CConfigurationThread* cConfigurationThread);

	private:

#define DEFAULT_LOCK_TIMEOUT			15														// 15 MINUTE
	};


//--------------------------------------------------------------------
//				CStartForceUnlockState class 	//4.0.22.45 TD 72716	//10/05/09 18:50
//--------------------------------------------------------------------

	class CStartForceUnlockState : public CBaseState
	{
	public:
		virtual void	Operation(CConfigurationThread* cConfigurationThread);
	};


//--------------------------------------------------------------------
//				CWaitForForceUnlockState class 	//4.0.22.45 TD 72716	//10/05/09 11:49
//--------------------------------------------------------------------

	class CWaitForForceUnlockState : public CBaseState
	{
	public:
		virtual void	Operation(CConfigurationThread* cConfigurationThread);
	};


//--------------------------------------------------------------------
//				CStartReconfigureState class 	//4.0.22.45 TD 72716	//26/04/09 16:26
//--------------------------------------------------------------------

	class CStartReconfigureState : public CBaseState
	{
	public:
		virtual void	Operation(CConfigurationThread* cConfigurationThread);

	private:

#define WAIT_FOR_RECONFIGURE_TIMEOUT	5 * MILLISECONDS_IN_A_SECOND	//4.0.22.60 TD 72716 TBD			// 5 SECONDS

	};


//--------------------------------------------------------------------
//				CWaitForReconfigureState class 	//4.0.22.45 TD 72716	//26/04/09 16:31
//--------------------------------------------------------------------

	class CWaitForReconfigureState : public CBaseState, public CTimer
	{
	public:
		virtual void	Operation(CConfigurationThread* cConfigurationThread);
	};


//--------------------------------------------------------------------
//				CStartUnlockState class 	//4.0.22.45 TD 72716	//26/04/09 16:45
//--------------------------------------------------------------------

	class CStartUnlockState : public CBaseState
	{
	public:
		virtual void	Operation(CConfigurationThread* cConfigurationThread);
	};


//--------------------------------------------------------------------
//				CWaitForUnlockState class 	//4.0.22.45 TD 72716	//26/04/09 16:49
//--------------------------------------------------------------------

	class CWaitForUnlockState : public CBaseState
	{
	public:
		virtual void	Operation(CConfigurationThread* cConfigurationThread);
	};



//--------------------------------------------------------------------
//				CStateMachine class 	//4.0.22.45 TD 72716
//--------------------------------------------------------------------

	class CStateMachine
	{
	public:

		CStateMachine();
		~CStateMachine();

		// Initialize methods
		void			Init();

		// Main
		void			Operation(CConfigurationThread* cConfigurationThread);

		// Access methods
		inline void		SetCurrentState(const State eState);

		// Control methods
		inline BOOL		IsInMiddleOfProcess()const;

	private:

		// Log methods
		void			StateChangedLogMsg(const State eNewState)const;

	private:

		State			m_eCurrState;	

		CBaseState*		m_cStates[CTS_STATE_SIZE];
	};
//--------------------------------------------------------------------

#define THREAD_SLEEP_DURATION 100								// 100 MiliSecond

	BOOL				m_bThreadAlive;

	CStateMachine		m_cStateMachine;
	CConfigData			m_cConfigData;

	// Configuration members
	DWORD				m_dwLockTimeOut;
	DWORD				m_dwWaitForDataInterval;
	BOOL				m_bLookForDataFile;
	BYTE				m_szDataFilePath[MAX_FILE_PATH];
		
	// Thread data members
	BYTE*				m_pszThreadData;
	size_t				m_iThreadDataLength;

	CRITICAL_SECTION	m_csThreadData;

	CFileMonitor		m_cFileMonitor;
	CMultipleEvents		m_cMultipleEvents;
};


#endif // !defined(AFX_CONFIGURATIONTHREAD1_H__2A9DAAC4_65E2_430E_8571_22C87E7AA7D0__INCLUDED_)
