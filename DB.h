// DB.h: interface for the CRelativeFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DB_H__4EBD2013_923A_11D1_9F7A_004005615A06__INCLUDED_)
#define AFX_DB_H__4EBD2013_923A_11D1_9F7A_004005615A06__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif  // _MSC_VER >= 1000

#include "Q-dex.h"
#include "Convert.h"
#include <comutil.h>

#define LOAD_SERVER_INFO			0x00000001
#define LOAD_SHIFT_INFO				0x00000002
#define LOAD_PUMPSTAT_INFO			0x00000004 
#define NOT_LOAD_PUMPSTAT_INFO		0xfffffffb

#define PUMP_PRE_PAY_FILE				1
#define PUMP_OLA_FILE					2        
#define PUMP_STAT_FILE					3          
#define PUMP_TOTALS_FILE				4
#define TANK_READS_FILE					5
#define FUEL_DELIVERIES_FILE			6
#define ALARM_FILE						7
#define CAR_WASH_FILE					8
#define RECEIPT_FILE					9
#define RFS_FILE		   				10				// 3.1.1.6
#define EXTRA_INFO_FILE					13  	
#define ITEMS_FILE						14   			//5.0.0.55	
#define SHIFT_TANKS_FILE				16
#define SHIFT_PUMP_FILE					17
#define SHIFT_DELIVERY_FILE				18               //4.0.2.15 End
#define ASH2000_FILE					19               //4.0.3.47
#define DB_TANK_GAUGE_FILE				20               //4.0.5.24
#define ACCUMULATION_SHIFT_TOTAL_FILE   21               //4.0.5.26
#define RECEIPT_EXTRA_DATA_FILE			22               //4.0.5.39
#define RECEIPT_EXTRA_INDEX_FILE        23				//4.0.7.500         
#define PUMP_OLA_TEMP_FILE			    24				//4.0.9.508	    
#define CASH_ACCEPTOR_FILE			    25				//4.0.9.508	    
#define PAID_TRS_FILE					26				//4.0.11.503
#define RECEIPT_ZONE_FILE				27				// 97890 4.0.25.30 merge from Love's
#define SINGLE_SHIFT_ACCUMULATIONS_FILE	28				// 4.0.25.50 - TD 108311



#define FUEL_LOYALTY_SYS_PARAM_FILE		01
#define FUEL_LOYALTY_SERVER_FILE		02
#define FUEL_LOYALTY_TRIGGER_PROM_FILE	03
#define FUEL_LOYALTY_QUERY_PROM_FILE	04
#define FUEL_LOYALTY_ADD_ITEM_PROM_FILE 05



//4.0.23.20
#define PUMP_PRE_PAY_FILE_SIZE		   sizeof(PRE_PAY_REC)			   //512 
#define PUMP_OLA_FILE_SIZE		       1000							   //Should not be sizeof - do not touch !
#define PUMP_STAT_FILE_SIZE			   0                               //Relative !!
#define PUMP_TOTALS_FILE_SIZE		   sizeof(TOTAL_REC)               //120
#define TANK_READS_FILE_SIZE		   sizeof(TANK_READ_REC)		   //200 //4.0.23.500 - TD 72606
#define FUEL_DELIVERIES_FILE_SIZE	   sizeof(DELIVERY_REC)			   //200 
#define ALARM_FILE_SIZE				   sizeof(ALARM_REC)		       //100
#define CAR_WASH_FILE_SIZE			   sizeof(CAR_WASH_REC)			   //120
#define RECEIPT_FILE_SIZE			   1024							   //Should not be sizeof - do not touch !
#define MSC_FILE_SIZE		   	       0							   //Relative !!	
#define EXTRA_INFO_FILE_SIZE		   sizeof(EXTRA_INFO_REC)		   //800  	
#define ITEMS_FILE_SIZE				   1024							   //Should not be sizeof - do not touch !
#define ASH2000_FILE_SIZE			   sizeof(ASH2000_REC)			   //1024   
#define DB_TANK_GAUGE_SIZE			   0							   //Relative !!	
#define ACCUMULATION_SHIFT_TOTAL_SIZE  sizeof(GradeAccumulateDetails)  //512   
#define RECEIPT_EXTRA_DATA_SIZE		   sizeof(RECEIPT_EXTRA_DOUBLE_REC)       //1024	
#define RECEIPT_EXTRA_INDEX_FILE_SIZE  sizeof(RECEIPT_EXTRA_INDEX_REC) //256	
#define CASH_ACCEPTOR_FILE_SIZE		   sizeof(CASH_ACCEPTOR_REC)       //1024	
#define PAID_TRS_FILE_SIZE			   sizeof(PAID_TRS_REC_EX)	       //512	
#define RECEIPT_ZONE_FILE_SIZE			1024	//4.0.10.30 97890 4.0.25.30 merge from Love's'

//PumpSrv backup flags
#define MS_LOCAL_ONLY							0

#define MS_DEFAULT_CONFIG						0x00000001
#define MS_NOT_DEFAULT_CONFIG					0xfffffffe


#define MS_SERVER_INFO_DEFAULT_CONFIG			0x00000002
#define MS_NOT_SERVER_INFO_DEFAULT_CONFIG		0xfffffffd

#define MS_SHIFT_INFO_DEFAULT_CONFIG			0x00000004
#define MS_NOT_SHIFT_INFO_DEFAULT_CONFIG		0xfffffffb

#define MS_PUMP_STATE_INFO_DEFAULT_CONFIG		0x00000008
#define MS_NOT_PUMP_STATE_INFO_DEFAULT_CONFIG	0xfffffff7

#define Q_LOAD_APP "Q-LOAD32.exe" //4.0.18.0

enum eFileOperation {

		REC_INSERT,
		REC_READ,
		REC_UPDATE,
		REC_DELETE,
		REC_READ_FIRST,
		REC_READ_NEXT,
		REC_DELETE_TILL_INDEX,
		REC_READ_LAST,
		REC_READ_SEQ,
		REC_READ_PREV,
		REC_UPDATE_PREV

};

//33898
enum QdxMaintRetCode
{
	QDX_OK,
	QDX_IN_MIDDLE_OF_PROCESS = 200
};

enum QdxAlarmStatus{	
	QDEX_ALARM_OFF,
	QDEX_ALARM_ON,
};

enum QdxFileType{	
	RELATIVE_FILE_TYPE,
	INDEX_FILE_TYPE,
};

class CQdexFile  
{
public:
	BOOL			CheckPointerValidity(void * Ptr);									//4.0.2.15 

	short			GetFileType();
	unsigned char	GetQdexFileNumber(unsigned char byFileNum);
	void			SetFileNum(unsigned char byFileNum);

	CQdexFile();	
	virtual ~CQdexFile();

	BOOL			IsQdexAlarmOn();
	long			SetAlarm(long lQdxError, long lFileNumber, BOOL lRelative);			//4.0.3.21

	void			StartMaintenance(BOOL bUploadOnly = FALSE, BOOL bForce = FALSE);	//4.0.18.0
	
	BOOL			GetInMiddleOfQDXMaintFlag(){return m_bInMiddleOfQDXMaintFlag;}		//4.0.18.0
	void			SetInMiddleOfQDXMaintFlag(BOOL bInMiddleOfQDXMaintFlag){m_bInMiddleOfQDXMaintFlag = bInMiddleOfQDXMaintFlag;} //4.0.18.0
	
	void			SetFileName(char * sFileName);
	char *			GetFileName();
	
	void			ReSetQDXMaintSeqNumber(){m_lSeqNumber = 0;};						//4.0.18.40

protected:
	virtual void	SetBackUpFlags(long* plFlags);
	
	BOOL			QLoad();					//4.0.18.0
	virtual BOOL	UploadBinFiles();			//4.0.18.0
	virtual long	ParseBinFile(char * sFileName) {return TRUE;};

	void			CatchHydraCOMError(const char* szFuncName, const char* szHydraFuncName, _com_error& cException) const;						//4.0.23.500 TD 39254
	void			CatchAnyError(const char* szFuncName, const char* szHydraFuncName, const long lRetVal) const;								//4.0.23.500 TD 39254

	void			SetQParam(const WORD wOption, const WORD wPartLength, const DWORD dwPartOffset);	//4.0.23.500 TD 39254
public:
	struct q_parm_			q_param;
	unsigned char			nFileNum;
	long					m_lBackUpFlags;	
	CComAutoCriticalSection m_csFile;

protected:
	QdxFileType		enumFileType;
	QdxAlarmStatus	m_enumAlarmStatus;
	BOOL			m_bInMiddleOfQDXMaintFlag;	//4.0.18.0
	DWORD			m_lDoReload;				//4.0.18.0
 	char *			m_sFileName;				//4.0.18.20
	long			m_lDefaultDoReload;
	long			m_lSeqNumber;
};



class CRelativeFile  :	public CQdexFile
{
public:
	long Write(DWORD lWritePartOffset,WORD wWritePartLength,void *pBuff,long lFlags, long lPCNumber, long lTimeStampOffse,long lTimeDelta = 0,long lGroupNum = 0,long lClientEventData = 0);
	long Read(DWORD lReadPartOffset, WORD wReadPartLength, void *pBuff,long lFlags, long lPCNumber, long lTimeStampOffse,long lTimeDelta = 0,long lGroupNum = 0);
	
	long Flush();
	CRelativeFile();
	CRelativeFile(unsigned char byFileNum);
	virtual ~CRelativeFile();

};


class CIndexFile  :	public CQdexFile
{
public:
	
	
	CIndexFile();
	CIndexFile(unsigned char byFileNum , long lRecSize);
	virtual ~CIndexFile();
	long Read(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffset,long lTimeDelta = 0,long lGroupNum = 0);
	long Insert(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffse,long lTimeDelta = 0,long lGroupNum = 0);
	long Update(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffse,long lTimeDelta = 0,long lGroupNum = 0);
	long Delete(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffse,long lTimeDelta = 0,long lGroupNum = 0);
	long ReadNext(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffse,long lTimeDelta = 0,long lGroupNum = 0);
	long ReadPrev(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffse,long lTimeDelta = 0,long lGroupNum = 0);
	long ReadFirst(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffse,long lTimeDelta = 0,long lGroupNum = 0);
	long ReadLast(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffse,long lTimeDelta = 0,long lGroupNum = 0);

	//4.0.18.20 - CR 33898
	long WriteBinFile(char * sFileName, void *pBuff, long lDataSize);
	long ReadBinFile(char * sFileName, void *pBuff, long lDataSize);
	long SaveQDXMaintFile(eFileOperation eOpCode, void * pInfo, long lDataSize); //4.0.18.20

	long SwapLong(long lSrc);


private:

	void ClearQParam();
	long m_lRecSize;
protected:
	BOOL	UploadBinFiles();			//4.0.18.0

};

//4.0.1.32
typedef struct SHIFT_TANKS_REC_
{
	BYTE byValidRecord;
	
	long m_lTankNumber;
	long m_lProduct;
	long m_lStatusGeneral;
	long m_lStatusSpecial;
	long m_lTankCapacity;
	long m_lFuelLevel;
	long m_lWaterLevel;
	long m_lFuelVolume;
	long m_lWaterVolume;
	long m_lTempertaure;
	long m_lUllage;
	long m_lTCVolume;
	long m_lManifold;

	long m_lFlags;
	long m_lShiftNumber;

	short m_nNACSCode;

	char m_sProductName[20];
	SYSTEMTIME m_sTime;
} SHIFT_TANKS_REC;

typedef struct SHIFT_TANKS_FILE_REC_
{
	BYTE byValidRecord;
	SHIFT_TANKS_REC cStartOfShift;
	SHIFT_TANKS_REC cEndOfShift;
} SHIFT_TANKS_FILE_REC;

typedef struct SHIFT_TANKS_FILE_STRUCT_
{
	SHIFT_TANKS_FILE_REC cRec[MAX_TANKS];
} SHIFT_TANKS_FILE_STRUCT;


typedef struct
{
	BYTE byValidRecord;
	DELIVERY_INFO cDeliveryData;
} SHIFT_DELIVERY_REC;

typedef struct
{
	BYTE byValidRecord;
	SHIFT_DELIVERY_REC cStartOfShift;
	SHIFT_DELIVERY_REC cEndOfShift;
} SHIFT_DELIVERY_FILE_REC;

typedef struct
{
	SHIFT_DELIVERY_FILE_REC cRec[MAX_TANKS];
} SHIFT_DELIVERY_FILE_STRUCT;

typedef struct
{
	BYTE byValidRecord;
	
	long m_lIndexNumber;
	long m_lPumpNumber;
	DWORD m_dwShiftNumber;

	short m_nNozzle;
	short m_nGrade;
	short m_nNACSCode;
	short m_nFlags;

	BYTE m_byPumpStatus;
	BYTE m_byNozzleStatus;

	BYTE m_sVolume[MAX_NAME_LENGTH];
	BYTE m_sValueCash[MAX_NAME_LENGTH];
	BYTE m_sValueCredit[MAX_NAME_LENGTH];
	
	char m_sPumpManufacture[MAX_NAME_LENGTH];
	
} SHIFT_TOTALS_REC;  

typedef struct
{
	BYTE byValidRecord;
	SHIFT_TOTALS_REC cStartOfShift;
	SHIFT_TOTALS_REC cEndOfShift;
} SHIFT_TOTALS_FILE_REC;

typedef struct
{
	SHIFT_TOTALS_FILE_REC cRec[MAX_PUMPS_64][MAX_POSITIONS_PER_PUMP];
} SHIFT_TOTALS_FILE_STRUCT;

//4.0.1.32 end

#endif // !defined(AFX_DB_H__4EBD2013_923A_11D1_9F7A_004005615A06__INCLUDED_)
