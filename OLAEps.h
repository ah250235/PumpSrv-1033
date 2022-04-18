// OlaR10.h: interface for the COlaR10 class. R10 User Story - 71146 add new class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLAR10_H__A2097DBD_EE68_4855_809C_0FCD060077CE__INCLUDED_)
#define AFX_OLAR10_H__A2097DBD_EE68_4855_809C_0FCD060077CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "Retalix.Client.Forecourt.OlaAdapter.tlb"  raw_interfaces_only no_namespace 

#define OLA_RECOVERY_SLEEP_TIME			1000 // milisec

class COLAEps : public COLABase
{

protected:
	IOlaComAdapterPtr	m_pSrvPtr;

private:
	void InvokeOlaEpsApplication();
	long IsOlaAlreadyRuning();
	
	BYTE m_byInvokeOlaAppCounter;

public:
	void Init();
	long GetCardInfoEx(long lClientId, BSTR  sData, BSTR *pRetData,long lFlags = 0); 
	void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);

	COLAEps()
	{
		m_bAddPreAuthInfo = TRUE; 
		m_byInvokeOlaAppCounter = 0;
	}

	virtual ~COLAEps()
	{
	
	}

	//Virtual functions
	virtual long AddParams(long lSessId, char *pInBuffer );
	virtual long StartSession(char * pInBuffer, char *pszType);
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer);
	virtual long EndSession(long lSessId);
	
	virtual long GetOLAType() { return OLASRV_EPS;}

	// Function not Implemented
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;} 
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData) {return 0;}
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL);


};

#endif // !defined(AFX_OLAR10_H__A2097DBD_EE68_4855_809C_0FCD060077CE__INCLUDED_)
