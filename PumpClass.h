//-----------------------------------------------------------------------
// File Name   :	PumpClass.h
// 
// Description :	Basic Definitions for Pump Control Module
//					
// Written by  :	Moti Lichi 10.04.97
// 
//-----------------------------------------------------------------------


#ifndef __PUMP_CLASS_H__
#define __PUMP_CLASS_H__


#include "pump.h" 
#include "ola.h" 
#include "GeneralStruct.h"	//4.0.9.500
#include "XMLInterface.h"
#include "PumpSrvStructures.h"

using namespace PS;
#define ASH_2000_BUFFER_SIZE			1019

#define COLORLESS	0
#define YELLOW	1
#define RED		2
#define GREY	3
#define BLUE	4
#define PURPLE  5	//4.0.22.508 75592
#define GREEN	6
#define BLACK	7	//4.0.26.505	157073
#define TURQUOISE	8	//4.0.26.505	157073
#define PINK		9


//4.0.3.27
typedef enum PumpLockType
{
	LOCK_TYPE_MAINT,		//1
	LOCK_TYPE_EMERGENCY		//2
} PumpLockType;

//4.0.5.39
enum PumpLoginOp
{
	PUMP_NO_LOGIN = 0,
	PUMP_LOGIN = 1,
	PUMP_LOGOUT = 2,
	PUMP_WAIT_TO_BE_LOGIN = 3,
	PUMP_WAIT_TO_BE_LOGOUT = 4
};


//  4.0.28.500 TD 256429
enum TerminalCmdCode
{
	TERMINAL_CMD_UNKNOWN = 0,
	TERMINAL_CMD_LOGIN = 1,
};

typedef struct 
{
	TAG_SYSTEMTIME	sTime;
	BYTE			sNull;	
}SZ_TAG_SYSTEMTIME;


typedef struct 
{
	LONG			lIndexNumber;	// key		
	LONG			lPump;
	short			nNozzle;
	short 			nGrade;
	short			nFlags;
	char			sVolume[MAX_NAME_LENGTH];
	char			sValueA[MAX_NAME_LENGTH];
	char			sValueB[MAX_NAME_LENGTH];	
	DWORD			dwActiveReadNumber;
	DWORD			dwShiftNumber;
	SYSTEMTIME		cSysTime;
	BYTE			byPumpStatus;
	BYTE			byNozzleStatus;
	LONG			lShiftAccumulatedPumpTest; //4.0.25.50 - TD 108311
	LONG			lShiftAccumulatedSales; //4.0.25.50 - TD 108311
	BYTE			spare[11]; //4.0.25.50 - TD 108311
}TOTAL_INFO;

//4.0.11.503
typedef struct 
{
	BYTE				sIndex[15];// key - Pump Number + SysTime		
	BYTE				bOnOffer;//to tell us this trs already taken
	TAG_PUMP_TRANSACT2  cTagPump2;
	long				lServiceMode; //4.0.14.504
	BYTE				sFiler[357];
	//BYTE				sFiler[361];
}PAID_TRS;




//Add by Yaron
typedef struct 
{
	BYTE			byTankValid[STR_LONG_VAL5];
	BYTE			byTankNumber[STR_LONG_VAL5];
	BYTE			byTankCapacity[STR_LONG_VAL10];
	BYTE			byProductName[STR_LONG_VAL10];
	BYTE			byProduct[STR_LONG_VAL10];
	BYTE			byStatusGeneral[STR_LONG_VAL10];	 // tank status

	BYTE			nFlags[STR_LONG_VAL5];    // the flags indicate which of next the fields are valid.
	BYTE			byFuelLevel[STR_LONG_VAL10];
	BYTE			byWaterLevel[STR_LONG_VAL10];
	BYTE			byTemperature[STR_LONG_VAL10];
	BYTE			byFuelVolume[STR_LONG_VAL10];
	BYTE			byWaterVolume[STR_LONG_VAL10];
	BYTE			byUllage[STR_LONG_VAL10];
	BYTE			byTCVolume[STR_LONG_VAL10];
	BYTE			byShiftNumber[STR_LONG_VAL10];
	TAG_SYSTEMTIME	sDateTime;
	BYTE			byStatusSpecial[STR_LONG_VAL10]; //For Meggitt that sends 2 field of status
	BYTE			byManifold[STR_LONG_VAL10];
	BYTE			byVolumeFactor[5]; // 4.0.1.32
	BYTE            byTankLowLevelLimit[STR_LONG_VAL10];
	BYTE            byTankHighLevelLimit[STR_LONG_VAL10];
	BYTE			byTankAlarmFromAlarmSrv; //4.0.2.30  //4.0.2.37
	BYTE			byLevelFactor [5] ; //4.0.3.36
	BYTE			byTemperatureFactor [5] ; //4.0.3.51
	BYTE			spare[14]; //4.0.2.30 //4.0.2.37  //4.0.3.36//4.0.3.51
	//BYTE			spare[19]; //4.0.2.30 //4.0.2.37  //4.0.3.36 //4.0.3.51
	//BYTE			spare[24]; //4.0.2.30 //4.0.2.37  //4.0.3.36

	//BYTE			spare[25]; //4.0.2.30 4.0.2.37
	//BYTE			spare[45]; //4.0.1.32
	//BYTE			spare[50]; 4.0.1.32
}TANK_READ_MEMO;



// start 3.1.1.64
typedef struct    // 3.1.1.5
{
	LONG			lIndexNumber;						//   1 -   4  // equel to Transaction number.
	BYTE			sLoyalId[STR_20];					//   5 -  24 
	LONG			lDiscountArray[MAX_GRADES];			//  24 -  88
// start 3.1.1.66
	BYTE			sPromotionNumber[STR_LONG_VAL10];	//  89 -  98
	LONG			lFlag;								//  99 - 102
	BYTE			sAttendantID[STR_LONG_VAL10];		// 103 - 112				3.2.0.76 
	BYTE			sAttendantName[STR_15];				// 113 - 127				3.2.0.76 
	BYTE			sFullVehicleId[FULL_VEHICLE_ID];	// 128 - 143 				3.2.0.76
	BYTE			byCashDeposit1;						// 144 - 144
	BYTE			byCashDeposit2;						// 145 - 145
	BYTE			byCashDeposit5;						// 146 - 146
	BYTE			byCashDeposit10;					// 147 - 147 
	BYTE			byCashDeposit20;					// 147 - 147 
	BYTE			byCashDeposit50;					// 148 - 148
	BYTE			byCashDeposit100;					// 149 - 149
	long			lCashDepositTotal;					// 150 - 153	//99,999.00
	BYTE			sInvoiceNumber[7];					// 154 - 160	4.0.1.38		
	BYTE			sTransactionDateTime[10];			// 161 - 170	//YYMMDDHHMM  //4.0.1.38
	long			lTransactionAmount;					// 171 - 174	//4.0.1.38
	long			lRefundAmount;						// 175 - 178	//4.0.1.38
	BYTE			sCarWashProgram;					// 179 - 179	//4.0.1.38
	BYTE			sWashCode[6];						// 180 - 183 	//4.0.1.38	//4.0.21.501 - TD 70634
	BYTE		    sWashExpire[6];						// 184 - 189	//4.0.1.38		
	long		    lCarWashTax;						// 190 - 193	//4.0.1.38
	long			lCarWashAmount;						// 194 - 197	//4.0.1.38		
	long			lServiceType;						// 198 - 201	//4.0.1.38
	long			lSalesTax;							// 202 - 205	//4.0.1.38		
	long			lDiscount;							// 206 - 209	//4.0.1.38
	BYTE			sBarCode[40];						// 210 - 249	//4.0.1.38
	long			lPumpNumber;						// 250 - 253
	long			lSeqNumber;							// 254 - 257
	long			lDevice;							// 258 - 261
  	BYTE			sAccountNumber[21];					// 262 - 283 4.0.2.21
	BYTE			sAuthNumber[12];					// 284 - 295  4.0.2.21
	long			lAuthAmt;							// 296 - 299 4.0.2.21
	BYTE			sTrack2Data	 [TRACK_2_DATA_SIZE];   // 300 - 336   //101- 137 Generated at the S.C side and return as is // Track 2 Data
	BYTE			sTrack1Data	[TRACK_1_DATA_SIZE];    // 337 - 412 Generated at the S.C side and return as is // Track 1 Data	
	BYTE			sCustomerName[20];					// 413 - 432  // 4.0.5.26
	BYTE			sTotalBalance[8];					// 433 - 440	// 4.0.5.26
	BYTE			sBalanceType;   					// 441 - 441 // 4.0.5.26
	BYTE			sHostTransId[8];					// 442 - 449 
	long			lControllerSeqNum;					// 450 - 453//4.0.5.34	
	BYTE			sLAMFlags;							// 454 - 454 ' ' -  regular flow , 1 - LAM Failed  2- LAM succeeded  	
	BYTE			sLAM[25];							// 455 - 479 4.0.5.44
	BYTE			sDiscountType[2];					// 480 - 481 4.0.6.502	
	BYTE			sClearanceID;						//4.0.6.506
	BYTE			sCardName[8];						//4.0.6.506
	BYTE			sOLDSession[6];						//4.0.6.506
	BYTE			sMediaMasterType[2];				//4.0.6.506	
	BYTE			sExpire[4];							//4.0.6.507			
	BYTE			sReferenceNumber[12];				//4.0.8.507
	BYTE			sCustomerID[10];					//4.0.8.507
	BYTE			sGRDiscountType;					//4.0.9.500			
	BYTE			sGRDiscountPerGallon[5];			//4.0.9.500
	BYTE			sPMTReferenceNumber[10];			//4.0.9.503
	BYTE			sManagerID[10];			//4.0.9.506
	BYTE			sClFlags[2];						//4.0.9.508
	BYTE			sServiceFee[5];						//4.0.10.20
	BYTE			sIgnoreServiceFee;					////4.0.10.504
	BYTE			sPayInCreate;						//4.0.10.504
	BYTE			sDryItemsRestriction;				//4.0.10.504	
	long			lCarWashDiscount;				    //4.0.11.504
	long			lPreSetlimit;					//4.0.11.200
	BYTE			sPreSetLimitType;				//4.0.11.200					
	BYTE			sNonDiscountTotalAmount[10];		//4.0.15.10
	BYTE			sOriginalUnitPrice[5];				// Liat LPE	
	DISCOUNT_ATTRIBUTES	discountAttrsInfo;				//4.0.17.501 - CR 38162	//28 //4.0.18.502
	BYTE			sDPTProductCode[2];					//4.0.21.500 - TD 14778
	BYTE			sLoyaltyVolumeLimit[7];				//4.0.20.450 66977	//4.0.21.501
	BYTE			sDeferralAccepted;					//44.0.22.504 69344
	DISCOUNT_ATTRIBUTES_EXT	sDiscountAttrExt;//8		//4.0.22.509 38162
	BYTE			sTranType;							//4.0.22.511 - TD 74796
	BYTE			sLoyaltyApplicable;					//4.0.22.511 - TD 74796
	BYTE			sANPRRefNumber[STR_LONG_VAL5];		//4.0.23.500 - TD 69914
	BYTE			sPreDispensingVolumeLow[5];			//4.0.23.509 97360
	BYTE			sPreDispensingVolumeHigh[15];		//4.0.23.509 97360
	BYTE			sPostDispensingVolumeLow[5];		//4.0.23.509 97360
	BYTE			sPostDispensingVolumeHigh[15];		//4.0.23.509 97360
	BYTE            sGRTotalDiscountCarWash[10];		//4.0.24.60  105269 - The total discount amount as a result of the special CarWash cents off on the fuel.
	BYTE			sGRTotalDiscountFuelItem[10];		//4.0.24.60  105269 - The total discount amount as a result of the OLA cents off on the fuel.
	GRADE_AMOUNT_LIMITS	GradeAmountLimit[MAX_POSITIONS_PER_PUMP];				//4.0.23.270 111746 //4.0.24.60
	BYTE			sANPRTrsStatus[STR_LONG_VAL2];	//4.0.26.505 157073
	TAG_SYSTEMTIME	cVRNDateTime;		//4.0.26.505 157073
	BYTE			sTrsFlowRate[4];					//4.0.27.40 TD 165585
	BYTE			sAverageFlowRate[4];				//4.0.27.40 TD 165585
	BYTE			sRatesFromDevice;					//4.0.27.40 TD 165585
	GRADE_AMOUNT_LIMITS GradeVolumeLimit[MAX_POSITIONS_PER_PUMP];
	BYTE			sVAT_RateMSB;						//4.0.31.500  TD 404911
	BYTE			sLockingKey[40];
	BYTE			sStoreIDField[24];
	BYTE			sPosId[10];
	BYTE			sTransId[10];
	BYTE			sLoyaltyStartDateTime[19];
	BYTE			sTicketId[10];
	BYTE			sClubType[2];						//  '01' - Punchh
	BYTE			sPromotionID[STR_15];				//	RFUEL-155
	BYTE			sMaxDiscountAllowed[5]; 	//RFUEL-3309 Get the maximal discount allowed for the grade  used when calling  CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_EXTRA_INFO* StructIn) /retrieves ExtraInfo1
	BYTE			spare[67];							//4.0.23.500 - TD 69914 //4.0.23.509 97360 //4.0.24.60 //4.0.27.40 TD 165585

	void ClearRecord()
	{
		lPumpNumber = 0;
		lRefundAmount = 0;
		lSalesTax = 0;
		lDevice = 0; // RFUEL-3708
		lPreSetlimit = 0;
		lIndexNumber = 0;
		lFlag = 0;
		lDiscount = 0;
		lControllerSeqNum = 0;
		lCashDepositTotal = 0;
		lCarWashTax = 0;
		lCarWashDiscount = 0;
		lCarWashAmount = 0;
		lAuthAmt = 0;
		lTransactionAmount = 0;
		lServiceType = 0;
		lSeqNumber = 0;
		memset(lDiscountArray, 0, sizeof(lDiscountArray));
		// RFUEL-3708
		for (int i = 0; i < MAX_POSITIONS_PER_PUMP; i++)
		{
			GradeAmountLimit[i].lGradeLimit = 0;
			GradeVolumeLimit[i].lGradeLimit = 0;
		}
	}
}EXTRA_INFO; //1023 //4.0.26.505 157073 Should be 1023 because Extra_Info_Rec has 1 more byte for 1024


//4.0.9.507

typedef struct
{
	BYTE sGrade[3];
	BYTE  sLevel[3];
	BYTE sGradePrices[10];		
	BYTE  sDiscount[6];
	BYTE  sMaxValue[10];
	BYTE  sFlags[10];	
	BYTE  sTotalAmount[10];  //4.0.11.503
	BYTE sFiller[30];  //4.0.9999.0
	//BYTE sFiller[40];  //4.0.9999.0 4.0.11.503
}TAG_PRE_AUTH_TRS;

typedef struct
{
	long	lGrade;
	long	lLevel;
	long	lGradePrices;		
	long	lDiscount;
	long	lMaxValue;
	long	lFlags;		//4.0.9999.0
	long	lTotalAmount;		//4.0.11.503
}PREPAY_AUTH_TRS;


typedef struct
{
	TAG_PRE_AUTH_TRS   cItem1;   // diesel 
	TAG_PRE_AUTH_TRS   cItem2;   //reefer
	char sFiller[80] ; 
	//char sFiller[100] ; 
}TAG_PRE_AUTH_TRS_DETAILS;


typedef struct
{
	long	lGrade;
	long	lLevel;
	long	lGradePrice;
	long	lDiscount;
	long	lValue;
	long	lVolume;	
}PREPAY_SOLD_TRS;


typedef struct
{
	BYTE  sGrade[3];		//1  - 3
	BYTE  sLevel[3];	//4	-  6	
	BYTE  sGradePrice[10]; //7	16
	BYTE  sDiscount[6];  //17 - 22
	BYTE  sValue[10];    //23 - 33 
	BYTE  sVolume[10];   //34 - 43
	BYTE sFiller[50];   
}TAG_PRE_SOLD_TRS;

typedef struct
{
	TAG_PRE_SOLD_TRS   sFirstSoldItem;
	TAG_PRE_SOLD_TRS   sSecondSoldItem;
	char sFiller[100];
}TAG_PRE_ALL_SOLD_TRS;




typedef struct    // 3.1.1.5
{
	long				lIndexNumber;						//   1 -   4  // equel to Transaction number.
	char				sFiller[94];
	long				lFlag;	
	PREPAY_AUTH_TRS		cItem1;
	PREPAY_AUTH_TRS		cItem2;
	PREPAY_SOLD_TRS		cItemSold1;
	PREPAY_SOLD_TRS		cItemSold2;
	BYTE				byItemRestriction;						//merge from 4.0.8190.0 - 4.0.22.10 TD 71055
	BYTE				sFormatedLine[MAX_FORMATED_POS_LINE];   //merge from 4.0.8190.0 - 4.0.22.10 TD 71055
	BYTE				sNotAllowAddItem;						//merge from 4.0.8190.0 - 4.0.22.10 TD 71055
	char				sFiller02[342];							//merge from 4.0.8190.0 - 4.0.22.10 TD 71055
	
}DIESEL_INFO;


typedef struct    // 3.1.1.5
{
	TAG_PRE_AUTH_TRS		cItem1;
	TAG_PRE_AUTH_TRS		cItem2;
	TAG_PRE_SOLD_TRS		cItemSold1;
	TAG_PRE_SOLD_TRS		cItemSold2;
	BYTE	sFiller[200];	
}TAG_DIESEL_INFO;

typedef struct    // 3.1.1.5   //4.0.9999.0
{
	TAG_PRE_SOLD_TRS		cItemSold1;
	TAG_PRE_SOLD_TRS		cItemSold2;
	BYTE	sFiller[200];	
}TAG_DIESEL_SOLD_INFO;


//4.0.9.507


#define MAX_CONN_SUPPORTED_POS	7
typedef struct 
{
	BYTE		sPOSNumber		[4];
	BYTE		sConnStatus		[2];
} POS_INFO;

//4.0.5.26 start
typedef struct 
{
	BYTE		sPStoCL			[STR_LONG_VAL2];	
	BYTE		sCLtoPS			[STR_LONG_VAL2];
	BYTE		sPStoQDEX		[STR_LONG_VAL2];
	BYTE		sPStoOLA		[STR_LONG_VAL2];
	BYTE		sPStoAS			[STR_LONG_VAL2];
	BYTE		sCLtoFC			[STR_LONG_VAL2];
	BYTE		sCLtoGEN		[STR_LONG_VAL2];
	BYTE		sGENtoCL		[STR_LONG_VAL2];
	BYTE		sCLtoBAP		[STR_LONG_VAL2];
	BYTE		sBAPtoCL		[STR_LONG_VAL2];
	BYTE		sCLtoTS			[STR_LONG_VAL2];
	BYTE		sTStoCL			[STR_LONG_VAL2];
	BYTE		sBAPtoPMT		[STR_LONG_VAL2];	
	BYTE		sPMTtoBAP		[STR_LONG_VAL2];	
	BYTE		sGENtoXML		[STR_LONG_VAL2];
	BYTE		sRFStoQDEX		[STR_LONG_VAL2];
	BYTE		sTStoTG			[STR_LONG_VAL2];  //4.0.5.39
	BYTE		sPMTtoBO		[STR_LONG_VAL2];	
	BYTE		sPMTtoOLA		[STR_LONG_VAL2];
	POS_INFO	cPosInfo		[MAX_CONN_SUPPORTED_POS];
}PS_TAG_CONNECTIVITY_INFO;


///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Pump Status 
typedef struct 
{
	BYTE			sMode			[STR_LONG_CODE];//PumpMode			lMode;
	BYTE			sRes			[STR_LONG_CODE];//PumpReservation		lRes;
	BYTE			sResExt			[STR_LONG_CODE];//LONG				lResExt;
	BYTE			sStatus			[STR_LONG_CODE];//PumpStatus			lStatus;
	BYTE			sDevice			[STR_LONG_CODE];//PumpDevice			lDevice;  // last device report error/warning
	BYTE			sWarning		[STR_LONG_VAL5];//LONG				lWarning; // warning code
	BYTE			sError			[STR_LONG_VAL5];//LONG				lError;	  // error code
	BYTE			sMisc			[STR_LONG_VAL5];//LONG				lMisc;	  // additional data (error/code)
}TAG_PUMP_STATUS;


typedef struct 
{
protected:
   	long			m_lMode;		//0
	PumpReservation	m_lRes;			//4
	PumpResExt		m_lResExt;		//8
	PumpStatus		m_lStatus;		//12
	PumpDevice		m_lDevice;		//16
	LONG			m_lWarning;		//20
	LONG			m_lError;		//24
	LONG			m_lMisc;		//28
	LONG			m_lServiceMode;	//32
	LONG			m_lMessage;		//36
	CTime			m_cTime;		//40	//4.0.17.20 - TD 26384
	SHORT			m_nGrade;		//44	//4.0.19.500 - TD 59171
	SHORT			m_nPosition;	//46	//4.0.19.500 - TD 59171
	LONG			m_lMiscExt;		//40	//4.0.12.500	69203
	
}PUMP_STATUS;//Total size - 48

#define NULL_TRS_NUM 0 // 256101

/////////////////////////////////////////////////////////////////////////////
// CPumpStatus

class CPumpStatus :		public 
						PUMP_STATUS,			
						CObject 
{


public:
	void SetResExt(PumpResExt lResExt);
	BOOL operator !=( const CPumpStatus& pumpStat );
	

	CPumpStatus();
	CPumpStatus(const CPumpStatus & pumpStat); // copy constructor

	CTime			GetTime();		//4.0.17.20 - TD 26384
	void			UpdateTime();	//4.0.17.20 - TD 26384

	PumpReservation	GetRes();
	PumpStatus		GetStatus();
	PumpResExt		GetResExt();

	void			operator =( TAG_PUMP_STATUS & tagPumpStat );
	void			operator =( TAG_PUMP_STATUS2 & tagPumpStat );
	long			GetMode();
	long			GetServiceMode() const;
	PumpDevice		GetDevice();
	LONG			GetWarning();
	LONG			GetError();
	void			GetTag(void *pBuff,long lVersion = 1); 
// Server capabilities

	void			SetRes(PumpReservation res, PumpResExt lResExt);
	void			SetStatus(PumpStatus lStatus);
	void			operator =( const CPumpStatus &  pumpStat );
	void			SetMode(long lMode);
	void			SetServiceMode(long lServiceMode);
	void			SetDeviceError(long lError, PumpDevice lDevice);
	void			SetError(long lError);
	void			SetDeviceWarning(long lWarning, PumpDevice lDevice);
	void			SetWarning(long lWarning);
	void			SetMisc(long lMisc);
	void			SetDevice(PumpDevice lDevice);
	long			GetMessage();
	void			SetMessage(long lMessage);
	long			GetMisc();
	void			SetMiscExt(long lMisc); //4.0.21.500	69203
	long			GetMiscExt();			//4.0.21.500	69203
	void			AddMiscExt(long lMiscExt);		//  4.0.23.1800
	void			RemoveMiscExt(long lMiscExt);	//  4.0.23.1800


	//4.0.19.500 - TD 59171
	void			SetGrade(short nGrade);
	long			GetGrade();
	void			SetPosition(short nPosition);
	long			GetPosition();
};



/////////////////////////////////////////////////////////////////////////////
// Fuel Transaction


typedef struct 
{
	BYTE			sNumber				[STR_LONG_VAL5];//LONG			lNumber;
	BYTE			sStatus				[STR_LONG_CODE];//TrsStatus		lStatus;
	BYTE			sOnOffer			[STR_LONG_CODE];//LONG			lOnOffer;
	BYTE			sRawVolume			[STR_LONG_VAL10];//LONG			lRawVolume;
	BYTE			sRoundedVolume		[STR_LONG_VAL10];//LONG			lRoundedVolume;
	BYTE			sRoundedValue		[STR_LONG_VAL10];//LONG			lRoundedValue;
	BYTE			sGradePrice			[STR_LONG_VAL10];//LONG			lGradePrice;
	BYTE			sNozzle				[STR_LONG_CODE];//SHORT			nNozzle;
	BYTE			sGrade				[STR_LONG_CODE];//SHORT			nGrade;
	BYTE			sLevel				[STR_LONG_CODE];//SHORT			nLevel;
	BYTE			sPump				[STR_LONG_CODE];//LONG			lPump;
	BYTE			sMode				[STR_LONG_CODE];//PumpMode		lMode;
	BYTE			sRes				[STR_LONG_CODE];//PumpReservation	lRes;
	BYTE			sResExt				[STR_LONG_VAL5];//LONG			lResExt;
	BYTE			sLinkNumber			[STR_LONG_VAL10]; // POS ticket Link (PrePay), OLA Session Link (PAP)
	BYTE			sLinkFlags			[STR_LONG_CODE];//SHORT			nLinkFlags;
	BYTE			sActiveReadNumber	[STR_LONG_VAL5]; 
	BYTE			sActiveShiftNumber	[STR_LONG_VAL5]; 
	BYTE			sMisc				[STR_LONG_VAL5]; 
}TAG_PUMP_TRANSACT;


typedef struct 
{
	BYTE			sNumber				[STR_LONG_VAL5];//LONG			lNumber;
	BYTE			sStatus				[STR_LONG_CODE];//TrsStatus		lStatus;
	BYTE			sOnOffer			[STR_LONG_CODE];//LONG			lOnOffer;
	BYTE			sRawVolume			[STR_LONG_VAL10];//LONG			lRawVolume;
	BYTE			sRoundedVolume		[STR_LONG_VAL10];//LONG			lRoundedVolume;
	BYTE			sRoundedValue		[STR_LONG_VAL10];//LONG			lRoundedValue;
	BYTE			sGradePrice			[STR_LONG_VAL10];//LONG			lGradePrice;
	BYTE			sNozzle				[STR_LONG_CODE];//SHORT			nNozzle;
	BYTE			sGrade				[STR_LONG_CODE];//SHORT			nGrade;
	BYTE			sLevel				[STR_LONG_CODE];//SHORT			nLevel;
	BYTE			sPump				[STR_LONG_CODE];//LONG			lPump;
	BYTE			sMode				[STR_LONG_CODE];//PumpMode		lMode;
	BYTE			sRes				[STR_LONG_CODE];//PumpReservation	lRes;
	BYTE			sResExt				[STR_LONG_VAL5];//LONG			lResExt;
	BYTE			sLinkNumber			[STR_LONG_VAL10]; // POS ticket Link (PrePay), OLA Session Link (PAP)
	BYTE			sLinkFlags			[STR_LONG_CODE];//SHORT			nLinkFlags;
	BYTE			sActiveReadNumber	[STR_LONG_VAL5]; 
	BYTE			sActiveShiftNumber	[STR_LONG_VAL5]; 
	BYTE			sMisc				[STR_LONG_VAL5]; 
	TAG_SYSTEMTIME	sDateTime;
	BYTE			sPrePaidValue		[STR_LONG_VAL10]; 
	BYTE			sMiscH				[STR_LONG_VAL5]; 
	BYTE			sServiceMode		[STR_LONG_VAL2]; // 
	BYTE			sOffer2Pos;			//4.0.9999.0
	BYTE			sGrade2				[STR_LONG_VAL2]; //4.0.9.507 
	BYTE			sRewardsTotal		[STR_LONG_VAL10];   //4.0.3.62
	BYTE			sPreSetLimit[7];					//4.0.15.500
	BYTE			sPreSetLimitType;					//4.0.15.500
	TAG_SYSTEMTIME  sStartDateTime;						//38654 cnt.
	BYTE			sDiscountPerUnit[STR_LONG_VAL5];
	BYTE			sLoyaltyAutoArm;
	BYTE			sFiller[2];							//38654 cnt.
}TAG_PUMP_TRANSACT3;

// 4.0.5.44
typedef struct 
{
	TAG_PUMP_TRANSACT2  cTagPumpTransact2;
	BYTE				sLAMFlags;    //0 - no lam valid  1-lam valid. 
	BYTE				sFiller[50];
}TAG_POST_VOID;

typedef struct 
{
	TAG_PUMP_TRANSACT5  cTagPumpTransact;
	BYTE				sLAMFlags;    //0 - no lam valid  1-lam valid. 
	BYTE				sPreDispensingVolumeLow[5];		//4.0.25.30 - TD 69655
    BYTE				sPreDispensingVolumeHigh[15];	//4.0.25.30 - TD 69655
	BYTE				sPostDispensingVolumeLow[5];	//4.0.25.30 - TD 69655
	BYTE				sPostDispensingVolumeHigh[15];	//4.0.25.30 - TD 69655
	BYTE				sFiller[50];
}TAG_POST_VOID_EX;	//TD 38654


typedef struct 
{
	long  lMinUnitForDiscount ;
	long  lMaxUnitForDiscount ;
	long  lDiscountPerUnit;	
	long  lPrice ;
	long  lTaxRate ;
	long  lUnitDiscountType ;
	short nNozzle;
	short nLevel ;
	long  lLoyaltyApplicable;

}GENERAL_NEW_PRICE_INFO;

typedef struct 
{		
	BYTE			sNozzle				[STR_LONG_CODE];//SHORT			nNozzle;
	BYTE			sLevel				[STR_LONG_CODE];//SHORT			nLevel;	
	BYTE			sPrice			    [STR_LONG_VAL10];//LONG			    lPrice
	TAG_SYSTEMTIME  sTimeValid;
	BYTE			sTaxRate            [STR_LONG_VAL10];//LONG			lTax
	BYTE			sDiscountPerUnit    [STR_LONG_VAL10];//LONG			lTax
	BYTE			sMaxUnitForDiscount [STR_LONG_VAL10];//LONG			lTax
	BYTE			sMinUnitForDiscount [STR_LONG_VAL10];//LONG			lTax
	BYTE			sUnitDiscountType   [STR_LONG_VAL5];//LONG			lTax
	BYTE			sLoyaltyApplicable[STR_LONG_VAL6];
	BYTE			sFiler[30];
}TAG_PRICE_CHANGE;



/////////////////////////////////////////////////////////////////////////////
// CPumpTransact

class CPumpTransact : public PumpTransactBase, public CObject 
{
public:
	GPIRetCode SetStatePaid(long lRequestSource, long lFlags = 0, BSTR bstrData = NULL); //4.0.22.501 - TD 69904
	GPIRetCode SetStateAccepted(long lRequestSource);
	GPIRetCode SetStateForcedRelease(long lRequestSource);
	GPIRetCode SetStateRelease(long lRequestSource);
	GPIRetCode SetStateOnOffer(long lRequestSource, long lFlags = 0); //4.0.23.508 - TD 89646


	void UpdateTime();
	void UpdateStartTime();	//TD 40492, 38654
	void UpdateStartFuelingTime(); //4.0.22.511 72606
	CTime GetTime(); 
	CTime GetStartTime();	//TD 40492, 38654
	CTime GetStartFuelingTime();	//4.0.22.511 72606
	inline void  SetTime(SYSTEMTIME & sTime){m_cTime = sTime;};
	inline void  SetStartTime(SYSTEMTIME & sTime){m_cStartTime = sTime;};//TD 40492, 38654
	inline void  SetStartFuelingTime(SYSTEMTIME & sTime){m_cStartFuelingTime = sTime;};//4.0.22.511 72606
	long GetLinkNumber();
	short GetLinkFlags();
	void SetLink(short nLinkFlags, long lLinkNumber);

	CPumpTransact();
	CPumpTransact(long lPumpNumber);
	CPumpTransact(const CPumpTransact& pumpTrs);
	CPumpTransact(const PumpTransactBase& pumpTrs);

	void		operator =( TAG_PUMP_TRANSACT& tagPumpTrans);
	TrsStatus	GetStatus();
	PumpReservation GetRes();//4.0.9.506 //amram
	void		GetTag(void *pBuff,long lVersion = 1, long lPreSet = 0 , long lGrade = 0,long lGrade2 = 0, __int64 lPurchaseID = 0, long lPreSetType = 0); //4.0.9.507 , 4.0.20.54	//4.0.22.503	72394
	LONG		GetOnOffer();
	BOOL		TrsShouldBeReportedPaid(); //4.0.23.650 133966 //4.0.25.50

	CPumpTransact(long lTrsNumber,long lActiveShiftNumber); // started transaction
	void		Clear();
	void		operator =( const CPumpTransact& pumpTrs);	
	void		SetStatus(TrsStatus lStatus);
	void		UpdateRolling(LONG newVolume, LONG newValue);	
	void		SetOnOffer(LONG lOnOffer);

	void		Convert2PumpTrsStruct(TAG_PUMP_TRANSACT2* cTagPumpTrs); //4.0.14.500
	void		ConvertFromPumpTrsStruct(TAG_PUMP_TRANSACT2* cTagPumpTrs); //4.0.14.500
	void		Convert2PumpTrsStruct(TAG_PUMP_TRANSACT5* cTagPumpTrs); //TD 38654
	void		ConvertFromPumpTrsStruct(TAG_PUMP_TRANSACT5* cTagPumpTrs); //TD 38654

	SHORT       GetAutoReconcileTimeout() const { return m_nAutoReconcileTimeout; } // 4.0.23.1050 TD 233242
	void		SetAutoReconcileTimeout(SHORT nTrsAuthorizeTimeout) { m_nAutoReconcileTimeout = nTrsAuthorizeTimeout; } // 4.0.23.1050 TD 233242

	void		setPumpTestOrCalib(BYTE pumpTestVal) { m_sPumpTest = pumpTestVal; } // 4.0.27.60	TD 231472 
	BOOL		IsTrsPumpTestOrCalib(){ return (m_sPumpTest != '0'); }   //4.0.25.1140 TD 412765

};


/////////////////////////////////////////////////////////////////////////////
// 


typedef struct 
{
	BYTE				sPumpMenufacture	[MAX_NAME_LENGTH];//unsigned char   szPumpMeufacture[MAX_NAME_LENGTH];
	BYTE				sNumber				[STR_LONG_CODE];//LONG			lNumber;
	BYTE				sType				[STR_LONG_CODE];//LONG			lType;				
	TAG_PUMP_STATUS		pumpStat;
   	BYTE				sNumberOfTrs		[STR_LONG_CODE];//SHORT			nNumberOfTrs;
	TAG_PUMP_TRANSACT	pumpTrs				[MAX_TRS_IN_LIST];
}TAG_PUMP;


typedef struct 
{
	BYTE			sNumber				[STR_LONG_CODE];//LONG				lNumber;
	BYTE			sRes				[STR_LONG_CODE];//PumpReservation	lRes;
	BYTE			sResExt				[STR_LONG_CODE];//
	BYTE			sStatus				[STR_LONG_CODE];//PumpStatus		lStatus;
	BYTE			sWarning			[STR_LONG_CODE];//BYTE				bWarning;
	BYTE			sNumberOfTrs		[STR_LONG_CODE];//SHORT				nNumberOfTrs;	
	BYTE			sNumberOfUnPaidTrs	[STR_LONG_CODE];//SHORT				nNumberOfUnPaidTrs;	
	BYTE			sNumberOfStackedPrePayTrs	[STR_LONG_CODE];//SHORT		nNumberOfUnPaidTrs;	
}TAG_MINIMAL_PUMP;

typedef struct 
{
	BYTE				sNumberOfPumps		[STR_LONG_CODE];//BYTE			nNumberOfPumps;			
	TAG_MINIMAL_PUMP	minimalPump			[MAX_PUMPS];
}TAG_MINIMAL_ALL_PUMPS;


typedef struct 
{
	BYTE			sNumber				[STR_LONG_CODE];//LONG				lNumber;
	BYTE			sRes				[STR_LONG_CODE];//PumpReservation		lRes;
	BYTE			sResExt				[STR_LONG_CODE];//
	BYTE			sStatus				[STR_LONG_CODE];//PumpStatus			lStatus;
	BYTE			sWarning			[STR_LONG_CODE];//BYTE				bWarning;
	BYTE			sNumberOfTrs		[STR_LONG_CODE];//SHORT				nNumberOfTrs;	
	BYTE			sNumberOfUnPaidTrs	[STR_LONG_CODE];//SHORT				nNumberOfUnPaidTrs;	
	BYTE			sNumberOfStackedPrePayTrs		[STR_LONG_CODE];//SHORT				nNumberOfUnPaidTrs;	
	BYTE			sNumberOfUnpaidTrsTimeout		[STR_LONG_CODE];
	BYTE			sNumberOfOnOfferOrAcceptedTrs	[STR_LONG_CODE];
	BYTE			sMessage			[STR_LONG_CODE];
	BYTE			sPumpMisc			[STR_LONG_VAL5];
	BYTE			sPumpMode			[STR_LONG_CODE];
}TAG_MINIMAL_PUMP_2;


typedef struct 
{
	BYTE				sSysError			[STR_LONG_VAL5];	
	BYTE				sSysMisc			[STR_LONG_VAL5];
	BYTE				sNumberOfPumps		[STR_LONG_CODE];//BYTE			nNumberOfPumps;			
	TAG_MINIMAL_PUMP_2	minimalPump			[MAX_PUMPS];
	BYTE				sPumpSrvVer			[STR_LONG_VAL10];	
	BYTE				sConvertLayerVer	[STR_LONG_VAL10];	
}TAG_MINIMAL_ALL_PUMPS_2;


typedef struct 
{
	BYTE			sNumber				[STR_LONG_CODE];//LONG				lNumber;
	BYTE			sRes				[STR_LONG_CODE];//PumpReservation		lRes;
	BYTE			sResExt				[STR_LONG_CODE];//
	BYTE			sStatus				[STR_LONG_CODE];//PumpStatus			lStatus;
	BYTE			sWarning			[STR_LONG_CODE];//BYTE				bWarning;
	BYTE			sNumberOfTrs		[STR_LONG_CODE];//SHORT				nNumberOfTrs;	
	BYTE			sNumberOfUnPaidTrs	[STR_LONG_CODE];//SHORT				nNumberOfUnPaidTrs;	
	BYTE			sNumberOfStackedPrePayTrs		[STR_LONG_CODE];//SHORT				nNumberOfUnPaidTrs;	
	BYTE			sNumberOfUnpaidTrsTimeout		[STR_LONG_CODE];
	BYTE			sNumberOfOnOfferOrAcceptedTrs	[STR_LONG_CODE];
	BYTE			sMessage			[STR_LONG_CODE];
	BYTE			sPumpMisc			[STR_LONG_VAL10];  //[STR_LONG_VAL5]; 3.0.2.3
	BYTE			sPumpMode			[STR_LONG_CODE];
	BYTE			sNumberOfHotPumpTrs [STR_LONG_VAL2];
	BYTE			sNumberOfHotPumpTrsTimeOut [STR_LONG_VAL2];
	BYTE			sUnBalancePrePayCounter[STR_LONG_CODE];							//3.0.2.3
	BYTE			sLatestPrePaySyncNumber[STR_LONG_VAL5];  // 5
	BYTE			sLatestPrePayPosNumber[STR_LONG_CODE];
	BYTE			sPumpTransactionWasOffer;
	BYTE			sTransactionAutoAuthorize;			//4.0.3.28
	BYTE			sPumpAsTerminal;					//4.0.3.60
	BYTE			sRoundedValue[STR_LONG_CODE];		//4.0.7.500
	BYTE			sNoParallelPakPrePayflg;			//4.0.9.501			
	BYTE			sNumberOfFailedCatTrs[3];			//4.0.22.508 75592
	BYTE            sGrade[STR_LONG_VAL2];				//4.0.23.509
	BYTE            sPosition;							//4.0.23.509
	BYTE			sFiler[2];							//4.0.9.501	//4.0.22.508 75592 //4.0.23.509
	//BYTE			sFiler[9];							//4.0.7.500	 //4.0.9.501
	//BYTE			sFiler[12];							//3.2.1.46  //4.0.3.28 //4.0.3.60 //4.0.7.500
	//BYTE			sFiler[13];							//3.2.1.46  //4.0.3.28 //4.0.3.60
	//BYTE			sFiler[14];							//3.2.1.46
	//BYTE			sFiler[15];							//3.1.1.4
	//BYTE			sFiler[23];							//3.0.2.3
}TAG_MINIMAL_PUMP_3;


typedef struct 
{
	BYTE				sSysError			[STR_LONG_VAL10];			//[STR_LONG_VAL5];	3.0.2.3
	BYTE				sSysMisc			[STR_LONG_VAL10];			//[STR_LONG_VAL5];  3.0.2.3
	BYTE				sNumberOfPumps		[STR_LONG_CODE];//BYTE			nNumberOfPumps;			
	TAG_MINIMAL_PUMP_3	minimalPump			[MAX_PUMPS];
	BYTE				sPumpSrvVer			[STR_LONG_VAL10];	
	BYTE				sConvertLayerVer	[STR_LONG_VAL10];
	BYTE				sOLACode			[STR_LONG_CODE]; //3.0.3.11
	BYTE				sTerminalNumber		[STR_LONG_VAL5]; //3.0.3.11
	BYTE                sTankAlarmGroup;		//4.0.2.23
	BYTE                sOLAAlarmGroup;		//4.0.2.23	
	BYTE                sServerStatus;		//4.0.3.55
	BYTE				sCarWashStatus;		//4.0.9.501  0 -onLine , 1-Offline
	BYTE				sUllageReportDone;	//4.0.9.503
    BYTE				sDeliveryReportDone; //4.0.9.503
	BYTE				sAlarmsSeverityOn;	//4.0.13.500	
	BYTE				byIsMaster;			//4.0.14.502	
	BYTE				sFiler[12];			//4.0.13.500	
//	BYTE				sFiler[14];			//4.0.3.503	
//	BYTE				sFiler[16];			//4.0.3.55	4.0.9.503
	//BYTE				sFiler[17];			//4.0.3.55	
	//BYTE				sFiler[18];			//4.0.2.23	
	//BYTE				sFiler[20];        //4.0.2.23
}TAG_MINIMAL_ALL_PUMPS_3;

typedef struct 
{
	BYTE				sSysError			[STR_LONG_VAL10];			//[STR_LONG_VAL5];	3.0.2.3
	BYTE				sSysMisc			[STR_LONG_VAL10];			//[STR_LONG_VAL5];  3.0.2.3
	BYTE				sNumberOfPumps		[STR_LONG_CODE];//BYTE			nNumberOfPumps;			
	TAG_MINIMAL_PUMP_3	minimalPump			[MAX_PUMPS];
	TAG_MINIMAL_PUMP_3	minimalPump2		[MAX_PUMPS];
	BYTE				sPumpSrvVer			[STR_LONG_VAL10];	
	BYTE				sConvertLayerVer	[STR_LONG_VAL10];
	BYTE				sOLACode			[STR_LONG_CODE]; //3.0.3.11
	BYTE				sTerminalNumber		[STR_LONG_VAL5]; //3.0.3.11
	BYTE                sTankAlarmGroup;		//4.0.2.23
	BYTE                sOLAAlarmGroup;		//4.0.3.55	 
	BYTE                sServerStatus;		//4.0.3.55	
	BYTE				sCarWashStatus;		//4.0.9.501  0 -onLine , 1-Offline
	BYTE				sUllageReportDone;	//4.0.9.503
    BYTE				sDeliveryReportDone; //4.0.9.503
	BYTE				sAlarmsSeverityOn;	//4.0.13.500	
	BYTE				byIsMaster;			//4.0.14.502	
	BYTE				sFiler[12];			//4.0.14.502	
//	BYTE				sFiler[17];			//4.0.3.55	
//	BYTE				sFiler[18];			//4.0.3.55	
	//BYTE				sFiler[20];
}TAG_MINIMAL_ALL_PUMPS_4;

//Liat TD 37244
typedef struct  
{
	BYTE			sNumber				[STR_LONG_CODE];
	BYTE			sRes				[STR_LONG_CODE];
	BYTE			sResExt				[STR_LONG_CODE];
	BYTE			sStatus				[STR_LONG_CODE];
	BYTE			sWarning			[STR_LONG_CODE];
	BYTE			sNumberOfTrs		[STR_LONG_CODE];
	BYTE			sNumberOfUnPaidTrs	[STR_LONG_CODE];
	BYTE			sNumberOfStackedPrePayTrs		[STR_LONG_CODE];
	BYTE			sNumberOfUnpaidTrsTimeout		[STR_LONG_CODE];
	BYTE			sNumberOfOnOfferOrAcceptedTrs	[STR_LONG_CODE];
	BYTE			sMessage			[STR_LONG_CODE];
	BYTE			sPumpMisc			[STR_LONG_VAL10];  
	BYTE			sPumpMode			[STR_LONG_CODE];
	BYTE			sNumberOfHotPumpTrs [STR_LONG_VAL2];
	BYTE			sNumberOfHotPumpTrsTimeOut [STR_LONG_VAL2];
	BYTE			sUnBalancePrePayCounter[STR_LONG_CODE];	
	BYTE			sLatestPrePaySyncNumber[STR_LONG_VAL5]; 
	BYTE			sLatestPrePayPosNumber[STR_LONG_CODE];
	BYTE			sPumpTransactionWasOffer;
	BYTE			sTransactionAutoAuthorize;	
	BYTE			sPumpAsTerminal;			
	BYTE			sRoundedValue[STR_LONG_CODE];	
	BYTE			sNoParallelPakPrePayflg;		
	BYTE			sRollingValue[STR_LONG_VAL10];
	BYTE			sCurrentValue[STR_LONG_VAL10];
	BYTE			sStoredValue[STR_LONG_VAL10];
	BYTE			sCurrentTrsStatus;
	BYTE			sStoredTrsStatus;
	BYTE            sGrade[STR_LONG_VAL2]; //4.0.19.504
	BYTE            sPosition;   	  
	BYTE            sCaption[MAX_PUMP_CAPTION_LEN];//4.0.20.500 -  TD 64967
	BYTE			sPumpMiscExt[STR_LONG_VAL10];	//TD 69203 4.0.21.500
	BYTE			sDieselPump;						//For Pilot
	BYTE			sNumberOfGreenPumpTrs;				//4.0.110.50 For Loves
	BYTE			sNumberOfGreenPumpTrsTimeOut;		//For Loves
	BYTE			sNumberOfFailedCatTrs[3];			//4.0.22.508 75592
	BYTE			sPriceHeld;							//4.9190.0 For Pilot	
	BYTE			sNumberOfANPRBlockedTrs[STR_LONG_VAL2];			//4.0.26.505 157073
	BYTE			sNumberOfANPRAuthWithFailedTrs[STR_LONG_VAL2];	//4.0.26.505 157073
	BYTE			sRollingVolume[STR_LONG_VAL10];
	BYTE			sTenderType[STR_LONG_VAL2];	
	BYTE			sFiler[2];		
}TAG_MINIMAL_PUMP_5;

//Liat TD 37244
typedef struct 
{
	BYTE				sSysError			[STR_LONG_VAL10];
	BYTE				sSysMisc			[STR_LONG_VAL10];
	BYTE				sNumberOfPumps		[STR_LONG_CODE];
	TAG_MINIMAL_PUMP_5	minimalPump			[MAX_PUMPS];
	TAG_MINIMAL_PUMP_5	minimalPump2		[MAX_PUMPS];
	BYTE				sPumpSrvVer			[STR_LONG_VAL10];	
	BYTE				sConvertLayerVer	[STR_LONG_VAL10];
	BYTE				sOLACode			[STR_LONG_CODE];
	BYTE				sTerminalNumber		[STR_LONG_VAL5];
	BYTE                sTankAlarmGroup;		
	BYTE                sOLAAlarmGroup;		
	BYTE                sServerStatus;		
	BYTE				sCarWashStatus;		
	BYTE				sUllageReportDone;	
    BYTE				sDeliveryReportDone; 
	BYTE				sAlarmsSeverityOn;	
	BYTE				byIsMaster;	
	BYTE                sActiveShiftNumber [STR_LONG_VAL5]; //4.0.24.10
	BYTE				byServerLockStatus[STR_LONG_VAL2];	//TD 107655 - 4.0.24.70 merge from 4.0.21.150 - LockStatus - STARTED ('0') , WAIT_LOCK ('1'), LOCKED ('2')...
	BYTE				byPriceChangeStatus[STR_LONG_VAL2];	//4.0.26.508 147183
	BYTE				sPriceChangeBatchNumber[STR_LONG_VAL10]; //4.0.26.508 147183
	BYTE                byPriceChangePOSMsg[STR_LONG_VAL2];     //4.0.27.42
	BYTE				sPriceDisplayPanelStatus;//CR 404591
	BYTE				sFiler[179];		
}TAG_MINIMAL_ALL_PUMPS_5;


// Current OLA information about the last OLA transaction.
//4.0.9.507
typedef struct 
{
	BYTE            m_byState;			//0 (1) OLA_NONE, OLA_REQUEST_TO_SEND, OLA_WAIT_RESPONSE, OLA_WAIT_FOR_MORE_INFO
	BYTE            m_byCheckCount;		//1 Timeout for response check
	LONG            m_lSessionNumber;   //2 Request seq. number.
	SYSTEMTIME		m_cSysTime;			//6 
}OLD_OLA_STAT; //Total size - 22

typedef struct 
{
	short           m_byState;			//0 (1) OLA_NONE, OLA_REQUEST_TO_SEND, OLA_WAIT_RESPONSE, OLA_WAIT_FOR_MORE_INFO
	BYTE            m_byCheckCount;		//1 Timeout for response check
	LONG            m_lSessionNumber;   //2 Request seq. number.
	SYSTEMTIME		m_cSysTime;			//6 
}OLA_STAT; //Total size - 23

typedef CArray <CPumpTransact, CPumpTransact &> TRS_ARRAY; // 4.0.23.1050 TD 233242


/////////////////////////////////////////////////////////////////////////////
// CPump
class CPump :	public 				
				CObject
{
 
protected:
	long				m_lLatestPrePaySyncNumber;
	long				m_lLatestPrePayPosNumber;
	DWORD				m_dwUnBalancePrePayCounter;
	DWORD				m_dwDeclinePrePayCounter;//4.0.12.502
	SYSTEMTIME			m_sSearchFeeTime;
	long				m_lCurrentServiceFee;  
	long				m_lCurrentServiceMode;    //4.0.2.21
	//Roni Start (version 4.0.9.800)
	long				m_lPriceLevel;
	long				m_lModeService;
	//Roni End
	CTime				m_cTimeSuppressedDriveoffTimeout[MAX_TRS_IN_LIST];  //2.9.1.1  //4.0.2.37 //4.0.2.38
	
	CString				m_strPumpMenufacture;
	CString				m_strPumpCaption; //4.0.20.500 -  TD 64967
	LONG				m_lNumber;
	LONG				m_lType;				
	CPumpStatus			m_cPumpStat;
	CArray <CPumpTransact, CPumpTransact &>m_cTrsArray;
	OLA_STAT			m_cOla;
	
	LockStatus			m_lLockStatus;
	long				m_lShiftNumber;
	
	PAY_AT_PUMP_INFO 	m_cPAPInfo;	//4.0.18.501
	//char				m_cPAPInfo[MAX_XML_BUF_LEN]; //4.0.14.500

	//4.0.5.39
	PumpLoginOp         m_byPumpLoginOp;


// Use to be in CServerPump
	SHORT				m_nVer;
	SHORT				m_nDetailedVer;
	
	DWORD				m_dwMaxUnpaidTrsAllowed;
	DWORD				m_dwMaxPrePayTrsAllowed;
	DWORD				m_dwMaxPostVoidTrsAllowed; //CR 351687: Enhanced post void transactions
	DWORD				m_dwDispensingTimeOut;
	DWORD				m_dwUnpaidTrsTimeOut;
	DWORD				m_dwPriceLevelDefault;
	DWORD				m_dwClosedPrePayTimeOut;
	DWORD				m_dwAllowPrePayOnRequestingPump;
	DWORD				m_dwPAKAuthorizedTimeOut;
	DWORD				m_dwMaxTrsNumber; //4.0.13.500
	BYTE				m_byPosition[MAX_POSITIONS_PER_PUMP];
	BYTE				m_byTankPerPosition[MAX_POSITIONS_PER_PUMP];	//4.0.20.52 (62818)
	BYTE				m_byMaskPosition[MAX_POSITIONS_PER_PUMP];  //4.0.9.507
	DWORD				m_dwPumpAsTerminal ;  //4.0.3.60
	DWORD				m_byPriceChangePosition[MAX_POSITIONS_PER_PUMP]; //4.0.9.503	
	DWORD				m_dwDieselPump;	//4.0.9.507
	DWORD				m_dwRequestingOnlyAfterPayInsidePress; //4.0.19.504 - 47559
	DWORD				m_dwMaxAttendantTrsAllowed;  // 4.0.22.501 - from 4.0.22.12 TD 73062
	DWORD				m_dwAllowPAKWithoutPayInsidePress;		//4.0.25.30 - TD 118690

	long				m_lPapToPakTrsNum; // 256101

	long				m_lLoyaltyPromptRetry;
	
public:
	long		m_lWaitCounter;

// Use to be in CServerPump
	LONG		m_lChangedTrsNumber;	// The old number of current transaction that was changed 
										// in while in progress from pre pay to pay at kiosk


public:

	CPump();					// default constructor
	CPump(const CPump& pump);
	CPump(long lPumpNumber);

	virtual ~CPump();

	void		SetLoyaltyPromptRetry(long lLoyaltyPromptRetry) {m_lLoyaltyPromptRetry = lLoyaltyPromptRetry; };
	long		GetLoyaltyPromptRetry() {return m_lLoyaltyPromptRetry; };

	BOOL        PumpStoppedByController();  //4.0.2.23
	BOOL		PumpValidForPrePay();
	void		ClearOlaStat();
	void		SetOlaStat(OLA_STAT *pOlaStat);
	void		GetOlaStat(OLA_STAT *pOlaStat);

	void		SetOlaStat(short OLAState) {
		//  I find that updating entire object isn't necessary for most of the times
		//  If it is, the caller can just go with the existing logic of Get/Set
		//	OLA_STAT OlaStat;
		//	GetOlaStat(&OlaStat);
		//	OlaStat.m_byState = OLAState;
		//	SetOlaStat(&OlaStat);
		m_cOla.m_byState = OLAState;
	}	
	short		GetOlaStat() const {
		return m_cOla.m_byState;
	}

	void		GetName(char * pName, int iMaxBuffLen);	
	long		GetType();
	BOOL		AnyTrsOnOffer(CPumpTransact	trs);

	int			GetTrsArraySize() const;
	LONG		GetNumber();
	void		SetTrsArraySize(int size);
	int			UnpaidFailedCATTransactions();		//4.0.22.508 75592
	int			UnpaidANPRBlockedTransactions();	//4.0.26.505 157073
	int			UnpaidANPRAuthWithFailedTransactions();	//4.0.26.505 157073
	int			UnpaidTransactions();	
	int			AcceptedTransactions(); //4.0.13.500	
	int			NumOfUnpaidAndAcceptedTrs(); //4.0.13.500
	int			UnpaidTimeoutTransactions();
	void		SetAllowPrePayOnRequestingPump(DWORD dwAllowPrePayOnRequestingPump);
	int			GetTrsByIndex(int index, CPumpTransact *pTrs) const;
	CPumpStatus	GetStat();
	void        operator =( TAG_PUMP & tagPump );
	CPump&      operator=(CPump & cPump);

	short GetVer() { return m_nVer; }
	short GetDetailedVer() { return m_nDetailedVer; }

	BYTE * GetPositionArray() { return (BYTE*)m_byPosition; }

	BOOL IsPumpOnHoldMaxConcurrent();		//4.0.21.500 69203
	void SetPumpOnHoldMaxConcurrent();		//4.0.21.500 69203
	void UnsetPumpOnHoldMaxConcurrent();	//4.0.21.500 69203
	short GetMaxAttendantTrsAllowed();      //4.0.21.501 merged from 4.0.22.12

	BYTE *	GetTankPerPositionArray() { return (BYTE*)m_byTankPerPosition; }	//4.0.20.52 (62818)
	void	SetTankPerPosition(short nPosition,BYTE byTank);					//4.0.20.52 (62818)
	BYTE	GetTankPerPosition(short nPosition);								//4.0.20.52 (62818)

	inline void	SetAutoReconcileTimeout(SHORT nTimeOut , UINT iTrs) 
	{ m_cTrsArray[iTrs].SetAutoReconcileTimeout(nTimeOut); } // 4.0.23.1050 TD 233242
	
// Use to be in CServerPump
public:
	BOOL IsAutoAuthForLoyaltyCustomer();
	void ForceCloseOlaSession();
	long GetDieselGrade();
	void SetDieselGrade(long lVal);

	BOOL CheckUnpaidFleetTrs();
	long m_lReeferId;
	long m_lDieselId; //4.0.11.509
	long GetReeferGrade();
	void SetReeferGrade(long lVal);
	void LoadManualInfo(long lModeNumber);
	BOOL GetDieselPumpFlags();
	void SetDiselPumpFlags(DWORD dwTmp);
	long GetInConpleteTrsNumber(long *pTrsNumber);
	int ReleasePendingPriceChangeTrs();
	void SetPriceChangeStatus(long lPosition , BOOL bVal);
	BOOL GetPriceChangeStatus();
	long RemovedStuckTransaction();
	void SetMiscCardNotAuthorized();
	long GetCurrentTrsRollingUpdateValue(BOOL bGetRemaining = FALSE );
	long GetCurrentTrsRollingUpdateVolume(BOOL bGetRemaning = FALSE); // 4.0.25.600   CR 296433
	void GetCurrentTrsTenderType(BYTE * sTenderType); // 4.0.25.600   CR 296433
	long GetCurrentTrsRollingUpdateVolumeByNozzle(short nNozzle); //4.0.25.50 - TD 108311
	long GetPAKLastUnpaidTrsRollingUpdateValue(long *); //TD 37244
	long GetPAKStoredUnpaidTrsRollingUpdateValue(long *); //TD 37244
	long GetUnpaidTrsStatus(long lTrsNum); //TD 37244
	DWORD IsPumpDefineAsTerminal(); 
	BOOL IsPerformLogin (PumpStatus lPumpStatus); //4.0.5.39
	BOOL IsPerformLogout(PumpStatus lPumpStatus); //4.0.5.39
	void SetPumpLoginOp(PumpLoginOp byLoginOp);   //4.0.5.39
	PumpLoginOp GetPumpLoginOp();                 //4.0.5.39
	BOOL IsUpdatePrepayAllow();
	void SetUnBalancePrepayCounter(DWORD dwUnBalancePrepayCounter);
	long GetIndexNumber(long lTrsNumber);
	long GetModeSchemeServiceMode();
	void SetModeSchemeServiceMode(long lServiceMode);
	//Roni Start(version 4.0.9.800)
	void SetPriceLevel(long lPriceLevel);
	long GetPriceLevel();

	void SetModeService(long lModeService);
	long GetModeService();
	//Roni End
	BOOL FindMatchingTimeOutPrePay();
	BOOL IsPumpIdle();   //4.0.1.0
	BOOL IsExistTransactionOnOffer();
	int GetAttendTrs(long lFlags, CPumpTransact *pTrs, long & lNumberOfTrs); //4.0.22.501 - from 4.0.22.12 TD 73062
	void SetLatestPrePayPosNumber(long lPosNumber);
	long GetLatestPrePayPosNumber();
	void SetLatestPrePaySyncNumber( long lSyncNumber);
	long GetLatestPrePaySyncNumber();
	BOOL IsLoyaltyClub();
	BOOL IsPapToPakTrs();							//4.0.22.506 67786
	void ClearPapToPakFlag();
	BOOL IsLoyaltyAutoStartPump();					//4.0.23.508 89149
	BOOL IsOLAExtraInfoTrs();						//4.0.24.140 118124

	BOOL IsSavedDataInAsh2000Qdx();					//118124 4.0.25.50
	void SetPriceChangeStatusToPOS(short iStatus);		//4.0.26.508 147183

	BOOL IsForceZeroPakCompletionOn(); //4.0.14.1390 67387 //4.0.21.0
	BOOL IsZeroCompletionNeededForPAP();			//4.0.24.140 130693 

	void SetInPAKLoyaltyWithComplete(BOOL bVal); //4.0.14.1390 67387 //4.0.21.0
	BOOL GetInPAKLoyaltyWithComplete(); //4.0.14.1390 67387 //4.0.21.0
	void ClearPAKLoyaltyWithCompleteFlags(); //4.0.14.1390 67387 //4.0.21.0
	
	void DecreaseUnBalancePrePayCounter();
	void IncreaseUnBalancePrePayCounter();
	void DecreaseDeclinePrePayCounter();//4.0.12.502
	void IncreaseDeclinePrePayCounter();//4.0.12.502

	long GetUnBalancePrepayCounter();
	long GetDeclinePrepayCounter();//4.0.12.502
	long IsAllowPrePayOnRequestingPump();
	void SetTagPumpTrsToTrs(TAG_PUMP_TRANSACT2 &tagTrs, CPumpTransact &trs);
	void SetTagPumpTrsToTrs(TAG_PUMP_TRANSACT5 &tagTrs, CPumpTransact &trs); //TD 38654

	long GetNumberOfHotPumpTransactions(long  & lNumberOfHotPumpTrs,long & lNumberOfHotPumpTrsTimeOut);
	long GetPrePayInfo(long lSyncNumber,PRE_PAY_INFO *pInfo)const;
	void SetCurentServiceFee(long lFeeNumber);
	long GetCurrentServiceFee();
	BOOL CanReserveForPumpLock(PumpLockType eLockType = LOCK_TYPE_MAINT);  //4.0.3.27
	CTime GetTimeSuppressedDriveoffTimeout(long lPosition);
	void SetTimeSuppressedDriveoffTimeout(long lPosition ,CTime time);
	BOOL		CanStopPump();
	DWORD		GetUnpaidTrsTimeOut();
	DWORD		GetDispensingTimeOut();
	void		SetDispensingTimeOut(DWORD dwDispensingTimeOut);
	DWORD		GetClosedPrePayTimeOut();
	DWORD		GetPAKAuthorizedTimeOut();
	void		SetPAKAuthorizedTimeOut(DWORD dwPAKAuthorizedTimeOut);
	DWORD		GetMaxTrsNumber(); //4.0.13.500
	void		SetMaxTrsNumber(DWORD dwMaxTrsNumber); //4.0.13.500
	short		GetMaxPrePayTrsAllowed();
	short		GetMaxPostVoidTrsAllowed();
	short		GetMaxUnpaidTrsAllowed();
	long		GetPAPSeqNumber();
	void		SetPAPSeqNumber(long lSeqNumber);
	BYTE		GetPAPTranStatus();
	void		SetGradePosition(short nPosition,BYTE byGrade);
	BYTE		GetGradePosition(short nPosition,long * idx=0);//amram 4.0.10.503
	short		NozzleToGrade(short nNozzle);
	long		GetPriceLevelDefault();
	long		GetNumOfPositionForGrade(short nGrade);//amram 4.0.10.503
	void		SetPriceLevelDefault(long lLevel);//4.0.11.509

	void		LoadDefinition();
	void		SetPAPInfo(const PAY_AT_PUMP_INFO *pPAPInfo);
	void		GetPAPInfo(PAY_AT_PUMP_INFO *pPAPInfo) const;
	void		SetType(long lType);
	void		SetName(char * pName, int iBuffLen);
	BOOL		GetTrsByStatus(TrsStatus lStatus,short nLinkFlags, CPumpTransact *pTrs);
	BOOL		GetNextWaitingTrs(CPumpTransact * pTrs);
	BOOL		CanReserve();
	BOOL		CanReserveForPrePay();
	BOOL		AlreadyReserveForSinglePrePay();
	void		GetTrsListCount(short * pWaiting, short * pFree, short *pUnpaid);
	short		GetPostVoidTrsCount(); //CR 351687: Enhanced post void transactions
	void		GetDummyPrePayTrs(short * pDummyPrePayWaiting); //4.0.19.509 (62335)
	BOOL		CheckIfAllUnpaidArePAK();//4.0.9.506 //amram
	long		FindOldestUnpaidPakTrs();//4.0.9.506 //amram
	short		GetNumberOfNonClosedPrePay();
	short		GetNumberOfTimeOutPrePay();
	short		GetNumberOfTrsInList();
	int			ClearTrs(long lTrsNumber);
	int			GetCurrentTrsNumber(long *pTrsNumber);
	BOOL		IsCurrentTrs();
	int			GetCurrentTrs(CPumpTransact * pTrs);
	int			GetInProgressWaitProgressTrs(CPumpTransact * pTrs);
	int			GetTrsByNumber(LONG lTrsNumber, CPumpTransact *pTrs );	
	int			GetTrsByLinkNumber(CPumpTransact * pTrs);
	void		SetShiftNumber(long lNumber);
	long		GetShiftNumber();
	LockStatus	GetLockStatus();
	void		SetLockStatus(LockStatus lStatus,long lMisc = MISC_IGNORE_FIELD_VALUE);

	void		GetTagMinimal(void *pBuff,long lVersion = 1);
	void		GetTag(void *pBuff, long lVersion = 1);
	void		FillShortCardName(long lPumpNumber,BYTE  sShortCardNameArr[MAX_TRS_IN_LIST][SHORT_CARD_NAME_LEN]);
	int			GetTagTrsByIndex(int index, void *pBuff,long lVersion = 1) const;
	int         GetTagTrsByNumber(LONG lTrsNumber,void *pBuff,long lVersion = 1) const;
	void		UpdateVer();
	void		UpdateDetailedVer();
	int			InsertTrs(CPumpTransact *pTrs,BOOL bAccept0Transaction = FALSE , BOOL bPostVoid = FALSE);
	void		SetNumber(LONG lNumber);	
	int         SetTrsByIndex(int index,CPumpTransact *pTrs, BOOL bRollingUpdate);
	int			SetTrsByNumber(LONG lTrsNumber, CPumpTransact *pTrs,  BOOL bRollingUpdate);	
	void		SetStat(const CPumpStatus & pumpStat);
	void		GetTagStat(void *pBuff, long lVersion = 1);
	long		GetPreSetValue(long lSyncNumber) const;
	void		FillPreSetData(long lPumpNumber, BYTE sPreSetLimit[MAX_TRS_IN_LIST][STR_LONG_VAL7], BYTE sPreSetLimitType[MAX_TRS_IN_LIST], long lVersion);		// Ella 4.0.15.500 - CR 19675
	long		GetPapToPakTrsNum() const { return m_lPapToPakTrsNum; } // TD 256101
	void		SetPapToPakTrsNum(long lTrsNum) { m_lPapToPakTrsNum = lTrsNum; } // TD 256101
	int			GetTrsIndexByReservation(PumpReservation res , int & iTrsNum); // TD 233242	
	const TRS_ARRAY *	GetTrsArray() const { return &m_cTrsArray; } // 4.0.23.1050 TD 232243
	long		GetLastUnpaidPAKTrs(); //4.0.29.10 - TD 264032
};


#define THRD_PUMPSRV_IS_MASTER		(pInfo->m_pServer->GetOperatingMode() == MASTER)			
#define THRD_PUMPSRV_IS_SLAVE		(pInfo->m_pServer->GetOperatingMode() == SLAVE)			
#define THRD_PUMPSRV_IS_NOT_MASTER	(pInfo->m_pServer->GetOperatingMode() != MASTER)			
#define THRD_PUMPSRV_IS_NOT_SLAVE	(pInfo->m_pServer->GetOperatingMode() != SLAVE)			

#define THRD_PUMPSRV_IS_MODE_MASTER	(pInfo->m_pServer->m_bControllerManagePumpMode == FALSE)			
#define THRD_PUMPSRV_IS_MODE_SLAVE	(pInfo->m_pServer->m_bControllerManagePumpMode == TRUE)			



typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	TAG_PUMP_TRANSACT		tagTrs;
}TAG_OLA_REC;

typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	TAG_PUMP_TRANSACT2		tagTrs;
}TAG_OLA_REC2;

typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	TAG_PUMP_TRANSACT5		tagTrs;
}TAG_OLA_REC2_EX;	//TD 38654


typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	CARD_SALE_EXTRA_DATA2_  extraData2;
	TAG_PUMP_TRANSACT2		tagTrs;
	BYTE					sAsh2000[ASH_2000_BUFFER_SIZE];    //4.0.3.27
}TAG_OLA_REC3_PLUS_ASH_DATA;

typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	CARD_SALE_EXTRA_DATA2_  extraData2;
	TAG_PUMP_TRANSACT5		tagTrs;
	BYTE					sAsh2000[ASH_2000_BUFFER_SIZE];    //4.0.3.27
}TAG_OLA_REC3_PLUS_ASH_DATA_EX; //TD 38654

typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	CARD_SALE_EXTRA_DATA2_  extraData2;	
	TAG_PUMP_TRANSACT2		tagTrs;
	BYTE					sAsh2000[ASH_2000_BUFFER_SIZE];    //4.0.3.27
	CARD_SALE_EXTRA_DATA3_  extraData3;
	
}TAG_OLA_REC5_PLUS_ASH_DATA;

typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	CARD_SALE_EXTRA_DATA2_  extraData2;	
	TAG_PUMP_TRANSACT5		tagTrs;
	BYTE					sAsh2000[ASH_2000_BUFFER_SIZE];    //4.0.3.27
	CARD_SALE_EXTRA_DATA3_  extraData3;
	
}TAG_OLA_REC5_PLUS_ASH_DATA_EX;	//TD 38654


typedef struct 
{	
	TAG_PRE_PAY_INFO		tagInfo;		
	TAG_PUMP_TRANSACT2		tagTrs;	
	BYTE sTotalReward[STR_LONG_VAL10];
	BYTE sPosOffers[5];    //4.0.5.39
	BYTE sFiler[15];
	//BYTE sFiler[20];	
}TAG_PRE_PAY_REC2;					

typedef struct 
{	
	TAG_PRE_PAY_INFO		tagInfo;		
	TAG_PUMP_TRANSACT5		tagTrs;	
	BYTE sTotalReward[STR_LONG_VAL10];
	BYTE sPosOffers[5];    //4.0.5.39
	BYTE sFiler[15];
}TAG_PRE_PAY_REC2_EX; //TD 38654	

typedef struct     // 4.0.0.6
{
	TAG_PUMP_TRANSACT2		tagTrs2;
	CARD_SALE_DATA			cardData;
	unsigned char sFuelTotal[8];
	BYTE	sTotalAmount[9];		//4.0.1.35
	BYTE	sAuthAmount[9];			//4.0.6.501
	char	sFiler24[24];			//4.0.6.501
	//char	sFiler33[33];			//4.0.1.35
	//char sFiler42[42];		//4.0.1.35
}TAG_OLA_MEM_REC;

typedef struct     // 4.0.0.6
{
	TAG_PUMP_TRANSACT5		tagTrs;
	CARD_SALE_DATA			cardData;
	unsigned char sFuelTotal[8];
	BYTE	sTotalAmount[9];		//4.0.1.35
	BYTE	sAuthAmount[9];			//4.0.6.501
	char	sFiler[50];			//4.0.6.501
}TAG_OLA_MEM_REC_EX;	//TD 38654

typedef struct     
{
	unsigned char cAviStatus[MAX_PUMPS];

}TAG_OLA_MEM_STATUS_AVI;

typedef struct
{
	unsigned char cAviStatus[MAX_PUMPS_64];
		
}TAG_OLA_MEM_STATUS_AVI2;

#define USE_TAG_OLA_MEM_STATUS_AVI2 0x00000040

PumpStatus l2PumpStatus(long lVal);
PumpReservation l2PumpReservation(long lVal);
PumpDevice l2PumpDevice(long lVal);
TrsStatus l2TrsStatus(long lVal);
PumpResExt l2PumpResExt(long lVal);	


typedef struct 
{
	BYTE sLoyalId[STR_20];	
	TAG_DISCOUNT_MAP sDiscountArray;
	BYTE sPromotionNumber[10];		
	BYTE sAttendantID[10];				//3.2.0.76
	BYTE sAttendantName[15];			//3.2.0.76
//  3.1.1.66
	BYTE sDiscountType[2];	            //4.0.5.502    None-0, 1-cents, 2-percentage  
	BYTE sPreSetLimit[7];				// 4.0.15.500
	BYTE sPreSetLimitType;				// 4.0.15.500	
	BYTE sFiller[5];					//3.2.0.76 4.0.5.502 //4.0.15.500
	//BYTE sFiller[15];					//3.2.0.76 //4.0.5.502
//  3.1.1.66
	//BYTE Filer[40];
// 3.1.1.66
}TAG_DISCOUNT_LOYALTY_INFO;

//4.0.9.507
union EXTRA_INFO_
{
	 EXTRA_INFO		cExtraInfo;
	 DIESEL_INFO	cDieselInfo;
};

//4.0.5.26
typedef struct 
{
	BYTE sLoyalId[STR_20];	
	TAG_DISCOUNT_MAP sDiscountArray;
	BYTE sPromotionNumber[10];		
	BYTE sAttendantID[10];	
	BYTE sAttendantName[15];
	BYTE sDiscountType[2];
	BYTE sPreSetLimit[7];			// 4.0.15.500
	BYTE sPreSetLimitType;			// 4.0.15.500
	BYTE sFiller01[5];				// add new	4.0.5.30 // 4.0.15.500
	BYTE sCustomerName[20];
	BYTE sTotalBalance[8];
	BYTE sBalanceType;   // '1' - volume, '0' value
	BYTE sHostTransId[8];
	BYTE sLAMFlags;	
	BYTE sLAM[50];			//4.0.5.44			
	BYTE   sCarWashProgram;//4.0.11.90
	BYTE   sWashCode[6];     //Car Wash Code
	BYTE   sWashExpire[6];  //4.0.11.90
	BYTE   sCarWashDiscount[7]; //4.0.11.90
	BYTE   sCarWashTax[7];//4.0.11.90
	BYTE   sCarWashAmount[7];    //99,999.99 //4.0.11.90
	BYTE sFiller[7];		//4.0.11.90	
	//BYTE sFiller[92];			4.0.5.44			 
}TAG_DISCOUNT_LOYALTY_INFO2;

typedef struct 
{
	BYTE sLoyalId[STR_20];	
	TAG_DISCOUNT_MAP sDiscountArray;
	BYTE sPromotionNumber[10];		
	BYTE sAttendantID[10];	
	BYTE sAttendantName[15];
	BYTE sDiscountType[2];
	BYTE sPreSetLimit[7];			
	BYTE sPreSetLimitType;			
	BYTE sFiller01[5];				
	BYTE sCustomerName[20];
	BYTE sTotalBalance[8];
	BYTE sBalanceType;   
	BYTE sHostTransId[8];
	BYTE sLAMFlags;	
	BYTE sLAM[50];						
	BYTE sCarWashProgram;
	BYTE sWashCode[6];     
	BYTE sWashExpire[6];  
	BYTE sCarWashDiscount[7]; 
	BYTE sCarWashTax[7];
	BYTE sCarWashAmount[7];    
	BYTE sOLDSession[6];		//marikdebug //4.0.24.60 BP 105272
	//BYTE sFiller[7];			//marikdebug //4.0.24.60 BP 105272
	BYTE sFiller1;			
	BYTE sTotalDiscount[10];
	BYTE sOriginalUnitPrice[5]; //29-Jul-07
	DISCOUNT_ATTRIBUTES	discountAttrsInfo; //4.0.17.501 - CR 38162	//28 //4.0.18.502
	BYTE sLoyaltyVolumeLimit[7];//4.0.20.450 66977	//4.0.21.501
	BYTE sDeferralAccepted;	//4.0.22.504 69344
	DISCOUNT_ATTRIBUTES_EXT	sDiscountAttrExt;//8		//4.0.22.509 38162
	BYTE	sMaxDiscountAllowed[5]; 	//RFUEL-3309 Get the maximal discount allowed for the grade used on call QueryExtraInfoData  with flags 2049/(QEI_GET_DIRECT_INDEX_Flag = 0x1 + QEI_STRUCT_IS_LOYALTY_INFO4 = 0x800)
	BYTE sFiller90[36]; //4.0.24.70
}TAG_DISCOUNT_LOYALTY_INFO3;	//Liat LPE

typedef struct
{
	BYTE				sANPRRefNumber[STR_LONG_VAL5];
	BYTE				sFullVehicleId[FULL_VEHICLE_ID]; //16
	BYTE				sANPRTrsStatus[STR_LONG_VAL2];						//4.0.26.505 157073
	TAG_SYSTEMTIME		cVRNDateTime;										//4.0.26.505 157073
	BYTE                sTrsFlowRate[4];									//4.0.27.40 TD 165585
	BYTE                sAverageFlowRate[4];								//4.0.27.40 TD 165585
	BYTE                sRatesFromDevice;									//4.0.27.40 TD 165585
	BYTE				sFiller[553];										//4.0.26.505 157073	//4.0.27.40 TD 165585
}TAG_ANPR_INFO_PLUS;

typedef struct 
{
	TAG_DISCOUNT_LOYALTY_INFO3	cDiscountLoyaltyInfo3;
	RECEIPT_EXTRA_INFO			cReceiptExtraInfo;	 		 
	BYTE						sPreDispensingVolumeLow[5];			//4.0.23.509 97360
	BYTE						sPreDispensingVolumeHigh[15];		//4.0.23.509 97360
	BYTE						sPostDispensingVolumeLow[5];		//4.0.23.509 97360
	BYTE						sPostDispensingVolumeHigh[15];		//4.0.23.509 97360
	GRADE_LIMITS				GradeAmountLimit[8];				//4.0.24.60 
	BYTE						sTrsAutoReconcileTimeout[3];			// 4.0.23.1050 TD 233242
	BYTE						sLockingKey[40];
	BYTE						sStoreIDField[24];
	BYTE						sPosId[10];
	BYTE						sTransId[10];
	BYTE						sLoyaltyStartDateTime[19];
	BYTE						sTicketId[10];
	BYTE						sClubType[2];
	BYTE						sPromotionID[15];
	LOYALTY_CARD_DETAILS		sLoyaltyCard;					// RFUEL-520
	TAG_ANPR_INFO_PLUS			cANPRInfo; //
//	BYTE						sFiller[347];
}TAG_DISCOUNT_LOYALTY_INFO4; //4.0.19.710 66705 //4.0.23.504 //4.0.23.270 111746												//4.0.23.500 - TD 69914 

typedef struct
{
	BYTE				sUnLockSerNumPumpTest[12];
	BYTE				sFiller[100];
}TAG_PUMP_TEST_INFO;											 //4.0.25.1140 TD 412765

//4.0.1.32 start
typedef struct
{
	BYTE			    byIndexNumber[STR_LONG_VAL10];		 
	BYTE				byTank[STR_LONG_VAL10];				 
	BYTE				byProduct[STR_LONG_VAL10];			 
	BYTE				byStatus[STR_LONG_VAL10];			 
	BYTE				byFlags[STR_LONG_VAL5];				 
	BYTE				byTag[STR_LONG_VAL10];				 
	BYTE				byDeliveredVolume[STR_LONG_VAL10];	 
	BYTE				byShiftNumber[STR_LONG_VAL10];		 
	TAG_SYSTEMTIME		cDeliveryDateTime;	 
	TAG_SYSTEMTIME		cEndDeliveryDateTime;
	BYTE				byStartVolume[STR_LONG_VAL10];		 
	BYTE				byEndVolume[STR_LONG_VAL10];			 
	BYTE				byTemperature[STR_LONG_VAL10];		 

} TAG_SHIFT_DELIVERY_REC;

typedef struct
{
	BYTE byNozzleStatus[STR_LONG_VAL5];
	BYTE byPumpStatus[STR_LONG_VAL5];
	BYTE byShiftNumber[STR_LONG_VAL10];
	BYTE byIndexNumber[STR_LONG_VAL10];
	BYTE byPumpNumber[STR_LONG_VAL5];
	BYTE byFlags[STR_LONG_VAL5];
	BYTE byGrade[STR_LONG_VAL5];
	BYTE byNACSCode[STR_LONG_VAL5];
	BYTE byNozzle[STR_LONG_VAL5];

	BYTE sVolume[MAX_NAME_LENGTH];
	BYTE sValueCash[MAX_NAME_LENGTH];
	BYTE sValueCredit[MAX_NAME_LENGTH];
	
	char sPumpManufacture[MAX_NAME_LENGTH];

} TAG_SHIFT_TOTALS_REC;


typedef struct
{
  BYTE sData[1019];
}TAG_ASH2000_DATA;


typedef struct 
{
	CARD_SALE_DATA			CardSaleData;	
	CARD_SALE_EXTRA_DATA	CardSaleExtraData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT2		PumpTransact;
	CARD_SALE_EXTRA_DATA3_  extData3;
}TAG_RECEIPT_DATA3;

typedef struct 
{
	CARD_SALE_DATA			CardSaleData;	
	CARD_SALE_EXTRA_DATA	CardSaleExtraData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT5		PumpTransact;
	CARD_SALE_EXTRA_DATA3_  extData3;
}TAG_RECEIPT_DATA3_EX;	//TD 38654

typedef struct 
{
	CARD_SALE_DATA			CardSaleData;	
	CARD_SALE_EXTRA_DATA	CardSaleExtraData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT2		PumpTransact;
	CARD_SALE_EXTRA_DATA3_  extData3;
	RECEIPT_EXTRA_INFO		cReceiptExtraInfo;
}TAG_RECEIPT_DATA4;


typedef struct 
{
	CARD_SALE_DATA			CardSaleData;	
	CARD_SALE_EXTRA_DATA	CardSaleExtraData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT5		PumpTransact;
	CARD_SALE_EXTRA_DATA3_  extData3;
	RECEIPT_EXTRA_INFO		cReceiptExtraInfo;
}TAG_RECEIPT_DATA4_EX;	//TD 38654

// 4.0.5.26
typedef struct
{
	BYTE			sShiftNumber[10];
	BYTE  			sTransactionType [5];
	BYTE  			sVal[15];
	BYTE  	 		sVolume[15];
	BYTE			sCounter[8];
	BYTE			sGradeId[5];
	TAG_SYSTEMTIME	sDateTime;
	BYTE			sFiler[26];
}TAG_ACCUMULATE_TOTALS;

class CMode
{
public:
	BOOL PAKAutoReconcile();
	BOOL PAKAutoAuthLoyaltyOnly();					//4.0.23.508 89149
	void GetModeDefTagInfo(void *pBuff);
	void SetLimit(long lLimit,long lPriceLevel);
	long GetLimit(long lPriceLevel);
	BOOL CashAcceptorValid();
	BOOL CashAcceptorAuth();
	BOOL IsUnmannedMode(); //4.0.14.503
	BOOL IsNeedToLockPump();
	void SetNumber(long lNumber);
	void operator =( const CMode& pumpStat );
	long LoadDefinition();
	BOOL ValidMode();
	CString GetName();
	long GetFlags();
	CMode(const CMode& mode);	
	CMode(long lNumber);
	void SetName(CString strName);
	void SetName(char *pszName);
	void SetFlags(long lFlags);
	void SetMsgNumber(long lMsgNumber); //4.0.5.41
	long GetMsgNumber();  //4.0.5.41
	BOOL PayAVINotValid();
	BOOL PayAVIValid();
	BOOL PayAttendNotValid();
	BOOL PayAttendValid();
	BOOL PrePayNotValid();
	BOOL PrePayValid();
	BOOL PAPNotValid();
	BOOL PAPValid();
	BOOL PAKNotValid();
	BOOL PAKValid();
	BOOL PayAVIAutoAuth();
	BOOL PayAttendAutoAuth();
	BOOL PrePayAutoAuth();
	BOOL PAPAutoAuth();
	BOOL PAKAutoAuth();
	BOOL PAPPostPayValid(); //4.0.29.10 - TD 264032
	BOOL PAMNotValid();// CR 444830
	BOOL PAMValid();// CR 444830
	BOOL PAMAutoAuth();// CR 444830

	BOOL AutoAuthorize(PumpReservation lRes, PumpResExt lResExt = NULL_RES_EXT);
	BOOL ResNotValid(PumpReservation lRes,PumpResExt lResExt = NULL_RES_EXT);
		//Roni Start(version 4.0.9.800)
	long GetPriceLevel();
	void SetPriceLevel(long priceLevel);

	long GetModeService();
	void SetModeService(long ModeService);
	//Roni End

	CMode();
	virtual ~CMode();

protected:
	long	m_lCreditLimit;//4.0.6.506
	long	m_lCashLimit;
	long	m_lNumber;
	long	m_lFlags;
	CString	m_sName;
	long    m_lMsgNumber; //4.0.5.41
	//Roni Start(version 4.0.9.800)
	long    m_lPriceLevel;
	long	m_lModeService;
	//Roni End
};


class CModeScheme
{
public:	
	long GetServiceMode();
	void SetServiceMode(long lFlags);
	long GetPumpServiceMode();
	void SetServiceFeeSet(long lServiceFeeNumber);
	long GetServiceFeeSet();
	BOOL IsValidPeriod();
	void SetIndex(long lPumpNumber,long lScheme);
	CString GetStringPeriodEndTime();
	CString GetStringPeriodStartTime();
	CTime GetPeriodEndTime();
	CTime GetPeriodStartTime();
	void SetPeriodEndTime(SYSTEMTIME sysTime);
	void SetPeriodEndTime(TAG_SYSTEMTIME *pTagTime);
	void SetPeriodStartTime(SYSTEMTIME sysTime);
	void SetPeriodStartTime(TAG_SYSTEMTIME *pTagTime);
	BOOL YearlyModeScheme();
	long SaveNotValid();
	void SetMode(long lMode);
	long GetMode();
	long LoadDefinition(long lVersion=0);
	long SaveDefinition(long lVersion=0);
	BOOL ValidModeScheme();
	long GetFlags();
	CModeScheme(const CModeScheme& modeScheme);	
	CModeScheme(long lPumpNumber,long lScheme);	
	void SetFlags(long lFlags);
	BOOL DailyModeScheme();
	BOOL WeeklyModeScheme();
	BOOL InWeeklyTime(SYSTEMTIME & sysTime);
	BOOL InPeriodDailyTime(SYSTEMTIME & sysTime);//4.0.2.40
	CModeScheme();
	virtual ~CModeScheme();
	BOOL Active(SYSTEMTIME & sysTime);
	void SetStartTime(SYSTEMTIME sysTime);
	void SetStartTime(TAG_SYSTEMTIME *pTagTime);
	void SetEndTime(SYSTEMTIME sysTime);
	void SetEndTime(TAG_SYSTEMTIME *pTagTime);
	CTime GetStartTime();
	CTime GetEndTime();
	CString GetStringEndTime();
	CString GetStringStartTime();

protected:
	long	m_lNumber;
	long	m_lPump;
	long	m_lFlags;
	long	m_lMode;
	CTime   m_cStartTime;
	CTime   m_cEndTime;
	CTime	m_cPeriodEndTime;
	CTime	m_cPeriodStartTime;
	long	m_lServiceFeeSet;
	long	m_lServiceMode;  //4.0.2.21	
};

class CLanguages
{
public:
	CLanguages();
	virtual ~CLanguages();
};


typedef struct 
{
	CARD_SALE_DATA			cCardData;
	CARD_SALE_EXTRA_DATA	cCardSaleExtra;
	CARD_SALE_EXTRA_DATA2_	cCardSaleExtra2;	
	CPumpTransact			cTrs2;
	BYTE					cRecordStatus;  //3.0.1.1 0 - not use , 1 - charge avi , 2 -	time out avi ,  			
	BYTE					byIndex;	
	unsigned char			sFiler[44];	// Not relevant //4.0.22.501	TD 72716 //4.0.22.511 72606

	BYTE					sReceiptIndex[ENCRYPTED_DATA_SIZE];		//4.0.26.500 TD 144219
	//unsigned char  sFiler[50];
}CMemPumpInfo;

#include "PumpSrvDB.h"

class CMemPump  
{
public:
	void CleanList(); //4.0.19.504 - 47559
	BOOL SetMemPumpInfo(CMemPumpInfo  & Info);
	BOOL GetMemPumpInfo(CMemPumpInfo *pInfo , short nIndex );
	BOOL SetMemPumpInfo(PAY_AT_PUMP_INFO *pData , CPumpTransact  *pTrs , RECEIPT_INFO *pReceiptInfo = NULL , BYTE byIndex=0); //3.0.4.9
	BOOL SetMemPumpInfo(CXMLInterface *pData , CPumpTransact  *pTrs, BYTE byIndex=0); //4.0.14.500

	long LookForInternalReceiptIndex(char* psIndexOut, const BYTE* szTrsNumIn);		//4.0.26.500 TD 144219
	long LookForInternalReceiptIndex(char* psIndexOut, long lTrsNumber);		//4.0.26.500 TD 144219
	BOOL RemoveTrsMemPumpInfo(long lTrsNumber); 

	CMemPump();
	virtual ~CMemPump();
	CArray<CMemPumpInfo , CMemPumpInfo&>  m_cFifoQueue;
protected:
	long OperationOnPumpInfo(CARD_SALE_DATA *pCardData);
}; 


class CServiceFee  
{
public:
	long GetSelfAttendService();
	void SetSelfAttendService(long lSelfAttendService);
	long GetSelfAviService( );
	void SetSelfAviService( long lSelfAviService);
	long GetSelfPrePayService();
	void SetSelfPrePayService( long m_lSelfPrePayService);
	long GetSelfPapService( );
	void SetSelfPapService( long lSelfPapService);
	long GetSelfPakService( );
	void SetSelfPakService( long lSelfPakService);
	CString GetName();
	void SetName(CString sName);
	void SetName(char *pszName);
	long GetFlagsServiceFee();
	void SetFlagsServiceFee(long lFlags);
	void SetAttendService (long lServiceAttend);
	void SetAviService(long lServiceAvi);
	void SetPapService(long lPapService);
	void SetPrePayService(long lPrePayService);
	void SetPakService(long lPakService);
	long GetPakService();
	long GetAttendService();
	long GetAviService();
	long GetPapService();
	long GetPrePayService();
	BOOL ValidServiceFee();
	CServiceFee(const CServiceFee & ServiceFee);
	void SetNumber(long lNumber);
	long LoadDefinition();
	CServiceFee();
	virtual ~CServiceFee();

protected:
	long m_lFlags;
	long m_lAttendService;
	long m_lPapService;
	long m_lPrePayService;
	long m_lAviService;
	long m_lPakService;
	long m_lSelfAttendService;
	long m_lSelfPapService;
	long m_lSelfPrePayService;
	long m_lSelfAviService;
	long m_lSelfPakService;

	long m_lNumber;
	CString m_sName;

};





class CTagOlaRecConvert  
{
public:
	CTagOlaRecConvert();
	virtual ~CTagOlaRecConvert();

};

#endif // __PUMP_CLASS_H__