// ForecourtProductList.cpp: implementation of the CForecourtProductList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ForecourtProductList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CForecourtProductList::CForecourtProductList()
{
	for (int i = 0; i < MAX_PUMPS_64; i++)	//LPE
		m_cPumpItemsArray[i].lCounter = 0;
}

CForecourtProductList::~CForecourtProductList()
{

}
/******************************************************************************
 Description:	build map array of fuel product that sold on Pump 	
 Returns:      	None
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			22/4/2004   9:37		Start
******************************************************************************/
void CForecourtProductList::LoadAllProductSoldOnPump(long &lPumpNumber)
{	
	// Clear all buffer.
	memset(&m_cPumpItemsArray , 0, sizeof(m_cPumpItemsArray));
	
	if (lPumpNumber)
	{
		LoadProductsForSinglePump(lPumpNumber, m_cPumpItemsArray[lPumpNumber]);
	}
	else
	{
		for(long lPumpNumber = 1; lPumpNumber <= MAX_PUMPS_64; lPumpNumber++)  
		{
			LoadProductsForSinglePump(lPumpNumber, m_cPumpItemsArray[lPumpNumber - 1]);
		}// end pump

	}
}

PumpItems & CForecourtProductList::operator=(long lPumpNumber)
{
	return (m_cPumpItemsArray[lPumpNumber - 1]);
}

/******************************************************************************
 Description:	Update forecourt list map with item registry setting  	
 Returns:      	BOOL succeeded/failed to get information 
 Parameters:   	long  lPumpNumber - Pump number 
				PumpItems   cRec  - Items buffer lits.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			25/4/2004   9:29		Start
******************************************************************************/

BOOL CForecourtProductList::LoadProductsForSinglePump(long & lPumpNumber, PumpItems &  cRec)
{
	DWORD	dwTmp = 0;													 	
	long	lIndex = 0;
	BOOL	bRet = FALSE;
	long	lReefer = 0;
	ItemSoldInfo cItemSoldInfo;

	_Module.m_server.m_cParam.GetParam("Pumps", lPumpNumber, 0, "ValidEntry", FIELD_DWORD,NULL, 0, &dwTmp, FALSE);
	if(dwTmp)
	{
		for(int n = 1; n <= MAX_POSITIONS_PER_PUMP; n++)
		{
			dwTmp = 0;
			lReefer = 0;
			_Module.m_server.m_cParam.GetParam("Position", lPumpNumber, n, "ValidEntry", FIELD_DWORD,NULL, 0, &dwTmp, FALSE);

			if(dwTmp)
			{
				_Module.m_server.m_cParam.GetParam("Position", lPumpNumber, n, "Grade1", FIELD_DWORD, NULL, 0, &dwTmp, FALSE);					
				//4.0.9.507
				if (LoadGradeDetalis(lPumpNumber, dwTmp, cItemSoldInfo, lReefer))
				{
					lIndex = m_cPumpItemsArray[lPumpNumber - 1].lCounter;	
					cRec.m_cItemSoldInfo[lIndex] = cItemSoldInfo;
					cRec.lCounter++;

					// check valid grade for diesel pump. 
					if (lReefer)
					{
						dwTmp = lReefer ;
						lReefer = 0;
						if (LoadGradeDetalis(lPumpNumber, dwTmp, cItemSoldInfo, lReefer))
						{
							lIndex = m_cPumpItemsArray[lPumpNumber - 1].lCounter;	
							cRec.m_cItemSoldInfo[lIndex] = cItemSoldInfo;
							cRec.lCounter++;
						}
					}
				}
			}
		}// end position

		
		LoadCarWashItem(lPumpNumber, cRec);

		bRet = TRUE;
	}// end valid

	return bRet;
}



/******************************************************************************
 Description: Return Grades of the pump number 
 Returns:      	
 Parameters:   	long  lPumpNumber - Pump number 
				PumpItems   cRec  - Items buffer lits.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			25/4/2004   9:29		Start
******************************************************************************/

void CForecourtProductList::GetAllProductsForSinglePump(long & lPumpNumber, PumpItems & cRec)
{
	cRec = m_cPumpItemsArray[lPumpNumber - 1];
}



//4.0.9.507
BOOL CForecourtProductList::LoadGradeDetalis(long & lPumpNumber, DWORD & lGrade, ItemSoldInfo & ItemSoldInfo, long & lReefer)
{
	BOOL bRetCode = FALSE;
	DWORD dwTmp = 0;

	if (lGrade)
	{
		CString sGradeTable("Grades\\Grade");		
		char sNumber[8];
		memset(sNumber,0,sizeof(sNumber));
		sprintf(sNumber,"%02d",lGrade);
		sGradeTable += sNumber;
		
		// update item sold information
		ItemSoldInfo.lGrade=(short)lGrade;
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCredit_FullService", &dwTmp,(DWORD)0, TRUE , TRUE, 0, HOP_DIRECT_LOCAL,TRUE);
		ItemSoldInfo.lPriceCreditFullService = dwTmp;
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCredit_SelfService", &dwTmp,(DWORD)0, TRUE,TRUE, 0, HOP_DIRECT_LOCAL,TRUE);
		ItemSoldInfo.lPriceCreditSelfService = dwTmp;

		// RFUEL-3939 - added Cash prices
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCash_FullService", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
		ItemSoldInfo.lPriceCashFullService = dwTmp;
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "New_PriceCash_SelfService", &dwTmp, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
		ItemSoldInfo.lPriceCashSelfService = dwTmp;

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "Code", &dwTmp, (DWORD)0, TRUE);
		ItemSoldInfo.lNACSCode = dwTmp;
		ItemSoldInfo.bFuelItem= TRUE;

		//4.0.24.60 103546
		char sTmpStr[MAX_FIELD_VAL] = {0};	//4.0.23.170 102319
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "AdditionalCode", (char *) sTmpStr, sizeof(sTmpStr), "0", TRUE); //4.0.23.170 102319
		
		ItemSoldInfo.lAdditionalCode = a2i64((unsigned char*)sTmpStr, sizeof(sTmpStr)); //4.0.23.170 102319

		
		bRetCode = TRUE;

		if (_Module.m_server.m_cPumpArray[lPumpNumber-1].GetDieselPumpFlags())
		{
			_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sGradeTable), "ConvertCode", &dwTmp, (DWORD)0, FALSE);

			if (dwTmp)  //4.0.10.20
			{
				lReefer = dwTmp;
				_Module.m_server.m_cPumpArray[lPumpNumber-1].SetDieselGrade(lGrade); //4.0.11.509 
				_Module.m_server.m_cPumpArray[lPumpNumber-1].SetReeferGrade(lReefer);  //4.0.9999.0
			}
		}
	}
	
	return bRetCode;
}

void CForecourtProductList::LoadCarWashItem(long lPumpNumber, PumpItems &cRec )
{

	DWORD	dwCarWashCode = 0, dwTmp = 0;
	CString sCarWashTable("CarWash");

	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sCarWashTable), "Enabled", &dwTmp, (DWORD)0, TRUE);
	if (dwTmp)
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sCarWashTable), "CarWashCode", &dwCarWashCode, (DWORD)0, TRUE);
	if (dwCarWashCode)
	{
		long lIndex = m_cPumpItemsArray[lPumpNumber - 1].lCounter;					
		// update item sold information
		cRec.m_cItemSoldInfo[lIndex].lGrade = dwCarWashCode;				
		cRec.m_cItemSoldInfo[lIndex].lNACSCode = dwCarWashCode;	 //4.0.6.507			  
		cRec.m_cItemSoldInfo[lIndex].bFuelItem = FALSE; 
		cRec.lCounter++;							
	}	

}
//4.0.9.507


// Debug stream dump for ItemSoldInfo
std::ostream& operator << (std::ostream& os, const ItemSoldInfo& Item)
{
	os << " Grade=" << Item.lGrade
		<< " NACSCode=" << Item.lNACSCode
		<< " PriceCreditFullService=" << Item.lPriceCreditFullService
		<< " PriceCreditSelfService=" << Item.lPriceCreditSelfService
		<< " PriceCashFullService=" << Item.lPriceCashFullService
		<< " PriceCashSelfService=" << Item.lPriceCashSelfService
		<< " FuelItem=" << ((Item.bFuelItem ? "TRUE" : "FALSE"))
		<< " AdditionalCode=" << Item.lAdditionalCode;
	return os;
}


