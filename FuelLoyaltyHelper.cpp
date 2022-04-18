#include "stdafx.h"
#include "FuelLoyaltyHelper.h"
#include "stdlib.h"



CFuelLoyaltyHelper::CFuelLoyaltyHelper()
{
}

BOOL CFuelLoyaltyHelper::GetLoyaltyCustomerFileName(char * sPath, CString LoyaltyPath, long lPumpNumber)
{
	if (!(LoyaltyPath.IsEmpty()) &&  mLoyaltyPath.IsEmpty())
		mLoyaltyPath = LoyaltyPath; //set loyalty path only once;
	strcat_s((char *)sPath, MAX_FILE_PATH, mLoyaltyPath);
	if (!strlen(sPath))
		strcat_s((char *)sPath, MAX_FILE_PATH, "c:\\Office\\PumpSrv\\Data");
	char sFileName[MAX_FILE_PATH] = { 0 };
	sprintf_s(sFileName, MAX_FILE_PATH, "LoyaltyCustomer.xml");
	strcat_s((char *)sPath, MAX_FILE_PATH, "\\");
	strcat_s((char *)sPath, MAX_FILE_PATH, (char *)sFileName);
    return TRUE;
}

BOOL CFuelLoyaltyHelper::GetTransactionDatalong(PAY_AT_PUMP_INFO & pInfo, CString FieldName, long & RetVal)
{
	BOOL bSuccessfull = FALSE;
	if (FieldName == "sTranNbr")
	{
		BYTE		sTrsNumber[6] = { 0 }; //4.0.23.1310
		memset(sTrsNumber, 0, sizeof(sTrsNumber));    //4.0.23.1310
		memcpy(sTrsNumber, pInfo.CardSaleInfo.cardData.sTranNbr, min(sizeof(sTrsNumber), sizeof(pInfo.CardSaleInfo.cardData.sTranNbr)));	//4.0.19.500 55286  //4.0.23.1310
		RetVal = a2l(sTrsNumber, strlen((char*)sTrsNumber));
		bSuccessfull = TRUE;
	}
	return bSuccessfull;
}

BOOL CFuelLoyaltyHelper::GetTransactionDataString(PAY_AT_PUMP_INFO & pInfo, CString FieldName, CString & RetVal)
{
	BOOL bSuccessfull = FALSE;
	int StrLength = 0;
	try
	{
		if (FieldName == "sTransactionId")
		{
			long lStrLen = sizeof(pInfo.CardSaleExtraData6.sTransactionId);
			char tmpStr[MAX_PATH];
			memset(tmpStr, 0, MAX_PATH);
			memcpy(tmpStr, &(pInfo.CardSaleExtraData6.sTransactionId), min(_countof(tmpStr), lStrLen));
			CString cStr = tmpStr;
			cStr.Trim();
			RetVal = cStr;
			bSuccessfull = TRUE;
		}

	}
	catch (...)
	{
			_LOGMSG.LogMsg("CFuelLoyaltyHelper::GetTransactionDataString, Unexpected error has occurred");
			bSuccessfull = FALSE;
	}

	return bSuccessfull;
}

CString CFuelLoyaltyHelper::GetNameOfCarWashProgramByID(PAY_AT_PUMP_INFO & cTmpPapInfo, long lProgramNameIndx /*= -1*/)
{
	CString strCarWash;
	if (lProgramNameIndx == -1)
		lProgramNameIndx = a2l((BYTE *)&cTmpPapInfo.CardSaleInfo.extraData.sCarWashProgram, sizeof(cTmpPapInfo.CardSaleInfo.extraData.sCarWashProgram));
	if (lProgramNameIndx)
	{
		TStaticString<STATIC_STRING_64> sCarWashProg;
		sCarWashProg.Clear();
		strCarWash.Format("WashPrograms\\WashProgram%02d", lProgramNameIndx);
		_Module.m_server.m_cParam.LoadParam((LPCSTR)strCarWash, "FullName", (char *)sCarWashProg, sCarWashProg.Size(), "", FALSE);
		strCarWash = sCarWashProg;
	}
	return strCarWash;
}

long CFuelLoyaltyHelper::GetWashProgramNacsCodeByProgramCode(long lProdCode)
{
	long lRetVal = -1;
	CString str("WashPrograms\\WashProgram");
	char sTmpStr[MAX_FIELD_VAL];
	ZeroMemory((char *)sTmpStr, sizeof(sTmpStr));
	char sNumber[8];
	memset(sNumber, 0, sizeof(sNumber));
	sprintf_s(sNumber, sizeof sNumber,"%02d", lProdCode);
	str += sNumber;
	_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "CCH_Product_Code", sTmpStr, sizeof(sTmpStr), sNumber, TRUE);
	lRetVal = a2l((BYTE *)&sTmpStr, sizeof(sTmpStr));
	return lRetVal;
}


CString CFuelLoyaltyHelper::mLoyaltyPath;

CFuelLoyaltyHelper::~CFuelLoyaltyHelper()
{
}

