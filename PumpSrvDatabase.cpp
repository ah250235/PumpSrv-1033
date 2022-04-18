#include "stdafx.h"
#include "PumpSrvDatabase.h"
#include "SemaphoreLock.h"
#include "FuelErrorInfo.h"
#include "PumpSrvRealTimeInfoDB.h"
#include "FuelDriver_PumpSrv.h"
#include "PumpSrvStructures.h"
#include "PumpSrvPumpTotalsDB.h"
#include "PumpSrvTankReadDB.h"
#include "PumpSrvFuelDeliveriesDB.h"
#include "PumpSrvShiftRecDB.h"
#include "PumpSrvPrePayDB.h"
#include "PumpSrvServerInfoDB.h"
#include "PumpSrvExtraInfoDB.h"
#include "PumpSrvReceiptsDB.h"
#include "PumpSrvAlarmSrvInfoDB.h"
#include "PumpSrvCarWashDB.h"
#include "PumpSrvOLATrsDB.h"
#include "PumpSrvOLATempTrsDB.h"
#include "PumpSrvItemsDB.h"
#include "PumpSrvPaidTrsDB.h"
#include "PumpSrvTankGaugeDB.h"
#include "PumpSrvAsh200DB.h"
#include "PumpSrvSingleShiftAccumulationDB.h"
#include "FuelDriver_Maintenance.h"
#include "TimingProfiling.h"
#include "HelperFunctions.h"


CPumpSrvDatabase::CPumpSrvDatabase()
{
	m_LockingSemaphore.reset(new CLockingSemaphore);
}

CPumpSrvDatabase::~CPumpSrvDatabase()
{
}

bool CPumpSrvDatabase::Initialize(void)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	auto ReturnCode = ConnectNewDBManagerInstance_DefaultSQL();

	if (ReturnCode == DB_SUCCESS)
	{
		auto & FuelDriver_Maintenance = m_AbstractDBManager->GetFuelDriver_Maintenance();
		FuelDriver_Maintenance.ValidatePumpSrvDatabase();
	}

	return (ReturnCode == DB_SUCCESS);
}

void CPumpSrvDatabase::GetFuelMsSqlVersion(CString & version)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	auto ReturnCode = ConnectNewDBManagerInstance_DefaultSQL();

	if (ReturnCode == DB_SUCCESS)
	{
		auto & FuelDriver_Maintenance = m_AbstractDBManager->GetFuelDriver_Maintenance();
		version = FuelDriver_Maintenance.GetFuelMsSqlVersion();
	}
}

bool CPumpSrvDatabase::Close(void)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	if (!m_LockingSemaphore.get())
		return true;

	DbManagerRetCode ReturnCode = DB_SUCCESS;

	{
		CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

		if (m_AbstractDBManager.get())
		{
			DbManagerRetCode ReturnCode = m_AbstractDBManager->CloseDatabase();

			m_AbstractDBManager.reset(NULL);
		}

		m_FuelDB.CloseDatabase();
	}

	m_Callbacks.clear();

	m_LockingSemaphore.reset(NULL);

	return (ReturnCode == DB_SUCCESS);
}
bool CPumpSrvDatabase::ValidateDatabase(void)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	auto & FuelDriver_Maintenance = m_AbstractDBManager->GetFuelDriver_Maintenance();

	bool bReturnValue = FuelDriver_Maintenance.ValidatePumpSrvDatabase();

	return bReturnValue;
}

bool CPumpSrvDatabase::LoadPayAtPumpInfo(PS::PAY_AT_PUMP_INFO & cTmpPumpInfo, long lPumpNumber)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadRealTimeInfo(lPumpNumber, cTmpPumpInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SavePayAtPumpInfo(const PS::PAY_AT_PUMP_INFO & Record, long lPumpNumber, LPCSTR szFunction, int LineNumber)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	if (!&Record)
	{
		CString csLogMessage;

		csLogMessage.Format("%s:%d - Record references a null pointer.", szFunction, LineNumber);

		m_AbstractDBManager->PostError(csLogMessage);

		return false;
	}

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveRealTimeInfo(lPumpNumber, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadPumpInfo(PUMP_INFO & PumpInfo, long lPumpNumber)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	CByteArray TempBuffer;
	TempBuffer.SetSize(sizeof(PumpInfo));

	bool bReturnValue = (FuelDriver.LoadRealTimeInfoPumpInfo(lPumpNumber, TempBuffer) == DB_SUCCESS);
	if (bReturnValue)
	{
		PumpInfo = *(PUMP_INFO *)TempBuffer.GetData();
	}

	return bReturnValue;
}

bool CPumpSrvDatabase::GetRealTimeInfoCount(size_t & Count, LPCSTR szFunction, int LineNumber)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.GetRealTimeInfoCount(Count) == DB_SUCCESS) ? true : false;
}

bool CPumpSrvDatabase::SaveRealTimeInfo(const PAY_AT_PUMP_INFO & PayAtPumpInfoRecord, const PUMP_INFO & PumpInfo, long lPumpNumber, LPCSTR szFunction, int LineNumber)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	CByteArray TempBuffer;

	TempBuffer.SetSize(sizeof(PumpInfo));
	memcpy(TempBuffer.GetData(), &PumpInfo, sizeof(PumpInfo));

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveRealTimeInfo(lPumpNumber, PayAtPumpInfoRecord, TempBuffer) == DB_SUCCESS);
}

DbManagerRetCode CPumpSrvDatabase::ConnectNewDBManagerInstance_DefaultSQL(void)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	DbManagerRetCode ReturnCode = ConnectNewDBManagerInstance(_T("CFuelMSSQLDBManager"), _T(".\\FuelDB"), _T(""), _T("FuelMSSQLDB.dll"));

	return ReturnCode;
}

void CPumpSrvDatabase::PostError(const CFuelErrorInfo & FuelErrorInfo)
{
	TRACE(FuelErrorInfo.GetErrorText());

	for (auto pCallBack : m_Callbacks)
	{
		pCallBack->PostError(FuelErrorInfo);
	}
}

DbManagerRetCode CPumpSrvDatabase::ConnectNewDBManagerInstance(const CString & DBManagerName, const CString & DBInstanceName, const CString & DBParameters, const CString & DBLibraryPath)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	DbManagerRetCode ReturnCode = DB_SUCCESS;

	m_DBManagerName = DBManagerName;
	m_DBInstanceName = DBInstanceName;
	m_DBParameters = DBParameters;
	m_DBLibraryPath = DBLibraryPath;

	m_AbstractDBManager.reset(CAbstractDBManager::CreateNewDBManagerInstance(DBManagerName, DBLibraryPath));

	if (!m_AbstractDBManager)
	{
		ReturnCode = DB_UNABLE_TO_CREATE_DB_INSTANCE;

		CString ErrorString;
		ErrorString.Format(_T("%s:%d - ErrorCode = %d (%s), DBManagerName = '%s', DBLibraryPath = '%s'")
			, __FUNCTION__
			, __LINE__
			, ReturnCode
			, CAbstractDBManager::GetErrorCodeName(ReturnCode)
			, DBManagerName
			, DBLibraryPath
			);
		 
		PostError(CFuelErrorInfo(ErrorString, CFuelErrorInfo::FuelLogMessageError));

		return ReturnCode;
	}

	ReturnCode = m_AbstractDBManager->OpenDatabase(DBInstanceName, DBParameters);
	if (DB_SUCCESS != ReturnCode)
	{
		m_AbstractDBManager.reset();

		CString ErrorString;
		ErrorString.Format(_T("%s:%d - ErrorCode = %d (%s), DBManagerName = '%s', DBLibraryPath = '%s', DBInstanceName = '%s', DBParameters = '%s'")
			, __FUNCTION__
			, __LINE__
			, ReturnCode
			, CAbstractDBManager::GetErrorCodeName(ReturnCode)
			, DBManagerName
			, DBLibraryPath
			, DBInstanceName
			, DBParameters
			);

		PostError(CFuelErrorInfo(ErrorString, CFuelErrorInfo::FuelLogMessageError));

		return ReturnCode;
	}

	m_AbstractDBManager->AddCallback(this);

	return ReturnCode;
}

void CPumpSrvDatabase::AddCallBack(CPumpSrvDatabaseCallback * pCallBack)
{
	m_Callbacks.push_back(pCallBack);
}

void CPumpSrvDatabase::RemoveCallBack(CPumpSrvDatabaseCallback * pCallBack)
{
	for (auto Iter = m_Callbacks.begin(); Iter != m_Callbacks.end(); ++Iter)
	{
		if (*Iter == pCallBack)
		{
			// We found the call back. Erase it, and break out of the loop.
			m_Callbacks.erase(Iter);
			break;
		}
	}
}

bool CPumpSrvDatabase::SavePumpTotals(long lIndex, const PS::PumpTotals & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SavePumpTotals(lIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadPumpTotals(long lIndex, PS::PumpTotals & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadPumpTotals(lIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadNextPumpTotals(long lIndex, PS::PumpTotals & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadNextPumpTotals(lIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadShift(PS::SHIFT_REC & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Load(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SaveShift(const PS::SHIFT_REC & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveShiftRecord(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadServerInfo(PS::SERVER_INFO & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadServerInfo(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SaveServerInfo(const PS::SERVER_INFO & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveServerInfo(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstPumpTotals(PS::PumpTotals & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadFirstPumpTotals(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::DeletePumpTotals(long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return(FuelDriver.DeletePumpTotals(lIndex) == DB_SUCCESS);
}


bool CPumpSrvDatabase::SaveFuelDelivery(long lIndex, const DELIVERY_INFO & FuelDeliveryInfo)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveDeliveryInfo(lIndex, FuelDeliveryInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFuelDelivery(long lIndex, DELIVERY_INFO & FuelDeliveryInfo)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadDeliveryInfo(lIndex, FuelDeliveryInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstFuelDelivery(DELIVERY_INFO & FuelDeliveryInfo)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadFirstDeliveryInfo(FuelDeliveryInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadNextFuelDelivery(long lIndex, DELIVERY_INFO & FuelDeliveryInfo)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadNextDeliveryInfo(lIndex, FuelDeliveryInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::DeleteFuelDelivery(long lIndex)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.DeleteFuelDelivery(lIndex) == DB_SUCCESS);
}




bool CPumpSrvDatabase::SavePrePayInfo(PS::PUMPSRV_PRE_PAY_REC & PrePayInfo, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Save(lIndex, PrePayInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadPrePayInfo(PS::PUMPSRV_PRE_PAY_REC & PrePayInfo, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Load(lIndex, PrePayInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::DeletePrepayInfo(long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.DeletePrepayInfo(lIndex) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstPrepayInfo(PS::PUMPSRV_PRE_PAY_REC & PrePayInfo)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadFirstPrePayInfo(PrePayInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadNext(long lIndex ,PS::PUMPSRV_PRE_PAY_REC & PrePayInfo)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadNextPrePayInfo(lIndex,PrePayInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SaveTankReading(long lIndex, const PS::TankRead & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveTankRead(lIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadTankReading(long lIndex, PS::TankRead & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadTankRead(lIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::DeleteTankReading(long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.DeleteTankReading(lIndex) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstTankReading(PS::TankRead & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadFirstTankRead(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadNextTankReading(long lIndex, PS::TankRead & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadNextTankRead(lIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SaveExtraInfo(PS::PUMPSRV_EXTRA_INFO_REC & ExtraInfo, const  CString & ExtInfoXml, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Save(lIndex, ExtraInfo, ExtInfoXml) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadExtraInfo(PS::PUMPSRV_EXTRA_INFO_REC & ExtraInfo, CString & ExtInfoXml, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Load(lIndex, ExtraInfo, ExtInfoXml) == DB_SUCCESS);
}

bool CPumpSrvDatabase::DeleteExtraInfo(long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.DeleteExtraInfo(lIndex) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstExtraInfo(PS::PUMPSRV_EXTRA_INFO_REC & ExtraInfo, CString & ExtInfoXml)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Load(ExtraInfo, ExtInfoXml) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadNext(long lIndex, PS::PUMPSRV_EXTRA_INFO_REC & ExtraInfo, CString & ExtInfoXml)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadNext(lIndex, ExtraInfo, ExtInfoXml) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadAlarmSrv(long AlarmSrvIndex, PS::ALARM_INFO & AlarmInfo)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Load(AlarmSrvIndex, AlarmInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstAlarmSrv(PS::ALARM_INFO & AlarmInfo)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadFirst(AlarmInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadNextAlarmSrv(long CurrentAlarmSrvIndex, PS::ALARM_INFO & AlarmInfo)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadNext(CurrentAlarmSrvIndex, AlarmInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SaveAlarmSrv(const PS::ALARM_INFO & AlarmInfo)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Save(AlarmInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::Delete(const PS::ALARM_INFO & AlarmInfo)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Delete(AlarmInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SaveOLATrs(long lIndex,const PS::PAY_AT_PUMP_INFO & OlaTrs, const PS::PumpTransactBase & PumpTransactBase)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveOla(lIndex,OlaTrs, PumpTransactBase) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadOLATrs(long lIndex,PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadOla(lIndex , OlaTrs, PumpTransactBase) == DB_SUCCESS);
}

bool CPumpSrvDatabase::DeleteOLATrs(long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.DeleteOLATrs(lIndex) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstOLATrs(PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadFirstOla(OlaTrs, PumpTransactBase) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadNextOLATrs(long lIndex, PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadNextOla(lIndex, OlaTrs, PumpTransactBase) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SaveOLATempTrs(long lIndex,const PS::PAY_AT_PUMP_INFO & OlaTrs, const PS::PumpTransactBase & PumpTransactBase)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveOlaTemp(lIndex,OlaTrs, PumpTransactBase) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadOLATempTrs(long lIndex , PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadOlaTemp(lIndex ,OlaTrs, PumpTransactBase) == DB_SUCCESS);
}

bool CPumpSrvDatabase::DeleteOLATempTrs(long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.DeleteOLATempTrs(lIndex) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstOLATempTrs(PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadFirstOlaTemp(OlaTrs, PumpTransactBase) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadNextOLATempTrs(long lIndex, PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadNextOlaTemp(lIndex, OlaTrs, PumpTransactBase) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SaveCarWashInfo(const PS::CAR_WASH_INFO & Record, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveCarWashInfo(lIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstCarWashInfoRecord(PS::CAR_WASH_INFO & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Load(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadCarWashInfoRecord(PS::CAR_WASH_INFO & Record,long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Load(Record,lIndex) == DB_SUCCESS);
}


bool CPumpSrvDatabase::Delete(const PS::CAR_WASH_INFO & carWashInfo)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Delete(carWashInfo) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadNextCarWashInfoRecord(long lCurrentIndex, PS::CAR_WASH_INFO & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadNext(lCurrentIndex,Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SaveItemsInfo(long lIndex, const PS::ITEMS_INFO & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveItemsInfo(lIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadItemsInfo(long lIndex, PS::ITEMS_INFO & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadItemsInfo(lIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::DeleteItemsInfo(const PS::ITEMS_INFO & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.DeleteItemsInfo(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstItemsInfo(PS::ITEMS_INFO & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadFirstItem(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SaveTankGaugeRecord(const PS::TankGauge &Record, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveTankGauge(lIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadTankGaugeRecord(PS::TankGauge &Record, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadTankGauge(lIndex, Record) == DB_SUCCESS);
}
bool CPumpSrvDatabase::SavePaidTrs(const PS::PAID_TRS_EX & record, long lIndex, long lPumpNumber)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SavePaidTrs(record, lIndex, lPumpNumber) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstPaidTrs(PS::PAID_TRS_EX & record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadFirstPaidTrs(record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::Load(PS::PAID_TRS_EX & Record, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Load(Record, lIndex) == DB_SUCCESS);
}

bool CPumpSrvDatabase::Delete(const PS::PAID_TRS_EX & Record, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Delete(Record, lIndex) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadNext(long lCurrentIndex, PS::PAID_TRS_EX & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadNext(lCurrentIndex,Record) == DB_SUCCESS);
}

long CPumpSrvDatabase::GetPaidTrsCount(long lPumpNUmber)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	long lCount = 0;
	if (FuelDriver.GetPaidTrsCount(lPumpNUmber, lCount) == DB_SUCCESS)
		return lCount;
	else
		return 0;
}

bool CPumpSrvDatabase::SaveAsh200Record(const PS::ASH2000_REC & Record, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Save(lIndex,Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadFirstAsh2000Record(PS::ASH2000_REC &Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Load(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadAsh2000Record(PS::ASH2000_REC & Record, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Load(lIndex,Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::Delete(const PS::ASH2000_REC & Record)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Delete(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SaveSingleShiftRecord(const PS::SINGLE_SHIFT_ACCUMULATION_REC &Record, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Save(lIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadSingleShiftRecord(PS::SINGLE_SHIFT_ACCUMULATION_REC &Record, long lIndex)
{
	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.Load(lIndex, Record) == DB_SUCCESS);
}



bool CPumpSrvDatabase::SaveReceipt(const CReceiptInfo & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveReceipt(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::DeleteReceiptRecord(CReceiptInfo & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.DeleteReceiptRecord(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::GetFirstReceiptRecord(CReceiptInfo & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.GetFirstReceiptRecord(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::GetFirstReceiptRecordWithHashedKey(CReceiptInfo & Record, const CString & HashedKey)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.GetFirstReceiptRecordWithHashedKey(Record, HashedKey) == DB_SUCCESS);
}

bool CPumpSrvDatabase::GetNextReceiptRecord(int LastTrsNumber, CReceiptInfo & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.GetNextReceiptRecord(LastTrsNumber, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::GetNextReceiptRecord(int LastTrsNumber, const CString & HashKey, CReceiptInfo & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.GetNextReceiptRecord(LastTrsNumber, HashKey, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::GetLastReceiptRecord(CReceiptInfo & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.GetLastReceiptRecord(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadReceipt(int TrsIndex, CReceiptInfo & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadReceipt(TrsIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadReceipt(const CString & HashKey, std::vector<CReceiptInfo> & RecordList)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadReceipt(HashKey, RecordList) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadReceiptByVehicle(const CString & VehicleKey, CReceiptInfo & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadReceiptByVehicle(VehicleKey, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::ReceiptIncrementNumberOfCopies(const CString & HashKey, bool ReceiptPrinted)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.ReceiptIncrementNumberOfCopies(HashKey, ReceiptPrinted) == DB_SUCCESS);
}

bool CPumpSrvDatabase::ReceiptIncrementNumberOfCopies(int TrsIndex, bool ReceiptPrinted)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.ReceiptIncrementNumberOfCopies(TrsIndex, ReceiptPrinted) == DB_SUCCESS);
}

bool CPumpSrvDatabase::InsertReceipt(CReceiptInfo & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.InsertReceipt(Record) == DB_SUCCESS);
}
//Receipt extra info

bool CPumpSrvDatabase::SaveReceiptExtraInfo(const PS::RECEIPT_EXTRA_INFO_REC & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveReceiptExtraInfo(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::DeleteReceiptExtraInfoRecord(PS::RECEIPT_EXTRA_INFO_REC & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.DeleteReceiptExtraInfoRecord(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::GetFirstReceiptExtraInfoRecord(PS::RECEIPT_EXTRA_INFO_REC & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.GetFirstReceiptExtraInfoRecord(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::GetNextReceiptExtraInfoRecord(int LastTrsNumber, PS::RECEIPT_EXTRA_INFO_REC & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.GetNextReceiptExtraInfoRecord(LastTrsNumber, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::GetLastReceiptExtraInfoRecord(PS::RECEIPT_EXTRA_INFO_REC & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.GetLastReceiptExtraInfoRecord(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadReceiptExtraInfo(int TrsIndex, PS::RECEIPT_EXTRA_INFO_REC & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.LoadReceiptExtraInfo(TrsIndex, Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::InsertReceiptExtraInfo(PS::RECEIPT_EXTRA_INFO_REC & Record)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.InsertReceiptExtraInfo(Record) == DB_SUCCESS);
}

bool CPumpSrvDatabase::SavePumpInfo(const PUMP_INFO & PumpInfo, long lPumpNumber, LPCSTR szFunction, int LineNumber)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	CByteArray TempBuffer;
	CopyToByteArray<PUMP_INFO>(TempBuffer, PumpInfo);

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	return (FuelDriver.SaveRealTimeInfoPumpInfo(lPumpNumber, TempBuffer) == DB_SUCCESS);
}

bool CPumpSrvDatabase::LoadRealTimeInfo(PAY_AT_PUMP_INFO & PayAtPumpInfoRecord, PUMP_INFO & PumpInfo, long lPumpNumber, LPCSTR szFunction, int LineNumber)
{
	TimingProfile(PROFILE_ALL, _T(""), DATABASE_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	CByteArray TempBuffer;
	TempBuffer.SetSize(sizeof(PumpInfo));

	bool bReturnValue = (FuelDriver.LoadRealTimeInfo(lPumpNumber, PayAtPumpInfoRecord, TempBuffer) == DB_SUCCESS);
	if (bReturnValue)
	{
		PumpInfo = *(PUMP_INFO *)TempBuffer.GetData();
	}

	return bReturnValue;
}

bool CPumpSrvDatabase::SetReceiptPrinted(CString HashKey, int & TrsIndex)
{
	TimingProfile(PROFILE_ALL, CString(_T("TLOG Data = ")) + ConvertValuesToXML(_T("HashKey"), HashKey), RECEIPT_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	HashKey.Trim();
	TrsIndex = 0;

	if (FuelDriver.SetReceiptPrinted(HashKey) != DB_SUCCESS)
	{
		return false;
	}

	SQLINTEGER sqlTrsIndex = 0;

	if (FuelDriver.GetLastReceiptPrinted(HashKey, sqlTrsIndex) != DB_SUCCESS)
	{
		return false;
	}

	TrsIndex = sqlTrsIndex;

	return true;
}

bool CPumpSrvDatabase::UpdateReceiptPumpNumber(int TrsIndex, long lNewPumpNumber)
{
	TimingProfile(PROFILE_ALL, CString(_T("TLOG Data = ")) + ConvertValuesToXML(_T("TrsIndex"), TrsIndex), RECEIPT_PROFILING);

	CSemaphoreLock SemaphoreLock(*m_LockingSemaphore);

	if (!m_AbstractDBManager.get())
		return false;

	auto & FuelDriver = m_AbstractDBManager->GetFuelDriver_PumpSrv();

	if (FuelDriver.UpdateReceiptPumpNumber(TrsIndex,lNewPumpNumber) != DB_SUCCESS)
	{
		return false;
	}

	return true;
}