// SingleShiftAccumulations.h: interface for the CSingleShiftAccumulations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLESHIFTACCUMULATIONS_H__2CC77B17_F0B9_46C9_8390_F18B42AA812E__INCLUDED_)
#define AFX_SINGLESHIFTACCUMULATIONS_H__2CC77B17_F0B9_46C9_8390_F18B42AA812E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


///////////////////////////////////////////////////////////////////
// SingleShiftAccumulations QDEX

// 4.0.25.50 - TD 108311
// This Qdex is responsible to store tankreading and pumptotals
// accumulations per single shift
///////////////////////////////////////////////////////////////////

typedef struct 
{
	long lOffSet;
	long lSize;
}SingleShiftAccumRelativeIndex;


class CSingleShiftAccumulations : public CRelativeFile  
{
public:
	CSingleShiftAccumulations();
	virtual ~CSingleShiftAccumulations();
	long Operation(long lOpCode, void *pRec, void *pData);
	long ValidityCheck();

};

#endif // !defined(AFX_SINGLESHIFTACCUMULATIONS_H__2CC77B17_F0B9_46C9_8390_F18B42AA812E__INCLUDED_)
