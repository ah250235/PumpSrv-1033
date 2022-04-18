
// EventManager.cpp: implementation of the CEventManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GPIEventManager.h"
#include "gpi.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPIEventManager::CGPIEventManager()
{
	InitializeCriticalSection(&m_csEventQueue);

}

CGPIEventManager::~CGPIEventManager()
{
	DeleteCriticalSection(&m_csEventQueue);

}


// Function name	: CEventManager::AddEvent
// Description	    : Adds an event to the events queue.
// Return type		: long 
// Argument         : long lDataId - event data id.
// Argument         : long lFlags - for future use.
long CGPIEventManager::AddEvent(Command lCommand, LONG lSrc, LONG lParam, LPVOID pData, SHORT nDataLen)
{
	long lRetVal = 0;
	CString	str;
	

	//adding avents to the queue only if there are connected event clients.
	if(_Module.m_server.m_GpiEventsObjMap.IsEmpty()== FALSE)
	{
		EnterCriticalSection(&m_csEventQueue);
		long lMsgNumber = _Module.m_server.NewMessageNumber();

		if(pData)
		{
			CCmdInfo	cCmd(lCommand,lSrc,lMsgNumber,lParam,pData,nDataLen);
			lRetVal = m_cEventsQueue.Set(&cCmd);	
		}
		else
		{
			
			CCmdInfo	cCmd(lCommand,lSrc,lMsgNumber,lParam);
			lRetVal = m_cEventsQueue.Set(&cCmd);		
		}
		LeaveCriticalSection(&m_csEventQueue);


	
		if(lRetVal == QUEUE_FULL)
		{
			if(LOG_DETAIL_FLOW_CONTROL)
			{
				str.Format("GPI Event Queue is Full.Event %d not added", lCommand); 
				_LOGMSG.LogMsg(str, LOG_LEVEL_1);					//4.0.26.503 TD 165971
			}
		}
		else
		{
			str.Format("AddEvent -  add new event %ld to GPI event",lCommand);
			_LOGMSG.LogMsg(str);
		}
	}



	return lRetVal;
}


// Function name	: CEventManager::FireCtrlEvent
// Description	    : Getting the event from the events queue and 
//					  fires it according to connected clients in the
//					  m_server.m_CtrlEventsObjMap.
// Return type		: long 
long CGPIEventManager::FireGPIEvent()
{

	HRESULT		hr;
	long		lRetVal;
	long		lKey;
	int			iNumOfElements = 0;
	POSITION	pos = _Module.m_server.m_GpiEventsObjMap.GetStartPosition();
	CGPI*		pCgpiObj = NULL;
	IGPIEvents *pGpiEventObj = NULL;
	CCmdInfo	Cmd;	
	CString		strCmdName,strLog;
		
	
	if(m_cEventsQueue.Empty())
		lRetVal = QUEUE_EMPTY;
	else
	{
		lRetVal = m_cEventsQueue.GetHead(&Cmd);

		if( lRetVal == QUEUE_NO_ERROR )
		{
			iNumOfElements = _Module.m_server.m_GpiEventsObjMap.GetCount();
					
			while (pos != NULL)
			{
				_Module.m_server.m_GpiEventsObjMap.GetNextAssoc(pos, lKey, pGpiEventObj );
				if (pGpiEventObj)
				{
					try
					{
						pCgpiObj = (CGPI*)pGpiEventObj;
						switch (Cmd.GetCommand())
						{
						case CMD_SET_PUMP_STATUS:
						{
							strCmdName = "SetPumpStatus";
							CMD_STRUCT_SET_PUMP_STATUS *pCmdInfo;
							pCmdInfo = (CMD_STRUCT_SET_PUMP_STATUS *)Cmd.GetDataPtr();
							hr = pCgpiObj->Fire_SetPumpStatus(pCmdInfo->lPumpNumber, pCmdInfo->lStatus, pCmdInfo->lRes, pCmdInfo->lResExt, pCmdInfo->lFlags, pCmdInfo->lParam1); //4.0.19.504 - 47559
							strLog.Format("FireGPIEvent -  fire SetPumpStatus to GPI clients");
							break;
						}

						case CMD_SET_PUMP_STATUS_EX: //CR 444830
						{
							strCmdName = "SetPumpStatusEx";
							CMD_STRUCT_SET_PUMP_STATUS_EX *pCmdInfo;
							pCmdInfo = (CMD_STRUCT_SET_PUMP_STATUS_EX *)Cmd.GetDataPtr();

							char sTmpAllocationBuffer[sizeof(SALE_EXTRA_DETAILS) + 2] = { 0 };
							memset(sTmpAllocationBuffer, 0, sizeof(sTmpAllocationBuffer));
							CComBSTR strData(sizeof(sTmpAllocationBuffer), (LPCSTR)sTmpAllocationBuffer);


							hr = pCgpiObj->Fire_SetPumpStatusEx(pCmdInfo->lPumpNumber, pCmdInfo->lStatus, pCmdInfo->lRes, pCmdInfo->lResExt, pCmdInfo->lFlags, pCmdInfo->lLimit, pCmdInfo->nGrade, pCmdInfo->nPosition, strData);
							strLog.Format("FireGPIEvent -  fire SetPumpStatusEx to GPI clients");
							break;
						}

						case CMD_SET_PAK_READY_TRS:
						{
							strCmdName = "PakReadyTrs";
							hr = S_OK;
							CMD_STRUCT_PAK_READY_TRS *pCmdInfo;
							pCmdInfo = (CMD_STRUCT_PAK_READY_TRS *)Cmd.GetDataPtr();

							//4.0.19.504 - 47559
							// 4.0.19.2040   TD 379709
							if (SERVER_NOT_IDLE && _Module.m_server.GetIFSFTerminalSrvInUse() != EPS_IFSF)	//Send the PAKReady only when the system is ready (keep it in the queue if not ready)
								m_cEventsQueue.Set(&Cmd);
							else
							{
								char sTmpAllocationBuffer[sizeof(SALE_EXTRA_DETAILS) + 2] = { 0 };
								memset(sTmpAllocationBuffer, 0, sizeof(sTmpAllocationBuffer));
								Str2StrNull((BYTE*)&pCmdInfo->cSaleExtraDetails, sizeof(pCmdInfo->cSaleExtraDetails), (BYTE*)sTmpAllocationBuffer);
								CComBSTR strData(sizeof(sTmpAllocationBuffer), (LPCSTR)sTmpAllocationBuffer);

								hr = pCgpiObj->Fire_PAKReadyTransaction(pCmdInfo->lPumpNumber, pCmdInfo->lTrsNumber,
									pCmdInfo->lNozzle, pCmdInfo->lVolume, pCmdInfo->lValue, pCmdInfo->lLevel, pCmdInfo->lGradePrice,
									pCmdInfo->lGrade, pCmdInfo->lRetCode, pCmdInfo->lFlags, strData);
								strLog.Format("FireGPIEvent -  fire PakReadyTrs to GPI clients");

								_Module.m_server.m_cLogger.LogMsg(strLog);
							}
							break;
						}
						case CMD_SET_PUMP_MODE3:
						{
							strCmdName = "SetPumpMode";
							CMD_STRUCT_MODE3 *pCmdInfo;
							pCmdInfo = (CMD_STRUCT_MODE3 *)Cmd.GetDataPtr();
							hr = pCgpiObj->Fire_SetPumpMode(Cmd.GetSrc(), pCmdInfo->lMode, pCmdInfo->lServiceMode, pCmdInfo->lServiceFee, pCmdInfo->lPriceLevel, pCmdInfo->lFlags);
							strLog.Format("FireGPIEvent -  fire SetPumpMode to GPI clients");
							break;

						}
						//4.0.12.503
						case CMD_SWIPE_RESULT:
						{
							hr = S_OK;
							strCmdName = "SetPAPSwipeResult";
							CMD_STRUCT_SEND_PAP_SWIPE_RESULT* pCmdInfo;
							pCmdInfo = (CMD_STRUCT_SEND_PAP_SWIPE_RESULT *)Cmd.GetDataPtr();

							char sTmpAllocationBuffer[sizeof(CARD_SALE_ALL3) + 2] = { 0 };
							memset(sTmpAllocationBuffer, 0, sizeof(sTmpAllocationBuffer));
							Str2StrNull((BYTE*)&pCmdInfo->cardDataAll3, sizeof(pCmdInfo->cardDataAll3), (BYTE*)sTmpAllocationBuffer);

							CComBSTR strData(sizeof(sTmpAllocationBuffer), (LPCSTR)sTmpAllocationBuffer);

							long lPosNumber = pCgpiObj->GetPosNumber();
							long lPumpNumber = Cmd.GetParameter();

							BOOL bSendToClient = FALSE;

							if (lPumpNumber > 0)
							{
								CPumpStatus stat = _Module.m_server.m_cPumpArray[lPumpNumber - 1].GetStat();

								bSendToClient = pCgpiObj->GetPosNumber() == Cmd.GetSrc();

// 								if (stat.GetResExt() == RES_EXT_MOBILE)
// 									bSendToClient = pCgpiObj->GetPosNumber() == MOBILE_SRV_POS_NUMBER;
// 								else
// 									bSendToClient = pCgpiObj->GetPosNumber() == TERMINAL_SRV_POS_NUMBER;

							}
							else
								strLog.Format("FireGPIEvent -  Error on PAPSwipeResult. PumpNumber is ZERO.");



							if (TRUE == bSendToClient)
							{
								hr = pCgpiObj->Fire_PAPSwipeResult(Cmd.GetParameter(), pCmdInfo->lDeviceNumber, pCmdInfo->lSeqNumber, pCmdInfo->lRetcode, pCmdInfo->lFlags, strData);
								strLog.Format("FireGPIEvent -  fire PAPSwipeResult Pump(%ld) to GPI client (%ld)", lPumpNumber, pCgpiObj->GetPosNumber());
							}
							else
								strLog.Format("FireGPIEvent -  ignore send PAPSwipeResult Pump(%ld) to GPI client (%ld)", lPumpNumber, pCgpiObj->GetPosNumber());

							_Module.m_server.m_cLogger.LogMsg(strLog);
							break;
						}
						case CMD_SEND_CANCEL_TRS:	//4.0.19.504 - 47559
						{
							strCmdName = "CancelTrs";
							CMD_STRUCT_SEND_CANCEL_TRS* pCmdInfo;
							pCmdInfo = (CMD_STRUCT_SEND_CANCEL_TRS *)Cmd.GetDataPtr();
							hr = pCgpiObj->Fire_CancelTrs(Cmd.GetParameter(), pCmdInfo->lTrsNumber, pCmdInfo->lPumpStatus, pCmdInfo->lRes, pCmdInfo->lResExt);

							strLog.Format("FireGPIEvent -  fire CancelTrs Pump(%ld) to GPI client (%ld)", pCmdInfo->lPumpNumber, pCgpiObj->GetPosNumber());
							_Module.m_server.m_cLogger.LogMsg(strLog);
							break;

						}
						case CMD_SEND_GET_RECEIPTS_ROW_DATA: //4.0.19.504 - 47559
						{

							long lRecords = 0;
							strCmdName = "Get Receipt details";
							CComBSTR sArrayBuffer(" ");
							CMD_STRUCT_GET_RECEIPTS_ROW_DATA *pCmdInfo;

							pCmdInfo = (CMD_STRUCT_GET_RECEIPTS_ROW_DATA *)Cmd.GetDataPtr();

							if (pCmdInfo->lPumpNumber > 0)
								_Module.m_server.m_cMemPumpArray[pCmdInfo->lPumpNumber - 1].CleanList();
							else
							{
								for (int p = 0; p < _Module.m_server.GetLastValidPump(); p++)
								{
									_Module.m_server.m_cMemPumpArray[p].CleanList();
								}
							}

							char sTmpAllocationBuffer[sizeof(pCmdInfo->sBuffer) + 2] = { 0 };
							memset(sTmpAllocationBuffer, 0, sizeof(sTmpAllocationBuffer));

							Str2StrNull((BYTE*)pCmdInfo->sBuffer, pCmdInfo->lBuffSize, (BYTE*)sTmpAllocationBuffer);
							CComBSTR strTrack2(sizeof(sTmpAllocationBuffer), (LPCSTR)sTmpAllocationBuffer);

							sArrayBuffer.Empty();

							hr = pCgpiObj->Fire_GetReceiptTransactionDetails(Cmd.GetParameter(), strTrack2.Copy(), pCmdInfo->lFlag, &sArrayBuffer, &lRecords);

							strLog.Format("FireGPIEvent -  Get receipt row data");
							_LOGMSG.LogMsg(strLog);

							if (SUCCEEDED(hr))
							{
								//Liat
								if (pCmdInfo->lFlag & GET_RECEIPT_TRS_BY_TRACK2)
								{
									_Module.m_server.ParseReceiptRowDataArrayAsOLARec(sArrayBuffer.m_str, lRecords, pCmdInfo->lPosNumber);

								}
								else
									_Module.m_server.ParseReceiptRowDataArray(sArrayBuffer.m_str, lRecords, pCmdInfo->lPosNumber);
							}

							if (LOG_DETAIL_FLOW_CONTROL)
							{
								strLog.Format("Pump %02ld , GetTransactionList found %ld records, Result=%ld", pCmdInfo->lPumpNumber, lRecords, hr);
								_LOGMSG.LogMsg(strLog);
							}


							break;
						}
						case CMD_GET_FORMATED_RECEIPT_DATA: //4.0.19.504 - 47559
						{
							long lRecords = 0;
							strCmdName = "Get Formated receipt";
							CComBSTR bstrReceiptData("");
							CMD_STRUCT_GET_FORMATED_RECEIPT_DATA *pCmdInfo;

							pCmdInfo = (CMD_STRUCT_GET_FORMATED_RECEIPT_DATA *)Cmd.GetDataPtr();

							CComBSTR strInvoiceNumber(sizeof(pCmdInfo->sInvoiceNumber), (LPCSTR)pCmdInfo->sInvoiceNumber);
							BSTR bstrInvoiceNumber = strInvoiceNumber.Copy();

							hr = pCgpiObj->Fire_GetReceipt(pCmdInfo->lPumpNumber, bstrInvoiceNumber, &bstrReceiptData.m_str);

							strLog.Format("FireGPIEvent - Format receipt data");
							_LOGMSG.LogMsg(strLog);

							if (SUCCEEDED(hr))
							{
								_Module.m_server.SendFormatReceiptData(pCmdInfo->lPosNumber, bstrReceiptData);
							}
							break;
						}
						case CMD_SEND_WASH_CODE_READY:  //4.0.19.504 - 47559
						{
							strCmdName = "Car Wash Code ready";
							CMD_STRUCT_SEND_WASH_CODE_READY* pCmdInfo;
							pCmdInfo = (CMD_STRUCT_SEND_WASH_CODE_READY *)Cmd.GetDataPtr();
							hr = pCgpiObj->Fire_WashCodeReady(pCmdInfo->lDevice, pCmdInfo->lSyncNumber, pCmdInfo->lFlags);
							strLog.Format("FireGPIEvent -  fire WashCodeReady to GPI clients");
							break;
						}
						case CMD_RECONFIGURE_EVENT: //4.0.19.504 - 47559
						{
							strCmdName = "Reconfigure";
							CMD_STRUCT_RECONFIGURATION_EVENT* pCmdInfo;
							pCmdInfo = (CMD_STRUCT_RECONFIGURATION_EVENT *)Cmd.GetDataPtr();
							hr = pCgpiObj->Fire_Reconfigure(pCmdInfo->lTableId, pCmdInfo->lSubTableId, pCmdInfo->lSubTableIdExt, pCmdInfo->lFlags);
							strLog.Format("FireGPIEvent -  fire Reconfigure event to GPI clients");
							break;
						}
						//CR 442586
						case CMD_SMART_RECONFIGURE_EVENT:
						{
							strCmdName = "SmartReconfigure from FireGPIEvent";
							CMD_STRUCT_SMART_RECONFIGURATION_EVENT* pCmdInfo;
							pCmdInfo = (CMD_STRUCT_SMART_RECONFIGURATION_EVENT *)Cmd.GetDataPtr();
							hr = pCgpiObj->Fire_SmartReconfigure(pCmdInfo->lDeviceNumberMask1, pCmdInfo->lDeviceNumberMask2, pCmdInfo->lTableId, pCmdInfo->lSubTableId, pCmdInfo->lSubTableIdExt, pCmdInfo->lFlags);
							strLog.Format("FireGPIEvent -  fire Reconfigure 3 event to GPI clients");
							break;
						}
						//END CR 442586
						case CMD_SEND_SET_PRESET_LIMIT: //4.0.19.504 - 47559
						{
							strCmdName = "Set Preset limit";
							CMD_STRUCT_SET_PRESET_LIMIT_EVENT* pCmdInfo;
							pCmdInfo = (CMD_STRUCT_SET_PRESET_LIMIT_EVENT *)Cmd.GetDataPtr();
							hr = pCgpiObj->Fire_SetPreSetLimit(pCmdInfo->lPumpNumber, pCmdInfo->lPresetLimitType, pCmdInfo->lPresetLimitValue);
							strLog.Format("FireGPIEvent -  fire SetPresetLimit event to GPI clients");
							break;
						}
						case CMD_GENERAL_SYSTEM_EVENT: //4.0.19.504 - 47559
						{
							strCmdName = "General sytem event";
							CMD_STRUCT_GENERAL_SYSTEM_EVENT* pCmdInfo;
							pCmdInfo = (CMD_STRUCT_GENERAL_SYSTEM_EVENT *)Cmd.GetDataPtr();
							hr = pCgpiObj->Fire_GeneralSystemEvent(pCmdInfo->lEventId, pCmdInfo->lFlags, pCmdInfo->lParam1);
							strLog.Format("FireGPIEvent -  fire General system event to GPI clients");
							break;
						}

						case CMD_TRS_STATE_CHANGED:		//?.??.?.?? - TD 335186
						{
							strCmdName = "Trs state changed event";
							CMD_STRUCT_TRS_STATE_CHANGED* pCmdInfo;
							pCmdInfo = (CMD_STRUCT_TRS_STATE_CHANGED *)Cmd.GetDataPtr();
							hr = pCgpiObj->Fire_TrsStateChangedEvent(pCmdInfo->lPumpNumber, pCmdInfo->lTrsNumber, pCmdInfo->lState, pCmdInfo->lFlags);
							strLog.Format("FireGPIEvent -  fire Trs state changed event to GPI clients");
							break;
						}

						case CMD_SET_PROMOTIONS_EVENT: // 4.0.19.1750 TD  349356
						{
							strCmdName = "SetPromotions event";
							CMD_STRUCT_SET_PROMOTIONS_EVENT *pCmdInfo;
							pCmdInfo = (CMD_STRUCT_SET_PROMOTIONS_EVENT *)Cmd.GetDataPtr();
							hr = pCgpiObj->Fire_SetPromotionsEvent(pCmdInfo->lPumpNumber, pCmdInfo->lFlags);
							strLog.Format("FireGPIEvent -  fire PromotionsDetails event to GPI clients");
							break;



						}

						}


						if (LOG_DETAIL_FLOW_CONTROL) //  4.0.19.2040   TD 379709
							_LOGMSG.LogMsg(strLog);

						
							long lClientNumber = 0;
							if (pCgpiObj != NULL)
								lClientNumber = pCgpiObj->GetPosNumber();

							HandleGPIEventResult(hr, lClientNumber, Cmd); //  4.0.19.2040   TD 379709


						
				

					}catch(...)
					{
						strLog.Format("FireGPIEvent unexpected error!!!!");
						_LOGMSG.LogMsg(strLog,LOG_LEVEL_1);

					}
				}
			}
		}
						
	}//end of else QUEUE_EMPTY;

	return lRetVal;
}


void CGPIEventManager::AddPakReadyTrs(long lPumpNumber, long lTrsNumber, long nNozzle, long lVolume, long lValue, long nLevel, long lGradePrice, long nGrade, long lFlags, long lTotalVal, long lResExt, SALE_EXTRA_DETAILS cSaleExtraDetails, long lRetCode) //4.0.19.504 - 47559
{
	CMD_STRUCT_PAK_READY_TRS cmdInfo;

	memset(&cmdInfo, 0,sizeof(cmdInfo));
	cmdInfo.lFlags=  lFlags;
	cmdInfo.lGradePrice = lGradePrice;
	cmdInfo.lPumpNumber = lPumpNumber;
	cmdInfo.lRetCode = lRetCode;
	cmdInfo.lTotalVal = lTotalVal;
	cmdInfo.lTrsNumber = lTrsNumber;
	cmdInfo.lValue = lValue;
	cmdInfo.lVolume = lVolume;
	cmdInfo.lGrade = nGrade;
	cmdInfo.lLevel = nLevel;
	cmdInfo.lNozzle = nNozzle;
	cmdInfo.lResExt = lResExt;
	cmdInfo.cSaleExtraDetails = cSaleExtraDetails;
	
	AddEvent(CMD_SET_PAK_READY_TRS, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_PAK_READY_TRS));

}


void CGPIEventManager::AddSetPumpStatus(long lPumpNumber, long lStatus, long lRes, long lResExt,long lFlags, long lParam1) //4.0.19.504 - 47559
{
	CMD_STRUCT_SET_PUMP_STATUS cmdInfo;
	memset(&cmdInfo, 0,sizeof(cmdInfo));
	cmdInfo.lRes = lRes;
	cmdInfo.lStatus = lStatus;
	cmdInfo.lResExt = lResExt; 
	cmdInfo.lPumpNumber = lPumpNumber;
	cmdInfo.lFlags = lFlags;
	cmdInfo.lParam1 = lParam1;

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;
		str.Format("Pump %02ld state %ld res %ld",lPumpNumber, lStatus ,lRes);
		_LOGMSG.LogMsg("CGPIEventManager","SetPumpStatus",lPumpNumber,LOG_PUMP, str);
	}
		
	AddEvent(CMD_SET_PUMP_STATUS, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_SET_PUMP_STATUS));
}


void CGPIEventManager::AddSetPumpStatusEx(long lPumpNumber, long lStatus, long lRes, long lResExt, long lFlags, long lLimit, short nGrade, short nPosition) //CR 444830
{
	CMD_STRUCT_SET_PUMP_STATUS_EX cmdInfo;
	memset(&cmdInfo, 0, sizeof(cmdInfo));
	cmdInfo.lRes = lRes;
	cmdInfo.lStatus = lStatus;
	cmdInfo.lResExt = lResExt;
	cmdInfo.lPumpNumber = lPumpNumber;
	cmdInfo.lFlags = lFlags;
	cmdInfo.nGrade = nGrade;
	cmdInfo.nPosition = nPosition;
	cmdInfo.lLimit = lLimit;

	if (LOG_BASIC_FLOW_CONTROL)
	{
		// 		CString str;
		// 		str.Format("Pump %02ld state %ld res %ld",lPumpNumber, lStatus ,lRes);

		CString str;
		str.Format("CGPIEventManager::AddSetPumpStatusEx: status %ld res %ld", lStatus, lRes); // 4.0.22.900   TD 321585
		_Module.m_server.m_cLogger.LogMsg("CGPIEventManager", "SetPumpStatusEx", lPumpNumber, LOG_PUMP, str);
	}

	AddEvent(CMD_SET_PUMP_STATUS_EX, SERVER_POS_NUMBER, lPumpNumber, &cmdInfo, sizeof(CMD_STRUCT_SET_PUMP_STATUS_EX));
}


void CGPIEventManager::AddSetPumpMode(long lPumpNumber,long lMode,long lServiceMode,long lServiceFee,long lPriceLevel,long lFlags)
{
	CMD_STRUCT_MODE3 cmdInfo;
	memset(&cmdInfo, 0, sizeof(CMD_STRUCT_MODE3) );
	cmdInfo.lMode = lMode;
	cmdInfo.lServiceMode = lServiceMode;
	cmdInfo.lServiceFee =lServiceFee;
	cmdInfo.lPriceLevel = lPriceLevel;
	cmdInfo.lFlags;

	AddEvent(CMD_SET_PUMP_MODE3, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_MODE3));						
}

//4.0.12.503
void CGPIEventManager::AddPAPSwipeResult(long lPumpNumber,long lDeviceNumber,long lSeqNumber,long lRetcode,long lFlags,BSTR sData, long lPosNumber)
{

	CMD_STRUCT_SEND_PAP_SWIPE_RESULT cmdInfo;

	memset(&cmdInfo, 0, sizeof(CMD_STRUCT_SEND_PAP_SWIPE_RESULT) );

	CARD_SALE_ALL3 cardDataAll3;
	memset(&cardDataAll3,0,sizeof(CARD_SALE_ALL3));
	int iLen = Bstr2Buffer(sData, (void *)&cardDataAll3,sizeof(CARD_SALE_ALL3));

	cmdInfo.lDeviceNumber = lDeviceNumber;
	cmdInfo.lSeqNumber = lSeqNumber;
	cmdInfo.lRetcode = lRetcode;
	cmdInfo.lFlags = lFlags;
	cmdInfo.cardDataAll3 = cardDataAll3;

	AddEvent(CMD_SWIPE_RESULT, lPosNumber, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_SEND_PAP_SWIPE_RESULT));						
}


//4.0.19.504 - 47559
void CGPIEventManager::AddCancelTrs(long lPumpNumber, LONG lTrsNumber, LONG lPumpStatus, LONG lRes, LONG lResExt)
{

	CMD_STRUCT_SEND_CANCEL_TRS  cmdInfo;

	memset(&cmdInfo, 0, sizeof(CMD_STRUCT_SEND_CANCEL_TRS) );


	cmdInfo.lPumpNumber = lPumpNumber;
	cmdInfo.lTrsNumber = lTrsNumber;
	cmdInfo.lPumpStatus = lPumpStatus;
	cmdInfo.lRes = lRes;
	cmdInfo.lResExt = lResExt;

	AddEvent(CMD_SEND_CANCEL_TRS, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_SEND_CANCEL_TRS));						
}

//4.0.19.504 - 47559
void CGPIEventManager::AddRequestGetTransactionList(long &lPumpNumber, long &lPosNumber, long lFlag, char * szTrack2, long lenTrack2)
{
	CMD_STRUCT_GET_RECEIPTS_ROW_DATA cmdInfo;

	memset(&cmdInfo , 0 , sizeof(cmdInfo));
	
	//cmdInfo.sBuffer[0]= ' '; 
	cmdInfo.lPumpNumber =  lPumpNumber;
	cmdInfo.lPosNumber = lPosNumber;
	cmdInfo.lFlag = lFlag;
	
	if (szTrack2 != NULL)
	{
		memcpy(cmdInfo.sBuffer, szTrack2, min(lenTrack2, sizeof(cmdInfo.sBuffer))); // CID_199328 Out-of-bounds access
		cmdInfo.lBuffSize = TRACK_2_DATA_SIZE; 
	}
	else
		cmdInfo.lBuffSize = 0; 

	AddEvent(CMD_SEND_GET_RECEIPTS_ROW_DATA, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_GET_RECEIPTS_ROW_DATA));							

}

//4.0.19.504 - 47559
void CGPIEventManager::AddRequest4FormatReceiptData(long &lPumpNumber, long &lPosNumber, BYTE * sInvoiceNumber) //long &lInvoiceNumber
{
	CMD_STRUCT_GET_FORMATED_RECEIPT_DATA cmdInfo;

	memset(&cmdInfo , 0 , sizeof(cmdInfo));
	cmdInfo.lPumpNumber =  lPumpNumber;
	cmdInfo.lPosNumber = lPosNumber;
	//cmdInfo.lInvoiceNumber = lInvoiceNumber;
	memcpy(cmdInfo.sInvoiceNumber, sInvoiceNumber, sizeof(cmdInfo.sInvoiceNumber));

	AddEvent(CMD_GET_FORMATED_RECEIPT_DATA, SERVER_POS_NUMBER, lPumpNumber,&cmdInfo,sizeof(CMD_STRUCT_GET_FORMATED_RECEIPT_DATA));							
}


//4.0.19.504 - 47559
/******************************************************************************
 Description: Send the Wash Code ready GPI event .
 Return:  NONE
				
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			25/11/2007				Start
******************************************************************************/
void CGPIEventManager::AddWashCodeReady(const long lDevice, const long lSyncNumber,const long lFlags) //4.0.140.14
{

	CMD_STRUCT_SEND_WASH_CODE_READY  cmdInfo;

	memset(&cmdInfo, 0, sizeof(CMD_STRUCT_SEND_WASH_CODE_READY) );

	cmdInfo.lDevice = lDevice;
	cmdInfo.lSyncNumber = lSyncNumber;
	cmdInfo.lFlags = lFlags;


	AddEvent(CMD_SEND_WASH_CODE_READY, SERVER_POS_NUMBER, 0,&cmdInfo,sizeof(CMD_STRUCT_SEND_WASH_CODE_READY));						
}

//4.0.19.504 - 47559
/******************************************************************************
 Description:	Send Reconfigure event to GPI event
 Return:		NONE
				
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			13/12/2007				Start
******************************************************************************/
void CGPIEventManager::AddReconfigure(long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags) //4.0.140.18
{

	CMD_STRUCT_RECONFIGURATION_EVENT  cmdInfo;

	memset(&cmdInfo, 0, sizeof(CMD_STRUCT_RECONFIGURATION_EVENT) );

	cmdInfo.lTableId = lTableId;
	cmdInfo.lSubTableId = lSubTableId;
	cmdInfo.lSubTableIdExt = lSubTableIdExt;
	cmdInfo.lFlags = lFlags;


	AddEvent(CMD_RECONFIGURE_EVENT, SERVER_POS_NUMBER, 0, &cmdInfo, sizeof(CMD_STRUCT_RECONFIGURATION_EVENT));						
}

//4.0.19.504 - 47559
/******************************************************************************
 Description:	Send Reconfigure event to GPI event
 Return:		NONE
				
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			25/12/2007				Start
******************************************************************************/
void CGPIEventManager::AddSetPreSetLimit(long lPumpNumber, long lPresetLimitType, long lPresetLimitValue) //4.0.140.20
{

	CMD_STRUCT_SET_PRESET_LIMIT_EVENT  cmdInfo;

	memset(&cmdInfo, 0, sizeof(CMD_STRUCT_SET_PRESET_LIMIT_EVENT) );

	cmdInfo.lPumpNumber = lPumpNumber;
	cmdInfo.lPresetLimitType = lPresetLimitType;
	cmdInfo.lPresetLimitValue = lPresetLimitValue;
	

	AddEvent(CMD_SEND_SET_PRESET_LIMIT, SERVER_POS_NUMBER, 0, &cmdInfo, sizeof(CMD_STRUCT_SET_PRESET_LIMIT_EVENT));						
}


/******************************************************************************
 Description:	Send general system event to GPI event
 Return:		NONE
				
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			26/12/2007				Start
******************************************************************************/
void CGPIEventManager::AddGeneralSystemEvent(long lEventId, long lFlags, long lParam1) //4.0.140.20
{

	CMD_STRUCT_GENERAL_SYSTEM_EVENT  cmdInfo;

	memset(&cmdInfo, 0, sizeof(CMD_STRUCT_GENERAL_SYSTEM_EVENT) );

	cmdInfo.lEventId = lEventId;
	cmdInfo.lFlags = lFlags;
	cmdInfo.lParam1 = lParam1;
	

	AddEvent(CMD_GENERAL_SYSTEM_EVENT, SERVER_POS_NUMBER, 0, &cmdInfo, sizeof(CMD_STRUCT_GENERAL_SYSTEM_EVENT));						
}

//?.??.?.?? - TD 335186
void CGPIEventManager::AddTrsStateChangedEvent(long lPumpNumber, long lTrsNumber, long lState, long lFlags)
{
	CMD_STRUCT_TRS_STATE_CHANGED cmdInfo;
	
	memset(&cmdInfo , 0 , sizeof(cmdInfo));
	
	cmdInfo.lPumpNumber =  lPumpNumber;
	cmdInfo.lTrsNumber = lTrsNumber;
	cmdInfo.lState = lState;
	cmdInfo.lFlags = lFlags;
	
	AddEvent(CMD_TRS_STATE_CHANGED, SERVER_POS_NUMBER, 0, &cmdInfo, sizeof(CMD_STRUCT_TRS_STATE_CHANGED));							
	
}

/******************************************************************************
 Description:	Handle fire event to GPI
 Return:		NONE
				
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 OmerT			17/11/2013				Start		 4.0.19.2040   TD 379709
******************************************************************************/
void CGPIEventManager::HandleGPIEventResult(HRESULT hr, long lClientNumber, CCmdInfo & Cmd)
{
	
	if(hr != S_OK)
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	strLog;
			strLog.Format("FireGPIEvent HRESULT: %d; Command: %d, GpiClient: %d", hr, Cmd.GetCommand(), lClientNumber);
			_Module.m_server.m_cLogger.LogMsg(strLog);
		}
	}
	
	if (SUCCEEDED(hr))
	{
		m_cEventsQueue.Remove(&Cmd);
	}
	else
	{
		_Module.m_server.CloseConnectToIFSFTerminalSrv();
		_Module.m_server.CreateConnectToIFSFTerminalSrv();
		_Module.m_server.CloseConnectToFuelMobileSrv();// CR 474446 
		_Module.m_server.CreateConnectToFuelMobileSrv();// CR 474446 
	}
}

///CR 442586
void CGPIEventManager::AddSmartReconfigure(long lDeviceNumberMask1, long lDeviceNumberMask2, long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags)
{

	CMD_STRUCT_SMART_RECONFIGURATION_EVENT  cmdInfo;

	memset(&cmdInfo, 0, sizeof(CMD_STRUCT_SMART_RECONFIGURATION_EVENT));

	cmdInfo.lDeviceNumberMask1 = lDeviceNumberMask1;
	cmdInfo.lDeviceNumberMask2 = lDeviceNumberMask2;
	cmdInfo.lTableId = lTableId;
	cmdInfo.lSubTableId = lSubTableId;
	cmdInfo.lSubTableIdExt = lSubTableIdExt;
	cmdInfo.lFlags = lFlags;


	AddEvent(CMD_SMART_RECONFIGURE_EVENT, SERVER_POS_NUMBER, 0, &cmdInfo, sizeof(CMD_STRUCT_SMART_RECONFIGURATION_EVENT));
} ///END CR 442586
