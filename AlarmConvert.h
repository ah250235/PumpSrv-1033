// AlarmConvert.h: interface for the CAlarmConvert class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMCONVERT_H__23312FD7_AEA7_11D6_BB71_00500470AD8E__INCLUDED_)
#define AFX_ALARMCONVERT_H__23312FD7_AEA7_11D6_BB71_00500470AD8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ALARM_PUMP_ERROR_NON_SPECIFIC 						201001
#define ALARM_CARD_READER_ERROR_NON_SPECIFIC 				202001
#define ALARM_PRINTER_ERROR_NON_SPECIFIC 					203001
#define ALARM_KEYBOARD_ERROR_NON_SPECIFIC 					204001
#define ALARM_CASH_ACCEPTOR_ERROR_NON_SPECIFIC 				207001
#define ALARM_DISPENSER_ERROR_NON_SPECIFIC 					208001
#define ALARM_RFID_ANTENNA_ERROR_NON_SPECIFIC 				209001
#define ALARM_ENCRYPTION_NON_SPECIFIC						210001    //4.0.3.14 

#define ALARM_PUMP_ERROR_CONFIG	      						208004
#define ALARM_PUMP_ERROR_BLENDER							208003		
#define ALARM_PUMP_ERROR_CASH_READER						207006
#define ALARM_PUMP_ERROR_CARD_READER						202002	


#define ALARM_SYS_ERROR_SW									301003
#define ALARM_SYS_ERROR_COMM								301004
#define ALARM_SYS_ERROR_TANK_CONFIG							301005
#define ALARM_SYS_ERROR_MISSING_TANK_READING				301006

#define ALARM_SYS_ERROR_CONFIG								501006
#define ALARM_SYS_POWER_WARNING 							501001
#define ALARM_SYS_POWER_FAILED  							501002
#define ALARM_SYS_AUDIT_OFFLINE								501003
#define ALARM_SYS_AUDIT_OUT_OF_PAPER 						501004
#define ALARM_SYS_AUDIT_GENERAL_ERROR 						501005
#define ALARM_SYS_SHUTDOWN_POS 								501007
#define ALARM_SYS_SHUTDOWN_POWER							501008
#define ALARM_SYS_SHUTDOWN_TASK								501009	
#define ALARM_SYS_SHUTDOWN_CONFIG							501010	
#define ALARM_SYS_ERROR_HW 									501011
#define ALARM_SYS_SHUTDOWN_LEFT								501012	
#define ALARM_MEGGIT_SYS_NO_ERROR							501013
#define ALARM_SYS_ERROR_DELIVERY_FILE_FULL					502005

#define ALARM_SYS_TANK_ALARM								102000

// PumpSrv software definition
#define ALARM_CLEAR_PUMPSRV_SOFTWARE_MISC					301000
#define ALARM_SYS_ERROR_OLA									301013
#define ALARM_SYS_ERROR_DISK_WARNING						301014	
#define ALARM_SYS_ERROR_DISK_FULL							301015

#define ALARM_SYS_PROCESS_LOCK								301011	
#define ALARM_SYS_ENCRYPTION_LOAD_FAILED					301012


#define ALARM_PUMP_DEVICE_WARNING_NON_SPECIFIC				203008
#define ALARM_PUMP_DEVICE_WARNING_PAPER_LOW					203002
#define ALARM_PUMP_DEVICE_WARNING_OUT_OF_PAPER				203003
#define ALARM_PUMP_DEVICE_WARNING_PAPER_JAM					203004
#define ALARM_PUMP_DEVICE_WARNING_CASH_REJECTED				207002
#define ALARM_PUMP_DEVICE_WARNING_CASH_JAMED				207003
#define ALARM_PUMP_DEVICE_WARNING_CASH_CASSETTE_FULL		207004
#define ALARM_PUMP_DEVICE_WARNING_CASH_CASSETTE_REMOVED		207005
#define ALARM_PUMP_DEVICE_WARNING_CARD_ENCRYPTION			210002
#define ALARM_CLEAR_TERMINAL_ERROR							201000
#define ALARM_CLEAR_CARD_READER_ERROR						202000
#define ALARM_CLEAR_TERMINAL_PRINTER_ERROR					203000
#define ALARM_CLEAR_TERMINAL_KEYBOARD_ERROR					204000
#define ALARM_CLEAR_TERMINAL_DISPLAY_ERROR					205000
#define ALARM_CLEAR_TERMINAL_SPEAKER_ERROR					206000
#define ALARM_CLEAR_TERMINAL_CASH_ACCEPTOR_ERROR			207000
#define ALARM_CLEAR_DISPENSER_ERROR							208000


#define ALARM_TERMINAL_PUMP_DEVICE_WARNING_OFFLINE				201020
#define ALARM_CARD_READER_PUMP_DEVICE_WARNING_OFFLINE			202020
#define ALARM_TERMINAL_PRINTER_PUMP_DEVICE_WARNING_OFFLINE		203020
#define ALARM_TERMINAL_KEYBOARD_PUMP_DEVICE_WARNING_OFFLINE		204020
#define ALARM_TERMINAL_DISPLAY_PUMP_DEVICE_WARNING_OFFLINE		205020
#define ALARM_TERMINAL_SPEAKER_PUMP_DEVICE_WARNING_OFFLINE		206020
#define ALARM_TERMINAL_CASH_ACCEPTOR_PUMP_DEVICE_WARNING_OFFLINE 207020
#define ALARM_DISPENSER_PUMP_DEVICE_WARNING_OFFLINE				208020




// Allied software definition
#define ALARM_ALLIED_SYS_ERROR_COMM								302001	 //misc
#define ALARM_ALLIED_SYS_ERROR_NON_SPECIFIC						302002  //misc
#define ALARM_ALLIED_SYS_ERROR_HW								302003  //misc	
#define ALARM_ALLIED_SYS_ERROR_SW								302004  //misc	 
#define ALARM_ALLIED_SYS_ERROR_DELIVERY_FILE_FULL				302005  //misc



class CAlarmConvert  
{
public:
	long ConvertMiscAlarmCode(long lPumpNumber, long lPumpDevice,BOOL bErrorRep, long lError, BOOL bMiscRep, long lMisc, long *pVal);
	BOOL IsPumpSrvAlarm(long lCategory);
	long ConvertAlarmCodeToDeviceErrorWarning(long lAlarmId, long &lPumpNumber, long &lAlarmType, long &lVal);
	long ConvertWarningToAlarmCode(long lPumpNumber, long lPumpDevice, long lWarning, long &lCategory, long &lSubCategory, long &lAlarmNumber, long &lAlarmDevice, long &lAlarmStatus);
	long ConvertErrorToAlarmCode(long lPumpNumber, long lPumpDevice,BOOL bErrorRep, long lError, long &lCategory, long &lSubCategory, long &lAlarmNumber, long & lAlarmDevice,long &lAlarmStatus);		 
	CAlarmConvert();
	virtual ~CAlarmConvert();

};

#endif // !defined(AFX_ALARMCONVERT_H__23312FD7_AEA7_11D6_BB71_00500470AD8E__INCLUDED_)
