// CarWash.h: interface for the CCarWash class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARWASH_H__743B6936_2B1F_11D4_9130_00C0F02C5991__INCLUDED_)
#define AFX_CARWASH_H__743B6936_2B1F_11D4_9130_00C0F02C5991__INCLUDED_

#include "commonDef.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBaseCarWash  //4.0.23.504 76432
{
public:
	
	CBaseCarWash();
	virtual ~CBaseCarWash();

	//Cuurrent request 
	void		 SetCurrentRequest(long lIndexNumber,long lRequestType,SYSTEMTIME *pcRequestSysTime = NULL);
	void		 GetCurrentRequest(long *plCurrentRequest,long *plRequestType,SYSTEMTIME *pcRequestSysTime = NULL);
	virtual void GetWashCode(long lSyncNumber, long *lResult, BSTR *sData, long *lFlags, long *pRetCode, long lPosNumber) = 0;
	virtual void GenerateWashCode(long lDevice, long lWashProgram, long lFlags, long *lSyncNumber, long *pRetCode, long lPosNumber) = 0;
	virtual BOOL CarWashSessionEmpty() = 0;
	virtual BOOL RequestWasSent(long lIndexNumber) = 0;
	virtual void LoadCarWashDefinition() = 0;
	virtual void CancelWashCode(long * lSyncNumber, long lDevice, long lFlags, BSTR sData,long lRefundCarWash, long *pRetCode, long lPosNumber) = 0;
	
protected:
	LONG				m_lCurrentRequest;
	LONG				m_lRequestType;
	SYSTEMTIME			m_cRequestSysTime;

	CCriticalSection	m_csCarWash;		//4.0.3.51
};


class CCarWash  : public CBaseCarWash	//4.0.23.504
{
public:
	CarWashStatus_ GetCarWashStatus();
	void		SetCarWashStatus(CarWashStatus_ eVal);
	
	BOOL		CarWashSessionEmpty();
	CCarWash();
	virtual ~CCarWash();
	void		LoadCarWashDefinition();
	void		InitCarWashLists();
	BOOL		CarWashEnabled();
	void		GetWashCode(long lSyncNumber, long *lResult, BSTR *sData, long *lFlags, long *pRetCode, long lPosNumber);
	void		GenerateWashCode(long lDevice, long lWashProgram, long lFlags, long *lSyncNumber, long *pRetCode, long lPosNumber);
	void		ValidateWashCode(long lDevice, long *lSyncNumber, long lWashCode, long lWashProgramID, long lFlags, long lPosNumber, long * pRetCode); //4.0.26.501 144221
	int         GetUnusedWashProgramIndex();
	int         FindWashProgramIndexByProdcutCode(std::string sProductCode);

	//Cuurrent request index
//	void SetCurrentRequest(long lIndexNumber,long lRequestType,SYSTEMTIME *pcRequestSysTime = NULL);
//	void GetCurrentRequest(long *plCurrentRequest,long *plRequestType,SYSTEMTIME *pcRequestSysTime = NULL);
	//Request list handeling
	void		AddNewRequest(CAR_WASH_INFO & cNewCarWashRequest); //4.0.26.501 144221
//	void		AddNewRequest(long lNewIndexnumber);
	long		GetRequestIndex();
	long		GetRequestType(); //4.0.26.501 144221
//	CAR_WASH_INFO &		GetLastRequest(); //4.0.26.501 144221

	BOOL		RequestsWaiting();
	BOOL		RequestWasSent(long lIndexNumber);
	void		RemoveRequest();
	void		RemoveRequest(long lIndexNumber);
	//Cancel list handeling
	void		AddCancelRequest(long lIndexNumber);
	void		RemoveCancelRequest();
	long		GetCancelRequest();
	long		GetCarWashRecord(CAR_WASH_INFO *pInfo, char *szCarWashCode);
	void		CancelWashCode(long * lSyncNumber, long lDevice, long lFlags, BSTR sData,long lRefundCarWash, long *pRetCode, long lPosNumber);

private:
	CList<CAR_WASH_INFO,CAR_WASH_INFO&>	m_cWarWashRequest;
	CList<long,long>	m_cCancelList;
	CarWashStatus_ m_eCarWashStatus;	//4.0.9.501
	BOOL				m_bCarWashEnabled;

};

class CCarWashGeneralRequest:public CBaseCarWash //4.0.23.504 76432
{
public:
	void		SetCarWashRequestStatus(CWRequestStatus eVal);
	CWRequestStatus GetCarWashRequestStatus();
	void		SetRequestBuffer(BSTR bstrIn);
	CCarWashGeneralRequest();
	virtual ~CCarWashGeneralRequest();
	void		GetWashCode(long lSyncNumber, long *lResult, BSTR *sData, long *lFlags, long *pRetCode, long lPosNumber);
	void		GenerateWashCode(long lDevice, long lWashProgram, long lFlags, long *lSyncNumber, long *pRetCode, long lPosNumber);
	long		GetDeviceNumber(){return m_lDevice;};
	void		SetDeviceNumber(long lVal){m_lDevice = lVal;};
	long		GetPosNumber(){return m_lPOSNumber;};
	//long		GetRequestTypeFlags(){return info.lRequestType;};
	BOOL		CarWashSessionEmpty();
	CWResult	GetCarWashResult(){return m_lResult;};
	void		SetCarWashResult(CWResult eVal){m_lResult = eVal;};
	BOOL		RequestWasSent(long lIndexNumber);
	void		SetRequestResponse(long lResult, long lDevice, BSTR sData, long lFlags);
	long		GetCurrentCarWashRequestFlags();
	void		SetCurrentCarWashRequestFlags(long lVal){m_lReturnedFlags = lVal;};
	void		LoadCarWashDefinition();
	DWORD		GetGeneralRequestTimeout(){return m_lGeneralRequestsTimeout;};
	long		GetReturnedFlags(){return m_lReturnedFlags;};
	void		SetReturnedFlags(long lVal){m_lReturnedFlags = lVal;};
	void		CancelWashCode(long * lSyncNumber, long lDevice, long lFlags, BSTR sData,long lRefundCarWash, long *pRetCode, long lPosNumber);


private:
	void		InitRequestInfo();
	BSTR				m_bstrResponseBuff;
	CWRequestStatus		m_lRequestStatus;
	CWResult			m_lResult;
	long				m_lDevice;
	long				m_lPOSNumber;
//	CAR_WASH_INFO		info;
	DWORD				m_lGeneralRequestsTimeout;
	long				m_lReturnedFlags;
};  

struct CAR_WASH_PROGRAM		// TD COSTCO-Tax
{
	char		sFullName[MAX_NAME_LENGTH];
	char		sShortName[MAX_NAME_LENGTH];
	char		sProductCode[MAX_CAR_WASH_PROD_CODE_LEN];
	long		lOlaProductCode;
	long		lMapTaxLink;
	long		lPrice;
	long		lValidEntry;
	long        lTaxRate;  // COSTCO
	long		lDiscount;//CR 105269
	long        lVolumeLimit;
	long		lDisplayOrder;//cr 404903
	BYTE		sUpc[15];
};

#endif // !defined(AFX_CARWASH_H__743B6936_2B1F_11D4_9130_00C0F02C5991__INCLUDED_)
