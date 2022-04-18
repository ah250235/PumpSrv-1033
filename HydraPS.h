// HydraPS.h: interface for the CHydraPS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HYDRAPS_H__533C4359_5DC3_48C9_8167_CD3E7151A774__INCLUDED_)
#define AFX_HYDRAPS_H__533C4359_5DC3_48C9_8167_CD3E7151A774__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Hydra.h"

typedef struct 
{
	BYTE			sServerState[STR_LONG_VAL2];	// 0 - 1	//HYDSERVICE_IDLE,HYDSERVICE_NO_BACKUP_DATABASE.
	BYTE			sConnState[STR_LONG_VAL2];		// 2 - 3	//CONN_FAILED,CONN_ESTABLISH.
	BYTE			sErrors[STR_LONG_VAL10];		// 4- 13 	//Errors misk
	BYTE			sUpdateDataState[STR_LONG_VAL2];// 14 - 15  //UPDATE_IN_PROCESS,UPDATE_COMPLETE
	BYTE			sFiler[198];					
}TAG_SERVER_INFO;

class CHydraPS : public CHydra  
{
public:
	long GetHydServiceErrors();
	BOOL IsUpdateComplete();
	BOOL IsHydSrvConnected();
	long WriteComError(_com_error & e );
	
	CHydraPS();
	virtual ~CHydraPS();

protected:
	long GetHydSrvInfo();

private:
	long	m_lConnState;
	long	m_lUpdateState;
	long	m_lHydErrors;
};

#endif // !defined(AFX_HYDRAPS_H__533C4359_5DC3_48C9_8167_CD3E7151A774__INCLUDED_)
