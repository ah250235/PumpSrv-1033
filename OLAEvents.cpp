// OLAEvents.cpp: implementation of the COLAEvents class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PumpSrv.h"
#include "OLAEvents.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


COLAEvents  *_pOLAEvents;

HRESULT  COLAEvents::OlaEvent ( long EventId, long TerminalId, BSTR sParams )
{
	
	CString sMsg;
	sMsg.Format("OlaEvent !!!!!!!! EventId=[%ld] , TerminalId=[%ld]" ,EventId , TerminalId);
	_LOGMSG.LogMsg(sMsg);


	switch (EventId)
	{
		case OLA_EVENT_REPORT_READY:
		{
			_bstr_t sTmpBuffer(sParams,false);			
			CString sParams((const char *)sTmpBuffer);			//4.0.22.473 TD 142857	
			
			OLA_STAT ola;
			PAY_AT_PUMP_INFO info;
			
			short nReport = -1;
			
			EnterCriticalSection( &_Module.m_server.m_csReportArray );

			_Module.m_server.m_cOLASrv->ParseData(0,&sParams,&info);

			if (TerminalId == 99) //PumpSrv request
			{
				for (int i=0; i<MAX_REPORTS; i++)
					if (_Module.m_server.m_cReportArray.GetAt(i).GetRequestStatus() == RS_SENT)
					{
						PAY_AT_PUMP_INFO ArrayInfo;
						
						_Module.m_server.m_cReportArray.GetAt(i).GetPAPInfo(&ArrayInfo);
						
						//if (!memcmp(info.CardSaleInfo.cardData.sDeviceNumber, ArrayInfo.CardSaleInfo.cardData.sDeviceNumber, sizeof(info.CardSaleInfo.cardData.sDeviceNumber)))
						if ( a2l(info.CardSaleInfo.cardData.sDeviceNumber,2) == a2l(ArrayInfo.CardSaleInfo.cardData.sDeviceNumber,2) )
							if (!memcmp(&info.CardSaleExtraData2.sOriginTrans , &ArrayInfo.CardSaleExtraData2.sOriginTrans, sizeof(info.CardSaleExtraData2.sOriginTrans)))
							{
								nReport = i;
								break;
							}
					}

				//Did not find a request for this report
				//Try to open new entry in ReportArray
				if (nReport == -1) 
				{
					//get free entry in ReportArray
					for (int i=1; i<MAX_REPORTS; i++)
						if (_Module.m_server.m_cReportArray.GetAt(i).GetRequestStatus() == RS_EMPTY)
						{
							nReport = i;

							if(LOG_BASIC_FLOW_CONTROL)
							{
								CString str;
								str.Format("OlaEvent : new entry is written to ReportArray[%d], DeviceNumber=[%.2s], OriginTrans=[%.1s]",nReport, info.CardSaleInfo.cardData.sDeviceNumber, &info.CardSaleExtraData2.sOriginTrans);
								_LOGMSG.LogMsg(str);
							}

							break;
						}
				}
					
				if (nReport != -1)
				{
					//update data in ReportArray
					ola.m_byState = SESSION_GET_REPORT | OLA_REQUEST_TO_SEND ;

					_Module.m_server.m_cReportArray.GetAt(nReport).SetOlaStat(&ola); 	
					_Module.m_server.m_cReportArray.GetAt(nReport).SetPAPInfo(&info);
					_Module.m_server.m_cReportArray.GetAt(nReport).SetRequestStatus(RS_RECEIVED);

				}
				
			}
			else //POS request
			{
				//send "event" to pos //3.0.3.11
				long lDeviceNumber = a2l(info.CardSaleInfo.cardData.sDeviceNumber,sizeof(info.CardSaleInfo.cardData.sDeviceNumber));
				long lPOSNumber;
				CPosInfo tmpPos;
				POSITION position = _Module.m_server.m_cPosMap.GetStartPosition();

				while(position)
				{
					_Module.m_server.m_cPosMap.GetNextAssoc(position,lPOSNumber,tmpPos);
					if (lPOSNumber == lDeviceNumber)
					{
						//set m_bOLAEvent for this POSNumber !!
						if(_Module.m_server.m_cPosMap.Lookup(lPOSNumber,tmpPos))
						{
							tmpPos.SetOLAEvent(EventId);
							_Module.m_server.m_cPosMap.SetAt(lPOSNumber,tmpPos);
						}
						break;
					}
				}	
				
				nReport = 1;
				
			}		

			LeaveCriticalSection( &_Module.m_server.m_csReportArray );

			if (nReport == -1) //did not find matching request in ReportArray
				if(LOG_ERROR)
					{
						CString str;
						str.Format("OlaEvent : NO PLACE IN REPORT ARRAY !! TerminalId=[%ld], DeviceNumber=[%.2s], OriginTrans=[%.1s]",TerminalId, info.CardSaleInfo.cardData.sDeviceNumber, &info.CardSaleExtraData2.sOriginTrans);
						_LOGMSG.LogMsg(str);
					}
				
			break;
		}
		//4.0.5.39
		case OLA_EVENT_DISPLAY_MSG:
		{
			if(PUMPSRV_REQUEST == TerminalId) //Intended for PumpSrv
			{
				_bstr_t sTmpBuffer(sParams,false);
				
				char sParsedParams[100];	
				CString sParams;
				BOOL bFound;
				int iLen ;
				char *pParam;
				char sMsgDisplay[MAX_TERMINAL_MSG_LEN];
				long lPumpNumber;
				long lTimeOut;
				long lMsgDisplayLen;

				strcpy(sParsedParams,"");
				strcpy(sMsgDisplay,"");
				lPumpNumber = 0;
				lTimeOut = 0;
				lMsgDisplayLen = 0;

				strcpy(sParsedParams, (const char*) sTmpBuffer);		//4.0.22.473 TD 142857	
				//Parsing the PumpNumber, Timeout and the actual message to display from sParsedParams
				//sscanf(sParsedParams, "%d,%d,%s,%d", lPumpNumber, lTimeOut, sMsgDisplay, lMsgDisplayLen);
				sParams = (LPCSTR) sParsedParams;
				bFound = _Module.m_server.m_cOLASrv->GetParam(&sParams, sParams.Find("PumpNumber[I]"),&pParam,&iLen);
				if(bFound) 
				{
					if(iLen)
					{
						lPumpNumber = a2i((BYTE *)pParam,iLen);
					}
					else
					{
						lPumpNumber = 0;
					}		
				}	

				bFound = _Module.m_server.m_cOLASrv->GetParam(&sParams, sParams.Find("TimeOut[I]"),&pParam,&iLen);
				if(bFound) 
				{
					if(iLen)
					{
						lTimeOut = a2i((BYTE *)pParam,iLen);
					}
					else
					{
						lTimeOut = 0;
					}		
				}
				
				bFound = _Module.m_server.m_cOLASrv->GetParam(&sParams, sParams.Find("ResultMessageLen[I]"),&pParam,&iLen);
				if(bFound) 
				{
					if(iLen)
					{
						lMsgDisplayLen = a2i((BYTE *)pParam,iLen);
					}
					else
					{
						lMsgDisplayLen = 0;
					}		
				}
				
				bFound = _Module.m_server.m_cOLASrv->GetParam(&sParams, sParams.Find("ResultMessage[S]"),&pParam,&iLen);
				if(bFound) 
				{
					if(iLen)
					{
						memcpy(sMsgDisplay, pParam, min(sizeof(sMsgDisplay),iLen));
					}
					else
					{
						strcpy(sMsgDisplay,"");
					}		
				}

				CMD_STRUCT_DISPLAY_MSG cmdInfo;
				//Filling the Display Msg command
				cmdInfo.lTimeOut= lTimeOut;
				strcpy(cmdInfo.sDisplayMsg , sMsgDisplay);
				cmdInfo.lDisplayMsgLen = lMsgDisplayLen;
				if(LOG_DETAIL_FLOW_CONTROL)
				{
					CString str;
					str.Format("Sending a DisplayMessage command from COLAEvents::OlaEvent. Pump %02ld ,lTimeOut %ld, sMsgDisplay %s",
						        lPumpNumber, lTimeOut, sMsgDisplay);									
					_LOGMSG.LogMsg(str);							
				}
				//Sending the Display Msg command
				_Module.m_server.SetCommand(CMD_DISPLAY_MSG, SERVER_POS_NUMBER, lPumpNumber, &cmdInfo, sizeof(CMD_STRUCT_DISPLAY_MSG));
			}
			else //Intended for POS, PumpSrv has nothing to do with it
			{
				CString str;
				str.Format("COLAEvents::OlaEvent: OLA_EVENT_DISPLAY_MSG event arrived for POS - irrelevant for PUMPSRV");
				_LOGMSG.LogMsg(str);
			}

			break;
		}
	}

	return S_OK;
}