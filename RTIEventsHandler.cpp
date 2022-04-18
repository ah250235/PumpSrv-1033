// GAIEventsHandler.cpp: implementation of the CGAIEventsHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RTIEventsHandler.h"
#include "GLIEventsHandler.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CRTIEventsHandler* _pRTIEventsHandler;



CRTIEventsHandler::CRTIEventsHandler()
{
	m_lEventsClientId = 0;
}

CRTIEventsHandler::~CRTIEventsHandler()
{

}

HRESULT __stdcall CRTIEventsHandler::ResultStartTicket(BSTR sXMLDiscountGrade)
{
	
	_Module.m_server.m_cFuelLoayaltyPS.StartTicketResult(sXMLDiscountGrade);
	return S_OK;

}


HRESULT __stdcall CRTIEventsHandler::ResultAddItem(long lPumpNumber , BSTR bstrItemPromotionXML)
{
	try
	{
		BYTE sItemPromotionXML[MAX_XML_BUF];
		int iLen = Bstr2Buffer(bstrItemPromotionXML,sItemPromotionXML,sizeof(sItemPromotionXML));

		if(iLen <= 0)
		{
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,"CRTIEventsHandler::ResultAddItem recived empty buffer.");
			_Module.m_server.m_cFuelLoayaltyPS.AddItemResult(lPumpNumber,NULL);
		}
		else
		{
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,"CRTIEventsHandler::ResultAddItem recevied AddItem successfully");
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

void CRTIEventsHandler::SetEventClientId(long lClientId)
{
	m_lEventsClientId = lClientId;	
}



HRESULT __stdcall CRTIEventsHandler::ResultTriggerProm(BSTR sXMLTriggerProm)
{
	
	_Module.m_server.m_cFuelLoayaltyPS.GetTriggerPromResult(sXMLTriggerProm);
	return S_OK;

}

HRESULT __stdcall CRTIEventsHandler::ResultTriggerProm2(long lPumpNumber, BSTR sXMLTriggerProm)
{

	_Module.m_server.m_cFuelLoayaltyPS.GetTriggerPromResult(sXMLTriggerProm, lPumpNumber);
	return S_OK;

}

HRESULT __stdcall CRTIEventsHandler::Error(BSTR sXMLError)
{
	_Module.m_server.m_cFuelLoayaltyPS.GetErrorFromRtapsAndContinue(sXMLError);
	return S_OK;

}

HRESULT __stdcall CRTIEventsHandler::ErrorAfterFuel ( BSTR sXMLError )
{

	_Module.m_server.m_cFuelLoayaltyPS.GetErrorAfterFuelAndContinue(sXMLError);
	return S_OK;
}

HRESULT __stdcall CRTIEventsHandler::QueryResult(BSTR sXMLGradeDiscount)
{
	
	_Module.m_server.m_cFuelLoayaltyPS.QueryResult(sXMLGradeDiscount);
	return S_OK;

}

HRESULT __stdcall CRTIEventsHandler::GeneralResult ( BSTR sXMLResult )
{

	_Module.m_server.m_cFuelLoayaltyPS.GeneralResult(sXMLResult);
	return S_OK;
}

//4.0.23.1220 155754
HRESULT __stdcall CRTIEventsHandler::PhoneLookUpResult (long lPumpNumber ,  int iRetCode , BSTR bstrRes )
{
	BYTE strClubCard[MAX_FIELD_VAL];
	int iLen = WideCharToMultiByte(CP_ACP,0,bstrRes ,-1,(char *) strClubCard,sizeof(strClubCard),NULL,NULL);

	_Module.m_server.m_cFuelLoayaltyPS.PhoneLookUpResult(lPumpNumber , iRetCode , strClubCard);

	return S_OK;
}
// CR 312183
HRESULT __stdcall CRTIEventsHandler::TargetMessageResult(long lPumpNumber, BSTR bstrTargetMessage )
{
	try
	{
		BYTE strTargetMessage[MAX_XML_BUF];
		int iLen = Bstr2Buffer(bstrTargetMessage,strTargetMessage,sizeof(strTargetMessage));

		if(iLen <= 0)
		{
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,"CRTIEventsHandler::TargetMessageResult Error while Extracting Target Message");
		}
		else
		{
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,"CRTIEventsHandler::TargetMessageResult Recevied Target Message Successfully");
			_Module.m_server.m_cFuelLoayaltyPS.TargetMessageResult(lPumpNumber , (char *) strTargetMessage);
		}
	}
	catch(_com_error & e )
	{
		_Module.m_server.m_cLogger.LogResult(0,"CRTIEventsHandler","TargetMessageResult",FALSE,0, TRUE,e.Error(),e);
	}
	catch(...)
	{
		_Module.m_server.m_cLogger.LogMsg("CRTIEventsHandler::TargetMessageResult Unexpected error has occurred!",LOG_LEVEL_1);
	}

	return S_OK;
}



HRESULT __stdcall CRTIEventsHandler::PromotionResult(long lPumpNumber ,  long lEarnValue, BSTR bstrRes )
{
	_Module.m_server.m_cFuelLoayaltyPS.PromotionResult(lPumpNumber , lEarnValue,bstrRes);
	return S_OK;
}

// CR 405435
HRESULT __stdcall CRTIEventsHandler::GetMemberInfoResult(long lPumpNumber, BSTR bstrRes)
{
	try
	{
		BYTE strMemberInfoResponse[MAX_XML_BUF] = { 0 };
		int iLen = Bstr2Buffer(bstrRes, strMemberInfoResponse, sizeof(strMemberInfoResponse));

		if (iLen <= 0)
		{
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, "CRTIEventsHandler::GetMemberInfoResult Error while Extracting GetMemberInfoResult Message");
			_Module.m_server.m_cFuelLoayaltyPS.HandleGetMemberInfoResult(lPumpNumber, NULL);
		}
		else
		{
			
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber, LOG_PUMP, "CRTIEventsHandler::GetMemberInfoResult Recevied GetMemberInfoResult Successfully");
			_Module.m_server.m_cFuelLoayaltyPS.HandleGetMemberInfoResult(lPumpNumber, (char *)strMemberInfoResponse);
		}

		
	}
	catch (_com_error & e)
	{
		_Module.m_server.m_cLogger.LogResult(0, "CRTIEventsHandler", "GetMemberInfoResult", FALSE, 0, TRUE, e.Error(), e);
	}
	catch (...)
	{
		_Module.m_server.m_cLogger.LogMsg("CRTIEventsHandler::GetMemberInfoResult Unexpected error has occurred!", LOG_LEVEL_1);
	}

	return S_OK;
}