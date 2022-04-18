#pragma once


class CLoyaltyNMPDetails: public CNMPDetails
{
public:
	CLoyaltyNMPDetails();
	~CLoyaltyNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;

	void GetMessageForTerminal(long lPumpNumber, PAY_AT_PUMP_INFO &cTmpInfo, int iSizeOfOutMsg, char * sOutMsg);
	void StartAlternateIDRetryFlow(PAY_AT_PUMP_INFO cInfo, long lPumpNumber, OLA_STAT * pOla);

	virtual void LoadConfiguration() override;

private:

	void BuildAlternateIDRetryMessage(PAY_AT_PUMP_INFO cInfo, long lPumpNumber, CString &sMsg);
	char  m_sAlternateIDRetrylMsg[MAX_PATH];
};
