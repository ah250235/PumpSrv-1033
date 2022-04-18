// OLATcpPositiveSrv.cpp: implementation of the COLATcpPositiveSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLATcpPositiveSrv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//Eyal 4.0.0.34 chamged reg entry to PSRemote_OLA
COLATcpPositiveSrv::COLATcpPositiveSrv() : COLATcpBase(L"PSRemote_OLA")
{
	CreateRegEntries("SOFTWARE\\Positive\\System\\Servers\\PSRemote_OLA",
		             (BYTE*)"OLASrv.Services");
}

COLATcpPositiveSrv::~COLATcpPositiveSrv()
{

}

long COLATcpPositiveSrv::CreateRegEntries(char * sEntry , BYTE * sVal)
{
	HKEY hkeyRes;
	DWORD dwDisposition = 0;

	long lErr = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
				               sEntry,
				               0,
				               NULL,
							   0,
							   KEY_WRITE|KEY_READ|KEY_QUERY_VALUE,
							   NULL,
							   &hkeyRes,
							   &dwDisposition);

	if(dwDisposition == REG_CREATED_NEW_KEY)
	{
		if(!lErr)
		{
			lErr = RegSetValueEx(hkeyRes,
								 "",
								 0,
								 REG_SZ,
								 sVal,
								 (strlen(sEntry) + 1));

		}

		//Eyal 4.0.0.34 Start
		if(!lErr)
		{
			lErr = RegSetValueEx(hkeyRes,
								 "MasterHost",
								 0,
								 REG_SZ,
								 (BYTE*)"noserver",
								 9);
		}
		//Eyal 4.0.0.34 End
	}
	
	RegCloseKey(hkeyRes);
	
	return lErr;
}