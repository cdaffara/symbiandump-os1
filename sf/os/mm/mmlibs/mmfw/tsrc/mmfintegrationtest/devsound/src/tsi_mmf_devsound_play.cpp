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
// Implementation of test step classes for DevSound - Play.
// 
//


#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>
#include <mmf/server/devsoundstandardcustominterfaces.h>
#include "tsi_mmf_devsound_play.h"

//
// Positive Tests //
//

//
// RTestStepDevSoundPlayBase
//

/**
 *
 * RTestStepDevSoundPlayBase - Test step constructor
 *
 */
RTestStepDevSoundPlayBase::RTestStepDevSoundPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    {
    // Store the name of this test case
    iTestStepName = aTestName;

    // Section name for retrieving filename
    iSectName = aSectName;

    // Key name for retrieving filename
    iKeyName = aKeyName;
    
    // we are allocating buffers in these tests so we must grow the heap
    iHeapSize = 0x500000;     // 5MB
    
    // Datatype of the Decoder
    iInputDataType = aInputDataType;
    
    iLastBuffer = EFalse;   // Set last buffer to be played
    iBufferCount = 0;       // Number of buffers played
    iTestType = ENormalTest;
    
    iMMFDevSound = NULL;
    }

/**
 *
 * CopyFile - To copy the original file for Play / record.
 *
 */
TInt RTestStepDevSoundPlayBase::CopyFile(TPtrC aFileNameSrc, TPtrC aFileNameDst)
{
	TInt theErr = KErrNone;

	RFs		fs;
	fs.Connect();
	RFile	theFileNew;
	RFile	theFile;

	fs.MkDirAll(aFileNameDst);
	theFile.Open(fs, aFileNameSrc, EFileShareReadersOnly);
	TInt	theSize;
	theFile.Size(theSize);
	
	TBuf8<KSizeBuf>	theBuf;
	TInt	theStep = KSizeBuf;
	if(theStep > theSize)
		{
		theStep = theSize;
		}
	
	TInt theRes = theFileNew.Create(fs, aFileNameDst, EFileShareAny);
	if(theRes == KErrAlreadyExists)
		{
		theRes = theFileNew.Replace(fs, aFileNameDst, EFileShareAny);
		}
		

	if(theRes != KErrNone)
		{
		return theRes;
		}
		

	TInt	theCurLength = 0;
	while(theCurLength < theSize)
		{
		theFile.Read(theBuf, theStep);
		theFileNew.Write(theBuf, theStep);
		theCurLength += theStep;
		
		if(theCurLength + theStep > theSize )
			{
			theStep = theSize - theCurLength;
			}
		}

	theFileNew.Close();
	theFile.Close();
	fs.Close();

	return theErr;
}

/**
 *
 * Fsm()
 * Executes events of DevSound in sequence
 *
 */
void RTestStepDevSoundPlayBase::Fsm(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
            {
            INFO_PRINTF1(_L("Initializing DevSound..."));

            if (iTestType == EUidTest)
                {
                // Initializing DevSound object for the mode for processing audio data, with hardware device UID
                INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
                TRAPD(err, iMMFDevSound->InitializeL(*this, iDecoderHwDevUid, EMMFStatePlaying));
                iCanSchedulerStart = EFalse;
                if (err == KErrNotSupported)
                    {
                    INFO_PRINTF1(_L("Initialization of DevSound failed for Uid with 'KErrNotSupported'. Test PASS!"));
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
                TRAPD(err, iMMFDevSound->InitializeL(*this, iDecoderHwDevUid, EMMFStatePlaying));
                iCanSchedulerStart = EFalse;
                #ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
                if (err == KErrNotSupported)
                    {
                    INFO_PRINTF1(_L("Initialization of DevSound failed for invalid Uid with 'KErrNotSupported'. Test PASS!"));
                    iTestStepResult = EPass;
                    }
                #else
                if (err == KErrNotFound)
                    {
                    INFO_PRINTF1(_L("Initialization of DevSound failed for invalid Uid with 'KErrNotFound'. Test PASS!"));
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
                // Initializing DevSound object for the mode for processing audio data, with invalid TFourCC code
                INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
                TRAPD(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
                iCanSchedulerStart = EFalse;
                #ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
                if (err == KErrNotSupported)
                    {
                    INFO_PRINTF1(_L("Initialization of DevSound failed for invalid input data type with 'KErrNotSupported'. Test PASS!"));
                    iTestStepResult = EPass;
                    }
                #else
                if (err == KErrNotFound)
                    {
                    INFO_PRINTF1(_L("Initialization of DevSound failed for invalid input data type with 'KErrNotFound'. Test PASS!"));
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
                // Initializing DevSound object for the mode for processing audio data, with TFourCC code
                INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
                TRAPD(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
                if (err != KErrNone)
                    {
                    iCanSchedulerStart = EFalse;
                    ERR_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
                    Fsm(EDevSoundError);
                    }
                break;
                }
            }

        case EDevSoundPlayInit:
            {
            // Initializing the audio device and starting the play process
            INFO_PRINTF1(_L("iMMFDevSound->PlayInitL()"));
            TRAPD(err, iMMFDevSound->PlayInitL());// Get buffer from DevSound
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("DevSound PlayInitL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            break;
            }

        case EDevSoundPlayData:
            {
            // Fill the buffer with audio data and play the data in the buffer
            // Read sizeof buffer from file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            TInt err = iFile.Read(buffer->Data());
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR : iFile.Read() failed with %d"), err);
                Fsm(EDevSoundError);
                }
            
            if (buffer->Data().Length() != buffer->RequestSize())
                {
                INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
                iBuffer->SetLastBuffer(ETrue);
                iLastBuffer = ETrue;
                }

            if (iTestType == EBitRateTest)
                {
                // Set bit-rate
                MMMFDevSoundCustomInterfaceBitRate* bitrate = static_cast <MMMFDevSoundCustomInterfaceBitRate*>(iMMFDevSound->CustomInterface(KUidCustomInterfaceDevSoundBitRate));
                if (bitrate)
                    {
                    ERR_PRINTF1(_L("Succeeded in getting the CustomInterface for bitrate, should have returned NULL..."));
                    Fsm(EDevSoundError);
                    break;
                    }
                else
                    {
                    INFO_PRINTF1(_L("Failed while getting the CustomInterface for bitrate, returned NULL..."));
                    }
                }

            if (iTestType == EPositionTest)
                {
                // Get Position of the current data
                TInt64 position = iMMFDevSound->SamplesPlayed();
                position = position * KMicroSecsInOneSec / KSampleRate8000Hz;
                INFO_PRINTF2(_L("Current position = %d. "), position);
                }

            // Playing data in the buffer at the current volume
            INFO_PRINTF1(_L("iMMFDevSound->PlayData()"));
            iMMFDevSound->PlayData();
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
 * Test Preamble routines.
 *
 */
TVerdict RTestStepDevSoundPlayBase::DoTestStepPreambleL()
    {
    //Install the Active Scheduler
    iActiveScheduler = new (ELeave) CActiveScheduler;
    CActiveScheduler::Install (iActiveScheduler);

    INFO_PRINTF1(_L("-- Creating DevSound object..."));
    TRAPD(err,  iMMFDevSound = CMMFDevSound::NewL());
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not create DevSound object. Error = %d"), err);
        return EInconclusive;
        }

    //Get the filename of the audio file to play
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
    
    // open using RFile for playback
    err = iFile.Open(iFs, iFilename, EFileRead);
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
        iFs.Close();
        return EInconclusive;
        }
    
    // display trace about filename and codec under test
    INFO_PRINTF2(_L("File under test  -> %S"), &iFilename);
    TUint32 fourCC = iInputDataType.FourCC();
    INFO_PRINTF5(_L("Codec under test -> %c%c%c%c"), fourCC, fourCC>>8, fourCC>>16, fourCC>>24);
    
    return EPass;
    }

/**
 *
 * Test Postamble routines.
 *
 */
TVerdict RTestStepDevSoundPlayBase::DoTestStepPostambleL()
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
void RTestStepDevSoundPlayBase::InitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);
    
    if (aError != KErrNone)
        {
        Fsm(EDevSoundError);
        return;
        }

     // Call to initialize the audio device and start the play process
     Fsm(EDevSoundPlayInit);
    }

/**
 *
 * ToneFinished()
 *
 */
void RTestStepDevSoundPlayBase::ToneFinished(TInt /*aError*/)
    {
    }

/**
 *
 * BufferToBeFilled()
 *
 */
void RTestStepDevSoundPlayBase::BufferToBeFilled(CMMFBuffer* aBuffer)
    {
    INFO_PRINTF1(_L("DevSound called BufferToBeFilled."));
    
    if (!aBuffer)
        {
        ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
        Fsm(EDevSoundError);        
        return;
        }

    iBuffer = aBuffer;    

    // Call to initialize the audio device and start the play process
    Fsm(EDevSoundPlayData);
    }

/**
 *
 * PlayError()
 *
 */
void RTestStepDevSoundPlayBase::PlayError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called PlayError with error = %d"), aError);
    
    if ((iLastBuffer) && (aError == KErrUnderflow))
        {
        iTestStepResult = EPass;
        }

    Fsm(EDevSoundError);
    }

/**
 *
 * BufferToBeEmptied()
 *
 */
void RTestStepDevSoundPlayBase::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
    {
    }

/**
 *
 * RecordError()
 *
 */
void RTestStepDevSoundPlayBase::RecordError(TInt /*aError*/)
    {
    }

/**
 *
 * ConvertError
 *
 */
void RTestStepDevSoundPlayBase::ConvertError(TInt /*aError*/)
    {
    }

/**
 *
 * DeviceMessage
 *
 */
void RTestStepDevSoundPlayBase::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
    {
    }

/**
 *
 * Finished()
 *
 */
void RTestStepDevSoundPlayBase::Finished()
    {
    INFO_PRINTF1(_L("RTestStepDevSoundPlayBase::Finished"));
    //overridden in play/stop and play/pause tests.
    }

//
// RTestStepDevSoundPlayDatatype
//

/**
 *
 * RTestStepDevSoundPlayDatatype - Test step constructor
 *
 */
RTestStepDevSoundPlayDatatype::RTestStepDevSoundPlayDatatype(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    : RTestStepDevSoundPlayBase(aTestName, aSectName, aKeyName, aInputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundPlayDatatype* RTestStepDevSoundPlayDatatype::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    {
    RTestStepDevSoundPlayDatatype* self = new (ELeave) RTestStepDevSoundPlayDatatype(aTestName, aSectName, aKeyName, aInputDataType);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundPlayDatatype::DoTestStepL()
    {
    iTestStepResult = EFail; // Assume test will fail

    if (iTestStepName.Compare(_L("MM-MMF-DEVSOUND-I-0501-HP")) == 0)
        {
        // Test: MM-MMF-DEVSOUND-I-0501-HP
        INFO_PRINTF1(_L("<<< Play an Ogg vorbis audio file specifying invalid input data type >>>"));
        iTestType = EInvDataTypeTest;
        }
    else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUND-I-0001-HP")) == 0)
        {
        // Test: MM-MMF-DEVSOUND-I-0001-HP
        
        // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
		INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
		return EInconclusive;
#else
        INFO_PRINTF1(_L("<<< Play an OggVorbis audio file >>>"));
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND
        }

    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
    }

//
// RTestStepDevSoundPlayHwDevUid
//

/**
 *
 * RTestStepDevSoundPlayHwDevUid - Test step constructor
 *
 */
RTestStepDevSoundPlayHwDevUid::RTestStepDevSoundPlayHwDevUid(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    : RTestStepDevSoundPlayBase(aTestName, aSectName, aKeyName)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundPlayHwDevUid* RTestStepDevSoundPlayHwDevUid::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
    {
    RTestStepDevSoundPlayHwDevUid* self = new (ELeave) RTestStepDevSoundPlayHwDevUid(aTestName, aSectName, aKeyName);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundPlayHwDevUid::DoTestStepL()
    {
    iTestStepResult = EFail; // Assume test will fail

    if (iTestStepName.Compare(_L("MM-MMF-DEVSOUND-I-0502-HP")) == 0)
        {
        // Test:MM-MM-MMF-DEVSOUND-I-0502-HP
        INFO_PRINTF1(_L("<<< Play an Ogg vorbis audio file specifying invalid Uid >>>"));

        iTestType = EInvUidTest; // Test uses invalid Uid for initialization
        iDecoderHwDevUid.iUid = 0x10273838; // Invalid Uid
        }
    else if (iTestStepName.Compare(_L("MM-MMF-DEVSOUND-I-0503-HP")) == 0)
        {
        // Test: MM-MMF-DEVSOUND-I-0503-HP
        INFO_PRINTF1(_L("<<< Play an OggVorbis audio file specifying Uid of HwDevice >>>"));

        iTestType = EUidTest; // Test uses Uid for initialization (earlier - KMmfUidHwDeviceMulawToPCM16)
        iDecoderHwDevUid.iUid = KUidTestVorbisDecoderPU;//KUidHwDeviceAudioAdapter
        }
    
    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
    }

//
// RTestStepDevSoundPlayConfHwDev
//

/**
 *
 * 
 * RTestStepDevSoundPlayConfHwDev - Test step constructor
 *
 */
RTestStepDevSoundPlayConfHwDev::RTestStepDevSoundPlayConfHwDev(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    : RTestStepDevSoundPlayBase(aTestName, aSectName, aKeyName, aInputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundPlayConfHwDev* RTestStepDevSoundPlayConfHwDev::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    {
    RTestStepDevSoundPlayConfHwDev* self = new (ELeave) RTestStepDevSoundPlayConfHwDev(aTestName, aSectName, aKeyName, aInputDataType);
    return self;
    }

/**
 *
 * Fsm()
 * Executes events of DevSound in sequence
 *
 */
void RTestStepDevSoundPlayConfHwDev::Fsm(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
            {
            // Initializing DevSound object for the mode for processing audio data, with TFourCC code
            INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
            TRAPD(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
            if (err != KErrNone)
                {
                iCanSchedulerStart = EFalse;
                ERR_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            break;
            }

        case EDevSoundPlayInit:
            {
            // Seek to the place we want to start reading from 
            TInt filePosition = 0;
            TInt err = iFile.Seek(ESeekStart, filePosition);
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("iFile.Seek() returned %d"), err);
                Fsm(EDevSoundError);
                }

            // Initializing the audio device and starting the play process
            INFO_PRINTF1(_L("iMMFDevSound->PlayInitL()"));
            TRAP(err, iMMFDevSound->PlayInitL());// Get buffer from DevSound
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("DevSound PlayInitL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            break;
            }

        case EDevSoundPlayData:
            {
            // Fill the buffer with audio data and play the data in the buffer
            // Read sizeof buffer from file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            TInt err = iFile.Read(buffer->Data());
            if (buffer->Data().Length() != buffer->RequestSize())
                {
                INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
                iBuffer->SetLastBuffer(ETrue);
                iLastBuffer = ETrue;
                }

            // Playing data in the buffer at the current volume
            INFO_PRINTF1(_L("iMMFDevSound->PlayData()"));
            iMMFDevSound->PlayData();
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
 * InitializeComplete()
 *
 */
void RTestStepDevSoundPlayConfHwDev::InitializeComplete(TInt aError)
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
        capabilities.iRate = EMMFSampleRate16000Hz;
        capabilities.iEncoding = EMMFSoundEncoding16BitPCM;
        capabilities.iChannels = EMMFStereo;

        INFO_PRINTF1(_L("Configuring DevSound object with the settings for sampling rate, encoding and stereo..."));
        TRAPD(err, iMMFDevSound->SetConfigL(capabilities));
        if (err != KErrNone)
            {
            ERR_PRINTF2(_L("Configuration of DevSound object failed! Left with error = %d"), err);
            Fsm(EDevSoundError);        
            }

        // Call to initialize the audio device and start the play process
        Fsm(EDevSoundPlayInit);
        }
    }

/**
 *
 * BufferToBeFilled()
 *
 */
void RTestStepDevSoundPlayConfHwDev::BufferToBeFilled(CMMFBuffer* aBuffer)
    {
    INFO_PRINTF1(_L("DevSound called BufferToBeFilled."));

    if (!aBuffer)
        {
        ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
        Fsm(EDevSoundError);        
        return;
        }

    iBuffer = aBuffer;

    // Call to initialize the audio device and start the play process
    Fsm(EDevSoundPlayData);
    }

/**
 *
 * PlayError()
 *
 */
void RTestStepDevSoundPlayConfHwDev::PlayError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called PlayError with error = %d"), aError);

    if ((iLastBuffer) && (aError == KErrUnderflow))
        {
        if (iTestType == EReconfigureTest)
            {
            // Call to initialize the audio device and start the play process
            iLastBuffer = EFalse;
            iTestType = ENormalTest;
            
            // reconfiguring (for play the configuration values should be the same, as the file being played)
            TMMFCapabilities capabilities;
            capabilities.iRate = EMMFSampleRate16000Hz;
            capabilities.iEncoding = EMMFSoundEncoding16BitPCM;
            capabilities.iChannels = EMMFStereo;

            INFO_PRINTF1(_L("Reconfiguring DevSound object with the settings for sampling rate, encoding and stereo..."));
            TRAPD(err, iMMFDevSound->SetConfigL(capabilities));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("Reconfiguration of DevSound object failed! Left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            Fsm(EDevSoundPlayInit);
            }
        else
            {
            iTestStepResult = EPass;
            Fsm(EDevSoundError);
            }
        }
    else
        {
        Fsm(EDevSoundError);
        }
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundPlayConfHwDev::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0002-HP
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
    INFO_PRINTF1(_L("<<< This test is reenabled as of Def125498. \"VORB\" is used as FourCC instead of \"VRB2\" >>>"));
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND
    INFO_PRINTF1(_L("<<< Configure the audio HwDevice and play an OggVorbis audio file >>>"));
    iTestStepResult = EFail;
    iTestType = EReconfigureTest;

    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
    }

//
// RTestStepDevSoundListInputDataTypes
//

/**
 * 
 * RTestStepDevSoundListInputDataTypes - Test step constructor
 *
 */
RTestStepDevSoundListInputDataTypes::RTestStepDevSoundListInputDataTypes(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    : RTestStepDevSoundPlayBase(aTestName, aSectName, aKeyName, aInputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundListInputDataTypes* RTestStepDevSoundListInputDataTypes::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    {
    RTestStepDevSoundListInputDataTypes* self = new (ELeave) RTestStepDevSoundListInputDataTypes(aTestName, aSectName, aKeyName, aInputDataType);
    return self;
    }

/**
 *
 * InitializeComplete()
 *
 */
void RTestStepDevSoundListInputDataTypes::InitializeComplete(TInt aError)
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
TVerdict RTestStepDevSoundListInputDataTypes::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0003-HP
    INFO_PRINTF1(_L("<<< List of supported input data types >>>"));
    iTestStepResult = EPass;

    TUint numberOfSupportedDataTypes = 0;

    TMMFPrioritySettings prioritySettings;
    prioritySettings.iPref = EMdaPriorityPreferenceNone;
    prioritySettings.iPriority = 0;
    prioritySettings.iState = EMMFStatePlaying;

    RArray<TFourCC> supportedDataTypes;
    CleanupClosePushL(supportedDataTypes);
    INFO_PRINTF1(_L("iMMFDevSound->GetSupportedInputDataTypesL()"));
    TRAPD(err, iMMFDevSound->GetSupportedInputDataTypesL(supportedDataTypes, prioritySettings));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Error in getting supported input data types = %d"), err);
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
                if (err != KErrNone)
                    {
                    ERR_PRINTF5(_L("DevSound failed to support input data type FourCC = %c%c%c%c"), fourCC, fourCC>>8, fourCC>>16, fourCC>>24);
                    ERR_PRINTF2(_L("Left with error = %d"), err);
                    iTestStepResult = EFail;
                    }
                else
                    {
                    StartScheduler();
                    INFO_PRINTF5(_L("DevSound supports input data type FourCC =  %c%c%c%c"), fourCC, fourCC>>8, fourCC>>16, fourCC>>24);
                    }
                }
            }
        else
            {
            ERR_PRINTF1(_L("No supported datatypes detected for playing."));
            iTestStepResult = EFail;
            }
        }
    CleanupStack::PopAndDestroy(); // supportedDataTypes
    return iTestStepResult;
    }
 
//
// RTestStepDevSoundPositionData
//
  
/**
 * 
 * RTestStepDevSoundPositionData - Test step constructor
 *
 */
RTestStepDevSoundPositionData::RTestStepDevSoundPositionData(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    : RTestStepDevSoundPlayBase(aTestName, aSectName, aKeyName, aInputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundPositionData* RTestStepDevSoundPositionData::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    {
    RTestStepDevSoundPositionData* self = new (ELeave) RTestStepDevSoundPositionData(aTestName, aSectName, aKeyName, aInputDataType);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundPositionData::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0004-HP
    
   	// This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
	INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
	return EInconclusive;
#else    
    INFO_PRINTF1(_L("<<< Position in stream data required for variable bit rate audio codec >>>"));
    iTestStepResult = EFail;
    iTestType = EPositionTest;

    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once
   
    return iTestStepResult;
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
    }

//
// RTestStepDevSoundPlayAlloc
//

/**
 *
 * RTestStepDevSoundPlayAlloc - Test step constructor
 *
 */
RTestStepDevSoundPlayAlloc::RTestStepDevSoundPlayAlloc(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    : RTestStepDevSoundPlayBase(aTestName, aSectName, aKeyName, aInputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundPlayAlloc* RTestStepDevSoundPlayAlloc::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    {
    RTestStepDevSoundPlayAlloc* self = new (ELeave) RTestStepDevSoundPlayAlloc(aTestName, aSectName, aKeyName, aInputDataType);
    return self;
    }

/**
 *
 * Fsm()
 * Executes events of DevSound in sequence
 *
 */
void RTestStepDevSoundPlayAlloc::FsmL(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
            {
            // Initializing DevSound object for the mode for processing audio data, with TFourCC code
            INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
            TRAPD(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
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
        case EDevSoundPlayInit:
            {
            // Seek to the place we want to start reading from 
            TInt filePosition = 0;
            TInt err = iFile.Seek(ESeekStart, filePosition);
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("iFile.Seek() failed with %d"), err);
                StopScheduler();
                User::Leave(err);
                }
            
            // Initializing the audio device and starting the play process
            INFO_PRINTF1(_L("iMMFDevSound->PlayInitL()"));
            TRAP(err, iMMFDevSound->PlayInitL());// Get buffer from DevSound
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("DevSound PlayInitL left with error = %d"), err);
                StopScheduler();
                User::Leave(err);
                }
            break;
            }
        case EDevSoundPlayData:
            {
            // Fill the buffer with audio data and play the data in the buffer
            // Read sizeof buffer from file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            iFile.Read(buffer->Data());
            if (buffer->Data().Length() != buffer->RequestSize())
                {
                INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
                iBuffer->SetLastBuffer(ETrue);
                iLastBuffer = ETrue;
                }
            // Playing data in the buffer at the current volume
            INFO_PRINTF1(_L("iMMFDevSound->PlayData()"));
            iMMFDevSound->PlayData();
            break;
            }
        default:
            {
            INFO_PRINTF1(_L("Invalid DevSound event!"));
            break;
            }
        }
    }

/**
 *
 * Play audio by marking heap.
 *
 */
void RTestStepDevSoundPlayAlloc::AudioPlayAllocL()
    {
    __MM_HEAP_MARK;
    
    // create new devsound instance
	ASSERT(iMMFDevSound == NULL);
    TRAPD(err,  iMMFDevSound = CMMFDevSound::NewL());
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not create DevSound object. Error = %d"), err);
        User::Leave(err);
        }
    
    // call the state handler and start the test
    iError = KErrNone;
    FsmL(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    // cleanup devsound instance
    delete iMMFDevSound;
    iMMFDevSound = NULL;
    
    // check for errors
    if ((iError != KErrNone) && (iError != KErrUnderflow))
        {
        ERR_PRINTF2(_L("Opening/playing the audio file failed, unexpected error : %d."), iError);
        User::Leave(iError);
        }
    
    __MM_HEAP_MARKEND;
    }

/**
 *
 * DoTestStepL()
 *
 */
TVerdict RTestStepDevSoundPlayAlloc::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0005-HP
    
    // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
	INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
	return EInconclusive;
#else    
    // Delete the instance created by the PreambleL, AudioPlayAllocL will recreate it.
    if (iMMFDevSound)
    	{
    	INFO_PRINTF1(_L(" - Deleting existing instance of DevSound before alloc test"));
    	delete iMMFDevSound;
    	iMMFDevSound = NULL;
    	}
        
    INFO_PRINTF1(_L("<<< Alloc test to load an audio HwDevice to play an OggVorbis audio file >>>"));
    iTestStepResult = EFail;
    
    TRAPD(err, AudioPlayAllocL());
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Opening/playing the audio file failed, unexpected error %d."), err);
        return iTestStepResult;
        }

    iTestStepResult = EFail;

    TInt failCount = 1;
    TBool completed = EFalse;
    TBool badResult = EFalse;
    TBool reachedEnd = EFalse;

    for(;;)
        {
        __UHEAP_SETFAIL(RHeap::EFailNext, failCount);
        __MM_HEAP_MARK;

        TInt err = KErrNone;
        TRAP(err, AudioPlayAllocL());
        
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
void RTestStepDevSoundPlayAlloc::InitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);
    
    iError = aError;
    if (aError != KErrNone)
        {
        StopScheduler();
        }
    else
        {
        // Call to initialize the audio device and start the play process
         TRAP(iError, FsmL(EDevSoundPlayInit));
        }
    }

/**
 *
 * BufferToBeFilled()
 *
 */
void RTestStepDevSoundPlayAlloc::BufferToBeFilled(CMMFBuffer* aBuffer)
    {
    INFO_PRINTF1(_L("DevSound called BufferToBeFilled."));
    
    if (!aBuffer)
        {
        ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
        StopScheduler();
        iError = KErrNotFound;
        }
    else
        {
        iBuffer = aBuffer;
        // Call to initialize the audio device and start the play process
        TRAP(iError, FsmL(EDevSoundPlayData));
        }
    }

/**
 *
 * PlayError()
 *
 */
void RTestStepDevSoundPlayAlloc::PlayError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called PlayError with error = %d"), aError);
    
    StopScheduler();
    iError = aError;
    
    if ((iLastBuffer) && (aError == KErrUnderflow))
        {
        INFO_PRINTF1(_L("Last Buffer is set, so test PASS"));
        iTestStepResult = EPass;
        }
    else
        {
        ERR_PRINTF1(_L("Last Buffer is NOT set, so test FAIL"));
        iTestStepResult = EFail;
        }
    }

//
// RTestStepDevSoundPlayStop
//

/**
 *
 * RTestStepDevSoundPlayStop - Test step constructor
 *
 */
RTestStepDevSoundPlayStop::RTestStepDevSoundPlayStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    : RTestStepDevSoundPlayBase(aTestName, aSectName, aKeyName, aInputDataType)
    {
    iPlayTimer = NULL;
    iStopCount = 0;
    iRestarted = EFalse;
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundPlayStop* RTestStepDevSoundPlayStop::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    {
    RTestStepDevSoundPlayStop* self = new (ELeave) RTestStepDevSoundPlayStop(aTestName, aSectName, aKeyName, aInputDataType);
    return self;
    }


/**
 * set finished on callback from a timer
 */
void RTestStepDevSoundPlayStop::Finished()
    {
    iStopCount++;
    
    INFO_PRINTF1(_L("-- iMMFDevSound->Stop()"));
    iMMFDevSound->Stop();
    iRestarted = EFalse;
    Fsm(EDevSoundPlayStopped);
    }

/**
 *
 * Fsm()
 * Executes events of DevSound in sequence
 *
 */
void RTestStepDevSoundPlayStop::Fsm(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
            {
            INFO_PRINTF1(_L("Initializing DevSound..."));

            // Initializing DevSound object for the mode for processing audio data, either TFourCC code
            INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
            TInt err;
            TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
            if (err != KErrNone)
                {
                iCanSchedulerStart = EFalse;
                ERR_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
                
            // configuring
            TMMFCapabilities capabilities;
            capabilities.iRate = EMMFSampleRate44100Hz;
            capabilities.iEncoding = EMMFSoundEncoding16BitPCM;
            capabilities.iChannels = EMMFStereo;

            INFO_PRINTF1(_L("Configuring DevSound object with the settings for sampling rate, encoding and stereo..."));
            TRAP(err, iMMFDevSound->SetConfigL(capabilities));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("Configuration of DevSound object failed! Left with error = %d"), err);
                Fsm(EDevSoundError);
                }
                
            break;
            }

        case EDevSoundPlayInit:
            {
            // Initializing the audio device and starting the play process
            INFO_PRINTF1(_L("iMMFDevSound->PlayInitL()"));
            TRAPD(err, iMMFDevSound->PlayInitL());// Get buffer from DevSound
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("DevSound PlayInitL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            break;
            }

        case EDevSoundPlayData:
            {
            if(!iRestarted)
                {
                // initialise and start the timer.  the file is short and so decode occur rapidly
                // hence the short time passed to After()
                TRAPD(err, iPlayTimer = CMmfDevSoundPlayTimer::NewL(this, 500));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("CMmfDevSoundPlayTimer::NewL left with error = %d"), err);
                    Fsm(EDevSoundError);
                    break;
                    }
                iPlayTimer->Start();
                iRestarted = ETrue;
                }
            
            // Fill the buffer with audio data and play the data in the buffer
            // Read sizeof buffer from file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            TInt err = iFile.Read(buffer->Data());
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("iFile.Read returned the error %d"), err);
                iPlayTimer->Cancel();
                delete iPlayTimer;
                iPlayTimer = NULL;
                Fsm(EDevSoundError);
                break;
                }
            if (buffer->Data().Length() != buffer->RequestSize())
                {
                INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
                iBuffer->SetLastBuffer(ETrue);
                iLastBuffer = ETrue;
                }
           
            // Playing data in the buffer at the current volume
            iMMFDevSound->PlayData();
            iBufferCount++; 
            break;
            }
        
        case EDevSoundPlayStopped:
            {
            if(!iRestarted)
                {
                INFO_PRINTF1(_L("-- Restarting..."));
                iRestarted = ETrue;
                Fsm(EDevSoundPlayInit);
                break;
                }
            iPlayTimer->Cancel();
            delete iPlayTimer;
            iPlayTimer = NULL;
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
 * InitializeComplete()
 *
 */
void RTestStepDevSoundPlayStop::InitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);

    INFO_PRINTF1(_L("iMMFDevSound->Capabilities()"));
    iMMFDevSound->Capabilities();
	TMMFCapabilities capabilities;
        capabilities.iRate = EMMFSampleRate44100Hz;
        capabilities.iChannels = EMMFStereo;
        capabilities.iBufferSize = 4096;
    
    TRAPD(err, iMMFDevSound->SetConfigL( capabilities));// Get buffer from DevSound
    TRAP(err, iMMFDevSound->MaxVolume());// Get buffer from DevSound
    TRAP(err, iMMFDevSound->MaxGain());// Get buffer from DevSound
    TRAP(err, iMMFDevSound->SetVolume(5));// Get buffer from DevSound
    
    if (aError != KErrNone)
        {
        Fsm(EDevSoundError);
        return;
        }

    // Call to initialize the audio device and start the play process
    Fsm(EDevSoundPlayInit);
    }

/**
 *
 * BufferToBeFilled()
 *
 */
void RTestStepDevSoundPlayStop::BufferToBeFilled(CMMFBuffer* aBuffer)
    {
    INFO_PRINTF1(_L("DevSound called BufferToBeFilled."));

    if (!aBuffer)
        {
        ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
        Fsm(EDevSoundError);        
        return;
        }

    iBuffer = aBuffer;
    
    // Call to initialize the audio device and start the play process
    Fsm(EDevSoundPlayData);
    }

/**
 *
 * PlayError()
 *
 */
void RTestStepDevSoundPlayStop::PlayError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called PlayError with error = %d"), aError);

    if ((iLastBuffer) && (aError == KErrUnderflow))
        {
        if (iStopCount > 0)
            {
            INFO_PRINTF2(_L("Stop has been called %d time(s)"), iStopCount);
            iTestStepResult = EPass;
            }
        else
            {
            ERR_PRINTF1(_L("Stop has been NOT been called even once"));
            iTestStepResult = EFail;
            }
        }
    // delete the iPlayTimer
    iPlayTimer->Cancel();
    delete iPlayTimer;
    iPlayTimer = NULL;
    Fsm(EDevSoundError);
    }

/**
 *
 * DoTestStepL()
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundPlayStop::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0006-HP
    
    // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
	INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
	return EInconclusive;
#else    
    INFO_PRINTF1(_L("<<< Play/stop playing an OggVorbis audio file >>>"));
    iTestStepResult = EFail;
    
    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
    }

//
// RTestStepDevSoundPlayPause
//

/**
 *
 * RTestStepDevSoundPlayPause - Test step constructor
 *
 */
RTestStepDevSoundPlayPause::RTestStepDevSoundPlayPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    : RTestStepDevSoundPlayBase(aTestName, aSectName, aKeyName, aInputDataType)
    {
    iPlayTimer = NULL;
    iPauseCount = 0;
    iRestarted = EFalse;
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundPlayPause* RTestStepDevSoundPlayPause::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    {
    RTestStepDevSoundPlayPause* self = new (ELeave) RTestStepDevSoundPlayPause(aTestName, aSectName, aKeyName, aInputDataType);
    return self;
    }

/**
 * set finished on callback from a timer
 */
void RTestStepDevSoundPlayPause::Finished()
    {
    iPauseCount++;
    
    INFO_PRINTF1(_L("-- iMMFDevSound->Pause();"));
    iMMFDevSound->Pause();
    iRestarted = EFalse;
    Fsm(EDevSoundPlayPaused);
    }

/**
 *
 * Fsm()
 * Executes events of DevSound in sequence
 *
 */
void RTestStepDevSoundPlayPause::Fsm(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
            {
            // Initializing DevSound object for the mode for processing audio data, with TFourCC code
            INFO_PRINTF1(_L("iMMFDevSound->InitializeL()"));
            TInt err;
            TRAP(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
            if (err != KErrNone)
                {
                iCanSchedulerStart = EFalse;
                ERR_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
                
            // configuring
            TMMFCapabilities capabilities;
            capabilities.iRate = EMMFSampleRate44100Hz;
            capabilities.iEncoding = EMMFSoundEncoding16BitPCM;
            capabilities.iChannels = EMMFStereo;

            INFO_PRINTF1(_L("Configuring DevSound object with the settings for sampling rate, encoding and stereo..."));
            TRAP(err, iMMFDevSound->SetConfigL(capabilities));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("Configuration of DevSound object failed! Left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            
            break;
            }

        case EDevSoundPlayInit:
            {
            // Initializing the audio device and starting the play process
            INFO_PRINTF1(_L("iMMFDevSound->PlayInitL()"));
            TRAPD(err, iMMFDevSound->PlayInitL());// Get buffer from DevSound
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("DevSound PlayInitL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            break;
            }

        case EDevSoundPlayData:
            {
            if(!iRestarted)
                {
                // initialise and start the timer.  the file is short and so decode occur rapidly
                // hence the short time passed to After()                
                TRAPD(err, iPlayTimer = CMmfDevSoundPlayTimer::NewL(this, 500));//500
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("CMmfDevSoundPlayTimer::NewL left with error = %d"), err);
                    Fsm(EDevSoundError);
                    break;
                    }
                iPlayTimer->Start();
                iRestarted = ETrue;
                }
            
            // Fill the buffer with audio data and play the data in the buffer
            // Read sizeof buffer from file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            TInt err = iFile.Read(buffer->Data());
			if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("CMmfDevSoundPlayTimer::NewL left with error = %d"), err);
                    iPlayTimer->Cancel();
                    delete iPlayTimer;
                    iPlayTimer = NULL;
                    Fsm(EDevSoundError);
                    break;
                    }
            if (buffer->Data().Length() != buffer->RequestSize())
                {
                INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
                iBuffer->SetLastBuffer(ETrue);
                iLastBuffer = ETrue;
                }

            // Playing data in the buffer at the current volume.
            iMMFDevSound->PlayData();
            iBufferCount++;
            break;
            }

        case EDevSoundPlayPaused:
            {
            if(!iRestarted)
                {
                INFO_PRINTF1(_L("-- Restarting..."));
                iRestarted = ETrue;
                Fsm(EDevSoundPlayInit);
                break;
                }
            iPlayTimer->Cancel();
            delete iPlayTimer;
            iPlayTimer = NULL;
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
 * InitializeComplete()
 *
 */
void RTestStepDevSoundPlayPause::InitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);
    
        INFO_PRINTF1(_L("iMMFDevSound->Capabilities()"));
    iMMFDevSound->Capabilities();
	TMMFCapabilities capabilities;
        capabilities.iRate = EMMFSampleRate44100Hz;
        capabilities.iChannels = EMMFStereo;
        capabilities.iBufferSize = 4096;
        
    TRAPD(err, iMMFDevSound->SetConfigL( capabilities));// Get buffer from DevSound
    TRAP(err, iMMFDevSound->MaxVolume());// Get buffer from DevSound
    TRAP(err, iMMFDevSound->MaxGain());// Get buffer from DevSound
    TRAP(err, iMMFDevSound->SetVolume(5));// Get buffer from DevSound
    
    if (aError != KErrNone)
        {        
        Fsm(EDevSoundError);        
        return;
        }
    
    // Call to initialize the audio device and start the play process
    Fsm(EDevSoundPlayInit);
    }

/**
 *
 * BufferToBeFilled()
 *
 */
void RTestStepDevSoundPlayPause::BufferToBeFilled(CMMFBuffer* aBuffer)
    {
    INFO_PRINTF1(_L("DevSound called BufferToBeFilled."));
    
    if (!aBuffer)
        {
        ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
        Fsm(EDevSoundError);
        return;
        }

    iBuffer = aBuffer;    

    // Call to initialize the audio device and start the play process
    Fsm(EDevSoundPlayData);
    }

/**
 *
 * PlayError()
 *
 */
void RTestStepDevSoundPlayPause::PlayError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called PlayError with error = %d"), aError);
        
    if ((iLastBuffer) && (aError == KErrUnderflow))
        {
        if (iPauseCount > 0)
            {
            INFO_PRINTF2(_L("Pause has been called %d time(s)"), iPauseCount);
            iTestStepResult = EPass;
            }
        else
            {
            ERR_PRINTF1(_L("Pause has been NOT been called even once"));
            iTestStepResult = EFail;
            }
        }
    // delete the iPlayTimer
    iPlayTimer->Cancel();
    delete iPlayTimer;
    iPlayTimer = NULL;
    Fsm(EDevSoundError);
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundPlayPause::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0007-HP
    INFO_PRINTF1(_L("<<< Play/pause playing an OggVorbis audio file >>>"));
    iTestStepResult = EFail;

    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
    }

//
// RTestStepDevSoundPlayRecord
//

/**
 *
 * RTestStepDevSoundPlayRecord - Test step constructor
 *
 */
RTestStepDevSoundPlayRecord::RTestStepDevSoundPlayRecord(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aKeyNameOutput, const TFourCC& aInputDataType)
    : RTestStepDevSoundPlayBase(aTestName, aSectName, aKeyName, aInputDataType)
    {
    iKeyNameOutput = aKeyNameOutput;
    iPlayTimer = NULL;
    iTestState = 0;
    iRetryTestState0 = 1; // number of times to retry
    iRunNextTestState = ETrue;
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundPlayRecord* RTestStepDevSoundPlayRecord::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aKeyNameOutput, const TFourCC& aInputDataType)
    {
    RTestStepDevSoundPlayRecord* self = new (ELeave) RTestStepDevSoundPlayRecord(aTestName, aSectName, aKeyName, aKeyNameOutput, aInputDataType);
    return self;
    }

/**
 * set finished on callback from a timer
 */
void RTestStepDevSoundPlayRecord::Finished()
    {
    iTestState++;
    
    // check the current state to display the trace
    if (iTestState == 1)
        {
        INFO_PRINTF1(_L("-- Playback stopped after .05 seconds, to start record..."));
        }
    else if (iTestState == 2)
        {
        INFO_PRINTF1(_L("-- Record stopped after 10 seconds, to start play again..."));
        }
    
    // stop devsound & delete the iPlayTimer
    iMMFDevSound->Stop();
    iPlayTimer->Cancel();
    delete iPlayTimer;
    iPlayTimer = NULL;
    Fsm(EDevSoundError);
    }
    
/**
 *
 * Fsm()
 * Executes events of DevSound in sequence
 *
 */
void RTestStepDevSoundPlayRecord::Fsm(TMmfDevSoundEvents aDevSoundEvent)
    {
    switch (aDevSoundEvent)
        {
        case EDevSoundInit:
            {
            if (iTestState == 1)
                {
                // Attempt to use VORB type instead of KMMFFourCCCodePCM16
                INFO_PRINTF1(_L("iMMFDevSound->InitializeL() for Recording"));
                TRAPD(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStateRecording));
                if (err != KErrNone)
                    {
                    iCanSchedulerStart = EFalse;
                    ERR_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
                    Fsm(EDevSoundError);
                    }
                }
            else
                {
                // Initializing DevSound object for the mode for processing audio data, with TFourCC code
                INFO_PRINTF1(_L("iMMFDevSound->InitializeL() for Playing"));
                TRAPD(err, iMMFDevSound->InitializeL(*this, iInputDataType, EMMFStatePlaying));
                if (err != KErrNone)
                    {
                    iCanSchedulerStart = EFalse;
                    ERR_PRINTF2(_L("DevSound InitializeL left with error = %d"), err);
                    Fsm(EDevSoundError);
                    }
                }
            break;
            }

        case EDevSoundPlayInit:
            {
            // Initializing the audio device and starting the play process
            INFO_PRINTF1(_L("iMMFDevSound->PlayInitL()"));
            TRAPD(err, iMMFDevSound->PlayInitL());// Get buffer from DevSound
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("DevSound PlayInitL left with error = %d"), err);
                Fsm(EDevSoundError);
                }
            if (iTestState == 0)
                {
                // initialise and start the timer. the file is short and so decode occur rapidly
                // hence the short time passed to After()
                TRAP(err, iPlayTimer = CMmfDevSoundPlayTimer::NewL(this, 500));//500
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("CMmfDevSoundPlayTimer::NewL left with error = %d"), err);
                    Fsm(EDevSoundError);
                    break;
                    }
                iPlayTimer->Start();
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
            // initialise and start the timer. to stop recording after 10 secs
            TRAP(err, iPlayTimer = CMmfDevSoundPlayTimer::NewL(this, KMicroSecsInTenSec));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("CMmfDevSoundPlayTimer::NewL left with error = %d"), err);
                Fsm(EDevSoundError);
                break;
                }
            iPlayTimer->Start();
            break;
            }
            
        case EDevSoundPlayData:
            {
            // Fill the buffer with audio data and play the data in the buffer
            // Read sizeof buffer from file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            TInt err = iFile.Read(buffer->Data());
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("iFile.Read() returned with %d"), err);
                Fsm(EDevSoundError);
                }
            if (buffer->Data().Length() != buffer->RequestSize())
                {
                INFO_PRINTF3(_L("Data length copied from file = %d. Expected = %d. Must be EOF."), buffer->Data().Length(), buffer->RequestSize());
                iBuffer->SetLastBuffer(ETrue);
                iLastBuffer = ETrue;
                }

            // Playing data in the buffer at the current volume
            INFO_PRINTF1(_L("iMMFDevSound->PlayData()"));
            iMMFDevSound->PlayData();
            iBufferCount++;
            break;
            }

         case EDevSoundRecordData:
            {
            // Write buffer to file
            CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iBuffer);
            TInt err = iFile.Write(buffer->Data());
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("iFile.Write() returned with %d"), err);
                Fsm(EDevSoundError);
                }
            // Recording Data to a Buffer
            INFO_PRINTF1(_L("iMMFDevSound->RecordData()"));
            iMMFDevSound->RecordData();
            iBufferCount++;
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
 * InitializeComplete()
 *
 */
void RTestStepDevSoundPlayRecord::InitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called InitializeComplete with error = %d"), aError);
    
    if (aError != KErrNone)
        {
        Fsm(EDevSoundError);        
        return;
        }
    else
        {
        if (iTestState == 1)
            {
            // Call to initialize the audio device and start the record process
            Fsm(EDevSoundRecordInit);
            }
        else
            {
            // Call to initialize the audio device and start the play process
            Fsm(EDevSoundPlayInit);
            }
         }
     }

/**
 *
 * BufferToBeFilled()
 *
 */
void RTestStepDevSoundPlayRecord::BufferToBeFilled(CMMFBuffer* aBuffer)
    {
    INFO_PRINTF1(_L("DevSound called BufferToBeFilled."));

    if (!aBuffer)
        {
        ERR_PRINTF1(_L("BufferToBeFilled callback received a NULL CMMFBuffer!"));
        Fsm(EDevSoundError);        
        return;
        }

    iBuffer = aBuffer;
    Fsm(EDevSoundPlayData); // Call to start the play process
    }

/**
 *
 * PlayError()
 *
 */
void RTestStepDevSoundPlayRecord::PlayError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called PlayError with error = %d"), aError);
    
    if((iTestState == 0) && (aError == KErrUnderflow))
    	{
    	if(!iLastBuffer)
    		{
    		if(iRetryTestState0) // Retry Test State 0
    			{
    			ERR_PRINTF2(_L("Underflow occured when test state is 0, trying %d more time!"), iRetryTestState0);    			
    			iRetryTestState0--;
    			iMMFDevSound->Stop();
    			iPlayTimer->Cancel();
    			delete iPlayTimer;
    			iPlayTimer = NULL;
    			Fsm(EDevSoundInit);
    			}
    		else // Do not retry Test State 0 and end test step
    			{
    			ERR_PRINTF1(_L("Underflow occured too many times, exiting test step."));
    			iRunNextTestState = EFalse;
    			Fsm(EDevSoundError);
    			}
    		}
    	else // Underflow with last Buffer, call Finished manually
    		{
    		ERR_PRINTF1(_L("Underflow with iLastBuffer = ETrue, calling Finished() manually..."));
    		Finished();
    		}
    	}
    else if ((iTestState == 2) && (iLastBuffer) && (aError == KErrUnderflow))
        {
        iTestStepResult = EPass;
        Fsm(EDevSoundError);
        }
    else 
    	{
    	iRunNextTestState = EFalse;
        Fsm(EDevSoundError);	
    	}
    }

/**
 *
 * BufferToBeEmptied()
 *
 */
void RTestStepDevSoundPlayRecord::BufferToBeEmptied(CMMFBuffer* aBuffer)
    {
    if (!aBuffer)
        {
        INFO_PRINTF1(_L("BufferToBeEmptied callback received a NULL CMMFBuffer"));
        Fsm(EDevSoundError);
        return;
        }
    INFO_PRINTF2(_L("DevSound called BufferToBeEmptied, with size = %d"), aBuffer->BufferSize());
    iBuffer = aBuffer;
    Fsm(EDevSoundRecordData); // Call to start the record process
    }

/**
 *
 * RecordError()
 *
 */
void RTestStepDevSoundPlayRecord::RecordError(TInt aError)
    {
    INFO_PRINTF2(_L("DevSound called RecordError with error = %d"), aError);
	iRunNextTestState = EFalse;
    Fsm(EDevSoundError);
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundPlayRecord::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0008-HP
    
    // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
	INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
	return EInconclusive;
#else    
    INFO_PRINTF1(_L("<<< Play/record an OggVorbis audio file >>>"));
    iTestStepResult = EFail;
    
    // get the original source file, and create a copy for recording.
    TPtrC tmpFilenameOp;
    TBuf<KSizeBuf> filenameOutput;
    if ( !GetStringFromConfig(iSectName, iKeyNameOutput, tmpFilenameOp) )
        {
        ERR_PRINTF1(_L("Error in getting filename for Output from INI file."));
        return EInconclusive;
        }
    GetDriveName(filenameOutput);
    filenameOutput.Append(tmpFilenameOp);
    
    // Close iFile and open the new file after copying the original
    iFile.Close();
    CopyFile(iFilename, filenameOutput);
    TInt err = iFile.Open(iFs, filenameOutput, EFileRead);
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
        iFs.Close();
        return EInconclusive;
        }
    
    // Display the name of the output file under test.
    INFO_PRINTF3(_L("Output File => %S (copied from %S)"), &filenameOutput, &iFilename);
    
    Fsm(EDevSoundInit); // -- Call to initialize DevSound for play
    StartScheduler();

    if( !iRunNextTestState)
    	{
    	return EFail;
    	}

    // Close file and open again
    iFile.Close();
    err = iFile.Open(iFs, filenameOutput, EFileWrite);
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
        iFs.Close();
        return EInconclusive;
        }
    // Seek to the end of file, from where we want to write
    TInt filePosition = 0;
    err = iFile.Seek(ESeekEnd, filePosition);
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("iFile.Seek() returned %d"), err);
        iFs.Close();
        return EInconclusive;
        }
        
    Fsm(EDevSoundInit); // -- Call to initialize DevSound for record
    StartScheduler();

    if( !iRunNextTestState)
    	{
    	return EFail;
    	}
    
    // Close file and open again
    iFile.Close();
    err = iFile.Open(iFs, filenameOutput, EFileRead);
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not open input file. Error = %d"), err);
        iFs.Close();
        return EInconclusive;
        }

    Fsm(EDevSoundInit); // -- Call to initialize DevSound for play
    StartScheduler();

    return iTestStepResult;
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
    }

//
// Negative Tests //
//

//
// RTestStepDevSoundPlayInvBitrate
//
   
/**
 *
 * RTestStepDevSoundPlayInvBitrate - Test step constructor
 *
 */
RTestStepDevSoundPlayInvBitrate::RTestStepDevSoundPlayInvBitrate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    : RTestStepDevSoundPlayBase(aTestName, aSectName, aKeyName, aInputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundPlayInvBitrate* RTestStepDevSoundPlayInvBitrate::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    {
    RTestStepDevSoundPlayInvBitrate* self = new (ELeave) RTestStepDevSoundPlayInvBitrate(aTestName, aSectName, aKeyName, aInputDataType);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundPlayInvBitrate::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0504-HP
    
    // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
	INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
	return EInconclusive;
#else
    INFO_PRINTF1(_L("<<< Play an OggVorbis audio file at an invalid bit rate >>>"));
    iTestStepResult = EFail;
    iTestType = EBitRateTest;

    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
    }

//
// RTestStepDevSoundPlayCorruptFile
//

/**
 *
 * Test step constructor.
 * Ctor for RTestStepDevSoundPlayCorruptFile.
 *
 */
RTestStepDevSoundPlayCorruptFile::RTestStepDevSoundPlayCorruptFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    : RTestStepDevSoundPlayBase(aTestName, aSectName, aKeyName, aInputDataType)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestStepDevSoundPlayCorruptFile* RTestStepDevSoundPlayCorruptFile::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TFourCC& aInputDataType)
    {
    RTestStepDevSoundPlayCorruptFile* self = new (ELeave) RTestStepDevSoundPlayCorruptFile(aTestName, aSectName, aKeyName, aInputDataType);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestStepDevSoundPlayCorruptFile::DoTestStepL()
    {
    // Test: MM-MMF-DEVSOUND-I-0505-HP
    
    // This test only runs on legacy builds.
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND    
	INFO_PRINTF1(_L("<<< No operations being carried out as this test does not run on A3F builds as of Def124186 >>>"));
	return EInconclusive;
#else    
    INFO_PRINTF1(_L("<<< Play a corrupted OggVorbis audio file >>>"));
    iTestStepResult = EFail;

    Fsm(EDevSoundInit); // Call to initialize DevSound
    StartScheduler(); // ActiveScheduler started ONLY once

    return iTestStepResult;
#endif //SYMBIAN_MULTIMEDIA_A3FDEVSOUND
    }

//
// CMmfDevSoundPlayTimer
//

/**
 * NewL
 */
CMmfDevSoundPlayTimer* CMmfDevSoundPlayTimer::NewL(RTestStepDevSoundPlayBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    {
    CMmfDevSoundPlayTimer* self = new (ELeave) CMmfDevSoundPlayTimer(aParent, aWaitTime);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }
    
/**
 * Constructor
 */
CMmfDevSoundPlayTimer::CMmfDevSoundPlayTimer(RTestStepDevSoundPlayBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    : CTimer(EPriorityNormal), iParent(aParent), iWaitTime(aWaitTime)
    {    
    CActiveScheduler::Add(this);
    }    
    
/**
 * Start() - Start the timer
 */
void CMmfDevSoundPlayTimer::Start()
    {
    After(iWaitTime);
    }

/**
 * RunL() - see CActive class for more info
 */
void CMmfDevSoundPlayTimer::RunL()
    {        
    // (iParent->Finished() should call Stop() or not, as we decide)    
    iParent->Finished();
    }

/**
 * RunError() - see CActive class for more info
 */
TInt CMmfDevSoundPlayTimer::RunError(TInt aError)
    {
    iParent->Fsm(EDevSoundError);
    return aError;
    }

