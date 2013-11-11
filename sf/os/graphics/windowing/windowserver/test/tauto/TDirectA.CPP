// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test Direct Screen Access
// 
//

#include "tdirecta.h"

static TRect WinExt;
static TInt WinCol=0;
static TTimeIntervalMicroSeconds32 MoveInterval;
static TTimeIntervalMicroSeconds32 ModeInterval;
static TTimeIntervalMicroSeconds32 FlipInterval;
static TBool ImmediateModeSwitch;
#if defined(LOGGING)
	LOCAL_D TLogMessageText LogMessageText;
#endif

_LIT(SemControl,"Control");
_LIT(SemNextOp,"TrigerWindow");
_LIT(FontName,"DejaVu Serif Condensed");
_LIT(QueueControl,"Queue");

LOCAL_D TSize FullScreenModeSize;
LOCAL_D TInt Copy2ndHalfOfScreen;

const TBool KRegionTrackingOnly = ETrue;
const TBool KDrawingDsa = EFalse;

const TInt KPanicTestOrdinalPriority=65536;
const TInt KMainTestOrdinalPriority=65535;
const TInt KMainTestBaseWindow=KMainTestOrdinalPriority/3;
const TInt KAboveMainTestBaseWindow = KMainTestBaseWindow +1;

const TInt KMaxIdlingTime = 25; //used for RegionTrackingOnly DSAs, it represents the maximum number of times the Idling function can be called
//Ids of two RegionTrackingOnly DSAs
const TInt KRegionTrackingOnlyDsaWaitingForAbortSignal = 25;
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
const TInt KRegionTrackingOnlyDsaNoAbortSignal = 26;
#endif

const TInt KRegionTrackingOnlyDsaExistLast = 0;
const TInt KDrawingDsaExistLast = 1;
const TInt KShortDelay = 5000;
#define SHORT_DELAY	TTimeIntervalMicroSeconds32(KShortDelay)

_LIT(KSem_DefectFix_KAA_5J3BLW_Name, "DefectFix_KAA_5J3BLW");

GLDEF_C TInt ProcDirect(TAny *aScreenNumber)
	{
	CTrapCleanup* cleanupStack = NULL;
	User::LeaveIfNull(cleanupStack = CTrapCleanup::New());

#if defined(LOGGING)
	_LIT(KWindow,"Window Toggle Vis=");
	_LIT(KParams,"%d, Pos=(%d,%d,%d,%d), Col=%d");
#endif
	RSemaphore controlSem;
	RSemaphore windowSem;
	User::LeaveIfError(controlSem.OpenGlobal(SemControl));
	User::LeaveIfError(windowSem.OpenGlobal(SemNextOp));
	RWsSession ws;
	User::LeaveIfError(ws.Connect());

	// assign to the correct screen
	CWsScreenDevice* screen = NULL;
	TInt err;
	TRAP(err, screen = new (ELeave) CWsScreenDevice(ws));
	if (err!=KErrNone)
		return err;

	if ((err=screen->Construct((TInt)aScreenNumber))!=KErrNone)
		{
		delete screen;
		return err;
		}

	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(898));
	group.EnableReceiptOfFocus(EFalse);
	group.SetOrdinalPosition(0,KAboveMainTestBaseWindow );
	RBlankWindow window(ws);
	User::LeaveIfError(window.Construct(group,899));
	TBool vis=EFalse;
	window.SetVisible(vis);
	window.Activate();
	ws.Flush();
	controlSem.Signal();
	windowSem.Wait();
	RMsgQueueBase queue;
	TInt open = queue.OpenGlobal(QueueControl);
	TInt data;
	while (queue.Receive(&data,sizeof(TInt)) != KErrNone)
		{
		vis=!vis;
		if (vis)
			{
			window.SetColor(TRgb::Gray4(WinCol));
			window.SetExtent(WinExt.iTl,WinExt.Size());
		#if defined(LOGGING)
			LogMessageText.Copy(KWindow);
			LogMessageText.AppendFormat(KParams,vis,WinExt.iTl.iX,WinExt.iTl.iY,WinExt.iBr.iX,WinExt.iBr.iY,WinCol);
			ws.LogMessage(LogMessageText);
		#endif
			}
		window.SetVisible(vis);
		ws.Flush();
		windowSem.Wait();
		}
	queue.Close();

	window.Close();
	group.Close();

	delete screen;
	ws.Close();
	controlSem.Close();
	windowSem.Close();

	delete cleanupStack;
	return(KErrNone);
	}


/*CDirectScreenAccessOld*/

CDirectScreenAccessOld* CDirectScreenAccessOld::NewL(RWsSession& aWs,MAbortDirectScreenAccess& aAborter)
	{
	CDirectScreenAccessOld* self=new(ELeave) CDirectScreenAccessOld(aWs,aAborter);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CDirectScreenAccessOld::~CDirectScreenAccessOld()
	{
	__ASSERT_ALWAYS(!iAborting,AutoPanic(EAutoPanicDirect));
	Cancel();
	iDirectAccess.Close();
	}

void CDirectScreenAccessOld::ConstructL()
	{
	User::LeaveIfError(iDirectAccess.Construct());
	CActiveScheduler::Add(this);
	}

TInt CDirectScreenAccessOld::Request(RRegion*& aRegion, RWindowBase& aWindow)
	{
	TInt ret=iDirectAccess.Request(aRegion,iStatus,aWindow);
	if (ret==KErrNone)
		SetActive();
	return ret;
	}

void CDirectScreenAccessOld::DoCancel()
	{
	iDirectAccess.Cancel();
	}

void CDirectScreenAccessOld::RunL()
	{
	iAborting=ETrue;
	iAborter.AbortNow(REINTERPRET_CAST(RDirectScreenAccess::TTerminationReasons&,iStatus));
	iAborting=EFalse;
	iDirectAccess.Completed();
	}


/*CColorAnimation*/

CColorAnimation* CColorAnimation::NewL(TInt aScreenNumber,TInt aId,MAnimCallBacks& aCallBack,CTWinBase& aParent,TRect aExtent,TBool aStart,TBool aRegionTrackingOnly)
	{
	CColorAnimation* self=new(ELeave) CColorAnimation(aScreenNumber,aId,aCallBack);
	CleanupStack::PushL(self);
	self->ConstructL(aParent,aExtent,aRegionTrackingOnly);
	if (aStart)
		self->StartL();
	CleanupStack::Pop(self);
	return self;
	}

CColorAnimation::~CColorAnimation()
	{
	delete iTimer;
	delete iDrawer;
	delete iWindow;
	delete iWindow2;
	TheClient->Flush();
	}

void CColorAnimation::ConstructL(CTWinBase& aParent,TRect aExtent,TBool aRegionTrackingOnly,TInt aTypeWindow/*=0*/,TInt aSingleWinForMultipleDSA/*=0*/)
	{	
	iRegionTrackingOnly = aRegionTrackingOnly;
	// Make sure that the top right corner is not 0,0
	if (aTypeWindow && aExtent.iTl==TPoint(0,0))
		{
		aExtent.iTl=TPoint(10,10);
		}
	RWindowBase* win=NULL;
	iSingleWinForMultipleDSA=aSingleWinForMultipleDSA;
	if (aSingleWinForMultipleDSA)
		{
		win=iCallBack.iCallBackWin->BaseWin();
		iWinSize=TheClient->iScreen->SizeInPixels();
		}
	else
		{
		iWindow=new(ELeave) CTBlankWindow();
		iWinSize=aExtent.Size();
		iWindow->ConstructExtLD(aParent,aExtent.iTl,iWinSize);
		ChangeModeL(iCallBack.DisplayMode(iId));
		win=iWindow->BaseWin();
		win->SetShadowDisabled(ETrue);
		win->Activate();
		}
	if (!aTypeWindow)
		{
		if(iRegionTrackingOnly)
			{
			iDrawer=CDirectScreenAccess::NewL(TheClient->iWs,*TheClient->iScreen,*win,*this,iRegionTrackingOnly);
			}
		else
			{
			//needed for the non NGA case: only the old API is allowed to be used
			iDrawer=CDirectScreenAccess::NewL(TheClient->iWs,*TheClient->iScreen,*win,*this);
			}
		}
	else
		{
		TRect childRect(0,0,100,100);
		switch(aTypeWindow)
			{
			case 1:
				{
				// Create a Blank Window smaller than it's parent and its top left corner within the parent area and withn the screen area
	 			childRect.Shrink(10,10);
				break;
				}
			case 2:
				{
				// Create a Blank Window with its top left corner being left side of its parent
				childRect.Move(-10,0);
				break;
				}
			}
		iWindow2=new(ELeave) CTBlankWindow();
		iWindow2->ConstructExtLD(*iWindow,childRect.iTl,childRect.Size());
		// Finish constructing the window
	 	RWindowBase& win=*iWindow2->BaseWin();
	 	win.SetShadowDisabled(ETrue);
	 	win.Activate();
		// Create the Direct Screen Access object
	 	if(iRegionTrackingOnly)
	 		{
	 		iDrawer=CDirectScreenAccess::NewL(TheClient->iWs,*TheClient->iScreen,win,*this,iRegionTrackingOnly);
	 		}
	 	else
	 		{
	 		//needed for the non NGA case: only the old API is allowed to be used
	 		iDrawer=CDirectScreenAccess::NewL(TheClient->iWs,*TheClient->iScreen,win,*this);
	 		}
		}
	TheClient->Flush();
	if (iId == 4)
	    {
        // This clause is needed to prevent sub-test 9 from taking an 
        // exceedingly large amount of time. The CColorAnimation with iId 4 is 
        // the one which controls when the test stops. This raise in 
        // priority compared to the other CColorAnimations, allows it's 
        // callbacks to get through when otherwise they would take much longer
        // due to the volume of callbacks generated by the other
        // CColorAnimations
        iTimer=CPeriodic::NewL(1);
	    }
	else
	    {
        iTimer=CPeriodic::NewL(0);
	    }
	}

TPoint CColorAnimation::AbsoluteWindowPosition(TInt aWindowId/*=0*/)
	{
	if (iSingleWinForMultipleDSA)
		{
		return iCallBack.iCallBackWin->BaseWin()->AbsPosition();
		}
	if (!aWindowId)
		{
		// Return the absolute position of iWindow, if Color Animation is not for Position Relative to Screen test.
		return iWindow->BaseWin()->AbsPosition();
		}
	else
		{
		// Return the absolute position of iWindow2, if it is for Position Relative to Screen test.
		return iWindow2->BaseWin()->AbsPosition();
		}
	}

TInt CColorAnimation::DrawColorL(TAny* aAnimation)
	{
	STATIC_CAST(CColorAnimation*,aAnimation)->DrawColorL();
	return(KErrNone);
	}

TInt CColorAnimation::IdlingL(TAny* aAnimation)
	{
	(static_cast<CColorAnimation*>(aAnimation))->IdlingL();
	return(KErrNone);
	}

void CColorAnimation::StartL(TBool aChildWindow/*=EFalse*/)
	{
	iDrawer->StartL();
	TRect bounding=iDrawer->DrawingRegion()->BoundingRect();
	TRect window;
	if (aChildWindow)
		{
		window.SetRect(AbsoluteWindowPosition(1),iWinSize);
		}
	else
		{
		window.SetRect(AbsoluteWindowPosition(),iWinSize);
		}
	// Check that the window contains the bounding area (a bounding rect of (0,0,0,0) shouldn't fail the test)
	if (!(window.Contains(bounding.iTl) && window.Contains(bounding.iBr-TPoint(1,1))) && bounding.Size()!=TSize(0,0))
		{
		iCallBack.Fail();
	//	iDrawer->Cancel();
	//	iDrawer->StartL();
	//	bounding=iDrawer->DrawingRegion()->BoundingRect();
		}
	if(!iRegionTrackingOnly)                                                                                                                                                          
		{
		iTimer->Start(0,iCallBack.TimerInterval(iId),TCallBack(CColorAnimation::DrawColorL,this));
		iDrawer->Gc()->SetPenStyle(CGraphicsContext::ENullPen);
		iDrawer->Gc()->SetBrushStyle(CGraphicsContext::ESolidBrush);
		}                                                                                                                                                                                
	else                                                                                                                                                                              
		{
		iTimer->Start(0,iCallBack.TimerInterval(iId),TCallBack(CColorAnimation::IdlingL,this));
		}
	}

void CColorAnimation::StartOrPanic()
	{
	TRAPD(err,StartL());
	if (err!=KErrNone)
		{
		iCallBack.LogLeave(err);
		iCallBack.Fail();
		}
	}

void CColorAnimation::Stop()
	{
	iTimer->Cancel();
	iDrawer->Cancel();
	}

void CColorAnimation::BringWindowToFront()
	{
	iWindow->WinTreeNode()->SetOrdinalPosition(0);
	}

void CColorAnimation::ChangeModeL(TDisplayMode aMode)
	{
	User::LeaveIfError(iWindow->BaseWin()->SetRequiredDisplayMode(aMode));
	TheClient->Flush();
	}

void CColorAnimation::FinishTest()
	{
	iCallBack.Finished(iId);
	}

inline CDirectScreenAccess* CColorAnimation::GetDrawer()
	{
	return iDrawer;
	}

void CColorAnimation::DrawColorL()
	{
	TBool aFinished;
	iDrawer->Gc()->SetBrushColor(iCallBack.BrushColorL(iId,iColor,aFinished));
	if (iSingleWinForMultipleDSA==1)
		{
		iDrawer->Gc()->DrawRect(iWinSize-TSize(iWinSize.iWidth/2,iWinSize.iHeight/2));
		}
	else if (iSingleWinForMultipleDSA==2)
		{
		iDrawer->Gc()->DrawRect(TRect(TPoint(iWinSize.iWidth/2,iWinSize.iHeight/2),TSize(iWinSize.iWidth/2,iWinSize.iHeight/2)));
		}
	else
		{
		iDrawer->Gc()->DrawRect(iWinSize);
		}
	iDrawer->ScreenDevice()->Update();
	if (aFinished)
		{
		if (iId==1)
			{
			iWindow->SetSize(TSize(48,52));
			TheClient->Flush();
			}
		if (aFinished==1)
			Stop();
		iCallBack.Finished(iId);
		}
	}

void CColorAnimation::IdlingL()
	{
	iIdling++;
	if(iIdling == KMaxIdlingTime)
		{
		Stop();
		iIdling = 0;
		if (iId == KRegionTrackingOnlyDsaWaitingForAbortSignal)
			{
			_LIT(KErrorAbortNotReceived,"DSA didn't get an abort signal even though the window was opened in front");
			CallBack().Log((TText8*)__FILE__,__LINE__, ESevrErr,KErrorAbortNotReceived);
			CallBack().Fail();
			}
		iCallBack.Finished(iId);
		}
	}

void CColorAnimation::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	TInt slow=iCallBack.SlowStopping(iId,iCount);
	switch (slow)
		{
	case eAbortAll:
		iCallBack.Finished(iId);
	case eAbort:
		Stop();
		return;
	case eStopDelayed:
		User::After(750000);		//0.75 secs
		break;
	default:;
		}
	++iCount;
	iTimer->Cancel();
	}

void CColorAnimation::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	TRAPD(err,StartL());
	if (err!=KErrNone)
		iCallBack.FailedReStart(iId,err);
	}


/*CScrollingTextDrawer*/

CScrollingTextDrawer* CScrollingTextDrawer::NewL(TInt aScreenNumber,CFbsScreenDevice*& aDevice,CFbsBitGc& aGc)
	{
	CScrollingTextDrawer* self=new(ELeave) CScrollingTextDrawer(aDevice,aGc);
	CleanupStack::PushL(self);
	self->ConstructL(aScreenNumber);
	CleanupStack::Pop(self);
	return self;
	}

CScrollingTextDrawer::~CScrollingTextDrawer()
	{
	if(iFontDevice)
		{
		iFontDevice->ReleaseFont(iFont);
		delete iFontDevice;
		}
	}

void CScrollingTextDrawer::ConstructL(TInt aScreenNumber)
	{
	iFontDevice=CFbsScreenDevice::NewL(aScreenNumber,iDevice->DisplayMode());
	_LIT(text,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	iText=text;
	CreateFontL();
	//iFirstChar=0;
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
#if defined(LOGGING)
	iWs.Connect();
	// point to correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(iWs);
	CleanupStack::PushL(screen);
	User::LeaveIfError(screen->Construct(aScreenNumber));
	CleanupStack::Pop(screen);

	_LIT(KConstTextDrawer,"Constructed Text Drawer");
	LogMessageText.Copy(KConstTextDrawer);
	iWs.LogMessage(LogMessageText);
	iWs.Flush();

	delete screen;
	iWs.Close();
#endif
	}

void CScrollingTextDrawer::CreateFontL()
	{
	TInt screenHeight=iDevice->SizeInPixels().iHeight;
	TFontSpec fontSpec(FontName,screenHeight);
	User::LeaveIfError(iFontDevice->GetNearestFontToDesignHeightInPixels(iFont,fontSpec));
	iGc->UseFont(iFont);
	TInt fontHeight=iFont->HeightInPixels();
	iDrawRect.iTl.iY=(screenHeight-fontHeight)/2;
	iDrawRect.iBr.iY=iDrawRect.iTl.iY+fontHeight;
	iDrawHeight=iFont->AscentInPixels();
	iCharWidth=iFont->CharWidthInPixels(iText[iFirstChar]);
	iDrawRect.iBr.iX=Max(iDevice->SizeInPixels().iWidth,iDevice->SizeInPixels().iHeight);
	iDrawRect.iTl.iX=iDrawRect.iBr.iX-iCharWidth;
	}

void CScrollingTextDrawer::SetBottomOfTest(TInt aBottom)
	{
	if (iDrawRect.iBr.iY>aBottom)
		{
		iDrawRect.iTl.iY-=iDrawRect.iBr.iY-aBottom;
		iDrawRect.iBr.iY=aBottom;
		}
	}

void CScrollingTextDrawer::Scroll()
	{
	iCharWidth=iFont->CharWidthInPixels(iText[iFirstChar]);
	iDrawRect.iTl.iX-=iJump;
	iGc->DrawText(iText.Mid(iFirstChar),iDrawRect,iDrawHeight);
	iDevice->Update();
	if (iDrawRect.iTl.iX<-iCharWidth)
		{
		if (++iFirstChar==iText.Length())
			{
			iFirstChar=0;
			iDrawRect.iTl.iX=iDevice->SizeInPixels().iWidth;
			}
		else
			iDrawRect.iTl.iX+=iCharWidth;
		iCharWidth=iFont->CharWidthInPixels(iText[iFirstChar]);
		}
	}


/*CScrollText*/

TInt CScrollText::DrawText(TAny* aAnimation)
	{
	STATIC_CAST(CScrollText*,aAnimation)->ScrollText();
	return(KErrNone);
	}

CScrollText* CScrollText::NewL(TInt aScreenNumber,TInt aId,CTWindowGroup& aParent,TInt aScrollJump,TBool aStart/*=EFalse*/)
	{
	CScrollText* self=new(ELeave) CScrollText(aId,aScrollJump,aScreenNumber);
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	if (aStart)
		self->StartL();
	CleanupStack::Pop(self);
	return self;
	}

CScrollText::~CScrollText()
	{
	delete iTimer;
	delete iTextDraw;
	iTextDraw=NULL;
	delete iDrawer;
	delete iWindow;
	TheClient->Flush();
	}

void CScrollText::ConstructL(CTWindowGroup& aParent)
	{
	iWindow=new(ELeave) CTBlankWindow();
	iWindow->ConstructL(aParent);
	RWindowBase& win=*iWindow->BaseWin();
	win.Activate();
	iDrawer=CDirectScreenAccess::NewL(TheClient->iWs,*TheClient->iScreen,win,*this);
	TheClient->Flush();
	iTimer=CPeriodic::NewL(0);
	}

void CScrollText::StartL()
	{
	DoContinueL();
	iDrawer->Gc()->SetBrushStyle(CGraphicsContext::ESolidBrush);
	if (!iTextDraw)
		{
		iTextDraw=CScrollingTextDrawer::NewL(iScreenNumber,iDrawer->ScreenDevice(),*iDrawer->Gc());
		iTextDraw->SetScrollJump(iScrollJump);
		}
	}

void CScrollText::ContinueL()
	{
	DoContinueL();
	}

void CScrollText::DoContinueL()
	{
	iTimer->Start(0,10000,TCallBack(CScrollText::DrawText,this));
	
		//0.01secs
	iDrawer->StartL();
	TRect bounding=iDrawer->DrawingRegion()->BoundingRect();
#if defined(LOGGING)
	_LIT(KBoundRect,"Continue Scroll Text  Rect=(%d,%d,%d,%d)");
	LogMessageText.Zero();
	LogMessageText.AppendFormat(KBoundRect,bounding.iTl.iX,bounding.iTl.iY,bounding.iBr.iX,bounding.iBr.iY);
	TheClient->iWs.LogMessage(LogMessageText);
	TheClient->Flush();
#endif
	TRect window=TRect(iWindow->BaseWin()->AbsPosition(),iWindow->Size());
	if (!window.Contains(bounding.iTl) || !window.Contains(bounding.iBr-TPoint(1,1)))
		AutoPanic(EAutoPanicTestFailed);
	}

void CScrollText::Stop()
	{
	iTimer->Cancel();
	iDrawer->Cancel();
	}

void CScrollText::ScrollText()
	{
	iTextDraw->Scroll();
	}

void CScrollText::AbortNow(RDirectScreenAccess::TTerminationReasons aReason)
	{
	iTimer->Cancel();
	if (!iCounting)
		return;
	if (iAbortCountDown>0)
		--iAbortCountDown;
	else
		{
		if (aReason==RDirectScreenAccess::ETerminateRegion)
			{
			User::After(1500000);		//1.5secs
			iAbortCountDown=7;
			}
		}
	}

void CScrollText::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/)
//This function is pure virtual and so cannot have an 'L' at the end of it's name
	{
	ContinueL();
	}

/*CWindowWithChild*/

CWindowWithChild* CWindowWithChild::NewL(TInt aScreenNumber, CTWindowGroup& aParent,TBool aStart/*=EFalse*/)
	{
	CWindowWithChild* self=new(ELeave) CWindowWithChild(aScreenNumber);
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	if (aStart)
		self->StartL();
	CleanupStack::Pop(self);
	return self;
	}

CWindowWithChild::~CWindowWithChild()
	{
	delete iDrawer;
	delete iChildWindow;
	delete iWindow;
	TheClient->Flush();
	}

void CWindowWithChild::ConstructL(CTWindowGroup& aParent)
	{
	iWindow=new(ELeave) CTBlankWindow();
	iWindow->ConstructL(aParent);

	iChildWindow = new(ELeave) CTWin();
	iChildWindow->ConstructWin(*iWindow);
	
	iChildWindow->SetExt(TPoint(0,0), TSize(4,4));
	RWindowBase& cwin=*iChildWindow->BaseWin();
	cwin.Activate();

	RWindowBase& win=*iWindow->BaseWin();
	win.Activate();

	iDrawer=CDirectScreenAccess::NewL(TheClient->iWs,*TheClient->iScreen,win,*this);
	
	TheClient->Flush();
	}

void CWindowWithChild::StartL()
	{
	DoContinueL();
	}

void CWindowWithChild::ContinueL()
	{
	DoContinueL();
	}

void CWindowWithChild::DoContinueL()
	{
	iDrawer->StartL();
	iRunning = ETrue;
	}

void CWindowWithChild::Stop()
	{
	iDrawer->Cancel();
	}

void CWindowWithChild::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	iRunning = EFalse;
	}

void CWindowWithChild::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/)
//This function is pure virtual and so cannot have an 'L' at the end of it's name
	{
	ContinueL();
	}

void CWindowWithChild::PerformCoverageCalls()
	{
	//add coverage to commands with no/partial coverage
	RWindow& cwin=*iChildWindow->Win();	
	TInt priority = 0;
	
	cwin.SetPointerCapturePriority(priority);
	__ASSERT_ALWAYS(cwin.GetPointerCapturePriority()==priority, User::Invariant());
	cwin.ClaimPointerGrab(EFalse);
	cwin.EnableBackup(0);
	__ASSERT_ALWAYS(cwin.PrevSibling()==0, User::Invariant());	
	cwin.Invalidate(TRect(0, 0, 10, 10));
	cwin.FadeBehind(ETrue);
	TheClient->Flush();
	// cover (empty) False condition in CWsWindow::SetFadeBehind
	cwin.FadeBehind(ETrue);
	TheClient->Flush();
	}

/*CWsBase*/

void CWsBase::ConstructL(TInt aScreenNumber, TInt aHandle)
	{
	User::LeaveIfError(iWs.Connect());
	iScrDev=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScrDev->Construct(aScreenNumber));
	iGroup=RWindowGroup(iWs);
	User::LeaveIfError(iGroup.Construct(aHandle,EFalse));
	iGroup.SetOrdinalPosition(0,KMainTestBaseWindow);
	}

void CWsBase::CreateBlankWindowL(RBlankWindow& iWin,TInt aHandle)
	{
	iWin=RBlankWindow(iWs);
	User::LeaveIfError(iWin.Construct(iGroup,aHandle));
	}

CWsBase::~CWsBase()
	{
	iGroup.Close();
	delete iScrDev;
	iWs.Close();
	}


/*CAnimating*/

TInt CAnimating::StartLC(TAny* aScreenNumber)
	{
	CAnimating* self=new(ELeave) CAnimating();
	CleanupStack::PushL(self);
	self->ConstructL((TInt)aScreenNumber);
	return KErrNone;
	}

void CAnimating::ConstructL(TInt aScreenNumber)
	{
	CWsBase::ConstructL(aScreenNumber,798);
#if defined(LOGGING)
	_LIT(KAnimate1,"Constructed CWsBase");
	LogMessageText.Copy(KAnimate1);
	iWs.LogMessage(LogMessageText);
	iWs.Flush();
#endif
	iGroup.EnableReceiptOfFocus(EFalse);
	CreateBlankWindowL(iWindow,799);
	User::LeaveIfError(iWindow.SetRequiredDisplayMode(EGray16));
	iWindow.Activate();
#if defined(LOGGING)
	_LIT(KAnimate2,"Set up Windows");
	LogMessageText.Copy(KAnimate2);
	iWs.LogMessage(LogMessageText);
#endif
	iWs.Flush();
	TDisplayMode displayMode=CWsBase::iScrDev->DisplayMode();
	if (displayMode<EGray16)
		displayMode=EGray16;
	iScrDev=CFbsScreenDevice::NewL(aScreenNumber,displayMode);
	User::LeaveIfError(iScrDev->CreateContext(iGc));
#if defined(LOGGING)
	_LIT(KAnimate3,"Created Screen Device");
	LogMessageText.Copy(KAnimate3);
	iWs.LogMessage(LogMessageText);
	iWs.Flush();
#endif
	iDirect=CDirectScreenAccessOld::NewL(iWs,*this);
	iTimer=CPeriodic::NewL(0);
	User::LeaveIfError(iControlSem.OpenGlobal(SemControl,EOwnerThread));		//Must be thread relative, since it won't get cleaned up when the thread is killed otherwise
	User::LeaveIfError(iControlQueue.OpenGlobal(QueueControl,EOwnerThread));	//Must be thread relative, since it won't get cleaned up when the thread is killed otherwise
	iSemCreated=ETrue;
#if defined(LOGGING)
	_LIT(KAnimate5,"Created Direct, Timer and Semaphore");
	LogMessageText.Copy(KAnimate5);
	iWs.LogMessage(LogMessageText);
	iWs.Flush();
#endif
	iScrSize=iScrDev->SizeInPixels();
	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	StartAnimatingL();
#if defined(LOGGING)
	_LIT(KAnimate6,"Started Animation");
	LogMessageText.Copy(KAnimate6);
	iWs.LogMessage(LogMessageText);
	iWs.Flush();
#endif
	}

CAnimating::~CAnimating()
	//This function should never actually get run in practice
	{
	if (iSemCreated)
		{
		iControlSem.Close();
		iControlQueue.Close();
		}
	if (iDrawingRegion)
		iDrawingRegion->Destroy();
	delete iTimer;
	delete iDirect;
	delete iGc;
	delete iScrDev;
	iWindow.Close();
	}

void CAnimating::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	iTimer->Cancel();
	iControlSem.Signal();
	TInt data = 2;
	iControlQueue.Send(&data,sizeof(TInt));
	User::After(10000000);		//10 secs
	}

TInt NextColor(TAny* aAnimation)
	{
	STATIC_CAST(CAnimating*,aAnimation)->DrawFrame();
	return(KErrNone);
	}

void CAnimating::DrawFrame()
	{
	iColor=(iColor+3)%16;
	iGc->SetBrushColor(TRgb::Gray16(iColor));
	iGc->DrawRect(iScrSize);
	iScrDev->Update();
	if (iColor==2)
		{
		iControlSem.Signal();
		TInt data = 1;
		iControlQueue.Send(&data,sizeof(TInt));
		}
	}

void CAnimating::StartAnimatingL()
	{
	iTimer->Start(0,150000,TCallBack(NextColor,this));		//0.15 secs

	User::LeaveIfError(iDirect->Request(iDrawingRegion,iWindow));
	iGc->SetClippingRegion(iDrawingRegion);
	}


/*CMoveWindow*/

TInt CMoveWindow::StartLC(TAny* aScreenNumber)
	{
	CMoveWindow* self=new(ELeave) CMoveWindow();
	CleanupStack::PushL(self);
	self->ConstructL((TInt)aScreenNumber);
	return KErrNone;
	}

TInt CMoveWindow::FlipChange(TAny* aMoveWin)
	{
	Cast(aMoveWin)->FlipChange();
	return(KErrNone);
	}

TInt CMoveWindow::StateChange(TAny* aMoveWin)
	{
	Cast(aMoveWin)->StateChange();
	return(KErrNone);
	}

CMoveWindow::~CMoveWindow()
	{
	delete iStateTimer;
	delete iTimer;
	iWindow.Close();
	iBackUpWin.Close();
	delete iGc;
	}

TInt MoveWin(TAny* aMoveWin)
	{
	STATIC_CAST(CMoveWindow*,aMoveWin)->MoveWindow();
	return(KErrNone);
	}

void CMoveWindow::ConstructL(TInt aScreenNumber)
	{
	CWsBase::ConstructL(aScreenNumber,898);
	iBounceArea=TRect(iScrDev->SizeInPixels());
	iSize.iHeight=iBounceArea.iBr.iY/5;
	iSize.iWidth=iBounceArea.iBr.iX/5;
	iBounceArea.iTl.iX=iBounceArea.iBr.iX/6;
	iBounceArea.iBr.iX=5*iBounceArea.iBr.iX/6;
	iBounceArea.iTl.iY=iBounceArea.iBr.iY/4;
	iBounceArea.iBr.iY=3*iBounceArea.iBr.iY/4;
	iDelta=TSize(3,3);
	iTl=iBounceArea.iTl;
	CreateBlankWindowL(iWindow,899);
	iWindow.SetExtent(iTl,iSize);
	iWindow.SetShadowDisabled(ETrue);
	iWindow.SetColor(TRgb::Gray4(2));
	iWindow.Activate();
	iBackUpWin=RBackedUpWindow(iWs);
	User::LeaveIfError(iBackUpWin.Construct(iGroup,EGray4,698));
	User::LeaveIfError(iBackUpWin.SetSizeErr(TSize(2,2)));
	iGc=new(ELeave) CWindowGc(iScrDev);
	User::LeaveIfError(iGc->Construct());
	iDisplayMode=EGray4;
	if (MoveInterval>TTimeIntervalMicroSeconds32(0))
		CreateTimerL();
	if (ModeInterval>TTimeIntervalMicroSeconds32(0))
		{
		iStateTimer=CPeriodic::NewL(0);
		iStateTimer->Start(ModeInterval,ModeInterval,TCallBack(CMoveWindow::StateChange,this));
		}
	if (FlipInterval>TTimeIntervalMicroSeconds32(0))
		{
		iDevice=new(ELeave) CWsScreenDevice(iWs);
		User::LeaveIfError(iDevice->Construct(aScreenNumber));
		iNumOfModes=iDevice->NumScreenModes();
		if (iNumOfModes>1)
			{
			iFlipTimer=CPeriodic::NewL(0);
			iFlipTimer->Start(FlipInterval,FlipInterval,TCallBack(CMoveWindow::FlipChange,this));
			}
		}
	iStateCountDown=0;
	}

void CMoveWindow::CreateTimerL()
	{
	iTimer=CPeriodic::NewL(0);
	iTimer->Start(0,MoveInterval,TCallBack(MoveWin,this));

	}

void CMoveWindow::MoveWindow()
	{
	if (--iStateCountDown>=0)
		{
		if (iStateCountDown==2)
			ToggleDisplayMode();
		return;
		}
	TPoint iBr=iTl+iSize;
	TSize iDeltaSize;
	if ((iDelta.iHeight<0 && iTl.iY<=iBounceArea.iTl.iY) || (iDelta.iHeight>0 && iBr.iY>=iBounceArea.iBr.iY))
		{
		iDelta.iHeight=-iDelta.iHeight;
		iDeltaSize.iWidth=2;
		}
	if ((iDelta.iWidth<0 && iTl.iX<=iBounceArea.iTl.iX) || (iDelta.iWidth>0 && iBr.iX>=iBounceArea.iBr.iX))
		{
		iDelta.iWidth=-iDelta.iWidth;
		if (iDeltaSize.iWidth==0)
			iDeltaSize.iHeight=2;
		else
			iDeltaSize.iWidth=0;
		}
	iTl+=iDelta;
	iSize+=iDeltaSize;
	iWindow.SetExtent(iTl,iSize);
	iWs.Flush();
	}

void CMoveWindow::StateChange()
	{
	if (ImmediateModeSwitch>1)
		{
		if (MoveInterval>TTimeIntervalMicroSeconds32(0) && iTimer==NULL)
			{
			TRAPD(err,CreateTimerL());
			if (err==KErrNone)
				ImmediateModeSwitch=ETrue;
			}
		}
	if (ImmediateModeSwitch || MoveInterval==TTimeIntervalMicroSeconds32(0))
		ToggleDisplayMode();
	else
		{
		if (iStateCountDown<-8)
			iStateCountDown=6;
		}
	}

void CMoveWindow::ToggleDisplayMode()
	{
	if(iDisplayMode==EColor16MU)
		{
		return;		//Test probably doesn't work with mode EColor16MU
		}
	iDisplayMode=(iDisplayMode==EColor16M ? EGray4:EColor16M);
#if defined(LOGGING)
	TInt newMode=
#endif
				iWindow.SetRequiredDisplayMode(iDisplayMode);
	iGc->Activate(iBackUpWin);
	iBackUpWin.SetPosition(iDisplayMode==EColor16M ? TPoint(1,0):TPoint(0,1));
#if defined(LOGGING)
	TDisplayMode curentMode=iScrDev->DisplayMode();
	_LIT(KToggleMode,"Toggle Display Mode  Mode=%d Window=%d Actual=%d");
	LogMessageText.Zero();
	LogMessageText.AppendFormat(KToggleMode,(TInt&)iDisplayMode,newMode,(TInt&)curentMode);
	iWs.LogMessage(LogMessageText);
	//iWs.LogCommand(RWsSession::ELoggingStatusDump);
#endif
	iWs.Flush();
	iGc->Deactivate();
	}

void CMoveWindow::FlipChange()
	{
	if (++iCurrentMode==2) //flip between modes 0 and 1
		iCurrentMode=0;
	iDevice->SetScreenMode(iCurrentMode);
	iWs.Flush();
	}

//
// CBugFixColorAnimation
//
// This class is used for reproducing a defect found on 6.1: KAA-5J3BLW "Unnecessary Wserv's DSA abort".
// The problem was that a direct screen access client was getting an unnecessary abort notification
// when a new window (or window group) was created but not visible.
// This class will simulate the direct screen access client and it will check whether the first DSA abort
// is not caused by just creating a window.

CBugFixColorAnimation* CBugFixColorAnimation::NewL(TInt aScreenNumber,TInt aId,MAnimCallBacks& aCallBack,CTWindowGroup& aParent,TRect aExtent,TBool aStart)
	{
	CBugFixColorAnimation* self=new(ELeave) CBugFixColorAnimation(aScreenNumber, aId,aCallBack);
	CleanupStack::PushL(self);
	self->ConstructL(aParent,aExtent);
	if (aStart)
		{
		self->StartL();
		self->Started();
		}
	CleanupStack::Pop(self);
	return self;
	}

CBugFixColorAnimation::CBugFixColorAnimation(TInt aScreenNumber,TInt aId,MAnimCallBacks& aCallBack)
	: CColorAnimation(aScreenNumber, aId, aCallBack)
	{
		iThreadParam.iScreenNumber = aScreenNumber;
	}

CBugFixColorAnimation::~CBugFixColorAnimation()
	{
	iSem.Close();
	if(iThread)
		{
		TRequestStatus status;
		iThread->Logon(status);
		if (iThread->StillAlive())
			{
 			iThread->Terminate(KErrNone);
			User::WaitForRequest(status);
			}
		delete iThread;
		}
	iTestFailed->Cancel();
	delete iTestFailed;
	}


LOCAL_D TInt TestFailed(TAny* aAnimation)
	{
	CBugFixColorAnimation* anim=reinterpret_cast<CBugFixColorAnimation*>(aAnimation);
	TRAP_IGNORE(
		anim->Stop();
		anim->CallBack().Fail();
		anim->FinishTest();
		);
	return KErrNone;
	}

LOCAL_D TInt CreateNewWindowGroup(TAny* aParam)
	{
	TDirectThreadParam* param = (TDirectThreadParam*)aParam;
	TRect rect = param->iRect;
	TBool isInFront = param->iIsInFront;
	RWsSession ws;
	TInt error=ws.Connect();

	CWsScreenDevice* screen = NULL;
	TRAP(error, screen = new (ELeave) CWsScreenDevice(ws));
	if (error!=KErrNone)
		{
		ws.Close();
		RThread::Rendezvous(error);
		return error;
		}

	if ((error=screen->Construct(param->iScreenNumber))!=KErrNone)
		{
		delete screen;
		ws.Close();
		RThread::Rendezvous(error);
		return error;
		}
	RSemaphore sem;
	error = sem.OpenGlobal(KSem_DefectFix_KAA_5J3BLW_Name, EOwnerThread);
	if (error!=KErrNone)
	    {
	    ws.Close();
	    RThread::Rendezvous(error);
	    return error;
	    }
	
	RThread::Rendezvous(KErrNone);

	// wait for the dsa to start before creating the new window group
	sem.Wait();
	sem.Close();

	
	RWindowGroup group(ws);
	group.Construct(431,EFalse);
	if(isInFront)
		{
		group.SetOrdinalPosition(0,KAboveMainTestBaseWindow);
		}
	else
		{
		group.SetOrdinalPosition(2);
		}
	RWindow window(ws);
	error=window.Construct(group, 432);
	if (error==KErrNone)
		{
		window.SetExtentErr(rect.iTl, rect.Size());
		window.SetOrdinalPosition(0,0);
		ws.Flush();
		window.Activate();
		ws.Flush();
		if(!isInFront)
			{
			group.SetOrdinalPosition(-1);
			}
		ws.Flush();
		//Wait for wserv to render new window on top of the existing DSA region.
		ws.Finish(); 
		//Now the window has been rendered (and DSA should have been aborted).
		window.Close();
		}

	group.Close();
	delete screen;
	ws.Close();
	return error;
	}

void CBugFixColorAnimation::ConstructL(CTWindowGroup& aParent,TRect aExtent)
	{
	CColorAnimation::ConstructL(aParent, aExtent,KDrawingDsa);
	_LIT(ThreadName,"Create new Window");
	iAnimRect=aExtent;
	TInt error=iSem.CreateGlobal(KSem_DefectFix_KAA_5J3BLW_Name, 0);
	if (error==KErrNone)
	    {
	    iThreadParam.iRect = iAnimRect;
	    iThreadParam.iIsInFront = EFalse;
	    TThreadStartUp function=TThreadStartUp(CreateNewWindowGroup, &iThreadParam);
	    TRequestStatus status;
	    iThread=CProcess::NewThreadRendezvousL(ThreadName,&function, status);
	    User::WaitForRequest(status);
	    if (status != KErrNone)
	        {
	        RDebug::Printf("the request status is returned to be non KErrNone: %d", status.Int());
	        TestFailed(this);
	        }
		iTestFailed = CIdle::NewL(0);
	    }
	else
	    {
	    TestFailed(this);
	    }
	}

void CBugFixColorAnimation::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	Stop();
	if (!iTestFailed->IsActive())
		{
		iTestFailed->Start(TCallBack(TestFailed,this));
		}
	}

void CBugFixColorAnimation::Restart(RDirectScreenAccess::TTerminationReasons aReason)
	{
	AbortNow(aReason);
	}

TInt CPanicDirect::DoTestOnNewSchedulerL(TInt aInt, TAny* aPtr)
	{	
	CActiveScheduler* activeScheduler=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);
	CleanupStack::PushL(activeScheduler);
	DoTestNowL(aInt,aPtr);
	CleanupStack::PopAndDestroy(activeScheduler);
	return(EWsExitReasonBad);	//calls to this method currently all ignore the return code anyway.
	}

TInt CPanicDirect::DoTestOnNewScheduler(TInt aInt, TAny* aPtr)
	{
	TInt rv=EWsExitReasonBad;	
	TRAP_IGNORE(rv=DoTestOnNewSchedulerL(aInt,aPtr));
	return(rv);	//calls to this method currently all ignore the return code anyway.
 	}

void CPanicDirect::DoTestNowL(TInt aInt, TAny* aPtr)
	{
	CPanicDirect* self=new(ELeave) CPanicDirect();
	CleanupStack::PushL(self);
	self->ConstructL((TInt)aPtr, aInt);
	self->TestL();
	CleanupStack::PopAndDestroy(self);
	}

void CPanicDirect::ConstructL(TInt aScreenNumber, TInt aInt)
	{
	iScreenNumber = aScreenNumber;

	User::LeaveIfError(iTimer.CreateLocal());
	User::LeaveIfError(iWs.Connect());
	iWsScrDev=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iWsScrDev->Construct(iScreenNumber));

#if defined(LOGGING)
	_LIT(KPanicThead1,"PanicThread: Into ConstructL");
	LogMessageText.Copy(KPanicThead1);
	iWs.LogMessage(LogMessageText);
	iWs.Flush();
#endif
	iGroup=RWindowGroup(iWs);
	User::LeaveIfError(iGroup.Construct(2200+aInt,EFalse));
	iGroup.SetOrdinalPosition(0,KPanicTestOrdinalPriority);
	iBlankWin=RBlankWindow(iWs);
	User::LeaveIfError(iBlankWin.Construct(iGroup,2300+aInt));
	iBlankWin.Activate();
	iDirect=RDirectScreenAccess(iWs);
	User::LeaveIfError(iDirect.Construct());
	iDisplayMode=iWsScrDev->DisplayMode();
#if defined(LOGGING)
	_LIT(KPanicThead2,"PanicThread: Creating Screen Device, Mode=%d");
	LogMessageText.Format(KPanicThead2,iDisplayMode);
	iWs.LogMessage(LogMessageText);
	iWs.Flush();
#endif
	iScreenDevice=CFbsScreenDevice::NewL(aScreenNumber,iDisplayMode);
#if defined(LOGGING)
	_LIT(KPanicThead3,"PanicThread: Created Device");
	LogMessageText.Copy(KPanicThead3);
	iWs.LogMessage(LogMessageText);
	iWs.Flush();
#endif
	User::LeaveIfError(iScreenDevice->CreateContext(iGc));
#if defined(LOGGING)
	_LIT(KPanicThead4,"PanicThread: Created Context");
	LogMessageText.Copy(KPanicThead4);
	iWs.LogMessage(LogMessageText);
	iWs.Flush();
#endif
	iTextDraw=CScrollingTextDrawer::NewL(iScreenNumber,iScreenDevice,*iGc);
#if defined(LOGGING)
	_LIT(KPanicThead5,"PanicThread: Created ScrollDrawer");
	LogMessageText.Copy(KPanicThead5);
	iWs.LogMessage(LogMessageText);
	iWs.Flush();
#endif
	iTextDraw->SetScrollJump(4);
#if defined(LOGGING)
	_LIT(KCreatedDrawer,"PanicThread: CreatedDrawer");
	LogMessageText.Copy(KCreatedDrawer);
	iWs.LogMessage(LogMessageText);
	iWs.Flush();
#endif
	iTestNo=aInt;
	}

CPanicDirect::~CPanicDirect()
	{
	iDirect.Close();
	delete iGc;
	delete iScreenDevice;
	delete iTextDraw;
	iBlankWin.Close();
	iGroup.Close();
	delete iWsScrDev;
	iWs.Close();
	if (iRegion)
		iRegion->Close();
	iTimer.Close();
	}

void CPanicDirect::TestL()
	{
	if (iTestNo==2)
		{
		iDirect.Completed();
		return;
		}
	TInt err=iDirect.Request(iRegion,iDirectStatus,iBlankWin);
	if (err!=KErrNone || !iRegion)
		return;
	TRect screen(iScreenDevice->SizeInPixels());
	TRect bounding=iRegion->BoundingRect();
	if (!screen.Contains(bounding.iTl) || !screen.Contains(bounding.iBr-TPoint(1,1)))
		goto Cancel;
	iGc->SetClippingRegion(iRegion);
	iDrawingAllowed=ETrue;
	iTimer.After(iTimerStatus,50000);		//0.05secs
	FOREVER
		{
		User::WaitForRequest(iDirectStatus,iTimerStatus);
		if (iDirectStatus!=KRequestPending)
			iDrawingAllowed=EFalse;
		else if (iTimerStatus!=KRequestPending)
			{
			if (iDrawingAllowed)
				DoDrawingL();
			iTimer.After(iTimerStatus,50000);		//0.05secs
			}
		else
			{
	Cancel:
			iDirect.Cancel();
			return;
			}
		}
	}

void CPanicDirect::DoDrawingL()
	{
	++iCount;
	iTextDraw->Scroll();
	if (iTestNo==1 && iCount==8)
		{
		iDirect.Request(iRegion,iDirectStatus,iBlankWin);
		return;
		}
	if (iTestNo==3 && iCount==12)
		{
		iDirect.Completed();
		iDirect.Completed();
		return;
		}
	if (iTestNo==4 && iCount==16)
		{
		iBlankWin.SetSize(TSize(20,25));
		delete iRegion;
		User::LeaveIfError(iDirect.Request(iRegion,iDirectStatus,iBlankWin));
		return;
		}
	if (iTestNo==5 && iCount==19)
		{
		iWs.Close();
		return;
		}
	}


/*CTDirect*/

CTDirect::CTDirect(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	iState = 0;
	iNextFrameFinished = ETrue;
	iPackagingFinished = EFalse;
	iTimerRunning = EFalse;
	}

CTDirect::~CTDirect()
	{
	DeleteMoveWindow();
	DeleteScroll();
			
	delete iAnim;
	TheClient->WaitForRedrawsToFinish();
	delete iCallBackWin;
	// put focus back to current screen as this test changed the focus screen to primary screen
	TheClient->iWs.SetFocusScreen(iTest->iScreenNumber);
	}

void CTDirect::ConstructL()
	{
	_LIT(KCTDirectConstructL,"AUTO  Construct Direct Test");
	LOG_MESSAGE(KCTDirectConstructL);
	FullScreenModeSize=TheClient->iScreen->SizeInPixels();
	iIsScalingSupported=CheckScalingSupportedOrNot();
	iNumOfCallBack=0;
	iCallBackWin=new(ELeave) CTBlankWindow;
	iCallBackWin->ConstructL(*TheClient->iGroup);
	User::LeaveIfError(iCallBackWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	iCallBackWin->SetExt(TPoint(),TheClient->iScreen->SizeInPixels());
	iCallBackWin->SetVisible(EFalse);
	iCallBackWin->Activate();
	// the following line makes sure that a console object hidden outside of
	// screens range doesn't affect test results being on top of tested objects
	TheClient->iGroup->GroupWin()->SetOrdinalPosition(0, KMainTestBaseWindow);
	}

void CTDirect::ConstrucBlankWindowL()
	{
	delete iWin;
	iWin=new(ELeave) CTBlankWindow;
	iWin->ConstructL(*TheClient->iGroup);
	iWin->BaseWin()->SetShadowDisabled(ETrue);
	iWin->BaseWin()->Activate();
	TheClient->Flush();
	}

void CTDirect::InitialiseAnimationL()
	{
	ConstrucBlankWindowL();
	TInt colors,greys;
	TDisplayMode dispMode=TheClient->iWs.GetDefModeMaxNumColors(colors,greys);
	iScrDev=CFbsScreenDevice::NewL(iTest->iScreenNumber,dispMode);
	User::LeaveIfError(iScrDev->CreateContext(iGc));
	INFO_PRINTF1(_L(" Constructed Screen Device"));
	iScrSize=iScrDev->SizeInPixels();
	TFontSpec fontSpec(FontName,iScrSize.iHeight);
	fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	User::LeaveIfError(iScrDev->GetNearestFontToDesignHeightInPixels(iFont,fontSpec));
	iGc->UseFont(iFont);
	iFlags|=eFontSet;
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TInt fontHeight=iFont->HeightInPixels();
	iDrawRect.iTl.iY=(iScrSize.iHeight-fontHeight)/2;
	iDrawRect.SetHeight(fontHeight);
	iDrawHeight=iFont->AscentInPixels();
	TSize winSize(25,20);
	iWin2=new(ELeave) CTBlankWindow;
	iWin2->ConstructL(*TheClient->iGroup);
	iWin2->SetExt(TPoint((iScrSize.iWidth-winSize.iWidth)/2,(iScrSize.iHeight-winSize.iHeight)/2),winSize);
	iWin2->SetColor(TRgb::Gray4(2));
	iWin2->BaseWin()->Activate();
	INFO_PRINTF1(_L(" Setup Window"));
	iScrDev->Update();
	TheClient->iWs.Finish();
	
	User::LeaveIfError(iControlSem.CreateGlobal(SemControl,0,KOwnerType));
	User::LeaveIfError(iControlQueue.CreateGlobal(QueueControl,1,sizeof(TInt),KOwnerType));

	iFlags|=eDirectControlSem;
	User::LeaveIfError(iWinSem.CreateGlobal(SemNextOp,0,KOwnerType));
	iFlags|=eDirectWindowSem;
	iProcess=CProcess::NewTL(CProcess::eProcessDirectTest,iTest->iScreenNumber,&iThreadStatus);
	INFO_PRINTF1(_L(" Constructed Semaphores & Thread"));
	iControlSem.Wait();	
	
	iTimer=CPeriodic::NewL(iTest->EAutoTestPriority);
	iRestart=CIdle::NewL(iTest->EAutoTestPriority+5);
	iDirect=CDirectScreenAccessOld::NewL(TheClient->iWs,*this);
	}

void CTDirect::DestroyAnimation()
	{
	ResetScreenSizeMode();
	if(iProcess && iProcess->StillAlive())
		{
		if (iFlags&eDirectControlSem)
			{
			iControlSem.Signal();
			TInt data = 3;
			iControlQueue.Send(&data,sizeof(TInt));
			}
		if (iFlags&eDirectWindowSem)
			iWinSem.Signal();
		}
	
	delete iTimer;
	iTimer=NULL;
	delete iRestart;
	iRestart=NULL;
	delete iDirect;
	iDirect=NULL;
	delete iChangeScreenModeTimer;
	iChangeScreenModeTimer=NULL;
	delete iScreenModeTimer;
	iScreenModeTimer=NULL;
	
	if (iFlags&eFontSet)
		{
		iGc->DiscardFont();
		iScrDev->ReleaseFont(iFont);
		}
	delete iGc;
	if (iDrawingRegion)
		iDrawingRegion->Destroy();
	delete iScrDev;
	delete iWin;
	delete iWin2;
	if (iProcess)
		{
		User::WaitForRequest(iThreadStatus);
		delete iProcess;
		iProcess = NULL;
		}
	if (iFlags&eDirectControlSem)
		{
		iControlSem.Close();
		iControlQueue.Close();
		}
	if (iFlags&eDirectWindowSem)
		iWinSem.Close();
	}

void CTDirect::ResetScreenSizeMode()
	{
	if (iModeBackup!=ESizeEnforcementNone)
		{
		TheClient->iScreen->SetScreenModeEnforcement(ESizeEnforcementNone);
		iModeBackup=ESizeEnforcementNone;
		}
	TheClient->iScreen->SetCurrentRotations(0,CFbsBitGc::EGraphicsOrientationNormal);
	TheClient->iScreen->SetScreenMode(0);
	TPixelsAndRotation pixelsAndRotation;
	TheClient->iScreen->GetDefaultScreenSizeAndRotation(pixelsAndRotation);
	TheClient->iScreen->SetScreenSizeAndRotation(pixelsAndRotation);
	User::After(350000);		//0.35secs
	}

void CTDirect::ScanTypeFacesL()
	{
	CFbsScreenDevice* scrdev=CFbsScreenDevice::NewL(iTest->iScreenNumber,EGray4);
	TTypefaceSupport typeFace;
	TInt numFaces=scrdev->NumTypefaces();
	TInt ii;
	for (ii=0;ii<numFaces;++ii)
		{
		scrdev->TypefaceSupport(typeFace,ii);
		}
	delete scrdev;
	}

TInt RestartAnimation(TAny* aTest)
	{
	STATIC_CAST(CTDirect*,aTest)->Restart();
	return(KErrNone);
	}

TInt NextFrame(TAny* aTest)
	{
	STATIC_CAST(CTDirect*,aTest)->DrawFrame();
	return(KErrNone);
	}

void CTDirect::AbortNow(RDirectScreenAccess::TTerminationReasons aReason)
	{
	TEST(aReason==RDirectScreenAccess::ETerminateRegion);
	
	if (iDelay)
		User::After(2000000);		//2 secs
	if (iCancelNext)
		{
		iFrameNo=eDirectNumFrames;
		iTimer->Cancel();	
		iDirect->Cancel();
		TheClient->Flush();
		iNextFrameFinished = ETrue;
		iState++;
		return;
		}
	iRestart->Start(TCallBack(RestartAnimation,this));
	}

TBool CTDirect::Restart()
	{
	StartDirect();
	return ETrue;
	}

#pragma warning( disable : 4244 )	//conversion from 'int' to 'unsigned short', possible loss of data
TBool CTDirect::DrawFrame()
	{
	if (iFrameNo==eDirectNumFrames)
		{
		iState++;
		iTimer->Cancel();
		if (iFrameNo==eDirectNumFrames)
			iDirect->Cancel();
		iNextFrameFinished = ETrue;
		return EFalse;
		}
	_LIT(Text,"A0");
	TBuf<2> iString(Text);		//_LIT(Text,"ABCabc123");
	iString[0]+=iFrameNo;
	iString[1]+=iFrameNo;
	TInt textWidth=iFont->TextWidthInPixels(iString);
	TInt maxTextWidth=Max(textWidth,iLastTextWidth);
	iLastTextWidth=textWidth;
	if (iLastMaxTextWidth!=maxTextWidth)
		{
		iLastMaxTextWidth=maxTextWidth;
		iDrawRect.iTl.iX=(iScrSize.iWidth-iLastMaxTextWidth)/2;
		iDrawRect.SetWidth(iLastMaxTextWidth);
		}
	TInt colorOffset=iFrameNo%2;
	iGc->SetPenColor(TRgb::Gray4(colorOffset));
	iGc->DrawText(iString,iDrawRect,iDrawHeight,CGraphicsContext::ECenter);
	iGc->SetPenColor(TRgb::Gray4(1+colorOffset));
	iGc->DrawLine(TPoint(),iScrSize.AsPoint());
	iGc->DrawLine(TPoint(iScrSize.iWidth,0),TPoint(0,iScrSize.iHeight));
	iGc->DrawLine(TPoint(iScrSize.iWidth/2,0),TPoint(iScrSize.iWidth/2,iScrSize.iHeight));
	iGc->DrawLine(TPoint(0,iScrSize.iHeight/2),TPoint(iScrSize.iWidth,iScrSize.iHeight/2));
	iScrDev->Update();
	++iFrameNo;
	if (iFrameNo==iFrameToSignalAfter)
		SignalWindow();
	return ETrue;
	}
#pragma warning( default : 4244 )

void CTDirect::SignalWindow()
	{
	switch(iState)
		{
	case 0:
		switch(iFrameNo)
			{
		case 0:
			iFrameToSignalAfter=0;
			}
		return;
	case 1:
	case 3:
		switch(iFrameNo)
			{
		case 0:
			iFrameToSignalAfter=3;
			iDelay=(iState==3);
			return;
		case 3:
			WinExt.SetSize(TSize(iScrSize.iWidth/2-5,iScrSize.iHeight/2+30));
			iFrameToSignalAfter=5;
			break;
		case 5:
			iFrameToSignalAfter=7;
			break;
		case 7:
			WinExt.SetSize(TSize(iScrSize.iWidth/2+30,iScrSize.iHeight/2-5));
			WinCol=1;
			iFrameToSignalAfter=8;
			break;
		case 8:
			iFrameToSignalAfter=0;
			iDelay=EFalse;
			break;
			}
		break;
	case 2:
		switch(iFrameNo)
			{
		case 0:
			iFrameToSignalAfter=2;
			return;
		case 2:
			WinExt.SetRect(TPoint(iScrSize.iWidth/2-15,iScrSize.iHeight/2-20),TSize(iScrSize.iWidth/3,iScrSize.iHeight/3));
			WinCol=2;
			iFrameToSignalAfter=5;
			break;
		case 5:
			iFrameToSignalAfter=6;
			break;
		case 6:
			WinExt.SetRect(TPoint(iScrSize.iWidth/2-50,iScrSize.iHeight/3),TSize(iScrSize.iWidth/4,iScrSize.iHeight/3));
			WinCol=3;
			iFrameToSignalAfter=8;
			break;
		case 8:
			iFrameToSignalAfter=0;
			break;
			}
		break;
	case 4:
		switch(iFrameNo)
			{
		case 0:
			iFrameToSignalAfter=3;
			return;
		case 3:
			iFrameToSignalAfter=6;
			iWin2->SetVisible(EFalse);
			TheClient->Flush();
			return;
		case 6:
			iFrameToSignalAfter=0;
			iWin2->SetVisible(ETrue);
			TheClient->Flush();
			return;
			}
		break;
	case 5:
		switch(iFrameNo)
			{
		case 0:
			iFrameToSignalAfter=6;
			return;
		case 6:
			iFrameToSignalAfter=0;
			WinExt.SetRect(10,10,20,20);
			iCancelNext=ETrue;
			break;
			}
		break;
	case 6:
		switch(iFrameNo)
			{
		case 0:
			iFrameToSignalAfter=7;
			iCancelNext=EFalse;
			return;
		case 7:
			iFrameToSignalAfter=0;
			delete iWin;
			TheClient->Flush();
			iWin=NULL;
			iWindowDead=ETrue;
			return;
			}
		break;
		}
#if defined(LOGGING)
	_LIT(KSignal,"Signaling Move Window");
	LogMessageText.Copy(KSignal);
	TheClient->iWs.LogMessage(LogMessageText);
	TheClient->Flush();
#endif
	iWinSem.Signal();
	}

void CTDirect::StartDirect()
	{
	if (iWin==NULL)
		{
		TEST(iWindowDead);
		if (!iWindowDead)
			INFO_PRINTF3(_L("iWindowDead - Expected: %d, Actual: %d"), ETrue, iWindowDead);		
	
		iFrameNo=eDirectNumFrames;
		return;
		}
	TEST(!iWindowDead);
	if (iWindowDead)
		INFO_PRINTF3(_L("iWindowDead - Expected: %d, Actual: %d"), EFalse, iWindowDead);		

	if (iDrawingRegion)
		iDrawingRegion->Destroy();
	TInt retVal = iDirect->Request(iDrawingRegion,*iWin->BaseWin());
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("iDirect->Request(iDrawingRegion,*iWin->BaseWin()) return value - Expected: %d, Actual: %d"), KErrNone, retVal);		

	iGc->SetClippingRegion(iDrawingRegion);
	}

void CTDirect::LogLeave(TInt aErr)
	{
	iTest->LogLeave(aErr);
	}

void CTDirect::Fail()
	{
	TEST(EFalse);
	}

void CTDirect::Finished(TInt aId)
	{
	//aId refers to the animation, it is not the test number
	_LIT(KCTDirectFinished,"Destroying animation number %d");
	LOG_MESSAGE2(KCTDirectFinished,aId);
	switch (aId)
		{
	case 10:
	case 11:
		ResetScreenSizeMode();
	case 9:
		iPackagingFinished = ETrue;
	case 8:
		DeleteMoveWindow();
		//iPackagingFinished = ETrue;
		if (iScroll)
			{
			DeleteScroll();
		#if defined(LOGGING)
			_LIT(KDeleteScroll,"Deleting Scroll Text  Id=%d  TimerActive=%d");
			LogMessageText.Zero();
			LogMessageText.AppendFormat(KDeleteScroll,aId,iAnim->IsTimerActive());
			TheClient->iWs.LogMessage(LogMessageText);
			TheClient->Flush();
		#endif
			}
		if (aId==8  && iAnim->IsTimerActive())
			return;
	case 1:
	case 12:
	case 25:	//case DSA using Region tracking Only (abort signal expceted)
	case 26:	//case DSA using Region tracking Only (abort signal not expceted)
		{
		iPackagingFinished = ETrue;
		delete iAnim;
		iAnim=NULL;
CHECKHANDLES:
		TInt numProcessHandles;
		TInt numThreadHandles;
		RThread().HandleCount(numProcessHandles,numThreadHandles);
		TEST(Abs(numThreadHandles-iNumThreadHandles)<2);
		if (Abs(numThreadHandles-iNumThreadHandles)>=2)
			INFO_PRINTF3(_L("Abs(numThreadHandles-iNumThreadHandles)<2 - Expected: %d or less, Actual: %d"), 2, Abs(numThreadHandles-iNumThreadHandles));		
		}
		break;
	case 16:
	case 15:	
		{		
		// Stop the animation and delete it, but make sure that you don't increment iState and call Rquest()
		// until this is the second call i.e. one of aId = 15 or aId = 16 has already been finished.
		TInt index = aId-15;
		iAnims[index]->Stop();
		delete iAnims[index];
		iAnims[index]=NULL;
		// This test creates two animations iAnims[0] and iAnims[1].
		// Use (index ^ 1 ) = Toggle the Index, to get the index of other animation.
		// If iAnims [ index ^ 1] is NULL then this is the second Call and test is finished.
		if (iAnims[index ^ 1] != NULL)
			{
			return;
			}
		iPackagingFinished = ETrue;
		goto CHECKHANDLES;
		}
	case 13:
		{
		iPackagingFinished = ETrue;
		delete iBlankTopClientWin1;
	//	delete iBlankTopClientWin2;
		TInt jj;
		for (jj=0;jj<4;++jj)
			iAnims[jj]->Stop();
		for (jj=0;jj<4;++jj)
			delete iAnims[jj];
		break;
		}
	case 14:
		{
		iPackagingFinished = ETrue;
		iAnims[0]->Stop();
		delete iAnims[0];
		if (iNumOfModes==iCurrentMode)
			{
			ResetScreenSizeMode();
			iCallBackWin->SetVisible(EFalse);
			break;
			}
		break;
		}
	case 18:
	case 17:
		{
		// Stop the animation and delete it, but make sure that you don't increment iState and call Rquest()
		// until this is the second call i.e. one of aId = 17 or aId = 18 has already been finished.
		TInt ii = 0;
		TBool finished = ETrue;
		if (aId == 17)
			{
			iAnims[0]->Stop();
			delete iAnims[0];
			iAnims[0] = NULL;
			for (ii=1;ii<=iNumAnimation;++ii)
				{
				 if (iAnims[ii])
				 	{
				 	finished = EFalse;
				 	break;
				 	}
				}
			}
		else // aId == 18
			{
			for (ii=1;ii<=iNumAnimation;++ii)
				{
				iAnims[ii]->Stop();
				delete iAnims[ii];
				iAnims[ii] = NULL;
				}
			if (iAnims[0])
			 	{
			 	finished = EFalse;
			 	}	
			}
		if (!finished)
			{
			return;
			}
		iPackagingFinished = ETrue;
		break;
		}
	case 19:
		{
		iPackagingFinished = ETrue;
		iAnims[0]->Stop();
		delete iAnims[0];
		break;
		}
	case 21:
	case 20:
		{	
		// Stop the animation and delete it, but make sure that you don't increment iState and call Rquest()
		// until this is the second call i.e. one of aId = 20 or aId = 21 has already been finished.
		TInt index = aId-20;
		iAnims[index]->Stop();
		delete iAnims[index];
		iAnims[index]=NULL;
		// This test creates two animations iAnims[0] and iAnims[1].
		// Use (index ^ 1 ) = Toggle the Index, to get the index of other animation.
		// If iAnims [ index ^ 1] is NULL then this is the second Call and test is finished.
		if (iAnims[index ^ 1] != NULL)
			{
			return;
			}
		DeleteMoveWindow();
		iPackagingFinished = ETrue;
		goto CHECKHANDLES;
		}		//unreachable line, no need for break
	case 24:
		iPackagingFinished = ETrue;
		iAnim->Stop();
		delete iAnim;
		iAnim=NULL;
		//don't want to increase state, so decrement, since it is incremented below
		//as part of processing normal completions.  This is to reenter the test
		//to call Fail() if required
		iState--;
		break;
	
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
		iAnims[aId-27]->Stop();
		delete iAnims[aId-27];
		iAnims[aId-27] = NULL;
		if(aId != 32)
			{
			return;
			}
		DeleteMoveWindow();
		iPackagingFinished = ETrue;
		break;
	default:
		iPackagingFinished = ETrue;
		TInt ii;
		for (ii=0;ii<6;++ii)
			iAnims[ii]->Stop();
		for (ii=0;ii<6;++ii)
			delete iAnims[ii];
		DeleteMoveWindow();
		break;
		}
#if defined(LOGGING)
	_LIT(KRequest,"Signal Start NextTest  Id=%d, State=%d(+1)");
	LogMessageText.Zero();
	LogMessageText.AppendFormat(KRequest,aId,iState);
	TheClient->iWs.LogMessage(LogMessageText);
	TheClient->Flush();
#endif
	iState++;
	}

void CTDirect::DeleteMoveWindow()
	{
	if (iMoveWin)
		{
		TRequestStatus status;
		iMoveWin->Logon(status);
		if (iMoveWin->StillAlive())
			{
			iMoveWin->Terminate(KErrNone);
			User::WaitForRequest(status);
			}
		delete iMoveWin;
		iMoveWin=NULL;
		}
	}

void CTDirect::DeleteScroll()
	{
	if (iScroll)
		{
		TheClient->iScreen->SetScreenMode(0);
		iScroll->Stop();
		delete iScroll;
		iScroll=NULL;
		}
	}

TDisplayMode CTDirect::DisplayMode(TInt aId)
	{
	switch (aId)
		{
	case 1:
	case 13:
	case 15:
	case 16:
		return EColor16;
	case 2:
		return EGray16;
	case 3:
		return EColor16;
	case 4:
	case 14:
	case 17:
	case 18:
	case 19:
	case 24:
		return EColor256;
	case 5:
		return EGray256;
	case 6:
		return EColor4K;
	case 7:
		return EColor64K;
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		{
		TInt colors,grays;
		/*TDisplayMode mode=*/TheClient->iWs.GetDefModeMaxNumColors(colors,grays);
		if (colors==0)
			return EGray4;
		return (aId==8? EGray16:EColor16);
		}
	default:;
		return TheClient->iScreen->DisplayMode();
		}
	}

void CTDirect::Log(const TText8* aFile, TInt aLine, TInt aSeverity,const TDesC& aString)
	{
	Logger().LogExtra(((TText8*)aFile), aLine, aSeverity, aString) ;
	}

TRgb CTDirect::BrushColorL(TInt aId,TInt& aColor,TBool& aFinished)
	{
	aFinished=EFalse;
	switch (aId)
		{
	case 1:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 27:
		aFinished=(aColor==20);
		return TRgb::Color16(27 + aColor++);
	case 28:
		aFinished=(aColor==17);
		return TRgb::Color16(28 + aColor++);
	case 29:
		aFinished=(aColor==15);
		return TRgb::Color16(29 + aColor++);
	case 8:
		if (aColor==15)
			{
			++iCycles;
			aFinished=(iCycles==5);
			}
		else if (!iScroll)
			aFinished=ETrue;
		else if (!iScroll->IsRunning())
			iScroll->ContinueL();
		{
	#if defined(LOGGING)
		if (aFinished || iScroll==NULL)
			{
			_LIT(KColor,"Col=%d Cycles=%d Fin=%d  iScroll=%d");
			LogMessageText.Zero();
			LogMessageText.AppendFormat(KColor,aColor,iCycles,aFinished,(TInt&)iScroll);
			TheClient->iWs.LogMessage(LogMessageText);
			TheClient->Flush();
			}
	#endif
		}
	case 2:
		if (aColor==16)
			aColor=0;
		return TRgb::Gray16(aColor++);
	case 9:
	case 10:
		aFinished=iCycles>3;
	case 11:
	case 3:
		if (aColor==16)
			{
			++iCycles;
			aColor=0;
			}
		return TRgb::Color16(aColor++);
	case 4:
		aFinished=(aColor==256);
		return TRgb::Color256(aColor++);
	case 5:
		if (aColor==256)
			aColor=0;
		return TRgb::Gray256(aColor++);
	case 6:
		aColor+=127;
		if (aColor>4095)
			aColor-=4096;
		return TRgb::Color4K(aColor);
	case 7:
		aColor+=211;
		if (aColor>65535)
			aColor-=65536;
		return TRgb::Color64K(aColor++);
	case 12:
		aFinished=2*(aColor>5);
		return TRgb::Color16(aColor++);
	case 24:
		iCycles++;
		if (iCycles==5)
			{
			aFinished=ETrue;
			CheckForTemporaryDeadlock();
			}
		return TRgb::Color16(aColor++);
	case 30:
		aFinished=(aColor==256);
		return TRgb::Color16(aColor++);
	case 31:
		aFinished=(aColor==100);
		return TRgb::Color16(aColor++);
	case 32:
		aFinished=(aColor==300);
		return TRgb::Color16(aColor++);
	default:;
		aFinished=ETrue;
		return TRgb::Gray2(1);		//White
		}
	}

TInt CTDirect::TimerInterval(TInt aId)
	{
	switch (aId)
		{
	case 1:
	case 15:
	case 16:
	case 17:
	case 18:
	case 20:
	case 21:
		return 143200;
	case 27:	
		return 200000;
	case 28:
		return 180000;
	case 29:
		return 170000;
	case 30:
		return 200000;
	case 31:
		return 205000;
	case 32:
		return 300000;
	case 2:		
	case 25:
	case 26:
		return 234567;
	case 3:
		return 200000;
	case 4:
		return 11718;
	case 5:
		return 13719;
	case 6:
		return 14719;
	case 7:
		return 15719;
	case 8:
		return 275000;
	case 9:
		return 210000;
	case 10:
		return 110000;
	case 11:
		return 123456;
	case 12:
		return 10627;
	case 19:
		return 1000000;
	default:;
		return 1;
		}
	}

void CTDirect::FailedReStart(TInt /*aId*/,TInt /*aReason*/)
	{
	Fail();
	}

TInt CTDirect::SlowStopping(TInt aId,TInt aCount)
	{
	if (aId==8)
		return (aCount>1 ? CColorAnimation::eAbortAll : CColorAnimation::eStopNow);
	if (aId==9 || aId==10)
		return CColorAnimation::eStopNow;
	TInt ret=(2*aId-3==aCount%12);
	if (ret && aId==5)
		{
		++iCount;
		if (iCount==5)
			return CColorAnimation::eAbort;
		}
	return ret;
	}

void CTDirect::ModeSwitch()
	{
	TRAPD(err,iAnim->ChangeModeL(EGray4));
	iWin2->SetSize(TSize(20,25));
	TheClient->Flush();
	if (err!=KErrNone)
		Fail();
	}

TestState CTDirect::AnimateWindowL()
	{
//	if (!iTimer->IsActive())
	{
		
	
	iFrameNo=0;
	SignalWindow();
	
	
		iTimer->Start(0,200000,TCallBack(NextFrame,this));		//0.2 secs

	StartDirect();
	TEST(iDrawingRegion->BoundingRect()==iScrSize);
	}
//	else
//	{
//		int i=0;
//	}
	return EWait;
	}

TestState CTDirect::AnimationDiesL()
	{
	INFO_PRINTF1(_L("AUTO  Animation Dies "));
	_LIT(ThreadName,"AnimationDie");
	iFirstFunction=TThreadStartUp(CAnimating::StartLC,(TAny*)iTest->iScreenNumber);
	CProcess* thread=CProcess::NewThreadL(ThreadName,&iFirstFunction);
	CleanupStack::PushL(thread);
	thread->LeaveIfDied();
	INFO_PRINTF1(_L(" Constructed Thread"));
	iControlSem.Wait();
	RWindowGroup group(TheClient->iWs);
	group.Construct(431,EFalse);
	RBlankWindow win(TheClient->iWs);
	win.Construct(group,432);
	win.SetExtent(TPoint(12,34),TSize(56,78));
	win.Activate();
	INFO_PRINTF1(_L(" Constructed Windows"));
	iControlSem.Wait();
	User::After(1000000);		//1 sec
	TRequestStatus threadDied;
	thread->Logon(threadDied);
	thread->Terminate(KErrGeneral);
	INFO_PRINTF1(_L(" Waiting for Thread"));
	User::WaitForRequest(threadDied);
	CleanupStack::PopAndDestroy(thread);
	win.Close();
	group.Close();
	INFO_PRINTF1(_L(" Finished"));
	return ENext;
	}

TestState CTDirect::PackagingClassL()
	{
	ConstrucBlankWindowL();
	TInt numProcessHandles;
	RThread().HandleCount(numProcessHandles,iNumThreadHandles);
	iAnim=CColorAnimation::NewL(iTest->iScreenNumber,1,*this,*TheClient->iGroup,TRect(10,10,630,230),ETrue);
	return EWait;
	}

TestState CTDirect::MultipleL()
	{
	TSize scrSize=TheClient->iScreen->SizeInPixels();
	
	iFlags|=eMultiAnim;
	iAnims[0]=CColorAnimation::NewL(iTest->iScreenNumber,2,*this,*TheClient->iGroup,
									TRect(0,10,scrSize.iWidth/3+20,scrSize.iHeight/2-5),EFalse);
	iAnims[1]=CColorAnimation::NewL(iTest->iScreenNumber,3,*this,*TheClient->iGroup,
									TRect(10,scrSize.iHeight/2-20,scrSize.iWidth/3-5,scrSize.iHeight),EFalse);
	iAnims[3]=CColorAnimation::NewL(iTest->iScreenNumber,5,*this,*TheClient->iGroup,
									TRect(scrSize.iWidth/3-20,scrSize.iHeight/2+5,2*scrSize.iWidth/3+20,scrSize.iHeight-10),EFalse);
	iAnims[2]=CColorAnimation::NewL(iTest->iScreenNumber,4,*this,*TheClient->iGroup,
									TRect(scrSize.iWidth/3+5,0,2*scrSize.iWidth/3-5,scrSize.iHeight/2+20),EFalse);
	iAnims[4]=CColorAnimation::NewL(iTest->iScreenNumber,6,*this,*TheClient->iGroup,
									TRect(2*scrSize.iWidth/3-20,15,scrSize.iWidth,scrSize.iHeight/2+50),EFalse);
	iAnims[5]=CColorAnimation::NewL(iTest->iScreenNumber,7,*this,*TheClient->iGroup,
									TRect(2*scrSize.iWidth/3+5,-20,scrSize.iWidth-15,scrSize.iHeight+10),ETrue);
	TInt ii;
	for (ii=0;ii<5;++ii)
		iAnims[ii]->StartL();
	_LIT(ThreadName,"MoveWin");
	
	MoveInterval=100000;		//0.1 secs
	ModeInterval=1200000;		//1.2 sec
	ImmediateModeSwitch=EFalse;
	iFirstFunction=TThreadStartUp(CMoveWindow::StartLC,(TAny*)iTest->iScreenNumber);
	iMoveWin=CProcess::NewThreadL(ThreadName,&iFirstFunction);
	return EWait;
	}

TestState CTDirect::MixDsaAndRegionTrackingOnlyL(TBool aWhoExitsLast)
	{
	TSize scrSize=TheClient->iScreen->SizeInPixels();
	
	iFlags|=eMultiAnim;
	TUint firstRegionTrackinOnlyDsa = (aWhoExitsLast == KRegionTrackingOnlyDsaExistLast ? 0 : 3);
	TUint firstDrawingDsa = (aWhoExitsLast == KDrawingDsaExistLast ? 0 : 3);
	
	//DSAs who use region tracking only
	iAnims[firstRegionTrackinOnlyDsa]=CColorAnimation::NewL(iTest->iScreenNumber,firstRegionTrackinOnlyDsa+27,*this,*TheClient->iGroup,
											TRect(10,scrSize.iHeight/2-20,scrSize.iWidth/3-5,scrSize.iHeight),EFalse,KRegionTrackingOnly);
	iAnims[firstRegionTrackinOnlyDsa+1]=CColorAnimation::NewL(iTest->iScreenNumber,firstRegionTrackinOnlyDsa+28,*this,*TheClient->iGroup,
											TRect(2*scrSize.iWidth/3-20,15,scrSize.iWidth,scrSize.iHeight/2+50),EFalse,KRegionTrackingOnly);
	iAnims[firstRegionTrackinOnlyDsa+2]=CColorAnimation::NewL(iTest->iScreenNumber,firstRegionTrackinOnlyDsa+29,*this,*TheClient->iGroup,
											TRect(2*scrSize.iWidth/3+5,-20,scrSize.iWidth-15,scrSize.iHeight+10),EFalse,KRegionTrackingOnly);
	
	//DSAs who actually draw
	iAnims[firstDrawingDsa]=CColorAnimation::NewL(iTest->iScreenNumber,firstDrawingDsa+27,*this,*TheClient->iGroup,
											TRect(0,10,scrSize.iWidth/3+20,scrSize.iHeight/2-5),EFalse,KDrawingDsa);
	iAnims[firstDrawingDsa+1]=CColorAnimation::NewL(iTest->iScreenNumber,firstDrawingDsa+28,*this,*TheClient->iGroup,
											TRect(scrSize.iWidth/3+5,0,2*scrSize.iWidth/3-5,scrSize.iHeight/2+20),EFalse,KDrawingDsa);
	iAnims[firstDrawingDsa+2]=CColorAnimation::NewL(iTest->iScreenNumber,firstDrawingDsa+29,*this,*TheClient->iGroup,
											TRect(scrSize.iWidth/3-20,scrSize.iHeight/2+5,2*scrSize.iWidth/3+20,scrSize.iHeight-10),EFalse,KDrawingDsa);
	
	TInt ii;
	for (ii=0;ii<6;++ii)
		{
		if(iAnims[ii])
			iAnims[ii]->StartL();
		}
		
	_LIT(ThreadName,"MoveWin");
	
	MoveInterval=100000;		//0.1 secs
	ModeInterval=1200000;		//1.2 secs
	ImmediateModeSwitch=EFalse;
	iFirstFunction=TThreadStartUp(CMoveWindow::StartLC,(TAny*)iTest->iScreenNumber);
	iMoveWin=CProcess::NewThreadL(ThreadName,&iFirstFunction);
	return EWait;
	}

TestState CTDirect::TryDifferentSupportedModesL()
	{
	
	RWsSession session;
	User::LeaveIfError(session.Connect());
	CWsScreenDevice* device = new(ELeave) CWsScreenDevice(session);
	CleanupStack::PushL(device);
	User::LeaveIfError(device->Construct(iTest->iScreenNumber));
	TUint numOfModes=device->NumScreenModes();
	_LIT(KTryingCurrentMode,"Trying Mode = %d");
	for(TUint currentMode =0; currentMode < numOfModes; currentMode++)
		{
		LOG_MESSAGE2(KTryingCurrentMode,currentMode);
		device->SetScreenMode(currentMode);
		TRAPD(err,iAnims[0]=CColorAnimation::NewL(iTest->iScreenNumber,4,*this,*TheClient->iGroup,
										TRect(0,0,50,50),ETrue));
		if(err!=KErrNone)
			{
			TEST(EFalse);
			}
		else
			{
			User::After(2000000);
			iAnims[0]->Stop();
			delete iAnims[0];
			}
		}
	CleanupStack::PopAndDestroy(device);
	session.Close();
	iState++;
	return EWait;
	}


TestState CTDirect::FailCodesL()
	{
 	__UHEAP_MARK;
	TRequestStatus status;
	RRegion region(TRect(0,0,10,10),1);
	RRegion* pRegion=&region;
	RDirectScreenAccess direct(TheClient->iWs);
	User::LeaveIfError(direct.Construct());
	RBlankWindow window(TheClient->iWs);
	User::LeaveIfError(window.Construct(*TheClient->iGroup->WinTreeNode(),1234));
	TInt err=direct.Request(pRegion,status,window);
	TEST(err==KErrNone);
	if (err!=KErrNone)
		INFO_PRINTF3(_L("direct.Request(pRegion,status,window) return value - Expected: %d, Actual: %d"), KErrNone, err);		
	TEST(pRegion!=NULL);
	if (pRegion==NULL)
		INFO_PRINTF3(_L("pRegion!=NULL - Expected: %d, Actual: %d"), 0, 0);		
	pRegion->Destroy();
	direct.Cancel();
	User::WaitForRequest(status);
	window.Activate();
	window.SetOrdinalPosition(-2);
	pRegion=&region;
	err=direct.Request(pRegion,status,window);
	TEST(err==KErrNone);
	if (err!=KErrNone)
		INFO_PRINTF3(_L("direct.Request(pRegion,status,window) return value - Expected: %d, Actual: %d"), KErrNone, err);		
	TEST(pRegion!=NULL);
	if (pRegion==NULL)
		INFO_PRINTF3(_L("pRegion!=NULL - Expected: %d, Actual: %d"), 0, 0);		
	pRegion->Destroy();
	direct.Cancel();
	User::WaitForRequest(status);
#if defined(__WINS__)
	// Loop over the allocations done.
	// There is one allocating of the rectangle list and
	// one of the region holding the list
	for (TInt rate = 1; rate <= 2; rate++)
		{
		window.SetOrdinalPosition(0);
		pRegion=&region;
		TheClient->Flush();
		__UHEAP_FAILNEXT(rate);
		err=direct.Request(pRegion,status,window);
		TEST(err==KErrNoMemory);
		if (err!=KErrNoMemory)
			INFO_PRINTF3(_L("direct.Request(pRegion,status,window) return value - Expected: %d, Actual: %d"), KErrNoMemory, err);		

		TEST(pRegion==NULL);
		if (pRegion!=NULL)
			INFO_PRINTF3(_L("pRegion!=NULL - Expected: %d, Actual: %d"), 0, 0);		
		}
#endif
	window.Close();
	direct.Close();
	region.Close();
	__UHEAP_MARKEND;
	return ENext;
	}

void CTDirect::ScrolingTextL(TInt aId,TRect aWinRect,TBool aStartThread,TInt aScreenMode/*=0*/)
	{
	INFO_PRINTF1(_L("AUTO  Construct ScrolingText "));
	INFO_PRINTF1(_L(" Constructed Window"));
	iCycles=0;
	TInt numProcessHandles;
	RThread().HandleCount(numProcessHandles,iNumThreadHandles);
	iAnim=CColorAnimation::NewL(iTest->iScreenNumber,aId,*this,*TheClient->iGroup,aWinRect,EFalse);
	iScroll=CScrollText::NewL(iTest->iScreenNumber,1,*TheClient->iGroup,5,EFalse);
	INFO_PRINTF1(_L(" Constructed Text Scroller & Animation"));
	iAnim->BringWindowToFront();
	if (aStartThread)
		{
		_LIT(ThreadName,"MoveWin");
		INFO_PRINTF1(_L(" About to constructed Thread"));
		iFirstFunction=TThreadStartUp(CMoveWindow::StartLC,(TAny*)iTest->iScreenNumber);
		__ASSERT_DEBUG(!iMoveWin,AutoPanic(EAutoPanicDirect));
		iMoveWin=CProcess::NewThreadL(ThreadName,&iFirstFunction);
		INFO_PRINTF1(_L(" Constructed Thread"));
		}
	if (aScreenMode>0)
		{
		TInt numScreenModes = TheClient->iScreen->NumScreenModes();
		if (aScreenMode < numScreenModes)
			TheClient->iScreen->SetScreenMode(aScreenMode);
		else
			LOG_MESSAGE(_L("WARNING: Failed to change screen mode"));
			// We have to run the test anyway to avoid things going wrong later
		}
	iAnim->StartOrPanic();
	iScroll->StartL();
	INFO_PRINTF1(_L(" Constructed/Started"));
#if defined(LOGGING)
	_LIT(KStarted,"Finished StartUp ScrollText");
	LogMessageText.Copy(KStarted);
	TheClient->iWs.LogMessage(LogMessageText);
	TheClient->Flush();
#endif
	}

TestState CTDirect::ScrolingText1L()
	{
	MoveInterval=0;
	ModeInterval=1200000;		//1.2 sec
	ScrolingTextL(8,TRect(20,20,40,40),ETrue);
	return EWait;
	}

TestState CTDirect::RClassL()
	{
#if defined(LOGGING)
	_LIT(KRClass,"Starting Panic test on RClass");
	LogMessageText.Zero();
	LogMessageText.AppendFormat(KRClass);
	TheClient->iWs.LogMessage(LogMessageText);
	TheClient->Flush();
#endif
	TEST(iTest->TestWsPanicL(CPanicDirect::DoTestOnNewScheduler,EWservPanicDirectMisuse,1,(TAny*)iTest->iScreenNumber));
#if defined(LOGGING)
	_LIT(KPanic1,"Done First Panic");
	LogMessageText.Zero();
	LogMessageText.AppendFormat(KPanic1);
	TheClient->iWs.LogMessage(LogMessageText);
	TheClient->Flush();
#endif
#if defined(__WINS__)
#if defined(LOGGING)
	_LIT(KPanic2,"Doing 2 debug panics");
	LogMessageText.Zero();
	LogMessageText.AppendFormat(KPanic2);
	TheClient->iWs.LogMessage(LogMessageText);
	TheClient->Flush();
#endif
	TEST(iTest->TestW32PanicL(CPanicDirect::DoTestOnNewScheduler,EW32PanicDirectMisuse,2,(TAny*)iTest->iScreenNumber));
	TEST(iTest->TestW32PanicL(CPanicDirect::DoTestOnNewScheduler,EW32PanicDirectMisuse,3,(TAny*)iTest->iScreenNumber));
#endif
#if defined(LOGGING)
	_LIT(KPanicTest,"Trying Panic %d");
	LogMessageText.Zero();
	LogMessageText.AppendFormat(KPanicTest,4);
	TheClient->iWs.LogMessage(LogMessageText);
	TheClient->Flush();
#endif
	TEST(iTest->TestWsPanicL(CPanicDirect::DoTestOnNewScheduler,EWservPanicDirectMisuse,4,(TAny*)iTest->iScreenNumber));
	TEST(iTest->TestW32PanicL(CPanicDirect::DoTestOnNewScheduler,EW32PanicDirectMisuse,5,(TAny*)iTest->iScreenNumber));
	return ENext;
	}

TestState CTDirect::ScrolingText2L()
	{
	MoveInterval=81234;		//0.08 sec
	ModeInterval=140123;		//0.14 sec
	ImmediateModeSwitch=2;
	ScrolingTextL(9,TRect(15,25,45,35),ETrue);
	iScroll->SetCountDown(5);
	return EWait;
	}

TestState CTDirect::ScrolingText3L()
	{
	iModeBackup=TheClient->iScreen->ScreenModeEnforcement();
	if (iModeBackup!=ESizeEnforcementNone)
		TheClient->iScreen->SetScreenModeEnforcement(ESizeEnforcementNone);
	MoveInterval=0;
	ModeInterval=0;
	FlipInterval=1200000;		//1.2 secs
	ScrolingTextL(10,TRect(25,30,35,45),ETrue,1);
	iScroll->SetBottomOfTest(80);		//So it can be seen in both screen modes
	return EWait;
	}

TestState CTDirect::ScrolingText4L()
	{
	iModeBackup=TheClient->iScreen->ScreenModeEnforcement();
	if (iModeBackup!=ESizeEnforcementNone)
		TheClient->iScreen->SetScreenModeEnforcement(ESizeEnforcementNone);
	MoveInterval=876543;		//0.88 secs
	ModeInterval=2178900;		//2.18 secs
	FlipInterval=5000000;		//5 secs
	ScrolingTextL(11,TRect(30,40,55,65),ETrue);
	iScroll->SetBottomOfTest(80);		//So it can be seen in both screen modes
	return EWait;
	}

void CTDirect::CreateAnimForScreenModeL(TInt aAnimIndex,CTWinBase& aParent,TRect aRect,TInt aId)
	{
	iAnims[aAnimIndex]=CColorAnimation::NewL(iTest->iScreenNumber,aId,*this,aParent,aRect,EFalse);
	iAnims[aAnimIndex]->StartL();
	iAnims[aAnimIndex]->BringWindowToFront();
	}

void CTDirect::BlankTopClientWindowL(CTBlankWindow& aBlankWindow,TRect aRect)
	{
	aBlankWindow.ConstructExtLD(*TheClient->iGroup,aRect.iTl,aRect.Size());
	User::LeaveIfError(aBlankWindow.BaseWin()->SetRequiredDisplayMode(EColor256));
	aBlankWindow.BaseWin()->SetShadowDisabled(ETrue);
	aBlankWindow.Activate();
	}

static TInt ChangeScreenModeL(TAny* aTest)
	{
	STATIC_CAST(CTDirect*,aTest)->ChangeToNextScreenModeL();
	return KErrNone;
	}

void CTDirect::ChangeToNextScreenModeL()
	{
#if defined(LOGGING)
	_LIT(KChangeMode,"AUTO Screen Mode Pos Test2 CallBack");
	LogMessageText.Copy(KChangeMode);
	TheClient->LogMessage(LogMessageText);
	TheClient->iWs.LogCommand(RWsSession::ELoggingStatusDump);
#endif
	iNumOfModes=TheClient->iScreen->NumScreenModes();
	if (iCurrentMode<iNumOfModes)
		{
		TPixelsAndRotation pixelsAndRotation;
		SetScreenMode(iCurrentMode,pixelsAndRotation);
		TInt oldCurrentMode=iCurrentMode;
		CArrayFixFlat<TInt>* rotations=new(ELeave) CArrayFixFlat<TInt>(1);
		CleanupStack::PushL(rotations);
		User::LeaveIfError(TheClient->iScreen->GetRotationsList(iCurrentMode,rotations));
		TInt count=rotations->Count();
		TInt jj=0;
		if (count>1)
			{
			for (jj=0;jj<count;)
				{
				if ((*rotations)[jj++]==pixelsAndRotation.iRotation)
					{
					break;
					}
				}
			if (jj==count)
				{
				jj=0;
				}
			}
		if (jj==0)
			{
			iCurrentMode++;
			}
		TInt currentRotation=(*rotations)[jj];
		TheClient->iScreen->SetCurrentRotations(oldCurrentMode,REINTERPRET_CAST(CFbsBitGc::TGraphicsOrientation&,currentRotation));
		CleanupStack::PopAndDestroy(rotations);
		TheClient->iScreen->GetScreenModeSizeAndRotation(oldCurrentMode,pixelsAndRotation);
		TInt screenWidth=pixelsAndRotation.iPixelSize.iWidth;
		TInt screenHeight=pixelsAndRotation.iPixelSize.iHeight;
		TPoint screenModeOrigin=TheClient->iScreen->GetScreenModeScaledOrigin(oldCurrentMode);
		TPoint point1(screenModeOrigin.iX,screenModeOrigin.iY+(screenModeOrigin.iY+screenHeight)/2);
		TPoint point2(screenModeOrigin.iX+(screenModeOrigin.iX+screenWidth)/2,screenHeight+screenModeOrigin.iY);
		TRect rect0(point1,point2);
		TRect rect1(TPoint(0,0),rect0.Size());
		CreateAnimForScreenModeL(0,*TheClient->iGroup,rect1,14);
		TPoint pos=iAnims[0]->AbsoluteWindowPosition();
		TEST(pos==TPoint(0,0));
		}
	}

TInt ChangeScreenScaleCallBack(TAny* aTest)
	{
	static_cast<CTDirect*>(aTest)->ChangeScreenScale();
	return KErrNone;
	}

void CTDirect::ChangeScreenScale()
	{
	if (!iNumOfCallBack)
		{
		iCurrentMode=TheClient->iScreen->CurrentScreenMode();
		iModeData=TheClient->iScreen->GetCurrentScreenModeAttributes();
		iModeData.iScreenScale.SetSize(1,1);
		}
	TSizeMode testMode=iModeData;
	if (!iNumOfCallBack)
		{
		testMode.iScreenScale.SetSize(2,2);
		}
	else if (iNumOfCallBack==1)
		{
		testMode.iScreenScale.SetSize(3,2);
		}
	else if (iNumOfCallBack==2)
		{
		testMode.iScreenScale.SetSize(2,3);
		}
	TheClient->iScreen->SetCurrentScreenModeAttributes(testMode);
	TheClient->iScreen->SetAppScreenMode(iCurrentMode);
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	if (iNumOfCallBack==2)
		{
		iScreenModeTimer->Cancel();
		delete iScreenModeTimer;
		iScreenModeTimer=NULL;
		}
	iNumOfCallBack++;
	}

/*
 * Creates a DSA for screen mode 0. After DSA has displayd 2 or 3 frames screen mode
 * scale is changed with a timer. The DSA aborts and restarts once again
 * and completes itself in different screen mode.
 */
TestState CTDirect::ScreenModeTestForScalingL()
	{
	CreateAnimForScreenModeL(0,*TheClient->iGroup,TRect(TSize(10,10)),19);
	iScreenModeTimer=CPeriodic::NewL(0);
	MoveInterval=2000000;
	iScreenModeTimer->Start(1000,MoveInterval,TCallBack(ChangeScreenScaleCallBack,this));
	return EWait;
	}

/*
 * Creates a DSA for screen mode 0, tests API AbsoluteWindowPosition()
 * Then sets screen mode to last(test) screen mode, here it does the same thing as
 * done for screenmode 0, but with diffrerent scale (2,2) (2,3) (3,2) (3,3) and
 * with different origin (20,30) (30,20) (20,20).
 * Lastly copy back the test screen mode values.
 */
TestState CTDirect::ScreenModeScalingTestL()
	{
#if defined(LOGGING)
INFO_PRINTF1("AUTO  ScreenModeScalingTest ");
#else
//	DisabledStartLogText();
#endif
INFO_PRINTF1(_L(" Switch to mode 0"));
	iModeData.iScreenScale.iWidth=1;
	iModeData.iScreenScale.iHeight=1;
	TheClient->iScreen->SetCurrentScreenModeAttributes(iModeData);
	TheClient->iScreen->SetAppScreenMode(0);
	TheClient->iScreen->SetScreenMode(0);
	iCurrentMode=0;
INFO_PRINTF1(_L(" Get Parameters"));
	iCurrentScreenModeOrigin=TheClient->iScreen->GetDefaultScreenModeOrigin();
	iCurrentScreenModeScale=TheClient->iScreen->GetCurrentScreenModeScale();
	Copy2ndHalfOfScreen=(iCurrentScreenModeOrigin.iX>FullScreenModeSize.iWidth/2? 1 : 0);
	TRect testWinRect(PhysicalToLogical(TPoint(),iCurrentScreenModeScale),
					  PhysicalToLogical(TPoint((Copy2ndHalfOfScreen ? FullScreenModeSize.iWidth :
	                                                                  FullScreenModeSize.iWidth/2),
										       FullScreenModeSize.iHeight)-
										iCurrentScreenModeOrigin,iCurrentScreenModeScale)
					 );
	testWinRect.Shrink(10,10);
INFO_PRINTF1(_L(" Create Animation"));
	CreateAnimForScreenModeL(0,*TheClient->iGroup,testWinRect,17);
	TPoint pos0=iAnims[0]->AbsoluteWindowPosition();
	TEST(pos0==TPoint(10,10));

	TInt numOfModes=TheClient->iScreen->NumScreenModes();
	iCurrentMode=numOfModes-1;
INFO_PRINTF1(_L(" Switch to Last Mode"));
	TheClient->iScreen->SetAppScreenMode(iCurrentMode);
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	TSizeMode storeModeData=TheClient->iScreen->GetCurrentScreenModeAttributes();
	TSizeMode testMode=storeModeData;
	iNumAnimation=0;
	for (TInt xScale=2;xScale<4;xScale++)
		{
INFO_PRINTF1(_L(" New X-Scale"));
		for (TInt yScale=2;yScale<4;yScale++)
			{
INFO_PRINTF1(_L(" New Y-Scale"));
			testMode.iScreenScale=TSize(xScale,yScale);
			TestDifferentOriginAndScaleL(testMode,TPoint(20,20));
			TestDifferentOriginAndScaleL(testMode,TPoint(20,30));
			TestDifferentOriginAndScaleL(testMode,TPoint(30,20));
			}
		}
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	TheClient->iScreen->SetCurrentScreenModeAttributes(storeModeData);
INFO_PRINTF1(_L(" Set To Mode 0"));
	TheClient->iScreen->SetAppScreenMode(0);
	TheClient->iScreen->SetScreenMode(0);
	return EWait;
	}

void CTDirect::TestDifferentOriginAndScaleL(TSizeMode &aMode,TPoint aOrigin)
	{
	aMode.iOrigin=aOrigin;
	TheClient->iScreen->SetCurrentScreenModeAttributes(aMode);
	TheClient->iScreen->SetAppScreenMode(iCurrentMode);
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	iCurrentScreenModeOrigin=TheClient->iScreen->GetScreenModeOrigin(iCurrentMode);
	iCurrentScreenModeScale=TheClient->iScreen->GetScreenModeScale(iCurrentMode);
	Copy2ndHalfOfScreen=(iCurrentScreenModeOrigin.iX>FullScreenModeSize.iWidth/2? 1 : 0);
	TRect testWinRect(PhysicalToLogical(TPoint(),iCurrentScreenModeScale),
					  PhysicalToLogical(TPoint((Copy2ndHalfOfScreen ? FullScreenModeSize.iWidth
																	: FullScreenModeSize.iWidth/2),
											   FullScreenModeSize.iHeight)
										-iCurrentScreenModeOrigin,iCurrentScreenModeScale)
					 );
	testWinRect.Shrink(10,10);
	++iNumAnimation;
	CreateAnimForScreenModeL(iNumAnimation,*TheClient->iGroup,testWinRect,18);
	TPoint pos0=iAnims[iNumAnimation]->AbsoluteWindowPosition();
	TEST(pos0==TPoint(10,10));
	}

//REQUIREMENT: CR PHAR-5SJGAM, PREQ673
//Tests that DSA works correctly in screen modes with non-zero screen mode origin.
TestState CTDirect::DSAWithScreenModeOffset1L()
	{
	TPoint screenModeTwoOrigin=TheClient->iScreen->GetScreenModeScaledOrigin(2);
	TPixelsAndRotation pixelsAndRotation;
	TheClient->iScreen->GetScreenModeSizeAndRotation(2,pixelsAndRotation);
	if(pixelsAndRotation.iRotation != CFbsBitGc::EGraphicsOrientationNormal)
		{
		TheClient->iScreen->SetCurrentRotations(2, CFbsBitGc::EGraphicsOrientationNormal);
		}
	TheClient->iScreen->GetScreenModeSizeAndRotation(2,pixelsAndRotation);
	//check that the current rotation is normal
	__ASSERT_DEBUG(pixelsAndRotation.iRotation==CFbsBitGc::EGraphicsOrientationNormal,AutoPanic(EAutoPanicDirect));
	TInt screenWidthMode2=pixelsAndRotation.iPixelSize.iWidth;
	TInt screenHeightMode2=pixelsAndRotation.iPixelSize.iHeight;
	TPoint point1(screenModeTwoOrigin.iX,screenModeTwoOrigin.iY+(screenModeTwoOrigin.iY+screenHeightMode2)/2);
	TPoint point2(screenModeTwoOrigin.iX+(screenModeTwoOrigin.iX+screenWidthMode2)/2,screenHeightMode2+screenModeTwoOrigin.iY);
	TPoint point3(screenModeTwoOrigin.iX+screenWidthMode2,screenModeTwoOrigin.iY+screenHeightMode2);

	SetScreenMode(0,pixelsAndRotation);
	TRect rect0(point1,point2);
	CreateAnimForScreenModeL(0,*TheClient->iGroup,rect0,13);
	TPoint pos0=iAnims[0]->AbsoluteWindowPosition();
	TEST(pos0==point1);

	SetScreenMode(2,pixelsAndRotation);
	TRect rect1(TPoint(0,0),rect0.Size());
	CreateAnimForScreenModeL(1,*TheClient->iGroup,rect1,13);
	TPoint pos1=iAnims[1]->AbsoluteWindowPosition();
	TEST(pos1==rect1.iTl);

	SetScreenMode(0,pixelsAndRotation);
	iBlankTopClientWin1=new(ELeave) CTBlankWindow();
//	TInt ordpos = iBlankTopClientWin1->BaseWin()->OrdinalPosition();
	TRect rect2(TPoint(point2.iX,point1.iY),point3);
	BlankTopClientWindowL(*iBlankTopClientWin1,rect2);
	TInt x=rect2.Size().iWidth/4;
	TPoint animWinPt(x,0);
	rect2.Shrink(x,0);
	CreateAnimForScreenModeL(2,*iBlankTopClientWin1,TRect(animWinPt,rect2.Size()),13);
	TPoint pos2=iAnims[2]->AbsoluteWindowPosition();
	TEST(pos2==rect2.iTl);

	SetScreenMode(2,pixelsAndRotation);
	iBlankTopClientWin2=new(ELeave) CTBlankWindow();
//	ordpos = iBlankTopClientWin2->BaseWin()->OrdinalPosition();
	TPoint tl(rect1.iBr.iX,0);
	TRect rect3(tl,TPoint(screenWidthMode2,rect1.iBr.iY));
	BlankTopClientWindowL(*iBlankTopClientWin2,rect3);
	x=rect3.Size().iWidth/4;
	animWinPt=TPoint(x,0);
	rect3.Shrink(x,0);
	CreateAnimForScreenModeL(3,*iBlankTopClientWin2,TRect(animWinPt,rect3.Size()),13);
	TPoint pos3=iAnims[3]->AbsoluteWindowPosition();
	TEST(pos3==(animWinPt+tl));

	SetScreenMode(0,pixelsAndRotation);
	return EWait;
	}

//REQUIREMENT: CR PHAR-5SJGAM, PREQ673
//Tests that DSA works correctly in screen modes with non-zero screen mode origin and different rotations
TestState CTDirect::DSAWithScreenModeOffset2L()
	{
	iCurrentMode=0;
	TTimeIntervalMicroSeconds32 timeBetweenScreenModeChange=3200000;
	iCallBackWin->WinTreeNode()->SetOrdinalPosition(0);
	iCallBackWin->SetVisible(ETrue);		//Used to forsce screen into Color256 so that it will rotate
	iChangeScreenModeTimer=CPeriodic::NewL(0);
	iChangeScreenModeTimer->Start(0,timeBetweenScreenModeChange,TCallBack(ChangeScreenModeL,this));
	return EWait;
	}

void CTDirect::SetScreenMode(TInt aMode,TPixelsAndRotation& aPixelsAndRotation)
	{
	TheClient->iScreen->SetScreenMode(aMode);
	TheClient->iScreen->SetScreenModeEnforcement(ESizeEnforcementNone);
	TheClient->iScreen->GetDefaultScreenSizeAndRotation(aPixelsAndRotation);
	TheClient->iScreen->SetScreenSizeAndRotation(aPixelsAndRotation);
	TheClient->Flush();
	}

TestState CTDirect::DefectFix_KAA_5J3BLW_L()
	{
	TInt numProcessHandles;
	RThread().HandleCount(numProcessHandles,iNumThreadHandles);
	const TSize screenSize(TheClient->iScreen->SizeInPixels());
	const TRect dsaRect(0,0,screenSize.iWidth>>2,screenSize.iHeight>>2);
	iAnim=CBugFixColorAnimation::NewL(iTest->iScreenNumber, 1, *this, *TheClient->iGroup, dsaRect,ETrue);
	return EWait;
	}

TestState CTDirect::RegionTrackingOnlyNotificationsL(TUint aId)
	{
	TInt numProcessHandles;
	RThread().HandleCount(numProcessHandles,iNumThreadHandles);
	const TSize screenSize(TheClient->iScreen->SizeInPixels());
	const TRect dsaRect(0,0,screenSize.iWidth>>2,screenSize.iHeight>>2);
	TBool isWindowOpenedInFrontOfDsa = (aId == KRegionTrackingOnlyDsaWaitingForAbortSignal);
	iAnim=CRegionTrackingOnly::NewL(iTest->iScreenNumber, aId, *this, *TheClient->iGroup, dsaRect,ETrue,isWindowOpenedInFrontOfDsa);
	return EWait;
	}

// Tests the new function of getting the window's absolute position
TestState CTDirect::WindowPoistionRelativeToScreenL()
	{
	//.. delete screen mode timer
	delete iChangeScreenModeTimer;
	iChangeScreenModeTimer=NULL;

	TInt numProcessHandles;
	RThread().HandleCount(numProcessHandles,iNumThreadHandles);
	TSize screenSize(TheClient->iScreen->SizeInPixels());
	TRect rect(0,0,screenSize.iWidth>>1,screenSize.iHeight);
	rect.Shrink(10,10);
	// First animation is for showing that child window is within the visible part of the parent window and within the visible screen area
	iAnims[0]=new(ELeave) CColorAnimation(iTest->iScreenNumber,15,*this);
	iAnims[0]->ConstructL(*TheClient->iGroup,rect,KDrawingDsa,1);
	iAnims[0]->StartL(ETrue);
	// First animation is for showing that child window is to the side of visible part of parent window
	rect.Move(screenSize.iWidth>>1,0);
	iAnims[1]=new(ELeave) CColorAnimation(iTest->iScreenNumber,16,*this);
	iAnims[1]->ConstructL(*TheClient->iGroup,rect,KDrawingDsa,2);
	iAnims[1]->StartL(ETrue);
	return EWait;
	}

TestState CTDirect::MultipleDSAsOnSameWindowL()
	{
	TInt numProcessHandles;
	RThread().HandleCount(numProcessHandles,iNumThreadHandles);
	iCallBackWin->SetVisible(ETrue);
	iCallBackWin->WinTreeNode()->SetOrdinalPosition(0);
	iAnims[0]=new(ELeave) CColorAnimation(iTest->iScreenNumber,20,*this);
	iAnims[0]->ConstructL(*TheClient->iGroup,TRect(),KDrawingDsa,0,1);
	iAnims[0]->StartL();
	iAnims[1]=new(ELeave) CColorAnimation(iTest->iScreenNumber,21,*this);
	iAnims[1]->ConstructL(*TheClient->iGroup,TRect(),KDrawingDsa,0,2);
	iAnims[1]->StartL();
	_LIT(ThreadName,"MoveWin");
	MoveInterval=100000;
	ModeInterval=0;
	FlipInterval=0;
	ImmediateModeSwitch=EFalse;
	iFirstFunction=TThreadStartUp(CMoveWindow::StartLC,(TAny*)iTest->iScreenNumber);
	iMoveWin=CProcess::NewThreadL(ThreadName,&iFirstFunction);
	return EWait;
	}

TestState CTDirect::KillAnimationL()
	{
	TInt numProcessHandles;
	RThread().HandleCount(numProcessHandles,iNumThreadHandles);
	iAnim=CColorAnimation::NewL(iTest->iScreenNumber,12,*this,*TheClient->iGroup,TRect(15,15,625,225),ETrue);
	return EWait;
	}

TestState CTDirect::TemporaryDeadlockL()
	{

	if (iTestJustCompleted)
		{
			if (iTestJustFailed)
			{
			Fail();
			}
		iState++;
		return ENext;
		}

	//make sure this code isn't called a second time
	__ASSERT_ALWAYS(iAnim==NULL,AutoPanic(EAutoPanicDirect));

	TSize screenSize(TheClient->iScreen->SizeInPixels());
	TRect rect(0,0,screenSize.iWidth>>1,screenSize.iHeight);
	rect.Shrink(10,10);
	iAnim=new(ELeave) CColorAnimation(iTest->iScreenNumber,24,*this);
	iAnim->ConstructL(*TheClient->iGroup,rect,KDrawingDsa,1);
	iAnim->StartL(ETrue);
	return EWait;
	}

void CTDirect::CheckForTemporaryDeadlock()
	{

	TBool result=ETrue;
	TInt error;


	//Create a window for placing on top
	TTime beforeTime;
	beforeTime.HomeTime();

	RWindow window(TheClient->iWs);

	error=window.Construct(*TheClient->iGroup->GroupWin(), reinterpret_cast<TInt>(&window));
	if (error==KErrNone)
		{
		window.SetOrdinalPosition(0);
		window.SetExtent(TPoint(30,30),TSize(10,10));
		window.SetBackgroundColor(TRgb(255,0,255));

		//make sure the basewin is towards the back
		iCallBackWin->BaseWin()->SetOrdinalPosition(5);

		window.SetRequiredDisplayMode(EColor256);
		window.Activate();
		TheClient->iWs.Flush();
		//need code similar to below, but the status of the active object we
		//really want is too private
		//if (!iAnim->IsReadyToAbort())
		//	{
		//	result=EFalse;
		//	}
		window.Close();
		}
	else
		{
		result = EFalse;
		}

	TTime afterTime;
	afterTime.HomeTime();
	TTimeIntervalMicroSeconds difference = afterTime.MicroSecondsFrom(beforeTime);

	//make time difference 350ms, since the two timers to be checked are 400ms and 500ms
	if (difference>TTimeIntervalMicroSeconds(1000*350))
		{
		result=EFalse;
		}

	iTestJustCompleted = ETrue;
	if (result==EFalse)
		{
		iTestJustFailed=ETrue;
		}
	}

void CTDirect::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(Animation1,"Animating");
	_LIT(Animation2,"Animating Dies");
	_LIT(Animation3,"Packaging Class");
	_LIT(Animation4,"Many Animations");
	_LIT(Animation5,"Fail Codes");
	_LIT(Animation6,"Cancel The Other");
	_LIT(Animation7,"'R' Class API");
	_LIT(Animation8,"Switch Clear Type");
	_LIT(Animation9,"SizeMode Change");
	_LIT(Animation10,"Soak Testing");
	_LIT(Animation11,"Kill Animation");
	_LIT(Animation12,"Defect-Fix: KAA-5J3BLW");
	_LIT(Animation13,"Screen Mode Positioning DSA Test 1");
	_LIT(Animation14,"Screen Mode Positioning DSA Test 2");
	_LIT(Animation15,"Position Relative to Screen");
	_LIT(Animation16,"Screen mode Scaling DSA Test 1");
	_LIT(Animation17,"Screen mode Scaling DSA Test 2");
	_LIT(Animation18,"Multiple DSAs on same window");
	_LIT(Animation19,"DSA and windows temporary deadlock");
	_LIT(Animation25,"RegionTrackingOnly DSA, window opened in front");
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	_LIT(Animation26,"RegionTrackingOnly DSA, window opened behind");
	_LIT(Animation27,"Mixed DSAs, RegionTrackingOnly DSA last to exit");
	_LIT(Animation28,"Mixed DSAs, drawing DSA last to exit");
	_LIT(Animation29,"Trying all the screen supported modes");
#endif
	TestState ret=ENext;

	if (iTimerRunning && !iPackagingFinished)
		{
		// Prevent test harness from repeatedly running the test case too quickly.
		User::After(SHORT_DELAY);
		}

	//if (iState==0) iState=18;
	iTest->iState=iState;
	((CTDirectStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0158

@SYMDEF             DEF081259

@SYMTestCaseDesc    Create seven seperate animations and run them to completion

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create animations and start them running. Run until the animations finish.

@SYMTestExpectedResults The animation run to completion without error
*/
	case 0:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0158"));
		if (iNextFrameFinished)
			InitialiseAnimationL();
	case 1:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0158"));
	case 2:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0158"));
	case 3:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0158"));
	case 4:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0158"));
	case 5:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0158"));
	case 6:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0158"));
		if (iNextFrameFinished)
			{
			iTest->LogSubTest(Animation1);
			ret=AnimateWindowL();
			iNextFrameFinished=EFalse;
			}
		else
			{
			// Prevent test harness from repeatedly running the test case too quickly.
			User::After(SHORT_DELAY);
			}
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0159

@SYMDEF             DEF081259

@SYMTestCaseDesc    Check animation dies correctly when run in a thread

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create animation and run in from a thread that dies. Once the thread
					has died check the animation has been dealt with correctly.

@SYMTestExpectedResults The animation dies correctly
*/
	case 7:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0159"));
		iTest->LogSubTest(Animation2);
		ret=AnimationDiesL();
		++iState;
		DestroyAnimation();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0160

@SYMDEF             DEF081259

@SYMTestCaseDesc    Check animation runs correctly in blank window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create an animation and run it in a blank window

@SYMTestExpectedResults The animation runs to completion without error
*/
	case 8:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0160"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation3);
			ret=PackagingClassL();
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0161

@SYMDEF             DEF081259

@SYMTestCaseDesc    Check many animations can be run in the same window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create multiple animations in a window and run them all
					until completion

@SYMTestExpectedResults The animations run without error
*/
	case 9:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0161"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation4);
			ret=MultipleL();
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0162

@SYMDEF             DEF081259

@SYMTestCaseDesc    Direct screen access out of memory test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Direct screen access out of memory test

@SYMTestExpectedResults The out of memory error is handled correctly
*/
	case 10:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0162"));
		iTest->LogSubTest(Animation5);
		ret=FailCodesL();
		iState++;
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0163

@SYMDEF             DEF081259

@SYMTestCaseDesc    Two animations, one scrolling text

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create two animations, one which scrolls text across the screen and
					run them to completion

@SYMTestExpectedResults The animations run without error
*/
	case 11:		
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0163"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation6);
			ret=ScrolingText1L();
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0164

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test direct screen access panic messages

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Call the direct screen access panic's and check they are handled
					correctly

@SYMTestExpectedResults The panic's are handled correctly
*/
	case 12:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0164"));
		iTest->LogSubTest(Animation7);
		ret=RClassL();
		iTest->CloseAllPanicWindows();
		iState++;
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0165

@SYMDEF             DEF081259

@SYMTestCaseDesc    Two animations, one scrolling text

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create two animations, one which scrolls text across the screen and
					run them to completion

@SYMTestExpectedResults The animations run without error
*/
	case 13:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0165"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation8);
			ret=ScrolingText2L();
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0166

@SYMDEF             DEF081259

@SYMTestCaseDesc    Two animations, one scrolling text. Change the screen mode an run.

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create two animations, one which scrolls text across the screen and
					run them to completion while changing the screen mode

@SYMTestExpectedResults The animations run without error
*/
	
	case 14:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0166"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation9);
			ret=ScrolingText3L();
			if (ret == ENext)
				iPackagingFinished = ETrue;
			else
				iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			FlipInterval = 0; // Stops the tests (erroneously) flipping for the rest of the run
			}
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0167

@SYMDEF             DEF081259

@SYMTestCaseDesc    Start an animation then kill it

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Start an animation running then kill it. Check the animation dies correctly

@SYMTestExpectedResults The animations dies correctly
*/
	case 15:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0167"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation11);
			ret=KillAnimationL();
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
	case 16:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0167"));
		iTest->LogSubTest(Animation10);		//This test is designed to be left running for at least several hours
		//ret=ScrolingText4L();
		iState++;
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0168

@SYMDEF             DEF081259

@SYMTestCaseDesc    CBugFixColorAnimation

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     This class is used for reproducing a defect found on 6.1: KAA-5J3BLW "Unnecessary Wserv's DSA abort".
					The problem was that a direct screen access client was getting an unnecessary abort notification
					when a new window (or window group) was created but not visible.
					This class will simulate the direct screen access client and it will check whether the first DSA abort
					is not caused by just creating a window.

@SYMTestExpectedResults Abort is not caused when creatung a window
*/	
	case 17:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0168"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation12);
			ret=DefectFix_KAA_5J3BLW_L();
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0169

@SYMDEF             DEF081259

@SYMTestCaseDesc    Direct screen access in screen modes with non-zero screen mode origin

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Tests that DSA works correctly in screen modes with non-zero screen mode origin

@SYMTestExpectedResults The DSA works correctly
*/
	case 18:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0169"));
		if (!CheckNonZeroOriginsSupportedOrNot())
			{
			INFO_PRINTF1(_L("Non Zero Origins not supported\n"));
			iState++;
			}
		else
			{
			if (!iTimerRunning)
				{
				iTest->LogSubTest(Animation13);
				ret=DSAWithScreenModeOffset1L();
				iTimerRunning = ETrue;
				}
			if (iPackagingFinished)
				{
				iPackagingFinished = EFalse;
				iTimerRunning = EFalse;
				}
			}
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0170

@SYMDEF             DEF081259

@SYMTestCaseDesc    Direct screen access in screen modes with non-zero screen mode origin

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Tests that DSA works correctly in screen modes with non-zero screen mode origin and different rotations

@SYMTestExpectedResults The DSA works correctly
*/
	case 19:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0170"));
		if (!CheckNonZeroOriginsSupportedOrNot())
			{
			INFO_PRINTF1(_L("Non Zero Origins not supported\n"));
			iState++;
			}
		else
			{
			if (!iTimerRunning)
				{
				iTest->LogSubTest(Animation14);
				ret=DSAWithScreenModeOffset2L();
				iTimerRunning = ETrue;
				}
			if (iPackagingFinished)
				{
				iPackagingFinished = EFalse;
				iTimerRunning = EFalse;
				}
			}
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0171

@SYMDEF             DEF081259

@SYMTestCaseDesc    Window absolute position

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Tests the new function of getting the window's absolute position

@SYMTestExpectedResults Function works correctly
*/
	case 20:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0171"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation15);
			ret=WindowPoistionRelativeToScreenL();
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0172

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test direct screen access restart

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates a DSA for screen mode 0. After DSA has displayed 2 or 3 frames screen mode
 					scale is changed with a timer. The DSA aborts and restarts once again
 					and completes itself in different screen mode.

@SYMTestExpectedResults DSA restarts and completes correctly
*/
	case 21:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0172"));
		if (iIsScalingSupported)
			{
			if (!iTimerRunning)
				{
				iTest->LogSubTest(Animation16);
				ret=ScreenModeTestForScalingL();
				iTimerRunning = ETrue;
				}
			if (iPackagingFinished)
				{
				iPackagingFinished = EFalse;
				iTimerRunning = EFalse;
				}
			break;
			}
		iState++;
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0173

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test direct screen access scaling

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates a DSA for screen mode 0, tests API AbsoluteWindowPosition()
 					Then sets screen mode to last(test) screen mode, here it does the same thing as
 					done for screenmode 0, but with diffrerent scale (2,2) (2,3) (3,2) (3,3) and
 					with different origin (20,30) (30,20) (20,20).
 					Lastly copy back the test screen mode values.

@SYMTestExpectedResults DSA scales correctly
*/
	case 22:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0173"));
		if (iIsScalingSupported)
			{
			if (!iTimerRunning)
				{
				iTest->LogSubTest(Animation17);
				ret=ScreenModeScalingTestL();
				iTimerRunning = ETrue;
				}
			if (iPackagingFinished)
				{
				iPackagingFinished = EFalse;
				iTimerRunning = EFalse;
				}
			break;
			}
		iState++;
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0174

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test multiple direct screen access elements on the same window 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create a number of direct screen access elements on the same window and
					check that they work correctly

@SYMTestExpectedResults DSAs work correctly
*/
	case 23:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0174"));
		if (iIsScalingSupported)
			{
			if (!iTimerRunning)
				{
				iTest->LogSubTest(Animation18);
				ret=MultipleDSAsOnSameWindowL();
				iTimerRunning = ETrue;
				}
			if (iPackagingFinished)
				{
				iPackagingFinished = EFalse;
				iTimerRunning = EFalse;
				}	
			break;
			}
		iState++;
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0175

@SYMDEF             DEF081259

@SYMTestCaseDesc    Create a temporary deadlock on a DSA and resolve it 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Resolve a temporary deadlock on a DSA

@SYMTestExpectedResults DSA resolves the deadlock
*/
	case 24:
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0175"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation19);
			ret=TemporaryDeadlockL();//for INC072887 - removing a 0.5s delay in wserv.
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			iState++;
			}
		break;
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	case 25:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0533
*/
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0533"));
		if (!iTimerRunning)
			{
			FlipInterval=0;
			iTest->LogSubTest(Animation25);
			//Opens a window in front of a region tracking only DSA
			ret=RegionTrackingOnlyNotificationsL(KRegionTrackingOnlyDsaWaitingForAbortSignal);
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
	case 26:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0534
*/
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0534"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation26);
			//Opens a window behind a region tracking only DSA
			ret=RegionTrackingOnlyNotificationsL(KRegionTrackingOnlyDsaNoAbortSignal);
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
	case 27:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0535
*/
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0535"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation27);
			ret=MixDsaAndRegionTrackingOnlyL(KRegionTrackingOnlyDsaExistLast);
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
	case 28:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0536
*/
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0536"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation28);
			ret=MixDsaAndRegionTrackingOnlyL(KDrawingDsaExistLast);
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
	case 29:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0537
*/
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0537"));
		if (!iTimerRunning)
			{
			iTest->LogSubTest(Animation29);
			ret=TryDifferentSupportedModesL();
			iTimerRunning = ETrue;
			}
		if (iPackagingFinished)
			{
			iPackagingFinished = EFalse;
			iTimerRunning = EFalse;
			}
		break;
#else
//NON NGA negative test for RegionTrackingOnly DSA
	case 25:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0575
*/
		((CTDirectStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0575"));
		if (!iTimerRunning)
			{
			FlipInterval=0;
			iTest->LogSubTest(Animation25);
			CColorAnimation* temp = NULL;
			//Attempt to create a RegionTrackingOnly DSA in non NGA code
			TRAPD(err,temp = CColorAnimation::NewL(iTest->iScreenNumber,1,*this,*TheClient->iGroup,TRect(10,10,630,230),ETrue,KRegionTrackingOnly));
			if(err!=KErrNotSupported)
				{
				_LIT(KCTDirectNonNgaError,"Attempt to creat a RegionTrackingOnly DSA did not return KErrNotSupported on non-NGA");
				LOG_MESSAGE(KCTDirectNonNgaError);
				if(temp)
					{
					delete temp;
					}
				Fail();
				}
			else
				{
				_LIT(KCTDirectNonNgaSuccess,"RegionTrackingOnly DSA not supported on non-NGA as expected");
				LOG_MESSAGE(KCTDirectNonNgaSuccess);
				}
			iState++;
			}
		break;
#endif
	default:
		((CTDirectStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTDirectStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTDirectStep*)iStep)->RecordTestResultL();
	}

CRegionTrackingOnly* CRegionTrackingOnly::NewL(TInt aScreenNumber,TInt aId,MAnimCallBacks& aCallBack,CTWindowGroup& aParent,TRect aExtent,TBool aStart,TBool aOpenWindowInFrontDsa)
	{
	CRegionTrackingOnly* self=new(ELeave) CRegionTrackingOnly(aScreenNumber, aId,aCallBack);
	CleanupStack::PushL(self);
	self->ConstructL(aParent,aExtent,aOpenWindowInFrontDsa);
	if (aStart)
		{
		self->StartL();
		self->Started();
		}
	CleanupStack::Pop(self);
	return self;
	}

CRegionTrackingOnly::CRegionTrackingOnly(TInt aScreenNumber,TInt aId,MAnimCallBacks& aCallBack)
	: CColorAnimation(aScreenNumber, aId, aCallBack)
	{
	iThreadParam.iScreenNumber = aScreenNumber;
	}

void CRegionTrackingOnly::ConstructL(CTWindowGroup& aParent,TRect aExtent,TBool aOpenWindowInFrontDsa)
	{
	iExpectedToAbort = aOpenWindowInFrontDsa; 
	CColorAnimation::ConstructL(aParent, aExtent, KRegionTrackingOnly);
	_LIT(ThreadName,"Create new Window");
	
    TInt error=iSem.CreateGlobal(KSem_DefectFix_KAA_5J3BLW_Name, 0);
    if (error==KErrNone)
        {
        iAnimRect=aExtent;
        iThreadParam.iRect = iAnimRect;
        iThreadParam.iIsInFront = aOpenWindowInFrontDsa;
        TThreadStartUp function=TThreadStartUp(CreateNewWindowGroup, &iThreadParam);
        TRequestStatus status;
        iThread=CProcess::NewThreadRendezvousL(ThreadName,&function,status);
        User::WaitForRequest(status);
        if (status != KErrNone)
            {
            RDebug::Printf("the request status is returned to be non KErrNone: %d", status.Int());
            TestFailed(this);
            }
        }
    else
        {
        TestFailed(this);
        }
	}

TBool CColorAnimation::TestGcAndScreenDeviceValues()
	{
	TBool succeeded = ETrue;
	_LIT(KErrorLogGraphicContext,"GraphicsContext not NULL when using the region tracking feature only");
	_LIT(KErrorLogScreenDevice,"ScreenDevice not NULL when using the region tracking feature only");
	if(iRegionTrackingOnly && iDrawer->Gc() != NULL)
		{
		CallBack().Log((TText8*)__FILE__,__LINE__, ESevrErr,KErrorLogGraphicContext);
		succeeded = EFalse;
		}
	if(iRegionTrackingOnly && iDrawer->ScreenDevice() != NULL)
		{
		CallBack().Log((TText8*)__FILE__,__LINE__, ESevrErr,KErrorLogScreenDevice);
		succeeded = EFalse;
		}
	return succeeded;
	}

void CRegionTrackingOnly::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	Stop();
	}

void CRegionTrackingOnly::Restart(RDirectScreenAccess::TTerminationReasons/* aReason*/)
	{
	if(!TestGcAndScreenDeviceValues())
		{
		CallBack().Fail();
		}
	if (iExpectedToAbort)
		{
		_LIT(KExpected,"DSA got an abort signal as expected");
		CallBack().Log((TText8*)__FILE__,__LINE__,ESevrInfo,KExpected);
		}
	else
		{
		_LIT(KError,"DSA got an abort signal even though the window was opened behind it");
		CallBack().Log((TText8*)__FILE__,__LINE__,ESevrErr,KError);
		CallBack().Fail();
		}
	FinishTest();
	}

CRegionTrackingOnly::~CRegionTrackingOnly()
	{
	iSem.Close();
	if(iThread)
		{
		TRequestStatus status;
		iThread->Logon(status);
		if (iThread->StillAlive())
			{
 			iThread->Terminate(KErrNone);
			User::WaitForRequest(status);
			}
		delete iThread;
		}
	}

__WS_CONSTRUCT_STEP__(Direct)
