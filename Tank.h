// Tank.h: interface for the CTank class.
// Written by  :	Yaron Azachi	15/07/01
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TANK_H__F90EC310_5FA8_4CDF_8F17_A8811370B8DE__INCLUDED_)
#define AFX_TANK_H__F90EC310_5FA8_4CDF_8F17_A8811370B8DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MILLISECONDS_IN_A_SECOND 1000
#define FUEL_DELIVERY_WEIGHT_FACTOR			1000	//4.0.28.500 - TD 250092
#define FUEL_DELIVERY_DENSITY_FACTOR 		1000	//4.0.28.500 - TD 250092

struct MEMO_DELIVERY_INFO
{
	CTime		m_cDeliveryTime;
	long		m_lDeliveryVolume;	
	long		m_lStatus;
};

enum eTankState							//	4.0.25.1130 
{
	TANK_STATE_READY = 1,
	TANK_STATE_BLOCKED_BY_USER = 2
};

class CDeliveryEmulationInfo	//4.0.20.52 (62818)
{
public:
	//Class Ctor
	CDeliveryEmulationInfo();

	void LoadDeliveryEmulationInfo(long lTankNumber);
	//ResetDeliveryEmulationInfo - only info which needs to be saved in the QDX, and therefore: can be reseted together, after writing to QDX:
	void ResetDeliveryEmulationInfo(){ m_lStartDeliveryEmulationVolume = m_lEndDeliveryEmulationVolume = m_lAccumulatedDeliveryEmulationSales = m_lStartDeliveryEmulationTankTemperature= m_lEndDeliveryEmulationTankTemperature =
		m_lStartDeliveryEmulationDensity  = m_lStartDeliveryEmulationWeight = m_lEndDeliveryEmulationDensity = m_lEndDeliveryEmulationWeight = 0; }  //4.0.25.380 - TD 287958
	void SaveDeliveryEmulationInfo(long lTankNumber);

	void SetStartDeliveryEmulationVolume(long lFuelVol){ m_lStartDeliveryEmulationVolume = lFuelVol; }
	long GetStartDeliveryEmulationVolume(){ return m_lStartDeliveryEmulationVolume; }
	void SaveStartDeliveryEmulationVolume(long lTankNumber);

	void SetEndDeliveryEmulationVolume(long lFuelVol){ m_lEndDeliveryEmulationVolume = lFuelVol; }
	long GetEndDeliveryEmulationVolume(){ return m_lEndDeliveryEmulationVolume; }
	void SaveEndDeliveryEmulationVolume(long lTankNumber);

	void SetStartDeliveryEmulationTankTemperature(long lStartTankTemperature){ m_lStartDeliveryEmulationTankTemperature = lStartTankTemperature; }
	long GetStartDeliveryEmulationTankTemperature(){ return m_lStartDeliveryEmulationTankTemperature; }
	void SaveStartDeliveryEmulationTankTemperature(long lTankNumber);

	void SetEndDeliveryEmulationTankTemperature(long lEndTankTemperature){ m_lEndDeliveryEmulationTankTemperature = lEndTankTemperature; }
	long GetEndDeliveryEmulationTankTemperature(){ return m_lEndDeliveryEmulationTankTemperature; }
	void SaveEndDeliveryEmulationTankTemperature(long lTankNumber);
	
	void SetStartDeliveryEmulationWaterVolume(long lStartWaterVolume){ m_lStartDeliveryEmulationWaterVolume = lStartWaterVolume; }//4.0.21.500 TD 66935
	long GetStartDeliveryEmulationWaterVolume(){ return m_lStartDeliveryEmulationWaterVolume; }//4.0.21.500 TD 66935
	void SaveStartDeliveryEmulationWaterVolume(long lTankNumber);//4.0.21.500 - TD 66935
    
	void SetEndDeliveryEmulationWaterVolume(long lEndWaterVolume){ m_lEndDeliveryEmulationWaterVolume = lEndWaterVolume; }//4.0.21.500 TD 66935
	long GetEndDeliveryEmulationWaterVolume(){ return m_lEndDeliveryEmulationWaterVolume; }//4.0.21.500 TD 66935
	void SaveEndDeliveryEmulationWaterVolume(long lTankNumber);  //4.0.21.500 - TD 66935
    
	void ResetDeliveryEmulationWaterVolume(long lTankNumber);    //4.0.21.500 - TD 66935
	void ResetDeliveryEmulationWaterVolumeMember(){ m_lStartDeliveryEmulationWaterVolume = m_lEndDeliveryEmulationWaterVolume = 0; } //4.0.21.500 TD 66935
	
	void IncAccumulatedDeliveryEmulationSales(long lDeliverySaleValue){ m_lAccumulatedDeliveryEmulationSales += lDeliverySaleValue; }
	void ReSetAccumulatedDeliveryEmulationSales(){ m_lAccumulatedDeliveryEmulationSales = 0; }
	void SaveAccumulatedDeliveryEmulationSales(long lTankNumber);
	
	void SetFoundDelivery(BOOL bFoundDelivery){ m_bFoundDelivery = bFoundDelivery; } //4.0.21.500 TD 66935
	BOOL GetFoundDelivery(){ return m_bFoundDelivery; } //4.0.21.500 TD 66935
	void ResetFoundDeliveryMember(){ m_bFoundDelivery = FALSE; } //4.0.21.500 TD 66935
	
	void SetVPNotification(BOOL bVPNotification){ m_bVPNotification = bVPNotification; } //4.0.21.500 TD 66935
	BOOL GetVPNotification(){ return m_bVPNotification; } //4.0.21.500 TD 66935

	void SetDeliveryReference(long lDeliveryReference){ m_lDeliveryReference = lDeliveryReference; } //4.0.20.501 (merge from 4.0.19.300) - TD 65174
	long GetDeliveryReference(){ return m_lDeliveryReference; } //4.0.20.501 (merge from 4.0.19.300) - TD 65174
	void ResetDeliveryReferenceMember(){ m_lDeliveryReference = 0; } //4.0.20.501 (merge from 4.0.19.300) - TD 65174
 
	void SetLockConnectedPumps(BOOL bLockConnectedPumps){ m_bLockConnectedPumps = bLockConnectedPumps;} //4.0.21.500 TD 66935
	BOOL GetLockConnectedPumps(){ return m_bLockConnectedPumps; } //4.0.21.500 TD 66935
	void ResetLockConnectedPumpsMember(){ m_bLockConnectedPumps = FALSE; } //4.0.21.500 TD 66935

	void		SetStartDeliveryEmulationTime(SYSTEMTIME* cTime);
	SYSTEMTIME	GetStartDeliveryEmulationTime(){ return m_cStartDeliveryEmulationTime; }	//4.0.21.500 - TD 69374
	void		SaveStartDeliveryEmulationTime(long lTankNumber);		//4.0.21.500 - TD 69374

	void		SetEndDeliveryEmulationTime(SYSTEMTIME* cTime);
	SYSTEMTIME	GetEndDeliveryEmulationTime(){ return m_cEndDeliveryEmulationTime; }	//4.0.21.500 - TD 69374
	void		SaveEndDeliveryEmulationTime(long lTankNumber);		//4.0.21.500 - TD 69374

	void SaveFoundDelivery(long lTankNumber);                    //4.0.21.500 - TD 66935
	void ResetFoundDelivery(long lTankNumber);                   //4.0.21.500 - TD 66935

	void SaveDeliveryReference(long lTankNumber);                //4.0.20.501 (merge from 4.0.19.300) - TD 65174
	void ResetDeliveryReference(long lTankNumber);               //4.0.20.501 (merge from 4.0.19.300) - TD 65174
	
	void SaveLockConnectedPumps(long lTankNumber);               //4.0.21.500 - TD 66935
	void ResetLockConnectedPumps(long lTankNumber);              //4.0.21.500 - TD 66935

	void ResetDeliveryEmulationSingleTank(long lTankNumber);     //4.0.21.500 - TD 66935

	long CalculateDeliveryEmulation();

	void SetWasUnlockCommandSet(BOOL bWasUnlockCommandSet){ m_bWasUnlockCommandSet = bWasUnlockCommandSet; } //4.0.21.500 TD 66935
	BOOL GetWasUnlockCommandSet(){ return m_bWasUnlockCommandSet; } //4.0.21.500 TD 66935

	void InitWaitForLockTime(){ GetLocalTime(&m_tWaitForLockTime); }			   	 //4.0.21.500 TD 66935 
	void ResetWaitForLockTime(){ memset(&m_tWaitForLockTime, 0, sizeof(SYSTEMTIME));}//4.0.21.500 TD 66935
	BOOL IsTimeOutBeforeReading();												     //4.0.21.500 TD G

	void InitWaitForReadTime(){ GetLocalTime(&m_tWaitForReadTime); }				 //4.0.21.500 TD 66935
	void ResetWaitForReadTime(){ memset(&m_tWaitForReadTime, 0, sizeof(SYSTEMTIME));}//4.0.21.500 TD 66935
	BOOL IsTimeOutForReading(const long lTankNumber);                                //4.0.21.500 TD 66935

	void InitEndLockTime(){ GetLocalTime(&m_tEndLockTime); }					     //4.0.21.500 TD G
	void ResetEndLockTime(){ memset(&m_tEndLockTime, 0, sizeof(SYSTEMTIME));}	     //4.0.21.500 TD G					
	BOOL IsTimeOutForLocking(const long lTankNumber);                                //4.0.21.500 TD 66935
	
	void TurnOnIncDeliveryEmulationSales() {m_bCanIncDeliveryEmulationSales = TRUE;}	//4.0.19.1260 - TD 242013
	void TurnOffIncDeliveryEmulationSales() {m_bCanIncDeliveryEmulationSales = FALSE;}	//4.0.19.1260 - TD 242013
	BOOL CanIncDeliveryEmulationSales() {return m_bCanIncDeliveryEmulationSales;}		//4.0.19.1260 - TD 242013
	void UpdateCanIncDeliveryEmulationSales(const long lTankNumber);					//4.0.19.1260 - TD 242013
	
	void SetStartDeliveryEmulationDensity(long lStartDensity) {m_lStartDeliveryEmulationDensity = lStartDensity;}	//4.0.28.500 - TD 250092
	long GetStartDeliveryEmulationDensity(){ return m_lStartDeliveryEmulationDensity; }								//4.0.28.500 - TD 250092
	void SaveStartDeliveryEmulationDensity(long lTankNumber);                                                       //4.0.25.380 - TD 287958    

	void SetEndDeliveryEmulationDensity(long lEndDensity) {m_lEndDeliveryEmulationDensity = lEndDensity;}			//4.0.28.500 - TD 250092
	long GetEndDeliveryEmulationDensity(){ return m_lEndDeliveryEmulationDensity; }									//4.0.28.500 - TD 250092
	void SaveEndDeliveryEmulationDensity(long lTankNumber);                                                         //4.0.25.380 - TD 287958    

	void SetStartDeliveryEmulationWeight(long lStartWeight) {m_lStartDeliveryEmulationWeight = lStartWeight;}		//4.0.28.500 - TD 250092
	long GetStartDeliveryEmulationWeight(){ return m_lStartDeliveryEmulationWeight; }								//4.0.28.500 - TD 250092
	void SaveStartDeliveryEmulationWeight(long lTankNumber);                                                         //4.0.25.380 - TD 287958    

	void SetEndDeliveryEmulationWeight(long lEndWeight) {m_lEndDeliveryEmulationWeight = lEndWeight;}				//4.0.28.500 - TD 250092
	long GetEndDeliveryEmulationWeight(){ return m_lEndDeliveryEmulationWeight; }									//4.0.28.500 - TD 250092
	void SaveEndDeliveryEmulationWeight(long lTankNumber);                                                           //4.0.25.380 - TD 287958    

	// 4.0.25.1060  TD 377336
	void SetStartDeliveryEmulationProductLevel(long lStartProductLevel) {m_lStartDeliveryEmulationProductLevel = lStartProductLevel;}				
	long GetStartDeliveryEmulationProductLevel(){ return m_lStartDeliveryEmulationProductLevel; }									
	void SaveStartDeliveryEmulationProductLevel(long lTankNumber);                                               
	
	void SetEndDeliveryEmulationProductLevel(long lEndProductLevel) {m_lEndDeliveryEmulationProductLevel = lEndProductLevel;}				
	long GetEndDeliveryEmulationProductLevel(){ return m_lEndDeliveryEmulationProductLevel; }									
	void SaveEndDeliveryEmulationProductLevel(long lTankNumber);                                               
	
	void SetStartDeliveryEmulationWaterLevel(long lStartWaterLevel) {m_lStartDeliveryEmulationWaterLevel = lStartWaterLevel;}				
	long GetStartDeliveryEmulationWaterLevel(){ return m_lStartDeliveryEmulationWaterLevel; }									
	void SaveStartDeliveryEmulationWaterLevel(long lTankNumber);                                               
	
	void SetEndDeliveryEmulationWaterLevel(long lEndWaterLevel) {m_lEndDeliveryEmulationWaterLevel = lEndWaterLevel;}				
	long GetEndDeliveryEmulationWaterLevel(){ return m_lEndDeliveryEmulationWaterLevel; }									
	void SaveEndDeliveryEmulationWaterLevel(long lTankNumber);                                               

protected:
	long		m_lStartDeliveryEmulationVolume;
	long		m_lEndDeliveryEmulationVolume;
	long		m_lAccumulatedDeliveryEmulationSales;
	long		m_lStartDeliveryEmulationTankTemperature; 
	long		m_lEndDeliveryEmulationTankTemperature;
	long		m_lStartDeliveryEmulationWaterVolume;//4.0.20.501 (merge from 4.0.19.300) - TD 65174 
	long		m_lEndDeliveryEmulationWaterVolume;  //4.0.20.501 (merge from 4.0.19.300) - TD 65174
	long		m_lDeliveryReference;                //4.0.20.501 (merge from 4.0.19.300) - TD 65174
   	BOOL		m_bFoundDelivery;                    //4.0.21.500 - TD 66935
	BOOL		m_bLockConnectedPumps;               //4.0.21.500 - TD 66935
	BOOL		m_bVPNotification;                   //4.0.21.500 - TD 66935 - Should be saved also in ini (m_bVPNotification)*?*?
	BOOL		m_bCanIncDeliveryEmulationSales;	 //4.0.19.1260 - TD 242013
	

	SYSTEMTIME	m_cStartDeliveryEmulationTime;		 //4.0.21.500 - TD 69374
	SYSTEMTIME	m_cEndDeliveryEmulationTime;		 //4.0.21.500 - TD 69374
	
	SYSTEMTIME	m_tWaitForLockTime;			//4.0.21.500 TD 66935
    SYSTEMTIME	m_tWaitForReadTime;			//4.0.21.500 TD 66935
	SYSTEMTIME	m_tEndLockTime;				//4.0.21.500 TD 66935

	BOOL		m_bWasUnlockCommandSet;
	CString		m_szTankINIPath;

	long		m_lStartDeliveryEmulationDensity;		//4.0.28.500 - TD 250092
	long		m_lEndDeliveryEmulationDensity;			//4.0.28.500 - TD 250092
	long		m_lStartDeliveryEmulationWeight;		//4.0.28.500 - TD 250092
	long		m_lEndDeliveryEmulationWeight;			//4.0.28.500 - TD 250092
	long		m_lStartDeliveryEmulationProductLevel;		//4.0.25.1060  TD 377336
	long		m_lEndDeliveryEmulationProductLevel;		//4.0.25.1060  TD 377336
	long		m_lStartDeliveryEmulationWaterLevel;		//4.0.25.1060  TD 377336
	long		m_lEndDeliveryEmulationWaterLevel;			//4.0.25.1060  TD 377336
};


class CTank  
{
friend class CShiftTanks;
friend class CTankXML;//4.0.21.500 - TD 66935
public:
	void SetTankStatus(long lStatus); //4.0.15.120 - CR 44000
	void ResetDevilveryStatus();
	void SetDeliveryInfo(SYSTEMTIME & cCurrentTime , long lVolume , long lStatus);
	void SetTankValidity(bool bValid);
	void SetTankNumber(long lTankNumber);
	bool IsTankValid();
	long GetTankStatus() { return m_lStatusGeneral; }   //CR 443524
	
	void GetTankTagInfo(void *pBuff,long lVersion= 0);
	void SetTankInfo(long lProduct, long lStatusGeneral, long nFlags,
		long lFuelLevel, long lWaterLavel, long lTemp,
		long lFuelVol, long lWaterVol, long lUllage,
		long lTCVol, long lStatusSpecial, SYSTEMTIME cTime); //443035 added SYSTEMTIME cTime

	void SetPumpsLinkedMap1(const long lPumpNumber); //TD 66935
	void SetPumpsLinkedMap2(const long lPumpNumber); //TD 66935
	long GetPumpsLinkedMap1();						 //TD 66935
	long GetPumpsLinkedMap2();						 //TD 66935

	//4.0.20.52 (62818) {
	void SetStartDeliveryEmulationVolume(long lFuelVol){m_cDeliveryEmulationInfo.SetStartDeliveryEmulationVolume(lFuelVol);}
	void SetEndDeliveryEmulationVolume(long lFuelVol){m_cDeliveryEmulationInfo.SetEndDeliveryEmulationVolume(lFuelVol);}
	void IncAccumulatedDeliveryEmulationSales(long lDeliverySaleValue){m_cDeliveryEmulationInfo.IncAccumulatedDeliveryEmulationSales(lDeliverySaleValue);}
	void SetStartDeliveryEmulationTankTemperature(long lTemprature){m_cDeliveryEmulationInfo.SetStartDeliveryEmulationTankTemperature(lTemprature);}
	void SetEndDeliveryEmulationTankTemperature(long lTemprature){m_cDeliveryEmulationInfo.SetEndDeliveryEmulationTankTemperature(lTemprature);}
	void SetStartDeliveryEmulationWaterVolume(long lWaterVolume){m_cDeliveryEmulationInfo.SetStartDeliveryEmulationWaterVolume(lWaterVolume);}//4.0.21.500 - TD 66935
	void SetEndDeliveryEmulationWaterVolume(long lWaterVolume){m_cDeliveryEmulationInfo.SetEndDeliveryEmulationWaterVolume(lWaterVolume);}//4.0.21.500 - TD 66935
	//4.0.21.500 TD 66935
	long GetStartDeliveryEmulationVolume(){ return m_cDeliveryEmulationInfo.GetStartDeliveryEmulationVolume(); }
	long GetEndDeliveryEmulationVolume(){ return m_cDeliveryEmulationInfo.GetEndDeliveryEmulationVolume(); }
	long GetStartDeliveryEmulationTankTemperature(){ return m_cDeliveryEmulationInfo.GetStartDeliveryEmulationTankTemperature(); }
	long GetEndDeliveryEmulationTankTemperature(){ return m_cDeliveryEmulationInfo.GetEndDeliveryEmulationTankTemperature(); }
	long GetStartDeliveryEmulationWaterVolume(){ return m_cDeliveryEmulationInfo.GetStartDeliveryEmulationWaterVolume(); }
	long GetEndDeliveryEmulationWaterVolume(){ return m_cDeliveryEmulationInfo.GetEndDeliveryEmulationWaterVolume(); }
	void SetFoundDelivery(BOOL bFoundDelivery){ m_cDeliveryEmulationInfo.SetFoundDelivery(bFoundDelivery); }//4.0.21.500 - TD 66935
	BOOL GetFoundDelivery(){ return m_cDeliveryEmulationInfo.GetFoundDelivery(); } //4.0.21.500 - TD 66935
	void SetVPNotification(BOOL bVPNotification){ m_cDeliveryEmulationInfo.SetVPNotification(bVPNotification); } //4.0.21.500 TD 66935
	BOOL GetVPNotification(){ return m_cDeliveryEmulationInfo.GetVPNotification(); } //4.0.21.500 TD 66935
	void SetDeliveryReference(long lDeliveryReference){ m_cDeliveryEmulationInfo.SetDeliveryReference(lDeliveryReference); }//4.0.20.501 (merge from 4.0.19.300) - TD 65174
	long GetDeliveryReference(){ return m_cDeliveryEmulationInfo.GetDeliveryReference(); } //4.0.20.501 (merge from 4.0.19.300) - TD 65174
	void SetLockConnectedPumps(BOOL bLockConnectedPumps){ m_cDeliveryEmulationInfo.SetLockConnectedPumps(bLockConnectedPumps); }//4.0.21.500 - TD 66935
	BOOL GetLockConnectedPumps(){ return m_cDeliveryEmulationInfo.GetLockConnectedPumps(); } //4.0.21.500 - TD 66935
	//ResetDeliveryEmulationInfo - only info which needs to be saved in the QDX, and therefore: can be reseted together, after writing to QDX:
	void ResetDeliveryEmulationInfo(){m_cDeliveryEmulationInfo.ResetDeliveryEmulationInfo();}
	void ResetFoundDeliveryMember(){ m_cDeliveryEmulationInfo.ResetFoundDeliveryMember(); }//4.0.21.500 - TD 66935
	void ResetDeliveryReferenceMember(){ m_cDeliveryEmulationInfo.ResetDeliveryReferenceMember(); } //4.0.20.501 (merge from 4.0.19.300) - TD 65174
	void ResetLockConnectedPumpsMember(){ m_cDeliveryEmulationInfo.ResetLockConnectedPumpsMember(); }//4.0.21.500 - TD 66935
	void ResetDeliveryEmulationWaterVolumeMember(){ m_cDeliveryEmulationInfo.ResetDeliveryEmulationWaterVolumeMember(); }//4.0.21.500 - TD 66935
	void SaveStartDeliveryEmulationVolume(){m_cDeliveryEmulationInfo.SaveStartDeliveryEmulationVolume(m_lTankNumber);}
	void SaveEndDeliveryEmulationVolume(){m_cDeliveryEmulationInfo.SaveEndDeliveryEmulationVolume(m_lTankNumber);}
	void SaveAccumulatedDeliveryEmulationSales(){m_cDeliveryEmulationInfo.SaveAccumulatedDeliveryEmulationSales(m_lTankNumber);}
	void SaveStartDeliveryEmulationTankTemperature(){m_cDeliveryEmulationInfo.SaveStartDeliveryEmulationTankTemperature(m_lTankNumber);}
	void SaveEndDeliveryEmulationTankTemperature(){m_cDeliveryEmulationInfo.SaveEndDeliveryEmulationTankTemperature(m_lTankNumber);}
	void SaveStartDeliveryEmulationWaterVolume(){m_cDeliveryEmulationInfo.SaveStartDeliveryEmulationWaterVolume(m_lTankNumber);}//4.0.21.500 - TD 66935
	void SaveEndDeliveryEmulationWaterVolume(){m_cDeliveryEmulationInfo.SaveEndDeliveryEmulationWaterVolume(m_lTankNumber);}//4.0.21.500 - TD 66935
	void SaveFoundDelivery(){m_cDeliveryEmulationInfo.SaveFoundDelivery(m_lTankNumber);}//4.0.21.500 - TD 66935
	void ResetFoundDelivery(){ m_cDeliveryEmulationInfo.ResetFoundDelivery(m_lTankNumber);} //4.0.21.500 - TD 66935
	void SaveDeliveryReference(){m_cDeliveryEmulationInfo.SaveDeliveryReference(m_lTankNumber);}//4.0.20.501 (merge from 4.0.19.300) - TD 65174
	void ResetDeliveryReference(){ m_cDeliveryEmulationInfo.ResetDeliveryReference(m_lTankNumber);} //4.0.20.501 (merge from 4.0.19.300) - TD 65174
	void SaveLockConnectedPumps(){m_cDeliveryEmulationInfo.SaveLockConnectedPumps(m_lTankNumber);}//4.0.21.500 - TD 66935
	void ResetLockConnectedPumps(){ m_cDeliveryEmulationInfo.ResetLockConnectedPumps(m_lTankNumber);} //4.0.21.500 - TD 66935
	void ResetDeliveryEmulationWaterVolume(){ m_cDeliveryEmulationInfo.ResetDeliveryEmulationWaterVolume(m_lTankNumber);} //4.0.21.500 - TD 66935
	void ResetDeliveryEmulationSingleTank(){m_cDeliveryEmulationInfo.ResetDeliveryEmulationSingleTank(m_lTankNumber);}    //4.0.21.500 - TD 66935
	void SaveDeliveryEmulationInfo(){m_cDeliveryEmulationInfo.SaveDeliveryEmulationInfo(m_lTankNumber);}
	long SaveDeliveryEmulationInfoInDB(short nFlags, long lVolume, const CString szDeliveryType);
	long CalculateDeliveryEmulation();
	void LoadDeliveryEmulationInfo(){m_cDeliveryEmulationInfo.LoadDeliveryEmulationInfo(m_lTankNumber);}

	void ReSetAccumulatedDeliveryEmulationSales(){m_cDeliveryEmulationInfo.ReSetAccumulatedDeliveryEmulationSales();}

	void SetWasUnlockCommandSet(BOOL bWasUnlockCommandSet){ m_cDeliveryEmulationInfo.SetWasUnlockCommandSet(bWasUnlockCommandSet); }//4.0.21.500 - TD 66935
	BOOL GetWasUnlockCommandSet(){ return m_cDeliveryEmulationInfo.GetWasUnlockCommandSet(); } //4.0.21.500 - TD 66935
	
	//4.0.20.52 (62818) }

	void		SetStartDeliveryEmulationTime(SYSTEMTIME* cTime){m_cDeliveryEmulationInfo.SetStartDeliveryEmulationTime(cTime);}	//4.0.21.500 - TD 69374
	SYSTEMTIME	GetStartDeliveryEmulationTime(){ return m_cDeliveryEmulationInfo.GetStartDeliveryEmulationTime();}					//4.0.21.500 - TD 69374
	void		SaveStartDeliveryEmulationTime(){m_cDeliveryEmulationInfo.SaveStartDeliveryEmulationTime(m_lTankNumber);}			//4.0.21.500 - TD 69374

	void		SetEndDeliveryEmulationTime(SYSTEMTIME* cTime){m_cDeliveryEmulationInfo.SetEndDeliveryEmulationTime(cTime);}		//4.0.21.500 - TD 69374
	SYSTEMTIME	GetEndDeliveryEmulationTime(){ return m_cDeliveryEmulationInfo.GetEndDeliveryEmulationTime();}						//4.0.21.500 - TD 69374
	void		SaveEndDeliveryEmulationTime(){m_cDeliveryEmulationInfo.SaveEndDeliveryEmulationTime(m_lTankNumber);}				//4.0.21.500 - TD 69374

	void InitEndLockTime(){m_cDeliveryEmulationInfo.InitEndLockTime();}						       //4.0.21.500 TD G
	void ResetEndLockTime(){m_cDeliveryEmulationInfo.ResetEndLockTime();}						   //4.0.21.500 TD G
	void InitWaitForLockTime(){m_cDeliveryEmulationInfo.InitWaitForLockTime();}			   	       //4.0.21.500 TD 66935 
	void ResetWaitForLockTime(){m_cDeliveryEmulationInfo.ResetWaitForLockTime();}                  //4.0.21.500 TD 66935
	void InitWaitForReadTime(){m_cDeliveryEmulationInfo.InitWaitForReadTime();}				       //4.0.21.500 TD 66935
	void ResetWaitForReadTime(){m_cDeliveryEmulationInfo.ResetWaitForReadTime();}                  //4.0.21.500 TD 66935
	BOOL IsTimeOutBeforeReading(){return m_cDeliveryEmulationInfo.IsTimeOutBeforeReading();}	   //4.0.21.500 TD G
	BOOL IsTimeOutForLocking(){return m_cDeliveryEmulationInfo.IsTimeOutForLocking(m_lTankNumber);}//4.0.21.500 TD 66935
	BOOL IsTimeOutForReading(){return m_cDeliveryEmulationInfo.IsTimeOutForReading(m_lTankNumber);}//4.0.21.500 TD 66935

	void TurnOnIncDeliveryEmulationSales() {m_cDeliveryEmulationInfo.TurnOnIncDeliveryEmulationSales();}	//4.0.19.1260 - TD 242013
	void TurnOffIncDeliveryEmulationSales() {m_cDeliveryEmulationInfo.TurnOffIncDeliveryEmulationSales();}	//4.0.19.1260 - TD 242013
	BOOL CanIncDeliveryEmulationSales() {return m_cDeliveryEmulationInfo.CanIncDeliveryEmulationSales();}	//4.0.19.1260 - TD 242013
	void UpdateCanIncDeliveryEmulationSales() {m_cDeliveryEmulationInfo.UpdateCanIncDeliveryEmulationSales(m_lTankNumber);} //4.0.19.1260 - TD 242013

	long GetStartDeliveryEmulationDensity(){ return m_cDeliveryEmulationInfo.GetStartDeliveryEmulationDensity(); }				//4.0.28.500 - TD 250092
	void SetStartDeliveryEmulationDensity(long lDensity){m_cDeliveryEmulationInfo.SetStartDeliveryEmulationDensity(lDensity);}	//4.0.28.500 - TD 250092
	void SaveStartDeliveryEmulationDensity(){m_cDeliveryEmulationInfo.SaveStartDeliveryEmulationDensity(m_lTankNumber);}        //4.0.25.380 - TD 287958 
	long GetEndDeliveryEmulationDensity(){ return m_cDeliveryEmulationInfo.GetEndDeliveryEmulationDensity(); }					//4.0.28.500 - TD 250092
	void SetEndDeliveryEmulationDensity(long lDensity){m_cDeliveryEmulationInfo.SetEndDeliveryEmulationDensity(lDensity);}		//4.0.28.500 - TD 250092
	void SaveEndDeliveryEmulationDensity(){m_cDeliveryEmulationInfo.SaveEndDeliveryEmulationDensity(m_lTankNumber);}			//4.0.25.380 - TD 287958

	long GetStartDeliveryEmulationWeight(){ return m_cDeliveryEmulationInfo.GetStartDeliveryEmulationWeight(); }				//4.0.28.500 - TD 250092
	void SetStartDeliveryEmulationWeight(long lWeight){m_cDeliveryEmulationInfo.SetStartDeliveryEmulationWeight(lWeight);}		//4.0.28.500 - TD 250092
	void SaveStartDeliveryEmulationWeight(){m_cDeliveryEmulationInfo.SaveStartDeliveryEmulationWeight(m_lTankNumber);}          //4.0.25.380 - TD 287958 
	long GetEndDeliveryEmulationWeight(){ return m_cDeliveryEmulationInfo.GetEndDeliveryEmulationWeight(); }					//4.0.28.500 - TD 250092
	void SetEndDeliveryEmulationWeight(long lWeight){m_cDeliveryEmulationInfo.SetEndDeliveryEmulationWeight(lWeight);}			//4.0.28.500 - TD 250092
	void SaveEndDeliveryEmulationWeight(){m_cDeliveryEmulationInfo.SaveEndDeliveryEmulationWeight(m_lTankNumber);}             //4.0.25.380 - TD 287958

    // 4.0.25.1060  TD 377336
	long GetStartDeliveryEmulationProductLevel(){ return m_cDeliveryEmulationInfo.GetStartDeliveryEmulationProductLevel(); }
	void SetStartDeliveryEmulationProductLevel(long lProductLevel){m_cDeliveryEmulationInfo.SetStartDeliveryEmulationProductLevel(lProductLevel);}
	void SaveStartDeliveryEmulationProductLevel(){m_cDeliveryEmulationInfo.SaveStartDeliveryEmulationProductLevel(m_lTankNumber);}
	long GetEndDeliveryEmulationProductLevel(){ return m_cDeliveryEmulationInfo.GetEndDeliveryEmulationProductLevel(); }
	void SetEndDeliveryEmulationProductLevel(long lProductLevel){m_cDeliveryEmulationInfo.SetEndDeliveryEmulationProductLevel(lProductLevel);}
	void SaveEndDeliveryEmulationProductLevel(){m_cDeliveryEmulationInfo.SaveEndDeliveryEmulationProductLevel(m_lTankNumber);}
	
	long GetStartDeliveryEmulationWaterLevel(){ return m_cDeliveryEmulationInfo.GetStartDeliveryEmulationWaterLevel(); }
	void SetStartDeliveryEmulationWaterLevel(long lWaterLevel){m_cDeliveryEmulationInfo.SetStartDeliveryEmulationWaterLevel(lWaterLevel);}
	void SaveStartDeliveryEmulationWaterLevel(){m_cDeliveryEmulationInfo.SaveStartDeliveryEmulationWaterLevel(m_lTankNumber);}
	long GetEndDeliveryEmulationWaterLevel(){ return m_cDeliveryEmulationInfo.GetEndDeliveryEmulationWaterLevel(); }
	void SetEndDeliveryEmulationWaterLevel(long lWaterLevel){m_cDeliveryEmulationInfo.SetEndDeliveryEmulationWaterLevel(lWaterLevel);}
	void SaveEndDeliveryEmulationWaterLevel(){m_cDeliveryEmulationInfo.SaveEndDeliveryEmulationWaterLevel(m_lTankNumber);}

	void SetTankState(long lState){ m_eTankState = eTankState(lState); }		//4.0.25.1130 
	long GetTankState(){ return long(m_eTankState); }							//4.0.25.1130 
	BOOL IsTankBlockedByUser();												//4.0.25.1130 

	//Class Ctor
	CTank();

	//Class Copy C'tor
	CTank(const CTank & cTank, BOOL bOnlyTankInfo = TRUE);

	//Class Dtor
	virtual ~CTank();

protected:
	CString GeTankStateDesc();								//4.0.25.1130

	CDeliveryEmulationInfo	m_cDeliveryEmulationInfo;		//4.0.20.52 (62818)
	MEMO_DELIVERY_INFO	m_cMemoDeliveryInfo;
	short m_nNACSCode;
	CComAutoCriticalSection m_csTank;
	bool	m_lTankValid;
	long	m_lTankNumber;
	long	m_lProduct; 
	CString	m_cProductName; 
	long	m_lStatusGeneral;
	long	m_lTankCapacity;

	long	m_lFlags;
	long	m_lFuelLavel;
	long	m_lWaterLavel;
	long	m_lTemperature;
	long	m_lFuelVolume;
	long	m_lWaterVolume;
	long	m_lUllage;
	long	m_lTCVolume;

	long	m_lShiftNumber;
	CTime	m_cTime;
	long	m_lStatusSpecial;
	long	m_lManifold;
	long	m_lVolumeFactor;
	
	//4.0.2.30
	long    m_lLowTankLevelLimit;
	long    m_lHighTankLevelLimit;
	long	m_lLevelFactor; //4.0.3.36
	long	m_lTemperatureFactor; //4.0.3.51
  //Two bit maps parameters, one for pumps 1-32, and the other for pumps 33-64,
  //to sign whether each pump is connected to this tank or not. 4.0.21.500 TD 66935
	long	m_lPumpsLinkedMap1;
	long	m_lPumpsLinkedMap2;
	eTankState	m_eTankState;			 //4.0.25.1130 	
};

#endif // !defined(AFX_TANK_H__F90EC310_5FA8_4CDF_8F17_A8811370B8DE__INCLUDED_)
