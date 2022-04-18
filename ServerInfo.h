// ServerInfo.h: interface for the CServerInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERINFO_H__013B7D7E_DBAA_4DA3_A95C_576921AA6957__INCLUDED_)
#define AFX_SERVERINFO_H__013B7D7E_DBAA_4DA3_A95C_576921AA6957__INCLUDED_

#include "Pump.h"	// Added by ClassView
#include "DeliveryEmulationSingleTankServerInfo.h"//4.0.21.500 - TD 66935
#include "ReportGenerator.h"
#include "ChangeProductInTank.h"
#include "PriceChangeWithAckPerPump.h"
#include <afxmt.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServerInfo  
{
public:
	void SetWaitForPriceChangeAckFlags(BOOL bVal , BOOL bMarkActivePump = FALSE);	
	BOOL GetWaitForPriceChangeAckFlags();
	void ResetServerInfo();
	void ClearDeliveryEmulationFlags();
	void ClearShiftFlags();
	ChangeState GetPriceChangeState();
	void SetPriceChangeState(ChangeState lStatus);
	void UnLock();
	void Lock();
	long GetInfo(SERVER_INFO & rec);
	long SetInfo(SERVER_INFO & rec);
	void ClearAlarmFlags();
	void ClearDeliveryReportFlags();
	void ClearPumpTotalsFlags();
	void ClearAllWetStockFlags();
	void ClearTankReadingFlags();	
	BYTE GetDeliveryEmulationProcessType();
	void SetDeliveryEmulationProcessType(BYTE byVal);
	BYTE GetDeliveryEmulationStatus();
	void SetDeliveryEmulationStatus(BYTE byVal);
	BYTE GetDebitShiftProcessFlags();
	void SetForceShiftOLAFlags(BYTE byVal);
	BYTE GetForceShiftOLAFlags();
	void SetDebitShiftProcessFlags(BYTE byVal);
	BOOL GetMemoProcessFlags();	
	void SetMemoProcessFlags(BOOL bVal);
	//4.0.9.503BYTE GetWetStockFlags();
	long GetWetStockFlags();
	//4.0.9.503void SetWetStockFlags(BYTE byVal);
	void SetWetStockFlags(long lVal);  //4.0.9.503
	BOOL GetRequiredMaintenanceProcessFlags();
	BOOL GetLockByUpsFlags(); //4.0.14.502
	void SetLockByUpsFlags(BOOL bVal); //4.0.14.502
	void SetRequiredMaintenanceProcessFlags(BOOL bVal);
	BOOL GetInStopAllPumps();          //4.0.13.80
	void SetInStopAllPumps(BOOL bVal); //4.0.13.80
	BOOL GetRequiredModeProcessFlags();
	void SetRequiredModeProcessFlags(BOOL bVal);
	BOOL GetShiftLockFlags();
	void SetShiftLockFlags(BOOL bVal);
	BOOL GetLockForecourtOnPriceChangeFlags();
	void SetLockForecourtOnPriceChangeFlags(BOOL bVal);
	BOOL GetRequiredPriceChangeFlags();
	void SetRequiredPriceChangeFlags(BOOL bVal);
	ChangeState GetShiftChangeStatus();
	void SetShiftChangeStatus(ChangeState    eShiftChangeState);
	BOOL GetRequiredShiftProcessFlags();
	void SetRequiredShiftProcessFlags(BOOL bVal);
	long GetServerReservation();
	void SetServerReservation(long lVal);
	LockStatus GetServerStatus();
	void SetServerStatus(LockStatus eStatus);

	void SetInPriceChangeProcess(BOOL bVal); //4.0.12.515
	BOOL GetInPriceChangeProcess(); //4.0.12.515
	void SetSendMemoPumpEvent(const BOOL bVal); //4.0.15.182
	BOOL GetSendMemoPumpEvent(); //4.0.15.182

	//4.0.18.20 - TD 33898
	BOOL GetRequiredQDXMaintenance();
	void SetRequiredQDXMaintenance(BOOL bVal);

	BYTE GetDeliveryEmulationSingleTankStatus(const long lTankNumber);						                        //TD 66935
	void SetDeliveryEmulationSingleTankStatus(const long lTankNumber, const BYTE byVal);	                        //TD 66935
	ProcessDeliverySingleTank GetDeliveryEmulationSingleTankProcessType(const long lTankNumber);				    //TD 66935
	void SetDeliveryEmulationSingleTankProcessType(const long lTankNumber, const ProcessDeliverySingleTank byVal);  //TD 66935
	BYTE GetDeliveryEmulationSingleTankSuccess(const long lTankNumber);						                        //TD 66935
	void SetDeliveryEmulationSingleTankSuccess(const long lTankNumber, const BYTE byVal);	                        //TD 66935
	SingleTankProcessStopped GetDeliveryEmulationSingleTankProcessStopped(const long lTankNumber);                  //TD 66935
	void SetDeliveryEmulationSingleTankProcessStopped(const long lTankNumber, const SingleTankProcessStopped byVal);//TD 66935
	SingleTankLockStatus GetSingleTankLockStatus(const long lTankNumber);					                        //TD 66935
	void SetSingleTankLockStatus(const long lTankNumber, const SingleTankLockStatus byVal);                         //TD 66935
	void ClearSingleTankFlags(); //TD 66935
	void ClearSingleSpecificTankFlags(const long lTankNumber);//TD 66935

	//4.0.22.501 - TD 69905
	void SetReportRequestState(const ReportRequestState eState, const char* const callerFunction);               // TD 351225
	ReportRequestState GetReportRequestState();                                                                  // TD 351225
	void SetReportRequestFailureResult(const ReportRequestError eError, const char* const callerFunction);       // TD 351225
	long GetReportRequestFailureResult();
	void HandleGenerateReportProcess(BOOL bInit = FALSE, long lFlags = 0, BOOL bReportDone = FALSE);
	void SetReportBuffer(BSTR wszReportText);
	_bstr_t GetReportBuffer();
	SYSTEMTIME GetReportStartTime() {return m_cReportGenerator.GetStartTime();}

	//4.0.22.503 - TD 72189
	long HandleChangeProductInTankRequest(BSTR bstrRec, BOOL bSetupOnly = FALSE);
	void HandleChangeProductInTankProcess();
	void SetChangeProductInTankState(const long lState);
	long GetChangeProductInTankState();
	void SetChangeProductInTankFailureResult(const long lError);
	long GetChangeProductInTankFailureResult();
	void GetChangeProductInTankInfo(BSTR *pbstrData);
	void GetChangeProductInTankXmlResultBuffer(BSTR *pbstrRec);	
	void SetChangeProductTankId(long lTankNumber);
	void SetChangeProductGradeId(long lGradeNumber);
	void SetChangeProductName(char * sProductName);

	//4.0.19.820 - TD 78775 - Price change with ack per pump process
	BOOL GetInPriceChangeWithAckProcessFlag(); 
	void SetInPriceChangeWithAckProcessFlag(BOOL bInPriceChangeWithAckProcess);
	void StartPriceChangeWithAckPerPumpProcess();
	void SetPriceChangeAckResult(long lPumpNumber, long lErrorCode);
	long GetPriceChangeWithAckFailureResult();
	long GetPriceChangeWithAckState();
	void GetPriceChangeWithAckXmlResultBuffer(BSTR *pbstrBuff);
	BOOL IsPriceChangeWithAckReceivedForAllPumps();
	void SetPriceChangeWithAckRefNumber(long lRefNumber);
	void HandlePriceChangeWithAckPerPumpProcess();
	void SetPriceDisplayStatus(long lVal){m_lPriceDisplayStatus = lVal;}
	long GetPriceDisplayStatus(){return m_lPriceDisplayStatus;}

	
	
	BYTE GetInPriceChangeWithIntegrityProcessFlags();										//4.0.27.20 147183
	void SetInPriceChangeWithIntegrityProcessFlags(BYTE byPriceChangeWithIntegrityFlags);	
	void SetPriceChangeResponseFromIntegritiyProcessFlag();									
	BOOL IsInCommitPriceChangeWithIntegrityProcess();										
	BOOL IsInPriceChangeWithIntegrityProcess();												
	BOOL IsPendingCommitPriceChangeWithIntegrityProcess();									
	
	CServerInfo();
	virtual ~CServerInfo();

public:
		CCriticalSection	m_csServerInfo;
private: 	
	LockStatus		m_eLockStatus;
	long			m_lLockResExt;
	BYTE			m_bRequiredShiftProcess;
	ChangeState		m_eShiftChangeState;
	BYTE			m_bRequiredPriceChange;
	ChangeState		m_lPriceChangeState; 	
	BYTE			m_bPriceChangeLock; 
	BYTE			m_bShiftChangeLock; 
	BYTE			m_bRequiredModeProcess;
	BYTE			m_byRequiredMaintenanceProcess;
	BYTE			m_byLockByUps;     //4.0.14.502
	BYTE			m_bInStopAllPumps; //4.0.13.80
	//4.0.9.503BYTE	m_byWetStockFlags;
	BYTE			m_byMemoInProcess;	
	BYTE			m_byDebitShiftInProcess; 
	BYTE			m_byFoceShiftOLA;			
	BYTE         	m_byDeliveryEmulationStatus;  
	BYTE            m_byDeliveryEmulationProcess;
	BYTE			m_byWaitForPriceChangeAck; //4.0.9.503	
	long			m_lWetStockFlags;          //4.0.9.503
	BYTE			m_bInPriceChangeProcess;   //4.0.12.515
	BOOL			m_bSendMemoPumpEvent;
	BOOL			m_bRequiredQDXMaintenance; //4.0.18.20 - TD 33898
	CDeliveryEmulationSingleTankServerInfo m_cDeliveryEmulationSingleTankServerInfo; //4.0.21.500 - TD 66935
	CReportGenerator m_cReportGenerator;						//4.0.22.501 - TD 69905
	CChangeProductInTankManager m_cChangeProductInTankManager;  //4.0.22.503 - TD 72189
	BOOL			m_bInPriceChangeWithAckPerPumpProcess;		//4.0.19.820 - TD 78775
	CPriceChangeWithAckPerPump m_cPriceChangeWithAckPerPump;	//4.0.19.820 - TD 78775
	BYTE			m_byInPriceChangeWithIntegrityProcessFlags;	//4.0.26.508	147183	
	BOOL			m_lPriceDisplayStatus;
	
};


#endif // !defined(AFX_SERVERINFO_H__013B7D7E_DBAA_4DA3_A95C_576921AA6957__INCLUDED_)
