// CompConectivity.cpp: implementation of the CCompConectivity class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CompConnectivity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCompConnectivity::CCompConnectivity()
{
	m_lPStoCL = CS_NOT_SUPPORTED;
	m_lCLtoPS = CS_NOT_SUPPORTED;
	m_lPStoQDEX = CS_NOT_SUPPORTED;
	m_lPStoOLA = CS_NOT_SUPPORTED;
	m_lPStoAS = CS_NOT_SUPPORTED;
	m_lCLtoFC = CS_NOT_SUPPORTED;
	m_lCLtoGEN = CS_NOT_SUPPORTED;
	m_lGENtoCL = CS_NOT_SUPPORTED;
	m_lCLtoBAP = CS_NOT_SUPPORTED;
	m_lBAPtoCL = CS_NOT_SUPPORTED;
	m_lCLtoTS = CS_NOT_SUPPORTED;
	m_lTStoCL = CS_NOT_SUPPORTED;
	m_lBAPtoPMT = CS_NOT_SUPPORTED;
	m_lPMTtoBAP = CS_NOT_SUPPORTED;
	m_lGENtoXML = CS_NOT_SUPPORTED;
	m_lRFStoQDEX = CS_NOT_SUPPORTED;
	m_lTStoTG = CS_NOT_SUPPORTED;
	m_lPMTtoBO = CS_NOT_SUPPORTED;
	m_lPMTtoOLA = CS_NOT_SUPPORTED;

	for( int i = 0; i < MAX_CONN_SUPPORTED_POS; i++)
	{
		m_cPosData[i].m_lConnStatus = CS_NOT_SUPPORTED;
		m_cPosData[i].m_lPOSNumber = 0;
	}
}

CCompConnectivity::~CCompConnectivity()
{

}
/******************************************************************************
 Description:	Retreives the PS connectivity status 	
 Returns:      	void
 Parameters:   	void* bBuff - buffer to fill with data	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			22/2/2004   16:46		Start
******************************************************************************/
void CCompConnectivity::GetCompConnectivityTagInfo(void *pBuff)
{
	if (pBuff)
	{
		PS_TAG_CONNECTIVITY_INFO *pTagCompConn = reinterpret_cast<PS_TAG_CONNECTIVITY_INFO *>(pBuff);

		//refreshing the OLA status before setting it.
		SetOLAConnStatus();

		l2a(m_lPStoCL, pTagCompConn->sPStoCL, sizeof(pTagCompConn->sPStoCL));
		l2a(m_lCLtoPS, pTagCompConn->sCLtoPS, sizeof(pTagCompConn->sCLtoPS));
		l2a(m_lPStoOLA, pTagCompConn->sPStoOLA, sizeof(pTagCompConn->sPStoOLA));//
		l2a(m_lPStoAS, pTagCompConn->sPStoAS, sizeof(pTagCompConn->sPStoAS));
		l2a(m_lCLtoFC, pTagCompConn->sCLtoFC, sizeof(pTagCompConn->sCLtoFC));
		l2a(m_lCLtoGEN, pTagCompConn->sCLtoGEN, sizeof(pTagCompConn->sCLtoGEN));
		l2a(m_lGENtoCL, pTagCompConn->sGENtoCL, sizeof(pTagCompConn->sGENtoCL));
		l2a(m_lCLtoBAP, pTagCompConn->sCLtoBAP, sizeof(pTagCompConn->sCLtoBAP));
		l2a(m_lBAPtoCL, pTagCompConn->sBAPtoCL, sizeof(pTagCompConn->sBAPtoCL));
		l2a(m_lCLtoTS, pTagCompConn->sCLtoTS, sizeof(pTagCompConn->sCLtoTS));
		l2a(m_lTStoCL, pTagCompConn->sTStoCL, sizeof(pTagCompConn->sTStoCL));
		l2a(m_lBAPtoPMT, pTagCompConn->sBAPtoPMT, sizeof(pTagCompConn->sBAPtoPMT));
		l2a(m_lPMTtoBAP, pTagCompConn->sPMTtoBAP, sizeof(pTagCompConn->sPMTtoBAP));
		l2a(m_lGENtoXML, pTagCompConn->sGENtoXML, sizeof(pTagCompConn->sGENtoXML));
		l2a(m_lRFStoQDEX, pTagCompConn->sRFStoQDEX, sizeof(pTagCompConn->sRFStoQDEX));
		l2a(m_lTStoTG, pTagCompConn->sTStoTG, sizeof(pTagCompConn->sTStoTG)); //4.0.5.39
		l2a(m_lPMTtoBO, pTagCompConn->sPMTtoBO, sizeof(pTagCompConn->sPMTtoBO)); //4.0.8.500
		l2a(m_lPMTtoOLA, pTagCompConn->sPMTtoOLA, sizeof(pTagCompConn->sPMTtoOLA)); //4.0.8.500

		// RFUEL-3032
		// Exiting logic broken, added workaround 
		//l2a(m_lPStoQDEX, pTagCompConn->sPStoQDEX, sizeof(pTagCompConn->sPStoQDEX));		
		const BOOL bDBConnection = _Module.m_server.DBConnectionAvailable();
		m_lPStoQDEX = bDBConnection ? CS_OK : CS_NOT_OK;
		l2a(m_lPStoQDEX, pTagCompConn->sPStoQDEX, sizeof(pTagCompConn->sPStoQDEX));

		////4.0.8.500 Getting the POS status
		SetPosConnStatus();
		for (int i = 0; i < MAX_CONN_SUPPORTED_POS; i++)
		{
			l2a(m_cPosData[i].m_lConnStatus, pTagCompConn->cPosInfo[i].sConnStatus, sizeof(pTagCompConn->cPosInfo[i].sConnStatus));
			l2a(m_cPosData[i].m_lPOSNumber, pTagCompConn->cPosInfo[i].sPOSNumber, sizeof(pTagCompConn->cPosInfo[i].sPOSNumber));;
		}
	}

}

/******************************************************************************
 Description:	Sets the data comes from CL layer to the PS connectivity data 	
 Returns:      	void
 Parameters:   	void* bBuff - buffer to get data from	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			22/2/2004   16:46		Start
******************************************************************************/
void CCompConnectivity::SetCLCompConnectivity(void *pBuff,long lFlags)
{	
	if(pBuff)
	{
		CL_TAG_CONNECTIVITY_INFO2* pCLCompConn = reinterpret_cast<CL_TAG_CONNECTIVITY_INFO2*>(pBuff);
		
		m_lCLtoPS = a2l(pCLCompConn->sCLtoPS,sizeof(pCLCompConn->sCLtoPS));
		m_lCLtoGEN = a2l(pCLCompConn->sCLtoGEN,sizeof(pCLCompConn->sCLtoGEN));
		m_lGENtoCL = a2l(pCLCompConn->sGENtoCL,sizeof(pCLCompConn->sGENtoCL));
		m_lCLtoBAP = a2l(pCLCompConn->sCLtoBAP,sizeof(pCLCompConn->sCLtoBAP));
		m_lBAPtoCL = a2l(pCLCompConn->sBAPtoCL,sizeof(pCLCompConn->sBAPtoCL));
		m_lCLtoTS = a2l(pCLCompConn->sCLtoTS,sizeof(pCLCompConn->sCLtoTS));
		m_lTStoCL = a2l(pCLCompConn->sTStoCL,sizeof(pCLCompConn->sTStoCL));
		m_lBAPtoPMT = a2l(pCLCompConn->sBAPtoPMT,sizeof(pCLCompConn->sBAPtoPMT));
		m_lPMTtoBAP = a2l(pCLCompConn->sPMTtoBAP,sizeof(pCLCompConn->sPMTtoBAP));
		m_lGENtoXML = a2l(pCLCompConn->sGENtoXML,sizeof(pCLCompConn->sGENtoXML));
		m_lRFStoQDEX = a2l(pCLCompConn->sRFStoQDEX,sizeof(pCLCompConn->sRFStoQDEX));
		m_lTStoTG = a2l(pCLCompConn->sTStoTG,sizeof(pCLCompConn->sTStoTG)); //4.0.5.39
		
		//4.0.8.500
		m_lPMTtoBO = a2l(pCLCompConn->sPMTtoBO,sizeof(pCLCompConn->sPMTtoBO));
		m_lPMTtoOLA = a2l(pCLCompConn->sPMTtoOLA,sizeof(pCLCompConn->sPMTtoOLA));
	}
}
/******************************************************************************
 Description:	Sets the qdex connectivity status 	
 Returns:      	void
 Parameters:   	BOOL bConnected - TRUE when qdex is connected
								  FALSE when qdex is not connected		
		
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			22/2/2004   16:50		Start
******************************************************************************/
void CCompConnectivity::SetQdexConn(long lQdexRetVal)
{
	if( lQdexRetVal )
		m_lPStoQDEX = CS_NOT_OK;
	else
		m_lPStoQDEX = CS_OK;
}

/******************************************************************************
 Description:	Sets the AlarmSrv connectivity status 	
 Returns:      	void
 Parameters:   	BOOL bConnected - TRUE when AlarmSrv is connected
								  FALSE when AlarmSrv is not connected		
		
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			22/2/2004   16:50		Start
******************************************************************************/
void CCompConnectivity::SetAlarmSrvConn(BOOL bConnected)
{
	if( bConnected )
		m_lPStoAS = CS_OK;
	else
		m_lPStoAS = CS_NOT_OK;
}

/******************************************************************************
 Description:	Sets PumpSrv to CL and FC connectivity status,uses
				PumpSrv server state 
				(SERVER_NO_CONVERT, SERVER_STARTUP,SERVER_IDLE)
 Returns:      	void
 Parameters:   	ServerState lState					
		
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			22/2/2004   16:50		Start
******************************************************************************/
void CCompConnectivity::SetPumpSrvState(ServerState lState)
{
	switch(lState)
	{
		case SERVER_NO_CONVERT:
		{
			m_lPStoCL = CS_NOT_OK;
			m_lCLtoPS = CS_NOT_OK;
			m_lCLtoFC = CS_NOT_OK;	
			break;
		}
		case SERVER_STARTUP:
		{
			m_lPStoCL = CS_NOT_OK;
			m_lCLtoPS = CS_OK;
			m_lCLtoFC = CS_NOT_OK;
			break;
		}
		case SERVER_IDLE:
		{
			m_lPStoCL = CS_OK;
			m_lCLtoPS = CS_OK;
			m_lCLtoFC = CS_OK;			
			break;
		}
	}
}

/******************************************************************************
 Description:	Sets the OLA connectivity status, reads it from
				PumpSrv, using existing method in ServerMain.
 Returns:      	void
 Parameters:   	None.		
		
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			22/2/2004   16:50		Start
******************************************************************************/
void CCompConnectivity::SetOLAConnStatus()
{
	long lOlaStatus = SERVICE_OFF;

	lOlaStatus = _Module.m_server.GetOLAServiceStatus();

	switch(lOlaStatus)
	{
		case SERVICE_OFF:
		{
			m_lPStoOLA = CS_NOT_OK;
			break;
		}
		case SERVICE_IDLE:
		case SERVICE_IN_MIDDLE_OF_OPERATION:
		{
			m_lPStoOLA = CS_OK;
			break;
		}
		default:
			m_lPStoOLA = CS_NOT_OK;
	}
}

/******************************************************************************
 Description:	Sets the POSs connectivity status, reads it from
				PumpSrv, using existing method in m_cPosMap.
 Returns:      	void
 Parameters:   	None.		
		
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			02/1/2005   16:50		Start 4.0.8.500
******************************************************************************/
void CCompConnectivity::SetPosConnStatus()
{
	CPosInfo tmpPos;
	POSITION position = _Module.m_server.m_cPosMap.GetStartPosition();
	long lTmpPOSNumber = 0;
	int i = 0;

	while(position)
	{
		_Module.m_server.m_cPosMap.GetNextAssoc(position,lTmpPOSNumber,tmpPos);
		
//		if(position == NULL)
//			break;
		//Check that not passed max conn supported POS
		if(i >= MAX_CONN_SUPPORTED_POS)
			break;

		if (tmpPos.IsRealPOS())
		{
			m_cPosData[i].m_lPOSNumber = lTmpPOSNumber;
			if(tmpPos.IsPOSConnected())
				m_cPosData[i].m_lConnStatus = CS_OK;
			else
				m_cPosData[i].m_lConnStatus = CS_NOT_OK;

			//only if real POS
			i++;
		}
			
	}	
}
