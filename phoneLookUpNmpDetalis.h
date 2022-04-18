
// 4.0.23.1220 TD 155754
class CPhoneLookUpNMPDetails : public CNMPDetails
{
public:
	CPhoneLookUpNMPDetails();
	~CPhoneLookUpNMPDetails() {};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT & ola) override;
};