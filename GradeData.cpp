// GradeData.cpp: implementation of the CGradeData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GradeData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGradeData::CGradeData()
{

}

CGradeData::~CGradeData()
{

}

void CGradeData::ResetPriceChangedFlags(long lMaxGrade)
{
	for (int j = 0 ; j < lMaxGrade; j++)
		SetGradePriceChanged(j + 1, FALSE);
}

void CGradeData::SetGradeName(long lGradeNumber, TStaticString<STATIC_STRING_64>  szInGradeName)
{
	if ((lGradeNumber > 0 ) && ( MAX_GRADES_99 > lGradeNumber))
		m_cGradeDetails[lGradeNumber - 1].sGradeFullName  = szInGradeName;
	
}

 void CGradeData::GetGradeName(long lGradeNumber, TStaticString<STATIC_STRING_64> & sGradeFullName)
{

	if ((lGradeNumber > 0 ) && ( MAX_GRADES_99 > lGradeNumber))
		sGradeFullName = m_cGradeDetails[lGradeNumber - 1].sGradeFullName;		
	else
		sGradeFullName = "Grade can't found";		
}
