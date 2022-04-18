
#include "stdafx.h"
#include "GLIEventsHandler.h"

CGLIEventsHandler* _pGLIEventsHandler;


CGLIEventsHandler::CGLIEventsHandler()
{
	m_lEventsClientId = 0;
}

CGLIEventsHandler::~CGLIEventsHandler()
{

}


HRESULT __stdcall CGLIEventsHandler::ResultStartTicket(BSTR sXMLDiscountGrade)
{

	_Module.m_server.m_cFuelLoayaltyPS.StartTicketResult(sXMLDiscountGrade);
	return S_OK;

}
HRESULT __stdcall CGLIEventsHandler::ResultAddItem(long lPumpNumber , BSTR bstrItemPromotionXML)
{
	try
	{
		BYTE sItemPromotionXML[MAX_XML_BUF];
		int iLen = Bstr2Buffer(bstrItemPromotionXML,sItemPromotionXML,sizeof(sItemPromotionXML));

		if(iLen <= 0)
		{
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,"CGLIEventsHandler::ResultAddItem recived empty buffer");
			_Module.m_server.m_cFuelLoayaltyPS.AddItemResult(lPumpNumber,NULL);
		}
		else
		{
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,"CGLIEventsHandler::ResultAddItem recevied AddItem Successfully");
			_Module.m_server.m_cFuelLoayaltyPS.AddItemResult(lPumpNumber,(char *)sItemPromotionXML);
		}

	}
	catch(_com_error & e )
	{
		_Module.m_server.m_cLogger.LogResult(0,"CRTIEventsHandler","ResultAddItem",FALSE,0, TRUE,e.Error(),e);
	}
	catch(...)
	{
		_Module.m_server.m_cLogger.LogMsg("CRTIEventsHandler::ResultAddItem Unexpected error has occurred!",LOG_LEVEL_1);
	}

	if(bstrItemPromotionXML)
	{
		SysFreeString(bstrItemPromotionXML);
	}

	return S_OK;
}

void CGLIEventsHandler::SetEventClientId(long lClientId)
{
	m_lEventsClientId = lClientId;	
}



HRESULT __stdcall CGLIEventsHandler::ResultTriggerProm(BSTR sXMLTriggerProm)
{

	_Module.m_server.m_cFuelLoayaltyPS.GetTriggerPromResult(sXMLTriggerProm);
	return S_OK;

}

HRESULT __stdcall CGLIEventsHandler::Error(BSTR sXMLError)
{
	_Module.m_server.m_cFuelLoayaltyPS.GetErrorFromRtapsAndContinue(sXMLError);
	return S_OK;

}

HRESULT __stdcall CGLIEventsHandler::ErrorAfterFuel ( BSTR sXMLError )
{

	_Module.m_server.m_cFuelLoayaltyPS.GetErrorAfterFuelAndContinue(sXMLError);
	return S_OK;
}

HRESULT __stdcall CGLIEventsHandler::QueryResult(BSTR sXMLGradeDiscount)
{

	_Module.m_server.m_cFuelLoayaltyPS.QueryResult(sXMLGradeDiscount);
	return S_OK;

}

HRESULT __stdcall CGLIEventsHandler::GeneralResult ( BSTR sXMLResult )
{
	_Module.m_server.m_cFuelLoayaltyPS.GeneralResult(sXMLResult);
	return S_OK;
}


//4.0.23.1220 155754
HRESULT __stdcall CGLIEventsHandler::PhoneLookUpResult(long lPumpNumber, int iRetCode, BSTR bstrRes)
{
	BYTE strClubCard[MAX_FIELD_VAL];
	int iLen = Bstr2Buffer(bstrRes,strClubCard,MAX_FIELD_VAL);

	_Module.m_server.m_cFuelLoayaltyPS.PhoneLookUpResult(lPumpNumber, iRetCode, strClubCard);

	return S_OK;
}

// CR 312183
HRESULT __stdcall CGLIEventsHandler::TargetMessageResult(long lPumpNumber, BSTR bstrTargetMessage)
{
	try
	{
		BYTE strTargetMessage[MAX_XML_BUF];
		int iLen = Bstr2Buffer(bstrTargetMessage,strTargetMessage,sizeof(strTargetMessage));

		if(iLen <= 0)
		{
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,"CGLIEventsHandler::TargetMessageResult Error while Extracting Target Message");
		}
		else
		{
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,"CGLIEventsHandler::TargetMessageResult Recevied Target Message Successfully");
			_Module.m_server.m_cFuelLoayaltyPS.TargetMessageResult(lPumpNumber , (char *) strTargetMessage);
		}
	}
	catch(_com_error & e )
	{
		_Module.m_server.m_cLogger.LogResult(0,"CGLIEventsHandler","TargetMessageResult",FALSE,0, TRUE,e.Error(),e);
	}
	catch(...)
	{
		_Module.m_server.m_cLogger.LogMsg("CGLIEventsHandler::TargetMessageResult Unexpected error has occurred!",LOG_LEVEL_1);
	}

	return S_OK;
}

HRESULT __stdcall CGLIEventsHandler::PromotionResult (long lPumpNumber ,  long lEarnValue, BSTR bstrRes )
{
	return S_OK;
}


// CR 405435
HRESULT __stdcall CGLIEventsHandler::GetMemberInfoResult(long lPumpNumber, BSTR bstrRes)
{
	return S_OK;
}


