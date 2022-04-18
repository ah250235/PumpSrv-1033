// PumpTotalsXml.cpp: implementation of the CPumpTotalsXml class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PumpTotalsXml.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPumpTotalsXml::CPumpTotalsXml()
{

}

CPumpTotalsXml::~CPumpTotalsXml()
{

}

void CPumpTotalsXml::PumpTotalsInfo2Xml(TOTAL_INFO* info, BSTR* pRec)
{
	CXMLInterface	cXmlPumpInfo;
	CXMLPointers	cXMLPointers;
	CXMLPointers	cXMLPointers2;
	char			sDateAndtime[20];
	char			szTmp[MAX_NAME_LENGTH+1] = {0};

	if(cXmlPumpInfo.InitDoc() == TRUE)
	{

		long p = info->lPump-1;
	
		cXmlPumpInfo.CreateElement("PumpReadingInfo",&cXMLPointers);
		cXmlPumpInfo.AppendRootElement(&cXMLPointers);
		
		cXmlPumpInfo.CreateElement("Pump",&cXMLPointers2);

		cXmlPumpInfo.SetLongAttribute("PumpID",info->lPump, &cXMLPointers2);
		cXmlPumpInfo.SetLongAttribute("Nozzle",info->nNozzle, &cXMLPointers2);
		cXmlPumpInfo.SetLongAttribute("Grade",info->nGrade, &cXMLPointers2);
		
		//4.0.26.502 - TD 108311
		memcpy(szTmp, info->sVolume, sizeof(info->sVolume));
		cXmlPumpInfo.SetCharAttribute("Volume",szTmp, &cXMLPointers2);

		memset(szTmp, 0, sizeof(szTmp));
		memcpy(szTmp, info->sValueA, sizeof(info->sValueA));
		cXmlPumpInfo.SetCharAttribute("ValueA",szTmp, &cXMLPointers2);

		memset(szTmp, 0, sizeof(szTmp));
		memcpy(szTmp, info->sValueB, sizeof(info->sValueB));
		cXmlPumpInfo.SetCharAttribute("ValueB",szTmp, &cXMLPointers2);

		cXmlPumpInfo.SetLongAttribute("ActiveReadNumber",info->dwActiveReadNumber, &cXMLPointers2);
		cXmlPumpInfo.SetLongAttribute("ShiftNumber",info->dwShiftNumber, &cXMLPointers2);

		
		//setting time info
		memset(sDateAndtime,' ',sizeof(sDateAndtime));
		sprintf(sDateAndtime,"%04d-%02d-%02d,%02d:%02d:%02d",info->cSysTime.wYear,info->cSysTime.wMonth,info->cSysTime.wDay,info->cSysTime.wHour,info->cSysTime.wMinute,info->cSysTime.wSecond);
		cXmlPumpInfo.SetCharAttribute("DateAndTime",sDateAndtime,&cXMLPointers2);

		cXmlPumpInfo.SetLongAttribute("PumpStatus", info->byPumpStatus, &cXMLPointers2);

		cXmlPumpInfo.SetLongAttribute("NozzleStatus", info->byNozzleStatus, &cXMLPointers2);

		//indication if we are in the middle of shift
		long lEndOfShiftStatus = 0;
		if (info->nFlags & TOTALS_END_OF_SHIFT)
			lEndOfShiftStatus = 1;
		cXmlPumpInfo.SetLongAttribute("EndOfShiftStatus",lEndOfShiftStatus,&cXMLPointers2);

		long lRollingUpdateVolume = _Module.m_server.m_cPumpArray[p].GetCurrentTrsRollingUpdateVolumeByNozzle(info->nNozzle);
		cXmlPumpInfo.SetLongAttribute("RollingUpdateVolume",lRollingUpdateVolume,&cXMLPointers2);

		//Get the tank number that this nuzzle is connected to
		long lTank = _Module.m_server.m_cPumpArray[p].GetTankPerPosition(info->nNozzle);
		cXmlPumpInfo.SetLongAttribute("Tank1",lTank,&cXMLPointers2);

		cXmlPumpInfo.SetLongAttribute("Tank2",0,&cXMLPointers2);

		cXmlPumpInfo.SetLongAttribute("ShiftAccumulatedSales", info->lShiftAccumulatedSales, &cXMLPointers2);
		cXmlPumpInfo.SetLongAttribute("ShiftAccumulatedPumpTest", info->lShiftAccumulatedPumpTest, &cXMLPointers2);
		 // CR_475695
		long lRequestedBySrc = SetReadingType(info->nFlags);
		cXmlPumpInfo.SetLongAttribute("ReadingType", lRequestedBySrc, &cXMLPointers2); 
		
		cXmlPumpInfo.AppendChild(&cXMLPointers2, &cXMLPointers);
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			char sBuff[MAX_XML_BUF_LEN] = {0};
			cXmlPumpInfo.GetXmlString(sBuff);			
			str.Format("CPumpTotalsXml::PumpTotalsInfo2Xml. XmlBuffer=%s", sBuff);
			_Module.m_server.m_cLogger.LogMsg(info->lPump, LOG_PUMP, str);
		}

		cXmlPumpInfo.GetXmlString((BSTR*)pRec);	

		cXmlPumpInfo.CloseDoc();

	}
}


void CPumpTotalsXml::Xml2PumpTotalsInfo(BSTR* pRec, TOTAL_INFO* info)
{

	CXMLInterface cXmlInterface;
	char szPumpInfo[MAX_REC_VALUES];
	BOOL bRetCode;

	char szStr[MAX_FIELD_VAL];
	_variant_t varValue;

	CXMLPointers cPumpRoot;
	CXMLPointers cAllPumpElements;
	CXMLPointers cPumpElement;

	char* szPumpElementsStr[11] = {"PumpID","Nozzle","Grade","Volume","ValueA",
		"ValueB","ActiveReadNumber","ShiftNumber","DateAndTime","PumpStatus","NozzleStatus"};

	memset(&szStr,0,sizeof(szStr));

	bRetCode = cXmlInterface.InitDoc();
	if(bRetCode)
	{
		memset(szPumpInfo,' ',sizeof(szPumpInfo));
		int nLen = Bstr2Buffer(*pRec,szPumpInfo,sizeof(szPumpInfo));

		if (nLen > 0)
		 	bRetCode = cXmlInterface.LoadXmlString(szPumpInfo);
		else
			bRetCode = 0;

		if(bRetCode)
		{
			cXmlInterface.GetChildNodesList("Pump",NULL, &cAllPumpElements);

			while(cXmlInterface.GetNextItem(cAllPumpElements, cPumpElement))
			{

				for (int k = 0; k < 11; k++)
				{

					varValue = cXmlInterface.GetAttribute(szPumpElementsStr[k], &cPumpElement);
					if (varValue.bstrVal)
					{
						memcpy(szStr,(char*)((_bstr_t)varValue),sizeof(varValue));
					}

					if (!strncmp(szPumpElementsStr[k], "PumpID", strlen("PumpID")))
					{
						info->lPump = a2l((unsigned char*)szStr,sizeof(szStr));
					}
					else if (!strncmp(szPumpElementsStr[k], "Nozzle", strlen("Nozzle")))
					{
						info->nNozzle = a2l((unsigned char*)szStr,sizeof(szStr));
					}
					else if (!strncmp(szPumpElementsStr[k], "Grade", strlen("Grade")))
					{
						info->nGrade = a2l((unsigned char*)szStr,sizeof(szStr));
					}
					else if (!strncmp(szPumpElementsStr[k], "Volume", strlen("Volume")))
					{
						memcpy(info->sVolume, (char *)szStr, sizeof(info->sVolume));
					}
					else if (!strncmp(szPumpElementsStr[k], "ValueA", strlen("ValueA")))
					{
						memcpy(info->sValueA, (char *)szStr, sizeof(info->sValueA));
					}
					else if (!strncmp(szPumpElementsStr[k], "ValueB", strlen("ValueB")))
					{
						memcpy(info->sValueB, (char *)szStr, sizeof(info->sValueB));
					}
					else if (!strncmp(szPumpElementsStr[k], "ActiveReadNumber", strlen("ActiveReadNumber")))
					{
						info->dwActiveReadNumber = a2l((unsigned char*)szStr,sizeof(szStr));
					}
					else if (!strncmp(szPumpElementsStr[k], "ShiftNumber", strlen("ShiftNumber")))
					{
						info->dwShiftNumber = a2l((unsigned char*)szStr,sizeof(szStr));
					}
					else if (!strncmp(szPumpElementsStr[k], "DateAndTime", strlen("DateAndTime")))
					{
						//get the time info
						if(szStr)
						{
							sscanf(szStr,"%04d-%02d-%02d,%02d:%02d:%02d",&(info->cSysTime.wYear),&(info->cSysTime.wMonth),&(info->cSysTime.wDay),&(info->cSysTime.wHour),&(info->cSysTime.wMinute),&(info->cSysTime.wSecond));
						}
					}
					else if (!strncmp(szPumpElementsStr[k], "PumpStatus", strlen("PumpStatus")))
					{
						info->byPumpStatus = (BYTE) a2l((unsigned char*)szStr,sizeof(szStr));
					}
					else if (!strncmp(szPumpElementsStr[k], "NozzleStatus", strlen("NozzleStatus")))
					{
						info->byNozzleStatus = (BYTE) a2l((unsigned char*)szStr,sizeof(szStr));
					}
					
				}
			}

			cXmlInterface.CloseDoc();
		}
	}
}
//CR_475695_Start
long CPumpTotalsXml::SetReadingType(short nFlags)
{

	CString str;		
	str.Format("CPumpTotalsXml::SetReadingType(). nFlags=0x%x", nFlags);
	_Module.m_server.m_cLogger.LogMsg(str);


    long lRequestedBySrc = READ_ON_OTHER_REQUEST;
	
	if (nFlags & TOTALS_READ_ON_EOS_REQUEST) 
	{
		lRequestedBySrc = READ_ON_EOS_REQUEST;
	}
	
	else if (nFlags & TOTALS_ON_EOD_REQUEST) 
	{
		lRequestedBySrc = READ_ON_EOD_REQUEST;
	}
	
	else if (nFlags & TOTALS_ON_PRICE_CHANGE_REQUEST) 
	{
		lRequestedBySrc = READ_ON_PRICE_CHANGE_REQUEST;
	}
	
	else if (nFlags & TOTALS_READ_ON_SCHEDULE_CHANGE_REQUEST) 
	{
		lRequestedBySrc = READ_ON_SCHEDULED_REQUEST;
	}
	
	else if (nFlags & TOTALS_READ_ON_MANUAL_REQUEST) 
	{
		lRequestedBySrc = READ_ON_MANUAL_REQUEST;
	}

	str.Format("CPumpTotalsXml::SetReadingType(). lRequestedBySrc=%d", lRequestedBySrc);
	_Module.m_server.m_cLogger.LogMsg(str);

	return lRequestedBySrc;
}
//CR_475695_End