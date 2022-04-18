#pragma once


class CPakFlow :public CBaseFlow
{
public:
	static CPakFlow* getInstance();
	static void DeleteFlow();
	virtual ~CPakFlow() {}
	void	LoadNMPConfiguration(BOOL bReconfigure, char *sPath);	//4.0.22.508	 74810
	void	StartFlowControl(long lPumpNumber, short iStartOlaNMPState, OLA_STAT & ola);	//4.0.22.508 74810
	BOOL	Init(BOOL bReconfigure);
	void	LoadNMPRegistryEntries();
	short	GetNextStateAfterFlowControl();
	void	UpdateQueryOrderInTheFlow(); //4.0.23.860 212541

private:
	CPakFlow();
	static BOOL m_bInstanceFlag;
	static CPakFlow* m_pInstance;

};

