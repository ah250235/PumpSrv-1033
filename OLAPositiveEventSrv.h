// OLAPositiveEventSrv.h: interface for the COLAPositiveEventSrv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLAPOSITIVEEVENTSRV_H__9882E263_BBB2_4204_A301_C45001F0F1D1__INCLUDED_)
#define AFX_OLAPOSITIVEEVENTSRV_H__9882E263_BBB2_4204_A301_C45001F0F1D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OLAPositiveSrv.h"

class COLAPositiveEventSrv : public COLAPositiveSrv  
{
//private:
protected:   //4.0.5.40
	ICallBackPtr m_pCallBack;
		
public:	
	COLAPositiveEventSrv();
	virtual ~COLAPositiveEventSrv();

	//Virtual functions
	virtual void Init();
	virtual void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);

	//4.0.0.73
	virtual long GetOLAType() { return OLASRV_POSITIVE_EVENTS; }
};

#endif // !defined(AFX_OLAPOSITIVEEVENTSRV_H__9882E263_BBB2_4204_A301_C45001F0F1D1__INCLUDED_)
