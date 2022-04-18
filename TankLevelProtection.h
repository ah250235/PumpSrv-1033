//
//4.0.9.506 //amram
#ifndef _TANK_LEVEL_PROTECTION_
#define _TANK_LEVEL_PROTECTION_

#include "Grade.h"//4.0.9.506 //amram




typedef struct 
{
	long		lPumpNumber;
	CGrade		GradeInfo[MAX_POSITIONS_PER_PUMP];
	long		lNumOfValidGrades;
	
}MAP_GRADE_TANK_LEVEL;





class CTankLevelProtection
{
public:
	CTankLevelProtection();
	~CTankLevelProtection();
	void LoadTankMapPerPump();
	void SetStatusPerTankWithPump(BSTR sTagPumpsNuzzlesRestrict, long lFlags);
	void GetValidGradesTankLevelForOnePump(long lPumpNumber,MAP_GRADE_TANK_LEVEL & cRec);
	
	//set and get param from registry
	inline void SetProtectTrsLimit(long lProtLimit) {m_lProtectTrsLimit = lProtLimit;};
	inline long GetProtectTrsLimit() {return m_lProtectTrsLimit;};
	inline void SetTankMinVolumeLimit(long lTankVolume) {m_lTankMinVolumeLimit = lTankVolume;};
	inline long GetTankMinVolumeLimit() {return m_lTankMinVolumeLimit;};


private:
	CRITICAL_SECTION   	 m_csTankLevelMap;
	MAP_GRADE_TANK_LEVEL m_cTankLevelMap[MAX_PUMPS_64];
	long	m_lNumOfPumps;//hold the number of pumps in the system
	long	m_lProtectTrsLimit;//: When set to 1, 
							   //every request for pump authorization will be checked, and trs will be authorized only if product amount in tank is above the requested limit.  
	long	m_lTankMinVolumeLimit;//Meaning: The opposite parameter to the existing TankMaxVolumeLimit. Mention what is the minimum volume limit per tank.

};




#endif //_TANK_LEVEL_PROTECTION_
