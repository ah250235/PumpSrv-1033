#include "stdafx.h"
#include "MutipleTaxCalculator.h"


void CMutipleTaxCalculator::Calculate(IN FuelTaxDetails cFuelTaxDetails,IN CarWashTaxDetails cCarWashTaxDetails, OUT CARD_SALE_ALL3 * pCardSaleAll3)
{
	CalculateFuelTax(cFuelTaxDetails, pCardSaleAll3);

	CalculateCarWashTax(cCarWashTaxDetails, pCardSaleAll3);

}

long CMutipleTaxCalculator::CalculateTax( IN long lSaleAmount , IN DWORD dwTaxRate , BOOL bTaxIncluded /* = 0 */)
{
	if( dwTaxRate == 0 )
		return 0;

	double dTaxAmount = 0;

	double dSaleAmount = lSaleAmount;

	if(bTaxIncluded)
	{
		double dTaxRate  = (double)dwTaxRate/100000 + 1;
		double dSaleWithoutTax = dSaleAmount/dTaxRate;
		dTaxAmount = dSaleAmount - dSaleWithoutTax;
	}
	else
	{
		double dTaxRate  = (double)dwTaxRate/100000;
		dTaxAmount = lSaleAmount * dTaxRate;
	}

	return dTaxAmount;


}

void CMutipleTaxCalculator::UpdateFuelTaxInCardSaleData( IN long lTaxAmout , IN int iTaxIndex , OUT CARD_SALE_ALL3 * pCardSaleAll3, BYTE sTaxType  /* = 0 */ )
{
	if(lTaxAmout == 0 || pCardSaleAll3 == NULL)
		return;

	BYTE sTax[5] = {0};
	sprintf((char*)sTax,"%d",lTaxAmout);

	if (iTaxIndex == TAX_INDEX_1)
	{
		memcpy(pCardSaleAll3->extData5.sTaxAmt1, (char*)sTax, sizeof(pCardSaleAll3->extData5.sTaxAmt1));
		pCardSaleAll3->extData6.sTaxType1 = sTaxType;
	}
	if (iTaxIndex == TAX_INDEX_2)
	{
		memcpy(pCardSaleAll3->extData2.sTaxAmt2, (char*)sTax, sizeof(pCardSaleAll3->extData2.sTaxAmt2));
		pCardSaleAll3->extData6.sTaxType2 = sTaxType;
	}
	if(iTaxIndex == TAX_INDEX_3)
	{
		memcpy(pCardSaleAll3->extData2.sTaxAmt3, (char*)sTax, sizeof(pCardSaleAll3->extData2.sTaxAmt3));
		pCardSaleAll3->extData6.sTaxType3 = sTaxType;
	}
	if (iTaxIndex == TAX_INDEX_4)
	{
		memcpy(pCardSaleAll3->extData6.sTaxAmt4, (char*)sTax, sizeof(pCardSaleAll3->extData6.sTaxAmt4));
		pCardSaleAll3->extData6.sTaxType4 = sTaxType;
	}
}

void CMutipleTaxCalculator::UpdateCarWashTaxInCardSaleData(IN long lTaxAmout, IN int iTaxIndex, OUT CARD_SALE_ALL3 * pCardSaleAll3)
{
	if (lTaxAmout == 0 || pCardSaleAll3 == NULL)
		return;

	BYTE sTax[5] = { 0 };
	sprintf((char*)sTax, "%d", lTaxAmout);

	if (iTaxIndex == TAX_INDEX_1)
		memcpy(pCardSaleAll3->extData6.CarWashItem.sTaxAmount1, (char*)sTax, sizeof(pCardSaleAll3->extData6.CarWashItem.sTaxAmount1));

	if (iTaxIndex == TAX_INDEX_2)
		memcpy(pCardSaleAll3->extData6.CarWashItem.sTaxAmount2, (char*)sTax, sizeof(pCardSaleAll3->extData6.CarWashItem.sTaxAmount2));
}

void CMutipleTaxCalculator::CalculateFuelTax( FuelTaxDetails &cFuelTaxDetails, CARD_SALE_ALL3 * pCardSaleAll3 )
{

	CString	str;
	str.Format("CMutipleTaxCalclator::CalculateFuelTax, Fuel item details: lAmount = %d, Volume = %d, Grade = %d, Taxlink = %d", cFuelTaxDetails.lAmount, cFuelTaxDetails.lVolume, cFuelTaxDetails.nGrade, cFuelTaxDetails.tax.dwLink);
	_LOGMSG.LogMsg(str);

	if ( cFuelTaxDetails.tax.dwLink == 0 )
	{
		long lGradeTax = CalculateTax(cFuelTaxDetails.lAmount,cFuelTaxDetails.tax.dwRate);
		char sTax[21];
		sprintf((char*)sTax,"%d",lGradeTax);
		memcpy(pCardSaleAll3->CardSaleAll.extData.sTaxAmt, (char*)sTax, sizeof(pCardSaleAll3->CardSaleAll.extData.sTaxAmt));
	}
	else
	{
		long lAllTaxRate = 0;
		int iNumberOfTaxes = 0;
		long lTaxNumber = 1;
		BYTE sTaxRateCPL = '1';
		long lAllCPLTaxRate = 0;

		for(int i = 0; i < MAX_TAXES; i++)
		{
			int iTaxMapIndex = (1<<i);
			if (_Module.m_server.m_cTaxMap[i].bValid && (cFuelTaxDetails.tax.dwLink & iTaxMapIndex) && !_Module.m_server.m_cTaxMap[i].bIsTaxTypeCPL)
			{
				lAllTaxRate += _Module.m_server.m_cTaxMap[i].lTaxRate;
				iNumberOfTaxes++;
			}
			else if (_Module.m_server.m_cTaxMap[i].bValid && (cFuelTaxDetails.tax.dwLink & iTaxMapIndex) && _Module.m_server.m_cTaxMap[i].bIsTaxTypeCPL)
			{
				lAllCPLTaxRate = _Module.m_server.m_cTaxMap[i].lTaxRate;
				iNumberOfTaxes++;
			}
		}
		
		long lTotalTaxAmount = CalculateTax(cFuelTaxDetails.lAmount,lAllTaxRate,TRUE);
		long lTotalCPLTaxAmount = CalculateCentsPerLiterTax(lAllCPLTaxRate, cFuelTaxDetails.lVolume);

		long lNet = cFuelTaxDetails.lAmount - lTotalTaxAmount;
		lTotalTaxAmount += lTotalCPLTaxAmount;
		long lTotalLoopTax = 0;
		
		for (int i = 0; (i < MAX_TAXES) && (lTaxNumber <= MAX_TAXES_ALLOWED); i++)
		{
			int iTaxMapIndex = (1<<i);
			long lTaxAmount;
			if (_Module.m_server.m_cTaxMap[i].bValid && (cFuelTaxDetails.tax.dwLink & iTaxMapIndex) && _Module.m_server.m_cTaxMap[i].bIsTaxTypeCPL)
			{
				long lTaxRate = _Module.m_server.m_cTaxMap[i].lTaxRate;

				if (iNumberOfTaxes == 1)
				{
					lTaxAmount = lTotalTaxAmount - lTotalLoopTax;
					
					double dbVal = lTaxAmount;
					dbVal = (floor(dbVal / 10 + 0.5)) * 10;
					lTaxAmount = dbVal;
				}
				else
				{
					lTaxAmount = CalculateCentsPerLiterTax(lTaxRate, cFuelTaxDetails.lVolume);

					double dbVal = lTaxAmount;
					dbVal = (floor(dbVal / 10 + 0.5)) * 10;
					lTaxAmount = dbVal;

					lTotalLoopTax += lTaxAmount;
				}

				long lOffset = GetCardSaleParameterOffset(&pCardSaleAll3->CardSaleAll.data.sTranStatus, pCardSaleAll3->extData6.sTaxName1);
				memcpy(&pCardSaleAll3->CardSaleAll.data.sTranStatus + lOffset + ((lTaxNumber - 1)*sizeof(pCardSaleAll3->extData6.sTaxName1)), _Module.m_server.m_cTaxMap[i].sTaxName, min(sizeof(pCardSaleAll3->extData6.sTaxName1), sizeof(_Module.m_server.m_cTaxMap[i].sTaxName)));
				str.Format("CMultipleTaxCalclator::CalculateCentsPerLiterTax: TaxIndex = %d, lTaxAmount = %d, lTaxCPLRate = %d", i + 1, lTaxAmount, lTaxRate);
				_LOGMSG.LogMsg(str);

				UpdateFuelTaxInCardSaleData(lTaxAmount, lTaxNumber, pCardSaleAll3, sTaxRateCPL);
				lTaxNumber++;
				iNumberOfTaxes--;
			}
			else if (_Module.m_server.m_cTaxMap[i].bValid && (cFuelTaxDetails.tax.dwLink & iTaxMapIndex) && !_Module.m_server.m_cTaxMap[i].bIsTaxTypeCPL)
			{

				long lTaxRate = _Module.m_server.m_cTaxMap[i].lTaxRate;
				
				if(iNumberOfTaxes == 1)
				{
					lTaxAmount = lTotalTaxAmount - lTotalLoopTax;
					double dbVal = lTaxAmount;
				    dbVal = (floor(dbVal / 10 + 0.5)) * 10;
					lTaxAmount = dbVal;
				}
				else
				{
					lTaxAmount = CalculateTax(lNet,lTaxRate);

					double dbVal = lTaxAmount;
					dbVal = (floor(dbVal / 10 + 0.5)) * 10;
					lTaxAmount = dbVal;	

					lTotalLoopTax += lTaxAmount;
				}

				long lOffset = GetCardSaleParameterOffset(&pCardSaleAll3->CardSaleAll.data.sTranStatus, pCardSaleAll3->extData6.sTaxName1);
				memcpy(&pCardSaleAll3->CardSaleAll.data.sTranStatus + lOffset + ((lTaxNumber-1)*sizeof(pCardSaleAll3->extData6.sTaxName1)) , _Module.m_server.m_cTaxMap[i].sTaxName , min(sizeof(pCardSaleAll3->extData6.sTaxName1) , sizeof(_Module.m_server.m_cTaxMap[i].sTaxName)));
				str.Format("CMultipleTaxCalclator::CalculateFuelTax: TaxIndex = %d, lTaxAmount = %d, lTaxRate = %d", i + 1, lTaxAmount, lTaxRate);
				_LOGMSG.LogMsg(str);

				UpdateFuelTaxInCardSaleData(lTaxAmount, lTaxNumber, pCardSaleAll3);

				lTaxNumber++;

				iNumberOfTaxes--;
			}
			
		}
		Convertl2Str3(lTotalTaxAmount, &pCardSaleAll3->extData3.sTaxAmt_Msb2, sizeof(pCardSaleAll3->extData3.sTaxAmt_Msb2), pCardSaleAll3->extData3 .sTaxAmt_Msb, sizeof(pCardSaleAll3->extData3.sTaxAmt_Msb),  
			pCardSaleAll3->CardSaleAll.extData.sTaxAmt, 
			sizeof(pCardSaleAll3->CardSaleAll.extData.sTaxAmt));
	}
}

long CMutipleTaxCalculator::GetCardSaleParameterOffset(BYTE *byFirst, BYTE * byParameter)
{
	return byParameter - byFirst;
}

long CMutipleTaxCalculator::CalculateCentsPerLiterTax(long lTaxRate, long lTotalVolume)
{
	double dTaxRate = (double)lTaxRate / 100000;
	double dAmount = (double)lTotalVolume * dTaxRate;
	long   lAmount = dAmount;
	return lAmount;
}

void CMutipleTaxCalculator::CalculateCarWashTax(CarWashTaxDetails cCarWashDetalis, CARD_SALE_ALL3 * pCardSaleAll3)
{
	if (cCarWashDetalis.lAmount == 0)
		return;
	
	BYTE sCarWashAmount[21] = { 0 };
	long lCarWashTaxAmount = 0;
	CString	str;

	if (cCarWashDetalis.tax.dwLink > 0)
	{
		str.Format("CMutipleTaxCalculator::CalculateCarWashTax, car wash item details: lAmount = %d, tax link = %d", cCarWashDetalis.lAmount, cCarWashDetalis.tax.dwLink);
		_LOGMSG.LogMsg(str);
	}
	else
	{
		str.Format("CMutipleTaxCalculator::CalculateCarWashTax, car wash item details: lAmount = %d, Tax: included = %d, rate = %ld", cCarWashDetalis.lAmount, cCarWashDetalis.dwIncluded, cCarWashDetalis.tax.dwRate);
		_LOGMSG.LogMsg(str);
	}

	if (cCarWashDetalis.tax.dwRate == 0)
	{
		lCarWashTaxAmount = CalculateTax(cCarWashDetalis.lAmount, cCarWashDetalis.tax.dwRate);
		
		sprintf((char*)sCarWashAmount, "%d", lCarWashTaxAmount);
		memcpy(pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax, (char*)sCarWashAmount, sizeof(pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax));
	}
	else
	{
		long lTaxNumber = 1;
		for (int iTaxIndex = 0; iTaxIndex < MAX_TAXES; iTaxIndex++)
		{
			int iTaxMapIndex = (1 << iTaxIndex);
			if (_Module.m_server.m_cTaxMap[iTaxIndex].bValid && (cCarWashDetalis.tax.dwLink & iTaxMapIndex))
			{
				long lTaxAmount;
				long lTaxRate = _Module.m_server.m_cTaxMap[iTaxIndex].lTaxRate;
				lTaxAmount = CalculateTax(cCarWashDetalis.lAmount, lTaxRate);

				str.Format("CMultipleTaxCalclator::CalculateCarWashTax: TaxIndex = %d, lTaxAmount = %d, lTaxRate = %d", iTaxIndex, lTaxAmount, lTaxRate);
				_LOGMSG.LogMsg(str);

				UpdateCarWashTaxInCardSaleData(lTaxAmount, lTaxNumber, pCardSaleAll3); //TD 442282
				pCardSaleAll3->extData6.CarWashItem.sTaxName1[0] == ' '? memcpy(pCardSaleAll3->extData6.CarWashItem.sTaxName1 , _Module.m_server.m_cTaxMap[iTaxIndex].sTaxName , min(sizeof(_Module.m_server.m_cTaxMap[iTaxIndex].sTaxName),sizeof(pCardSaleAll3->extData6.CarWashItem.sTaxName1)))
					:memcpy(pCardSaleAll3->extData6.CarWashItem.sTaxName2 , _Module.m_server.m_cTaxMap[iTaxIndex].sTaxName , min(sizeof(_Module.m_server.m_cTaxMap[iTaxIndex].sTaxName),sizeof(pCardSaleAll3->extData6.CarWashItem.sTaxName2)));
				lTaxNumber++;

				lCarWashTaxAmount += lTaxAmount;
			}

		}

		sprintf((char*)sCarWashAmount, "%d", lCarWashTaxAmount);
		memcpy(pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax, (char*)sCarWashAmount, sizeof(pCardSaleAll3->CardSaleAll.extData.SALES[0].sTax));

		// update total tax
		long lTotalTaxAmount = a2l(pCardSaleAll3->CardSaleAll.extData.sTaxAmt, sizeof(pCardSaleAll3->CardSaleAll.extData.sTaxAmt)) + lCarWashTaxAmount;
		l2a(lCarWashTaxAmount, pCardSaleAll3->CardSaleAll.extData.sTaxAmt, sizeof(pCardSaleAll3->CardSaleAll.extData.sTaxAmt));
	}
}