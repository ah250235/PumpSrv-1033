// PumpSrvDB.cpp: implementation of the CPumpFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PumpSrvDB.h"
#include "DB.h"
#include "SQLPumpSrvFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//	CPumpFile Class 
//////////////////////////////////////////////////////////////////////

CPumpFile::CPumpFile():CRelativeFile(PUMP_STAT_FILE)
{
	m_lPumpFileStatus = FILE_OK;
}

CPumpFile::~CPumpFile()
{

}

// 0 - Valid signature
// 1 - Not Valid signature
// 2 - Cant check signature
long CPumpFile::ValidityCheck()
{	
	BYTE		sFileValid[FILE_VALIDATION_LEN+1];
	long		lRetCode = 0;
	
	memset(sFileValid,0,sizeof(sFileValid));

	

	m_csFile.Lock();
	long lRtc = Read(0,FILE_VALIDATION_LEN,sFileValid,HOP_DIRECT_LOCAL ,0 ,0);	
	m_csFile.Unlock();
	
	if(lRtc == OK)
	{
		
		if (!memcmp(sFileValid,PUMPSRV_PUMPSTAT_CURRENT_VERSION,FILE_VALIDATION_LEN))
		{// valid 
			lRetCode = 0;			
		}
		else
		{// not valid

			//4.0.14.500
			if (!memcmp(sFileValid,PUMPSRV_PUMPSTAT_CURRENT_409, FILE_VALIDATION_LEN)) // 4.0.9.507
			{
				//Add convert!!! //rtttttt
			}
			else if (!memcmp(sFileValid,PUMPSRV_PUMPSTAT_CURRENT_405, FILE_VALIDATION_LEN)) // 4.0.9.507
			{
				ExpendedSessTypeAndPumps();  //4.0.9.601
			}
			else if (!memcmp(sFileValid,PUMPSRV_PUMPSTAT_CURRENT_40, FILE_VALIDATION_LEN))
			{
				WidenOLA2ThreeRec(); //4.0.5.0
				ExpendedSessTypeAndPumps();  //4.0.9.507
				lRetCode = 0;
			}
			else if (!memcmp(sFileValid,PUMPSRV_PUMPSTAT_VERSION_24, FILE_VALIDATION_LEN))
			{
				
				if(LOG_DETAIL_FLOW_CONTROL)
				{
					_LOGMSG.LogMsg("Upadte data base from ver 2.4 to ver 3.0");
				}

				Convert24VerTo29Ver();
				Convert29VerTo30Ver();
				
				//4.0.1.0 Rearrange Q-Dex for 64 pump support
				Convert30VerTo64Pumps();
				WidenOLA2ThreeRec(); //4.0.5.0
				ExpendedSessTypeAndPumps();  //4.0.9.507

				lRetCode = 0;
			}
			else
			{
				if (!memcmp(sFileValid,PUMPSRV_PUMPSTAT_VERSION_29,FILE_VALIDATION_LEN))	
				{
					if(LOG_DETAIL_FLOW_CONTROL)
					{
						_LOGMSG.LogMsg("Upadte data base from ver 2.9 to ver 3.0");
					}

					Convert29VerTo30Ver();

					//4.0.1.0 Rearrange Q-Dex for 64 pump support
					Convert30VerTo64Pumps();
					WidenOLA2ThreeRec(); //4.0.5.0
					ExpendedSessTypeAndPumps();  //4.0.9.507

					lRetCode = 0;
				}
				else
				{
					if(!memcmp(sFileValid , PUMPSRV_PUMPSTAT_VERSION_30 , FILE_VALIDATION_LEN))
					{
						if(LOG_DETAIL_FLOW_CONTROL)
						_LOGMSG.LogMsg("Upadte PUMPSRV : 64 pumps");
						
						Convert30VerTo64Pumps();
						WidenOLA2ThreeRec();  //4.0.5.0
						ExpendedSessTypeAndPumps();  //4.0.9.507
						lRetCode = 0;
					}
					else
						lRetCode = 1;
				}
			}

			if (lRetCode)
			{
				if(LOG_ERROR)
				{
					CString str;
					_LOGMSG.LogMsg("ValidityCheck Failed, ",LOG_LEVEL_0);
					str.Format(						  "Signature read from file: '%*s'",FILE_VALIDATION_LEN,sFileValid);
					_LOGMSG.LogMsg(str,LOG_LEVEL_0);
					str.Format(						  "Compared to:              '%*s'",FILE_VALIDATION_LEN,PUMPSRV_PUMPSTAT_CURRENT_VERSION);
					_LOGMSG.LogMsg(str,LOG_LEVEL_0);
				}
			}			
			
			_Module.m_server.CheckExistUnBalancePrePay();
			
			if(LOG_ERROR)
			{
				CString strLog;
				strLog.Format("ValidityCheck, Delivery emulation");
				_LOGMSG.LogMsg(strLog);
			}

			_Module.m_server.m_cServerInfo.SetDeliveryEmulationStatus(DES_IDLE);
			_Module.m_server.m_cServerInfo.SetDeliveryEmulationProcessType(PD_NONE);
		}
	}
	else
	{
		lRetCode = 2;
		if(LOG_ERROR)
		{
			CString str;
			str.Format("ValidityCheck, Read error %d, (indx_num %d, offset %d, length %d)",lRtc,q_param.indx_num,q_param.write_part_offset,q_param.write_part_length);
			_LOGMSG.LogMsg(str,LOG_LEVEL_0);
		}
		if (lRtc == ERR_NOT_OPENED)	//4.0.22.430 131250 //4.0.25.30
		{
			lRetCode = 3;
		}
	}

	_Module.m_server.m_cCompConnectivity.SetQdexConn(lRtc);
	
	return(lRetCode);
}

// 0		- OK
// Non 0	- Failed	
long CPumpFile::SetFileSignature()
{
	BYTE		sFileValid[FILE_VALIDATION_LEN];

	memcpy(sFileValid, PUMPSRV_PUMPSTAT_CURRENT_VERSION, FILE_VALIDATION_LEN);
	
	m_csFile.Lock();
	long lRtc = Write(0,FILE_VALIDATION_LEN,sFileValid, HOP_DEFAULT_CONFIG,0,0);	
	m_csFile.Unlock();

	if(lRtc)
	{
		if(LOG_ERROR)
		{
			CString str;
			str.Format("SetFileSignature, Write error %d, (indx_num %d, offset %d, length %d) , lRtc=%ld",lRtc,q_param.indx_num,q_param.write_part_offset,q_param.write_part_length ,lRtc);
			_LOGMSG.LogMsg(str,LOG_LEVEL_0);
		}
	}

	return lRtc ;	
}

long CPumpFile::Convert24VerTo29Ver()
{
	BYTE		sFileValid[FILE_VALIDATION_LEN];

	typedef struct 
	{
		BYTE				sFileValid[sizeof(PUMP_INFO)];	//0		(604)
		PUMP_INFO			p[MAX_PUMPS]; 					//604	(604 * 32 = 19328)
		CARD_SALE_INFO_		pap[MAX_PUMPS];					//19932	(900 * 32 = 28800)
		SERVER_INFO			serverInfo;						//48732	(68)
		SHIFT_REC			currentShift;					//48800	(52)
	}PUMP_FILE_TMP;	// Total size 48852


	long lRtc;
	m_csFile.Lock();
    {
		//Move shift record.
		SHIFT_REC shiftInfo;
		memset(&shiftInfo,0,sizeof(SHIFT_REC));

		lRtc = Read(offsetof(PUMP_FILE_TMP,currentShift) - sizeof(long)*(MAX_PUMPS+1),
					(WORD)sizeof(SHIFT_REC),
					&shiftInfo,HOP_DIRECT_LOCAL,0,0);

		lRtc = Write(offsetof(PUMP_FILE_TMP,currentShift),
					 (WORD)sizeof(SHIFT_REC),
					 &shiftInfo,HOP_DEFAULT_CONFIG,0,0);

		//Move server info record.
		SERVER_INFO serverInfo;
		memset(&serverInfo,0,sizeof(SERVER_INFO));

		lRtc = Read(offsetof(PUMP_FILE_TMP,serverInfo) - sizeof(long)*(MAX_PUMPS+1),
					(WORD)sizeof(SERVER_INFO),
					&serverInfo,HOP_DIRECT_LOCAL,0,0);

		
		lRtc = Write(offsetof(PUMP_FILE_TMP,serverInfo),
					 (WORD)sizeof(SERVER_INFO),
					 &serverInfo,HOP_DEFAULT_CONFIG,0,0);

		//Move PAP record.
		CARD_SALE_INFO_ papInfo;
				
		for ( int i = MAX_PUMPS-1 ; i >= 0  ; i-- )
		{
			memset(&papInfo,0,sizeof(CARD_SALE_INFO_));

			lRtc = Read(offsetof(PUMP_FILE_TMP,pap[i]) - sizeof(long)*(MAX_PUMPS+1),
						sizeof(CARD_SALE_INFO_),
						&papInfo,HOP_DIRECT_LOCAL,0,0);
		
			lRtc = Write(offsetof(PUMP_FILE_TMP,pap[i]),
						 sizeof(CARD_SALE_INFO_),
						 &papInfo,HOP_DEFAULT_CONFIG,0,0);
		}



		//Move Pump info record.
		PUMP_INFO pumpInfo;
		memset(&pumpInfo,0,sizeof(PUMP_INFO));
		long lMessage = 0;

		for ( int i = MAX_PUMPS-1 ; i >= 0  ; i-- )
		{
			
			lRtc = Read(offsetof(PUMP_FILE_TMP,p[i]) - sizeof(long)*(i+1),
						sizeof(PUMP_INFO),
						&pumpInfo,HOP_DIRECT_LOCAL,0,0);



			lRtc = Write(offsetof(PUMP_FILE_TMP,p[i]),
						546,//offset of PUMP_FILE.PUMP_INFO.CPumpStatus.m_lMessage
						&pumpInfo,HOP_DEFAULT_CONFIG,0,0);

			lRtc = Write(offsetof(PUMP_FILE_TMP,p[i]) + 546,
						sizeof(lMessage),
						&lMessage,HOP_DEFAULT_CONFIG,0,0);

			lRtc = Write(offsetof(PUMP_FILE_TMP,p[i]) + 546 + sizeof(lMessage),
						54,//offset of PUMP_FILE.PUMP_INFO.CPumpStatus.m_lMessage
						((char *)&pumpInfo) + 546 ,HOP_DEFAULT_CONFIG,0,0);

			lRtc = Read(offsetof(PUMP_FILE_TMP,p[i]),
						sizeof(PUMP_INFO),
						&pumpInfo, HOP_DIRECT_LOCAL,0,0);
		}

		lRtc = Write(sizeof(PUMP_INFO) - sizeof(long),
					sizeof(long),
					&lMessage,HOP_DEFAULT_CONFIG,0,0);
		


	///////////////////////////////////////////////////////////

		lRtc = Read(offsetof(PUMP_FILE_TMP,currentShift) ,
					(WORD)sizeof(SHIFT_REC),
					&shiftInfo,HOP_DIRECT_LOCAL,0,0);

		
		//Move server info record.

		memset(&serverInfo,0,sizeof(SERVER_INFO));

		lRtc = Read(offsetof(PUMP_FILE_TMP,serverInfo) ,
					(WORD)sizeof(SERVER_INFO),
					&serverInfo,HOP_DIRECT_LOCAL,0,0);//4.0.0.66

		

		for (int i = 0 ; i < MAX_PUMPS ; i++ )
		{
			long lWriteOffset;
			memset(&papInfo,0,sizeof(CARD_SALE_INFO_));

			
			lRtc = Read(offsetof(PUMP_FILE_TMP,pap[i]),
						 sizeof(CARD_SALE_INFO_),
						 &papInfo,HOP_DIRECT_LOCAL,0,0);

			lWriteOffset = offsetof(PUMP_FILE_TMP,pap[i]); 
		}

		for (int i = MAX_PUMPS-1 ; i >= 0  ; i-- )
		{
			
			lRtc = Read(offsetof(PUMP_FILE_TMP,p[i]) ,
						sizeof(PUMP_INFO),
						&pumpInfo,HOP_DIRECT_LOCAL,0,0);

		}


	////////////////////////////////////////////////




		//	SetFileSignature
		memcpy(sFileValid, PUMPSRV_PUMPSTAT_VERSION_29, FILE_VALIDATION_LEN);
		lRtc = Write(0,FILE_VALIDATION_LEN,sFileValid ,HOP_DEFAULT_CONFIG,0,0);
    }
	//4.0.2.15
	m_csFile.Unlock();
	

	if(lRtc)
	{
		if(LOG_ERROR)
		{
			CString str;
			str.Format("SetFileSignature, from version 2.4 to  2.9 , Write error %d, (indx_num %d, offset %d, length %d)",lRtc,q_param.indx_num,q_param.write_part_offset,q_param.write_part_length);
			_LOGMSG.LogMsg(str,LOG_LEVEL_0);
		}
	}

	return 0;
}
//4.0.3.56
/******************************************************************************
 Description:	Widen PumpState to support tree ola records 	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			27/10/2003   14:59		Start
******************************************************************************/

long CPumpFile::WidenOLA2ThreeRec()
{
	long lRtc;
	CString sMsg;	

	// find the offset to old strcut data.

	size_t CutPasteOffset = sizeof(CARD_SALE_EXTRA_DATA3_) * MAX_PUMPS_64;

		                    	
	m_csFile.Lock();

	SHIFT_REC shiftInfo;
	memset(&shiftInfo,0,sizeof(SHIFT_REC));

	SERVER_INFO servInfo;
	memset(&servInfo , 0 , sizeof(SERVER_INFO));

	PAY_AT_PUMP_INFO tmpPap;
	memset(&tmpPap , 0 , sizeof(PAY_AT_PUMP_INFO));

	//Read shift info from its old offset
	lRtc = Read(offsetof(PUMP_FILE,currentShift) - CutPasteOffset, (WORD)(sizeof(SHIFT_REC)), &shiftInfo, HOP_DIRECT_LOCAL, 0, 0);	//4.0.20.503

	//4.0.1.1 Update shift record held in PumpSrv memory, so that the call
	//to ResetPumpStatQdxData made later on in this method has the shift
	//number ready for use.
	_Module.m_server.SetCurrentShift(&shiftInfo);

	if(lRtc)
	{
		sMsg.Format("Read shift info returned %d" , lRtc);
		_LOGMSG.LogMsg(sMsg);
	}
	//Write it back to the file at the proper place.
	lRtc = Write(offsetof(PUMP_FILE,currentShift),
				 (WORD)sizeof(SHIFT_REC),
				 &shiftInfo , HOP_DEFAULT_CONFIG,0,0);
	
	if(lRtc)
	{
		sMsg.Format("Write shift info returned %d" , lRtc);
		_LOGMSG.LogMsg(sMsg);
	}

	//Read server info from its old position
	lRtc = Read(offsetof(PUMP_FILE , serverInfo) - CutPasteOffset, (WORD)(sizeof(SERVER_INFO)), &servInfo, HOP_DIRECT_LOCAL, 0, 0);		//4.0.20.503

	if(lRtc)
	{
		sMsg.Format("Read server info returned %d" , lRtc);
		_LOGMSG.LogMsg(sMsg);
	}

	//Write it back at the correct offset
	lRtc = Write(offsetof(PUMP_FILE , serverInfo),
				 (WORD)sizeof(SERVER_INFO),
				 &servInfo,HOP_DEFAULT_CONFIG,0,0);

	if(lRtc)
	{
		sMsg.Format("Write server info returned %d" , lRtc);
		_LOGMSG.LogMsg(sMsg);
	}

	//Due to Q-Dex limitations, we will read\write one record at a time

	for(int iOffset = MAX_PUMPS_64 - 1;iOffset >= 0;iOffset--)
	{

		lRtc = Read(offsetof(PUMP_FILE,pap) + iOffset * sizeof(PAY_AT_PUMP_INFO) - CutPasteOffset, 
					(WORD)(sizeof(tmpPap)),	&tmpPap , HOP_DIRECT_LOCAL, 0, 0); //4.0.20.503

		if(lRtc)
		{
			sMsg.Format("Read PAP info returned %d" , lRtc);
			_LOGMSG.LogMsg(sMsg);
		}

		//Write it back to the file at the proper place.
		lRtc = Write(offsetof(PUMP_FILE,pap) + iOffset * sizeof(PAY_AT_PUMP_INFO),
					 (WORD)sizeof(tmpPap),
					 &tmpPap , HOP_DEFAULT_CONFIG,0,0);

		if(lRtc)
		{
			sMsg.Format("Write PAP info returned %d" , lRtc);
			_LOGMSG.LogMsg(sMsg);
		}
	}

	m_csFile.Unlock();

	
	SetFileSignature();

	_LOGMSG.LogMsg("Widen OLA to three records." , DEFAULT_LOG_LEVEL);
	
	return 0;
}



/******************************************************************************
 Description:	Checks if the PumpStat is ready to use after data synchronizing.
				The method save or reload PumpStat data according to the state.
 Returns:      	BOOL - TRUE if ready, FALSE - if not ready to use.
 Parameters:   	BOOL* pbStatusChange - return if qdex file status was change.		
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			3/2/2004   13:45		Start
******************************************************************************/
BOOL CPumpFile::IsFileReady(BOOL* pbChangeToReady)
{
	BOOL bFileReady = TRUE;
	long lRetVal = 0;
	BOOL bChangeToReady = FALSE;

	if( m_lPumpFileStatus == FILE_OK)
		bFileReady = TRUE;
	else
	{
		if( bFileReady )
		{
			//check what is needed
			if(m_lPumpFileStatus == FILE_WAIT_FOR_LOAD)
			{
				CRegTable cReg;
				cReg.LoadPumpsTable(0 , TRUE , FALSE, TRUE);
				bChangeToReady = TRUE;
				m_lPumpFileStatus = FILE_OK;
			}
			else if(m_lPumpFileStatus == FILE_WAIT_FOR_SAVE)
			{
				//save all pump stat info from memory to qdex
				lRetVal = SaveAllData();
				if(lRetVal)
					bFileReady = FALSE;
				else
				{
					m_lPumpFileStatus = FILE_OK;
					bChangeToReady = TRUE;
				}		
			}
		}
	}
	//here to add the bStatusChange
	if(pbChangeToReady)
		*pbChangeToReady = bChangeToReady;
	
	return bFileReady;
}

/******************************************************************************
 Description:	Checks if the PumpStat is ready for operations by trying to 
				read from the file.
 Returns:      	BOOL - TRUE if ready, FALSE - if not ready to use.
 Parameters:   	None		
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			3/2/2004   13:45		Start
******************************************************************************/
BOOL CPumpFile::IsQdexReady()
{
	BOOL	bQdexReady = TRUE;	
	BYTE	sFileValid[FILE_VALIDATION_LEN+1];
	long	lRetCode = 0;


	memset(sFileValid,0,sizeof(sFileValid));

	m_csFile.Lock();
	long lRtc = Read(0,FILE_VALIDATION_LEN,sFileValid,HOP_DIRECT_LOCAL ,0 ,0);	
	m_csFile.Unlock();
	
	if(lRtc == OK)
		bQdexReady = TRUE;
	else
		bQdexReady = FALSE;
	
	return bQdexReady;
}
/******************************************************************************
 Description:	Saves all PumpSrv run time data into the PumpStat qdex. 	
 Returns:      	long - qdex engine return codes.
 Parameters:   	None.

 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			3/2/2004   13:52		Start
******************************************************************************/
long CPumpFile::SaveAllData()
{
	CSQLPumpSrvFile & SQLPumpSrvFile = CSQLPumpSrvFile::GetInstance();

	long lRetCode = 0;
	long lLastValidPump = _Module.m_server.GetLastValidPump();

	SQLPumpSrvFile.SaveServerInfo();
	lRetCode = SQLPumpSrvFile.SaveShiftInfo();

	
	for (int p = 1; p <= lLastValidPump; p++)
	{
		SQLPumpSrvFile.SaveInfo(p, __FUNCTION__, __LINE__);
		SQLPumpSrvFile.SavePAPInfo(p, __FUNCTION__, __LINE__);
	}
	
	return lRetCode;
}


//////////////////////////////////////////////////////////////////////
// CPumpPaidTrs Class
//////////////////////////////////////////////////////////////////////

//4.0.11.503
CPumpPaidTrs::CPumpPaidTrs():CIndexFile(PAID_TRS_FILE, PAID_TRS_FILE_SIZE)
{
}

long CPumpFile::Convert29VerTo30Ver()
{
	long lRtc;

	//4.0.1.1 Added this decleration and REPLACED ALL OLD REFERENCES
	//to PUMP_FILE in order to avoid miscalculated offsets due to
	//changes in MAX_PUMPS macro
	typedef struct 
	{
		BYTE				sFileValid[sizeof(PUMP_INFO)];
		PUMP_INFO			p[MAX_PUMPS];
		PAY_AT_PUMP_INFO	pap[MAX_PUMPS];
		SERVER_INFO			serverInfo; 
		SHIFT_REC			currentShift; 
	}OLD_PUMP_FILE;


	m_csFile.Lock();


	//Move shift record.
	SHIFT_REC shiftInfo;
	memset(&shiftInfo,0,sizeof(SHIFT_REC));

	lRtc = Read(offsetof(OLD_PUMP_FILE,currentShift) - (sizeof(CARD_SALE_EXTRA_DATA2_) * MAX_PUMPS),
				(WORD)sizeof(SHIFT_REC),
				&shiftInfo, HOP_DIRECT_LOCAL,0,0);

	lRtc = Write(offsetof(OLD_PUMP_FILE,currentShift),
				 (WORD)sizeof(SHIFT_REC),
				 &shiftInfo, HOP_DEFAULT_CONFIG,0,0);

	//Move server info record.
	SERVER_INFO serverInfo;
	memset(&serverInfo,0,sizeof(SERVER_INFO));

	lRtc = Read(offsetof(OLD_PUMP_FILE,serverInfo) - (sizeof(CARD_SALE_EXTRA_DATA2_) * MAX_PUMPS),
				(WORD)sizeof(SERVER_INFO),
				&serverInfo, HOP_DIRECT_LOCAL,0,0);

	
	lRtc = Write(offsetof(OLD_PUMP_FILE,serverInfo),
				 (WORD)sizeof(SERVER_INFO),
				 &serverInfo, HOP_DEFAULT_CONFIG,0,0);

	//Move PAP record from last to start
	PAY_AT_PUMP_INFO papInfo;
	
	for ( int i = (MAX_PUMPS - 1);  i >= 0  ; i--)
	{
		// empty buffer...
		long	lIndexRead = offsetof(OLD_PUMP_FILE,pap[i]) - (sizeof(CARD_SALE_EXTRA_DATA2_) * i);
		long	lIndexWrite = offsetof(OLD_PUMP_FILE,pap[i]);

		memset(&papInfo,0,sizeof(PAY_AT_PUMP_INFO));

		lRtc = Read(offsetof(OLD_PUMP_FILE,pap[i]) - (sizeof(CARD_SALE_EXTRA_DATA2_) * i),
					sizeof(CARD_SALE_INFO_),
					&papInfo, HOP_DIRECT_LOCAL,0,0);					

		lRtc = Write(offsetof(OLD_PUMP_FILE,pap[i]),
					 sizeof(PAY_AT_PUMP_INFO),
					 &papInfo, HOP_DEFAULT_CONFIG,0,0);	
		

	}

	m_csFile.Unlock();

	for (int i=1;i <= MAX_MODES;i++)
	{
		CString str("Modes\\Mode");
		CString strModeName("Mode");
		char sNumber[8];
		DWORD  dwTmp;

		memset(sNumber,0,sizeof(sNumber));
		sprintf(sNumber,"%02d",i);
		str += sNumber;
		strModeName += sNumber;

		 if (!_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)str), "PrePayValid",&dwTmp,(DWORD)1,FALSE))
		 {
			 if (dwTmp)
				_Module.m_server.m_cParam.SaveParam((char *)((LPCTSTR)str), "PrePayAuto",dwTmp);			 	
		 }
	}

	SetFileSignature();
	

	// OLAIndex
	DWORD		dwOLAIndex;
	
	dwOLAIndex = MAX_INDEX_NUMBER; 
	_Module.m_server.m_cParam.SaveParam("GeneralParam","OLAIndex",dwOLAIndex ,TRUE,0,0,0,TRUE); //4.0.8.503
	

	return 0;
}


CItemsFile::CItemsFile():CIndexFile(ITEMS_FILE,ITEMS_FILE_SIZE)
{

}

CItemsFile::~CItemsFile()
{

}

long CItemsFile::ParseBinFile(char * sFileName) //33898
{
	ITEMS_INFO_QDX_MAINT_REC maintRec;
	memset(&maintRec, 0, sizeof(ITEMS_INFO_QDX_MAINT_REC));
	BOOL bRetVal = ReadBinFile(sFileName,&maintRec,sizeof(ITEMS_INFO_QDX_MAINT_REC));
	if (bRetVal)
	{
		CString str;
		str.Format("QDXMaint - CItemsFile::ParseBinFile file = %s",sFileName); //4.0.18.40 //4.0.18.501
		_LOGMSG.LogMsg(str);
		Operation(maintRec.opCode, &maintRec.info, TRUE);
	}
	return bRetVal;
}


long CItemsFile::Operation(eFileOperation OpCode, void *pInfo, BOOL bIsSaveByMaint) //4.0.18.20
{
	ITEMS_INFO  *pDoubleItemRecord = NULL;	
	long lRetVal = 0;
	long lIndexNumber = 0;

	ITEM_REC		itemRec;
	ITEM_REC2		itemRec2;
	ITEM_REC3		itemRec3;
	ITEM_REC4		itemRec4;
	long lPCNumber = 0 ;
	long lTimeStampOffset = 0;
	

	memset(&itemRec , 0, sizeof(ITEM_REC));
	memset(&itemRec2 , 0, sizeof(ITEM_REC2));
	pDoubleItemRecord = (ITEMS_INFO *)pInfo;

	if (m_bInMiddleOfQDXMaintFlag && !bIsSaveByMaint) //4.0.18.20 - CR 33898
	{
		ITEMS_INFO_QDX_MAINT_REC		maintRec;
		memset (&maintRec,0,sizeof(ITEMS_INFO_QDX_MAINT_REC));
		maintRec.opCode = OpCode;
		if (pDoubleItemRecord)
			maintRec.info = *pDoubleItemRecord;
		lRetVal = SaveQDXMaintFile(OpCode, &maintRec, sizeof(ITEMS_INFO_QDX_MAINT_REC));
	}
	else
	{

		m_csFile.Lock();
		
		switch (OpCode)	
		{
			case REC_INSERT:			
			{
				itemRec = pDoubleItemRecord->ItemRec;
				lIndexNumber = itemRec.lIndexNumber;
				
				itemRec.lIndexNumber = SwapLong(lIndexNumber); 						
				lRetVal = Insert(&itemRec,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			

				 // if first record save ok. insert second record			 			
				if (!lRetVal)
				{				
					itemRec2 = pDoubleItemRecord->ItemRec2;
					itemRec2.lIndexNumber = SwapLong(lIndexNumber + 1); 
					lRetVal = Insert(&itemRec2,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			

					// if fail insert second record to qdx, then delete first record
					if (lRetVal)
					{
						itemRec.lIndexNumber = SwapLong(lIndexNumber); 
						lRetVal = Delete(&itemRec,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);
						lRetVal = Q_ERROR;
					}
					//4.0.1.34
					else
					{
						itemRec3 = pDoubleItemRecord->ItemRec3;
						itemRec3.lIndexNumber = SwapLong(lIndexNumber + 2); 
						lRetVal = Insert(&itemRec3,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			

						if (!lRetVal)
						{
							itemRec4 = pDoubleItemRecord->ItemRec4;
							itemRec4.lIndexNumber = SwapLong(lIndexNumber + 3); 
							lRetVal = Insert(&itemRec4,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			
						}

					}
					//4.0.1.34

				}
				break;
			}
			case REC_READ:
			{
				itemRec = pDoubleItemRecord->ItemRec;
				lIndexNumber = itemRec.lIndexNumber;
				
				itemRec.lIndexNumber = SwapLong(lIndexNumber); 						
				lRetVal = Read(&itemRec,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);			

				if(!lRetVal )
				{	
					itemRec.lIndexNumber = SwapLong(itemRec.lIndexNumber);

					if(pDoubleItemRecord)
					{					
						pDoubleItemRecord->ItemRec = itemRec;							
					}

					// if first record save ok. insert second record			 			
					itemRec2.lIndexNumber = SwapLong(lIndexNumber + 1); 

					lRetVal = Read(&itemRec2,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);			

					if (!lRetVal)
					{ 
						itemRec2.lIndexNumber = SwapLong(lIndexNumber+ 1);					
						pDoubleItemRecord->ItemRec2 = itemRec2;	


						//4.0.1.34
						// if first record save ok. insert second record			 			
						itemRec3.lIndexNumber = SwapLong(lIndexNumber + 2); 

						lRetVal = Read(&itemRec3,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);			
						if (!lRetVal)
						{ 
							itemRec3.lIndexNumber = SwapLong(lIndexNumber+ 2);					

							pDoubleItemRecord->ItemRec3 = itemRec3;	
							
							
							if (!lRetVal)
							{					
								//4.0.5.29
								itemRec4.lIndexNumber = SwapLong(lIndexNumber + 3); 							
								lRetVal = Read(&itemRec4,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			
								pDoubleItemRecord->ItemRec4 = itemRec4;
							}


						}
						//4.0.1.34
					}

				}
				else if(lRetVal == Q_ERROR)
					lRetVal = ERR_NOT_FOUND;
				break;
			}
			case REC_READ_FIRST:
			{
				
				lRetVal = ReadFirst(&itemRec,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);		
				
				if(!lRetVal )
				{	
					itemRec.lIndexNumber = SwapLong(itemRec.lIndexNumber);

					lIndexNumber = itemRec.lIndexNumber;

					if(pDoubleItemRecord)
					{
						pDoubleItemRecord->ItemRec =itemRec;
					}

					itemRec2.lIndexNumber = SwapLong(lIndexNumber + 1);
					lRetVal = Read(&itemRec2,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);		

					if(!lRetVal )
					{
						itemRec2.lIndexNumber = SwapLong(itemRec2.lIndexNumber);					
						pDoubleItemRecord->ItemRec2 = itemRec2;

						//4.0.1.34
						itemRec3.lIndexNumber = SwapLong(lIndexNumber + 2);
						lRetVal = Read(&itemRec3,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);		

						if(!lRetVal )
						{
							itemRec3.lIndexNumber = SwapLong(itemRec3.lIndexNumber);					
							pDoubleItemRecord->ItemRec3 = itemRec3;

							if (!lRetVal)
							{							
								itemRec4.lIndexNumber = SwapLong(lIndexNumber + 3); 
								lRetVal = Read(&itemRec4,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			
								pDoubleItemRecord->ItemRec4 = itemRec4;

							}

						}
						//4.0.1.34
					}
				}
				else if(lRetVal == Q_ERROR)
					lRetVal = ERR_NOT_FOUND;

				break;
			}

			case REC_UPDATE:
			{			
				itemRec = pDoubleItemRecord->ItemRec;	
				lIndexNumber = itemRec.lIndexNumber;

				itemRec.lIndexNumber = SwapLong(lIndexNumber);
				lRetVal = Update(&itemRec,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);

				// if first record update  ok then update second record			 			
				if(!lRetVal )
				{
					itemRec2 = pDoubleItemRecord->ItemRec2;	
					itemRec2.lIndexNumber = SwapLong(lIndexNumber + 1);
					lRetVal = Update(&itemRec2,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);		

					//4.0.1.34
					if(!lRetVal )
					{
						itemRec3 = pDoubleItemRecord->ItemRec3;	
						itemRec3.lIndexNumber = SwapLong(lIndexNumber + 2);
						lRetVal = Update(&itemRec3,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);		


						if (!lRetVal)
						{	
							itemRec4 = pDoubleItemRecord->ItemRec4;
							itemRec4.lIndexNumber = SwapLong(lIndexNumber + 3); 							
							lRetVal = Update(&itemRec4,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);									
						}

					}
					//4.0.1.34


				}

				break;
			}
			case REC_DELETE:
			{
				lIndexNumber = pDoubleItemRecord->ItemRec.lIndexNumber;

				itemRec.lIndexNumber = SwapLong(lIndexNumber);			
				lRetVal = Delete(&itemRec,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);		

				if (!lRetVal)
				{
					itemRec2.lIndexNumber = SwapLong(lIndexNumber + 1);			
					lRetVal = Delete(&itemRec2,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			
					//4.0.1.34
					if (!lRetVal)
					{
						itemRec3.lIndexNumber = SwapLong(lIndexNumber + 2);			
						lRetVal = Delete(&itemRec3,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			

						if (!lRetVal)
						{					
							
							itemRec4.lIndexNumber = SwapLong(lIndexNumber + 3); 							
							lRetVal = Delete(&itemRec4,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);									
						}

					}	
					//4.0.1.34
				}
				break;
			}

			case REC_READ_NEXT:
			{
				lIndexNumber = pDoubleItemRecord->ItemRec.lIndexNumber + 2;

				itemRec.lIndexNumber = SwapLong(lIndexNumber);	
				//lRetVal = ReadNext(&rec);
				lRetVal = Read(&itemRec,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);
				if(!lRetVal )
				{
					itemRec.lIndexNumber = SwapLong(itemRec.lIndexNumber);
					
					if(pDoubleItemRecord)
					{
						pDoubleItemRecord->ItemRec = itemRec;
					}
					
					itemRec2.lIndexNumber = SwapLong(lIndexNumber + 1); 
					
					lRetVal = Read(&itemRec2,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);

					if (!lRetVal)
					{ 												
						pDoubleItemRecord->ItemRec2= itemRec2;
						//4.0.1.34	
						if(!lRetVal )
						{						
							itemRec3.lIndexNumber = SwapLong(lIndexNumber + 2); 
							
							lRetVal = Read(&itemRec3,HOP_DIRECT_LOCAL ,lPCNumber,lTimeStampOffset);

							if (!lRetVal)
							{ 												
								pDoubleItemRecord->ItemRec3= itemRec3;

								itemRec4.lIndexNumber = SwapLong(lIndexNumber + 3); 							
								if (!lRetVal)
								{														
									lRetVal = Delete(&itemRec4,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);									
								}

							}



						}
						//4.0.1.34	
					}



				}
				else if(lRetVal == Q_ERROR)
					lRetVal = ERR_NOT_FOUND;

				break;
			
			}
	
		}

		m_csFile.Unlock();
	}
	return  lRetVal;	
	
}
//5.0.0.55
//4.0.5.28
long CItemsFile::SplitBAPRecorToItems(long lItemIndex, BAP_ALL_DATA_STRUCTS & cTagItemsRec, ITEMS_INFO &cItemInfo )
{
	long lRecSize = sizeof(BAP_ALL_DATA_STRUCTS);

	memset(&cItemInfo , ' ' , sizeof(cItemInfo));
	// Copy first record.
	cItemInfo.ItemRec.lIndexNumber  = lItemIndex;
	memcpy(cItemInfo.ItemRec.sBuff1, &cTagItemsRec  , sizeof(cItemInfo.ItemRec.sBuff1));
	lRecSize -= sizeof(cItemInfo.ItemRec.sBuff1);

	// Copy second record
	cItemInfo.ItemRec2.lIndexNumber = lItemIndex + 1;
	memcpy(cItemInfo.ItemRec2.sBuff1, (char *)&cTagItemsRec + sizeof(cItemInfo.ItemRec2.sBuff1) , min(lRecSize, sizeof(cItemInfo.ItemRec2.sBuff1)));
	lRecSize -= sizeof(cItemInfo.ItemRec2.sBuff1);

	//4.0.1.34
	// Copy second record
	cItemInfo.ItemRec3.lIndexNumber = lItemIndex + 2;
	memcpy(cItemInfo.ItemRec3.sBuff1, (char *)&cTagItemsRec + sizeof(cItemInfo.ItemRec.sBuff1) + sizeof(cItemInfo.ItemRec2.sBuff1) , min(lRecSize, sizeof(cItemInfo.ItemRec3.sBuff1)));  //4.0.5.18
	lRecSize -= sizeof(cItemInfo.ItemRec3.sBuff1);
	//4.0.1.34

	//4.0.5.27
	// Copy second record
	cItemInfo.ItemRec4.lIndexNumber = lItemIndex + 3;
	memcpy(cItemInfo.ItemRec4.sBuff1, (char *)&cTagItemsRec + sizeof(cItemInfo.ItemRec.sBuff1) + sizeof(cItemInfo.ItemRec2.sBuff1) , min(lRecSize, sizeof(cItemInfo.ItemRec3.sBuff1)));  //4.0.5.18	
	//4.0.5.27


	return 0;
}

long CItemsFile::ConstructItemsRecordToBAP(ITEMS_INFO &cItemInfo, BAP_ALL_DATA_STRUCTS &cTagItemsRec)
{
	 long lRecSize = sizeof(BAP_ALL_DATA_STRUCTS); //4.0.5.28

	 memset(&cTagItemsRec , ' ' , sizeof(BAP_ALL_DATA_STRUCTS));

	// Copy first record.
	memcpy(&cTagItemsRec, cItemInfo.ItemRec.sBuff1, sizeof(cItemInfo.ItemRec.sBuff1));
	lRecSize -= sizeof(cItemInfo.ItemRec.sBuff1);

	// Copy second record
	memcpy((char *)&cTagItemsRec + sizeof(cItemInfo.ItemRec.sBuff1), cItemInfo.ItemRec2.sBuff1, min(lRecSize, sizeof(cItemInfo.ItemRec2.sBuff1)));
	lRecSize -= sizeof(cItemInfo.ItemRec2.sBuff1);

	//4.0.5.14
	// Copy Third record
	memcpy((char *)&cTagItemsRec + sizeof(cItemInfo.ItemRec.sBuff1) + sizeof(cItemInfo.ItemRec2.sBuff1), cItemInfo.ItemRec3.sBuff1, min(lRecSize, sizeof(cItemInfo.ItemRec3.sBuff1)));
	lRecSize -= sizeof(cItemInfo.ItemRec3.sBuff1);

	// Copy   record    //4.0.5.27
	memcpy((char *)&cTagItemsRec + sizeof(cItemInfo.ItemRec.sBuff1) + sizeof(cItemInfo.ItemRec2.sBuff1) + sizeof(cItemInfo.ItemRec.sBuff1), cItemInfo.ItemRec4.sBuff1, min(lRecSize, sizeof(cItemInfo.ItemRec4.sBuff1)));


	return 0;
}


/******************************************************************************
 Description:	Rewrite Pumps.qdx to comply with 64 pumps 	
 Returns:      	long
 Parameters:   	
 Assumptions:   the system can hold up to  32 pumps, 
                and is now being reconfigured for the first time
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Eyal			25/8/2002   11:00		Start
******************************************************************************/
long CPumpFile::Convert30VerTo64Pumps()
{


	long lRtc;
	CString sMsg;

	//The arrays p and pap have grown from 32 to 64.  Therefore, all the records
	//that come after both arrays must be moved forward by CutPasteOffset
	size_t CutPasteOffset = sizeof(PAY_AT_PUMP_INFO) * MAX_PUMPS + 
		                    sizeof(PUMP_INFO) * MAX_PUMPS;
	
	m_csFile.Lock();

	SHIFT_REC shiftInfo;
	memset(&shiftInfo,0,sizeof(SHIFT_REC));

	SERVER_INFO servInfo;
	memset(&servInfo , 0 , sizeof(SERVER_INFO));

	PAY_AT_PUMP_INFO tmpPap;
	memset(&tmpPap , 0 , sizeof(PAY_AT_PUMP_INFO));

	//Read shift info from its old offset
	lRtc = Read(offsetof(PUMP_FILE,currentShift) - CutPasteOffset, (WORD)(sizeof(SHIFT_REC)), &shiftInfo, HOP_DIRECT_LOCAL, 0, 0);	//4.0.20.503

	//4.0.1.1 Update shift record held in PumpSrv memory, so that the call
	//to ResetPumpStatQdxData made later on in this method has the shift
	//number ready for use.
	_Module.m_server.SetCurrentShift(&shiftInfo);

	if(lRtc)
	{
		sMsg.Format("Read shift info returned %d" , lRtc);
		_LOGMSG.LogMsg(sMsg);
	}
	//Write it back to the file at the proper place.
	lRtc = Write(offsetof(PUMP_FILE,currentShift),
				 (WORD)sizeof(SHIFT_REC),
				 &shiftInfo , HOP_DEFAULT_CONFIG,0,0);
	
	if(lRtc)
	{
		sMsg.Format("Write shift info returned %d" , lRtc);
		_LOGMSG.LogMsg(sMsg);
	}

	//Read server info from its old position
	lRtc = Read(offsetof(PUMP_FILE , serverInfo) - CutPasteOffset, (WORD)(sizeof(SERVER_INFO)), &servInfo, HOP_DIRECT_LOCAL,0,0);	//4.0.20.503

	if(lRtc)
	{
		sMsg.Format("Read server info returned %d" , lRtc);
		_LOGMSG.LogMsg(sMsg);
	}

	//Write it back at the correct offset
	lRtc = Write(offsetof(PUMP_FILE , serverInfo),
				 (WORD)sizeof(SERVER_INFO),
				 &servInfo,HOP_DEFAULT_CONFIG,0,0);

	if(lRtc)
	{
		sMsg.Format("Write server info returned %d" , lRtc);
		_LOGMSG.LogMsg(sMsg);
	}

	//Due to Q-Dex limitations, we will read\write one record at a time
	for(int iOffset = MAX_PUMPS - 1;iOffset >= 0;iOffset--)
	{
		//4.0.0.83
		//Read pap info from its old offset
		//Here, the offset is less than the others because there is only
		//one array before "pap".
		lRtc = Read(offsetof(PUMP_FILE,pap) + iOffset * sizeof(PAY_AT_PUMP_INFO) - sizeof(PUMP_INFO) * MAX_PUMPS ,
					(WORD)(sizeof(tmpPap)), &tmpPap , HOP_DIRECT_LOCAL,0,0);	//4.0.20.503

		if(lRtc)
		{
			sMsg.Format("Read PAP info returned %d" , lRtc);
			_LOGMSG.LogMsg(sMsg);
		}

		//Write it back to the file at the proper place.
		lRtc = Write(offsetof(PUMP_FILE,pap) + iOffset * sizeof(PAY_AT_PUMP_INFO),
					 (WORD)sizeof(tmpPap),
					 &tmpPap , HOP_DEFAULT_CONFIG,0,0);

		if(lRtc)
		{
			sMsg.Format("Write PAP info returned %d" , lRtc);
			_LOGMSG.LogMsg(sMsg);
		}
	}

	//Insert default pump statistics in Q-Dex for new pumps
	for( long p = 33; p <= MAX_PUMPS_64; p++)
	{
		_Module.m_server.ResetPumpStatQdxData(p , RESET_PUMP_INFO);						
	}

	m_csFile.Unlock();

	
	SetFileSignature();

	_LOGMSG.LogMsg("Convert to 64 pumps" , DEFAULT_LOG_LEVEL);
	
	return 0;
}

//4.0.3.27
/******************************************************************************
 Description:	Save OLA data 	
 Returns:      	Qdx return code 
 Parameters:   	lPumpNumber = Pump number
				pData = OLA buffer, 
				lSize = buffer length


 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			18/5/2003   16:24		Start
*****************************************************************************/
long CAsh2000File::InsertData(long lPumpNumber, void *pData, long lSize , CPumpTransact  *pTrs)
{
	CPumpTransact	cTrs; 
	ASH2000_REC		cASH2000_REC;
	BOOL			bFound = FALSE;	
	CString str;
	long lRtc = 0 ;	

	if (pTrs)
	{
		cTrs = *pTrs;
		bFound =TRUE;	
	}
	else
	{
		cTrs.SetLink(TRS_LINK_OLA_NUMBER, _Module.m_server.m_cPumpArray[lPumpNumber -1 ].GetPAPSeqNumber());					

		if(!_Module.m_server.m_cPumpArray[lPumpNumber -1 ].GetTrsByLinkNumber(&cTrs))					
		{
			bFound =TRUE;	
		}
	}

	if (bFound)
	{
		memset(&cASH2000_REC , 0 , sizeof(ASH2000_REC));
			
		cASH2000_REC.lIndexNumber = cTrs.m_lNumber;
// 		if (Operation(REC_READ, &cASH2000_REC))
// 		{
// 			memset(cASH2000_REC.sData , ' ' , sizeof(cASH2000_REC.sData));
// 			memcpy(cASH2000_REC.sData,(char *)pData , min(lSize ,sizeof(cASH2000_REC.sData)) );		
// 			lRtc =Operation(REC_INSERT, &cASH2000_REC);
// 		}
// 		else
// 		{
// 			cASH2000_REC.lIndexNumber = cTrs.m_lNumber;
// 			memset(cASH2000_REC.sData , ' ' , sizeof(cASH2000_REC.sData));
// 			memcpy(cASH2000_REC.sData,(char *)pData , min(lSize ,sizeof(cASH2000_REC.sData)) );				
// 
// 			lRtc =Operation(REC_UPDATE, &cASH2000_REC);
// 		}

		if(LOG_DETAIL_FLOW_CONTROL)
		{
			str.Format("CAsh2000File - InsertData , Index=%ld lRtc=%ld" , cTrs.m_lNumber , lRtc);
			_LOGMSG.LogMsg(str);
		}


	}
	else
	{
		if(LOG_DETAIL_FLOW_CONTROL)
		{
			str.Format("CAsh2000File - InsertData Not found Index number");
			_LOGMSG.LogMsg(str);
		}


	}

	return lRtc;
}

CAsh2000File::CAsh2000File():CIndexFile(ASH2000_FILE,ASH2000_FILE_SIZE)
{

}




/******************************************************************************
 Description:	Check file signuature if is not ok,  
 Returns:      	lValid - validation check status.
 Parameters:   	None 
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/5/2003   19:21		Start
******************************************************************************/
long CPumpFile::VerifyFileSignatureAndData()
{
	BYTE		sFileValid[FILE_VALIDATION_LEN+1];
	long		lRetCode = 0;
	
	memset(sFileValid,0,sizeof(sFileValid));

	m_csFile.Lock();
	long lRtc = Read(0,FILE_VALIDATION_LEN,sFileValid,HOP_DIRECT_LOCAL ,0 ,0);	
	m_csFile.Unlock();
	
	if(lRtc == OK)
	{
		//   file signature ok. 
		if (!memcmp(sFileValid,PUMPSRV_PUMPSTAT_CURRENT_VERSION,FILE_VALIDATION_LEN))
		{// valid 
			lRetCode = 0;			
		}
		else
		{
			// signature not found in file. 
			SetFileSignature();
			SaveAllData();

			if(LOG_DETAIL_FLOW_CONTROL)
			{
				CString str;
				str.Format("File Signature not found force signauture and save server data !!!!!!");
				_LOGMSG.LogMsg(str);
			}
		}
	}

	return lRetCode;
}

void CPumpFile::ExpendedSessTypeAndPumps()
{
	long	lRtc=0;	

	PUMP_INFO pumpInfo;
	SHORT_OLA_STATE_PUMP_INFO cShorOlaStatpumpInfo;
	memset(&pumpInfo,0,sizeof(PUMP_INFO));

	m_csFile.Lock();

	for (long i = MAX_PUMPS-1 ; i >= 0  ; i-- )
	{
		
		lRtc = Read(offsetof(PUMP_FILE,p[i]),
					sizeof(SHORT_OLA_STATE_PUMP_INFO),
					&cShorOlaStatpumpInfo,HOP_DIRECT_LOCAL,0,0);


		pumpInfo.m_lNumber = cShorOlaStatpumpInfo.m_lNumber;
		memcpy(pumpInfo.m_sPumpMenufacture, cShorOlaStatpumpInfo.m_sPumpMenufacture  , sizeof(pumpInfo.m_sPumpMenufacture));
		pumpInfo.m_lType =  cShorOlaStatpumpInfo.m_lType;					
		memcpy(&pumpInfo.m_cTrs ,cShorOlaStatpumpInfo.m_cTrs, sizeof(pumpInfo.m_cTrs));
		memset(&pumpInfo.m_cOLA	, 0, sizeof(pumpInfo.m_cOLA));
	
		pumpInfo.m_cPumpStat = cShorOlaStatpumpInfo.m_cPumpStat;
		pumpInfo.m_lLockStatus = cShorOlaStatpumpInfo.m_lLockStatus;
		pumpInfo.m_lShiftNumber = cShorOlaStatpumpInfo.m_lShiftNumber;
		pumpInfo.m_cLastTimeUpdate = cShorOlaStatpumpInfo.m_cLastTimeUpdate;
 
		lRtc = Write(offsetof(PUMP_FILE,p[i]),
					sizeof(PUMP_INFO),
					&pumpInfo,HOP_DIRECT_LOCAL,0,0);
	}


	m_csFile.Unlock();

	
	SetFileSignature();

	_LOGMSG.LogMsg("Expended session Type  !!!!." , DEFAULT_LOG_LEVEL);
}