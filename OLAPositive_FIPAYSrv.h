// OLAPositive_FIPAYSrv.h: interface for the COLAPositive_FIPAYSrv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLAPOSITIVE_FIPAYSRV_H__4010AA54_9751_42D8_9583_46FCEC157038__INCLUDED_)
#define AFX_OLAPOSITIVE_FIPAYSRV_H__4010AA54_9751_42D8_9583_46FCEC157038__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OLA32.h"
#include "OLABase.h"

class COLAPositive_FIPAYSrv : public COLABase  
{
private:
	IServicesDll * m_pSrvPtr;
	IUnknown * m_pEventUnknown;  //4.0.0.38 Eyal

	//4.0.0.38 Eyal
	BOOL InitEventSink();

public:
	COLAPositive_FIPAYSrv();
	virtual ~COLAPositive_FIPAYSrv();

	//Virtual functions
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData) {return 0;} //4.0.6.506
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetDatalong, long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL) {return 0;} 	//4.0.20.52 (TD 63936)
	virtual long AddParams(long lSessId, char *pInBuffer );

	virtual long StartSession(char * pInBuffer, char *pszType);
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer);
	virtual long EndSession(long lSessId);
	
	virtual void Init();
	virtual void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);

	//4.0.0.73
	virtual long GetOLAType() { return OLASRV_POSITIVE_FIPAY; }
	//4.0.5.39
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;} 

};

#endif // !defined(AFX_OLAPOSITIVE_FIPAYSRV_H__4010AA54_9751_42D8_9583_46FCEC157038__INCLUDED_)
