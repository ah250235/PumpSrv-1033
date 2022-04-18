// OLA32Srv.h: interface for the COLA32Srv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLA32SRV_H__9FA9BC21_BEAF_4992_A4A7_B219252AF5B5__INCLUDED_)
#define AFX_OLA32SRV_H__9FA9BC21_BEAF_4992_A4A7_B219252AF5B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define NUMBER_OF_IMPLEMENTED_METHODS 12

#include "OLABase.h"

class COLA32Srv : public COLABase  
{
protected:
	IUnknown * m_pUnknown;
	IDispatch * m_pDispatch;
	DISPID MethodIDs[NUMBER_OF_IMPLEMENTED_METHODS];

	BOOL InitInterface();
	HRESULT GetFunctionPointers();
	
public:	
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData) {return 0;}
	COLA32Srv();
	virtual ~COLA32Srv();

	//Virtual functions
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL) { return 0; }	//4.0.20.52 (TD 63936)
	virtual long AddParams(long lSessId, char *pInBuffer );

	virtual long StartSession(char * pInBuffer, char *pszType);
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer);
	virtual long EndSession(long lSessId);
	
	virtual void Init();
	virtual void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);

	//4.0.0.73
	virtual long GetOLAType() { return OLASRV_POSITIVE_32; }
	//4.0.5.39
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;} 
};

#endif // !defined(AFX_OLA32SRV_H__9FA9BC21_BEAF_4992_A4A7_B219252AF5B5__INCLUDED_)
