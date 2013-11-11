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
// General window tests
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TWINDOW.H"

const TInt ENumCornerInsets=6;
const TInt corner0[ENumCornerInsets]={0,0,0,0,0,0};
const TInt corner1[ENumCornerInsets]={1,0,0,0,0,0};
const TInt corner2[ENumCornerInsets]={2,1,0,0,0,0};
const TInt corner3[ENumCornerInsets]={3,1,1,0,0,0};
const TInt corner5[ENumCornerInsets]={5,3,2,1,1,0};
const TInt KBaseUserEvent=61750;		//0xF136 - a random number that is unlikely to be used by other applicaions for user event sending

//

CWinTestWindow::CWinTestWindow(TRgb aCol) : CBlankWindow(aCol)
	{
	}

CWinTestWindow::~CWinTestWindow()
	{
	delete iChild;
	}

void CWinTestWindow::ConstructL(TPoint pos,TSize size,CTWinBase *aParent, CWindowGc &aGc, TInt aDepth)
	{
	iWin=RWindow(TheClient->iWs);
	User::LeaveIfError(iWin.Construct(*(aParent->WinTreeNode()),ENullWsHandle));
	SetExt(pos,size);
	if (aDepth<3)
		{
		iChild=new(ELeave) CWinTestWindow(TRgb::Gray256(iCol.Gray256()+34));
		size.iWidth-=8;
		size.iHeight-=8;
		iChild->ConstructL(TPoint(4,4),size,this,aGc,aDepth+1);
		}
	Activate();
	AssignGC(aGc);
	}

//

CEvWindowGroup* CEvWindowGroup::NewLC(CTClient* aClient,CTWsGraphicsBase* aTest)
	{
	CEvWindowGroup* self=new(ELeave) CEvWindowGroup(aClient,aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CEvWindowGroup::CEvWindowGroup(CTClient* aClient,CTWsGraphicsBase* aTest) : CTWindowGroup(aClient), iTest(aTest)
	{}

void CEvWindowGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	iGroupWin.EnableReceiptOfFocus(EFalse);
	}

void CEvWindowGroup::SetExpectedEvent(TInt aType)
	{
	iTest->TEST(!iExpectingEvent);
	if (iExpectingEvent)
		{
		_LIT(KLog,"Setting expected event of type %d, when previouse event of type %d has not arrived (GpWinId=%d).");
		iTest->LOG_MESSAGE4(KLog,aType,iExpectedEvent,iGroupWin.Identifier());
		}
	iExpectedEvent=aType;
	iExpectingEvent=ETrue;
	}

void CEvWindowGroup::SendEvent(TInt aType)
	{
	TWsEvent event;
	event.SetType(aType);
	iClient->iWs.SendEventToWindowGroup(iGroupWin.Identifier(),event);
	SetExpectedEvent(aType);
	iClient->Flush();
	}

void CEvWindowGroup::UserEvent(TInt aEventType)
	{
	iTest->TEST(iExpectingEvent && iExpectedEvent==aEventType);
	if (!iExpectingEvent || iExpectedEvent!=aEventType)
		{
		TInt id=iGroupWin.Identifier();
		if (!iExpectingEvent)
			{
			_LIT(KLog,"Event of type %d recieved when not expecting an event (GpWinId=%d).");
			iTest->LOG_MESSAGE3(KLog,aEventType,id);
			}
		else
			{
			_LIT(KLog,"Event of type %d when expecting an event of type %d (GpWinId=%d).");
			iTest->LOG_MESSAGE4(KLog,aEventType,iExpectingEvent,id);
			}
		}
	iExpectingEvent=EFalse;
	}

//

CTWindowTest::CTWindowTest(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{}

CTWindowTest::~CTWindowTest()
	{
	delete iWin;
	}

void CTWindowTest::ConstructL()
	{}

TInt CTWindowTest::MoveGroup(TAny* aParam)
	{
	RWsSession ws;
	TInt err=ws.Connect();
	if (err==KErrNone)
		{
		TInt command=static_cast<TWindowThreadParam*>(aParam)->iCommand;
		err=ws.SetWindowGroupOrdinalPosition(command&EIdMask,(command&EPosMask)>>EPosShift);
		ws.Finish();
		ws.Close();
		}
	return err;
	}

void CTWindowTest::CreateTestWindowL()
	{
	iWin=new(ELeave) CWinTestWindow(TRgb(0,0,0));
	iWin->ConstructL(TPoint(1,1),TSize(TestWin->Size().iWidth/2,TestWin->Size().iHeight/2),TheClient->iGroup,*TheClient->iGc,0);
	}

void CTWindowTest::DrawWindows(CWinTestWindow *aWin)
	{
	if (aWin->iChild)
		DrawWindows(aWin->iChild);
	aWin->Win()->BeginRedraw();
	TheGc->Activate(*aWin->Win());
	TheGc->DrawRect(TRect(aWin->Win()->Size()));
	TheGc->Deactivate();
	aWin->Win()->EndRedraw();
	TheClient->iWs.Flush();
	}

void CTWindowTest::CheckAndDestroyWindows()
	{
	DrawWindows(iWin);
	delete iWin;
	iWin=NULL;
	}

void CTWindowTest::DestroyWindowWithActiveGc()
	{
	iWin->Win()->BeginRedraw();
	TheGc->Activate(*iWin->Win());
	delete iWin;
	TheGc->Deactivate();
	iWin=NULL;
	}

void CTWindowTest::DestroyWindowWithActiveGc2L()
	{
	iWin->Win()->BeginRedraw();
	TheGc->Activate(*iWin->Win());
	CWindowGc *gc1=new(ELeave) CWindowGc(TheClient->iScreen);
	CWindowGc *gc2=new(ELeave) CWindowGc(TheClient->iScreen);
	CWindowGc *gc3=new(ELeave) CWindowGc(TheClient->iScreen);
	gc1->Construct();
	gc2->Construct();
	gc3->Construct();
	gc1->Activate(*iWin->Win());
	gc2->Activate(*iWin->Win());
	gc3->Activate(*iWin->Win());
	delete iWin;
	TheGc->Deactivate();
	delete gc1;
	delete gc2;
	delete gc3;
	iWin=NULL;
	}

LOCAL_C TInt DoPanicTest(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));

	RWindowGroup group(ws);
	group.Construct(888);
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close

	RWindow redraw(ws);
	redraw.Construct(group,88);

	RBackedUpWindow backedUp(ws);
	backedUp.Construct(group,EGray16,99);

	RBlankWindow blank(ws);
	blank.Construct(group,111);

	RWindowBase *base=NULL;
	TInt source=aInt/10000;
	TInt target=(aInt%10000)/100;
	TInt panic=aInt%100;
	switch(source)
		{
		case 0:
			base= &redraw;
			break;
		case 1:
			base= &backedUp;
			break;
		case 2:
			base= &blank;
			break;
		}
	switch(target)
		{
		case 0:
			{
			RWindow *win=(RWindow *)base;
			switch(panic)
				{
				case 0:
					win->BeginRedraw();
					break;
				case 1:
					ws.SetAutoFlush(ETrue);
					win->BeginRedraw(TRect(0,0,1,1));
					ws.SetAutoFlush(EFalse);
					break;
				case 2:
					ws.SetAutoFlush(ETrue);
					win->EndRedraw();
					ws.SetAutoFlush(EFalse);
					break;
				case 3:
					win->Invalidate();
					break;
				case 4:
					win->Invalidate(TRect(0,0,1,1));
					break;
				case 5:
					{
					RRegion region;
					win->GetInvalidRegion(region);
					}
					break;
				case 6:
					win->SetBackgroundColor(TRgb(0,0,0));
					break;
				case 7:
					return(EWsExitReasonFinished);
				}
			break;
			}
		case 1:
			{
			RBackedUpWindow *win=(RBackedUpWindow *)base;
			switch(panic)
				{
				case 0:
					win->BitmapHandle();
					break;
				case 1:
					win->UpdateScreen();
					break;
				case 2:
					win->UpdateScreen(TRegionFix<1>(TRect(10,10,20,20)));
					break;
				case 3:
					win->UpdateBackupBitmap();
					break;
				case 4:
					win->MaintainBackup();
					break;
				case 5:
					return(EWsExitReasonFinished);
				}
			break;
			}
		case 2:
			{
			RBlankWindow *win=(RBlankWindow *)base;
			switch(panic)
				{
				case 0:
					win->SetColor(TRgb(0));
					break;
				case 1:
					return(EWsExitReasonFinished);
				}
			break;
			}
		}
	ws.Flush();
	return(EWsExitReasonBad);	// Should never get here, but it's baaddd if it does
	}

struct TWsLocalStructure
	{
	TInt xPos;
	TInt yPos;
	TInt Length;
	TAny *ptr;
	};

LOCAL_C TInt DoPanicTest2(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
//
	CWsScreenDevice *screen=new(ELeave) CWsScreenDevice(ws);
	screen->Construct((TInt)aScreenNumber);
//
	RWindowGroup group(ws);
	group.Construct(999);
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close

	RWindow win(ws);
	win.Construct(group,122);
	win.Activate();

	switch(aInt)
		{
		case 0:
			{
			win.BeginRedraw();
			CWindowGc *gc;
			screen->CreateContext(gc);
			gc->Activate(win);
			CFbsFont *font;
			screen->GetNearestFontToDesignHeightInTwips((CFont *&)font,TFontSpec());
			gc->UseFont(font);
			TWsLocalStructure params;
			params.xPos=0;
			params.yPos=0;
			params.Length=1;
			TPckgC<TWsLocalStructure> pkg(params);
			TPtr8 ptr(NULL,10,10);
			ws.TestWriteReplyByProvidingRemoteReadAccess(gc->WsHandle(),EWsGcOpDrawTextPtr,pkg,ptr); // Bad source descriptor
			}
			break;
		case 1:
			{
			TPtr8 bad(NULL,0); // Bad descriptor
			ws.TestWriteReplyP(win.WsHandle(),EWsWinOpSize,NULL,0,&bad); // Bad descriptor
			}
			break;
		case 2:
			{
			TSize size;
			TPtr8 bad((TUint8 *)&size,4,4); // Short descriptor
			ws.TestWriteReplyP(win.WsHandle(),EWsWinOpSize,NULL,0,&bad); // Short descriptor
			}
			break;
		case 3:
			win.EnablePointerMoveBuffer();
			break;
		case 4:
			{
			RBackedUpWindow backup(ws);
			backup.Construct(group,EGray16,123);
			backup.Activate();
			backup.SetRequiredDisplayMode(EGray16);
			}
			break;
		default:
			return(EWsExitReasonFinished);
		}
	ws.Flush();
	return(EWsExitReasonBad);	// Should never get here, but it's baaddd if it does
	}

LOCAL_C TInt CallWindowFuction(RWindowTreeNode* aWin,TInt aWinType,TInt aFunc,RWsSession aWs)
	{
	switch(aWinType)
		{
	case 0:		//Call functions from RWindowTreeNode
		{
		RWindowTreeNode* win=aWin;
		switch(aFunc)
			{
		case 0:
			win->OrdinalPosition();
			break;
		case 1:
			win->SetOrdinalPosition(1);
			break;
		case 2:
			win->SetOrdinalPosition(1,2);
			break;
		case 3:
			win->FullOrdinalPosition();
			break;
		case 4:
			win->Parent();
			break;
		case 5:
			win->PrevSibling();
			break;
		case 6:
			win->SetFaded(0,RWindowTreeNode::EFadeIncludeChildren);
			break;
		case 7:
			win->WindowGroupId();
			break;
		//The following can be called on a window with no parent without panicking
		case 8:
			win->ClearPointerCursor();
			break;
		case 9:
			win->ClientHandle();
			break;
		case 10:
			win->DisableErrorMessages();
			break;
		case 11:
			win->DisableFocusChangeEvents();
			break;
		case 12:
			win->DisableGroupChangeEvents();
			break;
		case 13:
			win->DisableGroupListChangeEvents();
			break;
		case 14:
			win->DisableModifierChangedEvents();
			break;
		case 15:
			win->DisableOnEvents();
			break;
		case 16:
			win->DisableVisibilityChangeEvents();
			break;
		case 17:
			win->EnableErrorMessages(EEventControlAlways);
			break;
		case 18:
			win->EnableFocusChangeEvents();
			break;
		case 19:
			win->EnableGroupChangeEvents();
			break;
		case 20:
			win->EnableGroupListChangeEvents();
			break;
		case 21:
			win->EnableModifierChangedEvents(0,EEventControlAlways);
			break;
		case 22:
			win->EnableVisibilityChangeEvents();
			break;
		case 23:
			win->NextSibling();
			break;
		case 24:
			win->OrdinalPriority();
			break;
		case 25:
			win->SetNonFading(0);
			break;
		case 26:
			win->SetPointerCursor(0);
			break;
		case 27:
			win->Child();
			break;
		case 28:
			return(EWsExitReasonFinished);
		default:;
			}
		}
		break;
	case 1:		//Call functions from RWindowGroup
		{
		RWindowGroup* win=(RWindowGroup*)aWin;
		switch(aFunc)
			{
		case 0:
			win->EnableReceiptOfFocus(ETrue);
			break;
		case 1:
			win->AutoForeground(ETrue);
			break;
		case 2:
			win->SetOrdinalPriorityAdjust(5);
			break;
		case 3:
			win->CaptureKey(20,0,0);
			break;
		case 4:
			win->CaptureKeyUpAndDowns(20,0,0);
			break;
		case 5:
		case 6:
			{
			RWindowGroup group(aWs);
			group.Construct(876);
			group.EnableReceiptOfFocus(EFalse);		// Stop auto group switching on close
			if (aFunc==5)
				win->CancelCaptureKey(group.CaptureKey(20,0,0));
			else
				win->CancelCaptureKeyUpAndDowns(group.CaptureKeyUpAndDowns(20,0,0));
			}
			break;
		case 7:
			win->AddPriorityKey(20,0,0);
			break;
		case 8:
			win->RemovePriorityKey(20,0,0);
			break;
		case 9:
		case 10:
			{
			RWindowGroup group(aWs);
			group.Construct(765);
			group.EnableReceiptOfFocus(EFalse);		// Stop auto group switching on close
 			RWindow window(aWs);
			window.Construct(group,79);
			if (aFunc==9)
				win->SetTextCursor(window,TPoint(45,46),TTextCursor());
			else
				win->SetTextCursor(window,TPoint(55,66),TTextCursor(),TRect(TSize(5,8)));
			}
			break;
		case 11:
			win->CancelTextCursor();
			break;
		case 12:
			win->SetOwningWindowGroup(456);
			break;
		case 13:
			win->DefaultOwningWindow();
			break;
		case 14:
			{
			TBufC<8> text(_L("abcdef"));
			win->SetName(text);
			}
			break;
		case 15:
			{
			TBuf<16> text;
			User::LeaveIfError(win->Name(text));
			}
			break;
		case 16:
			win->Identifier();
			break;
		case 17:
			win->DisableKeyClick(ETrue);
			break;
		case 18:
			/*{
			TPtr8 text(NULL,0);
			TUid uid;
			User::LeaveIfError(win->FetchMessage(uid,text));
			}*/
			win->Identifier();
			break;
		case 19:
			User::LeaveIfError(win->EnableScreenChangeEvents());
			break;
		case 20:
			win->EnableScreenChangeEvents();
			break;
		case 21:
			win->DisableScreenChangeEvents();
			break;
		case 22:
			win->SimulatePointerEvent(TRawEvent());
			break;
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
        case 23: 	
        	win->SimulateAdvancedPointerEvent(TRawEvent());
  	  		break;
        case 24:	
			return(EWsExitReasonFinished);
#else
		case 23:	
			return(EWsExitReasonFinished);
#endif			
		default:;
			}
		}
		break;
	case 2:					//Call functions from RWindowBase
		{
		RWindowBase* win=(RWindowBase*)aWin;
		switch(aFunc)
			{
		case 0:
			win->SetPosition(TPoint(7,8));
			break;
		case 1:
			User::LeaveIfError(win->SetSizeErr(TSize(21,22)));
			break;
		case 2:
			User::LeaveIfError(win->SetExtentErr(TPoint(8,9),TSize(21,22)));
			break;
		case 3:
			win->ClaimPointerGrab();
			break;
		case 4:
			win->SetVisible(ETrue);
			break;
		case 5:
			win->EnableBackup();
			break;
		case 6:
			win->RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(100000),TRect());
			break;
		case 7:
			win->PasswordWindow(EPasswordCancel);
			break;
		case 8:
			win->FadeBehind(ETrue);
			break;
		//These can all be called on a window with no parent without panicking
		case 9:
			win->SetVisible(EFalse);
			break;
		case 10:
			{
			RWindowGroup group(aWs);
			group.Construct(567);
			group.EnableReceiptOfFocus(EFalse);		// Stop auto group switching on close
 			RWindow window(aWs);
			window.Construct(group,97);
			win->InquireOffset(window);
			}
			break;
		case 11:
			win->PointerFilter(0,0);
			break;
		case 12:
			win->SetPointerGrab(ETrue);
			break;
		case 13:
			win->SetPointerCapture(0);
			break;
		case 14:
			win->Size();
			break;
		case 15:
			win->Position();
			break;
		case 16:
			User::LeaveIfError(win->SetCornerType(EWindowCorner5,0));
			break;
		case 17:
			{
			TRegionFix<3> region;
			User::LeaveIfError(win->SetShape(region));
			}
			break;
		case 18:
			User::LeaveIfError(win->SetRequiredDisplayMode(EColor16));
			break;
		case 19:
			win->DisplayMode();
			break;
		case 20:
			win->CancelPointerRepeatEventRequest();
			break;
		case 21:
			win->AllocPointerMoveBuffer(10,0);
			break;
		case 22:
			win->FreePointerMoveBuffer();
			break;
		case 23:
			win->DisablePointerMoveBuffer();
			break;
		case 24:
			{
			TBuf8<16> buf;
			win->RetrievePointerMoveBuffer(buf);
			}
			break;
		case 25:
			win->DiscardPointerMoveBuffer();
			break;
		case 26:
			User::LeaveIfError(win->AddKeyRect(TRect(TSize(10,10)),20,ETrue));
			break;
		case 27:
			win->RemoveAllKeyRects();
			break;
		case 28:
			win->EnablePointerMoveBuffer();
			break;
		case 29:
			return(EWsExitReasonFinished);
		default:;
			}
		}
		break;
	case 3:					//Call functions from RDrawableWindow
		{
		RDrawableWindow* win=(RDrawableWindow*)aWin;
		switch(aFunc)
			{
		//The following can be called on a window with no parent without panicking
		case 0:
			win->Scroll(TPoint(7,8));
			break;
		case 1:
			win->Scroll(TRect(9,10,11,12),TPoint(13,14));
			break;
		case 2:
			win->Scroll(TPoint(15,16),TRect(17,18,19,20));
			break;
		case 3:
			win->Scroll(TRect(21,22,23,24),TPoint(25,26),TRect(27,28,29,30));
			break;
		case 4:
			return(EWsExitReasonFinished);
		default:;
			}
		}
		break;
	case 4:					//Call functions from RBlankWindow
		{
		RBlankWindow* win=(RBlankWindow*)aWin;
		switch(aFunc)
			{
		case 0:
			win->SetSize(TSize(7,8));
			break;
		case 1:
			win->SetExtent(TPoint(27,28),TSize(17,18));
			break;
		//The following function can be called on a window with no parent without panicking
		case 2:
			win->SetColor(TRgb::Gray4(2));
			break;
		case 3:
			return(EWsExitReasonFinished);
		default:;
			}
		}
		break;
	case 5:					//Call functions from RWindow
		{
		RWindow* win=(RWindow*)aWin;
		switch(aFunc)
			{
		case 0:
			win->BeginRedraw();
			break;
		case 1:
			aWs.SetAutoFlush(ETrue);
			win->BeginRedraw(TRect(31,32,43,44));
			aWs.SetAutoFlush(EFalse);
			break;
		case 2:
			win->SetSize(TSize(5,6));
			break;
		case 3:
			win->SetExtent(TPoint(25,26),TSize(15,16));
			break;
		//The following can be called on a window with no parent without panicking
		case 4:
			win->Invalidate();
			break;
		case 5:
			win->Invalidate(TRect(51,52,63,64));
			break;
		case 6:			//These ones don't panic
			aWs.SetAutoFlush(ETrue);
			win->EndRedraw();
			aWs.SetAutoFlush(EFalse);
			break;
		case 7:
			{
			RRegion region;
			win->GetInvalidRegion(region);
			}
			break;
		case 8:
			win->SetBackgroundColor(TRgb::Gray4(1));
			break;
		case 9:
			win->SetBackgroundColor();
			break;
		case 10:
			return(EWsExitReasonFinished);
		default:;
			}
		}
		break;
	case 6:					//Call functions from RBackedUpWindow
		{
		RBackedUpWindow* win=(RBackedUpWindow*)aWin;
		switch(aFunc)			//None of these functions panic
			{
		//The following can be called on a window with no parent without panicking
		case 0:
			win->BitmapHandle();
			break;
		case 1:
			win->UpdateBackupBitmap();
			break;
		case 2:
			win->MaintainBackup();
			break;
		case 3:
			win->UpdateScreen();
			break;
		case 4:
			win->UpdateScreen(TRegionFix<1>(TRect(1,1,22,22)));
			break;
		case 5:
			return(EWsExitReasonFinished);
		default:;
			}
		}
		break;
		}
	aWs.Flush();
	return(EWsExitReasonBad);	// Should never get here, but it's baaddd if it does
	}

LOCAL_C TInt DoPanicTest3(TInt aInt, TAny *aScreenNumber)
	{
	TInt winType=aInt/CTWindowTest::EWinTypeFactor;
	TInt func=aInt%CTWindowTest::EWinTypeFactor;
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));

	RWindowGroup group(ws);
	group.Construct(888);
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
 	RWindow window(ws);
	window.Construct(group,789);
	RWindowTreeNode* win;
	if (winType==1)
		win=&window;
	else
		win=&group;

	return CallWindowFuction(win,winType,func,ws);
	}

//PanicTestNoPanic
//This function is called from DoPanicTest4 for window functions that should run without
//panicking the user thread if its parent has been deleted.
TInt PanicTestNoPanic(RWindowTreeNode* aWin,TInt aWinType,TInt aFunc,RWsSession aWs,const TInt* const aFuncToSkip)
	{
	TInt funcReturn;
	for(;;)
		{
		if (aFuncToSkip && *aFuncToSkip == aFunc)
			{
			aFunc++;
			}
		funcReturn = CallWindowFuction(aWin,aWinType,aFunc,aWs);
		if (funcReturn==EWsExitReasonBad)
			{
			aFunc++;
			}
		else if (funcReturn==EWsExitReasonFinished)
			{
			return EWsExitReasonFinished;
			}
		else
			{
			return EWsExitReasonBad;
			}
		}
	}

LOCAL_C TInt DoPanicTest4(TInt aInt, TAny *aScreenNumber)
	{
 	TInt winType=aInt/CTWindowTest::EWinTypeFactor;
	TInt func=aInt%CTWindowTest::EWinTypeFactor;
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));

	RWindowGroup group(ws);
	group.Construct(234);
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
 	RWindow window(ws);
	window.Construct(group,897);
	RBackedUpWindow backedUp(ws);
	backedUp.Construct(group,EGray16,98);
	RBlankWindow blank(ws);
	blank.Construct(group,169);
	RWindowTreeNode* win=&window;
	switch (winType)
		{
	case 0:
		{
		if (func > 8)			//if a func 8+ had panicked, fail the test
			{
			return EWsExitReasonBad;
			}
		else if (func==8)
			{
			group.Close();
			return PanicTestNoPanic(win,winType,func,ws,NULL);
			}
		}
		break;
	case 2:
		{
		if (func>9)			//if a func 9+ had panicked, fail the test
			{
			return EWsExitReasonBad;
			}
		else if (func==9)	//set window visible so that SetVisible(EFalse) (func 9) would crash before fix
			{
			window.SetVisible(ETrue);
			ws.Flush();
			group.Close();
			TInt funcToSkip = 28;	//this call needs to have already successfully allocated a pointer cursor
			return PanicTestNoPanic(win,winType,func,ws,&funcToSkip);
			}
		}
		break;
	case 4:
		{
		win=&blank;
		if (func>2)			//if a func 2+ had panicked, fail the test
			{
			return EWsExitReasonBad;
			}
		else if (func==2)
			{
			group.Close();
			return PanicTestNoPanic(win,winType,func,ws,NULL);
			}
		}
		break;
	case 5:
		{
		if (func>6)			//if a func 4+ had panicked, fail the test
			{
			return EWsExitReasonBad;
			}
		else if (func==4 || func==5)
			{
			if (CallWindowFuction(win,winType,func,ws)==EWsExitReasonBad)
				{
				func = 1;
				}
			else
				{
				return EWsExitReasonBad;
				}
			}
		else if (func==6)
			{
			group.Close();
			TInt funcToSkip = 6;	//Skip the call to EndRedraw in CallWindowFunction, it is not safe to call it on Window casts.
			return PanicTestNoPanic(win,winType,func,ws,&funcToSkip);
			}
		}
		break;
	case 6:
		{
		win=&backedUp;
		if (func>0)
			return EWsExitReasonBad;
		else		//if (func==0)
			{
			TInt end=2;
			while (func==0)
				{
				group.Close();
				while (CallWindowFuction(win,winType,func,ws)==EWsExitReasonBad && ++func<end)
					{}
				if (func==end && end==2)
					{
					func=0;
					end=4;
					}
				}
			if (func==end)
				return EWsExitReasonFinished;
			else
				return EWsExitReasonBad;
			}
		}
		/*break;*/
	default:;
		}
	group.Close();
	return CallWindowFuction(win,winType,func,ws);
	}

LOCAL_C TInt DoPanicTest5(TInt aTest, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());

	CWsScreenDevice *scrDev=new(ELeave) CWsScreenDevice(ws);
	scrDev->Construct((TInt)aScreenNumber);

	RWindowGroup group(ws);
	group.Construct(235);
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
 	RWindow window(ws);
	window.Construct(group,896);
	CWindowGc *gc=new(ELeave) CWindowGc(scrDev);
	gc->Construct();
	gc->Activate(window);
	group.Close();
	switch (aTest)
		{
	case 0:
		gc->Deactivate();
		gc->Activate(window);
		break;
	case 1:
		gc->DrawLine(TPoint(0,0),TPoint(10,10));
		break;
	default:
		return(EWsExitReasonFinished);
		}
	ws.Flush();
	return(EWsExitReasonBad);	// Should never get here, but it's baaddd if it does
	}

#if defined(_DEBUG)
LOCAL_C TInt DoPanicTest6(TInt /*aInt*/, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	RWindowGroup group1(ws);
	CWsScreenDevice* screen=new(ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
	group1.Construct(123456,ETrue,screen);
	RWindowGroup group2(ws);
	group2.Construct(123456,ETrue,screen);	// Should panic client here
	return(EWsExitReasonBad);	// Should never get here
	}
#endif

void CTWindowTest::TestInvalidFunctionsL()
//
// This code casts windows to be different types and then sends messages for the 'cast' that
// should not be sent to the original type of window. These should all result in panics
//
	{
	static TClientPanic PanicCode[]={EWservPanicDescriptor,EWservPanicDescriptor,EWservPanicDescriptor,
									 EWservPanicNoPointerBuffer,EWservPanicBackupDisplayMode,
									 EWservPanicNoFont};	// Dummy end value to catch out overflowing the array
	INFO_PRINTF1(_L("AUTO  Test Invalid Functions "));
	INFO_PRINTF1(_L(" Opcode Panics"));

	for (TInt source=0;source<3;source++)
		for (TInt target=0;target<3;target++)
			if (source!=target)
				for (TInt panic=0;;panic++)
					{
					TBool testFinished=EFalse;
					TEST(iTest->TestWsPanicL(DoPanicTest,EWservPanicOpcode,source*10000+target*100+panic,(TAny*)iTest->iScreenNumber,&testFinished));
					if (testFinished)
						break;
					}
	_LIT(KSet,"Various Different Panics");
	INFO_PRINTF1(KSet);
	RDebug::Print(KPlatsecBegin);
	for(TInt index=0;;index++)
		{
		TBool testFinished=EFalse;
		TEST(iTest->TestWsPanicL(DoPanicTest2,PanicCode[index],index,(TAny*)iTest->iScreenNumber,&testFinished));
		if (testFinished)
			break;
		}
	RDebug::Print(KPlatsecEnd);
	iTest->CloseAllPanicWindows();
	}

void CTWindowTest::ShadowAutoClearTest()
	{
	RWindowGroup group(TheClient->iWs);
	group.Construct(1111);
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close

	RWindow background(TheClient->iWs);
	background.Construct(group,133);
	background.SetBackgroundColor();
	background.Activate();

	background.BeginRedraw();
	TheClient->iGc->Activate(background);
	TheClient->iGc->SetBrushColor(TRgb::Gray4(1));
	TheClient->iGc->Clear();
	background.EndRedraw();
//
	RBlankWindow tab2(TheClient->iWs);
	tab2.Construct(group,144);
	tab2.SetExtent(TPoint(10,00),TSize(200,10));
	tab2.SetColor(TRgb::Gray256(170));
	tab2.SetShadowHeight(1);
	tab2.Activate();
	RBlankWindow tab1(TheClient->iWs);
	tab1.Construct(group,155);
	tab1.SetExtent(TPoint(70,00),TSize(10,10));
	tab1.SetColor(TRgb::Gray256(170));
	tab1.SetShadowHeight(1);
	tab1.Activate();
	RBlankWindow blank(TheClient->iWs);
	blank.Construct(group,156);
	blank.SetExtent(TPoint(50,10),TSize(100,100));
	blank.SetColor(TRgb::Gray256(170));
	blank.SetShadowHeight(0);
	blank.Activate();
//
	tab1.Close();
	tab1.Construct(group,166);
	tab1.SetExtent(TPoint(50,00),TSize(10,10));
	tab1.SetColor(TRgb::Gray256(170));
	tab1.SetShadowHeight(1);
	tab1.Activate();
	blank.Close();
	RBlankWindow blank2(TheClient->iWs);
	blank2.Construct(group,177);
	blank2.SetColor(TRgb::Gray256(255));
	blank2.SetExtent(TPoint(10,10),TSize(100,120));
	blank2.SetShadowHeight(2);
	blank2.Activate();
	TheClient->iWs.Flush();
//
	background.BeginRedraw();
	TheClient->iGc->SetBrushColor(TRgb::Gray4(1));
	TheClient->iGc->Clear();
	background.EndRedraw();
//
	tab1.Close();
	tab2.Close();
	blank2.Close();
	background.BeginRedraw();
	TheClient->iGc->Clear();
	TheClient->iGc->Deactivate();
	background.EndRedraw();
	TheClient->iWs.Finish();
	TSize size=TheClient->iScreen->SizeInPixels();
	TBool rectCompare = TheClient->iScreen->RectCompare(TRect(0,0,size.iWidth>>1,size.iHeight),TRect(size.iWidth>>1,0,(size.iWidth>>1)<<1,size.iHeight));	
	TEST(rectCompare);
	if(!rectCompare)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare return value - Expected: %d, Actual: %d"), ETrue, rectCompare);
	background.Close();
	group.Close();
	}

void CTWindowTest::ClearRedraw(RWindow &aWindow, TRgb aRgb)
	{
	aWindow.BeginRedraw();
	TheClient->iGc->Activate(aWindow);
	TheClient->iGc->SetBrushColor(aRgb);
	TheClient->iGc->Clear();
	TheClient->iGc->Deactivate();
	aWindow.EndRedraw();
	}

void CTWindowTest::CheckCorner(TCorner aCorner, const TInt *aInsetList)
	{
	TRect rect(TPoint(1,1),iBlankWin2->Size());
	TPoint pos;
	TInt direction;
	if (aCorner==ECornerTL || aCorner==ECornerTR)
		{
		pos.iY=rect.iTl.iY;
		direction=1;
		}
	else
		{
		pos.iY=rect.iBr.iY-1;
		direction= -1;
		}
	if (aCorner==ECornerTL || aCorner==ECornerBL)
		pos.iX=rect.iTl.iX;
	else
		pos.iX=rect.iBr.iX-8;
	TRgb rgbBuf[8];
	TPtr8 desc((TUint8 *)rgbBuf,sizeof(rgbBuf));
	for(TInt count=0;count<ENumCornerInsets;count++,pos.iY+=direction)
		{
		iScreenDev->GetScanLine(desc,pos,8,EColor16MA);
		if (aCorner==ECornerTR || aCorner==ECornerBR)
			{
			for(TInt loop=0;loop<4;loop++)
				{
				TRgb tmp=rgbBuf[loop];
				rgbBuf[loop]=rgbBuf[8-1-loop];
				rgbBuf[8-1-loop]=tmp;
				}
			}
		// We can't compare rgb value from original source against screen value in EColor64K mode as
		// the color component might be truncated (EColor64K is 16-bit using format RGB565),
		// ie R or B components might be reduced from 8-bit to 5-bit and G from 8-bit to 6-bit
		//
		// For example: RGB value of Gray4(1) is 0x555555, it is drawn to screen in RGB565 as 0x52AA,
		// when it's converted back to RGB for comparison, the value becomes 0x525552
		TRgb col1=TRgb::Gray4(1);
		TRgb col2=TRgb::Gray4(3);
		if (iScreenDev->DisplayMode()==EColor64K)
			{
			col1=TRgb::Color64K(col1.Color64K());
			col2=TRgb::Color64K(col2.Color64K());
			}
		TInt loop2=0;
		for(;loop2<aInsetList[count];loop2++)
			{	
			TEST(rgbBuf[loop2]==col1);				
			}
		for(;loop2<8;loop2++)
			{		
			TEST(rgbBuf[loop2]==col2);
			}
		}
	}

void CTWindowTest::doCornerTest(TCornerType aCornerType, TInt aFlags)
	{
	const TInt *corners=corner0;
	switch(aCornerType)
		{
		case EWindowCorner1:
			corners=corner1;
			break;
		case EWindowCorner2:
			corners=corner2;
			break;
		case EWindowCorner3:
			corners=corner3;
			break;
		case EWindowCorner5:
			corners=corner5;
			break;
		default:
			break;
		}
	iBlankWin2->BaseWin()->SetCornerType(aCornerType,aFlags);
	if (!(aFlags&EWindowCornerNotTL))
		CheckCorner(ECornerTL, corners);
	if (!(aFlags&EWindowCornerNotTR))
		CheckCorner(ECornerTR, corners);
	if (!(aFlags&EWindowCornerNotBL))
		CheckCorner(ECornerBL, corners);
	if (!(aFlags&EWindowCornerNotBR))
		CheckCorner(ECornerBR, corners);
//
	if (aFlags&EWindowCornerNotTL)
		CheckCorner(ECornerTL, corner0);
	if (aFlags&EWindowCornerNotTR)
		CheckCorner(ECornerTR, corner0);
	if (aFlags&EWindowCornerNotBL)
		CheckCorner(ECornerBL, corner0);
	if (aFlags&EWindowCornerNotBR)
		CheckCorner(ECornerBR, corner0);
	}

void CTWindowTest::doCornerTestsL()
	{
	iScreenDev=new(ELeave) CWsScreenDevice(TheClient->iWs);
	User::LeaveIfError(iScreenDev->Construct(iTest->iScreenNumber));
	iBlankWin1=new(ELeave) CTBlankWindow();
	iBlankWin1->SetUpL(TPoint(1,1),TSize(100,50),TheClient->iGroup,*TheClient->iGc);
	iBlankWin1->SetColor(TRgb::Gray4(1));
	iBlankWin2=new(ELeave) CTBlankWindow();
	iBlankWin2->SetUpL(TPoint(1,1),iBlankWin1->Size(),TheClient->iGroup,*TheClient->iGc);
	iBlankWin2->SetColor(TRgb::Gray4(3));
	iBlankWin2->BaseWin()->SetShadowHeight(0);
	doCornerTest(EWindowCornerSquare,0);
	doCornerTest(EWindowCornerSquare,EWindowCornerNotBL);
	doCornerTest(EWindowCorner1,0);	// 0 + all corners missing
	doCornerTest(EWindowCorner1,EWindowCornerNotTL|EWindowCornerNotTR|EWindowCornerNotBL|EWindowCornerNotBR);
	doCornerTest(EWindowCorner2,0);	// 0 + all sets of 3 corners missing
	doCornerTest(EWindowCorner2,EWindowCornerNotTR|EWindowCornerNotBL|EWindowCornerNotBR);
	doCornerTest(EWindowCorner2,EWindowCornerNotTL|EWindowCornerNotBL|EWindowCornerNotBR);
	doCornerTest(EWindowCorner2,EWindowCornerNotTL|EWindowCornerNotTR|EWindowCornerNotBR);
	doCornerTest(EWindowCorner2,EWindowCornerNotTL|EWindowCornerNotTR|EWindowCornerNotBL);
	doCornerTest(EWindowCorner3,0);	// 0 + all 4 individual corners missing
	doCornerTest(EWindowCorner3,EWindowCornerNotTL);
	doCornerTest(EWindowCorner3,EWindowCornerNotTR);
	doCornerTest(EWindowCorner3,EWindowCornerNotBL);
	doCornerTest(EWindowCorner3,EWindowCornerNotBR);
	doCornerTest(EWindowCorner5,0);	// 0 + all pairs of corners missing
	doCornerTest(EWindowCorner5,EWindowCornerNotTL|EWindowCornerNotTR);
	doCornerTest(EWindowCorner5,EWindowCornerNotTL|EWindowCornerNotBL);
	doCornerTest(EWindowCorner5,EWindowCornerNotTL|EWindowCornerNotBR);
	doCornerTest(EWindowCorner5,EWindowCornerNotTR|EWindowCornerNotBL);
	doCornerTest(EWindowCorner5,EWindowCornerNotTR|EWindowCornerNotBR);
	doCornerTest(EWindowCorner5,EWindowCornerNotBL|EWindowCornerNotBR);
	}

void CTWindowTest::CornerTests()
	{
	TRAP_IGNORE(doCornerTestsL());
	delete iBlankWin2;
	delete iBlankWin1;
	delete iScreenDev;
	}

LOCAL_C void doMegaTreeThread(TInt aScreenNumber)
	{
	CTrapCleanup::New();
	RWsSession ws;
	ws.Connect();
	CWsScreenDevice *scrDev=new(ELeave) CWsScreenDevice(ws);
	scrDev->Construct(aScreenNumber);
	RWindowGroup group(ws);
	group.Construct(1);
	group.EnableReceiptOfFocus(EFalse);
	RWindow parent(ws);
	parent.Construct(group,123);
	parent.Activate();
	CWindowGc *gc=new(ELeave) CWindowGc(scrDev);
	gc->Construct();
	TSize max(parent.Size());
	RWindow prev=parent;
	TBool horiz=EFalse;
	TInt color=0;
	for(TInt count=0;count<100;count++)
		{
		RWindow win(ws);
		if (win.Construct(prev,ENullWsHandle)!=KErrNone)
			break;
		win.SetExtent(horiz?TPoint(1,0):TPoint(0,1),max);
		win.SetBackgroundColor(TRgb::Gray4(color));
		color=(color+1)%4;
		win.Activate();
		win.BeginRedraw();
		gc->Activate(win);
		gc->Clear();
		gc->Deactivate();
		win.EndRedraw();
		prev=win;
		horiz=!horiz;
		}
	parent.SetVisible(EFalse);
	parent.SetVisible(ETrue);
	parent.SetPosition(TPoint(-1,-1));
	parent.SetPosition(TPoint(0,0));
	parent.Close();
	ws.Close();
	}

LOCAL_C TInt MegaTreeThread(TAny *aScreenNumber)
	{
	TRAPD(err,doMegaTreeThread((TInt)aScreenNumber));
	return(err);
	}

void CTWindowTest::CreateMegaTree()
	{
	const TUint KThreadHeapSize=0x2000;
	RThread thread;
	if (thread.Create(_L("MegaTree"),MegaTreeThread,KDefaultStackSize,KThreadHeapSize,KThreadHeapSize,(TAny*)iTest->iScreenNumber,EOwnerThread)==KErrNone)
		{
		TRequestStatus stat;
		thread.Logon(stat);
		thread.Resume();
		User::WaitForRequest(stat);		
		TEST(stat==KErrNone);
		}
	thread.Close();
	}

void CTWindowTest::TiledWindowTestL()
	{
	RWindow parent(TheClient->iWs);
	User::LeaveIfError(parent.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&parent));
	parent.SetExtent(TPoint(10,10),TSize(50,50));
	parent.Activate();
//
	RWindow child1(TheClient->iWs);
	User::LeaveIfError(child1.Construct(parent,ENullWsHandle));
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&child1));
	child1.SetExtent(TPoint(0,0),TSize(50,20));
	child1.Activate();
//
	RWindow child2(TheClient->iWs);
	User::LeaveIfError(child2.Construct(parent,ENullWsHandle));
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&child2));
	child2.SetExtent(TPoint(0,20),TSize(50,30));
	child2.Activate();
//
	child1.BeginRedraw();
	TSize rect1Size(25,20);
	TheGc->Activate(child1);
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->DrawRect(TRect(rect1Size));
	TheGc->DrawRect(TRect(TPoint(rect1Size.iWidth,0),rect1Size));
	TheGc->Deactivate();
	child1.EndRedraw();
//
	child2.BeginRedraw();
	TSize rect2Size(25,30);
	TheGc->Activate(child2);
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->DrawRect(TRect(rect2Size));
	TheGc->DrawRect(TRect(TPoint(rect2Size.iWidth,0),rect2Size));
	TheGc->Deactivate();
	child2.EndRedraw();
//
// Left and right halves should be identical
//
	TBool rectCompare = !TheClient->iScreen->RectCompare(TRect(10,10,35,60),TRect(35,10,70,60));	
	TEST(rectCompare);
	if(!rectCompare)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare return value - Expected: %d, Actual: %d"), ETrue, rectCompare);
//
	CleanupStack::PopAndDestroy(3);
	}

void CTWindowTest::TiledWindowTest2L()
	{
	TSize size(200,240);		//Parent Windows
	TSize childSize(TSize(size.iWidth/2,size.iHeight/2));		//Child Windows
	TSize dialogueSize(50,50);

	TheClient->iWs.SetAutoFlush(ETrue);
	RBlankWindow parent1(TheClient->iWs);
	User::LeaveIfError(parent1.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&parent1));
	parent1.SetExtent(TPoint(240,0),size);
	parent1.SetColor(TRgb::Gray4(2));
	parent1.Activate();

	TheClient->iWs.SetAutoFlush(ETrue);
	RBlankWindow parent2(TheClient->iWs);
	User::LeaveIfError(parent2.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&parent2));
	parent2.SetExtent(TPoint(440,0),size);
	parent2.SetColor(TRgb::Gray4(2));
	parent2.Activate();

	TheClient->iWs.SetAutoFlush(ETrue);
	RBlankWindow child1(TheClient->iWs);
	User::LeaveIfError(child1.Construct(parent1,ENullWsHandle));
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&child1));
	child1.SetExtent(TPoint(0,0),childSize);
	child1.SetColor(TRgb::Gray4(2));
	child1.Activate();

	TheClient->iWs.SetAutoFlush(ETrue);
	RBlankWindow child2(TheClient->iWs);
	User::LeaveIfError(child2.Construct(parent1,ENullWsHandle));
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&child2));
	child2.SetExtent(TPoint(0,childSize.iHeight),childSize);
	child2.SetColor(TRgb::Gray4(2));
	child2.Activate();

	TheClient->iWs.SetAutoFlush(ETrue);
	RBlankWindow child3(TheClient->iWs);
	User::LeaveIfError(child3.Construct(parent1,ENullWsHandle));
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&child3));
	child3.SetExtent(TPoint(childSize.iWidth,0),TSize(childSize.iWidth,size.iHeight));
	child3.SetColor(TRgb::Gray4(2));
	child3.Activate();

	TPoint dialoguePos(375,93);
	TheClient->iWs.SetAutoFlush(ETrue);
	RBlankWindow dialog1(TheClient->iWs);
	User::LeaveIfError(dialog1.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&dialog1));
	dialog1.SetExtent(dialoguePos,dialogueSize);
	dialog1.SetColor(TRgb::Gray4(3));
	dialog1.SetShadowHeight(4);
	dialog1.Activate();

	TheClient->iWs.SetAutoFlush(ETrue);
	RBlankWindow dialog2(TheClient->iWs);
	User::LeaveIfError(dialog2.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&dialog2));
	dialog2.SetExtent(TPoint(dialoguePos.iX+size.iWidth,dialoguePos.iY),dialogueSize);
	dialog2.SetColor(TRgb::Gray4(3));
	dialog2.SetShadowHeight(0);
	dialog2.Activate();

	TInt ii;
	for (ii=400;ii>374;ii-=25)
		dialog1.SetPosition(TPoint(ii,93));
	TheClient->iWs.SetAutoFlush(EFalse);
	CleanupStack::PopAndDestroy(7);
	}

void CTWindowTest::ColorTestL()
	{
	if (iTest->MaxGrays() == 0)
		return;

	_LIT(KSet,"AUTO  Color Test ");
	INFO_PRINTF1(KSet);
	RWindow window(TheClient->iWs);
	User::LeaveIfError(window.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	PushWindowL(&window);
	window.SetExtent(TPoint(10,10),TSize(50,50));
	window.Activate();
	TInt colorMode,mode;
	TInt currentMode=TheClient->iScreen->DisplayMode();	
	TEST(currentMode==EGray4 || currentMode==EColor16 || currentMode==EColor256 || currentMode == EColor64K);
	if(currentMode!=EGray4 && currentMode!=EColor16 && currentMode!=EColor256 && currentMode != EColor64K)
		INFO_PRINTF5(_L("TheClient->iScreen->DisplayMode() return value - Expected: %d or %d or %d, Actual: %d"), EGray4, EColor16, EColor256, currentMode);
	INFO_PRINTF1(_L(" Done Setup"));
	for(colorMode=EGray4;colorMode<EColorLast;colorMode++)
		{
		INFO_PRINTF1(_L(" Loop 1"));
		if (colorMode==ERgb || (colorMode==EGray256 && TDisplayModeUtils::IsDisplayModeColor(REINTERPRET_CAST(TDisplayMode&,currentMode))) ||  (colorMode==EGray256 && TDisplayModeUtils::IsDisplayModeColor(REINTERPRET_CAST(TDisplayMode&,currentMode))) )
			continue;
		User::LeaveIfError(window.SetRequiredDisplayMode((TDisplayMode&)colorMode));
		mode=window.DisplayMode();
		if (!(mode==currentMode || mode==colorMode))
			{
			_LIT(KModes," Failed in Loop1  SetTo=%d, Actual=%d, Current=%d");
			TBuf<64> log;
			log.Format(KModes,colorMode,mode,currentMode);
			INFO_PRINTF1(log);
			}
		if (mode>ERgb && colorMode==EColor64K)
			break;	
		TEST(mode==currentMode || mode==colorMode );
		if (mode!=currentMode && mode!=colorMode)
			INFO_PRINTF4(_L("window.DisplayMode() return value - Expected: %d or %d, Actual: %d"), currentMode, colorMode, mode);
		INFO_PRINTF1(_L(" Loop 2"));
		if (mode>currentMode)
			currentMode=mode;
		if (colorMode==EColor16)
			window.SetSize(TSize(40,60));
		else if (colorMode==EColor4K)
			window.SetSize(TSize(60,40));
		mode=TheClient->iScreen->DisplayMode();
		if(TDisplayModeUtils::NumDisplayModeColors(STATIC_CAST(TDisplayMode,currentMode))==16777216)
			{
			TEST(mode==CFbsDevice::DisplayMode16M());
			if (mode!=CFbsDevice::DisplayMode16M())
				INFO_PRINTF3(_L("window.DisplayMode() return value - Expected: %d, Actual: %d"), CFbsDevice::DisplayMode16M(), mode);
			}
		else
			{		
			TEST(currentMode==mode || currentMode==colorMode);
			if (currentMode!=mode && currentMode!=colorMode)
				INFO_PRINTF3(_L("TheClient->iScreen->DisplayMode() return value  return value - Expected: %d , Actual: %d"), currentMode, mode);
			}
		}
	INFO_PRINTF1(_L(" Done First Loop"));
	TInt color,gray;
	TDisplayMode defMode=TheClient->iWs.GetDefModeMaxNumColors(color,gray);
	TInt screenNo =TheClient->iScreen->GetScreenNumber();
	TDisplayMode defModeForScreen=TheClient->iWs.GetDefModeMaxNumColors(screenNo,color,gray); 
	TEST(defMode==defModeForScreen);
	if (defMode!=defModeForScreen)
		INFO_PRINTF3(_L("TheClient->iScreen->DisplayMode() return value  return value - Expected: %d , Actual: %d"), defMode, defModeForScreen);
	if (color==16777216 && gray==256)
		{		
		TEST(defMode==EGray4 || defMode==EColor256 || defMode==EColor64K);		//WINS
		if (defMode!=EGray4 && defMode!=EColor256 && defMode != EColor64K)
			INFO_PRINTF4(_L("TheClient->iWs.GetDefModeMaxNumColors(color,gray) return value  return value - Expected: %d or %d, Actual: %d"), EGray4, EColor256, defMode);
		}
	else
		{
		if (color==0 && gray==16)
			{		
			TEST(defMode==EGray4);		//S5 family
			if (defMode!=EGray4)
				INFO_PRINTF3(_L("TheClient->iScreen->DisplayMode() return value  return value - Expected: %d , Actual: %d"), EGray4, defMode);
			}
		else
			{
			TLogMessageText buf;
			_LIT(KColorSettings,"##Data  Most Colors=%d, Most Greys=%d, DefMode=%d");
			buf.Format(KColorSettings,color,gray,defMode);
			TheClient->iWs.LogMessage(buf);
			TheClient->iWs.Flush();		
			TEST(defMode==EGray4 || defMode==EColor256);
			if (defMode!=EGray4 && defMode!=EColor256)
				INFO_PRINTF4(_L("TheClient->iWs.GetDefModeMaxNumColors(color,gray) return value  return value - Expected: %d or %d, Actual: %d"), EGray4, EColor256, defMode);
			}
		}
	CArrayFixFlat<TInt> *colorModes=new(ELeave) CArrayFixFlat<TInt>(1);
	CleanupStack::PushL(colorModes);
	User::LeaveIfError(TheClient->iWs.GetColorModeList(screenNo,colorModes));
	TDisplayMode mostColor=ENone;
	TDisplayMode lastGray=ENone;
	TDisplayMode dispMode;
	TInt ii;
	TInt colorModes16M = 0;
	INFO_PRINTF1(_L(" Done Setup 2"));
	for (ii=0;ii<colorModes->Count();ii++)
		{
		dispMode=(TDisplayMode)(*colorModes)[ii];	
		TEST(dispMode!=ERgb);
		if (dispMode==ERgb)
			INFO_PRINTF3(_L("(TDisplayMode)(*colorModes)[i] return value - Not Expected: %d , Actual: %d"), ERgb, dispMode);	
		TEST(dispMode!=ENone);
		if (dispMode==ENone)
			INFO_PRINTF3(_L("(TDisplayMode)(*colorModes)[i] return value - Not Expected: %d , Actual: %d"), ENone, dispMode);
		if (dispMode<=EGray256)
			lastGray=dispMode;
		else
			mostColor=dispMode;
		if(dispMode == EColor16M || dispMode == EColor16MU || dispMode == EColor16MA)
			colorModes16M++;
		}	
	TEST(mostColor!=ERgb);
	if (mostColor==ERgb)
		INFO_PRINTF3(_L("mostColor - Not Expected: %d , Actual: %d"), ERgb, mostColor);	
	TEST(colorModes16M <= 1);
	if (colorModes16M > 1)
		INFO_PRINTF3(_L("Number of times color Modes 16M - less than or equal to: %d , Actual: %d"), 1, colorModes16M);
#if defined(__WINS__)	
	TEST(colorModes16M == 1);
	if (colorModes16M != 1)
		INFO_PRINTF3(_L("Number of times color Modes 16M on wins - equal to: %d , Actual: %d"), 1, colorModes16M);
#endif
	TInt retVal;
	if (color==0)
		{
		TEST(mostColor==ENone);
		if (mostColor!=ENone)
			INFO_PRINTF3(_L("Most color - equal to: %d , Actual: %d"), ENone ,mostColor);
		}
	else
		{
		retVal = TDisplayModeUtils::NumDisplayModeColors(mostColor);		
		TEST(color==retVal);
		if (color!=retVal)
			INFO_PRINTF3(_L("TDisplayModeUtils::NumDisplayModeColors(mostColor) return value - equal to: %d , Actual: %d"), TDisplayModeUtils::NumDisplayModeColors(mostColor), retVal);
		}
	if (gray==0)
		{		
		TEST(lastGray==ENone);
		if (lastGray!=ENone)
			INFO_PRINTF3(_L("LastGray - equal to: %d , Actual: %d"), ENone, lastGray);
		}
	else
		{
		retVal = TDisplayModeUtils::NumDisplayModeColors(lastGray);	
		TEST(gray==retVal);	
		if(gray!=retVal)
			INFO_PRINTF3(_L("Gray - equal to: %d , Actual: %d"), retVal, gray);
		}
	retVal = TDisplayModeUtils::NumDisplayModeColors(ENone);	
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("TDisplayModeUtils::NumDisplayModeColors(ENone) return value - equal to: %d , Actual: %d"), 0, retVal);	
	CleanupStack::PopAndDestroy(2);		//window and mode-array
	}

void CTWindowTest::TestInvalidFunctions2L()
//
// This code casts windows to group windows and vice-versa and then sends messages for the 'cast' that
// should not be sent to the original type of window. These should all result in panics
//
	{
	for (TInt winType=1;winType<7;++winType)		//Skip type 0 (RWindowTreeNode)
		for (TInt panic=0;;panic++)
			{
			TBool testFinished=EFalse;
			TEST(iTest->TestWsPanicL(DoPanicTest3,EWservPanicOpcode,EWinTypeFactor*winType+panic,(TAny*)iTest->iScreenNumber,&testFinished));
			if (testFinished)
				break;
			}
	iTest->CloseAllPanicWindows();
	}

void CTWindowTest::TestDeletedParentPanics1L()
	{
	for (TInt winType=0;winType<7;++winType)
		{
		if (winType==1)		//Skip type 1 (RWindowGroup)
			++winType;
		for (TInt panic=0;;panic++)
			{
			TBool testFinished=EFalse;
			TEST(iTest->TestWsPanicL(DoPanicTest4,EWservPanicParentDeleted,EWinTypeFactor*winType+panic,(TAny*)iTest->iScreenNumber,&testFinished));
			if (testFinished)
				break;
			}
		}
	iTest->CloseAllPanicWindows();
	}

void CTWindowTest::TestDeletedParentPanics2L()
	{
	for (TInt panic=0;;panic++)
		{
		TBool testFinished=EFalse;
		TEST(iTest->TestWsPanicL(DoPanicTest5,EWservPanicParentDeleted,panic,(TAny*)iTest->iScreenNumber,&testFinished));
		if (testFinished)
			break;
		}
	iTest->CloseAllPanicWindows();
	}

void CTWindowTest::Bug1L()
//
// Test a defect found with WSERV 099 that caused a full Eikon ROM to crash before even the splach screen appeared
//
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	CleanupStack::PushL(screen);
	User::LeaveIfError(screen->Construct(iTest->iScreenNumber));

	RWindowGroup group(ws);
	group.Construct(344);
	group.EnableReceiptOfFocus(EFalse);		//Not done by Eikon, but needed to stop shell window comming to front.
	RBlankWindow blank(ws);
	blank.Construct(group,345);
	blank.SetOrdinalPosition(0,1000);
	blank.Activate();
	RWindow window(ws);
	window.Construct(group,346);

	//Must delete this window to tidy up
	window.Close();
	blank.Close();
	group.Close();

	CleanupStack::PopAndDestroy(screen);
	ws.Close();
	}

void CTWindowTest::TestWindowDelete()
	{
	TInt handles = 344;
	TInt err = KErrNone;
	TInt loop = 0;
	TInt allocFailRate = 0;
	
	RWindowGroup group(TheClient->iWs);
	RWindow parent1(TheClient->iWs);
	RWindow child1(TheClient->iWs);
	RWindow testWindow(TheClient->iWs);
	
	while (loop < 5)
		{
		err = group.Construct(++handles, EFalse);		
		if (err == KErrNone)
			{
			TheClient->iWs.HeapSetFail(RHeap::EDeterministic, allocFailRate);

			//Create parent 1
			err = parent1.Construct(group,++handles);
			if (err == KErrNone)
				{
				parent1.SetExtent(TPoint(10,10),TSize(50,50));
				parent1.Activate();
				}			
			}
		//Create child 1
		if (err == KErrNone)
			{
			err = child1.Construct(parent1,++handles);
			if (err == KErrNone)
				{
				child1.SetExtent(TPoint(),TSize(50,20));
				child1.Activate();
				}
			//Only delete the parent 1, but not the child 1
			parent1.Close();	
			}
		
		if (err == KErrNone) 
			{			
			//Create testWindow
			err = testWindow.Construct(group,++handles);
			if (err == KErrNone)
				{
				testWindow.SetExtent(TPoint(0,20),TSize(50,30));
				testWindow.Activate();
				}
			}
		TheClient->iWs.HeapSetFail(RAllocator::ENone, 0);
			
		child1.Close();
		testWindow.Close();
		group.Close();
		++allocFailRate;
		loop = (err == KErrNone) ? loop + 1 : 0; 
		}
	}
void CTWindowTest::Bug2L()
//
// Test a defect ...
//
	{
	TSize offset(20,20);
	TRect screen(TheClient->iScreen->SizeInPixels()-offset);
	TRgb color;
	RWsSession ws1;
	User::LeaveIfError(ws1.Connect());
	ws1.SetAutoFlush(ETrue);

	// use correct screen
	//
	CWsScreenDevice* scr1 = new (ELeave) CWsScreenDevice(ws1);
	CleanupStack::PushL(scr1);
	User::LeaveIfError(scr1->Construct(iTest->iScreenNumber));

	RWsSession ws2;
	User::LeaveIfError(ws2.Connect());
	ws2.SetAutoFlush(ETrue);

	// use correct screen
	//
	CWsScreenDevice* scr2 = new (ELeave) CWsScreenDevice(ws2);
	CleanupStack::PushL(scr2);
	User::LeaveIfError(scr2->Construct(iTest->iScreenNumber));

	RWindowGroup group1a(ws1);
	group1a.Construct(344);
	group1a.EnableReceiptOfFocus(EFalse);
	group1a.SetOrdinalPosition(0,5);
	RBlankWindow blank1a(ws1);
	blank1a.Construct(group1a,345);
	color=TRgb::Gray4(0);
	blank1a.SetColor(color);
	blank1a.SetExtent(screen.iTl,screen.Size());
	blank1a.EnableBackup();
	blank1a.Activate();

	RWindowGroup group2(ws2);
	group2.Construct(342);
	group2.EnableReceiptOfFocus(EFalse);
	group2.SetOrdinalPosition(0,5);
	RBlankWindow blank2(ws2);
	blank2.Construct(group2,347);
	color=TRgb::Gray4(1);
	blank2.SetColor(color);
	blank2.SetExtent(screen.iTl+TSize(20,0),screen.Size());
	blank2.EnableBackup();
	blank2.Activate();

	RWindowGroup group1b(ws1);
	group1b.Construct(343);
	//group1b.EnableReceiptOfFocus(EFalse);
	group1b.SetOrdinalPosition(0,5);
	RBlankWindow blank1b(ws1);
	blank1b.Construct(group1b,346);
	color=TRgb::Gray4(2);
	blank1b.SetColor(color);
	blank1b.SetExtent(screen.iTl+offset,screen.Size());
	blank1b.EnableBackup();
	blank1b.Activate();

	group1b.Close();
	blank1a.Close();
	blank1b.Close();
	blank2.Close();
	group1a.Close();
	group2.Close();

	CleanupStack::PopAndDestroy(2,scr1);
	ws1.Close();
	ws2.Close();
	}

void CTWindowTest::Bug3L()
//
// Actiate then make visible and backup behind window caused panic.
//
	{
	TSize offset(20,20);
	TRect screen(TheClient->iScreen->SizeInPixels()-offset);
	TRgb color;
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	ws.SetAutoFlush(ETrue);

	// use correct screen
	//
	CWsScreenDevice* scr = new (ELeave) CWsScreenDevice(ws);
	CleanupStack::PushL(scr);
	User::LeaveIfError(scr->Construct(iTest->iScreenNumber));

	RWindowGroup group(ws);
	group.Construct(348);
	group.EnableReceiptOfFocus(EFalse);
	RBlankWindow blank1(ws);
	blank1.Construct(group,341);
	color=TRgb::Gray4(1);
	blank1.SetColor(color);
	blank1.SetExtent(screen.iTl,screen.Size());
	blank1.EnableBackup();
	blank1.SetVisible(EFalse);
	blank1.Activate();
	blank1.SetVisible(ETrue);
	RBlankWindow blank2(ws);
	blank2.Construct(group,342);
	color=TRgb::Gray4(2);
	blank2.SetColor(color);
	blank2.SetExtent(screen.iTl,screen.Size());
	blank2.EnableBackup();
	blank2.SetVisible(EFalse);
	blank2.SetVisible(ETrue);
	blank2.Activate();
	group.Close();
	blank1.Close();
	blank2.Close();

	CleanupStack::PopAndDestroy(scr);
	ws.Close();
	}

void CTWindowTest::ErrorCodesL()
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	CWsScreenDevice* scr = new (ELeave) CWsScreenDevice(ws);
	CleanupStack::PushL(scr);
	User::LeaveIfError(scr->Construct(iTest->iScreenNumber));


	RWindowGroup group(ws);
	group.Construct(349);
	group.EnableReceiptOfFocus(EFalse);
	RWindow window(TheClient->iWs);
	User::LeaveIfError(window.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	TInt retVal = window.MoveToGroup(22222);		
	TEST(retVal==KErrNotFound);
	if (retVal!=KErrNotFound)
		INFO_PRINTF3(_L("window.MoveToGroup(22222) return value - equal to: %d , Actual: %d"), KErrNotFound, retVal);
	retVal = window.MoveToGroup(group.Identifier());	
	TEST(retVal==KErrNotFound);
	if (retVal!=KErrNotFound)
		INFO_PRINTF3(_L("window.MoveToGroup(group.Identifier()) return value - equal to: %d , Actual: %d"), KErrNotFound, retVal);
	window.Close();
	group.Close();

	CleanupStack::PopAndDestroy(scr);
	ws.Close();
	}

void CTWindowTest::BackColorBugL()
//
// Test a defect with window being drawn with the wrong background color when they are moved on the screen
//
	{
	TSize scrSize(TheClient->iScreen->SizeInPixels());
	TSize checkSize(12,10);			//X-Size needs to be multiple of 4 due to BITGDI change/defect
	CBlankWindow* win;
	win=new(ELeave) CBlankWindow(TRgb(16,16,240));
	CleanupStack::PushL(win);
	win->SetUpL(TPoint(5,5),scrSize-TSize(10,10),TheClient->iGroup,*TheClient->iGc);
	TInt mode=win->BaseWin()->SetRequiredDisplayMode(EColor256);
	const TDisplayMode actualMode=reinterpret_cast<TDisplayMode&>(mode);
	if (!TDisplayModeUtils::IsDisplayModeColor(actualMode) || TDisplayModeUtils::NumDisplayModeColors(actualMode)<256)
		{
		CleanupStack::PopAndDestroy(win);
		return;
		}
	win->RealDraw(ETrue);
	win->Win()->SetBackgroundColor(TRgb(64,224,64));
	CBlankWindow* win3;
	win3=new(ELeave) CBlankWindow(TRgb::Gray16(8));
	CleanupStack::PushL(win3);
	win3->SetUpL(TPoint(12,12),checkSize,TheClient->iGroup,*TheClient->iGc);
	win3->BaseWin()->SetRequiredDisplayMode(EColor256);
	CBlankWindow* win2;
	win2=new(ELeave) CBlankWindow(TRgb(240,16,16));
	CleanupStack::PushL(win2);
	win2->SetUpL(TPoint(10,scrSize.iHeight/2),TSize(scrSize.iWidth/3,scrSize.iHeight/2-10),TheClient->iGroup,*TheClient->iGc);
	win2->BaseWin()->SetRequiredDisplayMode(EColor256);
	win2->RealDraw(EFalse);

	win2->Win()->SetBackgroundColor(TRgb::Gray16(8));
	win2->SetExt(TPoint(scrSize.iWidth/4,30),TSize(scrSize.iWidth/2,2*scrSize.iHeight/3));
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();
	if (!CheckRect(win2,win3,TRect(checkSize)))
		{
		_LIT(KLog,"After window is moved and resizes it doesn't matches the other window");
		LOG_MESSAGE(KLog);
		//Code to save a screen shot useful if this test fails
		/*_LIT(KTest,"E:\\logs\\testexecute\\Window%d");
		TBuf<64> buf;
		buf.Format(KTest,iTest->iState);
		TInt err=iTest->SaveScreen(buf);
		_LIT(KLogSave,"Saved screenshot to file %S, (err=%d)");
		LOG_MESSAGE3(KLogSave,&buf,err);*/
		}
	TheClient->WaitForRedrawsToFinish();
	CleanupStack::PopAndDestroy(3,win);
	}

void CTWindowTest::FocusChangedL()
//
// Test that the focus change is available when the redraw is
// it should be available before, but it isn't always possible to test that
//
	{
	_LIT(KThreadName,"MoveGroup");
	_LIT(KEventWaiting,"Event Waiting when none expected.");
	TInt command;
	iThreadParam.iScreenNumber=iTest->iScreenNumber;
	iFirstFunction=TThreadStartUp(CTWindowTest::MoveGroup,&iThreadParam);
	TRequestStatus status;
	TSize scrSize(TheClient->iScreen->SizeInPixels());
	CTWindowGroup* group1=new(ELeave) CTWindowGroup(TheClient);
	group1->ConstructL();
	CleanupStack::PushL(group1);
	TInt winId1=group1->GroupWin()->Identifier();
	CBlankWindow* win1=new(ELeave) CBlankWindow(TRgb::Gray4(1));
	CleanupStack::PushL(win1);
	win1->SetUpL(TPoint(1,1),TSize(2*scrSize.iWidth/3,2*scrSize.iHeight/3),group1,*TheClient->iGc);
	win1->RealDraw(ETrue);
	win1->Win()->SetBackgroundColor(TRgb::Gray4(3));
	CTWindowGroup* group2=new(ELeave) CTWindowGroup(TheClient);
	group2->ConstructL();
	CleanupStack::PushL(group2);
	TInt winId2=group2->GroupWin()->Identifier();
	CBlankWindow* win2=new(ELeave) CBlankWindow(TRgb::Gray4(2));
	CleanupStack::PushL(win2);
	win2->SetUpL(TPoint(scrSize.iWidth/3-5,scrSize.iHeight/3-5),TSize(2*scrSize.iWidth/3,2*scrSize.iHeight/3),group2,*TheClient->iGc);
	win2->RealDraw(ETrue);
	win2->Win()->SetBackgroundColor(TRgb::Gray4(0));

	TheClient->WaitForRedrawsToFinish();
	TheClient->WaitForAllEventProcessingToFinish();
	command=winId1;
	iThreadParam.iCommand=command;
	TBool noEventWaiting=!TheClient->IsEventWaiting();
	TEST(noEventWaiting);
	if (!noEventWaiting)
		LOG_MESSAGE(KEventWaiting);
	_LIT(KTest1,"FocusChangedL: 1st test completed");
	LOG_MESSAGE(KTest1);
	CProcess* iMoveGroup=CProcess::NewSimpleThreadL(KThreadName,&iFirstFunction,&status);
	User::WaitForRequest(status);
	delete iMoveGroup;
	TEST(TheClient->WaitForEvent());
	TheClient->WaitForAllEventProcessingToFinish();

	command=winId2;
	iThreadParam.iCommand=command;
	TheClient->WaitForRedrawsToFinish();
	noEventWaiting=!TheClient->IsEventWaiting();
	TEST(noEventWaiting);
	if (!noEventWaiting)
		LOG_MESSAGE(KEventWaiting);
	_LIT(KTest2,"FocusChangedL: 2nd test completed");
	LOG_MESSAGE(KTest2);
	iMoveGroup=CProcess::NewSimpleThreadL(KThreadName,&iFirstFunction,&status);
	User::WaitForRequest(status);
	delete iMoveGroup;
	TEST(TheClient->WaitForEvent());
	TheClient->WaitForAllEventProcessingToFinish();

	command=winId2|(1<<EPosShift);
	iThreadParam.iCommand=command;
	TheClient->WaitForRedrawsToFinish();
	noEventWaiting=!TheClient->IsEventWaiting();
	TEST(noEventWaiting);
	if (!noEventWaiting)
		LOG_MESSAGE(KEventWaiting);
	_LIT(KTest3,"FocusChangedL: 3rd test completed");
	LOG_MESSAGE(KTest3);
	iMoveGroup=CProcess::NewSimpleThreadL(KThreadName,&iFirstFunction,&status);
	User::WaitForRequest(status);
	delete iMoveGroup;
	TEST(TheClient->WaitForEvent());
	TheClient->WaitForAllEventProcessingToFinish();

	command=winId1|(1<<EPosShift);
	iThreadParam.iCommand=command;
	TheClient->WaitForRedrawsToFinish();
	noEventWaiting=!TheClient->IsEventWaiting();
	TEST(noEventWaiting);
	if (!noEventWaiting)
		LOG_MESSAGE(KEventWaiting);
	_LIT(KTest4,"FocusChangedL: 4th test completed");
	LOG_MESSAGE(KTest4);
	iMoveGroup=CProcess::NewSimpleThreadL(KThreadName,&iFirstFunction,&status);
	User::WaitForRequest(status);
	delete iMoveGroup;
	TEST(TheClient->WaitForEvent());
	TheClient->WaitForAllEventProcessingToFinish();

	TheClient->WaitForRedrawsToFinish();
	noEventWaiting=!TheClient->IsEventWaiting();
	TEST(noEventWaiting);
	if (!noEventWaiting)
		LOG_MESSAGE(KEventWaiting);
	_LIT(KTest5,"FocusChangedL: 5th and last test completed");
	LOG_MESSAGE(KTest5);
	CleanupStack::PopAndDestroy(2,group2);
	TheClient->Flush();
	TEST(TheClient->WaitForEvent());
	TheClient->WaitForAllEventProcessingToFinish();
	CleanupStack::PopAndDestroy(2,group1);
	}

#define ALT_PRI 78
void CTWindowTest::EventsToAllL()
	{
	CTClient* client1=new(ELeave) CTClient;
	CleanupStack::PushL(client1);
	client1->SetScreenNumber(iTest->iScreenNumber);
	client1->ConstructL();
	CTClient* client2=new(ELeave) CTClient;
	CleanupStack::PushL(client2);
	client2->SetScreenNumber(iTest->iScreenNumber);
	client2->ConstructL();
	CEvWindowGroup* group1a=CEvWindowGroup::NewLC(client1,this);
	CEvWindowGroup* group1b=CEvWindowGroup::NewLC(client1,this);
	CEvWindowGroup* group2a=CEvWindowGroup::NewLC(client2,this);
	CEvWindowGroup* group2b=CEvWindowGroup::NewLC(client2,this);
	/*_LIT(KLog,"Window Group Id's: %d,%d,%d,%d");
	LOG_MESSAGE5(KLog,group1a->GroupWin()->Identifier(),group1b->GroupWin()->Identifier(),group2a->GroupWin()->Identifier(),group2b->GroupWin()->Identifier());*/
	group1a->SendEvent(KBaseUserEvent+1);
	group2b->SendEvent(KBaseUserEvent+2);
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	TWsEvent event;
	// Assigned random value because before PREQ1226, TWsevent's data wasn't zero initialised.
	// In techview, following function
	// void CEikServAppUi::HandleSystemEventL(const TWsEvent& aEvent)
	// was relaying on TWsevent's data not be zero 
	*(event.Int()) = 0XCCCCCCCC;
	event.SetType(KBaseUserEvent+3);
	group1a->SetExpectedEvent(KBaseUserEvent+3);
	group1b->SetExpectedEvent(KBaseUserEvent+3);
	group2a->SetExpectedEvent(KBaseUserEvent+3);
	group2b->SetExpectedEvent(KBaseUserEvent+3);
	TheClient->iWs.SendEventToAllWindowGroups(event);
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	event.SetType(KBaseUserEvent+4);
	group1a->SetExpectedEvent(KBaseUserEvent+4);
	group1b->SetExpectedEvent(KBaseUserEvent+4);
	group2a->SetExpectedEvent(KBaseUserEvent+4);
	group2b->SetExpectedEvent(KBaseUserEvent+4);
	TheClient->iWs.SendEventToAllWindowGroups(event);
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	event.SetType(KBaseUserEvent+5);
	//group1a->SetExpectedEvent(KBaseUserEvent+5);
	group1b->SetExpectedEvent(KBaseUserEvent+5);
	//group2a->SetExpectedEvent(KBaseUserEvent+5);
	group2b->SetExpectedEvent(KBaseUserEvent+5);
	TheClient->iWs.SendEventToOneWindowGroupsPerClient(event);
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	group1a->WinTreeNode()->SetOrdinalPosition(0);
	client1->Flush();
	event.SetType(KBaseUserEvent+6);
	group1a->SetExpectedEvent(KBaseUserEvent+6);
	group2b->SetExpectedEvent(KBaseUserEvent+6);
	TheClient->iWs.SendEventToOneWindowGroupsPerClient(event);
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	group2b->WinTreeNode()->SetOrdinalPosition(6);
	client2->Flush();
	event.SetType(KBaseUserEvent+7);
	group1a->SetExpectedEvent(KBaseUserEvent+7);
	group2a->SetExpectedEvent(KBaseUserEvent+7);
	TheClient->iWs.SendEventToOneWindowGroupsPerClient(event);
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	event.SetType(KBaseUserEvent+8);
	group1a->SetExpectedEvent(KBaseUserEvent+8);
	group1b->SetExpectedEvent(KBaseUserEvent+8);
	group2a->SetExpectedEvent(KBaseUserEvent+8);
	group2b->SetExpectedEvent(KBaseUserEvent+8);
	TheClient->iWs.SendEventToAllWindowGroups(0,event);
	group1a->WinTreeNode()->SetOrdinalPosition(0,ALT_PRI);
	client1->Flush();
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	event.SetType(KBaseUserEvent+9);
	group1a->SetExpectedEvent(KBaseUserEvent+9);
	TheClient->iWs.SendEventToAllWindowGroups(ALT_PRI,event);
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	group2b->WinTreeNode()->SetOrdinalPosition(0,ALT_PRI);
	client2->Flush();
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	event.SetType(KBaseUserEvent+10);
	group1a->SetExpectedEvent(KBaseUserEvent+10);
	group2b->SetExpectedEvent(KBaseUserEvent+10);
	TheClient->iWs.SendEventToAllWindowGroups(ALT_PRI,event);
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	event.SetType(KBaseUserEvent+11);
	group1b->SetExpectedEvent(KBaseUserEvent+11);
	group2a->SetExpectedEvent(KBaseUserEvent+11);
	TheClient->iWs.SendEventToAllWindowGroups(0,event);
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	event.SetType(KBaseUserEvent);
	group1a->SetExpectedEvent(KBaseUserEvent);
	group1b->SetExpectedEvent(KBaseUserEvent);
	group2a->SetExpectedEvent(KBaseUserEvent);
	group2b->SetExpectedEvent(KBaseUserEvent);
	TheClient->iWs.SendEventToAllWindowGroups(event);
	client1->WaitForAllEventProcessingToFinish();
	client2->WaitForAllEventProcessingToFinish();
	CleanupStack::PopAndDestroy(6,client1);
	}

void DestroyWindow(TAny* aWindow)
	{
	static_cast<RWindowTreeNode*>(aWindow)->Destroy();
	}

void CTWindowTest::GroupIdL()
	{
	const TInt numWindows=10;
	RWindowGroup group(TheClient->iWs);
	CleanupClosePushL(group);
	User::LeaveIfError(group.Construct(ENullWsHandle));
	TInt id=group.Identifier();
	RWindowTreeNode* prevWindow=&group;
	TInt ii;
	for (ii=0;ii<numWindows;++ii)
		{
		RBlankWindow* window=new(ELeave) RBlankWindow(TheClient->iWs);
		CleanupStack::PushL(TCleanupItem(&DestroyWindow,window));
		User::LeaveIfError(window->Construct(*prevWindow,ENullWsHandle));
		TInt retVal = window->WindowGroupId();		
		TEST(retVal==id);
		if (retVal!=id)
			INFO_PRINTF3(_L("window->WindowGroupId() return value  - equal to: %d , Actual: %d"), id, retVal);
		prevWindow=window;
		}
	CleanupStack::PopAndDestroy(numWindows+1);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0495

@SYMDEF				PDEF131541

@SYMTestCaseDesc	Sending Events using one of the functions SendEventToWindowGroup, SendEventToAllWindowGroups (x2) 
					and SendEventToOneWindowGroupsPerClient when an event queue is full returns an error

@SYMTestPriority	Low

@SYMTestStatus		Implemented

@SYMTestActions		Call the functions repeatly many times and check that an error gets returned eventually

@SYMTestExpectedResults		Functions return the error KErrNoMemory
*/
const TInt numTest=75;		//Must be at least 33
void CTWindowTest::SaturateSendEvent()
	{
	const TInt id=TheClient->iGroup->GroupWin()->Identifier();
	TWsEvent event;
	event.SetType(KBaseUserEvent);
	TInt ii;

	TInt err=KErrNone;
	for (ii=0; ii<=numTest && err==KErrNone;)
		{
		++ii;
		err=TheClient->iWs.SendEventToWindowGroup(id,event);
		}
	TEST(err==KErrNoMemory);
	if (err!=KErrNoMemory)
		{
		_LIT(KLog,"After %d/%d iterations SendEventToWindowGroup returned the error %d");
		LOG_MESSAGE4(KLog,ii,numTest,err);
		}
	TheClient->WaitForAllEventProcessingToFinish();

	err=KErrNone;
	for (ii=0; ii<=numTest && err==KErrNone;)
		{
		++ii;
		err=TheClient->iWs.SendEventToAllWindowGroups(event);
		}
	TEST(err==KErrNoMemory);
	if (err!=KErrNoMemory)
		{
		_LIT(KLog,"After %d/%d iterations SendEventToAllWindowGroups returned the error %d");
		LOG_MESSAGE4(KLog,ii,numTest,err);
		}
	TheClient->WaitForAllEventProcessingToFinish();
	User::After(1000000);		//1sec, give other sessions a chance to respond to the events

	err=KErrNone;
	for (ii=0; ii<=numTest && err==KErrNone;)
		{
		++ii;
		err=TheClient->iWs.SendEventToAllWindowGroups(0,event);
		}
	TEST(err==KErrNoMemory);
	if (err!=KErrNoMemory)
		{
		_LIT(KLog,"After %d/%d iterations SendEventToAllWindowGroups_Priority0 returned the error %d");
		LOG_MESSAGE4(KLog,ii,numTest,err);
		}
	TheClient->WaitForAllEventProcessingToFinish();
	User::After(1000000);		//1sec, give other sessions a chance to respond to the events

	err=KErrNone;
	for (ii=0; ii<=numTest && err==KErrNone;)
		{
		++ii;
		err=TheClient->iWs.SendEventToOneWindowGroupsPerClient(event);
		}
	TEST(err==KErrNoMemory);
	if (err!=KErrNoMemory)
		{
		_LIT(KLog,"After %d/%d iterations SendEventToOneWindowGroupsPerClient returned the error %d");
		LOG_MESSAGE4(KLog,ii,numTest,err);
		}
	TheClient->WaitForAllEventProcessingToFinish();
	User::After(1000000);		//1sec, give other sessions a chance to respond to the events
	}

void CTWindowTest::TestNotSupportedFunctionality()
    {
    TInt err = KErrNone;
    TSize windowSize = TSize(10,10);
    TSize testSize = TSize();
    
    RWindowGroup group(TheClient->iWs);
    RWindow window(TheClient->iWs);

    err = group.Construct((TUint32)&group, EFalse);
    TEST(err==KErrNone);
    err = window.Construct(group,(TUint32)&window);
    TEST(err==KErrNone);
    
    window.SetExtent(TPoint(0,0),windowSize);
    window.Activate();
    
    testSize = window.Size();
    TEST((testSize.iWidth==windowSize.iWidth) && (testSize.iHeight==windowSize.iHeight));
    
    testSize = window.Size();
    TEST((testSize.iWidth==windowSize.iWidth) && (testSize.iHeight==windowSize.iHeight));
    
    window.Close();
    group.Close();  
    }

void CTWindowTest::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest1,"Window");
	_LIT(KTest2,"DestroyWindowWithActiveGc");
	_LIT(KTest3,"Shadow/NoAutoClear");
	_LIT(KTest4,"Corner Tests");
	_LIT(KTest5,"Invalid Window Functions");
	_LIT(KTest7,"Mega-Tree");
	_LIT(KTest8,"Tiled Window One");
	_LIT(KTest9,"Tiled Window Two");
	_LIT(KTest10,"Color Test");
	_LIT(KTest11,"Invalid Window Functions2");
	_LIT(KTest12,"Defect 1");
	_LIT(KTest13,"Defect 2");
	_LIT(KTest14,"Parent Deleted 1");
	_LIT(KTest15,"Parent Deleted 2");
	_LIT(KTest16,"Defect 3");
	_LIT(KTest17,"Background Color");
	_LIT(KTest18,"Focus Changed");
	_LIT(KTest21,"Events To All");
	_LIT(KTest22,"Error Codes");
	_LIT(KTest23,"Group Id");
	_LIT(KTest24,"DeleteParentWindowOnly");
#if defined(_DEBUG)
	_LIT(KTest25,"DuplicateWindowHandles");
#else
	_LIT(KTest25skipped,"DuplicateWindowHandles skipped");
#endif

	((CTWindowTestStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);

	_LIT(KTest26,"Saturate SendEvent");
	_LIT(KTest27,"Not Supported Functionality");

	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0029

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test window can be created and destroyed correctly

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates and destroys a window

@SYMTestExpectedResults Window is created and destroyed without error
*/
		case 1:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0029"));
			iTest->LogSubTest(KTest1);
			CreateTestWindowL();
			CheckAndDestroyWindows();
			//iState=17;
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0030

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Creates and destroys window with an active gc

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create and destroy a window with an active gc

@SYMTestExpectedResults Window is created and destroyed without error
*/
		case 2:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0030"));
			iTest->LogSubTest(KTest2);
			CreateTestWindowL();
			DestroyWindowWithActiveGc();
			CreateTestWindowL();
			DestroyWindowWithActiveGc2L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0031

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test that shadow is not automatically cleared when
					drawing a window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw windows and check that the shadow is not automatically
					cleared

@SYMTestExpectedResults Screen comparison returns that the shadow was not cleared
*/
		case 3:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0031"));
			//window shadowing is no longer supported. keep the test to make sure clients can still call the methods
			iTest->LogSubTest(KTest3);
			ShadowAutoClearTest();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0032

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test the drawing of different types of corner of
					a window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw a window with different types of corner

@SYMTestExpectedResults The window is drawn correctly for each corner type
*/
		case 4:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0032"));
			iTest->LogSubTest(KTest4);
			CornerTests();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0033

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test for panics when window is sent wrong requests

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send wrong requests to windows and deal with panic

@SYMTestExpectedResults The windows panic as expected
*/
		case 5:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0033"));
			iTest->LogSubTest(KTest5);
			TestInvalidFunctionsL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0035

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Draw 100 windows from a thread

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw 100 windows from the same thread

@SYMTestExpectedResults The windows are all drawn without error
*/
		case 6:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0035"));
			iTest->LogSubTest(KTest7);
			CreateMegaTree();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0036

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Check that windows can be drawn in a tiled format

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw two windows in a tiled format and check they
					are identical

@SYMTestExpectedResults The tiled windows are identical
*/
		case 7:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0036"));

			iTest->LogSubTest(KTest8);
			TiledWindowTestL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0037

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Check that windows and dialogs can be drawn in a
					tiled format

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw windows and dialogs in a tiled format

@SYMTestExpectedResults The windows and dialogs are drawn correctly
*/
		case 8:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0037"));
			iTest->LogSubTest(KTest9);
			TiledWindowTest2L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0038

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Check drawing into a window with different
					colour set ups

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw in a window using different colour
					configurations

@SYMTestExpectedResults The different colours are drawn correctly
*/
		case 9:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0038"));
			iTest->LogSubTest(KTest10);
			ColorTestL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0039

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test for panics when window is sent wrong requests

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send wrong requests to windows and deal with panic

@SYMTestExpectedResults The windows panic as expected
*/
		case 10:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0039"));
			iTest->LogSubTest(KTest11);
			TestInvalidFunctions2L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0040

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test for a defect that causes ROM to crash

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test a defect is not present which causes the ROM
					to crash

@SYMTestExpectedResults The defect is not present
*/
		case 11:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0040"));
			iTest->LogSubTest(KTest12);
			Bug1L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0041

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test a previous defect has not returned

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Exercise the code the defect was discovered in

@SYMTestExpectedResults The defect is not present
*/
		case 12:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0041"));
			iTest->LogSubTest(KTest13);
			Bug2L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0042

@SYMDEF  			DEF081259
					DEF115543

@SYMTestCaseDesc    Test that when a parent window is deleted a panic
					occurs

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Delete a parent window and check for a panic
					NOTE: DEF115543 has corrected GRAPHICS-WSERV-0042, and added a lot
					more window functions.

@SYMTestExpectedResults All functions either:
					Panic the owner thread with EWservPanicParentDeleted or
					Get performed without accessing any NULL iParent pointers in the WSERV thread
*/
		case 13:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0042"));
			iTest->LogSubTest(KTest14);
			TestDeletedParentPanics1L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0043

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test that when a parent window is deleted a panic
					occurs

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Delete a parent window and check for a panic

@SYMTestExpectedResults The panic occurs as expected
*/
		case 14:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0043"));
			iTest->LogSubTest(KTest15);
			TestDeletedParentPanics2L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0044

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Check that activate then make visible and backup
					behind a window does not panic.

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Activate then make visible and backup
					behind a window

@SYMTestExpectedResults No panic occurs
*/
		case 15:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0044"));
			iTest->LogSubTest(KTest16);
			Bug3L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0045

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test window being drawn with the correct background
					color when they are moved on the screen

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Move window on the screen and check it is drawn with
					the correct background colour

@SYMTestExpectedResults The background coloour is correct
*/
		case 16:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0045"));
			iTest->LogSubTest(KTest17);
			BackColorBugL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0046

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test that the focus change is available after redraw

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Redraw and check the focus change is available

@SYMTestExpectedResults The focus change is available
*/
		case 17:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0046"));
			iTest->LogSubTest(KTest18);
			FocusChangedL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0048

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test that events can be sent to a number of window
					groups simultaneously

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Send events to a number of window groups and check
					that they all receive them

@SYMTestExpectedResults The events and sent to the window groups correctly
*/
		case 18:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0048"));
			iTest->LogSubTest(KTest21);
			EventsToAllL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0049

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test error code when incorrectly moving a window to
					a group.

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Try to move a window to a group and check the error
					codes

@SYMTestExpectedResults The correct error codes are returned
*/
		case 19:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0049"));
			iTest->LogSubTest(KTest22);
			ErrorCodesL();
/**
@SYMTestCaseID		GRAPHICS-WSERV-0050

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Check that the correct group id is assigned to a
					chain of windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create a chain of windows in the same group and check
					the all have the same group id

@SYMTestExpectedResults The windows all have the same group id
*/
		case 20:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0050"));
			iTest->LogSubTest(KTest23);
			GroupIdL();
			break;
			
/**
@SYMTestCaseID		GRAPHICS-WSERV-0461

@SYMDEF  			PDEF114190

@SYMTestCaseDesc    Test window redraw queue cleanup when window is deleted in low memory conditions

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Have a loop which increases the number of allocations in the server thread before failure;
					Within the loop:
					1) Create a parent window and a child window of the parent; 
					2) Delete the parent window only, but not the child window; 
					3) Create a testWindow. This new window gets added to the redraw queue which will force it 
					reordering. This would crash WSERV due to this defect because the orphaned window (which is 
					the previous child window) is still in the redraw queue
					4) Delete all the windows involved;

@SYMTestExpectedResults		The window redraw queue should be cleaned up when the first window is deleted;
							WSERV should not crash. The test should pass. 
*/		
		case 21:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0461"));
			iTest->LogSubTest(KTest24);
			TestWindowDelete();
			break;
			
/**
@SYMTestCaseID		GRAPHICS-WSERV-0463

@SYMDEF				DEF115601

@SYMTestCaseDesc    Two RWindowGroup objects using same window client handle cause Emulator crash

@SYMTestPriority    Medium

@SYMTestStatus      Implemented

@SYMTestActions     Create two window groups with the same handle.

@SYMTestExpectedResults		WSERV should panic the client thread with the code EWservPanicDuplicateHandle
							and then destroy the window groups without crashing WSERV
*/
		case 22:
#if defined(_DEBUG)
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0463"));
			iTest->LogSubTest(KTest25);
			TEST(iTest->TestWsPanicL(DoPanicTest6,EWservPanicDuplicateHandle,0,(TAny*)iTest->iScreenNumber,NULL));
			iTest->CloseAllPanicWindows();
#else
			iTest->LogSubTest(KTest25skipped);	//Client side panic will only occur in debug builds of WServ
#endif
			break;
		case 23:
			((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0495"));
			iTest->LogSubTest(KTest26);
			SaturateSendEvent();
			break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0655

@SYMDEF             ou1cimx1#329309

@SYMTestCaseDesc    Fix native orientation functionality not supported

@SYMTestPriority    Medium

@SYMTestStatus      Implemented

@SYMTestActions     Call fucntions related to fix native orientation on a window

@SYMTestExpectedResults     Fix native orientation functionality not supported
			*/			
        case 24:
            ((CTWindowTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0655"));
            iTest->LogSubTest(KTest27);
            TestNotSupportedFunctionality();
            break;			
		default:
            		((CTWindowTestStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTWindowTestStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTWindowTestStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(WindowTest)
