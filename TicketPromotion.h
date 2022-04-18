#pragma once
#include "Promotion.h"
#include "ExtraFields.h"


class CTicketPromotion;
typedef  shared_ptr<CTicketPromotion>  CTicketPromotionPtr;

using TicketPromotionlist = std::list<CTicketPromotionPtr>;


class CTicketPromotionlist :
	public TicketPromotionlist
{
public:
	BOOL	 GetPromotionByName(CString sName , CTicketPromotionPtr pCticketPromo); // retrieves the first one for this name
	BOOL	 GetPromotionByPlu(CString sPlu, CTicketPromotionPtr pCticketPromo); // retrieves the first one for this Plu
	BOOL	 AddPromotion(CTicketPromotionPtr pCticketPromo);
	//	BOOL	 GetPromotionListByCommonField(CString sField , CTicketPromotionlist &TicketPromotionlist); // retrievs a list of CTicketPromotion that has the in pramater field
};


class CTicketPromotion :
	public CPromotion
{
public:
	CTicketPromotion();
	~CTicketPromotion();
	BOOL AddField(CString sFieldName, CString sFieldValue);
	BOOL IsFieldExists(CString sFieldName);
	CString GetFieldValue(CString sFieldName);
	void SetProductPlu(long lproductPlu) {SetPluCode(lproductPlu);}
private:
	CExtraFields m_ExtraFields;
};

