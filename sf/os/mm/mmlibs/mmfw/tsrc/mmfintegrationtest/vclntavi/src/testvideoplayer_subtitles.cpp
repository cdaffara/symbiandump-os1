// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testvideoplayer_subtitles.h"

const TUint KCacheSize = 1000000; //1MB
const TUint KMaxCacheSize = 50000000; //50MB


CPlayTimer* CPlayTimer::NewL(RTestVclnt2Subtitle& aTestStep)
	{
	CPlayTimer* self = new (ELeave) CPlayTimer(aTestStep);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CPlayTimer::CPlayTimer(RTestVclnt2Subtitle& aTestStep)
	: CTimer(EPriorityHigh), iTestStep(aTestStep)
	{
	CActiveScheduler::Add(this);
	}

void CPlayTimer::RunL()
	{
	iTestStep.DoDuringPlaybackActionsL();
	}

TInt CPlayTimer::RunError(TInt aError)
	{
	// Tell the test to fail with error code aError.  
	iTestStep.FailTest(aError);
	return KErrNone;
	}

//
// RTestVclnt2Subtitle
//

RTestVclnt2Subtitle* RTestVclnt2Subtitle::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2Subtitle::RTestVclnt2Subtitle(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	: RTestVclnt2AviPlayerStep(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2Subtitle::FailTest(TInt aError)
	{
	ERR_PRINTF2(_L("Test step failed with error %d."), aError);
	iError = aError;
	iTestStepResult = EFail;
    CActiveScheduler::Stop();
	}

TVerdict RTestVclnt2Subtitle::DoTestStepPreambleL()
	{
	User::LeaveIfError(SetCacheSize());
	TVerdict result = RTestVclnt2AviPlayerStep::DoTestStepPreambleL();
	
	if (result == EPass)
		{
		iPlayTimer = CPlayTimer::NewL(*this);
		}
	
	return result;
	}

TVerdict RTestVclnt2Subtitle::DoTestStepPostambleL()
	{
	User::LeaveIfError(RevertCacheSize());
	
	delete iPlayTimer;
	iPlayTimer = NULL;
	return RTestVclnt2AviPlayerStep::DoTestStepPostambleL();
	}

void RTestVclnt2Subtitle::DoBeforePlaybackActionsL()
	{
	if (!iVideoPlayer2->SubtitlesAvailable())
		{
		ERR_PRINTF1(_L("SubtitlesAvailable() returned EFalse.  Expected ETrue."));
		User::Leave(KErrNotSupported);
		}
	
	iVideoPlayer2->EnableSubtitlesL();
	}

void RTestVclnt2Subtitle::DoDuringPlaybackActionsL()
	{
	}

void RTestVclnt2Subtitle::FsmL(TVclntTestPlayEvents aEventCode)
	{
	if (FsmCheck(aEventCode))
		{
		switch (aEventCode)
			{
		case EVPIdle:
			// Open iVideoPlayer2
			LOG_CALL(iVideoPlayer2->OpenFileL(iFilename, KMmfTestAviPlayControllerUid));
			PrepareState(EVPOpenComplete, KErrNone);
			break;

		case EVPOpenComplete:
			// Prepare iVideoPlayer2
			LOG_CALL(iVideoPlayer2->Prepare());
			PrepareState(EVPPrepareComplete, KErrNone);
			break;

		case EVPPrepareComplete:
			// Add display window using default values - ie. video extent & window clipping 
			// defaulted to whole window.
			LOG_CALL(iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
			
			// Perform any actions that the class or subclasses need before starting video playback.
			DoBeforePlaybackActionsL();
			
			// trigger the video to start playing
			iError = KErrTimedOut;
			PrepareState(EVPPlayComplete, KErrNone);
			LOG_CALL(iVideoPlayer2->Play());
			
			// Trigger a call to DoDuringPlaybackActionsL
			iPlayTimer->After(KOneSecond);
			break;

		case EVPPlayComplete:
			iTestStepResult = EPass;
			iPlayTimer->Cancel();
			CActiveScheduler::Stop();
			break;
			}
		}
	}

TInt RTestVclnt2Subtitle::SetCacheSize()
	{
	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)KCacheSize,(TAny*)KMaxCacheSize);
	TInt ret=CheckCacheError(error);
		
	return ret;
	}

TInt RTestVclnt2Subtitle::CheckCacheError(TInt aError)
	{
	TInt ret=KErrNone;
	#ifdef __WINSCW__
	//Winscw does not support getting cache size. Ignoring -5 error
	if(aError!=KErrNone)
		{
		if(aError!=KErrNotSupported)
			{
			INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
			ret=aError;
			}
		}
	#else
	if(aError!=KErrNone)
		{//For ARMV5 we stop for all errors
		INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
		ret=aError;
		}
	#endif
	return ret;
	}

TInt RTestVclnt2Subtitle::RevertCacheSize()
	{
	TUint defaultCacheSize = 0; 
	//Done with the test. Setting 0 makes the cache size revert back to boot up values
	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)defaultCacheSize,(TAny*)defaultCacheSize);
	return CheckCacheError(error);
  	}


//
// RTestVclnt2SubtitleEnableDuringPlayback
//

RTestVclnt2SubtitleEnableDuringPlayback* RTestVclnt2SubtitleEnableDuringPlayback::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleEnableDuringPlayback(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleEnableDuringPlayback::RTestVclnt2SubtitleEnableDuringPlayback(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleEnableDuringPlayback::DoBeforePlaybackActionsL()
	{
	INFO_PRINTF1(_L("Don't enable subtitles before playback; do nothing in DoBeforePlaybackActionsL()"));
	}

void RTestVclnt2SubtitleEnableDuringPlayback::DoDuringPlaybackActionsL()
	{
	if (!iVideoPlayer2->SubtitlesAvailable())
		{
		ERR_PRINTF1(_L("SubtitlesAvailable() returned EFalse.  Expected ETrue."));
		User::Leave(KErrNotSupported);
		}
	
	iVideoPlayer2->EnableSubtitlesL();
	}

//
// RTestVclnt2SubtitleDisableDuringPlayback
//

RTestVclnt2SubtitleDisableDuringPlayback* RTestVclnt2SubtitleDisableDuringPlayback::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleDisableDuringPlayback(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleDisableDuringPlayback::RTestVclnt2SubtitleDisableDuringPlayback(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleDisableDuringPlayback::DoDuringPlaybackActionsL()
	{
	iVideoPlayer2->DisableSubtitles();
	}

//
// RTestVclnt2SubtitleDisableTwice
//

RTestVclnt2SubtitleDisableTwice* RTestVclnt2SubtitleDisableTwice::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleDisableTwice(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleDisableTwice::RTestVclnt2SubtitleDisableTwice(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleDisableTwice::DoBeforePlaybackActionsL()
	{
	iVideoPlayer2->DisableSubtitles();
	}

void RTestVclnt2SubtitleDisableTwice::DoDuringPlaybackActionsL()
	{
	iVideoPlayer2->DisableSubtitles();
	}

//
// RTestVclnt2SubtitleWindowClipBefore
//

RTestVclnt2SubtitleWindowClipBefore* RTestVclnt2SubtitleWindowClipBefore::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleWindowClipBefore(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleWindowClipBefore::RTestVclnt2SubtitleWindowClipBefore(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleWindowClipBefore::DoBeforePlaybackActionsL()
	{
	RTestVclnt2Subtitle::DoBeforePlaybackActionsL();
		
	TSize windowSize(iWindow->Size().iWidth / 2, iWindow->Size().iHeight / 2);
    
    // shrink the window clipping rect by half the height & width            
    iVideoPlayer2->SetWindowClipRectL(*iWindow, TRect(windowSize));
	}

//
// RTestVclnt2SubtitleWindowClipDuring
//

RTestVclnt2SubtitleWindowClipDuring* RTestVclnt2SubtitleWindowClipDuring::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleWindowClipDuring(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleWindowClipDuring::RTestVclnt2SubtitleWindowClipDuring(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleWindowClipDuring::DoDuringPlaybackActionsL()
	{
	TSize windowSize(iWindow->Size().iWidth / 2, iWindow->Size().iHeight / 2);
    
    // shrink the window clipping rect by half the height & width            
    iVideoPlayer2->SetWindowClipRectL(*iWindow, TRect(windowSize));
	}

//
// RTestVclnt2SubtitleRotateBefore
//

RTestVclnt2SubtitleRotateBefore* RTestVclnt2SubtitleRotateBefore::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleRotateBefore(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleRotateBefore::RTestVclnt2SubtitleRotateBefore(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleRotateBefore::DoBeforePlaybackActionsL()
	{
	RTestVclnt2Subtitle::DoBeforePlaybackActionsL();
    iVideoPlayer2->SetRotationL(*iWindow, EVideoRotationClockwise180);
	}

//
// RTestVclnt2SubtitleRotateDuring
//

RTestVclnt2SubtitleRotateDuring* RTestVclnt2SubtitleRotateDuring::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleRotateDuring(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleRotateDuring::RTestVclnt2SubtitleRotateDuring(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleRotateDuring::DoDuringPlaybackActionsL()
	{
    iVideoPlayer2->SetRotationL(*iWindow, EVideoRotationClockwise180);
	}

//
// RTestVclnt2SubtitleRemoveDisplayDuring
//

RTestVclnt2SubtitleRemoveDisplayDuring* RTestVclnt2SubtitleRemoveDisplayDuring::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleRemoveDisplayDuring(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleRemoveDisplayDuring::RTestVclnt2SubtitleRemoveDisplayDuring(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleRemoveDisplayDuring::DoDuringPlaybackActionsL()
	{
    iVideoPlayer2->RemoveDisplayWindow(*iWindow);
    iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow);
	}

//
// RTestVclnt2SubtitleNotAvailable
//

RTestVclnt2SubtitleNotAvailable* RTestVclnt2SubtitleNotAvailable::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleNotAvailable(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleNotAvailable::RTestVclnt2SubtitleNotAvailable(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleNotAvailable::DoBeforePlaybackActionsL()
	{
	if (iVideoPlayer2->SubtitlesAvailable())
		{
		ERR_PRINTF1(_L("SubtitlesAvailable() returned ETrue.  Expected EFalse."));
		User::Leave(KErrGeneral);
		}
	
	TRAPD(err, iVideoPlayer2->EnableSubtitlesL());
	
	if (KErrNotFound != err)
		{
		ERR_PRINTF3(_L("EnableSubtitlesL() left with %d. Expected %d."), err, KErrNotFound);
		User::Leave(KErrGeneral);
		}
	}

//
// RTestVclnt2SubtitleLanguage
//

RTestVclnt2SubtitleLanguage* RTestVclnt2SubtitleLanguage::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleLanguage(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleLanguage::RTestVclnt2SubtitleLanguage(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleLanguage::DoBeforePlaybackActionsL()
	{
	RTestVclnt2Subtitle::DoBeforePlaybackActionsL();
	
	TLanguage lang = ELangNone;
	TRAPD(err, lang = iVideoPlayer2->SubtitleLanguageL());
	if (KErrNone != err)
		{
		ERR_PRINTF3(_L("SubtitleLanguageL() left with %d. Expected %d."), err, KErrNone);
		User::Leave(err);
		}
	else if (ELangNone != lang)
		{
		// Languages are not supported; ELangNone should be returned.
		ERR_PRINTF3(_L("SubtitleLanguageL() returned %d.  Expected %d."), lang, ELangNone);
		User::Leave(KErrGeneral);
		}
	
	TInt count = -1;
	TRAP(err, count = iVideoPlayer2->SupportedSubtitleLanguagesL().Count());
	if (KErrNone != err)
		{
		ERR_PRINTF3(_L("SupportedSubtitleLanguagesL() left with %d. Expected %d."), err, KErrNone);
		User::Leave(KErrGeneral);
		}
	if (count != 0)
		{
		ERR_PRINTF1(_L("SupportedSubtitleLanguagesL() returned a non-empty array"));
		User::Leave(KErrGeneral);
		}
	
	TRAP(err, iVideoPlayer2->SetSubtitleLanguageL(lang));
	
	if (KErrNotSupported != err)
		{
		ERR_PRINTF3(_L("SetSubtitleLanguageL() left with %d. Expected %d."), err, KErrNone);
		User::Leave(KErrGeneral);
		}
	}

//
// RTestVclnt2SubtitleRedraw
//

RTestVclnt2SubtitleRedraw* RTestVclnt2SubtitleRedraw::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TBool aEnableSubtitles)
	{
	return new (ELeave) RTestVclnt2SubtitleRedraw(aTestName, aSectName, aKeyName, aExpectedError, aEnableSubtitles);
	}

RTestVclnt2SubtitleRedraw::RTestVclnt2SubtitleRedraw(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TBool aEnableSubtitles)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError), iEnableSubtitles(aEnableSubtitles)
	{
	}

void RTestVclnt2SubtitleRedraw::DoBeforePlaybackActionsL()
	{
	if (iEnableSubtitles)
		{
		// The parent class will enable subtitle support
		RTestVclnt2Subtitle::DoBeforePlaybackActionsL();
		}
	}

void RTestVclnt2SubtitleRedraw::DoDuringPlaybackActionsL()
	{
	// Redraw the whole subtitle
	iWindow->Invalidate();
	LOG_CALL(iVideoPlayer2->RedrawSubtitle(*iWindow, TRect(iWindow->Size())));
	}

//
// RTestVclnt2SubtitleRotateClipRedraw
//

RTestVclnt2SubtitleRotateClipRedraw* RTestVclnt2SubtitleRotateClipRedraw::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleRotateClipRedraw(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleRotateClipRedraw::RTestVclnt2SubtitleRotateClipRedraw(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleRotateClipRedraw::DoBeforePlaybackActionsL()
	{
	RTestVclnt2Subtitle::DoBeforePlaybackActionsL();
	
	iVideoPlayer2->SetRotationL(*iWindow, EVideoRotationClockwise90);
    
	// shrink the window clipping rect by half the height & width
	TSize windowSize(iWindow->Size().iWidth / 3, iWindow->Size().iHeight / 2);
	iVideoPlayer2->SetWindowClipRectL(*iWindow, TRect(windowSize));
	}

void RTestVclnt2SubtitleRotateClipRedraw::DoDuringPlaybackActionsL()
	{
	// Redraw the whole subtitle
	iWindow->Invalidate();
	iVideoPlayer2->RedrawSubtitle(*iWindow, TRect(iWindow->Size()));
	}

//
// RTestVclnt2SubtitleMultipleWindows
//

RTestVclnt2SubtitleMultipleWindows* RTestVclnt2SubtitleMultipleWindows::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleMultipleWindows(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleMultipleWindows::RTestVclnt2SubtitleMultipleWindows(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

TVerdict RTestVclnt2SubtitleMultipleWindows::DoTestStepPreambleL()
	{
	TVerdict result = RTestVclnt2Subtitle::DoTestStepPreambleL();
	
	if (result != EPass)
		{
		return result;
		}
	
	iSecondaryWin = new(ELeave) RWindow(iWs);
	   
	// Construct an RWindow with a unique id.  Use this pointer to get a unique number.
	User::LeaveIfError(iSecondaryWin->Construct(iRootWindow, ((TInt) this) + 3));
	iSecondaryWin->SetRequiredDisplayMode(EColor16MA);
	iSecondaryWin->SetExtent(TPoint(0,0), TSize(100,100));
	iSecondaryWin->SetVisible(ETrue);
	iSecondaryWin->SetBackgroundColor(TRgb(0, 0));
	iSecondaryWin->Activate();
	iSecondaryWin->BeginRedraw();
	iSecondaryWin->EndRedraw();
    iWs.Flush();
    
    return EPass;
	}

TVerdict RTestVclnt2SubtitleMultipleWindows::DoTestStepPostambleL()
	{
	
	delete iVideoPlayer2;  
    iVideoPlayer2 = NULL;      
    
    iSecondaryWin->Close();
	delete iSecondaryWin;
    iSecondaryWin = NULL;
        
    // Base DoTestStepPostambleL
    return RTestVclnt2Subtitle::DoTestStepPostambleL();
	}
	
void RTestVclnt2SubtitleMultipleWindows::DoBeforePlaybackActionsL()
	{
	TRAPD(err, iVideoPlayer2->AddDisplayWindowL(iWs, *iScreen, *iSecondaryWin));
	
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("Adding second display window with AddDisplayWindowL() left with %d. Expected %d."), err, KErrNone);
		User::Leave(err);
		}
	
	RTestVclnt2Subtitle::DoBeforePlaybackActionsL();
	}

//
// RTestVclnt2SubtitleEnableTwice
//

RTestVclnt2SubtitleEnableTwice* RTestVclnt2SubtitleEnableTwice::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleEnableTwice(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleEnableTwice::RTestVclnt2SubtitleEnableTwice(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

void RTestVclnt2SubtitleEnableTwice::DoBeforePlaybackActionsL()
	{	
	RTestVclnt2Subtitle::DoBeforePlaybackActionsL();
	
	// Enable subtitles again
	TRAPD(err, iVideoPlayer2->EnableSubtitlesL());
	
	if (KErrInUse != err)
		{
		ERR_PRINTF3(_L("Calling EnableSubtitlesL twice left with %d. Expected %d."), err, KErrInUse);
		// Leave with KErrGeneral if err is no error.  Otherwise leave with err.
		User::Leave(KErrNone == err ? KErrGeneral : err);
		}
	}

//
// RTestVclnt2SubtitleEnableDisableStress
//

RTestVclnt2SubtitleEnableDisableStress* RTestVclnt2SubtitleEnableDisableStress::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleEnableDisableStress(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleEnableDisableStress::RTestVclnt2SubtitleEnableDisableStress(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	// Expect subtitles to be available if this is not being run with "subtitlesUnavailable" file.
	iExpectAvailable = (aKeyName != _L("subtitleUnavailable"));
	}

void RTestVclnt2SubtitleEnableDisableStress::DoBeforePlaybackActionsL()
	{
	TBool subtitlesAvail = iVideoPlayer2->SubtitlesAvailable();
		
	if (iExpectAvailable != subtitlesAvail)
		{
		ERR_PRINTF1(_L("SubtitlesAvailable() gave unexpected result"));
		User::Leave(KErrGeneral);
		}
		
	if (subtitlesAvail)
		{		
		INFO_PRINTF1(_L("Enabling subtitles"));
		iVideoPlayer2->EnableSubtitlesL();
		}
	else
		{
		INFO_PRINTF1(_L("Subtitles not available"));
		}
	
	iDisableNext = ETrue;
	}

void RTestVclnt2SubtitleEnableDisableStress::DoDuringPlaybackActionsL()
	{
	TBool subtitlesAvail = iVideoPlayer2->SubtitlesAvailable();
	
	if (iExpectAvailable != subtitlesAvail)
		{
		ERR_PRINTF1(_L("SubtitlesAvailable() gave unexpected result"));
		User::Leave(KErrGeneral);
		}
	
	if (iDisableNext)
		{
		INFO_PRINTF1(_L("Disabling subtitles"));
		iVideoPlayer2->DisableSubtitles();
		}
	else if (subtitlesAvail)
		{
		INFO_PRINTF1(_L("Enabling subtitles"));
		iVideoPlayer2->EnableSubtitlesL();
		}
	else
		{
		INFO_PRINTF1(_L("Subtitles not available... cannot enable"));
		}
	
	iDisableNext = !iDisableNext;
	iPlayTimer->After(KOneSecond / 4);
	}

//
// RTestVclnt2SubtitleAlloc
//

RTestVclnt2SubtitleAlloc* RTestVclnt2SubtitleAlloc::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
	{
	return new (ELeave) RTestVclnt2SubtitleAlloc(aTestName, aSectName, aKeyName, aExpectedError);
	}

RTestVclnt2SubtitleAlloc::RTestVclnt2SubtitleAlloc(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError)
: RTestVclnt2Subtitle(aTestName, aSectName, aKeyName, aExpectedError)
	{
	}

TVerdict RTestVclnt2SubtitleAlloc::DoTestStepPreambleL()
	{
	// Don't do the normal initialization here.  It will be done before each alloc test iteration.
	return EPass;
	}

TVerdict RTestVclnt2SubtitleAlloc::DoTestStepPostambleL()
	{
	// Don't do the normal cleanup here.  It will be done after each alloc test iteration.
	return EPass;
	}

TVerdict  RTestVclnt2SubtitleAlloc::PerformTestL()
	{	
	TVerdict result = EFail;
	TRAPD(err, result = RTestVclnt2Subtitle::DoTestStepPreambleL());
	if (iError != KErrNone)
		{
		err = iError;
		}
	
	if (err == KErrNone && result == EPass && iError == KErrNone)
		{
		TRAP(err, result = RTestVclnt2AviPlayerStep::DoTestStepL());
		
		if (iError != KErrNone)
			{
			err = iError;
			}
		}
	
	// Call postable function, but don't overwrite verdict from previous test call results.
	TVerdict postResult = RTestVclnt2Subtitle::DoTestStepPostambleL();

	if (iError != KErrNone)
		{
		err = iError;
		}

	User::LeaveIfError(err);
	if (EPass != result)
		{
		return result;
		}
	if (EPass != postResult)
		{
		return postResult;
		}
	
	return EPass;
	}

TVerdict RTestVclnt2SubtitleAlloc::DoTestStepL()
	{
	TVerdict allocTestResult = EFail;
	TVerdict result = EFail;
	
	TRAPD(err, result = PerformTestL());
	
	if (err != KErrNone || result != EPass)
		{
		ERR_PRINTF1(_L("Failed to run test iteration without setting heap allocation failure flag"));
		return allocTestResult;
		}
	
	TBool completed = EFalse;
	
	for (TInt rate = 1; !completed; rate++)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, rate);
        __MM_HEAP_MARK;
        
        result = EFail;
        TRAPD(err, result = PerformTestL());
        
        INFO_PRINTF3(_L("Iteration %d. Alloc failed %d times"), rate, __UHEAP_CHECKFAILURE);
        
        if (__UHEAP_CHECKFAILURE == 0)
        	{
        	// Heap did not fail.  Test should have passed with no errors.
        	if (KErrNone != result)
        		{
        		ERR_PRINTF2(_L("Failed on iteration %d. Test failed when no heap allocation failed"), rate);
        		allocTestResult = EFail;
        		}
        	else
        		{
        		INFO_PRINTF2(_L("Passed on iteration %d."), rate);
        		allocTestResult = EPass;
        		}
        	
        	completed = ETrue; // Finished iterating the test
        	}
        else if ((err == KErrNone && result == EPass) || (err == KErrNoMemory))
        	{
        	// Continue with next iteration if (a) alloc fail was handled and no error and a pass or 
        	// (b) we got an out of memory error.
        	INFO_PRINTF3(_L("Iteration result is %d, error is %d.  Starting next iteration"), result, err);
        	}
        else
        	{
        	ERR_PRINTF4(_L("Failed on iteration %d. Test failed with error %d.  Expected %d."), rate, err, KErrNoMemory);
        	completed = ETrue;
        	allocTestResult = EFail;
        	}
        
        __MM_HEAP_MARKEND;
		}
	
    __UHEAP_SETFAIL(RHeap::ENone, 0);
    
    return allocTestResult;
	}
