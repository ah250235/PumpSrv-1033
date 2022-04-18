#pragma once

typedef struct 
{
	short nSettings;
	long lScreenNumber;
	CString sMessage;

} TERMINAL_CONFIGURATION;

class CTreeFlowChart
{
public:
	CTreeFlowChart(void);
	virtual ~CTreeFlowChart(void);
	short GetNextState(long lPumpNumber, long lLastInput, TERMINAL_CONFIGURATION & cTerminalConfiguration);
	short MoveStatesAfterPaymentCardSwipe(long lPumpNumber);
	void GetCurrentCardsCounters(IN long lLastInput, OUT long & lCurrentAltId, OUT long &lCurrentPayment);
	BOOL IsValidInput(long lInput);
	CString ConvertInputToAttributeName(long lInput);
	void GetCurrentTerminalConfiguration(TERMINAL_CONFIGURATION & cTerminalConfiguration);
	void Init();

private:
	CString m_sXmlPathAndName;
	CXMLInterface m_XmlFlowObject;
	CXMLPointers m_CurrentNodePtr;
};

