#ifndef __GPI_EVENT_MANAGER_H_
#define __GPI_EVENT_MANAGER_H_

#include "Queue.h"
#include "CmdInfo.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct 
{
	long	m_lDataId;
	long	m_lFlags;
}CGPIEventInfo;

class CGPIEventManager  
{
public:
	long FireGPIEvent();
	void HandleGPIEventResult( HRESULT hr, long lClientNumber, CCmdInfo &Cmd );
	long AddEvent(Command lCommand, LONG lSrc, LONG lParam, LPVOID pData, SHORT nDataLen);
	void AddPakReadyTrs(long lPumpNumber, long lTrsNumber, long nNozzle, long lVolume, long lValue, long nLevel, long lGradePrice, long nGrade, long lFlags, long lTotalVal, long lResExt, SALE_EXTRA_DETAILS cSaleExtraDetails, long lRetCode); //4.0.19.504 - 47559
	void AddSetPumpStatus(long lPumpNumber, long lStatus, long lRes, long lResExt,long lFlags, long lParam1); //4.0.19.504 - 47559
	void AddSetPumpStatusEx(long lPumpNumber, long lStatus, long lRes, long lResExt, long lFlags, long lLimit, short nGrade, short nPosition); //CR 444830
	void AddSetPumpMode(long lPumpNumber,long lMode,long lServiceMode,long lServiceFee,long lPriceLevel,long lFlags);
	void AddPAPSwipeResult(long lPumpNumber,long lDeviceNumber,long lSeqNumber,long lRetcode,long lFlags,BSTR sData, long lPosNumber); //4.0.12.503
	//4.0.19.504 - 47559
	void AddCancelTrs(long lPumpNumber, LONG lTrsNumber, LONG lPumpStatus, LONG lRes, LONG lResExt);
	void AddWashCodeReady(const long lDevice,const long lSyncNumber, const long lFlags);
	void AddReconfigure(long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags);
	void AddSetPreSetLimit(long lPumpNumber, long lPresetLimitType, long lPresetLimitValue);
	void AddGeneralSystemEvent(long lEventId, long lFlags, long lParam1);
	void AddRequest4FormatReceiptData(long & lPumpNumber, long  & lPosNumber, BYTE * sInvoiceNumber);
	void AddRequestGetTransactionList(long  & lPumpNumber , long  & lPosNumber, long lFlag, char * szTrack2, long lenTrack2); // CID_199328 Out-of-bounds access
	void AddTrsStateChangedEvent(long lPumpNumber, long lTrsNumber, long lState, long lFlags); //?.??.?.?? - TD 335186
	void AddSmartReconfigure(long 	lDeviceNumberMask1, long lDeviceNumberMask2, long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags);   //CR 442586

	CGPIEventManager();
	virtual ~CGPIEventManager();


protected:
	CQueue<CCmdInfo,500>		m_cEventsQueue; //4.0.20.0
	CRITICAL_SECTION			m_csEventQueue;
};

#endif //__GPI_EVENT_MANAGER_H_