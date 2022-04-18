

#include "stdafx.h"
#include "CandianTaxCalc.h"
#include "Logger.h"


CCandianTaxCalc::CCandianTaxCalc( DWORD dwGSTTaxRate , DWORD dwPSTTaxRate,DWORD dwQSTTaxRate ) : m_dwPSTRate(dwPSTTaxRate),m_dwGSTRate(dwGSTTaxRate),m_dwQSTRate(dwQSTTaxRate)
{

}

void CCandianTaxCalc::Print()
{
//	_LOGMSG.PrintToLog("Canadian Calculator initialized with parameters: GST = %ld , PST = %ld , QST = %ld",m_dwGSTRate,m_dwPSTRate,m_dwQSTRate);
	CString	str;
	str.Format("Canadian Calculator initialized with parameters: GST = %ld , PST = %ld , QST = %ld", m_dwGSTRate, m_dwPSTRate, m_dwQSTRate);
	_LOGMSG.LogMsg(str);
}

void CCandianTaxCalc::Calculate(IN FuelTaxDetails cFuelTaxDetails,IN CarWashTaxDetails cCarWashDetalis, OUT CARD_SALE_ALL3 * pCardSaleAll3)
{	
	CalculateFuelTax(cFuelTaxDetails,pCardSaleAll3);
	CalculateCarWashTax(cCarWashDetalis, pCardSaleAll3);
}

// TD COSTCO-Tax
long CCandianTaxCalc::CalculateQST( IN const double dSaleAmount)
{
	if( m_dwQSTRate == 0 )
		return 0;
	
	double dTaxRate  = (double)m_dwQSTRate/100000.0 + 1.0;
	double dSaleWithoutTax = dSaleAmount/dTaxRate;
	double dTaxAmount = dSaleAmount - dSaleWithoutTax;
	
	dTaxAmount= (dTaxAmount * 100.0) + 0.5;
	dTaxAmount= floor(dTaxAmount);
	
//	_LOGMSG.PrintToLog("QST : TaxRate = %d , sTaxAmt2 = %f",m_dwQSTRate,dTaxAmount);
	CString	str;
	str.Format("QST : TaxRate = %d , sTaxAmt2 = %f", m_dwQSTRate, dTaxAmount);
	_LOGMSG.LogMsg(str);
	return dTaxAmount;
	
}


long CCandianTaxCalc::CalculatePST( IN const double dSaleAmount)
{	
	if( m_dwPSTRate == 0 )
		return 0;
	
	double dTaxRate  = ((double)m_dwPSTRate/100000.0) + ((double)m_dwGSTRate/100000.0) + 1.0;
	double dTotalSale = dSaleAmount/dTaxRate;
	double dTaxAmount = dTotalSale * (double)m_dwPSTRate/100000.0;
	
	dTaxAmount= (dTaxAmount * 100.0) + 0.5; 
	dTaxAmount= floor(dTaxAmount);

//	_LOGMSG.PrintToLog("PST : TaxRate = %d , sTaxAmt3 = %f",m_dwPSTRate, dTaxAmount);
	CString	str;
	str.Format("PST : TaxRate = %d , sTaxAmt3 = %f", m_dwPSTRate, dTaxAmount);
	_LOGMSG.LogMsg(str);

	return dTaxAmount;
}

long CCandianTaxCalc::CalculateGST( IN const double lFuelAmount , IN double lQST /* = 0  */ , IN double lPST /* = 0  */)
{
	long lFuelAmountAfterTax = lFuelAmount - (lQST /100) - (lPST /100);
	

	if( m_dwGSTRate == 0 )
		return 0;

	double dTaxRate  = (double)m_dwGSTRate/100000.0 + 1.0;
    double dTotalSale = lFuelAmountAfterTax/dTaxRate;
    double dTaxAmount = lFuelAmountAfterTax - dTotalSale;
	
    dTaxAmount= (dTaxAmount *100.0) + 0.5;
    dTaxAmount= floor(dTaxAmount);

//	_LOGMSG.PrintToLog("GST : TaxRate = %d , sTaxAmt = %f",m_dwGSTRate,dTaxAmount);
	CString	str;
	str.Format("GST : TaxRate = %d , sTaxAmt = %f", m_dwGSTRate, dTaxAmount);
	_LOGMSG.LogMsg(str);

	return dTaxAmount;
}

void CCandianTaxCalc::CalculateFuelTax( IN FuelTaxDetails cFuelTaxDetails, OUT CARD_SALE_ALL3 * pCardSaleAll3)
{
// 	_LOGMSG.PrintToLog("CCandianTaxCalc::CalculateFuelTax, Fueling params: lAmount = %ld , nGrade = %d , Tax: link = %ld , rate = %ld",
// 		cFuelTaxDetails.lAmount,cFuelTaxDetails.nGrade,cFuelTaxDetails.tax.dwLink,cFuelTaxDetails.tax.dwRate);
	CString	str;
	str.Format("CCandianTaxCalc::CalculateFuelTax, Fueling params: lAmount = %ld , nGrade = %d , Tax: link = %ld , rate = %ld",
		cFuelTaxDetails.lAmount, cFuelTaxDetails.nGrade, cFuelTaxDetails.tax.dwLink, cFuelTaxDetails.tax.dwRate);
	_LOGMSG.LogMsg(str);

	if ( cFuelTaxDetails.tax.dwLink == 0 )
	{
		long lTaxAmount = CalculateTax(cFuelTaxDetails.lAmount,cFuelTaxDetails.tax.dwRate);
		char sTax[21];
		sprintf((char*)sTax,"%d",lTaxAmount);
		memcpy(pCardSaleAll3->CardSaleAll.extData.sTaxAmt, (char*)sTax, sizeof(pCardSaleAll3->CardSaleAll.extData.sTaxAmt));
	}
	else
	{
		double dSaleAmount = cFuelTaxDetails.lAmount / 100;
		long lQST = 0 , lPST = 0 ,lGST = 0;
		if( IsQST(cFuelTaxDetails.tax.dwLink)) 
		{
			lQST = CalculateQST(dSaleAmount);
			UpdateQST(lQST,pCardSaleAll3);
		}

		if ( IsPST(cFuelTaxDetails.tax.dwLink)) 
		{
			lPST = CalculatePST(dSaleAmount);
			UpdatePST(lPST,pCardSaleAll3);

		}
		if ( IsGST(cFuelTaxDetails.tax.dwLink))
		{
			long lGST = CalculateGST(dSaleAmount,lQST,lPST);
			UpdateGST(lGST,pCardSaleAll3);	
		}
	}
}


void CCandianTaxCalc::UpdateGST( IN long lGST , OUT CARD_SALE_ALL3 * pCardSaleAll3 )
{
	BYTE sTax[21] = {0};
	sprintf((char*)sTax,"%d",lGST);		
	memcpy(pCardSaleAll3->extData5.sTaxAmt1, (char*)sTax, sizeof(pCardSaleAll3->extData5.sTaxAmt1));
}

void CCandianTaxCalc::UpdatePST( IN long lPST , OUT CARD_SALE_ALL3 * pCardSaleAll3 )
{	
	BYTE sTax[21] = {0};
	sprintf((char*)sTax,"%d",lPST);		
	memcpy(pCardSaleAll3->extData2.sTaxAmt3, (char*)sTax, sizeof(pCardSaleAll3->extData2.sTaxAmt3));	
}

void CCandianTaxCalc::UpdateQST( IN long lQST , OUT CARD_SALE_ALL3 * pCardSaleAll3 )
{
	BYTE sTax[21] = {0};
	sprintf((char*)sTax,"%d",lQST);
	memcpy(pCardSaleAll3->extData2.sTaxAmt2, (char*)sTax, sizeof(pCardSaleAll3->extData2.sTaxAmt2));
}



