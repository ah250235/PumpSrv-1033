
// CommonFunction.h: interface for the CCommonFunction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMONFUNCTION_H__8BF61215_1AB7_11D8_BBDF_00500470AD8E__INCLUDED_)
#define AFX_COMMONFUNCTION_H__8BF61215_1AB7_11D8_BBDF_00500470AD8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCommonFunction  
{

public:		
	void _Reconfigure2(long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags, long *pRetVal,BOOL bSendEvent);
	void GetTransactionList4IFSFTerminal(long & lPumpNumber, long lPos, long lFlag, char * szTrack2 = NULL, long lenTrack2 = NULL); //4.0.19.504 - 47559
	long RemovePrePaidTrs__(long &lPumpNumber, long  & lSyncNumber);
	long CheckDeliveryProcessState(long &lParam1, long &lParam2 , long *pVal);
	long CheckDeliverySingleTankProcessState(const long lTankNumber, const long lProcessCommand);//4.0.21.500 - TD 66935
	long GetWetStockData__(long lWetStockFlags, long *pVal);
	BOOL IsPumpIdle(short nPump);
	void GetStationStatus(long *lPumpMap1, long *lPumpMap2, long lFlags, long *pRetVal);
	void GetDeliveryEmulationStatusSingleTank(long lTankNumber, long *plStatus, BSTR *pRec);//4.0.21.500 TD 66935
	GetPAKQueryRetCode GetPAKQuery(BSTR * sData , long * plRecCount , long *lFlags , BSTR  sRestrictionData=NULL , long lPumpNumber=0);
	GetPAKQueryRetCode GetPAKQuery2(BSTR * sData , long * plRecCount , long *lFlags , BSTR  sRestrictionData=NULL , long lPumpNumber=0);
	long SetRecordStatus(long lShiftNumber, long lState, long lPosNumber, long lFlags);
	long GetReport(long lShiftNumber, long lState, long lPosNumber, long lFlags, long lVal, SYSTEMTIME  & cStartTime, SYSTEMTIME  & cEndTime, long *lRecordCount, BSTR  & sRec);
	void Reconfigure2(long lTableId , long lSubTableId , long lSubTableIdExt , long lFlags , long * pRetVal,BOOL bSendEvent = TRUE);
	void GetTankMemoInfo(long lTankNumber, BSTR *pRec, long lFlags, long *pVal,long *lRecordCount , long lVersion=0);
	void SetProcess (long lProcessNumber, long lParam1, long lParam2, long lParam3, long lParam4, long *pVal); //4.0.5.24	
	BOOL IsPumpIdleForModeChange(long lPumpNumber);//4.0.9.506 //amram
	GetPAKQueryRetCode GetTransListQuery(long lPumpNumber,BSTR *sData, long *plRecCount, long *lFlags);//4.0.9.506 //amram
	void GetTotalPumpsInfo(long lPumpNumber,long * lCount,BSTR * pRec);//4.0.12.506
	void GetReceipt2(long lPosNumber, BSTR sTrack2, long lIndex, BSTR *sReceiptData, long lFlags,long lFromPosNumber, long *pVal); //4.0.12.512
	void GetShiftProcessStatus(long *lShiftNumber, long *lShiftStatus); //4.0.16.500
	CtrlRetCode SetCarWashInfo(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord); //4.0.16.500 - CR 34815
	long ClearCashAcceptorData(long lParam1, long lParam2); //4.0.16.502 - TD 35765
	
	//TD 38654- start	CONVERT METHODS!!!!!!!
	void ConvertFromTrs5ToTrs2(TAG_PUMP_TRANSACT5 *structIn, TAG_PUMP_TRANSACT2 *structOut);
	void ConvertFromTagPump2ExToTagPump2(TAG_PUMP2_EX *structIn, TAG_PUMP2 *structOut);
	void ConvertFromTagPump3ExToTagPump3(TAG_PUMP3_EX *structIn, TAG_PUMP3 *structOut);
	void ConvertFromTagPump4ExToTagPump4(TAG_PUMP4_EX *structIn, TAG_PUMP4 *structOut);
	void ConvertFromTagPump5ExToTagPump5(TAG_PUMP5_EX *structIn, TAG_PUMP5 *structOut);
	void ConvertFromTagPump6ExToTagPump6(TAG_PUMP6_EX *structIn, TAG_PUMP6 *structOut);
	void ConvertFromTagPump7ExToTagPump7(TAG_PUMP7_EX *structIn, TAG_PUMP7 *structOut);
	void ConvertFromTagPump8ExToTagPump8(TAG_PUMP8_EX *structIn, TAG_PUMP8 *structOut);
	void ConvertFromTagPrepay2ExToTagPrepay2(TAG_PRE_PAY_REC2_EX *structIn, TAG_PRE_PAY_REC2 *structOut);
	void ConvertFromTagPrepayExToTagPrepay(TAG_PRE_PAY_REC_EX *structIn, TAG_PRE_PAY_REC *structOut);
	void ConvertFromTagOlaRec2ExToTagOlaRec2(TAG_OLA_REC2_EX *structIn, TAG_OLA_REC2 *structOut);
	void ConvertFromTagOlaRec5ExToTagOlaRec5(TAG_OLA_REC5_EX *structIn, TAG_OLA_REC5 *structOut);
	void ConvertFromTagOlaRec9ExToTagOlaRec9(TAG_OLA_REC9_EX *structIn, TAG_OLA_REC9 *structOut);
	void ConvertFromTagOlaRec12ToTagOlaRec9(TAG_OLA_REC12 *structIn, TAG_OLA_REC9 *structOut);//4.0.27.47 
	void ConvertFromTagOlaMemRecExToTagOlaMemRec(TAG_OLA_MEM_REC_EX *structIn, TAG_OLA_MEM_REC *structOut);

	void ConvertCardSaleAll3ToPayAtPumpInfo(PAY_AT_PUMP_INFO& cPayAtPumpInfoOut,const CARD_SALE_ALL3&  cCardSaleAll3In);
	void ConvertPayAtPumpInfoToCardSaleAll3(CARD_SALE_ALL3&  cCardSaleAll3In, const PAY_AT_PUMP_INFO& cPayAtPumpInfoOut);

	void ConvertExtraInfoRecord2Tag(TAG_DIESEL_SOLD_INFO*		psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber, const long lPosNumber);
	void ConvertExtraInfoRecord2Tag(TAG_DISCOUNT_LOYALTY_INFO*	psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber, const long lPosNumber);
	void ConvertExtraInfoRecord2Tag(TAG_DISCOUNT_LOYALTY_INFO2* psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber, const long lPosNumber);
	void ConvertExtraInfoRecord2Tag(TAG_DISCOUNT_LOYALTY_INFO3* psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber, const long lPosNumber);
	void ConvertExtraInfoRecord2Tag(TAG_ANPR_INFO_PLUS*			psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber);
	void ConvertExtraInfoRecord2Tag(TAG_DISCOUNT_LOYALTY_INFO4* psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber, const long lPosNumber);	//4.0.23.504 66705
	void ConvertExtraInfoRecord2Tag(TAG_PUMP_TEST_INFO*			psTagInfo, const EXTRA_INFO_REC& rsExtraInfo, const long lPumpNumber);						    //4.0.25.1140 TD 412765
	
	void ConvertFromTagOlaRecToPAPInfo(	PAY_AT_PUMP_INFO* pPAPInfoOut,	const TAG_OLA_REC12* pTagOlaRecIn );														//4.0.24.71 No TD
	long ConvertFromTagOlaRecToItems( ITEMS_INFO* pItemsInfoOut,		const TAG_OLA_REC12* pTagOlaRecIn );														//4.0.24.71 No TD
	long ConvertFromTagOlaRecToZones(PS::RECEIPT_EXTRA_INFO_REC* pZonesInfoOut, const TAG_OLA_REC12* pTagOlaRecIn, long lTrsNumber );										//4.0.24.71 No TD
	void ConvertFromTagOlaRecToTagDiscountLoyalty(TAG_DISCOUNT_LOYALTY_INFO4* pTagDiscountLoyaltyInfoOut, const TAG_OLA_REC13* pTagOlaRecIn, long lPumpNumber );	//4.0.24.71 No TD
	
	void ConvertDiscountArrayToDiscountMap( TAG_DISCOUNT_MAP* pDiscountMapOut, const DISCNT* pDiscountArrIn );										//4.0.24.71 No TD	
	void ConvertGradeDiscountToNozzleDiscount( long lPumpNumber, TAG_DISCOUNT_MAP* pNozzleDiscount, const TAG_DISCOUNT_MAP* pGradeDiscount );		//4.0.24.71 No TD
	void ArrangeGradesLimitsByPosition( long lPumpNumber, GRADE_LIMITS* pGradeLimitsArray ); //4.0.24.140 127928
	
	void SetPumpMisc(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal, BOOL bMiscExt = FALSE);	//4.0.20.451 65268 //4.0.21.501
	void SetPumpMiscExt(long lPumpNumber, long lMisc, long bRemoveMisc, long *pVal);

	BOOL DeleteAllLoyaltyXmls(); //4.0.19.504
	BOOL DeleteLoyaltyXmslFile(long lPumpNumber, long lTrsNum); //4.0.19.504
	BOOL GetLoyaltySummaryXmlFileName(long lPumpNumber , long lTrsNum, char * szOutXmlFileName);  // TD 321602   4.0.111.174
	BOOL GetLoyaltyPromotionsXmlFileName(long lPumpNumber , long lTrsNum, char * szOutXmlFileName);  // TD 321602   4.0.111.174
	
	//End of TD 38654
    long GetAccountNumByTrack2(char* sOutAccountNum, const BSTR sTrack2, const long lFlags, const long lPumpNumber = 0); //4.0.20.501 TD-60236

	CtrlRetCode ParseTanksDeliveryReferenceNumbers(BSTR sXmlRec); //4.0.20.501 - TD 65174
	void        ResetTanksReference(); //4.0.20.501 - TD 65174

	CtrlRetCode GetDeliveryData(BSTR sXmlRec, long &lTankNumber, long &lTankReference, BOOL &bLockConnectedPumps);//4.0.21.500 - TD 66935
	void ResetDeliveryEmulationSingleTankReadingOutData(const long lTankNumber);//4.0.21.500 TD 66935
	void ResetDeliveryEmulationSingleTankReadingInData(const long lTankNumber); //4.0.21.500 TD 66935
	void ResetDeliveryEmulationSingleTankProcessData(const long lTankNumber);   //4.0.21.500 TD 66935
	BOOL IsDeliveryEmulationSingleTankInProcess(long & lTankNumber);            //4.0.21.500 TD 66935
	BOOL IsDeliveryEmulationSingleTankInReadingProcess(long & lTankNumber) const;//4.0.21.500 TD 66935 // 4.0.28.501 TD 254219
	BOOL IsTimeOutPassed(const SYSTEMTIME tTimeVal, const long lTimeOutVal);    //4.0.21.500 TD 66935
	BOOL IsPaymentCard(const long lCardType);                                   //4.0.21.501 TD 67443
	BOOL IsLoyaltytCard(const long lCardType);
	BOOL IsPumpOnUnmannedMode(const long lPumpNumber);  //4.0.22.506 TD 74605
	BOOL IsClubCardHashNumberPresent(const long lPumpNumber);
	
	long GetTagRecSize(const short nFlags);
	long GetExtraInfoRecFromDB(EXTRA_INFO_REC* pExtraInfo, const short nFlags, const long* plIndexNumber, CString *pExtraInfoXml = NULL);
	
	void CalcLoyaltyVolumeLimit( TAG_DISCOUNT_LOYALTY_INFO4* pTagDiscountLoyaltyInfoOut );															//4.0.24.71 No TD
	void SetLowesBetweenOLAandLoyaltyLimits( long lPumpNumber,TAG_DISCOUNT_LOYALTY_INFO4* rLoyaltyInfoInOut, PRE_PAY_INFO* rPAPInfoInOut );		//4.0.24.71 No TD
	void UpdateOLAFlags( PAY_AT_PUMP_INFO* pPAPInfoInOut, long lNewFlag );																			//4.0.24.71 No TD	
	void GeneratePrePayTrsLinkFlags( const long lFlags, short* lTrsLinkFlags );

	DWORD ExecuteFile (char* szFilePath, char* szFileAction, char* szExtraParameters, long lConsolShow = SW_HIDE);
	long  LookForInternalReceiptIndex(char* psIndexOut, const long lTrsNumber);

	CtrlRetCode GetRollingUpdateInfo(long lPumpNumber, long lFlags, BSTR *pbstrData); //4.0.26.501 - TD 108311

	BOOL IsIntheMiddleOfProc() const; // TD 254219   4.0.28.501

	void LoadMessageFromLanguagesReg(int lPumpNumber, const char * const sMsgPathKey, char * sMsgOut, int iSizeOfMsg, char *pszDefaultVal = "");

	void CopyZoneToCardSale(long lPumpNumber, long lReceiptIndex,/*CARD_SALE_EXTRA_DATA4_PLUS &cCardSaleExtraDataPlus*/ BYTE *sFiler2043); // // No TD - print receipt on card denial.

	void HandleMiscCardNotAuthorized(long lPumpNumber, PAY_AT_PUMP_INFO* pInfo = NULL, CPumpTransact* pTrs = NULL );
	void ResetMiscCardNotAuthorized(long& lMisc);
	void ClearDeclinedMiscs(long lPumpNumber, const char* const pCallingMethodName);
	void LoadDefaultLaguageString(long lPumpNumber , char * sOutStr , int iOutStrSize);
	void PayAtPumpInfoToCardSaleAll3(CARD_SALE_ALL3& cCardSaleAll3In, const PAY_AT_PUMP_INFO& cPayAtPumpInfoOut);
	void CardSaleAll3ToPayAtPumpInfo(PAY_AT_PUMP_INFO& cPayAtPumpInfoOut,const CARD_SALE_ALL3& cCardSaleAll3In);
	void SetProcessExt(long lProcessNumber, long lParam1, BSTR sInData, BSTR * sOutData, long *pVal);

	CtrlRetCode GetConfiguration(BSTR sClientName, BSTR *sConfigurationDatal);   // 4.0.25.994  TD 325383
	CtrlRetCode SetTankState(long lTankNumber, long lState, long lFlags, BSTR sRec); // 4.0.25.1130 
	BOOL GetGradePositionInfo(long lPumpNumber, long lPosition, TAG_GRADE_POSITION_INFO &cTagGradePositionInfo);	// 4.0.25.1130 
	BOOL IsNozzleLocked(long lPumpNumber, long nPosition);	//4.0.25.1130 
	CtrlRetCode GetTankState(long lTankNumber, long lFlags, BSTR * pRec);	//4.0.25.1130 
	BOOL SetPumpCalibrationDetails(long lPumpNumber, BSTR sXmlRec);     //4.0.25.1140 TD 412765 

	void _SmartReconfigure(long lDeviceNumberMask1, long lDeviceNumberMask2, long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags, long *pRetVal, BOOL bSendEvent); //CR 442586
	void SmartReconfigure(long lDeviceNumberMask1, long lDeviceNumberMask2, long lTableId, long lSubTableId, long lSubTableIdExt, long lFlags, long * pRetVal, BOOL bSendEvent = TRUE);  //CR 442586
	void SetMaximalAllowedDiscountToPumpTransaction(CPumpTransact *pTrs); 	//RFUEL-3309 Get the maximal discount allowed for the grade 

	CCommonFunction();
	virtual ~CCommonFunction();

    virtual long HandleTerminalCmd(TerminalCmdCode eTerminalCmdCode, long lTerminalId);	
	BOOL IsFileExists( const CString& sFileName );   //  R10 User Story - 92780 - Fuel P@P - OLA recovery.
private:
	// Car wash related functions - 4.0.16.500 - CR 34815
	CtrlRetCode SetWashProgramsData(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord);
	CtrlRetCode SetCarWashDeviceData(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord);
	CtrlRetCode SetCarWashGeneralData(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord);
	CtrlRetCode SetTanksData(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord); //4.0.20.501 - TD 65174
	CtrlRetCode GetTankData(CXMLInterface *pXmlDoc, CXMLPointers *pXmlRecord, long &lTankNumber, long &lTankReference, BOOL &bLockConnectedPumps);//4.0.21.500 - TD 66935
	long GetStatusSingleTank(long lTankNumber); //4.0.21.500 TD 66935
	void BulidDeliverySingleTankXML(long lTankNumber, long lStatus, BSTR *pRec);                   //4.0.21.500 TD 66935
	void SetProcessDeliveryEmulationSingleTank(long lTankNumber, long lTankReference, BOOL bLockConnectedPumps, long lProcessCommand, long *pVal);//4.0.21.500 TD 66935
    BOOL IsValidReferenceNumber(const long lTankNumber,const long lTankReference);//4.0.21.500 TD 66935
	BOOL DeliveryEmulationSingleTankInProcess(const long lTankNumber);//4.0.21.500 TD 66935
	BOOL CheckIncludeTankInReport(BYTE *pBuff); //4.0.22.503 - TD 69905

	void GetStationStatusPumpNotIdle(long *lPumpMap1, long *lPumpMap2); //4.0.22.504 - TD 72710
	void GetStationStatusPumpOffline(long *lPumpMap1, long *lPumpMap2); //4.0.22.504 - TD 72710

	void CreateRollingUpdatePumpInfo(long lPumpNumber, CXMLInterface *pXmlInterface, CXMLPointers *pXmlPumpsElement); //4.0.26.501 - TD 108311
	void ConvertToExtendedGradeForPAKQuery(const long lPump, unsigned char *sGrade, const long lSizeOfGrade, const long lFlags); //4.0.28.500 - TD 224805
    void BuildTerminalCmdRequest(TerminalCmdCode eTerminalCmdCode, long lTerminalId, char * pInBuffer); 
    long ProcessTerminallogOn(long lTerminalId);	
	void GetStationStatusUnpaidTrsOnPump(long *lPumpMap1, long *lPumpMap2);//4.0.29.501 284709
	void SetSingleTankState(long lTankNumber, long lState);
	void SetAllTanksState(long lState);
	BOOL SetCashierInfo(long lPumpNumber, BSTR sXmlRec);
	void SetPumpNozzleNotification(long lPumpNumber, long lPosition);
};

#endif // !defined(AFX_COMMONFUNCTION_H__8BF61215_1AB7_11D8_BBDF_00500470AD8E__INCLUDED_)
