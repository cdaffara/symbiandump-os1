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
// Declarations of Integration test step classes for A3F DevSound Record cases.
// 
//

#ifndef TSI_A3F_DEVSOUND_RECORD_H
#define TSI_A3F_DEVSOUND_RECORD_H

// User includes
#include "tsi_a3f_devsound_teststep.h"

// Forward class references
class CAsyncWriteBufferToFile;
class CAsyncReadBufferFromFile;

// Enumerations
enum TDevSoundRecordState
	{
	EStateRecordStart = 0,
	EStateRecordInitializing,
	EStateRecording,
	EStateRecordPaused
	};

enum TDevSoundRecordEvents
	{
	EEventRecordStart = 0,
	EEventInitCompleteRecord,
	EEventBTBE,
	EEventRecordTimerComplete
	};

enum TSampleRates
	{
	E8000Hz = 0,
	E11025Hz,
	E12000Hz,
	E16000Hz,
	E22050Hz,
	E24000Hz,
	E32000Hz,
	E44100Hz,
	E48000Hz,
	E64000Hz,
	E88200Hz,
	E96000Hz,
	ESampleRateInvalid
	};

enum TChannelModes
	{
	EMono = 0,
	EStereo,
	EChannelModeInvalid
	};

/*
 *
 * RTestStepA3FDevSoundRecordBase
 *
 */
class RTestStepA3FDevSoundRecordBase : public RAsyncTestStep,
									   public MDevSoundObserver
	{
public:
	// From RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();

	void CallStopTest(TInt aError);

protected:
	RTestStepA3FDevSoundRecordBase(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);
	RTestStepA3FDevSoundRecordBase(const TDesC& aTestName, const TFourCC& aOutputDataType);

	virtual void DoKickoffTestL();

	// Executes the recording events of DevSound using a Finite State Machine
	virtual void Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError);

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
	TInt SampleRateLookup(const TDesC& aSampleRate);
	TInt ChannelModeLookup(const TDesC& aChannelMode);
	void SetSampleRateAndChannelMode();
	void CheckSampleRateAndChannelMode();
	void SetRecordBalanceFromIni();
	void CheckRecordBalance();
	void DisplayCodecInfo();
	void DisplaySamplesAndBuffersRecorded();
	void StartRecordTimer();
	void WriteBufferToFileAndRecord();

protected:
	RFs iFs;
	RFile iFile;
	CMMFDevSound* iMMFDevSound;
	CMMFBuffer* iBuffer;
	CPeriodic* iRecordTimer;
	CAsyncWriteBufferToFile* iAsyncWriteBTFAO;
	TFourCC iOutputDataType; // datatype (FourCC) of the Encoder
	TBuf<KNameSize> iSectName;
	TFileName iFilename;
	TPtrC iSampleRate;
	TPtrC iMonoStereo;
	TDevSoundRecordState iState;
	TInt iBufferCount;
	TInt iRecordTime;
	TInt iRecordInterval;
	TInt iLeftPercentage;
	TInt iRightPercentage;
	TInt iRecordCycleCounter;
	TBool iConfigureSettings;
	TBool iAsyncWriteBTF;
	TBool iLogBTBECallback;

	};

/*
 *
 * RStepA3FDevSoundRecordSimple
 *
 */
class RStepA3FDevSoundRecordSimple : public RTestStepA3FDevSoundRecordBase
	{
public:
	static RStepA3FDevSoundRecordSimple* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

private:
	RStepA3FDevSoundRecordSimple(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

	// From RTestStepA3FDevSoundRecordBase
	virtual void DoKickoffTestL();

	};

/*
 *
 * RStepA3FDevSoundRecordLong
 *
 */
class RStepA3FDevSoundRecordLong : public RTestStepA3FDevSoundRecordBase
	{
public:
	static RStepA3FDevSoundRecordLong* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

private:
	RStepA3FDevSoundRecordLong(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

	// From RTestStepA3FDevSoundRecordBase
	virtual void DoKickoffTestL();
	virtual void DoTimerCallback();

private:
	TInt iRecordCycleCounter;
	TInt iRecordIntervalMultiplier;

	};

/*
 *
 * RStepA3FDevSoundRecordMultipleTimes
 *
 */
class RStepA3FDevSoundRecordMultipleTimes : public RTestStepA3FDevSoundRecordBase
	{
public:
	static RStepA3FDevSoundRecordMultipleTimes* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

private:
	RStepA3FDevSoundRecordMultipleTimes(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

	// From RTestStepA3FDevSoundRecordBase
	virtual void DoKickoffTestL();

	};

/*
 *
 * RStepA3FDevSoundRecordAndPlay
 *
 */
class RStepA3FDevSoundRecordAndPlay : public RTestStepA3FDevSoundRecordBase
	{
public:
	static RStepA3FDevSoundRecordAndPlay* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);
	virtual void CloseTest();
	void BufferInfoPrinter(TInt aActual, TInt aExpected);

private:
	RStepA3FDevSoundRecordAndPlay(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

	// From RTestStepA3FDevSoundRecordBase
	virtual void DoKickoffTestL();

	// Executes the playing events of DevSound using a Finite State Machine
	void FsmPlayback(TDevSoundPlayEvents aDevSoundEvent, TInt aError);

	// From MDevSoundObserver
	virtual void InitializeComplete(TInt aError);
	virtual void BufferToBeEmptied(CMMFBuffer* aBuffer);
	virtual void BufferToBeFilled(CMMFBuffer* aBuffer);
	virtual void PlayError(TInt aError);

	void DisplaySamplesAndBuffersPlayed();

private:
	TDevSoundPlayState iPlayState;
	TFourCC iInputDataType; // Datatype of the Decoder to play back the recorded audio data
	CAsyncReadBufferFromFile* iAsyncReadBFFAO;
	TBool iAsyncReadBFF;
	TTime iStart;
	TTime iFinished;

	};

/*
 *
 * RStepA3FDevSoundSetGainDuringRecord
 *
 */
class RStepA3FDevSoundSetGainDuringRecord : public RTestStepA3FDevSoundRecordBase
	{
public:
	static RStepA3FDevSoundSetGainDuringRecord* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

private:
	RStepA3FDevSoundSetGainDuringRecord(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

	// From RTestStepA3FDevSoundRecordBase
	virtual void DoKickoffTestL();
	virtual void Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError);

	// From MDevSoundObserver
	virtual void BufferToBeEmptied(CMMFBuffer* aBuffer);

private:
	TInt iGain;
	TInt iCounter;
	TBool iCheckGain;
	TBool iSetGain;

	};

/*
 *
 * RStepA3FDevSoundInvalidModeRecord
 *
 */
class RStepA3FDevSoundInvalidModeRecord : public RTestStepA3FDevSoundRecordBase
	{
public:
	static RStepA3FDevSoundInvalidModeRecord* NewL(const TDesC& aTestName, const TFourCC& aOutputDataType);

	// From RAsyncTestStep
	virtual void KickoffTestL();

private:
	RStepA3FDevSoundInvalidModeRecord(const TDesC& aTestName, const TFourCC& aOutputDataType);

	// From RTestStepA3FDevSoundRecordBase
	virtual void Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError);

	};


/*
 *
 * RStepA3FDevSoundInitializeWhileRecording
 *
 */
class RStepA3FDevSoundInitializeWhileRecording : public RTestStepA3FDevSoundRecordBase
	{
public:
	static RStepA3FDevSoundInitializeWhileRecording* NewL(const TDesC& aTestName, const TFourCC& aOutputDataType);

	// From RAsyncTestStep
	virtual void KickoffTestL();

private:
	RStepA3FDevSoundInitializeWhileRecording(const TDesC& aTestName, const TFourCC& aOutputDataType);

	// From RTestStepA3FDevSoundRecordBase
	virtual void Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError);

	};

/*
 *
 * RStepA3FDevSoundCancelInitializeRecord
 *
 */
class RStepA3FDevSoundCancelInitializeRecord : public RTestStepA3FDevSoundRecordBase
	{
public:
	static RStepA3FDevSoundCancelInitializeRecord* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

private:
	RStepA3FDevSoundCancelInitializeRecord(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

	// From RTestStepA3FDevSoundRecordBase
	virtual void KickoffTestL();
	virtual void DoTimerCallback();
	virtual void StartRecordTimer();
	virtual void Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError);
	// From MDevSoundObserver
	virtual void InitializeComplete(TInt aError);
	
	};
	
/*
 *
 * RStepA3FDevSoundEmptyBuffersInitializedToRecord
 *
 */
class RStepA3FDevSoundEmptyBuffersInitializedToRecord : public RTestStepA3FDevSoundRecordBase
	{
public:
	static RStepA3FDevSoundEmptyBuffersInitializedToRecord* NewL(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

private:
	RStepA3FDevSoundEmptyBuffersInitializedToRecord(const TDesC& aTestName, const TDesC& aSectName, const TFourCC& aOutputDataType);

	// From RTestStepA3FDevSoundRecordBase
	virtual void KickoffTestL();
	virtual void Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError);
	
	};
	
/*
 *
 * RStepA3FDevSoundInitializeWhileRecording
 *
 */
class RStepA3FDevSoundEmptyBuffersWhileRecording : public RTestStepA3FDevSoundRecordBase
	{
public:
	static RStepA3FDevSoundEmptyBuffersWhileRecording* NewL(const TDesC& aTestName, const TFourCC& aOutputDataType);

	// From RAsyncTestStep
	virtual void KickoffTestL();

private:
	RStepA3FDevSoundEmptyBuffersWhileRecording(const TDesC& aTestName, const TFourCC& aOutputDataType);

	// From RTestStepA3FDevSoundRecordBase
	virtual void Fsm(TDevSoundRecordEvents aDevSoundEvent, TInt aError);

	};
	
/*
 *
 * CAsyncWriteBufferToFile
 *
 */

class CAsyncWriteBufferToFile : public CActive
	{
public:
	static CAsyncWriteBufferToFile* NewL(RFile& aFile, CMMFDevSound* aDevSound, RTestStepA3FDevSoundRecordBase& aTestStep);
	~CAsyncWriteBufferToFile();
	void Start(CMMFDataBuffer* aBuffer);

private:
	CAsyncWriteBufferToFile(RFile& aFile, CMMFDevSound* aDevSound, RTestStepA3FDevSoundRecordBase& aTestStep);
	void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);

private:
	RFile& iFile;
	CMMFDevSound* iDevSound;
	RTestStepA3FDevSoundRecordBase& iTestStep;

	};

/*
 *
 * CAsyncReadBufferFromFile
 *
 */

class CAsyncReadBufferFromFile : public CActive
	{
public:
	static CAsyncReadBufferFromFile* NewL(RFile& aFile, CMMFDevSound* aDevSound, RStepA3FDevSoundRecordAndPlay& aTestStep);
	~CAsyncReadBufferFromFile();
	void Start(CMMFDataBuffer* aBuffer);

private:
	CAsyncReadBufferFromFile(RFile& aFile, CMMFDevSound* aDevSound, RStepA3FDevSoundRecordAndPlay& aTestStep);
	void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);

private:
	RFile& iFile;
	CMMFDevSound* iDevSound;
	RStepA3FDevSoundRecordAndPlay& iTestStep;
	CMMFDataBuffer* iBuffer;

	};

#endif // TSI_A3F_DEVSOUND_RECORD_H
