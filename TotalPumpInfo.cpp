// TotalPumpInfo.cpp: implementation of the CTotalPumpInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TotalPumpInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTotalPumpInfo::CTotalPumpInfo()
{
	InitializeCriticalSection(&m_csTotalPumpInfo);

}

CTotalPumpInfo::~CTotalPumpInfo()
{
	DeleteCriticalSection(&m_csTotalPumpInfo);
}


void CTotalPumpInfo::SetTotalsWitnNewData(long lPumpNumber,long lGrade,long lNozzle,char *sVolume,char * sValueA,char * sValueB )
{
	CString str;
	long p=  lPumpNumber-1;
	long n = lNozzle-1;

	if(LOG_BASIC_FLOW_CONTROL)
	{
		str.Format("SetTotalsWitnNewData Update the array  with new data: Pump %ld,Grade %ld,Nuzzle %ld,Volume %.20s, Total Value (A) %.20s, Total Value (B) %.20s)",
			lPumpNumber,lGrade,lNozzle,sVolume,sValueA,sValueB);
		_LOGMSG.LogMsg(str);
	}
	try
	{
		
		EnterCriticalSection( &m_csTotalPumpInfo);
		m_cTotalPumpInfo[p].cTotalNozzleInfo[n].lIsNozzleValid = 1;
		m_cTotalPumpInfo[p].lPumpNumber = lPumpNumber;
		m_cTotalPumpInfo[p].cTotalNozzleInfo[n].lGrade = lGrade;
		m_cTotalPumpInfo[p].cTotalNozzleInfo[n].lNozzle = lNozzle;
		memcpy(m_cTotalPumpInfo[p].cTotalNozzleInfo[n].cTotalInfo.sValueA,
			sValueA,strlen(sValueA));
		
		memcpy(m_cTotalPumpInfo[p].cTotalNozzleInfo[n].cTotalInfo.sValueB,
			sValueB,strlen(sValueB));
		
		memcpy(m_cTotalPumpInfo[p].cTotalNozzleInfo[n].cTotalInfo.sVolume,
			sVolume,strlen(sVolume));
	}
	catch(...)
	{
		str.Format("CTotalPumpInfo::SetTotalsWitnNewData Had UnExcpcted Error");
		_LOGMSG.LogMsg(str);

	}
	LeaveCriticalSection( &m_csTotalPumpInfo );
}



BOOL CTotalPumpInfo::GetTotalData(long lPumpNumber,long lFlags,TAG_MEMO_TOTAL_PUMPS_INFO & tagTotalInfo)
{
	long p=lPumpNumber-1,lNozzle=0;
	CString str;
	BOOL bRetCode= FALSE;

	memset(&tagTotalInfo,' ',sizeof(tagTotalInfo));

	l2a(lPumpNumber, tagTotalInfo.sPump, sizeof(tagTotalInfo.sPump));
	l2a(m_cTotalPumpInfo[p].lPumpStatus, tagTotalInfo.sPumpStatus, sizeof(tagTotalInfo.sPumpStatus)); //4.0.15.120 - CR 44000

	for(lNozzle=0;lNozzle<MAX_NOZZLE_PER_PUMP;lNozzle++) //4.0.15.502
	{
		if(m_cTotalPumpInfo[p].cTotalNozzleInfo[lNozzle].lIsNozzleValid)
		{
					
			EnterCriticalSection( &m_csTotalPumpInfo);
			

			l2a(m_cTotalPumpInfo[p].cTotalNozzleInfo[lNozzle].lGrade , tagTotalInfo.cMapNozzel[lNozzle].sGrade, sizeof(tagTotalInfo.cMapNozzel[lNozzle].sGrade));
			l2a(lNozzle , tagTotalInfo.cMapNozzel[lNozzle].sNozzle, sizeof(tagTotalInfo.cMapNozzel[lNozzle].sNozzle));



			memcpy(tagTotalInfo.cMapNozzel[lNozzle].sValueA,
				m_cTotalPumpInfo[p].cTotalNozzleInfo[lNozzle].cTotalInfo.sValueA,
				sizeof(tagTotalInfo.cMapNozzel[lNozzle].sValueA));
			memcpy(tagTotalInfo.cMapNozzel[lNozzle].sValueB,
				m_cTotalPumpInfo[p].cTotalNozzleInfo[lNozzle].cTotalInfo.sValueB,
				sizeof(tagTotalInfo.cMapNozzel[lNozzle].sValueB));
			memcpy(tagTotalInfo.cMapNozzel[lNozzle].sVolume,
				m_cTotalPumpInfo[p].cTotalNozzleInfo[lNozzle].cTotalInfo.sVolume,
				sizeof(tagTotalInfo.cMapNozzel[lNozzle].sVolume));
		
			bRetCode = TRUE;
			LeaveCriticalSection( &m_csTotalPumpInfo );
			if(LOG_BASIC_FLOW_CONTROL)
			{
				str.Format("GetTotalData return data to : Pump %ld, Status %ld, Grade %ld,Nozzle DATA : %ld,Volume %.20s, Total Value (A) %.20s, Total Value (B) %.20s)",
					lPumpNumber, m_cTotalPumpInfo[p].lPumpStatus, m_cTotalPumpInfo[p].cTotalNozzleInfo[lNozzle].lGrade,lNozzle,tagTotalInfo.cMapNozzel[lNozzle].sVolume,tagTotalInfo.cMapNozzel[lNozzle].sValueA,tagTotalInfo.cMapNozzel[lNozzle].sValueB);
				_LOGMSG.LogMsg(str);
			}
			
		}
		
	}

	return bRetCode;

}
// 4.0.15.120 - CR 44000
// The method sets the status of the pump in case it's offline or any other non-valid status
void CTotalPumpInfo::SetPumpStatus(long lPumpNumber, long lStatus)
{
	CString str;
	long p=  lPumpNumber-1;

	if(LOG_BASIC_FLOW_CONTROL)
	{
		str.Format("SetPumpStatus Update the pump status data: Pump %ld, Status%ld", lPumpNumber, lStatus);
		_LOGMSG.LogMsg(str);
	}
	
	try
	{		
		EnterCriticalSection( &m_csTotalPumpInfo);

		m_cTotalPumpInfo[p].lPumpStatus = lStatus;		
	}
	catch(...)
	{
		str.Format("CTotalPumpInfo::SetPumpStatus Had UnExcpcted Error");
		_LOGMSG.LogMsg(str);

	}
	LeaveCriticalSection( &m_csTotalPumpInfo );

}
