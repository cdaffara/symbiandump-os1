/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#if !defined(__D_MMCSDIF_H__)
#include <e32cmn.h>
#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif
#include "MmcTestUtils.h"


#ifdef __DEBUG
	#define _MMCSDLDDLOGGING   // Enable LDD logging
#endif

#if defined( _MMCSDLDDLOGGING )
	 #define MMCSDLOG(A) Kern::Printf(A)
	 #define MMCSDLOG2(A, B) Kern::Printf(A, B)
	 #define MMCSDLOG3(A, B, C) Kern::Printf(A, B, C)
	 #define MMCSDLOG4(A, B, C, D) Kern::Printf(A, B, C, D)
	 #define MMCSDLOG5(A, B, C, D, E) Kern::Printf(A, B, C, D, E)
	 #define MMCSDLOG6(A, B, C, D, E, F) Kern::Printf(A, B, C, D, E, F)
#else
	 #define MMCSDLOG(A)
	 #define MMCSDLOG2(A, B)
	 #define MMCSDLOG3(A, B, C)
	 #define MMCSDLOG4(A, B, C, D)
	 #define MMCSDLOG5(A, B, C, D, E)
	 #define MMCSDLOG6(A, B, C, D, E, F)
#endif


_LIT(KLddName, "MMCSDTestCtrlInf");

//Card Constants
const TInt KMaxCardsPerStack=2;
const TInt KSectorSizeInBytes=512;
const TInt KMaxBlocklengthInBytes=1024;
const TInt KSectorSizeShift=9;
const TInt KDrvBufSizeInSectors=8;

const TInt KStackNo=0;

//Used to check erase command is supported by the card.
const TUint32 KMMCEraseClassCmdSupported=KBit0;

//Used to check group erase command is supported by the card.
const TUint32 KMMCEraseGroupCmdSupported=KBit1;


/**
 * Card power supply information
 *
 */
class TPBusPsuInf
	{
public:

	/**	Indicates the voltage level, or range of voltages supported.
		Reperesented in OCR (Operation Condition Register) */
	TUint iVoltageSupported;

   	/**	Maximum Current Supplied by the PSU */
	TInt iMaxCurrentInMicroAmps;

	/**	PSU Voltage level within expected limit while turning ON */
	TUint iVoltCheckInterval;

	/**	VCC voltage when ON */
	TUint iVoltCheckMethod;

    /**	Bus not locked timeout period,in seconds,when no clients are registered */
    TInt iNotLockedTimeOut;

	/**	Bus inactivity timeout period, in seconds, when clients are registered */
	TInt iInactivityTimeOut;

	};
	
/**
 * MMC card Version
 *
 */
class TCapsMmcIfV01
	{
public:
	TVersion version;
	};


/**
 * Class for MMCSD controller interface to test MMC/SD Cards
 *
 */
class RMMCSDTestControllerInterface : public RBusLogicalChannel
	{
public:
	enum {EMajorVersionNumber=1,EMinorVersionNumber=0,EBuildVersionNumber=1};
	enum TRequest
		{
		/** Read a sector */
		EReadSector,
		/** Write a sector */
		EWriteSector
		};
	enum TControl
		{
		/** Return whether SD card is in use */
		ESDCard,
		/** Read CSD register */
		EReadCSD,
		/** Read extended CSD register */
		EReadExtCSD,
		/** Get card info */
		ECardInfo,
		/** Return card object */
		ECard,
		/** Return card is present status */
		ECardIsPresent,
		/** Return card is ready status */
		ECardIsReady,
		/** Return card is locked status */
		ECardIsLocked
		};
	enum TPsuControl
        {
		/** Return psu object */
		EPsu,
    	/** Gets the PSU Information */
		EPsuInfo,
		/** Set Power Supply State */
		EPsuState,
		/**Set Psu Voltage in OCR format */
		EPsuVoltage,
		/** Read Power supply unit number */
		EPsuNum,
		/** Read the Media Change ID */
		EMediaChangeNum,
		/** PSU current limit safe level or Not */
		EPsuCurLimit,
		/** Gets the PBUS state */
		EPbusState,
		/**	Gets the Power Supply Status */
		EVccState,
		/** Checks whether this PSU is powering a bus containing a locked device*/
		EIsLocked,
		/** Checks whether the PSU is off*/
		EIsOff,
		/** Gets the maximum current (in microAmps) that the PSU is able to supply*/
		EMaxCurrentInMicroAmps,
		/** Resets inactivity and not-locked counts*/
		EResetInactivityTimer,
		/** Gets the voltage level, or range of supported voltage levels*/
		EVoltageSupported,
		/** Bus inactivity counter*/
		EMemInactivityCount,
		/** Bus not locked counter*/
		EMemNotLockedCount,
		/** Bus inactivity timeout period, in seconds, when clients are registered*/
		EMemInactivityTimeout,
		/** Bus not locked timeout period, in seconds, when no clients are registered*/
		EMemNotLockedTimeout,
		/** Indicates the voltage level, or range of voltages supported*/
		EMemVoltageSupported,
		/** The maximum current (in microAmps) that the PSU is able to supply*/
		EMemMaxCurrentInMicroAmps,
		/**Indicates whether the platform hardware has support for checking whether the voltage level*/
		EMemVoltCheckInterval
		};
	enum TMediaChangeControl
		{
		/** Return media change object */
		EMediaChange,
		/** Gets the MMC stack media state */
		EMediaStateInfo,
		/** Force Media Change */
		EForceMediaChange,
		/** Media Change Replay Count */
		EMediaChangeCounter
		};
	enum TSocketControl
        {
		/** Return socket object */
		ESocket
		/** DMMCSocket::AdjustPartialRead */
		,ESocketAdjustPartialRead
    	/** DMMCSocket::CardIsPresent */
		,ESocketCardIsPresent
		/** DMMCSocket::GetBufferInfo */
		,ESocketGetBufferInfo
		/** DMMCSocket::Init */
		,ESocketInit
		/** DMMCSocket::InitiatePowerUpSequence */
		,ESocketInitiatePowerUpSequence
		/** DMMCSocket::MachineInfo */
		,ESocketMachineInfo
		/** DMMCSocket::PrepareStore */
		,ESocketPrepareStore
		/** DMMCSocket::Reset1 */
		,ESocketReset1
		/** DMMCSocket::Reset2 */
		,ESocketReset2
		/** DMMCSocket::ResetInactivity */
		,ESocketResetInactivity
		/** DMMCSocket::Stack */
		,ESocketStack
		/** DMMCSocket::iState */
		,ESocketiState_Get
		/** DMMCSocket::iDoorOpened */
		,ESocketiDoorOpened_Get
		/** DMMCSocket::iStandby */
		,ESocketiStandby_Get
		/** DMMCSocket::iType */
		,ESocketiType_Get
		/** DMMCSocket::PowerUp */
		,ESocketPowerUp
       };
	enum TStackControl
        {
		/** Return stack object */
		EStack
		/** DMMCStack::AdjustPartialRead */
		,EStackAdjustPartialRead
		/** DMMCStack::AllocSession */
		,EStackAllocSession
		/** DMMCStack::BufferInfo */
		,EStackBufferInfo
		/** DMMCStack::CardP */
		,EStackCardP
		/** DMMCStack::DemandPagingInfo */
		,EStackDemandPagingInfo
		/** DMMCStack::EffectiveModes */
		,EStackEffectiveModes
		/** DMMCStack::GetBufferInfo */
		,EStackGetBufferInfo
		/** DMMCStack::HasCardsPresent */
		,EStackHasCardsPresent
		/** DMMCStack::HasSessionsQueued */
		,EStackHasSessionsQueued
		/** DMMCStack::Init */
		, EStackInit
		/** DMMCStack::InitStackInProgress */
		, EStackInitStackInProgress
		/** DMMCStack::MMCSocket */
		, EStackMMCSocket
		/** DMMCStack::MachineInfo */
		, EStackMachineInfo
		/** DMMCStack::MaxCardsInStack */
		, EStackMaxCardsInStack
		/** DMMCStack::PasswordStore read*/
		, EStackReadPasswordStore
		/** DMMCStack::PasswordStore write*/
		, EStackWritePasswordStore
		/** DMMCStack::ProgramPeriodInMs */
		, EStackProgramPeriodInMs
		/** DMMCStack::StackStop */
		, EStackStop
		/** DMMCStack::ReportPowerDown */
		, EStackReportPowerDown
		/** DMMCStack::ReportPowerUp */
		, EStackReportPowerUp
		/** see TEffectiveModesFunctionId enum */
		, EStackConfigureEffectiveModes
        };

	enum TEffectiveModesFunctionId {
		/** TMMCStackConfig::RemoveMode(TUint32 aMask) */
		EStackEffectiveModesRemoveMode = 1
		/** TMMCStackConfig::SetBusClockInKhz(TUint aParam) */
		, EStackEffectiveModesSetBusClockInKhz
		/** TMMCStackConfig:: SetBusClockInKhz(TUint aParam); */
		, EStackEffectiveModesSetBusyTimeOutInMcs
		/** TMMCStackConfig::SetCRCRetries(TUint aData)*/
		, EStackEffectiveModesSetCRCRetries
		/** TMMCStackConfig::SetDataTimeOutInMcs(TUint aParam) */
		, EStackEffectiveModesSetDataTimeOutInMcs
		/** TMMCStackConfig::TUint32 aMask*/
		, EStackEffectiveModesSetMode
		/** TMMCStackConfig::SetOpCondBusyTimeout(TUint16 aData); */
		, EStackEffectiveModesSetOpCondBusyTimeout
		/** TMMCStackConfig::SetPollAttempts(TUint aData); */
		, EStackEffectiveModesSetPollAttempts
		/** TMMCStackConfig::SetResponseTimeOutInTicks(TUint aParam); */
		, EStackEffectiveModesSetResponseTimeOutInTicks
		/** TMMCStackConfig:: SetTicksClockIn(TUint aParam); */
		, EStackEffectiveModesSetTicksClockIn
		/** TMMCStackConfig::SetTicksClockOut(TUint aParam); */
		, EStackEffectiveModesSetTicksClockOut
		/** TMMCStackConfig::SetTimeOutRetries(TUint aData); */
		, EStackEffectiveModesSetTimeOutRetries

        };
public:
	inline void Cancel();
	inline TInt Open(TInt aStack,const TVersion& aVer);
	inline TVersion VersionRequired() const;
	inline void Reset();
	// requests
	inline TInt ReadBlock();

	//DMMCPsu
	inline TInt Psu(TAny* aPsu);
	inline TInt PsuInfo(TPBusPsuInf& anInfo);
	inline TInt SetPsuState(TInt& aState);
	inline TInt SetPsuVoltage(TUint& aVol);
	inline TInt PsuNumber(TInt& aPsuNum);
	inline TInt MediaChangeID(TInt& aMCId);
	inline TInt PsuCurLimit(TBool& aSafe);
	inline TInt PbusState(TInt& aState);
	inline TInt VccState(TInt& aVccState);
	inline TInt IsLocked(TBool& aState);
	inline TInt IsOff(TBool& aState);
	inline TInt MaxCurrentInMicroAmps(TInt& aMaxCurrent);
	inline TInt ResetInactivityTimer();
	inline TInt VoltageSupported(TUint& aVoltage);
	inline TInt InactivityCount(TInt& aInactivityCount);
	inline TInt NotLockedCount(TInt& aNotLockedCount);
	inline TInt InactivityTimeout(TInt& aInactivityTimeout);
	inline TInt NotLockedTimeout(TInt& aNotLockedTimeout);
	inline TInt MemVoltageSupported(TUint& aVoltage);
	inline TInt MemMaxCurrentInMicroAmps(TInt& aMaxCurrent);
	inline TInt VoltCheckInterval(TUint& aVoltCheckInterval);

	//DMMCMediaChange
	inline TInt MediaChange(TAny* aMediaChange);
	inline TInt MediaStateInfo(TInt& aMediaState);
	inline TInt ForceMediaChange();
	inline TInt MediaChangeCounter(TInt& aCounter);
	
	// DMMCSocket
	inline TInt Socket(TAny* aSocket);
	inline TInt SocketAdjustPartialRead(TPartialReadData& aPartialReadData);
	inline TInt SocketCardIsPresent(TBool& aCardPresent);
	inline TInt SocketGetBufferInfo(TBufferInfo& aBufferInfo);
	inline TInt SocketInit();
	inline TInt SocketInitiatePowerUpSequence();
	inline TInt SocketMachineInfo(TMachineInfo& aMachineInfo);
	inline TInt SocketPrepareStore(TPasswordPrepareStoreData& aData);
	inline TInt SocketReset1();
	inline TInt SocketReset2();
	inline TInt SocketResetInactivity(TInt aBusNumber);
	inline TInt SocketStack(TStackPtr& aStackPtr);
	inline TInt SocketiState_Get(TBusState& aState);
	inline TInt SocketiDoorOpened_Get(TBool& aDoorOpened);
	inline TInt SocketiStandby_Get(TBool& aStandby);
	inline TInt SocketiType_Get(TCardBusType& aType);
	inline void SocketPowerUp(TRequestStatus& aStatus);

	// DMMCStack
	inline TInt Stack(TAny* aStack);
	inline TInt StackAdjustPartialRead(TPartialReadData& aPartialReadData);
	inline TInt StackAllocSession();
	inline TInt StackGetBufferInfo(TBufferInfo& aBufferInfo);
	inline TInt StackBufferInfo(TBufferInfo& aBufferInfo);
	inline TInt StackCardP(TCardPtr& aCardPtr);
	inline TInt StackStop();
	inline TInt StackMaxCardsInStack(TUint& aNumCardsInStack);
	inline TInt StackMMCSocket(TAny* aAny);

	inline TInt StackPasswordStore(TAny** aPwdStorePtr);
	inline TInt StackReadPasswordStore(TStackPasswordStoreData& aPwdStoreData);
	inline TInt StackWritePasswordStore(TStackPasswordStoreData& aPwdStoreData);
	inline TInt StackProgramPeriodInMilliSeconds(TInt& aPeriodMs);
	inline TInt StackInitStackInProgress(TBool& aInProgress);
	inline TInt StackHasSessionsQueued(TBool& aSessionsQueued);
	inline TInt StackHasCardsPresent(TBool& aCardsPresent);
	inline TInt StackMachineInfo(TMachineInfo& aMachineInfo);
	inline TInt StackDemandPagingInfo(TTCDemandPagingInfo& aInfo);
	inline TInt StackEffectiveModes(TUint32& aRetVal);
	inline TInt StackInit();
	inline TInt StackReportPowerUp();
	inline TInt StackReportPowerDown();
	inline TInt StackAlterEffectiveModes( TEffectiveModesFunctionId FunctionId, TAny* aData);

	// general
	inline void ReadSector(TRequestStatus &aStatus,TInt aSectNum,TDes8 &aDes);
	inline void WriteSector(TRequestStatus &aStatus,TInt aSectNum,const TDesC8 &aDes);
	inline TInt CardInfo(TMMCCardInfo& aCardInfo);
	inline TInt ReadCSD(TCSDInfo& aCSDInfo);
	inline TInt ReadExtCSD(TExtendedCSDInfo& aExtCSDInfo);
	inline TInt Card(TAny* aCard);
	inline TInt CardIsPresent(TBool& aIsPresent);
	inline TInt CardIsReady(TBool& aIsReady);
	inline TInt CardIsLocked(TBool& aIsLocked);
	inline void PwrDownStack();

private:
	inline TInt TestDoControl(TTCFunctionId aFunctionId, TAny *a1 = NULL, TAny *a2 = NULL);
	inline void TestDoRequest(TTCFunctionId aFunctionId, TRequestStatus &aStatus, TAny *a1 = NULL, TAny *a2 = NULL);
	};

#include "d_mmcsdif.inl"

#endif
