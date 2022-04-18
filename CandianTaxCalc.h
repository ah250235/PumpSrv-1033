
#if !defined(AFX_CANADATAXCALC_H__19902049_0CC7_42D3_8BAC_BB7F6A4229B5__INCLUDED_)
#define AFX_CANADATAXCALC_H__19902049_0CC7_42D3_8BAC_BB7F6A4229B5__INCLUDED_


//#include "CL2PumpSrv.h"	// Added by ClassView
#include "stdafx.h"
#include "TaxCalc.h"


class CCandianTaxCalc  : public CTaxCalculatorBase
{

public:
	CCandianTaxCalc(DWORD dwGSTTaxRate , DWORD dwPSTTaxRate,DWORD dwQSTTaxRate );
	virtual ~CCandianTaxCalc(){}
	virtual void Calculate(IN FuelTaxDetails cFuelTaxDetails,IN CarWashTaxDetails cCarWashDetalis, OUT CARD_SALE_ALL3 * pCardSaleAll3);
	virtual void Print();

private:

	long CalculateGST(IN const double lSaleAmount , IN double lQST = 0 , IN double lPST = 0 );
	long CalculatePST(IN const double lSaleAmount);
	long CalculateQST(IN const double lSaleAmount);

	BOOL IsGST(DWORD dwLink) {return dwLink & 0x01;}
	BOOL IsQST(DWORD dwLink) {return dwLink & 0x02;}
	BOOL IsPST(DWORD dwLink) {return dwLink & 0x04;}

	void UpdateGST(IN long lGST , OUT CARD_SALE_ALL3 * pCardSaleAll3);
	void UpdatePST(IN long lPST , OUT CARD_SALE_ALL3 * pCardSaleAll3);
	void UpdateQST(IN long lQST , OUT CARD_SALE_ALL3 * pCardSaleAll3);

	void CalculateFuelTax( IN FuelTaxDetails cFuelTaxDetails, OUT CARD_SALE_ALL3 * pCardSaleAll3);
	
	DWORD m_dwGSTRate;
	DWORD m_dwPSTRate;
	DWORD m_dwQSTRate;
};


#endif // !defined(AFX_CANADATAXCALC_H__19902049_0CC7_42D3_8BAC_BB7F6A4229B5__INCLUDED_)