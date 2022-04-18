// RegTable.h: interface for the CRegTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGTABLE_H__4F4A75EC_8143_4D5F_8508_588ADAC2E5BD__INCLUDED_)
#define AFX_REGTABLE_H__4F4A75EC_8143_4D5F_8508_588ADAC2E5BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegTable  
{
public:
	void ReloadDataExportSrv(BOOL bCreate , BOOL bCompare);
	long ConvertTableNameToTableId(char* pszTableName);
	void LoadDOMSSubTable(long lSubTable , BOOL bCreate , BOOL bCompare);
	void LoadBankNoteTable(BOOL bCreate, BOOL bCompare, BOOL bRun);
	CRegTable();
	virtual ~CRegTable();

	static BYTE ReconfigInProgress() { return m_byReconfigInProgress; }
	static void SetReconfigInProgress(BYTE byInProgress) { (byInProgress?1:0); }

	//4.0.2.37
	//This method automatically returns TRUE, unless one of the
	//reconfigured tables is the OLA table, in which case it returns the
	//LoadOLATable reurn code.
	BOOL LoadGeneralParamSubTable(long lSubTable , BOOL bCreate , BOOL bCompare);
	void LoadRFSSubTable(long lSubTable , BOOL bCreate , BOOL bCompare);

	void LoadPumpSrvInternalParams();
	void LoadForecourtLockConfiguration();
	
	//4.0.2.37
	//This method can fail, if we try to reconfigure when
	//in middle of trs.
	BOOL LoadOLATable(BOOL bCreate , BOOL bCompare);
	
	void LoadForecourtLimitSettings(BOOL bCreate, BOOL bCompare);
	void LoadActivesTable(BOOL bCreate, BOOL bCompare);
	void LoadWetStockConfiguration(BOOL bCreate , BOOL bCompare);
	void LoadTimers(BOOL bCreate, BOOL bCompare);
	void LoadAVIParameters(BOOL bCreate, BOOL bCompare);
	void LoadMiscellaneousParameters(BOOL bCreate, BOOL bCompare);
	void LoadLogTable(BOOL bCreate, BOOL bCompare);
	
	void LoadFuellingPoints(long lNumber, BOOL bCreate, BOOL bCompare, BOOL bRun);
	void LoadDispensersTable(long lPumpNumber, BOOL bCreate, BOOL bCompare, BOOL bRun);
	void LoadPumpsTable(long lPumpNumber, BOOL bCreate, BOOL bCompare, BOOL bRun);

	void LoadModeSchemeTable(BOOL bRun);

	void LoadGradesTable(BOOL bCreate, BOOL bCompare, BOOL bRun);

	void LoadWashProgramsTable(BOOL bCreate , BOOL bCompare , BOOL bRun);

	void LoadPureProductsTable(BOOL bCreate , BOOL bCompare , BOOL bRun);

	void LoadShiftsTable(BOOL bCreate , BOOL bCompare , BOOL bRun);

	void LoadReceiptTable(BOOL bCreate , BOOL bCompare , BOOL bRun);

	void LoadMessagesTable(BOOL bCreate , BOOL bCompare  , BOOL bRun);

	void LoadScreenLayoutsTable(BOOL bCreate, BOOL bCompare , BOOL bRun);

	void LoadModesTable(BOOL bCreate, BOOL bCompare , BOOL bRun);

	void LoadLanguagesTable(BOOL bCreate, BOOL bCompare , BOOL bRun); //4.0.5.42 

	void LoadServiceFeeTable(BOOL bCreate, BOOL bCompare , BOOL bRun);

	void LoadTanksTable(BOOL bCreate , BOOL bCompare , BOOL bRun);

	void LoadDatabaseTable(BOOL bCreate , BOOL bCompare , BOOL bRun);

	void LoadPathsTable(BOOL bCreate , BOOL bCompare);

	void LoadPricePolesTable(BOOL bCreate , BOOL bCompare);
	
	void LoadANPRTable(BOOL bCreate , BOOL bCompare); //4.0.26.505 157073

	void LoadTaxesTable(BOOL bCreate , BOOL bCompare);

	void LoadIPTTable(BOOL bCreate , BOOL bCompare);

	void LoadTerminalTable(BOOL bCreate , BOOL bCompare);

	void LoadFuelLoyalty(BOOL bCreate , BOOL bCompare);//4.0.9.510

	void LoadMultiSync(BOOL bCreate , BOOL bCompare);//4.0.14.502
	void LoadFuelFlowControl(BOOL bCreate , BOOL bCompare, BOOL bReconfigure = FALSE); //4.0.19.508	//4.0.22.504 69344
	void LoadFuelFlowControlForNMP(char *sPath, BOOL bCreate , BOOL bCompare, long lSetUp); //4.0.19.508 4.0.20.10

	void LoadTerminalSrvParams(BOOL bCreate); //4.0.20.501 - TD 65908


	void LoadEncodingTable(const BOOL bCreate, const BOOL bCompare = TRUE)const;				//4.0.22.501	TD 72716
protected:
	void ReloadPostVoid(BOOL bCreate , BOOL bCompare);
	void ReloadCashAcceptorActive(BOOL bCreate , BOOL bCompare);//4.0.12.502

	void ReloadAlarmSrv(BOOL bCreate , BOOL bCompare);
	void ReloadHydra(BOOL bCreate , BOOL bCompare);
	void ReloadVP(BOOL bCreate , BOOL bCompare);
	void ReloadIFSFTerminalSrv(BOOL bCreate , BOOL bCompare); //4.0.12.503
	void ReloadFuelMobileSrv(BOOL bCreate, BOOL bCompare); // CR 474446 

private:
	void ReloadDefinitionForAllPumps();
	void LoadRealTimeTable(BOOL bCreate);

	static BYTE m_byReconfigInProgress;
};

#endif // !defined(AFX_REGTABLE_H__4F4A75EC_8143_4D5F_8508_588ADAC2E5BD__INCLUDED_)
