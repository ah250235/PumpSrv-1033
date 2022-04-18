// BaseEncodeHandler.cpp: implementation of the CBaseEncodeHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseEncodeHandler.h"


CBaseEncodingComDll::CBaseEncodingComDll(const GUID& guidName) : GUID_NAME(guidName)
{	
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

CBaseEncodingComDll::~CBaseEncodingComDll()
{
	CoUninitialize();
}


EncodeComDllRetCodeEnum CBaseEncodingComDll::Init()
{
	return ConnectEncodingComDll();
}

void CBaseEncodingComDll::WriteComError(_com_error &e) const
{
	_LOGMSG.LogResult(0, "CEncodingComDll", "WriteComError",	FALSE, 0, TRUE, e.Error(), e);	
}



EncodeComDllRetCodeEnum CBaseEncodingComDll::RestoreCOM()
{
	EncodeComDllRetCodeEnum eEncodeDLLRetCode = ENCODE_DLL_OK;

	eEncodeDLLRetCode = CloseConnectionEncodingComDll();

	if(eEncodeDLLRetCode == ENCODE_DLL_OK)
		eEncodeDLLRetCode = ConnectEncodingComDll();
	else
		_LOGMSG.LogMsg("CEncodingComDll - RestoreCOM failed",LOG_LEVEL_1);
	
	return eEncodeDLLRetCode;
}


EncodeComDllRetCodeEnum CBaseEncodingComDll::CleanUp()
{
	return CloseConnectionEncodingComDll();
}



//////////////////////////////////////////////////////////////////////
/*                     CBaseEncodeHandler Methods	                */
//////////////////////////////////////////////////////////////////////

/************************************************************************/
/*						Construction/Destruction						*/
/************************************************************************/

CBaseEncodeHandler::CBaseEncodeHandler(const CBasicProtectData& cBasicProtectData) :
		CBasicProtectData(cBasicProtectData) 
{

}


CBaseEncodeHandler::~CBaseEncodeHandler()
{

}


/************************************************************************/
/*        Protected Configuration Loader overloaded methods				*/
/************************************************************************/
//4.0.22.13	TD 72716
void CBaseEncodeHandler::LoadConfiguration(const BOOL bCreate)
{
	DWORD dwTmp = 0;
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PCIExcludesLoyalty",	&dwTmp, (DWORD)0, bCreate);
	m_bExcludesLoyalty = (0 == dwTmp) ? FALSE : TRUE;
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "PCIStrictness",		&dwTmp, (DWORD)0, bCreate);
	
	//***************************
	//**** If region is USA or PCIStrictness is ON - Activate the RemoveProhibitedData !! //4.0.23.508 TD 94548
	//***************************
	if(_Module.m_server.IsCurrentRegion(REGION_USA) == TRUE)
	{		
		dwTmp = 1;
	}
	else if (0 == dwTmp)
	{
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "RemoveProhibitedData",	&dwTmp, (DWORD)0, bCreate);
	}

	m_bRemoveProhibitedData = (0 == dwTmp) ? FALSE : TRUE;
}


/************************************************************************/
/*                       DB Methods                                     */
/************************************************************************/

void CBaseEncodeHandler::RemoveConfidencesData(PAY_AT_PUMP_INFO* pInfoInOut)
{
	memset(pInfoInOut->CardSaleInfo.cardData.sTrack1Data,	' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sTrack1Data));
//	memset(pInfoInOut->CardSaleInfo.cardData.sAccountNumber, ' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sAccountNumber));
	if (pInfoInOut->CardSaleInfo.cardData.sTranType != CD_TYPE_CASH_ACCEPTOR) //4.0.26.500 - TD 154350
		memset(pInfoInOut->CardSaleInfo.cardData.sTrack2Data,	' ', sizeof(pInfoInOut->CardSaleInfo.cardData.sTrack2Data));

	memset(pInfoInOut->CardSaleExtraData3.sParamResult,		' ', sizeof(pInfoInOut->CardSaleExtraData3.sParamResult)); //TD 276656 4.0.124.471

	memset(pInfoInOut->CardSaleExtraData6.LoyaltyCard.sAccountNumber, ' ', sizeof(pInfoInOut->CardSaleExtraData6.LoyaltyCard.sAccountNumber)); //TD 276656 4.0.124.471
}


void CBaseEncodeHandler::RemoveConfidencesData(CARD_SALE_ALL3* pInfoInOut)
{
	memset(pInfoInOut->CardSaleAll.data.sTrack1Data, ' ', sizeof(pInfoInOut->CardSaleAll.data.sTrack1Data));
//	memset(pInfoInOut->CardSaleAll.data.sAccountNumber, ' ', sizeof(pInfoInOut->CardSaleAll.data.sAccountNumber));
	memset(pInfoInOut->CardSaleAll.data.sTrack2Data, ' ', sizeof(pInfoInOut->CardSaleAll.data.sTrack2Data));
	memset(pInfoInOut->extData3.sParamResult,		' ', sizeof(pInfoInOut->extData3.sParamResult)); //TD 276656 4.0.124.471
	memset(pInfoInOut->extData2.sResInfoAMax37, ' ', sizeof(pInfoInOut->extData2.sResInfoAMax37)); // QC 465917
}




inline BOOL	CBaseEncodeHandler::IsPCIExcludesLoyaltyChanged(const BOOL bLastPCIExludesLoyalty)const
{
	BOOL	bRetCode = (bLastPCIExludesLoyalty != m_bExcludesLoyalty) ? TRUE : FALSE;

	return bRetCode;
}