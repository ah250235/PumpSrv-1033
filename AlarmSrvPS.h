// AlarmSrvPS.h: interface for the CAlarmSrvPS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMSRVPS_H__471A86F3_AD02_11D6_BB70_00500470AD8E__INCLUDED_)
#define AFX_ALARMSRVPS_H__471A86F3_AD02_11D6_BB70_00500470AD8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlarmClass.h"
#include "AlarmSrv.h"
#include "Queue.h"


//#pragma pack(1)   //4.0.3.14


//typedef struct
//{
//	BYTE byOpCode;
//	BYTE sfiler[82];  // 4.0.3.11
//	//BYTE sfiler[78]; 4.0.3.11
//}CAlarmSrvGeneralInfo;


typedef struct
{
	//BYTE byOpCode;						// 0 -  0   //4.0.3.15
	long lCatagory;						// 1 -  4 	
	long lSubCatagory;					// 5 -  8 
	long lAlarmNumber;					// 9 - 12
	long lDevice;						//13 - 16		
	//long lAlarmStatus;					//17 - 20 	  //4.0.3.15
	long lFlags; 						//21 - 24
	long lAlarmValue;					//25 - 28	
	char sMessages[MAX_ALARM_DESC_LEN]; //29 - 78
	long lFiller;						//79 - 82	 //4.0.3.11
}CAlarmSrvSendInfo;


typedef struct
{
	BYTE byOpCode;         // 0 -  0		//4.0.3.15
	long lCatagory;		   // 1 -  4 	
	long lSubCatagory;	   // 5	-  8 
	long lAlarmNumber;     // 9 - 12
	long lDevice;		   //13 - 16		
	//long lAlarmStatus;	   //17 - 20 	 //4.0.3.15
	long lAlarmId; 		   //21 - 24	
	long lAlarmValue;	   //25 - 28
	char sFiler[MAX_ALARM_DESC_LEN]; //29 - 78
	long lFlags;					//79-82   //4.0.3.11	
}CAlarmSrvReceivedEventInfo;



//union CAlarmSrvBuffer
//{
//	CAlarmSrvGeneralInfo		cAlarmSrvGeneralInfo;

//};
//#pragma pack()

#define  MAX_ALARM_SRV_QUEUE_BUFF                100    

enum AlarmSrvCmdType
{	
	SEND_TO_ALARM_SRV,
	RECEIVED_TO_ALARM_SRV,
};

class CAlarmSrvQueueData  
{
public:
	void SetCmdData( char *sBuff , long lSize);
	void SetCmd(AlarmSrvCmdType enmCmdType);
	void SetStatus(AlarmStatus	enmAlarmStatus);
	char * GetDataPtr();
	AlarmStatus GetStatus();
	AlarmSrvCmdType GetCommand();
	CAlarmSrvQueueData();
	virtual ~CAlarmSrvQueueData();
protected:	
	AlarmSrvCmdType m_enmAlarmSrvCmdType;
	char	m_sData[MAX_ALARM_SRV_QUEUE_BUFF];
	AlarmStatus	m_enmAlarmStatus;
};

class CAlarmSrvPS  : public CAlarmSrv
{
public:
	long ServerStateAlarm(ServerState lCurrState, ServerState lNewState);
	long PumpStatusAlarm(long lPumpNumber,PumpStatus lCurrStatus, PumpStatus lNewStatus);
	CQueue<CAlarmSrvQueueData, 300>		m_cAlarmSrvQueue;       // 4.0.29.501 TD 290189      
	CQueue<CAlarmSrvQueueData, 300>		m_cAlarmReceivedQueue;  //4.0.3.15  // 4.0.29.501 TD 290189
	BOOL HandleComException( char *szMsgFuncName, int &iRetryCount, int &iBigRetryCount, _com_error e = NULL);
	BOOL IsPumpSrvGroup(long lCategory) const;
	long IsAnyAlarmPump(long lPump) const;
	long DecreaseAlarmNumerator(CAlarmSrvQueueData & cAlarmSrvQueueData);
	void AddAlarmNumerator(CAlarmSrvQueueData & cAlarmSrvQueueData);
	void AddReceivedMessage(long lCatagory,long lSubCatagory,long lAlarmNumber,long lDevice,long lAlarmStatus,long lAlarmId, long lFlags);
	long AddSendMessage(long lCatagory, long lSubCatagory, long lAlarmNumber, long lDevice, long lAlarmStatus, long lAlarmValue, long lFlags, char * sMessages = NULL );	
	long SendAlarmToAlarmSrv(CAlarmSrvQueueData & cAlarmSrvQueueData);
	long WriteComError(_com_error &e);
	CAlarmSrvPS();
	virtual ~CAlarmSrvPS();

private:
	long CheckSpecialAlarmGroup(CAlarmSrvReceivedEventInfo *cAlarmSrvReceivedEventInfo);
	CRITICAL_SECTION m_csAlarmArray;
};


#endif // !defined(AFX_ALARMSRVPS_H__471A86F3_AD02_11D6_BB70_00500470AD8E__INCLUDED_)
