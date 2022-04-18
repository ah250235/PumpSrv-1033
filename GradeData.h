// GradeData.h: interface for the CGradeData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRADEDATA_H__47B82E64_A818_4429_86E9_5F42E96E81A1__INCLUDED_)
#define AFX_GRADEDATA_H__47B82E64_A818_4429_86E9_5F42E96E81A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGradeData  
{
public:
	void GetGradeName(long lGradeNumber, TStaticString<STATIC_STRING_64> & sGradeFullName);
	void SetGradeName(long lGradeNumber, TStaticString<STATIC_STRING_64>  szInGradeName);
	CGradeData();
	struct GradeDetails 
	{
		TStaticString<STATIC_STRING_64> sGradeFullName;
		//unsigned char	sGradeFullName[MAX_STRING_LEN];
		DWORD			dwTaxLink;
		DWORD			dwTaxRate;
		DWORD			dwDiscountPerUnit;
		DWORD			dwLoyaltyApplicable;
		BOOL			bPriceChanged;
	}m_cGradeDetails[MAX_GRADES_99];
	
	BOOL GetGradePriceChanged(int iGradeNumber) {return m_cGradeDetails[iGradeNumber - 1].bPriceChanged;}
	void SetGradePriceChanged(long iGradeNumber, BOOL bVal){m_cGradeDetails[iGradeNumber - 1].bPriceChanged = bVal;};
	void ResetPriceChangedFlags(long lMaxGrade);
	virtual ~CGradeData();

};

#endif // !defined(AFX_GRADEDATA_H__47B82E64_A818_4429_86E9_5F42E96E81A1__INCLUDED_)
