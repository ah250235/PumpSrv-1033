// RelativeDoubleRecord.cpp: implementation of the CRelativeDoubleRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RelativeDoubleRecord.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRelativeDoubleRecord::CRelativeDoubleRecord(BYTE byFileIndex) : CRelativeFile(byFileIndex)
{

}

CRelativeDoubleRecord::~CRelativeDoubleRecord()
{

}

//DEL long CRelativeDoubleRecord::Read(DWORD lReadPartOffset , WORD wReadPartLength , 
//DEL 								 void * pBuff , long lFlags , long lPCNumber,
//DEL 								 long lTimeStampOffset)
//DEL {
//DEL 	long lRtc = 0;
//DEL 	
//DEL 	lRtc = CRelativeFile::Read(lReadPartOffset,
//DEL 							   wReadPartLength,
//DEL 							   pBuff,
//DEL 							   lFlags,
//DEL 							   0,
//DEL 							   0);
//DEL 	
//DEL 	return lRtc;
//DEL }

//Each record in the q-dex is actually composed of two identical sub-records
long CRelativeDoubleRecord::Write(DWORD lWritePartOffset , WORD wWritePartLength , 
								  void * pBuff , long lFlags , long lPCNumber,
								  long lTimeStampOffset)
{
	long lRtc = OK;
	size_t iSizeOfDoubleRecord = wWritePartLength * 2;
	size_t iSizeOfDoubleRecordPlusValidityByte = iSizeOfDoubleRecord + 1;

	DWORD dwOffsetOfFirstRec = 1;   //First record begins after validity byte.
	DWORD dwOffsetOfSecondRec = dwOffsetOfFirstRec + wWritePartLength;

	//Allocate a buffer big enough for two records and an "IsValid" byte
	BYTE * pTmpBuff = new BYTE[iSizeOfDoubleRecord + 1];

	//Get old record
	lRtc = CRelativeFile::Read(lWritePartOffset, iSizeOfDoubleRecordPlusValidityByte, pTmpBuff, HOP_DIRECT_LOCAL, 0, 0);	//4.0.20.503

	//No QDEX error (if file not configured correctly
	//or not configured at all).
	if(!lRtc)  
	{
		//Every double record is assumed to have a validity
		//flag at the beginning.
		if(pTmpBuff[0] != (BYTE)1)
			lRtc = ERR_NOT_FOUND;   //Record not found

		//If read failed, this is first record to be written, so we set
		//both halves of double record to the same value, and set the validity byte
		if(lRtc)
		{
			pTmpBuff[0] = (BYTE)1;
			memcpy((char*)pTmpBuff + dwOffsetOfFirstRec,  //Offset of first rec
				   (char*)pBuff,                          //Rec to write
				   wWritePartLength);

			lRtc = OK;
		}

		//Read op succeeded, so we must be updating an existing record.
		//In this case, we overwrite the first half of the record with the 
		//second half of the record.
		else
			memcpy((char*)pTmpBuff + dwOffsetOfFirstRec,
				   (char*)pTmpBuff + dwOffsetOfSecondRec,
				   wWritePartLength);

		//Update second half of the record with new data
		memcpy((char*)pTmpBuff + dwOffsetOfSecondRec,
			   (char*)pBuff,
			   wWritePartLength);
									  
		CRelativeFile::Write(lWritePartOffset,
							 iSizeOfDoubleRecordPlusValidityByte,
							 pTmpBuff,
							 lFlags,
							 0,
							 0);
	}
	
	delete []pTmpBuff;
	return lRtc;
}


long CRelativeDoubleRecord::Read(DWORD lReadPartOffset , WORD wReadPartLength , 
								 void *pBuff , long lFlags , long lPCNumber , 
								 long lTimeStampOffset)
{
	long lRtc = 0;
	DWORD dwOffset = lReadPartOffset + 1;

	lRtc = CRelativeFile::Read(dwOffset, wReadPartLength, pBuff, HOP_DIRECT_LOCAL, lPCNumber, lTimeStampOffset); //4.0.20.503

	return lRtc;
}





//DEL void CRelativeDoubleRecord::SetFileSignature()
//DEL {
//DEL 	DWORD dwFileSignatureOffset = 0;
//DEL 
//DEL 	long lFlags = HOP_DIRECT_LOCAL;
//DEL 
//DEL 	if(_HOST.m_lHydraActive)
//DEL 		lFlags = HOP_DEFAULT_CONFIG;
//DEL 
//DEL 	CRelativeFile::Write(dwFileSignatureOffset,
//DEL 							 MAX_NAME_LENGTH,
//DEL 							 VALID_SIGNATURE,
//DEL 							 lFlags,
//DEL 							 0,
//DEL 							 0);
//DEL 
//DEL 	return;
//DEL }
