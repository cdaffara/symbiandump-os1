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
// This file contains test step class declaration for DevSound integration tests.
// 
//


#ifndef TSI_MMF_TESTSTEP_DEVSOUND_H
#define TSI_MMF_TESTSTEP_DEVSOUND_H

// system includes
#include <testframework.h>
#include <mmf/server/sounddevice.h>

// forward declaration
class CTestSuiteDevSound;

//UIDs declaration
#define KUidHwDeviceAudioAdapter	0x10273791
#define KUidTestVorbisEncoderPU		0x102737CE 
#define KUidTestVorbisDecoderPU		0x102737D0 

// constants declaration
const TInt KArrayGranularity = 7;
const TInt KNameBufSize = 25;
const TInt KSizeBuf = 256;
const TInt KInvalidBitRateSetValue = 88888;
const TInt KInvalidBitRateGetValue = 80000;
const TInt KMicroSecsInOneSec = 1000000;
const TInt KMicroSecsInOneMinute = 60000000;
const TInt KMicroSecsInTenSec = 10000000;
const TInt KMicroSecsInThirtySec = 30000000;
const TUint KSampleRate8000Hz = 8000;

// Get the drive name
TAny GetDriveName(TDes& aFilename);

// DevSound events
enum TMmfDevSoundEvents
    {
    EDevSoundInit = 0,
    EDevSoundPlayInit,
    EDevSoundPlayData,
    EDevSoundPlayStopped,
    EDevSoundPlayPaused,
    EDevSoundRecordInit,
    EDevSoundRecordData,
    EDevSoundRecordStopped,
    EDevSoundRecordPaused,
    EDevSoundError
    };

// DevSound callbacks
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

// Format of the audio file
enum TTestFormat
    {
    ENone = 0,
    EPcm16Wav,
    EMulawRaw,
    E16BitAu,
    EAlawAu,
    EPcm16Au,
    EImaAdpcmWav,
    EImasPcmWav,
    EPcm8,
    EPcmU8,
    EPcmU16,
    EAlawWav,
    EEpocWve
    };

// Type of test whether it is normal test or considers Uid, for example
enum TTestType
    {
    ENormalTest = 0,
    EPositionTest,      // Specifies whether this is a position test or not.
    EUidTest,           // Specifies whether a test uses Uid or not, during initialization.
    EInvUidTest,        // Specifies whether a test uses invalid Uid for initialization.
    EBitRateTest,       // Specifies whether this is a bitrate test or not.
    EReconfigureTest,   // Whether audio HwDevice is reconfigured or not.
    EInvDataTypeTest    // Specifies whether this test uses invalid input/output data type.
    };

/**
 *
 * RTestStepDevSoundBase
 * base class for all DevSound test steps in this suite.
 *
 */
class RTestStepDevSoundBase : public RTestStep
    {
public:
    RTestStepDevSoundBase();

    // Set the suite name
    void SetTestSuite(const CTestSuiteDevSound* aTestSuite);

protected:
    // Member function to start and stop the scheduler, setting the flags
    void StartScheduler();
    void StopScheduler();
    
    // Pointer to suite which owns this test
    const CTestSuiteDevSound* iTestSuiteDevSound;

    // Section name for retrieving filename
    TBuf<KNameBufSize> iSectName;

    // Key name for retrieving filename
    TBuf<KNameBufSize> iKeyName;

    // Active Scheduler and Flag to check if scheduler is running (to avoid PANIC : E32USER-CBASE:45)
    CActiveScheduler* iActiveScheduler;
    TBool iIsSchedulerStarted;
    TBool iCanSchedulerStart;

    CMMFDevSound* iMMFDevSound;
    CMMFBuffer* iBuffer;

    // Filename of audio file
    TBuf<KSizeBuf> iFilename;

    // Input and output data types for initilizing DevSound for playing and recording
    TFourCC iInputDataType;
    TFourCC iOutputDataType;
    
    // Uid of the HwDevice used in the test
    TUid iDecoderHwDevUid;
    TUid iEncoderHwDevUid;

    // Creating and opening the audio file for playing and recording
    RFile iFile;

    // Handle to a file server session
    RFs iFs;

    TTestType iTestType;
    };

#endif    // TSI_MMF_TESTSTEP_DEVSOUND_H
