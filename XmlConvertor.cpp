//XmlConvertor.cpp: implementation of the CXmlConvertor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XmlConvertor.h"
#include <exception>
#include "CStrFormat.h"
#include "DynamicBuffer.h"

/* dead code, maybe used in the past, and possibly used in the future but not used now
static void AddToXml(CXMLInterface *xmlInitDoc, CXMLPointers *pCurrElement, char *psChildName, BYTE *value, long lStrLen)
{
	CXMLPointers	pParam1, pParam2;
	unsigned char  *tmpStr;
	BOOL bValid = TRUE;
	tmpStr = new unsigned char[lStrLen+1];

	tmpStr[lStrLen] = 0;
	memcpy(tmpStr, value, lStrLen);

	for(int i = 0; i < lStrLen; i++)
	{
		if(tmpStr[i] != ' ')
		{
			bValid = FALSE;
			break;
		}
	}
	if(!bValid)
	{
		xmlInitDoc->CreateChildElement(psChildName,(char*)tmpStr, &pParam1);
		xmlInitDoc->AppendChildElement(&pParam1, pCurrElement);
	}
	delete[] tmpStr;
}
*/

static void EndXmlLineWithNull(CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement, char * psAttributeName, 
	BYTE * value, unsigned int uStrLen)
{
	// RFUEL-2817 - optimized out unnecessary waste of time by identifying invalid data (data not going to be added to XML)	

	// bValueAllBlank TRUE if all data memory is filled with spaces
	const bool bValueAllBlank = std::all_of(value, value + uStrLen, [](BYTE Data){ return Data == ' ';});

	// if not everything blank, then we have valid data
	BOOL	 bValid = !bValueAllBlank;

	if (bValid)
	{
		// make fast copy into buffer, null terminate the data
		DynamicBuffer<80>	pBuffer((char*)value, uStrLen);

		xmlInitDoc->SetCharAttribute(psAttributeName, (char *)pBuffer, pCurrElement);
	}	
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlConvertor::CXmlConvertor()
{

}

CXmlConvertor::~CXmlConvertor()
{

}

/******************************* Convert from xml to struct functions ********************************

 Description:		Convert an xml to struct
 Returns:			BOOL l_bReturn - FALSE\TRUE
 Parameters:   		1.pszXMLIn - A string that holds the xml
					2.StructOut - The full struct.

 WHO				WHEN                    WHAT
------------------------------------------------------------------------------------------------------
 Ariel			8/11/2005   12:02			Start
******************************************************************************************************/
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL3* StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_ALL3_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//4.0.20.503 66960
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL_CHIP_EXTRA_DATA* StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_ALL_CHIP_EXTRA_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}



BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL2* StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_ALL2_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_ALL_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_DATA * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_DATA_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_EXTRA_DATA_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA2_ * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_EXTRA_DATA2_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA3_ * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_EXTRA_DATA3_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, GRADE_LIMITS  * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, GRADE_LIMITS_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, DISCNT * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, DISCNT_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, SALE * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, SALE_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}


BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, BAP_ITEM_FOR_RECEIPT_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT2 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, BAP_ITEM_FOR_RECEIPT2_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, BAP_ALL_ITEM_FOR_RECEIPT_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, BAP_ALL_ITEM_FOR_RECEIPT2_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, BAP_FUEL_ITEM * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, BAP_FUEL_ITEM_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, BAP_FUEL_ITEM_EX * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, BAP_FUEL_ITEM_EX_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_GENERAL_INFO * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_GENERAL_INFO_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, RECEIPT_EXTRA_INFO * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, RECEIPT_EXTRA_INFO_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}


BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA6 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_RECEIPT_GENERAL_DATA6_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}


BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA5 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_RECEIPT_GENERAL_DATA5_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA4 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_RECEIPT_GENERAL_DATA4_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA3 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_RECEIPT_GENERAL_DATA3_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_RECEIPT_GENERAL_DATA_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_SYSTEMTIME * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_SYSTEMTIME_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_PUMP_TRANSACT2 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_PUMP_TRANSACT2_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_PUMP_TRANSACT5 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_PUMP_TRANSACT5_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC13 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC13_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

//4.0.22.511 71868
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC12 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC12_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

//4.0.20.507
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC11 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC11_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

//4.0.17.501 - TD 38162
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC10 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC10_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC9 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC9_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC9_EX * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC9_EX_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC8 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC8_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}
//TD 38654
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC8_EX * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC8_EX_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC7 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC7_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC7_EX * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC7_EX_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC6 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC6_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC6_EX * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC6_EX_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC5 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC5_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC5_EX * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC5_EX_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC4 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC4_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}
//TD 38654
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC4_EX * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC4_EX_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC3 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC3_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}
//TD 38654
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC3_EX * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAG_OLA_REC3_EX_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}


BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, TAX_DATA_SUMMARY * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, TAX_DATA_SUMMARY_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_PLUS * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_EXTRA_DATA4_PLUS_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, RECEIPT_DATA4 * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, RECEIPT_DATA4_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, RECEIPT_DATA4_EX * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, RECEIPT_DATA4_EX_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

//4.0.16.504
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_ * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_EXTRA4_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

//4.0.18.501
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA5_ * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_EXTRA5_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

//4.0.16.504
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, DALLAS_KEYS_INFO * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, DALLAS_KEYS_INFO_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;

}

//4.0.17.501 - CR 38162
BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, DISCOUNT_ATTRIBUTES * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, DISCOUNT_ATTRIBUTES_INFO_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;

}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, DISCOUNT_ATTRIBUTES_EXT * StructOut)	//4.0.22.509 38162
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, DISCOUNT_ATTRIBUTES_EXT_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;

}

BOOL CXmlConvertor::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA6_ * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStruct(pszXMLIn, StructOut, CARD_SALE_EXTRA6_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}
	return l_bReturn;
}
/////////////////////////////////////////////////////
//****************************** convert from struct to xml functions ********************************
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_ALL3 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, CARD_SALE_ALL3_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_ALL2 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, CARD_SALE_ALL2_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_ALL * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, CARD_SALE_ALL_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_DATA * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, CARD_SALE_DATA_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, CARD_SALE_EXTRA_DATA_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA2_ * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, CARD_SALE_EXTRA_DATA2_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA3_ * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, CARD_SALE_EXTRA_DATA3_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, GRADE_LIMITS * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, GRADE_LIMITS_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, DISCNT * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, DISCNT_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, SALE * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, SALE_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, BAP_ITEM_FOR_RECEIPT_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT2 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, BAP_ITEM_FOR_RECEIPT2_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, BAP_ALL_ITEM_FOR_RECEIPT_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, BAP_ALL_ITEM_FOR_RECEIPT2_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}


BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, BAP_FUEL_ITEM * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, BAP_FUEL_ITEM_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, BAP_FUEL_ITEM_EX * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, BAP_FUEL_ITEM_EX_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_GENERAL_INFO * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_GENERAL_INFO_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, RECEIPT_EXTRA_INFO * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, RECEIPT_EXTRA_INFO_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}


BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA6 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_RECEIPT_GENERAL_DATA6_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}


BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA5 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_RECEIPT_GENERAL_DATA5_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA4 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_RECEIPT_GENERAL_DATA4_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA3 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_RECEIPT_GENERAL_DATA3_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_RECEIPT_GENERAL_DATA_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_SYSTEMTIME * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_SYSTEMTIME_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_PUMP_TRANSACT2 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_PUMP_TRANSACT2_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_PUMP_TRANSACT5 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_PUMP_TRANSACT5_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}


//4.0.22.511 71868
BOOL CXmlConvertor::ConvertFromStructToXml(char* pszXMLOut, TAG_OLA_REC12* StructIn)
{
	BOOL l_bReturn;

	try	{
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC12_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char* pszXMLOut, TAG_OLA_REC13* StructIn)
{
	BOOL l_bReturn;

	try	{
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC13_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//4.0.20.501 TD 60236
BOOL CXmlConvertor::ConvertFromStructToXml(char* pszXMLOut, TAG_OLA_REC11* StructIn)
{
	BOOL l_bReturn;

	try	{
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC11_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}

	return l_bReturn;
}


//4.0.17.501 - CR 38162
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC10 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC10_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC9 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC9_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}
	return l_bReturn;
}
//TD 38654
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC9_EX * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC9_EX_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC8 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC8_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC8_EX * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC8_EX_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC7 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC7_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC7_EX * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC7_EX_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC6 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC6_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC6_EX * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC6_EX_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC5 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC5_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC5_EX * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC5_EX_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC4 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC4_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC4_EX * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC4_EX_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC3 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC3_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC3_EX * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAG_OLA_REC3_EX_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAX_DATA_SUMMARY * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, TAX_DATA_SUMMARY_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA4_PLUS * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, CARD_SALE_EXTRA_DATA4_PLUS_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, RECEIPT_DATA4 * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, RECEIPT_DATA4_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//TD 38654
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, RECEIPT_DATA4_EX * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, RECEIPT_DATA4_EX_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//4.0.16.504
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA4_ * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, CARD_SALE_EXTRA4_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//4.0.18.501
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA5_ * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, CARD_SALE_EXTRA5_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA6_ * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, CARD_SALE_EXTRA6_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}



//4.0.16.504
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, DALLAS_KEYS_INFO * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, DALLAS_KEYS_INFO_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

//4.0.17.501 - CR 38162
BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, DISCOUNT_ATTRIBUTES * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, DISCOUNT_ATTRIBUTES_INFO_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, DISCOUNT_ATTRIBUTES_EXT* StructIn) //4.0.22.509 38162
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXml(pszXMLOut, StructIn, DISCOUNT_ATTRIBUTES_EXT_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO4* StructIn) // RFUEL-520
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateExtraDataXml(pszXMLOut, StructIn, DISCOUNT_LOYALTY_INFO4_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, TAG_EXTRA_INFO* StructIn) // RFUEL-520
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateExtraDataXml(pszXMLOut, StructIn, DISCOUNT_LOYALTY_INFO4_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}

	return l_bReturn;
}
/******************************* Create struct from xml with out nodes functions ********************************/

void CXmlConvertor::CreateStructCardSaleAll3FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_ALL3 * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers pChildNode, pChildNode2, pChildNode3;

	CreateStructCardSaleAllFromXmlWithOutNodes(pszXMLIn, &StructOut->CardSaleAll, pXml, xmlInitDoc, &StructOut->extData2, &StructOut->extData3); //4.0.15.502 //4.0.26.500 TD 146525
	CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &StructOut->extData2, pXml, xmlInitDoc);
	CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &StructOut->extData3, pXml, xmlInitDoc);
	CreateStructCardSaleExtraData4FromXmlWithOutNodes(pszXMLIn, &StructOut->extData4, pXml, xmlInitDoc); //4.0.17.501
	CreateStructCardSaleExtraData5FromXmlWithOutNodes(pszXMLIn, &StructOut->extData5, pXml, xmlInitDoc); //4.0.23.21
	CreateStructCardSaleExtraData6FromXmlWithOutNodes(pszXMLIn, &StructOut->extData6, pXml, xmlInitDoc); //4.0.23.21
}

void CXmlConvertor::CreateStructCardSaleAll2FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_ALL2 * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers	pChildNode, pChildNode2;

	CreateStructCardSaleAllFromXmlWithOutNodes(pszXMLIn, &StructOut->CardSaleAll, pXml, xmlInitDoc, &StructOut->extData2); //4.0.15.502
	CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &StructOut->extData2, pXml, xmlInitDoc);
}

void CXmlConvertor::CreateStructCardSaleAllFromXmlWithOutNodes  (char * pszXMLIn, CARD_SALE_ALL * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc, CARD_SALE_EXTRA_DATA2_ * StructOut2, CARD_SALE_EXTRA_DATA3_ * StructOut3) //4.0.15.502 //4.0.23.509 100885
{
	CXMLPointers	pChildNode, pChildNode2;

	CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &StructOut->data, pXml, xmlInitDoc, &StructOut->extData, StructOut2); //4.0.15.502
	CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &StructOut->extData, pXml, xmlInitDoc, StructOut3);//4.0.23.509 100885
}

void CXmlConvertor::CreateStructCardSaleDataFromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_DATA * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc, CARD_SALE_EXTRA_DATA * StructOut2, CARD_SALE_EXTRA_DATA2_ * StructOut3) //4.0.15.502
{
	CXMLPointers	pItemElement, pChildNode;
	char			tmpAuthAmt [10];
	char			tmpAuthorizeSessionID[6];	//4.0.15.502

	memset(&tmpAuthAmt, ' ', sizeof(tmpAuthAmt));
	memset(&tmpAuthorizeSessionID, ' ', sizeof(tmpAuthorizeSessionID)); //4.0.15.502

	if (StructOut2 == NULL)
		xmlInitDoc->GetAttributeToByteArrWithConvert("AuthAmt", StructOut->sAuthAmt, pXml);
	else	//split TotalAmt to two fields
	{
		xmlInitDoc->GetAttributeToByteArrWithConvert("AuthAmt", tmpAuthAmt, pXml);
		SplitValue(tmpAuthAmt, StructOut2->sAuthAmt_Msb, sizeof(StructOut2->sAuthAmt_Msb), StructOut->sAuthAmt, sizeof(StructOut->sAuthAmt)); //4.0.17.500
	//	SplitValue(tmpAuthAmt, StructOut->sAuthAmt, sizeof(StructOut->sAuthAmt), StructOut2->sAuthAmt_Msb, sizeof(StructOut2->sAuthAmt_Msb)); //4.0.16.40
	}

	//4.0.15.502
	if (StructOut3 == NULL)
		xmlInitDoc->GetAttributeToByteArrWithConvert("AuthorizeSessionID", StructOut->sAuthorizeSessionID, pXml);
	else	//split AuthorizeSessionID to two fields
	{
		xmlInitDoc->GetAttributeToByteArrWithConvert("AuthorizeSessionID", tmpAuthorizeSessionID, pXml);
		if (strlen(tmpAuthorizeSessionID) > 4)
			SplitValue(tmpAuthorizeSessionID, StructOut3->sAuthorizeSessionID_MSB, sizeof(StructOut3->sAuthorizeSessionID_MSB), StructOut->sAuthorizeSessionID, sizeof(StructOut->sAuthorizeSessionID));
		else
			xmlInitDoc->GetAttributeToByteArrWithConvert("AuthorizeSessionID", StructOut->sAuthorizeSessionID, pXml);
	}

	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDTranStatus", StructOut->sTranStatus, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TranType", StructOut->sTranType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("MID", StructOut->sMID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PumpNumber", StructOut->sPumpNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DeviceNumber", StructOut->sDeviceNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TranNbr", StructOut->sTranNbr, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RequestAmt", StructOut->sRequestAmt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("SurchargeAmt", StructOut->sSurchargeAmt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Track1Data", StructOut->sTrack1Data, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Track2Data", StructOut->sTrack2Data, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("StartDate", StructOut->sStartDate, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardIssue", StructOut->sCardIssue, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DriverId", StructOut->sDriverId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDOdometer", StructOut->sOdometer, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PINBlock", StructOut->sPINBlock, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReturnTank", StructOut->sReturnTank, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDReserved", StructOut->sReserved, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("EntryMethod", StructOut->sEntryMethod, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IsStoreAndForward", StructOut->sIsStoreAndForward, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDCardType", StructOut->sCardType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDFullVehicleId", StructOut->sFullVehicleId, pXml);
	//xmlInitDoc->GetAttributeToByteArrWithConvert("AuthorizeSessionID", StructOut->sAuthorizeSessionID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDAuthNumber", StructOut->sAuthNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDIsOnLine", StructOut->sIsOnLine, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ClearanceID", StructOut->sClearanceID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDAccountNumber", StructOut->sAccountNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDFullCardName", StructOut->sFullCardName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDDateTime", StructOut->sDateTime, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDNozzle", StructOut->sNozzle, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ResultMsg", StructOut->sResultMsg, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("MerchantID", StructOut->sMerchantID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("NozzleRestriction", StructOut->sNozzleRestriction, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReceiptRestriction", StructOut->sReceiptRestriction, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Approval", StructOut->sApproval, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ResInfoAMax8", StructOut->sResInfoAMax8, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDReferenceNumber", StructOut->sReferenceNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardName", StructOut->sCardName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDExpire", StructOut->sExpire, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GradeRestriction", StructOut->sGradeRestriction, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("HostTerminalID", StructOut->sHostTerminalID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GRPrompt", StructOut->sGRPrompt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVIEngineHours", StructOut->sAVIEngineHours, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVISystemCustomer", StructOut->sAVISystemCustomer, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVICompanyId", StructOut->sAVICompanyId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVIUserId", StructOut->sAVIUserId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVILimit", StructOut->sAVILimit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVILimitType", StructOut->sAVILimitType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVISpecialCodes", StructOut->sAVISpecialCodes, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ExtendPIN", StructOut->sExtendPIN, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FinalRefNumber", StructOut->sFinalRefNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FinalAuthNumber", StructOut->sFinalAuthNumber, pXml);
	//xmlInitDoc->GetAttributeToByteArrWithConvert("Spare5", StructOut->sSpare5, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GRVoucherNumber", StructOut->sGRVoucherNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GRKeyCode", StructOut->sGRKeyCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GRDiscountPerGallon", StructOut->sGRDiscountPerGallon, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GRMaxVolume", StructOut->sGRMaxVolume, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthTerminal", StructOut->sAuthTerminal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthVersion", StructOut->sAuthVersion, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GeneralID", StructOut->sGeneralID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSDTransBatch", StructOut->sTransBatch, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TransBatchSequence", StructOut->sTransBatchSequence, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ClubCardTrack2Data", StructOut->sClubCardTrack2Data, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthCode", StructOut->sAuthCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AppCodeElectr", StructOut->sAppCodeElectr, pXml);
}

void CXmlConvertor::CreateStructTaxDataSummaryFromXmlWithOutNodes  (char * pszXMLIn, TAX_DATA_SUMMARY * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxName", StructOut->sTaxName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxRate", StructOut->sTaxRate, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TDSTaxPrintChar", StructOut->sTaxPrintChar, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TDSTaxAmount", StructOut->sTaxAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxNetAmount", StructOut->sTaxNetAmount, pXml);
}

void CXmlConvertor::CreateStructCardSaleExtraDataFromXmlWithOutNodes  (char * pszXMLIn, CARD_SALE_EXTRA_DATA * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc, CARD_SALE_EXTRA_DATA3_ * StructOut3)	//4.0.22.502 73612
{
	CXMLPointers pItemElement, pChildNode;
	char tmpTotalAmount [10];
	char tmpFuelAmount [10];
	char tmpTaxAmt [8];	//4.0.18.670 73612	//4.0.22.502

	memset(&tmpTaxAmt, ' ', sizeof(tmpTaxAmt));

	memset(&tmpTotalAmount, ' ', sizeof(tmpTotalAmount));
	memset(&tmpFuelAmount, ' ', sizeof(tmpFuelAmount));

	if (StructOut3 == NULL)	//4.0.22.502 73612
	{
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmt", StructOut->sTaxAmt, pXml);
//		_LOGMSG.LogMsg("CXmlConvertor::CreateStructCardSaleExtraDataFromXmlWithOutNodes, No StructExt3");
	}
	else
	{
		//4.0.23.504 87373
		long lTaxAmtMsb2Size = sizeof(StructOut3->sTaxAmt_Msb2);
		long lTaxAmtMsbSize = sizeof(StructOut3->sTaxAmt_Msb);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmt", tmpTaxAmt, pXml);
		memcpy(&StructOut3->sTaxAmt_Msb2, (unsigned char *) tmpTaxAmt, lTaxAmtMsb2Size);
		memcpy(StructOut3->sTaxAmt_Msb, (unsigned char *) tmpTaxAmt + lTaxAmtMsb2Size, lTaxAmtMsbSize);
		memcpy(StructOut->sTaxAmt, (unsigned char *) tmpTaxAmt + lTaxAmtMsbSize + lTaxAmtMsb2Size, sizeof(StructOut->sTaxAmt));
		//4.0.23.504 87373SplitValue(tmpTaxAmt, StructOut3->sTaxAmt_Msb, sizeof(StructOut3->sTaxAmt_Msb), StructOut->sTaxAmt, sizeof(StructOut->sTaxAmt)); //4.0.17.500
	//	SplitValue(tmpAuthAmt, StructOut->sAuthAmt, sizeof(StructOut->sAuthAmt), StructOut2->sAuthAmt_Msb, sizeof(StructOut2->sAuthAmt_Msb)); //4.0.16.40
//		_LOGMSG.LogMsg("CXmlConvertor::CreateStructCardSaleExtraDataFromXmlWithOutNodes, Exists StructExt3");

	}

	BYTE CWProductCode[MAX_PRODUCT_CODE_LEN];  
	int CWIndex = a2i((BYTE *)StructOut->SALES[0].sDPTProdCode, sizeof(StructOut->SALES[0].sDPTProdCode));
	CString sProdcutCode(_Module.m_server.m_cCarWashProgramsDetails[CWIndex].sProductCode);
	memcpy(CWProductCode, _Module.m_server.m_cCarWashProgramsDetails[CWIndex].sProductCode, sizeof(CWProductCode));
	
	xmlInitDoc->GetAttributeToByteArrWithConvert("WashProductCode", CWProductCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("WashCode", StructOut->sWashCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("WashExpire", StructOut->sWashExpire, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PrnDateTime", StructOut->sPrnDateTime, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TotalAmt", tmpTotalAmount, pXml);
//4.0.22.502 73612	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmt", StructOut->sTaxAmt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyalId", StructOut->sLoyalId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyalScheme", StructOut->sLoyalScheme, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyalPoints", StructOut->sLoyalPoints, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("VAT_Rate", StructOut->sVAT_Rate, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReciptStatus", StructOut->sReciptStatus, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDReserved", StructOut->sReserved, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GRTotalDiscount", StructOut->sGRTotalDiscount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GRActualDiscountPerGallon", StructOut->sGRActualDiscountPerGallon, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("NumberOfExtraSale", StructOut->sNumberOfExtraSale, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FuelAmt", tmpFuelAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarWashProgram", StructOut->sCarWashProgram, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDBankTermId", StructOut->sBankTermId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDCustomerName", StructOut->sCustomerName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDISOResponse", StructOut->sISOResponse, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDSPDHCode", StructOut->sSPDHCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyActive", StructOut->sLoyaltyActive, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ForceCompleteZero", StructOut->sForceCompleteZero, pXml);
//	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthAmt_Msb", StructOut->sAuthAmt_Msb, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DateTimeExt", StructOut->sDateTimeExt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("OdometerExt", StructOut->sOdometerExt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CompleteDate", StructOut->sCompleteDate, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CompleteTime", StructOut->sCompleteTime, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AttandantSession", StructOut->sAttandantSession, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDSlipFlags", StructOut->sSlipFlags, pXml);
	StructOut->sSlipFlags -=48; //4.0.14.505

	xmlInitDoc->GetAttributeToByteArrWithConvert("RedeemPoints", StructOut->sRedeemPoints, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ConfirmPrompt", StructOut->sConfirmPrompt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CouponReward", StructOut->sCouponReward, pXml);

	//split TotalAmt to two fields
	SplitValue(tmpTotalAmount, StructOut->sTotalAmt_Msb, sizeof(StructOut->sTotalAmt_Msb), StructOut->sTotalAmt, sizeof(StructOut->sTotalAmt));
	//split FuelAmt to two fields
	SplitValue(tmpFuelAmount, StructOut->sFuelAmt_Msb, sizeof(StructOut->sFuelAmt_Msb), StructOut->sFuelAmt, sizeof(StructOut->sFuelAmt));

	int i = 0;
	BOOL bIsExists;

	bIsExists = xmlInitDoc->SelectSingleElementNode("Sales", &pChildNode, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetChildNodesList(&pChildNode, &pItemElement);
		while(xmlInitDoc->GetNextItem(pItemElement, pChildNode))
		{
			i = xmlInitDoc->GetAttributeToLong("ID", &pChildNode);
			if ((i >= 0) && (i < 4)) // Max size of SALES!
				CreateStructSaleFromXmlWithOutNodes(pszXMLIn, &StructOut->SALES[i], &pChildNode, xmlInitDoc);
		}
	}
}

void CXmlConvertor::CreateStructCardSaleExtraData2FromXmlWithOutNodes  (char * pszXMLIn, CARD_SALE_EXTRA_DATA2_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers	pItemElement, pChildNode;
	char			tempCompletedSessionID [6];
	char			tempPriceRecalcDiscount [7];

	memset(&tempCompletedSessionID, ' ', sizeof(tempCompletedSessionID));
	memset(&tempPriceRecalcDiscount, ' ', sizeof(tempPriceRecalcDiscount));

	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDSiteId", StructOut->sSiteId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Unit", StructOut->sUnit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TripNumber", StructOut->sTripNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CustomerReference", StructOut->sCustomerReference, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TractorHub", StructOut->sTractorHub, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TrailerNumber", StructOut->sTrailerNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TrailerHub", StructOut->sTrailerHub, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("OriginTrans", StructOut->sOriginTrans, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ScripUnitPrice", StructOut->sScripUnitPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("MarginUnitPrice", StructOut->sMarginUnitPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TotalScripAmount", StructOut->sTotalScripAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DeclinedLoyaltyPrompt", StructOut->sDeclinedLoyaltyPrompt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DeclinedLoyaltyResponse", StructOut->sDeclinedLoyaltyResponse, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDAccountType", StructOut->sAccountType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CustomerMessage", StructOut->sCustomerMessage, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDCouponMessage", StructOut->sCouponMessage, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDReserveForCoupon", StructOut->sReserveForCoupon, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CouponId", StructOut->sCouponId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyProgName", StructOut->sLoyaltyProgName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PassCode", StructOut->sPassCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GRDiscountType", StructOut->sGRDiscountType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDReceiptType", StructOut->sReceiptType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ServiceFee", StructOut->sServiceFee, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PrinterWidth", StructOut->sPrinterWidth, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ParamNumber", StructOut->sParamNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Param1", StructOut->sParam1, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Param2", StructOut->sParam2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PaymentStage", StructOut->sPaymentStage, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TextLength", StructOut->sTextLength, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FormattedAccount", StructOut->sFormattedAccount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AutoAuthorize", StructOut->sAutoAuthorize, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("WaitForReportFlag", StructOut->sWaitForReportFlag, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDInvoiceNumber", StructOut->sInvoiceNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDAuthorizationSerialNumber", StructOut->sAuthorizationSerialNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDAuthorizerFileNumber", StructOut->sAuthorizerFileNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PinPadAction", StructOut->sPinPadAction, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PinPadResult", StructOut->sPinPadResult, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CalculatedMAC", StructOut->sCalculatedMAC, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PinSessionKey", StructOut->sPinSessionKey, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("MACSessionKey", StructOut->sMACSessionKey, pXml);
//4.0.24.70 115660	4.0.24.71
	xmlInitDoc->GetAttributeToByteArrWithConvert("Language", StructOut->sLanguage, pXml);

	if ((StructOut->sLanguage  > '0') && (StructOut->sLanguage != ' '))   //4.0.23.360 4.0.24.71
		StructOut->sLanguage -= '0';

	xmlInitDoc->GetAttributeToByteArrWithConvert("PromotionNumber", StructOut->sPromotionNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PriceLevel", StructOut->sPriceLevel, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDSignatureSlip", StructOut->sSignatureSlip, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReceiptPrinted", StructOut->sReceiptPrinted, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ResInfoAMax37", StructOut->sResInfoAMax37, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDGradePrice", StructOut->sGradePrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TotalDiscount", StructOut->sTotalDiscount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("VolumeLimit", StructOut->sVolumeLimit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PriceRecalcDiscountSign", StructOut->sPriceRecalcDiscountSign, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TotalBalance", StructOut->sTotalBalance, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AttendantID", StructOut->sAttendantID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AttendantName", StructOut->sAttendantName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IgnoreServiceFee", StructOut->sIgnoreServiceFee, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("SessionOffLine", StructOut->sSessionOffLine, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ClockDateTime", StructOut->sClockDateTime, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ClockFlags", StructOut->sClockFlags, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemsLinkIndex", StructOut->sItemsLinkIndex, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("MessageTimeOut", StructOut->sMessageTimeOut, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Token", StructOut->sToken, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BarCode", StructOut->sBarCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Flags", StructOut->sFlags, pXml);
//	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthorizeSessionID_MSB", StructOut->sAuthorizeSessionID_MSB, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ZipCode", StructOut->sZipCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmt2", StructOut->sTaxAmt2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmt3", StructOut->sTaxAmt3, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CashDeposit1", StructOut->sCashDeposit1, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CashDeposit2", StructOut->sCashDeposit2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CashDeposit5", StructOut->sCashDeposit5, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CashDeposit10", StructOut->sCashDeposit10, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CashDeposit20", StructOut->sCashDeposit20, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CashDeposit50", StructOut->sCashDeposit50, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CashDeposit100", StructOut->sCashDeposit100, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CashDepositTotal", StructOut->sCashDepositTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CompletedSessionID", tempCompletedSessionID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthorizerID", StructOut->sAuthorizerID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyTotalAmount", StructOut->sLoyaltyTotalAmount, pXml);
//	xmlInitDoc->GetAttributeToByteArrWithConvert("PriceRecalcDiscount_Msb", StructOut->sPriceRecalcDiscount_Msb, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("MediaMasterType", StructOut->sMediaMasterType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DisplayOLAResultMessage", StructOut->sDisplayOLAResultMessage, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ChargeTo", StructOut->sChargeTo, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVIFuelCode", StructOut->sAVIFuelCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PriceRecalcDiscount", tempPriceRecalcDiscount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RefundAmount", StructOut->sRefundAmount, pXml);

	//split CompletedSessionID to two fields
	SplitValue(tempCompletedSessionID, StructOut->sCompletedSessionID_MSB, sizeof(StructOut->sCompletedSessionID_MSB), StructOut->sCompletedSessionID, sizeof(StructOut->sCompletedSessionID));
	//split PriceRecalcDiscount to two fields
	SplitValue(tempPriceRecalcDiscount, StructOut->sPriceRecalcDiscount_Msb, sizeof(StructOut->sPriceRecalcDiscount_Msb), StructOut->sPriceRecalcDiscount, sizeof(StructOut->sPriceRecalcDiscount));

	int i = 0;
	BOOL bIsExists;

	bIsExists = xmlInitDoc->SelectSingleElementNode("Discounts", &pChildNode, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetChildNodesList(&pChildNode, &pItemElement);

		while(xmlInitDoc->GetNextItem(pItemElement, pChildNode))
		{
			i = xmlInitDoc->GetAttributeToLong("ID", &pChildNode);
			if ((i >= 0) && (i < MAX_GRADES)) // Max size of sDiscountPerGrade.
				CreateStructDiscntFromXmlWithOutNodes(pszXMLIn, &StructOut->sDiscountPerGrade[i], &pChildNode, xmlInitDoc);
		}
	}
}

void CXmlConvertor::CreateStructCardSaleExtraData3FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA3_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers pItemElement, pChildNode;

	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDTotalRedeemedPoints", StructOut->sTotalRedeemedPoints, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDAmountByPoints", StructOut->sAmountByPoints, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDCreditAmount", StructOut->sCreditAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ControllerSeqNum", StructOut->sControllerSeqNum, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReSend", StructOut->sReSend, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RemoteID", StructOut->sRemoteID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarWashDiscount", StructOut->sCarWashDiscount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarWashLimit", StructOut->sCarWashLimit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ParamResult", StructOut->sParamResult, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromptType", StructOut->sPromptType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromptMinLen", StructOut->sPromptMinLen, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromptMaxLen", StructOut->sPromptMaxLen, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ParamIndex", StructOut->sParamIndex, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarwashRestriction", StructOut->sCarwashRestriction, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReservedForfutureCarWash", StructOut->sReservedForfutureCarWash, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DriverId_MSB", StructOut->sDriverId_MSB, pXml); //4.0.23.370 117473 //4.0.24.90
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDLogicShiftNumber", StructOut->sLogicShiftNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Flags1", StructOut->sFlags1, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("OriginalUnitPrice", StructOut->sOriginalUnitPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("NonDiscountTotalAmount", StructOut->sNonDiscountTotalAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("OfflineTrans", StructOut->sOfflineTrans, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RequiredOlaData", StructOut->sRequiredOlaData, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("UnitPriceIncludeDiscount", StructOut->sUnitPriceIncludeDiscount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Threshold", StructOut->sThreshold, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ThresholdType", StructOut->sThresholdType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ClockRequired", StructOut->sClockRequired, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CSEDReceiptMode", StructOut->sReceiptMode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CustomerID", StructOut->sCustomerID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PreviousMediaMasterType", StructOut->sPreviousMediaMasterType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ManagerID", StructOut->sManagerID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DoSessionAuth", StructOut->sDoSessionAuth, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("WaitForPriceChange", StructOut->sWaitForPriceChange, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PMTReferenceNumber", StructOut->sPMTReferenceNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemRestriction", StructOut->sItemRestriction, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DieselLimit", StructOut->sDieselLimit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReeferLimit", StructOut->sReeferLimit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CashBacklimit", StructOut->sCashBacklimit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReeferTotal", StructOut->sReeferTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromptName", StructOut->sPromptName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromptMode", StructOut->sPromptMode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromptEncrypted", StructOut->sPromptEncrypted, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("WaitPromptNeedMoreParam", StructOut->sWaitPromptNeedMoreParam, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ClFlags", StructOut->sClFlags, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("OperationalPrompt", StructOut->sOperationalPrompt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RestrictionItemRequired", StructOut->sRestrictionItemRequired, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReeferVolume", StructOut->sReeferVolume, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DryItemsRestriction", StructOut->sDryItemsRestriction, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PositionRestrictionActive", StructOut->sPositionRestrictionActive, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Track3Data", StructOut->sTrack3Data, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AmountLimitIncludesCashBack", StructOut->sAmountLimitIncludesCashBack, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DisplayCashBackAmount", StructOut->sDisplayCashBackAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AllowPartialCashBack", StructOut->sAllowPartialCashBack, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("OLABatchId", StructOut->sOLABatchId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CustomerValidationType", StructOut->sCustomerValidationType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PreSetlimit", StructOut->sPreSetlimit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PreSetlimitType", StructOut->sPreSetlimitType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TruckingCompanyName", StructOut->sTruckingCompanyName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("MagCapabilities", StructOut->sMagCapabilities, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IssueNumber", StructOut->sIssueNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthorizerSystemID", StructOut->sAuthorizerSystemID, pXml);  //4.0.23.360 //4.0.24.71

	xmlInitDoc->GetAttributeToByteArrWithConvert("TerminalType", StructOut->sTerminalType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TerminalCountryCode", StructOut->sTerminalCountryCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TransactionCurrencyCode", StructOut->sTransactionCurrencyCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AIP", StructOut->sAIP, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ATC", StructOut->sATC, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TVR", StructOut->sTVR, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AID", StructOut->sAID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromotionTotalDiscount", StructOut->sPromotionTotalDiscount, pXml); //4.0.19.810 66705 //4.0.23.504
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVN", StructOut->sAVN, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IssuerApplicationData", StructOut->sIssuerApplicationData, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CryptogramInfoData", StructOut->sCryptogramInfoData, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CVMResult", StructOut->sCVMResult, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CombinedCardSelect", StructOut->sCombinedCardSelect, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TermResponseCode", StructOut->sTermResponseCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ScreenPrompt", StructOut->sScreenPrompt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TimeCardInserted", StructOut->sTimeCardInserted, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TimeTRSRequired", StructOut->sTimeTRSRequired, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TimeTRSResponse", StructOut->sTimeTRSResponse, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TimeHCResponse", StructOut->sTimeHCResponse, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TimePINRequest", StructOut->sTimePINRequest, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TimeOLARequest", StructOut->sTimeOLARequest, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TimeOLAResult", StructOut->sTimeOLAResult, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TimeTNXDetails", StructOut->sTimeTNXDetails, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IssuerActiondesDenial", StructOut->sIssuerActiondesDenial, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IssuerActiondesOnLinel", StructOut->sIssuerActiondesOnLinel, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IssuerActiondesDefault", StructOut->sIssuerActiondesDefault, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TransactionStatusMode", StructOut->sTransactionStatusMode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ControllerRestriction", StructOut->sControllerRestriction, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxId", StructOut->sTaxId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltySessionID", StructOut->sLoyaltySessionID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AmountByPoints2", StructOut->sAmountByPoints2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromotionNumber2", StructOut->sPromotionNumber2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("OLAQualifySpend0", StructOut->sOLAQualifySpend0, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("OLAQualifySpend1", StructOut->sOLAQualifySpend1, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardHolderVerifyMethod", StructOut->sCardHolderVerifyMethod, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("HostOnLineID", StructOut->sHostOnLineID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReasonOnLineCode", StructOut->sReasonOnLineCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IssueDate", StructOut->sIssueDate, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PONumber", StructOut->sPONumber, pXml); //4.0.15.502
	xmlInitDoc->GetAttributeToByteArrWithConvert("DoNotPrintExpirationDate", StructOut->sDoNotPrintExpirationDate, pXml); //4.0.23.500
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxIncluded", StructOut->sTaxIncluded, pXml); //4.0.24.60 109690
	xmlInitDoc->GetAttributeToByteArrWithConvert("KeyboardMask", StructOut->sKeyboardMask, pXml); //4.0.16.70
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardSegment", StructOut->sCardSegment, pXml); //Liat LPE
	xmlInitDoc->GetAttributeToByteArrWithConvert("IssuerAuthenticationData", StructOut->sIssuerAuthData, pXml); //TD 44410 4.0.15.101
	xmlInitDoc->GetAttributeToByteArrWithConvert("UnrecognizedBillFound", StructOut->sUnrecognizedBillFound, pXml); //4.0.17.90
	xmlInitDoc->GetAttributeToByteArrWithConvert("OverfillAdjusted", StructOut->sOverfillAdjusted, pXml); //4.0.23.500
	xmlInitDoc->GetAttributeToByteArrWithConvert("MaskAccountNumber", StructOut->sMaskAccountNumber, pXml); //4.0.25.80
	xmlInitDoc->GetAttributeToByteArrWithConvert("ChargeTo_MSB", StructOut->sChargeTo_MSB, pXml); //4.0.27.44 from 1023 TD 226700

	CreateStructGradeLimitFromXmlWithOutNodes(xmlInitDoc, pXml, pszXMLIn, StructOut);
}


void CXmlConvertor::CreateStructAmountGradeLimitFromXmlWithOutNodes  (char * pszXMLIn, GRADE_LIMITS * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	if(xmlInitDoc->GetAttribute("GradeAmountLimit", pXml)) // TD 370688
	{
		xmlInitDoc->GetAttributeToByteArrWithConvert("GradeAmountLimit", StructOut->sGradeAmountLimit, pXml);
		xmlInitDoc->GetAttributeToByteArrWithConvert("GradeID", StructOut->sGradeID, pXml);
		xmlInitDoc->GetAttributeToByteArrWithConvert("LimitType", StructOut->sLimitType, pXml);
	}
}


void CXmlConvertor::CreateStructVolumeGradeLimitFromXmlWithOutNodes  (char * pszXMLIn, GRADE_LIMITS * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	if(xmlInitDoc->GetAttribute("GradeVolumeLimit", pXml)) // TD 370688
	{
		xmlInitDoc->GetAttributeToByteArrWithConvert("GradeVolumeLimit", StructOut->sGradeAmountLimit, pXml);
		xmlInitDoc->GetAttributeToByteArrWithConvert("GradeID", StructOut->sGradeID, pXml);
		xmlInitDoc->GetAttributeToByteArrWithConvert("LimitType", StructOut->sLimitType, pXml);
	}
}


void CXmlConvertor::CreateStructDiscntFromXmlWithOutNodes(char * pszXMLIn, DISCNT * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("Discount", StructOut->sDiscount, pXml);
}

void CXmlConvertor::CreateStructSaleFromXmlWithOutNodes(char * pszXMLIn, SALE * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("ProdDesc", StructOut->sProdDesc, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DPTProdCode", StructOut->sDPTProdCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Qty", StructOut->sQty, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("UnitPrice", StructOut->sUnitPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("SaleDiscount", StructOut->sDiscount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Tax", StructOut->sTax, pXml);
}

void CXmlConvertor::CreateStructBuyAtPumpItemForReceiptFromXmlWithOutNodes(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("BOItemID", StructOut->sBOItemID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Quantity", StructOut->sQuantity, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemCode", StructOut->sItemCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemBarcode", StructOut->sItemBarcode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemName", StructOut->sItemName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxPrintChar", StructOut->sTaxPrintChar, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("UnitPrice", StructOut->sUnitPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AmountTaxIncluded", StructOut->sAmountTaxIncluded, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AmountTaxExluded", StructOut->sAmountTaxExluded, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmount", StructOut->sTaxAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromotionVal", StructOut->sPromotionVal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DiscountVal", StructOut->sDiscountVal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IsFuelItem", StructOut->byIsFuelItem, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FuelVolume", StructOut->sFuelVolume, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IsCarWashItem", StructOut->byIsCarWashItem, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarWashCode", StructOut->sCarWashCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarWashExpire", StructOut->sCarWashExpire, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmount2", StructOut->sTaxAmount2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmount3", StructOut->sTaxAmount3, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemIsTaxIncluded", StructOut->byItemIsTaxIncluded, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IsWeightItem", StructOut->byIsWeightItem, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemSaleStatus", StructOut->byItemSaleStatus, pXml);
//	xmlInitDoc->GetAttributeToByteArrWithConvert("Buff", StructOut->sBuff, pXml);
}

void CXmlConvertor::CreateStructBuyAtPumpItemForReceipt2FromXmlWithOutNodes(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT2 * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemDiscPerUnit", StructOut->byItemDiscPerUnit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemUnitPriceWithDisc", StructOut->byItemUnitPriceWithDisc, pXml);
//	xmlInitDoc->GetAttributeToByteArrWithConvert("sFiller", StructOut->sFiller, pXml);
}

void CXmlConvertor::CreateTwoStructBuyAtPumpWithOutNodes(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT * StructOut, BAP_ITEM_FOR_RECEIPT2 * StructOut2, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("BOItemID", StructOut->sBOItemID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Quantity", StructOut->sQuantity, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemCode", StructOut->sItemCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemBarcode", StructOut->sItemBarcode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemName", StructOut->sItemName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxPrintChar", StructOut->sTaxPrintChar, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("UnitPrice", StructOut->sUnitPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AmountTaxIncluded", StructOut->sAmountTaxIncluded, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AmountTaxExluded", StructOut->sAmountTaxExluded, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmount", StructOut->sTaxAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromotionVal", StructOut->sPromotionVal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DiscountVal", StructOut->sDiscountVal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IsFuelItem", StructOut->byIsFuelItem, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FuelVolume", StructOut->sFuelVolume, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IsCarWashItem", StructOut->byIsCarWashItem, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarWashCode", StructOut->sCarWashCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarWashExpire", StructOut->sCarWashExpire, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmount2", StructOut->sTaxAmount2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmount3", StructOut->sTaxAmount3, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemIsTaxIncluded", StructOut->byItemIsTaxIncluded, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IsWeightItem", StructOut->byIsWeightItem, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemSaleStatus", StructOut->byItemSaleStatus, pXml);
//	xmlInitDoc->GetAttributeToByteArrWithConvert("Buff", StructOut->sBuff, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemDiscPerUnit", StructOut2->byItemDiscPerUnit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemUnitPriceWithDisc", StructOut2->byItemUnitPriceWithDisc, pXml);
//	xmlInitDoc->GetAttributeToByteArrWithConvert("Filler", StructOut2->sFiller, pXml);
}

void CXmlConvertor::CreateStructBuyAtPumpAllItemsForReceiptFromXmlWithOutNodes  (char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers	pItemElement, pChildNode;

	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemsNumber", StructOut->sItemsNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TenderTrans", StructOut->sTenderTrans, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemsTotal", StructOut->sItemsTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromotionTotal", StructOut->sPromotionTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DiscountTotal", StructOut->sDiscountTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotal", StructOut->sTaxTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TransactionTotal", StructOut->sTransactionTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPInvoiceNumber", StructOut->sInvoiceNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPFullCardName", StructOut->sFullCardName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPCardType", StructOut->sCardType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAuthNumber", StructOut->sAuthNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAccountNumber", StructOut->sAccountNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPExpire", StructOut->sExpire, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPDateTime", StructOut->sDateTime, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPFullVehicleId", StructOut->sFullVehicleId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("MagKey", StructOut->sMagKey, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPOdometer", StructOut->sOdometer, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPBankTermId", StructOut->sBankTermId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPSiteId", StructOut->sSiteId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPTransBatch", StructOut->sTransBatch, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPReferenceNumber", StructOut->sReferenceNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPReceiptType", StructOut->sReceiptType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPSignatureSlip", StructOut->bySignatureSlip, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPIsOnLine", StructOut->byIsOnLine, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("NonFuelTotal_TaxInc", StructOut->sNonFuelTotal_TaxInc, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("NonFuelTotal_TaxExc", StructOut->sNonFuelTotal_TaxExc, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAuthorizationSerialNumber", StructOut->sAuthorizationSerialNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAuthorizerFileNumber", StructOut->sAuthorizerFileNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TotalRemainingBalance", StructOut->sTotalRemainingBalance, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAccountType", StructOut->sAccountType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ABANumber", StructOut->sABANumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Branch", StructOut->sBranch, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CheckSerial", StructOut->sCheckSerial, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPISOResponse", StructOut->sISOResponse, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPSPDHCode", StructOut->sSPDHCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPTranStatus", StructOut->sTranStatus, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("SlipFlags", StructOut->sSlipFlags, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthorizationDateTime", StructOut->sAuthorizationDateTime, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LanguageId", StructOut->sLanguageId, pXml);

	if ((StructOut->sLanguageId  > '0') && (StructOut->sLanguageId != ' '))   //4.0.23.360 //4.0.24.71 115660
		StructOut->sLanguageId -= '0';

	xmlInitDoc->GetAttributeToByteArrWithConvert("TrsRoundingAmount", StructOut->sTrsRoundingAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TransactionNum", StructOut->sTransactionNum, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TenderAmount", StructOut->sTenderAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ChangeAmount", StructOut->sChangeAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPTotalRedeemedPoints", StructOut->sTotalRedeemedPoints, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAmountByPoints", StructOut->sAmountByPoints, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPCreditAmount", StructOut->sCreditAmount, pXml);


	int i = 0;
	BOOL bIsExists;

	bIsExists =	xmlInitDoc->SelectSingleElementNode("Items", &pChildNode, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetChildNodesList(&pChildNode, &pItemElement);

		while(xmlInitDoc->GetNextItem(pItemElement, pChildNode))
		{
			i = xmlInitDoc->GetAttributeToLong("ID", &pChildNode);
			if ((i >= 0) && (i < MAX_ITEMS_NUM)) // Max size of Items
				CreateStructBuyAtPumpItemForReceiptFromXmlWithOutNodes(pszXMLIn, &StructOut->Items[i], &pChildNode, xmlInitDoc);
		}
	}
}

void CXmlConvertor::CreateStructBuyAtPumpAllItemsForReceipt2FromXmlWithOutNodes  (char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers	pItemElement, pChildNode, pChildNode2;

	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPCouponMessage", StructOut->sCouponMessage, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPCustomerName", StructOut->sCustomerName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CustomerNewObligo", StructOut->sCustomerNewObligo, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Tmp", StructOut->sTmp, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAttributes", StructOut->sTaxAttributes, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPLogicShiftNumber", StructOut->sLogicShiftNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPReserveForCoupon", StructOut->sReserveForCoupon, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPReceiptMode", StructOut->sReceiptMode, pXml);
//	xmlInitDoc->GetAttributeToByteArrWithConvert("Filler", StructOut->sFiller, pXml);


	int i = 0;
	BOOL bIsExists;

	bIsExists = xmlInitDoc->SelectSingleElementNode("Items", &pChildNode, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetChildNodesList(&pChildNode, &pItemElement);

		while(xmlInitDoc->GetNextItem(pItemElement, pChildNode))
		{
			i = xmlInitDoc->GetAttributeToLong("ID", &pChildNode);
			if ((i >= 0) && (i < MAX_ITEMS_NUM)) // Max size of Items
				CreateStructBuyAtPumpItemForReceipt2FromXmlWithOutNodes(pszXMLIn, &StructOut->Items[i], &pChildNode, xmlInitDoc);
		}
	}

	i=0;
	bIsExists = xmlInitDoc->SelectSingleElementNode("TaxTotalList", &pChildNode2, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotalList0", StructOut->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotalList1", StructOut->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotalList2", StructOut->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotalList3", StructOut->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotalList4", StructOut->sTaxTotalList[i++], &pChildNode2);
	}

	i=0;
	bIsExists = xmlInitDoc->SelectSingleElementNode("TaxNameList", &pChildNode2, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxNameList0", StructOut->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxNameList1", StructOut->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxNameList2", StructOut->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxNameList3", StructOut->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxNameList4", StructOut->sTaxTotalList[i++], &pChildNode2);
	}
}

void CXmlConvertor::CreateTwoStructBuyAtPumpAllWithOutNodes(char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT * StructOut, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructOut2, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	BOOL			bIsExists;
	CXMLPointers	pItemElement, pChildNode, pChildNode2;

	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemsNumber", StructOut->sItemsNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TenderTrans", StructOut->sTenderTrans, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ItemsTotal", StructOut->sItemsTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromotionTotal", StructOut->sPromotionTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DiscountTotal", StructOut->sDiscountTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotal", StructOut->sTaxTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TransactionTotal", StructOut->sTransactionTotal, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPInvoiceNumber", StructOut->sInvoiceNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPFullCardName", StructOut->sFullCardName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPCardType", StructOut->sCardType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAuthNumber", StructOut->sAuthNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAccountNumber", StructOut->sAccountNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPExpire", StructOut->sExpire, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPDateTime", StructOut->sDateTime, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPFullVehicleId", StructOut->sFullVehicleId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("MagKey", StructOut->sMagKey, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPOdometer", StructOut->sOdometer, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPBankTermId", StructOut->sBankTermId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPSiteId", StructOut->sSiteId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPTransBatch", StructOut->sTransBatch, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPReferenceNumber", StructOut->sReferenceNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPReceiptType", StructOut->sReceiptType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPSignatureSlip", StructOut->bySignatureSlip, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPIsOnLine", StructOut->byIsOnLine, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("NonFuelTotal_TaxInc", StructOut->sNonFuelTotal_TaxInc, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("NonFuelTotal_TaxExc", StructOut->sNonFuelTotal_TaxExc, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAuthorizationSerialNumber", StructOut->sAuthorizationSerialNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAuthorizerFileNumber", StructOut->sAuthorizerFileNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TotalRemainingBalance", StructOut->sTotalRemainingBalance, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAccountType", StructOut->sAccountType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ABANumber", StructOut->sABANumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Branch", StructOut->sBranch, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CheckSerial", StructOut->sCheckSerial, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPISOResponse", StructOut->sISOResponse, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPSPDHCode", StructOut->sSPDHCode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPTranStatus", StructOut->sTranStatus, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPSlipFlags", StructOut->sSlipFlags, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthorizationDateTime", StructOut->sAuthorizationDateTime, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LanguageId", StructOut->sLanguageId, pXml);
	if ((StructOut->sLanguageId  > '0') && (StructOut->sLanguageId != ' '))   //4.0.23.360 //4.0.24.71
		StructOut->sLanguageId -= '0';

	xmlInitDoc->GetAttributeToByteArrWithConvert("TrsRoundingAmount", StructOut->sTrsRoundingAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TransactionNum", StructOut->sTransactionNum, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TenderAmount", StructOut->sTenderAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ChangeAmount", StructOut->sChangeAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPTotalRedeemedPoints", StructOut->sTotalRedeemedPoints, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPAmountByPoints", StructOut->sAmountByPoints, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPCreditAmount", StructOut->sCreditAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPCouponMessage", StructOut2->sCouponMessage, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPCustomerName", StructOut2->sCustomerName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CustomerNewObligo", StructOut2->sCustomerNewObligo, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Tmp", StructOut2->sTmp, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAttributes", StructOut2->sTaxAttributes, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPLogicShiftNumber", StructOut2->sLogicShiftNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPReserveForCoupon", StructOut2->sReserveForCoupon, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("BAPReceiptMode", StructOut2->sReceiptMode, pXml);
//	xmlInitDoc->GetAttributeToByteArrWithConvert("Filler", StructOut2->sFiller, pXml);

	int i = 0;
	bIsExists = xmlInitDoc->SelectSingleElementNode("Items", &pChildNode, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetChildNodesList(&pChildNode, &pItemElement);

		while(xmlInitDoc->GetNextItem(pItemElement, pChildNode))
		{
			i = xmlInitDoc->GetAttributeToLong("ID", &pChildNode);
			if ((i >= 0) && (i < MAX_ITEMS_NUM)) // Items Max size.
				CreateTwoStructBuyAtPumpWithOutNodes(pszXMLIn, &StructOut->Items[i], &StructOut2->Items[i], &pChildNode, xmlInitDoc);
		}
	}

	i=0;
	bIsExists = xmlInitDoc->SelectSingleElementNode("TaxTotalList", &pChildNode2, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotalList0", StructOut2->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotalList1", StructOut2->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotalList2", StructOut2->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotalList3", StructOut2->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxTotalList4", StructOut2->sTaxTotalList[i++], &pChildNode2);
	}

	i=0;
	bIsExists = xmlInitDoc->SelectSingleElementNode("TaxNameList", &pChildNode2, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxNameList0", StructOut2->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxNameList1", StructOut2->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxNameList2", StructOut2->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxNameList3", StructOut2->sTaxTotalList[i++], &pChildNode2);
		xmlInitDoc->GetAttributeToByteArrWithConvert("TaxNameList4", StructOut2->sTaxTotalList[i++], &pChildNode2);
	}
}


void CXmlConvertor::CreateStructTagGeneralInfoFromXmlWithOutNodes  (char * pszXMLIn, TAG_GENERAL_INFO * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers			pItemElement, pChildNode;

	xmlInitDoc->GetAttributeToByteArrWithConvert("Volume", StructOut->sVolume, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RECGradePrice", StructOut->sGradePrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Grade", StructOut->sGrade, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FuelValue", StructOut->sFuelValue, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReceiptNumber", StructOut->sReceiptNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GradeName", StructOut->cGradeName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PathToPumpSrv", StructOut->cPathToPumpSrv, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("VolumeFactor", StructOut->sVolumeFactor, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CurrencyFactor", StructOut->sCurrencyFactor, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IsOriginalCopy", StructOut->byIsOriginalCopy, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("HeaderDynamicBuffer", StructOut->cHeaderDynamicBuffer, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("HeaderDynamicBufferLen", StructOut->sHeaderDynamicBufferLen, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FooterDynamicBuffer", StructOut->cFooterDynamicBuffer, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FooterDynamicBufferLen", StructOut->sFooterDynamicBufferLen, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarWashProgramName", StructOut->cCarWashProgramName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarWashTotalPrice", StructOut->sCarWashTotalPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CouponExpDate", StructOut->sCouponExpDate, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TransactionNumber", StructOut->sTransactionNumber, pXml);
}

void CXmlConvertor::CreateStructReceiptExtraInfoFromXmlWithOutNodes  (char * pszXMLIn, RECEIPT_EXTRA_INFO * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers	pItemElement, pChildNode;

	xmlInitDoc->GetAttributeToByteArrWithConvert("Filer2043", StructOut->sFiler2043, pXml);
}

void CXmlConvertor::CreateStructTagPumpTransact2FromXmlWithOutNodes(char * pszXMLIn, TAG_PUMP_TRANSACT2 * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers	pChildNode;
	char			tmpMisc [10];

	memset(&tmpMisc, ' ', sizeof(tmpMisc));

	xmlInitDoc->GetAttributeToByteArrWithConvert("Number", StructOut->sNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Status", StructOut->sStatus, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("OnOffer", StructOut->sOnOffer, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RawVolume", StructOut->sRawVolume, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RoundedVolume", StructOut->sRoundedVolume, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RoundedValue", StructOut->sRoundedValue, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TRGradePrice", StructOut->sGradePrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TRNozzle", StructOut->sNozzle, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Grade", StructOut->sGrade, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Level", StructOut->sLevel, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Pump", StructOut->sPump, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Mode", StructOut->sMode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Res", StructOut->sRes, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ResExt", StructOut->sResExt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LinkNumber", StructOut->sLinkNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LinkFlags", StructOut->sLinkFlags, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ActiveReadNumber", StructOut->sActiveReadNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ActiveShiftNumber", StructOut->sActiveShiftNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Misc", tmpMisc, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PrePaidValue", StructOut->sPrePaidValue, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ServiceMode", StructOut->sServiceMode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Offers2Pos", StructOut->sOffers2Pos, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Grade2", StructOut->sGrade2, pXml);

	#ifdef _DEBUG
	xmlInitDoc->SaveToFile("c:\\Trs.xml");
	#endif
	//split Misc to two fields
	SplitValue(tmpMisc, StructOut->sMiscH, sizeof(StructOut->sMiscH), StructOut->sMisc, sizeof(StructOut->sMisc));

	BOOL bIsExists;
	bIsExists = xmlInitDoc->SelectSingleElementNode("TAG_SYSTEMTIME", &pChildNode, pXml);
	if (bIsExists)
		CreateStructTagSystemTimeFromXmlWithOutNodes(pszXMLIn, &StructOut->sDateTime, &pChildNode, xmlInitDoc);
}

//TD 38654
void CXmlConvertor::CreateStructTagPumpTransact5FromXmlWithOutNodes(char * pszXMLIn, TAG_PUMP_TRANSACT5 * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers	pChildNode;
	char			tmpMisc[10];

	memset(&tmpMisc, ' ', sizeof(tmpMisc));

	xmlInitDoc->GetAttributeToByteArrWithConvert("Number", StructOut->sNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Status", StructOut->sStatus, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("OnOffer", StructOut->sOnOffer, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RawVolume", StructOut->sRawVolume, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RoundedVolume", StructOut->sRoundedVolume, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RoundedValue", StructOut->sRoundedValue, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TRGradePrice", StructOut->sGradePrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TRNozzle", StructOut->sNozzle, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Grade", StructOut->sGrade, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Level", StructOut->sLevel, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Pump", StructOut->sPump, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Mode", StructOut->sMode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Res", StructOut->sRes, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ResExt", StructOut->sResExt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LinkNumber", StructOut->sLinkNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LinkFlags", StructOut->sLinkFlags, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ActiveReadNumber", StructOut->sActiveReadNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ActiveShiftNumber", StructOut->sActiveShiftNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Misc", tmpMisc, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PrePaidValue", StructOut->sPrePaidValue, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ServiceMode", StructOut->sServiceMode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Offers2Pos", StructOut->sOffers2Pos, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Grade2", StructOut->sGrade2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyPriceProtection", StructOut->sLoyaltyPriceProtection, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyAutoArm", StructOut->sLoyaltyAutoArm, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LowestGradePrice", StructOut->sLowestGradePrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TankId", StructOut->sTankID, pXml); //4.0.27.60   TD 231475

	#ifdef _DEBUG
	xmlInitDoc->SaveToFile("c:\\Trs.xml");
	#endif
	//split Misc to two fields
	SplitValue(tmpMisc, StructOut->sMiscH, sizeof(StructOut->sMiscH), StructOut->sMisc, sizeof(StructOut->sMisc));

	BOOL bIsExists;
	bIsExists = xmlInitDoc->SelectSingleElementNode("TAG_SYSTEMTIME", &pChildNode, pXml);
	if (bIsExists)
		CreateStructTagSystemTimeFromXmlWithOutNodes(pszXMLIn, &StructOut->sDateTime, &pChildNode, xmlInitDoc);
	bIsExists = xmlInitDoc->SelectSingleElementNode("FlowStarted", &pChildNode, pXml);
	if (bIsExists)
		CreateStructTagSystemTimeFromXmlWithOutNodes(pszXMLIn, &StructOut->sStartDateTime, &pChildNode, xmlInitDoc);

	//4.0.22.511 72606
	bIsExists = xmlInitDoc->SelectSingleElementNode("DispensingStarted", &pChildNode, pXml);
	if (bIsExists)
		CreateStructTagSystemTimeFromXmlWithOutNodes(pszXMLIn, &StructOut->sStartFuelingDateTime, &pChildNode, xmlInitDoc);

}
void CXmlConvertor::CreateStructTagSystemTimeFromXmlWithOutNodes(char * pszXMLIn, TAG_SYSTEMTIME * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("Year", StructOut->sYear, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Month", StructOut->sMonth, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Day", StructOut->sDay, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Hour", StructOut->sHour, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Minute", StructOut->sMinute, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Second", StructOut->sSecond, pXml);
}

//4.0.16.504
void CXmlConvertor::CreateStructDallasKeysInfoFromXmlWithOutNodes(char * pszXMLIn, DALLAS_KEYS_INFO * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{

	xmlInitDoc->GetAttributeToByteArrWithConvert("AVIBlockedListBuffer", StructOut->sBlockedListBuffer, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVIBlockedListBufferLength", StructOut->sBlockedListBufferLength, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVIBlockListPointer", StructOut->sBlockListPointer, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AVIBlockedKeysRequest", StructOut->sBlockedKeysRequest, pXml);
}
//4.0.16.504
void CXmlConvertor::CreateStructCardSaleExtraData4FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	char tempCardRangeNo [4];
	memset(&tempCardRangeNo, ' ', sizeof(tempCardRangeNo));

	CreateStructDallasKeysInfoFromXmlWithOutNodes(pszXMLIn, &StructOut->dallasKeysInfo, pXml, xmlInitDoc);
	CreateStructDiscountAttrsInfoFromXmlWithOutNodes(pszXMLIn, &StructOut->discountAttrsInfo, pXml, xmlInitDoc);  //4.0.17.501 - CR 38162
	CreateStructDiscountAttrsExtFromXmlWithOutNodes(pszXMLIn, &StructOut->sDiscountAttrExt, pXml, xmlInitDoc);	//4.0.22.509 38162
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyOnOff", StructOut->loyaltyOnOff, pXml);	//LPE
	xmlInitDoc->GetAttributeToByteArrWithConvert("ExtendedZipCode", StructOut->sExtendedZipCode, pXml); //46512
	xmlInitDoc->GetAttributeToByteArrWithConvert("FormattedClubCardNumber", StructOut->sFormattedClubCardNumber, pXml); //42728
	xmlInitDoc->GetAttributeToByteArrWithConvert("IFD", StructOut->sIFD, pXml); //42728
	xmlInitDoc->GetAttributeToByteArrWithConvert("MediaNumber", StructOut->sMediaNumber, pXml); //42728
	xmlInitDoc->GetAttributeToByteArrWithConvert("PumpTestPassword", StructOut->sPumpTestPassword, pXml); //54295
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyEntryMethod", StructOut->sLoyaltyEntryMethod, pXml);	//4.0.18.504
	xmlInitDoc->GetAttributeToByteArrWithConvert("ExtendedAVISpecialCodes", StructOut->sExtendedAVISpecialCodes, pXml); //25853 4.0.18.503
	xmlInitDoc->GetAttributeToByteArrWithConvert("FallBackTrs", StructOut->sIsOfflineTrs, pXml); //4.0.19.502
	xmlInitDoc->GetAttributeToByteArrWithConvert("PreAuthAmount", StructOut->sPreAuthAmount, pXml); 	//4.0.19.502
	xmlInitDoc->GetAttributeToByteArrWithConvert("TotalEftNetworkTime", StructOut->sTotalEftNetworkTime, pXml); //59424 	//4.0.19.502
	xmlInitDoc->GetAttributeToByteArrWithConvert("CustomerEftWaitTime", StructOut->sCustomerEftWaitTime, pXml); //59424 	//4.0.19.502
	xmlInitDoc->GetAttributeToByteArrWithConvert("CLDateTime", StructOut->sCLDateTime, pXml);	//58096 	//4.0.19.502
	xmlInitDoc->GetAttributeToByteArrWithConvert("StaffFlag", StructOut->sStaffFlag, pXml);		//58096 	//4.0.19.502
	xmlInitDoc->GetAttributeToByteArrWithConvert("ONMPProperties", StructOut->sONMPProperties, pXml);		//55286 Copient 	//4.0.19.502
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarWashOption", StructOut->sCarWashOption, pXml);			//55286 Copient 	//4.0.19.502
	xmlInitDoc->GetAttributeToByteArrWithConvert("HotCard", StructOut->sHotCard, pXml);						//4.0.18.502 //60638
	xmlInitDoc->GetAttributeToByteArrWithConvert("SponsorNum", StructOut->sSponsorNum, pXml); //TD 59898 4.0.19.503
	xmlInitDoc->GetAttributeToByteArrWithConvert("PanSeqNum", StructOut->sPanSeqNum, pXml); //TD 59898 4.0.19.503
	xmlInitDoc->GetAttributeToByteArrWithConvert("ExtraChipInfo", StructOut->sExtraChipInfo, pXml);	//4.0.120.20
	xmlInitDoc->GetAttributeToByteArrWithConvert("TransactionType", StructOut->sTransactionType, pXml);	//4.0.20.20
	xmlInitDoc->GetAttributeToByteArrWithConvert("ExistLoyaltyLimit", StructOut->sExistLoyaltyLimit, pXml);				//4.0.20.501 TD 60236
	xmlInitDoc->GetAttributeToByteArrWithConvert("EncryptedAccount", StructOut->sEncryptedAccount, pXml);				//4.0.20.501 TD 60236
	xmlInitDoc->GetAttributeToByteArrWithConvert("EncryptedAccount2", StructOut->sEncryptedAccount2, pXml);				//4.0.20.501 TD 60236
	xmlInitDoc->GetAttributeToByteArrWithConvert("DSSFailed", StructOut->sDSSFailed, pXml);								//4.0.20.501 TD 60236
	xmlInitDoc->GetAttributeToByteArrWithConvert("GetReceiptMethod", StructOut->byGetReceiptMethod, pXml);				//4.0.20.501 TD 60203
	xmlInitDoc->GetAttributeToByteArrWithConvert("RemoveProhibitedData", StructOut->byIsRemoveProhibitedData, pXml);	//4.0.20.501 TD 60236
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyVolumeLimit", StructOut->sLoyaltyVolumeLimit, pXml); //4.0.21.0 67854
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyOriginalUnitPrice", StructOut->sLoyaltyOriginalUnitPrice, pXml); //4.0.21.0 67854
	xmlInitDoc->GetAttributeToByteArrWithConvert("ForceZeroPAKCompletion", StructOut->sForceZeroPAKCompletion, pXml); //4.0.21.10
	xmlInitDoc->GetAttributeToByteArrWithConvert("InPAKLoyaltyWithComplete", StructOut->sInPAKLoyaltyWithComplete, pXml); //4.0.21.10
	xmlInitDoc->GetAttributeToByteArrWithConvert("CancelAborts", StructOut->sCancelAborts, pXml); //4.0.21.10 64119
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardRangeNo", tempCardRangeNo, pXml);							//4.0.22.504 TD 68871
	xmlInitDoc->GetAttributeToByteArrWithConvert("PanRequiredProtection", StructOut->sPanRequiredProtection, pXml);	//4.0.22.504 TD 72716
	xmlInitDoc->GetAttributeToByteArrWithConvert("StoreMode", StructOut->sStoreMode, pXml);							//4.0.22.504 TD 72716
	xmlInitDoc->GetAttributeToByteArrWithConvert("ActualMode", StructOut->sActualMode, pXml);							//4.0.22.504 TD 72716
	xmlInitDoc->GetAttributeToByteArrWithConvert("EncryptionKeyRef", StructOut->sEncryptionKeyRef, pXml);				//4.0.22.504 TD 72716
	xmlInitDoc->GetAttributeToByteArrWithConvert("Track2Exist", StructOut->sTrack2Exist, pXml);						//4.0.22.40 TD 72716
	xmlInitDoc->GetAttributeToByteArrWithConvert("DeferralAccepted", StructOut->sDeferralAccepted, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyName", StructOut->sLoyaltyName, pXml);						//4.0.22.509 74387
	xmlInitDoc->GetAttributeToByteArrWithConvert("Job", StructOut->sJob, pXml);										//4.0.22.511 74797
	xmlInitDoc->GetAttributeToByteArrWithConvert("Department", StructOut->sDepartment, pXml);						//4.0.22.511 74797
	xmlInitDoc->GetAttributeToByteArrWithConvert("OdometerExt2", StructOut->sOdometerExt2, pXml);					//4.0.22.511 74797
	xmlInitDoc->GetAttributeToByteArrWithConvert("License", StructOut->sLicense, pXml);								//4.0.22.511 74797
	xmlInitDoc->GetAttributeToByteArrWithConvert("CustomerReferenceExt", StructOut->sCustomerReferenceExt, pXml);	//4.0.22.511 74797
	xmlInitDoc->GetAttributeToByteArrWithConvert("CustomerIDExt", StructOut->sCustomerIDExt, pXml);					//4.0.22.511 74797
	xmlInitDoc->GetAttributeToByteArrWithConvert("DriverId_MSB2", StructOut->sDriverId_MSB2, pXml);					//4.0.23.370 117473 //4.0.24.90
	xmlInitDoc->GetAttributeToByteArrWithConvert("ZeroCompleteStatus", StructOut->sZeroCompleteStatus, pXml);					// 216043


	//split CardRangeNo to two fields:
	SplitValue(tempCardRangeNo, StructOut->sCardRangeNo, sizeof(StructOut->sCardRangeNo), &(StructOut->sCardRangeNoExt), sizeof(StructOut->sCardRangeNoExt)); //4.0.27.501    TD 228117
}

//4.0.18.501
void CXmlConvertor::CreateStructCardSaleExtraData5FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA5_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
// 4.0.23.509 97360 start
	char tempTotalsStart[20];
	char tempTotalsEnd[20];

	memset(&tempTotalsStart, ' ', sizeof(tempTotalsStart));
	memset(&tempTotalsEnd, ' ', sizeof(tempTotalsEnd));
// 4.0.23.509 97360 end

	xmlInitDoc->GetAttributeToByteArrWithConvert("EncryptData", StructOut->sEncryptData, pXml);				//4.0.22.511 71868
	xmlInitDoc->GetAttributeToByteArrWithConvert("HashedData", StructOut->sHashedData, pXml);				//4.0.22.511 71868
	xmlInitDoc->GetAttributeToByteArrWithConvert("EncryptedAccount3", StructOut->sEncryptedAccount3, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PercentOffDiscount", StructOut->sPercentOffDiscount, pXml);	//4.0.23.21 75234
	xmlInitDoc->GetAttributeToByteArrWithConvert("FuelTotalTicketDiscount", StructOut->sFuelTotalTicketDiscount, pXml);	 //4.0.23.21 75234

	// 4.0.23.509 97360 start
	xmlInitDoc->GetAttributeToByteArrWithConvert("PreDispensingVolume", tempTotalsStart, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PostDispensingVolume", tempTotalsEnd, pXml);

	SplitValue(tempTotalsStart, StructOut->sPreDispensingVolumeHigh, sizeof(StructOut->sPreDispensingVolumeHigh), StructOut->sPreDispensingVolumeLow, sizeof(StructOut->sPreDispensingVolumeLow));
	SplitValue(tempTotalsEnd, StructOut->sPostDispensingVolumeHigh, sizeof(StructOut->sPostDispensingVolumeHigh), StructOut->sPostDispensingVolumeLow, sizeof(StructOut->sPostDispensingVolumeLow));
	// 4.0.23.509 97360 end

	xmlInitDoc->GetAttributeToByteArrWithConvert("GRTotalDiscountCarWash", StructOut->sGRTotalDiscountCarWash, pXml);	 //4.0.24.60 105269
	xmlInitDoc->GetAttributeToByteArrWithConvert("GRTotalDiscountFuelItem", StructOut->sGRTotalDiscountFuelItem, pXml);	 //4.0.24.60 105269
	xmlInitDoc->GetAttributeToByteArrWithConvert("AttendantTag", StructOut->sAttendantTag, pXml);			//4.0.25.501 TD 137509
	xmlInitDoc->GetAttributeToByteArrWithConvert("OfflineForceAcc", StructOut->sOfflineForceAcc, pXml);			//4.0.25.504 TD 137509
	xmlInitDoc->GetAttributeToByteArrWithConvert("RestrictPoints", StructOut->sRestrictPoints, pXml);			//4.0.26.507
	xmlInitDoc->GetAttributeToByteArrWithConvert("ChipLocalPromptsMandated", StructOut->sChipLocalPromptsMandated, pXml);		//4.0.27.30 Arval start
	xmlInitDoc->GetAttributeToByteArrWithConvert("ChipLocalValidationMandated", StructOut->sChipLocalValidationMandated, pXml); //4.0.27.30
	xmlInitDoc->GetAttributeToByteArrWithConvert("ChipCarRegistration", StructOut->sChipCarRegistration, pXml);		          	//4.0.27.30
	xmlInitDoc->GetAttributeToByteArrWithConvert("ChipItemCode", StructOut->sChipItemCode, pXml);		          	            //4.0.27.30
	xmlInitDoc->GetAttributeToByteArrWithConvert("ChipFillVolume", StructOut->sChipFillVolume, pXml);		          	        //4.0.27.30  Arval end

	xmlInitDoc->GetAttributeToByteArrWithConvert("TrsFlowRate", StructOut->sTrsFlowRate, pXml);									//4.0.27.40 TD 165585
	xmlInitDoc->GetAttributeToByteArrWithConvert("AverageFlowRate", StructOut->sAverageFlowRate, pXml);							//4.0.27.40 TD 165585
	xmlInitDoc->GetAttributeToByteArrWithConvert("RatesFromDevice", StructOut->sRatesFromDevice, pXml);						//4.0.27.40 TD 165585

	xmlInitDoc->GetAttributeToByteArrWithConvert("CompleteRemainingBalance", StructOut->sCompleteRemainingBalance, pXml);	//4.0.27.43 Egift
	xmlInitDoc->GetAttributeToByteArrWithConvert("OverFillAmount", StructOut->sOverFillAmount, pXml);						//4.0.27.43	Egift
	xmlInitDoc->GetAttributeToByteArrWithConvert("CompleteResponseCode", StructOut->sCompleteResponseCode, pXml);			//4.0.27.43 Egift
	xmlInitDoc->GetAttributeToByteArrWithConvert("PreAuthResponseCode", StructOut->sPreAuthResponseCode, pXml);			//4.0.27.44 Egift

	xmlInitDoc->GetAttributeToByteArrWithConvert("ValidationResult", StructOut->sValidationResult, pXml);			//4.0.27.43	Arval
	xmlInitDoc->GetAttributeToByteArrWithConvert("OlaRequestAmount", StructOut->sOlaRequestAmount, pXml);			//4.0.27.43 Arval
	xmlInitDoc->GetAttributeToByteArrWithConvert("ParamResultExt", StructOut->sParamResultExt, pXml);					//4.0.28.502 255727
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyTenderAmount", StructOut->sLoyaltyTenderAmount, pXml);		//4.0.28.503 - TD 247268
	xmlInitDoc->GetAttributeToByteArrWithConvert("ScreenMsgNumber", StructOut->sScreenMsgNumber, pXml); //4.0.31.0
	xmlInitDoc->GetAttributeToByteArrWithConvert("PumpAutoStart", StructOut->sPumpAutoStart, pXml);	 //4.0.23.1461 323583
	xmlInitDoc->GetAttributeToByteArrWithConvert("ClubId", StructOut->byClubId, pXml); // 332329
	xmlInitDoc->GetAttributeToByteArrWithConvert("DedicatedFileName", StructOut->sDedicatedFileName, pXml); // 332329
	xmlInitDoc->GetAttributeToByteArrWithConvert("CarWashDiscountPerGallon", StructOut->sCarWashDiscountPerGallon, pXml);	//365865

	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthNumberEx", StructOut->sAuthNumberEx, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyAutoArm", StructOut->sLoyaltyAutoArm, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PriceProtectionUsed", StructOut->sPriceProtectionUsed, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LockingKey", StructOut->sLockingKey, pXml);								//CR 425758
	xmlInitDoc->GetAttributeToByteArrWithConvert("NoMatchingAID", StructOut->sNoMatchingAID, pXml);			// AID
	xmlInitDoc->GetAttributeToByteArrWithConvert("ChipMalfunc", StructOut->sChipMalfunc, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("UserCancel", StructOut->sUserCancel, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthorisationResponseCode", StructOut->sAuthorisationResponseCode, pXml); //  [7/7/2014 MaximB]
	xmlInitDoc->GetAttributeToByteArrWithConvert("Vat_RateMsb",	StructOut->sVAT_RateMSB, pXml);  //4.0.31.500  TD 404911
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmt1", StructOut->sTaxAmt1, pXml);  // TD 404911
	xmlInitDoc->GetAttributeToByteArrWithConvert("IssuerCodeTableIndex", StructOut->sIssuerCodeTableIndex, pXml); // QC 485279

	xmlInitDoc->GetAttributeToByteArrWithConvert("ExtraEMVDataReq", StructOut->ExtraEMVDataReq, pXml);				//  [18/5/2017 MaximB]
	xmlInitDoc->GetAttributeToByteArrWithConvert("ExtraEMVData", StructOut->sExtraEMVData, pXml);					//  [18/5/2017 MaximB]
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyPointsBalance", StructOut->sLoyaltySummaryPoints, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PinPadCapabilities", StructOut->sPinPadCapabilities, pXml);
}

void CXmlConvertor::CreateStructCardSaleExtraData6FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA6_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc )
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("TransactionId", StructOut->sTransactionId, pXml);  
	xmlInitDoc->GetAttributeToByteArrWithConvert("StoreID", StructOut->sStoreIDField, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PosID", StructOut->sPosId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TransID", StructOut->sTransId, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("StartDateTime", StructOut->sLoyaltyStartDateTime, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TicketIdentifier", StructOut->sTicketId, pXml);

	xmlInitDoc->GetAttributeToByteArrWithConvert("ClubType", StructOut->sClubType, pXml); //marik1969
	xmlInitDoc->GetAttributeToByteArrWithConvert("IdRetryCount", StructOut->sIdRetryCount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromotionID", StructOut->sPromotionID, pXml);

	xmlInitDoc->GetAttributeToByteArrWithConvert("CardTypeThirdParty", StructOut->sCardTypeThirdParty, pXml); // TD 453560  [6/4/2017 MaximB]
	xmlInitDoc->GetAttributeToByteArrWithConvert("AppVersionNumberICC", StructOut->sAppVersionNumberICC, pXml); // TD 453560  [6/4/2017 MaximB]
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardRiskManagementDataObjectList2", StructOut->sCardRiskManagementDataObjectList2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("IssuerScriptResult", StructOut->sIssuerScriptResult, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PinPadCapabilities", StructOut->sPinPadCapabilities, pXml);

	xmlInitDoc->GetAttributeToByteArrWithConvert("WashExpireTime", StructOut->sWashExpireTime, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ClubCardHashNumber", StructOut->sClubCardHashNumber, pXml);   // 4.0.32.??? TD 405435
	xmlInitDoc->GetAttributeToByteArrWithConvert("PartialApproval", StructOut->sPartialApproval, pXml);   // 4.0.32.??? TD 405435
	xmlInitDoc->GetAttributeToByteArrWithConvert("RemainingBalance", StructOut->sRemainingBalance, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TenderDiscount", StructOut->sTenderDiscount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TenderPromotionDescription", StructOut->sTenderPromotionDescription, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyPointsExt", StructOut->sLoyaltyPointsExt, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxName1", StructOut->sTaxName1, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxName2", StructOut->sTaxName2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxName3", StructOut->sTaxName3, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxName4", StructOut->sTaxName4, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmt4", StructOut->sTaxAmt4, pXml);

	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxType1", StructOut->sTaxType1, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxType2", StructOut->sTaxType2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxType3", StructOut->sTaxType3, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxType4", StructOut->sTaxType4, pXml);

	xmlInitDoc->GetAttributeToByteArrWithConvert("CCMTaxLink", StructOut->sCCMTaxLink, pXml);

	xmlInitDoc->GetAttributeToByteArrWithConvert("CommunicationReward", StructOut->sCommunicationReward, pXml);

	CXMLPointers pChildNode, pChildNode2;
	BOOL bIsExists = xmlInitDoc->SelectSingleElementNode("CarWashItem", &pChildNode, pXml);
	if (bIsExists)
	{
		CreateStructCarWashItemFromXmlWithOutNodes(pszXMLIn, &StructOut->CarWashItem, &pChildNode, xmlInitDoc);
	}

	bIsExists = xmlInitDoc->SelectSingleElementNode("PaymentCardDetails", &pChildNode, pXml);
	if (bIsExists)
	{
		xmlInitDoc->SelectSingleElementNode("PaymentCard1", &pChildNode2, &pChildNode);
		CreateStructPaymentCardFromXmlWithOutNodes(pszXMLIn, &StructOut->PaymentCard1, &pChildNode2, xmlInitDoc);

		xmlInitDoc->SelectSingleElementNode("PaymentCard2", &pChildNode2, &pChildNode);
		CreateStructPaymentCardFromXmlWithOutNodes(pszXMLIn, &StructOut->PaymentCard2, &pChildNode2, xmlInitDoc);
	}


	bIsExists = xmlInitDoc->SelectSingleElementNode("LoyaltyStartTime", &pChildNode, pXml);
	if (bIsExists)
		CreateStructTagSystemTimeFromXmlWithOutNodes(pszXMLIn, (TAG_SYSTEMTIME*)&StructOut->sLoyaltyStartTime, &pChildNode, xmlInitDoc);

	bIsExists = xmlInitDoc->SelectSingleElementNode("CustomerInformation", &pChildNode, pXml); //MichaelPo -	US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
	if (bIsExists)
	{
		CreateStructLoyaltyCardDetailsFromXmlWithOutNodes(pszXMLIn, &StructOut->LoyaltyCard, &pChildNode, xmlInitDoc);
	}

	xmlInitDoc->GetAttributeToByteArrWithConvert("QuickChipTrs", StructOut->sQuickChipTrs, pXml);  //4.0.33.20

// RFUEL-2817 - XML was missing TenderDiscount
	xmlInitDoc->GetAttributeToByteArrWithConvert("TenderDiscount", StructOut->sTenderDiscount, pXml);  //4.0.33.20
	
	// RFUEL-3049
	xmlInitDoc->GetAttributeToByteArrWithConvert("HostTranId", StructOut->sLoylatyRefHostTranId, pXml);

	char sTmp[15];
	memset(sTmp, 0, sizeof(sTmp));

	bIsExists = xmlInitDoc->SelectSingleElementNode("GradeDiscountID", &pChildNode, pXml);//RFUEL-2093
	if (bIsExists) 
	{
		for (int i = 0; i < MAX_POSITIONS_PER_PUMP; i++)
		{
			sprintf(sTmp, "ID%ld", i);
			xmlInitDoc->GetAttributeToByteArrWithConvert(sTmp, StructOut->sGradeDiscountID[i], &pChildNode);
		}
	}
	//RFUEL-3309 Get the maximal discount allowed for the grade 
		xmlInitDoc->GetAttributeToByteArrWithConvert("MaxAllowedDiscount", StructOut->sMaxDiscountAllowed, pXml);

}

//4.0.17.501 - CR 38162
void CXmlConvertor::CreateStructDiscountIDsFromXmlWithOutNodes(char * pszXMLIn, ITEM_DISCOUNT_ID * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("Grade", StructOut->sDiscountGrade, pXml); //4.0.22.66 38162 //4.0.22.509
	xmlInitDoc->GetAttributeToByteArrWithConvert("ID", StructOut->sDiscountID, pXml);
}

//4.0.17.501 - CR 38162
void CXmlConvertor::CreateStructDiscountAttrsInfoFromXmlWithOutNodes(char * pszXMLIn, DISCOUNT_ATTRIBUTES * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers pItemElement, pChildNode;
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyType", StructOut->sLoyaltyType, pXml);

	long lDiscountID = 0;
	int iIndex = 0;
	BOOL bIsExists;

	bIsExists = xmlInitDoc->SelectSingleElementNode("ItemsDiscountIDs", &pChildNode, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetChildNodesList(&pChildNode, &pItemElement);

		while(xmlInitDoc->GetNextItem(pItemElement, pChildNode))
		{
			if ((iIndex >= 0) && (iIndex < MAX_DISCOUNTS_PER_ITEM))
			{
				CreateStructDiscountIDsFromXmlWithOutNodes(pszXMLIn, &StructOut->DiscountIDs[iIndex], &pChildNode, xmlInitDoc);
				iIndex++;
			}
		}
	}
}

//4.0.22.509 - CR 38162
void CXmlConvertor::CreateStructDiscountAttrsExtFromXmlWithOutNodes(char * pszXMLIn, DISCOUNT_ATTRIBUTES_EXT * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers pItemElement, pChildNode;

	int iIndex = 0;
	BOOL bIsExists;

	bIsExists = xmlInitDoc->SelectSingleElementNode("ItemsDiscountIDs", &pChildNode, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetChildNodesList(&pChildNode, &pItemElement);

		while(xmlInitDoc->GetNextItem(pItemElement, pChildNode))
		{
			if ((iIndex >= 0) && (iIndex < MAX_DISCOUNTS_PER_ITEM))
			{
				xmlInitDoc->GetAttributeToByteArrWithConvert("ItemDiscountType", StructOut->sDiscountTypes[iIndex].sItemDiscountType, &pChildNode);
				iIndex++;
			}
		}
	}
}

//4.0.20.503 66960
void CXmlConvertor::CreateStructExtraChipFromXmlWithOutNodes(char * pszXMLIn, EXTRA_CHIP_INFO * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("CvmList", StructOut->byData, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CvmListSize", StructOut->byLen, pXml);
}

//4.0.24.60 109103
void CXmlConvertor::CreateStructOlaExtraDataFromXmlWithOutNodes(char * pszXMLIn, OLA_EXTRA_DATA * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc) //4.0.23.60 109103
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("OlaExtraData", StructOut->byData, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("OlaExtraDataLen", StructOut->byLen, pXml);
}



/******************************* Create xml from struct with out nodes functions ********************************/

void CXmlConvertor::CreateXMLFromStructCardSaleAll3(char * pszXMLOut, CARD_SALE_ALL3 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	CreateXMLFromStructCardSaleAll(pszXMLOut, &StructIn->CardSaleAll, xmlInitDoc, pCurrElement, &StructIn->extData2, &StructIn->extData3); //4.0.15.502 //4.0.23.509 100885
	CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn->extData2, xmlInitDoc, pCurrElement);
	CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn->extData3, xmlInitDoc, pCurrElement);
	CreateXMLFromStructCardSaleExtraData4(pszXMLOut, &StructIn->extData4, xmlInitDoc, pCurrElement); //4.0.17.501
	CreateXMLFromStructCardSaleExtraData5(pszXMLOut, &StructIn->extData5, xmlInitDoc, pCurrElement); //4.0.23.21
	CreateXMLFromStructCardSaleExtraData6(pszXMLOut, &StructIn->extData6, xmlInitDoc, pCurrElement); //4.0.23.21
}

void CXmlConvertor::CreateXMLFromStructCardSaleAll2(char * pszXMLOut, CARD_SALE_ALL2 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	CreateXMLFromStructCardSaleAll(pszXMLOut, &StructIn->CardSaleAll, xmlInitDoc, pCurrElement, &StructIn->extData2); //4.0.15.502
	CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn->extData2, xmlInitDoc, pCurrElement);
}

void CXmlConvertor::CreateXMLFromStructCardSaleAll(char * pszXMLOut, CARD_SALE_ALL * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement, CARD_SALE_EXTRA_DATA2_ * StructIn2, CARD_SALE_EXTRA_DATA3_ * StructIn3) //4.0.15.502 //4.0.18.670 73612	//4.0.22.502
{
	CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn->data, xmlInitDoc, pCurrElement, &StructIn->extData, StructIn2); //4.0.15.502
	CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn->extData, xmlInitDoc, pCurrElement, StructIn3);	//4.0.18.670 73612
}


void CXmlConvertor::CreateXMLFromStructCardSaleData(char * pszXMLOut, CARD_SALE_DATA * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement, CARD_SALE_EXTRA_DATA * StructIn2, CARD_SALE_EXTRA_DATA2_ * StructIn3) //4.0.15.502
{
	char tempAutoAmt[10] = {0};
	char tmpAuthorizeSessionID[6] = {0}; //4.0.15.502

	if (StructIn2 == NULL)
		EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthAmt", StructIn->sAuthAmt, sizeof(StructIn->sAuthAmt) );
	else
	{
		//connecting the values of AuthAmt_Msb and sAuthAmt to one.
		memcpy(tempAutoAmt, StructIn2->sAuthAmt_Msb, sizeof(StructIn2->sAuthAmt_Msb));
		memcpy(tempAutoAmt + 5, StructIn->sAuthAmt, sizeof(StructIn->sAuthAmt));
		EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthAmt", (unsigned char*) tempAutoAmt, sizeof(tempAutoAmt));
	}

	//4.0.15.502
	if (StructIn3 == NULL)
		EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthorizeSessionID", StructIn->sAuthorizeSessionID, sizeof(StructIn->sAuthorizeSessionID) );
	else
	{
		//connecting the values of sAuthorizeSessionID_Msb and sAuthorizeSessionID to one.
		memcpy(tmpAuthorizeSessionID, StructIn3->sAuthorizeSessionID_MSB, sizeof(StructIn3->sAuthorizeSessionID_MSB));
		memcpy(tmpAuthorizeSessionID + 2, StructIn->sAuthorizeSessionID, sizeof(StructIn->sAuthorizeSessionID));
		EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthorizeSessionID", (unsigned char*) tmpAuthorizeSessionID, sizeof(tmpAuthorizeSessionID));
	}

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDTranStatus", &StructIn->sTranStatus, sizeof(StructIn->sTranStatus));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TranType", &StructIn->sTranType, sizeof(StructIn->sTranType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MID", StructIn->sMID, sizeof(StructIn->sMID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PumpNumber",  StructIn->sPumpNumber, sizeof(StructIn->sPumpNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DeviceNumber", StructIn->sDeviceNumber, sizeof(StructIn->sDeviceNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TranNbr", StructIn->sTranNbr, sizeof(StructIn->sTranNbr) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RequestAmt", StructIn->sRequestAmt, sizeof(StructIn->sRequestAmt) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "SurchargeAmt", StructIn->sSurchargeAmt, sizeof(StructIn->sSurchargeAmt) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Track1Data", StructIn->sTrack1Data, sizeof(StructIn->sTrack1Data) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Track2Data", StructIn->sTrack2Data, sizeof(StructIn->sTrack2Data) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "StartDate", StructIn->sStartDate, sizeof(StructIn->sStartDate) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CardIssue", StructIn->sCardIssue, sizeof(StructIn->sCardIssue) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DriverId", StructIn->sDriverId, sizeof(StructIn->sDriverId) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDOdometer", StructIn->sOdometer, sizeof(StructIn->sOdometer) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PINBlock", StructIn->sPINBlock, sizeof(StructIn->sPINBlock) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ReturnTank", &StructIn->sReturnTank, sizeof(StructIn->sReturnTank) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDReserved", StructIn->sReserved, sizeof(StructIn->sReserved) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "EntryMethod", &StructIn->sEntryMethod, sizeof(StructIn->sEntryMethod) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IsStoreAndForward", &StructIn->sIsStoreAndForward, sizeof(StructIn->sIsStoreAndForward) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDCardType", StructIn->sCardType, sizeof(StructIn->sCardType) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDFullVehicleId", StructIn->sFullVehicleId, sizeof(StructIn->sFullVehicleId) );
	//EndXmlLineWithNull(xmlInitDoc, pCurrElement,"AuthorizeSessionID", StructIn->sAuthorizeSessionID, sizeof(StructIn->sAuthorizeSessionID) );
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDAuthNumber", StructIn->sAuthNumber, sizeof(StructIn->sAuthNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDIsOnLine", &StructIn->sIsOnLine, sizeof(StructIn->sIsOnLine));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ClearanceID", &StructIn->sClearanceID, sizeof(StructIn->sClearanceID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDAccountNumber", StructIn->sAccountNumber, sizeof(StructIn->sAccountNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDFullCardName", StructIn->sFullCardName, sizeof(StructIn->sFullCardName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDDateTime", StructIn->sDateTime, sizeof(StructIn->sDateTime));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDNozzle", &StructIn->sNozzle, sizeof(StructIn->sNozzle));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ResultMsg", StructIn->sResultMsg, sizeof(StructIn->sResultMsg));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MerchantID", StructIn->sMerchantID, sizeof(StructIn->sMerchantID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "NozzleRestriction", StructIn->sNozzleRestriction, sizeof(StructIn->sNozzleRestriction));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ReceiptRestriction", StructIn->sReceiptRestriction, sizeof(StructIn->sReceiptRestriction));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Approval", StructIn->sApproval, sizeof(StructIn->sApproval));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ResInfoAMax8", StructIn->sResInfoAMax8, sizeof(StructIn->sResInfoAMax8));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDReferenceNumber", StructIn->sReferenceNumber, sizeof(StructIn->sReferenceNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CardName", StructIn->sCardName, sizeof(StructIn->sCardName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDExpire", StructIn->sExpire, sizeof(StructIn->sExpire));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GradeRestriction", StructIn->sGradeRestriction, sizeof(StructIn->sGradeRestriction));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "HostTerminalID", StructIn->sHostTerminalID, sizeof(StructIn->sHostTerminalID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GRPrompt", &StructIn->sGRPrompt, sizeof(StructIn->sGRPrompt));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVIEngineHours", StructIn->sAVIEngineHours, sizeof(StructIn->sAVIEngineHours));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVISystemCustomer", StructIn->sAVISystemCustomer, sizeof(StructIn->sAVISystemCustomer));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVICompanyId", StructIn->sAVICompanyId, sizeof(StructIn->sAVICompanyId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVIUserId", StructIn->sAVIUserId, sizeof(StructIn->sAVIUserId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVILimit", StructIn->sAVILimit, sizeof(StructIn->sAVILimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVILimitType", &StructIn->sAVILimitType, sizeof(StructIn->sAVILimitType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVISpecialCodes", StructIn->sAVISpecialCodes, sizeof(StructIn->sAVISpecialCodes));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ExtendPIN", StructIn->sExtendPIN, sizeof(StructIn->sExtendPIN));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FinalRefNumber", StructIn->sFinalRefNumber, sizeof(StructIn->sFinalRefNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FinalAuthNumber", StructIn->sFinalAuthNumber, sizeof(StructIn->sFinalAuthNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GRVoucherNumber", StructIn->sGRVoucherNumber, sizeof(StructIn->sGRVoucherNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GRKeyCode", StructIn->sGRKeyCode, sizeof(StructIn->sGRKeyCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GRDiscountPerGallon", StructIn->sGRDiscountPerGallon, sizeof(StructIn->sGRDiscountPerGallon));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GRMaxVolume", StructIn->sGRMaxVolume, sizeof(StructIn->sGRMaxVolume));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthTerminal", StructIn->sAuthTerminal, sizeof(StructIn->sAuthTerminal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthVersion", StructIn->sAuthVersion, sizeof(StructIn->sAuthVersion));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GeneralID", StructIn->sGeneralID, sizeof(StructIn->sGeneralID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSDTransBatch", StructIn->sTransBatch, sizeof(StructIn->sTransBatch));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TransBatchSequence", StructIn->sTransBatchSequence, sizeof(StructIn->sTransBatchSequence));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ClubCardTrack2Data", StructIn->sClubCardTrack2Data, sizeof(StructIn->sClubCardTrack2Data));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthCode", StructIn->sAuthCode, sizeof(StructIn->sAuthCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AppCodeElectr", StructIn->sAppCodeElectr, sizeof(StructIn->sAppCodeElectr));
}

void CXmlConvertor::CreateXMLFromStructCardSaleExtraData(char * pszXMLOut, CARD_SALE_EXTRA_DATA * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement, CARD_SALE_EXTRA_DATA3_ * StructIn3)	//4.0.18.670 73612 //4.0.22.502
{
	CXMLPointers	pChildNode, pChildNode2;
	char			tempTotal[10] = {0};
	char			tempFuel [10] = {0};
	char			tempTaxAmt [8] = {0};

	if (StructIn3 == NULL)	//4.0.18.670 73612	//4.0.22.502
	{
		EndXmlLineWithNull(xmlInitDoc, pCurrElement,"TaxAmt", StructIn->sTaxAmt, sizeof(StructIn->sTaxAmt));
//		_LOGMSG.LogMsg("CXmlConvertor::CreateXMLFromStructCardSaleExtraData, No StructExt3");

	}
	else
	{
		//connecting the values of sAuthorizeSessionID_Msb and sAuthorizeSessionID to one.
		//4.0.23.504 87373
		long lTaxAmtMsbSize = sizeof(StructIn3->sTaxAmt_Msb);
		long lTaxAmtMsb2Size = sizeof(StructIn3->sTaxAmt_Msb2);
		//connecting the values of sAuthorizeSessionID_Msb and sAuthorizeSessionID to one.
		memcpy(tempTaxAmt, &StructIn3->sTaxAmt_Msb2, lTaxAmtMsb2Size);
		memcpy(tempTaxAmt + lTaxAmtMsb2Size, StructIn3->sTaxAmt_Msb, lTaxAmtMsbSize);
		memcpy(tempTaxAmt + lTaxAmtMsbSize + lTaxAmtMsb2Size, StructIn->sTaxAmt, sizeof(StructIn->sTaxAmt));
		EndXmlLineWithNull(xmlInitDoc, pCurrElement,"TaxAmt",(unsigned char*)tempTaxAmt, sizeof(tempTaxAmt));
//		_LOGMSG.LogMsg("CXmlConvertor::CreateXMLFromStructCardSaleExtraData, Exists StructExt3");
	}
	//connecting the values of sTotalAmt_Msb and sTotalAmt to one.
	memcpy(tempTotal, StructIn->sTotalAmt_Msb, sizeof(StructIn->sTotalAmt_Msb));
	memcpy(tempTotal + 5, StructIn->sTotalAmt, sizeof(StructIn->sTotalAmt));

	//connecting the values of sFuelAmt_Msb and sFuelAmt to one.
	memcpy(tempFuel, StructIn->sFuelAmt_Msb, sizeof(StructIn->sFuelAmt_Msb));
	memcpy(tempFuel+5, StructIn->sFuelAmt, sizeof(StructIn->sFuelAmt));

	BYTE CWProductCode[MAX_PRODUCT_CODE_LEN];  
	int CWIndex = a2i((BYTE *)StructIn->SALES[0].sDPTProdCode, sizeof(StructIn->SALES[0].sDPTProdCode));
	CString sProdcutCode(_Module.m_server.m_cCarWashProgramsDetails[CWIndex].sProductCode);
	memcpy(CWProductCode, _Module.m_server.m_cCarWashProgramsDetails[CWIndex].sProductCode, sizeof(CWProductCode));
	
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "WashProductCode", CWProductCode, sizeof(CWProductCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "WashCode", StructIn->sWashCode, sizeof(StructIn->sWashCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "WashExpire", StructIn->sWashExpire, sizeof(StructIn->sWashExpire));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PrnDateTime", StructIn->sPrnDateTime, sizeof(StructIn->sPrnDateTime));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TotalAmt", (unsigned char*) tempTotal, sizeof(tempTotal));
//4.0.22.502 73612	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmt", StructIn->sTaxAmt, sizeof(StructIn->sTaxAmt));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyalId", StructIn->sLoyalId, sizeof(StructIn->sLoyalId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyalScheme", StructIn->sLoyalScheme, sizeof(StructIn->sLoyalScheme));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyalPoints", StructIn->sLoyalPoints, sizeof(StructIn->sLoyalPoints));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "VAT_Rate", StructIn->sVAT_Rate, sizeof(StructIn->sVAT_Rate));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ReciptStatus", &StructIn->sReciptStatus, sizeof(StructIn->sReciptStatus));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDReserved", StructIn->sReserved, sizeof(StructIn->sReserved));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GRTotalDiscount", StructIn->sGRTotalDiscount, sizeof(StructIn->sGRTotalDiscount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GRActualDiscountPerGallon", StructIn->sGRActualDiscountPerGallon, sizeof(StructIn->sGRActualDiscountPerGallon));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "NumberOfExtraSale", &StructIn->sNumberOfExtraSale, sizeof(StructIn->sNumberOfExtraSale));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FuelAmt", (unsigned char*)tempFuel, sizeof(tempFuel));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashProgram", &StructIn->sCarWashProgram, sizeof(StructIn->sCarWashProgram));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDBankTermId", StructIn->sBankTermId, sizeof(StructIn->sBankTermId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDCustomerName", StructIn->sCustomerName, sizeof(StructIn->sCustomerName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDISOResponse", StructIn->sISOResponse, sizeof(StructIn->sISOResponse));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDSPDHCode", StructIn->sSPDHCode, sizeof(StructIn->sSPDHCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyActive", &StructIn->sLoyaltyActive, sizeof(StructIn->sLoyaltyActive));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ForceCompleteZero", &StructIn->sForceCompleteZero, sizeof(StructIn->sForceCompleteZero));
	//EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthAmt_Msb", StructIn->sAuthAmt_Msb, sizeof(StructIn->sAuthAmt_Msb)); ariel check
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DateTimeExt", StructIn->sDateTimeExt, sizeof(StructIn->sDateTimeExt));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OdometerExt", StructIn->sOdometerExt, sizeof(StructIn->sOdometerExt));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CompleteDate", StructIn->sCompleteDate, sizeof(StructIn->sCompleteDate));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CompleteTime", StructIn->sCompleteTime, sizeof(StructIn->sCompleteTime));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AttandantSession", &StructIn->sAttandantSession, sizeof(StructIn->sAttandantSession));

	StructIn->sSlipFlags += 48; //4.0.14.505
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDSlipFlags", &StructIn->sSlipFlags, sizeof(StructIn->sSlipFlags));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RedeemPoints", &StructIn->sRedeemPoints, sizeof(StructIn->sRedeemPoints));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ConfirmPrompt", &StructIn->sConfirmPrompt, sizeof(StructIn->sConfirmPrompt));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CouponReward", StructIn->sCouponReward, sizeof(StructIn->sCouponReward));

	xmlInitDoc->CreateElement("Sales", &pChildNode);
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);

	for (int i = 0 ; i < 4 ; i++)
	{
		xmlInitDoc->CreateElement("Sale", &pChildNode2);
		xmlInitDoc->AppendChild(&pChildNode2, &pChildNode);
		CreateXMLFromStructSaleWithOutNodes(pszXMLOut, &StructIn->SALES[i], long(i), xmlInitDoc, &pChildNode2);
	}
}

void CXmlConvertor::CreateXMLFromStructCardSaleExtraData2(char * pszXMLOut, CARD_SALE_EXTRA_DATA2_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	CXMLPointers	pChildNode, pChildNode2;
	char			tempCompletedSessionID [6] = {0};
	char			tempPriceRecalcDiscount[7] = {0};

	//connecting the values of sCompletedSessionID_MSB and sCompletedSessionID to one.
	memcpy(tempCompletedSessionID, StructIn->sCompletedSessionID_MSB, sizeof(StructIn->sCompletedSessionID_MSB));
	memcpy(tempCompletedSessionID + 2, StructIn->sCompletedSessionID, sizeof(StructIn->sCompletedSessionID));

	//connecting the values of sPriceRecalcDiscount_Msb and sPriceRecalcDiscount to one.
	memcpy(tempPriceRecalcDiscount, StructIn->sPriceRecalcDiscount_Msb, sizeof(StructIn->sPriceRecalcDiscount_Msb));
	memcpy(tempPriceRecalcDiscount+2, StructIn->sPriceRecalcDiscount, sizeof(StructIn->sPriceRecalcDiscount));

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDSiteId", StructIn->sSiteId, sizeof(StructIn->sSiteId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Unit", StructIn->sUnit, sizeof(StructIn->sUnit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TripNumber", StructIn->sTripNumber, sizeof(StructIn->sTripNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CustomerReference", StructIn->sCustomerReference, sizeof(StructIn->sCustomerReference));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TractorHub", StructIn->sTractorHub, sizeof(StructIn->sTractorHub));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TrailerNumber", StructIn->sTrailerNumber, sizeof(StructIn->sTrailerNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TrailerHub", StructIn->sTrailerHub, sizeof(StructIn->sTrailerHub));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OriginTrans", &StructIn->sOriginTrans, sizeof(StructIn->sOriginTrans));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ScripUnitPrice", StructIn->sScripUnitPrice, sizeof(StructIn->sScripUnitPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MarginUnitPrice", StructIn->sMarginUnitPrice, sizeof(StructIn->sMarginUnitPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TotalScripAmount", StructIn->sTotalScripAmount, sizeof(StructIn->sTotalScripAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DeclinedLoyaltyPrompt", &StructIn->sDeclinedLoyaltyPrompt, sizeof(StructIn->sDeclinedLoyaltyPrompt));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DeclinedLoyaltyResponse", &StructIn->sDeclinedLoyaltyResponse, sizeof(StructIn->sDeclinedLoyaltyResponse));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDAccountType", &StructIn->sAccountType, sizeof(StructIn->sAccountType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CustomerMessage", StructIn->sCustomerMessage, sizeof(StructIn->sCustomerMessage));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDCouponMessage", StructIn->sCouponMessage, sizeof(StructIn->sCouponMessage));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDReserveForCoupon", StructIn->sReserveForCoupon, sizeof(StructIn->sReserveForCoupon));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CouponId", StructIn->sCouponId, sizeof(StructIn->sCouponId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyProgName", StructIn->sLoyaltyProgName, sizeof(StructIn->sLoyaltyProgName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PassCode", StructIn->sPassCode, sizeof(StructIn->sPassCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GRDiscountType", &StructIn->sGRDiscountType, sizeof(StructIn->sGRDiscountType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDReceiptType", StructIn->sReceiptType, sizeof(StructIn->sReceiptType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ServiceFee", StructIn->sServiceFee, sizeof(StructIn->sServiceFee));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PrinterWidth", StructIn->sPrinterWidth, sizeof(StructIn->sPrinterWidth));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ParamNumber", StructIn->sParamNumber, sizeof(StructIn->sParamNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Param1", StructIn->sParam1, sizeof(StructIn->sParam1));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Param2", StructIn->sParam2, sizeof(StructIn->sParam2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PaymentStage", &StructIn->sPaymentStage, sizeof(StructIn->sPaymentStage));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TextLength", StructIn->sTextLength, sizeof(StructIn->sTextLength));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FormattedAccount", StructIn->sFormattedAccount, sizeof(StructIn->sFormattedAccount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AutoAuthorize", &StructIn->sAutoAuthorize, sizeof(StructIn->sAutoAuthorize));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "WaitForReportFlag", &StructIn->sWaitForReportFlag, sizeof(StructIn->sWaitForReportFlag));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDInvoiceNumber", StructIn->sInvoiceNumber, sizeof(StructIn->sInvoiceNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDAuthorizationSerialNumber", StructIn->sAuthorizationSerialNumber	, sizeof(StructIn->sAuthorizationSerialNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDAuthorizerFileNumber", StructIn->sAuthorizerFileNumber, sizeof(StructIn->sAuthorizerFileNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PinPadAction", &StructIn->sPinPadAction, sizeof(StructIn->sPinPadAction));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PinPadResult", &StructIn->sPinPadResult, sizeof(StructIn->sPinPadResult));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CalculatedMAC", StructIn->sCalculatedMAC, sizeof(StructIn->sCalculatedMAC));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PinSessionKey", StructIn->sPinSessionKey, sizeof(StructIn->sPinSessionKey));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MACSessionKey", StructIn->sMACSessionKey, sizeof(StructIn->sMACSessionKey));
//4.0.24.70 115660
	if (StructIn->sLanguage != ' ')   //4.0.23.360 //4.0.24.71
		StructIn->sLanguage += '0';


	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Language", &StructIn->sLanguage, sizeof(StructIn->sLanguage)); //4.0.24.71

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromotionNumber", StructIn->sPromotionNumber, sizeof(StructIn->sPromotionNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PriceLevel", &StructIn->sPriceLevel, sizeof(StructIn->sPriceLevel));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDSignatureSlip", &StructIn->sSignatureSlip, sizeof(StructIn->sSignatureSlip));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ReceiptPrinted", &StructIn->sReceiptPrinted, sizeof(StructIn->sReceiptPrinted));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ResInfoAMax37", StructIn->sResInfoAMax37, sizeof(StructIn->sResInfoAMax37));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDGradePrice", StructIn->sGradePrice, sizeof(StructIn->sGradePrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TotalDiscount", StructIn->sTotalDiscount, sizeof(StructIn->sTotalDiscount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "VolumeLimit", StructIn->sVolumeLimit, sizeof(StructIn->sVolumeLimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PriceRecalcDiscountSign", &StructIn->sPriceRecalcDiscountSign, sizeof(StructIn->sPriceRecalcDiscountSign));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TotalBalance", StructIn->sTotalBalance, sizeof(StructIn->sTotalBalance));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AttendantID", StructIn->sAttendantID, sizeof(StructIn->sAttendantID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AttendantName", StructIn->sAttendantName, sizeof(StructIn->sAttendantName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IgnoreServiceFee", &StructIn->sIgnoreServiceFee, sizeof(StructIn->sIgnoreServiceFee));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "SessionOffLine", &StructIn->sSessionOffLine, sizeof(StructIn->sSessionOffLine));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ClockDateTime", StructIn->sClockDateTime, sizeof(StructIn->sClockDateTime));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ClockFlags", StructIn->sClockFlags, sizeof(StructIn->sClockFlags));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemsLinkIndex", StructIn->sItemsLinkIndex, sizeof(StructIn->sItemsLinkIndex));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MessageTimeOut", StructIn->sMessageTimeOut, sizeof(StructIn->sMessageTimeOut));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Token", StructIn->sToken, sizeof(StructIn->sToken));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BarCode", StructIn->sBarCode, sizeof(StructIn->sBarCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Flags", StructIn->sFlags, sizeof(StructIn->sFlags));
	//EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthorizeSessionID_MSB", StructIn->sAuthorizeSessionID_MSB, sizeof(StructIn->sAuthorizeSessionID_MSB));ariel check
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ZipCode", StructIn->sZipCode, sizeof(StructIn->sZipCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmt2", StructIn->sTaxAmt2, sizeof(StructIn->sTaxAmt2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmt3", StructIn->sTaxAmt3, sizeof(StructIn->sTaxAmt3));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CashDeposit1", StructIn->sCashDeposit1, sizeof(StructIn->sCashDeposit1));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CashDeposit2", StructIn->sCashDeposit2, sizeof(StructIn->sCashDeposit2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CashDeposit5", StructIn->sCashDeposit5, sizeof(StructIn->sCashDeposit5));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CashDeposit10", StructIn->sCashDeposit10, sizeof(StructIn->sCashDeposit10));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CashDeposit20", StructIn->sCashDeposit20, sizeof(StructIn->sCashDeposit20));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CashDeposit50", StructIn->sCashDeposit50, sizeof(StructIn->sCashDeposit50));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CashDeposit100", StructIn->sCashDeposit100, sizeof(StructIn->sCashDeposit100));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CashDepositTotal", StructIn->sCashDepositTotal, sizeof(StructIn->sCashDepositTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CompletedSessionID", (unsigned char *)tempCompletedSessionID, sizeof(tempCompletedSessionID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthorizerID", StructIn->sAuthorizerID, sizeof(StructIn->sAuthorizerID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyTotalAmount", StructIn->sLoyaltyTotalAmount, sizeof(StructIn->sLoyaltyTotalAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MediaMasterType", StructIn->sMediaMasterType, sizeof(StructIn->sMediaMasterType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DisplayOLAResultMessage", &StructIn->sDisplayOLAResultMessage, sizeof(StructIn->sDisplayOLAResultMessage));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ChargeTo", StructIn->sChargeTo, sizeof(StructIn->sChargeTo));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVIFuelCode", &StructIn->sAVIFuelCode, sizeof(StructIn->sAVIFuelCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PriceRecalcDiscount", (unsigned char *)tempPriceRecalcDiscount, sizeof(tempPriceRecalcDiscount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RefundAmount", StructIn->sRefundAmount, sizeof(StructIn->sRefundAmount));


	xmlInitDoc->CreateElement("Discounts", &pChildNode);
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);

	for (int i = 0 ; i < MAX_GRADES ; i++)
	{
		xmlInitDoc->CreateElement("Discount", &pChildNode2);
		xmlInitDoc->AppendChild(&pChildNode2, &pChildNode);
		CreateXMLFromStructDiscntWithOutNodes (pszXMLOut, &StructIn->sDiscountPerGrade[i], long(i), xmlInitDoc, &pChildNode2);
	}
}

void CXmlConvertor::CreateXMLFromStructCardSaleExtraData3(char * pszXMLOut, CARD_SALE_EXTRA_DATA3_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	CXMLPointers	pChildNode, pChildNode2;

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDTotalRedeemedPoints", StructIn->sTotalRedeemedPoints, sizeof(StructIn->sTotalRedeemedPoints));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDAmountByPoints", StructIn->sAmountByPoints, sizeof(StructIn->sAmountByPoints));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDCreditAmount", StructIn->sCreditAmount, sizeof(StructIn->sCreditAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ControllerSeqNum", StructIn->sControllerSeqNum, sizeof(StructIn->sControllerSeqNum));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ReSend", &StructIn->sReSend, sizeof(StructIn->sReSend));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RemoteID", StructIn->sRemoteID, sizeof(StructIn->sRemoteID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashDiscount", StructIn->sCarWashDiscount, sizeof(StructIn->sCarWashDiscount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashLimit", StructIn->sCarWashLimit, sizeof(StructIn->sCarWashLimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ParamResult", StructIn->sParamResult, sizeof(StructIn->sParamResult));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromptType", StructIn->sPromptType, sizeof(StructIn->sPromptType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromptMinLen", StructIn->sPromptMinLen, sizeof(StructIn->sPromptMinLen));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromptMaxLen", StructIn->sPromptMaxLen, sizeof(StructIn->sPromptMaxLen));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ParamIndex", StructIn->sParamIndex, sizeof(StructIn->sParamIndex));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarwashRestriction", &StructIn->sCarwashRestriction, sizeof(StructIn->sCarwashRestriction));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ReservedForfutureCarWash", StructIn->sReservedForfutureCarWash, sizeof(StructIn->sReservedForfutureCarWash));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DriverId_MSB", StructIn->sDriverId_MSB, sizeof(StructIn->sDriverId_MSB)); //4.0.23.370 117473 //4.0.24.90
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDLogicShiftNumber", StructIn->sLogicShiftNumber, sizeof(StructIn->sLogicShiftNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Flags1", StructIn->sFlags1, sizeof(StructIn->sFlags1));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OriginalUnitPrice", StructIn->sOriginalUnitPrice, sizeof(StructIn->sOriginalUnitPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "NonDiscountTotalAmount", StructIn->sNonDiscountTotalAmount, sizeof(StructIn->sNonDiscountTotalAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OfflineTrans", &StructIn->sOfflineTrans, sizeof(StructIn->sOfflineTrans));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RequiredOlaData", &StructIn->sRequiredOlaData, sizeof(StructIn->sRequiredOlaData));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "UnitPriceIncludeDiscount", &StructIn->sUnitPriceIncludeDiscount, sizeof(StructIn->sUnitPriceIncludeDiscount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Threshold", StructIn->sThreshold, sizeof(StructIn->sThreshold));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ThresholdType", &StructIn->sThresholdType, sizeof(StructIn->sThresholdType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ClockRequired", &StructIn->sClockRequired, sizeof(StructIn->sClockRequired));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CSEDReceiptMode", &StructIn->sReceiptMode, sizeof(StructIn->sReceiptMode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CustomerID", StructIn->sCustomerID, sizeof(StructIn->sCustomerID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PreviousMediaMasterType", StructIn->sPreviousMediaMasterType, sizeof(StructIn->sPreviousMediaMasterType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ManagerID", StructIn->sManagerID, sizeof(StructIn->sManagerID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DoSessionAuth", &StructIn->sDoSessionAuth, sizeof(StructIn->sDoSessionAuth));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "WaitForPriceChange", &StructIn->sWaitForPriceChange, sizeof(StructIn->sWaitForPriceChange));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PMTReferenceNumber", StructIn->sPMTReferenceNumber, sizeof(StructIn->sPMTReferenceNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemRestriction", StructIn->sItemRestriction, sizeof(StructIn->sItemRestriction));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DieselLimit", StructIn->sDieselLimit, sizeof(StructIn->sDieselLimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ReeferLimit", StructIn->sReeferLimit, sizeof(StructIn->sReeferLimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CashBacklimit", StructIn->sCashBacklimit, sizeof(StructIn->sCashBacklimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ReeferTotal", StructIn->sReeferTotal, sizeof(StructIn->sReeferTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromptName", StructIn->sPromptName, sizeof(StructIn->sPromptName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromptMode", &StructIn->sPromptMode, sizeof(StructIn->sPromptMode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromptEncrypted", &StructIn->sPromptEncrypted, sizeof(StructIn->sPromptEncrypted));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "WaitPromptNeedMoreParam", &StructIn->sWaitPromptNeedMoreParam, sizeof(StructIn->sWaitPromptNeedMoreParam));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ClFlags", StructIn->sClFlags, sizeof(StructIn->sClFlags));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OperationalPrompt", &StructIn->sOperationalPrompt, sizeof(StructIn->sOperationalPrompt));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RestrictionItemRequired", &StructIn->sRestrictionItemRequired, sizeof(StructIn->sRestrictionItemRequired));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ReeferVolume", StructIn->sReeferVolume, sizeof(StructIn->sReeferVolume));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DryItemsRestriction", &StructIn->sDryItemsRestriction, sizeof(StructIn->sDryItemsRestriction));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PositionRestrictionActive", &StructIn->sPositionRestrictionActive, sizeof(StructIn->sPositionRestrictionActive));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Track3Data", StructIn->sTrack3Data, sizeof(StructIn->sTrack3Data));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AmountLimitIncludesCashBack", &StructIn->sAmountLimitIncludesCashBack, sizeof(StructIn->sAmountLimitIncludesCashBack));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DisplayCashBackAmount", &StructIn->sDisplayCashBackAmount, sizeof(StructIn->sDisplayCashBackAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AllowPartialCashBack", &StructIn->sAllowPartialCashBack, sizeof(StructIn->sAllowPartialCashBack));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OLABatchId", StructIn->sOLABatchId, sizeof(StructIn->sOLABatchId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CustomerValidationType", &StructIn->sCustomerValidationType, sizeof(StructIn->sCustomerValidationType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PreSetlimit", StructIn->sPreSetlimit, sizeof(StructIn->sPreSetlimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PreSetlimitType", &StructIn->sPreSetlimitType, sizeof(StructIn->sPreSetlimitType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TruckingCompanyName", StructIn->sTruckingCompanyName, sizeof(StructIn->sTruckingCompanyName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MagCapabilities", StructIn->sMagCapabilities, sizeof(StructIn->sMagCapabilities));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IssueNumber", StructIn->sIssueNumber, sizeof(StructIn->sIssueNumber));

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthorizerSystemID", StructIn->sAuthorizerSystemID, sizeof(StructIn->sAuthorizerSystemID)); //4.0.23.360 //4.0.24.71 117472

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TerminalType", StructIn->sTerminalType, sizeof(StructIn->sTerminalType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TerminalCountryCode", StructIn->sTerminalCountryCode, sizeof(StructIn->sTerminalCountryCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TransactionCurrencyCode", StructIn->sTransactionCurrencyCode, sizeof(StructIn->sTransactionCurrencyCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AIP", StructIn->sAIP, sizeof(StructIn->sAIP));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ATC", StructIn->sATC, sizeof(StructIn->sATC));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TVR", StructIn->sTVR, sizeof(StructIn->sTVR));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AID", StructIn->sAID, sizeof(StructIn->sAID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromotionTotalDiscount", StructIn->sPromotionTotalDiscount, sizeof(StructIn->sPromotionTotalDiscount)); //4.0.19.810 66705 //4.0.23.504
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVN", StructIn->sAVN, sizeof(StructIn->sAVN));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IssuerApplicationData", StructIn->sIssuerApplicationData, sizeof(StructIn->sIssuerApplicationData));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CryptogramInfoData", StructIn->sCryptogramInfoData, sizeof(StructIn->sCryptogramInfoData));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CVMResult", StructIn->sCVMResult, sizeof(StructIn->sCVMResult));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CombinedCardSelect", &StructIn->sCombinedCardSelect, sizeof(StructIn->sCombinedCardSelect));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TermResponseCode", StructIn->sTermResponseCode, sizeof(StructIn->sTermResponseCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ScreenPrompt", StructIn->sScreenPrompt, sizeof(StructIn->sScreenPrompt));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TimeCardInserted", StructIn->sTimeCardInserted, sizeof(StructIn->sTimeCardInserted));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TimeTRSRequired", StructIn->sTimeTRSRequired, sizeof(StructIn->sTimeTRSRequired));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TimeTRSResponse", StructIn->sTimeTRSResponse, sizeof(StructIn->sTimeTRSResponse));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TimeHCResponse", StructIn->sTimeHCResponse, sizeof(StructIn->sTimeHCResponse));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TimePINRequest", StructIn->sTimePINRequest, sizeof(StructIn->sTimePINRequest));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TimeOLARequest", StructIn->sTimeOLARequest, sizeof(StructIn->sTimeOLARequest));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TimeOLAResult", StructIn->sTimeOLAResult, sizeof(StructIn->sTimeOLAResult));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TimeTNXDetails", StructIn->sTimeTNXDetails, sizeof(StructIn->sTimeTNXDetails));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IssuerActiondesDenial", StructIn->sIssuerActiondesDenial, sizeof(StructIn->sIssuerActiondesDenial));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IssuerActiondesOnLinel", StructIn->sIssuerActiondesOnLinel, sizeof(StructIn->sIssuerActiondesOnLinel));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IssuerActiondesDefault", StructIn->sIssuerActiondesDefault, sizeof(StructIn->sIssuerActiondesDefault));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TransactionStatusMode", StructIn->sTransactionStatusMode, sizeof(StructIn->sTransactionStatusMode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ControllerRestriction", StructIn->sControllerRestriction, sizeof(StructIn->sControllerRestriction));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxId", StructIn->sTaxId, sizeof(StructIn->sTaxId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltySessionID", StructIn->sLoyaltySessionID, sizeof(StructIn->sLoyaltySessionID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AmountByPoints2", StructIn->sAmountByPoints2, sizeof(StructIn->sAmountByPoints2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromotionNumber2", StructIn->sPromotionNumber2, sizeof(StructIn->sPromotionNumber2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OLAQualifySpend0", StructIn->sOLAQualifySpend0, sizeof(StructIn->sOLAQualifySpend0));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OLAQualifySpend1", StructIn->sOLAQualifySpend1, sizeof(StructIn->sOLAQualifySpend1));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CardHolderVerifyMethod", StructIn->sCardHolderVerifyMethod, sizeof(StructIn->sCardHolderVerifyMethod));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "HostOnLineID", StructIn->sHostOnLineID, sizeof(StructIn->sHostOnLineID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ReasonOnLineCode", StructIn->sReasonOnLineCode, sizeof(StructIn->sReasonOnLineCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IssueDate", StructIn->sIssueDate, sizeof(StructIn->sIssueDate));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PONumber", StructIn->sPONumber, sizeof(StructIn->sPONumber)); //4.0.15.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DoNotPrintExpirationDate", &StructIn->sDoNotPrintExpirationDate, sizeof(StructIn->sDoNotPrintExpirationDate)); //4.0.23.500
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxIncluded", &StructIn->sTaxIncluded, sizeof(StructIn->sTaxIncluded));		//4.0.24.60 109690
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "KeyboardMask", &StructIn->sKeyboardMask, sizeof(StructIn->sKeyboardMask));	//4.0.16.70
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CardSegment", &StructIn->sCardSegment, sizeof(StructIn->sCardSegment));		//Liat LPE
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IssuerAuthenticationData", StructIn->sIssuerAuthData, sizeof(StructIn->sIssuerAuthData));				//TD 44410 4.0.15.101
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "UnrecognizedBillFound", &StructIn->sUnrecognizedBillFound, sizeof(StructIn->sUnrecognizedBillFound));	//4.0.17.90
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OverfillAdjusted", &StructIn->sOverfillAdjusted, sizeof(StructIn->sOverfillAdjusted));					//4.0.23.500
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MaskAccountNumber", &StructIn->sMaskAccountNumber, sizeof(StructIn->sMaskAccountNumber)); //4.0.25.80
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ChargeTo_MSB", &StructIn->sChargeTo_MSB, sizeof(StructIn->sChargeTo_MSB)); //4.0.27.44 from 1023 TD 226700





	xmlInitDoc->CreateElement("Limits", &pChildNode);
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);

	for (int i = 0 ; i < MAX_POSITIONS_PER_PUMP ; i++)
	{
		xmlInitDoc->CreateElement("Limit", &pChildNode2);
		xmlInitDoc->AppendChild(&pChildNode2, &pChildNode);
		CreateXMLFromStructGradeLimitsWithOutNodes (pszXMLOut, &StructIn->GradeLimitsArray[i], long(i), xmlInitDoc, &pChildNode2);
	}
}

void CXmlConvertor::CreateXMLFromStructTaxDataSummaryWithOutNodes(char * pszXMLOut, TAX_DATA_SUMMARY * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxName", StructIn->sTaxName, sizeof(StructIn->sTaxName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxRate", StructIn->sTaxRate, sizeof(StructIn->sTaxRate));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TDSTaxPrintChar", &StructIn->sTaxPrintChar, sizeof(StructIn->sTaxPrintChar));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TDSTaxAmount", StructIn->sTaxAmount, sizeof(StructIn->sTaxAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxNetAmount", StructIn->sTaxNetAmount, sizeof(StructIn->sTaxNetAmount));
}


void CXmlConvertor::CreateXMLFromStructBuyAtPumpTwoStructs(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT * StructIn, long line_id, BAP_ITEM_FOR_RECEIPT2 * StructIn2, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	BYTE			sTmp[10];	//4.0.18.501 merge from 4.0.15.291
	memset(sTmp, 0, sizeof(sTmp));
	sprintf((char *)sTmp, "%ld", line_id);

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ID", sTmp,  min(sizeof(sTmp), strlen((char *)sTmp)));	//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BOItemID", StructIn->sBOItemID, sizeof(StructIn->sBOItemID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Quantity", StructIn->sQuantity, sizeof(StructIn->sQuantity));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemCode", StructIn->sItemCode, sizeof(StructIn->sItemCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemBarcode", StructIn->sItemBarcode, sizeof(StructIn->sItemBarcode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemName", StructIn->sItemName, sizeof(StructIn->sItemName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxPrintChar", StructIn->sTaxPrintChar, sizeof(StructIn->sTaxPrintChar));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "UnitPrice", StructIn->sUnitPrice, sizeof(StructIn->sUnitPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AmountTaxIncluded", StructIn->sAmountTaxIncluded, sizeof(StructIn->sAmountTaxIncluded));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AmountTaxExluded", StructIn->sAmountTaxExluded, sizeof(StructIn->sAmountTaxExluded));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmount", StructIn->sTaxAmount, sizeof(StructIn->sTaxAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromotionVal", StructIn->sPromotionVal, sizeof(StructIn->sPromotionVal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DiscountVal", StructIn->sDiscountVal, sizeof(StructIn->sDiscountVal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IsFuelItem", &StructIn->byIsFuelItem, sizeof(StructIn->byIsFuelItem));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FuelVolume", StructIn->sFuelVolume, sizeof(StructIn->sFuelVolume));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IsCarWashItem", &StructIn->byIsCarWashItem, sizeof(StructIn->byIsCarWashItem));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashCode", StructIn->sCarWashCode, sizeof(StructIn->sCarWashCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashExpire", StructIn->sCarWashExpire, sizeof(StructIn->sCarWashExpire));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmount2", StructIn->sTaxAmount2, sizeof(StructIn->sTaxAmount2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmount3", StructIn->sTaxAmount3, sizeof(StructIn->sTaxAmount3));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemIsTaxIncluded", &StructIn->byItemIsTaxIncluded, sizeof(StructIn->byItemIsTaxIncluded));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IsWeightItem", &StructIn->byIsWeightItem, sizeof(StructIn->byIsWeightItem));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemSaleStatus", &StructIn->byItemSaleStatus, sizeof(StructIn->byItemSaleStatus));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Buff", StructIn->sBuff, sizeof(StructIn->sBuff));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemDiscPerUnit", StructIn2->byItemDiscPerUnit, sizeof(StructIn2->byItemDiscPerUnit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemUnitPriceWithDisc", StructIn2->byItemUnitPriceWithDisc, sizeof(StructIn2->byItemUnitPriceWithDisc));
}

void CXmlConvertor::CreateXMLFromStructBuyAtPumpAllTwoStructs(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT * StructIn, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructIn2, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	CXMLPointers pChildNode, pChildNode2, pChildNode3, pChildNode4;

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemsNumber", StructIn->sItemsNumber, sizeof(StructIn->sItemsNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TenderTrans", StructIn->sTenderTrans, sizeof(StructIn->sTenderTrans));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemsTotal", StructIn->sItemsTotal, sizeof(StructIn->sItemsTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromotionTotal", StructIn->sPromotionTotal, sizeof(StructIn->sPromotionTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DiscountTotal", StructIn->sDiscountTotal, sizeof(StructIn->sDiscountTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxTotal", StructIn->sTaxTotal, sizeof(StructIn->sTaxTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TransactionTotal", StructIn->sTransactionTotal, sizeof(StructIn->sTransactionTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPInvoiceNumber", StructIn->sInvoiceNumber, sizeof(StructIn->sInvoiceNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPFullCardName", StructIn->sFullCardName, sizeof(StructIn->sFullCardName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPCardType", StructIn->sCardType, sizeof(StructIn->sCardType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAuthNumber", StructIn->sAuthNumber, sizeof(StructIn->sAuthNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAccountNumber", StructIn->sAccountNumber, sizeof(StructIn->sAccountNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPExpire", StructIn->sExpire, sizeof(StructIn->sExpire));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPDateTime", StructIn->sDateTime, sizeof(StructIn->sDateTime));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPFullVehicleId", StructIn->sFullVehicleId, sizeof(StructIn->sFullVehicleId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MagKey", StructIn->sMagKey, sizeof(StructIn->sMagKey));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPOdometer", StructIn->sOdometer, sizeof(StructIn->sOdometer));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPBankTermId", StructIn->sBankTermId, sizeof(StructIn->sBankTermId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPSiteId", StructIn->sSiteId, sizeof(StructIn->sSiteId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPTransBatch", StructIn->sTransBatch, sizeof(StructIn->sTransBatch));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPReferenceNumber", StructIn->sReferenceNumber, sizeof(StructIn->sReferenceNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPReceiptType", StructIn->sReceiptType, sizeof(StructIn->sReceiptType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPSignatureSlip", &StructIn->bySignatureSlip, sizeof(StructIn->bySignatureSlip));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPIsOnLine", &StructIn->byIsOnLine, sizeof(StructIn->byIsOnLine));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "NonFuelTotal_TaxInc", StructIn->sNonFuelTotal_TaxInc, sizeof(StructIn->sNonFuelTotal_TaxInc));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "NonFuelTotal_TaxExc", StructIn->sNonFuelTotal_TaxExc, sizeof(StructIn->sNonFuelTotal_TaxExc));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAuthorizationSerialNumber", StructIn->sAuthorizationSerialNumber, sizeof(StructIn->sAuthorizationSerialNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAuthorizerFileNumber", StructIn->sAuthorizerFileNumber, sizeof(StructIn->sAuthorizerFileNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TotalRemainingBalance", StructIn->sTotalRemainingBalance, sizeof(StructIn->sTotalRemainingBalance));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAccountType", &StructIn->sAccountType, sizeof(StructIn->sAccountType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ABANumber", StructIn->sABANumber, sizeof(StructIn->sABANumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Branch", StructIn->sBranch, sizeof(StructIn->sBranch));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CheckSerial", StructIn->sCheckSerial, sizeof(StructIn->sCheckSerial));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPISOResponse", StructIn->sISOResponse, sizeof(StructIn->sISOResponse));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPSPDHCode", StructIn->sSPDHCode, sizeof(StructIn->sSPDHCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPTranStatus", &StructIn->sTranStatus, sizeof(StructIn->sTranStatus));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPSlipFlags", &StructIn->sSlipFlags, sizeof(StructIn->sSlipFlags));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthorizationDateTime", StructIn->sAuthorizationDateTime, sizeof(StructIn->sAuthorizationDateTime));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LanguageId", &StructIn->sLanguageId, sizeof(StructIn->sLanguageId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TrsRoundingAmount", StructIn->sTrsRoundingAmount, sizeof(StructIn->sTrsRoundingAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TransactionNum", StructIn->sTransactionNum, sizeof(StructIn->sTransactionNum));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TenderAmount", StructIn->sTenderAmount, sizeof(StructIn->sTenderAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ChangeAmount", StructIn->sChangeAmount, sizeof(StructIn->sChangeAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPTotalRedeemedPoints", StructIn->sTotalRedeemedPoints, sizeof(StructIn->sTotalRedeemedPoints));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAmountByPoints", StructIn->sAmountByPoints, sizeof(StructIn->sAmountByPoints));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPCreditAmount", StructIn->sCreditAmount, sizeof(StructIn->sCreditAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPCouponMessage", StructIn2->sCouponMessage, sizeof(StructIn2->sCouponMessage));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPCustomerName", StructIn2->sCustomerName, sizeof(StructIn2->sCustomerName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CustomerNewObligo", StructIn2->sCustomerNewObligo, sizeof(StructIn2->sCustomerNewObligo));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Tmp", StructIn2->sTmp, sizeof(StructIn2->sTmp));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAttributes", StructIn2->sTaxAttributes, sizeof(StructIn2->sTaxAttributes));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPLogicShiftNumber", StructIn2->sLogicShiftNumber, sizeof(StructIn2->sLogicShiftNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPReserveForCoupon", StructIn2->sReserveForCoupon, sizeof(StructIn2->sReserveForCoupon));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPReceiptMode", &StructIn2->sReceiptMode, sizeof(StructIn2->sReceiptMode));

	xmlInitDoc->CreateElement("Items", &pChildNode);
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);

	for (int i = 0 ; i < MAX_ITEMS_NUM ; i++)
	{
		xmlInitDoc->CreateElement("Item", &pChildNode2);
		xmlInitDoc->AppendChild(&pChildNode2, &pChildNode);
		CreateXMLFromStructBuyAtPumpTwoStructs(pszXMLOut, &StructIn->Items[i], long(i), &StructIn2->Items[i], xmlInitDoc, &pChildNode2);
	}
	BYTE			sTmp[15];
	memset(sTmp, 0, sizeof(sTmp));

	xmlInitDoc->CreateElement("TaxTotalList", &pChildNode3);
	xmlInitDoc->AppendChild(&pChildNode3, pCurrElement);
	xmlInitDoc->CreateElement("TaxNameList", &pChildNode4);
	xmlInitDoc->AppendChild(&pChildNode4, pCurrElement);

	for (int i = 0 ; i < MAX_TAX ; i++)
	{
		sprintf((char *) sTmp, "TaxTotalList%ld", i);
		EndXmlLineWithNull(xmlInitDoc, &pChildNode3, (char*)sTmp, StructIn2->sTaxTotalList[i], sizeof(StructIn2->sTaxTotalList[i]));
		sprintf((char *) sTmp, "TaxNameList%ld", i);
		EndXmlLineWithNull(xmlInitDoc, &pChildNode4, (char*)sTmp, StructIn2->sTaxNameList[i], sizeof(StructIn2->sTaxNameList[i]));
	}
}

void CXmlConvertor::CreateXMLFromStructGradeLimitsWithOutNodes(char * pszXMLOut, GRADE_LIMITS * StructIn, long line_id, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	BYTE sTmp[10]; //4.0.18.501 merge from 4.0.15.291
	memset(sTmp, 0, sizeof(sTmp));
	sprintf((char *)sTmp, "%ld", line_id);

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ID", sTmp, min(sizeof(sTmp), strlen((char *)sTmp)));		//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GradeAmountLimit", StructIn->sGradeAmountLimit, sizeof(StructIn->sGradeAmountLimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GradeID", StructIn->sGradeID, sizeof(StructIn->sGradeID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LimitType", &StructIn->sLimitType, sizeof(StructIn->sLimitType));
}

void CXmlConvertor::CreateXMLFromStructDiscntWithOutNodes(char * pszXMLOut, DISCNT * StructIn, long line_id, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	BYTE			sTmp[10]; //4.0.18.501 merge from 4.0.15.291
	memset(sTmp, 0, sizeof(sTmp));
	sprintf((char *)sTmp, "%ld", line_id);

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ID", sTmp, min(sizeof(sTmp), strlen((char *)sTmp)));		//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Discount", StructIn->sDiscount, sizeof(StructIn->sDiscount));
}

void CXmlConvertor::CreateXMLFromStructSaleWithOutNodes(char * pszXMLOut, SALE * StructIn, long line_id, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	BYTE			sTmp[10]; //4.0.18.501 merge from 4.0.15.291
	memset(sTmp, 0, sizeof(sTmp));
	sprintf((char *)sTmp, "%ld", line_id);

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ID", sTmp, min(sizeof(sTmp), strlen((char *)sTmp)));		//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ProdDesc", StructIn->sProdDesc, sizeof(StructIn->sProdDesc));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DPTProdCode", StructIn->sDPTProdCode, sizeof(StructIn->sDPTProdCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Qty", StructIn->sQty, sizeof(StructIn->sQty));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "UnitPrice", StructIn->sUnitPrice, sizeof(StructIn->sUnitPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "SaleDiscount", StructIn->sDiscount, sizeof(StructIn->sDiscount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Tax", StructIn->sTax, sizeof(StructIn->sTax));
	
	if (a2l(StructIn->sDPTProdCode, sizeof(StructIn->sDPTProdCode)) > 0)
		EndXmlLineWithNull(xmlInitDoc, pCurrElement, "sValueFactor", (unsigned char*)"2", sizeof("2"));		//4.0.33.313
}


void CXmlConvertor::CreateXMLFromStructBuyAtPumpItemForReceiptWithOutNodes(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT * StructIn, long line_id, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	BYTE sTmp[10]; //4.0.18.501 merge from 4.0.15.291
	memset(sTmp, 0, sizeof(sTmp));
	sprintf((char *) sTmp, "%ld", line_id);

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ID", sTmp, min(sizeof(sTmp), strlen((char *) sTmp)));		//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BOItemID", StructIn->sBOItemID, sizeof(StructIn->sBOItemID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Quantity", StructIn->sQuantity, sizeof(StructIn->sQuantity));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemCode", StructIn->sItemCode, sizeof(StructIn->sItemCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemBarcode", StructIn->sItemBarcode, sizeof(StructIn->sItemBarcode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemName", StructIn->sItemName, sizeof(StructIn->sItemName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxPrintChar", StructIn->sTaxPrintChar, sizeof(StructIn->sTaxPrintChar));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "UnitPrice", StructIn->sUnitPrice, sizeof(StructIn->sUnitPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AmountTaxIncluded", StructIn->sAmountTaxIncluded, sizeof(StructIn->sAmountTaxIncluded));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AmountTaxExluded", StructIn->sAmountTaxExluded, sizeof(StructIn->sAmountTaxExluded));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmount", StructIn->sTaxAmount, sizeof(StructIn->sTaxAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromotionVal", StructIn->sPromotionVal, sizeof(StructIn->sPromotionVal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DiscountVal", StructIn->sDiscountVal, sizeof(StructIn->sDiscountVal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IsFuelItem", &StructIn->byIsFuelItem, sizeof(StructIn->byIsFuelItem));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FuelVolume", StructIn->sFuelVolume, sizeof(StructIn->sFuelVolume));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IsCarWashItem", &StructIn->byIsCarWashItem, sizeof(StructIn->byIsCarWashItem));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashCode", StructIn->sCarWashCode, sizeof(StructIn->sCarWashCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashExpire", StructIn->sCarWashExpire, sizeof(StructIn->sCarWashExpire));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmount2", StructIn->sTaxAmount2, sizeof(StructIn->sTaxAmount2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmount3", StructIn->sTaxAmount3, sizeof(StructIn->sTaxAmount3));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemIsTaxIncluded", &StructIn->byItemIsTaxIncluded, sizeof(StructIn->byItemIsTaxIncluded));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IsWeightItem", &StructIn->byIsWeightItem, sizeof(StructIn->byIsWeightItem));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemSaleStatus", &StructIn->byItemSaleStatus, sizeof(StructIn->byItemSaleStatus));
}

void CXmlConvertor::CreateXMLFromStructBuyAtPumpItemForReceipt2WithOutNodes(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT2 * StructIn, long line_id, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	BYTE			sTmp[10]; //4.0.18.501 merge from 4.0.15.291
	memset(sTmp, 0, sizeof(sTmp));
	sprintf((char *)sTmp, "%ld", line_id);

	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"ID", sTmp, min(sizeof(sTmp), strlen((char *)sTmp)));		//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"byItemDiscPerUnit", StructIn->byItemDiscPerUnit, sizeof(StructIn->byItemDiscPerUnit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"byItemUnitPriceWithDisc", StructIn->byItemUnitPriceWithDisc, sizeof(StructIn->byItemUnitPriceWithDisc));
}


void CXmlConvertor::CreateXMLFromStructBuyAtPumpAllItemsForReceiptWithOutNodes(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	CXMLPointers	pChildNode, pChildNode2;

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemsNumber", StructIn->sItemsNumber, sizeof(StructIn->sItemsNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TenderTrans", StructIn->sTenderTrans, sizeof(StructIn->sTenderTrans));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ItemsTotal", StructIn->sItemsTotal, sizeof(StructIn->sItemsTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromotionTotal", StructIn->sPromotionTotal, sizeof(StructIn->sPromotionTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DiscountTotal", StructIn->sDiscountTotal, sizeof(StructIn->sDiscountTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxTotal", StructIn->sTaxTotal, sizeof(StructIn->sTaxTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TransactionTotal", StructIn->sTransactionTotal, sizeof(StructIn->sTransactionTotal));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPInvoiceNumber", StructIn->sInvoiceNumber, sizeof(StructIn->sInvoiceNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPFullCardName", StructIn->sFullCardName, sizeof(StructIn->sFullCardName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPCardType", StructIn->sCardType, sizeof(StructIn->sCardType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAuthNumber", StructIn->sAuthNumber, sizeof(StructIn->sAuthNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAccountNumber", StructIn->sAccountNumber, sizeof(StructIn->sAccountNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPExpire", StructIn->sExpire, sizeof(StructIn->sExpire));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPDateTime", StructIn->sDateTime, sizeof(StructIn->sDateTime));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPFullVehicleId", StructIn->sFullVehicleId, sizeof(StructIn->sFullVehicleId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MagKey", StructIn->sMagKey, sizeof(StructIn->sMagKey));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPOdometer", StructIn->sOdometer, sizeof(StructIn->sOdometer));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPBankTermId", StructIn->sBankTermId, sizeof(StructIn->sBankTermId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPSiteId", StructIn->sSiteId, sizeof(StructIn->sSiteId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPTransBatch", StructIn->sTransBatch, sizeof(StructIn->sTransBatch));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPReferenceNumber", StructIn->sReferenceNumber, sizeof(StructIn->sReferenceNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPReceiptType", StructIn->sReceiptType, sizeof(StructIn->sReceiptType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPSignatureSlip", &StructIn->bySignatureSlip, sizeof(StructIn->bySignatureSlip));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPIsOnLine", &StructIn->byIsOnLine, sizeof(StructIn->byIsOnLine));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "NonFuelTotal_TaxInc", StructIn->sNonFuelTotal_TaxInc, sizeof(StructIn->sNonFuelTotal_TaxInc));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "NonFuelTotal_TaxExc", StructIn->sNonFuelTotal_TaxExc, sizeof(StructIn->sNonFuelTotal_TaxExc));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAuthorizationSerialNumber", StructIn->sAuthorizationSerialNumber, sizeof(StructIn->sAuthorizationSerialNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAuthorizerFileNumber", StructIn->sAuthorizerFileNumber, sizeof(StructIn->sAuthorizerFileNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TotalRemainingBalance", StructIn->sTotalRemainingBalance, sizeof(StructIn->sTotalRemainingBalance));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAccountType", &StructIn->sAccountType, sizeof(StructIn->sAccountType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ABANumber", StructIn->sABANumber, sizeof(StructIn->sABANumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Branch", StructIn->sBranch, sizeof(StructIn->sBranch));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CheckSerial", StructIn->sCheckSerial, sizeof(StructIn->sCheckSerial));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPISOResponse", StructIn->sISOResponse, sizeof(StructIn->sISOResponse));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPSPDHCode", StructIn->sSPDHCode, sizeof(StructIn->sSPDHCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPTranStatus", &StructIn->sTranStatus, sizeof(StructIn->sTranStatus));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPSlipFlags", &StructIn->sSlipFlags, sizeof(StructIn->sSlipFlags));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthorizationDateTime", StructIn->sAuthorizationDateTime, sizeof(StructIn->sAuthorizationDateTime));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LanguageId", &StructIn->sLanguageId, sizeof(StructIn->sLanguageId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TrsRoundingAmount", StructIn->sTrsRoundingAmount, sizeof(StructIn->sTrsRoundingAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TransactionNum", StructIn->sTransactionNum, sizeof(StructIn->sTransactionNum));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TenderAmount", StructIn->sTenderAmount, sizeof(StructIn->sTenderAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ChangeAmount", StructIn->sChangeAmount, sizeof(StructIn->sChangeAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPTotalRedeemedPoints", StructIn->sTotalRedeemedPoints, sizeof(StructIn->sTotalRedeemedPoints));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPAmountByPoints", StructIn->sAmountByPoints, sizeof(StructIn->sAmountByPoints));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPCreditAmount", StructIn->sCreditAmount, sizeof(StructIn->sCreditAmount));

	xmlInitDoc->CreateElement("Items", &pChildNode);
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);

	for (int i = 0 ; i < MAX_ITEMS_NUM ; i++)
	{
		xmlInitDoc->CreateElement("Item", &pChildNode2);
		xmlInitDoc->AppendChild(&pChildNode2, &pChildNode);
		CreateXMLFromStructBuyAtPumpItemForReceiptWithOutNodes (pszXMLOut, &StructIn->Items[i], long(i), xmlInitDoc, &pChildNode2);
	}
}

void CXmlConvertor::CreateXMLFromStructBuyAtPumpAllItemsForReceipt2WithOutNodes(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	CXMLPointers	pChildNode, pChildNode2, pChildNode3, pChildNode4;

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPCouponMessage", StructIn->sCouponMessage, sizeof(StructIn->sCouponMessage));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPCustomerName", StructIn->sCustomerName, sizeof(StructIn->sCustomerName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CustomerNewObligo", StructIn->sCustomerNewObligo, sizeof(StructIn->sCustomerNewObligo));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Tmp", StructIn->sTmp, sizeof(StructIn->sTmp));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAttributes", StructIn->sTaxAttributes, sizeof(StructIn->sTaxAttributes));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPLogicShiftNumber", StructIn->sLogicShiftNumber, sizeof(StructIn->sLogicShiftNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPReserveForCoupon", StructIn->sReserveForCoupon, sizeof(StructIn->sReserveForCoupon));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BAPReceiptMode", &StructIn->sReceiptMode, sizeof(StructIn->sReceiptMode));
	//EndXmlLineWithNull(xmlInitDoc, pCurrElement,"sFiller", StructIn->sFiller, sizeof(StructIn->sFiller));

	xmlInitDoc->CreateElement("Items", &pChildNode);
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);

	for (int i = 0 ; i < MAX_ITEMS_NUM ; i++)
	{
		xmlInitDoc->CreateElement("Item", &pChildNode2);
		xmlInitDoc->AppendChild(&pChildNode2, &pChildNode);
		CreateXMLFromStructBuyAtPumpItemForReceipt2WithOutNodes (pszXMLOut, &StructIn->Items[i], long(i), xmlInitDoc, &pChildNode2);
	}

	BYTE			sTmp[15];
	memset(sTmp, 0, sizeof(sTmp));

	xmlInitDoc->CreateElement("TaxTotalList", &pChildNode3);
	xmlInitDoc->AppendChild(&pChildNode3, pCurrElement);
	xmlInitDoc->CreateElement("TaxNameList", &pChildNode4);
	xmlInitDoc->AppendChild(&pChildNode4, pCurrElement);

	for (int i = 0 ; i < MAX_TAX ; i++)
	{
		sprintf((char *) sTmp, "TaxTotalList%ld", i);
		EndXmlLineWithNull(xmlInitDoc, &pChildNode3,(char*) sTmp, StructIn->sTaxTotalList[i], sizeof(StructIn->sTaxTotalList[i]));
		sprintf((char *) sTmp, "TaxNameList%ld", i);
		EndXmlLineWithNull(xmlInitDoc, &pChildNode3,(char*) sTmp, StructIn->sTaxNameList[i], sizeof(StructIn->sTaxNameList[i]));
	}
}

void CXmlConvertor::CreateXMLFromStructTagGeneralInfoWithOutNodes(char * pszXMLOut, TAG_GENERAL_INFO * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Volume", StructIn->sVolume, sizeof(StructIn->sVolume));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RECGradePrice", StructIn->sGradePrice, sizeof(StructIn->sGradePrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Grade", StructIn->sGrade, sizeof(StructIn->sGrade));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FuelValue", StructIn->sFuelValue, sizeof(StructIn->sFuelValue));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ReceiptNumber", StructIn->sReceiptNumber, sizeof(StructIn->sReceiptNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GradeName", StructIn->cGradeName, sizeof(StructIn->cGradeName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PathToPumpSrv", StructIn->cPathToPumpSrv, sizeof(StructIn->cPathToPumpSrv));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "VolumeFactor", StructIn->sVolumeFactor, sizeof(StructIn->sVolumeFactor));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CurrencyFactor", StructIn->sCurrencyFactor, sizeof(StructIn->sCurrencyFactor));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IsOriginalCopy", &StructIn->byIsOriginalCopy, sizeof(StructIn->byIsOriginalCopy));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "HeaderDynamicBuffer", StructIn->cHeaderDynamicBuffer, sizeof(StructIn->cHeaderDynamicBuffer));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "HeaderDynamicBufferLen", StructIn->sHeaderDynamicBufferLen, sizeof(StructIn->sHeaderDynamicBufferLen));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FooterDynamicBuffer", StructIn->cFooterDynamicBuffer, sizeof(StructIn->cFooterDynamicBuffer));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FooterDynamicBufferLen", StructIn->sFooterDynamicBufferLen, sizeof(StructIn->sFooterDynamicBufferLen));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashProgramName", StructIn->cCarWashProgramName, sizeof(StructIn->cCarWashProgramName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashTotalPrice", StructIn->sCarWashTotalPrice, sizeof(StructIn->sCarWashTotalPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CouponExpDate", StructIn->sCouponExpDate, sizeof(StructIn->sCouponExpDate));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TransactionNumber", StructIn->sTransactionNumber, sizeof(StructIn->sTransactionNumber));
}

void CXmlConvertor::CreateXMLFromStructReceiptExtraInfoWithOutNodes(char * pszXMLOut, RECEIPT_EXTRA_INFO * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"Filer2043", StructIn->sFiler2043, sizeof(StructIn->sFiler2043));
}

//4.0.24.60 109103
void CXmlConvertor::CreateXMLFromStructOlaExtraDataWithOutNodes(char * pszXMLOut, OLA_EXTRA_DATA * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement) //4.0.24.60 109103
{
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OlaExtraDataLen", StructIn->byLen, sizeof(StructIn->byLen));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OlaExtraData", StructIn->byData, sizeof(StructIn->byData));
}

void CXmlConvertor::CreateXMLFromStructTagPumpTransact2WithOutNodes(char * pszXMLOut, TAG_PUMP_TRANSACT2 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	CXMLPointers	pChildNode;
	char			tempMisch [10] = {0};

	//connecting the values of sMiscH and sMisc to one.
	memcpy(tempMisch, StructIn->sMiscH, sizeof(StructIn->sMiscH));
	memcpy(tempMisch+5, StructIn->sMisc, sizeof(StructIn->sMisc));

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Number", StructIn->sNumber, sizeof(StructIn->sNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Status", StructIn->sStatus, sizeof(StructIn->sStatus));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OnOffer", StructIn->sOnOffer, sizeof(StructIn->sOnOffer));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RawVolume", StructIn->sRawVolume, sizeof(StructIn->sRawVolume));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RoundedVolume", StructIn->sRoundedVolume, sizeof(StructIn->sRoundedVolume));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RoundedValue", StructIn->sRoundedValue, sizeof(StructIn->sRoundedValue));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TRGradePrice", StructIn->sGradePrice, sizeof(StructIn->sGradePrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TRNozzle", StructIn->sNozzle, sizeof(StructIn->sNozzle));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Grade", StructIn->sGrade, sizeof(StructIn->sGrade));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Level", StructIn->sLevel, sizeof(StructIn->sLevel));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Pump", StructIn->sPump, sizeof(StructIn->sPump));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Mode", StructIn->sMode, sizeof(StructIn->sMode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Res", StructIn->sRes, sizeof(StructIn->sRes));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ResExt", StructIn->sResExt, sizeof(StructIn->sResExt));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LinkNumber", StructIn->sLinkNumber, sizeof(StructIn->sLinkNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LinkFlags", StructIn->sLinkFlags, sizeof(StructIn->sLinkFlags));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ActiveReadNumber", StructIn->sActiveReadNumber, sizeof(StructIn->sActiveReadNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ActiveShiftNumber", StructIn->sActiveShiftNumber, sizeof(StructIn->sActiveShiftNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Misc",(unsigned char*)tempMisch, sizeof(tempMisch));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PrePaidValue", StructIn->sPrePaidValue, sizeof(StructIn->sPrePaidValue));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ServiceMode", StructIn->sServiceMode, sizeof(StructIn->sServiceMode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Offers2Pos", &StructIn->sOffers2Pos, sizeof(StructIn->sOffers2Pos));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Grade2", StructIn->sGrade2, sizeof(StructIn->sGrade2));

	xmlInitDoc->CreateElement("TAG_SYSTEMTIME", &pChildNode);
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);
	CreateXMLFromStructTagSystemTimeWithOutNodes (pszXMLOut, &StructIn->sDateTime, xmlInitDoc, &pChildNode);
}

//TD 38654
void CXmlConvertor::CreateXMLFromStructTagPumpTransact5WithOutNodes(char * pszXMLOut, TAG_PUMP_TRANSACT5 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	CXMLPointers	pChildNode;
	char			tempMisch [10] = {0};

	//connecting the values of sMiscH and sMisc to one.
	memcpy(tempMisch, StructIn->sMiscH, sizeof(StructIn->sMiscH));
	memcpy(tempMisch+5, StructIn->sMisc, sizeof(StructIn->sMisc));

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Number", StructIn->sNumber, sizeof(StructIn->sNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Status", StructIn->sStatus, sizeof(StructIn->sStatus));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OnOffer", StructIn->sOnOffer, sizeof(StructIn->sOnOffer));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RawVolume", StructIn->sRawVolume, sizeof(StructIn->sRawVolume));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RoundedVolume", StructIn->sRoundedVolume, sizeof(StructIn->sRoundedVolume));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RoundedValue", StructIn->sRoundedValue, sizeof(StructIn->sRoundedValue));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TRGradePrice", StructIn->sGradePrice, sizeof(StructIn->sGradePrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TRNozzle", StructIn->sNozzle, sizeof(StructIn->sNozzle));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Grade", StructIn->sGrade, sizeof(StructIn->sGrade));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Level", StructIn->sLevel, sizeof(StructIn->sLevel));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Pump", StructIn->sPump, sizeof(StructIn->sPump));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Mode", StructIn->sMode, sizeof(StructIn->sMode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Res", StructIn->sRes, sizeof(StructIn->sRes));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ResExt", StructIn->sResExt, sizeof(StructIn->sResExt));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LinkNumber", StructIn->sLinkNumber, sizeof(StructIn->sLinkNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LinkFlags", StructIn->sLinkFlags, sizeof(StructIn->sLinkFlags));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ActiveReadNumber", StructIn->sActiveReadNumber, sizeof(StructIn->sActiveReadNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ActiveShiftNumber", StructIn->sActiveShiftNumber, sizeof(StructIn->sActiveShiftNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Misc",(unsigned char*)tempMisch, sizeof(tempMisch));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PrePaidValue", StructIn->sPrePaidValue, sizeof(StructIn->sPrePaidValue));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ServiceMode", StructIn->sServiceMode, sizeof(StructIn->sServiceMode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Offers2Pos", &StructIn->sOffers2Pos, sizeof(StructIn->sOffers2Pos));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Grade2", StructIn->sGrade2, sizeof(StructIn->sGrade2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyPriceProtection", &StructIn->sLoyaltyPriceProtection, sizeof(StructIn->sLoyaltyPriceProtection));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyAutoArm", &StructIn->sLoyaltyAutoArm, sizeof(StructIn->sLoyaltyAutoArm));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LowestGradePrice", StructIn->sLowestGradePrice, sizeof(StructIn->sLowestGradePrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TankId", StructIn->sTankID, sizeof(StructIn->sTankID)); //4.0.27.60   TD 231475

	long lGrade = a2l(StructIn->sGrade, sizeof(StructIn->sGrade));
	CString csTmpAditionalCode = _Module.m_server.GetGradeAditionalCode(lGrade);
	BYTE sAditionalCode[16];

	memset(sAditionalCode, 0, sizeof(sAditionalCode));

	if (csTmpAditionalCode.GetLength() > 0)
	{
		memcpy(sAditionalCode, csTmpAditionalCode, min(sizeof(sAditionalCode), csTmpAditionalCode.GetLength()));
	}

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "NacsAdditionalCode", sAditionalCode, sizeof(sAditionalCode));


	xmlInitDoc->CreateElement("TAG_SYSTEMTIME", &pChildNode);
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);
	CreateXMLFromStructTagSystemTimeWithOutNodes (pszXMLOut, &StructIn->sDateTime, xmlInitDoc, &pChildNode);

	xmlInitDoc->CreateElement("FlowStarted", &pChildNode);
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);
	CreateXMLFromStructTagSystemTimeWithOutNodes(pszXMLOut, &StructIn->sStartDateTime, xmlInitDoc, &pChildNode);

	//4.0.22.511 72606
	xmlInitDoc->CreateElement("DispensingStarted", &pChildNode);
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);
	CreateXMLFromStructTagSystemTimeWithOutNodes(pszXMLOut, &StructIn->sStartFuelingDateTime, xmlInitDoc, &pChildNode);
	//	xmlInitDoc->SaveToFile("c:\\xxTAG_OLA_REC5_EX.xml"); //Liat 2/5/07
}
void CXmlConvertor::CreateXMLFromStructTagSystemTimeWithOutNodes(char * pszXMLOut, TAG_SYSTEMTIME * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Year", StructIn->sYear, sizeof(StructIn->sYear));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Month", StructIn->sMonth, sizeof(StructIn->sMonth));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Day", StructIn->sDay, sizeof(StructIn->sDay));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Hour", StructIn->sHour, sizeof(StructIn->sHour));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Minute", StructIn->sMinute, sizeof(StructIn->sMinute));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Second", StructIn->sSecond, sizeof(StructIn->sSecond));
}

//4.0.16.504
void CXmlConvertor::CreateXMLFromStructDallasKeysInfo(char * pszXMLOut, DALLAS_KEYS_INFO * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVIBlockedListBuffer", StructIn->sBlockedListBuffer, sizeof(StructIn->sBlockedListBuffer));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVIBlockedListBufferLength", &StructIn->sBlockedListBufferLength, sizeof(StructIn->sBlockedListBufferLength));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVIBlockListPointer", StructIn->sBlockListPointer, sizeof(StructIn->sBlockListPointer));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AVIBlockedKeysRequest", &StructIn->sBlockedKeysRequest, sizeof(StructIn->sBlockedKeysRequest));
}

//4.0.17.501 - CR 38162
void CXmlConvertor::CreateXMLFromStructDiscountAttrsInfo(char * pszXMLOut, DISCOUNT_ATTRIBUTES * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement, DISCOUNT_ATTRIBUTES_EXT * StructIn2)	//4.0.22.509 38162
{
	CXMLPointers	pChildNode, pChildNode2;

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyType", StructIn->sLoyaltyType, sizeof(StructIn->sLoyaltyType));

	xmlInitDoc->CreateElement("ItemsDiscountIDs", &pChildNode);
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);
	for (int i = 0 ; i < MAX_DISCOUNTS_PER_ITEM ; i++)
	{
		xmlInitDoc->CreateElement("DiscountID", &pChildNode2);
		xmlInitDoc->AppendChild(&pChildNode2, &pChildNode);
		CreateXMLFromStructDiscountIDsWithOutNodes (pszXMLOut, &StructIn->DiscountIDs[i], xmlInitDoc, &pChildNode2);
		EndXmlLineWithNull(xmlInitDoc, &pChildNode2, "ItemDiscountType", StructIn2->sDiscountTypes[i].sItemDiscountType, sizeof(StructIn2->sDiscountTypes[i].sItemDiscountType)); //4.0.22.509 38162

	}
}

void CXmlConvertor::CreateXMLFromStructDiscountIDsWithOutNodes(char * pszXMLOut, ITEM_DISCOUNT_ID * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Grade", StructIn->sDiscountGrade, sizeof(StructIn->sDiscountGrade)); //4.0.22.509 38162
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ID", StructIn->sDiscountID, sizeof(StructIn->sDiscountID));
}

//4.0.16.504
void CXmlConvertor::CreateXMLFromStructCardSaleExtraData4(char * pszXMLOut, CARD_SALE_EXTRA_DATA4_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	BYTE tempCardRangeNo[4]= {0};
	//connecting the values of sCardRangeNo
	memcpy(tempCardRangeNo, StructIn->sCardRangeNo, sizeof(StructIn->sCardRangeNo));
	memcpy(tempCardRangeNo+3, &(StructIn->sCardRangeNoExt), sizeof(StructIn->sCardRangeNoExt)); //4.0.27.501    TD 228117

	CreateXMLFromStructDallasKeysInfo(pszXMLOut, &StructIn->dallasKeysInfo, xmlInitDoc, pCurrElement);
	CreateXMLFromStructDiscountAttrsInfo(pszXMLOut, &StructIn->discountAttrsInfo, xmlInitDoc, pCurrElement, &StructIn->sDiscountAttrExt); //4.0.17.501 //4.0.22.509 38162
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyOnOff", &StructIn->loyaltyOnOff, sizeof(StructIn->loyaltyOnOff));	//LPE
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ExtendedZipCode", StructIn->sExtendedZipCode, sizeof(StructIn->sExtendedZipCode)); //46512
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IFD", StructIn->sIFD, sizeof(StructIn->sIFD)); //42728
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MediaNumber", StructIn->sMediaNumber, sizeof(StructIn->sMediaNumber)); //42728
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FormattedClubCardNumber", StructIn->sFormattedClubCardNumber, sizeof(StructIn->sFormattedClubCardNumber)); //42728
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyEntryMethod", &StructIn->sLoyaltyEntryMethod, sizeof(StructIn->sLoyaltyEntryMethod));	//4.0.18.504

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PumpTestPassword", StructIn->sPumpTestPassword, sizeof(StructIn->sPumpTestPassword)); //54295
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ExtendedAVISpecialCodes", StructIn->sExtendedAVISpecialCodes, sizeof(StructIn->sExtendedAVISpecialCodes)); //25853 4.0.18.503
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FallBackTrs", &StructIn->sIsOfflineTrs, sizeof(StructIn->sIsOfflineTrs));	//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PreAuthAmount", StructIn->sPreAuthAmount, sizeof(StructIn->sPreAuthAmount));// merge from 4.0.18.181
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TotalEftNetworkTime", StructIn->sTotalEftNetworkTime, sizeof(StructIn->sTotalEftNetworkTime)); //54295	//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CustomerEftWaitTime", StructIn->sCustomerEftWaitTime, sizeof(StructIn->sCustomerEftWaitTime)); //54295	//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CLDateTime", StructIn->sCLDateTime, sizeof(StructIn->sCLDateTime)); //58096	//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "StaffFlag", &StructIn->sStaffFlag, sizeof(StructIn->sStaffFlag));	//58096	//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ONMPProperties", StructIn->sONMPProperties, sizeof(StructIn->sONMPProperties));	//TD 55286	//4.0.19.502 Copient
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashOption", StructIn->sCarWashOption, sizeof(StructIn->sCarWashOption));	//	TD 55286	//4.0.19.502 Copient
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "HotCard", &StructIn->sHotCard, sizeof(StructIn->sHotCard));	//4.0.19.502
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PanSeqNum", StructIn->sPanSeqNum, sizeof(StructIn->sPanSeqNum)); //TD 59898 4.0.19.503
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "SponsorNum", StructIn->sSponsorNum, sizeof(StructIn->sSponsorNum)); //TD 59898 4.0.19.503
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ExtraChipInfo", &StructIn->sExtraChipInfo, sizeof(StructIn->sExtraChipInfo));	//4.0.20.20
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TransactionType", StructIn->sTransactionType, sizeof(StructIn->sTransactionType));	//4.0.20.20
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ExistLoyaltyLimit", &StructIn->sExistLoyaltyLimit, sizeof(StructIn->sExistLoyaltyLimit));					//4.0.20.501 TD 60236
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "EncryptedAccount", StructIn->sEncryptedAccount, sizeof(StructIn->sEncryptedAccount));						//4.0.20.501 TD 60236
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "EncryptedAccount2", StructIn->sEncryptedAccount2, sizeof(StructIn->sEncryptedAccount2));						//4.0.20.501 TD 60236
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DSSFailed", &StructIn->sDSSFailed, sizeof(StructIn->sDSSFailed));											//4.0.20.501 TD 60236
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GetReceiptMethod", &StructIn->byGetReceiptMethod, sizeof(StructIn->byGetReceiptMethod));						//4.0.20.501 TD 60203
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RemoveProhibitedData", &StructIn->byIsRemoveProhibitedData, sizeof(StructIn->byIsRemoveProhibitedData));	//4.0.20.501 TD 60236
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyVolumeLimit", StructIn->sLoyaltyVolumeLimit, sizeof(StructIn->sLoyaltyVolumeLimit));	//4.0.21.0 67854
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyOriginalUnitPrice", StructIn->sLoyaltyOriginalUnitPrice, sizeof(StructIn->sLoyaltyOriginalUnitPrice));	//4.0.21.0 67854
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ForceZeroPAKCompletion", &StructIn->sForceZeroPAKCompletion, sizeof(StructIn->sForceZeroPAKCompletion));	//4.0.21.10
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "InPAKLoyaltyWithComplete", &StructIn->sInPAKLoyaltyWithComplete, sizeof(StructIn->sInPAKLoyaltyWithComplete));	//4.0.21.10
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CancelAborts", &StructIn->sCancelAborts, sizeof(StructIn->sCancelAborts));	//4.0.21.10 64119
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CardRangeNo", tempCardRangeNo, sizeof(tempCardRangeNo)); //4.0.22.504 TD 68871
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PanRequiredProtection", &StructIn->sPanRequiredProtection, sizeof(StructIn->sPanRequiredProtection));			//4.0.22.504 TD 72716
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "StoreMode", &StructIn->sStoreMode, sizeof(StructIn->sStoreMode));												//4.0.22.504 TD 72716
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ActualMode", &StructIn->sActualMode, sizeof(StructIn->sActualMode));											//4.0.22.504 TD 72716
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "EncryptionKeyRef", StructIn->sEncryptionKeyRef, sizeof(StructIn->sEncryptionKeyRef));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Track2Exist", &StructIn->sTrack2Exist, sizeof(StructIn->sTrack2Exist));											//4.0.22.40 TD 72716
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DeferralAccepted", &StructIn->sDeferralAccepted, sizeof(StructIn->sDeferralAccepted));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyName", StructIn->sLoyaltyName, sizeof(StructIn->sLoyaltyName));											//4.0.22.509 74387
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Job", StructIn->sJob, sizeof(StructIn->sJob));															//4.0.22.511 74797
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "Department", StructIn->sDepartment, sizeof(StructIn->sDepartment));										//4.0.22.511 74797
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OdometerExt2", &StructIn->sOdometerExt2, sizeof(StructIn->sOdometerExt2));								//4.0.22.511 74797
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "License", StructIn->sLicense, sizeof(StructIn->sLicense));												//4.0.22.511 74797
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CustomerReferenceExt", StructIn->sCustomerReferenceExt, sizeof(StructIn->sCustomerReferenceExt));		//4.0.22.511 74797
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CustomerIDExt", StructIn->sCustomerIDExt, sizeof(StructIn->sCustomerIDExt));							//4.0.22.511 74797
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DriverId_MSB2", StructIn->sDriverId_MSB2, sizeof(StructIn->sDriverId_MSB2)); //4.0.23.370 117473	//4.0.24.90
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ZeroCompleteStatus", StructIn->sZeroCompleteStatus, sizeof(StructIn->sZeroCompleteStatus)); //  216043
}

//4.0.18.501
void CXmlConvertor::CreateXMLFromStructCardSaleExtraData5(char * pszXMLOut, CARD_SALE_EXTRA_DATA5_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	//4.0.23.509 97360 start
	char tempTotalStart[20] = {0};
	char tempTotalEnd[20] = {0};

	//connecting the values of sPreDispensingVolumeHigh and sPreDispensingVolumeLow to one.
	memcpy(tempTotalStart, StructIn->sPreDispensingVolumeHigh, sizeof(StructIn->sPreDispensingVolumeHigh));
	memcpy(tempTotalStart + sizeof(StructIn->sPreDispensingVolumeHigh), StructIn->sPreDispensingVolumeLow, sizeof(StructIn->sPreDispensingVolumeLow));

	memcpy(tempTotalEnd, StructIn->sPostDispensingVolumeHigh, sizeof(StructIn->sPostDispensingVolumeHigh));
	memcpy(tempTotalEnd+ sizeof(StructIn->sPostDispensingVolumeHigh), StructIn->sPostDispensingVolumeLow, sizeof(StructIn->sPostDispensingVolumeLow));
	//4.0.23.509 97360 end

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "EncryptData", StructIn->sEncryptData, sizeof(StructIn->sEncryptData));		//4.0.22.511 71868
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "HashedData", StructIn->sHashedData, sizeof(StructIn->sHashedData));			//4.0.22.511 71868
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "EncryptedAccount3", StructIn->sEncryptedAccount3, sizeof(StructIn->sEncryptedAccount3));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PercentOffDiscount", StructIn->sPercentOffDiscount, sizeof(StructIn->sPercentOffDiscount));	 //4.0.23.21 75234
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FuelTotalTicketDiscount", StructIn->sFuelTotalTicketDiscount, sizeof(StructIn->sFuelTotalTicketDiscount));//4.0.23.21 75234
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PreDispensingVolume", (unsigned char *)tempTotalStart, sizeof(tempTotalStart));	//4.0.23.509 97360
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PostDispensingVolume", (unsigned char *)tempTotalEnd, sizeof(tempTotalEnd));	//4.0.23.509 97360
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GRTotalDiscountCarWash", StructIn->sGRTotalDiscountCarWash, sizeof(StructIn->sGRTotalDiscountCarWash));		//4.0.24.60  105269
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GRTotalDiscountFuelItem", StructIn->sGRTotalDiscountFuelItem, sizeof(StructIn->sGRTotalDiscountFuelItem));	//4.0.24.60  105269
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AttendantTag", StructIn->sAttendantTag, sizeof(StructIn->sAttendantTag));										//4.0.25.501 TD 137509
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OfflineForceAcc", StructIn->sOfflineForceAcc, sizeof(StructIn->sOfflineForceAcc));							//4.0.26.501 TD 143919
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RestrictPoints", &StructIn->sRestrictPoints, sizeof(StructIn->sRestrictPoints));	//4.0.26.507

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ChipLocalPromptsMandated", StructIn->sChipLocalPromptsMandated, sizeof(StructIn->sChipLocalPromptsMandated));		      //4.0.27.30 Arval start
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ChipLocalValidationMandated", StructIn->sChipLocalValidationMandated, sizeof(StructIn->sChipLocalValidationMandated));  //4.0.27.30
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ChipCarRegistration", StructIn->sChipCarRegistration, sizeof(StructIn->sChipCarRegistration));						  //4.0.27.30
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ChipItemCode", StructIn->sChipItemCode, sizeof(StructIn->sChipItemCode));												  //4.0.27.30
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ChipFillVolume", StructIn->sChipFillVolume, sizeof(StructIn->sChipFillVolume));										  //4.0.27.30 Arval end

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TrsFlowRate", StructIn->sTrsFlowRate, sizeof(StructIn->sTrsFlowRate));												//4.0.27.40 TD 165585
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AverageFlowRate", StructIn->sAverageFlowRate, sizeof(StructIn->sAverageFlowRate));									//4.0.27.40 TD 165585
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RatesFromDevice", &StructIn->sRatesFromDevice, sizeof(StructIn->sRatesFromDevice));									//4.0.27.40 TD 165585

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CompleteRemainingBalance", StructIn->sCompleteRemainingBalance, sizeof(StructIn->sCompleteRemainingBalance));	//Egift
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OverFillAmount", StructIn->sOverFillAmount, sizeof(StructIn->sOverFillAmount));								//Egift
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CompleteResponseCode", StructIn->sCompleteResponseCode, sizeof(StructIn->sCompleteResponseCode));				//Egift
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PreAuthResponseCode", StructIn->sPreAuthResponseCode, sizeof(StructIn->sPreAuthResponseCode));				//Egift //4.0.27.44


	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ValidationResult", StructIn->sValidationResult, sizeof(StructIn->sValidationResult));	//4.0.27.43	Arval
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OlaRequestAmount", StructIn->sOlaRequestAmount, sizeof(StructIn->sOlaRequestAmount));	//4.0.27.43	Arval

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ParamResultExt", StructIn->sParamResultExt, sizeof(StructIn->sParamResultExt));		//4.0.28.502 255727
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyTenderAmount", StructIn->sLoyaltyTenderAmount, sizeof(StructIn->sLoyaltyTenderAmount));	//4.0.28.503 - TD 247268
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ScreenMsgNumber", StructIn->sScreenMsgNumber, sizeof(StructIn->sScreenMsgNumber));	//4.0.31.0
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PumpAutoStart", &StructIn->sPumpAutoStart, sizeof(StructIn->sPumpAutoStart));//4.0.23.1461 323583
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ClubId", StructIn->byClubId, sizeof(StructIn->byClubId));	//332329
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DedicatedFileName", StructIn->sDedicatedFileName, sizeof(StructIn->sDedicatedFileName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashDiscountPerGallon", StructIn->sCarWashDiscountPerGallon, sizeof(StructIn->sCarWashDiscountPerGallon));		//365865
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthNumberEx", StructIn->sAuthNumberEx, sizeof(StructIn->sAuthNumberEx));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyAutoArm", &StructIn->sLoyaltyAutoArm, sizeof(StructIn->sLoyaltyAutoArm));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PriceProtectionUsed", &StructIn->sPriceProtectionUsed, sizeof(StructIn->sPriceProtectionUsed));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LockingKey",			StructIn->sLockingKey,			sizeof(StructIn->sLockingKey));				// CR 425758 

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ExtraEMVDataReq", &StructIn->ExtraEMVDataReq, sizeof(StructIn->ExtraEMVDataReq));				//  [18/5/2017 MaximB]
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ExtraEMVData", StructIn->sExtraEMVData, sizeof(StructIn->sExtraEMVData));				//  [18/5/2017 MaximB]

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "NoMatchingAID",		&StructIn->sNoMatchingAID,		sizeof(StructIn->sNoMatchingAID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ChipMalfunc",			&StructIn->sChipMalfunc,		sizeof(StructIn->sChipMalfunc));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "UserCancel",			&StructIn->sUserCancel,			sizeof(StructIn->sUserCancel));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AuthorisationResponseCode", StructIn->sAuthorisationResponseCode,	sizeof(StructIn->sAuthorisationResponseCode));	 //  [7/7/2014 MaximB]
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "VAT_RateMsb", &StructIn->sVAT_RateMSB, sizeof(StructIn->sVAT_RateMSB));  //4.0.31.500  TD 404911
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmt1", StructIn->sTaxAmt1, sizeof(StructIn->sTaxAmt1)); // TD 404911
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyPointsBalance", StructIn->sLoyaltySummaryPoints, sizeof(StructIn->sLoyaltySummaryPoints));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IssuerCodeTableIndex", StructIn->sIssuerCodeTableIndex, sizeof(StructIn->sIssuerCodeTableIndex));

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PinPadCapabilities", StructIn->sPinPadCapabilities, sizeof(StructIn->sPinPadCapabilities));
}

/*******************************************************************
 Description:		Convert a struct to xml
 Returns:			BOOL l_bReturn - FALSE\TRUE
 Parameters:   		1.pszXMLOut - this string will hold the xml.
					2.vpStructIn - the struct that we need to convert.
					3.lXmlFlags - hold the sturct type

 WHO				WHEN                    WHAT
--------------------------------------------------------------------
 Ariel			15/12/2006   12:02			Start
********************************************************************/
BOOL CXmlConvertor::CreateXml(char * pszXMLOut, void * vpStructIn, __int64 lXmlFlags)
{
	BOOL			bRetVal = TRUE;
	CXMLInterface	xmlInitDoc;
	CXMLPointers	pCurrElement;
	CXMLPointers	pChildNode , pChildNode2 , pChildNode3 , pChildNode4, pChildNode5, pCustomerInfoNode, pChildNode6;
	CComBSTR		bstrXML;
	char tmp [50000];

	bRetVal = xmlInitDoc.InitDoc();
	if (!bRetVal)
		return bRetVal;

	//create the root and append it to the XML.
	xmlInitDoc.CreateElement("FuelTransactionData", &pCurrElement);
	xmlInitDoc.AppendRootElement(&pCurrElement);
	{
		CARD_SALE_ALL3 StructIn;
		memset(&StructIn, 0x00, sizeof(StructIn));
		memcpy(&StructIn, vpStructIn, sizeof(StructIn));

	}


	//TD 38654
	if ((lXmlFlags & CARD_SALE_ALL3_FLAG) || (lXmlFlags & CARD_SALE_ALL2_FLAG) || (lXmlFlags & CARD_SALE_ALL_FLAG) ||
		(lXmlFlags & CARD_SALE_EXTRA_DATA3_FLAG) || (lXmlFlags & CARD_SALE_EXTRA_DATA2_FLAG) || (lXmlFlags & CARD_SALE_EXTRA_DATA_FLAG) ||
		(lXmlFlags & CARD_SALE_DATA_FLAG) || (lXmlFlags & GRADE_LIMITS_FLAG) || (lXmlFlags & DISCNT_FLAG) || (lXmlFlags & SALE_FLAG) ||
		(lXmlFlags & TAG_OLA_REC9_FLAG) || (lXmlFlags & TAG_OLA_REC9_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC8_FLAG) || (lXmlFlags & TAG_OLA_REC8_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC7_FLAG) || (lXmlFlags & TAG_OLA_REC7_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC6_FLAG) || (lXmlFlags & TAG_OLA_REC6_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC5_FLAG) || (lXmlFlags & TAG_OLA_REC5_EX_FLAG) || (lXmlFlags & TAG_OLA_REC4_FLAG) || (lXmlFlags & TAG_OLA_REC4_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC3_FLAG) || (lXmlFlags & TAG_OLA_REC3_EX_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG) ||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG) ||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG) ||
		(lXmlFlags & TAX_DATA_SUMMARY_FLAG) || (lXmlFlags & CARD_SALE_EXTRA_DATA4_PLUS_FLAG) || (lXmlFlags & RECEIPT_DATA4_FLAG) || (lXmlFlags & RECEIPT_DATA4_EX_FLAG) ||
		(lXmlFlags & CARD_SALE_EXTRA4_FLAG) || (lXmlFlags & DALLAS_KEYS_INFO_FLAG) || (lXmlFlags & TAG_OLA_REC10_FLAG) || (lXmlFlags & CARD_SALE_EXTRA4_FLAG) ||
		(lXmlFlags & CARD_SALE_ALL_CHIP_EXTRA_FLAG) || (lXmlFlags & TAG_OLA_REC11_FLAG) || (lXmlFlags & TAG_OLA_REC12_FLAG)|| (lXmlFlags & TAG_OLA_REC13_FLAG)) //4.0.20.20 //4.0.22.511 71868
	{
		//Create CardDataNode
		xmlInitDoc.CreateElement("CardData", &pChildNode);
		xmlInitDoc.AppendChild(&pChildNode, &pCurrElement);

		if (lXmlFlags & CARD_SALE_ALL3_FLAG)
		{
			CARD_SALE_ALL3 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleAll3(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		else if(lXmlFlags & CARD_SALE_ALL_CHIP_EXTRA_FLAG) //4.0.20.20
		{
			//create card sale all 3
			CreateXMLFromStructCardSaleAll3(pszXMLOut, &((CARD_SALE_ALL_CHIP_EXTRA_DATA*) vpStructIn)->cCardSaleAll3, &xmlInitDoc, &pChildNode);
			//create chip extra data
			xmlInitDoc.CreateElement("ChipCardExtraData", &pChildNode);
		    xmlInitDoc.AppendChild(&pChildNode, &pCurrElement);
			CreateXMLFromChipExtraData(pszXMLOut, &((CARD_SALE_ALL_CHIP_EXTRA_DATA*) vpStructIn)->cChipExtraData, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & CARD_SALE_ALL2_FLAG)
		{
			CARD_SALE_ALL2 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleAll2(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & CARD_SALE_ALL_FLAG)
		{
			CARD_SALE_ALL StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleAll(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & CARD_SALE_EXTRA_DATA3_FLAG)
		{
			CARD_SALE_EXTRA_DATA3_ StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & CARD_SALE_EXTRA_DATA2_FLAG)
		{
			CARD_SALE_EXTRA_DATA2_ StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & CARD_SALE_EXTRA_DATA_FLAG)
		{
			CARD_SALE_EXTRA_DATA StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & CARD_SALE_DATA_FLAG)
		{
			CARD_SALE_DATA StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & GRADE_LIMITS_FLAG)
		{
			GRADE_LIMITS StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructGradeLimitsWithOutNodes(pszXMLOut, &StructIn, 0, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & DISCNT_FLAG)
		{
			DISCNT StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructDiscntWithOutNodes(pszXMLOut, &StructIn, 0, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & SALE_FLAG)
		{
			SALE StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructSaleWithOutNodes(pszXMLOut, &StructIn, 0, &xmlInitDoc, &pChildNode);
		}
		else if( (lXmlFlags & TAG_OLA_REC11_FLAG) )	//4.0.20.501 TD 60236
		{
			TAG_OLA_REC11 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.tagOlaRec10.lIndexNumber, sizeof(StructIn.tagOlaRec10.lIndexNumber));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.tagOlaRec10.cardData, &xmlInitDoc, &pChildNode, &StructIn.tagOlaRec10.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.tagOlaRec10.extraData, &xmlInitDoc, &pChildNode, &StructIn.tagOlaRec10.extraData3); //4.0.23.509 100885
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.tagOlaRec10.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.tagOlaRec10.extraData3, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData4(pszXMLOut, &StructIn.tagOlaRec10.extraData4, &xmlInitDoc, &pChildNode);

			EndXmlLineWithNull(&xmlInitDoc, &pChildNode, "ReceiptKey", StructIn.outputData.bReceiptKey, sizeof(StructIn.outputData.bReceiptKey));
		}
		else if( (lXmlFlags & TAG_OLA_REC12_FLAG) )	//4.0.22.511 71868
		{
			TAG_OLA_REC12 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.tagOlaRec11.tagOlaRec10.lIndexNumber, sizeof(StructIn.tagOlaRec11.tagOlaRec10.lIndexNumber));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.tagOlaRec11.tagOlaRec10.cardData, &xmlInitDoc, &pChildNode, &StructIn.tagOlaRec11.tagOlaRec10.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.tagOlaRec11.tagOlaRec10.extraData, &xmlInitDoc, &pChildNode, &StructIn.tagOlaRec11.tagOlaRec10.extraData3); //4.0.22.690 244515 //4.0.28.500
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.tagOlaRec11.tagOlaRec10.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.tagOlaRec11.tagOlaRec10.extraData3, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData4(pszXMLOut, &StructIn.tagOlaRec11.tagOlaRec10.extraData4, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData5(pszXMLOut, &StructIn.extraData5, &xmlInitDoc, &pChildNode);

			EndXmlLineWithNull(&xmlInitDoc, &pChildNode, "ReceiptKey", StructIn.tagOlaRec11.outputData.bReceiptKey, sizeof(StructIn.tagOlaRec11.outputData.bReceiptKey));
		}
		else if( (lXmlFlags & TAG_OLA_REC13_FLAG) )	//4.0.22.511 71868
		{
			TAG_OLA_REC13 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement, "IndexNumber", StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber, sizeof(StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData, &xmlInitDoc, &pChildNode, &StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData, &xmlInitDoc, &pChildNode, &StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData3); //4.0.22.690 244515 //4.0.28.500
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData3, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData4(pszXMLOut, &StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData4, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData5(pszXMLOut, &StructIn.tagOlaRec12.extraData5, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData6(pszXMLOut, &StructIn.extraData6, &xmlInitDoc, &pChildNode);

			EndXmlLineWithNull(&xmlInitDoc, &pChildNode, "ReceiptKey", StructIn.tagOlaRec12.tagOlaRec11.outputData.bReceiptKey, sizeof(StructIn.tagOlaRec12.tagOlaRec11.outputData.bReceiptKey));
		}
		else if( (lXmlFlags & TAG_OLA_REC10_FLAG) )	//4.0.17.501
		{
			TAG_OLA_REC10 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode, &StructIn.extraData3);	//4.0.18.670 73612 //4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extraData3, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData4(pszXMLOut, &StructIn.extraData4, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & TAG_OLA_REC9_FLAG)
		{
			TAG_OLA_REC9 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode, &StructIn.extraData3);//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extraData3, &xmlInitDoc, &pChildNode);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC9_EX_FLAG)
		{
			TAG_OLA_REC9_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode, &StructIn.extraData3);//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extraData3, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & TAG_OLA_REC8_FLAG)
		{
			TAG_OLA_REC8 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode, &StructIn.extraData3);//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extraData3, &xmlInitDoc, &pChildNode);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC8_EX_FLAG)
		{
			TAG_OLA_REC8_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode, &StructIn.extraData3);//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extraData3, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & TAG_OLA_REC7_FLAG)
		{
			TAG_OLA_REC7 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode, &StructIn.extraData3);//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extraData3, &xmlInitDoc, &pChildNode);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC7_EX_FLAG)
		{
			TAG_OLA_REC7_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement, "IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode, &StructIn.extraData3);//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extraData3, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & TAG_OLA_REC6_FLAG)
		{
			TAG_OLA_REC6 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode, &StructIn.extraData3);//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extraData3, &xmlInitDoc, &pChildNode);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC6_EX_FLAG)
		{
			TAG_OLA_REC6_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode, &StructIn.extraData3);//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extraData3, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & TAG_OLA_REC5_FLAG)
		{
			TAG_OLA_REC5 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode, &StructIn.extraData3);//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extraData3, &xmlInitDoc, &pChildNode);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC5_EX_FLAG)
		{
			TAG_OLA_REC5_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode, &StructIn.extraData3);//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extraData3, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & TAG_OLA_REC4_FLAG)
		{
			TAG_OLA_REC4 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC4_EX_FLAG)
		{
			TAG_OLA_REC4_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
		}

		else if (lXmlFlags & TAG_OLA_REC3_FLAG)
		{
			TAG_OLA_REC3 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC3_EX_FLAG)
		{
			TAG_OLA_REC3_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			EndXmlLineWithNull(&xmlInitDoc, &pCurrElement,"IndexNumber", StructIn.lIndexNumber, sizeof(StructIn.lIndexNumber)); //4.0.17.500

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cardData, &xmlInitDoc, &pChildNode, &StructIn.extraData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.extraData, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extraData2, &xmlInitDoc, &pChildNode);
		}

		else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cData, &xmlInitDoc, &pChildNode, &StructIn.cExtData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.cExtData, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.cExtData2, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA3 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cData, &xmlInitDoc, &pChildNode, &StructIn.cExtData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.cExtData, &xmlInitDoc, &pChildNode, &StructIn.cExtData3);	//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.cExtData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.cExtData3, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA4 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cData, &xmlInitDoc, &pChildNode, &StructIn.cExtData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.cExtData, &xmlInitDoc, &pChildNode, &StructIn.cExtData3);	//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.cExtData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.cExtData3, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA5 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cData, &xmlInitDoc, &pChildNode, &StructIn.cExtData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.cExtData, &xmlInitDoc, &pChildNode, &StructIn.cExtData3);	//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.cExtData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.cExtData3, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructTaxDataSummaryWithOutNodes(pszXMLOut, &StructIn.cTaxSummary, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA6 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.cData, &xmlInitDoc, &pChildNode, &StructIn.cExtData);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.cExtData, &xmlInitDoc, &pChildNode, &StructIn.cExtData3);	//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.cExtData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.cExtData3, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData4(pszXMLOut, &StructIn.cExtData4, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData5(pszXMLOut, &StructIn.cExtData5, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData6(pszXMLOut, &StructIn.cExtData6, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructTaxDataSummaryWithOutNodes(pszXMLOut, &StructIn.cTaxSummary, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & TAX_DATA_SUMMARY_FLAG)
		{
			TAX_DATA_SUMMARY StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTaxDataSummaryWithOutNodes(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & CARD_SALE_EXTRA_DATA4_PLUS_FLAG)
		{
			CARD_SALE_EXTRA_DATA4_PLUS StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleAll3(pszXMLOut, &StructIn.CardSaleAll3, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructOlaExtraDataWithOutNodes(pszXMLOut, &StructIn.DebitExtraData, &xmlInitDoc, &pChildNode); //4.0.24.60 109103
			// 4.0.15.500
			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.cReceiptExtraInfo, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & RECEIPT_DATA4_FLAG)
		{
			RECEIPT_DATA4 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.CardSaleData, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.CardSaleExtraData, &xmlInitDoc, &pChildNode, &StructIn.extData3);	//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extData3, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & RECEIPT_DATA4_EX_FLAG)	//TD 38654
		{
			RECEIPT_DATA4_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn.CardSaleData, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn.CardSaleExtraData, &xmlInitDoc, &pChildNode, &StructIn.extData3);	//4.0.18.670 73612 4.0.22.502
			CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn.extData2, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn.extData3, &xmlInitDoc, &pChildNode);
			CreateXMLFromStructCardSaleExtraData4(pszXMLOut, &StructIn.extData4, &xmlInitDoc, &pChildNode); //4.0.27.45 - Egift
			CreateXMLFromStructCardSaleExtraData5(pszXMLOut, &StructIn.extData5, &xmlInitDoc, &pChildNode); //4.0.27.45 - Egift
			CreateXMLFromStructCardSaleExtraData6(pszXMLOut, &StructIn.extData6, &xmlInitDoc, &pChildNode); //4.0.27.45 - Egift

		}
		else if (lXmlFlags & CARD_SALE_EXTRA4_FLAG)
		{
			CARD_SALE_EXTRA_DATA4_ StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleExtraData4(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & CARD_SALE_EXTRA5_FLAG)	//4.0.18.501
		{
			CARD_SALE_EXTRA_DATA5_ StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleExtraData5(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & CARD_SALE_EXTRA6_FLAG)	//4.0.18.501
		{
			CARD_SALE_EXTRA_DATA6_ StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructCardSaleExtraData6(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & DALLAS_KEYS_INFO_FLAG)
		{
			DALLAS_KEYS_INFO StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructDallasKeysInfo(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
	}
	//TD 38654
	if ((lXmlFlags & TAG_PUMP_TRANSACT2_FLAG) || (lXmlFlags & TAG_PUMP_TRANSACT5_FLAG) ||
	(lXmlFlags & TAG_OLA_REC9_FLAG) || (lXmlFlags & TAG_OLA_REC9_EX_FLAG) ||
	(lXmlFlags & TAG_OLA_REC8_FLAG) || (lXmlFlags & TAG_OLA_REC8_EX_FLAG) ||
	(lXmlFlags & TAG_OLA_REC7_FLAG) || (lXmlFlags & TAG_OLA_REC7_EX_FLAG) ||
	(lXmlFlags & TAG_OLA_REC6_FLAG) || (lXmlFlags & TAG_OLA_REC6_EX_FLAG) ||
	(lXmlFlags & TAG_OLA_REC5_FLAG) || (lXmlFlags & TAG_OLA_REC5_EX_FLAG) ||
	(lXmlFlags & TAG_OLA_REC4_FLAG) || (lXmlFlags & TAG_OLA_REC4_EX_FLAG) ||
	(lXmlFlags & TAG_OLA_REC3_FLAG) || (lXmlFlags & TAG_OLA_REC3_EX_FLAG) ||
	(lXmlFlags & RECEIPT_DATA4_FLAG) || (lXmlFlags & RECEIPT_DATA4_FLAG)  ||
	(lXmlFlags & TAG_OLA_REC10_FLAG) || (lXmlFlags & TAG_OLA_REC11_FLAG)  ||
	(lXmlFlags & TAG_OLA_REC12_FLAG)  || (lXmlFlags & TAG_OLA_REC13_FLAG))	//4.0.22.511 71868
	{
		xmlInitDoc.CreateElement("Transaction", &pChildNode2);
		xmlInitDoc.AppendChild(&pChildNode2, &pCurrElement);

		if (lXmlFlags & TAG_OLA_REC13_FLAG) //4.0.22.511 71868
		{
			TAG_OLA_REC13 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes(pszXMLOut, &StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs, &xmlInitDoc, &pChildNode2);
		}

		if (lXmlFlags & TAG_OLA_REC12_FLAG) //4.0.22.511 71868
		{
			TAG_OLA_REC12 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes (pszXMLOut, &StructIn.tagOlaRec11.tagOlaRec10.tagTrs, &xmlInitDoc, &pChildNode2);
		}

		if (lXmlFlags & TAG_OLA_REC11_FLAG) //4.0.17.501
		{
			TAG_OLA_REC11 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes (pszXMLOut, &StructIn.tagOlaRec10.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		if (lXmlFlags & TAG_OLA_REC10_FLAG) //4.0.17.501
		{
			TAG_OLA_REC10 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes (pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}

		if (lXmlFlags & TAG_OLA_REC9_FLAG)
		{
			TAG_OLA_REC9 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact2WithOutNodes (pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		//TD 38654
		if (lXmlFlags & TAG_OLA_REC9_EX_FLAG)
		{
			TAG_OLA_REC9_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes (pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		else if (lXmlFlags & TAG_OLA_REC8_FLAG)
		{
			TAG_OLA_REC8 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact2WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC8_EX_FLAG)
		{
			TAG_OLA_REC8_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		else if (lXmlFlags & TAG_OLA_REC7_FLAG)
		{
			TAG_OLA_REC7 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact2WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC7_EX_FLAG)
		{
			TAG_OLA_REC7_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		else if (lXmlFlags & TAG_OLA_REC6_FLAG)
		{
			TAG_OLA_REC6 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact2WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC6_EX_FLAG)
		{
			TAG_OLA_REC6_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		else if (lXmlFlags & TAG_OLA_REC5_FLAG)
		{
			TAG_OLA_REC5 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact2WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC5_EX_FLAG)
		{
			TAG_OLA_REC5_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		else if (lXmlFlags & TAG_OLA_REC4_FLAG)
		{
			TAG_OLA_REC4 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact2WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC4_EX_FLAG)
		{
			TAG_OLA_REC4_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		else if (lXmlFlags & TAG_OLA_REC3_FLAG)
		{
			TAG_OLA_REC3 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact2WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC3_EX_FLAG)
		{
			TAG_OLA_REC3_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes(pszXMLOut, &StructIn.tagTrs, &xmlInitDoc, &pChildNode2);
		}

		if (lXmlFlags & TAG_PUMP_TRANSACT2_FLAG)
		{
			TAG_PUMP_TRANSACT2 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact2WithOutNodes (pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode2);
		}

		if (lXmlFlags & TAG_PUMP_TRANSACT5_FLAG) //TD 38654
		{
			TAG_PUMP_TRANSACT5 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes (pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode2);
		}
		else if (lXmlFlags & RECEIPT_DATA4_FLAG)
		{
			RECEIPT_DATA4 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact2WithOutNodes(pszXMLOut, &StructIn.PumpTransact, &xmlInitDoc, &pChildNode);
		}
		else if (lXmlFlags & RECEIPT_DATA4_EX_FLAG)
		{
			RECEIPT_DATA4_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagPumpTransact5WithOutNodes(pszXMLOut, &StructIn.PumpTransact, &xmlInitDoc, &pChildNode);
		}
	}
	//TD 38654
	if ((lXmlFlags & BAP_ITEM_FOR_RECEIPT_FLAG) || (lXmlFlags & BAP_ITEM_FOR_RECEIPT2_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG) ||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG) ||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG) ||
		(lXmlFlags & TAG_OLA_REC9_FLAG) || (lXmlFlags & TAG_OLA_REC9_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC7_FLAG) || (lXmlFlags & TAG_OLA_REC7_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC6_FLAG) || (lXmlFlags & TAG_OLA_REC6_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC4_FLAG) || (lXmlFlags & TAG_OLA_REC4_EX_FLAG) ||
		(lXmlFlags & BAP_ALL_ITEM_FOR_RECEIPT_FLAG) || (lXmlFlags & BAP_ALL_ITEM_FOR_RECEIPT2_FLAG) ||
		(lXmlFlags & BAP_FUEL_ITEM_FLAG) || (lXmlFlags & BAP_FUEL_ITEM_EX_FLAG) || (lXmlFlags & TAG_OLA_REC10_FLAG) ||
		(lXmlFlags & TAG_OLA_REC11_FLAG) || (lXmlFlags & TAG_OLA_REC12_FLAG)|| (lXmlFlags & TAG_OLA_REC13_FLAG))	//4.0.22.511 71868
	{
		xmlInitDoc.CreateElement("BuyAtPump", &pChildNode3);
		xmlInitDoc.AppendChild(&pChildNode3, &pCurrElement);
		if (lXmlFlags & BAP_ITEM_FOR_RECEIPT_FLAG)
		{
			BAP_ITEM_FOR_RECEIPT StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpItemForReceiptWithOutNodes(pszXMLOut, &StructIn, 0, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & BAP_ITEM_FOR_RECEIPT2_FLAG)
		{
			BAP_ITEM_FOR_RECEIPT2 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpItemForReceipt2WithOutNodes(pszXMLOut, &StructIn, 0, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & BAP_ALL_ITEM_FOR_RECEIPT_FLAG)
		{
			BAP_ALL_ITEMS_FOR_RECEIPT StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllItemsForReceiptWithOutNodes(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & BAP_ALL_ITEM_FOR_RECEIPT2_FLAG)
		{
			BAP_ALL_ITEMS_FOR_RECEIPT2 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllItemsForReceipt2WithOutNodes(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode3);
		}

		if (lXmlFlags & BAP_FUEL_ITEM_FLAG)
		{
			BAP_FUEL_ITEM StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpFuelItem(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode3);
		}

		if (lXmlFlags & BAP_FUEL_ITEM_EX_FLAG) //TD 38654
		{
			BAP_FUEL_ITEM_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpFuelItem(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode3);
		}

		if (lXmlFlags & TAG_OLA_REC6_FLAG)
		{
			TAG_OLA_REC6 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllItemsForReceiptWithOutNodes(pszXMLOut, &StructIn.Items, &xmlInitDoc, &pChildNode3);
		}
		//TD 38654
		if (lXmlFlags & TAG_OLA_REC6_EX_FLAG)
		{
			TAG_OLA_REC6_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllItemsForReceiptWithOutNodes(pszXMLOut, &StructIn.Items, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_OLA_REC4_FLAG)
		{
			TAG_OLA_REC4 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllItemsForReceiptWithOutNodes(pszXMLOut, &StructIn.Items, &xmlInitDoc, &pChildNode3);
		}
		//TD 38654
		if (lXmlFlags & TAG_OLA_REC4_EX_FLAG)
		{
			TAG_OLA_REC4_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllItemsForReceiptWithOutNodes(pszXMLOut, &StructIn.Items, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllItemsForReceiptWithOutNodes(pszXMLOut, &StructIn.cBuyAtPumpDataForReceipt, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA3 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllItemsForReceiptWithOutNodes(pszXMLOut, &StructIn.cBuyAtPumpDataForReceipt, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA4 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllTwoStructs(pszXMLOut, &StructIn.cBuyAtPumpDataForReceipt, &StructIn.cBuyAtPumpDataForReceipt2, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA5 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllTwoStructs(pszXMLOut, &StructIn.cBuyAtPumpDataForReceipt, &StructIn.cBuyAtPumpDataForReceipt2, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA6 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllTwoStructs(pszXMLOut, &StructIn.cBuyAtPumpDataForReceipt, &StructIn.cBuyAtPumpDataForReceipt2, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_OLA_REC7_FLAG)
		{
			TAG_OLA_REC7 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllTwoStructs(pszXMLOut, &StructIn.Items, &StructIn.Items2, &xmlInitDoc, &pChildNode3);
		}
		//TD 38654
		if (lXmlFlags & TAG_OLA_REC7_EX_FLAG)
		{
			TAG_OLA_REC7_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllTwoStructs(pszXMLOut, &StructIn.Items, &StructIn.Items2, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_OLA_REC9_FLAG)
		{
			TAG_OLA_REC9 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllTwoStructs(pszXMLOut, &StructIn.Items, &StructIn.Items2, &xmlInitDoc, &pChildNode3);
		}
		//TD 38654
		if (lXmlFlags & TAG_OLA_REC9_EX_FLAG)
		{
			TAG_OLA_REC9_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllTwoStructs(pszXMLOut, &StructIn.Items, &StructIn.Items2, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_OLA_REC10_FLAG)
		{
			TAG_OLA_REC10 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllTwoStructs(pszXMLOut, &StructIn.Items, &StructIn.Items2, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_OLA_REC11_FLAG)
		{
			TAG_OLA_REC11 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllTwoStructs(pszXMLOut, &StructIn.tagOlaRec10.Items, &StructIn.tagOlaRec10.Items2, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_OLA_REC12_FLAG) //4.0.22.511 71868
		{
			TAG_OLA_REC12 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllTwoStructs(pszXMLOut, &StructIn.tagOlaRec11.tagOlaRec10.Items, &StructIn.tagOlaRec11.tagOlaRec10.Items2, &xmlInitDoc, &pChildNode3);
		}
		if (lXmlFlags & TAG_OLA_REC13_FLAG) //4.0.22.511 71868
		{
			TAG_OLA_REC13 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructBuyAtPumpAllTwoStructs(pszXMLOut, &StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.Items, &StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.Items2, &xmlInitDoc, &pChildNode3);
		}
	}
	//TD 38654
	if ((lXmlFlags & TAG_GENERAL_INFO_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG) ||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG) ||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG) ||
		(lXmlFlags & RECEIPT_EXTRA_INFO_FLAG) ||
		(lXmlFlags & TAG_OLA_REC9_FLAG) || (lXmlFlags & TAG_OLA_REC9_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC8_FLAG) || (lXmlFlags & TAG_OLA_REC8_EX_FLAG) ||
		(lXmlFlags & CARD_SALE_EXTRA_DATA4_PLUS_FLAG) ||
		(lXmlFlags & RECEIPT_DATA4_FLAG) || (lXmlFlags & RECEIPT_DATA4_EX_FLAG) || (lXmlFlags & TAG_OLA_REC10_FLAG) ||
		(lXmlFlags & TAG_OLA_REC11_FLAG) || (lXmlFlags & TAG_OLA_REC12_FLAG)|| (lXmlFlags & TAG_OLA_REC13_FLAG)) //4.0.22.511 71868
	{
		xmlInitDoc.CreateElement("Receipt", &pChildNode4);
		xmlInitDoc.AppendChild(&pChildNode4, &pCurrElement);

		if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA6 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagGeneralInfoWithOutNodes(pszXMLOut, &StructIn.cTagGeneralInfo, &xmlInitDoc, &pChildNode4);

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.cReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA5 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagGeneralInfoWithOutNodes(pszXMLOut, &StructIn.cTagGeneralInfo, &xmlInitDoc, &pChildNode4);

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.cReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA4 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagGeneralInfoWithOutNodes(pszXMLOut, &StructIn.cTagGeneralInfo, &xmlInitDoc, &pChildNode4);
		}
		else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA3 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagGeneralInfoWithOutNodes(pszXMLOut, &StructIn.cTagGeneralInfo, &xmlInitDoc, &pChildNode3);
		}
		else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG)
		{
			TAG_RECEIPT_GENERAL_DATA StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagGeneralInfoWithOutNodes(pszXMLOut, &StructIn.cTagGeneralInfo, &xmlInitDoc, &pChildNode4);
		}
		else if (lXmlFlags & TAG_GENERAL_INFO_FLAG)
		{
			TAG_GENERAL_INFO StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructTagGeneralInfoWithOutNodes(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode4);
		}
		else if (lXmlFlags & RECEIPT_EXTRA_INFO_FLAG)
		{
			RECEIPT_EXTRA_INFO StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode5);
		}
		else if (lXmlFlags & TAG_OLA_REC11_FLAG) //4.0.20.507
		{
			TAG_OLA_REC11 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.tagOlaRec10.ReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		else if (lXmlFlags & TAG_OLA_REC12_FLAG) //4.0.22.511 71868
		{
			TAG_OLA_REC12 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.tagOlaRec11.tagOlaRec10.ReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		else if (lXmlFlags & TAG_OLA_REC13_FLAG) //4.0.22.511 71868
		{
			TAG_OLA_REC13 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.tagOlaRec12.tagOlaRec11.tagOlaRec10.ReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		else if (lXmlFlags & TAG_OLA_REC10_FLAG) //4.0.17.501
		{
			TAG_OLA_REC10 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.ReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		else if (lXmlFlags & TAG_OLA_REC9_FLAG)
		{
			TAG_OLA_REC9 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.ReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC9_EX_FLAG)
		{
			TAG_OLA_REC9_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.ReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		else if (lXmlFlags & TAG_OLA_REC8_FLAG)
		{
			TAG_OLA_REC8 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.ReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		//TD 38654
		else if (lXmlFlags & TAG_OLA_REC8_EX_FLAG)
		{
			TAG_OLA_REC8_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.ReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		else if (lXmlFlags & CARD_SALE_EXTRA_DATA4_PLUS_FLAG)
		{
			CARD_SALE_EXTRA_DATA4_PLUS StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.cReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		else if (lXmlFlags & RECEIPT_DATA4_FLAG)
		{
			RECEIPT_DATA4 StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.cReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
		else if (lXmlFlags & RECEIPT_DATA4_EX_FLAG)
		{
			RECEIPT_DATA4_EX StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			xmlInitDoc.CreateElement("ExtraInfo", &pChildNode5);
			xmlInitDoc.AppendChild(&pChildNode5, &pChildNode4);

			CreateXMLFromStructReceiptExtraInfoWithOutNodes(pszXMLOut, &StructIn.cReceiptExtraInfo, &xmlInitDoc, &pChildNode5);
		}
	}

	xmlInitDoc.GetXmlString(&bstrXML);
	int iLen = WideCharToMultiByte(CP_ACP, 0, bstrXML, -1, tmp, sizeof(tmp), NULL, NULL);
	strcpy(pszXMLOut, tmp);

#ifdef DEBUG
	{
		CString sFileNameAndPath = _Module.m_server.m_cLogger.GetDefaultLogPath() + "\\FuelTransactionData.xml";
		xmlInitDoc.SaveToFile(sFileNameAndPath.GetBuffer());
	}
#endif 

	xmlInitDoc.CloseDoc();
	return bRetVal;
}

BOOL CXmlConvertor::CreateExtraDataXml(char * pszXMLOut, void * vpStructIn, __int64 lXmlFlags)
{
	BOOL			bRetVal = TRUE;
	CXMLInterface	xmlInitDoc;
	CXMLPointers	pCurrElement;
	CXMLPointers	pChildNode, pDiscoutnArrNode, pChildNode2, pFlowRateNode, pChildNode5, pCustomerInfoNode, pChildNode6;
	CComBSTR		bstrXML;
	CXMLPointers pCustomerInfo, pTriggerProm;
	CXMLPointers pLoyaltyCardDetalis;

	char tmp[50000];

	bRetVal = xmlInitDoc.InitDoc();
	if (!bRetVal)
		return bRetVal;

	//create the root and append it to the XML.
	xmlInitDoc.CreateElement("ExtraInfo", &pCurrElement);
	xmlInitDoc.AppendRootElement(&pCurrElement);
	
	CXmlParser xmlParse;
	TAG_EXTRA_INFO StructIn;
	memset(&StructIn, 0x00, sizeof(StructIn));
	memcpy(&StructIn, vpStructIn, sizeof(StructIn));

	xmlInitDoc.CreateElement("ExtraInfo1", &pChildNode);
	xmlInitDoc.AppendChild(&pChildNode, &pCurrElement);
	
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "ResultCode", StructIn.sLoyaltyServerResultCode, sizeof(StructIn.sLoyaltyServerResultCode));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc,&pChildNode, "LoyalId", StructIn.sLoyalId, sizeof(StructIn.sLoyalId));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "TransactionId", StructIn.sTransactionId, sizeof(StructIn.sTransactionId)); 
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "PromotionNumber", StructIn.sPromotionNumber, sizeof(StructIn.sPromotionNumber));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "AttendantID", StructIn.sAttendantID, sizeof(StructIn.sAttendantID));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "AttendantName", StructIn.sAttendantName, sizeof(StructIn.sAttendantName));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "DiscountType", StructIn.sDiscountType, sizeof(StructIn.sDiscountType));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "PreSetLimit", StructIn.sPreSetLimit, sizeof(StructIn.sPreSetLimit));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "PreSetLimitType", &StructIn.sPreSetLimitType, sizeof(StructIn.sPreSetLimitType));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "CustomerName", StructIn.sCustomerName, sizeof(StructIn.sCustomerName));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "TransactionAmount", StructIn.sTotalBalance, sizeof(StructIn.sTotalBalance));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "BalanceType", &StructIn.sBalanceType, sizeof(StructIn.sBalanceType));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "HostTransId", StructIn.sHostTransId, sizeof(StructIn.sHostTransId));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "LAMFlags", &StructIn.sLAMFlags, sizeof(StructIn.sLAMFlags));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "LAM", StructIn.sLAM, sizeof(StructIn.sLAM));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "CarWashProgram", StructIn.sCarWashProgram, sizeof(StructIn.sCarWashProgram));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "WashCode", StructIn.sWashCode, sizeof(StructIn.sWashCode));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "WashExpire", StructIn.sWashExpire, sizeof(StructIn.sWashExpire));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "CarWashDiscount", StructIn.sCarWashDiscount, sizeof(StructIn.sCarWashDiscount));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "CarWashTax", StructIn.sCarWashTax, sizeof(StructIn.sCarWashTax));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "CarWashAmount", StructIn.sCarWashAmount, sizeof(StructIn.sCarWashAmount));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "OLDSession", StructIn.sOLDSession, sizeof(StructIn.sOLDSession));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "NonDiscountTotalAmount", StructIn.sNonDiscountTotalAmount, sizeof(StructIn.sNonDiscountTotalAmount));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "OriginalUnitPrice", StructIn.sOriginalUnitPrice, sizeof(StructIn.sOriginalUnitPrice));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "TotalDiscount", StructIn.sLoyaltyTotalDiscount, sizeof(StructIn.sLoyaltyTotalDiscount));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pCurrElement, "DeferralAccepted", &StructIn.sDeferralAccepted, sizeof(StructIn.sDeferralAccepted));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "MaxAllowedDiscount", StructIn.sMaxDiscountAllowed, sizeof(StructIn.sMaxDiscountAllowed));  	//RFUEL-3309 Get the maximal discount allowed for the grade 
	// LOYALTY_INFO4
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "PostDispensingVolumeHigh", StructIn.sPostDispensingVolumeHigh, sizeof(StructIn.sPostDispensingVolumeHigh));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "PostDispensingVolumeLow", StructIn.sPostDispensingVolumeLow, sizeof(StructIn.sPostDispensingVolumeLow));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "PreDispensingVolumeHigh", StructIn.sPreDispensingVolumeHigh, sizeof(StructIn.sPreDispensingVolumeHigh));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pChildNode, "PreDispensingVolumeLow", StructIn.sPreDispensingVolumeLow, sizeof(StructIn.sPreDispensingVolumeLow));


	xmlInitDoc.CreateElement("FlowRate", &pFlowRateNode);
	xmlInitDoc.AppendChild(&pFlowRateNode, &pCurrElement);

	//Flow Rate
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pFlowRateNode, "FullVehicleId", StructIn.sFullVehicleId, sizeof(StructIn.sFullVehicleId));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pFlowRateNode, "ANPRRefNumber", StructIn.sANPRRefNumber, sizeof(StructIn.sANPRRefNumber));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pFlowRateNode, "ANPRTrsStatus", StructIn.sANPRTrsStatus, sizeof(StructIn.sANPRTrsStatus));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pFlowRateNode, "TrsFlowRate", StructIn.sTrsFlowRate, sizeof(StructIn.sTrsFlowRate));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pFlowRateNode, "AverageFlowRate", StructIn.sAverageFlowRate, sizeof(StructIn.sAverageFlowRate));
	xmlParse.AddNullTerminatedNode(&xmlInitDoc, &pFlowRateNode, "RatesFromDevice", &StructIn.sRatesFromDevice, sizeof(StructIn.sRatesFromDevice));
	
	CreateXMLFromStructTagSystemTimeWithOutNodes(pszXMLOut, (TAG_SYSTEMTIME*)&StructIn.cVRNDateTime, &xmlInitDoc, &pFlowRateNode);


	xmlInitDoc.GetXmlString(&bstrXML);
	int iLen = WideCharToMultiByte(CP_ACP, 0, bstrXML, -1, tmp, sizeof(tmp), NULL, NULL);
	strcpy(pszXMLOut, tmp);

#ifdef DEBUG
	CString sFileNameAndPath = _Module.m_server.m_cLogger.GetDefaultLogPath() + "\\ExtraInfoPAP2PAK.xml";
	xmlInitDoc.SaveToFile(sFileNameAndPath.GetBuffer());
#endif


	xmlInitDoc.CloseDoc();
	return bRetVal;
}

/*******************************************************************
 Description:		Convert a struct to xml
 Returns:			BOOL l_bReturn - FALSE\TRUE
 Parameters:   		1.pszXMLIn - the xml that we need to convert.
 2.vpStructOut - Empty sturct that we need to fill.
 3.lXmlFlags - hold the sturct type

 WHO				WHEN                    WHAT
 --------------------------------------------------------------------
 Ariel			15/12/2006   12:02			Start
 ********************************************************************/
BOOL CXmlConvertor::CreateStruct(char * pszXMLIn, void * vpStructOut, __int64 lXmlFlags)
{
	BOOL			l_bReturn = TRUE;
	BOOL			l_bFoundCardData, l_bFoundTransaction, l_bFoundBuyAtPump, l_bFoundReceipt, l_bFoundReceiptExtraInfo;
	CXMLInterface	xmlInitDoc;
	CXMLPointers	pChildNode, pChildNode2;
	CString sName;

	//create the xml for the received buffer.
	l_bReturn = xmlInitDoc.InitDoc();
	if(l_bReturn == TRUE)
	{
		l_bReturn = xmlInitDoc.LoadXmlString(pszXMLIn);
		if (!l_bReturn)
		{
			WriteToLog("Failed to load XML !!!", TRUE);
		}
	}

	//check if the xml root name is FuelTransactionData
	if (l_bReturn)
	{

		l_bReturn = xmlInitDoc.GetName(sName);
		if (l_bReturn)
		{
			if (strcmp("FuelTransactionData", sName) && strcmp("AddFuelItem", sName))
			{
				WriteToLog("The XML root name is not FuelTransactionData - Not parsing the xml !!!");
				l_bReturn = FALSE;
			}
			/*else
				WriteToLog("Start parsing the xml.");*/
		}
	}

	//strat parsing the xml
	if (l_bReturn)
	{
		//TD 38654
		if ((lXmlFlags & CARD_SALE_ALL3_FLAG) || (lXmlFlags & CARD_SALE_ALL2_FLAG) || (lXmlFlags & CARD_SALE_ALL_FLAG) ||
		(lXmlFlags & CARD_SALE_EXTRA_DATA3_FLAG) || (lXmlFlags & CARD_SALE_EXTRA_DATA2_FLAG) || (lXmlFlags & CARD_SALE_EXTRA_DATA_FLAG) ||
		(lXmlFlags & CARD_SALE_DATA_FLAG) || (lXmlFlags & GRADE_LIMITS_FLAG) || (lXmlFlags & DISCNT_FLAG) || (lXmlFlags & SALE_FLAG) ||
		(lXmlFlags & TAG_OLA_REC9_FLAG) || (lXmlFlags & TAG_OLA_REC9_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC8_FLAG) || (lXmlFlags & TAG_OLA_REC8_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC7_FLAG) || (lXmlFlags & TAG_OLA_REC7_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC6_FLAG) || (lXmlFlags & TAG_OLA_REC6_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC5_FLAG) || (lXmlFlags & TAG_OLA_REC5_EX_FLAG) || (lXmlFlags & TAG_OLA_REC4_FLAG) || (lXmlFlags & TAG_OLA_REC4_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC3_FLAG) || (lXmlFlags & TAG_OLA_REC3_EX_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG) ||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG)||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG) ||
		(lXmlFlags & TAX_DATA_SUMMARY_FLAG) || (lXmlFlags & CARD_SALE_EXTRA_DATA4_PLUS_FLAG) || (lXmlFlags & RECEIPT_DATA4_FLAG)|| (lXmlFlags & RECEIPT_DATA4_EX_FLAG)||
		(lXmlFlags & CARD_SALE_EXTRA4_FLAG) || (lXmlFlags & DALLAS_KEYS_INFO_FLAG) || (lXmlFlags & DISCOUNT_ATTRIBUTES_INFO_FLAG) || (lXmlFlags & CARD_SALE_EXTRA4_FLAG) ||
		(lXmlFlags & TAG_OLA_REC10_FLAG) || (lXmlFlags & TAG_OLA_REC11_FLAG) || (lXmlFlags & CARD_SALE_ALL_CHIP_EXTRA_FLAG) || (lXmlFlags & DISCOUNT_ATTRIBUTES_EXT_FLAG) ||  //4.0.17.501 //4.0.18.501 //4.0.20.503 66960 //4.0.22.509 38162
		(lXmlFlags & TAG_OLA_REC12_FLAG) || (lXmlFlags & TAG_OLA_REC13_FLAG)) //4.0.22.511 71868
		{
			//check if the xml has a child with the name CardData.
			l_bFoundCardData = xmlInitDoc.SelectSingleElementNode("CardData", &pChildNode, NULL);

			if (l_bFoundCardData)
			{
				if (lXmlFlags & CARD_SALE_ALL3_FLAG)
				{
					CreateStructCardSaleAll3FromXmlWithOutNodes(pszXMLIn,(CARD_SALE_ALL3*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & CARD_SALE_ALL2_FLAG)
				{
					CreateStructCardSaleAll2FromXmlWithOutNodes(pszXMLIn,(CARD_SALE_ALL2*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & CARD_SALE_ALL_FLAG)
				{
					CreateStructCardSaleAllFromXmlWithOutNodes(pszXMLIn,(CARD_SALE_ALL*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & CARD_SALE_EXTRA_DATA3_FLAG)
				{
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn,(CARD_SALE_EXTRA_DATA3_*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & CARD_SALE_EXTRA_DATA2_FLAG)
				{
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn,(CARD_SALE_EXTRA_DATA2_*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & CARD_SALE_EXTRA_DATA_FLAG)
				{
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn,(CARD_SALE_EXTRA_DATA*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & CARD_SALE_DATA_FLAG)
				{
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn,(CARD_SALE_DATA*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & GRADE_LIMITS_FLAG)
				{
					BOOL bExists;
					bExists = xmlInitDoc.SelectSingleElementNode("Limits", &pChildNode2, &pChildNode);
					if (bExists)
						CreateStructAmountGradeLimitFromXmlWithOutNodes(pszXMLIn,(GRADE_LIMITS*)vpStructOut, &pChildNode2, &xmlInitDoc);
				}
				else if (lXmlFlags & DISCNT_FLAG)
				{
					BOOL bExists;
					bExists = xmlInitDoc.SelectSingleElementNode("Discounts", &pChildNode2, &pChildNode);
					if (bExists)
						CreateStructDiscntFromXmlWithOutNodes(pszXMLIn,(DISCNT*)vpStructOut, &pChildNode2, &xmlInitDoc);
				}
				else if (lXmlFlags & SALE_FLAG)
				{
					BOOL bExists;
					bExists = xmlInitDoc.SelectSingleElementNode("Sales", &pChildNode2, &pChildNode);
					if (bExists)
						CreateStructSaleFromXmlWithOutNodes(pszXMLIn,(SALE*)vpStructOut, &pChildNode2, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC13_FLAG) //4.0.22.511 71868
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber", ((TAG_OLA_REC13*)vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData6FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC13*)vpStructOut)->extraData6, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC13*)vpStructOut)->tagOlaRec12.extraData5, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData4FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC13*)vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData4, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC13*)vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC13*)vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC13*)vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC13*)vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData3); //4.0.22.690 244515 //4.0.28.500
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC13*)vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC13*)vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData, &((TAG_OLA_REC13*)vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.extraData2);

					EndXmlLineWithNull(&xmlInitDoc, &pChildNode, "ReceiptKey", ((TAG_OLA_REC13*) vpStructOut)->tagOlaRec12.tagOlaRec11.outputData.bReceiptKey, sizeof(((TAG_OLA_REC13*) vpStructOut)->tagOlaRec12.tagOlaRec11.outputData.bReceiptKey));
				}
				else if (lXmlFlags & TAG_OLA_REC12_FLAG) //4.0.22.511 71868
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.tagOlaRec10.lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC12*)vpStructOut)->extraData5, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData4FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.tagOlaRec10.extraData4, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.tagOlaRec10.extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.tagOlaRec10.extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.tagOlaRec10.extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.tagOlaRec10.extraData3); //4.0.22.690 244515 //4.0.28.500
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.tagOlaRec10.cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.tagOlaRec10.extraData, &((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.tagOlaRec10.extraData2);

					EndXmlLineWithNull(&xmlInitDoc, &pChildNode, "ReceiptKey", ((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.outputData.bReceiptKey, sizeof(((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.outputData.bReceiptKey));
				}
				else if (lXmlFlags & TAG_OLA_REC11_FLAG) //4.0.20.507
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData4FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.extraData4, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.extraData3); //4.0.22.690 244515 //4.0.28.500
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.extraData, &((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.extraData2);

					EndXmlLineWithNull(&xmlInitDoc, &pChildNode, "ReceiptKey", ((TAG_OLA_REC11*)vpStructOut)->outputData.bReceiptKey, sizeof(((TAG_OLA_REC11*)vpStructOut)->outputData.bReceiptKey));
				}
				else if (lXmlFlags & TAG_OLA_REC10_FLAG) //4.0.20.507
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC10*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData4FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC10*)vpStructOut)->extraData4, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC10*)vpStructOut)->extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC10*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC10*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC10*)vpStructOut)->extraData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC10*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC10*)vpStructOut)->extraData, &((TAG_OLA_REC10*)vpStructOut)->extraData2);
				}
				else if (lXmlFlags & TAG_OLA_REC9_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC9*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9*)vpStructOut)->extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC9*)vpStructOut)->extraData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC9*)vpStructOut)->extraData, &((TAG_OLA_REC9*)vpStructOut)->extraData2); //4.0.15.502
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC9_EX_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC9_EX*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9_EX*)vpStructOut)->extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9_EX*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9_EX*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC9_EX*)vpStructOut)->extraData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9_EX*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC9_EX*)vpStructOut)->extraData, &((TAG_OLA_REC9_EX*)vpStructOut)->extraData2);
				}
				else if (lXmlFlags & TAG_OLA_REC8_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC8*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8*)vpStructOut)->extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC8*)vpStructOut)->extraData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC8*)vpStructOut)->extraData, &((TAG_OLA_REC8*)vpStructOut)->extraData2); //4.0.15.502
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC8_EX_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC8_EX*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8_EX*)vpStructOut)->extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8_EX*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8_EX*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC8_EX*)vpStructOut)->extraData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8_EX*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC8_EX*)vpStructOut)->extraData, &((TAG_OLA_REC8_EX*)vpStructOut)->extraData2);
				}
				else if (lXmlFlags & TAG_OLA_REC7_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC7*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC7*)vpStructOut)->extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC7*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC7*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC7*)vpStructOut)->extraData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC7*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC7*)vpStructOut)->extraData, &((TAG_OLA_REC7*)vpStructOut)->extraData2); //4.0.15.502
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC7_EX_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC7_EX*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC7_EX*)vpStructOut)->extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC7_EX*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC7_EX*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC7_EX*)vpStructOut)->extraData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC7_EX*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC7_EX*)vpStructOut)->extraData, &((TAG_OLA_REC7_EX*)vpStructOut)->extraData2);
				}
				else if (lXmlFlags & TAG_OLA_REC6_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC6*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6*)vpStructOut)->extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC6*)vpStructOut)->extraData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC6*)vpStructOut)->extraData, &((TAG_OLA_REC6*)vpStructOut)->extraData2); //4.0.15.502
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC6_EX_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC6_EX*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6_EX*)vpStructOut)->extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6_EX*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6_EX*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC6_EX*)vpStructOut)->extraData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6_EX*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC6_EX*)vpStructOut)->extraData, &((TAG_OLA_REC6_EX*)vpStructOut)->extraData2); //4.0.15.502
				}
				else if (lXmlFlags & TAG_OLA_REC5_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC5*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC5*)vpStructOut)->extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC5*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC5*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC5*)vpStructOut)->extraData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC5*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC5*)vpStructOut)->extraData, &((TAG_OLA_REC5*)vpStructOut)->extraData2); //4.0.15.502
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC5_EX_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC5_EX*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC5_EX*)vpStructOut)->extraData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC5_EX*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC5_EX*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC5_EX*)vpStructOut)->extraData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC5_EX*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC5_EX*)vpStructOut)->extraData, &((TAG_OLA_REC5_EX*)vpStructOut)->extraData2);
				}
				else if (lXmlFlags & TAG_OLA_REC4_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC4*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC4*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC4*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC4*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC4*)vpStructOut)->extraData, &((TAG_OLA_REC4*)vpStructOut)->extraData2); //4.0.15.502
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC4_EX_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC4_EX*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC4_EX*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC4_EX*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC4_EX*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC4_EX*)vpStructOut)->extraData, &((TAG_OLA_REC4_EX*)vpStructOut)->extraData2);
				}
				else if (lXmlFlags & TAG_OLA_REC3_FLAG)
				{
					xmlInitDoc.GetAttributeToByteArrWithConvert("IndexNumber",((TAG_OLA_REC3*)vpStructOut)->lIndexNumber, &pChildNode); //4.0.17.500 //4.0.23.503
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC3*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC3*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC3*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC3*)vpStructOut)->extraData, &((TAG_OLA_REC3*)vpStructOut)->extraData2); //4.0.15.502
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC3_EX_FLAG)
				{
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC3_EX*)vpStructOut)->extraData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC3_EX*)vpStructOut)->extraData, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC3_EX*)vpStructOut)->cardData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC3_EX*)vpStructOut)->extraData, &((TAG_OLA_REC3_EX*)vpStructOut)->extraData2);
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG)
				{
					CreateStructCardSaleExtraData6FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA6*) vpStructOut)->cExtData6, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData5FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA6*) vpStructOut)->cExtData5, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData4FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA6*) vpStructOut)->cExtData4, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA6*) vpStructOut)->cExtData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA6*) vpStructOut)->cExtData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA6*) vpStructOut)->cExtData, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA6*) vpStructOut)->cData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC6*) vpStructOut)->extraData, &((TAG_OLA_REC6*) vpStructOut)->extraData2); //4.0.15.502
					CreateStructTaxDataSummaryFromXmlWithOutNodes (pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA6*) vpStructOut)->cTaxSummary, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG)
				{
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA5*) vpStructOut)->cExtData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA5*) vpStructOut)->cExtData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA5*) vpStructOut)->cExtData, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA5*) vpStructOut)->cData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC5*) vpStructOut)->extraData, &((TAG_OLA_REC5*) vpStructOut)->extraData2); //4.0.15.502
					CreateStructTaxDataSummaryFromXmlWithOutNodes (pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA5*) vpStructOut)->cTaxSummary, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG)
				{
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA4*)vpStructOut)->cExtData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA4*)vpStructOut)->cExtData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA4*)vpStructOut)->cExtData, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA4*)vpStructOut)->cData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC4*)vpStructOut)->extraData, &((TAG_OLA_REC4*)vpStructOut)->extraData2); //4.0.15.502
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG)
				{
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA3*)vpStructOut)->cExtData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA3*)vpStructOut)->cExtData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA3*)vpStructOut)->cExtData, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA3*)vpStructOut)->cData, &pChildNode, &xmlInitDoc, &((TAG_OLA_REC3*)vpStructOut)->extraData, &((TAG_OLA_REC3*)vpStructOut)->extraData2); //4.0.15.502
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG)
				{
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA*)vpStructOut)->cExtData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA*)vpStructOut)->cExtData, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA*)vpStructOut)->cData, &pChildNode, &xmlInitDoc, &((TAG_RECEIPT_GENERAL_DATA*)vpStructOut)->cExtData, &((TAG_RECEIPT_GENERAL_DATA*)vpStructOut)->cExtData2); //4.0.15.502
				}
				else if (lXmlFlags & TAX_DATA_SUMMARY_FLAG)
				{
					CreateStructTaxDataSummaryFromXmlWithOutNodes (pszXMLIn,(TAX_DATA_SUMMARY*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & CARD_SALE_EXTRA_DATA4_PLUS_FLAG)
				{
					CreateStructCardSaleAll3FromXmlWithOutNodes(pszXMLIn, &((CARD_SALE_EXTRA_DATA4_PLUS*)vpStructOut)->CardSaleAll3, &pChildNode, &xmlInitDoc);
					CreateStructOlaExtraDataFromXmlWithOutNodes(pszXMLIn, &((CARD_SALE_EXTRA_DATA4_PLUS*)vpStructOut)->DebitExtraData, &pChildNode, &xmlInitDoc); //4.0.24.60 109103
				}
				else if (lXmlFlags & RECEIPT_DATA4_FLAG)
				{
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4*)vpStructOut)->CardSaleData, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4*)vpStructOut)->CardSaleExtraData, &pChildNode, &xmlInitDoc, &((RECEIPT_DATA4*)vpStructOut)->extData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4*)vpStructOut)->extData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4*)vpStructOut)->extData3, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				else if (lXmlFlags & RECEIPT_DATA4_EX_FLAG)
				{
					CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4_EX*)vpStructOut)->CardSaleData, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4_EX*)vpStructOut)->CardSaleExtraData, &pChildNode, &xmlInitDoc, &((RECEIPT_DATA4_EX*)vpStructOut)->extData3);	//4.0.18.670 73612 4.0.22.502
					CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4_EX*)vpStructOut)->extData2, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4_EX*)vpStructOut)->extData3, &pChildNode, &xmlInitDoc);
					CreateStructCardSaleExtraData4FromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4_EX*)vpStructOut)->extData4, &pChildNode, &xmlInitDoc); //4.0.27.45 - Egift
					CreateStructCardSaleExtraData5FromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4_EX*)vpStructOut)->extData5, &pChildNode, &xmlInitDoc); //4.0.27.45 - Egift
					CreateStructCardSaleExtraData6FromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4_EX*)vpStructOut)->extData6, &pChildNode, &xmlInitDoc); //4.0.27.45 - Egift

				}
				else if (lXmlFlags & CARD_SALE_EXTRA4_FLAG)
				{
					CreateStructCardSaleExtraData4FromXmlWithOutNodes(pszXMLIn, (CARD_SALE_EXTRA_DATA4_ *) vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & CARD_SALE_EXTRA5_FLAG)	//4.0.18.501
				{
					CreateStructCardSaleExtraData5FromXmlWithOutNodes(pszXMLIn, (CARD_SALE_EXTRA_DATA5_ *) vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & CARD_SALE_EXTRA6_FLAG)	//4.0.18.501
				{
					CreateStructCardSaleExtraData6FromXmlWithOutNodes(pszXMLIn, (CARD_SALE_EXTRA_DATA6_ *) vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & DALLAS_KEYS_INFO_FLAG)
				{
					CreateStructDallasKeysInfoFromXmlWithOutNodes(pszXMLIn, (DALLAS_KEYS_INFO *) vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & DISCOUNT_ATTRIBUTES_INFO_FLAG)
				{
					CreateStructDiscountAttrsInfoFromXmlWithOutNodes(pszXMLIn, (DISCOUNT_ATTRIBUTES *) vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & DISCOUNT_ATTRIBUTES_EXT_FLAG)	//4.0.22.509 38162
				{
					CreateStructDiscountAttrsExtFromXmlWithOutNodes(pszXMLIn, (DISCOUNT_ATTRIBUTES_EXT *) vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & CARD_SALE_ALL_CHIP_EXTRA_FLAG)	//4.0.20.503 66960
				{
					CreateStructCardSaleAll3FromXmlWithOutNodes(pszXMLIn, &((CARD_SALE_ALL_CHIP_EXTRA_DATA*)vpStructOut)->cCardSaleAll3, &pChildNode, &xmlInitDoc);
					CreateStructExtraChipFromXmlWithOutNodes(pszXMLIn,	&((CARD_SALE_ALL_CHIP_EXTRA_DATA*)vpStructOut)->cChipExtraData, &pChildNode, &xmlInitDoc);
				}
			}
		}
		//TD 38654
		if ((lXmlFlags & TAG_PUMP_TRANSACT2_FLAG) || (lXmlFlags & TAG_PUMP_TRANSACT5_FLAG) ||
		(lXmlFlags & TAG_OLA_REC9_FLAG) || (lXmlFlags & TAG_OLA_REC9_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC8_FLAG) || (lXmlFlags & TAG_OLA_REC8_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC7_FLAG) || (lXmlFlags & TAG_OLA_REC7_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC6_FLAG) || (lXmlFlags & TAG_OLA_REC6_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC5_FLAG) || (lXmlFlags & TAG_OLA_REC5_EX_FLAG) || (lXmlFlags & TAG_OLA_REC4_FLAG) || (lXmlFlags & TAG_OLA_REC4_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC3_FLAG) || (lXmlFlags & TAG_OLA_REC3_EX_FLAG) || (lXmlFlags & RECEIPT_DATA4_FLAG) || (lXmlFlags & RECEIPT_DATA4_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC10_FLAG) ||(lXmlFlags & TAG_OLA_REC11_FLAG) ||(lXmlFlags & TAG_OLA_REC12_FLAG) ||(lXmlFlags & TAG_OLA_REC13_FLAG)) //4.0.22.511 71868
		{
			//check if the xml has a child with the name Transaction.
			l_bFoundTransaction = xmlInitDoc.SelectSingleElementNode("Transaction", &pChildNode, NULL);

			if (l_bFoundTransaction)
			{
				if (lXmlFlags & TAG_OLA_REC13_FLAG) //4.0.22.511 71868
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC13*) vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.tagTrs, &pChildNode, &xmlInitDoc);
				}
				if (lXmlFlags & TAG_OLA_REC12_FLAG) //4.0.22.511 71868
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC12*) vpStructOut)->tagOlaRec11.tagOlaRec10.tagTrs, &pChildNode, &xmlInitDoc);
				}
				if (lXmlFlags & TAG_OLA_REC11_FLAG) //4.0.20.507
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.tagTrs, &pChildNode, &xmlInitDoc);
				}
				if (lXmlFlags & TAG_OLA_REC10_FLAG) //4.0.17.501
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC10*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				if (lXmlFlags & TAG_OLA_REC9_FLAG)
				{
					CreateStructTagPumpTransact2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				if (lXmlFlags & TAG_OLA_REC9_EX_FLAG)
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9_EX*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC8_FLAG)
				{
					CreateStructTagPumpTransact2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC8_EX_FLAG)
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8_EX*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC7_FLAG)
				{
					CreateStructTagPumpTransact2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC7*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC7_EX_FLAG)
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC7_EX*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC6_FLAG)
				{
					CreateStructTagPumpTransact2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC6_EX_FLAG)
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6_EX*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC5_FLAG)
				{
					CreateStructTagPumpTransact2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC5*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC5_EX_FLAG)
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC5_EX*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC4_FLAG)
				{
					CreateStructTagPumpTransact2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC4*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC4_EX_FLAG)
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC4_EX*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC3_FLAG)
				{
					CreateStructTagPumpTransact2FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC3*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC3_EX_FLAG)
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC3_EX*)vpStructOut)->tagTrs, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_PUMP_TRANSACT2_FLAG)
				{
					CreateStructTagPumpTransact2FromXmlWithOutNodes(pszXMLIn,(TAG_PUMP_TRANSACT2*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_PUMP_TRANSACT5_FLAG) //TD 38654
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn,(TAG_PUMP_TRANSACT5*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & RECEIPT_DATA4_FLAG)
				{
					CreateStructTagPumpTransact2FromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4*)vpStructOut)->PumpTransact, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & RECEIPT_DATA4_EX_FLAG)
				{
					CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4_EX*)vpStructOut)->PumpTransact, &pChildNode, &xmlInitDoc);
				}
			}
		}
		//TD 38654
		if ((lXmlFlags & BAP_ITEM_FOR_RECEIPT_FLAG) || (lXmlFlags & BAP_ITEM_FOR_RECEIPT2_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG) ||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG) ||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG) ||
		(lXmlFlags & TAG_OLA_REC9_FLAG) || (lXmlFlags & TAG_OLA_REC9_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC7_FLAG) || (lXmlFlags & TAG_OLA_REC7_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC6_FLAG) || (lXmlFlags & TAG_OLA_REC6_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC4_FLAG) || (lXmlFlags & TAG_OLA_REC4_EX_FLAG) ||
		(lXmlFlags & BAP_ALL_ITEM_FOR_RECEIPT_FLAG) || (lXmlFlags & BAP_ALL_ITEM_FOR_RECEIPT2_FLAG) ||
		(lXmlFlags & BAP_FUEL_ITEM_FLAG) || (lXmlFlags & BAP_FUEL_ITEM_EX_FLAG) || (lXmlFlags & TAG_OLA_REC10_FLAG) ||
		(lXmlFlags & TAG_OLA_REC11_FLAG) || (lXmlFlags & TAG_OLA_REC12_FLAG)|| (lXmlFlags & TAG_OLA_REC13_FLAG))  //4.0.22.511 71868
		{
			//check if the xml has a child with the name BuyAtPump.
			if (!strcmp("AddFuelItem", sName))
			{
				CXMLPointers pRootNode;
				l_bFoundBuyAtPump = xmlInitDoc.SelectSingleElementNode("FuelTransactionData", &pRootNode, NULL);
				l_bFoundBuyAtPump = xmlInitDoc.SelectSingleElementNode("BuyAtPump", &pChildNode, &pRootNode);
			}
			else
			{
				l_bFoundBuyAtPump = xmlInitDoc.SelectSingleElementNode("BuyAtPump", &pChildNode, NULL);
			}
			if (l_bFoundBuyAtPump)
			{
				if (lXmlFlags & BAP_ITEM_FOR_RECEIPT_FLAG)
				{
					CreateStructBuyAtPumpItemForReceiptFromXmlWithOutNodes(pszXMLIn,(BAP_ITEM_FOR_RECEIPT*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & BAP_ITEM_FOR_RECEIPT2_FLAG)
				{
					CreateStructBuyAtPumpItemForReceipt2FromXmlWithOutNodes(pszXMLIn,(BAP_ITEM_FOR_RECEIPT2*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & BAP_ALL_ITEM_FOR_RECEIPT_FLAG)
				{
					CreateStructBuyAtPumpAllItemsForReceiptFromXmlWithOutNodes(pszXMLIn,(BAP_ALL_ITEMS_FOR_RECEIPT*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & BAP_ALL_ITEM_FOR_RECEIPT2_FLAG)
				{
					CreateStructBuyAtPumpAllItemsForReceipt2FromXmlWithOutNodes(pszXMLIn,(BAP_ALL_ITEMS_FOR_RECEIPT2*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & BAP_FUEL_ITEM_FLAG)
				{
					CreateStructBuyAtPumpFuelItemFromXMLWithOutNode(pszXMLIn,(BAP_FUEL_ITEM*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & BAP_FUEL_ITEM_EX_FLAG) //TD 38654
				{
					CreateStructBuyAtPumpFuelItemFromXMLWithOutNode(pszXMLIn,(BAP_FUEL_ITEM_EX*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC6_FLAG)
				{
					CreateStructBuyAtPumpAllItemsForReceiptFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6*)vpStructOut)->Items, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC6_EX_FLAG)
				{
					CreateStructBuyAtPumpAllItemsForReceiptFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC6_EX*)vpStructOut)->Items, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC4_FLAG)
				{
					CreateStructBuyAtPumpAllItemsForReceiptFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC4*)vpStructOut)->Items, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC4_EX_FLAG)
				{
					CreateStructBuyAtPumpAllItemsForReceiptFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC4_EX*)vpStructOut)->Items, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG)
				{
					CreateStructBuyAtPumpAllItemsForReceiptFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA3*)vpStructOut)->cBuyAtPumpDataForReceipt, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG)
				{
					CreateStructBuyAtPumpAllItemsForReceiptFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA*)vpStructOut)->cBuyAtPumpDataForReceipt, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG)
				{
					CreateTwoStructBuyAtPumpAllWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA6*)vpStructOut)->cBuyAtPumpDataForReceipt, &((TAG_RECEIPT_GENERAL_DATA6*)vpStructOut)->cBuyAtPumpDataForReceipt2, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG)
				{
					CreateTwoStructBuyAtPumpAllWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA5*)vpStructOut)->cBuyAtPumpDataForReceipt, &((TAG_RECEIPT_GENERAL_DATA5*)vpStructOut)->cBuyAtPumpDataForReceipt2, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG)
				{
					CreateTwoStructBuyAtPumpAllWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA4*)vpStructOut)->cBuyAtPumpDataForReceipt, &((TAG_RECEIPT_GENERAL_DATA4*)vpStructOut)->cBuyAtPumpDataForReceipt2, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC13_FLAG) //4.0.22.511 71868
				{
					CreateTwoStructBuyAtPumpAllWithOutNodes(pszXMLIn, &((TAG_OLA_REC13*) vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.Items, &((TAG_OLA_REC10*) vpStructOut)->Items2, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC12_FLAG) //4.0.22.511 71868
				{
					CreateTwoStructBuyAtPumpAllWithOutNodes(pszXMLIn, &((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.tagOlaRec10.Items, &((TAG_OLA_REC10*)vpStructOut)->Items2, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC11_FLAG) //4.0.20.507
				{
					CreateTwoStructBuyAtPumpAllWithOutNodes(pszXMLIn, &((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.Items, &((TAG_OLA_REC10*)vpStructOut)->Items2, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC10_FLAG) //4.0.17.501
				{
					CreateTwoStructBuyAtPumpAllWithOutNodes(pszXMLIn, &((TAG_OLA_REC10*)vpStructOut)->Items, &((TAG_OLA_REC10*)vpStructOut)->Items2, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC9_FLAG)
				{
					CreateTwoStructBuyAtPumpAllWithOutNodes(pszXMLIn, &((TAG_OLA_REC9*)vpStructOut)->Items, &((TAG_OLA_REC9*)vpStructOut)->Items2, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC9_EX_FLAG)
				{
					CreateTwoStructBuyAtPumpAllWithOutNodes(pszXMLIn, &((TAG_OLA_REC9_EX*)vpStructOut)->Items, &((TAG_OLA_REC9_EX*)vpStructOut)->Items2, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_OLA_REC7_FLAG)
				{
					CreateTwoStructBuyAtPumpAllWithOutNodes(pszXMLIn, &((TAG_OLA_REC7*)vpStructOut)->Items, &((TAG_OLA_REC7*)vpStructOut)->Items2, &pChildNode, &xmlInitDoc);
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC7_EX_FLAG)
				{
					CreateTwoStructBuyAtPumpAllWithOutNodes(pszXMLIn, &((TAG_OLA_REC7_EX*)vpStructOut)->Items, &((TAG_OLA_REC7_EX*)vpStructOut)->Items2, &pChildNode, &xmlInitDoc);
				}
			}
		}
		//TD 38654
		if ((lXmlFlags & TAG_GENERAL_INFO_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG) ||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG) ||
		(lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG) || (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG) ||
		(lXmlFlags & RECEIPT_EXTRA_INFO_FLAG) ||
		(lXmlFlags & TAG_OLA_REC9_FLAG) || (lXmlFlags & TAG_OLA_REC9_EX_FLAG) ||
		(lXmlFlags & TAG_OLA_REC8_FLAG) || (lXmlFlags & TAG_OLA_REC8_EX_FLAG) ||
		(lXmlFlags & CARD_SALE_EXTRA_DATA4_PLUS_FLAG) || (lXmlFlags & RECEIPT_DATA4_FLAG) ||
		(lXmlFlags & RECEIPT_DATA4_EX_FLAG) || (lXmlFlags & TAG_OLA_REC10_FLAG) || (lXmlFlags & TAG_OLA_REC11_FLAG) ||
		(lXmlFlags & TAG_OLA_REC12_FLAG)|| (lXmlFlags & TAG_OLA_REC13_FLAG) ) //4.0.22.511 71868
		{
			//check if the xml has a child with the name Receipt.
			l_bFoundReceipt = xmlInitDoc.SelectSingleElementNode("Receipt", &pChildNode, NULL);
			if (l_bFoundReceipt)
			{

				if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA6_FLAG)
				{
					CreateStructTagGeneralInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA6*)vpStructOut)->cTagGeneralInfo, &pChildNode, &xmlInitDoc);
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA6*)vpStructOut)->cReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA5_FLAG)
				{
					CreateStructTagGeneralInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA5*)vpStructOut)->cTagGeneralInfo, &pChildNode, &xmlInitDoc);
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA5*)vpStructOut)->cReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA4_FLAG)
				{
					CreateStructTagGeneralInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA4*)vpStructOut)->cTagGeneralInfo, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA3_FLAG)
				{
					CreateStructTagGeneralInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA3*)vpStructOut)->cTagGeneralInfo, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_RECEIPT_GENERAL_DATA_FLAG)
				{
					CreateStructTagGeneralInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_RECEIPT_GENERAL_DATA*)vpStructOut)->cTagGeneralInfo, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & TAG_GENERAL_INFO_FLAG)
				{
					CreateStructTagGeneralInfoFromXmlWithOutNodes(pszXMLIn,(TAG_GENERAL_INFO*)vpStructOut, &pChildNode, &xmlInitDoc);
				}
				else if (lXmlFlags & RECEIPT_EXTRA_INFO_FLAG)
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn,(RECEIPT_EXTRA_INFO*)vpStructOut, &pChildNode2, &xmlInitDoc);
					}
				}
				else if (lXmlFlags & TAG_OLA_REC8_FLAG)
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8*)vpStructOut)->ReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC8_EX_FLAG)
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC8_EX*)vpStructOut)->ReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				else if (lXmlFlags & TAG_OLA_REC9_FLAG)
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9*)vpStructOut)->ReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				//TD 38654
				else if (lXmlFlags & TAG_OLA_REC9_EX_FLAG)
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC9_EX*)vpStructOut)->ReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				else if (lXmlFlags & TAG_OLA_REC10_FLAG)
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC10*)vpStructOut)->ReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				else if (lXmlFlags & TAG_OLA_REC11_FLAG)
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC11*)vpStructOut)->tagOlaRec10.ReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				else if (lXmlFlags & TAG_OLA_REC13_FLAG)//4.0.22.511 71868
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC13*)vpStructOut)->tagOlaRec12.tagOlaRec11.tagOlaRec10.ReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				else if (lXmlFlags & TAG_OLA_REC12_FLAG)//4.0.22.511 71868
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((TAG_OLA_REC12*)vpStructOut)->tagOlaRec11.tagOlaRec10.ReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				else if (lXmlFlags & CARD_SALE_EXTRA_DATA4_PLUS_FLAG)
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((CARD_SALE_EXTRA_DATA4_PLUS*)vpStructOut)->cReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				else if (lXmlFlags & RECEIPT_DATA4_FLAG)
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4*)vpStructOut)->cReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
				else if (lXmlFlags & RECEIPT_DATA4_EX_FLAG)
				{
					l_bFoundReceiptExtraInfo = xmlInitDoc.SelectSingleElementNode("ExtraInfo", &pChildNode2, &pChildNode);
					if (l_bFoundReceiptExtraInfo)
					{
						CreateStructReceiptExtraInfoFromXmlWithOutNodes(pszXMLIn, &((RECEIPT_DATA4_EX*)vpStructOut)->cReceiptExtraInfo, &pChildNode2, &xmlInitDoc);
					}
				}
			}
		}
	}

	xmlInitDoc.CloseDoc();
	return l_bReturn;
}


/*******************************************************************
 Description:		Split a string to two strings
 Returns:			void
 Parameters:   		1.sMsg - The string that we need to split.
					2.pStrMSB - The MSB part of the string.
					3.lStrMSBLen - Length of the MSB string.
					4.pStrLSB - The LSB part of the string.
					5.lStrLSBLen - Length of the LSB string.

 WHO				WHEN                    WHAT
--------------------------------------------------------------------
 Ariel			15/12/2006   12:02			Start
********************************************************************/
void CXmlConvertor::SplitValue(char *sMsg, unsigned char *pStrMSB, unsigned int lStrMSBLen, unsigned char *pStrLSB, unsigned int lStrLSBLen)
{
	memcpy((unsigned char *) pStrMSB, (unsigned char *) sMsg, lStrMSBLen );
	memcpy((unsigned char *) pStrLSB, (unsigned char *) sMsg + lStrMSBLen, lStrLSBLen );
}

void CXmlConvertor::CreateXMLFromStructBuyAtPumpFuelItem (char * pszXMLOut, BAP_FUEL_ITEM * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	CXMLPointers pChildNode;

	xmlInitDoc->CreateElement("Transactions", &pChildNode);//Add TagPumpTransact2
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);
	CreateXMLFromStructTagPumpTransact2WithOutNodes(pszXMLOut, &StructIn->cPumpTransact, xmlInitDoc, &pChildNode);
	CreateXMLFromStructDiscountAttrsInfo(pszXMLOut, &StructIn->discountAttrsInfo, xmlInitDoc, pCurrElement, &StructIn->sDiscountAttrExt); //CR-38162 //4.0.22.509 38162

	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"ServiceFee", StructIn->sServiceFee, sizeof(StructIn->sServiceFee));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"FullServiceCashPrice", StructIn->sFullSerCashPrice, sizeof(StructIn->sFullSerCashPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"FullServiceCreditPrice", StructIn->sFullSerCreditPrice, sizeof(StructIn->sFullSerCreditPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"SelfServiceCashPrice", StructIn->sSelfSerCashPrice, sizeof(StructIn->sSelfSerCashPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"SelfServiceCreditPrice", StructIn->sSelfSerCreditPrice, sizeof(StructIn->sSelfSerCreditPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"GradeTotalDiscount", StructIn->sGRTotalDiscount, sizeof(StructIn->sGRTotalDiscount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"CouponAmount", StructIn->sCouponAmount, sizeof(StructIn->sCouponAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"LoyaltyID", StructIn->sLoyaltyID, sizeof(StructIn->sLoyaltyID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"VehicleID", StructIn->sVehicleID, sizeof(StructIn->sVehicleID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"CardAmount", StructIn->sCardAmount, sizeof(StructIn->sCardAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"DiscountPerLiter", StructIn->sDiscountPerLiter, sizeof(StructIn->sDiscountPerLiter));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"CardAmountByVolume", &StructIn->sCardAmountByVolume, sizeof(StructIn->sCardAmountByVolume));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"ReferenceNumber", StructIn->sPMTReferenceNumber, sizeof(StructIn->sPMTReferenceNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"LoyaltyRecalculationRequiered", &StructIn->sLoyaltyRecalculationRequired, sizeof(StructIn->sLoyaltyRecalculationRequired));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"PresetLimit", StructIn->sPreSetlimit, sizeof(StructIn->sPreSetlimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"PresetLimitType", &StructIn->sPreSetlimitType, sizeof(StructIn->sPreSetlimitType));
//	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"Spare", StructIn->spare, sizeof(StructIn->spare));
}

//TD 38654
void CXmlConvertor::CreateXMLFromStructBuyAtPumpFuelItem (char * pszXMLOut, BAP_FUEL_ITEM_EX * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	CXMLPointers pChildNode;

	xmlInitDoc->CreateElement("Transactions", &pChildNode);//Add TagPumpTransact2
	xmlInitDoc->AppendChild(&pChildNode, pCurrElement);
	CreateXMLFromStructTagPumpTransact5WithOutNodes(pszXMLOut, &StructIn->cPumpTransact, xmlInitDoc, &pChildNode);

	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"ServiceFee", StructIn->sServiceFee, sizeof(StructIn->sServiceFee));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"FullServiceCashPrice", StructIn->sFullSerCashPrice, sizeof(StructIn->sFullSerCashPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"FullServiceCreditPrice", StructIn->sFullSerCreditPrice, sizeof(StructIn->sFullSerCreditPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"SelfServiceCashPrice", StructIn->sSelfSerCashPrice, sizeof(StructIn->sSelfSerCashPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"SelfServiceCreditPrice", StructIn->sSelfSerCreditPrice, sizeof(StructIn->sSelfSerCreditPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"GradeTotalDiscount", StructIn->sGRTotalDiscount, sizeof(StructIn->sGRTotalDiscount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"CouponAmount", StructIn->sCouponAmount, sizeof(StructIn->sCouponAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"LoyaltyID", StructIn->sLoyaltyID, sizeof(StructIn->sLoyaltyID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"VehicleID", StructIn->sVehicleID, sizeof(StructIn->sVehicleID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"CardAmount", StructIn->sCardAmount, sizeof(StructIn->sCardAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"DiscountPerLiter", StructIn->sDiscountPerLiter, sizeof(StructIn->sDiscountPerLiter));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"CardAmountByVolume", &StructIn->sCardAmountByVolume, sizeof(StructIn->sCardAmountByVolume));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"ReferenceNumber", StructIn->sPMTReferenceNumber, sizeof(StructIn->sPMTReferenceNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"LoyaltyRecalculationRequiered", &StructIn->sLoyaltyRecalculationRequired, sizeof(StructIn->sLoyaltyRecalculationRequired));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"PresetLimit", StructIn->sPreSetlimit, sizeof(StructIn->sPreSetlimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"PresetLimitType", &StructIn->sPreSetlimitType, sizeof(StructIn->sPreSetlimitType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"Spare", StructIn->spare, sizeof(StructIn->spare));
}

void CXmlConvertor::CreateStructBuyAtPumpFuelItemFromXMLWithOutNode (char * pszXMLIn, BAP_FUEL_ITEM * StructOut, CXMLPointers *pXml, CXMLInterface * xmlInitDoc)
{
	BOOL bTrnsNode;
	CXMLPointers pTransactionsNode;
	bTrnsNode = xmlInitDoc->SelectSingleElementNode("Transactions", &pTransactionsNode, pXml);
	CreateStructTagPumpTransact2FromXmlWithOutNodes(pszXMLIn, &StructOut->cPumpTransact, &pTransactionsNode, xmlInitDoc);
	CreateStructDiscountAttrsInfoFromXmlWithOutNodes(pszXMLIn, &StructOut->discountAttrsInfo, pXml/*&pTransactionsNode*/, xmlInitDoc);  //CR-38162
	CreateStructDiscountAttrsExtFromXmlWithOutNodes(pszXMLIn, &StructOut->sDiscountAttrExt, pXml/*&pTransactionsNode*/, xmlInitDoc);  //CR-38162

	xmlInitDoc->GetAttributeToByteArrWithConvert("ServiceFee", StructOut->sServiceFee, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FullServiceCashPrice", StructOut->sFullSerCashPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FullServiceCreditPrice", StructOut->sFullSerCreditPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("SelfServiceCashPrice", StructOut->sSelfSerCashPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("SelfServiceCreditPrice", StructOut->sSelfSerCreditPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GradeTotalDiscount", StructOut->sGRTotalDiscount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CouponAmount", StructOut->sCouponAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyID", StructOut->sLoyaltyID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("VehicleID", StructOut->sVehicleID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardAmount", StructOut->sCardAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DiscountPerLiter", StructOut->sDiscountPerLiter, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardAmountByVolume", StructOut->sCardAmountByVolume, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReferenceNumber", StructOut->sPMTReferenceNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyRecalculationRequiered", StructOut->sLoyaltyRecalculationRequired, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PresetLimit", StructOut->sPreSetlimit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PresetLimitType", StructOut->sPreSetlimitType, pXml);
//	xmlInitDoc->GetAttributeToByteArrWithConvert("Spare", StructOut->spare, pXml);
}
//TD 38654
void CXmlConvertor::CreateStructBuyAtPumpFuelItemFromXMLWithOutNode (char * pszXMLIn, BAP_FUEL_ITEM_EX * StructOut, CXMLPointers *pXml, CXMLInterface * xmlInitDoc)
{
	BOOL bTrnsNode;
	CXMLPointers pTransactionsNode;
	bTrnsNode = xmlInitDoc->SelectSingleElementNode("Transactions", &pTransactionsNode, NULL);
	CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, &StructOut->cPumpTransact, &pTransactionsNode, xmlInitDoc);

	xmlInitDoc->GetAttributeToByteArrWithConvert("ServiceFee", StructOut->sServiceFee, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FullServiceCashPrice", StructOut->sFullSerCashPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FullServiceCreditPrice", StructOut->sFullSerCreditPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("SelfServiceCashPrice", StructOut->sSelfSerCashPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("SelfServiceCreditPrice", StructOut->sSelfSerCreditPrice, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GradeTotalDiscount", StructOut->sGRTotalDiscount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CouponAmount", StructOut->sCouponAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyID", StructOut->sLoyaltyID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("VehicleID", StructOut->sVehicleID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardAmount", StructOut->sCardAmount, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("DiscountPerLiter", StructOut->sDiscountPerLiter, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardAmountByVolume", StructOut->sCardAmountByVolume, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ReferenceNumber", StructOut->sPMTReferenceNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LoyaltyRecalculationRequiered", StructOut->sLoyaltyRecalculationRequired, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PresetLimit", StructOut->sPreSetlimit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PresetLimitType", StructOut->sPreSetlimitType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Spare", StructOut->spare, pXml);
}

BOOL CXmlConvertor::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_ALL3 * StructIn, EXTRA_CHIP_INFO * cChipExtraData)
{
	BOOL l_bReturn = TRUE;
	CARD_SALE_ALL_CHIP_EXTRA_DATA cAllChipExtraData3;
	memset(&cAllChipExtraData3, 0x00, sizeof(cAllChipExtraData3));
	memcpy(&cAllChipExtraData3.cCardSaleAll3, StructIn, sizeof(cAllChipExtraData3.cCardSaleAll3));
	memcpy(&cAllChipExtraData3.cChipExtraData, cChipExtraData, sizeof(cAllChipExtraData3.cChipExtraData));

	try {
			l_bReturn = CreateXml(pszXMLOut,(void*)&cAllChipExtraData3, CARD_SALE_ALL_CHIP_EXTRA_FLAG);

	}
	catch (...) {
			l_bReturn = FALSE;
	}

	return l_bReturn;
}

void CXmlConvertor::CreateXMLFromChipExtraData(char * pszXMLOut, EXTRA_CHIP_INFO * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"CvmList", StructIn->byData, sizeof(StructIn->byData));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"CvmListSize", StructIn->byLen, sizeof(StructIn->byLen));
}


void CXmlConvertor::CreateStructGradeLimitFromXmlWithOutNodes(CXMLInterface * xmlInitDoc, CXMLPointers * pXml, char * pszXMLIn, CARD_SALE_EXTRA_DATA3_ * StructOut )
{
	CXMLPointers pChildNode, pItemElement;
	int i = 0;
	BOOL bIsExists;

	bIsExists = xmlInitDoc->SelectSingleElementNode("Limits", &pChildNode, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetChildNodesList(&pChildNode, &pItemElement);

		while(xmlInitDoc->GetNextItem(pItemElement, pChildNode))
		{
			if ( i < MAX_POSITIONS_PER_PUMP )
			{
				CreateStructAmountGradeLimitFromXmlWithOutNodes(pszXMLIn, &StructOut->GradeLimitsArray[i], &pChildNode, xmlInitDoc);
				if (a2l(StructOut->GradeLimitsArray[i].sGradeAmountLimit, sizeof(StructOut->GradeLimitsArray[i].sGradeAmountLimit)))
				{
					i++;
				}
			}
			else
			{
				char sMsg[256];
				sprintf(sMsg, "CreateStructGradeLimitFromXmlWithOutNodes(GradeLimitsArray) - Found unavailable nozzle position - %ld", i);
				_Module.m_server.m_cLogger.LogMsg(sMsg);
			}
		}
	}
}

// TD 370688
void CXmlConvertor::CreateStructGradeLimitFromXmlWithOutNodes(CXMLInterface * xmlInitDoc, CXMLPointers * pXml, char * pszXMLIn, CARD_SALE_EXTRA_DATA5_ * StructOut )
{
	CXMLPointers pChildNode, pItemElement;
	int i = 0;
	BOOL bIsExists;

	bIsExists = xmlInitDoc->SelectSingleElementNode("Limits", &pChildNode, pXml);
	if (bIsExists)
	{
		xmlInitDoc->GetChildNodesList(&pChildNode, &pItemElement);

		while(xmlInitDoc->GetNextItem(pItemElement, pChildNode))
		{
			if ( i < MAX_POSITIONS_PER_PUMP )
			{
				CreateStructVolumeGradeLimitFromXmlWithOutNodes(pszXMLIn, &StructOut->GradeLimitsArrayEx[i], &pChildNode, xmlInitDoc);
				if (a2l(StructOut->GradeLimitsArrayEx[i].sGradeAmountLimit, sizeof(StructOut->GradeLimitsArrayEx[i].sGradeAmountLimit)))
				{
					i++;
				}
			}
			else
			{
				char sMsg[256];
				sprintf(sMsg, "CreateStructGradeLimitFromXmlWithOutNodes(GradeLimitsArrayEx) - Found unavailable nozzle position - %ld", i);
				_Module.m_server.m_cLogger.LogMsg(sMsg);
			}
		}
	}
}

void CXmlConvertor::CreateXMLFromStructCardSaleExtraData6(char * pszXMLOut, CARD_SALE_EXTRA_DATA6_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	CXMLPointers  pCarWashDetailsNode, pLoyaltyStartTicketTimeNode, pGradeDiscountID;


	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TransactionId", StructIn->sTransactionId, sizeof(StructIn->sTransactionId));  
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "StoreID", StructIn->sStoreIDField, sizeof(StructIn->sStoreIDField));				// CR 425758 
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PosID", StructIn->sPosId, sizeof(StructIn->sPosId));				// CR 425758 
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TransID", StructIn->sTransId, sizeof(StructIn->sTransId));				// CR 425758 
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "StartDateTime", StructIn->sLoyaltyStartDateTime, sizeof(StructIn->sLoyaltyStartDateTime));				// CR 425758 
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TicketIdentifier", StructIn->sTicketId, sizeof(StructIn->sTicketId));				// CR 425758 

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ClubType", StructIn->sClubType, sizeof(StructIn->sClubType));				// marik1969
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IdRetryCount", &StructIn->sIdRetryCount, sizeof(StructIn->sIdRetryCount));	// marik1969
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromotionID", StructIn->sPromotionID, sizeof(StructIn->sPromotionID));	// RFUEL-155

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "WashExpireTime", StructIn->sWashExpireTime, sizeof(StructIn->sWashExpireTime));

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ClubCardHashNumber", StructIn->sClubCardHashNumber, sizeof(StructIn->sClubCardHashNumber));  // 4.0.32.??? TD 405435
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TenderDiscount", StructIn->sTenderDiscount, sizeof(StructIn->sTenderDiscount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TenderPromotionDescription", StructIn->sTenderPromotionDescription, sizeof(StructIn->sTenderPromotionDescription));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyPointsExt", StructIn->sLoyaltyPointsExt, sizeof(StructIn->sLoyaltyPointsExt));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CommunicationReward", StructIn->sCommunicationReward, sizeof(StructIn->sCommunicationReward));

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AppVersionNumberICC", StructIn->sAppVersionNumberICC, sizeof(StructIn->sAppVersionNumberICC)); // TD 453560  [6/4/2017 MaximB]
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CardRiskManagementDataObjectList2", StructIn->sCardRiskManagementDataObjectList2, sizeof(StructIn->sCardRiskManagementDataObjectList2)); // TD 453560  [6/4/2017 MaximB]
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CardTypeThirdParty", StructIn->sCardTypeThirdParty, sizeof(StructIn->sCardTypeThirdParty)); // TD 453560  [6/4/2017 MaximB]
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "IssuerScriptResult", StructIn->sIssuerScriptResult, sizeof(StructIn->sIssuerScriptResult));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PinPadCapabilities", StructIn->sPinPadCapabilities, sizeof(StructIn->sPinPadCapabilities)); // TD 453560  [6/4/2017 MaximB]

	xmlInitDoc->CreateElement("CarWashItem", &pCarWashDetailsNode);
	xmlInitDoc->AppendChild(&pCarWashDetailsNode, pCurrElement);
	CreateXMLFromStructCarWashItemWithOutNodes(pszXMLOut, &StructIn->CarWashItem, xmlInitDoc, &pCarWashDetailsNode);

	CXMLPointers pPaymentCardDetalis;
	xmlInitDoc->CreateElement("PaymentCardDetails", &pPaymentCardDetalis);
	xmlInitDoc->AppendChild(&pPaymentCardDetalis, pCurrElement);
	CreateXmlFromStructPaymentCardDetails(xmlInitDoc, &pPaymentCardDetalis, pszXMLOut, StructIn);
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "MaxAllowedDiscount", StructIn->sMaxDiscountAllowed, sizeof(StructIn->sMaxDiscountAllowed));  //RFUEL-3309 Get the maximal discount allowed for the grade 
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PartialApproval", &StructIn->sPartialApproval, sizeof(StructIn->sPartialApproval));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "RemainingBalance", StructIn->sRemainingBalance, sizeof(StructIn->sRemainingBalance));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxName1", StructIn->sTaxName1, sizeof(StructIn->sTaxName1));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxName2", StructIn->sTaxName2, sizeof(StructIn->sTaxName2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxName3", StructIn->sTaxName3, sizeof(StructIn->sTaxName3));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxName4", StructIn->sTaxName4, sizeof(StructIn->sTaxName4));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmt4", StructIn->sTaxAmt4, sizeof(StructIn->sTaxAmt4));

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxType1", &StructIn->sTaxType1, sizeof(StructIn->sTaxType1));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxType2", &StructIn->sTaxType2, sizeof(StructIn->sTaxType2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxType3", &StructIn->sTaxType3, sizeof(StructIn->sTaxType3));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxType4", &StructIn->sTaxType4, sizeof(StructIn->sTaxType4));

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CCMTaxLink", StructIn->sCCMTaxLink, sizeof(StructIn->sCCMTaxLink));

	xmlInitDoc->CreateElement("LoyaltyStartTime", &pLoyaltyStartTicketTimeNode);
	xmlInitDoc->AppendChild(&pLoyaltyStartTicketTimeNode, pCurrElement);
		CreateXMLFromStructTagSystemTimeWithOutNodes(pszXMLOut, (TAG_SYSTEMTIME*)&StructIn->sLoyaltyStartTime, xmlInitDoc, &pLoyaltyStartTicketTimeNode);

	//MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
	CXMLPointers pLoyaltyCardDetalis;
	xmlInitDoc->CreateElement("CustomerInformation", &pLoyaltyCardDetalis);
	xmlInitDoc->AppendChild(&pLoyaltyCardDetalis, pCurrElement);
	CreateXmlFromStructLoyaltyCardDetailsWithOutNodes(pszXMLOut, &StructIn->LoyaltyCard, xmlInitDoc, &pLoyaltyCardDetalis);

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "QuickChipTrs", &StructIn->sQuickChipTrs, sizeof(StructIn->sQuickChipTrs));
	// RFUEL-2817, XML was missing TenderDiscount
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TenderDiscount", &StructIn->sTenderDiscount[0], sizeof(StructIn->sTenderDiscount));
	
	// RFUEL-3049
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "HostTranId", StructIn->sLoylatyRefHostTranId, sizeof(StructIn->sLoylatyRefHostTranId));

	char sTmp[15];
	memset(sTmp, 0, sizeof(sTmp));

	xmlInitDoc->CreateElement("GradeDiscountID", &pGradeDiscountID);//RFUEL-2093
	xmlInitDoc->AppendChild(&pGradeDiscountID, pCurrElement);
	for (int i = 0; i < MAX_POSITIONS_PER_PUMP; i++)
	{
		sprintf(sTmp, "ID%ld", i);
		EndXmlLineWithNull(xmlInitDoc, &pGradeDiscountID, sTmp, StructIn->sGradeDiscountID[i], sizeof(StructIn->sGradeDiscountID[i]));
	}
}

void CXmlConvertor::CreateStructCarWashItemFromXmlWithOutNodes(char * pszXMLIn, CARWASH_ITEM * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc )
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("BarCode", StructOut->sBarcode, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Sku", StructOut->sSku, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("Upc", StructOut->sUpc, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("VaildDays", StructOut->sVaildDays, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("WeatherGuarentee", StructOut->sWeatherGuarentee, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("UnitsPerDay", StructOut->sUnitsPerDay, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CWBarcodeType", StructOut->sCWBarcodeType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmount1", StructOut->sTaxAmount1, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxAmount2", StructOut->sTaxAmount2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxName1", StructOut->sTaxName1, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TaxName2", StructOut->sTaxName2, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ExpirationDate", StructOut->sExpirationDate, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PromotionDescription", StructOut->sPromotionDescription, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TotalDiscount", StructOut->sTotalDiscount, pXml);
}

void CXmlConvertor::CreateStructPaymentCardFromXmlWithOutNodes(char * pszXMLIn, PAYMENT_CARD_DETAILS * StructOut, CXMLPointers *pXml /*= NULL */, CXMLInterface *xmlInitDoc /*= NULL*/ )
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("AmountPaid", StructOut->sAmountPaid, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("RefernceNumber", StructOut->sRefernceNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AuthorizeAmountLimit", StructOut->sAuthorizeAmountLimit, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardName", StructOut->sCardName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("TenderId", StructOut->sTenderId, pXml);
}

void CXmlConvertor::CreateXMLFromStructCarWashItemWithOutNodes(char * pszXMLOut, CARWASH_ITEM * StructIn, CXMLInterface * xmlInitDoc /*= NULL */, CXMLPointers * pCurrElement /*= NULL*/ )
{
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"BarCode", StructIn->sBarcode, sizeof(StructIn->sBarcode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"Sku", StructIn->sSku, sizeof(StructIn->sSku));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"Upc", StructIn->sUpc, sizeof(StructIn->sUpc));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"VaildDays", StructIn->sVaildDays, sizeof(StructIn->sVaildDays));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"WeatherGuarentee", StructIn->sWeatherGuarentee, sizeof(StructIn->sWeatherGuarentee));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"UnitsPerDay", StructIn->sUnitsPerDay, sizeof(StructIn->sUnitsPerDay));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"CWBarcodeType", &StructIn->sCWBarcodeType, sizeof(StructIn->sCWBarcodeType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmount1", StructIn->sTaxAmount1, sizeof(StructIn->sTaxAmount1));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxAmount2", StructIn->sTaxAmount2, sizeof(StructIn->sTaxAmount2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxName1", StructIn->sTaxName1, sizeof(StructIn->sTaxName1));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TaxName2", StructIn->sTaxName2, sizeof(StructIn->sTaxName2));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ExpirationDate", StructIn->sExpirationDate, sizeof(StructIn->sExpirationDate));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromotionDescription", StructIn->sPromotionDescription, sizeof(StructIn->sPromotionDescription));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TotalDiscount", StructIn->sTotalDiscount, sizeof(StructIn->sTotalDiscount));

}

void CXmlConvertor::CreateXMLFromStructPaymentCardDetailsWithOutNodes(char * pszXMLOut, PAYMENT_CARD_DETAILS * StructIn, CXMLInterface * xmlInitDoc /*= NULL */, CXMLPointers * pCurrElement /*= NULL*/ )
{
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"AmountPaid", StructIn->sAmountPaid, sizeof(StructIn->sAmountPaid));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"AuthorizeAmountLimit", StructIn->sAuthorizeAmountLimit, sizeof(StructIn->sAuthorizeAmountLimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"RefernceNumber", StructIn->sRefernceNumber, sizeof(StructIn->sRefernceNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"CardName", StructIn->sCardName, sizeof(StructIn->sCardName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement,"TenderId", StructIn->sTenderId, sizeof(StructIn->sTenderId));
}

void CXmlConvertor::CreateXmlFromStructPaymentCardDetails(CXMLInterface * xmlInitDoc, CXMLPointers * pPaymentCardDetails, char * pszXMLOut, CARD_SALE_EXTRA_DATA6_ * StructIn)
{
	CXMLPointers pPaymentCard1, pPaymentCard2;
	xmlInitDoc->CreateElement("PaymentCard1", &pPaymentCard1);
	xmlInitDoc->AppendChild(&pPaymentCard1, pPaymentCardDetails);
	CreateXMLFromStructPaymentCardDetailsWithOutNodes(pszXMLOut, &StructIn->PaymentCard1, xmlInitDoc, &pPaymentCard1);

	xmlInitDoc->CreateElement("PaymentCard2", &pPaymentCard2);
	xmlInitDoc->AppendChild(&pPaymentCard2, pPaymentCardDetails);
	CreateXMLFromStructPaymentCardDetailsWithOutNodes(pszXMLOut, &StructIn->PaymentCard2, xmlInitDoc, &pPaymentCard2);
}

//MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
void CXmlConvertor::CreateStructLoyaltyCardDetailsFromXmlWithOutNodes(char * pszXMLIn, LOYALTY_CARD_DETAILS * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	xmlInitDoc->GetAttributeToByteArrWithConvert("ClubNumber", StructOut->sClubNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("AccountNumber", StructOut->sAccountNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ProgramLongName", StructOut->sProgramLongName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("ProgramShortName", StructOut->sProgramShortName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CardType", StructOut->sCardType, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("GivenName", StructOut->sGivenName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("FamilyName", StructOut->sFamilyName, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("LocalNumber", StructOut->sLocalNumber, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("EMailAddress", StructOut->sEMailAddress, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("CustomerID", StructOut->sCustomerID, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PointsBalance", StructOut->sPointsBalance, pXml);
	xmlInitDoc->GetAttributeToByteArrWithConvert("PointsOpenBalance", StructOut->sPointsOpenBalance, pXml);
}

void CXmlConvertor::CreateXmlFromStructLoyaltyCardDetailsWithOutNodes(char * pszXMLOut, LOYALTY_CARD_DETAILS * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement)
{
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ClubNumber", StructIn->sClubNumber, sizeof(StructIn->sClubNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AccountNumber", StructIn->sAccountNumber, sizeof(StructIn->sAccountNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ProgramLongName", StructIn->sProgramLongName, sizeof(StructIn->sProgramLongName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "ProgramShortName", StructIn->sProgramShortName, sizeof(StructIn->sProgramShortName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CardType", StructIn->sCardType, sizeof(StructIn->sCardType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "GivenName", StructIn->sGivenName, sizeof(StructIn->sGivenName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "FamilyName", StructIn->sFamilyName, sizeof(StructIn->sFamilyName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LocalNumber", StructIn->sLocalNumber, sizeof(StructIn->sLocalNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "EMailAddress", StructIn->sEMailAddress, sizeof(StructIn->sEMailAddress));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CustomerID", StructIn->sCustomerID, sizeof(StructIn->sCustomerID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PointsBalance", StructIn->sPointsBalance, sizeof(StructIn->sPointsBalance));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PointsOpenBalance", StructIn->sPointsOpenBalance, sizeof(StructIn->sPointsOpenBalance));
}

// RFUEL-520
void CXmlConvertor::CreateXmlFromStructLoyaltyInfo3WithOutNodes(char * pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO3 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)  
{
	CXMLPointers pCustomerInfo, pTriggerProm;
	CXMLPointers pLoyaltyCardDetalis;

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyalId", StructIn->sLoyalId, sizeof(StructIn->sLoyalId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PromotionNumber", StructIn->sPromotionNumber, sizeof(StructIn->sPromotionNumber));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AttendantID", StructIn->sAttendantID, sizeof(StructIn->sAttendantID));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "AttendantName", StructIn->sAttendantName, sizeof(StructIn->sAttendantName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DiscountType", StructIn->sDiscountType, sizeof(StructIn->sDiscountType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PreSetLimit", StructIn->sPreSetLimit, sizeof(StructIn->sPreSetLimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PreSetLimitType", &StructIn->sPreSetLimitType, sizeof(StructIn->sPreSetLimitType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CustomerName", StructIn->sCustomerName, sizeof(StructIn->sCustomerName));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TotalBalance", StructIn->sTotalBalance, sizeof(StructIn->sTotalBalance));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "BalanceType", &StructIn->sBalanceType, sizeof(StructIn->sBalanceType));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "HostTransId", StructIn->sHostTransId, sizeof(StructIn->sHostTransId));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LAMFlags", &StructIn->sLAMFlags, sizeof(StructIn->sLAMFlags));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LAM", StructIn->sLAM, sizeof(StructIn->sLAM));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashProgram", &StructIn->sCarWashProgram, sizeof(StructIn->sCarWashProgram));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "WashCode", StructIn->sWashCode, sizeof(StructIn->sWashCode));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "WashExpire", StructIn->sWashExpire, sizeof(StructIn->sWashExpire));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashDiscount", StructIn->sCarWashDiscount, sizeof(StructIn->sCarWashDiscount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashTax", StructIn->sCarWashTax, sizeof(StructIn->sCarWashTax));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "CarWashAmount", StructIn->sCarWashAmount, sizeof(StructIn->sCarWashAmount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OLDSession", StructIn->sOLDSession, sizeof(StructIn->sOLDSession));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "TotalDiscount", StructIn->sTotalDiscount, sizeof(StructIn->sTotalDiscount));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "OriginalUnitPrice", StructIn->sOriginalUnitPrice, sizeof(StructIn->sOriginalUnitPrice));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "LoyaltyVolumeLimit", StructIn->sLoyaltyVolumeLimit, sizeof(StructIn->sLoyaltyVolumeLimit));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "DeferralAccepted", &StructIn->sDeferralAccepted, sizeof(StructIn->sDeferralAccepted));
}

// RFUEL-520
void CXmlConvertor::CreateXmlFromStructLoyaltyInfo4WithOutNodes(char * pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO4 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)  
{
	CXMLPointers pCustomerInfo, pLoyaltyInfo, pTriggerProm;
	CXMLPointers pLoyaltyCardDetalis;

	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PostDispensingVolumeHigh", StructIn->sPostDispensingVolumeHigh, sizeof(StructIn->sPostDispensingVolumeHigh));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PostDispensingVolumeLow", StructIn->sPostDispensingVolumeLow, sizeof(StructIn->sPostDispensingVolumeLow));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PreDispensingVolumeHigh", StructIn->sPreDispensingVolumeHigh, sizeof(StructIn->sPreDispensingVolumeHigh));
	EndXmlLineWithNull(xmlInitDoc, pCurrElement, "PreDispensingVolumeLow", StructIn->sPreDispensingVolumeLow, sizeof(StructIn->sPreDispensingVolumeLow));
	
	xmlInitDoc->CreateElement("LoyaltyInfo", &pLoyaltyInfo);
	xmlInitDoc->AppendChild(&pLoyaltyInfo, pCurrElement);
	CreateXmlFromStructLoyaltyInfo3WithOutNodes(pszXMLOut, &StructIn->cDiscountLoyaltyInfo3, xmlInitDoc, &pLoyaltyInfo);

	xmlInitDoc->CreateElement("CustomerInformation", &pCustomerInfo);
	xmlInitDoc->AppendChild(&pCustomerInfo, pCurrElement);
	CreateXmlFromStructLoyaltyCardDetailsWithOutNodes(pszXMLOut, &StructIn->sLoyaltyCard, xmlInitDoc, &pCustomerInfo);
}

