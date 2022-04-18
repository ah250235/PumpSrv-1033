//-----------------------------------------------------------------------
// File Name   :	CmdInfo
// 
// Description :	Command Information class definition
//					
// Written by  :	Moti Lichi 10.04.97
// 
//-----------------------------------------------------------------------

#ifndef __COMMAND_INFO_H__
#define __COMMAND_INFO_H__


// Any Command have :
// --------------------------------
//  long	m_lNumber;
//	Command m_lCommand;
//	LONG	m_lSrc;
//
// And Extra information (optional): 
// --------------------------------
//	LONG	m_lParameter;
//	char	m_Data[MAX_CMD_DATA_SIZE];



// Transaction extra information:
typedef struct 
{
	LONG	lTrsNumber;
	LONG	lFlags;		//4.0.22.501 - TD 69904
	BSTR	sData;		//4.0.22.501 - TD 69904 //4.0.23.503 
} CMD_STRUCT_TRS;

// Limit Pump extra information:
typedef struct 
{
	short	nLevel;
	long	lMaxValue;
	long	lMaxVolume;
	short	nFlags;
} CMD_STRUCT_LIMIT;
	
// Pump mode extra information:
typedef struct 
{
	LONG	lMode;	
} CMD_STRUCT_MODE;


// 4.0.2.21
typedef struct 
{
	LONG	lMode;
	LONG	lFlags;
	LONG	lServiceMode;
	LONG	lServiceFee;
} CMD_STRUCT_MODE2;
// 4.0.2.21


//4.0.9.506
// mode + price
typedef struct
{
	LONG	lMode;
	LONG	lPriceLevel;
	LONG	lFlags;
	LONG	lServiceMode;
	LONG	lServiceFee;
} CMD_STRUCT_MODE3;




typedef struct CMD_STRUCT_AUTHORIZE_LIMIT
{
	short	nLevel;
	LONG	lGradePrice;
	LONG	lMaxValue;
	LONG	lMaxVolume;
	long	lFlags;			//4.0.9.507
	LONG	lSyncNumber;
	LONG	lDiscountArray[MAX_GRADES];	
	BYTE	sDiscountType[2]; // 4.0.6.502	
	long	lTotalAmount;  //4.0.10.130  //4.0.11.504
	long	lPreviousFueledAmount;  //4.0.10.130
	long	lLoyaltyVolumeLimit;	//4.0.20.450 66977 //4.0.21.501
	GRADE_LIMITS GradeAmountLimit[MAX_POSITIONS_PER_PUMP];
	GRADE_LIMITS GradeVolumeLimit[MAX_POSITIONS_PER_PUMP];

	// C'tor
	CMD_STRUCT_AUTHORIZE_LIMIT() : nLevel(0), lGradePrice(0), lMaxValue(0), lMaxVolume(0), lFlags(0), lSyncNumber(0),
									lTotalAmount(0), lPreviousFueledAmount(0), lLoyaltyVolumeLimit(0)
	{
		memset(&lDiscountArray, 0, sizeof(lDiscountArray));
		memset(&sDiscountType, '0', 2);
		memset(&GradeAmountLimit, '0', MAX_POSITIONS_PER_PUMP);
		memset(&GradeVolumeLimit, '0', MAX_POSITIONS_PER_PUMP);
	}

} CMD_STRUCT_AUTHORIZE_LIMIT;


typedef struct 
{
	LONG	lRes;
	LONG	lResExt;
} CMD_STRUCT_RESERVE_PUMP;

typedef struct 
{
	LONG	lRes;
	LONG	lResExt;
} CMD_STRUCT_RELEASE_PUMP;


typedef struct 
{
	LONG	lTrsNumber;
	LONG	lPumpStatus;
	LONG	lRes;
	LONG	lResExt;
} CMD_STRUCT_TRS_CANCEL;

typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	CARD_SALE_DATA			data;	
	CARD_SALE_EXTRA_DATA	extData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	//CARD_SALE_ALL2	data;	

} CMD_STRUCT_PAP_SWIPE_RESULT;

typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	CARD_SALE_DATA			data;	
	CARD_SALE_EXTRA_DATA	extData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	CARD_SALE_EXTRA_DATA3_  extData3;
}CMD_STRUCT_PAP_SWIPE_RESULT3;


typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	CARD_SALE_DATA			data;	
	CARD_SALE_EXTRA_DATA	extData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	CARD_SALE_EXTRA_DATA3_  extData3;
	CARD_SALE_EXTRA_DATA4_  extData4;
	CARD_SALE_EXTRA_DATA5_  extData5;
	CARD_SALE_EXTRA_DATA6_  extData6;
}CMD_STRUCT_PAP_SWIPE_RESULT3_EX; //4.0.27.45


//4.0.14.500
typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	BYTE					data[MAX_XML_BUF_LEN];	
} CMD_STRUCT_PAP_SWIPE_RESULT_XML;

typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	CARD_SALE_DATA			data;	
	CARD_SALE_EXTRA_DATA	extData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
}CMD_STRUCT_CARD_INFO_RESULT;

typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	BYTE					data[MAX_XML_BUF_LEN];	
}CMD_STRUCT_CARD_INFO_RESULT_XML;

typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	CARD_SALE_DATA			data;	
	CARD_SALE_EXTRA_DATA	extData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT2		trs;	
} CMD_STRUCT_PAP_SWIPE_RECEIPT;


typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	CARD_SALE_DATA			data;	
	CARD_SALE_EXTRA_DATA	extData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT5		trs;	
} CMD_STRUCT_PAP_SWIPE_RECEIPT_EX;	//TD 38654


//4.0.5.0
typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	CARD_SALE_DATA			data;	
	CARD_SALE_EXTRA_DATA	extData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT2		trs;	
	CARD_SALE_EXTRA_DATA3_  extData3;
}CMD_STRUCT_PAP_SWIPE_RECEIPT3;
//4.0.5.0

typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	CARD_SALE_DATA			data;	
	CARD_SALE_EXTRA_DATA	extData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT5		trs;	
	CARD_SALE_EXTRA_DATA3_  extData3;
	CARD_SALE_EXTRA_DATA4_  extData4;  //4.0.27.45
	CARD_SALE_EXTRA_DATA5_  extData5;  //4.0.27.45
	CARD_SALE_EXTRA_DATA6_  extData6;
}CMD_STRUCT_PAP_SWIPE_RECEIPT3_EX; //TD 38654

typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	TAG_PUMP_TRANSACT2		trs;	
	BYTE					data[MAX_XML_BUF_LEN];	
}CMD_STRUCT_PAP_SWIPE_RECEIPT_XML;


typedef struct 
{
	LONG					lDeviceNumber;
	LONG					lSeqNumber;
	LONG					lAnswer;
	LONG					lFlags;
	TAG_PUMP_TRANSACT5		trs;	
	BYTE					data[MAX_XML_BUF_LEN];	
}CMD_STRUCT_PAP_SWIPE_RECEIPT_XML_EX;	//TD 38654


typedef struct 
{
	LONG			lPumpNumber;
	short			nNozzle;
	short			nGrade;
	short 			nFlags;
	LONG			lPriceA;
	LONG			lPriceB;	
} CMD_STRUCT_GRADE_PRICE;


typedef struct 
{
	LONG			lPumpNumber;
	short			nNozzle;
	short			nGrade;
	short 			nFlags;
	LONG			lPriceA;
	LONG			lPriceB;
	TAG_SYSTEMTIME	sDateTime;
} CMD_STRUCT_GRADE_PRICE_WITH_TIME; //TD 58094

typedef struct 
{
	LONG	lStatus;
} CMD_STRUCT_TANK_READING;

typedef struct 
{
	char szTableName[MAX_TABLE_NAME];
	long lKey1;
	long lKey2;
	char szFieldName[MAX_FIELD_NAME]; 
	short nFieldType;
	char szFieldVal[MAX_FIELD_VAL]; 
	long lVal;

} CMD_STRUCT_UPDATE_PARAM;

typedef struct 
{
	LONG				lSyncNumber;
	LONG				lWashProgram;
	LONG				lFlags;
	TAG_CAR_WASH_DATA	carWashData;
} CMD_STRUCT_CAR_WASH;

typedef struct 
{
	LONG			lPosNumber;
	char			sTrack2[ENCRYPTED_DATA_SIZE]; //4.0.20.501 TD-60236
	long			lIndex;
	LONG			lFlags;
}CMD_STRUCT_FORMAT_RECEIPT;


//4.0.9.503
typedef struct 
{
	LONG					lOriginTrans;
	LONG					lDeviceNumber;
	LONG					lFlags;
	char					data[REPORT_TEXT_LENGTH];
	LONG					lTextLength;
}CMD_STRUCT_PRINT_REPORT;


//4.0.9.503
typedef struct 
{
	LONG			lPosNumber;
	LONG			lNumber;		
	LONG			lType;	
	LONG			lFlags;
}CMD_STRUCT_FORMAT_REPORT;

//4.0.5.39
typedef struct 
{
    long			lTimeOut;
	char			sDisplayMsg [MAX_TERMINAL_MSG_LEN];
	long            lDisplayMsgLen;
} CMD_STRUCT_DISPLAY_MSG;

//3.2.0.3 Casey's Eyal
typedef struct TLS_Command
{
	LONG lFlags;
	LONG lCommand;
	LONG lTankNumber;
	BYTE sCommand[MAX_TLS_CMD_LEN];
} CMD_STRUCT_TLS;

typedef struct
{
	LONG lPumpNumber;
	LONG lSessionId;
	LONG lFlags;
	OLA_DEBIT_PROCESS_INFO		cOLaDebitInfo;	
}CMD_STRUCT_DEBIT_PROCESS;



//4.0.2.29
typedef struct
{
	long lTableId;
	long lSubTableId;
	long lSubTableIdExt;
	long lFlags;
} CMD_STRUCT_RECONFIGURATION_EVENT;


typedef struct   ///CR 442586
{
	long lDeviceNumberMask1;
	long lDeviceNumberMask2;
	long lTableId;
	long lSubTableId;
	long lSubTableIdExt;
	long lFlags;
} CMD_STRUCT_SMART_RECONFIGURATION_EVENT;

//4.0.5.24

typedef struct
{
	LONG lTankNumber;	
	LONG lFlags;
	TAG_TGDTankReading  cTGDTankReading;	
}CMD_STRUCT_TANK_GAUGE_EMULATION_READING;

typedef struct
{
	LONG lTankNumber;	
	LONG lFlags;
	TAG_TGDDeliveryData cTGDDeliveryData;
}CMD_STRUCT_TANK_GAUGE_EMULATION_DELIVERY;

//4.0.5.24

//4.0.19.504 - 47559 Start
typedef struct
{
	long lPumpNumber;
	long lPresetLimitType;
	long lPresetLimitValue;
} CMD_STRUCT_SET_PRESET_LIMIT_EVENT;

typedef struct
{
	long lEventId;
	long lFlags;
	long lParam1;
} CMD_STRUCT_GENERAL_SYSTEM_EVENT;

typedef struct 
{
	LONG	lTrsNumber;
	LONG	lState;
} CMD_STRUCT_RELEASE_CONTROLLER_TRS;

//4.0.19.504 - 47559 End

// Commands used internaly by the server 
// and/or translated to generic server interface

//GPI Event 4.0.12.501
typedef struct
{
	LONG lPumpNumber;	
	LONG lRes;
	LONG lStatus;
	LONG lResExt;
	LONG lFlags;
	LONG lParam1;	//4.0.19.504 - 47559
	LONG* pVal; //4.0.12.503
}CMD_STRUCT_SET_PUMP_STATUS; // 4.0.19.1750 TD  349356

typedef struct
{
	LONG lPumpNumber;
	LONG lFlags;
	LONG* pVal;
}CMD_STRUCT_SET_PROMOTIONS_EVENT;

typedef struct
{
	LONG lPumpNumber;
	LONG lRes;
	LONG lStatus;
	LONG lResExt;
	LONG lFlags;
	LONG lLimit;
	short nGrade;
	short nPosition;

}CMD_STRUCT_SET_PUMP_STATUS_EX; //CR 444830


typedef struct  
{
	LONG lPumpNumber;
	LONG lTrsNumber;
	LONG lNozzle;
	LONG lVolume;
	LONG lValue;
	LONG lLevel;
	LONG lGradePrice;
	LONG lGrade;
	LONG lFlags;
	LONG lTotalVal;
	LONG lResExt;
	TAG_EXTRA_INFO	   cTagExtraInfo;
	SALE_EXTRA_DETAILS cSaleExtraDetails; //4.0.19.504 - 47559
	LONG lRetCode;
}CMD_STRUCT_PAK_READY_TRS;

//4.0.12.503
typedef struct 
{
	LONG lDeviceNumber; 
	LONG lSeqNumber;
	LONG lFlags;
	CARD_SALE_ALL3 cardDataAll3;
	LONG* pVal;
}CMD_STRUCT_PAP_SWIPE;

typedef struct 
{
	LONG lPumpNumber;
    LONG lDeviceNumber;
    LONG lSeqNumber;
    LONG lRetcode;
    LONG lFlags;
	CARD_SALE_ALL3 cardDataAll3;
	BYTE sData[MAX_XML_BUF_LEN];	
}CMD_STRUCT_SEND_PAP_SWIPE_RESULT;

typedef struct 
{
	LONG lSeqNumber;
	LONG lTrsNumber; 
	SHORT nNozzle;
	LONG lVolume;
	LONG lValue;
	SHORT nLevel;
	LONG lGradePrice;
	SHORT nGrade;
	LONG lFlags;
	CARD_SALE_ALL3 cardDataAll3;
	BSTR* sRetData;
	LONG* pVal;
}CMD_STRUCT_PAP_READY_TRS;

typedef struct 
{
	BSTR sParamPosition;
	LONG lKey1;
	LONG lKey2;
	SHORT nFiledType;
	BSTR* sVal;
	LONG* lVal;
	BOOL bCreate;
	LONG* pVal;
}CMD_STRUCT_GET_PARAM3;

typedef struct 
{
	LONG lDevice;
	LONG lFlags;
	BSTR *sVal;
	LONG* pVal;
}CMD_STRUCT_DISABLE_ENABLE_PD_PANEL;//CR 404591

//4.0.19.504 - 47559
typedef struct 
{
	LONG lPumpNumber;
	LONG lTrsNumber;
	LONG lPumpStatus;
	LONG lRes;
	LONG lResExt;
}CMD_STRUCT_SEND_CANCEL_TRS;


//4.0.19.504 - 47559
typedef struct 
{
	LONG lPumpNumber;
	LONG lFlags;
}CMD_STRUCT_SEND_SHIFT_STARTED;

//4.0.19.504 - 47559
typedef struct 
{
	LONG lDevice;
	LONG lSyncNumber;
	LONG lFlags;
}CMD_STRUCT_SEND_WASH_CODE_READY;

//4.0.19.504 - 47559
typedef struct 
{
	LONG lPumpNumber;
	LONG lPosNumber;
	BYTE sBuffer[MAX_FIELD_VAL];
	LONG lBuffSize;	
	LONG lFlag;
	//BYTE sTrack2[TRACK_2_DATA_SIZE];
}CMD_STRUCT_GET_RECEIPTS_ROW_DATA;

//4.0.19.504 - 47559
typedef struct 
{
	LONG lPumpNumber;
	BYTE sInvoiceNumber[10];
	LONG lPosNumber;
}CMD_STRUCT_GET_FORMATED_RECEIPT_DATA;

typedef struct
{
	LONG	lPumpNumber;
	LONG	lTrsNumber;
	LONG	lState;
	LONG	lFlags;
} CMD_STRUCT_TRS_STATE_CHANGED;	//?.??.?.?? - TD 335186

#define MAX_CMD_DATA_SIZE	3100//2100//1200   //4.0.0.6   //4.0.5.0


enum Command {
				CMD_NULL,				// Controller work as
				CMD_AUTHORIZE_LIMIT,	// Master & Slave -> IGCIEvents
				CMD_STOP_PUMP,			// Master & Slave -> IGCIEvents
				CMD_RESTART_PUMP,		// Master & Slave -> IGCIEvents
				CMD_RESET_CONTROLLER,	// Master & Slave -> IGCIEvents

				CMD_GET_PUMP_STATUS,	// Master & Slave -> IGCIEvents

				CMD_RESERVE_PUMP,		// Master & Slave (for pre pay) -> IGCIEvents
				CMD_SET_PUMP_LIMITS,	// Master & Slave (for pre pay) -> IGCIEvents
				CMD_RELEASE_PUMP,		// Master & Slave (for pre pay) -> IGCIEvents
			
				CMD_SET_PUMP_MODE,		// Master -> IGCIEvents

				CMD_TRS_OFFER,			// Master -> IGCIEvents , Master -> Internaly
				CMD_TRS_RELEASE,		// Master -> IGCIEvents , Master -> Internaly
				CMD_TRS_FORCE_RELEASE,	// Master -> IGCIEvents , Master -> Internaly
				CMD_TRS_ACCEPTED,		// Master -> IGCIEvents , Master -> Internaly
				CMD_TRS_PAID,			// Master -> IGCIEvents , Master -> Internaly

				CMD_TRS_CANCEL,			// Master & Slave (for pre pay) -> IGCIEvents
				CMD_SWIPE_RESULT,
				CMD_SWIPE_RECEIPT,
				CMD_GET_PUMP_TOTALS,
				CMD_GET_TANK_READING,
				CMD_SET_GRADE_PRICE,
				CMD_GET_DELIVERY_REPORT,
				CMD_UPDATE_PARAM,
				CMD_WATCHDOG,
				CMD_GET_NEW_WASH_CODE,
				CMD_GET_CAR_WASH_STATUS,
				CMD_CANCEL_WASH_CODE,
				CMD_WET_STOCK_DATA,
				CMD_RE_CONFIGURE,
				CMD_FORMAT_RECEIPT,
				CMD_PRINT_REPORT,
				CMD_SET_REALTIME_CLOCK,
				CMD_TLS,    //Eyal 3.2.0.2 Casey's
				CMD_DEBIT_PROCESS,
				CMD_GENERAL_SYSTEM_EVENT,
				CMD_SET_PUMP_MODE2,  //4.0.2.21
				CMD_RECONFIGURE_EVENT,  //4.0.2.29
				CMD_CARD_INFO_RESULT, //4.0.3,21
				CMD_TANK_GAUGE_EMULATION_READING,   //4.0.5.24
				CMD_TANK_GAUGE_EMULATION_DELIVERY,   //4.0.5.24
				CMD_DISPLAY_MSG,					 //4.0.5.39
				CMD_FORMAT_REPORT,					 //4.0.9.555
				CMD_SET_PUMP_MODE3,				//4.0.9.506
				CMD_SET_PUMP_STATUS,			//4.0.12.501 GPI Event
				CMD_SET_PAK_READY_TRS,			//4.0.12.501 GPI Event
				CMD_PAP_SWIPE,					//4.0.12.503 GPI 
				CMD_PAP_READY_TRS,				//4.0.12.503 GPI
				CMD_GET_PARAM3,					//4.0.12.503 GPI Event
				CMD_SET_GRADE_PRICE_WITH_TIME,   // From CTRL Master -> IGCIEvents
				CMD_SEND_CANCEL_TRS,			 //4.0.19.504 - 47559
				CMD_RELEASE_CONTROLLER_TRS,      //4.0.19.504 - 47559
				CMD_SEND_GET_RECEIPTS_ROW_DATA,  //4.0.19.504 - 47559
				CMD_GET_FORMATED_RECEIPT_DATA,   //4.0.19.504 - 47559
				CMD_SEND_WASH_CODE_READY,		 //4.0.19.504 - 47559
				CMD_SEND_SET_PRESET_LIMIT,		 //4.0.19.504 - 47559
				CMD_CAR_WASH_GENERAL_REQUEST,	 //4.0.23.504 - 76432
				CMD_CAR_WASH_VALIDITY,			//4.0.26.501 144221
				CMD_TRS_STATE_CHANGED,			//?.??.?.?? - TD 335186
				CMD_SET_PROMOTIONS_EVENT,		//4.0.19.1750 TD  349356
				CMD_DISABLE_PD_PANEL,			//CR 404591
				CMD_ENABLE_PD_PANEL,			//CR 404591
				CMD_SET_PUMP_STATUS_EX,			//CR 444830 
				CMD_SMART_RECONFIGURE_EVENT,	//CR 442586
};


class CCmdInfo 
{
public:
	char * GetDataPtr();
	void SetNumber(long lNumber);
	long GetNumber();
	void SetSrc(LONG lSrc);
	LONG GetSrc();
	void SetParameter(LONG lParam);
	LONG GetParameter();
	void SetCommand(Command lCommand);
	Command GetCommand();
	CCmdInfo();		// default constructor
	CCmdInfo(Command lCommand, LONG lSrc, LONG lNumber, LONG lParam);
	CCmdInfo(Command lCommand, LONG lSrc, LONG lNumber, LONG lParam,LPVOID pData, SHORT nDataLen);
	CCmdInfo(const CCmdInfo& cmdInfo);
	void operator =( const CCmdInfo& cmdInfo );

protected:
	long	m_lNumber;
	Command m_lCommand;
	LONG	m_lSrc;
	LONG	m_lParameter;
	char	m_Data[MAX_XML_BUF_LEN]; //MAX_CMD_DATA_SIZE
};

#endif