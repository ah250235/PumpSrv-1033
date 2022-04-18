// OlaR10.h: interface for the COlaR10 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLAR10_H__A2097DBD_EE68_4855_809C_0FCD060077CE__INCLUDED_)
#define AFX_OLAR10_H__A2097DBD_EE68_4855_809C_0FCD060077CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "../ThirdParty/Retalix.Fuel.HardwareLayer.Com.tlb"  raw_interfaces_only no_namespace 

class COLAEps : public COLABase
{

protected:
	IGciComAdapterPtr	m_pSrvPtr;

public:
	void Init();
	long GetCardInfoEx(long lClientId, BSTR  sData, BSTR *pRetData,long lFlags = 0); //4.0.12.505
	void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);//4.0.20.56

	COLAEps()
	{
		m_bAddPreAuthInfo = TRUE; 
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
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL);		//4.0.20.52 63936	//4.0.21.21	 TD-69389
	

};

#endif // !defined(AFX_OLAR10_H__A2097DBD_EE68_4855_809C_0FCD060077CE__INCLUDED_)
