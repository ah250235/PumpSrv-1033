// DB.cpp: implementation of the CRelativeFile class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DB.h"
//#include "Convert.h"
#include <fstream>
#include <iostream>
#include "windows.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CQdexFile Class
//////////////////////////////////////////////////////////////////////

CQdexFile::CQdexFile():
m_enumAlarmStatus(QDEX_ALARM_OFF),
m_sFileName(NULL), //4.0.18.20
m_lSeqNumber(0),    //4.0.18.40
m_bInMiddleOfQDXMaintFlag(0) //4.0.20.507
{
}

CQdexFile::~CQdexFile()
{
	if (m_sFileName)
		delete[] m_sFileName;
}

void CQdexFile::SetFileNum(unsigned char byFileNum)
{
	nFileNum = GetQdexFileNumber(byFileNum);
	q_param.indx_num = nFileNum;	
}

unsigned char CQdexFile::GetQdexFileNumber(unsigned char byFileNum )
{
	DWORD dwVal = (DWORD)byFileNum;	
	CString strFileName; 
	BOOL bExit = FALSE;
//	long bDoReloadDefault;
	CString sDatabasePath;

	switch(byFileNum)
	{
		case PUMP_PRE_PAY_FILE:
			strFileName = "PrePay";
			m_lDefaultDoReload = 1;
			break;
		case PUMP_OLA_FILE:
			strFileName = "OLA";
			m_lDefaultDoReload = 1;
			break;
		case PUMP_STAT_FILE:
			strFileName = "PumpStat";
			m_lDefaultDoReload = 0;
			break;
		case PUMP_TOTALS_FILE:
			strFileName = "PumpTotals";
			m_lDefaultDoReload = 1;
			break;
		case TANK_READS_FILE:
			strFileName = "TankRead";
			m_lDefaultDoReload = 1;
			break;
		case FUEL_DELIVERIES_FILE:
			strFileName = "Delivery";
			m_lDefaultDoReload = 1;
			break;
		case ALARM_FILE:
			strFileName = "Alarm";
			m_lDefaultDoReload = 0;
			break;
		case CAR_WASH_FILE:
			strFileName = "CarWash";
			m_lDefaultDoReload = 1;
			break;
		case RECEIPT_FILE:
			strFileName = "Receipt";
			m_lDefaultDoReload = 1;
			break;
		// start 3.1.1.6
		case RFS_FILE:
			strFileName = "RFS";
		// end 3.1.1.6
			m_lDefaultDoReload = 0;
			break;
		case EXTRA_INFO_FILE:
			strFileName = "ExtraInfo";
			m_lDefaultDoReload = 1;
			break;
		//5.0.0.55
		case ITEMS_FILE:
			strFileName = "Items";
			m_lDefaultDoReload = 0;
			break;
		//5.0.0.55			
			//4.0.1.38
		case SHIFT_TANKS_FILE:
			strFileName = "ShiftTankTotals";
			m_lDefaultDoReload = 0;
			break;

		case SHIFT_PUMP_FILE :
			strFileName = "ShiftPumpTotals";
			m_lDefaultDoReload = 0;
			break;

		case SHIFT_DELIVERY_FILE:
			strFileName = "ShiftDeliveryTotals";
			m_lDefaultDoReload = 0;
			break;
			//4.0.1.38
		case ASH2000_FILE:    //4.0.3.47
			strFileName = "Ash2000";
			m_lDefaultDoReload = 0;
			break;
		case DB_TANK_GAUGE_FILE:    //4.0.5.24
			strFileName = "DBTankGauge";
			m_lDefaultDoReload = 0;
			break;

		case ACCUMULATION_SHIFT_TOTAL_FILE: //4.0.5.27
			strFileName = "AccumulationShiftTotal";
			m_lDefaultDoReload = 0;
			break;			
		case RECEIPT_EXTRA_DATA_FILE: //4.0.5.27
			strFileName = "ReceiptExtraData";
			m_lDefaultDoReload = 1;
			break;			
		case RECEIPT_EXTRA_INDEX_FILE:
			strFileName = "ReceiptExtraIndex";
			m_lDefaultDoReload = 1;
			break;			
		case PUMP_OLA_TEMP_FILE:
			strFileName = "OLATemporary";
			m_lDefaultDoReload = 0;
			break;
		case CASH_ACCEPTOR_FILE:
			strFileName = "CashAcceptorFile";
			m_lDefaultDoReload = 0;
			break;
		case PAID_TRS_FILE:
			strFileName = "PaidTrs";
			m_lDefaultDoReload = 0;
			break;
		case SINGLE_SHIFT_ACCUMULATIONS_FILE:    //4.0.25.50 - TD 108311
			strFileName = "SingleShiftAccumulations";
			m_lDefaultDoReload = 0;
			break;
		default :
			bExit = TRUE;
			break;

	}
		
	SetFileName(strFileName.GetBuffer(0));

	if(!bExit)
	{
		DWORD dwFlags = 0, dwDoReload = 0;

		//4.0.0.81
		_Module.m_server.m_cParam.GetParam("Database",
									0,0,
									(char *)((LPCTSTR)strFileName),
									0,
									NULL,0,
									&dwVal,
									TRUE);
		

		_Module.m_server.m_cParam.GetParam("Database",
									byFileNum,0,
									"UseBackUp",
									0,
									NULL,0,
									&dwFlags,
									TRUE);
		//4.0.18.0 - Get the DoReload key from the registry
		sDatabasePath.Format("Database\\%s",strFileName);
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sDatabasePath), "DoReload",&dwDoReload, (DWORD)m_lDefaultDoReload, TRUE);
				
		
		 m_lBackUpFlags =(long)dwFlags;
		 m_lDoReload = (long)dwDoReload;
	}
	return((unsigned char)dwVal);

}
//4.0.0.66 start
/******************************************************************************
 Description:	Sets the backup working flags.
 Returns:      	void
 Parameters:   	long *plFlags - In/Out parameter.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Yaron			2/7/2002   13:38		Start
******************************************************************************/
void CQdexFile::SetBackUpFlags(long *plFlags)
{
	long lFlags = HOP_DIRECT_LOCAL;
	
	if( (m_lBackUpFlags & *plFlags) || (m_lBackUpFlags & MS_DEFAULT_CONFIG) )
	{
		//reseting PumpSrv backup flags.
		*plFlags &= MS_NOT_DEFAULT_CONFIG;
		*plFlags &= MS_NOT_SHIFT_INFO_DEFAULT_CONFIG;
		*plFlags &= MS_NOT_SERVER_INFO_DEFAULT_CONFIG;
		*plFlags &= MS_NOT_PUMP_STATE_INFO_DEFAULT_CONFIG;
		
		lFlags = HOP_DEFAULT_CONFIG;
	}
	else
		lFlags = HOP_DIRECT_LOCAL;
	
	*plFlags |= lFlags;

}
//4.0.0.66 end

//4.0.2.15 Start
BOOL CQdexFile::CheckPointerValidity(void * Ptr)
{
	BOOL bRetVal = TRUE;

	if(!Ptr)
	{
		_LOGMSG.LogMsg("CQDexFile::CheckPointerValidity() - null pointer");
		bRetVal = FALSE;
	}

	return bRetVal;
}
//4.0.2.15 End


//////////////////////////////////////////////////////////////////////
// CRelativeFile Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRelativeFile::CRelativeFile()
{
	memset(&q_param,0,sizeof(struct q_parm_));	
	q_param.option = 1; // seek before r/w
}

CRelativeFile::CRelativeFile(unsigned char byFileNum)
{
	memset(&q_param,0,sizeof(struct q_parm_));
	nFileNum = GetQdexFileNumber(byFileNum);
	q_param.option   = 1; // seek before r/w
	q_param.indx_num = nFileNum;
}

CRelativeFile::~CRelativeFile()
{
																													
}


/******************************************************************************
 Description:	Write Record to qdx Relative file 	
 Returns:      	Qdx return code
 Parameters:   	DWORD lWritePartOffset,WORD wWritePartLength,void *pBuff ,long lFlags,long lPCNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2001   14:41		Start
 AmitH			29/06/2008   14:00		Continue - replace Hydra call to RelWrite with RelWrite2 //4.0.20.56
******************************************************************************/

long CRelativeFile::Write(DWORD lWritePartOffset,WORD wWritePartLength,void *pBuff ,long lFlags,long lPCNumber , long lTimeStampOffset,long lTimeDelta,long lGroupNum,long lClientEventData)
{ 
	long lRetVal = 0;
	BOOL bWorkLocal = TRUE;
	CString str1;

	CString sMsg;
	
	if(q_param.indx_num != nFileNum)
	{
		if(LOG_ERROR)
		{
			CString str;
			str.Format("CRelativeFile::Write, (Before) Qdex file number changed (indx_num %d, nFileNum %d)",q_param.indx_num,nFileNum);
			_LOGMSG.LogMsg(str,LOG_LEVEL_0);
		}
		q_param.indx_num = nFileNum;
	}

	q_param.option   = 1; // seek before r/w
    q_param.write_part_length = wWritePartLength;
    q_param.hi_word_write_part_offset = HIWORD(lWritePartOffset);
    q_param.write_part_offset = LOWORD(lWritePartOffset);

	//setting the flags
	SetBackUpFlags(&lFlags);
	try  
	{    
		if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pRTDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
		{
			/********************************/
			//Convert from Buffer to Variant
			/********************************/
			
			_variant_t variantBuff;
			int nError = 0;
			BOOL	bRetVal = FALSE;
				
			bRetVal = PutBinaryIntoVariant(&variantBuff, (BYTE *)pBuff, wWritePartLength, &nError);
		
			if (!bRetVal)
			{
				char sStr[MAX_FIELD_VAL];
				sprintf(sStr,"CRT::Write - Failed in PutBinaryIntoVariant nError=%d" , nError);	
				_LOGMSG.LogMsg(sStr, LOG_LEVEL_0);
			}
			else
			{	
				//Remove 4.0.21.0
				/*if(LOG_FCC_FLOW_CONTROL)
				{
					CString sMsg1;
					sMsg1.Format("CRelativeFile::Write - Before Write WritePartLength= %ld buffer is:",wWritePartLength);
					
					for(int i=0; i < wWritePartLength; i++)
					{
						char t = ((char)(*(((BYTE*)pBuff)+i)));
						sMsg1 += t==0 ? '0':t;
					}
					
					_LOGMSG.LogMsg(sMsg1);
					
				}*/
										
				try
				{				
					
					if(LOG_FCC_FLOW_CONTROL) 
					{
						CString sMsg;
						sMsg.Format("CRelativeFile::Write HYDRA, %ld ,%ld , %ld",lWritePartOffset,wWritePartLength,lFlags);
						_LOGMSG.LogMsg(sMsg);
					}
					

					if (_HOST.m_pRTDB!= NULL) 
					{
						lRetVal = _HOST.m_pRTDB->RelWrite2(nFileNum,lWritePartOffset, wWritePartLength,variantBuff,lFlags,lPCNumber , 0,lTimeDelta,lGroupNum,lClientEventData); //4.0.15.460
					}
					else
						_LOGMSG.LogMsg("CRelativeFile::Write ,_HOST.m_pRTDB is NULL"); 
				
					if(!lRetVal)
						bWorkLocal = FALSE;
					else
					{
						CString str;
						str.Format("CRelativeFile::Write lRetVal = %ld", lRetVal);
						_LOGMSG.LogMsg(str,LOG_LEVEL_1); 
					}
					
				}
				catch(_com_error & e )
				{
					//4.0.15.420
					str1.Format("CRelativeFile::Write ,catch _com_error m_pRTDB = %ld", _HOST.m_pRTDB);
					_LOGMSG.LogMsg(str1); 

					_LOGMSG.LogResult(
						0, // Src
						"IRTDB",
						"RelWrite",
						FALSE,0,
						TRUE,e.Error(),e);
					//trying to restore the com
					bWorkLocal = TRUE;
					_HOST.TryToRestoreHydConn(e.Error());
				}	
				catch(...)
				{
					str1.Format("CRelativeFile::Write ,catch ... m_pRTDB = %ld", _HOST.m_pRTDB);
					_LOGMSG.LogMsg(str1); 

					if(LOG_LOGIC_ERROR)
						_LOGMSG.LogResult(
							0, // Src
							"IRTDB",
							"RelWrite",
							TRUE,0,
							(lRetVal!=0)? TRUE:FALSE,lRetVal);

					//trying to restore the com
					bWorkLocal = TRUE;
					_HOST.TryToRestoreHydConn();
				}			
			}
		}
	}
	catch (...)	
	{
		_LOGMSG.LogMsg("CRelativeFile::Write ,main catch ...");

	}

	if(bWorkLocal)
	{		
		lRetVal = (long)q_swrite(&q_param,pBuff);

		long j = q_param.write_part_offset;

		if ((j <= 30)  && (nFileNum == 3))   //4.0.7.60		
		{
			if(LOG_ERROR)
			{ 
				CString str;
				str.Format("CRelativeFile::signature , (After) Qdex file number changed (indx_num %d, nFileNum %d), q_param.write_part_offset=%d, size=%d , lRetVal=%ld",q_param.indx_num,nFileNum , q_param.write_part_offset , wWritePartLength , lRetVal);
				_LOGMSG.LogMsg(str,LOG_LEVEL_0);
			}

		}
	
		if(LOG_FCC_FLOW_CONTROL) 
		{
			CString sMsg;
			sMsg.Format("CRelativeFile::Write LOCAL, %ld ,%ld , %ld",lWritePartOffset,wWritePartLength,lFlags);
			_LOGMSG.LogMsg(sMsg);
		}
	
	}
    
	if(q_param.indx_num != nFileNum)
	{
		if(LOG_ERROR)
		{
			CString str;
			str.Format("CRelativeFile::Write, (After) Qdex file number changed (indx_num %d, nFileNum %d)",q_param.indx_num,nFileNum);
			_LOGMSG.LogMsg(str,LOG_LEVEL_0);
		}
		q_param.indx_num = nFileNum;
	}

	//4.0.5.24
	SetAlarm(lRetVal, (long)nFileNum, TRUE);

	return lRetVal;
}

/******************************************************************************
 Description:	Read record form relative file 	
 Returns:      	Qdx return code
 Parameters:   	DWORD lReadPartOffset, WORD wReadPartLength, void *pBuff ,long lFlags, long lPCNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2001   14:44		Start
******************************************************************************/
long CRelativeFile::Read(DWORD lReadPartOffset, WORD wReadPartLength, void *pBuff ,long lFlags, long lPCNumber, long lTimeStampOffset,long lTimeDelta,long lGroupNum)
{  
	long lRetVal = 0;
	BOOL bWorkLocal = TRUE;
	CString sMsg;

	if(q_param.indx_num != nFileNum)
	{
		if(LOG_ERROR)
		{
			CString str;
			str.Format("CRelativeFile::Read, (Before) Qdex file number changed (indx_num %d, nFileNum %d)",q_param.indx_num,nFileNum);
			_LOGMSG.LogMsg(str,LOG_LEVEL_0);
		}

		q_param.indx_num = nFileNum;
	}

	q_param.option   = 1; // seek before r/w
    q_param.write_part_length = wReadPartLength;
    q_param.hi_word_write_part_offset = HIWORD(lReadPartOffset);
    q_param.write_part_offset = LOWORD(lReadPartOffset);

	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pRTDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
	{
		BSTR bstrRec = NULL;
		bstrRec = SysAllocStringLen((OLECHAR*)pBuff,wReadPartLength);
	
		bWorkLocal = FALSE;

		try
		{
			//4.0.2.15 start
			if(CheckPointerValidity(bstrRec))
			{
				lRetVal = _HOST.m_pRTDB->RelRead(nFileNum,lReadPartOffset, wReadPartLength , &bstrRec, lFlags,lPCNumber, lTimeStampOffset,lTimeDelta,lGroupNum,0);	
				memcpy((char *)pBuff , bstrRec , wReadPartLength);
			}

			else
				bWorkLocal = FALSE;
			//4.0.2.15 end
		}
		catch(_com_error & e )
		{
			_LOGMSG.LogResult(
				0, // Src
				"IRTDB",
				"RelRead",
				FALSE,0,
				TRUE,e.Error(),e);
			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();
		}
		catch(...)
		{
			if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"IRTDB",
					"RelRead",
					TRUE,0,
					(lRetVal!=0)? TRUE:FALSE,lRetVal);

			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();			
		}
		SysFreeString(bstrRec);

	}
	if(bWorkLocal)
	{

		lRetVal = (long)q_sread(&q_param,pBuff);   
	}  

	if(q_param.indx_num != nFileNum)
	{
		if(LOG_ERROR)
		{
			CString str;
			str.Format("CRelativeFile::Read, (After) Qdex file number changed (indx_num %d, nFileNum %d)",q_param.indx_num,nFileNum);
			_LOGMSG.LogMsg(str,LOG_LEVEL_0);
		}
		q_param.indx_num = nFileNum;
	}


	//4.0.5.24
	SetAlarm(lRetVal, (long)nFileNum, TRUE);


	return lRetVal;	
}


//void CRelativeFile::SetFileNum(unsigned char byFileNum)
//{
//	nFileNum = GetQdexFileNumber(byFileNum);
//	q_param.indx_num = nFileNum;
//	 
//}

long CRelativeFile::Flush()
{

	unsigned char	buff[100];

	memset(&q_param,0,sizeof(struct q_parm_));
	memset(buff,0,sizeof(buff));
	q_param.indx_num = nFileNum;

	return ((long)q_flush(&q_param,buff));	
}


//////////////////////////////////////////////////////////////////////
// CIndexFile Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIndexFile::CIndexFile()
{
	memset(&q_param,0,sizeof(struct q_parm_));
	q_param.option	= 0x10;
	enumFileType = INDEX_FILE_TYPE;
}

/******************************************************************************
 Description:	Initial index file parmeter  	
 Returns:      	None
 Parameters:   	unsigned char byFileNum ,long lRecSize
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2001   14:52		Start
******************************************************************************/
CIndexFile::CIndexFile(unsigned char byFileNum,long lRecSize)
{
	memset(&q_param,0,sizeof(struct q_parm_));
	nFileNum = GetQdexFileNumber(byFileNum);
	q_param.indx_num = nFileNum;	 
	q_param.option	= 0x10; // multi user operation
	enumFileType = INDEX_FILE_TYPE;
	m_lRecSize = lRecSize;
}

CIndexFile::~CIndexFile()
{

}

//void CIndexFile::SetFileNum(unsigned char byFileNum)
//{
//	q_param.indx_num = byFileNum;	
//	nFileNum = byFileNum;
//}

void CIndexFile::ClearQParam()
{
	q_param.reserved = 0;
	memset (&(q_param.hi_word_write_part_offset), 0, sizeof( q_param) - 4);

	if(q_param.indx_num != nFileNum)
	{
		if(LOG_ERROR)
		{
			CString str;
			str.Format("CIndexFile::ClearQParam, Qdex file number changed (indx_num %d, nFileNum %d)",q_param.indx_num,nFileNum);
			_LOGMSG.LogMsg(str,LOG_LEVEL_0);
		}
		q_param.indx_num = nFileNum;
	}
	q_param.option	= 0x10; // multi user operation
}

/******************************************************************************
 Description:	Read record from Index Qdx file. 	
 Returns:      	Qdx return code.
 Parameters:   	void *pBuff, long lFlags, long lPCNumber.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2001   14:55		Start
******************************************************************************/
long CIndexFile::Read(void *pBuff, long lFlags, long lPCNumber , long lTimeStampOffset,long lTimeDelta,long lGroupNum)
{
	long lRetVal = 0 ;
	BOOL bWorkLocal = TRUE;

	ClearQParam();

	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pRTDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
	{
		BSTR bstrRec = NULL;
		bstrRec = SysAllocStringLen((OLECHAR*)pBuff,m_lRecSize);		

		bWorkLocal = FALSE;

		try
		{
			//4.0.2.15 start
			if(CheckPointerValidity(bstrRec))
			{
				lRetVal =_HOST.m_pRTDB->Read(nFileNum,&bstrRec, m_lRecSize,lFlags,lPCNumber, lTimeStampOffset,lTimeDelta,lGroupNum,0);		
				memcpy((char *)pBuff , bstrRec , m_lRecSize);
			}
			
			else
				bWorkLocal = TRUE;
			//4.0.2.15 end
		}
		catch(_com_error & e )
		{
			_LOGMSG.LogResult(
				0, // Src
				"IRTDB",
				"Read",
				FALSE,0,
				TRUE,e.Error(),e);
			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();
		}
		catch(...)
		{
			if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"IRTDB",
					"Read",
					TRUE,0,
					(lRetVal!=0)? TRUE:FALSE,lRetVal);

			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();			
		}
		SysFreeString(bstrRec);
	}
	if(bWorkLocal)
	{	
		lRetVal = (long)q_read( &q_param, pBuff );
	}

	return (lRetVal);
}

/******************************************************************************
 Description:	Read next record from Qdx file. 	
 Returns:      	Qdx return Code.
 Parameters:   	void *pBuff, long lFlags, long lPCNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2001   14:57		Start
******************************************************************************/
long CIndexFile::ReadNext(void *pBuff, long lFlags, long lPCNumber, long lTimeStampOffset,long lTimeDelta,long lGroupNum)
{
	long lRetVal = 0 ;
	BOOL bWorkLocal = TRUE;

	ClearQParam();

	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pRTDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
	{
		BSTR bstrRec = NULL;
		bstrRec = SysAllocStringLen((OLECHAR*)pBuff,m_lRecSize);		

		bWorkLocal = FALSE;

		try
		{
			//4.0.2.15 start
			if(CheckPointerValidity(bstrRec))
			{
				lRetVal =_HOST.m_pRTDB->ReadNext(nFileNum,&bstrRec, m_lRecSize,lFlags,lPCNumber,lTimeStampOffset,lTimeDelta,lGroupNum,0);		
				memcpy((char *)pBuff , bstrRec , m_lRecSize);
			}
			
			else
				bWorkLocal = TRUE;
			//4.0.2.15 start
		}
		catch(_com_error & e )
		{
			
			_LOGMSG.LogResult(
				0, // Src
				"IRTDB",
				"ReadNext",
				FALSE,0,
				TRUE,e.Error(),e);//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();
		}	
		catch(...)
		{
			if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"IRTDB",
					"ReadNext",
					TRUE,0,
					(lRetVal!=0)? TRUE:FALSE,lRetVal);

			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();			
		}
		
		SysFreeString(bstrRec);
	}
	if(bWorkLocal)
	{	
		lRetVal = (long)q_readn( &q_param, pBuff );
	}

	return  (lRetVal);

}

/******************************************************************************
 Description:	Read the previous record form Qdx Index file. 	
 Returns:      	Qdx return code.
 Parameters:   	void *pBuff, long lFlags, long lPCNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2001   14:58		Start
******************************************************************************/
long CIndexFile::ReadPrev(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffset,long lTimeDelta,long lGroupNum)
{
	long lRetVal = 0 ;
	BOOL bWorkLocal = TRUE;

	ClearQParam();

	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pRTDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
	{
		BSTR bstrRec = NULL;
		bstrRec = SysAllocStringLen((OLECHAR*)pBuff,m_lRecSize);		

		bWorkLocal = FALSE;

		try
		{
			//4.0.2.15 start
			if(CheckPointerValidity(bstrRec))
			{
				lRetVal =_HOST.m_pRTDB->ReadPrev(nFileNum,&bstrRec, m_lRecSize,lFlags,lPCNumber,lTimeStampOffset,lTimeDelta,lGroupNum,0);		
				memcpy((char *)pBuff , bstrRec , m_lRecSize);
			}

			else 
				bWorkLocal = TRUE;
			//4.0.2.15 end
		}
		catch(_com_error & e )
		{
			_LOGMSG.LogResult(
				0, // Src
				"IRTDB",
				"ReadPrev",
				FALSE,0,
				TRUE,e.Error(),e);
			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();
		}				
		catch(...)
		{
			if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"IRTDB",
					"ReadPrev",
					TRUE,0,
					(lRetVal!=0)? TRUE:FALSE,lRetVal);

			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();			
		}

		SysFreeString(bstrRec);
	}
	if(bWorkLocal)
	{	
		lRetVal =(long)q_readp( &q_param, pBuff );
	}

	return (lRetVal);
}

/******************************************************************************
 Description:	Find the first record in Index Qdx file 	
 Returns:      	Qdx return code
 Parameters:   	void *pBuff, long lFlags, long lPCNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2001   14:59		Start
******************************************************************************/
long CIndexFile::ReadFirst(void *pBuff, long lFlags, long lPCNumber , long lTimeStampOffset,long lTimeDelta,long lGroupNum)
{
	long lRetVal = 0 ;
	BOOL bWorkLocal = TRUE;

	ClearQParam();
	
	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pRTDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
	{
		BSTR bstrRec = NULL;
		bstrRec = SysAllocStringLen((OLECHAR*)pBuff,m_lRecSize);		

		bWorkLocal = FALSE;

		try
		{
			//4.0.2.15 start
			if(CheckPointerValidity(bstrRec))
			{
				lRetVal =_HOST.m_pRTDB->ReadFirst(nFileNum,&bstrRec, m_lRecSize,lFlags,lPCNumber,lTimeStampOffset,lTimeDelta,lGroupNum,0);		
				memcpy((char *)pBuff , bstrRec , m_lRecSize);
			}

			else
				bWorkLocal = TRUE;
			//4.0.2.15 end
		}
		catch(_com_error & e )
		{
			_LOGMSG.LogResult(
				0, // Src
				"IRTDB",
				"ReadFirst",
				FALSE,0,
				TRUE,e.Error(),e);
			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();
		}				
		catch(...)
		{
			if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"IRTDB",
					"ReadFirst",
					TRUE,0,
					(lRetVal!=0)? TRUE:FALSE,lRetVal);

			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();			
		}

		SysFreeString(bstrRec);
	}
	if(bWorkLocal)
	{	
		lRetVal = (long)q_start( &q_param, pBuff );
	}

	//4.0.5.24
	if(lRetVal == ERR_NOT_FOUND)
	{
		/*Error not find is when read succeed but the file is empty.
		  we treat it loke OK*/
		SetAlarm(OK, (long)nFileNum, FALSE);		
	}
	else
		SetAlarm(lRetVal, (long)nFileNum, FALSE);


	return (lRetVal);
}	

/******************************************************************************
 Description:	Find the lateness record in Qdx file 	
 Returns:      	Qdx return code
 Parameters:   	void *pBuff, long lFlags, long lPCNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2001   15:02		Start
******************************************************************************/
long CIndexFile::ReadLast(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffset,long lTimeDelta,long lGroupNum)
{
	long lRetVal = 0 ;
	BOOL bWorkLocal = TRUE;
	ClearQParam();
	
	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pRTDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
	{
		BSTR bstrRec = NULL;
		bstrRec = SysAllocStringLen((OLECHAR*)pBuff,m_lRecSize);		

		bWorkLocal = FALSE;

		try
		{
			//4.0.2.15 start
			if(CheckPointerValidity(bstrRec))
			{
				lRetVal =_HOST.m_pRTDB->ReadLast(nFileNum,&bstrRec, m_lRecSize,lFlags,lPCNumber,lTimeStampOffset,lTimeDelta,lGroupNum,0);		
				memcpy((char *)pBuff , bstrRec , m_lRecSize);
			}

			else
				bWorkLocal = TRUE;
			//4.0.2.15 end
		}
		catch(_com_error & e )
		{
			_LOGMSG.LogResult(
				0, // Src
				"IRTDB",
				"ReadLast",
				FALSE,0,
				TRUE,e.Error(),e);
			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();
		}
		catch(...)
		{
			if(LOG_LOGIC_ERROR)
				_LOGMSG.LogResult(
					0, // Src
					"IRTDB",
					"ReadLast",
					TRUE,0,
					(lRetVal!=0)? TRUE:FALSE,lRetVal);

			//trying to restore the com
			bWorkLocal = TRUE;
			_HOST.TryToRestoreHydConn();			
		}
		
		SysFreeString(bstrRec);
	}
	if(bWorkLocal)
	{	
		lRetVal = (long)q_read_last( &q_param, pBuff );
	}

	return (lRetVal);
}

/******************************************************************************
 Description:	Insert record to index Qdx file 	
 Returns:      	Qdx return code
 Parameters:   	void *pBuff, long lFlags, long lPCNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2001   15:02		Start
 AmitH			29/06/2008   14:00		Continue - replace Hydra call to Insert with Insert2 //4.0.20.56
******************************************************************************/

long CIndexFile::Insert(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffset,long lTimeDelta,long lGroupNum)

{
	long lRetVal = 0;
	BOOL bWorkLocal = TRUE;
	CString str;

	//setting the flags
	SetBackUpFlags(&lFlags);

	try	
	{
		if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pRTDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
		{
			/********************************/
			//Convert from Buffer to Variant
			/********************************/
			_variant_t variantBuff;
			int nError = 0;
			BOOL	bRetVal = FALSE;
				
			bRetVal = PutBinaryIntoVariant( &variantBuff, (BYTE *)pBuff, m_lRecSize, &nError);
						
			if (!bRetVal)
			{
				char sStr[MAX_FIELD_VAL];
				sprintf(sStr,"CRT::Insert - Failed in PutBinaryIntoVariant nError=%d" , nError);	
				_LOGMSG.LogMsg(sStr, LOG_LEVEL_1);
			}
			else
			{	
				//Remove 4.0.21.0
				/*if(LOG_FCC_FLOW_CONTROL)
				{
					CString sMsg1;
					sMsg1.Format("CIndexFile::Insert - Before Insert PartLength= %ld buffer is:",m_lRecSize);
					
					for(int i=0; i < m_lRecSize; i++)
					{
						char t = ((char)(*(((BYTE*)pBuff)+i)));
						sMsg1 += t==0 ? '0':t;
					}
					
					_LOGMSG.LogMsg(sMsg1);
					
				}*/
									
				try
				{			
					if (_HOST.m_pRTDB!= NULL)
					{
						lRetVal = _HOST.m_pRTDB->Insert2(nFileNum, variantBuff, m_lRecSize,lFlags,lPCNumber,0,lTimeDelta,lGroupNum,0);
					}
					
					if(!lRetVal)
						bWorkLocal = FALSE;
					else
					{
						str.Format("CIndexFile::Insert lRetVal = %ld", lRetVal);
						_LOGMSG.LogMsg(str,LOG_LEVEL_1); 
					}
				}
				catch(_com_error & e )
				{
					str.Format("CIndexFile::Insert ,catch _com_error m_pRTDB = %ld", _HOST.m_pRTDB);
					_LOGMSG.LogMsg(str); 
					
					_LOGMSG.LogResult(
						0, // Src
						"IRTDB",
						"IndexInsert",
						FALSE,0,
						TRUE,e.Error(),e);
					//trying to restore the com
					bWorkLocal = TRUE;
					_HOST.TryToRestoreHydConn(e.Error());
				}
				catch(...)
				{
					//4.0.15.420
					str.Format("CIndexFile::Insert ,catch ... m_pRTDB = %ld", _HOST.m_pRTDB);
					_LOGMSG.LogMsg(str); 
					
					if(LOG_LOGIC_ERROR)
						_LOGMSG.LogResult(
							0, // Src
							"IRTDB",
							"IndexInsert",
							TRUE,0,
							(lRetVal!=0)? TRUE:FALSE,lRetVal);

					//trying to restore the com
					bWorkLocal = TRUE;
					_HOST.TryToRestoreHydConn();	
				}
			}
		}
	}
	catch (...)	
	{
		_LOGMSG.LogMsg("CIndexFile::Insert ,main catch ...",LOG_LEVEL_1);
	}

	if(bWorkLocal)
	{	
		ClearQParam();		
		lRetVal = (long)q_insert( &q_param, pBuff );
	}
	
	if (lRetVal) //4.0.6.501
	{
		CString str; //4.0.24.100
		str.Format("Failure - CIndexFile::Insert, indx_num %d, nFileNum %d, lFlags=%ld ,Hydra active=%ld,Retcode=%ld ",q_param.indx_num,nFileNum , lFlags , _HOST.m_lHydraActive, lRetVal);
		
		if (lRetVal == ERR_EXISTS)
		{
			_LOGMSG.LogMsg(str);  //Do not print ...Exception 1 in case of ERR_EXISTS... to log
		}
		else
		{
			_LOGMSG.LogMsg(str , LOG_ERROR);
		}
	}

	//4.0.5.24
	SetAlarm(lRetVal, (long)nFileNum, FALSE);
	
	return (lRetVal);
}

/******************************************************************************
 Description:	Update record in index Qdx file 	
 Returns:      	Qdx return code
 Parameters:   	void *pBuff, long lFlags, long lPCNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2001   15:03		Start
 AmitH			29/06/2008   14:00		Continue - replace Hydra call to Update with Update2 //4.0.20.56
******************************************************************************/

long CIndexFile::Update(void *pBuff, long lFlags, long lPCNumber,long lTimeStampOffset,long lTimeDelta,long lGroupNum)
{
	long lRetVal = 0;
	BOOL bWorkLocal = TRUE;
	ClearQParam();
	CString str1;

	//setting the flags
	SetBackUpFlags(&lFlags);

	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pRTDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
	{	
		/********************************/
		//Convert from Buffer to Variant
		/********************************/
		_variant_t variantBuff;
		int nError = 0;
		BOOL	bRetVal = FALSE;
				
		bRetVal = PutBinaryIntoVariant(&variantBuff, (BYTE *)pBuff, m_lRecSize, &nError);
		
		if (!bRetVal)
		{
			char sStr[MAX_FIELD_VAL];
			sprintf(sStr,"CRT::Update - Failed in PutBinaryIntoVariant nError=%d" , nError);	
			_LOGMSG.LogMsg(sStr, LOG_LEVEL_0);
		}
		else
		{	
			//Remove 4.0.21.0
			/*if(LOG_FCC_FLOW_CONTROL)
			{
				CString sMsg1;
				sMsg1.Format("CIndexFile::Update - Before Update PartLength= %ld buffer is:",m_lRecSize);
				
				for(int i=0; i < m_lRecSize; i++)
				{
					char t = ((char)(*(((BYTE*)pBuff)+i)));
					sMsg1 += t==0 ? '0':t;
				}
				
				_LOGMSG.LogMsg(sMsg1);
				
			}*/
	
			try
			{				
				if (_HOST.m_pRTDB!= NULL)
				{
					lRetVal = _HOST.m_pRTDB->Update2(nFileNum, variantBuff, m_lRecSize,lFlags,lPCNumber,0,lTimeDelta,lGroupNum,0);
				}
														
				if(!lRetVal)
					bWorkLocal = FALSE;
				else
				{
					CString str;
					str.Format("CIndexFile::Update lRetVal = %ld", lRetVal);
					_LOGMSG.LogMsg(str,LOG_LEVEL_1); 	
				}
			}
			catch(_com_error & e )
			{
				str1.Format("CIndexFile::Update ,catch _com_error m_pRTDB = %ld", _HOST.m_pRTDB); 
				_LOGMSG.LogMsg(str1); 
					
				_LOGMSG.LogResult(
					0, // Src
					"IRTDB",
					"Update",
					FALSE,0,
					TRUE,e.Error(),e);
				//trying to restore the com
				bWorkLocal = TRUE;
				_HOST.TryToRestoreHydConn(e.Error());
			}
			catch(...)
			{
				str1.Format("CIndexFile::Update ,catch ... m_pRTDB = %ld", _HOST.m_pRTDB); 
				_LOGMSG.LogMsg(str1); 

				if(LOG_LOGIC_ERROR)
					_LOGMSG.LogResult(
						0, // Src
						"IRTDB",
						"Update",
						TRUE,0,
						(lRetVal!=0)? TRUE:FALSE,lRetVal);

				//trying to restore the com
				bWorkLocal = TRUE;
				_HOST.TryToRestoreHydConn();			
			}		
		}
	}
	if(bWorkLocal)
	{	
		lRetVal = (long)q_write( &q_param, pBuff);
	}

	if (lRetVal)  //4.0.6.501
	{
		CString str;
		str.Format("CIndexFile::Update, indx_num %d, nFileNum %d, lFlags=%ld ,Hydra active=%ld,Retcode=%ld ",q_param.indx_num,nFileNum , lFlags , _HOST.m_lHydraActive, lRetVal);
		_LOGMSG.LogMsg(str , LOG_ERROR);
	}

	return (lRetVal);

}

/******************************************************************************
 Description:	Delete record from Index Qdx file 	
 Returns:      	Qdx return code
 Parameters:   	void *pBuff, long lFlags, long lPCNumber
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			20/11/2001   15:04		Start
 AmitH			29/06/2008   14:00		Continue - replace Hydra call to Delete with Delete2 //4.0.20.56
 AlexM          08/09/2011   16:00      TD 243954 -to prevent raise alarm in case that record not found in QDX on DELETE operation. 
******************************************************************************/
long CIndexFile::Delete(void *pBuff, long lFlags, long lPCNumber , long lTimeStampOffset,long lTimeDelta,long lGroupNum)
{
	long lRetVal = 0;
	BOOL bWorkLocal = TRUE;
	CString str1;

	//setting the flags
	SetBackUpFlags(&lFlags);

	if ( _HOST.m_lHydraActive && _HOST.CheckConnectionStatus() && _HOST.m_pRTDB!= NULL && !(lFlags & HOP_DIRECT_LOCAL) )
	{
		/********************************/
		//Convert from Buffer to Variant
		/********************************/
		_variant_t variantBuff; 
		int nError = 0;
		BOOL	bRetVal = FALSE;
			
		bRetVal = PutBinaryIntoVariant( &variantBuff, (BYTE *)pBuff, m_lRecSize, &nError);
					
		if (!bRetVal)
		{
			char sStr[MAX_FIELD_VAL];
			sprintf(sStr,"CRT::Delete - Failed in PutBinaryIntoVariant nError=%d" , nError);	
			_LOGMSG.LogMsg(sStr, LOG_LEVEL_0);
		}
		else
		{	
			//Remove 4.0.21.0
			/*if(LOG_FCC_FLOW_CONTROL)
			{
				CString sMsg1;
				sMsg1.Format("CIndexFile::Delete - Before Delete PartLength= %ld buffer is:",m_lRecSize);
				
				for(int i=0; i < m_lRecSize; i++)
				{
					char t = ((char)(*(((BYTE*)pBuff)+i)));
					sMsg1 += t==0 ? '0':t;
				}
				
				_LOGMSG.LogMsg(sMsg1);				
			}*/
							
			try
			{			
				if (_HOST.m_pRTDB!= NULL)
				{
					lRetVal = _HOST.m_pRTDB->Delete2(nFileNum, variantBuff, m_lRecSize,lFlags,lPCNumber,0,lTimeDelta,lGroupNum,0);
				}
				
				if(!lRetVal)
					bWorkLocal = FALSE;	
				else
				{	
					CString str;
					str.Format("CIndexFile::Delete lRetVal = %ld", lRetVal);
					_LOGMSG.LogMsg(str,LOG_LEVEL_1); 	
				}
				
			}
			catch(_com_error & e )
			{
				str1.Format("CIndexFile::Delete ,catch _com_error m_pRTDB = %ld", _HOST.m_pRTDB); 
				_LOGMSG.LogMsg(str1); 

				_LOGMSG.LogResult(
					0, // Src
					"IRTDB",
					"Delete",
					FALSE,0,
					TRUE,e.Error(),e);
				//trying to restore the com
				bWorkLocal = TRUE;
				_HOST.TryToRestoreHydConn(e.Error());
			}
			catch(...)
			{
				str1.Format("CIndexFile::Delete ,catch ... m_pRTDB = %ld", _HOST.m_pRTDB); 
				_LOGMSG.LogMsg(str1); 

				if(LOG_LOGIC_ERROR)
					_LOGMSG.LogResult(
						0, // Src
						"IRTDB",
						"Delete",
						TRUE,0,
						(lRetVal!=0)? TRUE:FALSE,lRetVal);

				//trying to restore the com
				bWorkLocal = TRUE;
				_HOST.TryToRestoreHydConn();			
			}		
		}
	}
	if(bWorkLocal)
	{	
		ClearQParam();
		lRetVal = (long)q_del( &q_param, pBuff );
	}


    // TD 243954 4.0.28.502
	// to prevent raise alarm in case that record not found in QDX. on DELETE operation
	long ltmpRet = lRetVal;
	if (lRetVal ==  ERR_NOT_FOUND)
	{
		ltmpRet = OK;
	}
	
	//4.0.5.24
	SetAlarm(ltmpRet, (long)nFileNum, FALSE);


	return (lRetVal);
}




long CIndexFile::SwapLong(long lSrc)
{
	long l2 ;
	char *c1 =(char *)&lSrc ;
	char *c2 =(char *)&l2 ;
	c2[0]=c1[3] ;
	c2[1]=c1[2] ;
    c2[2]=c1[1] ;
    c2[3]=c1[0] ;
	return l2	;
}




short CQdexFile::GetFileType()
{
	return (short)enumFileType;
}


//4.0.3.21
long CQdexFile::SetAlarm(long lQdxError, long lFileNumber,  BOOL lRelative)
{
	long lRetVal = 0;
	BOOL bSendAlarm = FALSE;
	long lAlarmStatus = ALARM_OFF;
	//check if alarm status was change
	if(lQdxError && m_enumAlarmStatus == QDEX_ALARM_OFF)
	{
		m_enumAlarmStatus = QDEX_ALARM_ON;
		lAlarmStatus = ALARM_ON;
		bSendAlarm = TRUE;
	}
	else if(lQdxError == OK && m_enumAlarmStatus == QDEX_ALARM_ON)
	{
		m_enumAlarmStatus = QDEX_ALARM_OFF;
		lAlarmStatus = ALARM_OFF;
		bSendAlarm = TRUE;
	}
	
	if (_ALARM_SRV.m_lAlarmSrvActive && bSendAlarm)
	{
		switch(lQdxError)
		{
			case OK:
			{
				//clears all alarms for this subcategory
				_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE,SUB_QDX, 0,lFileNumber,lAlarmStatus,lFileNumber,0);				
			}
			break;
			case ERR_DELETED:
			{
				_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE,SUB_QDX, QDX_ERR_DELETED,lFileNumber,lAlarmStatus,lFileNumber,0);
			}
			break;
			case ERR_EXISTS:
			{
				_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE,SUB_QDX, QDX_ERR_EXISTS,lFileNumber,lAlarmStatus,lFileNumber,0);
			
			}
			break;
			case ERR_DISK_READ:
			{
				//4.0.3.30
				// this error will reported only in relative Qdx.	
				//if (lRelative)
				//4.0.3.30
					_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE,SUB_QDX, QDX_ERR_DISK_READ,lFileNumber,lAlarmStatus,lFileNumber,0);
			}
			break;
			case ERR_DISK_WRITE:
			{
				_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE,SUB_QDX, QDX_ERR_DISK_WRITE,lFileNumber,lAlarmStatus,lFileNumber,0);
			}
			break;
			case ERR_EXP_FILE_FULL:
			case ERR_FILE_FULL:
			{
				_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE,SUB_QDX, QDX_ERR_FILE_FULL,lFileNumber,lAlarmStatus,lFileNumber,0);
			}
			break;
			case ERR_NOT_OPENED:
			{
				_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE,SUB_QDX, QDX_ERR_QDX_NOT_OPEN,lFileNumber,lAlarmStatus,lFileNumber,0);			
			}
			break;	
			default:
			{
				_Module.m_server.m_cAlarm.AddSendMessage(CATEGORY_SOFTWARE,SUB_QDX, QDX_FATAL_ERROR,lFileNumber,lAlarmStatus,lFileNumber,0);			
			}		
		}
	}

	return (lRetVal);
	
}
//4.0.5.24
BOOL CQdexFile::IsQdexAlarmOn()
{
	BOOL bQdexAlarmOn = FALSE;

	if(m_enumAlarmStatus == QDEX_ALARM_ON)
		bQdexAlarmOn = TRUE;
	
	return bQdexAlarmOn;
}


/******************************************************************************
 Description:	Start QDX maintenance process
 Returns:      	return code
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			02/09/2007   15:30		Start	4.0.18.20 - CR 33898
******************************************************************************/
void CQdexFile::StartMaintenance(BOOL bUploadOnly, BOOL bForce)
{
	if (bUploadOnly)
	{
		UploadBinFiles();
		_Module.m_server.m_cParam.SaveParam("GeneralParam","InMaintenanceQdxNum",(DWORD)0,FALSE,0,0,0,TRUE);//33898
	}
	else
	{
		//4.0.18.20 - Get the DoReload key from the registry
		CString sDatabasePath;
		DWORD dwDoReload = 0;

		sDatabasePath.Format("Database\\%s",GetFileName());
		_Module.m_server.m_cParam.LoadParam((char *)((LPCTSTR)sDatabasePath), "DoReload",&m_lDoReload, (DWORD)m_lDefaultDoReload, TRUE);

		if ((m_lDoReload) || (bForce))
		{
			CString str;
			str.Format("QDXMaint - Start maintenance for qdx file number %ld (%s)", q_param.indx_num, GetFileName()); //4.0.18.40
			_LOGMSG.LogMsg(str);	//4.0.18.30
			// Set In middle of QDX maintenance process to TRUE
			SetInMiddleOfQDXMaintFlag(TRUE);
			_Module.m_server.m_cParam.SaveParam("GeneralParam","InMaintenanceQdxNum",(DWORD)q_param.indx_num,FALSE,0,0,0,TRUE);//33898


			// Start Q-Load
			QLoad();

			// Upload the files back to the QDX file
			UploadBinFiles();
			_Module.m_server.m_cParam.SaveParam("GeneralParam","InMaintenanceQdxNum",(DWORD)0,FALSE,0,0,0,TRUE);//33898

			SetInMiddleOfQDXMaintFlag(FALSE);

			//Reset the Sequence Number:
			ReSetQDXMaintSeqNumber();
			
			str.Format("QDXMaint - End maintenance for qdx file number %ld (%s)", q_param.indx_num, GetFileName()); //4.0.18.40
			_LOGMSG.LogMsg(str); //4.0.18.30
		}
	}


}

/******************************************************************************
 Description:	Make QLoad to specific QDX file by calling to Q-Load32.exe command 
 Returns:      	return code
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			02/09/2007   15:30		Start	4.0.18.20 - CR 33898
 AmitH          19/11/2007              Cont.  4.0.18.40
 Shuki			14/07/2008				Merge from 1018.323
 Gena			25/08/2010				Update 4.0.25.80 TD 142081
******************************************************************************/
BOOL CQdexFile::QLoad()
{
	BOOL	bRetVal = TRUE;
	char	szQdexEngineDir[MAX_FILE_PATH];		//4.0.20.500

	memset(szQdexEngineDir,	0, sizeof(szQdexEngineDir));	//4.0.20.500

	_Module.m_server.m_cParam.LoadParam("Paths\\Long", "QdxEngineDir", szQdexEngineDir, sizeof(szQdexEngineDir), "", FALSE, FALSE);		//4.0.20.500

	if ( strlen(szQdexEngineDir) > 0 )
	{
		char	szQLoadFullFileName[MAX_FILE_PATH];
		memset(szQLoadFullFileName,	0, sizeof(szQLoadFullFileName));

		//Create the Q-load full file name
		sprintf(szQLoadFullFileName, "%s\\%s", szQdexEngineDir, Q_LOAD_APP);	//4.0.18.31	//4.0.20.500		

		//check if the QLoad file exists before trying to run it
		WIN32_FIND_DATA FindFileData;
			
		HANDLE hFile = FindFirstFile(szQLoadFullFileName, &FindFileData);
		
		if(hFile == INVALID_HANDLE_VALUE)
		{
			CString strLog;
			strLog.Format("QDXMaint - CQdexFile::QLoad failed to locate in directory [%s]", szQLoadFullFileName);		//4.0.18.40	//4.0.20.500
			_LOGMSG.LogMsg(strLog,LOG_LEVEL_1);
		}
		else
		{
			char	szQdexFilesDir[MAX_FILE_PATH];		
			memset(szQdexFilesDir,	0, sizeof(szQdexFilesDir));

			_Module.m_server.m_cParam.LoadParam("Paths\\Long", "QdxFilesDir", szQdexFilesDir, sizeof(szQdexFilesDir), "", FALSE, FALSE);		//4.0.20.500

			if( SetCurrentDirectory(szQdexFilesDir) )		
			{
				char	szExParameters[MAX_FILE_PATH];
				memset(szExParameters,	0, sizeof(szExParameters));

				sprintf(szExParameters,"/L%ld", q_param.indx_num);	
				
				//*******************************
				// Run the Q-load command
				//*******************************
				
				long lRetVal = _Module.m_server.m_cCommonFunction.ExecuteFile(szQLoadFullFileName, "open", szExParameters);

				if ( lRetVal != RetCode_Success )
				{
					CString strLog;
					strLog.Format("QDXMaint - CQdexFile::QLoad command [%s] failed for file number %ld [%s], retCode=%ld", szQLoadFullFileName, (long)q_param.indx_num, m_sFileName, lRetVal); //4.0.18.22 //4.0.18.40
					_LOGMSG.LogMsg(strLog, LOG_LEVEL_1);
				}
				else
				{
					bRetVal = FALSE;

					CString strLog;
					strLog.Format("QDXMaint - CQdexFile::QLoad command [%s] run successfully for file number %ld [%s]", szQLoadFullFileName, (long)q_param.indx_num, m_sFileName); //4.0.18.22 //4.0.18.40
					_LOGMSG.LogMsg(strLog);
				}
			}
			else
			{
				CString strLog;
				strLog.Format("QDXMaint - CQdexFile::QLoad failed to locate and set qdx files directory [%s]", szQdexFilesDir); //4.0.18.320
				_LOGMSG.LogMsg(strLog, LOG_LEVEL_1);			
			}
		}
		
		FindClose(hFile);

		//Restore path back to the home dir: //4.0.20.500
		char szHomeDir[MAX_FILE_PATH];
		memset(szHomeDir, 0, sizeof(szHomeDir));

		_Module.m_server.m_cParam.LoadParam("Paths\\Long", "HomeDir", szHomeDir, sizeof(szHomeDir), "", FALSE, FALSE);
		
		if (strlen((char*)szHomeDir) > 0)
			SetCurrentDirectory(szHomeDir);		
	}
	else		//4.0.20.500
	{		
		CString str;
		str.Format("QDXMaint - CQdexFile::QLoad wrong Qdex Engine Directory [%s]", szQdexEngineDir); 
		_LOGMSG.LogMsg(str, LOG_LEVEL_1);		
	}

	return bRetVal;
}


/******************************************************************************
 Description:	After QLoad - upload the binary files back into the QDX files	
 Returns:      	return code
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			02/09/2007   14:30		Start	4.0.18.20 - CR 33898
******************************************************************************/
BOOL CQdexFile::UploadBinFiles()
{
	// call ReadBinFile();
	// call Operation(..., bIsSaveByMaint = TRUE);
	return 0;
}

//33898 4.0.18.20
void CQdexFile::SetFileName(char * sFileName)
{
	if (m_sFileName)
	{
		delete[] m_sFileName;
		m_sFileName = NULL;
	}
	m_sFileName = new char[strlen(sFileName) + 1];
	if (m_sFileName)
		strcpy(m_sFileName, sFileName);
	else
	{
		char sLogMsg[MAX_FILE_PATH];
		sprintf(sLogMsg, "QDXMaint - CQdexFile::SetFileName failed for %s",sFileName); //4.0.18.40
		_LOGMSG.LogMsg(sLogMsg,LOG_LEVEL_1);

	}
}
char *	CQdexFile::GetFileName()
{
	return m_sFileName;
}



/******************************************************************************
 Description:	During QDX maintenance save the data into a binary file	
 Returns:      	return code
 Parameters:   	void *pBuff
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			02/09/2007   13:00		Start	4.0.18.20 - CR 33898
 Liat			04/09/2007	 12:00		Update
******************************************************************************/
long CIndexFile::WriteBinFile(char * sFileName, void *pBuff, long lDataSize) 
{
	long lRetVal = 0;
	BOOL bRetVal = FALSE;
	
	BSTR bstrRec = NULL;

	try
	{
		std::fstream fQdxMaintBinFile;
		char sLogMsg[MAX_FILE_PATH];
		sprintf(sLogMsg, "QDXMaint - Writing File:%s , data size: %ld",sFileName,lDataSize); //4.0.18.40
		_LOGMSG.LogMsg(sLogMsg);
			
		//open file for writing , overwrite file if exists
		fQdxMaintBinFile.open(sFileName, std::ios::out | std::ios::trunc | std::ios::binary);

		//write to file
		fQdxMaintBinFile.write((const char *)(pBuff),lDataSize);
		fQdxMaintBinFile.close();
		lRetVal = FALSE;	//means ok
	}
	catch(...)
	{
		_LOGMSG.LogMsg("CIndexFile::SaveQDXMaintFile() - Failed to save data into binary file",LOG_LEVEL_1);
		lRetVal = TRUE;
	}

	return (lRetVal);
}


/******************************************************************************
 Description:	During QDX maintenance get the data from a binary file	
 Returns:      	return code
 Parameters:   	char * sFileName
				void *pBuff
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Liat			05/09/2007   9:00		Start	4.0.18.20 - CR 33898
******************************************************************************/

long CIndexFile::ReadBinFile(char * sFileName, void *pBuff, long lDataSize)
{
	long lRetVal = 0;
	BOOL bRetVal = FALSE;
	
	try
	{
		std::fstream fQdxMaintBinFile;
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
  
		hFind = FindFirstFile(sFileName, &FindFileData);

		if (hFind != INVALID_HANDLE_VALUE) 
		{
		    //open file for reading 
			FindClose(hFind);
			fQdxMaintBinFile.open(sFileName, std::ios::binary|std::ios::in);
			fQdxMaintBinFile.read((char*)pBuff, lDataSize);
			fQdxMaintBinFile.close();
			lRetVal = TRUE;
		}
		else
		{
			lRetVal = FALSE;
		}
			
	}
	catch(...)
	{
		_LOGMSG.LogMsg("QDXMaint - CIndexFile::ReadBinFile() - Failed to read data from a binary file",LOG_LEVEL_1); //4.0.18.40
		lRetVal = FALSE;
	}

	return (lRetVal);

} 

BOOL	CIndexFile::UploadBinFiles()			//4.0.18.0
{
	long lNumOfOpcodes = 11;
	char sFileToSearch[MAX_FILE_PATH + MAX_FILE_NAME];
	char sFileNameWithPath[MAX_FILE_PATH + MAX_FILE_NAME];
	char sQdexWorkDir[MAX_FILE_PATH];
	long lRetVal = 0;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	
	memset(sQdexWorkDir,0,sizeof(sQdexWorkDir));
	memset(sFileNameWithPath, 0, sizeof(sFileNameWithPath));
	memset(sFileToSearch, 0, sizeof(sFileToSearch));

	_Module.m_server.m_cParam.LoadParam("Paths\\Long","MaintQDXDir", sQdexWorkDir, sizeof(sQdexWorkDir), "", FALSE, FALSE);

	sprintf(sFileToSearch,"%s\\%s*.bin",sQdexWorkDir,GetFileName());
	hFind = FindFirstFile(sFileToSearch, &FindFileData);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		sprintf(sFileNameWithPath,"%s\\%s",sQdexWorkDir,FindFileData.cFileName);

		lRetVal = ParseBinFile(sFileNameWithPath);
		if (lRetVal)
			DeleteFile(sFileNameWithPath);
		FindClose(hFind);
		hFind = FindFirstFile(sFileToSearch, &FindFileData);
	}

	return TRUE;
}


long CIndexFile::SaveQDXMaintFile(eFileOperation eOpCode, void * pInfo, long lDataSize) //4.0.18.20
{
	long lRetVal = FALSE;

	m_csFile.Lock();
	switch (eOpCode)
	{
		case REC_READ:
		case REC_READ_FIRST:
		case REC_READ_NEXT:
		case REC_READ_LAST:
		case REC_READ_SEQ:
		case REC_READ_PREV:
		{
			lRetVal = QDX_IN_MIDDLE_OF_PROCESS;	//was TRUE
			break;
		}
		default:
		{
			char sFileNameWithPath[MAX_FILE_PATH + MAX_FILE_NAME];
			char sQdexWorkDir[MAX_FILE_PATH];
		
			memset(sQdexWorkDir,0,sizeof(sQdexWorkDir));
			_Module.m_server.m_cParam.LoadParam("Paths\\Long","MaintQDXDir", sQdexWorkDir, sizeof(sQdexWorkDir), "", FALSE, FALSE);

			sprintf(sFileNameWithPath,"%s\\%s_%03ld_%d.bin",sQdexWorkDir, GetFileName(),m_lSeqNumber++, eOpCode);

			lRetVal = WriteBinFile(sFileNameWithPath, pInfo, lDataSize);			
		}		
		
	}
	m_csFile.Unlock();
	
	return lRetVal;
}

