//-----------------------------------------------------------------------
// File Name   :	PosInfo.h
// 
// Description :	POS Information class definition
//					
// Written by  :	Moti Lichi 10.04.97
// 
//-----------------------------------------------------------------------



#ifndef __POS_INFO_H__
#define __POS_INFO_H__

// The different statuses a printer can be found at 
enum PrinterStatus
{
	PRINTER_OFFLINE = 0,
	PRINTER_ONLINE = 1
};

class  CPosInfo : public CObject
{
public:
	virtual ~CPosInfo();

	void SetLastCmd(long lCmdCode, long lCmdNumber );
	void UpdateLastCmd(long lCmdCode, long lCmdNumber, PosCmdState lCmdStatus);
	void UpdateLastCmd(long lCmdCode, long lCmdNumber, PosCmdState lCmdStatus, long lRetCode);
		
public:
	//4.0.1.32
	long GetLastRetCode() { return m_lLastCmdRetCode; }
	void SetLastRetCode(long lRetCode) { m_lLastCmdRetCode = lRetCode; }
	//4.0.1.32 end
	
	long GetOLAEvent();
	void SetOLAEvent(long lOLAEvent);
	void ResetOLAEvent();
	void RemoveEventReceipt();
	void CreateEventReceipt();
	HANDLE	GetEventReceipt(){return m_hEventReceipt;}	//4.0.19.508 (61994)
	long WaitForSingleObjectReceipt(const long lTimeout = 5000); //4.0.19.504 - 47559
	BSTR GetReceiptText();
	void SetReceiptText(CComBSTR & sReceiptText);  //4.0.2.15 Pass by reference : does not cause memory leaks
	void SendEventReceipt();
	BOOL IsPumpInFieldOfView(long lPumpNumber);
	BOOL IsPOSConnected();
	void SetRealPOS();
	BOOL IsRealPOS();
	void SetPumpsInView(long lPumpsInView);

	//4.0.0.79 Start
	void SetPumpsInView(DWORD64 dw64PumpsInView);
	void SetPumpsInView(unsigned long lPumpMap1to32 , unsigned long lPumpMap33to64);
	//4.0.0.79 End

	// 4.0.5.44 Start
	long GetPrinterStatus();
	void SetPrinterStatus(PrinterStatus lPrinterStatus);
	// 4.0.5.44 End
	
	CPosInfo();		// default constructor
	CPosInfo(LONG lPosNumber, LONG lVersion); // build constructor
	CPosInfo(const CPosInfo & posInfo); // copy constructor
	void operator =( const CPosInfo & posInfo );
	void SetConnectionTime();
	
	CArray<TAG_OLA_REC5_EX, TAG_OLA_REC5_EX&>	m_cPOSTrsList;
		
protected:
	LONG		m_lOLAEvent; //3.0.3.11
	HANDLE      m_hEventReceipt;
	_bstr_t	    m_sReceiptText; //4.0.18.501 merge from 4.0.15.291
	long		m_lGPIVersion;
	long		m_lLastCmdCode;
	long		m_lLastCmdNumber;
	long		m_lLastCmdRetCode;
	PosCmdState m_lLastCmdStatus;
	CTime		m_cTimeConnection;
	LONG		m_lPosNumber;	
	///LONG		m_lPumpsInView;
	DWORD64     m_lPumpsInView;   //4.0.0.79
	BOOL		m_bRealPOS;
	PrinterStatus m_lPrinterStatus; //4.0.5.44 - holds the current printer status
};

#endif