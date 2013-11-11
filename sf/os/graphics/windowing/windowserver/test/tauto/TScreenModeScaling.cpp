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
// Test code for screen mode scaling CR
// Test code for the Scaling part of Change Request PHAR-5SJGAM 
// ("Enable screen mode positioning and scaling").
// Tests screen scale being configurable for a screen mode -  
// eg it is now possible to set in wsini.ini amount by which a screen 
// mode's screen will be scaled when drawn on the physical screen. 
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TScreenModeScaling.h"

#define MY_TEST_BITMAP _L("Z:\\WSTEST\\MYTEST.MBM")

LOCAL_D TSize FullScreenModeSize;
LOCAL_D TInt Copy2ndHalfOfScreen;

void ClearBitmap(CFbsBitmap* aBitMap)
	{
	// clear the content of bitmap before it is used for copying
	CFbsBitmapDevice *device=CFbsBitmapDevice::NewL(aBitMap);
	CleanupStack::PushL(device);
	CFbsBitGc *gc=NULL;
	User::LeaveIfError(device->CreateContext(gc));
	CleanupStack::PushL(gc);
	gc->Clear();
	CleanupStack::PopAndDestroy(2,device);
	}

/*CBitMapWin*/

void CBitMapWin::Draw()
	{
	iGc->BitBlt(TPoint(),&iBackup->Bitmap());
	}

/*CTestSpriteWin*/

CTestSpriteWin::~CTestSpriteWin()
	{
	}

void CTestSpriteWin::UpdateWin(TPoint aOrigin)
	{
	SetOrigin(aOrigin);
	DrawNow();
	}

void CTestSpriteWin::Draw()
	{
	iGc->BitBlt(iOrigin,&iSpriteBitmap);
	}

/*CTScreenModeScaling*/

CTScreenModeScaling::CTScreenModeScaling(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}
	
CTScreenModeScaling::~CTScreenModeScaling()
	{
	delete iTestWin;
	delete iBlankWin;
	delete iBackedUpWin;
	delete iTestChildWin;
	delete iScreenBitmap;
	delete iBitmapWin;
	delete iCheckWin;
	delete iTransWin;
	delete iBackgroundWin;
	delete iCheckBitmap;
	delete iTransparencyBitmap;
	delete iBackgroundBitmap;
	delete iForegroundBitmap;
	}

void CTScreenModeScaling::ConstructL()
	{
	iDisplayMode=TheClient->iScreen->DisplayMode();
	if (iDisplayMode<EColor256)
		iDisplayMode=EColor256;
	User::LeaveIfError(iSpriteBitmap.Load(MY_TEST_BITMAP,0));

	TheClient->iScreen->SetAppScreenMode(TheClient->iScreenModes[0]);
	FullScreenModeSize=TheClient->iScreen->SizeInPixels();
	iBlankWin=new(ELeave) CTBlankWindow();
	iBlankWin->ConstructL(*TheClient->iGroup);
	User::LeaveIfError(iBlankWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	iBlankWin->SetExt(TPoint(),FullScreenModeSize);
	iBlankWin->Activate();

	iTestWin=new(ELeave) CBasicWin;
	iTestWin->ConstructExtLD(*TheClient->iGroup,TPoint(),FullScreenModeSize);
	User::LeaveIfError(iTestWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	iTestWin->AssignGC(*TheClient->iGc);
	iTestWin->SetVisible(EFalse);
	iTestWin->Activate();
	iTestWin->BaseWin()->SetShadowDisabled(ETrue);
	iTestWin->BaseWin()->SetShadowHeight(0);

	iBackedUpWin=new(ELeave) CTBackedUpWin(iDisplayMode);
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

	iScreenBitmap=new(ELeave) CFbsBitmap();
	User::LeaveIfError(iScreenBitmap->Create(TSize(FullScreenModeSize.iWidth/2,FullScreenModeSize.iHeight),iDisplayMode));

	iBitmapWin=new(ELeave) CBmpWin(*iScreenBitmap);
	iBitmapWin->ConstructExtLD(*TheClient->iGroup,TPoint(),iScreenBitmap->SizeInPixels());
	User::LeaveIfError(iBitmapWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	iBitmapWin->AssignGC(*TheClient->iGc);
	iBitmapWin->Activate();
	iBitmapWin->SetVisible(EFalse);
	iBitmapWin->BaseWin()->SetShadowDisabled(ETrue);
	iBitmapWin->BaseWin()->SetShadowHeight(0);

	//.. Create all the bitmaps and transparent windows
	iWinSize.SetSize(20,20);
	iTransparencyBitmap=CBitmap::NewL(iWinSize,EGray256);
	iBackgroundBitmap=CBitmap::NewL(iWinSize,iDisplayMode);
	iForegroundBitmap=CBitmap::NewL(iWinSize,iDisplayMode);

	CBitMapWin* backgroundWin=new(ELeave) CBitMapWin(iBackgroundBitmap);
	backgroundWin->ConstructExtLD(*TheClient->iGroup,TPoint(),iWinSize);
	iBackgroundWin=backgroundWin;
	User::LeaveIfError(iBackgroundWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	iBackgroundWin->SetVisible(EFalse);
	iBackgroundWin->BaseWin()->SetShadowDisabled(ETrue);
	iBackgroundWin->BaseWin()->SetShadowHeight(0);
	iBackgroundWin->AssignGC(*TheClient->iGc);
	iBackgroundWin->Activate();

	CBitMapWin* transWin=new(ELeave) CBitMapWin(iForegroundBitmap);
	transWin->ConstructExtLD(*TheClient->iGroup,TPoint(),iWinSize);
	iTransWin=transWin;
	RWindowBase& transWinB=*iTransWin->BaseWin();
	User::LeaveIfError(transWinB.SetRequiredDisplayMode(EColor256));
	transWinB.SetShadowDisabled(ETrue);
	transWinB.SetShadowHeight(0);
	iTransWin->SetVisible(EFalse);
	iTransWin->AssignGC(*TheClient->iGc);
	iTransWin->Win()->SetTransparencyBitmap(iTransparencyBitmap->Bitmap());
	iTransWin->Activate();

	CTBackedUpWin* checkWin=new(ELeave) CTBackedUpWin(iDisplayMode);
	checkWin->ConstructExtLD(*TheClient->iGroup,TPoint(iWinSize.iWidth+1,0),iWinSize);
	iCheckWin=checkWin;
	RBackedUpWindow& win=*iCheckWin->BackedUpWin();
	win.SetShadowHeight(0);
	iCheckWin->Activate();
	win.MaintainBackup();
	iCheckBitmap=CBitmap::NewL(win.BitmapHandle());
	iCheckWin->BaseWin()->SetShadowDisabled(ETrue);
	iCheckWin->BaseWin()->SetShadowHeight(0);
	iCheckWin->BaseWin()->SetVisible(EFalse);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	}

void CTScreenModeScaling::TestTopClientWindowPositionAPIs(TPoint aPos,RWindowBase* aWin)
	{
	TEST(aWin->AbsPosition()==aPos);
	TEST(aWin->Position()==aPos);
	TEST(aWin->InquireOffset(*TheClient->iGroup->GroupWin())==aPos);
	}

void CTScreenModeScaling::TestChildWindowPositionAPIs(TPoint aPos,TPoint aParentPos,RWindowBase* aWin,RWindowBase* aParentWin)
	{
	TEST(aWin->AbsPosition()==aParentPos+aPos);
	TEST(aWin->Position()==aPos);
	TEST(aWin->InquireOffset(*TheClient->iGroup->GroupWin())==aParentPos+aPos);
	TEST(aWin->InquireOffset(*aParentWin)==aPos);
	}

void CTScreenModeScaling::TestRect()
	{
	// Here if the width or height of the screen cannot be divided by 2 
	// then make both the rect size same by reducing the first or second which ever is smaller
	// and make sure that top left corner is not disturbed.
	TRect rect1=TRect(PhysicalToLogical(TPoint()-iCurrentScreenModeOrigin,iCurrentScreenModeScale),
					  PhysicalToLogical(TPoint(FullScreenModeSize.iWidth/2,FullScreenModeSize.iHeight)-iCurrentScreenModeOrigin,
										iCurrentScreenModeScale)
					  );
	TRect rect2=TRect(PhysicalToLogical(TPoint(FullScreenModeSize.iWidth/2,0)-iCurrentScreenModeOrigin,
										iCurrentScreenModeScale),
					  PhysicalToLogical(FullScreenModeSize.AsPoint()-iCurrentScreenModeOrigin,
										iCurrentScreenModeScale)
					  );
	if ((Abs(rect1.iBr.iX-rect1.iTl.iX)>Abs(rect2.iBr.iX-rect2.iTl.iX)) || (Abs(rect1.iBr.iY-rect1.iTl.iY)>Abs(rect2.iBr.iY-rect2.iTl.iY)))
		{
		rect1.SetRect(rect1.iTl,rect2.Size());
		}
	if ((Abs(rect1.iBr.iX-rect1.iTl.iX)<Abs(rect2.iBr.iX-rect2.iTl.iX)) || (Abs(rect1.iBr.iY-rect1.iTl.iY)<Abs(rect2.iBr.iY-rect2.iTl.iY)))
		{
		rect2.SetRect(rect2.iTl,rect1.Size());
		}
	TBool retVal = TheClient->iScreen->RectCompare(rect1,rect2);
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare(rect1,rect2) return value  - Expected: %d, Actual: %d"), ETrue, retVal);		
	}

void CTScreenModeScaling::CompareRegionsL(const TRegion &aRegion1,const TRegion &aRegion2)
	{
	RRegion tmp;
	tmp.Copy(aRegion1);
	tmp.SubRegion(aRegion2);
	if (tmp.CheckError())
		User::Leave(KErrNoMemory);	
	TBool retVal = tmp.IsEmpty();
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("RRegion1.IsEmpty() return value  - Expected: %d, Actual: %d"), ETrue, retVal);		
	tmp.Copy(aRegion2);
	tmp.SubRegion(aRegion1);
	if (tmp.CheckError())
		User::Leave(KErrNoMemory);
	retVal = tmp.IsEmpty();
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("RRegion2.IsEmpty() return value  - Expected: %d, Actual: %d"), ETrue, retVal);		

	tmp.Close();
	}

void CTScreenModeScaling::SetUpSpriteLC(RWsSprite &aSprite, RWsSession &aSession, RWindowTreeNode &aWindow,TInt aFlags)
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

void CTScreenModeScaling::TestGetInvalidRegionL(TRect& aRect)
	{
	RRegion testRegion;
	RRegion invalid;
	iTestWin->Invalidate(aRect);
	testRegion.AddRect(aRect);
	iTestWin->Win()->GetInvalidRegion(invalid);
	CleanupClosePushL(testRegion);
	CleanupClosePushL(invalid);
	CompareRegionsL(testRegion, invalid);
	CleanupStack::PopAndDestroy(2, &testRegion);
	TheClient->WaitForRedrawsToFinish();
	}

void CTScreenModeScaling::CopyAndCompareL()
	{
	TInt oldOrdinalPriority = TheClient->iGroup->GroupWin()->OrdinalPriority();
	TInt oldOrdinalPosition = TheClient->iGroup->GroupWin()->OrdinalPosition();
	// the following line makes sure that a console object hidden outside of
	// screens range doesn't affect test results ocerlapping the bitmap window
	TheClient->iGroup->GroupWin()->SetOrdinalPosition(0, 65535); 

	// clear the content of bitmap before it is used for copying
	ClearBitmap(iScreenBitmap);	

	// Copy first half or second half of the screen to a bitmap then paste it to second or first half of the screen
	TRect testWinRect(PhysicalToLogical(TPoint((Copy2ndHalfOfScreen ? FullScreenModeSize.iWidth/2 : 0),0)-iCurrentScreenModeOrigin,iCurrentScreenModeScale),
					  PhysicalToLogical(TPoint((Copy2ndHalfOfScreen ? FullScreenModeSize.iWidth : FullScreenModeSize.iWidth/2),FullScreenModeSize.iHeight)-iCurrentScreenModeOrigin,iCurrentScreenModeScale)
					  );
	TheClient->iScreen->CopyScreenToBitmap(iScreenBitmap,testWinRect);
	iBitmapWin->SetExt(PhysicalToLogical(TPoint((Copy2ndHalfOfScreen ? 0 : FullScreenModeSize.iWidth/2),0)-iCurrentScreenModeOrigin,iCurrentScreenModeScale),testWinRect.Size());
	iBitmapWin->SetVisible(ETrue);
	iBitmapWin->DrawNow();
	TheClient->Flush();
	TestRect();
	iBitmapWin->SetVisible(EFalse);
	iTestWin->SetVisible(EFalse);
	TheClient->Flush();

	TheClient->iGroup->GroupWin()->SetOrdinalPosition(oldOrdinalPosition, oldOrdinalPriority); 
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0093

@SYMDEF             DEF081259, DEF111847

@SYMTestCaseDesc    Window tests
					REQUIREMENT: PREQ673 (Screen Scaling)
					API: RWindowBase::InquireOffset(), RWindowBase::AbsPosition(),RWindowBase::Position()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Loops through all the screen modes present in the wsini file, and then moves to the test screen
	mode where it goes on changing the screen scale and origin. In each of the wsini screen modes and test
	screen modes, it checks the test window's API's RWindowBase::InquireOffset(), RWindowBase::AbsPosition()
	RWindowBase::Position(). Then it copies the content from either first or second half to second or first
	half and compares both the regions so that its content are same.
	Additionally, as part of defect fix DEF111847, this also loops through all the display modes, and if possible
	tests each with the above described method. Previously, it use to only test Color256.

@SYMTestExpectedResults The content of both halves of the screen should match.
*/		
void CTScreenModeScaling::WindowTestsL()
	{
	INFO_PRINTF1(_L("AUTO  WindowTests : "));
	
	TDisplayMode curDispMode; // Holds the current display mode being tested 
	
	for(curDispMode = EGray2; curDispMode < EColorLast; curDispMode = TDisplayMode(curDispMode+1))
		{
		if (curDispMode == ERgb)
			{
			continue;
			}
		CTClient* client=new(ELeave) CTClient();
		CleanupStack::PushL(client);
		client->SetScreenNumber(iTest->iScreenNumber);
		client->ConstructL();
		client->iGroup=new(ELeave) TestWindowGroup(client);
		client->iGroup->ConstructL();
		client->iGroup->WinTreeNode()->SetOrdinalPosition(1);
		RBlankWindow testWindow(client->iWs);
		User::LeaveIfError(testWindow.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
		CleanupClosePushL(testWindow);
	
		TInt setDispMode; // Holds the actual display mode that was set
		setDispMode = testWindow.SetRequiredDisplayMode(curDispMode);
		
		//Only do the tests if the requested mode was actually set
		if(curDispMode == setDispMode)
			{
			//Create and show DisplayMode details message
			_LIT(KModeDetails, "Display Mode: ");
			TBuf<30> modeDetailsMessage(KModeDetails);
			modeDetailsMessage.Append(DisplayModeAsString(curDispMode));
			LOG_MESSAGE(modeDetailsMessage);
			
			testWindow.Activate();
			TheClient->iGroup->GroupWin()->EnableScreenChangeEvents();
			TInt numOfModes=TheClient->iScreenModes.Count();
			TInt ii;
			for (ii=0; ii<numOfModes; ++ii)
				{
				INFO_PRINTF1(_L(" Start of Loop"));
				iCurrentMode=TheClient->iScreenModes[ii];
				TPixelsAndRotation pixelsAndRotation;
				iCurrentScreenModeOrigin=TheClient->iScreen->GetScreenModeOrigin(iCurrentMode);
				iCurrentScreenModeScale=TheClient->iScreen->GetScreenModeScale(iCurrentMode);
				TheClient->iScreen->GetScreenModeSizeAndRotation(iCurrentMode,pixelsAndRotation);
				if (pixelsAndRotation.iRotation==CFbsBitGc::EGraphicsOrientationNormal)
					{
					INFO_PRINTF1(_L(" Do Tests"));
					TRAPD(ret,DoWindowTestsL());
					TEST(ret==KErrNone);
					if (ret!=KErrNone)
						{
						INFO_PRINTF3(_L("DoWindowTestsL() return value  - Expected: %d, Actual: %d"), KErrNone, ret);		
						}

					INFO_PRINTF1(_L(" Window Trees"));
					client->iScreen->SetAppScreenMode(iCurrentMode);
					client->Flush();
					}
				}
				TestDifferentScales(numOfModes-1);
			}
		CleanupStack::PopAndDestroy(2,client);
		}
	}

void CTScreenModeScaling::TestDifferentScales(TInt aLastModeIdx)
	{
	if (aLastModeIdx<0) return;
	TPixelsAndRotation pixelsAndRotation;
	for (TInt modeCount=aLastModeIdx;modeCount>=0;--modeCount)
		{
		TheClient->iScreen->GetScreenModeSizeAndRotation(TheClient->iScreenModes[modeCount],pixelsAndRotation);
		if (pixelsAndRotation.iRotation==CFbsBitGc::EGraphicsOrientationNormal)
			break;
		}
	iCurrentMode=TheClient->iScreenModes[aLastModeIdx];
	TheClient->iScreen->SetAppScreenMode(iCurrentMode);
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	TSizeMode storeModeData=TheClient->iScreen->GetCurrentScreenModeAttributes();
	TSizeMode testMode=storeModeData;
	for (TInt xScale=1;xScale<4;++xScale)
		{
		for (TInt yScale=1;yScale<4;++yScale)
			{
			testMode.iScreenScale=TSize(xScale,yScale);
			TestDifferentOrigin(testMode,TPoint(20,20));
			TestDifferentOrigin(testMode,TPoint(20,30));
			TestDifferentOrigin(testMode,TPoint(30,20));
			TestDifferentOrigin(testMode,TPoint(FullScreenModeSize.iWidth/2+640/FullScreenModeSize.iWidth*10,60));
			}
		}
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	TheClient->iScreen->SetCurrentScreenModeAttributes(storeModeData);
	TInt defaultMode=TheClient->iScreenModes[0];
	TheClient->iScreen->SetAppScreenMode(defaultMode);
	TheClient->iScreen->SetScreenMode(defaultMode);
	}

void CTScreenModeScaling::TestDifferentOrigin(TSizeMode &aMode,TPoint aOrigin)
	{
	TheClient->iScreen->SetScreenMode(iCurrentMode);//.. this is required because at the end of next screenmode test it will be set to zero screen mode
	aMode.iOrigin=aOrigin;
	TheClient->iScreen->SetCurrentScreenModeAttributes(aMode);
	TheClient->iScreen->SetAppScreenMode(iCurrentMode);
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	iCurrentScreenModeOrigin=TheClient->iScreen->GetScreenModeOrigin(iCurrentMode);
	iCurrentScreenModeScale=TheClient->iScreen->GetScreenModeScale(iCurrentMode);
	TRAPD(ret,DoWindowTestsL());
	TEST(ret==KErrNone);
	if (ret!=KErrNone)
		INFO_PRINTF3(_L("DoWindowTestsL() return value  - Expected: %d, Actual: %d"), KErrNone, ret);		
	}

void CTScreenModeScaling::DoWindowTestsL()
	{
	TheClient->iScreen->SetAppScreenMode(iCurrentMode);
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	Copy2ndHalfOfScreen=(iCurrentScreenModeOrigin.iX>FullScreenModeSize.iWidth/2? 1 : 0);
	TRect testWinRect(PhysicalToLogical(TPoint(),iCurrentScreenModeScale),
					  PhysicalToLogical(TPoint((Copy2ndHalfOfScreen ? FullScreenModeSize.iWidth 
																	: FullScreenModeSize.iWidth/2),
											   FullScreenModeSize.iHeight)-iCurrentScreenModeOrigin,iCurrentScreenModeScale) 
					  );
	testWinRect.Shrink(10,10);
	iTestWinSize=testWinRect.Size();
	PositionTestL(testWinRect.iTl);
	testWinRect.Shrink(10,10);
	iTestWinSize=testWinRect.Size();
	PositionTestL(testWinRect.iTl);
	BackedUpChildWindowTestL(testWinRect.iTl);
	GetInvalidRegionTestL(testWinRect.iTl);
	//NextScreenModeTestL(testWinRect.iTl);		//This test needs a lot more work ###
	}

void CTScreenModeScaling::PositionTestL(TPoint aPostion)
	{
	iTestWin->SetExt(aPostion,iTestWinSize);
	iTestWin->SetVisible(ETrue);
	iTestWin->Invalidate();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TestTopClientWindowPositionAPIs(aPostion,iTestWin->BaseWin());
	CopyAndCompareL();
	}

void CTScreenModeScaling::BackedUpChildWindowTestL(TPoint aPostion)
	{
	iTestWin->SetVisible(ETrue);
	TPoint backedUpWinPt=TPoint(iTestWinSize.iWidth/3,iTestWinSize.iHeight/4);
	iBackedUpWin->SetExtL(backedUpWinPt,TSize(iTestWinSize.iWidth/6,iTestWinSize.iHeight/6));
	iBackedUpWin->SetVisible(ETrue);
	TestChildWindowPositionAPIs(backedUpWinPt,aPostion,iBackedUpWin->BaseWin(),iTestWin->BaseWin());
	aPostion+=TPoint(10,10);
	iTestWin->SetPos(aPostion);
	TestTopClientWindowPositionAPIs(aPostion,iTestWin->BaseWin());
	TestChildWindowPositionAPIs(backedUpWinPt,aPostion,iBackedUpWin->BaseWin(),iTestWin->BaseWin());
	iTestWin->Invalidate();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TestTopClientWindowPositionAPIs(aPostion,iTestWin->BaseWin());
	TestChildWindowPositionAPIs(backedUpWinPt,aPostion,iBackedUpWin->BaseWin(),iTestWin->BaseWin());
	CopyAndCompareL();
	iBackedUpWin->SetVisible(EFalse);
	}

void CTScreenModeScaling::NextScreenModeTestL(TPoint aPos)
	{
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
		screenModeOrigin=TheClient->iScreen->GetScreenModeOrigin(mode);
		TheClient->iScreen->GetScreenModeSizeAndRotation(mode,pixelsAndRotation);
		}
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
	Copy2ndHalfOfScreen=(iCurrentScreenModeOrigin.iX>FullScreenModeSize.iWidth/2? 1 : 0);
	TheClient->iScreen->SetAppScreenMode(mode);
	TheClient->iScreen->SetScreenMode(mode);
	iTestWin->SetVisible(ETrue);
	CopyAndCompareL();
	TheClient->iScreen->SetAppScreenMode(defaultMode);
	TheClient->iScreen->SetScreenMode(defaultMode);
	CleanupStack::PopAndDestroy(basicWin);
	}

void CTScreenModeScaling::GetInvalidRegionTestL(TPoint aPos)
	{
	iTestWin->SetExt(aPos,iTestWinSize);
	iTestWin->SetVisible(ETrue);
	iTestChildWin->SetExt(TPoint(iTestWinSize.iWidth>>2,iTestWinSize.iHeight>>2),TSize(iTestWinSize.iWidth>>1,iTestWinSize.iHeight>>1));
	iTestChildWin->SetVisible(ETrue);
	TheClient->Flush(); // ensure testchildwin is on-screen

	CArrayFixFlat<TRect> *rectList=new(ELeave) CArrayFixFlat<TRect>(3);
	CleanupStack::PushL(rectList);
	rectList->AppendL(TRect(1,1,5,2));
	rectList->AppendL(TRect(iTest->StdTestWindowSize().iWidth>>1,iTest->StdTestWindowSize().iHeight>>1,iTest->StdTestWindowSize().iWidth,iTest->StdTestWindowSize().iHeight));
	rectList->AppendL(TRect(2,0,4,5));
	
	// set iTestWin to a 'clean state' before invalidating rects in the window
	iTestWin->DrawNow();
	TheClient->Flush();

	// invalidate the various rectangles in iTestWin & then initiate a redraw of the window
	RRegion invalidRegion;
	RRegion region;
	for (TInt index=0;index<rectList->Count();index++)
		{
		const TRect& myRect = (*rectList)[index];
		iTestWin->Invalidate(myRect);
		region.AddRect(myRect);
		}
	CleanupStack::PopAndDestroy(rectList);
	CleanupClosePushL(invalidRegion);
	CleanupClosePushL(region);
	iTestWin->Win()->GetInvalidRegion(invalidRegion);
	CompareRegionsL(region, invalidRegion);
	CleanupStack::PopAndDestroy(2,&invalidRegion);
	iTestChildWin->SetVisible(EFalse);

	// ensure the test child win is removed
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	TRect rect1(iTestWinSize);
	TestGetInvalidRegionL(rect1);
	TInt width=iTestWinSize.iWidth;
	TInt height=iTestWinSize.iHeight;
	TRect rect2(TPoint(width/6,height/6),TSize(width/3,height/3));
	TestGetInvalidRegionL(rect2);
	
	// invalidate the entire test window
	iTestWin->Invalidate();
	TheClient->Flush();

	iTestWin->SetPos(TPoint(15,15));
	iTestWin->SetPos(TPoint());
	iTestWin->SetPos(TPoint(-15,-15));
	iTestWin->SetPos(aPos);	
	RRegion invalid;
	RRegion testRegion(rect1);
	CleanupClosePushL(invalid);
	CleanupClosePushL(testRegion);
	iTestWin->Win()->GetInvalidRegion(invalid);
	TBool retVal = !invalid.CheckError();
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("RRegion.CheckError() return value  - Expected: %d, Actual: %d"), ETrue, retVal);		
	TEST(invalid.BoundingRect().iBr.iX<=iTestWinSize.iWidth);
	if (invalid.BoundingRect().iBr.iX>iTestWinSize.iWidth)
		INFO_PRINTF3(_L("invalid.BoundingRect().iBr.iX<=iTestWinSize.iWidth  - Expected: %d, Actual: %d"), invalid.BoundingRect().iBr.iX, iTestWinSize.iWidth);		
	CompareRegionsL(testRegion,invalid);
	CleanupStack::PopAndDestroy(2,&invalid);

	// redraw the test window & confirm
	iTestWin->DrawNow();
	TheClient->Flush();
	CopyAndCompareL();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0094

@SYMDEF             DEF081259

@SYMTestCaseDesc    SpriteTestL
					REQUIREMENT: PREQ673 (Screen Scaling)
					API: RWsSprite::SetPosition()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     In the same way as in window tests it shifts the screen modes.
					In each screen modes it creates a sprite window and then it moves slightly
					either side or bottom of the sprite, then it compares both the regions
					to check whether content are same.

@SYMTestExpectedResults The content of both halves of the screen should match.
*/		
void CTScreenModeScaling::SpriteTestL()
	{
	CTClient* client=new(ELeave) CTClient();
	CleanupStack::PushL(client);
	client->SetScreenNumber(iTest->iScreenNumber);
	client->ConstructL();
	client->iGroup=new(ELeave) TestWindowGroup(client);
	client->iGroup->ConstructL();
	client->iGroup->WinTreeNode()->SetOrdinalPosition(1);
	RBlankWindow color256(client->iWs);
	User::LeaveIfError(color256.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	CleanupClosePushL(color256);
	color256.SetRequiredDisplayMode(EColor256);
	color256.Activate();
	TInt numOfModes=TheClient->iScreenModes.Count();
	TInt flushState=TheClient->iWs.SetAutoFlush(ETrue);
	TInt ii;
	for (ii=0; ii<numOfModes; ++ii)
		{
		iCurrentMode=TheClient->iScreenModes[ii];
		TPixelsAndRotation pixelsAndRotation;
		iCurrentScreenModeOrigin=TheClient->iScreen->GetScreenModeOrigin(iCurrentMode);
		iCurrentScreenModeScale=TheClient->iScreen->GetScreenModeScale(iCurrentMode);
		TheClient->iScreen->GetScreenModeSizeAndRotation(iCurrentMode,pixelsAndRotation);
		DoSpriteTestsL();
		client->iScreen->SetAppScreenMode(iCurrentMode);
		client->Flush();
		}
	iCurrentMode=TheClient->iScreenModes[numOfModes-1];
	TheClient->iScreen->SetAppScreenMode(iCurrentMode);
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	TSizeMode storeModeData=TheClient->iScreen->GetCurrentScreenModeAttributes();
	TSizeMode testMode=storeModeData;
	for (TInt xScale=1;xScale<4;xScale++)
		{
		for (TInt yScale=1;yScale<4;yScale++)
			{
			testMode.iScreenScale=TSize(xScale,yScale);
			TestDifferentOriginAndScaleForSpritesL(testMode,TPoint(20,20));
			TestDifferentOriginAndScaleForSpritesL(testMode,TPoint(20,30));
			TestDifferentOriginAndScaleForSpritesL(testMode,TPoint(30,20));
			TestDifferentOriginAndScaleForSpritesL(testMode,TPoint(FullScreenModeSize.iWidth/2+10,60));
			}
		}
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	TheClient->iScreen->SetCurrentScreenModeAttributes(storeModeData);
	TInt defaultMode=TheClient->iScreenModes[0];
	TheClient->iScreen->SetAppScreenMode(defaultMode);
	TheClient->iScreen->SetScreenMode(defaultMode);
	CleanupStack::PopAndDestroy(2,client);
	TheClient->iWs.SetAutoFlush(flushState);
	}

void CTScreenModeScaling::TestDifferentOriginAndScaleForSpritesL(TSizeMode &aMode, TPoint aOrigin)
	{
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	aMode.iOrigin=aOrigin;
	TheClient->iScreen->SetCurrentScreenModeAttributes(aMode);
	iCurrentScreenModeOrigin=TheClient->iScreen->GetScreenModeOrigin(iCurrentMode);
	iCurrentScreenModeScale=TheClient->iScreen->GetScreenModeScale(iCurrentMode);
	DoSpriteTestsL();
	}

void CTScreenModeScaling::DoSpriteTestsL()
	{
	TheClient->iScreen->SetAppScreenMode(iCurrentMode);
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	TPixelsAndRotation pixelsAndRotation;
	TheClient->iScreen->GetScreenModeSizeAndRotation(iCurrentMode,pixelsAndRotation);
	RWsSprite sprite;
	TSize spriteSize=iSpriteBitmap.SizeInPixels();
	if (spriteSize.iWidth*iCurrentScreenModeScale.iWidth>FullScreenModeSize.iWidth/2)
		spriteSize.iWidth=(FullScreenModeSize.iWidth/2-20)/iCurrentScreenModeScale.iWidth;
	SetUpSpriteLC(sprite,TheClient->iWs,*iBlankWin->BaseWin());
	sprite.SetPosition(TPoint());
	CTestSpriteWin* spriteWin=new(ELeave) CTestSpriteWin(iSpriteBitmap);
	CleanupStack::PushL(spriteWin);
	spriteWin->ConstructExtLD(*TheClient->iGroup,PhysicalToLogical(TPoint()-iCurrentScreenModeOrigin,iCurrentScreenModeScale),spriteSize);
	User::LeaveIfError(spriteWin->BaseWin()->SetRequiredDisplayMode(EColor256));
	spriteWin->AssignGC(*TheClient->iGc);
	spriteWin->SetVisible(ETrue);
	spriteWin->Activate();
	spriteWin->UpdateWin(TPoint());
	spriteWin->BaseWin()->SetShadowDisabled(ETrue);
	spriteWin->BaseWin()->SetShadowHeight(0);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TPoint spritePosition;
	if (iCurrentScreenModeOrigin.iX<=spriteSize.iWidth || iCurrentScreenModeOrigin.iY<=spriteSize.iHeight)
		{
		if (pixelsAndRotation.iRotation==CFbsBitGc::EGraphicsOrientationNormal || pixelsAndRotation.iRotation==CFbsBitGc::EGraphicsOrientationRotated180)
			spritePosition=TPoint(spriteSize.iWidth+1,0);
		else
			spritePosition=TPoint(0,spriteSize.iHeight+1);
		}
	sprite.SetPosition(spritePosition);	
	const TInt KAnimationGrace = 35000; //defined in server.cpp, but can be changed in wsini.ini
	User::After(KAnimationGrace);
	TBool retVal = TheClient->iScreen->RectCompare(TRect(PhysicalToLogical(TPoint()-iCurrentScreenModeOrigin,iCurrentScreenModeScale),spriteSize),TRect(spritePosition,spriteSize),CWsScreenDevice::EIncludeSprite);
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare(rect1,rect2) return value  - Expected: %d, Actual: %d"), ETrue, retVal);		

	sprite.SetPosition(TPoint());	
	CleanupStack::PopAndDestroy(spriteWin);
	CleanupStack::PopAndDestroy(&sprite);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0095

@SYMDEF             DEF081259

@SYMTestCaseDesc    Rotation Tests
					REQUIREMENT: PREQ673 (Screen Scaling)
					API: RWindowBase::InquireOffset(), RWindowBase::AbsPosition(),RWindowBase::Position()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     For each of the screen mode with all its rotation it checks whether the window
					is correctly placed in particular screen mode. Then it creates a child window
					and checks whether it s correctly placed.

@SYMTestExpectedResults Checks the windows are positioned correctly according to the origin,scale and rotation 
					of the new screen mode.
*/		
void CTScreenModeScaling::RotationTestsL()
	{
	CTClient* client=new(ELeave) CTClient();
	CleanupStack::PushL(client);
	client->SetScreenNumber(iTest->iScreenNumber);
	client->ConstructL();
	client->iGroup=new(ELeave) TestWindowGroup(client);
	client->iGroup->ConstructL();
	client->iGroup->WinTreeNode()->SetOrdinalPosition(1);
	RBlankWindow color256(client->iWs);
	User::LeaveIfError(color256.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	CleanupClosePushL(color256);
	color256.SetRequiredDisplayMode(EColor256);
	color256.Activate();
	TInt oldCurrentMode=0;
	TInt ii;
	//TInt screenMode=TheClient->iScreen->CurrentScreenMode();
	for (ii=0;ii<TheClient->iScreenModes.Count();)
		{
		iCurrentMode=TheClient->iScreenModes[ii];
		if (iCurrentMode!=oldCurrentMode)
			{
			client->iScreen->SetAppScreenMode(iCurrentMode);
			TheClient->iScreen->SetScreenMode(iCurrentMode);
			TheClient->iScreen->SetAppScreenMode(iCurrentMode);
			}
		TPixelsAndRotation pixelsAndRotation;
		TheClient->iScreen->GetDefaultScreenSizeAndRotation(pixelsAndRotation);
		oldCurrentMode=iCurrentMode;
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
		iCurrentScreenModeScale=TheClient->iScreen->GetScreenModeScale(oldCurrentMode);
		TRect testWinRect(PhysicalToLogical(TPoint(),iCurrentScreenModeScale),
						  PhysicalToLogical(TPoint((Copy2ndHalfOfScreen ? FullScreenModeSize.iWidth : FullScreenModeSize.iWidth/2),FullScreenModeSize.iHeight)-iCurrentScreenModeOrigin,iCurrentScreenModeScale) 
						 );
		testWinRect.Shrink(10,10);
		iTestWin->SetExtL(testWinRect.iTl,testWinRect.Size());
		iTestWin->Invalidate();
		iTestWin->SetVisible(ETrue);
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		TestTopClientWindowPositionAPIs(testWinRect.iTl,iTestWin->BaseWin());
		TPoint backedUpWinPt=TPoint(testWinRect.Width()/3,testWinRect.Height()/4);
		iBackedUpWin->SetVisible(ETrue);
		iBackedUpWin->SetExtL(backedUpWinPt,TSize(testWinRect.Width()/6,testWinRect.Height()/6));
		TestChildWindowPositionAPIs(backedUpWinPt,testWinRect.iTl,iBackedUpWin->BaseWin(),iTestWin->BaseWin());
		iTestWin->SetVisible(EFalse);
		iBackedUpWin->SetVisible(EFalse);
		}
	CleanupStack::PopAndDestroy(2,client);
	}

void CTScreenModeScaling::DrawTransparentWindows()
	{
	//.. First Draw on Backgroundbitmap
	iBackgroundBitmap->Gc().SetBrushColor(TRgb(255,0,255));
	iBackgroundBitmap->Gc().SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBackgroundBitmap->Gc().SetPenStyle(CGraphicsContext::ENullPen);
	iBackgroundBitmap->Gc().DrawRect(iWinSize);
	
	//.. Invalidate the background bitmap and do bitBlt to iCheckBitmap also
	iBackgroundWin->BaseWin()->SetVisible(ETrue);
	iBackgroundWin->DrawNow();

	//.. Copy to checkbitmap
	iCheckBitmap->Gc().SetFaded(EFalse);
	iCheckBitmap->Gc().BitBlt(TPoint(),&iBackgroundBitmap->Bitmap());

	//.. Set the grade of transperency
	iTransparencyBitmap->Gc().SetBrushColor(TRgb::Gray256(128));
	iTransparencyBitmap->Gc().SetBrushStyle(CGraphicsContext::ESolidBrush);
	iTransparencyBitmap->Gc().SetPenStyle(CGraphicsContext::ENullPen);
	iTransparencyBitmap->Gc().DrawRect(iWinSize);

	//.. Then draw to the fore ground bitmap and invalidate the second window
	iForegroundBitmap->Gc().Reset();
	iForegroundBitmap->Gc().SetPenStyle(CGraphicsContext::ESolidPen);
	iForegroundBitmap->Gc().SetPenSize(TSize(1,1));
	iForegroundBitmap->Gc().SetPenColor(TRgb(0,0,0));
	iForegroundBitmap->Gc().DrawLine(TPoint(0,0),TPoint(iWinSize.iWidth,iWinSize.iHeight));
	iForegroundBitmap->Gc().DrawLine(TPoint(iWinSize.iWidth,0),TPoint(0,iWinSize.iHeight));
	iTransWin->BaseWin()->SetVisible(ETrue);
	iTransWin->Invalidate();
	iTransWin->DrawNow();

	iCheckBitmap->Gc().AlphaBlendBitmaps(TPoint(0,0),&iForegroundBitmap->Bitmap(),&iCheckBitmap->Bitmap()
													,TRect(iWinSize),TPoint(0,0),&iTransparencyBitmap->Bitmap(),TPoint(0,0));
	iCheckWin->BaseWin()->SetVisible(ETrue);
	iCheckWin->BackedUpWin()->UpdateScreen();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	CheckRect(iTransWin,iCheckWin,TRect(iWinSize),_L("DrawTransparentWindows() CheckRect failed"));
	}

// !!! THE COMMENT BELOW IS DISABLED BECAUSE TransparentTests() METHOD IS NOT USED !!!
//**
//@SYMTestCaseID		GRAPHICS-WSERV-0096
//
//@SYMDEF             DEF081259
//
//@SYMTestCaseDesc    TransparentTests
//					REQUIREMENT: PREQ673 (Screen Scaling)
//					API: CWindowGc::AlphaBlendBitmaps()
//
//@SYMTestPriority    High
//
//@SYMTestStatus      Implemented
//
//@SYMTestActions     The main logic behind this test is to copy the content of the background window
//					bitmap to checkwindow bitmap and then use the foreground window bitmap and transparency bitmap
//					with the function AlphaBlendBitmaps() to get the content present on the combination of 
//					foreground(transparent) window and background window. Then compare both the regions of the 
//					window to check whether the content is same.
//
//@SYMTestExpectedResults The content of both halves of the screen should match.
//*/		
void CTScreenModeScaling::TransparentTestsL()
	{
	TInt flushState=TheClient->iWs.SetAutoFlush(ETrue);
	TInt defaultMode=TheClient->iScreenModes[0];
	TheClient->iScreen->SetAppScreenMode(defaultMode);
	TheClient->iScreen->SetScreenMode(defaultMode);
	CTClient* client=new(ELeave) CTClient();
	CleanupStack::PushL(client);
	client->SetScreenNumber(iTest->iScreenNumber);
	client->ConstructL();
	client->iGroup=new(ELeave) TestWindowGroup(client);
	client->iGroup->ConstructL();
	client->iGroup->WinTreeNode()->SetOrdinalPosition(1);
	RBlankWindow color256(client->iWs);
	User::LeaveIfError(color256.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	CleanupClosePushL(color256);
	color256.SetRequiredDisplayMode(EColor256);
	color256.Activate();
	client->Flush();
	DrawTransparentWindows();
	iCurrentMode=TheClient->iScreenModes[TheClient->iScreenModes.Count()-1];
	TheClient->iScreen->SetAppScreenMode(iCurrentMode);
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	TSizeMode storeModeData=TheClient->iScreen->GetCurrentScreenModeAttributes();
	TSizeMode testMode=storeModeData;
	for (TInt xScale=1;xScale<4;xScale++)
		{
		for (TInt yScale=1;yScale<4;yScale++)
			{
			testMode.iScreenScale=TSize(xScale,yScale);
			TestDifferentOriginAndScaleForTranspWin(testMode,TPoint(20,20));
			TestDifferentOriginAndScaleForTranspWin(testMode,TPoint(20,30));
			TestDifferentOriginAndScaleForTranspWin(testMode,TPoint(30,20));
			TestDifferentOriginAndScaleForTranspWin(testMode,TPoint(FullScreenModeSize.iWidth/2+1,60));
			}
		}
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	TheClient->iScreen->SetCurrentScreenModeAttributes(storeModeData);
	client->iScreen->SetAppScreenMode(iCurrentMode);
	client->Flush();
	TheClient->iScreen->SetAppScreenMode(defaultMode);
	TheClient->iScreen->SetScreenMode(defaultMode);
	TheClient->iWs.SetAutoFlush(flushState);
	CleanupStack::PopAndDestroy(2,client);
	}

void CTScreenModeScaling::TestDifferentOriginAndScaleForTranspWin(TSizeMode &aMode,TPoint aOrigin)
	{
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	aMode.iOrigin=aOrigin;
	TheClient->iScreen->SetCurrentScreenModeAttributes(aMode);
	TheClient->iScreen->SetAppScreenMode(iCurrentMode);
	TheClient->iScreen->SetScreenMode(iCurrentMode);
	TRAPD(ret,DrawTransparentWindows());
	TEST(ret==KErrNone);
	if (ret!=KErrNone)
		INFO_PRINTF3(_L("DrawTransparentWindows() return value  - Expected: %d, Actual: %d"), KErrNone, ret);		

	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0097

@SYMDEF             DEF081259

@SYMTestCaseDesc    AppScreenModeTest
					REQUIREMENT: PREQ673 (Screen Scaling)
					API: CWsScreenDevice::SetAppScreenMode()

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Enable the visibility of Test window, Call SetAppScreenMode() API with different
					screen mode. Check the variable which is set in its draw function. 

@SYMTestExpectedResults The variable should not be set when the application screen mode is different 
					then current screen mode. 
*/
void CTScreenModeScaling::AppScreenModeTestL()
	{
	TheClient->iScreen->SetAppScreenMode(0);
	TheClient->iScreen->SetScreenMode(0);
	iTestWin->iDrawn=EFalse;
	iTestWin->SetExtL(TPoint(),TSize(FullScreenModeSize.iWidth/2,FullScreenModeSize.iHeight));
	iTestWin->SetVisible(ETrue);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TEST(iTestWin->iDrawn);
	if (!iTestWin->iDrawn)
		INFO_PRINTF3(_L("iTestWin->iDrawn  - Expected: %d, Actual: %d"), ETrue, iTestWin->iDrawn);

	iTestWin->iDrawn=EFalse;
	TheClient->iScreen->SetAppScreenMode(2);
	TheClient->iScreen->SetScreenMode(0);
	iTestWin->Invalidate();
	TheClient->Flush();
	TEST(!iTestWin->iDrawn);
	if (iTestWin->iDrawn)
		INFO_PRINTF3(_L("iTestWin->iDrawn  - Expected: %d, Actual: %d"), EFalse, iTestWin->iDrawn);

	TheClient->iScreen->SetAppScreenMode(0);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TEST(iTestWin->iDrawn);
	if (!iTestWin->iDrawn)
		INFO_PRINTF3(_L("iTestWin->iDrawn  - Expected: %d, Actual: %d"), ETrue, iTestWin->iDrawn);

	iTestWin->iDrawn=EFalse;
	TheClient->iScreen->SetScreenMode(1);
	iTestWin->Invalidate();
	TheClient->Flush();
	TEST(!iTestWin->iDrawn);
	if (iTestWin->iDrawn)
		INFO_PRINTF3(_L("iTestWin->iDrawn  - Expected: %d, Actual: %d"), EFalse, iTestWin->iDrawn);

	iTestWin->SetVisible(EFalse);
	TheClient->iScreen->SetAppScreenMode(0);
	TheClient->iScreen->SetScreenMode(0);
	}

void CTScreenModeScaling::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KWindowTests,"Window Tests");
	_LIT(KSpriteTest,"Sprite Test");
	_LIT(KRotationTests,"Rotation Tests");
	_LIT(KTransparentTests,"Transparent Tests");
	_LIT(KAppScreenModeTest,"AppScreenMode Test");
	_LIT(KScalling,"Scaling not Supported");
	_LIT(KModes,"Only one Screen Size Mode");
	
	((CTScreenModeScalingStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
		case 1:
			{
			((CTScreenModeScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0097"));
			TInt numScrModes=TheClient->iScreen->NumScreenModes();
			if (numScrModes<2 || !CheckScalingSupportedOrNot())
				{
				if (numScrModes<2)
					LOG_MESSAGE(KModes);
				else
					LOG_MESSAGE(KScalling);
				TestComplete();
				return;
				}
			iTest->LogSubTest(KAppScreenModeTest);
			AppScreenModeTestL();
			}
			break;
		case 2:
			((CTScreenModeScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0093"));
			iTest->LogSubTest(KWindowTests);
			WindowTestsL();
			break;
		case 3:
			((CTScreenModeScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0094"));
			iTest->LogSubTest(KSpriteTest);
			SpriteTestL();
			break;
		case 4:
			((CTScreenModeScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0095"));
			iTest->LogSubTest(KRotationTests);
			RotationTestsL();
			break;
		case 5:
			((CTScreenModeScalingStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			iTest->LogSubTest(KTransparentTests);
			//TransparentTestsL();		//This tests currently fails sometimes for reasons not understood ####
			break;
		default:
			{
			TInt defaultMode=TheClient->iScreenModes[0];
			TheClient->iScreen->SetAppScreenMode(defaultMode);
			TheClient->iScreen->SetScreenMode(defaultMode);
			}
			((CTScreenModeScalingStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTScreenModeScalingStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			return;
		}
	((CTScreenModeScalingStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(ScreenModeScaling)
