//-----------------------------------------------------------------------
// File Name   :	OLA.h
// 
// Description :	Basic Definitions for On Line Authorization
//					
// Written by  :	Moti Lichi 10.02.98
// 
//-----------------------------------------------------------------------
 
#ifndef __OLA_H_
#define __OLA_H_

//#pragma pack(1)

#include  "commonDef.h"
#include "PumpSrvStructures.h"

using namespace PS;

enum OlaSrvStatus
{
	OLA_SERVER_OFF,				// 0
	OLA_SERVER_ON,				// 1
	OLA_SERVER_WAIT_FOR_KILL,	// 2
	OLA_SERVER_WAIT_FOR_START	// 3 
};

enum DebitSrvStatus
{
	DEBIT_SERVER_IDLE,				// 0
	DEBIT_SERVER_REQUEST_TO_START,	// 1
	DEBIT_SERVER_INPROCESS,			// 2	
};

#define	MAX_DECLINE_MESSAGES	100
#define	REPORT_TEXT_LENGTH		2000
#define MAX_BEDIT_BUFFER		400

#define MAX_TERMINAL_MSG_LEN	80   //4.0.5.39
#define PUMPSRV_REQUEST         99

//Constants for OLA 
#define NEW_OLA_STRUCT		1
#define CANCEL_IN_MIDDLE	1

//4.0.13.505
//OLA Server type definitions
#define OLASRV_NOT_CONFIGURED           0 //No OLA configured, or server has not been installed
#define	OLASRV_POSITIVE					1
#define	OLASRV_PROGRESS					2
#define	OLASRV_POSWARE					3
#define	OLASRV_POSITIVE_FIPAY			4
#define OLASRV_POSITIVE_EVENTS			5
#define OLASRV_POSITIVE_32              6
#define OLASRV_POSITIVE_32_EVENTS       7
#define OLASRV_POSITIVE_IFSF	        8   //4.0.5.39
#define OLASRV_TRAVEL_CENTER			9	//4.0.9.509	
#define OLASRV_PROGRESS_EX				10  //4.0.12.505
#define OLASRV_DOT_NET					11  //4.0.16.504
#define OLASRV_STORE_LINE				12	//TD 42728
#define OLASRV_SIM_LINK_SRV				13	//4.0.20.500 TD 27377
#define OLASRV_POSITIVE_US				14  //TD 72894 - 4.0.22.504 
#define OLASRV_EService_SRV           	15  //AlexM
#define OLASRV_CONNECTED_PAYMENTS		16  // 4.0.23.1491
#define OLASRV_EPS              		17  //R10 User Story - 71146
#define OLASRV_EPSILON             		18  //CR 418937
#define OLASRV_EPSILON_DISCOUNT    		19  //RFUEL-2817

#define OLASRV_SIMULATOR    	        99  //4.0.5.42

// PumpSrv OLA internal Status
#define OLA_NONE					0x00 // No Request no session

#define OLA_STATE(s)				(0x00ff & s)						//4.0.9.507
#define OLA_NEW_STATE(s,news)		((0xff00 & s)|(news & 0x00ff))//4.0.9.507

#define OLA_REQUEST_TO_SEND												0x01 
#define OLA_WAIT_RESPONSE												0x02
#define OLA_WAIT_FOR_MORE_INFO											0x03 
#define OLA_REQUEST_TO_SEND_MORE										0x04 
#define OLA_SEND_END													0x05
#define OLA_GET_CARD_INFO												0x06
#define OLA_WAIT_REEFER													0x07	//4.0.9.507
#define OLA_LOYALTY_START_TICKET									    0x08	//4.0.9.508
#define OLA_LOYALTY_WAIT_RESPONSE										0x09	//4.0.9.508
#define OLA_LOYALTY_WAIT_QUERY_RESPONSE									0x0A	//4.0.23.950 216343
#define OLA_LOYALTY_WAIT_QUERY_CW_RESPONSE								0x0B	//4.0.23.950 216343
#define OLA_LOYALTY_ADD_ITEM											0x0C	//4.0.9.508
#define OLA_LOYALTY_END_TICKET											0x0D	//4.0.9.508
#define OLA_SEND_ENDEX													0x0E	//4.0.9.509	
#define OLA_WAIT_CONTROLLER_RESPONSE 									0x0F	//4.0.9.508

#define OLA_LOYALTY_CANCEL_TICKET_AFTER_START_TICKET					0x10	//Liat LPE	//Copient 4.0.19.502 + Rtaps 59557
#define OLA_WAIT_CONTROLLER_MSG											0x11	//Liat LPE
#define OLA_LOYALTY_WAIT_MORE_DATA										0x12	//Liat LP
#define OLA_LOYALTY_START_FUELING										0x13	//Liat copient
#define OLA_LOYALTY_CANCEL_TICKET_BEFORE_START_TICKET					0x14	//Copient 4.0.19.502 + Rtaps 59557
#define	OLA_LOYALTY_WAIT_FUELING										0x15	//4.0.20.611 72527 //4.0.22.500
#define	OLA_LOYALTY_RUN_QUERY											0x16	//4.0.22.504 69344
#define	OLA_LOYALTY_ADD_MEMBER_CARD										0x17	//4.0.22.504 69344
#define OLA_LOYALTY_WAIT_CONTROLLER_MSG_START_TICKET_FAIL				0x18	//4.0.22.504 69344
#define OLA_LOYALTY_WAIT_PHONE_LOOKUP_RESPONSE							0x19	//4.0.23.1220 155754
#define OLA_LOYALTY_WAIT_END_TICKET_RESPONSE							0x1A

/// Before fueling before start ticket
#define OLA_LOYALTY_START_NMP_BEFORE_FUELING_BEFORE_START_TICKET		0x1B	//Liat copient
#define OLA_LOYALTY_CW_BEFORE_FUELING_BEFORE_START_TICKET				0x1C	//Liat copient
#define OLA_LOYALTY_COUPONS_BEFORE_FUELING_BEFORE_START_TICKET			0x1D	//Liat copient
#define OLA_LOYALTY_MEMBER_CARD_BEFORE_FUELING_BEFORE_START_TICKET		0x1E	//Liat copient
#define OLA_LOYALTY_PROMPT_BEFORE_FUELING_BEFORE_START_TICKET			0x1F	//Liat copient
#define OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_BEFORE_START_TICKET			0x20	//Liat copient	4.0.20.54 64528

/// Before fueling after start ticket
#define OLA_LOYALTY_START_NMP_BEFORE_FUELING_AFTER_START_TICKET			0x21	//Liat copient
#define OLA_LOYALTY_CW_BEFORE_FUELING_AFTER_START_TICKET				0x22	//Liat copient
#define OLA_LOYALTY_COUPONS_BEFORE_FUELING_AFTER_START_TICKET			0x23	//Liat copient
#define OLA_LOYALTY_MEMBER_CARD_BEFORE_FUELING_AFTER_START_TICKET		0x24	//Liat copient
#define OLA_LOYALTY_PROMPT_BEFORE_FUELING_AFTER_START_TICKET			0x25	//Liat copient
#define OLA_LOYALTY_DEFERRAL_BEFORE_FUELING_AFTER_START_TICKET			0x26	//Liat copient	4.0.20.54 64528

/// After fueling before add item
#define OLA_LOYALTY_START_NMP_AFTER_FUELING_BEFORE_ADD_ITEM				0x27	//Liat copient
#define OLA_LOYALTY_CW_AFTER_FUELING_BEFORE_ADD_ITEM					0x28	//Liat copient
#define OLA_LOYALTY_COUPONS_AFTER_FUELING_BEFORE_ADD_ITEM				0x29	//Liat copient
#define OLA_LOYALTY_MEMBER_CARD_AFTER_FUELING_BEFORE_ADD_ITEM			0x2A	//Liat copient
#define OLA_LOYALTY_PROMPT_AFTER_FUELING_BEFORE_ADD_ITEM				0x2B	//Liat copient
#define OLA_LOYALTY_DEFERRAL_AFTER_FUELING_BEFORE_ADD_ITEM				0x2C	//Liat copient	4.0.20.54 64528

//4.0.22.500 70767
#define OLA_LOYALTY_SWIPE_PAYMENT										0x2D
#define OLA_LOYALTY_AUTHORIZE_PAYMENT									0x2E
#define OLA_LOYALTY_SWIPE_LOYALTY										0x2F
#define OLA_LOYALTY_AUTHORIZE_LOYALTY									0x30
#define OLA_LOYALTY_CAR_WASH											0x31
#define OLA_LOYALTY_PROMPT												0x32
#define OLA_LOYALTY_METHOD_OF_PAYMENT									0x33//4.0.23.20 76383
#define OLA_LOYALTY_CAR_WASH_DISCOUNT									0x34	//4.0.23.950 216343
#define OLA_LOYALTY_PHONE_LOOKUP										0x35    //4.0.23.1220 155754
#define OLA_LOYALTY_PHONE_LOOKUP_ENTER_PHONE_NUMBER						0x36	//4.0.23.1220 155754
#define OLA_LOYALTY_PHONE_LOOKUP_ENTER_REWARD_CARD						0x37	//4.0.23.1220 155754
#define OLA_LOYALTY_TARGET_MESSAGE										0x38    // 312183
#define OLA_LOYALTY_WAIT_TARGET_MESSAGE_RESPONSE						0x39    // 312183
#define OLA_LOYALTY_WAIT_FOR_DEFERAL_ANSWER								0x3A    //4.0.23.???? TD 295237
#define OLA_LOYALTY_WAIT_ADD_MEMBER_INFO_RESPONSE						0x3B	// 4.0.23.2280 305114

#define OLA_LOYALTY_SWIPE_CARD											0x3C
#define OLA_LOYALTY_AUTHORIZE_CARD										0x3D
#define OLA_LOYALTY_DISPLAY_PRE_AUTHOIRZE_SCREEN						0x3E
#define OLA_LOYALTY_CARWASH_SALE										0x3F
#define OLA_LOYALTY_CARWASH_PROMOTION									0x40
#define OLA_LOYALTY_DISPLAY_PARITAL_APPROVAL_SCREEN						0X41
#define OLA_LOYALTY_DECLINE												0X42
#define OLA_LOYALTY_WAIT_GET_MEMBER_INFO_RESPONSE						0x43
#define OLA_LOYALTY_ALTERNATE_ID										0x44
#define OLA_LOYALTY_ALTERNATE_ID_MANUAL_ENTER							0x45
#define OLA_LOYALTY_ALTERNATE_ID_SWIPE_LOYALTY_CARD						0x46
#define OLA_LOYALTY_MEMBER_NOT_ACTIVE									0x47
#define OLA_LOYALTY_WAIT_MEMBER_INFO_RESPONSE							0x48	// 305114
#define OLA_LOYALTY_INVALID_INPUT										0x49
#define OLA_LOYALTY_ALTERNATE_ID_RETRY									0x4A	// CR 501164
#define OLA_LOYALTY_WAIT_FOR_ALTERNATE_ID_RETRY_ANSWER					0x4B    // CR 501164

#define OLA_LOYALTY_APPLY_PAYMENT_DISCOUNT								0x4C     // RFUEL-2817

//4.0.9.509
#define ADD_DRY_ITEMS	1
#define ADD_REEFER		2
#define ADD_CASH_BACK	4
#define ADD_DIESEL		8


#define SESSION_TYPE(s)				(0xff00 & s)				//4.0.9.507
#define SESSION_NEW_TYPE(s,news)	((0x00ff & s)|(0xff00 & news))//4.0.9.507

#define SESSION_AUTHORIZE			0x100
#define SESSION_COMPLETE			0x200
#define SESSION_GET_RECEIPT			0x300
#define SESSION_RECEIPT_UPDATE		0x400
#define SESSION_RECEIPT_AUTH		0x500
#define SESSION_CANCEL				0x600// equel to force with value 0.
#define SESSION_CANCEL_AUTH			0x700 // Cancel in middle of auth, wait for "Authorize" session to conclude
#define SESSION_SALE				0x800 //3.0.0.8
#define SESSION_GENERATE_REPORT		0x900
#define SESSION_GET_REPORT			0xa00
#define SESSION_INIT_DEBIT			0xb00 //husky april //3.2.0.9
#define SESSION_SHIFT_DEBIT			0xc00 //husky april //3.2.0.9
#define SESSION_CAPTURE				0xd00 //delek 5.0.0.55
#define SESSION_CLOCK				0xe00 //delek 5.0.0.55
#define SESSION_INQUIRY				0xf00 //4.0.3.21
#define SESSION_REPORT_PRICE_CHANGE	0x1000 //4.0.3.21
#define SESSION_LOGON               0x2000  //TD 256429 
#define SESSION_GET_CUSTOMER_INFO	0x3000
#define SESSION_SET_CUSTOMER_INFO	0x4000


//4.0.5.24
// session type  
#define ST_DIRECT					0
#define ST_SESSION					1


#define NO_SESSION	-1 // Session Number can be 0-9999


#define REQUIRED_CAR_REGISTRATION		0x000000001
#define REQUIRED_ODOMETER				0x000000002
#define REQUIRED_PIN					0x000000004
#define REQUIRED_DRIVER_ID				0x000000008
#define REQUIRED_RETURN_TANK			0x000000010
#define REQUIRED_PAYMENT_CARD			0x000000020
#define REQUIRED_PROMPT_REWARDS			0x000000040
#define REQUIRED_VOUCHER_NUMBER			0x000000080
#define REQUIRED_KEY_CODE				0x000000100
#define REQUIRED_GENERAL_ID				0x000000200
#define REQUIRED_MEDIA_TYPE				0x000000400
#define REQUIRED_UNIT					0x000000800
#define REQUIRED_TRIP_NUMBER			0x000001000	
#define REQUIRED_CUSTOMER_REFERENCE		0x000002000
#define REQUIRED_TRACTOR_HUB			0x000004000
#define REQUIRED_TRAILER_NUMBER			0x000008000
#define REQUIRED_TRAILER_HUB			0x000010000
#define REQUIRED_TRANS_AMOUNT			0x000020000
#define REQUIRED_DECLINE_LOYALTY_PROMTE	0x000040000
#define REQUIRED_PASS_CODE				0x000080000
#define REQUIRED_PINPAD_ACTION			0x000100000 //3.1.1.1
#define REQUIRED_CHARGE_TO				0x000200000 //4.0.1.1
#define REQUIRED_ZIP_CODE				0x000400000 //4.0.1.31
#define REQUIRED_REDEED_POINT_ACTION	0x000800000 //4.0.3.47
#define REQUIRED_CONFIRM_PROMPT_ACTION	0x001000000 //4.0.3.54
#define REQUIRED_PAYMENT_LOYALTY_CARD	0x002000000 //4.0.5.26  
#define REQUIRED_PARAM_RESULT_DATA		0x004000000 //4.0.5.39  
#define REQUIRED_REMOTE_ID				0x008000000 //4.0.5.41  
#define REQUIRED_LOGIC_SHIFT_NUMBER		0x010000000 //4.0.5.42
#define REQUIRED_AUTHORIZATION_NUMBER	0x020000000 //4.0.6.502
#define REQUIRED_CnP_PARAM				0x040000000 //4.0.6.502
#define REQUIRED_PO_NUMBER_PARAM		0x080000000 //4.0.15.502
#define REQUIRED_JOB					0x100000000 //4.0.22.511 74797
#define REQUIRED_DEPARTMENT				0x200000000 //4.0.22.511 74797
#define REQUIRED_LICENSE				0x400000000 //4.0.22.511 74797
#define REQUIRED_USER_ID				0x800000000 //4.0.22.511 74797
#define REQUIRED_ACCOUNT_TYPE			0x1000000000 //Costco EMV
#define REQUIRED_MORE_EMV_DATA			0x2000000000


//===================================================================
// OLA DLL 
//===================================================================
// OLA DLL Flags 
#define AUTHORIZE_REQUEST						 0x0000  // Pre authorize
#define FORCE_REQUEST							 0x0001  // Billing

#define STRUCT_IS_CARD_SALE_DATA 				0x0002	// card original request information
#define STRUCT_IS_CARD_SALE_EXTRA_DATA			0x0004	// card extra sales (st the pump) info 
#define STRUCT_IS_CARD_ALL_DATA 				0x0008	// combination of the above
#define STRUCT_IS_RECEIPT_DATA 					0x0010	// combination of the above
#define STRUCT_IS_CARD_SALE_EXTRA_DATA2			0x0020	// card extra sales (st the pump) info2
#define STRUCT_IS_CARD_ALL_DATA2 				0x0040	// combination of the above
#define PAP_SERVICE_FEE_CHARGED_BY_POS			0x0080
#define STRUCT_IS_CARD_SALE_EXTRA_DATA2_PLUS	0x0100  // 3.1.1.1
#define STRUCT_IS_CLOCK_DATA					0x0200  // 5.0.0.55
#define PAP_DECLINE_INFO						0x0800  // use to update buffer MemOlaInfo with the decline code
#define STRUCT_IS_UPDATE_RECEIPT				0x1000  // Update Receipt Qdx  4.0.2.29
#define PAP_FORCE_CLOSE_SESSION					0x2000  //4.0.3.12 Force close ola session.
#define PAP_OPEN_SESSION_COMPLETE				0x4000
#define STRUCT_IS_CARD_ALL_DATA3 				0x8000	// combination of the above
#define STRUCT_IS_CARD_SALE_EXTRA_DATA3_PLUS	0x10000  // 4.0.5.0
#define STRUCT_IS_RECEIPT_DATA3 				0x20000	// 4.0.5.0
#define STRUCT_IS_RECEIPT_DATA4 				0x40000	// 4.0.5.39
#define TRS_PAK_CHANGE_PAP						0x80000   //4.0.5.34
#define TRS_OLA_TRANSACTION_CHANGE_PAK		    0x100000   //4.0.5.42
#define STRUCT_IS_PAP_CANCEL					0x200000   //4.0.5.42
#define PAP_WITH_CARWASH						0x400000   //4.0.7.500
#define TRS_OLA_FORCE_CLOSE_AS_ATTENDANT		0x800000   //4.0.11.505
#define STRUCT_IS_UPDATE_OLA					0x1000000  //4.0.12.501
#define STRUCT_IS_SALE_COMMAND					0x2000000  //4.0.12.501
#define STRUCT_IS_CARD_SALE_EXTRA_DATA4_PLUS	0x4000000  //4.0.19.502 - CR 58096
#define STRUCT_IS_OFFLINE_PUMP_TRANSACT			0x8000000  //offline trs
#define STRUCT_IPT_CARWASH					    0x10000000  //IPT transaction trs //marikTest //4.0.23.504


// OLA DLL Return Card Type
#define CARD_TYPE_NOT_VALID	0
#define CARD_TYPE_AX		1 
#define CARD_TYPE_CB		2
#define CARD_TYPE_DC		3
#define CARD_TYPE_DS		4
#define CARD_TYPE_MC		5
#define CARD_TYPE_VI		6
//===================================================================

// Slip flag
#define IGNORE_ODOMETER			0x01
#define IGNORE_CAR_REG			0x02

#define NOT_IGNORE_ODOMETER		~(0x01)
#define NOT_IGNORE_CAR_REG		~(0x02)


//===================================================================
// OLASrv 
//===================================================================
// OLASrv return codes
#define OLASRV_PROCESS_FINISHED			0	// 
#define OLASRV_PROCESS_NOT_FINISHED		-1  //
#define OLASRV_NEED_MORE_PARAMS			-2  // 
#define OLASRV_GENERAL_ERROR			-3  //
#define OLASRV_CONVERT_LAYER_ERROR		-4  //
#define OLASRV_NOT_SUPPORTED			-5
#define OLASRV_UNKNOWN_SESSION_ID		-6
#define OLASRV_NO_HOST					-7

#define OLASRV_IN_MAINTENANCE			-16
#define OLASRV_COM_NOT_RESPONSE			-98
#define OLASRV_COM_ERROR				-99
#define OLASRV_COM_ERROR_OLA_EXCEPTION	-100


// OLASrv ResultCode[S] values:
#define OLASRV_Approved							0	// Approved OK
#define OLASRV_BatchOK						    1	// Succesful receipt of batch totals
#define OLASRV_Decline_FailedSelfCheck			101 // Failed to pass self check or denied by the HOST system
#define OLASRV_Decline_CardExpired				102 // Card expired
#define OLASRV_Decline							103 // Transaction has been declined
#define OLASRV_Decline_DebitNetworkDown			104 // Debit network is down
#define OLASRV_Decline_InvalidStateCode			105 // State code invalid
#define OLASRV_Decline_BadFormat				106 // Transaction data is invalid or formated incorrectly by the site	
#define OLASRV_Decline_NoHost					107 // Host is experiencing system problem
#define OLASRV_Decline_UseAlt					108 // Use alternate authorization method
#define OLASRV_Decline_CallCenter				109	// Call center for voice authorization
#define OLASRV_Decline_PINError					110	// Excessive PIN Error
#define OLASRV_Decline_VoidNoTarget				111 // Void has no target on host system
#define OLASRV_Decline_NoMoney					112 // Insufficient funds
#define OLASRV_Decline_AmountMismatch			113 // Void amount does not match the target transaction on the host
#define OLASRV_Decline_AccountMismatch			114 // Void account number does not match the target transaction on the host.
#define OLASRV_Decline_InvalidDriverID			115 // Invalid driver number
#define OLASRV_Decline_InvalidVeicleID			116 // Invalid veicle number
#define OLASRV_Decline_InvalidMerchantID		117 // Merchant ID error
#define OLASRV_Decline_InvalidPIN				118 // PIN error
#define OLASRV_Decline_BatchOutOfBalance		119 // Batch was closed out of balance
#define OLASRV_Decline_RetriveCard				120 // Transaction is declined and the card should retrieved from the customer
#define OLASRV_Decline_Transaction				121 // Transaction is declined
#define OLASRV_Decline_DateTime					122 // Date/Time error
#define OLASRV_Decline_CardType					123 // Card not accepted by the merchant
#define OLASRV_Decline_Error					124 // Unknown error
#define OLASRV_Decline_BadSwipe                 125 
#define OLASRV_Decline_CardNotValid				126
#define OLASRV_Decline_AmountOutsideRange       127
#define OLASRV_Decline_UnknownPromptId          128
#define OLASRV_Decline_InvalidProdId            129
#define OLASRV_Decline_RestrictedAccountNumber  130
#define OLASRV_Decline_DailyLimitExceeded		131
#define OLASRV_Receipt_PrintedBefore			132
#define OLASRV_Receipt_NoReceipt				133
#define OLASRV_Receipt_OldReceipt				134
#define OLASRV_Close_Batch_Faild				135
#define OLASRV_Decline_NoMember					136
#define OLASRV_Decline_NotInValidRang   		137
#define OLASRV_Decline_PanMismatchCard  		138  // 4.0.31.270 FSD 403522 [23/7/2014 MaximB]
#define OLASRV_Decline_ContactLessError		    139  //  [16/6/2015 MaximB]


#define OLASRV_Decline_GR						200

#define OLASRV_Decline_Transaction_Not_Completed 226 // debit decline 4.0.3.2
#define OLASRV_Decline_Transaction_Not_Approved	 227 // debit decline 4.0.3.2

#define OLASRV_Decline_Transaction2				228 // Transaction is declined SVS //4.0.2.36
#define OLASRV_Decline_NoMoney2					229  //// Insufficient funds SVS 4.0.2.36


//3.2.0.73
//3.2.1.312
#define OLASRV_Decline_balaceAmount				230
#define OLASRV_Decline_payment_Need_clear_OLA_buffer_except_loyalty    231  //4.0.14.505

//4.0.14.1470 68304
#define OLASRV_MembershipExpired				240
#define OLASRV_MembershipInactive				241
#define OLASRV_MembershipPendingExpired			242
#define OLASRV_MembershipPendingInactive		243

//3.2.0.73
#define OLASRV_Offline_Host						403

#define Loyalty_Unavailable						900

#define CARDSRV_Decline_Card_Blocked			251

// CardSrv ResultCode[I] values:
#define	CARDSRV_Approved						0
#define	CARDSRV_NeedMoreParam					100


//#define MAX_AMOUNT_FOR_PRE_AUTH			35000L	// 35 $
#define SURCHARGE_AMOUNT_FOR_PRE_AUTH	0L		// 0$

//Open Need More Param properties	55286 Copient	4.0.19.502
#define ONMP_OpenCardReader			0x0001
#define ONMP_OpenScanner			0x0002
#define ONMP_OpenYesNoKeys			0x0004
#define ONMP_OpenEnterCancelKeys	0x0008
#define ONMP_OpenNumericKeys		0x0010
#define ONMP_YesAsEnter				0x0020
#define ONMP_NoAsCancel				0x0040
#define ONMP_EnterAsYes				0x0080
#define	ONMP_LoyaltyCard			0x0100	//4.0.20.50
#define ONMP_OpenPayInsideKey		0x0200	//4.0.23.860 212541
#define ONMP_YesAsPayPal			0x0400   // TD 339257
#define ONMP_OtherAmount			0x0800
#define ONMP_AuthorizedAmount		0x1000
#define ONMP_OpenContaless			0x2000	// TD 460484 [7/28/2016 MaximB]
#define ONMP_OpenSoftKeys			0x4000	// UDF - Epsilon
#define ONMP_AlternateID			0x8000	// UDF - Epsilon
#define ONMP_UseNMPOption			0x10000


// Allied 
//-----------------------------------------------------
//	A = Attendant Card Sale
//	B = Cash Sale
//	C = Credit Sale
//	D = Debit Sale
//	E = Barcode Sale
//	F = Credit Portion of Split Payment
//	G= Debit Portion of Split Payment
//	H= Prepaid Portion of Split Payment
//	I= Loyalty Transaction with Debit Card
//	K= ExxonMobil-- Post Sale from Sales
//	L= Loyalty Transaction (ExxonMobil Credit Card w/ Loyalty)
//	M= Loyalty Transaction w/ RFID device
//	P= Prepaid Card Sale
//	T = RFID Transaction
//	U = Unknown Card-Type (POS determines Credit or Debit)
//	W= Dealer Card
//	X= Member ID, MOP is yet unknown
//	Y= Loyalty Transaction w/ Proprietary Card
//	Z= Proprietary Card Sale
//-------------------------------------------------------


//	Types of Transaction (sTranType)
#define	CD_TYPE_AVI					'A'
#define	CD_TYPE_CASH				'B'
#define	CD_TYPE_CREDIT				'C'
#define	CD_TYPE_DEBIT				'D'
#define	CD_TYPE_CASH_CARD			'E'
#define	CD_TYPE_ATTENDANT			'V'
#define	CD_TYPE_LOYALTY				'M'
#define	CD_TYPE_CASH_ACCEPTOR		'R'  //4.0.1.38
#define	CD_TYPE_DELAYED_PREPAY		'P'  //4.0.2.23
#define	CD_TYPE_PAK_TO_PAP_DECLINE	'F'  //4.0.2.23
#define	CD_TYPE_SPEED_PASS			'T'  //4.0.5.24
#define	CD_TYPE_FLEET				'H'  //4.0.5.24
#define CD_TYPE_VOUCHER				'L'  //4.0.22.500 70767
#define CD_TYPE_MOBILE				'W'
#define CD_TYPE_PAYPAL				'Z'	 // TD 339257


//	Summarized result 
#define	CD_OK						0
#define	CD_NOT_OK					1
#define	CD_MORE_DATA_REQUIRED		2


#define	CD_RECEIPT					3
#define	CD_NO_RECEIPT				4
#define	CD_FORCE_OK					5
#define	CD_FORCE_NOT_OK				6
#define	CD_RECEIPT_UPDATE_OK		7
#define	CD_RECEIPT_UPDATE_NOT_OK	8


#define CD_HOT_PUMP_AUTHORIZE		9
#define	CD_CLOCK_OK					10
#define	CD_CLOCK_NOT_OK  			11
#define	CD_CANCEL_OK	 			12   //4.0.6.30



// sTranStatus 
#define CD_READY				' ' // Ready Waiting for sending 
#define CD_RESPONSE_EXPECTED	'1' // Request sent ,  Waiting for response
#define CD_APPROVE_OK			'A'	// Approved
#define CD_APPROVE_BACTCHOK		'X'	// Successful receipt of batch totals
#define CD_DECLINE_INVALID		'2'	// Account Number does not pass self-check
#define CD_DECLINE_EXPIRE		'3'	// The card being processed has expired
#define CD_DECLINE_DECLINED		'4'	// The transaction has been declined
#define CD_DECLINE_NONET		'5'	// The Network is Down
#define CD_DECLINE_NOSTATE		'6'	// Invalid State Code
#define CD_DECLINE_BADFMT		'7'	// Transaction data is invalid of formatted wrong
#define CD_DECLINE_NOHOST		'8'	// Host is having system problems
#define CD_DECLINE_USEALT		'B'	// Use an alternate authorization method
#define CD_DECLINE_CALL			'C'	// Call center for a voice authorization
#define CD_DECLINE_TRYPIN		'D'	// Excessive PIN errors - decline due to X retries.
#define CD_DECLINE_VOID			'E'	// Void has no target on the host system
#define CD_DECLINE_NOMONEY		'F'	// Insufficient funds
#define CD_DECLINE_AMTERR		'G'	// Void Amount does not match the target transaction
#define CD_DECLINE_ACTERR		'H'	// Void account # does not match the target transaction
#define CD_DECLINE_DRIVER		'J'	// Invalid Driver #
#define CD_DECLINE_VEHICLE		'K'	// Invalid Vehicle #
#define CD_DECLINE_MERCHANT		'M'	// Merchant ID Error - Company/terminal Id invalid
#define CD_DECLINE_PIN			'N'	// Invalid PIN - please reenter PIN
#define CD_DECLINE_OUTBAL		'O'	// Batch was closed out of balance
#define CD_DECLINE_GETCARD		'P'	// Issuer wants card from customer
#define CD_DECLINE_NOGOOD		'R'	// Transaction is declined
#define CD_DECLINE_DATETIME		'T'	// Date/Time Error
#define CD_DECLINE_BADCARD		'U'	// Card is not accepted by the merchant
#define CD_DECLINE_ERROR		'?'	// Host comm did not occur due to error flagged by PCPOS

#define CD_DECLINE_DAILYLIMIT	'V'	// Daily limit exceeded
#define CD_DECLINE_GR			'W'	
#define CD_DECLINE_NO_NOZZLE	'Z'	//The card was approved but none of the grade match any nozzle at the pump
#define CD_DECLINE_NO_MEMBER	'9'	//OLA expect club member first.
#define CD_DECLINE_MODE			'm'	//Decline because PAP is not valid in the current mode
#define CD_VIT_CONFLICT			'c'	// VIT Conflict was detected. save data for later usage //3.0.0.8
#define CD_DECLINE_AMOUNT		'a'	// Decline PAP transaction with approve for amount equal or less to zero. 
#define CD_FORCE_COMPLETION_ZERO 'b'// Decline PAP transaction with sending zero completion. 3.2.0.64
#define CD_DECLINE_NOT_VALID_RANG 'd' //Transaction code is not in valid code.  //4.0.0.78
#define CD_DECLINE_DECLINE_TRANSACTION_2 'e' //Transaction code invalid SVS.  4.0.2.36
#define CD_DECLINE_NOMONEY_2			'f'	// Insufficient funds    //4.0.2.36

#define CD_DECLINE_DSS_ERROR                  'g' // DSS decline
#define CD_DECLINE_TRANSACTION_NOT_COMPLETED 'j' // debit decline 4.0.3.2
#define CD_DECLINE_TRANSACTION_NOT_APPROVED	  'h' // debit decline 4.0.3.2

#define CD_DECLINE_PAYMENT_NEED_CLEAR_OLA_BUFFER_EXCEPT_LOYALTY  '{'   //4.0.14.505

#define CD_DECLINE_MEMBERSHIP_EXPIRED				'r'
#define CD_DECLINE_MEMBERSHIP_INACTIVE				's'
#define CD_DECLINE_MEMBERSHIP_PENDING_EXPIRED		't'
#define CD_DECLINE_MEMBERSHIP_PENDING_INACTIVE		'u'

////4.0.22.0 70991
#define CD_DECLINE_CARD_BLOCKED						'v'
#define CD_DECLINE_INVALID_LOYALTY_CARD				'x'

#define CD_DECLINE_PAN_MISMATCH_CARD				'w'		// 4.0.31.270 FSD 403522 [23/7/2014 MaximB]
#define CD_DECLINE_CONTACTLESS_ERROR				'y'		//  [16/6/2015 MaximB]

// PumpSrv internal flage (sFlags)
#define FL_DO_NOT_OPEN_TRS					 1//'1'   //4.0.3.27
#define FL_CONNECTED_RECORD_IN_ASH2000_FILE	 2 //'2'  //4.0.3.49
#define FL_PAK_TO_PAP						 4 //'4'  //4.0.3.45
#define FL_PAP_AS_THIRD_REC					 8 //  //4.0.5.00
#define FL_EXTRA_ITEM_REC		 			16 //  //4.0.5.27
#define FL_EXTRA_RECEIPT_REC		 		32 //  //4.0.5.39
#define FL_CANCEL_REC		 				64 //  //4.0.6.501
#define FL_DIESEL_REC		 				0x80 //  //4.0.9.507
#define	FL_SALE_REC							0x100 //4.0.12.501 for car wash sale.
#define FL_PAP_TO_PAK						0x200	//4.0.22.506 67786
#define FL_OLA_EXTRA_DATA					0x400	//4.0.24.90 118124
#define FL_PAP_PAYMENT_ACCEPTED  			0x800	//4.0.23.273?
#define FL_PAP_LOYALTY_ACCEPTED				0x1000	//4.0.23.273?

// sRestrictions 
#define OLA_RESTRICT_NONE		' ' // no restrictions 

// sDeclinedLoyaltyForamt

#define DECLINE_LOYALTY_NON				'0'
#define DECLINE_LOYALTY_INVALID			'1'
#define DECLINE_LOYALTY_EXPIRED			'2'
#define DECLINE_LOYALTY_WITH_PAYCARD	'3'
#define DECLINE_PAYMENT_CARD			'4'  // 4.0.0.86


// Restriction by grade 
#define OLA_RESTRICT_FUEL_ONLY	'0' // 
#define OLA_RESTRICT_GRADE_1	'1' // only specific grade is allowed
#define OLA_RESTRICT_GRADE_2	'2' // 
#define OLA_RESTRICT_GRADE_3	'3' // 
#define OLA_RESTRICT_GRADE_4	'4' // 
#define OLA_RESTRICT_GRADE_5	'5' // 
#define OLA_RESTRICT_GRADE_6	'6' // 
#define OLA_RESTRICT_GRADE_7	'7' // 
#define OLA_RESTRICT_GRADE_8	'8' // 
#define OLA_RESTRICT_GRADE_9	'9' // 
#define OLA_RESTRICT_GRADE_10	'A' // 
#define OLA_RESTRICT_GRADE_11	'B' // 
#define OLA_RESTRICT_GRADE_12	'C' // 
#define OLA_RESTRICT_GRADE_13	'D' // 
#define OLA_RESTRICT_GRADE_14	'E' // 
#define OLA_RESTRICT_GRADE_15	'F' // 
#define OLA_RESTRICT_GRADE_16	'G' // 

// Restriction by nozzle 
#define OLA_RESTRICT_NOZZLE_1	'1'
#define OLA_RESTRICT_NOZZLE_2	'2'
#define OLA_RESTRICT_NOZZLE_3	'3'
#define OLA_RESTRICT_NOZZLE_4	'4'
#define OLA_RESTRICT_NOZZLE_5	'5'
#define OLA_RESTRICT_NOZZLE_6	'6'
#define OLA_RESTRICT_NOZZLE_7	'7'
#define OLA_RESTRICT_NOZZLE_8	'8'

// Restricted field, shouldn't be print on the receipt. (CARD_SALE_DATA::sReceiptRestriction)

#define PRINT_RECEIPT_TIME		0x00000001
#define PRINT_RECEIPT_PRICE		0x00000002

//Prompt Types
enum PromptType
{
	PT_NONE,			//0	
	PT_BOOL,			//1
	PT_STRING,			//2
	PT_NON_MASK_NUM,	//3
	PT_MASK_NUM,		//4
	PT_ENCRYPTED,		//5
	PT_CARD_READER,		//6
	PT_SCANNER,			//7
	PT_CONFIRM,			//8   //4.0.10.504
	PT_DISPLAY,			//9	  //4.0.10.504
	PT_GENERIC,			//10	//4.0.19.502 55286 Copient
	PT_DECLINE_MSG,		//11  // TD 68304
	PT_BOOL_AND_CARD_READER, //12	//4.0.22.506 67786
	PT_SUSPEND_IDLE,							//13   //4.0.22.507 TD 75089
	PT_BOOL_AND_CARD_READER_AND_ALT_ID,			 //14   //4.0.23.??? CR 321885
	PT_CAR_WASH,			//15 CR 405461
	PT_PRE_AUTHORIZE,
};

//4.0.6.506
//Upload Buffer types
#define UPLOAD_BUFF_TYPE_NONE	0
#define UPLOAD_BUFF_TYPE_AVI	1

struct CCMReserveCode{
	BYTE sSku[20];
	BYTE sUpc[15];
	BYTE sVaildDays[3];
	BYTE sUnitsPerDay[2];
	BYTE sBarcode[11];
	BYTE sUnitPrice[6];
	BYTE sWashCode[6];
	BYTE sExpiration[8];
	long lSyncNumber;
	BYTE sWeatherGuarentee[2];};





typedef	struct
{
	BYTE sGradeRestrictionLimit[7];  //#####.##
}GRADE_RESTRICTION_LIMIT;



//4.0.3.48
typedef struct
{
	BYTE byLen[4];
	BYTE byData[MAX_BEDIT_BUFFER];
}OLA_EXTRA_DATA;

/*
typedef struct  
{
	BYTE    sGradeID[2];
	long	lGradeLimit;
} GRADE_AMOUNT_LIMITS;
*/

typedef struct
{
	CARD_SALE_ALL			CardSaleAll;
	CARD_SALE_EXTRA_DATA2_  extData2;
}CARD_SALE_ALL2;
// 4.0.5.39
// typedef struct {
// 	BYTE	sFiler2043[2043];	//4.0.128.280 increase filer from 1019 to 2043
// }RECEIPT_EXTRA_INFO;


typedef struct
{
	CARD_SALE_ALL3 CardSaleAll3;
	OLA_EXTRA_DATA DebitExtraData;
	RECEIPT_EXTRA_INFO cReceiptExtraInfo;
}CARD_SALE_EXTRA_DATA4_PLUS;
//4.0.17.501 - CR 38162


typedef struct
{
	BYTE					   sTaxName[15];
	BYTE                       sTaxRate[5];
	BYTE                       sTaxAmount[10];
	BYTE                       sTaxId[3];
} TAX_DETAILS;





typedef struct 
{
	BYTE	sPinPadAction;				
	BYTE	sPinPadResult;				
	BYTE	sCalculatedMAC[8];			
	BYTE	sPinSessionKey[16];			
	BYTE	sMACSessionKey[16];			
	OLA_EXTRA_DATA sData;	
	BYTE	sToken[2];			//4.0.0.48	
	BYTE	sFiler[50];			//4.0.0.48
	BYTE	sTranStatus;
}OLA_DEBIT_PROCESS_INFO;

typedef struct
{
	SYSTEMTIME  cSysTime;
	long		lInterval;
}OLA_DEBIT_MESSAGES_TIME_OUT_INFO;






//4.0.12.501
typedef struct    {
	long lIdex;
	long lNumOfRows;
	BYTE sXML[991];
}XML_BUFFER;



typedef struct    // 4.0.12.501
{
	XML_BUFFER		sXML1;
	XML_BUFFER		sXML2;
	XML_BUFFER		sXML3;
}PREPAY_INFO;

//4.0.20.20 - Start
typedef struct
{
	BYTE byLen[4];
	BYTE byData[512];
}EXTRA_CHIP_INFO;


typedef struct   
{
	EXTRA_CHIP_INFO		byIssuerScriptData;
	EXTRA_CHIP_INFO		byIssuerScriptData2;

}EXTRA_CHIP_INFO_PACKAGE;

//4.0.20.20 - End

//#pragma pack()

#endif
