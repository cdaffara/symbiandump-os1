// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test the Wserv heart beat switch on and off
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "THeartBeat.h"
#include <w32debug.h>

CTHeartBeatTest::CTHeartBeatTest(CTestStep* aStep) :
	CTWsGraphicsBase(aStep)
	{
	}

CTHeartBeatTest::~CTHeartBeatTest()
	{
	iAnimDll.Close();
	delete iHbTestWin;
	}

void CTHeartBeatTest::ConstructL()
	{
	TSize screenSize=TheClient->iGroup->Size();
	TInt winWidth=(screenSize.iWidth/3)-10;
	TInt winHeight=screenSize.iHeight-10;
	iHbTestWin=new(ELeave) CAnimWindow(EFalse, CAnimWindow::ERedraw);
	iHbTestWin->ConstructL(TPoint(screenSize.iWidth/3*2+5,5),TSize(winWidth,winHeight));
	iAnimDll=RAnimDll(TheClient->iWs);
	User::LeaveIfError(iAnimDll.Load(KAnimDLLName));
	}

void CTHeartBeatTest::SetCursor(const TPoint& aPos,const TSize& aSize, TUint aFlags/*=0*/)
	{ // Default aFlags=0 gives a flashing cursor
	TTextCursor tc;
	tc.iType=TTextCursor::ETypeRectangle;
	tc.iHeight=aSize.iHeight;
	tc.iAscent=aSize.iHeight*4/5;
	tc.iWidth=aSize.iWidth;
	tc.iFlags=aFlags;	
	tc.iColor=KRgbGreen;
	TheClient->iGroup->GroupWin()->SetTextCursor(*iHbTestWin->BaseWin(),TPoint(aPos.iX,aPos.iY+tc.iAscent),tc);
	}

TBool CTHeartBeatTest::SetAutoHeartBeatPauseState(TBool aState) const
	{
	return(TheClient->iWs.DebugInfo(EWsDebugSetAutoHeartBeatPauseState,aState));
	}

TBool CTHeartBeatTest::GetHeartBeatTimerState() const
	{
	return(TheClient->iWs.DebugInfo(EWsDebugHeartBeatState));
	}

void CTHeartBeatTest::CancelTextCursor()
	{
	TheClient->iGroup->GroupWin()->CancelTextCursor();
	}

void CTHeartBeatTest::TestTextCursorTimerL()
	{
	TEST(!GetHeartBeatTimerState());
	SetCursor(TPoint(0,0),TSize(10,10));
	TEST(GetHeartBeatTimerState());
	CancelTextCursor();
	TEST(!GetHeartBeatTimerState());
//
	TPoint testCursorPos;
	TSize testCursorSize(10,10);
	SetCursor(testCursorPos,testCursorSize);
	TEST(GetHeartBeatTimerState());
	iHbTestWin->BaseWin()->SetVisible(EFalse);
	TEST(!GetHeartBeatTimerState());
	iHbTestWin->BaseWin()->SetVisible(ETrue);
	TEST(GetHeartBeatTimerState());
//
// Cover the cursor and check heartbeat disabled
	RWindow coverCursor(TheClient->iWs);
	CleanupClosePushL(coverCursor);
	User::LeaveIfError(coverCursor.Construct(*iHbTestWin->BaseWin(), 0xBADBAD));
	coverCursor.SetExtent(testCursorPos,testCursorSize);
	coverCursor.Activate();
	TEST(!GetHeartBeatTimerState());
// Now fractionally expose the cursor and check heartbeat re-enabled
	coverCursor.SetExtent(testCursorPos+TPoint(1,1),testCursorSize);
	TEST(GetHeartBeatTimerState());
// Re-cover it
	coverCursor.SetExtent(testCursorPos,testCursorSize);
	TEST(!GetHeartBeatTimerState());
// Enlarge cursor to expose itself then check heartbeat
	SetCursor(testCursorPos,TSize(testCursorSize.iWidth+1,testCursorSize.iHeight));
	TEST(GetHeartBeatTimerState());
// Shrink cursor then check heartbeat
	SetCursor(testCursorPos,testCursorSize);
	TEST(!GetHeartBeatTimerState());
// Turn off auto heart beat pausing, should turn timer back on
	SetAutoHeartBeatPauseState(EFalse);
	TEST(GetHeartBeatTimerState());
// With auto pausing enabled covering/uncovering the cursor should have no effect,
// timer will always be on
	coverCursor.SetVisible(EFalse);
	TEST(GetHeartBeatTimerState());
	coverCursor.SetVisible(ETrue);
	TEST(GetHeartBeatTimerState());
// Re-enable auto-pause should turn timer back off
	SetAutoHeartBeatPauseState(ETrue);
	TEST(!GetHeartBeatTimerState());
// Destroy covering window and check heartbeat re-enabled
	CleanupStack::PopAndDestroy(&coverCursor);
	TEST(GetHeartBeatTimerState());
// Check using no flash flag turns timer off
	SetCursor(testCursorPos,testCursorSize,TTextCursor::EFlagNoFlash);
	TEST(!GetHeartBeatTimerState());
// and turning no flash flag off turns timer back on
	SetCursor(testCursorPos,testCursorSize);
	TEST(GetHeartBeatTimerState());
// Preparing for switch off should disable the heartbeat until another event is received
	TheClient->iWs.PrepareForSwitchOff();
	TEST(!GetHeartBeatTimerState());
// Any event should wake the hearbeat back up again
	TRawEvent rawEvent;
	rawEvent.Set(TRawEvent::EPointerMove,0,0);
	TheClient->iWs.SimulateRawEvent(rawEvent);
	TEST(GetHeartBeatTimerState());
//
	CancelTextCursor();
	TEST(!GetHeartBeatTimerState());
// Check heartbeat still off as expected
	TheClient->iWs.PrepareForSwitchOff();
	TEST(!GetHeartBeatTimerState());
// Check when hearbeat not required that it's not turned back when coming out of PrepareForSwitchOff() mode.
	TheClient->iWs.SimulateRawEvent(rawEvent);
	TheClient->Flush();
	TEST(!GetHeartBeatTimerState());
	}

void CTHeartBeatTest::doTestActiveInactiveEvents(TBool aCursorOn, TBool aAutoPauseOn)
	{
	SetAutoHeartBeatPauseState(aAutoPauseOn);
	if (aCursorOn)
		SetCursor(TPoint(0,0),TSize(10,10));
// Should disable timer, regardless of other states
	TRawEvent event;
	event.Set(TRawEvent::EInactive);
	UserSvr::AddEvent(event);
	TEST(!GetHeartBeatTimerState());
// ...and switch it back on
	event.Set(TRawEvent::EActive);
	UserSvr::AddEvent(event);
	TBool testResult1=EFalse;
	if (aAutoPauseOn)
		testResult1=aCursorOn; // Only on if cursor is on without auto pause enabled
	else
		testResult1=ETrue; // Always on if auto pause disabled
	TBool hbState=GetHeartBeatTimerState();
	if ((!hbState)!=(!testResult1))
		{
		_LIT(KLog,"Fail in TestActiveInactiveEvents(%d.%d)  HBState=%d result=%d");
		LOG_MESSAGE5(KLog,aCursorOn,aAutoPauseOn,hbState,testResult1);
		}
	TEST((!GetHeartBeatTimerState())==(!testResult1));		//Works with any positive value for True
//
	if (aCursorOn)
		CancelTextCursor();
	}

void CTHeartBeatTest::TestActiveInactiveEvents()
	{
	doTestActiveInactiveEvents(EFalse,EFalse);
	doTestActiveInactiveEvents(ETrue,EFalse);
	doTestActiveInactiveEvents(EFalse,ETrue);
	doTestActiveInactiveEvents(ETrue,ETrue);
	}

void CTHeartBeatTest::FailCursorHbTest(const TDesC& aErrorBase, const TDesC* aExtraText, TBool aHbState, TBool aHbCheckState)
	{
	TBuf<256> error(aErrorBase);
	if (aExtraText)
		{
		error.Append(*aExtraText);
		}
	_LIT(KHbErrorAppendFmt," HB=%d (Testing for %d)");
	error.AppendFormat(KHbErrorAppendFmt,aHbState,aHbCheckState);
	LOG_MESSAGE(error);
	TEST(EFalse);
	}

TBool CTHeartBeatTest::ChangeTransAndCheckHeartBeat(TInt aTestMode, RWindow& aTransWin, const TRect& aGraphicRect, TInt aExtraWinTestMode, const TDesC* aErrorMsg, TBool aSetVisibleAfterTransUpdate)
	{
	TRect cursorRect(aGraphicRect);
	cursorRect.Move(iHbTestWin->BaseWin()->InquireOffset(aTransWin));
	TRect winRect(aTransWin.Size());
	__ASSERT_ALWAYS(winRect.Contains(cursorRect.iTl) && winRect.Contains(cursorRect.iBr),AutoPanic(EAutoPanicHeartBeatCursorRect));
//
	TRegionFix<8> transRegion;
	TBool heartBeatCheck=EFalse;
	switch(aTestMode)
		{
		case ECursorTransWinTestModeFullTrans:
			transRegion.AddRect(winRect);
			heartBeatCheck=ETrue;
			break;
		case ECursorTransWinTestNoTrans:
			break;
		case ECursorTransWinTestPartialTransCoveringCursor:
			transRegion.AddRect(winRect);
			transRegion.SubRect(cursorRect);
			break;
		case ECursorTransWinTestPartialTransExposingCursor:
			transRegion.AddRect(cursorRect);
			heartBeatCheck=ETrue;
			break;
		default:
			ASSERT(EFalse);
		}
	TBool invisHeartBeatCheck=ETrue;
	switch(aExtraWinTestMode)
		{
		case ECursorTransWinTestNoTrans:
		case ECursorTransWinTestPartialTransCoveringCursor:
			heartBeatCheck=EFalse;
			invisHeartBeatCheck=EFalse;
			break;
		default:
			break;
		}
//
	ASSERT(!transRegion.CheckError());
	if (aSetVisibleAfterTransUpdate)
		aTransWin.SetVisible(EFalse);
	aTransWin.SetTransparentRegion(transRegion);
	if (aSetVisibleAfterTransUpdate)
		aTransWin.SetVisible(ETrue);
//
	TBool testSuccess=ETrue;
	TBool heartBeatState=GetHeartBeatTimerState();
	TBool failed1stTest=EFalse;
	if ((!heartBeatState)!=(!heartBeatCheck))		//Works with any positive value for True
		{
		failed1stTest=ETrue;
		_LIT(KFailedHbTest,"Failed ");
		FailCursorHbTest(KFailedHbTest,aErrorMsg,heartBeatState,heartBeatCheck);
		testSuccess=EFalse;
		}
//
	aTransWin.SetVisible(EFalse);
	heartBeatState=GetHeartBeatTimerState();
	_LIT(KGoingInvisAfterTestAppend," after %S");
	TBuf<256> errorExtra;
	if (aErrorMsg)
		{
		errorExtra.Format(KGoingInvisAfterTestAppend,aErrorMsg);
		}
	if ((!heartBeatState)!=(!invisHeartBeatCheck))		//Works with any positive value for True
		{
		_LIT(KFailedGoingInvis,"Failed setting trans win invisible");
		FailCursorHbTest(KFailedGoingInvis,&errorExtra,heartBeatState,invisHeartBeatCheck);
		testSuccess=EFalse;
		}
//
	aTransWin.SetVisible(ETrue);
	heartBeatState=GetHeartBeatTimerState();
	if (!failed1stTest && (!heartBeatState)!=(!heartBeatCheck))		//Works with any positive value for True
		{
		_LIT(KFailedResettingVis,"Failed re-setting trans win visible");
		FailCursorHbTest(KFailedResettingVis,&errorExtra,heartBeatState,heartBeatCheck);
		testSuccess=EFalse;
		}
	return(testSuccess);
	}

void CTHeartBeatTest::LoopThroughTransWinTestModes(RWindow& aTransWin, const TRect& aGraphicRect, TInt aExtraWinTestMode)
	{
	TBuf<256> error;
	for(TInt setVisMode=0;setVisMode<2;setVisMode++)
		{
		if (setVisMode==0)
			continue;	// zzz setinvis/makevis needed to flush out changes it seems
		const TBool setVisibleAfterTransUpdate=setVisMode>0;
		for(TInt startTestMode=0;startTestMode<ENumCursorTransWinTestModes;startTestMode++)
			{
			if (startTestMode==ECursorTransWinTestNoTrans)
				continue;// zzz bugged trans win problem
			_LIT(KTcHbModeSetFail,"setting test mode %d (vis mode %d)");
			error.Format(KTcHbModeSetFail,startTestMode,setVisibleAfterTransUpdate);
			for(TInt switchToTestMode=0;switchToTestMode<ENumCursorTransWinTestModes;switchToTestMode++)
				{
				if (switchToTestMode==ECursorTransWinTestNoTrans)
					continue;// zzz bugged trans win problem
				if (startTestMode==switchToTestMode)
					continue;
				if (ChangeTransAndCheckHeartBeat(startTestMode, aTransWin, aGraphicRect, aExtraWinTestMode, &error, setVisibleAfterTransUpdate))
					{
					_LIT(KTcHbModeSwitchFail,"switching from test mode %d to %d (vis mode %d)");
					error.Format(KTcHbModeSwitchFail, startTestMode, switchToTestMode, setVisibleAfterTransUpdate);
					ChangeTransAndCheckHeartBeat(switchToTestMode, aTransWin, aGraphicRect, aExtraWinTestMode, &error, setVisibleAfterTransUpdate);
					}
				}
			}
		ChangeTransAndCheckHeartBeat(0, aTransWin, aGraphicRect, aExtraWinTestMode, NULL, setVisibleAfterTransUpdate);
		}
	}

void CTHeartBeatTest::RunTransWinTestsL(const TRect& aGraphicRect)
	{
// Test a variety of patterns of changing the layout of transparent windows over the cursor
// cause the heartbeat state to be modified accordingly.
//
// Each transparent window has four specific states to test:
// i) Transparency area is the whole window
// ii) Partial transparency that exposes the cursor
// iii) Partial transparency that covers the cursor
// iv) Empty transparent region, i.e. totally opaque
//
// We need to test various transitions between these states, first on a single window, then
// in a various layouts with transparent windows layered over each other.
//
	RWindow transWin1(TheClient->iWs);
	User::LeaveIfError(transWin1.Construct(*TheClient->iGroup->GroupWin(),0xDEADBAD1));
	CleanupClosePushL(transWin1);
	if (transWin1.SetTransparencyFactor(TRgb::Gray256(128))!=KErrNone)
		{	// Transparency not enabled, skip tests
		CleanupStack::PopAndDestroy(&transWin1);
		return;
		}
	transWin1.Activate();
	TEST(GetHeartBeatTimerState());
// First simply check activating default full screen transparent window doesn't disable the heartbeat
	_LIT(KBasicTransCursorTests,"Running basic transparent window cursor heartbeat tests");
	LOG_MESSAGE(KBasicTransCursorTests);
	LoopThroughTransWinTestModes(transWin1, aGraphicRect, ECursorTransWinTestNA);
//
//  Lots of test code disabled due to bugs in handling of transparent window regions
// See DEF110677, hopefully when the issues in there are addressed the rest of the
// test code here can be enabled
//  See sections marked with zzz for places where certain tests are disabled, once DEF110677
// is marked as fixed these sections should be re-enabled and if problems still occur they
// need to be investigated.
//
//
// Now check more complex modes with two transparent windows over the test window
	RWindow transWin2(TheClient->iWs);
	User::LeaveIfError(transWin2.Construct(*TheClient->iGroup->GroupWin(),0xDEADBAD2));
	CleanupClosePushL(transWin2);
	User::LeaveIfError(transWin2.SetTransparencyFactor(TRgb::Gray256(128)));
	TSize screenSize=TheClient->iGroup->Size();
	screenSize.iWidth-=10;
	transWin1.SetExtent(TPoint(0,0),screenSize);
	transWin2.SetExtent(TPoint(10,0),screenSize);
	transWin2.Activate();
	for(TInt win1OrdPos=0;win1OrdPos<2;win1OrdPos++)
		{
		transWin1.SetOrdinalPosition(win1OrdPos);
		_LIT(KOrdinalTransCursorTests,"Running tests with transwin1 ordinal pos=%d");
		LOG_MESSAGE2(KOrdinalTransCursorTests,win1OrdPos);
		for(TInt topWinTestMode=0;topWinTestMode<ENumCursorTransWinTestModes;topWinTestMode++)
			{
			if (topWinTestMode==ECursorTransWinTestNoTrans)
				continue;// zzz bugged transparent window problem
			if (topWinTestMode==ECursorTransWinTestPartialTransCoveringCursor)
				continue;// zzz bugged transparent window problem
			_LIT(KTopTransWinCursorTest,"Setting top trans win mode %d");
			TBuf<256> msg;
			msg.Format(KTopTransWinCursorTest,topWinTestMode);
			LOG_MESSAGE(msg);
			ChangeTransAndCheckHeartBeat(0, transWin2, aGraphicRect, topWinTestMode, NULL, ETrue);
			LoopThroughTransWinTestModes(transWin1, aGraphicRect, topWinTestMode);
			}
		}
	ChangeTransAndCheckHeartBeat(0, transWin2, aGraphicRect, 0, NULL, ETrue);
	CleanupStack::PopAndDestroy(&transWin2);
	TEST(GetHeartBeatTimerState());
//
	CleanupStack::PopAndDestroy(&transWin1);
	TEST(GetHeartBeatTimerState());
	}

void CTHeartBeatTest::TestTextCursorTimerTransL()
	{
	TSize testWinSize(iHbTestWin->BaseWin()->Size());
	TRect cursorRect(testWinSize.iWidth/4,testWinSize.iHeight/3,testWinSize.iWidth*3/4,testWinSize.iHeight*2/3);
	TEST(!GetHeartBeatTimerState());
	SetCursor(cursorRect.iTl,cursorRect.Size());
	TEST(GetHeartBeatTimerState());
	RunTransWinTestsL(cursorRect);
	CancelTextCursor();
	TEST(!GetHeartBeatTimerState());
	}

void CTHeartBeatTest::TestHeartBeatStopStart(TBool aHeartbeatStateOff,TBool aHeartbeatStateOn,const TDesC& aOffText,const TDesC& aOnText)
	{
	TEST(!aHeartbeatStateOff);
	if (aHeartbeatStateOff)
		LOG_MESSAGE2(_L("%S failed to disable heartbeat timer"),&aOffText);
	TEST(aHeartbeatStateOn);
	if (!aHeartbeatStateOn)
		LOG_MESSAGE2(_L("%S failed to re-enable heartbeat timer"),&aOnText);
	}

void CTHeartBeatTest::TestAnimDllL(TBool aSpriteMode, TUint aSpriteFlags)
	{
	RTestAnim anim(iAnimDll);
	TEST(!GetHeartBeatTimerState());
	TSize testWinSize(iHbTestWin->BaseWin()->Size());
	TRect animRect(1,1,testWinSize.iWidth/3,testWinSize.iWidth/5);
	TPckgC<TRect> rectPckg(animRect);
	RWsSprite sprite(TheClient->iWs);
	CleanupClosePushL(sprite);
	if (aSpriteMode)
		{
		sprite.Construct(*iHbTestWin->BaseWin(),TPoint(),aSpriteFlags);
		CFbsBitmap* bitmap1=new(ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap1);
		CFbsBitmap* bitmap2=new(ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap2);
		CFbsBitmap* bitmap3=new(ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap3);
		TSize animSize(animRect.Size());
		User::LeaveIfError(bitmap1->Create(TSize(animSize.iWidth/2,animSize.iHeight/2),EGray4));
		User::LeaveIfError(bitmap2->Create(TSize(animSize.iWidth,animSize.iHeight/3),EGray4));
		User::LeaveIfError(bitmap3->Create(TSize(animSize.iWidth/3,animSize.iHeight),EGray4));
//
		TSpriteMember member1;
		member1.iMaskBitmap=NULL;
		member1.iInvertMask=EFalse;
		member1.iDrawMode=CGraphicsContext::EDrawModePEN;
		member1.iOffset=TPoint();
		member1.iInterval=TTimeIntervalMicroSeconds32(1);
		member1.iBitmap=bitmap1;
		member1.iMaskBitmap=bitmap1;
		sprite.AppendMember(member1);
//
		TSpriteMember member2;
		member2.iInvertMask=EFalse;
		member2.iDrawMode=CGraphicsContext::EDrawModeXOR;
		member2.iOffset=TPoint(1,2);
		member2.iInterval=TTimeIntervalMicroSeconds32(2);
		member2.iBitmap=bitmap2;
		member2.iMaskBitmap=bitmap2;
		sprite.AppendMember(member2);
//
		TSpriteMember member3;
		member3.iInvertMask=ETrue;
		member3.iDrawMode=CGraphicsContext::EDrawModeOR;
		member3.iOffset=TPoint(3,4);
		member3.iInterval=TTimeIntervalMicroSeconds32(3);
		member3.iBitmap=bitmap3;
		member3.iMaskBitmap=bitmap3;
		sprite.AppendMember(member3);
//
		User::LeaveIfError(anim.Construct(sprite,EAnimTypeSprite,rectPckg));
		CleanupStack::PopAndDestroy(3,bitmap1);
		}
	else
		{
		User::LeaveIfError(anim.Construct(*iHbTestWin->BaseWin(),EAnimTypeTest3,rectPckg));
		}
	CleanupClosePushL(anim);
	TEST(GetHeartBeatTimerState());

	// Cover up the window with the animation and check this disables the heartbeat timer.
	RBlankWindow blankwin1(TheClient->iWs);
	User::LeaveIfError(blankwin1.Construct(*TheClient->iGroup->GroupWin(),123));
	TPoint winOffset(iHbTestWin->BaseWin()->InquireOffset(*TheClient->iGroup->GroupWin()));
	if (aSpriteMode)
		blankwin1.SetExtent(winOffset,testWinSize);
	else
		blankwin1.SetExtent(winOffset+animRect.iTl,animRect.Size());
	blankwin1.Activate();
	TBool heartbeatStateOff=GetHeartBeatTimerState();
	TEST(!heartbeatStateOff);
	if (heartbeatStateOff)
		{
		LOG_MESSAGE(_L("Covering window failed to disable heartbeat timer"));
		}
	TBool heartbeatStateOn;
	if (!aSpriteMode)
		{
		// Resize the anim so it becomes visible, then shrink back down again
		animRect.iBr.iX++;
		anim.Command(EADllSetRect,rectPckg);
		heartbeatStateOn=GetHeartBeatTimerState();
		animRect.iBr.iX--;
		anim.Command(EADllSetRect,rectPckg);
		heartbeatStateOff=GetHeartBeatTimerState();
		TestHeartBeatStopStart(heartbeatStateOff,heartbeatStateOn,_L("Shrinking anim"),_L("Growing anim"));
		}
//
	blankwin1.Close();
	heartbeatStateOn=GetHeartBeatTimerState();
	TEST(heartbeatStateOn);
	if (!heartbeatStateOn)
		{
		LOG_MESSAGE(_L("Un-covering window failed to disable heartbeat timer"));
		}

	// Put a transparent window over the animation and check the heartbeat timer is not disabled
	RWindow transWin(TheClient->iWs);
	User::LeaveIfError(transWin.Construct(*TheClient->iGroup->GroupWin(),123));
	if (transWin.SetTransparencyFactor(TRgb::Gray256(128))==KErrNone)
		{
		transWin.Activate();
		heartbeatStateOn=GetHeartBeatTimerState();
		TEST(heartbeatStateOn);
		if (!heartbeatStateOn)
			LOG_MESSAGE(_L("Transparent window caused heartbeat timer to be disabled"));
		TRegionFix<1> emptyRegion;
		transWin.SetTransparentRegion(emptyRegion);
		heartbeatStateOff=GetHeartBeatTimerState();
		TRegionFix<1> fullRegion(transWin.Size());
		transWin.SetTransparentRegion(fullRegion);
		heartbeatStateOn=GetHeartBeatTimerState();
		TestHeartBeatStopStart(heartbeatStateOff,heartbeatStateOn,_L("Making covering window opaque"),_L("Making covering window transparent"));
		if (aSpriteMode)
			{
			RBlankWindow blankwin2(TheClient->iWs);
			User::LeaveIfError(blankwin2.Construct(*iHbTestWin->BaseWin(),1234));
			blankwin2.Activate();
			heartbeatStateOn=GetHeartBeatTimerState();
			// With ESpriteNoChildClip the heartbeat should still be on as the sprite will still be visible
			// Without it blankwin2 will cover up the sprite and cancel the heartbeat
			TBool heartBeatCheck=aSpriteFlags&ESpriteNoChildClip;
			TEST((!heartbeatStateOn)==(!heartBeatCheck));		//Works with any positive value for True
			blankwin2.Close();
			}
		transWin.SetTransparentRegion(emptyRegion);
		heartbeatStateOff=GetHeartBeatTimerState();
		transWin.Close();
		heartbeatStateOn=GetHeartBeatTimerState();
		TestHeartBeatStopStart(heartbeatStateOff,heartbeatStateOn,_L("Making covering window opaque"),_L("Un-covering window"));
		}
	else
		transWin.Close();

	// Make the window with the animation invisible and check this disables the heartbeat timer.
	iHbTestWin->BaseWin()->SetVisible(EFalse);
	heartbeatStateOff=GetHeartBeatTimerState();
	iHbTestWin->BaseWin()->SetVisible(ETrue);
	heartbeatStateOn=GetHeartBeatTimerState();
	TestHeartBeatStopStart(heartbeatStateOff,heartbeatStateOn,_L("Making window invisible"),_L("Making window visible"));

	// Move the window with the animation off screen and check this disables the heartbeat timer.
	TSize screenSize=TheClient->iGroup->Size();
	TPoint oldPos(iHbTestWin->BaseWin()->Position());
	iHbTestWin->BaseWin()->SetPosition(screenSize.AsPoint());
	heartbeatStateOff=GetHeartBeatTimerState();
	iHbTestWin->BaseWin()->SetPosition(oldPos);
	heartbeatStateOn=GetHeartBeatTimerState();
	TestHeartBeatStopStart(heartbeatStateOff,heartbeatStateOn,_L("Moving window offscreen"),_L("Moving window back onscreen"));
	TEST(GetHeartBeatTimerState());
	CleanupStack::PopAndDestroy(&anim);
	TEST(!GetHeartBeatTimerState());
	CleanupStack::PopAndDestroy(&sprite);
	}

void CTHeartBeatTest::TestAnimDllTransL()
	{
	RTestAnim anim(iAnimDll);
	TSize testWinSize(iHbTestWin->BaseWin()->Size());
	TRect animRect(testWinSize.iWidth/4,testWinSize.iHeight/3,testWinSize.iWidth*3/4,testWinSize.iHeight*2/3);
	TPckgBuf<TRect> rectPckg;
	rectPckg()=animRect;
	TEST(!GetHeartBeatTimerState());
	User::LeaveIfError(anim.Construct(*iHbTestWin->BaseWin(),EAnimTypeTest3,rectPckg));
	CleanupClosePushL(anim);
	TEST(GetHeartBeatTimerState());
//
	RunTransWinTestsL(animRect);
//
	CleanupStack::PopAndDestroy(&anim);
	TEST(!GetHeartBeatTimerState());
	}

void CTHeartBeatTest::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	
	switch(++iTest->iState)
		{
		case 1:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0568
*/
			((CTHeartBeatTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0568"));
			_LIT(KTestTextCursorTimer,"Basic heartbeat timer test");
			iTest->LogSubTest(KTestTextCursorTimer);
			TestTextCursorTimerL();
			break;
		case 2:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0569
*/
			((CTHeartBeatTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0569"));
			_LIT(KTestTextCursorTimerTrans,"Heartbeat timer test with transparent windows");
			iTest->LogSubTest(KTestTextCursorTimerTrans);
			TestTextCursorTimerTransL();
			break;
		case 3:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0570
*/
			((CTHeartBeatTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0570"));
			_LIT(KTestAnimDll1,"Anim DLL Heartbeat check (Win)");
			iTest->LogSubTest(KTestAnimDll1);
			TestAnimDllL(EFalse,0);
			break;
		case 4:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0571
*/
			((CTHeartBeatTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0571"));
			_LIT(KTestAnimDll2,"Anim DLL Heartbeat check (Sprite)");
			iTest->LogSubTest(KTestAnimDll2);
			TestAnimDllL(ETrue,0);
			TestAnimDllL(ETrue,ESpriteNoChildClip);
			break;
		case 5:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0572
*/
			((CTHeartBeatTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0572"));
			_LIT(KTestAnimDllTrans,"Anim DLL Heartbeat check with transparent windows");
			iTest->LogSubTest(KTestAnimDllTrans);
			TestAnimDllTransL();
			break;
		case 6:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0573
*/
			((CTHeartBeatTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0573"));
			_LIT(KTestActiveInactiveEvents,"Handling of TRawEvent::EInactive and EActive");
			iTest->LogSubTest(KTestActiveInactiveEvents);
			TestActiveInactiveEvents();
			break;
		default:
			((CTHeartBeatTestStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTHeartBeatTestStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTHeartBeatTestStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(HeartBeatTest)
