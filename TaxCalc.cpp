// TaxCalc.cpp: implementation of the CTaxCalc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TaxCalc.h"
#include "Logger.h"
#include "CandianTaxCalc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


long CTaxCalculatorBase::CalculateTax( IN long lSaleAmount, IN DWORD dwTaxRate , BOOL bTaxIncluded)
{
	if( dwTaxRate )
	{
		double dTaxRate  = (double)dwTaxRate/100000.0 + 1.0;
		double dTotalSale = lSaleAmount/dTaxRate;
		double dTaxAmount = lSaleAmount - dTotalSale;
		return dTaxAmount * 100;
	}
	
	return 0;
}

void CTaxCalculatorBase::CalculateCarWashTax( CarWashTaxDetails cCarWashDetalis, CARD_SALE_ALL3 * pCardSaleAll3)
{
	if(cCarWashDetalis.lAmount == 0)
		return;

	long lCarWashTaxAmount = 0;

	BYTE sCarWashAmount[21] = {0};
	double dCarWashValue  = cCarWashDetalis.lAmount  / 100;

	if ( cCarWashDetalis.tax.dwRate == 0 )
	{
		lCarWashTaxAmount = CalculateTax(dCarWashValue ,cCarWashDetalis.tax.dwRate);
		sprintf((char*)sCarWashAmount,"%d",lCarWashTaxAmount);
		memcpy(pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax, (char*)sCarWashAmount, sizeof(pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax));	
	}
	else
	{
		if (cCarWashDetalis.dwIncluded)
		{
			lCarWashTaxAmount = CalculateTax(dCarWashValue, cCarWashDetalis.tax.dwRate);
			sprintf((char*)sCarWashAmount, "%d", lCarWashTaxAmount);
			memcpy(pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax, (char*)sCarWashAmount, sizeof(pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax));
		}
		else
		{
			double dTaxRate = (double)cCarWashDetalis.tax.dwRate / 100000.0;
			double dTaxAmount = dCarWashValue * dTaxRate;
			lCarWashTaxAmount = dTaxAmount * 100;

			sprintf((char*)sCarWashAmount, "%d", lCarWashTaxAmount);
			memcpy(pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax, (char*)sCarWashAmount, sizeof(pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax));

			long lTotalTaxAmount = a2l(pCardSaleAll3->CardSaleAll.extData.sTaxAmt, sizeof(pCardSaleAll3->CardSaleAll.extData.sTaxAmt));
			lTotalTaxAmount += lCarWashTaxAmount;
			l2a(lCarWashTaxAmount, pCardSaleAll3->CardSaleAll.extData.sTaxAmt, sizeof(pCardSaleAll3->CardSaleAll.extData.sTaxAmt));
		}

		
	}

// 	_LOGMSG.PrintToLog("CTaxCalculatorBase::CalculateCarWashTax, CarWash detalis: lAmount = %ld, Tax: included = %ld, link = %ld, rate = %ld, lCarWashTaxAmount = %ld",
// 		cCarWashDetalis.lAmount,cCarWashDetalis.dwIncluded,cCarWashDetalis.tax.dwLink,cCarWashDetalis.tax.dwRate,lCarWashTaxAmount);

	CString	str;
	str.Format("CTaxCalculatorBase::CalculateCarWashTax, CarWash detalis: lAmount = %ld, Tax: included = %ld, link = %ld, rate = %ld, lCarWashTaxAmount = %ld",
		cCarWashDetalis.lAmount, cCarWashDetalis.dwIncluded, cCarWashDetalis.tax.dwLink, cCarWashDetalis.tax.dwRate, lCarWashTaxAmount);
	_LOGMSG.LogMsg(str);
}

void CTaxCalculatorBase::Print()
{
	return;
}

void CLegacyTaxCalc::Calculate( IN FuelTaxDetails cFuelTaxDetails,IN CarWashTaxDetails cCarWashDetalis, OUT CARD_SALE_ALL3 * pCardSaleAll3 )
{
	long lIncludedTax = 0;

	if(cFuelTaxDetails.nGrade)
	{
		lIncludedTax = RoundAmount(cFuelTaxDetails.lAmount - (long)((cFuelTaxDetails.lAmount * 100) / (cFuelTaxDetails.tax.dwRate/(float)PUMPSRV_VOL_VAL_FACTOR + 100)));
	}
	
	Convertl2Str3(lIncludedTax, &pCardSaleAll3->extData3.sTaxAmt_Msb2, sizeof(pCardSaleAll3->extData3.sTaxAmt_Msb2), pCardSaleAll3->extData3 .sTaxAmt_Msb, sizeof(pCardSaleAll3->extData3.sTaxAmt_Msb),  
		pCardSaleAll3->CardSaleAll.extData.sTaxAmt, 
		sizeof(pCardSaleAll3->CardSaleAll.extData.sTaxAmt));
	l2a0(lIncludedTax/10, pCardSaleAll3->extData2.sTaxAmt2 , sizeof(pCardSaleAll3->extData2.sTaxAmt2));	
}


long CLegacyTaxCalc::RoundAmount(long lValue)
{
	return ((lValue + 5) / 10) * 10;
}


/*************************************************************************

								Tester Class				  
				
**************************************************************************/

void CTaxCalcTester::CalcTrsTaxes()
{
	
	long lGrade = 1;
	
	char       strRegEntry[50];


	FuelTaxDetails cFuelTaxDetails;
	CarWashTaxDetails cCarWashTax;


	cFuelTaxDetails.lAmount = 41470;
	cCarWashTax.lAmount = 10000;

	//Fuel Taxes
	sprintf(strRegEntry,"Grades\\Grade%.2d",lGrade);
//	_Module.m_server.LoadParam(strRegEntry, 0, 0, "MapTaxLink", &cFuelTaxDetails.tax.dwLink, 0 , FALSE);
	_Module.m_server.m_cParam.LoadParam((LPCSTR)strRegEntry, "MapTaxLink", &cFuelTaxDetails.tax.dwLink, (DWORD)0, FALSE);
	
	sprintf(strRegEntry,"Grades\\Grade%.2d",lGrade);
//	_Module.m_server.LoadParam(strRegEntry, 0, 0, "TaxRate", &cFuelTaxDetails.tax.dwRate, 0 , FALSE);
	_Module.m_server.m_cParam.LoadParam((LPCSTR)strRegEntry, "TaxRate", &cFuelTaxDetails.tax.dwRate, (DWORD)0, FALSE);

	//CarWash Taxes

	sprintf(strRegEntry,"WashPrograms\\WashProgram%.2d",lGrade);
//	_Module.m_server.LoadParam(strRegEntry, 0, 0, "MapTaxLink", &cCarWashTax.tax.dwLink, 0 , FALSE);
	_Module.m_server.m_cParam.LoadParam((LPCSTR)strRegEntry, "MapTaxLink", &cCarWashTax.tax.dwLink, (DWORD)0, FALSE);

	if ( cCarWashTax.tax.dwLink== 0 ) //No linked taxes! Use TaxRate
	{
		
		sprintf(strRegEntry,"WashPrograms\\WashProgram%.2d",lGrade);
//		_Module.m_server.LoadParam(strRegEntry, 0, 0, "TaxRate", &cCarWashTax.tax.dwRate, 0 , FALSE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)strRegEntry, "TaxRate", &cCarWashTax.tax.dwRate, (DWORD)0, FALSE);

		cCarWashTax.dwIncluded = 0;

	}
	else
	{
		int iTax=0;
		for (; iTax<16; iTax++)
		{
			if ( cCarWashTax.tax.dwLink & (2^iTax) )
			{
				iTax++;
				break;
			}
		}
		sprintf(strRegEntry,"Taxes\\Tax%.2d", iTax);
//		_Module.m_server.LoadParam(strRegEntry, 0, 0, "TaxRate", &cCarWashTax.tax.dwRate, 0 , FALSE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)strRegEntry, "TaxRate", &cCarWashTax.tax.dwRate, (DWORD)0, FALSE);
//		_Module.m_server.LoadParam(strRegEntry, 0, 0, "Included", &cCarWashTax.dwIncluded, 0 , FALSE);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)strRegEntry, "Included", &cCarWashTax.dwIncluded, (DWORD)0, FALSE);
	}

	//Candian taxes
	
	DWORD      dwPST = 0;
	sprintf(strRegEntry,"Taxes\\Tax03");
//	_Module.m_server.LoadParam(strRegEntry, 0, 0, "TaxRate", &dwPST, 0 , FALSE);
	_Module.m_server.m_cParam.LoadParam((LPCSTR)strRegEntry, "TaxRate", &dwPST, (DWORD)0, FALSE);
	
	DWORD      dwQST = 0;
	sprintf(strRegEntry,"Taxes\\Tax02");
//	_Module.m_server.LoadParam(strRegEntry, 0, 0, "TaxRate", &dwQST, 0 , FALSE);
	_Module.m_server.m_cParam.LoadParam((LPCSTR)strRegEntry, "TaxRate", &dwQST, (DWORD)0, FALSE);
	
	DWORD      dwGST = 0;
	sprintf(strRegEntry,"Taxes\\Tax01");
//	_Module.m_server.LoadParam(strRegEntry, 0, 0, "TaxRate", &dwGST, 0 , FALSE);
	_Module.m_server.m_cParam.LoadParam((LPCSTR)strRegEntry, "TaxRate", &dwGST, (DWORD)0, FALSE);

	CCandianTaxCalc cTaxCalc(dwGST,dwPST,dwQST);

	CARD_SALE_ALL3 cardSale;
	
	cTaxCalc.Calculate(cFuelTaxDetails,cCarWashTax,&cardSale);
	
}	

