// ReceiptExtraIndexFile.cpp: implementation of the CReceiptExtraIndexFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReceiptExtraIndexFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReceiptExtraIndexFile::CReceiptExtraIndexFile():m_bActiveProcess(0),CIndexFile(RECEIPT_EXTRA_INDEX_FILE,RECEIPT_EXTRA_INDEX_FILE_SIZE)
{

}

CReceiptExtraIndexFile::~CReceiptExtraIndexFile()
{

}
/******************************************************************************
 Description:	get process activity 	
 Returns:      	Active\None
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			2/11/2004   17:49		Start
******************************************************************************/

BOOL CReceiptExtraIndexFile::GetReceiptExtraIndexFlags()
{
	return m_bActiveProcess;
}
/******************************************************************************
 Description:	Update process activity 	
 Returns:      	None
 Parameters:   	lVal - incoming process status.
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			2/11/2004   17:49		Start
******************************************************************************/

void CReceiptExtraIndexFile::SetReceiptExtraIndexFlags(DWORD lVal)
{
	if (lVal)
		m_bActiveProcess = TRUE;
	else
		m_bActiveProcess = FALSE;


	if (LOG_DETAIL_FLOW_CONTROL)		
	{
		CString sMsg;
		sMsg.Format("Extra receipt index file is:%s" ,(m_bActiveProcess)?"Active":"not Active");
		_LOGMSG.LogMsg(sMsg);
	}
	

}
/******************************************************************************
 Description:	Update receipt index with new record data 	
 Returns:      	lRetVal - Qdx operation code
 Parameters:   	RECEIPT_INFO *sRec - Receipt raw data.  
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			2/11/2004   18:12		Start
******************************************************************************/

long CReceiptExtraIndexFile::UpdateReceiptIndex(RECEIPT_INFO *sRec)
{
	PAY_AT_PUMP_INFO			PayAtPumpInfo;
	RECEIPT_EXTRA_INDEX_INFO	receiptExtraIndexInfo;	
	RECEIPT_EXTRA_INDEX_INFO	cTmpReceiptExtraIndexInfo;
	CPumpTransact		trs;
	long lRetVal= -1;


	if (GetReceiptExtraIndexFlags())
	{
		memset(&PayAtPumpInfo,0,sizeof(PAY_AT_PUMP_INFO));
		_Module.m_server.ConvertReceiptToOla(sRec,&trs,  &PayAtPumpInfo);

		// check that we have car registration details
		if (!ChrAll((char *)PayAtPumpInfo.CardSaleInfo.cardData.sFullVehicleId , sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sFullVehicleId)))
		{
			memset(&receiptExtraIndexInfo , ' ' , sizeof(receiptExtraIndexInfo));//4.0.21.20
			//link flags.
			memcpy(receiptExtraIndexInfo.myIndex.sTrack2, 
					sRec->cReceipt1.Index.sTrack2 , 
					sizeof(receiptExtraIndexInfo.myIndex.sTrack2));

			receiptExtraIndexInfo.myIndex.byIndex = 
				sRec->cReceipt1.Index.byIndex;
			
			CString strMsg;	//4.0.21.20 4.0.25.30
			if (_Module.m_server.m_cProtectedDataHandler.IsNeedToRemoveCardDataFromLog())
				strMsg.Format("CReceiptExtraIndexFile::UpdateReceiptIndex  insert to extra index file rec.index.index = [%d] ",receiptExtraIndexInfo.myIndex.byIndex); 					
			else
				strMsg.Format("CReceiptExtraIndexFile::UpdateReceiptIndex  insert to extra index file rec.index.track2 = [%.100s], rec.index.index = [%d] ",receiptExtraIndexInfo.myIndex.sTrack2, receiptExtraIndexInfo.myIndex.byIndex); 	
			
			_LOGMSG.LogMsg(strMsg); 			 			

			memcpy(receiptExtraIndexInfo.sKeyIndex ,PayAtPumpInfo.CardSaleInfo.cardData.sFullVehicleId , sizeof(receiptExtraIndexInfo.sKeyIndex));

			cTmpReceiptExtraIndexInfo = receiptExtraIndexInfo;
			// try read record from Qdx
			if (lRetVal = Operation(REC_READ , &receiptExtraIndexInfo))
			{
				//If we are here is means that we failed in read Then  try to insert the record.
				lRetVal = Operation(REC_INSERT , &receiptExtraIndexInfo);
			}
			else
				lRetVal = Operation(REC_UPDATE , &cTmpReceiptExtraIndexInfo);
		}
	}

	if (lRetVal > 0)
	{
		if (LOG_DETAIL_FLOW_CONTROL)		
		{
			CString sMsg;
			sMsg.Format("Update Extra receipt index %.16s failed lRetVal=%ld", PayAtPumpInfo.CardSaleInfo.cardData.sFullVehicleId ,lRetVal);
			_LOGMSG.LogMsg(sMsg);
		}
	}
	else if (!lRetVal)
	{
		if (LOG_DETAIL_FLOW_CONTROL)		
		{
			CString sMsg;
			sMsg.Format("Update  successfully Extra receipt index %.16s", PayAtPumpInfo.CardSaleInfo.cardData.sFullVehicleId );
			_LOGMSG.LogMsg(sMsg);
		}

	}


	return lRetVal;
}



/******************************************************************************
 Description:	delete receipt index  	
 Returns:      	lRetVal - Qdx operation code
 Parameters:   	RECEIPT_INFO *sRec - Receipt raw data.  
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			2/11/2004   18:12		Start
******************************************************************************/
long CReceiptExtraIndexFile::DeleteReceiptIndex(RECEIPT_INFO *sRec)
{	
	PAY_AT_PUMP_INFO			PayAtPumpInfo;
	RECEIPT_EXTRA_INDEX_INFO	receiptExtraIndexInfo;	
	CPumpTransact		trs;
	long lRetVal=-1;

	if (GetReceiptExtraIndexFlags())
	{
		memset(&PayAtPumpInfo,0,sizeof(PAY_AT_PUMP_INFO));
		_Module.m_server.ConvertReceiptToOla(sRec,&trs,  &PayAtPumpInfo);
		
		if (!ChrAll((char *)PayAtPumpInfo.CardSaleInfo.cardData.sFullVehicleId , sizeof(PayAtPumpInfo.CardSaleInfo.cardData.sFullVehicleId)))
		{						
			memset(&receiptExtraIndexInfo , 0 , sizeof(receiptExtraIndexInfo));
			memcpy(receiptExtraIndexInfo.sKeyIndex ,PayAtPumpInfo.CardSaleInfo.cardData.sFullVehicleId , sizeof(receiptExtraIndexInfo.sKeyIndex));

			lRetVal = Operation(REC_READ , &receiptExtraIndexInfo);

			if (!lRetVal)
			{
				// check  key match
				if (!memcmp(&receiptExtraIndexInfo.myIndex , &sRec->cReceipt1.Index, sizeof(receiptExtraIndexInfo.myIndex)))
				{
					// delete index.
					lRetVal = Operation(REC_DELETE , &receiptExtraIndexInfo);

					if (LOG_DETAIL_FLOW_CONTROL)		
					{
						if (lRetVal)
						{
							CString sMsg;
							sMsg.Format("DeleteReceiptIndex, failed delete receipt index=%d, ret=%ld", sRec->cReceipt1.Index.byIndex, lRetVal);
							_LOGMSG.LogMsg(sMsg);
//							_LOGMSG.LogMsg("CReceiptFile::ReOrganize STARTED !!");
						}
					}
				}				
			}
		}
	}
	
	return lRetVal;

}

long CReceiptExtraIndexFile::ParseBinFile(char * sFileName) //33898
{
	RECEIPT_EXTRA_INDEX_QDX_MAINT_REC maintRec;
	memset(&maintRec, 0, sizeof(RECEIPT_EXTRA_INDEX_QDX_MAINT_REC));
	BOOL bRetVal = ReadBinFile(sFileName,&maintRec,sizeof(RECEIPT_EXTRA_INDEX_QDX_MAINT_REC));
	if (bRetVal)
	{
		CString str;
		str.Format("QDXMaint - CReceiptExtraIndexFile::ParseBinFile file = %s",sFileName); //4.0.18.40	//4.0.18.501
		_LOGMSG.LogMsg(str);
		Operation(maintRec.opCode, &maintRec.info, TRUE);
	}
	return bRetVal;
}

/******************************************************************************
 Description:	delete receipt index  	
 Returns:      	lRetVal - Qdx operation code
 Parameters:   	
				eFileOperation OpCode	-  Qdx operation flags 
				void *pInfo				-  Index details.	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Marik			2/11/2004   18:12		Start
******************************************************************************/

long CReceiptExtraIndexFile::Operation(eFileOperation OpCode, void *pInfo, BOOL bIsSaveByMaint)
{	
	RECEIPT_EXTRA_INDEX_REC		rec;
	RECEIPT_EXTRA_INDEX_INFO	*pReceiptExtraIndexInfo = NULL;	
	long lRetVal = 0;	
	long lPCNumber = 0 ;
	long lTimeStampOffset = 0;

	if(pInfo)
		pReceiptExtraIndexInfo = (RECEIPT_EXTRA_INDEX_INFO *)pInfo;
	if (m_bInMiddleOfQDXMaintFlag && !bIsSaveByMaint) //4.0.18.20 - CR 33898	//4.0.18.501
	{
		RECEIPT_EXTRA_INDEX_QDX_MAINT_REC		maintRec;
		memset (&maintRec,0,sizeof(RECEIPT_EXTRA_INDEX_QDX_MAINT_REC));
		maintRec.opCode = OpCode;
		if (pReceiptExtraIndexInfo)
			maintRec.info = *pReceiptExtraIndexInfo;
		lRetVal = SaveQDXMaintFile(OpCode, &maintRec, sizeof(RECEIPT_EXTRA_INDEX_QDX_MAINT_REC));
	}	
	else
	{


		memset (&rec,0,sizeof(RECEIPT_EXTRA_INDEX_REC));
		m_csFile.Lock();
		
		switch (OpCode)	
		{
			case REC_INSERT:			
				rec.info = 	*pReceiptExtraIndexInfo;
				lRetVal = Insert(&rec,HOP_DEFAULT_CONFIG,lPCNumber,lTimeStampOffset);			
				break;

			case REC_READ:
				rec.info = 	*pReceiptExtraIndexInfo;
				lRetVal = Read(&rec,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			
				if(!lRetVal )
				{	
					if(pReceiptExtraIndexInfo)
						memcpy(pReceiptExtraIndexInfo, &rec.info, sizeof(rec.info));//4.0.21.20
						//*pReceiptExtraIndexInfo = rec.info;				
				}
				else if(lRetVal == Q_ERROR)
					lRetVal = ERR_NOT_FOUND;
				break;

			case REC_READ_FIRST:
				lRetVal = ReadFirst(&rec,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			
				if(!lRetVal )
				{	
					if(pReceiptExtraIndexInfo)
						*pReceiptExtraIndexInfo = rec.info;				
				}
				else if(lRetVal == Q_ERROR)
					lRetVal = ERR_NOT_FOUND;

				break;

			case REC_READ_NEXT:

				rec.info = 	*pReceiptExtraIndexInfo;
				lRetVal = ReadNext(&rec,HOP_DIRECT_LOCAL,lPCNumber,lTimeStampOffset);			
				if(!lRetVal )
				{	
					if(pReceiptExtraIndexInfo)
						*pReceiptExtraIndexInfo = rec.info;				
				}
				else if(lRetVal == Q_ERROR)
					lRetVal = ERR_NOT_FOUND;
				break;

			case REC_UPDATE:
					rec.info = 	*pReceiptExtraIndexInfo;					
					lRetVal = Update(&rec,HOP_DEFAULT_CONFIG,lPCNumber,lTimeStampOffset);			

				break;

			case REC_DELETE:
				rec.info = 	*pReceiptExtraIndexInfo;					
				lRetVal = Delete(&rec,HOP_DEFAULT_CONFIG,lPCNumber,lTimeStampOffset);			
				break;
		}


		m_csFile.Unlock();
	}
	return lRetVal;
}
