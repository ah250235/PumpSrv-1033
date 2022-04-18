// DeviceRecord.h: interface for the CDeviceRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICERECORD_H__0E254E25_009B_462E_8A89_291C9F07990C__INCLUDED_)
#define AFX_DEVICERECORD_H__0E254E25_009B_462E_8A89_291C9F07990C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum Index
{
	BLENDERS,
	CARD_READERS,
	RFIDS,
	SCANNERS,
	CASH_ACCEPTORS,
	END_OF_LIST     //This enum member MUST ALWAYS BE LAST
	
} CDeviceRecordIndex;

class CDeviceRecord  
{
public:
	BOOL CanUpdate(long lSource);
	CDeviceRecord();
	virtual ~CDeviceRecord();

	void SetNumberOfBlenders();
	void SetNumberOfBlenders(BYTE byNum);
	void SetNumberOfCardReaders(BYTE byNum);
	void SetNumberOfRFID(BYTE byNum);
	void SetNumberOfScanners(BYTE byNum);
	void SetNumberOfCashAcceptors(BYTE byNum);

	BYTE GetNumberOfBlenders() { return m_byDeviceQuantity[BLENDERS]; }
	BYTE GetNumberOfCardReaders() { return m_byDeviceQuantity[CARD_READERS]; }
	BYTE GetNumberOfRFID() { return m_byDeviceQuantity[RFIDS]; }
	BYTE GetNumberOfScanners() { return m_byDeviceQuantity[SCANNERS]; }
	BYTE GetNumberOfCashAcceptors() { return m_byDeviceQuantity[CASH_ACCEPTORS]; }

	void UpdateMap(unsigned long lPumpNumber);

	void GetUpdateMaps(unsigned long * plUpdateMap1 , unsigned long * plUpdateMap2);

	BYTE GetNumberOfDevices(CDeviceRecordIndex enumIndex);

	DWORD GetNACSCode() { return m_dwNACSCode; }
	void SetNACSCode(DWORD dwCode) { m_dwNACSCode = dwCode; }

	CDeviceRecord & operator +=(CDeviceRecord &cRec);
	CDeviceRecord & operator =(CDeviceRecord &cRec);

private:
	BOOL bDoneUpdating;
	DWORD m_dwUpdateMap1 , m_dwUpdateMap2 , m_dwNACSCode;

	BYTE m_byDeviceQuantity[END_OF_LIST];
};

#endif // !defined(AFX_DEVICERECORD_H__0E254E25_009B_462E_8A89_291C9F07990C__INCLUDED_)
