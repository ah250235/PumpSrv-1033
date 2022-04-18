// DeliveryEmulationSingleTankServerInfo.h: interface for the CDeliveryEmulationSingleTankServerInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DELIVERYEMULATIONSINGLETANKSERVERINFO_H__70BD4B94_AB28_4308_AD95_0529B2C8D63D__INCLUDED_)
#define AFX_DELIVERYEMULATIONSINGLETANKSERVERINFO_H__70BD4B94_AB28_4308_AD95_0529B2C8D63D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//******************************************************************************
//******************************************************************************

//				CTankDeliveryEmulationServerInfo class

//Holds all the tank server data for the Delivery Emulation single tank process.
//4.0.21.500 - TD 66935

//******************************************************************************
//******************************************************************************
class CTankDeliveryEmulationServerInfo  
{
public:
	//Class Ctor:
	CTankDeliveryEmulationServerInfo();
	//Class Dtor:
	virtual ~CTankDeliveryEmulationServerInfo();

	//Set and get functions:
	void SetDeliveryEmulationStatus(const BYTE byVal);	  
	BYTE GetDeliveryEmulationStatus();

	void SetDeliveryEmulationProcessType(const ProcessDeliverySingleTank lVal); 
	ProcessDeliverySingleTank GetDeliveryEmulationProcessType();					  						  
	
	void SetDeliveryEmulationSuccess(const BYTE byVal);	  
	BYTE GetDeliveryEmulationSuccess();						  
	
	void SetDeliveryEmulationProcessStopped(const SingleTankProcessStopped lVal);
	SingleTankProcessStopped GetDeliveryEmulationProcessStopped();

	void SetLockStatus(const SingleTankLockStatus lVal);   
	SingleTankLockStatus GetLockStatus();					                  

	//Init functions:
	void InitFlags();

private:
	BYTE                      m_byDeliveryEmulationSingleTankStatus;       //Reading emulation single tank state machine statuses
	ProcessDeliverySingleTank m_lDeliveryEmulationSingleTankProcess;       //Delivery emulation single tank state machine statuses
	BYTE                      m_byDeliveryEmulationSingleTankSuccess;      //Outer reading process end status (for BOS)
	SingleTankProcessStopped  m_lDeliveryEmulationSingleTankProcessStopped;//Inner reading process end status
	SingleTankLockStatus      m_lSingleTankLockStatus;                     //Single tank lock pumps' statuses
	CRITICAL_SECTION          m_csTankDeliveryEmulationServerInfo;
};


//******************************************************************************
//******************************************************************************

//				CDeliveryEmulationSingleTankServerInfo class

//Holds all the server data for the Delivery Emulation single tank process.
//4.0.21.500 - TD 66935

//******************************************************************************
//******************************************************************************
class CDeliveryEmulationSingleTankServerInfo  
{
public:
	//Class Ctor:
	CDeliveryEmulationSingleTankServerInfo();
	//Class Dtor:
	virtual ~CDeliveryEmulationSingleTankServerInfo();

	//Set and get functions:
	void SetDeliveryEmulationSingleTankStatus(const long lTankNumber, const BYTE byVal);	  
	BYTE GetDeliveryEmulationSingleTankStatus(const long lTankNumber);

	void SetDeliveryEmulationSingleTankProcessType(const long lTankNumber, const ProcessDeliverySingleTank lVal); 
	ProcessDeliverySingleTank GetDeliveryEmulationSingleTankProcessType(const long lTankNumber);					  						  
	
	void SetDeliveryEmulationSingleTankSuccess(const long lTankNumber, const BYTE byVal);	  
	BYTE GetDeliveryEmulationSingleTankSuccess(const long lTankNumber);						  
	
	void SetDeliveryEmulationSingleTankProcessStopped(const long lTankNumber, const SingleTankProcessStopped lVal);
	SingleTankProcessStopped GetDeliveryEmulationSingleTankProcessStopped(const long lTankNumber);

	void SetSingleTankLockStatus(const long lTankNumber, const SingleTankLockStatus lVal);   
	SingleTankLockStatus GetSingleTankLockStatus(const long lTankNumber);					                  

	//Init functions:
	void InitSingleTankFlags(); 
	void InitSingleSpecificTankFlags(const long lTankNumber);

private:
	CTankDeliveryEmulationServerInfo m_cTankDeliveryEmulationServerInfoArray[MAX_TANKS];
	CRITICAL_SECTION m_csDeliveryEmulationSingleTankServerInfo;
};

#endif // !defined(AFX_DELIVERYEMULATIONSINGLETANKSERVERINFO_H__70BD4B94_AB28_4308_AD95_0529B2C8D63D__INCLUDED_)
