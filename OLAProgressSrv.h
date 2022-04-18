// OLAProgressSrv.h: interface for the COLAProgressSrv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLAPROGRESSSRV_H__E619B549_E5BB_4380_A8B1_B6DB435B8EBB__INCLUDED_)
#define AFX_OLAPROGRESSSRV_H__E619B549_E5BB_4380_A8B1_B6DB435B8EBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "OLA/Ola.dll" no_namespace named_guids

#include "OLABase.h"

class COLAProgressSrv : public COLABase  
{
protected://4.0.12.505
	IServicePtr m_pSrvPtr;

public:
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData) {return 0;}
	COLAProgressSrv();
	virtual ~COLAProgressSrv();

	//Virtual functions
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL) {return 0;}	//4.0.20.52 63936
	virtual long AddParams(long lSessId, char *pInBuffer );
	
	virtual long StartSession(char * pInBuffer, char *pszType);
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer);
	virtual long EndSession(long lSessId);
	
	virtual void Init();
	virtual void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);

	//4.0.0.73
	virtual long GetOLAType() { return OLASRV_PROGRESS ;}

	//4.0.5.39
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;} 

};

#endif // !defined(AFX_OLAPROGRESSSRV_H__E619B549_E5BB_4380_A8B1_B6DB435B8EBB__INCLUDED_)
