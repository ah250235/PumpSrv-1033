// ForecourtProductList.h: interface for the CForecourtProductList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORECOURTPRODUCTLIST_H__4901C9E7_D44F_4DCC_AAF7_87E2DE0AD088__INCLUDED_)
#define AFX_FORECOURTPRODUCTLIST_H__4901C9E7_D44F_4DCC_AAF7_87E2DE0AD088__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_CAR_WASH_ITEMS		1
#define MAX_ITEMS				MAX_POSITIONS_PER_PUMP  + MAX_CAR_WASH_ITEMS //4.0.5550.1000 amram
	
typedef struct
{
	long  lGrade;
	long  lNACSCode ;
	long  lPriceCreditFullService;
	long  lPriceCreditSelfService;
	BOOL  bFuelItem;			 
	INT64  lAdditionalCode;	//4.0.23.170 102319	 //4.0.24.60 103546	 

	// RFUEL-3939
	long  lPriceCashFullService;
	long  lPriceCashSelfService;


public:	

} ItemSoldInfo;

// for debugging
std::ostream& operator << (std::ostream& os, const ItemSoldInfo& Item);

typedef struct
{
      long			  lCounter;
      ItemSoldInfo    m_cItemSoldInfo[MAX_ITEMS]; 
}PumpItems;


class CForecourtProductList  
{
public:
	BOOL LoadProductsForSinglePump(long & lPumpNumber, PumpItems  & cRec);

	void LoadCarWashItem( long lPumpNumber, PumpItems &cRec );
	void LoadAllProductSoldOnPump(long  & lPumpNumber);
	void GetAllProductsForSinglePump(long & lPumpNumber, PumpItems  & cRec);//amram
	CForecourtProductList();
	virtual ~CForecourtProductList();	
	PumpItems & operator=(long lPumpNumber);
private:
	BOOL LoadGradeDetalis(long & lPumpNumber, DWORD  & lGrade, ItemSoldInfo &ItemSoldInfo , long & lReefer);
	PumpItems m_cPumpItemsArray[MAX_PUMPS_64];
};

#endif // !defined(AFX_FORECOURTPRODUCTLIST_H__4901C9E7_D44F_4DCC_AAF7_87E2DE0AD088__INCLUDED_)
