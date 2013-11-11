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
// Implementation of test step classes for DevVideoRecord.
// 
//

#include "tsi_mmf_devvideo_record.h"

//
// Positive Tests //
//

//
// RTestStepDevVideoRecordBase
//

/**
 *
 * RTestStepDevVideoRecordBase - Test step constructor
 *
 */
RTestStepDevVideoRecordBase::RTestStepDevVideoRecordBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    {
    // Store the name of this test case
    iTestStepName = aTestName;

    // Section name for retrieving filename
    iSectName = aSectName;

    // Key name for retrieving filename
    iKeyName = aKeyName;

    // Keep track on number of times MdvroStreamEnd callback is called
    iEndCount = 0;
    
    // we are allocating buffers in these tests so we must grow the heap
    iHeapSize = 0x100000;     // 1M
    
    // Initialise callback counters
    iCallback1 = 0;
    iCallback2 = 0;
    
    // initialise the Encoder UID
    iEncoderUid = aEncoderUid;
    
    iMMFDevVideoRecord = NULL;
    }

/**
 *
 * Fsm()
 * Executes events of DevVideoRecord in sequence
 *
 */
void RTestStepDevVideoRecordBase::Fsm(TMmfDevVideoRecordEvents aDevVideoRecordEvent)
    {
    switch (aDevVideoRecordEvent)
        {
        case EDevVideoRecordInit:
            {
            TInt err = KErrNone;

            // Select the video encoder to be used (Earlier... KUidDevVideoTestEncodeHwDevice)
            INFO_PRINTF2(_L("iMMFDevVideoRecord->SelectEncoderL(0x%8x)"), iEncoderUid);
            TRAP(err, iEncoderHwDeviceId = iMMFDevVideoRecord->SelectEncoderL(iEncoderUid));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->SelectEncoderL() left with error : %d"), err);
                return;
                }

            // Set the Video Input and Output Formats for encode
            INFO_PRINTF1(_L("Set the Input and Output Video formats for encode..."));
            TRAP(err, err = SetInputOutputFormatsL());
            if (err != KErrNone)
                {
                ERR_PRINTF1(_L("ERROR - in Setting Input / Output Formats."));
                return;
                }

            // Initializes video device
            INFO_PRINTF1(_L("iMMFDevVideoRecord->Initialize()"));
            iMMFDevVideoRecord->Initialize();
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->Initialize() left with error : %d."), iError);
                return;
                }
            
            // Start recording video
            iMMFDevVideoRecord->Start();
            
            // encode a picture
            iBitmap->LockHeap();
            TUint32* dataAddress = iBitmap->DataAddress();
            TSize theSize = iBitmap->SizeInPixels();
            TInt frameSize = theSize.iHeight * theSize.iWidth * 3;
            TVideoPicture theVideoPicture;
            TPictureData& thePictureData = theVideoPicture.iData;
            TPtr8 theRawData((TUint8*)dataAddress, frameSize);
            thePictureData.iDataSize = theSize;
            thePictureData.iRawData = &theRawData;
            
            // write the encoded picture
            INFO_PRINTF1(_L("iMMFDevVideoRecord->WritePictureL()"));
            TRAP(err, iMMFDevVideoRecord->WritePictureL(&theVideoPicture));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->WritePictureL() returned error : %d."), err);
                iTestStepResult = EFail;
                }
            iBitmap->UnlockHeap(ETrue);
            
            // start the scheduler
            CActiveScheduler::Start();
            
            break;
            }

        default:
            {
            INFO_PRINTF1(_L("Invalid DevVideoRecord event!"));
            }
        }
    }

/**
 *
 * DoTestStepPreambleL() - Test Preamble routines
 *
 */
TVerdict RTestStepDevVideoRecordBase::DoTestStepPreambleL()
    {
    // Install the Active Scheduler
    iActiveScheduler = new (ELeave) CActiveScheduler;
    CActiveScheduler::Install (iActiveScheduler);
    
    // connect to FBS session
    RFbsSession::Connect();
        
    // create DevVideoRecord object
    INFO_PRINTF1(_L("-- Creating DevVideoRecord object"));
    TRAPD(err, iMMFDevVideoRecord = CMMFDevVideoRecord::NewL(*this));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR : Could not create DevVideoRecord object. Error = %d"), err);
        return EInconclusive;
        }

    // Get the filename of the video file to record
    TPtrC filename; 
    if ( !GetStringFromConfig(iSectName, iKeyName, filename) )
        {
        ERR_PRINTF1(_L("ERROR : Could not get filename from INI file."));
        return EInconclusive;
        }
    GetDriveName(iFilename);
    iFilename.Append(filename);
    
    // connect to File Session
    err = iFs.Connect();
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR : Could not connect to Filesystem. Error = %d"), err);
        return EInconclusive;
        }

    // Load the bitmap to encode
    iBitmap = new(ELeave)CFbsBitmap;
    err = iBitmap->Load(iFilename, 0, ETrue);
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR : Could not load bitmap. Error = %d"), err);
        return EInconclusive;
        }
    
    // Get the file name for the RecordBinFile, to compare the output buffers
    TFileName recordBufFile;
    if ( !GetStringFromConfig(iSectName, _L("RecordBinFile"), filename) )
        {
        ERR_PRINTF1(_L("ERROR : Could not get value for key 'RecordBinFile' from INI file."));
        return EInconclusive;
        }
    GetDriveName(recordBufFile);
    recordBufFile.Append(filename);
    
    // Load the test buffer and fill with the contents of bin file
    static const TInt KDataBufSize = 128000; // 128k
    iDataBuf = HBufC8::NewMaxL(KDataBufSize);
    err = iFile.Open(iFs, recordBufFile, EFileRead);
    if (err != KErrNone)
        {
        ERR_PRINTF3(_L("ERROR : Could not open bin file - '%S'. Error = %d"), &recordBufFile, err);
        iFs.Close();
        return EInconclusive;
        }
    TPtr8 dataBufPtr = iDataBuf->Des();
    err = iFile.Read(dataBufPtr);
    if (err != KErrNone)
        {
        ERR_PRINTF3(_L("ERROR : Could not read from file '%S', error : %d"), &recordBufFile, iError);
        return EInconclusive;
        }
    iFile.Close();
    
    // Get the file name for Output
    TFileName recordOutFile;
    if ( !GetStringFromConfig(iSectName, _L("RecordOutFile"), filename) )
        {
        ERR_PRINTF1(_L("ERROR : Could not get value for key 'RecordOutFile' from INI file."));
        return EInconclusive;
        }
    GetDriveName(recordOutFile);
    recordOutFile.Append(filename);
    err = iFile.Replace(iFs, recordOutFile, EFileWrite|EFileShareExclusive);
    if (err != KErrNone)
        {
        ERR_PRINTF3(_L("ERROR : Could not open output file - '%S'. Error = %d"), &recordOutFile, err);
        iFs.Close();
        return EInconclusive;
        }
        
    INFO_PRINTF2(_L("-- File under test - < %S >"), &iFilename);
    return EPass;
    }

/**
 *
 * DoTestStepPostambleL() - Test Postamble routines
 *
 */
TVerdict RTestStepDevVideoRecordBase::DoTestStepPostambleL()
    {
    INFO_PRINTF1(_L("-- Deleting DevVideoRecord object..."));
    delete iMMFDevVideoRecord;
    iMMFDevVideoRecord = NULL;

    delete iActiveScheduler;
    iActiveScheduler = NULL;
    
    iFile.Close();
    iFs.Close();
    
    delete iDataBuf;
    delete iBitmap;
    
    // disconnect FBS session
    RFbsSession::Disconnect();
    
    return EPass;
    }

/**
 *
 * SetInputOutputFormatsL()
 *
 */
TInt RTestStepDevVideoRecordBase::SetInputOutputFormatsL()
    {
    // set video format.
    TSize size(176,144);
    TReal32 rate = 10.0;
    TUncompressedVideoFormat videoFormat;
    videoFormat.iDataFormat = ERgbFbsBitmap;
    videoFormat.iRgbFormat = EFbsBitmapColor16M;
        
    // set input format for encoder
    TRAP(iError, iMMFDevVideoRecord->SetInputFormatL(iEncoderHwDeviceId, videoFormat, size) );
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("-- SetInputFormatL() on Encoder gave error %d."), iError);
        return iError;
        }
    else
        {
        INFO_PRINTF2(_L("-- SetInputFormat() returned %d"), iError);
        }
    
    // set source memory for encoder
    TRAP(iError, iMMFDevVideoRecord->SetSourceMemoryL(rate, ETrue, ETrue) );
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("-- SetSourceMemoryL() on Encoder gave error %d."), iError);
        return iError;
        }
    else
        {
        INFO_PRINTF2(_L("-- SetSourceMemoryL() returned %d"), iError);
        }
    
    // create CCompressedVideoFormat with XviD mime type.
    CCompressedVideoFormat* vidCV = NULL;
    if (iEncoderUid == KUidTestVideoPuXvidEncoderDevice)
    	{
    	vidCV = CCompressedVideoFormat::NewL(KXvidEncoderMimeType, KNullDesC8);	
    	}
    else
    	{
    	vidCV = CCompressedVideoFormat::NewL(KTestEncoderMimeType, KNullDesC8);		
    	}

    CleanupStack::PushL(vidCV);
    // set output format for encoder (Not Supported at the moment)
    TRAP(iError, iMMFDevVideoRecord->SetOutputFormatL(iEncoderHwDeviceId, *vidCV, EDuCodedPicture, EDuElementaryStream, ETrue) );
    if (iError != KErrNotSupported)
        {
        ERR_PRINTF2(_L("-- SetOutputFormatL() on Encoder gave error %d."), iError);
        return iError;
        }
    else
        {
        INFO_PRINTF2(_L("-- SetOutputFormatL() returned %d"), iError);
        }
    CleanupStack::PopAndDestroy(vidCV);
    
    // if it comes here, no error so, reset iError to KErrNone and return
    iError = KErrNone;
    return iError;
    }

/**
 *
 * MdvroReturnPicture()
 * Returns a used input video picture back to the caller.
 *
 */
void RTestStepDevVideoRecordBase::MdvroReturnPicture(TVideoPicture* /*aPicture*/)
    {
    INFO_PRINTF1(_L("RTestStepDevVideoRecordBase::MdvroReturnPicture()."));
    iCallback1++;
    }

/**
 *
 * MdvroSupplementalInfoSent()
 * Signals that the supplemental info send request has completed.
 *
 */
void RTestStepDevVideoRecordBase::MdvroSupplementalInfoSent()
    {
    INFO_PRINTF1(_L("RTestStepDevVideoRecordBase::MdvroSupplementalInfoSent()."));
    }

/**
 *
 * MdvroNewBuffers()
 * Notifies the client that one or more new output buffers are available.
 *
 */
void RTestStepDevVideoRecordBase::MdvroNewBuffers()
    {
    INFO_PRINTF1(_L("RTestStepDevVideoRecordBase::MdvroNewBuffers()."));
    TRAP(iError, iOutputBuf = iMMFDevVideoRecord->NextBufferL());
    iCallback2++;
    }

/**
 *
 * MdvroFatalError()
 * Reports a fatal encoding or capturing error to the client.
 *
 */
void RTestStepDevVideoRecordBase::MdvroFatalError(TInt aError)
    {
    INFO_PRINTF2(_L("RTestStepDevVideoRecordBase::MdvroFatalError(). Error = %d"), aError);
    iError = aError;
    }

/**
 *
 * MdvroInitializeComplete()
 * Reports that DevVideoRecord initialization has completed.
 *
 */
void RTestStepDevVideoRecordBase::MdvroInitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("RTestStepDevVideoRecordBase::MdvroInitializeComplete():  Error = %d"), aError);
    iError = aError;
    }

/**
 *
 * MdvroStreamEnd()
 * Reports that the input video data end has been reached and all pictures have been processed.
 *
 */
void RTestStepDevVideoRecordBase::MdvroStreamEnd()
    {
    INFO_PRINTF1(_L("RTestStepDevVideoRecordBase::MdvroStreamEnd()."));

    // Keep track on number of times this callback is called
    iEndCount++;
    }

//
// RTestDevVideoRecordSimple
//

/**
 *
 * RTestDevVideoRecordSimple - Test step constructor
 *
 */
RTestDevVideoRecordSimple::RTestDevVideoRecordSimple(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    :RTestStepDevVideoRecordBase(aTestName, aSectName, aKeyName, aEncoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoRecordSimple* RTestDevVideoRecordSimple::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    {
    RTestDevVideoRecordSimple* self = new (ELeave) RTestDevVideoRecordSimple(aTestName, aSectName, aKeyName, aEncoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoRecordSimple::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Record a video file, specifying Uid of XviD encoder without PU >>>"));
    iTestStepResult = EFail; // Assume test will fail
    iTestType = EUidTest;
    
    Fsm(EDevVideoRecordInit); // Call to initialize DevVideoRecord
    
    // compare the data
    if(!iOutputBuf)
        {
        ERR_PRINTF1(_L("iOutputBuf was null"));
        iTestStepResult = EFail;
        }
    else
        {
        TInt err = iDataBuf->Compare(iOutputBuf->iData);
        if(!err)
            {
            INFO_PRINTF2(_L("Data matches. Length (Actual == Expected) %d"), iOutputBuf->iData.Length());
            iTestStepResult = EPass;
            }
        else
            {
            ERR_PRINTF3(_L("Data mismatch. Length (Actual Vs Expected) %d Vs %d"), iOutputBuf->iData.Length(), iDataBuf->Des().Length());
            iTestStepResult = EFail;
            }
        }
    return iTestStepResult;
    }

/**
 *
 * MdvroNewBuffers()
 * Notifies the client that one or more new output buffers are available.
 *
 */
void RTestDevVideoRecordSimple::MdvroNewBuffers()
    {
    INFO_PRINTF1(_L("RTestDevVideoRecordSimple::MdvroNewBuffers()."));
    TRAP(iError, iOutputBuf = iMMFDevVideoRecord->NextBufferL());
    iCallback2++;
    CActiveScheduler::Stop();
    }

//
// RTestDevVideoRecordUidHwDev
//

/**
 *
 * RTestDevVideoRecordUidHwDev - Test step constructor
 *
 */
RTestDevVideoRecordUidHwDev::RTestDevVideoRecordUidHwDev(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, TTestType aTestType, const TUid& aEncoderUid)
    :RTestStepDevVideoRecordBase(aTestName, aSectName, aKeyName, aEncoderUid)
    {
    iTestType = aTestType;
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoRecordUidHwDev* RTestDevVideoRecordUidHwDev::NewL(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, TTestType aTestType, const TUid& aEncoderUid)
    {
    RTestDevVideoRecordUidHwDev* self = new (ELeave) RTestDevVideoRecordUidHwDev(aTestName, aSectName, aKeyName, aTestType, aEncoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoRecordUidHwDev::DoTestStepL()
    {
    if (iTestType == EInvalidUidTest)
        {
        INFO_PRINTF1(_L("<<< Try to Record a video file, specifying invalid Uid of video HwDevice >>>"));
        }
    else
        {
        INFO_PRINTF1(_L("<<< Record a video specifying the Uid of the video encoder HwDevice >>>"));
        }
    iTestStepResult = EFail;
        
    Fsm(EDevVideoRecordInit); // Call to initialize DevVideoRecord
    
    return iTestStepResult;
    }

/**
 *
 * Fsm()
 * Executes events of DevVideoRecord in sequence
 *
 */
void RTestDevVideoRecordUidHwDev::Fsm(TMmfDevVideoRecordEvents aDevVideoRecordEvent)
    {
    switch (aDevVideoRecordEvent)
        {
        case EDevVideoRecordInit:
            {
            TInt err = KErrNone;

            if (iTestType == EInvalidUidTest)
                {
                // Select the video encoder to be used
                INFO_PRINTF2(_L("iMMFDevVideoRecord->SelectEncoderL(0x%8x)"), KInvalidUidDevVideoTestEncodeHwDevice);
                TRAP(err, iEncoderHwDeviceId = iMMFDevVideoRecord->SelectEncoderL(KInvalidUidDevVideoTestEncodeHwDevice));
                if (err == KErrNotFound)
                    {
                    INFO_PRINTF1(_L("iMMFDevVideoRecord->SelectEncoderL(invalid Uid) left with error KErrNotFound. Test PASS!"));
                    iTestStepResult = EPass;
                    }
                else
                    {
                    ERR_PRINTF2(_L("iMMFDevVideoRecord->SelectEncoderL() left with error : %d"), err);
                    }
                return;
                }

            if (iTestType == EUidTest)
                {
                // Select the video encoder to be used (Earlier... KUidDevVideoTestEncodeHwDevice)
                INFO_PRINTF2(_L("iMMFDevVideoRecord->SelectEncoderL(0x%8x)"), iEncoderUid);
                TRAP(err, iEncoderHwDeviceId = iMMFDevVideoRecord->SelectEncoderL(iEncoderUid));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->SelectEncoderL() left with error : %d"), err);
                    return;
                    }
                }

            // Set the Video Input and Output Formats for encode
            TRAP(err, err = SetInputOutputFormatsL());
            if (err != KErrNone)
                {
                ERR_PRINTF1(_L("ERROR - in Setting Input / Output Formats."));
                return;
                }

            // Initializes video device
            INFO_PRINTF1(_L("iMMFDevVideoRecord->Initialize()"));
            iMMFDevVideoRecord->Initialize();
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->Initialize() left with error : %d."), iError);
                return;
                }

            // Start recording video
            iMMFDevVideoRecord->Start();
            
            // encode a picture
            iBitmap->LockHeap();
            TUint32* dataAddress = iBitmap->DataAddress();
            TSize theSize = iBitmap->SizeInPixels();
            TInt frameSize = theSize.iHeight * theSize.iWidth * 3;
            TVideoPicture theVideoPicture;
            TPictureData& thePictureData = theVideoPicture.iData;
            TPtr8 theRawData((TUint8*)dataAddress, frameSize);
            thePictureData.iDataSize = theSize;
            thePictureData.iRawData = &theRawData;
            
            // write the encoded picture
            INFO_PRINTF1(_L("iMMFDevVideoRecord->WritePictureL()"));
            TRAP(err, iMMFDevVideoRecord->WritePictureL(&theVideoPicture));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->WritePictureL() returned error : %d."), err);
                iTestStepResult = EFail;
                }
            iBitmap->UnlockHeap(ETrue);
            
            // start the scheduler
            CActiveScheduler::Start();
            
            // Check for the size of the output file.
            TInt size = 0;
		    User::LeaveIfError(iFile.Size(size));
            if (size > 0)
                {
                INFO_PRINTF1(_L("The output filesize is greater than ZERO."));
                iTestStepResult = EPass;
                }
            else{
                ERR_PRINTF1(_L("The output filesize is NOT greater than ZERO."));
                iTestStepResult = EFail;
                }

            break;
            }

        default:
            {
            INFO_PRINTF1(_L("Invalid DevVideoRecord event!"));
            }
        }
    }

/**
 *
 * MdvroNewBuffers()
 * Notifies the client that one or more new output buffers are available.
 *
 */
void RTestDevVideoRecordUidHwDev::MdvroNewBuffers()
    {
    INFO_PRINTF1(_L("RTestDevVideoRecordUidHwDev::MdvroNewBuffers()."));
    TRAP(iError, iOutputBuf = iMMFDevVideoRecord->NextBufferL());
    iCallback2++;
    iFile.Write(iOutputBuf->iData);
    iMMFDevVideoRecord->ReturnBuffer(iOutputBuf);
    CActiveScheduler::Stop();
    }

//
// RTestDevVideoListVideoEncoders
//

/**
 *
 * RTestDevVideoListVideoEncoders - Test step constructor
 *
 */
RTestDevVideoListVideoEncoders::RTestDevVideoListVideoEncoders(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    :RTestStepDevVideoRecordBase(aTestName, aSectName, aKeyName, aEncoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoListVideoEncoders* RTestDevVideoListVideoEncoders::NewL(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    {
    RTestDevVideoListVideoEncoders* self = new (ELeave) RTestDevVideoListVideoEncoders(aTestName, aSectName, aKeyName, aEncoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoListVideoEncoders::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Get a list of available video encoders >>>"));
    iTestStepResult = EFail;

    RArray<TUid> foundEncoders;
    CleanupClosePushL(foundEncoders);
    
    // Get the encoders list in an array
    TRAPD(err, iMMFDevVideoRecord->GetEncoderListL(foundEncoders));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("iMMFDevVideoRecord->GetEncoderListL() left with error : %d"), err);
        }
    else
        {
        // Make sure that the UID of the XviD test plugin is in the array..
        TInt numberFound = foundEncoders.Count();
        INFO_PRINTF2(_L("iMMFDevVideoRecord->GetEncoderListL() returned %d encoders"), numberFound);
        for (TInt i = 0; i < numberFound; i++)
            {
            INFO_PRINTF3(_L("Device#%d => UID - '0x%8x'"), i, foundEncoders[i]);
            if (foundEncoders[i] == iEncoderUid)
                {
                INFO_PRINTF2(_L("The UID of the Video Encode HwDevice '0x%8x' is found !"), iEncoderUid);
                iTestStepResult = EPass;
                break;
                }
            }
        }
    
    // display message if the required encoder is not listed.
    if(iTestStepResult == EFail)
        {
        INFO_PRINTF2(_L("The UID of the Video Encode HwDevice '0x%8x' is NOT Listed !"), iEncoderUid);
        }
    
    CleanupStack::PopAndDestroy(&foundEncoders); //-- foundEncoders

    return iTestStepResult;
    }

//
// RTestDevVideoListVideoPreProcessors
//

/**
 *
 * RTestDevVideoListVideoPreProcessors - Test step constructor
 *
 */
RTestDevVideoListVideoPreProcessors::RTestDevVideoListVideoPreProcessors(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    :RTestStepDevVideoRecordBase(aTestName, aSectName, aKeyName, aEncoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoListVideoPreProcessors* RTestDevVideoListVideoPreProcessors::NewL(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    {
    RTestDevVideoListVideoPreProcessors* self = new (ELeave) RTestDevVideoListVideoPreProcessors(aTestName, aSectName, aKeyName, aEncoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoListVideoPreProcessors::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Get a list of available video Pre-processors >>>"));
    iTestStepResult = EPass; //--No Preprocessors available

    RArray<TUid> foundPreProc;
    CleanupClosePushL(foundPreProc);
    
    // Get the encoders list in an array
    TRAPD(err, iMMFDevVideoRecord->GetPreProcessorListL(foundPreProc));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("iMMFDevVideoRecord->GetPreProcessorListL() left with error : %d"), err);
        iTestStepResult = EFail; // if the API leaves, then the test fails...
        }
    else
        {
        // Make sure that the UID of the XviD test plugin is in the array..
        TInt numberFound = foundPreProc.Count();
        INFO_PRINTF2(_L("iMMFDevVideoRecord->GetPreProcessorListL() returned %d pre-processors"), numberFound);
        for (TInt i = 0; i < numberFound; i++)
            {
            if (foundPreProc[i] == KUidTestVideoEncodeStubPU)
                {
                INFO_PRINTF1(_L("The UID of the Video Pre-processor HwDevice is found !"));
                iTestStepResult = EPass;
                break;
                }
            }
        }
    
    CleanupStack::PopAndDestroy(&foundPreProc); //-- foundPreProc

    return iTestStepResult;
    }

//
// RTestDevVideoRecordConfHwDev
//

/**
 *
 * RTestDevVideoRecordConfHwDev - Test step constructor
 *
 */
RTestDevVideoRecordConfHwDev::RTestDevVideoRecordConfHwDev(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    :RTestStepDevVideoRecordBase(aTestName, aSectName, aKeyName, aEncoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoRecordConfHwDev* RTestDevVideoRecordConfHwDev::NewL(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    {
    RTestDevVideoRecordConfHwDev* self = new (ELeave) RTestDevVideoRecordConfHwDev(aTestName, aSectName, aKeyName, aEncoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoRecordConfHwDev::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Configure and reconfigure the video HwDevice and record a video file >>>"));
    iTestStepResult = EFail;
    TInt err = KErrNone;
    
    Fsm(EDevVideoRecordInit); // Call to initialize and configure DevVideoRecord
    
    INFO_PRINTF1(_L("<<< Reconfigure the video HwDevice and record the video file >>>"));
    // destroy existing DevVideoRecord instance
    INFO_PRINTF1(_L("Deleting DevVideoRecord object..."));
    delete iMMFDevVideoRecord;
    iMMFDevVideoRecord = NULL;
    
    // create a new DevVideoRecord instance
    INFO_PRINTF1(_L("Creating DevVideoRecord object..."));
    TRAP(err, iMMFDevVideoRecord = CMMFDevVideoRecord::NewL(*this));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not create DevVideoRecord object. Error = %d"), err);
        return EFail;
        }
    
    Fsm(EDevVideoRecordInit); // Call to re-initialize and re-configure DevVideoRecord
    
    // Check for the number of callbacks
    if(iCallback1 == 2 && iCallback2 == 2) 
        {
        INFO_PRINTF1(_L("The Correct number of callbacks were received..."));
        iTestStepResult = EPass;
        }
    
    return iTestStepResult;
    }

/**
 *
 * MdvroNewBuffers()
 * Notifies the client that one or more new output buffers are available.
 *
 */
void RTestDevVideoRecordConfHwDev::MdvroNewBuffers()
    {
    INFO_PRINTF1(_L("RTestDevVideoRecordConfHwDev::MdvroNewBuffers()."));
    TRAP(iError, iOutputBuf = iMMFDevVideoRecord->NextBufferL());
    iCallback2++;
    CActiveScheduler::Stop();
    }

//
// RTestDevVideoRecordAlloc
//

/**
 *
 * RTestDevVideoRecordAlloc - Test step constructor
 *
 */
RTestDevVideoRecordAlloc::RTestDevVideoRecordAlloc(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    :RTestStepDevVideoRecordBase(aTestName, aSectName, aKeyName, aEncoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoRecordAlloc* RTestDevVideoRecordAlloc::NewL(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    {
    RTestDevVideoRecordAlloc* self = new (ELeave) RTestDevVideoRecordAlloc(aTestName, aSectName, aKeyName, aEncoderUid);
    return self;
    }


/**
 *
 * DoTestL()
 * Executes events of DevVideoAlloc in sequence
 *
 */
void RTestDevVideoRecordAlloc::DoTestL()
    {
    TInt err = KErrNone;

    // Select the video encoder to be used (Earlier... KUidDevVideoTestEncodeHwDevice)
    INFO_PRINTF2(_L("iMMFDevVideoRecord->SelectEncoderL(0x%8x)"), iEncoderUid);
    TRAP(err, iEncoderHwDeviceId = iMMFDevVideoRecord->SelectEncoderL(iEncoderUid));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->SelectEncoderL() left with error : %d"), err);
        User::Leave(err);
        }

    // Set the Video Input and Output Formats for encode
    INFO_PRINTF1(_L("Set the Input and Output Video formats for encode..."));
    TRAP(err, err = SetInputOutputFormatsL());
    if (err != KErrNone)
        {
        ERR_PRINTF1(_L("ERROR - in Setting Input / Output Formats."));
        User::Leave(err);
        }

    // Initializes video device
    INFO_PRINTF1(_L("iMMFDevVideoRecord->Initialize()"));
    iMMFDevVideoRecord->Initialize();
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->Initialize() left with error : %d."), iError);
        User::Leave(err);
        }
    
    // Start recording video
    INFO_PRINTF1(_L("iMMFDevVideoRecord->Start()"));
    iMMFDevVideoRecord->Start();
    User::LeaveIfError(iError);
    
    // encode a picture
    iBitmap->LockHeap();
    TUint32* dataAddress = iBitmap->DataAddress();
    TSize theSize = iBitmap->SizeInPixels();
    TInt frameSize = theSize.iHeight * theSize.iWidth * 3;
    TVideoPicture theVideoPicture;
    TPictureData& thePictureData = theVideoPicture.iData;
    TPtr8 theRawData((TUint8*)dataAddress, frameSize);
    thePictureData.iDataSize = theSize;
    thePictureData.iRawData = &theRawData;
    
    // write the encoded picture
    INFO_PRINTF1(_L("iMMFDevVideoRecord->WritePictureL()"));
    TRAP(err, iMMFDevVideoRecord->WritePictureL(&theVideoPicture));
    // moved here else the heap stays locked if we leave
	iBitmap->UnlockHeap(ETrue);
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->WritePictureL() returned error : %d."), err);
        User::Leave(err);
        }
    
    // added as MdvrpFatalError may have been called back
    User::LeaveIfError(iError);
    
    // start the scheduler
    iSchedulerStarted = ETrue;
    CActiveScheduler::Start();
    iSchedulerStarted = EFalse;
    User::LeaveIfError(iError);
    
    // Check for the size of the output file.
    TInt size = 0;
    User::LeaveIfError(iFile.Size(size));
    if (size > 0)
        {
        INFO_PRINTF1(_L("The output filesize is greater than ZERO."));
        iTestStepResult = EPass;
        }
    else{
        ERR_PRINTF1(_L("The output filesize is less than or equal to ZERO."));
        iTestStepResult = EFail;
        }
    }

/**
 *
 * Record video by marking heap.
 *
 */
void RTestDevVideoRecordAlloc::VideoRecordAllocL()
    {
    __MM_HEAP_MARK;
    TInt startAllocCount = User::CountAllocCells();
    TInt err = KErrNone;
    
    // create new CMMFDevVideoRecord instance
    TRAP(err,  iMMFDevVideoRecord = CMMFDevVideoRecord::NewL(*this));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not create DevVideoRecord object. Error = %d"), err);
        //RFbsSession::Disconnect();
        User::Leave(err);
        }
    
    // Call to do the testing
    TRAP(err,  DoTestL());
    
    // cleanup CMMFDevVideoRecord instance
    delete iMMFDevVideoRecord;
    iMMFDevVideoRecord = NULL;

    // final check for errors
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("DoTestL() left with error : %d"), err);
        User::Leave(err);
        }

    TInt endAllocCount = User::CountAllocCells();
    __MM_HEAP_MARKEND;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoRecordAlloc::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Alloc test to load a video HwDevice to record a video file >>>"));
    iTestStepResult = EFail;
    iTestType = EUidTest;
    
    TRAPD(err, VideoRecordAllocL());
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Opening/recording the video file failed, unexpected error %d."), err);
        return iTestStepResult;
        }

    iTestStepResult  = EPass;

    TInt failCount = 1;
    TBool completed = EFalse;
    TBool badResult  = EFalse;
    TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
    for(;;)    
        {
        __UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
        __MM_HEAP_MARK;

        // call the test method
        TRAP(err, VideoRecordAllocL());

        completed = EFalse;
        if (err == KErrNone)
            {
            TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
            if (testAlloc==NULL)
                {
                reachedEnd = ETrue;
                failCount--; // -= 1;
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
            INFO_PRINTF2(_L("Bad error code %d"), err);
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
    
    // final check for errors
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
    
    // for the postamble to clean it up...
    //RFbsSession::Connect();
    
    return iTestStepResult;
    }

/**
 *
 * MdvroNewBuffers()
 * Notifies the client that one or more new output buffers are available.
 *
 */
void RTestDevVideoRecordAlloc::MdvroNewBuffers()
    {
    INFO_PRINTF1(_L("RTestDevVideoRecordAlloc::MdvroNewBuffers()."));
    TRAP(iError, iOutputBuf = iMMFDevVideoRecord->NextBufferL());
    iCallback2++;
    iFile.Write(iOutputBuf->iData);
    iMMFDevVideoRecord->ReturnBuffer(iOutputBuf);
    CActiveScheduler::Stop();
    }

/**
 *
 * MdvroFatalError()
 * Reports a fatal encoding or capturing error to the client.
 *
 */
void RTestDevVideoRecordAlloc::MdvroFatalError(TInt aError)
    {
    INFO_PRINTF2(_L("RTestDevVideoRecordAlloc::MdvroFatalError(). Error = %d"), aError);
    iError = aError;
    if(iSchedulerStarted)
        {
        CActiveScheduler::Stop();
        }
    }

//
// RTestDevVideoRecordStop
//

/**
 *
 * RTestDevVideoRecordStop - Test step constructor
 *
 */
RTestDevVideoRecordStop::RTestDevVideoRecordStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    :RTestStepDevVideoRecordBase(aTestName, aSectName, aKeyName, aEncoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoRecordStop* RTestDevVideoRecordStop::NewL(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    {
    RTestDevVideoRecordStop* self = new (ELeave) RTestDevVideoRecordStop(aTestName, aSectName, aKeyName, aEncoderUid);
    return self;
    }

/**
 *
 * Fsm()
 * Executes events of DevVideoRecord in sequence
 *
 */
void RTestDevVideoRecordStop::Fsm(TMmfDevVideoRecordEvents aDevVideoRecordEvent)
    {
    switch (aDevVideoRecordEvent)
        {
        case EDevVideoRecordInit:
            {
            TInt err = KErrNone;

            // Select the video encoder to be used (Earlier... KUidDevVideoTestEncodeHwDevice)
            INFO_PRINTF2(_L("iMMFDevVideoRecord->SelectEncoderL(0x%8x)"), iEncoderUid);
            TRAP(err, iEncoderHwDeviceId = iMMFDevVideoRecord->SelectEncoderL(iEncoderUid));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->SelectEncoderL() left with error : %d"), err);
                return;
                }

            // Set the Video Input and Output Formats for encode
            TRAP(err, err = SetInputOutputFormatsL());
            if (err != KErrNone)
                {
                ERR_PRINTF1(_L("ERROR - in Setting Input / Output Formats."));
                return;
                }
                
            // Initializes video device
            INFO_PRINTF1(_L("iMMFDevVideoRecord->Initialize()"));
            iMMFDevVideoRecord->Initialize();
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->Initialize() left with error : %d."), iError);
                return;
                }

            // Stop recording video
            iMMFDevVideoRecord->Stop();
            
            // Check for fatal error - KErrNotSupported
            // NB - Record Stop gives a fatal error (MdvroFatalError - KErrNotSupported)...
            if (iError == KErrNotSupported)
                {
                INFO_PRINTF1(_L("WARNING - Fatal Error - KErrNotSupported"));
                iTestStepResult = EPass;
                }
            
            break;
            }

        default:
            {
            INFO_PRINTF1(_L("Invalid DevVideoRecord event!"));
            }
        }
    }

/**
 *
 * MdvroFatalError()
 * Reports a fatal encoding or capturing error to the client.
 *
 */
void RTestDevVideoRecordStop::MdvroFatalError(TInt aError)
    {
    INFO_PRINTF2(_L("RTestDevVideoRecordStop::MdvroFatalError(). Error = %d"), aError);
    iError = aError;
    }

/**
 *
 * MdvroInitializeComplete()
 * Reports that DevVideoRecord initialization has completed.
 *
 */
void RTestDevVideoRecordStop::MdvroInitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("RTestDevVideoRecordStop::MdvroInitializeComplete():  Error = %d"), aError);
    iError = aError;
    }

/**
 *
 * MdvroStreamEnd()
 * Reports that the input video data end has been reached and all pictures have been processed.
 *
 */
void RTestDevVideoRecordStop::MdvroStreamEnd()
    {
    INFO_PRINTF1(_L("RTestDevVideoRecordStop::MdvroStreamEnd()."));
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoRecordStop::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Record/stop recording a video file >>>"));
    iTestStepResult = EFail;

    Fsm(EDevVideoRecordInit); // Call to initialize DevVideoRecord
    
    return iTestStepResult;
    }

//
// RTestDevVideoRecordPause
//

/**
 *
 * RTestDevVideoRecordPause - Test step constructor
 *
 */
RTestDevVideoRecordPause::RTestDevVideoRecordPause(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    :RTestStepDevVideoRecordBase(aTestName, aSectName, aKeyName, aEncoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoRecordPause* RTestDevVideoRecordPause::NewL(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    {
    RTestDevVideoRecordPause* self = new (ELeave) RTestDevVideoRecordPause(aTestName, aSectName, aKeyName, aEncoderUid);
    return self;
    }

/**
 *
 * Fsm()
 * Executes events of DevVideoRecord in sequence
 *
 */
void RTestDevVideoRecordPause::Fsm(TMmfDevVideoRecordEvents aDevVideoRecordEvent)
    {
    switch (aDevVideoRecordEvent)
        {
        case EDevVideoRecordInit:
            {
            TInt err = KErrNone;

            // Select the video encoder to be used (Earlier... KUidDevVideoTestEncodeHwDevice)
            INFO_PRINTF2(_L("iMMFDevVideoRecord->SelectEncoderL(0x%8x)"), iEncoderUid);
            TRAP(err, iEncoderHwDeviceId = iMMFDevVideoRecord->SelectEncoderL(iEncoderUid));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->SelectEncoderL() left with error : %d"), err);
                return;
                }

            // Set the Video Input and Output Formats for encode
            TRAP(err, err = SetInputOutputFormatsL());
            if (err != KErrNone)
                {
                ERR_PRINTF1(_L("ERROR - in Setting Input / Output Formats."));
                return;
                }

            // Initializes video device
            INFO_PRINTF1(_L("iMMFDevVideoRecord->Initialize()"));
            iMMFDevVideoRecord->Initialize();
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->Initialize() left with error : %d."), iError);
                return;
                }

            // Pause recording video
            INFO_PRINTF1(_L("iMMFDevVideoRecord->Pause()"));
            iMMFDevVideoRecord->Pause();
            
            // Check for fatal error - KErrNotSupported
            // NB - Record Pause gives a fatal error (MdvroFatalError - KErrNotSupported)...
            if (iError == KErrNotSupported)
                {
                INFO_PRINTF1(_L("WARNING - Fatal Error - KErrNotSupported"));
                iTestStepResult = EPass;
                }

            break;
            }

        default:
            {
            INFO_PRINTF1(_L("Invalid DevVideoRecord event!"));
            }
        }
    }


/**
 *
 * MdvroFatalError()
 * Reports a fatal encoding or capturing error to the client.
 *
 */
void RTestDevVideoRecordPause::MdvroFatalError(TInt aError)
    {
    INFO_PRINTF2(_L("RTestDevVideoRecordPause::MdvroFatalError(). Error = %d"), aError);
    iError = aError;
    }

/**
 *
 * MdvroInitializeComplete()
 * Reports that DevVideoRecord initialization has completed.
 *
 */
void RTestDevVideoRecordPause::MdvroInitializeComplete(TInt aError)
    {
    INFO_PRINTF2(_L("RTestDevVideoRecordPause::MdvroInitializeComplete():  Error = %d"), aError);
    iError = aError;
    }

/**
 *
 * MdvroStreamEnd()
 * Reports that the input video data end has been reached and all pictures have been processed.
 *
 */
void RTestDevVideoRecordPause::MdvroStreamEnd()
    {
    INFO_PRINTF1(_L("RTestDevVideoRecordPause::MdvroStreamEnd()."));
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoRecordPause::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Record/pause recording a video file >>>"));
    iTestStepResult = EFail;

    Fsm(EDevVideoRecordInit); // Call to initialize DevVideoRecord
    
    return iTestStepResult;
    }

//
// RTestDevVideoRecordRecordingPos
//

/**
 *
 * RTestDevVideoRecordRecordingPos - Test step constructor
 *
 */
RTestDevVideoRecordRecordingPos::RTestDevVideoRecordRecordingPos(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    :RTestStepDevVideoRecordBase(aTestName, aSectName, aKeyName, aEncoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoRecordRecordingPos* RTestDevVideoRecordRecordingPos::NewL(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    {
    RTestDevVideoRecordRecordingPos* self = new (ELeave) RTestDevVideoRecordRecordingPos(aTestName, aSectName, aKeyName, aEncoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoRecordRecordingPos::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Get current recording position of the video file >>>"));
    iTestStepResult = EFail;
    iTestType = EUidTest;
        
    Fsm(EDevVideoRecordInit); // Call to initialize DevVideoRecord

    // Get recording position
    TTimeIntervalMicroSeconds recordPos = iMMFDevVideoRecord->RecordingPosition();

    // Check against test data
    // we know the encoding runs at 10 fps, so it should be 100000 for the first frame..
    TTimeIntervalMicroSeconds testTime(KTestRecordPosition);
    if (recordPos != testTime)
        {
        ERR_PRINTF3(_L("RecordingPosition() incorrect - gave %d (expected %d)"),I64LOW(recordPos.Int64()), KTestRecordPosition);
        }
    else
        {
        INFO_PRINTF1(_L("RecordingPosition() called successfully."));
        iTestStepResult = EPass;
        }

    return iTestStepResult;
    }

/**
 *
 * MdvroNewBuffers()
 * Notifies the client that one or more new output buffers are available.
 *
 */
void RTestDevVideoRecordRecordingPos::MdvroNewBuffers()
    {
    INFO_PRINTF1(_L("RTestDevVideoRecordRecordingPos::MdvroNewBuffers()."));
    TRAP(iError, iOutputBuf = iMMFDevVideoRecord->NextBufferL());
    iCallback2++;
    CActiveScheduler::Stop();
    }

//
// RTestDevVideoRecordEncoderInfo
//

/**
 *
 * RTestDevVideoRecordEncoderInfo - Test step constructor
 *
 */
RTestDevVideoRecordEncoderInfo::RTestDevVideoRecordEncoderInfo(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    :RTestStepDevVideoRecordBase(aTestName, aSectName, aKeyName, aEncoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoRecordEncoderInfo* RTestDevVideoRecordEncoderInfo::NewL(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    {
    RTestDevVideoRecordEncoderInfo* self = new (ELeave) RTestDevVideoRecordEncoderInfo(aTestName, aSectName, aKeyName, aEncoderUid);
    return self;
    }

/**
 *
 * ValidEncoderInfo()
 *
 */
TBool RTestDevVideoRecordEncoderInfo::ValidEncoderInfoL(CVideoEncoderInfo* aInfo)
    {
    if (!aInfo) 
        {
        ERR_PRINTF1(_L("ERROR - Encoder Info is NULL!"));
        return EFalse;
        }

    // check the encoder against test data
    // need to check every call so as to keep code coverage
    // and prevent a situation where if the first test fails
    // then subsequent tests will not be called
    TBool ret = ETrue;

    // UID check
    if (aInfo->Uid() != iEncoderUid)
        {
        ERR_PRINTF2(_L("ERROR - Encoder UID '0x%8x' is incorrect!"), aInfo->Uid());
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF2(_L("INFO - Encoder UID '0x%8x' is correct!"), iEncoderUid);
        }

    // Manufacturer check
    if (!(aInfo->Manufacturer() == KXvidEncoderInfoManufacturer))
        {
        ERR_PRINTF1(_L("ERROR - Manufacturer is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Manufacturer is correct!"));
        }

    // Identifier check
    if (!(aInfo->Identifier() == KNullDesC))
        {
        ERR_PRINTF1(_L("ERROR - Identifier is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Identifier is correct!"));
        }

    // Version check
    TVersion decVer = aInfo->Version();
    if ( (decVer.iMajor != KXvidEncoderInfoVersionMaj) ||
         (decVer.iMinor != KXvidEncoderInfoVersionMin) ||
         (decVer.iBuild != KXvidEncoderInfoVersionBuild) )
        {
        ERR_PRINTF1(_L("ERROR - Version is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Version is correct!"));
        }

    // Accelerated check
    if (aInfo->Accelerated())
        {
        ERR_PRINTF1(_L("ERROR - Accelerated = ETrue!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Accelerated = EFalse!"));
        }

    // Direct display check
    if (aInfo->SupportsDirectCapture())
        {
        ERR_PRINTF1(_L("ERROR - Supports Direct Capture = ETrue!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Supports Direct Capture = EFalse!"));
        }

    // Max picture size
    if (!(aInfo->MaxPictureSize() == TSize(KXvidEncoderInfoMaxSizeX, KXvidEncoderInfoMaxSizeY)))
        {
        ERR_PRINTF1(_L("ERROR - Max picture size is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Max picture size is correct!"));
        }

    // Supported data unit types
    if (aInfo->SupportedDataUnitTypes() != KXvidUnitType1)
        {
        ERR_PRINTF1(_L("ERROR - Supported data unit types != EDuCodedPicture!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Supported data unit types == EDuCodedPicture!"));
        }

    // Supported data unit encapsulation
    if (aInfo->SupportedDataUnitEncapsulations() != KXvidEncapType1)
        {
        ERR_PRINTF1(_L("ERROR - Supported data unit encapsulation != EDuElementaryStream!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Supported data unit encapsulation == EDuElementaryStream!"));
        }

    // Max bitrate layers
    if (aInfo->MaxBitrateLayers() != KXvidNumBitrateLayers)
        {
        ERR_PRINTF1(_L("ERROR - Max bitrate layers is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Max bitrate layers is correct!"));
        }

    // Supports supplemental enhancement info
    if (aInfo->SupportsSupplementalEnhancementInfo())
        {
        ERR_PRINTF1(_L("ERROR - Supports Enhancement Info = ETrue!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Supports Enhancement Info = EFalse!"));
        }

    // Max unequal error protection levels
    if (aInfo->MaxUnequalErrorProtectionLevels() != KXvidEncoderInfoMaxUEPLevels)
        {
        ERR_PRINTF1(_L("ERROR - Max UEP levels is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Max UEP levels is correct!"));
        }

    // Max bitrate
    if (aInfo->MaxBitrate() != KXvidEncoderInfoMaxBitrate)
        {
        ERR_PRINTF1(_L("ERROR - Encoder Max bitrate is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Encoder Max bitrate is correct!"));
        }

    // Max in layer scalability steps
    if (aInfo->MaxInLayerScalabilitySteps() != KXvidEncoderInfoMaxILSSteps)
        {
        ERR_PRINTF1(_L("ERROR - Max ILS steps is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Max ILS steps is correct!"));
        }

    // Supported picture options
    if (aInfo->SupportedPictureOptions() != KXvidEncoderInfoPictureOptions)
        {
        ERR_PRINTF1(_L("ERROR - Supported Picture options is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Supported Picture options is correct!"));
        }


    // Supports Picture Loss
    if (aInfo->SupportsPictureLoss())
        {
        ERR_PRINTF1(_L("ERROR - Supports Picture Loss = ETrue!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Supports Picture Loss = EFalse!"));
        }

    // Supports Slice Loss
    if (aInfo->SupportsSliceLoss())
        {
        ERR_PRINTF1(_L("ERROR - Supports Slice Loss = ETrue!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Supports Slice Loss = EFalse!"));
        }

    // Coding standard info
    if (!(aInfo->CodingStandardSpecificInfo() == KXvidEncoderInfoCSInfo))
        {
        ERR_PRINTF1(_L("ERROR - Coding standard specific info is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Coding standard specific info is correct!"));
        }

    // Implementation info
    if (!(aInfo->ImplementationSpecificInfo() == KXvidEncoderInfoISInfo))
        {
        ERR_PRINTF1(_L("ERROR - Implementation specific info is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Implementation specific info is correct!"));
        }
    
    // Get encoder supported output formats array
    const RPointerArray<CCompressedVideoFormat>& vidFormats = aInfo->SupportedOutputFormats();
    
    // Get the expected Test formats supported and check if XviD CV output format is supported
    CCompressedVideoFormat* testFormat = NULL;
    TInt err = KErrNone;
    if (iEncoderUid == KUidTestVideoPuXvidEncoderDevice)
    	{
    	TRAP(err, testFormat = GetTestCVFormatL(KXvidEncoderMimeType));	
    	}
    else
    	{
    	TRAP(err, testFormat = GetTestCVFormatL(KTestEncoderMimeType));	
    	}
    if (err != KErrNone)
        {
        ERR_PRINTF1(_L("ERROR - Failed to construct CCompressedVideoFormat for XviD mimetype (video/mp4v-es)!"));
        ret = EFalse;
        }
    else if ( !(*testFormat == *(vidFormats[0])) || !(aInfo->SupportsOutputFormat(*testFormat)) )
        {
        ERR_PRINTF1(_L("ERROR - SupportsOutputFormat(XviD) returned EFalse!"));
        ret = EFalse;
        }
    delete testFormat;
    testFormat=NULL;

    // Finally check an CV output format which isn't supposed to be supported
    TRAP(err, testFormat = GetTestCVFormatL(_L8("video/unsupported")));
    if (err != KErrNone)
        {
        ERR_PRINTF1(_L("ERROR - Failed to construct CCompressedVideoFormat for un-supported mimetype!"));
        ret = EFalse;
        }
    else if (aInfo->SupportsOutputFormat(*testFormat))
        {
        ERR_PRINTF1(_L("ERROR - SupportsOutputFormat(un-supported) returned ETrue!"));
        ret = EFalse;
        }
    delete testFormat;
    testFormat=NULL;
        
    // Test supported max picture rates
    const RArray<TPictureRateAndSize>& maxRates = aInfo->MaxPictureRates();
    // Test supported max picture rates
    if (!(maxRates[0].iPictureSize == TSize(KXvidEncoderInfoMaxPicSizeX, KXvidEncoderInfoMaxPicSizeY)) || 
        !(maxRates[0].iPictureRate == KXvidEncoderInfoMaxPicRate) )
        {
        ERR_PRINTF1(_L("ERROR - Max Picture size & rate is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Max Picture size & rate is correct!"));
        }
    
    // expected input formats array
    // TUncompressedVideoFormat is a union, therefore use hard initialisation
    RArray<TUncompressedVideoFormat> expectedVidFormats;
    CleanupClosePushL(expectedVidFormats);
    expectedVidFormats.Reset();
    TUncompressedVideoFormat inputFormats[3];
    inputFormats[0].iDataFormat = ERgbRawData;
    inputFormats[0].iRgbFormat = ERgb16bit565;
    expectedVidFormats.AppendL(inputFormats[0]);
    inputFormats[1].iDataFormat = ERgbFbsBitmap;
    inputFormats[1].iRgbFormat = EFbsBitmapColor16M;
    expectedVidFormats.AppendL(inputFormats[1]);
    inputFormats[2].iDataFormat = EYuvRawData;
    memset(&inputFormats[2].iYuvFormat, 0, sizeof(TYuvFormat));
    expectedVidFormats.AppendL(inputFormats[2]);

    // Get supported uncompressed input formats
    const RArray<TUncompressedVideoFormat>& vidFormatsUC = aInfo->SupportedInputFormats();
    if(vidFormatsUC.Count() != 3)
        {
        ERR_PRINTF2(_L("ERROR - XviD Encoder supports more than 3 input formats (%d)!"), vidFormatsUC.Count());
        ret = EFalse;
        }

    // Test input formats supported
    for (TUint j = 0; j < 3; j++)
        {
        //if ( !(expectedVidFormats[j] == (vidFormatsUC[j])) || !(aInfo->SupportsInputFormat(expectedVidFormats[j])) )
        if ( !(expectedVidFormats.operator[](j) == (vidFormatsUC.operator[](j)))) 
            {
            ERR_PRINTF2(_L("ERROR - Video Format %d is NOT matching!"), j);
            ret = EFalse;
            }
        else if(!(aInfo->SupportsInputFormat(expectedVidFormats[j])))
            {
            ERR_PRINTF2(_L("ERROR - Video Format %d is NOT supported!"), j);
            ret = EFalse;
            }
        else
            {
            INFO_PRINTF2(_L("INFO - Video Format %d is matching & supported!"), j);
            }
        }
    expectedVidFormats.Close(); // no need to reset here, as Close() is called.
    CleanupStack::PopAndDestroy(&expectedVidFormats);
    
    // Check an input format that isn't supposed to be supported
    TUncompressedVideoFormat testUCFormat;
    testUCFormat = KTestPostProcInfoNegativeFormat;
    testUCFormat.iYuvFormat = KTestYuvFormat1;
    if (aInfo->SupportsInputFormat(testUCFormat))
        {
        ERR_PRINTF1(_L("ERROR - Failed negative SupportsInputFormat() check!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Passed negative SupportsInputFormat() check!"));
        }

    return ret;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoRecordEncoderInfo::DoTestStepL()
    {
    INFO_PRINTF2(_L("<<< Get information about the '%S' encoder >>>"), &iSectName);
    iTestStepResult = EPass; // Assume success
    TBool valid = EFalse;

    // Get encoder information from XviD encoder plugin (Earlier... KUidDevVideoTestEncodeHwDevice)
    CVideoEncoderInfo* videoEncoderInfo = NULL;
    INFO_PRINTF2(_L("iMMFDevVideoRecord->VideoEncoderInfoLC(0x%8x)"), iEncoderUid);
    videoEncoderInfo = iMMFDevVideoRecord->VideoEncoderInfoLC(iEncoderUid);
    valid = ValidEncoderInfoL(videoEncoderInfo);
    CleanupStack::PopAndDestroy(videoEncoderInfo);

    if (!valid) // Check if the Video Encoder Info class is valid
        {
        ERR_PRINTF1(_L("CVideoEncoderInfo class is corrupt!"));
        iTestStepResult = EFail;
        }
    else
        {
        INFO_PRINTF1(_L("VideoEncoderInfoLC() passed check successfully!"));
        }

    return iTestStepResult;
    }
    
//
// Negative Tests //
//

//
// RTestDevVideoRecordWithoutVideoType
//

/**
 *
 * RTestDevVideoRecordWithoutVideoType - Test step constructor
 *
 */
RTestDevVideoRecordWithoutVideoType::RTestDevVideoRecordWithoutVideoType(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    :RTestStepDevVideoRecordBase(aTestName, aSectName, aKeyName, aEncoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoRecordWithoutVideoType* RTestDevVideoRecordWithoutVideoType::NewL(const TDesC& aTestName, 
    const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid)
    {
    RTestDevVideoRecordWithoutVideoType* self = new (ELeave) RTestDevVideoRecordWithoutVideoType(aTestName, aSectName, aKeyName, aEncoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoRecordWithoutVideoType::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Open and record video file without specifying input / output video  types. >>>"));
    iTestStepResult = EFail;

    Fsm(EDevVideoRecordInit); // Call to initialize DevVideoRecord

    return iTestStepResult;
    }

/**
 *
 * Fsm()
 * Executes events of DevVideoRecord in sequence
 *
 */
void RTestDevVideoRecordWithoutVideoType::Fsm(TMmfDevVideoRecordEvents aDevVideoRecordEvent)
    {
    switch (aDevVideoRecordEvent)
        {
        case EDevVideoRecordInit:
            {
            TInt err = KErrNone;

            // Select the video encoder to be used (Earlier... KUidDevVideoTestEncodeHwDevice)
            INFO_PRINTF2(_L("iMMFDevVideoRecord->SelectEncoderL(0x%8x)"), iEncoderUid);
            TRAP(err, iEncoderHwDeviceId = iMMFDevVideoRecord->SelectEncoderL(iEncoderUid));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoRecord->SelectEncoderL() left with error : %d"), err);
                return;
                }

            // This test case does not set the Input / Output format for test.
            // DO NOT Set the Video Input and Output Formats for encode

            // Initializes video device
            INFO_PRINTF1(_L("iMMFDevVideoRecord->Initialize()"));
            iMMFDevVideoRecord->Initialize();
            
            if (iError == KErrNotReady)
                {
                ERR_PRINTF1(_L("ERROR - iMMFDevVideoRecord->Initialize() left with error KErrNotReady"));
                iTestStepResult = EPass;
                }

            break;
            }

        default:
            {
            INFO_PRINTF1(_L("Invalid DevVideoRecord event!"));
            }
        }
    }

