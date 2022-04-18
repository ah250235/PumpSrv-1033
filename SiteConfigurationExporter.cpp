// SiteConfigurationExporter.cpp: implementation of the CSiteConfigurationExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SiteConfigurationExporter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSiteConfigurationExporter::CSiteConfigurationExporter()
{

}

CSiteConfigurationExporter::~CSiteConfigurationExporter()
{

}


void CSiteConfigurationExporter::LoadConfiguration()
{
	LoadExportSiteConfigActive();
	LoadExportSiteConfigStartTime();
	LoadExportSiteConfigLastTime();
}


void CSiteConfigurationExporter::ExportSiteConfiguration(const BOOL bExportImmediately)
{
	BOOL bExportSiteConfiguration = bExportImmediately;

	if (bExportSiteConfiguration == FALSE && IsActive())
	{	
		if (IsTimeForExportSiteConfiguration())
		{	
			SetExportSiteConfigLastTimeWithCurrentTime();
			SaveExportSiteConfigLastTime();

			bExportSiteConfiguration = TRUE;
		}
	}

	if (bExportSiteConfiguration)
		_Module.m_server.m_cEventManager.AddEvent(CTRL_EVENT_EXPORT_SITE_CONFIG, 0); 
}


inline BOOL	CSiteConfigurationExporter::IsActive()
{
	return (m_dwExportSiteConfigActive == 1);
}


void CSiteConfigurationExporter::ClearExportSiteConfigLastTime()
{
	SYSTEMTIME	sSystemTime;	
	memset(&sSystemTime, 0, sizeof(sSystemTime));
	
	SetExportSiteConfigLastTime(&sSystemTime);
}

//*************************************************************************
//	Private methods
//*************************************************************************


void CSiteConfigurationExporter::LoadExportSiteConfigActive()
{
	DWORD dwExportSiteConfigActive = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "DataExportInUse", &dwExportSiteConfigActive, (DWORD)0, FALSE);

	if (dwExportSiteConfigActive == 1)
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "ExportSiteConfigActive", &dwExportSiteConfigActive, (DWORD)0, TRUE);	

	m_dwExportSiteConfigActive = dwExportSiteConfigActive;
}

void CSiteConfigurationExporter::LoadExportSiteConfigStartTime()
{
	SZ_TAG_SYSTEMTIME	szTagTime;
	SYSTEMTIME			sSystemTime;
	
	memset(&sSystemTime, 0, sizeof(sSystemTime));
	memset(&szTagTime, 0, sizeof(szTagTime));
	szTagTime.sNull = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "ExportSiteConfigStartTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME), (char *)&szTagTime, TRUE);
	Tag2SystemTime((TAG_SYSTEMTIME*)&szTagTime.sTime, &sSystemTime);

	SetExportSiteConfigStartTime(&sSystemTime);
}

void CSiteConfigurationExporter::LoadExportSiteConfigLastTime()
{
	SZ_TAG_SYSTEMTIME	szTagTime;
	SYSTEMTIME			sSystemTime;
	
	memset(&sSystemTime, 0, sizeof(sSystemTime));
	memset(&szTagTime, 0, sizeof(szTagTime));
	szTagTime.sNull = 0;

	_Module.m_server.m_cParam.LoadParam("GeneralParam", "ExportSiteConfigLastTime", (char *)&szTagTime, sizeof(SZ_TAG_SYSTEMTIME) , (char *)&szTagTime, TRUE, TRUE, 0, HOP_DIRECT_LOCAL, TRUE);
	Tag2SystemTime((TAG_SYSTEMTIME*)&szTagTime.sTime, &sSystemTime);

	SetExportSiteConfigLastTime(&sSystemTime);
}


void CSiteConfigurationExporter::SetExportSiteConfigStartTime(const SYSTEMTIME* pSystemTime)
{
	memset(&m_sExportSiteConfigStartTime, 0, sizeof(m_sExportSiteConfigStartTime));
	memcpy(&m_sExportSiteConfigStartTime, pSystemTime, sizeof(m_sExportSiteConfigStartTime));

	SetDefaultDateToSYSTEMTIME(&m_sExportSiteConfigStartTime);
	m_sExportSiteConfigStartTime.wMilliseconds = 0;
}


void CSiteConfigurationExporter::SetExportSiteConfigLastTime(const SYSTEMTIME* pSystemTime)
{
	memset(&m_sExportSiteConfigLastTime, 0, sizeof(m_sExportSiteConfigLastTime));
	memcpy(&m_sExportSiteConfigLastTime, pSystemTime, sizeof(m_sExportSiteConfigLastTime));
	m_sExportSiteConfigLastTime.wMilliseconds = 0;
}


void CSiteConfigurationExporter::SetExportSiteConfigLastTimeWithCurrentTime()
{
	SYSTEMTIME	sSystemTime;	
	memset(&sSystemTime, 0, sizeof(sSystemTime));

	GetLocalTime(&sSystemTime);
	SetExportSiteConfigLastTime(&sSystemTime);
}

/******************************************************************************
 Description:	Save m_sExportSiteConfigLastTime member to RunTime ini file
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		29/12/2010   09:11				Start		//4.0.26.508 TD 146575
******************************************************************************/
void CSiteConfigurationExporter::SaveExportSiteConfigLastTime()
{
	SZ_TAG_SYSTEMTIME	szTagTime;

	memset(&szTagTime, 0, sizeof(szTagTime));
	szTagTime.sNull = 0;
	
	SystemTime2Tag(&m_sExportSiteConfigLastTime, &szTagTime.sTime);
	_Module.m_server.m_cParam.SaveParam("GeneralParam", "ExportSiteConfigLastTime", (char *)&szTagTime, TRUE, 0, 0, 0, TRUE);	
}




/******************************************************************************
 Description:	IsTimeForExportSiteConfiguration
 Returns:      	
 Parameters:   

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena		28/12/2010   21:16			Start		//4.0.26.508 TD 146575
******************************************************************************/
BOOL CSiteConfigurationExporter::IsTimeForExportSiteConfiguration()const
{
	BOOL bIsTimeForExportSiteConfiguration = FALSE;
	 
	if (LastExportDayIsToday())
		bIsTimeForExportSiteConfiguration = (StartTimeBiggerThanLastTime() && CurrentTimeBiggerThanStartTime());
	else 
		bIsTimeForExportSiteConfiguration = (CurrentTimeBiggerThanStartTime());	

	return bIsTimeForExportSiteConfiguration;
}


BOOL CSiteConfigurationExporter::StartTimeBiggerThanLastTime()const
{
	SYSTEMTIME	sExportSiteConfigStartTime;
	memset(&sExportSiteConfigStartTime, 0, sizeof(sExportSiteConfigStartTime));

	DateCopy(&sExportSiteConfigStartTime, &m_sExportSiteConfigLastTime);
	TimeCopy(&sExportSiteConfigStartTime, &m_sExportSiteConfigStartTime);

	long lCompareRetCode = CompareSystemTime((SYSTEMTIME*)&sExportSiteConfigStartTime, (SYSTEMTIME*)&m_sExportSiteConfigLastTime, NULL, INTERVAL_IN_SECONDS);

	return (lCompareRetCode == SECOND_SYSTEMTIME_BIGGER || lCompareRetCode == COMPARE_ERROR) ? FALSE : TRUE;
}


BOOL CSiteConfigurationExporter::CurrentTimeBiggerThanStartTime()const
{
	SYSTEMTIME	sCurrentSysTime;

	GetLocalTime(&sCurrentSysTime);
	SetDefaultDateToSYSTEMTIME(&sCurrentSysTime);

	long lCompareRetCode = CompareSystemTime(&sCurrentSysTime, (SYSTEMTIME*)&m_sExportSiteConfigStartTime, NULL, INTERVAL_IN_SECONDS);

	return (lCompareRetCode == SECOND_SYSTEMTIME_BIGGER || lCompareRetCode == COMPARE_ERROR) ? FALSE : TRUE;
}


BOOL CSiteConfigurationExporter::LastExportDayIsToday()const
{
	SYSTEMTIME	sCurrentTime;
	GetLocalTime(&sCurrentTime);

	return (m_sExportSiteConfigLastTime.wDay == sCurrentTime.wDay);
}
