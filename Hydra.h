// Hydra.h: interface for the CHydra class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HYDRA_H__1D11684B_F220_4709_B133_A649BF40D87F__INCLUDED_)
#define AFX_HYDRA_H__1D11684B_F220_4709_B133_A649BF40D87F__INCLUDED_

#import "HydService.tlb" no_namespace named_guids

#define MAX_SERVER_NAME				100
#define MAX_CLIENT_NAME				20

#define RTDB_TYPE	1
#define CFGDB_TYPE	2
#define CTRL_TYPE	3

#include "HydraDef.h"


enum CHydraRetCode {

					CHYDRA_OK,							//0
					CHYDRA_FAIL_CREATE_RTDB_CONN,		//1
					CHYDRA_FAIL_CREATE_CFGDB_CONN,		//2
					CHYDRA_SERVER_NAME_TOO_LONG,		//3
					CHYDRA_FAIL_CREATE_CTRL_CONN,		//4
					CHYDRA_COM_ERROR,					//5
					CHYDRA_FAIL_CREATE_RTEVENT_CONN,	//6
					CHYDRA_FAIL_CREATE_CFGEVENT_CONN,	//7	
					CHYDRA_NOT_CONNECTED,				//8
};

#define HYDRA_RTDB_INT					0x00000001
#define HYDRA_CFGDB_INT 				0x00000002
#define HYDRA_CTRL_INT					0x00000004
#define HYDRA_RTEVENTS_INT				0x00000008
#define HYDRA_CFGEVENTS_INT 			0x00000010
#define HYDRA_CTLEVENTS_INT				0x00000020
#define HYDRA_REDUNDANCY_BY_CL			0x00000040				////4.0.23.508 TD 89049

#define HYD_NOT_ACTIVE	0
#define HYD_CONN_FAIL1	1
#define HYD_CONN_FAIL2	2
#define HYD_CONN_FAIL3	3
#define HYD_ACTIVE		4

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHydra  
{
public:
	long TryToRestoreHydConn(HRESULT hRes = 0 );
	BOOL CheckConnectionStatus();
	void SetLastTime(SYSTEMTIME cSysTime);
	void SetHydStatus(long lStatus);
	virtual long WriteComError(_com_error & e )=0;
	long	CloseConnectionToHydra();
	long	InitHydra(long lHydraActive, char* pszServerPath, char* pszServerIniFilePath, long lInterfacesFlags, long lGroupNum,char* pszClientName,long lRecoveryTime); //4.0.12.506 add IniFileServerPath
	CHydra();
	virtual ~CHydra();

	BYTE				m_byDummyArray[2048]; //4.0.19.502
	IRTPtr				m_pRTDB;
	ICFGPtr				m_pCFGDB;
	ICTLPtr				m_pCTRL;
	long				m_lHydraActive;
	long				m_lGroupNum;
	long				m_lInterfacesFlag;
	char				m_szServerPath[MAX_SERVER_NAME];
	char				m_szServerIniFilePath[MAX_SERVER_NAME]; //4.0.12.506
	char				m_szClientName[MAX_CLIENT_NAME];

protected:
	long Restore_COM( HRESULT hResFailure, long lComType);
	long CreateConnectionToRT();
	long CreateConnectionToCFG();
	long CreateConnectionToCTL();
	long CloseConnectionToRT();
	long CloseConnectionToCFG();
	long CloseConnectionToCTL();
	long CreateConnectionToRTEvents();
	long CreateConnectionToCFGEvents();

private:
	SYSTEMTIME	m_cLastSysTime;
	long		m_lRecoveryTime;	//Interval time in minutes.
	long		m_lHydStatus;	
};

#endif // !defined(AFX_HYDRA_H__1D11684B_F220_4709_B133_A649BF40D87F__INCLUDED_)
