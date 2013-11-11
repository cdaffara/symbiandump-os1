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
// PP_MMC.H
// 
//

#ifndef __PP_MMC_H__

#if defined(_UNICODE) && !defined(UNICODE)
	#define UNICODE
#endif

#include <mmc.h>
#include <emulator.h>

//	MultiMedia Card Controller - platform specific layer.

GLREF_C TInt MapLastErrorEpoc();
GLREF_C TMMCErr MapLastErrorMmc();

const TInt KMediaChangeTickInterval=20000;	// Units 1uS


const TInt KMax_TotalWinsCardSlots=8; ///< Maximal number of MMC cards supported by the emulator
const TInt KMax_TotalWinsCards=(KMax_TotalWinsCardSlots+1); ///< Maximal number of MMC slots, supported by the emulator. Need 2 cards for slot 0

const TInt KDefault_TotalWinsCardSlots=2; ///< Default number of MMC cards supported by the emulator
const TInt KDefault_TotalWinsCards=(KDefault_TotalWinsCardSlots+1); ///< Default number of MMC slots, supported by the emulator. Need 2 cards for slot 0


const TUint32 KMMCWinsCardOCRValue = 0x00FFFF00;

class TWinsCardInfo
	{
public:
	void GetCSD(TUint8* aResp) const;
public:
	TCID iCID;
	TMediaPassword* iPWD;				// PWD_LEN calculated from PWD.
	TBool iIsLocked;					// Could use iCurrentCardIsLocked
	TMMCardStateEnum iState;			// Simulation of card's current state
	HANDLE iWinHandle;
	TRCA iRCA;
	
	TBool iForceMount;
	TUint iForceMountCSIZE;             // mmc card size field
	TUint iForceMountCSIZE_MULT;        // mmc card size field
	};

class DWinsMMCStack : public DMMCStack
	{
private:
	DWinsMMCStack(TInt aBus, DMMCSocket* aSocket);
	TInt Init();
private:
	virtual void  MachineInfo(TMMCMachineInfo& aMachineInfo);
public:
	virtual void  AdjustPartialRead(const TMMCard* aCard, TUint32 aStart, TUint32 aEnd, TUint32* aPhysStart, TUint32* aPhysEnd) const;
	virtual void  GetBufferInfo(TUint8** aMDBuf, TInt* aMDBufLen);

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
		EStkFOCNotSelCard = 0x011, EStkFOCMultiSel, EStkFOCNoneSel
		};
	
    static TInt  TotalMDiskSize;		// mmc card size for emulator
	static TUint CSIZE;					// mmc card size field
	static TUint CSIZE_MULT;			// mmc card size field
    
    static TInt TotalWinsMMC_CardSlots; // total number of MMC card slots for the emulator
    static TInt TotalWinsMMC_Cards;     // total number of MMC cards for the emulator

    static void Panic(TWinsMMCPanic aPanic);

private:
	TUint8* iMDBuf;
	TInt iMDBufLen;

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
	TMMCErr ModifyCardCapabilitySM();
private:
	TInt SetupSimulatedCard(TInt aCardNum);						// init
	TInt CreateBinFileForCard(TInt aCardNum, HANDLE* aHandle, TBool aCreateNew = EFalse);
	TInt GetTargetSlotNumber(TBool anRCASupplied, const TRCA& anRCA);
	TInt FindAnyCardInStack(TMMCardStateEnum aState);
	TInt FindFirstCardInStack(TMMCardStateEnum aState);
	TInt FindOneCardInStack(TMMCardStateEnum aState);
	void SetupDiskParms(TUint);
	TMMCErr ReadWriteData(TInt aCardIdx, TUint8* aDataP, TUint32 aLength, TMMCCmdDirEnum aDir);
private:
	TWinsCardInfo* iCardPool[KMax_TotalWinsCards]; 		// All the cards created
	TWinsCardInfo* iCardInfo[KMax_TotalWinsCardSlots]; 	// Cards currently present (1 per slot)
	TBool iCMD42Failed;
	TUint32 iBytesToTransfer;
	friend class DWinsMMCMediaChange;
	friend class TMMCardControllerInterfaceWins;			
	};

class DWinsMMCStack;
class DWinsMMCMediaChange : public DMMCMediaChange
	{
public:
	DWinsMMCMediaChange(TInt aMediaChangeNum);
	virtual TInt Create();
	virtual void ForceMediaChange();
	virtual void DoDoorOpen();
	virtual void DoDoorClosed();
	virtual TMediaState MediaState();
	inline void SetStackP(DWinsMMCStack* aStackP) {iStackP=aStackP;}	
protected:
	void DoorOpenService();
private:
	static void Tick(TAny *aPtr);
	void TickService();
	void Enable();
	void Disable();
	static void MediaChangeCallBack(TAny *aPtr);
	static void MediaChangeDFC(TAny* aPtr);
private:
	TTickLink iTickLink;
	TInt iDoorClosedCount;
	TBool iMediaChangeEnable;
	TInt iMediaDoorCloseReload; 	// Units: In theory-20ms, Actual-100ms  
	DWinsMMCStack* iStackP;
	friend class DWinsMMCStack;
	};

class DWinsMMCPsu : public DMMCPsu 
    {
public:
    DWinsMMCPsu(TInt aVccNum, TInt aMcId);
    virtual TInt DoCreate();
	virtual void DoSetState(TPBusPsuState aState);
	virtual TInt VoltageInMilliVolts();
private:
	virtual void DoCheckVoltage();
    virtual void PsuInfo(TPBusPsuInfo &anInfo);
    };

#endif	// #ifndef __PP_MMC_H__
