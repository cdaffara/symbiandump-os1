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
// Implementation of test step classes for DevSound - Record.
// 
//

#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>
#include <mmf/server/devsoundstandardcustominterfaces.h>
#include "tsi_mmf_devsound_record.h"

//
// Positive Tests //
//

//
// RTestStepDevSoundRecordBase
//

/**
 *
 * RTestStepDevSoundRecordBase - Test step constructor
 *
 */
RTestStepDevSoundRecordBase::RTestStepDevSoundRecordBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType)
    {
    // Store the name of this test case
    iTestStepName = aTestName;

    // Section name for retrieving filename
    iSectName = aSectName;

    // Key name for retrieving filename
    iKeyName = aKeyName;
    
    // we are allocating buffers in these tests so we must grow the heap
    iHeapSize = 0xa00000;     // 10MB
    
    // Datatype of the Encoder
    iOutputDataType = aOutputDataType;
    
    iBufferCount = 0;       // Number of buffers recorded
    iTestType = ENormalTest;
    
    iMMFDevSound = NULL;
    }

/**
 *
 * Fsm()
 * Executes events of DevSound in sequence
 *
 */
void RTestStepDevSoundRecordBase::Fsm(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
            {
            if (iTestType == EUidTest)
                {
                // Initializing DevSound object for the mode for processing audio data, with hardware device UID
                INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
                TRAPD(err, iMMFDevSound->InitializeL(*this, iEncoderHwDevUid, EMMFStateRecording));
                iCanSchedulerStart = EFalse;
                if (err == KErrNotSupported)
                    {
                    INFO_PRINTF1(_L("Initialization of DevSound failed for Uid with KErrNotSupported. Test PASS!"));
                    iTestStepResult = EPass;
                    }
                else
                    {
                    ERR_PRINTF2(_L("Initialization of DevSound failed with error : %d!"), err);
                    Fsm(EDevSoundError);
                    }
                break;
                }
            else if (iTestType == EInvUidTest)
                {
                // Initializing DevSound object for the mode for processing audio data, with hardware device UID
                INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
                TRAPD(err, iMMFDevSound->InitializeL(*this, iEncoderHwDevUid, EMMFStateRecording));
                iCanSchedulerStart = EFalse;
                #ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
                if (err == KErrNotSupported)
                    {
                    INFO_PRINTF1(_L("Initialization of DevSound failed for invalid Uid with KErrNotSupported. Test PASS!"));
                    iTestStepResult = EPass;
                    }
				#else
                if (err == KErrNotFound)
                    {
                    INFO_PRINTF1(_L("Initialization of DevSound failed for invalid Uid with KErrNotFound. Test PASS!"));
                    iTestStepResult = EPass;
                    }
                #endif
                else
                    {
                    ERR_PRINTF2(_L("Initialization of DevSound failed with error : %d!"), err);
                    Fsm(EDevSoundError);
                    }
                break;
                }
           else if (iTestType == EInvDataTypeTest)
                {
                // Initializing DevSound object for the mode for processing audio data, with TFourCC Code
                INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
                TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
                iCanSchedulerStart = EFalse;
                #ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
                if (err == KErrNotSupported)
                    {
                    INFO_PRINTF1(_L("Initialization of DevSound failed for invalid output data type. Test PASS!"));
                    iTestStepResult = EPass;
                    }
                #else
                if (err == KErrNotFound)
                    {
                    INFO_PRINTF1(_L("Initialization of DevSound failed for invalid output data type. Test PASS!"));
                    iTestStepResult = EPass;
                    }
                #endif
                else
                    {
                    iCanSchedulerStart = EFalse;
                    ERR_PRINTF1(_L("Initialization of DevSound failed for different reason!"));
                    ERR_PRINTF2(_L("Left with error = %d"), err);
                    Fsm(EDevSoundError);
                    }
                break;
                }
            else
                {
                // Initializing DevSound object for the mode for processing audio data, with TFourCC Code
                INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
                TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
                if (err != KErrNone)
                    {
                    iCanSchedulerStart = EFalse;
                    ERR_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
                    Fsm(EDevSoundError);
                    }
                break;    
                }
            }
        
        case EDevSoundRecordInit:
            {            
            TInt maxgain = iMMFDevSound->MaxGain();
            INFO_PRINTF1(_L("iMMFDevSound->SetGain() to MaxGain()"));
            iMMFDevSound->SetGain(maxgain);
            // Initializes audio device and starts the recording process. 
            INFO_PRINTF1(_L("iMMFDevSound->RecordInitL()"));
            TRAPD(err, iMMFDevSound->RecordInitL()); // Get buffer from DevSound
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            break;
            }
        
        case EDevSoundRecordData:
            {
            if (iTestType == EBitRateTest)
                {
                // set config
                TMMFCapabilities devSoundCaps = iMMFDevSound->Capabilities();
                TMMFCapabilities caps;
                
                caps.iRate = EMMFSampleRate8000Hz;    
                caps.iChannels = EMMFStereo;
                caps.iEncoding = EMMFSoundEncoding16BitPCM;
                TInt err = KErrNone;
                TRAP(err,iMMFDevSound->SetConfigL(caps));
                #ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
                if(err == KErrNotReady)
                    {
                    INFO_PRINTF2(_L("SetConfigL() left with expected error = %d"), err);
                    iTestStepResult = EPass;
                    Fsm(EDevSoundError);
                    }
                else
                	{
					ERR_PRINTF2(_L("ERROR : SetConfigL() left with unexpected error = %d"), err);
					Fsm(EDevSoundError);
					}
				break;
                #else
                if(err)
                    {
                    ERR_PRINTF2(_L("ERROR : SetConfigL() left with error = %d"), err);
                    Fsm(EDevSoundError);
                    break;
                    }
                // Set bit-rate
                INFO_PRINTF1(_L("-- iMMFDevSound->CustomInterface(KUidCustomInterfaceDevSoundBitRate)"));
                MMMFDevSoundCustomInterfaceBitRate* bitrate = static_cast <MMMFDevSoundCustomInterfaceBitRate*>(iMMFDevSound->CustomInterface(KUidCustomInterfaceDevSoundBitRate));
                if (!bitrate)
                    {
                    ERR_PRINTF1(_L("Error in getting CustomInterface for bitrate..."));
                    Fsm(EDevSoundError);
                    break;
                    }
                
                INFO_PRINTF2(_L("-- bitrate->SetBitRateL(%d)"), KInvalidBitRateSetValue);
                TRAP(err, bitrate->SetBitRateL(KInvalidBitRateSetValue));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("ERROR : SetBitRateL() left with error = %d"), err);
                    Fsm(EDevSoundError);
                    break;
                    }
                
                // Get bit-rate, and compare with nearest valid bitrate...(88888 -> 80000)
                TInt newBitrate = 0;
                TRAP(err, newBitrate = bitrate->BitRateL());
                INFO_PRINTF2(_L("-- bitrate->BitRateL() returned (%d)"), newBitrate);
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("ERROR : BitRateL() left with error = %d"), err);
                    Fsm(EDevSoundError);
                    break;
                    }
                else if(newBitrate != KInvalidBitRateGetValue)
                    {
                    ERR_PRINTF3(_L("ERROR : Returned bitrate(%d) doesn't match expected(%d)"), newBitrate, KInvalidBitRateGetValue);
                    Fsm(EDevSoundError);
                    break;
                    }
                #endif
                }
                
            if(!iRestarted)
                {
                // initialise and start the timer.
                TRAPD(err, iRecordTimer = CMmfDevSoundRecordTimer::NewL(this, KMicroSecsInOneMinute));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("CMmfDevSoundRecordTimer::NewL left with error = %d"), err);
                    Fsm(EDevSoundError);
                    break;
                    }
                iRecordTimer->Start();
                iRestarted = ETrue;
                }
            
            // Write buffer to file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            TInt err = iFile.Write(buffer->Data());
            if(err != KErrNone)
            	{
            	ERR_PRINTF2(_L("iFile.Write() returned the error %d"), err);
                Fsm(EDevSoundError);
                break;	
            	}

            iBufferCount++;
            
             // Recording Data to a Buffer
            iMMFDevSound->RecordData();
            
            break;
            }
        
        case EDevSoundRecordStopped:
            {
            // delete the iRecordTimer
            iRecordTimer->Cancel();
            delete iRecordTimer;
            iRecordTimer = NULL;
            User::After(100000); // wait for stop to be complete
            // get the samples recorded
            TInt sampRec = iMMFDevSound->SamplesRecorded();
            INFO_PRINTF2(_L("Number of Samples recorded = %d"), sampRec);
            // check for buffer count
            INFO_PRINTF2(_L("Number of Buffers recorded = %d"), iBufferCount);
            iTestStepResult = EPass;
            Fsm(EDevSoundError);
            break;
            }
        
        case EDevSoundError:
            {
            StopScheduler(); // Stop the Scheduler
            break;
            }
 
        default:
            {
            INFO_PRINTF1(_L("Invalid DevSound event!"));
            }
        }
    }

/**
 *
 * DoTestStepPreambleL()
 *
 */
TVerdict RTestStepDevSoundRecordBase::DoTestStepPreambleL()
    {
    //Install the Active Scheduler
    iActiveScheduler = new (ELeave) CActiveScheduler;
    CActiveScheduler::Install (iActiveScheduler);
    
    //Create DevSound object
    ASSERT(iMMFDevSound == NULL);
    TRAPD(err, iMMFDevSound = CMMFDevSound::NewL());
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not create DevSound object. Error = %d"), err);
        return EInconclusive;
        }

    //Get the filename of the audio file to record
    TPtrC filename; 
    if ( !GetStringFromConfig(iSectName, iKeyName, filename) )
        {
        ERR_PRINTF1(_L("Error in getting filename from INI file."));
        return EInconclusive;
        }

    GetDriveName(iFilename);
    iFilename.Append(filename);
    
    // connect RFs
    err = iFs.Connect();
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not connect to Filesystem. Error = %d"), err);
        return EInconclusive;
        }
    
    // Create output directory if it doesn't already exist
    err = iFs.MkDir(_L("c:\\tsi_mmf_devsound\\output\\"));
    if (err != KErrAlreadyExists && err != KErrNone)
        {
        ERR_PRINTF2(_L("iFile.MkDir failed with %d"), err);
        return EInconclusive;
        }
    
    // create / replace a file using RFile for recording
    err = iFile.Replace(iFs, iFilename, EFileWrite);
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not create output file. Error = %d"), err);
        iFs.Close();
        return EInconclusive;
        }
    
    // display trace about filename and codec under test
    INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
    TUint32 fourCC = iOutputDataType.FourCC();
    INFO_PRINTF5(_L("Codec under test -> %c%c%c%c"), fourCC, fourCC>>8, fourCC>>16, fourCC>>24);
    
    return EPass;
    }

/**
 *
 * DoTestStepPostambleL()
 *
 */
TVerdict RTestStepDevSoundRecordBase::DoTestStepPostambleL()
    {
    if (iMMFDevSound)
        {
        INFO_PRINTF1(_L("-- Deleting DevSound object..."));
        delete iMMFDevSound;
        iMMFDevSound = NULL;
        }

    delete iActiveScheduler;
    iActiveScheduler = NULL;

    iFile.Close();
    iFs.Close();

    return EPass;
    }

/**
 *
 * InitializeComplete()
 *
 */
void RTestStepDevSoundRecordBase::InitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);
    
    if (aError != KErrNone)
        {
        Fsm(EDevSoundError);
        return;
        }
    else
        {
        // Call to initialize the audio device and start the record process
        Fsm(EDevSoundRecordInit);
        }
     }

/**
 *
 * ToneFinished()
 *
 */
void RTestStepDevSoundRecordBase::ToneFinished(TInt /*aError*/)
    {
    }

/**
 *
 * BufferToBeFilled()
 *
 */
void RTestStepDevSoundRecordBase::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
    {
    }

/**
 *
 * PlayError()
 *
 */
void RTestStepDevSoundRecordBase::PlayError(TInt /*aError*/)
    {
    }

/**
 *
 * BufferToBeEmptied()
 *
 */
void RTestStepDevSoundRecordBase::BufferToBeEmptied(CMMFBuffer* aBuffer)
    {
    INFO_PRINTF1(_L("DevSound called BufferToBeEmptied."));
    
    if (!aBuffer)
        {
        INFO_PRINTF1(_L("BufferToBeEmptied callback received a NULL CMMFBuffer"));
        Fsm(EDevSoundError);
        return;
        }
        
    iBuffer = aBuffer;
    Fsm(EDevSoundRecordData);
    }

/**
 *
 * RecordError()
 *
 */
void RTestStepDevSoundRecordBase::RecordError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called RecordError with error = %d"), aError);
    Fsm(EDevSoundError);
    }

/**
 *
 * ConvertError
 *
 */
void RTestStepDevSoundRecordBase::ConvertError(TInt /*aError*/)
    {
    }

/**
 *
 * DeviceMessage
 *
 */
void RTestStepDevSoundRecordBase::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
    {
    }

/**
 * set finished on callback from a timer
 */
void RTestStepDevSoundRecordBase::Finished()
    {
    INFO_PRINTF1(_L("-- Stopping finally after 60 seconds..."));
    iMMFDevSound->Stop();
    Fsm(EDevSoundRecordStopped);
    }

//
// RTestStepDevSoundRecordDatatype
//

/**
 *
 * RTestStepDevSoundRecordDatatype - Test step constructor
 *
 */
RTestStepDevSoundRecordDatatype::RTestStepDevSoundRecordDatatype(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType) 
    : RTestStepDevSoundRecordBase(aTestName, aSectName, aKeyName, aOutputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundRecordDatatype* RTestStepDevSoundRecordDatatype::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType)
    {
    RTestStepDevSoundRecordDatatype* self = new (ELeave) RTestStepDevSoundRecordDatatype(aTestName, aSectName, aKeyName, aOutputDataType);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundRecordDatatype::DoTestStepL()
    {
    iTestStepResult = EFail; // Assume test will fail

    if (iTestStepName.Compare(_L("MM-MMF-DEVSOUND-I-0601-HP")) == 0)
        {
        // Test: MM-MMF-DEVSOUND-I-0601-HP
        INFO_PRINTF1(_L("<<< Record to an Ogg vorbis audio file specifying invalid output data type >>>"));
        iTestType = EInvDataTypeTest;
        }
    else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUND-I-0101-HP")) == 0)
        {
        // Test: MM-MMF-DEVSOUND-I-0101-HP
        
       	// This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
		INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
		return EInconclusive;
#else        
        INFO_PRINTF1(_L("<<< Record an OggVorbis audio file >>>"));
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND        
        }
    
    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
    }

//
// RTestStepDevSoundRecordHwDevUid
//

/**
 *
 * RTestStepDevSoundRecordHwDevUid - Test step constructor
 *
 */
RTestStepDevSoundRecordHwDevUid::RTestStepDevSoundRecordHwDevUid(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepDevSoundRecordBase(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundRecordHwDevUid* RTestStepDevSoundRecordHwDevUid::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    {
    RTestStepDevSoundRecordHwDevUid* self = new (ELeave) RTestStepDevSoundRecordHwDevUid(aTestName, aSectName, aKeyName);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundRecordHwDevUid::DoTestStepL()
    {
    iTestStepResult = EFail; // Assume test will fail

    if (iTestStepName.Compare(_L("MM-MMF-DEVSOUND-I-0602-HP")) == 0)
        {
        // Test: MM-MMF-DEVSOUND-I-0602-HP
        INFO_PRINTF1(_L("<<< Record to an Ogg vorbis audio file specifying invalid Uid >>>"));

        iTestType = EInvUidTest; // Test uses Invalid Uid for initialization
        iEncoderHwDevUid.iUid = 0x10273838; // Invalid Uid
        }
    else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUND-I-0603-HP")) == 0)
        {
        // Test: MM-MMF-DEVSOUND-I-0603-HP
        INFO_PRINTF1(_L("<<< Record an OggVorbis audio file specifying Uid of HwDevice >>>"));

        iTestType = EUidTest; // Test uses Uid for initialization (earlier - KMmfUidHwDeviceMulawToPCM16)
        iEncoderHwDevUid.iUid = KUidTestVorbisEncoderPU;//KUidHwDeviceAudioAdapter
        }

    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
    }

//
// RTestStepDevSoundRecordConfHwDev
//
    
/**
 * 
 * RTestStepDevSoundRecordConfHwDev - Test step constructor
 *
 */
RTestStepDevSoundRecordConfHwDev::RTestStepDevSoundRecordConfHwDev(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType) 
    : RTestStepDevSoundRecordBase(aTestName, aSectName, aKeyName, aOutputDataType)
    {
    iReconfigured = EFalse;
    iPaused = EFalse;
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundRecordConfHwDev* RTestStepDevSoundRecordConfHwDev::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType)
    {
    RTestStepDevSoundRecordConfHwDev* self = new (ELeave) RTestStepDevSoundRecordConfHwDev(aTestName, aSectName, aKeyName, aOutputDataType);
    return self;
    }

/**
 *
 * Fsm()
 * Executes events of DevSound in sequence
 *
 */
void RTestStepDevSoundRecordConfHwDev::Fsm(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
            {
            // Initializing DevSound object for the mode for processing audio data, with TFourCC code
            INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
            TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
            if (err != KErrNone)
                {
                iCanSchedulerStart = EFalse;
                ERR_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            break;
            }
        
        case EDevSoundRecordInit:
            {
            // Initializes audio device and starts the recording process. 
            INFO_PRINTF1(_L("iMMFDevSound->RecordInitL()"));
            TRAPD(err, iMMFDevSound->RecordInitL()); // Get buffer from DevSound
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            
            // initialise and start the timer
            TRAP(err, iRecordTimer = CMmfDevSoundRecordTimer::NewL(this, KMicroSecsInThirtySec));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("CMmfDevSoundRecordTimer::NewL left with error = %d"), err);
                Fsm(EDevSoundError);
                break;
                }
            iRecordTimer->Start();
            INFO_PRINTF1(_L("Recording Timer has been started - will call Stop() in 30 seconds"));
            break;
            }
        
        case EDevSoundRecordData:
            {
            // Write buffer to file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            TInt err = iFile.Write(buffer->Data());
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("iFile.Write() failed with error = %d"), err);
                Fsm(EDevSoundError);
                }

            iBufferCount++;
            
             // Recording Data to a Buffer
            iMMFDevSound->RecordData();
            
            break;
            }
        
        case EDevSoundRecordPaused:
            {
            break; //- wait for empty buffer
            }
        
        case EDevSoundError:
            {
            StopScheduler(); // Stop the Scheduler
            break;
            }
        
        default:
            {
            INFO_PRINTF1(_L("Invalid DevSound event!"));
            }
        }
    }

/**
 *
 * InitializeComplete()
 *
 */
void RTestStepDevSoundRecordConfHwDev::InitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);
    
    if (aError != KErrNone)
        {
        Fsm(EDevSoundError);
        return;
        }
    else
        {
        TMMFCapabilities capabilities;
        if(iReconfigured)
        	{
        	// re-configure devsound & restart recording
            capabilities.iRate = EMMFSampleRate8000Hz;
            capabilities.iEncoding = EMMFSoundEncoding8BitPCM;
            capabilities.iChannels = EMMFMono;
            INFO_PRINTF1(_L("-- Configuring :: samplerate='EMMFSampleRate8000Hz', encoding='EMMFSoundEncoding8BitPCM' and channel='EMMFMono'."));
            }   
        else
        	{
        	// configure for the first time
	        capabilities.iRate = EMMFSampleRate16000Hz;
	        capabilities.iEncoding = EMMFSoundEncoding16BitPCM;
	        capabilities.iChannels = EMMFStereo;
	        INFO_PRINTF1(_L("-- Configuring :: samplerate='EMMFSampleRate16000Hz', encoding='EMMFSoundEncoding16BitPCM' and channel='EMMFStereo'."));
	        }  	  
        TRAPD(err, iMMFDevSound->SetConfigL(capabilities));
        if (err != KErrNone)
            {
            ERR_PRINTF2(_L("Configuration of DevSound object failed! Left with error = %d"), err);
            Fsm(EDevSoundError);
            }
        // Call to initialize the audio device and start the record process
        Fsm(EDevSoundRecordInit);
        }
    }
    
/**
 *
 * BufferToBeEmptied()
 *
 */
void RTestStepDevSoundRecordConfHwDev::BufferToBeEmptied(CMMFBuffer* aBuffer)
    {
    if (!aBuffer)
        {
        INFO_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer"));
        Fsm(EDevSoundError);
        return;
        }
        
    iBuffer = aBuffer;    
	INFO_PRINTF2(_L("DevSound called BufferToBeEmptied, with size = %d"), aBuffer->BufferSize());
    
	if(aBuffer->BufferSize() == 0)
    	{
    	if(iPaused)
    		{
    		if(iReconfigured)
	    		{
	    		// 	We need to call Stop here if we have an empty buffer.
				INFO_PRINTF1(_L("In Paused state and have received an empty CMMFBuffer"));
				iMMFDevSound->Stop();
				INFO_PRINTF1(_L("Called iMMFDevSound->Stop()"));
				iTestStepResult = EPass;
				iRecordTimer->Cancel();
				delete iRecordTimer;
				iRecordTimer = NULL;
				Fsm(EDevSoundError);
	    		}    	
    		else
	    		{
	    		// We just need the empty buffer, then we re-init the encode and timer
	    		INFO_PRINTF1(_L("In Paused state and have received an empty CMMFBuffer"));
	    		iPaused = EFalse;
				INFO_PRINTF1(_L("**** Resuming the encode"));
				iRecordTimer->Cancel();
				delete iRecordTimer;
				iRecordTimer = NULL;
				iMMFDevSound->Stop();
				INFO_PRINTF1(_L("Called iMMFDevSound->Stop()"));
				iReconfigured = ETrue;
				// Call to initialize the audio device and start the record process
	        	Fsm(EDevSoundInit);
	    		}
    		}
    	else
    		{
    		INFO_PRINTF1(_L("**** We received null buffer before calling pause. UnExpected behaviour"));
    		Fsm(EDevSoundError);
    		}        
        }
    else
    	{
    	// Call to continue the Record process
        Fsm(EDevSoundRecordData);
    	}
    }
 
/**
 *
 * RecordError()
 *
 */
void RTestStepDevSoundRecordConfHwDev::RecordError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called RecordError with error = %d"), aError);
    Fsm(EDevSoundError);
    }
  
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundRecordConfHwDev::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0102-HP
    
    // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
	INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
	return EInconclusive;
#else    
    INFO_PRINTF1(_L("<<< Configure the audio HwDevice and record an OggVorbis audio file >>>"));
    iTestStepResult = EFail;
    
    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
    }

/**
 * set finished on callback from a timer
 */
void RTestStepDevSoundRecordConfHwDev::Finished()
    {
    INFO_PRINTF1(_L("-- iMMFDevSound->Pause()"));
    iMMFDevSound->Pause();
    iPaused = ETrue;	
    Fsm(EDevSoundRecordPaused);
    }

//
// RTestStepDevSoundListOutputDataTypes
//

/**
 *
 * RTestStepDevSoundListOutputDataTypes - Test step constructor
 *
 */
RTestStepDevSoundListOutputDataTypes::RTestStepDevSoundListOutputDataTypes(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType) 
    : RTestStepDevSoundRecordBase(aTestName, aSectName, aKeyName, aOutputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundListOutputDataTypes* RTestStepDevSoundListOutputDataTypes::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType)
    {
    RTestStepDevSoundListOutputDataTypes* self = new (ELeave) RTestStepDevSoundListOutputDataTypes(aTestName, aSectName, aKeyName, aOutputDataType);
    return self;
    }

/**
 *
 * InitializeComplete()
 *
 */
void RTestStepDevSoundListOutputDataTypes::InitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);
    
    Fsm(EDevSoundError);
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundListOutputDataTypes::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0106-HP
    INFO_PRINTF1(_L("<<< List of supported output data types >>>"));

    iTestStepResult = EPass;
    TUint numberOfSupportedDataTypes = 0;

    TMMFPrioritySettings prioritySettings;
    prioritySettings.iPref = EMdaPriorityPreferenceNone;
    prioritySettings.iPriority = 0;
    prioritySettings.iState = EMMFStateRecording;
    
    RArray<TFourCC> supportedDataTypes;
    CleanupClosePushL(supportedDataTypes);
    INFO_PRINTF1(_L("iMMFDevSound->GetSupportedOutputDataTypesL()"));
    TRAPD(err, iMMFDevSound->GetSupportedOutputDataTypesL(supportedDataTypes, prioritySettings));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Error in getting supported output data types = %d"), err);
        iTestStepResult = EFail;
        }
    else
        {
        numberOfSupportedDataTypes = supportedDataTypes.Count();
        // List the supported data types
        // Note test does not make any assumptions as to what the supported datatypes are
        if (numberOfSupportedDataTypes)
            {
            for (TUint entryNumber = 0; entryNumber < numberOfSupportedDataTypes; entryNumber++)
                {
                TUint32 fourCC = supportedDataTypes[entryNumber].FourCC();
                INFO_PRINTF5(_L("iMMFDevSound->InitializeL(%c%c%c%c)"), fourCC, fourCC>>8, fourCC>>16, fourCC>>24);
                TRAP(err, iMMFDevSound->InitializeL(*this, fourCC, prioritySettings.iState));
                if(err != KErrNone)
                    {
                    ERR_PRINTF5(_L("DevSound failed to support output data type FourCC = %c%c%c%c"), fourCC, fourCC>>8, fourCC>>16, fourCC>>24);
                    ERR_PRINTF2(_L("Left with error = %d"), err);
                    iTestStepResult = EFail;
                    }
                else
                    {
                    StartScheduler();
                    INFO_PRINTF5(_L("DevSound supports output data type FourCC =  %c%c%c%c"), fourCC, fourCC>>8, fourCC>>16, fourCC>>24);
                    }
                }
            }
        else
            {
            ERR_PRINTF1(_L("No supported data types detected for recording."));
            iTestStepResult = EFail;
            }
        }

    CleanupStack::PopAndDestroy(); // supportedDataTypes
    
    return iTestStepResult;
    }

//
// RTestStepDevSoundRecordAlloc
//

/**
 *
 * Test step constructor.
 * Ctor for RTestStepDevSoundRecordAlloc.
 *
 */
RTestStepDevSoundRecordAlloc::RTestStepDevSoundRecordAlloc(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType) 
    : RTestStepDevSoundRecordBase(aTestName, aSectName, aKeyName, aOutputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundRecordAlloc* RTestStepDevSoundRecordAlloc::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType)
    {
    RTestStepDevSoundRecordAlloc* self = new (ELeave) RTestStepDevSoundRecordAlloc(aTestName, aSectName, aKeyName, aOutputDataType);
    return self;
    }

/**
 *
 * FsmL()
 * Executes events of DevSound in sequence
 *
 */
void RTestStepDevSoundRecordAlloc::FsmL(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
            {
            // Initializing DevSound object for the mode for processing audio data, with TFourCC code
            INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
            TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
            iCanSchedulerStart = ETrue;
            if (err != KErrNone)
                {
                iCanSchedulerStart = EFalse;
                ERR_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
                StopScheduler();
                User::Leave(err);
                }
            break;    
            }
        case EDevSoundRecordInit:
            {
            // Initializes audio device and starts the recording process. 
            INFO_PRINTF1(_L("iMMFDevSound->RecordInitL()"));
            TRAPD(err, iMMFDevSound->RecordInitL()); // Get buffer from DevSound
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
                StopScheduler();
                User::Leave(err);
                }
            break;
            }
        case EDevSoundRecordData:
            {
            // Write buffer to file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            TInt err = iFile.Write(buffer->Data());
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("iFile.Write failed with %d"), err);
                StopScheduler();
                User::Leave(err);
                }

            iBufferCount++;
            
             // Recording Data to a Buffer
            iMMFDevSound->RecordData();
            
            break;
            }
        default:
            {
            INFO_PRINTF1(_L("Invalid DevSound event!"));
            }
        }
    }

/**
 *
 * Record audio by marking heap.
 *
 */
void RTestStepDevSoundRecordAlloc::AudioRecordAllocL()
    {
    __MM_HEAP_MARK;
    
    // create new devsound instance
    ASSERT(iMMFDevSound == NULL);
    iMMFDevSound = CMMFDevSound::NewL();
    
    // call the state handler and start the test
    iError = KErrNone;
    FsmL(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once
    
    // cleanup devsound instance
    delete iMMFDevSound;
    iMMFDevSound = NULL;
    
    // check for errors
    if ((iError != KErrNone) && (iError != KErrOverflow))
        {
        ERR_PRINTF2(_L("Opening/recording the audio file failed, unexpected error : %d."), iError);
        User::Leave(iError);
        }
    
    __MM_HEAP_MARKEND;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundRecordAlloc::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0103-HP
    
    // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
	INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
	return EInconclusive;
#else    
    // The base class PreambleL will create the DevSound object.
    // The calls to AudioRecordAllocL will recreate it.
    if (iMMFDevSound)
    	{
    	delete iMMFDevSound;
    	iMMFDevSound = NULL;
    	}
    
    INFO_PRINTF1(_L("<<< Alloc test to load an audio HwDevice to record an OggVorbis audio file >>>"));
    iTestStepResult = EFail;
    
    TRAPD(err, AudioRecordAllocL());
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Opening/recording the audio file failed, unexpected error %d."), err);
        return iTestStepResult;
        }

    iTestStepResult  = EPass;

    TInt failCount   = 1;
    TBool completed  = EFalse;
    TBool badResult  = EFalse;
    TBool reachedEnd = EFalse;

    for(;;)
        {
        __UHEAP_SETFAIL(RHeap::EFailNext, failCount);
        __MM_HEAP_MARK;

        TInt err = KErrNone;
        TRAP(err, AudioRecordAllocL());

        delete iMMFDevSound;
        iMMFDevSound = NULL;
        
        completed = EFalse;
        if (err == KErrNone)
            {
            TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
            if (!testAlloc)
                {
                reachedEnd = ETrue;
                failCount--;
                }
            else
                {
                User::Free(testAlloc);
                testAlloc = NULL;
                }

            if (iTestStepResult != EPass)
                {
                badResult = ETrue;
                }

            completed = reachedEnd || badResult;
            }
        else if (err != KErrNoMemory)
            {
            __UHEAP_SETFAIL(RHeap::ENone, 0);
            ERR_PRINTF2(_L("Bad error code %d"), err);
            completed = ETrue;
            badResult = EFail;
            }

        __MM_HEAP_MARKEND;
        __UHEAP_SETFAIL(RHeap::ENone, 0);

        if (completed)
            {
            break;
            }

        failCount++;
        }

    if (err != KErrNone || badResult)
        {
        if (badResult)
            {
            ERR_PRINTF3(_L("Bad result with %d memory allocations tested. The Error returned is %d."), failCount, err);
            }
        else
            {
            ERR_PRINTF3(_L("Error(%d) with %d memory allocations tested."), failCount, err);
            }

        iTestStepResult = EFail;
        }
    else
        {
        INFO_PRINTF2(_L("Completed OK with %d memory allocations tested.\n"), failCount);
        iTestStepResult = EPass;
        }

    return iTestStepResult;
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
    }

/**
 *
 * InitializeComplete()
 *
 */
void RTestStepDevSoundRecordAlloc::InitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);
    
    iError = aError;
    if (aError != KErrNone)
        {
        StopScheduler();
        }
    else
        {
        // Call to initialize the audio device and start the record process
        TRAP(iError, FsmL(EDevSoundRecordInit));
        }
     }


/**
 *
 * BufferToBeEmptied()
 *
 */
void RTestStepDevSoundRecordAlloc::BufferToBeEmptied(CMMFBuffer* aBuffer)
    {
    INFO_PRINTF1(_L("DevSound called BufferToBeEmptied."));
    
    if (!aBuffer)
        {
        INFO_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer"));
        StopScheduler();
        iError = KErrNotFound;
        return;
        }
        
    iBuffer = aBuffer;

    if (iBufferCount < 5) // Record 5 buffers
        {
        // Call to initialize the audio device and start the Record process
        TRAP(iError, FsmL(EDevSoundRecordData));
        }
    else
        {
        iMMFDevSound->Stop();
        iTestStepResult = EPass;
        StopScheduler();
        }
    }

/**
 *
 * RecordError()
 *
 */
void RTestStepDevSoundRecordAlloc::RecordError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called RecordError with error = %d"), aError);
    
    StopScheduler();
    iError = aError;
    
    if ((iBufferCount == 5) && (aError == KErrOverflow))
        {
        INFO_PRINTF1(_L("We have recorded 5 buffers, test PASS"));
        iTestStepResult = EPass;
        }
    else
        {
        ERR_PRINTF1(_L("Last Buffer is NOT set, so test FAIL"));
        iTestStepResult = EFail;
        }
    }

//
// RTestStepDevSoundRecordStop
//

/**
 *
 * RTestStepDevSoundRecordStop - Test step constructor
 *
 */
RTestStepDevSoundRecordStop::RTestStepDevSoundRecordStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType) 
    : RTestStepDevSoundRecordBase(aTestName, aSectName, aKeyName, aOutputDataType)
    {
    iRecordStopTimer = NULL;
    iStopCount = 0;
    iStopped = EFalse;
    iPaused = EFalse;
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundRecordStop* RTestStepDevSoundRecordStop::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType)
    {
    RTestStepDevSoundRecordStop* self = new (ELeave) RTestStepDevSoundRecordStop(aTestName, aSectName, aKeyName, aOutputDataType);
    return self;
    }

/**
 * set finished on callback from a timer
 */
void RTestStepDevSoundRecordStop::Finished()
    {
    iStopCount++;
    INFO_PRINTF1(_L("-- iMMFDevSound->Pause()"));
    iMMFDevSound->Pause();
    iPaused = ETrue;
    Fsm(EDevSoundRecordPaused);
    }

/**
 *
 * Fsm()
 * Executes events of DevSound in sequence
 *
 */
void RTestStepDevSoundRecordStop::Fsm(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
            {
            // Initializing DevSound object for the mode for processing audio data, with TFourCC code
            INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
            TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
            if(err != KErrNone)
                {
                iCanSchedulerStart = EFalse;
                ERR_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            break;
            }

        case EDevSoundRecordInit:
            {
            // Initializes audio device and starts the recording process. 
            INFO_PRINTF1(_L("iMMFDevSound->RecordInitL()"));
            TRAPD(err, iMMFDevSound->RecordInitL()); // Get buffer from DevSound
            if(err != KErrNone)
                {
                ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            
            // initialise and start the timer
            TRAP(err, iRecordStopTimer = CMmfDevSoundRecordTimer::NewL(this, KMicroSecsInThirtySec)); // 30 seconds
            if (err != KErrNone)
            	{
            	ERR_PRINTF2(_L("CMmfDevSoundPlayTimer::NewL left with error = %d"), err);
                Fsm(EDevSoundError);
                break;
                }
            iRecordStopTimer->Start();
            INFO_PRINTF1(_L("Recording Timer has been started - will call Stop() in 30 seconds"));
            break;
            }

        case EDevSoundRecordData:
            {
            // Write buffer to file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            TInt err = iFile.Write(buffer->Data());
            if(err != KErrNone)
                {
                ERR_PRINTF2(_L("iFile.Write() failed with %d"), err);
                Fsm(EDevSoundError);
                }

            iBufferCount++;
            
            // Recording Data to a Buffer
            iMMFDevSound->RecordData();
            break;
            }
        
        case EDevSoundRecordPaused:
            {
            break; // await empty buffer callback
            }
        
        case EDevSoundError:
            {
            StopScheduler(); // Stop the Scheduler
            break;
            }    
 
        default:
            {
            INFO_PRINTF1(_L("Invalid DevSound event!"));
            }
        }
    }

/**
 *
 * InitializeComplete()
 *
 */
void RTestStepDevSoundRecordStop::InitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);
    
    if (aError != KErrNone)
        {
        Fsm(EDevSoundError);
        return;
        }
    else
        {
        // Call to initialize the audio device and start the record process
        Fsm(EDevSoundRecordInit);
        }
    }
    
/**
 *
 * BufferToBeEmptied()
 *
 */
void RTestStepDevSoundRecordStop::BufferToBeEmptied(CMMFBuffer* aBuffer)
    {
    if (!aBuffer)
        {
        INFO_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer"));
        Fsm(EDevSoundError);
        return;
        }
    
    iBuffer = aBuffer;
	INFO_PRINTF2(_L("DevSound called BufferToBeEmptied, with size = %d"), aBuffer->BufferSize());
    
    if(aBuffer->BufferSize() == 0)
    	{
    	if(iPaused)
    		{
    		// 	We need to call Stop here if we have an empty buffer.
			INFO_PRINTF1(_L("In Paused state and have received an empty CMMFBuffer"));
			iMMFDevSound->Stop();
			INFO_PRINTF1(_L("Called iMMFDevSound->Stop()"));
			INFO_PRINTF1(_L("**** Ending the test"));
			iTestStepResult = EPass;
			iRecordStopTimer->Cancel();
			delete iRecordStopTimer;
			iRecordStopTimer = NULL;
			iPaused = EFalse;
			if(!iStopped)
				{
				iStopped = ETrue;//stopped for the first time
				Fsm(EDevSoundInit);
				}
			else
				{
				Fsm(EDevSoundError);
				}
			}
    	else
    		{
    		INFO_PRINTF1(_L("**** We received null buffer before calling pause. UnExpected behaviour"));
    		Fsm(EDevSoundError);
    		}
    	}
    else
    	{
    	// Call to continue the Record process
        Fsm(EDevSoundRecordData);
        }
    }
 
/**
 *
 * RecordError()
 *
 */
void RTestStepDevSoundRecordStop::RecordError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called RecordError with error = %d"), aError);
    Fsm(EDevSoundError);
    }
  
/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundRecordStop::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0104-HP
    
    // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
		INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
		return EInconclusive;
#else    
    INFO_PRINTF1(_L("<<< Record/stop recording an OggVorbis audio file >>>"));
    iTestStepResult = EFail;

    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND
    }

//
// RTestStepDevSoundRecordPause
//

/**
 *
 * RTestStepDevSoundRecordPause - Test step constructor
 *
 */
RTestStepDevSoundRecordPause::RTestStepDevSoundRecordPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType) 
    : RTestStepDevSoundRecordBase(aTestName, aSectName, aKeyName, aOutputDataType)
    {
    iRecordPauseTimer = NULL;
    iPauseCount = 0;
    iRestarted = EFalse;
    iPaused = EFalse;
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundRecordPause* RTestStepDevSoundRecordPause::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType)
    {
    RTestStepDevSoundRecordPause* self = new (ELeave) RTestStepDevSoundRecordPause(aTestName, aSectName, aKeyName, aOutputDataType);
    return self;
    }

/**
 * set finished on callback from a timer
 */
void RTestStepDevSoundRecordPause::Finished()
    {
    iPauseCount++;
    
    INFO_PRINTF1(_L("-- iMMFDevSound->Pause()"));
    iMMFDevSound->Pause();
    iPaused = ETrue;
    iRestarted = EFalse;
    Fsm(EDevSoundRecordPaused);
    }

/**
 *
 * Fsm()
 * Executes events of DevSound in sequence
 *
 */
void RTestStepDevSoundRecordPause::Fsm(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
            {
            // Initializing DevSound object for the mode for processing audio data, with TFourCC code
            INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
            TRAPD(err, iMMFDevSound->InitializeL(*this, iOutputDataType, EMMFStateRecording));
            if(err != KErrNone)
                {
                iCanSchedulerStart = EFalse;
                ERR_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            break;
            }
        
        case EDevSoundRecordInit:
            {
            // Initializes audio device and starts the recording process. 
            INFO_PRINTF1(_L("iMMFDevSound->RecordInitL()"));
            TRAPD(err, iMMFDevSound->RecordInitL()); // Get buffer from DevSound
            if(err != KErrNone)
                {
                ERR_PRINTF2(_L("DevSound RecordInitL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            
            // initialise and start the timer, to pause in 30 secs
            TRAP(err, iRecordPauseTimer = CMmfDevSoundRecordTimer::NewL(this, KMicroSecsInThirtySec));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("CMmfDevSoundPlayTimer::NewL left with error = %d"), err);
                Fsm(EDevSoundError);
                break;
                }
            iRecordPauseTimer->Start();
            INFO_PRINTF1(_L("Recording Timer has been started - will call Pause() in 30 seconds"));
            break;
            }
        
        case EDevSoundRecordData:
            {
            // Write buffer to file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            TInt err = iFile.Write(buffer->Data());
			if (err != KErrNone)
                {
                ERR_PRINTF2(_L("iFile.Write() failed with error = %d"), err);
                Fsm(EDevSoundError);
                break;
                }

            iBufferCount++;
            
            // Recording Data to a Buffer
            iMMFDevSound->RecordData();
            break;
            }
        
        case EDevSoundRecordPaused:
            {
            INFO_PRINTF1(_L("-- Recording is Paused, waiting for the empty buffer..."));
            break; // wait for the empty buffer
            }
        
        case EDevSoundError:
            {
            StopScheduler(); // Stop the Scheduler
            break;
            }
 
        default:
            {
            INFO_PRINTF1(_L("Invalid DevSound event!"));
            }
        }
    }

/**
 *
 * InitializeComplete()
 *
 */
void RTestStepDevSoundRecordPause::InitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);
    
    if (aError != KErrNone)
        {        
        Fsm(EDevSoundError);        
        return;
        }
    else
        {
        // Call to initialize the audio device and start the record process
        Fsm(EDevSoundRecordInit);
        }
    }

/**
 *
 * BufferToBeEmptied()
 *
 */
void RTestStepDevSoundRecordPause::BufferToBeEmptied(CMMFBuffer* aBuffer)
    {
    INFO_PRINTF1(_L("DevSound called BufferToBeEmptied."));
    
    if (!aBuffer)
        {
        ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer"));
        Fsm(EDevSoundError);
        return;
        }
        
    iBuffer = aBuffer;    

    
    if(aBuffer->BufferSize() == 0)
    	{
    	if(iPaused)
    		{
    		// 	We need to call Stop here if we have an empty buffer.
			INFO_PRINTF1(_L("In Paused state and have received an empty CMMFBuffer"));
			iMMFDevSound->Stop();
			INFO_PRINTF1(_L("Called iMMFDevSound->Stop()"));
			INFO_PRINTF1(_L("**** Ending the test"));
			iTestStepResult = EPass;
			iRecordPauseTimer->Cancel();
			delete iRecordPauseTimer;
			iRecordPauseTimer = NULL;
			Fsm(EDevSoundError);
    		}
    	else
    		{
    		INFO_PRINTF1(_L("**** We received null buffer before calling pause. UnExpected behaviour"));
    		Fsm(EDevSoundError);
    		}
    	}
    else
    	{
    	// Call to continue the Record process
        Fsm(EDevSoundRecordData);
        }
    }

/**
 *
 * RecordError()
 *
 */
void RTestStepDevSoundRecordPause::RecordError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called RecordError with error = %d"), aError);
    Fsm(EDevSoundError);
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundRecordPause::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0105-HP
    
    // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
	INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
	return EInconclusive;
#else    
    INFO_PRINTF1(_L("<<< Record/pause recording an OggVorbis audio file >>>"));
    iTestStepResult = EFail;

    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND
    }


//
// Negative Tests //
//

//
// RTestStepDevSoundRecordInvBitrate
//
   
/**
 *
 * RTestStepDevSoundRecordInvBitrate - Test step constructor
 *
 */
RTestStepDevSoundRecordInvBitrate::RTestStepDevSoundRecordInvBitrate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType)
    : RTestStepDevSoundRecordBase(aTestName, aSectName, aKeyName, aOutputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundRecordInvBitrate* RTestStepDevSoundRecordInvBitrate::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType)
    {
    RTestStepDevSoundRecordInvBitrate* self = new (ELeave) RTestStepDevSoundRecordInvBitrate(aTestName, aSectName, aKeyName, aOutputDataType);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundRecordInvBitrate::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0604-HP
    
    // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
	INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
	return EInconclusive;
#else    
    INFO_PRINTF1(_L("<<< Record to an OggVorbis audio file at an invalid bit rate >>>"));
    iTestStepResult = EFail;
    iTestType = EBitRateTest;

    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND
    }

//
// RTestStepDevSoundRecordCorruptFile
//

/**
 *
 * RTestStepDevSoundRecordCorruptFile - Test step constructor
 *
 */
RTestStepDevSoundRecordCorruptFile::RTestStepDevSoundRecordCorruptFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType)
    : RTestStepDevSoundRecordBase(aTestName, aSectName, aKeyName, aOutputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundRecordCorruptFile* RTestStepDevSoundRecordCorruptFile::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aOutputDataType)
    {
    RTestStepDevSoundRecordCorruptFile* self = new (ELeave) RTestStepDevSoundRecordCorruptFile(aTestName, aSectName, aKeyName, aOutputDataType);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundRecordCorruptFile::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0605-HP
    
    // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
	INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
	return EInconclusive;
#else    
    INFO_PRINTF1(_L("<<< Record to a corrupted OggVorbis audio file >>>"));
    iTestStepResult = EFail;

    // Write some junk data to the file, and make it corrupt.
    TInt err = iFile.Write(_L8("This is added to possibly corrupt the file..."));
    if(err != KErrNone)
    	{
    	ERR_PRINTF2(_L("iFile.Write() failed with %d"), err);
    	return iTestStepResult;
    	}
    
    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND
    }

//
// CMmfDevSoundRecordTimer
//

/**
 * NewL
 */
CMmfDevSoundRecordTimer* CMmfDevSoundRecordTimer::NewL(RTestStepDevSoundRecordBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    {
    CMmfDevSoundRecordTimer* self = new (ELeave) CMmfDevSoundRecordTimer(aParent, aWaitTime);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }
    
/**
 * Constructor
 */
CMmfDevSoundRecordTimer::CMmfDevSoundRecordTimer(RTestStepDevSoundRecordBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    : CTimer(EPriorityNormal), iParent(aParent), iWaitTime(aWaitTime)
    {    
    CActiveScheduler::Add(this);
    }    
    
/**
 * Start() - Start the timer
 */
void CMmfDevSoundRecordTimer::Start()
    {
    After(iWaitTime);
    }

/**
 * RunL() - see CActive class for more info
 */
void CMmfDevSoundRecordTimer::RunL()
    {        
    // (iParent->Finished() should call Stop() or not, as we decide)    
    iParent->Finished();
    }

/**
 * RunError() - see CActive class for more info
 */
TInt CMmfDevSoundRecordTimer::RunError(TInt aError)
    {
    iParent->Fsm(EDevSoundError);
    return aError;
    }

