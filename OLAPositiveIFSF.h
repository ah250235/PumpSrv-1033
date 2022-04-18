// OLAPositiveIFSF.h: interface for the COLAPositiveIFSF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLAPOSITIVEIFSF_H__57AB8F15_1C28_44FC_B33B_E89B2D726721__INCLUDED_)
#define AFX_OLAPOSITIVEIFSF_H__57AB8F15_1C28_44FC_B33B_E89B2D726721__INCLUDED_

#include "ForecourtProductList.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PumpClass.h"
#include "OLAPositiveEventSrv.h"

class COLAPositiveIFSF : public COLAPositiveEventSrv  
{
public:
	//void LoadAllProductSoldOnPump(long lPumpNumber); //4.0.7.507
	//void ParseItemProduct(long &lPumpNumber, CString *pInStr, PAY_AT_PUMP_INFO *pInfo); //4.0.7.507
	virtual void AddPreAuthInformation(long lPumpNumber, long & lOffset, char *pOutBuffer, long & lOLAPumpServiceMode, long & lGradeRestrict);
	void Init();
	//CForecourtProductList m_cForecourtProductList; //4.0.7.507	 
	COLAPositiveIFSF();
	virtual ~COLAPositiveIFSF();
	virtual long GetOLAType() { return OLASRV_POSITIVE_IFSF; }
	//4.0.5.39
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber); 
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData) {return 0;} //4.0.6.506
	virtual void MatchSentLoyaltyCardInfoToOLA(PAY_AT_PUMP_INFO &pInOutPAPBuffer);  //4.0.23.121 89548 //4.0.24.60 109616
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL){return 1;}; //4.0.23.240 109616  //4.0.24.60
	virtual long GetCardInfoEx(long lClientID,BSTR sInBuffer,BSTR *sRetData,long lFlags /* = 0 */){return 1;};//4.0.23.240 109616 //4.0.24.60

private:
	//BOOL ConvertNACSItemsToIndex(long & lPumpNumber,long &lNACS,long &lIndex); //4.0.7.507
};

#endif // !defined(AFX_OLAPOSITIVEIFSF_H__57AB8F15_1C28_44FC_B33B_E89B2D726721__INCLUDED_)
