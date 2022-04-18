// ChangeProductInTank.h: interface for the CChangeProductInTank class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANGEPRODUCTINTANK_H__8E480455_F464_439D_951E_166BEC20A2B2__INCLUDED_)
#define AFX_CHANGEPRODUCTINTANK_H__8E480455_F464_439D_951E_166BEC20A2B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************************************************************************
*******************************************************************************
			Classes:
					CChangeProductInTankSetup
					CChangeProductInTankProcess
					CChangeProductInTankManager 

Those classes are responsible for managing the process of changing product in tank
4.0.22.503 - TD 72189
 
*******************************************************************************
******************************************************************************/

#define MAX_RESULT_XML_BUF					1200
#define TIME_OUT_WAIT_CL_RESPONSE			20000 

//4.0.23.501 - TD 76330
typedef struct
{
	BYTE byGradeFullName[MAX_FIELD_VAL];		
	BYTE byGradeShortName3[STR_LONG_CODE + 1];
	BYTE byGradeShortName5[STR_LONG_VAL6];
	BYTE byAdditionalProductCode[MAX_STRING_LEN];
	long lPriceCashSelf;
	long lPriceCashFull;
	long lPriceCreditSelf;
	long lPriceCreditFull;
		
} NEW_GRADE_SETUP;

class CChangeProductInTankSetup
{
public:
	CChangeProductInTankSetup();
	virtual ~CChangeProductInTankSetup();

	long HandleRequest(BSTR sRec); 
	long HandleRequestSetupOnly(BSTR sRec);
	void GetChangeProductInTankInfo(BSTR *pbstrData);
	void ChangeSetup();
	
	inline long GetGradeNumber() {return m_lGradeNumber;};
	inline long GetTankId() {return m_lTankNumber;};
	inline char* GetProductName(){return m_szProductName;};

	inline void SetGradeNumber(long lGradeId) {m_lGradeNumber = lGradeId;};
	inline void SetTankId(long lTankId) {m_lTankNumber = lTankId;};
	inline void SetProductName(char * sProductName){memcpy(&m_szProductName, sProductName, min(sizeof(m_szProductName), strlen(sProductName)));};

private:
	long ParseData(BSTR sRec);
	long CheckValidTankGrade();	
	long CheckOfflinePumps();								//4.0.23.501 - TD 78465
	long CreateNewProduct(BSTR sXmlRec);					//4.0.23.501 - TD 76330
	long ParseNewProductData(BSTR sXmlRec);					//4.0.23.501 - TD 76330
	short FindUnusablePureProduct();						//4.0.23.501 - TD 76330
	void UpdateNewGrade(short nPureProduct);				//4.0.23.501 - TD 76330
	void UpdateNewPureProduct(short nPureProduct);			//4.0.23.501 - TD 76330
	void UpdateMaxValidGradeNumber();						//4.0.23.504 - TD 75226
	short CheckPureProductExist();							//4.0.24.70 - TD 111683
	void ClearNewGradeDetails();							//4.0.25.30 - TD 127181
	void FindNonValidGrade(long &lGradeId);					//4.0.26.501 - TD 116227

private:
	long m_lTankNumber;
	long m_lGradeNumber;
	long m_lOldGradeNumber; //4.0.23.500
	char m_szProductName[MAX_NAME_LENGTH];
	BOOL m_bIsNewProduct;	//4.0.23.501 - TD 76330
	long m_lExtGradeNumber; //4.0.26.501 - TD 116227

	NEW_GRADE_SETUP m_cNewGradeDetails; //4.0.23.501 - TD 76330
};

class CChangeProductInTankProcess
{
public:
	CChangeProductInTankProcess();
	virtual ~CChangeProductInTankProcess();

	void HandleProcess(CChangeProductInTankSetup &cSetup);
	void SetState(const long lState);
	void SetProductInTankState(const long lState);
	long GetState();
	void SetFailureResult(const long lError);
	long GetFailureResult();
	void GetResultBuffer(BSTR *pbstrBuffer);
	void ClearResultBufferXml();

private:
	CRITICAL_SECTION				m_csChangeProductInTank;

	ChangeProductInTankState		m_eState;
	ChangeProductInTankError		m_eFailureResult;

	char							m_szResultBufferXml[MAX_RESULT_XML_BUF];

	SYSTEMTIME						m_tWaitCloseOpenSalesTime;
	SYSTEMTIME						m_tWaitCLResponseTime;

	void InitWaitCloseOpenSalesTime(){ GetLocalTime(&m_tWaitCloseOpenSalesTime); }
	void ResetWaitCloseOpenSalesTime(){ memset(&m_tWaitCloseOpenSalesTime, 0, sizeof(SYSTEMTIME));}
	BOOL IsTimeOutWaitCloseOpenSales();	
	
	void InitWaitCLResponseTime(){ GetLocalTime(&m_tWaitCLResponseTime); }
	void ResetWaitCLResponseTime(){ memset(&m_tWaitCLResponseTime, 0, sizeof(SYSTEMTIME));}
	BOOL IsTimeOutWaitCLResponse();	
	
	BOOL CheckOpenSales();
	void HandleStartLockState();
	void HandleForecourtLockedState();
	void HandleWaitCloseOpenSalesState();
	void HandleChangeSetupAndSendToCLState(CChangeProductInTankSetup &cSetup);
	void HandleWaitCLResponseState();
	void HandleCLResponseState();
	void HandleSendPriceChangeState(CChangeProductInTankSetup &cSetup);
	BOOL SetOfflinePumpsXml(CChangeProductInTankSetup &cSetup);
};


class CChangeProductInTankManager  
{

public:
	long StartProcess(BSTR bstrRec, BOOL bSetupOnly);
	void HandleProcess();
	void GetChangeProductInTankInfo(BSTR *pbstrData) {cChangeProductInTankSetup.GetChangeProductInTankInfo(pbstrData);};
	void GetChangeProductInTankXmlResultBuffer(BSTR *pbstrBuffer) {cChangeProductInTankProcess.GetResultBuffer(pbstrBuffer);};

	void SetState(const long lState) {cChangeProductInTankProcess.SetState(lState);};
	void SetProductInTankState(const long lState) {cChangeProductInTankProcess.SetProductInTankState(lState);};//4.0.27.20 TD 147183	
	long GetState() {return cChangeProductInTankProcess.GetState();};
	void SetFailureResult(const long lError) {cChangeProductInTankProcess.SetFailureResult(lError);};
	long GetFailureResult() {return cChangeProductInTankProcess.GetFailureResult();};
	
	void SetGradeId(long lGradeNumber) {cChangeProductInTankSetup.SetGradeNumber(lGradeNumber);};
	void SetTankId(long lTankNumber) {cChangeProductInTankSetup.SetTankId(lTankNumber);};
	void SetProductName(char * sProductName) {cChangeProductInTankSetup.SetProductName(sProductName);};

	CChangeProductInTankSetup	cChangeProductInTankSetup;
	CChangeProductInTankProcess cChangeProductInTankProcess;
};

#endif // !defined(AFX_CHANGEPRODUCTINTANK_H__8E480455_F464_439D_951E_166BEC20A2B2__INCLUDED_)
