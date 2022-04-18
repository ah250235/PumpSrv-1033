// SiteConfigurationExporter.h: interface for the CSiteConfigurationExporter class.
//		Created by Gena 4.0.26.508 TD 146575
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SITECONFIGURATIONEXPORTER_H__8A7945C2_F6C2_4B54_9957_690666ECAB89__INCLUDED_)
#define AFX_SITECONFIGURATIONEXPORTER_H__8A7945C2_F6C2_4B54_9957_690666ECAB89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSiteConfigurationExporter  
{
public:
	CSiteConfigurationExporter();
	virtual ~CSiteConfigurationExporter();

	void	LoadConfiguration();
	void	ExportSiteConfiguration(const BOOL bExportImmediately = FALSE);
	inline BOOL	IsActive();
	void	ClearExportSiteConfigLastTime();												

private:
	void	LoadExportSiteConfigActive();
	void	LoadExportSiteConfigStartTime();
	void	LoadExportSiteConfigLastTime();

	void	SetExportSiteConfigStartTime(const SYSTEMTIME* pTagTime);				
	void	SetExportSiteConfigLastTime(const SYSTEMTIME* pTagTime);				

	void	SetExportSiteConfigLastTimeWithCurrentTime();
	void	SaveExportSiteConfigLastTime();											

	BOOL	IsTimeForExportSiteConfiguration()const;								
	BOOL	LastExportDayIsToday()const;
	BOOL	CurrentTimeBiggerThanStartTime()const;
	BOOL	StartTimeBiggerThanLastTime()const;

private:
	DWORD			m_dwExportSiteConfigActive;				
	SYSTEMTIME		m_sExportSiteConfigStartTime;			
	SYSTEMTIME		m_sExportSiteConfigLastTime;			

};

#endif // !defined(AFX_SITECONFIGURATIONEXPORTER_H__8A7945C2_F6C2_4B54_9957_690666ECAB89__INCLUDED_)
