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
// Integration test class declarations for DevSound - Record
// 
//


#ifndef TESTSTEPDEVSOUNDRECORD_H
#define TESTSTEPDEVSOUNDRECORD_H

#include "tsi_mmf_devsound_stepbase.h"

class CMmfDevSoundRecordTimer;

/**
 *
 * RTestStepDevSoundRecordBase
 *
 */
class RTestStepDevSoundRecordBase : public RTestStepDevSoundBase, public MDevSoundObserver
    {
public:
    RTestStepDevSoundRecordBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType=KMMFFourCCCodeNULL);

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
    
protected:
    // Number of buffers to be recorded
    TInt iBufferCount;
    CMmfDevSoundRecordTimer* iRecordTimer;
    TBool iRestarted;
    };

// Positive Tests

/**
 *
 * RTestStepDevSoundRecordDatatype
 * - Record an OggVorbis audio file.
 * - Record to an Ogg vorbis audio file specifying invalid output data type.
 * 
 */
class RTestStepDevSoundRecordDatatype : public RTestStepDevSoundRecordBase
    {
public:
    static RTestStepDevSoundRecordDatatype* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);

    // from RTestStep
    TVerdict DoTestStepL();

private:
    RTestStepDevSoundRecordDatatype(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);
    };

/**
 *
 * RTestStepDevSoundRecordHwDevUid
 * - Record an OggVorbis audio file, specifying Uid of audio HwDevice.
 * - Record to an Ogg vorbis audio file specifying invalid Uid.
 * 
 */
class RTestStepDevSoundRecordHwDevUid : public RTestStepDevSoundRecordBase
    {
public:
    static RTestStepDevSoundRecordHwDevUid* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);

    // from RTestStep
    TVerdict DoTestStepL();

private:
    RTestStepDevSoundRecordHwDevUid(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    };

/**
 *
 * RTestStepDevSoundRecordConfHwDev
 * - Configure and reconfigure the audio HwDevice and record an OggVorbis audio file.
 * 
 */
class RTestStepDevSoundRecordConfHwDev : public RTestStepDevSoundRecordBase
    {
public:
    static RTestStepDevSoundRecordConfHwDev* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);

    // Executes events of DevSound in sequence
    void Fsm(TMmfDevSoundEvents aDevSoundEvent);
    
    // from RTestStep
    TVerdict DoTestStepL();
    
    // from MDevSoundObserver
    void InitializeComplete(TInt aError);
    void BufferToBeEmptied(CMMFBuffer* aBuffer);
    void RecordError(TInt aError);
    
    // timer handling
    virtual void Finished();

private:
    RTestStepDevSoundRecordConfHwDev(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);

private:
    TBool iReconfigured;
    TBool iPaused;
    };

/**
 *
 * RTestStepDevSoundListOutputDataTypes
 * - List of supported input and output data types.
 * 
 */
class RTestStepDevSoundListOutputDataTypes: public RTestStepDevSoundRecordBase
    {
public:
    static RTestStepDevSoundListOutputDataTypes* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);

    // from MDevSoundObserver
    void InitializeComplete(TInt aError);

    // from RTestStep
    TVerdict DoTestStepL();

private:
    RTestStepDevSoundListOutputDataTypes(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);
    };

/**
 *
 * RTestStepDevSoundRecordAlloc
 * - Alloc test to load an audio HwDevice to record an OggVorbis audio file.
 * 
 */
class RTestStepDevSoundRecordAlloc : public RTestStepDevSoundRecordBase
    {
public:
    static RTestStepDevSoundRecordAlloc* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);

    void AudioRecordAllocL();
    void FsmL(TMmfDevSoundEvents aDevSoundEvent);
    
    // from RTestStep
    TVerdict DoTestStepL();
    
    // from MDevSoundObserver
    void InitializeComplete(TInt aError);
    void BufferToBeEmptied(CMMFBuffer* aBuffer);
    void RecordError(TInt aError);

private:
    RTestStepDevSoundRecordAlloc(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);
    TInt iError;
    };

/**
 *
 * RTestStepDevSoundRecordStop
 * - Record/stop recording an OggVorbis audio file.
 * 
 */
class RTestStepDevSoundRecordStop : public RTestStepDevSoundRecordBase
    {
public:
    static RTestStepDevSoundRecordStop* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);

    // Executes events of DevSound in sequence
    void Fsm(TMmfDevSoundEvents aDevSoundEvent);

    // from RTestStep
   	TVerdict DoTestStepL();

    // from MDevSoundObserver
    void InitializeComplete(TInt aError);
    void BufferToBeEmptied(CMMFBuffer* aBuffer);
    void RecordError(TInt aError);
    
    // timer handling
    void Finished();
    
private:
    RTestStepDevSoundRecordStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);

protected:
    CMmfDevSoundRecordTimer* iRecordStopTimer;
    TInt iStopCount;
    TBool iStopped;
    TBool iPaused;
    };

/**
 *
 * RTestStepDevSoundRecordPause
 * - Record/pause recording an OggVorbis audio file.
 * 
 */
class RTestStepDevSoundRecordPause : public RTestStepDevSoundRecordBase
    {
public:
    static RTestStepDevSoundRecordPause* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);

    // Executes events of DevSound in sequence
    void Fsm(TMmfDevSoundEvents aDevSoundEvent);

    // from RTestStep
    TVerdict DoTestStepL();

    // from MDevSoundObserver
    void InitializeComplete(TInt aError);
    void BufferToBeEmptied(CMMFBuffer* aBuffer);
    void RecordError(TInt aError);
    
    // timer handling
    void Finished();

private:
    RTestStepDevSoundRecordPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);
    
protected:
    CMmfDevSoundRecordTimer* iRecordPauseTimer;
    TInt iPauseCount;
    TBool iPaused;
    };

// Negative Tests

/**
 *
 * RTestStepDevSoundRecordInvBitrate
 * - Record to an OggVorbis audio file at an invalid bit rate.
 * 
 */
class RTestStepDevSoundRecordInvBitrate : public RTestStepDevSoundRecordBase
    {
public:
    static RTestStepDevSoundRecordInvBitrate* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);

    // from RTestStep
    TVerdict DoTestStepL();

private:
    RTestStepDevSoundRecordInvBitrate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);
    };

/**
 *
 * RTestStepDevSoundRecordCorruptFile
 * - Record to a corrupted OggVorbis audio file.
 * 
 */
class RTestStepDevSoundRecordCorruptFile : public RTestStepDevSoundRecordBase
    {
public:
    static RTestStepDevSoundRecordCorruptFile* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);

    // from RTestStep
    TVerdict DoTestStepL();

private:
    RTestStepDevSoundRecordCorruptFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType);
    };

/**
 * 
 * CMmfDevSoundRecordTimer 
 * - Timer for those test classes requiring timed stop
 *
 */
class CMmfDevSoundRecordTimer : public CTimer
    {
public:
    static CMmfDevSoundRecordTimer* NewL(RTestStepDevSoundRecordBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    void Start();
protected:
    TInt RunError(TInt aError);
    void RunL();
private:
    CMmfDevSoundRecordTimer(RTestStepDevSoundRecordBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    RTestStepDevSoundRecordBase* iParent;
    TTimeIntervalMicroSeconds32 iWaitTime;
    };

#endif// TESTSTEPDEVSOUNDRECORD_H
