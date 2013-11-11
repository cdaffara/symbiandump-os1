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
// Test screen mode switching
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TSCRMODE.H"

const TInt KBlankWinPosX=10;
const TInt KBlankWinPosY=20;
const TInt KBlankWinSizeX=30;
const TInt KBlankWinSizeY=25;

const TInt KScreenMode0=0;
const TInt KScreenMode1=1;
_LIT(KScreenModeDisplayMode,"GetScreenModeDisplayMode");


CTScrMode::CTScrMode(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}

//
// Static func to check windows
//

LOCAL_C TInt DoPanicTest(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	CWsScreenDevice *screen=NULL;
	if (ws.Connect()==KErrNone)
		{
		screen = new (ELeave) CWsScreenDevice(ws);
		if (screen && screen->Construct((TInt)aScreenNumber)==KErrNone)
			{
			TPixelsTwipsAndRotation sar;
			CFbsBitGc::TGraphicsOrientation rot=CFbsBitGc::EGraphicsOrientationNormal;
			TInt mode=0;
			switch(aInt)
				{
				case 0:
				case 1:
					screen->SetScreenMode(aInt==0?1000:-1);
					break;
				case 2:
				case 3:
					screen->GetScreenModeSizeAndRotation(aInt==2?screen->NumScreenModes():-100000, sar);
					break;
				case 4:
				case 5:
					screen->GetScreenModeSizeAndRotation(aInt==4?screen->NumScreenModes()+100:-10, sar);
					break;
				case 6:
					mode=screen->NumScreenModes();
					goto SetRot;
				case 7:
					mode=-55;
					goto SetRot;
				case 8:
					mode=234;
					goto SetRot;
				case 9:
					mode=screen->NumScreenModes()+2;
					rot=CFbsBitGc::EGraphicsOrientationRotated270;
				SetRot:
					screen->SetCurrentRotations(mode,rot);
					break;
				case 10:
				case 11:
					ws.SetPointerCursorArea(aInt==10?1000:-1,TRect());
					break;
				case 12:
				case 13:
					ws.PointerCursorArea(aInt==12?1003:-2);
					break;
				case 14:
				case 15:
					{
					CArrayFixFlat<TInt> *rotations=new(ELeave) CArrayFixFlat<TInt>(1);
					screen->GetRotationsList(aInt==12?1003:-2,rotations);
					}
					break;
				case 100:
					rot=CFbsBitGc::EGraphicsOrientationRotated90;
					goto SetRot;
				case 101:
					rot=CFbsBitGc::EGraphicsOrientationRotated270;
					goto SetRot;
				case 102:
					mode=1;
					rot=CFbsBitGc::EGraphicsOrientationRotated270;
					goto SetRot;
				case 103:
					mode=1;
				#if defined(__EPOC32__)
					rot=CFbsBitGc::EGraphicsOrientationRotated180;
				#else
					rot=CFbsBitGc::EGraphicsOrientationNormal;
				#endif
					goto SetRot;
				}
			ws.Flush();
			}
		}

	return(EWsExitReasonBad);
	}

void SetupTestRWindowLC(RWindow& aWindow, const TPoint& aPos, const TSize& aSize, TDisplayMode aDisplayMode)
	{
	CleanupClosePushL(aWindow);
	User::LeaveIfError(aWindow.Construct(*TheClient->iGroup->GroupWin(), (TUint)&aWindow));
	aWindow.SetExtent(aPos, aSize);
	User::LeaveIfError(aWindow.SetRequiredDisplayMode(aDisplayMode));
	aWindow.Activate();
	}

void CTScrMode::TestPanicsL()
	{
	TInt ii;
	for (ii=0;ii<16;++ii)
		TEST(iTest->TestWsPanicL(DoPanicTest,EWservPanicScreenModeNumber,ii,(TAny*)iTest->iScreenNumber));
	for (ii=100;ii<104;++ii)
		TEST(iTest->TestWsPanicL(DoPanicTest,EWservPanicRotation,ii,(TAny*)iTest->iScreenNumber));
	iTest->CloseAllPanicWindows();
	}

CTScrMode::~CTScrMode()
	{
	((CTScrModeStep*)iStep)->CloseTMSGraphicsStep();
	delete iSecondConnection;
	TheClient->iGroup->GroupWin()->DisableScreenChangeEvents();
	SetScreenModeEnforcement(iOldEnfMode);
	}

void CTScrMode::ConstructL()
	{
	//Remove next line when bitmaps are stored correctly
	TestWin->Win()->EnableRedrawStore(EFalse);
	BaseWin->Win()->EnableRedrawStore(EFalse);
//
	TheClient->iGroup->GroupWin()->EnableScreenChangeEvents();
//
	//TheClient->iWs.SetAutoFlush(ETrue);
	ValidateWin(BaseWin,TRgb::Gray256(204));
	ValidateWin(TestWin,TRgb::Gray256(204));
//
	iWinState=0;
	iWinPos=TPoint(2*TheClient->iGroup->Size().iWidth/3,0);
//
	iSecondConnection=new(ELeave) CSecondConnection;
	iSecondConnection->ConstructL(iTest->iScreenNumber, TRect(KBlankWinPosX,KBlankWinPosY,KBlankWinPosX+KBlankWinSizeX,KBlankWinPosY+KBlankWinSizeY),iTest,iStep);
//
	iOldEnfMode=TheClient->iScreen->ScreenModeEnforcement();
	if (TheClient->iScreenModes.Count()<2)
		{
		iTest->iState=1000;	// Miss all tests
		_LIT(KLog,"Skipping all tests as less that 2 screen size modes");
		LOG_MESSAGE(KLog);
		}
	else
		{
		SetScreenModeEnforcement(ESizeEnforcementNone);
		TheClient->iScreen->SetScreenMode(TheClient->iScreenModes[0]);
		}
	}

void CTScrMode::SetScreenModeEnforcement(TScreenModeEnforcement aMode)
	{
	iCurEnforcement=aMode;
	TheClient->iScreen->SetScreenModeEnforcement(iCurEnforcement);
	}

void CTScrMode::ScaledDrawingL()
	{
	const TInt KLineXPos=10;
	const TInt KLineYPos=30;
	const TInt KLineLength=10;
	const TInt KXScaleFactor=4;
	const TInt KYScaleFactor=3;
//
	// Draw rectangle onto TestWin
 	CWindowGc *gc=TheClient->iGc;
	gc->Activate(*(TestWin->Win()));
	gc->SetBrushColor(KRgbBlack);
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->DrawRect(TRect(TPoint(KLineXPos,KLineYPos),TSize(KLineLength*KXScaleFactor,KYScaleFactor)));
	gc->Deactivate();
//
	// Draw scaled bitmap onto BaseWin
	TSize bitSize(KLineLength,1);
	TSize bitTwipSize(TheClient->iScreen->HorizontalPixelsToTwips(bitSize.iWidth*KXScaleFactor),
					  TheClient->iScreen->VerticalPixelsToTwips(bitSize.iHeight*KYScaleFactor));
	CFbsBitGc *bitGc=NULL;
	CFbsBitmapDevice *device=NULL;
	CFbsBitmap *bitmap=NULL;
	bitmap=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(bitSize,EGray16));
	bitmap->SetSizeInTwips(bitTwipSize);
	device=CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(device);
	User::LeaveIfError(device->CreateContext(bitGc));
	bitGc->SetBrushColor(KRgbBlack);
	bitGc->Clear();
	delete bitGc;
//
 	gc->Activate(*(BaseWin->Win()));
	gc->DrawBitmap(TPoint(KLineXPos,KLineYPos),bitmap);
	gc->Deactivate();
//
	TheClient->iWs.Flush();

	CleanupStack::PopAndDestroy(2);	// bitmap,device

	CompareWindows(_L("CTScrMode::ScaledDrawingL() CompareWindows() failed"));
	}

void CTScrMode::ValidateWin(TestWindow *aWin, TRgb aColor)
	{
	aWin->Win()->Invalidate();
	RedrawWin(*aWin->Win(),aColor);
	}

void CTScrMode::RedrawWin(RWindow &aWin, TRgb aColor)
	{
	aWin.BeginRedraw();
	TheClient->iGc->Activate(aWin);
	TheClient->iGc->SetBrushColor(aColor);
	TheClient->iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheClient->iGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheClient->iGc->Clear();
	TheClient->iGc->Deactivate();
	aWin.EndRedraw();
	}

void CTScrMode::CheckWindows(TBool aWinIsVis)
	{
	iSecondConnection->SetWindow2Visibility(aWinIsVis);
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();
	CompareWindows(_L("CTScrMode::CheckWindows() CompareWindows() failed"));
	}

void CTScrMode::ScreenRotationsL()
	{
	CWsScreenDevice *screen=TheClient->iScreen;
	CArrayFixFlat<TInt> *rotations=new(ELeave) CArrayFixFlat<TInt>(1);
	CFbsBitGc::TGraphicsOrientation currentRotation;
	TPixelsAndRotation sizeAndRotation;
	TPixelsTwipsAndRotation sizeAndRotation2;
	TBool found;
	TInt numModes=TheClient->iScreenModes.Count();
	TInt count;
	TInt ii,jj;
	CleanupStack::PushL(rotations);
	for (ii=0;ii<numModes;++ii)
		{
		TInt mode=TheClient->iScreenModes[ii];
		User::LeaveIfError(screen->GetRotationsList(mode,rotations));
		screen->GetScreenModeSizeAndRotation(mode,sizeAndRotation);
		screen->GetScreenModeSizeAndRotation(mode,sizeAndRotation2);
		TEST(sizeAndRotation.iPixelSize.iWidth==sizeAndRotation2.iPixelSize.iWidth);
		if (sizeAndRotation.iPixelSize.iWidth!=sizeAndRotation2.iPixelSize.iWidth)
			INFO_PRINTF3(_L("sizeAndRotation.iPixelSize.iWidth==sizeAndRotation2.iPixelSize.iWidth  - Expected: %d, Actual: %d"), sizeAndRotation.iPixelSize.iWidth, sizeAndRotation2.iPixelSize.iWidth);

		TEST(sizeAndRotation.iPixelSize.iHeight==sizeAndRotation2.iPixelSize.iHeight);
		if (sizeAndRotation.iPixelSize.iHeight!=sizeAndRotation2.iPixelSize.iHeight)
			INFO_PRINTF3(_L("sizeAndRotation.iPixelSize.iHeight==sizeAndRotation2.iPixelSize.iHeight  - Expected: %d, Actual: %d"), sizeAndRotation.iPixelSize.iHeight, sizeAndRotation2.iPixelSize.iHeight);

		count=rotations->Count();
		TEST(count>=1);
		if (count<1)
			INFO_PRINTF3(_L("rotations->Count() return value  - Expected: %d or more, Actual: %d"), 1, count);

		TBool retVal;

		if (sizeAndRotation.iPixelSize.iWidth!=sizeAndRotation.iPixelSize.iHeight)
			{
			TEST(count<=2);
			if (count>2)
				INFO_PRINTF3(_L("rotations->Count() return value  - Expected: %d or less, Actual: %d"), 2, count);

			if (count==2)
				{
				retVal = (*rotations)[0]+2==(*rotations)[1];
				TEST(retVal);		//Must only have rotations 180 degrees apart
				if (!retVal)
					INFO_PRINTF3(_L("(*rotations)[0]+2==(*rotations)[1]  - Expected: %d, Actual: %d"), ETrue, retVal);
				}
			}
		found=EFalse;
		for (jj=0;jj<count;++jj)
			{
			if ((*rotations)[jj]==sizeAndRotation.iRotation)
				{
				retVal = (*rotations)[jj]==sizeAndRotation2.iRotation;
				TEST(retVal);
				if (!retVal)
					INFO_PRINTF3(_L("(*rotations)[jj]==sizeAndRotation2.iRotation  - Expected: %d, Actual: %d"), ETrue, retVal);

				found=ETrue;
				break;
				}
			}
		TEST(found);
		if (!found)
				INFO_PRINTF3(_L("found  - Expected: %d, Actual: %d"), ETrue, found);

		if (count>1)
			{
			currentRotation=sizeAndRotation.iRotation;
			for (jj=0;jj<count;++jj)
				{
				screen->SetCurrentRotations(mode,REINTERPRET_CAST(CFbsBitGc::TGraphicsOrientation&,(*rotations)[jj]));
				screen->GetScreenModeSizeAndRotation(mode,sizeAndRotation);
				screen->GetScreenModeSizeAndRotation(mode,sizeAndRotation2);
				retVal = (*rotations)[jj]==sizeAndRotation.iRotation;
				TEST(retVal);
				if (!retVal)
					INFO_PRINTF3(_L("(*rotations)[jj]==sizeAndRotation.iRotation  - Expected: %d, Actual: %d"), ETrue, retVal);

				retVal = (*rotations)[jj]==sizeAndRotation2.iRotation;
				TEST(retVal);
				if (!retVal)
					INFO_PRINTF3(_L("(*rotations)[jj]==sizeAndRotation2.iRotation  - Expected: %d, Actual: %d"), ETrue, retVal);

				}
			screen->SetCurrentRotations(mode,REINTERPRET_CAST(CFbsBitGc::TGraphicsOrientation&,currentRotation));
			screen->GetScreenModeSizeAndRotation(mode,sizeAndRotation);
			screen->GetScreenModeSizeAndRotation(mode,sizeAndRotation2);

			retVal = currentRotation==sizeAndRotation.iRotation;
			TEST(retVal);
			if (!retVal)
				INFO_PRINTF3(_L("currentRotation==sizeAndRotation.iRotation  - Expected: %d, Actual: %d"), ETrue, retVal);

			retVal = currentRotation==sizeAndRotation2.iRotation;
			TEST(retVal);
			if (!retVal)
				INFO_PRINTF3(_L("currentRotation==sizeAndRotation2.iRotation  - Expected: %d, Actual: %d"), ETrue, retVal);
			}
		}
	CleanupStack::PopAndDestroy();
	}

void CTScrMode::MoreScreenRotationsL()
	{
	RWindow shield4Gray(TheClient->iWs);
	// The default display mode needs to be updated to EColor64K for Oghma integ
	SetupTestRWindowLC(shield4Gray,TPoint(),TSize(100000,100000),EColor64K);
//
	CWsScreenDevice *screen=TheClient->iScreen;
	//CFbsBitGc::TGraphicsOrientation currentRotation;
	TPixelsAndRotation sizeAndRotation;
	TInt currentRotation;
	TInt currentMode=FindCurrentMode();
	TInt numModes=TheClient->iScreenModes.Count();
	TInt currentScreenMode;
	TInt count;
	TInt ii,jj;
	CArrayFixFlat<TInt> *currentRotations=new(ELeave) CArrayFixFlat<TInt>(1);
	CleanupStack::PushL(currentRotations);
	currentRotations->ResizeL(numModes);
	CArrayFixFlat<TInt> *originalRotation=new(ELeave) CArrayFixFlat<TInt>(1);
	CleanupStack::PushL(originalRotation);
	originalRotation->ResizeL(numModes);
	CArrayFixFlat<TInt> *rotations=new(ELeave) CArrayFixFlat<TInt>(1);
	CleanupStack::PushL(rotations);
	for (ii=0;ii<numModes;++ii)
		{
		TInt mode=TheClient->iScreenModes[ii];
		screen->GetScreenModeSizeAndRotation(mode,sizeAndRotation);
		(*currentRotations)[ii]=sizeAndRotation.iRotation;
		(*originalRotation)[ii]=sizeAndRotation.iRotation;
		}
	for (ii=0;ii<numModes;++ii)
		{
		TInt mode=TheClient->iScreenModes[ii];
		screen->SetScreenMode(mode);
		currentScreenMode = screen->CurrentScreenMode();
		TEST(currentScreenMode == mode);
		if (currentScreenMode != mode)
			INFO_PRINTF3(_L("screen->CurrentScreenMode() return value - Expected: %d, Actual: %d"), mode, currentScreenMode);

		screen->GetDefaultScreenSizeAndRotation(sizeAndRotation);
		currentRotation=(*currentRotations)[ii];
		TEST(sizeAndRotation.iRotation==currentRotation);

		screen->GetScreenModeSizeAndRotation(mode,sizeAndRotation);
		TEST(sizeAndRotation.iRotation==currentRotation);

		User::LeaveIfError(screen->GetRotationsList(mode,rotations));
		count=rotations->Count();
		if (count>1)
			{
			for (jj=0;jj<count;)
				{
				if ((*rotations)[jj++]==currentRotation)
					break;
				}
			if (jj==count)
				jj=0;
			currentRotation=(*rotations)[jj];
			screen->SetCurrentRotations(mode,REINTERPRET_CAST(CFbsBitGc::TGraphicsOrientation&,currentRotation));
			(*currentRotations)[ii]=currentRotation;
			}
		}
	for (ii=0;ii<numModes;++ii)
		{
		TInt mode=TheClient->iScreenModes[ii];
		screen->SetScreenMode(mode);
		currentScreenMode = screen->CurrentScreenMode();
		TEST(currentScreenMode == mode);
		if (currentScreenMode != mode)
			INFO_PRINTF3(_L("(screen->CurrentScreenMode() return value - Expected: %d, Actual: %d"), mode, currentScreenMode);

		screen->GetDefaultScreenSizeAndRotation(sizeAndRotation);
		currentRotation=(*currentRotations)[ii];
		TEST(sizeAndRotation.iRotation==currentRotation);
		if (sizeAndRotation.iRotation != currentRotation)
			INFO_PRINTF3(_L("(screen->GetDefaultScreenSizeAndRotation() return value - Expected: %d, Actual: %d"), currentRotation, sizeAndRotation.iRotation);

		screen->GetScreenModeSizeAndRotation(mode,sizeAndRotation);
		TEST(sizeAndRotation.iRotation==currentRotation);
		if (sizeAndRotation.iRotation != currentRotation)
			INFO_PRINTF3(_L("(screen->GetDefaultScreenSizeAndRotation() return value - Expected: %d, Actual: %d"), currentRotation, sizeAndRotation.iRotation);

		if (currentRotation!=(*originalRotation)[ii])
			screen->SetCurrentRotations(mode,REINTERPRET_CAST(CFbsBitGc::TGraphicsOrientation&,(*originalRotation)[ii]));
		}
	screen->SetScreenMode(currentMode);
	currentScreenMode = screen->CurrentScreenMode();
	TEST(currentScreenMode == currentMode);
	if (currentScreenMode != currentMode)
		INFO_PRINTF3(_L("(screen->CurrentScreenMode() return value - Expected: %d, Actual: %d"), currentMode, currentScreenMode);

	CleanupStack::PopAndDestroy(4,&shield4Gray);
	}

TInt CTScrMode::FindCurrentMode()
	{
	CWsScreenDevice *screen=TheClient->iScreen;
	TPixelsTwipsAndRotation sizeAndRotation;
	TPixelsTwipsAndRotation sizeAndRotation2;
	TInt numModes=TheClient->iScreenModes.Count();
	TInt ii;
	screen->GetDefaultScreenSizeAndRotation(sizeAndRotation);
	for (ii=0;ii<numModes;++ii)
		{
		TInt mode=TheClient->iScreenModes[ii];
		screen->GetScreenModeSizeAndRotation(mode,sizeAndRotation2);
		if (Equal(sizeAndRotation,sizeAndRotation2))
			return mode;
		}
	TEST(EFalse);
	return -1;
	}

TBool CTScrMode::Equal(const TPixelsTwipsAndRotation& aLeft,const TPixelsTwipsAndRotation& aRight)
	{
	if (aLeft.iPixelSize!=aRight.iPixelSize)
		return EFalse;
	if (aLeft.iRotation!=aRight.iRotation)
		return EFalse;
	if (aLeft.iTwipsSize!=aRight.iTwipsSize)
		return EFalse;
	return ETrue;
	}

TBool CTScrMode::RectClearBugL()
	{
	CWsScreenDevice *screen=TheClient->iScreen;
	TPixelsTwipsAndRotation sizeAndRotation1;
	TPixelsTwipsAndRotation sizeAndRotation2;
	TInt screenMode=FindCurrentMode();
	TInt largeMode=0;
	TInt smallMode=1;
	INFO_PRINTF1(_L("Rotation 1"));
	screen->GetScreenModeSizeAndRotation(0,sizeAndRotation1);
	INFO_PRINTF1(_L("Rotation 2"));
	screen->GetScreenModeSizeAndRotation(1,sizeAndRotation2);
	TSize winSize=TSize(Max(sizeAndRotation1.iPixelSize.iWidth,sizeAndRotation2.iPixelSize.iWidth)
						,Max(sizeAndRotation1.iPixelSize.iHeight,sizeAndRotation2.iPixelSize.iHeight));
	if (sizeAndRotation1.iPixelSize.iWidth<=sizeAndRotation2.iPixelSize.iWidth
									&& sizeAndRotation1.iPixelSize.iHeight<=sizeAndRotation2.iPixelSize.iHeight)
		{
		largeMode=1;
		smallMode=0;
		}
	if (screenMode!=largeMode)
	{
		INFO_PRINTF2(_L("Large Mode %d\r\n"),largeMode);
		screen->SetScreenMode(largeMode);
	}
	CBlankWindow *blankWin=new(ELeave) CBlankWindow(TRgb::Gray256(204));
	CleanupStack::PushL(blankWin);
	blankWin->SetUpL(TPoint(),winSize,TheClient->iGroup,*TheClient->iGc);
	TheClient->iGc->Activate(*blankWin->Win());		//blankWin->Win()->Invalidate();
	blankWin->Draw();
	TheClient->iWs.Flush();
	blankWin->Win()->SetRequiredDisplayMode(EGray4);
	INFO_PRINTF2(_L("Small Mode %d\r\n"),smallMode);
	screen->SetScreenMode(smallMode);
	TheClient->iWs.Flush();
	TheClient->iGc->Deactivate();
	CleanupStack::PopAndDestroy();
	if (screenMode!=smallMode)
	{
		INFO_PRINTF2(_L("Screen Mode %d\r\n"),screenMode);
		screen->SetScreenMode(screenMode);
	}

	return ETrue;
	}

void CTScrMode::ScreenModeChange1L()
	{
	CTWin* color64win;
	color64win=new(ELeave) CTWin();
	CleanupStack::PushL(color64win);
	color64win->ConstructL(*TheClient->iGroup);
	color64win->BaseWin()->SetRequiredDisplayMode(EColor64K);
	//color64win->SetBackgroundColor(TRgb(0,0,255));
	color64win->Win()->SetBackgroundColor();
	color64win->Activate();
	TheClient->Flush();
	CTWin* color4win;
	color4win=new(ELeave) CTWin();
	CleanupStack::PushL(color4win);
	color4win->ConstructL(*TheClient->iGroup);
	color4win->BaseWin()->SetRequiredDisplayMode(EColor4K);
	//color4win->SetBackgroundColor(TRgb(255,0,0));
	color4win->Win()->SetBackgroundColor();
	color4win->SetSize(TSize(600,200));
	color4win->Activate();
	TheClient->Flush();
	color4win->SetSize(TSize(640,240));
	TheClient->Flush();
	color64win->SetSize(TSize(20,20));
	color64win->WinTreeNode()->SetOrdinalPosition(0);
	TheClient->Flush();
	CleanupStack::Pop(2,color64win);
	delete color64win;
	TheClient->Flush();
	delete color4win;
	//CleanupStack::PopAndDestroy(2,color64win);
	}

void CTScrMode::ScreenModeChange2L()
	{
	CTBlankWindow* color64win;
	color64win=new(ELeave) CTBlankWindow();
	CleanupStack::PushL(color64win);
	color64win->ConstructL(*TheClient->iGroup);
	color64win->BaseWin()->SetRequiredDisplayMode(EColor64K);
	color64win->SetColor(TRgb(0,0,255));
	//color64win->SetColor();
	color64win->Activate();
	TheClient->Flush();
	CTBlankWindow* color4win;
	color4win=new(ELeave) CTBlankWindow();
	CleanupStack::PushL(color4win);
	color4win->ConstructL(*TheClient->iGroup);
	color4win->BaseWin()->SetRequiredDisplayMode(EColor4K);
	color4win->SetColor(TRgb(255,0,0));
	//color4win->SetColor();
	color4win->SetSize(TSize(600,200));
	color4win->Activate();
	TheClient->Flush();
	color4win->SetSize(TSize(640,240));
	TheClient->Flush();
	color64win->SetSize(TSize(20,20));
	color64win->WinTreeNode()->SetOrdinalPosition(0);
	TheClient->Flush();
	CleanupStack::Pop(2,color64win);
	delete color64win;
	TheClient->Flush();
	delete color4win;
	//CleanupStack::PopAndDestroy(2,color64win);
	}

LOCAL_C void ResetAndDestroyWindows(TAny* aPointerArray)
	{
	static_cast<RPointerArray<CBlankWindow>*>(aPointerArray)->ResetAndDestroy();
	}

LOCAL_C TBool WalkThroughTree(RPointerArray<CBlankWindow> aListOfWindows)
	{
	TBool ret=ETrue;
	TInt count = aListOfWindows.Count();
	while (--count>=0 && ret)
		{
		RRegion region;
		((aListOfWindows)[count])->Win()->GetInvalidRegion(region);
		ret=region.IsEmpty();
		}
	return ret;
	}

void CTScrMode::InvisibleWndAndInfiniteRedrawBugL()
	{
	// Current Test client state
	CTClient* clientA=TheClient;
	CWsScreenDevice* screenClientA=clientA->iScreen;
	const TScreenModeEnforcement screenModeEnforcement = screenClientA->ScreenModeEnforcement();
	const TInt currentScreenMode=FindCurrentMode();
	TPixelsTwipsAndRotation sizeAndRotation;
	screenClientA->GetScreenModeSizeAndRotation(currentScreenMode,sizeAndRotation);


	SecondClientConnection* clientB = new(ELeave) SecondClientConnection();
	CleanupStack::PushL(clientB);
	clientB->SetScreenNumber(iTest->iScreenNumber);
	clientB->ConstructL(currentScreenMode, sizeAndRotation);

// Start test
	CWindowGc& gc=*clientB->iGc;
	RWsSession ws=clientB->iWs;
	CTWindowGroup* group=clientB->iGroup;

	const TInt shrinkFactor = 5;
	TRect parentRect(0,0, sizeAndRotation.iPixelSize.iWidth, sizeAndRotation.iPixelSize.iHeight);
	RPointerArray<CBlankWindow> windows;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyWindows, &windows));
	// create several children
	const TInt count = (iTest->iTestLevel==iTest->ELevelQuick ? 5 : 7);
	CBlankWindow* blankWin=NULL;
	CTWinBase* parent=group;
	for (TInt childNum=0; childNum<count; ++childNum)
		{
		blankWin=new(ELeave) CBlankWindow();
		CleanupStack::PushL(blankWin);
		User::LeaveIfError(windows.Append(blankWin));
		CleanupStack::Pop(blankWin);
		blankWin->SetUpL(parentRect.iTl, parentRect.Size(), parent, gc);
		// make it visible and draw it
		blankWin->Win()->Invalidate();
		gc.Activate(*blankWin->Win());
		blankWin->Draw();
		ws.Flush();
		gc.Deactivate();
		// prepare for next window
		parent=blankWin;
		parentRect.Shrink(shrinkFactor, shrinkFactor);
		}

	// change screen mode and check visibility of all windows

	const TInt newScreenMode = (currentScreenMode==KScreenMode0?KScreenMode1:KScreenMode0);
	TPixelsTwipsAndRotation sizeAndRotationForDifferentScreenMode;
	screenClientA->GetScreenModeSizeAndRotation(newScreenMode, sizeAndRotationForDifferentScreenMode);
	screenClientA->SetScreenSizeAndRotation(sizeAndRotationForDifferentScreenMode);
	screenClientA->SetScreenMode(newScreenMode);

	TheClient->WaitForRedrawsToFinish();
	TBool retVal = WalkThroughTree(windows);
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("WalkThroughTree(windows) return value - Expected: %d, Actual: %d"), ETrue, retVal);


	screenClientA->SetScreenModeEnforcement(screenModeEnforcement);
	screenClientA->SetScreenMode(currentScreenMode);
	screenClientA->SetScreenSizeAndRotation(sizeAndRotation);

	// destroy all windows
	CleanupStack::PopAndDestroy(2, clientB); // ResetAndDestroyWindows and client
	// Needed to get system back into a good state as this test can leave the shell in front of the test app
	TheClient->iGroup->GroupWin()->SetOrdinalPosition(0);
	}

//To test the APIs GetScreenModeDisplayMode() & GetDefModeMaxNumColors()
void CTScrMode::GetScreenDisplayMode()
	{
	TInt color,gray;
	CWsScreenDevice *screen=TheClient->iScreen;
	TInt currentScreenMode=screen->CurrentScreenMode(); //finding the current screen mode
	screen->SetScreenMode(KScreenMode1); //changing the current screen mode to 1

	//testing the display mode of Screen Mode using the API GetScreenModeDisplayMode()
	TDisplayMode displayMode=screen->GetScreenModeDisplayMode(KScreenMode1);

	TEST(displayMode!=ENone);
	TEST(displayMode!=ERgb);
	//testing the default mode of Screen Mode using the API GetDefModeMaxNumColors()
	TDisplayMode defaultMode=TheClient->iWs.GetDefModeMaxNumColors(color,gray);
	TDisplayMode defaultModeForScreen=TheClient->iWs.GetDefModeMaxNumColors(screen->GetScreenNumber(),color,gray);
	TEST(defaultMode==defaultModeForScreen);
	TEST(defaultMode!=ENone);
	TEST(defaultMode!=ERgb);
	TEST(color == 16777216 || color == 0 || color == 16 || color == 256  || color == 4096 || color == 65536);
	TEST(gray == 0 || gray == 2 || gray == 4  || gray == 16  || gray == 256);
  	screen->SetScreenMode(currentScreenMode);
  	
	CArrayFixFlat<TInt>* modeList=new CArrayFixFlat<TInt>(15);
	TEST(modeList != NULL);
	if(!modeList)
		{
		return;
		}
	TInt res = TheClient->iWs.GetColorModeList(modeList);
	TEST(res == KErrNone);
	TDisplayMode modeMax = ENone;
	for(TInt index = 0; index < modeList->Count(); index++)
		{
		TDisplayMode mode = (TDisplayMode) ((*modeList)[index]);
		if(mode > modeMax)
			{
			modeMax = mode;
			}
		}
	modeList->Reset();
	delete modeList;
	TInt realColor = 0;
	TInt realGray = 0;
	switch(modeMax)
		{
	case EGray2:
		realColor = 2;
		realGray = 2;
		break;
	case EGray4:
		realColor = 4;
		realGray = 4;
		break;
	case EGray16:
		realColor = 16;
		realGray = 16;
		break;
	case EGray256:
		realColor = 256;
		realGray = 256;
		break;
	case EColor16:
		realColor = 16;
		break;
	case EColor256:
		realColor = 256;
		break;
	case EColor4K:
		realColor = 4096;
		break;
	case EColor64K:
		realColor = 65536;
		break;
	case EColor16M:
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		realColor = 16777216;
		break;
	default:
		break;
		}
 
 	if (realColor > 0)
 	 	TEST(realColor == color);
 	if (realGray > 0)
 	 	TEST(gray == realGray);	
	}

void CTScrMode::SetScreenModeAfterScreenDeviceDeletedL()
	{
	// A separate session is needed, because this test will delete the primary screen device
	RWsSession ws; 
	User::LeaveIfError(ws.Connect());

	// Push the secondary screen device first onto the cleanup stack
	// so that we can pop and destroy the primary screen device first
	CWsScreenDevice* secondaryScreenDevice = new (ELeave) CWsScreenDevice(ws);
	CleanupStack::PushL(secondaryScreenDevice);

	CWsScreenDevice* primaryScreenDevice = new (ELeave) CWsScreenDevice(ws);
	CleanupStack::PushL(primaryScreenDevice);

	// Construct the primary screen device first to ensure that it is
	// used by the group window
	User::LeaveIfError(primaryScreenDevice->Construct(iTest->iScreenNumber));
	User::LeaveIfError(secondaryScreenDevice->Construct(iTest->iScreenNumber));

	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(888));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close

	RArray<TInt> screenModes;
	primaryScreenDevice->GetScreenSizeModeList(&screenModes);
	primaryScreenDevice->SetScreenMode(screenModes[0]);

	// Prematurely destroy the primary screen device used by the group window
	CleanupStack::PopAndDestroy(primaryScreenDevice);

	// Simulate screen rotation - call SetScreenMode()
	// This would trigger the defective behaviour and wserv would panic if broken
	secondaryScreenDevice->SetScreenMode(screenModes[0]);

	// Tidy up
	screenModes.Close();
	group.Close();
	CleanupStack::PopAndDestroy(secondaryScreenDevice);
	ws.Flush();
	ws.Close();
	}
	
void CTScrMode::DrawTestBmpL(CFbsBitmap* aTestBitmap)
	{
	CFbsBitmapDevice* device=CFbsBitmapDevice::NewL(aTestBitmap);
	CleanupStack::PushL(device);
	CGraphicsContext* bmpgc;
	User::LeaveIfError(device->CreateContext(bmpgc));
	CleanupStack::PushL(bmpgc);
	bmpgc->SetPenColor(KRgbDarkRed);
	bmpgc->SetBrushColor(KRgbYellow);
	bmpgc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bmpgc->SetPenStyle(CGraphicsContext::ESolidPen);
	bmpgc->DrawRect(TRect(aTestBitmap->SizeInPixels()));
	CleanupStack::PopAndDestroy(2,device);
	}

void CTScrMode::TestDrawingToWindows(CWindowGc* aWinGc1, CWindowGc* aWinGc2, RWindow& aWindow1,RWindow& aWindow2,CFbsBitmap* aBitmap64K,CFbsBitmap* aBitmap16M,CFbsFont* aFont, TBool aAllInRedraw, TBool aUseSystemGc, TBool aUseBmp16M)
	{
	CFbsBitmap* bitmap=aUseBmp16M?aBitmap16M:aBitmap64K;
	TInt ascent=aFont->AscentInPixels();
	for(TInt win=0;win<2;win++)
		{
		RWindow* winPtr=(win==0)?&aWindow1:&aWindow2;
		winPtr->Invalidate();
		winPtr->BeginRedraw();
		CWindowGc* winGc;
		if (aUseSystemGc)
			{
			winGc=TheClient->iGc;
			winGc->Activate(*winPtr);
			}
		else
			{
			winGc=(win==0)?aWinGc1:aWinGc2;
			winGc->Reset();
			}
		winGc->SetBrushColor(KRgbBlue);
		winGc->Clear();
		if (!aAllInRedraw)
			winPtr->EndRedraw();
//
		winGc->BitBlt(TPoint(0,0),bitmap);
		winGc->UseFont(aFont);
		_LIT(KTestText123,"Test text 123");
		winGc->DrawText(KTestText123,TPoint(0,ascent));
		if (aAllInRedraw)
			winPtr->EndRedraw();
		if (aUseSystemGc)
			winGc->Deactivate();
		}
	TBool winCheck=DoCheckRectRWin(aWindow1,aWindow2,TRect(aWindow1.Size()));
	if (!winCheck)
		{
		TBuf<256> errBuf(_L("Rot-mode test failed"));
		if (aAllInRedraw)
			errBuf.Append(_L(", All in Redraw"));
		if (aUseSystemGc)
			errBuf.Append(_L(", System Gc"));
		if (aUseBmp16M)
			errBuf.Append(_L(", 16M bmp"));
		else
			errBuf.Append(_L(", 64K bmp"));
		INFO_PRINTF1(errBuf);
		}
	TEST(winCheck);
	}

void SetWsAndAppScreenModes(TInt aMode)
	{
	TheClient->iScreen->SetScreenMode(aMode);
	TheClient->iScreen->SetAppScreenMode(aMode);
	}
	
TDisplayMode HighestMatch(CArrayFixFlat<TInt>* aModeList,TInt &aMaxBpp)
	{
	TDisplayMode highMode=ENone;
	TInt highBpp=0;
	for(TInt loop=aModeList->Count()-1;loop>=0;loop--)
		{
		TDisplayMode mode=(TDisplayMode)(*aModeList)[loop];
		TInt bpp;
		switch(mode)
			{
			case EGray2:
				bpp=1;
				break;
			case EGray4:
				bpp=2;
				break;
			case EGray16:
			case EColor16:
				bpp=4;
				break;
			case EColor256:
			case EGray256:
				bpp=8;
				break;
			case EColor64K:
				bpp=16;
				break;
			case EColor4K:
				bpp=12;
				break;
			case EColor16M:
			case EColor16MU:
			case EColor16MA:
			case EColor16MAP:
				bpp=32;
				break;
			default:
				bpp=0;
				break;
			}
		if (bpp>highBpp && bpp<aMaxBpp)
			{
			highBpp=bpp;
			highMode=mode;
			}
		}
	aMaxBpp=highBpp;
	return(highMode);
	}

void CTScrMode::TestRotateAndScreenModeL()
	{
	TheClient->iGroup->GroupWin()->SetOrdinalPosition(0);
// When switching colour modes Wserv takes into account the full device size, not the clipped size
// associated with any specific screen mode. Therefore any windows designed to influence the screen
// mode must use the appropriately rotated device base screen size, not any cutdown screen mode sizes.
	TSize baseScreenSize=TheClient->iScreen->SizeInPixels();
	TSize rotatedBaseSize(baseScreenSize.iHeight,baseScreenSize.iWidth);
	TInt maxScreenDimension=Max(baseScreenSize.iHeight,baseScreenSize.iWidth);
//
	CArrayFixFlat<TInt>* modeList=new(ELeave) CArrayFixFlat<TInt>(15);
	CleanupStack::PushL(modeList);
	User::LeaveIfError(TheClient->iWs.GetColorModeList(modeList));
	TInt maxBpp=KMaxTInt;
	TDisplayMode testDispModeHigh=HighestMatch(modeList,maxBpp);
	TDisplayMode testDispModeLow=HighestMatch(modeList,maxBpp);
	CleanupStack::PopAndDestroy(modeList);
// If only one available display mode skip tests
	if (testDispModeLow==ENone)
		return;
//
	CFbsFont* font;
	TFontSpec fspec;
	fspec.iHeight=240;
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont*&)font,fspec));
// Create a massive 16M window to make sure 16M is the fallback mode when any area of the screen is exposed
	RWindow shield16M(TheClient->iWs);
	SetupTestRWindowLC(shield16M,TPoint(),TSize(100000,100000),testDispModeHigh);
	RedrawWin(shield16M,KRgbBlack);
//
	TInt numModes=TheClient->iScreenModes.Count();
	for(TInt modeIndex=0;modeIndex<numModes;modeIndex++)
		{
		TInt screenMode=TheClient->iScreenModes[modeIndex];
		TPoint origin=TheClient->iScreen->GetScreenModeScaledOrigin(screenMode);
		if (origin.iX!=0 || origin.iY!=0)
			continue;
		TPixelsTwipsAndRotation sar;
		TheClient->iScreen->GetScreenModeSizeAndRotation(screenMode,sar);
		TSize screenSize=sar.iPixelSize;
		if (screenSize.iWidth>baseScreenSize.iWidth || screenSize.iHeight>baseScreenSize.iHeight)
			continue;
		TSize rotatedScreenDeviceSize(sar.iRotation==CFbsBitGc::EGraphicsOrientationRotated90 || sar.iRotation==CFbsBitGc::EGraphicsOrientationRotated270?
				rotatedBaseSize:baseScreenSize);
//
		TheClient->iScreen->SetScreenMode(screenMode);
//
		TSize screenSize1 = screenSize;
		screenSize1.iWidth = Min(screenSize1.iWidth, rotatedScreenDeviceSize.iWidth); //this is to guarantee we won't compare beyond screen size
		TSize testSize(screenSize1.iWidth/2,screenSize1.iHeight);
		TSize testBmpSize(testSize.iWidth,testSize.iHeight/4);
//
		CFbsBitmap* testBitmap64=new(ELeave) CFbsBitmap;
		CleanupStack::PushL(testBitmap64);
		User::LeaveIfError(testBitmap64->Create(testBmpSize,testDispModeLow));
		DrawTestBmpL(testBitmap64);
//
		CFbsBitmap* testBitmap16M=new(ELeave) CFbsBitmap;
		CleanupStack::PushL(testBitmap16M);
		User::LeaveIfError(testBitmap16M->Create(testBmpSize,testDispModeHigh));
		DrawTestBmpL(testBitmap16M);
//
		RWindow window(TheClient->iWs);
		SetupTestRWindowLC(window,TPoint(0,0), testSize,testDispModeHigh);
		RWindow window2(TheClient->iWs);
		SetupTestRWindowLC(window2,TPoint(testSize.iWidth,0),testSize,testDispModeHigh);
//
		CWindowGc* winGc1=new(ELeave) CWindowGc(TheClient->iScreen);
		CleanupStack::PushL(winGc1);
		User::LeaveIfError(winGc1->Construct());
		winGc1->Activate(window);
		CWindowGc* winGc2=new(ELeave) CWindowGc(TheClient->iScreen);
		CleanupStack::PushL(winGc2);
		User::LeaveIfError(winGc2->Construct());
		winGc2->Activate(window2);
//
		for(TInt modeIndex2=0;modeIndex2<numModes;modeIndex2++)
			{
			if (modeIndex==modeIndex2)
				continue;
			TInt screenMode2=TheClient->iScreenModes[modeIndex2];
			TPoint origin2=TheClient->iScreen->GetScreenModeScaledOrigin(screenMode2);
			if (origin2.iX!=0 || origin2.iY!=0)
				continue;
			TDisplayMode baseDisplayMode=TheClient->iScreen->DisplayMode();
			INFO_PRINTF5(_L("Testing with screen modes %d/%d, display modes %d/%d"),modeIndex,modeIndex2,testDispModeLow,testDispModeHigh);
			SetWsAndAppScreenModes(screenMode2);
			TDisplayMode dispMode11 = TheClient->iScreen->DisplayMode();
			TPixelsTwipsAndRotation sar1;
			TheClient->iScreen->GetScreenModeSizeAndRotation(screenMode2, sar1);
			if(sar1.iRotation == sar.iRotation)
				{
				continue;
				}
//
			RWindow window64K(TheClient->iWs);
			SetupTestRWindowLC(window64K,TPoint(),TSize(maxScreenDimension,maxScreenDimension),testDispModeLow);
			RedrawWin(window64K,KRgbGreen);
			// Should now have switched Wserv to 64K mode
			TEST(TheClient->iScreen->DisplayMode()==testDispModeLow);
//
			RWindow window16M(TheClient->iWs);
			SetupTestRWindowLC(window16M,TPoint(),TSize(maxScreenDimension/2,maxScreenDimension/2),testDispModeHigh);
			RedrawWin(window16M,KRgbCyan);
			// Should switch Wserv back to 16M mode
			TEST(TheClient->iScreen->DisplayMode()==testDispModeHigh);
//
			window64K.SetOrdinalPosition(-1);
			window16M.SetOrdinalPosition(-1);
			SetWsAndAppScreenModes(screenMode);
			TEST(TheClient->iScreen->DisplayMode()==baseDisplayMode);
//
			const TInt KNumTestFlags=3;
			const TInt KMaxTestFlags=1<<KNumTestFlags;
			for(TUint testFlags=0;testFlags<KMaxTestFlags;testFlags++)
				{
				TestDrawingToWindows(winGc1,winGc2,window,window2,testBitmap64,testBitmap16M,font,testFlags&0x1,testFlags&0x2,testFlags&0x4);
				}
// Test various functions change the display mode appropriately
			window64K.SetOrdinalPosition(0);
			TEST(TheClient->iScreen->DisplayMode()==testDispModeLow);
			window64K.SetVisible(EFalse);
			TEST(TheClient->iScreen->DisplayMode()==baseDisplayMode);
			window64K.SetVisible(ETrue);
			TEST(TheClient->iScreen->DisplayMode()==testDispModeLow);
			window64K.SetSize(rotatedScreenDeviceSize);
			TEST(TheClient->iScreen->DisplayMode()==testDispModeLow);
			// Changing screen mode now should expose the 16M window
			SetWsAndAppScreenModes(screenMode2);
			TEST(TheClient->iScreen->DisplayMode()==testDispModeHigh);
			SetWsAndAppScreenModes(screenMode);
			// And back to the 64K win covering the screen
			TEST(TheClient->iScreen->DisplayMode()==testDispModeLow);
// Create a new group with a 16M window and switching ordinal pos to back and back to front
			RWindowGroup testGroup(TheClient->iWs);
			CleanupClosePushL(testGroup);
			User::LeaveIfError(testGroup.Construct(111,EFalse));
			RWindow testGroupWin(TheClient->iWs);
			CleanupClosePushL(testGroupWin);
			User::LeaveIfError(testGroupWin.Construct(testGroup, 112));
			testGroupWin.SetExtent(TPoint(), TSize(1,1));	// Just enough to change mode
			testGroupWin.SetRequiredDisplayMode(testDispModeHigh);
			testGroupWin.Activate();
			TEST(TheClient->iScreen->DisplayMode()==testDispModeHigh);
			TInt oldPos=testGroup.OrdinalPosition();
			testGroup.SetOrdinalPosition(oldPos+1);
			TEST(TheClient->iScreen->DisplayMode()==testDispModeLow);
			testGroup.SetOrdinalPosition(oldPos);
			TEST(TheClient->iScreen->DisplayMode()==testDispModeHigh);
			CleanupStack::PopAndDestroy(2,&testGroup);
			TEST(TheClient->iScreen->DisplayMode()==testDispModeLow);
//
			CleanupStack::PopAndDestroy(2,&window64K);
			}
		CleanupStack::PopAndDestroy(2,winGc1);
		CleanupStack::PopAndDestroy(2,&window);
		CleanupStack::PopAndDestroy(2,testBitmap64);
		}
	CleanupStack::PopAndDestroy(&shield16M);
	TheClient->iScreen->ReleaseFont(font);
	SetWsAndAppScreenModes(TheClient->iScreenModes[0]);
	}

void CTScrMode::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	TBuf<32> buf;
	TBool checkWindowParam=EFalse;
	TInt count=0;
	TInt mode=0;
	TBool enable=EFalse;
	TBool disable=EFalse;
	TInt retVal;
	
	((CTScrModeStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**

  @SYMTestCaseID GRAPHICS-WSERV-0296

  @SYMDEF  			DEF081259

  @SYMTestCaseDesc Test screen modes can be set while the screen is rotated
  					through 180 degrees.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Rotate the screen through 180 degrees while setting each
  					of the screen modes available

  @SYMTestExpectedResults The screen is rotated and screen modes set correctly

*/
		case 1:
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0296"));
			iTest->LogSubTest(_L("Orientation1"));
			ScreenRotationsL();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0297

  @SYMDEF  			DEF081259

  @SYMTestCaseDesc Change the screen orientation then test screen modes can
  					be set while the screen is rotated through 180 degrees.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Rotate the screen through 180 degrees while setting each
  					of the screen modes available

  @SYMTestExpectedResults The screen is rotated and screen modes set correctly

*/
		case 2:
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0297"));
			iTest->LogSubTest(_L("Orientation2"));
			MoreScreenRotationsL();
			iSubState=0;
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0298

  @SYMDEF  			DEF081259

  @SYMTestCaseDesc Test for the rect clear defect while setting screen mode

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Change the screen mode and check the rect clear defect is
  					not present

  @SYMTestExpectedResults The defect is not present

*/
		case 3:
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0298"));
			iTest->LogSubTest(_L("Rect Clear Defect"));
			RectClearBugL();
			break;
		case 4:
/**

  @SYMTestCaseID GRAPHICS-WSERV-0299

  @SYMDEF  			DEF081259

  @SYMTestCaseDesc Test display mode can be set for test windows and
  					that the windows then function correctly

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Set the display mode in two test windows and then call
  					methods on the windows

  @SYMTestExpectedResults The windows function correctly

*/
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0299"));

			iTest->LogSubTest(_L("ScreenModeChange"));
			ScreenModeChange1L();
			ScreenModeChange2L();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0300

  @SYMDEF  			DEF081259

  @SYMTestCaseDesc Test display mode can be set and retrieved

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Set the display mode in a window and the retrieve it

  @SYMTestExpectedResults The display mode retrieved is the same as the
  							one set.

*/
		case 5:
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0300"));
			iTest->LogSubTest(KScreenModeDisplayMode);
			GetScreenDisplayMode();
			break;

	
#if defined(SCREEN_MODE_TESTING)
/**

  @SYMTestCaseID GRAPHICS-WSERV-0301

  @SYMDEF  			DEF081259

  @SYMTestCaseDesc Case 101 to 105 tests that the screen is drawn correctly
  					as different screen modes and screen orientation are
  					set for the screen

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Set different screen modes and orientations for the screen and
  					redraw the screen

  @SYMTestExpectedResults The screen is drawn correctly for each setting

*/
		case 101:
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0301"));
			iSubState = 0;
			buf.Format(TRefByValue<const TDesC>(_L("Screen mode %d:%d")),101,iSubState);
			INFO_PRINTF1(buf);
			checkWindowParam=ETrue;
			mode=1;
			if (iCurEnforcement==ESizeEnforcementPixelsAndRotation)
				count+=3;
			else if (iCurEnforcement==ESizeEnforcementPixelsTwipsAndRotation)
				count+=6;
			if (iSecondConnection->DeviceMessageCount()!=count)
				{
				TLogMessageText buf;
				_LIT(KFailCount,"Event Mismatch, Exp=%d, Act=%d");
				buf.Format(KFailCount,count,iSecondConnection->DeviceMessageCount());
				TheClient->iWs.LogMessage(buf);
				TheClient->iWs.Flush();
				}
			retVal = iSecondConnection->DeviceMessageCount();
			TEST(retVal==count);
			if (retVal!=count)
				INFO_PRINTF3(_L("(iSecondConnection->DeviceMessageCount() return value - Expected: %d, Actual: %d"),count ,retVal);
			
			CheckWindows(iCurEnforcement==ESizeEnforcementNone?ETrue:checkWindowParam);
			if (enable)
				iSecondConnection->EnableMessages();
			if (disable)
				iSecondConnection->DisableMessages();
			TheClient->iScreen->SetScreenMode(mode);
			TheClient->iWs.Flush();
			iSubState++;
			break;
		case 102:
/**
  @SYMTestCaseID GRAPHICS-WSERV-0521
*/
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0521"));
			buf.Format(TRefByValue<const TDesC>(_L("Screen mode %d:%d")),101,iSubState);
			INFO_PRINTF1(buf);
			checkWindowParam=EFalse;
			enable=ETrue;
			if (iCurEnforcement==ESizeEnforcementPixelsAndRotation)
				count+=3;
			else if (iCurEnforcement==ESizeEnforcementPixelsTwipsAndRotation)
				count+=6;
			if (iSecondConnection->DeviceMessageCount()!=count)
				{
				TLogMessageText buf;
				_LIT(KFailCount,"Event Mismatch, Exp=%d, Act=%d");
				buf.Format(KFailCount,count,iSecondConnection->DeviceMessageCount());
				TheClient->iWs.LogMessage(buf);
				TheClient->iWs.Flush();
				}
			retVal = iSecondConnection->DeviceMessageCount();
			TEST(retVal==count);
			if (retVal!=count)
				INFO_PRINTF3(_L("(iSecondConnection->DeviceMessageCount() return value - Expected: %d, Actual: %d"),count ,retVal);

			if (enable)
				iSecondConnection->EnableMessages();
			if (disable)
				iSecondConnection->DisableMessages();
			TheClient->iScreen->SetScreenMode(mode);
			TheClient->iWs.Flush();
			iSubState++;
			break;
		case 103:
/**
  @SYMTestCaseID GRAPHICS-WSERV-0522
*/
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0522"));
			buf.Format(TRefByValue<const TDesC>(_L("Screen mode %d:%d")),101,iSubState);
			INFO_PRINTF1(buf);
			count=1;
			mode=1;
			checkWindowParam=ETrue;
			if (iCurEnforcement==ESizeEnforcementPixelsAndRotation)
				count+=3;
			else if (iCurEnforcement==ESizeEnforcementPixelsTwipsAndRotation)
				count+=6;
			if (iSecondConnection->DeviceMessageCount()!=count)
				{
				TLogMessageText buf;
				_LIT(KFailCount,"Event Mismatch, Exp=%d, Act=%d");
				buf.Format(KFailCount,count,iSecondConnection->DeviceMessageCount());
				TheClient->iWs.LogMessage(buf);
				TheClient->iWs.Flush();
				}
			retVal = iSecondConnection->DeviceMessageCount();
			TEST(retVal==count);
			if (retVal!=count)
				INFO_PRINTF3(_L("(iSecondConnection->DeviceMessageCount() return value - Expected: %d, Actual: %d"),count ,retVal);

			CheckWindows(iCurEnforcement==ESizeEnforcementNone?ETrue:checkWindowParam);
			if (enable)
				iSecondConnection->EnableMessages();
			if (disable)
				iSecondConnection->DisableMessages();
			TheClient->iScreen->SetScreenMode(mode);
			TheClient->iWs.Flush();
			iSubState++;
			break;
		case 104:
/**
  @SYMTestCaseID GRAPHICS-WSERV-0523
*/
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0523"));
			buf.Format(TRefByValue<const TDesC>(_L("Screen mode %d:%d")),101,iSubState);
			INFO_PRINTF1(buf);
			checkWindowParam=ETrue;
			count=2;
			disable=ETrue;
			if (iCurEnforcement==ESizeEnforcementPixelsAndRotation)
				count+=3;
			else if (iCurEnforcement==ESizeEnforcementPixelsTwipsAndRotation)
				count+=6;
			if (iSecondConnection->DeviceMessageCount()!=count)
				{
				TLogMessageText buf;
				_LIT(KFailCount,"Event Mismatch, Exp=%d, Act=%d");
				buf.Format(KFailCount,count,iSecondConnection->DeviceMessageCount());
				TheClient->iWs.LogMessage(buf);
				TheClient->iWs.Flush();
				}
			retVal = iSecondConnection->DeviceMessageCount();
			TEST(retVal==count);
			if (retVal!=count)
				INFO_PRINTF3(_L("(iSecondConnection->DeviceMessageCount() return value - Expected: %d, Actual: %d"),count ,retVal);

			if (enable)
				iSecondConnection->EnableMessages();
			if (disable)
				iSecondConnection->DisableMessages();
			TheClient->iScreen->SetScreenMode(mode);
			TheClient->iWs.Flush();
			iSubState++;
			break;
		case 105:
/**
  @SYMTestCaseID GRAPHICS-WSERV-0524
*/
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0524"));
			buf.Format(TRefByValue<const TDesC>(_L("Screen mode %d:%d")),101,iSubState);
			INFO_PRINTF1(buf);
			checkWindowParam=EFalse;
			count=2;
			enable=ETrue;
			if (iCurEnforcement==ESizeEnforcementPixelsAndRotation)
				count+=3;
			else if (iCurEnforcement==ESizeEnforcementPixelsTwipsAndRotation)
				count+=6;
			if (iSecondConnection->DeviceMessageCount()!=count)
				{
				TLogMessageText buf;
				_LIT(KFailCount,"Event Mismatch, Exp=%d, Act=%d");
				buf.Format(KFailCount,count,iSecondConnection->DeviceMessageCount());
				TheClient->iWs.LogMessage(buf);
				TheClient->iWs.Flush();
				}
			retVal = iSecondConnection->DeviceMessageCount();
			TEST(retVal==count);
			if (retVal!=count)
				INFO_PRINTF3(_L("(iSecondConnection->DeviceMessageCount() return value - Expected: %d, Actual: %d"),count ,retVal);

			CheckWindows(iCurEnforcement==ESizeEnforcementNone?ETrue:checkWindowParam);
			if (enable)
				iSecondConnection->EnableMessages();
			if (disable)
				iSecondConnection->DisableMessages();
			TheClient->iScreen->SetScreenMode(mode);
			TheClient->iWs.Flush();


			iSecondConnection->DisableMessages();
			if (iCurEnforcement==ESizeEnforcementNone)
				{	// Do it again with different enforcement mode
				SetScreenModeEnforcement(ESizeEnforcementPixelsAndRotation);
				ResetCounter(100);
				}
			else if (iCurEnforcement==ESizeEnforcementPixelsAndRotation)
				{	// Do it again with different enforcement mode
				SetScreenModeEnforcement(ESizeEnforcementPixelsTwipsAndRotation);
				ResetCounter(100);
				}
			else
				{
				SetScreenModeEnforcement(ESizeEnforcementNone);
				delete iSecondConnection;
				iSecondConnection=NULL;
				}
			TheClient->iWs.Flush();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0302

  @SYMDEF  			DEF099638

  @SYMTestCaseDesc Test that a drawing can be scaled while different screen
  					modes are set

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Set different screen modes and scale a drawing

  @SYMTestExpectedResults The drawing is redrawn to scale

*/
		case 110:
			{
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0302"));
			// Ensure that the TestWin and BaseWin will be drawn properly in all screen modes
			SetScreenModeEnforcement(ESizeEnforcementPixelsAndRotation);
			TheClient->SetTestClientScreenMode(0);
			INFO_PRINTF1(_L("Scaled drawing - screen mode 0"));
			ScaledDrawingL();
			TheClient->SetTestClientScreenMode(1);
			INFO_PRINTF1(_L("Scaled drawing - screen mode 1"));
			ScaledDrawingL();
			// Restore screen mode and enforcement mode				
			TheClient->SetTestClientScreenMode(0);
			SetScreenModeEnforcement(iOldEnfMode);
			break;
			}

/**

  @SYMTestCaseID GRAPHICS-WSERV-0303

  @SYMDEF  			DEF081259

  @SYMTestCaseDesc Test screen mode and rotation panic messages

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Test the panics fro screen mode and rotation respond as
  					expected

  @SYMTestExpectedResults Panics respond as expected

*/
		case 111:
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0303"));
			INFO_PRINTF1(_L("Panic"));
			TestPanicsL();
			break;
#endif
/**

  @SYMTestCaseID GRAPHICS-WSERV-0304

  @SYMDEF  			DEF081259

  @SYMTestCaseDesc Test that if there are invisible windows and then the
  					screen mode is changed, the windows are redrawn correctly

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create some windows, make some invisible, change the screen
  					mode and redraw the windows

  @SYMTestExpectedResults The windows are redrawn without error

*/
		case 112:
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0304"));
			INFO_PRINTF1(_L("Invisible Wnds And Infinite Redraw Bugs")); // two defects one problem
			InvisibleWndAndInfiniteRedrawBugL();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0358

  @SYMDEF  			PDEF096151

  @SYMTestCaseDesc Ensure that wserv does not panic when calling CWsScreenDevice::SetScreenMode()
                   after a client has deleted a screen device used by an active group window

  @SYMTestPriority Critical

  @SYMTestStatus Implemented

  @SYMTestActions Creates a RWsSession
                  Creates a primary screen device
                  Creates another screen device
                  Creates a group window
                  Gets all the valid screen modes
                  Deletes the primary screen device 
                  Calls SetScreenMode on the second screen device
                    (this step triggered the defect - panicking wserv)
                  Cleans up the above

  @SYMTestExpectedResults The call to SetScreenMode should not cause wserv to panic

*/
 		case 113:
 			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0358"));
			INFO_PRINTF1(_L("Set screen mode after a client has deleted the screen device"));
			SetScreenModeAfterScreenDeviceDeletedL();
			break;
		case 114:
			INFO_PRINTF1(_L("Rotate/screen mode test"));
/**
  @SYMTestCaseID GRAPHICS-WSERV-0525
*/
			((CTScrModeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0525"));
			TestRotateAndScreenModeL();
			break;
		default:
            ((CTScrModeStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			if (iTest->iState==115 || iTest->iState>1000)
				TestComplete();
			break;
		}
	((CTScrModeStep*)iStep)->RecordTestResultL();
	}

//
// CSecondConnection
//

CSecondConnection::~CSecondConnection()
	{
	delete iWindow;
	delete iWindow2;
	iGroup->GroupWin()->EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	delete iGroup;
	delete iClient;
	}

void CSecondConnection::ConstructL(TInt aScreenNumber,const TRect &aWinRect, CTestBase *aTest, CTestStep* aTestStep)
	{
	iClient=new(ELeave) CTClient;
	iClient->SetScreenNumber(aScreenNumber);
	iClient->ConstructL();
	iGroup=new(ELeave) CSecondConnectionGroup(iClient,this,aTest,aTestStep);
	iGroup->ConstructL();
//
	iWindow=new(ELeave) CTBlankWindow;
	iWindow->ConstructL(*iGroup);
	iWindow->BaseWin()->SetShadowDisabled(ETrue);
	iWindow->SetColor(TRgb::Gray4(1));
	TPoint offset=BaseWin->BaseWin()->InquireOffset((*iGroup->GroupWin()));
	iWindow->SetExtL(offset+aWinRect.iTl,aWinRect.Size());
	iWindow->Activate();
//
	iWindow2=new(ELeave) CTBlankWindow;
	iWindow2->ConstructL(*iGroup);
	iWindow2->BaseWin()->SetShadowDisabled(ETrue);
	iWindow2->SetColor(TRgb::Gray4(1));
	TPoint offset2=TestWin->BaseWin()->InquireOffset((*iGroup->GroupWin()));
	iWindow2->SetExtL(offset2+aWinRect.iTl,aWinRect.Size());
	iWindow2->BaseWin()->SetVisible(EFalse);
	iWindow2->Activate();
	}

void CSecondConnection::SetWindow2Visibility(TBool aVisible)
	{
	iWindow2->BaseWin()->SetVisible(aVisible);
	iClient->iWs.Flush();
	iClient->iWs.Finish();
	}

void CSecondConnection::EnableMessages()
	{
	iGroup->GroupWin()->EnableScreenChangeEvents();
	iClient->iWs.Flush();
	}

void CSecondConnection::DisableMessages()
	{
	iGroup->GroupWin()->DisableScreenChangeEvents();
	iClient->iWs.Flush();
	}

TInt CSecondConnection::DeviceMessageCount() const
	{
	return(iMessageCount);
	}

void CSecondConnection::ScreenDeviceChanged()
	{
	iMessageCount++;
	}

//
// CSecondConnectionGroup
//

CSecondConnectionGroup::CSecondConnectionGroup(CTClient *aClient, CSecondConnection *aSecondConnection, CTestBase *aTest, CTestStep* aTestStep) : CTWindowGroup(aClient), iTest(aTest), iSecondConnection(aSecondConnection), iTestStep(aTestStep)
	{}

void CSecondConnectionGroup::ScreenDeviceChanged()
	{
	iSecondConnection->ScreenDeviceChanged();
	TPixelsTwipsAndRotation sar;
	Client()->iScreen->GetDefaultScreenSizeAndRotation(sar);
	Client()->iScreen->SetScreenSizeAndRotation(sar);
//
	iTestStep->TEST(Client()->iScreen->SizeInPixels()==sar.iPixelSize);
	iTestStep->TEST(Client()->iScreen->SizeInTwips()==sar.iTwipsSize);
	TSize pixelConv;
	pixelConv.iWidth=Client()->iScreen->HorizontalTwipsToPixels(sar.iTwipsSize.iWidth);
	pixelConv.iHeight=Client()->iScreen->VerticalTwipsToPixels(sar.iTwipsSize.iHeight);
	iTestStep->TEST(pixelConv==sar.iPixelSize);
	}


//
// SecondClientConnection
//

SecondClientConnection::SecondClientConnection()
	{
	}

SecondClientConnection::~SecondClientConnection()
	{
	}

void SecondClientConnection::ConstructL(TInt aScreenMode, const TPixelsTwipsAndRotation& aSizeAndRotation)
	{
	CTClient::ConstructL();
	iGroup=new(ELeave) TestWindowGroup(this);
	iGroup->ConstructL();
	iScreen->SetScreenModeEnforcement(ESizeEnforcementPixelsAndRotation);
	iScreen->SetScreenSizeAndRotation(aSizeAndRotation);
	iScreen->SetScreenMode(aScreenMode);
	}

__WS_CONSTRUCT_STEP__(ScrMode)
