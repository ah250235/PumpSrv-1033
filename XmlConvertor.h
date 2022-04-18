// XmlConvertor.h: interface for the CXmlConvertor class.
//
////////////////////////////////////////////////////////////////////// 

#if !defined(AFX_XMLCONVERTOR_H__8E9D895B_42E4_4A4A_9A00_15F7D2ECFE63__INCLUDED_)
#define AFX_XMLCONVERTOR_H__8E9D895B_42E4_4A4A_9A00_15F7D2ECFE63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define MAX_STRUCT_NAME						1000
#define MAX_NAME_LEN							50
#define TAG_RECEIPT_GENERAL_DATA5_STRUCT_SIZE	50000
#define TAG_RECEIPT_GENERAL_DATA6_STRUCT_SIZE	50000
#define XML_WITHOUT_NODES						2


//XML flags
#define	CARD_SALE_ALL3_FLAG						0x0000000001
#define	CARD_SALE_ALL2_FLAG						0x0000000002
#define	CARD_SALE_ALL_FLAG						0x0000000004
#define	CARD_SALE_DATA_FLAG						0x0000000008
#define	CARD_SALE_EXTRA_DATA_FLAG				0x0000000010
#define	CARD_SALE_EXTRA_DATA2_FLAG				0x0000000020
#define	CARD_SALE_EXTRA_DATA3_FLAG				0x0000000040
#define	GRADE_LIMITS_FLAG						0x0000000080
#define	DISCNT_FLAG								0x0000000100
#define	SALE_FLAG								0x0000000200
#define	BAP_ITEM_FOR_RECEIPT_FLAG				0x0000000400
#define	BAP_ITEM_FOR_RECEIPT2_FLAG				0x0000000800
#define	TAG_GENERAL_INFO_FLAG					0x0000001000
#define	RECEIPT_EXTRA_INFO_FLAG					0x0000002000
#define	TAG_RECEIPT_GENERAL_DATA5_FLAG			0x0000004000
#define	TAG_RECEIPT_GENERAL_DATA4_FLAG			0x0000008000
#define	TAG_RECEIPT_GENERAL_DATA3_FLAG			0x0000010000
#define	TAG_RECEIPT_GENERAL_DATA_FLAG			0x0000020000
#define	TAG_SYSTEMTIME_FLAG						0x0000040000
#define	TAG_PUMP_TRANSACT2_FLAG					0x0000080000
#define	TAG_OLA_REC9_FLAG						0x0000100000
#define	TAG_OLA_REC8_FLAG						0x0000200000
#define	TAG_OLA_REC7_FLAG						0x0000400000
#define	TAG_OLA_REC6_FLAG						0x0000800000
#define	TAG_OLA_REC5_FLAG						0x0001000000
#define	TAG_OLA_REC4_FLAG						0x0002000000
#define	TAG_OLA_REC3_FLAG						0x0004000000
#define	BAP_ALL_ITEM_FOR_RECEIPT_FLAG			0x0008000000
#define	BAP_ALL_ITEM_FOR_RECEIPT2_FLAG			0x0010000000
#define	TAX_DATA_SUMMARY_FLAG					0x0020000000
#define	PAY_AT_PUMP_INFO_FLAG					0x0040000000
#define CARD_SALE_EXTRA_DATA4_PLUS_FLAG			0x0080000000
#define RECEIPT_DATA4_FLAG						0x0100000000	
#define BAP_FUEL_ITEM_FLAG						0x0200000000
#define	CARD_SALE_EXTRA4_FLAG					0x0400000000	//4.0.16.504
#define DALLAS_KEYS_INFO_FLAG					0x0800000000	//4.0.16.504
#define CARD_SALE_EXTRA5_FLAG					0x1000000000	//4.0.18.501
#define CARD_SALE_EXTRA6_FLAG					0x2000000000	//4.0.18.501
//#define CARD_SALE_ALL4_FLAG						0x1000000000	//4.0.16.504
#define	TAG_PUMP_TRANSACT5_FLAG					0x2000000000	//TD 38654
#define BAP_FUEL_ITEM_EX_FLAG					0x4000000000	//TD 38654
#define RECEIPT_DATA4_EX_FLAG					0x8000000000	//TD 38654	
#define	TAG_OLA_REC3_EX_FLAG					0x10000000000	//TD 38654
#define	TAG_OLA_REC4_EX_FLAG					0x20000000000	//TD 38654
#define	TAG_OLA_REC5_EX_FLAG					0x40000000000	//TD 38654
#define	TAG_OLA_REC6_EX_FLAG					0x80000000000	//TD 38654
#define	TAG_OLA_REC7_EX_FLAG					0x100000000000	//TD 38654
#define	TAG_OLA_REC8_EX_FLAG					0x200000000000	//TD 38654
#define	TAG_OLA_REC9_EX_FLAG					0x400000000000	//TD 38654
#define	DISCOUNT_ATTRIBUTES_INFO_FLAG			0x800000000000	//4.0.17.501 - CR 38162
#define	TAG_OLA_REC10_FLAG						0x1000000000000 //4.0.17.501 - CR 38162
#define	TAG_RECEIPT_GENERAL_DATA6_FLAG			0x2000000000000 //4.0.18.    - CR 46512
#define CARD_SALE_ALL_CHIP_EXTRA_FLAG           0x4000000000000 //4.0.20.20
#define	TAG_OLA_REC11_FLAG						0x8000000000000 //4.0.20.501 TD 60236
#define DISCOUNT_ATTRIBUTES_EXT_FLAG			0x10000000000000	//4.0.22.509 38162
#define TAG_OLA_REC12_FLAG						0x20000000000000	//4.0.22.511 71868
#define TAG_OLA_REC13_FLAG						0x40000000000000	//4.0.22.511 71868
#define DISCOUNT_LOYALTY_INFO4_FLAG				0x80000000000000	//RFUEL-520

#define MAX_DYNAMIC_BUFFERS_LENGTH 270
#define SPARE_SIZE 108


typedef struct
{
	BYTE    sVolume[10];
	BYTE    sGradePrice[10];
	BYTE    sGrade[10];
	BYTE    sFuelValue[10];
	BYTE	sReceiptNumber[10];
	BYTE    cGradeName[MAX_STRING_LEN];
	BYTE    cPathToPumpSrv[_MAX_PATH];
	BYTE	sVolumeFactor[5];
	BYTE	sCurrencyFactor[5];
	BYTE	byIsOriginalCopy;
	BYTE    cHeaderDynamicBuffer[MAX_DYNAMIC_BUFFERS_LENGTH]; //  9 lines * 30 chars per line
	BYTE    sHeaderDynamicBufferLen[10];
	BYTE    cFooterDynamicBuffer[MAX_DYNAMIC_BUFFERS_LENGTH]; //  9 lines * 30 chars per line
	BYTE    sFooterDynamicBufferLen[10];
	BYTE	cCarWashProgramName[MAX_STRING_LEN];
	BYTE	sCarWashTotalPrice[10];
	BYTE    sCouponExpDate[10];
	BYTE	sTransactionNumber[10];
}TAG_GENERAL_INFO;

typedef struct
{
	CARD_SALE_DATA				cData;
	CARD_SALE_EXTRA_DATA		cExtData;
	CARD_SALE_EXTRA_DATA2_		cExtData2;
	BAP_ALL_ITEMS_FOR_RECEIPT	cBuyAtPumpDataForReceipt;
	TAG_GENERAL_INFO			cTagGeneralInfo;
	BYTE						Spare[SPARE_SIZE];
}TAG_RECEIPT_GENERAL_DATA;

typedef struct
{
	CARD_SALE_DATA				cData;
	CARD_SALE_EXTRA_DATA		cExtData;
	CARD_SALE_EXTRA_DATA2_		cExtData2;
	BAP_ALL_ITEMS_FOR_RECEIPT	cBuyAtPumpDataForReceipt;
	TAG_GENERAL_INFO			cTagGeneralInfo;
	CARD_SALE_EXTRA_DATA3_		cExtData3;
	BYTE						Spare[SPARE_SIZE];
}TAG_RECEIPT_GENERAL_DATA3;

typedef struct
{
	CARD_SALE_DATA				cData;
	CARD_SALE_EXTRA_DATA		cExtData;
	CARD_SALE_EXTRA_DATA2_		cExtData2;
	BAP_ALL_ITEMS_FOR_RECEIPT	cBuyAtPumpDataForReceipt;
	TAG_GENERAL_INFO			cTagGeneralInfo;
	CARD_SALE_EXTRA_DATA3_		cExtData3;
	BAP_ALL_ITEMS_FOR_RECEIPT2	cBuyAtPumpDataForReceipt2;
	BYTE						Spare[SPARE_SIZE];
}TAG_RECEIPT_GENERAL_DATA4;

typedef struct
{
	BYTE sTaxName[20];
	BYTE sTaxRate[8];
	BYTE sTaxPrintChar;
	BYTE sTaxAmount[5]; 	//Tax part from all items with this tax type
	BYTE sTaxNetAmount[8]; 	//Price of all items with this tax type excluding the tax
}TAX_DATA_SUMMARY;

typedef struct
{
	CARD_SALE_DATA				cData;
	CARD_SALE_EXTRA_DATA		cExtData;
	CARD_SALE_EXTRA_DATA2_		cExtData2;
	BAP_ALL_ITEMS_FOR_RECEIPT	cBuyAtPumpDataForReceipt;
	TAG_GENERAL_INFO			cTagGeneralInfo;
	CARD_SALE_EXTRA_DATA3_		cExtData3;
	BAP_ALL_ITEMS_FOR_RECEIPT2	cBuyAtPumpDataForReceipt2;
	RECEIPT_EXTRA_INFO			cReceiptExtraInfo;
	TAX_DATA_SUMMARY			cTaxSummary;
	BYTE						Spare[SPARE_SIZE];
}TAG_RECEIPT_GENERAL_DATA5;

typedef struct
{
	CARD_SALE_DATA    cData;
	CARD_SALE_EXTRA_DATA  cExtData;
	CARD_SALE_EXTRA_DATA2_  cExtData2;
	BAP_ALL_ITEMS_FOR_RECEIPT cBuyAtPumpDataForReceipt;
	TAG_GENERAL_INFO   cTagGeneralInfo;
	CARD_SALE_EXTRA_DATA3_  cExtData3;
	BAP_ALL_ITEMS_FOR_RECEIPT2 cBuyAtPumpDataForReceipt2;
	RECEIPT_EXTRA_INFO   cReceiptExtraInfo;
	TAX_DATA_SUMMARY   cTaxSummary;
	BYTE      Spare[SPARE_SIZE];
	CARD_SALE_EXTRA_DATA4_  cExtData4;
	CARD_SALE_EXTRA_DATA5_  cExtData5;
	CARD_SALE_EXTRA_DATA6_  cExtData6;
}TAG_RECEIPT_GENERAL_DATA6;

typedef struct  
{
	CARD_SALE_ALL3 cCardSaleAll3;	//4.0.21.14 69073
	EXTRA_CHIP_INFO cChipExtraData;
}CARD_SALE_ALL_CHIP_EXTRA_DATA;  //4.0.20.20


#include "Logger.h"

class CXMLInterface;
class CXMLPointers;

class CXmlConvertor  
{

public:
	CXmlConvertor();
	virtual ~CXmlConvertor();

	/****************************************************************
	ConvertFromXmlToStruct function overloading
	****************************************************************/
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL3 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL2 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_DATA * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA2_ * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA3_ * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, GRADE_LIMITS * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, DISCNT * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, SALE * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT2 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, BAP_FUEL_ITEM * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, BAP_FUEL_ITEM_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_GENERAL_INFO * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, RECEIPT_EXTRA_INFO * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA6 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA5 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA4 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA3 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_SYSTEMTIME * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_PUMP_TRANSACT2 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_PUMP_TRANSACT5 * StructOut); //TD 38654

	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC9 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC8 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC7 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC6 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC5 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC4 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC3 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAX_DATA_SUMMARY * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_PLUS * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, RECEIPT_DATA4 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, RECEIPT_DATA4_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC3_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC4_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC5_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC6_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC7_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC8_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC9_EX * StructOut); //TD 38654

	//4.0.17.501 - CR 38162
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, DISCOUNT_ATTRIBUTES * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, DISCOUNT_ATTRIBUTES_EXT* StructOut); //4.0.22.509 38162

	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC10 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC11 * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC12 * StructOut); //4.0.22.511 71868
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC13 * StructOut);
	//4.0.16.504
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_ * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, DALLAS_KEYS_INFO * StructOut);
	void CreateStructDallasKeysInfoFromXmlWithOutNodes(char * pszXMLIn, DALLAS_KEYS_INFO * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc);
	void CreateStructCardSaleExtraData4FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc);
		
	//4.0.18.501
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA5_ * StructOut);
	void CreateStructCardSaleExtraData5FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA5_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc);

	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA6_ * StructOut);
	void CreateStructCardSaleExtraData6FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA6_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc);
	
	//4.0.20.503 66960
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL_CHIP_EXTRA_DATA* StructOut);
	
	/****************************************************************
	ConvertFromStructToXml function overloading
	****************************************************************/

	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_ALL3 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_ALL2 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_ALL * StructIn );
	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_DATA * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA2_ * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA3_ * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, GRADE_LIMITS * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, DISCNT * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, SALE * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT2 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, BAP_FUEL_ITEM * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, BAP_FUEL_ITEM_EX * StructIn); //TD 38654

	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_GENERAL_INFO * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, RECEIPT_EXTRA_INFO * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA6 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA5 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA4 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA3 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_SYSTEMTIME * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_PUMP_TRANSACT2 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_PUMP_TRANSACT5 * StructIn); //TD 38654

	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC9 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC8 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC7 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC6 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC5 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC4 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC3 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAX_DATA_SUMMARY * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA4_PLUS * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, RECEIPT_DATA4 * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, RECEIPT_DATA4_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC3_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC4_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC5_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC6_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC7_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC8_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC9_EX * StructIn); //TD 38654

	//4.0.17.501 - CR 38162
	BOOL ConvertFromStructToXml(char * pszXMLOut, DISCOUNT_ATTRIBUTES * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, DISCOUNT_ATTRIBUTES_EXT* StructIn); //4.0.22.509 38162
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC10 * StructIn);


	//4.0.16.504
	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA4_ * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, DALLAS_KEYS_INFO * StructIn);
	void CreateXMLFromStructDallasKeysInfo(char * pszXMLOut, DALLAS_KEYS_INFO * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement);
	void CreateXMLFromStructCardSaleExtraData4(char * pszXMLOut, CARD_SALE_EXTRA_DATA4_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement);
	
	//4.0.18.501
	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA5_ * StructIn);
	void CreateXMLFromStructCardSaleExtraData5(char * pszXMLOut, CARD_SALE_EXTRA_DATA5_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement);

	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA6_ * StructIn);
	void CreateXMLFromStructCardSaleExtraData6(char * pszXMLOut, CARD_SALE_EXTRA_DATA6_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement);
	void CreateXmlFromStructPaymentCardDetails(CXMLInterface * xmlInitDoc, CXMLPointers * pPaymentCardDetails, char * pszXMLOut, CARD_SALE_EXTRA_DATA6_ * StructIn);

	//4.0.20.20
	BOOL ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_ALL3 * StructIn, EXTRA_CHIP_INFO * cChipExtraData);
	void CreateXMLFromChipExtraData(char * pszXMLOut, EXTRA_CHIP_INFO * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement);
	
	BOOL ConvertFromStructToXml(char* pszXMLOut, TAG_OLA_REC11* StructIn);	//4.0.20.501 TD 60236
	BOOL ConvertFromStructToXml(char* pszXMLOut, TAG_OLA_REC12* StructIn);	//4.0.22.511 71868
	BOOL ConvertFromStructToXml(char* pszXMLOut, TAG_OLA_REC13* StructIn);
	BOOL ConvertFromStructToXml(char* pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO4* SrtuctIn); // RFUEL-520
	BOOL ConvertFromStructToXml(char* pszXMLOut, TAG_EXTRA_INFO* SrtuctIn); // RFUEL-520
	
protected:
	
	/****************************************************************
	Create Struct functions without nodes
	****************************************************************/

	BOOL CreateStruct(char * pszXMLIn, void * vpStructOut, __int64 lXmlFlags);
	void CreateStructCardSaleAll3FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_ALL3 * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructCardSaleAll2FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_ALL2 * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructCardSaleAllFromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_ALL * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL, CARD_SALE_EXTRA_DATA2_ * StructOut2 = NULL, CARD_SALE_EXTRA_DATA3_ * StructOut3 = NULL); //4.0.15.502 //4.0.23.509 100885
	void CreateStructCardSaleDataFromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_DATA * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL, CARD_SALE_EXTRA_DATA * StructOut2 = NULL, CARD_SALE_EXTRA_DATA2_ * StructOut3 = NULL); //4.0.15.502
	void CreateStructCardSaleExtraDataFromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL, CARD_SALE_EXTRA_DATA3_ * StructOut3 = NULL);	//4.0.18.670 73612  4.0.22.502
	void CreateStructCardSaleExtraData2FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA2_ * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructCardSaleExtraData3FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA3_ * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	
	void CreateStructGradeLimitFromXmlWithOutNodes(CXMLInterface * xmlInitDoc,  CXMLPointers * pXml,  char * pszXMLIn, CARD_SALE_EXTRA_DATA5_ * StructOut ); // TD 370688
	void CreateStructGradeLimitFromXmlWithOutNodes(CXMLInterface * xmlInitDoc,  CXMLPointers * pXml,  char * pszXMLIn, CARD_SALE_EXTRA_DATA3_ * StructOut );
	void CreateStructAmountGradeLimitFromXmlWithOutNodes(char * pszXMLIn, GRADE_LIMITS * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructVolumeGradeLimitFromXmlWithOutNodes(char * pszXMLIn, GRADE_LIMITS * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructDiscntFromXmlWithOutNodes(char * pszXMLIn, DISCNT * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructSaleFromXmlWithOutNodes(char * pszXMLIn, SALE * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructBuyAtPumpItemForReceiptFromXmlWithOutNodes(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructBuyAtPumpItemForReceipt2FromXmlWithOutNodes(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT2 * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateTwoStructBuyAtPumpWithOutNodes(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT * StructOut, BAP_ITEM_FOR_RECEIPT2 * StructOut2, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructBuyAtPumpAllItemsForReceiptFromXmlWithOutNodes(char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructBuyAtPumpAllItemsForReceipt2FromXmlWithOutNodes(char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateTwoStructBuyAtPumpAllWithOutNodes(char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT * StructOut, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructOut2, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructBuyAtPumpFuelItemFromXMLWithOutNode(char * pszXMLIn, BAP_FUEL_ITEM * StructOut, CXMLPointers *pXml = NULL, CXMLInterface * xmlInitDoc = NULL);
	void CreateStructBuyAtPumpFuelItemFromXMLWithOutNode(char * pszXMLIn, BAP_FUEL_ITEM_EX * StructOut, CXMLPointers *pXml, CXMLInterface * xmlInitDoc); //TD 38654
	void CreateStructTagGeneralInfoFromXmlWithOutNodes(char * pszXMLIn, TAG_GENERAL_INFO * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructReceiptExtraInfoFromXmlWithOutNodes(char * pszXMLIn, RECEIPT_EXTRA_INFO * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructTagSystemTimeFromXmlWithOutNodes(char * pszXMLIn, TAG_SYSTEMTIME * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructTagPumpTransact2FromXmlWithOutNodes(char * pszXMLIn, TAG_PUMP_TRANSACT2 * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructTagPumpTransact5FromXmlWithOutNodes(char * pszXMLIn, TAG_PUMP_TRANSACT5 * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc); //TD 38654
	void CreateStructDiscountAttrsInfoFromXmlWithOutNodes(char * pszXMLIn, DISCOUNT_ATTRIBUTES * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc); //4.0.17.501 - CR 38162 
	void CreateStructDiscountAttrsExtFromXmlWithOutNodes(char * pszXMLIn, DISCOUNT_ATTRIBUTES_EXT * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc); //4.0.17.501 - CR 38162 //4.0.22.509
	void CreateStructDiscountIDsFromXmlWithOutNodes(char * pszXMLIn, ITEM_DISCOUNT_ID * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc); //4.0.17.501 - CR 38162
	
	void CreateStructTaxDataSummaryFromXmlWithOutNodes(char * pszXMLIn, TAX_DATA_SUMMARY * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructCardSaleExtraPlusFromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_PLUS * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructExtraChipFromXmlWithOutNodes(char * pszXMLIn, EXTRA_CHIP_INFO * StructOut, CXMLPointers *pXml,  CXMLInterface *xmlInitDoc);	//4.0.20.503 66960
	void CreateStructOlaExtraDataFromXmlWithOutNodes(char * pszXMLIn, OLA_EXTRA_DATA * StructOut, CXMLPointers *pXml,  CXMLInterface *xmlInitDoc); //4.0.24.60 109103
	void CreateStructCarWashItemFromXmlWithOutNodes(char * pszXMLIn, CARWASH_ITEM * StructOut, CXMLPointers *pXml = NULL, CXMLInterface *xmlInitDoc = NULL);
	void CreateStructPaymentCardFromXmlWithOutNodes( char * pszXMLIn, PAYMENT_CARD_DETAILS * StructOut, CXMLPointers *pXml /*= NULL */,CXMLInterface *xmlInitDoc /*= NULL*/ );

	void CreateStructLoyaltyCardDetailsFromXmlWithOutNodes(char * pszXMLIn, LOYALTY_CARD_DETAILS * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc); //MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling

	/****************************************************************
	Create xml functions without nodes
	****************************************************************/
		
	BOOL CreateXml(char * pszXMLOut, void * vpStructIn, __int64 lXmlFlags);
	void CreateXMLFromStructCardSaleAll3(char * pszXMLOut, CARD_SALE_ALL3 * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructCardSaleAll2(char * pszXMLOut, CARD_SALE_ALL2 * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructCardSaleAll(char * pszXMLOut, CARD_SALE_ALL * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL, CARD_SALE_EXTRA_DATA2_ * StructIn2 = NULL, CARD_SALE_EXTRA_DATA3_ * StructIn3 = NULL); //4.0.15.502	//4.0.18.670 73612 4.0.22.502
	void CreateXMLFromStructCardSaleData(char * pszXMLOut, CARD_SALE_DATA * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL, CARD_SALE_EXTRA_DATA * StructIn2 = NULL, CARD_SALE_EXTRA_DATA2_ * StructIn3 = NULL); //4.0.15.502
	void CreateXMLFromStructCardSaleExtraData(char * pszXMLOut, CARD_SALE_EXTRA_DATA * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL, CARD_SALE_EXTRA_DATA3_ * StructIn3 = NULL);	//4.0.18.670 73612 4.0.22.502
	void CreateXMLFromStructCardSaleExtraData2(char * pszXMLOut, CARD_SALE_EXTRA_DATA2_ * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructCardSaleExtraData3(char * pszXMLOut, CARD_SALE_EXTRA_DATA3_ * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructTagPumpTransact2WithOutNodes(char * pszXMLOut, TAG_PUMP_TRANSACT2 * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructTagPumpTransact5WithOutNodes(char * pszXMLOut, TAG_PUMP_TRANSACT5 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement); //TD 38654
	void CreateXMLFromStructGradeLimitsWithOutNodes(char * pszXMLOut, GRADE_LIMITS * StructIn, long line_id = 0, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructDiscntWithOutNodes(char * pszXMLOut, DISCNT * StructIn, long line_id = 0, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructSaleWithOutNodes(char * pszXMLOut, SALE * StructIn, long line_id = 0, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructBuyAtPumpItemForReceiptWithOutNodes(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT * StructIn, long line_id = 0, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructBuyAtPumpItemForReceipt2WithOutNodes(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT2 * StructIn, long line_id = 0, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructBuyAtPumpAllItemsForReceiptWithOutNodes(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructBuyAtPumpAllItemsForReceipt2WithOutNodes(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructBuyAtPumpTwoStructs(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT * StructIn, long line_id = 0, BAP_ITEM_FOR_RECEIPT2 * StructIn2 = NULL, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructBuyAtPumpAllTwoStructs(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT * StructIn, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructIn2, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructTagGeneralInfoWithOutNodes(char * pszXMLOut, TAG_GENERAL_INFO * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructTagSystemTimeWithOutNodes(char * pszXMLOut, TAG_SYSTEMTIME * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructReceiptExtraInfoWithOutNodes(char * pszXMLOut, RECEIPT_EXTRA_INFO * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructTaxDataSummaryWithOutNodes(char * pszXMLOut, TAX_DATA_SUMMARY * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructCardSaleExtraPlusWithOutNodes(char * pszXMLOut, CARD_SALE_EXTRA_DATA4_PLUS * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructBuyAtPumpFuelItem(char * pszXMLOut, BAP_FUEL_ITEM * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL);
	void CreateXMLFromStructBuyAtPumpFuelItem(char * pszXMLOut, BAP_FUEL_ITEM_EX * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL); //TD 38654
	void CreateXMLFromStructDiscountAttrsInfo(char * pszXMLOut, DISCOUNT_ATTRIBUTES * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement, DISCOUNT_ATTRIBUTES_EXT * StructIn2); //4.0.17.501 - CR 38162 //4.0.22.509
	void CreateXMLFromStructDiscountIDsWithOutNodes(char * pszXMLOut, ITEM_DISCOUNT_ID * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement);				//4.0.17.501 - CR 38162
	void CreateXMLFromStructOlaExtraDataWithOutNodes(char * pszXMLOut, OLA_EXTRA_DATA * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers *pCurrElement = NULL); //4.0.24.60 109103
	void CreateXMLFromStructCarWashItemWithOutNodes(char * pszXMLOut, CARWASH_ITEM * StructIn, CXMLInterface * xmlInitDoc = NULL, CXMLPointers * pCurrElement = NULL);
	void CreateXMLFromStructPaymentCardDetailsWithOutNodes( char * pszXMLOut, PAYMENT_CARD_DETAILS * StructIn, CXMLInterface * xmlInitDoc /*= NULL */, CXMLPointers * pCurrElement /*= NULL*/ );
	void CreateXmlFromStructLoyaltyCardDetailsWithOutNodes(char * pszXMLOut, LOYALTY_CARD_DETAILS * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement);//MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
	void CreateXmlFromStructLoyaltyInfo4WithOutNodes(char * pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO4 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement);  // RFUEL-520
	void CreateXmlFromStructLoyaltyInfo3WithOutNodes(char * pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO3 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement);  // RFUEL-520
	BOOL CreateExtraDataXml(char * pszXMLOut, void * vpStructIn, __int64 lXmlFlags);

	virtual void WriteToLog(char *sMsg, BOOL bWriteAlways = FALSE) = 0;
	void SplitValue(char *sMsg, unsigned char *pStrMSB, unsigned int lStrMSBLen, unsigned char *pStrLSB, unsigned int lStrLSBLen);

};																	 

#endif // !defined(AFX_XMLCONVERTOR_H__8E9D895B_42E4_4A4A_9A00_15F7D2ECFE63__INCLUDED_)
