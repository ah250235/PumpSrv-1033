// CopientCommandExecuter.cpp: implementation of the CCopientCommandExecuterHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CopientCommandExecuter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoyaltyCopientCommandExecuter::CLoyaltyCopientCommandExecuter()
{

}

CLoyaltyCopientCommandExecuter::~CLoyaltyCopientCommandExecuter()
{

}

BOOL CLoyaltyCopientCommandExecuter::Query(long lPumpNumber,PAY_AT_PUMP_INFO & pInfo)	//4.0.22.504
{
	char		strMsg[MAX_FIELD_VAL];
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
		if ((( cPumpStatus.GetRes() == PAY_AT_KIOSK ) || (cPumpStatus.GetResExt() == RES_EXT_PAK_LOYALTY)))
		{

			trs.m_lNumber	= _Module.m_server.NewTransactNumber();
			trs.m_lActiveReadNumber	= _Module.m_server.GetActiveReadNumber();

			if (cPumpStatus.GetResExt() == RES_EXT_PAK_LOYALTY) // RFS PAK //4.0.20.20
				trs.SetLink(TRS_LINK_OLA_NUMBER, _Module.m_server.m_cPumpArray[p].GetPAPSeqNumber());
			else	//Allied PAK
				trs.SetLink(TRS_NO_LINK,0);	

			trs.m_lActiveShiftNumber = _Module.m_server.m_cPumpArray[p].GetShiftNumber();

			trs.SetStatus(WAIT_PROGRESS);
			
			trs.m_lPump = lPumpNumber;

			_Module.m_server.CentralInsertTrs(p+1,&trs);

			l2a(trs.m_lNumber,sTrsNumber, sizeof(sTrsNumber));
		}
		else
		{
			memset(sTrsNumber , 0 , sizeof(sTrsNumber));    //4.0.23.1310
			memcpy(sTrsNumber,pInfo.CardSaleInfo.cardData.sTranNbr,min(sizeof(sTrsNumber) , sizeof(pInfo.CardSaleInfo.cardData.sTranNbr)));	//4.0.19.500 55286  //4.0.23.1310
		}

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

			_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"Start Session with Loyalty server");
			if (bAddCarWashBeforeFueling)
			{
				bRetCode =  _Module.m_server.m_cXmlParser.CreateAddCarWashItemXML(pInfo, strAddItemXML, lPumpNumber);
				if (bRetCode)
				{
					sAddItemXML = strAddItemXML;
					bRetCode = m_pGLI->StartTicketWithAddItem(bstrXml.m_str,lPumpNumber,lTrsNumber,0, sAddItemXML.m_str);
					_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"Start ticket with car wash item.");
				}
				else
				{
					bRetCode = m_pGLI->StartTicket(bstrXml.m_str,lPumpNumber,lTrsNumber,0);
					_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"Start ticket without car wash item.");
				}
			}
			else
			{
				bRetCode = m_pGLI->StartTicket(bstrXml.m_str,lPumpNumber,lTrsNumber,0);
				_LOGMSG.LogMsg("CFuelLoyaltyPS","StartTicket",lPumpNumber,LOG_PUMP,"Start ticket without car wash item.");
			}
					

			
			sprintf_s(strMsg, "CLoyaltyCopientCommandExecuter::Query PumpNumber = %ld, TrsNumber = %ld", lPumpNumber, lTrsNumber);
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,strMsg);
			
		}
		else
		{
			_LOGMSG.LogMsg("CLoyaltyCopientCommandExecuter","RunQuery",lPumpNumber,LOG_PUMP,"Failed Create Query XML");
		}
	}
	
	catch(...)
	{
		_LOGMSG.LogMsg("CLoyaltyCopientCommandExecuter","RunQuery",lPumpNumber,LOG_PUMP,"Unexpected Error Had occurred",LOG_LEVEL_0);
		bRetCode = FALSE;
	}

	if (LOG_DETAIL_FLOW_CONTROL)
	{
		sprintf_s(strMsg,"Query was finished");
		_LOGMSG.LogMsg("CLoyaltyCopientCommandExecuter","RunQuery",lPumpNumber,LOG_PUMP,strMsg);
	}
	

	return bRetCode;
}
