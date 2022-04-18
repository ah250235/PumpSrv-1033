// OLABase.h: interface for the COLABase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLABASE_H__D65B1A7D_7944_4772_8515_C69EEA35967D__INCLUDED_)
#define AFX_OLABASE_H__D65B1A7D_7944_4772_8515_C69EEA35967D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//A remote OLA server is designated by turning
//the M.S. (left-most) bit OLASrvInUse into one.
#define IS_OLA_REMOTE 0x80000000
#define NO_NEW_SERVER -1  //4.0.0.73 changed value from 0 to -1

#define MAX_OLA_RETRIES 10
#define MAX_OLA_BUFFER			10000 //4096     //4.0.5.12  //4.0.10.50

#include "ForecourtProductList.h"
#include "XMLInterface.h"


class COLABase  
{
protected:
	long	m_lOlaMaxDeclineMessToDisplay;
	BOOL	m_bValid;
	int		m_nDeclineCounter;     // 3.0.1.68
	BOOL	m_bOLAStopped;
	CRITICAL_SECTION	m_csOLASrv;
	CRITICAL_SECTION	m_csRetry;//4.0.3.38								//4.0.3.37	

	BYTE m_byRetryAttemps;       //4.0.2.15
	BYTE m_byConnectionAttempts; //4.0.2.37
	BOOL m_bNoMoreRetries;       //4.0.2.162
	BOOL m_bNoMoreConnectionRetries;
	BOOL m_bSupportIFSF;
	BOOL m_bPreAuthIFSF;
	BOOL m_bUseSessionEnd;	//4.0.9.509
	BOOL m_bAddPreAuthInfo; // TD 235737 4.0.27.501
	
public:	
	virtual long GetItemPrice(long lPumpNumber , PumpItems * pInfo , int iItem); // 4.0.23.1250
	virtual BOOL ConvertGradeItemsToIndex(long &lPumpNumber, long &lGrade, long &lIndex); // 4.0.23.1250
	long GetParamLenEx(char *sFiledName ,CString *pInStr, int iPos);
	void AddedCompletePriceInformation(long lPumpNumber, long &lOffset, char *pOutBuffer, long &lOLAPumpServiceMode, long &lGradeRestrict, long &lGrade); //4.0.28.500 - TD 238829
	long GetCardInfoSession(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData, long lFlags, long *lSesionType, long lPumpNumber);
	long GetCardInfoSessionEx(long lClientID,BSTR sInBuffer,BSTR *sRetData,long lFlags); //4.0.13.501
//4.0.20.503	66960 long GetCardInfoSessionEx(long lClientID,CXMLInterface* cXMLInterface,BSTR *sRetData,long lFlags); //4.0.13.501
	void TrimRightSpace(char *sbuffer, long lSize);
	virtual void AddPromptsRespons(long &lOffset, char * pOutBuffer, PAY_AT_PUMP_INFO *pInfo) {}
	void AddPriceInformation(long &lOffset, char *pOutBuffer);
	virtual long EndSessionEx(long lSessID ,long SessionResult); //4.0.9999.0
	BOOL GetEndSessionEx();
	void SetEndSessionEx(BOOL bVal);
	BOOL GetPreAuthIFSF();
	void SetPreAuthIFSF(BOOL bVal);
	BOOL ConvertNACSItemsToIndex(long & lPumpNumber,long & lNACSCode, long &lIndex);
	virtual void LoadAllProductSoldOnPump(long lPumpNumber);
	//virtual void ParseItemProduct(long &lPumpNumber, CString * pInStr, CXMLInterface  *pInfo ,CPumpTransact *pTrs); //4.0.9999.9
	virtual void ParseItemProduct(long &lPumpNumber, CString * pInStr, PAY_AT_PUMP_INFO  *pInfo ,CPumpTransact *pTrs); //4.0.9999.9
	BOOL	ParsePositionGrades(long &lPumpNumber, CString *pInStr, PAY_AT_PUMP_INFO  *pInfo,int i);//4.0.10.502
	virtual void AddedPreAuthInformation(long lPumpNumber ,long &lOffset ,char *pOutBuffer , long & lOLAPumpServiceMode, long & lGradeRestrict);
	virtual	BOOL IsAddPreAuthInfo(short sByOlaStat) const; // TD 235737 4.0.27.501

	void SetIFSFProtocoll(BOOL bVal);
	BOOL SupportIFSFProtocoll();
	void ReasetCounter();
	void CheckSlipFlags(CString *pInStr, BYTE & sSlipFlags);
	long GetGradePrice(short nGrade ,long & lPriceCash , long & lPriceCredit , long & lServiceMode);
	COLABase();
	virtual ~COLABase();

	virtual void BuildEpsilonFields(long lPumpIndex, long lTotalAmount, long &lOffset, char *pOutBuffer, PAY_AT_PUMP_INFO *pPAPInfo) {}
	virtual void Destruct(); //SR 362184: PumpSrv freeze on COLABase destructor
	BSTR m_pbsrOlaBuffer;
	long m_lSessionId;
	long m_lOlaRtc;
	char m_sOlaReqBuff[MAX_OLA_BUFFER];   // ola12345  //4.0.5.13
	long m_lQuerySessionResult[MAX_PUMPS_64];		//4.0.9.509 //4.0.9999.0
	long m_lSessionResult;  //4.0.9999.0	
	char m_sOlaReqType[50];	    // ola12345		
	CForecourtProductList m_cForecourtProductList;	 //4.0.7.507
	BOOL CheckIfStrIsReal(char * pszStr,CString sInStr,long * lIdx);
	
	//Load parameters from registry and start OLA thread.
	void LoadRegistryParams();
	void LaunchOLA();

	//Build an OLA request.
	long BuildRequest(short byOlaStat, long lPumpNumber, char * pInBuffer, char *pOutBuffer,CPumpTransact *pTrs , BOOL bBuildBufferToClient= FALSE); // .4.0.9.507

	virtual void AddItemsToOla( PAY_AT_PUMP_INFO * pInfo, long &lOffset, char * pOutBuffer, long lPumpNumber, short byOlaStat, long lValue, long &lVolume, long &lGradePrice, short &nGrade, unsigned long lFuelLoyalApp, long &lLoyaltyTotalAmount );
	void BuildTransAmount(long lPumpNumber , long lTotalAmount, short byOlaStat,PAY_AT_PUMP_INFO * pInfo ,long &lOffset, char * pOutBuffer);
	void BuildServiceMode( long lPumpIndex, IN OUT long & lOffset, char * pOutBuffer );
	void BuildItems(long lPumpIndex, CPumpTransact *pTrs, IN OUT long & lOffset, char * pOutBuffer);
	long BuildCarWashItem(long lPumpIndex, IN OUT long & lOffset, char * pOutBuffer, long &lTransTotalTax);
	long BuildFuelItem(long lPumpIndex, CPumpTransact *pTrs, IN OUT long & lOffset, char * pOutBuffer, long &lTransTotalTax);
	long BuildSessionType(short byOlaStat,char *sReqType);   //4.0.9.507
	
	//Parse data returned by OLA server.
	//void ParseReportData(CString *pInStr, CXMLInterface *pInfo, char *pText, long *plLen);
	void ParseReportData(CString *pInStr, PAY_AT_PUMP_INFO *pInfo, char *pText, long *plLen);
	void ParseCardData(BSTR sCardBuf, CARD_SALE_ALL3 *pCardData,long lPumpNumber); //4.0.0.75
	
	__int64 ParseData(long lState,CString *pInStr,PAY_AT_PUMP_INFO *pInfo,CPumpTransact *pTrs = NULL , short byOlaStat = 0, long lPumpNumber = 0); //4.0.16.503 //4.0.22.511 74797

	void ParseZoneFromOlaBuffer(long lPumpNumber,CString * pInStr, char * pParam, int iLen, short byOlaStat, PAY_AT_PUMP_INFO * pInfo );

	void SaveZone( PAY_AT_PUMP_INFO * pInfo, char * pParam, int iLen, int iZoneIndex = 0 );

	int  RemoveZone(RECEIPT_EXTRA_INFO_REC& Info, int iZoneIndex);

	void UpdateZoneIndex(PS::RECEIPT_EXTRA_INFO_REC &rec, int &iLen, int iZoneIndex, int iMaxLen);
	void ParseAmountLimit(CString * pInStr, char * pParam,PAY_AT_PUMP_INFO * pInfo);

	//4.0.18.501	long ParseData(long lState, CString *pInStr,CXMLInterface *pInfo, CPumpTransact *pTrs = NULL , short byOlaStat = 0, long lPumpNumber = 0); //4.0.16.503
	virtual void ParseNeedMoreParamData(CString *pInStr,PAY_AT_PUMP_INFO *pInfo, CPumpTransact *pTrs, __int64 *lRetCode); //4.0.18.12 //4.0.18.501 //4.0.22.511 74797
	
	
	//Get various state parameters.
	BOOL CanPrintLogMessages();
	BOOL IsDisplayMessageValid();
	BOOL Valid();
	BOOL OLAStopped();

	BYTE DecrementRetryAttemps();
	BYTE DecrementConnectionAttempts();   //4.0.2.37

	BYTE GetRetryAttempts() { return m_byRetryAttemps; }       //4.0.2.15
	BYTE GetConnectionAttempts() { return m_byConnectionAttempts; }

	void ResetRetryAttemps(BOOL bResetOnlyWhenZero = FALSE);   //4.0.2.15 4.0.2.32
	void ResetConnectionAttempts(BOOL bResetOnlyWhenZero = FALSE);

	BOOL NoMoreRetries() { return m_bNoMoreRetries; }     //4.0.2.162
	void SetNoMoreRetries(BOOL bRetriesLeft) { m_bNoMoreRetries = bRetriesLeft; }

	BOOL NoMoreConnectionRetries() { return m_bNoMoreConnectionRetries; }
	void SetNoMoreConnectionRetries(BOOL bRetriesLeft) { m_bNoMoreConnectionRetries = bRetriesLeft; } 
	

	//Handle various OLA parameters.
	BOOL GetParam(CString *pInStr,int iPos,char **ppParam,int *pLen, char *pFindStr = NULL);
	long GetParamLen(CString *pInStr,int iPos);
	long SetParam(char *pBuff,BYTE *pParamName);
	long SetParam(char *pBuff,BYTE *pParamName,BYTE *pParamFormat,BYTE *pParamVal,long lParamLen);
	long SetParam(char *pBuff,BYTE *pParamName,BYTE *pParamFormat,long lVal);
	long SetParam(char *pBuff,BYTE *pParamName,BYTE *pParamFormat,long lVal1, long lVal2);
	long SetParam(char *pBuff,BYTE *pParamName1,long lItemCount,BYTE *pParamName2,BYTE *pParamFormat,long lVal);
	long SetParam(char *pBuff,BYTE *pParamName1,long lItemCount,BYTE *pParamName2,BYTE *pParamFormat,long lVal1, long lVal2);
	long SetParamNoCheck(char *pBuff,BYTE *pParamName,BYTE *pParamFormat,BYTE *pParamVal,long lParamLen);
	void GetProductForSinglePump(long & lPumpNumber,PumpItems & cRec );//4.0.9.508
	
	//Recreate OLA service.
	BOOL RecreateService();

	//Start and Stop OLA.
	void StartOLA();
	void StopOLA();

	//Error handling.
	void Error(_com_error & e,BOOL bWriteToLog = TRUE);
	void Error(_com_error & e, HRESULT & hRes,BOOL bWriteToLog = TRUE);
	void Error(BOOL bWriteToLog = TRUE);

	//4.0.1.32
	void CatchCOMError(_com_error cError);
	virtual void SendAllPrompts(long &lOffset,char * pOutBuffer, PAY_AT_PUMP_INFO *pInfo){}
	//Virtual functions
	//virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData) = 0;
	virtual long GetCardInfo(BSTR sAccountNumber, BSTR sTrack1, BSTR sTrack2, BSTR sShortCardName, BSTR *pRetData,long lFlags = 0,long *lSesionType=0, long lPumpNumber=0, PAY_AT_PUMP_INFO * pInPAPBuffer = NULL) = 0;  //4.0.5.24	//4.0.20.52 63936
	virtual long GetCardInfoEx(long lClientID,BSTR sInBuffer,BSTR *sRetData,long lFlags = 0){return 1;};//4.0.11.500

	virtual long AddParams(long lSessId, char *pInBuffer ) = 0;

	virtual long StartSession(char * pInBuffer, char *pszType) = 0;
	virtual long QuerySession(long lSessionNumber, BSTR *pOutBuffer) = 0;
	virtual long EndSession(long lSessId) = 0;
	
	virtual void Init() = 0;
	virtual void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE) = 0; //4.0.20.56

	//4.0.0.73
	virtual long GetOLAType() = 0;

	virtual long Login(PumpLoginOp ePumpLoginOp, long lPumpNumber) = 0; //4.0.5.39
	virtual long UploadBuffer(long lTerminalId, long lPumpNumber, long lSessionId, long lUploadType, BSTR sInBuffer, BSTR *sOutBuffer, long lExDataSize, BSTR sExData) = 0;

	virtual void MatchSentLoyaltyCardInfoToOLA(PAY_AT_PUMP_INFO &pInOutPAPBuffer){};  //4.0.23.507 89548
	virtual void MatchReceivedLoyaltyCardInfoToOLA(CARD_SALE_ALL3 &pInOutPAPBuffer){};  //4.0.23.507 89548

	virtual void BuildGetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, char *pOutBuffer) { ; };

	virtual void BuildSetCustomerInfoRequest(long lPumpNumber, char * pInBuffer, CPumpTransact *pTrs, char *pOutBuffer) {};
	virtual long GetCustomerInfo(BSTR sInBuffer, BSTR *pRetData) { return FALSE; };

	virtual long GetHashedPAN(const long lPumpNumber, const char* pszOpenBuffIn, char* psHashedBuffOut, size_t* piHashedDataSize){return -1;};		//4.0.27.44 TD 179125
	long GetOdometerFromCardSale( CARD_SALE_EXTRA_DATA4_ * pCardDataExtra4, CARD_SALE_EXTRA_DATA * pCardDataExtra, CARD_SALE_DATA * pCardData );
	int getLastZoneIndex(CString filer);  //4.0.33.302
};

#endif // !defined(AFX_OLABASE_H__D65B1A7D_7944_4772_8515_C69EEA35967D__INCLUDED_)
