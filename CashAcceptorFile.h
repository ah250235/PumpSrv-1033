// CashAcceptorFile.h: interface for the CCashAcceptorFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASHACCEPTORFILE_H__695DACAB_6AB6_4849_8346_2DD7E0775836__INCLUDED_)
#define AFX_CASHACCEPTORFILE_H__695DACAB_6AB6_4849_8346_2DD7E0775836__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DB.h"
#include "XMLInterface.h" //4.0.16.502 - TD 35765


#define MAX_CASH_ACCEPTOR_RECORD_SHIFT  100


class CCashAcceptorFile : public CIndexFile
{
public:
	BOOL DeleteOLAPeriod();
	void CreateFindFirstIndex(long lTerminalID, BYTE *sKeyIndex , BOOL bCurrentPeriod=TRUE);   //4.0.11.10
	long MarkRemovalOnterminal(long lTerminalId);
	long UpdateBillsCurrent(long lTerminal ,CASH_ACCEPTOR_REC  &cCashRec);
	void CreateCurrentIndex(long lTerminalID , BYTE *sKeyIndex, BOOL bCurrentPeriod=TRUE ,BOOL bAddDate=TRUE);
	void ConvertCahAcceptorRec2TagRec(CASH_ACCEPTOR_REC &sRec, TAG_CASH_ACCEPTOR & sTag ,BYTE sRecordType='0');
	void ConvertCahAcceptorRec2TagHead(CASH_ACCEPTOR_HEADER_REC  & sRec , TAG_CASH_ACCEPTOR_HEADER & sTag);
	long WriteCashAcceptorRecord(long lTerminalId, CASH_ACCEPTOR_REC &cCashAcceptorRec);
	long ReadCashAcceptorRecord(long lTerminalId ,CASH_ACCEPTOR_REC &cCashAcceptorRec);
	long GetCashAcceptorReport__(long lTerminalId, long lFlags, BSTR *sHeader, BSTR *sRecords, long *lRecordCount);
	long GetCashAcceptorReportXml__(long lTerminalId, long lFlags, BSTR *sHeader, BSTR *sXmlData, long *lRecordCount, BOOL bCheckRemovalDrawers); //4.0.16.502 - TD 35765 //4.0.17.501
	void ConvertCashAcceptorRec2Xml(CASH_ACCEPTOR_REC & sRec, CXMLInterface *pXmlData, CXMLPointers	*pXMLPointers, BYTE sRecordType='0'); //4.0.16.502 - TD 35765
	void ConvertCashAcceptorRecHeader2Xml(CASH_ACCEPTOR_HEADER_REC & sRec, CXMLInterface *pXmlData, CXMLPointers *pXMLPointers); //4.0.16.502 - TD 35765
	void CreateCashAcceptorTotalDays(CASH_ACCEPTOR_REC & sRec, CXMLInterface *pXmlData, CXMLPointers *pXMLPointers); //4.0.17.30 - TD 35765
	BOOL CheckRemovalDrawers(char *sOpenDrawersList); //4.0.11.505
	long Operation(eFileOperation OpCode, void *pData, BOOL bIsSaveByMaint = FALSE); //4.0.18.20
//	long	SaveQDXMaintFile(FileOperation OpCode, void * pInfo); //4.0.18.20

	void ClearHistoryParameters(CASH_ACCEPTOR_REC &cCashAcceptorRec);
	long CheckSignitureFile();
	long ReadHeaderFile(CASH_ACCEPTOR_HEADER_REC & cCashAcceptorHeaderRec);
	long WriteHeaderFile();
	void MoveShift();
	long ReadRecord(long  *lTerminalId, CASH_ACCEPTOR_REC  * sRec);
	long WriteRecord(long * lTerminalId ,CASH_ACCEPTOR_REC *sRec);
	CCashAcceptorFile();
	virtual ~CCashAcceptorFile();
protected:
	long ParseBinFile(char * sFileName);
private:
	CRITICAL_SECTION	m_csCashAcceptorFile; //4.0.16.502
};

#endif // !defined(AFX_CASHACCEPTORFILE_H__695DACAB_6AB6_4849_8346_2DD7E0775836__INCLUDED_)
