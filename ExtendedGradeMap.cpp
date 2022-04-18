// ExtendedGradeMap.cpp: implementation of the CExtendedGradeMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExtendedGradeMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExtendedGradeMap::CExtendedGradeMap()
{

}

CExtendedGradeMap::~CExtendedGradeMap()
{

}

void CExtendedGradeMap::Empty()
{
	if (!m_cGradeToExtGrade.IsEmpty())
		m_cGradeToExtGrade.RemoveAll();

	if (!m_cExtGradeToGrade.IsEmpty())
		m_cExtGradeToGrade.RemoveAll();
}

void CExtendedGradeMap::Insert(long lGradeId, long lExtGradeId)
{
	m_cGradeToExtGrade.SetAt(lGradeId, lExtGradeId);
	m_cExtGradeToGrade.SetAt(lExtGradeId, lGradeId);
}


void CExtendedGradeMap::GetExtGradeByGrade(long lGradeId, long &lExtGradeId, long lPumpNumber)
{
	m_cGradeToExtGrade.Lookup(lGradeId, lExtGradeId);

	if(LOG_FCC_FLOW_CONTROL)
	{	
		CString sMsg;
		sMsg.Format("CExtendedGradeMap::GetExtGradeByGrade. lGradeId = %ld; lExtGradeId = %ld" ,lGradeId, lExtGradeId);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
	}
}

void CExtendedGradeMap::GetExtGradeByGrade(BYTE *sGradeId, long lSize, long &lExtGradeId, long lPumpNumber)
{
	long lGradeId = a2l(sGradeId, lSize);
	GetExtGradeByGrade(lGradeId, lExtGradeId, lPumpNumber);
}

void CExtendedGradeMap::GetGradeByExtGrade(long lExtGradeId, long &lGradeId, long lPumpNumber)
{
	m_cExtGradeToGrade.Lookup(lExtGradeId, lGradeId);

	if(LOG_FCC_FLOW_CONTROL)
	{	
		CString sMsg;
		sMsg.Format("CExtendedGradeMap::GetGradeByExtGrade. lExtGradeId = %ld; lGradeId = %ld" ,lExtGradeId, lGradeId);
		_LOGMSG.LogMsg(lPumpNumber,LOG_PUMP,sMsg);
	}
	
}