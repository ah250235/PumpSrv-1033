#include "StdAfx.h"
#include "Grade.h"



CGrade::CGrade()
{
	m_bIsValid = TRUE;
	m_lGradeNumber = 0;
	strcpy(m_sGradeName,"");
}


CGrade::~CGrade()
{
}
