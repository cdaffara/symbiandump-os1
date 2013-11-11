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
//

#include <e32base.h>
#include "tsi_mvs_audio.h"

//
// RTestStepMVSAudPositionPlayRec
//

/**
 *
 * RTestStepMVSAudPositionPlayRec
 *
 */
RTestStepMVSAudPositionPlayRec::RTestStepMVSAudPositionPlayRec(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudPositionPlayRec::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< OGG Vorbis Play, Pause and Change Position. Record. >>>"));
    iTestStepResult = EFail;
    
    // Set the AudioPlayAgent as active agent & begin the test
    iActiveAgent = ETestAudioPlay;
    TRAPD(err, FsmL(EMvsIdle));
    if(err == KErrNone)
        {
        CActiveScheduler::Start();
        }
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudPositionPlayRec::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TTimeIntervalMicroSeconds position;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioPlayAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioPlayAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioPlayAgent->OpenFileL(iFilename);
            break;
        case EAudPlayerOpenComplete:
            // Get Clip duration.
            iPlayDuration = iMVSAudioPlayAgent->Duration();
            // Set Position
            position = TTimeIntervalMicroSeconds(KTwoSeconds);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->SetPosition(2 Seconds)"));
            iMVSAudioPlayAgent->SetPosition(position);
            // Play iMVSAudioPlayAgent
            PrepareState(EAudioPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Play()"));
            iMVSAudioPlayAgent->Play();
            // Pause after 2 seconds
            User::After(KTwoSeconds);
            PrepareState(EAudioPaused, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Pause()"));
            iMVSAudioPlayAgent->Pause();
            // Get position
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->GetPosition()"));
            iMVSAudioPlayAgent->GetPosition(position);
            // Set position to some other position (before the current position)
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->SetPosition(1 Second)"));
            iMVSAudioPlayAgent->SetPosition(TTimeIntervalMicroSeconds(KOneSecond));
            // Play till end
            PrepareState(EAudioPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Play()"));
            iMVSAudioPlayAgent->Play();
            // Play till end
            PrepareState(EAudioOpened, KErrNone);
            break;
        case EAudPlayerPlayComplete:
            // Close iMVSAudioPlayAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Reset()"));
            iMVSAudioPlayAgent->Reset();
            iActiveAgent = ETestAudioRecord;
            // Open iMVSAudioRecordAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioRecordAgent->OpenFileL() : %S"), &iFilename);
            //iMVSAudioRecordAgent->OpenFileL(iFilename, KTestOggPlayControllerUid);
            iMVSAudioRecordAgent->OpenFileL(iFilename);
            break;
        case EAudRecorderOpenComplete:
            // Record iMVSAudioRecordAgent
            PrepareState(EAudioRecording, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->RecordL()"));
            iMVSAudioRecordAgent->RecordL();
            break;
        case EAudRecorderRecordProgress:
            // Stop after 2 seconds
            User::After(KTwoSeconds);
            PrepareState(EAudioStopped, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Stop()"));
            iMVSAudioRecordAgent->Stop();
            // Get & Verify the duration of the recorded clip
            iRecordDuration = iMVSAudioRecordAgent->Duration();
            INFO_PRINTF4(_L("New file duration = %d, Play Duration = %d, Record time = %d"), I64INT(iRecordDuration.Int64()), I64INT(iPlayDuration.Int64()), KTwoSeconds);
            // NB - At the moment append is not supported, recording will overwrite existing data in the file. So, we check for the new duration.
            if (TimeComparison(I64INT(iRecordDuration.Int64()), KTwoSeconds, KExpectedDeviation))
                {
                iTestStepResult = EPass;
                }
            else
                {
                iTestStepResult = EFail;
                }
            // Close iMVSAudioRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Reset()"));
            iMVSAudioRecordAgent->Reset();
            CActiveScheduler::Stop();
            break;
        }
    }

//
// RTestStepMVSAudRptPlayRec
//

/**
 *
 * RTestStepMVSAudRptPlayRec
 *
 */
RTestStepMVSAudRptPlayRec::RTestStepMVSAudRptPlayRec(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudRptPlayRec::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< WAV Repeat Play and Check Record Duration >>>"));
    iTestStepResult = EFail;
    
    // Set the AudioPlayAgent as active agent & begin the test
    iActiveAgent = ETestAudioPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudRptPlayRec::FsmL(TMvsTestAgentEvents aEventCode)
    {
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioPlayAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioPlayAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioPlayAgent->OpenFileL(iFilename);
            break;
        case EAudPlayerOpenComplete:
            // Get Clip duration.
            iPlayDuration = iMVSAudioPlayAgent->Duration();
            // Set Repeat Count to 3
            iMVSAudioPlayAgent->SetRepeats(2, TTimeIntervalMicroSeconds(KOneSecond));
            // Play iMVSAudioPlayAgent
            PrepareState(EAudioPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Play()"));
            iMVSAudioPlayAgent->Play();
            // Set Volume to Maximum.
            iMVSAudioPlayAgent->SetVolume(iMVSAudioPlayAgent->MaxVolume(), TTimeIntervalMicroSeconds(KOneSecond));
            // Set Balance.
            iMVSAudioPlayAgent->SetBalance(60);
            // Play till end
            PrepareState(EAudioOpened, KErrNone);
            break;
        case EAudPlayerPlayComplete:
            // Close iMVSAudioPlayAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Reset()"));
            iMVSAudioPlayAgent->Reset();
            iActiveAgent = ETestAudioRecord;
            // Open iMVSAudioRecordAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioRecordAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioRecordAgent->OpenFileL(iFilename);
            break;
        case EAudRecorderOpenComplete:
            // Record iMVSAudioRecordAgent
            PrepareState(EAudioRecording, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->RecordL()"));
            iMVSAudioRecordAgent->RecordL();
            break;
        case EAudRecorderRecordProgress:
            // Stop after 5 seconds
            User::After(KFiveSeconds);
            PrepareState(EAudioStopped, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Stop()"));
            iMVSAudioRecordAgent->Stop();
            // Get & Verify the duration of the recorded clip
            iRecordDuration = iMVSAudioRecordAgent->Duration();
            INFO_PRINTF4(_L("New file duration = %d, Play Duration = %d, Record time = %d"), I64INT(iRecordDuration.Int64()), I64INT(iPlayDuration.Int64()), KFiveSeconds);
            if (TimeComparison(I64INT(iRecordDuration.Int64()), I64INT(iPlayDuration.Int64()) + KFiveSeconds, KExpectedDeviation))
                {
                iTestStepResult = EPass;
                }
            else
                {
                iTestStepResult = EFail;
                }
            // Close iMVSAudioRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Reset()"));
            iMVSAudioRecordAgent->Reset();
            CActiveScheduler::Stop();
            break;
        }
    }

//
// RTestStepMVSAudOtherPlayRec
//
/**
 *
 * RTestStepMVSAudOtherPlayRec
 *
 */
RTestStepMVSAudOtherPlayRec::RTestStepMVSAudOtherPlayRec(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudOtherPlayRec::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< OGG play window & Other Play & Record settings. >>>"));
    iTestStepResult = EFail;
    
    // Set the AudioPlayAgent as active agent & begin the test
    iActiveAgent = ETestAudioPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudOtherPlayRec::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TInt threeFourthMaxVolume = 0;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioPlayAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioPlayAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioPlayAgent->OpenFileL(iFilename);
            break;
        case EAudPlayerOpenComplete:
            // Play iMVSAudioPlayAgent
            PrepareState(EAudioPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Play()"));
            iMVSAudioPlayAgent->Play();
            // Modify Volume to 75% of maximum
            threeFourthMaxVolume = iMVSAudioPlayAgent->MaxVolume() * 3 / 4;
            iMVSAudioPlayAgent->SetVolume(threeFourthMaxVolume, TTimeIntervalMicroSeconds(KOneSecond));
            // Set Balance.
            iMVSAudioPlayAgent->SetBalance(70);
            // Define a play window from 1 to 5 seconds
            iMVSAudioPlayAgent->SetPlayWindow(TTimeIntervalMicroSeconds(KOneSecond), TTimeIntervalMicroSeconds(KFiveSeconds));
            // Play till end
            PrepareState(EAudioOpened, KErrNone);
            break;
        case EAudPlayerPlayComplete:
            // Get the bit-rate
            iMVSAudioPlayAgent->GetBitRate(iPlayBitRate);
            // Close iMVSAudioRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Reset()"));
            iMVSAudioPlayAgent->Reset();
            // Set the AudioRecordAgent as active agent & begin the test    
            iActiveAgent = ETestAudioRecord;
            // Open iMVSAudioRecordAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioRecordAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioRecordAgent->OpenFileL(iFilename);
            break;
        case EAudRecorderOpenComplete:
            // Set Gain to 50%
            iMVSAudioRecordAgent->SetGain(iMVSAudioRecordAgent->MaxGain() * 1 / 2);
            // Record iMVSAudioRecordAgent
            PrepareState(EAudioRecording, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->RecordL()"));
            iMVSAudioRecordAgent->RecordL();
            break;
        case EAudRecorderRecordProgress:
            // Stop after 5 seconds
            User::After(KFiveSeconds);
            PrepareState(EAudioStopped, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Stop()"));
            iMVSAudioRecordAgent->Stop();
            iRecordBitRate = iMVSAudioRecordAgent->SourceBitRateL();
            INFO_PRINTF3(_L("Play Bit-rate = %d, Record Bit-rate= %d"), iPlayBitRate, iRecordBitRate);
            if (iRecordBitRate == iPlayBitRate)
                {
                iTestStepResult = EPass;
                }
            else
                {
                iTestStepResult = EFail;
                }
            // Close iMVSAudioRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Reset()"));
            iMVSAudioRecordAgent->Reset();
            CActiveScheduler::Stop();
            break;
        }
    }

//
// RTestStepMVSAudRecNew
//
/**
 *
 * RTestStepMVSAudRecNew
 *
 */
RTestStepMVSAudRecNew::RTestStepMVSAudRecNew(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid aRecordControllerUid, const TUid aPlayControllerUid)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    iRecordControllerUid = aRecordControllerUid;
    iPlayControllerUid = aPlayControllerUid;
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudRecNew::PerformTestL()
    {
    INFO_PRINTF2(_L("<<< Create a new %S file. Record with Max Gain and adjust volume during play. >>>"), &iSectName);
    iTestStepResult = EFail;
    
    // Set the AudioRecordAgent as active agent & begin the test    
    iActiveAgent = ETestAudioRecord;
    FsmL(EMvsIdle);    
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudRecNew::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TTimeIntervalMicroSeconds recDuration;
    switch (aEventCode)
        {
        case EMvsIdle:
            // Delete the file, and simply ignore the error, should be KErrAlreadyExists
            DeleteFileL(iFilename);
            // Open iMVSAudioRecordAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioRecordAgent->CreateNewFileL() : %S"), &iFilename);
            iMVSAudioRecordAgent->CreateNewFileL(iFilename, iRecordControllerUid, iPlayControllerUid);
            break;
        case EAudRecorderOpenComplete:
            // Set Gain to Maximum.
            iMVSAudioRecordAgent->SetGain(iMVSAudioRecordAgent->MaxGain());
            // Set Record balance.
            iMVSAudioRecordAgent->SetRecordBalance(50);
            // Record iMVSAudioRecordAgent
            PrepareState(EAudioRecording, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->RecordL()"));
            iMVSAudioRecordAgent->RecordL();
            break;
        case EAudRecorderRecordProgress:
            // Stop after 5 seconds
            User::After(KFiveSeconds);
            PrepareState(EAudioStopped, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Stop()"));
            iMVSAudioRecordAgent->Stop();
            // Get & Verify the duration of the recorded clip
            recDuration = iMVSAudioRecordAgent->Duration();
            INFO_PRINTF3(_L("Returned Duration = %d, Actual Record duration = %d"), I64INT(recDuration.Int64()), KFiveSeconds);
            if (!TimeComparison(I64INT(recDuration.Int64()), KFiveSeconds, KExpectedDeviation))
                {
                ERR_PRINTF1(_L("Record Duration does not match"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                }
            else
                {
                // Play iMVSAudioRecordAgent
                PrepareState(EAudioPlaying, KErrNone);
                INFO_PRINTF1(_L("CMVSAudioRecordAgent->PlayL()"));
                TRAPD(err, iMVSAudioRecordAgent->PlayL());
                if(err != KErrNone)
                    {
                    ERR_PRINTF2(_L("iMVSAudioRecordAgent->PlayL() left with Error : %d"), err);
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    }
                }
            break;
        case EAudRecorderPlayProgress:
            PrepareState(EAudioOpened, KErrNone);
            break;
        case EAudRecorderPlayComplete:
            // Close iMVSAudioRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Reset()"));
            iMVSAudioRecordAgent->Reset();
            iTestStepResult = EPass;
            CActiveScheduler::Stop();
            break;
        }
    }

//
// RTestStepMVSAudPlaySettings
//
/**
 *
 * RTestStepMVSAudPlaySettings
 *
 */
RTestStepMVSAudPlaySettings::RTestStepMVSAudPlaySettings(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudPlaySettings::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Audio Play agent settings verification test. >>>"));
    iTestStepResult = EFail;
    
    // Set the AudioPlayAgent as active agent & begin the test
    iActiveAgent = ETestAudioPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudPlaySettings::FsmL(TMvsTestAgentEvents aEventCode)
    {
    RPointerArray<CMMFMetaDataEntry> metaArray;
    TInt err = KErrNone;
    TUint playBitRate = 0;
    const CMMFControllerImplementationInformation* theControllerImplInfo = NULL;
    TUid theControllerImplUid = KNullUid;
    TBuf<80> theControllerImplName;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioPlayAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioPlayAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioPlayAgent->OpenFileL(iFilename);
            break;
        case EAudPlayerOpenComplete:
            // Get the meta data entries and verify (with known value)
            CleanupClosePushL(metaArray);
            TRAP(err, iMVSAudioPlayAgent->GetMetaArrayL(metaArray));
            INFO_PRINTF2(_L("CMVSAudioPlayAgent->GetMetaArrayL() returned : %d"), err);
            if(err != KErrNone && err != KErrNotSupported)
                {
                ERR_PRINTF2(_L("CMVSAudioPlayAgent->GetMetaArrayL() left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                CleanupStack::PopAndDestroy(); // metaArray
                break;
                }
            CleanupStack::PopAndDestroy(); // metaArray
            // Get the bit-rate of the Audio file and verify
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->GetBitRate()"));
            err = iMVSAudioPlayAgent->GetBitRate(playBitRate);
            if (playBitRate != 128)
                {
                if(err == KErrNone)
                    {
                    INFO_PRINTF2(_L("Retrieved bit rate: %d"), playBitRate);
                    if(static_cast<TInt>(playBitRate) != 96000)
                        {
                        ERR_PRINTF1(_L("The Expected bit rate and retrieved bit rate does not match"));
                        CActiveScheduler::Stop();
                        iTestStepResult = EFail;
                        break;    
                        }
                    }
                else
                    {
                    ERR_PRINTF2(_L("iMVSAudioPlayAgent->GetBitRate() returned error : %d"), err);
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;
                    }
                }
            // Get the name of the currently active controller plug-in.
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->GetControllerInfoL()"));
            theControllerImplInfo = &iMVSAudioPlayAgent->GetControllerInfoL();
            theControllerImplUid = theControllerImplInfo->Uid();
            INFO_PRINTF2(_L("Active Controller Plugin -> UID is 0x%8x"), theControllerImplUid);
            theControllerImplName = theControllerImplInfo->DisplayName();
            INFO_PRINTF2(_L("Active Controller Plugin -> DisplayName is %S"), &theControllerImplName);
            if (theControllerImplUid != KTestOggPlayControllerUid)    // check it's the right one
                {
                ERR_PRINTF1(_L("Error: UID Does not match with OggPlayController"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            if (theControllerImplName != _L("OggPlayController"))    // check it's the right one
                {
                ERR_PRINTF1(_L("Error: DisplayName Does not match with OggPlayController"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Play iMVSAudioPlayAgent
            PrepareState(EAudioPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Play()"));
            iMVSAudioPlayAgent->Play();
            // Play till end
            PrepareState(EAudioOpened, KErrNone);
            break;
        case EAudPlayerPlayComplete:
            // Close iMVSAudioRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Reset()"));
            iMVSAudioPlayAgent->Reset();
            iTestStepResult = EPass;
            CActiveScheduler::Stop();
            break;
        }
    }

//
// RTestStepMVSAudRecConfig
//
/**
 *
 * RTestStepMVSAudRecConfig
 *
 */
RTestStepMVSAudRecConfig::RTestStepMVSAudRecConfig(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudRecConfig::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Audio Record agent format configuration test. >>>"));
    iTestStepResult = EFail;
    
    // Set the AudioRecordAgent as active agent & begin the test    
    iActiveAgent = ETestAudioRecord;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudRecConfig::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TInt numBitRates = 0;
    TInt numSampRates = 0;
    TInt numCodecs = 0;
    TInt numChannels = 0;
    TUint8 theBuffer8[10];
    TBuf<10> bufCodec;
    RArray<TUint> bitRates;
    RArray<TUint> sampRates;
    RArray<TUint> channels;
    RArray<TFourCC> codecs;
    TInt err = KErrNone;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Delete the file, and simply ignore the error, should be KErrAlreadyExists
            DeleteFileL(iFilename);
            // Open iMVSAudioRecordAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioRecordAgent->CreateNewFileL() : %S"), &iFilename);
            iMVSAudioRecordAgent->CreateNewFileL(iFilename, KTestOggRecordControllerUid, KTestOggPlayControllerUid);
            break;
        case EAudRecorderOpenComplete:
            // Get Supported Destination Bit Rates
            TRAP(err, iMVSAudioRecordAgent->GetSupportedBitRatesArrayL(bitRates));
            if(err != KErrNone)
                {
                if (err == KErrNotSupported)
                    {
                    INFO_PRINTF1(_L("CMVSAudioRecordAgent->GetSupportedBitRatesArrayL() returned KErrNotSupported"));
                    }
                else
                    {
                    ERR_PRINTF2(_L("CMVSAudioRecordAgent->GetSupportedBitRatesArrayL() left with error : %d"), err);
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;
                    }
                }
            // Set Destination Bit Rate
            numBitRates = bitRates.Count();
            INFO_PRINTF2(_L("--CMVSAudioRecordAgent->GetSupportedBitRatesArrayL() returned %d values"), numBitRates);
            for(TInt i=0;i<numBitRates;i++)
                {
                INFO_PRINTF2(_L("CMVSAudioRecordAgent->SetDestinationBitRateL(%d)"), bitRates[i]);
                TRAP(err, iMVSAudioRecordAgent->SetDestinationBitRateL(bitRates[i]));
                
                if(err != KErrNone)
                    {
                    ERR_PRINTF2(_L("CMVSAudioRecordAgent->SetDestinationBitRateL() left with error : %d"), err);
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;
                    }
                }
            // Get Supported Destination Sample Rates
            TRAP(err, iMVSAudioRecordAgent->GetSupportedSampleRatesArrayL(sampRates));
            if(err != KErrNone)
                {
                ERR_PRINTF2(_L("CMVSAudioRecordAgent->GetSupportedSampleRatesArrayL() left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Set Destination Sample Rate
            numSampRates = sampRates.Count();
            INFO_PRINTF2(_L("--CMVSAudioRecordAgent->GetSupportedSampleRatesArrayL() returned %d values"), numSampRates);
            for(TInt i=0;i<numSampRates;i++)
                {
                INFO_PRINTF2(_L("CMVSAudioRecordAgent->SetDestinationSampleRateL(%d)"),sampRates[i]);
                TRAP(err, iMVSAudioRecordAgent->SetDestinationSampleRateL(sampRates[i]));
                if(err != KErrNone)
                    {
                    ERR_PRINTF2(_L("CMVSAudioRecordAgent->SetDestinationSampleRateL() left with error : %d"),err);
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;
                    }
                }
            // Get Supported Destination Number Of Channels
            TRAP(err, iMVSAudioRecordAgent->GetSupportedNoChannelsArrayL(channels));
            if(err != KErrNone)
                {
                ERR_PRINTF2(_L("CMVSAudioRecordAgent->GetSupportedNoChannelsArrayL() left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Set Destination Number Of Channels
            numChannels = channels.Count();
            INFO_PRINTF2(_L("--CMVSAudioRecordAgent->GetSupportedNoChannelsArrayL() returned %d channels"), numChannels);
            INFO_PRINTF2(_L("CMVSAudioRecordAgent->SetDestinationNumberOfChannelsL(%d)"),2);
            TRAP(err, iMVSAudioRecordAgent->SetDestinationNumberOfChannelsL(2));
            if(err != KErrNone)
                {
                ERR_PRINTF2(_L("CMVSAudioRecordAgent->SetDestinationNumberOfChannelsL(2) left with error : %d"),err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Get Supported Destination Data Types
            TRAP(err, iMVSAudioRecordAgent->GetSupportedCodecsArrayL(codecs));
            if(err != KErrNone)
                {
                ERR_PRINTF2(_L("CMVSAudioRecordAgent->GetSupportedCodecsArrayL() left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Set Destination Data Type
            numCodecs = codecs.Count();
            INFO_PRINTF2(_L("--CMVSAudioRecordAgent->GetSupportedCodecsArrayL() returned %d codecs"), numCodecs);
            for(TInt i=0;i<numCodecs;i++)
                {
                TPtr8 ptrFour(theBuffer8, 10);
                 codecs[i].FourCC(&ptrFour);
                 bufCodec.Copy(ptrFour);
                INFO_PRINTF2(_L("CMVSAudioRecordAgent->SetDestinationDataTypeL(%S)"),&bufCodec);
                TRAP(err, iMVSAudioRecordAgent->SetDestinationDataTypeL(codecs[i]));
                if(err!=KErrNone)
                    {
                    ERR_PRINTF2(_L("CMVSAudioRecordAgent->SetDestinationNumberOfChannelsL(1) left with error : %d"),err);
                    CActiveScheduler::Stop();
                    iTestStepResult = EFail;
                    break;
                    }
                }
            // Record iMVSAudioRecordAgent
            PrepareState(EAudioRecording, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->RecordL()"));
            TRAP(err, iMVSAudioRecordAgent->RecordL());
            if(err != KErrNone)
                {
                ERR_PRINTF2(_L("CMVSAudioRecordAgent->RecordL() left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                }
            break;
        case EAudRecorderRecordProgress:
            // Stop after 5 seconds
            User::After(KFiveSeconds);
            PrepareState(EAudioStopped, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Stop()"));
            iMVSAudioRecordAgent->Stop();
            // Close iMVSAudioRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Reset()"));
            iMVSAudioRecordAgent->Reset();
            iTestStepResult = EPass;
            CActiveScheduler::Stop();
            bitRates.Close();
            sampRates.Close();
            channels.Close();
            codecs.Close();
            break;
        }
    }


//
// RTestStepMVSAudRecCrop
//
/**
 *
 * RTestStepMVSAudRecCrop
 *
 */
RTestStepMVSAudRecCrop::RTestStepMVSAudRecCrop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudRecCrop::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Audio cropping (before & after position). >>>"));
    iTestStepResult = EFail;
    
    // Set the AudioRecordAgent as active agent & begin the test    
    iActiveAgent = ETestAudioRecord;
    FsmL(EMvsIdle);    
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudRecCrop::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TTimeIntervalMicroSeconds fullDuration;
    TTimeIntervalMicroSeconds cropDuration;
    TInt err = KErrNone;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioRecordAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioRecordAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioRecordAgent->OpenFileL(iFilename);
            break;
        case EAudRecorderOpenComplete:
            // Get the file duration.
            fullDuration = iMVSAudioRecordAgent->Duration();
            // Crop the clip from a specific start position till end of file.
            INFO_PRINTF1(_L("iMVSAudioRecordAgent->CropClipL(ONE Second) -- preceding part is cut-off"));
            TTimeIntervalMicroSeconds cropStart = TTimeIntervalMicroSeconds(KOneSecond);;
            TRAP(err, iMVSAudioRecordAgent->CropClipL(cropStart, ETrue));
            WARN_PRINTF2(_L("CMVSAudioRecordAgent->CropClipL() left with error : %d"), err);

			// DEF127335
		   	// OggRecordController does not support recording into an already existing file.
		   	// This is due to unavailability of APIs at the level of Ogg Vorbis C libraries.
            if (err == KErrNotSupported && iTestStepName == _L("MM-MVS-AUDIOAGENT-I-0008-HP"))
	            {
	            INFO_PRINTF1(_L("Expected Result: OggRecordController returned KErrNotSupported(-5)"));
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
	            }
            else if ((err != KErrNone) && (err != KErrArgument))
                {
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            else if(err == KErrArgument)
                {
                WARN_PRINTF2(_L("Since CMVSAudioRecordAgent->CropClipL() left with error KErrArgument, the test can't check the crop duration..."), err);
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
                }
            // Verify the length of the cropped clip.
            cropDuration = iMVSAudioRecordAgent->Duration();
            INFO_PRINTF4(_L("Original File Duration = %d, Actual Crop duration = %d, Expected Crop Duration = %d"), I64INT(fullDuration.Int64()), I64INT(cropDuration.Int64()), I64INT(fullDuration.Int64()) - KOneSecond);
            if (!TimeComparison(I64INT(cropDuration.Int64()), I64INT(fullDuration.Int64()) - KOneSecond, KExpectedDeviation))
                {
                ERR_PRINTF1(_L("Crop Duration is not as expected."));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Get the file duration.
            fullDuration = iMVSAudioRecordAgent->Duration();
            // Crop the clip from the start of file to a specific end position.
            INFO_PRINTF1(_L("iMVSAudioRecordAgent->CropClipL(TWO Seconds) -- trailing part is cut-off"));
            TTimeIntervalMicroSeconds cropEnd = TTimeIntervalMicroSeconds(KTwoSeconds);
            TRAP(err, iMVSAudioRecordAgent->CropClipL(cropEnd, EFalse));
            if(err != KErrNone)
                {
                ERR_PRINTF2(_L("CMVSAudioRecordAgent->CropClipL() left with error : %d"), err);
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Verify the length of the cropped clip.
            cropDuration = iMVSAudioRecordAgent->Duration();
            INFO_PRINTF4(_L("Original File Duration = %d, Actual Crop duration = %d, Expected Crop Duration = %d"), I64INT(fullDuration.Int64()), I64INT(cropDuration.Int64()), KTwoSeconds);
            if (!TimeComparison(I64INT(cropDuration.Int64()), KTwoSeconds, KExpectedDeviation))
                {
                ERR_PRINTF1(_L("Crop Duration is not as expected."));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Close iMVSAudioRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Reset()"));
            iMVSAudioRecordAgent->Reset();
            iTestStepResult = EPass;
            CActiveScheduler::Stop();
            break;
        }
    }

//
// RTestStepMVSAudRecMeta
//
/**
 *
 * RTestStepMVSAudRecMeta
 *
 */
RTestStepMVSAudRecMeta::RTestStepMVSAudRecMeta(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudRecMeta::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Audio Record Agent : Meta-information test. >>>"));
    iTestStepResult = EFail;
    
    // Set the AudioRecordAgent as active agent & begin the test    
    iActiveAgent = ETestAudioRecord;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudRecMeta::FsmL(TMvsTestAgentEvents aEventCode)
    {
    _LIT(name1,"<Copyright>");
    _LIT(data1,"<Symbian (c) 2005>");
    _LIT(name2,"Artist");
    _LIT(data2,"Tester");
    TInt err = KErrNone;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioRecordAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioRecordAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioRecordAgent->OpenFileL(iFilename, KTestOggRecordControllerUid, KTestOggPlayControllerUid);
            break;
        case EAudRecorderOpenComplete:
            // Stop the scheduler as no more Async request follow.
            CActiveScheduler::Stop();
            // build a meta data array object.
            RPointerArray<CMMFMetaDataEntry> metaArray;
            CleanupClosePushL(metaArray);
            // Populate the meta array object to set
            CMMFMetaDataEntry* metaData[2];
            metaData[0] = CMMFMetaDataEntry::NewL(name1,data1);
            CleanupStack::PushL(metaData[0]);
            metaArray.Append(metaData[0]);
            metaData[1] = CMMFMetaDataEntry::NewL(name2,data2);
            CleanupStack::PushL(metaData[1]);
            metaArray.Append(metaData[1]);
            // Add two meta-Information to be written to the clip header.
            INFO_PRINTF1(_L("iMVSAudioRecordAgent->SetMetaDataL()"));
            TRAP(err, iMVSAudioRecordAgent->SetMetaDataL(metaArray));
            if(err != KErrNotSupported)
                {
                CleanupStack::PopAndDestroy(3);
                ERR_PRINTF2(_L("iMVSAudioRecordAgent->SetMetaDataL() left with error : %d"), err);
                iTestStepResult = EFail;
                break;
                }
            else
                {
                INFO_PRINTF1(_L("iMVSAudioRecordAgent->SetMetaDataL() left with error : KErrNotSupported"));
                }
            CleanupStack::PopAndDestroy(3);
            // build a meta data array object to get
            RPointerArray<CMMFMetaDataEntry> metaArrayGet;
            CleanupClosePushL(metaArrayGet);
            // Get the the meta entries and check the count.
            INFO_PRINTF1(_L("iMVSAudioRecordAgent->GetMetaDataArrayL()"));
            //Not working at the moment........
            iMVSAudioRecordAgent->GetMetaDataArrayL(metaArrayGet);
            if (metaArrayGet.Count() != 0)
                {
                CleanupStack::PopAndDestroy(1);
                ERR_PRINTF2(_L("Meta entries count expected = ZERO, actual = %d"), metaArrayGet.Count());
                iTestStepResult = EFail;
                break;
                }
            else
                {
                INFO_PRINTF1(_L("Meta entries count is ZERO"));
                }
            CleanupStack::PopAndDestroy(1);
            // Close iMVSAudioRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Close()"));
            iMVSAudioRecordAgent->Reset();
            iTestStepResult = EPass;
            break;
        }
    }

//
// RTestStepMVSAudioAgentBasic
//
/**
 *
 * RTestStepMVSAudioAgentBasic
 *
 */
RTestStepMVSAudioAgentBasic::RTestStepMVSAudioAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName), iTestType(aTestType)
    {
    }

/**
 *
 * DoTestStepPreamble
 *
 */
TVerdict RTestStepMVSAudioAgentBasic::DoTestStepPreambleL()
    {
    // Call base DoTestStepPreambleL()
    return RTestStepMVSAudioAgent::DoTestStepPreambleL();
    }    


/**
 *
 * DoTestStepPostambleL
 *
 */
TVerdict RTestStepMVSAudioAgentBasic::DoTestStepPostambleL()
    {
    // Base Postamble.
    return RTestStepMVSAudioAgent::DoTestStepPostambleL();
    }

/**
 * DoTestStepL
 */
TVerdict RTestStepMVSAudioAgentBasic::DoTestStepL()
    {
    TPtrC16 ptr(KTestMvsAgents[iAgentUnderTest]);
    INFO_PRINTF3(_L("<<< %S test for %S.>>>"), &iTestType, &ptr);
    TVerdict testResult = EPass;
    TInt err = KErrNone;
    
    if (iTestType.Compare(_L("ALLOC")) == 0)
        {
        //Check for Alloc Test Support
        __UHEAP_SETFAIL(RHeap::EFailNext, 1);
        TAny *testAllocSupport = User::Alloc(1);
        if (testAllocSupport!=NULL)
            {
            delete testAllocSupport;
            testAllocSupport=NULL;
            INFO_PRINTF1(_L("This build does not support ALLOC testing"));
            return EInconclusive;
            }
        __UHEAP_SETFAIL(RHeap::ENone, 0);
        
        //Initialise Variables
        TVerdict allocTestStepResult=EPass;
        TBool badResult = EFalse;
        
        //>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
        INFO_PRINTF1(_L("Executing the Test Method call Outside the FOR LOOP."));
        TRAP(err, allocTestStepResult = PerformTestL());
        if (err != KErrNone)
            {
            INFO_PRINTF2(_L("Decoding error, returned error code =  %d"), err);
            return EFail;
            }
        else
            {
            //Check the allocTestStepResult
            if (allocTestStepResult != EPass)
                {
                badResult = ETrue;
                }
            }
        
        TInt failCount = 1;
        TBool completed = EFalse;
        allocTestStepResult = EPass; // reinitialise
        TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
        for(;;)    
            {
            __UHEAP_SETFAIL(RHeap::EFailNext, failCount);
            //__MM_HEAP_MARK;

            //>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
            INFO_PRINTF2(_L("Executing the Test Method call #%d."), failCount);
            TRAP(err, allocTestStepResult = PerformTestL());
            
            completed = EFalse;
            if (err == KErrNone)
                {
                TAny *testAlloc = NULL;
                testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
                if (testAlloc == NULL)
                    {
                    reachedEnd = ETrue;
                    failCount -= 1;
                    }
                else
                    {
                    User::Free(testAlloc);
                    testAlloc=NULL;
                    }            
                
                //Check the allocTestStepResult
                if (allocTestStepResult != EPass)
                    {
                    badResult = ETrue;
                    }
                
                completed = reachedEnd || badResult;
                }
            else if (err != KErrNoMemory) // bad error code
                {
                completed = ETrue;
                badResult = EFail;
                }            

            //__MM_HEAP_MARKEND;
            __UHEAP_SETFAIL(RHeap::ENone, 0);

            if (completed)
                {
                break; // exit loop
                }

            failCount++;
            }

        //failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc

        if (err != KErrNone || badResult)
            {
            if (badResult)
                {
                INFO_PRINTF3(_L("  Bad result with %d memory allocations tested. The Error returned is %d"), failCount, err);
                }
            else
                {
                INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested\n"), err, failCount);
                }
            allocTestStepResult = EFail;
            }
        else 
            {
            INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested\n"), failCount);
            allocTestStepResult = EPass;
            }
            testResult = allocTestStepResult;
        }
    else if (iTestType.Compare(_L("LOG")) == 0)
        {
        // Run the test by calling PerformTestL()
        TRAP(err, testResult = PerformTestL());
        if (err != KErrNone)
            {
            INFO_PRINTF2(_L("Test FAILED with error %d..."), err);
            testResult = EFail;
            }
        
        // Code to Check the existance of new log file here xxxx
        _LIT(KFileLogTestString, "Audio Opened");
        _LIT8(KFileLogTestString8, "Audio Opened");
        if(CheckLogFileL(KMVSAgentsLogPath, KFileLogTestString8, KFileLogTestString))
            {
            testResult = EPass;
            }
        else
            {
            testResult = EFail;
            }
        }
    return testResult;
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudioAgentBasic::PerformTestL()
    {
    iTestStepResult = EFail;
    
    // Set the active agent based on iAgentUnderTest & begin the test.
    iActiveAgent = iAgentUnderTest;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    User::LeaveIfError(iError);
    
    return iTestStepResult;
    }


//
// RTestStepMVSAudioRecordAgentBasic
//
/**
 *
 * RTestStepMVSAudioRecordAgentBasic
 *
 */
RTestStepMVSAudioRecordAgentBasic::RTestStepMVSAudioRecordAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType, const TUid aRecordControllerUid, const TUid aPlayControllerUid)
    : RTestStepMVSAudioAgentBasic(aTestName, aSectName, aKeyName, aTestType)
    {
    iAgentUnderTest = ETestAudioRecord;
    iRecordControllerUid = aRecordControllerUid;
    iPlayControllerUid = aPlayControllerUid;
    iHeapSize = 500000;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudioRecordAgentBasic::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TTimeIntervalMicroSeconds recDuration;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Delete the file, and simply ignore the error, should be KErrAlreadyExists
            DeleteFileL(iFilename);
            // Open iMVSAudioRecordAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioRecordAgent->CreateNewFileL() : %S"), &iFilename);
            iMVSAudioRecordAgent->CreateNewFileL(iFilename, iRecordControllerUid, iPlayControllerUid);
            break;
        case EAudRecorderOpenComplete:
            // Record iMVSAudioRecordAgent
            PrepareState(EAudioRecording, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->RecordL()"));
            iMVSAudioRecordAgent->RecordL();
            break;
        case EAudRecorderRecordProgress:
            // Stop after 5 seconds (reduced to 2 as Alloc tests take a long time)
            User::After(KFiveSeconds);
            PrepareState(EAudioStopped, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Stop()"));
            iMVSAudioRecordAgent->Stop();
            // Get & Verify the duration of the recorded clip
            recDuration = iMVSAudioRecordAgent->Duration();
            INFO_PRINTF3(_L("Returned Duration = %d, Actual Record duration = %d"), I64INT(recDuration.Int64()), KFiveSeconds);
            if (!TimeComparison(I64INT(recDuration.Int64()), KFiveSeconds, KExpectedDeviation))
                {
                ERR_PRINTF1(_L("Record Duration does not match"));
                CActiveScheduler::Stop();
                iTestStepResult = EFail;
                break;
                }
            // Close iMVSAudioRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Reset()"));
            iMVSAudioRecordAgent->Reset();
            iTestStepResult = EPass;
            CActiveScheduler::Stop();
            break;
        }
    }
    
//
// RTestStepMVSAudioPlayAgentBasic
//
/**
 *
 * RTestStepMVSAudioPlayAgentBasic
 *
 */
RTestStepMVSAudioPlayAgentBasic::RTestStepMVSAudioPlayAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType)
    : RTestStepMVSAudioAgentBasic(aTestName, aSectName, aKeyName, aTestType)
    {
    iAgentUnderTest = ETestAudioPlay;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudioPlayAgentBasic::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TTimeIntervalMicroSeconds recDuration;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioPlayAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioPlayAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioPlayAgent->OpenFileL(iFilename);
            PrepareState(EAudioOpened, KErrNone);
            break;
        case EAudPlayerOpenComplete:
            // Play iMVSAudioPlayAgent
            PrepareState(EAudioPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Play()"));
            iMVSAudioPlayAgent->Play();
            PrepareState(EAudioOpened, KErrNone);
            break;
        case EAudPlayerPlayComplete:
            // Close iMVSAudioPlayAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Reset()"));
            iMVSAudioPlayAgent->Reset();
            iTestStepResult = EPass;
            CActiveScheduler::Stop();
            break;
        }
    }

//
// RTestStepMVSAudOpenWhenOpen
//
/**
 *
 * RTestStepMVSAudOpenWhenOpen
 *
 */
RTestStepMVSAudOpenWhenOpen::RTestStepMVSAudOpenWhenOpen(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aKeyName2)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName), iKeyName2(aKeyName2)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudOpenWhenOpen::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Try to open another audio file when one file is already opened. >>>"));
    iTestStepResult = EFail;
    
    // Get the 2nd filename
    TPtrC tmpFilename;
    if(!GetStringFromConfig(iSectName, iKeyName2, tmpFilename))
        {
        ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
        return EInconclusive;
        }
    GetDriveName(iFilename2);
    iFilename2.Append(tmpFilename);
    
    // Set the AudioPlayAgent as active agent & begin the test
    iActiveAgent = ETestAudioPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudOpenWhenOpen::FsmL(TMvsTestAgentEvents aEventCode)
    {
    // Local variables
    TInt openCount = 0;
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioPlayAgent
            PrepareState(EAudioOpening, KErrNone);
            openCount++;
            INFO_PRINTF2(_L("CMVSAudioPlayAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioPlayAgent->OpenFileL(iFilename);
            break;
        case EAudPlayerOpenComplete:
            if (openCount == 1)
                {
                // Open iMVSAudioPlayAgent
                PrepareState(EAudioOpening, KErrNone);
                openCount++;
                INFO_PRINTF2(_L("CMVSAudioPlayAgent->OpenFileL() : %S"), &iFilename2);
                iMVSAudioPlayAgent->OpenFileL(iFilename2);
                PrepareState(EAudioOpened, KErrInUse);
                }
            else
                {
                // Close iMVSAudioPlayAgent
                PrepareState(ENotReady, KErrNone);
                INFO_PRINTF1(_L("CMVSAudioPlayAgent->Reset()"));
                iMVSAudioPlayAgent->Reset();
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                }
            break;
        }
    }

//
// RTestStepMVSAudOpenUnsupportedFile
//
/**
 *
 * RTestStepMVSAudOpenUnsupportedFile
 *
 */
RTestStepMVSAudOpenUnsupportedFile::RTestStepMVSAudOpenUnsupportedFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudOpenUnsupportedFile::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Open Unsupported file (text file) and try to play. >>>"));
    iTestStepResult = EFail;
    
    // Set the AudioPlayAgent as active agent & begin the test
    iActiveAgent = ETestAudioPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudOpenUnsupportedFile::FsmL(TMvsTestAgentEvents aEventCode)
    {
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioPlayAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioPlayAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioPlayAgent->OpenFileL(iFilename);
            PrepareState(ENotReady, KErrNotSupported);
            break;
        case EAudPlayerOpenComplete:
            // Close iMVSAudioPlayAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Reset()"));
            iMVSAudioPlayAgent->Reset();
            CActiveScheduler::Stop();
            iTestStepResult = EPass;
            break;
        }
    }


//
// RTestStepMVSAudOpenWhenPlaying
//
/**
 *
 * RTestStepMVSAudOpenWhenPlaying
 *
 */
RTestStepMVSAudOpenWhenPlaying::RTestStepMVSAudOpenWhenPlaying(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudOpenWhenPlaying::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< Open the same file using record agent while audio playe agent is playing. >>>"));
    iTestStepResult = EFail;
    
    // Set the AudioPlayAgent as active agent & begin the test
    iActiveAgent = ETestAudioPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudOpenWhenPlaying::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TTimeIntervalMicroSeconds position(KTwoSeconds);
    
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioPlayAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioPlayAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioPlayAgent->OpenFileL(iFilename);
            break;
        case EAudPlayerOpenComplete:
            // Set Position
            iMVSAudioPlayAgent->SetPosition(position);
            // Play iMVSAudioPlayAgent
            PrepareState(EAudioPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Play()"));
            iMVSAudioPlayAgent->Play();
            iActiveAgent = ETestAudioRecord;
            // Open iMVSAudioRecordAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioRecordAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioRecordAgent->OpenFileL(iFilename, KTestOggPlayControllerUid, KTestOggPlayControllerUid);
            break;
        case EAudRecorderOpenComplete:
            // Close iMVSAudioPlayAgent
            iActiveAgent = ETestAudioPlay;
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Reset()"));
            iMVSAudioPlayAgent->Reset();
            // Close iMVSAudioRecordAgent
            iActiveAgent = ETestAudioRecord;
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Reset()"));
            iMVSAudioRecordAgent->Reset();
            iTestStepResult = EPass;
            CActiveScheduler::Stop();
            break;
        }
    }


//
// RTestStepMVSAudRecUnsupportedFile
//
/**
 *
 * RTestStepMVSAudRecUnsupportedFile
 *
 */
RTestStepMVSAudRecUnsupportedFile::RTestStepMVSAudRecUnsupportedFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudRecUnsupportedFile::PerformTestL()
    {
    INFO_PRINTF2(_L("<<< Record audio to unsupported (%S) file. >>>"), &iKeyName);
    iTestStepResult = EFail;
    
    // Set the AudioRecordAgent as active agent & begin the test    
    iActiveAgent = ETestAudioRecord;
    FsmL(EMvsIdle);    
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudRecUnsupportedFile::FsmL(TMvsTestAgentEvents aEventCode)
    {
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioRecordAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioRecordAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioRecordAgent->OpenFileL(iFilename);
            PrepareState(ENotReady, KErrNotSupported);
            break;
        case EAudRecorderOpenComplete:
            // Close iMVSAudioRecordAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioRecordAgent->Reset()"));
            iMVSAudioRecordAgent->Reset();
            iTestStepResult = EPass;
            CActiveScheduler::Stop();
            break;
        }
    }

//
// RTestStepMVSAudPlayPausePlay
// Test added as part of CR1566 (TruePause)
// Test should work in both A3F and non-A3F even when the DevSound implementation doesn't support true pause
//

/**
 *
 * RTestStepMVSAudPlayPausePlay
 *
 */
RTestStepMVSAudPlayPausePlay::RTestStepMVSAudPlayPausePlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepMVSAudioAgent(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSAudPlayPausePlay::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< WAV Play-Pause-Resume >>>"));
    iTestStepResult = EFail;
    
    // Set the AudioPlayAgent as active agent & begin the test
    iActiveAgent = ETestAudioPlay;
    FsmL(EMvsIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }

/**
 *
 * FsmL
 *
 */
void RTestStepMVSAudPlayPausePlay::FsmL(TMvsTestAgentEvents aEventCode)
    {
    TTimeIntervalMicroSeconds position;
    TTimeIntervalMicroSeconds pauseposition;
    TTimeIntervalMicroSeconds resumeposition;
    TTime starttime;
    TTime endtime;
    TTimeIntervalMicroSeconds totaltime;
    switch (aEventCode)
        {
        case EMvsIdle:
            // Open iMVSAudioPlayAgent
            PrepareState(EAudioOpening, KErrNone);
            INFO_PRINTF2(_L("CMVSAudioPlayAgent->OpenFileL() : %S"), &iFilename);
            iMVSAudioPlayAgent->OpenFileL(iFilename);
            break;
        case EAudPlayerOpenComplete:
            // Get Clip duration.
            iPlayDuration = iMVSAudioPlayAgent->Duration();
            // Play iMVSAudioPlayAgent
            PrepareState(EAudioPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Play()"));
            iMVSAudioPlayAgent->Play();
            starttime.HomeTime(); //Get start time
            // Pause after 2 seconds
            User::After(KTwoSeconds);
            PrepareState(EAudioPaused, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Pause()"));
            iMVSAudioPlayAgent->Pause();
            // Get position
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->GetPosition()"));
            iMVSAudioPlayAgent->GetPosition(position);
            User::After(KTwoSeconds); //Wait two seconds in Pause
            //Verify position was maintained
            INFO_PRINTF1(_L("Verify position was maintained while in Pause"));
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->GetPosition()"));
            iMVSAudioPlayAgent->GetPosition(pauseposition);
            
            if(position == pauseposition)
            	{
            	INFO_PRINTF2(_L("Position was maintained during Pause as %d"),I64LOW(position.Int64()));
            	iTestStepResult = EPass;
            	}
            else
            	{
            	ERR_PRINTF3(_L("Position was not maintained during Pause. Expected %d, Retrieved %d"), I64LOW(position.Int64()), I64LOW(pauseposition.Int64()));
            	iTestStepResult = EFail;
            	}
            // Play till end
            PrepareState(EAudioPlaying, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Play()"));
            iMVSAudioPlayAgent->Play();
            iMVSAudioPlayAgent->GetPosition(resumeposition);
            INFO_PRINTF1(_L("Verify resume position"));
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->GetPosition()"));
            if(Abs(resumeposition.Int64() - pauseposition.Int64()) <= KOneSecond/2)
            	{
            	INFO_PRINTF2(_L("Playback resumed from expected position %Ld"), resumeposition.Int64());
            	iTestStepResult = EPass;
            	}
            else
            	{
				ERR_PRINTF3(_L("Playback did not resume from expected position. Expected %Ld, Retrieved %Ld"), pauseposition.Int64(), resumeposition.Int64());
            	iTestStepResult = EFail;
            	}
            // Play till end
            PrepareState(EAudioOpened, KErrNone);
            break;
        case EAudPlayerPlayComplete:
            // Close iMVSAudioPlayAgent
            PrepareState(ENotReady, KErrNone);
            INFO_PRINTF1(_L("CMVSAudioPlayAgent->Reset()"));
            iMVSAudioPlayAgent->Reset();
            CActiveScheduler::Stop();
            break;
        }
    }
