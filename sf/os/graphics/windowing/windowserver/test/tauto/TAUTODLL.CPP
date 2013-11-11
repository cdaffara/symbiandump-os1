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
// Client side of AUTODLL test code
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TAUTODLL.H"
#include "TEVENT.H"

//#define LOGGING

//


CGraphicsContext::TDrawMode CAnimWindow::iEllipseDrawMode;

CTAnimDll::CTAnimDll(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}

RTestAnim::RTestAnim() : RAnim()
	{
	}

RTestAnim::RTestAnim(RAnimDll &aDll) : RAnim(aDll)
	{
	}

LOCAL_C TInt DoPanicTest(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());

	// assign to the correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));

	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(333));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	RAnimDll animDll=RAnimDll(ws);
	User::LeaveIfError(animDll.Load(KAnimDLLName));
	RTestAnim panic(animDll);
	RWindow window(ws);
	RBlankWindow blankwin(ws);
	if (aInt==16 || aInt==17)	// Use a blank window for this one
		{
		User::LeaveIfError(blankwin.Construct(group,123));
		blankwin.SetSize(TSize(10,10));
		blankwin.Activate();
		User::LeaveIfError(panic.Construct(blankwin,EAnimTypeTest1,TPtrC8()));
		}
	else
		{
		User::LeaveIfError(window.Construct(group,123));
		window.SetSize(TSize(10,10));
		window.Activate();
		window.BeginRedraw();
		window.EndRedraw();
		if (aInt==0)
			User::LeaveIfError(panic.Construct(window,EAnimTypeTest2,TPtrC8()));
		User::LeaveIfError(panic.Construct(window,EAnimTypeTest1,TPtrC8()));
		}
	TPckgBuf<TPoint> dummyParam;
	switch(aInt)
		{
		case 1:
			panic.Command(EADllPanicCallPanic);
			break;
		case 2:
			panic.Command(EADllPanicCallPanic,dummyParam);
			break;
		case 3:
			panic.CommandReply(EADllPanicCallPanic);
			break;
		case 4:
			panic.CommandReply(EADllPanicCallPanic,dummyParam);
			break;
		case 5:
			panic.Command(EADllPanicSetInterval);
			break;
		case 6:
			panic.Command(EADllPanicSetNextInterval);
			break;
		case 7:
			panic.CommandReply(EADllPanicBadFont);
			break;
		case 8:
			panic.CommandReply(EADllPanicBadBitmap);
			break;
		case 9:
			panic.Command(EADllPanicLeave);
			break;
		case 10:
			panic.Command(EADllPanicDoubleActivateGC);
			break;
		case 11:
			panic.Command(EADllPanicDrawRectWithNoGc);
			break;
		case 12:
			panic.Command(EADllPanicDrawTextWithNoFont);
			break;
		case 13:
			panic.Command(EADllPanicLeaveInAnimate);
			ws.Finish();
			panic.CommandReply(EADllBlankOpCode);
			break;
		case 14:
			panic.Command(EADllPanicLeaveInRedraw);
			window.BeginRedraw();
			window.EndRedraw();
			ws.Finish();
			panic.CommandReply(EADllBlankOpCode);
			break;
		case 15:
			ws.TestWrite(animDll.WsHandle(),999,NULL,0);
			break;
		case 16:
			panic.Command(EADllPanicSetVisWithGcActive);
			break;
		case 17:
			panic.CommandReply(EADllPanicSetVisWithGcActiveReply);
			break;
		case 18:
			panic.Command(EADllPanicLeaveInActiveCallback);
			ws.Flush();
			User::After(2000000);
			panic.CommandReply(EADllBlankOpCode);
			break;
		case 19:
			panic.Command(EADllPanicSetClippingRectWithNoGc);
			break;
		case 20:
			panic.CommandReply(EADllPanicSetClippingRegionWithNoGc);
			break;
		case 21:
			panic.Command(EADllPanicCancelClippingRegionWithNoGc);
			break;
		case 22:
			panic.Command(EADllPanicCancelClippingRectWithNoGc);
			break;
		case 23:
			panic.Command(EADllPanicSetDrawModeWithNoGc);
			break;
		case 24:
			panic.Command(EADllPanicUseFontWithNoGc);
			break;
		case 25:
			panic.Command(EADllPanicDiscardFontWithNoGc);
			break;
		case 26:
			panic.Command(EADllPanicSetUnderlineStyleWithNoGc);
			break;
		case 27:
			panic.Command(EADllPanicSetStrikeThoughStyleWithNoGc);
			break;
		case 28:
			panic.Command(EADllPanicSetWordJustificationWithNoGc);
			break;
		case 29:
			panic.Command(EADllPanicSetCharJustificationWithNoGc);
			break;
		case 30:
			panic.Command(EADllPanicSetPenColorWithNoGc);
			break;
		case 31:
			panic.Command(EADllPanicSetPenStyleWithNoGc);
			break;
		case 32:
			panic.Command(EADllPanicSetPenSizeWithNoGc);
			break;
		case 33:
			panic.Command(EADllPanicSetBrushColorWithNoGc);
			break;
		case 34:
			panic.Command(EADllPanicSetBrushStyleWithNoGc);
			break;
		case 35:
			panic.Command(EADllPanicSetBrushOriginWithNoGc);
			break;
		case 36:
			panic.Command(EADllPanicUseBrushPatterWithNoGc);
			break;
		case 37:
			panic.Command(EADllPanicDiscardBrushPatternWithNoGc);
			break;
		case 38:
			panic.Command(EADllPanicSetFadedWithNoGc);
			break;
		case 39:
			panic.Command(EADllPanicSetFadingParametersWithNoGc);
			break;
		case 40:
			panic.Command(EADllPanicDrawArcWithNoGc);
			break;
		case 41:
			panic.Command(EADllPanicDrawPieWithNoGc);
			break;
		case 42:
			panic.Command(EADllPanicDrawLineWithNoGc);
			break;
		case 43:
			panic.Command(EADllPanicDrawLineToWithNoGc);
			break;
		case 44:
			panic.Command(EADllPanicDrawLineByWithNoGc);
			break;
		case 45:
			panic.Command(EADllPanicDrawEllipseWithNoGc);
			break;
		case 46:
			panic.Command(EADllPanicDrawRoundedRecWithNoGc);
			break;
		case 47:
			panic.Command(EADllPanicClearWithRectWithNoGc);
			break;
		case 48:
			panic.Command(EADllPanicClearWithNoGc);
			break;
		case 49:
			panic.Command(EADllPanicBitBltWithNoGc);
			break;
		case 50:
			panic.Command(EADllPanicBitBltWithRectWithNoGc);
			break;
		case 51:
			panic.Command(EADllPanicBitBltMaskedWithNoGc);
			break;
		case 52:
			panic.Command(EADllPanicDrawBitmapPointWithNoGc);
			break;
		case 53:
			panic.Command(EADllPanicDrawBitmapWithNoGc);
			break;
		case 54:
			panic.Command(EADllPanicDrawBitmapRectWithNoGc);
			break;
		case 55:
			panic.Command(EADllPanicDrawBitmapMaskedFbsBitmapWithNoGc);
			break;
		case 56:
			panic.Command(EADllPanicDrawBitmapMaskedWsBitmapWithNoGc);
			break;
		case 57:
			panic.Command(EADllPanicDrawPolyLinePointsWithNoGc);
			break;
		case 58:
			panic.Command(EADllPanicDrawPolyLineArrayWithNoGc);
			break;
		case 59:
			panic.CommandReply(EADllPanicDrawPolygonPointsWithNoGc);
			break;
		case 60:
			panic.CommandReply(EADllPanicDrawPolygonArrayWithNoGc);
			break;
		case 61:
			panic.Command(EADllPanicMoveToWithNoGc);
			break;
		case 62:
			panic.Command(EADllPanicMoveByWithNoGc);
			break;
		case 63:
			panic.Command(EADllPanicPlotWithNoGc);
			break;
		case 64:
			panic.Command(EADllPanicSetOriginWithNoGc);
			break;
		case 65:
			panic.Command(EADllPanicCopyRectWithNoGc);
			break;
		case 66:
			panic.Command(EADllPanicResetWithNoGc);
			break;
		case 67:
			panic.CommandReply(EADllPanicAlphaBlendingBitmapsFbsBitmapWithNoGc);
			break;
		case 68:
			panic.Command(EADllPanicMapColorsWithNoGc);
			break;
		case 69:
			panic.Command(EADllPanicDrawTextWithRectWithNoFont);
			break;
		case 70:
			panic.Command(EADllPanicDrawTextVerticalWithNoFont);
			break;
		case 71:
			panic.Command(EADllPanicDrawTextVerticalWithRectWithNoFont);
			break;
		case 72:
			panic.CommandReply(EADllPanicDevice);
			break;
		case 73:
			panic.Command(EADllPanicInvalidFocusScreenTooBig);
			break;
		case 74:
			panic.Command(EADllPanicInvalidFocusScreenNegative);
			break;
		default:
			return(EWsExitReasonFinished);
		}

	ws.Flush();
	return(EWsExitReasonBad);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0125

@SYMDEF             DEF081259

@SYMTestCaseDesc    Panic tests

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test creates an animation and animation's client threads.
					Each thread calls some
					command of the animation which is expected to panic on certain reason.
					The test checks that those client threads will panic returning
					a proper panic code. Client threads are created and tested for
					the following actions:
					1. A Command() call to an animation which executes
					iFunctions->Panic() method. Client thread must panic with
					code EWservPanicAnimDll.
					2. A Command() call with dummy parameter to an animation which
					executes iFunctions->Panic() method. Client thread must panic
					with code EWservPanicAnimDll.
					3. A CommandReply() call to an animation which executes
					iFunctions->Panic() method. Client thread must panic with code
					EWservPanicAnimDll.
					4. A CommandReply() call with dummy parameter to an animation
					which executes iFunctions->Panic() method. Client thread must
					panic with code EWservPanicAnimDll.
					5. A command that calls MAnimGeneralFunctions::SetInterval() when
					the synchronisation mode is not TAnimSync::ESyncNone. Client
					thread must panic with code EWservPanicAnimDll.
					6. A command that calls MAnimGeneralFunctions::SetNextInterval()
					when the synchronisation mode is not TAnimSync::ESyncNone. Client
					thread must panic with code EWservPanicAnimDll.
					7. A command that calls MAnimGeneralFunctions::DuplicateFontL() to
					duplicate undefined font. Client thread must panic with code
					EWservPanicFont.
					8. A command that calls MAnimGeneralFunctions::DuplicateBitmapL()
					to duplicate undefined bitmap. Client thread must panic with code
					EWservPanicBitmap.
					9. A command that calls User::Leave(KErrGeneral). Client thread
					must panic with code EWservPanicAnimLeave.
					10. A command that activates window animation's graphical context
					twice. Client thread must panic with code EWservPanicAnimDll.
					11. A command that tries to draw a rectangle while graphical
					context is not defined yet. Client thread must panic with code
					EWservPanicAnimLeave.
					12. A command that tries to draw a text whihout a font defined yet.
					Client thread must panic with code EWservPanicNoFont.
					13. A command that causes an animation to leave during execution
					of CAnim::Animate() method; and a new command call 4 seconds after
					that. Client thread must panic with code EWservPanicAnimLeave.
					14. A command that causes an animation to leave during execution
					of CAnim::Redraw() method. The client thread initiates it's window
					redraw after that. Client thread must panic with code
					EWservPanicAnimLeave.
					15. The client thread calls RWsSession::TestWrite() on animation
					DLL server-side handle with opcode 999. Client thread must panic with code
					EWservPanicOpcode.
					16. A command that activates graphical context and sets the
					window to invisible and then to visible. Client thread must panic
					with code EWservPanicAnimDll.


@SYMTestExpectedResults See test actions.
*/
void CTAnimDll::TestPanicsL()
	{
	TInt index=0;
	TBool finishTests=EFalse;
	do
		{


		TClientPanic panicCode=EWservPanicAnimDll;
		switch(index)
			{
			// panic the server with a KERN-4. When server fixed these this can be removed.
			case 19:
			case 20:
			case 22:
			case 24:
				goto bypasstest;

			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 10:
			case 16:
			case 17:
			case 18:
			case 73:
			case 74:
				panicCode=EWservPanicAnimDll;
				break;
			case 7:
				panicCode=EWservPanicFont;
				break;
			case 8:
				panicCode=EWservPanicBitmap;
				break;
			case 12:
			case 69:
			case 70:
			case 71:
				panicCode=EWservPanicNoFont;
				break;
			case 15:
				panicCode=EWservPanicOpcode;
				break;
			case 9:
			case 11:
			case 13:
			case 14:
			case 72:
			default: // cases 19 - 68
				panicCode=EWservPanicAnimLeave;
				break;
			}
			
		TEST(iTest->TestWsPanicL(&DoPanicTest,panicCode,index,(TAny*)iTest->iScreenNumber,&finishTests));
bypasstest:
		if (iTest->iScreenNumber == 0)
			iTest->CloseAllPanicWindows();
		index++;
		} while(!finishTests);
	}

void CTAnimDll::TestSyncModes(MAnimGeneralFunctions::TAnimSync aSyncMode)
	{
	RTestAnim anim=RTestAnim(iAnimDll);
	User::LeaveIfError(anim.Construct(*iTestWin->BaseWin(),EAnimTypeTest1,TPtrC8()));
	CleanupClosePushL(anim);

	TPckgBuf<TSyncTests> params;
	TRequestStatus status(KRequestPending);
	params().status=&status;
	params().syncMode=aSyncMode;
	anim.CommandReply(EADllSyncTests,params);
	User::WaitForRequest(status);
	TEST(status==KErrNone);

	CleanupStack::PopAndDestroy(&anim);
	}

void CTAnimDll::TestTimeChange()
	{
	RTestAnim anim=RTestAnim(iAnimDll);
	User::LeaveIfError(anim.Construct(*iTestWin->BaseWin(),EAnimTypeTest1,TPtrC8()));
	CleanupClosePushL(anim);

	TPckgBuf<TTimeChangeTest> params;
	TRequestStatus status(KRequestPending);
	TRequestStatus status2(KRequestPending);
	params().status=&status;
	params().status2=&status2;
	anim.CommandReply(EADllTimeChangedTest,params);
	User::WaitForRequest(status);
	TTime time;
	time.HomeTime();
	time+=TTimeIntervalHours(1);
	User::SetHomeTime(time);
	User::WaitForRequest(status2);
	time.HomeTime();
	time-=TTimeIntervalHours(1);
	User::SetHomeTime(time);
	TEST(status2==KErrNone);

	CleanupStack::PopAndDestroy(&anim);
	}

void CTAnimDll::ConstructL()
	{
	TheClient->iGroup->WinTreeNode()->SetOrdinalPosition(0);
	iRedrawWin=new(ELeave) CAnimWindow(EFalse, CAnimWindow::ERedraw);
	iBackedUpTestWin=new(ELeave) CAnimWindow(EFalse, CAnimWindow::EBackedUp);
	iBlankTestWin=new(ELeave) CAnimWindow(EFalse, CAnimWindow::EBlank);
	iBaseWin=new(ELeave) CAnimWindow(ETrue, CAnimWindow::ERedraw);

//
	TSize screenSize=TheClient->iGroup->Size();
	TInt winWidth=(screenSize.iWidth/3)-10;
	TInt winHeight=screenSize.iHeight-10;
	iBaseWin->ConstructL(TPoint(screenSize.iWidth/3+5,5),TSize(winWidth,winHeight));
	iBlankTestWin->ConstructL(TPoint(screenSize.iWidth/3*2+5,5),TSize(winWidth,winHeight));
	((RBlankWindow *)iBlankTestWin->BaseWin())->SetColor(TRgb::Gray4(2));
	iBackedUpTestWin->ConstructL(TPoint(screenSize.iWidth/3*2+5,5),TSize(winWidth,winHeight));
	iRedrawWin->ConstructL(TPoint(screenSize.iWidth/3*2+5,5),TSize(winWidth,winHeight));
//
	iTestWin=iRedrawWin;
//
	iAnimDll=RAnimDll(TheClient->iWs);
	TInt retVal = iAnimDll.Load(KAnimDLLName);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("iAnimDll.Load(KAnimDLLName) return value - Expected: %d , Actual: %d"), KErrNone, retVal);
	}

CTAnimDll::~CTAnimDll()
	{
	iAnimDll.Close();
	delete iBackedUpTestWin;
	delete iBlankTestWin;
	delete iRedrawWin;
	delete iBaseWin;
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0126

@SYMDEF             DEF081259

@SYMTestCaseDesc    Miscellaneous animation tests.

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Tests window animation's window funtions like WindowSize(),
					IsHidden(), SetVisible().
					Tests RAnimDll's constructor, Close(), Load() and copy-constructor.

@SYMTestExpectedResults Expects window funtions and RAnimDll's methods work properly.
*/
void CTAnimDll::Misc()
	{
	RTestAnim anim=RTestAnim(iAnimDll);
	User::LeaveIfError(anim.Construct(*iTestWin->BaseWin(),EAnimTypeTest1,TPtrC8()));
	CleanupClosePushL(anim);

	TPckgBuf<TSize> params;
	params()=iTestWin->Size();
	TBool retBool = anim.CommandReply(EADllTestWindowSize,params);
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("anim.CommandReply(EADllTestWindowSize,params) return value - Expected: %d , Actual: %d"), ETrue, retBool);
	TheClient->iWs.Finish();
	retBool = anim.CommandReply(EADllTestSetVisible);
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("anim.CommandReply(EADllTestSetVisible) return value - Expected: %d , Actual: %d"), ETrue, retBool);
	CleanupStack::PopAndDestroy(&anim);

	RAnimDll closeme(TheClient->iWs);
	closeme.Close();
	RAnimDll animdll(TheClient->iWs);
	TInt retVal = animdll.Load(KAnimDLLName);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("animdll.Load(KAnimDLLName) return value - Expected: %d , Actual: %d"), KErrNone, retVal);

	RTestAnim anim2(animdll);
	anim2.Close();
	animdll.Close();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0127

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test animation's command call with reply

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Puts an array containing a sequnce of numbers into a command argument.
					Performs command call to server, assuming that the server-side
					should return a sum of the numbers in sequence.

@SYMTestExpectedResults Checks that server returns the correct sum.
*/
void CTAnimDll::RemoteBuffer()
	{
	RTestAnim anim=RTestAnim(iAnimDll);
	User::LeaveIfError(anim.Construct(*iTestWin->BaseWin(),EAnimTypeTest1,TPtrC8()));
	CleanupClosePushL(anim);

	TBuf<ERemoteBufSize> buf(ERemoteBufSize);
	TInt total=0;
	for (TInt8 index=0;index<ERemoteBufSize;index++)
		{
		total+=index;
		buf[index]=index;
		}
	TIpcArgs ipcArgs;
	ipcArgs.Set(KIpcSlot,&buf);
 	TInt retVal = anim.CommandReply(EADllReadRemoteDescriptor,KNullDesC8,ipcArgs);
	TEST(retVal==total);
	if (retVal!=total)
		INFO_PRINTF3(_L("anim.CommandReply(EADllReadRemoteDescriptor,KNullDesC8,ipcArgs) return value - Expected: %d , Actual: %d"), total, retVal);

	CleanupStack::PopAndDestroy(&anim);
	}
//
/**
@SYMTestCaseID		GRAPHICS-WSERV-0128

@SYMDEF             DEF081259

@SYMTestCaseDesc    RAnimDll extracopy test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates another RAnimDll instance for a dll that already has a client-side interface.

@SYMTestExpectedResults Expects that operation completes without errors.
*/
void CTAnimDll::ExtraCopyTest()
	{
	RAnimDll extraCopy(TheClient->iWs);
	TInt retVal = extraCopy.Load(KAnimDLLName);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("extraCopy.Load(KAnimDLLName) return value - Expected: %d , Actual: %d"), KErrNone, retVal);

	extraCopy.Close();
	}
//
void CTAnimDll::SyncMode1()
	{
	TestSyncModes(MAnimGeneralFunctions::ESyncSecond);
	}

void CTAnimDll::CheckWindows()
	{
	CheckRect(iBaseWin->CtBaseWin(),iTestWin->CtBaseWin(),TRect(iBaseWin->Size()),_L("CTAnimDll::CheckWindows() failed"));
	}

void CTAnimDll::InvalidatePauseAndRedraw(const TRect &aRect,TTimeIntervalMicroSeconds32 aInterval)
	{
	iTestWin->Invalidate(aRect);
	TheClient->iWs.Flush();
	User::After(aInterval);
	__ASSERT_DEBUG(iTestWin->iWinType==CAnimWindow::ERedraw,AutoPanic(EAutoPanicWindowType));
	RWindow *win=(RWindow *)iTestWin->BaseWin();
	win->BeginRedraw();
	iTestWin->DoDraw();
	win->EndRedraw();
	TheClient->iWs.Flush();
	}

void CTAnimDll::RedrawAndCheckWindows(TBool aBlankIt)
	{
	if (iTestWin->iWinType==CAnimWindow::ERedraw)
		{
		RWindow *win=(RWindow *)iTestWin->BaseWin();
		win->BeginRedraw();
		iTestWin->DoDraw(aBlankIt);
		win->EndRedraw();
		}
	RWindow *win=(RWindow *)iBaseWin->BaseWin();
	win->BeginRedraw();
	iBaseWin->DoDraw(aBlankIt);
	win->EndRedraw();
	TheClient->WaitForRedrawsToFinish();
	CheckWindows();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0129

@SYMDEF             DEF081259

@SYMTestCaseDesc    Animation drawing test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Performs animation drawing ontop of redrawable window, backed-up window and blank window.
					Draws the same shapes using directly window graphics on an other window (base window).
					The following shapes are drawn:
					1. An animation with a small ellipse drawn in XOR mode on top of redrawable
					black-lined gray window.
					2. The same with a bigger ellipse but previously invalidating an
					area which intersects the old ellipse and the new ellipse.
					3. Runs an animation that continuously moves a rectangle and shrinks another on top of
					redrawable black-lined gray window and then cancels an animation.
					The test checks that the window will be identical with the gray base window
					that contains only black lines.
					4. An animation with a small ellipse drawn in XOR mode on top of backed-up
					black-lined gray window. The test also calls Invalidate() method and performs
					a check again to make sure that the test-window and base window are identical.
					5. An animation with a small ellipse drawn in XOR mode on top of the blank
					gray window. The test also sends commands to the animation that call windows
					SetVisible(EFalse) and SetVisible(ETrue) and checks that the
					test-window and base window are identical.


@SYMTestExpectedResults Each time performs pixelwise comparisons of animated windows
					and the base window expecting the windows to be identical.
*/
void CTAnimDll::DrawingTestL()
	{
	// draws a little circle on the base window using window graphics and using window animation on the test window.
	RTestAnim drawAnim=RTestAnim(iAnimDll);
//
	TRect rect;
	TPckgBuf<TRect> rectPckg;
	TPckgBuf<TBool> boolPckg;

//
	_LIT(KLog1,"Draw Ellipse Test1");
	LOG_MESSAGE(KLog1);
	rect = TRect(10,10,50,50);
	CAnimWindow::SetEllipseDrawMode(CGraphicsContext::EDrawModeXOR);
	iTestWin->Invalidate();
	iTestWin->SetRect(rect);
	iBaseWin->Invalidate();
	iBaseWin->SetRect(rect);
	rectPckg()=rect;
	User::LeaveIfError(drawAnim.Construct(*iTestWin->BaseWin(),EAnimTypeTest3,rectPckg));
	RedrawAndCheckWindows();
//
	_LIT(KLog2,"Draw Ellipse Test2");
	LOG_MESSAGE(KLog2);
	iTestWin->Invalidate(TRect(20,20,100,40));
	iBaseWin->Invalidate(TRect(20,20,100,40));
	iBaseWin->DoDrawEllipse();	// Should clear old Ellipse (except invalid bits)
	rect=TRect(30,30,200,100);
	rectPckg()=rect;
	iBaseWin->SetRect(rect);
	iBaseWin->DoDrawEllipse();	// Should draw new Ellipse (except invalid bits)
	drawAnim.Command(EADllSetRect,rectPckg);
	RedrawAndCheckWindows();
	iBaseWin->DoDrawEllipse();	// Clear old ellipse
//
	_LIT(KLog3,"Window Overlap Anim Test");
	LOG_MESSAGE(KLog3);

	iBaseWin->SetRect(TRect());
	iBaseWin->DoDraw();
	TheClient->iWs.Finish();

	// create another window on top and check that animation beneath is not visible	
	TSize screenSize=TheClient->iGroup->Size();
	TInt winWidth=(screenSize.iWidth/3)-10;
	TInt winHeight=screenSize.iHeight-10;
	TSize size=iTestWin->Size();
	rectPckg()=TRect(size);
	drawAnim.Command(EADllStartAnimate,rectPckg);

	CAnimWindow *topWindow = new (ELeave) CAnimWindow(ETrue, CAnimWindow::ERedraw);
	CleanupStack::PushL(topWindow);
	topWindow->ConstructL(TPoint(screenSize.iWidth/3*2+5,5),TSize(winWidth,winHeight));
	
	RWindow *win=(RWindow *)iTestWin->BaseWin();
	win->BeginRedraw();
	topWindow->DoDraw(EFalse);
	win->EndRedraw();
	
	TheClient->iWs.Flush();
	for(TInt i=0;i<size.iWidth/2;i+=5)
		InvalidatePauseAndRedraw(TRect(i,size.iHeight-i-i,i+i,size.iHeight-i),TTimeIntervalMicroSeconds32(400000));

	TheClient->WaitForRedrawsToFinish();
	CheckRect(iBaseWin->CtBaseWin(),iTestWin->CtBaseWin(),TRect(iBaseWin->Size()));
	
	win->BeginRedraw(rect);
	drawAnim.Command(EADllCancelAnimate,rectPckg);
	win->EndRedraw();

	CleanupStack::PopAndDestroy(topWindow);
//
	iTestWin->BaseWin()->SetVisible(EFalse);
	drawAnim.Close();
//
	_LIT(KLog4,"Draw Ellipse Test4&5");
	LOG_MESSAGE(KLog4);
	rect=TRect(20,20,50,50);
	iBackedUpTestWin->SetRect(rect);
	iBackedUpTestWin->DoDraw();
	iBaseWin->SetRect(rect);
	win->BeginRedraw();
	iBaseWin->DoDraw();
	win->EndRedraw();
	TheClient->iWs.Finish();

	rectPckg()=rect;
	User::LeaveIfError(drawAnim.Construct(*iBackedUpTestWin->BaseWin(),EAnimTypeTest3,rectPckg));
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();
	CheckWindows();
	iBackedUpTestWin->Invalidate();
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	CheckWindows();
	iBackedUpTestWin->BaseWin()->SetVisible(EFalse);
//
	_LIT(KLog5,"Draw Ellipse Test6&7");
	LOG_MESSAGE(KLog5);
	rect=TRect(20,20,50,50);
	TheClient->iWs.Flush();
	RWindow *winBase=(RWindow *)iBaseWin->BaseWin();
	winBase->BeginRedraw();
	iBaseWin->SetRect(rect);
	iBaseWin->DoDraw(ETrue);
	winBase->EndRedraw();
	rectPckg()=rect;
	User::LeaveIfError(drawAnim.Construct(*iBlankTestWin->BaseWin(),EAnimTypeTest3,rectPckg));
	TheClient->iWs.Flush();
	iBlankTestWin->Invalidate();
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	CheckWindows();
	boolPckg()=EFalse;
	drawAnim.Command(EADllSetVisible,boolPckg);
	TheClient->iWs.Flush();
	boolPckg()=ETrue;
	drawAnim.Command(EADllSetVisible,boolPckg);
	TheClient->WaitForRedrawsToFinish();
	CheckWindows();

//This test does functional coverage by calling code that executes each of the 
//server's class (CWsAnimGc) members in turn.


	_LIT(KLog6,"Draw Primitives (Coverage) Test");
	LOG_MESSAGE(KLog6);
	rect = TRect(10,10,150,150);
	iBaseWin->BaseWin()->SetVisible(ETrue);
	iBaseWin->SetRect(TRect(0,0,0,0));
	winBase->BeginRedraw();
	iBaseWin->DoDraw();
	iBaseWin->SetRect(rect);
	iBaseWin->DoDrawCoverage();
	winBase->EndRedraw();

	iTestWin->BaseWin()->SetVisible(ETrue);
	iTestWin->SetRect(rect);
	win->BeginRedraw();
	iTestWin->DoDraw();
	win->EndRedraw();

	rectPckg()=rect;
	User::LeaveIfError(drawAnim.Construct(*iTestWin->BaseWin(),EAnimTypeCoverage,rectPckg));
	
	//set font
	CFbsFont* font;
	User::LeaveIfError(TheClient->iGc->Device()->GetNearestFontToDesignHeightInTwips((CFont*&)font, TFontSpec()));
	TPckgBuf<TInt> fontHandle;
	fontHandle() = font->Handle();
	const TInt err = drawAnim.CommandReply(EADllSetFont, fontHandle);
	TheClient->iGc->Device()->ReleaseFont(font);
	User::LeaveIfError(err);
	
	//draw 
	User::LeaveIfError(drawAnim.CommandReply(EADllDrawNow));

	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();
	CheckWindows();
	drawAnim.Close();
	}

void CTAnimDll::ShadowDrawingTestL()
	{
// Create a blank window to cast a shadow over the animation
	TSize scrSize(TheClient->iScreen->SizeInPixels());
	CTBlankWindow *blank=new(ELeave) CTBlankWindow();
	CleanupStack::PushL(blank);
	blank->SetUpL(TPoint(0,10), TSize(scrSize.iWidth,scrSize.iHeight/3), TheClient->iGroup, *TheClient->iGc);
	blank->BaseWin()->SetShadowHeight(2);
	iBlankTestWin->BaseWin()->SetShadowHeight(0);
//
	RTestAnim drawAnim=RTestAnim(iAnimDll);
	TRect rect(10,scrSize.iHeight/3-20,50,scrSize.iHeight/3+40);
	TPckgBuf<TRect> rectPckg;
	rectPckg()=rect;
	User::LeaveIfError(drawAnim.Construct(*iBlankTestWin->BaseWin(),EAnimTypeTest3,rectPckg));
	drawAnim.Command(EADllSetShadowDrawMode);
	CAnimWindow::SetEllipseDrawMode(CGraphicsContext::EDrawModePEN);
//
	iBlankTestWin->Invalidate();
	iBlankTestWin->SetRect(rect);
	iBaseWin->Invalidate();
	iBaseWin->SetRect(rect);
	RedrawAndCheckWindows(ETrue);
//
	for(TInt ypos=10;ypos>0;ypos-=4)
		{
		blank->SetPos(TPoint(0,ypos));
		TPckgBuf<TShadowDrawTest> params;
		TRequestStatus status(KRequestPending);
		params().status=&status;
		drawAnim.CommandReply(EADllShadowAnimTest,params);
		User::WaitForRequest(status);
		RedrawAndCheckWindows(ETrue);
		}
//
	CleanupStack::PopAndDestroy(blank);	// Destroy the shadow window
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0130

@SYMDEF             DEF081259

@SYMTestCaseDesc    Complicated drawing test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Performs complicated drawing both through the animation on
					a blank window and through direct fbs drawing on the second
					window.

@SYMTestExpectedResults Expects the both windows to be are identical
*/
void CTAnimDll::GeneralDrawingTestL()
	{
	// This test has been removed as it is no longer relevant
	// Anims are not allowed to draw immediately to the screen
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0131

@SYMDEF             DEF081259

@SYMTestCaseDesc    Animated window destruction test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates a window and a child window inside it, runs an animation
					in the child window in ESyncFlash mode, destroys windows.

@SYMTestExpectedResults Expects the operations not to fail.
*/
void CTAnimDll::DestroyWindowTestL()
	{
	RWindow window(TheClient->iWs);
	User::LeaveIfError(window.Construct(*TheClient->iGroup->GroupWin(),123));
	CleanupClosePushL(window);
	window.SetSize(TSize(100,100));
	window.Activate();
	RWindow window2(TheClient->iWs);
	CleanupClosePushL(window2);
	User::LeaveIfError(window2.Construct(window,124));
	window2.Activate();
	window2.BeginRedraw();	// Validate it
	window2.EndRedraw();
//
	RTestAnim drawAnim;
	drawAnim=RTestAnim(iAnimDll);
	TRect rect(10,10,50,50);
	TPckgBuf<TRect> rectPckg;
	rectPckg()=rect;
	User::LeaveIfError(drawAnim.Construct(window2,EAnimTypeTest3,rectPckg));
	drawAnim.Command(EADllStartAnimate,rectPckg);
	CleanupStack::Pop(&window2);
	CleanupStack::PopAndDestroy(&window);
	TheClient->iWs.Flush();
	User::After(TTimeIntervalMicroSeconds32(2000000));
	window2.Close();
	drawAnim.Close();
	}

void CTAnimDll::SetUpMember(TSpriteMember &aMember,TInt aType)
	{
	switch (aType)
		{
	case 1:
		iTest->SetUpMember(aMember);
		aMember.iOffset=TPoint();
		aMember.iInterval=TTimeIntervalMicroSeconds32(1);
		break;
	case 2:
		aMember.iInvertMask=EFalse;
		aMember.iDrawMode=CGraphicsContext::EDrawModeXOR;
		aMember.iOffset=TPoint(1,2);
		aMember.iInterval=TTimeIntervalMicroSeconds32(2);
		break;
	case 3:
		aMember.iInvertMask=ETrue;
		aMember.iDrawMode=CGraphicsContext::EDrawModeOR;
		aMember.iOffset=TPoint(3,4);
		aMember.iInterval=TTimeIntervalMicroSeconds32(3);
		break;
	default:
		iTest->SetUpMember(aMember);
		}
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0132

@SYMDEF             DEF081259

@SYMTestCaseDesc    Sprite animation test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates a sprite with three sprite members and a sprite animation
					passing the sprite to it. The test updates sprite members'
					properties in various ways verifying that the properties are changing
					on the server-side as well.

@SYMTestExpectedResults Expects that operations above do not fail and that client-side
					changes to the sprite affect server-side properly.
*/
#define HALF_SEC 500000
void CTAnimDll::SpriteAnimL()
	{
	// create a sprite, add 3 members to it
	RWsSprite sprite(TheClient->iWs);
	iRedrawWin->BaseWin()->SetVisible(ETrue);
	sprite.Construct(*iRedrawWin->CtBaseWin()->BaseWin(),TPoint(),0);
	CleanupClosePushL(sprite);
	CFbsBitmap* bitmap1=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap1);
	CFbsBitmap* bitmap2=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap2);
	CFbsBitmap* bitmap3=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap3);
	User::LeaveIfError(bitmap1->Create(TSize(10,12),EGray4));
	User::LeaveIfError(bitmap2->Create(TSize(15,17),EGray4));
	User::LeaveIfError(bitmap3->Create(TSize(20,22),EGray4));
	TSpriteMember member1;
	TSpriteMember member2;
	TSpriteMember member3;
	SetUpMember(member1,1);
	member1.iBitmap=bitmap1;
	member1.iMaskBitmap=bitmap1;
	SetUpMember(member2,2);
	member2.iBitmap=bitmap2;
	member2.iMaskBitmap=bitmap2;
	SetUpMember(member3,3);
	member3.iBitmap=bitmap3;
	member3.iMaskBitmap=bitmap3;
	sprite.AppendMember(member1);
	sprite.AppendMember(member2);
	sprite.AppendMember(member3);

	// create a sprite animation for the sprite
	RTestAnim spriteAnim(iAnimDll);
	TPtrC8 des(NULL,0);
	TPoint pos(1,2);
	User::LeaveIfError(spriteAnim.Construct(sprite,EAnimTypeSprite,des));
	CleanupClosePushL(spriteAnim);

	// call EADllCheckMember-command to verify that sprite animation's members
	// have the same properties as the sprite instance
	TPckgBuf<TSpriteMemberInfo> spriteInfo;
	spriteInfo()=TSpriteMemberInfo(TPoint(),0,member1);
	TBool retBool = spriteAnim.CommandReply(EADllCheckMember,spriteInfo);
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("spriteAnim.CommandReply(EADllCheckMember,spriteInfo) return value - Expected: %d , Actual: %d"), ETrue, retBool);

	sprite.SetPosition(pos);
	spriteInfo()=TSpriteMemberInfo(pos,1,member2);
	retBool = spriteAnim.CommandReply(EADllCheckMember,spriteInfo);
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("spriteAnim.CommandReply(EADllCheckMember,spriteInfo) return value - Expected: %d , Actual: %d"), ETrue, retBool);

	pos.SetXY(3,4);
	sprite.SetPosition(pos);
	spriteInfo()=TSpriteMemberInfo(pos,2,member3);
	retBool = spriteAnim.CommandReply(EADllCheckMember,spriteInfo);
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("spriteAnim.CommandReply(EADllCheckMember,spriteInfo) return value - Expected: %d , Actual: %d"), ETrue, retBool);

	// call commands to change sprite members' bitmaps and update members
	spriteAnim.Command(EADllDraw1);
	spriteAnim.Command(EADllDraw2);
	spriteAnim.Command(EADllDraw3);
	TheClient->iWs.Flush();
	User::After(HALF_SEC);

	// call commands to change sprite's position
	TPckgBuf<TPoint> position;
	position().SetXY(60,50);
	spriteAnim.Command(EADllSetPos,position);
	TheClient->iWs.Flush();
	User::After(HALF_SEC);

	// change 1st member's bitmap size and interval, update it and call command to check
	// that the new properties are applied
	User::LeaveIfError(bitmap1->Resize(TSize(30,32)));
	member1.iMaskBitmap=NULL;
	member1.iInterval=TTimeIntervalMicroSeconds32(25);
	User::LeaveIfError(sprite.UpdateMember(0,member1));
	spriteInfo()=TSpriteMemberInfo(TPoint(),0,member1);
	retBool = spriteAnim.CommandReply(EADllCheckMember,spriteInfo);
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("spriteAnim.CommandReply(EADllCheckMember,spriteInfo) return value - Expected: %d , Actual: %d"), ETrue, retBool);

	User::After(HALF_SEC);

	// change 1st member's bitmap size and calls command that invokes
	// MAnimSpriteFunctions::SizeChangedL(). Check that new property is applied
	spriteAnim.CommandReply(EADllDeactivate);
	User::LeaveIfError(bitmap1->Resize(TSize(35,37)));
	spriteAnim.Command(EADllSizeChanged);
	spriteAnim.CommandReply(EADllActivate);
	spriteInfo()=TSpriteMemberInfo(TPoint(),0,member1);
	retBool = spriteAnim.CommandReply(EADllCheckMember,spriteInfo);
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("spriteAnim.CommandReply(EADllCheckMember,spriteInfo) return value - Expected: %d , Actual: %d"), ETrue, retBool);

	User::After(HALF_SEC);

	// call command that changes 1st member's bitmap image
	spriteAnim.Command(EADllIncDraw);
	TheClient->iWs.Flush();
	User::After(HALF_SEC);


	CleanupStack::PopAndDestroy(5,&sprite);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0133

@SYMDEF             DEF081259

@SYMTestCaseDesc    Free timer animation test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test creates a free timer window animation that animates
                    a shrinking filled rect which changes colour in a predefined way.

@SYMTestExpectedResults The test verifies that the colour changes gets through to the the display.
*/

#define NOT_TIMED_OUT timeWaitingForFrame < frametime<<4
void CTAnimDll::FreeTimerL()
    {
    RTestAnim freeTimerAnim(iAnimDll);
    User::LeaveIfError(freeTimerAnim.Construct(*iRedrawWin->BaseWin(),EAnimTypeFreeTimer,KNullDesC8));
    CleanupClosePushL(freeTimerAnim);
    
    const TInt frametime = 100000;  // 100 ms
    const TInt samplingtime = frametime>>1; // 50 ms
    
    TPckgC<TInt> frametimeBuf(frametime);
    freeTimerAnim.Command(ESetFrameTime, frametimeBuf);
    freeTimerAnim.Command(EStartAnimation);
    TheClient->iWs.Flush();
    TheClient->iWs.Finish();

    TInt x = iRedrawWin->BaseWin()->AbsPosition().iX + (iRedrawWin->BaseWin()->Size().iWidth >> 1);
    TInt y = iRedrawWin->BaseWin()->AbsPosition().iY + (iRedrawWin->BaseWin()->Size().iHeight >> 1);
    TPoint middle(x,y);
    
    TRgb color = KRgbMagenta;
    TBool foundRedFrame = EFalse;
    TBool foundGreenFrame = EFalse;
    TBool foundBlueFrame = EFalse;
    TBool foundBlackFrame = EFalse;
    
    //The free-timer animates the colour in this predefined way.
    //Test that the screen gets updated.
    TInt timeWaitingForFrame = 0;
    while(NOT_TIMED_OUT && !(foundRedFrame && foundGreenFrame && foundBlueFrame && foundBlackFrame)) //wait max 16*frametime=1280ms
        {
        //wait for the colour frame
        color = PixelColour(middle);

        if(color == KRgbRed)
            {
            foundRedFrame = ETrue;
            }
        else if(color == KRgbGreen)
            {
            foundGreenFrame = ETrue;
            }
        else if(color == KRgbBlue)
            {
            foundBlueFrame = ETrue;
            }
        else if(color == KRgbBlack)
            {
            foundBlackFrame = ETrue;
            }

        timeWaitingForFrame += samplingtime;
        User::After(samplingtime); 
        }
    
    TEST(foundRedFrame);
    TEST(foundGreenFrame);
    TEST(foundBlueFrame);
    TEST(foundBlackFrame);
    
    freeTimerAnim.Command(EStopAnimation);  
    CleanupStack::PopAndDestroy(&freeTimerAnim);
    }

TRgb CTAnimDll::PixelColour(const TPoint& aPoint) const
    {
    TRgb pixel;
    TheClient->iScreen->GetPixel(pixel, aPoint);
    return pixel;
    }

/**
@SYMTestCaseID		GRAPHICS-WSERV-0134

@SYMDEF             DEF081259

@SYMTestCaseDesc    Animation's ESyncSecond sync-mode test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an animation in ESyncSecond sync-mode.
					Checks that animation's Animate() method was called either 9, 10 or 11
					times during a 10-seconds-wait.
					The test also checks that if RWsSession::PrepareForSwitchOff()
					is called then the Animate() method will not be called until
					TRawEvent::EActive event is simulated.

@SYMTestExpectedResults See actions.
*/
static const TInt K1Second =  1000000;
static const TInt K3Seconds = 3000000;
static const TInt K9Seconds = 9500000;
void CTAnimDll::DisableTimerL()
	{
	//Defer start of timerAnim until the systen time are close to a full second
	TTime now;
	now.HomeTime();
	TInt deferTime = K1Second - now.DateTime().MicroSecond();
	User::After(deferTime);
	
	//Kick off timerAnim
	RTestAnim timerAnim(iAnimDll);
	User::LeaveIfError(timerAnim.Construct(*iRedrawWin->BaseWin(),EAnimTypeTimer,KNullDesC8));
	
	User::After(K9Seconds);
	TInt count=timerAnim.CommandReply(EADllCurrentCount);
	
#if defined(LOGGING)
	TLogMessageText buf;
	_LIT(KLog,"Anim DisableTimer  Count=%d");
	buf.Format(KLog,count);
	TheClient->LogMessage(buf);
#endif
	
	TEST(count>=9 && count<=11);
	if (count != 10)
		INFO_PRINTF2(_L("count>=9 && count<=11 - Expected: 9,10 or 11 , Actual: %d"), count);
	
#if defined(__WINS__)
	TheClient->iWs.SimulateXyInputType(EXYInputNone);	//Turn off pen in case it turns on the timer
#endif

	// reset timer's counter and call RWsSession::PrepareForSwitchOff(),
	// check that 3 seconds after the counter = 0
	timerAnim.Command(EADllResetCount);
	timerAnim.Command(EADllNoTimer);
	TheClient->iWs.PrepareForSwitchOff();
	TheClient->iWs.Flush();
	User::After(K3Seconds);
	count=timerAnim.CommandReply(EADllCurrentCount);
#if defined(LOGGING)
	buf.Format(KLog,count);
	TheClient->LogMessage(buf);
#endif
	TEST(count==0);
	if (count!=0)
		INFO_PRINTF3(_L("count==0 - Expected: %d , Actual: %d"), 0,  count);


	// simulates TRawEvent::EActive event,
	// check that the timer didn't ticked until the event were simulated
	timerAnim.Command(EADllTimerStarted);
	TRawEvent event;
	event.Set(TRawEvent::EActive);
	TheClient->iWs.SimulateRawEvent(event);
#if defined(__WINS__)
	TheClient->iWs.SimulateXyInputType(EXYInputPointer);	//Turn pen on again
#endif
	TInt retVal = timerAnim.CommandReply(EADllFailed);
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("timerAnim.CommandReply(EADllFailed) return value - Expected: %d , Actual: %d"), 0,  retVal);

	timerAnim.Close();
	}


/**
@SYMTestCaseID		GRAPHICS-WSERV-0135

@SYMDEF             DEF081259

@SYMTestCaseDesc    Multiple animations test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates multiple animations for a window.

@SYMTestExpectedResults The test doesn't perform any specific validations.
*/
void CTAnimDll::MultipleAnimsL()
	{
	RTestAnim anim1(iAnimDll);
	RTestAnim anim2(iAnimDll);
	RTestAnim anim3(iAnimDll);
	RTestAnim anim4(iAnimDll);
	RTestAnim anim5(iAnimDll);
	RTestAnim anim6(iAnimDll);
	TPtrC8 des(NULL,0);
	User::LeaveIfError(anim1.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	CleanupClosePushL(anim1);
	User::LeaveIfError(anim2.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	CleanupClosePushL(anim2);
	User::LeaveIfError(anim3.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	CleanupClosePushL(anim3);
	User::LeaveIfError(anim4.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	CleanupClosePushL(anim4);
	User::LeaveIfError(anim5.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	CleanupClosePushL(anim5);
	User::LeaveIfError(anim6.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	anim6.Close();
	User::LeaveIfError(anim6.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	anim6.Close();
	User::LeaveIfError(anim6.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	CleanupStack::PopAndDestroy(&anim5);
	anim6.Close();
	User::LeaveIfError(anim5.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	CleanupClosePushL(anim5);
	User::LeaveIfError(anim6.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	CleanupStack::Pop(&anim5);
	CleanupStack::PopAndDestroy(&anim4);
	anim5.Close();
	anim6.Close();
	User::LeaveIfError(anim4.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	CleanupStack::PopAndDestroy(&anim3);
	anim4.Close();
	User::LeaveIfError(anim3.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	CleanupStack::PopAndDestroy(&anim2);
	anim3.Close();
	User::LeaveIfError(anim2.Construct(*iRedrawWin->BaseWin(),EAnimTypeTest3,des));
	CleanupStack::PopAndDestroy(&anim1);
	anim2.Close();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0136

@SYMDEF             DEF081259

@SYMTestCaseDesc    Click plugin DLL test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Created two handles for a click plugin dll (RSoundPlugIn instances).
					Plays with Load(), Unload() methods to verify that they affect
					IsLoaded() call result.
					Checks that CommandReply() for different opcodes
					returns RSoundPlugIn::ESoundWrongPlugIn value.
					Plays with SetPenClick(), SetKeyClick() to verify that they affect
					PenClickEnabled() and KeyClickEnabled() call result.


@SYMTestExpectedResults The test checks that RSoundPlugIn's methods work properly
					and that both handles behave identically.
*/
void CTAnimDll::ClickPlugInL()
	{
	_LIT(KDllName,"CLICK");
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	CleanupClosePushL(ws);
	TBool retBool;
	TInt retVal;

	// assign to the correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	CleanupStack::PushL(screen);
	User::LeaveIfError(screen->Construct(iTest->iScreenNumber));

	RSoundPlugIn click1(ws);
	RSoundPlugIn click2(TheClient->iWs);
	User::LeaveIfError(click1.Construct());
	CleanupClosePushL(click1);
	User::LeaveIfError(click2.Construct());
	CleanupClosePushL(click2);
	TBool changeable1;
	TBool changeable2;
	TBool loaded=click1.IsLoaded(changeable1);
	//test that either both are loaded or both are not loaded
	TEST(!loaded==!click2.IsLoaded(changeable2));
	if (!loaded!=!click2.IsLoaded(changeable2))
		INFO_PRINTF3(_L("!loaded==!click2.IsLoaded(changeable2) - Expected: %d , Actual: %d"), !loaded,  !click2.IsLoaded(changeable2));

	//test that either both are changeable or both are not changeable
	TEST(!changeable1==!changeable2);
	if (!changeable1!=!changeable2)
		INFO_PRINTF3(_L("!changeable1==!changeable2) - Expected: %d , Actual: %d"), !changeable1,  !changeable2);

	if (loaded && changeable1)
		{
		// unload the first one
		User::LeaveIfError(click1.Unload());
		// check that the second is not loaded and still changeable
		retBool = !click2.IsLoaded(changeable2);
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("!click2.IsLoaded(changeable2) return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		TEST(changeable2);
		if (!changeable2)
			INFO_PRINTF3(_L("changeable2 - Expected: %d , Actual: %d"), ETrue,  changeable2);


		// load the first one and check that the second became loaded and is changeable
		User::LeaveIfError(click1.Load(KDllName));
		retBool = click2.IsLoaded(changeable2);
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("click2.IsLoaded(changeable2) return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		TEST(changeable2);
		if (!changeable2)
			INFO_PRINTF3(_L("changeable2 - Expected: %d , Actual: %d"), ETrue,  changeable2);

		// unload the second one
		User::LeaveIfError(click2.Unload());
		// check that the first is not loaded and still changeable
		retBool = !click1.IsLoaded(changeable1);
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("!click1.IsLoaded(changeable1) return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		TEST(changeable1);
		if (!changeable1)
			INFO_PRINTF3(_L("changeable1 - Expected: %d , Actual: %d"), ETrue,  changeable1);

		// load the second one and check that the first one became loaded and is changeable
		User::LeaveIfError(click2.Load(KDllName));
		loaded=click1.IsLoaded(changeable1);
		TEST(loaded);
		if (!loaded)
			INFO_PRINTF3(_L("loaded - Expected: %d , Actual: %d"), ETrue,  loaded);

		TEST(changeable1);
		if (!changeable1)
			INFO_PRINTF3(_L("changeable1 - Expected: %d , Actual: %d"), ETrue,  changeable1);
		}
	if (loaded)
		{
		TPtrC8 des(NULL,0);
		retVal = click1.CommandReply(0,des);
		TEST(RSoundPlugIn::ESoundWrongPlugIn==retVal);
		if (RSoundPlugIn::ESoundWrongPlugIn!=retVal)
			INFO_PRINTF3(_L("click1.CommandReply(0,des) return value - Expected: %d , Actual: %d"), RSoundPlugIn::ESoundWrongPlugIn,  retVal);

		retVal = click1.CommandReply(1,des);
		TEST(RSoundPlugIn::ESoundWrongPlugIn==retVal);
		if (RSoundPlugIn::ESoundWrongPlugIn!=retVal)
			INFO_PRINTF3(_L("click1.CommandReply(1,des) return value - Expected: %d , Actual: %d"), RSoundPlugIn::ESoundWrongPlugIn,  retVal);

		retVal = click1.CommandReply(2,des);
		TEST(RSoundPlugIn::ESoundWrongPlugIn==retVal);
		if (RSoundPlugIn::ESoundWrongPlugIn!=retVal)
			INFO_PRINTF3(_L("click1.CommandReply(2,des) return value - Expected: %d , Actual: %d"), RSoundPlugIn::ESoundWrongPlugIn,  retVal);

		retVal = click1.CommandReply(-1,des);
		TEST(RSoundPlugIn::ESoundWrongPlugIn==retVal);
		if (RSoundPlugIn::ESoundWrongPlugIn!=retVal)
			INFO_PRINTF3(_L("click1.CommandReply(-1,des) return value - Expected: %d , Actual: %d"), RSoundPlugIn::ESoundWrongPlugIn,  retVal);

		retBool = click1.KeyClickEnabled();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("click1.KeyClickEnabled() return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		retBool = click1.PenClickEnabled();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("click1.PenClickEnabled() return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		click1.SetPenClick(EFalse);
		retBool = click1.KeyClickEnabled();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("click1.KeyClickEnabled() return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		retBool = !click1.PenClickEnabled();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("!click1.PenClickEnabled() return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		click1.SetKeyClick(EFalse);

		retBool = !click1.KeyClickEnabled();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("!click1.KeyClickEnabled() return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		retBool = !click1.PenClickEnabled();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("!click1.PenClickEnabled() return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		click1.SetPenClick(ETrue);
		retBool = !click1.KeyClickEnabled();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("!click1.KeyClickEnabled() return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		retBool = click1.PenClickEnabled();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("click1.PenClickEnabled() return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		click1.SetKeyClick(ETrue);
		retBool = click1.KeyClickEnabled();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("click1.KeyClickEnabled() return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		retBool = click1.PenClickEnabled();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("click1.PenClickEnabled() return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		/*RLibrary lib;
		TInt err=lib.Load(KDllName);
		if (err==KErrNone)
			{
			TPtrC8 des(NULL,0);
			TEST(lib.Type()[2].iUid==click1.CommandReply(0,des));
			lib.Close();
			}*/
		}
	CleanupStack::PopAndDestroy(4,&ws);
	}

void CTAnimDll::TestWindowInfo(RTestAnim& aAnim,TInt aScreen,TInt aPos,TInt aId)
	{
	TPckgBuf<TWindowGroupInfoParms> params;
	params().iScreen=aScreen;
	params().iOrdinalPosition=aPos;
	TPckgBuf<MAnimGeneralFunctionsWindowExtension::TWindowGroupInfo> winGpInfo;
	TIpcArgs ipcArgs;
	ipcArgs.Set(KIpcSlot,&winGpInfo);
	TBool retBool = aAnim.CommandReply(EADllWindowGroupInfo,params,ipcArgs);
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("aAnim.CommandReply(EADllWindowGroupInfo,params,ipcArgs) return value - Expected: %d , Actual: %d"), ETrue,  retBool);

	MAnimGeneralFunctionsWindowExtension::TWindowGroupInfo& info=winGpInfo();
	TEST(info.iId==aId);
	if (info.iId!=aId)
		INFO_PRINTF3(_L("winGpInfo() return value - Expected: %d , Actual: %d"), aId,  info.iId);

	TInt retVal = TheClient->iWs.GetWindowGroupOrdinalPriority(aId);
	TEST(info.iOrdinalPriority==retVal);
	if (info.iOrdinalPriority!=retVal)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupOrdinalPriority(aId) return value - Expected: %d , Actual: %d"), info.iOrdinalPriority, retVal);

	TBuf<64> windowGroupNameS;
	TBuf<64> windowGroupNameA;
	TheClient->iWs.GetWindowGroupNameFromIdentifier(aId,windowGroupNameS);
	retVal = windowGroupNameS.Length();
	TEST(info.iNameLength==retVal);
	if (info.iNameLength!=retVal)
		INFO_PRINTF3(_L("windowGroupNameS.Length() return value - Expected: %d , Actual: %d"), info.iNameLength, retVal);

	if (info.iNameLength<64)
		{
		ipcArgs.Set(KIpcSlot,&windowGroupNameA);
		retBool = aAnim.CommandReply(EADllWindowGroupName,params,ipcArgs);
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("aAnim.CommandReply(EADllWindowGroupName,params,ipcArgs) return value - Expected: %d , Actual: %d"), ETrue,  retBool);

		if (info.iNameLength>0)
			{
			TEST(windowGroupNameA==windowGroupNameS);
			}
		else
			{
			TEST(windowGroupNameA.Length()==0);
			if (windowGroupNameA.Length()!=0)
				INFO_PRINTF3(_L("iwindowGroupNameA.Length()==0 - Expected: %d, Actual: %d"), 0, windowGroupNameA.Length());
			}

		}
	TEST(info.iParentId>=-1);
	if (info.iParentId<-1)
		INFO_PRINTF3(_L("info.iParentId>=-1 - Expected: %d or greater, Actual: %d"), -1, info.iParentId);

	}

void CTAnimDll::TestEventHandlerRemovalL()
	{
	RTestAnim anim(iAnimDll);
	User::LeaveIfError(anim.Construct(*iRedrawWin->BaseWin(),EAnimTypeEventHandler,KNullDesC8));
	anim.Close();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0137

@SYMDEF             DEF081259

@SYMTestCaseDesc    Animation's extended window functions test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test creates a window animation that has a couple of
					commands that work with extended window functions encapsulated
					into MAnimGeneralFunctionsWindowExtension class instance.
					The test calls those commands to compare MAnimGeneralFunctionsWindowExtension's
					methods' results with the results retuned by WSERV client classes.
					MAnimGeneralFunctionsWindowExtension methods tested here:
					Screens(), FocusScreens(), WindowGroups(), WindowGroupInfo(),
					WindowGroupName()

@SYMTestExpectedResults MAnimGeneralFunctionsWindowExtension methods must give the same results as
					the corresponding WSERV client methods would.
*/
void CTAnimDll::WinFunctionsL()
	{
	RTestAnim anim(iAnimDll);
	User::LeaveIfError(anim.Construct(*iRedrawWin->BaseWin(),EAnimTypeWindowFunctions,KNullDesC8));
	CleanupClosePushL(anim);
	TInt screens=anim.CommandReply(EADllScreen);
	TEST(screens>0);
	if (screens<=0)
		INFO_PRINTF3(_L("anim.CommandReply(EADllScreen) return value - Expected: %d or greater, Actual: %d"), 1, screens);

	TInt focusScreen=anim.CommandReply(EADllFocusScreen);
	TInt retVal = TheClient->iWs.GetFocusScreen();
	TEST(focusScreen==retVal && focusScreen<=screens);
	if (focusScreen!=retVal || focusScreen>screens)
		INFO_PRINTF4(_L("anim.CommandReply(EADllFocusScreen) return value - Expected: %d and less than or equal to  %d, Actual: %d"), retVal, screens, focusScreen);

	TInt ii;
	TPckgBuf<TInt> screen;
	CArrayPtrFlat<CMinWin>* wins=new(ELeave) CArrayPtrFlat<CMinWin>(screens);
	CleanupStack::PushL(wins);
	// create one window on each screen
	for(ii=0;ii<screens;++ii)
		{
		CMinWin* win=new(ELeave) CMinWin(ii);
		CleanupStack::PushL(win);
		win->ConstructL();
		wins->AppendL(win);
		}
	if (screens>1)
		{
		for (ii=screens-1;ii>=0;--ii)
			{
			TheClient->iWs.SetFocusScreen(ii);
			retVal = anim.CommandReply(EADllFocusScreen);
			TEST(retVal==ii);
			if (retVal!=ii)
				INFO_PRINTF3(_L("anim.CommandReply(EADllFocusScreen) return value - Expected: %d, Actual: %d"), ii, retVal);
			}
		for (ii=screens-1;ii>=0;--ii)
			{
			screen()=ii;
			anim.CommandReply(EADllSetFocusScreen,screen);
			retVal = anim.CommandReply(EADllFocusScreen);
			TEST(retVal==ii);
			if (retVal!=ii)
				INFO_PRINTF3(_L("anim.CommandReply(EADllFocusScreen) return value - Expected: %d, Actual: %d"), ii, retVal);
			}
		}
	TheClient->iWs.SetFocusScreen(focusScreen);
	retVal = anim.CommandReply(EADllFocusScreen);
	TEST(retVal==focusScreen);
	if (retVal!=focusScreen)
		INFO_PRINTF3(_L("anim.CommandReply(EADllFocusScreen) return value - Expected: %d, Actual: %d"), focusScreen, retVal);

	TInt winGroupsAll=0;
	for(ii=0;ii<screens;++ii)
		{
		screen()=ii;
		winGroupsAll+=anim.CommandReply(EADllWindowGroups,screen);
		}
	TInt winGroupsS=TheClient->iWs.NumWindowGroups();
	TEST(winGroupsAll==winGroupsS);
	if (winGroupsAll!=winGroupsS)
		INFO_PRINTF3(_L("TheClient->iWs.NumWindowGroups() return value - Expected: %d, Actual: %d"), winGroupsS, winGroupsAll);

	// the remaining test will use winGroupsA and performed on current focus screen only
	screen()=iTest->iScreenNumber;
	TInt winGroupsA=anim.CommandReply(EADllWindowGroups,screen);
	CArrayFixFlat<TInt>* windowList=new(ELeave) CArrayFixFlat<TInt>(3);
	CleanupStack::PushL(windowList);
	// wserv implementation returns window groups from current focus screen, followed by the rest
	TheClient->iWs.WindowGroupList(windowList);
	// only interested in window group from current focus screen
	for (ii=0;ii<winGroupsA;++ii)
		TestWindowInfo(anim,iTest->iScreenNumber,ii,(*windowList)[ii]);
	TPckgBuf<MAnimGeneralFunctionsWindowExtension::TWindowGroupInfo> winGpInfo;
	TIpcArgs ipcArgs;
	TPckgBuf<TWindowGroupInfoParms> params;
	params().iScreen=iTest->iScreenNumber;
	params().iOrdinalPosition=winGroupsA;
	ipcArgs.Set(KIpcSlot,&winGpInfo);
	TBuf<64> windowGroupName;
	TBool retBool = !anim.CommandReply(EADllWindowGroupInfo,params,ipcArgs);
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("!anim.CommandReply(EADllWindowGroupInfo,params,ipcArgs) return value - Expected: %d, Actual: %d"), ETrue, retBool);

	ipcArgs.Set(KIpcSlot,&windowGroupName);
	retBool = !anim.CommandReply(EADllWindowGroupName,params,ipcArgs);
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("!anim.CommandReply(EADllWindowGroupName,params,ipcArgs) return value - Expected: %d, Actual: %d"), ETrue, retBool);

	/*params().iOrdinalPosition=-1;		//Behaviour for negative positions not defined - but as writtin this code will pass
	ipcArgs.Set(KIpcSlot,&winGpInfo);
	TEST(anim.CommandReply(EADllWindowGroupInfo,params,ipcArgs));
	ipcArgs.Set(KIpcSlot,&windowGroupNameA);
	TEST(anim.CommandReply(EADllWindowGroupName,params,ipcArgs));*/
	RWindowGroup group(TheClient->iWs);
	User::LeaveIfError(group.Construct(ENullWsHandle));
	CleanupClosePushL(group);
	_LIT(KGroupName,"Testxyz1234");
	group.SetName(KGroupName);
	group.SetOrdinalPosition(0,TheClient->iWs.GetWindowGroupOrdinalPriority((*windowList)[0])+1);
	TInt groupId=group.Identifier();
	TestWindowInfo(anim,iTest->iScreenNumber,0,groupId);
	group.SetOrdinalPosition(0,TheClient->iWs.GetWindowGroupOrdinalPriority((*windowList)[winGroupsA-1])-1);
	TestWindowInfo(anim,iTest->iScreenNumber,winGroupsA,groupId);
	CleanupStack::PopAndDestroy(screens+4,&anim);
// Restore us back to the front
	TheClient->iGroup->GroupWin()->SetOrdinalPosition(0);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0138

@SYMDEF             DEF081259

@SYMTestCaseDesc    Animation's extended window functions test 2

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test creates a window animation that has a command
					for setting window's ordinal position and priority.
					The test creates 4 window groups with the same priority and
					checks that they have correct ordinal positions after the creation.
					After that the test plays with updating ordinal positions and
					priorities of the window groups using the animation's command.

@SYMTestExpectedResults The test checks that the command was executed with no errors
					and affected ordinal positions of the window groups properly.
*/
void CTAnimDll::WinFunctions2L()
	{
	RTestAnim anim(iAnimDll);
	User::LeaveIfError(anim.Construct(*iRedrawWin->BaseWin(),EAnimTypeWindowFunctions,KNullDesC8));
	CleanupClosePushL(anim);
	CArrayFixFlat<TInt>* windowList=new(ELeave) CArrayFixFlat<TInt>(3);
	TheClient->iWs.WindowGroupList(windowList);
	TInt priority=TheClient->iWs.GetWindowGroupOrdinalPriority((*windowList)[0])+2;
	delete windowList;
	RWindowGroup group1(TheClient->iWs);
	User::LeaveIfError(group1.Construct(ENullWsHandle));
	CleanupClosePushL(group1);
	group1.SetOrdinalPosition(0,priority);
	RWindowGroup group2(TheClient->iWs);
	User::LeaveIfError(group2.Construct(ENullWsHandle));
	CleanupClosePushL(group2);
	group2.SetOrdinalPosition(0,priority);
	RWindowGroup group3(TheClient->iWs);
	User::LeaveIfError(group3.Construct(ENullWsHandle));
	CleanupClosePushL(group3);
	group3.SetOrdinalPosition(0,priority);
	RWindowGroup group4(TheClient->iWs);
	User::LeaveIfError(group4.Construct(ENullWsHandle));
	CleanupClosePushL(group4);
	group4.SetOrdinalPosition(0,priority);

	TInt retVal = group1.OrdinalPosition();
	TEST(retVal==3);
	if (retVal!=3)
		INFO_PRINTF3(_L("group1.OrdinalPosition() return value - Expected: %d, Actual: %d"), 3, retVal);

	retVal = group2.OrdinalPosition();
	TEST(retVal==2);
	if (retVal!=2)
		INFO_PRINTF3(_L("group2.OrdinalPosition() return value - Expected: %d, Actual: %d"), 2, retVal);

	retVal = group3.OrdinalPosition();
	TEST(retVal==1);
	if (retVal!=1)
		INFO_PRINTF3(_L("group3.OrdinalPosition() return value - Expected: %d, Actual: %d"), 1, retVal);

	retVal = group4.OrdinalPosition();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("group4.OrdinalPosition() return value - Expected: %d, Actual: %d"), 0, retVal);

	TPckgBuf<TSetOrdinalParms> params;
	params().iIdentifier=group4.Identifier();
	params().iOrdinalPriority=priority;
	params().iOrdinalPosition=1;
	retVal = anim.CommandReply(EADllSetOrdinalPosition,params);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("anim.CommandReply(EADllSetOrdinalPosition,params) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = group3.OrdinalPosition();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("group3.OrdinalPosition() return value - Expected: %d, Actual: %d"), 0, retVal);

	retVal = group4.OrdinalPosition();
	TEST(retVal==1);
	if (retVal!=1)
		INFO_PRINTF3(_L("group4.OrdinalPosition() return value - Expected: %d, Actual: %d"), 1, retVal);

	params().iOrdinalPosition=2;
	retVal = anim.CommandReply(EADllSetOrdinalPosition,params);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("anim.CommandReply(EADllSetOrdinalPosition,params) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = group2.OrdinalPosition();
	TEST(retVal==1);
	if (retVal!=1)
		INFO_PRINTF3(_L("group2.OrdinalPosition() return value - Expected: %d, Actual: %d"), 1, retVal);

	retVal = group4.OrdinalPosition();
	TEST(retVal==2);
	if (retVal!=2)
		INFO_PRINTF3(_L("group4.OrdinalPosition() return value - Expected: %d, Actual: %d"), 2, retVal);

	params().iOrdinalPosition=3;
	retVal = anim.CommandReply(EADllSetOrdinalPosition,params);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("anim.CommandReply(EADllSetOrdinalPosition,params) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = group1.OrdinalPosition();
	TEST(retVal==2);
	if (retVal!=2)
		INFO_PRINTF3(_L("group1.OrdinalPosition() return value - Expected: %d, Actual: %d"), 2, retVal);

	retVal = group2.OrdinalPosition();
	TEST(retVal==1);
	if (retVal!=1)
		INFO_PRINTF3(_L("group2.OrdinalPosition() return value - Expected: %d, Actual: %d"), 1, retVal);

	retVal = group3.OrdinalPosition();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("group3.OrdinalPosition() return value - Expected: %d, Actual: %d"), 0, retVal);

	retVal = group4.OrdinalPosition();
	TEST(retVal==3);
	if (retVal!=3)
		INFO_PRINTF3(_L("group4.OrdinalPosition() return value - Expected: %d, Actual: %d"), 3, retVal);

	params().iOrdinalPriority=priority+1;
	params().iOrdinalPosition=3;
	retVal = anim.CommandReply(EADllSetOrdinalPosition,params);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("anim.CommandReply(EADllSetOrdinalPosition,params) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = group4.OrdinalPosition();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("group4.OrdinalPosition() return value - Expected: %d, Actual: %d"), 0, retVal);

	params().iOrdinalPriority=priority;
	params().iOrdinalPosition=-2;
	retVal = anim.CommandReply(EADllSetOrdinalPosition,params);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("anim.CommandReply(EADllSetOrdinalPosition,params) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = group4.OrdinalPosition();
	TEST(retVal==3);
	if (retVal!=3)
		INFO_PRINTF3(_L("group4.OrdinalPosition() return value - Expected: %d, Actual: %d"), 3, retVal);

	params().iOrdinalPosition=0;
	retVal = anim.CommandReply(EADllSetOrdinalPosition,params);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("anim.CommandReply(EADllSetOrdinalPosition,params) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = group1.OrdinalPosition();
	TEST(retVal==3);
	if (retVal!=3)
		INFO_PRINTF3(_L("group1.OrdinalPosition() return value - Expected: %d, Actual: %d"), 3, retVal);

	retVal = group2.OrdinalPosition();
	TEST(retVal==2);
	if (retVal!=2)
		INFO_PRINTF3(_L("group2.OrdinalPosition() return value - Expected: %d, Actual: %d"), 2, retVal);

	retVal = group3.OrdinalPosition();
	TEST(retVal==1);
	if (retVal!=1)
		INFO_PRINTF3(_L("group3.OrdinalPosition() return value - Expected: %d, Actual: %d"), 1, retVal);

	retVal = group4.OrdinalPosition();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("group4.OrdinalPosition() return value - Expected: %d, Actual: %d"), 0, retVal);

	params().iOrdinalPriority=priority-1;
	params().iOrdinalPosition=2;
	retVal = anim.CommandReply(EADllSetOrdinalPosition,params);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("anim.CommandReply(EADllSetOrdinalPosition,params) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = group1.OrdinalPosition();
	TEST(retVal==2);
	if (retVal!=2)
		INFO_PRINTF3(_L("group1.OrdinalPosition() return value - Expected: %d, Actual: %d"), 2, retVal);

	retVal = group2.OrdinalPosition();
	TEST(retVal==1);
	if (retVal!=1)
		INFO_PRINTF3(_L("group2.OrdinalPosition() return value - Expected: %d, Actual: %d"), 1, retVal);

	retVal = group3.OrdinalPosition();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("group3.OrdinalPosition() return value - Expected: %d, Actual: %d"), 0, retVal);

	retVal = group4.OrdinalPosition();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("group4.OrdinalPosition() return value - Expected: %d, Actual: %d"), 0, retVal);


	params().iOrdinalPriority=priority;
	params().iOrdinalPosition=0;
	retVal = anim.CommandReply(EADllSetOrdinalPosition,params);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("anim.CommandReply(EADllSetOrdinalPosition,params) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = group1.OrdinalPosition();
	TEST(retVal==3);
	if (retVal!=3)
		INFO_PRINTF3(_L("group1.OrdinalPosition() return value - Expected: %d, Actual: %d"), 3, retVal);

	retVal = group2.OrdinalPosition();
	TEST(retVal==2);
	if (retVal!=2)
		INFO_PRINTF3(_L("group2.OrdinalPosition() return value - Expected: %d, Actual: %d"), 2, retVal);

	retVal = group3.OrdinalPosition();
	TEST(retVal==1);
	if (retVal!=1)
		INFO_PRINTF3(_L("group3.OrdinalPosition() return value - Expected: %d, Actual: %d"), 1, retVal);

	retVal = group4.OrdinalPosition();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("group4.OrdinalPosition() return value - Expected: %d, Actual: %d"), 0, retVal);

	CleanupStack::PopAndDestroy(5,&anim);
	/*TInt pos1=group1.OrdinalPosition();
	TInt pos2=group2.OrdinalPosition();
	TInt pos3=group3.OrdinalPosition();
	TInt pos4=group4.OrdinalPosition();*/
	}


/**
@SYMTestCaseID		GRAPHICS-WSERV-0139

@SYMDEF             DEF081259, DEF122220

@SYMTestCaseDesc    Animation's notifications test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test works with two screens. An animation is created that keeps
					track on all notifications about EDirectScreenAccess, EHeartbeatTimer,
					EScreenDeviceChange events.
					The test generates those events by performing operations on direct
					screen access and simulating TRawEvent::EInactive and TRawEvent::EAactive
					events.

@SYMTestExpectedResults  The test checks that the animation didn't miss any of EDirectScreenAccess
					or EHeartbeatTimer notifications. (The test doesn't check EScreenDeviceChange
					notifications however).
					The test also makes sure that the animation didn't animate
					during inactive state.
					For DEF12220, the test will crash when a untidied notifier from a previous RTestAnim object
					is accessed if the fix is not present. If the fix is present, the test should not crash.
*/
void CTAnimDll::TestNotificationsL()
	{
	RTestAnim anim(iAnimDll);
	User::LeaveIfError(anim.Construct(*iRedrawWin->BaseWin(),EAnimTypeNotificationTest,KNullDesC8));
	CleanupClosePushL(anim);

	TPckgBuf<TInt> screenNum;

	// initial state - test that we have no DSA on either screen
	screenNum() = 0;
	TInt dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	TEST(!dsa);
	if (dsa)
		INFO_PRINTF3(_L("anim.CommandReply(EADllQueryDSA, screenNum) return value - Expected: %d, Actual: %d"), 0, dsa);

	screenNum() = 1;
	dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	TEST(!dsa);
	if (dsa)
		INFO_PRINTF3(_L("anim.CommandReply(EADllQueryDSA, screenNum) return value - Expected: %d, Actual: %d"), 0, dsa);

	// create first DSA, but don't start it - test that we have no DSA on either screen
	CScrollText* dsa1 = CScrollText::NewL(iTest->iScreenNumber,1,*TheClient->iGroup,5,EFalse);
	CleanupStack::PushL(dsa1);
	screenNum() = 0;
	dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	TEST(!dsa);
	if (dsa)
		INFO_PRINTF3(_L("anim.CommandReply(EADllQueryDSA, screenNum) return value - Expected: %d, Actual: %d"), 0, dsa);

	screenNum() = 1;
	dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	TEST(!dsa);
	if (dsa)
		INFO_PRINTF3(_L("anim.CommandReply(EADllQueryDSA, screenNum) return value - Expected: %d, Actual: %d"), 0, dsa);

	// start first DSA - test that we have DSA on this screen but not other screen
	dsa1->StartL();
	screenNum() = 0;
	dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	TInt retVal = (screenNum() == iTest->iScreenNumber);
	TEST(dsa == retVal);
	if (dsa != retVal)
		INFO_PRINTF3(_L("dsa == (screenNum() == iTest->iScreenNumber) - Expected: %d, Actual: %d"), retVal, dsa);

	screenNum() = 1;
	dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	retVal = (screenNum() == iTest->iScreenNumber);
	TEST(dsa == retVal);
	if (dsa != retVal)
		INFO_PRINTF3(_L("dsa == (screenNum() == iTest->iScreenNumber) - Expected: %d, Actual: %d"), retVal, dsa);

	// start second DSA
	CScrollText* dsa2 = CScrollText::NewL(iTest->iScreenNumber,2,*TheClient->iGroup,5,EFalse);
	CleanupStack::PushL(dsa2);
	dsa2->StartL();
	screenNum() = 0;
	dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	retVal = (screenNum() == iTest->iScreenNumber);
	TEST(dsa == retVal);
	if (dsa != retVal)
		INFO_PRINTF3(_L("dsa == (screenNum() == iTest->iScreenNumber) - Expected: %d, Actual: %d"), retVal, dsa);

	screenNum() = 1;
	dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	retVal = (screenNum() == iTest->iScreenNumber);
	TEST(dsa == retVal);
	if (dsa != retVal)
		INFO_PRINTF3(_L("dsa == (screenNum() == iTest->iScreenNumber) - Expected: %d, Actual: %d"), retVal, dsa);

	// stop second DSA
	dsa2->Stop();
	CleanupStack::PopAndDestroy(dsa2);
	screenNum() = 0;
	dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	retVal = (screenNum() == iTest->iScreenNumber);
	TEST(dsa == retVal);
	if (dsa != retVal)
		INFO_PRINTF3(_L("dsa == (screenNum() == iTest->iScreenNumber) - Expected: %d, Actual: %d"), retVal, dsa);

	screenNum() = 1;
	dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	retVal = (screenNum() == iTest->iScreenNumber);
	TEST(dsa == retVal);
	if (dsa != retVal)
		INFO_PRINTF3(_L("dsa == (screenNum() == iTest->iScreenNumber) - Expected: %d, Actual: %d"), retVal, dsa);

	// stop first DSA
	dsa1->Stop();
	CleanupStack::PopAndDestroy(dsa1);
	screenNum() = 0;
	dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	TEST(!dsa);
	if (dsa)
		INFO_PRINTF3(_L("anim.CommandReply(EADllQueryDSA, screenNum) return value - Expected: %d, Actual: %d"), 0, dsa);

	screenNum() = 1;
	dsa=anim.CommandReply(EADllQueryDSA, screenNum);
	TEST(!dsa);
	if (dsa)
		INFO_PRINTF3(_L("anim.CommandReply(EADllQueryDSA, screenNum) return value - Expected: %d, Actual: %d"), 0, dsa);

	// TEST heartbeat timer start\stop notifications - no longer relevant - has been removed 

	CleanupStack::PopAndDestroy(&anim);
	
	// Create second RTestAnim for DEF12220 test
	RTestAnim anim2(iAnimDll);
	User::LeaveIfError(anim2.Construct(*iRedrawWin->BaseWin(),EAnimTypeNotificationTest,KNullDesC8));
	CleanupClosePushL(anim2);
	
	// Create third dsa
	CScrollText* dsa3 = CScrollText::NewL(iTest->iScreenNumber,1,*TheClient->iGroup,5,EFalse);
	CleanupStack::PushL(dsa3);
	// If the fix for DEF12220 is not present, starting dsa3 will cause a crash.
	dsa3->StartL();
	dsa3->Stop();
	CleanupStack::PopAndDestroy(dsa3);
	CleanupStack::PopAndDestroy(&anim2);
	}

/**
 A minimal dsa call to a window with a child window, for running coverage on
 ClipWindows member funcion.
 
@SYMTestCaseID GRAPHICS-WSERV-0410

@SYMPREQ PREQ1841

@SYMTestExpectedResults Nothing visible, the CWsClientWindow::ClipWindows 
should be partially covered.
 */
void CTAnimDll::TestCoverageL()
	{
	// create first DSA, but don't start it - test that we have no DSA on either screen
	CWindowWithChild* dsa1 = CWindowWithChild::NewL(iTest->iScreenNumber, *TheClient->iGroup, EFalse);
	CleanupStack::PushL(dsa1);

	// start first DSA - test that we have DSA on this screen but not other screen
	dsa1->StartL();
	dsa1->PerformCoverageCalls();
	dsa1->Stop();

	// cover another trivial case for a non-visible window
	RWindow* cwin = dsa1->ChildWindow();
	
	cwin->SetTransparencyAlphaChannel();
	dsa1->ContinueL();

	//ClipWindows with visible and invisible branches
	cwin->SetVisible(EFalse);

	//cover CWsWindow::StatusDump (visible and invisible branch)
	TheClient->iWs.LogCommand(RWsSession::ELoggingStatusDump);
	
	dsa1->Stop();
	dsa1->StartL();
	cwin->SetVisible(ETrue);
	dsa1->Stop();

	CleanupStack::PopAndDestroy(dsa1);
	}

/**
 Executes a Panic coverage test for a given test-number.
 Most of the first lines copied from DoPanicTest.
 CWsWindow is tested through CommandL (case 0-3) and other API funcs
 */
LOCAL_C TInt DoPanicTestCoverage(TInt aTestNum, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());

	// assign to the correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct(reinterpret_cast<TInt>(aScreenNumber)));

	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(333));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	RAnimDll animDll=RAnimDll(ws);
	User::LeaveIfError(animDll.Load(KAnimDLLName));
	
	RWindow window(ws);
	User::LeaveIfError(window.Construct(group,123));
	window.SetSize(TSize(10,10));
	window.Activate();
	window.BeginRedraw();
	window.EndRedraw();

	switch (aTestNum)
	{
	case 0: //non translucent window
		//EWsWinOpSetTransparentRegion -> EWservPanicTransparencyObjNotCreated
		{
		RRegion r;
		window.SetTransparentRegion(r);
		break;
		}
	case 1:
		//EWsWinOpSetTransparencyPolicy -> EWservPanicTransparencyObjNotCreated
		window.SetTransparencyPolicy(ETransparencyDefault);
		break;
	case 2:
		// -> EWservPanicWindowActive
		window.Activate();
		break;
	case 3:
		// -> EWservPanicCornerParams
		window.SetCornerType(EWindowCornerSquare, ECornerTypeMask);
		break;
	case 4:
		// -> EWservPanicCornerParams
		window.SetCornerType(ECornerTypeMask, 0);
		break;
	default:
		return EWsExitReasonFinished;
	}
	ws.Flush();
	
	return(EWsExitReasonBad);	// Should never get here, but it's baaddd if it does
	}

/**
@SYMTestCaseID GRAPHICS-WSERV-0411

@SYMPREQ PREQ1841

@SYMTestCaseDesc Does functional coverage for code that Panics

*/
void CTAnimDll::TestPanicCoverageL()
	{
	TClientPanic expectedPanicCode[] = 
		{
		EWservPanicTransparencyObjNotCreated,	//0
		EWservPanicTransparencyObjNotCreated,	//1
		EWservPanicWindowActive,				//2
		EWservPanicCornerParams,				//3
		EWservPanicCornerParams,				//4
		};
	for (TInt i=0; i<sizeof(expectedPanicCode)/sizeof(TClientPanic); i++)
		{
		TEST(iTest->TestWsPanicL(
				DoPanicTestCoverage,			//aFunction
				expectedPanicCode[i],			//aExitReason
				i,								//aInt
				(TAny*)iTest->iScreenNumber,	//aPtr
				NULL)); //&finishTests			//aTestFinished
		iTest->CloseAllPanicWindows();
		}
	}


/**
@SYMTestCaseID GRAPHICS-CODEBASE-WSERV-0056-0001

@SYMPREQ PGM027

@SYMTestCaseDesc Tests RAnimDll::Load (by passing invalid file name)

@SYMTestPriority 1 

@SYMTestStatus Implemented

@SYMTestActions Call RAnimDll::Load() by passing invalid file name\n

@SYMTestExpectedResults Should return with appropiate error message. KErrNotFound
 */
void CTAnimDll::TestLoadApiL()	
	{
	RAnimDll* animDll=new(ELeave) RAnimDll(TheClient->iWs);
	CleanupStack::PushL(animDll);
	TInt ret=0;
	_LIT(KAnimFile,"Nothing.dll");
	_LIT(KEmpty,"");
	ret=animDll->Load(KAnimFile);
	TEST(ret==KErrNotFound);
	ret=animDll->Load(KEmpty);
	TEST(ret==KErrNotFound);
	CleanupStack::PopAndDestroy(animDll);
	}

TInt CTAnimDll::CheckError(TInt aError,TInt aExpected)
	{
	if (aError!=0)
		{
		if (aError>0)
			{
			_LIT(KLog,"The event at position %d did not match, total number of events=%d");
			LOG_MESSAGE3(KLog,aError,aExpected);
			}
		else
			{
			_LIT(KLog,"Only recieved %d events, expecting %d events");
			LOG_MESSAGE3(KLog,-aError,aExpected);
			}
		return EFalse;
		}
	return ETrue;
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0448

@SYMCR				CR1164

@SYMTestCaseDesc	Test events get to Anims at a higher priorty than commands

@SYMTestPriority	High

@SYMTestStatus		Implemented

@SYMTestActions		Create a 2nd thread that sends events via kernal, send also commands from client to an Anim

@SYMTestExpectedResults	Check that events intersperse commands from client
*/
void CTAnimDll::KeyClickPriorityL()
	{
	_LIT(KFailedAdd,"Device Driver Failed to Add an Event, error=%d");
	REventAnim* anim=REventAnim::NewL(iTestWin->BaseWin(),&iAnimDll);
	TPckgBuf<TInt> events;
	TInt err=anim->CommandReply(EADllLoadDeviceDriver);
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to load Device Driver for sending Events");
		TEST(EFalse);
		LOG_MESSAGE(KLog);
		}
	TInt error;
	TInt ii;
	for (ii=1;ii<5;++ii)
		{
		events()=ii;
		error=anim->CommandReply(EADllSendEvent,events);
		if (error!=KErrNone)
			LOG_MESSAGE2(KFailedAdd,error);
		anim->Command(EADllAfterEvent);
		TheClient->Flush();
		error=anim->TestFail(ii);
		TEST(CheckError(error,ii));
		}
	const TInt iterations=15;
	const TInt eventsPerIteration=2;
	const TInt totalEvents=iterations*eventsPerIteration;
	events()=eventsPerIteration;
	for (ii=iterations;ii>0;--ii)
		{
		error=anim->CommandReply(EADllSendEvent,events);
		if (error!=KErrNone)
			LOG_MESSAGE2(KFailedAdd,error);
		anim->Command(EADllAfterEvent);
		}
	TheClient->Flush();
	error=anim->TestFail(totalEvents);
	TEST(CheckError(error,totalEvents));
	anim->CommandReply(EADllUnloadDeviceDriver);
	delete anim;
	}

/**
@SYMDEF             INC117828

@SYMTestCaseDesc    Tests Window Rect returned by CWsAnim::Parameters

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     This test creates a new window which overlaps the screen edge
					and creates an animation on this window. An additional
					CommandReplyL() option has been added to the AnimDLL to compare
					the expected value of this rect with the actual value.
					

@SYMTestExpectedResults The result of the EADllTestParameterRect CommandReplyL call will equal ETrue
						if the passed Rect matches, EFalse otherwise.
*/
void CTAnimDll::ParameterValueTestL()
	{
	//Window Location & Dimension
	TPoint winOrigin(-1,15);
	TSize winSize(250,65);
	
	//Create a new window to perform the test; window needs to overlap the left edge of the screen.
	CAnimWindow* paramTestWin = new(ELeave) CAnimWindow(EFalse, CAnimWindow::ERedraw);
	CleanupStack::PushL(paramTestWin);
	paramTestWin->ConstructL(winOrigin, winSize);

	RTestAnim anim = RTestAnim(iAnimDll);

	TPckgBuf<TRect> rectPckg;
	TRect winRect(winOrigin, winSize);
	rectPckg() = winRect;

	anim.Construct(*(paramTestWin->BaseWin()), EAnimTypeTest3, rectPckg);
	anim.Command(EADllStartAnimate, rectPckg);

	TPckgBuf<TBool> boolPckg;
	boolPckg() = ETrue;
	anim.Command(EADllSetVisible, boolPckg);

	TEST(anim.CommandReply(EADllParameterRectValueTest, rectPckg));

	boolPckg() = EFalse;
	anim.Command(EADllSetVisible, boolPckg);
	anim.Command(EADllCancelAnimate);	
	anim.Close();
	CleanupStack::PopAndDestroy(paramTestWin);
	}

/**
@SYMDEF             DEF122176

@SYMTestCaseDesc    Test the operation of SetInterval with both negative and positive intervals

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     This test uses the test anim CAutoAnim3 in mode 3 which alternates
					between drawing a large and a small ellipse at each interval.
					The large ellipse is drawn first.
					Note that the anims are redrawn at the rate of 2 intervals per second, so
					setting an interval of 2 will mean that the anim is redrawn every second.
						Step 1: The interval is set to be -2 and the test then waits for 0.9
								seconds (to make sure one interval has passed) then checks that 
								only the large ellipse has been drawn.							
						Step 2: The interval is set to be 2 (redraw every second) and the test then 
								waits for 1.2 seconds (to make sure two intervals have passed) and 
								checks that the small ellipse has been drawn.
					
@SYMTestExpectedResults 
					After Step 1 a large ellipse will be drawn on both windows.
					After Step 1 a small ellipse will be drawn on both windows. 

*/
void CTAnimDll::TestSetIntervalL()
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	
	// draws a little circle on the base window using window graphics and using window animation on the test window.
	RTestAnim drawAnim=RTestAnim(iAnimDll);

	_LIT(KLog1,"SetInterval Test");
	LOG_MESSAGE(KLog1);
	TRect rect(10,10,110,110);
	CAnimWindow::SetEllipseDrawMode(CGraphicsContext::EDrawModePEN);
	iTestWin->Invalidate();
	iTestWin->SetRect(rect);
	iBaseWin->Invalidate();
	iBaseWin->SetRect(rect);
	TPckgBuf<TRect> rectPckg;
	rectPckg()=rect;
	User::LeaveIfError(drawAnim.Construct(*iTestWin->BaseWin(),EAnimTypeTest3,rectPckg));	
	
	TPckgBuf<TInt> intPckg;
	intPckg() = 3; // mode 3 for this anim flips between drawing a large and a small ellipse
	drawAnim.Command(EADllSetMode, intPckg);
	
	intPckg() = MAnimGeneralFunctions::ESyncNone;
	drawAnim.CommandReply(EADllDoSetSync, intPckg);	
	
	// Set an interval of -2 then wait 0.9 seconds, after which only the large ellipse should be visible 
	intPckg() = -2;
	drawAnim.CommandReply(EADllSetInterval, intPckg);
	// Draw the large ellipse on the base window to match what the anim should be drawing
	iBaseWin->SetRect(rect);	
	iBaseWin->DoDraw();
	ws.Finish();	
	// Check Step 1 completed successfully
	RedrawAndCheckWindows();	
	
	// Set an interval of 2 and wait 0.9 seconds, after which the small ellipse should be visible	
	intPckg() = 2;
	drawAnim.CommandReply(EADllSetInterval, intPckg);
	// Draw the small ellipse on the base window to match what the anim should be drawing
	iBaseWin->SetRect(TRect(40,40,80,80));	
	iBaseWin->DoDraw();	
	ws.Finish();
	// Check Step 2 completed successfully
	RedrawAndCheckWindows();		

	// clean up
	drawAnim.Close();
	ws.Close();
	}

void CTAnimDll::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest0,"RemoteBuffer");
	_LIT(KTest1,"ExtraCopyTest");
	_LIT(KTest2,"Misc");
	_LIT(KTest3,"SyncMode1");
	_LIT(KTest4,"Anim Dll Panics");
	_LIT(KTest5,"Drawing test");
	_LIT(KTest6,"Shadow drawing test");
	_LIT(KTest7,"General drawing test");
	_LIT(KTest8,"Destroy window test");
	_LIT(KTest9,"Sprite Anim test");
	_LIT(KTest10,"Free Timer test");
	_LIT(KTest11,"Disable Timer test");
	_LIT(KTest12,"Multiple Anims");
	_LIT(KTest13,"ReLoad Click PlugIn");
	_LIT(KTest14,"Window Functions");
	_LIT(KTest15,"Window Functions 2");
	_LIT(KTest16,"Notifications");
	_LIT(KTest17,"Event Handler removal");
	_LIT(KTest18,"Anim dll - Load Api Negative Test");
	_LIT(KTest19,"Key Click Priority Test");
	_LIT(KTest20,"CWsAnim::Parameters() WindowRect Value Test");
	_LIT(KTest21,"SetInterval test");
	_LIT(KTest22,"Client Window Coverage");
	_LIT(KTest23,"Client Window Panic Coverage");
	((CTAnimDllStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

//	if (iTest->iState==0) iTest->iState=22;	//Use this line to start running tests from a particular test
	switch(++iTest->iState)
		{
		case 1:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0127"));
			iTest->LogSubTest(KTest0);
			RemoteBuffer();
			break;
		case 2:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0128"));
			iTest->LogSubTest(KTest1);
			ExtraCopyTest();
			break;
		case 3:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0126"));
			iTest->LogSubTest(KTest2);
			Misc();
			break;
		case 4:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0503
*/
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0503"));
			iTest->LogSubTest(KTest3);
// Disabled temporarily (process isolation/ RThread::RequestComplete issue) to allow WSERV test to continue
			break;
		case 5:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0125"));
			iTest->LogSubTest(KTest4);
			TestPanicsL();
			if (iTest->iScreenNumber == 1)
				iTest->CloseAllPanicWindows();
			break;
		case 6:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0129"));
			iTest->LogSubTest(KTest5);
			DrawingTestL();
			break;
		case 7:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0504
*/
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0504"));
			iTest->LogSubTest(KTest6);
// Disabled temporarily (process isolation/ RThread::RequestComplete issue) to allow WSERV test to continue
			break;
		case 8:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0130"));
			iTest->LogSubTest(KTest7);
			GeneralDrawingTestL();
			break;
		case 9:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0131"));
			iTest->LogSubTest(KTest8);
			DestroyWindowTestL();
			break;
		case 10:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0132"));
			iTest->LogSubTest(KTest9);
			SpriteAnimL();
			break;
		case 11:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0133"));
			iTest->LogSubTest(KTest10);
			FreeTimerL();
			break;
		case 12:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0134"));
			iTest->LogSubTest(KTest11);
			DisableTimerL();
			break;
		case 13:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0135"));
			iTest->LogSubTest(KTest12);
			MultipleAnimsL();
			break;
		case 14:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0136"));
			iTest->LogSubTest(KTest13);
			ClickPlugInL();
			break;
		case 15:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0137"));
			iTest->LogSubTest(KTest14);
			WinFunctionsL();
			break;
		case 16:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0138"));
			iTest->LogSubTest(KTest15);
			WinFunctions2L();
			break;
		case 17:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0139"));
			iTest->LogSubTest(KTest16);
			TestNotificationsL();
			break;
		case 18:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0137"));
			iTest->LogSubTest(KTest17);
			TestEventHandlerRemovalL();
			break;
		case 19:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-CODEBASE-WSERV-0056-0001"));
			iTest->LogSubTest(KTest18);			
			TestLoadApiL();
			break;
		case 20:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0448"));
			iTest->LogSubTest(KTest19);
			KeyClickPriorityL();
			break;
		case 21:
 /**
@SYMTestCaseID		GRAPHICS-WSERV-0505
*/
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0505"));
			iTest->LogSubTest(KTest20);
			ParameterValueTestL();
			break;
		case 22:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0506
*/
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0506"));
			iTest->LogSubTest(KTest21);
			TestSetIntervalL();
		case 23:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0410"));
			iTest->LogSubTest(KTest22);			
			TestCoverageL();
			break;
		case 24:
			((CTAnimDllStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0411"));
			iTest->LogSubTest(KTest23);
			TestPanicCoverageL();
		default:
			((CTAnimDllStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTAnimDllStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
		}
	User::Check();
	((CTAnimDllStep*)iStep)->RecordTestResultL();
	}

//

CAnimRedrawWindow::CAnimRedrawWindow(CAnimWindow *aAnimWindow, TBool aIsBase) : CTWin(),
	iAnimWindow(aAnimWindow),
	iIsBase(aIsBase)
	{
	}

CAnimRedrawWindow::~CAnimRedrawWindow()
	{
	}

void CAnimRedrawWindow::Draw()
	{
	CAnimWindow::Draw(Gc(),Size(),iIsBase,iAnimWindow->iRect,EFalse);
	}

//

CAnimWindow::CAnimWindow(TBool aIsBase, TWinType aWinType) : iWinType(aWinType), iIsBase(aIsBase)
	{
	}


CAnimWindow::~CAnimWindow()
	{
	delete iCtWin;
	}

void CAnimWindow::ConstructL(const TPoint &aPos, const TSize &aSize)
	{
	switch(iWinType)
		{
		case ERedraw:
			iCtWin=new(ELeave) CAnimRedrawWindow(this, iIsBase);
			break;
		case EBlank:
			iCtWin=new(ELeave) CTBlankWindow();
			break;
		case EBackedUp:
			iCtWin=new(ELeave) CTBackedUpWin(EGray4);
			break;
		}
	iCtWin->SetUpL(aPos, aSize, TheClient->iGroup, *TheClient->iGc);
	if (iWinType==ERedraw)
		{
		static_cast<CTWin*>(iCtWin)->Win()->EnableRedrawStore(EFalse);
		}
	}

void CAnimWindow::SetEllipseDrawMode(CGraphicsContext::TDrawMode aEllipseDrawMode)
	{
	iEllipseDrawMode=aEllipseDrawMode;
	}

void CAnimWindow::SetRect(const TRect &aRect)
	{
	iRect=aRect;
	}

void CAnimWindow::DrawEllipse(CBitmapContext *aGc, const TRect &aRect)
	{
	aGc->SetDrawMode(iEllipseDrawMode);
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetBrushColor(TRgb(85,85,85));
	aGc->SetPenColor(TRgb(170,170,170));
	aGc->DrawEllipse(aRect);
	}

void CAnimWindow::Draw(CBitmapContext *aGc, const TSize &aSize, TBool aIsBase, const TRect &aRect, TBool aBlankIt)
	{
	aGc->SetBrushColor(TRgb::Gray4(2));
	aGc->Clear();
	if (!aBlankIt)
		for(TInt i=0;i<aSize.iWidth;i+=10)
			aGc->DrawLine(TPoint(i,0),TPoint(i+10,aSize.iHeight));
	if (aIsBase)
		DrawEllipse(aGc, aRect);
	}

void CAnimWindow::DoDraw(TBool aBlankIt)
	{
	__ASSERT_ALWAYS(iWinType!=EBlank,AutoPanic(EAutoPanicWindowType));
	iCtWin->Gc()->Activate(*(RDrawableWindow *)BaseWin());
	Draw(iCtWin->Gc(),Size(),iIsBase,iRect,aBlankIt);
	iCtWin->Gc()->Deactivate();
	}

void CAnimWindow::DoDrawEllipse()
	{
	__ASSERT_ALWAYS(iWinType!=EBlank,AutoPanic(EAutoPanicWindowType));
	iCtWin->Gc()->Activate(*(RDrawableWindow *)BaseWin());
	DrawEllipse(iCtWin->Gc(),iRect);
	iCtWin->Gc()->Deactivate();
	}

void CAnimWindow::DoDrawCoverage()
	{
	__ASSERT_ALWAYS(iWinType!=EBlank,AutoPanic(EAutoPanicWindowType));
	CWindowGc *gc = iCtWin->Gc();
	gc->Activate(*(RDrawableWindow *)BaseWin());

	gc->SetClippingRect(iRect);
	gc->Clear();
	gc->Clear(iRect);

	gc->SetDrawMode(CGraphicsContext::EDrawModeAND);
	gc->SetBrushStyle(CGraphicsContext::ENullBrush);
	gc->SetBrushColor(TRgb::Gray256(85));
	gc->SetBrushOrigin(TPoint(0,0));
	gc->SetPenColor(TRgb::Gray256(170));

	//primitive method calls
	gc->SetFaded(ETrue);
	gc->SetFadingParameters(1, 1);

	gc->SetPenStyle(CGraphicsContext::ESolidPen);
	gc->SetStrikethroughStyle(EStrikethroughOff);
	gc->SetUnderlineStyle(EUnderlineOff);
	gc->SetWordJustification(2, 1);

	CFbsFont* font;
	gc->Device()->GetNearestFontInTwips((CFont*&)font, TFontSpec());

	gc->UseFont(font);
	
	gc->DrawArc(iRect,
			TPoint(iRect.Center().iX, iRect.iTl.iY),
			TPoint(iRect.iBr.iX, iRect.Center().iY));
	gc->DrawLine(iRect.iTl,iRect.Center());
	gc->DrawLineTo(TPoint(iRect.iBr.iX, iRect.iTl.iY));

	gc->DrawLineBy(TPoint(iRect.iTl.iX, iRect.iBr.iY));
	gc->MoveBy(iRect.iTl + TPoint(1,1));
	gc->MoveTo(iRect.iTl + TPoint(0,0)); 
	gc->SetPenSize(TSize(10,10));
	gc->Plot(iRect.iTl + TPoint(2,2));
	gc->SetPenSize(TSize(1,1));

	CArrayFixFlat<TPoint>* polyPoints = new(ELeave) CArrayFixFlat<TPoint>(3); //CArrayFixFlat
	CleanupStack::PushL(polyPoints);
	polyPoints->AppendL(iRect.iTl);
	polyPoints->AppendL(iRect.Center());
	polyPoints->AppendL(TPoint(iRect.iBr.iX, iRect.iTl.iY));
	
	gc->DrawPolyLine(polyPoints);
	gc->DrawPolyLine(&polyPoints->At(0), 3);
	gc->DrawPolygon(polyPoints, CGraphicsContext::EWinding);
	gc->DrawPolygon(&polyPoints->At(0), 3, CGraphicsContext::EAlternate);

	gc->DrawPie(iRect, TPoint(iRect.Center().iX, iRect.iTl.iY), TPoint(iRect.iBr.iX, iRect.Center().iY));
	gc->DrawEllipse(iRect);
	gc->DrawRect(iRect);
	gc->DrawRoundRect(iRect, TSize(iRect.Width()/8, iRect.Height()/8));

	CleanupStack::PopAndDestroy(polyPoints);

	gc->CopyRect(TPoint(10, 10), iRect);

	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(16,16),EGray4));

	gc->UseBrushPattern(bitmap);
	gc->DrawBitmap(iRect.iTl, bitmap);
	gc->DrawBitmap(iRect, bitmap);
	gc->DrawBitmap(iRect, bitmap, TRect(0, 0, 16, 16));
	gc->DrawBitmapMasked(iRect, bitmap, TRect(0, 0, 16, 16), bitmap, ETrue);

	gc->BitBlt(TPoint(0, 0), bitmap);
	gc->BitBlt(TPoint(0, 0), bitmap, iRect);
	gc->BitBltMasked(TPoint(0, 0), bitmap, iRect, bitmap, ETrue);
	gc->AlphaBlendBitmaps(TPoint(0, 0), bitmap, iRect, bitmap, TPoint(0,0));
	
	CleanupStack::PopAndDestroy(bitmap);

	_LIT(KHelloWorld,"Hello World");
	gc->SetCharJustification(1,1);
	gc->SetClippingRect(iRect);
	gc->DrawText(*&KHelloWorld, iRect.iTl);
	gc->DrawText(*&KHelloWorld, iRect, 0, CGraphicsContext::ELeft, 0);
	gc->DrawTextVertical(*&KHelloWorld, iRect.iBr, ETrue);
	gc->DrawTextVertical(*&KHelloWorld, iRect, 0, ETrue, CGraphicsContext::ELeft, 0);

	TRgb rgbs[2];
	gc->MapColors(iRect, rgbs, 1, ETrue);
	gc->DiscardBrushPattern();

	gc->DiscardFont();
	gc->Device()->ReleaseFont(font);

	gc->Deactivate();
	}

TSize CAnimWindow::Size()
	{
	return(iCtWin->Size());
	}

RWindowBase *CAnimWindow::BaseWin() const
	{
	return(iCtWin->BaseWin());
	}

CTBaseWin *CAnimWindow::CtBaseWin()
	{
	return(iCtWin);
	}

void CAnimWindow::Invalidate()
	{
	CTUser::Splat(TheClient,TRect(iCtWin->Position(),iCtWin->Size()),TRgb::Gray256(0));
	}

void CAnimWindow::Invalidate(const TRect &aRect)
	{
	TRect rect(aRect);
	rect.Move(iCtWin->Position());
	CTUser::Splat(TheClient,rect,TRgb::Gray256(0));
	}

void CAnimWindow::DrawTestScreen(CFbsBitmap *aBitmap, CFbsBitmap *aMaskBitmap, CFbsFont *aFont)
	{
	iCtWin->Gc()->Activate(*(RDrawableWindow *)BaseWin());
	CBitmapContext *gc=iCtWin->Gc();
	TSize size(Size());
	TBool aExtraDrawBitMap=ETrue;
//
#include "DLLDRAW.H"
//
	iCtWin->Gc()->Deactivate();
	}


__WS_CONSTRUCT_STEP__(AnimDll)
