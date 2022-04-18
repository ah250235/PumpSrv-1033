// OlaTravelCenter.h: interface for the COlaTravelCenter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLATRAVELCENTER_H__A42B8057_40A0_4AE0_8E3C_596994DA0B05__INCLUDED_)
#define AFX_OLATRAVELCENTER_H__A42B8057_40A0_4AE0_8E3C_596994DA0B05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OLABase.h"

#define MOSAIC_NUMBER_OF_IMPLEMENTED_METHODS 5


//4.0.9.602
class COLATravelCenter : public COLABase  
{
public:
	void SendAllPrompts(long &lOffset, char *pOutBuffer, PAY_AT_PUMP_INFO *pInfo);
	//void ParseItemProduct(long &lPumpNumber, CString *pInStr, CXMLInterface  *pInfo ,CPumpTransact *pTrs); //4.0.9999.0
	void ParseItemProduct(long &lPumpNumber, CString *pInStr, PAY_AT_PUMP_INFO  *pInfo ,CPumpTransact *pTrs); //4.0.9999.0

	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;} 
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData){return 0;}
	long EndSessionEx( long lSessID, long SessionResult);
	void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);
	long QuerySession(long lSessionNumber, BSTR * pOutBuffer);
	long EndSession(long lSessId);
	long AddParams(long lSessId, char * pInBuffer);
	void Init();
	long SessionPriceChange(char *pInStr , long * lOffset);
	COLATravelCenter();
	virtual ~COLATravelCenter();
	virtual long GetOLAType() { return OLASRV_TRAVEL_CENTER; }
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL) {return 0;} //4.0.20.52 63936
	virtual void AddPromptsRespons(long &lOffset, char *pOutBuffer, PAY_AT_PUMP_INFO *pInfo);

protected:
	long StartSession(char * pInBuffer, char *pszType);
	IUnknown * m_pUnknown;
	IDispatch * m_pDispatch;
	DISPID MethodIDs[MOSAIC_NUMBER_OF_IMPLEMENTED_METHODS];

	BOOL InitInterface();
	HRESULT GetFunctionPointers();


};

#endif // !defined(AFX_OLATRAVELCENTER_H__A42B8057_40A0_4AE0_8E3C_596994DA0B05__INCLUDED_)
