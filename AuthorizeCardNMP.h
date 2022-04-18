#pragma once
class CAuthorizeCardNMP : public CNMPDetails
{
public:
	CAuthorizeCardNMP();
	virtual ~CAuthorizeCardNMP();
	virtual void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola) override;

private:
	void AuthorizePaymentCard(OLA_STAT &ola, long lPumpNumber, PAY_AT_PUMP_INFO & cTmpInfo );
	void AuthorizeLoyaltyCard(OLA_STAT &ola, long lPumpNumber, PAY_AT_PUMP_INFO & cTmpInfo );
	void AuthorizeAltId(OLA_STAT & ola, long lPumpNumber, PAY_AT_PUMP_INFO & cTmpInfo);
};

