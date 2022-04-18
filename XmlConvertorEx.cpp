// XmlConvertorEx.cpp: implementation of the CXmlConvertorEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XmlConvertorEx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CXmlConvertorEx::CXmlConvertorEx()
{

}

CXmlConvertorEx::~CXmlConvertorEx()
{

}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL3* StructOut)
{
	return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}


BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL2* StructOut)
{
	return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL * StructOut)
{
	return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_DATA * StructOut)
{
	return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA * StructOut)
{
	return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA2_ * StructOut)
{
    return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA3_ * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, GRADE_LIMITS  * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, DISCNT * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, SALE * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}


BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT * StructOut)
{
	return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, BAP_ITEM_FOR_RECEIPT2 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_GENERAL_INFO * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, RECEIPT_EXTRA_INFO * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA5 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA4 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA3 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_RECEIPT_GENERAL_DATA * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_SYSTEMTIME * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_PUMP_TRANSACT2 * StructOut)
{

	BOOL			l_bReturn = TRUE;
	BOOL			l_bFoundCardData, l_bFoundTransaction;
	CXMLInterface	xmlInitDoc;
	CXMLPointers	pChildNode, pChildNode2;
	
	//create the xml for the received buffer. 
	l_bReturn = xmlInitDoc.InitDoc();
	if(l_bReturn == TRUE)
	{
		l_bReturn = xmlInitDoc.LoadXmlString(pszXMLIn);
		if (!l_bReturn)
		{
			WriteToLog("Failed to load XML !!!",TRUE);
		}
	}
	
	//strat parsing the xml
	if (l_bReturn)
	{

		l_bFoundCardData = xmlInitDoc.SelectSingleElementNode("CardData", &pChildNode, NULL);

		if (l_bFoundCardData)
		{
			//check if the xml has a child with the name Transaction.
			//l_bFoundTransaction = xmlInitDoc.SelectSingleElementNode(ELEMENT_TRANSACTION_ONLY, &pChildNode, &pChildNode);
			l_bFoundTransaction = xmlInitDoc.SelectSingleElementNode(ELEMENT_TRANSACTION_ONLY, &pChildNode, NULL); 
				
				
			if (l_bFoundTransaction) 
			{
				CreateStructTagPumpTransact2FromXmlWithOutNodes(pszXMLIn, (TAG_PUMP_TRANSACT2*) StructOut, &pChildNode, &xmlInitDoc);
			}
		}
	}
	
	xmlInitDoc.CloseDoc();
	return l_bReturn;

//   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}
//TD 38654
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_PUMP_TRANSACT5 * StructOut)
{

	BOOL			l_bReturn = TRUE;
	BOOL			l_bFoundCardData, l_bFoundTransaction;
	CXMLInterface	xmlInitDoc;
	CXMLPointers	pChildNode, pChildNode2;
	
	//create the xml for the received buffer. 
	l_bReturn = xmlInitDoc.InitDoc();
	if(l_bReturn == TRUE)
	{
		l_bReturn = xmlInitDoc.LoadXmlString(pszXMLIn);
		if (!l_bReturn)
		{
			WriteToLog("Failed to load XML !!!",TRUE);
		}
	}
	
	//strat parsing the xml
	if (l_bReturn)
	{

		l_bFoundCardData = xmlInitDoc.SelectSingleElementNode("CardData",  &pChildNode, NULL);

		if (l_bFoundCardData)
		{
			//check if the xml has a child with the name Transaction.
			//l_bFoundTransaction = xmlInitDoc.SelectSingleElementNode(ELEMENT_TRANSACTION_ONLY, &pChildNode, &pChildNode);
			l_bFoundTransaction = xmlInitDoc.SelectSingleElementNode(ELEMENT_TRANSACTION_ONLY, &pChildNode, NULL); 
				
				
			if (l_bFoundTransaction) 
			{
				CreateStructTagPumpTransact5FromXmlWithOutNodes(pszXMLIn, (TAG_PUMP_TRANSACT5*) StructOut, &pChildNode, &xmlInitDoc);
			}
		}
	}
	xmlInitDoc.CloseDoc();
	return l_bReturn;

//   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

//4.0.22.511 71868
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC12 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

//4.0.19.504
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC11 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

//4.0.19.504
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC10 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}


BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC9 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC9_EX * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC8 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC8_EX * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC7 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC7_EX * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC6 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC6_EX * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC5 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC5_EX * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC4 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC4_EX * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC3 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC3_EX * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAX_DATA_SUMMARY * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_PLUS * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, RECEIPT_DATA4 * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}
//TD 38654
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, RECEIPT_DATA4_EX * StructOut)
{
   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, PAY_AT_PUMP_INFO * StructOut)
{
	BOOL l_bReturn;
	try	{
		l_bReturn = CreateStructEx(pszXMLIn, StructOut, PAY_AT_PUMP_INFO_FLAG);
	}
	catch (...){
		l_bReturn = FALSE;
	}	
	return l_bReturn;
}

//4.0.16.504
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_ * StructOut)
{
	   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);

}

//4.0.18.501
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA5_ * StructOut)
{
	   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}


//4.0.16.504
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, DALLAS_KEYS_INFO * StructOut)
{
	   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);

}

//4.0.20.503 66960
BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_ALL_CHIP_EXTRA_DATA * StructOut)
{
	   return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, CARD_SALE_EXTRA_DATA6_ * StructOut)
{
	return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

BOOL CXmlConvertorEx::ConvertFromXmlToStruct(char * pszXMLIn, TAG_OLA_REC13 * StructOut)
{
	return this->CXmlConvertor::ConvertFromXmlToStruct(pszXMLIn, StructOut);
}

/////////////////////////////////////////////////////////////////////////

void EndXmlLineWithNullEx(CXMLInterface * xmlInitDoc, CXMLPointers * pCurrElement, char * psAttributeName, BYTE  * value, unsigned int uStrLen)
{
	unsigned int i = 0;
	unsigned char  *tmpStr;
	BOOL	 bValid = TRUE;
	tmpStr = new unsigned char[uStrLen+1];

	tmpStr[uStrLen] = 0;
	memcpy(tmpStr, value, uStrLen);
	for(i = 0; i < uStrLen; i++)
	{
		if(tmpStr[i] != ' ')
		{
			bValid = FALSE;
			break;
		}
	}
	if(!bValid)
		xmlInitDoc->SetCharAttribute(psAttributeName, (char *)tmpStr, pCurrElement);

	delete[] tmpStr;
}

/////////////////////////////////////////////////////////////////////////

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_ALL3 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_ALL2 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_ALL * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_DATA * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA2_ * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA3_ * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, GRADE_LIMITS * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, DISCNT * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, SALE * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, BAP_ITEM_FOR_RECEIPT2 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, BAP_ALL_ITEMS_FOR_RECEIPT2 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_GENERAL_INFO * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, RECEIPT_EXTRA_INFO * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA5 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA4 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA3 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_RECEIPT_GENERAL_DATA * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_SYSTEMTIME * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_PUMP_TRANSACT2 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_PUMP_TRANSACT5 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//4.0.22.511 71868
BOOL CXmlConvertorEx::ConvertFromStructToXml(char* pszXMLOut, TAG_OLA_REC12* StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//4.0.20.501 TD 60236
BOOL CXmlConvertorEx::ConvertFromStructToXml(char* pszXMLOut, TAG_OLA_REC11* StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}


//4.0.17.501 - CR 38162
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC10 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC9 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC9_EX * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC8 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC8_EX * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC7 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC7_EX * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC6 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC6_EX * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC5 * StructIn)
{
	return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC5_EX * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC4 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC4_EX * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC3 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC3_EX * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAX_DATA_SUMMARY * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA4_PLUS * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, RECEIPT_DATA4 * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//TD 38654
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, RECEIPT_DATA4_EX * StructIn)
{
   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}


BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, PAY_AT_PUMP_INFO * StructIn)
{
	BOOL l_bReturn = TRUE;
	try {
		l_bReturn = CreateXmlEx(pszXMLOut, StructIn, PAY_AT_PUMP_INFO_FLAG);
	}
	catch (...) {
		l_bReturn = FALSE;
	}	
	return l_bReturn;
}

//4.0.16.504
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA4_ * StructIn)
{
	   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);

}

//4.0.18.501
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA5_ * StructIn)
{
	   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);

}

//4.0.16.504
BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, DALLAS_KEYS_INFO * StructIn)
{
	   return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);

}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_OLA_REC13 * StructIn )
{
	return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO4 * StructIn)
{
	return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);

}

BOOL CXmlConvertorEx::ConvertFromStructToXml(OUT char * pszXMLOut, IN TAG_EXTRA_INFO * StructIn)
{
	return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);

}

void CXmlConvertorEx::CreateXMLFromStructDallasKeysInfo(char * pszXMLOut , DALLAS_KEYS_INFO * StructIn, CXMLInterface * xmlInitDoc ,CXMLPointers *pCurrElement)
{
	   this->CXmlConvertor::CreateXMLFromStructDallasKeysInfo(pszXMLOut, StructIn, xmlInitDoc, pCurrElement);

}
void CXmlConvertorEx::CreateXMLFromStructCardSaleExtraData4(char * pszXMLOut, CARD_SALE_EXTRA_DATA4_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	   this->CXmlConvertor::CreateXMLFromStructCardSaleExtraData4(pszXMLOut, StructIn, xmlInitDoc, pCurrElement);

}

//4.0.18.501
void CXmlConvertorEx::CreateXMLFromStructCardSaleExtraData5(char * pszXMLOut, CARD_SALE_EXTRA_DATA5_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{  
	this->CXmlConvertor::CreateXMLFromStructCardSaleExtraData5(pszXMLOut, StructIn, xmlInitDoc, pCurrElement);
}

BOOL CXmlConvertorEx::ConvertFromStructToXml(char * pszXMLOut, CARD_SALE_EXTRA_DATA6_ * StructIn)
{
	return this->CXmlConvertor::ConvertFromStructToXml(pszXMLOut, StructIn);
}

//4.0.18.501
void CXmlConvertorEx::CreateXMLFromStructCardSaleExtraData6(char * pszXMLOut, CARD_SALE_EXTRA_DATA6_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{  
	this->CXmlConvertor::CreateXMLFromStructCardSaleExtraData6(pszXMLOut, StructIn, xmlInitDoc, pCurrElement);
}

void CXmlConvertorEx::CreateXmlFromStructLoyaltyInfo4WithOutNodes(char * pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO4 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	this->CXmlConvertor::CreateXmlFromStructLoyaltyInfo4WithOutNodes(pszXMLOut, StructIn, xmlInitDoc, pCurrElement);  // RFUEL-520
}

void CXmlConvertorEx::CreateXmlFromStructLoyaltyInfo3WithOutNodes(char * pszXMLOut, TAG_DISCOUNT_LOYALTY_INFO3 * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement) 
{
	this->CXmlConvertor::CreateXmlFromStructLoyaltyInfo3WithOutNodes(pszXMLOut, StructIn, xmlInitDoc, pCurrElement); // RFUEL-520
}

//4.0.14.500
GCIRetCode CXmlConvertorEx::GetXmlInterface(BSTR sData, long lOlaRecSize, CXMLInterface* cXmlInterface, int* ilen)
{

	char sXmlBuff[MAX_XML_BUF_LEN];
	int iLen = 0;
	CARD_SALE_ALL3 cardDataAll3;
	GCIRetCode lRetCode = GCI_OK;

	memset(&sXmlBuff, 0, MAX_XML_BUF_LEN);
	memset(&cardDataAll3, 0, sizeof(CARD_SALE_ALL3));

	//if the buffer is not as xml, convert it to xml
	if (!(_Module.m_server.m_bUseXmlBuffer))
	{

		iLen = Bstr2Buffer(sData, (void *) &cardDataAll3, lOlaRecSize);

		if(iLen == 0)
		{
			lRetCode = GCI_LAST_ERROR;
			WriteToLog("Problem with the received buffer !!!",TRUE);
		}
		else
		{
			//Convert any 0 in the buffer to ' ' 
			BYTE *pTmp = (BYTE *) &cardDataAll3;
			for(int i = 0; i < lOlaRecSize; i++)
				if( pTmp[i] == 0) pTmp[i] = ' ';
				
			//rttttt temp!!!!!!!! remove after fix
			if (cardDataAll3.extData3.sPreSetlimitType < 48)				
				cardDataAll3.extData3.sPreSetlimitType += 48;

			//Convert the struct to xml
			if (!(ConvertFromStructToXml(sXmlBuff, &cardDataAll3)))
			{
				lRetCode = GCI_FAILED_IO;
				WriteToLog("Problem with converting from struct to xml!!!", TRUE);
			}
		}

	}
	else
	{
		//The buffer is already in xml format
		iLen = Bstr2Buffer(sData, (void *) &sXmlBuff, MAX_XML_BUF_LEN);
		if(iLen == 0)
		{
			WriteToLog("Problem with the received xml buffer !!!", TRUE);
			lRetCode = GCI_LAST_ERROR;
		}
		
	}

	if (lRetCode == GCI_OK)
	{
		//Load the xml buffer we got to xml interface
		
		if (sXmlBuff[0])
		{
			if (!(cXmlInterface->LoadXmlString((char*)&sXmlBuff)))
			{
				_LOGMSG.LogXmlOLABufferMsg("XmlProblem: GetXmlInterface, LoadXmlString", cXmlInterface, LOG_LEVEL_1);		// TD 389915
				lRetCode = GCI_FAILED_IO; 
			}
		}
		else
		{
			_LOGMSG.LogMsg("The XmlBuffer is empty",LOG_LEVEL_1);
		}
	}

	return lRetCode;

}

//4.0.20.503 66960
GCIRetCode CXmlConvertorEx::GetStructCardSaleAll3ChipExtra(BSTR sData, long lOlaRecSize, CARD_SALE_ALL_CHIP_EXTRA_DATA* cOutDataAll3ChipExtra, int* ilen)
{
	char sXmlBuff[MAX_XML_BUF_LEN];
	int iLenTmp = 0;							//4.0.24.90
	GCIRetCode lRetCode = GCI_OK;

	memset(&sXmlBuff, 0, MAX_XML_BUF_LEN);

	//if the buffer is not as xml, convert it to xml
	if (!(_Module.m_server.m_bUseXmlBuffer))
	{

		iLenTmp = Bstr2Buffer(sData, (void *)cOutDataAll3ChipExtra, lOlaRecSize);  //4.0.24.90

		if(iLenTmp == 0)   //4.0.24.90
		{
			lRetCode = GCI_LAST_ERROR;
			WriteToLog("Problem with the received buffer !!!",TRUE);
		}
		else
		{
			//Convert any 0 in the buffer to ' ' 
			BYTE *pTmp = (BYTE *) &cOutDataAll3ChipExtra->cCardSaleAll3;
			for(int i=0;i < lOlaRecSize; i++)
				if( pTmp[i] == 0) pTmp[i] = ' ';
				
			//rttttt temp!!!!!!!! remove after fix
			if (cOutDataAll3ChipExtra->cCardSaleAll3.extData3.sPreSetlimitType < 48)				
				cOutDataAll3ChipExtra->cCardSaleAll3.extData3.sPreSetlimitType += 48;

		}
	}
	else
	{
		//The buffer is in xml format
		iLenTmp = Bstr2Buffer(sData, (void *) &sXmlBuff, MAX_XML_BUF_LEN);	//4.0.24.90
		if(iLenTmp == 0)													//4.0.24.90
		{
			WriteToLog("Problem with the received xml buffer !!!",TRUE);
			lRetCode = GCI_LAST_ERROR;
		}
		else
		{
			memset(cOutDataAll3ChipExtra, ' ', sizeof(CARD_SALE_ALL_CHIP_EXTRA_DATA));
			//Convert the xml to struct
			if (!(ConvertFromXmlToStruct(sXmlBuff, cOutDataAll3ChipExtra)))
			{
				lRetCode = GCI_FAILED_IO;
				WriteToLog("Problem with converting from xml to struct!!!",TRUE);
			}

		}
		
	}

	*ilen = iLenTmp; //4.0.24.90

	return lRetCode;

}

//////////////////////////////////////////////////////////

BOOL CXmlConvertorEx::CreateStructEx(char * pszXMLIn, void * vpStructOut, __int64 lXmlFlags)
{
	BOOL			l_bReturn = TRUE;
	BOOL			l_bFoundCardData;
	CXMLInterface	xmlInitDoc;
	CXMLPointers	pChildNode, pChildNode2;
	
	//create the xml for the received buffer. 
	l_bReturn = xmlInitDoc.InitDoc();
	if(l_bReturn == TRUE)
	{
		l_bReturn = xmlInitDoc.LoadXmlString(pszXMLIn);
		if (!l_bReturn)
		{
			WriteToLog("Failed to load XML !!!",TRUE);
		}
	}
	
	//check if the xml root name is FuelTransactionData
	if (l_bReturn)
	{
		CString sName;
		l_bReturn = xmlInitDoc.GetName(sName);
		if (l_bReturn)
		{
			if (strcmp("FuelTransactionData",sName))
			{
				WriteToLog("The XML root name is not FuelTransactionData - Not parsing the xml !!!");
				l_bReturn = FALSE;
			}
			/*else
				WriteToLog("CXmlConvertorEx::Start parsing the xml.");*/
		}
	}
	
	//strat parsing the xml
	if (l_bReturn)
	{
		if (lXmlFlags & PAY_AT_PUMP_INFO_FLAG)
		{
			//check if the xml has a child with the name CardData.
			l_bFoundCardData = xmlInitDoc.SelectSingleElementNode("CardData", &pChildNode, NULL);

			if (l_bFoundCardData)
			{
				if (lXmlFlags & PAY_AT_PUMP_INFO_FLAG)
				{
					CreateStructPayAtPumpInfoFromXmlWithOutNodes(pszXMLIn, (PAY_AT_PUMP_INFO*) vpStructOut, &pChildNode, &xmlInitDoc);
				}
				
			}
		}
	}
	xmlInitDoc.CloseDoc();
	return l_bReturn;
}



void CXmlConvertorEx::CreateStructPayAtPumpInfoFromXmlWithOutNodes(char * pszXMLIn, PAY_AT_PUMP_INFO * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	CXMLPointers	pChildNode, pChildNode2, pChildNode3;

	CreateStructCardSaleInfoFromXmlWithOutNodes(pszXMLIn, &StructOut->CardSaleInfo, pXml, xmlInitDoc, &StructOut->CardSaleExtraData2, &StructOut->CardSaleExtraData3); //4.0.15.502	//4.0.18.670 73612 4.0.22.502
	CreateStructCardSaleExtraData2FromXmlWithOutNodes(pszXMLIn, &StructOut->CardSaleExtraData2, pXml, xmlInitDoc);
	CreateStructCardSaleExtraData3FromXmlWithOutNodes(pszXMLIn, &StructOut->CardSaleExtraData3, pXml, xmlInitDoc);
	CreateStructCardSaleExtraData4FromXmlWithOutNodes(pszXMLIn, &StructOut->CardSaleExtraData4, pXml, xmlInitDoc);//4.0.16.504
	CreateStructCardSaleExtraData5FromXmlWithOutNodes(pszXMLIn, &StructOut->CardSaleExtraData5, pXml, xmlInitDoc);//4.0.23.1461 323583
	CreateStructCardSaleExtraData6FromXmlWithOutNodes(pszXMLIn, &StructOut->CardSaleExtraData6, pXml, xmlInitDoc);//4.0.23.1461 323583

}

void CXmlConvertorEx::CreateStructCardSaleInfoFromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_INFO_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc, CARD_SALE_EXTRA_DATA2_ * StructOut2, CARD_SALE_EXTRA_DATA3_ * StructOut3) //4.0.15.502	//4.0.18.670 73612 4.0.22.502
{
	StructOut->lIndexNumber = xmlInitDoc->GetAttributeToLong("IndexNumber",pXml);
//	CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &StructOut->cardData, pXml, xmlInitDoc, NULL, StructOut2); //4.0.15.502
	CreateStructCardSaleDataFromXmlWithOutNodes(pszXMLIn, &StructOut->cardData, pXml, xmlInitDoc, &StructOut->extraData, StructOut2); //4.0.16.505
	CreateStructCardSaleExtraDataFromXmlWithOutNodes(pszXMLIn, &StructOut->extraData, pXml, xmlInitDoc, StructOut3);	//4.0.18.670 73612 4.0.22.502
}

//4.0.16.504
void CXmlConvertorEx::CreateStructDallasKeysInfoFromXmlWithOutNodes(char * pszXMLIn, DALLAS_KEYS_INFO * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	   this->CXmlConvertor::CreateStructDallasKeysInfoFromXmlWithOutNodes(pszXMLIn, StructOut, pXml, xmlInitDoc);

}
//4.0.16.504
void CXmlConvertorEx::CreateStructCardSaleExtraData4FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA4_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	   this->CXmlConvertor::CreateStructCardSaleExtraData4FromXmlWithOutNodes(pszXMLIn, StructOut, pXml, xmlInitDoc);

}

//4.0.18.501
void CXmlConvertorEx::CreateStructCardSaleExtraData5FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA5_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	   this->CXmlConvertor::CreateStructCardSaleExtraData5FromXmlWithOutNodes(pszXMLIn, StructOut, pXml, xmlInitDoc);
}

void CXmlConvertorEx::CreateStructCardSaleExtraData6FromXmlWithOutNodes(char * pszXMLIn, CARD_SALE_EXTRA_DATA6_ * StructOut, CXMLPointers *pXml, CXMLInterface *xmlInitDoc)
{
	this->CXmlConvertor::CreateStructCardSaleExtraData6FromXmlWithOutNodes(pszXMLIn, StructOut, pXml, xmlInitDoc);
}

/////////////////////////////////////////////////////////////////////

BOOL CXmlConvertorEx::CreateXmlEx(char * pszXMLOut, void * vpStructIn, __int64 lXmlFlags)
{
	BOOL			bRetVal = TRUE;
	CXMLInterface	xmlInitDoc;
	CXMLPointers	pCurrElement;
	CXMLPointers	pChildNode, pChildNode2, pChildNode3, pChildNode4, pChildNode5;
	CComBSTR		bstrXML;
	char tmp [50000];
	
	bRetVal = xmlInitDoc.InitDoc();
	if (!bRetVal)
		return bRetVal;

	//create the root and append it to the XML.
	xmlInitDoc.CreateElement("FuelTransactionData",&pCurrElement);
	xmlInitDoc.AppendRootElement(&pCurrElement);

	if (lXmlFlags & PAY_AT_PUMP_INFO_FLAG)
	{
		//Create CardDataNode
		xmlInitDoc.CreateElement("CardData", &pChildNode);
		xmlInitDoc.AppendChild(&pChildNode, &pCurrElement);

		if (lXmlFlags & PAY_AT_PUMP_INFO_FLAG)
		{
			PAY_AT_PUMP_INFO StructIn;
			memset(&StructIn, 0x00, sizeof(StructIn));
			memcpy(&StructIn, vpStructIn, sizeof(StructIn));

			CreateXMLFromStructPayAtPumpInfo(pszXMLOut, &StructIn, &xmlInitDoc, &pChildNode);
		}
		
	}

	xmlInitDoc.GetXmlString(&bstrXML);
	int iLen = WideCharToMultiByte(CP_ACP, 0, bstrXML, -1, tmp, sizeof(tmp), NULL, NULL);
	strcpy(pszXMLOut, tmp);
#ifdef DEBUG
	CString sFileNameAndPath = _Module.m_server.m_cLogger.GetDefaultLogPath() + "\\FuelTransactionData.xml";
	xmlInitDoc.SaveToFile(sFileNameAndPath);
#endif // // DEBUG
	xmlInitDoc.CloseDoc();
	return bRetVal;
}



void CXmlConvertorEx::CreateXMLFromStructPayAtPumpInfo(char * pszXMLOut, PAY_AT_PUMP_INFO * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement)
{
	CreateXMLFromStructCardSaleInfo(pszXMLOut, &StructIn->CardSaleInfo, xmlInitDoc, pCurrElement, &StructIn->CardSaleExtraData2, &StructIn->CardSaleExtraData3); //4.0.15.502	//4.0.18.670 73612 4.0.22.502
	CreateXMLFromStructCardSaleExtraData2(pszXMLOut, &StructIn->CardSaleExtraData2, xmlInitDoc, pCurrElement);
	CreateXMLFromStructCardSaleExtraData3(pszXMLOut, &StructIn->CardSaleExtraData3, xmlInitDoc, pCurrElement);
	CreateXMLFromStructCardSaleExtraData4(pszXMLOut, &StructIn->CardSaleExtraData4, xmlInitDoc, pCurrElement); //4.0.16.504
	CreateXMLFromStructCardSaleExtraData5(pszXMLOut, &StructIn->CardSaleExtraData5, xmlInitDoc, pCurrElement); //4.0.23.1461 323583
	CreateXMLFromStructCardSaleExtraData6(pszXMLOut, &StructIn->CardSaleExtraData6, xmlInitDoc, pCurrElement); //4.0.23.1461 323583
}

void CXmlConvertorEx::CreateXMLFromStructCardSaleInfo(char * pszXMLOut, CARD_SALE_INFO_ * StructIn, CXMLInterface * xmlInitDoc, CXMLPointers *pCurrElement, CARD_SALE_EXTRA_DATA2_ * StructIn2, CARD_SALE_EXTRA_DATA3_ * StructIn3) //4.0.15.502	//4.0.18.670 73612 4.0.22.502
{

	BYTE sIndexNum[20];
	memset(sIndexNum, 0, sizeof(sIndexNum));
	l2a(StructIn->lIndexNumber, sIndexNum, sizeof(sIndexNum));

	EndXmlLineWithNullEx(xmlInitDoc, pCurrElement, "IndexNumber", sIndexNum, sizeof(sIndexNum));
//	CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn->cardData, xmlInitDoc, pCurrElement, NULL, StructIn2); //4.0.15.502
	CreateXMLFromStructCardSaleData(pszXMLOut, &StructIn->cardData, xmlInitDoc, pCurrElement, &StructIn->extraData, StructIn2); //4.0.15.505
	CreateXMLFromStructCardSaleExtraData(pszXMLOut, &StructIn->extraData, xmlInitDoc, pCurrElement, StructIn3);	//4.0.18.670 73612 4.0.22.502

}



/******************************************************************************
 Description:	 Append the "trnasaction" brance according to TAG_PUMP_TRANSACT2
				 struct to an existing xml file.
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			15/10/2006   12:26		Start
 Amit H			02/05/2007   18:00		Continue - change the method to EndXmlLineWithNullEx //4.0.17.20 - Merge from 1015
******************************************************************************/

BOOL CXmlConvertorEx::AppendTransactionStructToXml(CXMLInterface * xmlInitDocInOut, TAG_PUMP_TRANSACT2 * cTagPumpTrsIn)
{
	BOOL			bRetVal = FALSE;
	CXMLPointers	pChildNode;

	if (!xmlInitDocInOut)
		bRetVal = TRUE;
		

	BOOL bFoundTransaction = xmlInitDocInOut->SelectSingleElementNode(ELEMENT_TRANSACTION_ONLY, &pChildNode, NULL); 
				
	if (bFoundTransaction) 
	{
		//50742 - remove all "transaction\\"
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Number", (cTagPumpTrsIn->sNumber), sizeof(cTagPumpTrsIn->sNumber));		
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Status", (cTagPumpTrsIn->sStatus), sizeof(cTagPumpTrsIn->sStatus));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "OnOffer", (cTagPumpTrsIn->sOnOffer), sizeof(cTagPumpTrsIn->sOnOffer));;
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "RawVolume", (cTagPumpTrsIn->sRawVolume), sizeof(cTagPumpTrsIn->sRawVolume));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "RoundedVolume", (cTagPumpTrsIn->sRoundedVolume), sizeof(cTagPumpTrsIn->sRoundedVolume));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "RoundedValue", (cTagPumpTrsIn->sRoundedValue), sizeof(cTagPumpTrsIn->sRoundedValue));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "TRGradePrice", (cTagPumpTrsIn->sGradePrice), sizeof(cTagPumpTrsIn->sGradePrice));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "TRNozzle", (cTagPumpTrsIn->sNozzle), sizeof(cTagPumpTrsIn->sNozzle));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Grade", (cTagPumpTrsIn->sGrade), sizeof(cTagPumpTrsIn->sGrade));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Level", (cTagPumpTrsIn->sLevel), sizeof(cTagPumpTrsIn->sLevel));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Pump", (cTagPumpTrsIn->sPump), sizeof(cTagPumpTrsIn->sPump));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Mode", (cTagPumpTrsIn->sMode), sizeof(cTagPumpTrsIn->sMode));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Res", (cTagPumpTrsIn->sRes), sizeof(cTagPumpTrsIn->sRes));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "ResExt", (cTagPumpTrsIn->sResExt), sizeof(cTagPumpTrsIn->sResExt));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "LinkNumber", (cTagPumpTrsIn->sLinkNumber), sizeof(cTagPumpTrsIn->sLinkNumber));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "LinkFlags", (cTagPumpTrsIn->sLinkFlags), sizeof(cTagPumpTrsIn->sLinkFlags));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "ActiveReadNumber", (cTagPumpTrsIn->sActiveReadNumber), sizeof(cTagPumpTrsIn->sActiveReadNumber));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "ActiveShiftNumber", (cTagPumpTrsIn->sActiveShiftNumber), sizeof(cTagPumpTrsIn->sActiveShiftNumber));

		BYTE tempMisch [STR_LONG_VAL5+STR_LONG_VAL5] = {0}; 

		//connecting the values of sMiscH and sMisc to one.
		memcpy(tempMisch, cTagPumpTrsIn->sMiscH, sizeof(cTagPumpTrsIn->sMiscH));
		memcpy(tempMisch + STR_LONG_VAL5, cTagPumpTrsIn->sMisc, sizeof(cTagPumpTrsIn->sMisc));

		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode,"Misc",			(tempMisch),						sizeof(tempMisch));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode,"PrePaidValue",	(cTagPumpTrsIn->sPrePaidValue),		sizeof(cTagPumpTrsIn->sPrePaidValue));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode,"ServiceMode",	(cTagPumpTrsIn->sServiceMode),		sizeof(cTagPumpTrsIn->sServiceMode));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode,"Offers2Pos",		&cTagPumpTrsIn->sOffers2Pos,		sizeof(cTagPumpTrsIn->sOffers2Pos));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode,"Grade2",			(cTagPumpTrsIn->sGrade2),			sizeof(cTagPumpTrsIn->sGrade2));

		CXMLPointers	tagSysTimeXml2;
		BOOL bFoundSysTimeInTrs = xmlInitDocInOut->SelectSingleElementNode("TAG_SYSTEMTIME", &tagSysTimeXml2, &pChildNode ); 

		if(bFoundSysTimeInTrs)
		{			
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2,"Year",	(cTagPumpTrsIn->sDateTime.sYear),	sizeof(cTagPumpTrsIn->sDateTime.sYear));		  
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2,"Month",	(cTagPumpTrsIn->sDateTime.sMonth),	sizeof(cTagPumpTrsIn->sDateTime.sMonth));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2,"Day",	(cTagPumpTrsIn->sDateTime.sDay),	sizeof(cTagPumpTrsIn->sDateTime.sDay));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2,"Hour",	(cTagPumpTrsIn->sDateTime.sHour),	sizeof(cTagPumpTrsIn->sDateTime.sHour));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2,"Minute",	(cTagPumpTrsIn->sDateTime.sMinute),	sizeof(cTagPumpTrsIn->sDateTime.sMinute));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2,"Second",	(cTagPumpTrsIn->sDateTime.sSecond),	sizeof(cTagPumpTrsIn->sDateTime.sSecond));
		}
	}
	return bRetVal;
}

//TD 38654
BOOL CXmlConvertorEx::AppendTransactionStructToXml(CXMLInterface * xmlInitDocInOut, TAG_PUMP_TRANSACT5 * cTagPumpTrsIn) //4.0.17.20 - Merge from 1015
{
	BOOL			bRetVal = FALSE;
	CXMLPointers	pChildNode;

	if (!xmlInitDocInOut)
		bRetVal = TRUE;
		

	BOOL bFoundTransaction = xmlInitDocInOut->SelectSingleElementNode(ELEMENT_TRANSACTION_ONLY, &pChildNode, NULL); 
				
	if (bFoundTransaction) 
	{
		//50742 - remove all "transaction\\"
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Number", (cTagPumpTrsIn->sNumber), sizeof(cTagPumpTrsIn->sNumber));		
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Status", (cTagPumpTrsIn->sStatus), sizeof(cTagPumpTrsIn->sStatus));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "OnOffer", (cTagPumpTrsIn->sOnOffer), sizeof(cTagPumpTrsIn->sOnOffer));;
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "RawVolume", (cTagPumpTrsIn->sRawVolume), sizeof(cTagPumpTrsIn->sRawVolume));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "RoundedVolume", (cTagPumpTrsIn->sRoundedVolume), sizeof(cTagPumpTrsIn->sRoundedVolume));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "RoundedValue", (cTagPumpTrsIn->sRoundedValue), sizeof(cTagPumpTrsIn->sRoundedValue));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "TRGradePrice", (cTagPumpTrsIn->sGradePrice), sizeof(cTagPumpTrsIn->sGradePrice));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "TRNozzle", (cTagPumpTrsIn->sNozzle), sizeof(cTagPumpTrsIn->sNozzle));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Grade", (cTagPumpTrsIn->sGrade), sizeof(cTagPumpTrsIn->sGrade));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Level", (cTagPumpTrsIn->sLevel), sizeof(cTagPumpTrsIn->sLevel));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Pump", (cTagPumpTrsIn->sPump), sizeof(cTagPumpTrsIn->sPump));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Mode", (cTagPumpTrsIn->sMode), sizeof(cTagPumpTrsIn->sMode));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Res", (cTagPumpTrsIn->sRes), sizeof(cTagPumpTrsIn->sRes));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "ResExt", (cTagPumpTrsIn->sResExt), sizeof(cTagPumpTrsIn->sResExt));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "LinkNumber", (cTagPumpTrsIn->sLinkNumber), sizeof(cTagPumpTrsIn->sLinkNumber));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "LinkFlags", (cTagPumpTrsIn->sLinkFlags), sizeof(cTagPumpTrsIn->sLinkFlags));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "ActiveReadNumber", (cTagPumpTrsIn->sActiveReadNumber), sizeof(cTagPumpTrsIn->sActiveReadNumber));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "ActiveShiftNumber", (cTagPumpTrsIn->sActiveShiftNumber), sizeof(cTagPumpTrsIn->sActiveShiftNumber));

		BYTE tempMisch [STR_LONG_VAL5+STR_LONG_VAL5] = {0}; 

		//connecting the values of sMiscH and sMisc to one.
		memcpy(tempMisch, cTagPumpTrsIn->sMiscH, sizeof(cTagPumpTrsIn->sMiscH));
		memcpy(tempMisch + STR_LONG_VAL5, cTagPumpTrsIn->sMisc, sizeof(cTagPumpTrsIn->sMisc));

		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Misc", (tempMisch),sizeof(tempMisch));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "PrePaidValue", (cTagPumpTrsIn->sPrePaidValue), sizeof(cTagPumpTrsIn->sPrePaidValue));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "ServiceMode", (cTagPumpTrsIn->sServiceMode), sizeof(cTagPumpTrsIn->sServiceMode));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Offers2Pos", &cTagPumpTrsIn->sOffers2Pos, sizeof(cTagPumpTrsIn->sOffers2Pos));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "Grade2",(cTagPumpTrsIn->sGrade2),sizeof(cTagPumpTrsIn->sGrade2));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "LowestGradePrice", (cTagPumpTrsIn->sLowestGradePrice), sizeof(cTagPumpTrsIn->sLowestGradePrice));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "LoyaltyPriceProtection", (&cTagPumpTrsIn->sLoyaltyPriceProtection), sizeof(cTagPumpTrsIn->sLoyaltyPriceProtection));
		EndXmlLineWithNullEx(xmlInitDocInOut, &pChildNode, "LoyaltyAutoArm", (&cTagPumpTrsIn->sLoyaltyAutoArm), sizeof(cTagPumpTrsIn->sLoyaltyAutoArm));
		
		CXMLPointers	tagSysTimeXml2;
		BOOL bFoundSysTimeInTrs = xmlInitDocInOut->SelectSingleElementNode("TAG_SYSTEMTIME", &tagSysTimeXml2, &pChildNode ); 

		if(bFoundSysTimeInTrs)
		{			
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2, "Year",		(cTagPumpTrsIn->sDateTime.sYear),	sizeof(cTagPumpTrsIn->sDateTime.sYear));		  
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2, "Month",		(cTagPumpTrsIn->sDateTime.sMonth),	sizeof(cTagPumpTrsIn->sDateTime.sMonth));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2, "Day",		(cTagPumpTrsIn->sDateTime.sDay),	sizeof(cTagPumpTrsIn->sDateTime.sDay));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2, "Hour",		(cTagPumpTrsIn->sDateTime.sHour),	sizeof(cTagPumpTrsIn->sDateTime.sHour));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2, "Minute",	(cTagPumpTrsIn->sDateTime.sMinute),	sizeof(cTagPumpTrsIn->sDateTime.sMinute));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml2, "Second",	(cTagPumpTrsIn->sDateTime.sSecond),	sizeof(cTagPumpTrsIn->sDateTime.sSecond));
		}

		CXMLPointers	tagSysTimeXml3;
		BOOL bFoundSysStartTimeInTrs = xmlInitDocInOut->SelectSingleElementNode("FlowStarted", &tagSysTimeXml3, &pChildNode); 

		if(bFoundSysStartTimeInTrs)
		{
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml3,	"Year",		(cTagPumpTrsIn->sStartDateTime.sYear),		sizeof(cTagPumpTrsIn->sStartDateTime.sYear));		  
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml3,	"Month",	(cTagPumpTrsIn->sStartDateTime.sMonth),		sizeof(cTagPumpTrsIn->sStartDateTime.sMonth));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml3,	"Day",		(cTagPumpTrsIn->sStartDateTime.sDay),		sizeof(cTagPumpTrsIn->sStartDateTime.sDay));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml3,	"Hour",		(cTagPumpTrsIn->sStartDateTime.sHour),		sizeof(cTagPumpTrsIn->sStartDateTime.sHour));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml3,	"Minute",	(cTagPumpTrsIn->sStartDateTime.sMinute),	sizeof(cTagPumpTrsIn->sStartDateTime.sMinute));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml3,	"Second",	(cTagPumpTrsIn->sStartDateTime.sSecond),	sizeof(cTagPumpTrsIn->sStartDateTime.sSecond));
		}
		//4.0.22.511 72606
		CXMLPointers tagSysTimeXml4;
		BOOL bFoundSysStartFuelingTimeInTrs = xmlInitDocInOut->SelectSingleElementNode("DispensingStarted", &tagSysTimeXml4, &pChildNode ); 

		if(bFoundSysStartFuelingTimeInTrs)
		{
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml4, "Year",		(cTagPumpTrsIn->sStartFuelingDateTime.sYear),	sizeof(cTagPumpTrsIn->sStartFuelingDateTime.sYear));		  
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml4, "Month",		(cTagPumpTrsIn->sStartFuelingDateTime.sMonth),	sizeof(cTagPumpTrsIn->sStartFuelingDateTime.sMonth));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml4, "Day",		(cTagPumpTrsIn->sStartFuelingDateTime.sDay),	sizeof(cTagPumpTrsIn->sStartFuelingDateTime.sDay));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml4, "Hour",		(cTagPumpTrsIn->sStartFuelingDateTime.sHour),	sizeof(cTagPumpTrsIn->sStartFuelingDateTime.sHour));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml4, "Minute",	(cTagPumpTrsIn->sStartFuelingDateTime.sMinute),	sizeof(cTagPumpTrsIn->sStartFuelingDateTime.sMinute));
			EndXmlLineWithNullEx(xmlInitDocInOut, &tagSysTimeXml4, "Second",	(cTagPumpTrsIn->sStartFuelingDateTime.sSecond),	sizeof(cTagPumpTrsIn->sStartFuelingDateTime.sSecond));
		}
	}

//	xmlInitDocInOut->SaveToFile("C:\\xxAppendTrsStrToXmlTagPumpTrs5.xml"); //Liat 2/5/07
	return bRetVal;
}

// 4.0.15.500
void CXmlConvertorEx::WriteToLog(char *sMsg, BOOL bWriteAlways)
{
	_LOGMSG.LogMsg(sMsg, LOG_LEVEL_3);
}


/******************************************************************************
 Description:	 Add an attribute to the XML
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat P			23/07/2007   12:00		Start
******************************************************************************/

BOOL CXmlConvertorEx::AddAttributeToXml(char * pszXMLIn, char * psAttributeName, char * psAttributeVal, long lAttributeSize, char * pszXMLOut)
{
	BOOL			bRetVal = TRUE;
	CXMLInterface	xmlInitDoc;
	CXMLPointers	pCurrElement;
	CXMLPointers	pChildNode, pChildNode2, pChildNode3, pChildNode4, pChildNode5;
	CComBSTR		bstrXML;
	char tmp [MAX_XML_BUF_LEN];
	
	bRetVal = xmlInitDoc.InitDoc();
	if (!bRetVal)
		return bRetVal;
	bRetVal = xmlInitDoc.LoadXmlString(pszXMLIn);
	if (bRetVal)
	{
		bRetVal = xmlInitDoc.SetXmlFieldWithSize(psAttributeName, (char*)psAttributeVal, lAttributeSize);
	}
	if (bRetVal)
	{
		memset(tmp, ' ', sizeof(tmp));
		xmlInitDoc.GetXmlString(&bstrXML);
		int iLen = WideCharToMultiByte(CP_ACP, 0, bstrXML, -1, tmp, sizeof(tmp), NULL, NULL);
		strcpy(pszXMLOut, tmp);
	}
	xmlInitDoc.CloseDoc();
	return bRetVal;

}
