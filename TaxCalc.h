// TaxCalc.h: interface for the CTaxCalc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAXCALC_H__19902049_0CC7_42D3_8BAC_BB7F6A4229B5__INCLUDED_)
#define AFX_TAXCALC_H__19902049_0CC7_42D3_8BAC_BB7F6A4229B5__INCLUDED_


//#include "CL2PumpSrv.h"	// Added by ClassView
#define PUMPSRV_VOL_VAL_FACTOR		1000 // Factor between PumpSrv volume/value units to litres

typedef struct Tax
{
	DWORD dwLink;
	DWORD dwRate;
	DWORD dwTypeOfTax;
}Tax;


typedef struct FuellingDetails
{
	long lAmount;
	long lVolume;
	SHORT nGrade;
	Tax tax;

}FuelTaxDetails;


typedef struct  CarWashTaxDetails
{	
	long       lAmount;
	Tax tax;
	DWORD dwIncluded;	
	
} CarWashTaxDetails;

enum TaxCalclator
{	
	eDefaultTaxCalclator = 1,
	eCandianTaxCalclator,
	eMultipleTaxCalclator
	
};

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTaxCalculatorBase  
{
public:
	CTaxCalculatorBase() {}
	virtual ~CTaxCalculatorBase() {}
	virtual  void Calculate(IN FuelTaxDetails cFuelTaxDetails,IN CarWashTaxDetails cCarWashDetalis, OUT CARD_SALE_ALL3 * pCardSaleAll3) = 0;
	virtual  void Print();
	virtual  void CalculateCarWashTax( CarWashTaxDetails cCarWashDetalis, CARD_SALE_ALL3 * pCardSaleAll3);

protected:	
	virtual  long CalculateTax(IN long lSaleAmount, IN DWORD lTaxRate,IN BOOL bTaxIncluded = 0);
	
};

class CLegacyTaxCalc :  public CTaxCalculatorBase
{
public:
	CLegacyTaxCalc() {}
	virtual ~CLegacyTaxCalc() {}
	virtual  void Calculate(IN FuelTaxDetails cFuelTaxDetails,IN CarWashTaxDetails cCarWashTaxDetails, OUT CARD_SALE_ALL3 * pCardSaleAll3);
	long RoundAmount(long lValue);
};


class CTaxCalcTester  
{
public:
	CTaxCalcTester() {};
	virtual ~CTaxCalcTester() {};

	void CalcTrsTaxes();
};


#endif // !defined(AFX_TAXCALC_H__19902049_0CC7_42D3_8BAC_BB7F6A4229B5__INCLUDED_)
