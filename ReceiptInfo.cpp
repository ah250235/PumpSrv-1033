#include "stdafx.h"
#include "ReceiptInfo.h"
#include "InfoData.h"

namespace PS
{
	CReceiptInfo::CReceiptInfo()
		: m_TrsIndex(0)
		, m_PumpNumber(0)
		, m_HashedKey("")
		, m_VehicleKey("")
	{
		memset(&m_PAPInfo, 0, sizeof(m_PAPInfo));
		memset(&m_PumpTransact, 0, sizeof(m_PumpTransact));
	}

	CReceiptInfo::~CReceiptInfo()
	{
	}

	void CReceiptInfo::Clear(void)
	{
		__super::Clear();

		m_TrsIndex = 0;
		m_PumpNumber = 0;
		m_HashedKey = "";
		m_VehicleKey = "";
		memset(&m_PAPInfo, 0, sizeof(m_PAPInfo));
		memset(&m_PumpTransact, 0, sizeof(m_PumpTransact));
	}


	const CReceiptInfo & CReceiptInfo::operator = (const CInfoData & rhs)
	{
		*(CInfoData *) this = rhs;

		return *this;
	}
};