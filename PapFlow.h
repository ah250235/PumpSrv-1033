#pragma once

#include "LoyaltyDefs.h"


class CPapFlow : public CBaseFlow
{
public:
	static CPapFlow* getInstance();
	static void DeleteFlow();
	virtual ~CPapFlow();
	void	LoadNMPConfiguration(BOOL bReconfigure, char *sPath);	//4.0.22.508	74810
	void	StartFlowControl(long lPumpNumber, short iStartOlaNMPState, OLA_STAT & ola);	//4.0.22.508 74810
	BOOL	Init(BOOL bReconfigure);

private:
	CPapFlow();
	static BOOL m_bInstanceFlag;
	static CPapFlow* m_pInstance;
};



