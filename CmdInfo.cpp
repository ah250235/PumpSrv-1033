// CommandInfo.cpp

#include "stdafx.h"
#include "CmdInfo.h"

CCmdInfo::CCmdInfo()
{
	memset(m_Data,0,sizeof(m_Data));
	m_lCommand = CMD_NULL;
	m_lParameter = 0;
	m_lNumber = 0;
	m_lSrc = 0;
}

CCmdInfo::CCmdInfo(Command lCommand, LONG lSrc, LONG lNumber, LONG lParam)
{
	m_lCommand = lCommand;
	m_lParameter = lParam;
	m_lSrc = lSrc;
	m_lNumber = lNumber;
	memset(m_Data,0,sizeof(m_Data));
}

CCmdInfo::CCmdInfo(Command lCommand, LONG lSrc, LONG lNumber, LONG lParam,LPVOID pData, SHORT nDataLen)
{
	m_lCommand = lCommand;
	m_lParameter = lParam;
	m_lSrc = lSrc;
	m_lNumber = lNumber;
	if(pData)
	{
		memcpy((char *)m_Data,(char *)pData, min (sizeof(m_Data), nDataLen));
	}
}


Command CCmdInfo::GetCommand()
{
	return(m_lCommand);
}

void CCmdInfo::SetCommand(Command lCommand)
{
	m_lCommand = lCommand;
}

LONG CCmdInfo::GetParameter()
{
	return(m_lParameter);
}

void CCmdInfo::SetParameter(LONG lParam)
{
	m_lParameter = lParam;
}

LONG CCmdInfo::GetSrc()
{
	return(m_lSrc);
}

void CCmdInfo::SetSrc(LONG lSrc)
{
	m_lSrc = lSrc;
}

void CCmdInfo::operator =( const CCmdInfo& cmdInfo )
{

	m_lCommand = cmdInfo.m_lCommand;
	m_lParameter = cmdInfo.m_lParameter;
	m_lSrc = cmdInfo.m_lSrc;
	m_lNumber = cmdInfo.m_lNumber;
	memcpy(m_Data, cmdInfo.m_Data, sizeof(m_Data));

}

CCmdInfo::CCmdInfo(const CCmdInfo& cmdInfo)
{
	m_lCommand = cmdInfo.m_lCommand;
	m_lParameter = cmdInfo.m_lParameter;
	m_lSrc = cmdInfo.m_lSrc;
	m_lNumber = cmdInfo.m_lNumber;
	memcpy(m_Data, cmdInfo.m_Data, sizeof(m_Data));
}

long CCmdInfo::GetNumber()
{
	return(m_lNumber);
}

void CCmdInfo::SetNumber(long lNumber)
{
	m_lNumber = lNumber;
}

char * CCmdInfo::GetDataPtr()
{
	return(m_Data);
}
