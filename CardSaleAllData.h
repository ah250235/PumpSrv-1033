#ifndef _CARD_SALE_ALL_STRUCT
#define _CARD_SALE_ALL_STRUCT

//#include "xmlinterface.h"
//#include "PumpSrvDB.h"
#include <list>

#include "CTendersCard.h"
typedef std::list<CTendersCard> TendersCards;					//CR 404913
typedef std::list<CTendersCard>::iterator CTendersCardsIterator;//CR 404913

typedef struct 
{
	BYTE  PromptName0[MAX_PROM_NAME];   //1 -20		4.0.9.507   //4.0.10.49
	BYTE  PromptDataType0;				//21-21 	
	BYTE  PromptMaxLen0[2];				//22-23
	BYTE  PromptMinLen0[2];				//24-25
	BYTE  PromptMode0;					//26-26
	BYTE  PromptSource0[2];				//27-28
	BYTE  PromptEncrypted0;				//29-29
	BYTE  PromptText0[MAX_PROM_TEXT];	//30-49	
	BYTE  PromptResult[MAX_PROM_RESULT];//50-79
}TravelCenterData;


//4.0.999.0
typedef struct  {
long lTax;
BYTE sTaxDescription[15];
}TRAVEL_CENTER_TAX;


class CCardSaleAllData	  {

public:
	CCardSaleAllData();
	long	CreateFleetPAPXML(CPumpTransact &cTrs, PAY_AT_PUMP_INFO & PayAtPumpInfo ,char *&outXML); //4.0.10.492
	long	CreateFleetPAKXML(CPumpTransact & cTrs , char *& outXML);
	BOOL    CreateXMLFromStruct(char *& pszXMLOut,long lOLAIndex ,CARD_SALE_ALL3 * pvStructIn ,CPumpTransact & trs ,void * pcRec=NULL); //4.0.9999.0 //4.0.10.492
	void	CreateDieselSale( CPumpTransact &trs, CARD_SALE_ALL3 * pvStructIn, CXMLInterface &xmlInitDoc, CXMLPointers pChildNode9, long &lCarWashIndex, long lValue, long lAmountCarwash, long lDiscountCarwash, long &lVolume, long &lGradePrice );
	void	WriteTransactionXml(CXMLInterface &xmlInitDoc );
	void	AttachFuelItemToCardSaleDataXml( long lValue, long lAmountCarwash, long lDiscountCarwash, CARD_SALE_ALL3 * pvStructIn, CXMLInterface &xmlInitDoc, CXMLPointers pChildNode4, CPumpTransact &trs, long lGradePrice, long lVolume, CXMLPointers pChildNode9 );
	void	AttachCarWashItemToCardSaleDataXml( long lCarWashIndex, CXMLInterface &xmlInitDoc, CXMLPointers pChildNode4,CARD_SALE_ALL3 * pvStructIn , long lDiscountCarwash, CXMLPointers pChildNode9 );
	void	AttachCarWashTaxesToCardSaleDataXml( CXMLInterface &xmlInitDoc, CARD_SALE_ALL3 * pvStructIn, CXMLPointers &pCurrElement );
	void	AttachFuelTaxesToCardSaleDataXml( CXMLInterface &xmlInitDoc, CARD_SALE_ALL3 * pvStructIn,CXMLPointers pCurrElement ,long lGrade);
	void	BuildLimitsToCardSaleXML(CXMLInterface &xmlInitDoc,CARD_SALE_ALL3 * pvStructIn, CXMLPointers & pCurrElement);
	BOOL	CreateStructFromXML(CARD_SALE_ALL3 * pvStructOut,void * pvTravelCenter,CPumpTransact & trs,char * pszXMLOIn);//4.0.10.310
	void    ParsePilotString(CString * pInStr,TravelCenterData & sTravelCenterData ,PAY_AT_PUMP_INFO *pInfo );
	void	CreateTendersData(CXMLInterface &xmlInitDoc, CXMLPointers & pCurrElement, CARD_SALE_ALL3 * pvStructIn);	//CR 404913 

	
	BOOL	CreatePakTrsFromXML(char* sInXML, LONG& lNewTrsNumber, BOOL bCreateNewTransactionNumber = FALSE);//4.0.10.310 //CR 351687: Coles Enhanced post void transaction
	BOOL	AppendLoyaltyMessagesToXmlObject(long lPumpNumber, CXMLInterface & pRoot);
	


private:
	char	m_strCName[MAX_STRUCT_NAME][NAME_LEN];//hold the fields in each struct
	char	m_StructName[MAX_STRUCT_IN_STRUCT][NAME_LEN];//hold the struct name and the inner struct
	char	m_szDeclineMsg[MAX_FIELD_VAL];
	TendersCards  			m_cTendersCard[MAX_PUMPS];
};




#endif //_CARD_SALE_ALL_STRUCT

