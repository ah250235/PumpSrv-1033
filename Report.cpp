// Report.cpp: implementation of the CReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PumpSrv.h"
#include "Report.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CReport::CReport()
{
}

CReport::~CReport()
{
}

void CReport::GetOlaStat(OLA_STAT * pOlaStat)
{
	*pOlaStat = m_cOla;
}

void CReport::SetOlaStat(OLA_STAT * pOlaStat)
{
	m_cOla = *pOlaStat;
}

void CReport::ClearOlaStat()
{
	m_cOla.m_byState			= OLA_NONE;
	m_cOla.m_byCheckCount	= 0;
	m_cOla.m_lSessionNumber	= 0;
	memset(&m_cOla.m_cSysTime,0,sizeof(SYSTEMTIME));
}

void CReport::SetPAPInfo(PAY_AT_PUMP_INFO *pPAPInfo)
{
	memset(&m_cPAPInfo,0,sizeof(m_cPAPInfo));
	_Module.m_server.m_cXmlConvertorEx.ConvertFromStructToXml((char*)&m_cPAPInfo,pPAPInfo);

	//m_cPAPInfo =  *pPAPInfo;
}

//4.0.14.500
void CReport::SetPAPInfo(CXMLInterface *pPAPInfo)
{
	memset(&m_cPAPInfo,0,sizeof(m_cPAPInfo));
	pPAPInfo->GetXmlString(m_cPAPInfo);
}

void CReport::GetPAPInfo(PAY_AT_PUMP_INFO *pPAPInfo)
{
	memset(pPAPInfo,' ',sizeof(PAY_AT_PUMP_INFO));
	_Module.m_server.m_cXmlConvertorEx.ConvertFromXmlToStruct((char*)&m_cPAPInfo,pPAPInfo);
}
/*
void CReport::GetPAPInfo(CXMLInterface *pPAPInfo)
{
	//Only if we have xml information
	if (m_cPAPInfo[0])
	{
		pPAPInfo->LoadXmlString((char*)&m_cPAPInfo);
	}
	//*pPAPInfo =  m_cPAPInfo;
}

*/
short CReport::GetRequestStatus()
{
	return m_cRequestStatus;
}

void CReport::SetRequestStatus(unsigned char byNewStatus)
{
	m_cRequestStatus = byNewStatus;
}
