#ifndef ___COLAEPSILON_H___
#define ___COLAEPSILON_H___

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// OLAEpsilon.h: interface for the COLAEpsilon class.
//
//////////////////////////////////////////////////////////////////////

#include "OLA32Srv.h"
 
class COLAEpsilon :	public COLA32Srv
{


public:	
	COLAEpsilon(void);
	virtual ~COLAEpsilon(void);

	void AddedPreAuthInformation(long lPumpNumber, long &lOffset, char *pOutBuffer, long & lOLAPumpServiceMode, long & lGradeRestrict);
	virtual long GetOLAType() { return OLASRV_EPSILON; }
	HRESULT GetFunctionPointers();
	long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags ,long *lSesionType, long lPumpNumber, PAY_AT_PUMP_INFO * pInPAPBuffer);
	long GetCardInfoEx(long lClientID, BSTR sInBuffer, BSTR *sRetData, long lFlags = 0);
	void Init();
	BOOL InitInterface();
	long StartSession(char * pInBuffer, char *pszType) override;

	virtual void AddPreAuthInformation(long lPumpNumber ,long &lOffset ,char *pOutBuffer , long & lOLAPumpServiceMode, long & lGradeRestrict);
	virtual void BuildGetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, char *pOutBuffer);
	virtual void BuildSetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, CPumpTransact *pTrs, char *pOutBuffer);
	virtual void BuildEpsilonFields(long lPumpIndex, long lTotalAmount, long &lOffset, char *pOutBuffer, PAY_AT_PUMP_INFO *pPAPInfo);

	// empty methods:
	virtual void AddItemsToOla( PAY_AT_PUMP_INFO * pInfo, long &lOffset, char * pOutBuffer, long lPumpNumber, short byOlaStat, long lValue, long &lVolume, long &lGradePrice, short &nGrade, unsigned long lFuelLoyalApp, long &lLoyaltyTotalAmount ) {return;}

	virtual void MatchSentLoyaltyCardInfoToOLA(PAY_AT_PUMP_INFO &pInOutPAPBuffer);

	HRESULT Login();
	HRESULT Logout();
	void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown /*= FALSE */);
	
	virtual long GetCustomerInfo(BSTR sInBuffer, BSTR *pRetData);
	//void BuildGetCustomerInfoRequest( long lPumpNumber, char * pInBuffer, char *pOutBuffer );
	
	OLECHAR FAR * sMethodName[12];
	OLECHAR m_sMethodName[32];

};

#endif // ___COLAEPSILON_H___