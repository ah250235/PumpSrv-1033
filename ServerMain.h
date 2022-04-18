// ServerMain.h: interface for the CServerMain class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "IniFile.h"
#include "ServerInfo.h"	// Added by ClassView
#include "DB.h"	// Added by ClassView
#include "TerminalMessages.h"	// Added by ClassView
#include "AlarmSrvPS.h"	// Added by ClassView
#include "XmlDefinitions.h"
#include "XmlParser.h"
#include "FlowControlManager.h"
#include "Ccard.h"
#include "TaxCalc.h"
#include <list>
#include "SQLPumpTotalsFile.h"
#include "SQLTankReadsFile.h"
#include "SQLFuelDeliveriesFile.h"
#include "SQLPrePayFile.h"
#include "SQLExtraInfoFile.h"
#include "SQLOLATempTrs.h"
#include "SQLOLATrs.h"
#include "SQLCarWashFile.h"
#include "SQLItemsFile.h"
#include "SQLTankGauge.h"
#include "SQLPaidTrsFile.h"
#include "SQLAsh2000.h"
#include "SQLPumpSrvSingleShiftAccumulation.h"
#include "SQLPumpSrvReceiptExtraInfoDB.h"
#include "AlarmClass.h"
#include "FuelCommonDef.h"


// typedef std::list<CTendersCard> TendersCards;					//CR 404913
// typedef std::list<CTendersCard>::iterator CTendersCardsIterator;//CR 404913
// typedef std::list<CTendersCard> TendersCards_copy;

#import "IFSFTerminalSrv.tlb"  no_namespace named_guids //4.0.12.503
#import "FuelMobileSrv.tlb"  no_namespace named_guids

#include <afxmt.h>
#include "ConfigurationThread.h"				//4.0.22.43	TD 72716
#include "ProjectRemark.h"
#include "Logger.h"			
#include "CmdInfo.h"	
#include "PosInfo.h"
#include "Pump.h"	// Added by ClassView
#include "OLABase.h"    //Eyal 4.0.0.22
#include "Param.h"	// Added by ClassView
#include "CarWash.h"
#include "Tank.h"
#include "Report.h"
#include "OLA.h"	// Added by ClassView
#include "EventManager.h"	// Added by ClassView
#include "ProtectedDataHandler.h"		//4.0.22.501 TD 72716
#include "CompChecker.h"
#include "HydraPS.h"
#include "DebitInfo.h"
#include "DeviceRecord.h"	// Added by ClassView
#include "VpMng.h"	// Added by ClassView
#include "RegTable.h"
#include "TrsLog.h"	// Added by ClassView
#include "PumpLock.h"
#include "InitDebitHandler.h"	// Added by ClassView
#include "CommonFunction.h"	// Added by ClassView
#include "CommonStruct.h"  //4.0.9.500 added by marik * turj
#include "SinglePumpLock.h" 
#include "CompConnectivity.h"	// Added by ClassView
#include "AccumulationShiftTotal.h"	// Added by ClassView
#include "TankGaugeFactor.h"	// Added by ClassView
#include "DataExport.h"	// Added by ClassView
#include "ReceiptExtraIndexFile.h"	// Added by ClassView
#include "ShiftProcess.h"	// Added by ClassView
#include "PriceChangeFlags.h"	// Added by ClassView
#include "BaseReceipt.h"//4.0.9.503
#include "Track2Receipt.h"//4.0.9.503
#include "AccountReceipt.h"//4.0.9.503
#include "ProtectedReceipt.h"	//4.0.20.501 TD 60236
#include "CardSaleAllData.h"//4.0.9.506
#include "TankLevelProtection.h"//4.0.9.506 //amram
#include "Grade.h"//4.0.9.506 //amram
#include "FuelLoyaltyPSR.h"//4.0.9.508
#include "CashAcceptorFile.h"
#include "ConvertXML.h"//4.0.12.501
#include "GPIEventManager.h"
#include "TotalPumpInfo.h"//4.0.12.506
#include "TankXML.h" //4.0.13.503
#include "XmlConvertorEx.h" //4.0.14.500 
#include "QDXMaintThread.h" //4.0.18.0
#include "PumpLockByTank.h"
#include "FuelingRestriction.h"	//4.0.22.501 TD 70956
#include "VPThread.h"			//4.0.24.60

#include "WetStockDetails.h"	// Added by ClassView
#include "BuildLine2PosPump.h"	// Added by ClassView
#include "GradeData.h"			// Added by ClassView

#include "PumpTotalsXml.h"		//4.0.25.41 - TD 108311
#include "SingleShiftAccumulations.h" //4.0.25.80 - TD 108311
#include "SingleShiftAccumMgr.h"	  //4.0.25.80 - TD 108311
#include "ExtendedGradeMap.h"			//4.0.26.501 - TD 116227
#include "SiteConfigurationExporter.h"	//4.0.26.508 - TD 146575	
#include "FlowRate.h"			
#include "FuelMobileSrvThread.h"
#include "AbstractDBManager.h"
#include "ThreadedQueue.h"


enum _DbManagerRetCode;
typedef _DbManagerRetCode DbManagerRetCode;

#define MAX_XML_BUF 15000 //4.0.12.501
#define QDX_XML_LEN 991 //4.0.12.501

#define PUMP_NONE_FIRST			1
#define PUMP_READY_FIRST		2
#define PUMP_WAIT_FIRST			3	


//session parameter 
#define TIME_OUT_USUAL_SESSION		0
#define TIME_OUT_BEBIT_SESSION		1
#define TIME_OUT_MAX_SESSION		2

#define AUTOMATIC_PAK_RES			TRUE

#define OLA_MODE_WAIT				0 // do not authorize card until Card approved
#define OLA_MODE_HOT				1 // authorize any card immidietly.

#define TIME_BETWEEN_RETRYING_RESERVED_PRE_PAID_PUMP   12  //4.0.24.351 //4.0.124.355 //4.0.27.60 231255

#define SHIFT_MODE_SINGLE			0 //  
#define SHIFT_MODE_AUTO				1 //  


enum PriceChangeResponse {
	PRICE_CHANGE_NO_RESPONSE,   //0
	GOT_PRICE_CHANGE_SUCCESS,   //1
	GOT_PRICE_CHANGE_FAILURE,   //2
	GOT_PRICE_CHANGE_PENDING_COMMIT, //3 //4.0.26.508 147183
	GOT_PRICE_CHANGE_PENDING_COMMIT_AFTER_FAILURE, //4 //4.0.26.508 147183
}; 


enum GradePriceType{
	HIGHEST_PRICE = -2,  //RFUEL-3309 Get the maximal discount allowed for the grade 
	LOWEST_PRICE = -1,  
	SELF_CASH = 0,
	FULL_CASH,
	SELF_CREDIT,
	FULL_CREDIT,
	PRICE_PROTECTION_LOWEST_PRICE
};


//4.0.27.20 - 147183
#define PRICE_CHANGE_WITH_INTEGRITY_NONE							0x00
#define PRICE_CHANGE_WITH_INTEGRITY_ACTIVE							0x01
#define PRICE_CHANGE_WITH_INTEGRITY_PENDING_COMMIT					(0x02 | PRICE_CHANGE_WITH_INTEGRITY_ACTIVE)
#define PRICE_CHANGE_WITH_INTEGRITY_PENDING_COMMIT_AFTER_FAILURE	(0x04 | PRICE_CHANGE_WITH_INTEGRITY_ACTIVE)
#define PRICE_CHANGE_WITH_INTEGRITY_GOT_COMMIT						(0x08 | PRICE_CHANGE_WITH_INTEGRITY_ACTIVE)

#define TEMPORARY_DEBUG		1
#define SPEEDPASS_SIMULATOR	0
#define CATCH_GCI		1
#define CATCH_GPI		1
#define	CATCH_CTRL		1

enum OperatingMode {
				NULL_MODE,
				MASTER,		   // Initializing
				SLAVE,
};

enum eFuelLoyaltyState
{
		No_Loyalty,							//0
		PAP_Loyalty_Swiped,					//1	//4.0.22.500
		PAP_Payment_Swiped,					//2
		PAK_Loyalty_Swiped,					//3
		PAK_Payment_Swiped,					//4
		PAP_No_Card,						//5
		PAK_No_Card,						//6
		PAP_PaymentFirst_Payment_swiped,	//7
		PAP_PaymentFirst_Loyalty_swiped,	//8
		PAP_Payment_Scanned,				//9	//4.0.20.440 68857	//4.0.21.501
		PAP_Prepaid_Voucher,				//10	//4.0.22.500 70767
		PAP_No_Payment,						//11 4.0.22.504 69344
};

typedef struct
{
	BOOL bValid;
	char sTaxName[21];
	DWORD lTaxId;
	DWORD lTaxRate;
	DWORD bIsIncluded;
	char sTaxPrintChar;
	DWORD bIsTaxTypeCPL;
}TAX_PROPERTIES;

typedef struct
{
	DWORD m_dwGradesCode;
	DWORD m_dwTaxLink;
	BOOL m_bTypeOfMeasure; // TD 363041
	CString m_sFullName;
	CString m_sAdditionalCode;
}GRADE_PROPERTIES;

typedef struct
{
	DWORD m_dwPrice;
	DWORD m_dwTaxLink;
	DWORD m_dwOrder;
	CString m_sProduct_Code;
	CString m_sFullName;
}WASH_PROGRAMS_PROPERTIES;

typedef struct
{
	DWORD dwCarWashLoyaltyApplicable;
	DWORD dwCarWashPluCode;
	DWORD dwDiscountCode;
	DWORD dwMinGallons;
	DWORD dwDiscountPerMinGals;
	DWORD dwCarwashScaleFactor;
	CString sType;
}CARWASH_GENERAL_DEFINITIONS;


//4.0.25.80
enum eGeneralRetCode {
		RetCode_Critical_Failure = -1,	//-1		//4.0.25.81
		RetCode_Success,				//0
		RetCode_Failure,				//1
}; 

#define ALRMSRV_NO_ACTIVE				0	
#define ALRMSRV_WITH_CONVERT_ACTIVE		1
#define ALRMSRV_DIRECT_ACTIVE			2


#define ALARM_TYPE_MESSAGE				8   //4.0.3.14  


// StationLimitType
#define STATION_LIMIT_BY_VALUE		1
#define STATION_LIMIT_BY_VOLUME		2

#define SERVER_STATE_EX_SHOW_GPI_MSG_CONVERT_LAYER_GONE		(_Module.m_server.GetServerStateEx(SSEF_SHOW_GPI_MSG_CONVERT_LAYER_GONE) & SSEBM_SHOW_GPI_MSG_CONVERT_LAYER_GONE)		//4.0.20.950 TD 97894
#define SERVER_STATE_IDLE									(_Module.m_server.GetServerState() == SERVER_IDLE)//4.0.3.37
#define SERVER_NOT_IDLE										(_Module.m_server.GetServerState() != SERVER_IDLE)
#define SERVER_STATE_STARTUP								(_Module.m_server.GetServerState() == SERVER_STARTUP)
#define PUMP_NOT_VALID(pump)								(_Module.m_server.PumpNotValid(pump))
#define PUMP_VALID(pump)									(!_Module.m_server.PumpNotValid(pump))
#define GRADE_NOT_VALID(grade)								(_Module.m_server.GradeNotValid(grade))
#define LEVEL_NOT_VALID(level)								(_Module.m_server.LevelNotValid(level))
#define NOZZLE_NOT_VALID(nozzle)							(_Module.m_server.NozzleNotValid(nozzle))
#define TANK_NOT_VALID(tank)								(_Module.m_server.TankNotValid(tank))
#define PUMP_NOT_IN_WORKING_STATE(pump)						(_Module.m_server.PumpNotInWorkingState(pump))
#define MODE_NOT_VALID(mode)								(_Module.m_server.ModeNotValid(mode))
#define MODE_VALID(mode)									(FALSE == _Module.m_server.ModeNotValid(mode))
#define SCHEME_NOT_VALID(scheme)							(_Module.m_server.SchemeNotValid(scheme))
#define SERVICE_FEE_NOT_VALID(ServiceNumber)				(_Module.m_server.ServiceFeeNotValid(ServiceNumber))
#define SERVICE_MODE_NOT_VALID(ServiceMode)					(_Module.m_server.ServiceModeNotValid(ServiceMode))

#define IS_SERVER_IN_MIDDLE_LOCK_SHIFT				(_Module.m_server.IsActiveShiftWithLock())  //4.0.1.40

// support old PAK and new  PAK with loyalty
#define PAK_LINK_TYPE(lLinkNumber)				(_Module.m_server.PakLinkNumber(lLinkNumber))		
// support old PrePay and new PrePay  with loyalty
#define PRE_PAY_LINK_TYPE(lLinkNumber)				(_Module.m_server.PakPayLinkNumber(lLinkNumber))

#define PUMPSRV_IS_MASTER		(_Module.m_server.GetOperatingMode() == MASTER)			
#define PUMPSRV_IS_SLAVE		(_Module.m_server.GetOperatingMode() == SLAVE)			
#define PUMPSRV_IS_NOT_MASTER	(_Module.m_server.GetOperatingMode() != MASTER)			
#define PUMPSRV_IS_NOT_SLAVE	(_Module.m_server.GetOperatingMode() != SLAVE)			
#define PUMPSRV_NOT_INIT		(PUMPSRV_IS_NOT_MASTER && PUMPSRV_IS_NOT_SLAVE)
#define PUMPSRV_REPORT_ON_TRS_PAID	(_Module.m_server.m_bControllerReportOnPaidTrs)  //4.0.5.33  

#define OLA_BY_CONTROLLER		(_Module.m_server.OLAByController() != 0)
#define OLA_BY_PUMPSRV			((_Module.m_server.OLAByController() == 0) || (_Module.m_server.OLAByController() == 2))   //4.0.19.509

#define PUMPSRV_IS_MODE_MASTER			(!_Module.m_server.m_bControllerManagePumpMode)
#define CONTROLLER_IS_MODE_MASTER		(_Module.m_server.m_bControllerManagePumpMode)
#define PUMPSRV_IS_PUMP_LOCK_MASTER		(!_Module.m_server.m_bControllerManagePumpLock)
#define CONTROLLER_IS_PUMP_LOCK_MASTER	(_Module.m_server.m_bControllerManagePumpLock)
#define CONTROLLER_SUPPORT_AVI			(_Module.m_server.m_bControllerSupportAVI)

#define PUMPSRV_IS_MODE_MASTER2			(_Module.m_server.m_bControllerManagePumpMode2) //4.0.5.39  //4.0.5.41  
#define CONTROLLER_MANAGE_PRINTER_STATUS (_Module.m_server.m_bControllerManagePrinterStatus) //4.0.5.44  

//Hot Pumping
#define PUMP_PAP_CANCELED(pump)			(_Module.m_server.m_cPumpArray[pump-1].GetStat().GetMisc() & MISC_CURRENT_TRS_PAP_CANCELED)
#define PUMP_NOT_PAP_CANCELED(pump)		(!(PUMP_PAP_CANCELED(pump)))
#define PUMP_STOPPED_BY_PUMPSRV(pump)	(PUMP_PAP_CANCELED(pump))

#define SOURCE_GPI				0
#define	SOURCE_GCI				1
#define SOURCE_INTERNAL			2

#define TRS_FIELD_NUMBER				0x00000001
#define TRS_FIELD_STATUS				0x00000002
#define TRS_FIELD_ON_OFFER				0x00000004
#define TRS_FIELD_RAW_VOLUME			0x00000008
#define TRS_FIELD_ROUNDED_VOLUME		0x00000010	
#define TRS_FIELD_ROUNDED_VALUE			0x00000020
#define TRS_FIELD_GRADE_PRICE			0x00000040
#define TRS_FIELD_NOZZLE				0x00000080
#define TRS_FIELD_GRADE					0x00000100
#define TRS_FIELD_LEVEL					0x00000200
#define TRS_FIELD_PUMP					0x00000400
#define TRS_FIELD_MODE					0x00000800
#define TRS_FIELD_RES					0x00001000
#define TRS_FIELD_RESEXT				0x00002000
#define TRS_FIELD_LINK_NUMBER			0x00004000
#define TRS_FIELD_LINK_FLAGS			0x00008000
#define TRS_FIELD_ACTIVE_READ_NUMBER	0x00010000
#define TRS_FIELD_ACTIVE_SHIFT_NUMBER	0x00020000
#define TRS_FIELD_MISC					0x00040000

#define EVENT_THRD_SLEEP_TIME			100		// milisec	//from500to100 4.0.26.503 TD 165971
#define ALARM_SRV_THRD_SLEEP_TIME		100		//4.0.2.27

#define IN_MIDDLE_OF_WET_STOCK			999		//4.0.13.501


#define MAX_COM_ATTEMPTS				3 
#define INTERAC_CARD_AID				"A0000002771010" //274401
#define INTERAC_CARD_AID_SIZE			14			//274401

enum ThreadState {
				THREAD_STOPPED_STATE,     //4.0.1.40
				THREAD_STARTED_STATE,	    //4.0.1.40
}; 

//Registry Refresh Types 
#define REFRESH_REGISTRY_KEY		1
#define REFRESH_REGISTRY_TABLE		2

#define VP_THRD_SLEEP_TIME			500 // milisec

#define POS_NUMBER_PAID_QDX      100 //4.0.11.503 add to PumpNumber

#define TIME_BETWEEN_LOYALTY_TIMEOUT_CHECK		5000 //  milisec (= 10 sec) 	//4.0.23.1050 TD 233242
#define LOYALTY_START_TRS_TIMEOUT				20    //  seconds
#define LOYALTY_QUERY_TIMEOUT					20    //  seconds					

#define ROUNDING_FACTOR							5

enum eWideTrsCancel
{
	TRS_CANCEL = 0,
	TRS_NOT_FOUND,
	TRS_CANT_CANCEL,
	TRS_NOT_PREPAY,
	TRS_NOT_CURRENT,
	TRS_FAILED_IO,
	TRS_CANT_CANCEL_PREPAY_FROM_FORECOURT
}; // NO TD

// CR 438993
#define PRICE_PROTECTION_NOT_ACTIVE					0
#define PRICE_PROTECTION_ACTIVE_INSIDE_TIME_FRAME	1
#define PRICE_PROTECTION_ACTIVE_OUTSIDE_TIME_FRAME	2

class CServerMain 
{
friend class CRegTable;
public:

	void HandlePunchhAlternateIDfailure(long lPumpNumber, OLA_STAT * pOla, PAY_AT_PUMP_INFO* cInfo);
	long IsRollingUpdateDataNeedtoAddToLog() const { return m_dwAddRollingUpdateDataToLog; }
	void ResetPriceChangedFlags();

	// CR 438993
	long AdjustPriceProtectionDiscount(long & lGRDiscount, CPumpTransact *pTrs);
	void PriceChangeNoChangeEvent();
	long GetGradePriceMinimumLimit();

	void UnRegisterEOSReconfigureEvent(IN ULONG ulTableId);	
	void RegisterEOSReconfigureEvent(IN CMD_STRUCT_RECONFIGURATION_EVENT const * const pRecEvent);
	void UnRegisterEOSReconfigureEvent(IN CMD_STRUCT_RECONFIGURATION_EVENT const * const pRecEvent);
	BOOL IsRegistredReconfigureEvents();
	void SetSwipeResultDataToCommand(CMD_STRUCT_PAP_SWIPE_RESULT3_EX & cmdInfo, PAY_AT_PUMP_INFO &cTmpInfo, long lAnswer);


	void SaveTenderAuthorizedAmount( long lPumpNumber, PAY_AT_PUMP_INFO * pInfo );
	void SaveTenderData(long lPumpNumber, PAY_AT_PUMP_INFO * pInfo);		//CR 404913 
	void ClearTendersCards(long lPumpIndex);

	DWORD   m_dwFlowControlType;
	DWORD	m_dwLoyaltyClubNumber;
	DWORD	m_dwLoyaltyClubNumberRetry;
	DWORD	m_dwLoyaltySendAllTrsToLoyalty;
	DWORD	GetFlowControlType() const { return m_dwFlowControlType	; }
	long	GetCustInfoRetryRequest(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola);
	long	SendEventKillOla();
	CGradeData m_GradeData;

	DWORD   m_dwSortedLoyaltyMessages;
	DWORD   IsSortedLoyaltyMessages() const { return m_dwSortedLoyaltyMessages; }
	BOOL    m_bTwoTendersInTransaction;

	void ConcatenateLoyaltyReceiptMessages(CXMLInterface & xmlFatherDoc, CXMLPointers & pCurrElement, char * sFileName, long  & lPumpNumber);
	long GetNumberOfTendersAuthotized(long lPumpIndex) const { return m_lNumberOfTendersAuthorized[lPumpIndex]; }
	void ResetNumberOfTendersAuthorized(long lPumpIndex) { m_lNumberOfTendersAuthorized[lPumpIndex] = 0; }
	void IncrementNumberOfTendersAuthorized(long lPumpIndex) {  m_lNumberOfTendersAuthorized[lPumpIndex]++; }
	CARWASH_GENERAL_DEFINITIONS	 m_CarWashGeneralDefinitions;
	
	DWORD IsRemoveStuckTrsConfigured() const { return m_dwRemoveStuckTrs;} // 4.0.24.770  TD 275612
	BOOL SearchForStuckTrs(long lPumpNumber); // 4.0.24.770  TD 275612
	void LoadLoyaltyReceiptInfo( CXMLInterface & xmlFatherDoc ,CXMLPointers & pCurrElement, char * sFileName); // 4.0.111.171
	void SetPrePayReTryReservation(long lPumpNumber, long lVal); //4.0.24.351 //4.0.124.355 //4.0.27.60  231255
	BOOL ReachPrePayReTryReservation(long p); //4.0.24.351 //4.0.124.355 //4.0.27.60 231255
	long IsAllowedMultiPosOwnersOfPumpTrs(){return m_dwAllowMultiPosOwnersOfPumpTrs;};	//4.0.23.501 78146
	//4.0.19.504 - 47559
	void SendFormatReceiptData(long lPosNumber ,CComBSTR  & sReceipt);
	void ParseReceiptRowDataArray(BSTR  & sArrayBuffer , long  & lRecords , long & lPosNumber);
	void ParseReceiptRowDataArrayAsOLARec(BSTR  & sArrayBuffer, long & lRecords , long & lPosNumber);
	long GetTaxID(long lTax);
	long GetTaxRate(long lTax);
	void CalculateTaxLink(double dValue ,long *lTaxMap , long lTaxLink, SALE_EXTRA_DETAILS  & cSaleExtraDetails, long lItemIndex); //4.0.25.90 - TD 142798
	long CalculateTaxValueRate(double dTmp ,long lTaxId, SALE_EXTRA_DETAILS  & cSaleExtraDetails, long lItemIndex); //4.0.25.90 - TD 142798
	void SetTotalTrsTaxes(SALE_EXTRA_DETAILS & cSaleExtraDetails); //4.0.25.90 - TD 142798
	long CalcTotalTaxAmount(SALE_EXTRA_DETAILS  & cSaleExtraDetails); //4.0.26.0 - TD 145615
	void CalculateTax(short nGrade , SALE_EXTRA_DETAILS  & cSaleExtraDetails ,long lFuelValue, long lWashValue , long lWashProgram , PAY_AT_PUMP_INFO & info);
	void UpdateCardSaleData(long lPumpNumber, long lTrsNumber,  short nNozzle,long lVolume,long lValue,short nLevel,long lGradePrice,  short nGrade ,PAY_AT_PUMP_INFO * info, CPumpTransact & trs ,SALE_EXTRA_DETAILS  & cSaleExtraDetails);

	void SetLoyaltyTenderAmount( IN OUT  PAY_AT_PUMP_INFO * info, IN long lTotalTransactionAmount ,  OUT SALE_EXTRA_DETAILS &cSaleExtraDetails );
	
	void UpdatePrePayExtraDetails(long lPumpNumber, long lTrsNumber, CPumpTransact & trs ,SALE_EXTRA_DETAILS  & cSaleExtraDetails, short nGrade, long lVolume, long lValue, PAY_AT_PUMP_INFO * info); //4.0.22.73 - TD 74796 //4.0.25.50 - TD 136915 /4.0.25.80 - TD 140655
	void GetSaleExtraDetails__(long lPumpNumber, long lFlags, BSTR sData,  BSTR *sRetData, long *pVal);
	long GetDebitZeroCompleteDataFlags();//4.0.19.508
	long GetSaveZeroPakTrs();
	
	long GetFleetSecondTransactionTimeOut();
	void CheckVlidityTransactionNumber(long lTrsNumber);
	void MoveCashAcceptorShift();
	long GetCashAcceptorReport__(long lTerminalId, long lFlags, BSTR *sHeader, BSTR *sRecords, long *lRecordCount);
	long GetCashAcceptorReportXml__(long lTerminalId, long lFlags, BSTR *sHeader, BSTR *sXmlData, long *lRecordCount, BOOL bCheckRemovalDrawers); //4.0.16.502 - TD 35765 //4.0.17.501
	void SaveAndParsePromptInfo(CString *pInStr, CPumpTransact *pTrs ,PAY_AT_PUMP_INFO *pInfo);
	
	BOOL IsMemberCardAccepted(long lPumpNumber) { return m_bMemberCardAccepted[lPumpNumber]; }
	void SetMemberCardStatus(long lPumpNumber , BOOL bStatus) {m_bMemberCardAccepted[lPumpNumber] = bStatus;}

	void DoAutomaticCheckPriceChangeAck();
	long dwPosRequestReport;// 4.0.9.503
	void SetInformPriceChangeFlags(DWORD dwVal);
	BOOL GetInformPriceChangeFlags();
	DWORD m_dwInformPriceChangeTotalCounter; //4.0.10.811
	DWORD m_dwInformPriceChange; //4.0.9.503
	CWetStockDetails m_cWetStockDetails;
	CarWashStatus_	m_eCarWashStatus;		//4.0.9.501
	void SetNoParallelPakPrePayFlag(DWORD dwVal);
	BOOL GetNoParallelPakPrePayFlag();
	long PumpNotAllowedMixPAKAndPrePay(long lPumpNumber ,PumpReservation eRes);
	void SetManualServiceFee(long lServiceFee);
	long GetManualServiceFee();
	CPriceChangeFlags m_cPriceChangeFlags;
	CServerInfo	m_cServerInfo;
	CShiftProcess m_cShiftProcess;	
	BOOL CheckOLAReportRequird(CPumpTransact &trs, PAY_AT_PUMP_INFO *pInfo );
	void SendWetStockEvents(BYTE byMemeo, long lWetStockFlags); //4.0.9.503
	long GetWetStockMemoFlags();
	long GetRunningValueThreshold();
	long GetEnhancedRollingUpdate();	//TD 37244
	long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData);
	void OverwriteModeLimit(long & lPumpNumber , short & nPumpLevel , long & lValueLimit);
	void ConnectToDataExport();
	void KillDataExportSrvAndAgent(); //4.0.21.0
	void ConnectToIFSFTerminalSrv(); //4.0.12.503
	void ConnectToFuelMobileSrv();   // CR 474446

	// removed qdx-maint
	void CreateQDXMaintenanceThread(/*m_cQDXMaintThread.Init();*/);	//4.0.18.0
	void QDXMaintenanceThreadCleanUp() {};	//4.0.18.0
	//BOOL StartQdxMaintenance(long lFileNum, BOOL bUploadOnly = FALSE) {return m_cQDXMaintThread.StartQDXMaintThread(lFileNum, bUploadOnly);};
	void DoAutomaticQdxMaintenance();
	void DoAutomaticQdxMaintenance__();
	BOOL IsAutomaticQdxMaintenanceActive()const; //4.0.26.501 TD 144220
	BOOL IsAutomaticQdxMaintenanceTimeOut()const; //4.0.26.501 TD 144220

	CString sSysVersion;
	CComBSTR m_sDevicesVersions; //4.0.14.507
	void DataExportCleanUp(BOOL bDisconnFromPS);
	void LoadScreenLayoutsTable(BOOL bCreate, BOOL bRun);
	CTankGaugeFactor m_cTankGaugeFactor;
	
	BOOL IsQdexFilesAlarmOn(short nFileNum);
	CCommonFunction m_cCommonFunction;
	CInitDebitHandler m_cInitDebitHandler;
	void LoadStatePumpLockByMode();
	DWORD IsLockPumpByModeActive();
	BOOL IsLockedByMode(long lPumpNumber);
	long DoAutomaticLockPumpByMode();
	void SetAutoTimeChecker(long lVal);
	BOOL IsHotPumpingActive();
	BOOL LimitedPreSet();
	long DeleteLink2Ola(long lSeqNumber);
	long DeleteFromAsh2000Qdx(long lTrsNumber); //4.0.24.100 118124
	void SetLoyaltyActivity(long lFlags);
	BOOL IsUnbalancedPrepay(CPumpTransact * pPumpTrs,PRE_PAY_INFO*  pPrepayInfo);
	void SetPrePayLimit(long lPrePayLimit);
	long GetPrePayLimit();
	void ServerCleanUp(BOOL bRun);  //4.0.5.30
	long GetOLAFlags(BYTE *sFlags , long lCondition, long lLen=2);
	BOOL IsNeedSaveZeroCompleteData(short bySessionType ,long lValue, long lVolume,BYTE byTransType); //4.0.9.507
	void SetMaintenance(long lLock, long lParam, long *pVal);
	long InsertXMLReceiptIntoReceiptQDX(long lFlags,BSTR & bstrInXML,CPumpTransact & trs, long lPumpNumber); //4.0.22.500 - TD 70353
	long InsertItemsToItemQdx(PAY_AT_PUMP_INFO *pcPAPInfo, TAG_OLA_REC13 *pcTagOlaRec, long lPumpNumber); //4.0.22.501 - TD 70353
	long InsertZonesToReceiptEQdx(PAY_AT_PUMP_INFO *pcPAPInfo, TAG_OLA_REC13 *pcTagOlaRec, long lPumpNumber,long lTrsNumber); //4.0.23.501 - TD 75947
	void ConvertXmlReceiptAndLoyaltyDiscountToDBData( long lPumpNumber, const char* szXmlBuffIn, CPumpTransact* pPumpTransactInOut, PRE_PAY_INFO* pPrePayInfoInOut, TAG_DISCOUNT_LOYALTY_INFO4* pDiscountLoyaltyInfoInOut );
	void UpdatePrePayWithXmlReceiptAndDiscountData( long lPumpNumber, const char* szXmlBuffIn, CPumpTransact* pPumpTransactInOut, PRE_PAY_INFO* pPrePayInfoInOut );
	DWORD GetIndicateAuthorizePumpSentFlag();	//4.0.30.500 - TD 320068
	TAX_PROPERTIES		m_cTaxMap [MAX_TAXES];
	CCompConnectivity	m_cCompConnectivity;//4.0.5.26				
	CHydraPS			m_cHydra;
	DWORD				m_dwHydraActive;
	BOOL				m_bRedundancyByCL;					//4.0.20.913

	CTerminalMessages	m_cTerminalMessages;
	CDeviceRecord		m_cDevicesPerGrade[MAX_GRADES_99];//amram 4.0.10.502
	CDeviceRecord		m_cTerminalDevices[MAX_TERMINALS];
	DWORD				m_dwWatchdogInterval;	//4.0.0.83

	DWORD				m_languagesId[MAX_LANGUAGES];

	SYSTEMTIME  m_tConnectionTimeOut;
	long GetInterval(SYSTEMTIME stStartTime);

	long GetPAPDeclineTimeValue() {return (long)m_dwPAPDeclineTimeValue;} //4.0.12.501
	long GetUnbalancedPrPTimer()  {return (long)m_dwUnbalancedPrPTimer;} //4.0.13.500


	long GetInvoiceRefNumber() {return m_lInvoiceRefNumber;} //4.0.13.505
	void SetInvoiceRefNumber(long lInvoiceRefNumber);		 //4.0.13.505
	DWORD GetVolumeMultiply() const { return m_dwVolumeMultiply; }

	CServerMain();
	virtual ~CServerMain();

	//P@K finalization
	BOOL SetPAKFinalizationAllowedWhenSystemDown(BOOL bAllowed);
	BOOL ISPAKFinalizationAllowedWhenSystemDown(long lPumpNumber = 0);
	BOOL ISPAKFinalizationFlagSet() { return m_bAllowPAKFinalizationWhenSytemDown; }

	//Returns true if one printer or more is connected, else returns false
	BOOL IsPrinterConnected(); //4.0.5.44  

	//Transaction handling
	void UpdateUnbalancedPrePay(long lPumpNumber);
	BOOL UpdateDeclinePAP(long lPumpNumber,long lTrsNumber);//4.0.12.502
	BOOL IsNeedToClearTrsOnIdlePump();

	//Service fees
	long GetMinFullingToChargeServiceFee();

	//Shifts
	void SetShift(short nDayOfWeek, short nShiftNumber, short nShiftFlags, BSTR &sStartTime, long *pVal , long & lQdx);
	long IsActiveShiftWithLock();

	//Debit
	BOOL UseSingleSession();
	void UpdateDebitTimeOutParmeters(long lPumpNumber ,OLA_STAT *pOla, CARD_SALE_ALL3 *pCardSaleAll2);
	void UpdateDebitTimeOutParmeters(long lPumpNumber ,OLA_STAT *pOla, CXMLInterface *pXMLInterface);

	long ClearPAPInfo(long lPumpNumber);
	long ClearLoyaltyInfo(long lPumpNumber); //4.0.20.180 67388 //4.0.21.0

	//Pump mode scheme
	long GetPumpModeScheme3(long lPumpNumber, long lScheme, long *plMode, BSTR *psPeriodDateTimeStart, BSTR *psPeriodDateTimeEnd, BSTR *psDateTimeStart, BSTR *psDateTimeEnd, long *plServiceFeeNo, long *plFlags, long *pVal);
	long SetPumpModeScheme3(long lPumpNumber, long lScheme, long lMode, BSTR sPeriodDateTimeStart, BSTR sPeriodDateTimeEnd, BSTR sDateTimeStart, BSTR sDateTimeEnd, long lServiceFeeNo, long lFlags, long *pVal);

	//AlarmSrv
	void StopAlarmSrvThread();
	void StartAlrmSrvThread();	
	static unsigned int CALLBACK AlarmSrvThread(void *pv);
	long GetAlarmSrvMode();
	long WriteToAlarmSrv(long lPumpNumber, BOOL bErrorRep, long lError, BOOL bWarningRep, long lWarning, BOOL bDeviceRep, long lDevice, long lDeviceCode, BOOL bMiscRep, long lMisc, long *pVal);

	//Timers
	BOOL CheckNozzleLeftOutTimeout();  //4.0.2.161 changed retcode to BOOL

	//VP
	void DisconnectFromVp();
	VpStatus ConnectToVp(); //4.0.24.60
	void	SetVpIsGone(BOOL bVpIsGone);			// 4.0.29.500 TD 264908				
    BOOL	GetVpIsGone()const;	                    // 4.0.29.500 TD 264908
	void    ResetVpRecoveryCounter();               // 4.0.29.500 TD 264908
	DWORD	 GetVpRecoveryMaxRetries(){return m_lVpRecoveryMaxRetries;}; // 4.0.29.500 TD 264908
    BOOL     IsDependByVpRecovery(); // 4.0.29.500 TD 264908

	BOOL    GetQdxIsFail();                     // 4.0.29.501 TD 288369
	void    SetQdxIsGone(BOOL bQdxIsGone);		// 4.0.29.501 TD 288369

	eFuelMobileSrvStatus	CheckFuelMobileSrv();

	//Pump management
	DWORD GetLockOnStopAll();

	//4.0.1.38 start
	struct CVPThreadInfo 
	{
		public:
		CServerMain *m_pServer;
		HANDLE m_hVPStartThread;
		HANDLE m_hVPStopThread;
		HANDLE m_hVPKillThread;
		HANDLE m_hVPThreadKilled;
	}m_cVPThreadInfo;

	HANDLE	m_hVPThread;
	DWORD	m_dwVPThreadId;
	void InitVPThreadInfo();
	void CreateVPThread();
	void DeleteVPThreadInfo();
	static unsigned int CALLBACK VPThread(void *pv);

	//4.0.1.38 end

	GCIRetCode UpdateCashAcceptorData(long lPumpNumber, long lDevice, long lSeqNum, long lFlags, BSTR CashSaleData);

	//Wetstock
	DWORD GetWetstockMaintFlag();

	//Conduction the Delivery Emulation State Machine

	BOOL SetDeliveryFlags(BYTE byDeliveryEmulationStatusFlag, BYTE byDeliveryEmulationProcessFlag, int & nErrorRetrieved, BOOL bDeliveryEndedSuccessfully = TRUE );

	BOOL SetDeliverySingleTankFlags(const long lTankNumber, const long lTankReference, const BOOL bLockConnectedPumps, const BYTE byDeliveryEmulationStatusFlag, const BYTE byDeliveryEmulationProcessFlag, int & nErrorRetrieved, BOOL bReadingEndedSuccessfully = TRUE);//4.0.21.500 TD 66935

	BOOL HandleDeliverySingleTankProcess(const long lTankNumber, const long lTankReference, const BOOL bLockConnectedPumps, const BYTE byDeliveryEmulationStatusFlag, const BYTE byDeliveryEmulationProcessFlag, int & nErrorRetrieved, BOOL bReadingEndedSuccessfully);//4.0.21.500 TD 66935

	BOOL AbortDeliverySingleTankProcess(const long lTankNumber, int & nErrorRetrieved); //4.0.21.500 TD 66935

	void PrepareDeliverySingleTankProcessFailure(BOOL & bRes, int & nErrorRetrieved, const int iErrorCode);//4.0.21.500 TD 66935

	BOOL ResetDeliverySingleTankProcess(const long lTankNumber, int & nErrorRetrieved);//4.0.21.500 TD 66935 

    BOOL StartDeliverySingleTankProcess(const long lTankNumber, const long lTankReference, const BOOL bLockConnectedPumps, const BYTE byDeliveryEmulationProcessFlag, int & nErrorRetrieved);//4.0.21.500 TD 66935
    
	BOOL EndDeliverySingleTankProcess(const long lTankNumber, int & nErrorRetrieved, BOOL bReadingEndedSuccessfully);//4.0.21.500 TD 66935

	void LoadTaxesTable();//4.0.1.32

	long CheckExistFile(char *sFile);
	long CheckForIncomingNewPrice();
	void XMLPriceChange(char *cFileName);

	BOOL IsNotAllowedWaitingPrePay();
	
	DWORD IsPrePayEnabled() { return m_dwPrePayEnabled; }//4.0.0.82
	DWORD IsCashAcceptorreportEnable() { return m_dwCashAcceptorReportActive; }
	
	long GetMinAlertSeverity_SoundAlert() { return (long)m_dwMinAlertSeverity_SoundAlert; } //4.0.13.500 

	//4.0.0.79 Start
	long GetMaxPumps() { return m_lMaxPumps; }
	long GetLastValidPump() { return m_lLastValidPump; }
	void SetLastValidPump(long lLastValidPump) { m_lLastValidPump = lLastValidPump; }
	//4.0.0.79 End
	
	long GetLastValidTank() { return m_lLastValidTank; } //Hermelin
	
	//Attendant transactions
	GCIRetCode UpdateAttendantSale(long lPumpNumber, long lTrsNumber, long lState, long lFlags, BSTR sData, long *plRetVal);
	GCIRetCode PaidAttendantTrs(long lPumpNumber, long lFlags, CARD_SALE_ALL3 *pInfo, CPumpTransact *pcTrs);
	GCIRetCode SetAttendantTrs(long lPumpNumber, long lTrsNumber, long lState, long lFlags, BSTR sData, long *plRetVal);

	long NewItemsIndex();
	long CreateRegEntries(char * sEntry , BYTE * sVal);
	void SendDebitResult(long lPumpNumber, long lSessionId);
	BOOL IsNeedCreditLimitCheck();
	long LoadPumpFileInfo(long lLoadFlag);
	void InitDebitShiftParameter();
	void InitTerminalIPT();

	//Eyal 4.0.0.25 Start
	long GetNewOLAServer();
	void SetNewOLAServer(long lNewSrv);
	void SwitchServer__(long lNewServerType);
	//Eyal 4.0.0.25 End

	long ResetPumpStatQdxData(long lPumpNum, long lFlags);
	void RefreshModeDefinitions(long lMode);//4.0.1.38
	void RefreshModeSchemeDefinitions(long lIndex);//4.0.1.38
	void RefreshPumpsKeys(char *szTableName,char *szFiledName,long lPumpNum);//4.0.1.38
	void RefreshRegistryTable(char * szTableName,char *szFiledName,long lRefreshType,long lClientEventData);
	void LoadPumpStat(long lPumpNum,BOOL bSaveInfo);
	BYTE GetPumpFileIndexNum();
	void SetTankReadingIndex(long lIndex);
	void SetDeliveryReporIndex(long Index);
	BOOL IsMeggitSite();
	void SetPumpTotalIndex(long lIndex);
	BOOL IsAutoTimeChecker();
	void SetPrevSysTime(SYSTEMTIME cCurrentTime);
	void DoAutomaticRealTime();
	void DeleteOldPumpInfo();
	long ConvertPumpInfo2TerminalInfo();
	long InitTerminalsInfo();
	
	long BuildRequestReceiptInfo(CMD_STRUCT_FORMAT_RECEIPT &pcmdInfo, CStringA cTrack2, long lIndex, long lFlags, long lPosNumber, TAG_OLA_REC13 *tagOlaRec);

	void SetApplicationToHighPriority();
	long CheckForLoyaltyDiscount(long lPumpNumber , CMD_STRUCT_AUTHORIZE_LIMIT & cmdAuthorize);
	BOOL IsActiveLoyaltyDiscount();
	BOOL ConvertWideCharLoyaltyInfo2RegularInfo(long lPumpNumber , BSTR *sLoyaltyInfo, TAG_DISCOUNT_LOYALTY_INFO4 *pRec); //4.0.20.450 66977 //4.0.21.501 //4.0.23.270 111746 //4.0.24.60
	BOOL MovePrepayGradesDiscounts(long lFromPumpNumber, long lToPumpNumber, EXTRA_INFO_REC *pOldExtraInfo, EXTRA_INFO_REC *pNewExtraInfo); //50269
	void FillInPromotionsArrayByPumpNum( long lPumpNumber );  // TD 378454
	void CancelLoyaltyTrs(long lPumpNumber, OLA_STAT * pOla, PAY_AT_PUMP_INFO * pTmpInfo, BOOL bSendSwipeResult = TRUE);
	long GetItemPrice(long lPumpNumber , PumpItems * pInfo , int iItem);
	long GetGradeCodeByNacsCode(PumpItems & pPumpItem,long lNacsCode);
	BOOL PakLinkNumber(long lLinkNumber);
	BOOL PrePayLinkNumber(long lLinkNumber);
	void GetServiceFeeInfo(long lPump, long lTrsType, long lTrsNumber ,BSTR stTime, long lFlags, long *pValue, long *pRetVal);
	long IsValidPAPAmount(long lPumpNumber,long lPreAuthorizedAmount, char sTranType , long lPreAuthorizedVolume);  //3.2.0.69
	BOOL IsRollbackOnPriceChangeFailure(); //4.0.12.515
	BOOL IsMasterComputer() {return m_bIsMasterComputer;} //4.0.14.502

	// RFUEL-2817 - start
	long GetPaymentDiscount(const PAY_AT_PUMP_INFO& Info) const;	
	long GetTenderDiscount(const PAY_AT_PUMP_INFO& Info) const;

	long GetLoyaltyDiscount(const PAY_AT_PUMP_INFO& Info, long PumpNumber, long lGrade) const;
	long FinalLoyaltyDiscount(PAY_AT_PUMP_INFO& Info, long lPumpNumber, long lGrade, long lAdditionalDiscount);
	bool ApplyPaymentCardDiscount(long PumpNumber, PAY_AT_PUMP_INFO& Info);	
	void CopyPaymentDiscountToTenderDiscount(PAY_AT_PUMP_INFO& Info);
	void CopyTenderDiscountToPaymentDiscount(PAY_AT_PUMP_INFO& Info);

	bool DumpPaymentCardDiscount(long PumpNumber, const PAY_AT_PUMP_INFO& Info);

	bool IsLoyaltyDiscountApplied(const long lPumpNumber) {
		CPumpTransact trs;
		auto& Pump = GetPump(lPumpNumber);
		BOOL bFound = Pump.GetTrsByStatus(WAIT_PROGRESS, TRS_LINK_OLA_NUMBER, &trs);
		bool bDiscountApplied = (trs.m_sLoyaltyDiscountApplied == LOYALTY_DISCOUNT_APPLIED);
		return bDiscountApplied;
	}

	void SetLoyaltyDiscountApplied(const long lPumpNumber, BYTE Flag = LOYALTY_DISCOUNT_APPLIED) {		
		auto& Pump = GetPump(lPumpNumber);

		CPumpTransact trs;
		BOOL bFound = Pump.GetTrsByStatus(WAIT_PROGRESS, TRS_LINK_OLA_NUMBER, &trs);
		trs.m_sLoyaltyDiscountApplied = Flag;
		CentralUpdateTrs(lPumpNumber, trs.m_lNumber, &trs, FALSE);

		// test code
		//bool testval = IsLoyaltyDiscountApplied(lPumpNumber);
	}

	bool IsLoyaltycardAuthorized(const PAY_AT_PUMP_INFO& Info) const {
		const unsigned char* pLoyaltyID = Info.CardSaleInfo.extraData.sLoyalId;
		constexpr size_t nFieldSize = sizeof(Info.CardSaleInfo.extraData.sLoyalId);
		//  All blank field if nothing entered
		const bool bLoyaltyNotEntered = std::all_of(pLoyaltyID, pLoyaltyID + nFieldSize, [](unsigned char C) {return C == ' '; });
		return (!bLoyaltyNotEntered);
	}

	void SetPaymentDiscount(PAY_AT_PUMP_INFO& Info, int lDiscount) const;	
	// RFUEL-2817 - end


	//RFUEL-3028
	bool CheckRealtimeDB(long lPumpNumber) const;

	void RegisterEOSReconfigureEventForAllTables ();
	BOOL ReconfigureEOSEnabledInRegistry ();
	void SetReconfigureInEOSRegistry (IN BOOL bEnable = FALSE);

	//Add by Yaron for Ctrl Events
	//Ctrl events
	void StopEventThread();
	void StartEventThread();
	static unsigned int CALLBACK EventsThread(void *pv);
	CEventManager m_cEventManager;

	long AddCtrlEventClient(long lClientId, ICtrlEvents* pCtrlObj);
	long RemoveCtrlEventPtr(long lClientId);
	void ResetCtrlEventNumerator();
	long GetCtrlEventClientId();
	long AddGpiEventClient(long lClientId, IGPIEvents* pGpiObj);//4.0.12.501
	long RemoveGpiEventPtr(long lClientId);//4.0.12.501
	long GetGpiEventClientId();//4.0.12.501
	//End add by Yaron for Ctrl Events

	//OLA
	void InitOLAThreadInfo();
	long CheckExistUnBalancePrePay();
	DWORD dwOlaWaitingMessTimeOut;
	int FindTrsByTrsNumber(long lTrsNumber);
	BOOL GetPosVoidFlags();
	BOOL GetCashAcceptorActive();//4.0.12.502
	void SetPosVoidFlags(BOOL bFlags); //4.0.0.81
	void SetCashAcceptorValid(BOOL bFlags);//4.0.12.502
	OlaSrvStatus GetOLAThreadState();
	void CloseOLAThread(BOOL bShutDown=FALSE);
	void WriteToLogMsg( long lPumpNumber,  BOOL bErrorRep,  long lError,  BOOL bWarningRep , long lWarning,  BOOL bDeviceRep,  long lDevice,  long lDeviceCode,  BOOL bMiscRep,  long lMisc, long *pVal);
	void ConstructOLAThreadInfo();
	void DeleteOLAThreadInfo();
	OlaSrvStatus m_enmOlaThreadStatus;
	OlaSrvStatus CheckOLA();
	void StartOLAThread();
	void CallOLA(long lCmdType, long lPump);
	static unsigned int CALLBACK OLAThread(void * pv);
	void DeleteMiscInfo(long lPump);
	BOOL AllowParallelAVIUsage();
	long GetAviTrsTimeOut();
	BOOL ServiceModeNotValid(long lServiceMode);
	void PrintReport(long lTerminalType, long lTerminalNumber, char *pData, long lFlags, long lLen);
	long FindServiceFee(long lPump , SYSTEMTIME & sysTime , long lTrsType);
	BOOL UseServiceFee();
	BOOL ServiceFeeNotValid(LONG lServiceNumber);
	long GenerateModeSchemeIndex(long lPump, long lScheme);
	void LoadTableModeSchemToArray__(BOOL bRun=TRUE);
	void GetReceiptCounter(long *plCounter, long lType, long lFlags);
	long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData ,long lFlags =0,long *lSesionType = 0, long lPumpNumber=0, BOOL bDoNotUseXmlBuffer = FALSE);  //4.0.5.24 4.0.25.0
	long GetCardInfoEx(long lClientId, BSTR sInBuffer, BSTR sInfoType, long lFlags, BSTR *sRetData, long *pVal);//4.0.11.500
	long GetCardInfoEx(long lPumpNumber, long lClientId, BYTE sTrack1[], BYTE sTrack2[],PAY_AT_PUMP_INFO * pInfo, long lFlags, BSTR *sRetData, long *pVal);
	void MatchSentLoyaltyCardInfoToOLA(PAY_AT_PUMP_INFO &pInOutPAPBuffer);  //4.0.23.507 89548
	void MatchReceivedLoyaltyCardInfoToOLA(CARD_SALE_ALL3 &pInOutPAPBuffer);  //4.0.23.507 89548
	long GetMaxVolumeForVITConflict(); 
	BOOL CheckVITConflict();
	BOOL ReleasePumpMessages(long lPump, long lVal);
	long GetTypeLimitByUser(short nTypeLimit);
	long ConvertReceiptToOla( RECEIPT_INFO *pReceipt  , CPumpTransact *pTrs, void *pInfo);
	long ConvertReceiptToOla(CReceiptInfo & ReceiptInfo, CPumpTransact *pTrs, PAY_AT_PUMP_INFO *pPAPInfo);
	BOOL IsInteracCard(long lPumpNumber);
	BOOL IsContactlessCard(long lPumpNumber);
	BOOL IsCardSwiped(BYTE	&sEntryMethod);

	long ConvertOlaToReceipt(CPumpTransact * pTrs , PAY_AT_PUMP_INFO * pInfo , RECEIPT_INFO *pReceipt, BYTE lOlaState=0);
	long ConvertOlaToCReceiptInfo(CPumpTransact *pTrs, PAY_AT_PUMP_INFO *pPAPInfo, CReceiptInfo & ReceiptInfo, BYTE lOlaState=0);
	void ClearPapToPakFlag(long lPumpNumber) /* 4.0.23.2060 TD 381449 */;
    long ProcessTerminalCmd(TerminalCmdCode eTerminalCmdCode, long lTerminalId);  //4.0.28.502  - TD 256429
	void DoAutomaticReceiptCheck();
	CString GetSiteID();
	void UpdateEncryptionKeys(long lPumpNumber,_bstr_t sReturnBuffer);
	long GetStationLimitVolume();
	void CheckPrPTimerForTrs(CPumpTransact trs, long lPumpNum); //4.0.13.500

	void UpdatePAPDeclineTimers(long lThrdSleepTime); //4.0.12.501

	BOOL					IsCurrentRegion(DWORD dwRegion) const;						//4.0.23.508

	__int64					CheckRequiredParam(CARD_SALE_ALL3 *pCardData);

	long					LogDeclinedCard(long lPumpNumber,CARD_SALE_ALL3 *pData);

	long					SaveCardSaleData(long lPumpNumber, CARD_SALE_ALL3 *pData);

	long					AllowAuthOnNonRequest();
	void					GetVersion();
	BOOL					IsNozzleRestrict(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);

	long					GetOLAServiceStatus();
	long					StopOLAService();
	BOOL					IsAutoForce();
	BOOL					IsAutoReceiptCheck();
	BOOL					IsControllerUpdateTrs();

	void					WatchdogRcved();
	void					DoAutomaticWatchdog();
	void					DoAutomaticCarWashGeneralReq(); //4.0.23.504 76432
	void					DoAutomaticCheckCarWash();

	void					DoAutomaticCheckCarWash__();
	void					DoAutomaticCarWashGeneralReq__();	//4.0.23.504 76432
	void					DoAutomaticlCarWashClear();
	void					DoAutomaticlCarWashClear__();
	void					SetReqiredModeProcess();
	void					SetReqiredModeProcess__();
	void					DoAutomaticPriceProtectionCheck__();
	long					GetRegion();
	long					GetOLAInUse();
	long					OLALogDeclineReq();
	long					OLAByController();
	long					GetLimit(short nLevel,long lPumpNumber = 0);
	BOOL					IsShiftChangeInProgress();
	BOOL					DBConnectionAvailable() const;
	void					CheckAuthorizedTimeOut();
	void					CheckAuthorizedTimeOut__();
	void					CheckDispensingTimeOut();
	void					CheckDispensingTimeOut__();
	void					CheckUnpaidTimeOut();
	void					CheckUnpaidTimeOut__();
	void					CheckAndReserveTimeOut();	//4.0.17.20 - TD 26384
	void					CheckAndReserveTimeOut__(); //4.0.17.20 - TD 26384

	void					CheckDeliveryAndTank();	
	long					GetShiftMode();
	BOOL					PumpNotInWorkingState(long lPumpNumber);
	BOOL					PumpInCommsTerminatedState(long lPumpNumber); //4.0.19.820 - TD 78775
	BOOL					PumpInUnknownState(long lPumpNumber); //4.0.24.70 - TD 115975
	long					GetTotalIndex();
	long					GetDeliveryIndex();
	long					NewAlarmIndex();
	long					NewCarWashIndex();
	long					GetTankReadIndex();
	long					GetAlarmIndex();
	long					GetWetStockMode();
	long					GetWetStockInterval();
	long					GetCarWashIndex();
	long					GetSeverityAlarmsCount() {return (long)m_dwSeverityAlarmsCount;} //4.0.13.500
	void					IncreaseSeverityAlarmsCount();	//4.0.13.500
	void					DecreaseSeverityAlarmsCount();	//4.0.13.500
	void					ResetSeverityAlarmsCount();		//4.0.13.500
	int						FindCurrentShift(SHIFT_REC *pShiftRec);
	int						FindNextShiftTime(SHIFT_REC * pShiftRec);
	void					CheckPriceChange();
	void					CheckPriceChange__();
	void					CheckShiftChange(BOOL bForceNonLock = FALSE);
	void					CheckShiftChange__(BOOL bForceNonLock = FALSE);
	void					GetWetStockData(long lMode,long lWetStockOnDemandFlags=0);
	void					GetWetStockData__(long lMode ,long lWetStockOnDemandFlags=0);

	void					ClearWetStockProcess(long lWetStockFlags); //4.0.9.503
	void					DoHotPumping(long lPumpNumber, _bstr_t sReturnBuffer);
	void					ZeroPAPTotals(CPumpTransact *pTrs,PAY_AT_PUMP_INFO *pInfo);

	BOOL					IsPumpHotAuthorized(long lPumpNumber,short byOlaStat); //4.0.9.507
	BOOL 					StopHotAuthorizedPump(long lPumpNumber);

	long					SavePumpInfo(long lPumpNumber);
	LockStatus				GetLockStatus();
	long					NewTransactNumber();
	long					NewActiveReadNumber();
	long					NewActiveShiftNumber();

	long					NewPumpTotalIndex();
	long					NewTankReadIndex();
	long					NewFuelDeliveryIndex();
	long					NewOLAIndex();
	long					GetActiveReadNumber();
	long					GetActiveShiftNumber();
	long					GetFormalShiftNumber();
	

	void					SetCurrentShift(SHIFT_REC *pInfo);
	void					GetCurrentShift(SHIFT_REC *pInfo);
	BOOL					ShiftChangeProcess();
	BOOL					ShiftChangeProcess__();

	BOOL					DoAutomaticReconfigure( BOOL bAllPumpsUnLocked );
	char *					ReconfigureTableIdToString (IN ULONG ulTableId);

	BOOL					PriceChangeProcess();
	BOOL					PriceChangeProcess__();

	BOOL					ModeSchemeProcess();
	BOOL					ModeSchemeProcess__();
	int						DoAutomaticReadTotal(long lPumpNumber, BOOL bLock = TRUE);	
	BOOL					DoAutomaticPumpLock(long lPumpNumber, long lServerLockReason);
	BOOL					DoAutomaticPumpUnLock(long lPumpNumber);
	void					UnlockPump(long p);

	
	void					SendSwipeResult(long lPumpNumber, long lAnswer,unsigned char	sTranStatus, CARD_SALE_ALL3* pData);	//4.0.18.501
	void					SendSwipeResult(short byOlaStat, long lPumpNumber, long lAnswer,unsigned char	sTranStatus, CARD_SALE_ALL3* pData);	//4.0.18.501
	void					SendSwipeResult(long lPumpNumber,long lAnswer = CD_OK);
	void					SendReceipt(short byOlaStat,long lPumpNumber, long lAnswer,unsigned char sTranStatus, CARD_SALE_ALL3 *pData, CPumpTransact *pTrs);
	void					SendCardInfoResult(short byOlaStat,long lPumpNumber, long lAnswer,unsigned char sTranStatus, CARD_SALE_ALL3 *pData, CPumpTransact *pTrs);
	void					SetPriceCommand(long lPumpNumber, short nNozzle,short nGrade,CMD_STRUCT_GRADE_PRICE *pCmdInfo,BOOL bNew,BOOL bLast);

	void					SendGradePrice(short nGrade);
	void					SetServerLockStatus(LockStatus lStat);
	void					SetShiftChangeState(ChangeState lStat);
	void					SetPriceChangeState(ChangeState lStat);
	long					GetLockResExt();
	void					SetLockResExt(long lStat);
	long					ConstructAndSaveCardData(long lFlags, long lPumpNumber, long lSeqNumber, CARD_SALE_ALL3 * pCardData,long lVarsion=2, BOOL bPAPInfoUpdate = TRUE);	//4.0.19.502

	void					DoAutomaticOLA();
	void					DoAutomaticOLA__();
	int						WideCancelTrs(long lSrc, long lPumpNumber, long lTrsNumber, long *pRtc,BOOL bSkipDelPrePayRecord=FALSE);
	long					WideRemoveTrs(CPumpTransact *pTrs, long *pRtc);
	long					ReleasePrePayReservation(long lPumpNumber, CPumpTransact *pTrs);
	int						FindTrsByLinkNumber(CPumpTransact *pTrs);
	CCmdInfo				GetNextCmdInQ();
	void					DoAutomaticPrePay();
	void					DoAutomaticPrePay__();
	void					DoAutomaticMaintenanceLock();
	void					DoAutomaticMaintenanceLock__();
	void					DoAutomaticMaintenanceUnlock();
	void					DoAutomaticMaintenanceUnlock__();
	void					DoAutomaticEmulationDelivery__(); //4.0.5.24
	void                    DoAutomaticEmulationDeliverySingleTank__();//4.0.21.500 TD 66935

	void					DoAutomaticRequestReport__();	//4.0.22.501 - TD 69905
	void					DoAutomaticRequestReport();		//4.0.22.501 - TD 69905

	void					DoAutomaticChangeProductInTank__();		//4.0.22.503 - TD 72189
	void					DoAutomaticChangeProductInTank();		//4.0.22.503 - TD 72189

	void					DoAutomaticTankLock();		//TD 66935
	void					DoAutomaticTankLock__();	//TD 66935
	void					DoAutomaticTankUnLock();	//TD 66935
	void					DoAutomaticTankUnLock__();	//TD 66935
	BOOL                    DoAutomaticLockPumpByTank(const long lPumpNumber); //TD 66935

	void					DoAutomaticTrsAutoReconcile__(); // 4.0.23.1050 TD 233242
	void					DoAutomaticTrsAutoReconcile();	  // 4.0.23.1050 TD 233242
	void					DoAutomaticLoyaltyTimeout__(long lPumpNumber, OLA_STAT *pOla);	
	void					SetAutoReconcileTimeout(long lPumpNumber , const TAG_DISCOUNT_LOYALTY_INFO4 *  pLoyaltyInfo , CPumpTransact *pTrs); // 4.0.23.1050 TD 233242
	void					SetAutoReconcileTimeout(long lPumpNumber , SHORT nTimeOut , PumpReservation res); // 4.0.23.1050 TD 233242
	void					LoyaltyFlowAfterTimeout(CString sTimerCode, long lPumpNumber, long lTimeOutCode, OLA_STAT * pOla );
	TendersCards  			m_cTendersCard[MAX_PUMPS];			//CR 404913 

	long					m_lNumberOfTendersAuthorized[MAX_PUMPS];
	
private:
	void					DoAutomaticExportSiteConfiguration();		//4.0.26.508 TD 146575
	
public:
	void					CalculateTaxes(PAY_AT_PUMP_INFO * pInfo,long lPumpNumber , CPumpTransact*pTrs);
	void					AddMemberInfo(long lPumpNumber, OLA_STAT * pOla, long p);
	void					GetMemberInfo(long lPumpNumber, OLA_STAT * pOla, long p); // CR 405435
				
	void					DoAutomaticExportSiteConfiguration__();		//4.0.26.508 TD 146575

	long					GetCreateFleetRecord() const { return m_lCreateFleetRecord; }

	void					SetReloadQdxLastTime(const SZ_TAG_SYSTEMTIME* pTagTime);		//4.0.26.501 TD 144220
	void					UpdateReloadQdxLastTime();										//4.0.26.501 TD 144220
	void					SaveReloadQdxLastTime();										//4.0.26.501 TD 144220

	long					TrsLinkOperation(eFileOperation OpCode, CPumpTransact *pTrs, void *pData, void *pExtraData=NULL, BOOL bSkipLoyaltyOperation= FALSE);
	long					TrsLinkOperationEx(eFileOperation OpCode, CPumpTransact *pTrs, void *pData, void *pExtraData=NULL,BOOL bSkipLoyaltyOperation=FALSE); 
	long					IndexOperation(short nFileNumber, eFileOperation OpCode, void *pData , void *pExtraData = NULL,long lFlags=0);//4.0.12.501
	
	long					CentralUpdateTrsByIndex(long lPumpNumber, long lTrsNumber, short nIndex, CPumpTransact * pTrs,BOOL bRolling);
	long					CentralUpdateTrs(long lPumpNumber, long lTrsNumber,CPumpTransact *pTrs,BOOL bRolling);
	long					SpecialUpdateTrs(long lPumpNumber, long lTrsNumber, CPumpTransact * pTrs,long lFlags,TrsStatus lConditionStatus );
	long					CentralInsertTrs(long lPumpNumber, CPumpTransact * pTrs, BOOL bPostVoid = FALSE , BOOL bValidateExtraInfo =FALSE); //4.0.14.508
	void					RemoveCmdFromQ();
	void					UpdateLastCmd(long lSrc, long lCommand, long lNumber, PosCmdState lCmdState, long lRetCode);
	void					UpdateParam(char *szTableName, long lKey1, long lKey2, char *szFiledName, short nFiledType, char *szVal, long lVal);	
	DWORD					SubSystemTime(SYSTEMTIME SysTime1, SYSTEMTIME SysTime2);
	void					LoadCarWashDefinition();
	BOOL					IsResForAVI(PumpResExt lResExt);
	
	BOOL					CmdInQ();
	long					SetCommand(Command lCommand, LONG lSrc, LONG lParam, LPVOID pData, SHORT nDataLen);	
	long					SetCommand(Command lCommand, LONG lSrc, LONG lParam);
	void					SetServerState(ServerState lNewState,BOOL bUpdateCompConn = TRUE);//4.0.5.26
	ServerState				GetServerState();	
	OperatingMode			GetOperatingMode();
	void					SetOperatingMode(OperatingMode lNewMode);
	BOOL					PumpNotValid(LONG lPumpNumber);
	BOOL					GradeNotValid(short nGrade);
	BOOL					LevelNotValid(short nLevel);
	BOOL					NozzleNotValid(short nNozzle);
	BOOL					TankNotValid(LONG lTankNumber);
	BOOL					SchemeNotValid(LONG lScheme);
	BOOL					ModeNotValid(LONG lMode);
	long					NewMessageNumber();
	BYTE *					GetStringTillDelimiter(BYTE *pszInString, BYTE cDelimiter, BYTE *pszOutString);
	BOOL					GetStringTillDelimiterOrEnd(BYTE *pszInString, BYTE *pszOutString, int iOutStringSize, BYTE **pNextString);
	BOOL					NozzleLevelLogic();
	long					GetGradeCode(short nGrade);
	long					GetGradeIndex(short nGradeCode);	//RFUEL-3309 Get the maximal discount allowed for the grade 
	CString					GetGradeAditionalCode(short nGrade);
	long					GetGradeTaxLink(short nGrade);
	CString					GetGradeName(short nGrade);
	long					GetWashProgramIndex(long lProductCode);
	long					GetWashProgramIndex(CString sProductCode);
	DWORD					GetWashProgramTaxLink(int lWashProgramNumber);
	DWORD					GetWashProgramPrice(int lWashProgramNumber);
	CString					GetWashProgramName(int lWashProgramNumber);
	CString					GetWashProgramCode(int lWashProgramNumber);
	long					GetWashIndexByProgramCode(CString sProductCode);
	//add by Yaron
	void					SetMemoInterval(long lMemoInterval);
	long					GetMemoInterval();
	void					SetWetStockClearTime(long lWetStockClearTime);
	long					GetWetStockClearTime();
	DWORD					GetWetStockStateTimeout(); //TD 351225 - Wetstock report does not print after wetstock process had arrived into a bad state
	//4.0.21.500 TD 66935:
	void                    SetPumpLockTimeOut(DWORD dwPumpLockTimeOut);
    DWORD                   GetPumpLockTimeOut();

	void                    SetAddEncodingStringToVPXml(DWORD dwAddEncodingStringToVPXml);			//4.0.24.90 TD 106227
    DWORD                   GetAddEncodingStringToVPXml();											//4.0.24.90 TD 106227

	void                    SetSingleCoreActive(DWORD dwSingleCoreActive);							//4.0.24.90 TD 106227
    DWORD                   GetSingleCoreActive();													//4.0.24.90 TD 106227	

	void                    SetSingleShiftAccumulationsActive(DWORD dwSingleShiftAccumulationsActive);	//4.0.25.50 - TD 108311
    DWORD                   GetSingleShiftAccumulationsActive();										//4.0.25.50 - TD 108311

	//end add by Yaron

	void SetQdxEmptied(DWORD bQdxEmptied) { m_bQdxEmptied = bQdxEmptied; }						 //4.0.27.500  TD 236009  
	BOOL IsQdxEmptied() const	{ return m_bQdxEmptied; }										// 4.0.27.500  TD 236009

	long					GetDataBaseMaintenanceTimeout() {return (long)m_dwDataBaseMaintenanceTimeout;} //4.0.5550.1504
	void					DataBaseMaintenanceTimeout(long lThreadTime); //4.0.5550.1504

	BOOL					InitServer(BOOL bRun);	
	
	void InitMiscCreditCardDecline();
	
	BOOL					IsPumpReadyForPriceChange(long lPumpNumber);//4.0.9.506 //amram 
	BOOL					IsPumpReadyForModeChange(long lPumpNumber);//4.0.9.506 //amram
	void					DoAutomaticInitLoyaltyFuel__(); //4.0.9.508
	void					DoAutomaticInitLoyaltyFuel();//4.0.9.508
	void					DoAutomaticInitFlowControl();//4.0.19.502 55286 Copient

	void					InsertTrsToShortCardArr(CPumpTransact * pTrs,BYTE * sShortCardName);//4.0.10.1000
	BOOL					LoadShortCardNameFromQDX();
	long					DoAutoOlaFuelLoyaltyStartTicket(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);//4.0.9.508
	long					DoAutoOlaFuelLoyaltyContNMPSessionsAfterAuthPayment(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);	//4.0.22.500, PAY_AT_PUMP_INFO *pInfo); //Copient 55286
	long					DoAutoOlaFuelLoyaltyEndTicket(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);//4.0.9.508
	long					DoAutoOlaFuelLoyaltyCancelTicket(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo); //Liat LPE
	long					DoAutoOlaFuelLoyaltyAuthorizeLoyalty(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);//4.0.22.504 69344
	long					DoAutoOlaFuelLoyaltyRunQuery(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);//4.0.22.504 69344
	long					DoAutoOlaFuelLoyaltyStartFueling(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo); //4.0.22.504 69344

	//  RFUEL-2817
	long					DoAutoOlaFuelLoyaltyNMPSessionsAfterPayDiscount(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo); 



	long					CheckActivityPAKAutoReconcile(long lPumpNumber , CPumpTransact & trs);//amram moved from gci
	inline	long			GetPAKAutoReconcileTimer(){return m_dwPAKAutoReconcileTimer;};
	inline	void			SetPAKAutoReconcileTimer(long lReconcile) {m_dwPAKAutoReconcileTimer = lReconcile;};

	inline	long			GetPriceChangeLockEx(){return m_dwPriceChangeLockEx;};//4.0.9.510
	inline	void			SetPriceChangeLockEx(long lPriceChangeEX) {m_dwPriceChangeLockEx = lPriceChangeEX;};//4.0.9.510
	BOOL					UpdatePAKReconcileTrans(long lPumpNumber);//amram 4.0.9.510
	void					SetPumpNumberToQDX(long lPumpNumber,char* sPumpNumber);//4.0.11.510
	
	long					GetGradePositionRestrection(long lGrade,long lPumpNumber);//4.0.10.503
	void					UpdateDiscountPerGrade(long lGrade, long lPumpNumber, long lCentsOff, PAY_AT_PUMP_INFO & cTmpInfo); //Liat LPE

	void					GetDeclineErrorFromOla(long lDeclineError,PAY_AT_PUMP_INFO * pInfo);//4.0.8.240
	long					DeleteFromPaidTrsQDX(CPumpTransact & cTrs);
	long					UpdatePaidTrsQDX(CPumpTransact & cTrs,long lUpdateFlag);
	BOOL					GetTrsFromPaidQdx(long lPumpNumber,long lTrsNumber,CPumpTransact & cTrs);
	BOOL					InsertToPaidQDX(CPumpTransact & trs,long lAddToPumpNumber=0);
	GCIRetCode				ChangeTrsToNotOnOffer(CPumpTransact & cTrs,long lTerminalNumber,long lPumpNumber);
	BOOL					UpdateDataInCarWashTrs(CPumpTransact & cTrs,long lPumpNumber,long lSeqNumber);//4.0.12.501
	void					IncreaseWetstockMemoCounter();//4.0.17.501
	BOOL                    IsTimeToProcessByMemo(const long lIntrervalMin); //4.0.19.500
	BOOL                    IsTimeToProcessByTime(const long lIntrervalMin); //4.0.19.500
	//4.0.18.501 merge from 4.0.18.30
	void                    SetInTheMiddleOfReceiptReOrganize(const BOOL bVal);
	BOOL					GetIsInTheMiddleOfDoAutomaticReceiptCheck();
	void                    SetInTheMiddleOfDoAutomaticReceiptCheck(const BOOL bVal);
	BOOL					IsPAPEnabledDSS();	//4.0.21.13 68756
	BOOL					IsPAPEnabledOLA();	//4.0.21.13 68756
	void					SetPAPEnabledOLA(BOOL bVal){m_bPAPEnabledOLA = bVal;}; //4.0.21.13 68756

	void					GetWetStockReportAsync(); //4.0.22.501 - TD 69905
	void					LoadGradeLimitArrayFromCardData( IN long lPumpNumber , OUT CMD_STRUCT_AUTHORIZE_LIMIT &cmdInfo);
	long					LoadGradeLimitArrayFromExtraInfo( IN CPumpTransact trs, OUT CMD_STRUCT_AUTHORIZE_LIMIT &cmdInfo );
	void					PrintGradeLimitArray(long lPumpNumber, PAY_AT_PUMP_INFO & cPapInfo);
	void					SetGradeLimitsAndDiscounts(IN long lPumpNumber,IN PAY_AT_PUMP_INFO & cPAPInfo, OUT EXTRA_INFO & cExtraInfo);
	BOOL					SaveGradeLimitsAndDiscountsArrayToExtraData(IN long lPumpNumber, IN BSTR & bstrInXML , IN OUT CPumpTransact &trs);
	void					ConvertXmlToCardSale( char * sXmlBuff, TAG_OLA_REC12 &cTagOlaRec, PAY_AT_PUMP_INFO &cPAPInfo );

	DWORD					GetCloseOpenSalesTimeout();	//4.0.22.503 - TD 72189
	DWORD					GetWetStockReportIncludeOldDataFlag();	//4.0.22.503 - TD 69905

	void					DoAutomaticPriceChangeWithAckPerPump__(); //4.0.19.820 - TD 78775
	void					DoAutomaticPriceChangeWithAckPerPump(); //4.0.19.820 - TD 78775
	DWORD					GetPriceChangeWithAckPerPumpResponseTimeout(); //4.0.19.820 - TD 78775

	void					SetCLIsGone(BOOL bCLIsGone);								//4.0.23.507 TD 89047
	BOOL					GetCLIsGone()const;											//4.0.23.507 TD 89047
	DWORD					GetServerStateEx(long lFlag);								//4.0.23.510 TD 89047

	void					SetShowGPIMsgCLIsGone(){m_bShowGPIMsgCLIsGone = TRUE;}			//4.0.23.510 TD 97894
	void					ClearShowGPIMsgCLIsGone(){m_bShowGPIMsgCLIsGone = FALSE;}		//4.0.23.510 TD 97894
	BOOL					GetShowGPIMsgCLIsGone()const {return m_bShowGPIMsgCLIsGone;}	//4.0.23.510 TD 97894


	void					HandleRedundancyByCL();										//4.0.23.508 TD 89049
	inline BOOL				IsRedundancyByCL()const;									//4.0.23.508 TD 89049
	inline void				SetRedundancyByCL(DWORD dwHydraActiveBitMap);				//4.0.23.508 TD 89049

	void					ResetExtraInfoQdxData();				//4.0.23.510 no TD

	long					CreateConnectToIFSFTerminalSrv();	//4.0.12.503
	long					CloseConnectToIFSFTerminalSrv();

	long					CreateConnectToFuelMobileSrv();	// CR 474446 
	long					CloseConnectToFuelMobileSrv();		// CR 474446 
	
	void					SetOLAInUse(long lServerType) {m_dwOLASrvInUse = lServerType;}	//Eyal 4.0.0.22

	void					GetCurrentTransactionNumber(long lPumpNumber, long *pTrsNumber, long *pVal); //TD 453656


		
	BOOL					IsAlarmSrvActive(){ return (m_dwAlarmSrvActive == 0) ? FALSE : TRUE; }				//4.0.23.501	TD 78374	
	

	inline BOOL  GetAutoReconcileTimeout() const { return m_bAutoReconcileProcess; } // 4.0.23.1050 TD 233242
    GCIRetCode              CheckPakReadyTransaction(long lPumpNumber, long lVolume,long lValue, long lFlags, long lTotalVal, long lTrsNumber);  //4.0.29.500 TD 277379	
	GPIRetCode				IsPostVoidTrsAllowed(long lPumpNumber ,short nUnpaid); //CR 351687: Coles Enhanced post void transaction
	BYTE				    GetTypeOfMeasure(short nGrade) const;
	long					GetCarWashAmountToCharge(long lPumpNumber);
	long					GetRequstedAuthorizeAmount(long lPumpNumber);
	long					GetAuthorizedAmount(long lPumpNumber);
	long					GetFirstTenderAuthorizedAmountLimit(long lPumpNumber);
	long					GetSecondTenderAuthorizedAmountLimit(long lPumpNumber);
	long					GetRemainingBalance(long lPumpNumber);
	void					FillTaxDetails(int iTaxIndex,TAX_PROPERTIES &cTaxPropertis);
	long					GetFuelTaxAmountFormCardSaleData(long lPumpIndex ,int iTaxIndex);
	long					GetCarWashTaxFromCardSaleData(long lPumpIndex,int iTaxId );

	void					CalculateTendersTotalAmount(long lPumpNumber, long lTotalAmount ,DOUBLE &dFirstCardTenderAmount,DOUBLE &dSecondCardTenderAmount);

	CAlarmSrvPS		m_cAlarm;		
	CCompChecker	m_cCompChecker;
	
	CAlarmSrvPS&	AlarmSrv() {
		return m_cAlarm;
	}

	const CAlarmSrvPS&	AlarmSrv() const {
		return m_cAlarm;
	}

	BOOL			m_bDBAlarm;

	// returns true if Database alaram has been sent
	BOOL			SentDBAlarm() const {
		return m_bDBAlarm;
	}

	// used to set or clear Database Alarm status
	void			SetDBAlaram(BOOL bFlag = TRUE) {
		m_bDBAlarm = bFlag;
	}

	CGPIEventManager m_cGPIEventMgr;//4.0.12.501
	CMap<long,long,ICtrlEvents* ,ICtrlEvents*> m_CtrlEventsObjMap;
	CMap<long,long,IGPIEvents* ,IGPIEvents*> m_GpiEventsObjMap;//4.0.12.501


		// Thread Data 
	struct COLAThreadInfo 
	{	
		CServerMain *m_pServer;
		HANDLE  m_hEventStartThread;
		HANDLE  m_hEventStopThread;
		HANDLE  m_hEventKillThread;
		HANDLE  m_hEventThreadKilled;
		HANDLE  m_hEventOLACmd;				
		HANDLE  m_hEventOLACmdDone;
	}m_COLAThreadInfo;

	DWORD	m_dwOLAThreadId;
	HANDLE	m_hThreadOLA;
	long	m_lOLAPumpNumber;
	long	m_lOLACmdType;

	DWORD GetCarWashPluCode() {return m_CarWashGeneralDefinitions.dwCarWashPluCode;}
	DWORD GetCarWashLoyaltyApplicable() {return m_CarWashGeneralDefinitions.dwCarWashLoyaltyApplicable;}
	DWORD GetCarWashDiscountCode() {return m_CarWashGeneralDefinitions.dwDiscountCode;}
	DWORD GetCarWashMinGallons() {return m_CarWashGeneralDefinitions.dwMinGallons;}
	DWORD GetCarWashDiscountPerGallon() {return m_CarWashGeneralDefinitions.dwDiscountPerMinGals;}
	DWORD GetCarWashScalarFactor() {return m_CarWashGeneralDefinitions.dwCarwashScaleFactor;}
	CString GetCarWashDeviceType() {return m_CarWashGeneralDefinitions.sType;}
	
	// Gets writable Pump
	CPump& GetPump(long PumpNumber) { const auto PumpIndex = PumpNumber - 1; return m_cPumpArray[PumpIndex]; }
	// RFUEL-2817
	// Gets Readonly Pump	
	const CPump& GetPump(long lPumpNumber) const { const auto PumpIndex = lPumpNumber - 1; return m_cPumpArray[PumpIndex]; }
	BOOL   IsValidPump(long lPumpNumber) const { const auto PumpIndex = lPumpNumber - 1; return (PumpIndex >= 0 && PumpIndex < m_cPumpArray.GetSize()); }
	BOOL   IsInvalidPump(long lPumpNumber) const {
		BOOL bValid = IsValidPump(lPumpNumber);
		BOOL bInvalid = (bValid == TRUE ? FALSE : TRUE);
		return bInvalid;
	}

	CFlowControlManager& GetFlowControlManager() { return	m_cFlowControlManager; }
	const CFlowControlManager& GetFlowControlManager() const { return	m_cFlowControlManager; }

	void					GetPumpInfo(const long lPumpNumber, PUMP_INFO* pPumpInfoOut);
	void GetPAPInfo(long lPumpNumber, PAY_AT_PUMP_INFO& cPumpInfo) const;
	void SetPAPInfo(long lPumpNumber, const PAY_AT_PUMP_INFO& cPumpInfo);



	// Controller info:	
	DWORD									m_dwMaxMemCreditDisplay;
	CString									m_strControllerName;
	CString									m_strConvertLayerVersion;
	CString									m_strPumpSrvVersion;
	BOOL									m_bControllerGenTrsNumber;
	BOOL									m_bControllerGenSeqNumber;
	BOOL									m_bControllerManagePumpMode;
	BOOL									m_bControllerManagePumpLock;
	BOOL									m_bControllerSupportAVI;
	BOOL									m_bControllerReportOnPaidTrs; //4.0.5.33
	BOOL									m_bControllerManagePumpMode2; //4.0.5.39
	BOOL									m_bControllerManagePrinterStatus; //4.0.5.44
	BOOL                                    m_bWasModeUpdated;               //4.0.5.48
	long									m_lSysError;
	long									m_lSysWarning;
	long									m_lSysMisc;
	long									m_lDiskWarning; //4.0.2.23
	DWORD									m_dwSysOnManualModes;//4.0.9.506
	CCardSaleAllData						m_cCardSaleDataXML;//4.0.9.506
	CTankLevelProtection					m_cTankLevelMap;//4.0.9.506 //amram
	BOOL									m_bControllerSupportNozzleRestrction;//4.0.10.502
	BOOL									m_bSupportControllerInMiddleRecoveryProcess;
	BOOL									m_bExpectAllDetailsInNeedMoreParam; //4.0.13.506;
	BOOL									m_bUseXmlBuffer; //4.0.14.500
	long									m_lOLAReportSoldItemOnPump;//4.0.10.502
	long									m_lSupportNACSRstrictions;	//49339
	DWORD									m_lPrintReceiptOnDecline;// Costco EMV
	BOOL									m_bAllowAuthOnNonRequestPapResPumpCL; //m_AllowAuthOnNonRequestPapResPump 4.0.21.500  TD 69683	
	BOOL									m_bControllerSupportStopAllPumps; //4.0.22.500 - TD 69907
	BOOL									m_AllowAuthOnStopByController; //4.0.22.511 
	BOOL									m_bControllerReportsSixDigitSeqNumber; //4.0.23.503
	
	//param that belong to mode and price changes

	long									m_lModeChangeNoOpenPrp;//4.0.9.506 //amram
	long									m_lModeChangeOnIdle;//4.0.9.506 //amram

	long									m_lPriceProtectionActive;
	long									m_lPriceProtectionStart;
	long									m_lPriceProtectionEnd;
	long									m_lPriceProtectionLevel;
	BOOL									m_bWaitForFinalizeResponse; //RFUEL-2117

	BOOL									m_bReduceCWDiscountFromCWAmount;

	void									DoAutomaticPriceProtectionCheck();

	DWORD									m_dwPAKAutoReconcileTimer;//4.0.9.506 //amram
	DWORD									m_dwPriceChangeLockEx;//4.0.9.510
	BOOL									m_bPresentCardName;//4.0.10.1000
	TRS_SHORT_CARD_LIST						m_cShortCardTrsArr[MAX_PUMPS_64];//4.0.10.1000
	
	// Pumps Info:
	CArray<CPump,CPump&>					m_cAlarmArray;

	// POS info:
	CMap< LONG, LONG, CPosInfo, CPosInfo& > m_cPosMap;
	CCriticalSection						m_csPosMap;


	// Pumps Info:
	CArray<CPump,CPump&>					m_cPumpArray;
	CCriticalSection						m_csPumpArray;


	// Internal Server:
	CList<CCmdInfo,CCmdInfo&>				m_cCmdQ;
	CCriticalSection						m_csCmdQ;


	// Modes Info:
	CArray<CMode,CMode&>					m_cModeArray;
	CRITICAL_SECTION						m_csModeArray;

    // ModeScheme Info
	CArray< CModeScheme ,CModeScheme& >	m_cModeSchemeArray;
	CRITICAL_SECTION				    m_csModeSchemeArray;	

	
	CArray<CMemPump, CMemPump&>				m_cMemPumpArray;
	CRITICAL_SECTION					    m_csMemPumpArray;

	// Tanks Info - add by Yaron:
	CArray<CTank,CTank&>					m_cTankArray;

	// Service fee info.
	CArray<CServiceFee,CServiceFee&>		m_cServiceFeeArray;

	CCriticalSection						m_csServiceFeeArray;

	// Reports Requests:
	CArray<CReport, CReport&>				m_cReportArray;
	CRITICAL_SECTION						m_csReportArray;
	OLA_DEBIT_MESSAGES_TIME_OUT_INFO		m_cOLADebitMessageTimeout[TIME_OUT_MAX_SESSION][MAX_PUMPS_64];  //4.0.0.66

	OLA_EXTRA_DATA							m_cOLAExtraArray[MAX_PUMPS_64];
	CArray<CDebitInfo, CDebitInfo&>			m_cDebitInfoArray;

	EXTRA_CHIP_INFO_PACKAGE					m_cExtraChipInfo[MAX_PUMPS_64]; //4.0.18.300
	EXTRA_CHIP_INFO                         m_cCvmListInfo[MAX_PUMPS_64]; //4.0.18.300
	
	//GetCardInfoEx
	CRITICAL_SECTION						m_csGetCardInfoEx; //4.0.19.0
	
	
	long									m_lPumpsManualModeArray[MAX_PUMPS_64];
	
	//The object in charge of handling exclusive single pump locks and unlocks
    CSinglePumpLock							m_cSinglePumpLock; //4.0.5.24 
	CFuelLoyaltyPS							m_cFuelLoayaltyPS;//4.0.9.508
	CFlowControlManager						m_cFlowControlManager; //Liat Copient 55286	//4.0.22.504 69344
	CBaseReceipt *							m_cBaseReceipt;//4.0.9.503 
	CParam									m_cParam;
	CIniFile								m_cIniFile;		//4.0.8.503		
	CLogger									m_cLogger;		

	CShiftLog								m_cShiftLog;
	CTrsLog									m_cTrsLog;//4.0.3.39
	COLABase *								m_cOLASrv;
	CExceptionLog							m_cExLog;
	CCarWash								m_cCarWash;
	CCarWashGeneralRequest					m_cCarWashGeneralRequest;//4.0.23.504 76432
	CAR_WASH_PROGRAM						m_cCarWashProgramsDetails[MAX_CAR_WASH_PROGRAM + 1];	// TD COSTCO-Tax 
	CProtectedDataHandler					m_cProtectedDataHandler; //4.0.20.501 TD 60236
	CConvertXML								m_cConvertXML;
	CTankXML								m_cTankXML;	//4.0.13.503
	CXmlConvertorEx							m_cXmlConvertorEx; //4.0.14.500
	CPumpTotalsXml							m_cPumpTotalsXml; //4.0.25.50 - TD 108311
	CStuckTrsLog							m_cStuckTrsLog; /// 4.0.24.770  TD 275612
	CXmlParser								m_cXmlParser;

	long									m_lPAPDeclineTimer[MAX_PUMPS_64]; //4.0.12.501
	CTotalPumpInfo							m_cTotalPumpInfo;//4.0.12.506
	long									m_lServerWaitCounter;
	BOOL									m_bTankReadingRcved;	
	BOOL									m_bDeliveryReportRcved;	
	DWORD									m_dwStationLimitType;  //1 for limit by value and 2 for limit by volume
	CAccumulationShiftTotal					m_cAccumulationShiftTotal; 
	CReceiptExtraIndexFile					m_cReceiptExtraIndexFile; //4.0.7.500
	CCashAcceptorFile						m_cCashAcceptorFile;  //4.0.10.506
	long									m_lMaxValidGradeNumber;//amram 4.0.10.502 hold the max number of valid grade
	DWORD									m_dwMaxGrade99;//amram 4.0.5550.1000 if on then we can use 99 grades
	CExtendedGradeMap						m_cExtendedGradeMap;	//4.0.26.501 - TD 116227
	DWORD									m_dwMaxGrade999;		//4.0.19.501 - TD 116227 - if on we support 999 grades (extended grades)
	CBuildLine2PosPump						m_cBuildLine2PosPump; //4.0.22.10 TD 71055
	CFuelingRestriction						m_cFuelingRestriction;		//4.0.22.501 TD 70956
	CSingleShiftAccumMgr					m_cSingleShiftAccumMgr;	 //4.0.25.80 - TD 108311
	CSiteConfigurationExporter				m_cSiteConfigurationExporter;				//4.0.26.508 TD 146575
	CFlowRateCalculator						m_cFlowRate;								//4.0.27.40 TD 165585
	BOOL									m_bOLAEpsilonSupport;
	long									m_lMaxAlternateIDInputData;	// QC 503092
	
	inline		CString						GetLoyaltyDataPath() const { return m_sLoyaltyDataPath;} // TD 321602   4.0.111.174 
	inline      DWORD						GetLoyaltyUniqueClient() const { return m_dwLoyaltyUniqueClient;} // TD 321602   4.0.111.174 
	inline		BOOL						GetTwoTendersInTransaction() const { return m_bTwoTendersInTransaction; }
	inline		BOOL						IsExportSiteConfiguration(){return m_cSiteConfigurationExporter.IsActive();};
	inline		long						GetMaxValidGradeNumber(){return m_lMaxValidGradeNumber;};//amram 4.0.10.502
	inline		void						SetMaxValidGradeNumber(long lMax){m_lMaxValidGradeNumber = lMax;};//amram 4.0.10.502
	inline		long						GetMaxGrade99(){return m_dwMaxGrade99;};//amram 4.0.10.502
	inline		void						SetMaxGrade99(long lGrade99){m_dwMaxGrade99 = lGrade99;};//amram 4.0.10.502		
	inline		long						GetOLAReportSoldItemOnPump(){return m_lOLAReportSoldItemOnPump;};//amram 4.0.10.502
	inline		void						SetOLAReportSoldItemOnPump(long lOlaReport){m_lOLAReportSoldItemOnPump = lOlaReport;};//amram 4.0.10.502		
	inline		void						SetSupportNACSRestriction(long lSupportNACS){m_lSupportNACSRstrictions = lSupportNACS;};	//49339
	inline		long						GetSupportNACSRestriction(){return m_lSupportNACSRstrictions;}; //49339
	inline		void						SetPrintReceiptOnDecline(DWORD lPrintReceipt){m_lPrintReceiptOnDecline = lPrintReceipt;};	//Costco EMV
	inline		DWORD						GetPrintReceiptOnDecline(){return m_lPrintReceiptOnDecline;};	//Costco EMV
	inline		long						GetMaxGrade999(){return m_dwMaxGrade999;};//4.0.26.501 - TD 116227
	inline		void						SetMaxGrade999(long lGrade999){m_dwMaxGrade999 = lGrade999;};//4.0.26.501 - TD 116227
	long									GetExtGradeIDByGradeID(long lGradeID, long lPumpNumber); //4.0.19.1100 128024 //4.0.25.30
	SHIFT_REC						    	GetShiftRecData(){return m_cCurrentShift;};//amram 4.0.10.505
	BOOL									IsServerInRecoveryProcess();
	void									SetServerRecoveryProcess(BOOL bServerInRecoveryProcess);
	BOOL									GetIFSFTerminalSrvInUse() {return (m_dwIFSFInUse != EPS_NONE && m_dwIFSFInUse != EPS_FLEXILANE);} //4.0.12.503 //4.0.31.?? - CR 335186
	BOOL									IsFuelMobileSrvInUse() { return (BOOL)m_dwFuelMobileSrvInUse; } // CR 444830
	void 									SetInOriginalPriceChangeProcess(BOOL bInOriginalPriceChangeProcess) {m_bInOriginalPriceChangeProcess = bInOriginalPriceChangeProcess;} //4.0.12.515
	BOOL									IsInOriginalPriceChangeProcess() {return m_bInOriginalPriceChangeProcess;} //4.0.12.515
	void 									SetPriceChangeResponse(long lPriceChangeResponse) {m_lPriceChangeResponse = lPriceChangeResponse;} //4.0.12.517
	long									GetPriceChangeResponse() {return m_lPriceChangeResponse;} //4.0.12.517
	
	void 									SetPriceChangePOSMsg(long lPriceChangePOSMsg) {m_lPriceChangePOSMsg = lPriceChangePOSMsg;} ///4.0.27.42
	long									GetPriceChangePOSMsg() {return m_lPriceChangePOSMsg;} //4.0.27.42

	void 									SetPriceChangeResponseDeviceNum(long lPriceChangeResponseDeviceNum) {m_lPriceChangeResponseDeviceNum = lPriceChangeResponseDeviceNum;} //4.0.12.517
	long									GetPriceChangeResponseDeviceNum() {return m_lPriceChangeResponseDeviceNum;} //4.0.12.517
	void 									SetRcvSetShiftCommand(long lRcvSetShiftCommand) {m_lRcvSetShiftCommand = lRcvSetShiftCommand;} //4.0.13.501
	long									GetRcvSetShiftCommand() {return m_lRcvSetShiftCommand;} //4.0.13.501
	void									PriceChangeSuccess(); //4.0.12.517
	void									PriceChangeFailure(); //4.0.12.517
	void									PriceChangePendingCommitAfterFailure(); 
	void									RollbackPriceChange(); //4.0.12.517 
	void									PriceChangeCommit(); //4.0.26.508 147183

	void 									SetInPriceChangeProcess(BOOL bInPriceChangeProcess); //4.0.12.518
	long									GetLoyaltyTransactionCount() {return (long)m_dwLoyaltyTransactionCount;} //4.0.14.501 	
	long									GetLoyaltyTransactionTimeout() {return (long)m_dwLoyaltyTransactionTimeout;} //4.0.14.501
	long									GetCheckAndReserveTimeout() {return (long)m_dwCheckAndReserveTimeOut;} //4.0.17.20 - TD 26384
	BOOL                                    GetActiveReOrganizeReceiptQdx() {return (m_bActiveReOrganizeReceiptQdx);}; //4.0.18.501 merge from 4.0.15.291
	long									GetLoyaltyTransactionCompleteTimeout() {return (long)m_dwLoyaltyTransactionCompleteTimeout;} //367311
	//4.0.18.501 merge from 4.0.18.20
	DWORD									GetReloadQDXEveryXShifts(){return m_dwReloadQDXEveryXShifts;};
	long									GetReloadQDXShiftsCounter(){return m_lReloadQDXShiftsCounter;};
	void									SetReloadQDXEveryXShifts(DWORD lNewVal){m_dwReloadQDXEveryXShifts = lNewVal;};
	void									SetReloadQDXShiftsCounter(long lNewVal){m_lReloadQDXShiftsCounter = lNewVal;};
	BOOL									GetStartupBaseTotalsUpdate(){return m_bStartupBaseTotalsUpdate;}; //47235 4.0.19.502
	void									SetStartupBaseTotalsUpdate(BOOL bVal){m_bStartupBaseTotalsUpdate = bVal;}; //47235 4.0.19.502
	BOOL									GetFinishedPrepayIndication(){return m_bFinishedPrepayIndication;};				//56040 4.0.19.508	
	void									SetFinishedPrepayIndication(BOOL bVal){m_bFinishedPrepayIndication = bVal;};	//56040 4.0.19.508	
	
	//4.0.19.504 - 47559
	void									CalculateTrsLimit(long lPumpNumber, long * lLimitType, long * lLimitValue);

	void									SetStartDeliveryTime();															//4.0.20.52 (62818)
	void									SetEndDeliveryTime();															//4.0.20.52 (62818)
	void									ReSetAccumulatedDeliveryEmulationSales();										//4.0.20.52
	void									SetFoundDelivery(BOOL bFoundDelivery){m_bFoundDelivery = bFoundDelivery;}		//4.0.20.52 (62818)
	BOOL									GetSupportDeliveryEmulation(){return m_bSupportDeliveryEmulation;}				//4.0.20.52 (62818)
	BOOL									GetActiveTankConnectedPumpsTable(){return m_bActiveTankConnectedPumpsTable;}	//4.0.20.52 (62818)
	void 									SetOfflineTrsInShift(BOOL bOfflineTrsInShift) {m_dwOfflineTrsInShift = (DWORD)bOfflineTrsInShift;} //4.0.12.54
	BOOL									GetOfflineTrsInShift(){return ((BOOL)m_dwOfflineTrsInShift);} //4.0.20.54
	
	inline DWORD							GetReceiptLookupMethod(){return m_dwReceiptLookupMethod;}						//4.0.20.501 TD 60236

	void                                    LoadTanksConnectedPumps();					   //TD 66935
	void									LockTankConnectedPumps(const long lTankNumber);//TD 66935
	void									SetLockPumpByTankStatus(const long lPumpNum,const LockPumpByTankStatus eLockPumpByTankStatus); //hermelin
	BOOL									IsPumpLockedByTank(const long lPumpNumber);
	DWORD									GetEmulationDeliveryTimeOut(){return m_dwEmulationDeliveryTimeOut;}	//4.0.21.500 TD G
	BOOL									GetIgnoreNonValidGradesOnPriceChangeFlag(){return m_bIgnoreNonValidGradesOnPriceChange;};				//4.0.24.60 - TD 107311

	inline void								LoadConfigurationThread(const BOOL bCreate);										//4.0.22.45 TD 72716
	inline BOOL								CreateConfigurationThread();														//4.0.22.45 TD 72716
	inline void								StartConfigurationThread();															//4.0.22.45 TD 72716
	BOOL									SetConfiguratonThreadData(const BYTE* szThreadData, const size_t iDataLength);		//4.0.22.45 TD 72716
	inline void								StopConfigurationThread();															//4.0.22.45 TD 72716
	inline void								CleanUpConfigurationThread();														//4.0.22.45 TD 72716
	DWORD									GetDynamicAttrForGradeID(){return m_dwDynamicAttrForGradeID;};	//4.0.23.170 102319
	void									SetDynamicAttrForGradeID(DWORD dwVal){m_dwDynamicAttrForGradeID = dwVal;}//4.0.23.170 102319
	void									SetFuelTransactionNumberMinValue(DWORD dwNewVal){m_dwFuelTransactionNumberMinValue = dwNewVal;}; //4.0.23.260 111318
	DWORD									GetFuelTransactionNumberMinValue(){return m_dwFuelTransactionNumberMinValue;}; //4.0.23.260 111318
	void									SetFuelTransactionNumberMaxValue(DWORD dwNewVal){m_dwFuelTransactionNumberMaxValue = dwNewVal;}; //4.0.23.260 111318
	DWORD									GetFuelTransactionNumberMaxValue(){return m_dwFuelTransactionNumberMaxValue;}; //4.0.23.260 111318
	void									SetForceSessionCompletePak(long p); // TD 256101
	inline BOOL								IsSiteLimitByVolume() { return m_dwStationLimitType == STATION_LIMIT_BY_VOLUME; }
	void									SetSiteLimit(long p , short nLevel , CMD_STRUCT_AUTHORIZE_LIMIT & cmdAuthorize); // 307758
	inline BOOL								IsRollingAsRemaning() const { return m_bRollingAsRemaining;}
	BOOL									IsPayPalActive() {return m_bPayPalActive;}
	void									SetMobilePaymentMisc(long lPumpNumber, BOOL bOn = TRUE);	//CR 438744
	BOOL 									GetShiftEOD(){return m_cCurrentShift.bShiftEOD;};//CR_475695_Start_End
	//4.0.26.501 - TD 116227
	short									GetExtGradeByGrade(long lGradeId, long *plExtGradeId, long lPumpNumber = 0);
	short									GetExtGradeByGrade(BYTE *sGradeId, long lSize, long lPumpNumber = 0);
	short									GetGradeByExtGrade(long lExtGradeId, long *plGradeId, long lPumpNumber = 0);
	BOOL									IsActiveANPR(); //4.0.26.505 157073
	void									SetANPRState(DWORD dwState); //4.0.26.505 157073
	void									SetNumOfPricePoleDisplays(long lNum); //4.0.26.508 147183
	BOOL									IsActivePricePole(); //4.0.26.508 147183	
	void									ActivatePriceChangeProcess(long lFlags, long lMaxUnitForDiscount); //4.0.26.508 147183
	long									GetPriceChangeBatchNumber(){return m_lPriceChangeBatchNumber;}; //4.0.26.508 147183
	void									SetPriceChangeBatchNumber(long lNewNumber); //4.0.26.508 147183
	BOOL									IsFlowRateActive();						//4.0.27.41							
	void									SetFlowRateActive(DWORD dwState);				
	void									UpdateTaxAmountWithoutOverfill(PAY_AT_PUMP_INFO *pData);  //TD 231498 4.0.27.500
	void									ResetGradePriceTable(short nGrade, long lMode, short nLevel,short nFlags); // TD 236747
	BOOL									CheckTripleAFlow(long lPumpNumber ,long lMediaMasterType);
	void									UnMaskAccountNumber(long lPumpNumber ,PAY_AT_PUMP_INFO * pInfo);
	BOOL									IsAfterOrInTheMiddleCancel( OLA_STAT * pOla );
	void									OpenNeedMoreParam(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, CString sMsg, int iMaxNumOfOpenKeys, long lTimeOut, short nONMPProperties, long lScreenId = 0 , PromptType ePromptType = PT_GENERIC);
	void									SortGradeLimitArrayByPosition(long lPumpNumber ,TAG_DISCOUNT_LOYALTY_INFO4 * pRec); // 4.0.23.1960  TD 378403
	BOOL									GetCancelInTheMiddleOfAuthorize(long lPumpNumber) { return m_bCancelInTheMiddleOfAuthorize[lPumpNumber-1];}  // TD 337501  4.0.23.1580
	void									SetCancelInTheMiddleOfAuthorize(long lPumpNumber,BOOL bMiddleOfAuthorize) { m_bCancelInTheMiddleOfAuthorize[lPumpNumber-1] = bMiddleOfAuthorize;} // TD 337501  4.0.23.1580 4.0.23.1580

	DWORD									GetMaxShiftNumber(){ return m_dwMaxShiftNumber; };		//TD 417989	

	BOOL									IsAutomaticAuthForLoyaltyCustomer() { return m_bAutomaticAuthForLoyaltyCustomer; }	// CR 407579
	long									GetPriceProtectionActive(){ return m_lPriceProtectionActive; }
	void									SetGradePriceToArray(long lGrade, long lPrice, long lPriceType);
	long									GetGradePriceFromArray(long lGrade, long lPriceType);
	long									GetHighestGradePriceFromArray(long lGrade); //RFUEL-3309 Get the maximal discount allowed for the grade 
	long									GetLowestGradePriceFromArray(long lGrade); //RFUEL-3309 Get the maximal discount allowed for the grade 
	long									m_lGradePriceArray[MAX_GRADES_99][5];//for every grade all 4 prices(in the order in the ini file) + lowest price of the day
	BOOL									BuildPriceProtectionDiscounts(long lPumpNumber, long* lDiscountArray);
	BOOL									BuildPriceProtectionDiscounts(long lPumpNumber, PAY_AT_PUMP_INFO*info);
	long									GetGradePriceProtectionLevel() { return m_lPriceProtectionLevel; }	// CR 438993
	DWORD									GetInitialGradePrice(long lPriceLevel, short nGrade);
	DWORD									GetInitialGradeLowestPrice(short nGrade); //RFUEL-3194 (task RFUEL-3257) limit minimal unit price after loyalty discounts are applied to according to lowest price
	DWORD									GetInitialGradeHighestPrice(short nGrade); //RFUEL-3309 Get the maximal discount allowed for the grade 
	DWORD									GetMaximalDiscountForGrade(short lGrade, long lPriceLevel = LOWEST_PRICE, long *plGradePrice=NULL); //RFUEL-3309 Get the maximal discount allowed for the grade 
	void									UpdatePakPumpTestExtraInfo(long lPumpNumber, CPumpTransact *pTrs);  //4.0.25.1140 TD 412765

	BOOL									GetReduceCWDiscountFromCWAmount() const { return m_bReduceCWDiscountFromCWAmount; }
	void									SetReduceCWDiscountFromCWAmount(BOOL val) { m_bReduceCWDiscountFromCWAmount = val; }	

protected:
	OperatingMode							m_lOperatingMode;
	ServerState								m_lServerState;
	
	// Server Files:
	CSQLExtraInfoFile						m_cSQLExtraInfoFile;
	CSQLOLATempTrs							m_cSQLOLATempTrs;
	CSQLOLATrs								m_cSQLOLATrs;
	CSQLPrePayFile							m_cSQLPrePayFile;
	CSQLCarWashFile							m_cSQLCarWashFile;
	CSQLItemsFile							m_cSQLItemsFile;
	CSQLTankGauge							m_cSQLTankGauge;
	CSQLPaidTrsFile							m_cSQLPaidTrsFile;
	CSQLPumpTotalsFile						m_cSQLPumpInoFile;
	CSQLTankReadsFile						m_cSQLTankReads;
	CSQLFuelDeliveriesFile					m_cSQLFuelDDeliveriesFile;
	CSQLAsh2000								m_cSQLAsh2000File;
	CSQLPumpSrvSingleShiftAccumulation		m_SQLSingleShiftAccumulationFile;
	CSQLPumpSrvReceiptExtraInfoDB			m_cSQLReceiptExtraInfoFile;
	CPumpFile								m_cPumpFile;
	

	void HandleReconfigureWhileEOS ();
	void DoAutomaticEmulationDelivery(); // Udi  - 4.0.5.24
	void DoAutomaticEmulationDeliverySingleTank(const long lTankNumber);//4.0.21.500 TD 66935
	void LogServerStatsOnStartup();
	BOOL m_bAllowPAKFinalizationWhenSytemDown;
	void LoadPricePolesTable();//4.0.1.32
	void ConvertDataBaseInfo();
	BOOL CanMoveToIDLE(BOOL* pbNeedAllPumpStatus);
	void OLAClassFactory(long lType , BOOL bRemote);
	void CreateOLAServer();
	void CreateBaseReceipt();//4.0.9.503
	void InitFuelEncryption();										//4.0.20.501 TD 60236
	void ConnectToHydService();
	void LoadPumpsTable(BOOL bCreate, BOOL bRun= TRUE);
	void LoadGeneralParamTable(BOOL bCreate);
	void RefreshGeneralParamKey(char *szFiledName);
	void LoadPureProductsTable(BOOL bCreate,  BOOL bRun =TRUE);
	void LoadShiftsTable(BOOL bCreate, BOOL bRun =TRUE);
	void LoadReceiptTable(BOOL bCreate,BOOL bRun =TRUE);
	void LoadMessagesTable(BOOL bCreate);
	void LoadScreenLalLogFormatyoutsTable(BOOL bCreate, BOOL bRun= TRUE);
	void LoadModesTable(BOOL bCreate, BOOL bRun);
	void LoadServiceFeeTable(BOOL bCreate, BOOL bRun=TRUE);
	void LoadPathsTable();
	void LoadTanksTable(BOOL bRun=TRUE);
	void LoadDataBaseTable();
	void SendPumpMode2Events(); 	//4.0.5.48 - Sending the initial changes in the modes when the server becomes idle 
	void SendPumpMode3Events();//4.0.9.506

	CDataExportThread		m_cDataExportSrv;	// 4.0.5.501
	//CQDXMaintThread			m_cQDXMaintThread;	// 4.0.18.0
	CVPThread				m_cVPThread;		// 4.0.24.60
	CConfigurationThread	m_cConfigurationThread;			//4.0.22.45 TD 72716
	CFuelMobileSrvThread	m_cFuelMobileSrvThread;

	CMD_STRUCT_RECONFIGURATION_EVENT m_cmdReconfigureEvents[32];
	ULONG m_ulRegisteredReconfigureEvents;
	BOOL m_bReconfigureRequestWasSent;
	DWORD m_dwLastReconfigureRequestTime;
	

private:				
	void CheckIfAnotherPumpSrvIsRunning(BOOL bCreateMutex = TRUE);
	void CheckIfAnotherVPIsRunningAndStopIt(BOOL bCreateMutex = FALSE);	//4.0.24.60

	void IncreaesPumpByModeRetryCounter(long lPumpNumber);
	void StartUnLockPumpByMode(long lPumpNumber);
	void StartLockPumpByMode(long lPumpNumber);
	void SetLockPumpByModeStatus(long lPumpNum,LockPumpByModeStatus eLockPumpByModeStatus);
	BOOL ProcessAlarmCommand(CAlarmSrvQueueData & cAlarmSrvQueueData);
	
	DWORD m_dwSaveZeroPakTrs;
	DWORD m_dwSaveDebitZeroCompleteData;  //4.0.2.36
	long  m_lMinFullingToChargeServiceFee;  //4.0.2.21
	long  m_lManualServiceFee;  //4.0.8.503
	DWORD m_dwDebitSingleSession;
	DWORD m_dwNozzleLeftOut;
	DWORD m_dwVpActive;
    long  m_lVpRecoveryMaxRetries;  // 4.0.29.500 TD 264908
	BOOL  m_bVpIsGone;
	BOOL  m_bQdxIsGone;               // 4.0.29.501 TD 288369
	DWORD m_dwDataBaseMaintenanceTimeout; //4.0.5550.1504
	DWORD m_dwNoParallelPAK_PrP;	//4.0.9.501
	DWORD m_dwIndicateAuthorizePumpSent;  //4.0.30.500 - TD 320068
	CVpMng m_cVpMng;     
	
	void UpdateBlenderCount();
	DWORD m_dwWetStockMaint;
	
	void UpdateShiftAtPumps();//4.0.0.78
	void CloseConnectionToAlarmSrv();
	void CloseConnectionToEncryptDll();//4.0.9.508
	
	DWORD m_dwAlarmSrvActive;
	void ConnectToAlarmSrv();
	
	DWORD m_dwItemsIndex;

	//Eyal 4.0.0.25 Start
	long m_lNewOLAServer;
	CRITICAL_SECTION m_csNewOLAServer;
	//Eyal 4.0.0.25 End

	void ResetPSPumpInfo(long lFileValid,long lPumpNumber);
	void ResetPSShiftInfo();
	void ResetPSServerInfo();

	BOOL	m_bAutoTimeCheck;
	SYSTEMTIME m_cPrevSysTime;
	
	long SetRealTimeClock();
	BOOL bPostVoidActive;
	BOOL bCashAcceptorActive;//4.0.12.502
	BOOL m_bAllowParallelAVIUsage;
	DWORD m_dwAviTrsTimeOut;
	DWORD m_dwSecondTrsAuthTimeout;//4.0.15.501
	void LoadTableModeSchemToArray(BOOL bRun= TRUE);
	DWORD m_dwMaxVolumeForVITConflict; //3.0.0.8
	DWORD m_dwCheckVITConflict; //3.0.0.8
	DWORD m_dwSuppressedDriveoffTimeout;
	long m_lWatchdogCount;
	BOOL m_bRollingAsRemaining;
	BOOL DoCancelInTheMiddleOfAuthorize( PAY_AT_PUMP_INFO * pInfo, OLA_STAT * pOla, long p, CPumpTransact &trs ); // 357023
	long DoAutoOLASendEnd(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);
	long DoAutoOLAProcessAfterSendEnd(long lPumpNumber, OLA_STAT& PreviousOLA,  OLA_STAT& Ola, PAY_AT_PUMP_INFO& pInfo);
	long DoAutoOLARequestToSendMore(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);
	long DoAutoOLAWaitForMoreInfo(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo, OLA_DEBIT_MESSAGES_TIME_OUT_INFO *pDebitTimeout = NULL);
	long DoAutoOLAWaitResponse(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);
	long DoAutoOLARequestToSend(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);
	
	//Start 4.0.21.500 TD 66935
    void HandleStartLockState(const BYTE lProcessCommand, const long lProcessStoppedStatus, const long lTankNumber, const long lTankReference, const BOOL bLockConnectedPumps);
	void HandleWaitForLockState(const BYTE lProcessCommand, const long lProcessStoppedStatus, const long lTankNumber, const long lTankReference, const BOOL bLockConnectedPumps);
	void HandleWaitTimeOutState(const BYTE lProcessCommand, const long lProcessStoppedStatus, const long lTankNumber, const long lTankReference, const BOOL bLockConnectedPumps);
	void HandleRequestTankReadingState(const BYTE lProcessCommand, const long lProcessStoppedStatus, const long lTankNumber, const long lTankReference, const BOOL bLockConnectedPumps);
	void HandleWaitTankReadingState(const BYTE lProcessCommand, const long lProcessStoppedStatus, const long lTankNumber, const long lTankReference, const BOOL bLockConnectedPumps);
	void HandleUnLockState(const BYTE lProcessCommand, const long lProcessStoppedStatus, const long lTankNumber, const long lTankReference, const BOOL bLockConnectedPumps);
	//End 4.0.21.500 TD 66935

	void ClearDiscountsInfo(long lPumpNumber, PAY_AT_PUMP_INFO *pPAPInfo); //4.0.28.503 - TD 263766

	
	void SetApplicationSingleCore();									//4.0.24.90 - TD 106227 
	// CR 438993
	void SetGradePriceMinimumLimit(long lGradePriceMinimumLimit);

	long ValidateDiscount(long lGradePrice, long lDiscount, long lMinimumGradePrice);
	void SetLowestPriceToCurrentPrice(SYSTEMTIME st);

	void HandlePriceChangeResponseReceived();	// TD 422282 
	void HandlePriceChangeLocked();				
	void HandlePriceChangeWaitLock();			
	
	void HandlePriceChangeUnlock();	

	// TD COSTCO-Tax:
	void CreateTaxCalcator();
	void CreateCandianTaxCalaclator();
	BOOL LoadCanaidanParams(OUT DWORD &dwGST, OUT DWORD &dwQST, OUT DWORD & dwPST);
	
	long GetTaxProperties(DWORD dwTaxMap, TAX_PROPERTIES * TaxProp = NULL);
	void FillCarWashTaxDetalis(long p, CarWashTaxDetails &cCarWashTax);
	void FillGradeTaxDetalis(long p, FuelTaxDetails &cFuelDetails,CPumpTransact*pTrs);
	long TranlateTaxMap(DWORD dwTaxMap, BOOL bTaxIncluded = 1);
	CTaxCalculatorBase			*m_cTaxCalclator;
	// TD COSTCO-Tax end

	static const int RECONFIGURE_TIMEOUT;

	DWORD									m_dwAddRollingUpdateDataToLog; //3.2.1.63
	DWORD									m_dwClearTrsOnIdlePump; //3.2.1.63
	DWORD									m_dwSetWaitingPrePayToTimeOutFlags; //4.0.0.84 //4.0.0.85
	DWORD									m_dwForceCreditLimitCheck;
	BOOL									m_bActiveLoyaltyDiscount;
	DWORD									m_dwLockPumpByMode;
	DWORD									m_dwEmulationDeliveryTimeOut; //Udi  - 4.0.5.24
	DWORD									m_dwVolumeMultiply;
	BOOL									m_bUseServiceFee;
	DWORD									m_dwTransactNumber;
	DWORD									m_dwShiftMode;
	DWORD									m_dwWetStockMode;
	DWORD									m_dwWetStockInterval;
	DWORD									m_dwWetStockMemoFlags;	//4.0.7.501
	DWORD									m_dwMemoInterval;		//Add by Yaron
	DWORD									m_dwWetStockClearTime;	//Add by Yaron
	DWORD									m_dwWetStockStateTimeout;	// TD 351225
	long									m_lMessageNumber;
	DWORD									m_dwOLAMode;		
	long									m_lAutoState[MAX_PUMPS_64];
	long									m_lPrePayReTryReservation[MAX_PUMPS_64]; //4.0.24.351 //4.0.124.355 //4.0.27.60 231255
	
	DWORD									m_dwCreditLimitByUser;	
	DWORD									m_dwDebitLimitByUser;	
	DWORD									m_dwVolumeLimit;	
	DWORD									m_dwCashLimit;
	DWORD									m_dwCreditLimit;
	DWORD									m_dwPrePayLimit; //4.0.3,32
	DWORD									m_dwForcePreSetToSiteLimit; //4.0.3.44
	DWORD									m_dwRunningValueThreshold;	//4.0.7.500
	DWORD									m_dwEnhancedRollingUpdate;  //TD 37244
	DWORD									m_dwActiveReceiptExtraIndex;
	DWORD									m_dwActiveReadNumber;
	DWORD									m_dwLoyaltyTransactionCount; //4.0.14.501	
	DWORD									m_dwLoyaltyTransactionTimeout; //4.0.14.501
	DWORD									m_dwLoyaltyTransactionCompleteTimeout; //367311
	DWORD									m_dwInMaintenanceQdxNum;	//33898
	SHIFT_REC								m_cCurrentShift;
	BOOL									m_bMemberCardAccepted[MAX_PUMPS_64];
	
	// File Indexes counters
	DWORD									m_dwTankReadIndex;
	DWORD									m_dwDeliveryIndex;
	DWORD									m_dwAlarmIndex;
	DWORD									m_dwSeverityAlarmsCount; //4.0.13.500
	DWORD									m_dwCarWashIndex;
	DWORD									m_dwTotalIndex;
	DWORD									m_dwOLAIndex;

	DWORD									m_dwPrePayAuthorizedTimeOut;
	DWORD                                   m_dwPrePayEnabled;    //4.0.0.73
	DWORD									m_dwCashAcceptorReportActive;
	DWORD									m_dwMinAlertSeverity_SoundAlert; //4.0.13.500
	DWORD									m_dwRegion; 
	DWORD									m_dwHotPumping;
	DWORD									m_dwStationLevelEncryption; 
	DWORD									m_dwTaxRegulation;
	DWORD									m_dwAutoReceiptCheck; 
	DWORD									m_dwAutoForce; 
	DWORD									m_dwOLAResponseTimeOut;
	DWORD									m_dwOLACheckingInterval;
	DWORD									m_dwAutoSwitch; 
	DWORD									m_dwRemoveStuckTrs;

	BOOL									m_bRcvAnyMsg;
	DWORD									m_dwOLASrvInUse; 
	DWORD									m_dwOLALogDeclineReq; 

	DWORD									m_dwAllowAuthOnNonRequest; 
	DWORD									m_dwNozzleLevelLogic; 
	long									m_lDataBaseMaintenanceTimer; //4.0.5550.1504
	DWORD									m_dwOLAByController;
	GRADE_PROPERTIES						m_GradesRegistryData[MAX_GRADES_99];
	WASH_PROGRAMS_PROPERTIES				m_WashProgramsData[MAX_WASH_PROGRAMAS+1];
	CString									m_strEncryptionKey;
	CString									m_sSiteID;  
	CString									m_sLoyaltyDataPath;	// TD 321602   4.0.111.174 
	DWORD									m_dwLoyaltyUniqueClient; // TD 321602   4.0.111.174 
	BOOL									m_bServerInRecoveryProcess;
	DWORD									m_dwPAPDeclineTimeValue; //4.0.12.501
	DWORD									m_dwUnbalancedPrPTimer; //4.0.13.500
	DWORD									m_dwIFSFInUse; //4.0.12.503
	DWORD									m_dwFuelMobileSrvInUse; // CR 474446
	DWORD									m_dwRollbackOnPriceChangeFailure; //4.0.12.515
	BOOL									m_bInOriginalPriceChangeProcess;  //4.0.12.515 
	long									m_lPriceChangeResponse; //4.0.12.517
	long									m_lPriceChangeResponseDeviceNum; //4.0.12.517	
	long                                    m_lPriceChangePOSMsg; //4.0.27.42
	long									m_lRcvSetShiftCommand; //4.0.13.501	
	long									m_lInvoiceRefNumber; //4.0.13.505 
	BOOL									m_bIsMasterComputer; //4.0.14.502
	DWORD									m_dwPrintZeroTrsReceipts;	// 4.0.15.504 - TD 38453
	DWORD									m_dwCheckAndReserveTimeOut;	// 4.0.17.20 - TD 26384
	DWORD									m_dwSendZeroValuesWhenOffline; //4.0.15.120 - CR
	BOOL									m_bPayPalActive;               // TD 339257
	DWORD									m_dwReloadQDXEveryXShifts;	//4.0.18.20 - CR 33898
	long									m_lReloadQDXShiftsCounter; //4.0.18.20 - CR 33898
	DWORD									m_dwAllowMultiPosOwnersOfPumpTrs; //4.0.23.501 78146
	DWORD									m_dwMaxShiftNumber;			//TD 417989
	BOOL                                    m_bIsInTheMiddleOfDoAutomaticReceiptCheck;  //CR 33898

	long									m_lCanMoveToIdleMaxCount; //4.0.18.501 merge from 4.0.15.280 
	BOOL									m_bActiveReOrganizeReceiptQdx; //4.0.15.291

	long									m_lReceipt00Counter; //4.0.19.502
	BOOL									m_bAutomaticAuthForLoyaltyCustomer;     // CR 407579

	//Add by Yaron for Ctrl Events
	long									m_lCtrlEventNumerator;//Ctrl events clients numerator.
	long									m_lGpilEventNumerator;//GPI events clients numerator.amram111 4.0.12.501
	BOOL									m_bStartupBaseTotalsUpdate; //47235 4.0.19.502
	BOOL									m_bFinishedPrepayIndication;	//56040 4.0.19.508	

	BOOL									m_bSupportDeliveryEmulation;		//4.0.20.52 (62818)
	BOOL									m_bActiveTankConnectedPumpsTable;	//4.0.20.52 (62818)
	BOOL									m_bFoundDelivery;					//4.0.20.52 (62818)
	DWORD									m_dwOfflineTrsInShift;				//4.0.20.54

	DWORD									m_dwReceiptLookupMethod;		//4.0.20.501 TD 60203
	BOOL									m_bPAPEnabledOLA;
	DWORD                                   m_dwPumpLockTimeOut;			//4.0.21.500 TD 66935

	DWORD                                   m_dwCloseOpenSalesTimeOut;			//4.0.22.503 - TD 72189

	DWORD                                   m_dwWetStockReportIncludeOldData;	//4.0.22.503 - TD 69905
	BOOL									m_bCLIsGone;						//4.0.23.507 TD 89047
	BOOL									m_bShowGPIMsgCLIsGone;				//4.0.23.510 TD 97894
	
	DWORD                                   m_dwPriceChangeWithAckPerPumpTimeout;	//4.0.19.820 - TD 78775
	
	BOOL									m_bIgnoreNonValidGradesOnPriceChange;	//4.0.24.60 - TD 107311
	DWORD									m_dwDynamicAttrForGradeID;				//4.0.23.170 102319 //4.0.24.60
	DWORD									m_dwUseAdditionalCodeAsPlu;				//4.0.23.170 102319 //4.0.24.60
	DWORD									m_dwFuelTransactionNumberMinValue;		//4.0.23.260 111318
	DWORD									m_dwFuelTransactionNumberMaxValue;		//4.0.23.260 111318
	DWORD                                   m_dwAddEncodingStringToVPXml;			//4.0.24.90 HOT fix 1022
	DWORD                                   m_dwSingleCoreActive;			        //4.0.24.90 TD 106227 
	DWORD                                   m_dwSingleShiftAccumulationsActive;		//4.0.25.50 - TD 108311
	DWORD									m_dwReloadQDXEveryXDays;				//4.0.26.501 TD 144220
	SYSTEMTIME								m_cReloadQdxLastTime;					//4.0.26.501 TD 144220
	DWORD									m_dwActiveANPR;							//4.0.26.505 157073
	DWORD									m_dwNumOfPricePoleDisplays;				//4.0.26.508 147183
	DWORD									m_lPriceChangeBatchNumber;				//4.0.26.508 147183
	DWORD									m_dwFlowRateActive;						//4.0.27.41 TD 165585
	BOOL									m_bAutoReconcileProcess;		   // 4.0.23.1050 TD 233242
	DWORD									m_dwManualLoyalty;
	BOOL									m_bManualPayment;					//4.0.20.1280 - CR 360293 - Electonic Wallet+ Manual Loyalty
	BOOL									m_bCancelInTheMiddleOfAuthorize[MAX_PUMPS_64];	// TD 337501
	long									m_lCheckLoyaltyTimeout;				// 4.0.23.2100	
	long									m_lCreateFleetRecord;
	long									m_lGradePriceMinimumLimit;		//	CR 438993

	

	struct CEventsThreadInfo 
	{
		public:
		CServerMain *m_pServer;
		HANDLE m_hEventStartThread;
		HANDLE m_hEventStopThread;
		HANDLE m_hEventKillThread;
		HANDLE m_hEventThreadKilled;
	}m_cEventsThreadInfo;	

	HANDLE	m_hEventThread;
	DWORD	m_dwEventThreadId;
	void InitEventThreadInfo();
	void CreateEventThread();
	void DeleteEventThreadInfo();
	

	struct CAlarmSrvThreadInfo 
	{
		public:
		CServerMain *m_pServer;
		HANDLE m_hAlarmSrvStartThread;
		HANDLE m_hAlarmSrvStopThread;
		HANDLE m_hAlarmSrvKillThread;
		HANDLE m_hAlarmSrvThreadKilled;
	}m_cAlarmSrvThreadInfo;

	HANDLE	m_hAlarmSrvThread;
	DWORD	m_dwAlarmSrvThreadId;

	void CreateAlarmSrvThread(); // 4.0.1.40
	void InitAlarmSrvThreadInfo(); // 4.0.1.40
	void DeleteAlarmSrvThreadInfo();// 4.0.1.40

	long m_lMaxPumps;					//4.0.0.79 
	long m_lLastValidPump;


	long m_lLastValidTank; 
	
	CPumpLock m_cPumpsLockStateArray[MAX_PUMPS_64];//4.0.3.49
	CPumpLockByTank m_cPumpsLockbyTankStateArray[MAX_PUMPS_64];//Hermelin

	IIFSFTerminalCtrlPtr m_pIFSFTerminalSrvCtrl; //4.0.12.503
	IFuelMobileCtrlPtr m_pFuelMobileSrvCtrl; //CR 474446

	BOOL	m_bQdxEmptied;							//4.0.27.500  TD 236009

	//4.0.29.0 - TD 241916
	BOOL m_bShiftStateUnlockHandled;			
	void HandleShiftStateUnlock();
	void HandleShiftStateStarted();
	void HandleShiftStateResponseReceived();
	void HandleShiftStateLocked();
	void HandleShiftStateWaitResponse();

	BOOL  IfExistTrsByControllerSeqNum(long lPumpNumber, long lFlags , long lControllerSeqNum, long lTrsNumber);  // 4.0.29.500 TD 277379
	BOOL  IsPakTransZero(long lVolume,long lValue, long lFlags);  //4.0.29.500 TD 277379
    BOOL  IsPakTransDuplControllerSeqNum(long lPumpNumber, long lFlags, long lControllerSeqNum, long lTrsNumber);  //4.0.29.500 TD 277379
	BOOL  IsPumpConnctedToTank(long lTankNumber,int p);
	BOOL  SetPumpsLockState(long lTankNumber,BOOL bLock);	
	BOOL  ClearPumpsConnectedToTankStatus(long lTankNumber);
	void  CreateFuelMobileSrvThread();
	void  FuelMobileSrvThreadCleanUp();
	
	DWORD	m_dwContactlessCreditLimit;	//  [1/18/2015 MaximB] CR 274401
	DWORD	m_dwContactlessDebitLimit;	//  [1/18/2015 MaximB] CR 274401
	//CR_475695_Start
	BOOL PriceChangeReadPumpTotals( );
	BOOL PriceChangeTankReadTotals();
	//CR_475695_End	
};

void HandleMiscCardNotAuthorized();


inline void CServerMain::GetPAPInfo(long lPumpNumber, PAY_AT_PUMP_INFO& cPumpInfo) const {
	if (IsValidPump(lPumpNumber)) {
		const auto& Pump = GetPump(lPumpNumber);
		Pump.GetPAPInfo(&cPumpInfo);
	}
	else {
		CString sLogMsg;
		sLogMsg.Format("%s  - Invalid Pump Number. PumpNumber=%d", __FUNCTION__, lPumpNumber);
		const_cast<CLogger&>(m_cLogger).LogMsg(sLogMsg, LOG_LEVEL_0);
	}
}


inline void CServerMain::SetPAPInfo(long lPumpNumber, const PAY_AT_PUMP_INFO& cPumpInfo) {
	if (IsValidPump(lPumpNumber)) {
		auto& Pump = GetPump(lPumpNumber);
		Pump.SetPAPInfo(&cPumpInfo);
	}
	else {
		CString sLogMsg;
		sLogMsg.Format("%s  - Invalid Pump Number. PumpNumber=%d", __FUNCTION__, lPumpNumber);
		m_cLogger.LogMsg(sLogMsg, LOG_LEVEL_0);
	}
}

