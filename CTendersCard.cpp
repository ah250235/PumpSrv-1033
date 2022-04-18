#include "stdafx.h"
#include <atlstr.h>
#include <iterator>
#include "PumpClass.h"
#include "CTendersCard.h"



CTendersCard::CTendersCard(int iIndex, long lPumpIndex)
{
	m_iIndex = iIndex;
	m_lCardSalePtr = sizeof(PAYMENT_CARD_DETAILS)* iIndex; //to check
	m_lPumpIndex = lPumpIndex;
}


CTendersCard::CTendersCard()
{

}

CTendersCard::~CTendersCard(void)
{
}

bool CTendersCard::operator==( CTendersCard & TendersCard) const
{
	CString s = TendersCard.GetAccountNumber();
	return TendersCard.GetAccountNumber().Compare(m_sAccountNumber) == 0;
}


void CTendersCard::SetExpireDate(CString sExpireDate, PAY_AT_PUMP_INFO & info)
{
	m_sExpire = sExpireDate;
}

void CTendersCard::SetAccountNumber(CString sAccountNumber)
{
	// TD 405456
	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[m_lPumpIndex].GetPAPInfo(&info);

	memcpy(info.CardSaleInfo.cardData.sAccountNumber, sAccountNumber.GetBuffer(0), min(sAccountNumber.GetLength(), sizeof(info.CardSaleInfo.cardData.sAccountNumber)));

	if (_Module.m_server.m_cProtectedDataHandler.GetPCIActiveFlag() == PCI_FLAG_ON)
	{
		_Module.m_server.m_cProtectedDataHandler.RemoveConfidencesData(&info);
	}

	m_sAccountNumber.Format("%.21s", info.CardSaleInfo.cardData.sAccountNumber);
}


CString CTendersCard::GetExpireDate() const
{
	return m_sExpire;
}

CString CTendersCard::GetFullCardName() const
{
	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[m_lPumpIndex].GetPAPInfo(&info);

	char sTmp[100] = { 0 };
	memcpy(sTmp, info.CardSaleExtraData6.PaymentCard1.sCardName + m_lCardSalePtr, sizeof(info.CardSaleExtraData6.PaymentCard1.sCardName));
	return sTmp;
}
CString CTendersCard::GetRefernceNumber() const
{
	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[m_lPumpIndex].GetPAPInfo(&info);

	char sTmp[100] = { 0 };
	memcpy(sTmp, info.CardSaleExtraData6.PaymentCard1.sRefernceNumber + m_lCardSalePtr, sizeof(info.CardSaleExtraData6.PaymentCard1.sRefernceNumber));
	return sTmp;

}
CString CTendersCard::GetMediaMasterType() const
{
	return m_sMediaMasterType;
}
CString CTendersCard::GetCompletedSessionId() const
{
	OLA_STAT pOla;
	_Module.m_server.m_cPumpArray[m_lPumpIndex].GetOlaStat(&pOla);
	char tempCompletedSessionID[6] = { 0 };

	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[m_lPumpIndex].GetPAPInfo(&info);

	if (pOla.m_lSessionNumber > 9999)
	{
		memcpy(tempCompletedSessionID, info.CardSaleExtraData2.sCompletedSessionID_MSB, sizeof(info.CardSaleExtraData2.sCompletedSessionID_MSB));
		memcpy(tempCompletedSessionID + 2, info.CardSaleExtraData2.sCompletedSessionID, sizeof(info.CardSaleExtraData2.sCompletedSessionID));
	}
	else
		memcpy(tempCompletedSessionID, info.CardSaleExtraData2.sCompletedSessionID, sizeof(info.CardSaleExtraData2.sCompletedSessionID));

	return tempCompletedSessionID;
}

CString CTendersCard::GetTenderIdStr() const
{
	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[m_lPumpIndex].GetPAPInfo(&info);

	char sTmp[100] = { 0 };
	memcpy(sTmp, info.CardSaleExtraData6.PaymentCard1.sTenderId + m_lCardSalePtr, sizeof(info.CardSaleExtraData6.PaymentCard1.sTenderId));
	return sTmp;
}

void CTendersCard::SetMediaMasterType(CString val)
{
	m_sMediaMasterType = val;
}

long CTendersCard::GetUsedAmount() const // 478137
{
	PAY_AT_PUMP_INFO info;
	_Module.m_server.m_cPumpArray[m_lPumpIndex].GetPAPInfo(&info);

	long lAmountPaid = a2l(info.CardSaleExtraData6.PaymentCard1.sAmountPaid + m_lCardSalePtr, sizeof(info.CardSaleExtraData6.PaymentCard1.sAmountPaid));
	return lAmountPaid;
}