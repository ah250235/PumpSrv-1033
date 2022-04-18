
#include "StdAfx.h"
#include "phoneLookUpNmpDetalis.h"


/******************************************************************************
 Description: CPhoneLookUpNMPDetails CTOR	
			  
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			12/10/2011       		Start			4.0.23.1220 TD 155754 
******************************************************************************/
CPhoneLookUpNMPDetails::CPhoneLookUpNMPDetails()
{
	SetEnableNumeric(TRUE);
	SetEnterCancel(TRUE);
	SetCardReader(TRUE);
	SetTimeout(NMP_DEFAULT_TIMEOUT);
	SetMessage("Swipe Reward Card | Select 1 to enter phone number | Select 2 to enter reward card number | NO to continue without reward card");
	SetStateType(OLA_LOYALTY_PHONE_LOOKUP);
}

/******************************************************************************
 Description:	
 Returns:      	

 Parameters:   	
 
 WHO			WHEN                    WHAT			Version
-------------------------------------------------------------------------------
 Omer			12/10/2011       		Start			4.0.23.1220 TD 155754
******************************************************************************/
void CPhoneLookUpNMPDetails::Execute(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola)
{
	char sPath[MAX_PATH] = { 0 };

	_Module.m_server.m_cPumpArray[lPumpNumber-1].SetPAPInfo(&cTmpInfo);

	long p = lPumpNumber-1;
	long lLoop = this->GetCurrentLoopNum();

	char sRegMsg[MAX_FIELD_VAL] = {0};

	if (lLoop > 1)
	{
		_Module.m_server.m_cCommonFunction.LoadMessageFromLanguagesReg(lPumpNumber, "PhoneNumberRetry", sRegMsg, sizeof(sRegMsg), "Phone Number Not Found. Try Again?");
	}
	else
	{
	//	_Module.m_server.m_cCommonFunction.LoadMessageFromLanguagesReg(lPumpNumber, "PhoneLookUp", sRegMsg, sizeof(sRegMsg));

		memcpy(sPath, this->GetPath(), sizeof(sPath));
		_Module.m_server.m_cParam.LoadParam(sPath, NMP_FIELD_MESSAGE, (char *)sRegMsg, sizeof(sRegMsg), "", FALSE);
		this->SetCurrentLoopNum(1);
	}

	if ((strlen(sRegMsg)) && (!ChrAll(sRegMsg, strlen(sRegMsg))))
	{
		memset(m_sMessage, 0, sizeof(m_sMessage));
		memcpy(m_sMessage, sRegMsg, sizeof(m_sMessage));
	}
	
	OpenNeedMoreParam(cTmpInfo ,lPumpNumber,m_sMessage,1,m_lTimeout,CreateGeneralNMPProperties() | ONMP_LoyaltyCard); // RFUEL-2006

	_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,"Execute Requested PhoneLookUp NMP");
}