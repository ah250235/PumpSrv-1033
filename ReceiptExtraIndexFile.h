// ReceiptExtraIndexFile.h: interface for the CReceiptExtraIndexFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECEIPTEXTRAINDEXFILE_H__2762EF3E_FD82_4E98_8991_2651436FA447__INCLUDED_)
#define AFX_RECEIPTEXTRAINDEXFILE_H__2762EF3E_FD82_4E98_8991_2651436FA447__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DB.h"

class CReceiptExtraIndexFile : public CIndexFile  
{
public:
	long Operation(eFileOperation OpCode, void * pInfo , BOOL bIsSaveByMaint = FALSE); //4.0.18.20 //4.0.18.501
//4.0.18.501	long DeleteReceiptIndex(CXMLInterface *sRec, BYTE byIndex);
	long DeleteReceiptIndex(RECEIPT_INFO *sRec);
	long UpdateReceiptIndex(RECEIPT_INFO *sRec);
//	long UpdateReceiptIndex(CXMLInterface *sRec, BYTE byIndex);
	void SetReceiptExtraIndexFlags(DWORD lVal);
	BOOL GetReceiptExtraIndexFlags();
	CReceiptExtraIndexFile();
	virtual ~CReceiptExtraIndexFile();
//	long	SaveQDXMaintFile(FileOperation OpCode, void * pInfo); //4.0.18.20 //4.0.18.501
protected:
	long ParseBinFile(char * sFileName);

private:
	BOOL m_bActiveProcess;
};

#endif // !defined(AFX_RECEIPTEXTRAINDEXFILE_H__2762EF3E_FD82_4E98_8991_2651436FA447__INCLUDED_)
