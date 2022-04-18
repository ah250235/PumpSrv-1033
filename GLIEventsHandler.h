
#ifndef _GLI_EVENTS_HANDLER_
#define _GLI_EVENTS_HANDLER_


class CGLIEventsHandler: public IDispEventImpl <0,CGLIEventsHandler,&DIID__IGLIEvents,&LIBID_FUELLOYALTYCLIENTLib,1,0>,public CComObjectRootEx<CComMultiThreadModel> 
{
public:

	HRESULT __stdcall Error(BSTR sXMLError);	
	HRESULT __stdcall ResultStartTicket(BSTR sXMLDiscountGrade);	
	HRESULT __stdcall ResultAddItem(long lPumpNumber ,BSTR sXMLAddItemDiscount);
	HRESULT __stdcall ResultTriggerProm(BSTR sXMLTriggerProm);
	HRESULT __stdcall ErrorAfterFuel ( BSTR sXMLError );
	HRESULT __stdcall GeneralResult ( BSTR sXMLResult );
	HRESULT __stdcall QueryResult ( BSTR sXMLGradeDiscount );
	HRESULT __stdcall PhoneLookUpResult ( long lPumpNumber , int iRetCode , BSTR bstrRes ); //4.0.23.1220 155754
	HRESULT __stdcall TargetMessageResult(long lPumpNumber ,BSTR bstrTargetMessage );
	HRESULT __stdcall PromotionResult(long lPumpNumber,long lEarnValue ,BSTR bstrRes );
	HRESULT __stdcall GetMemberInfoResult(long lPumpNumber,BSTR bstrRes); // CR 405435


	BEGIN_SINK_MAP(CGLIEventsHandler)		
		SINK_ENTRY_EX(0, DIID__IGLIEvents, 2,	Error)
		SINK_ENTRY_EX(0, DIID__IGLIEvents, 3,	ResultStartTicket)
		SINK_ENTRY_EX(0, DIID__IGLIEvents, 5,	ResultAddItem)
		SINK_ENTRY_EX(0, DIID__IGLIEvents, 6,	ResultTriggerProm)
		SINK_ENTRY_EX(0, DIID__IGLIEvents, 7,	ErrorAfterFuel)
		SINK_ENTRY_EX(0, DIID__IGLIEvents, 8,	GeneralResult)
		SINK_ENTRY_EX(0, DIID__IGLIEvents, 9,	QueryResult)
		SINK_ENTRY_EX(0, DIID__IGLIEvents, 10,	PhoneLookUpResult) //4.0.23.1220 155754
		SINK_ENTRY_EX(0, DIID__IGLIEvents, 11,	TargetMessageResult) // CR 312183
		SINK_ENTRY_EX(0, DIID__IGLIEvents, 12,	PromotionResult) // CR 312183	
		SINK_ENTRY_EX(0, DIID__IGLIEvents, 13,	GetMemberInfoResult) // CR 405435
	END_SINK_MAP()  

	CGLIEventsHandler();
	virtual ~CGLIEventsHandler();
	void 	SetEventClientId(long lClientId);
private:
	long m_lEventsClientId;
};

#endif//_GLI_EVENTS_HANDLER_
