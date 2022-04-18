// PriceChangeWithAckPerPump.h: interface for the CPriceChangeWithAckPerPump class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRICECHANGEWITHACKPERPUMP_H__8702C8AB_3F5A_414C_9959_BE1F761ED2C7__INCLUDED_)
#define AFX_PRICECHANGEWITHACKPERPUMP_H__8702C8AB_3F5A_414C_9959_BE1F761ED2C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct
{
	long				lGradeNumber;
	long				lErrorCode;
		
} PRODUCTS_PER_PUMP;

typedef struct
{
	BOOL				bIsPumpValid;
	long				lResultCode;
	short				nProductCounter;
	PRODUCTS_PER_PUMP	cProductsArr[MAX_POSITIONS_PER_PUMP];
		
} PRICE_CHANGE_ACK_PER_PUMP;



/******************************************************************************
 Class Description: This class handles the process of price change with Ack 
					from CL	per each pump.			  
 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			20/10/2009   13:00		Start	 4.0.19.820 - TD 78775
******************************************************************************/	
class CPriceChangeWithAckPerPump  
{
public:
	CPriceChangeWithAckPerPump();
	virtual ~CPriceChangeWithAckPerPump();

	long GetState();
	long GetFailureResult();
	void SetAckPerPumpProduct(long lPumpNumber, long lErrorCode);
	void StartProcess();
	void GetXmlResultBuffer(BSTR *pbstrBuffer);
	BOOL IsAckReceivedForAllPumps();
	void SetRefNumber(long lRefNumber);
	void HandleProcess();

private:
	void Init();

	long							m_lRefNumber;
	PriceChangeWithAckPerPumpState	m_eState;
	PriceChangeWithAckPerPumpError	m_eFailureResult;
	PRICE_CHANGE_ACK_PER_PUMP		m_cPriceChangeAckPerPump[MAX_PUMPS_64];
	SYSTEMTIME						m_tWaitResponseTimer;
	char							m_szResultBufferXml[MAX_RESULT_XML_BUF];

	BOOL IsGotResponseTimeout();
	void InitWaitResponseTimer(){ GetLocalTime(&m_tWaitResponseTimer); }
	void ResetWaitCLResponseTime(){ memset(&m_tWaitResponseTimer, 0, sizeof(SYSTEMTIME));}
	void HandleWaitCLResponseState();
	void SetResponseTimeoutError();
	BOOL IsProductFoundInArr(short nPump, short nGrade);
	void SetFinalResult(short nPumpNumber);
	BOOL SetXmlBuffer();

};

#endif // !defined(AFX_PRICECHANGEWITHACKPERPUMP_H__8702C8AB_3F5A_414C_9959_BE1F761ED2C7__INCLUDED_)
