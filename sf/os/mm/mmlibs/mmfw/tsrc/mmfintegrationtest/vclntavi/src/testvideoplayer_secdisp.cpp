// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testvideoplayer_secdisp.h"

//
// RTestVclntSetScrIdAndPlayBase
//

/**
 * RTestVclntSetScrIdAndPlayBase::Constructor
 */
RTestVclntSetScrIdAndPlayBase::RTestVclntSetScrIdAndPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    : RTestVclntAviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError)
    {
    iHeapSize = 1000000; //1MB
    iScreenNumber = aScreenNumber;
    iCurrentScreenNumber = 0;
    }
    
/**
 *
 * Finished()
 *
 */
void RTestVclntSetScrIdAndPlayBase::FinishedL()
    {
    INFO_PRINTF1(_L("RTestVclntSetScrIdAndPlayBase::FinishedL"));
    //overridden in play/stop and play/pause tests.
    }
    
/**
 * RTestVclntAviPlayerStep::MvpuoOpenComplete
 * Callback function for CVideoPlayerUtility
 */
void RTestVclntSetScrIdAndPlayBase::MvpuoOpenComplete(TInt aError)
    {
    iError = aError;
    INFO_PRINTF2(_L("iVideoPlayer - Open Complete callback returned with error %d"), iError);
#ifdef SYMBIAN_VARIABLE_BITRATE_CODEC
#if defined (__EABI__) || defined(__X86GCC__) // secondary screen is not supported in HW.
    if ((iCurrentScreenNumber != 0) && (iError == KErrNotSupported))
        {
        iTestStepResult = EKnownFailure;
        INFO_PRINTF1(_L("The secondary screen is not supported in HW"));
        CActiveScheduler::Stop();
        }
    else if((iCurrentScreenNumber == 0) && (iError == KErrNone))
        {
        TRAPD(err, FsmL(EVPOpenComplete));
        if (err != KErrNone)
            {
            iError = err;    
            }
        }
    else
        {
        iTestStepResult = EFail;
        ERR_PRINTF2(_L("Unexpected error in MvpuoOpenComplete %d"), iError);
        CActiveScheduler::Stop();
        }
#else
    TRAPD(err, FsmL(EVPOpenComplete));
    if (err != KErrNone)
        {
        iError = err;
        CActiveScheduler::Stop();
        }
#endif  // __EABI__ 
#else
    if(iError == KErrNotSupported)
        {
        iTestStepResult = EKnownFailure;
        INFO_PRINTF1(_L("The AVI Play Controller is not available in this OS version, hence KNOWN FAILURE"));
        }
    else
        {
        iTestStepResult = EFail;
        ERR_PRINTF2(_L("Unexpected error in MvpuoOpenComplete %d"), iError);
        }
    CActiveScheduler::Stop();
#endif // SYMBIAN_VARIABLE_BITRATE_CODEC
    }    
//
// RTestVclntSetScrIdAndPlayAviFile
//

/**
 * RTestVclntSetScrIdAndPlayAviFile::Constructor
 */
RTestVclntSetScrIdAndPlayAviFile::RTestVclntSetScrIdAndPlayAviFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    : RTestVclntSetScrIdAndPlayBase(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber)
    {
    }
   
/**
 * RTestVclntSetScrIdAndPlayAviFile::NewL
 */
RTestVclntSetScrIdAndPlayAviFile* RTestVclntSetScrIdAndPlayAviFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    {
    RTestVclntSetScrIdAndPlayAviFile* self = new (ELeave) RTestVclntSetScrIdAndPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }

/**
 * RTestVclntSetScrIdAndPlayAviFile::DoTestStepL
 */
TVerdict RTestVclntSetScrIdAndPlayAviFile::DoTestStepL()
    {
    iTestStepResult = EFail;
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
    return iTestStepResult;
    }

/**
 * RTestVclntSetScrIdAndPlayAviFile::FsmL
 */
void RTestVclntSetScrIdAndPlayAviFile::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                {
                // Set iScreenNumber
                iCurrentScreenNumber = iScreenNumber;
                INFO_PRINTF2(_L("iVideoPlayer->SetInitScreenNumber(%d)"), iScreenNumber);
                TInt err = KErrNone;
                err = iVideoPlayer->SetInitScreenNumber(iScreenNumber);
            #ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("ScreenNumber failed with error : %d"), err);
                    User::Leave(err);
                    }
            #else
                if (err == KErrNotSupported)
                    {
                    iTestStepResult = EKnownFailure;
                    INFO_PRINTF2(_L("SetInitScreenNumber() is not supported in this OS, returned %d"), err);
                    }
                else
                    {
                    iTestStepResult = EFail;
                    ERR_PRINTF2(_L("SetInitScreenNumber() failed with error : %d"), err);
                    }
                User::Leave(err);
            #endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT                                        
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                if (iBinaryCompatibility)
                	{
                	iVideoPlayer->OpenFileL(iFilename, ControllerUid());
                	}
                else
                	{
                	iVideoPlayer->OpenFileL(iFilename);
                	}
                
                PrepareState(EVPOpenComplete, KErrNone);
                break;
                }
            case EVPOpenComplete:
                {
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
                }
            case EVPPrepareComplete:
                {
                iError = KErrTimedOut;
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                PrepareState(EVPPlayComplete, KErrNone);
                break;
                }
            case EVPPlayComplete:
                {
                iTestStepResult = EPass;
                CActiveScheduler::Stop();
                break;
                }
            }
        }
    }

//
// RTestVclntSetScrIdAndPlayAviDes
//

/**
 * RTestVclntSetScrIdAndPlayAviDes::Constructor
 */
RTestVclntSetScrIdAndPlayAviDes::RTestVclntSetScrIdAndPlayAviDes(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    : RTestVclntSetScrIdAndPlayBase(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber)
    {
    iHeapSize = 500000;
    }

/**
 * RTestVclntSetScrIdAndPlayAviDes::NewL
 */
RTestVclntSetScrIdAndPlayAviDes* RTestVclntSetScrIdAndPlayAviDes::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    {
    RTestVclntSetScrIdAndPlayAviDes* self = new (ELeave) RTestVclntSetScrIdAndPlayAviDes(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber);
    return self;
    }

/**
 * RTestVclntSetScrIdAndPlayAviDes::DoTestStepPreambleL
 */
TVerdict  RTestVclntSetScrIdAndPlayAviDes::DoTestStepPreambleL()
    {
    TVerdict preamRes = EPass;
    
    // Base DoTestStepPreambleL
    preamRes = RTestVclntAviPlayerStep::DoTestStepPreambleL();
    if (preamRes != EPass)
        {
        return preamRes;
        }
    
    RFs fs;
    RFile file;
    TInt size = 0;

    // connect to file system and open file
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    User::LeaveIfError(file.Open(fs,iFilename,EFileRead));
    CleanupClosePushL(file);

    // Set HBuf size
    User::LeaveIfError(file.Size(size));
    INFO_PRINTF2(_L("size of file = %d\n"),size);//Statement Changed under DEF105143

    iVideo = HBufC8::NewMaxL(size);

    // read data into Hbuf
    TPtr8 bufferDes(iVideo->Des());
    User::LeaveIfError(file.Read(bufferDes));
    
    CleanupStack::PopAndDestroy(2, &fs); //fs, file
    
    return preamRes;
    }

/**
 * RTestVclntSetScrIdAndPlayAviDes::DoTestStepPostambleL
 */
TVerdict RTestVclntSetScrIdAndPlayAviDes::DoTestStepPostambleL()
    {
    delete iVideo;
    iVideo = NULL;
    return RTestVclntAviPlayerStep::DoTestStepPostambleL();
    }

/**
 * Load and initialise an video descriptor.
 */
TVerdict RTestVclntSetScrIdAndPlayAviDes::DoTestStepL()
    {
    iTestStepResult = EFail;
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
    return iTestStepResult;
    }

/**
 * RTestVclntSetScrIdAndPlayAviDes::FsmL
 */
void RTestVclntSetScrIdAndPlayAviDes::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                {
                // Set iScreenNumber
                iCurrentScreenNumber = iScreenNumber;
                INFO_PRINTF2(_L("iVideoPlayer->SetInitScreenNumber(%d)"), iScreenNumber);
                TInt err = KErrNone;
                err = iVideoPlayer->SetInitScreenNumber(iScreenNumber);
            #ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("ScreenNumber failed with error : %d"), err);
                    User::Leave(err);
                    }
            #else
                if (err == KErrNotSupported)
                    {
                    iTestStepResult = EKnownFailure;
                    INFO_PRINTF2(_L("SetInitScreenNumber() is not supported in this OS, returned %d"), err);
                    }
                else
                    {
                    iTestStepResult = EFail;
                    ERR_PRINTF2(_L("SetInitScreenNumber() failed with error : %d"), err);
                    }
                User::Leave(err);
            #endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenDesL() %S"), &iFilename);
                iVideoPlayer->OpenDesL(iVideo->Des());
                PrepareState(EVPOpenComplete, KErrNone);
                break;
                }
            case EVPOpenComplete:
                {
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
                }
            case EVPPrepareComplete:
                {
                iError = KErrTimedOut;
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                PrepareState(EVPPlayComplete, KErrNone);
                break;
                }
            case EVPPlayComplete:
                {
                iTestStepResult = EPass;
                CActiveScheduler::Stop();
                break;
                }
            }
        }
    }

//
// RTestVclntSetScrIdAndPlayAviUrl
//

/**
 * RTestVclntSetScrIdAndPlayAviUrl::Constructor
 */
 
RTestVclntSetScrIdAndPlayAviUrl::RTestVclntSetScrIdAndPlayAviUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    : RTestVclntSetScrIdAndPlayBase(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber)
    {
    }

/**
 * RTestVclntSetScrIdAndPlayAviUrl::NewL
 */

RTestVclntSetScrIdAndPlayAviUrl* RTestVclntSetScrIdAndPlayAviUrl::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    {
    RTestVclntSetScrIdAndPlayAviUrl* self = new (ELeave) RTestVclntSetScrIdAndPlayAviUrl(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber);
    return self;
    }

/**
 * RTestVclntSetScrIdAndPlayAviUrl::Constructor
 */

TVerdict RTestVclntSetScrIdAndPlayAviUrl::DoTestStepL()
    {
    iTestStepResult = EFail;
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
    return iTestStepResult;
    }

/**
 * RTestVclntSetScrIdAndPlayAviUrl::FsmL
 */
 
void RTestVclntSetScrIdAndPlayAviUrl::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                {
                // Set iScreenNumber
                iCurrentScreenNumber = iScreenNumber;
                INFO_PRINTF2(_L("iVideoPlayer->SetInitScreenNumber(%d)"), iScreenNumber);
                TInt err = KErrNone;
                err = iVideoPlayer->SetInitScreenNumber(iScreenNumber);
            #ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("ScreenNumber failed with error : %d"), err);
                    User::Leave(err);
                    }
            #else
                if (err == KErrNotSupported)
                    {
                    iTestStepResult = EKnownFailure;
                    INFO_PRINTF2(_L("SetInitScreenNumber() is not supported in this OS, returned %d"), err);
                    }
                else
                    {
                    iTestStepResult = EFail;
                    ERR_PRINTF2(_L("SetInitScreenNumber() failed with error : %d"), err);
                    }
                User::Leave(err);
            #endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenUrlL() %S"), &iFilename);
                iVideoPlayer->OpenUrlL(iFilename);
                PrepareState(EVPOpenComplete, KErrNone);
                break;
                }
            case EVPOpenComplete:
                {
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
                }
            case EVPPrepareComplete:
                {
                iError = KErrTimedOut;
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                PrepareState(EVPPlayComplete, KErrNone);
                break;
                }
            case EVPPlayComplete:
                {
                CActiveScheduler::Stop();
                iTestStepResult = EPass;
                break;
                }
            }
        }
    }

//
// RTestVclntSetScrIdAndPlayStopAviFile
//

/**
 * RTestVclntSetScrIdAndPlayStopAviFile::Constructor
 */
RTestVclntSetScrIdAndPlayStopAviFile::RTestVclntSetScrIdAndPlayStopAviFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber, TInt aScreenNumber2)
    : RTestVclntSetScrIdAndPlayBase(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber)
    {
    iScreenNumber2 = aScreenNumber2;
    iPlayTimer = NULL;
    iRestarted = EFalse;
    }

/**
 * RTestVclntSetScrIdAndPlayStopAviFile::NewL
 */
RTestVclntSetScrIdAndPlayStopAviFile* RTestVclntSetScrIdAndPlayStopAviFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber, TInt aScreenNumber2)
    {
    RTestVclntSetScrIdAndPlayStopAviFile* self = new (ELeave) RTestVclntSetScrIdAndPlayStopAviFile(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber, aScreenNumber2);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }

/**
 * set finished on callback from a timer
 */
void RTestVclntSetScrIdAndPlayStopAviFile::FinishedL()
    {
    INFO_PRINTF1(_L("-- iVideoPlayer->Stop();"));
    iVideoPlayer->Stop();
    iRestarted = EFalse;
    FsmL(EVPPlayStopped);
    }
    
/**
 * RTestVclntSetScrIdAndPlayStopAviFile::DoTestStepL
 */
TVerdict RTestVclntSetScrIdAndPlayStopAviFile::DoTestStepL()
    {
    iTestStepResult = EFail;
    iCountNumber = 0;
    iError = 0;
    // Get the filename
    TPtrC tmpFilename;
    if(!GetStringFromConfig(iSectName, _L("filename2"), tmpFilename))
        {
        ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
        return EInconclusive;
        }
    GetDriveName(iFilename2);
    iFilename2.Append(tmpFilename);
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
    return iTestStepResult;
    }

/**
 * RTestVclntSetScrIdAndPlayStopAviFile::FsmL
 */
void RTestVclntSetScrIdAndPlayStopAviFile::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                {
                // Set iScreenNumber
                iCurrentScreenNumber = iScreenNumber;
                INFO_PRINTF2(_L("iVideoPlayer->SetInitScreenNumber(%d)"), iScreenNumber);
                TInt err = KErrNone;
                err = iVideoPlayer->SetInitScreenNumber(iScreenNumber);
            #ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("ScreenNumber failed with error : %d"), err);
                    User::Leave(err);
                    }
            #else
                if (err == KErrNotSupported)
                    {
                    iTestStepResult = EKnownFailure;
                    INFO_PRINTF2(_L("SetInitScreenNumber() is not supported in this OS, returned %d"), err);
                    }
                else
                    {
                    iTestStepResult = EFail;
                    ERR_PRINTF2(_L("SetInitScreenNumber() failed with error : %d"), err);
                    }
                User::Leave(err);
            #endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                if (iBinaryCompatibility)
                	{
                	iVideoPlayer->OpenFileL(iFilename, ControllerUid());
                	}
                else
                	{
                	iVideoPlayer->OpenFileL(iFilename);
                	}
                
                PrepareState(EVPOpenComplete, KErrNone);
                break;
                }
            case EVPOpenComplete:
                {
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
                }
            case EVPPrepareComplete:
                {
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                if (iCountNumber == 0)
                    {
                    // initialise and start the timer.  the file is short and so decode occur rapidly
                    // hence the short time passed to After()                
                    TRAPD(err, iPlayTimer = CTestVclntAviPlayTimer::NewL(this, 2000000));
                    if (err != KErrNone)
                        {
                        ERR_PRINTF2(_L("CTestVclntAviPlayTimer::NewL left with error = %d"), err);
                        CActiveScheduler::Stop();
                        break;
                        }
                    iPlayTimer->Start();
                    // For first time we are expecting to stop the play after some second, 
                    // and second time it should play till the end of file.
                    PrepareState(EVPPlayStopped, KErrNone);
                    }
                else
                    {
                    PrepareState(EVPPlayComplete, KErrNone);
                    }
                break;
                }
           case EVPPlayStopped:
            {
            // Cancel the timer
            iPlayTimer->Cancel();
            delete iPlayTimer;
            iPlayTimer = NULL;
            // Set iScreenNumber
            iCurrentScreenNumber = iScreenNumber2;
            INFO_PRINTF2(_L("iVideoPlayer->SetInitScreenNumber(%d)"), iScreenNumber2);
            TInt err = KErrNone;
            err = iVideoPlayer->SetInitScreenNumber(iScreenNumber2);
        #ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("ScreenNumber failed with error : %d"), err);
                CActiveScheduler::Stop();
                User::Leave(err);
                }
        #else
            if (err == KErrNotSupported)
                {
                iTestStepResult = EKnownFailure;
                INFO_PRINTF2(_L("SetInitScreenNumber() is not supported in this OS, returned %d"), err);
                }
            else
                {
                iTestStepResult = EFail;
                ERR_PRINTF2(_L("SetInitScreenNumber() failed with error : %d"), err);
                }
            CActiveScheduler::Stop();
            User::Leave(err);
        #endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
            iCountNumber++;
            // Open iVideoPlayer
            INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename2);
            if (iBinaryCompatibility)
            	{
            	TRAP(iError, iVideoPlayer->OpenFileL(iFilename2, ControllerUid()));
            	}
            else
            	{
            	TRAP(iError, iVideoPlayer->OpenFileL(iFilename2));
            	}
            	
            if (iError != KErrNone)
                {
                ERR_PRINTF2(_L("iVideoPlayer->OpenFileL() left with error = %d"), iError);
                CActiveScheduler::Stop();
                }
            else
                {
                PrepareState(EVPOpenComplete, KErrNone);
                }
            break;
            }
           case EVPPlayComplete:
                {
                iTestStepResult = EPass;
                CActiveScheduler::Stop();
                break;
                }
            }
        }
    }

//
// RTestVclntSetScrIdAndPlayPauseAviFile
//

/**
 * RTestVclntSetScrIdAndPlayPauseAviFile::Constructor
 */
RTestVclntSetScrIdAndPlayPauseAviFile::RTestVclntSetScrIdAndPlayPauseAviFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber, TInt aScreenNumber2)
    : RTestVclntSetScrIdAndPlayBase(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber)
    {
    iScreenNumber2 = aScreenNumber2;
    }

/**
 * RTestVclntSetScrIdAndPlayPauseAviFile::NewL
 */
RTestVclntSetScrIdAndPlayPauseAviFile* RTestVclntSetScrIdAndPlayPauseAviFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber, TInt aScreenNumber2)
    {
    RTestVclntSetScrIdAndPlayPauseAviFile* self = new (ELeave) RTestVclntSetScrIdAndPlayPauseAviFile(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber, aScreenNumber2);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }

/**
 * set finished on callback from a timer
 */
void RTestVclntSetScrIdAndPlayPauseAviFile::FinishedL()
    {
    // We cannot use the logger until after OpenL is called again in EVPPlayPaused because
    // of the way the scheduler works.  The current thread is suspended until the video is 
    // finished playing if a synchronous call is made in WINSCW.
    TRAPD(err,iVideoPlayer->PauseL());
    if (err != KErrNotSupported)
        {
        INFO_PRINTF2(_L("Pause() returns with error :  %d"),err);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    iRestarted = EFalse;
    FsmL(EVPPlayPaused);
    }
    
/**
 * RTestVclntSetScrIdAndPlayPauseAviFile::DoTestStepL
 */
TVerdict RTestVclntSetScrIdAndPlayPauseAviFile::DoTestStepL()
    {
    iTestStepResult = EFail;
    iCountNumber = 0;
    iError = 0;
    // Get the filename
    TPtrC tmpFilename;
    if(!GetStringFromConfig(iSectName, _L("filename2"), tmpFilename))
        {
        ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
        return EInconclusive;
        }
    GetDriveName(iFilename2);
    iFilename2.Append(tmpFilename);
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
    return iTestStepResult;
    }

/**
 * RTestVclntSetScrIdAndPlayPauseAviFile::FsmL
 */
void RTestVclntSetScrIdAndPlayPauseAviFile::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                {
                // Set iScreenNumber
                iCurrentScreenNumber = iScreenNumber;
                INFO_PRINTF2(_L("iVideoPlayer->SetInitScreenNumber(%d)"), iScreenNumber);
                TInt err = KErrNone;
                err = iVideoPlayer->SetInitScreenNumber(iScreenNumber);
            #ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("ScreenNumber failed with error : %d"), err);
                    User::Leave(err);
                    }
            #else
                if (err == KErrNotSupported)
                    {
                    iTestStepResult = EKnownFailure;
                    INFO_PRINTF2(_L("SetInitScreenNumber() is not supported in this OS, returned %d"), err);
                    }
                else
                    {
                    iTestStepResult = EFail;
                    ERR_PRINTF2(_L("SetInitScreenNumber() failed with error : %d"), err);
                    }
                User::Leave(err);
            #endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                if (iBinaryCompatibility)
                	{
                	iVideoPlayer->OpenFileL(iFilename, ControllerUid());
                	}
                else
                	{
                	iVideoPlayer->OpenFileL(iFilename);
                	}
                
                PrepareState(EVPOpenComplete, KErrNone);
                break;
                }
            case EVPOpenComplete:
                {
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
                }
            case EVPPrepareComplete:
                {
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                // We cannot use the logger until after OpenL is called again in EVPPlayPaused because
                // of the way the scheduler works.  The current thread is suspended until the video is 
                // finished playing if a synchronous call is made in WINSCW.
                iVideoPlayer->Play();
                if (iCountNumber == 0)
                    {
                    // initialise and start the timer.  the file is short and so decode occur rapidly
                    // hence the short time passed to After()                
                    TRAPD(err, iPlayTimer = CTestVclntAviPlayTimer::NewL(this, 1000000));
                    if (err != KErrNone)
                        {
                        ERR_PRINTF2(_L("CTestVclntAviPlayTimer::NewL left with error = %d"), err);
                        CActiveScheduler::Stop();
                        break;
                        }
                    
                    iPlayTimer->Start();
                    PrepareState(EVPPlayPaused, KErrNone);
                    }
                else
                    {
                    PrepareState(EVPPlayComplete, KErrNone);
                    }
                break;
                }
            case EVPPlayPaused:
                {
                // Cancel the timer
                iPlayTimer->Cancel();
                delete iPlayTimer;
                iPlayTimer = NULL;
                
                // Set iScreenNumber
                iCurrentScreenNumber = iScreenNumber2;
                TInt err = iVideoPlayer->SetInitScreenNumber(iScreenNumber2);
            #ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("ScreenNumber failed with error : %d"), err);
                    CActiveScheduler::Stop();
                    User::Leave(err);
                    }
            #else
                if (err == KErrNotSupported)
                    {
                    iTestStepResult = EKnownFailure;
                    INFO_PRINTF2(_L("SetInitScreenNumber() is not supported in this OS, returned %d"), err);
                    }
                else
                    {
                    iTestStepResult = EFail;
                    ERR_PRINTF2(_L("SetInitScreenNumber() failed with error : %d"), err);
                    }
                CActiveScheduler::Stop();
                User::Leave(err);
            #endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                iCountNumber++;
                // Open iVideoPlayer
                if (iBinaryCompatibility)
	            	{
	            	TRAP(iError, iVideoPlayer->OpenFileL(iFilename2, ControllerUid()));
	            	}
	            else
	            	{
	            	TRAP(iError, iVideoPlayer->OpenFileL(iFilename2));
	            	}
	            
	            INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename2);

                if (iError != KErrNone)
                    {
                    ERR_PRINTF2(_L("iVideoPlayer->OpenFileL() left with error = %d"), iError);
                    CActiveScheduler::Stop();
                    }
                else
                    {
                    PrepareState(EVPOpenComplete, KErrNone);
                    }
                break;
                }
            case EVPPlayComplete:
                {
                iTestStepResult = EPass;
                CActiveScheduler::Stop();
                break;
                }
            }
        }
    }

//
// RTestVclntSetScrIdAfterPlayAndPlayAviFile
//

/**
 * RTestVclntSetScrIdAfterPlayAndPlayAviFile::Constructor
 */
RTestVclntSetScrIdAfterPlayAndPlayAviFile::RTestVclntSetScrIdAfterPlayAndPlayAviFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    : RTestVclntSetScrIdAndPlayBase(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber)
    {
    }

/**
 * RTestVclntSetScrIdAfterPlayAndPlayAviFile::NewL
 */
RTestVclntSetScrIdAfterPlayAndPlayAviFile* RTestVclntSetScrIdAfterPlayAndPlayAviFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    {
    RTestVclntSetScrIdAfterPlayAndPlayAviFile* self = new (ELeave) RTestVclntSetScrIdAfterPlayAndPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }

/**
 * RTestVclntSetScrIdAfterPlayAndPlayAviFile::DoTestStepL
 */
TVerdict RTestVclntSetScrIdAfterPlayAndPlayAviFile::DoTestStepL()
    {
    iTestStepResult = EFail;
    iCountNumber = 0;
    iError = 0;
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
    return iTestStepResult;
    }

/**
 * RTestVclntSetScrIdAfterPlayAndPlayAviFile::FsmL
 */
void RTestVclntSetScrIdAfterPlayAndPlayAviFile::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                {
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                if (iBinaryCompatibility)
	            	{
	            	iVideoPlayer->OpenFileL(iFilename, ControllerUid());
	            	}
	            else
	            	{
	            	iVideoPlayer->OpenFileL(iFilename);
	            	}
	            
                PrepareState(EVPOpenComplete, KErrNone);
                break;
                }
            case EVPOpenComplete:
                {
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
                }
            case EVPPrepareComplete:
                {
                iError = KErrTimedOut;
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                PrepareState(EVPPlayComplete, KErrNone);
                break;
                }
            case EVPPlayComplete:
                {
                if (iCountNumber == 0)
                    {
                    // Set iScreenNumber
                    iCurrentScreenNumber = iScreenNumber;
                    INFO_PRINTF2(_L("iVideoPlayer->SetInitScreenNumber(%d)"), iScreenNumber);
                    TInt err = KErrNone;
                    err = iVideoPlayer->SetInitScreenNumber(iScreenNumber);
                #ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                    if (err != KErrNone)
                        {
                        ERR_PRINTF2(_L("ScreenNumber failed with error : %d"), err);
                        CActiveScheduler::Stop();
                        User::Leave(err);
                        }
                #else
                    if (err == KErrNotSupported)
                        {
                        iTestStepResult = EKnownFailure;
                        INFO_PRINTF2(_L("SetInitScreenNumber() is not supported in this OS, returned %d"), err);
                        }
                    else
                        {
                        iTestStepResult = EFail;
                        ERR_PRINTF2(_L("SetInitScreenNumber() failed with error : %d"), err);
                        }
                    CActiveScheduler::Stop();
                    User::Leave(err);
                #endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                    iCountNumber++;
                    // Open iVideoPlayer
                    INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                    if (iBinaryCompatibility)
		            	{
		            	TRAP(iError, iVideoPlayer->OpenFileL(iFilename, ControllerUid()));
		            	}
		            else
		            	{
		            	TRAP(iError, iVideoPlayer->OpenFileL(iFilename));
		            	}
		            
                    if (iError != KErrNone)
                        {
                        ERR_PRINTF2(_L("iVideoPlayer->OpenFileL() left with error = %d"), iError);
                        CActiveScheduler::Stop();
                        }
                    else
                        {
                        PrepareState(EVPOpenComplete, KErrNone);
                        }
                    break;    
                    }
                else
                    {
                    iTestStepResult = EPass;
                    CActiveScheduler::Stop();    
                    }                    
                break;
                }
            }
        }
    }

//
// RTestVclntSetScrIdAfterOpenAndPlayAviFile
//

/**
 * RTestVclntSetScrIdAfterOpenAndPlayAviFile::Constructor
 */
RTestVclntSetScrIdAfterOpenAndPlayAviFile::RTestVclntSetScrIdAfterOpenAndPlayAviFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    : RTestVclntSetScrIdAndPlayBase(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber)
    {
    }

/**
 * RTestVclntSetScrIdAfterOpenAndPlayAviFile::NewL
 */
RTestVclntSetScrIdAfterOpenAndPlayAviFile* RTestVclntSetScrIdAfterOpenAndPlayAviFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    {
    RTestVclntSetScrIdAfterOpenAndPlayAviFile* self = new (ELeave) RTestVclntSetScrIdAfterOpenAndPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }

/**
 * RTestVclntSetScrIdAfterOpenAndPlayAviFile::DoTestStepL
 */
TVerdict RTestVclntSetScrIdAfterOpenAndPlayAviFile::DoTestStepL()
    {
    iTestStepResult = EFail;
    iCountNumber = 0;
    iError = 0;
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
    return iTestStepResult;
    }

/**
 * RTestVclntSetScrIdAfterOpenAndPlayAviFile::FsmL
 */
void RTestVclntSetScrIdAfterOpenAndPlayAviFile::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                {
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                if (iBinaryCompatibility)
	            	{
	            	iVideoPlayer->OpenFileL(iFilename, ControllerUid());
	            	}
	            else
	            	{
	            	iVideoPlayer->OpenFileL(iFilename);
	            	}
	            
                PrepareState(EVPOpenComplete, KErrNone);
                break;
                }
            case EVPOpenComplete:
                {
                if (iCountNumber == 0)
                    {
                    // Set iScreenNumber
                    iCurrentScreenNumber = iScreenNumber;
                    INFO_PRINTF2(_L("iVideoPlayer->SetInitScreenNumber(%d)"), iScreenNumber);
                    TInt err = 0;
                    err = iVideoPlayer->SetInitScreenNumber(iScreenNumber);
                #ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                    if (err != KErrNone)
                        {
                        ERR_PRINTF2(_L("ScreenNumber failed with error : %d"), err);
                        CActiveScheduler::Stop();
                        User::Leave(err);
                        }
                #else
                    if (err == KErrNotSupported)
                        {
                        iTestStepResult = EKnownFailure;
                        INFO_PRINTF2(_L("SetInitScreenNumber() is not supported in this OS, returned %d"), err);
                        }
                    else
                        {
                        iTestStepResult = EFail;
                        ERR_PRINTF2(_L("SetInitScreenNumber() failed with error : %d"), err);
                        }
                    CActiveScheduler::Stop();
                    User::Leave(err);
                #endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                    }
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
                }
            case EVPPrepareComplete:
                {
                iError = KErrTimedOut;
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                // When playing for the first time open the file again else play till the end.
                if (iCountNumber == 0)
                    {
                    iCountNumber++;
                    // Open iVideoPlayer
                    INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                    if (iBinaryCompatibility)
		            	{
		            	TRAP(iError, iVideoPlayer->OpenFileL(iFilename, ControllerUid()));
		            	}
		            else
		            	{
		            	TRAP(iError, iVideoPlayer->OpenFileL(iFilename));
		            	}
            		
                    if (iError != KErrNone)
                        {
                        ERR_PRINTF2(_L("iVideoPlayer->OpenFileL() left with error = %d"), iError);
                        CActiveScheduler::Stop();
                        }
                    else
                        {
                        PrepareState(EVPOpenComplete, KErrNone);
                        }
                    break;                        
                    }
                else
                    {
                    PrepareState(EVPPlayComplete, KErrNone);
                    }                    
                break;
                }
            case EVPPlayComplete:
                {
                iTestStepResult = EPass;
                CActiveScheduler::Stop();
                break;
                }
            }
        }
    }

//
// RTestVclntSetScrIdDuringPlayAndPlayAviFile
//

/**
 * RTestVclntSetScrIdDuringPlayAndPlayAviFile::Constructor
 */
RTestVclntSetScrIdDuringPlayAndPlayAviFile::RTestVclntSetScrIdDuringPlayAndPlayAviFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    : RTestVclntSetScrIdAndPlayBase(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber)
    {
    }

/**
 * RTestVclntSetScrIdDuringPlayAndPlayAviFile::NewL
 */
RTestVclntSetScrIdDuringPlayAndPlayAviFile* RTestVclntSetScrIdDuringPlayAndPlayAviFile::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber)
    {
    RTestVclntSetScrIdDuringPlayAndPlayAviFile* self = new (ELeave) RTestVclntSetScrIdDuringPlayAndPlayAviFile(aTestName, aSectName, aKeyName, aExpectedError, aScreenNumber);
    return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    }

/**
 * RTestVclntSetScrIdDuringPlayAndPlayAviFile::DoTestStepL
 */
TVerdict RTestVclntSetScrIdDuringPlayAndPlayAviFile::DoTestStepL()
    {
    iTestStepResult = EFail;
    iCountNumber = 0;
    iError = 0;
    // Call the state handler from IDLE state
    TRAPD(err, FsmL(EVPIdle));
    if (err == KErrNone)
        {
        // Start the scheduler - Done only once !
        CActiveScheduler::Start();    
        }
    return iTestStepResult;
    }

/**
 * RTestVclntSetScrIdDuringPlayAndPlayAviFile::FsmL
 */
void RTestVclntSetScrIdDuringPlayAndPlayAviFile::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                {
                // Open iVideoPlayer
                INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                if (iBinaryCompatibility)
	            	{
	            	iVideoPlayer->OpenFileL(iFilename, ControllerUid());
	            	}
	            else
	            	{
	            	iVideoPlayer->OpenFileL(iFilename);
	            	}
            	
                PrepareState(EVPOpenComplete, KErrNone);
                break;
                }
            case EVPOpenComplete:
                {
                // Prepare iVideoPlayer
                INFO_PRINTF1(_L("iVideoPlayer->Prepare()"));
                iVideoPlayer->Prepare();
                PrepareState(EVPPrepareComplete, KErrNone);
                break;
                }
            case EVPPrepareComplete:
                {
                iError = KErrTimedOut;
                INFO_PRINTF1(_L("iVideoPlayer->Play()"));
                iVideoPlayer->Play();
                if (iCountNumber == 0)
                    {
                    // Set iScreenNumber
                    iCurrentScreenNumber = iScreenNumber;
                    INFO_PRINTF2(_L("iVideoPlayer->SetInitScreenNumber(%d)"), iScreenNumber);
                    TInt err = KErrNone;
                    err = iVideoPlayer->SetInitScreenNumber(iScreenNumber);
                #ifdef SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                    if (err != KErrNone)
                        {
                        ERR_PRINTF2(_L("ScreenNumber failed with error : %d"), err);
                        CActiveScheduler::Stop();
                        User::Leave(err);
                        }
                #else
                    if (err == KErrNotSupported)
                        {
                        iTestStepResult = EKnownFailure;
                        INFO_PRINTF2(_L("SetInitScreenNumber() is not supported in this OS, returned %d"), err);
                        }
                    else
                        {
                        iTestStepResult = EFail;
                        ERR_PRINTF2(_L("SetInitScreenNumber() failed with error : %d"), err);
                        }
                    CActiveScheduler::Stop();
                    User::Leave(err);
                #endif // SYMBIAN_ENABLE_MMF_MULTISCREEN_SUPPORT
                    iCountNumber++;
                    // Open iVideoPlayer
                    INFO_PRINTF2(_L("iVideoPlayer->OpenFileL() %S"), &iFilename);
                    if (iBinaryCompatibility)
		            	{
		            	TRAP(iError, iVideoPlayer->OpenFileL(iFilename, ControllerUid()));
		            	}
		            else
		            	{
		            	TRAP(iError, iVideoPlayer->OpenFileL(iFilename));
		            	}
            	
                    if (iError != KErrNone)
                        {
                        ERR_PRINTF2(_L("iVideoPlayer->OpenFileL() left with error = %d"), iError);
                        CActiveScheduler::Stop();
                        }
                    else
                        {
                        PrepareState(EVPOpenComplete, KErrNone);
                        }
                    break;                        
                    }
                else
                    {
                    PrepareState(EVPPlayComplete, KErrNone);
                    }                                        
                break;
                }
            case EVPPlayComplete:
                {
                iTestStepResult = EPass;
                CActiveScheduler::Stop();
                break;
                }
            }
        }
    }

//
// CTestVclntAviPlayTimer
//

/**
 * NewL
 */
CTestVclntAviPlayTimer* CTestVclntAviPlayTimer::NewL(RTestVclntSetScrIdAndPlayBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    {
    CTestVclntAviPlayTimer* self = new (ELeave) CTestVclntAviPlayTimer(aParent, aWaitTime);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }
    
/**
 * Constructor
 */
CTestVclntAviPlayTimer::CTestVclntAviPlayTimer(RTestVclntSetScrIdAndPlayBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    : CTimer(EPriorityHigh), iParent(aParent), iWaitTime(aWaitTime)
    {    
    CActiveScheduler::Add(this);
    }    
    
/**
 * Start() - Start the timer
 */
void CTestVclntAviPlayTimer::Start()
    {
    After(iWaitTime);
    }

/**
 * RunL() - see CActive class for more info
 */
void CTestVclntAviPlayTimer::RunL()
    {        
    // (iParent->Finished() should call Stop() or not, as we decide)    
    iParent->FinishedL();
    }

/**
 * RunError() - see CActive class for more info
 */
TInt CTestVclntAviPlayTimer::RunError(TInt aError)
    {
    CActiveScheduler::Stop();
    return aError;
    }
