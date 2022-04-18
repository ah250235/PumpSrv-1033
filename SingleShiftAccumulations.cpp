// SingleShiftAccumulations.cpp: implementation of the CSingleShiftAccumulations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SingleShiftAccumulations.h"


//////////////////////////////////////////////////////////////////////
// CSingleShiftAccumulations Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSingleShiftAccumulations::CSingleShiftAccumulations():CRelativeFile(SINGLE_SHIFT_ACCUMULATIONS_FILE)
{

}

CSingleShiftAccumulations::~CSingleShiftAccumulations()
{

}
/******************************************************************************
 Description:	Check whether SingleShiftAccumulations Database is connective 	
 Returns:		lRtc = Qdx return code	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ella			08/07/2010   12:14		Start
******************************************************************************/
long CSingleShiftAccumulations::ValidityCheck()
{
	BYTE		sFileValid[STR_LONG_VAL10];
	long		lRtc = 0;

	m_csFile.Lock();
	lRtc = Read(0,STR_LONG_VAL10 ,sFileValid,HOP_DIRECT_LOCAL,0 ,0 ,0);	
	m_csFile.Unlock();

	return lRtc;
}

long CSingleShiftAccumulations::Operation(long lOpCode, void *pRec, void *pData)
{
	long	lRetVal=0;
	SingleShiftAccumRelativeIndex   *pIndex;
	
	if (pData  && pRec)
	{
		pIndex = (SingleShiftAccumRelativeIndex *)pData;		
	}
	else
		lRetVal =999;

	if (!lRetVal)
	{
		m_csFile.Lock();
		
		switch(lOpCode)
		{
			case REC_READ:
				lRetVal = Read(pIndex->lOffSet,(WORD)pIndex->lSize,pRec, HOP_DIRECT_LOCAL,0,0);	
				break;

			case REC_UPDATE:
				lRetVal = Write(pIndex->lOffSet,(WORD)pIndex->lSize, pRec, HOP_DEFAULT_CONFIG,0,0);	
				break;
			default:
				break;

		}
		m_csFile.Unlock();
	}


	if(LOG_FCC_FLOW_CONTROL)
	{
		CString str;
		str.Format("CSingleShiftAccumulations - Operation, Read error %d, (indx_num %d, offset %d, length %d)",lRetVal,q_param.indx_num,q_param.write_part_offset,q_param.write_part_length);
		_LOGMSG.LogMsg(str);
	}

	return lRetVal;	
}