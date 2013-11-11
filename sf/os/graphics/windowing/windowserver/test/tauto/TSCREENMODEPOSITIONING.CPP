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
// Test code for screen mode positioning CR
// GT 0164 Uikon: A3.26. IM 9.
// Test code for the positioning part of Change Request PHAR-5SJGAM
// ("Enable screen mode positioning and scaling").
// Note this feature is also in GT0199 as PREQ673"Screen Positioning".
// Tests screen position being configurable for a screen mode -
// eg it is now possible to set in wsini.ini the position on the
// physical screen where the origin of the screen mode's screen will appear.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TSCREENMODEPOSITIONING.H"
#include <graphics/displaycontrol.h>

//#define LOGGING on		//Uncomment this line to get extra logging useful when there is a tests that fails

LOCAL_D TSize FullScreenModeSize;
LOCAL_D TDisplayMode ScreenDisplayMode;
LOCAL_D TInt Copy2ndHalfOfScreen;


void CBasicWin::Draw()
	{
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->SetBrushColor(TRgb(255,0,0));
	TSize winSize=Size();
	iGc->DrawRect(TRect(winSize));
	iGc->SetBrushColor(TRgb(0,221,221));
	iGc->DrawEllipse(TRect(TPoint(winSize.iWidth/2,winSize.iHeight/2),winSize));
	iGc->DrawEllipse(TRect(TPoint(-winSize.iWidth/2,-winSize.iHeight/2),winSize));
	iDrawn=ETrue;
	}

void CSpriteWin::UpdateState(TInt aState)
	{
	SetState(aState);
	DrawNow();
	}

TPoint CheckSpritePos(233,60);
TPoint CheckSpritePos2(18,60);

void CSpriteWin::Draw()
	{
	switch(iState)
		{
		case 0:
			iGc->BitBlt(CheckSpritePos,&iSpriteBitmap);
			break;
		case 1:
			iGc->BitBlt(CheckSpritePos+TPoint(30,30),&iSpriteBitmap);
			iGc->BitBlt(CheckSpritePos,&iSpriteBitmap);
			break;
		case 2:
			iGc->BitBlt(CheckSpritePos2+TPoint(-10,40),&iSpriteBitmap);
			iGc->BitBlt(CheckSpritePos2+TPoint(-10,20),&iSpriteBitmap);
			break;
		case 3:
			iGc->BitBlt(TPoint(),&iSpriteBitmap);
			break;
		default:
			break;
		}
	}

CSpriteWin::~CSpriteWin()
	{
	}

void CBmpWin::Draw()
	{
	iGc->BitBlt(TPoint(),&iScreenBmp);
	}

CConnection2::~CConnection2()
	{
	delete iScreenBitmap;
	delete iBitmapWin;
	delete iSpriteWin;
	iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);
	delete iGroup;
	delete iClient;
	}

void CConnection2::ConstructL(CTestBase *aTest,CFbsBitmap& aBitmap)
	{
	iClient=new(ELeave) CTClient;
	iClient->SetScreenNumber(aTest->ScreenNumber());
	iClient->ConstructL();
	iGroup=new(ELeave) CConnection2Group(iClient,this,aTest);
	iGroup->ConstructL();
	iGroup->GroupWin()->EnableScreenChangeEvents();
	iClient->Flush();
	iSpriteWin=new(ELeave) CSpriteWin(aBitmap);
	iSpriteWin->ConstructExtLD(*iGroup,TPoint(),TSize(433,240));
	User::LeaveIfError(iSpriteWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	iSpriteWin->AssignGC(*iClient->iGc);
	iSpriteWin->Activate();
	iSpriteWin->SetVisible(EFalse);
	iSpriteWin->BaseWin()->SetShadowDisabled(ETrue);
	iSpriteWin->BaseWin()->SetShadowHeight(0);
	iScreenBitmap=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iScreenBitmap->Create(TSize(FullScreenModeSize.iWidth/2,FullScreenModeSize.iHeight), ScreenDisplayMode));
	iBitmapWin=new(ELeave) CBmpWin(*iScreenBitmap);
	iBitmapWin->ConstructExtLD(*iGroup,TPoint(FullScreenModeSize.iWidth/2,0),iScreenBitmap->SizeInPixels());
	User::LeaveIfError(iBitmapWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	iBitmapWin->AssignGC(*iClient->iGc);
	iBitmapWin->Activate();
	iBitmapWin->SetVisible(EFalse);
	iBitmapWin->BaseWin()->SetShadowDisabled(ETrue);
	iBitmapWin->BaseWin()->SetShadowHeight(0);
	}

void CConnection2::DrawBitmapWin()
	{
	TPoint pos=(Copy2ndHalfOfScreen? TPoint() : TPoint(FullScreenModeSize.iWidth/2,0));
	iBitmapWin->SetPos(pos);
	iBitmapWin->SetVisible(ETrue);
	iBitmapWin->DrawNow();
	iClient->Flush();
	}

CConnection2Group::CConnection2Group(CTClient *aClient, CConnection2 *aSecondConnection, CTestBase *aTest) : CTWindowGroup(aClient), iTest(aTest), iConnection2(aSecondConnection)
	{}




//CTScreenModePositioning
CTScreenModePositioning::CTScreenModePositioning(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}


CTScreenModePositioning::~CTScreenModePositioning()
	{
	delete iTestWin;
	delete iConnection2;
	delete iBlankWin;
	delete iBackedUpWin;
	delete iTestChildWin;
	}

void CTScreenModePositioning::ConstructL()
	{
	User::LeaveIfError(iSpriteBitmap.Load(TEST_BITMAP_NAME,0));
	iScalingSupported=CheckScalingSupportedOrNot();
	TheClient->iScreen->SetScreenMode(TheClient->iScreenModes[0]);
	FullScreenModeSize=TheClient->iScreen->SizeInPixels();
	ScreenDisplayMode=TheClient->iScreen->DisplayMode();
	iConnection2=new(ELeave) CConnection2;
	iConnection2->ConstructL(iTest,iSpriteBitmap);
	iBlankWin=new(ELeave) CTBlankWindow();
	iBlankWin->ConstructL(*TheClient->iGroup);
	User::LeaveIfError(iBlankWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	iBlankWin->SetExt(TPoint(),FullScreenModeSize);
	iBlankWin->SetVisible(EFalse);
	iBlankWin->Activate();
	iTestWin=new(ELeave) CBasicWin;
	iTestWin->ConstructExtLD(*TheClient->iGroup,TPoint(),FullScreenModeSize);
	User::LeaveIfError(iTestWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	iTestWin->AssignGC(*TheClient->iGc);
	iTestWin->BaseWin()->SetShadowDisabled(ETrue);
	iTestWin->BaseWin()->SetShadowHeight(0);
	iTestWin->SetVisible(EFalse);
	iTestWin->Activate();
	iBackedUpWin=new(ELeave) CTBackedUpWin(EColor256);
	iBackedUpWin->ConstructExtLD(*iTestWin,TPoint(),FullScreenModeSize);
	iBackedUpWin->SetVisible(EFalse);
	iBackedUpWin->Activate();
	iTestChildWin=new(ELeave) CTBlankWindow();
	iTestChildWin->ConstructL(*iTestWin);
	User::LeaveIfError(iTestChildWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	iTestChildWin->BaseWin()->SetShadowDisabled(ETrue);
	iTestChildWin->SetColor(KRgbGreen);
	iTestChildWin->BaseWin()->SetVisible(EFalse);
	iTestChildWin->Activate();
	//The Cursor appearing on screen will affect the result of RectCompare function. 
	//Set it to the position out of the screen
	TheClient->iWs.SetPointerCursorPosition(TPoint(-1,-1));
	}

void CTScreenModePositioning::ChangeScreenMode(CTClient* aClient,TPixelsAndRotation aPixelsAndRotation,TInt aMode)
	{
	ChangeScreenMode(aClient,aPixelsAndRotation,ESizeEnforcementNone,aMode);
	}

void CTScreenModePositioning::ChangeScreenMode(CTClient* aClient,TPixelsAndRotation aPixelsAndRotation,TScreenModeEnforcement aScreenModeEnforcement,TInt aMode)
	{
	aClient->iScreen->SetScreenSizeAndRotation(aPixelsAndRotation);
	aClient->iScreen->SetScreenModeEnforcement(aScreenModeEnforcement);
	aClient->iScreen->SetScreenMode(aMode);
	}

void CTScreenModePositioning::SetScreenMode(CTClient* aClient,TInt aMode,TScreenModeEnforcement aScreenModeEnforcement)
	{
	TPixelsAndRotation pixelsAndRotation;
	aClient->iScreen->SetScreenModeEnforcement(aScreenModeEnforcement);
	aClient->iScreen->SetScreenMode(aMode);
	aClient->iScreen->GetDefaultScreenSizeAndRotation(pixelsAndRotation);
	aClient->iScreen->SetScreenSizeAndRotation(pixelsAndRotation);
	aClient->Flush();
	}

void CTScreenModePositioning::SetUpSpriteLC(RWsSprite &aSprite, RWsSession &aSession, RWindowTreeNode &aWindow,TInt aFlags)
	{
	aSprite=RWsSprite(aSession);
	User::LeaveIfError(aSprite.Construct(aWindow,TPoint(),aFlags));
	CleanupClosePushL(aSprite);
	TSpriteMember member;
	iTest->SetUpMember(member);
	member.iBitmap=&iSpriteBitmap;
	User::LeaveIfError(aSprite.AppendMember(member));
	User::LeaveIfError(aSprite.Activate());
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0098

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Window Tests. Please see test cases GRAPHICS-WSERV-(0099-0103) which are a subset of this test case.
					REQUIREMENT: CR PHAR-5SJGAM, PREQ673

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Tests on windows in different screen modes each with differing screen mode origin's defined in the
					wsini file. This test case only tests screen modes with 0 orientation.
					Goes through each screen mode defined in the wsini file and exercises test cases GRAPHICS-WSERV-(0099-0103) for each mode.
					For more details see test cases GRAPHICS-WSERV-(0099-0103) which are a subset of this test case.

@SYMTestExpectedResults

*/
void CTScreenModePositioning::WindowTestsL()
	{
	TheClient->iGroup->GroupWin()->EnableScreenChangeEvents();
	TInt numOfModes=TheClient->iScreenModes.Count();
	TInt ii;
	for (ii=0; ii<numOfModes; ++ii)
		{
		iCurrentMode=TheClient->iScreenModes[ii];
		TPixelsAndRotation pixelsAndRotation;
		iCurrentScreenModeOrigin=TheClient->iScreen->GetScreenModeScaledOrigin(iCurrentMode);
		iCurrentScreenModeScale=TheClient->iScreen->GetScreenModeScale(iCurrentMode);
		TheClient->iScreen->GetScreenModeSizeAndRotation(iCurrentMode,pixelsAndRotation);
		iCurrentScreenModeSize=pixelsAndRotation.iPixelSize;
		if (iCurrentScreenModeOrigin.iX<=FullScreenModeSize.iWidth/2-10 /*&& iCurrentScreenModeOrigin!=TPoint() */&& pixelsAndRotation.iRotation==CFbsBitGc::EGraphicsOrientationNormal)
			{
		#if defined(LOGGING)
			_LIT(KLog,"Test mode %d");
			LOG_MESSAGE2(KLog,ii);
		#endif
			TRAPD(ret,DoWindowTestsL());
			if (ret==KErrNone)
				continue;
			RestoreScreenMode();
			if (ret<KErrNone)
				TEST(EFalse);
			else
				User::Leave(ret);
			}
		}
	}

void CTScreenModePositioning::DoWindowTestsL()
	{
	// Make this visible so that the display mode remains EColor256
	iBlankWin->SetVisible(ETrue);
	Copy2ndHalfOfScreen=(iCurrentScreenModeOrigin.iX>FullScreenModeSize.iWidth/2? 1 : 0);
	TInt testWinWidth=Max(FullScreenModeSize.iWidth/2-iCurrentScreenModeOrigin.iX,iCurrentScreenModeOrigin.iX-FullScreenModeSize.iWidth/2);
	iTestWinSize=TSize(testWinWidth,FullScreenModeSize.iHeight-iCurrentScreenModeOrigin.iY-60);
	iTestWinSize.iWidth/=(iCurrentScreenModeScale.iWidth > 1 ? iCurrentScreenModeScale.iWidth*2 : iCurrentScreenModeScale.iWidth);
	iTestWinSize.iHeight/=(iCurrentScreenModeScale.iHeight > 1 ? iCurrentScreenModeScale.iHeight*2 : iCurrentScreenModeScale.iHeight);
#if defined(LOGGING)
	_LIT(KLog1,"  PosTest1");
	LOG_MESSAGE(KLog1);
#endif
	PositionTest1L(TPoint());
#if defined(LOGGING)
	_LIT(KLog2,"  PosTest2");
	LOG_MESSAGE(KLog2);
#endif
	PositionTest1L(TPoint(25,35));
#if defined(LOGGING)
	_LIT(KLog3,"  PosTest3");
	LOG_MESSAGE(KLog3);
#endif
	PositionTest2L(TPoint(10,10));
#if defined(LOGGING)
	_LIT(KLog4,"  GetInvalidRegion");
	LOG_MESSAGE(KLog4);
#endif
	GetInvalidRegionTestL(TPoint(10,10));
#if defined(LOGGING)
	_LIT(KLog5,"  CreateWin1");
	LOG_MESSAGE(KLog5);
#endif
	CreateWindowsTestL(TPoint());
#if defined(LOGGING)
	_LIT(KLog6,"  CreateWin2");
	LOG_MESSAGE(KLog6);
#endif
	CreateWindowsTestL(TPoint(33,15));
#if defined(LOGGING)
	_LIT(KLog7,"  NextMode");
	LOG_MESSAGE(KLog7);
#endif
	NextScreenModeTestL(TPoint(12,6));
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0099

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Position Test 1
					REQUIREMENT: CR PHAR-5SJGAM, PREQ673
					API: RWindowBase::InquireOffset(), RWindowBase::AbsPosition(),RWindowBase::Position()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Sets the new screen mode on the main client's screen device. Checks that an window whose group window
					has this screen device is in the correct position and for the origin of the new screen mode.
					(Does this by copying the window to a bitmap, switching back to screen mode 0 and then setting the position
					of the window to be the expected position. Then the bitmap is blitted to the other half of the screen and the
					2 halves of the screen compared)

@SYMTestExpectedResults Checks the window is positioned correctly for the origin of the new screen mode.

*/
void CTScreenModePositioning::PositionTest1L(TPoint aPos)
	{
    ((CTScreenModePositioningStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0099"));    
	SetScreenMode(TheClient,iCurrentMode,ESizeEnforcementPixelsAndRotation);
	iTestWin->SetExt(aPos,iTestWinSize);
	iTestWin->SetVisible(ETrue);
	iTestWin->Invalidate();
	TheClient->Flush();
	TestTopClientWindowPositionAPIs(aPos,iTestWin->BaseWin());
	TheClient->WaitForRedrawsToFinish();
	CopyAndCompareL(aPos);
	((CTScreenModePositioningStep*)iStep)->RecordTestResultL();
	}

template <TBool newstate>
class TCleanupSetVisible:public TCleanupItem
	{
	static void Cleanup(void*v)
		{
			((CTWinBase*)v)[0].SetVisible(newstate);
		}
	public:
	TCleanupSetVisible(CTWinBase* v): 
		TCleanupItem(Cleanup,v)	
		{}
	};


/**
@SYMTestCaseID		GRAPHICS-WSERV-0100

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Position Test 2
					REQUIREMENT: CR PHAR-5SJGAM, PREQ673
					API: RWindowBase::SetPosition(),RWindowBase::InquireOffset(), RWindowBase::AbsPosition(),RWindowBase::Position()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     As in test case GRAPHICS-WSERV-0099 but also tests moving and resizing the window and also does these tests on a backed up child window.

@SYMTestExpectedResults Checks both the windows are positioned correctly for the origin of the new screen mode.

*/
void CTScreenModePositioning::PositionTest2L(TPoint aPos)
	{
	((CTScreenModePositioningStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0100"));
	SetScreenMode(TheClient,iCurrentMode,ESizeEnforcementPixelsAndRotation);
	iTestWin->SetPos(aPos);

	CleanupStack::PushL(TCleanupSetVisible<EFalse>(iBackedUpWin));
	iTestWin->SetVisible(ETrue);
	TestTopClientWindowPositionAPIs(aPos,iTestWin->BaseWin());
	TPoint backedUpWinPt=TPoint(iTestWinSize.iWidth/3,iTestWinSize.iHeight/4);
	iBackedUpWin->SetExtL(backedUpWinPt,TSize(iTestWinSize.iWidth/6,iTestWinSize.iHeight/6));
	iBackedUpWin->SetVisible(ETrue);
	TestChildWindowPositionAPIs(backedUpWinPt,aPos,iBackedUpWin->BaseWin(),iTestWin->BaseWin());
	aPos+=TPoint(20,20);
	iTestWin->SetPos(aPos);
	TestTopClientWindowPositionAPIs(aPos,iTestWin->BaseWin());
	TestChildWindowPositionAPIs(backedUpWinPt,aPos,iBackedUpWin->BaseWin(),iTestWin->BaseWin());
	iTestWin->Invalidate();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TestTopClientWindowPositionAPIs(aPos,iTestWin->BaseWin());
	TestChildWindowPositionAPIs(backedUpWinPt,aPos,iBackedUpWin->BaseWin(),iTestWin->BaseWin());
	CopyAndCompareL(aPos);
	CleanupStack::PopAndDestroy(iBackedUpWin);	//TCleanupSetVisible
	((CTScreenModePositioningStep*)iStep)->RecordTestResultL();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0101

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Invalid Region Test
					REQUIREMENT: CR PHAR-5SJGAM, PREQ673
					API: RWindow::GetInvalidRegion()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Sets the new screen mode, then invalidates different parts of the test window. Checks that the above API gets
					the correct invalid area from the server. Also tests moving the invalid area.

@SYMTestExpectedResults Checks the invalid region is correct for the origin of the new screen mode.

*/
void CTScreenModePositioning::GetInvalidRegionTestL(TPoint aPos)
	{
	((CTScreenModePositioningStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0101"));
	SetScreenMode(TheClient,iCurrentMode,ESizeEnforcementPixelsAndRotation);
	iTestWin->SetExt(aPos,iTestWinSize);
	iTestWin->SetVisible(ETrue);
	iTestChildWin->SetExt(TPoint(iTestWinSize.iWidth>>2,iTestWinSize.iHeight>>2),TSize(iTestWinSize.iWidth>>1,iTestWinSize.iHeight>>1));
	iTestChildWin->SetVisible(ETrue);
	iTestWin->Win()->BeginRedraw();
	iTestWin->Win()->EndRedraw();
	const TInt KNumRects=3;
	TRect rects[KNumRects];
	rects[0]=TRect(1,1,5,2);
	rects[1]=TRect(TPoint(iTest->StdTestWindowSize().iWidth>>1,iTest->StdTestWindowSize().iHeight>>1),iTest->StdTestWindowSize());
	rects[2]=TRect(2,0,4,5);
	RRegion region;
	CleanupClosePushL(region);
	for (TInt index=0;index<KNumRects;++index)
		{
		iTestWin->Invalidate(rects[index]);
		region.AddRect(rects[index]);
		}
	//PeterI original wserv assumes a region hidden beneath a child is not invalid
	//Mk3 doesn't make this assumption 
	//TRect subRect;
	//subRect.iTl=iTestChildWin->BaseWin()->InquireOffset(*iTestWin->BaseWin());
	//subRect.SetSize(iTestChildWin->Size());
	//region.Tidy();
	//region.SubRect(subRect);
	//region.ClipRect(TRect(iTestWin->Size()));
	RRegion invalidRegion;
	CleanupClosePushL(invalidRegion);
	iTestWin->Win()->GetInvalidRegion(invalidRegion);
	CompareRegionsL(region,invalidRegion);
	CleanupStack::PopAndDestroy(2,&region);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	iTestChildWin->SetVisible(EFalse);
	TRect rect1(TPoint(0,0),iTestWinSize);
	TestGetInvalidRegionL(rect1);
	TInt width=iTestWinSize.iWidth;
	TInt height=iTestWinSize.iHeight;
	TRect rect2(TPoint(width/6,height/6),TSize(width/3,height/3));
	TestGetInvalidRegionL(rect2);
	iTestWin->Invalidate();
	iTestWin->SetPos(TPoint(15,15));
	iTestWin->SetPos(TPoint());
	iTestWin->SetPos(TPoint(-15,-15));
	iTestWin->SetPos(aPos);
	RRegion invalid;
	RRegion testRegion(rect1);
	CleanupClosePushL(invalid);
	CleanupClosePushL(testRegion);
	iTestWin->Win()->GetInvalidRegion(invalid);
	TBool err=invalid.CheckError();
	TEST(!err);
	if (err)
		{
		_LIT(KLog,"Returned Invalid Region has an error");
		LOG_MESSAGE(KLog);
		}

	TEST(invalid.BoundingRect().iBr.iX<=iTestWinSize.iWidth);
	if (invalid.BoundingRect().iBr.iX>iTestWinSize.iWidth)
		{
		_LIT(KLog,"Invalid Region extends beyond right edge of window");
		LOG_MESSAGE(KLog);
		}

	CompareRegionsL(testRegion,invalid);
	CleanupStack::PopAndDestroy(2,&invalid);
	iTestWin->DrawNow();
	TheClient->Flush();
	CopyAndCompareL(aPos);
	((CTScreenModePositioningStep*)iStep)->RecordTestResultL();
	}

void CTScreenModePositioning::CopyAndCompareL(TPoint aPos)
	{
	CopyScreenAndChangeBackToDefScrModeL();
	iTestWin->SetPos(iCurrentScreenModeOrigin+aPos);
	iConnection2->DrawBitmapWin();
	TheClient->WaitForRedrawsToFinish();
	TestRect();
	iConnection2->BitmapWin()->SetVisible(EFalse);
	iTestWin->SetVisible(EFalse);
	}

void CTScreenModePositioning::TestGetInvalidRegionL(TRect& aRect)
	{
	RRegion testRegion;
	RRegion invalid;
	iTestWin->Invalidate(aRect);
	testRegion.AddRect(aRect);
	iTestWin->Win()->GetInvalidRegion(invalid);
	CompareRegionsL(testRegion,invalid);
	invalid.Close();
	testRegion.Close();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	}

void CTScreenModePositioning::CompareRegionsL(const TRegion &aRegion1,const TRegion &aRegion2)
	{
	_LIT(KLog,"Regions do not contain same area.");
	RRegion tmp;
	tmp.Copy(aRegion1);
	tmp.SubRegion(aRegion2);
	if (tmp.CheckError())
		User::Leave(KErrNoMemory);
	TBool retVal1=tmp.IsEmpty();
	TEST(retVal1);
	if (!retVal1)
		LOG_MESSAGE(KLog);

	tmp.Copy(aRegion2);
	tmp.SubRegion(aRegion1);
	if (tmp.CheckError())
		User::Leave(KErrNoMemory);
	TBool retVal2=tmp.IsEmpty();
	TEST(retVal2);
	if (!retVal2 && retVal1)
		LOG_MESSAGE(KLog);

	tmp.Close();
	}

void CTScreenModePositioning::TestTopClientWindowPositionAPIs(TPoint aPos,RWindowBase* aWin)
	{
	TEST(aWin->AbsPosition()==aPos);
	TEST(aWin->Position()==aPos);
	TEST(aWin->InquireOffset(*TheClient->iGroup->GroupWin())==aPos);
	}

void CTScreenModePositioning::TestChildWindowPositionAPIs(TPoint aPos,TPoint aParentPos,RWindowBase* aWin,RWindowBase* aParentWin)
	{
	TEST(aWin->AbsPosition()==aParentPos+aPos);
	TEST(aWin->Position()==aPos);
	TEST(aWin->InquireOffset(*TheClient->iGroup->GroupWin())==aParentPos+aPos);
	TEST(aWin->InquireOffset(*aParentWin)==aPos);
	}

void CTScreenModePositioning::RestoreScreenMode()
	{
	TPixelsAndRotation pixelsAndRotation;
	pixelsAndRotation.iPixelSize=FullScreenModeSize;
	TheClient->iScreen->GetDefaultScreenSizeAndRotation(pixelsAndRotation);
	ChangeScreenMode(TheClient,pixelsAndRotation,ESizeEnforcementNone,TheClient->iScreenModes[0]);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0102

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Create Windows Test
					REQUIREMENT: CR PHAR-5SJGAM, PREQ673
					API: RWindowBase::SetPosition(),RWindowBase::InquireOffset(), RWindowBase::AbsPosition(),RWindowBase::Position()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     As in test case GRAPHICS-WSERV-0100 but tests windows that are created in the new screen mode. Also tests moving and resizing these windows

@SYMTestExpectedResults Checks both the windows are positioned correctly for the origin of the new screen mode.

*/
void CTScreenModePositioning::CreateWindowsTestL(TPoint aPos)
	{
	((CTScreenModePositioningStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0102"));
	SetScreenMode(TheClient,iCurrentMode,ESizeEnforcementPixelsAndRotation);
	CBasicWin* basicWin=new(ELeave) CBasicWin;
	basicWin->ConstructExtLD(*TheClient->iGroup,aPos,iTestWinSize);
	CleanupStack::PushL(basicWin);
	TInt mode=basicWin->BaseWin()->SetRequiredDisplayMode(EColor256);
	basicWin->AssignGC(*TheClient->iGc);
	basicWin->BaseWin()->SetShadowDisabled(ETrue);
	basicWin->BaseWin()->SetShadowHeight(0);
	basicWin->Activate();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TestTopClientWindowPositionAPIs(aPos,basicWin->BaseWin());
	CTBlankWindow* blankChildWin=new(ELeave) CTBlankWindow ;
	CleanupStack::PushL(blankChildWin);
	blankChildWin->ConstructL(*basicWin);
	blankChildWin->BaseWin()->SetRequiredDisplayMode(EColor256);
	blankChildWin->BaseWin()->SetShadowDisabled(ETrue);
	blankChildWin->SetColor(KRgbYellow);
	TPoint childPos(iTestWinSize.iWidth/4,iTestWinSize.iHeight/4);
	blankChildWin->SetExtL(childPos,TSize(iTestWinSize.iWidth/2,iTestWinSize.iHeight/2));
	blankChildWin->Activate();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TestChildWindowPositionAPIs(childPos,aPos,blankChildWin->BaseWin(),basicWin->BaseWin());
	blankChildWin->SetPos(TPoint(-15,-20));
	TheClient->Flush();
	basicWin->DrawNow();
	TestChildWindowPositionAPIs(TPoint(-15,-20),aPos,blankChildWin->BaseWin(),basicWin->BaseWin());
	CopyScreenAndChangeBackToDefScrModeL();
	basicWin->SetPos(iCurrentScreenModeOrigin+aPos);
	iConnection2->DrawBitmapWin();
	TheClient->WaitForRedrawsToFinish();
	TestRect();
	iConnection2->BitmapWin()->SetVisible(EFalse);
	iTestWin->SetVisible(EFalse);
	CleanupStack::PopAndDestroy(blankChildWin);
	CleanupStack::PopAndDestroy(basicWin);
	((CTScreenModePositioningStep*)iStep)->RecordTestResultL();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0103

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Changing to next screen mode
					REQUIREMENT: CR PHAR-5SJGAM, PREQ673
					API: RWindowBase::SetPosition(),RWindowBase::InquireOffset(), RWindowBase::AbsPosition(),RWindowBase::Position()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Sets the new screen mode, creates a new child window and then changes to the next screen mode with non-zero origin,
					checks windows are in the expected position for the origin of the new screen mode.

@SYMTestExpectedResults Checks both the windows are positioned correctly for the origin of the new screen mode.

*/
void CTScreenModePositioning::NextScreenModeTestL(TPoint aPos)
	{
	((CTScreenModePositioningStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0103"));
	if (iCurrentScreenModeOrigin==TPoint())
		{
		return;
		}
	TInt numOfModes=TheClient->iScreenModes.Count();
	TInt defaultMode=TheClient->iScreenModes[0];
	TInt lastMode=TheClient->iScreenModes[numOfModes-1];
	TInt mode=(iCurrentMode<lastMode? iCurrentMode:defaultMode);
	// find current mode index
	TInt ii;
	TInt modeIdx=0;
	for (ii=0; ii<numOfModes; ++ii)
		{
		if (mode==TheClient->iScreenModes[ii])
			{
			modeIdx=ii;
			break;
			}
		}
	TPoint screenModeOrigin(0,0);
	TPixelsAndRotation pixelsAndRotation;
	while (screenModeOrigin==TPoint()||(pixelsAndRotation.iRotation!=CFbsBitGc::EGraphicsOrientationNormal))
		{
		mode=(mode==lastMode? defaultMode : TheClient->iScreenModes[++modeIdx]);
		if (mode==iCurrentMode)
			{
			return;
			}
		else if (mode==lastMode)
			{
			modeIdx=0;
			}		
		screenModeOrigin=TheClient->iScreen->GetScreenModeOrigin(mode);
		TheClient->iScreen->GetScreenModeSizeAndRotation(mode,pixelsAndRotation);
		}
	SetScreenMode(TheClient,iCurrentMode,ESizeEnforcementPixelsAndRotation);
	iTestWin->SetExt(aPos,iTestWinSize);
	iTestWin->SetVisible(ETrue);
	TheClient->Flush();
	CBasicWin* basicWin=new(ELeave) CBasicWin;
	CleanupStack::PushL(basicWin);
	basicWin->ConstructExtLD(*iTestWin,TPoint(),TSize(iTestWinSize.iWidth/5,iTestWinSize.iHeight/5));
	User::LeaveIfError(basicWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	basicWin->AssignGC(*TheClient->iGc);
	basicWin->BaseWin()->SetShadowDisabled(ETrue);
	basicWin->BaseWin()->SetShadowHeight(0);
	basicWin->Activate();
	TPoint pos(iTestWinSize.iWidth/3,iTestWinSize.iWidth/4);
	basicWin->SetPos(pos);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TestChildWindowPositionAPIs(pos,aPos,basicWin->BaseWin(),iTestWin->BaseWin());
	iCurrentScreenModeScale=TheClient->iScreen->GetScreenModeScale(mode);
	iCurrentScreenModeOrigin=TheClient->iScreen->GetScreenModeOrigin(mode);
	iTestWinSize.iWidth/=(iCurrentScreenModeScale.iWidth > 1 ? iCurrentScreenModeScale.iWidth*2 : iCurrentScreenModeScale.iWidth);
	iTestWinSize.iHeight/=(iCurrentScreenModeScale.iHeight > 1 ?  iCurrentScreenModeScale.iHeight*2 : iCurrentScreenModeScale.iHeight);
	iTestWin->SetExt(aPos,iTestWinSize);
	SetScreenMode(TheClient,mode,ESizeEnforcementPixelsAndRotation);
	CopyScreenAndChangeBackToDefScrModeL();
	iTestWin->SetPos(aPos+iCurrentScreenModeOrigin);
	basicWin->SetPos(pos);
	iConnection2->DrawBitmapWin();
	TestRect();
	iConnection2->BitmapWin()->SetVisible(EFalse);
	iTestWin->SetVisible(EFalse);
	CleanupStack::PopAndDestroy(basicWin);
	iCurrentScreenModeOrigin=TheClient->iScreen->GetScreenModeOrigin(iCurrentMode);
	((CTScreenModePositioningStep*)iStep)->RecordTestResultL();
	}

void CTScreenModePositioning::CopyScreenAndChangeBackToDefScrModeL()
	{
	// clear the content of bitmap before it is used for copying
	ClearBitmap(iConnection2->iScreenBitmap);

	// Copy the current content of the screen before moving to screen sizemode 0.
	if (!iScalingSupported)
		{
		TPoint pt=(Copy2ndHalfOfScreen? TPoint(FullScreenModeSize.iWidth/2,0) : TPoint());
		User::LeaveIfError(iConnection2->iClient->iScreen->CopyScreenToBitmap(iConnection2->iScreenBitmap,TRect(pt,TSize(FullScreenModeSize.iWidth/2,FullScreenModeSize.iHeight))));
		}
	else
		{
		TPoint pt=(Copy2ndHalfOfScreen? TPoint((iCurrentScreenModeSize.iWidth+iCurrentScreenModeOrigin.iX)/2,0) : -iCurrentScreenModeOrigin);
		TPoint ptBottom(FullScreenModeSize.iWidth/2,FullScreenModeSize.iHeight);
		ptBottom-=iCurrentScreenModeOrigin;
		ptBottom.iX=(ptBottom.iX>=0 ? ptBottom.iX/iCurrentScreenModeScale.iWidth : ((-ptBottom.iX)*iCurrentScreenModeScale.iWidth+ptBottom.iX)/iCurrentScreenModeScale.iWidth+ptBottom.iX);
		ptBottom.iY=(ptBottom.iY>=0 ? ptBottom.iY/iCurrentScreenModeScale.iHeight : ((-ptBottom.iY)*iCurrentScreenModeScale.iHeight+ptBottom.iY)/iCurrentScreenModeScale.iHeight+ptBottom.iY);
		User::LeaveIfError(iConnection2->iClient->iScreen->CopyScreenToBitmap(iConnection2->iScreenBitmap,TRect(pt,ptBottom)));
		}
	TheClient->Flush();
	SetScreenMode(TheClient,TheClient->iScreenModes[0],ESizeEnforcementPixelsAndRotation);
	TPixelsAndRotation dummySize;
	dummySize.iPixelSize=TSize(0,0);
	iConnection2->iClient->iScreen->SetScreenSizeAndRotation(dummySize); //to force an update in the server when we do the next line
	TPixelsAndRotation pixelsAndRotation;
	TheClient->iScreen->GetDefaultScreenSizeAndRotation(pixelsAndRotation);
	iConnection2->iClient->iScreen->SetScreenSizeAndRotation(pixelsAndRotation);
	iConnection2->iClient->iScreen->SetScreenModeEnforcement(ESizeEnforcementPixelsAndRotation);
	iConnection2->iClient->iScreen->SetScreenMode(TheClient->iScreenModes[0]);
	}

TBool CTScreenModePositioning::TestRect()
	{
	TSize size(FullScreenModeSize.iWidth/2,FullScreenModeSize.iHeight);
	TRect right(TPoint(FullScreenModeSize.iWidth/2,0),size);
	TBool retVal=TheClient->iScreen->RectCompare(TRect(size),right);
//	User::After(2000000);
	//Let the pixels cool down for a bit
	User::After(10);
	TEST(retVal);
	if(!retVal)
		{
		_LIT(KLog,"Left and Right halves of display don't match. RightHalf=(%d,%d,%d,%d)");
		LOG_MESSAGE5(KLog,right.iTl.iX,right.iTl.iY,right.iBr.iX,right.iBr.iY);
		}
	return retVal;
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0104

@SYMDEF  			DEF081259, DEF111847

@SYMTestCaseDesc    Sprite Tests
					REQUIREMENT: CR PHAR-5SJGAM, PREQ673
					API: RWsSprite::SetPosition()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test sprites associated with group windows in screen modes with non-zero origin:
					Sets such a new screen mode and creates 2 sprites.
					compares these sprites with	some blitted onto the screen in the expected position.
					Sets the rotation of screen mode to be 180 deg, sets the position of the sprites
					and does the same as above to compare expected position.
					Then changes back to screen mode 0 and sets a new positions of the sprites,changes to the new
					screen mode and checks the sprites have been moved to the correct position.
					Additionally, as part of defect fix DEF111847, this also loops through all display modes above and including Color256, 
					and if possible tests each with the above described method. Previously, it use to only test Color256.
					Display modes lower than EColor256 are not tested as they are not supported for origin change.

@SYMTestExpectedResults Checks the sprites are positioned correctly according to the origin
					of the new screen mode.

*/
void CTScreenModePositioning::SpriteTestL()
	{

#if defined(__WINS__)
	TBool retVal;
	if (iScalingSupported)
		{
		TDisplayMode curDispMode; // Holds the current display mode being tested 
		
		for(curDispMode = EColor256; curDispMode < EColorLast; curDispMode = TDisplayMode(curDispMode+1))
			{
			if (curDispMode == ERgb)
				{
				continue;
				}
			// Set screen mode to 3
			iCurrentMode=3;
			SetScreenMode(TheClient,iCurrentMode,ESizeEnforcementPixelsAndRotation);
			TPoint topLeft(-TheClient->iScreen->GetScreenModeOrigin(iCurrentMode));
			TPoint botRight(TheClient->iScreen->SizeInPixels().AsPoint());

			// Create a sprite at position (0,0) from current origin
			RWsSprite sprite;
			TSize spriteSize=iSpriteBitmap.SizeInPixels();
			SetUpSpriteLC(sprite,TheClient->iWs,*iBlankWin->BaseWin());
			TPoint winPos;
			if (botRight.iX<spriteSize.iWidth)
				{
				winPos.iX=botRight.iX-spriteSize.iWidth;
				iBlankWin->BaseWin()->SetPosition(winPos);
				}
			
			// Now create a spritewin at top left of the visible screen
			CSpriteWin* spriteWin=new(ELeave) CSpriteWin(iSpriteBitmap);
			CleanupStack::PushL(spriteWin);
			spriteWin->ConstructExtLD(*TheClient->iGroup,topLeft,spriteSize);
			
			// Set the display mode of the base window
			TInt setBlankWinDispMode = iBlankWin->BaseWin()->SetRequiredDisplayMode(curDispMode);
			// Set the display mode of the sprite window
			TInt setSpriteWinDispMode = spriteWin->BaseWin()->SetRequiredDisplayMode(curDispMode);
			
			//Only do the tests if the requested mode was actually set on both windows
			if(curDispMode == setBlankWinDispMode && curDispMode == setSpriteWinDispMode)
				{
				//Create and show DisplayMode details message
				_LIT(KModeDetails, "Display Mode: ");
				TBuf<30> modeDetailsMessage(KModeDetails);
				modeDetailsMessage.Append(DisplayModeAsString(curDispMode));
				LOG_MESSAGE(modeDetailsMessage);
			
				spriteWin->AssignGC(*TheClient->iGc);
				spriteWin->SetState(3);
				spriteWin->Activate();
				spriteWin->DrawNow();
				TheClient->WaitForRedrawsToFinish();
				retVal=TheClient->iScreen->RectCompare(TRect(topLeft,spriteSize),TRect(winPos,spriteSize),CWsScreenDevice::EIncludeSprite);
				TEST(retVal);
				if(!retVal)
					{
					_LIT(KLog,"Initial Drawing of sprite fails - does not compare to same bitmap drawn in window.");
					LOG_MESSAGE(KLog);
					}

				// Move position of the window and sprite and then test
				TPoint spritePos(Min(20,botRight.iX-spriteSize.iWidth-winPos.iX),20);
				spriteWin->SetExt(topLeft+spritePos,spriteSize);
				sprite.SetPosition(spritePos);
				spriteWin->DrawNow();
				TheClient->WaitForRedrawsToFinish();
				retVal=TheClient->iScreen->RectCompare(TRect(topLeft+spritePos,spriteSize),TRect(winPos+spritePos,spriteSize),CWsScreenDevice::EIncludeSprite);
				TEST(retVal);
				if(!retVal)
					{
					_LIT(KLog,"Second position of sprite fails - does not compare to same bitmap drawn in window.");
					LOG_MESSAGE(KLog);
					}
				iBlankWin->BaseWin()->SetPosition(TPoint());

				// Now test the same by moving into other screen mode
				iCurrentMode=4;
				SetScreenMode(TheClient,iCurrentMode,ESizeEnforcementNone);
				spritePos.SetXY(0,160);
				sprite.SetPosition(spritePos);
				spriteWin->SetExt(TPoint(),spriteSize);
				spriteWin->DrawNow();
				TheClient->WaitForRedrawsToFinish();
				retVal=TheClient->iScreen->RectCompare(TRect(TPoint(),spriteSize),TRect(spritePos,spriteSize),CWsScreenDevice::EIncludeSprite);
				TEST(retVal);
				if(!retVal)
					{
					_LIT(KLog,"Third position of sprite fails - does not compare to same bitmap drawn in window.");
					LOG_MESSAGE(KLog);
					}
				}

			CleanupStack::PopAndDestroy(spriteWin);
			CleanupStack::PopAndDestroy(&sprite);
			RestoreScreenMode();
			}
		}
	else
		{
		const TInt KScreenModeWithOffset = 2;
		TheClient->iGroup->GroupWin()->EnableScreenChangeEvents();
		iTestWin->SetVisible(EFalse);
		TheClient->Flush();
		iConnection2->iClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		TPixelsAndRotation pixelsAndRotation1;
		TheClient->iScreen->GetScreenModeSizeAndRotation(KScreenModeWithOffset,pixelsAndRotation1);
		ChangeScreenMode(TheClient,pixelsAndRotation1,KScreenModeWithOffset);
		RWsSprite sprite[2];
		for (TInt ii=0;ii<2;ii++)
			{
			SetUpSpriteLC(sprite[ii],TheClient->iWs,*TheClient->iGroup->WinTreeNode());
			}
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		TPixelsAndRotation pixelsAndRotation2;
		TheClient->iScreen->GetScreenModeSizeAndRotation(0,pixelsAndRotation2);
		iConnection2->iSpriteWin->UpdateState(0);
		iConnection2->iSpriteWin->SetVisible(ETrue);
		iConnection2->iClient->Flush();
		TSize spriteSize=iSpriteBitmap.SizeInPixels();
		TPoint screenMode2Origin;
		screenMode2Origin=TheClient->iScreen->GetScreenModeOrigin(KScreenModeWithOffset);
		//Check all reference bitmaps can be drawn inside the screen
		MDisplayControl* interface = static_cast<MDisplayControl*>
					(TheClient->iScreen->GetInterface(MDisplayControl::ETypeId));
		if(interface)
			{
			TDisplayConfiguration config;
			interface->GetConfiguration(config);
			TSize screenSize;
			config.GetResolution(screenSize);
			TEST(screenSize.iWidth > screenMode2Origin.iX+CheckSpritePos.iX+spriteSize.iWidth+30
					&& screenSize.iHeight > screenMode2Origin.iY+CheckSpritePos.iY + spriteSize.iHeight+30);
			}
		
		retVal = iConnection2->iClient->iScreen->RectCompare(TRect(screenMode2Origin+CheckSpritePos,spriteSize),TRect(screenMode2Origin,spriteSize),CWsScreenDevice::EIncludeSprite);
		TEST(retVal);
		if(!retVal)
			INFO_PRINTF3(_L("iConnection2->iClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);

		TheClient->iScreen->SetCurrentRotations(KScreenModeWithOffset,CFbsBitGc::EGraphicsOrientationRotated180);
		ChangeScreenMode(TheClient,pixelsAndRotation1,KScreenModeWithOffset);
		TheClient->Flush();
		sprite[0].SetPosition(TPoint(0,0));
		sprite[1].SetPosition(TPoint(30,30));
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		iConnection2->iSpriteWin->UpdateState(1);
		iConnection2->iClient->Flush();
		retVal = iConnection2->iClient->iScreen->RectCompare(TRect(screenMode2Origin+CheckSpritePos,spriteSize+TPoint(30,30)),TRect(screenMode2Origin,spriteSize+TPoint(30,30)),CWsScreenDevice::EIncludeSprite);
		TEST(retVal);
		if(!retVal)
			INFO_PRINTF3(_L("iConnection2->iClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);

		TheClient->iScreen->SetCurrentRotations(KScreenModeWithOffset,CFbsBitGc::EGraphicsOrientationNormal);
		ChangeScreenMode(TheClient,pixelsAndRotation2,0);
		iConnection2->iSpriteWin->SetExt(TPoint(215,0),TSize(218,240));
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		sprite[0].SetPosition(TPoint(-10,20));
		sprite[1].SetPosition(TPoint(-10,40));
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		ChangeScreenMode(TheClient,pixelsAndRotation1,ESizeEnforcementPixelsAndRotation,KScreenModeWithOffset);
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		iConnection2->iSpriteWin->UpdateState(2);
		iConnection2->iClient->Flush();
		retVal = iConnection2->iClient->iScreen->RectCompare(TRect(screenMode2Origin+CheckSpritePos+TPoint(0,20),spriteSize+TPoint(-10,20)),TRect(screenMode2Origin+TPoint(0,20),spriteSize+TPoint(-10,20)),CWsScreenDevice::EIncludeSprite);
		TEST(retVal);
		if(!retVal)
			INFO_PRINTF3(_L("iConnection2->iClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);

		CleanupStack::PopAndDestroy(2,&sprite[0]);
		iConnection2->iSpriteWin->SetVisible(EFalse);
		ChangeScreenMode(TheClient,pixelsAndRotation2,ESizeEnforcementPixelsAndRotation,0);
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		}
#endif
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0105

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Rotation Tests
					REQUIREMENT: CR PHAR-5SJGAM, PREQ673
					API: RWindowBase::InquireOffset(), RWindowBase::AbsPosition(),RWindowBase::Position()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Goes through all the screen modes defined in the wsini file and all the rotations in
					each screen mode. For each case sets the new screen mode and the the rotation on the current
					client's screen device. Then sets a suitable size and position of 2 windows (an RWindow
					and a child RBackedUpWindow) whose group window has the above screen device.

@SYMTestExpectedResults Checks the windows are positioned correctly according to the origin and rotation
					of the new screen mode.

*/
void CTScreenModePositioning::RotationTestsL()
	{
	RBlankWindow color256(TheClient->iWs);
	User::LeaveIfError(color256.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	CleanupClosePushL(color256);
	color256.SetRequiredDisplayMode(EColor256);
	color256.SetOrdinalPosition(2);
	color256.Activate();
	TInt ii;
	for (ii=0;ii<TheClient->iScreenModes.Count();)
		{
		iCurrentMode=TheClient->iScreenModes[ii];
		SetScreenMode(TheClient,iCurrentMode,ESizeEnforcementPixelsAndRotation);
		TPixelsAndRotation pixelsAndRotation;
		TheClient->iScreen->GetDefaultScreenSizeAndRotation(pixelsAndRotation);
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
			ii++;
			}
		TInt currentRotation=(*rotations)[jj];
		TheClient->iScreen->SetCurrentRotations(oldCurrentMode,REINTERPRET_CAST(CFbsBitGc::TGraphicsOrientation&,currentRotation));
		CleanupStack::PopAndDestroy(rotations);
		iCurrentScreenModeOrigin=TheClient->iScreen->GetScreenModeOrigin(oldCurrentMode);
		iCurrentScreenModeOrigin=TheClient->iScreen->GetScreenModeScaledOrigin(oldCurrentMode);
		TPoint point1(iCurrentScreenModeOrigin.iX,iCurrentScreenModeOrigin.iY+(iCurrentScreenModeOrigin.iY+pixelsAndRotation.iPixelSize.iHeight)/2);
		TPoint point2(iCurrentScreenModeOrigin.iX+(iCurrentScreenModeOrigin.iX+pixelsAndRotation.iPixelSize.iWidth)/2,pixelsAndRotation.iPixelSize.iHeight+iCurrentScreenModeOrigin.iY);
		TRect rect0(point1,point2);
		iTestWin->SetExtL(TPoint(),rect0.Size());
		iTestWin->Invalidate();
		
		iTestWin->SetVisible(ETrue);
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		TestTopClientWindowPositionAPIs(TPoint(),iTestWin->BaseWin());
		TPoint backedUpWinPt=TPoint(rect0.Width()/3,rect0.Height()/4);
		iBackedUpWin->SetVisible(ETrue);

		CleanupStack::PushL(TCleanupSetVisible<EFalse>(iBackedUpWin));
		CleanupStack::PushL(TCleanupSetVisible<EFalse>(iTestWin));

		
		iBackedUpWin->SetExtL(backedUpWinPt,TSize(rect0.Width()/6,rect0.Height()/6));
		TestChildWindowPositionAPIs(backedUpWinPt,TPoint(),iBackedUpWin->BaseWin(),iTestWin->BaseWin());

		CleanupStack::PopAndDestroy(2,iBackedUpWin); //TCleanupSetVisible
		}
	CleanupStack::PopAndDestroy(&color256);
	}

TBool CTScreenModePositioning::ScalingSupportedByDisplayMode()
	{
	//PeterI Standard ScreenDriver only supports scaling in EColor256 and EColor64k
	//see CDrawBitmap::CanBeScaled(), as mk3 is always in 16mu the tests will not pass.
	TBool ret=EFalse;
	TDisplayMode mode=TheClient->iScreen->DisplayMode();
	if (mode==EColor64K || mode==EColor256)
		{
		ret=ETrue;
		}
	return ret;
	}
void CTScreenModePositioning::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KWindowTests,"Window Tests");
	_LIT(KSpriteTest,"Sprite Test");
	_LIT(KRotationTests,"Rotation Tests");
	((CTScreenModePositioningStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(iTest->iState)
		{
	case 0:
		((CTScreenModePositioningStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		if (!CheckNonZeroOriginsSupportedOrNot())
			{
			_LIT(KLog,"Non Zero Origins not supported");
			LOG_MESSAGE(KLog);
			TestComplete();
			return;
			}
		break;
	case 1:
		iTest->LogSubTest(KWindowTests);
		if(ScalingSupportedByDisplayMode())
			WindowTestsL();
		((CTScreenModePositioningStep*)iStep)->SetOverallTestStepID(_L("GRAPHICS-WSERV-0098"));
		break;
	case 2:
		((CTScreenModePositioningStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0104"));
		iTest->LogSubTest(KSpriteTest);
		if(ScalingSupportedByDisplayMode())
			SpriteTestL();
		break;
	case 3:
		((CTScreenModePositioningStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0105"));
		iTest->LogSubTest(KRotationTests);
		RotationTestsL();
		break;
	default:
		((CTScreenModePositioningStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
  		((CTScreenModePositioningStep*)iStep)->CloseTMSGraphicsStep();
		RestoreScreenMode();
		TestComplete();
		return;
		}
	((CTScreenModePositioningStep*)iStep)->RecordTestResultL();
	++iTest->iState; // still used in the remaining code
	}

__WS_CONSTRUCT_STEP__(ScreenModePositioning)
