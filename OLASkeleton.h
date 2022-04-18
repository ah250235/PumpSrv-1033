// OLASkeleton.h: interface for the COLASkeleton class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLASKELETON_H__CEA08C3F_9C89_4966_8D1D_0445ACE84DB5__INCLUDED_)
#define AFX_OLASKELETON_H__CEA08C3F_9C89_4966_8D1D_0445ACE84DB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OLABase.h"

//The COLASkeleton class is a dummy class, designed to enable the use
//of base class features such as SwitchServer.
//This class should be used when no OLA server is defined (e.g. OLASrvInUse = 0)

class COLASkeleton : public COLABase  
{
public:
	COLASkeleton();
	virtual ~COLASkeleton();

	//Virtual functions
	//This functions are skeletal - they don't do anything.
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData){return 0;}
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL) { return 0;} //4.0.5.24	//4.0.20.52 63936
	virtual long AddParams(long lSessId, char *pInBuffer ) { return 0;}

	virtual long StartSession(char * pInBuffer, char *pszType) { return 0;}
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer) { return 0;}
	virtual long EndSession(long lSessId) { return 0;}
	
	virtual void Init() { return; }
	virtual void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE) { return; }

	//4.0.0.73
	virtual long GetOLAType() { return OLASRV_NOT_CONFIGURED; }

	//4.0.5.39
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;} 

};

#endif // !defined(AFX_OLASKELETON_H__CEA08C3F_9C89_4966_8D1D_0445ACE84DB5__INCLUDED_)
