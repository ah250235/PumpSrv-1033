// OLADotNet.h: interface for the COLADotNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLADOTNET_H__AC258688_9D1A_4E79_8328_C96557350E30__INCLUDED_)
#define AFX_OLADOTNET_H__AC258688_9D1A_4E79_8328_C96557350E30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "OLA32Srv.h"

class COLADotNet : public COLA32Srv  
{
public:
	COLADotNet();
	virtual ~COLADotNet();

	virtual BOOL InitInterface();

	virtual long GetOLAType() { return OLASRV_DOT_NET; }

	virtual HRESULT GetFunctionPointers();

	//4.0.18.505 Merge from 4.0.14.1010 start
	IUnknown * m_pEventUnknown;
	IDispatch * m_pEventDispatch;
	//4.0.18.505 Merge from 4.0.14.1010 end
};

#endif // !defined(AFX_OLADOTNET_H__AC258688_9D1A_4E79_8328_C96557350E30__INCLUDED_)
