// CFGEventsHandler.cpp: implementation of the CCFGEventsHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CFGEventsHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCFGEventsHandler* _pCFGEventHandler;

CCFGEventsHandler::CCFGEventsHandler()
{
	m_lEventsClientId = 0;	
}

CCFGEventsHandler::~CCFGEventsHandler()
{

}

HRESULT __stdcall CCFGEventsHandler::CFGDBNewData(BSTR sServerPath ,BSTR sTableName ,BSTR sKeyName,long lFlags,long lClientEventData)
{
	int iLen = 0;
	long lRetCode = 0;
	long lRefreshType = 0;
	CString cMsg;
	char szServerName[MAX_SERVER_NAME]; 
	char szTableName [MAX_TABLE_NAME]; 
	char szKeyName [MAX_FIELD_NAME]; 

	memset(szServerName,0,sizeof(szServerName));
	iLen = WideCharToMultiByte(CP_ACP,0,sServerPath,-1,szServerName,sizeof(szServerName),NULL,NULL);
	if(iLen == 0)
	{
		lRetCode = CFGDB_SERVER_NAME_TOO_LONG;
	}

	memset(szTableName,0,sizeof(szTableName));
	iLen = WideCharToMultiByte(CP_ACP,0,sTableName,-1,szTableName,sizeof(szTableName),NULL,NULL);

	if(iLen == 0)
	{
		lRetCode = CFGDB_TABLE_NAME_TOO_LONG;
	}

	memset(szKeyName,0,sizeof(szKeyName));
	iLen = WideCharToMultiByte(CP_ACP,0,sKeyName,-1,szKeyName,sizeof(szKeyName),NULL,NULL);
	if(iLen == 0)
	{
		lRetCode = CFGDB_FIELD_NAME_TOO_LONG;
	}
	if(lFlags & SET_RECORD_EVENT)
	{
		lRefreshType = REFRESH_REGISTRY_TABLE;
		cMsg.Format("CFGEventsHandler::CFGDBNewData: Group %d - Client Number %d,Table %s at Server Path:%s was update",_HOST.m_lGroupNum ,m_lEventsClientId,szTableName,szServerName);
	}	
	else
	{
		cMsg.Format("CFGEventsHandler::CFGDBNewData: Group %d - Client Number %d,Server Path:%s, Table Name: %s, Key Name: %s",_HOST.m_lGroupNum ,m_lEventsClientId,szServerName,szTableName,szKeyName);
		lRefreshType = REFRESH_REGISTRY_KEY;
	}
	
	//4.0.12.506
	if (( !strcmp(szServerName, _HOST.m_szServerPath) ) || ( !strcmp(szServerName, _HOST.m_szServerIniFilePath) ))
	{
		_Module.m_server.RefreshRegistryTable(szTableName,szKeyName,lRefreshType,lClientEventData);
	}

	if(LOG_DETAIL_FLOW_CONTROL)
		_LOGMSG.LogMsg(cMsg);
	
	return S_OK;
}

void CCFGEventsHandler::SetEventClientId(long lClientId)
{
	m_lEventsClientId = lClientId;	
}

long CCFGEventsHandler::GetEventClientId()
{
	return m_lEventsClientId;
}
