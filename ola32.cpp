// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "ola32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// IServicesDll properties

/////////////////////////////////////////////////////////////////////////////
// IServicesDll operations

long IServicesDll::StartSession(long ClientID, LPCTSTR InBuffer, LPCTSTR TransactionType)
{
	long result;
	static BYTE parms[] =
		VTS_I4 VTS_BSTR VTS_BSTR;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		ClientID, InBuffer, TransactionType);
	return result;
}

long IServicesDll::QuerySession(long SessionId, BSTR* OutBuffer)
{
	long result;
	static BYTE parms[] =
		VTS_I4 VTS_PBSTR;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		SessionId, OutBuffer);
	return result;
}

BOOL IServicesDll::EndSession(long SessionId)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		SessionId);
	return result;
}

void IServicesDll::AddParams(long SessionId, LPCTSTR InBuffer)
	{
	static BYTE parms[] =
		VTS_I4 VTS_BSTR;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 SessionId, InBuffer);
}

long IServicesDll::GetProvidersStatus(VARIANT* ProviderStatus)
{
	long result;
	static BYTE parms[] =
		VTS_PVARIANT;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		ProviderStatus);
	return result;
}

CString IServicesDll::GetTenderAllowedDepartments(LPCTSTR AccountNumber, LPCTSTR Track2Data)
{
	CString result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		AccountNumber, Track2Data);
	return result;
}

CString IServicesDll::GetTenderNotAllowedDepartments(LPCTSTR AccountNumber, LPCTSTR Track2Data)
{
	CString result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms,
		AccountNumber, Track2Data);
	return result;
}

BOOL IServicesDll::CloseBatch(LPCTSTR LineDelimeter, BSTR* DetailReport, BSTR* SummaryReport, const VARIANT& DetailBuffer, const VARIANT& SummaryBuffer, long Flags, long* ResultCode, BSTR* ErrorReport)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR VTS_PBSTR VTS_PBSTR VTS_VARIANT VTS_VARIANT VTS_I4 VTS_PI4 VTS_PBSTR;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		LineDelimeter, DetailReport, SummaryReport, &DetailBuffer, &SummaryBuffer, Flags, ResultCode, ErrorReport);
	return result;
}

void IServicesDll::LockService()
{
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IServicesDll::UnlockService()
{
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

long IServicesDll::CurrentBatchId()
{
	long result;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}
