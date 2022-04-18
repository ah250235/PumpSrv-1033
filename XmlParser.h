// XmlParser.h: interface for the CXmlParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLPARSER_H__5A0003A6_9AB5_4DC5_9B20_655986058147__INCLUDED_)
#define AFX_XMLPARSER_H__5A0003A6_9AB5_4DC5_9B20_655986058147__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ForecourtProductList.h"
#include "AbstractDBManager.h"

class CXmlParser  
{
public:
	CXmlParser();
	virtual ~CXmlParser();
	void AddLineWithNulll(CXMLInterface *xmlInitDoc, CXMLPointers *pCurrElement, char *psAttributeName, BYTE *value, long lStrLen);
	void AddNullTerminatedNode(CXMLInterface *xmlInitDoc, CXMLPointers *pParrentElement, char *psAttributeName, BYTE *value, long lStrLen);//MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
	BOOL CreateAddCarWashItemXML(PAY_AT_PUMP_INFO & cTmpPapInfo, char *szStrOutXml, long lPumpNumber);
	BOOL CreateQueryXML(long lPumpNumber , PumpItems * cTmpInfo,char * szStrOutXml,PAY_AT_PUMP_INFO & pInfo, BOOL bAddCarWashBeforeFueling);
	long GetDepCode(){ return m_lDepCode;};
	void SetDepCode(long lVal){if (lVal >= 0) m_lDepCode = lVal;};
	void CreateAddMemberInfoXml(char * szStrOutXml);
	BOOL ParseTargetMessage(char * sTargetMessageXml, char * sOutText, int iBufferSize);
	void ParseAutoStartFlag( long lPumpNumber ,long lPumpAutoStart, CXMLInterface &xmlInitDoc, CXMLPointers pPumpAutoStartNode, PAY_AT_PUMP_INFO &cTmpInfo);
	void BuildApprovalCommunicationMessage( CXMLInterface &xmlInitDoc,  long lPromID );
	BOOL ParseLoyaltySummaryXml(char * szLoyaltyFile, long & lPointsEarned, long &lOpenBalance, long & lEndningBalance); // 4.0.32.220 TD 405631
	BOOL ParseGetMemberInfoMessage(char * sGetMemberInfoXml, PAY_AT_PUMP_INFO *pInfo); // CR 405435
	void ParseStartTikcketTime(long lPumpNumber, CXMLInterface &xmlInitDoc);
	BOOL ParseRemainingValueToTrigger(IN CComBSTR bstrOutIncentiveMessage, OUT long & lRemainingValueToTrigger); // TD 466954
	BOOL CreateEmeraldFuelProductDataXML(BSTR& sXMLInData, CComBSTR& sXMLOutData, long lRequestId, long lPumpNumber, EmeraldLoyaltyRequestType eRequestType);  // MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
	BOOL CreateEmeraldCancelTicketQueryXML(long lPumpNumber, CComBSTR& sXMLOutData, PAY_AT_PUMP_INFO & pInfo , long lRequestId );
	BOOL CreateEmeraldAddCarWashItemXML(CComBSTR& sXMLInOutData, long lUnitPrice, long	lQuantity, __int64 lPluCode, CString sName); //RFUEL - 2142
	BOOL AddCustomerInformationToXML(long lPumpNumber, CXMLInterface  &xmlOutDataDoc, CXMLPointers   &pRootElement, BOOL bDeleteCustomerLoyaltyFile=TRUE );   // RFUEL - 2628 Fuel do not send Customer information in PAP / PAK transaction with loyalty
private:
	long m_lDepCode;
};

#endif // !defined(AFX_XMLPARSER_H__5A0003A6_9AB5_4DC5_9B20_655986058147__INCLUDED_)
