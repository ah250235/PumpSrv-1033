// TankXML.cpp: implementation of the CTankXML class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TankXML.h"
#include "XMLInterface.h"  


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//4.0.13.503

CTankXML::CTankXML()
{

}

CTankXML::~CTankXML()
{

}

void CTankXML::TankReadingInfo2Xml(TANK_READ_INFO* info, BSTR* pRec)
{

	CXMLInterface	cXmlTankInfo;
	CXMLPointers	cXMLPointers;
	CXMLPointers	cXMLPointers2;
	char			sDataAndtime[20];
	DWORD			dwTemp;

	if(cXmlTankInfo.InitDoc() == TRUE)
	{
	
		cXmlTankInfo.CreateElement("TankReadingInfo",&cXMLPointers);
		cXmlTankInfo.AppendRootElement(&cXMLPointers);
		
		cXmlTankInfo.CreateElement("Tank",&cXMLPointers2);

		cXmlTankInfo.SetLongAttribute("TankID",info->lTank, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("Product",info->lProduct, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("Status",info->lStatus, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("FuelLevel",info->lFuelLevel, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("Temperature",info->lTemperature, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("WaterLavel",info->lWaterLevel, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("WaterVolume",info->lWaterVolume, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("FuelVolume",info->lFuelVolume, &cXMLPointers2);

		cXmlTankInfo.SetLongAttribute("FuelWeight",info->lFuelWeight, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("ActiveReadNumber",info->dwActiveReadNumber, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("ShiftNumber",(long)info->dwShiftNumber, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("FuelValue",info->lFuelValue, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("PricePerLitre",info->lPricePerLitre, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("TcVolume",info->lTcVolume, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("Ullage",info->lUllage, &cXMLPointers2);

		//setting time info
		memset(sDataAndtime,' ',sizeof(sDataAndtime));
		sprintf(sDataAndtime,"%04d-%02d-%02d,%02d:%02d:%02d",info->cSysTime.wYear,info->cSysTime.wMonth,info->cSysTime.wDay,info->cSysTime.wHour,info->cSysTime.wMinute,info->cSysTime.wSecond);
		cXmlTankInfo.SetCharAttribute("DateAndTime",sDataAndtime,&cXMLPointers2);

		cXmlTankInfo.SetLongAttribute("Density",info->lDensity, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("TankOnline",info->lTankOnline, &cXMLPointers2);

		cXmlTankInfo.SetLongAttribute("DeliveryInProg",(long)info->byIsDeliveryInProgress, &cXMLPointers2);
		
		//Start 4.0.23.500 - TD 72606
		//setting tank gauge date info
		if (info->cTankGaugeReadDateTime.wYear > 0)
		{
			memset(sDataAndtime,' ',sizeof(sDataAndtime));
			sprintf(sDataAndtime,"%02d-%02d-%02d",info->cTankGaugeReadDateTime.wYear,info->cTankGaugeReadDateTime.wMonth,info->cTankGaugeReadDateTime.wDay);
			cXmlTankInfo.SetCharAttribute("TankGaugeReadDate",sDataAndtime,&cXMLPointers2);
		}

		if (info->cTankGaugeReadDateTime.wHour > 0)
		{
			//setting tank gauge time info
			memset(sDataAndtime,' ',sizeof(sDataAndtime));
			sprintf(sDataAndtime,"%02d:%02d", info->cTankGaugeReadDateTime.wHour,info->cTankGaugeReadDateTime.wMinute);
			cXmlTankInfo.SetCharAttribute("TankGaugeReadTime",sDataAndtime,&cXMLPointers2);
		}

		//Get some parameters from the registry for the xml
		CString strReg;
		strReg.Format("Tanks\\Tank%02d", info->lTank);
		
		//Product
		if (!info->lProduct)
		{
			_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "PureProduct", &dwTemp, (DWORD) 0, FALSE);
			info->lProduct = dwTemp;
		}
		cXmlTankInfo.SetLongAttribute("Product", info->lProduct, &cXMLPointers2);		

		//Get AdditionalCode from registry
		strReg.Format("PureProducts\\PureProduct%02d", info->lProduct);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "Code", &dwTemp, (DWORD) 0, FALSE);
		if (dwTemp > 0)
		{
			char szAdditionalCode[10] = {0};
			strReg.Format("Grades\\Grade%02d", dwTemp);
			_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "AdditionalCode", szAdditionalCode, sizeof(szAdditionalCode), "", FALSE);
			long lRet = cXmlTankInfo.SetCharAttribute("AdditionalCode", szAdditionalCode, &cXMLPointers2);
		}

		//Get FullHeightVolume from registry 
		strReg.Format("Tanks\\Tank%02d", info->lTank);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)strReg, "FullHeightVolume", &dwTemp, (DWORD) 0, FALSE);
		cXmlTankInfo.SetLongAttribute("Capacity", dwTemp, &cXMLPointers2);

		//End 4.0.23.500 - TD 72606

		//4.0.23.500 - TD 77765 - indication if we are in the middle of shift
		long lEndOfShiftStatus = a2l(&(info->byEndOfShiftStatus), sizeof(info->byEndOfShiftStatus));
		cXmlTankInfo.SetLongAttribute("EndOfShiftStatus",lEndOfShiftStatus,&cXMLPointers2);

		//CR_475695_Start
		long lRequestedBySrc = SetReadingType(info->nFlags);
		cXmlTankInfo.SetLongAttribute("ReadingType", lRequestedBySrc, &cXMLPointers2);
		//CR_475695_End
		//4.0.25.50 - TD 108311
		cXmlTankInfo.SetLongAttribute("ShiftAccumulatedSales",info->lShiftAccumulatedSales,&cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("ShiftAccumulatedDeliveryVolume",info->lShiftAccumulatedDeliveryVolume,&cXMLPointers2);

		cXmlTankInfo.AppendChild(&cXMLPointers2,&cXMLPointers);
		//CR_475695_Start
		if (LOG_DETAIL_FLOW_CONTROL)
		{
			CString str;
			char sBuff[MAX_XML_BUF_LEN] = {0};
			cXmlTankInfo.GetXmlString(sBuff);			
			str.Format("CTankXML::TankReadingInfo2Xml. XmlBuffer=^%s^", sBuff);
			_Module.m_server.m_cLogger.LogMsg(0, LOG_NONE, str);
		}
		//CR_475695_End
		cXmlTankInfo.GetXmlString((BSTR*)pRec);	

		cXmlTankInfo.CloseDoc();

	}


}

//CR_475695_Start 
long CTankXML::SetReadingType(short nFlags)
{

    long lRequestedBySrc = READ_ON_OTHER_REQUEST;

	if (nFlags & TANK_READ_ON_EOS_REQUEST) 
	{
		lRequestedBySrc = READ_ON_EOS_REQUEST;
	}
	
	else if (nFlags & TANK_READ_ON_EOD_REQUEST) 
	{
		lRequestedBySrc = READ_ON_EOD_REQUEST;
	}
	
	else if (nFlags & TANK_READ_ON_PRICE_CHANGE_REQUEST) 
	{
		lRequestedBySrc = READ_ON_PRICE_CHANGE_REQUEST;
	}
	
	else if (nFlags & TANK_READ_ON_SCHEDULE_CHANGE_REQUEST) 
	{
		lRequestedBySrc = READ_ON_SCHEDULED_REQUEST;
	}
	else if (nFlags & TANK_READ_ON_MANUAL_REQUEST) 
	{
		lRequestedBySrc = READ_ON_MANUAL_REQUEST;
	}
	
	return lRequestedBySrc;
}
//CR_475695_End

void CTankXML::Xml2TankReadingInfo(BSTR* pRec, TANK_READ_INFO* info)
{
	
	CXMLInterface cXmlInterface;
	char szTankInfo[MAX_REC_VALUES];
	BOOL bRetCode;
	
	char szStr[MAX_FIELD_VAL];
	_variant_t varValue;
	
	CXMLPointers cTankRoot;
	CXMLPointers cAllTankElements;
	CXMLPointers cTankElement;
	
	
	bRetCode = cXmlInterface.InitDoc();
	if(bRetCode)
	{
		memset(szTankInfo,' ',sizeof(szTankInfo));
		int nLen = Bstr2Buffer(*pRec,szTankInfo,sizeof(szTankInfo)); //4.0.23.508
		
		if (nLen > 0)
			bRetCode = cXmlInterface.LoadXmlString(szTankInfo);
		else
			bRetCode = 0;
		
		if(bRetCode)
		{
			
			cXmlInterface.GetChildNodesList("Tank",NULL, &cAllTankElements);
			
			while(cXmlInterface.GetNextItem(cAllTankElements, cTankElement))
			{
				// 4.0.28.320 Rewrote parsing using methods XmlInterface
				info->lTank = cXmlInterface.GetAttribute("TankID", &cTankElement);
				info->lProduct = cXmlInterface.GetAttribute("Product", &cTankElement);
				info->lFuelLevel = cXmlInterface.GetAttribute("FuelLevel", &cTankElement);
				info->lTemperature = cXmlInterface.GetAttribute("Temperature", &cTankElement);
				info->lWaterLevel = cXmlInterface.GetAttribute("WaterLevel", &cTankElement);
				info->lWaterVolume = cXmlInterface.GetAttribute("WaterVolume", &cTankElement);
				info->lFuelVolume = cXmlInterface.GetAttribute("FuelVolume", &cTankElement);
				info->lFuelWeight = cXmlInterface.GetAttribute("FuelWeight", &cTankElement);
				info->lFuelValue = cXmlInterface.GetAttribute("FuelValue", &cTankElement);
				info->lPricePerLitre = cXmlInterface.GetAttribute("PricePerLitre", &cTankElement);
				info->lTcVolume = cXmlInterface.GetAttribute("TcVolume", &cTankElement);
				info->lUllage = cXmlInterface.GetAttribute("Ullage", &cTankElement);
				info->lDensity = cXmlInterface.GetAttribute("Density", &cTankElement);
				info->lTankOnline = cXmlInterface.GetAttribute("TankOnline", &cTankElement);
				info->byIsDeliveryInProgress = cXmlInterface.GetAttribute("DeliveryInProg", &cTankElement);
				
				
				memset(&szStr,0,sizeof(szStr));
				cXmlInterface.GetAttributeToByteArrWithConvert("Status", szStr, &cTankElement);
				//get the Status info
				if(szStr)
				{
					sscanf(szStr,"%ld,%ld",&(info->lStatus),&(info->lStatus2));
				}
				
				memset(&szStr,0,sizeof(szStr));
				cXmlInterface.GetAttributeToByteArrWithConvert("DateAndTime", szStr, &cTankElement);
				//get the time info
				if(szStr)
				{
					sscanf(szStr,"%04d-%02d-%02d,%02d:%02d:%02d",&(info->cSysTime.wYear),&(info->cSysTime.wMonth),&(info->cSysTime.wDay),&(info->cSysTime.wHour),&(info->cSysTime.wMinute),&(info->cSysTime.wSecond));
				}
				
				
				memset(&szStr,0,sizeof(szStr));
				cXmlInterface.GetAttributeToByteArrWithConvert("TankGaugeReadDate", szStr, &cTankElement);
				//get the tank gauge read date info
				if(szStr)
				{
					sscanf(szStr,"%02d-%02d-%02d",&(info->cTankGaugeReadDateTime.wYear),&(info->cTankGaugeReadDateTime.wMonth),&(info->cTankGaugeReadDateTime.wDay));
				}
				
				
				memset(&szStr,0,sizeof(szStr));
				cXmlInterface.GetAttributeToByteArrWithConvert("TankGaugeReadTime", szStr, &cTankElement);
				//get the tank gauge read time info
				if(szStr)
				{
					sscanf(szStr,"%02d:%02d",&(info->cTankGaugeReadDateTime.wHour),&(info->cTankGaugeReadDateTime.wMinute));
				}
				
			}
			
			cXmlInterface.CloseDoc();
			
		}
		
	}
	
}


//4.0.13.504
/******************************************************************************
 Description: Convert the Delivery xml stream to DELIVERY_INFO struct	 	
 Returns:      	none
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			13/2/2007   18:39		Change - TD 39699 add 3 new fields to struct //4.0.19.503
 ShellyB        05/08/2008  10:00       Change - TD 63114 add 2 new fields: DeliveryRef, DriverID //4.0.20.501
******************************************************************************/
void CTankXML::Xml2DeliveryInfo(BSTR* pRec, DELIVERY_INFO* info)
{


	CXMLInterface cXmlInterface;
	char szDeliveryInfo[MAX_REC_VALUES];
	BOOL bRetCode;

	char szStr[MAX_FIELD_VAL];
	_variant_t varValue;

	CXMLPointers cDeliveryRoot;
	CXMLPointers cAllDeliveryElements;
	CXMLPointers cDeliveryElement;


	char* szDeliveryElementsStr[31] = {	"DeliveryID","TankNumber","Product","Status","Tag","DeliveredVolume",
										"ShiftNumber","DeliveryDateTime","EndDeliveryDateTime",
										"StartVolume","EndVolume","Temperature","Density","Weight", 
										"StartTankDensity" , "EndTankDensity" , "StartTankTemperature" ,
										"EndTankTemperature", "StartTankWeight" , "EndTankWeight" , "DeliveryWeight" ,
										"DeliveryRef","DriverID", "DateAndTime", "StartingTcVolume", "StartingWater",  //4.0.20.501 TD 63114
										"StartingHeight", "EndingTcVolume", "EndingWater", "EndingHeight", "ActionID"}; //4.0.21.120 - TD 72606
 

	memset(&szStr,0,sizeof(szStr));

	bRetCode = cXmlInterface.InitDoc();
	if(bRetCode)
	{
		memset(szDeliveryInfo,0,sizeof(szDeliveryInfo));//4.0.15.501
		Bstr2Buffer(*pRec,szDeliveryInfo,sizeof(szDeliveryInfo));
		bRetCode = cXmlInterface.LoadXmlString(szDeliveryInfo);

		

		if(bRetCode)
		{

			cXmlInterface.GetChildNodesList("Delivery",NULL, &cAllDeliveryElements);

			while(cXmlInterface.GetNextItem(cAllDeliveryElements, cDeliveryElement))
			{

				for (int k = 0; k < 31; k++)
				{

					memset(szStr , 0 , sizeof(szStr)); //4.0.15.501
					//4.0.14.201
					varValue = cXmlInterface.GetAttribute(szDeliveryElementsStr[k], &cDeliveryElement);
					if (varValue.bstrVal)
					{	
						memcpy(szStr,(const char*)((_bstr_t)varValue),strlen((char*)((_bstr_t)varValue)));		//4.0.25.80 TD 142857	
					}

					if (!strncmp(szDeliveryElementsStr[k], "DeliveryID", strlen("DeliveryID")))
					{
						info->lIndexNumber = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					else if (!strncmp(szDeliveryElementsStr[k], "TankNumber", strlen("TankNumber")))
					{
						info->lTank = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					else if (!strncmp(szDeliveryElementsStr[k], "Product", strlen("Product")))
					{
						info->lProduct = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					else if (!strncmp(szDeliveryElementsStr[k], "Status", strlen("Status")))
					{
						info->lStatus = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					else if (!strncmp(szDeliveryElementsStr[k], "Tag", strlen("Tag")))
					{
						info->lTag = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					else if (!strncmp(szDeliveryElementsStr[k], "DeliveredVolume", strlen("DeliveredVolume")))
					{
						info->lDeliveredVolume = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					else if (!strncmp(szDeliveryElementsStr[k], "ShiftNumber", strlen("ShiftNumber")))
					{
						info->dwShiftNumber = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					else if (!strncmp(szDeliveryElementsStr[k], "DeliveryDateTime", strlen("DeliveryDateTime")))
					{
						//get the time info
						if(szStr)
						{
							sscanf(szStr,"%04d-%02d-%02d,%02d:%02d:%02d",&(info->cDeliveryDateTime.wYear),&(info->cDeliveryDateTime.wMonth),&(info->cDeliveryDateTime.wDay),&(info->cDeliveryDateTime.wHour),&(info->cDeliveryDateTime.wMinute),&(info->cDeliveryDateTime.wSecond));
						}
					}
					else if (!strncmp(szDeliveryElementsStr[k], "EndDeliveryDateTime", strlen("EndDeliveryDateTime")))
					{
						//get the time info
						if(szStr)
						{
							sscanf(szStr,"%04d-%02d-%02d,%02d:%02d:%02d",&(info->cEndDeliveryDateTime.wYear),&(info->cEndDeliveryDateTime.wMonth),&(info->cEndDeliveryDateTime.wDay),&(info->cEndDeliveryDateTime.wHour),&(info->cEndDeliveryDateTime.wMinute),&(info->cEndDeliveryDateTime.wSecond));
						}
					}
					else if (!strncmp(szDeliveryElementsStr[k], "StartVolume", strlen("StartVolume")))
					{
						info->lStartVolume = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					else if (!strncmp(szDeliveryElementsStr[k], "EndVolume", strlen("EndVolume")))
					{
						info->lEndVolume = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					else if (!strncmp(szDeliveryElementsStr[k], "Temperature", strlen("Temperature")))
					{
						info->lTemperature = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					else if (!strncmp(szDeliveryElementsStr[k], "Density", strlen("Density")))
					{
						info->lDensity = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					else if (!strncmp(szDeliveryElementsStr[k], "Weight", strlen("Weight")))
					{
						info->lWeight = a2l((unsigned char*)szStr, min(sizeof(szStr),strlen(szStr)));	//4.0.19.502
					}
					//4.0.14.201

					//4.0.20.52	{					
					else if (!strncmp(szDeliveryElementsStr[k], "StartTankDensity", strlen("StartTankDensity"))) 
					{
						info->lStartDensity = a2l((unsigned char*)szStr,strlen(szStr)); 
					}
					else if (!strncmp(szDeliveryElementsStr[k], "StartTankTemperature", strlen("StartTankTemperature"))) 
					{
						info->lStartTemperature = a2l((unsigned char*)szStr,strlen(szStr));
					}
					else if (!strncmp(szDeliveryElementsStr[k], "StartTankWeight", strlen("StartTankWeight"))) 
					{
						info->lStartWeight = a2l((unsigned char*)szStr,strlen(szStr));
					}
					else if (!strncmp(szDeliveryElementsStr[k], "EndTankWeight", strlen("EndTankWeight"))) 
					{
						info->lTag = a2l((unsigned char*)szStr,strlen(szStr));
					}
					//4.0.20.52	}
					//4.0.20.501 TD 63114 - (need to verify RFS XML schema):
					else if (!strncmp(szDeliveryElementsStr[k], "DeliveryRef", strlen("DeliveryRef")))
					{
						info->lDeliveryRef = a2l((unsigned char*)szStr,strlen(szStr));
					}
					else if (!strncmp(szDeliveryElementsStr[k], "DriverID", strlen("DriverID")))
					{
						memset(info->sDriverID,' ',sizeof(info->sDriverID));
						memcpy(info->sDriverID,szStr,min(sizeof(info->sDriverID),strlen(szStr)));
					}
					//Start 4.0.23.500 - TD 72606
					else if (!strncmp(szDeliveryElementsStr[k], "StartingTcVolume", strlen("StartingTcVolume"))) 
					{
						info->lStartingTcVolume = a2l((unsigned char*)szStr,strlen(szStr));
					}
					else if (!strncmp(szDeliveryElementsStr[k], "StartingWater", strlen("StartingWater"))) 
					{
						info->lStartingWater = a2l((unsigned char*)szStr,strlen(szStr));
					}
					else if (!strncmp(szDeliveryElementsStr[k], "StartingHeight", strlen("StartingHeight"))) 
					{
						info->lStartingHeight = a2l((unsigned char*)szStr,strlen(szStr));
					}
					else if (!strncmp(szDeliveryElementsStr[k], "EndingTcVolume", strlen("EndingTcVolume"))) 
					{
						info->lEndingTcVolume = a2l((unsigned char*)szStr,strlen(szStr));
					}
					else if (!strncmp(szDeliveryElementsStr[k], "EndingWater", strlen("EndingWater"))) 
					{
						info->lEndingWater = a2l((unsigned char*)szStr,strlen(szStr));
					}
					else if (!strncmp(szDeliveryElementsStr[k], "EndingHeight", strlen("EndingHeight"))) 
					{
						info->lEndingHeight = a2l((unsigned char*)szStr,strlen(szStr));
					}
					else if (!strncmp(szDeliveryElementsStr[k], "ActionID", strlen("ActionID")))    //TD 367735
					{
						info->ucActionCode = (unsigned char)a2l((unsigned char*)szStr,strlen(szStr));
					}					

					//End 4.0.23.500 - TD 72606
				}
			}
		
		}

	}
	cXmlInterface.CloseDoc();
}

//4.0.13.505
/******************************************************************************
 Description: Convert the Delivery DELIVERY_INFO struct to xml	 	
 Returns:      	none
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			13/2/2007   18:39		Change - TD 39699 add 7 new fields to xml //4.0.19.153
 ShellyB        05/08/2008  10:00       Change - TD 63114 add 3 new fields : DeliveryRef, Flags, DriverID //4.0.20.501
******************************************************************************/
void CTankXML::DeliveryInfo2Xml(DELIVERY_INFO* info, BSTR* pRec)
{

	CXMLInterface	cXmlDeliveryInfo;
	CXMLPointers	cXMLPointers;
	CXMLPointers	cXMLPointers2;
	char			sDataAndtime[20];
	long            lTankNumber; 
	long            lTmpRef;

	if(cXmlDeliveryInfo.InitDoc() == TRUE)
	{
	
		cXmlDeliveryInfo.CreateElement("DeliveryInfo",&cXMLPointers);
		cXmlDeliveryInfo.AppendRootElement(&cXMLPointers);
		
		cXmlDeliveryInfo.CreateElement("Delivery",&cXMLPointers2);

        lTankNumber = info->lTank;

		cXmlDeliveryInfo.SetLongAttribute("DeliveryID",info->lIndexNumber, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("TankNumber",lTankNumber, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("Product",info->lProduct, &cXMLPointers2);

		cXmlDeliveryInfo.SetLongAttribute("Status",info->lStatus, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("Tag",info->lTag, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("DeliveredVolume",info->lDeliveredVolume, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("ShiftNumber",info->dwShiftNumber, &cXMLPointers2);

		//setting time info
		memset(sDataAndtime,' ',sizeof(sDataAndtime));
		sprintf(sDataAndtime,"%04d-%02d-%02d,%02d:%02d:%02d",info->cDeliveryDateTime.wYear,info->cDeliveryDateTime.wMonth,info->cDeliveryDateTime.wDay,info->cDeliveryDateTime.wHour,info->cDeliveryDateTime.wMinute,info->cDeliveryDateTime.wSecond);
		cXmlDeliveryInfo.SetCharAttribute("DeliveryDateTime",sDataAndtime,&cXMLPointers2);

		//setting time info
		memset(sDataAndtime,' ',sizeof(sDataAndtime));
		sprintf(sDataAndtime,"%04d-%02d-%02d,%02d:%02d:%02d",info->cEndDeliveryDateTime.wYear,info->cEndDeliveryDateTime.wMonth,info->cEndDeliveryDateTime.wDay,info->cEndDeliveryDateTime.wHour,info->cEndDeliveryDateTime.wMinute,info->cEndDeliveryDateTime.wSecond);
		cXmlDeliveryInfo.SetCharAttribute("EndDeliveryDateTime",sDataAndtime,&cXMLPointers2);


		cXmlDeliveryInfo.SetLongAttribute("StartVolume",info->lStartVolume, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("EndVolume",info->lEndVolume, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("Temperature",info->lTemperature, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("Density",info->lDensity, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("Weight",info->lWeight, &cXMLPointers2);

		//4.0.20.52 {
		cXmlDeliveryInfo.SetLongAttribute("StartTankDensity",info->lStartDensity, &cXMLPointers2);  
		cXmlDeliveryInfo.SetLongAttribute("EndTankDensity",info->lDensity, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("StartTankTemperature",info->lStartTemperature, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("EndTankTemperature",info->lTemperature, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("StartTankWeight",info->lStartWeight, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("EndTankWeight",info->lTag, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("DeliveryWeight",info->lWeight, &cXMLPointers2);
		//4.0.20.52 }

		//An inside sync number:
		cXmlDeliveryInfo.SetLongAttribute("ActiveReadNumber",info->dwActiveReadNumber, &cXMLPointers2);

		//The actual reference number we got from BOS on start/end delivery:
		
		//4.0.20.501 - TD 65174:
		if (_Module.m_server.GetSupportDeliveryEmulation() == TRUE)
		{
			lTmpRef = _Module.m_server.m_cTankArray[lTankNumber - 1].GetDeliveryReference();
			if (lTmpRef == 0)
				lTmpRef = _Module.m_server.GetInvoiceRefNumber();
		}
		else
		{
			if (info->nFlags & DELIVERY_REPORT_INVOICE_DATA)		//Process from terminal will be with this flag			//4.0.23.503 TD 78620
				lTmpRef = info->lDeliveryRef;
			else
				lTmpRef = _Module.m_server.GetInvoiceRefNumber();   //If flag is not there - this is a process from BOS - take from ini file	
		}

		cXmlDeliveryInfo.SetLongAttribute("BOSRefNumber",lTmpRef,&cXMLPointers2);

        //4.0.20.501 TD 63114 - (need to verify VP XML schema):

		//Delivery reference field from terminal - only for CR 63114
		//("DeliveryRef", and "DriverID" will be sent only in non actual delivery (only in the delivery info which is sent for this CR). 
		//Won't be sent in a actual delivery)
		cXmlDeliveryInfo.SetLongAttribute("DeliveryRef",info->lDeliveryRef, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("Flags",info->nFlags, &cXMLPointers2);
		
		//4.0.23.500 - TD 72606
		if (!ChrAll((char *)info->sDriverID, sizeof(info->sDriverID), ' '))
			cXmlDeliveryInfo.SetCharAttribute("DriverID",(char *)info->sDriverID, &cXMLPointers2);

		//Start 4.0.23.500 - TD 72606
		//setting date and time info for inserting Qdex record
		if (info->cQdxDateTime.wYear > 0)
		{
			memset(sDataAndtime,' ',sizeof(sDataAndtime));
			sprintf(sDataAndtime,"%04d-%02d-%02d,%02d:%02d:%02d",info->cQdxDateTime.wYear,info->cQdxDateTime.wMonth,info->cQdxDateTime.wDay,info->cQdxDateTime.wHour,info->cQdxDateTime.wMinute,info->cQdxDateTime.wSecond);
			cXmlDeliveryInfo.SetCharAttribute("DateAndTime",sDataAndtime,&cXMLPointers2);
		}

		cXmlDeliveryInfo.SetLongAttribute("StartingTcVolume",info->lStartingTcVolume, &cXMLPointers2); 
		cXmlDeliveryInfo.SetLongAttribute("StartingWater",info->lStartingWater, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("StartingHeight",info->lStartingHeight, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("EndingTcVolume",info->lEndingTcVolume, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("EndingWater",info->lEndingWater, &cXMLPointers2);
		cXmlDeliveryInfo.SetLongAttribute("EndingHeight",info->lEndingHeight, &cXMLPointers2);	
		//End 4.0.23.500 - TD 72606

		//4.0.23.500 - TD 77765 - indication if we are in the middle of shift
		long lEndOfShiftStatus = a2l(&(info->byEndOfShiftStatus), sizeof(info->byEndOfShiftStatus));
		cXmlDeliveryInfo.SetLongAttribute("EndOfShiftStatus",lEndOfShiftStatus,&cXMLPointers2);

		cXmlDeliveryInfo.SetLongAttribute("ActionID",(long)info->ucActionCode, &cXMLPointers2);

		cXmlDeliveryInfo.SetLongAttribute("StartWaterLevel",info->lStartWaterLevel, &cXMLPointers2);	// 4.0.25.1060  TD 377336
		cXmlDeliveryInfo.SetLongAttribute("EndWaterLevel",info->lEndWaterLevel, &cXMLPointers2);	    // 4.0.25.1060  TD 377336

		cXmlDeliveryInfo.AppendChild(&cXMLPointers2,&cXMLPointers);

		cXmlDeliveryInfo.GetXmlString((BSTR*)pRec);	

	}

	cXmlDeliveryInfo.CloseDoc();
}

/******************************************************************************
 Description: Build a tank's reading XML to BOS, as a part of the delivery process	 	
 Returns:      	none
 Parameters:   	info - the tank object
                pRec - the XML
                bAddReadings - wether to add the actual reading data
				bAddDeliveryIndication - wether to add a deliveryIndication
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Shelly B		07/10/2008   18:04		Start 4.0.21.500 TD 66935
******************************************************************************/
void CTankXML::TankReadingInfoDeliveryXml(CTank* info, BSTR* pRec, BOOL bAddReadings, BOOL bAddDeliveryIndication)
{

	CXMLInterface	cXmlTankInfo;
	CXMLPointers	cXMLPointers;
	CXMLPointers	cXMLPointers2;

	if(cXmlTankInfo.InitDoc() == TRUE)
	{
	
		cXmlTankInfo.CreateElement("TankReadings",&cXMLPointers);
		cXmlTankInfo.AppendRootElement(&cXMLPointers);
		
		cXmlTankInfo.CreateElement("Tank",&cXMLPointers2);

		cXmlTankInfo.SetLongAttribute("TankID",info->m_lTankNumber, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("Reference",info->GetDeliveryReference(),&cXMLPointers2);

		if (bAddReadings) //End of reading process:
		{
			if (!bAddDeliveryIndication) ///End of START:
			{
				cXmlTankInfo.SetLongAttribute("FuelVolume",info->GetStartDeliveryEmulationVolume(),   &cXMLPointers2);
		        cXmlTankInfo.SetLongAttribute("WaterVolume",info->GetStartDeliveryEmulationWaterVolume(), &cXMLPointers2);
		        cXmlTankInfo.SetLongAttribute("Temperature",info->GetStartDeliveryEmulationTankTemperature(), &cXMLPointers2);
				cXmlTankInfo.SetLongAttribute("LevelFactor",info->m_lLevelFactor, &cXMLPointers2);
				cXmlTankInfo.SetLongAttribute("TemperatureFactor",info->m_lTemperatureFactor, &cXMLPointers2);
				cXmlTankInfo.SetLongAttribute("VolumeFactor",info->m_lVolumeFactor, &cXMLPointers2);
				cXmlTankInfo.SetLongAttribute("Density",info->GetStartDeliveryEmulationDensity(), &cXMLPointers2);	//4.0.28.500 - TD 250092
				cXmlTankInfo.SetLongAttribute("DensityFactor",FUEL_DELIVERY_DENSITY_FACTOR, &cXMLPointers2);		//4.0.28.500 - TD 250092
				cXmlTankInfo.SetLongAttribute("Weight",info->GetStartDeliveryEmulationWeight(), &cXMLPointers2);	//4.0.28.500 - TD 250092
				cXmlTankInfo.SetLongAttribute("WeightFactor",FUEL_DELIVERY_WEIGHT_FACTOR, &cXMLPointers2);			//4.0.28.500 - TD 250092
				cXmlTankInfo.SetLongAttribute("FuelLevel",info->GetStartDeliveryEmulationProductLevel(), &cXMLPointers2);	// 4.0.25.1060  TD 377336
				cXmlTankInfo.SetLongAttribute("WaterLevel",info->GetStartDeliveryEmulationWaterLevel(), &cXMLPointers2);	// 4.0.25.1060  TD 377336
			}
			else ///End of END:
			{
				cXmlTankInfo.SetLongAttribute("FuelVolume",info->GetEndDeliveryEmulationVolume(),   &cXMLPointers2);
		        cXmlTankInfo.SetLongAttribute("WaterVolume",info->GetEndDeliveryEmulationWaterVolume(), &cXMLPointers2);
		        cXmlTankInfo.SetLongAttribute("Temperature",info->GetEndDeliveryEmulationTankTemperature(), &cXMLPointers2);
				cXmlTankInfo.SetLongAttribute("LevelFactor",info->m_lLevelFactor, &cXMLPointers2);
				cXmlTankInfo.SetLongAttribute("TemperatureFactor",info->m_lTemperatureFactor, &cXMLPointers2);
				cXmlTankInfo.SetLongAttribute("VolumeFactor",info->m_lVolumeFactor, &cXMLPointers2);
				cXmlTankInfo.SetLongAttribute("Delivery",info->GetFoundDelivery(), &cXMLPointers2);
				cXmlTankInfo.SetLongAttribute("Density",info->GetEndDeliveryEmulationDensity(), &cXMLPointers2);	//4.0.28.500 - TD 250092
				cXmlTankInfo.SetLongAttribute("DensityFactor",FUEL_DELIVERY_DENSITY_FACTOR, &cXMLPointers2);		//4.0.28.500 - TD 250092
				cXmlTankInfo.SetLongAttribute("Weight",info->GetEndDeliveryEmulationWeight(), &cXMLPointers2);		//4.0.28.500 - TD 250092
				cXmlTankInfo.SetLongAttribute("WeightFactor",FUEL_DELIVERY_WEIGHT_FACTOR, &cXMLPointers2);			//4.0.28.500 - TD 250092		
				cXmlTankInfo.SetLongAttribute("FuelLevel",info->GetEndDeliveryEmulationProductLevel(), &cXMLPointers2);	// 4.0.25.1060  TD 377336
				cXmlTankInfo.SetLongAttribute("WaterLevel",info->GetEndDeliveryEmulationWaterLevel(), &cXMLPointers2);	// 4.0.25.1060  TD 377336				
			}
		}
		
		cXmlTankInfo.AppendChild(&cXMLPointers2,&cXMLPointers);

		cXmlTankInfo.GetXmlString((BSTR*)pRec);	

		cXmlTankInfo.CloseDoc();

	}


}



long CTankXML::GetDensityFromXML(BSTR & bstrXml)
{
	if(bstrXml)
	{
		TANK_READ_INFO info;
		Xml2TankReadingInfo(&bstrXml, &info);
		return info.lDensity;
	}
	
	return 0;
}


void CTankXML::TankStateXml(CTank* info, BSTR * pRec)
{

	CXMLInterface	cXmlTankInfo;
	CXMLPointers	cXMLPointers;
	CXMLPointers	cXMLPointers2;
	char			sTankStateDesc[20];

	if (cXmlTankInfo.InitDoc() == TRUE)
	{

		cXmlTankInfo.CreateElement("TankState", &cXMLPointers);
		cXmlTankInfo.AppendRootElement(&cXMLPointers);

		cXmlTankInfo.CreateElement("Tank", &cXMLPointers2);

		cXmlTankInfo.SetLongAttribute("TankID", info->m_lTankNumber, &cXMLPointers2);
		cXmlTankInfo.SetLongAttribute("State", info->GetTankState(), &cXMLPointers2);


		memset(sTankStateDesc, ' ', sizeof(sTankStateDesc));
		sprintf(sTankStateDesc, "%s", info->GeTankStateDesc());
		cXmlTankInfo.SetCharAttribute("Description", sTankStateDesc, &cXMLPointers2);


		cXmlTankInfo.AppendChild(&cXMLPointers2, &cXMLPointers);

		cXmlTankInfo.GetXmlString((BSTR*)pRec);
		cXmlTankInfo.CloseDoc();

	}
}


void CTankXML::AllTanksStateXml(BSTR * pRec)
{
	CXMLInterface	cXmlTankInfo;
	CXMLPointers	cXMLPointers;
	CXMLPointers	cXMLPointers2;
	char			sTankStateDesc[20];

	if (cXmlTankInfo.InitDoc() == TRUE)
	{
		cXmlTankInfo.CreateElement("TankState", &cXMLPointers);
		cXmlTankInfo.AppendRootElement(&cXMLPointers);

		for (long ind = 0; ind <= _Module.m_server.GetLastValidTank(); ind++)
		{
			cXmlTankInfo.CreateElement("Tank", &cXMLPointers2);

			cXmlTankInfo.SetLongAttribute("TankID", _Module.m_server.m_cTankArray[ind].m_lTankNumber, &cXMLPointers2);
			cXmlTankInfo.SetLongAttribute("State", _Module.m_server.m_cTankArray[ind].GetTankState(), &cXMLPointers2);

			memset(sTankStateDesc, ' ', sizeof(sTankStateDesc));
			sprintf(sTankStateDesc, "%s", _Module.m_server.m_cTankArray[ind].GeTankStateDesc());
			cXmlTankInfo.SetCharAttribute("Description", sTankStateDesc, &cXMLPointers2);
			cXmlTankInfo.AppendChild(&cXMLPointers2, &cXMLPointers);
		}

		cXmlTankInfo.GetXmlString((BSTR*)pRec);
		cXmlTankInfo.CloseDoc();
	}
}

