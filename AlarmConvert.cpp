// AlarmConvert.cpp: implementation of the CAlarmConvert class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlarmConvert.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlarmConvert::CAlarmConvert()
{

}

CAlarmConvert::~CAlarmConvert()
{

}

long CAlarmConvert::ConvertErrorToAlarmCode(long lPumpNumber, long lPumpDevice,BOOL bErrorRep, long lError, long &lCategory, long &lSubCategory, long &lAlarmNumber, long &lAlarmDevice, long &lAlarmStatus)
{

	if (lPumpNumber && (lError == PUMP_NO_ERROR))
	{
		lCategory= CATEGORY_PUMP;
		lSubCategory = lPumpDevice;
		lAlarmNumber = 0;
		lAlarmStatus = FALSE;
	}
	else if (lPumpNumber && (lError ==PUMP_ERROR_NON_SPECIFIC))
	{

		lCategory= CATEGORY_PUMP;
		lSubCategory = lPumpDevice;
		lAlarmNumber = NO_SPECIFIC_ERROR;
		lAlarmStatus = TRUE;

	}
	else if (lPumpNumber && (lError ==PUMP_ERROR_CONFIG))
	{
		lCategory= CATEGORY_PUMP;
		lSubCategory = SUB_TERMINAL_DISPENSER ; 
		lAlarmNumber = CONFIGURATION_ERR;
		lAlarmStatus = TRUE;
	}
	else if (lPumpNumber && (lError ==PUMP_ERROR_BLENDER))
	{
		lCategory= CATEGORY_PUMP;
		lSubCategory = SUB_TERMINAL_DISPENSER ; 
		lAlarmNumber = BLENDER_ERR;
		lAlarmStatus = TRUE;
	}
	else if (lPumpNumber && (lError ==PUMP_ERROR_CASH_READER))
	{
		lCategory= CATEGORY_PUMP;
		lSubCategory = SUB_TERMINAL_CASH_ACCEPTOR ; 
		lAlarmNumber = CASH_READER_ERR;
		lAlarmStatus = TRUE;

	}
	else if (lPumpNumber && (lError == PUMP_ERROR_CARD_READER))
	{
		lCategory= CATEGORY_PUMP;
		lSubCategory = SUB_TERMINAL_CARD_READER ; 
		lAlarmNumber = CARD_READER_ERR;
		lAlarmStatus = TRUE;
		
	}
	else
	{	
		switch(lError)
		{
		   case SYS_NO_ERROR:
			{
				// Need to delete all Error to device.
				// is last error is OLA?
				if (_Module.m_server.m_lSysError == SYS_ERROR_OLA)
				{
					lCategory= CATEGORY_SOFTWARE;
					lSubCategory = SUB_PUMPSRV ; 
					lAlarmNumber = SOFTWARE_NO_ERROR;
					lAlarmStatus = FALSE;
				}
				else
				{
					lCategory= CATEGORY_CONTROLLER;
					lSubCategory = SUB_MEGGIT ; 
					lAlarmNumber = MEGGIT_SYS_NO_ERROR;
					lAlarmStatus = FALSE;
				}

			}					
			break;
			case SYS_ERROR_NON_SPECIFIC:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = SUB_TERMINAL ; 
				lAlarmNumber = NO_SPECIFIC_ERROR;
				lAlarmStatus = TRUE;
			}
			break;
			case SYS_ERROR_CONFIG:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_ERROR_CONFIG;
				lAlarmStatus = TRUE;
			}
			break;
			case SYS_ERROR_HW:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_SHUTDOWN_I2C;
				lAlarmStatus = TRUE;
			}
			break;
			case SYS_ERROR_SW:
			{
				lCategory= CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV ; 
				lAlarmNumber = SOFTWARE_ERR;
				lAlarmStatus = TRUE;
			}
			break;

			case SYS_ERROR_COMM:
			{
				lCategory= CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV ; 
				lAlarmNumber = SOFTWARE_COM_ERR;
				lAlarmStatus = TRUE;
			}
			break;
			case SYS_ERROR_TANK_CONFIG:
			{
				lCategory= CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV ; 
				lAlarmNumber = SOFTWARE_TANK_CONFIGURATION_ERR;
				lAlarmStatus = TRUE;
			}
			break;
			case SYS_ERROR_MISSING_TANK_READING:
			{
				lCategory= CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV ; 
				lAlarmNumber = SOFTWARE_MISSING_TANK_READING_ERR;
				lAlarmStatus = TRUE;
			}
			break ;
			case SYS_ERROR_DELIVERY_FILE_FULL:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_ALLIED ; 
				lAlarmNumber =  ALLIED_SYS_ERROR_DELIVERY_FILE_FULL;
				lAlarmStatus = TRUE;					
			}
			break;
			case SYS_AUDIT_OFFLINE:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_AUDIT_OFFLINE;
				lAlarmStatus = TRUE;
			}
			break;
			case SYS_AUDIT_OUT_OF_PAPER:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_AUDIT_OUT_OF_PAPER;
				lAlarmStatus = TRUE;
			}
			break;
			case SYS_AUDIT_GENERAL_ERROR:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_AUDIT_GENERAL_ERROR;
				lAlarmStatus = TRUE;
			}

			break;
			case SYS_POWER_WARNING:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_POWER_WARNING;
				lAlarmStatus = TRUE;
			}
			break;

			case SYS_POWER_FAILED:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_POWER_FAILED;
				lAlarmStatus = TRUE;
			}
			break;

			case SYS_SHUTDOWN_POS:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_SHUTDOWN_POS;
				lAlarmStatus = TRUE;

			}
			break;

			case SYS_SHUTDOWN_POWER:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_SHUTDOWN_POWER;
				lAlarmStatus = TRUE;
			}

			break;

			case SYS_SHUTDOWN_TASK:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_SHUTDOWN_TASK;
				lAlarmStatus = TRUE;

			}
			break;
			case SYS_SHUTDOWN_CONFIG:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_SHUTDOWN_CONFIG;
				lAlarmStatus = TRUE;
			}
			break;

			case SYS_SHUTDOWN_LEFT:
			{
				lCategory= CATEGORY_CONTROLLER;
				lSubCategory = SUB_MEGGIT ; 
				lAlarmNumber = MEGGIT_SYS_SHUTDOWN_LEFT;
				lAlarmStatus = TRUE;
			}
			break;

			case SYS_PROCESS_LOCK:
			{
				lCategory= CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV ; 
				lAlarmNumber = SOFTWARE_PROCESS_LOCK;
				lAlarmStatus = TRUE;
			}
			break;

			case SYS_ENCRYPTION_LOAD_FAILED:
			{
				lCategory= CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV ; 
				lAlarmNumber = SOFTWARE_ENCRYPTION_LOAD_FAIL;
				lAlarmStatus = TRUE;
			}
			break;

			case SYS_TANK_ALARM:
			{
				lCategory= CATEGORY_TANK;
				lSubCategory = SUB_TANKS ; 
				lAlarmNumber = TANK_ALARM_OK;
				lAlarmStatus = TRUE;
			}
			break;

			case SYS_ERROR_OLA:
			{
				lCategory= CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV ; 
				lAlarmNumber = SOFTWARE_GENERAL_OLA_ERR;
				lAlarmStatus = TRUE;
			}
			break;

			case SYS_ERROR_DISK_WARNING:
			{
				lCategory= CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV ; 
				lAlarmNumber = SOFTWARE_DISK_FULL_WARNING;
				lAlarmStatus = TRUE;
			}
			break;

			case SYS_ERROR_DISK_FULL:
			{
				lCategory= CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV ; 
				lAlarmNumber = SOFTWARE_DISK_FULL_ERR;
				lAlarmStatus = TRUE;
			}
			break;
			default:
				break;
		}		

	}	

	lAlarmDevice = lPumpNumber;						
	return 0;
}

long CAlarmConvert::ConvertWarningToAlarmCode(long lPumpNumber, long lPumpDevice, long lWarning, long &lCategory, long &lSubCategory, long &lAlarmNumber, long &lAlarmDevice, long &lAlarmStatus)
{
	if (lPumpNumber)
	{
		switch(lWarning)
		{			
			case PUMP_DEVICE_NO_WARNING:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = lPumpDevice;
				lAlarmNumber = 0;
				lAlarmStatus = FALSE;
			}
			break;

			case PUMP_DEVICE_WARNING_NON_SPECIFIC:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = lPumpDevice;
				lAlarmNumber = NO_SPECIFIC_ERROR ;
				lAlarmStatus = TRUE;
			}
			break;

			case PUMP_DEVICE_WARNING_PAPER_LOW:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = SUB_TERMINAL_PRINTER ; 
				lAlarmNumber =  PAPER_LOW_WARNING;
				lAlarmStatus = TRUE;
			}
			break;
			case PUMP_DEVICE_WARNING_OUT_OF_PAPER:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = SUB_TERMINAL_PRINTER ; 
				lAlarmNumber =  OUT_OF_PAPER_WARNING;
				lAlarmStatus = TRUE;
			}
			break;
			case PUMP_DEVICE_WARNING_PAPER_JAM:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = SUB_TERMINAL_PRINTER ; 
				lAlarmNumber =  PAPER_JAM_WARNING;
				lAlarmStatus = TRUE;
			}
			break;

			break;
			case PUMP_DEVICE_WARNING_CASH_REJECTED:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = SUB_TERMINAL_CASH_ACCEPTOR ; 
				lAlarmNumber = CASH_REJECTED_WARNING ;
				lAlarmStatus = TRUE;
			}
			break;
			case PUMP_DEVICE_WARNING_CASH_JAMED:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = SUB_TERMINAL_CASH_ACCEPTOR ; 
				lAlarmNumber =  CASH_JAMMED_WARNING;
				lAlarmStatus = TRUE;
			}
			break;
			case PUMP_DEVICE_WARNING_CASH_CASSETTE_FULL:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = SUB_TERMINAL_CASH_ACCEPTOR ; 
				lAlarmNumber =  CASH_CASSETT_FULL_WARNING;				
				lAlarmStatus = TRUE;
			}
			break;
			case PUMP_DEVICE_WARNING_CASH_CASSETTE_REMOVED:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = SUB_TERMINAL_CASH_ACCEPTOR ; 
				lAlarmNumber =  CASH_CASSETT_REMOVE_WARNING;
				lAlarmStatus = TRUE;
			}
			break;
			case PUMP_DEVICE_WARNING_CARD_ENCRYPTION:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = SUB_TERMINAL_ENCRYPTION; 
				lAlarmNumber =  CARD_ENCRYPTION_WARNING;
				lAlarmStatus = TRUE;
			}
			break;
			case PUMP_DEVICE_WARNING_OFFLINE:
			{
				lCategory= CATEGORY_PUMP;
				lSubCategory = lPumpDevice;
				lAlarmNumber =  OFFLINE_WARNING;				
				lAlarmStatus = TRUE;
			}
			break;
				   
		}
	}
	else
	{
		// need to be implement.
		
	}
	lAlarmDevice = lPumpNumber;	
	return 0;
}

long CAlarmConvert::ConvertAlarmCodeToDeviceErrorWarning(long lAlarmId, long &lPumpNumber, long &lAlarmType, long &lVal)
{
   long lAlarmNumber = lAlarmId/100;

	switch(lAlarmNumber)
	{			
		case ALARM_CLEAR_TERMINAL_ERROR: //							2010000
		case ALARM_CLEAR_CARD_READER_ERROR:						   //2020000
		case ALARM_CLEAR_TERMINAL_PRINTER_ERROR:	//				2030000
		case ALARM_CLEAR_TERMINAL_KEYBOARD_ERROR://					2040000
		case ALARM_CLEAR_TERMINAL_DISPLAY_ERROR:				 //	2050000
		case ALARM_CLEAR_TERMINAL_SPEAKER_ERROR:				//	2060000
		case ALARM_CLEAR_TERMINAL_CASH_ACCEPTOR_ERROR:			//2070000
		case ALARM_CLEAR_DISPENSER_ERROR:						//	2080000
		{
			lAlarmType = ALARM_TYPE_UNDEFINE;
			lPumpNumber = lAlarmId%100;
			lVal = 0;
		}
		break;
		case ALARM_PUMP_ERROR_NON_SPECIFIC: 						//2010001
		case ALARM_CARD_READER_ERROR_NON_SPECIFIC: 					//2020001
		case ALARM_PRINTER_ERROR_NON_SPECIFIC: 						//2030001
		case ALARM_KEYBOARD_ERROR_NON_SPECIFIC: 					//2040001
		case ALARM_CASH_ACCEPTOR_ERROR_NON_SPECIFIC: 				//2070001
		case ALARM_DISPENSER_ERROR_NON_SPECIFIC: 					//2080001
		case ALARM_RFID_ANTENNA_ERROR_NON_SPECIFIC: 				//2090001
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal =  PUMP_ERROR_NON_SPECIFIC;
		}
		break;

		case ALARM_TERMINAL_PUMP_DEVICE_WARNING_OFFLINE:				//2010020
		case ALARM_CARD_READER_PUMP_DEVICE_WARNING_OFFLINE:			//2020020
		case ALARM_TERMINAL_PRINTER_PUMP_DEVICE_WARNING_OFFLINE:		//2030020
		case ALARM_TERMINAL_KEYBOARD_PUMP_DEVICE_WARNING_OFFLINE:		//2040020
		case ALARM_TERMINAL_DISPLAY_PUMP_DEVICE_WARNING_OFFLINE:		//2050020
		case ALARM_TERMINAL_SPEAKER_PUMP_DEVICE_WARNING_OFFLINE:		//2060020
		case ALARM_TERMINAL_CASH_ACCEPTOR_PUMP_DEVICE_WARNING_OFFLINE: //2070020
		case ALARM_DISPENSER_PUMP_DEVICE_WARNING_OFFLINE:				//2080020
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_WARNING;
			lVal =  PUMP_DEVICE_WARNING_OFFLINE;
		}
		break;

		case ALARM_PUMP_ERROR_CONFIG:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = PUMP_ERROR_CONFIG;
		}
		break;

		case ALARM_PUMP_ERROR_BLENDER:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = PUMP_ERROR_BLENDER;
		}
		break;
		case ALARM_PUMP_ERROR_CASH_READER:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = PUMP_ERROR_CASH_READER;
		}
		break;
		case ALARM_PUMP_ERROR_CARD_READER:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = PUMP_ERROR_CARD_READER;
		}
		break;
		case ALARM_SYS_ERROR_CONFIG:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_ERROR_CONFIG;
		}
		break;
		case ALARM_SYS_ERROR_HW :
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_ERROR_HW;
		}
		break;
		case ALARM_SYS_ERROR_SW	:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_ERROR_SW;
		}
		break;
		case ALARM_SYS_ERROR_COMM:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_ERROR_COMM;
		}
		break;
		case ALARM_SYS_ERROR_TANK_CONFIG:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_ERROR_TANK_CONFIG;
		}
		break;
		case ALARM_SYS_ERROR_MISSING_TANK_READING:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_ERROR_MISSING_TANK_READING;
		}
		break;
		case ALARM_SYS_ERROR_DELIVERY_FILE_FULL:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_ERROR_DELIVERY_FILE_FULL;
		}
		break;
		case ALARM_SYS_AUDIT_OFFLINE:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_AUDIT_OFFLINE;
		}
		break;
		case ALARM_SYS_AUDIT_OUT_OF_PAPER:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_AUDIT_OUT_OF_PAPER;
		}
		break;
		case ALARM_SYS_AUDIT_GENERAL_ERROR:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_AUDIT_GENERAL_ERROR;
		}
		break;
		case ALARM_SYS_POWER_WARNING:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_POWER_WARNING;
		}
		break;
		case ALARM_SYS_POWER_FAILED:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_POWER_FAILED;
		}
		break;
		case ALARM_SYS_SHUTDOWN_POS:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_SHUTDOWN_POS;
		};
		break;
		case ALARM_SYS_SHUTDOWN_POWER:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_SHUTDOWN_POWER;
		}
		break;
		case ALARM_SYS_SHUTDOWN_TASK:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_SHUTDOWN_TASK;
		}
		break;
		case ALARM_SYS_SHUTDOWN_CONFIG:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_SHUTDOWN_CONFIG;
		}
		break;
		case ALARM_SYS_SHUTDOWN_LEFT:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_SHUTDOWN_LEFT;
		}
		break;
		case ALARM_SYS_PROCESS_LOCK:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_PROCESS_LOCK;
		}
		break;
		case ALARM_SYS_ENCRYPTION_LOAD_FAILED:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_ENCRYPTION_LOAD_FAILED;
		}
		break;
		case ALARM_SYS_TANK_ALARM:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_TANK_ALARM;
		}
		break;
		case ALARM_SYS_ERROR_OLA:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_ERROR_OLA;
		}
		break;
		case ALARM_SYS_ERROR_DISK_WARNING:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_MISC;
			lVal = SYS_ERROR_DISK_WARNING;
		}
		break;
		case ALARM_SYS_ERROR_DISK_FULL:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_MISC;
			lVal = SYS_ERROR_DISK_FULL;
		}
		break;
		case ALARM_CLEAR_PUMPSRV_SOFTWARE_MISC:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_MISC;
			lVal = 0;

		}
		break;
		case ALARM_PUMP_DEVICE_WARNING_NON_SPECIFIC:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_WARNING;
			lVal = PUMP_DEVICE_WARNING_NON_SPECIFIC;
		}
		break;
		case ALARM_PUMP_DEVICE_WARNING_PAPER_LOW:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_WARNING;
			lVal = PUMP_DEVICE_WARNING_PAPER_LOW;
		}
		break;
		case ALARM_PUMP_DEVICE_WARNING_OUT_OF_PAPER:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_WARNING;
			lVal = PUMP_DEVICE_WARNING_OUT_OF_PAPER;
		}
		break;
		case ALARM_PUMP_DEVICE_WARNING_PAPER_JAM:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_WARNING;
			lVal = PUMP_DEVICE_WARNING_PAPER_JAM; 
		}
		break;
		case ALARM_PUMP_DEVICE_WARNING_CASH_REJECTED:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_WARNING;
			lVal = PUMP_DEVICE_WARNING_CASH_REJECTED;
		}
		break;
		case ALARM_PUMP_DEVICE_WARNING_CASH_JAMED:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType =ALARM_TYPE_WARNING; 
			lVal = PUMP_DEVICE_WARNING_CASH_JAMED;
		}
		break;
		case ALARM_PUMP_DEVICE_WARNING_CASH_CASSETTE_FULL:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType =ALARM_TYPE_WARNING; 
			lVal = PUMP_DEVICE_WARNING_CASH_CASSETTE_FULL;
		}
		break;
		case ALARM_PUMP_DEVICE_WARNING_CASH_CASSETTE_REMOVED:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_WARNING;
			lVal = PUMP_DEVICE_WARNING_CASH_CASSETTE_REMOVED;
		}
		break;
		case ALARM_PUMP_DEVICE_WARNING_CARD_ENCRYPTION:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_WARNING;
			lVal = PUMP_DEVICE_WARNING_CARD_ENCRYPTION;
		}
		break;
		case ALARM_MEGGIT_SYS_NO_ERROR:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_ERROR;
			lVal = SYS_NO_ERROR;
		}
		break;
		case ALARM_ALLIED_SYS_ERROR_COMM:					//			3020001	 //misc
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_MISC;
			lVal = SYS_ERROR_COMM;

		}
		break;				
		case ALARM_ALLIED_SYS_ERROR_NON_SPECIFIC: //						3020002  //misc
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_MISC;
			lVal = SYS_ERROR_NON_SPECIFIC;

		}
		break;
		case ALARM_ALLIED_SYS_ERROR_HW: //								3020003  //misc	
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_MISC;
			lVal = SYS_ERROR_HW;

		}
		break;

		case ALARM_ALLIED_SYS_ERROR_SW:			//					3020004  //misc	 
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_MISC;
			lVal = SYS_ERROR_SW;

		}
		break;
		case ALARM_ALLIED_SYS_ERROR_DELIVERY_FILE_FULL: //				3020005  //misc
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_MISC;
			lVal = SYS_ERROR_DELIVERY_FILE_FULL;
		}
		break;
		
		case ALARM_ENCRYPTION_NON_SPECIFIC:
		{
			lPumpNumber = lAlarmId%100;
			lAlarmType = ALARM_TYPE_MESSAGE;   
			lVal = PUMP_MSG_CARD_ENCRYPTION;
		}
		break;
		
		default:
		{
			
		}
		break;

	}

	return 0;
}

BOOL CAlarmConvert::IsPumpSrvAlarm(long lCategory)
{
	BOOL bFound = FALSE;

	switch(lCategory)
	{
	case CATEGORY_TANK:
		bFound = TRUE;
		break;			
	case CATEGORY_PUMP:
		bFound = TRUE;
		break;			
	case CATEGORY_SOFTWARE :
		bFound = TRUE;
		break;			
	case CATEGORY_DEVICES:
		bFound = TRUE;
		break;			
	case CATEGORY_CONTROLLER :
		bFound = TRUE;
		break;			
	}

	return bFound;		
}

long CAlarmConvert::ConvertMiscAlarmCode(long lPumpNumber, long lPumpDevice, BOOL bErrorRep, long lError, BOOL bMiscRep, long lMisc , long *pVal)
{
	long lCategory = 0; 
	long lSubCategory =0; 
	long lAlarmNumber= 0; 
	long lAlarmDevice =0; 
	long lAlarmStatus =0;
	long lAlarmValue = 0;	
	long lFlags =0;
	

	BOOL bRtc = TRUE;

	if (!lPumpNumber)
	{
		switch (lMisc)
		{
			case SYS_NO_ERROR:   // try to clear all  system 
			{
				lCategory    = CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV;
				lAlarmNumber = SOFTWARE_NO_ERROR;				
				lAlarmStatus = FALSE;
				bRtc= FALSE;
			}
			break;
			
			case SYS_ERROR_DISK_FULL:
			{
				lCategory    = CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV;
				lAlarmNumber = SOFTWARE_DISK_FULL_ERR;				
				lAlarmStatus = TRUE;
				bRtc= FALSE;
			}
			break;
			case SYS_ERROR_DISK_WARNING:
			{
				lCategory    = CATEGORY_SOFTWARE;
				lSubCategory = SUB_PUMPSRV;
				lAlarmNumber = SOFTWARE_DISK_FULL_WARNING;				
				lAlarmStatus = TRUE;
				bRtc= FALSE;
			}			
			break;
			case SYS_ERROR_COMM : //3020001
			{
				lCategory    = CATEGORY_SOFTWARE;
				lSubCategory = SUB_ALLIED;
				lAlarmNumber = ALLIED_SYS_ERROR_COMM;				
				lAlarmStatus = TRUE;
				bRtc= FALSE;
			}
			break;
			case SYS_ERROR_NON_SPECIFIC :  //3020002
			{
				lCategory    = CATEGORY_SOFTWARE;
				lSubCategory = SUB_ALLIED;
				lAlarmNumber = ALLIED_SYS_ERROR_NON_SPECIFIC;				
				lAlarmStatus = TRUE;
				bRtc= FALSE;
			}
			break;
			case SYS_ERROR_HW :  //3020003
			{
				lCategory    = CATEGORY_SOFTWARE;
				lSubCategory = SUB_ALLIED;
				lAlarmNumber = ALLIED_SYS_ERROR_HW;				
				lAlarmStatus = TRUE;
				bRtc= FALSE;
			}
			break;
			case SYS_ERROR_SW :  //3020004
			{
				lCategory    = CATEGORY_SOFTWARE;
				lSubCategory = SUB_ALLIED;
				lAlarmNumber = ALLIED_SYS_ERROR_SW;
				bRtc= FALSE;
			}
			break;
			case SYS_ERROR_DELIVERY_FILE_FULL :  //3020005
			{
				lCategory    = CATEGORY_SOFTWARE;
				lSubCategory = SUB_ALLIED;
				lAlarmNumber = ALLIED_SYS_ERROR_DELIVERY_FILE_FULL;
				bRtc= FALSE;
			}
			break;

		}
	}

	if (!bRtc)
	{
		GCIRetCode retCode =GCI_OK;
		long lRetVal=0;

		if (_Module.m_server.GetAlarmSrvMode())
		{
			lRetVal = _Module.m_server.m_cAlarm.AddSendMessage(lCategory, lSubCategory, lAlarmNumber, lAlarmDevice,lAlarmStatus, lAlarmValue, lFlags);

			if (lRetVal)
				retCode = GCI_FAILED_IO;

			if(pVal != NULL)
				*pVal = retCode;			
		}	
		else
			bRtc= TRUE;			
	}

	return bRtc;
}
