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
// Source file: Base classes for Video Client tests
// 
//

#include "tsi_mmf_vclntavi_stepbase.h"


//
// Local Function(s)
//
/**
 * GetDriveName()
 * Method to get the drive name where the data files are present
 */
TAny GetDriveName(TDes& aFileName)
{
#ifdef __WINS__
    aFileName = _L("c:");    
#elif defined(__MARM__) || defined(__X86GCC__)
    #ifdef TEST_FILES_ON_C_DRIVE
        aFileName = _L("c:");
    #else
        aFileName = _L("z:");    
    #endif // TEST_FILES_ON_C_DRIVE
#endif    
}

//
// RTestMmfVclntAviStep
//

/**
 * RTestMmfVclntAviStep::WaitWithTimeout()
 * Timeout function
 */
void RTestMmfVclntAviStep::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
    {
    TRequestStatus timerStatus;
    RTimer timer ;
    timer.CreateLocal() ;
    timer.After(timerStatus,aNumberOfMicroSeconds);

    User::WaitForRequest(aStatus, timerStatus);
    if (timerStatus == KRequestPending)
        {
        timer.Cancel();
        User::WaitForRequest(timerStatus);
        }
    else
        {
        INFO_PRINTF1(_L("Time is over!!!")) ;
        }
    timer.Close() ;
    }

/**
 * RTestMmfVclntAviStep::TimeComparison()
 * Time comparison utility function
 * Returns true if the actual timed value fell within deviation limits
 */ 
TBool RTestMmfVclntAviStep::TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation)
    {
    // save unnessary conditions
    if(aActual == aExpected)
        return ETrue;    

    // Prevent unsigned wrapping errors 
    TUint difference;
    if(aActual > aExpected)
        difference = aActual - aExpected;
    else
        difference = aExpected - aActual;

    // comapare
    if(difference < aDeviation)
        return ETrue;
    return EFalse;
    }

/**
 * RTestMmfVclntAviStep::DoTestStepPreambleL()
 * Test Preample routines.
 * Creates our own Active Scheduler.
 */
TVerdict RTestMmfVclntAviStep::DoTestStepPreambleL()
    {
    // Install the Active Scheduler
    iActiveScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(iActiveScheduler);
    
    // Reset the error state flag.
    iError = KErrNone;
    
    // Get the filename
    TPtrC tmpFilename;
    if(!GetStringFromConfig(iSectName, iKeyName, tmpFilename))
        {
        ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
        return EInconclusive;
        }
        
    if (tmpFilename.Length() > 254)
    	{
        ERR_PRINTF1(_L("ERROR : Filename too long"));
        return EInconclusive;    	
    	}
        
    GetDriveName(iFilename);
    iFilename.Append(tmpFilename);
    
    // Check if we are doing a binary compatibility test. BC tests use a 
    // controller with no surface support.
    GetBoolFromConfig(iSectName, KBinaryCompatibilityKeyName, iBinaryCompatibility);
    
    // Initialise Window server objects
    InitWservL();
    
    return EPass;
    }

/**
 * RTestMmfVclntAviStep::DoTestStepPostambleL()
 * Test Postample routines.
 * Destroys our Active Scheduler.
 */
TVerdict RTestMmfVclntAviStep::DoTestStepPostambleL()
    {
    delete iActiveScheduler;
    iActiveScheduler = NULL;
    
    // Destroy Window server objects
    DeInitWserv();

    return EPass;
    }

TUid RTestMmfVclntAviStep::ControllerUid() const
	{
	return iBinaryCompatibility ? KMmfTestAviNonGcePlayControllerUid : KNullUid;
	}

/**
 * RTestMmfVclntAviStep::RTestMmfVclntAviStep()
 */
RTestMmfVclntAviStep::RTestMmfVclntAviStep(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
    {
    iTestStepName = aTestName;
    iSectName = aSectName;
    iKeyName = aKeyName;
    iTestExpectedError = aExpectedError;
    // expand heap, so we can load 80k video
    iHeapSize = 150000;
    }

/**
 * RTestMmfVclntAviStep::InitWservL()
 */
void RTestMmfVclntAviStep::InitWservL()
    {
    TInt err = iWs.Connect();
    if (err != KErrNone)
        {
        // Access violation if ws is null
        INFO_PRINTF1(_L("Cannot test, no window server available"));
        User::Leave(err);
        }

    iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
    User::LeaveIfError(iScreen->Construct()); // and complete its construction

    iRootWindow = RWindowGroup(iWs);
    User::LeaveIfError(iRootWindow.Construct((TInt32) this));

    iWindow = new(ELeave) RWindow(iWs);
    User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow, ((TInt32) this) + 1));
    iWindow->SetRequiredDisplayMode(EColor16MA);
    iWindow->SetExtent(TPoint(10,10), TSize(176,144));
    iWindow->SetVisible(ETrue);
    iWindow->SetBackgroundColor(TRgb(0, 0));
    iWindow->Activate();
    iWindow->BeginRedraw();
    iWindow->EndRedraw();
    
    
    iWs.Flush();
    }

/**
 * RTestMmfVclntAviStep::DeInitWserv
 */
void RTestMmfVclntAviStep::DeInitWserv()
    {
    if (iWindow)
        {
        iWindow->Close();
        delete iWindow;
        iWindow = NULL;
        }
    
    iRootWindow.Close();
    delete iScreen;
    iScreen = NULL;
    
    iWs.Flush();
    iWs.Close();
    }

/**
 *
 * DeleteFile
 *
 */
TInt RTestMmfVclntAviStep::DeleteFileL(const TDesC &aFileName)
    {
    // Errors ignored while deleting a file...
    RFs fs;
    TInt err = fs.Connect();
    if(err != KErrNone)
        return err;
    CleanupClosePushL(fs);
    err = fs.Delete(aFileName);
    CleanupStack::PopAndDestroy();
    INFO_PRINTF3(_L("DeleteFileL(%S) returned : %d"), &aFileName ,err);
    return err;
    }
//
// RTestVclntAviPlayerStep
//

/**
 * RTestVclntAviPlayerStep::RTestVclntAviPlayerStep()
 */
RTestVclntAviPlayerStep::RTestVclntAviPlayerStep(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
    : RTestMmfVclntAviStep(aTestName, aSectName, aKeyName, aExpectedError)
    {}

/**
 * RTestVclntAviPlayerStep::DoTestStepPreambleL()
 */
TVerdict RTestVclntAviPlayerStep::DoTestStepPreambleL()
    {
    INFO_PRINTF1(_L("RTestVclntAviPlayerStep::DoTestStepPreambleL()"));
    
    // Base DoTestStepPreambleL
    TVerdict preamRes = RTestMmfVclntAviStep::DoTestStepPreambleL();
    if (preamRes != EPass)
        {
        return preamRes;
        }
    
    // Create the Video Player Utility
    TPoint rectOrigin(10, 10);
    TSize rectSize(176, 144);
    TRect rect(rectOrigin, rectSize);
    
    TPoint clipOrigin(10, 10);
    TSize clipSize(176, 144);
    TRect clipRect(clipOrigin, clipSize);
    
    iVideoPlayer = CVideoPlayerUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality, iWs, *iScreen, *iWindow, rect, clipRect);
    
    // Initialise the State variables
    ResetState();
    
    return preamRes;
    }

/**
 * RTestVclntAviPlayerStep::DoTestStepPostambleL()
 */
TVerdict RTestVclntAviPlayerStep::DoTestStepPostambleL()
    {
    INFO_PRINTF1(_L("RTestVclntAviPlayerStep::DoTestStepPostambleL()"));

    TVerdict postRes;
    postRes = EPass;
    
    delete iVideoPlayer;        
    iVideoPlayer = NULL;
    
    // Base DoTestStepPostambleL
    postRes = RTestMmfVclntAviStep::DoTestStepPostambleL();
    return postRes;
    }

/**
 * RTestVclntAviPlayerStep::ResetState()
 */
void RTestVclntAviPlayerStep::ResetState()
    {
    iExpectedError = KErrNone;
    iExpectedEvent = EVPIdle;
    }

/**
 * RTestVclntAviPlayerStep::PrepareState()
 */
void RTestVclntAviPlayerStep::PrepareState(TVclntTestPlayEvents aExpectedEvent, TInt aExpectedError)
    {
    iExpectedError = aExpectedError;
    iExpectedEvent = aExpectedEvent;    
    }

/**
 * RTestVclntAviPlayerStep::FsmCheck()
 */
TBool RTestVclntAviPlayerStep::FsmCheck(TVclntTestPlayEvents aEventCode)
    {
    TBool retFsmCheck = EFalse;
    
    if (aEventCode != iExpectedEvent)
        {
        ERR_PRINTF3(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedEvent, aEventCode);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else if(iError != iExpectedError)
        {
        if (iError == iTestExpectedError)
            {
            ERR_PRINTF3(_L("TestExpectedError : %d, ActualError : %d"), iTestExpectedError, iError);
            iTestStepResult = EPass;
            }
        else
            {
            if ((iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-1010-HP")) == 0) ||
            	(iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-9110-HP")) == 0))
                {
                iTestExpectedError = KErrCorrupt;
                ERR_PRINTF3(_L("TestExpectedError : %d, ActualError : %d"), iTestExpectedError, iError);
                iTestStepResult = EPass;
                }
            else if ((iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-0003-CP")) == 0) ||
            		(iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-9003-CP")) == 0))
                {
                iTestExpectedError = KErrNotSupported;
                ERR_PRINTF3(_L("TestExpectedError : %d, ActualError : %d"), iTestExpectedError, iError);
                iTestStepResult = EPass;
                }
             else if ((iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-1011-CP")) == 0) ||
            		 (iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-9111-CP")) == 0))
                {
                iTestExpectedError = KErrNotSupported;
                ERR_PRINTF1(_L("Codec not supported"));
                ERR_PRINTF3(_L("TestExpectedError : %d, ActualError : %d"), iTestExpectedError, iError);
                iTestStepResult = EPass;
                }
            else if (iBinaryCompatibility && (iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-1012-HP")) == 0) || 
            		(iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-9112-HP")) == 0))
            	{
            	iTestExpectedError = KErrCorrupt;
            	if (iError == iTestExpectedError)
		            {
		            ERR_PRINTF3(_L("TestExpectedError : %d, ActualError : %d"), iTestExpectedError, iError);
		            iTestStepResult = EPass;
		            }
            	}
            else
                {
                ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, iError);
                iTestStepResult = EFail;                    
                }
            }
            CActiveScheduler::Stop();
        }
    else
        {
        retFsmCheck = ETrue;
        }

    return retFsmCheck;
    }

/**
 * RTestVclntAviPlayerStep::MvpuoOpenComplete
 * Callback function for CVideoPlayerUtility
 */
void RTestVclntAviPlayerStep::MvpuoOpenComplete(TInt aError)
    {
    iError = aError;
    INFO_PRINTF2(_L("Open Complete callback returned with error %d"), iError);
    TRAPD(err, FsmL(EVPOpenComplete));
    if (err != KErrNone)
        {
        iError = err;    
        iTestStepResult = EFail;
        INFO_PRINTF2(_L("Open Complete test handler left with error %d"), iError);
        CActiveScheduler::Stop();
        }
    }

/**
 * RTestVclntAviPlayerStep::MvpuoPrepareComplete
 * Callback function for CVideoPlayerUtility
 */
void RTestVclntAviPlayerStep::MvpuoPrepareComplete(TInt aError)
    {
    iError = aError;
    INFO_PRINTF2(_L("Prepare Complete callback with error %d"), iError);
    TRAPD(err, FsmL(EVPPrepareComplete));
    if (err != KErrNone)
        {
        iError = err;    
        iTestStepResult = EFail;
        INFO_PRINTF2(_L("Prepare Complete test handler left with error %d"), iError);
        CActiveScheduler::Stop();
        }
    }

/**
 * RTestVclntAviPlayerStep::MvpuoFrameReady
 * Callback function for CVideoPlayerUtility
 */
void RTestVclntAviPlayerStep::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt aError)
    {
    iError = aError;
    INFO_PRINTF2(_L("Frame Ready callback with error %d"), iError);
    TRAPD(err, FsmL(EVPFrameReady));
    if (err != KErrNone)
        {
        iError = err;    
        iTestStepResult = EFail;
        INFO_PRINTF2(_L("Frame Ready test handler left with error %d"), iError);
        CActiveScheduler::Stop();
        }
    }

/**
 * RTestVclntAviPlayerStep::MvpuoPlayComplete
 * Callback function for CVideoPlayerUtility
 */
void RTestVclntAviPlayerStep::MvpuoPlayComplete(TInt aError)
    {
    iError = aError;
    INFO_PRINTF2(_L("Play Complete callback with error %d"), iError);
    TRAPD(err, FsmL(EVPPlayComplete));
    if (err != KErrNone)
        {
        iError = err;    
        iTestStepResult = EFail;
        INFO_PRINTF2(_L("Play Complete test handler left with error %d"), iError);
        CActiveScheduler::Stop();
        }
    }
    
/**
 * RTestVclntAviPlayerStep::MvpuoEvent
 * Callback function for CVideoPlayerUtility
 */
void RTestVclntAviPlayerStep::MvpuoEvent(const TMMFEvent& /*aEvent*/)
    {
    INFO_PRINTF1(_L("Mvpuo Event callback"));
    }

#ifdef SYMBIAN_BUILD_GCE 
//
// RTestVclnt2AviPlayerStep
//

/**
 * RTestVclnt2AviPlayerStep::RTestVclnt2AviPlayerStep()
 */
RTestVclnt2AviPlayerStep::RTestVclnt2AviPlayerStep(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclntAviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError)
    {}

/**
 * RTestVclnt2AviPlayerStep::InitWservL()
 */
void RTestVclnt2AviPlayerStep::InitWservL()
    {
    RTestVclntAviPlayerStep::InitWservL();    

    iGc = new (ELeave) CWindowGc(iScreen);
    User::LeaveIfError(iGc->Construct());

    iRootWindow.AutoForeground(ETrue);    

    iWindow->SetRequiredDisplayMode(EColor16MA); // Need to use alpha.
    iWindow->SetSizeErr(iScreen->SizeInPixels());
    // XXX: This will not be needed when surfaces are displayed above the window background colour.
    iWindow->SetBackgroundColor(TRgb(0x000000, 0)); // use transparent black as the background colour.
    iWindow->BeginRedraw();
    iWindow->EndRedraw();

    iWs.Flush();
    }

/**
 * RTestVclnt2AviPlayerStep::DeInitWserv
 */
void RTestVclnt2AviPlayerStep::DeInitWserv()
    {
    delete iGc;
    iGc = NULL;
    
    RTestVclntAviPlayerStep::DeInitWserv();
    }


/**
 * RTestVclnt2AviPlayerStep::DoTestStepPreambleL()
 */
TVerdict RTestVclnt2AviPlayerStep::DoTestStepPreambleL()
    {
    INFO_PRINTF1(_L("RTestVclnt2AviPlayerStep::DoTestStepPreambleL()"));
        
    // Call RTestMmfVclntAviStep::DoTestStepPreambleL instead of the immediate parent's 
    // DoTestStepPreambleL as there is no need for CVideoPlayerUtility setup
    TVerdict preamRes = RTestMmfVclntAviStep::DoTestStepPreambleL();    
    if (preamRes != EPass)
        {
        return preamRes;
        }
    
    iVideoPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
    
    // Initialise the State variables
    ResetState();
    
    return EPass;
    }

/**
 * RTestVclnt2AviPlayerStep::DoTestStepPostambleL()
 */
TVerdict RTestVclnt2AviPlayerStep::DoTestStepPostambleL()
    {
    INFO_PRINTF1(_L("RTestVclnt2AviPlayerStep::DoTestStepPostambleL()"));

    delete iVideoPlayer2;  
    iVideoPlayer2 = NULL;      
        
    // Base DoTestStepPostambleL
    return RTestMmfVclntAviStep::DoTestStepPostambleL();
    }

/**
 * RTestVclnt2AviPlayerStep::DoTestStepL
 */
TVerdict RTestVclnt2AviPlayerStep::DoTestStepL()
    {
    // Call the state handler from IDLE state
    FsmL(EVPIdle);
    
    // Start the scheduler - Done only once !
    CActiveScheduler::Start();
    
    return iTestStepResult;
    }


/**
 * RTestVclnt2AviPlayerStep::FsmCheck()
 */
TBool RTestVclnt2AviPlayerStep::FsmCheck(TVclntTestPlayEvents aEventCode)
    {
    TBool retFsmCheck = EFalse;
    
    if (aEventCode != iExpectedEvent)
        {
        ERR_PRINTF3(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedEvent, aEventCode);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else if(iError != iExpectedError)
        {
        ERR_PRINTF3(_L("TestExpectedError : %d, ActualError : %d"), iTestExpectedError, iError);
        iTestStepResult = iError == iTestExpectedError ? EPass : EFail;
        CActiveScheduler::Stop();   
        }
    else
        {
        retFsmCheck = ETrue;
        }

    return retFsmCheck;
    }
    
#endif

//
// RTestVclntAviRecorderStep
//

/**
 * RTestVclntAviRecorderStep::RTestVclntAviRecorderStep()
 */
RTestVclntAviRecorderStep::RTestVclntAviRecorderStep(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
    : RTestMmfVclntAviStep(aTestName, aSectName, aKeyName, aExpectedError)
    {}

/**
 * RTestVclntAviRecorderStep::DoTestStepPreambleL()
 */
TVerdict RTestVclntAviRecorderStep::DoTestStepPreambleL()
    {
    INFO_PRINTF1(_L("RTestVclntAviRecorderStep::DoTestStepPreambleL()"));
    
    TVerdict preamRes;
    preamRes = EPass;
    
    // Base DoTestStepPreambleL
    preamRes = RTestMmfVclntAviStep::DoTestStepPreambleL();
    if (preamRes != EPass)
        {
        return preamRes;
        }
    
    // Create the MVS Client
    iVideoRecorder = CVideoRecorderUtility::NewL(*this);
    if(iVideoRecorder == NULL)
        {
        ERR_PRINTF1(_L("ERROR : iVideoRecorder cannot be created."));
        return EInconclusive;
        }
    
    // Create the camera
    iCamera = CCamera::NewL(*this,0,0);
    if(iCamera == NULL)
        {
        ERR_PRINTF1(_L("ERROR : iCamera cannot be created."));
        return EInconclusive;
        }
    iCamera->Reserve();
    CActiveScheduler::Start();
    
    // Initialise the camera handle
    iCameraHandle=iCamera->Handle();
    
    // delete the record file.
    DeleteFileL(iFilename);
    
    // Initialise the State variables
    ResetState();
    
    return preamRes;
    }

/**
 * RTestVclntAviRecorderStep::DoTestStepPostambleL()
 */
TVerdict RTestVclntAviRecorderStep::DoTestStepPostambleL()
    {
    INFO_PRINTF1(_L("RTestVclntAviRecorderStep::DoTestStepPostambleL()"));

//    TVerdict PostRes;
//    PostRes = EPass;
    
    // Delete CCamera
    if (iCamera!=NULL)
        {
        iCamera->Release();
        delete iCamera;
        iCamera = NULL;
        }
    
    // Delete CVideoRecorderUtility
    if (iVideoRecorder)
        {
        delete iVideoRecorder;
        iVideoRecorder = NULL;
        }
    
    // Base DoTestStepPostambleL
    INFO_PRINTF2(_L("RTestMmfVclntAviStep::DoTestStepPostambleL() %d"), RTestMmfVclntAviStep::DoTestStepPostambleL());
    return RTestMmfVclntAviStep::DoTestStepPostambleL();
    }

/**
 * RTestVclntAviRecorderStep::ResetState
 */
void RTestVclntAviRecorderStep::ResetState()
    {
    iExpectedError = KErrNone;
    iExpectedEvent = EVRIdle;
    }

/**
 * RTestVclntAviRecorderStep::PrepareState
 */
void RTestVclntAviRecorderStep::PrepareState(TVclntTestRecordEvents aExpectedEvent, TInt aExpectedError)
    {
    iExpectedError = aExpectedError;
    iExpectedEvent = aExpectedEvent;
    }

/**
 * RTestVclntAviRecorderStep::FsmCheck
 */
TBool RTestVclntAviRecorderStep::FsmCheck(TVclntTestRecordEvents aEventCode)
    {
    TBool retFsmCheck = EFalse;
    
    if (aEventCode != iExpectedEvent)
        {
        ERR_PRINTF3(_L("ExpectedEvent : %d, ActualEvent : %d"), iExpectedEvent, aEventCode);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    else if(iError != iExpectedError)
        {
        if (iError == iTestExpectedError)
            {
            ERR_PRINTF3(_L("TestExpectedError : %d, ActualError : %d"), iTestExpectedError, iError);
            iTestStepResult = EPass;
            }
        else
            {
            if (iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-0103-CP")) == 0 || iTestStepName.Compare(_L("MM-MMF-VCLNTAVI-I-0102-CP")) == 0) 
                {
                iTestExpectedError = KErrNotSupported;
                ERR_PRINTF3(_L("TestExpectedError : %d, ActualError : %d"), iTestExpectedError, iError);
                iTestStepResult = EPass;
                }
            else
                {
                ERR_PRINTF3(_L("ExpectedError : %d, ActualError : %d"), iExpectedError, iError);
                iTestStepResult = EFail;
                }
            }
        CActiveScheduler::Stop();
        }
    else
        {
        retFsmCheck = ETrue;
        }

    return retFsmCheck;
    }

/**
 * RTestVclntAviRecorderStep::MvruoOpenComplete
 * Callback function for CVideoRecorderUtility
 */
void RTestVclntAviRecorderStep::MvruoOpenComplete(TInt aError)
    {
    iError = aError;
    INFO_PRINTF2(_L("Open complete callback with error %d"), iError);
    TRAPD(err, FsmL(EVROpenComplete));
    if (err != KErrNotSupported)
        {
        iError = err;    
        }
    }

/**
 * RTestVclntAviRecorderStep::MvruoRecordComplete
 * Callback function for CVideoRecorderUtility
 */
void RTestVclntAviRecorderStep::MvruoRecordComplete(TInt aError)
    {
    iError = aError;
    INFO_PRINTF2(_L("Record complete callback with error %d"), iError);
    TRAPD(err, FsmL(EVRRecordComplete));
    if (err != KErrNone)
        {
        iError = err;    
        }
    }

/**
 * RTestVclntAviRecorderStep::MvruoPrepareComplete
 * Callback function for CVideoRecorderUtility
 */
void RTestVclntAviRecorderStep::MvruoPrepareComplete(TInt aError)
    {
    iError = aError;
    
    TInt errorCode = KErrNone;
    INFO_PRINTF2(_L("Prepare complete callback with error %d"), iError);
    
    TRAP(errorCode, iVideoRecorder->SetVideoFrameRateL(KDefaultFrameRate));
    if(errorCode)
        {
        INFO_PRINTF2(_L("SetFrameRate() after Prepare failed with  %d "), errorCode);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    
    const TSize KDefaultFrameSize(176,144);
    TRAP(errorCode, iVideoRecorder->SetVideoFrameSizeL(KDefaultFrameSize));
    if(errorCode)
        {
        INFO_PRINTF2(_L("SetFrameSize() after Prepare failed with  %d "), errorCode);
        iTestStepResult = EFail;
        CActiveScheduler::Stop();
        }
    
    if(errorCode == KErrNone)
        {
        TRAPD(err, FsmL(EVRPrepareComplete));
        if (err != KErrNone)
            {
            iError = err;    
            }    
        }
    }
    
/**
 * RTestVclntAviRecorderStep::MvruoEvent
 * Callback function for CVideoRecorderUtility
 */
void RTestVclntAviRecorderStep::MvruoEvent(const TMMFEvent& /*aEvent*/)
    {
    INFO_PRINTF1(_L("Mvruo Event callback"));
    }

/**
 * RTestVclntAviRecorderStep::HandleEvent
 * Callback function for CCamera
 */
void RTestVclntAviRecorderStep::HandleEvent(const TECAMEvent &aEvent)
    {
    iError = aEvent.iErrorCode;
    
    if (aEvent.iEventType == KUidECamEventReserveComplete)
        {
        INFO_PRINTF2(_L("Camera Reserve complete callback ( ERROR : %d )"), iError);
        }
    else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
        {
        INFO_PRINTF2(_L("Camera PowerOn complete callback ( ERROR : %d )"), iError);
        }
    else if (aEvent.iEventType == KUidECamEventCameraNoLongerReserved)
        {
        INFO_PRINTF2(_L("Camera No longer Reserved callback ( ERROR : %d )"), iError);
        }
    CActiveScheduler::Stop();
    }

/**
 * RTestVclntAviRecorderStep::ViewFinderReady
 * Callback function for CCamera
 */
void RTestVclntAviRecorderStep::ViewFinderReady(MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)
    {
        
    }

/**
 * RTestVclntAviRecorderStep::ImageBufferReady
 * Callback function for CCamera
 */
void RTestVclntAviRecorderStep::ImageBufferReady(MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)
    {
    }

/**
 * RTestVclntAviRecorderStep::VideoBufferReady
 * Callback function for CCamera
 */
void RTestVclntAviRecorderStep::VideoBufferReady(MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)
    {
    }


