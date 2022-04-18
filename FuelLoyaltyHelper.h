#pragma once
class CFuelLoyaltyHelper
{
public:
	static CString mLoyaltyPath;
	CFuelLoyaltyHelper();
	BOOL static GetLoyaltyCustomerFileName(char * sPath,CString LoyaltyPath, long lPumpNumber);
	BOOL static GetTransactionDatalong(PAY_AT_PUMP_INFO & pInfo, CString FieldName, long & RetVal);
	BOOL static GetTransactionDataString(PAY_AT_PUMP_INFO & pInfo, CString FieldName, CString & RetVal);
	CString static GetNameOfCarWashProgramByID(PAY_AT_PUMP_INFO & cTmpPapInfo, long lProgramNameIndx = -1);
	long static GetWashProgramNacsCodeByProgramCode(long lProdCode);
	~CFuelLoyaltyHelper();
};
