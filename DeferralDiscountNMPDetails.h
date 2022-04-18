

//4.0.20.54 64528
class CDeferralDiscountNMPDetails: public CNMPDetails
{
public:
	CDeferralDiscountNMPDetails();
	~CDeferralDiscountNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola) override;
	
	void GetMessageForTerminal( long lPumpNumber,PAY_AT_PUMP_INFO &cTmpInfo , int iSizeOfOutMsg,char * sOutMsg);
	void HandleDeferralAnswer(long lPumpNumber , OLA_STAT *pOla);
	void StartManualDeferralFlow( PAY_AT_PUMP_INFO cInfo, long lPumpNumber, OLA_STAT * pOla );
	void HandleManualDeferral(long lPumpNumber , OLA_STAT *pOla , PAY_AT_PUMP_INFO *pInfo);
	virtual void LoadConfiguration();

	
private:
	void ContiuneFlowAfterManualDeferral( long lPumpNumber, long lDeferralDiscount, BOOL bValidDeferralValue ,PAY_AT_PUMP_INFO * pInfo, OLA_STAT * pOla );
	long GetDeferralValueFromCardSale( PAY_AT_PUMP_INFO * pInfo );
	void TreatMaxDefrrralExceed(long lPunpNumber ,long lDeferralDiscount,long lMaxDeferral , OLA_STAT *pOla ,PAY_AT_PUMP_INFO *pInfo );
	int   m_lNumOfManualDeferralRetries[MAX_PUMPS_64];
	char  m_sManualDeferralMsg[MAX_PATH];
};