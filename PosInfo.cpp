// PosInfo.cpp

#include "stdafx.h"
#include "PosInfo.h"
#include <math.h>


CPosInfo::CPosInfo(LONG lPosNumber, LONG lVersion)
{
	m_lPosNumber = lPosNumber;
	m_cTimeConnection = CTime::GetCurrentTime();
	m_lLastCmdCode = 0;
	m_lLastCmdNumber = 0;
	m_lLastCmdRetCode = 0;
	m_lLastCmdStatus = NO_POS_CMD;
	m_lGPIVersion = lVersion;
	m_lPumpsInView = 0xffffffffffffffff;  //4.0.0.79
	m_bRealPOS = FALSE;
	m_lOLAEvent = 0;
	m_lPrinterStatus = PRINTER_OFFLINE; //4.0.5.44
	m_hEventReceipt = NULL;				//4.0.19.508 (61994)
}

CPosInfo::CPosInfo()
{
	m_lPosNumber = -1L;
	m_lLastCmdCode = 0;
	m_lLastCmdNumber = 0;
	m_lLastCmdRetCode = 0;
	m_lLastCmdStatus = NO_POS_CMD;
	m_lGPIVersion = GPI_VERSION_1;
	m_lPumpsInView = 0xffffffffffffffff;  //4.0.0.79
	m_bRealPOS = FALSE;
	m_lOLAEvent = 0;
	m_lPrinterStatus = PRINTER_OFFLINE; //4.0.5.44
	m_hEventReceipt = NULL;				//4.0.19.508 (61994)
}

CPosInfo::CPosInfo(const CPosInfo & posInfo)
{
	m_lPosNumber = posInfo.m_lPosNumber;
	m_cTimeConnection = posInfo.m_cTimeConnection;
	m_lLastCmdCode = posInfo.m_lLastCmdCode;
	m_lLastCmdNumber = posInfo.m_lLastCmdNumber;
	m_lLastCmdRetCode = posInfo.m_lLastCmdRetCode;
	m_lLastCmdStatus = posInfo.m_lLastCmdStatus;
	m_lGPIVersion = posInfo.m_lGPIVersion;
	m_lPumpsInView = posInfo.m_lPumpsInView;
	m_bRealPOS = posInfo.m_bRealPOS;
	m_sReceiptText = posInfo.m_sReceiptText;
	m_lOLAEvent = posInfo.m_lOLAEvent;
	m_lPrinterStatus = posInfo.m_lPrinterStatus; //4.0.5.44
	m_hEventReceipt = posInfo.m_hEventReceipt;	//4.0.19.508 (61994)

	//4.0.19.504 - 47559
	long lSize = posInfo.m_cPOSTrsList.GetSize();
	m_cPOSTrsList.RemoveAll();

	if (lSize)
	{	
		for (int i=0; i< lSize; i++)
		{
			m_cPOSTrsList.InsertAt(i, (TAG_OLA_REC5_EX&)posInfo.m_cPOSTrsList.GetAt(i));
		}
	}
}

CPosInfo::~CPosInfo()
{
	m_cPOSTrsList.RemoveAll(); //4.0.19.504 - 47559
}

void CPosInfo::operator =( const CPosInfo & posInfo )
{
	m_lPosNumber = posInfo.m_lPosNumber;
	m_cTimeConnection = posInfo.m_cTimeConnection;
	m_lLastCmdCode = posInfo.m_lLastCmdCode;
	m_lLastCmdNumber = posInfo.m_lLastCmdNumber;
	m_lLastCmdRetCode = posInfo.m_lLastCmdRetCode;
	m_lLastCmdStatus = posInfo.m_lLastCmdStatus;
	m_lGPIVersion = posInfo.m_lGPIVersion;
	m_lPumpsInView = posInfo.m_lPumpsInView;
	m_bRealPOS = posInfo.m_bRealPOS;
	m_hEventReceipt = posInfo.m_hEventReceipt;
	m_sReceiptText =  posInfo.m_sReceiptText;
	m_lOLAEvent = posInfo.m_lOLAEvent;
	m_lPrinterStatus = posInfo.m_lPrinterStatus; //4.0.5.44

	//4.0.19.504 - 47559
	long lSize = posInfo.m_cPOSTrsList.GetSize();
	m_cPOSTrsList.RemoveAll();

	if (lSize)
	{	
		for (int i=0; i< lSize; i++)
		{
			m_cPOSTrsList.InsertAt(i, (TAG_OLA_REC5_EX&)posInfo.m_cPOSTrsList.GetAt(i));
		}
	}
}

void CPosInfo::SetConnectionTime()
{
	m_cTimeConnection = CTime::GetCurrentTime();
}

void CPosInfo::SetLastCmd(long lCmdCode, long lCmdNumber )
{
	m_lLastCmdCode = lCmdCode;
	m_lLastCmdNumber = lCmdNumber;
	m_lLastCmdStatus = IN_QUEUE;
	m_lLastCmdRetCode = 0;
	m_cTimeConnection = CTime::GetCurrentTime();
}

void CPosInfo::UpdateLastCmd(long lCmdCode, long lCmdNumber, PosCmdState lCmdStatus)
{
	if((m_lLastCmdCode == lCmdCode)&&(m_lLastCmdNumber == lCmdNumber))
	{
		m_lLastCmdRetCode = 0;
		m_lLastCmdStatus = lCmdStatus;
	}
}

void CPosInfo::UpdateLastCmd(long lCmdCode, long lCmdNumber, PosCmdState lCmdStatus, long lRetCode)
{
	if((m_lLastCmdCode == lCmdCode)&&(m_lLastCmdNumber == lCmdNumber))
	{
		m_lLastCmdStatus = lCmdStatus;
		m_lLastCmdRetCode = lRetCode;
	}
}


void CPosInfo::SetPumpsInView(long lPumpsInView)
{
	m_lPumpsInView = lPumpsInView;
}

//Eyal 4.0.0.79 Start
void CPosInfo::SetPumpsInView(unsigned long lPumpMap1to32 , unsigned long lPumpMap33to64)
{
	DWORD64 tmpPumpsInView = 0;
	tmpPumpsInView |= lPumpMap33to64;
	tmpPumpsInView <<= 32;
	tmpPumpsInView |= lPumpMap1to32;

	m_lPumpsInView = tmpPumpsInView;
}

void CPosInfo::SetPumpsInView(DWORD64 dw64PumpsInView)
{
	m_lPumpsInView = dw64PumpsInView;
}

BOOL CPosInfo::IsPumpInFieldOfView(long lPumpNumber)
{
	long p = lPumpNumber-1;
	//unsigned long lMask = (long)(pow( 2.0, (double)p ));
	//unsigned long lMask = 1<<p;  Eyal 4.0.0.26
	DWORD64 lMask = (DWORD64)1 << p;
	BOOL bInView = FALSE;
	
	if(lMask & m_lPumpsInView)
		bInView = TRUE;

	return bInView;
}

long CPosInfo::GetPrinterStatus()
{
	return (m_lPrinterStatus);
}

void CPosInfo::SetPrinterStatus(PrinterStatus lPrinterStatus)
{
	m_lPrinterStatus = lPrinterStatus;
}

/*
BOOL CPosInfo::IsPumpInFieldOfView(long lPumpNumber)
{
	long p = lPumpNumber-1;
	//unsigned long lMask = (long)(pow( 2.0, (double)p ));
	unsigned long lMask = 1<<p;
	BOOL bInView = FALSE;
	
	if(lMask & m_lPumpsInView)
		bInView = TRUE;

	return bInView;
}
*/
//4.0.0.79 End

BOOL CPosInfo::IsPOSConnected()
{
	BOOL bPOSConnected = TRUE;
	CTime ct = CTime::GetCurrentTime();
	CTimeSpan ts;

	ts = ct - m_cTimeConnection;
	if (ts.GetTotalSeconds() > 5)//4.0.8.500
		bPOSConnected = FALSE;

	return bPOSConnected;
}

void CPosInfo::SetRealPOS()
{
	m_bRealPOS = TRUE;
}

BOOL CPosInfo::IsRealPOS()
{
	return m_bRealPOS;
}

void CPosInfo::SendEventReceipt()
{
	SetEvent(m_hEventReceipt);
}

//4.0.2.15 Change prototype to by reference
void CPosInfo::SetReceiptText(CComBSTR & sReceiptText)
{
	//removed Copy from ComBSTR class and changed type of m_sReceiptText to bstr_t from BSTR  //4.0.15.291
	m_sReceiptText = sReceiptText; //merge from 4.0.15.291
//	m_sReceiptText = sReceiptText.Copy();
}

BSTR CPosInfo::GetReceiptText()
{
	return m_sReceiptText;
}

long CPosInfo::WaitForSingleObjectReceipt(const long lTimeout)
{
	long	lRet  = 0;


	/** The database may complete before this function gets called, in which case, callin the ResetEvent here, will cause a timeout. **/
	//3.2.1.53 Make sure the event is not signalled from 
	//a previous attempt to reprint receipts	
	//ResetEvent(m_hEventReceipt);

	if (TEMPORARY_DEBUG)
	{
		////////////////////////////////////////////////////////////////////////////////////////
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("WAIT RECEIPT_INFO_DATA retCode=%d handle=%x " , lRet, m_hEventReceipt);
			_LOGMSG.LogMsg(m_lPosNumber,LOG_POS,str);
		}			

		/////////////////////////////////////////////////////////////////////////////////////////
	}

    //4.0.9.503if (WaitForSingleObject(m_hEventReceipt,2000) == WAIT_OBJECT_0)
	if (WaitForSingleObject(m_hEventReceipt, lTimeout) == WAIT_OBJECT_0) //5000
	{
		lRet = 1;
	}


	if (TEMPORARY_DEBUG)
	{
		////////////////////////////////////////////////////////////////////////////////////////
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			str.Format("FOUND_RECEIPT_INFO retCode=%ld" , lRet );
			_LOGMSG.LogMsg(m_lPosNumber,LOG_POS,str);
		}			
		/////////////////////////////////////////////////////////////////////////////////////////
	}



   return lRet;
}

void CPosInfo::CreateEventReceipt()
{
	m_hEventReceipt = CreateEvent(NULL, FALSE, FALSE, NULL);

		////////////////////////////////////////////////////////////////////////////////////////
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString	str;
			
			str.Format("CreateEventReceipt, Handle=%x" , m_hEventReceipt);
			_LOGMSG.LogMsg(str);
		}			
		/////////////////////////////////////////////////////////////////////////////////////////

}

void CPosInfo::RemoveEventReceipt()
{
	if (m_hEventReceipt)
	{
		CloseHandle(m_hEventReceipt);
		m_hEventReceipt = NULL;				//4.0.19.508 (61994)
	}
}

//3.0.3.11 start
void CPosInfo::ResetOLAEvent()
{
	m_lOLAEvent = 0L;
}

void CPosInfo::SetOLAEvent(long lOLAEvent)
{
	m_lOLAEvent = lOLAEvent;
}


long CPosInfo::GetOLAEvent()
{
	return m_lOLAEvent;
}

//3.0.3.11 end