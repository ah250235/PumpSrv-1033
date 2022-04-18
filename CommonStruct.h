/********************************************************************
	created:	2004/05/05
	created:	5:5:2004   17:51
	filename: 	D:\Fuel\Convert Layer\CL2PumpSrv(Postec)\CommonStruct.h
	file path:	D:\Fuel\Convert Layer\CL2PumpSrv(Postec)
	file base:	CommonStruct
	file ext:	h
	author:		Marik
	
	purpose:	PumpSrv shared structs
*********************************************************************/

#ifndef __COMMON_STRUCT_H_
#define __COMMON_STRUCT_H_


#include "GeneralStruct.h"
#include "BuyAtPumpDef.h"
#include "OLA.h"

// Taken from CMDInfo.h in pumpsrv project
typedef struct 
{
	CARD_SALE_DATA			CardSaleData;	
	CARD_SALE_EXTRA_DATA	CardSaleExtraData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT2		PumpTransact;
}RECEIPT_DATA;

typedef struct 
{
	CARD_SALE_DATA			CardSaleData;	
	CARD_SALE_EXTRA_DATA	CardSaleExtraData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT5		PumpTransact;
}RECEIPT_DATA_EX;	//TD 38654

typedef struct 
{
	CARD_SALE_DATA			CardSaleData;	
	CARD_SALE_EXTRA_DATA	CardSaleExtraData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT2		PumpTransact;
	CARD_SALE_EXTRA_DATA3_  extData3;
}RECEIPT_DATA3;

typedef struct 
{
	CARD_SALE_DATA			CardSaleData;	
	CARD_SALE_EXTRA_DATA	CardSaleExtraData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT5		PumpTransact;
	CARD_SALE_EXTRA_DATA3_  extData3;
}RECEIPT_DATA3_EX; //TD 38654

typedef struct 
{
	CARD_SALE_DATA			CardSaleData;	
	CARD_SALE_EXTRA_DATA	CardSaleExtraData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT2		PumpTransact;
	CARD_SALE_EXTRA_DATA3_  extData3;
	RECEIPT_EXTRA_INFO		cReceiptExtraInfo;
}RECEIPT_DATA4;


typedef struct 
{
	CARD_SALE_DATA			CardSaleData;	
	CARD_SALE_EXTRA_DATA	CardSaleExtraData;	
	CARD_SALE_EXTRA_DATA2_  extData2;
	TAG_PUMP_TRANSACT5		PumpTransact;
	CARD_SALE_EXTRA_DATA3_  extData3;
	RECEIPT_EXTRA_INFO		cReceiptExtraInfo;
	CARD_SALE_EXTRA_DATA4_  extData4;                //4.0.27.45 - Egift
	CARD_SALE_EXTRA_DATA5_  extData5;			 	 //4.0.27.45 - Egift
	CARD_SALE_EXTRA_DATA6_  extData6;
}RECEIPT_DATA4_EX; //TD 38654


///* Move this struct to  GeneralStruct.h 4.0.9.500
typedef struct
{
	BYTE						lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA				cardData;
	CARD_SALE_EXTRA_DATA		extraData;
	CARD_SALE_EXTRA_DATA2_		extraData2;
	TAG_PUMP_TRANSACT2			tagTrs;
	BAP_ALL_ITEMS_FOR_RECEIPT	Items;
}TAG_OLA_REC4;

typedef struct
{
	BYTE						lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA				cardData;
	CARD_SALE_EXTRA_DATA		extraData;
	CARD_SALE_EXTRA_DATA2_		extraData2;
	TAG_PUMP_TRANSACT5			tagTrs;
	BAP_ALL_ITEMS_FOR_RECEIPT	Items;
}TAG_OLA_REC4_EX; //TD 38654



typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	CARD_SALE_EXTRA_DATA2_  extraData2;	
	TAG_PUMP_TRANSACT2		tagTrs;
	CARD_SALE_EXTRA_DATA3_  extraData3; //4.0.5.0
}TAG_OLA_REC5;

typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	CARD_SALE_EXTRA_DATA2_  extraData2;	
	TAG_PUMP_TRANSACT5		tagTrs;
	CARD_SALE_EXTRA_DATA3_  extraData3; //4.0.5.0
}TAG_OLA_REC5_EX; //TD 38654

typedef struct
{
	BYTE						lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA				cardData;
	CARD_SALE_EXTRA_DATA		extraData;
	CARD_SALE_EXTRA_DATA2_		extraData2;
	TAG_PUMP_TRANSACT2			tagTrs;
	BAP_ALL_ITEMS_FOR_RECEIPT	Items;
	CARD_SALE_EXTRA_DATA3_		extraData3;   //4.0.5.0
}TAG_OLA_REC6;

typedef struct
{
	BYTE						lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA				cardData;
	CARD_SALE_EXTRA_DATA		extraData;
	CARD_SALE_EXTRA_DATA2_		extraData2;
	TAG_PUMP_TRANSACT5			tagTrs;
	BAP_ALL_ITEMS_FOR_RECEIPT	Items;
	CARD_SALE_EXTRA_DATA3_		extraData3;   //4.0.5.0
}TAG_OLA_REC6_EX; //TD 38654

typedef struct
{
	BYTE						lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA				cardData;
	CARD_SALE_EXTRA_DATA		extraData;
	CARD_SALE_EXTRA_DATA2_		extraData2;
	TAG_PUMP_TRANSACT2			tagTrs;
	BAP_ALL_ITEMS_FOR_RECEIPT	Items;
	CARD_SALE_EXTRA_DATA3_		extraData3;   //4.0.5.0
	BAP_ALL_ITEMS_FOR_RECEIPT2  Items2 ; 
}TAG_OLA_REC7;

typedef struct
{
	BYTE						lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA				cardData;
	CARD_SALE_EXTRA_DATA		extraData;
	CARD_SALE_EXTRA_DATA2_		extraData2;
	TAG_PUMP_TRANSACT5			tagTrs;
	BAP_ALL_ITEMS_FOR_RECEIPT	Items;
	CARD_SALE_EXTRA_DATA3_		extraData3;   //4.0.5.0
	BAP_ALL_ITEMS_FOR_RECEIPT2  Items2 ; 
}TAG_OLA_REC7_EX; //TD 38654

typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	CARD_SALE_EXTRA_DATA2_  extraData2;	
	TAG_PUMP_TRANSACT2		tagTrs;
	CARD_SALE_EXTRA_DATA3_  extraData3; //4.0.5.0
	RECEIPT_EXTRA_INFO		ReceiptExtraInfo;	 //4.0.5.39		 
}TAG_OLA_REC8;

typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	CARD_SALE_EXTRA_DATA2_  extraData2;	
	TAG_PUMP_TRANSACT5		tagTrs;
	CARD_SALE_EXTRA_DATA3_  extraData3; //4.0.5.0
	RECEIPT_EXTRA_INFO		ReceiptExtraInfo;	 //4.0.5.39		 
}TAG_OLA_REC8_EX; //TD 38654

typedef struct
{
	BYTE						lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA				cardData;
	CARD_SALE_EXTRA_DATA		extraData;
	CARD_SALE_EXTRA_DATA2_		extraData2;
	TAG_PUMP_TRANSACT2			tagTrs;
	BAP_ALL_ITEMS_FOR_RECEIPT	Items;
	CARD_SALE_EXTRA_DATA3_		extraData3;   //4.0.5.0
	BAP_ALL_ITEMS_FOR_RECEIPT2  Items2 ;	 //4.0.5.27	
	RECEIPT_EXTRA_INFO			ReceiptExtraInfo;	 //4.0.5.39		 
}TAG_OLA_REC9;

typedef struct
{
	BYTE						lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA				cardData;
	CARD_SALE_EXTRA_DATA		extraData;
	CARD_SALE_EXTRA_DATA2_		extraData2;
	TAG_PUMP_TRANSACT5			tagTrs;
	BAP_ALL_ITEMS_FOR_RECEIPT	Items;
	CARD_SALE_EXTRA_DATA3_		extraData3;   //4.0.5.0
	BAP_ALL_ITEMS_FOR_RECEIPT2  Items2 ;	 //4.0.5.27	
	RECEIPT_EXTRA_INFO			ReceiptExtraInfo;	 //4.0.5.39		 
}TAG_OLA_REC9_EX; //TD 38654


typedef struct
{
	BYTE	bReceiptKey[ENCRYPTED_DATA_SIZE];
	BYTE    sFiller[400]; //4.0.22.511 71868
}OUTPUT_DATA_NOT_SAVE_TO_QDX;	//4.0.20.501 TD-60236 - Total size is 500 bytes!! 

//4.0.17.501 - TD 38162
typedef struct
{
	BYTE						lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA				cardData;
	CARD_SALE_EXTRA_DATA		extraData;
	CARD_SALE_EXTRA_DATA2_		extraData2;
	TAG_PUMP_TRANSACT5			tagTrs;
	BAP_ALL_ITEMS_FOR_RECEIPT	Items;
	CARD_SALE_EXTRA_DATA3_		extraData3;
	BAP_ALL_ITEMS_FOR_RECEIPT2  Items2;
	RECEIPT_EXTRA_INFO			ReceiptExtraInfo;
	CARD_SALE_EXTRA_DATA4_		extraData4;	
}TAG_OLA_REC10;	 

typedef struct
{
	TAG_OLA_REC10					tagOlaRec10;			
	OUTPUT_DATA_NOT_SAVE_TO_QDX		outputData;
}TAG_OLA_REC11;	 


typedef struct
{
	TAG_OLA_REC11				tagOlaRec11;
	CARD_SALE_EXTRA_DATA5_		extraData5;	
}TAG_OLA_REC12;	 //4.0.22.511 71868

typedef struct
{
	TAG_OLA_REC12				tagOlaRec12;
	CARD_SALE_EXTRA_DATA6_		extraData6;
}TAG_OLA_REC13;	 //4.0.22.511 71868

typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	CARD_SALE_EXTRA_DATA2_  extraData2;
	TAG_PUMP_TRANSACT2		tagTrs;
}TAG_OLA_REC3;

typedef struct
{
	BYTE                    lIndexNumber [STR_LONG_VAL5];
	CARD_SALE_DATA			cardData;
	CARD_SALE_EXTRA_DATA	extraData;
	CARD_SALE_EXTRA_DATA2_  extraData2;
	TAG_PUMP_TRANSACT5		tagTrs;
}TAG_OLA_REC3_EX;	//TD 38654

typedef struct 
{
	BYTE sModeName [MAX_FIELD_VAL]; 
	BYTE sCreditLimit [STR_LONG_VAL10];
	BYTE sCashLimit [STR_LONG_VAL10];
	BYTE sLimitType[STR_LONG_VAL2];
	BYTE sFiller[20];	
}TAG_MODEDEF2;


//4.0.19.504 - 47559 Start
typedef struct 
{
	BYTE sProductTaxCode[10];
	BYTE sProductTaxAmount[10];
}ITEM_TAX_INFO;


typedef struct 
{
	BYTE sTransTaxCode[10];     
	BYTE sTransTaxAmount[10];
}ITEM_TOTALS_TAX_INFO;

typedef struct 
{
	BYTE sProductLoyaltyApplicable;
	BYTE sProductAmount[10];
	BYTE sProductUnitPrice[10];
	BYTE sProductTaxIncluded;
	BYTE sProductLabel[20];
	BYTE sProductCode[10];
	BYTE sProductUPCCode[10];
	BYTE sProductQty[10];
	BYTE sProductDiscountCode[10]; 
	BYTE sProductDiscountAmount[10]; 
	
	ITEM_TAX_INFO  cItemTax[MAX_TAXES_PER_PRODUCT]; //4.0.25.90 - TD 142798

}ITEM_INFO;
typedef struct
{
	BYTE sTransTaxRate[10];
}ITEM_TAX_RATE;

typedef struct
{
	BYTE sTransAmount[10];
	BYTE sTransLoyaltyAmount [10];

	BYTE sTransDiscountCode1[10];
	BYTE sTransDiscountAmount1[10];

	BYTE sTransDiscountCode2[10];
	BYTE sTransDiscountAmount2[10];

	BYTE sTransLoyaltyTenderAmount[8];	//1.0.22.710 TD 247268

	/* Tax  total section */
	ITEM_TOTALS_TAX_INFO cItemTotalsTaxInfo[MAX_TAXES_PER_PRODUCT];

	ITEM_INFO cItemInfo[MAX_PRODUCTS];

	BYTE sProductActivationCode1[10];
	
	BYTE sInvoiceNumber[10];
	BYTE sLoyaltyApplicable;		//4.0.22.511 - TD 74795
	BYTE sDryItemsTotal[10];		//4.0.22.511 - TD 74796
	BYTE sTranType;					//4.0.22.511 - TD 74796
	BYTE sPreAuthAmt[10];			//4.0.22.511 - TD 74796
	BYTE sPOSReferenceNumber[10];	//4.0.25.50  - TD 136918
	BYTE sLoyalId[STR_20];			//4.0.26.508 - TD 177984
	ITEM_TAX_RATE cItemTaxRate[MAX_TAXES_PER_PRODUCT];
	BYTE sFiler[194];  
 
}SALE_EXTRA_DETAILS;


typedef struct 
{
    BYTE sInvoiceNumber[10];
    BYTE sPumpNumber[2];
    BYTE sProductLabel1[20];
    BYTE sTransTaxAmount1[10];
    BYTE sFuelAmount[10];
    BYTE sFuelVolume[10];
    BYTE sCardType[2];
    BYTE sMaskedAccountNumber[21];
	TAG_SYSTEMTIME sDateTime;
    BYTE sTransactionStatus[2];
    BYTE sDeclineReason[10];
    BYTE sAuthAmount[10];
    BYTE sVehicleId[FULL_VEHICLE_ID];
	BYTE sFiler[200];  

}RECEIPT_TRANSACTIONS_LIST;
//4.0.19.504 - 47559 End

#endif
