// DebitInfo.h: interface for the CDebitInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBITINFO_H__58AF0C93_5C12_11D6_BB60_00500470AD8E__INCLUDED_)
#define AFX_DEBITINFO_H__58AF0C93_5C12_11D6_BB60_00500470AD8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDebitInfo  
{
public:
	void SetPendingInitDebitSesssion(BOOL bVal);
	BOOL GetPendingInitDebitSession();
	void ClearDebitInfo();
	void GetData(BYTE *byLen, BYTE *byData);
	void UpdateData( BYTE *byLen, BYTE *byData);
	void ConvertPAP2DebitInfo( PAY_AT_PUMP_INFO *pPAPInfo , long lFlags=0);
	void ConvertPAP2DebitInfo(CXMLInterface *pPAPInfo , long lFlags=0);
	void ConvertDebitInfo2PAP(PAY_AT_PUMP_INFO *pPAPInfo , long lPumpNumber);	//4.0.18.501
//	void ConvertDebitInfo2PAP(CXMLInterface *pPAPInfo , long lPumpNumber);
	void GetDebitInfo(OLA_DEBIT_PROCESS_INFO *pDebitInfo);
	void SetDebitInfo(OLA_DEBIT_PROCESS_INFO *pDebitInfo);
	void SetOlaStat(OLA_STAT * pOlaStat);
	void GetOlaStat(OLA_STAT * pOlaStat);
//	void SetPAPInfo(PAY_AT_PUMP_INFO *pPAPInfo);
//	void GetPAPInfo(PAY_AT_PUMP_INFO *pPAPInfo);
	void ClearOlaStat();
	CDebitInfo();
	virtual ~CDebitInfo();	

protected:	
	//PAY_AT_PUMP_INFO 		m_cPAPInfo;
	OLA_STAT				m_cOla;
	OLA_DEBIT_PROCESS_INFO 	m_cDebitProcessInfo;
	BOOL					m_bPendingInitDebitProcess; // 4.0.5.12
};

#endif // !defined(AFX_DEBITINFO_H__58AF0C93_5C12_11D6_BB60_00500470AD8E__INCLUDED_)
