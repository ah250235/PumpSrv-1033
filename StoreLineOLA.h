// StoreLineOLA.h: interface for the CStoreLineOLA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORELINEOLA_H__6374DDC1_F372_4FFD_B005_77AF8D432936__INCLUDED_)
#define AFX_STORELINEOLA_H__6374DDC1_F372_4FFD_B005_77AF8D432936__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CStoreLineOLA : public COLABase  
{
	
protected:
	OLASTORELINELib::IOLAServicePtr m_pSrvPtr;

public:
	CStoreLineOLA();
	virtual ~CStoreLineOLA();

	long GetCardInfoEx(long lClientId, BSTR  sData, BSTR *pRetData,long lFlags = 0); //4.0.12.505
	void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);
	
	void Init();
	virtual long GetOLAType() { return OLASRV_STORE_LINE;}
	BOOL CreateInitXML(char * strOutXml);
	BOOL CreateOLAEvent();
	void HandleStatusEvent(BSTR sInBuffer);

	//Virtual functions
	virtual long AddParams(long lSessId, char *pInBuffer );
	virtual long StartSession(char * pInBuffer, char *pszType);
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer);
	virtual long EndSession(long lSessId);
	
	// Function not Implemented
	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) {return 0;} //not in use here
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData) {return 0;}
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL);	//4.0.20.52 63936
	void ParseNeedMoreParamData(CString *pInStr,PAY_AT_PUMP_INFO *pInfo, CPumpTransact *pTrs, long *lRetCode); //4.0.18.12

	
	virtual long GetHashedPAN(const long lPumpNumber, const char* pszOpenBuffIn, char* psHashedBuffOut, size_t* piHashedDataSize);		//4.0.27.44 TD 179125
};

#endif // !defined(AFX_STORELINEOLA_H__6374DDC1_F372_4FFD_B005_77AF8D432936__INCLUDED_)
