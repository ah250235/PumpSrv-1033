// WetStockDetails.h: interface for the WetStockDetails class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WETSTOCKDETAILS_H__51AE9D2A_C645_4BCA_9864_88AF71F8A2B7__INCLUDED_)
#define AFX_WETSTOCKDETAILS_H__51AE9D2A_C645_4BCA_9864_88AF71F8A2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWetStockDetails  
{
public:
	void SetDeliveryStatus(long lVal);
	long GetDeliveryStatus();	
	CWetStockDetails();
	virtual ~CWetStockDetails();
private:
	long m_lDeliveryStatus;

};

#endif // !defined(AFX_WETSTOCKDETAILS_H__51AE9D2A_C645_4BCA_9864_88AF71F8A2B7__INCLUDED_)
