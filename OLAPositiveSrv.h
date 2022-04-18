// OLAPositiveSrv.h: interface for the COLAPositiveSrv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLALOCALSRV_H__6857A0AA_0206_4469_8D93_10AF24938C0B__INCLUDED_)
#define AFX_OLALOCALSRV_H__6857A0AA_0206_4469_8D93_10AF24938C0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "OLA/OLASrv.exe" auto_rename no_namespace named_guids


#include "OLABase.h"

class COLAPositiveSrv : public COLABase  
{
protected:
	IServicesPtr m_pSrvPtr;

public:	
	COLAPositiveSrv();
	virtual ~COLAPositiveSrv();

	//Virtual functions
	
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL); //4.0.20.52 (TD 63936)
	virtual long GetCardInfoEx(long lClientID,BSTR sInBuffer,BSTR *sRetData,long lFlags /* = 0 */);//4.0.11.500
	virtual long AddParams(long lSessId, char *pInBuffer );

	virtual long StartSession(char * pInBuffer, char *pszType);
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer);
	virtual long EndSession(long lSessId);

	virtual void Init();
	virtual void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);

	//4.0.0.73
	virtual long GetOLAType() { return OLASRV_POSITIVE; }
	//4.0.5.39
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;}
	//4.0.20.500 TD 27377 Changed function to be a virtual function
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData);

};

#endif // !defined(AFX_OLALOCALSRV_H__6857A0AA_0206_4469_8D93_10AF24938C0B__INCLUDED_)
