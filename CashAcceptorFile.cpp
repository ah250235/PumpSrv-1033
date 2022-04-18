// CashAcceptorFile.cpp: implementation of the CCashAcceptorFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CashAcceptorFile.h"
#include <vector>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCashAcceptorFile::CCashAcceptorFile():CIndexFile(CASH_ACCEPTOR_FILE,CASH_ACCEPTOR_FILE_SIZE) //4.0.11.10
{
	InitializeCriticalSection(&m_csCashAcceptorFile);
}

CCashAcceptorFile::~CCashAcceptorFile()
{
	DeleteCriticalSection(&m_csCashAcceptorFile);
}
/******************************************************************************
 Description:	Update bills into data base
 Returns:      	
 Parameters:   	lTerminalId - terminal Id , 
				cExtraInfoRec - Cash acceptor transaction detalis.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			19/7/2005   17:37		Start
******************************************************************************/

long CCashAcceptorFile::WriteRecord(long *lTerminalId, CASH_ACCEPTOR_REC *cCashRec)
{
	CASH_ACCEPTOR_REC cCashAcceptorRec;

	long lRtc = 0 ;

	m_csFile.Lock();

	lRtc = Operation(REC_READ, &cCashAcceptorRec);

	if(lRtc)
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("Terminal %02ld add new cash acceptor record" ,*lTerminalId);
			_LOGMSG.LogMsg(str,LOG_LEVEL_0);
		}

		cCashAcceptorRec.lCounter = 0;
		cCashAcceptorRec.lAmount= 0;;

	}
	
	//Check if record alreaady set , if no clear the record and set the terminal.
	if (cCashAcceptorRec.lTerminalId  != *lTerminalId)
	{
		memset(&cCashAcceptorRec , 0 , sizeof(cCashAcceptorRec));
		cCashAcceptorRec.lTerminalId = *lTerminalId;
	}

	cCashAcceptorRec.lCounter +=  cCashRec->lCounter;
	cCashAcceptorRec.lAmount += cCashRec->lAmount;

						 
	lRtc = Operation(REC_UPDATE, &cCashAcceptorRec);							

	m_csFile.Unlock();		

	return lRtc;

}

/******************************************************************************
 Description:	Reads bills from data base
 Returns:      	
 Parameters:   	lTerminalId - terminal Id , 
				cExtraInfoRec - Cash acceptor transaction detalis.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			19/7/2005   17:37		Start
******************************************************************************/
long CCashAcceptorFile::ReadRecord(long * lTerminalId, CASH_ACCEPTOR_REC  * cRes)
{

	return 0;
}

/******************************************************************************
 Description:	Move current cash acceptor shift to history part.
 Returns:      	
 Parameters:   	
				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			19/7/2005   17:37		Start
******************************************************************************/

void CCashAcceptorFile::MoveShift()
{
	CASH_ACCEPTOR_REC cCashAcceptorRec;
	CASH_ACCEPTOR_REC cCashAcceptorRecTmp;
	DWORD dwOffset = 0;
	long lRetCode = 0 ;
	CString str;

	long lRtc=0;
	BYTE byPeriodRequest = ' ';

	EnterCriticalSection(&m_csCashAcceptorFile);


	str.Format("CCashAcceptorFile move shift");
	_LOGMSG.LogMsg(str);

	DeleteOLAPeriod();
	

	CreateFindFirstIndex(0 ,cCashAcceptorRec.cIndex.cGeneral.sKeyIndex); //4.0.11.138
	lRetCode = Operation(REC_READ_FIRST , &cCashAcceptorRec);  //4.0.11.10

	while (lRetCode == OK)
	{
		if (cCashAcceptorRec.cIndex.cInfo.byPeriod == ' ' )
		{
			cCashAcceptorRecTmp = cCashAcceptorRec;
			cCashAcceptorRecTmp.cIndex.cInfo.byPeriod = '1';
			
			lRetCode = Operation(REC_INSERT , &cCashAcceptorRecTmp); 

			if (lRetCode)
			{
				str.Format("Terminal %02ld ,addd new record to history period has failed, retCode=%ld" ,cCashAcceptorRecTmp.lTerminalId,lRetCode);
				_LOGMSG.LogMsg(str);
			}
	
			lRetCode = Operation(REC_DELETE , &cCashAcceptorRec);  

			lRetCode = Operation(REC_READ_FIRST , &cCashAcceptorRec);  

			
		}
		else
			break;
		
	}

	WriteHeaderFile();

	LeaveCriticalSection(&m_csCashAcceptorFile);
}

/******************************************************************************
 Description:	Update hader with current date
 Returns:      	
 Parameters:   	
				
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			19/7/2005   17:37		Start
******************************************************************************/

long CCashAcceptorFile::WriteHeaderFile()
{

	CASH_ACCEPTOR_HEADER_REC cCashAcceptorHeaderRec;
	DWORD dwOffset =  0;
	long lRtc = 0 ;
	SYSTEMTIME	SysTime;
	GetLocalTime(&SysTime);

	memset(&cCashAcceptorHeaderRec , 0 , sizeof(cCashAcceptorHeaderRec));
	CreateCurrentIndex(0 ,cCashAcceptorHeaderRec.cIndex.cGeneral.sKeyIndex, TRUE ,FALSE);
	cCashAcceptorHeaderRec.lTerminalId = 0;
	cCashAcceptorHeaderRec.sValid = '1';

	SystemTime2Tag(&SysTime, &cCashAcceptorHeaderRec.sDateAndTime);	
							
	lRtc = Operation(REC_INSERT, &cCashAcceptorHeaderRec);		

	return lRtc;

}

long CCashAcceptorFile::ReadHeaderFile(CASH_ACCEPTOR_HEADER_REC &cCashAcceptorHeaderRec)
{
	long lRtc;
	CreateCurrentIndex(0 ,cCashAcceptorHeaderRec.cIndex.cGeneral.sKeyIndex, TRUE ,FALSE);
	lRtc = Operation(REC_READ, &cCashAcceptorHeaderRec);
	return lRtc;

}

long CCashAcceptorFile::CheckSignitureFile()
{
	CASH_ACCEPTOR_HEADER_REC   cCashAcceptorHeaderRec;
	long lRtc;

	CreateCurrentIndex(0, cCashAcceptorHeaderRec.cIndex.cGeneral.sKeyIndex, TRUE, FALSE);

	lRtc = Operation(REC_READ, &cCashAcceptorHeaderRec);

	if (lRtc != OK && lRtc != ERR_INDX_NOT_LOADED)			//4.0.25.90 TD 143987
	{
		if (cCashAcceptorHeaderRec.sValid != '1')
		{
			WriteHeaderFile();
		}
	}

	return 0;
}

void CCashAcceptorFile::ClearHistoryParameters(CASH_ACCEPTOR_REC &cCashAcceptorRec)
{
	cCashAcceptorRec.lAmount = 0;
	cCashAcceptorRec.lCounter =0;
}

long CCashAcceptorFile::ParseBinFile(char * sFileName) //33898
{
	CASH_ACCEPTOR_QDX_MAINT_REC maintRec;
	memset(&maintRec, 0, sizeof(CASH_ACCEPTOR_QDX_MAINT_REC));
	BOOL bRetVal = ReadBinFile(sFileName,&maintRec,sizeof(CASH_ACCEPTOR_QDX_MAINT_REC));
	if (bRetVal)
	{
		CString str;
		str.Format("QDXMaint - CCashAcceptorFile::ParseBinFile file = %s",sFileName); //4.0.18.40
		_LOGMSG.LogMsg(str);
		Operation(maintRec.opCode, &maintRec.cashAccRec, TRUE);
	}
	return bRetVal;
}

long CCashAcceptorFile::Operation(eFileOperation OpCode, void * pData, BOOL bIsSaveByMaint)
{
	// RFUEL-292 
// 	CASH_ACCEPTOR_REC		*pRec = NULL;
// 	CASH_ACCEPTOR_REC		cRecWork;
// 	long lRtc = 1 ;	
// 
// 	long				lRetVal= 0;	
// 
// 	long lPCNumber = 0 ;
// 	long lTimeStampOffset = 0;
// 
// 	if(pData)
// 		pRec  = (CASH_ACCEPTOR_REC *)pData;
// 
// 	if (m_bInMiddleOfQDXMaintFlag && !bIsSaveByMaint) //4.0.18.20 - CR 33898
// 	{
// 		CASH_ACCEPTOR_QDX_MAINT_REC cashAccRec;
// 		memset (&cashAccRec,0,sizeof(CASH_ACCEPTOR_QDX_MAINT_REC));
// 		if (pRec)
// 			cashAccRec.cashAccRec = *pRec;
// 		cashAccRec.opCode = OpCode;
// 		lRetVal = SaveQDXMaintFile(OpCode, &cashAccRec, sizeof(CASH_ACCEPTOR_QDX_MAINT_REC));
// 	}
// 	else
// 	{
// 
// 		memset (&cRecWork,0,sizeof(CASH_ACCEPTOR_REC));
// 		m_csFile.Lock();
// 		
// 		switch (OpCode)	
// 		{
// 			case REC_INSERT:	
// 				cRecWork = *pRec;			
// 				lRetVal = Insert(&cRecWork,HOP_DEFAULT_CONFIG,lPCNumber,lTimeStampOffset);			
// 				break;
// 
// 			case REC_READ:
// 				cRecWork = *pRec;			
// 				lRetVal = Read(&cRecWork, HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			
// 
// 				if (pData)		//4.0.11.12
// 					*pRec = cRecWork;
// 
// 				break;
// 
// 			case REC_UPDATE:
// 				cRecWork = *pRec;
// 				lRetVal = Update(&cRecWork,HOP_DEFAULT_CONFIG,lPCNumber,lTimeStampOffset);			
// 
// 				break;
// 
// 			case REC_DELETE:
// 				cRecWork = *pRec;
// 				lRetVal = Delete(&cRecWork,HOP_DEFAULT_CONFIG,lPCNumber,lTimeStampOffset);			
// 				break;
// 
// 			case REC_READ_FIRST:
// 				cRecWork = *pRec;
// 				lRetVal = ReadFirst(&cRecWork, HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			
// 				if(lRetVal == Q_ERROR)
// 					lRetVal = ERR_NOT_FOUND;
// 				else
// 					if (pData)		//4.0.11.12
// 						*pRec = cRecWork;
// 
// 				break;
// 
// 			case REC_READ_NEXT:
// 				cRecWork = *pRec;			
// 				lRetVal = ReadNext(&cRecWork, HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			
// 
// 				if(lRetVal == Q_ERROR)
// 					lRetVal = ERR_NOT_FOUND;
// 				else
// 					if (pData)		//4.0.11.12
// 						*pRec = cRecWork;
// 
// 				break;
// 
// 			case REC_DELETE_TILL_INDEX:
// 				break;
// 		}
// 	m_csFile.Unlock();
// 	}
// 	return lRetVal;
	return 0;
}

BOOL CCashAcceptorFile::CheckRemovalDrawers(char *sOpenDrawersList)
{
	CASH_ACCEPTOR_REC cCashAcceptorRec;
	BOOL bRet= TRUE;	
	long lRtc = 0;

	CString  sTmpOpenDrawersList ='[';
	char szTmpBuffer[MAX_FIELD_VAL]; 

	memset(szTmpBuffer , 0, sizeof(szTmpBuffer));

	if(LOG_BASIC_FLOW_CONTROL)
	{
		CString str;
		str.Format("CheckRemovalDrawers");
		_LOGMSG.LogMsg(str);
	}
	
	for(long i=1 ; i <=MAX_CASH_ACCEPTOR_RECORD_SHIFT ; i++)
	{
		CreateFindFirstIndex(i ,cCashAcceptorRec.cIndex.cGeneral.sKeyIndex);
		lRtc = Operation(REC_READ_FIRST , &cCashAcceptorRec);  //4.0.11.10
		
		if ((cCashAcceptorRec.lAmount) && (cCashAcceptorRec.cIndex.cInfo.byPeriod == ' ' ) && 
			(cCashAcceptorRec.lTerminalId == i))
		{
			if (cCashAcceptorRec.byDrawerRemovalStatus != '1')
			{
				//4.0.17.502 - TD 48819
				sprintf(szTmpBuffer , "%ld," ,i);
				sTmpOpenDrawersList += szTmpBuffer;
				
				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;
					str.Format("Terminal %02ld drawer not removed", i);
					_LOGMSG.LogMsg(str);
				}

				bRet = FALSE;
			}
		}

	}

	if (sTmpOpenDrawersList.GetLength())	//4.0.17.502 - TD 48819
		sTmpOpenDrawersList.Delete(sTmpOpenDrawersList.GetLength() -1 , 1);

	sTmpOpenDrawersList+= ']';
	memcpy(sOpenDrawersList ,sTmpOpenDrawersList , sTmpOpenDrawersList.GetLength());

	return bRet;
}

void sort_array(std::vector<CASH_ACCEPTOR_REC> &array)
{
	int cmp_res;
	
	for (size_t i = 0; i < array.size(); ++i)
	{
		for (size_t j = 0; j < array.size(); ++j)
		{
			cmp_res = strcmp((char *)array[i].cIndex.cInfo.sDateYYMMDD, (char *)array[j].cIndex.cInfo.sDateYYMMDD);
			
			if (cmp_res < 0)
			{
				std::swap(array[i], array[j]);
			}
		}	
	}
}

long CCashAcceptorFile::GetCashAcceptorReport__(long lTerminalId, long lFlags, BSTR *sHeader, BSTR *sRecords, long *lRecordCount)
{
	CASH_ACCEPTOR_REC cCashAcceptorRec;
	CASH_ACCEPTOR_REC cTmpTotalCashAcceptorRec;
	CASH_ACCEPTOR_HEADER_REC cCashAcceptorHeaderRec;
	TAG_CASH_ACCEPTOR_HEADER  cTagHead; 
	TAG_CASH_ACCEPTOR        cTag; 
	char	sTmp[sizeof(TAG_CASH_ACCEPTOR) +2];
	long	lCounter = 0;
	CComBSTR bstrCashAcceptorTrs;
	BOOL bFound = FALSE;
	CASH_ACCEPTOR_INDEX	cCashAcceptorIndex;
	long lRtc=0;
	BYTE byPeriodRequest = ' ';
	BOOL bIndexSearch = TRUE;
	long lQdxTotalRecords =0;
	std::vector<CASH_ACCEPTOR_REC> array;
	char     szOpenDrawerList[MAX_FIELD_VAL];

	*lRecordCount = 0;

	long lRecordsSize= sizeof(CASH_ACCEPTOR_REC);
	
	GCIRetCode retCode = GCI_OK;

	if (lFlags & GET_CASH_ACCEPTOR_REPORT_HISTORY)  //4.0.11.12
	{
		byPeriodRequest  = '1';
		bIndexSearch = FALSE;

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("Attempt to get history report");
			_LOGMSG.LogMsg(str);
		}

	}

	bstrCashAcceptorTrs.Empty();

	memset(sTmp , 0, sizeof(sTmp));
	memset(szOpenDrawerList , 0 , sizeof(szOpenDrawerList));

	if (CheckRemovalDrawers(szOpenDrawerList))
	{
		memset(sTmp , 0, sizeof(sTmp));

		CreateFindFirstIndex(0 ,cCashAcceptorHeaderRec.cIndex.cGeneral.sKeyIndex ,bIndexSearch);  //4.0.11.138
		lRtc = Operation(REC_READ_FIRST , &cCashAcceptorHeaderRec);  //4.0.11.10 //4.0.11
		
		if ((!lRtc) && (cCashAcceptorHeaderRec.cIndex.cInfo.byPeriod == byPeriodRequest) && (cCashAcceptorHeaderRec.lTerminalId == 0))   //4.0.11.10
		{
			ConvertCahAcceptorRec2TagHead(cCashAcceptorHeaderRec, cTagHead);
		}
		else
		{
			memset(&cTagHead ,' ' , sizeof(cTagHead));

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Cash acceptor header record nor found, use default value !!!!");
				_LOGMSG.LogMsg(str);
			}

		}
		
		memcpy((BYTE*)sTmp , &cTagHead , sizeof(cTagHead));
		CComBSTR bstrCashAcceptorHeader(sizeof(sTmp) , sTmp);
		*sHeader = bstrCashAcceptorHeader.Copy();

		memset(&cTmpTotalCashAcceptorRec , 0 , sizeof(cTmpTotalCashAcceptorRec)); //4.0.11.135
		// find all valid drawers per terminal.
		for( long  i =1 ; i <= MAX_CASH_ACCEPTOR_RECORD_SHIFT; i++)
		{
			
			bFound = FALSE;

			CreateFindFirstIndex(i ,cCashAcceptorRec.cIndex.cGeneral.sKeyIndex,bIndexSearch);
			lRtc = Operation(REC_READ_FIRST , &cCashAcceptorRec);  //4.0.11.10

			while (!lRtc)
			{
				if ((cCashAcceptorRec.lAmount) && (cCashAcceptorRec.cIndex.cInfo.byPeriod == byPeriodRequest ) && 
						(cCashAcceptorRec.lTerminalId == i))
				{
					bFound = TRUE;

					cTmpTotalCashAcceptorRec.lAmount +=  cCashAcceptorRec.lAmount;
					cTmpTotalCashAcceptorRec.lCounter += cCashAcceptorRec.lCounter;
					cTmpTotalCashAcceptorRec.lRemovalCounter += cCashAcceptorRec.lRemovalCounter; //4.0.11.135
					cTmpTotalCashAcceptorRec.lTerminalId = i;
					memcpy(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD , cCashAcceptorRec.cIndex.cInfo.sDateYYMMDD , sizeof(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD));  //4.0.11.135

					array.push_back(cCashAcceptorRec); //4.0.11.138
					lQdxTotalRecords++; //4.0.11.133

				}
				else
				{

					break;   // we arrived to end of list
				}

				lRtc = Operation(REC_READ_NEXT , &cCashAcceptorRec);  //4.0.11.10
			}

			if (bFound)
			{
				ConvertCahAcceptorRec2TagRec(cTmpTotalCashAcceptorRec, cTag);

				memset(&cTmpTotalCashAcceptorRec , 0 , sizeof(cTmpTotalCashAcceptorRec)); //4.0.11.135
				memset(sTmp , 0, sizeof(sTmp));
				memcpy((BYTE*)sTmp , &cTag , sizeof(cTag));
				bstrCashAcceptorTrs.Append((LPCSTR)sTmp);
				lCounter++;


				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;

					str.Format("Cash acceptor raw:: counter=%ld [%s]",  lCounter ,sTmp);  //4.0.11.115
					_LOGMSG.LogMsg(str);
				}

			}

		}


	
		if (lQdxTotalRecords)
		{
			sort_array(array);
			cCashAcceptorIndex = array[0].cIndex;
			memset(&cTmpTotalCashAcceptorRec , 0 , sizeof(cTmpTotalCashAcceptorRec));
		}


		bFound = FALSE;

		if (array.size())
		{
			cTmpTotalCashAcceptorRec.lAmount =  array[0].lAmount;
			cTmpTotalCashAcceptorRec.lCounter = array[0].lCounter; //4.0.11.135
			memcpy(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD ,array[0].cIndex.cInfo.sDateYYMMDD , sizeof(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD)); //4.0.11.135
			cTmpTotalCashAcceptorRec.lRemovalCounter += array[0].lRemovalCounter;
			bFound = TRUE;

		}

		for (size_t i=1 ; i < array.size() ; i++)
		{		
			if (!memcmp(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD , array[i].cIndex.cInfo.sDateYYMMDD,sizeof(cCashAcceptorIndex.cInfo.sDateYYMMDD)))
			{
				bFound = TRUE;

				cTmpTotalCashAcceptorRec.lAmount +=  array[i].lAmount;
				cTmpTotalCashAcceptorRec.lCounter += array[i].lCounter; //4.0.11.135
				memcpy(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD ,array[i].cIndex.cInfo.sDateYYMMDD , sizeof(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD)); //4.0.11.135
				cTmpTotalCashAcceptorRec.lRemovalCounter += array[i].lRemovalCounter;
			}
			else
			{

				if (bFound)
				{
					ConvertCahAcceptorRec2TagRec(cTmpTotalCashAcceptorRec, cTag, '1');

					memset(sTmp , 0, sizeof(sTmp));
					memcpy((BYTE*)sTmp , &cTag , sizeof(cTag));
					bstrCashAcceptorTrs.Append((LPCSTR)sTmp);
					lCounter++;

					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;

						str.Format("Cash acceptor total raw:: counter=%ld [%s]",  lCounter ,sTmp );  //4.0.11.115
						_LOGMSG.LogMsg(str);
					}

					memset(&cTmpTotalCashAcceptorRec , 0 , sizeof(cTmpTotalCashAcceptorRec)); //4.0.11.135

					bFound = TRUE;

					cTmpTotalCashAcceptorRec.lAmount +=  array[i].lAmount;
					cTmpTotalCashAcceptorRec.lCounter += array[i].lCounter; //4.0.11.135
					memcpy(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD ,array[i].cIndex.cInfo.sDateYYMMDD , sizeof(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD)); //4.0.11.135
					cTmpTotalCashAcceptorRec.lRemovalCounter += array[i].lRemovalCounter;

				}

			}

		}

		if (bFound)
		{
			ConvertCahAcceptorRec2TagRec(cTmpTotalCashAcceptorRec, cTag, '1');

			memset(sTmp , 0, sizeof(sTmp));
			memcpy((BYTE*)sTmp , &cTag , sizeof(cTag));
			bstrCashAcceptorTrs.Append((LPCSTR)sTmp);
			lCounter++;

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;

				str.Format("Cash acceptor total reports raw:: counter=%ld [%s]", lCounter ,sTmp );  //4.0.11.115
				_LOGMSG.LogMsg(str);
			}

			memset(&cTmpTotalCashAcceptorRec , 0 , sizeof(cTmpTotalCashAcceptorRec)); //4.0.11.135
		}

	}
	else
	{
		//4.0.11.115
		CComBSTR bstrOpenDrawerList(min(strlen(szOpenDrawerList) + 1, sizeof(szOpenDrawerList)) , szOpenDrawerList);	//4.0.19.502

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("Open drawers list is: %s" ,szOpenDrawerList);
			_LOGMSG.LogMsg(str);
		}

		*sHeader = bstrOpenDrawerList.Copy();	
		retCode = GCI_NOT_ALL_DRAWERS_REMOVED;
	}
		
	
	*lRecordCount =lCounter;
	*sRecords = bstrCashAcceptorTrs.Copy();

	return retCode;
}

long CCashAcceptorFile::GetCashAcceptorReportXml__(long lTerminalId, long lFlags, BSTR *sHeader, BSTR *sXmlData, long *lRecordCount, BOOL bCheckRemovalDrawers) //4.0.17.501
{
	GCIRetCode retCode = GCI_OK;

	CASH_ACCEPTOR_REC cCashAcceptorRec;
	CASH_ACCEPTOR_REC cTmpTotalCashAcceptorRec;
	CASH_ACCEPTOR_HEADER_REC cCashAcceptorHeaderRec;
	long	lCounter = 0;
	BOOL bFound = FALSE;
	CASH_ACCEPTOR_INDEX	cCashAcceptorIndex;
	long lRtc=0;
	BYTE byPeriodRequest = ' ';
	BOOL bIndexSearch = TRUE;
	long lQdxTotalRecords =0;
	std::vector<CASH_ACCEPTOR_REC> array;
	BOOL bRetCheckRemovalDrawers = FALSE;
	char szOpenDrawerList[MAX_FIELD_VAL];
		
	CXMLInterface	cXmlCashAcceptorReport;
	CXMLPointers	cXMLPicReportPointers;
	CXMLPointers	cXMLTerminalsSection;
	CXMLPointers	cXMLDaysTotalSection;

	*lRecordCount = 0;

	long lRecordsSize= sizeof(CASH_ACCEPTOR_REC);
	

	if (cXmlCashAcceptorReport.InitDoc())
	{
		cXmlCashAcceptorReport.CreateElement("PICReport",&cXMLPicReportPointers);
		cXmlCashAcceptorReport.AppendRootElement(&cXMLPicReportPointers);
	}
		


	if (lFlags & GET_CASH_ACCEPTOR_REPORT_HISTORY)  //4.0.11.12
	{
		byPeriodRequest  = '1';
		bIndexSearch = FALSE;

		if(LOG_BASIC_FLOW_CONTROL)
		{
			CString str;
			str.Format("Attempt to get history report");
			_LOGMSG.LogMsg(str);
		}

	}
	
	if (bCheckRemovalDrawers)	//4.0.17.501
	{
		memset(szOpenDrawerList , 0 , sizeof(szOpenDrawerList));
		if (!(bRetCheckRemovalDrawers = CheckRemovalDrawers(szOpenDrawerList)))
		{
			//4.0.17.502 - TD 48819
			CComBSTR bstrOpenDrawerList(strlen(szOpenDrawerList) + 1 , szOpenDrawerList);

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Open drawers list is: %s" ,szOpenDrawerList);
				_LOGMSG.LogMsg(str);
			}

			*sHeader = bstrOpenDrawerList.Copy();	

			retCode = GCI_NOT_ALL_DRAWERS_REMOVED;
		}
	}	
	
	if (!retCode && (!bCheckRemovalDrawers || bRetCheckRemovalDrawers)) //4.0.17.501
	{
		CreateFindFirstIndex(0 ,cCashAcceptorHeaderRec.cIndex.cGeneral.sKeyIndex ,bIndexSearch);
		lRtc = Operation(REC_READ_FIRST , &cCashAcceptorHeaderRec);  
		
		if ((!lRtc) && (cCashAcceptorHeaderRec.cIndex.cInfo.byPeriod == byPeriodRequest) && (cCashAcceptorHeaderRec.lTerminalId == 0))   //4.0.11.10
		{
			//4.0.18.504 Merge from 4.0.17.220 
			if ((!a2l(cCashAcceptorHeaderRec.sEndDateAndTime.sDay,sizeof(cCashAcceptorHeaderRec.sEndDateAndTime.sDay))) || (!a2l(cCashAcceptorHeaderRec.sEndDateAndTime.sMonth,sizeof(cCashAcceptorHeaderRec.sEndDateAndTime.sMonth)))||
				(!a2l(cCashAcceptorHeaderRec.sEndDateAndTime.sYear,sizeof(cCashAcceptorHeaderRec.sEndDateAndTime.sYear))))
			{
				SYSTEMTIME	SysTime;
				GetLocalTime(&SysTime);
				SystemTime2Tag(&SysTime, &cCashAcceptorHeaderRec.sEndDateAndTime);	
				lRtc = Operation(REC_UPDATE, &cCashAcceptorHeaderRec);	
				if (lRtc)
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("GetCashAcceptorReportXml__ - Cash acceptor header record can not be updated with end time!");
						_LOGMSG.LogMsg(str);
					}
				}
				else 
				{
					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;
						str.Format("GetCashAcceptorReportXml__ - Cash acceptor header record was updated with end time!");
						_LOGMSG.LogMsg(str);
					}
				}
			}

			ConvertCashAcceptorRecHeader2Xml(cCashAcceptorHeaderRec, &cXmlCashAcceptorReport, &cXMLPicReportPointers);
		}
		else
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("Cash acceptor header record nor found, use default value !!!!");
				_LOGMSG.LogMsg(str);
			}

		}

		// Append Terminals section to the xml
		if (cXmlCashAcceptorReport.CreateElement("Terminals", &cXMLTerminalsSection))
		{
			BOOL bRet = cXmlCashAcceptorReport.AppendChild(&cXMLTerminalsSection, &cXMLPicReportPointers);
		}

		memset(&cTmpTotalCashAcceptorRec , 0 , sizeof(cTmpTotalCashAcceptorRec)); //4.0.11.135
		memset(&cCashAcceptorRec, 0, sizeof(cCashAcceptorRec));

		// find all valid drawers per terminal.
		for( long  i =1 ; i <= MAX_CASH_ACCEPTOR_RECORD_SHIFT; i++)
		{
			
			bFound = FALSE;

			CreateFindFirstIndex(i ,cCashAcceptorRec.cIndex.cGeneral.sKeyIndex,bIndexSearch);
			lRtc = Operation(REC_READ_FIRST , &cCashAcceptorRec);  //4.0.11.10

			while (!lRtc)
			{
				if ((cCashAcceptorRec.lAmount) && (cCashAcceptorRec.cIndex.cInfo.byPeriod == byPeriodRequest ) && 
						(cCashAcceptorRec.lTerminalId == i))
				{
					bFound = TRUE;

					cTmpTotalCashAcceptorRec.lAmount +=  cCashAcceptorRec.lAmount;
					cTmpTotalCashAcceptorRec.lCounter += cCashAcceptorRec.lCounter;
					cTmpTotalCashAcceptorRec.lRemovalCounter += cCashAcceptorRec.lRemovalCounter; //4.0.11.135
					cTmpTotalCashAcceptorRec.lTerminalId = i;
					memcpy(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD , cCashAcceptorRec.cIndex.cInfo.sDateYYMMDD , sizeof(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD));  //4.0.11.135
					
					array.push_back(cCashAcceptorRec);
					lQdxTotalRecords++;

				}
				else
				{

					break;   // we arrived to end of list
				}

				lRtc = Operation(REC_READ_NEXT , &cCashAcceptorRec);
			}

			if (bFound)
			{
				ConvertCashAcceptorRec2Xml(cTmpTotalCashAcceptorRec, &cXmlCashAcceptorReport, &cXMLTerminalsSection);

				memset(&cTmpTotalCashAcceptorRec , 0 , sizeof(cTmpTotalCashAcceptorRec));
				
				lCounter++;


				if(LOG_BASIC_FLOW_CONTROL)
				{
					CString str;

					str.Format("Cash acceptor raw:: counter=%ld ",  lCounter );
					_LOGMSG.LogMsg(str);
				}
			}
		}


		if (lQdxTotalRecords)
		{
			sort_array(array);
			cCashAcceptorIndex = array[0].cIndex;
			memset(&cTmpTotalCashAcceptorRec , 0 , sizeof(cTmpTotalCashAcceptorRec));
		}


		bFound = FALSE;

		if (array.size())
		{
			cTmpTotalCashAcceptorRec.lAmount =  array[0].lAmount;
			cTmpTotalCashAcceptorRec.lCounter = array[0].lCounter; 
			memcpy(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD ,array[0].cIndex.cInfo.sDateYYMMDD , sizeof(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD)); //4.0.11.135
			cTmpTotalCashAcceptorRec.lRemovalCounter += array[0].lRemovalCounter;
			bFound = TRUE;

			// Append Days total section to the xml
			if (cXmlCashAcceptorReport.CreateElement("Days", &cXMLDaysTotalSection))
			{
				cXmlCashAcceptorReport.AppendChild(&cXMLDaysTotalSection, &cXMLPicReportPointers);
			}
		}


		for (size_t i=1 ; i < array.size() ; i++)
		{		
			if (!memcmp(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD , array[i].cIndex.cInfo.sDateYYMMDD,sizeof(cCashAcceptorIndex.cInfo.sDateYYMMDD)))
			{
				bFound = TRUE;

				cTmpTotalCashAcceptorRec.lAmount +=  array[i].lAmount;
				cTmpTotalCashAcceptorRec.lCounter += array[i].lCounter;
				memcpy(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD ,array[i].cIndex.cInfo.sDateYYMMDD , sizeof(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD)); //4.0.11.135
				cTmpTotalCashAcceptorRec.lRemovalCounter += array[i].lRemovalCounter;
			}
			else
			{

				if (bFound)
				{
					CreateCashAcceptorTotalDays(cTmpTotalCashAcceptorRec, &cXmlCashAcceptorReport, &cXMLDaysTotalSection); //4.0.17.30 - TD 35765

					lCounter++;

					if(LOG_BASIC_FLOW_CONTROL)
					{
						CString str;

						str.Format("Cash acceptor total raw:: counter=%ld ",  lCounter );  //4.0.11.115
						_LOGMSG.LogMsg(str);
					}

					memset(&cTmpTotalCashAcceptorRec , 0 , sizeof(cTmpTotalCashAcceptorRec)); //4.0.11.135

					bFound = TRUE;

					cTmpTotalCashAcceptorRec.lAmount +=  array[i].lAmount;
					cTmpTotalCashAcceptorRec.lCounter += array[i].lCounter; //4.0.11.135
					memcpy(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD ,array[i].cIndex.cInfo.sDateYYMMDD , sizeof(cTmpTotalCashAcceptorRec.cIndex.cInfo.sDateYYMMDD)); //4.0.11.135
					cTmpTotalCashAcceptorRec.lRemovalCounter += array[i].lRemovalCounter;

				}

			}

		}

		if (bFound)
		{
			CreateCashAcceptorTotalDays(cTmpTotalCashAcceptorRec, &cXmlCashAcceptorReport, &cXMLDaysTotalSection); //4.0.17.30 - TD 35765

			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;

				str.Format("Cash acceptor total reports raw:: counter=%ld ", lCounter );  //4.0.11.115
				_LOGMSG.LogMsg(str);
			}

			memset(&cTmpTotalCashAcceptorRec , 0 , sizeof(cTmpTotalCashAcceptorRec)); //4.0.11.135
		}
	}
	
	*lRecordCount =lCounter;
	cXmlCashAcceptorReport.GetXmlString(sXmlData);

	//Ella to remove
#ifdef _DEBUG
	cXmlCashAcceptorReport.SaveToFile("C:\\Office\\PumpSrv\\Log\\CashAcceptorReport.xml");
#endif
	cXmlCashAcceptorReport.CloseDoc();

	return retCode;
}

long CCashAcceptorFile::WriteCashAcceptorRecord(long lTerminalId, CASH_ACCEPTOR_REC &cCashAcceptorRec)
{
	long lRtc = 0 ;


	return lRtc;

}

void CCashAcceptorFile::ConvertCahAcceptorRec2TagHead(CASH_ACCEPTOR_HEADER_REC & sRec, TAG_CASH_ACCEPTOR_HEADER  & sTag )
{
	memset(&sTag , ' ', sizeof(sTag));
	memcpy(&sTag.sDateAndTime , &sRec.sDateAndTime , sizeof(sRec.sDateAndTime));
}

void CCashAcceptorFile::ConvertCahAcceptorRec2TagRec(CASH_ACCEPTOR_REC & sRec, TAG_CASH_ACCEPTOR & sTag ,BYTE sRecordType)
{
	memset(&sTag , ' ', sizeof(sTag));
	l2a(sRec.lTerminalId, sTag.sTerminalId, sizeof(sTag.sTerminalId));
	l2a(sRec.lAmount, sTag.sAmount, sizeof(sTag.sAmount));
	l2a(sRec.lCounter,sTag.sCounter , sizeof(sTag.sCounter));
	l2a(sRec.lRemovalCounter ,sTag.sRemovalCounter, sizeof(sTag.sRemovalCounter));
	memcpy(sTag.sDateYYMMDD , sRec.cIndex.cInfo.sDateYYMMDD,sizeof(sTag.sDateYYMMDD));
	
	sTag.byRecordType = sRecordType; //4.0.11.50
}

void CCashAcceptorFile::ConvertCashAcceptorRecHeader2Xml(CASH_ACCEPTOR_HEADER_REC & sRec, CXMLInterface *pXmlData, CXMLPointers	*pXMLPointers)
{
	CString str;

	if (pXmlData)
	{
		char			sDateAndTime[40] = {0};
		char			sEndDateAndTime[40] = {0};
	
		CXMLPointers	cXMLGeneralSection;
		BOOL			bRet = FALSE;

		bRet = pXmlData->CreateElement("General", &cXMLGeneralSection);		
		if (bRet)
			bRet = pXmlData->AppendChild(&cXMLGeneralSection, pXMLPointers);

		if (bRet)
		{
			memset(sDateAndTime,' ',sizeof(sDateAndTime));
			
			sprintf(sDateAndTime,"%.02d-%.02d-%.04d %.02d:%.02d",
						a2l(sRec.sDateAndTime.sDay, sizeof(sRec.sDateAndTime.sDay)),
						a2l(sRec.sDateAndTime.sMonth, sizeof(sRec.sDateAndTime.sMonth)),
						a2l(sRec.sDateAndTime.sYear, sizeof(sRec.sDateAndTime.sYear)),
						a2l(sRec.sDateAndTime.sHour, sizeof(sRec.sDateAndTime.sHour)),
						a2l(sRec.sDateAndTime.sMinute, sizeof(sRec.sDateAndTime.sMinute)));

					
			pXmlData->SetCharAttribute("StartDateAndTime",sDateAndTime, &cXMLGeneralSection);

			memset(sEndDateAndTime,' ',sizeof(sEndDateAndTime));
			sprintf(sEndDateAndTime,"%.02d-%.02d-%.04d %.02d:%.02d",
						a2l(sRec.sEndDateAndTime.sDay, sizeof(sRec.sEndDateAndTime.sDay)),
						a2l(sRec.sEndDateAndTime.sMonth, sizeof(sRec.sEndDateAndTime.sMonth)),
						a2l(sRec.sEndDateAndTime.sYear, sizeof(sRec.sEndDateAndTime.sYear)),
						a2l(sRec.sEndDateAndTime.sHour, sizeof(sRec.sEndDateAndTime.sHour)),
						a2l(sRec.sEndDateAndTime.sMinute, sizeof(sRec.sEndDateAndTime.sMinute)));
					
			pXmlData->SetCharAttribute("EndDateAndTime",sEndDateAndTime, &cXMLGeneralSection);

			str.Format("ConvertCashAcceptorRecHeader2Xml: startDate = %s, endDate = %s", sDateAndTime, sEndDateAndTime);
			_LOGMSG.LogMsg(str);
		}
	}
}

void CCashAcceptorFile::ConvertCashAcceptorRec2Xml(CASH_ACCEPTOR_REC & sRec, CXMLInterface *pXmlData, CXMLPointers *pXMLPointers, BYTE sRecordType)
{
	if (pXmlData && pXMLPointers)
	{
		CXMLPointers cXMLTerminal;
		BOOL bRet = FALSE;
		bRet = pXmlData->CreateElement("Terminal", &cXMLTerminal);		
		if (bRet)
			bRet = pXmlData->AppendChild(&cXMLTerminal, pXMLPointers);
		
		if (bRet)
		{
			long lId = sRec.lTerminalId;	//51031
			if (sRec.lTerminalId > 80)
				lId -= 80; 
			char sID[5] = {0};
			sprintf(sID,"%.2d",lId);

			pXmlData->SetLongAttribute("TerminalID",sRec.lTerminalId, &cXMLTerminal);
			pXmlData->SetLongAttribute("Counter",sRec.lCounter, &cXMLTerminal);
			pXmlData->SetLongAttribute("Amount",sRec.lAmount, &cXMLTerminal);
			pXmlData->SetLongAttribute("RemovalCounter",sRec.lRemovalCounter, &cXMLTerminal);
			
			pXmlData->SetXmlFieldWithSize("DrawerRemovalStatus",(char *)&sRec.byDrawerRemovalStatus, sizeof(sRec.byDrawerRemovalStatus), &cXMLTerminal);
		}
	}
}

//4.0.17.30 - TD 35765
void CCashAcceptorFile::CreateCashAcceptorTotalDays(CASH_ACCEPTOR_REC & sRec, CXMLInterface *pXmlData, CXMLPointers *pXMLPointers) 
{
	if (pXmlData && pXMLPointers)
	{
		CXMLPointers cXMLDay;
		BOOL bRet = FALSE;
		bRet = pXmlData->CreateElement("Day", &cXMLDay);		
		if (bRet)
			bRet = pXmlData->AppendChild(&cXMLDay, pXMLPointers);
		
		if (bRet)
		{
			//4.0.18.504 Merge from 4.0.17.220
			char	sDateAndTime[15] = {0};
			BYTE sDD[2] = {0}; 
			BYTE sYY[2] = {0};
			BYTE sMM[2] = {0};
			memset(sDateAndTime,' ',sizeof(sDateAndTime));
			memcpy(sDD,&sRec.cIndex.cInfo.sDateYYMMDD[4],2);
			memcpy(sMM,&sRec.cIndex.cInfo.sDateYYMMDD[2],2);
			memcpy(sYY,&sRec.cIndex.cInfo.sDateYYMMDD[0],2);

			sprintf(sDateAndTime,"%.02d-%.02d-00%.02d",
						a2l(sDD, sizeof(sDD)),
						a2l(sMM, sizeof(sMM)),
						a2l(sYY, sizeof(sYY)));
			
			pXmlData->SetCharAttribute("Date",sDateAndTime, &cXMLDay);	

			pXmlData->SetLongAttribute("Sum",sRec.lAmount, &cXMLDay);
		}
	}
}

void CCashAcceptorFile::CreateCurrentIndex(long lTerminalID, BYTE *sKeyIndex , BOOL bCurrentPeriod, BOOL bAddDate)
{
	SYSTEMTIME cCurrentTime;
	GetLocalTime(&cCurrentTime);
	char sTmpKey[11];
	char sPeriodSection = ' ';

	if(!bCurrentPeriod)
		sPeriodSection = '1';

	if (bAddDate)
		sprintf(sTmpKey , "%c%03ld%02d%02d%02d" , sPeriodSection,lTerminalID, cCurrentTime.wYear%2000 , cCurrentTime.wMonth ,cCurrentTime.wDay);
	else
		sprintf(sTmpKey , "%c%03ld%06ld" ,sPeriodSection, lTerminalID,0L);

	memcpy(sKeyIndex , sTmpKey , 10 );
}

long CCashAcceptorFile::UpdateBillsCurrent(long lTerminal, CASH_ACCEPTOR_REC & cCashRec)
{
	CASH_ACCEPTOR_REC cCashAcceptorRec;
	CString sMsg;
	long lRtc = 0 ;


	if (_Module.m_server.IsCashAcceptorreportEnable())
	{
		CreateCurrentIndex(lTerminal,cCashAcceptorRec.cIndex.cGeneral.sKeyIndex);

		lRtc = Operation(REC_READ, &cCashAcceptorRec);

		if(lRtc)
		{
			if(LOG_BASIC_FLOW_CONTROL)
			{
				sMsg.Format("Terminal %02ld add new cash acceptor record" ,lTerminal);
				_LOGMSG.LogMsg(sMsg);
			}

			memset(&cCashAcceptorRec , 0 , sizeof(cCashAcceptorRec));

		}
		else //4.0.11.11
			if(LOG_BASIC_FLOW_CONTROL)
			{
				
				sMsg.Format("Terminal %02ld update current cash acceptor record " ,lTerminal);
				_LOGMSG.LogMsg(sMsg);
			}
		
		//Check if record alreaady set , if no clear the record and set the terminal.
		if (cCashAcceptorRec.lTerminalId  != lTerminal)
		{
			memset(&cCashAcceptorRec , 0 , sizeof(cCashAcceptorRec));
			cCashAcceptorRec.lTerminalId = lTerminal;
		}

		CreateCurrentIndex(lTerminal,cCashAcceptorRec.cIndex.cGeneral.sKeyIndex);
		cCashAcceptorRec.lCounter +=  cCashRec.lCounter;
		cCashAcceptorRec.lAmount += cCashRec.lAmount;

							 
		if (!lRtc)
			lRtc= Operation(REC_UPDATE, &cCashAcceptorRec);	
		else
			lRtc= Operation(REC_INSERT, &cCashAcceptorRec);							

	}


	return lRtc;

}

long CCashAcceptorFile::MarkRemovalOnterminal(long lTerminalId)
{
	CASH_ACCEPTOR_REC cCashAcceptorRec;
	long lRetVal = 0;

	memset(&cCashAcceptorRec , 0 , sizeof(cCashAcceptorRec));

	CreateFindFirstIndex(lTerminalId ,cCashAcceptorRec.cIndex.cGeneral.sKeyIndex);

	lRetVal = Operation(REC_READ_FIRST, &cCashAcceptorRec);
	
	if ((!lRetVal) && (cCashAcceptorRec.lTerminalId == lTerminalId) && (cCashAcceptorRec.cIndex.cInfo.byPeriod == ' '))
	{
		cCashAcceptorRec.lRemovalCounter +=1; 
		cCashAcceptorRec.byDrawerRemovalStatus = '1';
		lRetVal = Operation(REC_UPDATE, &cCashAcceptorRec);

		if(LOG_BASIC_FLOW_CONTROL)
		{	
			CString str;
			str.Format("Terminal %02ld , report on removal drawer" ,lTerminalId);
			_LOGMSG.LogMsg(str);
		}
	}
	else
	{
		if(LOG_BASIC_FLOW_CONTROL)
		{	
			CString str;
			str.Format("Can't find record to mark Terminal %02ld ", 
				lTerminalId);
			_LOGMSG.LogMsg(str);
		}

	}

	return lRetVal;
}

void CCashAcceptorFile::CreateFindFirstIndex(long lTerminalID, BYTE *sKeyIndex, BOOL bCurrentPeriod)
{
	SYSTEMTIME cCurrentTime;
	GetLocalTime(&cCurrentTime);
	char sTmpKey[11];
	char sPeriodSection = ' ';

	if(!bCurrentPeriod)
		sPeriodSection = '1';

	sprintf(sTmpKey , "%c%03ld%06ld" ,sPeriodSection, lTerminalID,0L);

	memcpy(sKeyIndex , sTmpKey , 10 );
}

BOOL CCashAcceptorFile::DeleteOLAPeriod()
{
	CASH_ACCEPTOR_REC cCashAcceptorRec;
	CASH_ACCEPTOR_REC cCashAcceptorRecTmp;
	DWORD dwOffset = 0;
	long lRetCode = 0 ;
	CString str;
	BOOL  bMovePeriod =TRUE;

	str.Format("Delete cash acceptor old period" );
	_LOGMSG.LogMsg(str);

	CreateFindFirstIndex(0 ,cCashAcceptorRec.cIndex.cGeneral.sKeyIndex,FALSE);
	lRetCode = Operation(REC_READ_FIRST , &cCashAcceptorRec);  //4.0.11.10

	while (lRetCode == OK)
	{
		cCashAcceptorRecTmp = cCashAcceptorRec;

		if ( cCashAcceptorRec.cIndex.cInfo.byPeriod == '1') 
		{
			lRetCode = Operation(REC_DELETE , &cCashAcceptorRecTmp);  //4.0.11.10
		}
		else
			break;

		lRetCode = Operation(REC_READ_FIRST , &cCashAcceptorRec);  //4.0.11.10
	}


 return bMovePeriod;
}
