
#import "LoyaltyFuel.tlb" no_namespace named_guids
#import "FuelLoyaltyClient.tlb" no_namespace named_guids

#include "NmpDetalis.h"
#include "LoyaltyCommandExecuter.h"
#include "Promotion.h"
#include "TicketPromotion.h"
#include "EpsilonLoyaltyCommandExecuter.h"
#include "BaseFlow.h"
#include "DynamicPapFlow.h"
#include <list>
#include <map>

using namespace PS;
using PromotionList = std::list<CPromotion>;
using PromotionIterator = std::list<CPromotion>::iterator;

using LoyaltyRestrctionMap = std::map<DWORD, BOOL>;

#ifndef __FUEL_LOAYALTY_H__
#define __FUEL_LOAYALTY_H__

// macro redefinition FuelCommonDef.h #define MAX_BUF 15000
#define MAX_PROM_PER_PUMP 10
//LPE
#define CENTS_OFF_TYPE	1
#define QUANTITY_TYPE_COUNTED	0x1
#define QUANTITY_TYPE_WEIGHT	0x2

//4.0.19.503 Copient start
#define CANCEL_FROM_PAK			2
#define CAR_WASH_AFTER_FUELING	3

#define ADD_ITEM_CHECK_TIMEOUT 1
#define END_TICKET_CHECK_TIMEOUT 2
#define START_TICKET_CHECK_TIMEOUT 3
#define ADD_MEMBER_CARD_CHECK_TIMEOUT 4
#define ADD_ADD_MEMBER_INFO_CHECK_TIMEOUT 5
#define	QUERY_CHECK_TIMEOUT 6 // TD 406362  4.0.23.2390
#define ADD_CARWASH_ITEM_CHECK_TIMEOUT 7
#define ADD_GET_MEMBER_INFO_CHECK_TIMEOUT 8

#define CLUB_TYPE_PUNCHH	1

enum eSendTransactionTypeForPunchh
{
	_eSTTFP_None = 0,
	_eSTTFP_NoneLoyalty = 0x01,
	_eSTTFP_OtherLoyalty = 0x02
};

//4.0.19.503 Copient end

#define	ADD_MEMBER_CARD_METHOD	"AddMemberCard"	//4.0.22.504 69344
#define START_TICKET_METHOD		"StartTicket"	//4.0.22.504 69344
#define	ADD_MEMBER_INFO_METHOD	"AddMemberInfo"	//4.0.22.504 69344
#define	END_TICKET_METHOD		"EndTicket"	//1.0.22.500 69344

#define LOYALTY_EMERALD_QUERY_REQUEST	"Query" 
#define LOYALTY_EMERALD_CANCEL_REQUEST	"Cancel"

//we will define struct that hold the 
//prom id and the censtoff
//this because in one pump there are severel grades
//and each grade contain diffrend  promotion
typedef struct {
	long	lPromID;//hold the prom id
	double	fTotalCentsOff;//hold the censt off
	long	lGradeNumber;
	double	fDeferralTotalCentsOff; //4.0.20.54 64528
	double	fDeferrableRewardAmount; //4.0.20.54 64528
	BOOL	bUseDeferral; //4.0.20.54 64528
	double	fValueToDisplay; //4.0.22.504 69344
	long	lOriginalUnitPrice; ///4.0.23.640 136174  //4.0.25.50
	long    lPluCode;
	CString sPromotionDescription;
	PromotionList    cPromotionsList;

}cPromotionStruct;

typedef struct {
	long	lPumpNumber;
	long	lNumOfPromotionID;
	cPromotionStruct sPromID[MAX_PROM_PER_PUMP];
	long    lRemainingValueToTrigger; // TD 466954
}sPromotionData;

typedef struct
{
	long lCode;
	CTime cTime;

} LoyaltyTimer;




enum LoyaltyCancelTime //Copient 55286 + Rtaps 59557
{
	NoCancel,
	CancelBeforeStartTicket,
	CancelAfterStartTicket
};

enum LoyaltyServerInUse //4.0.22.504 69344
{
	LPE,		//0
	COPIENT,	//1
	SPPS,		//2		//4.0.23.504 66705
	LPE_SA,		//3		//4.0.23.508 88350
	EPSILON,    //4
	EMERALD,		//5
	EPSILON_DISCOUNT //6
};

typedef struct
{
	NMP_TIME eNMPTime;
	NMP_TYPE eNMPType;
	char sPath[MAX_PATH];
}NMP_RegEntry;



class CFuelLoyaltyPS
{
public:
	CFuelLoyaltyPS();
	~CFuelLoyaltyPS();
	void		CreateLoyaltyInterface();
	inline void	SetFuelLoyaltyFlag(BOOL lFuelLoyaltyFlag) { m_lFuelLoyaltyFlag = lFuelLoyaltyFlag; };
	inline BOOL GetFuelLoyaltyFlag() { return m_lFuelLoyaltyFlag; };
	inline void	SetInitDllFlag(BOOL lInitLoyaltyFlag) { m_lInitDllFlag = lInitLoyaltyFlag; };
	BOOL		GetInitDllFlag();
	BOOL		InitFuelLoyaltyInterface(long lNumberOfPumps);
	BOOL		StartTicket(long lPumpNumber, PAY_AT_PUMP_INFO & pInfo);	//4.0.19.500 55286
	BOOL		RunQuery(long lPumpNumber, PAY_AT_PUMP_INFO & pInfo);	//4.0.22.504 69344

	void		StartTicketResult(BSTR strGradesXML);
	void		QueryResult(BSTR strGradesXML);	//4.0.22.504 69344
	void		GeneralResult(BSTR sXMLResult);	//4.0.22.504 69344
	void		GetTriggerPromResult(BSTR strTriggerPromXML, long lPumpNumber = -1);
	long		EndTicket(long lPumpNumber, BOOL bInitLoyalID = TRUE); //RFUEL-2065
	BOOL		AddMemberCard(long lPumpNumber, PAY_AT_PUMP_INFO & pInfo);	//4.0.22.504 69344

	BOOL		AddCarWashItem(long lPumpNumber, long lCarWashSaleStatus);
	BOOL		CarPromotionAccepted(long lPumpNumber);
	BOOL		AddItem(long lPumpNumber, long lTrsNum, OLA_STAT	& ola);
	void		ReleaseFuelLoyalty();
	void		GetErrorFromRtapsAndContinue(BSTR sXMLError);
	void		SetTotalDiscount(long lPumpIdx, long lVal);
	long		GetTotalDiscount(long lPumpIdx);
	void		SetPrintReceipt(long lPumpIdx, BOOL bVal) { bPrintReceipt[lPumpIdx] = bVal; };
	BOOL		GetPrintReceipt(long lPumpIdx) { return bPrintReceipt[lPumpIdx]; };
	void		CancelTicket(long lPumpNumber, BOOL bPAKAddItem = FALSE, BOOL bInitLoyalID = TRUE);	//4.0.23.991 231113 //4.0.28.500
	void		GetErrorAfterFuelAndContinue(BSTR sXMLError);
	void		SimulStartOlaSession(long lPumpNumber, OLA_STAT	& ola);
	void		SimulStartFuelingSession(long lPumpNumber, OLA_STAT & ola, CARD_SALE_ALL3 * pcCardSale); //Copient 55286
	void		CancelLoyaltyTrs(PAY_AT_PUMP_INFO & cTmpInfo, long	lPumpNumber, OLA_STAT &  ola, BYTE sTranStatus = CD_FORCE_COMPLETION_ZERO);
	void		PAKResult(PAY_AT_PUMP_INFO & cTmpInfo, long	lPumpNumber, OLA_STAT &  ola, BOOL bCheckDeferralDiscountFirst = FALSE);	//4.0.20.54 64528
	void		GetPromotionId(long lGradeCode, long lPumpNumber, long *  lPromId, double *  lCentsOff);
	void		GetPricesByGradeCode(long lPumpNumber, long lGradeCode, long *lFullServicePrice, long *lSelfServicePrice);
	void		InitSavedLoyaltyInfo(long lPumpNumber); //4.0.20.360 68429 //4.0.21.12
	BOOL		IsPaymentFirstInFlow() { return m_bLoyaltyFlowPaymentFirst; };
	void		SetLoyaltyFlowPaymentFirst(BOOL bVal) { m_bLoyaltyFlowPaymentFirst = bVal; };
	//4.0.19.502 Copient + Rtaps start 59557
	BOOL		IsOLAInLoyaltyNMPStateBeforeStartTicket(long lPumpNumber);
	BOOL		IsOLAInLoyaltyNMPStateAfterStartTicket(long lPumpNumber);
	BOOL		IsOLAInLoyaltyNMPState(long lPumpNumber);
	LoyaltyCancelTime	GetCancelTrsTime(long lPumpNumber) { return m_eaCancelTrs[lPumpNumber - 1]; };
	void		SetCancelTrs(long lPumpNumber, LoyaltyCancelTime eVal);	//4.0.20.730 74977 {m_eaCancelTrs[lPumpNumber - 1] = eVal;}; //4.0.22.506
	BOOL		IsCancelTrs(long lPumpNumber) { return ((m_eaCancelTrs[lPumpNumber - 1] == CancelBeforeStartTicket) || (m_eaCancelTrs[lPumpNumber - 1] == CancelAfterStartTicket)); }
	long		GetDepCode() { return m_lDepCode; };
	void		SetDepCode(long lVal) { if (lVal >= 0) m_lDepCode = lVal; };
	double		GetDeferralValueToDisplayOnPump(long lPumpNumber);//4.0.22.504 69344
																  //4.0.20.54 start 64528
	double		GetDeferralDiscountForPump(long lPumpNumber);
	BOOL		IsDeferralDiscountSelected(long lPumpNumber);
	void		UpdateDeferralDiscountPerGrade(long lPumpNumber, PAY_AT_PUMP_INFO & cTmpInfo);
	//4.0.20.54 end
	//4.0.19.502 Copient + Rtaps end 59557
	void		CalculateLoyaltyValues(PAY_AT_PUMP_INFO & cTmpPapInfo, long lPumpNumber, CPumpTransact & trs); //4.0.20.300 //4.0.21.0
	BOOL		IsCopientFlowControlInUse() { return m_dwLoyaltyEngineType == COPIENT; }; //4.0.22.508 74810
	BOOL		IsEmeraldLoyaltyEnginelInUse() { return m_dwLoyaltyEngineType == EMERALD; }; //4.0.22.508 74810
	BOOL		IsEpslionLoyaltyEnginelInUse() { return m_dwLoyaltyEngineType == EPSILON; }; //4.0.22.508 74810
	DWORD		GetLoyaltySrvInUse() { return m_dwLoyaltyEngineType; };
	BOOL		HandleAddMemberCardResult(long lPumpNumber, OLA_STAT *pOla, long lResult);	//4.0.22.504	69344 //4.0.23.390 116544 //4.0.24.90
	int			GetGradeIndexInPumpPromotionsArray(long lPumpNumber, long lGradeCode); ///4.0.23.640 136174 //4.0.25.50
	long		GetSavedOriginalUnitPrice(long lPumpNumber, long lGradeCode); ///4.0.23.640 136174 //4.0.25.50
	void		FillInPromotionsArray(PumpItems * cTmpInfo, long lPumpNum); //4.0.25.501 140984
	void		SetdeferralDiscount(long lPumpNumber, double lDeferralDiscount);
	BOOL		RunPhoneLookUp(long lPumpNumber, BYTE * sPhoneNumber);
	void		PhoneLookUpResult(long lPumpNumber, int iRetCode, BYTE * strClubCard);
	void		SetAddMemberInfo(long lPumpNumber, long lVal); // TD 305114
	BOOL		AddMemberInfo(long lPumpNumber);
	void		ClearTimer(long p);
	void        SetStartTicketTime(long p, SYSTEMTIME st);
	void		SetTimer(long p, long lCode);
	void		GetTimer(IN long lPumpIndex, OUT long & lCode, OUT CTime & cTime);
	CString		GetStringTimeoutCode(long lTimeoutCode); // 4.0.23.2100
	BOOL		IsNeedToAddMemberInfo(long lPumpNumber);
	void		PrintLoyaltyStateToLog(long lPumpNumber, int iCurrState, int iNextState);
	BOOL		HandleAddMemberInfoResult(long lPumpNumber);
	BOOL		IsManualDeferral(long lPumpIndex);
	BOOL		HandleEndTicketResult(long lPumpNumber);
	void		UpdateCentsOffPerGrade(long lPumpNumber, PAY_AT_PUMP_INFO & cTmpInfo);
	void		TargetMessageResult(long lPumpNumber, char * sInTargetMsg); // CR 312183
	BOOL		GetTargetMessage(long lPumpNumber); // CR 312183
	BOOL		CreateAddMemberCardXML(PAY_AT_PUMP_INFO & pInfo, char *  strOutXml);	//4.0.22.504 69344
	BOOL		CreateAddItemXML(PAY_AT_PUMP_INFO  & cTmpPapInfo, CComBSTR & strOutXml, PumpItems & cTmpInfo, CPumpTransact & trs, long lPumpNumber);
	BOOL		AddItemResult(long lPumpNumber, char * bstrItemPromotion);
	BOOL		PromotionResult(long lPumpNumber, long lEarnValue, BSTR bstrItemPromotion);
	void		SetParam(long lPumpNumber);
	BOOL		GetMemberInfo(long lPumpNumber); // CR 405435
	BOOL		HandleGetMemberInfoResult(long lPumpNumber, char * sMemberInfoResonseXml);
	LONG		GetRemainingBalanceToTrigger(long lPumpIndex) { return m_Promotion[lPumpIndex].lRemainingValueToTrigger; } // TD 466954
	PromotionList GetPromotionsListByPosition(long lPumpIndex, long lPositionIndex) { return m_Promotion[lPumpIndex].sPromID[lPositionIndex].cPromotionsList; } // MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
	CTicketPromotionlist GetTicketPromotionlist() { return m_TicketPromotionlist; }

	double		GetTotalCentsOff(long lPumpIndex, long lPositionIndex) { return m_Promotion[lPumpIndex].sPromID[lPositionIndex].fTotalCentsOff; } // MichaelPo - US 434243 - [FUEL] - Support cents off discounts - PAY AT PUMP - Before Fueling
	BOOL		CreateAddItemEmeraldXML(CComBSTR& bstrOutXml, PumpItems& cItems, long lPumpNumber, CPumpTransact trs, double fTotalDiscount);

	DWORD				m_dwLoyaltyEngineType;
	DWORD				m_dwLoyaltyInterfaceType;
	DWORD				m_dwImmediateDiscounts;
	DWORD				m_dwUseCancelTicket2;
	DWORD				m_dwVirtualMemberAccountId;
	DWORD				m_dwUnlockLoyaltyMember;
	DWORD				m_dwCheckMemberStatus;

	double		GetDeferralReward(long lPumpNumber) { return m_Promotion[lPumpNumber - 1].sPromID->fDeferrableRewardAmount; }
	LoyaltyRestrctionMap		m_dwFuelLoyaltyCardsMap;
	LoyaltyRestrctionMap		m_dwFuelLoyaltyMembersMap;
	long GetNacsCodeByGradeCode(PumpItems & pPumpItem, long lGradeCode);
	INT64 GetAdditionalCodeByGradeCode(PumpItems & pPumpItem, long lGradeCode); //4.0.23.170 102319 //4.0.24.60 103546
	void  SetMinLoyaltyUnitPrice(long val);
	long  GetMinLoyaltyUnitPrice();



private:
	BOOL			m_lFuelLoyaltyFlag;//if we have in the system rtaps
	BOOL			m_lInitDllFlag;//if we init the pumps in the dll 
	sPromotionData	m_Promotion[MAX_PUMPS_64];
	CTicketPromotionlist m_TicketPromotionlist;
	long			lTotalDiscount[MAX_PUMPS_64];//lpe
	BOOL			bPrintReceipt[MAX_PUMPS_64];
	BOOL			m_bLoyaltyFlowPaymentFirst;
	long			m_lDepCode;
	long			m_lSegmentId[MAX_PUMPS_64];
	long            m_lPosSeqenceNumber[MAX_PUMPS_64]; // TD 405456

	BOOL			m_bIsQueryWithAuthoirze;

	CLoyaltyCommandExecuter * m_cLoyaltyCommandExecuter;
	long m_lMinLoyaltyUnitPrice; // (RFUEL-3194)  contains the minimal unit price after loyalty discounts are applied


	BOOL CreatePumpsInitXML(long    lPumpNumber, char	*   strOutXml, int iMaxSizeOfXml);
	BOOL CreatePumpsGradeXML(long lPumpNumber, PumpItems * cTmpInfo, CComBSTR & bstrXML, PAY_AT_PUMP_INFO & pInfo, BOOL bAddCarWashBeforeFueling = FALSE); //4.0.19.500 55286

	void SetCardSegment(PAY_AT_PUMP_INFO &pInfo, CXMLInterface &xmlInitDoc, CXMLPointers &pCurrElement);
	void SetLanguageId(PAY_AT_PUMP_INFO &pInfo, CXMLInterface &xmlInitDoc, CXMLPointers &pCurrElement);
	void SetLoyatyTrack2(PAY_AT_PUMP_INFO &pInfo, CXMLInterface &xmlInitDoc, CXMLPointers &pCurrElement);
	BOOL CreateQueryXML(long lPumpNumber, PumpItems * cTmpInfo, char *  strOutXml, PAY_AT_PUMP_INFO & pInfo, BOOL bAddCarWashBeforeFueling = FALSE); //4.0.22.504 69344

	long GetItemPrice(long lPumpNumber, PumpItems * pInfo, int iItem);
	long ParsePromotionsFromRtaps(BSTR bstrGradesXML);
	void ParsePromotionsError(long lPumpNumber); // TD 406362
	BOOL ParseTriggerPromFromRtaps(BSTR bstrTriggerPromXML, long lPumpNumber = -1);

	BOOL SaveTriggerPromoationsXml(long lPumpNumber, PAY_AT_PUMP_INFO &cTmpInfo, CXMLInterface &xmlInitDoc);
	void UpdateTriggerDiscountWithExtraInfoQdx(long lPumpNumber, long lTrsNumber, long &lReceiptExtraRecIndex, long lTotalReward, PAY_AT_PUMP_INFO &cTmpInfo);
	void ParseTotalDiscountFromTriggerXml(long lPumpNumber, long &lTotalReward, CXMLInterface &xmlInitDoc, CXMLPointers &pPromoSrvElement, long lTrsNumber, PAY_AT_PUMP_INFO &cTmpInfo);
	void SetLoyaltyPointsToCardSale(long lPumpNumber, long lTrsNumber, PAY_AT_PUMP_INFO &cTmpInfo);
	void SaveLoyaltyPrinitingsToReceipt(char * &sReceiptText, long lReceiptExtraRecIndex, PAY_AT_PUMP_INFO &cTmpInfo);



	long GetGradeCodeByAdditionalCode(PumpItems & pPumpItem, INT64 lAdditionalCode); //4.0.23.170 102319 //4.0.24.60 103546

	void UpdatePumpWithLoyaltyVolumeLimit(PAY_AT_PUMP_INFO &pInfo, long lPumpNumber); //4.0.20.300 //4.0.21.0
	BOOL HandleStartTicketResult(long lPumpNumber, CXMLInterface &xmlInitDoc);	//4.0.22.504	69344
	BOOL HandleAddMemberCardResult(long lPumpNumber, long lResult);	//4.0.22.504	69344
	void AddTotalTenderAmount(long lPumpNumber, PAY_AT_PUMP_INFO & cTmpPapInfo, CXMLInterface &xmlInitDoc, CXMLPointers & pPumpsPropElement, CXMLPointers  &pCurrElement, long lNacsCode, CPumpTransact * pTrs);
	void AddTenderToXml(long lPumpNumber, CXMLInterface &xmlInitDoc, CXMLPointers & pRootElement, long lTenderNum, DOUBLE dSecondCardTenderAmount);
	BOOL CreateAddCarWashItemXML(PAY_AT_PUMP_INFO & cTmpPapInfo, CComBSTR& bstrAddItemXml, long	lPumpNumber); //4.0.19.502 55286 Copient


	void GetCarWashDetalisFromCardSaleData(long lPumpNumber, __int64 &lPluCode, PAY_AT_PUMP_INFO &cTmpPapInfo, long &lPrice, long &lQuantity);
	BOOL CreatePromotionAccpectedXML(PAY_AT_PUMP_INFO & cTmpPapInfo, CComBSTR& bstrPromotionXml, long	lPumpNumber);
	void GetCarWashNacsCode(long lProdCode, long &lNacsCode);
	//	void SimulNeedMoreParamMmbrCardFailMsg(PAY_AT_PUMP_INFO & cTmpInfo, long lPumpNumber, OLA_STAT &  ola, char *sMsgToPrint);	//4.0.20.53
	BOOL ParseCustomerData(long	lPumpNumber, char * sCustXml, PAY_AT_PUMP_INFO  & cTmpInfo, BOOL & ActiveMember);

	BOOL IsMemberActive(long lPumpNumber, CXMLInterface & xmlInitDoc, CXMLPointers pLoyaltyInfoNode, PAY_AT_PUMP_INFO & cTmpInfo);

	BOOL ParseReceiptTextXml(CXMLInterface * pInXML, CXMLPointers * pXMLReceiptText, char *  strOutText, long lZoneNum);	//LPE
	BOOL ParseTargetMessageTextXml(CXMLInterface * pInXML, CXMLPointers * pSecetionElement, char * strOutText);
	BOOL ParseCommunicationRewardTextXml(CXMLInterface * pInXML, CXMLPointers * pSecetionElement, char * strOutText);
	BOOL ParseVocher(CXMLInterface * pInXML, CXMLPointers * pXMLReceiptText, char *  strOutText, long lZoneNum);	//LPE
	BOOL ParseReceiptPrintingFromRtaps(char * strReceiptPrintingXML, char ** sOutZoneText);	//LPE
	void SetZoneIndexAccordingToLoyaltyMessageType(CXMLInterface &xmlInitDoc, CXMLPointers pPrintMessagesElement, int iNumberOfZones, OUT int &iZoneIndex);
	void ParseSppsPromotions(CXMLInterface &xmlInitDoc, CXMLPointers pChildNode, CXMLPointers &pListNodes, char *& sAllText);
	long GetTenderNumber(PAY_AT_PUMP_INFO & cTmpPapInfo, BOOL bFirstTender = TRUE);	//4.0.20.50
	void ParsePromotions(long lPumpNumber, CXMLInterface &xmlInitDoc, CXMLPointers pRootNode, long lUseAdditionalCode, CXMLPointers pAdditionalCodeNode, PAY_AT_PUMP_INFO &cTmpInfo);
	void ParseCarWashPromotions(long lPumpNumber, CXMLInterface &xmlInitDoc, CXMLPointers pRootNode, long lUseAdditionalCode, CXMLPointers pAdditionalCodeNode, PAY_AT_PUMP_INFO &papInfo); //RFUEL - 2142
	void SavePromotions(long lPumpNumber, int iPositionIndex, PAY_AT_PUMP_INFO &cTmpInfo, long lGradeCode, CPromotion cPromotion, long lPluCode, CString sPromDescription);
	long GetNumberOfPromotions(long lPumpIndex, long lGrdaeCode);
	long CalclateDiscountForForceXml(long lPumpNumber, PAY_AT_PUMP_INFO &cPapInfo, long lPromotionDiscount, long lPromotionID, CPumpTransact &cTrs);
	void ForceFuelItem(long lNacsCode, CXMLInterface &xmlInitDoc, CXMLPointers pCurrElement3, CXMLPointers pCurrElement, CPumpTransact &trs, long lPumpNumber, PAY_AT_PUMP_INFO &cTmpPapInfo, long lOriginUnitPrice, CXMLPointers pChildNode);


	void PrintForceDiscounts(long lPumpNumber, PAY_AT_PUMP_INFO &cPapInfo);
	PromotionList GetPromotionsList(long lPumpIndex, long lGradeCode);
	long CreateForceXmlFuelItem(long lPumpNumber, PAY_AT_PUMP_INFO &cPapInfo, CPumpTransact & cTrs, long lOriginUnitPrice, long lPluCode, PromotionIterator & itPromotion, char * szOutXml);
	long CreateForceXmlCarWashItem(long lPumpNumber, PAY_AT_PUMP_INFO &cPapInfo, long lPluCode, char * szOutXml);
	long CalclateUnitPriceAfterDiscountForForceXml(CPumpTransact &cTrs);
	BOOL SetPumpNumberReceivedFromLPE(long &lPumpNumber, long &p, CXMLInterface &xmlInitDoc, CXMLPointers &pNode);
	BOOL ParseTargetMessage(char * sTargetMessageXml, char * sOutText);
	void SetLoyalIdToTrackNumber(PAY_AT_PUMP_INFO &pInfo, CXMLInterface &xmlInitDoc, CXMLPointers pTrack2Loyalty);				// 4.0.32.??? TD 405435
	void SetCardHashNumberToTrackNumber(PAY_AT_PUMP_INFO & pInfo, CXMLInterface & xmlInitDoc, CXMLPointers pTrack2Loyalty);		// 4.0.32.??? TD 405435
	long GetTimerCode(IN long lPumpIndex);
	LoyaltyCancelTime	m_eaCancelTrs[MAX_PUMPS_64];	// Copient 55286 + Rtaps 59557

	BOOL IsVoucher(const char * sPrintingType) { return strcmp(sPrintingType, "Voucher") == 0; }
	BOOL IsRewardSimulation(const char * sPrintingType) { return strcmp(sPrintingType, "RewardSimulation") == 0; }
	BOOL IsLoyaltySummary(const char * sPrintingType) { return strcmp(sPrintingType, "LoyaltySummary") == 0; }
	BOOL IsTargetMessage(const char * sPrintingType) { return strcmp(sPrintingType, "TargetMessage") == 0; }
	BOOL IsIncentiveMessage(const char * sPrintingType) { return strcmp(sPrintingType, "IncentiveMessage") == 0; }
	BOOL IsCommunicationReward(const char * sPrintingType) { return strcmp(sPrintingType, "CommunicationReward") == 0; }
	void ParseTerminalMessagesFromRtaps(char * strXML, PAY_AT_PUMP_INFO &cTempInfo);

	BOOL CheckCardRestriction(const int &iCardStatus);
	BOOL CheckMemberRestriction(const int &iMemberStatus);
	void SetVolumeLimitToLoyaltyMin(long lPumpNumber, CXMLInterface &xmlInitDoc, PAY_AT_PUMP_INFO &cTmpInfo);  // RFUEL-2445 
																											   //void ParseLPEBarcodes(long lPumpNumber, CXMLInterface &xmlInitDoc, CXMLPointers &pPromoSrvElement, PAY_AT_PUMP_INFO &cTmpInfo);
																											   //void CodeBarCodeType(CString * sBarcodeType);
																											   //void CopyLoyaltyBarcodesToCardSaleData(PAY_AT_PUMP_INFO &cTmpInfo, CString * sBarcode, CString * sBarcodeDescription, CString * sBarcodeEndDate, CString * sBarcodeType);

};

#endif //__FUEL_LOAYALTY_H__
