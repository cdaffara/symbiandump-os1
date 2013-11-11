// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef TSU_MMF_DEVSOUND_STEP_H
#define TSU_MMF_DEVSOUND_STEP_H

#include <testframework.h>
#include <mmf/server/sounddevice.h>

#include "TSU_MMF_DEVSOUND_SUITE.h"
#include "../TestInterface/TSU_MMF_DEVSOUND_TestInterface_ImplementationUID.hrh"
#include "../TestInterface1/TSU_MMF_DEVSOUND_TestInterface1_ImplementationUID.hrh"
#include "../TestInterface/TSU_MMF_DEVSOUND_TestInterface.h"
#include "../TestInterface1/TSU_MMF_DEVSOUND_TestInterface1.h"
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>
#include "../CIPlugins/src/MMFDevSoundCIMuxDeMuxPluginImp.h"

const TInt KMaxVolume = 0xFFFF;
const TInt KMaxGain   = 0xFFFF;

class CTestSuite;
class CTestSuiteDevSound;

/**
 *
 * CActiveListener provides the asynchronous operation
 * of an active object
 *
 * @lib "TSU_MMF_DEVSOUND_SUITE.lib"
 *
 *
 */
class CActiveListener : public CActive
	{
public:
	CActiveListener() : CActive(CActive::EPriorityStandard) 
		{ 
		CActiveScheduler::Add(this);
		}
	void InitialiseActiveListener()
		{
		iStatus = KRequestPending; SetActive();
		}
	~CActiveListener()
		{
		Cancel();
		}
	// From CActive
	virtual void RunL()
		{
		CActiveScheduler::Stop();
		}
	virtual void DoCancel()
		{
		CActiveScheduler::Stop();
		};
	};

/**
 *
 * CTestStepDevSound - Test class that enables DevSound tests.
 * It's a base class for all test steps.
 *
 * @lib "TSU_MMF_DEVSOUND_SUITE.lib"
 *
 *
 */
class CTestStepDevSound : public CTestStep, public MDevSoundObserver
	{
public:
	CTestStepDevSound();
	~CTestStepDevSound();
	static void CleanUp(TAny *aPtr);
	void SetTestSuite(const CTestSuiteDevSound* aTestSuite);

protected:
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();

	// Callback methods from MDevSoundObserver mixin
	void InitializeComplete (TInt aError);
	void ToneFinished (TInt aError);
	void BufferToBeFilled (CMMFBuffer* aBuffer);
	void PlayError (TInt aError);
	void BufferToBeEmptied (CMMFBuffer* aBuffer);
	void RecordError (TInt aError);
	void ConvertError (TInt aError);
	void DeviceMessage (TUid aMessageType, const TDesC8& aMsg);

	// Test methods (called by test cases)
	TVerdict TestInitialize(TMMFState aMode);
	TVerdict TestInitialize(TFourCC& aDataType, TMMFState aMode);
	TVerdict TestInitialize(TUid aHwDeviceUid, TMMFState aMode);
	TVerdict TestCaps();
	TVerdict TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur);
	TVerdict TestPlayDTMFString(TPtrC aDTMFString);
	void TestSetVolume(TInt aVol);
	void TestSetGain(TInt aGain);
	TVerdict TestSetVolRamp(TInt aVol, TInt aDur);
	TInt TestMaxVolume();
	TInt TestMaxGain();
	TInt TestVolume();
	TInt TestGain();
	TVerdict TestPlayInit();
	TVerdict TestPlayData();
	TVerdict TestRecordInit();
	TVerdict TestRecordInitCapsNotMatch();
	TVerdict TestRecordData();
	TVerdict TestRecordDataCapsNotMatch();
	

	// Utility Methods
	void ResetCallbacks();
	TInt GetCallbackTotal();
	TVerdict TestDigitalPlayback(TInt aNumSamples, TDesC& aFilename);
	TVerdict TestDigitalRecord(TInt aNumSamples, TDesC& aFilename);
	TVerdict TestDigitalPlayback(TInt aNumSamples, TDesC& aFilename, TFourCC& aDataType);
	TVerdict TestDigitalRecord(TInt aNumSamples, TDesC& aFilename, TFourCC& aDataType);

protected:

	typedef enum TCallback 
		{
		EInitComplete,
		EToneFinished,
		EBuffToFill,
		EPlayError,
		EBuffToEmpty,
		ERecError,
		EConvertMsg,
		EDeviceMsg
		};

	TFixedArray<TInt, 7> iCallbackArray;
	
	// pointer to suite which owns this test 
	const CTestSuiteDevSound* iDevSoundTestSuite;

	CActiveScheduler* iAS;
	CActiveListener* iAL;
	CMMFDevSound* iMMFDevSound;
	TInt iExpectedValue;
	TInt iCallbackError;
	CMMFBuffer* iBuffer;
};

// DevSound events
enum TMmfDevSoundEvents
    {
    EDevSoundInit = 0,
    EDevSoundPlayInit,
    EDevSoundPlayData,
    EDevSoundEmptyBuffer
    };
enum TCallback 
	{
	EInitComplete = 0,
	EToneFinished,
	EBuffToFill,
	EPlayError,
	EBuffToEmpty,
	ERecError,
	EConvertMsg,
	EDeviceMsg
	};
/**
 *
 * CTestStepDevSoundCustomInterface.
 * 
 *
 * @lib "TSU_MMF_DEVSOUND_SUITE.lib"
 *
 *
 */
class CTestStepDevSoundCustomInterface : public CTestStep, public MDevSoundObserver
	{
public:
	CTestStepDevSoundCustomInterface();
	~CTestStepDevSoundCustomInterface();
	void SetTestSuite(const CTestSuiteDevSound* aTestSuite);

protected:
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict RunTest();
	// Executes events of DevSound
    void FsmL(TMmfDevSoundEvents aDevSoundEvent);
	// Callback methods from MDevSoundObserver mixin
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

	// Utility Methods
	void ResetCallbacks();
	TInt GetCallbackTotal();
	
protected:

	TFixedArray<TInt, 7> iCallbackArray;
	
	// pointer to suite which owns this test 
	const CTestSuiteDevSound* iDevSoundTestSuite;

	CActiveScheduler* iAS;
	CMMFDevSound* iMMFDevSound;
	TInt iExpectedValue;
	TInt iCallbackError;
	CMMFBuffer* iBuffer;
	RFile iFile;
    RFs iFs;
    //The sequence of methods called in this test - 1.Initilaize DevSound
 	// 2.PlayInitL, 3.Play 4.EmptyBuffers .Continue sequence 3 to 4
 	//This variable is used to count number of buffers in the sequence of call from 3 to 4
    TInt iBufferCount;
    //To know the last buffer
    TBool iLastBuffer;
};

//------------------------------------------------------------------

class CTestStepDevSoundInitializePlay : public CTestStepDevSound
	{
public:
	CTestStepDevSoundInitializePlay();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundInitializeHwDeviceUidPlay : public CTestStepDevSound
	{
public:
	CTestStepDevSoundInitializeHwDeviceUidPlay();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundInitializeFourCCPlay : public CTestStepDevSound
	{
public:
	CTestStepDevSoundInitializeFourCCPlay();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundInitializeRecord : public CTestStepDevSound
	{
public:
	CTestStepDevSoundInitializeRecord();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundInitializeHwDeviceUidRecord : public CTestStepDevSound
	{
public:
	CTestStepDevSoundInitializeHwDeviceUidRecord();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundInitializeFourCCRecord : public CTestStepDevSound
	{
public:
	CTestStepDevSoundInitializeFourCCRecord();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundInitializeInvalidFourCC : public CTestStepDevSound
	{
public:
	CTestStepDevSoundInitializeInvalidFourCC();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundCheckCaps : public CTestStepDevSound
	{
public:
	CTestStepDevSoundCheckCaps();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundSimpleTone : public CTestStepDevSound
	{
public:
	CTestStepDevSoundSimpleTone();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundDTMFTones : public CTestStepDevSound
	{
public:
	CTestStepDevSoundDTMFTones();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundSimpleToneChangeVolume : public CTestStepDevSound
	{
public:
	CTestStepDevSoundSimpleToneChangeVolume();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundDTMFTonesInvalidStrings : public CTestStepDevSound
	{
public:
	CTestStepDevSoundDTMFTonesInvalidStrings();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundSimpleToneInvalidFreq : public CTestStepDevSound
	{
public:
	CTestStepDevSoundSimpleToneInvalidFreq();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundSimpleToneInvalidDuration : public CTestStepDevSound
	{
public:
	CTestStepDevSoundSimpleToneInvalidDuration();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundPlayInit : public CTestStepDevSound
	{
public:
	CTestStepDevSoundPlayInit();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundPlay1BufferPCM16 : public CTestStepDevSound
	{
public:
	CTestStepDevSoundPlay1BufferPCM16();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundPlay5BuffersPCM16 : public CTestStepDevSound
	{
public:
	CTestStepDevSoundPlay5BuffersPCM16();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundPlayEOFPCM16 : public CTestStepDevSound
	{
public:
	CTestStepDevSoundPlayEOFPCM16();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundPlay1BufferPCM8 : public CTestStepDevSound
	{
public:
	CTestStepDevSoundPlay1BufferPCM8();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundPlay5BuffersPCM8 : public CTestStepDevSound
	{
public:
	CTestStepDevSoundPlay5BuffersPCM8();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundPlayEOFPCM8 : public CTestStepDevSound
	{
public:
	CTestStepDevSoundPlayEOFPCM8();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundPlayGarbageEOF : public CTestStepDevSound
	{
public:
	CTestStepDevSoundPlayGarbageEOF();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundMaxGain : public CTestStepDevSound
	{
public:
	CTestStepDevSoundMaxGain();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundMaxVolume : public CTestStepDevSound
	{
public:
	CTestStepDevSoundMaxVolume();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundSetGain : public CTestStepDevSound
	{
public:
	CTestStepDevSoundSetGain();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundSetVolume : public CTestStepDevSound
	{
public:
	CTestStepDevSoundSetVolume();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundRecord1BufferPCM16 : public CTestStepDevSound
	{
public:
	CTestStepDevSoundRecord1BufferPCM16();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------
class CTestStepDevSoundRecord10BuffersPCM16 : public CTestStepDevSound
	{
public:
	CTestStepDevSoundRecord10BuffersPCM16();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundRecord1BufferPCM8 : public CTestStepDevSound
	{
public:
	CTestStepDevSoundRecord1BufferPCM8();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundRecord10BuffersPCM8 : public CTestStepDevSound
	{
public:
	CTestStepDevSoundRecord10BuffersPCM8();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundRecordInitNoUECap : public CTestStepDevSound
	{
public:
	CTestStepDevSoundRecordInitNoUECap(TBool aHasMMDDCap);
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundRecordInitWithUECap : public CTestStepDevSound
	{
public:
	CTestStepDevSoundRecordInitWithUECap(TBool aHasAllCaps);
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundRecordDataWithUECap : public CTestStepDevSound
	{
public:
	CTestStepDevSoundRecordDataWithUECap(TBool aHasAllCaps);
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundSetConfigStereo : public CTestStepDevSound
	{
public:
	CTestStepDevSoundSetConfigStereo();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundSetConfigSampleRate : public CTestStepDevSound
	{
public:
	CTestStepDevSoundSetConfigSampleRate();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundGetSupportedInputDataTypes : public CTestStepDevSound 
	{
public:
	CTestStepDevSoundGetSupportedInputDataTypes();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundGetSupportedOutputDataTypes : public CTestStepDevSound 
	{
public:
	CTestStepDevSoundGetSupportedOutputDataTypes();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundGetSupportedInputDataTypesOOM : public CTestStepDevSound
	{
public:
	CTestStepDevSoundGetSupportedInputDataTypesOOM();
	TVerdict DoTestStepL();
	};
	
//------------------------------------------------------------------

class CTestStepDevSoundGetSupportedOutputDataTypesOOM : public CTestStepDevSound
	{
public:
	CTestStepDevSoundGetSupportedOutputDataTypesOOM();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

class CTestStepDevSoundPlayPauseTone : public RAsyncTestStep, public MDevSoundObserver
	{
public:
	static CTestStepDevSoundPlayPauseTone* NewL(CTestSuite* aSuite);

	void Fsm();
	void FsmL();

	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

private:
	CTestStepDevSoundPlayPauseTone(CTestSuite* aSuite);
	void TestPlayToneL();
private:
	CMMFDevSound* iMMFAudioDevSound;
	CMMFDevSound* iMMFToneDevSound;
	RFs iFs;
	RFile iInputFile;
	enum TTestState
		{
		EStateInitialiseAudioDevSound,
		EStateInitialiseToneDevSound,
		EStatePlayInit,
		EStatePlayData,
		EStatePlayTone
		};
	TTestState iTestState;
	CMMFBuffer* iBuffer;
	TInt iBufferCount;
	TBool iTonePlayKickedOff;
	};
	
//------------------------------------------------------------------

class CTestStepDevSoundPlayPauseTone2 : public RAsyncTestStep, public MDevSoundObserver
	{
public:
	static CTestStepDevSoundPlayPauseTone2* NewL(CTestSuite* aSuite);

	void Fsm();
	void FsmL();

	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

private:
	CTestStepDevSoundPlayPauseTone2(CTestSuite* aSuite);
	void TestPlayToneL();
private:
	CMMFDevSound* iMMFAudioDevSound;
	CMMFDevSound* iMMFToneDevSound;
	RFs iFs;
	RFile iInputFile;
	enum TTestState
		{
		EStateInitialiseAudioDevSound,
		EStateInitialiseToneDevSound,
		EStatePlayInit,
		EStatePlayData,
		EStatePlayTone
		};
	TTestState iTestState;
	CMMFBuffer* iBuffer;
	TInt iBufferCount;
	TBool iTonePlayKickedOff;
	};
	
//------------------------------------------------------------------

class CTestStepDevSoundPlayPauseTone3 : public RAsyncTestStep, public MDevSoundObserver
	{
public:
	static CTestStepDevSoundPlayPauseTone3* NewL(CTestSuite* aSuite);

	void Fsm();
	void FsmL();

	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

private:
	CTestStepDevSoundPlayPauseTone3(CTestSuite* aSuite);
	void TestPlayToneL(TBool aLowPriority = ETrue);
private:
	CMMFDevSound* iMMFAudioDevSound;
	CMMFDevSound* iMMFLowPriorityToneDevSound;
	CMMFDevSound* iMMFHighPriorityToneDevSound;
	RFs iFs;
	RFile iInputFile;
	enum TTestState
		{
		EStateInitialiseAudioDevSound,
		EStateInitialiseLowPriorityToneDevSound,
		EStateInitialiseHighPriorityToneDevSound,
		EStatePlayInit,
		EStatePlayData,
		EStatePlayTone
		};
	TTestState iTestState;
	CMMFBuffer* iBuffer;
	TInt iBufferCount;
	TBool iLowPriorityTonePlayKickedOff;
	TBool iHighPriorityTonePlayKickedOff;
	};

//------------------------------------------------------------------

class RTestStepDevSoundToneTone : public RAsyncTestStep, public MDevSoundObserver
	{
public:
	static RTestStepDevSoundToneTone* NewL(CTestSuite* aSuite);
	static TInt TimerCallBack(TAny* aPtr);

	void Fsm();
	void FsmL();

	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

private:
	RTestStepDevSoundToneTone(CTestSuite* aSuite);
	void TestPlayToneL(TBool aLowPriority = ETrue);
private:
	CMMFDevSound* iMMFLowPriorityToneDevSound;
	CMMFDevSound* iMMFHighPriorityToneDevSound;
	enum TTestState
		{
		EStateInitialiseLowPriorityToneDevSound,
		EStateInitialiseHighPriorityToneDevSound,
		EStatePlayingHighPriorityTone,
		EStatePlayingLowPriorityTone
		};
	TTestState iTestState;
	CPeriodic* iTimer;
	};

//------------------------------------------------------------------

class RTestStepDevSoundRecordRecord : public RAsyncTestStep, public MDevSoundObserver
	{
public:
	static RTestStepDevSoundRecordRecord* NewL(CTestSuite* aSuite);
	static TInt TimerCallBack(TAny* aPtr);

	void Fsm();
	void FsmL();

	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

private:
	RTestStepDevSoundRecordRecord(CTestSuite* aSuite);
	//void TestPlayToneL(TBool aLowPriority = ETrue);
private:
	CMMFDevSound* iMMFLowPriorityRecordDevSound;
	CMMFDevSound* iMMFHighPriorityRecordDevSound;
	enum TTestState
		{
		EStateInitialiseLowPriorityRecordDevSound,
		EStateInitialiseHighPriorityRecordDevSound,
		EStateRecordingHighPriorityAudio,
		EStateRecordingLowPriorityAudio
		};
	TTestState iTestState;
	CPeriodic* iTimer;
	};

//------------------------------------------------------------------

class RTestStepDevSoundToneReplay : public RAsyncTestStep, public MDevSoundObserver
	{
public:
	static RTestStepDevSoundToneReplay* NewL(CTestSuite* aSuite);
	static TInt TimerCallBack(TAny* aPtr);

	void Fsm();
	void FsmL();

	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

private:
	RTestStepDevSoundToneReplay(CTestSuite* aSuite);
	void TestPlayToneL(TBool aLowPriority = ETrue);

private:
	CMMFDevSound* iMMFLowPriorityToneDevSound;
	CMMFDevSound* iMMFHighPriorityToneDevSound;
	enum TTestState
		{
		EStateInitialiseLowPriorityToneDevSound,
		EStateInitialiseHighPriorityToneDevSound,
		EStatePlayingLowPriorityTone,
		EStatePlayingHighPriorityTone,
		EStateReplayingLowPriorityTone,
		EStateWaitingAfterStop,		
		};
	TTestState iTestState;
	CPeriodic* iTimer;
	};

//------------------------------------------------------------------

class RTestStepDevSoundRecordRerecord : public RAsyncTestStep, public MDevSoundObserver
	{
public:
	static RTestStepDevSoundRecordRerecord* NewL(CTestSuite* aSuite);
	static TInt TimerCallBack(TAny* aPtr);

	void Fsm();
	void FsmL();

	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

private:
	RTestStepDevSoundRecordRerecord(CTestSuite* aSuite);
	void TestPlayToneL(TBool aLowPriority = ETrue);

private:
	CMMFDevSound* iMMFLowPriorityRecordDevSound;
	CMMFDevSound* iMMFHighPriorityToneDevSound;
	enum TTestState
		{
		EStateInitialiseLowPriorityRecordDevSound,
		EStateInitialiseHighPriorityToneDevSound,
		EStateRecordingLowPriority,
		EStatePlayingHighPriorityTone,
		EStateRerecordingLowPriority,
		EStateWaitingAfterStop,		
		};
	TTestState iTestState;
	CPeriodic* iTimer;
	TInt iBuffersRecordedCount;
	};
	
//------------------------------------------------------------------

class RTestStepDevSoundPlayReplay : public RAsyncTestStep, public MDevSoundObserver
	{
public:
	static RTestStepDevSoundPlayReplay* NewL(CTestSuite* aSuite);
	static TInt TimerCallBack(TAny* aPtr);

	void Fsm();
	void FsmL();

	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

private:
	RTestStepDevSoundPlayReplay(CTestSuite* aSuite);
	void TestPlayToneL(TBool aLowPriority = ETrue);

private:
	CMMFDevSound* iMMFLowPriorityPlayDevSound;
	CMMFDevSound* iMMFHighPriorityToneDevSound;
	enum TTestState
		{
		EStateInitialiseLowPriorityPlayDevSound,
		EStateInitialiseHighPriorityToneDevSound,
		EStatePlayingLowPriority,
		EStatePlayingHighPriorityTone,
		EStateReplayingLowPriority,
		EStateWaitingAfterStop,		
		};
	TTestState iTestState;
	CPeriodic* iTimer;
	RFile iInputFile;
	RFs iFs;
	TInt iBuffersPlayedCount;
	};
	
//------------------------------------------------------------------

/**
 *
 * 
 * Check that the SampleRate supported for tone play is 8000Hz only. Fix for DEF054803
 * 
 */
class CTestStepDevSoundCheckSampleRateforTone : public CTestStepDevSound
	{
public:
	CTestStepDevSoundCheckSampleRateforTone();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

	
//------------------------------------------------------------------

class RTestStepBasicCustomInterface : public RTestStep
	{
public:
	static RTestStepBasicCustomInterface* NewL();

	// from RTestStep
	TVerdict DoTestStepL();

protected:
	TVerdict TestCustomInterface(MMMFDevSoundTestCustomInterface* aInterface);
	RTestStepBasicCustomInterface();
	};

//------------------------------------------------------------------

class RTestStepBasicCustomInterfaceAlloc : public RTestStepBasicCustomInterface
	{
public:
	static RTestStepBasicCustomInterfaceAlloc* NewL();

	// from RTestStep
	TVerdict DoTestStepL();

protected:
	RTestStepBasicCustomInterfaceAlloc();
	};


class CTestStepDevSoundEmptyBuffersPos : public CTestStepDevSoundCustomInterface
	{
public:
	CTestStepDevSoundEmptyBuffersPos();
	TVerdict DoTestStepL();
	};
	
class CTestStepDevSoundEmptyBuffersNeg1 : public CTestStepDevSoundCustomInterface
	{
public:
	CTestStepDevSoundEmptyBuffersNeg1();
	TVerdict DoTestStepL();
	void FsmL(TMmfDevSoundEvents aDevSoundEvent);
	void InitializeComplete(TInt aError);
	};

class CTestStepDevSoundEmptyBuffersNeg2 : public CTestStepDevSoundCustomInterface
	{
public:
	CTestStepDevSoundEmptyBuffersNeg2();
	TVerdict DoTestStepL();
	};

class CTestStepDevSoundCITest;	
class CFsmHelper : public CActive
	{
public:
	static CFsmHelper* NewL();
	~CFsmHelper();
	void Start();
	TRequestStatus* GetStatus();
	TVerdict GetResult();
	void SetDevSoundPointer(CMMFDevSound* aMMFDevSound);
	void SetDevSoundTestPointer(CTestStepDevSoundCITest* aMMFDevSndTestPointer);
private:
	CFsmHelper();
	void ConstructL();
private://From CActive
	void RunL();
	void DoCancel();
private:
enum TEventStatus
	{
	EDevSoundInit1,
	EDevSoundInit2,
	EAddDelayClient2,
	EClient2Idle
	};
private:
	TEventStatus iEventStatus;
	TVerdict iTestResult;
	TEventStatus iClientStatus;
	TRequestStatus iRequestStatus 	;
	CLog* iLog;
	MMMFDevSoundCustomInterfaceBitRate* iTestInterface;
	CMMFDevSound* iMMFDevSound;
	CTestStepDevSoundCITest* iTestPointer;
	};

class CTestStepDevSoundCITest : public CTestStepDevSoundCustomInterface
	{
public:
	CTestStepDevSoundCITest();
	TVerdict DoTestStepL();
	void InitializeComplete(TInt aError);
	CFsmHelper* iFsmHelper;
	virtual void Close();
	
private:	
	MMMFDevSoundTestCustomInterface* iTestInterface;
	TInt iInitComplete;
	};	

#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND

class CTestStepDevSoundServerSideAlloc : public CTestStepDevSoundCustomInterface
	{
public:
	CTestStepDevSoundServerSideAlloc();
	TVerdict DoTestStepL();
	void AllocLoopTestL();
	void InitializeComplete(TInt aError);
	TInt MethodToTest();
	};

#endif

//test case for  PDEF117811 Ref DevSound policy updating on error condition is inconsistent 
class CTestDevSoundObserver;
class RDevSoundPolicyUpdateTest : public RAsyncTestStep
	{
public:	
enum TTestEvent
	{
	EDevSoundInitComplete,
	EBTBF,
	EBTBE,
	EPlayError,
	ETonePlayError,
	ERecordError
	};
enum TClientId
	{
	EObserver1,
	EObserver2
	};
enum TStage
	{
	EPlayData,
	EStartPlay,
	EStartRecord,
	ERecordData,
	EStartPlayTune,
	EStartDualPlayTone,
	EStartPlayDTMFString,
	EStartPlayToneSequence
	};
enum TTestState
	{
	//different stages are used to cover various functions in CMMFDevSoundSvrImp, where
	//changes are done as part of fix. Refer in .cpp file for more details about the test.
	//stage1 - test CMMFDevSoundSvrImp::PlayData error
	E1_InitDevSound1, //initialise DevSound1
	E1_PlayInitDevSound1, //Play init DevSound1
	//PlayData on DevSound1; this should result in KErrAbort due to error in 
	//CMMFSwCodecWrapper::ThisHwBufferFilled() 
	E1_Simulate_hw_ThisHwBufferFilled_error,
	E1_InitDevSound2, //initialise DevSound2
	E1_PlayInitDevSound2,  //playData on DevSound2
	
	//stage2- test CMMFDevSoundSvrImp::StartPlayL error
	//stage2_1
	E2_1_InitDevSound1,
	E2_Simulate_hw_SetConfig_Error,
	E2_1_PlayInitDevSound2,
	//stage2_2
	E2_2_InitDevSound1,
	E2_Simulate_hw_Start_Error,
	E2_2_PlayInitDevSound2,
	
	//stage3 - test CMMFDevSoundSvrImp::StartRecord error
	E3_1_InitDevSound1,
	E3_1_InitDevSound2,
	E3_Simulate_hw_SetConfig_Error,
	E3_1_PlayInitDevSound2,
	E3_2_InitDevSound1,
	E3_Simulate_hw_Start_Error,
	E3_2_PlayInitDevSound1,
	
	//stage4 - test CMMFDevSoundSvrImp::RecordData error
	E4_InitDevSound1,
	E4_RecordInitDevSound1,
	E4_Simulate_hw_ThisHwBufferEmptied_error,
	E4_RecordInitDevSound2,
	
	//stage5 - test CMMFDevSoundSvrImp::StartPlayTone; covers error from SetPlayFormat
	E5_InitDevSound1,
	E5_Simulate_hw_SetConfig_error, //from StartPlayToneL
	E5_InitDevSound2,
	E5_PlayInitDevSound2,
	
	//stage6 - test CMMFDevSoundSvrImp::StartDualPlayTone
	E6_InitDevSound1,
	E6_Simulate_hw_SetConfig_error, //from StartDualPlayToneL
	E6_PlayInitDevSound2,
	
	//stage7 - test CMMFDevSoundSvrImp::StartPlayDTMFString
	E7_Simulate_hw_SetConfig_error, //from StartPlayDTMFString
	E7_PlayInitDevSound2,
	
	//stage8- test CMMFDevSoundSvrImp::StartPlayToneSequence
	E8_Simulate_hw_SetConfig_error,
	E8_PlayInitDevSound2
	};
	
	static RDevSoundPolicyUpdateTest* NewL(const TDesC& aStepName);
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();	
	
	void Fsm(TClientId aClientId, TTestEvent aTestEvent, TInt aError);
	void FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError);
	
	//FSM s for different stages of testing. Each stage is to test a function in 
	//Devsound server.
	void PlayData_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError);
	void StartPlay_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError);
	void StartRecord_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError);
	void RecordData_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError);
	void StartPlayTone_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError);
	void StartPlayDualTone_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError);
	void StartPlayDTMFString_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError);
	void StartPlayToneSequence_FsmL(TClientId aClientId, TTestEvent aTestEvent, TInt aError);
private:
	RDevSoundPolicyUpdateTest(const TDesC& aStepName);
	CMMFDevSound* iMMFDevSound1;
	CMMFDevSound* iMMFDevSound2;
	CTestDevSoundObserver* iDevSoundObserver1;
	CTestDevSoundObserver* iDevSoundObserver2;
	TInt iTestState;
	TStage iStage;
	TUid iTestHwDevice1Uid;
	TUid iTestHwDevice2Uid;
	TUid iTestHwDevice3Uid;
	};

class CTestDevSoundObserver : public CBase, public MDevSoundObserver
	{
public:	
	static CTestDevSoundObserver* NewL(RDevSoundPolicyUpdateTest* aParent, RDevSoundPolicyUpdateTest::TClientId aClientId);
	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
private:
	CTestDevSoundObserver(RDevSoundPolicyUpdateTest* aParent, RDevSoundPolicyUpdateTest::TClientId aClientId);
	RDevSoundPolicyUpdateTest* iParent;
	RDevSoundPolicyUpdateTest::TClientId iSelfId;
	};


const TInt KMaxStringLength = 255;	// Used to define DTMF length

class MStoppableTestStep
	{
	public:
		virtual void CallStopTest(TInt aError) = 0;
	};

/*
 Base class for all DevSound Volume ramp tests
 */
class RDevSoundTestBase: public RAsyncTestStep, public MDevSoundObserver, public MStoppableTestStep
	{
public:
	// DevSound events
	enum TMmfDevSoundEvent
		{
		EEventInitialize = 0,
		EEventInitComplete,
		EEventBTBF,
		EEventBTBE,
		EEventTimerComplete
		};

	// DevSound states
	enum TMmfDevSoundState
		{
		EStateInitial = 0,
		EStateCreated,
		EStateInitializing,
		EStatePlaying,
		EStateRecording,
		EStatePause
		};

	enum TPanicCodes
		{
		EFsmIncorrectErrorPassed = 0,
		EInvalidCallbackCall
		};

	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	virtual void CallStopTest(TInt aError);

protected:
	RDevSoundTestBase(const TDesC& aTestName);
	virtual void DoKickoffTestL() = 0;
	virtual void DoTimerCallback();
	static TInt TimerCallback(TAny*);
	void StartTimer(TTimeIntervalMicroSeconds32 aWaitTime);
	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);

	// Executes events of DevSound using a Finite State Machine
	virtual void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError) = 0;
	void Panic(TInt aReason);

protected:
	TMmfDevSoundState iDevSoundState;
	CMMFDevSound* iMMFDevSound;
	CPeriodic* iTimer;
	};

class RDevSoundDualToneVolumeRampTest : public RDevSoundTestBase
	{
public:
	static RDevSoundDualToneVolumeRampTest* NewL(const TDesC& aTestName);
private:
	RDevSoundDualToneVolumeRampTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iVolumeRamp;
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iFrequencyTone2;
	};



class RDevSoundDTMFStringVolumeRampTest : public RDevSoundTestBase
	{
public:
	static RDevSoundDTMFStringVolumeRampTest* NewL(const TDesC& aTestName);
private:
	RDevSoundDTMFStringVolumeRampTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iVolumeRamp;
	TBuf16<KMaxStringLength> iDTMFString;
	};

class RDevSoundToneSequenceVolumeRampTest : public RDevSoundTestBase
	{
public:
	static RDevSoundToneSequenceVolumeRampTest* NewL(const TDesC& aTestName);
private:
	RDevSoundToneSequenceVolumeRampTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iVolumeRamp;
	};

class RDevSoundDualToneSetVolumeRampWhilePlayingTest : public RDevSoundTestBase
	{
public:
	static RDevSoundDualToneSetVolumeRampWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RDevSoundDualToneSetVolumeRampWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);

private:
	TInt iVolume;
	TInt iVolumeRamp;
	TInt iDuration; 
	TInt iFrequencyTone1;
	TInt iFrequencyTone2;
	TBool iTonePlayedTwice;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	TInt iTimesToneFinishedCalled;
	};

class RDevSoundDTMFStringSetVolumeRampWhilePlayingTest : public RDevSoundTestBase
	{
public:
	static RDevSoundDTMFStringSetVolumeRampWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RDevSoundDTMFStringSetVolumeRampWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);

private:
	TInt iVolume;
	TInt iVolumeRamp;
	TBool iFirstToneFinishedCall;
	TBuf16<KMaxStringLength> iDTMFString;
	TInt iDTMFPauses;
	TInt iDTMFTones;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;	
	TInt iTimesToneFinishedCalled;	
	};

class RDevSoundToneSequenceSetVolumeRampWhilePlayingTest : public RDevSoundTestBase
	{
public:
	static RDevSoundToneSequenceSetVolumeRampWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RDevSoundToneSequenceSetVolumeRampWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);

private:
	TInt iVolume;
	TInt iVolumeRamp;
	TBool iFirstToneFinishedCall;
	};

//Test case for PDEF121315  - Cannot update the observer specified with InitializeL
class CMultiInitTestDevSoundObserver;
class RDevSoundMultiInitTest : public RAsyncTestStep
	{
public:	
enum EMultiInitTestEvent
	{
	EDevSoundInitComplete,
	EBTBF
	};
enum EClientId
	{
	EObserver1,
	EObserver2
	};
enum ETestState
	{
	//states for testing CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode)
	EInit1Observer1, //DevSound initialised with Observer1
	EInit1Observer2, //DevSound initialised with Observer2
	//states for testing - CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode);
	EInit2Observer1, //DevSound initialised with Observer1
	EInit2Observer2, //DevSound initialised with Observer2
	//states for testing - CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, TFourCC aDesiredFourCC, TMMFState aMode);
	EInit3Observer1, //DevSound initialised with Observer1
	EInit3Observer2  //DevSound initialised with Observer2
	};

	static RDevSoundMultiInitTest* NewL(const TDesC& aStepName);
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();	
	
	void Fsm(EClientId aClientId, EMultiInitTestEvent aTestEvent, TInt aError);
	void FsmL(EClientId aClientId, EMultiInitTestEvent aTestEvent, TInt aError);
private:
	RDevSoundMultiInitTest(const TDesC& aStepName);
	CMMFDevSound* iMMFDevSound;
	CMultiInitTestDevSoundObserver* iDevSoundObserver1;
	CMultiInitTestDevSoundObserver* iDevSoundObserver2;
	TInt iTestState;
	};

class CMultiInitTestDevSoundObserver : public CBase, public MDevSoundObserver
	{
public:	
	static CMultiInitTestDevSoundObserver* NewL(RDevSoundMultiInitTest* aParent, RDevSoundMultiInitTest::EClientId aClientId);
	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
private:
	CMultiInitTestDevSoundObserver(RDevSoundMultiInitTest* aParent, RDevSoundMultiInitTest::EClientId aClientId);
	RDevSoundMultiInitTest* iParent;
	RDevSoundMultiInitTest::EClientId iSelfId;
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////							TruePause Devsound									/////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 *========================================================================================================
 * Constant declarations
 *========================================================================================================
 */

const TInt KSizeBuf = 256;
const TInt KMinimumPriority = -100;
const TInt KMaximumPriority = 100;
const TInt K8Bits = 8;
const TInt KTFourCC = 4;
const TInt KMaxSampleRateStringLength = 21;
const TInt KMaxChannelsStringLength = 10;
const TInt KDTMFTonedefaultToneOn = 250000;
const TInt KDTMFTonedefaultToneOff = 50000;
const TInt KDTMFTonedefaultPauseLength = 250000;
const TInt K25Percent = 4;
const TInt KPanicLength = 16;

_LIT(KMsgErrorFourccLength,"Error in getting parameter length from INI file %d");
_LIT(KPlayErrorText, "PlayError");
_LIT(KRecordErrorText, "RecordError");
_LIT(KToneFinishedText, "ToneFinished");

_LIT(KEMMFSampleRate8000Hz,"EMMFSampleRate8000Hz");
_LIT(KEMMFSampleRate11025Hz,"EMMFSampleRate11025Hz");
_LIT(KEMMFSampleRate16000Hz,"EMMFSampleRate16000Hz");
_LIT(KEMMFSampleRate22050Hz,"EMMFSampleRate22050Hz");
_LIT(KEMMFSampleRate32000Hz,"EMMFSampleRate32000Hz");
_LIT(KEMMFSampleRate44100Hz,"EMMFSampleRate44100Hz");
_LIT(KEMMFSampleRate48000Hz,"EMMFSampleRate48000Hz");
_LIT(KEMMFSampleRate88200Hz,"EMMFSampleRate88200Hz");
_LIT(KEMMFSampleRate96000Hz,"EMMFSampleRate96000Hz");
_LIT(KEMMFSampleRate12000Hz,"EMMFSampleRate12000Hz");
_LIT(KEMMFSampleRate24000Hz,"EMMFSampleRate24000Hz");
_LIT(KEMMFSampleRate64000Hz,"EMMFSampleRate64000Hz");

_LIT(KEMMFMono,"EMMFMono");
_LIT(KEMMFStereo,"EMMFStereo");

// Section name keys
_LIT(KSampleRate, "SampleRate");
_LIT(KSampleRate2, "SampleRate2");
_LIT(KChannel, "Channel");
_LIT(KFilename, "Filename");
_LIT(KDurationEvent, "DurationEvent");
_LIT(KOtherFilename, "OtherFilename");
_LIT(KRRecordBalance, "RRecordBalance");
_LIT(KLRecordBalance, "LRecordBalance");
_LIT(KGain, "Gain");
_LIT(KLSpeakerBalance, "LSpeakerBalance");
_LIT(KRSpeakerBalance, "RSpeakerBalance");
_LIT(KInvalidToneSequence,"InvalidToneSequence");
_LIT(KFourccCode, "FourccCode");
_LIT(KRecordDataFollowingResume, "RecordDataFollowingResume");
_LIT(KRecordDataPriorToResume, "RecordDataPriorToResume");
_LIT(KCheckForNoDataAfterResume, "CheckForNoDataAfterResume");
_LIT(KCallResumeAfterResume, "CallResumeAfterResume ");

enum TPanicCodes
	{
	EFsmIncorrectErrorPassed = 0,
	EInvalidCallbackCall
	};
	
// DevSound events
enum TMmfDevSoundEvent
	{
	EEventInitialize = 0,
	EEventInitComplete,
	EEventBTBF,
	EEventBTBE,
	EEventTimerComplete
	};

// DevSound states
enum TMmfDevSoundState
	{
	EStateInitial = 0,
	EStateCreated,
	EStateInitializing,
	EStatePlaying,
	EStateRecording,
	EStatePause
	};

// User defined "devSound client" error codes
enum TMmfDevSoundClientErrorCodes
	{
	EInvalidClientFSMEvent = 1,
	EInvalidClientFSMState,
	EReadFileErrorInClient,
	EWriteFileErrorInClient
	};

// Panic function
static void Panic(const TDesC &aCategory, TInt aReason)
	{
	User::Panic(aCategory, aReason);
	}

/*
 *========================================================================================================
 * Audio play client
 *========================================================================================================
 */

class MA3FDevsoundPlayAudioClientObserver
	{
public:
	virtual void ClientInitializeCompleteCallback(TInt aError) = 0;
	virtual void ClientBufferToBeFilledCallback(TInt aError) = 0;
	virtual void ClientPlayErrorCallback(TInt aError, TBool aLastBuffer) = 0;
	virtual void ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)=0;
	};

class CA3FDevSoundPlayClient : public CBase, public MDevSoundObserver
	{
public:
	static CA3FDevSoundPlayClient* NewL(MA3FDevsoundPlayAudioClientObserver &aObserver,TFourCC aFourccCode, RFs& aFs, CTestSuite* aSuite = NULL);
	virtual ~CA3FDevSoundPlayClient();
	// New functions
	void SetPriority(TInt priority);
	TInt PlayInit(const TDesC& aFileName);
	TInt PlayAudio();
	void StopPlay();
private:
	CA3FDevSoundPlayClient(MA3FDevsoundPlayAudioClientObserver &aObserver, TFourCC aFourccCode, RFs& aFs, CTestSuite* aSuite);
	void ConstructL();
	TInt Fsm(TMmfDevSoundEvent aDevSoundEvent);
	// from MDevSoundObserver
	void BufferToBeFilled(CMMFBuffer *aBuffer);
	void BufferToBeEmptied(CMMFBuffer *aBuffer);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void ConvertError(TInt aError);
	void InitializeComplete(TInt aError);
	void PlayError(TInt aError);
	void RecordError(TInt aError);
	void ToneFinished(TInt aError);
	void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
			TRefByValue<const TDesC16> aFmt,...);
	TMmfDevSoundState iDevSoundState;
	CMMFDevSound* iDevSound;
	MA3FDevsoundPlayAudioClientObserver &iObserver;
	CMMFBuffer* iBuffer;
	TFileName iFilename;
	// Handle to a file server session
	RFs iFs;
	// Handle for the audio file for playing
	RFile iFile;
	TFourCC iFourccCode;
	CTestSuite* iSuite;
	};

/*
 * TIntegrationTestLog16OverflowClient
 */
class TIntegrationTestLog16OverflowClient :public TDes16Overflow
	{
public:
	// TDes16Overflow pure virtual
	virtual void Overflow(TDes16& aDes);
	};

/*
 *========================================================================================================
 * Base class for TruePause DevSound test steps in this suite.
 *========================================================================================================
 */

class CAsyncWriteBufferToFile;

class RA3FDevSoundTestBase: public RAsyncTestStep, public MDevSoundObserver, public MStoppableTestStep
	{
public:
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	virtual void CallStopTest(TInt aError);
protected:
	RA3FDevSoundTestBase(const TDesC& aTestName);
	virtual void DoKickoffTestL() = 0;
	virtual void DoTimerCallback();
	static TInt TimerCallback(TAny*);
	void StartTimer(TTimeIntervalMicroSeconds32 aWaitTime);
	// Executes events of DevSound using a Finite State Machine
	virtual void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError) = 0;
	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);
	void SampleRateFromTIntToTMMFSampleRate(TInt aSampleRate, TMMFSampleRate &aESampleRate);
	void SampleRateFromTUintToString(TUint aSampleRate, TDes& aStringSampleRate);
	void ChannelsFromTUintToString(TUint aChannels, TDes& aStringChannels);
	void EncodingFromStringToTFourCC(const TDesC& aFourCCString);
	void PrintSupportedCapabilities(TUint aSampleRate,TUint aChannel);
protected:
	TBuf<KSizeBuf> iFourCCString;
	TFourCC iFourCCCode;
	TMmfDevSoundState iDevSoundState;
	CMMFDevSound* iMMFDevSound;
	CPeriodic* iTimer;
	CMMFBuffer* iBuffer;
	// Handle for the audio file for playing and recording
	RFile iFile;
	// Handle to a file server session
	RFs iFs;
	CAsyncWriteBufferToFile* iAsyncWriteBTFAO;
	};

class TSampleRateToTMMFSampleRate
	{
public:
	TInt iTIntSampleRate;
	TMMFSampleRate iTMMFSampleRate;
	};

class TSampleRateToString
	{
public:
	TUint iTUIntSampleRate;
	TPtrC iTPtrSampleRate;
	};

class TChannelsToString
	{
public:
	TUint iTUIntChannels;
	TPtrC iTPtrChannels;
	};

/*
 *========================================================================================================
 * CAsyncWriteBufferToFile
 *========================================================================================================
 */

class CAsyncWriteBufferToFile : public CActive
	{
public:
	static CAsyncWriteBufferToFile* NewL(RFile& aFile, CMMFDevSound* aDevSound, MStoppableTestStep* aTestStep);
	~CAsyncWriteBufferToFile();
	void Start(CMMFDataBuffer* aBuffer);

private:
	CAsyncWriteBufferToFile(RFile& aFile, CMMFDevSound* aDevSound, MStoppableTestStep* aTestStep);
	void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);

private:
	RFile& iFile;
	CMMFDevSound* iDevSound;
	MStoppableTestStep* iTestStep;
	};

/*
 *========================================================================================================
 * Play base class
 *========================================================================================================
 */

class RA3FDevSoundTestPlay : public RA3FDevSoundTestBase
	{
protected:
	RA3FDevSoundTestPlay(const TDesC& aTestName);
	~RA3FDevSoundTestPlay();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
protected:
	TFileName iFilename;
	TFileName iOtherFilename;
	TInt iSampleRate;
	TInt iSampleRate2;
	TInt iChannels;
	TInt iLSpeakerBalance;
	TInt iRSpeakerBalance;
	TInt iVolume;
	
	TBool iClientRejectedClause;
	TBool iClientPreemptedClause;
	TBool iStopClause;
	TBool iVolumeBalanceClause;
	TBool iConfigClause;
	TBool iCompletePlayBack;
	TBool iInitAfterPauseClause;
	TInt iErrExpected;
	
	TMMFSampleRate iESampleRate;
	TMMFCapabilities iCapabilitiesSet;
	CA3FDevSoundPlayClient* iDevsoundPlayClient;
	};

/*
 *========================================================================================================
 * Record base class
 *========================================================================================================
 */

class RA3FDevSoundTestRecord : public RA3FDevSoundTestBase
	{
protected:
	RA3FDevSoundTestRecord(const TDesC& aTestName);
	void Reset();
	~RA3FDevSoundTestRecord();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();	
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
protected:
	TInt iDuration;
	TInt iSampleRate;
	TInt iSampleRate2;
	TInt iLRecordBalance;
	TInt iRRecordBalance;
	TInt iChannels;
	TInt iGain;
	TBool isSecondTimeRecording;
	TBool iNoWaitLastBufferClause;
	TBool iPCMFormatClause;
	TBool iGainBalanceClause;
	TBool iConfigClause;
	TMMFSampleRate iESampleRate;
	TMMFCapabilities iCapabilitiesSet;
	TBool iFollowingResume;
	TBool iRecordDataFollowingResume;
	TBool iRecordDataPriorToResume;
	TBool iCheckForNoDataAfterResume;
	TBool iInitAfterPauseClause;
	TBool iResumeAfterResume;
	};

/*
 *========================================================================================================
 * Tone base class
 *========================================================================================================
 */

class RA3FDevSoundTestTone : public RA3FDevSoundTestBase
	{
protected:
	RA3FDevSoundTestTone(const TDesC& aTestName);
	~RA3FDevSoundTestTone();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
protected:
	enum TPlayToneType
		{
		EPlayTone,
		EPlayDualTone,
		EPlayDTMFString,
		EPlayToneSequence
		};
protected:
	TInt iDuration;
	TInt iDurationEvent;
	TInt iFrequencyTone1;
	TInt iFrequencyTone2;
	TInt iLSpeakerBalance;
	TInt iRSpeakerBalance;
	TInt iVolume;
	TBool iVolumeBalanceClause;
	TBool iPaused;
	TBuf16<KMaxStringLength> iDTMFString;
	TInt iDTMFPauses;
	TInt iDTMFTones;
	TPlayToneType iPlayToneType;
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0081-HP
 */

class RA3FDevSoundPauseAndResumePlayingTest : public RA3FDevSoundTestPlay
	{
public:
	static RA3FDevSoundPauseAndResumePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPauseAndResumePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0082-HP
 */
class RA3FDevSoundPlayRejectionTestWhilePausing : public RA3FDevSoundTestPlay, public MA3FDevsoundPlayAudioClientObserver
	{
public:
	static RA3FDevSoundPlayRejectionTestWhilePausing* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPlayRejectionTestWhilePausing(const TDesC& aTestName);
	void DoKickoffTestL();
	void PlayError(TInt aError);
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientBufferToBeFilledCallback(TInt aError);
	void ClientPlayErrorCallback(TInt aError, TBool aLastBuffer);
	void ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError);
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0083-HP
 */
class RA3FDevSoundPlayPreemptionTestWhilePausing : public RA3FDevSoundTestPlay, public MA3FDevsoundPlayAudioClientObserver
	{
public:
	static RA3FDevSoundPlayPreemptionTestWhilePausing* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPlayPreemptionTestWhilePausing(const TDesC& aTestName);
	void DoKickoffTestL();
	void PlayError(TInt aError);
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientBufferToBeFilledCallback(TInt aError);
	void ClientPlayErrorCallback(TInt aError, TBool aLastBuffer);
	void ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError);
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0084-HP
 */
class RA3FDevSoundPlayPauseAndStopPlayingTest : public RA3FDevSoundTestPlay
	{
public:
	static RA3FDevSoundPlayPauseAndStopPlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPlayPauseAndStopPlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0085-HP
 */
class RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest : public RA3FDevSoundTestPlay
	{
public:
	static RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0086-HP
 */
class RA3FDevSoundPauseSetConfigAndResumePlayingTest : public RA3FDevSoundTestPlay
	{
public:
	static RA3FDevSoundPauseSetConfigAndResumePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPauseSetConfigAndResumePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0087-HP
 */

class RNonA3FDevSoundPauseAndResumePlayingTest : public RA3FDevSoundTestPlay
	{
public:
	static RNonA3FDevSoundPauseAndResumePlayingTest* NewL(const TDesC& aTestName);
private:
	RNonA3FDevSoundPauseAndResumePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0088-HP
 */

class RA3FDevSoundPauseAndResumeRecordingPCMTest : public RA3FDevSoundTestRecord
	{
public:
	static RA3FDevSoundPauseAndResumeRecordingPCMTest* NewL(const TDesC& aTestName);
protected:
	RA3FDevSoundPauseAndResumeRecordingPCMTest(const TDesC& aTestName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0089-HP
 */

class RA3FDevSoundPauseAndResumeRecordingPCMDoNotWaitForLastBufferTest : public RA3FDevSoundTestRecord
	{
public:
	static RA3FDevSoundPauseAndResumeRecordingPCMDoNotWaitForLastBufferTest* NewL(const TDesC& aTestName);
protected:
	RA3FDevSoundPauseAndResumeRecordingPCMDoNotWaitForLastBufferTest(const TDesC& aTestName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0090-HP
 */

class RA3FDevSoundPauseAndResumeRecordingNonPCMTest : public RA3FDevSoundTestRecord
	{
public:
	static RA3FDevSoundPauseAndResumeRecordingNonPCMTest* NewL(const TDesC& aTestName);
protected:
	RA3FDevSoundPauseAndResumeRecordingNonPCMTest(const TDesC& aTestName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0091-HP
 */

class RA3FDevSoundPauseSetGainBalanceAndResumeRecordingPCMTest : public RA3FDevSoundTestRecord
	{
public:
	static RA3FDevSoundPauseSetGainBalanceAndResumeRecordingPCMTest* NewL(const TDesC& aTestName);
protected:
	RA3FDevSoundPauseSetGainBalanceAndResumeRecordingPCMTest(const TDesC& aTestName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0092-HP
 */

class RA3FDevSoundPauseSetConfigAndResumeRecordingPCMTest : public RA3FDevSoundTestRecord
	{
public:
	static RA3FDevSoundPauseSetConfigAndResumeRecordingPCMTest* NewL(const TDesC& aTestName);
protected:
	RA3FDevSoundPauseSetConfigAndResumeRecordingPCMTest(const TDesC& aTestName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0093-HP
 */

class RA3FDevSoundTonePlayTonePauseAndResume : public RA3FDevSoundTestTone
	{
public:
	static RA3FDevSoundTonePlayTonePauseAndResume* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayTonePauseAndResume(const TDesC& aTestName);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0094-HP
 */

class RA3FDevSoundTonePlayDualTonePauseAndResume : public RA3FDevSoundTestTone
	{
public:
	static RA3FDevSoundTonePlayDualTonePauseAndResume* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDualTonePauseAndResume(const TDesC& aTestName);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0095-HP
 */

class RA3FDevSoundTonePlayDTMFStringPauseAndResume : public RA3FDevSoundTestTone
	{
public:
	static RA3FDevSoundTonePlayDTMFStringPauseAndResume* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDTMFStringPauseAndResume(const TDesC& aTestName);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0096-HP
 */

class RA3FDevSoundTonePlayToneSequencePauseAndResume : public RA3FDevSoundTestTone
	{
public:
	static RA3FDevSoundTonePlayToneSequencePauseAndResume* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneSequencePauseAndResume(const TDesC& aTestName);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0097-HP
 */

class RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume : public RA3FDevSoundTestTone
	{
public:
	static RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume(const TDesC& aTestName);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0098-HP
 */

class RA3FDevSoundTonePlayToneSequenceSetVolumeBalancePauseAndResume : public RA3FDevSoundTestTone
	{
public:
	static RA3FDevSoundTonePlayToneSequenceSetVolumeBalancePauseAndResume* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneSequenceSetVolumeBalancePauseAndResume(const TDesC& aTestName);
	void DoKickoffTestL();
	};


/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0099-HP
 */
class RA3FDevSoundPlayPauseFlushResumeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayPauseFlushResumeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPlayPauseFlushResumeTest(const TDesC& aTestName);
	
	//from RA3FDevSoundTestBase
	virtual void DoKickoffTestL();	
	virtual void DoTimerCallback();
	virtual void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	
private:
	TFileName iFileName;	
	};

	
const TInt KTimerCallBackFinished = 0x12345;

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0103-HP
 */
 	
// For testing SMP SAFE condition for Custom Interface
class RTestStepMultipleCustomInterface : public RTestStep
	{
public:
	static RTestStepMultipleCustomInterface* NewL();

	// from RTestStep
	TVerdict DoTestStepL();
	TVerdict DoTestStepPostambleL();
	
protected:
	TVerdict TestCustomInterfaceL(MMMFDevSoundTestCustomInterface* aInterface1, MMMFDevSoundTestCustomInterface* aInterface2);
	RTestStepMultipleCustomInterface();
	
public:
	TInt iTimerFlag;
	TInt iAsyncCompleteFlag;	

private:
    CPeriodic* iPeriodic;
	};

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0104-HP
 */
 	
// Test that overflows during recording are ignored by the framework
class RTestStepRecordOverflow : public RDevSoundTestBase
	{
public:
	static RTestStepRecordOverflow* NewL(const TDesC& aTestName);
	~RTestStepRecordOverflow();	
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	virtual void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void HandleCallbackTimeout();
	static TInt TimerCallBack(TAny* aPtr);
private:
	RTestStepRecordOverflow(const TDesC& aTestName);
private:
	TInt iBuffersReceived;
	RFs iFs;
	RFile iFile;
	CAsyncWriteBufferToFile* iAsyncWriteBTFAO;
	CPeriodic* iSimulateOverflowTimer;
	TInt iLastSamplesRecorded;
	};	

/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0105-HP
 */

class RA3FDevSoundPauseAndInitRecordingPCMTest : public RA3FDevSoundTestRecord
    {
public:
    static RA3FDevSoundPauseAndInitRecordingPCMTest* NewL(const TDesC& aTestName);
protected:
    RA3FDevSoundPauseAndInitRecordingPCMTest(const TDesC& aTestName);
    void DoKickoffTestL();
    };


/*
 *========================================================================================================
 * MM-MMF-DEVSOUND-U-0199-HP
 */

class RA3FDevSoundPauseAndResumeAndInitPlayingTest : public RA3FDevSoundTestPlay
    {
public:
    static RA3FDevSoundPauseAndResumeAndInitPlayingTest* NewL(const TDesC& aTestName);
private:
    RA3FDevSoundPauseAndResumeAndInitPlayingTest(const TDesC& aTestName);
    void DoKickoffTestL();
    };

/*
 *========================================================================================================
  */
	
/**
 *
 * CMultipleAsyncListener provides the asynchronous operation
 * of an active object. Used for ensuring that only when two async calls 
 * are complete, only then CActiveScheduler::Stop is called.
 *
 * @lib "TSU_MMF_DEVSOUND_SUITE.lib"
 *
 *
 */
class CMultipleAsyncListener : public CActive
	{
public:
	CMultipleAsyncListener(RTestStepMultipleCustomInterface* aMultipleCustomInterface) : CActive(CActive::EPriorityStandard),iMultipleCustomInterface(aMultipleCustomInterface) 
		{ 
		CActiveScheduler::Add(this);
		}
	void InitialiseActiveListener()
		{
		iStatus = KRequestPending; SetActive();
		}
	~CMultipleAsyncListener()
		{
		Cancel();
		}
	// From CActive
	virtual void RunL()
		{
		iMultipleCustomInterface->iAsyncCompleteFlag++;
		if(iMultipleCustomInterface->iAsyncCompleteFlag == 2)
			{
			CActiveScheduler::Stop();
			}
		}
	virtual void DoCancel()
		{
		
		};
		
	public:
	
	RTestStepMultipleCustomInterface* iMultipleCustomInterface;	
	};
		

//------------------------------------------------------------------

#endif// __TSU_MMF_DEVSOUND_STEP_H__

