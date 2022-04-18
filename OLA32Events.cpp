// OLA32Events.cpp: implementation of the COLA32Events class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLA32Events.h"


COLA32Events  *_pOLA32Events;

HRESULT  COLA32Events::OlaEvent ( long EventId, long TerminalId, BSTR sParams )
{
	
	long lRtc = 0; 

	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;
		str.Format("Received OLA Event=%ld, TerminalId=%ld",EventId, TerminalId);
		_LOGMSG.LogMsg(str);
	}

	switch (EventId)
	{
		case OLA_EVENT_REPORT_READY:  // not use in this Ola version
		{
			_bstr_t sTmpBuffer(sParams,false);			
			CString sParams((const char *)sTmpBuffer);			//4.0.25.80 TD 142857
			
			OLA_STAT ola;
			PAY_AT_PUMP_INFO info;
			
			short nReport = -1;
			
			EnterCriticalSection( &_Module.m_server.m_csReportArray );

			_Module.m_server.m_cOLASrv->ParseData(0,&sParams,&info);

			if (TerminalId == 99) //PumpSrv request
			{
				for (int i=0; i<MAX_REPORTS; i++)
					if (_Module.m_server.m_cReportArray[i].GetRequestStatus() == RS_SENT)
					{
						PAY_AT_PUMP_INFO ArrayInfo;
						
						_Module.m_server.m_cReportArray[i].GetPAPInfo(&ArrayInfo);
						
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
						if (_Module.m_server.m_cReportArray[i].GetRequestStatus() == RS_EMPTY)
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

					_Module.m_server.m_cReportArray[nReport].SetOlaStat(&ola); 	
					_Module.m_server.m_cReportArray[nReport].SetPAPInfo(&info);
					_Module.m_server.m_cReportArray[nReport].SetRequestStatus(RS_RECEIVED);

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
		case  UPDATE_SHIFT_DEBIT_PARAM:
		{
			long lPumpNumber = 0;
			CString strOutBuff;			
			strOutBuff = sParams;
			strOutBuff += ',';
			char *pParameter;
			BOOL bFound;
			int iLen;

			if(LOG_BASIC_FLOW_CONTROL)
			{	
				CString str;
				str.Format("Received OLA Event 2 Buff=%s" ,(LPCTSTR)strOutBuff);
				_LOGMSG.LogMsg(str);
			}

			bFound = _Module.m_server.m_cOLASrv->GetParam(&strOutBuff,strOutBuff.Find("PumpNumber[I]"),&pParameter,&iLen);
			if(bFound) 
			{
				OLA_STAT	Ola;
				PAY_AT_PUMP_INFO PayAtPumpInfo;

				if(iLen)
				{
					lPumpNumber = a2l((BYTE *)pParameter,iLen); 

					if (PUMP_NOT_VALID(lPumpNumber))
					{
						lRtc =1 ;
					}
					else
					{						
						memset(&PayAtPumpInfo , ' ' , sizeof(PAY_AT_PUMP_INFO));
						l2a(lPumpNumber , PayAtPumpInfo.CardSaleInfo.cardData.sPumpNumber , sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sPumpNumber));

						CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);
						
						_Module.m_server.m_cDebitInfoArray[lPumpNumber - 1].GetOlaStat(&Ola);
						
						//_Module.m_server.m_cOLASrv->ParseData(OLASRV_NEED_MORE_PARAMS , &strOutBuff , &PayAtPumpInfo ,NULL , Ola.m_byStat); //4.0.2.29
						Ola.m_byState = OLA_WAIT_FOR_MORE_INFO | SESSION_SHIFT_DEBIT;
						Ola.m_lSessionNumber = TerminalId; 
						Ola.m_byCheckCount = 0;

						//4.0.2.29
						_Module.m_server.m_cDebitInfoArray[lPumpNumber - 1].SetOlaStat(&Ola);
						_Module.m_server.m_cOLASrv->ParseData(OLASRV_NEED_MORE_PARAMS , &strOutBuff , &PayAtPumpInfo ,NULL , Ola.m_byState); //4.0.2.29
						//4.0.2.29


						_Module.m_server.m_cOLASrv->ParseData(OLASRV_NEED_MORE_PARAMS , &strOutBuff , &PayAtPumpInfo ,NULL , Ola.m_byState);

						_Module.m_server.UpdateDebitTimeOutParmeters(lPumpNumber , &Ola, (CARD_SALE_ALL3 *)&PayAtPumpInfo.CardSaleInfo.cardData); //4.0.0.66 //4.0.5.0


						_Module.m_server.m_cDebitInfoArray[lPumpNumber - 1].ConvertPAP2DebitInfo(&PayAtPumpInfo);
						_Module.m_server.SendDebitResult(lPumpNumber ,Ola.m_lSessionNumber);
						//_Module.m_server.m_cDebitInfoArray[lPumpNumber - 1].SetOlaStat(&Ola); //4.0.2.29
					}
				}

				if(LOG_BASIC_FLOW_CONTROL)
				{	
					CString str;
					str.Format("Received OLA Event 2  Pump Id=%ld, lRtc=%ld", lPumpNumber , lRtc);
					_LOGMSG.LogMsg(str);
				}
			}
			else
			{
				if(LOG_BASIC_FLOW_CONTROL)
				{	
					CString str;
					str.Format("Received OLA Event 2 withount Pump Id");
					_LOGMSG.LogMsg(str);
				}
			}
		}
		break;
		case  END_SHIFT_DEBIT_PARAM:
		{	
			/*
			if (PUMP_NOT_VALID(TerminalId))
			{
				lRtc =1 ;
			}
			else
			{
				// Free Pump operation.
				_Module.m_server.m_ByLockPAPModeArray[TerminalId - 1] = 0;
			}*/

			if(LOG_BASIC_FLOW_CONTROL)
			{	
				CString str;
				str.Format("Received OLA Event 3 Pump=%ld, lRtc=%ld", TerminalId, lRtc);
				_LOGMSG.LogMsg(str);
			}
		}
		break;
		//4.0.2.35
		case  INIT_DEBIT_EVENT_PARAM:
		{
			CString str; //4.0.5.12
			CtrlRetCode retCode = CTRL_OK;
			CString strOutBuff;			
			strOutBuff = sParams;
			strOutBuff += ',';
			char *pParameter;
			BOOL bFound;
			int iLen;
			long lPumpNumber = 0;
			long lTmPumpNumber = 0;


			if(LOG_BASIC_FLOW_CONTROL)
			{					
				str.Format("Received OLA Event 4");
				_LOGMSG.LogMsg(str);
			}


			bFound = _Module.m_server.m_cOLASrv->GetParam(&strOutBuff,strOutBuff.Find("PumpNumber[I]"),&pParameter,&iLen);

			if(bFound) 
			{				
				lPumpNumber = a2l((BYTE *)pParameter,iLen); 	
				lTmPumpNumber = lPumpNumber;
				
				//CSingleLock slPumpArray(&_Module.m_server.m_csPumpArray, TRUE);

				if (!lPumpNumber) 
				{
					//for (int i= 1; i <= MAX_PUMPS ; i++)
					for (int i = 1; i <= _Module.m_server.GetLastValidPump(); i++)  //4.0.0.79
					{												
						if(PUMP_NOT_VALID(i))
							retCode = CTRL_INVALID_PUMP_NUMBER;
						if(PUMP_NOT_IN_WORKING_STATE(i))
							retCode = CTRL_PUMP_NOT_IN_WORKING_STATE;
						else
						{
							 //4.0.3.303
							if (_Module.m_server.m_cInitDebitHandler.GetStatus(i) == DEBIT_SERVER_IDLE)
							{
								_Module.m_server.m_cInitDebitHandler.SetStatus(i, DEBIT_SERVER_REQUEST_TO_START);
							}
							else
							{		
								_Module.m_server.m_cDebitInfoArray[i - 1].SetPendingInitDebitSesssion(TRUE);
								if(LOG_BASIC_FLOW_CONTROL)
								{
									str.Format("Received OLA Event 4 - (init debit), put request in pending queue Pump = %d ", i);
									_LOGMSG.LogMsg(str);
								}
							}
							//4.0.3.303
						}
					}
				}
				else
				{
					//4,0,3,305
					if(!PUMP_NOT_VALID(lPumpNumber))	
					{
						//lPumpNumber = lPumpNumber -1;

						if (_Module.m_server.m_cInitDebitHandler.GetStatus(lPumpNumber) == DEBIT_SERVER_IDLE)
						{		
							  _Module.m_server.m_cInitDebitHandler.SetStatus(lPumpNumber,DEBIT_SERVER_REQUEST_TO_START);

							if(LOG_BASIC_FLOW_CONTROL)
							{
								str.Format("Received OLA Event 4 process debit request Pump = %d " ,lPumpNumber);
								_LOGMSG.LogMsg(str);
							}

						}
						else
						{
							//retCode = CTRL_IN_MIDDLE_OF_PROCESS; //4.0.3.303
							
							//4.0.3.303
							_Module.m_server.m_cDebitInfoArray[lPumpNumber - 1].SetPendingInitDebitSesssion(TRUE);

							if(LOG_BASIC_FLOW_CONTROL)
							{
								str.Format("Received OLA Event 4  (init debit), put request in pending queue Pump = %d " ,lPumpNumber +1);
								_LOGMSG.LogMsg(str);
							}
							//4.0.3.303
						}
					}
					else
					{
						if(LOG_BASIC_FLOW_CONTROL)
						{
							str.Format("Received OLA Event 4 Pump not valid = %d " ,lPumpNumber);
							_LOGMSG.LogMsg(str);
						}
					}
						
				}				
			}			

			if(LOG_BASIC_FLOW_CONTROL)
			{
				str.Format("Received OLA Event 4 , Pump=%ld , Found data = %d, RetCode = %d ",
					lTmPumpNumber, 
					bFound, 
					retCode );
				_LOGMSG.LogMsg(str);
			}
		}
		//4.0.5.12
	}

	return S_OK;
}