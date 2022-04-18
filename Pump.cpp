#include "stdafx.h"
#include "PumpClass.h"
#include <CStrFormat.h>


/////////////////////////////////////////////////////////////////////////////
// CPump

// default constructor 
CPump::CPump() : m_lWaitCounter(0)
{
	m_strPumpMenufacture	= "";
	m_strPumpCaption        = ""; //4.0.20.500 -  TD 64967
	m_lType					= 0;
	m_lNumber				= NO_PUMP_NUMBER;
	m_cTrsArray.SetSize(MAX_TRS_IN_LIST);

	ClearOlaStat();

	m_lShiftNumber			= 0;

// Use to be in CServerPump
	m_nVer					= PUMP_START_VER;
	m_nDetailedVer			= PUMP_START_VER;
	memset(&m_byPosition, 0, sizeof(m_byPosition));
	m_lLatestPrePayPosNumber  = 0;
	m_lLatestPrePaySyncNumber = 0;
	m_lCurrentServiceFee = 0;//4.0.3.24
	//4.0.5.39
	m_byPumpLoginOp = PUMP_NO_LOGIN;
	//Roni Start(version 4.0.9.800)
	m_lModeService = 0;
	m_lPriceLevel  = 0;
	//Roni End
	m_lCurrentServiceMode  =  0 ; //4.0.11.505

	m_lPapToPakTrsNum = NULL_TRS_NUM; // 256101

	memset(&m_cPAPInfo,0,sizeof(m_cPAPInfo));
	memset(&m_byTankPerPosition, 0, sizeof(m_byTankPerPosition));	//4.0.20.52 (62818)

	m_lLoyaltyPromptRetry = 0;
}

CPump::CPump(long lPumpNumber) : m_lWaitCounter(0)
{
	m_strPumpMenufacture	= "";
	m_strPumpCaption        = ""; //4.0.20.500 -  TD 64967
	m_lType					= 0;
	m_cTrsArray.SetSize(MAX_TRS_IN_LIST); 

	ClearOlaStat();

// Use to be in CServerPump
	m_lNumber				= lPumpNumber;
	m_nVer					= PUMP_START_VER;
	m_nDetailedVer			= PUMP_START_VER;
	memset(&m_byPosition, 0, sizeof(m_byPosition));
	m_lLatestPrePayPosNumber  = 0;
	m_lLatestPrePaySyncNumber = 0;
	m_lCurrentServiceFee = 0;//4.0.3.24
	//4.0.5.39
	m_byPumpLoginOp = PUMP_NO_LOGIN;
	//Roni Start(version 4.0.9.800)
	m_lModeService = 0;
	m_lPriceLevel  = 0;
	//Roni End

	m_lPapToPakTrsNum = NULL_TRS_NUM; // 256101
	
	memset(&m_cPAPInfo,0,sizeof(m_cPAPInfo));
	memset(&m_byTankPerPosition, 0, sizeof(m_byTankPerPosition));	//4.0.20.52 (62818)

	m_lLoyaltyPromptRetry = 0;
}

// copy constructor
CPump::CPump(const CPump& pump)
{
	m_strPumpMenufacture	= pump.m_strPumpMenufacture;
	m_strPumpCaption        = pump.m_strPumpCaption; //4.0.20.500 -  TD 64967
	m_lType					= pump.m_lType;
	m_lNumber				= pump.m_lNumber;
	m_cPumpStat				= pump.m_cPumpStat;
	
	int iTrsArraySize = pump.GetTrsArraySize();	
	if(iTrsArraySize)
	{
		SetTrsArraySize(iTrsArraySize);	
		for (int i=0;i<iTrsArraySize;i++)
		{
			m_cTrsArray[i] = pump.m_cTrsArray[i];
		}
	}

	m_cOla			= pump.m_cOla;
	memset(&m_byPosition, 0, sizeof(m_byPosition));
	m_lLatestPrePayPosNumber  = pump.m_lLatestPrePayPosNumber;
	m_lLatestPrePaySyncNumber = pump.m_lLatestPrePaySyncNumber;
	//4.0.5.39
	m_byPumpLoginOp = pump.m_byPumpLoginOp;
	//Roni Start(version 4.0.9.800)
	m_lModeService = pump.m_lModeService;
	m_lPriceLevel  = pump.m_lPriceLevel;
	//Roni End
	
	m_lPapToPakTrsNum = pump.m_lPapToPakTrsNum; // 256101
}

CPump::~CPump()
{
}

int CPump::GetTrsArraySize() const
{
	return m_cTrsArray.GetSize();
}

// indexes are zero-based
int CPump::GetTrsByIndex(int index,CPumpTransact *trs) const
{
int iRetNum;	

	if((m_cTrsArray.GetSize()) < (index+1))
	{
		iRetNum = -1;
	}
	else if(index >= 0)
	{
		*trs = m_cTrsArray[index];
		iRetNum = 0;
	}
	else
		iRetNum = -1;


	return iRetNum;
}

void CPump::operator =( TAG_PUMP & tagPump )
{	
	unsigned char szName[MAX_NAME_LENGTH];
	short int iTrsArraySize;

	int iNameLen = min(MAX_NAME_LENGTH,sizeof(tagPump.sPumpMenufacture));
		
	memcpy(szName,tagPump.sPumpMenufacture,iNameLen);	
	szName[iNameLen-1] = 0;
	
	m_strPumpMenufacture	= szName;
	m_lNumber				= a2l(tagPump.sNumber,sizeof(tagPump.sNumber));
	m_lType					= a2l(tagPump.sType,sizeof(tagPump.sType));
	m_cPumpStat				= tagPump.pumpStat;
//	iTrsArraySize			= a2i(tagPump.sNumberOfTrs,sizeof(tagPump.sNumberOfTrs));

//	if(GetTrsArraySize() != (int)iTrsArraySize)
//		SetTrsArraySize((int)iTrsArraySize);
	
	iTrsArraySize = GetTrsArraySize();	
	for (int i=0;i<iTrsArraySize;i++)
	{
		m_cTrsArray[i] = tagPump.pumpTrs[i];
	}

}

CPump& CPump::operator=(CPump & cPump)
{
	memcpy((BYTE*)m_byPosition , cPump.GetPositionArray() , MAX_POSITIONS_PER_PUMP);

	cPump.GetOlaStat(&m_cOla);

	//4.0.14.500 cPump.GetPAPInfo(&m_cPAPInfo);
	memcpy(&m_cPAPInfo,&cPump.m_cPAPInfo,sizeof(m_cPAPInfo)); 

	m_cPumpStat = cPump.GetStat();
	///m_cTimeSuppressedDriveoffTimeout = cPump.GetTimeSuppressedDriveoffTimeout();  //4.0.2.38
	
	for(int i = 0;i < cPump.GetTrsArraySize();i++)
	{
		cPump.GetTrsByIndex(i , &(m_cTrsArray[i])); 
		m_cTimeSuppressedDriveoffTimeout[i] = cPump.GetTimeSuppressedDriveoffTimeout(i); //4.0.2.38

	}
		

	m_dwAllowPrePayOnRequestingPump = cPump.IsAllowPrePayOnRequestingPump();
	m_dwClosedPrePayTimeOut = cPump.GetClosedPrePayTimeOut();
	m_dwDispensingTimeOut = cPump.GetDispensingTimeOut();
	m_dwMaxPrePayTrsAllowed = cPump.GetMaxPrePayTrsAllowed();
	m_dwMaxUnpaidTrsAllowed = cPump.GetMaxUnpaidTrsAllowed();
	m_dwMaxAttendantTrsAllowed = cPump.GetMaxAttendantTrsAllowed(); //4.0.22.501 - merged from 4.0.22.12 TD 73062
	m_dwMaxPostVoidTrsAllowed = cPump.m_dwMaxPostVoidTrsAllowed; //CR 351687: Enhanced post void transactions
	m_dwPAKAuthorizedTimeOut = cPump.GetPAKAuthorizedTimeOut();
	m_dwPriceLevelDefault = cPump.GetPriceLevelDefault();
	m_dwUnBalancePrePayCounter = cPump.GetUnBalancePrepayCounter();
	m_dwDeclinePrePayCounter   = cPump.GetDeclinePrepayCounter();//4.0.12.502
	m_lChangedTrsNumber = cPump.m_lChangedTrsNumber;
	m_lCurrentServiceFee = cPump.GetCurrentServiceFee();
	m_lLatestPrePayPosNumber = cPump.GetLatestPrePayPosNumber();
	m_lLatestPrePaySyncNumber = cPump.GetLatestPrePaySyncNumber();
	m_lLockStatus = cPump.GetLockStatus();
	m_lNumber = cPump.GetNumber();
	m_lShiftNumber = cPump.GetShiftNumber();
	m_lType = cPump.GetType();
	m_lWaitCounter = cPump.m_lWaitCounter;
	m_nDetailedVer = cPump.GetDetailedVer();
	m_nVer = cPump.GetVer();

	memcpy(&m_sSearchFeeTime , &(cPump.m_sSearchFeeTime) , sizeof(m_sSearchFeeTime));
	m_strPumpMenufacture = cPump.m_strPumpMenufacture;
	m_strPumpCaption     = cPump.m_strPumpCaption; //4.0.20.500 -  TD 64967
	
	m_dwPumpAsTerminal = cPump.IsPumpDefineAsTerminal(); //4.0.3.60
	//4.0.9.800
	m_lModeService = cPump.GetModeService();
	m_lPriceLevel = cPump.GetPriceLevel();
	m_lCurrentServiceMode = cPump.GetModeSchemeServiceMode();

	m_dwMaxTrsNumber = cPump.GetMaxTrsNumber(); //4.0.13.500

	memcpy((BYTE*)m_byTankPerPosition, cPump.GetTankPerPositionArray(), MAX_POSITIONS_PER_PUMP);	//4.0.20.52 (62818)

	m_lPapToPakTrsNum = cPump.m_lPapToPakTrsNum; // 256101

	return *this;
}

/******************************************************************************
 Description: return number of Failed CAT transactions
 Returns:      	short
 Parameters:   	
 Assumptions:   
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			31/05/2009				Start //4.0.22.508 75592
******************************************************************************///  
int CPump::UnpaidFailedCATTransactions() 
{
	int iMaxTrs=0,iUnpaidFailedCatTrs=0;

	iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{
		if( (m_cTrsArray[i].GetStatus() == UNPAID) &&
			(m_cTrsArray[i].GetOnOffer() == 0) && ((m_cTrsArray[i].m_lMisc & MISC_TRS_FAILED_CAT))) 
			iUnpaidFailedCatTrs++;			
	}

	return iUnpaidFailedCatTrs;
}

/******************************************************************************
 Description: return number of ANPR blocked transactions
 Returns:      	short
 Parameters:   	
 Assumptions:   
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			15/12/2010				Start //4.0.26.505 157073
******************************************************************************///  
int CPump::UnpaidANPRBlockedTransactions() 
{
	int iMaxTrs=0,iUnpaidANPRBlockedTrs=0;

	iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{
		if( (m_cTrsArray[i].GetStatus() == UNPAID) &&
			(m_cTrsArray[i].GetOnOffer() == 0) && ((m_cTrsArray[i].m_lMisc & MISC_TRS_BLOCKED_ANPR))) 
			iUnpaidANPRBlockedTrs++;			
	}

	return iUnpaidANPRBlockedTrs;
}

/******************************************************************************
 Description: return number of ANPR Auth with failed transactions
 Returns:      	short
 Parameters:   	
 Assumptions:   
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			15/12/2010				Start //4.0.26.505 157073
******************************************************************************///  
int CPump::UnpaidANPRAuthWithFailedTransactions() 
{
	int iMaxTrs=0,iUnpaidANPRAuthWithFailedTrs=0;

	iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{
		if( (m_cTrsArray[i].GetStatus() == UNPAID) &&
			(m_cTrsArray[i].GetOnOffer() == 0) && ((m_cTrsArray[i].m_lMisc & MISC_TRS_AUTH_WITH_FAILED_ANPR))) 
			iUnpaidANPRAuthWithFailedTrs++;			
	}

	return iUnpaidANPRAuthWithFailedTrs;
}


// return number of UnPaid & not on offer transactions
int CPump::UnpaidTransactions() 
{
	int iMaxTrs=0,iUnpaidTrs=0;

	iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{
		if( (m_cTrsArray[i].GetStatus() == UNPAID) &&
			(m_cTrsArray[i].GetOnOffer() == 0) ) 
			iUnpaidTrs++;			
	}
	return iUnpaidTrs;
}

//4.0.13.500
int CPump::AcceptedTransactions()
{
	int iMaxTrs=0,iAcceptedTrs=0;

	iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{
		if(m_cTrsArray[i].GetStatus() == ACCEPTED && 
		   m_cTrsArray[i].GetRes() == PRE_PAY_AT_POS) //4.0.14.500
			iAcceptedTrs++;			
	}
	return iAcceptedTrs;

}

//4.0.13.500
int CPump::NumOfUnpaidAndAcceptedTrs()
{
	int iMaxTrs=0,iAcceptedTrs=0;

	iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{
		if(m_cTrsArray[i].GetStatus() == ACCEPTED || 
		   m_cTrsArray[i].GetStatus() == UNPAID)
			iAcceptedTrs++;			
	}
	return iAcceptedTrs;

}

// return number of UnPaid & not on offer & TimeOut  transactions
int CPump::UnpaidTimeoutTransactions() 
{
	int iMaxTrs=0,iUnpaidTrs=0;

	iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{
		if( (m_cTrsArray[i].GetStatus() == UNPAID) &&
			(m_cTrsArray[i].GetOnOffer() == 0) &&
			(m_cTrsArray[i].m_lMisc & MISC_UNPAID_TRS_TIMEOUT)) 
			
		{
			if(!(m_cTrsArray[i].m_lMisc & MISC_SUPPRESSED_UNPAID_TRS_TIMEOUT))
				iUnpaidTrs++;			
		}
	}
	return iUnpaidTrs;
}


void CPump::SetTrsArraySize(int size)
{
	m_cTrsArray.SetSize(size);
}


LONG CPump::GetNumber()
{
	return m_lNumber;
}


// Use to be in CServerPump 

void CPump::GetTag(void *pBuff, long lVersion)
{
	short int iTrsArraySize;
	TAG_PUMP *pTagPump;
	pTagPump = (TAG_PUMP *)pBuff; 
	long p = m_lNumber - 1;

	// Build Pump Menufacture - structure location same for ver 1 and ver 2 and ver 3
	memcpy(	pTagPump->sPumpMenufacture,
			(LPCTSTR)m_strPumpMenufacture,
			min(sizeof(pTagPump->sPumpMenufacture), m_strPumpMenufacture.GetLength()) );

	// Build Pump Number and Type - structure location same for ver 1 and ver 2 and ver 3
	l2a(m_lNumber,	pTagPump->sNumber,	sizeof(pTagPump->sNumber));
	l2a(m_lType,	pTagPump->sType,	sizeof(pTagPump->sType));

	// Build Pump Status structure (start) location same for ver 1 and ver 2 and ver 3
	m_cPumpStat.GetTag(&(pTagPump->pumpStat),lVersion);



	// Build Transaction Array according to version number.
	if(lVersion == TAG_PUMP_VERSION_1)
	{
		TAG_PUMP_TRANSACT *pTagPumpTrs = pTagPump->pumpTrs;

		iTrsArraySize = (short int)GetTrsArraySize();
		i2a(iTrsArraySize,pTagPump->sNumberOfTrs,sizeof(pTagPump->sNumberOfTrs));
	
		for (int i=0;i<iTrsArraySize;i++)
		{
			GetTagTrsByIndex(i,pTagPumpTrs);		
			pTagPumpTrs++;
		}	
	}
	else if(lVersion == TAG_PUMP_VERSION_2)
	{
		//TD 38654
	/*	TAG_PUMP2 *pTagPump2;
		pTagPump2 = (TAG_PUMP2 *)pBuff; */

		TAG_PUMP2_EX *pTagPump2;
		pTagPump2 = (TAG_PUMP2_EX *)pBuff; 

		iTrsArraySize = (short int)GetTrsArraySize();
		i2a(iTrsArraySize,pTagPump2->sNumberOfTrs,sizeof(pTagPump2->sNumberOfTrs));

		//TD 38654
		//TAG_PUMP_TRANSACT2 *pTagPumpTrs = pTagPump2->pumpTrs;
		TAG_PUMP_TRANSACT5 *pTagPumpTrs = pTagPump2->pumpTrs;
	
		for (int i=0;i<iTrsArraySize;i++)
		{
			GetTagTrsByIndex(i,pTagPumpTrs,2);		
			pTagPumpTrs++;
		}	
	}
	else if(lVersion == TAG_PUMP_VERSION_3)
	{
		//TD 38654
		/*TAG_PUMP3 *pTagPump3;
		pTagPump3 = (TAG_PUMP3 *)pBuff; */
		TAG_PUMP3_EX *pTagPump3;
		pTagPump3 = (TAG_PUMP3_EX *)pBuff; 
		
		PAY_AT_PUMP_INFO info;	//4.0.18.501
		p = m_lNumber - 1;

		memset(&info, ' ', sizeof(info));
		GetPAPInfo(&info);

		iTrsArraySize = (short int)GetTrsArraySize();
		i2a(iTrsArraySize,pTagPump3->sNumberOfTrs,sizeof(pTagPump3->sNumberOfTrs));

		//TD 38654
		//TAG_PUMP_TRANSACT2 *pTagPumpTrs = pTagPump3->pumpTrs;
		TAG_PUMP_TRANSACT5 *pTagPumpTrs = pTagPump3->pumpTrs;
	
		for (int i=0;i<iTrsArraySize;i++)
		{
			GetTagTrsByIndex(i,pTagPumpTrs,2);		
			pTagPumpTrs++;
		}

		memcpy(pTagPump3->sFullVehicleId, info.CardSaleInfo.cardData.sFullVehicleId, sizeof(pTagPump3->sFullVehicleId));
	}

	//4.0.1.40  added "|| (lVersion == 5)"
	else if((lVersion == TAG_PUMP_VERSION_4) || (lVersion == TAG_PUMP_VERSION_5) ||
			(lVersion == TAG_PUMP_VERSION_6) || (lVersion == TAG_PUMP_VERSION_7) ||		//4.0.9.507
			(lVersion == TAG_PUMP_VERSION_8) || (lVersion == TAG_PUMP_VERSION_9))		//4.0.15.500 //TD 38654

	{

		//TD 38654
		/*TAG_PUMP4 *pTagPump4;
		pTagPump4 = (TAG_PUMP4 *)pBuff; */

		TAG_PUMP4_EX *pTagPump4;
		pTagPump4 = (TAG_PUMP4_EX *)pBuff; 
		PAY_AT_PUMP_INFO info; //4.0.18.501
		p = m_lNumber - 1;

		memset(&info, ' ', sizeof(info));
		GetPAPInfo(&info);

		//AlarmSrv active 4,0,2.37
		if ((_Module.m_server.GetAlarmSrvMode() == ALRMSRV_DIRECT_ACTIVE) && (m_lNumber > 0))
		{		
			if (_Module.m_server.m_cAlarm.m_lAlarmPumpMap[m_lNumber])
				l2a(ALARM_SRV_CODE,pTagPump4->pumpStat.sError,sizeof(pTagPump4->pumpStat.sError));
			else
				l2a(0,pTagPump4->pumpStat.sError,sizeof(pTagPump4->pumpStat.sError));
		}
		
		//4.0.2.37


		iTrsArraySize = (short int)GetTrsArraySize();
		i2a(iTrsArraySize,pTagPump4->sNumberOfTrs,sizeof(pTagPump4->sNumberOfTrs));

		//TD 38654
		//TAG_PUMP_TRANSACT2 *pTagPumpTrs = pTagPump4->pumpTrs;
		TAG_PUMP_TRANSACT5 *pTagPumpTrs = pTagPump4->pumpTrs;
		
		for (int i=0;i<iTrsArraySize;i++)
		{
			GetTagTrsByIndex(i,pTagPumpTrs,2);
			pTagPumpTrs++;
		}

		memset(pTagPump4->sFullVehicleId,' ',sizeof(pTagPump4->sFullVehicleId));
		//info.GetXmlField(ATT_FULL_VEHICLE_ID,(char*)pTagPump4->sFullVehicleId);
		memcpy(pTagPump4->sFullVehicleId, info.CardSaleInfo.cardData.sFullVehicleId, sizeof(pTagPump4->sFullVehicleId));
		
		//4.0.5.0
		if (_Module.m_server.UseServiceFee())
			l2a(GetCurrentServiceFee(),pTagPump4->pumpStat.sServiceFee  , sizeof(pTagPump4->pumpStat.sServiceFee));	

		
		//4.0.3.60
		if (IsPumpDefineAsTerminal())
			pTagPump4->sPumpAsTerminal ='1';
		
		if (_Module.m_server.PumpNotAllowedMixPAKAndPrePay(m_lNumber ,PAY_AT_KIOSK))
			pTagPump4->sNoParallelPakPrePayflg = '1';


		//4.0.1.40
		if(lVersion == TAG_PUMP_VERSION_5)
		{
			//TD 38654
			//TAG_PUMP5 * pTagPump5 = (TAG_PUMP5*)pBuff;
			TAG_PUMP5_EX * pTagPump5 = (TAG_PUMP5_EX*)pBuff;

			_Module.m_server.m_cTerminalMessages.GetMessage(m_lNumber , pTagPump5->sTerminalMsg , MAX_TERMINAL_MESSAGE_LENGTH);
		}


		//4.0.9.507
		if(lVersion == TAG_PUMP_VERSION_6)
		{
			//TD 38654
			//TAG_PUMP6 * pTagPump6 = (TAG_PUMP6*)pBuff;
			TAG_PUMP6_EX * pTagPump6 = (TAG_PUMP6_EX*)pBuff;

			if (_Module.m_server.m_cPumpArray[p].GetDieselPumpFlags())
				pTagPump6->sIsPumpDiesel = '1';
			if(_Module.m_server.m_bPresentCardName)//4.0.10.1000
			{
				FillShortCardName(p,pTagPump6->sShortCardNameTrsArr);
				if(LOG_BASIC_FLOW_CONTROL)
					_LOGMSG.LogMsg("GetTag - return the short card name for all trs in tag_pump 6");
			}

		}

		if(lVersion == TAG_PUMP_VERSION_7)
		{
			//TD 38654
			//TAG_PUMP7 * pTagPump7 = (TAG_PUMP7*)pBuff;
			TAG_PUMP7_EX * pTagPump7 = (TAG_PUMP7_EX*)pBuff;

			_Module.m_server.m_cTerminalMessages.GetMessage(m_lNumber , pTagPump7->sTerminalMsg , MAX_TERMINAL_MESSAGE_LENGTH);
			if (_Module.m_server.m_cPumpArray[p].GetDieselPumpFlags())
				pTagPump7->sDieselPump = '1';
			if(_Module.m_server.m_bPresentCardName)//4.0.10.1000
			{
				FillShortCardName(p,pTagPump7->sShortCardNameTrsArr);
					if(LOG_BASIC_FLOW_CONTROL)
					_LOGMSG.LogMsg("GetTag - return the short card name for all trs in tag_pump 7");
			}

		}
		
		// CR 19675 - 4.0.15.500
		if (lVersion == TAG_PUMP_VERSION_8)
		{
			//TD 38654
			//TAG_PUMP8 * pTagPump8 = (TAG_PUMP8*)pBuff;
			TAG_PUMP8_EX * pTagPump8 = (TAG_PUMP8_EX*)pBuff;

			_Module.m_server.m_cTerminalMessages.GetMessage(m_lNumber , pTagPump8->sTerminalMsg , MAX_TERMINAL_MESSAGE_LENGTH);
			if (_Module.m_server.m_cPumpArray[p].GetDieselPumpFlags())
				pTagPump8->sIsPumpDiesel = '1';
			if(_Module.m_server.m_bPresentCardName)//4.0.10.1000
			{
				FillShortCardName(p,pTagPump8->sShortCardNameTrsArr);

				if(LOG_BASIC_FLOW_CONTROL)
					_LOGMSG.LogMsg("GetTag - return the short card name for all trs in tag_pump 8");
			}

			FillPreSetData(p, pTagPump8->sPreSetLimit, pTagPump8->sPreSetLimitType, lVersion); //4.0.15.501
			
		}
		//TD 38654
		if (lVersion == TAG_PUMP_VERSION_9)
		{
			//TD 38654
			TAG_PUMP9 * pTagPump9 = (TAG_PUMP9*)pBuff;

			_Module.m_server.m_cTerminalMessages.GetMessage(m_lNumber , pTagPump9->sTerminalMsg , MAX_TERMINAL_MESSAGE_LENGTH);
			if (_Module.m_server.m_cPumpArray[p].GetDieselPumpFlags())
				pTagPump9->sIsPumpDiesel = '1';
			if(_Module.m_server.m_bPresentCardName)//4.0.10.1000
			{
				FillShortCardName(p,pTagPump9->sShortCardNameTrsArr);

				if(LOG_BASIC_FLOW_CONTROL)
					_LOGMSG.LogMsg("GetTag - return the short card name for all trs in tag_pump 8");
			}

			FillPreSetData(p, pTagPump9->sPreSetLimit, pTagPump9->sPreSetLimitType, lVersion); //4.0.15.501
			
		}
	}
}

long CPump::GetPreSetValue(long lSyncNumber) const
{
	CPumpTransact		trs;
	PRE_PAY_INFO		info;
	long lRtc = 0;

	memset(&info,0,sizeof(PRE_PAY_INFO));

	trs.SetLink(TRS_LINK_PREPAY_NUMBER, lSyncNumber);				
	lRtc = _Module.m_server.TrsLinkOperation(REC_READ,&trs,&info);

	if (lRtc == 0)
		return  info.lMaxValue;
	else 
		return 0;

}

// indexes are zero-based
int CPump::GetTagTrsByIndex(int index, void *pBuff,long lVersion) const
{
int iRetNum;	

	if((m_cTrsArray.GetSize()) < (index+1))
	{
		iRetNum = -1;
	}
	else if(index >= 0)
	{
		long lPreSetVal = 0;
		long lGrade = 0; 
		long lPreSetType = 0; //Value 4.0.22.503 72394 

		if((lVersion == 2) && (((CPumpTransact)m_cTrsArray[index]).GetLinkFlags() & TRS_LINK_PREPAY_NUMBER))
		{
			PRE_PAY_INFO cInfo;
			memset(&cInfo , 0 , sizeof(PRE_PAY_INFO));
						
			//lPreSetVal = GetPreSetValue(m_cTrsArray[index].GetLinkNumber());
			if (GetPrePayInfo(((CPumpTransact)m_cTrsArray[index]).GetLinkNumber(), &cInfo))
			{
				lPreSetVal = cInfo.lMaxValue;

				//TD - 72394 - for prepay with volume limit  //4.0.22.503
				if (lPreSetVal == 0)
				{
					//TD 72394 4.0.15.770 //4.0.22.503
					lPreSetVal = cInfo.lMaxVolume;	
					lPreSetType = 1; //Volume
				}

				if (cInfo.lGradePrice)
				{
					if ( cInfo.lFlags & GRADE_PRICE_IS_POSITION_NUM) //4.0.9.507
					{
						 lGrade = _Module.m_server.m_cPumpArray[cInfo.lPumpNumber -1].NozzleToGrade((short)cInfo.lGradePrice);							
					}
				}
			}			
		}

		((CPumpTransact)m_cTrsArray[index]).GetTag(pBuff,lVersion,lPreSetVal , lGrade, 0, 0, lPreSetType); //4.0.22.503 72394
		iRetNum = 0;
	}
	else
		iRetNum = -1;

	return iRetNum;
}

int CPump::GetTagTrsByNumber(LONG lTrsNumber,void *pBuff,long lVersion) const
{
	int iRetNum = -1;	
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].m_lNumber == lTrsNumber)
			{
				long lPreSetVal = 0;
				long lGrade = 0;

				if((lVersion == 2) && (((CPumpTransact)m_cTrsArray[i]).GetLinkFlags() == TRS_LINK_PREPAY_NUMBER))
				{
					//lPreSetVal = GetPreSetValue(m_cTrsArray[i].GetLinkNumber());
					PRE_PAY_INFO cInfo;
					memset(&cInfo , 0 , sizeof(PRE_PAY_INFO));
								
					//lPreSetVal = GetPreSetValue(m_cTrsArray[index].GetLinkNumber());
					if (GetPrePayInfo(((CPumpTransact)m_cTrsArray[i]).GetLinkNumber(), &cInfo))
					{
						lPreSetVal = cInfo.lMaxValue;

						if (cInfo.lGradePrice)
						{
							if ( cInfo.lFlags & GRADE_PRICE_IS_POSITION_NUM) //4.0.9.507
							{
								 lGrade = _Module.m_server.m_cPumpArray[cInfo.lPumpNumber -1].NozzleToGrade((short)cInfo.lGradePrice);							
							}
						}
					}			

				}

				iRetNum = 0;
				((CPumpTransact)m_cTrsArray[i]).GetTag(pBuff,lVersion,lPreSetVal, lGrade);
				break;
			}
		}
	}
	
	return iRetNum;
}


void CPump::SetStat(const CPumpStatus& pumpStat)
{
	m_cPumpStat = pumpStat;	
	UpdateVer();
//  M.L 6/5/98
//	UpdateDetailedVer();
}


//
// Set transact method assume that the array size if
// fixed and set before.
// Set transact method return -1 if the index 
// is out of the array scope  
//

int CPump::SetTrsByIndex(int index,CPumpTransact *pTrs, BOOL bRollingUpdate)
{
	int iRetNum;	

	if((m_cTrsArray.GetSize()) < (index+1))
	{
		iRetNum = -1;
	}
	else if(index >= 0)
	{
		if( bRollingUpdate)
		{
			if ((m_cTrsArray[index].m_lRoundedValue != pTrs->m_lRoundedValue)||
				(m_cTrsArray[index].m_lRoundedVolume != pTrs->m_lRoundedVolume) )
			{
				UpdateDetailedVer();
				m_cTrsArray[index].UpdateTime();
			}
		}
		else
			UpdateVer();

		m_cTrsArray[index] = *pTrs;		
		iRetNum = 0;		
	}
	else
		iRetNum = -1;

	return iRetNum;
}

int	CPump::SetTrsByNumber(LONG lTrsNumber, CPumpTransact *pTrs,  BOOL bRollingUpdate)
{
	int iRetNum = -1;	
	int iArraySize;

	iArraySize = m_cTrsArray.GetSize();

	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].m_lNumber == lTrsNumber)
			{
				long lMisc = m_cPumpStat.GetMisc();
				if( bRollingUpdate)
				{
					if ((m_cTrsArray[i].m_lRoundedValue != pTrs->m_lRoundedValue)||
						(m_cTrsArray[i].m_lRoundedVolume != pTrs->m_lRoundedVolume) )
					{
						UpdateDetailedVer();
						pTrs->UpdateTime();
						if(lMisc & MISC_DISPENSING_TIMEOUT)
						{	
							lMisc &= MISC_NOT_DISPENSING_TIMEOUT;
							m_cPumpStat.SetMisc(lMisc);
						}
						if(lMisc & MISC_NOZZLE_LEFT_OUT_TIMEOUT)//4.0.19.502
						{	
							lMisc &= MISC_NOT_NOZZLE_LEFT_OUT_TIMEOUT;
							m_cPumpStat.SetMisc(lMisc);
						}
					}
				}
				else
				{
					UpdateVer();
					if(lMisc & MISC_DISPENSING_TIMEOUT)
					{	
						lMisc &= MISC_NOT_DISPENSING_TIMEOUT;
						m_cPumpStat.SetMisc(lMisc);
					}
					if(lMisc & MISC_NOZZLE_LEFT_OUT_TIMEOUT)//4.0.19.502
					{	
						lMisc &= MISC_NOT_NOZZLE_LEFT_OUT_TIMEOUT;
						m_cPumpStat.SetMisc(lMisc);
					}
				}
				m_cTrsArray[i] = *pTrs;
				iRetNum = 0;
				break;
			}			
		}
	}
	return iRetNum;
}



void CPump::SetNumber(LONG lPumpNumber)
{
	m_lNumber = lPumpNumber;
}
////////////////////////////////////////////////////////////////////
// Insert Trs can be used both 
// From the controller side and from the POS side (in case of prepay)
// 
// 
// The insert method assume that the array size is
// fixed and set before.
// The insert method return 1 if there is no place 
// to insert the new transaction.
//
// return code 0 - Trs inserted to list
// return code 1 - Trs list is full
// return code 2 - There is in progress Trs - need to cancel 
//					or concluded first.
// return code 3 - First in waiting list
// return code 4 - Invalid Trs Number 
////////////////////////////////////////////////////////////////////
int CPump::InsertTrs(CPumpTransact *pTrs, BOOL bAccept0Transaction, BOOL bPostVoid)
{
	int iRetNum = 1;	
	int iArraySize = m_cTrsArray.GetSize();

	// New Logic: Fix Places , new transaction 
	// inserted to first free place in the array

	if((!bAccept0Transaction)&&(pTrs->m_lNumber == 0))
	{
		iRetNum = 4; //Invalid Trs Number 
	}
	else if(iArraySize)
	{
		BOOL bReplace = FALSE;
		BOOL bInsert = TRUE;
		short nPrePayWaiting, nFree, nUnpaid;
		GetTrsListCount(&nPrePayWaiting,&nFree,&nUnpaid); 
		nPrePayWaiting = GetNumberOfNonClosedPrePay();
		// waiting = WAIT_RES,WAIT_AUTH,WAIT_PROGRESS, 

		// check first if new trs not conflict with another.
		if(pTrs->GetStatus() == IN_PROGRESS)
		{
			CPumpTransact currentTrs;
			BOOL bCurrent = TRUE;			
			bCurrent = !GetInProgressWaitProgressTrs(&currentTrs);

			//Check is transaction is PAK
			//if (pTrs->GetLinkFlags() == TRS_NO_LINK) 3.1.1.4
			if (PAK_LINK_TYPE(pTrs->GetLinkFlags()) ) 
			{
				if(bCurrent)
				{
					//if ( currentTrs.GetLinkFlags() == TRS_NO_LINK ) 3.1.1.4

					if ( PAK_LINK_TYPE(currentTrs.GetLinkFlags()))
					{
						bReplace = TRUE;
						bInsert = FALSE;								
					}
				}
			}
			else
			{
				long lTrsNumber=0;
				if(!GetCurrentTrsNumber(&lTrsNumber))
				{
					iRetNum = 2; // Another TRS already in progress 
					bInsert = FALSE;
				}
			}
		}
		else
		{
			short nTotalTrsWaiting = nPrePayWaiting;
			short nPostVoid =  (_Module.m_server.GetPosVoidFlags()) ? 1 :0;		//PostVoid12345	
			
			CPumpTransact currentTrs;
			//Check if there already a transaction IN_PROGRESS or WAIT_PROGRESS
			if ( (!GetInProgressWaitProgressTrs(&currentTrs)) ||
			   ( (GetStat().GetStatus() == REQUESTING) && (!m_dwAllowPrePayOnRequestingPump)) )
			{
				nTotalTrsWaiting++;
			}

			
			if(pTrs->GetLinkFlags() & TRS_LINK_CASH_ACCEPTOR_INFO)   //4.0.1.40 
			{
				// do not do any validation, always accept new transaction 
			}
			else if	
			( 
					//The meaning of m_dwMaxPrePayTrsAllowed is actually the number of stacked PrePay,
					//therefore if nPrePayWaiting = m_dwMaxPrePayTrsAllowed it is OK to allow PrePay
					(nFree == 0 ) ||
					((pTrs->GetLinkFlags() & TRS_LINK_PREPAY_NUMBER) && (nTotalTrsWaiting > (short)m_dwMaxPrePayTrsAllowed)) ||
					((PAK_LINK_TYPE(pTrs->GetLinkFlags())) && (nUnpaid >= (short)m_dwMaxUnpaidTrsAllowed + nPostVoid))
					//((pTrs->GetLinkFlags() == TRS_NO_LINK) && (nUnpaid >= (short)m_dwMaxUnpaidTrsAllowed + nPostVoid))3.1.1.4							 
				)
			{
				bInsert = FALSE;
				iRetNum = 1; // List is full
			}		
		}


		if(bInsert) //insert the transaction 		
		{ 
			for (int i=0;i<iArraySize; i++)
			{
				if(	(m_cTrsArray[i].GetStatus() == NULL_TRS)||
					(m_cTrsArray[i].GetStatus() == PAID) )
				{
					
					pTrs->m_lPump		= m_lNumber;
					if (!bPostVoid)      // PostVoid12345
					{
						pTrs->m_lMode		= m_cPumpStat.GetMode();
						pTrs->m_lResExt		= m_cPumpStat.GetResExt();
						pTrs->m_lRes		= m_cPumpStat.GetRes();

					}
				
					
					// 2.0.6 
					if(pTrs->m_lRes == NULL_RES)
					{
					
						//if( pTrs->GetLinkFlags() == TRS_NO_LINK)
						if( PAK_LINK_TYPE(pTrs->GetLinkFlags()))
						{
							pTrs->m_lRes		= PAY_AT_KIOSK;
							pTrs->m_lResExt		= NULL_RES_EXT;
						}
					}

					if(pTrs->GetLinkFlags() & TRS_LINK_PREPAY_NUMBER)
					{
						pTrs->m_lRes		= PRE_PAY_AT_POS;
						if (pTrs->GetLinkFlags()  & TRS_LINK_FLEET)   //4.0.9999.
							pTrs->m_lResExt		= RES_EXT_FLEET;
						else
							pTrs->m_lResExt		= NULL_RES_EXT;
					}

					else if(pTrs->GetLinkFlags() == TRS_LINK_OLA_NUMBER)
					{
						pTrs->m_lRes		= PAY_AT_PUMP;
						//pTrs->m_lResExt		= NULL_RES_EXT;
					}else if ((pTrs->m_lRes == PAY_AT_KIOSK) &&   //4.0.9.501
						(pTrs->m_lResExt ==RES_EXT_ATTENDANT) && (!pTrs->GetLinkFlags()))
					{
						pTrs->SetLink(TRS_LINK_ATTEND , 0);
					}


					if (m_lNumber)
					{
						memset(_Module.m_server.m_cShortCardTrsArr[m_lNumber - 1].cShortCardNameTrs[i].sShortCardName, ' ',   
						sizeof(_Module.m_server.m_cShortCardTrsArr[m_lNumber -1].cShortCardNameTrs[i].sShortCardName));   //4.0.12.518
					}

					
					m_cTrsArray[i] = *(pTrs);

					iRetNum = 0;
					if((pTrs->GetLinkFlags() &  TRS_LINK_PREPAY_NUMBER)&&(nPrePayWaiting==0))
						iRetNum = 3; // first in list 
					break;
				}							
			}
		}

		if(bReplace)
		{
			int i=0;
			for (i; i<iArraySize; i++)
			{
				if(m_cTrsArray[i].GetStatus() == IN_PROGRESS)
				{
					iRetNum = 0;
					break;
				}
			}

			if(iRetNum)				
			{//4.0.11.502
				for (int i=0;i<iArraySize; i++)
				{
					if(m_cTrsArray[i].GetStatus() == WAIT_PROGRESS)
					{
						iRetNum = 0;
						break;
					}
				}
			}

			if(!iRetNum)
			{
				pTrs->m_lPump		= m_lNumber;
				if (!bPostVoid)      // PostVoid12345
				{
					pTrs->m_lMode		= m_cPumpStat.GetMode();
					pTrs->m_lResExt		= m_cPumpStat.GetResExt();
					pTrs->m_lRes		= PAY_AT_KIOSK;
					pTrs->m_lResExt		= NULL_RES_EXT;

				}
				

				if(LOG_LOGIC_ERROR)
				{
					CString str;
					str.Format("Replacing known TRS %ld, with reported TRS %ld",m_cTrsArray[i].m_lNumber, pTrs->m_lNumber);
					_LOGMSG.LogMsg( pTrs->m_lPump,LOG_PUMP,str );
				}

				m_cTrsArray[i] = *(pTrs);
			}
		}
	}
	else
		iRetNum = 1;// no place in list 

	if((iRetNum == 0)||(iRetNum == 3))
	{
		UpdateVer();
// M.L 6/5/98
//		UpdateDetailedVer();
	}

	return iRetNum;	
}

void CPump::UpdateVer()
{
	if(m_nVer == SHRT_MAX)
		m_nVer = PUMP_START_VER;
	else
		m_nVer++;
}

void CPump::UpdateDetailedVer()
{
	if(m_nDetailedVer == SHRT_MAX)
		m_nDetailedVer = PUMP_START_VER;
	else
		m_nDetailedVer++;
}

void CPump::GetTagStat(void *pBuff, long lVersion)
{
	GetStat().GetTag(pBuff,lVersion);
}

void CPump::GetTagMinimal(void *pBuff,long lVersion)
{
	
//	_LOGMSG.LogMsg("CPump::GetTagMinimal starts");

	TAG_MINIMAL_PUMP *pTagMinimalPump;
	pTagMinimalPump = (TAG_MINIMAL_PUMP *)pBuff;

	l2a(m_lNumber,pTagMinimalPump->sNumber,sizeof(pTagMinimalPump->sNumber));
	l2a(GetStat().GetRes(),pTagMinimalPump->sRes,sizeof(pTagMinimalPump->sRes));
	
	//4.0.4.20
	if (GetStat().GetResExt() == RES_EXT_AVI_CANCELED)
	{
		l2a(RES_EXT_AVI,pTagMinimalPump->sResExt,sizeof(pTagMinimalPump->sResExt));
	}
	else
		l2a(GetStat().GetResExt(),pTagMinimalPump->sResExt,sizeof(pTagMinimalPump->sResExt));

/*
	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("GetTagMinimal res=%d, resExt=%d",GetStat().GetRes(),GetStat().GetResExt());
		_LOGMSG.LogMsg( str );
	}
*/

	l2a(GetStat().GetStatus(),pTagMinimalPump->sStatus,sizeof(pTagMinimalPump->sStatus));
	
	//AlarmSrv active 4,0,1.40
	if ((_Module.m_server.GetAlarmSrvMode() == ALRMSRV_DIRECT_ACTIVE) && (m_lNumber > 0))
	{
		if (_Module.m_server.m_cAlarm.IsAnyAlarmPump(m_lNumber))
			l2a(ALARM_SRV_CODE,pTagMinimalPump->sWarning,sizeof(pTagMinimalPump->sWarning));
		else
			l2a(0,pTagMinimalPump->sWarning,sizeof(pTagMinimalPump->sWarning));
	}
	else
	{
		if((GetStat().GetWarning() != 0)||(GetStat().GetError() != 0))
			l2a(1,pTagMinimalPump->sWarning,sizeof(pTagMinimalPump->sWarning));
		else
			l2a(0,pTagMinimalPump->sWarning,sizeof(pTagMinimalPump->sWarning));
	}


	short nPrePayWaiting, nFree, nUnpaid;
	GetTrsListCount(&nPrePayWaiting, &nFree, &nUnpaid);
	nPrePayWaiting = GetNumberOfNonClosedPrePay();

	i2a(GetNumberOfTrsInList(),pTagMinimalPump->sNumberOfTrs,sizeof(pTagMinimalPump->sNumberOfTrs));
/*
	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("Pump %d NumberOfTrsInList %d ",m_lNumber,GetNumberOfTrsInList());
		_LOGMSG.LogMsg( str );
	}
*/
	short nUnpaidNotOnOferTrs = UnpaidTransactions();
	short nAcceptedTrs = AcceptedTransactions(); //4.0.13.93

	i2a(nUnpaidNotOnOferTrs,pTagMinimalPump->sNumberOfUnPaidTrs,sizeof(pTagMinimalPump->sNumberOfUnPaidTrs));
	i2a(nPrePayWaiting,pTagMinimalPump->sNumberOfStackedPrePayTrs,sizeof(pTagMinimalPump->sNumberOfStackedPrePayTrs));

	if (lVersion == 2)
	{
//		 _LOGMSG.LogMsg("CPump::GetTagMinimal Received version 2");
		TAG_MINIMAL_PUMP_2 *pTagMinimalPump2;
		pTagMinimalPump2 = (TAG_MINIMAL_PUMP_2 *)pBuff;
			
		i2a(UnpaidTimeoutTransactions(),pTagMinimalPump2->sNumberOfUnpaidTrsTimeout,sizeof(pTagMinimalPump2->sNumberOfUnpaidTrsTimeout));
		//long lMisc = GetStat().GetMisc(); 
		long lMisc = GetStat().GetMisc() & 0x0000ffff;  //marik

		if(PumpValidForPrePay())
			lMisc |= MISC_PUMP_AVAILABLE_FOR_PRE_PAY;
		else
			lMisc &= MISC_NOT_PUMP_AVAILABLE_FOR_PRE_PAY;

		if(m_dwAllowPrePayOnRequestingPump)
			lMisc |= MISC_ALLOW_PREPAY_ON_REQUESTING_PUMP;
		else
			lMisc &= MISC_NOT_ALLOW_PREPAY_ON_REQUESTING_PUMP;


		l2a(lMisc,pTagMinimalPump2->sPumpMisc,sizeof(pTagMinimalPump2->sPumpMisc));

		//i2a((nUnpaid - nUnpaidNotOnOferTrs),pTagMinimalPump2->sNumberOfOnOfferOrAcceptedTrs, sizeof(pTagMinimalPump2->sNumberOfOnOfferOrAcceptedTrs));
		i2a((nUnpaid - nUnpaidNotOnOferTrs - nAcceptedTrs),pTagMinimalPump2->sNumberOfOnOfferOrAcceptedTrs, sizeof(pTagMinimalPump2->sNumberOfOnOfferOrAcceptedTrs)); //4.0.13.93

		l2a(GetStat().GetMode(),pTagMinimalPump2->sPumpMode,sizeof(pTagMinimalPump2->sPumpMode));

		if (GetStat().GetMessage() != 0)
			l2a(1,pTagMinimalPump2->sMessage,sizeof(pTagMinimalPump2->sMessage));
		else
			l2a(0,pTagMinimalPump2->sMessage,sizeof(pTagMinimalPump2->sMessage));
	}
	else if (lVersion == 3)		//3.0.2.3
	{
//		_LOGMSG.LogMsg("CPump::GetTagMinimal Received version 3");
		TAG_MINIMAL_PUMP_3 *pTagMinimalPump3;
		pTagMinimalPump3 = (TAG_MINIMAL_PUMP_3 *)pBuff;
		long lNumberOfHotPumpTrs = 0;
		long lNumberOfHotPumpTrsTimeOut = 0;

		short nFailedCATTrs = UnpaidFailedCATTransactions();	//4.0.22.508 75592
		i2a(nFailedCATTrs, pTagMinimalPump3->sNumberOfFailedCatTrs, sizeof(pTagMinimalPump3->sNumberOfFailedCatTrs)); //4.0.22.508 75592
			
		i2a(UnpaidTimeoutTransactions(),pTagMinimalPump3->sNumberOfUnpaidTrsTimeout,sizeof(pTagMinimalPump3->sNumberOfUnpaidTrsTimeout));
		long lMisc = GetStat().GetMisc(); 
//		if(LOG_BASIC_FLOW_CONTROL)
//		{	
//				CString str;
//				str.Format("GetTagMinimal3 Pump %d Current misc %ld",m_lNumber,lMisc );
//				_LOGMSG.LogMsg(str);
//		}


		if(PumpValidForPrePay())
			lMisc |= MISC_PUMP_AVAILABLE_FOR_PRE_PAY;
		else
			lMisc &= MISC_NOT_PUMP_AVAILABLE_FOR_PRE_PAY;

		if(m_dwAllowPrePayOnRequestingPump)
			lMisc |= MISC_ALLOW_PREPAY_ON_REQUESTING_PUMP;
		else
			lMisc &= MISC_NOT_ALLOW_PREPAY_ON_REQUESTING_PUMP;

		//4.0.12.502
		if(m_dwDeclinePrePayCounter)
			lMisc |= MISC_DECLINE_PRP;
		else
			lMisc &= MISC_NOT_MISC_DECLINE_PRP;
		//4.0.12.502


		l2a(lMisc,pTagMinimalPump3->sPumpMisc,sizeof(pTagMinimalPump3->sPumpMisc));

		//i2a((nUnpaid - nUnpaidNotOnOferTrs),pTagMinimalPump3->sNumberOfOnOfferOrAcceptedTrs, sizeof(pTagMinimalPump3->sNumberOfOnOfferOrAcceptedTrs));
		i2a((nUnpaid - nUnpaidNotOnOferTrs - nAcceptedTrs),pTagMinimalPump3->sNumberOfOnOfferOrAcceptedTrs, sizeof(pTagMinimalPump3->sNumberOfOnOfferOrAcceptedTrs)); //4.0.13.93

		l2a(GetStat().GetMode(),pTagMinimalPump3->sPumpMode,sizeof(pTagMinimalPump3->sPumpMode));

		if (GetStat().GetMessage() != 0)
		{
			long lMess = GetStat().GetMessage();
			// 3.0.2.3 l2a(1,pTagMinimalPump3->sMessage,sizeof(pTagMinimalPump3->sMessage));
			l2a(lMess,pTagMinimalPump3->sMessage,sizeof(pTagMinimalPump3->sMessage));
		}			
		else
			l2a(0,pTagMinimalPump3->sMessage,sizeof(pTagMinimalPump3->sMessage));

		GetNumberOfHotPumpTransactions(lNumberOfHotPumpTrs , lNumberOfHotPumpTrsTimeOut);

		l2a( lNumberOfHotPumpTrs, pTagMinimalPump3->sNumberOfHotPumpTrs  , sizeof(pTagMinimalPump3->sNumberOfHotPumpTrs));				
		l2a( lNumberOfHotPumpTrsTimeOut, pTagMinimalPump3->sNumberOfHotPumpTrsTimeOut  , sizeof(pTagMinimalPump3->sNumberOfHotPumpTrsTimeOut));	
		l2a((long)m_dwUnBalancePrePayCounter, pTagMinimalPump3->sUnBalancePrePayCounter  , sizeof(pTagMinimalPump3->sUnBalancePrePayCounter));	

	
		l2a(m_lLatestPrePayPosNumber , pTagMinimalPump3->sLatestPrePayPosNumber , sizeof(pTagMinimalPump3->sLatestPrePayPosNumber)); 
		l2a(m_lLatestPrePaySyncNumber , pTagMinimalPump3->sLatestPrePaySyncNumber, sizeof(pTagMinimalPump3->sLatestPrePaySyncNumber));
		//3.2.1.46
		pTagMinimalPump3->sPumpTransactionWasOffer = (IsExistTransactionOnOffer()) ? '1': '0';

		//4.0.2.23
		//If system is down, but PAK finalization is still allowed,
		//we would like to keep the POSPump pump display active, but 
		//we don't want to display a normal pump icon, so we change the 
		//pump status sent to POSPump to COMMS_TERNINATED, WITHOUT CHANGING
		//THE REAL PUMP STATUS!!!  We do this for all pumps, regardless of whether
		//they have transactions on them, in order to avoid the situation where
		//some pumps have a COMMS_TERMINATED icon, while others do not.
		if(_Module.m_server.ISPAKFinalizationFlagSet())
			l2a(COMMS_TERMINATED , (unsigned char*)pTagMinimalPump3->sStatus , sizeof(pTagMinimalPump3->sStatus));
		

		//4.0.3.28
		long lMode = GetStat().GetMode();
		//4.0.3.28
		if (GetStat().GetStatus() == REQUESTING)
		{
			if (MODE_VALID(lMode) && _Module.m_server.m_cModeArray[lMode - 1].AutoAuthorize(GetStat().GetRes(), GetStat().GetResExt()))
				pTagMinimalPump3->sTransactionAutoAuthorize = '1';
		}
		//4.0.3.28
		//4.0.3.33
		if (GetStat().GetStatus() == READY && GetStat().GetRes() == PAY_AT_PUMP)
		{
			if (MODE_VALID(lMode) && _Module.m_server.m_cModeArray[lMode - 1].AutoAuthorize(GetStat().GetRes(), GetStat().GetResExt()))
				pTagMinimalPump3->sTransactionAutoAuthorize = '1';
		}
		//4.0.3.33

		//4.0.3.60
		if (IsPumpDefineAsTerminal())
			pTagMinimalPump3->sPumpAsTerminal ='1';
		
		//4.0.7.500 - check if the feature is active
		long lRunningValueThreshold = _Module.m_server.GetRunningValueThreshold();
		long lCurrentRollingUpdateValue = 0;
		
		if( lRunningValueThreshold )
		{
			lCurrentRollingUpdateValue = GetCurrentTrsRollingUpdateValue()/1000;
			if(lCurrentRollingUpdateValue < lRunningValueThreshold)
				lCurrentRollingUpdateValue = 0;
		}
		
		l2a( lCurrentRollingUpdateValue, pTagMinimalPump3->sRoundedValue, sizeof(pTagMinimalPump3->sRoundedValue));

		//4.0.7.500 - end

		// do not allowed post paid
		if (nPrePayWaiting)   //4.0.9.501
			pTagMinimalPump3->sNoParallelPakPrePayflg = '1';
		
		//4.0.23.509 - also add the grade and the position to the minimal info to return to PosPump
		l2a(GetStat().GetPosition(),&pTagMinimalPump3->sPosition,sizeof(pTagMinimalPump3->sPosition));
		l2a(GetStat().GetGrade(),pTagMinimalPump3->sGrade,sizeof(pTagMinimalPump3->sGrade));
		

	}

	else if (lVersion == 5)		//Liat TD 37244
	{
//		 _LOGMSG.LogMsg("CPump::GetTagMinimal Received version 5");

		TAG_MINIMAL_PUMP_5 *pTagMinimalPump5;
		pTagMinimalPump5 = (TAG_MINIMAL_PUMP_5 *)pBuff;
		long lNumberOfHotPumpTrs = 0;
		long lNumberOfHotPumpTrsTimeOut = 0;

		short nANPRBlockedTrs = UnpaidANPRBlockedTransactions();	//4.0.26.505 157073
		i2a(nANPRBlockedTrs, pTagMinimalPump5->sNumberOfANPRBlockedTrs, sizeof(pTagMinimalPump5->sNumberOfANPRBlockedTrs)); //4.0.26.505 157073
		short nANPRAuthWithFailTrs = UnpaidANPRAuthWithFailedTransactions();	//4.0.26.505 157073
		i2a(nANPRAuthWithFailTrs, pTagMinimalPump5->sNumberOfANPRAuthWithFailedTrs, sizeof(pTagMinimalPump5->sNumberOfANPRAuthWithFailedTrs)); //4.0.26.505 157073

		short nFailedCATTrs = UnpaidFailedCATTransactions();	//4.0.22.508 75592
		i2a(nFailedCATTrs, pTagMinimalPump5->sNumberOfFailedCatTrs, sizeof(pTagMinimalPump5->sNumberOfFailedCatTrs)); //4.0.22.508 75592

		i2a(UnpaidTimeoutTransactions(),pTagMinimalPump5->sNumberOfUnpaidTrsTimeout,sizeof(pTagMinimalPump5->sNumberOfUnpaidTrsTimeout));
		long lMisc = GetStat().GetMisc(); 
//		if(LOG_BASIC_FLOW_CONTROL)
//		{	
//				CString str;
//				str.Format("GetTagMinimal3 Pump %d Current misc %ld",m_lNumber,lMisc );
//				_LOGMSG.LogMsg(str);
//		}


		if(PumpValidForPrePay())
			lMisc |= MISC_PUMP_AVAILABLE_FOR_PRE_PAY;
		else
			lMisc &= MISC_NOT_PUMP_AVAILABLE_FOR_PRE_PAY;

		if(m_dwAllowPrePayOnRequestingPump)
			lMisc |= MISC_ALLOW_PREPAY_ON_REQUESTING_PUMP;
		else
			lMisc &= MISC_NOT_ALLOW_PREPAY_ON_REQUESTING_PUMP;

		//4.0.12.502
		if(m_dwDeclinePrePayCounter)
			lMisc |= MISC_DECLINE_PRP;
		else
			lMisc &= MISC_NOT_MISC_DECLINE_PRP;
		//4.0.12.502


		l2a(lMisc,pTagMinimalPump5->sPumpMisc,sizeof(pTagMinimalPump5->sPumpMisc));

		//i2a((nUnpaid - nUnpaidNotOnOferTrs),pTagMinimalPump3->sNumberOfOnOfferOrAcceptedTrs, sizeof(pTagMinimalPump3->sNumberOfOnOfferOrAcceptedTrs));
		i2a((nUnpaid - nUnpaidNotOnOferTrs - nAcceptedTrs),pTagMinimalPump5->sNumberOfOnOfferOrAcceptedTrs, sizeof(pTagMinimalPump5->sNumberOfOnOfferOrAcceptedTrs)); //4.0.13.93

		l2a(GetStat().GetMode(),pTagMinimalPump5->sPumpMode,sizeof(pTagMinimalPump5->sPumpMode));

		//4.0.19.504 - TD 59171 - also add the grade and the position to the minimal info to return to PosPump
		l2a(GetStat().GetPosition(),&pTagMinimalPump5->sPosition,sizeof(pTagMinimalPump5->sPosition));
		l2a(GetStat().GetGrade(),pTagMinimalPump5->sGrade,sizeof(pTagMinimalPump5->sGrade));

		if (GetStat().GetMessage() != 0)
		{
			long lMess = GetStat().GetMessage();
			// 3.0.2.3 l2a(1,pTagMinimalPump3->sMessage,sizeof(pTagMinimalPump3->sMessage));
			l2a(lMess,pTagMinimalPump5->sMessage,sizeof(pTagMinimalPump5->sMessage));
		}			
		else
			l2a(0,pTagMinimalPump5->sMessage,sizeof(pTagMinimalPump5->sMessage));

		GetNumberOfHotPumpTransactions(lNumberOfHotPumpTrs , lNumberOfHotPumpTrsTimeOut);

		l2a( lNumberOfHotPumpTrs, pTagMinimalPump5->sNumberOfHotPumpTrs  , sizeof(pTagMinimalPump5->sNumberOfHotPumpTrs));				
		l2a( lNumberOfHotPumpTrsTimeOut, pTagMinimalPump5->sNumberOfHotPumpTrsTimeOut  , sizeof(pTagMinimalPump5->sNumberOfHotPumpTrsTimeOut));	
		l2a((long)m_dwUnBalancePrePayCounter, pTagMinimalPump5->sUnBalancePrePayCounter  , sizeof(pTagMinimalPump5->sUnBalancePrePayCounter));	

	
		l2a(m_lLatestPrePayPosNumber , pTagMinimalPump5->sLatestPrePayPosNumber , sizeof(pTagMinimalPump5->sLatestPrePayPosNumber)); 
		l2a(m_lLatestPrePaySyncNumber , pTagMinimalPump5->sLatestPrePaySyncNumber, sizeof(pTagMinimalPump5->sLatestPrePaySyncNumber));
		//3.2.1.46
		pTagMinimalPump5->sPumpTransactionWasOffer = (IsExistTransactionOnOffer()) ? '1': '0';

		//4.0.2.23
		//If system is down, but PAK finalization is still allowed,
		//we would like to keep the POSPump pump display active, but 
		//we don't want to display a normal pump icon, so we change the 
		//pump status sent to POSPump to COMMS_TERNINATED, WITHOUT CHANGING
		//THE REAL PUMP STATUS!!!  We do this for all pumps, regardless of whether
		//they have transactions on them, in order to avoid the situation where
		//some pumps have a COMMS_TERMINATED icon, while others do not.
		if(_Module.m_server.ISPAKFinalizationFlagSet())
			l2a(COMMS_TERMINATED , (unsigned char*)pTagMinimalPump5->sStatus , sizeof(pTagMinimalPump5->sStatus));
		

		//4.0.3.28
		long lMode = GetStat().GetMode();
		//4.0.3.28
		if (GetStat().GetStatus() == REQUESTING)
		{
			if (MODE_VALID(lMode) && _Module.m_server.m_cModeArray[lMode - 1].AutoAuthorize(GetStat().GetRes(), GetStat().GetResExt()))
				pTagMinimalPump5->sTransactionAutoAuthorize = '1';
		}
		//4.0.3.28
		//4.0.3.33
		if (GetStat().GetStatus() == READY && GetStat().GetRes() == PAY_AT_PUMP)
		{
			if (MODE_VALID(lMode) && _Module.m_server.m_cModeArray[lMode - 1].AutoAuthorize(GetStat().GetRes(), GetStat().GetResExt()))
				pTagMinimalPump5->sTransactionAutoAuthorize = '1';
		}
		//4.0.3.33

		//4.0.3.60
		if (IsPumpDefineAsTerminal())
			pTagMinimalPump5->sPumpAsTerminal ='1';
		
		//4.0.7.500 - check if the feature is active
		long lRunningValueThreshold = _Module.m_server.GetRunningValueThreshold();
		long lEnhancedRollingUpdate = _Module.m_server.GetEnhancedRollingUpdate();
		long lCurrentRollingUpdateValue = 0;
		long lLastUnpaidRollingUpdateValue = 0;
		long lStoredUnpaidRollingUpdateValue = 0;
		long lLastUnpaidRollingUpdateStatus = 0;
		long lStoredRollingUpdateStatus = 0;
		long lTrsNum = 0;
		long lRollingUpdateValue = 0;
		long lRollingUpdateVolume = 0; // 4.0.25.600 CR 296433

		if( lRunningValueThreshold )
		{
			lCurrentRollingUpdateValue = GetCurrentTrsRollingUpdateValue()/1000;
			if (lEnhancedRollingUpdate)		//TD 37244
			{
				lLastUnpaidRollingUpdateValue = GetPAKLastUnpaidTrsRollingUpdateValue(&lTrsNum);
				lLastUnpaidRollingUpdateStatus = GetUnpaidTrsStatus(lTrsNum);
				lStoredUnpaidRollingUpdateValue = GetPAKStoredUnpaidTrsRollingUpdateValue(&lTrsNum);
				lStoredRollingUpdateStatus = GetUnpaidTrsStatus(lTrsNum);
				lRollingUpdateValue = GetCurrentTrsRollingUpdateValue();
			}


			BOOL bGetRemaining;
			if(_Module.m_server.IsRollingAsRemaning())
				bGetRemaining = TRUE;
			else
				bGetRemaining = FALSE;

			lRollingUpdateValue = GetCurrentTrsRollingUpdateValue(bGetRemaining);
			lRollingUpdateVolume = GetCurrentTrsRollingUpdateVolume(bGetRemaining); // 4.0.25.600 CR 296433

			if(lRollingUpdateValue < 0)
					lRollingUpdateValue = 0;

			if(lRollingUpdateVolume < 0)
					lRollingUpdateVolume = 0;


			if(lCurrentRollingUpdateValue < lRunningValueThreshold)
			{
				lCurrentRollingUpdateValue = 0;
				lRollingUpdateValue = 0;
			}


		}
		
		l2a( lCurrentRollingUpdateValue, pTagMinimalPump5->sRoundedValue, sizeof(pTagMinimalPump5->sRoundedValue));
		l2a( lLastUnpaidRollingUpdateValue, pTagMinimalPump5->sCurrentValue, sizeof(pTagMinimalPump5->sCurrentValue)); //TD 37244
		l2a( lStoredUnpaidRollingUpdateValue, pTagMinimalPump5->sStoredValue, sizeof(pTagMinimalPump5->sStoredValue)); //TD 37244
		l2a( lRollingUpdateValue, pTagMinimalPump5->sRollingValue, sizeof(pTagMinimalPump5->sRollingValue)); //TD 37244
		l2a( lLastUnpaidRollingUpdateStatus, &pTagMinimalPump5->sCurrentTrsStatus, sizeof(pTagMinimalPump5->sCurrentTrsStatus));
		l2a( lStoredRollingUpdateStatus, &pTagMinimalPump5->sStoredTrsStatus, sizeof(pTagMinimalPump5->sStoredTrsStatus));
		l2a( lRollingUpdateVolume,pTagMinimalPump5->sRollingVolume,sizeof(pTagMinimalPump5->sRollingVolume));  // 4.0.25.600 CR 296433

		memcpy(&pTagMinimalPump5->sCaption,m_strPumpCaption.GetBuffer(MAX_PUMP_CAPTION_LEN),min(strlen(m_strPumpCaption.GetBuffer(MAX_PUMP_CAPTION_LEN)),MAX_PUMP_CAPTION_LEN)); //4.0.20.500 -  TD 64967

		//4.0.7.500 - end

		// do not allowed post paid
		if (nPrePayWaiting)   //4.0.9.501
			pTagMinimalPump5->sNoParallelPakPrePayflg = '1';

		//4.0.21.500 - TD 69203
		long lMiscExt = GetStat().GetMiscExt(); 
		l2a(lMiscExt,pTagMinimalPump5->sPumpMiscExt,sizeof(pTagMinimalPump5->sPumpMiscExt));

		GetCurrentTrsTenderType(pTagMinimalPump5->sTenderType); // 4.0.25.600 CR 296433
		

	}
	else
		 _LOGMSG.LogMsg("CPump::GetTagMinimal Received unknown version");

}


int CPump::GetTrsByNumber(LONG lTrsNumber, CPumpTransact * pTrs)
{
	int iRetNum = 1; // not found	
	int iArraySize = m_cTrsArray.GetSize();
		
	if((iArraySize)&&(lTrsNumber != 0))
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].m_lNumber == lTrsNumber)
			{
				iRetNum = 0; // found
				if(pTrs != NULL)
					*pTrs = m_cTrsArray[i];
				break;
			}
		}
	}
	
	return iRetNum;
}


int CPump::GetTrsByLinkNumber(CPumpTransact * pTrs)
{
	int iRetNum = 1; // not found	
	int iArraySize = m_cTrsArray.GetSize();
	
	if((iArraySize)&&(pTrs->GetLinkNumber() != 0))
	{
		for (int i=0; i<iArraySize; i++)
		{
			if((m_cTrsArray[i].GetLinkFlags() == pTrs->GetLinkFlags()) &&
				(m_cTrsArray[i].GetLinkNumber() == pTrs->GetLinkNumber()) )
			{
				iRetNum = 0; // found
				if(pTrs != NULL)
					*pTrs = m_cTrsArray[i];
				break;
			}
		}
	}
	
	return iRetNum;
}





int CPump::GetCurrentTrsNumber(long * pTrsNumber)
{
	int iRetNum = 1; // not found	
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].GetStatus() == IN_PROGRESS)
			{
				iRetNum = 0; // found
				*pTrsNumber = m_cTrsArray[i].m_lNumber;
				break;
			}
		}
	}
	return iRetNum;	
}

BOOL CPump::IsCurrentTrs()
{
	BOOL bFound = FALSE;
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].GetStatus() == IN_PROGRESS)
			{	
				bFound = TRUE;
				break;
			}
		}
	}
	return bFound;	
}


int CPump::GetInProgressWaitProgressTrs(CPumpTransact * pTrs)
{
	int iRetNum = 1; // not found	
	int iArraySize = m_cTrsArray.GetSize();
	
	iRetNum = GetCurrentTrs(pTrs);

	if(iRetNum)
	{
		if(iArraySize)
		{
			for (int i=0; i<iArraySize; i++)
			{
				if(m_cTrsArray[i].GetStatus() == WAIT_PROGRESS)
				{
					iRetNum = 0; // found
					*pTrs = m_cTrsArray[i];
					break;
				}
			}
		}
	}

	return iRetNum;	
}


int CPump::GetCurrentTrs(CPumpTransact * pTrs)
{
	int iRetNum = 1; // not found	
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].GetStatus() == IN_PROGRESS)
			{
				iRetNum = 0; // found
				*pTrs = m_cTrsArray[i];
				break;
			}
		}
	}
	return iRetNum;	
}


int CPump::ClearTrs(long lTrsNumber)
{
	int iRetNum=1; // not found
	int iArraySize = m_cTrsArray.GetSize();

	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].m_lNumber == lTrsNumber)
			{
				m_cTrsArray[i].Clear();
				UpdateVer();
			
				iRetNum = 0;
				break;
			}			
		}	
	}
	return iRetNum;
}

/******************************************************************************
 Description: AGIP - is some cases on mode change and prices change 
				we will do the action only if pump is idle and there is no prp trans
				but if we have unpaid trans and them from pak it's not make
				any diffrents.in other words if the pump idle but she only have unpaid pak
				we still continue the action
 Returns:      	bool
 Parameters:   	
 Assumptions:   
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			21/06/2005				Start
******************************************************************************/
BOOL CPump::CheckIfAllUnpaidArePAK()
{
	int i=0;
	BOOL bRetCode = TRUE;
	int iArraySize = m_cTrsArray.GetSize();
	
	short nUnpaid	= 0;
	
	if(iArraySize)
	{
		while(i<iArraySize)
		{
			long lStat; 
			lStat = m_cTrsArray[i].GetStatus();
			if(lStat==UNPAID)
			{
				if(m_cTrsArray[i].GetRes() != PAY_AT_KIOSK)
				{
					bRetCode = FALSE;
					break;
				}
			
			}
			i++;
		}
	}
	return bRetCode;
}

void CPump::GetTrsListCount(short * pPrePayWaiting, short * pFree, short *pUnpaid)
{	
	int iArraySize = m_cTrsArray.GetSize();
	short nPrePayWaiting = 0;
	short nFree		= 0;
	short nUnpaid	= 0;
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			long lStat, lMisc; 
			lStat = m_cTrsArray[i].GetStatus();
			lMisc = m_cTrsArray[i].m_lMisc; //4.0.17.500 - TD 26384


			// don't count the post void as post pay trs
			if ((lMisc & MISC_POST_VOID) && (m_dwMaxPostVoidTrsAllowed > 0))
				continue;

			switch(lStat)
			{
				case WAIT_RES:
				case WAIT_AUTH:
				//4.0.0.39 Eyal WAIT_TIME_OUT added here
				case WAIT_TIME_OUT:
					if (!(lMisc & MISC_PUMP_CHECK_AND_RESERVE)) //4.0.17.500 - TD 26384
						nPrePayWaiting++;
					break;
				case WAIT_PROGRESS:
					if(m_cTrsArray[i].GetLinkFlags() & TRS_LINK_PREPAY_NUMBER)
						nPrePayWaiting++;
					break;

				case IN_PROGRESS:
					 //if( ( GetStat().GetRes() == PRE_PAY_AT_POS) &&
					//	( (GetStat().GetStatus() == DISPENSING ) )						  
					//	nWaiting++;
					break;

				case UNPAID:
				case ACCEPTED:			
					nUnpaid++;
					break;

				case NULL_TRS:				
				case PAID:
					nFree++;
					break;

				case INCOMPLETE:				
				//4.0.0.39 Eyal WAIT_TIME_OUT removed from here
					break;

				default:	
					break;
			}
		}
	}

	*pPrePayWaiting = nPrePayWaiting;
	*pFree	 = nFree;
	*pUnpaid = nUnpaid;
}

short CPump::GetPostVoidTrsCount() //CR 351687: Enhanced post void transactions
{	
	int				iArraySize = m_cTrsArray.GetSize();
	short			sCounter = 0;
	
	for (int i = 0; i < iArraySize; ++i)
	{
		if ((m_cTrsArray[i].m_lMisc & MISC_POST_VOID) && (m_cTrsArray[i].m_lStatus != PAID))
		{
			++sCounter;
		}
	}
	return sCounter;
}

short CPump::GetNumberOfTrsInList()
{
	int iArraySize	= m_cTrsArray.GetSize();
	short nCount	= (short)iArraySize;
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].GetStatus() == NULL_TRS)
				nCount--;
		}
	}
	return nCount;
}

short CPump::GetNumberOfTimeOutPrePay()
{

	int iArraySize = m_cTrsArray.GetSize();
	short nPrePayWaiting = 0;
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			long lStat; 
			lStat = m_cTrsArray[i].GetStatus();
			switch(lStat)
			{
				case WAIT_TIME_OUT:
				case WAIT_AUTH:
					nPrePayWaiting++;
					break;
				default:								
					break;
			}
		}
	}

	return nPrePayWaiting;

}


short CPump::GetNumberOfNonClosedPrePay()
{

	int iArraySize = m_cTrsArray.GetSize();
	short nPrePayWaiting = 0;
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			long lStat; 
			lStat = m_cTrsArray[i].GetStatus();
			switch(lStat)
			{
				case WAIT_RES:
				case WAIT_AUTH:
				case WAIT_TIME_OUT:
					nPrePayWaiting++;
					break;
				case WAIT_PROGRESS:
					if(m_cTrsArray[i].GetLinkFlags() &  TRS_LINK_PREPAY_NUMBER)
						nPrePayWaiting++;
					break;

				case IN_PROGRESS: //4.0.3.55
					if(m_cTrsArray[i].GetLinkFlags() &  TRS_LINK_PREPAY_NUMBER)
					{
						//A transaction that has not been fuelled
						//might still revert to WAIT_TIME_OUT,
						//leading to two prepay transactions on the
						//same pump.
						if(m_cTrsArray[i].m_lRoundedValue == 0)
							nPrePayWaiting++;
					}
					break;

				case UNPAID:
				case ACCEPTED:			
				case NULL_TRS:				
				case PAID:
				case INCOMPLETE:								
					break;
			}
		}
	}

	return nPrePayWaiting;

}



BOOL CPump::CanReserve()
{
	
		
	if(	(GetStat().GetStatus() == READY) &&
		(GetStat().GetRes() == NULL_RES) &&
		(!IsCurrentTrs()) && 
		(GetStat().GetResExt() == NULL_RES_EXT))  //3.2.1.50

		return TRUE;
	else
		return FALSE;
		
}



BOOL CPump::CanReserveForPrePay()
{
	
	
//	if(	(GetStat().GetStatus() == READY) &&
//		(GetStat().GetRes() == NULL_RES) &&
//		(!IsCurrentTrs()))

	if(m_dwAllowPrePayOnRequestingPump)
	{
		
		if(	 ( (GetStat().GetStatus() == READY) || (GetStat().GetStatus() == REQUESTING)  || (GetStat().GetStatus() ==NOZZLE_LEFT_OUT)) && //4.0.3.50
			 ( (GetStat().GetRes() == NULL_RES) || (GetStat().GetRes() == PAY_AT_KIOSK) ) &&
			 (!IsCurrentTrs())

			)

			return TRUE;
		
		//Eyal 4.0.0.52 (3.2.1.32) start
		//Another situation may arise, where a pump receives its
		//REQUESTING status and the reservation for prepay before
		//the transaction has been switched to WAIT_RES.  
		//The transaction then switches to WAIT_RES after it has
		//the pump has been reserved and in REQUESTING.
		//This problem arises mainly when authorization of a prepay
		//on a REQUESTING pump is allowed.
		else if((GetStat().GetStatus() == REQUESTING) && 
			    (GetStat().GetRes() == PRE_PAY_AT_POS))
				return TRUE;

		else
			return FALSE;
	}
	else
		return (CanReserve());

		
}

BOOL CPump::PumpValidForPrePay()
{
	long lMode = GetStat().GetMode();
	if (MODE_NOT_VALID(lMode))
		return FALSE;

	// Check if Pump in working state
	if ((GetStat().GetStatus() == OUT_OF_SERVICE) ||
		(GetStat().GetStatus() == UNKNOWN_STATUS) ||
		(GetStat().GetStatus() == COMMS_TERMINATED))

		return FALSE;
	// check if current mode permit PrePay at this pump
	else if ((PUMPSRV_IS_MODE_MASTER) &&
		(_Module.m_server.m_cModeArray[lMode - 1].PrePayNotValid()))
		return FALSE;

	// check if there is place for the new transaction 
	else if ((DWORD)(GetNumberOfNonClosedPrePay()) < m_dwMaxPrePayTrsAllowed)
		return TRUE;
	else
		return FALSE;
}



BOOL CPump::AlreadyReserveForSinglePrePay()
{
	if(	((GetStat().GetStatus() == READY) ||
		 (GetStat().GetStatus() == NOZZLE_LEFT_OUT)) &&
		(GetStat().GetRes() == PRE_PAY_AT_POS) &&
		(GetNumberOfNonClosedPrePay() == 1) &&
		(!IsCurrentTrs()))

		return TRUE;
	else
		return FALSE;		

}


BOOL CPump::GetNextWaitingTrs(CPumpTransact * pTrs)
{
	int iArraySize = m_cTrsArray.GetSize();
	BOOL bFound = FALSE;
	CTime cCurTime;
	
	cCurTime = CTime::GetCurrentTime();
	int iPosition=0;
	TrsStatus stat;
	short nFlags;
	long lNum;


	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{			
			stat	= m_cTrsArray[i].GetStatus();
			nFlags	= m_cTrsArray[i].GetLinkFlags();
			lNum	= m_cTrsArray[i].m_lNumber;

			if(( stat == WAIT_RES) && (nFlags & TRS_LINK_PREPAY_NUMBER))
			{	
				if(m_cTrsArray[i].GetTime() <= cCurTime)
				{
					cCurTime = m_cTrsArray[i].GetTime();
					iPosition = i;
					bFound = TRUE;				
				}	
			}			

			else if(( stat == WAIT_AUTH) && (nFlags & TRS_LINK_PREPAY_NUMBER))
			{
				bFound = TRUE;
				iPosition = i;				
				break;
			}
			else if( ( nFlags & TRS_LINK_PREPAY_NUMBER) &&
					 ( stat == WAIT_PROGRESS) ) 
			{
				bFound = TRUE;
				iPosition = i;				
				break;
			}
		}
	}


	if(bFound)
		*pTrs = m_cTrsArray[iPosition];

	return bFound;
}


BOOL CPump::GetTrsByStatus(TrsStatus lStatus,short nLinkFlags, CPumpTransact *pTrs)
{
	int iArraySize = m_cTrsArray.GetSize();
	BOOL bFound = FALSE;

	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{		
			if(nLinkFlags == TRS_LINK_ANY)
			{
				if( m_cTrsArray[i].GetStatus() == lStatus)
				{
					bFound = TRUE;
					*pTrs = m_cTrsArray[i];
					break;
				}			
			}
			else
			{
				if( (m_cTrsArray[i].GetStatus() == lStatus) &&
					((m_cTrsArray[i].GetLinkFlags() & nLinkFlags) || 
					(PAK_LINK_TYPE(m_cTrsArray[i].GetLinkFlags()) && PAK_LINK_TYPE(nLinkFlags)))) //3.2.1.60 //4.0.3.50
					//((m_cTrsArray[i].GetLinkFlags() & nLinkFlags) || PAK_LINK_TYPE(m_cTrsArray[i].GetLinkFlags()))) //3.2.1.60 //4.0.3.50
				{
					bFound = TRUE;
					*pTrs = m_cTrsArray[i];
					break;
				}	
			}			
		}
	}

	return bFound;
}
 
void CPump::SetName(char * pName, int iBuffLen)
{	
	m_strPumpMenufacture.Format("%*s",iBuffLen,pName);
}

void CPump::SetType(long lType)
{
	m_lType = lType;
}


long CPump::GetShiftNumber()
{
	return m_lShiftNumber;
}

LockStatus CPump::GetLockStatus()
{
	return m_lLockStatus;
}

void CPump::SetLockStatus(LockStatus lStatus, long lMisc)
{

	m_lLockStatus = lStatus;
	if(lMisc != MISC_IGNORE_FIELD_VALUE)
		m_cPumpStat.SetMisc(lMisc);

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("(Reason %d) Lock State: ",m_cPumpStat.GetResExt());
		switch(lStatus)
		{
			case STARTED: str += "STARTED"; break;
			case WAIT_LOCK: str += "WAIT_LOCK"; break;
			case LOCKED: str += "LOCKED"; break;
			case WAIT_RESPONSE: str += "WAIT_RESPONSE"; break;
			case RESPONSE_RECEIVED: str += "RESPONSE_RECEIVED"; break;
			case UNLOCK: str += "UNLOCK"; break;
		}	
		
		if(lMisc != MISC_IGNORE_FIELD_VALUE)
		{
			str += " Misc: ";
			if(lMisc & MISC_WAIT_LOCK_TIMEOUT)
				str += "MISC_WAIT_LOCK_TIMEOUT ";
			if(lMisc & MISC_WAIT_RESPONSE_TIMEOUT)
				str += "MISC_WAIT_RESPONSE_TIMEOUT ";
			if(lMisc & MISC_OPEN_PRE_PAY_TRS)
				str += "MISC_OPEN_PRE_PAY_TRS ";
			if( lMisc & MISC_OPEN_PAY_AT_KIOSK_TRS)
				str += "MISC_OPEN_PAY_AT_KIOSK_TRS ";
			if( lMisc & MISC_OPEN_PAY_AT_PUMP_TRS)
				str += "MISC_OPEN_PAY_AT_PUMP_TRS ";
			if( lMisc & MISC_WAIT_FOR_PUMP_TO_BE_READY)
				str += "MISC_WAIT_FOR_PUMP_TO_BE_READY ";
		}
			
		 _LOGMSG.LogMsg(m_lNumber,LOG_PUMP,str);
	}
}

void CPump::SetShiftNumber(long lNumber)
{
	m_lShiftNumber = lNumber;
}

void CPump::SetPAPInfo(const PAY_AT_PUMP_INFO *pPAPInfo)
{//4.0.18.501
//	memset(&m_cPAPInfo,0,sizeof(m_cPAPInfo));
//	_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml((char*)m_cPAPInfo,pPAPInfo);

	m_cPAPInfo =  *pPAPInfo;
}


void CPump::GetPAPInfo(PAY_AT_PUMP_INFO *pPAPInfo) const
{
	*pPAPInfo =  m_cPAPInfo;	
}

//4.0.25.50 - TD 108311
long CPump::GetCurrentTrsRollingUpdateVolumeByNozzle(short nNozzle)
{
	long lRoundedVolume = 0;
	
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if((m_cTrsArray[i].GetStatus() == IN_PROGRESS) && (m_cTrsArray[i].m_nNozzle == nNozzle))
			{
				lRoundedVolume = m_cTrsArray[i].m_lRoundedVolume;
				break;
			}
		}
	}

	return lRoundedVolume;
}

//TD 37244
long CPump::GetPAKLastUnpaidTrsRollingUpdateValue(long * lTrsNum)
{
	long lRoundedValue = 0;
	long lLastTrsNumber = 0;
	
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(((m_cTrsArray[i].GetStatus() == UNPAID) || (m_cTrsArray[i].GetStatus() == ACCEPTED)) && (m_cTrsArray[i].GetRes() == PAY_AT_KIOSK) 
				&& (m_cTrsArray[i].m_lNumber > lLastTrsNumber) && (0 == (MISC2_HIDDEN_TRS & m_cTrsArray[i].m_lMisc2))) //CR 351687: Enhanced post void transactions
			{
				lRoundedValue = m_cTrsArray[i].m_lRoundedValue;
				lLastTrsNumber = m_cTrsArray[i].m_lNumber;
			}
		}
	}
	*lTrsNum = lLastTrsNumber;
	return lRoundedValue;
}


//Liat TD 37244

long CPump::GetUnpaidTrsStatus(long lTrsNum)
{
	long nLastStatus = 0;


	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].m_lNumber == lTrsNum) //UNPAID 
			{
				if( m_cTrsArray[i].m_lMisc2 & MISC2_HIDDEN_TRS )
				{
					nLastStatus =  COLORLESS; // 0; Cannot be seen on general TRS screen //CR 351687: Enhanced post void transactions
				}
				else if ( (m_cTrsArray[i].m_lMisc & MISC_POST_VOID) && (PAID != m_cTrsArray[i].GetStatus()) && (ACCEPTED != m_cTrsArray[i].GetStatus()))
				{
					//char logBuf[500];
					//sprintf(logBuf, "GetUnpaidTrsStatus: PINK misc[%ld] status[%ld]", m_cTrsArray[i].m_lMisc, m_cTrsArray[i].GetStatus());
					//_LOGMSG.LogMsg(logBuf);
					nLastStatus =  PINK; // 9; //CR 351687: Enhanced post void transactions
				}

				if(m_cTrsArray[i].GetStatus() == ACCEPTED) // && m_cTrsArray[i].GetStatus() == UNPAID)
				{
					//Unpaid And Accepted Trs
					nLastStatus =  GREY; // 3; //Grey
				}

				else if( (m_cTrsArray[i].GetStatus() == UNPAID) &&
					(m_cTrsArray[i].m_lMisc & MISC_TRS_PAP_CANCELED) &&
					(m_cTrsArray[i].GetOnOffer() == 0) && (_Module.m_server.IsHotPumpingActive() || GetDieselPumpFlags()))
				{
// 					if (m_cTrsArray[i].m_lMisc & MISC_UNPAID_TRS_TIMEOUT)
// 					{
							//Hot pumping trs
							nLastStatus = BLUE;	//4; //Blue

// 					}
				}

				else if( (m_cTrsArray[i].GetStatus() == UNPAID) &&
					(m_cTrsArray[i].GetOnOffer() == 0) &&
					(m_cTrsArray[i].m_lMisc & MISC_UNPAID_TRS_TIMEOUT))
				{
					if(!(m_cTrsArray[i].m_lMisc & MISC_SUPPRESSED_UNPAID_TRS_TIMEOUT))
					{
						if (m_cTrsArray[i].m_lMisc & MISC_TRS_FAILED_CAT)	//4.0.22.508 75592
						{
							//Failed CAT trs
							nLastStatus = PURPLE;	//2; //Red

						}
						else
						{
							//Timeout trs
							nLastStatus = RED;	//2; //Red

						}
					}
					else
						nLastStatus = YELLOW;	//1; //Yellow
				}
				else if( (m_cTrsArray[i].GetStatus() == UNPAID) &&
					(m_cTrsArray[i].GetOnOffer() == 0) )
				{
					if (m_cTrsArray[i].m_lMisc & MISC_TRS_FAILED_CAT)	//4.0.22.508 75592
					{
						//Failed CAT trs
						nLastStatus = PURPLE;	//2; //Red

					}
					else if (m_cTrsArray[i].m_lMisc & MISC_TRS_BLOCKED_ANPR) //4.0.26.505 157073
					{
						nLastStatus = BLACK;	//7; 
						
					}
					else if (m_cTrsArray[i].m_lMisc & MISC_TRS_AUTH_WITH_FAILED_ANPR) //4.0.26.505 157073
					{
						nLastStatus = TURQUOISE; //8
					}
					else
					{
						nLastStatus = YELLOW;	//1; //Yellow
					}
				}
				else if ((m_cTrsArray[i].GetStatus() == UNPAID) && (m_cTrsArray[i].GetOnOffer() != 0))	//4.0.22.508 75592
				{
					nLastStatus =  GREY; // 3; //Grey
				}

			}
		}
	}
	return nLastStatus;
}


//Liat TD 37244
long CPump::GetPAKStoredUnpaidTrsRollingUpdateValue(long * lTrsNum)
{
	long lRoundedValue = 0;
	long lLastTrsNumber = 0;
	long lLastUnpaidTrs = 0;
	GetPAKLastUnpaidTrsRollingUpdateValue(&lLastUnpaidTrs);
	
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(((m_cTrsArray[i].GetStatus() == UNPAID) || (m_cTrsArray[i].GetStatus() == ACCEPTED)) && (m_cTrsArray[i].GetRes() == PAY_AT_KIOSK) && 
				(m_cTrsArray[i].m_lNumber > lLastTrsNumber) && (m_cTrsArray[i].m_lNumber < lLastUnpaidTrs) && (0 == (MISC2_HIDDEN_TRS & m_cTrsArray[i].m_lMisc2))) //CR 351687: Enhanced post void transactions
			{
				lRoundedValue = m_cTrsArray[i].m_lRoundedValue;
				lLastTrsNumber = m_cTrsArray[i].m_lNumber;
			}
		}
	}
	*lTrsNum = lLastTrsNumber;
	return lRoundedValue;
}
/////////////////////////////////////////////////////////////////////////////
// CPumpStat

CPumpStatus CPump::GetStat()
{
	return(m_cPumpStat);
}


CPumpStatus::CPumpStatus()
{
	m_lError		= 0;
	m_lMisc			= 0;
	m_lMode			= DEFAULT_MODE;
	m_lRes			= NULL_RES;
	m_lResExt		= NULL_RES_EXT;
	m_lStatus		= UNKNOWN_STATUS;
	m_lDevice		= NO_DEVICE;
	m_lWarning		= 0;
	m_lServiceMode = SELF_SERVICE_MODE;
	m_lMessage		= PUMP_NO_MSG;
	m_cTime			= CTime::GetCurrentTime();		//4.0.17.20 - TD 26384
	m_nGrade		= 0;	//4.0.19.500 - TD 59171
	m_nPosition		= 0;	//4.0.19.500 - TD 59171
	m_lMiscExt		= 0;	//4.0.21.500 69203
}

void CPumpStatus::operator =( const CPumpStatus& pumpStat )
{
	m_lError		= pumpStat.m_lError;
	m_lMisc			= pumpStat.m_lMisc;
	m_lMode			= pumpStat.m_lMode;
	m_lRes			= pumpStat.m_lRes;
	m_lResExt		= pumpStat.m_lResExt;
	m_lStatus		= pumpStat.m_lStatus;
	m_lDevice		= pumpStat.m_lDevice;
	m_lWarning		= pumpStat.m_lWarning;
	m_lServiceMode	= pumpStat.m_lServiceMode;
	m_lMessage		= pumpStat.m_lMessage;
	m_cTime			= pumpStat.m_cTime;		//4.0.17.20 - TD 26384
	m_nGrade		= pumpStat.m_nGrade;		//4.0.19.500 - TD 59171
	m_nPosition		= pumpStat.m_nPosition;		//4.0.19.500 - TD 59171
	m_lMiscExt		= pumpStat.m_lMiscExt;		//4.0.21.501 65268 70731
}

void CPumpStatus::operator =( TAG_PUMP_STATUS & tagPumpStat )
{
	m_lError		= a2l(tagPumpStat.sError,sizeof(tagPumpStat.sError));
	m_lMisc			= a2l(tagPumpStat.sMisc,sizeof(tagPumpStat.sMisc));
	m_lMode			= a2l(tagPumpStat.sMode,sizeof(tagPumpStat.sMode));
	m_lRes			= l2PumpReservation(a2l(tagPumpStat.sRes,sizeof(tagPumpStat.sRes)));
	m_lResExt		= l2PumpResExt(a2l(tagPumpStat.sResExt,sizeof(tagPumpStat.sResExt)));
	m_lStatus		= l2PumpStatus(a2l(tagPumpStat.sStatus,sizeof(tagPumpStat.sStatus)));
	m_lDevice		= l2PumpDevice(a2l(tagPumpStat.sDevice,sizeof(tagPumpStat.sDevice)));
	m_lWarning		= a2l(tagPumpStat.sWarning,sizeof(tagPumpStat.sWarning));
}

void CPumpStatus::operator =( TAG_PUMP_STATUS2 & tagPumpStat )
{
	m_lError		= a2l(tagPumpStat.sError,sizeof(tagPumpStat.sError));
	m_lMisc			= a2l(tagPumpStat.sMisc,sizeof(tagPumpStat.sMisc));
	m_lMode			= a2l(tagPumpStat.sMode,sizeof(tagPumpStat.sMode));
	m_lRes			= l2PumpReservation(a2l(tagPumpStat.sRes,sizeof(tagPumpStat.sRes)));
	m_lResExt		= l2PumpResExt(a2l(tagPumpStat.sResExt,sizeof(tagPumpStat.sResExt)));
	m_lStatus		= l2PumpStatus(a2l(tagPumpStat.sStatus,sizeof(tagPumpStat.sStatus)));
	m_lDevice		= l2PumpDevice(a2l(tagPumpStat.sDevice,sizeof(tagPumpStat.sDevice)));
	m_lWarning		= a2l(tagPumpStat.sWarning,sizeof(tagPumpStat.sWarning));
	m_lServiceMode  = a2l(tagPumpStat.sServiceMode,sizeof(tagPumpStat.sServiceMode));
	m_lMessage		= a2l(tagPumpStat.sMessage,sizeof(tagPumpStat.sMessage));
}



void CPumpStatus::GetTag(void *pBuff, long lVersion)
{
	TAG_PUMP_STATUS *pTagPumpStat;

	pTagPumpStat = (TAG_PUMP_STATUS *)pBuff;

	l2a(m_lMode, pTagPumpStat->sMode, sizeof(pTagPumpStat->sMode));//=	m_lMode;
	l2a(m_lRes, pTagPumpStat->sRes, sizeof(pTagPumpStat->sRes));//=	m_lRes;
	l2a(m_lResExt, pTagPumpStat->sResExt, sizeof(pTagPumpStat->sResExt));//=	m_lResExt;
	l2a(m_lStatus, pTagPumpStat->sStatus, sizeof(pTagPumpStat->sStatus));//=	m_lStatus;
	l2a(m_lDevice, pTagPumpStat->sDevice, sizeof(pTagPumpStat->sDevice));//=	m_lDevice;
	l2a(m_lWarning, pTagPumpStat->sWarning, sizeof(pTagPumpStat->sWarning));//=	m_lWarning;

	l2a(m_lError, pTagPumpStat->sError, sizeof(pTagPumpStat->sError));//=	m_lError;

	l2a((m_lMisc & 0x0000ffff), pTagPumpStat->sMisc, sizeof(pTagPumpStat->sMisc));//=	m_lMisc; //3.0.3.11

	if (lVersion == 2 || lVersion == 3) //3.0.1.61
	{
		TAG_PUMP_STATUS2 *pTagPumpStat2;
		pTagPumpStat2 = (TAG_PUMP_STATUS2 *)pBuff;
		long lFlags = 0;
		if (MODE_VALID(m_lMode))
			lFlags = _Module.m_server.m_cModeArray[m_lMode - 1].GetFlags();

		l2a(lFlags, pTagPumpStat2->sModeInfo, sizeof(pTagPumpStat2->sModeInfo));
		l2a(m_lServiceMode, pTagPumpStat2->sServiceMode, sizeof(pTagPumpStat2->sServiceMode));
		l2a(m_lMessage, pTagPumpStat2->sMessage, sizeof(pTagPumpStat2->sMessage));
	}

	//4.0.3.14
	//versions 4 and up use the same version of TAG_PUMP_STATUS
	//so lVersion == 4 has been changed to lVersion >= 4
	else if (lVersion >= 4)
	{
		TAG_PUMP_STATUS3 *pTagPumpStat3;
		pTagPumpStat3 = (TAG_PUMP_STATUS3 *)pBuff;

		long lFlags = 0;
		if (MODE_VALID(m_lMode))
			lFlags = _Module.m_server.m_cModeArray[m_lMode - 1].GetFlags();

		l2a((m_lMisc), pTagPumpStat3->sMisc, sizeof(pTagPumpStat3->sMisc));
		l2a(lFlags, pTagPumpStat3->sModeInfo, sizeof(pTagPumpStat3->sModeInfo));
		l2a(m_lServiceMode, pTagPumpStat3->sServiceMode, sizeof(pTagPumpStat3->sServiceMode));
		l2a(m_lMessage, pTagPumpStat3->sMessage, sizeof(pTagPumpStat3->sMessage));
		l2a(m_nGrade, pTagPumpStat3->sGrade, sizeof(pTagPumpStat3->sGrade));				//4.0.19.500 - TD 59171
		l2a(m_nPosition, pTagPumpStat3->sPosition, sizeof(pTagPumpStat3->sPosition));	//4.0.19.500 - TD 59171
		l2a(m_lMiscExt & 0x0000ffff, pTagPumpStat3->sMiscExt, sizeof(pTagPumpStat3->sMiscExt));	//4.0.21.500 69203

	}

	//IMPORTANT!!!!!!!
	//If you add another version, change the lVersion >= 4 above to lVersion == 4

}

PumpStatus CPumpStatus::GetStatus()
{
	return m_lStatus;
}

void CPumpStatus::SetStatus(PumpStatus status)
{
	if(m_lStatus != status)
	{
		m_lStatus = status;		
//		UpdateTime();
	}
}


CPumpStatus::CPumpStatus(const CPumpStatus & pumpStat)
{
	m_lError		= pumpStat.m_lError;
	m_lMisc			= pumpStat.m_lMisc;
	m_lMode			= pumpStat.m_lMode;
	m_lRes			= pumpStat.m_lRes;
	m_lResExt		= pumpStat.m_lResExt;
	m_lStatus		= pumpStat.m_lStatus;
	m_lDevice		= pumpStat.m_lDevice;
	m_lWarning		= pumpStat.m_lWarning;	
	m_lServiceMode	= pumpStat.m_lServiceMode;
	m_lMessage		= pumpStat.m_lMessage;
	m_cTime			= pumpStat.m_cTime;
	m_nGrade		= pumpStat.m_nGrade;		//4.0.19.500 - TD 59171
	m_nPosition		= pumpStat.m_nPosition;		//4.0.19.500 - TD 59171
	m_lMiscExt		= pumpStat.m_lMiscExt;		//4.0.21.500 69203
}

void CPumpStatus::SetRes(PumpReservation res, PumpResExt lResExt)
{
	m_lRes = res;
	m_lResExt = lResExt;
}

PumpReservation CPumpStatus::GetRes()
{
	return(m_lRes);
}


long CPumpStatus::GetMode()
{
	return m_lMode;
}

long CPumpStatus::GetServiceMode() const
{
	return m_lServiceMode;
}

PumpDevice	CPumpStatus::GetDevice()
{
	return m_lDevice;
}

void CPumpStatus::SetMode(long lMode)
{
	m_lMode = lMode;
}

void CPumpStatus::SetServiceMode(long lServiceMode)
{
	m_lServiceMode = lServiceMode;
}

PumpResExt CPumpStatus::GetResExt()
{
	return m_lResExt;
}

/////////////////////////////////////////////////////////////////////////////
// CPumpTransact

CPumpTransact::CPumpTransact()
{

	m_lNumber			= 0L;
	m_lStatus			= NULL_TRS;
	m_lOnOffer			= 0L;
	m_lRawVolume        = 0L;
	m_lRoundedVolume	= 0L;
	m_lRoundedValue		= 0L;
	m_lGradePrice		= 0L;
	m_nLevel			= 0;
	m_nNozzle			= 0;
	m_nGrade			= 0;
	m_lPump				= NO_PUMP_NUMBER;
	m_lMode				= DEFAULT_MODE;
	m_lRes				= NULL_RES;
	m_lResExt			= NULL_RES_EXT;

	m_lLinkNumber		= 0L;
	m_nLinkFlags		= TRS_NO_LINK;
	m_cTime				= CTime::GetCurrentTime();
	m_cStartTime		= m_cTime; //TD 40492 //TD 38654
	m_cStartFuelingTime		= m_cTime; //4.0.22.511 72606
	m_lActiveReadNumber		=_Module.m_server.GetActiveReadNumber();
	m_lActiveShiftNumber	= 0L;
	m_lMisc				= 0L;
	m_lControllerSeqNum = 0L;	//4.0.18.660 68689	//4.0.21.502

	memset(m_sAttendantID, ' ', sizeof(m_sAttendantID));								//4.0.22.10 TD 71055
	memset(m_sAttendantName, ' ', sizeof(m_sAttendantName));							//4.0.22.10 TD 71055
	memset(m_sFullVehicleId, ' ', sizeof(m_sFullVehicleId));							//4.0.23.500 TD 69914	

	memset(m_sFiller, ' ', sizeof(m_sFiller));//4.0.22.510
	m_sReportPaid = ' '; //4.0.23.650 133966 //4.0.25.50
	memset(m_sAttendantTag, ' ', sizeof(m_sAttendantTag));		//4.0.25.501 TD 137509

	m_sPumpTest = '0';	 //4.0.25.60  	 TD 231472
	m_lTankID = 0L;      //4.0.25.60  	 TD 231475
	m_nAutoReconcileTimeout = 0; // TD 233242 4.0.23.1050
	m_lLowestGradePrice = 0L;
	m_sLoyaltyPriceProtection = '0';
	m_sLoyaltyAutoArm = '0';

	m_lMaxValue = 0;								 // 4.0.25.600 CR 296433
	m_lMaxVolume = 0;								 // 4.0.25.600 CR 296433			
	memset(m_sTenderType,' ',sizeof(m_sTenderType)); // 4.0.25.600 CR 296433

	m_lMisc2  = 0L;		//CR 351687: Enhanced post void transactions

	m_sTypeOfMeasure = '0'; // TD 363041
	memset(m_sMaxDiscountAllowed,' ', sizeof(m_sMaxDiscountAllowed));  //RFUEL-3309 Get the maximal discount allowed for the grade 
	
	// RFUEL-2817
	m_sLoyaltyDiscountApplied = ' ';
}

CPumpTransact::CPumpTransact(long lPumpNumber)
{
	m_lNumber			= 0L;
	m_lStatus			= NULL_TRS;
	m_lOnOffer			= 0L;
	m_lRawVolume        = 0L;
	m_lRoundedVolume	= 0L;
	m_lRoundedValue		= 0L;
	m_lGradePrice		= 0L;
	m_nLevel			= 0;
	m_nNozzle			= 0;
	m_nGrade			= 0;
	m_lPump				= lPumpNumber;
	m_lMode				= DEFAULT_MODE;
	m_lRes				= NULL_RES;
	m_lResExt			= NULL_RES_EXT;

	m_lLinkNumber		= 0L;
	m_nLinkFlags		= TRS_NO_LINK;
	m_cTime				= CTime::GetCurrentTime();
	m_cStartTime		= m_cTime;	//TD 40492 //TD 38654
	m_cStartFuelingTime		= m_cTime; //4.0.22.511 72606
	m_lActiveReadNumber		=_Module.m_server.GetActiveReadNumber();
	m_lActiveShiftNumber	= 0L;
	m_lMisc				= 0L;
	m_lMisc2  = 0L;		//CR 351687: Enhanced post void transactions
	m_sTypeOfMeasure = '0'; // TD 363041
	m_lControllerSeqNum = 0L;	//4.0.18.660 68689	//4.0.21.502

	memset(m_sAttendantID, ' ', sizeof(m_sAttendantID));								//4.0.22.10 TD 71055
	memset(m_sAttendantName, ' ', sizeof(m_sAttendantName));							//4.0.22.10 TD 71055
	memset(m_sFullVehicleId, ' ', sizeof(m_sFullVehicleId));							//4.0.23.500 TD 69914

	memset(m_sFiller, ' ', sizeof(m_sFiller));//4.0.22.510
	m_sReportPaid = ' '; //4.0.23.650 133966 //4.0.25.50
	memset(m_sAttendantTag, ' ', sizeof(m_sAttendantTag));		//4.0.25.501 TD 137509

	
	m_sPumpTest = '0';		 //4.0.25.60  	TD 231472	
	m_lTankID = 0L;		     //4.0.25.60  	TD 231475

	m_nAutoReconcileTimeout = 0; // TD 233242 4.0.23.1050
	m_lLowestGradePrice = 0L;
	m_sLoyaltyPriceProtection = '0';
	m_sLoyaltyAutoArm = '0';

	m_lMaxValue = 0;								 // 4.0.25.600 CR 296433
	m_lMaxVolume = 0;								 // 4.0.25.600 CR 296433			
	memset(m_sTenderType,' ',sizeof(m_sTenderType)); // 4.0.25.600 CR 296433
	memset(m_sMaxDiscountAllowed, ' ', sizeof(m_sMaxDiscountAllowed));  //RFUEL-3309 Get the maximal discount allowed for the grade 

	// RFUEL-2817
	m_sLoyaltyDiscountApplied = ' ';
}

CPumpTransact::CPumpTransact(const CPumpTransact& pumpTrs)
{
	m_lNumber			= pumpTrs.m_lNumber;			
	m_lStatus			= pumpTrs.m_lStatus;
	m_lOnOffer			= pumpTrs.m_lOnOffer;
	m_lRawVolume        = pumpTrs.m_lRawVolume;
	m_lRoundedVolume	= pumpTrs.m_lRoundedVolume;
	m_lRoundedValue		= pumpTrs.m_lRoundedValue;		
	m_lGradePrice		= pumpTrs.m_lGradePrice;		
	m_nNozzle			= pumpTrs.m_nNozzle;		
	m_nGrade			= pumpTrs.m_nGrade;		
	m_nLevel			= pumpTrs.m_nLevel;
	
	m_lPump				= pumpTrs.m_lPump;		
	m_lMode				= pumpTrs.m_lMode;			
	m_lRes				= pumpTrs.m_lRes;
	m_lResExt			= pumpTrs.m_lResExt;

	m_lLinkNumber		= pumpTrs.m_lLinkNumber;
	m_nLinkFlags		= pumpTrs.m_nLinkFlags;

	m_cTime				= pumpTrs.m_cTime;
	m_cStartTime		= pumpTrs.m_cStartTime;	//TD 40492 //TD 38654
	m_cStartFuelingTime	= pumpTrs.m_cStartFuelingTime; //4.0.22.511 72606

	m_lActiveReadNumber	= pumpTrs.m_lActiveReadNumber;
	m_lActiveShiftNumber	= pumpTrs.m_lActiveShiftNumber;
	m_lMisc				= pumpTrs.m_lMisc;
	m_lControllerSeqNum = pumpTrs.m_lControllerSeqNum;	//4.0.18.660 68689	//4.0.21.502

	memcpy(m_sAttendantID, pumpTrs.m_sAttendantID, sizeof(m_sAttendantID));					//4.0.22.10 TD 71055
	memcpy(m_sAttendantName, pumpTrs.m_sAttendantName, sizeof(m_sAttendantName));			//4.0.22.10 TD 71055
	memcpy(m_sFullVehicleId, pumpTrs.m_sFullVehicleId, sizeof(m_sFullVehicleId));			//4.0.23.500 TD 69914

	memset(m_sFiller, ' ', sizeof(m_sFiller));//4.0.22.510
	m_sReportPaid = pumpTrs.m_sReportPaid; //4.0.23.650 133966 //4.0.25.50
	memcpy(m_sAttendantTag, pumpTrs.m_sAttendantTag, sizeof(m_sAttendantTag));		//4.0.25.501 TD 137509

	m_sPumpTest = pumpTrs.m_sPumpTest; //4.0.27.60  	TD 231472
	m_lTankID = pumpTrs.m_lTankID;	   //4.0.27.60  	TD 231475

	m_nAutoReconcileTimeout = pumpTrs.m_nAutoReconcileTimeout; //  TD 233242 4.0.23.1050
	m_lLowestGradePrice = pumpTrs.m_lLowestGradePrice;
	m_sLoyaltyPriceProtection = pumpTrs.m_sLoyaltyPriceProtection;
	m_sLoyaltyAutoArm = pumpTrs.m_sLoyaltyAutoArm;

	m_lMaxValue = pumpTrs.m_lMaxValue;								 // 4.0.25.600 CR 296433
	m_lMaxVolume = pumpTrs.m_lMaxVolume;						    // 4.0.25.600 CR 296433			
	memcpy(m_sTenderType,pumpTrs.m_sTenderType,sizeof(m_sTenderType)); // 4.0.25.600 CR 296433

	m_lMisc2  = pumpTrs.m_lMisc2;		//CR 351687: Enhanced post void transactions
	m_sTypeOfMeasure = pumpTrs.m_sTypeOfMeasure; // TD 363041
	memcpy(m_sMaxDiscountAllowed, pumpTrs.m_sMaxDiscountAllowed, sizeof(m_sMaxDiscountAllowed)); //RFUEL-3309 Get the maximal discount allowed for the grade 

	//RFUEL-2817
	m_sLoyaltyDiscountApplied = pumpTrs.m_sLoyaltyDiscountApplied;
}

CPumpTransact::CPumpTransact(const PumpTransactBase& pumpTrs)
{
	m_lNumber = pumpTrs.m_lNumber;
	m_lStatus = pumpTrs.m_lStatus;
	m_lOnOffer = pumpTrs.m_lOnOffer;
	m_lRawVolume = pumpTrs.m_lRawVolume;
	m_lRoundedVolume = pumpTrs.m_lRoundedVolume;
	m_lRoundedValue = pumpTrs.m_lRoundedValue;
	m_lGradePrice = pumpTrs.m_lGradePrice;
	m_nNozzle = pumpTrs.m_nNozzle;
	m_nGrade = pumpTrs.m_nGrade;
	m_nLevel = pumpTrs.m_nLevel;

	m_lPump = pumpTrs.m_lPump;
	m_lMode = pumpTrs.m_lMode;
	m_lRes = pumpTrs.m_lRes;
	m_lResExt = pumpTrs.m_lResExt;

	m_lLinkNumber = pumpTrs.m_lLinkNumber;
	m_nLinkFlags = pumpTrs.m_nLinkFlags;

	m_cTime = pumpTrs.m_cTime;
	m_cStartTime = pumpTrs.m_cStartTime;	//TD 40492 //TD 38654
	m_cStartFuelingTime = pumpTrs.m_cStartFuelingTime; //4.0.22.511 72606

	m_lActiveReadNumber = pumpTrs.m_lActiveReadNumber;
	m_lActiveShiftNumber = pumpTrs.m_lActiveShiftNumber;
	m_lMisc = pumpTrs.m_lMisc;
	m_lControllerSeqNum = pumpTrs.m_lControllerSeqNum;	//4.0.18.660 68689	//4.0.21.502

	memcpy(m_sAttendantID, pumpTrs.m_sAttendantID, sizeof(m_sAttendantID));					//4.0.22.10 TD 71055
	memcpy(m_sAttendantName, pumpTrs.m_sAttendantName, sizeof(m_sAttendantName));			//4.0.22.10 TD 71055
	memcpy(m_sFullVehicleId, pumpTrs.m_sFullVehicleId, sizeof(m_sFullVehicleId));			//4.0.23.500 TD 69914

	memset(m_sFiller, ' ', sizeof(m_sFiller));//4.0.22.510
	m_sReportPaid = pumpTrs.m_sReportPaid; //4.0.23.650 133966 //4.0.25.50
	memcpy(m_sAttendantTag, pumpTrs.m_sAttendantTag, sizeof(m_sAttendantTag));		//4.0.25.501 TD 137509

	m_sPumpTest = pumpTrs.m_sPumpTest; //4.0.27.60  	TD 231472
	m_lTankID = pumpTrs.m_lTankID;	   //4.0.27.60  	TD 231475

	m_nAutoReconcileTimeout = pumpTrs.m_nAutoReconcileTimeout; //  TD 233242 4.0.23.1050
	m_lLowestGradePrice = pumpTrs.m_lLowestGradePrice;
	m_sLoyaltyPriceProtection = pumpTrs.m_sLoyaltyPriceProtection;
	m_sLoyaltyAutoArm = pumpTrs.m_sLoyaltyAutoArm;

	m_lMaxValue = pumpTrs.m_lMaxValue;								 // 4.0.25.600 CR 296433
	m_lMaxVolume = pumpTrs.m_lMaxVolume;						    // 4.0.25.600 CR 296433			
	memcpy(m_sTenderType, pumpTrs.m_sTenderType, sizeof(m_sTenderType)); // 4.0.25.600 CR 296433

	m_lMisc2 = pumpTrs.m_lMisc2;		//CR 351687: Enhanced post void transactions
	m_sTypeOfMeasure = pumpTrs.m_sTypeOfMeasure; // TD 363041
	memcpy(m_sMaxDiscountAllowed, pumpTrs.m_sMaxDiscountAllowed, sizeof(m_sMaxDiscountAllowed)); //RFUEL-3309 Get the maximal discount allowed for the grade 

	// RFUEL-2817
	m_sLoyaltyDiscountApplied = pumpTrs.m_sLoyaltyDiscountApplied;
}

CPumpTransact::CPumpTransact(long lTrsNumber,long lActiveShiftNumber)
{
	m_lNumber			= lTrsNumber;
	m_lStatus			= NULL_TRS;
	m_lOnOffer			= 0L;
	m_lRawVolume        = 0L;
	m_lRoundedVolume	= 0L;
	m_lRoundedValue		= 0L;
	m_lGradePrice		= 0L;
	m_nNozzle			= 0;
	m_nGrade			= 0;
	m_nLevel			= 0;
	
	m_lPump				= NO_PUMP_NUMBER;
	m_lMode				= DEFAULT_MODE;
	m_lRes				= NULL_RES;
	m_lResExt			= NULL_RES_EXT;	

	m_lLinkNumber		= 0L;
	m_nLinkFlags		= TRS_NO_LINK;

	m_cTime				= CTime::GetCurrentTime();
	m_cStartTime		= m_cTime;	//TD 40492 //TD 38654
	m_cStartFuelingTime	= m_cTime;	//4.0.22.511 72606
	m_lActiveReadNumber	=_Module.m_server.GetActiveReadNumber();
	m_lActiveShiftNumber = lActiveShiftNumber;

	m_lMisc				=0L;
	m_lControllerSeqNum = 0L;	//4.0.18.660 68689	//4.0.21.502

	memset(m_sAttendantID, ' ', sizeof(m_sAttendantID));			//4.0.22.10 TD 71055
	memset(m_sAttendantName, ' ', sizeof(m_sAttendantName));		//4.0.22.10 TD 71055
	memset(m_sFullVehicleId, ' ', sizeof(m_sFullVehicleId));		//4.0.23.500 TD 69914

	memset(m_sFiller, ' ', sizeof(m_sFiller));//4.0.22.510
	m_sReportPaid = ' '; //4.0.23.650 133966 //4.0.25.50
	memset(m_sAttendantTag, ' ', sizeof(m_sAttendantTag));		//4.0.25.501 TD 137509

	m_sPumpTest = '0';	        //4.0.25.60  	TD 231472
	m_lTankID		= 0L;		//4.0.25.60  	TD 231475

	m_nAutoReconcileTimeout = 0; //  TD 233242 4.0.23.1050
	m_lLowestGradePrice = 0L;
	m_sLoyaltyPriceProtection = '0';
	m_sLoyaltyAutoArm = '0';

	m_lMaxValue = 0;								 // 4.0.25.600 CR 296433
	m_lMaxVolume = 0;								 // 4.0.25.600 CR 296433			
	memset(m_sTenderType,' ',sizeof(m_sTenderType)); // 4.0.25.600 CR 296433

	m_lMisc2 = 0L;				//CR 351687: Enhanced post void transactions	
	
	m_sTypeOfMeasure = '0'; // TD 363041
	memset(m_sMaxDiscountAllowed, ' ', sizeof(m_sMaxDiscountAllowed)); //RFUEL-3309 Get the maximal discount allowed for the grade 
	
	m_sLoyaltyDiscountApplied = ' ';	
}

void CPumpTransact::UpdateRolling(LONG newVolume, LONG newValue)	
{
	m_lRawVolume = newVolume;
	m_lRoundedVolume = newVolume;
	m_lRoundedValue = newValue;
}


TrsStatus CPumpTransact::GetStatus()
{
	return m_lStatus;
}

//4.0.9.506 //amram
PumpReservation CPumpTransact::GetRes()
{
	return m_lRes;
}

void CPumpTransact::SetStatus(TrsStatus lStatus)
{
	m_lStatus = lStatus;
}

void CPumpTransact::SetOnOffer(LONG lOnOffer)
{
	m_lOnOffer = lOnOffer;
}

LONG CPumpTransact::GetOnOffer()
{
	return(m_lOnOffer);
}

BOOL CPumpTransact::TrsShouldBeReportedPaid() //4.0.23.650 133966 //4.0.25.50
{
	BOOL bReportPaid = FALSE;
	if (m_sReportPaid != ' ')
		bReportPaid = TRUE;
	return bReportPaid;
}

void CPumpTransact::operator =( const CPumpTransact & pumpTrs )
{
	m_lNumber		= pumpTrs.m_lNumber ;
	m_lStatus		= pumpTrs.m_lStatus ;
	m_lOnOffer		= pumpTrs.m_lOnOffer ;
	m_lRawVolume	= pumpTrs.m_lRawVolume ;
	m_lRoundedVolume= pumpTrs.m_lRoundedVolume ;
	m_lRoundedValue = pumpTrs.m_lRoundedValue ;
	m_lGradePrice	= pumpTrs.m_lGradePrice ;

	m_nNozzle		= pumpTrs.m_nNozzle ;
	m_nGrade		= pumpTrs.m_nGrade ;
	m_nLevel		= pumpTrs.m_nLevel;
	
	m_lPump			= pumpTrs.m_lPump ;
	m_lMode			= pumpTrs.m_lMode ;
	m_lRes			= pumpTrs.m_lRes ;
	m_lResExt		= pumpTrs.m_lResExt ;

	m_lLinkNumber	= pumpTrs.m_lLinkNumber;
	m_nLinkFlags	= pumpTrs.m_nLinkFlags;

	m_cTime			= pumpTrs.m_cTime;
	m_cStartTime	= pumpTrs.m_cStartTime;	//TD 40492 //TD 38654
	m_cStartFuelingTime = pumpTrs.m_cStartFuelingTime; //4.0.22.511 72606	
	
	m_lActiveReadNumber		=pumpTrs.m_lActiveReadNumber;
	m_lActiveShiftNumber	=pumpTrs.m_lActiveShiftNumber;
	m_lMisc			= pumpTrs.m_lMisc;
	m_lControllerSeqNum = pumpTrs.m_lControllerSeqNum;	//4.0.18.660 68689 //4.0.21.502

	memcpy(m_sAttendantID, pumpTrs.m_sAttendantID, sizeof(m_sAttendantID));				//4.0.22.10 TD 71055
	memcpy(m_sAttendantName, pumpTrs.m_sAttendantName, sizeof(m_sAttendantName));		//4.0.22.10 TD 71055
	memcpy(m_sFullVehicleId, pumpTrs.m_sFullVehicleId, sizeof(m_sFullVehicleId));		//4.0.23.500 TD 69914

	memset(m_sFiller, ' ', sizeof(m_sFiller));//4.0.22.511 72606
	m_sReportPaid = pumpTrs.m_sReportPaid; //4.0.23.650 133966 //4.0.25.50
	memcpy(m_sAttendantTag, pumpTrs.m_sAttendantTag, sizeof(m_sAttendantTag));		//4.0.25.501 TD 137509

	m_sPumpTest = pumpTrs.m_sPumpTest;     //4.0.25.60  	TD 231472
	m_lTankID = pumpTrs.m_lTankID;         //4.0.25.60  	TD 231475
	m_nAutoReconcileTimeout = pumpTrs.m_nAutoReconcileTimeout; //  TD 233242 4.0.23.1050
	m_lLowestGradePrice = pumpTrs.m_lLowestGradePrice;
	m_sLoyaltyPriceProtection = pumpTrs.m_sLoyaltyPriceProtection;
	m_sLoyaltyAutoArm = pumpTrs.m_sLoyaltyAutoArm;

	m_lMaxValue = pumpTrs.m_lMaxValue;								 // 4.0.25.600 CR 296433
	m_lMaxVolume = pumpTrs.m_lMaxVolume;						    // 4.0.25.600 CR 296433			
	memcpy(m_sTenderType,pumpTrs.m_sTenderType,sizeof(m_sTenderType)); // 4.0.25.600 CR 296433

	m_lMisc2  = pumpTrs.m_lMisc2;			//CR 351687: Enhanced post void transactions
	m_sTypeOfMeasure = pumpTrs.m_sTypeOfMeasure; // TD 363041
	memcpy(m_sMaxDiscountAllowed, pumpTrs.m_sMaxDiscountAllowed, sizeof(m_sMaxDiscountAllowed)); //RFUEL-3309 Get the maximal discount allowed for the grade 

	m_sLoyaltyDiscountApplied = pumpTrs.m_sLoyaltyDiscountApplied;
}

void CPumpTransact::operator =( TAG_PUMP_TRANSACT & tagPumpTrs )
{
	m_lNumber		= a2l(tagPumpTrs.sNumber ,sizeof(tagPumpTrs.sNumber ));
	m_lStatus		= l2TrsStatus(a2l(tagPumpTrs.sStatus ,sizeof(tagPumpTrs.sStatus )));
	m_lOnOffer		= a2l(tagPumpTrs.sOnOffer ,sizeof(tagPumpTrs.sOnOffer ));
	m_lRawVolume	= a2l(tagPumpTrs.sRawVolume ,sizeof(tagPumpTrs.sRawVolume ));
	m_lRoundedVolume= a2l(tagPumpTrs.sRoundedVolume ,sizeof(tagPumpTrs.sRoundedVolume ));
	m_lRoundedValue = a2l(tagPumpTrs.sRoundedValue ,sizeof(tagPumpTrs.sRoundedValue ));
	m_lGradePrice	= a2l(tagPumpTrs.sGradePrice ,sizeof(tagPumpTrs.sGradePrice ));
	m_nNozzle		= a2i(tagPumpTrs.sNozzle ,sizeof(tagPumpTrs.sNozzle ));
	m_nGrade		= a2i(tagPumpTrs.sGrade ,sizeof(tagPumpTrs.sGrade ));
	m_nLevel		= a2i(tagPumpTrs.sLevel,sizeof(tagPumpTrs.sLevel));

	m_lPump			= a2l(tagPumpTrs.sPump ,sizeof(tagPumpTrs.sPump ));
	m_lMode			= a2l(tagPumpTrs.sMode ,sizeof(tagPumpTrs.sMode ));
	m_lRes			= l2PumpReservation(a2l(tagPumpTrs.sRes ,sizeof(tagPumpTrs.sRes )));
	m_lResExt		= l2PumpResExt(a2l(tagPumpTrs.sResExt ,sizeof(tagPumpTrs.sResExt)));

	m_lLinkNumber	= a2l(tagPumpTrs.sLinkNumber ,sizeof(tagPumpTrs.sLinkNumber ));
	m_nLinkFlags	= a2i(tagPumpTrs.sLinkFlags ,sizeof(tagPumpTrs.sLinkFlags ));
	m_lActiveReadNumber		= a2l(tagPumpTrs.sActiveReadNumber,sizeof(tagPumpTrs.sActiveReadNumber )) ;
	m_lActiveShiftNumber	= a2l(tagPumpTrs.sActiveShiftNumber,sizeof(tagPumpTrs.sActiveShiftNumber )) ;

	m_lMisc			= a2l(tagPumpTrs.sMisc,sizeof(tagPumpTrs.sMisc )) ;
	m_lControllerSeqNum = 0L;	//4.0.18.660 68689	//4.0.21.502

	memset(m_sAttendantID, ' ', sizeof(m_sAttendantID));								//4.0.22.10 TD 71055	4.0.22.501
	memset(m_sAttendantName, ' ', sizeof(m_sAttendantName));							//4.0.22.10 TD 71055 4.0.22.501
	memset(m_sFullVehicleId, ' ', sizeof(m_sFullVehicleId));							//4.0.23.500 TD 69914
	memset(m_sAttendantTag, ' ', sizeof(m_sAttendantTag));								//4.0.25.501 TD 137509

	m_sPumpTest = '0';   //4.0.27.60	TD 231472 
	m_lTankID = 0L;		 //4.0.27.60  	TD 231475

	memset(m_sFiller, ' ', sizeof(m_sFiller));//4.0.22.511

	m_lMaxValue = 0;								 // 4.0.25.600 CR 296433
	m_lMaxVolume = 0;								 // 4.0.25.600 CR 296433			
	memset(m_sTenderType,' ',sizeof(m_sTenderType)); // 4.0.25.600 CR 296433

	m_lMisc2  = 0L;		 //CR 351687: Enhanced post void transactions
	m_sTypeOfMeasure = '0'; // TD 363041

	m_sLoyaltyDiscountApplied = ' ';
}

//4.0.14.500
void CPumpTransact::Convert2PumpTrsStruct(TAG_PUMP_TRANSACT2* cTagPumpTrs)
{

	memset(cTagPumpTrs,' ',sizeof(TAG_PUMP_TRANSACT2));

	l2a(m_lNumber,cTagPumpTrs->sNumber,sizeof(cTagPumpTrs->sNumber));
	l2a(m_lStatus,cTagPumpTrs->sStatus,sizeof(cTagPumpTrs->sStatus));
	l2a(m_lOnOffer,cTagPumpTrs->sOnOffer,sizeof(cTagPumpTrs->sOnOffer));
	l2a(m_lRawVolume,cTagPumpTrs->sRawVolume,sizeof(cTagPumpTrs->sRawVolume));
	l2a(m_lRoundedVolume,cTagPumpTrs->sRoundedVolume,sizeof(cTagPumpTrs->sRoundedVolume));
	l2a(m_lRoundedValue,cTagPumpTrs->sRoundedValue,sizeof(cTagPumpTrs->sRoundedValue));
	l2a(m_lGradePrice,cTagPumpTrs->sGradePrice,sizeof(cTagPumpTrs->sGradePrice));

	i2a(m_nNozzle,cTagPumpTrs->sNozzle,sizeof(cTagPumpTrs->sNozzle));
	i2a(m_nGrade,cTagPumpTrs->sGrade,sizeof(cTagPumpTrs->sGrade));
	//i2a(m_nLevel,cTagPumpTrs->sNozzle,sizeof(cTagPumpTrs->sLevel));


	l2a(m_nNozzle,cTagPumpTrs->sNozzle,sizeof(cTagPumpTrs->sNozzle));
	l2a(m_nGrade,cTagPumpTrs->sGrade,sizeof(cTagPumpTrs->sGrade));
	l2a(m_nLevel,cTagPumpTrs->sLevel,sizeof(cTagPumpTrs->sLevel));
	l2a(m_lPump,cTagPumpTrs->sPump,sizeof(cTagPumpTrs->sPump));
	l2a(m_lMode,cTagPumpTrs->sMode,sizeof(cTagPumpTrs->sMode));
	l2a(m_lRes,cTagPumpTrs->sRes,sizeof(cTagPumpTrs->sRes));
	l2a(m_lResExt,cTagPumpTrs->sResExt,sizeof(cTagPumpTrs->sResExt));
	l2a(m_lLinkNumber,cTagPumpTrs->sLinkNumber,sizeof(cTagPumpTrs->sLinkNumber));

	i2a(m_nLinkFlags,cTagPumpTrs->sLinkFlags,sizeof(cTagPumpTrs->sLinkFlags));

	l2a(m_lActiveReadNumber,cTagPumpTrs->sActiveReadNumber,sizeof(cTagPumpTrs->sActiveReadNumber));
	l2a(m_lActiveShiftNumber,cTagPumpTrs->sActiveShiftNumber,sizeof(cTagPumpTrs->sActiveShiftNumber));
	l2a(m_lMisc,cTagPumpTrs->sMisc,sizeof(cTagPumpTrs->sMisc));
	
	l2a(m_cTime.GetDay(),cTagPumpTrs->sDateTime.sDay,sizeof(cTagPumpTrs->sDateTime.sDay));
	l2a(m_cTime.GetHour(),cTagPumpTrs->sDateTime.sHour,sizeof(cTagPumpTrs->sDateTime.sHour));
	l2a(m_cTime.GetMinute(),cTagPumpTrs->sDateTime.sMinute,sizeof(cTagPumpTrs->sDateTime.sMinute));
	l2a(m_cTime.GetMonth(),cTagPumpTrs->sDateTime.sMonth,sizeof(cTagPumpTrs->sDateTime.sMonth));
	l2a(m_cTime.GetSecond(),cTagPumpTrs->sDateTime.sSecond,sizeof(cTagPumpTrs->sDateTime.sSecond));
	l2a(m_cTime.GetYear(),cTagPumpTrs->sDateTime.sYear,sizeof(cTagPumpTrs->sDateTime.sYear));

	m_sLoyaltyDiscountApplied = ' ';
}

//TD 38654
void CPumpTransact::Convert2PumpTrsStruct(TAG_PUMP_TRANSACT5* cTagPumpTrs)
{

	memset(cTagPumpTrs,' ',sizeof(TAG_PUMP_TRANSACT5));

	l2a(m_lNumber,cTagPumpTrs->sNumber,sizeof(cTagPumpTrs->sNumber));
	l2a(m_lStatus,cTagPumpTrs->sStatus,sizeof(cTagPumpTrs->sStatus));
	l2a(m_lOnOffer,cTagPumpTrs->sOnOffer,sizeof(cTagPumpTrs->sOnOffer));
	l2a(m_lRawVolume,cTagPumpTrs->sRawVolume,sizeof(cTagPumpTrs->sRawVolume));
	l2a(m_lRoundedVolume,cTagPumpTrs->sRoundedVolume,sizeof(cTagPumpTrs->sRoundedVolume));
	l2a(m_lRoundedValue,cTagPumpTrs->sRoundedValue,sizeof(cTagPumpTrs->sRoundedValue));
	l2a(m_lGradePrice,cTagPumpTrs->sGradePrice,sizeof(cTagPumpTrs->sGradePrice));
	//RFUEL-3309 Get the maximal discount allowed for the grade 
	long lMaxAllowedDiscount = _Module.m_server.GetMaximalDiscountForGrade(m_nGrade);
	l2a(lMaxAllowedDiscount, cTagPumpTrs->sMaxDiscountAllowed, sizeof(cTagPumpTrs->sMaxDiscountAllowed));
	i2a(m_nNozzle,cTagPumpTrs->sNozzle,sizeof(cTagPumpTrs->sNozzle));
	i2a(m_nGrade,cTagPumpTrs->sGrade,sizeof(cTagPumpTrs->sGrade));
	//i2a(m_nLevel,cTagPumpTrs->sNozzle,sizeof(cTagPumpTrs->sLevel));


	l2a(m_nNozzle,cTagPumpTrs->sNozzle,sizeof(cTagPumpTrs->sNozzle));
	l2a(m_nGrade,cTagPumpTrs->sGrade,sizeof(cTagPumpTrs->sGrade));
	l2a(m_nLevel,cTagPumpTrs->sLevel,sizeof(cTagPumpTrs->sLevel));
	l2a(m_lPump,cTagPumpTrs->sPump,sizeof(cTagPumpTrs->sPump));
	l2a(m_lMode,cTagPumpTrs->sMode,sizeof(cTagPumpTrs->sMode));
	l2a(m_lRes,cTagPumpTrs->sRes,sizeof(cTagPumpTrs->sRes));
	l2a(m_lResExt,cTagPumpTrs->sResExt,sizeof(cTagPumpTrs->sResExt));
	l2a(m_lLinkNumber,cTagPumpTrs->sLinkNumber,sizeof(cTagPumpTrs->sLinkNumber));

	i2a(m_nLinkFlags,cTagPumpTrs->sLinkFlags,sizeof(cTagPumpTrs->sLinkFlags));

	l2a(m_lActiveReadNumber,cTagPumpTrs->sActiveReadNumber,sizeof(cTagPumpTrs->sActiveReadNumber));
	l2a(m_lActiveShiftNumber,cTagPumpTrs->sActiveShiftNumber,sizeof(cTagPumpTrs->sActiveShiftNumber));
	l2a(m_lMisc,cTagPumpTrs->sMisc,sizeof(cTagPumpTrs->sMisc));
	
	l2a(m_cTime.GetDay(),cTagPumpTrs->sDateTime.sDay,sizeof(cTagPumpTrs->sDateTime.sDay));
	l2a(m_cTime.GetHour(),cTagPumpTrs->sDateTime.sHour,sizeof(cTagPumpTrs->sDateTime.sHour));
	l2a(m_cTime.GetMinute(),cTagPumpTrs->sDateTime.sMinute,sizeof(cTagPumpTrs->sDateTime.sMinute));
	l2a(m_cTime.GetMonth(),cTagPumpTrs->sDateTime.sMonth,sizeof(cTagPumpTrs->sDateTime.sMonth));
	l2a(m_cTime.GetSecond(),cTagPumpTrs->sDateTime.sSecond,sizeof(cTagPumpTrs->sDateTime.sSecond));
	l2a(m_cTime.GetYear(),cTagPumpTrs->sDateTime.sYear,sizeof(cTagPumpTrs->sDateTime.sYear));

	l2a(m_cStartTime.GetDay(),cTagPumpTrs->sStartDateTime.sDay,sizeof(cTagPumpTrs->sStartDateTime.sDay));
	l2a(m_cStartTime.GetHour(),cTagPumpTrs->sStartDateTime.sHour,sizeof(cTagPumpTrs->sStartDateTime.sHour));
	l2a(m_cStartTime.GetMinute(),cTagPumpTrs->sStartDateTime.sMinute,sizeof(cTagPumpTrs->sStartDateTime.sMinute));
	l2a(m_cStartTime.GetMonth(),cTagPumpTrs->sStartDateTime.sMonth,sizeof(cTagPumpTrs->sStartDateTime.sMonth));
	l2a(m_cStartTime.GetSecond(),cTagPumpTrs->sStartDateTime.sSecond,sizeof(cTagPumpTrs->sStartDateTime.sSecond));
	l2a(m_cStartTime.GetYear(),cTagPumpTrs->sStartDateTime.sYear,sizeof(cTagPumpTrs->sStartDateTime.sYear));
	l2a(m_lControllerSeqNum, cTagPumpTrs->sControllerSeqNum, sizeof(cTagPumpTrs->sControllerSeqNum));	//4.0.18.660 68689 //4.0.21.502

//	memcpy(m_sAttendantID, cTagPumpTrs->sAttendantID, sizeof(cTagPumpTrs->sAttendantID)); //4.0.22.10 TD 71055
	memcpy(cTagPumpTrs->sAttendantID, m_sAttendantID, sizeof(cTagPumpTrs->sAttendantID)); //4.0.22.10 TD 71055 //4.0.22.501

	//4.0.22.511 72606
	l2a(m_cStartFuelingTime.GetDay(),cTagPumpTrs->sStartFuelingDateTime.sDay,sizeof(cTagPumpTrs->sStartFuelingDateTime.sDay));
	l2a(m_cStartFuelingTime.GetHour(),cTagPumpTrs->sStartFuelingDateTime.sHour,sizeof(cTagPumpTrs->sStartFuelingDateTime.sHour));
	l2a(m_cStartFuelingTime.GetMinute(),cTagPumpTrs->sStartFuelingDateTime.sMinute,sizeof(cTagPumpTrs->sStartFuelingDateTime.sMinute));
	l2a(m_cStartFuelingTime.GetMonth(),cTagPumpTrs->sStartFuelingDateTime.sMonth,sizeof(cTagPumpTrs->sStartFuelingDateTime.sMonth));
	l2a(m_cStartFuelingTime.GetSecond(),cTagPumpTrs->sStartFuelingDateTime.sSecond,sizeof(cTagPumpTrs->sStartFuelingDateTime.sSecond));
	l2a(m_cStartFuelingTime.GetYear(),cTagPumpTrs->sStartFuelingDateTime.sYear,sizeof(cTagPumpTrs->sStartFuelingDateTime.sYear));

	l2a(m_lLowestGradePrice, cTagPumpTrs->sLowestGradePrice, sizeof(cTagPumpTrs->sLowestGradePrice));
	memcpy(&cTagPumpTrs->sLoyaltyPriceProtection, &m_sLoyaltyPriceProtection, sizeof(&cTagPumpTrs->sLoyaltyPriceProtection));
	memcpy(&cTagPumpTrs->sLoyaltyAutoArm, &m_sLoyaltyAutoArm, sizeof(&cTagPumpTrs->sLoyaltyAutoArm));
}


//4.0.14.500
void CPumpTransact::ConvertFromPumpTrsStruct(TAG_PUMP_TRANSACT2* cTagPumpTrs)
{

	m_lNumber = a2l(cTagPumpTrs->sNumber,sizeof(cTagPumpTrs->sNumber));
	m_lStatus = l2TrsStatus(a2l(cTagPumpTrs->sStatus,sizeof(cTagPumpTrs->sStatus)));
	m_lOnOffer = a2l(cTagPumpTrs->sOnOffer,sizeof(cTagPumpTrs->sOnOffer));
	m_lRawVolume = a2l(cTagPumpTrs->sRawVolume,sizeof(cTagPumpTrs->sRawVolume));
	m_lRoundedVolume = a2l(cTagPumpTrs->sRoundedVolume,sizeof(cTagPumpTrs->sRoundedVolume));
	m_lRoundedValue = a2l(cTagPumpTrs->sRoundedValue,sizeof(cTagPumpTrs->sRoundedValue));
	m_lGradePrice = a2l(cTagPumpTrs->sGradePrice,sizeof(cTagPumpTrs->sGradePrice));

	m_nNozzle = a2i(cTagPumpTrs->sNozzle,sizeof(cTagPumpTrs->sNozzle));
	m_nGrade = a2i(cTagPumpTrs->sGrade,sizeof(cTagPumpTrs->sGrade));
	m_nLevel = a2i(cTagPumpTrs->sLevel,sizeof(cTagPumpTrs->sLevel));

	m_lPump = a2l(cTagPumpTrs->sPump,sizeof(cTagPumpTrs->sPump));
	m_lMode = a2l(cTagPumpTrs->sMode,sizeof(cTagPumpTrs->sMode));
	m_lRes = l2PumpReservation(a2l(cTagPumpTrs->sRes,sizeof(cTagPumpTrs->sRes)));
	m_lResExt = l2PumpResExt(a2l(cTagPumpTrs->sResExt,sizeof(cTagPumpTrs->sResExt)));
	m_lLinkNumber = a2l(cTagPumpTrs->sLinkNumber,sizeof(cTagPumpTrs->sLinkNumber));

	m_nLinkFlags = a2i(cTagPumpTrs->sLinkFlags,sizeof(cTagPumpTrs->sLinkFlags));

	m_lActiveReadNumber = a2l(cTagPumpTrs->sActiveReadNumber,sizeof(cTagPumpTrs->sActiveReadNumber));
	m_lActiveShiftNumber = a2l(cTagPumpTrs->sActiveShiftNumber,sizeof(cTagPumpTrs->sActiveShiftNumber));
	m_lMisc = a2l(cTagPumpTrs->sMisc,sizeof(cTagPumpTrs->sMisc));

	if (!ChrAll((char*)&(cTagPumpTrs->sDateTime),sizeof(cTagPumpTrs->sDateTime)))
	{
		CTime cTmpTime(
			a2l(cTagPumpTrs->sDateTime.sYear,sizeof(cTagPumpTrs->sDateTime.sYear)),
			a2l(cTagPumpTrs->sDateTime.sMonth,sizeof(cTagPumpTrs->sDateTime.sMonth)), 
			a2l(cTagPumpTrs->sDateTime.sDay,sizeof(cTagPumpTrs->sDateTime.sDay)),
			a2l(cTagPumpTrs->sDateTime.sHour,sizeof(cTagPumpTrs->sDateTime.sHour)),
			a2l(cTagPumpTrs->sDateTime.sMinute,sizeof(cTagPumpTrs->sDateTime.sMinute)),
			a2l(cTagPumpTrs->sDateTime.sSecond,sizeof(cTagPumpTrs->sDateTime.sSecond)));

		m_cTime = cTmpTime;
	}


}

//TD 38654
void CPumpTransact::ConvertFromPumpTrsStruct(TAG_PUMP_TRANSACT5* cTagPumpTrs)
{

	m_lNumber = a2l(cTagPumpTrs->sNumber,sizeof(cTagPumpTrs->sNumber));
	m_lStatus = l2TrsStatus(a2l(cTagPumpTrs->sStatus,sizeof(cTagPumpTrs->sStatus)));
	m_lOnOffer = a2l(cTagPumpTrs->sOnOffer,sizeof(cTagPumpTrs->sOnOffer));
	m_lRawVolume = a2l(cTagPumpTrs->sRawVolume,sizeof(cTagPumpTrs->sRawVolume));
	m_lRoundedVolume = a2l(cTagPumpTrs->sRoundedVolume,sizeof(cTagPumpTrs->sRoundedVolume));
	m_lRoundedValue = a2l(cTagPumpTrs->sRoundedValue,sizeof(cTagPumpTrs->sRoundedValue));
	m_lGradePrice = a2l(cTagPumpTrs->sGradePrice,sizeof(cTagPumpTrs->sGradePrice));

	m_nNozzle = a2i(cTagPumpTrs->sNozzle,sizeof(cTagPumpTrs->sNozzle));
	m_nGrade = a2i(cTagPumpTrs->sGrade,sizeof(cTagPumpTrs->sGrade));
	m_nLevel = a2i(cTagPumpTrs->sLevel,sizeof(cTagPumpTrs->sLevel));

	m_lPump = a2l(cTagPumpTrs->sPump,sizeof(cTagPumpTrs->sPump));
	m_lMode = a2l(cTagPumpTrs->sMode,sizeof(cTagPumpTrs->sMode));
	m_lRes = l2PumpReservation(a2l(cTagPumpTrs->sRes,sizeof(cTagPumpTrs->sRes)));
	m_lResExt = l2PumpResExt(a2l(cTagPumpTrs->sResExt,sizeof(cTagPumpTrs->sResExt)));
	m_lLinkNumber = a2l(cTagPumpTrs->sLinkNumber,sizeof(cTagPumpTrs->sLinkNumber));

	m_nLinkFlags = a2i(cTagPumpTrs->sLinkFlags,sizeof(cTagPumpTrs->sLinkFlags));

	m_lActiveReadNumber = a2l(cTagPumpTrs->sActiveReadNumber,sizeof(cTagPumpTrs->sActiveReadNumber));
	m_lActiveShiftNumber = a2l(cTagPumpTrs->sActiveShiftNumber,sizeof(cTagPumpTrs->sActiveShiftNumber));
	m_lMisc = a2l(cTagPumpTrs->sMisc,sizeof(cTagPumpTrs->sMisc));
	//RFUEL-3309 Get the maximal discount allowed for the grade 
	memcpy(cTagPumpTrs->sMaxDiscountAllowed, m_sMaxDiscountAllowed, sizeof(m_sMaxDiscountAllowed));

	if (!ChrAll((char*)&(cTagPumpTrs->sDateTime),sizeof(cTagPumpTrs->sDateTime)))
	{
		CTime cTmpTime(
			a2l(cTagPumpTrs->sDateTime.sYear,sizeof(cTagPumpTrs->sDateTime.sYear)),
			a2l(cTagPumpTrs->sDateTime.sMonth,sizeof(cTagPumpTrs->sDateTime.sMonth)), 
			a2l(cTagPumpTrs->sDateTime.sDay,sizeof(cTagPumpTrs->sDateTime.sDay)),
			a2l(cTagPumpTrs->sDateTime.sHour,sizeof(cTagPumpTrs->sDateTime.sHour)),
			a2l(cTagPumpTrs->sDateTime.sMinute,sizeof(cTagPumpTrs->sDateTime.sMinute)),
			a2l(cTagPumpTrs->sDateTime.sSecond,sizeof(cTagPumpTrs->sDateTime.sSecond)));

		m_cTime = cTmpTime;
	}
	if (!ChrAll((char*)&(cTagPumpTrs->sStartDateTime),sizeof(cTagPumpTrs->sStartDateTime)))
	{
		CTime cTmpTime2(
			a2l(cTagPumpTrs->sStartDateTime.sYear,sizeof(cTagPumpTrs->sStartDateTime.sYear)),
			a2l(cTagPumpTrs->sStartDateTime.sMonth,sizeof(cTagPumpTrs->sStartDateTime.sMonth)), 
			a2l(cTagPumpTrs->sStartDateTime.sDay,sizeof(cTagPumpTrs->sStartDateTime.sDay)),
			a2l(cTagPumpTrs->sStartDateTime.sHour,sizeof(cTagPumpTrs->sStartDateTime.sHour)),
			a2l(cTagPumpTrs->sStartDateTime.sMinute,sizeof(cTagPumpTrs->sStartDateTime.sMinute)),
			a2l(cTagPumpTrs->sStartDateTime.sSecond,sizeof(cTagPumpTrs->sStartDateTime.sSecond)));

		m_cStartTime = cTmpTime2;
	}
	m_lControllerSeqNum = a2l(cTagPumpTrs->sControllerSeqNum, sizeof(cTagPumpTrs->sControllerSeqNum)); //4.0.18.660 68689 //4.0.21.502

//	memcpy(cTagPumpTrs->sAttendantID, m_sAttendantID, sizeof(m_sAttendantID)); //4.0.22.10 TD 71055
	memcpy(m_sAttendantID, cTagPumpTrs->sAttendantID, sizeof(m_sAttendantID)); //4.0.22.10 TD 71055	//4.0.22.501
	//4.0.22.511 72606
	if (!ChrAll((char*)&(cTagPumpTrs->sStartFuelingDateTime),sizeof(cTagPumpTrs->sStartFuelingDateTime)))
	{
		CTime cTmpTime3(
			a2l(cTagPumpTrs->sStartFuelingDateTime.sYear,sizeof(cTagPumpTrs->sStartFuelingDateTime.sYear)),
			a2l(cTagPumpTrs->sStartFuelingDateTime.sMonth,sizeof(cTagPumpTrs->sStartFuelingDateTime.sMonth)), 
			a2l(cTagPumpTrs->sStartFuelingDateTime.sDay,sizeof(cTagPumpTrs->sStartFuelingDateTime.sDay)),
			a2l(cTagPumpTrs->sStartFuelingDateTime.sHour,sizeof(cTagPumpTrs->sStartFuelingDateTime.sHour)),
			a2l(cTagPumpTrs->sStartFuelingDateTime.sMinute,sizeof(cTagPumpTrs->sStartFuelingDateTime.sMinute)),
			a2l(cTagPumpTrs->sStartFuelingDateTime.sSecond,sizeof(cTagPumpTrs->sStartFuelingDateTime.sSecond)));

		m_cStartFuelingTime = cTmpTime3;
	}
	m_lLowestGradePrice = a2l(cTagPumpTrs->sLowestGradePrice, sizeof(cTagPumpTrs->sLowestGradePrice));
	memcpy(&m_sLoyaltyPriceProtection, &cTagPumpTrs->sLoyaltyPriceProtection, sizeof(cTagPumpTrs->sLoyaltyPriceProtection));
	memcpy(&m_sLoyaltyAutoArm, &cTagPumpTrs->sLoyaltyAutoArm, sizeof(cTagPumpTrs->sLoyaltyAutoArm));
}

void CPumpTransact::Clear()
{	
	m_lStatus			= NULL_TRS;
	m_lNumber			= 0L;
	m_lOnOffer			= 0L;
	m_lRawVolume        = 0L;
	m_lRoundedVolume	= 0L;
	m_lRoundedValue		= 0L;
	m_lGradePrice		= 0L;
	m_nNozzle			= 0;
	m_nGrade			= 0;
	m_nLevel			= 0;
	m_lMode				= DEFAULT_MODE;
	m_lRes				= NULL_RES;
	m_lResExt			= NULL_RES_EXT;	
	m_nLinkFlags		= TRS_NO_LINK;
	m_lLinkNumber		 = 0L;
	m_lActiveShiftNumber = 0L;	
	m_lMisc				 = 0L;

	m_cTime				= CTime::GetCurrentTime();
	m_cStartTime		= m_cTime;	//TD 40492, 38654
	m_cStartFuelingTime	= m_cTime;	//4.0.22.511 72606
	m_lControllerSeqNum	= 0;//4.0.18.660 68689 //4.0.21.502
	m_sReportPaid		= ' '; //4.0.23.650 133966 //4.0.25.50

	
	memset(m_sAttendantID, ' ', sizeof(m_sAttendantID));								//4.0.22.10 TD 71055
	memset(m_sAttendantName, ' ', sizeof(m_sAttendantName));							//4.0.22.10 TD 71055
	memset(m_sFullVehicleId, ' ', sizeof(m_sFullVehicleId));							//4.0.23.500 TD 69914
	memset(m_sAttendantTag, ' ', sizeof(m_sAttendantTag));								//4.0.22.501 TD 137509
	memset(m_sFiller, ' ', sizeof(m_sFiller));//4.0.22.511
	
	m_sPumpTest			= '0';	//4.0.27.60 TD 231472
	m_lTankID = 0L;	            //4.0.27.60 TD 231475	

	m_nAutoReconcileTimeout = 0; //  TD 233242 4.0.23.1050
	m_lLowestGradePrice = 0L;
	m_sLoyaltyPriceProtection = '0';
	m_sLoyaltyAutoArm = '0';

	m_lMaxValue = 0;								 // 4.0.25.600 CR 296433
	m_lMaxVolume = 0;								 // 4.0.25.600 CR 296433			
	memset(m_sTenderType,' ',sizeof(m_sTenderType)); // 4.0.25.600 CR 296433

	m_lMisc2  = 0L;				//CR 351687: Enhanced post void transactions
	m_sTypeOfMeasure = '0'; // TD 363041
	memset(m_sMaxDiscountAllowed, ' ', sizeof(m_sMaxDiscountAllowed));  //RFUEL-3309 Get the maximal discount allowed for the grade 

	// RFUEL-2817
	m_sLoyaltyDiscountApplied = ' ';

	// Removed, generating too many log entries
	//{		
	//	LogStream LogMsg;
	//	LogMsg << __FUNCTION__ << " - Clearing Pump " << m_lPump << ends;
	//	_LOGMSG.LogMsg(LogMsg);
	//}
}


void CPumpTransact::GetTag(void *pBuff, long lVersion, long lPreSet , long lGrade, long lGrade2 ,__int64 lPurchaseID, long lPreSetType) //4.0.9.507 //4.0.20.54 //4.0.22.503	72394
{
	TAG_PUMP_TRANSACT *pTagPumpTrs;
	pTagPumpTrs = (TAG_PUMP_TRANSACT *)pBuff;

	l2a(m_lNumber,pTagPumpTrs->sNumber		,sizeof(pTagPumpTrs->sNumber		));//= m_lNumber;
	l2a(m_lStatus,pTagPumpTrs->sStatus		,sizeof(pTagPumpTrs->sStatus		));//= m_lStatus;
	l2a(m_lOnOffer,pTagPumpTrs->sOnOffer		,sizeof(pTagPumpTrs->sOnOffer		));//= m_lOnOffer;
	l2a(m_lRawVolume,pTagPumpTrs->sRawVolume		,sizeof(pTagPumpTrs->sRawVolume		));//= m_lRawVolume;
	l2a(m_lRoundedVolume,pTagPumpTrs->sRoundedVolume ,sizeof(pTagPumpTrs->sRoundedVolume ));//= m_lRoundedVolume;
	l2a(m_lRoundedValue,pTagPumpTrs->sRoundedValue	,sizeof(pTagPumpTrs->sRoundedValue	));//= m_lRoundedValue;
	l2a(m_lGradePrice,pTagPumpTrs->sGradePrice	,sizeof(pTagPumpTrs->sGradePrice	));//= m_lGradePrice;

	i2a(m_nNozzle,pTagPumpTrs->sNozzle		,sizeof(pTagPumpTrs->sNozzle		));//= m_nNozzle;
	i2a(m_nGrade,pTagPumpTrs->sGrade			,sizeof(pTagPumpTrs->sGrade			));//= m_nGrade;
	i2a(m_nLevel,pTagPumpTrs->sLevel	,sizeof(pTagPumpTrs->sLevel	));//= m_nLevel;
	l2a(m_lPump,pTagPumpTrs->sPump			,sizeof(pTagPumpTrs->sPump			));//= m_lPump;
	l2a(m_lMode,pTagPumpTrs->sMode			,sizeof(pTagPumpTrs->sMode			));//= m_lMode;
	l2a(m_lRes,pTagPumpTrs->sRes			,sizeof(pTagPumpTrs->sRes			));//= m_lRes;
	l2a(m_lResExt,pTagPumpTrs->sResExt		,sizeof(pTagPumpTrs->sResExt		));//= m_lResExt;

	l2a(m_lLinkNumber,pTagPumpTrs->sLinkNumber		,sizeof(pTagPumpTrs->sLinkNumber));//= m_lLinkNumber;
	i2a(m_nLinkFlags,pTagPumpTrs->sLinkFlags		,sizeof(pTagPumpTrs->sLinkFlags		));//= m_nLinkFlags;

	l2a(m_lActiveReadNumber,pTagPumpTrs->sActiveReadNumber,sizeof(pTagPumpTrs->sActiveReadNumber ));
	l2a(m_lActiveShiftNumber,pTagPumpTrs->sActiveShiftNumber,sizeof(pTagPumpTrs->sActiveShiftNumber ));
	l2a(m_lMisc,pTagPumpTrs->sMisc		,sizeof(pTagPumpTrs->sMisc));//= m_lMisc

	if(lVersion == 2)
	{
		TAG_PUMP_TRANSACT5 *pTagPumpTrs5;
		pTagPumpTrs5 = (TAG_PUMP_TRANSACT5 *)pBuff;
		unsigned char sTmpMisc[11];
		SYSTEMTIME sysTime, sysTime2, sysTime3;	//4.0.22.511 72606
		long lServiceMode;

		if ((m_lPump > 0) && (m_lPump <= _Module.m_server.GetLastValidPump())) //4.0.14.503
			lServiceMode = _Module.m_server.m_cPumpArray[m_lPump-1].GetStat().GetServiceMode();		
		else
			lServiceMode = 0;

		memset(sTmpMisc , 0, sizeof(sTmpMisc));
		m_cTime.GetAsSystemTime(sysTime);
		m_cStartTime.GetAsSystemTime(sysTime2);
		m_cStartFuelingTime.GetAsSystemTime(sysTime3);	//4.0.22.511 72606

		SystemTime2Tag(&sysTime,&(pTagPumpTrs5->sDateTime));
		SystemTime2Tag(&sysTime2,&(pTagPumpTrs5->sStartDateTime));
		SystemTime2Tag(&sysTime3,&(pTagPumpTrs5->sStartFuelingDateTime));	//4.0.22.511 72606

		l2a(lPreSet,pTagPumpTrs5->sPrePaidValue	,sizeof(pTagPumpTrs5->sPrePaidValue	));
		l2a(lPreSetType, &pTagPumpTrs5->sPreSetType, sizeof(pTagPumpTrs5->sPreSetType));  //4.0.22.503	72394
		if (!m_nGrade)
		{
			i2a((short)lGrade,pTagPumpTrs->sGrade ,sizeof(pTagPumpTrs->sGrade));
		}

		l2a(m_lMisc ,sTmpMisc , 10);
		// pass misc with 10 digit instead 5
		memcpy(pTagPumpTrs5->sMisc , &sTmpMisc[5], 5);
		memcpy(pTagPumpTrs5->sMiscH ,sTmpMisc, 5);			
		l2a(lServiceMode, pTagPumpTrs5->sServiceMode , sizeof(pTagPumpTrs5->sServiceMode));
		l2a(lGrade2, pTagPumpTrs5->sGrade2 , sizeof(pTagPumpTrs5->sGrade2)); //4.0.9.507
		
		l2a(m_lControllerSeqNum, pTagPumpTrs5->sControllerSeqNum, sizeof(pTagPumpTrs5->sControllerSeqNum)); //4.0.18.660 68689 //4.0.21.502

		//memcpy(&pTagPumpTrs5->sLoyaltyPriceProtection, &m_sLoyaltyPriceProtection, sizeof(pTagPumpTrs5->sLoyaltyPriceProtection));
		memset(pTagPumpTrs5->sFormatedLine, ' ', sizeof(pTagPumpTrs5->sFormatedLine));	//4.0.22.501
		//_Module.m_server.m_cBuildLine2PosPump.GetFormatedLine(this , (char *)pTagPumpTrs5->sFormatedLine, sizeof(pTagPumpTrs5->sFormatedLine)); //4.0.22.10 TD 71055
		memcpy(pTagPumpTrs5->sAttendantID ,m_sAttendantID, sizeof(m_sAttendantID));		//4.0.22.10 TD 71055
		memcpy(pTagPumpTrs5->sAttendantTag, m_sAttendantTag, sizeof(m_sAttendantTag));	//4.0.25.501 TD 137509

		// Init all fields which are not initialized
		memset(pTagPumpTrs5->sCarWashProgram, '0', sizeof(pTagPumpTrs5->sCarWashProgram));
		memset(pTagPumpTrs5->sWashCode, '0', sizeof(pTagPumpTrs5->sWashCode));
		memset(pTagPumpTrs5->sWashExpire, '0', sizeof(pTagPumpTrs5->sWashExpire));
		memset(pTagPumpTrs5->sCarWashTax, '0', sizeof(pTagPumpTrs5->sCarWashTax));
		memset(pTagPumpTrs5->sCarWashAmount, '0', sizeof(pTagPumpTrs5->sCarWashAmount));
		memset(pTagPumpTrs5->sCarWashUnitPrice, '0', sizeof(pTagPumpTrs5->sCarWashUnitPrice));
		memset(pTagPumpTrs5->sCarWashQty, '0', sizeof(pTagPumpTrs5->sCarWashQty));
		memset(pTagPumpTrs5->sCarWashDiscount, '0', sizeof(pTagPumpTrs5->sCarWashDiscount));
		memset(pTagPumpTrs5->sFullVehicleID, '0', sizeof(pTagPumpTrs5->sFullVehicleID));		

		pTagPumpTrs5->sPumpTest = m_sPumpTest;	                              //4.0.27.60   TD 231472
		l2a(m_lTankID,pTagPumpTrs5->sTankID, sizeof(pTagPumpTrs5->sTankID));  //4.0.25.60   TD 231475

				// cinook
		BYTE sTmpPurchaseID[STR_LONG_VAL10+STR_LONG_VAL2+1] = {0};
		i642a(lPurchaseID, (BYTE *)sTmpPurchaseID , sizeof(sTmpPurchaseID)); //4.0.19.180	
		memcpy(pTagPumpTrs5->sPurchaseIdMSB , sTmpPurchaseID ,sizeof(pTagPumpTrs5->sPurchaseIdMSB));
		memcpy(pTagPumpTrs5->sPurchaseID , sTmpPurchaseID + sizeof(pTagPumpTrs5->sPurchaseIdMSB) ,sizeof(pTagPumpTrs5->sPurchaseID));

		l2a(m_lMisc2,pTagPumpTrs5->sMisc2,sizeof(pTagPumpTrs5->sMisc2));		//CR 351687: Enhanced post void transactions

		pTagPumpTrs5->sTypeOfMeasure = m_sTypeOfMeasure; // TD 363041

		l2a(m_lLowestGradePrice, pTagPumpTrs5->sLowestGradePrice, sizeof(pTagPumpTrs5->sLowestGradePrice));
		memcpy(&pTagPumpTrs5->sLoyaltyPriceProtection, &m_sLoyaltyPriceProtection, sizeof(pTagPumpTrs5->sLoyaltyPriceProtection));
		memcpy(&pTagPumpTrs5->sLoyaltyAutoArm, &m_sLoyaltyAutoArm, sizeof(pTagPumpTrs5->sLoyaltyAutoArm));

		//RFUEL-3309 Get the maximal discount allowed for the grade 
		memcpy(&pTagPumpTrs5->sMaxDiscountAllowed, &m_sMaxDiscountAllowed, sizeof(m_sMaxDiscountAllowed));
	} 
}

LONG CPumpStatus::GetWarning()
{
	return m_lWarning;
}

LONG CPumpStatus::GetError()
{
	return m_lError;
}


// Global convert functions:
PumpStatus l2PumpStatus(long lVal)
{
	PumpStatus Status;
	switch (lVal)
	{
		case POWER_UP:					
			Status = POWER_UP;
			break;
		case READY:	
			Status = READY;
			break;
		case REQUESTING:
			Status = REQUESTING;
			break;
		case AUTHORIZED:
			Status = AUTHORIZED;
			break;
		case DISPENSING:
			Status = DISPENSING;
			break;
		case STOPED_BY_OPERATOR:
			Status = STOPED_BY_OPERATOR;
			break;
		case OUT_OF_SERVICE:
			Status = OUT_OF_SERVICE;
			break;
		case NOZZLE_LEFT_OUT:
			Status = NOZZLE_LEFT_OUT;
			break;
		case ERROR_TERMINATED:
			Status = ERROR_TERMINATED;
			break;
		case COMMS_TERMINATED:	
			Status = COMMS_TERMINATED;
			break;
		case STOPPED_BY_CONTROLLER:          //4.0.2.23
			Status = STOPPED_BY_CONTROLLER;
			break;
		default:
			Status = UNKNOWN_STATUS;
			break;

	}	
	return Status;	
}

TrsStatus l2TrsStatus(long lVal)
{
	TrsStatus Status;

	switch (lVal)
	{
		case NULL_TRS:					
			Status = NULL_TRS;
			break;

		case WAIT_RES:
			Status = WAIT_RES;
			break;

		case WAIT_AUTH:
			Status = WAIT_AUTH;
			break;

		case WAIT_PROGRESS:
			Status = WAIT_PROGRESS;
			break;

		case WAIT_TIME_OUT:
			Status = WAIT_TIME_OUT;
			break;

		case IN_PROGRESS:	
			Status = IN_PROGRESS;
			break;

		case UNPAID:
			Status = UNPAID;
			break;

		case ACCEPTED:
			Status = ACCEPTED;
			break;

		case PAID:
			Status = PAID;
			break;

		case INCOMPLETE:
			Status = INCOMPLETE;
			break;

		default:
			Status = UNKNOWN_TRS_STATUS;
			break;

	}	
	return Status;	
}

PumpResExt l2PumpResExt(long lVal)
{
	PumpResExt ResExt;
	switch(lVal)
	{
		case NULL_RES_EXT:			
			ResExt = NULL_RES_EXT;
			break;

		case RES_EXT_AVI:
			ResExt = RES_EXT_AVI;
			break;

		case RES_EXT_PRICE_CHANGE:
			ResExt = RES_EXT_PRICE_CHANGE;
			break;

		case RES_EXT_SHIFT_CHANGE:
			ResExt = RES_EXT_SHIFT_CHANGE;
			break;

		case RES_EXT_RFID_CT:
			ResExt = RES_EXT_RFID_CT;
			break;

		case RES_EXT_RFID_HT:
			ResExt = RES_EXT_RFID_HT;
			break;

		case RES_EXT_MAINTENANCE:
			ResExt = RES_EXT_MAINTENANCE;
			break;

		//4.0.5550.1504
		case RES_EXT_MAINTENANCE_DATABASE:
			ResExt = RES_EXT_MAINTENANCE_DATABASE;
			break;

		case RES_EXT_PAP_CANCELED:   // marik
			ResExt = RES_EXT_PAP_CANCELED;
			break;
		case RES_EXT_USER_MENU:      //3.2.1.50
			ResExt = RES_EXT_USER_MENU;
			break;
		case RES_EXT_ATTENDANT:       //3.2.0.76
			ResExt = RES_EXT_ATTENDANT;   //3.2.0.76 			
			break;			
		case RES_EXT_RFID_CT_CANCELED:   //4.0.1.40
			ResExt = RES_EXT_RFID_CT_CANCELED;   			
			break;
		case RES_EXT_RFID_HT_CANCELED:
			ResExt = RES_EXT_RFID_HT_CANCELED;   
			break;
		case RES_EXT_CASH_ACCEPTOR:
			ResExt = RES_EXT_CASH_ACCEPTOR;   
			break;
		case RES_EXT_AVI_CANCELED:       //3.2.1.74
			ResExt = RES_EXT_AVI_CANCELED;   //3.2.0.76 
			break;
		case RES_EXT_PUMP_SINGLE_LOCK:       //4.0.5.24
			ResExt = RES_EXT_PUMP_SINGLE_LOCK;  //4.0.5.24
			break;
		case RES_EXT_PAK_AUTO_RECONCILE:			//4.0.999.0
			ResExt = RES_EXT_PAK_AUTO_RECONCILE;
			break;
		case RES_EXT_FLEET: 	//4.0.999.0
			ResExt = RES_EXT_FLEET;
			break;
		case RES_EXT_LOYALTY_AVI: 	//4.0.11.503
			ResExt = RES_EXT_LOYALTY_AVI;
			break; 
		case RES_EXT_AUTO_AUTH://4.0.12.505
			ResExt = RES_EXT_AUTO_AUTH;
			break;
		case RES_EXT_PAP_EXTERNAL: //4.0.19.504 - 47559
			ResExt = RES_EXT_PAP_EXTERNAL;
			break;
		case RES_EXT_PAK_LOYALTY:	//4.0.19.506 Copient 55286
			ResExt = RES_EXT_PAK_LOYALTY;
			break;
		case RES_EXT_MOBILE:// CR 444830
			ResExt = RES_EXT_MOBILE;
			break;

		default:
			ResExt = UNKNOWN_RES_EXT;
			break;				
	}
	return ResExt;
}


PumpReservation l2PumpReservation(long lVal)
{

	PumpReservation Res;
	switch(lVal)
	{
		case NULL_RES:			
			Res = NULL_RES;
			break;
		case ATTEND:
			Res = ATTEND;
			break;
		case TEST:
			Res = TEST;
			break;
		case PAY_AT_PUMP:
			Res = PAY_AT_PUMP;
			break;
		case PAY_AT_KIOSK:
			Res = PAY_AT_KIOSK;
			break;
		case PAY_AT_OPT:
			Res = PAY_AT_OPT;
			break;			
		case PRE_PAY_AT_POS:
			Res = PRE_PAY_AT_POS;
			break;
		case PUMPSRV:
			Res = PUMPSRV;
			break;
		default:
			Res = UNKNOWN_RES;
			break;				
	}
	return Res;

}

PumpDevice l2PumpDevice(long lVal)
{

	PumpDevice Device;

	switch(lVal)
	{
		case NO_DEVICE:
			Device = NO_DEVICE;
			break;
		case TERMINAL:
			Device = TERMINAL;
			break;
		case TERMINAL_CARD_READER:
			Device = TERMINAL_CARD_READER;
			break;
		case TERMINAL_PRINTER:
			Device = TERMINAL_PRINTER;
			break;
		case TERMINAL_KEYBOARD:
			Device = TERMINAL_KEYBOARD;
			break;
		case TERMINAL_DISPLAY:
			Device = TERMINAL_DISPLAY;
			break;
		case TERMINAL_SPEAKER:
			Device = TERMINAL_SPEAKER; 
			break;
		case TERMINAL_CASH_ACCEPTOR:
			Device = TERMINAL_CASH_ACCEPTOR;
			break;
		case DISPENSER:
			Device = DISPENSER;
			break;
		case RFID_ANTENNA:
			Device = RFID_ANTENNA;   //3.0.1.69
		    break;
		default: 
			Device = UNKNOWN_DEVICE;
			break;
	}
	return Device;
}
/*
PumpMode l2PumpMode(long lVal)
{

	PumpMode Mode;
					
	switch(lVal)
	{
		case NORMAL_MODE:
			Mode = NORMAL_MODE;
			break;
		case AUTO_AUTHORISE_MODE:
			Mode = AUTO_AUTHORISE_MODE;
			break;
		case ATTENDED_MODE:
			Mode = ATTENDED_MODE;
			break;
		case TEST_MODE:
			Mode = TEST_MODE;
			break;
		case OPT_DAY_MODE:
			Mode = OPT_DAY_MODE;
			break;
		case OPT_NIGHT_MODE:
			Mode = OPT_NIGHT_MODE; 
			break;
		case OPT_TEST_MODE:
			Mode = OPT_TEST_MODE;
			break;
		case NOT_PRESENT_MODE:
			Mode = NOT_PRESENT_MODE;
			break;

		default: 
			Mode = UNKNOWN_MODE;
			break;

	}
	return Mode;
}
*/


void CPumpStatus::SetError(long lError)
{
	m_lError = lError;
}


void CPumpStatus::SetDevice(PumpDevice lDevice)
{	
	m_lDevice	= lDevice;
}


void CPumpStatus::SetDeviceError(long lError, PumpDevice lDevice)
{
	m_lError	= lError;
	m_lDevice	= lDevice;
}

void CPumpStatus::SetWarning(long lWarning)
{
	m_lWarning = lWarning;
}

void CPumpStatus::SetDeviceWarning(long lWarning, PumpDevice lDevice)
{
	m_lWarning = lWarning;
	m_lDevice  = lDevice;
}

void CPumpStatus::SetMisc(long lMisc)
{
	m_lMisc = lMisc;
}

long CPumpStatus::GetMisc()
{
	return (m_lMisc);
}

void CPumpStatus::SetMiscExt(long lMiscExt) //4.0.21.500 69203
{
	m_lMiscExt = lMiscExt;
}

void CPumpStatus::AddMiscExt(long lMiscExt) //  4.0.23.1800
{
	m_lMiscExt |= lMiscExt;
}

void CPumpStatus::RemoveMiscExt(long lMiscExt) //  4.0.23.1800
{
	long lTmp  = ~lMiscExt;
	m_lMiscExt &= lTmp;
}

long CPumpStatus::GetMiscExt() //4.0.21.500 69203
{
	return (m_lMiscExt);
}

void CPumpStatus::SetMessage(long lMessage)
{
	m_lMessage = lMessage;
}

long CPumpStatus::GetMessage()
{
	return m_lMessage;
}

//4.0.19.500 - TD 59171 Start
void CPumpStatus::SetGrade(short nGrade)
{
	m_nGrade = nGrade;
}

long CPumpStatus::GetGrade()
{
	return m_nGrade;
}

void CPumpStatus::SetPosition(short nPosition)
{
	m_nPosition = nPosition;
}

long CPumpStatus::GetPosition()
{
	return m_nPosition;
}
//4.0.19.500 - TD 59171 End

CTime CPumpStatus::GetTime()
{
	return m_cTime;
}


void CPumpStatus::UpdateTime()
{
	m_cTime	 = CTime::GetCurrentTime();
}


BOOL CPumpStatus::operator !=(const CPumpStatus & pumpStat)
{
	if	(
		(pumpStat.m_lMode		!=	m_lMode		) ||
		(pumpStat.m_lRes		!=	m_lRes		) ||
		(pumpStat.m_lResExt		!=	m_lResExt	) ||
		(pumpStat.m_lStatus		!=	m_lStatus	) ||
		(pumpStat.m_lDevice		!=	m_lDevice	) ||
		(pumpStat.m_lWarning	!=	m_lWarning	) ||
		(pumpStat.m_lError		!=	m_lError	) ||
		(pumpStat.m_lMisc		!=	m_lMisc		) ||
		(pumpStat.m_lMessage	!=	m_lMessage	)
		)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

//3.2.1.44
BOOL CPump::AnyTrsOnOffer(CPumpTransact	trs)
{
	int iMaxTrs=0; int iTrsOnOffer=0;

	iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{
				if((m_cTrsArray[i].GetStatus() == UNPAID)&&(m_cTrsArray[i].GetOnOffer()) ) //3.2.1.46

			iTrsOnOffer++;			
	}

	if(iTrsOnOffer>0)
		return TRUE;
	else
		return FALSE;
}

void CPumpTransact::SetLink(short nLinkFlags, long lLinkNumber)
{
	m_nLinkFlags = nLinkFlags;
	m_lLinkNumber = lLinkNumber;
}

short CPumpTransact::GetLinkFlags()
{
	return(m_nLinkFlags);
}

long CPumpTransact::GetLinkNumber()
{
	return(m_lLinkNumber);
}

CTime CPumpTransact::GetTime()
{
	return m_cTime;
}

//TD 40492, 38654
CTime CPumpTransact::GetStartTime()
{
	return m_cStartTime;
}

//4.0.22.511 72606
CTime CPumpTransact::GetStartFuelingTime()
{
	return m_cStartFuelingTime;
}

void CPumpTransact::UpdateTime()
{
	m_cTime	 = CTime::GetCurrentTime();
}

//TD 40492
void CPumpTransact::UpdateStartTime()
{
	m_cStartTime	 = CTime::GetCurrentTime();
}

//4.0.22.511 72606
void CPumpTransact::UpdateStartFuelingTime()
{
	m_cStartFuelingTime	 = CTime::GetCurrentTime();
}

/*
void CPumpTransact::GetTrsData(PUMP_TRS *pTrsData)
{
	pTrsData->m_lNumber =			m_lNumber;
	pTrsData->m_lStatus =			m_lStatus;
	pTrsData->m_lOnOffer =			m_lOnOffer;
	pTrsData->m_lRawVolume =		m_lRawVolume;
	pTrsData->m_lRoundedVolume =	m_lRoundedVolume;
	pTrsData->m_lRoundedValue =		m_lRoundedValue;
	pTrsData->m_lGradePrice =		m_lGradePrice;
	pTrsData->m_nNozzle =			m_nNozzle;
	pTrsData->m_nGrade =			m_nGrade;
	pTrsData->m_nLevel =			m_nLevel;
	pTrsData->m_lPump	=			m_lPump;
	pTrsData->m_lMode	=			m_lMode;
	pTrsData->m_lRes	=			m_lRes;
	pTrsData->m_lResExt =			m_lResExt ;
	pTrsData->m_lActiveReadNumber	= m_lActiveReadNumber;
	pTrsData->m_lActiveShiftNumber	= m_lActiveShiftNumber;
	pTrsData->m_lMisc	=			m_lMisc;
}

void CPumpTransact::SetTrsData(PUMP_TRS * pTrsData)
{
	m_lNumber =			pTrsData->m_lNumber;
	m_lStatus =			pTrsData->m_lStatus;
	m_lOnOffer =		pTrsData->m_lOnOffer;
	m_lRawVolume =		pTrsData->m_lRawVolume;
	m_lRoundedVolume =	pTrsData->m_lRoundedVolume;
	m_lRoundedValue =	pTrsData->m_lRoundedValue;
	m_lGradePrice =		pTrsData->m_lGradePrice;
	m_nNozzle =			pTrsData->m_nNozzle;
	m_nGrade =			pTrsData->m_nGrade;
	m_nLevel =			pTrsData->m_nLevel;
	m_lPump	=			pTrsData->m_lPump;
	m_lMode	=			pTrsData->m_lMode;
	m_lRes	=			pTrsData->m_lRes;
	m_lResExt =			pTrsData->m_lResExt ;
	m_lActiveReadNumber = pTrsData->m_lActiveReadNumber;
	m_lActiveShiftNumber = pTrsData->m_lActiveShiftNumber;
	m_lMisc = pTrsData->m_lMisc;
}
*/

long CPump::GetType()
{	
	return m_lType;
}

void CPump::GetName(char * pName, int iMaxBuffLen)
{
	memcpy(	pName,(LPCTSTR)m_strPumpMenufacture,min(iMaxBuffLen,m_strPumpMenufacture.GetLength()) );
}

void CPump::GetOlaStat(OLA_STAT * pOlaStat)
{
	*pOlaStat = m_cOla;
}

void CPump::SetOlaStat(OLA_STAT * pOlaStat)
{
	m_cOla = *pOlaStat;
}

void CPump::ClearOlaStat()
{
	m_cOla.m_byState			= OLA_NONE;
	m_cOla.m_byCheckCount	= 0;
	m_cOla.m_lSessionNumber	= 0;
	memset(&m_cOla.m_cSysTime,0,sizeof(SYSTEMTIME));
}

/*
Pumps\Pump01\Brand
Pumps\Pump01\Model 
Pumps\Pump01\ManufacturerName
Pumps\Pump01\BlenderType
Pumps\Pump01\ServiceMode
Pumps\Pump01\PriceLevelDefault
Pumps\Pump01\PriceLevelSelect
Pumps\Pump01\SingleHose
Pumps\Pump01\StartButton
Pumps\Pump01\CardReader
Pumps\Pump01\CashAcceptor
Pumps\Pump01\KeyboardType
Pumps\Pump01\PINEncryption
Pumps\Pump01\ModeDefault 
Pumps\Pump01\ModeSecond 
Pumps\Pump01\ModeStartTime
Pumps\Pump01\ModeEndTime
Pumps\Pump01\ModeAutoChange
Pumps\Pump01\ModeAutoDaily 
Pumps\Pump01\MaxPostPayTrs
Pumps\Pump01\MaxPrePayTrs
Pumps\Pump01\DispensingTimeOut
Pumps\Pump01\UnpaidTrsTimeOut
*/
void CPump::LoadDefinition()
{
	CString str("Pumps\\Pump");
	char sNumber[3];
	memset(sNumber, 0, sizeof(sNumber));
	sprintf(sNumber, "%02d", m_lNumber);
	str += sNumber;
	char tmpCptionStr[MAX_PUMP_CAPTION_LEN + 1] = { 0 }; //4.0.20.501 - TD 66092

														 // QC 501964
	if (_Module.m_server.m_cParam.CheckValidEntry((char *)((LPCTSTR)str)))
	{
		char tmpStr[MAX_FIELD_VAL];
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ManufacturerName", tmpStr, sizeof(tmpStr), "Tokheim", TRUE);
		m_strPumpMenufacture = tmpStr;

		//4.0.20.500 -  TD 64967
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Caption", tmpCptionStr, sizeof(tmpCptionStr), "", TRUE); //4.0.20.501 - TD 66046
		m_strPumpCaption = tmpCptionStr;

		m_lType = 0;

	 // RFUEL-2014
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "MaxPostVoidTrs", &m_dwMaxPostVoidTrsAllowed, MAX_POST_VOID_TRS, TRUE); //CR 351687: Enhanced post void transactions
	if (m_dwMaxPostVoidTrsAllowed > MAX_ACTIVE_POST_VOID)
	{
		m_dwMaxPostVoidTrsAllowed = MAX_ACTIVE_POST_VOID;
		_PARAM.SaveParam((char *)((LPCTSTR)str), "MaxPostVoidTrs", m_dwMaxPostVoidTrsAllowed);
	} // 4.0.27.230
	
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"MaxPostPayTrs", &m_dwMaxUnpaidTrsAllowed,MAX_UNPAID_TRS,TRUE);
	if(m_dwMaxUnpaidTrsAllowed > MAX_UNPAID_TRS )
	{
		m_dwMaxUnpaidTrsAllowed = MAX_UNPAID_TRS;
		_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str),"MaxPostPayTrs", m_dwMaxUnpaidTrsAllowed);
	}

		//4.0.22.501 - start from 4.0.22.12 TD 73062
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "MaxAttendantTrs", &m_dwMaxAttendantTrsAllowed, MAX_ATTENDANT_TRS, TRUE);
		if (m_dwMaxAttendantTrsAllowed > MAX_UNPAID_TRS)
		{
			m_dwMaxAttendantTrsAllowed = MAX_UNPAID_TRS;
			_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "MaxAttendantTrs", m_dwMaxAttendantTrsAllowed);
		}
		//4.0.22.501 - End from 4.0.22.12

	//4.0.0.81  
	if (_Module.m_server.GetPosVoidFlags()) // 4.0.27.230
	{
		if (m_dwMaxPostVoidTrsAllowed == 0)
		{
			if (m_dwMaxUnpaidTrsAllowed < MAX_ACTIVE_POST_VOID)
			{
				_Module.m_server.SetPosVoidFlags(FALSE);
				_Module.m_server.m_cLogger.LogMsg(m_lNumber, LOG_PUMP, "MAX unpaid transaction allowed is one, reject active Post Void !!!!!!");

			}
			else
			{
				// save room for 1 post void
				m_dwMaxUnpaidTrsAllowed -= SLOT_SAVED_FOR_POST_VOID_TRS;
			}
		}
		else
		{
			_Module.m_server.m_cLogger.LogMsg(m_lNumber, LOG_PUMP, "Post void counter is stand alone.");
		}
	}
	
	if (_Module.m_server.GetCashAcceptorActive())
	{
		if (m_dwMaxUnpaidTrsAllowed  > MINIMUM_ACTIVE_CASH_ACCEPTOR)
		{
			m_dwMaxUnpaidTrsAllowed--;
			_Module.m_server.SetCashAcceptorValid(FALSE);
			CString sMsg;
			sMsg.Format("Found Cash acceptor device increse the number of unpaid pak %ld",m_dwMaxUnpaidTrsAllowed);
			_LOGMSG.LogMsg(sMsg);
		}
		else
		{
			CString sMsg;
			_Module.m_server.SetCashAcceptorValid(FALSE);
			_LOGMSG.LogMsg("MAX unpaid transaction allowed is one, reject active Cash acceptor !!!!!!");
		}			
	}
	

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "MaxPrePayTrs", &m_dwMaxPrePayTrsAllowed, MAX_WAITING_TRS, TRUE);
		if (m_dwMaxPrePayTrsAllowed > MAX_WAITING_TRS)
		{
			m_dwMaxPrePayTrsAllowed = MAX_WAITING_TRS;
			_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "MaxPrePayTrs", m_dwMaxPrePayTrsAllowed);
		}

		// Dispensing Time Out 
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "DispensingTimeOut", &m_dwDispensingTimeOut, DEFAULT_DISPENSING_TIMEOUT, TRUE);
		// Unpaid Transaction Time Out
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "UnpaidTrsTimeOut", &m_dwUnpaidTrsTimeOut, DEFAULT_UNPAID_TRS_TIMEOUT, TRUE);
		// PAK Authorization timeout
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PAKAuthorizedTimeOut", &m_dwPAKAuthorizedTimeOut, DEFAULT_PAK_AUTHORIZE_TIMEOUT, TRUE);


		// Default Price Level 
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PriceLevelDefault", &m_dwPriceLevelDefault, (DWORD)PRICE_LEVEL_CASH, TRUE);
		// Default Closed prepay timeout
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ClosedPrePayTimeOut", &m_dwClosedPrePayTimeOut, (DWORD)DEFAULT_CLOSED_PREPAY_TIMEOUT, TRUE);

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "AllowPrePayOnRequestingPump", &m_dwAllowPrePayOnRequestingPump, (DWORD)0, TRUE);

		//_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"UnbalancePrePayCounter", &m_dwUnBalancePrePayCounter,(DWORD)0,TRUE);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "UnbalancePrePayCounter", &m_dwUnBalancePrePayCounter, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE); //4.0.8.505
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PrpDeclienNumber", &m_dwDeclinePrePayCounter, (DWORD)0, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE); //4.0.12.502

																																									 // _Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str),"StackedPrePayAutoTimeout", &m_dwStackedPrePayAutoTimeout,(DWORD)0,TRUE); 	

		DWORD dwTmp = 0;
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "TotalMaxDigits", &dwTmp, (DWORD)6, TRUE);

		//Save at the lMisc field, the pump group.
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Group", &dwTmp, (DWORD)0, TRUE);
		long lMisc = m_cPumpStat.GetMisc();
		lMisc &= MISC_NO_GROUP;
		switch (dwTmp)
		{
		case 0: lMisc |= MISC_GROUP_A; break;
		case 1: lMisc |= MISC_GROUP_B; break;
		case 2: lMisc |= MISC_GROUP_C; break;
		case 3: lMisc |= MISC_GROUP_D; break;
		}
		m_cPumpStat.SetMisc(lMisc);

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ServiceMode", &dwTmp, (DWORD)0, TRUE);
		m_cPumpStat.SetServiceMode((long)dwTmp);

		DWORD dwGrade = 0;
		DWORD dwTank = 0;	//4.0.20.52 (62818)

		CString strPumps("Pumps\\Pump");

		memset(sNumber, 0, sizeof(sNumber));

		sprintf(sNumber, "%02d", m_lNumber);
		strPumps += sNumber;
		for (int p = 1; p <= MAX_POSITIONS_PER_PUMP; p++)
		{
			CString strPosition = strPumps;
			memset(sNumber, 0, sizeof(sNumber));
			sprintf(sNumber, "%ld", p);
			strPosition += "\\Position";
			strPosition += sNumber;

			if (_Module.m_server.m_cParam.CheckValidEntry((char *)(LPCTSTR)strPosition))
			{
				_Module.m_server.m_cParam.LoadParam((char *)(LPCTSTR)strPosition, "Grade1", &dwGrade, p, FALSE);
				SetGradePosition(p, (BYTE)dwGrade);
			}

			if (_Module.m_server.GetSupportDeliveryEmulation() 
			|| _Module.m_server.GetSingleShiftAccumulationsActive()
			|| _Module.m_server.GetActiveTankConnectedPumpsTable() == FALSE)
			{
				_Module.m_server.m_cParam.LoadParam((char *)(LPCTSTR)strPosition, "TankNumber1", &dwTank, p, FALSE);
				SetTankPerPosition(p, (BYTE)dwTank);
			}
		}

		//4.0.3.60
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PumpAsTerminal", &m_dwPumpAsTerminal, (DWORD)0, TRUE);


		//4.0.5.37
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "IPAddress", tmpStr, sizeof(tmpStr), "", TRUE);

		//4.0.6.503
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Simulator", &dwTmp, (DWORD)0, TRUE);

		//4.0.9.509
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "Diesel_pump", &m_dwDieselPump, (DWORD)0, TRUE);


		/*@@@*/
		//Roni Start(version 4.0.9.800)
		//Load price level from registry when system start

		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "ManualMode", &dwTmp, (DWORD)0, TRUE);

		//4.0.13.500
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "MaxTrsNumber", &m_dwMaxTrsNumber, DEFAULT_MAX_TRS_NUMBER, TRUE);

		//4.0.14.500
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "RollbackAfterTrsDelay(Sec)", &dwTmp, 0, TRUE);

		//4.0.19.504 - 47559
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "RequestingOnlyAfterPayInsidePress", &m_dwRequestingOnlyAfterPayInsidePress, (DWORD)0, TRUE);


		/* 4.0.9.509
		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ModeService",&dwTmp,(DWORD)0,TRUE);
		m_lCurrentServiceMode = dwTmp;

		str.Format("Modes\\Mode%02ld",m_lCurrentServiceMode);

		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PriceLevel",&dwTmp,(DWORD)0,TRUE);
		SetPriceLevel(dwTmp);

		_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ModeService",&dwTmp,(DWORD)0,TRUE);
		SetModeService(dwTmp);

		//Roni End
		*/
	}
}

long CPump::GetPriceLevelDefault()
{
	return m_dwPriceLevelDefault;
}	

//4.0.11.509
void CPump::SetPriceLevelDefault(long lLevel)
{
	m_dwPriceLevelDefault = lLevel;
}	

long CPump::GetPAPSeqNumber()
{
	return(a2l(m_cPAPInfo.CardSaleInfo.cardData.sTranNbr,sizeof(m_cPAPInfo.CardSaleInfo.cardData.sTranNbr)));	
}

void CPump::SetPAPSeqNumber(long lSeqNumber)
{
	l2a(lSeqNumber,m_cPAPInfo.CardSaleInfo.cardData.sTranNbr,sizeof(m_cPAPInfo.CardSaleInfo.cardData.sTranNbr));	
}

BYTE CPump::GetPAPTranStatus()
{

	return m_cPAPInfo.CardSaleInfo.cardData.sTranStatus;
}

short CPump::GetMaxUnpaidTrsAllowed()
{
	return (short)m_dwMaxUnpaidTrsAllowed;
}

short CPump::GetMaxPrePayTrsAllowed()
{
	return (short)m_dwMaxPrePayTrsAllowed;
}


short CPump::GetMaxPostVoidTrsAllowed() //CR 351687: Enhanced post void transactions
{
	return (short)m_dwMaxPostVoidTrsAllowed;
}


DWORD CPump::GetDispensingTimeOut()
{
	return m_dwDispensingTimeOut;
}

void CPump::SetDispensingTimeOut(DWORD dwDispensingTimeOut)
{
	m_dwDispensingTimeOut = dwDispensingTimeOut;
}

void CPump::SetAllowPrePayOnRequestingPump(DWORD dwAllowPrePayOnRequestingPump) 
{ 
	m_dwAllowPrePayOnRequestingPump = dwAllowPrePayOnRequestingPump; 
}

DWORD CPump::GetUnpaidTrsTimeOut()
{
	return m_dwUnpaidTrsTimeOut;
}

DWORD CPump::GetClosedPrePayTimeOut()
{
	return m_dwClosedPrePayTimeOut;
}

DWORD CPump::GetPAKAuthorizedTimeOut()
{
	return m_dwPAKAuthorizedTimeOut;
}

void CPump::SetPAKAuthorizedTimeOut(DWORD dwPAKAuthorizedTimeOut)
{
	m_dwPAKAuthorizedTimeOut = dwPAKAuthorizedTimeOut;
}

//4.0.13.500
DWORD CPump::GetMaxTrsNumber()
{
	return m_dwMaxTrsNumber;
}

void CPump::SetMaxTrsNumber(DWORD dwMaxTrsNumber)
{
	m_dwMaxTrsNumber = dwMaxTrsNumber;
}

void CPump::SetGradePosition(short nPosition,BYTE byGrade)
{
	if ( (nPosition <= MAX_POSITIONS_PER_PUMP) && (nPosition > 0) )
		m_byPosition[nPosition-1] = byGrade;
}

//Fix by amram
//the idx is if we have more than one position per grade
//so each loop we will return the next idx for the loop
BYTE CPump::GetGradePosition(short nGrade,long *idx)
{
	BYTE byPosition = 0;
	
	long lIdx=0,lUpdate=0;
	if(!idx)
		lIdx=0;
	else
	{
		lIdx = *idx;
		lUpdate = 1;
	}

	int i = lIdx;

	for ( i ; i < MAX_POSITIONS_PER_PUMP ; i++ )
	{
		if ( m_byPosition[i] == nGrade )
		{
			byPosition = (BYTE)i+1;
			break;
		}
	}
	i++;
	if(lUpdate)
		*idx=i;
	return byPosition;
}

//amram
//get all the position per grade 
long CPump::GetNumOfPositionForGrade(short nGrade)
{
	long  lNumOfPosition=0;
	BYTE byPosition = 0;
	for ( int i = 0 ; i < MAX_POSITIONS_PER_PUMP ; i++ )
	{
		if ( m_byPosition[i] == nGrade )
		{
			lNumOfPosition++;
		}
	}

	return lNumOfPosition;
}

short CPump::NozzleToGrade(short nNozzle)
{
	short nGrade = 0;
	if ( (nNozzle <= MAX_POSITIONS_PER_PUMP) && (nNozzle > 0) )
		nGrade = m_byPosition[nNozzle-1];

	return nGrade;
}

//////////////////////////////////////////////////////////////////////
// CPumpMode Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMode::CMode()
{
	m_lFlags = 0;
	m_lMsgNumber = 1; // 4.0.5.41
	m_lCreditLimit = 0;
	m_lCashLimit = 0;
}

CMode::~CMode()
{
	
}

CMode::CMode(const CMode& mode)
{
	m_lNumber = mode.m_lNumber;			
	m_lFlags = mode.m_lFlags;
	m_sName = mode.m_sName;
	m_lMsgNumber = mode.m_lMsgNumber; // 4.0.5.41
	m_lCreditLimit = mode.m_lCreditLimit;
	m_lCashLimit = mode.m_lCashLimit;
}

CMode::CMode(long lNumber)
{
	m_lNumber = lNumber;
	m_lFlags = 0;
	m_lMsgNumber = 0; // 4.0.5.41
}

BOOL CMode::PAKNotValid()
{
	if (!(m_lFlags & MODE_DEF_AUTO_AUTH_LOYALTY_PAK))	//4.0.23.508 89149
	{
		if( m_lFlags & MODE_DEF_PAK_VALID)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CMode::PAPNotValid()
{
	if( m_lFlags & MODE_DEF_PAP_VALID)
		return FALSE;
	else
		return TRUE;
}

BOOL CMode::PrePayNotValid()
{
	if( m_lFlags & MODE_DEF_PRE_PAY_VALID)
		return FALSE;
	else
		return TRUE;
}

BOOL CMode::PayAttendNotValid()
{
	if( m_lFlags & MODE_DEF_PAY_ATTEND_VALID)
		return FALSE;
	else
		return TRUE;
}

BOOL CMode::PayAVINotValid()
{
	if( m_lFlags & MODE_DEF_PAY_AVI_VALID)
		return FALSE;
	else
		return TRUE;
}


BOOL CMode::PAKValid()
{
	if( m_lFlags & MODE_DEF_PAK_VALID)
		return TRUE;
	else
		return FALSE;
}

BOOL CMode::PAPValid()
{
	if( m_lFlags & MODE_DEF_PAP_VALID)
		return TRUE;
	else
		return FALSE;
}

BOOL CMode::PrePayValid()
{
	if( m_lFlags & MODE_DEF_PRE_PAY_VALID)
		return TRUE;
	else
		return FALSE;
}

BOOL CMode::PayAttendValid()
{
	if( m_lFlags & MODE_DEF_PAY_ATTEND_VALID)
		return TRUE;
	else
		return FALSE;
}

BOOL CMode::PayAVIValid()
{
	if( m_lFlags & MODE_DEF_PAY_AVI_VALID)
		return TRUE;
	else
		return FALSE;
}


BOOL CMode::PAKAutoAuth()
{
	if( m_lFlags & MODE_DEF_PAK_AUTO_AUTH)
		return TRUE;
	else
		return FALSE;
}

BOOL CMode::PAPAutoAuth()
{
	if( m_lFlags & MODE_DEF_PAP_AUTO_AUTH)
		return TRUE;
	else
		return FALSE;
}

BOOL CMode::PrePayAutoAuth()
{
	if( m_lFlags & MODE_DEF_PRE_PAY_AUTO_AUTH)
		return TRUE;
	else
		return FALSE;
}

BOOL CMode::PayAttendAutoAuth()
{
	if( m_lFlags & MODE_DEF_PAY_ATTEND_AUTO_AUTH)
		return TRUE;
	else
		return FALSE;
}

BOOL CMode::PayAVIAutoAuth()
{
	if( m_lFlags & MODE_DEF_PAY_AVI_AUTO_AUTH)
		return TRUE;
	else
		return FALSE;
}

//4.0.29.10 - TD 264032
BOOL CMode::PAPPostPayValid()
{
	if( m_lFlags & MODE_DEF_PAP_POSTPAY_VALID)
		return TRUE;
	else
		return FALSE;
}


BOOL CMode::PAMValid()// CR 444830
{
	if (m_lFlags & MODE_DEF_PAM_VALID)
		return TRUE;
	else
		return FALSE;
}

BOOL CMode::PAMNotValid()// CR 444830
{
	if (m_lFlags & MODE_DEF_PAM_VALID)
		return FALSE;
	else
		return TRUE;
}

BOOL CMode::PAMAutoAuth()// CR 444830
{
	if (m_lFlags & MODE_DEF_AUTO_AUTH_PAM)
		return TRUE;
	else
		return FALSE;
}


void CMode::SetFlags(long lFlags)
{
	m_lFlags = lFlags;
}

void CMode::SetName(CString strName)
{
	m_sName = strName;
}

void CMode::SetName(char *pszName)
{
	m_sName = pszName;
}

long CMode::GetFlags()
{
	return m_lFlags;
}

long CMode::GetMsgNumber()
{
	return m_lMsgNumber;
}
/*@@@*/
//Roni Start(version 4.0.9.800)
long CMode::GetPriceLevel()
{
	return m_lPriceLevel;
}

long CMode::GetModeService()
{
	return	m_lModeService;
}

void CMode::SetModeService(long lModeService)
{
	m_lModeService = lModeService;
}

void CMode::SetPriceLevel(long lPriceLevel)
{
	m_lPriceLevel = lPriceLevel;
}
//Roni End
/*@@@*/

void CMode::SetMsgNumber(long lMsgNumber)
{
	m_lMsgNumber = lMsgNumber;
}

void CMode::SetLimit(long lLimit,long lPriceLevel)
{
	switch(lPriceLevel)
	{
		case PRICE_LEVEL_DEFAULT:
		case PRICE_LEVEL_CREDIT:
			m_lCreditLimit = lLimit;
			break;
		case PRICE_LEVEL_CASH:
			m_lCashLimit = lLimit;	
			break;
	}
}
long CMode::GetLimit(long lPriceLevel)
{
	long lLimit = 0;
	
	switch(lPriceLevel)
	{
		case PRICE_LEVEL_DEFAULT:
		case PRICE_LEVEL_CREDIT:
			lLimit = m_lCreditLimit;
			break;
		case PRICE_LEVEL_CASH:
			lLimit = m_lCashLimit;	
			break;
	}

	return lLimit;
}

void CMode::GetModeDefTagInfo(void *pBuff)
{
	TAG_MODEDEF2 *pTagModeDef;
	pTagModeDef = (TAG_MODEDEF2 *)pBuff; 

	memcpy(pTagModeDef->sModeName,(LPCTSTR)m_sName,
		  min(sizeof(pTagModeDef->sModeName), m_sName.GetLength()) );

	l2a(m_lCashLimit, pTagModeDef->sCashLimit, sizeof(pTagModeDef->sCashLimit));
	l2a(m_lCreditLimit, pTagModeDef->sCreditLimit, sizeof(pTagModeDef->sCreditLimit));
}


CString CMode::GetName()
{
	return m_sName;
}

BOOL CMode::ValidMode()
{
	if( m_lFlags & MODE_DEF_VALID)
		return TRUE;
	else
		return FALSE;
}

long CMode::LoadDefinition()
{
	long lRtc ;
	char szName[MAX_FIELD_VAL]; 
	long lFlags = 0;
	memset(szName,0,sizeof(szName));

	lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"Name",FIELD_SZ,szName,sizeof(szName),NULL,FALSE);

	CString strVal(szName);
	SetName(strVal);

	DWORD dwTmp = 0;
	if(lRtc == 0)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PAKValid",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |= MODE_DEF_PAK_VALID;
	}

	if(lRtc == 0)
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PAKAuto",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |= MODE_DEF_PAK_AUTO_AUTH;
	}

	if(lRtc == 0)  //4.0.3.60
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PAKAutoReconcile",FIELD_DWORD,NULL,0,&dwTmp,FALSE); //4.0.3.51
		if(dwTmp)
			lFlags |=MODE_DEF_PAK_AUTO_RECONCILE;
	}

	if(lRtc == 0)  //4.0.3.60
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PAKAutoLoyaltyOnly",FIELD_DWORD,NULL,0,&dwTmp,FALSE); //4.0.23.508 89149
		if(dwTmp)
			lFlags |=MODE_DEF_AUTO_AUTH_LOYALTY_PAK;
	}

	if(lRtc == 0)
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PAPValid",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |= MODE_DEF_PAP_VALID;
	}

	if(lRtc == 0)
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PAPAuto",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |= MODE_DEF_PAP_AUTO_AUTH;
	}

	if(lRtc == 0)
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PayAttendValid",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |= MODE_DEF_PAY_ATTEND_VALID;
	}

	if(lRtc == 0)
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PayAttendAuto",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |= MODE_DEF_PAY_ATTEND_AUTO_AUTH;
	
	}

	if(lRtc == 0)
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PrePayValid",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |= MODE_DEF_PRE_PAY_VALID;
	}

	if(lRtc == 0)
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PrePayAuto",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |= MODE_DEF_PRE_PAY_AUTO_AUTH;

	}

	if(lRtc == 0)
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PayAVIValid",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |= MODE_DEF_PAY_AVI_VALID;
			
	}

	if(lRtc == 0)
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PayAVIAuto",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |= MODE_DEF_PAY_AVI_AUTO_AUTH;

	}

	if(lRtc == 0)
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"ValidEntry",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |= MODE_DEF_VALID;
	}

	if(lRtc == 0) //4.0.3.60
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"CashAcceptorValid",FIELD_DWORD,NULL,0,&dwTmp,FALSE); //4.0.3.51
		if(dwTmp)
			lFlags |=MODE_DEF_CASH_ACCEPTOR_VALID ;
	}

	if(lRtc == 0)  //4.0.3.60
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"CashAcceptorAuto",FIELD_DWORD,NULL,0,&dwTmp,FALSE); //4.0.3.51
		if(dwTmp)
			lFlags |=MODE_DEF_CASH_ACCEPTOR_AUTH ;
	}
	
	if(lRtc == 0)  //4.0.29.10 - TD 264032
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"PAPPostPayValid",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if(dwTmp)
			lFlags |=MODE_DEF_PAP_POSTPAY_VALID ;
	}


	if (lRtc == 0)
	{
		dwTmp = 0;
		lRtc = _Module.m_server.m_cParam.GetParam("Modes", m_lNumber, 0, "PAMValid", FIELD_DWORD, NULL, 0, &dwTmp, FALSE); // CR 444830
		if (dwTmp)
			lFlags |= MODE_DEF_PAM_VALID;
	}

	if (lRtc == 0)
	{
		dwTmp = 0;
		lRtc = _Module.m_server.m_cParam.GetParam("Modes", m_lNumber, 0, "PAMAuto", FIELD_DWORD, NULL, 0, &dwTmp, FALSE); // CR 444830
		if (dwTmp)
			lFlags |= MODE_DEF_AUTO_AUTH_PAM;
	}

	
	if(lRtc == 0)  //4.0.5.41
	{
		dwTmp = 0;	
		lRtc = _Module.m_server.m_cParam.GetParam("Modes", m_lNumber, 0, "IdlePromptNum", FIELD_DWORD, NULL, 0, &dwTmp, TRUE); 
		m_lMsgNumber = dwTmp;
	}

	if(lRtc == 0)  //4.0.6.506
	{
		dwTmp = 0;	
		lRtc = _Module.m_server.m_cParam.GetParam("Modes", m_lNumber, 0, "CashLimit", FIELD_DWORD, NULL, 0, &dwTmp, TRUE); 
		m_lCashLimit = dwTmp;
	}
	
	if(lRtc == 0)  //4.0.6.506
	{
		dwTmp = 0;	
		lRtc = _Module.m_server.m_cParam.GetParam("Modes", m_lNumber, 0, "CreditLimit", FIELD_DWORD, NULL, 0, &dwTmp, TRUE); 
		m_lCreditLimit = dwTmp;
	}
	
	// 4.0.6.506

	if(lRtc == 0)  //4.0.6.506
	{
		dwTmp = 0;	
		lRtc = _Module.m_server.m_cParam.GetParam("Modes", m_lNumber, 0, "LimitType", FIELD_DWORD, NULL, 0, &dwTmp, TRUE); 
	}


	/*@@@*/
	//Roni Start(version 4.0.9.800)
	if(lRtc == 0) 
	{
		dwTmp = 0;	
		lRtc = _Module.m_server.m_cParam.GetParam("Modes", m_lNumber, 0, "ModeService", FIELD_DWORD, NULL, 0, &dwTmp, TRUE); 
	}
	SetModeService(dwTmp);


	if(lRtc == 0)  
	{
		dwTmp = 0;	
		lRtc = _Module.m_server.m_cParam.GetParam("Modes", m_lNumber, 0, "PriceLevel", FIELD_DWORD, NULL, 0, &dwTmp, TRUE); 
	}
	SetPriceLevel(dwTmp);
	//Roni End
	/*@@@*/

	//4.0.14.503
	if(lRtc == 0)
	{
		dwTmp = 0;		
		lRtc = _Module.m_server.m_cParam.GetParam("Modes",m_lNumber,0,"UnmannedMode",FIELD_DWORD,NULL,0,&dwTmp,TRUE);
		if(dwTmp)
			lFlags |=MODE_DEF_UNMANNED_MODE ;
	}

	m_lFlags = lFlags;

	return lRtc;

}

void CMode::operator =( const CMode& mode )
{
	m_lFlags = mode.m_lFlags;
	m_lNumber = mode.m_lNumber;
	m_sName = mode.m_sName;
	m_lMsgNumber = mode.m_lMsgNumber; // 4.0.5.41
	m_lCreditLimit = mode.m_lCreditLimit;
	m_lCashLimit = mode.m_lCashLimit;
	//Roni Start(version 4.0.9.800)
	m_lPriceLevel  = mode.m_lPriceLevel;
	m_lModeService = mode.m_lModeService;
	//Roni End
}

BOOL CMode::ResNotValid(PumpReservation lRes, PumpResExt lResExt)
{
	BOOL bNotValid = TRUE;

	switch (lRes)
	{
		case ATTEND:
			bNotValid = PayAttendNotValid();
			break;
		case PAY_AT_PUMP:
			switch(lResExt)
			{
				case RES_EXT_AVI: 
				case RES_EXT_RFID_CT:
				case RES_EXT_RFID_HT:
					bNotValid = PayAVINotValid();
					break;
				case NULL_RES_EXT:
				default: 
					bNotValid = PAPNotValid();
					break;
			}
			break;
		case PAY_AT_KIOSK:
			if (lResExt == RES_EXT_ATTENDANT)  //4.0.1.37
			{
				bNotValid = PayAttendNotValid();					
			}
			else
				bNotValid = PAKNotValid();
			break;
		case PRE_PAY_AT_POS:
			bNotValid = PrePayNotValid();
			break;
	}

	return bNotValid;	
}


BOOL CMode::AutoAuthorize(PumpReservation lRes, PumpResExt lResExt)
{

	BOOL bAuto = FALSE;

	switch (lRes)
	{
		case ATTEND:
			bAuto = PayAttendAutoAuth();
			break;
		case PAY_AT_PUMP:
			switch(lResExt)
			{
				case RES_EXT_AVI: 
				case RES_EXT_RFID_CT:
				case RES_EXT_RFID_HT:
					bAuto = PayAVIAutoAuth();				
					break;
				case RES_EXT_PAP_CANCELED: //marik 
					break;
				case RES_EXT_MOBILE:
					bAuto = PAMAutoAuth();
					break;
				case NULL_RES_EXT:
				default: 
					bAuto = PAPAutoAuth();
					break;
			}

		break;
		case PAY_AT_KIOSK:			
		{
			if (lResExt == RES_EXT_ATTENDANT)
			{
				if (PayAttendValid())
					bAuto = PayAttendAutoAuth();
			}
			else if (PAKValid())   // 2.9.1.4 
			{
				if (!PAKAutoAuthLoyaltyOnly()) //4.0.23.508 89149
					bAuto = PAKAutoAuth();
				else
				{
					bAuto = FALSE;	//4.0.23.508 89149 Auto auth is not allowed when only loyalty authorizes the pump
				}
			}

		}
		break;
		case PRE_PAY_AT_POS:
			if ((lResExt == RES_EXT_CASH_ACCEPTOR) && (CashAcceptorValid()))  //4.0.3.60  //4.0.5.0
				bAuto = CashAcceptorAuth();
			else
				bAuto = PrePayAutoAuth();
			break;
	}

	return bAuto;
}

/******************************************************************************
 Description:	Checks if need to lock pump due to valid mode with
				no choosen transaction types.
 Returns:      	BOOL
 Parameters:   	None.
		
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			27/8/2003   11:21		Start	4.0.3.49
******************************************************************************/
BOOL CMode::IsNeedToLockPump()
{
	BOOL bNeedToLockPump = FALSE;

	/*This is the only situation where the entry is valid
	  and there are no valid transactions types*/
	if(m_lFlags == MODE_DEF_VALID)
		bNeedToLockPump = TRUE;
	
	return bNeedToLockPump;
}

//////////////////////////////////////////////////////////////////////
// CModeScheme Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

long CModeScheme::LoadDefinition(long lVersion)
{

	long lRtc = 0;
	SZ_TAG_SYSTEMTIME szTagTime;		
	SYSTEMTIME			sysTime;
	szTagTime.sNull = 0;
	
	m_lFlags = 0;

	DWORD dwTmp = 0;

	lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"ValidEntry",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
	if(dwTmp)
		m_lFlags |= MODE_SCHEME_VALID;

	// Read only mode scheme valid
	if (dwTmp)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"StartTime",FIELD_SZ,(char *)&szTagTime,sizeof(szTagTime),NULL,FALSE);
		if(lRtc == 0)
		{
			Tag2SystemTime(&szTagTime.sTime,&sysTime);
			CTime	cRcvTime(sysTime);	
			m_cStartTime = cRcvTime;

	//		SYSTEMTIME sysStartTime;
	//		m_cStartTime.GetAsSystemTime(sysStartTime);

		}

		szTagTime.sNull = 0;
		if(lRtc == 0)
		{
			lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"EndTime",FIELD_SZ,(char *)&szTagTime,sizeof(szTagTime),NULL,FALSE);
			Tag2SystemTime(&szTagTime.sTime,&sysTime);
			CTime	cRcvTime(sysTime);
			m_cEndTime = cRcvTime;

	//		SYSTEMTIME sysEndTime;
	//		m_cEndTime.GetAsSystemTime(sysEndTime);

		}

		if (lVersion)  // new fields in Modescheme
		{
			szTagTime.sNull = 0;
			if(lRtc == 0)
			{
				lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"PeriodStartTime",FIELD_SZ,(char *)&szTagTime,sizeof(szTagTime),NULL,FALSE);
				Tag2SystemTime(&szTagTime.sTime,&sysTime);
				CTime	cRcvTime(sysTime);
				m_cPeriodStartTime = cRcvTime;
			}
			
			szTagTime.sNull = 0;
			if(lRtc == 0)
			{
				lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"PeriodEndTime",FIELD_SZ,(char *)&szTagTime,sizeof(szTagTime),NULL,FALSE);
				Tag2SystemTime(&szTagTime.sTime,&sysTime);
				CTime	cRcvTime(sysTime);
				m_cPeriodEndTime = cRcvTime;
			}
		}

		dwTmp = 0;		
		if(lRtc == 0)
		{
			lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"ModeNumber",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
			m_lMode = (long)dwTmp;		
		}

		if(lRtc == 0)
		{
			dwTmp = 0;
			lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"AutoDaily",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
			if(dwTmp)
				m_lFlags |= MODE_SCHEME_DAILY;
		}

		if(lRtc == 0)
		{
			dwTmp = 0;
			lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"AutoWeekly",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
			if(dwTmp)
				m_lFlags |= MODE_SCHEME_WEEKLY;
		}

		if(lRtc == 0)
		{
			dwTmp = 0;
			lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"AutoYearly",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
			if(dwTmp)
				m_lFlags |= MODE_SCHEME_YEARLY;
		}

		//3.0.2.3
		if(lRtc == 0)
		{
			dwTmp = 0;
			lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"ServiceFeeSet",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
						
			m_lServiceFeeSet = long(dwTmp);
		}

		//4.0.2.21
		if(lRtc == 0)
		{
			dwTmp = 0;
			lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"ServiceMode",FIELD_DWORD,NULL,0,&dwTmp,FALSE);

			if (dwTmp == MODE_SHCEME_FULL_SERVICE)
			{
				m_lFlags |= MODE_SCHEME_FULL_SERVICE_VALID;
			}
			else if (dwTmp == MODE_SHCEME_SELF_SERVICE)			
			{
				m_lFlags |= MODE_SCHEME_SELF_SERVICE_VALID;
			}			
			m_lServiceMode = long(dwTmp);
		}
		//4.0.2.21

	//	if(lRtc == 0)
	//	{
	//		dwTmp = 0;
	//		lRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"ValidEntry",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
	//		if(dwTmp)
	//			m_lFlags |= MODE_SCHEME_VALID;
	//	}

	}
		
	return lRtc;
}

BOOL CModeScheme::ValidModeScheme()
{
	if( m_lFlags & MODE_SCHEME_VALID)
		return TRUE;
	else
		return FALSE;
}

long CModeScheme::GetFlags()
{
	return m_lFlags;
}

void CModeScheme::SetFlags(long lFlags)
{
	m_lFlags = lFlags;
}

BOOL CModeScheme::DailyModeScheme()
{
	if( m_lFlags & MODE_SCHEME_DAILY)
		return TRUE;
	else
		return FALSE;
}

BOOL CModeScheme::WeeklyModeScheme()
{
	if( m_lFlags & MODE_SCHEME_WEEKLY)
		return TRUE;
	else
		return FALSE;
}

CModeScheme::CModeScheme()
{

}

CModeScheme::~CModeScheme()
{

}

CModeScheme::CModeScheme(const CModeScheme& modeScheme)
{
	m_lNumber = modeScheme.m_lNumber;			
	m_lPump = modeScheme.m_lPump;
	m_lMode = modeScheme.m_lMode;			
	m_lFlags = modeScheme.m_lFlags;
	m_cEndTime = modeScheme.m_cEndTime;
	m_cStartTime = modeScheme.m_cStartTime;	
	m_cPeriodStartTime = modeScheme.m_cPeriodStartTime;
	m_cPeriodEndTime = modeScheme.m_cPeriodEndTime;
}

CModeScheme::CModeScheme(long lPumpNumber,long lScheme)
{
	m_lNumber = lScheme;
	m_lPump = lPumpNumber;
}

BOOL CModeScheme::Active(SYSTEMTIME & sysTime)
{
	BOOL bActive = FALSE;
	BOOL bAdjustDate = FALSE;
	SYSTEMTIME sysStartTime,sysEndTime;  //,sysTime;	
	CTime	cCurTime(sysTime);

	if(ValidModeScheme())
	{
		BOOL	bCheckTime	= TRUE;
		BOOL	bUpdateSysTime = FALSE;

		if (!IsValidPeriod())
		{
			return (bActive);
		}
		else
			bActive = TRUE;	//4.0.3.14

		m_cStartTime.GetAsSystemTime(sysStartTime);
		m_cEndTime.GetAsSystemTime(sysEndTime);

		CTime	tStart = m_cStartTime;
		CTime	tEnd = m_cEndTime;
		CTimeSpan ts(1,0,0,0); // 1 day

		//4.0.3.14 start
		if(YearlyModeScheme())
		{	
			bCheckTime = FALSE;		
		}
		//4.0.3.14 end
		//YA - 4.0.0.31 start
		if(WeeklyModeScheme())
		{
			bActive = InWeeklyTime(sysTime);
			bCheckTime = FALSE;
		}
		//YA - 4.0.0.31 end
	
		if(DailyModeScheme())
		{			
			bActive = FALSE;//4.0.3.14
			bCheckTime	= TRUE;//4.0.3.14
			//Do the update here and only for Daily logic
			if(sysStartTime.wDayOfWeek == sysEndTime.wDayOfWeek)
			{
				sysStartTime.wYear = sysTime.wYear; 
				sysStartTime.wMonth = sysTime.wMonth;			
				sysStartTime.wDayOfWeek = sysTime.wDayOfWeek;
				sysStartTime.wDay = sysTime.wDay;

				sysEndTime.wYear = sysTime.wYear; 
				sysEndTime.wMonth = sysTime.wMonth;
				sysEndTime.wDayOfWeek = sysTime.wDayOfWeek;
				sysEndTime.wDay = sysTime.wDay;

				CTime	cTmpTime1(sysEndTime);
				tEnd = cTmpTime1;

				CTime	cTmpTime2(sysStartTime);
				tStart = cTmpTime2;

				// do nothing 
			}
			else // End Day != Start Day  
			{
				sysStartTime.wYear = sysTime.wYear; 
				sysStartTime.wMonth = sysTime.wMonth;			
				sysStartTime.wDayOfWeek = sysTime.wDayOfWeek;
				sysStartTime.wDay = sysTime.wDay;

				sysEndTime.wYear = sysTime.wYear; 
				sysEndTime.wMonth = sysTime.wMonth;
				sysEndTime.wDayOfWeek = sysTime.wDayOfWeek;
				sysEndTime.wDay = sysTime.wDay;

				CTime	cTmpTime1(sysEndTime);
				tEnd = cTmpTime1;

				CTime	cTmpTime2(sysStartTime);
				tStart = cTmpTime2;

				tEnd += ts;
				
				if((cCurTime >= tStart) &&  (cCurTime <= tEnd))
				{
					// do nothing 
				}
				else
				{
					tEnd -= ts;
					tStart -= ts;
				}	
			}
		}

		if(bAdjustDate)
		{
			int lAddDayFlags = 0;

			if (sysEndTime.wHour < sysStartTime.wHour)
			{
				lAddDayFlags = 1; 
			}

			if ((sysEndTime.wHour == sysStartTime.wHour) && 
				(sysEndTime.wMinute < sysStartTime.wMinute))
			{
				lAddDayFlags = 1; 
			}

			sysEndTime.wYear = sysTime.wYear; 
			sysEndTime.wMonth = sysTime.wMonth;			
			sysEndTime.wDayOfWeek = sysTime.wDayOfWeek;
			sysEndTime.wDay = sysTime.wDay;
			sysStartTime.wYear = sysTime.wYear; 
			sysStartTime.wMonth = sysTime.wMonth;			
			sysStartTime.wDayOfWeek = sysTime.wDayOfWeek;
			sysStartTime.wDay = sysTime.wDay;

			if (lAddDayFlags)
			{
				CTime	cTmpTime1(sysEndTime);
				tEnd = cTmpTime1 + ts;  
			}else
			{
				CTime	cTmpTime1(sysEndTime);
				tEnd = cTmpTime1;
			}	
			CTime	cTmpTime2(sysStartTime);
			tStart = cTmpTime2;
		}

		if(bCheckTime)
		{
			//4.0.7.500 - start
			if(DailyModeScheme())
			{
				if((cCurTime >= tStart) &&  (cCurTime <= tEnd))
				{
					bActive = TRUE;
				}
				else
					bActive = FALSE;//4.0.3.14
			}
			else
			{
				SYSTEMTIME sysPeriodStartTime, sysPeriodEndTime;  	
				sysPeriodStartTime = sysStartTime;
				sysPeriodEndTime = sysEndTime;

				sysPeriodStartTime.wDay = m_cPeriodStartTime.GetDay ();
				sysPeriodStartTime.wDayOfWeek = m_cPeriodStartTime.GetDayOfWeek();
				sysPeriodStartTime.wMonth = m_cPeriodStartTime.GetMonth();
				sysPeriodStartTime.wYear = m_cPeriodStartTime.GetYear();

				sysPeriodEndTime.wDay = m_cPeriodEndTime.GetDay ();
				sysPeriodEndTime.wDayOfWeek = m_cPeriodEndTime.GetDayOfWeek();
				sysPeriodEndTime.wMonth = m_cPeriodEndTime.GetMonth();
				sysPeriodEndTime.wYear = m_cPeriodEndTime.GetYear();

				CTime cPeriodStart(sysPeriodStartTime);
				CTime cPeriodEnd(sysPeriodEndTime);

				if((cCurTime >= cPeriodStart) &&  (cCurTime <= cPeriodEnd))
				{
					bActive = TRUE;
				}
				else
				{
					bActive = FALSE;
				}										 
			}
			//4.0.7.500 - end
		}			
	}

	return bActive;
}

//YA - 4.0.0.31 start
BOOL CModeScheme::InWeeklyTime(SYSTEMTIME & sysTime)
{
	BOOL bWeeklyTime = FALSE;
	SYSTEMTIME sysStartTime,sysEndTime;  //,sysTime;

	m_cStartTime.GetAsSystemTime(sysStartTime);
	m_cEndTime.GetAsSystemTime(sysEndTime);

	if(sysStartTime.wDayOfWeek > sysEndTime.wDayOfWeek )
	{
	// Over weekend
		if( (sysTime.wDayOfWeek > sysStartTime.wDayOfWeek) || 
			(sysTime.wDayOfWeek < sysEndTime.wDayOfWeek) )
		{
			bWeeklyTime = TRUE;
		}
		else if(sysTime.wDayOfWeek == sysStartTime.wDayOfWeek)
		{
			if( sysTime.wHour > sysStartTime.wHour )
			{
				bWeeklyTime = TRUE;
			}
			else if( sysTime.wHour == sysStartTime.wHour && sysTime.wMinute >= sysStartTime.wMinute)
			{
				bWeeklyTime = TRUE;
			}
			else
				bWeeklyTime = FALSE;
		}
		else if(sysTime.wDayOfWeek == sysEndTime.wDayOfWeek)
		{
			if( sysTime.wHour < sysEndTime.wHour )
			{
				bWeeklyTime = TRUE;
			}
			else if( sysTime.wHour == sysEndTime.wHour && sysTime.wMinute <= sysEndTime.wMinute)
			{
				bWeeklyTime = TRUE;
			}
			else
				bWeeklyTime = FALSE;
		}
		else
			bWeeklyTime = FALSE;
	}
	else if( sysStartTime.wDayOfWeek < sysEndTime.wDayOfWeek)
	// middle of week 
	{
		if((sysTime.wDayOfWeek > sysStartTime.wDayOfWeek) && 
			(sysTime.wDayOfWeek < sysEndTime.wDayOfWeek) )
		{
			bWeeklyTime = TRUE;
		}
		else if(sysTime.wDayOfWeek == sysStartTime.wDayOfWeek)
		{
			if( sysTime.wHour > sysStartTime.wHour )
			{
				bWeeklyTime = TRUE;
			}
			else if( sysTime.wHour == sysStartTime.wHour && sysTime.wMinute >= sysStartTime.wMinute)
			{
				bWeeklyTime = TRUE;
			}
			else
				bWeeklyTime = FALSE;
		}
		else if(sysTime.wDayOfWeek == sysEndTime.wDayOfWeek)
		{
			if( sysTime.wHour < sysEndTime.wHour )
			{
				bWeeklyTime = TRUE;
			}
			else if( sysTime.wHour == sysEndTime.wHour && sysTime.wMinute <= sysEndTime.wMinute)
			{
				bWeeklyTime = TRUE;
			}
			else
				bWeeklyTime = FALSE;
		}
		else
			bWeeklyTime = FALSE;
	}
	//4.0.5.27
	else  if ((sysStartTime.wDayOfWeek == sysEndTime.wDayOfWeek ) &&
		       (sysTime.wDayOfWeek == sysEndTime.wDayOfWeek ))
	{
		CTime cCurTime(sysTime);
		sysStartTime.wMonth = sysEndTime.wMonth  = sysTime.wMonth;
		sysStartTime.wYear = sysEndTime.wYear    = sysTime.wYear;
		sysStartTime.wDay = sysEndTime.wDay    = sysTime.wDay;
		CTime cStart(sysStartTime);
		CTime cEnd(sysEndTime);		
		
		if((cCurTime >= cStart) &&  (cCurTime <= cEnd)) 
		{
			bWeeklyTime = TRUE;
		}
		else
			bWeeklyTime = FALSE;
					 
	}
	
	return bWeeklyTime;
}
//YA - 4.0.0.31 end
/******************************************************************************
 Description:	Checks if in daily time in current period only. 	
 Returns:      	BOOL - TRUE if in, FALSE if not
 Parameters:   	SYSTEMTIME & sysTime - current system time.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			12/3/2003   15:01		Start	4.0.2.40
******************************************************************************/
BOOL CModeScheme::InPeriodDailyTime(SYSTEMTIME & sysTime)
{
	BOOL bDailyTime = FALSE;
	SYSTEMTIME sysStartTime,sysEndTime;  //,sysTime;
	CTime tEnd,tStart;
	CTime	cCurTime(sysTime);
	
	m_cStartTime.GetAsSystemTime(sysStartTime);
	m_cEndTime.GetAsSystemTime(sysEndTime);

	sysStartTime.wYear = sysTime.wYear; 
	sysStartTime.wMonth = sysTime.wMonth;			
	sysStartTime.wDayOfWeek = sysTime.wDayOfWeek;
	sysStartTime.wDay = sysTime.wDay;

	sysEndTime.wYear = sysTime.wYear; 
	sysEndTime.wMonth = sysTime.wMonth;
	sysEndTime.wDayOfWeek = sysTime.wDayOfWeek;
	sysEndTime.wDay = sysTime.wDay;

	CTime	cTmpTime1(sysEndTime);
	tEnd = cTmpTime1;

	CTime	cTmpTime2(sysStartTime);
	tStart = cTmpTime2;
	
	if((cCurTime >= tStart) &&  (cCurTime <= tEnd))
	{
		bDailyTime = TRUE;
	}//check if time is from bigger hour to smaller e.g 22:00 to 06:00
	else if(tStart > tEnd)	
	{
		if((cCurTime >= tStart) ||  (cCurTime <= tEnd))	
			bDailyTime = TRUE;
	}	
	
	return bDailyTime;
}

void CModeScheme::SetStartTime(TAG_SYSTEMTIME *pTagTime)
{
	SYSTEMTIME systime;
	Tag2SystemTime(pTagTime,&systime);
	SetStartTime(systime);
}

void CModeScheme::SetStartTime(SYSTEMTIME sysTime)
{
	CTime	cRcvTime(sysTime);
	m_cStartTime = cRcvTime;
}


void CModeScheme::SetEndTime(TAG_SYSTEMTIME *pTagTime)
{
	SYSTEMTIME systime;
	Tag2SystemTime(pTagTime,&systime);
	SetEndTime(systime);
}

void CModeScheme::SetEndTime(SYSTEMTIME sysTime)
{
	CTime	cRcvTime(sysTime);
	m_cEndTime = cRcvTime;
}

CTime CModeScheme::GetStartTime()
{	
	return m_cStartTime;
}


CTime CModeScheme::GetEndTime()
{	
	return m_cEndTime;
}


CString CModeScheme::GetStringStartTime()
{
	SYSTEMTIME sysTime;
	SZ_TAG_SYSTEMTIME szTagTime;		

	m_cStartTime.GetAsSystemTime(sysTime);

	
	szTagTime.sNull = 0;
	SystemTime2Tag(&sysTime,&szTagTime.sTime);
	CString strVal((char *)&szTagTime);	

	return strVal;
}

CString CModeScheme::GetStringEndTime()
{
	SYSTEMTIME sysTime;
	SZ_TAG_SYSTEMTIME szTagTime;		

	m_cEndTime.GetAsSystemTime(sysTime);
	szTagTime.sNull = 0;
	SystemTime2Tag(&sysTime,&szTagTime.sTime);
	CString strVal((char *)&szTagTime);	

	return strVal;
}

long CModeScheme::GetMode()
{
	return m_lMode;
}

void CMode::SetNumber(long lNumber)
{
	m_lNumber = lNumber;
}

void CModeScheme::SetMode(long lMode)
{
	m_lMode = lMode;
}

long CModeScheme::SaveDefinition(long lVersion)
{

	long lRtc = 0;
	long lGetRtc = 0;
	SYSTEMTIME sysTime;
	SZ_TAG_SYSTEMTIME szTagTime;		
	SZ_TAG_SYSTEMTIME szTmpTagTime;		
	DWORD dwTmp = 0;
	DWORD dwCurrParam = 0;

	if(	m_lFlags & MODE_SCHEME_VALID)
		dwTmp = 1;

	/*4.0.1.38 start
	In ClientDataEvent we send the index. Doing in the last parameter only*/
	long lClientEventData = _Module.m_server.GenerateModeSchemeIndex(m_lPump , m_lNumber); 

	if(lRtc == 0)
	{	
		lGetRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"ValidEntry",FIELD_DWORD,NULL,0,&dwCurrParam,FALSE);

		if (lGetRtc || (!lGetRtc && (dwTmp != dwCurrParam)))
		{
			lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"ValidEntry",FIELD_DWORD,NULL,dwTmp,FALSE,_HOST.m_lGroupNum,0,lClientEventData);
		}

		//4.0.1.38 end - For ClientDataEvent
	}

	//4.0.13.500
	//Only if the mode scheme is valid - update the other fields.
	if (m_lFlags & MODE_SCHEME_VALID)
	{

		m_cStartTime.GetAsSystemTime(sysTime);
		szTagTime.sNull = 0;
		SystemTime2Tag(&sysTime,&szTagTime.sTime);

		lGetRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"StartTime",FIELD_SZ,(char *)&szTmpTagTime,sizeof(szTagTime),NULL,FALSE);

		//Set the param only if the current val is different from the new val. 
		if (lGetRtc || (!lGetRtc && (strcmp((char *)&szTagTime, (char *)&szTmpTagTime))))
		{
	  	  //lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"StartTime",FIELD_SZ,(char *)&szTagTime,0,FALSE);
			lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"StartTime",FIELD_SZ,(char *)&szTagTime,0,FALSE,_HOST.m_lGroupNum,0,lClientEventData);	 //4.0.17.501 merge from 4.0.15.200

		}

		m_cEndTime.GetAsSystemTime(sysTime);
		szTagTime.sNull = 0;
		SystemTime2Tag(&sysTime,&szTagTime.sTime);
		if(lRtc == 0)
		{
			lGetRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"EndTime",FIELD_SZ,(char *)&szTmpTagTime,sizeof(szTagTime),NULL,FALSE);

			//Set the param only if the current val is different from the new val. 
			if (lGetRtc || (!lGetRtc && (strcmp((char *)&szTagTime, (char *)&szTmpTagTime))))
			{
			//	lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"EndTime",FIELD_SZ,(char *)&szTagTime,0,FALSE);								
				lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"EndTime",FIELD_SZ,(char *)&szTagTime,0,FALSE,_HOST.m_lGroupNum,0,lClientEventData);	 //4.0.17.501 merge from 4.0.15.200
			}

		}

		if (lVersion )  //new fields in modeschme
		{
			if(lRtc == 0)
			{
				if (m_cPeriodStartTime == 0)
				{
					// in case unlimited period time 
					memset(&szTagTime.sTime , ' ' , sizeof(TAG_SYSTEMTIME));
					szTagTime.sNull = 0;
				}
				else
				{
					m_cPeriodStartTime.GetAsSystemTime(sysTime);
					szTagTime.sNull = 0;
					SystemTime2Tag(&sysTime,&szTagTime.sTime);
				}


				lGetRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"PeriodStartTime",FIELD_SZ,(char *)&szTmpTagTime,sizeof(szTagTime),NULL,FALSE);

				//Set the param only if the current val is different from the new val. 
				if (lGetRtc || (!lGetRtc && (strcmp((char *)&szTagTime, (char *)&szTmpTagTime))))
				{
				//	lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"PeriodStartTime",FIELD_SZ,(char *)&szTagTime,0,FALSE);								
					lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"PeriodStartTime",FIELD_SZ,(char *)&szTagTime,0,FALSE,_HOST.m_lGroupNum,0,lClientEventData);	 //4.0.17.501 merge from 4.0.15.200
				}

			}

			if(lRtc == 0)
			{
				if (m_cPeriodEndTime == 0 )
				{ 
					// in case unlimited period time 
					memset(&szTagTime.sTime , ' ' , sizeof(TAG_SYSTEMTIME));
					szTagTime.sNull = 0;
				}
				else
				{
					m_cPeriodEndTime.GetAsSystemTime(sysTime);
					szTagTime.sNull = 0;
					SystemTime2Tag(&sysTime,&szTagTime.sTime);
				}

				lGetRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"PeriodEndTime",FIELD_SZ,(char *)&szTmpTagTime,sizeof(szTagTime),NULL,FALSE);

				//Set the param only if the current val is different from the new val. 
				if (lGetRtc || (!lGetRtc && (strcmp((char *)&szTagTime, (char *)&szTmpTagTime))))
				{
				//	lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"PeriodEndTime",FIELD_SZ,(char *)&szTagTime,0,FALSE);								
					lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"PeriodEndTime",FIELD_SZ,(char *)&szTagTime,0,FALSE,_HOST.m_lGroupNum,0,lClientEventData);	 //4.0.17.501 merge from 4.0.15.200
				}
			}

		}

		if(lRtc == 0)
		{

			lGetRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"ModeNumber",FIELD_DWORD,NULL,0,&dwCurrParam,FALSE);

			if (lGetRtc || (!lGetRtc && (m_lMode != (long)dwCurrParam)))
			{
			//	lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"ModeNumber",FIELD_DWORD,NULL,(DWORD)m_lMode,FALSE);		
				lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"ModeNumber",FIELD_DWORD,NULL,(DWORD)m_lMode,FALSE,_HOST.m_lGroupNum,0,lClientEventData);	 //4.0.17.501 merge from 4.0.15.200
			}
		}

		dwTmp = 0;
		if(	m_lFlags & MODE_SCHEME_DAILY)
			dwTmp = 1;

		if(lRtc == 0)
		{
			lGetRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"AutoDaily",FIELD_DWORD,NULL,0,&dwCurrParam,FALSE);

			if (lGetRtc || (!lGetRtc && (dwTmp != dwCurrParam)))
			{
			//	lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"AutoDaily",FIELD_DWORD,NULL,dwTmp,FALSE);
				lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"AutoDaily",FIELD_DWORD,NULL,dwTmp,FALSE,_HOST.m_lGroupNum,0,lClientEventData);	 //4.0.17.501 merge from 4.0.15.200
			}
		}

		dwTmp = 0;
		if(	m_lFlags & MODE_SCHEME_WEEKLY)
			dwTmp = 1;

		if(lRtc == 0)
		{
			lGetRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"AutoWeekly",FIELD_DWORD,NULL,0,&dwCurrParam,FALSE);

			if (lGetRtc || (!lGetRtc && (dwTmp != dwCurrParam)))
			{
			//	lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"AutoWeekly",FIELD_DWORD,NULL,dwTmp,FALSE);
				lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"AutoWeekly",FIELD_DWORD,NULL,dwTmp,FALSE,_HOST.m_lGroupNum,0,lClientEventData);	 //4.0.17.501 merge from 4.0.15.200
			}
		}

		if (lVersion )  //new fields in modeschme
		{
			dwTmp = 0;
			if(	m_lFlags & MODE_SCHEME_YEARLY)
				dwTmp = 1;

			if(lRtc == 0)
			{
				lGetRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"AutoYearly",FIELD_DWORD,NULL,0,&dwCurrParam,FALSE);

				if (lGetRtc || (!lGetRtc && (dwTmp != dwCurrParam)))
				{
				//	lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"AutoYearly",FIELD_DWORD,NULL,dwTmp,FALSE);
					lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"AutoYearly",FIELD_DWORD,NULL,dwTmp,FALSE,_HOST.m_lGroupNum,0,lClientEventData);	//4.0.17.501 merge from 4.0.15.200
				}
			}
			
			//3.0.2.3
			if(lRtc == 0)
			{
				dwTmp = m_lServiceFeeSet;

				lGetRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"ServiceFeeSet",FIELD_DWORD,NULL,0,&dwCurrParam,FALSE);

				if (lGetRtc || (!lGetRtc && (dwTmp != dwCurrParam)))
				{
				//	lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"ServiceFeeSet",FIELD_DWORD,NULL,dwTmp,FALSE);
					lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"ServiceFeeSet",FIELD_DWORD,NULL,dwTmp,FALSE,_HOST.m_lGroupNum,0,lClientEventData);	 //4.0.17.501 merge from 4.0.15.200
				}
			}

			//4.0.2.21
			if(lRtc == 0)
			{
				//4.0.2.23
				if (m_lFlags & MODE_SCHEME_FULL_SERVICE_VALID)
				{
					dwTmp = MODE_SHCEME_FULL_SERVICE;
				}
				else if (m_lFlags & MODE_SCHEME_SELF_SERVICE_VALID )
				{
					dwTmp = MODE_SHCEME_SELF_SERVICE;
				} 
				else
				{
					dwTmp= 0;
				}
				//4.0.2.23

				lGetRtc = _Module.m_server.m_cParam.GetParam("ModeScheme",m_lPump,m_lNumber,"ServiceMode",FIELD_DWORD,NULL,0,&dwCurrParam,FALSE);
				if (lGetRtc || (!lGetRtc && (dwTmp != dwCurrParam)))
				{
				//	lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"ServiceMode",FIELD_DWORD,NULL,dwTmp,FALSE);
					lRtc = _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"ServiceMode",FIELD_DWORD,NULL,dwTmp,FALSE,_HOST.m_lGroupNum,0,lClientEventData);	 //4.0.17.501 merge from 4.0.15.200
				}
			}
			//4.0.2.21

		}
	}

	return lRtc;
}


long CModeScheme::SaveNotValid()
{
	m_lFlags &= MODE_SCHEME_NOT_VALID;

	return ( _Module.m_server.m_cParam.SetParam("ModeScheme",m_lPump,m_lNumber,"ValidEntry",FIELD_DWORD,NULL,0,FALSE));
}

CLanguages::CLanguages()
{
}

CLanguages::~CLanguages()
{
}


//4.0.2.23 Start
BOOL CPump::PumpStoppedByController()
{
	BOOL bRetVal = FALSE;

	if(m_cPumpStat.GetStatus() == STOPPED_BY_CONTROLLER)
		bRetVal = TRUE;

	return bRetVal;
}
//4.0.2.23 End

BOOL CPump::CanStopPump()
{
	BOOL bCanStop = TRUE;

	if( (GetStat().GetStatus() == OUT_OF_SERVICE) ||
		(GetStat().GetStatus() == UNKNOWN_STATUS) ||
		(GetStat().GetStatus() == COMMS_TERMINATED) ||
		(GetStat().GetStatus() == STOPED_BY_OPERATOR) ||
		(GetStat().GetStatus() == STOPPED_BY_CONTROLLER))  //4.0.2.23
		bCanStop = FALSE;
	else
		bCanStop = TRUE;

	return (bCanStop);
}
// 2.9.1.1 //4.0.2.38
void CPump::SetTimeSuppressedDriveoffTimeout(long lPosition, CTime time)
{
	if ((lPosition >= MAX_TRS_IN_LIST ) || (lPosition < 0)) 
	{
		m_cTimeSuppressedDriveoffTimeout[0] = time;
	}
	else
		m_cTimeSuppressedDriveoffTimeout[lPosition] = time;
}
//2.9.1.1
CTime CPump::GetTimeSuppressedDriveoffTimeout(long lPosition)
{
	if ((lPosition >= MAX_TRS_IN_LIST ) || (lPosition < 0)) 
	{
		lPosition = 0;
	}

	return (m_cTimeSuppressedDriveoffTimeout[lPosition]);
}
//4.0.2.38
//2.9.1.3 //4.0.2.38
BOOL CPump::CanReserveForPumpLock(PumpLockType eLockType)
{
	BOOL bCanLock = TRUE;

	int iArraySize = m_cTrsArray.GetSize();
	short nPrePayWaiting = 0,nUnpaid =0;
	bCanLock = CanReserve();

	//4.0.3.27  certain lock types do not care if any prepay
	//transactions are waiting
	if(eLockType == LOCK_TYPE_MAINT)
	{
		if(iArraySize)
		{
			for (int i=0; i<iArraySize; i++)
			{
				long lStat; 
				lStat = m_cTrsArray[i].GetStatus();

				switch(lStat)
				{
					case WAIT_RES:
					case WAIT_AUTH:
						nPrePayWaiting++;
						break;
					case WAIT_PROGRESS:
						if(m_cTrsArray[i].GetLinkFlags() & TRS_LINK_PREPAY_NUMBER)
							nPrePayWaiting++;
						break;

					case IN_PROGRESS:
						break;

					case UNPAID:
					case ACCEPTED:			
						nUnpaid++;
						break;

					case NULL_TRS:				
					case PAID:
						//nFree++;
						break;

					case INCOMPLETE:
						break;
					case WAIT_TIME_OUT:
						nPrePayWaiting++;
						break;

					default:	
						break;
				}
			}
		}

		if (nPrePayWaiting)
		{
			bCanLock = FALSE;	
		}
		//amram 4.0.9.510
		//check if  PriceChangeLockEx is on
		//if so wr will check the unpaid trans
	//	SHIFT_REC  tmp  = _Module.m_server.GetShiftRecData();
	//	if(tmp.bLockShiftDuringProcess == 1)
	//			bCanLock = FALSE;

		if((_Module.m_server.GetPriceChangeLockEx())||
			(_Module.m_server.GetShiftRecData().bLockShiftDuringProcess==1))//4.0.10.505 check unpaid if EOS
			if(nUnpaid)
				bCanLock = FALSE;

	}

	
	return bCanLock;
}

BOOL CModeScheme::YearlyModeScheme()
{
	if( m_lFlags & MODE_SCHEME_YEARLY)
		return TRUE;
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CMemPump Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemPump::CMemPump()
{
	
}

CMemPump::~CMemPump()
{

}

BOOL CMemPump::SetMemPumpInfo(CXMLInterface *pData, CPumpTransact *pTrs, BYTE byIndex)
{
	//rt change
	char sXmlBuff[MAX_XML_BUF_LEN];
	PAY_AT_PUMP_INFO cPapInfo;
	memset(sXmlBuff,0,sizeof(sXmlBuff));
	memset(&cPapInfo,' ',sizeof(cPapInfo));
	pData->GetXmlString(sXmlBuff);						

	_Module.m_server.m_cXmlConvertorEx.ConvertFromXmlToStruct(sXmlBuff,&cPapInfo);
	//rt end change

	return SetMemPumpInfo(&cPapInfo,pTrs,NULL,byIndex); //4.0.30.0 270322

}

BOOL CMemPump::SetMemPumpInfo(PAY_AT_PUMP_INFO *pData, CPumpTransact *pTrs, RECEIPT_INFO *pReceiptInfo /*= NULL */, BYTE byIndex /*=0*/) //4.0.30.0 270322
{
	BOOL bRtc = FALSE;
	CMemPumpInfo Info;

	EnterCriticalSection(&_Module.m_server.m_csMemPumpArray);
	
	Track2Format(pData->CardSaleInfo.cardData.sTrack2Data);
	
	Info.cCardData = pData->CardSaleInfo.cardData;
	Info.cCardSaleExtra = pData->CardSaleInfo.extraData;
	Info.cCardSaleExtra2 = pData->CardSaleExtraData2;	
	Info.byIndex = byIndex;
	memcpy(Info.sReceiptIndex, pData->CardSaleInfo.cardData.sTrack2Data, min(sizeof(Info.sReceiptIndex), sizeof(pData->CardSaleInfo.cardData.sTrack2Data)));	//4.0.26.500 TD 144219
	
	//memcpy( Info.sReceiptIndex, pData->CardSaleExtraData4.sEncryptedAccount, min( sizeof(Info.sReceiptIndex), sizeof(pData->CardSaleExtraData4.sEncryptedAccount)) );	//4.0.26.500 TD 144219
	// save the receipt index
	if(pReceiptInfo)	//4.0.30.0 270322
		memcpy( Info.sReceiptIndex, pReceiptInfo->cReceipt1.Index.sTrack2, min( sizeof(Info.sReceiptIndex), sizeof(pReceiptInfo->cReceipt1.Index.sTrack2)) );	//4.0.26.500 TD 144219

	if (pTrs)
		{
			Info.cTrs2  = *pTrs;
			if (pTrs->m_lNumber == 0)
			{
				CString sMsg;
				sMsg.Format("SetMemPumpInfo, trs.m_lNumber = [%ld], trs.m_lRoundedValue = [%ld], trs.m_lRoundedVolume = [%ld]",pTrs->m_lNumber, pTrs->m_lRoundedValue, pTrs->m_lRoundedVolume);
				_LOGMSG.LogMsg(pTrs->m_lPump,LOG_PUMP,sMsg);
			}
		}
	try
	{
		long lIndex = OperationOnPumpInfo(&Info.cCardData);

		if (lIndex >= 0)  // update Buffer (Record found in buffer we need only Update
		{
			m_cFifoQueue.SetAt(lIndex ,Info);
		}
		else
		{			
			if (m_cFifoQueue.GetSize() <(int) _Module.m_server.m_dwMaxMemCreditDisplay) //MAX_MESSAGES_TO_DISPLAY) 3.3.3.3	
			{
				m_cFifoQueue.InsertAt(0,Info);
			}else
			{
				m_cFifoQueue.RemoveAt(_Module.m_server.m_dwMaxMemCreditDisplay - 1);
				m_cFifoQueue.InsertAt(0,Info);
			}		
			bRtc = TRUE;
		}
		
	}
	catch(...)
	{

		if(LOG_ERROR)
			_LOGMSG.LogMsg(pTrs->m_lPump,LOG_PUMP,"SetMemPumpInfo", LOG_LEVEL_0);

	}

	LeaveCriticalSection(&_Module.m_server.m_csMemPumpArray);
	return bRtc;
}

//4.0.26.500 TD 144219 //4.0.30.0 270322
long CMemPump::LookForInternalReceiptIndex(char* psIndexOut, long lTrsNumber)		
{
	long lRetIndexLen = 0;

	for (int i = 0; i < m_cFifoQueue.GetSize() && lRetIndexLen == 0; i++)
	{
		if(  m_cFifoQueue[i].cTrs2.m_lNumber == lTrsNumber )
		{
			if ( !ChrAll((char*)m_cFifoQueue[i].sReceiptIndex, sizeof(m_cFifoQueue[i].sReceiptIndex), ' ') )
			{
				memcpy(psIndexOut, m_cFifoQueue[i].sReceiptIndex, sizeof(m_cFifoQueue[i].sReceiptIndex));
				lRetIndexLen = sizeof(m_cFifoQueue[i].sReceiptIndex);
			}
			else
			{
				i = m_cFifoQueue.GetSize();
			}
		}
	}

	return lRetIndexLen;
}


BOOL CMemPump::RemoveTrsMemPumpInfo(long lTrsNumber)
{
	BOOL bRtc = FALSE;

	for (int i = 0; i < m_cFifoQueue.GetSize(); i++)
	{
		if (m_cFifoQueue[i].cTrs2.m_lNumber == lTrsNumber)
		{
			m_cFifoQueue.RemoveAt(i);
			bRtc = TRUE;
			break;
		}
	}
	return bRtc;
}

BOOL CMemPump::GetMemPumpInfo(CMemPumpInfo *pInfo, short nIndex)
{
	BOOL bRtc = TRUE;
 
	if (((m_cFifoQueue.GetSize()- 1) < nIndex) ||
		 (nIndex > (short)_Module.m_server.m_dwMaxMemCreditDisplay) ||  //MAX_MESSAGES_TO_DISPLAY) ||3.3.3.3
		 (nIndex < 0))
	{
		bRtc = FALSE;
	}else
	{
		*pInfo = m_cFifoQueue[nIndex];
	}
	return bRtc;
}

long CMemPump::OperationOnPumpInfo(CARD_SALE_DATA *pCardData)
{
	long lIndex =  -1;

	for (int i=0; i < m_cFifoQueue.GetSize() ; i++)
	{
		if(!memcmp(pCardData->sTranNbr, m_cFifoQueue[i].cCardData.sTranNbr , sizeof(pCardData->sTranNbr)))
		{
			lIndex = i;
			break;
		}
	}

	if (lIndex>=0)
	{
		if(LOG_LOGIC_ERROR)
		{
			CString str;
			str.Format("CMemPump::OperationOnPumpInfo Update Memory buffer at position %ld  for seq=%.5s" , lIndex,pCardData->sTranNbr);
			_LOGMSG.LogMsg( str );
		}

	}
	
	return (lIndex);
}

BOOL CModeScheme::IsValidPeriod()
{
	BOOL bActive = FALSE;
	
	SYSTEMTIME sysPeriodStartTime,sysPeriodEndTime,sysTime;
	SYSTEMTIME sysStartTime,sysEndTime;//4.0.3.15
	GetLocalTime(&sysTime);
	CTime	cCurTime(sysTime);
	CTime	tStart = m_cPeriodStartTime;
	CTime	tEnd = m_cPeriodEndTime;
	CTime   tNoPeriod( 2000, 1, 1, 0, 0, 0 ); 
	


	if (YearlyModeScheme())
	{
		m_cStartTime.GetAsSystemTime(sysStartTime);//4.0.3.15
		m_cEndTime.GetAsSystemTime(sysEndTime);//4.0.3.15
		tStart.GetAsSystemTime(sysPeriodStartTime);
		tEnd.GetAsSystemTime(sysPeriodEndTime);

		if (sysTime.wYear != sysPeriodStartTime.wYear)
		{
			sysPeriodEndTime.wYear = sysTime.wYear;			
			sysPeriodStartTime.wYear = sysTime.wYear;			
			
		}
		//4.0.3.15 setting the time as well
		sysPeriodEndTime.wHour = sysEndTime.wHour;
		sysPeriodEndTime.wMinute = sysEndTime.wMinute;
		sysPeriodEndTime.wSecond = sysEndTime.wSecond;

		sysPeriodStartTime.wHour = sysStartTime.wHour;
		sysPeriodStartTime.wMinute = sysStartTime.wMinute;
		sysPeriodStartTime.wSecond = sysStartTime.wSecond;

		CTime	cTmpTime1(sysPeriodEndTime);
		tEnd = cTmpTime1;
		CTime	cTmpTime2(sysPeriodStartTime);
 		tStart = cTmpTime2;
		//4.0.3.15 end
	}

	//4.0.2.40 start
	/*if(((cCurTime >= tStart) &&  (cCurTime <= tEnd)) ||
		((m_cPeriodStartTime == tNoPeriod) && (m_cPeriodEndTime == tNoPeriod ))) // no limit period 
	{
		bActive = TRUE;
	}*/
	if((cCurTime >= tStart) &&  (cCurTime <= tEnd))
	{
		if(DailyModeScheme())
			bActive = InPeriodDailyTime(sysTime);
		else
			bActive = TRUE;
	}
	else if((m_cPeriodStartTime == tNoPeriod) && (m_cPeriodEndTime == tNoPeriod ))
	{
		bActive = TRUE;
	}
	//4.0.2.40 end

	return (bActive);
}

void CModeScheme::SetPeriodStartTime(TAG_SYSTEMTIME *pTagTime)
{
	SYSTEMTIME systime;
	Tag2SystemTime(pTagTime,&systime);
	SetPeriodStartTime(systime);
}

void CModeScheme::SetPeriodStartTime(SYSTEMTIME sysTime)
{
	CTime	cRcvTime(sysTime);
	m_cPeriodStartTime = cRcvTime;
}

void CModeScheme::SetPeriodEndTime(TAG_SYSTEMTIME *pTagTime)
{
	SYSTEMTIME systime;
	Tag2SystemTime(pTagTime,&systime);
	SetPeriodEndTime(systime);
}

void CModeScheme::SetPeriodEndTime(SYSTEMTIME sysTime)
{
	CTime	cRcvTime(sysTime);
	m_cPeriodEndTime = cRcvTime;
}

CTime CModeScheme::GetPeriodStartTime()
{
	return m_cPeriodStartTime;
}

CTime CModeScheme::GetPeriodEndTime()
{
	return m_cPeriodEndTime;
}

CString CModeScheme::GetStringPeriodStartTime()
{
	SYSTEMTIME sysTime;
	SZ_TAG_SYSTEMTIME szTagTime;		

	m_cPeriodStartTime.GetAsSystemTime(sysTime);
	szTagTime.sNull = 0;
	SystemTime2Tag(&sysTime,&szTagTime.sTime);
	CString strVal((char *)&szTagTime);	

	return strVal;
}

CString CModeScheme::GetStringPeriodEndTime()
{
	SYSTEMTIME sysTime;
	SZ_TAG_SYSTEMTIME szTagTime;		

	m_cPeriodEndTime.GetAsSystemTime(sysTime);
	szTagTime.sNull = 0;
	SystemTime2Tag(&sysTime,&szTagTime.sTime);
	CString strVal((char *)&szTagTime);	

	return strVal;
}

void CModeScheme::SetIndex(long lPumpNumber, long lScheme)
{
	m_lNumber = lScheme;
	m_lPump = lPumpNumber;
}

long CModeScheme::GetServiceFeeSet()
{
	
	return m_lServiceFeeSet;
}


// Function name	: CModeScheme::SetServiceFeeSet
// Description	    : 
// Return type		: void 
// Argument         : long lServiceFeeNumber
void CModeScheme::SetServiceFeeSet(long lServiceFeeNumber)
{
	m_lServiceFeeSet = lServiceFeeNumber;
}


//////////////////////////////////////////////////////////////////////
// CServiceFee Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServiceFee::CServiceFee()
{
	m_lFlags = FALSE ;
	m_lAttendService = 0;
	m_lPapService = 0;
	m_lPrePayService= 0;
	m_lAviService= 0;
	m_lPakService =0 ;
	m_lNumber = 0;
}

CServiceFee::~CServiceFee()
{

}

long CServiceFee::LoadDefinition()
{
	long  lRtc;
	DWORD dwTmp;
	
	lRtc = _Module.m_server.m_cParam.GetParam("ServiceFee",m_lNumber,0,"Valid" ,FIELD_DWORD,NULL,0,&dwTmp,FALSE);

	if (!lRtc)
	{
		if (dwTmp)
		{
			m_lFlags = SERVICE_FEE_VALID;	
		}
	}

	if (!lRtc)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("ServiceFee",m_lNumber,0, "PakService",FIELD_DWORD,NULL,0,&dwTmp,FALSE);

		if (!lRtc)
		{
			m_lPakService = (long)dwTmp;
		}
	}
	
	if (!lRtc)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("ServiceFee",m_lNumber,0, "PrePayService",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if (!lRtc)
		{
			m_lPrePayService = (long)dwTmp;
		}
	}

	if (!lRtc)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("ServiceFee",m_lNumber,0, "PapService",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if (!lRtc)
		{
			m_lPapService = (long)dwTmp;
		}
	}

	if (!lRtc)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("ServiceFee",m_lNumber,0, "AviService",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if (!lRtc)
		{
			m_lAviService = (long)dwTmp;
		}
	}	

	if (!lRtc)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("ServiceFee",m_lNumber,0, "AttendService",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if (!lRtc)
		{
			m_lAttendService = (long)dwTmp;
		}
	}	

	if (!lRtc)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("ServiceFee",m_lNumber,0, "SelfPakService",FIELD_DWORD,NULL,0,&dwTmp,FALSE);

		if (!lRtc)
		{
			m_lSelfPakService = (long)dwTmp;
		}
	}
	
	if (!lRtc)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("ServiceFee",m_lNumber,0, "SelfPrePayService",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if (!lRtc)
		{
			m_lSelfPrePayService = (long)dwTmp;
		}
	}

	if (!lRtc)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("ServiceFee",m_lNumber,0, "SelfPapService",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if (!lRtc)
		{
			m_lSelfPapService = (long)dwTmp;
		}
	}

	if (!lRtc)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("ServiceFee",m_lNumber,0, "SelfAviService",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if (!lRtc)
		{
			m_lSelfAviService = (long)dwTmp;
		}
	}	

	if (!lRtc)
	{
		lRtc = _Module.m_server.m_cParam.GetParam("ServiceFee",m_lNumber,0, "SelfAttendService",FIELD_DWORD,NULL,0,&dwTmp,FALSE);
		if (!lRtc)
		{
			m_lSelfAttendService = (long)dwTmp;
		}
	}	

	return lRtc;
}

void CServiceFee::SetNumber(long lNumber)
{
	m_lNumber = lNumber;
}

CServiceFee::CServiceFee(const CServiceFee &ServiceFee)
{
	m_lNumber = ServiceFee.m_lNumber;
	m_lFlags = ServiceFee.m_lFlags;
	m_lPakService = ServiceFee.m_lPakService;
	m_lPrePayService = ServiceFee.m_lPrePayService;
	m_lPapService = ServiceFee.m_lPapService;
	m_lPrePayService = ServiceFee.m_lPrePayService;
	m_lAviService = ServiceFee.m_lAviService;
	m_lAttendService = ServiceFee.m_lAttendService;
	m_sName	 = 	ServiceFee.m_sName;

	m_lSelfPakService = ServiceFee.m_lSelfPakService;
	m_lSelfPrePayService = ServiceFee.m_lSelfPrePayService;
	m_lSelfPapService = ServiceFee.m_lSelfPapService;
	m_lSelfPrePayService = ServiceFee.m_lSelfPrePayService;
	m_lSelfAviService = ServiceFee.m_lSelfAviService;
	m_lSelfAttendService = ServiceFee.m_lSelfAttendService;


	
}

BOOL CServiceFee::ValidServiceFee()
{

	if (m_lFlags & SERVICE_FEE_VALID)
		return TRUE;
	else
		return FALSE;
}

long CServiceFee::GetPakService()
{
	return (m_lPakService);
}

long CServiceFee::GetPrePayService()
{
	return (m_lPrePayService);
}

long CServiceFee::GetPapService()
{
	return  (m_lPapService);
}

long CServiceFee::GetAviService()
{
	return (m_lAviService);
}

long CServiceFee::GetAttendService()
{
	return (m_lAttendService);
}

void CServiceFee::SetPakService(long lPakService)
{
		m_lPakService = lPakService;
}

void CServiceFee::SetPrePayService(long lPrePayService)
{
	m_lPrePayService = lPrePayService;
}

void CServiceFee::SetPapService(long lPapService)
{
	m_lPapService= lPapService;
}

void CServiceFee::SetAviService(long lServiceAvi)
{
	m_lAviService = lServiceAvi;
}

void CServiceFee::SetAttendService(long lServiceAttend)
{
	m_lAttendService = lServiceAttend;

}

void CServiceFee::SetFlagsServiceFee(long lFlags)
{
	m_lFlags = lFlags;
}

long CServiceFee::GetFlagsServiceFee()
{
	return (m_lFlags);
}

long CPump::GetCurrentServiceFee()
{
	return (m_lCurrentServiceFee);
}

void CPump::SetCurentServiceFee(long lFeeNumber)
{
	m_lCurrentServiceFee = lFeeNumber;
}

void CServiceFee::SetName(char *pszName)
{
		m_sName = pszName;
}

void CServiceFee::SetName(CString sName)
{
	m_sName = sName;
}

CString CServiceFee::GetName()
{
	return (m_sName);
}

BOOL CMemPump::SetMemPumpInfo(CMemPumpInfo &Info)
{
	BOOL bRtc = FALSE;

	EnterCriticalSection(&_Module.m_server.m_csMemPumpArray);

	try
	{
		long lIndex = OperationOnPumpInfo(&Info.cCardData);

		if (lIndex >= 0)  // update Buffer (Record found in buffer we need only Update
		{
			m_cFifoQueue.SetAt(lIndex ,Info);
		}
		else
		{
			
			if (m_cFifoQueue.GetSize() <(int) _Module.m_server.m_dwMaxMemCreditDisplay) //MAX_MESSAGES_TO_DISPLAY) 3.3.3.3	
			{
				m_cFifoQueue.InsertAt(0,Info);
			}else
			{
				m_cFifoQueue.RemoveAt(_Module.m_server.m_dwMaxMemCreditDisplay - 1);
				m_cFifoQueue.InsertAt(0,Info);
			}		
			bRtc = TRUE;
		}
		
	}
	catch(...)
	{

		if(LOG_ERROR)
			_LOGMSG.LogMsg("SetMemPumpInfo1", LOG_LEVEL_0);

	}

	LeaveCriticalSection(&_Module.m_server.m_csMemPumpArray);
	return bRtc;
}


//4.0.26.500 TD 144219
long CMemPump::LookForInternalReceiptIndex(char* psIndexOut, const BYTE* szTrsNumIn)		
{
	long lRetIndexLen = 0;

	for (int i = 0; i < m_cFifoQueue.GetSize() && lRetIndexLen == 0; i++)
	{
		if( !memcmp(szTrsNumIn, m_cFifoQueue[i].cCardData.sTranNbr , sizeof(m_cFifoQueue[i].cCardData.sTranNbr)) )
		{
			if ( !ChrAll((char*)m_cFifoQueue[i].sReceiptIndex, sizeof(m_cFifoQueue[i].sReceiptIndex), ' ') )
			{
				memcpy(psIndexOut, m_cFifoQueue[i].sReceiptIndex, sizeof(m_cFifoQueue[i].sReceiptIndex));
				lRetIndexLen = sizeof(m_cFifoQueue[i].sReceiptIndex);
			}
			else
			{
				i = m_cFifoQueue.GetSize();
			}
		}
	}

	return lRetIndexLen;
}


long CPump::GetPrePayInfo(long lSyncNumber,  PRE_PAY_INFO *pInfo) const
{
	CPumpTransact		trs;
	PRE_PAY_INFO		info;
	long lRtc = 0;

	memset(&info,0,sizeof(PRE_PAY_INFO));

	trs.SetLink(TRS_LINK_PREPAY_NUMBER, lSyncNumber);				
	lRtc = _Module.m_server.TrsLinkOperation(REC_READ,&trs,&info);	

	if (lRtc == 0)
	{	
		memcpy(pInfo , &info , sizeof(PRE_PAY_INFO));
		lRtc = 1;		
	}
		

	return lRtc;
}

long CPump::GetNumberOfHotPumpTransactions(long  & lNumberOfHotPumpTrs,long & lNumberOfHotPumpTrsTimeOut)
{
	long lTmpNumberOfHotPumpTrs=0;
	long lTmpNumberOfHotPumpTrsTimeOut =0;
	int iMaxTrs=0;

	//4.0.3.45 - do not check if not hot pumping configured
	if(_Module.m_server.IsHotPumpingActive() || GetDieselPumpFlags())  //4.0.10.45
	{
		iMaxTrs = m_cTrsArray.GetSize();

		for(int i=0; i<iMaxTrs; i++)
		{
			if( (m_cTrsArray[i].GetStatus() == UNPAID) &&
				(m_cTrsArray[i].m_lMisc & MISC_TRS_PAP_CANCELED) &&
				(m_cTrsArray[i].GetOnOffer() == 0))
			{
				if (m_cTrsArray[i].m_lMisc & MISC_UNPAID_TRS_TIMEOUT)
				{
					lTmpNumberOfHotPumpTrsTimeOut++;
				}
				lTmpNumberOfHotPumpTrs++;
			}		
		}

		lNumberOfHotPumpTrsTimeOut = lTmpNumberOfHotPumpTrsTimeOut;
		lNumberOfHotPumpTrs		   = lTmpNumberOfHotPumpTrs;
	}
	return 0;
}

void CPump::SetTagPumpTrsToTrs(TAG_PUMP_TRANSACT2 &tagTrs, CPumpTransact &trs)
{
	CTime cCurTime;

	trs.m_lNumber			= a2l(tagTrs.sNumber		,sizeof(tagTrs.sNumber));
	trs.m_lRoundedVolume	= a2l(tagTrs.sRoundedVolume , sizeof(tagTrs.sRoundedVolume));
	trs.m_lRawVolume        = a2l(tagTrs.sRoundedVolume , sizeof(tagTrs.sRoundedVolume));
	trs.m_lRoundedValue		= a2l(tagTrs.sRoundedValue, sizeof(tagTrs.sRoundedValue));
	trs.m_lGradePrice		= a2l(tagTrs.sGradePrice, sizeof(tagTrs.sGradePrice));
	trs.m_nGrade			= (short)a2l(tagTrs.sGrade, sizeof(tagTrs.sGrade));
	trs.m_lPump				= a2l(tagTrs.sPump, sizeof(tagTrs.sPump));
	trs.m_lMode				= a2l(tagTrs.sMode, sizeof(tagTrs.sMode));
	trs.m_lActiveShiftNumber	= a2l(tagTrs.sActiveShiftNumber , sizeof(tagTrs.sActiveShiftNumber));

	trs.m_lStatus			= UNPAID;
	trs.m_lOnOffer			= 0;
	trs.m_nLevel			= 0;
	//trs.m_nNozzle			= 0;
	trs.m_nNozzle			= a2i(tagTrs.sNozzle, sizeof(tagTrs.sNozzle)); // 4.0.8.505
	trs.m_lRes				= PAY_AT_KIOSK;
	trs.m_lResExt			= NULL_RES_EXT;

	trs.m_lLinkNumber		= 0L;
	trs.m_nLinkFlags		= TRS_NO_LINK;
	trs.m_lActiveReadNumber		= _Module.m_server.GetActiveReadNumber();
	trs.m_lMisc				= MISC_POST_VOID;
	trs.m_lControllerSeqNum = 0;//4.0.18.660 68689 //4.0.21.502

	memset(trs.m_sAttendantID, ' ', sizeof(trs.m_sAttendantID));								//4.0.22.10 TD 71055	//4.0.22.501
	memset(trs.m_sAttendantName, ' ', sizeof(trs.m_sAttendantName));							//4.0.22.10 TD 71055	//4.0.22.501
	memset(trs.m_sFullVehicleId, ' ', sizeof(trs.m_sFullVehicleId));							//4.0.23.500 TD 69914
	memset(trs.m_sAttendantTag, ' ', sizeof(trs.m_sAttendantTag));								//4.0.25.501 TD 137509

	trs.m_sPumpTest = '0';			//4.0.25.60  	TD 231472
	trs.m_lTankID = 0L;				//4.0.25.60  	TD 231475

	trs.m_lMaxValue = 0;								 // 4.0.25.600 CR 296433
	trs.m_lMaxVolume = 0;								 // 4.0.25.600 CR 296433			
	memset(trs.m_sTenderType,' ',sizeof(trs.m_sTenderType)); // 4.0.25.600 CR 296433

	
	memset(trs.m_sFiller, ' ', sizeof(trs.m_sFiller)); //4.0.22.511 72606

	//trs.m_lMisc2 = MISC2_HIDDEN_TRS; //CR 351687: Enhanced post void transactions // TD 431756
	trs.m_lMisc2 = 0;

	trs.m_sTypeOfMeasure = '0'; // TD 363041

}

//TD 38654
void CPump::SetTagPumpTrsToTrs(TAG_PUMP_TRANSACT5 &tagTrs, CPumpTransact &trs)
{
	CTime cCurTime;

	trs.m_lNumber			= a2l(tagTrs.sNumber		,sizeof(tagTrs.sNumber));
	trs.m_lRoundedVolume	= a2l(tagTrs.sRoundedVolume , sizeof(tagTrs.sRoundedVolume));
	trs.m_lRawVolume        = a2l(tagTrs.sRoundedVolume , sizeof(tagTrs.sRoundedVolume));
	trs.m_lRoundedValue		= a2l(tagTrs.sRoundedValue, sizeof(tagTrs.sRoundedValue));
	trs.m_lGradePrice		= a2l(tagTrs.sGradePrice, sizeof(tagTrs.sGradePrice));
	trs.m_nGrade			= (short)a2l(tagTrs.sGrade, sizeof(tagTrs.sGrade));
	trs.m_lPump				= a2l(tagTrs.sPump, sizeof(tagTrs.sPump));
	trs.m_lMode				= a2l(tagTrs.sMode, sizeof(tagTrs.sMode));
	trs.m_lActiveShiftNumber	= a2l(tagTrs.sActiveShiftNumber , sizeof(tagTrs.sActiveShiftNumber));

	trs.m_lStatus			= UNPAID;
	trs.m_lOnOffer			= 0;
	trs.m_nLevel			= 0;
	//trs.m_nNozzle			= 0;
	trs.m_nNozzle			= a2i(tagTrs.sNozzle, sizeof(tagTrs.sNozzle)); // 4.0.8.505
	trs.m_lRes				= PAY_AT_KIOSK;
	trs.m_lResExt			= NULL_RES_EXT;

	trs.m_lLinkNumber		= 0L;
	trs.m_nLinkFlags		= TRS_NO_LINK;
	trs.m_lActiveReadNumber		= _Module.m_server.GetActiveReadNumber();
	trs.m_lMisc				= MISC_POST_VOID;
	trs.m_lControllerSeqNum = a2l(tagTrs.sControllerSeqNum, sizeof(tagTrs.sControllerSeqNum));	//4.0.18.660 68689 //4.0.21.502
	memset(trs.m_sAttendantID, ' ', sizeof(trs.m_sAttendantID));								//4.0.22.10 TD 71055	//4.0.22.501
	memset(trs.m_sAttendantName, ' ', sizeof(trs.m_sAttendantName));							//4.0.22.10 TD 71055	//4.0.22.501
	memset(trs.m_sFullVehicleId, ' ', sizeof(trs.m_sFullVehicleId));							//4.0.23.500 TD 69914
	memset(trs.m_sAttendantTag, ' ', sizeof(trs.m_sAttendantTag));								//4.0.25.501 TD 137509

	
	memset(trs.m_sFiller, ' ', sizeof(trs.m_sFiller)); //4.0.22.511 72606
	memcpy(trs.m_sAttendantID, tagTrs.sAttendantID, sizeof(tagTrs.sAttendantID)); //4.0.26.0 - No TD
	memcpy(trs.m_sAttendantTag, tagTrs.sAttendantTag, sizeof(tagTrs.sAttendantTag)); //4.0.26.0 - No TD

	trs.m_sPumpTest = '0';			//4.0.25.60  	TD 231472
	trs.m_lTankID = 0L;				//4.0.25.60  	TD 231475

	trs.m_lMaxValue = 0;								 // 4.0.25.600 CR 296433
	trs.m_lMaxVolume = 0;								 // 4.0.25.600 CR 296433			
	memset(trs.m_sTenderType,' ',sizeof(trs.m_sTenderType)); // 4.0.25.600 CR 296433

	trs.m_lMisc2 = a2l(tagTrs.sMisc2, sizeof(tagTrs.sMisc2));//= MISC2_HIDDEN_TRS; //CR 351687: Enhanced post void transactions
	trs.m_sTypeOfMeasure = '0'; // TD 363041

	trs.m_lLowestGradePrice = a2l(tagTrs.sLowestGradePrice, sizeof(tagTrs.sLowestGradePrice));
	memcpy(&trs.m_sLoyaltyPriceProtection, &tagTrs.sLoyaltyPriceProtection, sizeof(tagTrs.sLoyaltyPriceProtection));
	memcpy(&trs.m_sLoyaltyAutoArm, &tagTrs.sLoyaltyAutoArm, sizeof(tagTrs.sLoyaltyAutoArm));
	memcpy(&trs.m_sMaxDiscountAllowed, &tagTrs.sMaxDiscountAllowed, sizeof(tagTrs.sMaxDiscountAllowed)); //RFUEL-3309 Get the maximal discount allowed for the grade 

}


long CPump::IsAllowPrePayOnRequestingPump()
{

	 return (m_dwAllowPrePayOnRequestingPump);
}

long CPump::GetUnBalancePrepayCounter()
{
	return (m_dwUnBalancePrePayCounter); 
}
//4.0.12.502
long CPump::GetDeclinePrepayCounter()
{
	return (m_dwDeclinePrePayCounter); 
}

void CPump::IncreaseUnBalancePrePayCounter()
{
	CString str("Pumps\\Pump");
	char sNumber[8];
	memset(sNumber,0,sizeof(sNumber));
	sprintf(sNumber,"%02d",m_lNumber);
	str += sNumber;			

	m_dwUnBalancePrePayCounter++;
	//sending the event via MultiSync with the pump number as client data.
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str),"UnbalancePrePayCounter" , m_dwUnBalancePrePayCounter,FALSE,_HOST.m_lGroupNum,0,m_lNumber,TRUE);//4.0.3.27	 //4.0.8.505
	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;	//4.0.21.500 69053
		str.Format("Unbalanced Pre Pay counter was increased");
		_LOGMSG.LogMsg(m_lNumber,LOG_PUMP,str);
	}

}

void CPump::DecreaseUnBalancePrePayCounter()
{
	CString str("Pumps\\Pump");
	char sNumber[8];
	memset(sNumber,0,sizeof(sNumber));
	sprintf(sNumber,"%02d",m_lNumber);
	str += sNumber;			

	if(m_dwUnBalancePrePayCounter)
		m_dwUnBalancePrePayCounter--;

	//4.0.3.21  this check is incorrect, since m_dwUnBalancePrePayCounter is
	//an unsigned long, and will never be negative
	/*
	if (m_dwUnBalancePrePayCounter < 0)
		m_dwUnBalancePrePayCounter = 0;
	*/

	//sending the event via MultiSync with the pump number as client data.
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str),"UnbalancePrePayCounter",m_dwUnBalancePrePayCounter,FALSE,_HOST.m_lGroupNum,0,m_lNumber,TRUE);//4.0.3.27 //4.0.8.505
	if(LOG_BASIC_FLOW_CONTROL)
	{	
		CString str;	//4.0.21.500 69053
		str.Format("Unbalanced Pre Pay counter was decreased");
		_LOGMSG.LogMsg(m_lNumber,LOG_PUMP,str);
	}
}

void CServiceFee::SetSelfPakService(long lSelfPakService)
{
	m_lSelfPakService = lSelfPakService;
}

long CServiceFee::GetSelfPakService()
{
	return (m_lSelfPakService);
}

void CServiceFee::SetSelfPapService(long lSelfPapService)
{
	m_lSelfPapService = lSelfPapService;
}

long CServiceFee::GetSelfPapService()
{
	return (m_lSelfPapService);
}

void CServiceFee::SetSelfPrePayService(long lSelfPrePayService)
{
	m_lSelfPrePayService = lSelfPrePayService;
}

long CServiceFee::GetSelfPrePayService()
{
	return (m_lSelfPrePayService);
}

void CServiceFee::SetSelfAviService(long lSelfAviService)
{
	m_lSelfAviService = lSelfAviService;
}

long CServiceFee::GetSelfAviService()
{
	return (m_lSelfAviService);
}

void CServiceFee::SetSelfAttendService(long lSelfAttendService)
{
	m_lSelfAttendService = lSelfAttendService;
}

long CServiceFee::GetSelfAttendService()
{
	return (m_lSelfAttendService);
}

BOOL CPump::IsLoyaltyClub()
{	
	return ((m_cPAPInfo.CardSaleInfo.cardData.sTranType == CD_TYPE_LOYALTY)? TRUE: FALSE);
}

/******************************************************************************
 Description:	Check if trs has extra info saved from OLAin Ash2000 qdx
 Returns:      	TRUE/FALSE
 Parameters:   	long lPumpNumber, 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			09/06/2010   16:08		Start //4.0.24.140
******************************************************************************/
BOOL CPump::IsOLAExtraInfoTrs()
{	
	long lTmp = a2l(m_cPAPInfo.CardSaleExtraData3.sFlags1, sizeof(m_cPAPInfo.CardSaleExtraData3.sFlags1));
	return (((lTmp & FL_OLA_EXTRA_DATA) > 0)? TRUE: FALSE);
}

BOOL CPump::IsSavedDataInAsh2000Qdx()	//118124 4.0.25.50
{	
	long lTmp = a2l(m_cPAPInfo.CardSaleExtraData3.sFlags1, sizeof(m_cPAPInfo.CardSaleExtraData3.sFlags1));
	return (((lTmp & FL_OLA_EXTRA_DATA) > 0)? TRUE: FALSE);
}
/******************************************************************************
 Description:	Check if pump should be auto authorized by loyalty	
 Returns:      	TRUE/FALSE
 Parameters:   	long lPumpNumber, 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			24/11/2009   15:24		Start //4.0.23.508 89149
******************************************************************************/
BOOL CPump::IsLoyaltyAutoStartPump()	
{
	long lAutoStartPump = a2l(&m_cPAPInfo.CardSaleExtraData5.sPumpAutoStart, sizeof(m_cPAPInfo.CardSaleExtraData5.sPumpAutoStart));
	return (lAutoStartPump? TRUE:FALSE);
}

/******************************************************************************
 Description:	Check if expected a complete session on PAK with zero val/vol	
 Returns:      	TRUE/FALSE
 Parameters:   	long lPumpNumber, 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			07/09/2008   16:32		Start
******************************************************************************/
BOOL CPump::IsForceZeroPakCompletionOn() //4.0.14.1390 67387 //4.0.21.0
{
	return ((m_cPAPInfo.CardSaleExtraData4.sForceZeroPAKCompletion == 'Y')? TRUE: FALSE);
}

/******************************************************************************
 Description:	Check if expected a complete session on PAP with zero val/vol	
 Returns:      	TRUE/FALSE
 Parameters:   	long lPumpNumber, 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			24/06/2010   10:45		Start
 OmerT          Support Suncor force compilation  4.0.32.421
 OmerT          If payment card was declined zero complete in case FSR was authorized.
******************************************************************************/
BOOL CPump::IsZeroCompletionNeededForPAP() //4.0.24.140 130693 
{
	if (m_cPumpStat.GetRes() == PAY_AT_PUMP && m_cPAPInfo.CardSaleInfo.extraData.sForceCompleteZero == 'Y')
	{
		_LOGMSG.LogMsg(m_lNumber,LOG_PUMP,"Zero completion is needed for trs");
		return TRUE;
	}

	return FALSE;
}
/******************************************************************************
 Description:	turn on a flag indicating a complete session on PAK with zero val/vol	
 Returns:      	TRUE/FALSE
 Parameters:   	long lPumpNumber, 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			07/09/2008   16:32		Start
******************************************************************************/
void CPump::SetInPAKLoyaltyWithComplete(BOOL bVal) //4.0.14.1390 67387 //4.0.21.0
{
	if (bVal)
		l2a(1, &m_cPAPInfo.CardSaleExtraData4.sInPAKLoyaltyWithComplete, sizeof(m_cPAPInfo.CardSaleExtraData4.sInPAKLoyaltyWithComplete));
	else
		l2a(0, &m_cPAPInfo.CardSaleExtraData4.sInPAKLoyaltyWithComplete, sizeof(m_cPAPInfo.CardSaleExtraData4.sInPAKLoyaltyWithComplete));
}

BOOL CPump::GetInPAKLoyaltyWithComplete() //4.0.14.1390 67387 //4.0.21.0
{
	long lVal = a2l(&m_cPAPInfo.CardSaleExtraData4.sInPAKLoyaltyWithComplete, sizeof(m_cPAPInfo.CardSaleExtraData4.sInPAKLoyaltyWithComplete));
	if (lVal)
		return TRUE; 
	return FALSE; 
}

void CPump::ClearPAKLoyaltyWithCompleteFlags() //4.0.14.1390 67387 //4.0.21.0
{
	l2a(0, &m_cPAPInfo.CardSaleExtraData4.sInPAKLoyaltyWithComplete, sizeof(m_cPAPInfo.CardSaleExtraData4.sInPAKLoyaltyWithComplete));
	l2a(0, &m_cPAPInfo.CardSaleExtraData4.sForceZeroPAKCompletion, sizeof(m_cPAPInfo.CardSaleExtraData4.sForceZeroPAKCompletion));
	_LOGMSG.LogMsg(m_lNumber,LOG_PUMP,"PAKLoyaltyWithComplete flags were cleared");

}

long CPump::GetLatestPrePaySyncNumber()
{
	return (m_lLatestPrePaySyncNumber);
}

void CPump::SetLatestPrePaySyncNumber(long lSyncNumber)
{
	m_lLatestPrePaySyncNumber =lSyncNumber;
}

long CPump::GetLatestPrePayPosNumber()
{
	return (m_lLatestPrePayPosNumber);
}

void CPump::SetLatestPrePayPosNumber(long lPosNumber)
{
	m_lLatestPrePayPosNumber = lPosNumber;
}

/******************************************************************************
 Description:	Get the number of the Attendant (UNPAID or ACCEPTED) transactions on the pump
 Returns:      	
 Parameters:   	turns zero if any attendant trs found
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			16/02/2009   14:40		Start  //4.0.22.501 merged from 4.0.22.12 TD 73062
******************************************************************************/
int CPump::GetAttendTrs(long lFlags, CPumpTransact *pTrs, long & lNumberOfTrs)
{
	int iRetNum = 1; // not found	
	int iArraySize = m_cTrsArray.GetSize();
	BOOL bFound = FALSE;
	long lCounter = 0;

	for (int i=0; i<iArraySize; i++)
	{		
		if((m_cTrsArray[i].m_lResExt == RES_EXT_ATTENDANT) && 
			((m_cTrsArray[i].GetStatus() == UNPAID) || 
			(m_cTrsArray[i].GetStatus() == ACCEPTED)))
		{
			iRetNum = 0; // found
			lCounter ++;
			if((pTrs != NULL) && (bFound == FALSE))
			{	
				bFound = TRUE;
				*pTrs = m_cTrsArray[i];
			}
		}
	}
	
	lNumberOfTrs = lCounter;
	return iRetNum;
}

//3.2.1.46
BOOL CPump::IsExistTransactionOnOffer()
{
	BOOL bFound =FALSE; 
	int iMaxTrs=0;


	iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{
		if( (m_cTrsArray[i].GetStatus() == UNPAID) &&
			(m_cTrsArray[i].GetOnOffer()))
		{
			break;
			bFound = TRUE;
		}			
	}
	return bFound;
}
//3.2.1.46
//4.0.1.0
BOOL CPump::IsPumpIdle()
{
	if ((m_cPumpStat.GetRes() == NULL_RES ) && 
	    (m_cPumpStat.GetResExt() ==NULL_RES_EXT) &&  
		(m_cPumpStat.GetStatus() == READY))
		return TRUE;
	else
		return FALSE;
}
//4.0.1.0

//4.0.1.31
BOOL CPump::FindMatchingTimeOutPrePay()
{
	BOOL bFound = FALSE;
	int iMaxTrsIndex = m_cTrsArray.GetSize();

	for(int i = 0;i < iMaxTrsIndex;i++)
	{
		if((m_cTrsArray[i].m_lMisc & MISC_VIT_CONFLICT) &&
		   (m_cTrsArray[i].m_lMisc & MISC_PREPAY_TIMED_OUT))
		{
			bFound = TRUE;
			break;
		}
	}

	return bFound;
}


/*@@@*/
//Roni Start(version 4.0.9.800)
void CPump::SetPriceLevel(long lPriceLevel)
{
	m_lPriceLevel = lPriceLevel;
}

long CPump::GetPriceLevel()
{
	return m_lPriceLevel;
}

void CPump::SetModeService(long lModeService)
{
	m_lModeService = lModeService;
}

long CPump::GetModeService()
{
	return m_lModeService;
}
//Roni End
/*@@@*/

//4.0.2.21
void CPump::SetModeSchemeServiceMode(long lServiceMode)
{
	m_lCurrentServiceMode = lServiceMode;
}




long CPump::GetModeSchemeServiceMode()
{
	return m_lCurrentServiceMode;
}

//4.0.2.21

long CModeScheme::GetPumpServiceMode()
{
	return m_lServiceMode;
}




//4.0.2.23
GPIRetCode CPumpTransact::SetStateOnOffer(long lRequestSource, long lFlags) //4.0.19.830 - TD 89646
{
	GPIRetCode lRetVal = GPI_OK;

	CString sLogMsg;

	if(PUMPSRV_IS_MASTER)
	{
		if(m_lStatus == UNPAID)
		{
			if(m_lOnOffer == 0)   //Transaction not on offer
			{					
				if ((_Module.m_server.GetRegion() == REGION_UK) && (!_Module.m_server.IsAllowedMultiPosOwnersOfPumpTrs())) //4.0.23.501 78146
				{
					//pump currently on offer at another pos.
					if(_Module.m_server.m_cPumpArray[m_lPump - 1].AnyTrsOnOffer(*this))	
						lRetVal = GPI_ONE_OF_PUMP_TRS_ON_OFFER;
				}
				
				if(lRetVal == GPI_OK)
				{
					m_lOnOffer = lRequestSource;												
					
					_Module.m_server.CentralUpdateTrs(m_lPump,
													  m_lNumber,
													  this,
													  FALSE);
					
					lRetVal = GPI_OK;
				}
			}

			//Transaction already on offer at this POS
			else if(m_lOnOffer == lRequestSource)  
			{
				//4.0.23.508 - TD 89646
				if (lFlags & SET_STATE_FROM_EXT_TERMINAL)
					lRetVal = GPI_TRS_LOCKED;
				else
					lRetVal = GPI_OK;
			}

			//Transaction already on offer at a different POS
			else
			{
				lRetVal = GPI_TRS_LOCKED;
			}
		}
		
		else if(m_lStatus == ACCEPTED)
		{
			if(m_lOnOffer == lRequestSource)
				lRetVal = GPI_OK;
		
			else
				lRetVal = GPI_TRS_LOCKED;
		}	

		else 
			lRetVal = GPI_TRS_NOT_FOR_SALE;
	}

	else
	{
		CMD_STRUCT_TRS cTRSCmd;
		cTRSCmd.lTrsNumber = m_lNumber;

		_Module.m_server.SetCommand(CMD_TRS_OFFER,
			                        lRequestSource,
									m_lPump,
									&cTRSCmd,
									sizeof(cTRSCmd));

		lRetVal = GPI_OK;
	}
	
	if(LOG_BASIC_FLOW_CONTROL)
	{
		sLogMsg.Format("CPumpTransact::SetStateOnOffer() - trs # %d, POS # %d, returned %d" , m_lNumber , lRequestSource , lRetVal);
		_LOGMSG.LogMsg(m_lPump,LOG_PUMP,sLogMsg);
	}

	return lRetVal;
}


GPIRetCode CPumpTransact::SetStateRelease(long lRequestSource)
{
	GPIRetCode lRetVal = GPI_OK;

	CString sLogMsg;

	if(PUMPSRV_IS_MASTER)
	{
		//If transaction is ON_OFFER at lRequestSource
		if(m_lOnOffer == lRequestSource)
		{								
			m_lOnOffer = 0;
			m_lStatus = UNPAID;											
		
			_Module.m_server.CentralUpdateTrs(m_lPump,
											  m_lNumber,
											  this,
											  FALSE);

			lRetVal = GPI_OK;											
		}
		
		else if(!m_lOnOffer)
			lRetVal = GPI_OK;											
			
		else
			lRetVal = GPI_TRS_LOCKED;
	}
	
	else
	{
		CMD_STRUCT_TRS cTRSCmd;
		cTRSCmd.lTrsNumber = m_lNumber;

		_Module.m_server.SetCommand(CMD_TRS_RELEASE,
			                        lRequestSource,
									m_lPump,
									&cTRSCmd,
									sizeof(cTRSCmd));

		lRetVal = GPI_OK;
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{
		sLogMsg.Format("CPumpTransact::SetStateRelease() - trs # %d, POS # %d, returned %d" , m_lNumber , lRequestSource , lRetVal);
		_LOGMSG.LogMsg(m_lPump,LOG_PUMP,sLogMsg);
	}
		
	return lRetVal;
}

GPIRetCode CPumpTransact::SetStateForcedRelease(long lRequestSource)
{
	GPIRetCode lRetVal = GPI_OK;

	CString sLogMsg;

	if(PUMPSRV_IS_MASTER)
	{
		if((m_lOnOffer) || (m_lStatus == ACCEPTED))
		{
			m_lOnOffer = 0;
			m_lStatus = UNPAID;
			
			_Module.m_server.CentralUpdateTrs(m_lPump,
											  m_lNumber,
											  this,
											  FALSE);

			lRetVal = GPI_OK;
		}
		
		else
			lRetVal = GPI_OK;	
	}
	
	else
	{
		CMD_STRUCT_TRS cTRSCmd;
		cTRSCmd.lTrsNumber = m_lNumber;

		_Module.m_server.SetCommand(CMD_TRS_FORCE_RELEASE,
			                        lRequestSource,
									m_lPump,
									&cTRSCmd,
									sizeof(cTRSCmd));

		lRetVal = GPI_OK;
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{
		sLogMsg.Format("CPumpTransact::SetStateForcedRelease() - trs # %d, POS # %d, returned %d" , m_lNumber , lRequestSource , lRetVal);
		_LOGMSG.LogMsg(m_lPump,LOG_PUMP,sLogMsg);
	}
	
	return lRetVal;
}



GPIRetCode CPumpTransact::SetStateAccepted(long lRequestSource)
{
	GPIRetCode lRetVal = GPI_OK;

	CString sLogMsg;

	if(PUMPSRV_IS_MASTER)
	{
		if(m_lStatus == UNPAID)
		{
			if(m_lOnOffer == lRequestSource)
			{	
				m_lStatus = ACCEPTED;
				
				_Module.m_server.CentralUpdateTrs(m_lPump,
												  m_lNumber,
												  this,
												  FALSE);

				lRetVal = GPI_OK;
			}
			
			else if(m_lOnOffer == 0)
				lRetVal = GPI_TRS_NOT_RESERVED;
			
			else
				lRetVal = GPI_TRS_LOCKED;
		}
		
		//If transaction has already been accepted
		else if(m_lStatus == ACCEPTED)
		{
			//Another attempt from the same POS
			if(m_lOnOffer == lRequestSource) 
				lRetVal = GPI_OK;
		
			//Tranasction not on offer
			else if(!m_lOnOffer)
				lRetVal = GPI_TRS_NOT_RESERVED;
			
			//Transaction on offer at a different POS
			else
				lRetVal = GPI_TRS_LOCKED;
		}	
		
		else 
			lRetVal = GPI_TRS_NOT_FOR_SALE;
	}

	else
	{
		CMD_STRUCT_TRS cTRSCmd;
		cTRSCmd.lTrsNumber = m_lNumber;

		//4.0.2.23
		// if trs was accepted by controller do not send it again, 
		// improving performance
		if (m_lStatus != ACCEPTED)  
			_Module.m_server.SetCommand(CMD_TRS_ACCEPTED,
			                        lRequestSource,
									m_lPump,
									&cTRSCmd,
									sizeof(cTRSCmd));

		lRetVal = GPI_OK;
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{
		sLogMsg.Format("CPumpTransact::SetStateAccepted() - trs # %d, POS # %d, returned %d" , m_lNumber , lRequestSource , lRetVal);
		_LOGMSG.LogMsg(m_lPump,LOG_PUMP,sLogMsg);
	}

	return lRetVal;
}


GPIRetCode CPumpTransact::SetStatePaid(long lRequestSource, long lFlags, BSTR bstrData) //4.0.22.501 - TD 69904
{
	long lQDXRetCode = OK;
	GPIRetCode lRetVal = GPI_OK;
	BOOL bSendCommandToController = FALSE;  

	CString sLogMsg;
	
	if(PUMPSRV_IS_MASTER)
	{
		if ((PUMPSRV_REPORT_ON_TRS_PAID) && (m_lStatus == ACCEPTED))  //4.0.5.33
		{
			bSendCommandToController= TRUE;

		}
		else if(m_lStatus == ACCEPTED)
		{
			if(m_lOnOffer != lRequestSource)
			{	
				//Payment request is not from the same POS
				//that accepted the transaction
				if(LOG_DETAIL_FLOW_CONTROL)
				{
					sLogMsg.Format("CPumpTRansact::SetStatePaid() - Payment done from different POS ! (Pump %02d, Trs %d, Status %d, OnOffer at %d, Paid at %d, link Number %d", 
								   m_lPump, 
								   m_lNumber,
								   m_lStatus,
								   m_lOnOffer,
								   lRequestSource,
								   m_lLinkNumber );

					_LOGMSG.LogMsg(m_lPump,LOG_PUMP,sLogMsg); 
				}	
		
				//Update the number of the POS handling the transaction
				m_lOnOffer = lRequestSource;
			}

			m_lStatus = PAID;

			if (lFlags & TRS_PUMP_TEST)				//4.0.26.503 TD 108311
			{
				if (_Module.m_server.GetSingleShiftAccumulationsActive())
				{
					_Module.m_server.m_cSingleShiftAccumMgr.AccumulatePumpTest(m_lPump, m_nNozzle, m_lRoundedVolume);
				}
			}
			
			_Module.m_server.CentralUpdateTrs(m_lPump,
											  m_lNumber,
											  this,
											  FALSE);

								
			if (TrsShouldBeReportedPaid()) //4.0.23.650 133966 //4.0.25.50
			{
				bSendCommandToController= TRUE;
			}
			else if (m_nLinkFlags & TRS_LINK_PAK_AND_LOYALTY)
			{
				//Delete record from ExtarInfo
				lQDXRetCode = _Module.m_server.TrsLinkOperation(REC_DELETE , this , NULL);

				if (lQDXRetCode)
				{
					sLogMsg.Format("CPumpTransact::SetStatePaid() - Failure!!! Attempted to delete record=%ld form ExtaInfo Rtc=%d" , m_lNumber , lQDXRetCode);
					_LOGMSG.LogMsg(m_lPump,LOG_PUMP,sLogMsg);
				}
				CPumpTransact cTmpTrs;
				cTmpTrs = *this;
				_Module.m_server.CheckVlidityTransactionNumber(cTmpTrs.m_lNumber);  //4.0.24.100 118124
			}
			
			else if (m_lResExt == RES_EXT_ATTENDANT)
			{
				//Make a copy of the current transaction
				CPumpTransact cTmpTrs;
				cTmpTrs = *this;

				//Set the copy's link to TRS_LINK_ATTEND
				cTmpTrs.SetLink(TRS_LINK_ATTEND,0);

				lQDXRetCode = _Module.m_server.TrsLinkOperation(REC_DELETE , &cTmpTrs, NULL);

				if (lQDXRetCode)
				{
					sLogMsg.Format("CPumpTransact::SetStatePaid() - Failure!!!  Attempted to delete attendant record=%ld form ExtaInfo Rtc=%d" , cTmpTrs.m_lNumber , lQDXRetCode);	
					_LOGMSG.LogMsg(m_lPump,LOG_PUMP,sLogMsg);								
				}							

			}
			else if (m_lResExt == RES_EXT_FLEET)  //4.0.10.14
			{
				PAY_AT_PUMP_INFO cPatAtPumpInfo;
				TRAVEL_CENTER_REC cTravelCenterRec;  //4.0.9999.
				CPumpTransact cTmpTrs;
				cTmpTrs = *this;

				lQDXRetCode = _Module.m_server.IndexOperation(PUMP_OLA_TEMP_FILE , REC_READ, &cTmpTrs ,&cPatAtPumpInfo); //4.0.10.49

				cTravelCenterRec.lIndex = a2l(cPatAtPumpInfo.CardSaleInfo.cardData.sTranNbr, sizeof(cPatAtPumpInfo.CardSaleInfo.cardData.sTranNbr)); //4.0.10.49

				if ((LOG_BASIC_FLOW_CONTROL))
				{
					sLogMsg.Format("Find index  Index=%ld,retcode=%ld" ,cTravelCenterRec.lIndex ,lQDXRetCode);
					_LOGMSG.LogMsg(sLogMsg);
				}

				lQDXRetCode = _Module.m_server.IndexOperation(PUMP_OLA_TEMP_FILE , REC_DELETE, &cTmpTrs ,&cPatAtPumpInfo);

				if ((LOG_BASIC_FLOW_CONTROL) && (lQDXRetCode))
				{
					sLogMsg.Format("fail delete tmp ola file Index=%ld,retcode=%ld" ,cTmpTrs.m_lNumber ,lQDXRetCode);
					_LOGMSG.LogMsg(sLogMsg);
				}

				PS::RECEIPT_EXTRA_INFO_REC rec;
				memcpy(&rec, &cTravelCenterRec, min(sizeof(rec), sizeof(cTravelCenterRec)));
				lQDXRetCode = CSQLPumpSrvReceiptExtraInfoDB::GetInstance().OperationEx(REC_DELETE, rec);
				//lQDXRetCode = _Module.m_server.IndexOperation(RECEIPT_EXTRA_DATA_FILE,REC_DELETE,&cTravelCenterRec);


				if ((LOG_BASIC_FLOW_CONTROL) && (lQDXRetCode))
				{
					sLogMsg.Format("fail delete receit extra data file Index=%ld, retcode=%ld" ,cTmpTrs.m_lNumber , lQDXRetCode);
					_LOGMSG.LogMsg(sLogMsg);
				}
			}
			else 
			{
				CPumpTransact cTmpTrs =  *this;
				_Module.m_server.CheckVlidityTransactionNumber(cTmpTrs.m_lNumber);  //4.0.11.410
			}

			
			lRetVal = GPI_OK;											
		}
		
		else if(m_lStatus == UNPAID)
		{
			if(m_lOnOffer)
			{
				if(m_lOnOffer == lRequestSource)
					lRetVal = GPI_TRS_NOT_ACCEPTED;

				else
					lRetVal = GPI_TRS_LOCKED;
			} 											
			
			else
				lRetVal = GPI_TRS_NOT_RESERVED;
		}	
		
		else 
			lRetVal = GPI_TRS_NOT_FOR_SALE;
	}
	else
	{
		bSendCommandToController = TRUE;
	}

	// 4.0.5.33
	if (bSendCommandToController)
	{
		CMD_STRUCT_TRS cTRSCmd;
		cTRSCmd.lTrsNumber = m_lNumber;
		cTRSCmd.lFlags = 0;
		
		//4.0.22.501 - TD 69904
		if (lFlags & TRS_PUMP_TEST)
		{
			cTRSCmd.lFlags = TRS_PUMP_TEST;
			
			// Copy the data buffer
			//4.0.23.503 TD 78063
			_bstr_t bstrtTmp(bstrData, false);
			cTRSCmd.sData = bstrtTmp.copy(); 
		}

		_Module.m_server.SetCommand(CMD_TRS_PAID,
			                        lRequestSource,
									m_lPump,
									&cTRSCmd,
									sizeof(cTRSCmd));


		if(LOG_DETAIL_FLOW_CONTROL)
		{
			sLogMsg.Format("CPumpTransact::SetStatePaid() -  Send cmd to controller.  (Pump %02d, Trs %d, Status %d, OnOffer at %d, Paid at %d, link Number %d", 
						   m_lPump, 
						   m_lNumber,
						   m_lStatus,
						   m_lOnOffer,
						   lRequestSource,
						   m_lLinkNumber );

			_LOGMSG.LogMsg(m_lPump,LOG_PUMP,sLogMsg); 
		}	



		lRetVal = GPI_OK;
	}

	if(LOG_BASIC_FLOW_CONTROL)
	{
		sLogMsg.Format("CPumpTransact::SetStatePaid() - trs # %d, POS # %d, returned %d" , m_lNumber , lRequestSource , lRetVal);
		_LOGMSG.LogMsg(m_lPump,LOG_PUMP,sLogMsg);
	}

	return lRetVal;
}
//4.0.2.23
//4.0.2.23
void CModeScheme::SetServiceMode(long lFlags)
{
	m_lServiceMode = 0;

	if (m_lFlags & MODE_SCHEME_FULL_SERVICE_VALID)
	{
		m_lServiceMode = MODE_SHCEME_FULL_SERVICE;
	}
	else if (m_lFlags & MODE_SCHEME_SELF_SERVICE_VALID)
	{
		m_lServiceMode =MODE_SHCEME_SELF_SERVICE;
	} 
	else
	{
		m_lServiceMode= 0;
	}
}

long CModeScheme::GetServiceMode()
{
	return m_lServiceMode;
}
//4.0.2.23

/******************************************************************************
 Description:	Find trs poition in the buffer 	
 Returns:      	Poistion number
 Parameters:   	long lTrsNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			5/3/2003   15:51		Start
******************************************************************************/
//4.0.2.38
long CPump::GetIndexNumber(long lTrsNumber)
{	
	int iRetIndex = 0; 
	int iArraySize = m_cTrsArray.GetSize();
		
	if((iArraySize)&&(lTrsNumber != 0))
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].m_lNumber == lTrsNumber)
			{
				iRetIndex = i; 
				break;
			}
		}
	}

	return iRetIndex;
}
//4.0.2.38
//4.0.3.27
void CPump::SetUnBalancePrepayCounter(DWORD dwUnBalancePrepayCounter)
{
	m_dwUnBalancePrePayCounter = dwUnBalancePrepayCounter;
}

BOOL CPump::IsUpdatePrepayAllow()
{
	BOOL bUpdateAllow = TRUE;
	short nPrePayWaiting,nTotalTrsWaiting;
	CPumpTransact currentTrs;
	BOOL bCurrent = TRUE;
	
	bCurrent = !GetInProgressWaitProgressTrs(&currentTrs);
	nPrePayWaiting = GetNumberOfNonClosedPrePay();
	
	nTotalTrsWaiting = nPrePayWaiting;
	//m_nMaxPrePayAllowed = _Module.m_server.m_cPumpArray[p].GetMaxPrePayTrsAllowed();
	if( m_dwMaxPrePayTrsAllowed == 0)
	{
		if( IsCurrentTrs() )
			bUpdateAllow = FALSE;		
	}
	if ( (!GetInProgressWaitProgressTrs(&currentTrs)) ||
			   ( (GetStat().GetStatus() == REQUESTING) && (!m_dwAllowPrePayOnRequestingPump)) )
	{
		nTotalTrsWaiting++;
	}

	if( nTotalTrsWaiting > (short)m_dwMaxPrePayTrsAllowed )
		bUpdateAllow = FALSE;

	return bUpdateAllow;
}


BOOL CMode::CashAcceptorAuth()   //4.0.3.60
{
	if (m_lFlags &MODE_DEF_CASH_ACCEPTOR_AUTH)
		return TRUE;
	else
		return FALSE;

}

/******************************************************************************
 Description:  Get the definition of pump data =, is it regular  pump data or is it terminal data 
 Returns:		0 - Regular Pump
				1 - Terminal data
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			18/9/2003   0:19		Start
******************************************************************************/
DWORD CPump::IsPumpDefineAsTerminal()
{
	return m_dwPumpAsTerminal;	
}


/******************************************************************************
 Description:   Determine if a login to the pump should be performed
 Returns:		0 - No
				1 - Yes
 Parameters:   	PumpStatus lPumpStatus
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi			25/4/2004   15:30		Start
******************************************************************************/
BOOL CPump::IsPerformLogin (PumpStatus lPumpStatus)
{
	BOOL bRes = FALSE;
	//Pefroming Login if the last action performed isn't a Login action and the current status
	//requires a Login
	if((m_byPumpLoginOp != PUMP_LOGIN) && (m_byPumpLoginOp != PUMP_WAIT_TO_BE_LOGIN) )
	{
		if( (READY == lPumpStatus) || (REQUESTING == lPumpStatus) || (AUTHORIZED == lPumpStatus) || (DISPENSING == lPumpStatus) || (STOPED_BY_OPERATOR == lPumpStatus) ||
			(NOZZLE_LEFT_OUT == lPumpStatus) || (STOPPED_BY_CONTROLLER == lPumpStatus) )
		{
			bRes = TRUE;
		}
	}
	return bRes;
}

/******************************************************************************
 Description:   Determine if a logout to the pump should be performed
 Returns:		0 - No
				1 - Yes
 Parameters:   	PumpStatus lPumpStatus
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi			25/4/2004   15:33		Start
******************************************************************************/
BOOL CPump::IsPerformLogout(PumpStatus lPumpStatus)
{
	BOOL bRes = FALSE;
	//Pefroming Logout if the last action performed isn't a Logout action and the current status
	//requires a Logout

	if((m_byPumpLoginOp != PUMP_LOGOUT) && (m_byPumpLoginOp != PUMP_WAIT_TO_BE_LOGOUT) )
	{
		if( (OUT_OF_SERVICE == lPumpStatus) || (ERROR_TERMINATED == lPumpStatus) || (COMMS_TERMINATED == lPumpStatus) || (POWER_UP == lPumpStatus) ) 
		{
			bRes = TRUE;
		}
	}
	return bRes;
}

/******************************************************************************
 Description:   Sets m_byPumpLoginOp with the new Login Operation
 Returns:		

 Parameters:    PumpLoginOp byLoginOp
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi			25/4/2004   15:38		Start
******************************************************************************/
void CPump::SetPumpLoginOp(PumpLoginOp byLoginOp)
{
	m_byPumpLoginOp = byLoginOp;
}

/******************************************************************************
 Description:   Retrieves m_byPumpLoginOp
 Returns:		m_byPumpLoginOp
				
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Udi			25/4/2004   15:41		Start
******************************************************************************/
PumpLoginOp CPump::GetPumpLoginOp()
{
	return(m_byPumpLoginOp);
}

/******************************************************************************
 Description:  Check if cash acceptor mode valid 	
 Returns:      TRUE/FALSE	
 Parameters:   None;	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			10/9/2003   14:18		Start
******************************************************************************/
//4.0.3.60
BOOL CMode::CashAcceptorValid()
{
	if( m_lFlags & MODE_DEF_CASH_ACCEPTOR_VALID)
		return TRUE;
	else
		return FALSE;

}

//4.0.14.503
BOOL CMode::IsUnmannedMode()
{
	if( m_lFlags & MODE_DEF_UNMANNED_MODE)
		return TRUE;
	else
		return FALSE;

}

//////////////////////////////////////////////////////////////////////
// CTagOlaRecConvert Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTagOlaRecConvert::CTagOlaRecConvert()
{

}

CTagOlaRecConvert::~CTagOlaRecConvert()
{

}

/******************************************************************************
 Description:	Get the PAK auto reconcile attribute status 	
 Returns:      	TRUE/FALSE
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			8/12/2004   18:55		Start
******************************************************************************/
BOOL CMode::PAKAutoReconcile()
{
	if( m_lFlags & MODE_DEF_PAK_AUTO_RECONCILE)
		return TRUE;
	else
		return FALSE;

}

/******************************************************************************
 Description:	Get the PAK auto authorize with loyalty only status 	
 Returns:      	TRUE/FALSE
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			24/11/2009   11:55		Start 4.0.23.508 89149
******************************************************************************/
BOOL CMode::PAKAutoAuthLoyaltyOnly()
{
	if( m_lFlags & MODE_DEF_AUTO_AUTH_LOYALTY_PAK)
		return TRUE;
	else
		return FALSE;

}
/******************************************************************************
 Description:	Update misc card not authorize 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			29/12/2004   14:16		Start
 AmitH			09/08/2010   00:00      Support also AVI decline 4.0.22.461TD 
******************************************************************************/
void CPump::SetMiscCardNotAuthorized()
{	
	long lMisc = m_cPumpStat.GetMisc(); 
	long lResExt = m_cPumpStat.GetResExt();  //4.0.25.501 merge from TD 140738 4.0.22.470


	//4.0.13.73
	if (!(lMisc & MISC_CREDIT_CARD_NOT_AUTHORIZED) && (!(lMisc & MISC_AVI_NOT_AUTHORIZED))) //4.0.25.501 TD 132711
	{
		if (lResExt == RES_EXT_AVI) //TD 140738 4.0.22.470
			lMisc |=MISC_AVI_NOT_AUTHORIZED;
		else
			lMisc |=MISC_CREDIT_CARD_NOT_AUTHORIZED;
		m_cPumpStat.SetMisc(lMisc);
	}

	if (_Module.m_server.GetPAPDeclineTimeValue() && (!(lMisc & MISC_AVI_NOT_AUTHORIZED))) //4.0.25.501 TD 132711
	{
		if (lResExt == RES_EXT_AVI) //TD 140738 4.0.25.501
			lMisc |=MISC_AVI_NOT_AUTHORIZED;

		else if (!(lMisc & MISC_CREDIT_CARD_DECLINE))
		{
			lMisc |=MISC_CREDIT_CARD_DECLINE;
			m_cPumpStat.SetMisc(lMisc);
		}
	}	
}
/******************************************************************************
 Description:	Remove any stuck transaction in state 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			6/1/2005   11:25		Start
******************************************************************************/

long CPump::RemovedStuckTransaction()
{
	CPumpTransact currentTrs;			
	CString sMsg;
	int iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{			
		if(!GetInProgressWaitProgressTrs(&currentTrs))
		{
			// Update regular log
			sMsg.Format("found stuck transaction on trs=%ld ,and clear it",currentTrs.m_lNumber);
			_LOGMSG.LogMsg(currentTrs.m_lPump,LOG_PUMP,sMsg);

			_Module.m_server.m_cExLog.LogTrs(&currentTrs, NULL, NULL, EXCEPTION_MESS_SHIFT_FORCE_REMOVE);
			//found stuck transaction, need to delete record. 
			m_cTrsArray[i].Clear();
		}					
	}
	return 0;
}
//4.0.9.503
/******************************************************************************
 Description:	Return the price change status for each pump 	
 Returns:      	TRUE - in progress / FALSE end. 
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			26/5/2005   13:26		Start
******************************************************************************/

BOOL CPump::GetPriceChangeStatus()
{
	BOOL bRet = FALSE;

	for (int i=0 ;i < MAX_POSITIONS_PER_PUMP ;i++)
	{
		if (m_byPriceChangePosition[i])
		{			
			bRet = TRUE;
			break;
		}
	}	
	return bRet;
}
/******************************************************************************
 Description:	Update price change status on pump 	
 Returns:      	
 Parameters:   	lPosition - Nozzle position ,0 - all nozzle.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			26/5/2005   13:32		Start
******************************************************************************/

void CPump::SetPriceChangeStatus(long lPosition, BOOL bVal)
{
	if (lPosition)
	{
		m_byPriceChangePosition[lPosition -1] = (BYTE)bVal; 
	}
	else   
	{
		// update all position
		for (int i=0 ;i < MAX_POSITIONS_PER_PUMP ;i++)
		{
			m_byPriceChangePosition[i] = (BYTE)bVal; 			
		}	
	}
}

int CPump::ReleasePendingPriceChangeTrs()
{
	int iMaxTrs=0,iUnpaidTrs=0;

	iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{
		if( (m_cTrsArray[i].GetStatus() == UNPAID) &&
			(m_cTrsArray[i].GetOnOffer() == BASE_TERMINAL_NUMBER))
		{
			
			m_cTrsArray[i].SetOnOffer(0);
			iUnpaidTrs =1; 

			if(LOG_LOGIC_ERROR)
			{
				CString str;
				str.Format("trs %02ld , release from pending queue" , m_cTrsArray[i].m_lNumber);
				_LOGMSG.LogMsg(m_cTrsArray[i].m_lPump,LOG_PUMP,str);
			}

		}

	}
	
	return iUnpaidTrs;
}



/******************************************************************************
 Description: AGIP - this function run over all trans arr and for all unpaid PAK
			  Ahe find the oldest deal and return her index
 Returns:      	long - the idx of the
 Parameters:   	
 Assumptions:   
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			21/06/2005				Start
******************************************************************************/
long CPump::FindOldestUnpaidPakTrs()
{
	int iIdx=0;
	CPumpTransact trs;
	CTime cOldTime = CTime::GetCurrentTime();//will change every time we find sooner time
	
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			GetTrsByIndex (i,&trs);
			if(trs.GetStatus() == UNPAID)
			{
				if(!trs.GetOnOffer())
				{
					if(trs.GetTime()<cOldTime)//the trans time is small 
					{
						cOldTime = trs.GetTime();
						iIdx = i;

					}
				}
			}
		}
	}
	


	return iIdx;
}

/******************************************************************************
 Description:	Find incomplete transaction in queue
 Returns:      	found/not found
 Parameters:   	pTrsNumber - trs number
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			28/06/2005   13:32		Start
******************************************************************************/

long CPump::GetInConpleteTrsNumber(long *pTrsNumber)
{
	int iRetNum = 1; // not found	
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].GetStatus() == INCOMPLETE)
			{
				iRetNum = 0; // found
				*pTrsNumber = m_cTrsArray[i].m_lNumber;
				break;
			}
		}
	}
	return iRetNum;	
}
/******************************************************************************
 Description:	Update diesel parameter
 Returns:      	none
 Parameters:   	dwTmp - diesel new status
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			28/06/2005   13:32		Start
******************************************************************************/

void CPump::SetDiselPumpFlags(DWORD dwTmp)
{
	m_dwDieselPump = dwTmp;
}

BOOL CPump::GetDieselPumpFlags()
{
	return ((BOOL)m_dwDieselPump);
}
//4.0.9.507

void CPump::LoadManualInfo(long lModeNumber)
{
	CString str;
	DWORD   dwTmp;

	m_lCurrentServiceMode = lModeNumber;
	
	str.Format("Modes\\Mode%02ld",m_lCurrentServiceMode);
	
	_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "PriceLevel",&dwTmp,(DWORD)0,TRUE); 
	SetPriceLevel(dwTmp);
	
	_Module.m_server.m_cParam.LoadParam((LPCSTR)str, "ModeService",&dwTmp,(DWORD)0,TRUE); 
	SetModeService(dwTmp);

}

//4.0.9999.0
void CPump::SetReeferGrade(long lVal)
{
	m_lReeferId = lVal;
}

long CPump::GetReeferGrade()
{
	return (m_lReeferId);
}
//4.0.9999.0

BOOL CPump::CheckUnpaidFleetTrs()
{
	BOOL bRet = FALSE;
	int iMaxTrs=0,iUnpaidTrs=0;

	iMaxTrs = m_cTrsArray.GetSize();

	for(int i=0; i<iMaxTrs; i++)
	{
		if ((m_cTrsArray[i].GetStatus() == UNPAID) && GetDieselPumpFlags()) // 4.0.10.811
		{
			bRet= TRUE;
			if(LOG_LOGIC_ERROR)
			{
				CString str;
				str.Format("has open fleet TRS %ld ",m_cTrsArray[i].m_lNumber );
				_LOGMSG.LogMsg( m_cTrsArray[i].m_lPump,LOG_PUMP,str );
			}
			break;

		}
	}
	
	return bRet;
}
//4.0.11.509
void CPump::SetDieselGrade(long lVal)
{
	m_lDieselId = lVal;
}

long CPump::GetDieselGrade()
{
	return (m_lDieselId);	
}
//4.0.11.509

void CPumpStatus::SetResExt(PumpResExt lResExt)
{
	m_lResExt = lResExt;  //4.0.12.501
}

//4.0.12.502
void CPump::IncreaseDeclinePrePayCounter()
{
	CString str("Pumps\\Pump");
	char sNumber[8];
	memset(sNumber,0,sizeof(sNumber));
	sprintf(sNumber,"%02d",m_lNumber);
	str += sNumber;	
	CString strLog;
	
	

	m_dwDeclinePrePayCounter++;
	if(LOG_BASIC_FLOW_CONTROL)
	{
		strLog.Format("IncreaseUnBalancePrePayCounter - Update the number of decline PRP %ld",m_dwDeclinePrePayCounter);
		_LOGMSG.LogMsg(m_lNumber,LOG_PUMP,strLog);
	}
	//sending the event via MultiSync with the pump number as client data.
	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str),"PrpDeclienNumber" , m_dwDeclinePrePayCounter,FALSE,_HOST.m_lGroupNum,0,m_lNumber,TRUE);//4.0.3.27	 //4.0.8.505

}
//4.0.12.502
void CPump::DecreaseDeclinePrePayCounter()
{
	CString str("Pumps\\Pump");
	char sNumber[8];
	memset(sNumber,0,sizeof(sNumber));
	sprintf(sNumber,"%02d",m_lNumber);
	str += sNumber;			
	
	CString strLog;

	if(m_dwDeclinePrePayCounter)
		m_dwDeclinePrePayCounter--;

	
	
	if(LOG_BASIC_FLOW_CONTROL)
	{
		strLog.Format("DecreaseDeclinePrePayCounter - Update the number of decline PRP %ld",m_dwDeclinePrePayCounter);
		_LOGMSG.LogMsg(m_lNumber,LOG_PUMP,strLog);
	}

	_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str),"PrpDeclienNumber",m_dwDeclinePrePayCounter,FALSE,_HOST.m_lGroupNum,0,m_lNumber,TRUE);//4.0.3.27 //4.0.8.505
}


/******************************************************************************
 Description:	Fill the PreSet data (for version 8 and 9)
 Returns:      	none
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			06/12/2006   15:00		Change
******************************************************************************/
void CPump::FillPreSetData(long lPumpNumber, BYTE sPreSetLimit[MAX_TRS_IN_LIST][STR_LONG_VAL7], BYTE sPreSetLimitType[MAX_TRS_IN_LIST], long lVersion)
{
	long lRtc = 0; 
	
	for (int i=0; i < MAX_TRS_IN_LIST; i++)
	{
		if (m_cTrsArray[i].GetLinkFlags() & TRS_LINK_EXTRA_INFO)
		{
			// Read from Extrainfo qdx
			CPumpTransact		trs;
			EXTRA_INFO_REC	    info;

			trs.SetLink(TRS_LINK_EXTRA_INFO, m_cTrsArray[i].m_lNumber);				
		
			info.info.lIndexNumber = m_cTrsArray[i].m_lNumber;
			lRtc = _Module.m_server.TrsLinkOperation(REC_READ,&trs,&info);

			if(lRtc == OK)
			{
				if ((lVersion == TAG_PUMP_VERSION_8) || (lVersion == TAG_PUMP_VERSION_9))
				{
					//4.0.15.501
					l2a(info.info.lPreSetlimit, sPreSetLimit[i], sizeof(sPreSetLimit[i]));
					sPreSetLimitType[i] = info.info.sPreSetLimitType;
				}
			}
		}
	}
}


void CPump::FillShortCardName(long lPumpNumber,BYTE  sShortCardNameArr[MAX_TRS_IN_LIST][SHORT_CARD_NAME_LEN])
{
	int i=0;
	for(i=0;i<MAX_TRS_IN_LIST;i++)
	{
		memset(sShortCardNameArr[i],' ',sizeof(sShortCardNameArr[i]));
		memcpy(sShortCardNameArr[i],_Module.m_server.m_cShortCardTrsArr[lPumpNumber].cShortCardNameTrs[i].sShortCardName,SHORT_CARD_NAME_LEN);
	
	}

}	
/******************************************************************************
 Description:	Send End session
 Returns:      	none
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			02/05/2006   13:32		Start
******************************************************************************/

void CPump::ForceCloseOlaSession()
{
	OLA_STAT ola;

	GetOlaStat(&ola);

	if( OLA_STATE(ola.m_byState) != OLA_NONE )
	{
		ola.m_byState = OLA_NEW_STATE(ola.m_byState,OLA_SEND_END);
		SetOlaStat(&ola); 	
		_Module.m_server.SavePumpInfo(m_lNumber); // save ola stat.

		if(LOG_LOGIC_ERROR)
		{
			CString strLog;
			strLog.Format("Force close session - ola.m_byStat=%d" , ola.m_byState);
			_LOGMSG.LogMsg(m_lNumber,LOG_PUMP,strLog);
		}

	}

}

/******************************************************************************
 Description:	Clear list
 Returns:      	none
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			02/05/2006   13:32		Start	4.0.19.504 - 47559
******************************************************************************/

void CMemPump::CleanList()
{
	m_cFifoQueue.RemoveAll();
}


/******************************************************************************
 Description: Check if there is dummy PrePay transaction on the pump 
			  (transaction which marked with MISC_PUMP_CHECK_AND_RESERVE)
 Returns:     void
 Parameters:  short * pDummyPrePayWaiting
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			05/07/2007				Start		4.0.17.91 - TD 47526
 Gena			11/05/2008				Merge		4.0.19.509	(62335)
******************************************************************************/
void CPump::GetDummyPrePayTrs(short * pDummyPrePayWaiting)
{
	int iArraySize = m_cTrsArray.GetSize();
	short nDummyPrePayWaiting = 0;

	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			long lStat, lMisc; 
			lStat = m_cTrsArray[i].GetStatus();
			lMisc = m_cTrsArray[i].m_lMisc;
			if (lStat == WAIT_AUTH)
			{
				if ((lMisc & MISC_PUMP_CHECK_AND_RESERVE))
					nDummyPrePayWaiting++;
			}
		}
	}

	*pDummyPrePayWaiting = nDummyPrePayWaiting;
}


/******************************************************************************
 Description: 
 Returns:     void
 Parameters:  
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			16/06/2008				Merge		//4.0.20.52 (62818)
******************************************************************************/
void CPump::SetTankPerPosition(short nPosition, BYTE byTank)
{
	if ( (nPosition <= MAX_POSITIONS_PER_PUMP) && (nPosition > 0) )
		m_byTankPerPosition[nPosition-1] = byTank;
}

BYTE CPump::GetTankPerPosition(short nPosition)
{
	if ( (nPosition <= MAX_POSITIONS_PER_PUMP) && (nPosition > 0) )
		return m_byTankPerPosition[nPosition-1];
	else 
		return -1;

}

BOOL CPump::IsPumpOnHoldMaxConcurrent()	//4.0.21.500 69203
{
	if (m_cPumpStat.GetMiscExt() & MISC_EXT_MAX_CONCURRENT_DISPENSING)
		return TRUE;
	return FALSE;
}

void CPump::SetPumpOnHoldMaxConcurrent()	//4.0.21.500 69203
{
	long lMiscExt = m_cPumpStat.GetMiscExt();
	CString strLog;
	strLog.Format("SetPumpOnHoldMaxConcurrent OldMisc = %ld" , lMiscExt);
	_LOGMSG.LogMsg(m_lNumber,LOG_PUMP,strLog);
	
	lMiscExt |= MISC_EXT_MAX_CONCURRENT_DISPENSING;
	strLog.Format("SetPumpOnHoldMaxConcurrent NewMisc = %ld" , lMiscExt);
	_LOGMSG.LogMsg(m_lNumber,LOG_PUMP,strLog);
	
	m_cPumpStat.SetMiscExt(lMiscExt);
}

void CPump::UnsetPumpOnHoldMaxConcurrent()	//4.0.21.500 69203
{
	long lMiscExt = m_cPumpStat.GetMiscExt();
	lMiscExt &= MISC_EXT_NOT_MAX_CONCURRENT_DISPENSING;
	m_cPumpStat.SetMiscExt(lMiscExt);
}

short CPump::GetMaxAttendantTrsAllowed() //4.0.22.501 merged from 4.0.22.12 TD 73062
{
	return (short)m_dwMaxAttendantTrsAllowed;
}

/******************************************************************************
 GetTrsIndexByReservation
 Description: Finds trs with given reservation, returns it's and index in the
			  trs array. return the trs number in iTrsNum (by ref param)
 Returns:     int - The index of the trs with the given reservation.
 Parameters:  PumpReservation res - trs reservation
		      int & iTrsNum (by ref param) - used to reutrn the trs num		
			
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Omer			05/09/11				Merge		TD 233242 4.0.23.1050
******************************************************************************/
int CPump::GetTrsIndexByReservation(PumpReservation res , int & iTrsNum)	
{
	for (int i=0; i< m_cTrsArray.GetSize() ; i++)
	{		
		if( m_cTrsArray[i].GetRes() == res)
		{
			iTrsNum = m_cTrsArray[i].m_lNumber;
			return i;
		}
	}

	return -1;


} // TD 233242 4.0.23.1050


/******************************************************************************
 Description: Find the last unpaid PAK transaction

 Returns:     Transaction number
 Parameters:  None
			
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			09/10/11				Start		4.0.29.10 - TD 264032
******************************************************************************/
long CPump::GetLastUnpaidPAKTrs()
{
	long lLastTrsNumber = 0;
	
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if( (m_cTrsArray[i].GetStatus() == UNPAID) && 
				(m_cTrsArray[i].GetRes() == PAY_AT_KIOSK) && 
				(m_cTrsArray[i].m_lNumber > lLastTrsNumber) )
			{
				lLastTrsNumber = m_cTrsArray[i].m_lNumber;
			}
		}
	}

	return lLastTrsNumber;
}

long CPump::GetCurrentTrsRollingUpdateValue(BOOL bGetRemaining /* = FALSE */)
{
	long lRoundedValue = 0;
	
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].GetStatus() == IN_PROGRESS)
			{
				if(bGetRemaining)
					lRoundedValue = m_cTrsArray[i].m_lMaxValue - m_cTrsArray[i].m_lRoundedValue;
				else
					lRoundedValue = m_cTrsArray[i].m_lRoundedValue;
				break;
			}
		}
	}

	return lRoundedValue;
}


// 4.0.25.600 CR 296433
long CPump::GetCurrentTrsRollingUpdateVolume(BOOL bGetRemaining /* = FALSE */)
{
	long lRoundedVolume = 0;
	
	int iArraySize = m_cTrsArray.GetSize();
	
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if(m_cTrsArray[i].GetStatus() == IN_PROGRESS)
			{
				if(bGetRemaining)
					lRoundedVolume = m_cTrsArray[i].m_lMaxVolume - m_cTrsArray[i].m_lRoundedVolume;
				else
					lRoundedVolume = m_cTrsArray[i].m_lRoundedVolume;
				break;
			}
		}
	}

	return lRoundedVolume;
}

// 4.0.25.600 CR 296433
void CPump::GetCurrentTrsTenderType(BYTE * sTenderType)
{
	int iArraySize = m_cTrsArray.GetSize();
	if(iArraySize)
	{
		for (int i=0; i<iArraySize; i++)
		{
			if (m_cTrsArray[i].GetStatus() == IN_PROGRESS)
			{
				memcpy(sTenderType,m_cTrsArray[i].m_sTenderType,sizeof(m_cTrsArray[i].m_sTenderType));
				break;
			}
		}
	}
}


/******************************************************************************
 Description:	Check if trs should be converted from pap to pak	
 Returns:      	TRUE/FALSE
 Parameters:   	long lPumpNumber, 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			30/04/2009   16:32		Start //4.0.22.506 67786
 OmerT									TD 389498/381449/RFUEL-1237
******************************************************************************/
BOOL CPump::IsPapToPakTrs()
{	
	long lTmp = a2l(m_cPAPInfo.CardSaleExtraData3.sFlags1, sizeof(m_cPAPInfo.CardSaleExtraData3.sFlags1));
	return (((lTmp & FL_PAP_TO_PAK) > 0) ? TRUE : FALSE);
}

/******************************************************************************
 Description:	Clear PapToPak Flag	
 Returns:      	
 Parameters:   	long lPumpNumber, 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 OmerT			13/10/2013   16:32		Start 4.0.23.2060 TD 381449
										TD 389498/381449/RFUEL-1237
******************************************************************************/
void CPump::ClearPapToPakFlag()
{
	long lTmp = (a2l(m_cPAPInfo.CardSaleExtraData3.sFlags1, sizeof(m_cPAPInfo.CardSaleExtraData3.sFlags1)) - FL_PAP_TO_PAK);
	l2a(lTmp, m_cPAPInfo.CardSaleExtraData3.sFlags1, sizeof(m_cPAPInfo.CardSaleExtraData3.sFlags1));
}


/******************************************************************************
Description:	Check if pump should be auto authorized by loyalty
Returns:      	TRUE/FALSE
Parameters:   	long lPumpNumber,

WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Adir			23/07/2015   15:30		Start	// CR 407579
******************************************************************************/
BOOL CPump::IsAutoAuthForLoyaltyCustomer()
{
	BOOL bAutoAuthForLoyaltyCustomer = FALSE;

	if (_Module.m_server.IsAutomaticAuthForLoyaltyCustomer() == TRUE)
	{
		bAutoAuthForLoyaltyCustomer = a2l(&m_cPAPInfo.CardSaleExtraData5.sLoyaltyAutoArm, sizeof(m_cPAPInfo.CardSaleExtraData5.sLoyaltyAutoArm));
	}

	if (LOG_LOGIC_ERROR)
	{
		CString str;
		str.Format("IsAutoAuthForLoyaltyCustomer() - bAutoAuthForLoyaltyCustomer %d", bAutoAuthForLoyaltyCustomer);
		_Module.m_server.m_cLogger.LogMsg(m_lNumber, LOG_PUMP, str);
	}

	return (bAutoAuthForLoyaltyCustomer ? TRUE : FALSE);
}
