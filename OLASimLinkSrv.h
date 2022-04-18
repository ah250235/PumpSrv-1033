// OLASimLinkSrv.h: interface for the COLASimLinkSrv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLASIMLINKSRV_H__63300CEE_8DCB_4BEE_B3E2_B0D4D05B57C2__INCLUDED_)
#define AFX_OLASIMLINKSRV_H__63300CEE_8DCB_4BEE_B3E2_B0D4D05B57C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "OLA/SimLinkSrv.exe" named_guids
using namespace SimLinkSrv;

#include "OLABase.h"

class COLASimLinkSrv : public COLABase  
{
protected:
    SimLinkSrv::IServicesPtr m_pSrvPtr;

public:	
	COLASimLinkSrv();
	virtual ~COLASimLinkSrv();

	//Virtual functions
	
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL); //4.0.20.52 (TD 63936)
	virtual long GetCardInfoEx(long lClientID,BSTR sInBuffer,BSTR *sRetData,long lFlags /* = 0 */);//4.0.11.500
	virtual long AddParams(long lSessId, char *pInBuffer );

	virtual long StartSession(char * pInBuffer, char *pszType);
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer);
	virtual long EndSession(long lSessId);

	virtual void Init();
	virtual void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);
	virtual long GetOLAType() { return OLASRV_SIM_LINK_SRV; }
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;} 
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData) {return 0;}

	virtual void AddPreAuthInformation(long lPumpNumber ,long &lOffset ,char *pOutBuffer , long & lOLAPumpServiceMode, long & lGradeRestrict);
	void BuildGetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, char *pOutBuffer);
	void BuildSetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, CPumpTransact *pTrs, char *pOutBuffer);
	virtual void BuildEpsilonFields(long lPumpIndex, long lTotalAmount, long &lOffset, char *pOutBuffer, PAY_AT_PUMP_INFO *pPAPInfo);
	virtual void AddItemsToOla(PAY_AT_PUMP_INFO * pInfo, long &lOffset, char * pOutBuffer, long lPumpNumber, short byOlaStat, long lValue, long &lVolume, long &lGradePrice, short &nGrade, unsigned long lFuelLoyalApp, long &lLoyaltyTotalAmount) { return; }
};

#endif // !defined(AFX_OLASIMLINKSRV_H__63300CEE_8DCB_4BEE_B3E2_B0D4D05B57C2__INCLUDED_)
