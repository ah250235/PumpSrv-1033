#pragma once

#include <atlstr.h>
#include <iterator>
#include "PumpClass.h"
#include "CCard.h"

//CR 404913 

class CTendersCard : public CCard
{
public:
	CTendersCard();
	CTendersCard(int iIndex, long lPumpIndex);
	virtual ~CTendersCard(void);
	bool operator==( CTendersCard & TendersCard) const;
	CTendersCard operator=(CTendersCard &);

	long GetUsedAmount() const;// { return m_lAmount; }

	CString GetFullCardName() const;
//	void	SetFullCardName(CString sFullCardName, PAY_AT_PUMP_INFO & info);

	CString GetRefernceNumber() const;
//	void SetRefernceNumber(CString val, PAY_AT_PUMP_INFO & info);

	CString GetMediaMasterType() const;
	void	SetMediaMasterType(CString sMediaMasterType);

	CString GetCompletedSessionId() const;
	CString GetTenderIdStr() const;
	virtual void SetTrack2(CString val){ m_sTrack2 = val; }

	 CString GetAccountNumber() const { return m_sAccountNumber; }
	 void SetAccountNumber(CString sAccountNumber);

	 CString GetFormatAccountNumber() const { return ""; }

	 CString GetExpireDate() const;
	 void SetExpireDate(CString sExpireDate, PAY_AT_PUMP_INFO & info);

	 CString GetApproval() const;
	 void SetApproval(CString sApproval, PAY_AT_PUMP_INFO & info);

	 long GetPreAuthorizeSessionID() const;
	 void SetPreAuthorizeSessionID(long val, PAY_AT_PUMP_INFO & info);

	 BYTE GetMediaType() const { return m_sMediaType; }
	 void SetMediaType(BYTE val) { m_sMediaType = val; }

protected:
	CString m_sAccountNumber;				// Full account number.
	CString m_sTrack2;
	CString m_sMediaMasterType;
	CString m_sCardName;
	CString m_sExpire;
	CString m_sCompletedSessionId;
	BYTE	 m_sMediaType;

	long    m_lAmount;
	long    m_lPumpIndex;
	int		m_lCardSalePtr;
	int		m_iIndex;
};