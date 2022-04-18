// ExtendedGradeMap.h: interface for the CExtendedGradeMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTENDEDGRADEMAP_H__C6BFC20C_2B72_44C0_8A60_AE26DCADEB1F__INCLUDED_)
#define AFX_EXTENDEDGRADEMAP_H__C6BFC20C_2B72_44C0_8A60_AE26DCADEB1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExtendedGradeMap  
{
private:
	CMap< long, long, long, long > m_cGradeToExtGrade;
	CMap< long, long, long, long > m_cExtGradeToGrade;
	
public:
	CExtendedGradeMap();
	virtual ~CExtendedGradeMap();

	void Empty();
	void Insert(long lGradeId, long lExtGradeId);
	//void Update(long lGradeId, long lExtGradeId);
	void GetExtGradeByGrade(long lGradeId, long &lExtGradeId, long lPumpNumber = 0);
	void GetExtGradeByGrade(BYTE *sGradeId, long lSize, long &lExtGradeId, long lPumpNumber = 0);
	void GetGradeByExtGrade(long lExtGradeId, long &lGradeId, long lPumpNumber = 0);


};

#endif // !defined(AFX_EXTENDEDGRADEMAP_H__C6BFC20C_2B72_44C0_8A60_AE26DCADEB1F__INCLUDED_)
