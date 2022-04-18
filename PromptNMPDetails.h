#pragma once

class CPromptNMPDetails: public CNMPDetails
{
public:
	CPromptNMPDetails();
	~CPromptNMPDetails(){};
	void Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola, PromptType ePromptType = PT_GENERIC, INT iMaxNumOfOpenKeys = NMP_MAX_NUM_OF_OPEN_KEYS, short nONMPProperties = ONMP_OpenYesNoKeys | ONMP_NoAsCancel); //QC 461948
	void GetMessageForTerminal( long lPumpNumber,PAY_AT_PUMP_INFO & cTmpInfo  );
};

