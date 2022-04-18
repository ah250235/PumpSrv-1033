// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IServicesDll wrapper class

#if !defined(_OLA32_H__INCLUDED_)
#define _OLA32_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class IServicesDll : public COleDispatchDriver
{
public:
	IServicesDll() {}		// Calls COleDispatchDriver default constructor
	IServicesDll(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IServicesDll(const IServicesDll& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long StartSession(long ClientID, LPCTSTR InBuffer, LPCTSTR TransactionType);
	long QuerySession(long SessionId, BSTR* OutBuffer);
	BOOL EndSession(long SessionId);
	void AddParams(long SessionId, LPCTSTR InBuffer);
	long GetProvidersStatus(VARIANT* ProviderStatus);
	CString GetTenderAllowedDepartments(LPCTSTR AccountNumber, LPCTSTR Track2Data);
	CString GetTenderNotAllowedDepartments(LPCTSTR AccountNumber, LPCTSTR Track2Data);
	BOOL CloseBatch(LPCTSTR LineDelimeter, BSTR* DetailReport, BSTR* SummaryReport, const VARIANT& DetailBuffer, const VARIANT& SummaryBuffer, long Flags, long* ResultCode, BSTR* ErrorReport);
	void LockService();
	void UnlockService();
	long CurrentBatchId();
};

#endif // !defined(_OLA32_H__INCLUDED_)