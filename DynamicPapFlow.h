#pragma once

#include "AuthorizeCardNMP.h"
#include "SwipeCardNMPDetalis.h"
#include "TreeFlowChart.h"



class CDynamicPapFlow : public CBaseFlow
{
public:
	CDynamicPapFlow(long lPumpNumber);
	virtual ~CDynamicPapFlow();
	virtual BOOL	Init(BOOL bReconfigure = FALSE);
	virtual void	StartFlowControl(long lPumpNumber, short iStartOlaNMPState, OLA_STAT & ola);
	virtual short	GetNextNMPSession(long lPumpNumber, PAY_AT_PUMP_INFO *pInfo,long lCurrentState, BOOL bSkipRepeatOption, CNMPDetails ** psConfigDetails, BOOL bStartFueling = FALSE) override;
	virtual short	FindCurrentNMPSession(long lCurrentState, CNMPDetails ** psConfigDetails);	
	virtual BOOL	IsDeferralDiscountInFlow(){return FALSE;}
	virtual void	StartTicketFail(long lPumpNumber ,OLA_STAT *pOla);

	BOOL MoveToTheNextState(long lCurrentState);
	BOOL IsPumpNumberNotValid( long lPumpNumber );
	void StartTicket( long lPumpNumber, short iStartOlaNMPState, OLA_STAT &ola );
	

private:

	long m_lPumpNumber;
	BOOL m_lLoyaltySwiped;
	CTreeFlowChart m_cTreeFlowChart	;
	CNMPDetails * m_CurrentNmpStateObject;
	void ClearLastFlow();
	void SetTerminalConfiguration(TERMINAL_CONFIGURATION cTerminalConfiguration);
	BOOL IsVaildCard(long lMediaMasterType);
};

