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
// Implementation of test step classes for DevVideoPlay.
// 
//

#include "tsi_mmf_devvideo_play.h"

//
// Positive Tests //
//

//
// RTestStepDevVideoPlayBase
//

/**
 *
 * RTestStepDevVideoPlayBase - Test step constructor
 *
 */
RTestStepDevVideoPlayBase::RTestStepDevVideoPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
     // Store the name of this test case
     iTestStepName = aTestName;

    // Section name for retrieving filename
    iSectName = aSectName;

    // Key name for retrieving filename
    iKeyName = aKeyName;

    // Keep track on number of times MdvroStreamEnd callback is called
    iEndCount = 0;
	
	// Test for re-configuration of video Hw
    iReConfigure = EFalse;
    
    // we are allocating buffers in these tests so we must grow the heap
    iHeapSize = 0x100000;     // 1M
    
    // initialise the Decoder UID
    iDecoderUid = aDecoderUid;
    
    iMMFDevVideoPlay = NULL;
    }

/**
 *
 * Fsm()
 * Executes events of DevVideoPlay in sequence
 *
 */
void RTestStepDevVideoPlayBase::Fsm(TMmfDevVideoPlayEvents aDevVideoPlayEvent)
    {
    switch (aDevVideoPlayEvent)
        {
        case EDevVideoPlayInit:
            {
            TInt err = KErrNone;

            if (iTestType == EInvalidUidTest)
                {
                // Select the video decoder to be used
                INFO_PRINTF2(_L("iMMFDevVideoPlay->SelectDecoderL(0x%8x)"), KInvalidUidDevVideoTestDecodeHwDevice);
                TRAP(err, iDecoderHwDeviceId = iMMFDevVideoPlay->SelectDecoderL(KInvalidUidDevVideoTestDecodeHwDevice));
                if (err == KErrNotFound)
                    {
                    INFO_PRINTF1(_L("iMMFDevVideoPlay->SelectDecoderL(InvalidUID) left with error KErrNotFound. Test PASS!"));
                    iTestStepResult = EPass;
                    }
                else
                    {
                    ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->SelectDecoderL() left with unexpected error : %d"), err);
                    }
                return;
                }

            if (iTestType == EUidTest)
                {
                // Select the video decoder to be used (Earlier... KUidDevVideoTestDecodeHwDevice)
                INFO_PRINTF2(_L("iMMFDevVideoPlay->SelectDecoderL(0x%8x)"), iDecoderUid);
                TRAP(err, iDecoderHwDeviceId = iMMFDevVideoPlay->SelectDecoderL(iDecoderUid));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->SelectDecoderL() left with error : %d"), err);
                    return;
                    }
                }

            // Set the Video Input and Output Formats for decode
            TRAP(err, err = SetInputOutputFormatsL());
            if (err != KErrNone)
                {
                ERR_PRINTF1(_L("ERROR - in Setting Input / Output Formats."));
                return;
                }

            // Initializing the video device
            INFO_PRINTF1(_L("iMMFDevVideoPlay->Initialize()"));
            iMMFDevVideoPlay->Initialize();
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->Initialize() left with error : %d."), iError);
                return;
                }

            // Start video playback, including decoding, post-processing, and rendering
            INFO_PRINTF1(_L("iMMFDevVideoPlay->Start()"));
            iMMFDevVideoPlay->Start();
            
            // Start the scheduler
            CActiveScheduler::Start();
            
            // Stop playback, once callback is called...
            INFO_PRINTF1(_L("iMMFDevVideoPlay->Stop()"));
            iMMFDevVideoPlay->Stop();
            
            // check for the end count
            if (iEndCount != 1)
                {
                ERR_PRINTF2(_L("ERROR - MdvpoStreamEnd only called %d times!"), iEndCount);
                }
            else
                {
                INFO_PRINTF1(_L("InputEnd() called successfully."));
                iTestStepResult = EPass;
                }
                
            // flush any unplayed frames
            TVideoPicture* thePicture = NULL;
            do
                {
                TRAP(err, thePicture = iMMFDevVideoPlay->NextPictureL());
                if(err == KErrNone && thePicture != NULL)
                    {
                    iMMFDevVideoPlay->ReturnPicture(thePicture);
                    }
                }
            while(thePicture != NULL);
            
            break;
            }

        default:
            {
            INFO_PRINTF1(_L("Invalid DevVideoPlay event!"));
            }
        }
    }

/**
 *
 * DoTestStepPreambleL - Test Preamble routines
 *
 */
TVerdict RTestStepDevVideoPlayBase::DoTestStepPreambleL()
    {
    // Install the Active Scheduler
    iActiveScheduler = new (ELeave) CActiveScheduler;
    CActiveScheduler::Install(iActiveScheduler);
    
    // Connect to RFbsSession
    RFbsSession::Connect();
    
    // create DevVideoPlay object
    INFO_PRINTF1(_L("-- Creating DevVideoPlay object..."));
    TRAPD(err,  iMMFDevVideoPlay = CMMFDevVideoPlay::NewL(*this));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR : Could not create DevVideoPlay object. Error = %d"), err);
        return EInconclusive;
        }

    // Get the filename of the video file to play
    TPtrC filename;
    if ( !GetStringFromConfig(iSectName, iKeyName, filename) )
        {
        ERR_PRINTF1(_L("ERROR in getting filename from INI file."));
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
    
    // open the file
    err = iFile.Open(iFs, iFilename, EFileRead);
    if (err != KErrNone)
        {
        ERR_PRINTF3(_L("ERROR : Could not open input file - '%S'. Error = %d"), &iFilename, err);
        iFs.Close();
        return EInconclusive;
        }
    
    INFO_PRINTF2(_L("-- File under test - < %S >"), &iFilename);
    return EPass;
    }

/**
 *
 * DoTestStepPostambleL - Test Postamble routines
 *
 */
TVerdict RTestStepDevVideoPlayBase::DoTestStepPostambleL()
    {
    INFO_PRINTF1(_L("-- Deleting DevVideoPlay object..."));
    delete iMMFDevVideoPlay;
    iMMFDevVideoPlay = NULL;

    delete iActiveScheduler;
    iActiveScheduler = NULL;

    iFile.Close();
    iFs.Close();
    
    // Disconnect RFbsSession
    RFbsSession::Disconnect();
    
    return EPass;
    }

/**
 *
 * SetInputOutputFormatsL()
 *
 */
TInt RTestStepDevVideoPlayBase::SetInputOutputFormatsL()
    {
    // create CCompressedVideoFormat with appropriate mime type.
    CCompressedVideoFormat* vidCV = NULL;
    if (iDecoderUid == KUidTestVideoPuXvidDecoderDevice)
    	{
    	vidCV = CCompressedVideoFormat::NewL(KXvidDecoderMimeType, KNullDesC8);	
    	}
    else
    	{
    	vidCV = CCompressedVideoFormat::NewL(KTestDecoderMimeType, KNullDesC8);		
    	}
    	
    CleanupStack::PushL(vidCV);
    
    // Set a hardware device input format to (an un) a compressed video format
    TRAP(iError, iMMFDevVideoPlay->SetInputFormatL(iDecoderHwDeviceId, *vidCV, EDuCodedPicture, EDuElementaryStream, ETrue));
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("-- SetInputFormatL() on Post-Processor gave error %d."), iError);
        User::Leave(iError);
        }
    else
        {
        INFO_PRINTF2(_L("-- SetInputFormat() returned %d"), iError);
        }
    CleanupStack::PopAndDestroy(vidCV);

    // Create an array to get the list of decode formats supported
    RArray<TUncompressedVideoFormat> decodeFormats;
    CleanupClosePushL(decodeFormats);
    
    // load the output format list
    TRAP(iError, iMMFDevVideoPlay->GetOutputFormatListL(iDecoderHwDeviceId, decodeFormats));
    decodeFormats.Close(); // no need to reset here, as Close() is called.
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("-- GetOutputFormatListL() on Decoder gave error %d."), iError);
        User::Leave(iError);
        }
    else
        {
        INFO_PRINTF2(_L("-- GetOutputFormatListL() returned %d"), iError);
        }
    CleanupStack::PopAndDestroy(&decodeFormats); //-- decodeFormats

    // Set Output format to an uncompressed format.
    TUncompressedVideoFormat reqFormat;
    reqFormat.iDataFormat = ERgbFbsBitmap;
    reqFormat.iRgbFormat = EFbsBitmapColor16M;
    TRAP(iError, iMMFDevVideoPlay->SetOutputFormatL(iDecoderHwDeviceId, reqFormat) );
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("-- SetOutputFormatL() on Decoder gave error %d."), iError);
        User::Leave(iError);
        }
    else
        {
        INFO_PRINTF2(_L("-- SetOutputFormatL() returned %d"), iError);
        }

    // Check the file header
    TRAP(iError, CheckHeaderInfoL());
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("-- CheckHeaderInfoL() on Decoder gave error %d."), iError);
        User::Leave(iError);
        }
    else
        {
        INFO_PRINTF2(_L("-- CheckHeaderInfoL() returned %d"), iError);
        }

    // return if any error
    return iError;
    }

/**
 *
 * CheckHeaderInfoL()
 *
 */
void RTestStepDevVideoPlayBase::CheckHeaderInfoL()
    {
    TInt pos = 0;
    TVideoInputBuffer theHeaderBuffer;
    HBufC8* theHeaderBufferData = HBufC8::NewMaxL(KHeaderBytesSize);
    CleanupStack::PushL(theHeaderBufferData);
    theHeaderBuffer.iData.Set(theHeaderBufferData->Des());
    
    // seeking to start, read file header data and then re-seek to start...
    iError = iFile.Seek(ESeekStart, pos);
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("Could not seek to start of file, error : %d"), iError);
        return;
        }
    iError = iFile.Read(theHeaderBuffer.iData, theHeaderBuffer.iData.Size());
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("Could not read from file, error : %d"), iError);
        return;
        }
    iError = iFile.Seek(ESeekStart, pos);
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("Could not seek to start of file, error : %d"), iError);
        return;
        }
    
    TVideoPictureHeader* thePicHeader = iMMFDevVideoPlay->GetHeaderInformationL(EDuArbitraryStreamSection, EDuElementaryStream, &theHeaderBuffer);

    CleanupStack::PopAndDestroy(theHeaderBufferData);
    iMMFDevVideoPlay->ReturnHeader(thePicHeader);
    }

/**
 *
 * MdvpoNewBuffers()
 * Notifies the client that one or more new empty input buffers are available.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoNewBuffers()
    {
    INFO_PRINTF1(_L("RTestStepDevVideoPlayBase::MdvpoNewBuffers()."));
    TInt err = KErrNone;
    
    if(iInputEnd)
        {
        return;
        }
    TInt numBuffers = iMMFDevVideoPlay->NumFreeBuffers();
    if(numBuffers > 0)
        {
        // keep on getting data until there is no more
        TInt fileSize;
        iFile.Size(fileSize);
        if(iTotalBytesRead >= fileSize) 
            {
            iInputEnd = ETrue;
            INFO_PRINTF1(_L("Input end"));
            iMMFDevVideoPlay->InputEnd();
            }
        else 
            {
            TVideoInputBuffer* theInputBuffer = NULL;
            TRAP(iError, theInputBuffer = iMMFDevVideoPlay->GetBufferL(KInputBufferSize));
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("iMMFDevVideoPlay->GetBufferL() left with error : %d"), iError);
                return;
                }
            
            if(theInputBuffer) 
                {
                iError = iFile.Read(theInputBuffer->iData);
                if (iError != KErrNone)
                    {
                    ERR_PRINTF2(_L("Could not read from file, error : %d"), iError);
                    return;
                    }
                
                iTotalBytesRead += theInputBuffer->iData.Size();
                TRAP(err, iMMFDevVideoPlay->WriteCodedDataL(theInputBuffer));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("iMMFDevVideoPlay->WriteCodedDataL() left with error : %d"), err);
                    return;
                    }
                }
            theInputBuffer->iData.Delete(0, theInputBuffer->iData.Length());
            theInputBuffer = NULL;
            }
        }
    }

/**
 *
 * MdvpoReturnPicture()
 * Returns a used input video picture back to the caller.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoReturnPicture(TVideoPicture* /*aPicture*/)
    {
    INFO_PRINTF1(_L("RTestStepDevVideoPlayBase::MdvpoReturnPicture()."));
    }

/**
 *
 * MdvpoSupplementalInformation()
 * Delivers supplemental information from a coded data unit. The information is codec-dependent.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoSupplementalInformation(const TDesC8& /*aData*/, 
    const TTimeIntervalMicroSeconds& /*aTimestamp*/, const TPictureId& /*aPictureId*/)
    {
    INFO_PRINTF1(_L("RTestStepDevVideoPlayBase::MdvpoSupplementalInformation()."));
    }

/**
 *
 * MdvpoPictureLoss()
 * Back channel information from the decoder, indicating a picture loss without specifying the lost picture.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoPictureLoss()
    {
    INFO_PRINTF1(_L("RTestStepDevVideoPlayBase::MdvpoPictureLoss()."));
    }

/**
 *
 * MdvpoPictureLoss()
 * Back channel information from the decoder, indicating the pictures that have been lost.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoPictureLoss(const TArray<TPictureId>& /*aPictures*/)
    {
    INFO_PRINTF1(_L("RTestStepDevVideoPlayBase::MdvpoPictureLoss()."));
    }

/**
 *
 * MdvpoSliceLoss()
 * Back channel information from the decoder, indicating the loss of consecutive 
 * macroblocks in raster scan order.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoSliceLoss(TUint /*aFirstMacroblock*/, TUint /*aNumMacroblocks*/, const TPictureId& /*aPicture*/)
    {
    INFO_PRINTF1(_L("RTestStepDevVideoPlayBase::MdvpoSliceLoss()."));
    }

/**
 *
 * MdvpoReferencePictureSelection()
 * Back channel information from the decoder, indicating a reference picture selection request.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoReferencePictureSelection(const TDesC8& /*aSelectionData*/)
    {
    INFO_PRINTF1(_L("RTestStepDevVideoPlayBase::MdvpoReferencePictureSelection()."));
    }

/**
 *
 * MdvpoTimedSnapshotComplete()
 * Called when a timed snapshot request has been completed.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoTimedSnapshotComplete(TInt /*aError*/, 
                                    TPictureData* /*aPictureData*/, 
                                    const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/, 
                                    const TPictureId& /*aPictureId*/)
    {
    INFO_PRINTF1(_L("RTestStepDevVideoPlayBase::MdvpoTimedSnapshotComplete()."));
    }

/**
 *
 * MdvpoNewPictures()
 * Notifies the client that one or more new output pictures are available.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoNewPictures()
    {
    INFO_PRINTF1(_L("RTestStepDevVideoPlayBase::MdvpoNewPictures()."));
    iNewPictures = ETrue;
    
    // dispose of the picture
    TVideoPicture* thePicture = NULL;
    TRAPD(err, thePicture = iMMFDevVideoPlay->NextPictureL());
    if(err == KErrNone && thePicture != NULL)
        {
        iMMFDevVideoPlay->ReturnPicture(thePicture);
        }
    }

/**
 *
 * MdvpoFatalError()
 * Reports a fatal decoding or playback error to the client.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoFatalError(TInt aError)
    {
    INFO_PRINTF2(_L("RTestStepDevVideoPlayBase::MdvpoFatalError():  Error = %d"), aError);
    iError = aError;
    }

/**
 *
 * MdvpoInitComplete()
 * Reports that DevVideoPlay initialization has completed.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoInitComplete(TInt aError)
    {
    INFO_PRINTF2(_L("RTestStepDevVideoPlayBase::MdvpoInitComplete():  Error = %d"), aError);
    iError = aError;
    }

/**
 *
 * MdvpoStreamEnd()
 * Reports that the input video stream end has been reached and all pictures have been processed.
 *
 */
void RTestStepDevVideoPlayBase::MdvpoStreamEnd()
    {
    INFO_PRINTF1(_L("RTestStepDevVideoPlayBase::MdvpoStreamEnd()."));

    // Mark that we have come to the end of the stream
    iStreamEnd = ETrue;

    // Keep track on number of times this callback is called
    iEndCount++;
    
    // Stop the scheduler
    CActiveScheduler::Stop();
    }

//
// RTestDevVideoPlaySimple
//

/**
 *
 * RTestDevVideoPlaySimple - Test step constructor
 *
 */
RTestDevVideoPlaySimple::RTestDevVideoPlaySimple(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoPlaySimple* RTestDevVideoPlaySimple::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoPlaySimple* self = new (ELeave) RTestDevVideoPlaySimple(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoPlaySimple::DoTestStepL()
    {
    INFO_PRINTF2(_L("<<< Play a video file specifying Uid of video HwDevice - %S. >>>"), &iSectName);
    iTestStepResult = EFail; // Assume test will fail
    iTestType = EUidTest;
    
    Fsm(EDevVideoPlayInit); // Call to initialize DevVideoPlay

    return iTestStepResult;
    }

//
// RTestDevVideoListVideoDecoders
//

/**
 *
 * RTestDevVideoListVideoDecoders - Test step constructor
 *
 */
RTestDevVideoListVideoDecoders::RTestDevVideoListVideoDecoders(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)     
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoListVideoDecoders* RTestDevVideoListVideoDecoders::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoListVideoDecoders* self = new (ELeave) RTestDevVideoListVideoDecoders(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoListVideoDecoders::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Get a list of available video decoders >>>"));
    iTestStepResult = EFail;

    RArray<TUid> foundDecoders;
    CleanupClosePushL(foundDecoders);

    // Get the decoders list in an array
    TRAPD(err, iMMFDevVideoPlay->GetDecoderListL(foundDecoders));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("iMMFDevVideoPlay->GetDecoderListL() left with error : %d"), err);
        }
    else
        {
        // Make sure that the UID of the XviD test plugin is in the array..
        TInt numberFound = foundDecoders.Count();
        INFO_PRINTF2(_L("iMMFDevVideoPlay->GetDecoderListL() returned %d decoders"), numberFound);
        for (TInt j = 0; j < numberFound; j++)
            {
            INFO_PRINTF3(_L("Device#%d => UID - '0x%8x'"), j, foundDecoders[j]);
            if (foundDecoders[j] == iDecoderUid)
                {
                INFO_PRINTF2(_L("The UID of the Video Decode HwDevice '0x%8x' is found !"), iDecoderUid);
                iTestStepResult = EPass;
                break;
                }
            }
        }
    
    // display message if the required decoder is not listed.
    if(iTestStepResult == EFail)
        {
        INFO_PRINTF2(_L("The UID of the Video Decode HwDevice '0x%8x' is NOT Listed !"), iDecoderUid);
        }
        
    CleanupStack::PopAndDestroy(&foundDecoders); //-- foundDecoders

    return iTestStepResult;
    }

//
// RTestDevVideoListVideoPostProcessors
//

/**
 *
 * RTestDevVideoListVideoPostProcessors - Test step constructor
 *
 */
RTestDevVideoListVideoPostProcessors::RTestDevVideoListVideoPostProcessors(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)     
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoListVideoPostProcessors* RTestDevVideoListVideoPostProcessors::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoListVideoPostProcessors* self = new (ELeave) RTestDevVideoListVideoPostProcessors(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoListVideoPostProcessors::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Get a list of available video post-processors >>>"));
    iTestStepResult = EPass; //--No Postprocessors available

    RArray<TUid> foundPostProc;
    CleanupClosePushL(foundPostProc);

    // Get the decoders list in an array
    TRAPD(err, iMMFDevVideoPlay->GetPostProcessorListL(foundPostProc));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("iMMFDevVideoPlay->GetPostProcessorListL() left with error : %d"), err);
        iTestStepResult = EFail; // if the API leaves, then the test fails...
        }
    else
        {
        // Make sure that the UID of the XviD test plugin is in the array..
        TInt numberFound = foundPostProc.Count();
        INFO_PRINTF2(_L("iMMFDevVideoPlay->GetPostProcessorListL() returned %d post-processors"), numberFound);
        for (TInt j = 0; j < numberFound; j++)
            {
            if (foundPostProc[j] == KUidTestVideoDecodeStubPU)
                {
                INFO_PRINTF1(_L("The UID of the Video Post-processor HwDevice is found !"));
                iTestStepResult = EPass;
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy(&foundPostProc); //-- foundPostProc

    return iTestStepResult;
    }

//
// RTestDevVideoPlayConfHwDev
//

/**
 *
 * RTestDevVideoPlayConfHwDev - Test step constructor
 *
 */
RTestDevVideoPlayConfHwDev::RTestDevVideoPlayConfHwDev(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoPlayConfHwDev* RTestDevVideoPlayConfHwDev::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoPlayConfHwDev* self = new (ELeave) RTestDevVideoPlayConfHwDev(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }

/**
 *
 * Fsm()
 * Executes events of DevVideoPlay in sequence
 *
 */
void RTestDevVideoPlayConfHwDev::Fsm(TMmfDevVideoPlayEvents aDevVideoPlayEvent)
    {
    switch (aDevVideoPlayEvent)
        {
        case EDevVideoPlayInit:
            {
            // Select the video decoder to be used (Earlier... KUidDevVideoTestDecodeHwDevice)
            INFO_PRINTF2(_L("iMMFDevVideoPlay->SelectDecoderL(0x%8x)"), iDecoderUid);
            TRAPD(err, iDecoderHwDeviceId = iMMFDevVideoPlay->SelectDecoderL(iDecoderUid));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->SelectDecoderL() left with error : %d"), err);
                return;
                }
            
            // Set the Video Input and Output Formats for decode
            TRAP(err, err = SetInputOutputFormatsL());
            if (err != KErrNone)
                {
                ERR_PRINTF1(_L("ERROR - in Setting Input / Output Formats."));
                return;
                }
                
            // Initializing the video device
            INFO_PRINTF1(_L("iMMFDevVideoPlay->Initialize()"));
            iMMFDevVideoPlay->Initialize();
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->Initialize() left with error : %d."), iError);
                return;
                }

            // Start video playback, including decoding, post-processing, and rendering
            INFO_PRINTF1(_L("iMMFDevVideoPlay->Start()"));
            iMMFDevVideoPlay->Start();
            
            // Start the scheduler
            CActiveScheduler::Start();
            
            break;
            }

        default:
            {
            INFO_PRINTF1(_L("Invalid DevVideoPlay event!"));
            }
        }
    }


/**
 *
 * MdvpoFatalError()
 * Reports a fatal decoding or playback error to the client.
 *
 */
void RTestDevVideoPlayConfHwDev::MdvpoFatalError(TInt aError)
    {
    INFO_PRINTF2(_L("RTestDevVideoPlayConfHwDev::MdvpoFatalError():  Error = %d"), aError);
    iError = aError;
    }

/**
 *
 * MdvpoInitComplete()
 * Reports that DevVideoPlay initialization has completed.
 *
 */
void RTestDevVideoPlayConfHwDev::MdvpoInitComplete(TInt aError)
    {
    INFO_PRINTF2(_L("RTestDevVideoPlayConfHwDev::MdvpoInitComplete():  Error = %d"), aError);
    iError = aError;
    }

/**
 *
 * MdvpoStreamEnd()
 * Reports that the input video stream end has been reached and all pictures have been processed.
 *
 */
void RTestDevVideoPlayConfHwDev::MdvpoStreamEnd()
    {
    INFO_PRINTF1(_L("RTestDevVideoPlayConfHwDev::MdvpoStreamEnd()"));
    
    CActiveScheduler::Stop();
    iMMFDevVideoPlay->Stop();
    
    if (iReConfigure)
        {
        INFO_PRINTF1(_L("<<< Reconfigure the video HwDevice and play the video file >>>"));
        iReConfigure = EFalse;
        iInputEnd = EFalse;
        TInt err = KErrNone;
        
        // destroy existing DevVideoPlay instance        
        INFO_PRINTF1(_L("Deleting DevVideoPlay object..."));
        delete iMMFDevVideoPlay;
        iMMFDevVideoPlay = NULL;
        
        // create a new DevVideoPlay instance        
        INFO_PRINTF1(_L("Creating DevVideoPlay object..."));
        TRAP(err, iMMFDevVideoPlay = CMMFDevVideoPlay::NewL(*this));
        if (err != KErrNone)
            {
            ERR_PRINTF2(_L("Could not create DevVideoPlay object. Error = %d"), err);
            return;
            }
        
        // Seek to the place we want to start reading from
        TInt filePosition = 0;
        iFile.Seek(ESeekStart, filePosition);
        iTotalBytesRead = 0;
            
        // Select the video decoder to be used (Earlier... KUidDevVideoTestDecodeHwDevice)
        INFO_PRINTF2(_L("iMMFDevVideoPlay->SelectDecoderL(0x%8x)"), iDecoderUid);
        TRAP(err, iDecoderHwDeviceId = iMMFDevVideoPlay->SelectDecoderL(iDecoderUid));
        if (err != KErrNone)
            {
            ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->SelectDecoderL() left with error : %d"), err);
            return;
            }
            
        // Construct a test picture header
        TVideoPictureHeader pictureHeader;
        pictureHeader.iOptions = KTestPictureHeaderOptions;
        pictureHeader.iPresentationTimestamp = KTestInputBufferTimestamp;

        TPtrC8 testInputBufferDataLitCPtr(KTestInputBufferData);
        pictureHeader.iOptional = &testInputBufferDataLitCPtr;

        // Get header information from test data unit
        TRAP(err, iMMFDevVideoPlay->ConfigureDecoderL(pictureHeader));
        if (err == KErrNotSupported)
            {
            WARN_PRINTF1(_L("WARNING - ConfigureDecoderL() gave error KErrNotSupported (-5)"));
            }
        else if(err != KErrNone)
            {
            ERR_PRINTF2(_L("ERROR - ConfigureDecoderL() gave unexpected error (%d)"), err);
            return;
            }
            
        Fsm(EDevVideoPlayInit); // Call to initialize DevVideoPlay
        }

    // Keep track on number of times this callback is called
    iEndCount++;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoPlayConfHwDev::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Configure and reconfigure the video HwDevice and play a video file >>>"));
    TInt err = KErrNone;
    iTestStepResult = EFail;
    iReConfigure = ETrue;
    iTestType = EUidTest;

    // Select the video decoder to be used (Earlier... KUidDevVideoTestDecodeHwDevice)
    INFO_PRINTF2(_L("iMMFDevVideoPlay->SelectDecoderL(0x%8x)"), iDecoderUid);
    TRAP(err, iDecoderHwDeviceId = iMMFDevVideoPlay->SelectDecoderL(iDecoderUid));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->SelectDecoderL() left with error : %d"), err);
        return EFail;
        }

    // Construct a test picture header
    TVideoPictureHeader pictureHeader;
    pictureHeader.iOptions = KTestPictureHeaderOptions;
    pictureHeader.iPresentationTimestamp = KTestInputBufferTimestamp;

    TPtrC8 testInputBufferDataLitCPtr(KTestInputBufferData);
    pictureHeader.iOptional = &testInputBufferDataLitCPtr;

    // Get header information from test data unit
    TRAP(err, iMMFDevVideoPlay->ConfigureDecoderL(pictureHeader));
    if (err == KErrNotSupported)
        {
        WARN_PRINTF1(_L("WARNING - ConfigureDecoderL() gave error KErrNotSupported (-5)"));
        }
    else if(err != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR - ConfigureDecoderL() gave unexpected error (%d)"), err);
        return EFail;
        }

    Fsm(EDevVideoPlayInit); // Call to initialize DevVideoPlay

    if (iEndCount != 2)
        {
        ERR_PRINTF2(_L("ERROR - MdvpoStreamEnd only called %d times!"), iEndCount);
        }
    else
        {
        INFO_PRINTF1(_L("InputEnd() called successfully."));
        iTestStepResult = EPass;
        }

    return iTestStepResult;
    }

//
// RTestDevVideoPlayAlloc
//

/**
 *
 * RTestDevVideoPlayAlloc - Test step constructor
 *
 */
RTestDevVideoPlayAlloc::RTestDevVideoPlayAlloc(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoPlayAlloc* RTestDevVideoPlayAlloc::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoPlayAlloc* self = new (ELeave) RTestDevVideoPlayAlloc(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }


/**
 *
 * DoTestL()
 * Executes events of DevVideoPlay in sequence
 *
 */
void RTestDevVideoPlayAlloc::DoTestL()
    {
    TInt err = KErrNone;
    iError = KErrNone;

    // Select the video decoder to be used (Earlier... KUidDevVideoTestDecodeHwDevice)
    INFO_PRINTF2(_L("iMMFDevVideoPlay->SelectDecoderL(0x%8x)"), iDecoderUid);
    TRAP(err, iDecoderHwDeviceId = iMMFDevVideoPlay->SelectDecoderL(iDecoderUid));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->SelectDecoderL() left with error : %d"), err);
        User::Leave(err);
        }

    // Set the Video Input and Output Formats for decode
    TRAP(err, err = SetInputOutputFormatsL());
    INFO_PRINTF1(_L("Set the Video Input and Output Formats for decode..."));
    if (err != KErrNone)
        {
        ERR_PRINTF1(_L("ERROR - in Setting Input / Output Formats."));
        User::Leave(err);
        }
    
    // Seek to the place we want to start reading from & initialise other callback variables
    TInt filePosition = 0;
    iFile.Seek(ESeekStart, filePosition);
    iInputEnd = EFalse;
    iTotalBytesRead = 0;
    iEndCount = 0;
    
    // Initializing the video device
    INFO_PRINTF1(_L("iMMFDevVideoPlay->Initialize()"));
    iMMFDevVideoPlay->Initialize();
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->Initialize() left with error : %d."), iError);
        User::Leave(iError);
        }

    // Start video playback, including decoding, post-processing, and rendering
    INFO_PRINTF1(_L("iMMFDevVideoPlay->Start()"));
    iMMFDevVideoPlay->Start();
    User::LeaveIfError(iError);
    
    // start the scheduler
    iSchedulerStarted = ETrue;
    CActiveScheduler::Start();
    User::LeaveIfError(iError);
    
    // Stop playback, once callback is called
    INFO_PRINTF1(_L("iMMFDevVideoPlay->Stop()"));
    iMMFDevVideoPlay->Stop();
    
    // check whether streamend has been called
    if (iEndCount != 1)
        {
        ERR_PRINTF2(_L("ERROR - MdvpoStreamEnd only called %d times!"), iEndCount);
        iTestStepResult = EFail;
        }
    else
        {
        INFO_PRINTF1(_L("InputEnd() called successfully."));
        iTestStepResult = EPass;
        }
        
    // flush any unplayed frames
    TVideoPicture* thePicture = NULL;
    do
        {
        TRAP(err, thePicture = iMMFDevVideoPlay->NextPictureL());
        if(err == KErrNone && thePicture != NULL)
            {
            iMMFDevVideoPlay->ReturnPicture(thePicture);
            }
        }
    while(thePicture != NULL);
    }

/**
 *
 * Play video by marking heap.
 *
 */
void RTestDevVideoPlayAlloc::VideoPlayAllocL()
    {
    __MM_HEAP_MARK;
    TInt startAllocCount = User::CountAllocCells();
    TInt err = KErrNone;
    
    // connect to FB session
    err = RFbsSession::Connect();
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not connect to RFbsSession. Error = %d"), err);
        User::Leave(err);
        }
        
    // create new CMMFDevVideoPlay instance
    TRAP(err,  iMMFDevVideoPlay = CMMFDevVideoPlay::NewL(*this));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not create DevVideoPlay object. Error = %d"), err);
        RFbsSession::Disconnect();
        User::Leave(err);
        }
    
    // Call to do the testing
    TRAP(err,  DoTestL());

    // cleanup CMMFDevVideoPlay instance
    delete iMMFDevVideoPlay;
    iMMFDevVideoPlay = NULL;
    RFbsSession::Disconnect();
    
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
TVerdict RTestDevVideoPlayAlloc::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Alloc test to load a video HwDevice to play a video file >>>"));
    iTestStepResult = EFail;
    iTestType = EUidTest;
    RFbsSession::Disconnect();
    
    TRAPD(err, VideoPlayAllocL());
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("Opening/playing the video file failed, unexpected error %d."), err);
        return iTestStepResult;
        }

    iTestStepResult  = EFail;

    TInt failCount = 1;
    TBool completed = EFalse;
    TBool badResult  = EFalse;
    TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
    for(;;)    
        {
        __UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
        __MM_HEAP_MARK;

        // call the test method
        TRAP(err, VideoPlayAllocL());

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
    RFbsSession::Connect();
    
    return iTestStepResult;
    }

/**
 *
 * MdvpoNewBuffers()
 * Notifies the client that one or more new empty input buffers are available.
 *
 */
void RTestDevVideoPlayAlloc::MdvpoNewBuffers()
    {
    INFO_PRINTF1(_L("RTestDevVideoPlayAlloc::MdvpoNewBuffers()."));
    TInt err = KErrNone;
    
    if(iInputEnd)
        {
        return;
        }
    TInt numBuffers = iMMFDevVideoPlay->NumFreeBuffers();
    if(numBuffers > 0) 
        {
        // keep on getting data until there is no more
        TInt fileSize;
        iFile.Size(fileSize);
        if(iTotalBytesRead >= fileSize) 
            {
            iInputEnd = ETrue;
            INFO_PRINTF1(_L("Input end"));
            iMMFDevVideoPlay->InputEnd();
            }
        else 
            {
            TVideoInputBuffer* theInputBuffer = NULL;
            TRAP(iError, theInputBuffer = iMMFDevVideoPlay->GetBufferL(KInputBufferSize));
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("iMMFDevVideoPlay->GetBufferL() left with error : %d"), iError);
                return;
                }
            
            if(theInputBuffer) 
                {
                iError = iFile.Read(theInputBuffer->iData);
                if (iError != KErrNone)
                    {
                    ERR_PRINTF2(_L("Could not read from file, error : %d"), iError);
                    return;
                    }
                iTotalBytesRead += theInputBuffer->iData.Size();
                TRAP(err, iMMFDevVideoPlay->WriteCodedDataL(theInputBuffer));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("iMMFDevVideoPlay->WriteCodedDataL() left with error : %d"), err);
                    return;
                    }
                }
            theInputBuffer->iData.Delete(0, theInputBuffer->iData.Length());
            theInputBuffer = NULL;
            }
        }
    }

/**
 *
 * MdvpoNewPictures()
 * Notifies the client that one or more new output pictures are available.
 *
 */
void RTestDevVideoPlayAlloc::MdvpoNewPictures()
    {
    INFO_PRINTF1(_L("RTestDevVideoPlayAlloc::MdvpoNewPictures()."));
    iNewPictures = ETrue;
    
    // dispose of the picture
    TVideoPicture* thePicture = NULL;
    TRAPD(err, thePicture = iMMFDevVideoPlay->NextPictureL());
    if(err == KErrNone && thePicture != NULL)
        {
        iMMFDevVideoPlay->ReturnPicture(thePicture);
        }
    }

/**
 *
 * MdvpoFatalError()
 * Reports a fatal decoding or playback error to the client.
 *
 */
void RTestDevVideoPlayAlloc::MdvpoFatalError(TInt aError)
    {
    INFO_PRINTF2(_L("RTestDevVideoPlayAlloc::MdvpoFatalError():  Error = %d"), aError);
    iError = aError;
    if(iSchedulerStarted)
        {
        iSchedulerStarted = EFalse;
        CActiveScheduler::Stop();
        }
    }

/**
 *
 * MdvpoInitComplete()
 * Reports that DevVideoPlay initialization has completed.
 *
 */
void RTestDevVideoPlayAlloc::MdvpoInitComplete(TInt aError)
    {
    INFO_PRINTF2(_L("RTestDevVideoPlayAlloc::MdvpoInitComplete():  Error = %d"), aError);
    iError = aError;
    }

/**
 *
 * MdvpoStreamEnd()
 * Reports that the input video stream end has been reached and all pictures have been processed.
 *
 */
void RTestDevVideoPlayAlloc::MdvpoStreamEnd()
    {
    INFO_PRINTF1(_L("RTestDevVideoPlayAlloc::MdvpoStreamEnd()."));
    
    // Mark that we have come to the end of the stream
    iStreamEnd = ETrue;

    // Keep track on number of times this callback is called
    iEndCount++;
    
    // Stop the scheduler
    iSchedulerStarted = EFalse;
    CActiveScheduler::Stop();
    }

//
// RTestDevVideoPlayStop
//

/**
 *
 * RTestDevVideoPlayStop - Test step constructor
 *
 */
RTestDevVideoPlayStop::RTestDevVideoPlayStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)
    {
    iTimer = NULL;
    iCheckCounter = 0;
    iNewBuffers = EFalse;
    iIsActive = EFalse;
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoPlayStop* RTestDevVideoPlayStop::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoPlayStop* self = new (ELeave) RTestDevVideoPlayStop(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }

/**
 *
 * Check()
 *
 */
TInt RTestDevVideoPlayStop::Check(TAny* aObject)
    {
    // cast, and call non-static function
    ((RTestDevVideoPlayStop*)aObject)->DoCheck();
    return 1;
    }

/**
 *
 * DoCheck()
 *
 */
void RTestDevVideoPlayStop::DoCheck()
    {
    // just toggle between start and stop
    if(!iMMFDevVideoPlay->IsPlaying()) 
        {
        INFO_PRINTF1(_L("RTestDevVideoPlayStop: Starting"));
        iMMFDevVideoPlay->Start();
        }
    else 
        {
        iCheckCounter = (iCheckCounter + 1) % 5;
        if(!iCheckCounter)
            {
            INFO_PRINTF1(_L("RTestDevVideoPlayStop: Stopping"));
            iMMFDevVideoPlay->Stop();
            }
        }
    }

/**
 *
 * Fsm()
 * Executes events of DevVideoPlay in sequence
 *
 */
void RTestDevVideoPlayStop::Fsm(TMmfDevVideoPlayEvents aDevVideoPlayEvent)
    {
    switch (aDevVideoPlayEvent)
        {
        case EDevVideoPlayInit:
            {
            // Select the video decoder to be used (Earlier... KUidDevVideoTestDecodeHwDevice)
            INFO_PRINTF2(_L("iMMFDevVideoPlay->SelectDecoderL(0x%8x)"), iDecoderUid);
            TRAPD(err, iDecoderHwDeviceId = iMMFDevVideoPlay->SelectDecoderL(iDecoderUid));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->SelectDecoderL() left with error : %d"), err);
                return;
                }

            // Set the Video Input and Output Formats for decode
            TRAP(err, err = SetInputOutputFormatsL());
            if (err != KErrNone)
                {
                ERR_PRINTF1(_L("ERROR - in Setting Input / Output Formats."));
                return;
                }
                
            // Initializing the video device
            INFO_PRINTF1(_L("iMMFDevVideoPlay->Initialize()"));
            iMMFDevVideoPlay->Initialize();
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->Initialize() left with error : %d."), iError);
                return;
                }
            
            // Start video playback, including decoding, post-processing, and rendering
            INFO_PRINTF1(_L("iMMFDevVideoPlay->Start()"));
            iMMFDevVideoPlay->Start();
            
            // start up a timer to check the player state
            TRAP(err, iTimer = CPeriodic::NewL(EPriorityHigh));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - CPeriodic::NewL() returned error : %d."), err);
                return;
                }
            iTimer->Start(100000, 100000, TCallBack(Check, this));
            
            // check for new buffers and wait for other callbacks
            if(iNewBuffers) 
                {
                iNewBuffers = EFalse;
                iIsActive = ETrue;
                CActiveScheduler::Start();
                }

            // Call stop
            INFO_PRINTF1(_L("iMMFDevVideoPlay->Stop()"));
            iMMFDevVideoPlay->Stop();
            
            // delete the iTimer
            iTimer->Cancel();
            delete iTimer;
            
            // check for errors
            if(!iError)
                {
                INFO_PRINTF1(_L("No Errors..."));
                iTestStepResult = EPass;
                }
            
            // flush any unplayed frames
            TVideoPicture* thePicture = NULL;
            do
                {
                TRAP(err, thePicture = iMMFDevVideoPlay->NextPictureL());
                if(err == KErrNone && thePicture != NULL)
                    {
                    iMMFDevVideoPlay->ReturnPicture(thePicture);
                    }
                }
            while(thePicture != NULL);
            
            break;
            }

        default:
            {
            INFO_PRINTF1(_L("Invalid DevVideoPlay event!"));
            }
        }
    }

/**
 *
 * MdvpoNewBuffers()
 * Notifies the client that one or more new empty input buffers are available.
 *
 */
void RTestDevVideoPlayStop::MdvpoNewBuffers()
    {
    INFO_PRINTF1(_L("RTestDevVideoPlayStop::MdvpoNewBuffers()"));
    TInt err = KErrNone;
    
    TInt numBuffers = iMMFDevVideoPlay->NumFreeBuffers();
    if(numBuffers > 0)
        {
        iNewBuffers = ETrue;
        if(!iIsActive)
            {
            // get a buffer
            TVideoInputBuffer* theInputBuffer = NULL;
            TRAP(iError, theInputBuffer = iMMFDevVideoPlay->GetBufferL(KInputBufferSize));
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("iMMFDevVideoPlay->GetBufferL() left with error : %d"), iError);
                return;
                }
            
            if(theInputBuffer) 
                {
                // fill it from our file
                iError = iFile.Read(theInputBuffer->iData, theInputBuffer->iData.Size());
                if (iError != KErrNone)
                    {
                    ERR_PRINTF2(_L("Could not read from file, error : %d"), iError);
                    return;
                    }
                
                // write coded data
                TRAP(err, iMMFDevVideoPlay->WriteCodedDataL(theInputBuffer));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("iMMFDevVideoPlay->WriteCodedDataL() left with error : %d"), err);
                    return;
                    }
                }
            }
        else 
            {
            iMMFDevVideoPlay->InputEnd();
            CActiveScheduler::Stop();    
            iIsActive = EFalse;
            }
        }
    }

/**
 *
 * MdvpoNewPictures()
 * Notifies the client that one or more new output pictures are available.
 *
 */
void RTestDevVideoPlayStop::MdvpoNewPictures()
    {
    INFO_PRINTF1(_L("RTestDevVideoPlayStop::MdvpoNewPictures()"));
    
    // flag an error if this comes in at any time we're not playing -
    if(!iMMFDevVideoPlay->IsPlaying())
        {
        iError = ETrue;
        }
    
    // dispose of the picture
    TVideoPicture* thePicture = NULL;
    TRAPD(err, thePicture = iMMFDevVideoPlay->NextPictureL());
    if(err == KErrNone && thePicture != NULL)
        {
        iMMFDevVideoPlay->ReturnPicture(thePicture);
        }
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoPlayStop::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Play/stop playing a video file >>>"));
    iTestStepResult = EFail;

    Fsm(EDevVideoPlayInit); // Call to initialize DevVideoPlay

    return iTestStepResult;
    }

//
// RTestDevVideoPlayPause
//

/**
 *
 * RTestDevVideoPlayPause - Test step constructor
 *
 */
RTestDevVideoPlayPause::RTestDevVideoPlayPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)
    {
    iTimer = NULL;
    iCheckCounter = 0;
    iNewBuffers = EFalse;
    iIsActive = EFalse;
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoPlayPause* RTestDevVideoPlayPause::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoPlayPause* self = new (ELeave) RTestDevVideoPlayPause(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }

/**
 *
 * Check()
 *
 */
TInt RTestDevVideoPlayPause::Check(TAny* aObject)
    {
    // cast, and call non-static function
    ((RTestDevVideoPlayPause*)aObject)->DoCheck();
    return 1;
    }

/**
 *
 * DoCheck()
 *
 */
void RTestDevVideoPlayPause::DoCheck()
    {
    // just toggle between pause and resume
    if(!iMMFDevVideoPlay->IsPlaying()) 
        {
        INFO_PRINTF1(_L("RTestDevVideoPlayPause: Resuming"));
        iMMFDevVideoPlay->Resume();
        }
    else 
        {
        iCheckCounter = (iCheckCounter + 1) % 5;
        if(!iCheckCounter)
            {
            INFO_PRINTF1(_L("RTestDevVideoPlayPause: Pausing"));
            iMMFDevVideoPlay->Pause();
            }
        }
    }

/**
 *
 * Fsm()
 * Executes events of DevVideoPlay in sequence
 *
 */
void RTestDevVideoPlayPause::Fsm(TMmfDevVideoPlayEvents aDevVideoPlayEvent)
    {
    switch (aDevVideoPlayEvent)
        {
        case EDevVideoPlayInit:
            {
            // Select the video decoder to be used (Earlier... KUidDevVideoTestDecodeHwDevice)
            INFO_PRINTF2(_L("iMMFDevVideoPlay->SelectDecoderL(0x%8x)"), iDecoderUid);
            TRAPD(err, iDecoderHwDeviceId = iMMFDevVideoPlay->SelectDecoderL(iDecoderUid));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->SelectDecoderL() left with error : %d"), err);
                return;
                }

            // Set the Video Input and Output Formats for decode
            TRAP(err, err = SetInputOutputFormatsL());
            if (err != KErrNone)
                {
                ERR_PRINTF1(_L("ERROR - in Setting Input / Output Formats."));
                return;
                }
                
            // Initializing the video device
            INFO_PRINTF1(_L("iMMFDevVideoPlay->Initialize()"));
            iMMFDevVideoPlay->Initialize();
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->Initialize() left with error : %d."), iError);
                return;
                }
            
            // Start video playback, including decoding, post-processing, and rendering
            INFO_PRINTF1(_L("iMMFDevVideoPlay->Start()"));
            iMMFDevVideoPlay->Start();
            
            // start up a timer to check the player state
            TRAP(err, iTimer = CPeriodic::NewL(EPriorityHigh));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ERROR - CPeriodic::NewL() returned error : %d."), err);
                return;
                }
            iTimer->Start(100000, 100000, TCallBack(Check, this));
            
            // check for new buffers and wait for other callbacks
            if(iNewBuffers) 
                {
                iNewBuffers = EFalse;
                iIsActive = ETrue;
                CActiveScheduler::Start();
                }

            // Call stop
            INFO_PRINTF1(_L("iMMFDevVideoPlay->Stop()"));
            iMMFDevVideoPlay->Stop();
            
            // delete the iTimer
            iTimer->Cancel();
            delete iTimer;
            
            // check for errors
            if(!iError)
                {
                INFO_PRINTF1(_L("No Errors..."));
                iTestStepResult = EPass;
                }
            
            // flush any unplayed frames
            TVideoPicture* thePicture = NULL;
            do
                {
                TRAP(err, thePicture = iMMFDevVideoPlay->NextPictureL());
                if(err == KErrNone && thePicture != NULL)
                    {
                    iMMFDevVideoPlay->ReturnPicture(thePicture);
                    }
                }
            while(thePicture != NULL);
            
            break;
            }

        default:
            {
            INFO_PRINTF1(_L("Invalid DevVideoPlay event!"));
            }
        }
    }

/**
 *
 * MdvpoNewBuffers()
 * Notifies the client that one or more new empty input buffers are available.
 *
 */
void RTestDevVideoPlayPause::MdvpoNewBuffers()
    {
    INFO_PRINTF1(_L("RTestDevVideoPlayPause::MdvpoNewBuffers()"));
    TInt err = KErrNone;
    
    TInt numBuffers = iMMFDevVideoPlay->NumFreeBuffers();
    if(numBuffers > 0) 
        {
        iNewBuffers = ETrue;
        if(!iIsActive) 
            {
            // get a buffer
            TVideoInputBuffer* theInputBuffer = NULL;
            TRAP(iError, theInputBuffer = iMMFDevVideoPlay->GetBufferL(KInputBufferSize));
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("iMMFDevVideoPlay->GetBufferL() left with error : %d"), iError);
                return;
                }
            
            if(theInputBuffer) 
                {
                // fill it from our file
                iError = iFile.Read(theInputBuffer->iData, theInputBuffer->iData.Size());
                if (iError != KErrNone)
                    {
                    ERR_PRINTF2(_L("Could not read from file, error : %d"), iError);
                    return;
                    }
                
                // write coded data
                TRAP(err, iMMFDevVideoPlay->WriteCodedDataL(theInputBuffer));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("iMMFDevVideoPlay->WriteCodedDataL() left with error : %d"), err);
                    return;
                    }
                }
            }
        else 
            {
            iMMFDevVideoPlay->InputEnd();
            CActiveScheduler::Stop();    
            iIsActive = EFalse;
            }
        }
    }

/**
 *
 * MdvpoNewPictures()
 * Notifies the client that one or more new output pictures are available.
 *
 */
void RTestDevVideoPlayPause::MdvpoNewPictures()
    {
    INFO_PRINTF1(_L("RTestDevVideoPlayPause::MdvpoNewPictures()"));
    
    // flag an error if this comes in at any time we're not playing -
    if(!iMMFDevVideoPlay->IsPlaying()) 
        {
        iError = ETrue;
        }
    
    // dispose of the picture
    TVideoPicture* thePicture = NULL;
    TRAPD(err, thePicture = iMMFDevVideoPlay->NextPictureL());
    if(err == KErrNone && thePicture != NULL)
        {
        iMMFDevVideoPlay->ReturnPicture(thePicture);
        }
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoPlayPause::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Play/Pause playing a video file >>>"));
    iTestStepResult = EFail;

    Fsm(EDevVideoPlayInit); // Call to initialize DevVideoPlay

    return iTestStepResult;
    }


//
// RTestDevVideoPlayDecodePlaybackPos
//

/**
 *
 * RTestDevVideoPlayDecodePlaybackPos - Test step constructor
 *
 */
RTestDevVideoPlayDecodePlaybackPos::RTestDevVideoPlayDecodePlaybackPos(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)
    {
    iInitDecodingPos = 0;
    iInitPlaybackPos = 0;
    iFinalDecodingPos = 0;
    iFinalPlaybackPos = 0;
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoPlayDecodePlaybackPos* RTestDevVideoPlayDecodePlaybackPos::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoPlayDecodePlaybackPos* self = new (ELeave) RTestDevVideoPlayDecodePlaybackPos(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoPlayDecodePlaybackPos::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Get the decoding and playback position while playing a video file >>>"));
    iTestStepResult = EPass; // Assume success
    TInt err = KErrNone;

    // Select the video decoder to be used (Earlier... KUidDevVideoTestDecodeHwDevice)
    INFO_PRINTF2(_L("iMMFDevVideoPlay->SelectDecoderL(0x%8x)"), iDecoderUid);
    TRAP(err, iDecoderHwDeviceId = iMMFDevVideoPlay->SelectDecoderL(iDecoderUid));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->SelectDecoderL() left with error : %d"), err);
        return EFail;
        }

    // Set the Video Input and Output Formats for decode
    TRAP(err, err = SetInputOutputFormatsL());
    if (err != KErrNone)
        {
        ERR_PRINTF1(_L("ERROR - in Setting Input / Output Formats."));
        return EFail;
        }
    
    // Initialize CDevVideoPlay and wait for response from MdvpoInitComplete callback
    INFO_PRINTF1(_L("iMMFDevVideoPlay->Initialize()"));
    iMMFDevVideoPlay->Initialize();
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->Initialize() left with error : %d."), iError);
        return EFail;
        }

    // Start video playback, including decoding, post-processing, and rendering
    INFO_PRINTF1(_L("iMMFDevVideoPlay->Start()"));
    iMMFDevVideoPlay->Start();
    
    // Start the scheduler
    CActiveScheduler::Start();
    
    // Stop playback, once callback is called...
    INFO_PRINTF1(_L("iMMFDevVideoPlay->Stop()"));
    iMMFDevVideoPlay->Stop();
    
    // Verify the Initial Decoding and Playback positions
    // NB - initially, the playback would be running a few frames behind decoding, 
    // so test assumes PASS if initial playback position is less than or equal to initial decoding position
    // NB - after stream end the playback position should be equal to the decoding position
    // so test assumes PASS if final playback position is equal to final decoding position
    if((iInitDecodingPos >= iInitPlaybackPos) && (iFinalDecodingPos == iFinalPlaybackPos))
        {
        INFO_PRINTF1(_L("INFO - (iInitDecodingPos >= iInitPlaybackPos) && (iFinalDecodingPos == iFinalPlaybackPos)"));
        iTestStepResult = EPass;
        }
    else
        {
        ERR_PRINTF1(_L("ERROR - (iInitDecodingPos < iInitPlaybackPos) || (iFinalDecodingPos != iFinalPlaybackPos)"));
        iTestStepResult = EFail;
        }
    
    // flush any unplayed frames
    TVideoPicture* thePicture = NULL;
    do
        {
        TRAP(err, thePicture = iMMFDevVideoPlay->NextPictureL());
        if(err == KErrNone && thePicture != NULL)
            {
            iMMFDevVideoPlay->ReturnPicture(thePicture);
            }
        }
    while(thePicture != NULL);
    
    // terminate
    return iTestStepResult;
    }

/**
 *
 * MdvpoNewPictures()
 * Notifies the client that one or more new output pictures are available.
 *
 */
void RTestDevVideoPlayDecodePlaybackPos::MdvpoNewPictures()
    {
    // check playback position - we know this file runs at 10 fps
    // so it should be 100000 for the first frame
    iNewPictures = ETrue;
    TVideoPicture* thePicture = NULL;
    TRAPD(err, thePicture = iMMFDevVideoPlay->NextPictureL());
    if(err == KErrNone && thePicture != NULL)
        {
        // Get the initial decoding and playback position
        TTimeIntervalMicroSeconds thePos = iMMFDevVideoPlay->DecodingPosition();
        if(iInitDecodingPos == 0)
            {
            INFO_PRINTF2(_L("Initial DecodingPosition = %d"), I64INT(thePos.Int64()));
            iInitDecodingPos = thePos;
            }
        TTimeIntervalMicroSeconds thePos1 = iMMFDevVideoPlay->PlaybackPosition();
        if(iInitPlaybackPos == 0)
            {
            INFO_PRINTF2(_L("Initial PlaybackPosition = %d"), I64INT(thePos1.Int64()));
            iInitPlaybackPos = thePos1;
            }
        iMMFDevVideoPlay->ReturnPicture(thePicture);
        }
    
    }

/**
 *
 * MdvpoStreamEnd()
 * Reports that the input video stream end has been reached and all pictures have been processed.
 *
 */
void RTestDevVideoPlayDecodePlaybackPos::MdvpoStreamEnd()
    {
    INFO_PRINTF1(_L("RTestDevVideoPlayDecodePlaybackPos::MdvpoStreamEnd()."));

    // Mark that we have come to the end of the stream
    iStreamEnd = ETrue;

    // Keep track on number of times this callback is called
    iEndCount++;
    
    // Get the final decoding and playback position
    TTimeIntervalMicroSeconds thePos = iMMFDevVideoPlay->DecodingPosition();
    INFO_PRINTF2(_L("Final DecodingPosition = %d"), I64INT(thePos.Int64()));
    iFinalDecodingPos = thePos;
    TTimeIntervalMicroSeconds thePos1 = iMMFDevVideoPlay->PlaybackPosition();
    INFO_PRINTF2(_L("Final PlaybackPosition = %d"), I64INT(thePos1.Int64()));
    iFinalPlaybackPos = thePos1;
    
    // Stop the scheduler
    CActiveScheduler::Stop();
    }

//
// RTestDevVideoPlayDecoderInfo
//

/**
 *
 * RTestDevVideoPlayDecoderInfo - Test step constructor
 *
 */
RTestDevVideoPlayDecoderInfo::RTestDevVideoPlayDecoderInfo(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoPlayDecoderInfo* RTestDevVideoPlayDecoderInfo::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoPlayDecoderInfo* self = new (ELeave) RTestDevVideoPlayDecoderInfo(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }

/**
 *
 * ValidDecoderInfo()
 *
 */
TBool RTestDevVideoPlayDecoderInfo::ValidDecoderInfo(CVideoDecoderInfo* aInfo)
    {
    if (!aInfo) 
        {
        ERR_PRINTF1(_L("ERROR - Decoder Info is NULL!"));
        return EFalse;
        }

    // check the decoder against test data
    // need to check every call so as to keep code coverage
    // and prevent a situation where if the first test fails
    // then subsequent tests will not be called
    TBool ret = ETrue;

    // UID check
    if (aInfo->Uid() != iDecoderUid)
        {
        ERR_PRINTF2(_L("ERROR - Decoder UID '0x%8x' is incorrect!"), aInfo->Uid());
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF2(_L("INFO - Decoder UID '0x%8x' is correct!"), iDecoderUid);
        }

    // Manufacturer check
    if (!(aInfo->Manufacturer() == KXvidDecoderInfoManufacturer))
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
    if ( (decVer.iMajor != KXvidDecoderInfoVersionMaj) ||
         (decVer.iMinor != KXvidDecoderInfoVersionMin) ||
         (decVer.iBuild != KXvidDecoderInfoVersionBuild) )
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
    if (!aInfo->SupportsDirectDisplay())
        {
        ERR_PRINTF1(_L("ERROR - Supports Direct Display = EFalse!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Supports Direct Display = ETrue!"));
        }

    // Max picture size
    TSize maxSize = aInfo->MaxPictureSize();
    if (maxSize.iWidth != KXvidDecoderInfoMaxPicSizeX || maxSize.iHeight != KXvidDecoderInfoMaxPicSizeY)
        {
        ERR_PRINTF1(_L("ERROR - Max Picture Size is wrong!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Max Picture Size is correct!"));
        }

    // Max Bitrate
    if (aInfo->MaxBitrate() != KMaxTUint)
        {
        ERR_PRINTF1(_L("ERROR - Max Bitrate is wrong!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Max Bitrate is correct!"));
        }

    // Supports picture loss
    if (aInfo->SupportsPictureLoss())
        {
        ERR_PRINTF1(_L("ERROR - Supports Picture Loss = ETrue!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Supports Picture Loss = EFalse!"));
        }

    // Supports slice loss
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
    if (!(aInfo->CodingStandardSpecificInfo() == KXvidDecoderInfoCSInfo))
        {
        ERR_PRINTF1(_L("ERROR - Coding standard specific info is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Coding standard specific info is correct!"));
        }

    // Implementation info
    if (!(aInfo->ImplementationSpecificInfo() == KXvidDecoderInfoISInfo))
        {
        ERR_PRINTF1(_L("ERROR - Implementation specific info is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Implementation specific info is correct!"));
        }
    
    // Test supported max picture rates
    const RArray<TPictureRateAndSize>& maxRates = aInfo->MaxPictureRates();

    // Test supported max picture rates
    if (!(maxRates[0].iPictureSize == TSize(KXvidDecoderInfoMaxPicSizeX, KXvidDecoderInfoMaxPicSizeY)) || 
        !(maxRates[0].iPictureRate == KXvidDecoderInfoMaxPicRate) )
        {
        ERR_PRINTF1(_L("ERROR - Max Picture size & rate is incorrect!"));
        ret = EFalse;
        }
    else
        {
        INFO_PRINTF1(_L("INFO - Max Picture size & rate is correct!"));
        }
    
    // Get decoder supported formats array
    const RPointerArray<CCompressedVideoFormat>& vidFormats = aInfo->SupportedFormats();
    
    // Get the expected Test formats supported for Hw Device and check if XviD CV Format is supported
    CCompressedVideoFormat* testFormat = NULL;
    TInt err = KErrNone;
    if (iDecoderUid == KUidTestVideoPuXvidDecoderDevice)
    	{
    	TRAP(err, testFormat = GetTestCVFormatL(KXvidDecoderMimeType));	
    	}
    else
    	{
    	TRAP(err, testFormat = GetTestCVFormatL(KTestDecoderMimeType));	
    	}
    
    if (err != KErrNone)
        {
        ERR_PRINTF1(_L("ERROR - Failed to construct CCompressedVideoFormat for mimetype!"));
        ret = EFalse;
        }
    else if ( !(*testFormat == *(vidFormats[0])) || !(aInfo->SupportsFormat(*testFormat)) )
        {
        ERR_PRINTF1(_L("ERROR - SupportsFormat returned EFalse!"));
        ret = EFalse;
        }
    delete testFormat;
    testFormat=NULL;

    //Content Protection	
    if (aInfo->SupportsContentProtected())//Check if flag is set indicating Content Protection is supported.
	{
	ERR_PRINTF1(_L("Error - Supports Content Protection = ETrue!"));
	ret = EFalse;
	}
    else
        {
        INFO_PRINTF1(_L("INFO - Supports Content Protected = EFalse!"));
        }

    // Finally check a CV format which isn't supposed to be supported
    TRAP(err, testFormat = GetTestCVFormatL(_L8("video/unsupported")));
    if (err != KErrNone)
        {
        ERR_PRINTF1(_L("ERROR - Failed to construct CCompressedVideoFormat for un-supported mimetype!"));
        ret = EFalse;
        }
    else if (aInfo->SupportsFormat(*testFormat))
        {
        ERR_PRINTF1(_L("ERROR - SupportsFormat(un-supported) returned ETrue!"));
        ret = EFalse;
        }
    delete testFormat;
    testFormat=NULL;

    return ret;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoPlayDecoderInfo::DoTestStepL()
    {
    INFO_PRINTF2(_L("<<< Get information about the '%S' decoder >>>"), &iSectName);
    iTestStepResult = EPass; // Assume success
    TBool valid = EFalse;

    // Get decoder information from XviD decoder plugin
    CVideoDecoderInfo* videoDecoderInfo = NULL;
    INFO_PRINTF2(_L("iMMFDevVideoPlay->VideoDecoderInfoLC(0x%8x)"), iDecoderUid);
    videoDecoderInfo = iMMFDevVideoPlay->VideoDecoderInfoLC(iDecoderUid);
    valid = ValidDecoderInfo(videoDecoderInfo);
    CleanupStack::PopAndDestroy(videoDecoderInfo);
                
    if (!valid) // Check if the Video Decoder Info class is valid
        {
        ERR_PRINTF1(_L("CVideoDecoderInfo class is corrupt!"));
        iTestStepResult = EFail;
        }
    else
        {
        INFO_PRINTF1(_L("VideoDecoderInfoLC() passed all checks successfully!"));
        }

    return iTestStepResult;
    }

//
// Negative Tests //
//

//
// RTestDevVideoPlayUidHwDev
//

/**
 *
 * RTestDevVideoPlayUidHwDev - Test step constructor
 *
 */
RTestDevVideoPlayUidHwDev::RTestDevVideoPlayUidHwDev(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoPlayUidHwDev* RTestDevVideoPlayUidHwDev::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoPlayUidHwDev* self = new (ELeave) RTestDevVideoPlayUidHwDev(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoPlayUidHwDev::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Try to Play a video file specifying invalid Uid of video HwDevice. >>>"));
    iTestStepResult = EFail; // Assume test will fail
    iTestType = EInvalidUidTest; // Test uses Invalid Uid for initialization
    
    Fsm(EDevVideoPlayInit); // Call to initialize DevVideoPlay

    return iTestStepResult;
    }

//
// RTestDevVideoPlayCorruptFile
//

/**
 *
 * RTestDevVideoPlayCorruptFile - Test step constructor
 *
 */
RTestDevVideoPlayCorruptFile::RTestDevVideoPlayCorruptFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoPlayCorruptFile* RTestDevVideoPlayCorruptFile::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoPlayCorruptFile* self = new (ELeave) RTestDevVideoPlayCorruptFile(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoPlayCorruptFile::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Try to Play a corrupted video file. >>>"));
    iTestStepResult = EFail;
    iTestType = EUidTest;
    
    Fsm(EDevVideoPlayInit); // Call to initialize DevVideoPlay
    if (iError == KErrNotSupported)
        {
        iTestStepResult = EPass;
        }
    return iTestStepResult;
    }

//
// RTestDevVideoPlayWithoutVideoType
//

/**
 *
 * RTestDevVideoPlayWithoutVideoType - Test step constructor
 *
 */
RTestDevVideoPlayWithoutVideoType::RTestDevVideoPlayWithoutVideoType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    :RTestStepDevVideoPlayBase(aTestName, aSectName, aKeyName, aDecoderUid)
    {
    }

/**
 *
 * NewL()
 *
 */
RTestDevVideoPlayWithoutVideoType* RTestDevVideoPlayWithoutVideoType::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid)
    {
    RTestDevVideoPlayWithoutVideoType* self = new (ELeave) RTestDevVideoPlayWithoutVideoType(aTestName, aSectName, aKeyName, aDecoderUid);
    return self;
    }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 */
TVerdict RTestDevVideoPlayWithoutVideoType::DoTestStepL()
    {
    INFO_PRINTF1(_L("<<< Open and play video file without specifying input / output video  types. >>>"));
    iTestStepResult = EFail; // Assume test will fail
    iTestType = EUidTest;
    TInt err = KErrNone;

    // Select the video decoder to be used (Earlier... KUidDevVideoTestDecodeHwDevice)
    INFO_PRINTF2(_L("iMMFDevVideoPlay->SelectDecoderL(0x%8x)"), iDecoderUid);
    TRAP(err, iDecoderHwDeviceId = iMMFDevVideoPlay->SelectDecoderL(iDecoderUid));
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("ERROR - iMMFDevVideoPlay->SelectDecoderL() left with error : %d"), err);
        return EFail;
        }

    // This test case does not set the Input / Output format for test.
    // DO NOT Set the Video Input and Output Formats for decode

    // Initializing the video device
    INFO_PRINTF1(_L("iMMFDevVideoPlay->Initialize()"));
    iMMFDevVideoPlay->Initialize();
    if (iError == KErrNotReady)
        {
        INFO_PRINTF1(_L("INFO - couldn't initialize DevVideoPlay (KErrNotReady)."));
        iTestStepResult = EPass;
        }

    return iTestStepResult;
    }

