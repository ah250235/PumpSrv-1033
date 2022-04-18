// AlarmData.cpp: implementation of the AlarmData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlarmData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AlarmData::AlarmData()
{

}

AlarmData::~AlarmData()
{

}

long AlarmData::GetErrorId()
{
	return m_lErrorID;
}

void AlarmData::SetErrorId(long lError)
{
	m_lErrorID = lError;
}

long AlarmData::GetWarningId()
{
	return m_lWarningId;
}

void AlarmData::SetWarningID(long lWarning)
{
	m_lWarningId = lWarning;
}

long AlarmData::GetAlarmErrorFlags()
{
	return(m_AlarmErrorFlags);
}

void AlarmData::SetAlarmErrorFlags(long lFlags)
{
	m_AlarmErrorFlags = lFlags;
}

void AlarmData::SetAlarmWarningFlags(long lFlags)
{

}
