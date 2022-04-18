// OLAEserviceSrv.h: interface for the COLAEservice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLAESERVICESRV_H__2E08C902_2CE9_4635_BD67_C651442A2E3F__INCLUDED_)
#define AFX_OLAESERVICESRV_H__2E08C902_2CE9_4635_BD67_C651442A2E3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



 #import "SPEftAdapter.tlb" no_namespace named_guids

 #include "OLABase.h"


 class COLAEserviceSrv : public COLABase 
{
protected:

	ISPEftAdapterLinkServerPtr m_pSrvPtr;

public:
	COLAEserviceSrv();
	virtual ~COLAEserviceSrv();

	

	//Virtual functions
	
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL); 
	virtual long GetCardInfoEx(long lClientID,BSTR sInBuffer,BSTR *sRetData,long lFlags /* = 0 */);
	virtual long AddParams(long lSessId, char *pInBuffer );

	virtual long StartSession(char * pInBuffer, char *pszType);
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer);
	virtual long EndSession(long lSessId);

	virtual void Init();
	virtual void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);
	virtual long GetOLAType() { return OLASRV_EService_SRV; }
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;}
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData);
    
};

#endif // !defined(AFX_OLAESERVICESRV_H__2E08C902_2CE9_4635_BD67_C651442A2E3F__INCLUDED_)
