// PumpTotalsXml.h: interface for the CPumpTotalsXml class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUMPTOTALSXML_H__5ECFFABB_622C_4F84_984B_A86D2075FF73__INCLUDED_)
#define AFX_PUMPTOTALSXML_H__5ECFFABB_622C_4F84_984B_A86D2075FF73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////
//	4.0.25.50 - TD 108311
//	The class is responsible for building and parsing 
//	pump totals	information in xml format.
//////////////////////////////////////////////////////////////////

class CPumpTotalsXml  
{
public:
	CPumpTotalsXml();
	virtual ~CPumpTotalsXml();

	void PumpTotalsInfo2Xml(TOTAL_INFO* info, BSTR* pRec);
	void Xml2PumpTotalsInfo(BSTR* pRec, TOTAL_INFO* info);
private:
	long SetReadingType(short nFlags);//CR_475695
};

#endif // !defined(AFX_PUMPTOTALSXML_H__5ECFFABB_622C_4F84_984B_A86D2075FF73__INCLUDED_)
