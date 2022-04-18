// OLATcpPositiveSrv.h: interface for the COLATcpPositiveSrv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLATCPPOSITIVESRV_H__70998E26_6386_4299_B860_8BB9AB8AFB93__INCLUDED_)
#define AFX_OLATCPPOSITIVESRV_H__70998E26_6386_4299_B860_8BB9AB8AFB93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OLATcpBase.h"

class COLATcpPositiveSrv : public COLATcpBase  
{
public:
	COLATcpPositiveSrv();
	virtual ~COLATcpPositiveSrv();

	virtual long CreateRegEntries(char * sEntry , BYTE * sVal);

	//4.0.0.73
	virtual long GetOLAType() { return (OLASRV_POSITIVE | IS_OLA_REMOTE); }
};

#endif // !defined(AFX_OLATCPPOSITIVESRV_H__70998E26_6386_4299_B860_8BB9AB8AFB93__INCLUDED_)
