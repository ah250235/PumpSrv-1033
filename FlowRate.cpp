//4.0.27.40 TD 165585
// FlowRateCalculator.cpp: implementation of the CFlowRateCalculator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlowRate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlowRateCalculator::CFlowRateCalculator()
{

}

CFlowRateCalculator::~CFlowRateCalculator()
{

}

/******************************************************************************
 Description:	Calculate transaction flow rates (volume per time) 	
 Returns:      	long - transaction flow rates ("Liter"/Minutes)
 Parameters:   	cTimeStart = flow start - first rolling update time
				cTimeEnd   = flow end - nuzzle place back time
				lVolume    = volume in mili 
				lTrsFlowRate = flow rate calculated by device
				lRatesFromDevice = flag indicating what was calculated by device (average flow rate, trs flow rate)
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shuki			14/3/2011      			Start
******************************************************************************/
long CFlowRateCalculator::CalculateFlowRateByMinute(CTime cTimeStart, CTime cTimeEnd, long lVolume, long lTrsFlowRate,long lRatesFromDevice)
{
	long lRetVal = 0;
	double dTimeSpan = 0;

	if(lRatesFromDevice & FLOW_RATE_INCLUDE_TRS_FLOW)
	{ 
		lRetVal = lTrsFlowRate;
	}
	else
	{
		//calculates the fueling time in seconds
		dTimeSpan = abs(difftime(cTimeStart.GetTime(), cTimeEnd.GetTime()));	
		if(dTimeSpan > 0)
			lRetVal = (lVolume / 1000.0) / (dTimeSpan / 60.0);
		else
			lRetVal = 0;
	}

	return lRetVal;
}

/******************************************************************************
 Description:	Update Trs flow rate  	
 Returns:      	long - transaction flow rates ("Liter"/Minutes), return 0 if FlowRateActive = false or when rate is 0
 Parameters:   	trs = pump transact for which need to calculate the flow rate of
				cCardData   = card data, includes flow rates and flag indicating if rates were calculated by device
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shuki			14/3/2011      			Start
 Shuki          16/3/2011               Update 4.0.27.41
******************************************************************************/
long CFlowRateCalculator::SetFlowRateByMinute(CPumpTransact & trs, CARD_SALE_ALL3 & cCardData)
{
	long lRetVal = 0;
	long lPumpNumber = trs.m_lPump;

	if(_Module.m_server.IsFlowRateActive())
	{
		if(cCardData.extData5.sRatesFromDevice == ' ')
			memset(&cCardData.extData5.sRatesFromDevice, '0' ,sizeof(cCardData.extData5.sRatesFromDevice));


		if (ChrAll((char*)&cCardData.extData5.sAverageFlowRate, sizeof(cCardData.extData5.sAverageFlowRate)))
				memset(&cCardData.extData5.sAverageFlowRate, '0' ,sizeof(cCardData.extData5.sAverageFlowRate));

		lRetVal = CalculateFlowRateByMinute(trs.GetStartFuelingTime(), 
											trs.GetTime(), 
											trs.m_lRoundedVolume, 
											a2l(cCardData.extData5.sTrsFlowRate, sizeof(cCardData.extData5.sTrsFlowRate)), 
											a2l(&cCardData.extData5.sRatesFromDevice, sizeof(cCardData.extData5.sRatesFromDevice)));

		l2a0(lRetVal,cCardData.extData5.sTrsFlowRate, sizeof(cCardData.extData5.sTrsFlowRate));

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("PAP - FlowRate=%.4s, AverageFlowRate=%.4s, Flag=%c" ,cCardData.extData5.sTrsFlowRate,cCardData.extData5.sAverageFlowRate,cCardData.extData5.sRatesFromDevice);
			_LOGMSG.LogMsg("CFlowRateCalculator","SetFlowRateByMinute",lPumpNumber,LOG_PUMP,str);
		}	
	}

	return lRetVal;
}

/******************************************************************************
 Description:	Update Trs flow rate  	
 Returns:      	long - transaction flow rates ("Liter"/Minutes), return 0 if FlowRateActive = false or when rate is 0
 Parameters:   	trs = pump transact for which need to calculate the flow rate of
				cExtraInfo   = extra info, includes flow rates and flag indicating if rates were calculated by device
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shuki			14/3/2011      			Start
 Shuki          16/3/2011               Update 4.0.27.41
******************************************************************************/
long CFlowRateCalculator::SetFlowRateByMinute(CPumpTransact & trs, EXTRA_INFO & cExtraInfo)
{
	long lRetVal = 0;
	long lPumpNumber = trs.m_lPump;

	if(_Module.m_server.IsFlowRateActive())
	{
		if(cExtraInfo.sRatesFromDevice == ' ')
			memset(&cExtraInfo.sRatesFromDevice, '0' ,sizeof(cExtraInfo.sRatesFromDevice));

		if (ChrAll((char*)&cExtraInfo.sAverageFlowRate, sizeof(cExtraInfo.sAverageFlowRate)))
				memset(&cExtraInfo.sAverageFlowRate, '0' ,sizeof(cExtraInfo.sAverageFlowRate));

		lRetVal = CalculateFlowRateByMinute(trs.GetStartFuelingTime(), 
											trs.GetTime(), 
											trs.m_lRoundedVolume, 
											a2l(cExtraInfo.sTrsFlowRate,sizeof(cExtraInfo.sTrsFlowRate)), 
											a2l(&cExtraInfo.sRatesFromDevice, sizeof(cExtraInfo.sRatesFromDevice)));

		l2a0(lRetVal,cExtraInfo.sTrsFlowRate, sizeof(cExtraInfo.sTrsFlowRate));

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("PAK - FlowRate=%.4s, AverageFlowRate=%.4s, Flag=%c" ,cExtraInfo.sTrsFlowRate,cExtraInfo.sAverageFlowRate,cExtraInfo.sRatesFromDevice);
			_LOGMSG.LogMsg("CFlowRateCalculator","SetFlowRateByMinute",lPumpNumber,LOG_PUMP,str);
		}	
	}

	return lRetVal;
}