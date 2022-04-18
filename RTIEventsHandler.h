#ifndef _RTI_EVENTS_HANDLER_
#define _RTI_EVENTS_HANDLER_

class CRTIEventsHandler: 
			public IDispEventImpl <
			0,
			CRTIEventsHandler,
			&DIID__IRTIEvents,
			&LIBID_LOYALTYFUELLib,
			1,
			0>,
			
			public CComObjectRootEx<CComMultiThreadModel> 
{
public:
	 
					  
	HRESULT __stdcall Error(BSTR sXMLError);	
	HRESULT __stdcall ResultStartTicket(BSTR sXMLDiscountGrade);	
	HRESULT __stdcall ResultAddItem(long lPumpNumber , BSTR sXMLAddItemDiscount);
	HRESULT __stdcall ResultTriggerProm(BSTR sXMLTriggerProm);
	HRESULT __stdcall ErrorAfterFuel(BSTR sXMLError);
	HRESULT __stdcall GeneralResult(BSTR sXMLResult);
	HRESULT __stdcall QueryResult(BSTR sXMLGradeDiscount);
	HRESULT __stdcall PhoneLookUpResult ( long lPumpNumber, int iRetCode, BSTR bstrRes); //4.0.23.1220 15575
	HRESULT __stdcall TargetMessageResult(long lPumpNumber, BSTR bstrTargetMessage);
	HRESULT __stdcall PromotionResult(long lPumpNumber,long lEarnValue, BSTR bstrRes);
	HRESULT __stdcall GetMemberInfoResult(long lPumpNumber, BSTR bstrRes); // CR 405435
	HRESULT __stdcall ResultTriggerProm2(long lPumpNumber, BSTR bstrRes);


	BEGIN_SINK_MAP(CRTIEventsHandler)		
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 2,	Error)
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 3,	ResultStartTicket)
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 5,	ResultAddItem)
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 6,	ResultTriggerProm)
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 7,	ErrorAfterFuel)
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 8,	GeneralResult)
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 9,	QueryResult)
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 10,	PhoneLookUpResult) //4.0.23.1220 155754
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 11,	TargetMessageResult) // CR 312183
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 12,	PromotionResult) // CR 312183
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 13,	GetMemberInfoResult) // CR 405435
		SINK_ENTRY_EX(0, DIID__IRTIEvents, 14,	ResultTriggerProm2)
	END_SINK_MAP()  

	CRTIEventsHandler();
	virtual ~CRTIEventsHandler();
	void 	SetEventClientId(long lClientId);
private:
	long m_lEventsClientId;
};






#endif//_RTI_EVENTS_HANDLER_

