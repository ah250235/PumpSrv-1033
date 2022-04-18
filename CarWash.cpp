// CarWash.cpp: implementation of the CCarWash class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PumpSrv.h"
#include "CarWash.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//4.0.23.500 start
CBaseCarWash::CBaseCarWash()
{
	m_lCurrentRequest = 0L;
	m_lRequestType = 0L;
}

CBaseCarWash::~CBaseCarWash()
{

}



void CBaseCarWash::SetCurrentRequest(long lIndexNumber,long lRequestType,SYSTEMTIME *pcRequestSysTime)
{
	if(m_csCarWash.Lock(INFINITE))
	{
		m_lCurrentRequest = lIndexNumber;

		m_lRequestType = lRequestType;
		
		if (pcRequestSysTime)
			m_cRequestSysTime = *pcRequestSysTime;

		m_csCarWash.Unlock();
	}

	return;
}

void CBaseCarWash::GetCurrentRequest(long *plCurrentRequest,long *plRequestType,SYSTEMTIME *pcRequestSysTime)
{
	if(m_csCarWash.Lock(INFINITE))
	{
		*plCurrentRequest = m_lCurrentRequest;

		*plRequestType = m_lRequestType;
		
		if (pcRequestSysTime)
			*pcRequestSysTime = m_cRequestSysTime;

		m_csCarWash.Unlock();
	}

	return;
}



/*##################################################################################

					CCarWash  CLASS  - implements the regular car wash

##################################################################################*/
//4.0.23.504 end

CCarWash::CCarWash()
{
	m_bCarWashEnabled = FALSE;
	m_eCarWashStatus  = CAR_WASH_ONLINE ;  //4.0.23.504 end


}

CCarWash::~CCarWash()
{
	m_cWarWashRequest.RemoveAll(); //4.0.26.501 144221
}

BOOL CCarWash::CarWashEnabled()
{
	return m_bCarWashEnabled;
}

BOOL CarWashOrderSorter(CAR_WASH_PROGRAM & lhs, CAR_WASH_PROGRAM & rhs)
{
	if (lhs.lDisplayOrder > 0)
	{
		if (rhs.lDisplayOrder <= 0)
			return true;
		if (lhs.lDisplayOrder < rhs.lDisplayOrder)
			return true;
	}

	return false;
}

void CCarWash::LoadCarWashDefinition()
{
	DWORD dwTmp;
	char sTmpStr[MAX_FIELD_VAL];
	CString str("CarWash");

	_PARAM.LoadParam((char *)((LPCTSTR)str), "Enabled",&dwTmp,(DWORD)0,TRUE);
	_PARAM.LoadParam((char *)((LPCTSTR)str), "DiscountPerMinGals",&_Module.m_server.m_CarWashGeneralDefinitions.dwDiscountPerMinGals,(DWORD)0,TRUE);
	_PARAM.LoadParam((char *)((LPCTSTR)str), "MinGals",&_Module.m_server.m_CarWashGeneralDefinitions.dwMinGallons,(DWORD)0,TRUE);
	_PARAM.LoadParam((char *)((LPCTSTR)str), "CarWashCode",&_Module.m_server.m_CarWashGeneralDefinitions.dwCarWashPluCode,(DWORD)0,TRUE);
	_PARAM.LoadParam((char *)((LPCTSTR)str), "DiscountCode",&_Module.m_server.m_CarWashGeneralDefinitions.dwDiscountCode,(DWORD)0,TRUE);
	_PARAM.LoadParam((char *)((LPCTSTR)str), "ParamCRC",&dwTmp,(DWORD)0,TRUE);
	_PARAM.LoadParam((char *)((LPCTSTR)str), "LoyaltyApplicable",&_Module.m_server.m_CarWashGeneralDefinitions.dwCarWashLoyaltyApplicable,(DWORD)0,TRUE); //3.1.1.1
	_PARAM.LoadParam((char *)((LPCTSTR)str), "ScaleFactor",&_Module.m_server.m_CarWashGeneralDefinitions.dwCarwashScaleFactor,(DWORD)1,TRUE); //3.1.1.1
	
	_PARAM.LoadParam((char *)((LPCTSTR)str), "CommPort",&dwTmp,(DWORD)1,TRUE);
	_PARAM.LoadParam((char *)((LPCTSTR)str), "BaudRate",&dwTmp,(DWORD)9600,TRUE);
	_PARAM.LoadParam((char *)((LPCTSTR)str), "IPAddress",sTmpStr,sizeof(sTmpStr),"",TRUE);
	_PARAM.LoadParam((char *)((LPCTSTR)str), "Type",sTmpStr,sizeof(sTmpStr),"Uni-Cam",TRUE);
	_Module.m_server.m_CarWashGeneralDefinitions.sType = sTmpStr;
	_PARAM.LoadParam((char *)((LPCTSTR)str), "Simulator",&dwTmp,(DWORD)0,TRUE);
			
	dwTmp = 0;
	_PARAM.LoadParam((char *)((LPCTSTR)str), "Enabled",&dwTmp,(DWORD)0,FALSE);

	m_bCarWashEnabled = (dwTmp ? TRUE : FALSE);

	//Check if table "WashPrograms" exist
	for(int i = 1; i <= MAX_CAR_WASH_PROGRAM; i++)
	{		
		CString str ("WashPrograms\\WashProgram");
		CString strProgramName("Program");
		char sNumber[8];

		memset(sNumber,0,sizeof(sNumber));
		sprintf(sNumber,"%02d",i);
		str += sNumber;
		strProgramName += sNumber;
		ZeroMemory((char *)sTmpStr, sizeof(sTmpStr));

		_PARAM.LoadParam((char *)((LPCTSTR)str), "CCH_Product_Code", _Module.m_server.m_cCarWashProgramsDetails[i].sProductCode, sizeof(_Module.m_server.m_cCarWashProgramsDetails[i].sProductCode), (char *)((LPCTSTR)sNumber), TRUE);

		_PARAM.LoadParam((char *)((LPCTSTR)str), "ShortName", _Module.m_server.m_cCarWashProgramsDetails[i].sShortName, sizeof(_Module.m_server.m_cCarWashProgramsDetails[i].sShortName), /*(char *)((LPCTSTR)strProgramName)*/"", TRUE);

		_PARAM.LoadParam((char *)((LPCTSTR)str), "FullName", _Module.m_server.m_cCarWashProgramsDetails[i].sFullName, sizeof(_Module.m_server.m_cCarWashProgramsDetails[i].sFullName), (char *)((LPCTSTR)strProgramName), TRUE);
	
		_PARAM.LoadParam((char *)((LPCTSTR)str), "Price",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cCarWashProgramsDetails[i].lPrice = dwTmp;

		_PARAM.LoadParam((char *)((LPCTSTR)str), "TaxRate",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cCarWashProgramsDetails[i].lTaxRate = dwTmp;

		_PARAM.LoadParam((char *)((LPCTSTR)str), "ValidEntry",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cCarWashProgramsDetails[i].lValidEntry = dwTmp;

		_PARAM.LoadParam((char *)((LPCTSTR)str), "MapTaxLink",&dwTmp,(DWORD)0,TRUE);  //4.0.4.31
		_Module.m_server.m_cCarWashProgramsDetails[i].lMapTaxLink = dwTmp;

		//Add by Yaron CRC field
		_PARAM.LoadParam((char *)((LPCTSTR)str), "ParamCRC",&dwTmp,(DWORD)0,TRUE);

		_PARAM.LoadParam((char *)((LPCTSTR)str), "Product_Discount",&dwTmp,(DWORD)0,TRUE);  //Shuki 4.0.20.501 TD-
		_Module.m_server.m_cCarWashProgramsDetails[i].lDiscount = dwTmp;
		
		_PARAM.LoadParam((char *)((LPCTSTR)str), "GradePriceMinimumLimit",&dwTmp,(DWORD)0,TRUE); // 4.0.28.502 TD 264116

		_PARAM.LoadParam((char *)((LPCTSTR)str), "DisplayOrder",&dwTmp,(DWORD)0,TRUE);
		_Module.m_server.m_cCarWashProgramsDetails[i].lDisplayOrder = dwTmp;

		_PARAM.LoadParam((char *)((LPCTSTR)str), "Volume_Limit", &dwTmp, (DWORD)0, TRUE);	//TD 429143
		_Module.m_server.m_cCarWashProgramsDetails[i].lVolumeLimit = dwTmp;
	}

	std::sort(_Module.m_server.m_cCarWashProgramsDetails+1, &_Module.m_server.m_cCarWashProgramsDetails[MAX_CAR_WASH_PROGRAM], CarWashOrderSorter);//CR 404903
}

void CCarWash::InitCarWashLists()
{
	if ( m_bCarWashEnabled )
	{
		BOOL bMsgSent = FALSE;
		CAR_WASH_INFO info, tmpInfo;
		memset(&info, 0, sizeof(info));

		long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_READ_FIRST, &info);

		while (!lRtc)
		{
			switch (info.lRequestStatus)
			{
				case CW_REQ_OPEN:
				{
					switch (info.lRequestType)
					{
						case CW_TYPE_GENERATE:
							AddNewRequest(info);	//(info.lIndexNumber); 
							break;

						case CW_TYPE_CANCEL:
							AddCancelRequest(info.lIndexNumber);
							break;

						default:;
					}
					break;
				}
				case CW_REQ_SENT:
				{
					//This record must be at the tail, we will save it to the end.
					tmpInfo = info;
					bMsgSent = TRUE;
					break;
				}
				case CW_REQ_RESPONDED:
				case CW_REQ_CLOSE:
				default:;
			}

			lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_READ_NEXT, &info);
		}

		if ( bMsgSent )
		{
			switch (tmpInfo.lRequestType)
			{
				case CW_TYPE_GENERATE:
					if(m_csCarWash.Lock(INFINITE))
					{
						m_cWarWashRequest.AddTail(tmpInfo);	//(tmpInfo.lIndexNumber);
						m_csCarWash.Unlock();
					}
					break;

				case CW_TYPE_CANCEL:
					if(m_csCarWash.Lock(INFINITE))
					{
						m_cCancelList.AddTail(tmpInfo.lIndexNumber);
						m_csCarWash.Unlock();
					}
					break;

				default:;
			}
		}
	}
}


void CCarWash::AddNewRequest(CAR_WASH_INFO & cNewCarWashRequest)
//void CCarWash::AddNewRequest(long lNewIndexnumber) 
{ 
	if(m_csCarWash.Lock(INFINITE))
	{
		m_cWarWashRequest.AddHead(cNewCarWashRequest); 

		m_csCarWash.Unlock();
	}

	return;
}

long CCarWash::GetRequestIndex()
{ 
	long lRetCode = 0;

	if(m_csCarWash.Lock(INFINITE))
	{
		if ( m_cWarWashRequest.IsEmpty() == FALSE )
		{
			//lRetCode = m_cWarWashRequest.GetTail();
			CAR_WASH_INFO & cCWInfo = m_cWarWashRequest.GetTail();
			lRetCode = cCWInfo.lIndexNumber;

		}
		m_csCarWash.Unlock();
	}
	
	return lRetCode;
}

long CCarWash::GetRequestType()
{ 
	long lRetCode = 0;

	if(m_csCarWash.Lock(INFINITE))
	{
		if ( m_cWarWashRequest.IsEmpty() == FALSE )
		{
			//lRetCode = m_cWarWashRequest.GetTail();
			CAR_WASH_INFO & cCWInfo = m_cWarWashRequest.GetTail();
			lRetCode = cCWInfo.lRequestType;

		}
		m_csCarWash.Unlock();
	}
	
	return lRetCode;
}


BOOL CCarWash::RequestsWaiting() 
{ 
	BOOL bRetVal = FALSE;

	if(m_csCarWash.Lock(INFINITE))
	{
		bRetVal = !m_cWarWashRequest.IsEmpty();

		m_csCarWash.Unlock();
	}

	return bRetVal;
}

BOOL CCarWash::RequestWasSent(long lIndexNumber) 
{
	BOOL bRequestWasSent = TRUE;

	if(m_csCarWash.Lock(INFINITE))
	{
		//In lIndexNumber is in the list, then it didn't sent yet.
		if ( m_cWarWashRequest.IsEmpty() == FALSE )
		{
			POSITION pos = m_cWarWashRequest.GetHeadPosition();
			for (int i=0 ; i < m_cWarWashRequest.GetCount(); i++)
			{
				if ( (m_cWarWashRequest.GetNext(pos)).lIndexNumber == lIndexNumber )
				{
					bRequestWasSent = FALSE;
					break;
				}
			}
		}

		m_csCarWash.Unlock();
	}
	
	return bRequestWasSent;
}

void CCarWash::RemoveRequest() 
{ 
	if(m_csCarWash.Lock(INFINITE))
	{
		if(m_cWarWashRequest.IsEmpty() == FALSE)//4.0.5.26
			m_cWarWashRequest.RemoveTail(); 

		m_csCarWash.Unlock();
	}

	return;
}

void CCarWash::RemoveRequest(long lIndexNumber)
{
	if(m_csCarWash.Lock(INFINITE))
	{
		//In lIndexNumber is in the list, then it didn't sent yet.
		if ( m_cWarWashRequest.IsEmpty() == FALSE )
		{
			POSITION pos = m_cWarWashRequest.GetHeadPosition();
			POSITION posPrv;   //4.0.7.502
			
			for (int i=0 ; i < m_cWarWashRequest.GetCount(); i++)
			{
				posPrv = pos;

				if ( m_cWarWashRequest.GetNext(pos).lIndexNumber == lIndexNumber )
				{
					m_cWarWashRequest.RemoveAt(posPrv); //4.0.7.502
					break;
				}
			}
		}

		m_csCarWash.Unlock();
	}

	return;
}

void CCarWash::AddCancelRequest(long lIndexNumber)
{
	if(m_csCarWash.Lock(INFINITE))
	{
		m_cCancelList.AddHead(lIndexNumber);
		
		m_csCarWash.Unlock();
	}

	return;
}

void CCarWash::RemoveCancelRequest()
{
	if(m_csCarWash.Lock(INFINITE))
	{
		if(m_cCancelList.IsEmpty() == FALSE)//4.0.7.500
			m_cCancelList.RemoveTail();
		else
		{
			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString sMsg;
				sMsg.Format("CCarWash::RemoveCancelRequest,failed to remove record due to empty list");
				_LOGMSG.LogMsg(sMsg);
			}

		}

		m_csCarWash.Unlock();
	}

	return;
}

long CCarWash::GetCancelRequest()
{
	long lRtc = 0;

	if(m_csCarWash.Lock(INFINITE))
	{
		if (!m_cCancelList.IsEmpty())
			lRtc = m_cCancelList.GetTail();

		m_csCarWash.Unlock();
	}

	return lRtc;
}

long CCarWash::GetCarWashRecord(CAR_WASH_INFO *pInfo, char *szCarWashCode)
{
	long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_READ_FIRST, pInfo);

	while (!lRtc)
	{
		//Comapre the code at the record to that onr that was send.
		if ( !memcmp(szCarWashCode, pInfo->sCode, sizeof(pInfo->sCode)) )
			break;

		lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_READ_NEXT, pInfo);
	}

	return lRtc;
}

void CCarWash::CancelWashCode(long * lSyncNumber, long lDevice, long lFlags, BSTR sData,long lRefundCarWash, long *pRetCode, long lPosNumber)
{
	DWORD dwLastErr = 0L;
	CAR_WASH_INFO info;
	memset(&info,0,sizeof(CAR_WASH_INFO));

	TAG_CAR_WASH_DATA carWashData;
	memset(&carWashData,0,sizeof(carWashData));

	if (!CarWashSessionEmpty())  //4.0.6.161
		*pRetCode = GPI_CAR_WASH_IN_MIDDLE_OF_ANOTHER_SESSION;

	if(*pRetCode == GPI_OK)
	{
		//If POS sends lSyncNumber, then use it and don't look for code.
		if (*lSyncNumber!=FALSE)
		{
			//Check first if the request was sent to the controller,
			//becuse otherwise there is no reason to read the record from the Qdx file.
			info.lIndexNumber = *lSyncNumber;
			long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_READ,&info);
			if(lRtc)
			{
				if(LOG_LOGIC_ERROR)
				{
					CString str;
					str.Format("CancelWashCode, Read Operation for Car wash Failed !!! lRtc %d",lRtc); 
					_LOGMSG.LogMsg("CGPI","CancelWashCode", lPosNumber,LOG_POS,str, LOG_LEVEL_1);						
				}
				*pRetCode = GPI_FAILED_IO;
			}
			else
			{
				switch ( info.lRequestStatus )
				{
					case CW_REQ_OPEN:
						//Request wasn't sent yet, therefore just remove it from the list
						RemoveRequest(*lSyncNumber);
						//and mark the record as close.
						info.lRequestStatus = CW_REQ_CLOSE;
						break;

					case CW_REQ_SENT:
						//When we will get the response, make it CANCEL request.
						info.lRequestType |= CW_TYPE_CANCEL;
						SYSTEMTIME cRequestSysTime;
						GetLocalTime(&cRequestSysTime);
						break;

					case CW_REQ_RESPONDED:
					case CW_REQ_CLOSE:
						//If we received a code we must cancel it.
						//Open the request again
						info.lRequestStatus = CW_REQ_OPEN;
						//and make it CANCEL request
						info.lRequestType = CW_TYPE_CANCEL;
						//Add it to the cancel list
						AddCancelRequest(info.lIndexNumber);
					default:;
				}
				
				lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_UPDATE,&info);
				if(lRtc)
				{
					if(LOG_LOGIC_ERROR)
					{
						CString str;
						str.Format("CancelWashCode, Update Operation for Car wash Failed !!! lRtc %d",lRtc); 
						_LOGMSG.LogMsg("GPI","CancelWashCode", lPosNumber,LOG_POS,str, LOG_LEVEL_1);						
					}
					*pRetCode = GPI_FAILED_IO;
				}
			}
		}
		else
		{
			int iLen = Bstr2Buffer(sData,(void *)&carWashData,sizeof(carWashData));
			if(iLen)
			{
				if ( !GetCarWashRecord(&info, (char *)carWashData.sCode) )
				{
					CString str;
					str.Format("CGPI::CancelWashCode__ car wash code = %ld index number = %ld",
						a2l(carWashData.sCode,sizeof(carWashData.sCode)),info.lIndexNumber);
					_LOGMSG.LogMsg(str);
					//Open the request again
					info.lRequestStatus = CW_REQ_OPEN;
					//and make it CANCEL request
					*lSyncNumber = info.lIndexNumber;//amram 4.0.11.500
					info.lRequestType = CW_TYPE_CANCEL;
					*lSyncNumber = info.lIndexNumber;//4.0.11.170
					long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_UPDATE,&info);
					if(lRtc)
					{
						if(LOG_LOGIC_ERROR)
						{
							CString str;
							str.Format("CancelWashCode, Update Operation for Car wash Failed !!! lRtc %d",lRtc); 
							_LOGMSG.LogMsg("GPI","CancelWashCode",lPosNumber,LOG_POS,str, LOG_LEVEL_1);						
						}
						*pRetCode = GPI_FAILED_IO;
					}
					else
					{
						//Add it to the cancel listwashcode
						AddCancelRequest(info.lIndexNumber);
					}

				}
				else
				{
					//if it's not refund and not line void
					if(!lRefundCarWash && !(lFlags & CANCEL_WASH_CODE_LINE_VOID)) //4.0.21.500 - TD 14778
					{	
						*pRetCode = GPI_INVALID_CAR_WASH_CODE;
					}
					else
					{
						//amram
						//if we got here we have only wash code that only in the device
						// and not in QDX
						//in this case we have to cancel only with wash code
						
						int iLen = Bstr2Buffer(sData,(void *)&carWashData,sizeof(carWashData));
						if(!iLen)
							*pRetCode = GPI_INVALID_CAR_WASH_CODE;
						memset(&info,0,sizeof(CAR_WASH_INFO));

						info.lIndexNumber		= _Module.m_server.NewCarWashIndex();
						info.dwActiveReadNumber = _Module.m_server.GetActiveReadNumber();
						info.dwShiftNumber		= _Module.m_server.GetFormalShiftNumber();
						info.lDevice			= 1;
						info.lPOS				= 0;
						info.lRequestType		= CW_TYPE_CANCEL;
 						info.lRequestStatus		= CW_REQ_OPEN;
						info.lProgramIndex	    = 0;
						memcpy(info.sCode, carWashData.sCode, sizeof(carWashData.sCode));
						GetLocalTime(&info.cLastUpdate);
						long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_INSERT,&info);
						*lSyncNumber = info.lIndexNumber;//the numer that return to pos Pump
						CString sMsg;
						sMsg.Format("Create new Refound cancel trs and new seq number"\
							" SyncNumber = %ld",*lSyncNumber);

						_LOGMSG.LogMsg("GPI","CancelWashCode",lPosNumber,LOG_POS,sMsg);
						if(lRtc)
						{
							if(LOG_LOGIC_ERROR)
							{
								CString str;
								str.Format("CancelWashCode, Update Operation for Car wash Failed !!! lRtc %d",lRtc); 
								_LOGMSG.LogMsg("GPI","CancelWashCode",lPosNumber,LOG_POS,str, LOG_LEVEL_1);						
							}
							*pRetCode = GPI_FAILED_IO;
						}
						else
						{
							//Add it to the cancel list
							AddCancelRequest(info.lIndexNumber);
						}
					}
				}
			}
			else
			{
				dwLastErr = GetLastError();
				*pRetCode = GPI_LAST_ERROR;
			}
		}
	}
}


/******************************************************************************
 Description:	check  status car wash session.
 Returns:      	No active session = TRUE;
				   active session = FALSE
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			8/11/2004   16:44		Start
******************************************************************************/

BOOL CCarWash::CarWashSessionEmpty()
{
	BOOL  bRet = FALSE;

	if ((m_cWarWashRequest.IsEmpty() && m_cWarWashRequest.IsEmpty()))
		bRet = TRUE;

	return bRet;
}

void CCarWash::SetCarWashStatus(CarWashStatus_ eVal)
{
	m_eCarWashStatus  = eVal;
}

CarWashStatus_ CCarWash::GetCarWashStatus()
{
	return m_eCarWashStatus; 
}

void CCarWash::GetWashCode(long lSyncNumber, long *lResult, BSTR *sData, long *lFlags, long *pRetCode, long lPosNumber)
{

	TAG_CAR_WASH_DATA carWashData;
	memset(&carWashData, ' ',sizeof(carWashData));

	TAG_CAR_WASH_DATA_EXT carWashDataExt;
	memset(&carWashDataExt, ' ', sizeof(carWashDataExt));

	//Check first if the request was sent to the controller,
	//becuse otherwise there is no reason to read the record from the Qdx file.
	if ( RequestWasSent(lSyncNumber) )
	{
		CAR_WASH_INFO info;
		memset(&info,0,sizeof(CAR_WASH_INFO));

		info.lIndexNumber = lSyncNumber;
		long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_READ,&info);
		if(lRtc)
		{
			if(LOG_LOGIC_ERROR)
			{
				CString str;
				str.Format("GetWashCode, Read Operation for Car wash Failed !!! lRtc %d",lRtc); 
				_LOGMSG.LogMsg("CGPI","GetWashCode",lPosNumber,LOG_POS,str, LOG_LEVEL_1);						
			}
			*pRetCode = GPI_FAILED_IO;
		}
		else
		{
			switch ( info.lRequestStatus )
			{
				case CW_REQ_OPEN:
				case CW_REQ_SENT:
					//We should never get here, but just in case.
					*lResult = CW_DATA_NOT_YET_AVAILABLE;
					break;
				case CW_REQ_RESPONDED:
				{
					//Check if there a Code avliable
					long lRetCode = a2l(&info.bFailed, sizeof(info.bFailed)); //4.0.26.501 144221
					if (( info.sCode[0] != 0 ) && (lRetCode == CW_OK_VALID_CODE))//4.0.26.501 144221 (info.bFailed != '1'))//amram 4.0.11.500 
					{//Code avliable

						if(*lFlags == CAR_WASH_USE_EXT_TAG)  // Emerald
						{
							memcpy(carWashDataExt.sCode, info.sCode, sizeof(carWashDataExt.sCode));
							CString sProdcutCode(_Module.m_server.m_cCarWashProgramsDetails[info.lProgramIndex].sProductCode);
							memcpy(carWashDataExt.sProductCode, _Module.m_server.m_cCarWashProgramsDetails[info.lProgramIndex].sProductCode, MIN(sizeof(carWashDataExt.sProductCode), sProdcutCode.GetLength()));
							l2a(info.lPrice, carWashDataExt.sPrice, sizeof(carWashDataExt.sPrice));
							SystemTime2Tag(&info.cExpirationDate, &carWashDataExt.sExpirationDate);

							BYTE sTmp[sizeof(TAG_CAR_WASH_DATA_EXT) + 2];
							Str2StrNull((BYTE *)&carWashDataExt, sizeof(TAG_CAR_WASH_DATA_EXT), sTmp);
							CComBSTR strCarWashData(sizeof(sTmp), (LPCSTR)sTmp);
							*sData = strCarWashData.Copy();
						}
						else
						{
							memcpy(carWashData.sCode, info.sCode, sizeof(carWashData.sCode));

							l2a(info.lProgramIndex, carWashData.sProgram, sizeof(carWashData.sProgram)); // RFUEL-2609
							l2a(info.lPrice, carWashData.sPrice, sizeof(carWashData.sPrice));
							SystemTime2Tag(&info.cExpirationDate, &carWashData.sExpirationDate);
							BYTE sTmp[sizeof(TAG_CAR_WASH_DATA) + 2];
							Str2StrNull((BYTE *)&carWashData, sizeof(TAG_CAR_WASH_DATA), sTmp);
							CComBSTR strCarWashData(sizeof(sTmp), (LPCSTR)sTmp);
							*sData = strCarWashData.Copy();
						}	

						//Before handing the wash code to the POS, 
						//save the information to the QDX
						info.lRequestStatus = CW_REQ_CLOSE;
						GetLocalTime(&info.cLastUpdate);
						lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_UPDATE,&info);
						if(lRtc)
						{
							if(LOG_LOGIC_ERROR)
							{
								CString str;
								str.Format("GetWashCode, Update Operation for Car wash Failed !!! lRtc %d",lRtc); 
								_LOGMSG.LogMsg("CGPI","GetWashCode",lPosNumber,LOG_POS,str, LOG_LEVEL_1);						
							}
							*pRetCode = GPI_FAILED_IO;
							*lResult = CW_DATA_NOT_YET_AVAILABLE;
						}
						else
						{
							if(info.lRequestType == CW_TYPE_CANCEL)
								*lResult = CW_CODE_CANCELED;
							else
								*lResult = CW_OK_VALID_CODE;
						}
					}
					else
					{//Code not avliable
						info.lRequestStatus = CW_REQ_CLOSE;
						GetLocalTime(&info.cLastUpdate);
						lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_UPDATE,&info);
						if(lRtc)
						{
							if(LOG_LOGIC_ERROR)
							{
								CString str;
								str.Format("GetWashCode, Update Operation for Car wash Failed !!! lRtc %d",lRtc); 
								_LOGMSG.LogMsg("CGPI","GetWashCode", lPosNumber,LOG_POS,str, LOG_LEVEL_1);						
							}
							*pRetCode = GPI_FAILED_IO;
						}
						//*lResult = CW_OPERATION_FAILED;
						*lResult = lRetCode;
					}
					break;
				}
				case CW_REQ_CLOSE:
				{
					//Check if there a Code avliable
					if ( info.sCode[0] != 0 )
					{//Code avliable
						if(*lFlags == CAR_WASH_USE_EXT_TAG)  // Emerald
						{
							memcpy(carWashDataExt.sCode, info.sCode, sizeof(carWashDataExt.sCode));
							CString sProdcutCode(_Module.m_server.m_cCarWashProgramsDetails[info.lProgramIndex].sProductCode);
							memcpy(carWashDataExt.sProductCode, _Module.m_server.m_cCarWashProgramsDetails[info.lProgramIndex].sProductCode, MIN(sizeof(carWashDataExt.sProductCode), sProdcutCode.GetLength()));
							l2a(info.lPrice, carWashDataExt.sPrice, sizeof(carWashDataExt.sPrice));
							SystemTime2Tag(&info.cExpirationDate, &carWashDataExt.sExpirationDate);

							BYTE sTmp[sizeof(TAG_CAR_WASH_DATA_EXT) + 2];
							Str2StrNull((BYTE *)&carWashDataExt, sizeof(TAG_CAR_WASH_DATA_EXT), sTmp);
							CComBSTR strCarWashData(sizeof(sTmp), (LPCSTR)sTmp);
							*sData = strCarWashData.Copy();
						}
						else 
						{
							memcpy(carWashData.sCode, info.sCode, sizeof(carWashData.sCode));
							l2a(info.lProgramIndex, carWashData.sProgram, sizeof(carWashData.sProgram)); // RFUEL-2609
							l2a(info.lPrice, carWashData.sPrice, sizeof(carWashData.sPrice));
							SystemTime2Tag(&info.cExpirationDate, &carWashData.sExpirationDate);

							BYTE sTmp[sizeof(TAG_CAR_WASH_DATA) + 2];
							Str2StrNull((BYTE *)&carWashData, sizeof(TAG_CAR_WASH_DATA), sTmp);
							CComBSTR strCarWashData(sizeof(sTmp), (LPCSTR)sTmp);
							*sData = strCarWashData.Copy();
						}

						if(info.lRequestType == CW_TYPE_CANCEL)
							*lResult = CW_CODE_CANCELED;
						else
							*lResult = CW_OK_VALID_CODE;
					}
					else
					{//Code not avliable
						*lResult = CW_OPERATION_FAILED;
					}
				}
				default:;
			}
		}
	}

	//sData is [out] parameter, there for we must allocate memory for it
	//in case we didn't do it so far.
	if ((*lResult != CW_OK_VALID_CODE) && (*lResult != CW_CODE_CANCELED))
	{
		BYTE sTmp[sizeof(TAG_CAR_WASH_DATA)+2];
		Str2StrNull((BYTE *)&carWashData,sizeof(TAG_CAR_WASH_DATA),sTmp);
		CComBSTR strCarWashData(sizeof(sTmp),(LPCSTR)sTmp); 
		*sData = strCarWashData.Copy();
	}

}

void CCarWash::GenerateWashCode(long lDevice, long lWashProgram, long lFlags, long *lSyncNumber, long *pRetCode, long lPosNumber)
{
	if (!CarWashSessionEmpty())  //4.0.7.502
		*pRetCode = GPI_CAR_WASH_IN_MIDDLE_OF_ANOTHER_SESSION;

	if (*pRetCode == GPI_OK)
	{
		CAR_WASH_INFO info;
		memset(&info,0,sizeof(CAR_WASH_INFO));

		CString strWashCode;
		strWashCode.Format("%ld", lWashProgram);


		info.lIndexNumber		= _Module.m_server.NewCarWashIndex();
		info.dwActiveReadNumber = _Module.m_server.GetActiveReadNumber();
		info.dwShiftNumber		= _Module.m_server.GetFormalShiftNumber();
		info.lDevice			= lDevice;
		info.lPOS				= lPosNumber;
		info.lRequestType		= CW_TYPE_GENERATE;
		info.lRequestStatus		= CW_REQ_OPEN;
		info.lProgramIndex		= lWashProgram;
		GetLocalTime(&info.cLastUpdate);
		long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_INSERT,&info);
		if(lRtc)
		{
			if(LOG_LOGIC_ERROR)
			{
				CString str;
				str.Format("GenerateWashCode, Insert Operation for Car wash Failed !!! lRtc %d",lRtc); 
				_LOGMSG.LogMsg("CGPI","GenerateWashCode",lPosNumber,LOG_POS,str, LOG_LEVEL_1);						
			}
			*pRetCode = GPI_FAILED_IO;
		}
		else
		{
			//Use the record index file as the sync number
			*lSyncNumber = info.lIndexNumber;
			//Add this request to the requests list
			AddNewRequest(info);	//4.0.26.501.lIndexNumber);
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("GenerateWashCode, Device %ld, Wash Program %ld, Flags = 0x%.6X, SyncNumber %ld",lDevice,lWashProgram,lFlags,*lSyncNumber);
				_LOGMSG.LogMsg("CGPI","GenerateWashCode",lPosNumber,LOG_POS,str);
			}
		}
	}

}
//4.0.26.501 144221
void	CCarWash::ValidateWashCode(long lDevice, long *lSyncNumber, long lWashCode, long lWashProgramID, long lFlags, long lPosNumber, long * pRetCode) 
{
	if (!CarWashSessionEmpty())  //4.0.7.502
		*pRetCode = GPI_CAR_WASH_IN_MIDDLE_OF_ANOTHER_SESSION;
	if (*pRetCode == GPI_OK)
	{
		CAR_WASH_INFO info;
		memset(&info,0,sizeof(CAR_WASH_INFO));

		info.lIndexNumber		= _Module.m_server.NewCarWashIndex();
		info.dwActiveReadNumber = _Module.m_server.GetActiveReadNumber();
		info.dwShiftNumber		= _Module.m_server.GetFormalShiftNumber();
		info.lDevice			= lDevice;
		info.lPOS				= lPosNumber;
		info.lRequestType		= CW_TYPE_VALIDITY_CHECK;
		info.lRequestStatus		= CW_REQ_OPEN;
		info.lProgramIndex		= lWashProgramID;
		l2a(lWashCode, info.sCode, sizeof(info.sCode));
		GetLocalTime(&info.cLastUpdate);
		long lRtc = _Module.m_server.IndexOperation(CAR_WASH_FILE,REC_INSERT,&info);
		if(lRtc)
		{
			if(LOG_LOGIC_ERROR)
			{
				CString str;
				str.Format("ValidateWashCode, Insert Operation for Car wash Failed !!! lRtc %d",lRtc); 
				_LOGMSG.LogMsg("CGPI","ValidateWashCode", lPosNumber,LOG_POS,str, LOG_LEVEL_1);						
			}
			*pRetCode = GPI_FAILED_IO;
		}
		else
		{

			*lSyncNumber = info.lIndexNumber;
	
			AddNewRequest(info);	//4.0.26.501.lIndexNumber);
			if(LOG_BASIC_FLOW_CONTROL)
			{
				CString str;
				str.Format("ValidateWashCode, Wash Program %ld, Flags = 0x%.6X, SyncNumber %ld, Was Code = %ld ", lWashProgramID, lFlags, *lSyncNumber, lWashCode ); //4.0.26.508
				_LOGMSG.LogMsg("CGPI","ValidateWashCode",lPosNumber,LOG_POS,str);
			}
		}
	}
}

int CCarWash::GetUnusedWashProgramIndex()
{
	for (int i = 1; i <= MAX_CAR_WASH_PROGRAM; i++)
	{
		if (_Module.m_server.m_cCarWashProgramsDetails[i].lValidEntry == FALSE)
			return i;
	}
	return MAX_CAR_WASH_PROGRAM + 1; //invalid index
}


int CCarWash::FindWashProgramIndexByProdcutCode(string sProductCode)
{
	for (int i = 1; i < MAX_CAR_WASH_PROGRAM; i++)
	{
		if (_Module.m_server.m_cCarWashProgramsDetails[i].lValidEntry &&
			sProductCode.compare(_Module.m_server.m_cCarWashProgramsDetails[i].sProductCode) == 0)
		return i;
	}
	return MAX_CAR_WASH_PROGRAM + 1; //invalid index
}

/*##################################################################################

	  CCarWashGeneralRequest  CLASS  - implements the special car wash requests

##################################################################################*/


/******************************************************************************
 Description:	Implemetation of CCarWashGeneralRequest class to handle sync, 
				setup and cold start requests
 Returns:      	
				   
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			7/10/2009   10:44		Start
******************************************************************************/


CCarWashGeneralRequest::CCarWashGeneralRequest():m_bstrResponseBuff(NULL),
												 m_lRequestStatus(CW_REQ_CLOSE), 
												 m_lResult(CW_OK_VALID_CODE) ,
												 m_lDevice(0), m_lPOSNumber(0),	
												 m_lGeneralRequestsTimeout(0), 
												 m_lReturnedFlags(0)

{

}

CCarWashGeneralRequest::~CCarWashGeneralRequest()
{

}


void CCarWashGeneralRequest::InitRequestInfo()
{
	SetCarWashRequestStatus(CW_REQ_CLOSE);
	SetCarWashResult(CW_OK_VALID_CODE);
	SYSTEMTIME cRequestSysTime;
	GetLocalTime(&cRequestSysTime);
	SetCurrentRequest(0, CW_TYPE_NONE, &cRequestSysTime);
	SysFreeString(m_bstrResponseBuff);
	m_bstrResponseBuff = NULL;
	m_lPOSNumber = 0;
	m_lDevice = 0;
	m_lReturnedFlags = 0;

}

void CCarWashGeneralRequest::SetRequestBuffer(BSTR bstrIn)
{
	SysFreeString(m_bstrResponseBuff);
	_bstr_t bstrTmp(bstrIn, false);
	m_bstrResponseBuff = bstrTmp.copy();
	
}

/******************************************************************************
 Description:	Returns the result of the car wash general request if ready, 
					or CW_DATA_NOT_YET_AVAILABLE otherwise
 Returns:      	
				   
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			12/10/2009   10:44		Start
******************************************************************************/

void CCarWashGeneralRequest::GetWashCode(long lSyncNumber, long *lResult, BSTR *sData, long *lFlags, long *pRetCode, long lPosNumber)
{
	if (GetCarWashRequestStatus() == CW_REQ_RESPONDED) 	// A valid answer
	{
		*lResult = m_lResult;
	
		//return the result to the POS and initialize the request info
		if (m_lResult == CW_OK_VALID_CODE)
		{
			_bstr_t bstrtTmp(m_bstrResponseBuff);
			*sData = bstrtTmp.copy();
			*lFlags = GetReturnedFlags();
		}

		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("GetWashCode, Car wash request type %ld was responded. Result = %ld", m_lRequestType, m_lResult); 
			_LOGMSG.LogMsg("CGPI","GetWashCode",lPosNumber,LOG_POS,str);						
		}
		InitRequestInfo();
	}
	else
	{
		*lResult = CW_DATA_NOT_YET_AVAILABLE;

	}
}

/******************************************************************************
 Description:	Prepare car wash general request info to send to CL
 Returns:      	
				   
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			7/10/2009   10:44		Start
******************************************************************************/
void CCarWashGeneralRequest::GenerateWashCode(long lDevice, long lWashProgram, long lFlags, long *lSyncNumber, long *pRetCode, long lPosNumber)
{
	if (!CarWashSessionEmpty())  //4.0.7.502
		*pRetCode = GPI_CAR_WASH_IN_MIDDLE_OF_ANOTHER_SESSION;

	if (*pRetCode == GPI_OK)
	{
		long lRequestType = 0;
		switch (lFlags)
		{
		case CAR_WASH_SETUP_FUNC:
			{
				//info.lRequestType	= CAR_WASH_SETUP_FUNC; //flags
				lRequestType = CW_TYPE_SETUP_FUNC;
				break;
			}
		case CAR_WASH_SYNC_FUNC:
			{
				//info.lRequestType	= CAR_WASH_SYNC_FUNC; //flags
				lRequestType = CW_TYPE_SYNC_FUNC;
				break;
			}
		case CAR_WASH_COLD_START_FUNC:
			{
			//	info.lRequestType	= CAR_WASH_COLD_START_FUNC; //flags
				lRequestType = CW_TYPE_COLD_START_FUNC;
				break;
			}
		}
		SYSTEMTIME cRequestSysTime;
		GetLocalTime(&cRequestSysTime);
		SetCurrentRequest(0, lRequestType, &cRequestSysTime);
		SetDeviceNumber(lDevice);								//4.0.23.508 92295
		SetCarWashRequestStatus(CW_REQ_OPEN);

		if(LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			str.Format("GenerateWashCode, Saving general request %d for car wash.",lFlags); 
			_LOGMSG.LogMsg("CCarWashGeneralRequest","GenerateWashCode",lPosNumber,LOG_POS,str);						
		}
	}
}

BOOL CCarWashGeneralRequest::CarWashSessionEmpty()
{
	BOOL  bRet = FALSE;

	if (m_lRequestStatus == CW_REQ_CLOSE)
		bRet = TRUE;

	return bRet;
}

void CCarWashGeneralRequest::SetCarWashRequestStatus(CWRequestStatus eVal)
{
	m_lRequestStatus = eVal;
}

CWRequestStatus CCarWashGeneralRequest::GetCarWashRequestStatus()
{
	return m_lRequestStatus;
}

BOOL CCarWashGeneralRequest::RequestWasSent(long lIndexNumber)
{
	BOOL bRequestWasSent = FALSE;

	if (m_lRequestStatus == CW_REQ_SENT)
		bRequestWasSent = TRUE;
	return bRequestWasSent;

}


void CCarWashGeneralRequest::SetRequestResponse(long lResult, long lDevice, BSTR sData, long lFlags)
{
	SetDeviceNumber(lDevice);
	SetRequestBuffer(sData);
	SetReturnedFlags(lFlags);
	SetCarWashRequestStatus(CW_REQ_RESPONDED);
	if (lResult)
		SetCarWashResult(CW_OPERATION_FAILED);
	else
		SetCarWashResult(CW_OK_VALID_CODE);

}

long CCarWashGeneralRequest::GetCurrentCarWashRequestFlags()
{
	long lFlags = 0;
	switch (m_lRequestType)
	{
	case CW_TYPE_COLD_START_FUNC:
		{
			lFlags = CAR_WASH_COLD_START_FUNC;
			break;
		}
	case CW_TYPE_SETUP_FUNC:
		{
			lFlags = CAR_WASH_SETUP_FUNC;
			break;
		}
	case CW_TYPE_SYNC_FUNC:
		{
			lFlags = CAR_WASH_SYNC_FUNC;
			break;
		}
	default:
		{
			lFlags = 0;

		}
	}

	return lFlags;
}


void CCarWashGeneralRequest::LoadCarWashDefinition()
{
	CString str("CarWash");

	_PARAM.LoadParam((char *)((LPCTSTR)str), "GeneralRequestsTimeout(Sec)",&m_lGeneralRequestsTimeout,(DWORD)15,TRUE); //4.0.26.500 144665

}


void CCarWashGeneralRequest::CancelWashCode(long * lSyncNumber, long lDevice, long lFlags, BSTR sData,long lRefundCarWash, long *pRetCode, long lPosNumber)
{
	if (*pRetCode == GPI_OK)
	{
		if (lFlags == GetCurrentCarWashRequestFlags())
		{
			_Module.m_server.SetCommand(CMD_GENERAL_SYSTEM_EVENT, 0, GCI_EVENT_CODAX_REQUEST_TIMEOUT);
			
			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("CancelWashCode, General request %ld for car wash was canceled !!!",lFlags); 
				_LOGMSG.LogMsg("CGPI","CancelWashCode", lPosNumber,LOG_POS,str);						
			}
			
			InitRequestInfo();
		}
	}
}