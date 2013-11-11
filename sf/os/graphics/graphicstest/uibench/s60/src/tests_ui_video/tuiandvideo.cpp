/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

/**
This test depends on the following environment:

==========================================================================
== Required step - Install Xvid codec
==========================================================================
Note that these drivers are distributed under the GNU GENERAL PUBLIC LICENSE.

** Go to:
http://developer.symbian.com/wiki/display/pub/Open+source+projects
and get the XviD source zip:
http://developer.symbian.com/wiki/download/attachments/1154/xvid.zip

** Unzip this file and build the source in 
xvidhwdevice\group
xvidpu\group


==========================================================================
== Required step - Running on emulator - Modify epoc.ini and wsini.ini
==========================================================================

** Update your epoc.ini so that GCE is enabled. Add:
...
SYMBIAN_GRAPHICS_USE_GCE ON
SYMBIAN_BASE_USE_GCE ON
...

** Update your wsini.ini so that the following is present:
...
WINDOWMODE COLOR16MAP
KEYCLICKPLUGIN KeyClickRef
TRANSPARENCY
CHROMAKEYCOLOR 0xFFFF00FF
...


==========================================================================
== Required step - Running on hardware - Buildrom command
==========================================================================

** When building the rom, make sure you use the following flags
 buildrom 
 	<options> 
 	-DSYMBIAN_BASE_USE_GCE 
 	-DSYMBIAN_GRAPHICS_USE_GCE 
 	-DUSE_24BPP_DISPLAY_VARIANT
 	<further options/flags>
 	videoplay.iby
 	<further oby/iby files>
 
	 the xvid video drivers being installed in the runtime environment.
*/

#include "tuiandvideo.h"

#include <mmf/common/mmferrors.h>

#ifdef __WINS__
_LIT(KDefaultDirName, "c:\\videoplay\\");
#else
_LIT(KDefaultDirName, "e:\\videoplay\\");
#endif

_LIT(KDefaultFileExt, "*.avi");
_LIT(KDefaultFile, "skiing2.avi");

const TTimeIntervalMicroSeconds KTapTimeOut(300*1000);
const TTimeIntervalMicroSeconds32 KIdleTimeOut(1*1000*1000);
const TTimeIntervalMicroSeconds32 KPlayTimeOut(2*1000*1000);
const TTimeIntervalMicroSeconds32 KButtonTimeOut(2*1000*1000);

_LIT(KVideoAppBitmapsFile, "z:\\resource\\apps\\videoplay2.mbm");

//From videoplay.hrh
enum TTestCommands
    {
    ETestCmdOpen = 1000,
    ETestCmdStop,
    ETestCmdPlayFile
    };

//UIBench test step commands
enum TTestStepCommand
	{
    ETestCmdInitialised = 0,
    ETestCmdPlay,
    ETestCmdPause,
    ETestCmdBounce,
    ETestCmdJump,
    ETestCmdStill,
    ETestCmdExit
    };

// From videoplay.mbg
enum TMbmVideoplay
	{
	EMbmVideoplayBackground,
	EMbmVideoplayBackground_mask,
	};


/**
A command handler to act as an interface through which to drive the test.
@param aController	View controller, in the classic sense of MVC. This object
					is used to drive the test.
*/
CCommandSink::CCommandSink (CTestAppView* aTestView) :
	CActive(EPriorityStandard),
	iTestView (aTestView)
	{
	CActiveScheduler::Add(this);
	iStatus = KRequestPending;
 	SetActive();
	}

void CCommandSink::DoCancel()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

CCommandSink::~CCommandSink()
	{
	Cancel();
	}

void CCommandSink::Init(RThread* aTargetThread, TRequestStatus* aTargetStatus)
	{
	iTargetThread = aTargetThread;
	iTargetStatus = aTargetStatus;
	}

/**
Signals the control thread (target) with a command. There must be a protocol between the
controller and the control (the test thread) in terms of communication of command 
signals. 
*/
void CCommandSink::RunL ()
	{
	switch (iStatus.Int())
		{
		case ETestCmdPlay:
			iTestView->PlayVideo();
			break;
		case ETestCmdPause:
			iTestView->PauseVideo();
			break;
		case ETestCmdBounce:
		case ETestCmdJump:
		case ETestCmdStill:
			iTestView->SetRunMode(iStatus.Int());
			break;
		}
	
	iStatus = KRequestPending;
 	SetActive();
	}

CTUiAndVideo::CTUiAndVideo()
	{
	SetTestStepName(KTUiAndVideo);
	}

void CTUiAndVideo::IssueCommand (TUint aCommand)
	{
	TRequestStatus* targetStatus = iAppUi->CommandSink();
	ConeThread()->RequestComplete(targetStatus, aCommand);
	}

void CTUiAndVideo::StepState (TUint aNextState, TUint aPause)
	{
	User::After(aPause);
	TRequestStatus* testState = &iTestState;
	User::RequestComplete (testState, aNextState);
	}

/**
   @SYMTestCaseID GRAPHICS-UI-BENCH-XXXX
   @SYMTestCaseDesc Put description here
   @SYMTestActions Put description here
   @SYMTestExpectedResults Put description here
*/
TVerdict CTUiAndVideo::doTestStepL()
	{
	iProfiler->InitResults();

	StepState(ETestCmdInitialised, 0);
   	
	TBool continueTest = ETrue;
	while (continueTest)
		{
		User::WaitForRequest(iTestState);
		switch (iTestState.Int())
			{
			case ETestCmdInitialised:
				User::After(5000000);
			case ETestCmdPlay:
				IssueCommand(ETestCmdPlay);
				StepState(ETestCmdBounce, 5000000);
				break;
			case ETestCmdPause:
				break;
			case ETestCmdBounce:
				IssueCommand(ETestCmdBounce);
				StepState(ETestCmdJump, 15000000);
				break;
			case ETestCmdJump:
				IssueCommand(ETestCmdJump);
				StepState(ETestCmdExit, 15000000);
			case ETestCmdExit:
				continueTest = EFalse;
				break;
			default:
				continueTest = EFalse;
				RDebug::Printf("Invalid test state!");
			}
		}

	return TestStepResult();
	}

/**
Runs on test thread.
*/
void CTUiAndVideo::InitUIL(CCoeEnv* aCoeEnv)
	{
	iAppUi = new CTUiAndVideoAppUi();
	
   	iAppUi->ConstructL(iTestState);
   	aCoeEnv->SetAppUi(iAppUi);
  	}

//############################################################################################################
//############################################################################################################
//############################################################################################################
//############################################################################################################

//************************************************************************************************************
// Application UI implementation
// -- most control logic is here
//************************************************************************************************************

void SetCenter(TRect& aRect, const TPoint& aCenterPoint)
	{
	const TSize size = aRect.Size();
	const TPoint pos(aCenterPoint-TSize(size.iWidth/2,size.iHeight/2).AsPoint());
	aRect.SetRect(pos, size);
	}

void CTUiAndVideoAppUi::ConstructL(TRequestStatus /*iCommandSource*/)
	{
	BaseConstructL(ENoAppResourceFile);
	
	// Calculate the size of the application window
	// Make the player square, slightly (one 16th) smaller than 
	// the smallest side of the screen
	TRect rect (0, 0, 300, 150);
	
	TRect screenRect = iCoeEnv->ScreenDevice()->SizeInPixels();
	SetCenter(rect, screenRect.Center());	// Center the rect on the screen

	// Create the application view
	iAppView = CTestAppView::NewL(rect);
	AddToStackL(iAppView);
	
   	iCommandSink = new (ELeave) CCommandSink(iAppView);
   	
	// Enable iIdleTimer to make the app return to foreground after a set delay
	iIdleTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	// iUIMover moves the UI around the display.
	iUIMover   = CPeriodic::NewL(CActive::EPriorityStandard);
	iUIMover->Start(KIdleTimeOut, 50000, TCallBack(UIMoverCallBack, this));
	// Enable iPlayTimer to make the video pause a little while before starting playing again
	iPlayTimer = CPeriodic::NewL(CActive::EPriorityStandard);	

	iAppView->SetPos(&rect.iTl);
	iAppView->ActivateL();
	iAppView->DrawNow();
	
	// Load default video clip	
	if (KErrNone == PopulateDirFilesL(KDefaultDirName))
		{
		OpenSelectedFileL(KDefaultFile);
		}
	}

TRequestStatus* CTUiAndVideoAppUi::CommandSink ()
	{
	return &(iCommandSink->iStatus);
	}

CTUiAndVideoAppUi::~CTUiAndVideoAppUi()
	{
	if(iAppView)
		{
		RemoveFromStack(iAppView);
		delete iAppView;
		}
	delete iCommandSink;
	delete iIdleTimer;
	delete iUIMover;
	delete iPlayTimer;
	delete iLoadingPlayer;
	delete iDirList;
	}

TInt CTUiAndVideoAppUi::UIMoverCallBack(TAny* aSelf)
	{
	CTUiAndVideoAppUi* self = static_cast<CTUiAndVideoAppUi*>(aSelf);
	self->SetPos();
	return KErrNone;
	}

void CTUiAndVideoAppUi::SetPos()
	{
	iAppView->SetPos(NULL);
	}

TInt CTUiAndVideoAppUi::PopulateDirFilesL(const TDesC& aDir)
	{
	// Load files in default folder
	RFs fs;
	User::LeaveIfError(fs.Connect());
	
	// Get the file list, sorted by name - Only with KDefaultFileExt
	TFileName dir(aDir);
	dir.Append(KDefaultFileExt);
	
	delete iDirList;
	iDirList = NULL;
	TInt err = fs.GetDir(dir, KEntryAttMaskSupported, ESortByName, iDirList);
	fs.Close();
	
	if ((err == KErrPathNotFound) || (err == KErrNone && iDirList->Count()==0))
		{
		LogError(KNullDesC, _L("Default clip not found!"));
		return KErrNotFound;
		}
	__ASSERT_DEBUG(err==KErrNone, User::Panic(_L("PopulateDirFiles"), err));

	// set the current file with the current folder (for the time being)
	iCurrentFile.Zero();
	iCurrentFile.Append(aDir);
	return KErrNone;
	}

void CTUiAndVideoAppUi::OpenSelectedFileL(const TDesC& aFile)
	{
	__ASSERT_DEBUG(iDirList, User::Panic(_L("iDirList"), 0));
	TBool defaultFound = EFalse;
	for (iCurrentIndex=0; iCurrentIndex < iDirList->Count(); iCurrentIndex++)
		{
 		TFileName fileName = (*iDirList)[iCurrentIndex].iName;
 		if (fileName.Compare(aFile) ==0)
 			{
 			defaultFound = ETrue;
			break;
			}
		}
	if (!defaultFound)
		{
		// To avoid linking with eikcore.lib
		//iEikonEnv->InfoMsg(_L("Default file not found..."));
		iCurrentIndex = 0; // if no default, pick the first movie on the list
		}

	iCurrentFile.Append((*iDirList)[iCurrentIndex].iName);
	// play the selected file.
	HandleCommandL(ETestCmdOpen);
	}

TInt CTUiAndVideoAppUi::PlayNextFileL()
	{
	__ASSERT_DEBUG(iDirList, User::Panic(_L("iDirList"), 0));
	if (iCurrentIndex == (iDirList->Count()-1))
		{
		return KErrNotFound;
		}
	iCurrentIndex++;
	TParse file;
	file.Set(iCurrentFile, NULL, NULL);
	iCurrentFile = file.DriveAndPath();
	iCurrentFile.Append((*iDirList)[iCurrentIndex].iName);
	HandleCommandL(ETestCmdOpen);
	return KErrNone;
	}

TInt CTUiAndVideoAppUi::PlayPreviousFileL()
	{
	__ASSERT_DEBUG(iDirList, User::Panic(_L("iDirList"), 0));
	if (iCurrentIndex == 0)
		{
		return KErrNotFound;
		}
	iCurrentIndex--;
	TParse file;
	file.Set(iCurrentFile, NULL, NULL);
	iCurrentFile = file.DriveAndPath();
	iCurrentFile.Append((*iDirList)[iCurrentIndex].iName);
	HandleCommandL(ETestCmdOpen);
	return KErrNone;
	}

void CTUiAndVideoAppUi::LogError(const TDesC& /* aFileName */, const TDesC& /* aErrorMsg */)
	{
	// TODO: Fix this logging strategy.  Printf uses C-like pointers, not Symbian
	// descriptors
	//RDebug::Printf("Error: %s %s", aFileName, aErrorMsg);
	if(iAppView)
		iAppView->DisableVideo();
	}

void CTUiAndVideoAppUi::HandleForegroundEventL(TBool aForeground)
	{
	if(!aForeground && iIdleTimer)
		{
		if(!iIdleTimer->IsActive())
			iIdleTimer->Start(KIdleTimeOut, 0, TCallBack(BackgroundCallBack, this) );
		}
	}

TInt CTUiAndVideoAppUi::BackgroundCallBack(TAny* aSelf)
	{
	CTUiAndVideoAppUi* self = static_cast<CTUiAndVideoAppUi*>(aSelf);
	self->BringToForeground();
	return KErrNone;
	}

void CTUiAndVideoAppUi::BringToForeground()
	{
	iIdleTimer->Cancel();
	const TInt err = iCoeEnv->WsSession().SetWindowGroupOrdinalPosition(iCoeEnv->RootWin().Identifier(), 0);
	}

TInt CTUiAndVideoAppUi::PlayTimerCallBack(TAny* aSelf)
	{
	CTUiAndVideoAppUi* self = static_cast<CTUiAndVideoAppUi*>(aSelf);
	self->iPlayTimer->Cancel();
	self->iAppView->PlayVideo();
	self->iInitialised = ETrue;
	
	return KErrNone;
	}

void CTUiAndVideoAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case ETestCmdOpen: // Open and prepare
			{
			iAppView->DisableVideo();
			if(iLoadingPlayer)
				{
				iLoadingPlayer->RemoveDisplayWindow(const_cast<RWindow&>(iAppView->TheWindow()));
				iLoadingPlayer->Close();
				delete iLoadingPlayer;
				iLoadingPlayer = NULL;
				}	
			iLoadingPlayer = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceNone);
			iLoadingPlayer->OpenFileL(iCurrentFile);
			break;
			}
		case ETestCmdPlay: // Start playback
			iAppView->PlayVideo();
			break;
		case ETestCmdStop: // Stop playback
			iAppView->StopVideo();
			break;
		case ETestCmdPlayFile:
			break;
		case EEikCmdExit: // Exit
			if(iLoadingPlayer)
				iLoadingPlayer->RemoveDisplayWindow(const_cast<RWindow&>(iAppView->TheWindow()));
//			Exit();
			break;
		}
	}

// Video player utility callback: open complete
void CTUiAndVideoAppUi::MvpuoOpenComplete(TInt aError)
	{
	if (aError == KErrNone )
		{
		iLoadingPlayer->Prepare();
		}
	else if(aError == KErrNotFound)
		LogError(iCurrentFile, _L("Clip not found!"));
	else if(aError == KErrNotReady)
		LogError(iCurrentFile, _L("Clip not ready!"));
	else if(aError == KErrNotSupported)
		LogError(iCurrentFile, _L("Clip format not supported!"));
	else
		{
		User::Panic(_L("MvpuoOpen"), aError);
		}
	}

// Video player utility callback: prepare complete
void CTUiAndVideoAppUi::MvpuoPrepareComplete(TInt aError)
	{
	switch ( aError )
		{
		case KErrNone:
			{
			TBuf<256> loadingMsg(_L("Loading "));
			TParse file;
			file.Set(iCurrentFile, NULL, NULL);
			loadingMsg.Append(file.Name());
			
			TRAPD(err, iLoadingPlayer->AddDisplayWindowL(iEikonEnv->WsSession(), *iEikonEnv->ScreenDevice(), iAppView->TheWindow()));
			if(err == KErrInUse)
				LogError(iCurrentFile, _L("Window Display in use!"));
			else if(err == KErrNotReady)
				LogError(iCurrentFile, _L("Window Display not ready!"));
			else if(err)
				User::Panic(_L("PrepareComplete1"), err);

			iAppView->EnableVideo(iLoadingPlayer);
			}
			break;

		case KErrMMPartialPlayback:
			LogError(iCurrentFile, _L("Partial playback"));
			break;

		default:
			User::Panic(_L("Prepare"), aError);
		}
	}

// Video player utility callback, not used
void CTUiAndVideoAppUi::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt aError)
	{
	User::Panic(_L("FrameReady"), aError);
	}

// Video player utility callback: play complete (error or EOF)
void CTUiAndVideoAppUi::MvpuoPlayComplete(TInt aError)
	{
	iAppView->PlayComplete(aError);
	
	if (aError == KErrNone)
		{
		if(iPlayTimer && !iPlayTimer->IsActive())
			iPlayTimer->Start(KPlayTimeOut, 0, TCallBack(PlayTimerCallBack, this));
		}
	else if(aError == KErrNotSupported) 
		LogError(iCurrentFile, _L("Clip format not supported!"));
	else if(aError == KErrTooBig)
		LogError(iCurrentFile, _L("The clip is too big!"));
	else if(aError == KErrNoMemory)
		LogError(KNullDesC, _L("Out of memory. Likely memory leak!"));
	else if(aError == KErrNotReady)
		LogError(KNullDesC, _L("Clip not ready. Removed MMC?!"));
	else if (aError)
		User::Panic(_L("PlayComplete"), aError);
	}

//  Video player utility callback: event (not used?)
void CTUiAndVideoAppUi::MvpuoEvent(const TMMFEvent& aEvent)
	{
	User::Panic(_L("Event"), aEvent.iErrorCode);
	}


//************************************************************************************************************
// View implementation
// -- draws the display
//************************************************************************************************************

CTestAppView* CTestAppView::NewL(const TRect& aRect)
	{
	CTestAppView* self = new(ELeave) CTestAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}

CTestAppView::~CTestAppView()
	{
	if(iCurrentPlayer)
		{
		iCurrentPlayer->RemoveDisplayWindow(TheWindow());
		iCurrentPlayer->Close();
		delete iCurrentPlayer;
		}
	delete iBkgrdImage;
	delete iBkgrdMask;
	}

CTestAppView::CTestAppView() :
	iIncrement (2, 2),
	iDirection (1, 1)
	{
	}

RWindow& CTestAppView::TheWindow()
	{
	__ASSERT_ALWAYS(iVideoPane, User::Panic(_L("TheWindow"), 0));	
	return iVideoPane->TheWindow();
	}

void CTestAppView::ConstructL(const TRect& aWindowedRect)
	{
	iWindowedViewRect = aWindowedRect;	// The rect used when the view is not full-screen
	CreateWindowL();
	EnableWindowTransparency();
	EnableDragEvents();
	Window().SetPointerGrab(ETrue);

	InitComponentArrayL();

	iVideoPane = new (ELeave) CVideoPane();
	Components().AppendLC(iVideoPane, KVideoControlID);
	iVideoPane->ConstructL(*this);
	CleanupStack::Pop(iVideoPane);

	SetRect(iWindowedViewRect);

	iBkgrdImage = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iBkgrdImage->Load(KVideoAppBitmapsFile, EMbmVideoplayBackground));

	iBkgrdMask = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iBkgrdMask->Load(KVideoAppBitmapsFile, EMbmVideoplayBackground_mask));
	
	ActivateL();
	}

void CTestAppView::SizeChanged() 
	{
	const TRect reservedVideoRect = VideoPaneRect();
	const TRect usedVideoRect = VideoPaneRect();

	if(iVideoPane)
		{
		iVideoPane->BlackoutPane();
		iVideoPane->SetRect(reservedVideoRect);
		if(iCurrentPlayer)
			SetVideoSize(*iCurrentPlayer);
		}
	}

/**
Sets the size and stretch characteristics of the video.
*/
void CTestAppView::SetVideoSize(CVideoPlayerUtility2& aVideo) const
	{
	const TRect videoRect(iVideoPane->Rect());

	CTUiAndVideoAppUi* myAppUi = dynamic_cast<CTUiAndVideoAppUi *>(iCoeEnv->AppUi());
	
	TRAPD(err, aVideo.SetWindowClipRectL(TheWindow(), videoRect));
	if(err == KErrArgument)
		myAppUi->LogError(KNullDesC, _L("Clip rect not within window!"));
	else if(err == KErrNotReady)
		myAppUi->LogError(KNullDesC, _L("Video not ready!"));
	else if(err)
		User::Panic(_L("SetVideoSize2"), err);
#if 1	
	TRAP(err, aVideo.SetAutoScaleL(TheWindow(), EAutoScaleStretch));
	if(err == KErrNotFound)
		myAppUi->LogError(KNullDesC, _L("Window not added to iLoadingPlayer!"));
	else if(err == KErrNotReady)
		myAppUi->LogError(KNullDesC, _L("Controller not been opened!"));
	else if(err)
		User::Panic(_L("SetVideoSize2"), err);
#endif
	iVideoPane->SetVideoUseRect(VideoPaneRect());
	}

/**
@return The rect of the video pane, relative to the view.
*/ 
TRect CTestAppView::VideoPaneRect() const
	{
	TRect result = Rect();
	
	// Make the video screen rect 10% smaller than the whole app window
	result.Shrink(result.Width()/10, result.Height()/10);
	return result;
	}

void CTestAppView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	// call base class to dispatch any possible child events first
	// except we don't want to enter into a loop if this method has alraedy been
	// called from the video child control....
	if (!iVideoPane->IsVideoPanePointerEvent())
		CCoeControl::HandlePointerEventL(aPointerEvent);
	
	if(aPointerEvent.iType == TPointerEvent::EButton1Down)
		{
		// get the position (we may need to get the parent's if event came from video child control)
		iPointerDownPos = iVideoPane->IsVideoPanePointerEvent() ? aPointerEvent.iParentPosition : aPointerEvent.iPosition;
		TTime now;
		now.UniversalTime();
#if 0
		if(now.MicroSecondsFrom(iPointerDownTime) < KTapTimeOut)	// If quick double-tap
			{
			IgnoreEventsUntilNextPointerUp();
			ToggleViewSize();
			}
#endif
		iPointerDownTime.UniversalTime();
		}
	else if(aPointerEvent.iType == TPointerEvent::EDrag)
		{
		// Set new position relative to the amount of movement with pointer down event
		TPoint newPos = (iVideoPane->IsVideoPanePointerEvent() ? aPointerEvent.iParentPosition : aPointerEvent.iPosition) - iPointerDownPos;
		if(newPos != TPoint())
			SetPosition(Position()+newPos);
		}
	}

TKeyResponse CTestAppView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	if(aType == EEventKey)
		{
		switch(aKeyEvent.iCode)
			{
			case EKeyEscape:
			// To avoid linking with eikcore.lib
			//if(iEikonEnv->QueryWinL(_L("Exit?"), _L("Sure you want to exit?")))
				{
				iEikonEnv->EikAppUi()->HandleCommandL(EEikCmdExit); 
				return EKeyWasConsumed;
				}
			case EKeyLeftArrow:
				PlayPrevious();
				return EKeyWasConsumed;
			case EKeyRightArrow:
				PlayNext();
				return EKeyWasConsumed;
			case EKeySpace:
				if (iIsPlaying)
					PauseVideo();
				else
					PlayVideo();
				
				return EKeyWasConsumed;
			case EKeyEnter:
				iRunMode = ETestCmdStill; 
				return EKeyWasConsumed;
			case EKeyTab:
				return EKeyWasConsumed;
			default:
				break;	
			}
		}
		
	return EKeyWasNotConsumed;	
	}

void CTestAppView::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc = SystemGc();

	// Fill the window solid
	gc.SetDrawMode(CGraphicsContext::EDrawModePEN);
	gc.SetBrushStyle(CGraphicsContext::ENullBrush);
	gc.DrawBitmapMasked(Rect(), iBkgrdImage, iBkgrdImage->SizeInPixels(), iBkgrdMask, EFalse);
	}

void CTestAppView::HandleControlEventL(CCoeControl */*aControl*/, TCoeEvent /*aEventType*/)
	{
	return;
	}

// Enable video
void CTestAppView::EnableVideo(CVideoPlayerUtility2*& aPlayer)
	{
	if(iCurrentPlayer)
		{
		iCurrentPlayer->RemoveDisplayWindow(TheWindow());
		iCurrentPlayer->Close();
		delete iCurrentPlayer;
		}	
	
	iCurrentPlayer = aPlayer;
	aPlayer = NULL;	// Take ownership
	const TSize oldSize = iNativeVideoSize;
	iCurrentPlayer->VideoFrameSizeL(iNativeVideoSize);
	SetVideoSize(*iCurrentPlayer);
	if (oldSize!=iNativeVideoSize)
		SizeChanged();

#if 0 
	// Was getting a crash at startup on the emulator...not sure why.
	// Not currently a requirement.
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	if (iCurrentPlayer->SubtitlesAvailable())
		{
		iCurrentPlayer->EnableSubtitlesL();
		iVideoPane->EnableSubtitle(iCurrentPlayer);
		}
#endif		
#endif
	PlayVideo();
	}

// Disable video
void CTestAppView::DisableVideo()
	{
	iVideoPane->BlackoutPane();
	PauseVideo();	// Stop playing
	
	if(iCurrentPlayer)
		{
		iCurrentPlayer->RemoveDisplayWindow(TheWindow());
		iCurrentPlayer->Close();
		delete iCurrentPlayer;
		}	
	iVideoPane->EnableSubtitle(NULL);
	iCurrentPlayer = NULL;
 	iNativeVideoSize = TSize();
	}

void CTestAppView::SetRunMode (TUint32 aMode)
	{
	iRunMode = aMode;
	}

void CTestAppView::PlayVideo()
	{
	if(iCurrentPlayer && !iIsPlaying)
		{
		iCurrentPlayer->Play();
		iIsPlaying = ETrue;
		}
	else
		iIsPlaying	= EFalse;	// Can't play if there's no player
		
	UpdateButtonState();
	DrawNow();
	}
	
void CTestAppView::PauseVideo()
	{
	if(iCurrentPlayer && iIsPlaying)
		{
		TRAPD(err, iCurrentPlayer->PauseL());
		if(err)
			iCurrentPlayer->Stop();
		}
	
	iIsPlaying = EFalse;	
	UpdateButtonState();
	DrawNow();
	}
	
void CTestAppView::StopVideo()
	{
	if(iCurrentPlayer && iIsPlaying)
		{
		iCurrentPlayer->Stop();
		iIsPlaying = EFalse;
		}
	else
		iIsPlaying	= ETrue;	// Can't play if there's no player
		
	UpdateButtonState();
	DrawNow();
	}

void CTestAppView::PlayComplete(TInt /*aError*/)
	{
	iIsPlaying	= EFalse;
	UpdateButtonState();
	DrawNow();
	}

void CTestAppView::PlayPrevious()
	{
	CTUiAndVideoAppUi* myAppUi = dynamic_cast<CTUiAndVideoAppUi *>(iCoeEnv->AppUi());
	if (myAppUi->PlayPreviousFileL() != KErrNone)
		{
		// To avoid linking with eikcore.lib
		//EikonEnv->InfoMsg(_L("First clip"));
		}
	}
	
void CTestAppView::PlayNext()
	{
	CTUiAndVideoAppUi* myAppUi = dynamic_cast<CTUiAndVideoAppUi*>(iCoeEnv->AppUi());
	if (myAppUi->PlayNextFileL() != KErrNone)
		{
		// To avoid linking with eikcore.lib
		//iEikonEnv->InfoMsg(_L("Last clip"));
		}
	}

void CTestAppView::UpdateButtonState()
	{
	}

/**
Calculate and set the position of the video UI. If iAutoPosition is zero then this
method is a no-op. 
*/
void CTestAppView::SetPos(TPoint* aPos)
	{
	if (!aPos)
		{
		TRect screenRect = iCoeEnv->ScreenDevice()->SizeInPixels();
		
		TSize currentSize = Size();
		TPoint currentPos = Position();

		switch (iRunMode)
			{
			case (ETestCmdBounce):
				{
				if ((currentPos.iX + (currentSize.iWidth >> 1)) > screenRect.iBr.iX)
					iDirection.iX = -1;
			
				if ((currentPos.iY + (currentSize.iHeight >> 1)) > screenRect.iBr.iY)
					iDirection.iY = -1;
			
				if ((currentPos.iX + (currentSize.iWidth >> 1)) < 0)
					iDirection.iX = 1;
			
				if ((currentPos.iY + (currentSize.iHeight >> 1)) < 0)
					iDirection.iY = 1;
				
				iVidPos.iX += iIncrement.iX* iDirection.iX;
				iVidPos.iY += iIncrement.iY * iDirection.iY;
				break;
				}
			case (ETestCmdJump):
				{
				iVidPos.iX = Math::Random() % 100;//(screenRect.iBr.iX + (currentSize.iWidth >> 1));
				iVidPos.iY = Math::Random() % 100;//(screenRect.iBr.iY + (currentSize.iHeight >> 1));
				break;
				}
			default:
				{} // Do nothing.			
		
			}
		}
	else
		{
		iVidPos = *aPos;
		}
	
	SetPosition(iVidPos);
	}

//************************************************************************************************************
//** CVideoPane class - place 'video' window.
//************************************************************************************************************

/**
This controll is used so that the video is not rendered onto transparency.  
*/
CVideoPane::~CVideoPane()
	{
	}

CVideoPane::CVideoPane()
	{
	}

void CVideoPane::ConstructL(CCoeControl& aParent)
	{
	CreateWindowL(&aParent);
	EnableDragEvents();
	Window().SetPointerGrab(ETrue);
	}

void CVideoPane::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	// we need to set this flag to true, which is checked in the parent to avoid loops
	iIsVideoPanePointerEvent = ETrue;
	TRAPD(err, Parent()->HandlePointerEventL(aPointerEvent));
	iIsVideoPanePointerEvent = EFalse;
	User::LeaveIfError(err);
	}


void CVideoPane::BlackoutPane()
	{
	iVideoUseRect = TRect();	// reset
	}

void CVideoPane::SetVideoUseRect(const TRect& aRect)
	{
	iVideoUseRect = aRect;
	iVideoUseRect.Move(-Position());	// Compensate for the fact that the pane is window owning
	}
	
void CVideoPane::SizeChanged() 
	{
	CCoeControl::SizeChanged();
	iVideoUseRect = TRect();	// reset
	}

TBool CVideoPane::IsVideoPanePointerEvent()
	{
	return iIsVideoPanePointerEvent;
	}

void CVideoPane::EnableSubtitle(CVideoPlayerUtility2* aSubtitlePlayer)
	{
	iSubtitlePlayer = aSubtitlePlayer;
	}

void CVideoPane::Draw(const TRect& aRect) const
	{
	(void)aRect;	//to remove warning if SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT not defined
	CWindowGc& gc=SystemGc();
#ifdef _DEBUG
	gc.SetBrushColor(KRgbRed);
#else
	gc.SetBrushColor(KRgbBlack);
#endif
	DrawUtils::ClearBetweenRects(gc, Rect(), iVideoUseRect);

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	if (iSubtitlePlayer)
		iSubtitlePlayer->RedrawSubtitle(Window(), aRect);
#endif
	}
