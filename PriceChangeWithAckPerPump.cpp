// PriceChangeWithAckPerPump.cpp: implementation of the CPriceChangeWithAckPerPump class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PriceChangeWithAckPerPump.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPriceChangeWithAckPerPump::CPriceChangeWithAckPerPump()
{
	short nPump, nNozzle;

	m_lRefNumber = 0;
	_Module.m_server.m_cServerInfo.SetInPriceChangeWithAckProcessFlag(FALSE);
	m_eState = PCWA_IDLE;
	m_eFailureResult = PCWA_NO_ERROR;
	memset(&m_tWaitResponseTimer, 0, sizeof(SYSTEMTIME));
	memset(&m_szResultBufferXml, 0, sizeof(m_szResultBufferXml));

	for (nPump = 0; nPump < MAX_PUMPS_64; nPump++)
	{
		m_cPriceChangeAckPerPump[nPump].bIsPumpValid = FALSE; // pump not valid
		m_cPriceChangeAckPerPump[nPump].lResultCode = -1;
		m_cPriceChangeAckPerPump[nPump].nProductCounter = 0;

		for (nNozzle = 0; nNozzle < MAX_POSITIONS_PER_PUMP; nNozzle++)
		{
			m_cPriceChangeAckPerPump[nPump].cProductsArr[nNozzle].lErrorCode = -1;
			m_cPriceChangeAckPerPump[nPump].cProductsArr[nNozzle].lGradeNumber = 0;
		}
	}
}

CPriceChangeWithAckPerPump::~CPriceChangeWithAckPerPump()
{

}

/******************************************************************************
 Description: Init the array of price change acks per pump
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   16:00		Start	
******************************************************************************/	
void CPriceChangeWithAckPerPump::Init()
{
	short nPump, nNozzle;
	CString strPump;

	for (nPump = 0; nPump < _Module.m_server.GetLastValidPump(); nPump++)
	{
		strPump.Format("Pumps\\Pump%02d" , nPump+1);
		if ( _Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strPump) )
		{
			m_cPriceChangeAckPerPump[nPump].bIsPumpValid = TRUE;
			m_cPriceChangeAckPerPump[nPump].lResultCode = -1;
			m_cPriceChangeAckPerPump[nPump].nProductCounter = 0;

			if ( _Module.m_server.PumpInCommsTerminatedState(nPump+1) ||
				_Module.m_server.PumpInUnknownState(nPump+1) )	//4.0.24.70 - TD 115975
			{
				m_cPriceChangeAckPerPump[nPump].lResultCode = PCWA_ERROR_PUMP_OFFLINE;
				m_cPriceChangeAckPerPump[nPump].nProductCounter = 0;
			}
			else
			{
				for (nNozzle = 0; nNozzle < MAX_POSITIONS_PER_PUMP; nNozzle++)
				{
					m_cPriceChangeAckPerPump[nPump].cProductsArr[nNozzle].lGradeNumber = 0;
					short nGrade = _Module.m_server.m_cPumpArray[nPump].NozzleToGrade(nNozzle+1);

					if (nGrade > 0)
					{
						//Check if already have such grade in the array
						if (!IsProductFoundInArr(nPump, nGrade))
						{
							m_cPriceChangeAckPerPump[nPump].cProductsArr[nNozzle].lGradeNumber = nGrade;
							m_cPriceChangeAckPerPump[nPump].nProductCounter++;
						}
					}
					
					m_cPriceChangeAckPerPump[nPump].cProductsArr[nNozzle].lErrorCode = -1;
				}
			}
		}
		else
		{
			m_cPriceChangeAckPerPump[nPump].bIsPumpValid = FALSE;
		}
	}
	
	memset(&m_szResultBufferXml, 0, sizeof(m_szResultBufferXml));
}

/******************************************************************************
 Description: Check if this grade already exists in the products array
 Returns:			   	
 Parameters:  short nPump, short nGrade
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			28/10/2009   16:27		Start	
******************************************************************************/	
BOOL CPriceChangeWithAckPerPump::IsProductFoundInArr(short nPump, short nGrade)
{
	BOOL bFound = FALSE;

	for (short nNozzle = 0; nNozzle < MAX_POSITIONS_PER_PUMP; nNozzle++)
	{
		if (m_cPriceChangeAckPerPump[nPump].cProductsArr[nNozzle].lGradeNumber == nGrade)
		{
			bFound = TRUE;
			break;
		}
	}
	return bFound;
}

/******************************************************************************
 Description: Start the process of price change with ack per pump
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   16:00		Start	
******************************************************************************/	
void CPriceChangeWithAckPerPump::StartProcess()
{
	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("CPriceChangeWithAckPerPump::StartProcess.");
		_LOGMSG.LogMsg(str);
	}

	Init();

	

	// Turn on the price change with ack process flag
	 _Module.m_server.m_cServerInfo.SetInPriceChangeWithAckProcessFlag(TRUE);
	 m_eState = PCWA_PROCESS_STARTED;
	 m_eFailureResult = PCWA_NO_ERROR;	
}


/******************************************************************************
 Description: Returns the state of the process
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   14:00		Start	
******************************************************************************/	
long CPriceChangeWithAckPerPump::GetState()
{
	//return _Module.m_server.m_cServerInfo.GetPriceChangeState();
	return m_eState;
}


/******************************************************************************
 Description: Prepare xml data for offline pumps. This buffer will be returned
			  to BOS if the process succeeded
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   14:00		Start	
******************************************************************************/	
long CPriceChangeWithAckPerPump::GetFailureResult()
{
	if (m_eFailureResult != PCWA_ERROR_RESPONSE_TIMEOUT)
	{
		for (short nPump=0; nPump < _Module.m_server.GetLastValidPump(); nPump++)
		{
			if (m_cPriceChangeAckPerPump[nPump].bIsPumpValid == TRUE)
			{
				if (m_cPriceChangeAckPerPump[nPump].lResultCode > PCWA_NO_ERROR) //4.0.19.822
				{
					m_eFailureResult = (PriceChangeWithAckPerPumpError)m_cPriceChangeAckPerPump[nPump].lResultCode;
					break;
				}
			}
		}
	}
	
	return (long)m_eFailureResult;
}

/******************************************************************************
 Description:	Set the result came from CL into the ack array
 Returns:			   	
 Parameters:    long lPumpNumber, long lErrorCode
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   15:30		Start	
******************************************************************************/	
void CPriceChangeWithAckPerPump::SetAckPerPumpProduct(long lPumpNumber, long lErrorCode)
{
	if (!PUMP_NOT_VALID(lPumpNumber))
	{
		short nProduct;
		if (m_cPriceChangeAckPerPump[lPumpNumber-1].bIsPumpValid && m_cPriceChangeAckPerPump[lPumpNumber-1].nProductCounter > 0)
		{
			//m_cPriceChangeAckPerPump[lPumpNumber-1].lResultCode = lErrorCode;
			for (nProduct = 0; nProduct < MAX_POSITIONS_PER_PUMP; nProduct++)
			{
				if (m_cPriceChangeAckPerPump[lPumpNumber-1].cProductsArr[nProduct].lErrorCode < PCWA_NO_ERROR)
				{
					m_cPriceChangeAckPerPump[lPumpNumber-1].cProductsArr[nProduct].lErrorCode = lErrorCode;
					m_cPriceChangeAckPerPump[lPumpNumber-1].nProductCounter--;
					break;
				}
			}
			if (m_cPriceChangeAckPerPump[lPumpNumber-1].nProductCounter == 0)
				SetFinalResult(lPumpNumber-1);
		}
	}
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("CPriceChangeWithAckPerPump::SetAckPerPump. Pump not valid");
			_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,str);
		}
	}
}

/******************************************************************************
 Description: Check if results (Ack/Nack) for all pumps were received from CL
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			21/10/2009   09:30		Start	
******************************************************************************/	
BOOL CPriceChangeWithAckPerPump::IsAckReceivedForAllPumps()
{
	BOOL bReceivedForAll = TRUE;

	for (short nPump = 0; nPump < _Module.m_server.GetLastValidPump(); nPump++)
	{
		if (m_cPriceChangeAckPerPump[nPump].bIsPumpValid)
		{
			if (m_cPriceChangeAckPerPump[nPump].nProductCounter > 0)
			{
				bReceivedForAll = FALSE;
				break;
			}
		}
	}

	return bReceivedForAll;
}

/******************************************************************************
 Description: Set the state to response timeout
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			21/10/2009   20:00		Start	
******************************************************************************/	
void CPriceChangeWithAckPerPump::SetResponseTimeoutError()
{
	for (short nPump = 0; nPump < _Module.m_server.GetLastValidPump(); nPump++)
	{
		if (m_cPriceChangeAckPerPump[nPump].bIsPumpValid)
		{
			if (m_cPriceChangeAckPerPump[nPump].lResultCode == -1)
			{
				m_cPriceChangeAckPerPump[nPump].lResultCode = PCWA_ERROR_RESPONSE_TIMEOUT;
				m_cPriceChangeAckPerPump[nPump].nProductCounter = 0;
			}
		}
	}
}

/******************************************************************************
 Description: Set the reference number of the price change with ack process
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			21/10/2009   11:30		Start	
******************************************************************************/	
void CPriceChangeWithAckPerPump::SetRefNumber(long lRefNumber)
{
	m_lRefNumber = lRefNumber;
}

/******************************************************************************
 Description:	Prepare xml data for final result xml buffer. 
				This buffer will be returned to BOS if the process succeeded
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			02/03/2009   14:22		Start	
******************************************************************************/	
BOOL CPriceChangeWithAckPerPump::SetXmlBuffer()
{
	CXMLInterface	xmlInitDoc;
	CXMLPointers	cRootNode, cRefNumNode, cPumpsNode, cPumpNode;
	char szTmp[MAX_NAME_LENGTH] = {0};
	long lLength = 0;

	CComBSTR bstrStr; 
	bstrStr.Empty();
	
	BOOL bRetCode = TRUE;
	
	bRetCode = xmlInitDoc.InitDoc();

	if(bRetCode)
	{
		xmlInitDoc.CreateElement("PriceChangeWithAck", &cRootNode);
		xmlInitDoc.AppendRootElement(&cRootNode);

		sprintf((char *)szTmp,"%ld", m_lRefNumber);
		xmlInitDoc.CreateChildElement("RefNumber", (char*)szTmp, &cRefNumNode);
		xmlInitDoc.AppendChildElement(&cRefNumNode, &cRootNode);
		
		xmlInitDoc.CreateElement("Pumps", &cPumpsNode);
		xmlInitDoc.AppendChild(&cPumpsNode, &cRootNode);	

		//Check Offline pumps before price change
		for (short p = 0; p < _Module.m_server.GetLastValidPump(); p++) //loop on all valid pumps
		{
			if (m_cPriceChangeAckPerPump[p].bIsPumpValid)
			{
				xmlInitDoc.CreateElement("Pump",&cPumpNode);
				xmlInitDoc.SetLongAttribute("Number",p+1, &cPumpNode);
				xmlInitDoc.SetLongAttribute("ResultCode",m_cPriceChangeAckPerPump[p].lResultCode,&cPumpNode);
			}
			
			xmlInitDoc.AppendChild(&cPumpNode, &cPumpsNode);
		}

		bRetCode = xmlInitDoc.GetXmlString(&bstrStr);
		if(bRetCode)
		{
			lLength = Bstr2Buffer(bstrStr.m_str, m_szResultBufferXml, bstrStr.Length()+MAX_NAME_LENGTH);
			if (lLength > 0) 
				bRetCode = TRUE;
			else
				bRetCode = FALSE;
		}

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			char sBuff[MAX_XML_BUF_LEN] = {0};

			xmlInitDoc.GetXmlString(sBuff);			
			str.Format("Price change with ack. XmlResultBuffer=^%s^", sBuff);
			_LOGMSG.LogMsg(str);
		}

	}

	xmlInitDoc.CloseDoc();

	if(LOG_BASIC_FLOW_CONTROL && !bRetCode)
	{
		CString str;
		str.Format("Failed to create XML for price change with Ack process");
		_LOGMSG.LogMsg(str);
	}


	return bRetCode;
}

/******************************************************************************
 Description: Return the final xml result buffer
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			21/10/2009   09:30		Start	
******************************************************************************/	
void CPriceChangeWithAckPerPump::GetXmlResultBuffer(BSTR *pbstrBuffer)
{
	CComBSTR strResult(min(sizeof(m_szResultBufferXml), strlen(m_szResultBufferXml)+1),(LPCSTR)m_szResultBufferXml); 
	*pbstrBuffer = strResult.Copy();	
}


/******************************************************************************
 Description:	Handle all process states

 Returns:      	
 Parameters:   	

  WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			21/10/2009   14:15		Start	
******************************************************************************/
void CPriceChangeWithAckPerPump::HandleProcess()
{
	CString strLog;

	switch(m_eState)
	{
		case PCWA_IDLE:
		{			
			break;
		}

		case PCWA_PROCESS_STARTED:
		{
			if(_Module.m_server.m_cServerInfo.GetServerStatus() == RESPONSE_RECEIVED)
			{
				m_eState = PCWA_WAIT_TIMEOUT;
				ResetWaitCLResponseTime();
				InitWaitResponseTimer();
			}
			break;
		}

		case PCWA_WAIT_TIMEOUT:
		{
			HandleWaitCLResponseState();
			break;
		}

		default:
			m_eState = PCWA_IDLE;
			strLog.Format("CPriceChangeWithAckPerPump::HandleProcess - Invalid state.");
			_LOGMSG.LogMsg(strLog);
			
			break;
	}
}

/******************************************************************************
 Description: Checks if there was a timeout for CL response
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			21/10/2009   14:20		Start	
******************************************************************************/	
BOOL CPriceChangeWithAckPerPump::IsGotResponseTimeout()
{
	long lResponseTimeout = (long)(_Module.m_server.GetPriceChangeWithAckPerPumpResponseTimeout() * MILLISECONDS_IN_A_SECOND);
	return _Module.m_server.m_cCommonFunction.IsTimeOutPassed(m_tWaitResponseTimer, lResponseTimeout);
}

/******************************************************************************
 Description: Handle wait for response timeout from CL
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			21/10/2009   14:30		Start	
******************************************************************************/	
void CPriceChangeWithAckPerPump::HandleWaitCLResponseState()
{
	//Start timeout to check if there was a CL response timeout
	BOOL bRetVal = FALSE;
	bRetVal = IsGotResponseTimeout();

	if (bRetVal)
	{
		m_eState = PCWA_IDLE;
		m_eFailureResult = PCWA_ERROR_RESPONSE_TIMEOUT;
		SetResponseTimeoutError();
		_Module.m_server.m_cServerInfo.SetInPriceChangeWithAckProcessFlag(FALSE);

		SetXmlBuffer();

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("Price change with Ack per pump process state=%ld (PCWA_IDLE), error=%ld (PCWA_ERROR_RESPONSE_TIMEOUT)", m_eState, m_eFailureResult);
			_LOGMSG.LogMsg(str);
		}
	}
	else
	{
		if (IsAckReceivedForAllPumps())
		{
			m_eState = PCWA_IDLE;
			_Module.m_server.m_cServerInfo.SetInPriceChangeWithAckProcessFlag(FALSE);
			
			for (short nPump = 0; nPump < _Module.m_server.GetLastValidPump(); nPump++)
				SetFinalResult(nPump);

			SetXmlBuffer();

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString	str;
				str.Format("Price change with Ack per pump process state=%ld (PCWA_IDLE), error=%ld", m_eState, GetFailureResult());
				_LOGMSG.LogMsg(str);
			}
		}
	}
}

/******************************************************************************
 Description:	Set the final result which will be sent to BOS
 Returns:			   	
 Parameters:    
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			29/10/2009   11:00		Start	
******************************************************************************/	
void CPriceChangeWithAckPerPump::SetFinalResult(short nPump)
{
	if (m_cPriceChangeAckPerPump[nPump].bIsPumpValid)
	{
		if(m_cPriceChangeAckPerPump[nPump].lResultCode == -1)
		{
			m_cPriceChangeAckPerPump[nPump].lResultCode = PCWA_NO_ERROR;

			for (short nProduct = 0; nProduct < MAX_POSITIONS_PER_PUMP; nProduct++)
			{
				if (m_cPriceChangeAckPerPump[nPump].cProductsArr[nProduct].lErrorCode > PCWA_NO_ERROR)
				{
					m_cPriceChangeAckPerPump[nPump].lResultCode = m_cPriceChangeAckPerPump[nPump].cProductsArr[nProduct].lErrorCode;
					break;
				}
			}
		}
	}
}