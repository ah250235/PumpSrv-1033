#pragma once

#include "FuelDB.h"
#include "AbstractDBManager.h"
#include "PumpSrvDeclarations.h"
#include "ReceiptInfo.h"

class CLockingSemaphore;
class CGUID;

#ifndef REC_ALL
#define REC_NOTHING							0x00
#define REC_STATUS							0x01
#define REC_MISC							0x02
#define REC_TRS								0x04
#define REC_PAP_INFO						0x08
#define REC_GENERAL							0x10
#define REC_ALL								0xff
#endif // REC_ALL

struct _PUMP_INFO;
typedef struct _PUMP_INFO	PUMP_INFO;

class CPumpSrvDatabaseCallback
{
public:
	virtual void PostError(const CFuelErrorInfo & FuelErrorInfo)	= 0;
};


class CPumpSrvDatabase : public CAbstractDBManagerCallback
{
public:
	CPumpSrvDatabase();
	virtual ~CPumpSrvDatabase();

public:
	virtual bool	Initialize(void);
	virtual bool	Close(void);
	virtual void	GetFuelMsSqlVersion(CString & version);

public:
	virtual bool	ValidateDatabase(void);

public:
	virtual bool Delete(const PS::ALARM_INFO & AlarmInfo);

public:
	virtual bool	LoadAlarmSrv(long AlarmSrvIndex, PS::ALARM_INFO & AlarmInfo);
	virtual bool	LoadFirstAlarmSrv(PS::ALARM_INFO & AlarmInfo);
	virtual bool	LoadNextAlarmSrv(long CurrentAlarmSrvIndex, PS::ALARM_INFO & AlarmInfo);
	virtual bool	SaveAlarmSrv(const PS::ALARM_INFO & AlarmInfo);

	
	virtual bool	LoadPayAtPumpInfo(PS::PAY_AT_PUMP_INFO & cTmpPumpInfo, long lPumpNumber);
	virtual bool	SavePayAtPumpInfo(const PS::PAY_AT_PUMP_INFO & cTmpPumpInfo, long lPumpNumber, LPCSTR szFunction, int LineNumber);

	virtual bool	LoadPumpInfo(PUMP_INFO & Record, long lPumpNumber);
	virtual bool	SavePumpInfo(const PUMP_INFO & Record, long lPumpNumber, LPCSTR szFunction, int LineNumber);

	// totals file
	virtual bool    SavePumpTotals(long lIndex, const PS::PumpTotals &PumpTotals);
	virtual bool    LoadPumpTotals(long lIndex, PS::PumpTotals &PumpTotals);
	virtual bool    LoadFirstPumpTotals(PS::PumpTotals &PumpTotals);
	virtual bool    LoadNextPumpTotals(long lIndex, PS::PumpTotals &PumpTotals);
	virtual bool    DeletePumpTotals(long lIndex);

	// delivery file
	virtual bool    DeleteFuelDelivery(long lIndex);
	virtual bool    LoadFuelDelivery(long lIndex, DELIVERY_INFO &DeliveryInfo);
	virtual bool    LoadFirstFuelDelivery(DELIVERY_INFO &DeliveryInfo);
	virtual bool    LoadNextFuelDelivery(long lIndex, DELIVERY_INFO &DeliveryInfo);
	virtual bool    SaveFuelDelivery(long lIndex, const DELIVERY_INFO &DeliveryInfo);
	
	// shift
	virtual bool	LoadShift(PS::SHIFT_REC & Record);
	virtual bool	SaveShift(const PS::SHIFT_REC & Record);

	// server info
	virtual bool	LoadServerInfo(PS::SERVER_INFO & Record);
	virtual bool	SaveServerInfo(const PS::SERVER_INFO & Record);

	// prepay file
	virtual bool	SavePrePayInfo(PS::PUMPSRV_PRE_PAY_REC & PrePayInfo, long lIndex);
	virtual bool	LoadPrePayInfo(PS::PUMPSRV_PRE_PAY_REC & PrePayInfo, long lIndex);
	virtual bool	LoadFirstPrepayInfo(PS::PUMPSRV_PRE_PAY_REC & PrePayInfo);
	virtual bool    LoadNext(long lIndex , PS::PUMPSRV_PRE_PAY_REC & PrePayInfo);
	virtual bool	DeletePrepayInfo(long lIndex);

public:
	virtual bool	DeleteTankReading(long lIndex);
	virtual bool	LoadTankReading(long lIndex, PS::TankRead & Record);
	virtual	bool	LoadFirstTankReading(PS::TankRead & Record);
	virtual	bool	LoadNextTankReading(long lIndex, PS::TankRead & Record);
	virtual bool	SaveTankReading(long lIndex, const PS::TankRead & Record);

public:
	virtual bool	SaveExtraInfo(PS::PUMPSRV_EXTRA_INFO_REC & ExtraInfo, const CString & ExtInfoXml, long lIndex);
	virtual bool	LoadExtraInfo(PS::PUMPSRV_EXTRA_INFO_REC & ExtraInfo, CString & ExtInfoXml, long lIndex);
	virtual bool	LoadFirstExtraInfo(PS::PUMPSRV_EXTRA_INFO_REC & ExtraInfo, CString & ExtInfoXml);
	virtual bool	DeleteExtraInfo(long lIndex);
	virtual bool	LoadNext(long lIndex, PS::PUMPSRV_EXTRA_INFO_REC & ExtraInfo, CString & ExtInfoXml);

public:
	virtual bool DeleteReceiptRecord(CReceiptInfo & Record);
	virtual bool GetFirstReceiptRecord(CReceiptInfo & Record);
	virtual bool GetFirstReceiptRecordWithHashedKey(CReceiptInfo & Record, const CString & HashedKey);
	virtual bool GetNextReceiptRecord(int LastTrsNumber, CReceiptInfo & Record);
	virtual bool GetNextReceiptRecord(int LastTrsNumber, const CString & HashKey, CReceiptInfo & Record);
	virtual bool GetLastReceiptRecord(CReceiptInfo	& Record);
	virtual bool LoadReceipt(int TrsIndex, CReceiptInfo & Record);
	virtual bool LoadReceipt(const CString & HashKey, std::vector<CReceiptInfo> & RecordList);
	virtual bool LoadReceiptByVehicle(const CString & VehicleKey, CReceiptInfo & Record);
	virtual bool ReceiptIncrementNumberOfCopies(const CString & HashKey, bool ReceiptPrinted);
	virtual bool ReceiptIncrementNumberOfCopies(int TrsIndex, bool ReceiptPrinted);
	virtual bool SaveReceipt(const CReceiptInfo & Record);
	virtual bool InsertReceipt(CReceiptInfo & Record);
	virtual bool SetReceiptPrinted(CString HashKey, int & TrsIndex);
	virtual bool UpdateReceiptPumpNumber(int TrsIndex, long lNewPumpNumber);

	//Receipt extra info
	virtual bool DeleteReceiptExtraInfoRecord(PS::RECEIPT_EXTRA_INFO_REC & Record);
	virtual bool GetFirstReceiptExtraInfoRecord(PS::RECEIPT_EXTRA_INFO_REC & Record);
	virtual bool GetNextReceiptExtraInfoRecord(int LastTrsNumber, PS::RECEIPT_EXTRA_INFO_REC & Record);
	virtual bool GetLastReceiptExtraInfoRecord(PS::RECEIPT_EXTRA_INFO_REC	& Record);
	virtual bool LoadReceiptExtraInfo(int TrsIndex, PS::RECEIPT_EXTRA_INFO_REC & Record);
	virtual bool SaveReceiptExtraInfo(const PS::RECEIPT_EXTRA_INFO_REC & Record);
	virtual bool InsertReceiptExtraInfo(PS::RECEIPT_EXTRA_INFO_REC & Record);

	// ola file
	virtual bool	SaveOLATrs(long lIndex,const PS::PAY_AT_PUMP_INFO & OlaTrs, const PS::PumpTransactBase & PumpTransactBase);
	virtual bool	LoadOLATrs(long lIndex,PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase);
	virtual bool	LoadFirstOLATrs(PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase);
	virtual bool	DeleteOLATrs(long lIndex);
	virtual bool	LoadNextOLATrs(long lIndex, PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase);

	// ola temp file
	virtual bool	SaveOLATempTrs(long lIndex ,const PS::PAY_AT_PUMP_INFO & OlaTrs, const PS::PumpTransactBase & PumpTransactBase);
	virtual bool	LoadOLATempTrs(long lIndex,PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase);
	virtual bool	LoadFirstOLATempTrs(PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase);
	virtual bool	DeleteOLATempTrs(long lIndex);
	virtual bool	LoadNextOLATempTrs(long lIndex, PS::PAY_AT_PUMP_INFO & OlaTrs, PS::PumpTransactBase & PumpTransactBase);

	// carwash file
	virtual bool	SaveCarWashInfo(const PS::CAR_WASH_INFO & Record, long lIndex);
	virtual bool    LoadFirstCarWashInfoRecord(PS::CAR_WASH_INFO &);
	virtual bool	LoadCarWashInfoRecord(PS::CAR_WASH_INFO & Record, long lIndex);
	virtual bool	Delete(const PS::CAR_WASH_INFO & CarWashInfo);
	virtual bool	LoadNextCarWashInfoRecord(long lCurrentIndex, PS::CAR_WASH_INFO & Record);
	//Items file
	virtual bool	SaveItemsInfo(long lIndex,const PS::ITEMS_INFO & Record);
	virtual bool    LoadItemsInfo(long lIndex,PS::ITEMS_INFO & Record);
	virtual bool    LoadFirstItemsInfo(PS::ITEMS_INFO &Record);
	virtual bool	DeleteItemsInfo(const PS::ITEMS_INFO & Record);
	// paid file
	virtual bool	SavePaidTrs(const PS::PAID_TRS_EX & record, long lIndex, long lPumpNumber);
	virtual bool    LoadFirstPaidTrs(PS::PAID_TRS_EX &);
	virtual bool	Load(PS::PAID_TRS_EX & Record, long lIndex);
	virtual bool	Delete(const PS::PAID_TRS_EX & Record, long lIndex);
	virtual bool	LoadNext(long lCurrentIndex, PS::PAID_TRS_EX & Record);
	virtual long    GetPaidTrsCount(long lPumpNUmber);

	//TG file
	virtual bool	LoadTankGaugeRecord(PS::TankGauge &Record, long lIndex);
	virtual bool	SaveTankGaugeRecord(const PS::TankGauge &Record, long lIndex);

	//ASH2000 
	virtual bool	SaveAsh200Record(const PS::ASH2000_REC& Record, long lIndex);
	virtual bool    LoadFirstAsh2000Record(PS::ASH2000_REC &);
	virtual bool	LoadAsh2000Record(PS::ASH2000_REC & Record, long lIndex);
	virtual bool	Delete(const PS::ASH2000_REC & Record);

	//TG file
	virtual bool	LoadSingleShiftRecord(PS::SINGLE_SHIFT_ACCUMULATION_REC &Record, long lIndex);
	virtual bool	SaveSingleShiftRecord(const PS::SINGLE_SHIFT_ACCUMULATION_REC &Record, long lIndex);

public:
	virtual bool	GetRealTimeInfoCount(size_t & Count, LPCSTR szFunction, int LineNumber);
	virtual bool	LoadRealTimeInfo(PAY_AT_PUMP_INFO & PayAtPumpInfoRecord, PUMP_INFO & PumpInfo, long lPumpNumber, LPCSTR szFunction, int LineNumber);
	virtual bool	SaveRealTimeInfo(const PAY_AT_PUMP_INFO & PayAtPumpInfoRecord, const PUMP_INFO & PumpInfo, long lPumpNumber, LPCSTR szFunction, int LineNumber);

	
public:
	static CPumpSrvDatabase	&	GetInstance(void);

public:
	virtual DbManagerRetCode	ConnectNewDBManagerInstance(const CString & DBManagerName, const CString & DBInstanceName, const CString & DBParameters, const CString & DBLibraryPath);
	virtual DbManagerRetCode	ConnectNewDBManagerInstance_DefaultSQL(void);

public:
	virtual void PostError(const CFuelErrorInfo & FuelErrorInfo);

public:
	virtual void	AddCallBack(CPumpSrvDatabaseCallback *);
	virtual void	RemoveCallBack(CPumpSrvDatabaseCallback *);

public:
	CFuelDB	& GetFuelDB(void)	{ return m_FuelDB; }
	inline bool	IsDatabaseOpen(void)		const { bool ans = m_AbstractDBManager->IsDatabaseOpen();		return ans; };
	inline bool IsDatabaseConnected(void)	const { bool ans = m_AbstractDBManager->IsDatabaseConnected();	return ans; }


protected:
	CFuelDB									m_FuelDB;
	CString									m_DBManagerName;
	CString									m_DBInstanceName;
	CString									m_DBParameters;
	CString									m_DBLibraryPath;
	std::unique_ptr<CLockingSemaphore>		m_LockingSemaphore;
	std::vector<CPumpSrvDatabaseCallback *>	m_Callbacks;

protected:
	std::unique_ptr<CAbstractDBManager>		m_AbstractDBManager;
};

inline CPumpSrvDatabase & CPumpSrvDatabase::GetInstance(void)
{
	static CPumpSrvDatabase m_Database;

	return m_Database;
}


