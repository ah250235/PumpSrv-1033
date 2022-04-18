#include "stdafx.h"
#include "TicketPromotion.h"
#include "ExtraFieldS.h"
#include "XMLConsts.h"



CTicketPromotion::CTicketPromotion()
{
	OutputDebugStringW(L"CTicketPromotion constructor");
}


CTicketPromotion::~CTicketPromotion()
{
	OutputDebugStringW(L"CTicketPromotion distructor");
}

BOOL CTicketPromotion::AddField(CString sFieldName, CString sFieldValue)
{
	BOOL bRetVal = FALSE;
	eExtraFieldStatus eRet = m_ExtraFields.AddOrUpdateExtraField(sFieldName,sFieldValue);

	if ((eRet == efsUpdated) || (eRet == efsAdded))
		bRetVal = TRUE;
	return bRetVal;
}

BOOL CTicketPromotion::IsFieldExists(CString sFieldName)
{
	return (m_ExtraFields.IsFieldExists(sFieldName)== eExtraFieldStatus::efsFound);
}

CString CTicketPromotion::GetFieldValue(CString sFieldName)
{
	CString sRetVal;
	m_ExtraFields.GetExtraField(sFieldName, sRetVal);
	return sRetVal;
}







BOOL	CTicketPromotionlist::GetPromotionByName(CString sName, CTicketPromotionPtr pCticketPromo) // retrieves the first one for this name
{
	return FALSE;
};

BOOL	CTicketPromotionlist::GetPromotionByPlu(CString sPlu, CTicketPromotionPtr pCticketPromo) // retrieves the first one for this Plu
{
	BOOL bRetVal = FALSE;
	for (auto itPromotion = begin();itPromotion != end(); ++itPromotion)
		if ((*itPromotion)->GetFieldValue(CString(XMLSTR_PluCode)) == sPlu)
		{
			pCticketPromo = (*itPromotion);
			bRetVal = TRUE;
		}

	return bRetVal;
};

/*BOOL	CTicketPromotionlist::GetPromotionListByCommonField(CString sField, CTicketPromotionlist &TicketPromotionlist) // retrievs a list of CTicketPromotion that has the in pramater field
{
	return FALSE;
};
*/

BOOL	CTicketPromotionlist::AddPromotion(CTicketPromotionPtr pCticketPromo)
{
	push_back(pCticketPromo);
	return TRUE;
};

