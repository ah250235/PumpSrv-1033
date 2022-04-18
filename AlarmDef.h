#ifndef _ALARM_DEF_
#define _ALARM_DEF_

// interval Sleep thread
#define THRD_SLEEP_TIME			100
#define MAINTENANCE_THRD_SLEEP_TIME	500

/*
#define REC_READ				1
#define REC_DELETE				2
#define REC_INSERT				3
#define REC_UPDATE				4
*/


// field size 
#define STR_LONG_VAL2		2
#define STR_LONG_VAL3		3
#define STR_LONG_VAL5		5
#define STR_LONG_VAL10		10
#define STR_16				16

#define MAX_ALARM_SEQUENCES_NUMBER		999999999


#define TIME_BETWEEN_ALARM_ENDED_TYPE_CHECKS   1000
#define TIME_BETWEEN_SNOOZED_ALARM_CHECKS      1000 

#define CATCH_GAI					1
#define CATCH_CTRL					1

enum AlarmSrvRetCode {
					ALARM_SRV_OK,					//0
					ALARM_SRV_FAIL_DELETE,			//1
					ALARM_SRV_FAIL_INSERT,			//2
					ALARM_SRV_FAIL_UPDATE,			//3
					ALARM_SRV_REACHED_MAX_COUNTER,	//4
};

enum MailGroupType {
					MAIL_GROUP_REGULAR,				//0
					MAIL_GROUP_SMS,					//1
};

#define MICROSOFT_ALEF						0xE0
#define MICROSOFT_TAF						0xFA
#define ASCII_ALEF							0x80
#define MICROSOFT_SHACH                     '$'
#define ASCII_SHACH							0x9B
#define ASCII_TAF							0x9A
#define CR									0x0D
#define LF									0x0A


// General logger definition
#define ALARM_PATH				"Software\\Retalix\\AlarmSrv"
#define DEFAULT_LOG_FILE_PATH	"C:\\Program Files\\Retalix\\AlarmSrv\\Log\\"
#define DEFAULT_MAIL_INI_PATH	"C:\\Program Files\\Retalix\\AlarmSrv\\Mail.ini"
#define	DEFAULT_PATH			"C:\\Program Files\\Retalix\\AlarmSrv"
#define DEFAULT_HOME_DIR		"C:\\Program Files\\Retalix\\AlarmSrv"
#define DEFAULT_TEXT_FILE_PATH	"C:\\Program Files\\Retalix\\AlarmSrv\\AlarmsTextFile.txt"
#define ALARM_LOG_FILE			"AlarmSrv.Log"
#define ALARM_SIGNATURE			"AlarmSrv 1.0.0.0"
#define DEFAULT_MAIL_ADDRESS	"127.0.0.1"


// Maximum parameters number
#define MAX_CATEGORY_NUMBER		20
#define MAX_SUBCATEGORY_NUMBER	99
//#define MAX_ALARM_NUMBER		9999 //1.0.8.500
#define MAX_ALARM_NUMBER		999
#define MAX_DEVICE_NUMBER		999

#define MAX_EMAIL_GROUP			32	


#define FUEL_EVENT_LOG			"Retalix"
#define FUEL_EVENT_LOG_SOURCE	"AlarmSrv"
#define FUEL_EVENT_LOG_MAX_SIZE	524288


#pragma pack(1)

typedef struct 
{
	BYTE			sYear[4];    
	BYTE			sMonth[2];
	BYTE			sDay[2];    
	BYTE			sHour[2];
	BYTE			sMinute[2];    
	BYTE			sSecond[2];    	
}TAG_SYSTEMTIME2; //1.0.13.500

//1.0.13.500
typedef  struct
{
	BYTE sAlarmPriority[10];
	BYTE sFiller[100];
}ALARM_EXTRA_PARAMS;

typedef struct
{
	long lAlarmCategory;
	long lAlarmSubCategory;
	long lAlarmNumber;
	long lDeviceNumber;
	long lAlarmId;
	long lAlarmStatus;
	ALARM_EXTRA_PARAMS sAlarmExtraParam; //1.0.13.500
}ALARM_DATA;

#pragma pack()
#endif //_ALARM_DEF_
