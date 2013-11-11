// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declarations of Integration test step classes for A3F DevSound Play cases.
// 
//

#ifndef TSI_A3F_DEVSOUND_PLAY_H
#define TSI_A3F_DEVSOUND_PLAY_H

// User includes
#include "tsi_a3f_devsound_teststep.h"

/*
 *
 * RTestStepA3FDevSoundPlayBase
 *
 */
class RTestStepA3FDevSoundPlayBase : public RAsyncTestStep,
									 public MDevSoundObserver
	{
public:
	// From RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();

protected:
	RTestStepA3FDevSoundPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);
	RTestStepA3FDevSoundPlayBase(const TDesC& aTestName, const TDesC& aSectName);
	RTestStepA3FDevSoundPlayBase(const TDesC& aTestName, const TFourCC& aInputDataType);
	RTestStepA3FDevSoundPlayBase(const TDesC& aTestName);

	virtual void DoKickoffTestL();

	// Executes the playing events of DevSound using a Finite State Machine
	virtual void Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError);

	// From MDevSoundObserver
	virtual void InitializeComplete(TInt aError);
	virtual void ToneFinished(TInt aError);
	virtual void BufferToBeFilled(CMMFBuffer* aBuffer);
	virtual void PlayError(TInt aError);
	virtual void BufferToBeEmptied(CMMFBuffer* aBuffer);
	virtual void RecordError(TInt aError);
	virtual void ConvertError(TInt aError);
	virtual void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

	// Timer Callback
	static TInt TimerCallback(TAny*);
	virtual void DoTimerCallback();

	TUint ConvertDesToTMMFSampleRate(const TDesC& aSampleRateString);
	TUint ConvertDesToTMMFMonoStereo(const TDesC& aMonoStereoString);
	void DisplayCodecInfo();
	void SetSampleRateAndChannelMode();
	void CheckConfigAndFourCCUsage();

protected:
	RFs iFs;
	RFile iFile;
	CMMFDevSound* iMMFDevSound;
	CMMFBuffer* iBuffer;
	CPeriodic* iTimer;
	TCallBack iCallBack;
	TFourCC iInputDataType; // datatype (FourCC) of the Decoder
	TBuf<KNameSize> iSectName;
	TPtrC iFilename;
	TPtrC iSampleRate;
	TPtrC iMonoStereo;
	TDevSoundPlayState iPlayState;
	TInt iBufferCount;
	TBool iConfig;
	TBool iInvalidConfig;
	TBool iUseFourCC;

	};

/*
 *
 * RStepA3FDevSoundPlaySimple
 *
 */
class RStepA3FDevSoundPlaySimple : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundPlaySimple* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);
	static RStepA3FDevSoundPlaySimple* NewL(const TDesC& aTestName, const TDesC& aSectName);

private:
	RStepA3FDevSoundPlaySimple(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);
	RStepA3FDevSoundPlaySimple(const TDesC& aTestName, const TDesC& aSectName);

	};

/*
 *
 *	RStepA3FDevSoundPlayEmpty
 *
 */
class RStepA3FDevSoundPlayEmpty : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundPlayEmpty* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundPlayEmpty(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError);

	};

/*
 *
 *	RStepA3FDevSoundPlayInvalidConfig
 *
 */
class RStepA3FDevSoundPlayInvalidConfig : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundPlayInvalidConfig* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundPlayInvalidConfig(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RAsyncTestStep
	virtual void KickoffTestL();

	// From MDevSoundObserver
	virtual void PlayError(TInt aError);

	};

/*
 *
 * RStepA3FDevSoundPlayInvalidFourCC
 *
 */
class RStepA3FDevSoundPlayInvalidFourCC : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundPlayInvalidFourCC* NewL(const TDesC& aTestName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundPlayInvalidFourCC(const TDesC& aTestName, const TFourCC& aInputDataType);

	// From RAsyncTestStep
	virtual void KickoffTestL();

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError);

	};

/*
 *
 * RStepA3FDevSoundInitializeWithHwDeviceUID
 *
 */
class RStepA3FDevSoundInitializeWithHwDeviceUID : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundInitializeWithHwDeviceUID* NewL(const TDesC& aTestName);

private:
	RStepA3FDevSoundInitializeWithHwDeviceUID(const TDesC& aTestName);

	// From RAsyncTestStep
	virtual void KickoffTestL();

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aDevSoundEvent, TInt aError);

	};

/**
 *
 *	RStepA3FDevSoundReadConfigDuringPlay
 */
class RStepA3FDevSoundReadConfigDuringPlay : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundReadConfigDuringPlay* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundReadConfigDuringPlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From MDevSoundObserver
	virtual void BufferToBeFilled(CMMFBuffer* aBuffer);

	};

/*
 *
 * RStepA3FDevSoundSetVolPlayStopGetVol
 *
 */
class RStepA3FDevSoundSetVolPlayStopGetVol : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundSetVolPlayStopGetVol* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundSetVolPlayStopGetVol(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	virtual void DoKickoffTestL();
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);
	virtual void DoTimerCallback();

private:
	TInt iSetVolume;
	TInt iVolDuringStopped;
	TBool iCheckTimer;

	};

/*
 *
 * RStepA3FDevSoundFlushBuffersPaused
 *
 */
class RStepA3FDevSoundFlushBuffersPaused : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundFlushBuffersPaused* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundFlushBuffersPaused(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);
	virtual void DoTimerCallback();

	// From MDevSoundObserver
	void BufferToBeFilled(CMMFBuffer* aBuffer);

private:
	TBool iCheckTimer;

	};

/*
 *
 * RStepA3FDevSoundInvalidStatePlay
 *
 */
class RStepA3FDevSoundInvalidStatePlay : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundInvalidStatePlay* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundInvalidStatePlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RAsyncTestStep
	virtual void KickoffTestL();

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);

	// From MDevSoundObserver
	virtual void BufferToBeFilled(CMMFBuffer* aBuffer);
	virtual void PlayError(TInt aError);

	};

/*
 *
 * RStepA3FDevSoundFlushBuffersPlaying
 *
 */
class RStepA3FDevSoundFlushBuffersPlaying : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundFlushBuffersPlaying* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundFlushBuffersPlaying(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);

	};

/*
 *
 * RStepA3FDevSoundInitializeWhilePlaying
 *
 */
class RStepA3FDevSoundInitializeWhilePlaying : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundInitializeWhilePlaying* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundInitializeWhilePlaying(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);
	void DoTimerCallback();

private:
	TBool iCheckTimer;

	};

/*
 *
 * RStepA3FDevSoundSetBalanceDuringPlay
 *
 */
class RStepA3FDevSoundSetBalanceDuringPlay : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundSetBalanceDuringPlay* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundSetBalanceDuringPlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	virtual void DoKickoffTestL();

	// From MDevSoundObserver
	virtual void BufferToBeFilled(CMMFBuffer* aBuffer);

private:
	TInt iPlaybalanceLeft;
	TInt iPlaybalanceRight;

	};

/*
 *
 * RStepA3FDevSoundPlayStopMultipleTimes
 *
 */
class RStepA3FDevSoundPlayStopMultipleTimes : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundPlayStopMultipleTimes* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:

	RStepA3FDevSoundPlayStopMultipleTimes(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	virtual void DoKickoffTestL();
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);
	virtual void DoTimerCallback();

private:
	TInt iRepeatCount;
	TInt iRepeatCounter;
	TBool iCheckTimer;

	};

/*
 *
 * RStepA3FDevSoundPlayEOFPlayMultipleTimes
 *
 */
class RStepA3FDevSoundPlayEOFPlayMultipleTimes : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundPlayEOFPlayMultipleTimes* NewL(const TDesC& aTestName, const TDesC& aSectName);

private:
	RStepA3FDevSoundPlayEOFPlayMultipleTimes(const TDesC& aTestName, const TDesC& aSectName);

	// From RTestStepA3FDevSoundPlayBase
	virtual void DoKickoffTestL();

	// From MDevSoundObserver
	virtual void PlayError(TInt aError);

private:
	TInt iRepeatCount;
	TInt iRepeatCounter;

	};

/*
 *
 * RStepA3FDevSoundInititalizeDuringInitialize
 *
 */
class RStepA3FDevSoundInititalizeDuringInitialize : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundInititalizeDuringInitialize* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundInititalizeDuringInitialize(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RAsyncTestStep
	virtual void KickoffTestL();

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);

	// From MDevSoundObserver
	virtual void InitializeComplete(TInt aError);

private:
	TInt iSecondInitErr;

	};

/*
 *
 * RStepA3FDevSoundPlayInitDuringPlayInit
 *
 */
class RStepA3FDevSoundPlayInitDuringPlayInit : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundPlayInitDuringPlayInit* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundPlayInitDuringPlayInit(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RASyncTestStep
	virtual void KickoffTestL();

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);

	// From MDevSoundObserver
	virtual void BufferToBeFilled(CMMFBuffer* aBuffer);

private:
	TInt iFirstPlayInitErr;
	TInt iSecondPlayInitErr;

	};

/*
 *
 * RStepA3FDevSoundVolumeCrop
 *
 */
class RStepA3FDevSoundVolumeCrop : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundVolumeCrop* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundVolumeCrop(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RAsyncTestStep
	virtual void KickoffTestL();

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);

private:
	TInt iIncreaseMaxVolume;
	TInt iMinVolume;

	};

/*
 *
 * RStepA3FDevSoundInitializeForConverting
 *
 */
class RStepA3FDevSoundInitializeForConverting : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundInitializeForConverting* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundInitializeForConverting(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RAsyncTestStep
	virtual void KickoffTestL();

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);

	};

/*
 *
 * RStepA3FDevSoundGetSampleNumsDuringPlayStop
 *
 */
class RStepA3FDevSoundGetSampleNumsAfterStop : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundGetSampleNumsAfterStop* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundGetSampleNumsAfterStop(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);
	virtual void DoTimerCallback();

private:
	TBool iCheckTimer;

	};

/*
 *
 * RStepA3FDevSoundCancelInitializePlay
 *
 */
class RStepA3FDevSoundCancelInitializePlay : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundCancelInitializePlay* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundCancelInitializePlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);
	virtual void DoTimerCallback();
	virtual void KickoffTestL();
	
	// From MDevSoundObserver
	virtual void InitializeComplete(TInt aError);
	
	};	

/*
 *
 * RStepA3FDevSoundNegCancelInitializePlay
 *
 */
class RStepA3FDevSoundNegCancelInitializePlay : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundNegCancelInitializePlay* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundNegCancelInitializePlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);
	virtual void KickoffTestL();
	
	// From MDevSoundObserver
	virtual void InitializeComplete(TInt aError);

	};

/*
 *
 * RStepA3FDevSoundEmptyBuffersInCreatedState
 *
 */
class RStepA3FDevSoundEmptyBuffersInCreatedState : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundEmptyBuffersInCreatedState* NewL(const TDesC& aTestName);

private:
	RStepA3FDevSoundEmptyBuffersInCreatedState(const TDesC& aTestName);

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);
	virtual void KickoffTestL();
	
	};

/*
 *
 * RStepA3FDevSoundEmptyBuffersInInitializedState
 *
 */
class RStepA3FDevSoundEmptyBuffersInInitializedState : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundEmptyBuffersInInitializedState* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundEmptyBuffersInInitializedState(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);
	virtual void KickoffTestL();

	};
	
/*
 *
 * RStepA3FDevSoundEmptyBuffersInInitializingState
 *
 */
class RStepA3FDevSoundEmptyBuffersInInitializingState : public RTestStepA3FDevSoundPlayBase
	{
public:
	static RStepA3FDevSoundEmptyBuffersInInitializingState* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

private:
	RStepA3FDevSoundEmptyBuffersInInitializingState(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aInputDataType);

	// From RTestStepA3FDevSoundPlayBase
	virtual void Fsm(TDevSoundPlayEvents aEventCode, TInt aError);
	virtual void KickoffTestL();
	
	// From MDevSoundObserver
	virtual void InitializeComplete(TInt aError);

	};
	
#endif // TSI_A3F_DEVSOUND_PLAY_H
