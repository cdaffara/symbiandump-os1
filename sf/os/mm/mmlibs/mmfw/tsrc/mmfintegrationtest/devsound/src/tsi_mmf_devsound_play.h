// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Integration test class declarations for DevSound - Play
// 
//


#ifndef TESTSTEPDEVSOUNDPLAY_H
#define TESTSTEPDEVSOUNDPLAY_H

#include "tsi_mmf_devsound_stepbase.h"

class CMmfDevSoundPlayTimer;

/**
 *
 * RTestStepDevSoundPlayBase
 *
 */
class RTestStepDevSoundPlayBase : public RTestStepDevSoundBase, public MDevSoundObserver
    {
public:
    RTestStepDevSoundPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType=KMMFFourCCCodeNULL);

    // Executes events of DevSound in sequence
    void Fsm(TMmfDevSoundEvents aDevSoundEvent);

    // from RTestStep, implementation to be used by all tests
    TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();

    // from MDevSoundObserver
    void InitializeComplete(TInt aError);
    void ToneFinished(TInt aError);
    void BufferToBeFilled(CMMFBuffer* aBuffer);
    void PlayError(TInt aError);
    void BufferToBeEmptied(CMMFBuffer* aBuffer);
    void RecordError(TInt aError);
    void ConvertError(TInt aError);
    void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);

    // timer handling
    virtual void Finished();
    
    // copy file
    TInt CopyFile(TPtrC aFileNameSrc, TPtrC aFileNameDst);
        
protected:
    // Set last buffer to be played
    TBool iLastBuffer;

    // Number of buffers played
    TInt iBufferCount;
    };

// Positive Tests

/**
 *
 * RTestStepDevSoundPlayDatatype
 * - Play an OggVorbis audio file.
 *  -Play an Ogg vorbis audio file specifying invalid input data type.
 * 
 */
class RTestStepDevSoundPlayDatatype : public RTestStepDevSoundPlayBase
    {
public:
    static RTestStepDevSoundPlayDatatype* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);

    // from RTestStep
    TVerdict DoTestStepL();

private:
    RTestStepDevSoundPlayDatatype(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);
    };

/**
 *
 * RTestStepDevSoundPlayHwDevUid
 * - Play an OggVorbis audio file specifying Uid of HwDevice.
 * - Play an Ogg vorbis audio file specifying invalid Uid.
 *
 */
class RTestStepDevSoundPlayHwDevUid : public RTestStepDevSoundPlayBase
    {
public:
    static RTestStepDevSoundPlayHwDevUid* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

    // from RTestStep
    TVerdict DoTestStepL();

private:
    RTestStepDevSoundPlayHwDevUid(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    };

/**
 *
 * RTestStepDevSoundPlayConfHwDev
 * - Configure and reconfigure the audio HwDevice and play an OggVorbis audio file.
 * 
 */
class RTestStepDevSoundPlayConfHwDev : public RTestStepDevSoundPlayBase
    {
public:
    static RTestStepDevSoundPlayConfHwDev* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);

    // Executes events of DevSound in sequence
    void Fsm(TMmfDevSoundEvents aDevSoundEvent);

    // from RTestStep
    TVerdict DoTestStepL();

    // from MDevSoundObserver
    void InitializeComplete(TInt aError);
    void BufferToBeFilled(CMMFBuffer* aBuffer);
    void PlayError(TInt aError);

private:
    RTestStepDevSoundPlayConfHwDev(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);
    };

/**
 *
 * RTestStepDevSoundListInputDataTypes
 * - List of supported input and output data types.
 * 
 */
class RTestStepDevSoundListInputDataTypes: public RTestStepDevSoundPlayBase
    {
public:
    static RTestStepDevSoundListInputDataTypes* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);

    // from MDevSoundObserver
    void InitializeComplete(TInt aError);

    // from RTestStep
    TVerdict DoTestStepL();

private:
    RTestStepDevSoundListInputDataTypes(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);
    };

/**
 *
 * RTestStepDevSoundPositionData
 * - Position in stream data required for variable bit rate audio codec.
 * 
 */
class RTestStepDevSoundPositionData : public RTestStepDevSoundPlayBase
    {
public:
    static RTestStepDevSoundPositionData* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);

    // from RTestStep
    TVerdict DoTestStepL();

private:
    RTestStepDevSoundPositionData(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);
    };

/**
 *
 * RTestStepDevSoundPlayAlloc
 * - Alloc test to load an audio HwDevice to play an OggVorbis audio file.
 * 
 */
class RTestStepDevSoundPlayAlloc : public RTestStepDevSoundPlayBase
    {
public:
    static RTestStepDevSoundPlayAlloc* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);

    void AudioPlayAllocL();
    void FsmL(TMmfDevSoundEvents aDevSoundEvent);
    
    // from RTestStep
    TVerdict DoTestStepL();
    
    // from MDevSoundObserver
    void InitializeComplete(TInt aError);
    void BufferToBeFilled(CMMFBuffer* aBuffer);
    void PlayError(TInt aError);

private:
    RTestStepDevSoundPlayAlloc(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);
    TInt iError;
    };

/**
 *
 * RTestStepDevSoundPlayStop
 * - Play/stop playing an OggVorbis audio file.
 * 
 */
class RTestStepDevSoundPlayStop : public RTestStepDevSoundPlayBase
    {
public:
    static RTestStepDevSoundPlayStop* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);

    // Executes events of DevSound in sequence
    void Fsm(TMmfDevSoundEvents aDevSoundEvent);

    // from RTestStep
    TVerdict DoTestStepL();

    // from MDevSoundObserver
    void InitializeComplete(TInt aError);
    void BufferToBeFilled(CMMFBuffer* aBuffer);
    void PlayError(TInt aError);
    
    // timer handling
    void Finished();

private:
    RTestStepDevSoundPlayStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);

protected:
    CMmfDevSoundPlayTimer* iPlayTimer;
    TBool iRestarted;
    TInt iStopCount;
    };

/**
 *
 * RTestStepDevSoundPlayPause
 * - Play/pause playing an OggVorbis audio file.
 * 
 */
class RTestStepDevSoundPlayPause : public RTestStepDevSoundPlayBase
    {
public:
    static RTestStepDevSoundPlayPause* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);

    // Executes events of DevSound in sequence
    void Fsm(TMmfDevSoundEvents aDevSoundEvent);

    // from RTestStep
    TVerdict DoTestStepL();

    // from MDevSoundObserver
    void InitializeComplete(TInt aError);
    void BufferToBeFilled(CMMFBuffer* aBuffer);
    void PlayError(TInt aError);
    
    // timer handling
    void Finished();
    
private:
    RTestStepDevSoundPlayPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);
    
protected:
    CMmfDevSoundPlayTimer* iPlayTimer;
    TBool iRestarted;
    TInt iPauseCount;
    };

/**
 *
 * RTestStepDevSoundPlayRecord
 * - Play/record an OggVorbis audio file.
 * 
 */
class RTestStepDevSoundPlayRecord : public RTestStepDevSoundPlayBase
    {
public:
    static RTestStepDevSoundPlayRecord* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aKeyNameOutput, const TFourCC& aInputDataType);

    // Executes events of DevSound in sequence
    void Fsm(TMmfDevSoundEvents aDevSoundEvent);

    // from RTestStep
    TVerdict DoTestStepL();

    // from MDevSoundObserver
    void InitializeComplete(TInt aError);
    void BufferToBeFilled(CMMFBuffer* aBuffer);
    void PlayError(TInt aError);
    void BufferToBeEmptied(CMMFBuffer* aBuffer);
    void RecordError(TInt aError);
    
    // timer handling
    void Finished();

private:
    RTestStepDevSoundPlayRecord(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aKeyNameOutput, const TFourCC& aInputDataType);
    
protected:
    CMmfDevSoundPlayTimer* iPlayTimer;
    TInt iTestState;
    TBuf<KNameBufSize> iKeyNameOutput; // Key name for retrieving filename
    TInt iRetryTestState0;
    TInt iRunNextTestState;
    };


// Negative Tests

/**
 *
 * RTestStepDevSoundPlayInvBitrate
 * - Play an OggVorbis audio file at an invalid bit rate.
 * 
 */
class RTestStepDevSoundPlayInvBitrate : public RTestStepDevSoundPlayBase
    {
public:
    static RTestStepDevSoundPlayInvBitrate* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);

    // from RTestStep
    TVerdict DoTestStepL();

private:
    RTestStepDevSoundPlayInvBitrate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);
    };

/**
 *
 * RTestStepDevSoundPlayCorruptFile
 * - Play a corrupted OggVorbis audio file.
 * 
 */
class RTestStepDevSoundPlayCorruptFile : public RTestStepDevSoundPlayBase
    {
public:
    static RTestStepDevSoundPlayCorruptFile* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);

    // from RTestStep
    TVerdict DoTestStepL();

private:
    RTestStepDevSoundPlayCorruptFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType);
    };

/**
 * 
 * CMmfDevSoundPlayTimer 
 * - Timer for those test classes requiring timed stop
 *
 */
class CMmfDevSoundPlayTimer : public CTimer
    {
public:
    static CMmfDevSoundPlayTimer* NewL(RTestStepDevSoundPlayBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    void Start();
protected:
    TInt RunError(TInt aError);
    void RunL();
private:
    CMmfDevSoundPlayTimer(RTestStepDevSoundPlayBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    RTestStepDevSoundPlayBase* iParent;
    TTimeIntervalMicroSeconds32 iWaitTime;
    };

#endif// TESTSTEPDEVSOUNDPLAY_H
