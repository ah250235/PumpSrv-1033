
#ifndef __NMP_DETALIS__
#define __NMP_DETALIS__


//Please update NMP_MAX_TIME_OPTIONS in case of a change
enum NMP_TIME
{
	NMP_TIME_BeforeFuelingBeforeStartTicket,
	NMP_TIME_BeforeFuelingAfterStartTicket,
	NMP_TIME_AfterFuelingBeforeAddItem
};

//Please update NMP_MAX_TYPE_OPTIONS in case of a change
enum NMP_TYPE
{
	NMP_TYPE_CarWash,
	NMP_TYPE_Coupon,
	NMP_TYPE_LoyaltyCard,
	NMP_TYPE_MessagePrompt,
	NMP_TYPE_DeferralDiscount,	//4.0.20.54 64528
	NMP_TYPE_PaymentCard,		//4.0.22.500
	NMP_TYPE_AuthorizePayment,  //4.0.22.500
	NMP_TYPE_AuthorizeLoyalty,	//4.0.22.500
	NMP_TYPE_Fueling,			//4.0.22.500
	NMP_TYPE_MethodOfPayment,	//4.0.23.20 76383
	NMP_TYPE_RunQuery,	//4.0.23.860	212541
	NMP_TYPE_PhoneLookUp,        // 4.0.23.1220 TD 155754
	NMP_TYPE_CarWashDiscount,	//4.0.23.950 216343
	NMP_TYPE_TargetMessage,			// CR 312183
	NMP_TYPE_ApplyCalculateDiscount // RFUEL-2817
};


//4.0.19.503 Copient start
#define CANCEL_FROM_PAK			2
#define CAR_WASH_AFTER_FUELING	3

/////Flow control fields
#define NMP_FIELD_VALID_ENTRY			"ValidEntry"
#define NMP_FIELD_ORDER					"Order"
#define NMP_FIELD_REPEAT				"Repeat"
#define NMP_FIELD_CARD_READER			"CardReader"
#define NMP_FIELD_ENTER_CANCEL			"EnterCancel"
#define NMP_FIELD_YES_NO				"YesNo"
#define NMP_FIELD_SCANNER				"Scanner"
#define NMP_FIELD_NUMERIC				"Numeric"
#define NMP_FIELD_MESSAGE				"Message"
#define NMP_FIELD_TIMEOUT				"TimeOut"
#define NMP_FIELD_PAY_INSIDE			"PayInside"					//4.0.23.860 212541
#define NMP_FIELD_MANUAL_DEFERRAL		"ManualDeferralMessage"
#define NMP_FLOW_CONTROL_PAP_PATH		"FlowControl\\PAP"			//4.0.22.500
#define NMP_FLOW_CONTROL_PAK_PATH		"FlowControl\\PAK"			//4.0.22.504 69344
#define NMP_FIELD_CONTACTLESS			"Contactless"				// TD 460484 [7/28/2016 MaximB]
#define NMP_FIELD_OPEN_SOFT_KEYS		"SoftKeys"					// UDF - Epsilon
#define NMP_FIELD_ALTERNATE_ID			"AlternateID"				// UDF - Epsilon
#define NMP_FIELD_ALTERNATE_ID_RETRY	"AlternateIDRetryMessage"	// CR 501164


#define NMP_CAR_WASH_ENTRY_NAME						"OfferCarWash"
#define NMP_COUPON_ENTRY_NAME						"ScanCoupon"
#define NMP_LOYALTY_ENTRY_NAME						"SwipeLoyalty"
#define NMP_PROMPT_ENTRY_NAME						"Prompt"
#define NMP_DEFERRAL_ENTRY_NAME						"DeferralDiscount"	//4.0.20.54 64528
#define	NMP_PAYMENT_ENTRY_NAME						"SwipePayment" //4.0.22.500
#define	NMP_AUTHORIZE_PAYMENT_ENTRY_NAME			"AuthorizePayment" //4.0.22.500
#define NMP_AUTHORIZE_LOYALTY_ENTRY_NAME			"AuthorizeLoyalty" //4.0.22.500
#define NMP_FUELING_ENTRY_NAME						"Fueling"	//4.0.22.500
#define NMP_METHOD_OF_PAYMENT_ENTRY_NAME			"MethodOfPayment"	//4.0.23.20 76383
#define NMP_RUN_QUERY_ENTRY_NAME					"RunQuery"	//4.0.23.860 212541
#define NMP_CAR_WASH_DISCOUNT_ENTRY_NAME			"CarWashDiscount"	//4.0.23.950 216343
#define NMP_PHONE_LOOKUP_ENTRY_NAME					"PhoneLookUp"       //4.0.23.1220 155754 
#define NMP_MANUAL_DEFERRAL_ENTRY_NAME				"ManualDeferral"
#define NMP_TARGET_MESSAGE_ENTRY_NAME				"TargetMessage"     // CR 312183
#define NMP_ALTERNATE_ID_RETRY_NAME					"AlternateIDRetry"	// CR 501164
#define NMP_APPLY_CALCULATE_DISCOUNT_ENTRY_NAME		"ApplyCalculateDiscount"	//RFUEL-2817
#define NMP_USE_NMP_OPTION							"UseNMPOption"

//4.0.20.54 64528 //4.0.22.500 //4.0.23.20 76383 //4.0.23.860 212541 //4.0.23.950 216343 // //4.0.23.1220 TD 155754
// RFUEL-2817
#define NMP_MAX_TYPE_OPTIONS		15
#define NMP_DEFAULT_TIMEOUT			90
#define	NMP_MAX_NUM_OF_OPEN_KEYS	11
#define NMP_MAX_TIME_OPTIONS		3


//4.0.20.10 Start
#define	NMP_LOYALTY_DEFAULT_SETUP	(ONMP_OpenCardReader | ONMP_OpenScanner | ONMP_OpenYesNoKeys | ONMP_OpenEnterCancelKeys)
#define NMP_COUPON_DEFAULT_SETUP	0
#define	NMP_CARWASH_DEFAULT_SETUP	0
#define	NMP_CARWASH_DISCOUNT_DEFAULT_SETUP	0 //4.0.23.950 216343
#define NMP_PROMPT_DEFAULT_SETUP	ONMP_OpenYesNoKeys
#define NMP_DEFERRAL_DEFAULT_SETUP	ONMP_OpenYesNoKeys	//4.0.20.54 64528
//4.0.20.10 End
//4.0.22.500
#define	NMP_PAYMENT_DEFAULT_SETUP	(ONMP_OpenCardReader | ONMP_OpenYesNoKeys | ONMP_OpenEnterCancelKeys)
#define	NMP_AUTHORIZE_PAYMENT_DEFAULT_SETUP	0
#define	NMP_AUTHORIZE_LOYALTY_DEFAULT_SETUP	0
#define	NMP_FUELING_DEFAULT_SETUP	0
#define	NMP_METHOD_OF_PAYMENT_DEFAULT_SETUP	(ONMP_OpenCardReader | ONMP_OpenYesNoKeys) //4.0.23.20 76383
#define NMP_PHONE_LOOKUP_DEFAULT_SETUP  (ONMP_OpenNumericKeys | ONMP_OpenYesNoKeys) // 4.0.23.1220 155754

enum eCarWashSaleStatus
{
	eGetPromotion = 0,
	eCustomerAccpectedPromotion,
	eCustomerDeclinePromotion,
	eGetPromotionCentsOff
};

class CNMPDetails
{
public:
	CNMPDetails();
	virtual ~CNMPDetails(){};
	virtual void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola);
	virtual short CreateGeneralNMPProperties();
	virtual void LoadConfiguration();

	BOOL GetValidEntry() const {return m_bValidEntry;};
	BOOL GetCardReader() const {return m_bCardReader;};
	BOOL GetEnterCancel() const {return m_bEnterCancel;};
	BOOL GetYesNo() const {return m_bYesNo;};
	BOOL GetEnableScanner() const {return m_bEnableScanner;};
	BOOL GetEnableNumeric() const {return m_bEnableNumeric;};
	
	short GetState() const {return m_nOlaState;};
	// to sync Set and Get function name by creating alias function
	short GetStateType() const { return GetState(); }
	short GetNextFlowState() const {return m_iNextFlowState;};
	long GetRepeat() const {return m_lRepeat;};
	long GetCurrentLoopNum() const {return m_lCurrentLoopNum;};
	long GetTimeout() const {return m_lTimeout;};
	NMP_TIME GetNMPTime() const {return m_eNMPTime;};
	NMP_TYPE GetNMPType() const {return m_eNMPType;};
	char *GetMessage(){return m_sMessage;};
	char *GetPath(){return m_sPath;};	
	BOOL GetPayInside() const {return m_bPayInside;}; //4.0.23.860 212541
	
	void SetValidEntry(BOOL bVal){m_bValidEntry = bVal;};
	void SetStateType(short iVal){m_nOlaState = iVal;};
	void SetNextFlowState(short iVal){m_iNextFlowState = iVal;};
	void SetCurrentLoopNum(long lVal){m_lCurrentLoopNum = lVal;};
	void IncrementLoopNumber() { m_lCurrentLoopNum++; };
	void SetNMPTime(NMP_TIME eVal){m_eNMPTime = eVal;};
	void SetNMPType(NMP_TYPE eVal){m_eNMPType = eVal;};

	void SetMessage(char *sVal){strcpy_s(m_sMessage,MAX_FIELD_VAL,sVal);};
	void ClearMessage(){memset(m_sMessage,' ',MAX_FIELD_VAL);};
	void SetPath(char *sVal){strcpy_s(m_sPath,MAX_PATH,sVal);};
	void SetEnableScanner(BOOL bVal){m_bEnableScanner = bVal;};
	void SetEnableNumeric(BOOL bVal){m_bEnableNumeric = bVal;};
	void SetRepeat(long lVal){m_lRepeat = lVal;};
	void SetPayInside(BOOL bVal){m_bPayInside = bVal;}; //4.0.23.860 212541
	void SetCardReader(BOOL bVal){m_bCardReader = bVal;};
	void SetEnterCancel(BOOL bVal){m_bEnterCancel = bVal;};
	void SetYesNo(BOOL bVal){m_bYesNo = bVal;};
	void SetTimeout(long lVal){m_lTimeout = lVal;};
	void SetNmpDetails(short nNmpDetalis);
	void LoadMessage(long lPumpNumber , CString sDefaultMessage);

	// UDF - Epsilon
	void SetOpenSoftKeys(BOOL bVal) { m_bOpenSoftKeys = bVal; };
	void SetAlternateID(BOOL bVal) { m_bAlternateID = bVal; };

	long GetScreenNumber() const { return m_lScreenNumber; }
	void SetScreenNumber(long val) { m_lScreenNumber = val; }

	BOOL IsContaclessEnable() const { return m_bContacless; }		//TD 475451 
	void SetContaclessEnable(BOOL val) { m_bContacless = val; }		//TD 475451 

	void OpenNeedMoreParam(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, char * strMsg , int iMaxNumOfOpenKeys , long lTimeOut , short nONMPProperties,PromptType ePromptType = PT_GENERIC);

	// Dumps object detail in the logs
	// can override and create custom dump by the child classes
	virtual void DebugDump() const;

	BOOL GetUseNMPOption() const { return m_bUseNMPOption; };
	void SetUseNMPOption(BOOL val) { m_bUseNMPOption = val; }

protected:
	BOOL m_bValidEntry;
	short m_nOlaState;
	long m_lRepeat;
	long m_lCurrentLoopNum;
	BOOL m_bCardReader;
	BOOL m_bEnterCancel;
	BOOL m_bYesNo;
	BOOL m_bEnableScanner;
	BOOL m_bEnableNumeric;
	long m_lTimeout;
	char m_sMessage[MAX_FIELD_VAL];
	NMP_TIME m_eNMPTime;
	NMP_TYPE m_eNMPType;
	char m_sPath[MAX_PATH];
	short m_iNextFlowState;
	BOOL m_bPayInside; //4.0.23.860 212541
	BOOL m_bOpenSoftKeys;	// UDF - Epsilon
	BOOL m_bAlternateID;	// UDF - Epsilon
	long m_lScreenNumber;
	BOOL m_bContacless; // TD 460484 [7/28/2016 MaximB]
	BOOL m_bUseNMPOption;
};



//4.0.22.500 end
class CCarWashNMPDetails: public CNMPDetails
{
public:
	CCarWashNMPDetails();
	~CCarWashNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola) override;
};


class CCouponsNMPDetails: public CNMPDetails
{
public:
	CCouponsNMPDetails();
	~CCouponsNMPDetails(){};
};




#endif