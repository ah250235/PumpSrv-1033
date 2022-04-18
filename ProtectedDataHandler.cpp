// ProtectedDataHandler.cpp: implementation of the CProtectedDataHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProtectedDataHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



/************************************************************************/
/*                       Construction/Destruction						*/
/************************************************************************/

CProtectedDataHandler::CProtectedDataHandler():	m_pcBaseEncodeHandler(NULL),
												m_bPCIActiveFlag(FALSE),
												m_bDSSActiveFlag(FALSE)
{

}


CProtectedDataHandler::~CProtectedDataHandler()
{

}



/************************************************************************/
/*                       Initialization Methods                         */
/************************************************************************/

EncodeComDllRetCodeEnum CProtectedDataHandler::Init()
{
	EncodeComDllRetCodeEnum eEncodeDLLRetCode = ENCODE_DLL_CUSTOMIZE_ERROR;

	//According to the registry, decide which encoding to use
	if( PCI_FLAG_ON == m_bPCIActiveFlag )
	{	
		if ( NULL == m_pcBaseEncodeHandler )
		{		
			if (m_bDSSActiveFlag)
				m_pcBaseEncodeHandler = new CDSSEncodeHandler(m_cBasicProtectData);
			else
				m_pcBaseEncodeHandler = new CPCIEncodeHandler(m_cBasicProtectData);
			
			eEncodeDLLRetCode = m_pcBaseEncodeHandler->Init();	//The init calls connect	
		}		
	}

	return eEncodeDLLRetCode;
}

/******************************************************************************
 Description:	Load PCI\DSS registry configuration
 Returns:      	
 Parameters:   	bCreate - if TRUE -> create new parameter
						  if FALSE -> not 				 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			15/02/2009	13:00		Started //4.0.22.13	TD 72716
******************************************************************************/
void CProtectedDataHandler::LoadConfiguration(const BOOL bCreate)
{
	m_cBasicProtectData.LoadConfiguration(bCreate);

	DWORD dwTmp = 0;	

	//***************************
	//**** If region is USA - Force PCI compliant !! //4.0.23.508
	//***************************
	if(_Module.m_server.IsCurrentRegion(REGION_USA) == TRUE)			
	{	
		dwTmp = 1;
		_Module.m_server.m_cParam.SaveParam("GeneralParam", "PCICompliant", dwTmp, dwTmp, bCreate);
	}
	else
	{
		dwTmp = 0;
		_Module.m_server.m_cParam.LoadParam("GeneralParam", "PCICompliant", &dwTmp, (DWORD)0, bCreate);
	}
	
	m_bPCIActiveFlag = (0 == dwTmp) ? PCI_FLAG_OFF : PCI_FLAG_ON;
	
	dwTmp = 0;
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "DSSActive", &dwTmp, (DWORD)0, bCreate);	//74635	

	if (PCI_FLAG_ON == m_bPCIActiveFlag)	
	{
		if(0 == dwTmp)
		{
			m_bDSSActiveFlag = FALSE;
			_LOGMSG.LogMsg("PCICompliant is ON! DSSActive is OFF!");
		}
		else
		{
			m_bDSSActiveFlag = TRUE;
			_LOGMSG.LogMsg("PCICompliant is ON! DSSActive is ON!");
		}

	}
	else
	{
		m_bDSSActiveFlag = FALSE;
		_LOGMSG.LogMsg("PCICompliant is OFF! Set DSSActive to OFF!");
	}
}

/************************************************************************/
/*                       DB Methods                                     */
/************************************************************************/

void CProtectedDataHandler::RemoveConfidencesData(PAY_AT_PUMP_INFO* pInfoInOut)const
{
	if(IsEncodeActive())
		m_pcBaseEncodeHandler->RemoveConfidencesData(pInfoInOut);
}


/******************************************************************************
 Description:	If Encode Active (PCI/DSS) or RemoveCardDataFromLog is ON -> Replace BufferChangeData with length	
 Returns:      	
 Parameters:   	sBufferChangeData -		buffer for Replace
				lBufferChangeDataSize - size of buffer for Replace
				pszOutBuff			  - Replaced buffer	
				 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	19:00		Update 4.0.20.501 TD-60236
******************************************************************************/

void CProtectedDataHandler::RemoveConfidencesData(CARD_SALE_ALL3* pInfoInOut)const
{
	if(IsEncodeActive())
		m_pcBaseEncodeHandler->RemoveConfidencesData(pInfoInOut);
}


//4.0.24.60 TD 112773
void CProtectedDataHandler::RemoveConfidencesDataFromMemory(short bySessionType, PAY_AT_PUMP_INFO* pInfoInOut)const
{
	if(IsEncodeActive())
		m_pcBaseEncodeHandler->RemoveConfidencesDataFromMemory(bySessionType, pInfoInOut);
}


/************************************************************************/
/*                       Log Methods                                    */
/************************************************************************/


/******************************************************************************
 Description:	If Encode Active (PCI/DSS) or RemoveCardDataFromLog is ON -> Replace BufferChangeData with length	
 Returns:      	
 Parameters:   	sBufferChangeData -		buffer for Replace
				lBufferChangeDataSize - size of buffer for Replace
				pszOutBuff			  - Replaced buffer	
				 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	19:00		//4.0.22.13	TD 72716
******************************************************************************/

BOOL CProtectedDataHandler::LogReplaceBufferUseSize(char *pszOutBuff, const char *psBufferChangeData, const long lBufferChangeDataSize)const
{
	BOOL	bRetCode = TRUE;

	if(TRUE == IsEncodeActive())
	{
		bRetCode = m_pcBaseEncodeHandler->ReplaceBufferUseSize(pszOutBuff, psBufferChangeData, lBufferChangeDataSize);
	}
	else if( TRUE == m_cBasicProtectData.IsBasicProtectActive() )
	{
		bRetCode = m_cBasicProtectData.ReplaceBufferUseSize(pszOutBuff, psBufferChangeData, lBufferChangeDataSize);
	}
	else
	{
		memcpy(pszOutBuff, psBufferChangeData, lBufferChangeDataSize);
	}
	
	return bRetCode;
}

/******************************************************************************
 Description:if Pci is on than we remove card data from logs
 Returns:      
 Parameters:   	
				
 //4.0.10.10
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amram			08/11/2005				Start
 Gena			18/02/2009				move from ServerMain //4.0.22.13	TD 72716
******************************************************************************/
void CProtectedDataHandler::RemoveDataFromLog(CString& strOutBuff, CString& strNew)const
{
	//char		szResult[MAX_BUF] ;
	//memset(szResult, 0, sizeof(szResult));
	CString szResult;

	BOOL		bRetVal = TRUE;

	int iLen = strOutBuff.GetLength();
	strOutBuff.Insert(iLen, ',');  

	
	
	if (TRUE == IsEncodeActive())
	{
		bRetVal = m_pcBaseEncodeHandler->RemoveCardInfoFromMessage((char *)(LPCTSTR)strOutBuff, strOutBuff.GetLength(), szResult);
	}
	else if( TRUE == m_cBasicProtectData.IsBasicProtectActive() )
	{		
		bRetVal = m_cBasicProtectData.RemoveCardInfoFromMessage((char *)(LPCTSTR)strOutBuff, strOutBuff.GetLength(), szResult);
	}
	else
	{
		szResult = strOutBuff;
		//memcpy( szResult, (char *)(LPCTSTR)strOutBuff, strOutBuff.GetLength());
		bRetVal = FALSE;
	}

	if(TRUE == bRetVal)
		_LOGMSG.LogMsg("RemoveDataFromLog successfully");

	strNew.Format("%s",szResult); 
}

/******************************************************************************
  Description:		Encrypt OLA String
  Returns:      
  Parameters:  	
  
  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
  Gena			17/08/2008	19:00		Change class name //4.0.22.13	TD 72716
 ******************************************************************************/
BOOL CProtectedDataHandler::EncryptOLAString(char *pszOLAString, char *pszInBuff, char *pszOutBuff, long *plOutBuffLen)const
{
	BOOL	bRetCode = TRUE;

	if( TRUE == IsEncodeActive() )
	{
		bRetCode = m_pcBaseEncodeHandler->MaskOLAString(pszOLAString, pszInBuff, pszOutBuff, plOutBuffLen);
	}
	else if( TRUE == m_cBasicProtectData.IsBasicProtectActive() )
	{
		bRetCode = m_cBasicProtectData.MaskOLAString(pszOLAString, pszInBuff, pszOutBuff, plOutBuffLen);
	}
	else
	{
		memcpy(pszOutBuff, pszInBuff, strlen(pszInBuff));
	}

	return bRetCode;
}


/******************************************************************************
  Description:	Encrypt fields - AccountNumber, Track1, Track2, 
				from CXMLInterface.   	


  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
 Gena			17/08/2008	19:00		 //4.0.22.13	TD 72716
 AmitH          03/04/2011  20:00        //4.0.27.45    Add RECEIPT_DATA4_EX input support
******************************************************************************/

BOOL CProtectedDataHandler::EncryptOLAFields(CXMLInterface *sData)const
{
	BOOL	bRetCode = TRUE;

	if( TRUE == IsEncodeActive() )
	{
		m_pcBaseEncodeHandler->MaskOLAFields(sData);
	}
	else if( TRUE == m_cBasicProtectData.IsBasicProtectActive() )
	{
		m_cBasicProtectData.MaskOLAFields(sData);
	}

	return bRetCode;
}

BOOL CProtectedDataHandler::EncryptOLAFields(CARD_SALE_ALL3 *sData)const
{
	BOOL	bRetCode = TRUE;

	if( TRUE == IsEncodeActive() )
	{
		m_pcBaseEncodeHandler->MaskOLAFields(sData);
	}
	else if( TRUE == m_cBasicProtectData.IsBasicProtectActive() ) 
	{
		m_cBasicProtectData.MaskOLAFields(sData);
	}

	return bRetCode;
}

//4.0.27.45
BOOL CProtectedDataHandler::EncryptOLAFields(RECEIPT_DATA4_EX *sData)const
{
	BOOL	bRetCode = TRUE;

	if( TRUE == IsEncodeActive() )
	{
		m_pcBaseEncodeHandler->MaskOLAFields(sData);
	}
	else if( TRUE == m_cBasicProtectData.IsBasicProtectActive() ) 
	{
		m_cBasicProtectData.MaskOLAFields(sData);
	}

	return bRetCode;
}
//4.0.27.45
BOOL CProtectedDataHandler::EncryptOLAFields(CARD_SALE_ALL2 *sData)const
{
	BOOL	bRetCode = TRUE;

	if( TRUE == IsEncodeActive() )
	{
		m_pcBaseEncodeHandler->MaskOLAFields(sData);
	}
	else if( TRUE == m_cBasicProtectData.IsBasicProtectActive() ) 
	{
		m_cBasicProtectData.MaskOLAFields(sData);
	}

	return bRetCode;
}

/************************************************************************/
/*                         Account Methods                              */
/************************************************************************/


BOOL CProtectedDataHandler::GetDecryptedAccount(char* sEncryptAccount,char* sEncryptedPart,char* sDecryptedAccount)
{
	BOOL bRetCode = FALSE;

	if(TRUE == IsEncodeActive())
		bRetCode = m_pcBaseEncodeHandler->GetDecryptedAccount(sEncryptAccount, sEncryptedPart, sDecryptedAccount);

	return bRetCode;
}


BOOL CProtectedDataHandler::EncodeForTrs(const short nTrsStatus, CARD_SALE_ALL3* pcCardSaleAll3InOut)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;
	BOOL				bRetCode = TRUE;

	if(TRUE == IsEncodeActive())
	{
		eEncodeRetCode = m_pcBaseEncodeHandler->EncodeForTrs(nTrsStatus, pcCardSaleAll3InOut);

		if(eEncodeRetCode != ENCODE_OK) 
			bRetCode = FALSE;
	}
	
	return bRetCode;
}


BOOL CProtectedDataHandler::EncodeForTrs(const short nTrsStatus, PAY_AT_PUMP_INFO *pcPAPInfoInOut)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;
	BOOL				bRetCode = TRUE;

	if(TRUE == IsEncodeActive())
	{
		eEncodeRetCode = m_pcBaseEncodeHandler->EncodeForTrs(nTrsStatus, pcPAPInfoInOut);

		if(eEncodeRetCode != ENCODE_OK) 
			bRetCode = FALSE;
	}
	
	return bRetCode;
}


BOOL  CProtectedDataHandler::RestoreData(PAY_AT_PUMP_INFO* pcPayAtPumpInfoInOut, const long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;
	BOOL				bRetCode = TRUE;

	if(TRUE == IsEncodeActive())
	{
		eEncodeRetCode = m_pcBaseEncodeHandler->RestoreData(pcPayAtPumpInfoInOut, lTrsNumber);

		if(eEncodeRetCode != ENCODE_OK) 
			bRetCode = FALSE;
	}
	
	return bRetCode;
}



/************************************************************************/
/*                     Receipt Index Methods                            */
/************************************************************************/


long CProtectedDataHandler::GetReceiptIndex(char* sIndexOut, const PAY_AT_PUMP_INFO *pPAPInfoIn, const long lPumpNumber)
{
	long lRetIndexLen = 0;

	if(TRUE == IsEncodeActive())
		lRetIndexLen = m_pcBaseEncodeHandler->GetReceiptIndex(sIndexOut, pPAPInfoIn, lPumpNumber);
	
	return lRetIndexLen;
}


long CProtectedDataHandler::GetReceiptIndex(char* sIndexOut, const BSTR sTrack2In, const long lFlags, const long lPumpNumber, const long lTrsNumber)
{
	long lRetIndexLen = 0;

	if(TRUE == IsEncodeActive())
	{
		if ( !(lFlags & RECEIPT_INDEX_IS_RECEPT_KEY) && 
			  (lTrsNumber > 0) && 
			  (TRUE == m_pcBaseEncodeHandler->IsLookForInternalReceiptIndex()))			//4.0.26.500 TD 144219
		{
			lRetIndexLen = _Module.m_server.m_cCommonFunction.LookForInternalReceiptIndex(sIndexOut, lTrsNumber);
		}
		
		if (0 == lRetIndexLen)
		{
			lRetIndexLen = m_pcBaseEncodeHandler->GetReceiptIndex(sIndexOut, sTrack2In, lFlags, lPumpNumber);
		}
		else
		{
			CString strLogMsg;
			strLogMsg.Format("GetReceiptIndex, Receipt index is a internal saved index=[%.100s]", sIndexOut);
			_LOGMSG.LogMsg(strLogMsg);
		}
	}

	return lRetIndexLen;
}


void CProtectedDataHandler::GenerateIndex(PAY_AT_PUMP_INFO* pInfo, const BOOL bStoreAccountNum, const long lTrsNumber)
{
	if(TRUE == IsEncodeActive())
		m_pcBaseEncodeHandler->GenerateIndex(pInfo, bStoreAccountNum, lTrsNumber);
}


//Gena			01/07/2010	21:00		 //4.0.25.30 
void CProtectedDataHandler::GenerateIndex(CARD_SALE_ALL3* pcCardSaleAll3InOut, const BOOL bStoreAccountNum, const long lTrsNumber)
{
	if(TRUE == IsEncodeActive())
		m_pcBaseEncodeHandler->GenerateIndex(pcCardSaleAll3InOut, bStoreAccountNum, lTrsNumber);
}

/************************************************************************/
/*                     Control configurations Methods					*/
/************************************************************************/

BOOL CProtectedDataHandler::IsNeedToRemoveCardDataFromLog()const
{ 
	BOOL	bRetCode = (IsEncodeActive() || m_cBasicProtectData.GetRemoveCardDataFromLog()) ? TRUE : FALSE; 

	return bRetCode;
}


inline BOOL	CProtectedDataHandler::IsEncodeActive()const
{ 
	BOOL	bRetCode = (m_pcBaseEncodeHandler == NULL) ? FALSE : TRUE;

	return bRetCode;
}


BOOL CProtectedDataHandler::IsPAPEnabled()	//4.0.21.13 68756
{
	BOOL	bRetCode = (TRUE == IsEncodeActive()) ? m_pcBaseEncodeHandler->IsPAPEnabled() : TRUE;		//72716

	return bRetCode;
}


//4.0.27.501 TD 233308
void CProtectedDataHandler::SetNoneOPTReceiptAccountMask(PAY_AT_PUMP_INFO* pcPayAtPumpInfo, const char* pszAccountNum)const
{
	m_pcBaseEncodeHandler->SetNoneOPTReceiptAccountMask(pcPayAtPumpInfo, pszAccountNum);
}

/************************************************************************/
/*                     Access configurations Methods					*/
/************************************************************************/


BOOL CProtectedDataHandler::GetExcludesLoyalty()const
{
	BOOL	bExcludesLoyalty = (TRUE == IsEncodeActive()) ? m_pcBaseEncodeHandler->GetExcludesLoyalty() : FALSE;

	return bExcludesLoyalty;
}


BOOL CProtectedDataHandler::GetRemoveProhibitedData()const
{
	BOOL	bRemoveProhibitedData = (TRUE == IsEncodeActive()) ? m_pcBaseEncodeHandler->GetRemoveProhibitedData() : FALSE;

	return bRemoveProhibitedData;
}


void CProtectedDataHandler::SetRemoveProhibitedData(const BOOL bVal)
{
	if(TRUE == IsEncodeActive()) 
		m_pcBaseEncodeHandler->SetRemoveProhibitedData(bVal);
}


/******************************************************************************
  Description:	Check if encoding configurations was changes on th registry and send alert.   	
				ALL MEMBERS REMAINING WITHOUT CHANGE!

  WHO			WHEN                    WHAT
 -------------------------------------------------------------------------------
 Gena			16/04/2009	19:00		 //4.0.22.43	TD 72716
******************************************************************************/

void CProtectedDataHandler::ChangesResponse()
{
	const size_t	iParameterNumber = 2;
	const char		szParameterNames[iParameterNumber][MAX_NAME_LEN] = {"PCIActive,", 
																		"DSSActive,",
																		};
	CString			strAlertDescription;
	//Save last configuration
	BOOL			bPCIActiveFlag = m_bPCIActiveFlag;
	BOOL			bDSSActiveFlag = m_bDSSActiveFlag;

	strAlertDescription.Empty();

	//Load configuration from registry
	LoadConfiguration();
 	
	//Check if was changes and update Alert Description string
	if (IsPCIActiveFlagChanged(bPCIActiveFlag))
	{
		strAlertDescription +=	szParameterNames[0];
		//return the last configuration to member
		m_bPCIActiveFlag	=	bPCIActiveFlag;
	}

	if (IsDSSActiveFlagChanged(bDSSActiveFlag))
	{
		strAlertDescription +=	szParameterNames[1];
		//return the last configuration to member
		m_bDSSActiveFlag	=	bDSSActiveFlag;
	}

	if(TRUE == IsEncodeActive()) 
		m_pcBaseEncodeHandler->ChangesResponse(strAlertDescription);

	if (strAlertDescription.IsEmpty() == FALSE)
	{
		SendChangesAlert(strAlertDescription);
	}
}



void CProtectedDataHandler::SendChangesAlert(CString& strAlertDescriptionOut)
{
	CString strLogMsg;
	strLogMsg.Format("Encoding, SendChangesAlert, AlarmValue=%ld AlarmDescription=%s", 0, strAlertDescriptionOut.GetBuffer(5));
	_LOGMSG.LogMsg(strLogMsg);

	_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE, SUB_PUMPSRV, PCI_PARAMETER_CHANGED, 0, ALARM_STARTED, 0, 0, strAlertDescriptionOut.GetBuffer(5));	
}


/************************************************************************/
/*                     Finalization Methods                             */
/************************************************************************/

void CProtectedDataHandler::CleanUp()
{
	if(NULL != m_pcBaseEncodeHandler)
	{
		m_pcBaseEncodeHandler->CleanUp();

		delete m_pcBaseEncodeHandler;
		m_pcBaseEncodeHandler = NULL;
	}
}


/************************************************************************/
/*               ConfigurationLoader overloaded Methods					*/
/************************************************************************/

//protected:		//4.0.22.43	TD 72716
BOOL CProtectedDataHandler::IsReInitRequired()
{
	BOOL	bRetCode		= FALSE;
	BOOL	bDSSActiveFlag  = m_bDSSActiveFlag;
	BOOL	bPCIActiveFlag  = m_bPCIActiveFlag;

	LoadConfiguration(FALSE);

	bRetCode = (IsDSSActiveFlagChanged(bDSSActiveFlag) || IsPCIActiveFlagChanged(bPCIActiveFlag)) ? TRUE : FALSE;

	return bRetCode;
}


//protected:		//4.0.22.43	TD 72716
void CProtectedDataHandler::ReInit()
{
	CleanUp();
	Init();				//The init calls connect	
}


inline CConfigurationLoader*  CProtectedDataHandler::GetChild()const
{ 
	return m_pcBaseEncodeHandler; 
}


//PRIVATE:
inline BOOL CProtectedDataHandler::IsDSSActiveFlagChanged(const BOOL bLastDSSActiveFlag)const
{
	BOOL	bRetCode = (bLastDSSActiveFlag != m_bDSSActiveFlag) ? TRUE : FALSE;

	return bRetCode;
}


inline BOOL	CProtectedDataHandler::IsPCIActiveFlagChanged(const BOOL bLastPCIActiveFlag)const
{
	BOOL	bRetCode = (bLastPCIActiveFlag != m_bPCIActiveFlag) ? TRUE : FALSE;

	return bRetCode;
}
