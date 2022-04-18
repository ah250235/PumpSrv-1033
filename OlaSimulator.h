// OLASimulator.h: interface for the COLASimulator class.
// This class features an OLA simulator - the OLA actions are done but
// stay within the OLA server boundaries and not outside of it.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OLASIMULATOR_H__57AB8F15_1C28_44FC_B33B_E89B2D726721__INCLUDED_)
#define AFX_OLASIMULATOR_H__57AB8F15_1C28_44FC_B33B_E89B2D726721__INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PumpClass.h"
#include "OLAPositiveEventSrv.h"

	
class COLASimulator : public COLAPositiveEventSrv  
{
public:
	COLASimulator();
	virtual ~COLASimulator();
	virtual long GetOLAType() { return OLASRV_SIMULATOR; }
};

#endif // !defined(AFX_OLAPOSITIVEIFSF_H__57AB8F15_1C28_44FC_B33B_E89B2D726721__INCLUDED_)
