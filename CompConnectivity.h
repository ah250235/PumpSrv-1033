// CompConnectivity.h: interface for the CCompConnectivity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPCONNECTIVITY_H__23E12471_A0BF_4319_A1C0_09D30A8C40E9__INCLUDED_)
#define AFX_COMPCONNECTIVITY_H__23E12471_A0BF_4319_A1C0_09D30A8C40E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ConnectivityStatus{

			CS_NOT_SUPPORTED,
			CS_OK,	
			CS_NOT_OK
}; 


typedef struct 
{
	long		m_lPOSNumber;
	long		m_lConnStatus;
} POS_DATA;

class CCompConnectivity  
{
public:
	void SetAlarmSrvConn(BOOL bConnected);
	void SetPumpSrvState(ServerState lState);
	void SetQdexConn(long lQdexRetVal);
	void SetCLCompConnectivity(void *pBuff,long lFlags = 0);
	void GetCompConnectivityTagInfo(void *pBuff);
	CCompConnectivity();
	virtual ~CCompConnectivity();
	
protected:
	void SetPosConnStatus();
	void SetOLAConnStatus();

private:
	long		m_lPStoCL;
	long		m_lCLtoPS;
	long		m_lPStoQDEX;
	long		m_lPStoOLA;
	long		m_lPStoAS;
	long		m_lCLtoFC;
	long		m_lCLtoGEN;
	long		m_lGENtoCL;
	long		m_lCLtoBAP;
	long		m_lBAPtoCL;
	long		m_lCLtoTS;
	long		m_lTStoCL;
	long		m_lBAPtoPMT;
	long		m_lPMTtoBAP;
	long		m_lGENtoXML;
	long		m_lRFStoQDEX;
	long		m_lTStoTG;  //4.0.5.39
	long		m_lPMTtoBO;	//4.0.8.500
	long		m_lPMTtoOLA;//4.0.8.500
	POS_DATA	m_cPosData[MAX_CONN_SUPPORTED_POS];	//4.0.8.500

};

#endif // !defined(AFX_COMPCONNECTIVITY_H__23E12471_A0BF_4319_A1C0_09D30A8C40E9__INCLUDED_)
