// DSSEncodeHandler.cpp: implementation of the CDSSEncodeHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSSEncodeHandler.h"
using namespace std;



//////////////////////////////////////////////////////////////////////
// Class CDSSEncodeHandler
//////////////////////////////////////////////////////////////////////

/************************************************************************/
/*                    Construction/Destruction                          */
/************************************************************************/

CDSSEncodeHandler::CDSSEncodeHandler(const CBasicProtectData& cBasicProtectData) :
		CBaseEncodeHandler(cBasicProtectData),
		m_pcBaseEncodingAlgorithm(nullptr)
{

}

CDSSEncodeHandler::~CDSSEncodeHandler()
{

}


/************************************************************************/
/*                     Initialization Methods							*/
/************************************************************************/

/******************************************************************************
 Description:	Initialize the DSSEncodeHandler
 Returns:      	ENCODE_DLL_OK - OK
				else - fail
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Start 4.0.20.501 TD-60236
******************************************************************************/

EncodeComDllRetCodeEnum CDSSEncodeHandler::Init()
{
    EncodeComDllRetCodeEnum eEncodeDLLRetCode = ENCODE_DLL_FAIL_INITIALIZATON;
	
	LoadConfiguration();

 	if (nullptr == m_pcBaseEncodingAlgorithm)
	{
 		if(LoadEncodingAlgorithm())
			eEncodeDLLRetCode = m_pcBaseEncodingAlgorithm->Init();
		else
			_LOGMSG.LogMsg("UNKNOWN ALGORITHM!!!", LOG_LEVEL_1);
	}

	return eEncodeDLLRetCode;
}
 	

//protected
BOOL CDSSEncodeHandler::LoadEncodingAlgorithm()
{
	BOOL	bRetCode = TRUE;
	CString cLogMsg = "LoadEncodingAlgorithm, ";

	switch(m_lDSSAlgorithm)
	{
	case ENCODE_ALGORITHM_ENCRYPTION:
		{
			switch(m_lDSSServiceInUse)
			{
			case DSS:
				{
					m_pcBaseEncodingAlgorithm = make_unique<CDSSEncryptAlg>();		
					cLogMsg += "DSSEncryptAlg was created.";
				}
				break;
			case DPS:
				{	
					m_pcBaseEncodingAlgorithm = make_unique<CDPSEncryptAlg>(); //USA Hy-vee
					cLogMsg += "DPSEncryptAlg was created.";
				}
			    break;
			case GENERIC_DPS:												// TD 329049
				{
					m_pcBaseEncodingAlgorithm = make_unique<CSADPSEncryptAlg>();
					cLogMsg += "CSADPSEncryptAlg was created.";
				} 
				break;
			default:
				{
					bRetCode = FALSE;
					cLogMsg += "Unlegal Data Security Service in use was created.";
				}
			    break;
			}
		}
		break;

	case ENCODE_ALGORITHM_HASH:
		{
			switch(m_lDSSServiceInUse)
			{
			case DSS:
				{
					m_pcBaseEncodingAlgorithm = make_unique<CDSSHashAlg>(); //SB
					cLogMsg += "DSSHashAlg was created.";
				}
				break;
			case OLA:
				{
					m_pcBaseEncodingAlgorithm = make_unique<COLAHashAlg>(); //SB
					cLogMsg += "OLAHashAlg was created.";
				}
				break;
			default:
				{
					bRetCode = FALSE;
					cLogMsg += "Unlegal Data Security Service in use was created.";
				}
				break;
			}
		}
		break;

	case ENCODE_ALGORITHM_ENCRYPTION_AND_HASH:
		{
			m_pcBaseEncodingAlgorithm = make_unique<CDSSBothAlg>(); //Old Tesco - not in use
			cLogMsg += "DSSBothAlg was created.";
		}
	    break;

	case ENCODE_ALGORITHM_ASYMMETRIC_ENCRYPTION_AND_HASH:
		{
			m_pcBaseEncodingAlgorithm = make_unique<CDSSBothAsymmetricAlg>(); //New Tesco
			cLogMsg += "DSSBothAsymmetricAlg was created.";
		}
	    break;
	default:
		{
			m_pcBaseEncodingAlgorithm = nullptr;
			bRetCode = FALSE;			
		}
	    break;
	}

	_LOGMSG.LogMsg(cLogMsg);

	return bRetCode;
}


/************************************************************************/
// ConfigurationLoader overloaded Methods	//4.0.22.13	TD 72716
/************************************************************************/
//protected:
void CDSSEncodeHandler::LoadConfiguration(const BOOL bCreate)
{
	CBaseEncodeHandler::LoadConfiguration(bCreate);
	
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "DSSServiceInUse",	(DWORD*)&m_lDSSServiceInUse,	(DWORD)0, bCreate);		//4.0.23.0 TD 76407
	_Module.m_server.m_cParam.LoadParam("GeneralParam", "DSSAlgorithm",		(DWORD*)&m_lDSSAlgorithm,		(DWORD)0, bCreate);
}


//protected:
BOOL CDSSEncodeHandler::IsReInitRequired()
{
	BOOL	bRetCode		= FALSE;
	long	lDSSAlgorithm	= m_lDSSAlgorithm;

	LoadConfiguration();

	bRetCode = (lDSSAlgorithm != m_lDSSAlgorithm) ? TRUE : FALSE;

	return bRetCode;
}


//protected:
void CDSSEncodeHandler::ReInit()
{
	CleanUp();
	Init();				//The init calls connect	
}

/************************************************************************/
/*         General Encoding\Decoding Methods							*/
/************************************************************************/


/******************************************************************************
 Description:	Encode feild in CARD_SALE_ALL3 struct
 Returns:      	ENCODE_OK - ok, else - fail
 Parameters:   	cCardSaleAll3InOut - CARD_SALE_ALL3 struct
				lNumber -	from GCI is pump number
							from DoAutoOLASendEnd is trs number

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Start 4.0.20.501 TD-60236
******************************************************************************/
EncodeRetCodeEnum CDSSEncodeHandler::EncodeForTrs(const short nTrsStatus, CARD_SALE_ALL3* pcCardSaleAll3InOut, const long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;

	if (m_pcBaseEncodingAlgorithm)
	{
		if (m_pcBaseEncodingAlgorithm->IsPAPEnabled())				//4.0.23.508 TD 93615
		{			
			if (m_pcBaseEncodingAlgorithm->IsAlgorithmEnabled(nTrsStatus))
			{
				char	szAccountToEncode[ENCRYPTED_DATA_SIZE];

				memset(szAccountToEncode, 0, sizeof(szAccountToEncode));
				memcpy(szAccountToEncode, pcCardSaleAll3InOut->CardSaleAll.data.sAccountNumber, min(sizeof(szAccountToEncode), sizeof(pcCardSaleAll3InOut->CardSaleAll.data.sAccountNumber)));

				ChangeSpaceCharToNull((BYTE*)szAccountToEncode, sizeof(szAccountToEncode));	//4.0.22.41 TD 74458

				if (strlen(szAccountToEncode) > 0)
				{
					eEncodeRetCode = m_pcBaseEncodingAlgorithm->Algorithm(pcCardSaleAll3InOut, szAccountToEncode, lTrsNumber);
				}
			}
		}
		else	//4.0.23.508 TD 93615
		{
			eEncodeRetCode = ENCODE_FAIL;
			_LOGMSG.LogMsg("DSSEncodeHandler, Can't allow PAP - EncodeForTrs CARD_SALE_ALL3 failed.", LOG_LEVEL_1);
		}
	}

	return eEncodeRetCode;
}


EncodeRetCodeEnum	CDSSEncodeHandler::EncodeForTrs(const short nTrsStatus, PAY_AT_PUMP_INFO* pcPayAtPumpInfoInOut, const long lPumpNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;

	if (m_pcBaseEncodingAlgorithm)
	{
		if (m_pcBaseEncodingAlgorithm->IsPAPEnabled())				//4.0.23.508 TD 93615
		{
			if(m_pcBaseEncodingAlgorithm->IsAlgorithmEnabled(nTrsStatus))
			{
				CARD_SALE_ALL3		cCardSaleAll3;
				PAY_AT_PUMP_INFO	cPAPInfo;
				const size_t		iPAPInfoSize = sizeof(cPAPInfo);

				memset(&cCardSaleAll3, ' ', sizeof(cCardSaleAll3));
				memset(&cPAPInfo, ' ', iPAPInfoSize);

				memcpy(&cPAPInfo, pcPayAtPumpInfoInOut, iPAPInfoSize);
				_Module.m_server.m_cCommonFunction.ConvertPayAtPumpInfoToCardSaleAll3(cCardSaleAll3, cPAPInfo);

				eEncodeRetCode = EncodeForTrs(nTrsStatus, &cCardSaleAll3, lPumpNumber);

				if ( ENCODE_OK == eEncodeRetCode)
				{
					_Module.m_server.m_cCommonFunction.ConvertCardSaleAll3ToPayAtPumpInfo(cPAPInfo, cCardSaleAll3);
					memcpy(pcPayAtPumpInfoInOut, &cPAPInfo, iPAPInfoSize);
				}
			}
		}
		else
		{
			eEncodeRetCode = ENCODE_FAIL;
			_LOGMSG.LogMsg("DSSEncodeHandler, Can't allow PAP - EncodeForTrs PAY_AT_PUMP_INFO failed.", LOG_LEVEL_1);
		}
	}

	return eEncodeRetCode;
}


// Decoding Methods	//4.0.23.20 TD 76407
EncodeRetCodeEnum  CDSSEncodeHandler::RestoreData(PAY_AT_PUMP_INFO* pcPayAtPumpInfoInOut, const long lTrsNumber)
{
	EncodeRetCodeEnum	eEncodeRetCode = ENCODE_OK;

	if (m_pcBaseEncodingAlgorithm)
	{
		CARD_SALE_ALL3		cCardSaleAll3;
		PAY_AT_PUMP_INFO	cPAPInfo;
		const size_t		iPAPInfoSize = sizeof(cPAPInfo);

		memset(&cCardSaleAll3, ' ', sizeof(cCardSaleAll3));
		memset(&cPAPInfo, ' ', iPAPInfoSize);

		memcpy(&cPAPInfo, pcPayAtPumpInfoInOut, iPAPInfoSize);
		_Module.m_server.m_cCommonFunction.ConvertPayAtPumpInfoToCardSaleAll3(cCardSaleAll3, cPAPInfo);

		if (m_pcBaseEncodingAlgorithm->IsRestoreDataEnabled(&cCardSaleAll3))
		{
			eEncodeRetCode = m_pcBaseEncodingAlgorithm->RestoreData(&cCardSaleAll3, lTrsNumber);

			if ( ENCODE_OK == eEncodeRetCode)
			{
				_Module.m_server.m_cCommonFunction.ConvertCardSaleAll3ToPayAtPumpInfo(cPAPInfo, cCardSaleAll3);
				memcpy(pcPayAtPumpInfoInOut, &cPAPInfo, iPAPInfoSize);
			}
		}
	}

	return eEncodeRetCode;
}


BOOL CDSSEncodeHandler::RemoveCardInfoFromMessage(char* psIn, long lInSize, char* psOut)const
{
	BOOL bRetCode = FALSE;

	if (m_pcBaseEncodingAlgorithm)
	{
		bRetCode = m_pcBaseEncodingAlgorithm->RemoveCardInfoFromMessage(psIn, lInSize, psOut);
	}
	else
	{
		bRetCode = CBaseEncodeHandler::RemoveCardInfoFromMessage(psIn, lInSize, psOut);
	}

	return bRetCode;
}



BOOL CDSSEncodeHandler::MaskAccount(char* psMaskedAccountNumberOut, const char* psOpenAccountNumber, const MaskTypeEnum eMaskType)
{
	BOOL bRetCode = FALSE;

	if (m_pcBaseEncodingAlgorithm)
	{
		bRetCode = m_pcBaseEncodingAlgorithm->MaskAccount(psMaskedAccountNumberOut, psOpenAccountNumber, eMaskType);
	}
	else
	{
		bRetCode = CBaseEncodeHandler::MaskAccount(psMaskedAccountNumberOut, psOpenAccountNumber, eMaskType);
	}

	return bRetCode;
}


/************************************************************************/
/*                    Receipt Index Methods                             */
/************************************************************************/


/******************************************************************************
 Description:	Get Receipt Index (key for receipt QDX) from PAY_AT_PUMP_INFO struct
 Returns:      	lRetIndexLen - Length of index
 Parameters:   	sIndexOUT - index
				pInfoIn	- PAY_AT_PUMP_INFO struct
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Start 4.0.20.501 TD-60236
******************************************************************************/
long CDSSEncodeHandler::GetReceiptIndex(char* psIndexOUT, const PAY_AT_PUMP_INFO* pcPayAtPumpInfoIn, const long lPumpNumber)
{
	if (m_pcBaseEncodingAlgorithm)
		return m_pcBaseEncodingAlgorithm->GetReceiptIndex(psIndexOUT, pcPayAtPumpInfoIn, lPumpNumber);
	else
		return 0;
}


/******************************************************************************
 Description:	This method returns the qdx key for the receipt.
 Returns:      	sIndexOut
 Parameters:   	sTrack2In , lFlags
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Start 4.0.20.501 TD-60236 
 Gena			19/07/2009	 15:14		update //4.0.23.20 TD 77536
******************************************************************************/
long CDSSEncodeHandler::GetReceiptIndex(char* psIndexOut, const BSTR wsTrack2In, const long lFlags, const long lPumpNumber)	
{
	long lRetIndexLen = 0;

	if (m_pcBaseEncodingAlgorithm)
		lRetIndexLen = m_pcBaseEncodingAlgorithm->GetReceiptIndex(psIndexOut, wsTrack2In, lFlags, lPumpNumber);	

	return lRetIndexLen;
}


/******************************************************************************
 Description:	get PAY_AT_PUMP_INFO and Encode the data
					  the new account Encode will insert into
					  track1,track2,extrafield
					  after it we will Encode the account with our 
					  Encoding 	
 Parameters:	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
Gena			24/08/08				start //4.0.20.502 (2)
******************************************************************************/

void	CDSSEncodeHandler::GenerateIndex(PAY_AT_PUMP_INFO* pPAPInfo, const BOOL bStoreAccountNum, const long lTrsNumber)
{
	if (pPAPInfo != NULL)
	{
		if(ChrAll( (char*)(pPAPInfo->CardSaleExtraData4.sEncryptedAccount), sizeof(pPAPInfo->CardSaleExtraData4.sEncryptedAccount), ' ') )	
		{			
			long lTrsNumLocal = (lTrsNumber == 0) ? a2l(pPAPInfo->CardSaleInfo.cardData.sTranNbr, sizeof(pPAPInfo->CardSaleInfo.cardData.sTranNbr)) : lTrsNumber;	//4.0.28.500 TD 243638
			EncodeForTrs(SESSION_COMPLETE | SESSION_AUTHORIZE, pPAPInfo, lTrsNumLocal);	
		}
	}	
}


//Gena			01/07/2010	21:00		 //4.0.25.30 
void	CDSSEncodeHandler::GenerateIndex(CARD_SALE_ALL3* pcCardSaleAll3InOut, const BOOL bStoreAccountNum, const long lTrsNumber)
{
	if (pcCardSaleAll3InOut != NULL)
	{
		if(ChrAll( (char*)(pcCardSaleAll3InOut->extData4.sEncryptedAccount), sizeof(pcCardSaleAll3InOut->extData4.sEncryptedAccount), ' ') )	
		{			
			long lTrsNumLocal = (lTrsNumber == 0) ? a2l(pcCardSaleAll3InOut->CardSaleAll.data.sTranNbr, sizeof(pcCardSaleAll3InOut->CardSaleAll.data.sTranNbr)) : lTrsNumber;	//4.0.28.500 TD 243638
			EncodeForTrs(SESSION_COMPLETE | SESSION_AUTHORIZE, pcCardSaleAll3InOut, lTrsNumLocal);	
		}
	}	
}

//4.0.20.507
void	CDSSEncodeHandler::RemoveConfidencesData(PAY_AT_PUMP_INFO* pInfoInOut)
{
	CARD_SALE_ALL3		cCardSaleAll3;
	PAY_AT_PUMP_INFO	cPAPInfo;

	memset(&cCardSaleAll3, ' ', sizeof(cCardSaleAll3));
	memset(&cPAPInfo, ' ', sizeof(cPAPInfo));

	memcpy(&cPAPInfo, pInfoInOut, sizeof(cPAPInfo));

	_Module.m_server.m_cCommonFunction.ConvertPayAtPumpInfoToCardSaleAll3(cCardSaleAll3, cPAPInfo);

	// 4.0.28.52 TD 264447
	if(NULL != m_pcBaseEncodingAlgorithm)
	{
		m_pcBaseEncodingAlgorithm->RemoveConfidencesData(&cCardSaleAll3);
	}
	
	RemoveConfidencesData(&cCardSaleAll3);

	_Module.m_server.m_cCommonFunction.ConvertCardSaleAll3ToPayAtPumpInfo(cPAPInfo, cCardSaleAll3);

	memcpy(pInfoInOut, &cPAPInfo, sizeof(cPAPInfo));
}


//4.0.20.507
void	CDSSEncodeHandler::RemoveConfidencesData(CARD_SALE_ALL3* pCardSaleAll3InOut)
{
	if( pCardSaleAll3InOut->extData4.byIsRemoveProhibitedData == 'Y' || m_bRemoveProhibitedData )	//4.0.24.60	TD 106586	
	{
		if (NULL != m_pcBaseEncodingAlgorithm && m_pcBaseEncodingAlgorithm->IsMaskingAccountRequired(pCardSaleAll3InOut, MASK_TYPE_DB) == TRUE)
		{
			char	sMaskedAccount[ACCOUNT_NUM_SIZE];
			memset(sMaskedAccount, ' ', sizeof(sMaskedAccount));

			MaskAccount(sMaskedAccount, (char*)pCardSaleAll3InOut->CardSaleAll.data.sAccountNumber, MASK_TYPE_DB);	

			memcpy(pCardSaleAll3InOut->CardSaleAll.data.sAccountNumber, sMaskedAccount, sizeof(sMaskedAccount));

			if(LOG_BASIC_FLOW_CONTROL)
			{			
				_LOGMSG.LogClassMsg("CDSSEncodeHandler","RemoveConfidencesData", 0, LOG_NONE, "MaskAccount for DB");
			}

		}


	// 4.0.28.52 TD 264447
		if(NULL != m_pcBaseEncodingAlgorithm)
		{
			 m_pcBaseEncodingAlgorithm->RemoveConfidencesData(pCardSaleAll3InOut);
		}
 	    CBaseEncodeHandler::RemoveConfidencesData(pCardSaleAll3InOut);


		if(!m_bExcludesLoyalty)	
		{
			memset(pCardSaleAll3InOut->CardSaleAll.data.sClubCardTrack2Data, ' ', sizeof(pCardSaleAll3InOut->CardSaleAll.data.sClubCardTrack2Data));	
		}

		if(LOG_BASIC_FLOW_CONTROL)
		{			
			_LOGMSG.LogClassMsg("CDSSEncodeHandler","RemoveConfidencesData", 0, LOG_NONE, "RemoveConfidencesData for DB");
		}
	}
} 


void CDSSEncodeHandler::ChangesResponse(CString& strAlertDescriptionOut)
{
	const size_t	iParameterNumber = 2;
	const char		szParameterNames[iParameterNumber][MAX_NAME_LEN] = {"DSSAlgorithm,",
																		"PCIExludesLoyalty,",};

	long	lDSSAlgorithm = m_lDSSAlgorithm;
	BOOL	bPCIExcludesLoyalty = CBaseEncodeHandler::GetExcludesLoyalty();

	LoadConfiguration();

	if (IsDSSAlgorithmChanged(lDSSAlgorithm) == TRUE)
	{
		strAlertDescriptionOut += szParameterNames[0];
		m_lDSSAlgorithm = lDSSAlgorithm;
	}

	if (IsPCIExcludesLoyaltyChanged(bPCIExcludesLoyalty) == TRUE)
	{
		strAlertDescriptionOut += szParameterNames[1];
		CBaseEncodeHandler::SetExcludesLoyalty(bPCIExcludesLoyalty);
	}

	if (m_pcBaseEncodingAlgorithm)
	{
		m_pcBaseEncodingAlgorithm->ChangesResponse(strAlertDescriptionOut);
	}
}


/************************************************************************/
/*                Finalization Methods                                  */
/************************************************************************/

/******************************************************************************
 Description:	Clean Up
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Gena			17/08/2008	 13:34		Start 4.0.20.501 TD-60236
******************************************************************************/
void CDSSEncodeHandler::CleanUp()
{
	if(m_pcBaseEncodingAlgorithm)
	{
		m_pcBaseEncodingAlgorithm->CleanUp(); 
		m_pcBaseEncodingAlgorithm.release();
		m_pcBaseEncodingAlgorithm = NULL;
	}
}


BOOL CDSSEncodeHandler::IsPAPEnabled()
{
	return ((NULL != m_pcBaseEncodingAlgorithm) ? m_pcBaseEncodingAlgorithm->IsPAPEnabled() : TRUE);
}

void CDSSEncodeHandler::RemoveConfidencesDataFromMemory(short bySessionType, PAY_AT_PUMP_INFO* pInfoInOut)
{
	if (m_pcBaseEncodingAlgorithm)
		m_pcBaseEncodingAlgorithm->RemoveConfidencesDataFromMemory(bySessionType, pInfoInOut);
}

//4.0.26.500 TD 144219
BOOL CDSSEncodeHandler::IsLookForInternalReceiptIndex()
{
	BOOL bRetCode = FALSE;

	if (m_pcBaseEncodingAlgorithm)
	{
		bRetCode = m_pcBaseEncodingAlgorithm->IsLookForInternalReceiptIndex();
	}

	return bRetCode;
}



//4.0.27.501 TD 233308
void CDSSEncodeHandler::SetNoneOPTReceiptAccountMask(PAY_AT_PUMP_INFO* pcPayAtPumpInfo, const char* pszAccountNum)const
{
	if (m_pcBaseEncodingAlgorithm)
		m_pcBaseEncodingAlgorithm->SetNoneOPTReceiptAccountMask(pcPayAtPumpInfo, pszAccountNum);
}