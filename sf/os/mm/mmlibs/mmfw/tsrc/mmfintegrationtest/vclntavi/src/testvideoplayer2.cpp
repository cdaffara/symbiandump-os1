// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include <e32math.h>
#include "testvideoplayer2.h"

#ifdef SYMBIAN_BUILD_GCE
#include <graphics/surfacemanager.h>
#endif

const TInt KSettingsAndAlignmentTest = 3;

//
// RTestVclntPlayAviFile
//

/**
 * RTestVclnt2PlayFile::Constructor
 */
RTestVclnt2PlayFile::RTestVclnt2PlayFile(const TDesC& aTestName, 
                                         const TDesC& aSectName, 
                                         const TDesC& aKeyName, 
                                         TInt aExpectedError)
    : RTestVclnt2AviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2PlayFile::NewL
 */
RTestVclnt2PlayFile* RTestVclnt2PlayFile::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclnt2PlayFile* self = new (ELeave) RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclnt2PlayFile::DoTestStepPreambleL
 */
TVerdict RTestVclnt2PlayFile::DoTestStepPreambleL()
    {
    return RTestVclnt2AviPlayerStep::DoTestStepPreambleL();
    }

/**
 * RTestVclnt2PlayFile::DoTestStepPostambleL
 */
TVerdict RTestVclnt2PlayFile::DoTestStepPostambleL()
    {
    return RTestVclnt2AviPlayerStep::DoTestStepPostambleL();        
    }

/**
 * RTestVclnt2PlayFile::FsmL
 */
void RTestVclnt2PlayFile::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                HandleIdleL();
                break;
                
            case EVPOpenComplete:
                HandleOpenCompleteL();
                break;
                
            case EVPPrepareComplete:                    
                HandlePrepareCompleteL();
                break;
                
            case EVPPlayComplete:
                HandlePlayCompleteL();
                break;
            }
        }
    }
    
void RTestVclnt2PlayFile::HandleIdleL()
	{
	// Open iVideoPlayer
    INFO_PRINTF2(_L("iVideoPlayer2->OpenFileL() %S"), &iFilename);
    iVideoPlayer2->OpenFileL(iFilename, KMmfTestAviPlayControllerUid);
    PrepareState(EVPOpenComplete, KErrNone);
	}

void RTestVclnt2PlayFile::HandleOpenCompleteL()
	{
	// Prepare iVideoPlayer2
    INFO_PRINTF1(_L("iVideoPlayer2->Prepare()"));
    iVideoPlayer2->Prepare();
    PrepareState(EVPPrepareComplete, KErrNone);
	}
	
void RTestVclnt2PlayFile::HandlePrepareCompleteL()
	{
	// Add display window using default values - ie. video extent & window clipping 
	// defaulted to whole window
	INFO_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL()"));
	iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow);

    // trigger the video to start playing                
    StartPlayback();
	}

void RTestVclnt2PlayFile::HandlePlayCompleteL()
	{
	iTestStepResult = EPass;
	CActiveScheduler::Stop();
	}

void RTestVclnt2PlayFile::StartPlayback()
	{
    iError = KErrTimedOut;
    INFO_PRINTF1(_L("iVideoPlayer2->Play()"));
    PrepareState(EVPPlayComplete, KErrNone);
    iVideoPlayer2->Play();
	}
	
#ifdef SYMBIAN_BUILD_GCE
//
// RTestMediaClientVideoDisplay
//

/**
 * RTestMediaClientVideoDisplay::Constructor
 */


RTestMediaClientVideoDisplay::RTestMediaClientVideoDisplay(const TDesC& aTestName, 
                                         const TDesC& aSectName, 
                                         const TDesC& aKeyName, 
                                         TInt aExpectedError)
    : RTestVclnt2AviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestMediaClientVideoDisplay::NewL
 */

RTestMediaClientVideoDisplay* RTestMediaClientVideoDisplay::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestMediaClientVideoDisplay* self = new (ELeave) RTestMediaClientVideoDisplay(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestMediaClientVideoDisplay::DoTestStepPreambleL
 */
 
TVerdict RTestMediaClientVideoDisplay::DoTestStepPreambleL()
    {
    return RTestVclnt2AviPlayerStep::DoTestStepPreambleL();
    }

/**
 * RTestMediaClientVideoDisplay::DoTestStepPostambleL
 */
 
TVerdict RTestMediaClientVideoDisplay::DoTestStepPostambleL()
    {
    return RTestVclnt2AviPlayerStep::DoTestStepPostambleL();        
    }

/**
 * RTestMediaClientVideoDisplay::FsmL
 */
void RTestMediaClientVideoDisplay::FsmL(TVclntTestPlayEvents aEventCode)
    {
    if (FsmCheck(aEventCode))
        {
        switch (aEventCode)
            {
            case EVPIdle:
                HandleIdleL();
                break;
                
            case EVPOpenComplete:
                HandleOpenCompleteL();
                break;
                
            case EVPPrepareComplete:                    
                HandlePrepareCompleteL();
                break;
                
            case EVPPlayComplete:
                HandlePlayCompleteL();
                break;
            }
        }
    }
    
void RTestMediaClientVideoDisplay::HandleIdleL()
	{
	// Open iVideoPlayer
    INFO_PRINTF2(_L("iVideoPlayer2->OpenFileL() %S"), &iFilename);
    iVideoPlayer2->OpenFileL(iFilename, KMmfTestAviPlayControllerUid);
    PrepareState(EVPOpenComplete, KErrNone);
	}

void RTestMediaClientVideoDisplay::HandleOpenCompleteL()
	{
	// Prepare iVideoPlayer2
    INFO_PRINTF1(_L("iVideoPlayer2->Prepare()"));
    iVideoPlayer2->Prepare();
    PrepareState(EVPPrepareComplete, KErrNone);
	}
	
void RTestMediaClientVideoDisplay::HandlePrepareCompleteL()
	{
	// trigger the video to start playing  
	StartPlayback();
	
	TInt displayId = iScreen->GetScreenNumber();
	TRect clipRect = TRect(iWindow->Size());
	TRect cropRegion = clipRect;
	
	// ==========================
	
    iMediaClientVideoDisplay = CMediaClientVideoDisplay::NewL(displayId);
    
 	RSurfaceManager surfaceManager;
 	User::LeaveIfError(surfaceManager.Open());
	
	RSurfaceManager::TSurfaceCreationAttributesBuf bf;
	RSurfaceManager::TSurfaceCreationAttributes& b=bf();
	b.iSize.iWidth = 320;
	b.iSize.iHeight = 240;
	b.iBuffers = 1;
	b.iPixelFormat = EUidPixelFormatXRGB_8888;	
	b.iStride = 320 * 4;
	b.iOffsetToFirstBuffer = 0;
	b.iAlignment=4;
	b.iContiguous=ETrue;
	
	TRect cropRect(0, 0, 320, 240);
	TVideoAspectRatio par(1,2);
	TSurfaceId surfaceId;
	
	User::LeaveIfError(surfaceManager.CreateSurface(bf, surfaceId));
 	
    iMediaClientVideoDisplay->SurfaceCreated(surfaceId, cropRect, par, cropRegion);
    INFO_PRINTF1(_L("iMediaClientVideoDisplay->SurfaceCreated()"));
    
    delete iMediaClientVideoDisplay;
    iMediaClientVideoDisplay = NULL;
    
    // ==================
    
    iMediaClientVideoDisplay = CMediaClientVideoDisplay::NewL(displayId);
    
	TRect videoExtent = clipRect;
	TReal32 scaleWidth(100.0f);
	TReal32 scaleHeight(100.0f);
	TInt horizPos(EHorizontalAlignCenter);
	TInt vertPos(EVerticalAlignCenter);	
	
	TReal32 widthPercentage(50.0f); 
	TReal32 heightPercentage(50.0f);
	TReal32 widthP; 
	TReal32 heightP;
	RWindow newWindow;
	
	// Check for invalid rotation when adding new window
	INFO_PRINTF1(_L("Testing invalid rotations for AddDisplayWindowL"));
	TInt err = KErrNone;
	TVideoRotation rotation = static_cast<TVideoRotation>(-1);
	TRAP(err, iMediaClientVideoDisplay->AddDisplayWindowL(iWindow, clipRect, cropRegion, videoExtent, scaleWidth, scaleHeight, rotation, EAutoScaleBestFit, horizPos, vertPos, iWindow));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL() did not leave with KErrArgument for negative rotation"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}

	rotation = static_cast<TVideoRotation>(4);
	TRAP(err, iMediaClientVideoDisplay->AddDisplayWindowL(iWindow, clipRect, cropRegion, videoExtent, scaleWidth, scaleHeight, rotation, EAutoScaleBestFit, horizPos, vertPos, iWindow));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL() did not leave with KErrArgument for out of range rotation"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}

	rotation = EVideoRotationNone;

	// Check for invalid autoScaleType when adding new window
	INFO_PRINTF1(_L("Testing invalid autoScaleType for AddDisplayWindowL"));
	TAutoScaleType autoScaleType = static_cast<TAutoScaleType>(-1);
	TRAP(err, iMediaClientVideoDisplay->AddDisplayWindowL(iWindow, clipRect, cropRegion, videoExtent, scaleWidth, scaleHeight, rotation, autoScaleType, horizPos, vertPos, iWindow));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL() did not leave with KErrArgument for negative scale"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}

	autoScaleType = static_cast<TAutoScaleType>(4);
	TRAP(err, iMediaClientVideoDisplay->AddDisplayWindowL(iWindow, clipRect, cropRegion, videoExtent, scaleWidth, scaleHeight, rotation, autoScaleType, horizPos, vertPos, iWindow));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL() did not leave with KErrArgument for out of range scale"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}
	
	iMediaClientVideoDisplay->AddDisplayWindowL(iWindow, clipRect, cropRegion, videoExtent, scaleWidth, scaleHeight, rotation, EAutoScaleBestFit, horizPos, vertPos, iWindow);
	INFO_PRINTF1(_L("iMediaClientVideoDisplay->AddDisplayWindowL()"));   

	// Check for invalid rotation when setting rotation for window
	INFO_PRINTF1(_L("Testing invalid rotation for SetRotationL"));
	TVideoRotation videoRotation = static_cast<TVideoRotation>(-1);
	TRAP(err, iMediaClientVideoDisplay->SetRotationL(*iWindow, videoRotation, cropRegion));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->SetRotationL(window) did not leave with KErrArgument for negative value"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}

	videoRotation = static_cast<TVideoRotation>(4);
	TRAP(err, iMediaClientVideoDisplay->SetRotationL(*iWindow, videoRotation, cropRegion));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->SetRotationL(window) did not leave with KErrArgument for out of range value"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}

	// Check for invalid rotation when setting rotation for all windows
	videoRotation = static_cast<TVideoRotation>(-1);
	TRAP(err, iMediaClientVideoDisplay->SetRotationL(videoRotation, cropRegion));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->SetRotationL() did not leave with KErrArgument for negative value"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}

	videoRotation = static_cast<TVideoRotation>(4);
	TRAP(err, iMediaClientVideoDisplay->SetRotationL(videoRotation, cropRegion));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->SetRotationL() did not leave with KErrArgument for out of range value"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}

	videoRotation = EVideoRotationClockwise180;

    iMediaClientVideoDisplay->SetRotationL(*iWindow, videoRotation, cropRegion);
    INFO_PRINTF1(_L("iMediaClientVideoDisplay->SetRotationL()"));
    
    if (iMediaClientVideoDisplay->RotationL(*iWindow) != videoRotation)
        {
        ERR_PRINTF1(_L("iMediaClientVideoDisplay->RotationL() returns an unexpected value.  Test aborted."));

		iTestStepResult = EInconclusive;
		CActiveScheduler::Stop();
        }
	else
		{
		INFO_PRINTF1(_L("iMediaClientVideoDisplay->RotationL()"));	
		}
    
    iMediaClientVideoDisplay->SetScaleFactorL(*iWindow, widthPercentage, heightPercentage, cropRegion);
    iMediaClientVideoDisplay->SetScaleFactorL(widthPercentage, heightPercentage, cropRegion);
    INFO_PRINTF1(_L("iMediaClientVideoDisplay->SetScaleFactorL()"));
    
    iMediaClientVideoDisplay->SetRotationL(EVideoRotationClockwise90, cropRegion);
    INFO_PRINTF1(_L("iMediaClientVideoDisplay->SetRotationL()"));
    
    iMediaClientVideoDisplay->GetScaleFactorL(*iWindow, widthP, heightP);
    INFO_PRINTF3(_L("iMediaClientVideoDisplay->GetScaleFactorL() %df %df"), &widthP, &heightP);

    iMediaClientVideoDisplay->SetVideoExtentL(*iWindow, videoExtent, cropRegion);
    INFO_PRINTF1(_L("iMediaClientVideoDisplay->SetVideoExtentL()"));
    
    iMediaClientVideoDisplay->SetWindowClipRectL(*iWindow, clipRect, cropRegion);
    INFO_PRINTF1(_L("iMediaClientVideoDisplay->SetWindowClipRectL()"));

	// Check for invalid autoScaleType when setting autoScaleType for window
	INFO_PRINTF1(_L("Testing invalid autoScaleType for SetAutoScaleL"));
	autoScaleType = static_cast<TAutoScaleType>(-1);
	TRAP(err, iMediaClientVideoDisplay->SetAutoScaleL(*iWindow, autoScaleType, horizPos, vertPos, cropRegion));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->SetAutoScaleL(window) did not leave with KErrArgument for negative value"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}

	autoScaleType = static_cast<TAutoScaleType>(4);
	TRAP(err, iMediaClientVideoDisplay->SetAutoScaleL(*iWindow, autoScaleType, horizPos, vertPos, cropRegion));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->SetAutoScaleL(window) did not leave with KErrArgument for out of range value"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}

	// Check for invalid autoScaleType when setting autoScaleType for all windows
	autoScaleType = static_cast<TAutoScaleType>(-1);
	TRAP(err, iMediaClientVideoDisplay->SetAutoScaleL(autoScaleType, horizPos, vertPos, cropRegion));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->SetAutoScaleL() did not leave with KErrArgument for negative value"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}

	autoScaleType = static_cast<TAutoScaleType>(4);
	TRAP(err, iMediaClientVideoDisplay->SetAutoScaleL(autoScaleType, horizPos, vertPos, cropRegion));
	if (err != KErrArgument)
		{
		ERR_PRINTF1(_L("iVideoPlayer2->SetAutoScaleL() did not leave with KErrArgument for out of range value"));
    	iTestStepResult = EFail;
    	CActiveScheduler::Stop();
    	return;
		}
    
    iMediaClientVideoDisplay->SetAutoScaleL(EAutoScaleBestFit, horizPos, vertPos, cropRegion);
    INFO_PRINTF1(_L("iMediaClientVideoDisplay->SetAutoScaleL()"));
    
    iMediaClientVideoDisplay->RedrawWindows(cropRegion);
    INFO_PRINTF1(_L("iMediaClientVideoDisplay->RedrawWindows()"));   
    
	iMediaClientVideoDisplay->SurfaceParametersChanged(surfaceId, cropRect, par);
	INFO_PRINTF1(_L("iMediaClientVideoDisplay->SurfaceParametersChanged()"));
	
	iMediaClientVideoDisplay->RemoveSurface();
    INFO_PRINTF1(_L("iMediaClientVideoDisplay->ReplaceSurface()"));
    
    iMediaClientVideoDisplay->RemoveDisplayWindow(*iWindow);
    INFO_PRINTF1(_L("iMediaClientVideoDisplay->RemoveDisplayWindow()"));
    
    surfaceManager.CloseSurface(surfaceId);
	surfaceId = TSurfaceId::CreateNullId();
	surfaceManager.Close();
    
    delete iMediaClientVideoDisplay;  
    iMediaClientVideoDisplay = NULL;              
   	}

void RTestMediaClientVideoDisplay::HandlePlayCompleteL()
	{
	iTestStepResult = EPass;
	CActiveScheduler::Stop();
	}

void RTestMediaClientVideoDisplay::StartPlayback()
	{
    iError = KErrTimedOut;
    INFO_PRINTF1(_L("iVideoPlayer2->Play()"));
    PrepareState(EVPPlayComplete, KErrNone);
    iVideoPlayer2->Play();
	}
	
#endif

//
// RTestVclnt2PerformActionDuringVideoPlayback
//

/**
 * RTestVclnt2PerformActionDuringVideoPlayback::Constructor
 */
RTestVclnt2PerformActionDuringVideoPlayback::RTestVclnt2PerformActionDuringVideoPlayback(
    const TDesC& aTestName, 
    const TDesC& aSectName, 
    const TDesC& aKeyName, 
    TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2PerformActionDuringVideoPlayback::HandlePlayCompleteL
 */
void RTestVclnt2PerformActionDuringVideoPlayback::HandlePlayCompleteL()
    {
    if (iActionError != KErrNone)
        {
        ERR_PRINTF2(_L("Playback event left with error %d."), iActionError);
        iTestStepResult = EFail;
        }
    else 
        {
        iTestStepResult = EPass;
        }

	CActiveScheduler::Stop();        
    }

/**
 * RTestVclnt2PerformActionDuringVideoPlayback::StartPlayback
 */     
void RTestVclnt2PerformActionDuringVideoPlayback::StartPlayback()
	{
	RTestVclnt2PlayFile::StartPlayback();
	
	iActionError = KErrNone;
    TRAP(iActionError, DoThisActionDuringPlaybackL(*iVideoPlayer2));
	}


//
// RTestVclnt2AddWin
//
/**
 * RTestVclnt2AddWin::Constructor
 */
RTestVclnt2AddWin::RTestVclnt2AddWin(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2AddWin::NewL
 */
RTestVclnt2AddWin* RTestVclnt2AddWin::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclnt2AddWin* self = new (ELeave) RTestVclnt2AddWin(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }
   
/**
 * RTestVclnt2AddWin::HandlePrepareCompleteL
 */
void RTestVclnt2AddWin::HandlePrepareCompleteL()
    {
    // retrieve video size
    TSize videoSize;
    iVideoPlayer2->VideoFrameSizeL(videoSize);                

    if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0505")) == 0)
        {
        // create a clipping rect partially out of screen
        TRect windowRect(iWindow->Size());
        windowRect.SetWidth(windowRect.Width() * 2);
        
        // Add the display window with a partially out of screen clipping rect
        TRAPD(err, iVideoPlayer2->AddDisplayWindowL(iWs, 
                                                    *iScreen, 
                                                    *iWindow, 
                                                    TRect(iWindow->Size()), 
                                                    windowRect));
        if (err != iTestExpectedError)
            {
            ERR_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL() leaves with an unexpected error.  Test failed."));

    		iTestStepResult = EFail;
    		CActiveScheduler::Stop();

    		return;
            }

        // create a clipping rect completely out of screen
        windowRect.SetRect(TPoint(0, 0), iWindow->Size());
        windowRect.Move(windowRect.Width() + 10, 0);
        
        // Add the display window with a completely out of screen clipping rect
        TRAP(err, iVideoPlayer2->AddDisplayWindowL(iWs, 
                                                   *iScreen, 
                                                   *iWindow, 
                                                   TRect(iWindow->Size()), 
                                                   windowRect));
        if (err != iTestExpectedError)
            {
            ERR_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL() leaves with an unexpected error.  Test failed."));
            
    		iTestStepResult = EFail;
    		CActiveScheduler::Stop();

    		return;
            }
        else 
            {
            // Reset the expected error to KErrNone as all expected failure tests have been completed.  
            // Any error that appears after this should fail the test
            iTestExpectedError = KErrNone;
            }

        // after failing to add window display with a partially & a completely out of screen
        // clipping rect, proceed to add a valid clipping rect below            
        }    
    
    if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0401")) == 0 ||
        iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0505")) == 0) 
        {
        // shrink both the video & window size by half
        TRect videoRect(videoSize);     
        videoRect.SetHeight(videoSize.iHeight / 2);
        videoRect.SetWidth(videoSize.iWidth / 2); 
        
        TRect windowRect(iWindow->Size());
        windowRect.SetHeight(iWindow->Size().iHeight / 2);
        windowRect.SetWidth(iWindow->Size().iWidth / 2); 

        // ensure the rect is within the boundary of the window        
        TRect finalRect(windowRect);
        finalRect.Intersection(videoRect);

        // add display window using this valid clipping rect 
        iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, finalRect, finalRect);
        }
    else
        {
        // Invalid test case
        User::Panic(_L("RTestVclnt2AddWin"), KErrArgument);        
        }

    // Start video playback
    StartPlayback();
    }


//
// RTestVclnt2RemoveWin
//

/**
 * RTestVclnt2RemoveWin::Constructor
 */ 
RTestVclnt2RemoveWin::RTestVclnt2RemoveWin(const TDesC& aTestName, 
                                           const TDesC& aSectName,
                                           const TDesC& aKeyName, 
                                           TInt aExpectedError,
                                           TBool aAddBack)
    : RTestVclnt2PerformActionDuringVideoPlayback(aTestName, aSectName, aKeyName, aExpectedError),
      iAddBack(aAddBack)
    {
    }

/**
 * RTestVclnt2RemoveWin::NewL
 */
RTestVclnt2RemoveWin* RTestVclnt2RemoveWin::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TBool aAddBack)
    {
    RTestVclnt2RemoveWin* self = new (ELeave) RTestVclnt2RemoveWin(aTestName, aSectName, aKeyName, aExpectedError, aAddBack);
    return self;
    }

/**
 * RTestVclnt2RemoveWin::HandlePrepareCompleteL
 */
void RTestVclnt2RemoveWin::HandlePrepareCompleteL()
    {
    // retrieve video size
    TSize videoSize;
    iVideoPlayer2->VideoFrameSizeL(videoSize);

    // shrink both the video & window size by half
    TRect videoRect(videoSize);     
    videoRect.SetHeight(videoSize.iHeight / 2);
    videoRect.SetWidth(videoSize.iWidth / 2); 

    TRect windowRect(iWindow->Size());
    windowRect.SetHeight(iWindow->Size().iHeight / 2);
    windowRect.SetWidth(iWindow->Size().iWidth / 2); 

    // ensure the rect is within the boundary of the window        
    TRect finalRect(windowRect);
    finalRect.Intersection(videoRect);

    // add display window using this valid clipping rect 
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, finalRect, finalRect);

    // trigger the video to start playing
    StartPlayback();    
    }

/**
 * RTestVclnt2RemoveWin::DoThisActionDuringPlaybackL
 */ 
void RTestVclnt2RemoveWin::DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer)
    {
    // Play the video clip using a specific display window & video clipping setting 
    // first
    TTimeIntervalMicroSeconds duration = aPlayer.DurationL();        

    // After a quarter of the clip's duration
    User::After(duration.Int64() / 4);

    // remove the original display        
    aPlayer.RemoveDisplayWindow(*iWindow);


    if (iAddBack)
    	{
	    // Add the window using default values to continues
	    aPlayer.AddDisplayWindowL(iWs, *iScreen, *iWindow);
    	}
    }


//
// RTestVclnt2WinPos
//

/**
 * RTestVclnt2WinPos::Constructor
 */
RTestVclnt2WinPos::RTestVclnt2WinPos(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclnt2PerformActionDuringVideoPlayback(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2WinPos::NewL
 */
RTestVclnt2WinPos* RTestVclnt2WinPos::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclnt2WinPos* self = new (ELeave) RTestVclnt2WinPos(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclnt2WinPos::DoThisActionDuringPlaybackL
 */
void RTestVclnt2WinPos::DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer)
    {
    TSize windowSize(iWindow->Size().iWidth / 2, iWindow->Size().iHeight / 2);
            
    // shrink the window clipping rect by half the height & width            
    aPlayer.SetWindowClipRectL(*iWindow, TRect(windowSize));    
    }


//
// RTestVclnt2VideoExt
//

/**
 * RTestVclnt2VideoExt::Constructor
 */
RTestVclnt2VideoExt::RTestVclnt2VideoExt(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclnt2PerformActionDuringVideoPlayback(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2VideoExt::NewL
 */
RTestVclnt2VideoExt* RTestVclnt2VideoExt::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclnt2VideoExt* self = new (ELeave) RTestVclnt2VideoExt(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclnt2VideoExt::DoThisActionDuringPlaybackL
 */
void RTestVclnt2VideoExt::DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer)
    {
    TSize windowSize(iWindow->Size().iWidth / 2, iWindow->Size().iHeight / 2);
            
    // shrink the video extent by half the height & width            
    aPlayer.SetVideoExtentL(*iWindow, TRect(windowSize));
    }


//
// RTestVclnt2ClipRect
//

/**
 * RTestVclnt2ClipRect::Constructor
 */
RTestVclnt2ClipRect::RTestVclnt2ClipRect(const TDesC& aTestName, 
                                         const TDesC& aSectName,
                                         const TDesC& aKeyName, 
                                         TInt aExpectedError)
    : RTestVclnt2PerformActionDuringVideoPlayback(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2ClipRect::NewL
 */
RTestVclnt2ClipRect* RTestVclnt2ClipRect::NewL(const TDesC& aTestName, 
                                               const TDesC& aSectName,
                                               const TDesC& aKeyName, 
                                               TInt aExpectedError)
    {
    RTestVclnt2ClipRect* self 
        = new (ELeave) RTestVclnt2ClipRect(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclnt2ClipRect::DoThisActionDuringPlaybackL
 */
void RTestVclnt2ClipRect::DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer)
    {
    TRect clipRect;    
    if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0405")) == 0)
        {
        // shrink the current window clipping rect by half the height & width 
        clipRect.Resize(iWindow->Size().iWidth / 2, iWindow->Size().iHeight / 2);            
        }
    else if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0503")) == 0)
        {
        // grow the current window clipping rect by double the width         
        clipRect.Resize(iWindow->Size().iWidth * 2, iWindow->Size().iHeight);
        }
    else if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0504")) == 0)
        {
        // move the current window clipping rect to be out of the window
        clipRect.SetSize(iWindow->Size());
        clipRect.Move(TPoint(iWindow->Size().iWidth + 1, 0));
        }
    else
        {
        User::Panic(_L("RTestVclnt2ClipRect"), KErrArgument);
        }
    
    // change the window clipping of the display window
    TRAPD(err, aPlayer.SetWindowClipRectL(*iWindow, clipRect));
    if (err != iTestExpectedError)
        {
        if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0503")) == 0 || 
            iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0504")) == 0)
            {
            // for negative tests, stop the test if the failure is not expected
            ERR_PRINTF1(_L("iVideoPlayer2->SetWindowClipRectL() did not failed as expected.  Test failed."));

    		iTestStepResult = EInconclusive;
    		CActiveScheduler::Stop();

    		return;
            }
        else
            {
            // fail only if error is returned for positive tests
            User::LeaveIfError(err);
            }
        }
    else
        {        
        // Reset the expected error to KErrNone as all expected failure tests have been completed.  
        // Any error that appears after this should fail the test
        iTestExpectedError = KErrNone;
        }
    }

//
// RTestVclnt2Scale
//

/**
 * RTestVclnt2Scale::Constructor
 */
RTestVclnt2Scale::RTestVclnt2Scale(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    : RTestVclnt2PerformActionDuringVideoPlayback(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2Scale::NewL
 */
RTestVclnt2Scale* RTestVclnt2Scale::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclnt2Scale* self = new (ELeave) RTestVclnt2Scale(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclnt2Scale::DoThisActionDuringPlaybackL
 */
void RTestVclnt2Scale::DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer)
    {
    // Change the scale factor during playback
    aPlayer.SetScaleFactorL(*iWindow, 150, 150);
    }

/**
 * RTestVclnt2Scale::HandlePrepareCompleteL
 */ 
void RTestVclnt2Scale::HandlePrepareCompleteL()
    {
    // Continue to setup the video utility and then trigger playback
    RTestVclnt2PerformActionDuringVideoPlayback::HandlePrepareCompleteL();
    
    TReal32 scaleFactor = 50.0;
     
    // set scale factor to less than 100%
    iVideoPlayer2->SetScaleFactorL(*iWindow, scaleFactor, scaleFactor);

    // get scale factor 
    TReal32 widthScale;
    TReal32 heightScale;
    iVideoPlayer2->GetScaleFactorL(*iWindow, widthScale, heightScale);

    if (widthScale != scaleFactor || heightScale != scaleFactor)
        {
        ERR_PRINTF1(_L("iVideoPlayer2->GetScaleFactorL() returns unexpected values.  Test aborted."));
        
		iTestStepResult = EInconclusive;
		CActiveScheduler::Stop();
		
		return;
        }    
    }


//
// RTestVclnt2Viewport
//

/**
 * RTestVclnt2Viewport::Constructor
 */
RTestVclnt2Viewport::RTestVclnt2Viewport(const TDesC& aTestName,
                                         const TDesC& aSectName,
                                         const TDesC& aKeyName, 
                                         const TSize& aViewportSize,
                                         TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError),
      iViewportSize(aViewportSize)
    {
    }

/**
 * RTestVclnt2Viewport::NewL
 */
RTestVclnt2Viewport* RTestVclnt2Viewport::NewL(const TDesC& aTestName, 
                                               const TDesC& aSectName,
                                               const TDesC& aKeyName, 
                                               const TSize& aViewportSize,
                                               TInt aExpectedError)
    {
    RTestVclnt2Viewport* self = new (ELeave) RTestVclnt2Viewport(aTestName, 
                                                                 aSectName, 
                                                                 aKeyName, 
                                                                 aViewportSize,
                                                                 aExpectedError); 
    return self;
    }

void RTestVclnt2Viewport::HandlePrepareCompleteL()
	{
	// retrive original video size
    TSize videoSize;
    iVideoPlayer2->VideoFrameSizeL(videoSize);
        
    // crop the top left quarter of the video
    TRect cropRegion;
    if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0508")) == 0)
        {
        // Crop the right half of the video
        cropRegion.SetHeight(videoSize.iHeight);        
        cropRegion.SetWidth(videoSize.iWidth / 2);
        cropRegion.Move(cropRegion.Width(), 0);
        }
    else
        {
        cropRegion.SetWidth(iViewportSize.iWidth == -1 ? videoSize.iWidth : iViewportSize.iWidth);
        cropRegion.SetHeight(iViewportSize.iHeight == -1 ? videoSize.iHeight : iViewportSize.iHeight);
        }
    
    TRect videoExtentRect;
    TRect clipRegion;
    
    if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0508")) == 0)
        {
        videoExtentRect.SetRect(TPoint(0, 0), videoSize);
        clipRegion.SetRect(TPoint(0, 0), videoSize); 
        clipRegion.SetWidth(clipRegion.Width() / 2);
        clipRegion.Move(clipRegion.Width(), 0);
        }
    else 
        {
        // Calculate the video extent to be exactly the same size as the cropped video picture, 
        // placed at the center of the window
        TInt offsetX = (iWindow->Size().iWidth - cropRegion.Width()) / 2;
        TInt offsetY = (iWindow->Size().iHeight - cropRegion.Height()) / 2;
        videoExtentRect.SetRect(TPoint(offsetX, offsetY), cropRegion.Size());                    

        // Set the window clipping region to the top vertical half of the cropped image, originating
        // from the top left corner of the window                    
        clipRegion = videoExtentRect;                    
        clipRegion.SetHeight(videoExtentRect.Height() / 2);
        }
    
    HandlePrepareCompleteL(cropRegion, videoExtentRect, clipRegion);
    
    if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0508")) == 0)
    	{
    	// Align the image to the left of the window
        iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignLeft, 0);
    	}
    // trigger the video to start playing                
    StartPlayback();
	}

void RTestVclnt2Viewport::HandlePrepareCompleteL(const TRect& aCropRegion, 
                                                 const TRect& aVideoExtent, 
                                                 const TRect& aClipRect)
    {
    iVideoPlayer2->SetCropRegionL(aCropRegion);
    
    // get the video crop region to ensure the crop region is set
    TRect currentCropRegion;
    iVideoPlayer2->GetCropRegionL(currentCropRegion);
    
    if (currentCropRegion != aCropRegion)
        {
        ERR_PRINTF1(_L("iVideoPlayer2->GetCropRegionL() returns an unexpected value.  Test aborted."));
        
		iTestStepResult = EInconclusive;
		CActiveScheduler::Stop();
        }
        
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, aVideoExtent, aClipRect);                    
    }

//
// RTestVclnt2InvalidViewport
//

/**
 * RTestVclnt2InvalidViewport::Constructor
 */
RTestVclnt2InvalidViewport::RTestVclnt2InvalidViewport(const TDesC& aTestName,
                                                       const TDesC& aSectName,
                                                       const TDesC& aKeyName, 
                                                       TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2InvalidViewport::NewL
 */
RTestVclnt2InvalidViewport* RTestVclnt2InvalidViewport::NewL(const TDesC& aTestName, 
                                                             const TDesC& aSectName,
                                                             const TDesC& aKeyName, 
                                                             TInt aExpectedError)
    {
    RTestVclnt2InvalidViewport* self = new (ELeave) RTestVclnt2InvalidViewport(aTestName, 
                                                                               aSectName, 
                                                                               aKeyName, 
                                                                               aExpectedError); 
    return self;
    }

/**
 * RTestVclnt2InvalidViewport::HandlePrepareCompleteL
 */
void RTestVclnt2InvalidViewport::HandlePrepareCompleteL()
    {
	// retrive original video size
    TSize videoSize;
    iVideoPlayer2->VideoFrameSizeL(videoSize);

    // set the crop region to be completely out of the video image bound 
    TRect cropRegion;
    cropRegion.SetRect(TPoint(videoSize.iWidth, videoSize.iHeight), TSize(100, 100));

    // continue playback even if this fails
    TRAPD(err, iVideoPlayer2->SetCropRegionL(cropRegion));
    if (err == iTestExpectedError)
        {
        // the invalid crop area is not supposed to be set 
        INFO_PRINTF1(_L("iVideoPlayer2->SetCropRegionL() returned expected.  Test continues."));

        // reset the expected error.  Any error that appears after this should fail the test
        iTestExpectedError = KErrNone;
        }
    else 
        {
        // the invalid crop area is not supposed to be set 
        ERR_PRINTF1(_L("iVideoPlayer2->SetCropRegionL() didn't leave with expected error.  Test cannot be continued."));

		iTestStepResult = EInconclusive;
		CActiveScheduler::Stop();
		return;
        }

    // get the video crop region to ensure the crop region is set
    TRect currentCropRegion;
    iVideoPlayer2->GetCropRegionL(currentCropRegion);

    if (currentCropRegion != cropRegion)
        {
        // the invalid crop area is not supposed to be set 
        ERR_PRINTF1(_L("iVideoPlayer2->GetCropRegionL() returns an unexpected value.  Test cannot be continued."));
        
		iTestStepResult = EInconclusive;
		CActiveScheduler::Stop();
		return;
        }

    // setup the display then trigger the video to start playing
    RTestVclnt2PlayFile::HandlePrepareCompleteL();
    }


//
// RTestVclnt2Rotation
//
/**
 * RTestVclnt2Rotation::Constructor
 */
RTestVclnt2Rotation::RTestVclnt2Rotation(const TDesC& aTestName,
                                         const TDesC& aSectName,
                                         const TDesC& aKeyName, 
                                         TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2Rotation::NewL
 */
RTestVclnt2Rotation* RTestVclnt2Rotation::NewL(const TDesC& aTestName, 
                                               const TDesC& aSectName,
                                               const TDesC& aKeyName, 
                                               TInt aExpectedError)
    {
    RTestVclnt2Rotation* self = new (ELeave) RTestVclnt2Rotation(aTestName, 
                                                                 aSectName, 
                                                                 aKeyName, 
                                                                 aExpectedError);
    return self;
    }

/**
 * RTestVclnt2Rotation::HandlePrepareCompleteL
 */
void RTestVclnt2Rotation::HandlePrepareCompleteL()
    {
    // call parent's HandlePrepareCompleteL to finish off all preparation
    RTestVclnt2PlayFile::HandlePrepareCompleteL();
    
    // set the video to be displayed upside-down
    TVideoRotation videoRotation(EVideoRotationClockwise180);
    iVideoPlayer2->SetRotationL(*iWindow, videoRotation);

    // retrieve the rotation setting, and check if it's been properly updated
    if (iVideoPlayer2->RotationL(*iWindow) != videoRotation)
        {
        ERR_PRINTF1(_L("iVideoPlayer2->RotationL() returns an unexpected value.  Test aborted."));

		iTestStepResult = EInconclusive;
		CActiveScheduler::Stop();

		return;
        }
    }

//
// RTestVclnt2Settings
//
/**
 * RTestVclnt2Settings::Constructor
 */
RTestVclnt2Settings::RTestVclnt2Settings(const TDesC& aTestName,
                                         const TDesC& aSectName,
                                         const TDesC& aKeyName, 
                                         TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2Settings::NewL
 */
RTestVclnt2Settings* RTestVclnt2Settings::NewL(const TDesC& aTestName, 
                                               const TDesC& aSectName,
                                               const TDesC& aKeyName, 
                                               TInt aExpectedError)
    {
    RTestVclnt2Settings* self = new (ELeave) RTestVclnt2Settings(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }
   
/**
 * RTestVclnt2Settings::HandlePrepareCompleteL
 */
void RTestVclnt2Settings::HandlePrepareCompleteL()
    {
    // retrieve the current frame size
    TSize frameSize;
    iVideoPlayer2->VideoFrameSizeL(frameSize);    
    
    TRect cropRegion(frameSize);
    // shrink the crop region to half its original height
    cropRegion.SetHeight(cropRegion.Height() / 2);
    iVideoPlayer2->SetCropRegionL(cropRegion);
    
    TRect videoExtentRect;
    videoExtentRect.SetWidth(cropRegion.Width());
    videoExtentRect.SetHeight(cropRegion.Height());
    
    videoExtentRect.Move((iWindow->Size().iWidth - videoExtentRect.Width()) / 2,
                         (iWindow->Size().iHeight - videoExtentRect.Height()) / 2); 
    
    TRect clippingRect(videoExtentRect);
    clippingRect.SetWidth(clippingRect.Height() / 2);

    // add display window using this valid clipping rect 
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, videoExtentRect, clippingRect);

 	// Scale the movie with best fit scaling, and vertically center the picture
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleBestFit);
    
    // Start video playback
    StartPlayback();
    }



//
// RTestVclnt2SettingsAndAlignments
//
/**
 * RTestVclnt2SettingsAndAlignments::Constructor
 */
RTestVclnt2SettingsAndAlignments::RTestVclnt2SettingsAndAlignments(const TDesC& aTestName,
                                                                   const TDesC& aSectName,
                                                                   const TDesC& aKeyName, 
                                                                   TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2SettingsAndAlignments::NewL
 */
RTestVclnt2SettingsAndAlignments* RTestVclnt2SettingsAndAlignments::NewL(const TDesC& aTestName, 
                                                                         const TDesC& aSectName,
                                                                         const TDesC& aKeyName, 
                                                                         TInt aExpectedError)
    {
    RTestVclnt2SettingsAndAlignments* self = new (ELeave) RTestVclnt2SettingsAndAlignments(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

TVerdict RTestVclnt2SettingsAndAlignments::DoTestStepPreambleL()
	{
	iCount = 0;
	return RTestVclnt2PlayFile::DoTestStepPreambleL();
	}

/**
 * RTestVclnt2SettingsAndAlignments::HandlePrepareCompleteL
 */
void RTestVclnt2SettingsAndAlignments::HandlePrepareCompleteL()
    {    
    TRect cropRegion;
    SetCropRegionL(cropRegion);

	AddDisplayWindowL(cropRegion);
    
    if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0418")) == 0)
        {            
        SetContentOffsetL();
        }
    else if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0419")) == 0)        
        {
        SetContentAlignmentL();            
        }        

    SetScaleFactorL();
       
    // Start video playback
    StartPlayback();
    }

/**
 * RTestVclnt2SettingsAndAlignments::HandlePlayCompleteL
 */
void RTestVclnt2SettingsAndAlignments::HandlePlayCompleteL()
    {    
    iCount++;
    
    if (iCount == KSettingsAndAlignmentTest)
        {
    	iTestStepResult = EPass;
    	CActiveScheduler::Stop();
        }
    else
        {
        HandlePrepareCompleteL();                                
        }
    }

/**
 * RTestVclnt2SettingsAndAlignments::SetCropRegionL
 */
void RTestVclnt2SettingsAndAlignments::SetCropRegionL(TRect& aCropRegion)
    {
    // retrieve the current frame size
    TSize frameSize;
    iVideoPlayer2->VideoFrameSizeL(frameSize);    
    
    // Framesize is used in this case as crop region is not returning default values
    aCropRegion.SetRect(TPoint(0, 0), frameSize);
    
    // set the crop region 
    switch (iCount % KSettingsAndAlignmentTest)
        {
        case 0:                
            // set the crop region to the top half of the image
            aCropRegion.SetHeight(aCropRegion.Height() / 2);
            iVideoPlayer2->SetCropRegionL(aCropRegion);
            break;

        case 1:
            // set the crop region to the left half of the image
            aCropRegion.SetWidth(aCropRegion.Width() / 2);
            iVideoPlayer2->SetCropRegionL(aCropRegion);
            break;

        default:
            // set the crop region to the bottom, right quarter of the image
            aCropRegion.SetHeight(aCropRegion.Height() / 2);
            aCropRegion.SetWidth(aCropRegion.Width() / 2);
            aCropRegion.Move(aCropRegion.Width(), aCropRegion.Height());                        
            iVideoPlayer2->SetCropRegionL(aCropRegion);
            break;
        }
    }

/**
 * RTestVclnt2SettingsAndAlignments::SetContentOffsetL
 */
void RTestVclnt2SettingsAndAlignments::SetContentOffsetL()
    {
    switch (iCount % KSettingsAndAlignmentTest)
        {
        case 0:
            iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, 0, 100);
            break;
        
        case 1:
            iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, 100, 0);
            break;
        
        default:
            iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, 100, 100);
            break;
        }    
    }

/**
 * RTestVclnt2SettingsAndAlignments::SetContentAlignmentL
 */
void RTestVclnt2SettingsAndAlignments::SetContentAlignmentL()
    {
    switch (iCount % KSettingsAndAlignmentTest)
        {
        case 0:
            iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, 0, EVerticalAlignCenter);
            break;
        
        case 1:
            iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignCenter, 0);
            break;
        
        default:
            iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignCenter, EVerticalAlignCenter);
            break;
        }    
    }
    
/**
 * RTestVclnt2SettingsAndAlignments::SetScaleFactorL
 */
void RTestVclnt2SettingsAndAlignments::SetScaleFactorL()
    {
    switch (iCount % KSettingsAndAlignmentTest)
        {
        case 0:
            iVideoPlayer2->SetScaleFactorL(*iWindow, 1, 200);
            break;
        
        case 1:
            iVideoPlayer2->SetScaleFactorL(*iWindow, 200, 1);
            break;
        
        default:
            iVideoPlayer2->SetScaleFactorL(*iWindow, 200, 200);
            break;
        }    
    }    

/**
 * RTestVclnt2SettingsAndAlignments::AddDisplayWindowL
 */
void RTestVclnt2SettingsAndAlignments::AddDisplayWindowL(const TRect& aCropRegion)
    {
    TRect videoExtentRect(iWindow->Size());
    TRect clippingRect(iWindow->Size());
    
    switch (iCount % KSettingsAndAlignmentTest)
        {
        case 0:
            videoExtentRect.SetWidth(aCropRegion.Width());
            if (iWindow->Size().iWidth > videoExtentRect.Width())
                {                    
                videoExtentRect.Move((iWindow->Size().iWidth - videoExtentRect.Width()) / 2, 0);
                }
            clippingRect = videoExtentRect;
            break;
            
        case 1:
            videoExtentRect.SetHeight(aCropRegion.Height());
            if (iWindow->Size().iHeight > videoExtentRect.Height())
                {                    
                videoExtentRect.Move(0, (iWindow->Size().iHeight - videoExtentRect.Height()) / 2);
                }
            clippingRect = videoExtentRect;
            break;
            
        default:
            clippingRect.SetHeight(clippingRect.Height() / 2);
            clippingRect.SetWidth(clippingRect.Width() / 2);            
            clippingRect.Move(clippingRect.Width(), clippingRect.Height());            
            break;                        
        }
    
    iVideoPlayer2->RemoveDisplayWindow(*iWindow);    
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, videoExtentRect, clippingRect);    
    }
       
    
//
// RTestVclnt2SecDisplay
//
/**
 * RTestVclnt2SecDisplay::Constructor
 */
RTestVclnt2SecDisplay::RTestVclnt2SecDisplay(const TDesC& aTestName,
                                             const TDesC& aSectName,
                                             const TDesC& aKeyName, 
                                             TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2SecDisplay::NewL
 */
RTestVclnt2SecDisplay* RTestVclnt2SecDisplay::NewL(const TDesC& aTestName, 
                                                   const TDesC& aSectName,
                                                   const TDesC& aKeyName, 
                                                   TInt aExpectedError)
    {
    RTestVclnt2SecDisplay* self = new (ELeave) RTestVclnt2SecDisplay(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }
   
/**
 * RTestVclnt2SecDisplay::DoTestStepPreambleL
 */
TVerdict RTestVclnt2SecDisplay::DoTestStepPreambleL()
    {
    TVerdict verdict = RTestVclnt2PlayFile::DoTestStepPreambleL();    
    if (verdict == EPass)
        {
        // make a 2nd device for this session
        iSecondScreen = new (ELeave) CWsScreenDevice(iWs); 
        User::LeaveIfError(iSecondScreen->Construct()); 
        }
    
    return verdict;
    }

/**
 * RTestVclnt2SecDisplay::DoTestStepPostambleL
 */
TVerdict RTestVclnt2SecDisplay::DoTestStepPostambleL()
    {
    delete iSecondScreen;
    
    return EPass;
    }

/**
 * RTestVclnt2SecDisplay::HandlePrepareCompleteL
 */
void RTestVclnt2SecDisplay::HandlePrepareCompleteL()
	{
	// Add display window using default values - ie. video extent & window clipping 
	// defaulted to whole window
	INFO_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL()"));
	iVideoPlayer2->AddDisplayWindowL(iWs, *iSecondScreen, *iWindow);

    // trigger the video to start playing                
    StartPlayback();
	}


//
// RTestVclnt2CropRectPause
//
/**
 * RTestVclnt2CropRectPause::Constructor
 */
RTestVclnt2CropRectPause::RTestVclnt2CropRectPause(const TDesC& aTestName, 
                                                   const TDesC& aSectName, 
                                                   const TDesC& aKeyName, 
                                                   TInt aExpectedError)
    : RTestVclnt2PerformActionDuringVideoPlayback(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2CropRectPause::NewL
 */
RTestVclnt2CropRectPause* RTestVclnt2CropRectPause::NewL(const TDesC& aTestName, 
                                                         const TDesC& aSectName,
                                                         const TDesC& aKeyName, 
                                                         TInt aExpectedError)
    {
    RTestVclnt2CropRectPause* self 
        = new (ELeave) RTestVclnt2CropRectPause(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }
	
/**
 * RTestVclnt2CropRectPause::DoThisActionDuringPlaybackL
 */
void RTestVclnt2CropRectPause::DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer)
    {    
    TRect cropRegion;
	aPlayer.GetCropRegionL(cropRegion);
	
	// change the crop region and continue to play the clip
	cropRegion.SetHeight(cropRegion.Height() / 2);
    aPlayer.SetCropRegionL(cropRegion);    
    }
    
    
    
//
// RTestVclnt2Overlay
//
/**
 * RTestVclnt2Overlay::Constructor
 */
RTestVclnt2Overlay::RTestVclnt2Overlay(const TDesC& aTestName,
                                       const TDesC& aSectName,
                                       const TDesC& aKeyName, 
                                       TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2Overlay::NewL
 */
RTestVclnt2Overlay* RTestVclnt2Overlay::NewL(const TDesC& aTestName, 
                                             const TDesC& aSectName,
                                             const TDesC& aKeyName, 
                                             TInt aExpectedError)
    {
    RTestVclnt2Overlay* self = new (ELeave) RTestVclnt2Overlay(aTestName, 
                                                               aSectName, 
                                                               aKeyName, 
                                                               aExpectedError);
    return self;
    }

/**
 * RTestVclnt2Overlay::DoTestStepPreambleL
 */
TVerdict RTestVclnt2Overlay::DoTestStepPreambleL()
    {
    TVerdict verdict = RTestVclnt2PlayFile::DoTestStepPreambleL();    
    if (verdict == EPass)
        {
        TFontSpec fontSpec; // use default fonts
        fontSpec.iHeight = 20;

        // Get a font to draw overlay text
        if (iScreen->GetNearestFontToDesignHeightInPixels(iFont, fontSpec) != KErrNone)
            {
            // if no font can be retrieved, abort the test
            ERR_PRINTF1(_L("Cannot retrieve font to perform test.  Test aborted."));
            
    		return EInconclusive;
            }

        iGc->Activate(*iWindow);
        iGc->UseFont(iFont);
        }
    
    return verdict;
    }

/**
 * RTestVclnt2Overlay::DoTestStepPostambleL
 */
TVerdict RTestVclnt2Overlay::DoTestStepPostambleL()
    {
    iGc->DiscardFont();    
    iGc->Deactivate();                    
               
    return RTestVclnt2PlayFile::DoTestStepPostambleL();        
    }

/**
 * RTestVclnt2Overlay::HandlePrepareCompleteL
 */
void RTestVclnt2Overlay::HandlePrepareCompleteL()
	{
    TRect rect(iWindow->Size());
    TInt baseline = (rect.Height() + iFont->AscentInPixels()) >> 1;
    
    // Draw the overlay text if needed
    iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
    iGc->SetBrushColor(KRgbRed);
         
    iGc->SetPenStyle(CGraphicsContext::ESolidPen);
    iGc->SetPenColor(KRgbBlue);
    
    iGc->DrawText(_L("Overlay Testing"), rect, baseline, CGraphicsContext::ELeft);

    // setup the display window and trigger the video to start playing
    RTestVclnt2PlayFile::HandlePrepareCompleteL();
	}
	

//
// RTestVclnt2AddWin2
//

/**
 * RTestVclnt2AddWin2::Constructor
 */
RTestVclnt2AddWin2::RTestVclnt2AddWin2(const TDesC& aTestName,
                                       const TDesC& aSectName,
                                       const TDesC& aKeyName, 
                                       TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2AddWin2::NewL
 */
RTestVclnt2AddWin2* RTestVclnt2AddWin2::NewL(const TDesC& aTestName, 
                                             const TDesC& aSectName,
                                             const TDesC& aKeyName, 
                                             TInt aExpectedError)
    {
    RTestVclnt2AddWin2* self = new (ELeave) RTestVclnt2AddWin2(aTestName, 
                                                               aSectName, 
                                                               aKeyName, 
                                                               aExpectedError);
    return self;
    }

/**
 * RTestVclnt2AddWin2::HandlePrepareCompleteL
 */
void RTestVclnt2AddWin2::HandlePrepareCompleteL()
	{
    // add the display window for the player to use for displaying the video
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow);
    
    // Add the same display window again
    TRAPD(err, iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
    if (err != iTestExpectedError)
        {
        ERR_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL() leaves with an unexpected error.  Test failed."));

		iTestStepResult = EFail;
		CActiveScheduler::Stop();

		return;
        }
    else
        {
        INFO_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL() leaves with an expected error.  Test continues."));

        // reset the expected error.  Any error that appears after this should fail the test
        iTestExpectedError = KErrNone;        
        }

    // trigger the video to start playing
    StartPlayback();
	}

//
// RTestVclnt2AddWin2WithSettings
//

/**
 * RTestVclnt2AddWin2WithSettings::Constructor
 */
RTestVclnt2AddWin2WithSettings::RTestVclnt2AddWin2WithSettings(const TDesC& aTestName,
                                                               const TDesC& aSectName,
                                                               const TDesC& aKeyName, 
                                                               TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
    {
    }

/**
 * RTestVclnt2AddWin2WithSettings::NewL
 */
RTestVclnt2AddWin2WithSettings* RTestVclnt2AddWin2WithSettings::NewL(const TDesC& aTestName, 
                                                                     const TDesC& aSectName,
                                                                     const TDesC& aKeyName,     
                                                                     TInt aExpectedError)
    {
    RTestVclnt2AddWin2WithSettings* self = new (ELeave) RTestVclnt2AddWin2WithSettings(aTestName, 
                                                                                       aSectName, 
                                                                                       aKeyName, 
                                                                                       aExpectedError);
    return self;
    }

/**
 * RTestVclnt2AddWin2WithSettings::HandlePrepareCompleteL
 */
void RTestVclnt2AddWin2WithSettings::HandlePrepareCompleteL()
	{
	TRect windowRect(iWindow->Size());
	windowRect.SetHeight(windowRect.Height() / 2);

	iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, windowRect, windowRect);

	// Add the same display window again with default values
	TRAPD(err, iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != iTestExpectedError)
	    {
	    ERR_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL() leaves with an unexpected error.  Test failed."));

		iTestStepResult = EFail;
		CActiveScheduler::Stop();

		return;
	    }
    else
        {
        INFO_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL() leaves with an expected error.  Test continues."));

        // reset the expected error.  Any error that appears after this should fail the test
        iTestExpectedError = KErrNone;
	    }

	// trigger the video to start playing
    StartPlayback();
	}

//
// RTestVclnt2ContOffset
//

/**
 * RTestVclnt2ContOffset::Constructor
 */
RTestVclnt2ContOffset::RTestVclnt2ContOffset(const TDesC& aTestName, 
                                             const TDesC& aSectName, 
                                             const TDesC& aKeyName, 
                                             TInt aExpectedError)
    : RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
    {
    iHeapSize = 2000000; //~2MB
    }

/**
 * RTestVclnt2ContOffset::NewL
 */
RTestVclnt2ContOffset* RTestVclnt2ContOffset::NewL(const TDesC& aTestName, 
                                                   const TDesC& aSectName,
                                                   const TDesC& aKeyName, 
                                                   TInt aExpectedError)
    {
    RTestVclnt2ContOffset* self = new (ELeave) RTestVclnt2ContOffset(aTestName, 
                                                                     aSectName, 
                                                                     aKeyName, 
                                                                     aExpectedError);
    return self;
    }

/**
 * RTestVclnt2ContOffset::HandlePrepareCompleteL
 */
void RTestVclnt2ContOffset::HandlePrepareCompleteL()
	{
    // call parent's HandlePrepareCompleteL to finish off all preparation    
	RTestVclnt2PlayFile::HandlePrepareCompleteL();
    // Set the content offset before starting to play the video.
    INFO_PRINTF1(_L("iVideoPlayer2->SetAutoScale()"));
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, 100, 100);
    
    if (iTestStepName.CompareF(_L("MM-MMF-VCLNTAVI-I-0409")) == 0)
        {
        // Set the content alignment after setting the content offset
        iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignLeft, EVerticalAlignTop);
        }
	}

//
// RTestVclnt2ContOffsetAtPlay
//
/**
 * RTestVclnt2ContOffsetAtPlay::Constructor
 */
RTestVclnt2ContOffsetAtPlay::RTestVclnt2ContOffsetAtPlay(const TDesC& aTestName, 
                                                         const TDesC& aSectName, 
                                                         const TDesC& aKeyName, 
                                                         TInt aExpectedError)
    : RTestVclnt2PerformActionDuringVideoPlayback(aTestName, aSectName, aKeyName, aExpectedError)
    {
    iHeapSize = 2000000; //~2MB
    }

/**
 * RTestVclnt2ContOffsetAtPlay::NewL
 */
RTestVclnt2ContOffsetAtPlay* RTestVclnt2ContOffsetAtPlay::NewL(const TDesC& aTestName, 
                                                               const TDesC& aSectName,
                                                               const TDesC& aKeyName, 
                                                               TInt aExpectedError)
    {
    RTestVclnt2ContOffsetAtPlay* self = new (ELeave) RTestVclnt2ContOffsetAtPlay(aTestName, 
                                                                                 aSectName, 
                                                                                 aKeyName, 
                                                                                 aExpectedError);
    return self;
    }
        
/**
 * RTestVclnt2ContOffsetAtPlay::DoThisActionDuringPlaybackL
 */
void RTestVclnt2ContOffsetAtPlay::DoThisActionDuringPlaybackL(CVideoPlayerUtility2& /* aPlayer */)
    {
    // Change the scale factor during playback
	INFO_PRINTF1(_L("iVideoPlayer2->SetAutoScale()"));
	iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, 10, 10);    
    }


//
// RTestVclnt2Align
//
/**
 * RTestVclnt2Align::Constructor
 */
RTestVclnt2Align::RTestVclnt2Align(const TDesC& aTestName, 
                                   const TDesC& aSectName, 
                                   const TDesC& aKeyName, 
                                   TInt aExpectedError)
    : RTestVclnt2PerformActionDuringVideoPlayback(aTestName, aSectName, aKeyName, aExpectedError)
    {
    iHeapSize = 2000000; //~2MB
    }

/**
 * RTestVclnt2Align::NewL
 */
RTestVclnt2Align* RTestVclnt2Align::NewL(const TDesC& aTestName, 
                                         const TDesC& aSectName,
                                         const TDesC& aKeyName, 
                                         TInt aExpectedError)
    {
    RTestVclnt2Align* self = new (ELeave) RTestVclnt2Align(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclnt2Align::HandlePrepareCompleteL
 */ 
void RTestVclnt2Align::HandlePrepareCompleteL()
    {
    // Continue to setup the video utility and then trigger playback
    RTestVclnt2PlayFile::HandlePrepareCompleteL();
	// Set the content offset before starting to play the video.
    INFO_PRINTF1(_L("iVideoPlayer2->SetAutoScale()"));
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignCenter, EVerticalAlignCenter);
    }
    
/**
 * RTestVclnt2Align::DoThisActionDuringPlaybackL
 */
void RTestVclnt2Align::DoThisActionDuringPlaybackL(CVideoPlayerUtility2& /* aPlayer */)
    {    
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignLeft, EVerticalAlignCenter);    
    iVideoPlayer2->RemoveDisplayWindow(*iWindow);
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, TRect(10, 10, 100, 100), TRect(0, 3, 4, 220));        
    
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignRight, EVerticalAlignCenter);    
    iVideoPlayer2->RemoveDisplayWindow(*iWindow);
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow);        
        
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignCenter, EVerticalAlignTop);    
    iVideoPlayer2->RemoveDisplayWindow(*iWindow);
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, TRect(10, 10, 100, 100), TRect(0, 5, 5, 200));        
    
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignLeft, EVerticalAlignTop);
    iVideoPlayer2->RemoveDisplayWindow(*iWindow);
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow);        
    
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignRight, EVerticalAlignTop);
    iVideoPlayer2->RemoveDisplayWindow(*iWindow);
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, TRect(0, 0, 100, 100), TRect(0, 5, 5, 200));        
        
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignCenter, EVerticalAlignBottom);
    iVideoPlayer2->RemoveDisplayWindow(*iWindow);
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow);        
    
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignLeft, EVerticalAlignBottom);
    iVideoPlayer2->RemoveDisplayWindow(*iWindow);
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, TRect(0, 0, 100, 100), TRect(0, 5, 300, 230));        
    
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleNone, EHorizontalAlignRight, EVerticalAlignBottom);
    iVideoPlayer2->RemoveDisplayWindow(*iWindow);
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow);        
    }


//
// RTestVclnt2AutoScale
//
/**
 * RTestVclnt2AutoScale::Constructor
 */
RTestVclnt2AutoScale::RTestVclnt2AutoScale(const TDesC& aTestName, 
                                           const TDesC& aSectName, 
                                           const TDesC& aKeyName, 
                                           TInt aExpectedError)
    : RTestVclnt2PerformActionDuringVideoPlayback(aTestName, aSectName, aKeyName, aExpectedError)
    {
    iHeapSize = 2000000; //~2MB
    }

/**
 * RTestVclnt2AutoScale::NewL
 */
RTestVclnt2AutoScale* RTestVclnt2AutoScale::NewL(const TDesC& aTestName, 
                                                 const TDesC& aSectName,
                                                 const TDesC& aKeyName, 
                                                 TInt aExpectedError)
    {
    RTestVclnt2AutoScale* self = new (ELeave) RTestVclnt2AutoScale(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

/**
 * RTestVclnt2AutoScale::HandlePrepareCompleteL
 */ 
void RTestVclnt2AutoScale::HandlePrepareCompleteL()
    {
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow);
	// Set the content offset before starting to play the video.
    INFO_PRINTF1(_L("iVideoPlayer2->SetAutoScale()"));
    iVideoPlayer2->SetAutoScaleL(*iWindow, EAutoScaleClip);
    
    // trigger the video to start playing                
    StartPlayback();
    }
	
/**
 * RTestVclnt2AutoScale::DoThisActionDuringPlaybackL
 */
void RTestVclnt2AutoScale::DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer)
    {    
	aPlayer.SetAutoScaleL(*iWindow, EAutoScaleBestFit);
	aPlayer.SetAutoScaleL(*iWindow, EAutoScaleNone);
	aPlayer.SetAutoScaleL(*iWindow, EAutoScaleStretch);
    }

//
// RTestVclnt2OldController
//

RTestVclnt2OldController::RTestVclnt2OldController(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	: RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

/**
 * RTestVclnt2OldController::NewL
 */
RTestVclnt2OldController* RTestVclnt2OldController::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclnt2OldController* self = new (ELeave) RTestVclnt2OldController(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

void RTestVclnt2OldController::HandleIdleL()
	{
	// Open iVideoPlayer using a controller that does not support surfaces. 
    INFO_PRINTF2(_L("iVideoPlayer2->OpenFileL() %S"), &iFilename);
    iVideoPlayer2->OpenFileL(iFilename, KMmfTestAviNonGcePlayControllerUid);
    PrepareState(EVPOpenComplete, KErrNone);
	}

void RTestVclnt2OldController::HandlePrepareCompleteL()
	{
	TRAPD(err, RTestVclnt2PlayFile::HandlePrepareCompleteL());
	
	if (err != KErrNotSupported)
		{
		ERR_PRINTF1(_L("AddDisplay did not leave with KErrNotSupported."));
		iTestStepResult = EFail;
		}
	else
		{
		iTestStepResult = EPass;
		}
	
	// Stop the test.
	CActiveScheduler::Stop();
	}
//
// RTestVclnt2NoFile
//

RTestVclnt2NoFile::RTestVclnt2NoFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	: RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

/**
 * RTestVclnt2NoFile::NewL
 */
RTestVclnt2NoFile* RTestVclnt2NoFile::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclnt2NoFile* self = new (ELeave) RTestVclnt2NoFile(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

void RTestVclnt2NoFile::HandleIdleL()
	{
	iTestStepResult = EFail;
	TRAPD(err, iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	
	if (err != iTestExpectedError)
		{
		ERR_PRINTF1(_L("AddDisplayWindowL did not leave with KErrNotReady."));
		User::Leave(KErrGeneral);
		}
	
	TSize windowSize = iWindow->Size();
	TRect videoExtent(-5, -5, windowSize.iWidth + 5, windowSize.iHeight + 5);
	TRect windowClipRect(5, 5, windowSize.iWidth - 5, windowSize.iHeight - 5);
	TRAP(err, iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, videoExtent, windowClipRect));
	
	if (err != iTestExpectedError)
		{
		ERR_PRINTF1(_L("AddDisplayWindowL did not leave with KErrNotReady."));
		User::Leave(KErrGeneral);
		}
	
	TRAP(err, iVideoPlayer2->SetWindowClipRectL(*iWindow, windowClipRect));
	
	if (err != iTestExpectedError)
		{
		ERR_PRINTF1(_L("SetWindowClipRectL did not leave with KErrNotReady."));
		User::Leave(KErrGeneral);
		}
	
	TRAP(err, iVideoPlayer2->SetVideoExtentL(*iWindow, videoExtent));
	
	if (err != iTestExpectedError)
		{
		ERR_PRINTF1(_L("SetVideoExtentL did not leave with KErrNotReady."));
		User::Leave(KErrGeneral);
		}
	
	// reset the expected error to KErrNone as any error happens after this is unexpected
    iTestExpectedError = KErrNone;	
	
	// The video should still play when we try to play the video.  Perform 
	// the basic play test now.
	RTestVclnt2PlayFile::HandleIdleL();
	}

//
// RTestVclnt2PlayAfterRemoveWin
//

RTestVclnt2PlayAfterRemoveWin::RTestVclnt2PlayAfterRemoveWin(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	: RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

/**
 * RTestVclnt2PlayAfterRemoveWin::NewL
 */
RTestVclnt2PlayAfterRemoveWin* RTestVclnt2PlayAfterRemoveWin::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclnt2PlayAfterRemoveWin* self = new (ELeave) RTestVclnt2PlayAfterRemoveWin(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

void RTestVclnt2PlayAfterRemoveWin::HandlePrepareCompleteL()
	{
	// Don't add a window. Just play the video.
	StartPlayback();
	}

void RTestVclnt2PlayAfterRemoveWin::HandlePlayCompleteL()
	{
	switch(iPlayAttempt)
		{
	case 0:
		{
		TSize windowSize = iWindow->Size();
		TRect videoExtent(-5, -5, windowSize.iWidth + 5, windowSize.iHeight + 5);
		TRect windowClipRect(5, 5, windowSize.iWidth - 5, windowSize.iHeight - 5);
		iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow, videoExtent, windowClipRect);
		iVideoPlayer2->RemoveDisplayWindow(*iWindow);
		iVideoPlayer2->Play();
		PrepareState(EVPPlayComplete, KErrNone);
		break;
		}
	case 1:
		// Removing the window again should have no effect.
		iVideoPlayer2->RemoveDisplayWindow(*iWindow);
		iVideoPlayer2->Play();
		PrepareState(EVPPlayComplete, KErrNone);
		break;
	case 2:
		// The previous play commands should have no effect when we try to add a 
		// display window and render to surfaces. Use the base class to do this.
		RTestVclnt2PlayFile::HandlePrepareCompleteL();
		break;
	default:
		// Use the parent handler after we have tried the above play commands.
		RTestVclnt2PlayFile::HandlePlayCompleteL();
		break;
		}
	
	iPlayAttempt++;
	}

//
// RTestVclnt2NoGce
//

RTestVclnt2NoGce::RTestVclnt2NoGce(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	: RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

/**
 * RTestVclnt2NoGce::NewL
 */
RTestVclnt2NoGce* RTestVclnt2NoGce::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclnt2NoGce* self = new (ELeave) RTestVclnt2NoGce(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

void RTestVclnt2NoGce::HandleIdleL()
	{
	// Open iVideoPlayer
    INFO_PRINTF2(_L("iVideoPlayer2->OpenFileL() %S"), &iFilename);
    iVideoPlayer2->OpenFileL(iFilename, KMmfTestAviPlayControllerUid);
    PrepareState(EVPOpenComplete, KErrNotSupported);
	}

void RTestVclnt2NoGce::HandleOpenCompleteL()
	{
	iTestStepResult = EPass;
	CActiveScheduler::Stop();
	}
	
//
// RTestVclnt2Alloc
//

RTestVclnt2Alloc::RTestVclnt2Alloc(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	: RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

/**
 * RTestVclnt2Alloc::NewL
 */
RTestVclnt2Alloc* RTestVclnt2Alloc::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclnt2Alloc* self = new (ELeave) RTestVclnt2Alloc(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

TVerdict RTestVclnt2Alloc::DoTestStepPreambleL()
	{
    INFO_PRINTF1(_L("RTestVclnt2Alloc::DoTestStepPreambleL()"));
        
    // Call RTestMmfVclntAviStep::DoTestStepPreambleL instead of the immediate parent's 
    // DoTestStepPreambleL as there is no need for CVideoPlayerUtility setup.
    // Similarly, there is no need to initialize CVideoPlayerUtility2, as it will be
    // created in each alloc iteration.
    return RTestMmfVclntAviStep::DoTestStepPreambleL();
	}

TVerdict RTestVclnt2Alloc::PerformTestL()
	{
	iVideoPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	ResetState();
	iError = KErrNone;
	TVerdict result = RTestVclnt2AviPlayerStep::DoTestStepL();
	delete iVideoPlayer2;
	iVideoPlayer2 = NULL;
	return result;
	}

/**
 * RTestVclnt2Alloc::DoTestStepL()
 */
TVerdict RTestVclnt2Alloc::DoTestStepL()
    {
    TVerdict allocTestStepResult = EPass;
    TInt err = KErrNone;
    TBool result = EFalse;
    
    //>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
    if (PerformTestL() != EPass)
        {
        err = iError;
        }

    if (err != KErrNone)
        {
        INFO_PRINTF2(_L("Test error, returned error code =  %d"), err);
        User::Leave(err);
        }
    else
        {
        //Check the iAllocTestStepResult
        if (allocTestStepResult != EPass)
            {
            result = ETrue;
            }
        }    
    
    TInt failCount = 1;
    TBool completed = EFalse;
    allocTestStepResult = EPass;
    TBool reachedEnd = EFalse;
    for(;;)    
        {
        __UHEAP_SETFAIL(RHeap::EFailNext, failCount);
        __MM_HEAP_MARK;

        //>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
        TVerdict verdict = EFail;
        TRAP(err, verdict = PerformTestL());
        if (err == KErrNone && verdict != EPass)
            {
            err = iError;
            }

        completed = EFalse;
        if (err == KErrNone)
            {
            TAny* testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
            if (testAlloc == NULL)
                {
                reachedEnd = ETrue;
                failCount -= 1;
                }
            else
                {
                User::Free(testAlloc);    
                }            
            
            //Check the iAllocTestStepResult
            if (allocTestStepResult != EPass)
                {
                result = ETrue;
                }
            
            completed = reachedEnd || result;
            }
        else if (err != KErrNoMemory) // bad error code
            {
            completed = ETrue;
            result = EFail;
            }            

        __MM_HEAP_MARKEND;
        __UHEAP_SETFAIL(RHeap::ENone, 0);

        if (completed)
            {
            break; // exit loop
            }

        failCount++;
        }

    failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc

    if (err != KErrNone || result)
        {
        allocTestStepResult = EFail;
        TBuf<80> format;
        if (result)
            {
            format.Format(_L("  Bad result with %d memory allocations tested\n"), failCount);
            }
        else
            {
            format.Format(_L("  Error(%d) with %d memory allocations tested\n"), err, failCount);
            }
        Log(format);
        }
    else 
        {
        TBuf<80> format;
        format.Format(_L("  Completed OK with %d memory allocations tested\n"), failCount);
        Log(format);
        }

    return allocTestStepResult;
    }

//
// RTestVclnt2InvalidScaleFactor
//
RTestVclnt2InvalidScaleFactor::RTestVclnt2InvalidScaleFactor(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
	: RTestVclnt2PlayFile(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

/**
 * RTestVclnt2InvalidScaleFactor::NewL
 */
RTestVclnt2InvalidScaleFactor* RTestVclnt2InvalidScaleFactor::NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError)
    {
    RTestVclnt2InvalidScaleFactor* self = new (ELeave) RTestVclnt2InvalidScaleFactor(aTestName, aSectName, aKeyName, aExpectedError);
    return self;
    }

void RTestVclnt2InvalidScaleFactor::HandlePrepareCompleteL()
	{
	INFO_PRINTF1(_L("iVideoPlayer2->AddDisplayWindowL()"));
	iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow);

	// Try to scale the video to an invalid values
	TRAPD(err, iVideoPlayer2->SetScaleFactorL(*iWindow, 0, 100));
	if (err != KErrArgument)
		{
		User::Leave(err == KErrNone ? KErrGeneral : err); 
		}
	
	TRAP(err, iVideoPlayer2->SetScaleFactorL(*iWindow, 100, -100));
	if (err != KErrArgument)
		{
		User::Leave(err == KErrNone ? KErrGeneral : err); 
		}
	
	// trigger the video to start playing                
    StartPlayback();
	}
