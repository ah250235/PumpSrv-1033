// OLA32EventSrv.h: interface for the COLA32EventSrv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLA32EVENTSRV_H__3D271CBE_2F0B_44F3_908F_FD563E68A08B__INCLUDED_)
#define AFX_OLA32EVENTSRV_H__3D271CBE_2F0B_44F3_908F_FD563E68A08B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OLA32Srv.h"

class COLA32EventSrv : public COLA32Srv  
{
private:
	IUnknown * m_pEventUnknown;
	IDispatch * m_pEventDispatch;

	BOOL InitInterface();
		
public:
	COLA32EventSrv();
	virtual ~COLA32EventSrv();

	//Virtual functions
	virtual void Init();
	virtual void ReleaseService(BOOL bCloseThread, BOOL bIsShutDown = FALSE);

	//4.0.0.73
	virtual long GetOLAType() { return OLASRV_POSITIVE_32_EVENTS; }

};

#endif // !defined(AFX_OLA32EVENTSRV_H__3D271CBE_2F0B_44F3_908F_FD563E68A08B__INCLUDED_)
