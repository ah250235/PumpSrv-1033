/********************************************************************
	created:	2004/05/05
	created:	5:5:2004   17:51
	file base:	Commondef
	file ext:	h
	author:		shayt
	
	purpose:	PumpSrv shared def
*********************************************************************/

#ifndef __COMMON_DEF_H_
#define __COMMON_DEF_H_
#define MAX_WASH_PROGRAMAS		64  // Jira 170
#define OLASRV_ROUTESRV			3

#define	CD_TYPE_AVI				'A'
#define	CD_TYPE_CASH			'B'
#define	CD_TYPE_CREDIT			'C'
#define	CD_TYPE_DEBIT			'D'
#define	CD_TYPE_ATTENDANT		'V'
#define	CD_TYPE_LOYALTY			'M'
#define	CD_TYPE_CLOCK			'T'
#define	CD_TYPE_PAY_AT_KIOSK	'K'
#define	CD_TYPE_PRE_PAY			'P'

// general define
#define FULL_VEHICLE_ID			16


#define  PIN_BLOCK_SIZE			16		//4.0.17.20
#define	ENCRYPTED_DATA_SIZE		100		//4.0.20.501 TD 60236

#define ACCOUNT_NUM_SIZE		21 //4.0.14.500

#define PCI_COMPLIENT_DIGIT_NUM_TO_MASK 4
#define PCI_COMPLIENT_MASK_CHAR			'X'
#define PCI_COMPLIENT_MASK_LOCATION		6
#define PCI_COMPLIENT_MASK_NUM_START	6
#define PCI_COMPLIENT_MASK_NUM_END		4


#define NO_POS_NUMBER						-1L
#define SERVER_POS_NUMBER					0L  // for pumpsrv internal usage of POS functions (act like POS)
#define SERVER_OLA_POS_NUMBER				99L // for pumpsrv external usage representing all pumps.
#define TERMINAL_SRV_POS_NUMBER	801L		// for IfsfTerminalSrv usage.
#define MOBILE_SRV_POS_NUMBER	802L		// for FuelMobileSrv usage.
#define NO_PUMP_NUMBER						-1L
#define NO_NOZZLE_NUMBER					(short int)-1
#define NO_GRADE_CODE						(short int)-1
#define MAX_STRING_LEN						20
#define MAX_UNPAID_TRS						4 //  4 in memory , on change remember to update  PumpFile size
#define MAX_POST_VOID_TRS					2
#define MAX_WAITING_TRS						1 //  1 stacked,  on change remember to update PumpFile size
#define MAX_TRS_IN_LIST						MAX_WAITING_TRS + MAX_UNPAID_TRS + 1 // 1 in progress
#define MAX_MESSAGES_TO_DISPLAY				3			
#define BASE_TERMINAL_NUMBER				900 //4.0.3.13    //3.2.0.76	
#define MAX_ATTENDANT_TRS					1 // 4.0.22.501- 4.0.22.12 default - will always be out of MAX_UNPAID_TRS	 TD 73062			
#define PUMP_STOPPER_FLOW_HALTED_TIMEOUT	11 //TD 220310: Marker for pump stopper is flow halted timeout

#define HOME_DIR_STRING_LENGTH	256

#define MAX_WAIT_AUTH_TIMEOUT	30  // sec 


#define MAX_TERMINAL_MESSAGE_LENGTH 256//amram
#define MAX_POSITIONS_PER_PUMP	8//4.0.10.502
#define	SHORT_CARD_NAME_LEN  8 //4.0.10.1000


#define MAX_NAME_LENGTH			20
#define MAX_DATE_LENGTH			11			//TD 72716
#define MAX_TIME_LENGTH			9			//TD 72716

#define STR_LONG_VAL2			2
#define STR_LONG_CODE			3
#define STR_LONG_VAL5			5
#define STR_LONG_VAL6			6
#define STR_LONG_VAL7			7
#define STR_LONG_VAL10			10
#define STR_15					15
#define STR_16					16
#define STR_20					20 
#define MAX_FORMATED_POS_LINE   50 

#define MAX_PUMPS_64			64 //Eyal 4.0.0.79 & 4.0.1.31
#define MAX_PUMPS               32

#define MAX_GRADES				16
#define	MAX_GRADES_99			99 //amram 4.0.10.501


#define MAX_TERMINALS			64  //Eyal 4.0.0.79
#define MAX_IPT					16    //3.2.0.65

#define MAX_TANKS				32  //Eyal 3.2.0.1
#define OLD_MAX_TANKS			16	//Eyal 3.2.0.1
//#define	MAX_FIELD_VAL			256

#define ENLARGE_XML_BUF 40000
#define MIN_DELIVERY_XML_LEN    150  //4.0.21.500 TD 66935
#define IFSF_POS				801 //?.??.?.?? - TD 335186

#define MAX_UNPAID_TRANSACTION	9		//changed by nir from 4 to 9 at 24/11/05
	
#define TRS_PAYMENT_CARD_SWIPED 0x0001


#define MAX_DEFAULT_WAIT_TIMEOUT	5000  //4.0.29.500 - TD 275511
#define MAX_TERMINAL_SRV_WAIT	8000  //4.0.19.504 - 47559

//Paid Transaction  qdx define
#define MAX_PAID_TRS 100

//Minimal memo interval timer - 4.0.15.200
#define MIN_MEMO_INTERVAL_TIMER	 60000

#define MAX_DISCOUNTS_PER_ITEM	4 //4.0.17.501 - TD 38162

//4.0.22.500 70766 Language default messages 
#define SCREEN_MAX_COUPONS_STR	"Max coupons|has been reached"

#define MAX_TAXES_PER_PRODUCT	6	//4.0.25.90 - TD 142798
#define MAX_PRODUCTS			2	//4.0.25.90 - TD 142798

#define FLOW_RATE_INCLUDE_TRS_FLOW      0x1					//4.0.27.40 TD 165585
#define FLOW_RATE_INCLUDE_AVERAGE_FLOW  0x2					//4.0.27.40 TD 165585

#endif