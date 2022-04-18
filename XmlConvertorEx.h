// XmlConvertorEx.h: interface for the CXmlConvertorEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLCONVERTOREX_H__34E73055_8A8D_4D21_A526_19A25284F31B__INCLUDED_)
#define AFX_XMLCONVERTOREX_H__34E73055_8A8D_4D21_A526_19A25284F31B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////
//Only for the compilation of XmlConvertor 
//////////////////////////////////////////




#define _LOGMSG			_Module.m_server.m_cLogger
#define LOG_MGR_MSG			1 
#define LOG_MGR_CONSOLE		1

//////////////////////////////////////////

#include "XMLInterface.h"
#include "XmlConvertor.h"

class CXmlConvertorEx : public CXmlConvertor  
{
public:
	CXmlConvertorEx();
	virtual ~CXmlConvertorEx();

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
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_GENERAL_INFO * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, RECEIPT_EXTRA_INFO * StructOut);
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
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC4_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC3_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC5_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC6_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC7_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC8_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC9_EX * StructOut); //TD 38654
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC10 * StructOut);	//4.0.19.504
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC11 * StructOut);	//4.0.20.507
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC12 * StructOut);	//4.0.22.511 71868
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC13 * StructOut);	//4.0.22.511 71868

	//4.0.14.500
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, PAY_AT_PUMP_INFO * StructOut);
	BOOL CreateStructEx(char * pszXMLIn, void * vpStructOut, __int64 lXmlFlags);
	void CreateStructPayAtPumpInfoFromXmlWithOutNodes(char * pszXMLIn, PAY_AT_PUMP_INFO * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc);
	void CreateStructCardSaleInfoFromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_INFO_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc, CARD_SALE_EXTRA_DATA2_ * StructOut2 = NULL, CARD_SALE_EXTRA_DATA3_ * StructOut3 = NULL); //4.0.15.502	//4.0.18.670 73612 4.0.22.502
	
	//4.0.16.504
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_ * StructOut);
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, DALLAS_KEYS_INFO * StructOut);
	void CreateStructDallasKeysInfoFromXmlWithOutNodes(char * pszXMLIn, DALLAS_KEYS_INFO * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc);
	void CreateStructCardSaleExtraData4FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc);
	
	//4.0.18.501
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA5_ * StructOut);
	void CreateStructCardSaleExtraData5FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA5_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc);

	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL_CHIP_EXTRA_DATA* StructOut);	//4.0.20.503 66960
	BOOL ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA6_ * StructOut);
	void CreateStructCardSaleExtraData6FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA6_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc);

	
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
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_GENERAL_INFO * StructIn);
	BOOL ConvertFromStructToXml(char * pszXMLOut, RECEIPT_EXTRA_INFO * StructIn);
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
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC4_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC3_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC5_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC6_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC7_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC8_EX * StructIn); //TD 38654
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC9_EX * StructIn); //TD 38654	
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC10 * StructIn); //4.0.17.501 - CR 38162
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC11 * StructIn); //4.0.20.501 TD 60236
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC12 * StructIn); //4.0.22.511 71868
	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC13 * StructIn); //4.0.22.511 71868
	
	//4.0.14.500
	BOOL ConvertFromStructToXml(char * pszXMLOut, PAY_AT_PUMP_INFO * StructIn);
	BOOL CreateXmlEx(char * pszXMLOut, void * vpStructIn, __int64 lXmlFlags);
	void CreateXMLFromStructPayAtPumpInfo(char * pszXMLOut, PAY_AT_PUMP_INFO * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement);
	void CreateXMLFromStructCardSaleInfo(char * pszXMLOut, CARD_SALE_INFO_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement, CARD_SALE_EXTRA_DATA2_ * StructIn2 = NULL, CARD_SALE_EXTRA_DATA3_ * StructIn3 = NULL); //4.0.15.502	//4.0.18.670 73612 4.0.22.502


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

	BOOL ConvertFromStructToXml(char * pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO4 * StructIn);   // RFUEL-520	
	BOOL ConvertFromStructToXml(char* pszXMLOut, TAG_EXTRA_INFO* SrtuctIn); //RFUEL-520
	void CreateXmlFromStructLoyaltyInfo4WithOutNodes(char * pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO4 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement);	// RFUEL-520
	void CreateXmlFromStructLoyaltyInfo3WithOutNodes(char * pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO3 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement);  // RFUEL-520

	//4.0.15.10
	BOOL AppendTransactionStructToXml(CXMLInterface * xmlInitDocInOut, TAG_PUMP_TRANSACT2 * cTagPumpTrsIn);
	//TD 38654
	BOOL AppendTransactionStructToXml(CXMLInterface * xmlInitDocInOut, TAG_PUMP_TRANSACT5 * cTagPumpTrsIn);
	
	GCIRetCode GetXmlInterface(BSTR sData, long lOlaRecSize, CXMLInterface* cXmlInterface, int* ilen); //4.0.14.500
	
	GCIRetCode GetStructCardSaleAll3ChipExtra(BSTR sData, long lOlaRecSize, CARD_SALE_ALL_CHIP_EXTRA_DATA* cOutDataAll3ChipExtra, int* ilen); //4.0.20.503 66960

	// 4.0.15.500
	void WriteToLog(char *sMsg, BOOL bWriteAlways = FALSE);
	BOOL AddAttributeToXml(char * pszXMLIn, char * psAttributeName, char * psAttributeVal, long lAttributeSize, char * pszXMLOut);

	
};

#endif // !defined(AFX_XMLCONVERTOREX_H__34E73055_8A8D_4D21_A526_19A25284F31B__INCLUDED_)
