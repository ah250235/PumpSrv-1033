// OLADotNet.cpp: implementation of the OLADotNet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OLADotNet.h"

//4.0.18.505 Merge from 4.0.14.1010	start
#include "OLA32Events.h" 
extern 	COLA32Events *_pOLA32Events;
//4.0.18.505 Merge from 4.0.14.1010	end

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COLADotNet::COLADotNet()
{

}

COLADotNet::~COLADotNet()
{

}


BOOL COLADotNet::InitInterface()
{
	BOOL bRetCode = FALSE;
	HRESULT hRes = S_OK;
	CLSID clsid;

	//Get the CLSID for the IServices interface.
	hRes = CLSIDFromProgID(L"OLANET.Services.1" , &clsid);

	if(SUCCEEDED(hRes))
	{
		hRes = CoCreateInstance(clsid,
			                    NULL,
								CLSCTX_ALL,
								IID_IUnknown,
								(void FAR * FAR *)&m_pUnknown);
	}

	if(SUCCEEDED(hRes))
	{
		hRes = m_pUnknown->QueryInterface(IID_IDispatch , (void**)&m_pDispatch);

		if(SUCCEEDED(hRes))
		{
			//Get DISPID handles to OLA session handling methods
			hRes = GetFunctionPointers();

			//if the last function call was successful,
			//we can change the returned value to true.
			if(SUCCEEDED(hRes))
				bRetCode = TRUE;
		}
	}

	if(FAILED(hRes))
	{
		CString str;
		str.Format("COLADotNet::InitInterface(), failed to acquire interface hRes=[%x]",hRes);
		_LOGMSG.LogMsg(str,LOG_LEVEL_0);
	}	

	//4.0.18.505 Merge from 4.0.14.1010	start

	hRes = CLSIDFromProgID(L"Ola32.Ola32CallBack" , &clsid);

	if(SUCCEEDED(hRes))
	{
		hRes = CoCreateInstance(clsid,
					            NULL,
						        CLSCTX_ALL,
						        IID_IUnknown,
						        (void FAR * FAR *)&m_pEventUnknown);
	}

	if(SUCCEEDED(hRes))
	{
		_pOLA32Events = new COLA32Events;

		//Advise the server of the sink
		hRes = _pOLA32Events->DispEventAdvise(m_pEventUnknown);
	}
	
	if(SUCCEEDED(hRes))
		bRetCode = TRUE;

	else
		_LOGMSG.LogMsg("COLA32EventSrv::InitInterface(), failed to connect");


	//4.0.18.505 Merge from 4.0.14.1010	end

	return bRetCode;
}

HRESULT COLADotNet::GetFunctionPointers()
{
	OLECHAR FAR * sMethodName[NUMBER_OF_IMPLEMENTED_METHODS];
	HRESULT hRes = S_OK;

	sMethodName[0] = L"AddParams";
	sMethodName[1] = L"StartSession";
	sMethodName[2] = L"EndSession";
	sMethodName[3] = L"QuerySession";

	for (int i=0 ; i < 4 ; i++)
	{
		//Fill the member array MethodIDs with the
		//DISPIDs of the methods stored in the sMethodName array.
		hRes = m_pDispatch->GetIDsOfNames(IID_NULL,
										  &sMethodName[i],
										  1,
										  LOCALE_USER_DEFAULT,
										  &MethodIDs[i]);

		if(FAILED(hRes))
			break;
	}

	return hRes;
}