// DebitInfo.cpp: implementation of the CDebitInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DebitInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDebitInfo::CDebitInfo()
{
	memset(m_cDebitProcessInfo.sData.byLen , '0' , sizeof(m_cDebitProcessInfo.sData.byLen));
	memset(m_cDebitProcessInfo.sData.byData, '0' , sizeof(m_cDebitProcessInfo.sData.byData));
	ClearOlaStat();
	m_bPendingInitDebitProcess = FALSE; //4.0.5.12
}

CDebitInfo::~CDebitInfo()
{

}

void CDebitInfo::ClearOlaStat()
{
	m_cOla.m_byState			= OLA_NONE;
	m_cOla.m_byCheckCount	= 0;
	m_cOla.m_lSessionNumber	= 0;
	memset(&m_cOla.m_cSysTime,0,sizeof(SYSTEMTIME));
}

void CDebitInfo::GetOlaStat(OLA_STAT *pOlaStat)
{
	*pOlaStat	 = m_cOla;
}

void CDebitInfo::SetOlaStat(OLA_STAT *pOlaStat)
{
	m_cOla = *pOlaStat;
}

void CDebitInfo::SetDebitInfo(OLA_DEBIT_PROCESS_INFO *pDebitInfo)
{
	m_cDebitProcessInfo = *pDebitInfo;
}

void CDebitInfo::GetDebitInfo(OLA_DEBIT_PROCESS_INFO *pDebitInfo)
{
	*pDebitInfo = m_cDebitProcessInfo;
}

void CDebitInfo::ConvertPAP2DebitInfo(PAY_AT_PUMP_INFO *pPAPInfo , long lFlags)
{
	memcpy(m_cDebitProcessInfo.sCalculatedMAC,  pPAPInfo->CardSaleExtraData2.sCalculatedMAC, sizeof(m_cDebitProcessInfo.sCalculatedMAC));
	memcpy(m_cDebitProcessInfo.sMACSessionKey , pPAPInfo->CardSaleExtraData2.sMACSessionKey, sizeof(m_cDebitProcessInfo.sMACSessionKey));
	m_cDebitProcessInfo.sPinPadAction = pPAPInfo->CardSaleExtraData2.sPinPadAction;
	m_cDebitProcessInfo.sPinPadResult = pPAPInfo->CardSaleExtraData2.sPinPadResult;
	memcpy(m_cDebitProcessInfo.sPinSessionKey , pPAPInfo->CardSaleExtraData2.sPinSessionKey , sizeof(m_cDebitProcessInfo.sPinSessionKey));	
	m_cDebitProcessInfo.sTranStatus = pPAPInfo->CardSaleInfo.cardData.sTranStatus;

	if (lFlags)
	{
		memcpy(m_cDebitProcessInfo.sToken , pPAPInfo->CardSaleExtraData2.sToken , sizeof(m_cDebitProcessInfo.sToken));	
	}
	
}

//4.0.14.500
void CDebitInfo::ConvertPAP2DebitInfo(CXMLInterface *pPAPInfo , long lFlags)
{

	char sVal[MAX_FIELD_VAL]; 

	memset(sVal,' ',sizeof(sVal));
	pPAPInfo->GetXmlField((char*)ATT_CALCULATED_MAC,sVal);
	memcpy(m_cDebitProcessInfo.sCalculatedMAC,sVal,sizeof(m_cDebitProcessInfo.sCalculatedMAC));

	memset(sVal,' ',sizeof(sVal));
	pPAPInfo->GetXmlField((char*)ATT_MAC_SESSION_KEY,sVal);
	memcpy(m_cDebitProcessInfo.sMACSessionKey,sVal,sizeof(m_cDebitProcessInfo.sMACSessionKey));

	m_cDebitProcessInfo.sPinPadAction = pPAPInfo->GetXmlByteField(ATT_PIN_PAD_ACTION);
	m_cDebitProcessInfo.sPinPadResult = pPAPInfo->GetXmlByteField(ATT_PIN_PAD_RESULT);

	memset(sVal,' ',sizeof(sVal));
	pPAPInfo->GetXmlField((char*)ATT_PIN_SESSION_KEY,sVal);
	memcpy(m_cDebitProcessInfo.sPinSessionKey,ATT_PIN_SESSION_KEY,sizeof(m_cDebitProcessInfo.sPinSessionKey));

	if (lFlags)
	{
		memset(sVal,' ',sizeof(sVal));
		pPAPInfo->GetXmlField((char*)ATT_TOKEN,sVal);
		memcpy(m_cDebitProcessInfo.sToken,ATT_TOKEN,sizeof(m_cDebitProcessInfo.sToken));	
	}
	
}

//4.0.18.501
void CDebitInfo::ConvertDebitInfo2PAP(PAY_AT_PUMP_INFO *pPAPInfo , long lPumpNumber)
{
	memset(pPAPInfo , ' ' , sizeof(PAY_AT_PUMP_INFO)); 

	l2a(lPumpNumber ,pPAPInfo->CardSaleInfo.cardData.sPumpNumber , sizeof(pPAPInfo->CardSaleInfo.cardData.sPumpNumber)); 
	memcpy(  pPAPInfo->CardSaleExtraData2.sCalculatedMAC, m_cDebitProcessInfo.sCalculatedMAC ,sizeof(pPAPInfo->CardSaleExtraData2.sCalculatedMAC));
	memcpy( pPAPInfo->CardSaleExtraData2.sMACSessionKey, m_cDebitProcessInfo.sMACSessionKey , sizeof(pPAPInfo->CardSaleExtraData2.sMACSessionKey));
	pPAPInfo->CardSaleExtraData2.sPinPadAction = m_cDebitProcessInfo.sPinPadAction; //MARIK12345
	pPAPInfo->CardSaleExtraData2.sPinPadResult = m_cDebitProcessInfo.sPinPadResult; //MARIK12345
	memcpy(pPAPInfo->CardSaleExtraData2.sPinSessionKey ,m_cDebitProcessInfo.sPinSessionKey ,  sizeof(pPAPInfo->CardSaleExtraData2.sPinSessionKey));	
	pPAPInfo->CardSaleInfo.cardData.sTranStatus = m_cDebitProcessInfo.sTranStatus;
}


void CDebitInfo::UpdateData(BYTE *byLen, BYTE *byData)
{
	memset(m_cDebitProcessInfo.sData.byLen , ' ' , sizeof(m_cDebitProcessInfo.sData.byLen));
	memset(m_cDebitProcessInfo.sData.byData, ' ' , sizeof(m_cDebitProcessInfo.sData.byData));
		
	if (a2l(byLen ,4))
	{
		memcpy(m_cDebitProcessInfo.sData.byLen , byLen , sizeof(m_cDebitProcessInfo.sData.byLen)); 
		memcpy(m_cDebitProcessInfo.sData.byData , byData , min((a2l(byLen ,4)), sizeof(m_cDebitProcessInfo.sData.byData))); 
	}
}

void CDebitInfo::GetData(BYTE *byLen, BYTE *byData)
{
	memcpy(byLen , m_cDebitProcessInfo.sData.byLen , sizeof(m_cDebitProcessInfo.sData.byLen)); 
	memcpy( byData ,m_cDebitProcessInfo.sData.byData , sizeof(m_cDebitProcessInfo.sData.byData)); 
}

void CDebitInfo::ClearDebitInfo()
{
	memset(&m_cDebitProcessInfo , ' ', sizeof(OLA_DEBIT_PROCESS_INFO));
}

//4.0.5.12
BOOL CDebitInfo::GetPendingInitDebitSession()
{
	return (m_bPendingInitDebitProcess);
}

void CDebitInfo::SetPendingInitDebitSesssion(BOOL bVal)
{
	m_bPendingInitDebitProcess = bVal;
}

//4.0.5.12