// PumpSrvDB.h: interface for the CPumpFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUMPSRVDB_H__DD8F77B3_9301_11D1_9F7A_004005615A06__INCLUDED_)
#define AFX_PUMPSRVDB_H__DD8F77B3_9301_11D1_9F7A_004005615A06__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DB.h"
#include "PumpClass.h"
#include "CardSaleAllData.h" //4.0.9.507
#include "ReceiptInfo.h"

#define FILE_VALIDATION_LEN	31

#define MAX_RECEIPT_INDEX 177 /*148/*198*/ //4.0.16.503	//4.0.22.501 TD 72716 
#define MAX_INDEX_FOR_RECORDS_LEFT_AFTER_REORGINAZATION 30 /*40*/ //Ella 4.0.16.503

#define OVERFLOW_DANGER (MAX_RECEIPT_INDEX + MAX_INDEX_FOR_RECORDS_LEFT_AFTER_REORGINAZATION)

//4.0.14.500
#define MAX_OLA_REC	1000 
#define PAP_INFO_BUFF_PART_LEN 1974	//change of this const should chg MAX_XML_BUF_LEN const

#define MAX_RECEIPT_INDEX_TO_CHECK_REORGANIZATION	90 /*75*/	//4.0.22.501 TD 72716 
#define	XML_REC_SIZE								960	//4.0.16.503

//qdx parameters 4.0.18.501
#define OFFSET_TO_NEXT_RECORD	1
#define NUM_OF_RECORDS_FOR_ONE_OLA_BUFFER	5
#define NUM_OF_RECORDS_FOR_ONE_RECEIPT_BUFFER	7	//4.0.22.501 TD 72716


//#pragma pack(1)



// -----------------------------------
// Pump Section............
// -----------------------------------
//4.0.9.507
typedef struct 
{
	LONG			m_lNumber;
	BYTE			m_sPumpMenufacture[MAX_NAME_LENGTH];	
	LONG			m_lType;					
	CPumpTransact	m_cTrs[MAX_TRS_IN_LIST];
	OLD_OLA_STAT	m_cOLA;	
	CPumpStatus		m_cPumpStat;
	LockStatus		m_lLockStatus;
	long			m_lShiftNumber;
	TAG_SYSTEMTIME  m_cLastTimeUpdate; //14
	BYTE			spare[28]; //4.0.19.500 - TD 59171
	//BYTE			spare[32]; 
	//BYTE			spare[46]; 
}SHORT_OLA_STATE_PUMP_INFO; // 600 
typedef struct _PUMP_INFO
{
	LONG			m_lNumber;
	BYTE			m_sPumpMenufacture[MAX_NAME_LENGTH];	
	LONG			m_lType;					
	CPumpTransact	m_cTrs[MAX_TRS_IN_LIST]; //86*6 = 516 //4.0.18.660 68689 //4.0.21.502
	OLA_STAT		m_cOLA;	
	CPumpStatus		m_cPumpStat;
	LockStatus		m_lLockStatus;
	long			m_lShiftNumber;
	TAG_SYSTEMTIME  m_cLastTimeUpdate; //14
	//BYTE			spare[23]; //4.0.19.500 - TD 59171 //4.0.21.500 69203	//4.0.21.502 68689
	//BYTE			spare[31]; 
	//BYTE			spare[46]; 
}PUMP_INFO; // 600 //4.0.21.502 68689

//4.0.9.507

// DeliveryEmulationStatus defines //4.0.5.24
#define DES_IDLE              0
#define	DES_START_LOCK        1
#define	DES_WAIT_FOR_LOCK     2
#define	DES_WAIT_TIMEOUT      3
#define	DES_REQUEST_TANK_READING   4 //4.0.15.501
#define	DES_WAIT_TANK_READING 5 //4.0.14.240  4
#define	DES_UNLOCK            6 //4.0.14.240 5

// DeliveryEmulationSingleTankStatus defines 4.0.21.500 - TD 66935
#define DES_SINGLE_TANK_IDLE				 0
#define	DES_SINGLE_TANK_START_LOCK			 1
#define	DES_SINGLE_TANK_WAIT_FOR_LOCK		 2
#define	DES_SINGLE_TANK_WAIT_TIMEOUT	     3
#define	DES_SINGLE_TANK_REQUEST_TANK_READING 4 
#define	DES_SINGLE_TANK_WAIT_TANK_READING	 5 
#define	DES_SINGLE_TANK_UNLOCK				 6 

// DeliveryEmulationSingleTankFinalStatus (DeliveryEmulationSingleTankSuccess) defines 4.0.21.500 - TD 66935
#define DESF_SINGLE_TANK_NOT_DONE			 0
#define	DESF_SINGLE_TANK_TIMEOUT    		 1
#define	DESF_SINGLE_TANK_FAILURE    	     2
#define	DESF_SINGLE_TANK_DONE    			 3

#define MAX_RECS_IN_OLA       10 

typedef struct 
{
	BYTE				sFileValid[sizeof(PUMP_INFO)];
	PUMP_INFO			p[MAX_PUMPS_64]; 	
	PAY_AT_PUMP_INFO	pap[MAX_PUMPS_64];//4.0.20.504 - was PAP_INFO_XML_BUFF   pap[MAX_PUMPS_64];
	SERVER_INFO			serverInfo;       //300
	SHIFT_REC			currentShift;     //52
}PUMP_FILE; 

//Pump file statuses
enum PumpFileState {
		FILE_OK,	   
		FILE_WAIT_FOR_LOAD,
		FILE_WAIT_FOR_SAVE	
};

class CPumpFile :	public 
					CRelativeFile 			
{
public:
	long VerifyFileSignatureAndData();
	BOOL IsQdexReady();  //4.0.5.30
	BOOL IsFileReady(BOOL* pbChangeToReady);
	long WidenOLA2ThreeRec();
	long Convert29VerTo30Ver();
	long CPumpFile::Convert30VerTo64Pumps();
	long Convert24VerTo29Ver();
	long SetFileSignature();
	long ValidityCheck();
	CPumpFile();
	virtual ~CPumpFile();

private:
	void ExpendedSessTypeAndPumps();
	long SaveAllData();
	long m_lPumpFileStatus;
};

// -----------------------------------
// PRE PAY Section...........
// -----------------------------------

typedef struct 
{	
	PRE_PAY_INFO	info;			// sizeof(PRE_PAY_INFO) =  47  //4.0.20.54 
	CPumpTransact	trs;			// sizeof(CPumpTransact) = 200	//4.0.22.501	TD 72716
	BYTE			byQdexFlg;		// sizeof(BYTE)			=   1
	BYTE			sFiller[264];   //4.0.20.54	//4.0.18.660 68689 4.0.21.502 //4.0.22.10 TD 71055 //4.0.22.501	TD 72716
}PRE_PAY_REC;						// sizeof(PRE_PAY_REC)	= 512	4.0.17.23 TD 38654	//4.0.18.502 //4.0.21.502 68689

typedef struct  
{
	eFileOperation opCode;
	PRE_PAY_REC prePayRec;
}PRE_PAY_QDX_MAINT_REC;

class CPrePayTrsFile :	public 
						CIndexFile					
{
public:
	CPrePayTrsFile();
	virtual ~CPrePayTrsFile() {}
};

// -----------------------------------
// PUMP TOTALS Section...........
// -----------------------------------
typedef struct 
{	
	TOTAL_INFO		info;		// sizeof(TOTAL_INFO)	= 119
	BYTE			byQdexFlg;	// sizeof(BYTE)			=   1
}TOTAL_REC;						// sizeof(TOTAL_REC)	= 120	

typedef struct 
{
	eFileOperation opCode;
	TOTAL_INFO		info;		// sizeof(TOTAL_INFO)	= 119
}TOTAL_QDX_MAINT_REC;						// sizeof(TOTAL_REC)	= 120	

typedef struct 
{
	eFileOperation opCode;
	CPumpTransact trs;
	PAY_AT_PUMP_INFO info;
}TOTAL_QDX_MAINT_TRS_XML_REC;


class CPumpTotalsFile :	public CIndexFile	
{
public:
	CPumpTotalsFile();
	virtual ~CPumpTotalsFile() { }
};


//4.0.11.503
typedef struct 
{	
	PAID_TRS		info;		// sizeof(PAID_TRS)	= 
	BYTE			byQdexFlg;	// sizeof(BYTE)			=   1
}PAID_TRS_REC;						// sizeof(TOTAL_REC)	= 	

typedef struct 
{	
	PAID_TRS_EX		info;		// sizeof(PAID_TRS)	= 
	BYTE			byQdexFlg;	// sizeof(BYTE)			=   1
}PAID_TRS_REC_EX;	//TD 38654					// sizeof(TOTAL_REC)	= 	

typedef struct  
{
	eFileOperation opCode;
	PAID_TRS_EX	rec;
}PAID_TRS_MAINT_REC; //33898

class CPumpPaidTrs :	public CIndexFile	
{
public:
	CPumpPaidTrs();
	virtual ~CPumpPaidTrs() {}


};


// -----------------------------------
// TANK READ Section...........
// -----------------------------------
typedef struct 
{	
	TANK_READ_INFO	info;		// sizeof(TANK_READ_INFO)	=  99
	BYTE			byQdexFlg;	// sizeof(BYTE)				=   1
}TANK_READ_REC;					// sizeof(TANK_READ_REC)	= 100	

typedef struct 
{
	eFileOperation		opCode;
	TANK_READ_INFO		info;		
}TANK_READ_QDX_MAINT_REC;							


// -----------------------------------
// FUEL DELIVERY Section...........
// -----------------------------------

typedef struct 
{	
	DELIVERY_INFO	info;		// sizeof(DELIVERY_INFO)	=  99
	BYTE			byQdexFlg;	// sizeof(BYTE)				=   1
}DELIVERY_REC;					// sizeof(DELIVERY_REC)		= 100	

typedef struct  
{
	eFileOperation opCode;
	DELIVERY_INFO info;
}DELIVERY_QDX_MAINT_REC;



class CFuelDeliveriesFile :	public 
						CIndexFile	
{
public:
	long Operation(eFileOperation OpCode, void *pInfo, BOOL bIsSaveByMaint = FALSE); //4.0.18.20 - CR 33898
	CFuelDeliveriesFile();
	virtual ~CFuelDeliveriesFile();

protected:
	long ParseBinFile(char * sFileName);

};

// -----------------------------------
// ALARM Section...........
// -----------------------------------

typedef struct 
{	
	ALARM_INFO	info;		// sizeof(ALARM_INFO)	=  99
	BYTE		byQdexFlg;	// sizeof(BYTE)			=   1
}ALARM_REC;				// sizeof(ALARM_REC)		= 100	

typedef struct  
{
	eFileOperation opCode;
	ALARM_INFO info;
}ALARM_QDX_MAINT_REC;

class CAlarmFile :	public 
						CIndexFile	
{
public:
	long Operation(eFileOperation OpCode, void *pInfo, BOOL bIsSaveByMaint = FALSE); //4.0.18.20 - CR 33898
	CAlarmFile();
	virtual ~CAlarmFile();

protected:
	long ParseBinFile(char * sFileName);

};

// -----------------------------------
// CAR WASH Section...........
// -----------------------------------

typedef struct 
{	
	CAR_WASH_INFO	info;		// sizeof(CAR_WASH_INFO)	=  99
	BYTE			byQdexFlg;	// sizeof(BYTE)				=   1
}CAR_WASH_REC;				// sizeof(CAR_WASH_REC)			= 100	

typedef struct  
{
	eFileOperation opCode;
	CAR_WASH_INFO info;
}CAR_WASH_QDX_MAINT_REC;

class CCarWashFile :	public 
						CIndexFile	
{
public:
	long Operation(eFileOperation OpCode, void *pInfo, BOOL bIsSaveByMaint = FALSE); //4.0.18.20 - CR 33898
	CCarWashFile();
	virtual ~CCarWashFile();

protected:
	long ParseBinFile(char * sFileName);

};


// -----------------------------------
// RECEIPT Section...........
// -----------------------------------


//-----------------------------
// Extra info file 
//------------------------------

// start 3.1.1.64
typedef struct 
{	
	EXTRA_INFO	info;		// sizeof(CAR_WASH_INFO)	=  599
	BYTE		byQdexFlg;	// sizeof(BYTE)				=  1

	void ClearRecord()
	{
		memset(&info, ' ', sizeof(EXTRA_INFO)); //4.0.24.170 136062
		memset(&info.lPumpNumber, 0, sizeof(info.lPumpNumber));
		memset(&info.lRefundAmount, 0, sizeof(info.lRefundAmount));
		memset(&info.lSalesTax, 0, sizeof(info.lSalesTax));
		memset(&info.lSeqNumber, 0, sizeof(info.lSeqNumber));
		memset(&info.lServiceType, 0, sizeof(info.lServiceType));
		memset(&info.lTransactionAmount, 0, sizeof(info.lTransactionAmount));
		memset(&info.lAuthAmt, 0, sizeof(info.lAuthAmt));
		memset(&info.lCarWashAmount, 0, sizeof(info.lCarWashAmount));
		memset(&info.lCarWashDiscount, 0, sizeof(info.lCarWashDiscount));
		memset(&info.lCarWashTax, 0, sizeof(info.lCarWashTax));
		memset(&info.lCashDepositTotal, 0, sizeof(info.lCashDepositTotal));
		memset(&info.lControllerSeqNum, 0, sizeof(info.lControllerSeqNum));
		memset(&info.lDevice, 0, sizeof(info.lDevice));
		memset(&info.lDiscount, 0, sizeof(info.lDiscount));
		memset(&info.lDiscountArray, 0, sizeof(info.lDiscountArray));
		memset(&info.lFlag, 0, sizeof(info.lFlag));
		memset(&info.lIndexNumber, 0, sizeof(info.lIndexNumber));
		memset(&info.lPreSetlimit, 0, sizeof(info.lPreSetlimit));
		// RFUEL-3708
		for (int i = 0; i < MAX_POSITIONS_PER_PUMP; i++)
		{
			memset(&info.GradeAmountLimit[i].lGradeLimit, 0, sizeof(info.GradeAmountLimit[i].lGradeLimit));
			memset(&info.GradeVolumeLimit[i].lGradeLimit, 0, sizeof(info.GradeVolumeLimit[i].lGradeLimit));
		}
		byQdexFlg = ' '; // RFUEL-3708
	}
}EXTRA_INFO_REC;			// sizeof(EXTRA_INFO_REC)	=  600
// end 3.1.1.64

typedef struct  
{
	eFileOperation opCode;
	EXTRA_INFO	info;
}EXTRA_INFO_QDX_MAINT_REC; //33898


//4.0.9.507
typedef struct 
{	
	EXTRA_INFO_	info;		// sizeof(CAR_WASH_INFO)	=  599
	BYTE		byQdexFlg;	// sizeof(BYTE)				=  1
}EXTRA_INFO_REC1;			// sizeof(EXTRA_INFO_REC)	=  600


typedef struct  
{
	eFileOperation opCode;
	ITEMS_INFO	info;
}ITEMS_INFO_QDX_MAINT_REC; //33898


class CItemsFile : public CIndexFile  
{
public:
	long ConstructItemsRecordToBAP( ITEMS_INFO &cItemInfo, BAP_ALL_DATA_STRUCTS &cTagItemsRec);  //4.0.5.27
	long SplitBAPRecorToItems(long lItemsIndex ,BAP_ALL_DATA_STRUCTS &cTagItemsRec , ITEMS_INFO &cItemInfo); //4.0.5.27
	long Operation(eFileOperation OpCode, void * pInfo, BOOL bIsSaveByMaint = FALSE); //4.0.18.20 - CR 33898

	CItemsFile();
	virtual ~CItemsFile();
protected:
	long ParseBinFile(char * sFileName);

};


class CAsh2000File : public CIndexFile  
{
public:	

	CAsh2000File();
	virtual ~CAsh2000File() {}
	long CAsh2000File::InsertData(long lPumpNumber, void *pData, long lSize, CPumpTransact  *pTrs);
};

typedef struct
{
	long lCounter;		//1 - 4
	long lTotalVale;    //5 - 8 
	long lTotalVolume;  //9 - 12  
}GradeAccumulateTotals;


typedef struct 
 {
	long lShiftNumber;			//	1	-	4	
	long lPosNumber;			//	5	-	8
	long lRecordStatus;			//	9	-	12 
	long lTransactionType;		//	13	-	16
	GradeAccumulateTotals   cArrayGradeAccumulateTotals[AST_MAX_GRADES];  // 17  - 220 -  12 * 17 = 204
	SYSTEMTIME				cTime;									 // 221 - 236 					
	BYTE					sUsed;  //'1' - used					//  237 - 237
	BYTE sFiller[274];												 // 238 - 511 	 
	BYTE byQdxflag;												     // 512 - 512 	 
}GradeAccumulateDetails;

//4.0.5.26
//4.0.5.39
typedef struct  {
	long	lIndex;				//1		-	 4 
	PS::RECEIPT_EXTRA_INFO_REC sRec;		//5     - 1023  
}RECEIPT_EXTRA_REC;


typedef struct  {
	BYTE	sFiler1019[1019];
}RECEIPT_EXTRA_INFO_TO_QDX;

typedef struct  {
	long	lIndex;				//1		-	 4 
	RECEIPT_EXTRA_INFO_TO_QDX sRec;		//5     - 1023  
	BYTE	sQdxFlag;			//1024  - 1024
}RECEIPT_EXTRA_TO_QDX;	


//4.0.9.507
typedef struct  {
	long	lIndex;											//1		-	 4 
	TravelCenterData   cArrTravelCenterData[MAX_PROMPTS];		//5     - 1023  
	TRAVEL_CENTER_TAX	cItemTax[2][5];
	BYTE	sFiller[1051];	//4.0.128.280 increase filler from 1019 to 2043
	//BYTE	sFiller[129];	//4.0.999.0
}TRAVEL_CENTER_REC;	

// class CReceiptExtraFile : public CIndexFile  
// {
// public:
// 	long Operation(eFileOperation OpCode, void * pInfo, BOOL bIsSaveByMaint = FALSE); //4.0.18.20 - TD 33898
// 	CReceiptExtraFile();
// 	virtual ~CReceiptExtraFile();
// 
// protected:
// 	long ParseBinFile(char * sFileName);
// 
// };

typedef struct _RECEIPT_EXTRA_INDEX_INFO
{
    BYTE  sGroup;			//	1  -  1
    BYTE  sKeyIndex[16];	//	2  - 17
	cIndex myIndex;			//  18 - 118
    BYTE  sFiller8[137];	//	119 - 255		
}RECEIPT_EXTRA_INDEX_INFO;	//4.0.21.20


typedef struct
{
	RECEIPT_EXTRA_INDEX_INFO info;
	BYTE		byQdexFlg;	
}RECEIPT_EXTRA_INDEX_REC;

typedef struct  
{
	eFileOperation opCode;
	RECEIPT_EXTRA_INDEX_INFO	info;
}RECEIPT_EXTRA_INDEX_QDX_MAINT_REC; //33898


class COLATmpTrsFile : public 
					CIndexFile 
{
public:
	COLATmpTrsFile();
	virtual ~COLATmpTrsFile();
};

typedef struct
{	
	BYTE  sKeyIndex[10];	//1 - 10 	//' 'TTTYYMMDD		

}GENERAL;

typedef struct
{	
	BYTE  byPeriod;
	BYTE  sTerminalID[3];
	BYTE  sDateYYMMDD[6];
}INFO;


//4.0.10.506
union CASH_ACCEPTOR_INDEX
{
	GENERAL cGeneral;
	INFO	cInfo;
};

//4.0.10.506
typedef struct
{
	union CASH_ACCEPTOR_INDEX cIndex;			
	long  lTerminalId;		//11- 14
	long  lCounter;			//15- 18
	long  lAmount;			//19 -22	
	long  lRemovalCounter;	//23- 26
	BYTE  byDrawerRemovalStatus; //27-27	
	BYTE  sFiler[996];		//28-
	BYTE  QdxFlags;					
}CASH_ACCEPTOR_REC;

typedef struct  
{
	eFileOperation opCode;
	CASH_ACCEPTOR_REC cashAccRec;
}CASH_ACCEPTOR_QDX_MAINT_REC;

typedef struct
{
	union CASH_ACCEPTOR_INDEX cIndex;	
	long  lTerminalId;		//11- 14
	BYTE  sValid;	//- indicate that header in valid		
	TAG_SYSTEMTIME	sDateAndTime;
	TAG_SYSTEMTIME	sEndDateAndTime; //4.0.18.504 Merge from 4.0.17.220
	BYTE  sFiler[994];
	BYTE  QdxFlags;					
}CASH_ACCEPTOR_HEADER_REC;


//4.0.25.50 - TD 108311
typedef struct 
{
	long             lShiftAccumulatedDeliveryVolume;
	long             lShiftAccumulatedSales;
	BYTE			 sFiler[92];

}SINGLE_SHIFT_ACCUM_TANK_RECORD;

typedef struct 
{
	long             lShiftAccumulatedSales;
	long             lShiftAccumulatedPumpTest;
	BYTE			 sFiler[92];
	
}SINGLE_SHIFT_ACCUM_PUMP_POSITION_RECORD;


//5.0.0.55
//#pragma pack()



#endif // !defined(AFX_PUMPSRVDB_H__DD8F77B3_9301_11D1_9F7A_004005615A06__INCLUDED_)
