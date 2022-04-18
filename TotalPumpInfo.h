// TotalPumpInfo.h: interface for the CTotalPumpInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOTALPUMPINFO_H__014799EA_B950_45B6_994A_84BCF5F0D159__INCLUDED_)
#define AFX_TOTALPUMPINFO_H__014799EA_B950_45B6_994A_84BCF5F0D159__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_NOZZLE_PER_PUMP 8

typedef struct 
{
	char			sVolume[MAX_NAME_LENGTH];
	char			sValueA[MAX_NAME_LENGTH];
	char			sValueB[MAX_NAME_LENGTH];	
}TOTAL_DATA_FUEL;

typedef struct 
{
	TOTAL_DATA_FUEL cTotalInfo;
	long		 lNozzle;
	long  		 lGrade;
	long		 lIsNozzleValid;
}TOTAL_NOZZLE_INFO;

typedef struct 
{
	TOTAL_NOZZLE_INFO cTotalNozzleInfo[MAX_NOZZLE_PER_PUMP];
	long		 lPumpNumber;
	long		 lPumpStatus;
	
}TOTAL_PUMP_INFO;



class CTotalPumpInfo  
{
public:
	void SetPumpStatus(long lPumpNumber, long lStatus); // 4.0.15.120 - CR 44000
	CTotalPumpInfo();
	void SetTotalsWitnNewData(long lPumpNumber,long lGrade,long lNozzle,char *sVolume,char * sValueA,char * sValueB );
	BOOL GetTotalData(long lPumpNumber,long lNozzle,TAG_MEMO_TOTAL_PUMPS_INFO & tagTotalInfo);
	virtual ~CTotalPumpInfo();
private:
	CRITICAL_SECTION   	 m_csTotalPumpInfo;
	TOTAL_PUMP_INFO m_cTotalPumpInfo[MAX_PUMPS_64];

};

#endif // !defined(AFX_TOTALPUMPINFO_H__014799EA_B950_45B6_994A_84BCF5F0D159__INCLUDED_)
