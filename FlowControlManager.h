#pragma once

enum LOYALTY_FLOW_TYPE	//4.0.22.504 69344
{
	PAK_FLOW,	//0
	PAP_FLOW,	//1
	DYNAMIC_PAP_FLOW,

};

#include "BaseFlow.h"

class CFlowControlManager
{
public:
	CFlowControlManager();
	virtual ~CFlowControlManager();
	BOOL	Init(BOOL bReconfigure = FALSE);	//4.0.22.504 69344
	void	InitAllExistingFlows(BOOL bReconfigure = FALSE); //4.0.22.508 74810
	void	DeleteAllExistingFlows(); //4.0.22.508 74810
	void	SetInitFlag(BOOL bInitFlag) { m_bInitFlag = bInitFlag; };
	BOOL	GetInitFlag() { return m_bInitFlag; };
	void	SetFuelFlowControlFlag(BOOL bInitFlag) { m_bFuelFlowControlFlag = bInitFlag; };
	BOOL	GetFuelFlowControlFlag() { return m_bFuelFlowControlFlag; };
	void	SetEmeraldLoyaltyOnlineFlag(BOOL bOnlineFlag) { m_bEmeraldLoyaltyOnline = bOnlineFlag; };
	BOOL	GetEmeraldLoyaltyOnlineFlag() { return m_bEmeraldLoyaltyOnline; };
	void	SaveEmeraldLoyaltyErrorCode(long lErrorCode) { m_lEmeraldLoyaltyErrorCode = lErrorCode; };// RFUEL-797   
	long	GetEmeraldLoyaltyErrorCode() { return m_lEmeraldLoyaltyErrorCode; };						// RFUEL-797
	BOOL	IsNMPStartingTime(long lPumpNumber) { return m_NMPstartTime[lPumpNumber - 1]; };
	void	SetNMPTime(long lPumpNumber, BOOL bVal) { m_NMPstartTime[lPumpNumber - 1] = bVal; };
	void	HandleNMPSessions(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo, BOOL bSkipRepeatOption = TRUE, BOOL bStartFueling = FALSE);
	void	HandleNMPManualLoyalty(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);
	void	HandleNMPAlternateID(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);
	void	HandleNMPDeferralDiscount(long lPumpNumber, OLA_STAT *pOla);
	void	HandleNMPMethodOfPayment(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo, BOOL bPapToPak, BOOL bPaymentSwiped); //4.0.23.20 76383

	// Always check this pointer before using
	CBaseFlow *GetPumpFlow(long lPumpNumber) { return m_cPumpFlowObject[lPumpNumber - 1]; } //4.0.22.508 74810
	void	SetPumpFlow(long lPumpNumber, LOYALTY_FLOW_TYPE eType); //4.0.23.20 76383
	void	StartFlowControl(long lPumpNumber, short iStartOlaNMPState, OLA_STAT & ola, LOYALTY_FLOW_TYPE eType); //4.0.22.508 74810
	void	StartAfterQueryFlowControl(long lPumpNumber, OLA_STAT & ola, PAY_AT_PUMP_INFO & cTmpInfo);
	void	StartAfterAddMemberFlowControl(long lPumpNumber, OLA_STAT * ola, long lMemberRes);
	void	StartAfterStartTicketFlowControl(long lPumpNumber, OLA_STAT * pOla, long lStartTicketRes = 0);
	long	GetPumpSrvAddMemberCardRes(long lPumpNumber); //4.0.23.390 116544 //4.0.24.90
	void	SetPumpSrvAddMemberCardRes(long lPumpNumber, long lRes); //4.0.23.390 116544 //4.0.24.90
	void	SetStartOlaNMPState(long lPumpNumber, short iStartOlaNMPState) { m_iStartOlaNMPState[lPumpNumber - 1] = iStartOlaNMPState; } // TD 359309
	short	GetStartOlaNMPState(long lPumpNumber) { return m_iStartOlaNMPState[lPumpNumber - 1]; } // TD 359309
	void    HandleNMPPhoneLookUp(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo); // 4.0.23.1220 TD 155754
	void    HandleNMPGetPhoneNumber(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo); // 4.0.23.1220 TD 155754
	void    HandleNMPGetRewardCard(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo); // 4.0.23.1220 TD 155754
	void	HandleNMPDeferral(long lPumpNumber, OLA_STAT *pOla); //4.0.20.54 64528
	void    HandleManualDeferal(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);
	void    SetMediaMasterType(long lPumpNumber, long lMediaMaster);
	long	GetMediaMasterType(long lPumpNumber) const { return m_lMediaMasterType[lPumpNumber - 1]; }

	void	HandleNMPAlternateIdSwipeLoyaltyCard(long lPumpNumber, OLA_STAT * pOla, PAY_AT_PUMP_INFO* cInfo);
	void	HandleNMPAlternateIDManualEntry(long lPumpNumber, OLA_STAT * pOla, PAY_AT_PUMP_INFO* cInfo);
	void	HandlePunchhNMPAlternateIDRetry(long lPumpNumber, OLA_STAT * pOla, PAY_AT_PUMP_INFO* cInfo);

	void	SendNeedMoreParamSessionMemberInactive(long lPumpNumber, OLA_STAT & ola, PAY_AT_PUMP_INFO & cTmpInfo);
	void	HandleNeedMoreParamInactiveMemberResult(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);

	void	HandlePreAuthoitzeScreen(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO &cInfo, CARD_SALE_ALL3* pCardData);
	void    HandleNMPCarWashSale(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);
	void	HandleNMPCarWashPromotion(long lPumpNumber, OLA_STAT *pOla, PAY_AT_PUMP_INFO *pInfo);

	BOOL IsCarWashAmountBiggerThenPreAuthorizeAmount(PAY_AT_PUMP_INFO * pInfo, long lPumpNumber, OLA_STAT * pOla);// TD 466954
	BOOL IsCarWashAmountBiggerThenAuthoirzedAmount(PAY_AT_PUMP_INFO * pInfo, long lPumpNumber, OLA_STAT * pOla, long lAuthrizedAmount); // TD 467566) 


private:

	BOOL	m_bInitFlag;//if we init already - if we have read the configuration from the registry 
	BOOL	m_bFuelFlowControlFlag;//if we have flow control in the system
	BOOL	m_bEmeraldLoyaltyOnline;  //RFUEL-520
	BOOL	m_NMPstartTime[MAX_PUMPS_64];
	CBaseFlow *m_cPumpFlowObject[MAX_PUMPS_64]; //4.0.22.508 74810
	short   m_iStartOlaNMPState[MAX_PUMPS_64]; // TD 359309
	long	m_lPumpSrvMemberCardRes[MAX_PUMPS_64];
	long	m_lMediaMasterType[MAX_PUMPS_64];
	long	m_lEmeraldLoyaltyErrorCode;  //RFUEL - 797
	
	
};
