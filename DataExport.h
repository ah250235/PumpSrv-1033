// DataExport.h: interface for the CDataExportThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAEXPORT_H__F8CF1EC6_043F_429F_958E_05491485DF4B__INCLUDED_)
#define AFX_DATAEXPORT_H__F8CF1EC6_043F_429F_958E_05491485DF4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Send in the InitInterface
#define CLIENT_PUMPSRV					1

// Connection to PumpSrv flags
#define CONNECT_TO_PUMPSRV				0x00000001
#define DISCONNECT_FROM_PUMPSRV			0x00000002

//Data Formatsenum
enum CDataExportRetCode {

					DATA_EXPORT_OK,							//0
					DATA_EXPORT_FAIL_CREATE_INTERFACE,		//1
					DATA_EXPORT_COM_ERROR,					//2					
					DATA_EXPORT_NOT_CONNECTED,			    //3
					DATA_EXPORT_CUSTOMIZE_ERROR,			//4
					DATA_EXPORT_LOG_UNCUSTOMIZED,			//5
};

//#define CUSTOMIZE_DATA_FORMAT_NONE  0
//#define CUSTOMIZE_DATA_FORMAT_EPICS 1

//Operation codes
#define DATAEXPORT_OPCODE_NONE		0
#define DATAEXPORT_OPCODE_INIT_INT	1
//flags
#define DATAEXPORT_USES_SINGLE_CORE	0x00000001 //4.0.24.90 106227

#import "DataExportSrv.exe"  no_namespace named_guids

class CDataExportThread : public CThread 
{
public:	
	BOOL IsActive();
	long NotifySystemStartup();
	CThreadRetCode DataExportSrvInit(BOOL bActive);
    CThreadRetCode DataExportCleanUp(BOOL bDisconnFromPS);

	CDataExportThread();
	virtual ~CDataExportThread();

protected:
	long DisconnectFromPumpSrv();
	

private:
	CDataExportRetCode InitDataExportInt();
	CString CreateDataExportLog(CDataExportRetCode cDataExportRetCode);
	void SetDataExportOpCode(long lOpCode);
	long WriteComError(_com_error &e);
	long Restore_COM(); 
	CDataExportRetCode InitExportData();
	CDataExportRetCode CloseConnectionDataExport();
	CDataExportRetCode ConnectDataExport();
	virtual void ThreadRoutine(void *pData);
	IExportPtr	m_pExport;
	long		m_lOpCode;
	BOOL		m_bActive;
};

#endif // !defined(AFX_DATAEXPORT_H__F8CF1EC6_043F_429F_958E_05491485DF4B__INCLUDED_)
