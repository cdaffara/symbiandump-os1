// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// PP_SDC.H
// 
//

#ifndef __PP_SDC_H__
#define __PP_SDC_H__
#if defined(_UNICODE) && !defined(UNICODE)
	#define UNICODE
#endif

#include <sdiocard.h>
#include <emulator.h>

GLREF_C TInt MapLastErrorEpoc();
GLREF_C TMMCErr MapLastErrorMmc();

const TInt KMediaChangeTickInterval=20000;	// Units 1uS

const TInt KTotalWinsCardSlots=2;
const TInt KTotalWinsCards=(KTotalWinsCardSlots+1); // Need 2 cards for slot 0

const TUint32 KMMCWinsCardOCRValue = 0x00FFFF00;

const TInt KSDMediaAreasPerCard = 3;
const TInt KSDUserArea = 0;
const TInt KSDProtectedArea = 1;
const TInt KSDIOArea = 2;

const TUint32 KWinsSdioFunctionCount = 3;
const TUint8  KWinsSdioFunctionMask  = 0x0A;	/** TODO: Non-Contuguous functions not yet supported */
const TBool	  KWinsSdioMemoryPresent = ETrue;

const TUint32 KFunctionEnableDelay_uS = 1000000;	// 1S Second Function Enable Delay


class TWinsCardInfo
	{
public:
	void GetCSD(TUint8* aResp) const;
public:
	TCID iCID;
	TMediaPassword* iPWD;				// PWD_LEN calculated from PWD.
	TBool iIsLocked;					// Could use iCurrentCardIsLocked
	TMMCardStateEnum iState;			// Simulation of card's current state
	HANDLE iAreaHandles[KSDMediaAreasPerCard];
	TRCA iRCA;
	TBool iIsSDCard;
	TInt iBusWidth;
	};

class DWinsSDIOStack : public DSDIOStack
	{
private:
	DWinsSDIOStack(TInt aBus, DMMCSocket* aSocket);
	TInt Init();
private:
	virtual void MachineInfo(TMMCMachineInfo& aMachineInfo);
public:
	virtual void AdjustPartialRead(const TMMCard* aCard, TUint32 aStart, TUint32 aEnd, TUint32* aPhysStart, TUint32* aPhysEnd) const;
	virtual void GetBufferInfo(TUint8** aMDBuf, TInt* aMDBufLen);
	virtual void AddressCard(TInt aCardNumber);

	enum TWinsMMCPanic
		{
	    EWinsMMCLidOpenIntBind=0,
		EWinsMMCBadMachineInfo=1,
		EWinsMMCMediaChangeTickFault = 2,
		EWinsMMCCorruptCommand = 3,
		EWinsMMCLockAttempt = 4,
		EWinsMMCAPRNotSupp = 5,
		EWinsMMCAPRRange = 6,
		EWinsMMCAPRBoundary = 7,

		EStkFFCNotSelCard = 0x010, EStkFFCNoneSel,
		EStkFOCNotSelCard = 0x020, EStkFOCMultiSel, EStkFOCNoneSel,
		EStkIMCBadStateCmd2 = 0x30, EStkIMCBadStateCmd3, EStkIMCCmd6InvalidWidth,
			EStkICMACMD13NotSD, EStkICMACMD22NotSD,
		};
    static void Panic(TWinsMMCPanic aPanic);

	static TInt AccessIoEnable	(TInt aTargetCard, TInt aVal, TAny* aSelfP, TBool aRead, TUint8& aData);
	static TInt AccessCsaWindow (TInt aTargetCard, TInt aVal, TAny* aSelfP, TBool aRead, TUint8& aData);
	static TInt AccessCsaPointer(TInt aTargetCard, TInt aVal, TAny* aSelfP, TBool aRead, TUint8& aData);

private:
	// Stack service provided by ASSP layer
	void SetBusConfigDefaults(TMMCBusConfig&, TUint aClock);
	void InitClockOff();
	void ASSPReset();
	void ASSPDisengage();
	void DoPowerDown();
	// State Machine functions implemented in ASSP layer
	TMMCErr DoPowerUpSM();
	TMMCErr InitClockOnSM();
	TMMCErr IssueMMCCommandSM();

	void EnableSDIOInterrupt(TBool aEnable);
	void SetBusWidth(TUint32 aBusWidth);
	TUint32 MaxBlockSize() const;
	

private:
	TInt SetupSimulatedCard(TInt aCardNum);						// init
	TInt CreateBinFileForCard(TInt aCardNum, TInt aAreaNum, HANDLE* aHandle);
	TInt GetTargetSlotNumber(const TRCA& anRCA);
	TInt FindAnyCardInStack(TMMCardStateEnum aState);
	TInt FindFirstCardInStack(TMMCardStateEnum aState);
	TInt FindOneCardInStack(TMMCardStateEnum aState);

	static void EnableTimerCallback(TAny* aPtr);

private:
	TWinsCardInfo* iCardPool[KTotalWinsCards];		// all cards
	TWinsCardInfo* iCardInfo[KTotalWinsCardSlots];		// present cards
	TInt iAddressedCard;
	TBool iCMD42Failed;
	TInt iMBWOKBlocks;
	TUint8* iMDBuf;
	TInt iMDBufLen;

	NTimer iEnableTimer;

	friend class TSDCardControllerInterfaceWins;
	friend class DWinsMMCMediaChange;
	};

class DWinsMMCMediaChange : public DMMCMediaChange
	{
public:
	DWinsMMCMediaChange(TInt aMediaChangeNum);
	virtual TInt Create();
	virtual void ForceMediaChange();
	virtual void DoDoorOpen();
	virtual void DoDoorClosed();
	virtual TMediaState MediaState();
protected:
    void DoorOpenService();
private:
	static TInt MediaChangeDfc(TAny *aPtr);
	static void Tick(TAny *aPtr);
	void TickService();
	void Enable();
	void Disable();
	static void MediaChangeCallBack(TAny *aPtr);
	inline void SetStackP(DWinsSDIOStack* aStackP) {iStackP=aStackP;}	
private:
	TTickLink iTickLink;
	TInt iDoorClosedCount;
	TBool iMediaChangeEnable;
	TInt iMediaDoorCloseReload; 	// Units: In theory-20ms, Actual-100ms  
	DWinsSDIOStack* iStackP;
	friend class DWinsSDIOStack;
	};

class DWinsSDIOPsu : public DSDIOPsu 
    {
public:
    DWinsSDIOPsu(TInt aVccNum, TInt aMcId);
    virtual void Init();
	virtual void DoSetState(TPBusPsuState aState);
	virtual TInt VoltageInMilliVolts();
private:
	virtual void DoCheckVoltage();
    virtual void PsuInfo(TPBusPsuInfo &anInfo);
    };


#endif	// #ifndef __PP_SDC_H__
