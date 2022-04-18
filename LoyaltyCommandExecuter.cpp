// LoyaltyCommandExecuterHandler.cpp: implementation of the CLoyaltyCommandExecuterHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LoyaltyCommandExecuter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL bIsCriticalSection = FALSE;
CRITICAL_SECTION cRestorePumpSrv;
volatile long lIsRestoreInProgress = NULL;
DWORD lOwnerThreadId = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoyaltyCommandExecuter::CLoyaltyCommandExecuter()
{
	_pRTIEventsHandler = NULL;
	_pGLIEventsHandler = NULL;
}

CLoyaltyCommandExecuter::~CLoyaltyCommandExecuter()
{
}

BOOL CGLILoyaltyCommandExecuter::Query(long lPumpNumber,PAY_AT_PUMP_INFO & pInfo)	//4.0.22.504
{
	CString		strMsg;
	CComBSTR	bstrXml;
	CComBSTR	sAddItemXML;
	char		strAddItemXML[MAX_BUF] = {0}; //  4.0.23.980
	BOOL		bRetCode = TRUE;	//4.0.22.504
	PumpItems	cPumpItemsGrades;
	long		p = lPumpNumber - 1 ;
	BYTE		sTrsNumber[6]={0} ; //4.0.23.1310
	BOOL		bAddCarWashBeforeFueling = FALSE;
	CPumpTransact trs;	//4.0.19.508
	char		strXML[MAX_BUF] = {0}; //  4.0.23.980

	try
	{
		CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[p].GetStat();

		memset(sTrsNumber , 0 , sizeof(sTrsNumber));    //4.0.23.1310
		memcpy(sTrsNumber,pInfo.CardSaleInfo.cardData.sTranNbr,min(sizeof(sTrsNumber) , sizeof(pInfo.CardSaleInfo.cardData.sTranNbr)));	//4.0.19.500 55286  //4.0.23.1310
		

		cPumpItemsGrades.lCounter = 0;
		_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber,cPumpItemsGrades);
		if (a2l(pInfo.CardSaleInfo.extraData.sWashCode, sizeof(pInfo.CardSaleInfo.extraData.sWashCode)))	
			bAddCarWashBeforeFueling = TRUE;
		
		bRetCode =  _Module.m_server.m_cXmlParser.CreateQueryXML(lPumpNumber, &cPumpItemsGrades,strXML,pInfo, bAddCarWashBeforeFueling); //4.0.22.504 

		if(bRetCode)
		{
			bstrXml = strXML;
			(_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber))->UpdateCarWashTime(pInfo, lPumpNumber);	//4.0.20.10
			long lTrsNumber = a2l(sTrsNumber,strlen((char*)sTrsNumber));

			_Module.m_server.m_cFuelLoayaltyPS.SetTimer(p, QUERY_CHECK_TIMEOUT); // // TD 406362  4.0.23.2390

			bRetCode = m_pGLI->Query(bstrXml.m_str,lPumpNumber,0) == S_OK;
	
			strMsg.Format("CFuelLoyaltyPS::Query PumpNumber = %ld, TrsNumber = %ld", lPumpNumber, lTrsNumber);
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,strMsg);	
		}
		else
		{
			_Module.m_server.m_cLogger.LogMsg("CFuelLoyaltyPS","RunQuery",lPumpNumber,LOG_PUMP,"Failed Create Query XML");
		}
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"RunQuery");

		throw e;
	}
	
	catch(...)
	{
		_Module.m_server.m_cLogger.LogMsg("CFuelLoyaltyPS","RunQuery",lPumpNumber,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
		bRetCode = FALSE;
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		_Module.m_server.m_cLogger.LogMsg("CFuelLoyaltyPS","RunQuery",lPumpNumber,LOG_PUMP, "Query was finished");
	}
	

	return bRetCode;
}


BOOL CGLILoyaltyCommandExecuter::AddMemberInfo(long lPumpNumber,long lSegmentId)
{
	BOOL bRetCode;
	try
	{
		if(lSegmentId > 0)
		{
			char szXml[MAX_XML_BUF] = {0};
			
			_Module.m_server.m_cXmlParser.CreateAddMemberInfoXml(szXml);
			
			_bstr_t	bstrAddMemberInfoXml(szXml);

			_Module.m_server.m_cFuelLoayaltyPS.SetTimer(lPumpNumber-1, ADD_ADD_MEMBER_INFO_CHECK_TIMEOUT);

			m_pGLI->AddMemberInfo(bstrAddMemberInfoXml,lPumpNumber,0);

			_Module.m_server.m_cLogger.LogMsg("CFuelLoyaltyPS","AddMemberInfo",lPumpNumber,LOG_PUMP,"AddMmbrInfo was sent to loyaltFuel.");
		}
		bRetCode =  TRUE;
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"AddMemberInfo");
		throw e;
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CRTILoyaltyCommandExecuter","AddMemberInfo",0,LOG_PUMP,"Unexpected Error Had occurred",LOG_LEVEL_0);
		bRetCode = FALSE;
	}

	return bRetCode;
	
}

BOOL CRTILoyaltyCommandExecuter::AddMemberInfo(long lPumpNumber,long lSegmentId)
{
	try
	{
		if(lSegmentId > 0)
		{
			char szXml[MAX_XML_BUF] = {0};
			
			_Module.m_server.m_cXmlParser.CreateAddMemberInfoXml(szXml);
			
			_bstr_t	bstrAddMemberInfoXml(szXml);

			_Module.m_server.m_cFuelLoayaltyPS.SetTimer(lPumpNumber-1, ADD_ADD_MEMBER_INFO_CHECK_TIMEOUT);
			
			m_pRTI->AddMemberInfo(bstrAddMemberInfoXml,lPumpNumber,0);
			
			_Module.m_server.m_cLogger.LogMsg("CFuelLoyaltyPS","AddMemberInfo",lPumpNumber,LOG_PUMP,"AddMmbrInfo was sent to loyaltFuel.");
		}

		return TRUE;
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"AddMemberInfo");
		
		throw e;
	}	
	catch(...)
	{
		_LOGMSG.LogMsg("CRTILoyaltyCommandExecuter","AddMemberInfo",0,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
		return FALSE;
	}
}

BOOL CGLILoyaltyCommandExecuter::Init()
{
	BOOL bRetCode = TRUE;
	HRESULT hr;
	try
	{
		if(!bIsCriticalSection)
		{
			InitializeCriticalSection(&cRestorePumpSrv);  
			bIsCriticalSection = TRUE;
		}
		
		hr = m_pGLI.CreateInstance(CLSID_GLI);
		if ( FAILED(hr) ) 
		{
			_LOGMSG.LogMsg("CGLILoyaltyCommandExecuter","InitFuelLoyaltyPump",0,LOG_NONE,"failed to create instance of GLI");
			return FALSE;
		}
		
		hr = m_pGLI->InitInterface();
		if( FAILED(hr) )
		{
			return FALSE;
		}

		CreateEventHandler();
		
	}
	catch(_com_error & e )
	{
		HandleComException(0,e.Error(),"InitInterface");
		throw e;
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CGLILoyaltyCommandExecuter","InitInterface",0,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
		bRetCode = FALSE;
	}
	
	return bRetCode;
	
}

long CLoyaltyCommandExecuter::Restore_COM( HRESULT hResFailure )
{
	long lRetCode = 0;
	//TD 69813   
	CString strMsg;
    if(!lIsRestoreInProgress)
   	{
		
   	    lIsRestoreInProgress = 1;
        EnterCriticalSection(&cRestorePumpSrv);
		lOwnerThreadId = GetCurrentThreadId( );
		
		if((hResFailure == 0x800706ba)||(hResFailure == 0x800706bf))
		{
			DestuctLoyaltyPtr();
			lRetCode = _Module.m_server.m_cFuelLoayaltyPS.InitFuelLoyaltyInterface(_Module.m_server.GetLastValidPump());
		}
		
		strMsg.Format("Restore_COM: Thread: %ld, Connection To FuelLoyatyClient restored",lOwnerThreadId);
        _LOGMSG.LogMsg(strMsg);
		lOwnerThreadId = NULL;
		lIsRestoreInProgress = NULL;
		
		LeaveCriticalSection(&cRestorePumpSrv);
		
	}
	else
	{
		DWORD lThreadId = GetCurrentThreadId( );	
		strMsg.Format("Check_COM: Thread: %ld, Init Connection To PumpSrv already in progress.",lThreadId);
        _LOGMSG.LogMsg(strMsg);
		EnterCriticalSection(&cRestorePumpSrv);
		LeaveCriticalSection(&cRestorePumpSrv);	
	}	
	return lRetCode;
}


BOOL CLoyaltyCommandExecuter::HandleComException(long lPumpNumber , HRESULT hRes, char *szMsgFuncName)
{
	BOOL bRetCode = FALSE;
	if (FAILED(hRes))
	{
		Restore_COM( hRes );
		_LOGMSG.LogCOMError(lPumpNumber,szMsgFuncName,hRes,1);	
	}
	else
	{
		bRetCode = TRUE;
	}
	return bRetCode;
}

void CGLILoyaltyCommandExecuter::CreateEventHandler()
{
	_pGLIEventsHandler = new CGLIEventsHandler;

	IUnknownPtr pUnk= m_pGLI;
	HRESULT hRes = _pGLIEventsHandler->DispEventAdvise(pUnk);

	if ( FAILED(hRes) )
	{			
		_pGLIEventsHandler = NULL;
	}
	
}

long CGLILoyaltyCommandExecuter::StartTicketWithAddItem( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag, _bstr_t bstrXMLAddItemData )
{
	_Module.m_server.m_cFuelLoayaltyPS.SetTimer(lPumpNumber - 1, START_TICKET_CHECK_TIMEOUT);

	try
	{
		return m_pGLI->StartTicketWithAddItem(bstrXMLStuctGradesTrack, lPumpNumber, lTrsNumber, lFlag, bstrXMLAddItemData) == S_OK;
	}
	catch(_com_error & e )
	{

		HandleComException(lPumpNumber,e.Error(),"StartTicketWithAddItem");	
		throw e;
	}
}

long CGLILoyaltyCommandExecuter::StartTicket( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag )
{
	_Module.m_server.m_cFuelLoayaltyPS.SetTimer(lPumpNumber - 1, START_TICKET_CHECK_TIMEOUT);
	try 
	{
		return m_pGLI->StartTicket(bstrXMLStuctGradesTrack, lPumpNumber, lTrsNumber, lFlag) == S_OK;


		
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"StartTicket");
		throw e;
	}
	
}

long CGLILoyaltyCommandExecuter::InitPumps( _bstr_t bstrPumpsArr, long lFlag )
{
	try 
	{
		return m_pGLI->InitPumps(bstrPumpsArr,lFlag);
	}
	catch(_com_error & e )
	{
		HandleComException(0,e.Error(),"InitPumps");
		throw e;
	}
	
}



long CGLILoyaltyCommandExecuter::AddMemberCard(_bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lFlags, PAY_AT_PUMP_INFO & pInfo)
{
	_Module.m_server.m_cFuelLoayaltyPS.SetTimer(lPumpNumber - 1, ADD_MEMBER_CARD_CHECK_TIMEOUT);

	try 
	{
		return m_pGLI->AddMemberCard(bstrXMLStuctGradesTrack,lPumpNumber,lFlags);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"AddMemberCard");
		throw e;
	}
}

long CGLILoyaltyCommandExecuter::EndTicket( long lPumpNumber, long lFlag )
{
	_Module.m_server.m_cFuelLoayaltyPS.SetTimer(lPumpNumber - 1, END_TICKET_CHECK_TIMEOUT);
	try 
	{
		return m_pGLI->EndTicket(lPumpNumber,lFlag);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"EndTicket");
		throw e;
	}
	
	
}

long CGLILoyaltyCommandExecuter::CancelTicketWithAddItem( long lPumpNumber, long lFlag)
{
	try 
	{
		PAY_AT_PUMP_INFO  cTmpInfo;
		CPumpTransact trs;
		BOOL		bRetCode = FALSE;
		//char		strXML[MAX_BUF] = { 0 };
		CComBSTR strXML; // RFUEL-2067
		PumpItems	cPumpItemsGrades;
		_bstr_t	sXML;
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);
		bRetCode = _Module.m_server.m_cFuelLoayaltyPS.CreateAddItemXML(cTmpInfo,strXML, cPumpItemsGrades, trs, lPumpNumber); //RFUEL - 2067
		sXML = strXML;
		return m_pGLI->CancelTicketWithAddItem(lPumpNumber, lFlag, sXML);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"CancelTicketWithAddItem");
		throw e;
	}
	
	
}

long CGLILoyaltyCommandExecuter::CancelTicket( long lPumpNumber, long lFlag )
{	
	try {
		return m_pGLI->CancelTicket(lPumpNumber,lFlag);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"CancelTicket");

		throw e;
	}
}

long CGLILoyaltyCommandExecuter::AddItemFuelAndNonFuel( _bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, _bstr_t bstrXMLCarWashData )
{
	try 
	{
		return m_pGLI->AddItemFuelAndNonFuel(bstrXMLStuctData,lPumpNumber,lFlag,bstrXMLCarWashData);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"AddItemFuelAndNonFuel");
		throw e;
	}
	
}

long CGLILoyaltyCommandExecuter::AddItem(_bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, OLA_STAT & ola, long lTrsNum)
{
	try { 
		return m_pGLI->AddItem(bstrXMLStuctData,lPumpNumber,lFlag);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"AddItem");
		throw e;
	}	
}

long CGLILoyaltyCommandExecuter::PhoneLookUp( _bstr_t bstrXMLPhoneNumber, long lPumpNumber )
{
	try 
	{ 
		return m_pGLI->PhoneLookUp(bstrXMLPhoneNumber,lPumpNumber);	
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"PhoneLookUp");
		throw e;
	}
}

void CGLILoyaltyCommandExecuter::ReleaseLoyaltyInterface()
{
	if(m_pGLI)
	{
		try
		{
			DeleteCriticalSection(&cRestorePumpSrv);
			
			if(_pRTIEventsHandler)
			{
				IUnknownPtr pUnk = m_pGLI;
				_pRTIEventsHandler->DispEventUnadvise(pUnk);
				delete _pRTIEventsHandler;
			}
					
			m_pGLI->ReleaseFuelLoyalty();
			m_pGLI.Release();
			m_pGLI.Detach();
			m_pGLI = NULL;
		}
		catch(_com_error & e )
		{
			HandleComException(0,e.Error(),"ReleaseLoyaltyInterface");
			throw e;

		}
		catch(...)
		{
			_LOGMSG.LogMsg("CFuelLoyaltyPS","ReleaseGLIInterface",0,LOG_NONE,"Error !!!");
			m_pGLI = NULL;
		}
			
	}
}

void CGLILoyaltyCommandExecuter::DestuctLoyaltyPtr()
{
	if(m_pGLI)
		m_pGLI = NULL;
}

long CGLILoyaltyCommandExecuter::AddCarWashItem( long lPumpNumber,long lFlgas,_bstr_t bstrXMLPhoneNumber )
{
	try
	{
		;//return m_pGLI->AddCarWashItem(lPumpNumber,bstrCarWashItem);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"PhoneLookUp");
		throw e;
	}

	return TRUE;
}

long CGLILoyaltyCommandExecuter::SetParam( long lPumpNumber,long lTrsNumber,long lLanguageId, _bstr_t bstrXml )
{
	try
	{
		;//return m_pGLI->SetParam(lPumpNumber,bstrCarWashItem);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"SetParam");
		throw e;
	}

	return TRUE;
}

long CGLILoyaltyCommandExecuter::GetTargetMessage(long lPumpNumber)
{
	try
	{
		CComBSTR bstrXML(L"  ");
		return SUCCEEDED(m_pGLI->GetTargetMessage(lPumpNumber, bstrXML.m_str));
	}
	catch (_com_error & e)
	{
		HandleComException(lPumpNumber, e.Error(), "GetTargetMessage");
		throw e;
	}
}


// CR 405435
long CGLILoyaltyCommandExecuter::GetMemberInfo(long lPumpNumber, _bstr_t bstrXml)
{
	try
	{
		;//return m_pGLI->GetMemberInfo(lPumpNumber,bstrCarWashItem);
	}
	catch (_com_error & e)
	{
		HandleComException(lPumpNumber, e.Error(), "GetMemberInfo");
		throw e;
	}

	return TRUE;
}

/*******************************************************************************************************************
													RTI 
********************************************************************************************************************/

BOOL CRTILoyaltyCommandExecuter::Init()
{
	BOOL bRetCode = TRUE;
	HRESULT hr;
	try
	{
		if(!bIsCriticalSection)
		{
			InitializeCriticalSection(&cRestorePumpSrv);  
			bIsCriticalSection = TRUE;
		}
		
		hr = m_pRTI.CreateInstance(CLSID_RTI);
		if ( FAILED(hr) ) 
		{
			_LOGMSG.LogMsg("CRTILoyaltyCommandExecuter","InitFuelLoyaltyPump",0,LOG_NONE,"failed to create instance of RTI");
			return FALSE;
		}
		
		hr = m_pRTI->InitInterface();
		if( FAILED(hr) )
		{
			return FALSE;
		}
		
		CreateEventHandler();
	}
	catch(_com_error & e )
	{
		HandleComException(0,e.Error(),"Init");
		
		throw e;
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CRTILoyaltyCommandExecuter","InitInterface",0,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
		bRetCode = FALSE;
	}
	
	return bRetCode;
	
}

void CRTILoyaltyCommandExecuter::CreateEventHandler()
{
	_pRTIEventsHandler = new CRTIEventsHandler;
	
	IUnknownPtr pUnk = m_pRTI;
	HRESULT hRes = _pRTIEventsHandler->DispEventAdvise(pUnk);
	if ( FAILED(hRes) )
	{			
		_pRTIEventsHandler = NULL;
	}
	
}

void CRTILoyaltyCommandExecuter::ReleaseLoyaltyInterface()
{
	if(m_pRTI)
	{
		DeleteCriticalSection(&cRestorePumpSrv);
		
		try
		{
			if(_pRTIEventsHandler)
			{
				// clear event handler
				IUnknownPtr pUnk = m_pRTI;
				_pRTIEventsHandler->DispEventUnadvise(pUnk);
				delete _pRTIEventsHandler;	
			}
			
			// clear interface
			m_pRTI->ReleaseFuelLoyalty();
			m_pRTI.Release();
			m_pRTI.Detach();
			m_pRTI = NULL;
		}
		catch(...)
		{
			_LOGMSG.LogMsg("CRTILoyaltyCommandExecuter","ReleaseRTIInterface",0,LOG_NONE,"Error !!!");
			m_pRTI = NULL;
		}
		
	}	
}

BOOL CRTILoyaltyCommandExecuter::Query(long lPumpNumber,PAY_AT_PUMP_INFO & pInfo)	//4.0.22.504
{
	CString		strMsg;
	CComBSTR	bstrXml;
	CComBSTR	sAddItemXML;
	char		strAddItemXML[MAX_BUF] = {0}; //  4.0.23.980
	BOOL		bRetCode = TRUE;	//4.0.22.504
	PumpItems	cPumpItemsGrades;
	long		p = lPumpNumber - 1 ;
	BYTE		sTrsNumber[6]={0} ; //4.0.23.1310
	BOOL		bAddCarWashBeforeFueling = FALSE;
	CPumpTransact trs;	//4.0.19.508
	char		strXML[MAX_BUF] = {0}; //  4.0.23.980
	
	
	try
	{
		CPumpStatus cPumpStatus = _Module.m_server.m_cPumpArray[p].GetStat();
		
		memset(sTrsNumber , 0 , sizeof(sTrsNumber));    //4.0.23.1310
		memcpy(sTrsNumber,pInfo.CardSaleInfo.cardData.sTranNbr,min(sizeof(sTrsNumber) , sizeof(pInfo.CardSaleInfo.cardData.sTranNbr)));	//4.0.19.500 55286  //4.0.23.1310
		
		
		cPumpItemsGrades.lCounter = 0;
		_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber,cPumpItemsGrades);
		if (a2l(pInfo.CardSaleInfo.extraData.sWashCode, sizeof(pInfo.CardSaleInfo.extraData.sWashCode)))	
			bAddCarWashBeforeFueling = TRUE;
		
		bRetCode =  _Module.m_server.m_cXmlParser.CreateQueryXML(lPumpNumber, &cPumpItemsGrades,strXML,pInfo, bAddCarWashBeforeFueling); //4.0.22.504 
		
		if(bRetCode)
		{
			bstrXml = strXML;
			(_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber))->UpdateCarWashTime(pInfo, lPumpNumber);	//4.0.20.10
			long lTrsNumber = a2l(sTrsNumber,strlen((char*)sTrsNumber));

			_Module.m_server.m_cFuelLoayaltyPS.SetTimer(p, QUERY_CHECK_TIMEOUT); // // TD 406362  4.0.23.2390
			
			bRetCode = m_pRTI->Query(bstrXml.m_str,lPumpNumber,0) == S_OK; // RFUEL-1734
			
			strMsg.Format("CFuelLoyaltyPS::Query PumpNumber = %ld, TrsNumber = %ld, bRetCode = %d", lPumpNumber, lTrsNumber, bRetCode);
			_Module.m_server.m_cLogger.LogMsg(lPumpNumber,LOG_PUMP,strMsg);	
		}
		else
		{
			_Module.m_server.m_cLogger.LogMsg("CFuelLoyaltyPS","RunQuery",lPumpNumber,LOG_PUMP,"Failed Create Query XML");
		}
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"RunQuery");
		throw e;
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CFuelLoyaltyPS","RunQuery",lPumpNumber,LOG_PUMP,"Unexpected Error Had occurred", LOG_LEVEL_1); //  4.0.23.980
		bRetCode = FALSE;
	}
	
	if (LOG_DETAIL_FLOW_CONTROL)
	{
		_Module.m_server.m_cLogger.LogClassMsg("CFuelLoyaltyPS","RunQuery",lPumpNumber,LOG_PUMP, "Query was finished");
	}
	
	
	return bRetCode;
}


long CRTILoyaltyCommandExecuter::StartTicketWithAddItem( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag, _bstr_t bstrXMLAddItemData )
{
	_Module.m_server.m_cFuelLoayaltyPS.SetTimer(lPumpNumber - 1, START_TICKET_CHECK_TIMEOUT);
	try 
	{ 
		return m_pRTI->StartTicketWithAddItem(bstrXMLStuctGradesTrack,lPumpNumber,lTrsNumber,lFlag,bstrXMLAddItemData) == S_OK;
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"StartTicketWithAddItem");
		throw e;
	}
}

long CRTILoyaltyCommandExecuter::StartTicket( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lTrsNumber, long lFlag )
{
	_Module.m_server.m_cFuelLoayaltyPS.SetTimer(lPumpNumber - 1, START_TICKET_CHECK_TIMEOUT);
	try
	{
		return m_pRTI->StartTicket(bstrXMLStuctGradesTrack,lPumpNumber,lTrsNumber,lFlag) == S_OK;
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"StartTicket");

		throw e;
	}
	
}

long CRTILoyaltyCommandExecuter::InitPumps( _bstr_t bstrPumpsArr, long lFlag )
{
	try
	{
		return m_pRTI->InitPumps(bstrPumpsArr,lFlag);
	}
	catch(_com_error & e )
	{
		HandleComException(0,e.Error(),"InitPumps");
		throw e;
	}
	
}

long CRTILoyaltyCommandExecuter::AddMemberCard( _bstr_t bstrXMLStuctGradesTrack, long lPumpNumber, long lFlags, PAY_AT_PUMP_INFO & pInfo)
{
	_Module.m_server.m_cFuelLoayaltyPS.SetTimer(lPumpNumber - 1, ADD_MEMBER_CARD_CHECK_TIMEOUT);
	try
	{
		return SUCCEEDED(m_pRTI->AddMemberCard(bstrXMLStuctGradesTrack, lPumpNumber, lFlags));
	}
	catch (_com_error & e)
	{
		HandleComException(lPumpNumber, e.Error(), "AddMemberCard");
		throw e;
	}
}

long CRTILoyaltyCommandExecuter::EndTicket(long lPumpNumber, long lFlag)
{
	try 
	{	
		_Module.m_server.m_cFuelLoayaltyPS.SetTimer(lPumpNumber - 1, END_TICKET_CHECK_TIMEOUT);

		if (_Module.m_server.m_cFuelLoayaltyPS.m_dwUnlockLoyaltyMember)  // 4.0.23.2645 TD 425758
		{
			_bstr_t	bstrXML;
			char	strXML[MAX_BUF] = { 0 };
			PAY_AT_PUMP_INFO  cTmpInfo;
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);
			BOOL bRetCode = _Module.m_server.m_cFuelLoayaltyPS.CreateAddMemberCardXML(cTmpInfo, strXML);
			bstrXML = strXML;
			return SUCCEEDED(m_pRTI->EndTicket2(bstrXML, lPumpNumber, lFlag));
		}
		else
			return SUCCEEDED(m_pRTI->EndTicket(lPumpNumber, lFlag));
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"EndTicket");
		throw e;
	}	
}

long CRTILoyaltyCommandExecuter::CancelTicketWithAddItem( long lPumpNumber, long lFlag)
{
	try
	{
		PumpReservation cPumpRes = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetRes();
		if (_Module.m_server.m_cFuelLoayaltyPS.m_dwUnlockLoyaltyMember && cPumpRes == PAY_AT_PUMP) // 4.0.23.2645 TD 425758
		{
			_bstr_t	bstrXML;
			char	strXML[MAX_BUF] = { 0 };
			PAY_AT_PUMP_INFO  cTmpInfo;
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);
			BOOL bRetCode = _Module.m_server.m_cFuelLoayaltyPS.CreateAddMemberCardXML(cTmpInfo, strXML);
			bstrXML = strXML;
			return SUCCEEDED(m_pRTI->CancelTicket2(bstrXML, lPumpNumber, lFlag));
		}
		else
			return SUCCEEDED(m_pRTI->CancelTicket(lPumpNumber, lFlag));
	}
	catch (_com_error & e)
	{
		HandleComException(lPumpNumber, e.Error(), "CancelTicketWithAddItem");
		throw e;
	}
}

long CRTILoyaltyCommandExecuter::CancelTicket(long lPumpNumber, long lFlag)
{	
	try
	{
		PumpReservation cPumpRes = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat().GetRes();
		if (_Module.m_server.m_cFuelLoayaltyPS.m_dwUnlockLoyaltyMember && cPumpRes == PAY_AT_PUMP) // 4.0.23.2645 TD 425758
		{
			_bstr_t	bstrXML;
			char	strXML[MAX_BUF] = { 0 };
			PAY_AT_PUMP_INFO  cTmpInfo;
			_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);
			BOOL bRetCode = _Module.m_server.m_cFuelLoayaltyPS.CreateAddMemberCardXML(cTmpInfo, strXML);
			bstrXML = strXML;
			return SUCCEEDED(m_pRTI->CancelTicket2(bstrXML, lPumpNumber, lFlag));
		}
		else
			return SUCCEEDED(m_pRTI->CancelTicket(lPumpNumber, lFlag));

	}
	catch (_com_error & e)
	{
		HandleComException(lPumpNumber, e.Error(), "CancelTicket2");
		throw e;
	}
	
}

long CRTILoyaltyCommandExecuter::AddItemFuelAndNonFuel( _bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, _bstr_t bstrXMLCarWashData )
{
	
	try
	{
		return m_pRTI->AddItemFuelAndNonFuel(bstrXMLStuctData,lPumpNumber,lFlag,bstrXMLCarWashData);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"AddItemFuelAndNonFuel");
		throw e;
	}
	
}

long CRTILoyaltyCommandExecuter::AddItem(_bstr_t bstrXMLStuctData, long lPumpNumber, long lFlag, OLA_STAT & ola, long lTrsNum)
{
	char		strMsg[MAX_FIELD_VAL];
	CComBSTR	sXML, sCarWashXML;
	//char		strXML[MAX_BUF] = { 0 };
	CComBSTR    strXML; // RFUEL-2067
	char		strCarWashXML[MAX_BUF] = { 0 }; //  4.0.23.980
	BOOL		bRetCode = TRUE;	//4.0.22.504 69344
	PumpItems	cPumpItemsGrades;
	long		p = lPumpNumber - 1, lFullServicePrice = 0, lSelfServicePrice = 0, lTotalNoDiscount = 0, lPromID = 0,  lServiceMode = 0, lPriceLevel = 0;
	double fCentsOff = 0;
	PAY_AT_PUMP_INFO  cTmpInfo;
	CPumpTransact trs;

	EnterCriticalSection(_Module.m_server.m_csPumpArray);
	{
		_Module.m_server.m_cPumpArray[p].GetInProgressWaitProgressTrs(&trs);
		_Module.m_server.m_cPumpArray[p].GetPAPInfo(&cTmpInfo);
	}
	LeaveCriticalSection(_Module.m_server.m_csPumpArray);

	try
	{
		_Module.m_server.m_cOLASrv->GetProductForSinglePump(lPumpNumber, cPumpItemsGrades);

		_Module.m_server.m_cFuelLoayaltyPS.CalculateLoyaltyValues(cTmpInfo, lPumpNumber, trs);//4.0.20.300

		_Module.m_server.m_cFuelLoayaltyPS.GetPromotionId(trs.m_nGrade, p, &lPromID, &fCentsOff);
		l2a(lPromID, cTmpInfo.CardSaleExtraData2.sPromotionNumber, sizeof(cTmpInfo.CardSaleExtraData2.sPromotionNumber));
		_Module.m_server.m_cPumpArray[p].SetPAPInfo(&cTmpInfo);


		bRetCode = _Module.m_server.m_cFuelLoayaltyPS.CreateAddItemXML(cTmpInfo,strXML, cPumpItemsGrades, trs, lPumpNumber); // RFUEL-2067
		sXML.Empty(); //  4.0.23.980
		sXML = strXML;

		_Module.m_server.m_cFuelLoayaltyPS.SetTimer(p, ADD_ITEM_CHECK_TIMEOUT);

		long lCarWashOption = a2l(cTmpInfo.CardSaleExtraData4.sCarWashOption, sizeof(cTmpInfo.CardSaleExtraData4.sCarWashOption));
		if ((_Module.m_server.m_cFlowControlManager.GetPumpFlow(lPumpNumber)->IsCarWashAfterFueling()) && (lCarWashOption == CAR_WASH_AFTER_FUELING) && (a2l(cTmpInfo.CardSaleInfo.extraData.sWashCode, sizeof(cTmpInfo.CardSaleInfo.extraData.sWashCode))))	//Meaning we sold a car wash and need to add this item too //4.0.19.508
		{
			if (LOG_DETAIL_FLOW_CONTROL)
			{
				memset(strMsg, 0, sizeof(strMsg)); //4.0.20.56
				sprintf_s(strMsg, "Car wash details found and will be added to loyalty ticket, pump = %ld", lPumpNumber);
				_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);
			}
			bRetCode = _Module.m_server.m_cXmlParser.CreateAddCarWashItemXML(cTmpInfo, strCarWashXML, lPumpNumber);
			if (bRetCode)
			{
				sCarWashXML = strCarWashXML;
				bRetCode = AddItemFuelAndNonFuel(sXML.m_str, lPumpNumber, 0, sCarWashXML.m_str);
			}
			else
			{
				try
				{
					return SUCCEEDED(m_pRTI->AddItem(sXML.m_str, lPumpNumber, lFlag));
				}
				catch (_com_error & e)
				{
					HandleComException(lPumpNumber, e.Error(), "AddItem");
					throw e;
				}
			}
		}
		else
		{
			try
			{
				return SUCCEEDED(m_pRTI->AddItem(sXML.m_str, lPumpNumber, lFlag));
			}
			catch (_com_error & e)
			{
				HandleComException(lPumpNumber, e.Error(), "AddItem");
				throw e;
			}
		}
	}
	catch (...)
	{
		bRetCode = FALSE;
	}

	sprintf_s(strMsg, "CRTILoyaltyCommandExecuter::AddItem, A fuel item was added to loyalty ticket with result = %s", bRetCode ? "TRUE" : "FALSE");
	_LOGMSG.LogMsg(lPumpNumber, LOG_PUMP, strMsg);

	return bRetCode;
}

long CRTILoyaltyCommandExecuter::PhoneLookUp( _bstr_t bstrXMLPhoneNumber, long lPumpNumber )
{
	try
	{
		return m_pRTI->PhoneLookUp(bstrXMLPhoneNumber,lPumpNumber);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"PhoneLookUp");
		throw e;
	}
}

void CRTILoyaltyCommandExecuter::DestuctLoyaltyPtr()
{
	if(m_pRTI)
		m_pRTI = NULL;
	
}

long CRTILoyaltyCommandExecuter::AddCarWashItem( long lPumpNumber,long lCarWashSaleStatus,_bstr_t bstrCarWashItem)
{
	try
	{
		;// return m_pRTI->AddCarWashItem(lPumpNumber, lCarWashSaleStatus, bstrCarWashItem);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"AddCarWashItem");
		throw e;
	}

	return 0;
}

long CRTILoyaltyCommandExecuter::SetParam( long lPumpNumber,long lTrsNumber,long lLanguageId, _bstr_t bstrXml )
{
	try
	{
		//return m_pRTI->SetParam(lPumpNumber,lTrsNumber,lLanguageId,bstrXml);
	}
	catch(_com_error & e )
	{
		HandleComException(lPumpNumber,e.Error(),"SetParam");
		throw e;
	}

	return 0;
}

long CRTILoyaltyCommandExecuter::GetTargetMessage(long lPumpNumber)
{
	try
	{
		CComBSTR bstrXML(L"  ");
		return SUCCEEDED(m_pRTI->GetTargetMessage(lPumpNumber, bstrXML.m_str));
	}
	catch (_com_error & e)
	{
		HandleComException(lPumpNumber, e.Error(), "GetTargetMessage");
		throw e;
	}
}

// CR 405435
long CRTILoyaltyCommandExecuter::GetMemberInfo(long lPumpNumber, _bstr_t bstrXml)
{
	try
	{
		return m_pRTI->GetMemberInfo(bstrXml, lPumpNumber, 0);
	}
	catch (_com_error & e)
	{
		HandleComException(lPumpNumber, e.Error(), "GetMemberInfo");
		throw e;
	}

	return 0;
}

long CRTIStandAloneLoyaltyCommandExecuter::CancelTicket(long lPumpNumber, long lFlag)
{
	try
	{
		return SUCCEEDED(m_pRTI->CancelTicket(lPumpNumber, lFlag));
	}
	catch (_com_error & e)
	{
		HandleComException(lPumpNumber, e.Error(), "CancelTicket");
		throw e;
	}
}

long CRTIStandAloneLoyaltyCommandExecuter::CancelTicketWithAddItem(long lPumpNumber, long lFlag)
{
	try
	{
		PAY_AT_PUMP_INFO  cTmpInfo;
		CPumpTransact trs;
		BOOL		bRetCode = FALSE;
		//char		strXML[MAX_BUF] = { 0 };
		CComBSTR strXML; // RFUEL-2067
		PumpItems	cPumpItemsGrades;
		_bstr_t	sXML;
		_Module.m_server.m_cPumpArray[lPumpNumber - 1].GetPAPInfo(&cTmpInfo);
		bRetCode = _Module.m_server.m_cFuelLoayaltyPS.CreateAddItemXML(cTmpInfo,strXML, cPumpItemsGrades, trs, lPumpNumber); // RFUEL-2067
		sXML = strXML;
		return SUCCEEDED(m_pRTI->CancelTicketWithAddItem(lPumpNumber, lFlag, sXML));
	}
	catch (_com_error & e)
	{
		HandleComException(lPumpNumber, e.Error(), "CancelTicketWithAddItem");
		throw e;
	}
}
