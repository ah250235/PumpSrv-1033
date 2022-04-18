#pragma once
#include "taxcalc.h"

#define TAX_INDEX_1 1
#define TAX_INDEX_2 2
#define TAX_INDEX_3 3
#define TAX_INDEX_4 4


class CMutipleTaxCalculator :  public CTaxCalculatorBase
{
public:
	CMutipleTaxCalculator() {}
	virtual ~CMutipleTaxCalculator() {}
	virtual  void Calculate(IN FuelTaxDetails cFuelTaxDetails,IN CarWashTaxDetails cCarWashTaxDetails, OUT CARD_SALE_ALL3 * pCardSaleAll3);
	void	CalculateFuelTax( FuelTaxDetails &cFuelTaxDetails, CARD_SALE_ALL3 * pCardSaleAll3 );
	void	CalculateCarWashTax(CarWashTaxDetails cCarWashDetalis, CARD_SALE_ALL3 * pCardSaleAll3);
	virtual long CalculateTax(IN long lSaleAmount, IN DWORD lTaxRate, BOOL bTaxIncluded = FALSE);
	void UpdateFuelTaxInCardSaleData(IN long lTaxAmout, IN int iTaxIndex , OUT CARD_SALE_ALL3 * pCardSaleAll3, BYTE sTaxType = '0' );
	void UpdateCarWashTaxInCardSaleData(IN long lTaxAmout, IN int iTaxIndex, OUT CARD_SALE_ALL3 * pCardSaleAll3);
	long GetCardSaleParameterOffset(BYTE *byFirst, BYTE * byParameter);
	long CalculateCentsPerLiterTax(long lTaxRate, long lTotalVolume);
};
