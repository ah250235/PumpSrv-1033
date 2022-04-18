#pragma once

/////////////////////////////////////////////////////////////////////////////
// CPumpTransact


// #include "commonDef.h"
// #include "PumpSrvStructures.h"
// 
// namespace PS
// {
// 	enum PumpResExt;
// 	enum PumpReservation;
// 	enum TrsStatus;
// }
// 
// enum GPIRetCode;
// 
// typedef struct _TAG_PUMP_TRANSACT
// {
// 	BYTE			sNumber[STR_LONG_VAL5];//LONG			lNumber;
// 	BYTE			sStatus[STR_LONG_CODE];//TrsStatus		lStatus;
// 	BYTE			sOnOffer[STR_LONG_CODE];//LONG			lOnOffer;
// 	BYTE			sRawVolume[STR_LONG_VAL10];//LONG			lRawVolume;
// 	BYTE			sRoundedVolume[STR_LONG_VAL10];//LONG			lRoundedVolume;
// 	BYTE			sRoundedValue[STR_LONG_VAL10];//LONG			lRoundedValue;
// 	BYTE			sGradePrice[STR_LONG_VAL10];//LONG			lGradePrice;
// 	BYTE			sNozzle[STR_LONG_CODE];//SHORT			nNozzle;
// 	BYTE			sGrade[STR_LONG_CODE];//SHORT			nGrade;
// 	BYTE			sLevel[STR_LONG_CODE];//SHORT			nLevel;
// 	BYTE			sPump[STR_LONG_CODE];//LONG			lPump;
// 	BYTE			sMode[STR_LONG_CODE];//PumpMode		lMode;
// 	BYTE			sRes[STR_LONG_CODE];//PumpReservation	lRes;
// 	BYTE			sResExt[STR_LONG_VAL5];//LONG			lResExt;
// 	BYTE			sLinkNumber[STR_LONG_VAL10]; // POS ticket Link (PrePay), OLA Session Link (PAP)
// 	BYTE			sLinkFlags[STR_LONG_CODE];//SHORT			nLinkFlags;
// 	BYTE			sActiveReadNumber[STR_LONG_VAL5];
// 	BYTE			sActiveShiftNumber[STR_LONG_VAL5];
// 	BYTE			sMisc[STR_LONG_VAL5];
// }	TAG_PUMP_TRANSACT;
// 
// 
// class CPumpTransact : public PumpTransactBase, public CObject
// {
// public:
// 	GPIRetCode SetStatePaid(long lRequestSource);
// 	GPIRetCode SetStateAccepted(long lRequestSource);
// 	GPIRetCode SetStateForcedRelease(long lRequestSource);
// 	GPIRetCode SetStateRelease(long lRequestSource);
// 	GPIRetCode SetStateOnOffer(long lRequestSource);
// 
// 
// 	void UpdateTime();
// 	CTime GetTime() const;
// 	long GetLinkNumber() const;
// 	short GetLinkFlags() const;
// 	void SetLink(short nLinkFlags, long lLinkNumber);
// 
// 	CPumpTransact();
// 	CPumpTransact(long lPumpNumber);
// 	CPumpTransact(const PumpTransactBase& pumpTrs);
// 	CPumpTransact(const CPumpTransact& pumpTrs);
// 	virtual ~CPumpTransact();
// 
// 	void		operator =(TAG_PUMP_TRANSACT& tagPumpTrans);
// 	TrsStatus	GetStatus()	const;
// 	PumpReservation GetRes() const;//4.0.9.506 //amram
// 	void		GetTag(void *pBuff, long lVersion = 1, long lPreSet = 0, long lGrade = 0, long lGrade2 = 0) const; //4.0.9.507
// 	LONG		GetOnOffer() const;
// 
// 	CPumpTransact(long lTrsNumber, long lActiveShiftNumber); // started transaction
// 	void		Clear();
// 	void		operator =(const CPumpTransact& pumpTrs);
// 	void		SetStatus(TrsStatus lStatus);
// 	void		UpdateRolling(LONG newVolume, LONG newValue);
// 	void		SetOnOffer(LONG lOnOffer);
// 
// };


