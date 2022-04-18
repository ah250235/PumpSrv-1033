//-----------------------------------------------------------------------
// File Name   :	Pump.h
// 
// Description :	Constants definition for PumpSrv 
//					
// Written by  :	Moti Lichi 10.04.97
// 
//-----------------------------------------------------------------------

#ifndef __PUMP_H__
#define __PUMP_H__


#include <windef.h> // min .max
#include "LoyaltyDefs.h"

/////////////////////////////////////////////////////////////////////////////
// Pump Const.definition 


#define CAR_WASH_REQUEST_TIME_OUT	30L //Sec   4.0.5.26

#define DEFAULT_DISPENSING_TIMEOUT					13 // Seconds
#define DEFAULT_UNPAID_TRS_TIMEOUT					60 // Seconds
#define DEFAULT_PAK_AUTHORIZE_TIMEOUT				90 // Seconds
#define DEFAULT_CLOSED_PREPAY_TIMEOUT				1200 // Seconds  = 20 minuts

#define DEFAULT_MAX_TRS_NUMBER						6 //4.0.13.500

#define DEFAULT_PAK_CONFIRMATION_TIMEOUT			300 //4.0.20.500 - TD 65908

// Sleep time
#define REGISTRATION_SLEEP_TIME				1



#define MAX_DEBIT_DATA_BUFFER   400

#define ENABLE_64_PUMPS 0x0000001  //Eyal 4.0.0.79

#define MAX_ALLOWED_UNPAID_TRS(p)	(_Module.m_server.m_cPumpArray[p].GetMaxUnpaidTrsAllowed())	
#define MAX_ALLOWED_WAITING_TRS(p)  (_Module.m_server.m_cPumpArray[p].GetMaxPrePayTrsAllowed())	
#define MAX_ALLOWED_POST_VOID_TRS(p)(_Module.m_server.m_cPumpArray[p].GetMaxPostVoidTrsAllowed()) //CR 351687: Enhanced post void transactions


//Tanks flags
#define SUPPORT_32_TANKS        0x00000001  //Eyal 3.2.01
//TankInfo flag
#define REFRESH_TANK_INFO       0x00000002
	
#define STRUCT_INCLUDES_DELIVERY		0x00000004 //4.0.7.501
#define GTI_ULLAGE_READING_INFO			0x00000008 //4.0.9.503
#define GTI_DELIVERY_NO_HISTORY_INFO	0x00000010 //4.0.9.503


//Report Type  //4.0.9.503
#define  REPORT_TYPE_ULLAGE 	   1
#define  REPORT_TYPE_DERIVERY      2
#define	 REPORT_TYPE_PIC		   3	

#define  CLEAR_REPORT_TYPE_ULLAGE 	    ~(1)
#define  CLEAR_REPORT_TYPE_DERIVERY      ~(2)

//GPI::GetReport (Type) 4.0.16.502 - TD 35765
#define GET_REPORT_GENERAL_TYPE 			0
#define GET_REPORT_PIC_TYPE 				3
#define GET_REPORT_WETSTOCK_ASYNC_TYPE 		4	//4.0.22.501 - TD 69905


// Tank reading memo record type
#define TRM_TYPE_INCLUDE_DELIVERY_DATA  0x00000001 //4.0.7.501
#define TRM_TYPE_WETSTOCK_REPORT		0x00000002 //4.0.22.503 - TD 69905

#define READ_ALL_TANKS			0	//add by Yaron

#define MAX_PURE_PRODUCTS					16
#define MAX_MODES							16
#define MAX_SERVICE_FEE						16    // 3.0.2.3
#define	MAX_MODE_SCHEME						32
#define	MAX_DEP_IN_LIST						16
#define	MAX_CAR_WASH_PROGRAM				64  // Jira 170
#define MAX_CAR_WASH_PROD_CODE_LEN			60
#define	MAX_SCREEN_LAYOUT					32
#define MAX_LANGUAGES						4	 // TD 211528
#define MAX_SAFETY_PROMPTS					20
#define MAX_ACTIVE_POST_VOID 2			// 4.0.27.230
#define MINIMUM_ACTIVE_POST_VOID 2
#define MINIMUM_ACTIVE_CASH_ACCEPTOR 2 //4.0.12.502
#define SLOT_SAVED_FOR_POST_VOID_TRS 1 //CR 351687: Coles Enhanced post void transaction

#define PUMP_START_VER			1

// accumulation shift total definition 
// 4.0.5.26
#define AST_MAX_GRADES			MAX_GRADES + 1 //( //add one to garbage grade information)


// transaction type
enum AccumulationShiftTotalTransactionType
{	
	AST_TRS_ALL	= 0, 
	AST_TRS_PAK = 1,
	AST_TRS_PAP	= 2,
	AST_TRS_PREPAY = 3,
	AST_TRS_AVI = 4,
	AST_TRS_ATTENDANT = 5,
	AST_TRS_CASH_ACCEPTOR =6,
	ATS_TRS_PAK_AUTO_RECONCILE = 7,  //4.0.7.506
	ATS_TRS_future_USE1=8,		//4.0.7.506
	ATS_TRS_future_USE2=9,		//4.0.7.506
	ATS_TRS_future_USE3=10, 	//4.0.7.506
};



// accumulation flags.

#define GAT_PAK					0x00000001
#define GAT_PAP					0x00000002
#define GAT_PREPAY				0x00000004
#define GAT_AVI					0x00000008
#define GAT_ATTENDANT			0x00000010
#define GAT_CASH_ACCEPTOR		0x00000020
#define GAT_PAK_AUTO_RECONCILE	0x00000040
#define GAT_BY_GRADE			0x00001000

#define GAT_CURRENT_REPORTS		0x00010000
#define GAT_BY_DATE				0x00020000
#define GAT_LAST_REPORT			0x00040000
#define GAT_ALL_REPORT			0x00080000

#define GAT_SET_RECORD_STATUS   0x01000000


#define MAX_READ_NUMBER			99999 // 5 digit
#define MAX_TRANSACT_SEQ_NUMBER	99999 // 5 digit
#define MAX_INDEX_NUMBER		99999 // used by indexes - PUMP_TOTALS, TANK_REAGING, DELIVERY_REPORT 

#define MAX_INDEX_TO_TURN_OVER				50000 
#define MAX_SHORT_INDEX_TO_TURN_OVER		0000

#define	TRS_NO_LINK							0x0000
#define	TRS_LINK_PREPAY_NUMBER				0x0001 // Link Number == syncNumber
#define	TRS_LINK_OLA_NUMBER					0x0002 // Link Number == seqNumber
#define	TRS_LINK_ANY						0x0004
#define	TRS_LINK_PAK_AND_LOYALTY			0x0008 // Link Number ==  trs number
#define	TRS_LINK_PREPAY_NUMBER_AND_LOYALTY	0x0010 // Link Number == syncNumber
#define	TRS_LINK_ATTEND						0x0020 // Link Number == syncNumber //3.2.0.76
#define TRS_LINK_EXTRA_INFO					0x0040   
#define TRS_LINK_CASH_ACCEPTOR_INFO			0x0080    //4.0.1.48
#define TRS_LINK_ASH_DATA					0x0100    //4.0.2.27
#define TRS_LINK_FLEET						0x0200    //4.0.9.507
#define TRS_LINK_FLEET_PRICE_CHANGE			0x0400    //4.0.10.5
#define TRS_LINK_XML_BUFF					0x0800	  //4.0.12.501
#define TRS_LINK_ZONES						0x1000		//4.0.19.710 66705 //4.0.23.504
#define TRS_LINK_OLA_EXTRA_DATA				0x2000		//4.0.24.140	//4.0.27.40 166688 

// Get OLA buffer flags  111
#define DATA_IS_SESSION_AUTHORIZE        0x0001
#define DATA_IS_SESSION_COMPLETE         0x0002


//SetTrsStateFlags  3.2.0.76
#define GET_ATTEND_TRS				0x00000001
#define CHARGE_ATTEND_TRS			0x00000002
#define CHARGE_OLA_TRS				0x00000004
#define TRS_PAID_AT_POS				0x00000008   // fuel transaction was paid at POS
#define TRS_INCLUDE_ITEMS			0x00000010   // Trs include items sell.
#define TRS_IGNOR_OLA				0x00000020   // Trs include items sell.
#define TRS_ATTEND_MOVE_TO_PAK		0x00000040   // Trs include items sell.
#define TRS_ATTEND_IS_ACCEPTED_PUMP 0x00000080   // if this falgs is set, the parameter lTrsNumber contain  the  accepted Pump number.
#define TRS_FORCE_PAID				0x00000100 // Force paid offer trs  //4.0.5.0
#define TRS_OLA_THREE_REC 			0x00000200 //4.0.5.15
#define TRS_WITH_EXTRA_ITEM			0x00000400 //4.0.5.27 //3.2.0.76
#define TRS_OVERWRITE_TRS			0x00000800 //4.0.5.27 //3.2.0.76
#define TRS_PAK_PAYMENT_CARD		0x00001000 //4.0.9.506 //amram
#define TRS_LOYALTY_CARD			0x00002000 //4.0.9.506 //amram
#define TRS_FORCE_UNPAID_PAK		0x00004000 //4.0.9.508 come to supper t generic pak trs(ingenco) 
#define TRS_IGNORE_OFFER			0x00008000 //4.0.9.508 in case this flag is on we will return trs the are in offer(ingenco) 
#define TRS_ATT_TO_PAP				0x00010000 //4.0.11.500 to insert to add to OLA QDX
#define TRS_INCLUDE_ZONE			0x00020000 //4.0.11.503
#define TRS_UPDATE_PAID_QDX			0x00040000 //4.0.11.503 update  paid qdx to onOffer
#define TRS_WITH_EXTRA_POS_NUMBER   0x00080000 //4.0.11.506 Add tp GPI::SetTrsState if the pos number is +100
#define TRS_DELETE_TRANSACTIONS		0x00100000 //4.0.14.501

#define CANCEL_FROM_EXT_TERMINAL	0x00200000 //4.0.19.504 - 47559
#define TRS_PUMP_TEST				0x00400000 //4.0.22.501 - TD 69904
#define STRUCT_IS_PUMP_TEST_DATA_1	0x00800000 //4.0.20.701 74028
#define SET_STATE_FROM_EXT_TERMINAL	0x01000000 //4.0.19.830 - TD 89646


//Parse OLA buffer flage 
#define POB_APPEND_DATA				0x00000001

#define ENCRYPT_DATA				0x00000001

#define	FIELD_DWORD						0
#define	FIELD_SZ						1


#define MAX_XML_RECEIPT_ROW_DATA		10000		//4.0.9.509

#define	MAX_TABLE_NAME					100
#define	MAX_FIELD_NAME					MAX_TABLE_NAME					
//#define	MAX_FIELD_VAL					256
#define	TYPICAL_FIELDS_PER_RECORD		100 // 20 4.0.5.41
#define	MAX_RECORD_LEN					(TYPICAL_FIELDS_PER_RECORD * MAX_FIELD_VAL) + (TYPICAL_FIELDS_PER_RECORD * MAX_FIELD_NAME)
#define MAX_MSG_LINE_LEN                16
#define MAX_ID_LEN						3	//4.0.16.500
#define MAX_ENCRYPT_LEN					1000 //4.0.18.501
#define MAX_PUMP_CAPTION_LEN			12   //4.0.20.500 -  TD 64967

// Regional Settings
//-----------------
#define	REGION_USA						1
#define	REGION_UK						2
#define	REGION_ISRAEL					3
#define REGION_EUROPE					4 //4.0.12.506
#define REGION_AUSTRALIA				5 //4.0.12.506

// nMethodNumber
#define	METHOD_1						1
#define	METHOD_2						2


// Tag pump version
enum {	
		TAG_PUMP_VERSION_UNKNOWN	= 0,
		TAG_PUMP_VERSION_1			= 1, 
		TAG_PUMP_VERSION_2			= 2, 
		TAG_PUMP_VERSION_3			= 3, 
		TAG_PUMP_VERSION_4			= 4, 
		TAG_PUMP_VERSION_5			= 5, 
		TAG_PUMP_VERSION_6			= 6, 
		TAG_PUMP_VERSION_7			= 7, 
		TAG_PUMP_VERSION_8			= 8,
		TAG_PUMP_VERSION_9			= 9,

};

// Pump/Transact Misc. Flags
//---------------------------------------------------
// Pump level - 

// --------------------------------------------------
// MISC is passed as STR_LONG_VAL5 , 
// so the last Flag can be the 16 position =  0x00008000
// --------------------------------------------------

// Transaction level - 
#define	MISC_UNPAID_TRS_TIMEOUT					 0x00000002
#define	MISC_NOT_UNPAID_TRS_TIMEOUT				 0xfffffffd

#define	MISC_PAP_DECLINED						 0x00000004
#define	MISC_NOT_PAP_DECLINED					 0xfffffffb

#define	MISC_TRS_PENDING_CANCEL					 0x00000008
#define	MISC_NOT_TRS_PENDING_CANCEL				 0xfffffff7

#define	MISC_TRS_PAP_CANCELED					 0x00000010
#define	MISC_NOT_TRS_PAP_CANCELED				 0xffffffef

#define	MISC_SUPPRESSED_UNPAID_TRS_TIMEOUT		 0x00000400
#define	MISC_NOT_SUPPRESSED_UNPAID_TRS_TIMEOUT	 0xfffffbff

//4.0.3.46
#define	MISC_TRS_PAK_TO_PAP_FAILED				 0x00000800
#define	MISC_TRS_NOT_PAK_TO_PAP_FAILED			 0xfffff7ff

//4.0.22.508 75592
#define	MISC_TRS_FAILED_CAT						  0x10000000
#define	MISC_TRS_NOT_FAILED_CAT					~(0x10000000)

//#define	MISC_PUMP_CHECK_AND_RESERVE				 0x20000000		//4.0.17.20 - TD 26384 this misc is for both trs and pump
//#define	MISC_NOT_PUMP_CHECK_AND_RESERVE		   ~(0x20000000)	//4.0.17.20 - TD 26384

//4.0.26.505 157073
#define	MISC_TRS_BLOCKED_ANPR					  0x00002000 //BLACK //4.0.26.505 157073
#define	MISC_TRS_NOT_BLOCKED_ANPR				~(0x00002000)

#define	MISC_TRS_AUTH_WITH_FAILED_ANPR			  0x00004000 //Turquoise //4.0.26.505 157073
#define	MISC_TRS_NOT_AUTH_WITH_FAILED_ANPR		~(0x00004000)



// Pump level - Process extra information:
#define	MISC_DISPENSING_TIMEOUT					 0x00000001
#define	MISC_NOT_DISPENSING_TIMEOUT				 0xfffffffe

#define	MISC_UNPAID_TRS_TIMEOUT					 0x00000002	//trs
#define	MISC_NOT_UNPAID_TRS_TIMEOUT				 0xfffffffd

#define	MISC_CURRENT_TRS_PAP_CANCELED			 0x00000004
#define	MISC_NOT_CURRENT_TRS_PAP_CANCELED		 0xfffffffb

#define	MISC_WAIT_LOCK_TIMEOUT					 0x00000010
#define	MISC_NOT_WAIT_LOCK_TIMEOUT				 0xffffffef

#define	MISC_WAIT_RESPONSE_TIMEOUT				 0x00000020
#define	MISC_NOT_WAIT_RESPONSE_TIMEOUT			 0xffffffdf

#define	MISC_OPEN_PRE_PAY_TRS					 0x00000040
#define	MISC_NOT_OPEN_PRE_PAY_TRS				 0xffffffbf

#define	MISC_OPEN_PAY_AT_KIOSK_TRS				 0x00000080
#define	MISC_NOT_OPEN_PAY_AT_KIOSK_TRS			 0xffffff7f

#define	MISC_OPEN_PAY_AT_PUMP_TRS				 0x00000100
#define	MISC_NOT_OPEN_PAY_AT_PUMP_TRS			 0xfffffeff

#define	MISC_WAIT_FOR_PUMP_TO_BE_READY			 0x00000200
#define	MISC_NOT_WAIT_FOR_PUMP_TO_BE_READY		 0xfffffdff

#define	MISC_PUMP_AVAILABLE_FOR_PRE_PAY			 0x00000800
#define	MISC_NOT_PUMP_AVAILABLE_FOR_PRE_PAY		 0xfffff7ff

#define	MISC_ALLOW_PREPAY_ON_REQUESTING_PUMP	 0x00004000
#define	MISC_NOT_ALLOW_PREPAY_ON_REQUESTING_PUMP 0xffffbfff

#define	MISC_HELP_REQUEST						 0x00008000
#define	MISC_NOT_HELP_REQUEST					 0xffff7fff

#define	MISC_VIT_CONFLICT						 0x00010000
#define	MISC_NOT_VIT_CONFLICT					 0xfffeffff

#define	MISC_CREDIT_CARD_NOT_AUTHORIZED			 0x00020000
#define	MISC_NOT_CREDIT_CARD_NOT_AUTHORIZED		 0xfffdffff

#define	MISC_NEED_MORE_PARAM					 0x00040000
#define	MISC_NOT_NEED_MORE_PARAM				 0xfffbffff

#define	MISC_APPROVED_PAP						 0x00080000
#define	MISC_NOT_APPROVED_PAP					 0xfff7ffff

#define	MISC_MANUAL_MODE						 0x00100000
#define	MISC_NOT_MANUAL_MODE  /*Automatic*/		 0xffefffff

#define	MISC_POST_VOID							 0x00200000
#define	MISC_NOT_POST_VOID					 	 0xffdfffff

//Eyal 4.0.0.47 (3.2.0.93) Start
#define MISC_PREPAY_TIMED_OUT                    0x00400000

//4.0.1.38  changed ! to ~
#define MISC_NOT_PREPAY_TIMED_OUT                ~(0x00400000)
//Eyal 4.0.0.47 (3.2.0.93) End

//4.0.1.38 Start
#define MISC_NOZZLE_LEFT_OUT_TIMEOUT             0x00800000
#define MISC_NOT_NOZZLE_LEFT_OUT_TIMEOUT         ~(0x00800000)
//4.0.1.38 End

//4.0.2.13 Start
#define MISC_AVI_NOT_AUTHORIZED                  0x01000000
#define MISC_NOT_AVI_NOT_AUTHORIZED              ~(0x01000000)
//4.0.2.13 End

#define MISC_ORIGINAL_ATTENDANT_TRS               0x02000000
#define MISC_NOT_ORIGINAL_ATTENDANT_TRS         ~(0x02000000)

#define MISC_PAK_TO_PAP_FAILED					  0x04000000		//4.0.3.45
#define MISC_NOT_PAK_TO_PAP_FAILED				~(0x04000000)

#define MISC_DECLINE_PRP						  0x08000000		//4.0.12.502
#define MISC_NOT_MISC_DECLINE_PRP				~(0x08000000)

//4.0.13.73
#define	MISC_CREDIT_CARD_DECLINE				  0x10000000
#define	MISC_NOT_CREDIT_CARD_DECLINE			~(0x10000000)

#define MISC_IGNORE_FIELD_VALUE					 0xffffffff

#define	MISC_PUMP_CHECK_AND_RESERVE				 0x20000000		//4.0.17.20 - TD 26384
#define	MISC_NOT_PUMP_CHECK_AND_RESERVE		   ~(0x20000000)	//4.0.17.20 - TD 26384

#define	MISC_CASHIER_APPROVAL					 0x40000000		//4.0.20.40 - TD 58096
#define	MISC_NOT_CASHIER_APPROVAL			   ~(0x40000000)	//4.0.20.40 - TD 58096

#define MISC2_HIDDEN_TRS							0x00000001		//CR 351687: Enhanced post void transactions20068
#define MISC2_NOT_HIDDEN_TRS				 		~(0x00000001)	//CR 351687: Enhanced post void transactions20068


//4.0.21.500	69203
//IMPORTATNT : - THIS MISC IS GOING TO ANOTHER MEMBER IN A SIZE OF 1 BYTE !

#define MISC_EXT_MAX_CONCURRENT_DISPENSING			 0x01
#define MISC_EXT_NOT_MAX_CONCURRENT_DISPENSING		~(0x01)

#define MISC_EXT_PAP_REQUEST_NOTIFICATION			0x2	//4.0.21.501
#define MISC_EXT_NOT_PAP_REQUEST_NOTIFICATION		~(0x2)

#define MISC_EXT_ANPR_BLOCK							0x0004		//4.0.23.500	TD 69914
#define MISC_EXT_NOT_ANPR_BLOCK						~(0x0004)	//4.0.23.500	TD 69914

#define MISC_EXT_ANPR_FAILED						0x0008		//4.0.26.505	TD 157073
#define MISC_EXT_NOT_ANPR_FAILED					~(0x0008)	//4.0.26.505	TD 157073

#define MISC_EXT_PUMP_TEST							0x0010		//4.0.27.60
#define MISC_EXT_NOT_PUMP_TEST						~(0x0010)	//4.0.27.60

#define MISC_EXT_PUMP_CALIBRATION					0x0020		//4.0.27.60
#define MISC_EXT_NOT_PUMP_CALIBRATION				~(0x0020)	//4.0.27.60

#define	MISC_EXT_MOBILE_PAYMENT							0x0040		//4.0.28.502 - TD 240006/251326
#define	MISC_EXT_NOT_MOBILE_PAYMENT						~(0x0040)	//4.0.28.502 - TD 240006/251326

#define MISC_EXT_AUTHORIZE_PUMP_SENT				0x0080		//4.0.30.500 - TD 320068
#define MISC_EXT_NOT_AUTHORIZE_PUMP_SENT 			~(0x0080)	//4.0.30.500 - TD 320068	

#define	MISC_EXT_MOBILE_APPROVED						0x0100		
#define	MISC_EXT_NOT_MOBILE_APPROVED					~(0x0100)	

#define MISC_EXT_MOBILE_REQUEST_NOTIFICATION			0x0200	
#define MISC_EXT_NOT_MOBILE_REQUEST_NOTIFICATION		~(0x0200)


// Each pump can belong to one group (only) 
// 00, 01, 10, 11 
// 
#define MISC_GROUP_A							 0x00000000
#define MISC_GROUP_B							 0x00001000
#define MISC_GROUP_C							 0x00002000
#define MISC_GROUP_D							 0x00003000
#define MISC_NO_GROUP							 0xffffcfff
// --------------------------------------------------
// MISC is passed as STR_LONG_VAL5 , 
// so the last Flag can be the 16s position =  0x00008000
// --------------------------------------------------


// Combinations:
#define MISC_SHIFT_CHANGE_FLAGS				(MISC_WAIT_LOCK_TIMEOUT | MISC_WAIT_RESPONSE_TIMEOUT | MISC_OPEN_PRE_PAY_TRS | MISC_OPEN_PAY_AT_KIOSK_TRS | MISC_OPEN_PAY_AT_PUMP_TRS | MISC_WAIT_FOR_PUMP_TO_BE_READY)
#define MISC_PRICE_CHANGE_FLAGS				(MISC_WAIT_LOCK_TIMEOUT | MISC_WAIT_RESPONSE_TIMEOUT | MISC_OPEN_PRE_PAY_TRS | MISC_WAIT_FOR_PUMP_TO_BE_READY)
#define MISC_MAINTENANCE_FLAGS				(MISC_WAIT_LOCK_TIMEOUT | MISC_WAIT_RESPONSE_TIMEOUT | MISC_WAIT_FOR_PUMP_TO_BE_READY)
#define MISC_CLEAR_SHIFT_CHANGE_FLAGS		(MISC_IGNORE_FIELD_VALUE & MISC_NOT_WAIT_LOCK_TIMEOUT & MISC_NOT_WAIT_RESPONSE_TIMEOUT & MISC_NOT_OPEN_PRE_PAY_TRS & MISC_NOT_OPEN_PAY_AT_KIOSK_TRS & MISC_NOT_OPEN_PAY_AT_PUMP_TRS & MISC_NOT_WAIT_FOR_PUMP_TO_BE_READY)
#define MISC_CLEAR_PRICE_CHANGE_FLAGS		(MISC_IGNORE_FIELD_VALUE & MISC_NOT_WAIT_LOCK_TIMEOUT & MISC_NOT_WAIT_RESPONSE_TIMEOUT & MISC_NOT_OPEN_PRE_PAY_TRS & MISC_NOT_WAIT_FOR_PUMP_TO_BE_READY)
#define MISC_CLEAR_MAINTENANCE_FLAGS		(MISC_IGNORE_FIELD_VALUE & MISC_NOT_WAIT_LOCK_TIMEOUT & MISC_NOT_WAIT_RESPONSE_TIMEOUT & MISC_NOT_WAIT_FOR_PUMP_TO_BE_READY)
#define MISC_CLEAR_PROCESS_FLAGS			(MISC_CLEAR_SHIFT_CHANGE_FLAGS & MISC_CLEAR_PRICE_CHANGE_FLAGS & MISC_CLEAR_MAINTENANCE_FLAGS)




// PumpSrv reservation extention flags (station status)
#define IDLE					0x00000000
#define PRICE_CHANGE			0x00000001
#define SHIFT_CHANGE			0x00000002
#define PUMP_TOTALS_READING		0x00000004
#define TANK_READING			0x00000008
#define MAINTENANCE				0x00000010
#define EMERGENCY_LOCK	        0x00000020
#define FORCE_SHIFT_OLA	        0x00000040       //4.0.3.39
#define PUMP_MODE_LOCK	        0x00000080       //4.0.3.49
#define PUMP_SINGLE_LOCK        0x00000100       //4.0.5.24
#define DATABASE_MAINTENANCE    0x00000200       //4.0.5550.2000
#define UNLOCK_BY_TIMER		    0x00000400       //4.0.5550.2002

//Wet stock flags (Up to 0x00008000)
#define WET_STOCK_PUMP_TOTALS	0x00000001
#define WET_STOCK_TANKS 		0x00000002
#define WET_STOCK_DELIVERY		0x00000004
#define WET_STOCK_ALARM			0x00000008
#define WET_STOCK_PUMP_CFG		0x00000010
#define WET_STOCK_TANK_CFG		0x00000020
#define WET_STOCK_DELIVERY_START 0x00000040  //4.0.5.24
#define WET_STOCK_DELIVERY_END 0x00000080  //4.0.5.24
#define WET_STOCK_MOST_RECENT_DELIVERY 0x00000200  //4.0.7.500
#define WET_STOCK_DO_MEMO_PROCESS 		0x00000400  //4.0.7.501
#define WET_STOCK_REQUESTED_BY_POS 		0x00000800  //4.0.7.503
#define WET_STOCK_REQUESTED_BY_POS_PUMP	0x00001000  //4.0.16.500
#define WET_STOCK_REQUESTED_BY_REPORT	0x00002000  //4.0.22.501 - TD 69905


//All is not inclode pump and tank confige, because the respond never reach to PumpSrv.
#define WET_STOCK_ALL			(WET_STOCK_PUMP_TOTALS | WET_STOCK_TANKS | WET_STOCK_DELIVERY | WET_STOCK_ALARM |WET_STOCK_REQUESTED_BY_POS|WET_STOCK_REQUESTED_BY_POS_PUMP|WET_STOCK_REQUESTED_BY_REPORT)  //4.0.9.503 //4.0.16.500 //4.0.22.501 TD 69905


//Clear wet stock flags
#define WET_STOCK_CLEAR_PUMP_TOTALS		~(WET_STOCK_PUMP_TOTALS)
#define WET_STOCK_CLEAR_TANKS 			~(WET_STOCK_TANKS | WET_STOCK_REQUESTED_BY_POS | WET_STOCK_REQUESTED_BY_REPORT) // 4.0.9.503 //4.0.22.512 - TD 75378
#define WET_STOCK_CLEAR_DELIVERY		~(WET_STOCK_DELIVERY | WET_STOCK_MOST_RECENT_DELIVERY |WET_STOCK_REQUESTED_BY_POS ) //4.0.9.503
#define WET_STOCK_CLEAR_ALARM			~(WET_STOCK_ALARM)
#define WET_STOCK_CLEAR_PUMP_CFG		~(WET_STOCK_PUMP_CFG)
#define WET_STOCK_CLEAR_TANK_CFG		~(WET_STOCK_TANK_CFG)
#define WET_STOCK_CLEAR_All				~(WET_STOCK_PUMP_TOTALS | WET_STOCK_TANKS | WET_STOCK_DELIVERY | WET_STOCK_ALARM | WET_STOCK_MOST_RECENT_DELIVERY |WET_STOCK_REQUESTED_BY_POS|WET_STOCK_REQUESTED_BY_POS_PUMP|WET_STOCK_REQUESTED_BY_REPORT)  //4.0.9.503 //4.0.16.500 //4.0.22.501 TD 69905
#define WET_STOCK_CLEAR_MOST_RECENT_DELIVERY  ~(WET_STOCK_MOST_RECENT_DELIVERY)  //4.0.7.501
#define WET_STOCK_CLEAR_REQUESTED_BY_POS_PUMP	~(WET_STOCK_REQUESTED_BY_POS_PUMP) //4.0.16.500


//ReConfigure flags
#define RE_CONFIGURE_TERMINALS		0x00000001
#define RE_CONFIGURE_CAR_WASH		0x00000002
#define RE_CONFIGURE_DISPENSERS		0x00000004	//4.0.1.32
#define RE_CONFIGURE_AVI			0x00000008	//4.0.1.32
#define RE_CONFIGURE_PRICE_POLES	0x00000010	//4.0.1.32
#define RE_CONFIGURE_TANK_GAUGE		0x00000020	//4.0.1.32
#define RE_CONFIGURE_EMVCONVERT		0x00000400	//4.0.1.32
#define RE_CONFIGURE_All			0xFFFFFFFF

//service Mode
#define SELF_SERVICE_MODE		0
#define FULL_SERVICE_MODE		1  // ATTEND 
#define DEFAULT_MODE			1  // ATTEND 
#define MAX_SERVICE_LEVEL		1

//Car wash type
#define CW_TYPE_NONE			0x00000000
#define CW_TYPE_GENERATE		0x00000001
#define CW_TYPE_CANCEL			0x00000002
#define CW_TYPE_GET_REPORT		0x00000004
//4.0.23.504 76432 start
#define CW_TYPE_SYNC_FUNC		0x00000008
#define CW_TYPE_SETUP_FUNC		0x00000010
#define CW_TYPE_COLD_START_FUNC	0x00000020
//4.0.23.504 76432 end
#define CW_TYPE_VALIDITY_CHECK	0x00000040 //4.0.26.501 144221



//InitInterface2 - lFlags2
#define INIT_REAL_POS			0x00000001


//GetPAPSwipeData - lFlags
#define GPSD_FOR_CURRENT_PAP			0x00000001
#define GPSD_FOR_CURRENT_DEBIT_PAP		0x00000002  //4.0.0.741
#define GPSD_FOR_STORAGE_PAP_DETAILS    0x00000004  //4.0.5.23
#define GPSD_INSERT_EXTRA_INFO		    0x00000008  //4.0.9.501
#define GPSD_GET_EXTRA_CHIP_INFO		0x00000010  //4.0.20.20

//4.0.14.501
//SetVersion flags
#define SET_VERSION_BY_XML				0x00000001


#define GPSD_FOR_THREE_STRUCT			0x00100000  //4.0.5.0

//4.0.3.21
enum ServerState {
				SERVER_NO_CONVERT,	   // convert layer not available , controller not available
				SERVER_STARTUP,		   // convert layer available, controller not available 
				SERVER_IDLE,		   // convert layer available, controller available 
};
//4.0.3.21

//4.0.23.507 TD 89047
//Server State Extension Bit Map
#define SSEBM_NONE 0x00000000 // none extansion
#define SSEBM_CONVERT_LAYER_GONE 0x00000001 // convert layer is gone
#define SSEBM_SHOW_GPI_MSG_CONVERT_LAYER_GONE 0x00000002 // show convert layer is gone msg on the POSPump while PumpSrv is not Idle
#define SSEBM_QDX_FAIL 0x00000004 // QDX is not ready // 4.0.25.391 TD 288369
#define SSEBM_OUTDOOR_DROP_EXCEEDED_MAXIMUM 0x00000008 // CR 441147 4.0.19.2500
#define SSEBM_VP_GONE 0x00000010 // VP is gone // 4.0.25.391 TD 288369


// Server State Extension Flags
#define SSEF_CONVERT_LAYER_GONE							0x00000001
#define SSEF_SHOW_GPI_MSG_CONVERT_LAYER_GONE			0x00000002		//4.0.23.510 TD 97894







enum SingleTankProcessStopped { //4.0.21.500 TD 66935
	                SINGLE_TANK_PROCESS_NOT_STOPPED,
                    SINGLE_TANK_PROCESS_STOPPED_BY_ABORT,
                    SINGLE_TANK_PROCESS_STOPPED_BY_TIMEOUT,
                    SINGLE_TANK_PROCESS_STOPPED_BY_FAILURE
};

//4.0.22.501 - TD 69905
enum ReportRequestState {
					REPORT_PROCESS_IDLE,
					REPORT_SEND_TANK_READING,
					REPORT_WAIT_TANK_READING,
					REPORT_GOT_TANK_READING_TIMEOUT,
					REPORT_WAIT_SEND_REQUEST,
					REPORT_SEND_REQUEST,
					REPORT_WAIT_GENERATE_REPORT,
					REPORT_READY,
					REPORT_COUNT // MUST BE LAST!!               // TD 351225
};

const char* const ReportRequestState_str[REPORT_COUNT] =          // TD 351225
{
	"REPORT_PROCESS_IDLE",
		"REPORT_SEND_TANK_READING",
		"REPORT_WAIT_TANK_READING",
		"REPORT_GOT_TANK_READING_TIMEOUT",
		"REPORT_WAIT_SEND_REQUEST",
		"REPORT_SEND_REQUEST",
		"REPORT_WAIT_GENERATE_REPORT",
		"REPORT_READY"
};

//4.0.22.501 - TD 69905
enum ReportRequestError {
					REPORT_NO_ERROR,
					REPORT_TANK_READING_TIMEOUT,
					REPORT_ERROR_ALL_TANKS_NOT_AVAILABLE,
					REPORT_ERROR_GENERATE_REPORT_TIMEOUT,
					REPORT_ERROR_GENERATE,
					REPORT_ERROR_COUNT // MUST BE LAST!!        // TD 351225
};

const char* const ReportRequestError_str[REPORT_ERROR_COUNT] =         // TD 351225
{
	"REPORT_NO_ERROR",
		"REPORT_TANK_READING_TIMEOUT",
		"REPORT_ERROR_ALL_TANKS_NOT_AVAILABLE",
		"REPORT_ERROR_GENERATE_REPORT_TIMEOUT",
		"REPORT_ERROR_GENERATE"
};

//4.0.22.503 - TD 72189
enum ChangeProductInTankState
{
					CPIT_IDLE,							//0
					CPIT_START_LOCK_FORECOURT,			//1
					CPIT_LOCK_FORECOURT_IN_PROGRESS,	//2
					CPIT_FORECOURT_LOCKED,				//3
					CPIT_WAIT_CLOSE_OPEN_TRS,			//4
					CPIT_CHANGE_SETUP_AND_SEND_TO_CL,	//5
					CPIT_WAIT_SEND_TO_CL,				//6
					CPIT_WAIT_CL_RESPONSE,				//7
					CPIT_HANDLE_CL_RESPONSE,			//8
					CPIT_SEND_PRICE_CHANGE,				//9
					CPIT_WAIT_UNLOCK_FORECOURT,			//10
};

//4.0.22.503 - TD 72189
enum ChangeProductInTankError
{
					CPIT_NO_ERROR,						// 0 No error
					CPIT_ERROR_LOCK_FORECOURT_FAILED,	// 1 Failed to lock forecourt
					CPIT_ERROR_OPEN_SALES_FOUND,		// 2 Open sales were found
					CPIT_ERROR_CL_RESPONSE_TIMEOUT,		// 3 CL hasn't responded PumpSrv
					CPIT_ERROR_CL_RESPONSE_FAILURE,		// 4 CL hasn't responded PumpSrv
};

//4.0.19.820 - TD 78775
enum PriceChangeWithAckPerPumpError
{
					PCWA_NO_ERROR,							// 0 No error
					PCWA_ERROR_RESPONSE_TIMEOUT,			// 1 PumpSrv timeout for CL not responding		
					PCWA_ERROR_PUMP_OFFLINE,				// 2 Pump is offline (COMMS_TERMINATED)
					PCWA_ERROR_COMM_PUMP_FAILURE,			// 3 Communication with pump failed (0x35)
					PCWA_ERROR_AUTO_PROCESS_FAILURE,		// 4 Pump doesn't support Automatic price change (0x36)
					PCWA_ERROR_CONFIG_NO_SYNC,				// 5 Configuration is not synced (0x37)
};

//4.0.19.820 - TD 78775
enum PriceChangeWithAckPerPumpState
{
					PCWA_IDLE,							//0
					PCWA_PROCESS_STARTED,				//1
					PCWA_WAIT_TIMEOUT,					//2
};


#define LOCK_STATUS_IRRELEVANT -1

//4.0.3.49
enum LockPumpByModeStatus {
						PM_UNLOCK,			// 0 				
						PM_START_LOCK,		// 1
						PM_WAIT_LOCK,		// 2
						PM_LOCKED,			// 3
						PM_START_UNLOCK,	// 4					
						PM_WAIT_UNLOCK		// 5
};

enum LockPumpByTankStatus {
						PTL_UNLOCK,			// 0 				
						PTL_START_LOCK,		// 1
						PTL_WAIT_LOCK,		// 2
						PTL_LOCKED,			// 3
						PTL_START_UNLOCK,	// 4					
						PTL_WAIT_UNLOCK		// 5
};

enum PumpStatus {
					POWER_UP,			    // 0 Initializing
					READY,				    // 1 Ready (idle)
					REQUESTING,			    // 2 					
					AUTHORIZED,			    // 3					
					DISPENSING,			    // 4
					STOPED_BY_OPERATOR,	    // 5
					OUT_OF_SERVICE,         // 6
					NOZZLE_LEFT_OUT,        // 7
					ERROR_TERMINATED,       // 8
					COMMS_TERMINATED,       // 9
					UNKNOWN_STATUS,         // 10
					STOPPED_BY_CONTROLLER   // 11
};






//4.0.5.24
enum TankGaugeDataFlags
{
	TGD_NONE, 
	TGD_TankReading, 
	TGD_DeliveryReport, 
};

enum TankGaugeDBOperation
{
	TGDB_NONE, 
	TGDB_READ, 
	TGDB_WRITE, 
};

// Define  generic Error /warning flags, 
// This flags indicate that the Pos should Read the Alarm form Alarm srv 
#define ALARM_SRV_CODE					90

// Flags indicate that we should send to POS, System alarm combine category 3, 4 5
#define PUMPSRV_SYSTEM_GROUP           100    //4.0.2.37 // 

// Used in the Error report....and for additional information use misc report
enum SystemError {
					SYS_NO_ERROR,					//0
					SYS_ERROR_NON_SPECIFIC,			//1
					SYS_ERROR_CONFIG,				//2
					SYS_ERROR_HW,					//3
					SYS_ERROR_SW,					//4
					SYS_ERROR_COMM,					//5
					SYS_ERROR_TANK_CONFIG,			//6
					SYS_ERROR_MISSING_TANK_READING,	//7
					SYS_ERROR_DELIVERY_FILE_FULL,	//8
					SYS_AUDIT_OFFLINE,				//9
					SYS_AUDIT_OUT_OF_PAPER,			//10	
					SYS_AUDIT_GENERAL_ERROR,		//11
					SYS_POWER_WARNING,				//12
					SYS_POWER_FAILED,				//13
					SYS_SHUTDOWN_POS,				//14
					SYS_SHUTDOWN_POWER,				//15	
					SYS_SHUTDOWN_TASK,				//16
					SYS_SHUTDOWN_CONFIG,			//17
					SYS_SHUTDOWN_LEFT,				//18
					SYS_PROCESS_LOCK,				//19
					SYS_ENCRYPTION_LOAD_FAILED,		//20
					SYS_TANK_ALARM,		//Add by Yaron //21
					SYS_ERROR_OLA,		 // add marik	//22
					SYS_ERROR_DISK_WARNING, //23 - Eyal 4.0.0.42 //23
					SYS_ERROR_DISK_FULL,    //24 - Eyal 4.0.0.42 //24										
};


enum PumpDevice {
					NO_DEVICE,				// 0
					TERMINAL,				// 1	
					TERMINAL_CARD_READER,	// 2
					TERMINAL_PRINTER,		// 3
					TERMINAL_KEYBOARD,		// 4
					TERMINAL_DISPLAY,		// 5
					TERMINAL_SPEAKER,		// 6
					TERMINAL_CASH_ACCEPTOR,	// 7
					DISPENSER,				// 8 
					RFID_ANTENNA,			// 9
					UNKNOWN_DEVICE=20,		// 20
};

enum PumpWarning {
					PUMP_DEVICE_NO_WARNING,						// 0
					PUMP_DEVICE_WARNING_NON_SPECIFIC,			// 1
					PUMP_DEVICE_WARNING_PAPER_LOW,				// 2
					PUMP_DEVICE_WARNING_OUT_OF_PAPER,			// 3
					PUMP_DEVICE_WARNING_PAPER_JAM,				// 4
					PUMP_DEVICE_WARNING_CASH_REJECTED,			// 5
					PUMP_DEVICE_WARNING_CASH_JAMED,				// 6		
					PUMP_DEVICE_WARNING_CASH_CASSETTE_FULL,		// 7			
					PUMP_DEVICE_WARNING_CASH_CASSETTE_REMOVED,	// 8 								
					PUMP_DEVICE_WARNING_CARD_ENCRYPTION,		// 9
					PUMP_DEVICE_WARNING_OFFLINE,				// 10 
					PUMP_DEVICE_IPT_WARNING,					//11 4.0.10.1000
};

enum PumpError {
					PUMP_NO_ERROR,				// 0
					PUMP_ERROR_NON_SPECIFIC,	// 1 
					PUMP_ERROR_CONFIG,			// 2 
					PUMP_ERROR_BLENDER,			// 3 
					PUMP_ERROR_CASH_READER,		// 4 cash acceptor
					PUMP_ERROR_CARD_READER,		// 5 card reader					
					PUMP_ERROR_OFFLINE,				// 6 offline
					PUMP_ERROR_UNEXPECTED_STATE,	// 7 Unexpected state
					PUMP_ERROR_TRS,					// 8 Transaction data error
					PUMP_ERROR_OVER_FUELING,		// 9 over fueling
};

enum PumpMessage {
					PUMP_NO_MSG,							// 0
					PUMP_MSG_TAG_CHALLANGE_FAIL,			// 1
					PUMP_MSG_TAG_GONE,						// 2
					PUMP_MSG_TAG_NOT_FOUND,					// 3
					PUMP_MSG_TAG_CHANGED,					// 4
					PUMP_MSG_TAG_READ,						// 5
					PUMP_MSG_CAR_TAG_READ,					// 6
					PUMP_MSG_HAND_TAG_READ,					// 7
					PUMP_MSG_TAG_CANCELLED,					// 8
					PUMP_MSG_TAG_ERROR,						// 9
					PUMP_MSG_CARD_ENCRYPTION,				// 10
					PUMP_MSG_STOCK_PRE_PAY,					// 11 
					PUMP_MSG_GENERAL_SPEED_PASS_DECLINE,    // 12			
					PUMP_MSG_TAG_SALE_CANCELED,				// 13	//3.0.2.3
					PUMP_MSG_TAG_SALE_DECLINED,				// 14	//3.0.2.3					
};


enum PumpMisc {
					PUMP_MISC_EVENT_HELP_KEY,					// 0
					PUMP_MISC_EVENT_CANCEL_KEY,					// 1
					PUMP_MISC_EVENT_CARD_SWIPE_ERROR,			// 2
};


enum PostPayState {	
					STATE_NOT_ON_OFFER,	      //(0) UnCheckOut - Not on offer or accepted by customer
					STATE_ON_OFFER,		      //(1) CheckOut  - on offer at single POS
					STATE_ACCEPTED,		      //(2) Accepted by customer 
					STATE_FORCE_RELEASE,      //(3)
					STATE_PAID,			      //(4) Paid at POS  				
					STATE_SUPRESS_TIMOUT,     //(5)
					STATE_FORCE_REMOVE,       //(6)
					STATE_FORCE_PAID,         //(7)
					STATE_PAID_LOYLATY,       //(8) 4.0.11.503 Agip paid in injenco 				
					STATE_RELEASE_CONTROLLER, //(9) 4.0.19.504 - 47559
					STATE_FORCE_REPAIR,       //    4.0.21.12  - 66982
};


enum AVINextState {
					AVI_DO_NOT_UPDATE_STATUS,	
					AVI_UPDATE_STATUS,			
					AVI_STOP_PUMP,				
};

enum CarWashProgramSelltatus{
	OK_TO_SELL_PROGRAMS,
	DEVICE_OFFLINE_CAN_SELL_PROGRAM,
	DO_NOT_SELL_CW_PROGRAMS,
	NO_VALID_PROGRAMS_TO_SELL
};



enum CWResult {
				CW_OK_VALID_CODE,
				CW_DATA_NOT_YET_AVAILABLE,
				CW_OPERATION_FAILED,
				CW_CODE_CANCELED,
				CW_NOT_ISSUED,		//4.0.26.501 144221
				CW_USED,			//4.0.26.501 144221
				CW_EXPIRED,			//4.0.26.501 144221
				CW_CLEARED,			//4.0.26.501 144221
				CW_INVALID_CODE,	//4.0.26.501 144221
				CW_DEVICE_OFFLINE,	//405461

};

/////////////////////////////////////////////////////////////////////////////
//
//	COMMON FOR ALL INTERFACES
//
/////////////////////////////////////////////////////////////////////////////

enum ServiceFeeRetCode
{
	SERVICE_FEE_OK,
	SERVICE_FEE_NOT_VALID,
	SERVICE_FEE_INVALID_PUMP_NUMBER,
	SERVICE_FEE_TRS_NOT_FOUND,
	SERVICE_FEE_PARAM_NOT_FOUND,
};

//4.0.0.38
// Cash acceptor flags 
#define CASH_ACCEPTORS_TRANSACTION_CREATED		   0x0000
#define CASH_ACCEPTORS_TRANSACTION_UPDATE_BILLS    0x0001
#define CASH_ACCEPTORS_TRANSACTION_UPDATE_FINAL	   0x0002
#define CASH_ACCEPTORS_TRANSACTION_CANCELED		   0x0004
#define CASH_ACCEPTORS_TRANSACTION_FORCE_CANCELED  0x0008
#define CASH_ACCEPTORS_UNRECOGNIZED_BILL		   0x0010 //4.0.17.90 - TD 40564

// Get Service Fee operation flags
#define GET_FEE_BY_TRS_NUM				0x0001
#define GET_FEE_BY_TIME					0x0002
#define GET_FEE_BY_TRANSACTION_TYPE		0x0003


#define GET_FULL_SERVICE_FEE				0x40000
#define GET_SELF_SERVICE_FEE				0x80000


// Get Service Fee TrsType
#define SERVICE_TYPE_PAK				0x0000
#define SERVICE_TYPE_PREPAY				0x0001
#define SERVICE_TYPE_PAP				0x0002 
#define SERVICE_TYPE_AVI				0x0003 	
#define	SERVICE_TYPE_ATTEND				0x0004


// AuthorizeAndLimit
#define LIMIT_FILL_UP										   0x0001
#define LIMIT_BY_VALUE										   0x0002
#define LIMIT_BY_VOLUME										   0x0004
#define RE_AUTHORIZE_TIMEOUT								   0x0008
#define	GRADE_PRICE_IS_GRADE_CODE							   0x0010
#define	GRADE_PRICE_IS_POSITION_NUM							   0x0020
#define	SERVICE_FEE_CHARGED_BY_POS							   0x0040
#define IGNORE_VIT_DETECTION								   0x0080
#define PREPAY_FORCE										   0x0100	//Version 3.1.1.1
#define LOYALTY_DISCOUNT_DATA								   0x0200	//3.1.1.5
#define PREPAY_PROMPT_DISABLE								   0x0400	// 
#define FORCE_IGNORE_VIT									   0x0800	//4.0.5.41
#define PREPAY_CANCEL										   0x1000
#define PREPAY_CHANGE_TO_PAY_AT_KIOSK						   0x2000
#define PAK_WITH_LIMIT										   0x4000
#define PREPAY_MOVE											   0x8000	// CASH_ACCEPTOR_TRS//4.0.11.500 MARIK 
#define LOYALTY_DISCOUNT_STRUCT2							  0x10000   //4.0.5.26
#define LIMIT_FLEET											  0x20000   //4.0.9.507
#define LIMIT_REEFER										  0x40000   //4.0.9.507
#define	CASH_ACCEPTOR_TRS									  0x80000   //4.0.11.500
#define PAK_BOTH											 0x100000
#define PREPAY_WITH_XML_BUFF								 0x200000	//4.0.12.501
#define FORCE_PAK_WHEN_NOT_IN_MODE							 0x400000	//4.0.16.500 - TD 34675
#define CHECK_AND_RESERVE_FLAG								 0x800000	//4.0.17.20 - TD 26384
#define PREPAY_INCLUDES_GRADES_RESTRICTIONS					0x1000000	//4.0.23.270 111746 //4.0.24.60
#define PREPAY_WITH_XML_BUFF_WITH_LOYALTY_DISCOUNT_DATA		0x2000000	//4.0.24.71 No TD
#define PREPAY_WITH_LOYALTY_PRICE_PROTECTION				0x4000000   // [18/10/2020 MaximB] BUG TODO  NEED TO FIX was TRS_IS_PUMP_TEST	//4.0.27.60	TD 231472 
#define TRS_IS_PUMP_CALIBRATION								0x8000000	//4.0.27.60	TD 231472
#define CHECK_WITHOUT_RESERVE								0x10000000	//4.0.19.1250 - TD 236644
#define PREPAY_MOBILE_PAYMENT								0x20000000	//4.0.28.500 - TD 240006/251326
#define PREPAY_WITH_TENDER_INFO								0x40000000  // 4.0.25.600   CR 296433
#define MULTIPLE_VOLUME_AND_AMOUNT_LIMIT					0x80000000
#define TRS_IS_PUMP_TEST									0x100000000   // [18/10/2020 MaximB] BUG TODO  NEED TO FIX was TRS_IS_PUMP_CALIBRATION  THIS IS LARGE THAN LONG

// Grade Price Level
#define PRICE_LEVEL_DEFAULT				0 
#define PRICE_LEVEL_CASH				1
#define PRICE_LEVEL_CREDIT				2
#define PRICE_LEVEL_FIFTH_PRICE			3 //TD 354838
#define MAX_PRICE_LEVEL					3


//4.0.12.505 Grade Data
#define GPG_ALL_GRADES_DATA				0x00000001
#define GPG_ALL_GRADES_OLD_PRICES		0x00000002 //4.0.26.508 147183
#define GPG_GRADES_POSITIONS			0x00000004	//4.0.25.1130 

// Price Limit By User
#define LIMIT_BY_DEFAULT				0 
#define DEBIT_LIMIT_BY_USER				1 
#define CREDIT_LIMIT_BY_USER			2

// Price Level Selection
#define PRICE_LEVEL_ALLOW_DEFAULT		0 
#define PRICE_LEVEL_FORCE_SELECT		1
#define PRICE_LEVEL_CONFIRM				2



// Process list 

#define PROCESS_LOCK					1 // used as Maint Mode process.
#define PROCESS_HELP					2 // used by POS to remove help request
#define PROCESS_REMOVE_MSG				3 // used by POS to remove message
#define PROCESS_REMOVE_EVENT			4 //3.0.3.11
#define PROCESS_REMOVE_LATEST_PREPAY	5 //3.1.1.4
#define PROCESS_UPDATE_LOYALTY_ACTIVITY 6 //3.2.0.31
#define PROCESS_DELIVERY				7 //4.0.5.24
#define PROCESS_PRINTER_STATUS			8 //4.0.5.44
#define PROCESS_CLEAR_ULLAGE_REPORT		9 //4.0.9.503
#define PROCESS_CLEAR_DELIVERY_REPORT   10 //4.0.9.503
#define PROCESS_DECLINE_PAP				11 //4.0.12.502
#define PROCESS_TERMINAL_SW_RESET       12//4.0.10.1000
#define PROCESS_CHANGE_PRICES 			13 //4.0.12.515
#define PROCESS_TERMINAL_SW_DOWNLOAD    14 //4.0.13.503 
#define PROCESS_GET_SHIFT_GPI		    15 //4.0.16.500 
#define PROCESS_CLOSE_BATCH				16 //4.0.16.502 - TD 35765
#define PROCESS_FORCE_RELEASE_TRS		17 //4.0.20.0
#define PROCESS_RECONFIGURE				18

#define PROCESS_OLA_CLOSE_BATCH			400 //4.0.19.502 - TD 58096
#define PROCESS_PUMP_RESET				401	//4.0.19.502 - TD 58649
#define PROCESS_DELIVERY_SINGLE_TANK	402	//4.0.21.500 - TD 66935
#define PROCESS_CHANGE_PRODUCT_IN_TANK	403	//4.0.22.503 - TD 72189
#define PROCESS_ADD_CASHIER 			404	//4.0.22.504 - TD 70956
#define PROCESS_REMOVE_CASHIER			405	//4.0.22.504 - TD 70956
#define PROCESS_RESET_CASHIER			406	//4.0.22.506 - TD 70956
#define PROCESS_PRICE_CHANGE_WITH_ACK_PER_PUMP	407	//4.0.19.820 - TD 78775
#define PROCESS_TERMINAL_LOGON          408 //4.0.28.500  - TD 256429
#define PROCESS_ANY						409 // 4.0.28.501 TD 254219
#define PROCESS_TERMINAL_HW_RESET 		410 //4.0.31.??
#define PROCESS_OLA_ONLINE 	        	411 //4.0.31.?? - R10 User Story - 92780
#define PROCESS_OLA_OFFLINE				412 //4.0.31.?? - R10 User Story - 92780
#define PROCESS_NOZZLE_UP				413 // TD 471110 

#define PROCESS_EXT_SET_CASHIER_INFO		1

enum MiddleOfProcessId
{
	MIDDLE_OF_PRICE_CHNAGE_PROCESS = 1,
		MIDDLE_OF_PRICE_CHANGE_WITH_ROLLBACK_PROCESS,
		MIDDLE_OF_SHIFT_CHANGE_PROCESS,
		MIDDLE_OF_CHNAGE_MODE_PROCESS,
		MIDDLE_OF_QDX_MAINT_PROCESS,
		MIDDLE_OF_WETSTCOK_PROCESS,
		MIDDLE_OF_WETSTOCK_BY_MEMO_PROCESS,
		MIDDLE_OF_DELIVERY_EMULATION_PROCESS,
		MIDDLE_OF_DELIVERY_EMULATION_SINGLE_TANK_PROCESS,
		MIDDLE_OF_DELIVERY_CHANGE_PRODUCT_IN_TANK_PROCESS,
}; // TD 254219  4.0.28.501


//ANPR commands
#define ANPR_UNBLOCK_PUMP				1	//4.0.23.500 71727
#define ANPR_OVERRIDE_PUMP_BLOCK		2	//4.0.23.500	76438

//4.0.12.515
enum ProcessPriceChangeStatusNotification {	
		PROCESS_PRICE_CHANGE_SUCCESS,   //0	 - Got from CL
		PROCESS_PRICE_CHANGE_FAILURE,   //1  - Got from CL
		PROCESS_PRICE_CHANGE_COMMIT,	//2 //4.0.26.508 147183  - we got commit from POS
		PROCESS_PRICE_CHANGE_COMPLETE,	//3  - Got from POS
		PROCESS_PRICE_CHANGE_MSG_ACK,   //4 //4.0.27.42 - Got from POS
		PROCESS_PRICE_CHANGE_NO_CHANGE, //5   //4.0.23.1100
};


//4.0.5.24
//Table #6 DELIVERY Process flags 
#define PD_NONE						0x000000
#define PD_START					0x000001     
#define PD_END						0x000002
#define PD_FORCE_RELEASE			0x000003
//4.0.5.24

//Delivery emulation single tank 4.0.21.500 TD 66935
enum ProcessDeliverySingleTank {
		PD_SINGLE_TANK_NONE,					
		PD_SINGLE_TANK_START,					     
		PD_SINGLE_TANK_END,
		PD_SINGLE_TANK_FORCE_RELEASE,
		PD_SINGLE_TANK_CANCEL, //4.0.23.500 - TD 74964
};

//4.0.17.20 - TD 26384
enum CheckAndReserveStatus {
				CHECK_RESERVE_STATUS_NONE,
				CHECK_RESERVE_STATUS_START,
				CHECK_RESERVE_STATUS_END,
};

// Process Parameters 
#define PROCESS_OFF						0
#define PROCESS_ON						1


// Service 
#define ALL_SERVICES					0
#define OLA_SERVICE						1

// Service Status 
#define SERVICE_OFF						0
#define SERVICE_IDLE					1 	// can be shut down
#define SERVICE_IN_MIDDLE_OF_OPERATION	2  	// in middle of operation 

// Maintenance Process (MP)
#define MP_NONE							0
#define MP_LOCK							1
#define MP_UNLOCK						2
#define MP_FORCED_UNLOCK                3
#define MP_EMERGENCY_LOCK				4    //4.0.3.27
#define MP_DB_MAINTENANCE_LOCK			5    //4.0.5550.1504
#define MP_LOCK_BY_UPS					0x10000


// Wet Stock Mode (WSM)
#define WSM_NONE						0
#define WSM_BY_INTERVAL					1
#define WSM_BY_TIME						2
#define WSM_BY_MEMO						3	// Add by Yaron
#define WSM_BY_START_DELIVERY			4	// Add by Udi - 4.0.5.24
#define WSM_BY_END_DELIVERY				5	// Add by Udi - 4.0.5.24
#define WSM_BY_DEMAND_MEMO 				6	// 4.0.7.501
#define WSM_BY_REPORT 					7	// 4.0.22.501 TD 69905


//Memo Records types
#define TANK_MEMO_WITH_DELIVERY			1	 

const short TRS_NUMBER_LENGTH = 5;

// Get receipt flags
#define RECEIPT_INDEX_IS_TRS            0x000001
#define RECEIPT_ORIGINAL_PRINTED		0x000002   //3.2.0.22
#define RECEIPT_GET_LATEST_RECEIPT		0x000004   //3.2.0.22
#define RECEIPT_FORMAT_WITH_ITEMS       0x000008
#define RECEIPT_THREE_OLA_REC           0x000010  //4.0.5.0
#define RECEIPT_WITH_EXTRA_ITEM         0x000020  //4.0.5.27
#define RECEIPT_WITH_RECEIPT_EXTRA_DATA 0x000040  //4.0.5.39
#define RECEIPT_GET_LAST_MEMO_TRACK2	0x000080  //4.0.5.506
#define RECEIPT_CAR_REGISTRATION_INDEX	0x000100  //4.0.7.500
#define RECEIPT_IS_NONE_CREDIT_KEY_PCI	0x000200  //4.0.9.503		//not in use
#define RECEIPT_ATTENDANT_IDX			0x000400  //4.0.11.503
#define RECEIPT_TRACK2_TO_ACCOUNT_NUM	0x000800  //4.0.14.501
#define RECEIPT_UNRECOGNIZED_BILL		0x001000  //4.0.18.504 Merge from 4.0.17.192
#define RECEIPT_GET_ORIGINAL    		0x002000  //4.0.15.350
#define RECEIPT_BY_ACCOUNT_NUM			0x004000  //4.0.20.501 TD 60236
#define RECEIPT_INDEX_IS_RECEPT_KEY		0x008000  //4.0.20.502 (2)
#define RECEIPT_GET_FOR_REAL_POS		0x010000	//4.0.26.503
#define RECEIPT_TRACK2_IS_BARCODE		0x020000  //TD 219297
#define RECEIPT_TRACK2_HOLDS_TRS_NUM	0x040000  // TD 270322 //4.0.30.0
#define RECEIPT_PRINT_ON_TERMINAL		0x080000


// Attendant Clock Flags
#define ATTENDANT_CLOCK_IN			0x000001
#define ATTENDANT_CLOCK_OUT			0x000002
#define ATTENDANT_SIGN_IN			0x000004
#define ATTENDANT_SIGN_OUT			0x000008
#define ATTENDANT_BREAK_IN			0x000010 //(future)
#define ATTENDANT_BREAK_OUT			0x000020 //(future)


/////////////////////////////////////////////////////////////////////////////
//
//	G P I	I N T E R F A C E
//
/////////////////////////////////////////////////////////////////////////////


#define GPI_VERSION_1						1
#define GPI_LATEST_VERSION					GPI_VERSION_1
#define PUMPSRV_PUMPSTAT_VERSION_24			"PumpSrv PumpStat Version    2.4" 
#define PUMPSRV_PUMPSTAT_VERSION_29			"PumpSrv PumpStat Version    2.9"
#define PUMPSRV_PUMPSTAT_VERSION_30         "PumpSrv PumpStat Version    3.0"
#define PUMPSRV_PUMPSTAT_CURRENT_40			"PumpSrv PumpStat Version    4.0" 
#define PUMPSRV_PUMPSTAT_CURRENT_405		"PumpSrv PumpStat Version  4.0.5" //4.0.9.507
#define PUMPSRV_PUMPSTAT_CURRENT_409		"PumpSrv PumpStat Version  4.0.9" //4.0.9.507
#define PUMPSRV_PUMPSTAT_CURRENT_VERSION	"PumpSrv PumpStat Version 4.0.14" //4.0.14.500


#define PUMPSRV_GPI_VERSION					"GPI 1.00 GCI 1.00 Control 1.00" // The len of this field should be FILE_VALIDATION_LEN

enum PosCmdState {	
				NO_POS_CMD,
				IN_QUEUE,
				SENT,					
				NOT_SENT,
				ACKED,
				NACKED,				
};

 

enum GPIRetCode {

				GPI_OK,									// 0
				GPI_POS_ALREADY_CONNECTED,				// 1
				GPI_DUPLICATE_POS_NUMBER,				// 2
				GPI_MSG_SENT,							// 3
				GPI_NOT_SUPPORTED,						// 4
				GPI_SERVER_NOT_READY,					// 5  Convert layer not available.
				GPI_INVALID_PUMP_NUMBER,				// 6 
				GPI_PUMP_NOT_REQUESTING,				// 7 pump not requesting or already authorized
				GPI_PUMP_NOT_SUSPENDED,					// 8
				GPI_PUMP_TRS_LIST_FULL,					// 9		
				GPI_TRS_LOCKED,							// 10 trs already on offer or accepted by another POS		
				GPI_TRS_NOT_RESERVED,					// 11 trs must be reserved by calling POS
				GPI_TRS_NOT_FOUND,						// 12
				GPI_TRS_NOT_FOR_SALE,					// 13
				GPI_TRS_NOT_PREPAY,						// 14
				GPI_UNKNOWN_TRS_STATE,					// 15
				GPI_PUMP_ALREADY_SUSPENDED,				// 16
				GPI_MUST_INIT_FIRST,					// 17
				GPI_INVALID_POS_NUMBER,					// 18
				GPI_ONE_OF_PUMP_TRS_ON_OFFER,			// 19 not allowed to offer transaction 
														// if one of pump trs are currently on offer
				GPI_FAILED_PREPAY_IO,					// 20
				GPI_TRS_WAIT_IN_LIST,					// 21
				GPI_TRS_FIRST_IN_LIST,					// 22
				GPI_TRS_ALREADY_IN_PROGRESS,			// 23
				GPI_DUPLICATE_SYNC_NUMBER,				// 24
				GPI_CANT_CANCEL_TRS,					// 25
				GPI_TRS_NOT_TIMEOUT,					// 26
				GPI_PUMP_NOT_IN_WORKING_STATE,			// 27
				GPI_PUMP_LOCKED,						// 28
				GPI_FAILED_IO,							// 29
				GPI_PUMP_ALREADY_RESERVED,				// 30 // 12.7.98
				GPI_TRS_NOT_ACCEPTED,					// 31
				GPI_PUMP_OUT_OF_VIEW,					// 32
				GPI_INVALID_PUMP_MODE,					// 33
				GPI_NOT_VALID_IN_CURRENT_MODE,			// 34
				GPI_TABLE_NAME_TOO_LONG,				// 35 
				GPI_FIELD_NAME_TOO_LONG,				// 36				
				GPI_FAILED_TO_OPEN_TABLE,				// 37
				GPI_PARAM_NOT_FOUND_FAILED_TO_CREATE,	// 38
				GPI_PARAM_NOT_FOUND,					// 39	
				GPI_INVALID_TABLE_NAME,					// 40
				GPI_INVALID_FIELD_NAME,					// 41
				GPI_INVALID_PROCESS_NUMBER,				// 42
				GPI_INVALID_CAR_WASH_CODE,				// 43
				GPI_LAST_ERROR,							// 44
				GPI_CAR_WASH_NOT_ACTIVE,				// 45
				GPI_RESTART_PUMP_NOT_ALLOWED,			// 46
				GPI_WAIT_FOR_CONTROLLER_TRS_UPDATE,		// 47 
				GPI_NO_RECEIPT,							// 48
				GPI_SERVICE_FEE_NOT_VALID,              // 49	
				GPI_INVALID_GRADE_NUMBER,				// 50
				GPI_INVALID_LEVEL_NUMBER,				// 51
				GPI_INVALID_SERVICE_MODE_NUMBER,		// 52
				GPI_INVALID_TANK_NUMBER,				// 53 - Add by Yaron
				GPI_TANK_NOT_VALID,						// 54 - Add by Yaron
				GPI_POS_VOID_NOT_ALLOWED,				//55  //POsVoid12345 
				GPI_DUPLICATE_TRS,						//56	
				GPI_TRS_PARAM_NOT_VALID,				//57
				GPI_CANNOT_PRINT_RECEIPT,               //58  3.2.0.67 Eyal
				GPI_SERVER_IN_MIDDLE_LOCK_SHIFT,		//59  //4.0.1.40
				GPI_ALARMSRV_NOT_ACTIVE,				//60
				GPI_ALARMSRV_FAIL_RETRIEVED_DATA,		//61
				GPI_INVALID_SCHEME_NUMBER,              //62
				GPI_PUMP_STOPPED_BY_CONTROLLER,         //63
				GPI_PREPAY_REQUEST_PASSED_FORECOURT_LIMIT,//64
				GPI_CANT_RESERVEPUMP,                     //65 //4.0.5.24 
				GPI_ACCUMULATE_SHIFT_TOTAL_NOT_ACTIVE     = 66,//66	
				GPI_RECORD_NOT_FOUND,			
				GPI_CAR_WASH_IN_MIDDLE_OF_ANOTHER_SESSION,//68 //4.0.7.502
				GPI_PUMP_NOT_READY_FOR_MODE_CHANGE,		  //69
				GPI_SERVER_IN_MIDDLE_RECOVERY_PROCESS = 81,//81 
				GPI_CONTROLLER_NOT_AVAILABLE          = 90,//90 Forecourt controller not available.
				GPI_SYSTEM_DOWN_PAK_ENABLED           = 91,//Although pumps not available allow PAK to be finalized
				GPI_SYSTEM_DOWN_QDEX_NOT_READY        = 92,//PumpSrv moved to startup because runtime qdex(PumpStat) is not ready.
				GPI_DELIVERY_START_AFTER_START        = 93,
				GPI_DELIVERY_END_AFTER_END            = 94,
				GPI_DELIVERY_START_IN_MIDDLE_OF_END   = 95,
				GPI_DELIVERY_END_IN_MIDDLE_OF_START   = 96,
				GPI_IN_MIDDLE_OF_PROCESS              = 97, //4.0.16.500
				GPI_CASH_ACCEPTOR_REPORT_NOT_ACTIVE   = 98, //4.0.16.502 - TD 35765
				GPI_SERVER_NOT_AVAILABLE              = 99, //PumpSrv not available - 99 will never be used from PumpSrv,
				GPI_UNKNOWN_PUMP_RESERVATION,			    //4.0.17.20 - TD 26384
				GPI_IN_MIDDLE_OF_QDX_MAINT            = 101,//33898
				GPI_NO_LOYALTY_SUMMARY,				        //4.0.19.505 Copient + Rtaps
				GPI_DELIVERY_INVALID_REFERENCE_NUMBER,      //4.0.21.500 TD 66935
				GPI_DELIVERY_END_WITHOUT_START,             //4.0.21.500 TD 66935
				GPI_DELIVERY_WAITING_FOR_END,               //4.0.21.500 TD 66935
				GPI_DELIVERY_READING_PROCESS_DONE,          //4.0.21.500 TD 66935
				GPI_DELIVERY_ABORTING_IN_MIDDLE_OF_PROCESS, //4.0.21.500 TD 66935
				GPI_MAX_CONCURRENT_TRS_HAS_BEEN_REACHED,    //4.0.21.500 69203
				GPI_PUMP_POST_VOID_TRS_LIST_FULL,			//CR 351687: Enhanced post void transactions
				GPI_DUPLICATE_PURCHASE_ID					//TD  382012

				
};

#define GPI_EVENT_AUTO_RECONCILE 1 // TD 233242 4.0.23.1050

//Eyal 3.2.0.67
#define RECEIPT_NOT_FOUND  0
#define RECEIPT_FOUND      1
#define TOO_MANY_RECEIPTS  2

// GetPrePaidTrs Flags:
#define  PREPAY_GET_ANY								0x0000	// ANY - will return first closed (paid) trs 
#define  PREPAY_GET_BY_SYNC_NUMBER					0x0001
#define  PREPAY_GET_BY_PUMP_NUMBER					0x0002	// Not in use...
#define  PREPAY_GET_NEXT_INDEX	  					0x0004	//4.0.11.20 
#define  PREPAY_GET_BY_PURCHASE_ID_PAID_ONLY		0x0008	//4.0.20.500 
#define  PREPAY_GET_BY_PURCHASE_ID					0x0010	//4.0.20.500 
#define  PREPAY_IGNORE_EXTENDED_GRADE_ID_CONVERSION 0x0020	//4.0.24.90 - TD 116227

#define  PREPAY_GET_OPTIONS_MASK (PREPAY_GET_ANY + PREPAY_GET_BY_SYNC_NUMBER + PREPAY_GET_BY_PUMP_NUMBER + PREPAY_GET_NEXT_INDEX + PREPAY_GET_BY_PURCHASE_ID_PAID_ONLY + PREPAY_GET_BY_PURCHASE_ID + PREPAY_IGNORE_EXTENDED_GRADE_ID_CONVERSION)  //4.0.11.30, 4.0.19.180 //4.0.20.500

#define  PREPAY_GET_BY_STRUCT_TRANSACT3	0x0100 


// GetPumpInfo3 Flags:
#define  STRUCT_IS_TAG_PUMP3		0x0002
#define  STRUCT_IS_TAG_PUMP4		0x0004
#define  STRUCT_IS_TAG_PUMP5		0x0008
#define  STRUCT_IS_TAG_PUMP6		0x0010  //4.0.9.600
#define  STRUCT_IS_TAG_PUMP7		0x0020  //4.0.9.600
#define  STRUCT_IS_TAG_PUMP8		0x0040  //4.0.15.500
#define  STRUCT_IS_TAG_PUMP9		0x0080  //TD 38654
#define  START_TERMINAL_MSG         0x4000
#define  END_TERMINAL_MSG           0x8000
#define  IGNORE_SERVER_IN_MIDDLE_RECOVERY	0x00010000 //4.0.19.504 - TD 59460

//GetMinimalPumpInfo3
#define USE_TAG_MINIMAL_ALL_PUMPS_4							0x00000040
#define ENABLE_PAK_PAYMENT_WHEN_PUMPSRV_IN_STARTUP			0x00000080
#define USE_TAG_MINIMAL_ALL_PUMPS_5							0x00000100	//TD 37244

//4.0.1.40

/////////////////////////////////////////////////////////////////////////////
//
//	G C I	A N D    G C I - E V E N T S		I N T E R F A C E
//
/////////////////////////////////////////////////////////////////////////////

#define GCI_IGNORE_FIELD_VALUE	9999 // GCI::SetPumpStatus

#define GCI_VERSION_1			1
#define GCI_LATEST_VERSION		GCI_VERSION_1


enum GCIRetCode {

				GCI_OK,									// 0
				GCI_MUST_INIT_FIRST,					// 1
				GCI_INVALID_PUMP_NUMBER,				// 2
				GCI_UNKNOWN_PUMP_STATUS,				// 3
				GCI_UNKNOWN_PUMP_RESERVATION,			// 4
				GCI_UNKNOWN_PUMP_DEVICE,				// 5				
				GCI_UNKNOWN_TRANSACTION_STATUS,			// 6
				GCI_PUMP_TRS_LIST_IS_FULL,				// 7 
				GCI_PUMP_ALREADY_HAVE_IN_PROGRESS_TRS,	// 8 
				GCI_TRS_NOT_FOUND,						// 9
				GCI_TRS_NOT_CURRENT,					// 10
				GCI_NO_WAITING_TRS,						// 11
				GCI_CANT_CANCEL_TRS,					// 12
				GCI_INVALID_TANK_NUMBER,				// 13
				GCI_INVALID_GRADE_NUMBER,				// 14
				GCI_NOT_SUPPORTED,						// 15
				GCI_PUMP_NOT_IN_WORKING_STATE,			// 16
				GCI_INVALID_SEQ_NUMBER,					// 17
				GCI_TRS_DUPLICATE_UPDATE,				// 18
				GCI_TRS_ALREADY_PAID,					// 19
				GCI_TRS_MISMATCH,						// 20 when updating PAPReady instead of PAKReady or opposite way 
				GCI_FAILED_IO,							// 21
				GCI_REPETED_PAP_SWIPE_REQUEST,			// 22					
				GCI_TABLE_NAME_TOO_LONG,				// 23 
				GCI_FIELD_NAME_TOO_LONG,				// 24
				GCI_DEFAULT_FIELD_VAL_TOO_LONG,			// 25
				GCI_FAILED_TO_OPEN_TABLE,				// 26 Cant Open Key, Param set to default 
				GCI_PARAM_NOT_FOUND,					// 27 NOt Found, Didn't create, according to request
				GCI_PARAM_NOT_FOUND_AND_CREATED,		// 28 Not Found, Created and set to given default 
				GCI_PARAM_NOT_FOUND_FAILED_TO_CREATE,	// 29 Cant create new param , Param set to default
				GCI_PAP_REQUEST_OUT_OF_SYNC,			// 30
				GCI_NOT_VALID_IN_CURRENT_MODE,			// 31
				GCI_INVALID_LEVEL_NUMBER,				// 32
				GCI_INVALID_NOZZLE_NUMBER,				// 33
				GCI_INVALID_PUMP_MODE,					// 34
				GCI_INVALID_INDEX_NUMBER,				// 35
				GCI_AVI_CONFLICT_STATUS,				// 36 current status is in conflict with AVI request
				GCI_LAST_ERROR,							// 37
				GCI_CARD_NOT_APPROVED,					// 38
				GCI_FIELD_EMPTY,						// 39
				GCI_INVALID_VALUES,						// 40
				GCI_NO_CAR_WASH_REQUEST,				// 41
				GCI_TRS_CONVERTED_TO_PAK,				// 42
				GCI_NO_RECEIPT,							// 43	
				GCI_TRS_CONVERTED_TO_PAP,				// 44 3.0.0.8
				GCI_UPDATE_OLA_MEM_INFO,				// 45	
				GCI_PUMP_IN_SELF_SERVICE,               // 46
				GCI_PARALLEL_AVI_NOT_ALLOWED,			// 47
				GCI_OLA_NOT_ACTIVE,						// 48			
				GCI_SERVICE_FEE_NOT_VALID = 49,         // 49
				GCI_TRS_LOCKED,							// 50   //3.2.0.76
				GCI_TRS_NOT_FOR_SALE,					// 51   //3.2.0.76 
				GCI_TRS_NOT_RESERVED,					// 52   //3.2.0.76 
				GCI_ONE_OF_PUMP_TRS_ON_OFFER,			// 53   //3.2.0.76 
				GCI_TRS_NOT_ACCEPTED,					// 54   //3.2.0.76 
				GCI_UNKNOWN_TRS_STATE,					// 55   //3.2.0.76
				GCI_ALARMSRV_NOT_ACTIVE,				// 56	//4.0.1.40				
				GCI_INIT_DEBIT_IN_PROCESS,				// 57   //4.0.2.15
				GCI_SERVER_IN_MIDDLE_LOCK_SHIFT    = 59,			//4.0.1.40		
				GCI_OPEN_PRE_PAY_TRS,					// 60  Cant close shift 
				GCI_OPEN_PAY_AT_KIOSK_TRS,				// 61 Cant close shift 
				GCI_OPEN_PAY_AT_PUMP_TRS,				// 62 Cant close shift 
				GCI_INVALID_DAYOFWEEK,					// 63	
				GCI_PUMP_NOT_READY,						// 64
				GCI_SERVER_NOT_READY,					// 65 4.0.2.36
				CGI_INVALID_LOCK_OPTION,				// 66 4.0.2.36
				GCI_IN_MIDDLE_OF_PROCESS,				// 67 4.0.2.36
				GCI_CONTROLLER_NOT_AVAILABLE,			// 68 4.0.2.36
				GCI_FORCE_CLEAR_PAP_TRS,				// 69 4.0.3.65
				GCI_FORECOURT_EMERGENCY_LOCKED,         // 70 4.0.5.34
				GCI_CONVERT_OLA_TRNSACTION_TO_PAK,       // 71 4.0.5.42								
				GCI_DELIVERY_START_AFTER_START ,         //72 4.0.6.503
				GCI_DELIVERY_END_AFTER_END ,             //73 4.0.6.503
				GCI_DELIVERY_START_IN_MIDDLE_OF_END ,    //74 4.0.6.503
				GCI_DELIVERY_END_IN_MIDDLE_OF_START  ,   //75 4.0.6.503
				GCI_RECORD_NOT_FOUND,					 //76 4.0.7.501
				GCI_PAK_RECONCILE_TRS,					 //77	
				GCI_WRONG_ATT,							// 78  //4.0.9.500
				GCI_SHIFT_START_WITH_PUMP_NOT_IDLE,		//79 //4.0.10.500
				GCI_CASH_ACCEPTOR_REPORT_NOT_ACTIVE ,   //80
				GCI_NOT_ALL_DRAWERS_REMOVED ,			//81
				GCI_INVALID_TERMINAL_NUMBER,            //82
				GCI_CASH_ACCEPTOR_REPORT_NOT_FOUND,     //83 
				GCI_SERVER_IN_MIDDLE_RECOVERY_PROCESS , //84 
				GCI_CREATE_OLA_SESSION,					//85 //4.0.13.501 
				GCI_SERVER_IN_THE_MIDDLE_OF_OLA_SESSION,//86
				GCI_PAK_WITH_LOYALTY,	//4.0.22.504 69344
				GCI_MAX_CONCURRENT_TRS_HAS_BEEN_REACHED,
				GCI_PUMP_ALREADY_RESERVED,
				GCI_PUMP_LOCKED,
				GCI_CANT_RESERVEPUMP,
				GCI_NAK = 99,							//  99 
				GCI_TERMINAL_SRV_IN_USE		= 150,	    //4.0.18.505 Merge from 4.0.140.10
				GCI_IN_MIDDLE_OF_QDX_MAINT	= 200,		//33898				
};


enum GCIEventRetCode {

					GCI_EVENT_OK,
					GCI_EVENT_NAK,	
					GCI_EVENT_NOT_SUPPORTED
};


// PAPSwipe Flags: 
#define SWIPE_LOG_COMMAND				0x00000000 // request to logger 
#define SWIPE_OLA_COMMAND				0x00000001 // request to authorizer

#define SWIPE_REQUEST					0x00000002 // Command - sData field contains: track 1 and track 2
#define SWIPE_EXTRA_INFO				0x00000004 // Command - sData field contains: more information from customer

#define SWIPE_GET_RESPONSE				0x00000008 // Command - ask for response from authorizer - NOT IN USE 
#define SWIPE_CANCEL					0x00000010 // Command - sData field contains: approval information 
#define SWIPE_APPROVAL_INFO				0x00000020 // Log - sData field contains: approval information 

#define SWIPE_RECEIPT_CHECK				0x00000040 // Command
#define SWIPE_RECEIPT_UPDATE			0x00000080 // Command
#define SWIPE_FORCE						0x00000100 // Command
#define SWIPE_VIT_CONFLICT				0x00000200 // 3.0.0.8
#define SWIPE_REPORT_REQUEST			0x00000400
#define SWIPE_LOYALTY_CARD				0x00000800		
#define SWIPE_CLOCK_CARD				0x00001000		
#define SWIPE_PAYMENT					0x00002000	
#define SWIPE_PAK_TO_PAP				0x00004000
#define SWIPE_SALE_COMMAND				0x00008000  //4.0.12.501
#define SWIPE_INCLUDE_PROMPTS			0x00010000	//  CR 433623



#define SWIPE_THREE_OLA_REC				0x00100000
#define SWIPE_INCLUDE_EXTRA_CHIP_INFO	0x20000000  //4.0.20.20

//4.0.13.501
#define ALL_CARDS_DATA					0x00200000 //4.0.9.506 //amram //to be removed 
#define GET_ALL_CARDS_DATA				0x00200000
#define SWIPE_GET_LOYALTY_SESSION		0x10000000  //4.0.11.503 //to be removed 
#define GET_CARD_INFO_SESSION			0x10000000
#define GET_CARD_INFO_ORIGINAL			0x20000000 



// PAPReadyTransaction Flags, PAPSwipeResult Flags
#define PAP_NO_DATA						0x00000000 // 
#define PAP_CARD_DATA					0x00000001 // swipe and authorization info
#define PAP_EXTRA_CARD_SALE				0x00000002 // sale completion extra info


// PakReadyTransaction flage Table#114
#define PAK_INCLUDE_REWARD_DATA					0x00000001 
#define PAK_INCLUDE_CONTROLLER_TRS_NUMBER		0x00000002 
//4.0.23.507 #define PAK_INCLUDE_EXTRA_INFO_REC				0x00000003
#define PAK_INCLUDE_IGNORE_VALUE_VOLUME_CHECK	0x00000004
#define PAK_INCLUDE_EXTRA_INFO_REC				0x00000008	//4.0.23.507
#define PAK_INCLUDE_EXTRA_INFO_REC_2			0x00000020	//4.0.21.500 - TD 14778   
#define PAK_NEED_TO_REPORT_PAID					0x00000040	//4.0.25.50 133966
#define PAK_CHECK_DUBLICATE_BY_TRS_NUMBER       0x00000080  //4.0.25.480 TD 277030
#define PAK_TO_MOBILE							0x00000100	




// GPI:: Get pap trs
#define GET_PAP_THREE_OLA_REC   				0x00000001 
#define GET_PAP_XML_RECORD		   				0x00000002 //4.0.9999.0
#define GET_PAP_AS_STRUCT						0x00000004	//4.0.19.505
#define GET_PAP_AS_EXTENDED_STRUCT				0x00000008	//4.0.20.650 71868 //4.0.22.511
#define GET_PAP_CONCATENAT_LOYALTY_SUMMARY   	0x00000010

// GCI:: Set pump mode
#define SET_PUMP_MANUAL_MODE			0x00000001 
 



// Grade Price change flags:
#define PRICE_A_CASH			0x0001
#define PRICE_B_CREDIT			0x0002


#define PRICE_SELF_SERVICE					0x0004
#define PRICE_FULL_SERVICE					0x0008
#define PRICE_NO_PRICE_CHANGE				0x0010
#define PRICE_CHANGE_WITH_ACK_PER_PUMP 		0x0020 //4.0.19.820 - TD 78775
#define SEND_PRICE_DIRECT_TO_CONVER_LAYER   0x0100 
#define PRICE_CHANGE_COMMIT_PUMPS_ONLY		0x0200	//4.0.26.508 147183

#define PRICE_END_OF_LIST					0x1000
#define MAX_UNIT_IS_BATCH_NUM				0x2000 //4.0.12.515
#define EXPECT_PRICE_CHANGE_RESULT			0x4000 //4.0.12.515 
#define PRICE_CHANGE_INTEGRITY				0x8000	//4.0.26.508 147183
////////price change flags is a SHORT field ///////////



// 4.0.5.42 
#define INIT_CONTROLLER_IS_MASTER									0x00000001
#define INIT_CONTROLLER_GEN_SEQ_NUMBER 								0x00000002 // PAP sequence number 
#define INIT_CONTROLLER_GEN_TRS_NUMBER 								0x00000004 // General Trs number 
#define INIT_CONTROLLER_MANAGE_PUMP_MODE							0x00000008 //
#define INIT_CONTROLLER_NO_SERVICES									0x00000010 // Controller logged for monitoring puposes, 
														   // Support no services.
#define INIT_CONTROLLER_MANAGE_PUMP_LOCK							0x00000020
#define INIT_CONTROLLER_SUPPORT_AVI									0x00000040 // 
#define INIT_CONTROLLER_FORCE_REMOVE_TRS							0x00000080 //4.0.5.33 
#define INIT_CONTROLLER_MANAGE_PUMP_MODE2							0x00000100 //4.0.5.33 
#define INIT_CONTROLLER_MANAGE_PRINTER_STATUS						0x00000200 //4.0.5.42 
#define INIT_CONTROLLER_SUPPORT_NOZZLE_RESTRICTION					0x00000400 //4.0.10.502
#define INIT_CONTROLLER_IN_MIDDLE_RECOVERY_PROCESS					0x00000800 //4.0.11.508
#define INIT_CONTROLLER_EXPECT_ALL_DETAILS_IN_NEED_MORE_PARAM		0x00001000 //4.0.13.506
#define INIT_CONTROLLER_USE_XML_BUFFER								0x00002000 //4.0.14.500
#define INIT_CONTROLLER_ALLOW_AUTH_ON_NON_REQUEST_PAP_RES_PUMP	    0x00004000 //4.0.21.500  TD 69683
#define	INIT_CONTROLLER_SUPPORT_STOP_ALL_PUMPS					    0x00008000 //4.0.22.500 - TD 69907
#define	INIT_CONTROLLER_ALLOW_AUTH_ON_STOP_BY_CONTROLLER			0x00010000 //4.0.22.XXX - TD 75479
#define	INIT_CONTROLLER_REPORTS_SIX_DIGITS_SEQ_NUMBER    			0x00020000 //4.0.23.503  - TD 78769 

/////////////////////////////////////////////////////////////////////////////
//
//	C O N T R O L    I N T E R F A C E
//
/////////////////////////////////////////////////////////////////////////////

// When control interface will be available - move these lines to control.h
#define CTRL_VERSION_1		1
#define CTRL_LATEST_VERSION	CONTROL_VERSION_1


enum WetStockInternalRetCode //4.0.7.501
{
	WetStockInternal_OK,
	WetStockInternal_IN_MIDDLE_OF_PROCESS,
};

enum InjectedPrompts
{
	DEFAULT_INJECTED_PROMPT,		// Regular NMP / ONMP
	COMMERCIAL_INJECTED_PROMPT,		// Pilot NMP
	CL_INJECTED_PROMPT,				// BP NMP
};

enum CtrlRetCode {

				CTRL_OK,								// 0
				CTRL_INVALID_PUMP_NUMBER,				// 1
				CTRL_INVALID_PUMP_MODE,					// 2
				CTRL_NOT_SUPPORTED,						// 3
				CTRL_INVALID_GRADE_NUMBER,				// 4
				CTRL_INVALID_DAYOFWEEK,					// 5
				CTRL_RECORD_NOT_FOUND,					// 6
				CTRL_FAILED_IO,							// 7
				CTRL_PUMP_NOT_IN_WORKING_STATE,			// 8
				CTRL_OPEN_PRE_PAY_TRS,					// 9  Cant close shift 
				CTRL_OPEN_PAY_AT_KIOSK_TRS,				// 10 Cant close shift 
				CTRL_OPEN_PAY_AT_PUMP_TRS,				// 11 Cant close shift 
				CTRL_IN_MIDDLE_OF_SHIFT,				// 12 Cant close shift 
				CTRL_PUMP_NOT_READY,					// 13 Cant close shift
				CTRL_TABLE_NAME_TOO_LONG,				// 14 
				CTRL_FIELD_NAME_TOO_LONG,				// 15
				CTRL_DEFAULT_FIELD_VAL_TOO_LONG,		// 16
				CTRL_FAILED_TO_OPEN_TABLE,				// 17 Cant Open Key, Param set to default 
				CTRL_PARAM_NOT_FOUND,					// 18 Not Found, Didn't create, accoording to request
				CTRL_PARAM_NOT_FOUND_AND_CREATED,		// 19 Not Found, Created and set to given default 
				CTRL_PARAM_NOT_FOUND_FAILED_TO_CREATE,	// 20 Cant create new param , Param set to default
				CTRL_INVALID_SCHEME_NUMBER,				// 21	
				CTRL_MODE_NAME_TOO_LONG,				// 22
				CTRL_LAST_ERROR,						// 23
				CTRL_INVALID_SERVICE,					// 24
				CTRL_SERVICE_IN_MIDDLE_OF_OPERATION,	// 25
				CTRL_SERVER_NOT_READY,					// 26
				CTRL_INVALID_LOCK_OPTION,				// 27
				CTRL_IN_MIDDLE_OF_PROCESS,				// 28
				CTRL_TRS_NOT_FOUND,						// 29
				CTRL_INVALID_SERVICE_FEE_NUMBER,		// 30	
				CTRL_INVALID_TANK_NUMBER,				// 31  //4.0.5.24
				CTRL_FORECOURT_EMERGENCY_LOCKED,        // 32  //4.0.5.34 

				CTRL_SERVICE_FEE_NOT_VALID = 49,		// 49				
				CTRL_SERVER_IN_MIDDLE_LOCK_SHIFT	=	59,	  //4.0.1.40

				CTRL_ACCUMULATE_SHIFT_TOTAL_NOT_ACTIVE = 66,//4.0.5.26
				CTRL_SHIFT_START_WITH_PUMP_NOT_IDLE,		//4.0.10.500

				CTRL_CONTROLLER_NOT_AVAILABLE = 90,		    // 90 Forecourt controller not available.
				CTRL_INVALID_CAR_WASH_NUMBER,			    //4.0.16.500
				CTRL_IN_MIDDLE_OF_QDX_MAINT = 200,		    //33898
				CTRL_SHIFT_WITH_OFFLINE_TRS  = 300,         //4.0.20.54 46943
				CTRL_ADD_DELIVERY_INVOICE_REF_WITHOUT_STRAT,//4.0.20.501 - TD 65174

				CTRL_IDLE,								//302	//4.0.21.500 - TD 66935
				CTRL_READING_PROCESS_STARTED,			//303	//4.0.21.500 - TD 66935
				CTRL_WAIT_FOR_CONNECTED_PUMPS_LOCK,		//304	//4.0.21.500 - TD 66935
				CTRL_CONNECTED_PUMPS_LOCKED,			//305	//4.0.21.500 - TD 66935
                CTRL_WAIT_TANK_READING,					//306	//4.0.21.500 - TD 66935
                CTRL_READING_PROCESS_TIMEOUT,			//307	//4.0.21.500 - TD 66935
				CTRL_READING_PROCESS_SUCCESS,			//308	//4.0.21.500 - TD 66935
				CTRL_WAIT_FOR_CONNECTED_PUMPS_UNLOCK,	//309	//4.0.21.500 - TD 66935
				CTRL_READING_PROCESS_FAILURE,			//310	//4.0.21.500 - TD 66935

				CTRL_IN_MIDDLE_CHANGE_PRODUCT_PROCESS,  //311    //4.0.22.503 - TD 72189

				CTRL_IN_MIDDLE_OF_DELIVERY_SIGLE_TANK,			//4.0.23.500 - TD 74964
				CTRL_DELIVERY_SIGLE_TANK_NOT_IN_PROCESS,		//4.0.23.500 - TD 74964

};

//add by yaron for Ctrl Events numbers
#define	CTRL_EVENT_SHIFT_STARTED		1
#define	CTRL_EVENT_SHIFT_ENDED			2
#define CTRL_EVENT_PUMPS_TOTALS			3
#define CTRL_EVENT_TANK_READING			4
#define CTRL_EVENT_DELIVERY_REPORT		5
#define CTRL_EVENT_PAP_TRS				6
#define CTRL_EVENT_MEMO_TANK_READING	7	//4.0.5.22
#define CTRL_EVENT_MEMO_DELIVERY_REPORT	8	//4.0.7.501
#define CTRL_EVENT_CASH_ACCEPTOR_REPORT_IS_READY  9
#define CTRL_EVENT_MEMO_PUMPTOTAL_READING   10 //4.0.12.507
#define CTRL_EVENT_GET_VERSIONS			11 //4.0.14.502
#define CTRL_EVENT_RECONFIGURE			12 //4.0.14.507
#define CTRL_EVENT_EXPORT_SITE_CONFIG   13 //4.0.26.508 TD 146575
#define CTRL_PRICE_CHANGE_PUMPS_SUMMARY    14 //4.0.31.??? TD 405462



// Set Cash Acceptor Status flags

#define SET_CASH_ACCEPTOR_REMOVAL_DRAWER    1	
#define SET_CASH_ACCEPTOR_REPORT_READY  	99

//Get cash acceptor report

#define GET_CASH_ACCEPTOR_REPORT_HISTORY  1



//4.0.1.32 add by yaron for GCIEvent::GeneralSystemEvent(...) 
#define	GCI_EVENT_SHIFT_STARTED					1
#define	GCI_EVENT_RELOAD_PRICE_POLES_DATA		2
#define	GCI_EVENT_SHIFT_END						3
#define GCI_EVENT_START_TERMINAL_MESSAGE        4   //4.0.1.40
#define GCI_EVENT_STOP_TERMINAL_MESSAGE         5
#define	GCI_EVENT_GET_VERSION					6   //4.0.2.251
#define	GCI_EVENT_UPDATE_CONTROLLER_SOFTWARE	7   //4.0.3.12
#define GCI_EVENT_CLOSE_RECONCILIATION_SHIFT    8   // 4.0.3.60
#define GCI_EVENT_DELIVERY_START_ENDED          9   // 4.0.5.27
#define GCI_EVENT_DELIVERY_END_ENDED            10  // 4.0.5.27
#define GCI_EVENT_PRINTER_ONLINE          		11  // 4.0.5.42 
#define GCI_EVENT_PRINTER_OFFLINE               12 
#define GCI_EVENT_MEMO_TANK_READING             13  // 4.0.7.501
#define GCI_EVENT_MEMO_DELIVERY_REPORT			14
#define GCI_EVENT_SERVER_IDLE					15	// 4.0.9.511	
#define GCI_EVENT_TERMINAL_SW_RESET				16  // 4.0.10.1000	
#define GCI_EVENT_TERMINAL_SW_DOWNLOAD			17  // 4.0.13.503 
#define GCI_EVENT_OLA_ONLINE					18	// TD 42728
#define GCI_EVENT_OLA_OFFLINE					19	// TD 42728
#define GCI_EVENT_TERMINAL_ONLINE				20  // CR 47599
#define GCI_EVENT_TERMINAL_OFFLINE				21	// CR 47599
#define GCI_EVENT_PAY_INSIDE_PRESSED			22
#define GCI_EVENT_OLA_CLOSE_BATCH				23	//4.0.19.502 - CR 58096
#define GCI_EVENT_FORCE_RELEASE_TRS				24  //4.0.20.0
#define GCI_EVENT_DISABLE_REPRINT               25  //4.0.20.500
#define GCI_EVENT_CHANGE_PRODUCT_IN_TANK		26	//4.0.22.503 - TD 72189
#define GCI_EVENT_ANPR_UNBLOCK_PUMP				27	//4.0.23.500 - TD 72717 
#define GCI_EVENT_ANPR_OVERRIDE_PUMP_BLOCK		28	//4.0.23.500 - TD 76438
#define GCI_EVENT_CODAX_REQUEST_TIMEOUT			29	//4.0.23.504 76432
#define GCI_EVENT_AUTO_RECONCILE				30	// CR 233242 4.0.23.1050
#define GCI_EVENT_TERMINAL_HW_RESET			    31  //4.0.31.?? 
#define GCI_EVENT_TERMINAL_VERSION				32  // CR 445470 (Suncor)
#define GCI_EVENT_SEND_DPT_CONFIG_FILES			33  // EMV

//GetOlaMemInfo Flags
#define OLA_MEM_ALL					0
#define OLA_MEM_OPEN_AVI			1
#define OLA_MEM_TIME_OUT_AVI		2
#define OLA_MEM_CHARGE_AVI_TRS		3
#define OLA_MEM_AMOUNT				4
#define OLA_MEM_CREDIT              5
#define OLA_MEM_DEBIT               6
#define OLA_PAK_TO_PAP_DECLINE      7

#define OLA_MEM_FLAGS2              0x00000001
			
// QueryPAP		flag

#define OLA_MEM_WITHOUT_CANCEL		0x100000


//GetOlaMemInfo Record Status   //Table 103

#define OLA_MEM_RECORD_NOT_USE		0
#define OLA_MEM_RECORD_USE			1

// Service fee flags
#define SERVICE_FEE_VALID				0x00000001

// GetDeliveryReport, GetTankReading, GetPumpTotals , QueryPAPTrs, QueryPrePaidTrs
#define GET_DIRECT_INDEX	1 
#define GET_FIRST_INDEX		2 
#define GET_NEXT_INDEX		3 


//QueryPrePaidTrs flags
#define GPPT_OFFER_TRS_TO_POS   0x200

#define NO_POS_OWNER        0

// 4.0.5.39
#define PREPAY_STATE_ON_OFFER  1
#define PREPAY_STATE_FORCE_RELEASE  2


//4.0.5.26
//QueryExtraInfoData flags 
#define QEI_GET_DIRECT_INDEX			1   
#define QEI_GET_FIRST_INDEX				2 
#define QEI_GET_NEXT_INDEX				3 

#define QEI_STRUCT_IS_LOYALTY_INFO2		0x0100
#define QEI_FLEET_PAK					0x0200		//4.0.9.509
#define QEI_STRUCT_IS_LOYALTY_INFO3		0x0400		//Liat LPE
#define QEI_STRUCT_IS_LOYALTY_INFO4		0x0800		//4.0.19.710 66705
#define QEI_STRUCT_IS_ANPR_INFO_PLUS	0x1000		//4.0.23.500 TD 69914
#define QEI_STRUCT_IS_PUMP_TEST_INFO	0x2000		//4.0.25.1140 TD 412765
#define QEI_STRUCT_IS_LOYALTY_INFO4_XML	0x4000		//RFUEL-520

#define GET_INDEX_MASK				  0x000f 

//SetTransactionState flags    //4.0.5.44
#define ST_STATUS_WITH_ENLARGE_REC				0x0001   
#define ST_STATUS_INCLUDE_LAM					0x0002   
#define ST_STATUS_USING_XML						0x0004	//4.0.12.517   
#define ST_STATUS_INCLUDE_DISPENSING_VOLUMES	0x0008	//4.0.25.30 - TD 69655
#define ST_STATUS_INCLUDE_ATTENDANT_DATA		0x0010	//4.0.25.501 TD 137509

//Lam  flags. 
#define LAMF_DATA_VALID					'1'
#define LAMF_DATA_NOT_VALID				'2'

// QueryPrePaidTrs
#define GET_PRE_PAY_TAG2			0x0100  //4.0.3.61

#define QUERY_CLOSE_TRS				0x1000
#define QUERY_CLOSE_TIMEOUT			0x2000
#define QUERY_MASK2			        0x4000   //4.0.10.504
#define QUERY_MASK			        0xf000 
#define QUERY_FLAGS2		        0x00000001


// ReomoveDeliveryReport, ReomoveTankReading, ReomovePumpTotals 
#define REMOVE_DIRECT_INDEX			1
#define REMOVE_FIRST_TILL_INDEX		2

// PumpTotals Flags
#define TOTALS_NOT_AVAILABLE			0x0001
#define TOTALS_VALUEA_ONLY				0x0002// combined cash and credit totals
#define TOTALS_VALUEA_CASH				0x0004// 
#define TOTALS_VALUEB_CREDIT			0x0008//
#define TOTALS_ON_DEMAND_REQUEST		0x0010//
#define TOTALS_END_OF_SHIFT				0x0020// indication for end of shift process //4.0.23.500 - TD 77765
//CR_475695_start
#define	TOTALS_READ_ON_SCHEDULE_CHANGE_REQUEST	0x0040
#define TOTALS_READ_ON_MANUAL_REQUEST			0x0080
#define TOTALS_ON_EOD_REQUEST					0x0100	
#define TOTALS_ON_PRICE_CHANGE_REQUEST			0x0200
#define TOTALS_READ_ON_EOS_REQUEST				0x0400
//CR_475695_End
#define TOTALS_END_OF_LIST				0x1000// by pump 

// TankReading Flags
#define TANK_READ_NOT_AVAILABLE			0x0001
#define TANK_READ_FUEL_LEVEL			0x0002
#define TANK_READ_WATER_LEVEL			0x0004
#define TANK_READ_TEMPERATURE			0x0008
#define TANK_READ_FUEL_VOLUME			0x0010
#define TANK_READ_WATER_VOLUME			0x0020
#define TANK_READ_DELIVERY_IN_PROGRESS	0x0040
#define TANK_READ_ON_DEMAND_REQUEST		0x0080//
//CR_475695_Start
#define TANK_READ_ON_MANUAL_REQUEST				0x0100
#define TANK_READ_ON_EOS_REQUEST				0x0200
#define TANK_READ_ON_EOD_REQUEST				0x0400
#define TANK_READ_ON_PRICE_CHANGE_REQUEST		0x0800
#define	TANK_READ_ON_SCHEDULE_CHANGE_REQUEST	0x2000
//CR_475695_End
#define TANK_READ_END_OF_LIST			0x1000 // all tanks since the original request call with 0 - all tanks

#define TANK_READ_NOT_AVAILABLE_FOR_ALL	2
//CR_475695_Start
// Reading type
enum{
	READ_ON_OTHER_REQUEST		=	1,
	READ_ON_EOD_REQUEST		    =   2,
	READ_ON_EOS_REQUEST			=	3,
	READ_ON_PRICE_CHANGE_REQUEST=	4,
	READ_ON_SCHEDULED_REQUEST	=	5,
	READ_ON_MANUAL_REQUEST		=	6,
};
//CR_475695_End

// TankReading Status  
#define TANK_STATUS_IDLE				0  // The read is done as part of shift change
#define TANK_STATUS_BUSY				1  // Station busy - Snap Shot 
#define TANK_STATUS_ERROR				2  // Station busy - Snap Shot 
#define TANK_STATUS_GAUGE_NOT_PRESENT	3  // Tank Gauge not present
#define TANK_STATUS_PART_HOUR			4  // 


// DeliveryReport Flags
#define DELIVERY_REPORT_NOT_AVAILABLE			0x0001
#define DELIVERY_REPORT_MANUAL					0x0002
#define DELIVERY_REPORT_AUTOMATED				0x0004
#define DELIVERY_REPORT_ON_DEMAND_REQUEST		0x0008//
#define DELIVERY_REPORT_ADJUST_TC_VOL_IN_TEMP	0x0010	//4.0.21.511 - TD 72606
#define DELIVERY_FORCE_ACCEPT_DATA				0x0100  //4.0.7.505
#define DELIVERY_REPORT_END_OF_LIST				0x1000
#define DELIVERY_REPORT_INVOICE_DATA			0x2000


// Alarm Flags
#define ALARM_REPORT_NOT_AVAILABLE		0x0001
#define ALARM_REPORT_MANUAL				0x0002
#define ALARM_REPORT_AUTOMATED			0x0004
#define ALARM_REPORT_ON_DEMAND_REQUEST	0x0008
#define ALARM_REPORT_END_OF_LIST		0x1000


// Set Shift Flags   Table #001
#define SET_SHIFT						0x0000   // not used 
#define REMOVE_SHIFT					0x0001   // remmove shift validation (shift not valid)
#define DO_PUMP_TOTALS					0x0002   // shift with  pump total 
#define DO_TANK_READING					0x0004   // shift with tank reading 
#define SET_SHIFT_TO_CURRENT_TIME		0x0008   // start manually shift, by the cashier.     
#define FORCE_NON_LOCK					0x0010   // start shift without lock the site.
#define DO_FORCE_LOCK_PUMP				0x0020   // lock forecourt after end of shift..
#define FORCE_SHIFT_OLA_NOT_EMPTY		0x0040	 // force end of shift althoght there are record in the OLA qdex.
#define DO_RECONCILIATION				0x0080   // reconcile TankSrv and VeederRoot
#define FORCE_LOCK_FORECOURT_IGNORE_CURRENT_SETTING	0x0100   // lock site through interface ignore  his current setting in registry   //4.0.5.381
#define DO_MAINTENANCE_LOCK_FORECOURT	0x0200   // lock site through interface ignore  his current setting in registry   //4.0.5.381
#define DO_DATABASE_MAINTENANCE		    0x0400	 // create process of database maintenance. 
#define DO_GET_DEVICES_VERSIONS			0x0800   // Get devices versions info //4.0.14.501
#define DO_OLA_CLOSE_BATCH				0x1000	 // Ola close batch - 4.0.19.502 CR 58096
#define SET_SHIFT_REPORT_OFFLINE_TRS_IF_EXIST       0x2000  //If got this flag, Pumpsrv will return a special return code if offline transaction exist on shift //4.0.20.54 46943
#define SET_SHIFT_EXPORT_SITE_CONFIG				0x4000	//4.0.26.508 TD 146575
#define SET_SHIFT_WITH_EOD              			0x8000    //CR 475695 
//Get Station Status flags - 4.0.22.504 TD 72710
#define STATION_STATUS_NOT_IDLE_PUMPS	0x00000000
#define STATION_STATUS_OFFLINE_PUMPS 	0x00000001
#define STATION_STATUS_UNPAID_TRS_ON_PUMPS	0x00000002 //4.0.29.501 284709
		
//3.2.0.69
//#define DO_DEBIT_SHIFT					0x0040 // Satrt debit shift
//3.2.0.69
// Control Status flags
#define CONTROL_IGNORE_IN_VALUES		0x0001
#define CONTROL_HANDLE_TOTAL			0x0002 
#define CONTROL_HANDLE_TANK_READ		0x0004
#define CONTROL_HANDLE_DELIVERY			0x0008

// Process Number
#define PROCESS_SHIFT_CHANGE			1
#define PROCESS_PUMP_TOTALS				2
#define PROCESS_TANK_READING			3
#define PROCESS_PRICE_CHANGE			4
#define PROCESS_MAINTENANCE				5
#define PROCESS_DELIVERY_EMULATION		6		//4.0.15.501
#define PROCESS_GET_SHIFT_CTRL  		7		//4.0.16.500

// Mode definition flags
#define MODE_DEF_PAK_VALID				0x00000001
#define MODE_DEF_PAK_AUTO_AUTH			0x00000002
#define MODE_DEF_PAP_VALID				0x00000004
#define MODE_DEF_PAP_AUTO_AUTH			0x00000008
#define MODE_DEF_PRE_PAY_VALID			0x00000010
#define MODE_DEF_PRE_PAY_AUTO_AUTH		0x00000020
#define MODE_DEF_PAY_ATTEND_VALID		0x00000040
#define MODE_DEF_PAY_ATTEND_AUTO_AUTH	0x00000080
#define MODE_DEF_PAY_AVI_VALID			0x00000100
#define MODE_DEF_PAY_AVI_AUTO_AUTH		0x00000200
#define MODE_DEF_CASH_ACCEPTOR_VALID	0x00000400  //4.0.3.60
#define MODE_DEF_CASH_ACCEPTOR_AUTH		0x00000800  //4.0.3.60
#define MODE_DEF_CASH_ACCEPTOR_AUTH		0x00000800  //4.0.3.60
#define MODE_DEF_PAK_AUTO_RECONCILE		0x00001000  //4.0.7.504 
#define MODE_DEF_MANUAL_CHANGE			0x00100000 //4.0.9.509
#define MODE_DEF_UNMANNED_MODE			0x00200000 //4.0.14.503

#define MODE_DEF_AUTO_AUTH_LOYALTY_PAK	0x00400000 //4.0.23.508 89149
#define MODE_DEF_PAP_POSTPAY_VALID		0x00800000 // Adi L - CR 264032

#define MODE_DEF_PAM_VALID				0x01000000
#define MODE_DEF_AUTO_AUTH_PAM			0x02000000

#define MODE_DEF_VALID					0x10000000
#define MODE_DEF_END_OF_LIST			0x20000000	//4.0.20.504 - TD 65134

// Mode scheme flags
#define MODE_SCHEME_DAILY				0x00000001
#define MODE_SCHEME_WEEKLY				0x00000002
#define MODE_SCHEME_YEARLY				0x00000004

#define MODE_SCHEME_FULL_SERVICE_VALID	0x01000000  // 4.0.2.21
#define MODE_SCHEME_SELF_SERVICE_VALID  0x02000000  // 4.0.2.21


#define MODE_SCHEME_VALID				0x10000000
#define MODE_SCHEME_NOT_VALID			0xefffffff

// Mode scheme service mode
#define MODE_SHCEME_FULL_SERVICE		1
#define MODE_SHCEME_SELF_SERVICE	    2

// 4.0.2.21


//Eyal 3.2.0.3 Casey's
#define TLS_INPUT_IS_BSTR               0x00000001
#define TLS_OUTPUT_IS_TEXT              0x00000002

#define TLS_CMD_TANK_LEAK_TEST_RESULTS             0x00000001  //208 
#define TLS_CMD_TANK_DELIVERY_REPORT			   0x00000002  //202
#define TLS_CMD_TANK_INVENTORY_REPORT			   0x00000004  //201
#define TLS_CMD_GET_TIME_OF_DAY					   0x00000008  //501
#define TLS_CMD_TANK_PRODUCT_CODE				   0x00000010  //603
#define TLS_CMD_TANK_PRODUCT_LABEL                 0x00000020  //602
#define TLS_CMD_TANK_LINEAR_CALCULATED_FULL_VOLUME 0x00000040  //60A
#define TLS_CMD_NON_PRIORITY_ALARM                 0x00000080  //112
#define TLS_CMD_PRIORITY_ALARM_HISTORY             0x00000100  //111
#define TLS_CMD_HEADER_LINE                        0x00000200  //503
#define TLS_CMD_PC_DIAGNOSTIC_DATA                 0x00000400  //902     //Eyal 1.0.0.2

#define MAX_TLS_CMD_LEN                            128

//Reset PumpStat qdx file flags
#define	RESET_SERVER_INFO  				0x00000001
#define	RESET_SHIFT_INFO  				0x00000002
#define	RESET_PUMP_INFO  				0x00000004
#define RESET_WITHOUT_REPORT_TO_CL		0x00000008		//4.0.23.508 TD 89049

#define RESET_ALL_PUMPS			0	//add by Yaron

#define MAX_PANELS_PER_DEVICE			16
#define MAX_PRICE_DISPLAYS				8

#define MAX_BANK_NOTE_LEVEL				16  //4.0.3.60

// Max tax entries
#define MAX_TAXES						32
#define MAX_TAXES_ALLOWED				4

//4.0.2.24 Start
//Registry table IDs
#define CARWASH_TABLE               0x00000001
#define DATABASE_TABLE              0x00000002
#define GRADES_TABLE                0x00000004
#define IPT_TABLE                   0x00000008
#define MESSAGES_TABLE              0x00000010
#define MODES_TABLE                 0x00000020
#define PATHS_TABLE                 0x00000040
#define PRICE_POLES_TABLE           0x00000080
#define PUMPS_TABLE                 0x00000100
#define PURE_PRODUCTS_TABLE         0x00000200
#define RECEIPT_TABLE               0x00000400
#define RFS_TABLE                   0x00000800
#define SCREEN_LAYOUT_TABLE         0x00001000
#define SERVICE_FEE_TABLE           0x00002000
#define SHIFTS_TABLE                0x00004000
#define TANKS_TABLE                 0x00008000
#define TAXES_TABLE                 0x00010000
#define TERMINALS_TABLE             0x00020000
#define WASH_PROGRAMS_TABLE         0x00040000
#define ALLIED_TABLE                0x00080000
#define GENERAL_PARAM_TABLE         0x00100000
#define MEGGITT_TABLE               0x00200000
#define DOMS_TABLE                  0x00400000  //4.0.3.60
#define LANGUAGES_TABLE				0x00800000  //4.0.5.42
#define POSTEC_TABLE				0x01000000  //4.0.21.500 TD 69287
#define	FLOW_CONTROL_TABLE			0x02000000	//4.0.22.504 TD 69344
#define ANPR_TABLE					0x04000000	//4.0.26.505 157073
#define ENCRYPTION_SRV_TABLE		0x08000000
#define TANK_SRV_TABLE				0x10000000	// CR 414103
#define	LOYALTY_TABLE				0x20000000	// 4.0.23.2812

//Sub table IDs
#define ALL_TABLES                                       0xffffffff

// Special Flags for Reconfigure Purpose
#define FLAG_RECONFIGURE_IN_EOS				               0x00000001

#define GENERAL_PARAM_TABLE_OLA                          0x00000001
#define GENERAL_PARAM_TABLE_FORECOURT_LIMITS             0x00000002
#define GENERAL_PARAM_TABLE_ACTIVES                      0x00000004
#define GENERAL_PARAM_TABLE_WETSTOCK                     0x00000008
#define GENERAL_PARAM_TABLE_LOGS						 0x00000010
#define GENERAL_PARAM_TABLE_TIMERS                       0x00000020
#define GENERAL_PARAM_TABLE_AVI_TRS                      0x00000040
#define GENERAL_PARAM_TABLE_GENERALS                     0x00000080
#define GENERAL_PARAM_TABLE_LOCK_CONFIGURATION			 0x00000100
#define GENERAL_PARAM_TABLE_ENCODING					 0x00000200			// 4.0.22.501	TD 72716
#define GENERAL_PARAM_TABLE_REAL_TIME					 0x00000400			// CR 438993

#define DATABASE_TABLE_ALARM                             0x00000001
#define DATABASE_TABLE_CAR_WASH                          0x00000002
#define DATABASE_TABLE_DELIVERY                          0x00000004
#define DATABASE_TABLE_EXTRA_INFO                        0x00000008
#define DATABASE_TABLE_ITEMS                             0x00000010
#define DATABASE_TABLE_OLA                               0x00000020
#define DATABASE_TABLE_PREPAY                            0x00000040
#define DATABASE_TABLE_PUMPSTAT                          0x00000080
#define DATABASE_TABLE_PUMPTOTALS                        0x00000100
#define DATABASE_TABLE_RECEIPT                           0x00000200
#define DATABASE_TABLE_RFS                               0x00000400
#define DATABASE_TABLE_SHIFT_DELIVERY_TOTALS             0x00000800
#define DATABASE_TABLE_SHIFT_PUMP_TOTALS                 0x00001000
#define DATABASE_TABLE_SHIFT_TANK_TOTALS                 0x00002000
#define DATABASE_TABLE_TANK_READ                         0x00004000
#define	DATABASE_TABLE_ASH2000							 0x00008000   //4.0.7.501
#define	DATABASE_TABLE_DB_TANK_GAUGE					 0x00010000	
#define	DATABASE_TABLE_ACCUMULATION_SHIFT_TOTAL			 0x00020000	
#define	DATABASE_TABLE_RECEIPT_EXTRA					 0x00040000
#define	DATABASE_TABLE_RECEIPT_EXTRA_INDEX				 0x00080000

#define PATHS_TABLE_LONG                                 0x00000001
#define PATHS_TABLE_SHORT                                0x00000002

#define RFS_TABLE_AVI                                    0x00000001
#define RFS_TABLE_KEYBOARDS                              0x00000002
#define RFS_TABLE_LANGUAGES                              0x00000004
#define RFS_TABLE_GENERAL								 0x00000008

#define ALLIED_TABLE_GENERAL_PARAM                       0x00000001	//4.0.20.506 66221
#define ALLIED_TABLE_KEYBOARDS                           0x00000002 //4.0.20.506 66221

//4.0.3.60
#define DOMS_TABLE_GENERAL_PARAM                         0x00000001
#define DOMS_TABLE_DECIMAL_POINT                         0x00000002
#define DOMS_TABLE_FUELLING_MODE_GROUPS                  0x00000004
#define DOMS_TABLE_LANGUAGES							 0x00000008
#define DOMS_TABLE_FUELLING_MODES						 0x00000010
#define DOMS_TABLE_IPT_EXTRA_INFOS						 0x00000020
#define DOMS_TABLE_SERVICE_MODES						 0x00000040
#define DOMS_TABLE_TRANSACTION_TYPES					 0x00000080
//4.0.3.60


//Flags
#define SUB_TABLE_IS_DEVICE_MAP                          0x00000001


//GeneralSystemRequest flags Table #150
#define GSR_SOFTWARE_REQUEST_VERSION					 0x00000001
#define GSR_RESET_INI									 0x00000002
#define GSR_START_DELIVERY								 0x00000004		//4.0.13.504
#define GSR_END_DELIVERY								 0x00000008		//4.0.13.504
#define GSR_ADD_DELIVERY_INVOICE_REF_NUMBER				 0x00000010		//4.0.20.501 - TD 65174
#define GSR_START_DELIVERY_SINGLE_TANK					 0x00000020		//4.0.21.500 - TD 66935
#define GSR_END_DELIVERY_SINGLE_TANK					 0x00000040		//4.0.21.500 - TD 66935
#define GSR_ABORT_DELIVERY_SINGLE_TANK					 0x00000080		//4.0.21.500 - TD 66935
#define GSR_CHANGE_PRODUCT_IN_TANK						 0x00000100		//4.0.22.503 - TD 72189
#define GSR_CHANGE_PRODUCT_IN_TANK_SETUP_ONLY			 0x00000200		//4.0.22.503 - TD 72189
#define GSR_CANCEL_DELIVERY_SINGLE_TANK					 0x00000400		//4.0.23.500 - TD 74964
#define GSR_SEND_DPT_CONFIG_FILES						 0x00000800

// GetPAKQuery return code

enum GetPAKQueryRetCode
{
	GetPakQuery_OK =0,    
	GetPakQuery_MissingData =1, 
};

//4.0.3.37 PAK query flags table#113
#define PAK_QUERY_UNPAID_PAK							0x000001
#define PAK_QUERY_UNBALANCE_PREPAY						0x000002
#define PAK_QUERY_WAIT_PREPAY							0x000004
#define PAK_QUERY_FOR_ATTENDANT							0x000008	//4.0.5.42
#define PAID_TRS_FOR_PUMP								0x000010	//4.0.11.503
#define PAID_TRS_ONE_TRS								0x000020	//4.0.11.503
#define PAK_QUERY_FOR_ATTENDANT_BY_ID					0x000040	//4.0.11.503
#define PAK_QUERY_AUTH_PREPAY							0x000080	//4.0.16.501 - TD 38806
#define PAK_QUERY_UNPAID_PAK_BY_PUMP					0x000100	//4.0.20.501 - TD 65908
#define PAK_QUERY_UNPAID_ON_OFFER_PAK					0x000200	//4.0.22.503 69173 
#define PAK_QUERY_UNPAID_PAK_BY_PUMP_NOZZLE				0x000400	//4.0.22.507 - TD 72675
#define PAK_QUERY_IGNORE_EXTENDED_GRADE_ID_CONVERSION	0x000800	//4.0.24.90 - TD 116227
#define GET_STATISTIC_DATA								0x001000	// 4.0.15290.0 - CR 433623
#define PAK_QUERY_IN_PROGRESS_PREPAY					0x002000	// emerald

// GetReceiptTransactionDetails from TerminalSrv
#define GET_RECEIPT_TRS_BY_PUMP         0x000001
#define GET_RECEIPT_TRS_BY_TRACK2		0x000002

//4.0.5.0
// QueryReceipt		flage
#define QUERY_RECEIPT_THREE_OLA_REC	 0x000100
#define QUERY_RECEIPT_TRACK2_PCI	 0x000200	


// QueryPAP		flage
#define QUERY_PAP_THREE_OLA_REC	 0x000001

//4.0.6.501
//GetGeneralData record types
#define PS_REC_TYPE_STARTUP  0x00000001
#define PS_TYPE_SHIFT_SALES  0x00000002


//4.0.12.507
//GetPumpTotalInfo flags
#define MEMO_PUMP_TOTAL 0x000001

//4.0.9.500
//GetAttTrs	flags
#define INDEX_IS_ATTID	0x0001
#define INDEX_IS_TRACK2	0x0002



#define MAX_OLA_BUFFER_LEN	5000

#define TERMINAL_ONLINE		1
#define TERMINAL_OFFLINE	0

//4.0.19.504 - 47559
//For GPIEvents::GeneralSystemEvent(...) 
#define	GPI_EVENT_SHIFT_STARTED					1
#define GPI_EVENT_CARWASH_ONLINE				2
#define GPI_EVENT_CARWASH_OFFLINE				3
#define GPI_EVENT_LOAD_PUMP_GRADES				4	// TD 415542
#define GPI_EVENT_SERVER_IDLE					5	// TD 433623
#define GPI_EVENT_PAP_READY_CONFIRM				6	// TD 474446


//4.0.21.500
#define CANCEL_WASH_CODE_REFUND					0x000001 
#define CANCEL_WASH_CODE_LINE_VOID				0x000002

//4.0.23.504 76432 start
#define CAR_WASH_SYNC_FUNC  					0x000001
#define CAR_WASH_SETUP_FUNC						0x000002
#define CAR_WASH_COLD_START_FUNC				0x000004
#define CAR_WASH_REFUND_FUNC					0X000008	//TD 144221
#define CAR_WASH_USE_EXT_TAG					0x000010
	

enum CarWashOperation
{
	CW_GenerateWashCode,	//0
	CW_GetWashCode,			//1
		
};



// Ctrl::GetProcessStatusEx - process type
#define PROCESS_CHANGE_PRICE_WITH_ACK_PER_PUMP			1

//4.0.24.90 1022 HOT fix
//For CGPI::RemovePAPTrs(...)

#define GPI_REMOVE_PAP_TRS_REMOVE_FIRST			0xfffffffe

/*			FOR FUTURE USE!!!! 
#define GPI_REMOVE_PAP_XXXXXXXXXXXXXXXX			0xfffffffd
#define GPI_REMOVE_PAP_XXXXXXXXXXXXXXXX			0xfffffffb
#define GPI_REMOVE_PAP_XXXXXXXXXXXXXXXX			0xfffffff7
#define GPI_REMOVE_PAP_XXXXXXXXXXXXXXXX			0xffffffef
#define GPI_REMOVE_PAP_XXXXXXXXXXXXXXXX			0xffffffdf
#define GPI_REMOVE_PAP_XXXXXXXXXXXXXXXX			0xffffffbf
#define GPI_REMOVE_PAP_XXXXXXXXXXXXXXXX			0xffffff7f
*/


// Ctrl::SetParam4 flags
#define SET_PARAM_USE_EXTENDED_GRADE_ID					0x00001		//4.0.24.90 - TD 116227

// GPI::GetParam4 flags
#define GET_PARAM_USE_EXTENDED_GRADE_ID					0x00001		//4.0.26.501 - TD 116227

// Flag to ignore extended grade conversion in CGPI::GetPAKTrs and GPI::GetGradePrices
#define IGNORE_EXTENDED_GRADE_ID_CONVERSION				0x00001		//4.0.24.90 - TD 116227

//Flags for CBaseReceipt::ConvertOlaToReceipt
#define	DO_NOT_RESET_RECEIPT_BUFFER						0x00001		//4.0.26.0 - TD 69677
#define TRS_NUMBER_IN_RECEIPT_INDEX						0x00002

//Flags for FinalizePrePayTrs
#define FINALIZE_PREPAY_CREATE_RECEIPT_REC				0x000002		//4.0.28.500 - TD 240006/251326


#endif //__PUMP_H__