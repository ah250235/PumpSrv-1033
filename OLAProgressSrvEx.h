//class the implement the Tesco OLA
//4.0.9.503
#if !defined(AFX_OLALOCALSRVEX_H__6857A0AA_0206_4469_8D93_10AF24938C0B__INCLUDED_)
#define AFX_OLALOCALSRVEX_H__6857A0AA_0206_4469_8D93_10AF24938C0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#import "OLA/OlaInterface.exe" named_guids
using namespace OLAINTERFACELib;

//4.0.12.505
#include "OLABase.h"

class COLAProgressSrvEx : public COLABase
{
protected:
	OLAINTERFACELib::IOLAServicePtr m_pSrvPtr;
public:
	void Init();
	long GetCardInfoEx(long lClientId, BSTR  sData, BSTR *pRetData,long lFlags = 0); //4.0.12.505
	void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);//4.0.20.56

	COLAProgressSrvEx()
	{
		m_bAddPreAuthInfo = TRUE; // 4.0.27.501 TD 235737
	}

	virtual ~COLAProgressSrvEx()
	{
	}
	
	
	//Virtual functions
	virtual long AddParams(long lSessId, char *pInBuffer );
	virtual long StartSession(char * pInBuffer, char *pszType);
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer);
	virtual long EndSession(long lSessId);
	
	virtual long GetOLAType() { return OLASRV_PROGRESS_EX;}

	// Function not Implemented
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;} 
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData) {return 0;}
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL);		//4.0.20.52 63936	//4.0.21.21	 TD-69389
};


#endif  //_OLA_TESCO_H