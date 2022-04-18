#pragma once

#include "NmpDetalis.h"
#include "LoyaltyDefs.h"


//4.0.22.508 start 74810
class CBaseFlow
{
public:
	CBaseFlow();
	virtual ~CBaseFlow();
	virtual void	LoadNMPConfiguration(BOOL bReconfigure, char *sPath);		
	virtual void	StartFlowControl(long lPumpNumber, short iStartOlaNMPState, OLA_STAT & ola){};	
	virtual void	HandleNMPWaitingSessions(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo, BOOL bSkipRepeatOption, BOOL bStartFueling = FALSE);
	virtual void	StartTicketFail( long lPumpNumber ,OLA_STAT *pOla );
	virtual BOOL	IsDeferralDiscountInFlow(){return m_psNMPRegistryEntries[NMP_TYPE_DeferralDiscount]->GetValidEntry();};	
	virtual BOOL	Init(BOOL bReconfigure);
	virtual short	GetNextNMPSession(long lPumpNumber, PAY_AT_PUMP_INFO *pInfo, long lCurrentState, BOOL bSkipRepeatOption, CNMPDetails ** psConfigDetails, BOOL bStartFueling = FALSE);


	BOOL			IsNeedToSkipDeferralStep(short iNextState, long lPumpNumber );
	void			SkipDeferralStep(CNMPDetails * &psConfigDetails, short &iNextState, PAY_AT_PUMP_INFO * pInfo, OLA_STAT * pOla, BOOL bSkipRepeatOption, long lPumpNumber);
	virtual short	FindCurrentNMPSession(long lCurrentState, CNMPDetails ** psConfigDetails);

	void	ExecuteNMPRegularSessions(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);	
	void	HandleNMPSessions(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo, BOOL bSkipRepeatOption, BOOL bStartFueling = FALSE);
	void	ClearOldSetup();
	void	ClearFlowOrder();	
	void	StartAfterQueryFlowControl(long lPumpNumber, OLA_STAT & ola, PAY_AT_PUMP_INFO & cTmpInfo);	
	void	StartAfterAddMemberFlowControl(long lPumpNumber, OLA_STAT * ola, long lMemberRes);	
	void	StartAfterStartTicketFlowControl(long lPumpNumber, OLA_STAT * pOla, long lStartTicketRes = 0);	
	

	char*	GetDeferralDiscountMsg();	
	BOOL	IsCarWashAfterFueling(){return bCarWashAfterFueling;};	
	void	UpdateCarWashTime(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber);	
	BOOL	IsDeferralDiscountBeforeFuelingPresented(){return bPresentDeferralDiscountMsg;};	
	BOOL	IsNMPStateBeforeStartTicket(long lState);	
	BOOL	IsNMPStateAfterStartTicket(long lState);	
	
	DWORD	GetNMPOrder(NMP_TYPE eType){return GetNMPOrder(m_psNMPRegistryEntries[eType]->GetPath());}; //4.0.23.20 76383
	BOOL	IsNumericAllowedInSwipeLoyalty(){return m_psNMPRegistryEntries[NMP_TYPE_LoyaltyCard]->GetEnableNumeric();}; //4.0.23.410 120603 //4.0.24.90
	virtual CNMPDetails * GetNMPObject(enum NMP_TYPE eNmpType) { return m_psNMPRegistryEntries[eNmpType];}
	BOOL	GetLoyaltyServerOnline() const { return m_bLoyaltyServerOnline; }
	void	SetLoyaltyServerOnline(BOOL val) { m_bLoyaltyServerOnline = val; }
	void	HandleNMPRegularSessions(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);

	//CNMPDetails* CreateNMPDetail(int iNMP, char sPath[MAX_PATH]);
	void	DumpOrderNMNP() const;

protected:
	DWORD	GetNMPOrder(char * sPath); 
	virtual void	LoadNMPRegistryEntries();
	virtual	short GetNextStateAfterFlowControl();
//	long			GetDeferralState();
	virtual void	UpdateQueryOrderInTheFlow(); //4.0.23.860 212541
	void	LoadLoyaltyErrorMsg(LoyaltyState_ eLoyaltyState, char * sOutMsg, int iLen, long &lScreenNumber) const;
	void	LoyaltyDecline( long lPumpNumber , long lMemberRes, OLA_STAT * pOla, PAY_AT_PUMP_INFO cTmpInfo );


	CNMPDetails* m_psNMPRegistryEntries[NMP_MAX_TYPE_OPTIONS];		
	CNMPDetails* m_psOrderNMP[NMP_MAX_TYPE_OPTIONS]; 
	BOOL	bCarWashAfterFueling;
	BOOL	bPresentDeferralDiscountMsg;
	BOOL	m_bPhoneConfigured;      // 4.0.23.1220 TD 155754
	BOOL    m_bLoyaltyServerOnline;
};

