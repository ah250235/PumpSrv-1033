// COLATcpBase.h: interface for the COLATcpBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPOLA_H__92E3879D_000D_4845_900E_50A1922BDC16__INCLUDED_)
#define AFX_TCPOLA_H__92E3879D_000D_4845_900E_50A1922BDC16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OLABase.h"

typedef enum TcpOlaRetCode
{
	TCPOLA_OK,
	BAD_PARAM_COUNT,
	BAD_PARAM,
	BAD_MEMBER_NAME,
	INVOKE_EXCEPTION,
	RETRY,
	NAMED_ARGS_NOT_SUPPORTED,
	DISP_OVERFLOW,
	PARAM_NOT_FOUND,
	TYPE_MISMATCH,
	GENERAL_ERROR
} TcpOlaRetCode;

class COLATcpBase : public COLABase  
{
private:
	IDispatch * m_pCommonDispatch;
	IUnknown * m_pCommonUnknown;
	IControlSrvPtr m_pCtrlPtr;
	OLECHAR m_sMethodName[32];

	BOOL InitInterface();
	
	long AnalyzeInvokeResult(OLECHAR FAR * sMethodName , HRESULT hRes , EXCEPINFO& cException , unsigned int iErrArg);
	long Call(OLECHAR FAR * sMethodName , CComVariant * cArgBuffer , int iArgBufferLen , VARIANT& cRes , EXCEPINFO& cException);
	
public:
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData){return 0;}
	COLATcpBase(OLECHAR FAR * sMethodName);
	virtual ~COLATcpBase();

	void SwitchServer();

	//Virtual functions
	virtual long CreateRegEntries(BYTE * sEntry) {return 0;}

	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL); //4.0.20.52 63936
	virtual long AddParams(long lSessId , char * pInBuffer);

	virtual long StartSession(char * pInBuffer, char *pszType);
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer);
	virtual long EndSession(long lSessId);
	
	virtual void Init();
	virtual void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);

	//4.0.2.29
	virtual long GetOLAType() { return OLASRV_NOT_CONFIGURED | IS_OLA_REMOTE; }
	//4.0.5.39
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;} 
};

#endif // !defined(AFX_TCPOLA_H__92E3879D_000D_4845_900E_50A1922BDC16__INCLUDED_)
