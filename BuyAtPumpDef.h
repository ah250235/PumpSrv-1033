// BuyAtPumpDef.h: definitions for BuyAtPump.exe
//
// Basic protocol:
// ===============
// 1. A client should connect to ITerminalUI interface.
// 2. First action is to call to InitAll(..) function.
// 3. For each active terminal the client should call to InitTerminal(..)
// 4. Changing one of the default strings will be through SetMenuString(..)
// 5. When a sale session is starting on a terminal, the client should 
//    update StartSaleSession(..)
// 6. From that moment the client should update ITerminalUI in every 
//    KeyPresed(..), PrinterStatusUpdate(..), and CardSwiped(..) or CardSwipe2(..)
// 7. the client will catch ITerminalUI GeneralEvent(..) and act accordingly.
// 8. When the session is closed the cliet should update - CloseSaleSession(..)
// 9. When disconect from B@P the client MUST(!!!) call to CloseAllConnections
//    for a taken care shut down.
//
// functions prototypes:
// =====================
//	InitAll(/*[in]*/BSTR sPath,/*[in]*/long lTimeOutVal,/*[in]*/long lFlags,/*[out,retval]*/ long * pRet)
//  sPath - DynamicMenu file path (example : "D:\Data")
//  lTimeOutVal - TimeOut value in seconds
//  lFlags - Table # 8
//  pRet - Table # 7
//
//	InitTerminal(/*[in]*/long lTerminalNum, /*[in]*/long lPumpMap1, /*[in]*/long lPumpMap2, /*[in]*/long lDisplayWidth,/*[in]*/long lDisplayLines,/*[in]*/long lFlags,/*[out,retval]*/ long *pVal)
//  lTerminalNum - Terminal ID 
//  lPumpMap1- Connected Pumps
//  lPumpMap2- Connected Pumps
//  lDisplayWidth - Display width
//  lDisplayLines - Display lines
//  lFlags - Not in use yet
//  pVal - Table # 7
//
//	StartSaleSession(/*[in]*/long lTerminalNum,/*[in]*/long lPumpNum,/*[in]*/BSTR sCashier,/*[in]*/ long lCashierID,/*[in]*/short lLanguage, /*[in]*/ long lFlags,/*[out,retval]*/ long *pVal)
//  lTerminalNum - Terminal ID 
//  lPumpNum - Pump number
//  sCashier - Cashier data
//  lCashierID - Cashier identifier
//  lLanguage - The relevant language for this session (1 or 2 for now).
//  lFlags - Table # 12
//  pVal - Table # 7
//  
//	GetUIScheme(/*[in]*/long lTerminalNum,/*[in]*/ long lPumpNum,/*[out]*/long * lFlags,/*[out]*/BSTR *sMenu,/*[out]*/long * lKeyInputLen,/*[out,retval]*/ long *pVal)
//  lTerminalNum - Terminal ID 
//  lPumpNum - Pump number
//  lFlags - How to handle the terminal devices according to table # 3
//  sMenu - Display data
//  lKeyInputLen - how many key we are expecting to get in this state. 
//  pVal - Table # 7
//
//	KeyPressed(/*[in]*/long lTerminalNum,/*[in]*/long lPumpNum,/*[in]*/long lKeyType,/*[in]*/ long lTypeValue,/*[in]*/ long lFlags,/*[out,retval]*/ long *pVal)
//  lTerminalNum - Terminal ID 
//  lPumpNum - Pump number
//  lKeyType - the key that was pressed (Table # 2)
//  lTypeValue - the value that was entered when lKeyType =BAP_KB_NUMERIC_INPUT (one digit or more)
//  lFlags - Not in use yet
//  pVal - Table # 7
//  
//	CardSwiped(/*[in]*/long lTerminalNum,/*[in]*/long lPumpNum,/*[in]*/BSTR sTrackData,/*[in]*/long lFlags,/*[out,retval]*/ long *pVal)
//  lTerminalNum - Terminal ID
//  lPumpNum - Pump number
//  sTrackData - Track2
//  lFlags - Table # 10
//  pVal - Table # 7
//
//  CardSwipe2(/*[in]*/ long lTerminalNum,/*[in]*/ long lPumpNum,/*[in]*/ BSTR sTrack1,/*[in]*/ BSTR sTrack2,/*[in]*/ long lFlags,/*[out,retval]*/  long *pVal);
//  lTerminalNum - Terminal ID
//  lPumpNum - Pump number
//  sTrack1 - Track1
//  sTrack2 - Track2
//  lFlags - Table # 10
//  pVal - Table # 7
//  
//	SetMenuString(/*[in]*/long lStringID, /*[in]*/ BSTR sNewStr,/*[out,retval]*/ long *pVal)
//  lStringID - Table # 1
//  sNewStr -  new string
//  pVal - Table # 7
//  
//	PrinterStatusUpdate (/*[in]*/long lTerminalNum,/*[in]*/long lPumpNum,/*[in]*/long lFlags,/*[out,retval]*/long* pval)
//  lTerminalNum - Terminal ID
//  lPumpNum - Pump number
//  lFlags - Table # 9
//  pVal - Table # 7
//  
//  GetRawData(/*[in]*/long lTerminalNum,/*[in]*/long lPumpNum,/*[out]*/BSTR* sRawData,/*[in]*/long lFlags,/*[out,retval]*/ long* pVal)
//  lTerminalNum - Terminal ID
//  lPumpNum - Pump number
//	sRawData - struct BAP_ALL_ITEMS_FOR_ RECEIPT
//  lFlags - Table # 19
//  pVal - Table # 7
//
//  AddFuelItemToTicket(/*[in]*/long lTerminalNum,/*[in]*/long lPumpNum,/*[in]*/BSTR sItemData,/*[in]*/long lAttID,/*[in]*/long lFlags,/*[out,retval]*/long* lRet)
//  lTerminalNum - Terminal ID
//  lPumpNum - Pump number
//  sItemData - struct BaP_FUEL_ITEM
//  lAttID - the ID of the attendant that authorize this fuel trs
//  lFlags - Not in use yet
//  lRet - Table # 7
//
//	GeneralEvent(/*[in]*/long lEventID, /*[in]*/long lTerminalNum, /*[in]*/long lFlags);
//  lEventID - Table # 5
//  lTerminalNum - Terminal ID
//  lFlags - Table # 16
//
//	CloseSaleSession(/*[in]*/long lTerminalNum,/*[in]*/long lPumpNum,/*[in]*/ long lFlags,/*[out,retval]*/ long *pVal)
//  lTerminalNum - Terminal ID
//  lPumpNum - Pump number
//  lFlags - Table 17
//  lRet - Table # 7
//
//	SignIn(/*[in]*/ long lEmployeeId,/*[in]*/ BSTR sPassword,/*[in]*/ long lForce,/*[out,retval]*/ long* lRet);
//  lEmployeeId - Employee ID
//  sPassword - Employee Password
//  lForce - Not in use yet
//  lRet - Table # 7
//
//	SignOut(/*[in]*/ long lEmployeeId,/*[in]*/ long lFlags,/*[out,retval]*/ long * lRetVal);
//  lEmployeeId - Employee ID
//  lFlags - Not in use yet
//  lRetVal - Table # 7
//
//	EndOfShift(/*[out,retval]*/ long * lRetVal);
//  lRet - Table # 7
//
//  RequestReport(/*[in]*/  long lTerminalId, /*[in]*/  long lAttId, /*[in]*/  long lReportType,/*[in]*/ long lFlags, /*[out,retval]*/  long * lRetVal);
//  lTerminalId - Terminal ID
//  lAttId - Attendant ID
//  lReportType - Table # 13
//  lFlags- Not in use yet
//  lRetVal - Table # 7
//
//  GetReportData(/*[in]*/ long lTerminalNum, /*[in]*/  long lAttID,/*[in]*/ long lReportType,/*[out]*/ BSTR * sReportData,/*[in]*/ long lFlags,/*[out,retval]*/ long *lRetVal);
//  lTerminalNum - Terminal ID
//  lAttID - Attendant ID
//  lReportType - Table # 13
//  sReportData - The data in xml format string OR in BAP_ATTENDENT_TOTALS struct
//  lFlags- Not in use
//  lRetVal - Table # 7
//
//	CloseAllConnections(/*[out,retval]*/long *lRet)
//	lRet - Table # 7
//
//  SetExtraInfo([in]long lTerminalNum,[in]long lPumpNum,[in]long lData,[in]BSTR sData,[in]long lFlags,[out,retval]long* lRet)
//  lTerminalNum - Terminal ID
//  lPumpNum - Pump number
//  lData - Not in use yet
//  sData - struct BAP_DEBIT_PROCESS_INFO
//  lFlags - Not in use yet
//  lRet - Table # 7
//
//  GetExtraInfo([in]long lTerminalNum,[in]long lPumpNum,[out]long *lData,[out]BSTR* sData,[in]long lInFlags,[out]long* lOutFlags,[out,retval]long* lRet)		
//  lTerminalNum - Terminal ID
//  lPumpNum - Pump number
//  lData - Not in use yet
//  sData - struct BAP_DEBIT_PROCESS_INFO or Amount data according to lInFlags
//  lInFlags - Table # 15
//  lOutFlags - Table # 15
//  lRet - Table # 7
//
//	StartSaleSession2(/*[in]*/long lTerminalNum,/*[in]*/long lPumpNum,/*[in]*/BSTR sCashier,/*[in]*/ long lCashierID,/*[in]*/short lLanguage,/*[in]*/long lAmountLimit,/*[in]*/BSTR sOLAData, /*[in]*/ long lFlags,/*[out,retval]*/ long *pVal)
//  lTerminalNum - Terminal ID 
//  lPumpNum - Pump number
//  sCashier - Cashier data
//  lCashierID - Cashier identifier
//  lLanguage - The relevant language for this session (1 or 2 for now).
//  lAmountLimit - amount limit supplied from the Pre authorized session
//	sOLAData - OLA buffer from the Pre authorized session
//  lFlags - Table # 12
//  pVal - Table # 7
//
//	KeyPressed2(/*[in]*/long lTerminalNum,/*[in]*/long lPumpNum,/*[in]*/long lKeyType,/*[in]*/ long lTypeValue,/*[in]*/ BSTR sValue,/*[in]*/ long lFlags,/*[out,retval]*/ long *pVal)
//  lTerminalNum - Terminal ID 
//  lPumpNum - Pump number
//  lKeyType - the key that was pressed (Table # 2)
//  lTypeValue - the value that was entered when lKeyType =BAP_KB_NUMERIC_INPUT (one digit or more) as long
//          	 in case it has less then 10 digits.
//  sValue - the value that was entered when lKeyType =BAP_KB_NUMERIC_INPUT (one digit or more) as string
//  lFlags - Not in use yet
//  pVal - Table # 7
//
//  CheckLimitsStatus(/*[in]*/long lTerminalNum,/*[in]*/long lOperatorId,/*[in]*/long lFlags,/*[out,retval]*/ long *pVal)
//  lTerminalNum - Terminal ID 
//  lOperatorId - operator ID
//  lFlags - function number for PMT::IsAllowed check
//
//  SetCarWashAccessCode(/*[in]*/long lTerminalNum,/*[in]*/long lPumpNum, /*[in]*/BSTR sAccessCode,/*[in]*/ long lFlags,/*[out,retval]*/ long *lRetVal)
//  lTerminalNum - Terminal ID 
//  lPumpNum - Pump number
//  sAccessCode - Wash code that was generated at the pump
//  lFlags - Not in use yet
//  pVal - Table # 7
//
//  ReloadMenus ([in]BSTR sPath,[in] long lFlags ,[out,retval]long * lRetVal);
//  sPath - the xml menus folder
//  lFlags - Not in use yet
//  pVal - Table # 7
//
//  SwitchLanguage([in]long lPumpNum,[in]long lTerminalNum,[in]long lLangID,[in]long lFlags,[out,retval]long * lRet);
//  lTerminalNum - Terminal number 
//  lPumpNum - Pump number
//  lLangID - New language
//  lFlags - Not in use yet
//  pVal - Table # 7
//
//  ScannerCmd([in]long lTerminalNum,[in]long lPumpNum,[in] BSTR sData,[in] long lFlags,[out,retval] long *pVal);
//  lTerminalNum - Terminal number 
//  lPumpNum - Pump number
//  BSTR sData - Item's barcode
//  lFlags - Not in use yet
//  pVal - Table # 7
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __BAP_DEF_H_
#define __BAP_DEF_H_

#include "GeneralStruct.h"

#pragma pack(1)

#define BAP_MAX_BUFFER_LEN						256
#define BAP_MAX_REPORT_LEN						32768
#define BAP_NMP_LIST							1500
#define MAX_ITEMS_NUM							10
#define MAX_TAX									5
#define OLA_RESULT_MSG							81
#define OLA_OPEN_NMP_RESULT_MSG					113
#define CUSTOMER_NAME_STR_LEN					20
#define MAX_ITEMS_NUM_AFTER_SAVING_FUEL_PLACE	MAX_ITEMS_NUM - 2 //Save place for fuel item + service fee item 
#define BAP_UPDATE_MENUS_TIMEOUT				295000			  // 295sec, timeout for rolling update of dynamic xml menus
#define MAX_OPEN_PARAMS_LENGTH					100
#define	DEFAULT_MAX_SCREEN_MSG_LEN				260
#define TERMINAL_INIT_MAX_RETRIES               2

// Table # 1: lStringID parameter in SetMenuString(..) function
enum BAP_DISPLAY_MESSAGES 
{
	BAP_DISPLAY_MORE_ITEMS,			//0
	BAP_DISPLAY_QUANTITY,			//1
	BAP_DISPLAY_TENDER,				//2
	BAP_DISPLAY_THANK_YOU,			//3
	BAP_DISPLAY_SELECTED,			//4
	BAP_DISPLAY_YES_NO,				//5
	BAP_DISPLAY_CURRENCY_SIGN,		//6
	BAP_DISPLAY_CANCEL,				//7
	BAP_DISPLAY_TRY_AGAIN,			//8
	BAP_DISPLAY_SEE_CASHIER,		//9
	BAP_DISPLAY_MEMBER,				//10
	BAP_DISPLAY_PAYMENT,			//11
	BAP_DISPLAY_TOTAL_PURCH,		//12
	BAP_DISPLAY_ACCOUNT,			//13
	BAP_DISPLAY_PIN,				//14
	BAP_DISPLAY_ODOMETER,			//15
	BAP_DISPLAY_UNIT,				//16
	BAP_DISPLAY_TRS_COMPLETE,		//17
	BAP_DISPLAY_RECEIPT,			//18
	BAP_DISPLAY_CAR_REG,			//19
	BAP_DISPLAY_CANOT_ADD_ITEM,		//20
	BAP_DISPLAY_ONE_MOMENT,			//21
	BAP_DISPLAY_PROCESS_FAILED,		//22
	BAP_DISPLAY_FINAL_AMOUNT,		//23
	BAP_DISPLAY_ENTER_PLU,			//24
	BAP_DISPLAY_ACCOUNT_NUM,		//25
	BAP_DISPLAY_ABA_NUM,			//26
	BAP_DISPLAY_ACCOUNT_NUM_EX,		//27
	BAP_DISPLAY_BRANCH,				//28
	BAP_DISPLAY_CHECK_CODE,			//29
	BAP_DISPLAY_CHECK_DATE,			//30
	BAP_DISPLAY_CHECK_SERIAL,		//31
	BAP_DISPLAY_COUNTRY_CODE,		//32
	BAP_DISPLAY_EXP_DATE,			//33
	BAP_DISPLAY_INITIAL_CODE,		//34
	BAP_DISPLAY_REDEEM_POINTS,		//35
	BAP_DISPLAY_TRANS_REF,			//36
	BAP_DISPLAY_USE_CREDIT,			//37
	BAP_DISPLAY_ENTER_COUPON_BARCODE,	//38
	BAP_DISPLAY_ENTER_COUPON_SERIAL,	//39
	BAP_DISPLAY_ENTER_REFERENCE,		//40
	BAP_DISPLAY_AMOUNT,					//41
	BAP_DISPLAY_CHANGE,					//42
	BAP_DISPLAY_AUTH_NUM,				//43
	BAP_DISPLAY_DRIVER_ID,				//44
	BAP_DISPLAY_CHANGE_PMNT_METHOD,		//45
	BAP_DISPLAY_ENTER_UNIT_PRICE,		//46
	BAP_DISPLAY_SCANNER_QUANTITY,       //47
	BAP_DISPLAY_CUSTOMER_REFERENCE,		//48
	BAP_DISPLAY_ASK_DEBT_AMOUNT,		//49
	BAP_DISPLAY_FAILED_TO_VOID_ITEM,	//50
	BAP_DISPLAY_SUCCESS_TO_VOID_ITEM,	//51
	// !!! When adding a string change BAP_MAX_MESSEGES 
};


// Table # 2: lKeyCode parameter in KeyPressed(..) function
enum BAP_KEYBOARD_KEYS
{
	BAP_KB_INVALID_KEY,					//0
	BAP_KB_NUMERIC_INPUT,				//1			
	BAP_KB_FUNCTION_KEY_PRESSED,		//2
	BAP_KB_CANCEL_PRESSED,				//3
	BAP_KB_MENU_PRESSED,				//4
	BAP_KB_YES_PRESSED,					//5
	BAP_KB_NO_PRESSED,					//6
	BAP_KB_LIMIT_VALUE_PRESSED,			//7
	BAP_KB_LIMIT_VOLUME_PRESSED,		//8
	BAP_KB_CASH,						//9
	BAP_KB_CREDIT,						//10
	BAP_KB_DEBIT,						//11
	BAP_KB_HELP,						//12
	BAP_KB_REPRINT_RECEIPT,				//13
	BAP_KB_START_BUTTON,				//14
	BAP_KB_PAY_OUTSIDE,					//15
	BAP_KB_PAY_INSIDE,					//16
	BAP_KB_CLEAR,						//17
	BAP_KB_MENU_HOOK,					//18
	BAP_KB_NUMERIC_INPUT_WITH_DECIMAL,	//19
};	


// Table # 3: GetUIScheme(..) lFlags 
#define BAP_TERMINAL_KB_MASK_NUMBERS    					0x00000001
#define BAP_TERMINAL_KB_1    								0x00000002
#define BAP_TERMINAL_KB_2    								0x00000004
#define BAP_TERMINAL_KB_3    								0x00000008
#define BAP_TERMINAL_KB_4    								0x00000010
#define BAP_TERMINAL_KEY_YES								0x00000020	
#define BAP_TERMINAL_KEY_NO									0x00000040
#define BAP_TERMINAL_KEY_LIMIT_VALUE						0x00000080
#define BAP_TERMINAL_KEY_LIMIT_VOLUME						0x00000100
#define BAP_TERMINAL_KEY_MENU								0x00000200
#define BAP_TERMINAL_KEY_CANCEL								0x00000400
#define BAP_TERMINAL_CARDREADER_ON							0x00002000
#define BAP_TERMINAL_CARDREADER_OFF							0x00004000
#define BAP_TERMINAL_CASHACCEPTOR_ON						0x00008000
#define BAP_TERMINAL_CASHACCEPTOR_OFF						0x00010000
#define BAP_TERMINAL_WAIT_FOR_TERMINATOR					0x00020000
#define BAP_TERMINAL_KEY_CASH								0x00040000
#define BAP_TERMINAL_KEY_LIMIT								0x00080000
#define BAP_TERMINAL_KEY_CLEAR								0x00100000
#define BAP_TERMINAL_KEY_MENU_HOOK							0x00200000
#define BAP_TERMINAL_KEY_SCANNER_ON							0x00400000
#define BAP_TERMINAL_KEY_SCANNER_OFF						0x00800000
#define BAP_TERMINAL_KEY_ALPHANUMERIC						0x01000000
#define BAP_TERMINAL_KEY_ASTERISK							0x02000000
#define BAP_TERMINAL_KEY_DECIMAL_POINT						0x04000000
#define BAP_TERMINAL_KEY_OK_NEEDED							0x08000000
#define BAP_TERMINAL_AVI_ALLOW								0x10000000


// Table # 4: Definition for terminal display states
enum BAP_TERMINAL_STATES
{
	// We will reach these stages due to actions from CL
	BAP_TRM_NONE,					// 0   
	BAP_TRM_SHOW_SUB_MENUS,			// 1 
	BAP_TRM_WAIT_USER_INPUT,		// 2 
	BAP_TRM_MORE_ITEMS,				// 3
	BAP_TRM_GET_QUANTITY,			// 4 
	BAP_TRM_CONFIRMATION,			// 5
	BAP_TRM_END,					// 6 
	BAP_TRM_SHOW_SUB_TENDER_MENUS,	// 7
	BAP_TRM_WAIT_TENDER_INPUT,	    // 8
	BAP_TRM_MEMBER_CARD,			// 9  
	// We will reach these stages due to orders from PMT (OLA)
	BAP_TRM_PAYMENT_CARD,			// 10
	BAP_TRM_TOTAL_PURCH_NMP,		// 11
	BAP_TRM_ACCOUNT_TYPE,			// 12
	BAP_TRM_PIN,					// 13
	BAP_TRM_ODOMETER,				// 14
	BAP_TRM_UNIT,					// 15
	BAP_TRM_SEE_CASHIER	,			// 16
	BAP_TRM_TRS_COMPLETE,			// 17
	BAP_TRM_RECEIPT,				// 18
	BAP_TRM_FAILURE_WITH_RETRY,		// 19
	BAP_TRM_CAR_REG,				// 20
	BAP_TRM_FINAL_PAYMENT_AMOUNT,	// 21
	// We will reach these stages due to actions from CL
	BAP_TRM_WAITING,				// 22
	BAP_TRM_FAILURE_END,			// 23
	BAP_TRM_ENTER_PLU,				// 24
	// We will reach these stages due to orders from PMT (OLA)
	BAP_TRM_ACCOUNT_NUM,			// 25
	BAP_TRM_ABA_NUM,				// 26
	BAP_TRM_ACCOUNT_NUM_EX,			// 27
	BAP_TRM_BRANCH,					// 28
	BAP_TRM_CHECK_CODE,				// 29
	BAP_TRM_CHECK_DATE,				// 30
	BAP_TRM_CHECK_SERIAL,			// 31
	BAP_TRM_COUNTRY_CODE,			// 32
	BAP_TRM_EXP_DATE,				// 33
	// We will reach these stages due to actions from CL
	BAP_TRM_FAILURE_ADDING_ITEM,	// 34
	// We will reach these stages due to orders from PMT (OLA)
	BAP_TRM_INITIAL_CODE,			// 35
	BAP_TRM_TOTAL_PURCH,			// 36
	BAP_TRM_VEHICLE_ID,				// 37
	BAP_TRM_REDEEM_POINTS,			// 38
	BAP_TRM_TRANS_REF,				// 39
	BAP_TRM_USE_CREDIT,				// 40
	BAP_TRM_COUPON_BARCODE,			// 41
	BAP_TRM_COUPON_SERIAL,			// 42
	BAP_TRM_REFERENCE,				// 43
	BAP_TRM_GET_AMOUNT,				// 44
	// We will reach these stages due to actions from CL
	BAP_TRM_CHANGE,					// 45
	// We will reach these stages due to orders from PMT (OLA)
	BAP_TRM_AUTHORIZATION_NUMBER,	// 46
	BAP_TRM_DRIVER_ID,				// 47
	BAP_TRM_CUSTOMER_REFERENCE,		// 48
	// We will reach these stages due to actions from CL
	BAP_TRM_CHANGE_PMNT_METHOD,		// 49
	BAP_TRM_ASK_MENU_PARAM,			// 50
	BAP_TRM_UNIT_PRICE_NMP,		    // 51  Open Price
	BAP_TRM_ASK_DEBT_AMOUNT,		// 52 Debt pay in.
	BAP_TRM_OPEN_NEED_MORE_PARAM,   // 53   //Open need more param TD 8302
	BAP_TRM_FAILURE_WITH_VOID_ITEM,	//54
	BAP_TRM_SUCCESS_WITH_VOID_ITEM,	//55
};


// Table # 5: Event IDs
enum BAP_EVENT
{
	BAP_EVENT_GET_UI_SCHEME,					// 0
	BAP_EVENT_END_SESSION,						// 1
	BAP_EVENT_GET_RAW_DATA,						// 2
	BAP_EVENT_PAYMENT_STAGE,					// 3
	BAP_EVENT_PAYMENT_SUCCEED,					// 4
	BAP_EVENT_PAYMENT_FAILED,					// 5
	BAP_EVENT_REPORT_IS_READY,					// 6
	BAP_EVENT_SET_CAR_WASH_ACCESS_CODE,			// 7
	BAP_EVENT_GET_EXTRA_UI,						// 8
	BAP_EVENT_MUST_DROP,						// 9
	BAP_EVENT_CASH_WARNING,						// 10
	BAP_EVENT_PMT_LIMIT_STAT_OK,				// 11
	BAP_EVENT_LAST_REPORT,						// 12
	BAP_EVENT_ATT_NOT_VALID,					// 13
	BAP_EVENT_ATT_ALLOWED,						// 14
	BAP_EVENT_ATT_NOT_ALLOWED,					// 15
	BAP_EVENT_COMMIT_EOD,						// 16
	BAP_EVENT_COMMIT_EOS,						// 17
	BAP_EVENT_TENDER_INFORMATION,				// 18
	BAP_EVENT_ATT_SIGN_IN_SUCCEEDED,			// 19
	BAP_EVENT_ATT_SIGN_IN_FAILED,				// 20
	BAP_EVENT_ATT_SIGN_OUT_SUCCEEDED,			// 21
	BAP_EVENT_ATT_SIGN_OUT_FAILED				// 22		
};

// Table # 7:	BuyAtBump return codes:
enum ITerminalUIRetCode 
{
	BAP_TERMINAL_UI_OK,										// 0
	BAP_TERMINAL_UI_MUST_INIT_FIRST,						// 1
	BAP_TERMINAL_UI_TERMINAL_MUST_INIT_FIRST,				// 2
	BAP_TERMINAL_UI_INVALID_PUMP_NUMBER,					// 3
	BAP_TERMINAL_UI_INVALID_TERMINAL_NUMBER,				// 4
	BAP_TERMINAL_UI_CANT_CONNECT_TO_POS_MULTI_TRS_SERVER,	// 5
	BAP_TERMINAL_UI_CANT_LOAD_DYNAMIC_MENU,					// 6
	BAP_TERMINAL_UI_INVALID_STRING_ID,						// 7
	BAP_TERMINAL_UI_INVALID_DYNAMIC_MENU_PATH,				// 8
	BAP_TERMINAL_UI_TERMIANL_IS_ALREADY_INIT,				// 9
	BAP_TERMINAL_UI_ALREADY_INIT,							// 10
	BAP_TERMINAL_UI_UNKNOWN_ERROR,							// 11
	BAP_TERMINAL_UI_PMT_THREW_EXCEPTION,					// 12
	BAP_TERMINAL_UI_INVALID_LANGUAGE,						// 13
	BAP_TERMINAL_UI_PAYMENT_PROBLEM,						// 14
	BAP_TERMINAL_UI_CARD_SWIPE_WITHOUT_SPECIFIC_ORDER,		// 15
	BAP_TERMINAL_UI_PRINTER_WITHOUT_SPECIFIC_ORDER,			// 16
	BAP_TERMINAL_UI_SESSION_ISNOT_OPEN,						// 17
	BAP_TERMINAL_UI_NA_YET,									// 18
	BAP_TERMINAL_UI_FUEL_ITEM_ALREADY_EXIST_IN_TICKET,		// 19
	BAP_TERMINAL_UI_MAX_ITEMS_IN_TICKET,					// 20
	BAP_TERMINAL_UI_NO_PERMISSION,							// 21
	BAP_TERMINAL_UI_RETRY,									// 22
	BAP_TERMINAL_UI_TERMINAL_MUST_OPEN_SESSION_FIRST,		// 23
	BAP_QUEUE_ERROR,										// 24
	BAP_THREW_EXCEPTION,									// 25
	BAP_TERMINAL_UI_NOT_READY_TO_HANDLE,					// 26 Nirl - Report Not ready on interface function when connection to PMT is not OK 
};

//Table # 8: m_lFlags InitAll
#define BAP_NONE										0x000000
#define BAP_STRING_IN_CLIENT_RESPONSIBILITY				0x000001
#define BAP_ALL_ITEMS_FOR_RECEIPT_STRUCT_2				0x000002
#define BAP_ALL_ITEMS_FOR_RECEIPT_STRUCT_3				0x000004
#define BAP_ALL_ITEMS_FOR_RECEIPT_STRUCT_4				0x000008
#define BAP_QUANTITY_STAGE_WITH_ITEM_NAME				0x000010
#define BAP_SKIP_CONFIRM_STAGE							0x000020 
#define BAP_QTY_TERMINATOR_1							0x000040
#define BAP_QTY_TERMINATOR_2							0x000080
#define BAP_QTY_TERMINATOR_3							0x000100
#define BAP_QTY_TERMINATOR_4							0x000200
#define BAP_QTY_TERMINATOR_5							0x000400
#define BAP_OPEN_CARD_READER_WHEN_FAILURE				0x000800
#define BAP_NOT_APPROVED_WITHOUT_QUESTION				0x001000
#define BAP_NO_LOYALTY_STATE							0x002000
#define BAP_LOYALTY_AS_CARD_ONLY						0x004000
#define BAP_ADD_TOTAL_PURCH_STATE						0x008000
#define BAP_CONTROL_RECEIPT_STATE						0x010000
#define BAP_GOTO_TENDER_AFTER_1_ITEM					0x020000
#define BAP_CHANGE_TREATMENT							0x040000
#define BAP_QUANTITY_STAGE_WITHOUT_YES					0x080000
#define BAP_QUANTITY_STAGE_WITH_NO						0x100000
#define BAP_RECONFIGURE									0x200000
#define BAP_NEVER_CLOSE_AUTO_AS_CASH					0x400000
#define BAP_LOYALTY_ALWAYS								0x800000
#define BAP_IMMEDIATE_MENU_UPDATE					   0x1000000
#define BAP_FORCE_PAYMENT_ON_TENDER_MENU			   0x2000000



//Table # 9 : lFlags for PrinterEvent 
#define BAP_PRINTER_IDLE								0x0000
#define BAP_PRINTER_START_PRINTING						0x0001
#define BAP_PRINTER_FINISH_PRINTING						0x0002
#define BAP_PRINTER_FINISH_PRINTING_FAILURE_RECEIPT		0x0004


//Table # 10 : lFlags for CardSwiped
#define BAP_CARD_READER_IDLE			0x0000
#define BAP_CARD_SWIPED_FOR_PAYMENT		0x0001
//#define BAP_TRACK2_IS_ACCOUNT_NUMBER	0x0002 not in use!!



//Table # 11 : Tender states
#define BAP_TENDER_STATE_NO_ACTION_YET							0x0000
#define BAP_TENDER_STATE_FIRE_TENDER_PAYMENT_STAGE_EVENT		0x0001
#define BAP_TENDER_STATE_FAILED									0x0002
#define BAP_TENDER_STATE_SUCCEED								0x0004



//Table # 12 : StartSaleSession
#define BAP_INIT_SALE_FUEL										0x0001
#define BAP_INIT_SALE_ITEMS										0x0002
#define BAP_INIT_REFUND_SALE									0x0004
#define BAP_INIT_AUTO_CASH_SALE									0x0008
#define BAP_INIT_BUFFER_INCLUDE_PAYMENT_INFO					0x0010
#define BAP_INIT_TENDER_SELECT_SESSION							0x0020
#define BAP_INIT_ENABLE_SWITCH_PAYMENT_METHOD					0x0040
#define BAP_INIT_WASTE_TRS      					            0x0080
#define BAP_INIT_SALE_WITH_LOYALTY								0x0100
#define BAP_INIT_SALE_DURING_DISPENSING							0x0200
#define BAP_INIT_SALE_WITH_PMNT_PARAM_VALIDATION 				0x0400
#define BAP_INIT_LOYALTY_WAS_SWIPED				 				0x0800

//Close sale session flags
#define BAP_CLOSE_SALE_SESSION_FORCE							0x00000001


//Table # 13 : GetReport report types
#define BAP_SLIP_REPORT											0x0001
#define BAP_X_DISPLAY											0x0002


//Table # 15 : GetExtraInfo
#define BAP_STRUCT_DEBIT										0x0001
#define BAP_ACCOUNT_DATA										0x0002
#define BAP_REFRESH_UI											0x0004
#define BAP_STRUCT_HOT_KEYS										0x0008


// Table # 16: GeneralEvent lFlags
#define BAP_STRUCT_DEBIT										0x0001
#define BAP_ACCOUNT_DATA										0x0002

// Table # 17: CloseSaleSession lFlags
#define CLOSE_SESSION_OK										0x0000
#define CLOSE_SESSION_AS_FAILURE								0x0001


// Table # 18:lFuncID param for  PMT::UserAllowed and for CheckLimitsStatus lFlags.
#define PMT_EOS_CMD				19
#define PMT_REFUND_CMD			29
#define PMT_DROP_CMD			14
#define PMT_WASTE_CMD			26
#define PMT_SALE_ITEMS_CMD		2	// not in use
#define PMT_CLOCK_IN_CMD		39
#define PMT_CLOCK_OUT_CMD		40
#define PMT_X_REPORT_CMD		37
#define PMT_SALE_FUEL			41
#define PMT_SALE_ITEMS			02
#define PMT_POS_REPORT_CMD		208


// Table # 19: GetRawData lFlags.
#define ROW_DATA_STRUCT2				0x0001  


// Table # 20: relevant for event BAP_EVENT_TENDER_INFORMATION & GetTenderInfo
#define TENDER_IS_CARD					0x0001  
#define TENDER_CONTINUE_NMP				0x0002  
#define TENDER_HAS_NMP_INFO				0x0004  


// PMT field "ItemStatusType".  
#define PMT_ITEM_STATUS_SALE			1 
#define PMT_ITEM_STATUS_WASTE			3 

//PMT Item properties
#define PMT_ITEM_IS_FROM_SCANNER        0x0001

// Relevant flags for StartSpecialSession
#define	SESSION_DEBT_CUST	0x0001  
#define	SESSION_TENDER_EX	0x0002 //(for future use)



#define ITEM_BARCODE_TYPE_UPCA      101  // Digits
#define ITEM_BARCODE_TYPE_UPCE		102  // Digits
#define ITEM_BARCODE_TYPE_EAN8		103  // (EAN)
#define ITEM_BARCODE_TYPE_EAN13		104  // (EAN)
#define ITEM_BARCODE_TYPE_EAN128	120  // EAN 128
#define ITEM_BARCODE_TYPE_EAN8S     118; // EAN 8 with supplemental barcode
#define ITEM_BARCODE_TYPE_EAN13S    119; // EAN 13 with supplemental barcode


// BaP WatchDog Flag.
#define NO_CONNECTION_TO_PMT					  0x00000000
#define CONNECTION_TO_PMT_OK_SND_RCV			  0x00000010
#define CONNECTION_TO_PMT_OK_SND_ONLY			  0x00000020
#define CONNECTION_TO_PMT_OK_RCV_ONLY			  0x00000040

// PMT - OLA ; PMT - BO
#define CONNECTION_PMT_TO_BO_NOT_OK_OLA_NOT_OK    0x00000100   
#define CONNECTION_PMT_TO_BO_OK_OLA_NOT_OK	      0x00000200
#define CONNECTION_PMT_TO_BO_NOT_OK_OLA_OK	      0x00000400
#define CONNECTION_PMT_TO_BO_OK_OLA_OK	          0x00000800

//Open price.
#define ADD_PARAM_AFTER_CHECK_PMNT_PARAMS        101
#define ADD_PARAM_AFTER_ADD_ITEM                 201
#define PMT_NEED_MORE_PARAM_DURING_ADD_ITEM      -34
#define PMT_FAILED_WITH_ERROR_MSG				 -36
///////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    TAG_PUMP_TRANSACT2		cPumpTransact;
    BYTE					sServiceFee			[STR_LONG_VAL10];
	BYTE					sFullSerCashPrice	[STR_LONG_VAL10];
	BYTE					sFullSerCreditPrice	[STR_LONG_VAL10];
	BYTE					sSelfSerCashPrice	[STR_LONG_VAL10];
	BYTE					sSelfSerCreditPrice	[STR_LONG_VAL10];
	BYTE					sGRTotalDiscount	[STR_LONG_VAL5];
	BYTE					sCouponAmount		[STR_LONG_VAL5];
	BYTE					sLoyaltyID			[21];
	BYTE					sVehicleID			[FULL_VEHICLE_ID];
	BYTE					sCardAmount			[STR_LONG_VAL10];
	BYTE					sDiscountPerLiter[8];      //31/10/04 - New for Avi As loyalty
	BYTE					sCardAmountByVolume;
	BYTE					sPMTReferenceNumber[10];
	BYTE					sLoyaltyRecalculationRequired;
	BYTE					sPreSetlimit[7];				//CR#19675 Preset data		
	BYTE					sPreSetlimitType; 
	DISCOUNT_ATTRIBUTES	    discountAttrsInfo;	//4 + 6*4 = 28 CR-38162    
	DISCOUNT_ATTRIBUTES_EXT	sDiscountAttrExt;//8		//4.0.22.509 38162
	//BYTE					spare[23];			//-to be deleted, no need
}BAP_FUEL_ITEM;

typedef struct
{
    TAG_PUMP_TRANSACT5		cPumpTransact;
    BYTE					sServiceFee			[STR_LONG_VAL10];
	BYTE					sFullSerCashPrice	[STR_LONG_VAL10];
	BYTE					sFullSerCreditPrice	[STR_LONG_VAL10];
	BYTE					sSelfSerCashPrice	[STR_LONG_VAL10];
	BYTE					sSelfSerCreditPrice	[STR_LONG_VAL10];
	BYTE					sGRTotalDiscount	[STR_LONG_VAL5];
	BYTE					sCouponAmount		[STR_LONG_VAL5];
	BYTE					sLoyaltyID			[21];
	BYTE					sVehicleID			[FULL_VEHICLE_ID];
	BYTE					sCardAmount			[STR_LONG_VAL10];
	BYTE					sDiscountPerLiter[8];      //31/10/04 - New for Avi As loyalty
	BYTE					sCardAmountByVolume;
	BYTE					sPMTReferenceNumber[10];
	BYTE					sLoyaltyRecalculationRequired;
	BYTE					sPreSetlimit[7];    //CR#19675 Preset data  //4.0.15.500
	BYTE					sPreSetlimitType;							//4.0.15.500
	BYTE					spare[100];
}BAP_FUEL_ITEM_EX;	//TD 38654


typedef struct 
{
	BYTE	sPinPadAction;				
	BYTE	sPinPadResult;				
	BYTE	sCalculatedMAC[8];			
	BYTE	sPinSessionKey[16];			
	BYTE	sMACSessionKey[16];			
	BYTE	sOLAExtraLen[4];
	BYTE	sOLAExtraData[400];
	BYTE	sFiller[50];
}BAP_DEBIT_PROCESS_INFO;


struct FUNC_PMT_PAYMENT_EVENT
{
	long	lPumpId;
	char    sNeededParams[BAP_NMP_LIST];
	long	lSessionId;
	long	lStatus;
	long	lTrsNumber;
	long	lFlags;
};

struct FUNC_PMT_NMP_EVENT //Open price
{
	long	lPumpId;
	long    lItemID;
	char    sNeededParams[BAP_NMP_LIST];
	long	lSessionId;
	long	lStatus;
	long	lTrsNumber;
	long	lFlags;
};


struct FUNC_PMT_REPORT_EVENT
{
	long lAttendantNo;
	char sResultString [BAP_MAX_REPORT_LEN];
	long lPumpId;
	char sReportType[BAP_MAX_BUFFER_LEN];
	long lFlag ;
};

struct FUNC_PMT_CLOCK_OUT_EVENT
{
	long lPumpId;
	long lAttendant;
	long lFlag; 
};

struct FUNC_INIT_TERMINAL
{
	long lTerminalNum; 
	long lPumpMap1; 
	long lPumpMap2;
	long lDisplayWidth;
	long lDisplayLines;  
	long lFlags; 
};


struct FUNC_KEY_PRESSED
{
	long lTerminalNum;
	long lPumpNum;
	long lKeyType;
	long lTypeValue;
	char sValue[BAP_MAX_BUFFER_LEN];
	long lFlags;
};

struct FUNC_SCANNER_CMD
{
	long lTerminalNum;
	long lPumpNum;
	char sValue[BAP_MAX_BUFFER_LEN];
	long lFlags;
};

struct FUNC_AVI_CONNECTED
{
	long lTerminalNum;
	long lPumpNum;
	char sOlaBuffer[BAP_MAX_REPORT_LEN];
	long lFlags;
};

struct FUNC_CARD_SWIPE
{
	long lTerminalNum;
	long lPumpNum;
	char sTrack1[BAP_MAX_BUFFER_LEN];
	char sTrack2[BAP_MAX_BUFFER_LEN];
	long lFlags;
};

struct FUNC_INIT_ALL
{
	char sPath[BAP_MAX_BUFFER_LEN];
	long lTimeOutVal;
	long lFlags;
};

struct FUNC_RELOAD_MENUS
{
	char sPath[BAP_MAX_BUFFER_LEN];
	long lFlags;
	BOOL bIsReloadFromPMT;
};

struct FUNC_START_SALE_SESSION
{
	long lTerminalNum;
	long lPumpNum;
	char sCashier[BAP_MAX_BUFFER_LEN];
	long lCashierID;
	long lManagerID;
	short lLanguage;
	long lAmountLimit;
	long lFlags;
	char sOlaData[BAP_MAX_REPORT_LEN ];
};

struct FUNC_INIT_PRINTER_STATUS_UPDATE
{
	long lTerminalNum;
	long lPumpNum;
	long lFlags;
};

struct FUNC_SIGN_IN
{
	long   lID;
	char   sPassword[BAP_MAX_BUFFER_LEN];
	long   lPumpNum;
	long   lFlags;
	long   lShift;
};

struct FUNC_SIGN_OUT
{
	long   lID;
	long   lPumpNum;
	long   lFlags;
};

struct FUNC_CLOSE_SALE_SESSION
{
	long lTerminalNum;
	long lPumpNum;
	long lFlags;
};

struct FUNC_SET_MENU_STR
{
	short iLanguage;
	long lStringID;
	char sString[BAP_MAX_BUFFER_LEN]; 
};

struct FUNC_SET_EXTRA_INFO
{
	long lTerminalNum;
	long lPumpNum;
	long lData;
	char sData[sizeof(BAP_DEBIT_PROCESS_INFO)];
	long lFlags;

};

struct FUNC_GET_REPORT
{
	long lPumpId;
	long lTerminalId;
	long lAttId;
	long lPrinterWidth;
	long lReportType;
	long lFlags; 
	long lReportId;
	char sOpenParam[MAX_OPEN_PARAMS_LENGTH];
};

struct FUNC_EOS
{
	long lPumpId;
	long lTerminalNum;
	long lOperatorId;
	long lFlags; 
};

struct FUNC_DROP
{
	long lPumpId;
	long lTerminalNum;
	long lAmount;
	long lOperatorId; 
	long lRefNum;
	long lFlag; 
};

struct FUNC_CHECK_LIMIT_STATUS
{
	long lPumpId;
	long lTerminalNum;
	long lOperatorId; 
	long lFlags; 
};

struct FUNC_SWITCH_LANG
{
	long lPumpNum;
	long lTerminalNum;
	long lNewLang; 
	long lFlags; 
};

struct FUNC_IS_EOD
{
	long lPumpNum;
	long lTerminalNum;
	long lAtt;
	long lBaPFlags; 
	long lPMTFlags; 
};

struct FUNC_PMT_SHUTDOWN
{
	long lFlags; 
};

struct FUNC_CLOSE_CONNECTION_EX
{
	long lFlags; 
};

struct FUNC_SAVE_TRS_TEXT
{
	long lPumpId;
	long lTermId;
	long lTransID;
	long lFlags;
	char sSlipData[BAP_MAX_REPORT_LEN];
};

struct FUNC_CLOCK
{
	long lEmployeeID;
	long lClockType;
	long lFlags;
};

struct FUNC_PMT_WATCH_DOG
{
	long lStatus;
	long lFlags;
	long lFlags2;
};

struct FUNC_START_SPECIAL_SESSION
{
	long lTerminalNum;
	long lPumpNum;
	char sCashier[BAP_MAX_BUFFER_LEN];
	long lCashierID;
	short lLanguage;
	char sCustomerData[BAP_MAX_BUFFER_LEN];
	long lFlags;
};


struct FUNC_USER_ALLOWED
{
	long lTerminalNum;
	long lPumpNum;
	long lOperatorId;
	long lFuncID;
	long lFlags;
};

enum PMT_DROP_WARNING 
{
	PMT_OK,								//0
    PMT_WARNING_SHOULD_DROP,			//1
    PMT_ERROR_MUST_DROP,				//2
};



enum MainServerFunc 
{
	BAP_FUNC_NONE,						// 0
	BAP_FUNC_INIT_TERMINAL,				// 1
	BAP_FUNC_KEY_PRESSED,				// 2
	BAP_FUNC_CARD_SWIPE,				// 3
	BAP_FUNC_INIT_ALL,					// 4
	BAP_FUNC_START_SALE_SESSION,		// 5
	BAP_FUNC_PRINTER_STATUS_UPDATE,		// 6
	BAP_FUNC_ADD_FUEL_ITEM,				// 7
	BAP_FUNC_CLOSE_SALE_SESSION,		// 8
	BAP_FUNC_SET_MENU_STR,				// 9
	BAP_FUNC_END_OF_SHIFT,				// 10
	BAP_FUNC_SIGN_IN,					// 11
	BAP_FUNC_PMT_PAYMENT_EVENT,			// 12
	BAP_FUNC_SIGN_OUT,					// 13
	BAP_FUNC_REQUEST_REPORT,			// 14
	BAP_FUNC_SET_CAR_WASH_ACCESS,		// 15
	BAP_FUNC_SET_EXTRA_INFO,			// 16
	BAP_FUNC_PMT_REPORT_EVENT,			// 17
	BAP_FUNC_DROP,						// 18
	BAP_FUNC_CHECK_LIMIT_STATUS,		// 19
	BAP_FUNC_RELOAD_MENUS,				// 20
	BAP_FUNC_SWITCH_LANG,				// 21
	BAP_FUNC_SCANNER_CMD,				// 22
	BAP_FUNC_IS_EOD,					// 23
	BAP_FUNC_CLOSE_CONNECTION,			// 24
	BAP_FUNC_CLOSE_CONNECTION_EX,		// 25
	BAP_FUNC_PMT_SHUTDOWN,				// 26
	BAP_FUNC_SAVE_TRS_TEXT,				// 27
	BAP_FUNC_PMT_INIT_COMPLETE,			// 28
	BAP_FUNC_PMT_WATCH_DOG,				// 29 
	BAP_FUNC_PMT_NMP_EVENT,             // 30
	BAP_CLOCK,							// 31
	BAP_FUNC_START_SPECIAL_SESSION,		// 32
	BAP_FUNC_USER_ALLOWED ,				// 33
	BAP_FUNC_AVI_CONNECTED,				// 34

};

typedef struct
{
	BYTE 				sBOItemID			[5];		//0-4     BO identifier	
	BYTE				sQuantity			[5];		//5-9     The Unit Count (+/-)
	BYTE 				sItemCode			[13];		//10-22   Item code (today the same like barcode)
	BYTE				sItemBarcode		[13];		//23-35   The item barcode
	BYTE				sItemName			[20];  		//36-55   Short Item Name 
	BYTE				sTaxPrintChar		[10];		//56-65   A list of Tax-Symbols that applies to this item
	BYTE 				sUnitPrice			[10];		//66-75   Basic Unit-Price
	BYTE 				sAmountTaxIncluded	[10];		//76-85   Processed final Line Price 
	BYTE 				sAmountTaxExluded	[10];		//86-95   Processed final Line Price
	BYTE 				sTaxAmount			[10];		//96-105  The item's Tax
	BYTE				sPromotionVal		[10];		//106-115 Promotion value. 0 if none.
	BYTE				sDiscountVal		[10];		//116-125 Discount value. 0 if none.
	BYTE				byIsFuelItem;					//126     Is it a fuel item or not
	BYTE				sFuelVolume			[10];		//127-136 If fuel item then sQuantity=1 and this field contains the volume
	BYTE				byIsCarWashItem;				//137     Is it a fuel item or not
	BYTE				sCarWashCode		[6];		//138-143
	BYTE				sCarWashExpire		[6];		//144-149
	BYTE 				sTaxAmount2			[10];		//150-159  The item's Tax type 2
	BYTE 				sTaxAmount3			[10];		//160-169  The item's Tax type 2
	BYTE				byItemIsTaxIncluded;			//170
	BYTE				byIsWeightItem;					//171      Is it a weight item or not
	BYTE				byItemSaleStatus;				//172      Is it a weight item or not
	BYTE				sBuff				[4];		//173-176 Spare bytes
}BAP_ITEM_FOR_RECEIPT;

typedef struct
{
	BYTE				byItemDiscPerUnit		[8];		// Discount per unit price
	BYTE				byItemUnitPriceWithDisc	[8];		// Final price after the discount
	BYTE 				sFiller					[44];		// Spare bytes
}BAP_ITEM_FOR_RECEIPT2;

typedef struct
{
	BYTE					sItemsNumber	 [5];				//0-4       Number of Items in ticket ( Include Fuel items).
	BAP_ITEM_FOR_RECEIPT	Items			 [MAX_ITEMS_NUM];	//5-1774    Array of items (Include Fuel items). 1274 = (127*10)-1+5. 
	BYTE					sTenderTrans	 [2];				//1775-1776 Trs Tendar type
	BYTE 					sItemsTotal		 [10];				//1777-1786 Total of items’ Amount  ( Include Fuel items). 
	BYTE 					sPromotionTotal  [10];				//1787-1796 Total of Promotion within the transaction( Include Fuel items).
	BYTE 					sDiscountTotal   [10];				//1797-1806 Total of Ticket-Discount in transaction( Include Fuel items).
	BYTE 					sTaxTotal		 [10];				//1807-1816 Total tax in trans( Include Fuel items).
	BYTE 					sTransactionTotal[10]; 				//1817-1826 Total to pay ( Include Fuel items). 
	BYTE					sInvoiceNumber	 [10];				//1827-1836 Receipt number
	BYTE					sFullCardName	 [20];				//1837-1856 Card name
	BYTE					sCardType		 [2];				//1857-1858 Card type
	BYTE					sAuthNumber	     [12];				//1859-1870 Authorizer #
	BYTE					sAccountNumber	 [21];				//1871-1891 Account #
	BYTE					sExpire			 [4];				//1892-1895 Expire date
	BYTE					sDateTime		 [10];				//1896-1905 Date & time
	BYTE					sFullVehicleId	 [10];				//1906-1915 car ID
	BYTE					sMagKey			 [40];				//1916-1955 Member account number
	BYTE					sOdometer		 [8];				//1956-1963
	BYTE					sBankTermId		 [8];				//1964-1971
	BYTE					sSiteId			 [10];				//1972-1981
	BYTE					sTransBatch		 [5];				//1982-1986
	BYTE					sReferenceNumber [12];				//1987-1998
	BYTE					sReceiptType	 [3];				//1999-2001
	BYTE					bySignatureSlip;				    //2002
	BYTE					byIsOnLine;						    //2003		1 if OLA works online
	BYTE					sNonFuelTotal_TaxInc		[10];	//2004-2013 The total amount of non fuel items + tax
	BYTE					sNonFuelTotal_TaxExc		[10];	//2014-2023 The total amount of non fuel items without the tax
	BYTE					sAuthorizationSerialNumber	[3];	//2024-2026 TR file number
 	BYTE					sAuthorizerFileNumber		[3];	//2027-2029 The record number in the TR file
	BYTE					sTotalRemainingBalance		[8];	//2030-2037 The remaining balance in a pre pay card
	BYTE					sAccountType;						//2038		Chequing/Saving
	BYTE					sABANumber					[5];	//2039-2043 bank number
	BYTE					sBranch						[5];	//2044-2048 brunch number
	BYTE					sCheckSerial				[10];	//2049-2058 check sirial number
	BYTE					sISOResponse				[2];	//2059-2060 
	BYTE					sSPDHCode					[5];	//2061-2065
	BYTE					sTranStatus;						//2066
	BYTE					sSlipFlags;							//2067		Bit wise, restrict several fields in the receipt
	BYTE					sAuthorizationDateTime		[10];	//2068-2077 Authorization Date & time
	BYTE					sLanguageId;						//2078		Language ID between 0-255
	BYTE					sTrsRoundingAmount			[5];	//2079-2083
	BYTE					sTransactionNum				[10];	//2084-2093
	BYTE					sTenderAmount				[10];	//2094-2103
	BYTE					sChangeAmount				[10];	//2104-2113
	BYTE					sTotalRedeemedPoints		[6];    //2114-2119
	BYTE					sAmountByPoints				[8];	//2120-2127
	BYTE					sCreditAmount				[9];	//2128-2136
}
BAP_ALL_ITEMS_FOR_RECEIPT;

typedef struct
{
	BAP_ITEM_FOR_RECEIPT2	Items					[MAX_ITEMS_NUM];	// 60*10 = 600
	BYTE					sCouponMessage			[40];			// Coupon message comes from OLA
	BYTE					sCustomerName			[CUSTOMER_NAME_STR_LEN];
	BYTE					sCustomerNewObligo		[10];
	BYTE					sTmp					[10];				// Saved for couple of weeks for Eran.
	BYTE					sTaxAttributes			[5];
	BYTE					sTaxTotalList			[MAX_TAX][10];		// 50
	BYTE					sTaxNameList			[MAX_TAX][10];		// 50
	BYTE					sLogicShiftNumber		[2];
	BYTE					sReserveForCoupon		[176];
	BYTE					sReceiptMode;
	DISCOUNT_ATTRIBUTES		discountAttrsInfo;				//CR-38162 4 + 6*4 = 28 
	DISCOUNT_ATTRIBUTES_EXT	sDiscountAttrExt;//8		//4.0.22.509 38162
	BYTE					sFiller					[200];	//236-4-24		
		
}
BAP_ALL_ITEMS_FOR_RECEIPT2;							// Total 1200

typedef struct
{
	BAP_ALL_ITEMS_FOR_RECEIPT	 cBAPReceiptItem;
	BAP_ALL_ITEMS_FOR_RECEIPT2	 cBAPReceiptItem2;
	RECEIPT_EXTRA_INFO			 cReceiptExtraInfo;
}
BAP_ALL_DATA_STRUCTS;

typedef struct 
{	
	BAP_ALL_ITEMS_FOR_RECEIPT   cBAPItemsForReceipt;
}TAG_ITEMS_REC;


typedef struct
{
    BYTE					sCashTotals			[STR_LONG_VAL10];
	BYTE					sCreditTotals		[STR_LONG_VAL10];
	BYTE					sCuponsTotals		[STR_LONG_VAL10];
	BYTE					sOtherTotals		[STR_LONG_VAL10];
	BYTE					sFiler				[40];
}BAP_ATTENDENT_TOTALS;

typedef struct
{
    BYTE					sMainTender			[STR_LONG_VAL5];
	BYTE					sSubTender			[STR_LONG_VAL5];
	BYTE					sTrack2				[TRACK_2_DATA_SIZE];
	BYTE					sTrack1				[TRACK_1_DATA_SIZE];
	BYTE					sNMPdata			[BAP_NMP_LIST];
}TAG_BAP_PAYMENT_INFO;



typedef struct
{
	unsigned char bPrint:1;
	unsigned char bQty:1;
	unsigned char spare:5;
	unsigned char bInit:1; // Make sure this byte is not 0 so the BSTR won't be cut later on.
}BAP_TAX_ATTRIBUTES;


typedef struct
{
    BYTE					sHotKey1			[STR_LONG_VAL5];
	BYTE					sHotKey2			[STR_LONG_VAL5];
	BYTE					sHotKey3			[STR_LONG_VAL5];
	BYTE					sHotKey4			[STR_LONG_VAL5];
	BYTE					sHotKey5			[STR_LONG_VAL5];
}BAP_HOT_KEYS;


typedef struct 
{
		BYTE	sManualIdentifier;	// True is number was enter manually
		BYTE	sCustomerIdentifier[38];	// manual number or card track	
} BAP_CUST_DATA;

#pragma pack()

#endif