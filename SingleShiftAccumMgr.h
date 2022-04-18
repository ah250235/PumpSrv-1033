// SingleShiftAccumMgr.h: interface for the CSingleShiftAccumMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLESHIFTACCUMMGR_H__89D7D06D_6F70_4847_8A9B_B8AFCCC63162__INCLUDED_)
#define AFX_SINGLESHIFTACCUMMGR_H__89D7D06D_6F70_4847_8A9B_B8AFCCC63162__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define SINGLE_SHIFT_ACCUM_RECORD_SIZE						256
#define SINGLE_SHIFT_ACCUM_FIRST_TANK_RECORD				512
#define SINGLE_SHIFT_ACCUM_FIRST_PUMP_POSITION_RECORD		SINGLE_SHIFT_ACCUM_FIRST_TANK_RECORD + SINGLE_SHIFT_ACCUM_RECORD_SIZE*64

class CSingleShiftAccumMgr  
{
public:
	CSingleShiftAccumMgr();
	virtual ~CSingleShiftAccumMgr();

	void ResetAllRecordsInDB();

	void LoadDataFromDB();

	long GetTankRecord(const long lTankNum, SINGLE_SHIFT_ACCUM_TANK_RECORD &cTankRecord);
	long GetPumpPositionRecord(const long lPump,const short nPosition, SINGLE_SHIFT_ACCUM_PUMP_POSITION_RECORD &cPumpPositionRecord);

	long AccumulateDeliveryVolume(const long lTankNum, const long lDeliveryVolume);
	long AccumulateSales(const long lPump,const short nPosition, const long lSaleAmount);
	long AccumulatePumpTest(const long lPump,const short nPosition, const long lPumpTestSaleAmount);

private:
	void LoadTanksFromDB();
	void LoadPumpsFromDB();
	
	void ResetPumpPositionRecordInDB(const long lPump,const short nPosition);
	void ResetTankRecordInDB(const long lTankNumber);

	long GetDbIndexByTank(const long lTankNumber);
	long GetDbIndexByPumpPosition(const long lPump,const short nPosition);

	long SaveTankRecordInDB(const long lTankNum);
	long LoadTankRecordFromDB(const long lTankNum);
	
	long SavePumpPositionRecordInDB(const long lPump, const short nPosition);
	long LoadPumpPositionRecordFromDB(const long lPump, const short nPosition);	

	
	SINGLE_SHIFT_ACCUM_TANK_RECORD m_cSingleShiftAccumTankArray[MAX_TANKS];
	SINGLE_SHIFT_ACCUM_PUMP_POSITION_RECORD m_cSingleShiftAccumPumpPositionArray[MAX_PUMPS_64][MAX_POSITIONS_PER_PUMP];


};


#endif // !defined(AFX_SINGLESHIFTACCUMMGR_H__89D7D06D_6F70_4847_8A9B_B8AFCCC63162__INCLUDED_)
