#pragma once

#include <atlstr.h>
#include "PumpClass.h"

//CR 404913 

class CCard
{
public:

	virtual CString GetAccountNumber() const = 0;
	virtual CString GetFormatAccountNumber() const = 0;
	virtual CString GetExpireDate() const = 0;
	virtual CString GetFullCardName() const = 0;
	virtual CString GetRefernceNumber() const = 0;
	virtual CString GetMediaMasterType() const = 0;
	virtual CString GetCompletedSessionId() const = 0;
	virtual CString GetTenderIdStr() const = 0;
	virtual void SetAccountNumber(CString sAccountNumber) = 0;
	virtual long GetUsedAmount() const = 0;

//	virtual void SetTrack2(CString val) = 0;

};