// Report.h: interface for the CReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORT_H__9A12F378_6483_11D5_AC10_000102191E6D__INCLUDED_)
#define AFX_REPORT_H__9A12F378_6483_11D5_AC10_000102191E6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_REPORTS 50

//Report Status (in ReportArray) 
#define RS_EMPTY		0
#define RS_WAIT_TO_SEND	1
#define RS_SENT			2
#define RS_RECEIVED		3

//#define REPORT_TEXT_LENGTH 4000

class CReport  
{
public:
	void SetRequestStatus(unsigned char nNewStatus);
	short GetRequestStatus();
	void ClearOlaStat();
	void GetPAPInfo(PAY_AT_PUMP_INFO *pPAPInfo);
//	void GetPAPInfo(CXMLInterface *pPAPInfo); //4.0.14.500
	void SetPAPInfo(PAY_AT_PUMP_INFO *pPAPInfo);
	void SetPAPInfo(CXMLInterface *pPAPInfo); //4.0.14.500
	void GetOlaStat(OLA_STAT * pOlaStat);
	void SetOlaStat(OLA_STAT * pOlaStat);

	CReport();
	virtual ~CReport();

protected:	
	BYTE				m_cRequestStatus;
	char				m_cPAPInfo[MAX_XML_BUF_LEN];
	//PAY_AT_PUMP_INFO 	m_cPAPInfo;
	OLA_STAT			m_cOla;

};

#endif // !defined(AFX_REPORT_H__9A12F378_6483_11D5_AC10_000102191E6D__INCLUDED_)
