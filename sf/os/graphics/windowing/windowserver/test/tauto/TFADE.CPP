// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test Fading and UnFading of windows
// 
//

#include "TFADE.H"
#define __DISPLAY_MODE_64K_COLOR

//CRedrawWin

CRedrawWin::CRedrawWin(CTFade *aTest) : CTWin(), iTest(aTest)
	{}

CRedrawWin::~CRedrawWin()
	{
	}

void CRedrawWin::ConstructL(TRect aArea)
	{

	ConstructExtLD(*TheClient->iGroup,aArea.iTl,aArea.Size());
	AssignGC(*TheClient->iGc);
	Activate();
	iWin.BeginRedraw();
	iWin.EndRedraw();
	}

void CRedrawWin::Draw()
	{
	iTest->Drawing(iTest->Content(),iGc);
	}


//CFadeTest

CTFade::CTFade(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
#if defined (__MARM_THUMB__)
	CanFade = false;
	CanFadeSet = false;
#endif
	}

void CTFade::ConstructL()
	{
	iDeviceDisplayMode=TheClient->iScreen->DisplayMode();
	iTestWinSize=StdTestWindowSize();
	iFadeDrawMode = iDeviceDisplayMode;
	iTestWinCopy = new (ELeave) CFbsBitmap();
	iTestWinCopy->Create(iTestWinSize,TheClient->iScreen->DisplayMode());
	iCheckWinCopy = new (ELeave) CFbsBitmap();
	iCheckWinCopy->Create(iTestWinSize,TheClient->iScreen->DisplayMode());
	}

CTFade::~CTFade()
	{
	__ASSERT_ALWAYS(!iBlankWin,AutoPanic(EAutoFadeWindow));
	delete iTestWinCopy;
	delete iCheckWinCopy;
	}

TInt CTFade::Content()
	{
	return iContent;
	}

void CTFade::CreateBlankWindowL()
	{
	__ASSERT_ALWAYS(!iBlankWin,AutoPanic(EAutoFadeWindow));
	TSize scrSize(TheClient->iScreen->SizeInPixels());
	iTestWinSize.SetSize(2*scrSize.iWidth/3-6,scrSize.iHeight-6);
	iBlankWin=new(ELeave) CTBlankWindow();
	iWindowRect.SetRect(TPoint(3+scrSize.iWidth/3,3),iTestWinSize);
	iBlankWin->SetUpL(iWindowRect.iTl,iTestWinSize,TheClient->iGroup,*TheClient->iGc);
	iBlankWin->BaseWin()->SetRequiredDisplayMode(EGray16);
	}

void CTFade::DestroyBlankWindow()
	{
	__ASSERT_ALWAYS(iBlankWin,AutoPanic(EAutoFadeWindow));
	delete iBlankWin;
	iBlankWin=NULL;
	}

void CTFade::CreateBackupWindowL(TBool aMaintainBackup)
	{
	__ASSERT_ALWAYS(!iWin,AutoPanic(EAutoFadeWindow));
	CTBackedUpWin* backUpWin=new(ELeave) CTBackedUpWin(MODE_LT_64K(iDeviceDisplayMode)?iFadeDrawMode:iDeviceDisplayMode);
	iWin=backUpWin;
	iOwnWindow=ETrue;
	iWindowRect.SetRect(TPoint(2*iTestWinSize.iWidth+35,7),iTestWinSize);
	backUpWin->ConstructExtLD(*TheClient->iGroup,iWindowRect.iTl,iTestWinSize);
	if (aMaintainBackup)
		backUpWin->BackedUpWin()->MaintainBackup();
	backUpWin->Activate();
	TheClient->WaitForRedrawsToFinish();		//Without this bitmaps won't draw into the window
	}

void CTFade::CreateRedrawWindowL()
	{
	__ASSERT_ALWAYS(!iWin,AutoPanic(EAutoFadeWindow));
	iWindowRect.SetRect(TPoint(2*iTestWinSize.iWidth+35,7),iTestWinSize);
	CRedrawWin* redrawWin=new(ELeave) CRedrawWin(this);
	redrawWin->ConstructL(iWindowRect);
	redrawWin->Win()->EnableRedrawStore(EFalse);
	iWin=redrawWin;
	iOwnWindow=ETrue;
	iWin->BaseWin()->SetRequiredDisplayMode(iFadeDrawMode);
	}

void CTFade::DestroyDrawableWindow()
	{
	__ASSERT_ALWAYS(iWin,AutoPanic(EAutoFadeWindow));
	if (iOwnWindow)
		delete iWin;
	iWin=NULL;
	}

void CTFade::CreateBitmapsL()
	{
	iTestWinSize=BaseWin->Size();
	iBaseRect.SetRect(BaseWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),iTestWinSize);
	iNormalBitmap.Create(iTestWinSize,MODE_LT_64K(iDeviceDisplayMode)?iFadeDrawMode:iDeviceDisplayMode);
	iFadedBitmap.Create(iTestWinSize,MODE_LT_64K(iDeviceDisplayMode)?iFadeDrawMode:iDeviceDisplayMode);
	iNormalBitmapDevice=CFbsBitmapDevice::NewL(&iNormalBitmap);
	iFadedBitmapDevice=CFbsBitmapDevice::NewL(&iFadedBitmap);
	User::LeaveIfError(iNormalBitmapDevice->CreateContext(iNormalBitGc));
	User::LeaveIfError(iFadedBitmapDevice->CreateContext(iFadedBitGc));
	iFadedBitGc->SetFadeMode(ETrue);
	iBaseWinMode=BaseWin->BaseWin()->DisplayMode();
	BaseWin->BaseWin()->SetRequiredDisplayMode(iFadeDrawMode);
	}

void CTFade::DestroyBitmaps()
	{
	delete iNormalBitGc;
	delete iFadedBitGc;
	delete iNormalBitmapDevice;
	delete iFadedBitmapDevice;
	BaseWin->BaseWin()->SetRequiredDisplayMode(iBaseWinMode);
	}

void CTFade::Drawing(TInt aDrawFunc, CBitmapContext *gc)
	{
	TRgb grey=TRgb::Gray4(2);
	TInt ii;
	gc->Reset();
	switch (aDrawFunc)
		{
	case 1:
	case 2:
		Draw(0,&grey,gc);
		gc->Reset();
		Draw(1,NULL,gc);
		if (aDrawFunc==1)
			break;
		gc->Reset();
		Draw(3,NULL,gc);
		break;
	case 3:
		Draw(0,&grey,gc);
		gc->Reset();
		Draw(3,NULL,gc);
		break;
	case 4:
		grey=TRgb::Gray4(1);
		gc->Reset();
		Draw(0,&grey,gc);
		gc->Reset();
		for (ii=1;ii<37;ii+=7)
			Draw(2,&ii,gc);
		break;
	case 5:
		Draw(0,&grey,gc);
		gc->Reset();
		Draw(1,NULL,gc);
		gc->Reset();
		for (ii=2;ii<60;ii+=11)
			Draw(2,&ii,gc);
		break;
	case 6:
	case 7:
		Draw(0,&grey,gc);
		gc->Reset();
		Draw(3,NULL,gc);
		gc->Reset();
		for (ii=3;ii<70;ii+=13)
			Draw(2,&ii,gc);
		if (aDrawFunc==6)
			break;
		gc->Reset();
		Draw(1,NULL,gc);
		break;
	default:;
		}
	}

void CTFade::Draw(TInt aDrawFunc, TAny *aParam, TBool aAlternativeFade/*=EFalse*/, TBool aFade/*=EFalse*/)
	{
	CWindowGc *gc=TheClient->iGc;
	gc->Activate(*iWin->DrawableWin());
	gc->Reset();
	if (aFade)
		gc->SetFaded(ETrue);
	iNormalBitGc->Reset();
	iFadedBitGc->Reset();
	iFadedBitGc->SetFadeMode(ETrue);
	if (aAlternativeFade)
		iFadedBitGc->SetFadingParameters(BLACK_ALTERNATE,WHITE_ALTERNATE);
	Draw(aDrawFunc,aParam,gc);
	Draw(aDrawFunc,aParam,iNormalBitGc);
	Draw(aDrawFunc,aParam,iFadedBitGc);
	gc->Deactivate();
	if (aAlternativeFade)
		iFadedBitGc->SetFadingParameters(BLACK_NORMAL,WHITE_NORMAL);
	}

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA // CWindowGc fading is deprecated in NGA
void CTFade::GCDraw(TInt aDrawFunc, TAny *aParam, TBool aAlternativeFade)
	{
	CWindowGc *gc=TheClient->iGc;
	gc->Activate(*iWin->DrawableWin());
	gc->Reset();
	Draw(aDrawFunc,aParam,gc);
	gc->Deactivate();
	gc->Activate(*BaseWin->DrawableWin());
	gc->Reset();
	gc->SetFaded(ETrue);
	if (aAlternativeFade)
		gc->SetFadingParameters(BLACK_ALTERNATE,WHITE_ALTERNATE);
	Draw(aDrawFunc,aParam,gc);
	gc->Deactivate();
	}
#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA

void CTFade::Draw(TInt aDrawFunc, TAny *aParam, CBitmapContext *aGc)
	{
	switch(aDrawFunc)
		{
		case 0:
			aGc->SetBrushColor(*((TRgb *)aParam));
			aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
			aGc->SetPenStyle(CGraphicsContext::ENullPen);
			aGc->DrawRect(TRect(iTestWinSize));
			break;
		case 1:
			{
			TSize half(iTestWinSize.iWidth/2,iTestWinSize.iHeight/2);
			TRect rect(half);
			aGc->DrawEllipse(rect);
			aGc->DrawEllipse(TRect(TPoint(0,half.iHeight),half));
			aGc->SetOrigin(TPoint(half.iWidth,0));
			aGc->SetClippingRect(rect);
			aGc->DrawEllipse(rect);
			aGc->SetOrigin(TPoint(half.iWidth,half.iHeight));
			aGc->SetClippingRect(rect);
			aGc->DrawEllipse(rect);
			aGc->SetOrigin(TPoint());
			aGc->CancelClippingRect();
			}
			break;
		case 2:
			{
			TInt param= *((TInt *)aParam);
			if (param&0x1)
				aGc->DrawLine(TPoint(param+(param*27)%iTestWinSize.iWidth,0),
							 TPoint(iTestWinSize.iWidth-((param<<1)+(param*19)%iTestWinSize.iWidth),iTestWinSize.iHeight));
			else
				aGc->DrawLine(TPoint(0, (param<<1)+(param*7)%iTestWinSize.iHeight),
							 TPoint(iTestWinSize.iWidth,param+(param*13)%iTestWinSize.iHeight));
			}
			break;
		case 3:
			{
			TPoint pos;
			for(;pos.iX<iTestWinSize.iWidth;pos.iX+=10)
				aGc->DrawLine(pos,pos+TSize(0,iTestWinSize.iHeight));
			for(pos.iX=0;pos.iY<iTestWinSize.iHeight;pos.iY+=10)
				aGc->DrawLine(pos,pos+TSize(iTestWinSize.iWidth,0));
			}
			break;
		}
	}

void CTFade::CompareWithBitmap(TBool aFade)
	{
	TBool match;
	CWindowGc& gc=*TheClient->iGc;
	CFbsBitmap* bitmap;
	TheClient->iWs.Flush();
	if (aFade)
		bitmap=&iFadedBitmap;
	else
		bitmap=&iNormalBitmap;
	gc.Activate(*BaseWin->DrawableWin());
	gc.Reset();
	gc.BitBlt(TPoint(),bitmap);
	gc.Deactivate();
	TheClient->iWs.Flush();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	match=TheClient->iScreen->RectCompare(iBaseRect,iWindowRect);
#else // In NGA comparison has to be lossy because fading can be implemented on hardware
	match=LossyCompareWindow(*TheClient->iScreen, *iTestWinCopy, *iCheckWinCopy, iWindowRect);
#endif
	TEST(match);
	if (!match)
			{
			_LIT(KLog,"Windows content don't match when they should");
			LOG_MESSAGE(KLog);
			}
	}

void CTFade::CompareWindows(TBool aAlternativeFade/*=EFalse*/)
	{
	if (aAlternativeFade)
		iWin->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly,BLACK_ALTERNATE,WHITE_ALTERNATE);
	else
		iWin->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
	TheClient->iWs.Flush();
	TBool retVal = TheClient->iScreen->RectCompare(iBaseRect,iWindowRect);
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare(iBaseRect,iWindowRect) return value - Expected: %d, Actual: %d"), ETrue, retVal);		

	iWin->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	}

#define FADE(n,col) n/2+col/2
TInt CTFade::Fade4(TInt aGray4)
	{
#if defined(__WINS__)
	return FADE(4,aGray4);
#elif defined (__MARM_THUMB__)
	return (CanFade ? FADE(4,aGray4):aGray4);
#elif defined (__MARM__)
	return FADE(4,aGray4);
#else
	return aGray4;
#endif
	}

TInt CTFade::Fade16(TInt aGray16)
	{
#if defined(__WINS__)
	return FADE(16,aGray16);
#elif defined (__MARM_THUMB__)
	return (CanFade ? FADE(16,aGray16):aGray16);
#elif defined (__MARM__)
	return FADE(16,aGray16);
#else
	return aGray16;
#endif
	}

TRgb CTFade::FadeRgb(TRgb aColor)
	{
	switch (iFadeDrawMode)
		{
	case EColor16M:
	case EColor16MU:		
	case EColor16MA:
	case EColor16MAP:
		break; 
	default:
		aColor=TRgb::Color64K(aColor.Color64K());
		break; 
		}
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA // Color fading calculation differs in NGA
	TInt fadeMapFactor=WHITE_NORMAL-BLACK_NORMAL+1;
#else
	TInt fadeMapFactor=WHITE_NORMAL-BLACK_NORMAL;
#endif
	TInt fadeMapOffset=BLACK_NORMAL;
	TInt value = aColor.Internal();

	TInt b = (((value & 0x000000ff) * fadeMapFactor) >> 8) + fadeMapOffset;
	TInt g = (((value & 0x0000ff00) * fadeMapFactor) >> 16) + fadeMapOffset;
	TInt r = (((value & 0x00ff0000) * fadeMapFactor) >> 24) + fadeMapOffset;
	return TRgb(r,g,b);
	}

TRgb CTFade::FadeRgb(TRgb aColor, TInt aFadeMapFactor, TInt aFadeMapOffset)
	{
	switch (iFadeDrawMode)
		{
	case EColor16M:
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		break; 
	default:
		aColor=TRgb::Color64K(aColor.Color64K());
		break; 
		}
	TInt value = aColor.Internal();

	TInt b = (((value & 0x000000ff) * aFadeMapFactor) >> 8) + aFadeMapOffset;
	TInt g = (((value & 0x0000ff00) * aFadeMapFactor) >> 16) + aFadeMapOffset;
	TInt r = (((value & 0x00ff0000) * aFadeMapFactor) >> 24) + aFadeMapOffset;
	return TRgb(r,g,b);
	}

inline void CTFade::ViewDelay()
	//
	//This routine can provide a delay which will allow the user to see the colors changing
	//
	{
	TheClient->iWs.Finish();
	/*TheClient->iWs.Flush();
	User::After(1000000);*/		// 1 sec
	}

void CTFade::ColorTest()
	{
	__ASSERT_ALWAYS(iBlankWin,AutoPanic(EAutoFadeWindow));
	TBool retVal;
	TSize size(iTestWinSize);
#if defined(SMALL_RECTS)
	size.iWidth=Min(SIZE_X,size.iWidth);
	size.iHeight=Min(SIZE_Y,size.iHeight);
#endif
	TRect windowRect(iBlankWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),size);
	TInt ii;
	iBlankWin->BaseWin()->SetRequiredDisplayMode(EGray4);
	TheClient->WaitForRedrawsToFinish();		//Force the screen into 4 gray mode
	TRgb fadedColor;
	for (ii=1;ii<4;ii+=2)		//0 and 3 now give dithered colors when fading so causing this test to fail
		{
		iBlankWin->SetColor(TRgb::Gray4(ii));
		ViewDelay();
		INFO_PRINTF1(_L(" Testing Normal Color"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		retVal = CheckBlankWindow(windowRect,TRgb::Gray4(ii),TheClient->iScreen);
#else
		TheClient->iScreen->CopyScreenToBitmap(iTestWinCopy, windowRect);
		retVal = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, TRgb::Gray4(ii));
#endif
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("CheckBlankWindow(windowRect,TRgb::Gray4(ii),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retVal);		
		iBlankWin->BaseWin()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
		ViewDelay();
		fadedColor=MODE_LT_64K(iDeviceDisplayMode)?TRgb::Gray4(Fade4(ii)):FadeRgb(TRgb::Gray4(ii));
#if defined (__MARM_THUMB__)
		if (ii==1)
			{
			CanFade=!CheckBlankWindow(windowRect,fadedColor,TheClient->iScreen);
			CanFadeSet=ETrue;
			fadedColor=MODE_LT_64K(iDeviceDisplayMode)?TRgb::Gray4(Fade4(ii)):FadeRgb(TRgb::Gray4(ii));
			}
#endif
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		retVal = CheckBlankWindow(windowRect,fadedColor,TheClient->iScreen);
#else
		retVal = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, fadedColor);
#endif
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("CheckBlankWindow(windowRect,fadedColor,TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retVal);		
	
		iBlankWin->BaseWin()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
		ViewDelay();
		INFO_PRINTF1(_L(" Testing Unfaded Color"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		retVal = CheckBlankWindow(windowRect,TRgb::Gray4(ii),TheClient->iScreen);
#else
		retVal = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, TRgb::Gray4(ii));
#endif
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("CheckBlankWindow(windowRect,TRgb::Gray4(ii),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retVal);		
		}
	iBlankWin->BaseWin()->SetRequiredDisplayMode(EGray16);

	if (MaxGrays()==0 && MaxColors()<256)
		return;

	INFO_PRINTF1(_L(" Doing 16 Gray Colors"));
	for (ii=0;ii<16;++ii)
		{
		iBlankWin->SetColor(TRgb::Gray16(ii));
		ViewDelay();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		retVal = CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen);
#else
		retVal = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, TRgb::Gray16(ii));
#endif
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retVal);		

		iBlankWin->BaseWin()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
		ViewDelay();
		fadedColor=MODE_LT_64K(iDeviceDisplayMode)?TRgb::Gray16(Fade16(ii)):FadeRgb(TRgb::Gray16(ii));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		retVal = CheckBlankWindow(windowRect,fadedColor,TheClient->iScreen);
#else
		retVal = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, fadedColor);
#endif
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("CheckBlankWindow(windowRect,fadedColor,TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retVal);		

		iBlankWin->BaseWin()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
		ViewDelay();
	
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		retVal = CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen);
#else
		retVal = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, TRgb::Gray16(ii));
#endif
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retVal);		
		}
	}

void CTFade::BlankWindowL()
	{
	TBool retVal;
	if (MaxGrays()==0 && MaxColors()<256)
		return;

	__ASSERT_ALWAYS(iBlankWin,AutoPanic(EAutoFadeWindow));
	CTBlankWindow* blankWin=new(ELeave) CTBlankWindow();
	TRect testArea(iWindowRect);
	TRect windowRect(testArea);
#if defined(SMALL_RECTS)
	TSize size(testArea.Size());
	size.iWidth=Min(2*SIZE_X,size.iWidth);
	size.iHeight=Min(2*SIZE_Y,size.iHeight);
	testArea.SetHeight(size.iHeight);
	testArea.iTl.iX=testArea.iBr.iX-size.iWidth;
	windowRect=testArea;
	windowRect.Shrink(size.iWidth/4,size.iHeight/4);
#else
	windowRect.Shrink(30,30);
#endif
	blankWin->SetUpL(windowRect.iTl,windowRect.Size(),TheClient->iGroup,*TheClient->iGc);
	for (TInt col=2;col<16;col+=6)
		{
		iBlankWin->SetColor(TRgb::Gray16(col));
		TheClient->iScreen->CopyScreenToBitmap(iTestWinCopy, windowRect);
		iBlankWin->BaseWin()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
		blankWin->SetVisible(EFalse);
		TheClient->iWs.Finish();
		TRgb fadedColor=MODE_LT_64K(iDeviceDisplayMode)?TRgb::Gray16(Fade16(col)):FadeRgb(TRgb::Gray16(col));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
 		retVal = CheckBlankWindow(testArea,fadedColor,TheClient->iScreen);
#else
		retVal = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, fadedColor);
#endif
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, fadedColor) return value - Expected: %d, Actual: %d"), ETrue, retVal);		

 		blankWin->SetVisible(ETrue);
		iBlankWin->BaseWin()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
		blankWin->SetVisible(EFalse);
		TheClient->iWs.Finish();
		retVal = CheckBlankWindow(testArea,TRgb::Gray16(col),TheClient->iScreen);
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("CheckBlankWindow(testArea,TRgb::Gray16(col),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retVal);		

 		blankWin->SetVisible(ETrue);
		iBlankWin->SetPos(iWindowRect.iTl+TPoint(15,-15));
		iBlankWin->BaseWin()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
		blankWin->SetVisible(EFalse);
		iBlankWin->SetPos(iWindowRect.iTl);
		fadedColor=MODE_LT_64K(iDeviceDisplayMode)?TRgb::Gray16(Fade16(col)):FadeRgb(TRgb::Gray16(col));
		TheClient->iWs.Finish();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		retVal = CheckBlankWindow(testArea,fadedColor,TheClient->iScreen);
#else
		retVal = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, fadedColor);
#endif
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, fadedColor) return value - Expected: %d, Actual: %d"), ETrue, retVal);		

 		blankWin->SetVisible(ETrue);
		iBlankWin->SetPos(iWindowRect.iTl+TPoint(15,-15));
		iBlankWin->BaseWin()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
		blankWin->SetVisible(EFalse);
		iBlankWin->SetPos(iWindowRect.iTl);
		TheClient->iWs.Finish();
 		retVal = CheckBlankWindow(testArea,TRgb::Gray16(col),TheClient->iScreen);
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("CheckBlankWindow(testArea,TRgb::Gray16(col),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retVal);		

		blankWin->SetVisible(ETrue);
		}
	delete blankWin;
	}

void CTFade::TestStrips(TRect aRect,TInt aHeight,TInt aNumNotFaded,TBool aInvert/*=EFalse*/)
	{
	TBool isFaded;
	TInt ii;
	TRgb col;
	for (ii=0;ii<16;++ii)
		{
		isFaded=((!aInvert)==(!(ii<aNumNotFaded)));
		if (isFaded)
			col=MODE_LT_64K(iDeviceDisplayMode)?TRgb::Gray16(Fade16(ii)):FadeRgb(TRgb::Gray16(ii));
		else
			col=TRgb::Gray16(ii);
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		TBool retVal = CheckBlankWindow(aRect,col,TheClient->iScreen);
#else
		TBool retVal = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, aRect, col);
#endif
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("CheckBlankWindow(aRect,col,TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retVal);		

		aRect.Move(0,aHeight);
		}
	}

void CTFade::FadeChildrenL()
	{
	if (MaxGrays()==0 && MaxColors()<256)
		return;
	
	TInt retVal;
	TBool retBool;

	__ASSERT_ALWAYS(iBlankWin,AutoPanic(EAutoFadeWindow));
	iBlankWin->BaseWin()->SetRequiredDisplayMode(EGray16);
	iBlankWin->SetColor(TRgb::Gray16(0));
	CTBlankWindow* win[16];
	win[0]=iBlankWin;
	TRect rect(iTestWinSize);
	TInt height=iTestWinSize.iHeight/16;
	rect.iTl.iY=height;
	TInt ii;
	for (ii=1;ii<16;++ii)		//Causes memory leakage under OOM
		{
		win[ii]=new(ELeave) CTBlankWindow();
		win[ii]->SetUpL(rect.iTl,rect.Size(),win[ii-1],*TheClient->iGc);
		win[ii]->BaseWin()->SetRequiredDisplayMode(EGray16);
		win[ii]->SetColor(TRgb::Gray16(ii));
		rect.iBr.iY-=height;
		}
	rect=iWindowRect;
	rect.SetHeight(height);
#if defined(SMALL_RECTS)
	TSize size(rect.Size());
	size.iWidth=Min(SIZE_X,size.iWidth);
	size.iHeight=Min(SIZE_Y,size.iHeight);
	rect.SetSize(size);
#endif
	for (ii=0;ii<15;++ii)
		{
		win[ii]->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
		retVal = win[ii]->BaseWin()->IsFaded();
		TEST(retVal==1);
		if (retVal!=1)
			INFO_PRINTF3(_L("win[ii]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), 1, retVal);		

		TestStrips(rect,height,ii);
		win[ii]->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
		retBool = !win[ii]->BaseWin()->IsFaded();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("!win[ii]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), ETrue, retBool);		

		TestStrips(rect,height,16);
		}
	TheClient->iGroup->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(rect,height,0);
	TheClient->iGroup->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(rect,height,16);
	for (ii=0;ii<16;++ii)
		{
		win[ii]->BaseWin()->FadeBehind(ETrue);
		retBool = !win[ii]->BaseWin()->IsFaded();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("!win[ii]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), ETrue, retBool);		

		retVal = win[0]->BaseWin()->IsFaded();
		TEST(retVal==(ii>0));
		if (retVal!=(ii>0))
			INFO_PRINTF3(_L("win[0]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), (ii>0), retVal);		

		TestStrips(rect,height,ii,ETrue);
		win[ii]->BaseWin()->FadeBehind(EFalse);
		TestStrips(rect,height,16);
		}
	iBlankWin->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(rect,height,0);
	iBlankWin->WinTreeNode()->SetNonFading(ETrue);
	TestStrips(rect,height,16);
	win[8]->WinTreeNode()->SetNonFading(EFalse);
	TestStrips(rect,height,8);
	iBlankWin->WinTreeNode()->SetNonFading(EFalse);
	TestStrips(rect,height,0);
	iBlankWin->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(rect,height,16);
	win[8]->WinTreeNode()->SetNonFading(ETrue);
	TestStrips(rect,height,16);
	iBlankWin->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(rect,height,8,ETrue);
	win[8]->WinTreeNode()->SetNonFading(EFalse);
	TestStrips(rect,height,0);
	iBlankWin->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(rect,height,16);
	for (ii=15;ii>0;--ii)
		delete win[ii];
	}

static void ResetAndDestroyWindows(TAny* aPtr)
	{
	RPointerArray<CTBlankWindow>& win = *(static_cast<RPointerArray<CTBlankWindow>*>(aPtr));
	win.Remove(0);
	win.ResetAndDestroy();
	win.Close();
	}

void CTFade::FadeChildrenAfterNewChildIsCreatedL()
	{
	if (MaxGrays()==0 && MaxColors()<256)
		return;
	
	TBool retBool;
	TInt retVal;

	__ASSERT_ALWAYS(iBlankWin,AutoPanic(EAutoFadeWindow));
	iBlankWin->BaseWin()->SetRequiredDisplayMode(iFadeDrawMode);
	iBlankWin->SetColor(TRgb::Gray16(0));
	RPointerArray<CTBlankWindow> win;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyWindows, &win));
	TInt height=iTestWinSize.iHeight/NUMBER_OF_WINDOWS;
	CTBlankWindow* window=NULL;
	TInt ii;
	for (TInt firstLoop=0;firstLoop<NUMBER_OF_WINDOWS-1;)
		{
		win.ResetAndDestroy();
		TheClient->iGroup->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
		TRect rect(iTestWinSize);
		rect.iTl.iY=height;
		CTBlankWindow* parent=iBlankWin;
		for (TInt secondLoop=0;secondLoop<=firstLoop;)
			{
			window=new(ELeave) CTBlankWindow();
			CleanupStack::PushL(window);
			User::LeaveIfError(win.Append(window));
			CleanupStack::Pop(window);
			window->SetUpL(rect.iTl,rect.Size(),parent,*TheClient->iGc);
			window->BaseWin()->SetRequiredDisplayMode(iFadeDrawMode);
			window->SetColor(TRgb::Gray16(++secondLoop));
			rect.iBr.iY-=height;
			parent=window;
			retBool = window->BaseWin()->IsFaded();
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("window->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), ETrue, retBool);		

			}
		++firstLoop;
		TheClient->iGroup->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
		const TInt count=win.Count();
		for(TInt index=0;index<count;++index)
			{
			retBool = win[index]->BaseWin()->IsFaded();
			TEST(!retBool);
			if (retBool)
				INFO_PRINTF3(_L("win[index]->BaseWin()->IsFaded return value - Expected: %d, Actual: %d"), EFalse, retBool);		

			}
		}
	User::LeaveIfError(win.Insert(iBlankWin,0));
	TRect testRect=iWindowRect;
	testRect.SetHeight(height);
#if defined(SMALL_RECTS)
	TSize size(testRect.Size());
	size.iWidth=Min(SIZE_X,size.iWidth);
	size.iHeight=Min(SIZE_Y,size.iHeight);
	testRect.SetSize(size);
#endif
	for (ii=0;ii<NUMBER_OF_WINDOWS;++ii)
		{
		win[ii]->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
		retVal = win[ii]->BaseWin()->IsFaded();
		TEST(retVal==1);
		if (retVal!=1)
			INFO_PRINTF3(_L("win[index]->BaseWin()->IsFaded return value - Expected: %d, Actual: %d"), 1, retVal);		

		TestStrips(testRect,height,ii);
		win[ii]->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
		retBool = !win[ii]->BaseWin()->IsFaded();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("!win[ii]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), EFalse, retBool);		

		TestStrips(testRect,height,NUMBER_OF_WINDOWS);
		}
	TheClient->iGroup->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(testRect,height,0);
	TheClient->iGroup->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(testRect,height,16);
	for (ii=0;ii<NUMBER_OF_WINDOWS;++ii)
		{
		win[ii]->BaseWin()->FadeBehind(ETrue);
		retBool = !win[ii]->BaseWin()->IsFaded();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("!win[ii]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), EFalse, retBool);		
		
		retVal = win[0]->BaseWin()->IsFaded();
		TEST(retVal==(ii>0));
		if (retVal!=(ii>0))
			INFO_PRINTF3(_L("win[index]->BaseWin()->IsFaded return value - Expected: %d, Actual: %d"), (ii>0), retVal);		

		TestStrips(testRect,height,ii,ETrue);
		win[ii]->BaseWin()->FadeBehind(EFalse);
		TestStrips(testRect,height,NUMBER_OF_WINDOWS);
		}
	iBlankWin->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(testRect,height,0);
	iBlankWin->WinTreeNode()->SetNonFading(ETrue);
	TestStrips(testRect,height,NUMBER_OF_WINDOWS);
	win[8]->WinTreeNode()->SetNonFading(EFalse);
	TestStrips(testRect,height,NUMBER_OF_WINDOWS/2);
	iBlankWin->WinTreeNode()->SetNonFading(EFalse);
	TestStrips(testRect,height,0);
	iBlankWin->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(testRect,height,NUMBER_OF_WINDOWS);
	win[8]->WinTreeNode()->SetNonFading(ETrue);
	TestStrips(testRect,height,NUMBER_OF_WINDOWS);
	iBlankWin->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(testRect,height,NUMBER_OF_WINDOWS/2,ETrue);
	win[8]->WinTreeNode()->SetNonFading(EFalse);
	TestStrips(testRect,height,0);
	iBlankWin->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeIncludeChildren);
	TestStrips(testRect,height,NUMBER_OF_WINDOWS);

	CleanupStack::PopAndDestroy(&win);
	}

void CTFade::TestBlocks(TRect aRect,TSize aSize,TInt aNumNotFaded,TBool aInvert/*=EFalse*/)
	{
	TInt count=0;
	TInt ii,jj;
	TRgb col;
	for (ii=0;ii<4;++ii)
		{
		for (jj=0;jj<4;++jj)
			{
			if ((!aInvert)!=(!(count<aNumNotFaded)))
				col=TRgb::Gray16(count);
			else
				{
				col = TRgb::Gray16(count);
				col = MODE_LT_64K(iDeviceDisplayMode)?TRgb::Gray16(Fade16(count)):FadeRgb(col);
				}
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			TBool retBool = CheckBlankWindow(aRect,col,TheClient->iScreen);
#else
			TBool retBool = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, aRect, col);
#endif
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("CheckBlankWindow(aRect,col,TheClient->iScreen) return value - Expected: %d, Actual: %d"), EFalse, retBool);		
	
			aRect.Move(0,aSize.iHeight);
			count++;
			}
		aRect.Move(aSize.iWidth,-jj*aSize.iHeight);
		}
	}

void CTFade::FadeBehindL()
	{
	if (MaxGrays()==0 && MaxColors()<256)
		return;
	
	TBool retBool;
	TInt retVal;

	__ASSERT_ALWAYS(iBlankWin,AutoPanic(EAutoFadeWindow));
	iBlankWin->BaseWin()->SetRequiredDisplayMode(EGray16);
	iBlankWin->SetColor(TRgb::Gray16(0));
	CTBlankWindow* win[4][4];
	win[0][0]=iBlankWin;
	TSize size=iTestWinSize;
	TInt height=iTestWinSize.iHeight/4;
	TInt width=iTestWinSize.iWidth/4;
	TRect rect(iWindowRect.iTl,TSize(width,height));
	TSize rectSize(rect.Size());
	TPoint offset(0,height);
	TPoint topLeft;
	TInt ii,jj;
	CTWinBase* parent;
#if defined(SMALL_RECTS)
	TSize rSize(rectSize);
	rSize.iWidth=Min(SIZE_X,rSize.iWidth);
	rSize.iHeight=Min(SIZE_Y,rSize.iHeight);
	rect.SetSize(rSize);
#endif
	for (ii=0;ii<4;++ii)		//Causes memory leakage under OOM
		{
		parent=TheClient->iGroup;
		topLeft=iWindowRect.iTl+TPoint(ii*width,0);
		size.iHeight=iTestWinSize.iHeight;
		for (jj=0;jj<4;++jj)
			{
			if (ii+jj>0)
				{
				win[ii][jj]=new(ELeave) CTBlankWindow();
				win[ii][jj]->SetUpL(topLeft,size,parent,*TheClient->iGc);
				win[ii][jj]->BaseWin()->SetRequiredDisplayMode(EGray16);
				win[ii][jj]->SetColor(TRgb::Gray16(4*ii+jj));
				}
			size.iHeight-=height;
			topLeft=offset;
			parent=win[ii][jj];
			}
		size.iWidth-=width;
		}
	for (ii=0;ii<4;++ii)
		{
		for (jj=0;jj<4;++jj)
			{
			win[ii][jj]->BaseWin()->FadeBehind(ETrue);
			retVal = win[0][0]->BaseWin()->IsFaded();
			TEST(retVal==(ii>0||jj>0));
			if (retVal!=(ii>0||jj>0))
				INFO_PRINTF3(_L("win[0][0]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), (ii>0||jj>0), retVal);		

			retBool = !win[ii][jj]->BaseWin()->IsFaded();
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("!win[ii][jj]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), ETrue, retBool);		

			TestBlocks(rect,rectSize,4*ii+jj,ETrue);
			win[ii][jj]->BaseWin()->FadeBehind(EFalse);
			TestBlocks(rect,rectSize,16);
			}
		}
   	for (ii=0;ii<4;++ii)
		{
		for (jj=0;jj<4;++jj)
			{
			win[ii][jj]->BaseWin()->FadeBehind(ETrue);
			TestBlocks(rect,rectSize,4*ii+jj,ETrue);
			}
		retVal = win[ii][0]->BaseWin()->IsFaded();
		TEST(retVal==3);
		if (retVal!=3)
			INFO_PRINTF3(_L("win[ii][0]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), 3, retVal);		
		}
   	for (ii=3;ii>=0;--ii)
		{
		retVal = win[ii][0]->BaseWin()->IsFaded();
		TEST(retVal==3);
		if (retVal!=3)
			INFO_PRINTF3(_L("win[ii][0]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), 3, retVal);		

		for (jj=3;jj>=0;--jj)
			{
			TestBlocks(rect,rectSize,4*ii+jj,ETrue);
			win[ii][jj]->BaseWin()->FadeBehind(EFalse);
			}
		}
 	TestBlocks(rect,rectSize,0,ETrue);
   	for (ii=0;ii<4;++ii)
		{
		for (jj=0;jj<4;++jj)
			{
			win[ii][jj]->BaseWin()->FadeBehind(ETrue);
			}
		}
	TInt fadeCount;
   	for (ii=0;ii<4;++ii)
		{
		for (jj=0;jj<4;++jj)
			{
			fadeCount=15-(4*ii+jj);
			retVal = win[0][0]->BaseWin()->IsFaded();
			TEST(retVal==Min(15,fadeCount+1));
			if (retVal!=Min(15,fadeCount+1))
				INFO_PRINTF3(_L("win[0][0]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), Min(15,fadeCount+1), retVal);		

			retVal = win[ii][jj]->BaseWin()->IsFaded();
			TEST(retVal==Max(0,fadeCount));
			if (retVal!=Max(0,fadeCount))
				INFO_PRINTF3(_L("win[ii][jj]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), Max(0,fadeCount), retVal);		

			TestBlocks(rect,rectSize,15,ETrue);
			win[ii][jj]->BaseWin()->FadeBehind(EFalse);
			retVal = win[ii][jj]->BaseWin()->IsFaded();
			TEST(retVal==Max(0,fadeCount));
			if (retVal!=Max(0,fadeCount))
				INFO_PRINTF3(_L("win[ii][jj]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), Max(0,fadeCount), retVal);		

			}
		}
 	TestBlocks(rect,rectSize,0,ETrue);
	if (Fade16(15)==15)			
		{
		win[3][3]->BaseWin()->FadeBehind(ETrue);		//All faded as the only unfaded one is white
		for (ii=3;ii>=0;--ii)
			{
			retVal = win[ii][0]->BaseWin()->IsFaded();
			TEST(retVal==1);
			if (retVal!=1)
				INFO_PRINTF3(_L("win[ii][0]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), 1, retVal);		

			retBool = !win[ii][1]->BaseWin()->IsNonFading();
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("!win[ii][1]->BaseWin()->IsNonFading() return value - Expected: %d, Actual: %d"), ETrue, retBool);		

			win[ii][0]->WinTreeNode()->SetNonFading(ETrue);
			retBool = win[ii][1]->BaseWin()->IsNonFading();
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("win[ii][1]->BaseWin()->IsNonFading() return value - Expected: %d, Actual: %d"), ETrue, retBool);		

			TestBlocks(rect,rectSize,4*ii,ETrue);
			}
		for (ii=3;ii>=0;--ii)
			{
			retVal = win[ii][0]->BaseWin()->IsFaded();
			TEST(retVal==1);
			if (retVal!=1)
				INFO_PRINTF3(_L("win[ii][0]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), 1, retVal);		
			
			retBool = win[ii][1]->BaseWin()->IsNonFading();
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("win[ii][1]->BaseWin()->IsNonFading() return value - Expected: %d, Actual: %d"), ETrue, retBool);		

			win[ii][0]->WinTreeNode()->SetNonFading(EFalse);
			retBool = !win[ii][1]->BaseWin()->IsNonFading();
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("win[ii][1]->BaseWin()->IsNonFading() return value - Expected: %d, Actual: %d"), ETrue, retBool);		

			TestBlocks(rect,rectSize,4*ii);
			}
		win[3][3]->BaseWin()->FadeBehind(EFalse);
		}
	for (ii=3;ii>=0;--ii)
		{
		for (jj=3;jj>=0;--jj)
			{
			if (ii+jj>0)
				delete win[ii][jj];
			}
		}
	}

void CTFade::ColorTest2()
	{
#if defined(__MARM__)
	return;
#else
	__ASSERT_ALWAYS(iBlankWin,AutoPanic(EAutoFadeWindow));
	TSize size=iTestWinSize;
#if defined(SMALL_RECTS)
	size.iWidth=Min(SIZE_X,size.iWidth);
	size.iHeight=Min(SIZE_Y,size.iHeight);
#endif
	if (MaxGrays()==0 && MaxColors()<256)
		return;
	TRect windowRect(iBlankWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),size);
	TUint8 bm;
	TUint8 wm;
	TInt test=0;
	TInt ii;
	FOREVER
		{
		TInt fadeMapFactor = 0;
		TInt fadeMapOffset = 0;

		switch (test)
			{
		case 0:			//Nothing
			bm=0;
			wm=15;
			break;
		case 1:			//Shadowing or Quartz fading
			bm=0;
			wm=7;
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			TheClient->iWs.SetDefaultFadingParameters(0,127);
			fadeMapFactor = 128;
#else
			TheClient->iWs.SetDefaultFadingParameters(STATIC_CAST(TUint8,bm*17),127);
			fadeMapFactor = 127-STATIC_CAST(TUint8,bm*17);
#endif
			break;
		default:
			TheClient->iWs.SetDefaultFadingParameters(BLACK_NORMAL,WHITE_NORMAL);
			return;
			}
		if (wm!=7)
			{
			fadeMapOffset = STATIC_CAST(TUint8,bm*17);
			fadeMapFactor = STATIC_CAST(TUint8,wm*17) - fadeMapOffset;
			TheClient->iWs.SetDefaultFadingParameters(STATIC_CAST(TUint8,bm*17),STATIC_CAST(TUint8,wm*17));
			}

		for (ii=0;ii<16;ii+=5)
			{
			iBlankWin->SetColor(TRgb::Gray16(ii));
			ViewDelay();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			TBool retBool=CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen);
#else
			TBool retBool = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, TRgb::Gray16(ii));
#endif
			TEST(retBool);
			if (!retBool)
				{
				_LIT(KLog,"Setting color on blank window failed  mappings=%d  color=%d");
				LOG_MESSAGE3(KLog,test,ii);
				}
			iBlankWin->BaseWin()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
			ViewDelay();
		
			TRgb col1 = FadeRgb(TRgb::Gray16(ii), fadeMapFactor, fadeMapOffset);
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			retBool=CheckBlankWindow(windowRect,col1,TheClient->iScreen);
#else
			retBool = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, col1);
#endif
			TEST(retBool);
			if (!retBool)
				{
				_LIT(KLog,"Fading the window gave wrong color  mappings=%d  color=%d");
				LOG_MESSAGE3(KLog,test,ii);
				}
			iBlankWin->BaseWin()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
			ViewDelay();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			retBool=CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen);
#else
			retBool = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, TRgb::Gray16(ii));
#endif
			TEST(retBool);
			if (!retBool)
				{
				_LIT(KLog,"Unfading the window gave wrong color  mappings=%d  color=%d");
				LOG_MESSAGE3(KLog,test,ii);
				}
			}
		++test;
		}
#endif	//__MARM__
	}

void CTFade::FadeTest()
	{
#if defined(__MARM__)
	return;
#else
	__ASSERT_ALWAYS(iBlankWin,AutoPanic(EAutoFadeWindow));
	TSize size=iTestWinSize;
#if defined(SMALL_RECTS)
	size.iWidth=Min(SIZE_X,size.iWidth);
	size.iHeight=Min(SIZE_Y,size.iHeight);
#endif
	if (MaxGrays()==0 && MaxColors()<256)
		return;
	TRect windowRect(iBlankWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),size);
	TUint8 bm;
	TUint8 wm;
	TUint8 fb;
	TUint8 fw;
	TInt test=0;
	TInt ii;
	FOREVER
		{
		TInt fadeMapFactor = 0;
		TInt fadeMapOffset = 0;
		fw=0;
		switch (test)
			{
			case 0:			//Nothing
				bm=0;
				wm=15;
				break;
			case 1:			//Shadowing or Quartz fading

			bm=0;
			wm=7;
			fw=127;
			break;
		default:
			return;
			}
		fb=STATIC_CAST(TUint8,17*bm);
		if (!fw)
			fw=STATIC_CAST(TUint8,17*wm);
		fadeMapFactor = fw - fb;
		fadeMapOffset = fb;
		for (ii=0;ii<16;ii+=5)
			{
			iBlankWin->SetColor(TRgb::Gray16(ii));
			ViewDelay();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			TBool retBool = CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen);
#else
			TBool retBool = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, TRgb::Gray16(ii));
#endif
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retBool);
	
			TRgb col3 = TRgb::Gray16(ii).Internal();
			TRgb col1 = FadeRgb(col3, fadeMapFactor, fadeMapOffset);
			iBlankWin->BaseWin()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly,fb,fw);
			ViewDelay();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			retBool = CheckBlankWindow(windowRect,col1,TheClient->iScreen);
#else
			retBool = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, col1);
#endif
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("CheckBlankWindow(windowRect,TRgb::Gray16((ii*wb+add)/15),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retBool);
	
			iBlankWin->BaseWin()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
			ViewDelay();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA			
			retBool = CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen);
#else
			retBool = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, TRgb::Gray16(ii));
#endif
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retBool);

			}
		++test;
		}
#endif	//__MARM__
	}

void CTFade::Draw(TBool aAlternativeFade/*=EFalse*/)
	{
	__ASSERT_ALWAYS(iWin,AutoPanic(EAutoFadeWindow));
	TRgb grey=TRgb::Gray4(2);
	Draw(0,&grey,aAlternativeFade);
	Draw(1,NULL,aAlternativeFade);
	iContent=1;
	CompareWithBitmap(EFalse);
	if (aAlternativeFade)
		iWin->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly,BLACK_ALTERNATE,WHITE_ALTERNATE);
	else
		iWin->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
	CompareWithBitmap(ETrue);
	Draw(3,NULL,aAlternativeFade);
	iContent=2;
	CompareWithBitmap(ETrue);
	iWin->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
	CompareWithBitmap(EFalse);
	}

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
void CTFade::GCDraw()
	{
	__ASSERT_ALWAYS(iWin,AutoPanic(EAutoFadeWindow));
	TRgb grey=TRgb::Gray4(2);
	TInt ii;
	GCDraw(0,&grey);
	GCDraw(3,NULL);
	for (ii=3;ii<70;ii+=13)
		GCDraw(2,&ii);
	iContent=6;
	CompareWindows();
	GCDraw(0,&grey,ETrue);
	GCDraw(3,NULL,ETrue);
	for (ii=3;ii<70;ii+=13)
		GCDraw(2,&ii,ETrue);
	GCDraw(1,NULL,ETrue);
	iContent=7;
	CompareWindows(ETrue);
	}
#endif // TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA

void CTFade::ObscuredL()
	{
	CTBlankWindow* blankWin=new(ELeave) CTBlankWindow();
	TRect windowRect(iWindowRect);
	windowRect.Shrink(30,30);
	blankWin->SetUpL(windowRect.iTl,windowRect.Size(),TheClient->iGroup,*TheClient->iGc);

	__ASSERT_ALWAYS(iWin,AutoPanic(EAutoFadeWindow));
	TRgb grey=TRgb::Gray4(2);
	Draw(0,&grey);
	Draw(3,NULL);
	iContent=3;
 	iWin->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
	blankWin->SetVisible(EFalse);
	CompareWithBitmap(ETrue);
	blankWin->SetVisible(ETrue);
	grey=TRgb::Gray4(1);
	Draw(0,&grey);
	for (TInt ii=1;ii<37;ii+=7)
		Draw(2,&ii);
	iContent=4;
	blankWin->SetVisible(EFalse);
	CompareWithBitmap(ETrue);
	blankWin->SetVisible(ETrue);
 	iWin->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
 	blankWin->SetVisible(EFalse);
	CompareWithBitmap(EFalse);

	delete blankWin;
	}

void CTFade::MovingL()
	{
	CTBlankWindow* blankWin=new(ELeave) CTBlankWindow();
	TRect windowRect(iWindowRect);
	windowRect.Shrink(40,40);
	blankWin->SetUpL(windowRect.iTl,windowRect.Size(),TheClient->iGroup,*TheClient->iGc);

	__ASSERT_ALWAYS(iWin,AutoPanic(EAutoFadeWindow));
	TRgb grey=TRgb::Gray4(2);
	Draw(0,&grey);
	Draw(1,NULL);
	for (TInt ii=2;ii<60;ii+=11)
		Draw(2,&ii);
	iContent=5;
	blankWin->SetPos(windowRect.iTl+TPoint(25,-25));
 	iWin->WinTreeNode()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
 	blankWin->SetPos(windowRect.iTl+TPoint(10,25));
	blankWin->SetVisible(EFalse);
	CompareWithBitmap(ETrue);
	blankWin->SetVisible(ETrue);
	blankWin->SetPos(windowRect.iTl+TPoint(25,-25));
 	iWin->WinTreeNode()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
	blankWin->SetPos(windowRect.iTl+TPoint(-5,10));
 	blankWin->SetVisible(EFalse);
	CompareWithBitmap(EFalse);

	delete blankWin;
	}

void CTFade::SystemFadeL()
	{
	if (MaxGrays()==0 && MaxColors()<256)
		return;

	CTBlankWindow* win[16];
	TInt height=iTestWinSize.iHeight/16;
	TRect rect(iWindowRect);
	rect.SetHeight(height);
	TInt ii;
	for (ii=0;ii<16;++ii)		//Causes memory leakage under OOM
		{
		win[ii]=new(ELeave) CTBlankWindow();
		win[ii]->SetUpL(rect.iTl,rect.Size(),TheClient->iGroup,*TheClient->iGc);
		win[ii]->BaseWin()->SetRequiredDisplayMode(EGray16);
		win[ii]->SetColor(TRgb::Gray16(ii));
		rect.Move(0,height);
		}
	rect=iWindowRect;
	rect.SetHeight(height);
#if defined(SMALL_RECTS)
	TSize size(rect.Size());
	size.iWidth=Min(SIZE_X,size.iWidth);
	size.iHeight=Min(SIZE_Y,size.iHeight);
	rect.SetSize(size);
#endif

	TBool retBool;

	// system fade on
	TheClient->iWs.SetSystemFaded(ETrue);
	for (ii=0;ii<16;++ii)
		{
		retBool = win[ii]->BaseWin()->IsFaded();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("win[ii]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), ETrue, retBool);
		}
	TestStrips(rect,height,0);

	// system fade off
	TheClient->iWs.SetSystemFaded(EFalse);
	for (ii=0;ii<16;++ii)
		{
		retBool = !win[ii]->BaseWin()->IsFaded();
		TEST(retBool);
		if (!retBool)
			INFO_PRINTF3(_L("!win[ii]->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), ETrue, retBool);
		}
	TestStrips(rect,height,16);

	// Now with half non fading
	for (ii=8;ii<16;++ii)
		win[ii]->WinTreeNode()->SetNonFading(ETrue);

	// system fade on
	TheClient->iWs.SetSystemFaded(ETrue);
	TestStrips(rect,height,8,ETrue);

	// system fade off
	TheClient->iWs.SetSystemFaded(EFalse);
	TestStrips(rect,height,16);

	for (ii=0;ii<16;++ii)
		delete win[ii];
	}

void CTFade::SystemAlternateFadeL()
	{
#if defined(__MARM__)
	return;
#else
	__ASSERT_ALWAYS(iBlankWin,AutoPanic(EAutoFadeWindow));
	TSize size=iTestWinSize;
#if defined(SMALL_RECTS)
	size.iWidth=Min(SIZE_X,size.iWidth);
	size.iHeight=Min(SIZE_Y,size.iHeight);
#endif
	if (MaxGrays()==0 && MaxColors()<256)
		return;
	TRect windowRect(iBlankWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),size);
	TUint8 bm;
	TUint8 wm;
	TUint8 fb;
	TUint8 fw;
	TInt wb;
	TInt add;
	TInt test=0;
	TInt ii;
	TBool retBool;
	FOREVER
		{
		TInt fadeMapFactor = 0;
		TInt fadeMapOffset = 0;
		fw=0;
		switch (test)
			{
			case 0:			//Nothing
				bm=0;
				wm=15;
				break;
			case 1:			//Shadowing or Quartz fading
			bm=0;
			wm=7;
			fw=127;
			break;
		default:
			return;
			}
		wb=wm-bm;
		add=15*bm+7;
		fb=STATIC_CAST(TUint8,17*bm);
		if (!fw)
			fw=STATIC_CAST(TUint8,17*wm);
		fadeMapFactor = fw - fb;
		fadeMapOffset = fb;
		for (ii=0;ii<16;ii+=5)
			{
			iBlankWin->SetColor(TRgb::Gray16(ii));
			ViewDelay();
			TheClient->iWs.Finish();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			retBool = CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen);
#else
			retBool = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, TRgb::Gray16(ii));
#endif
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retBool);

			TheClient->iWs.SetSystemFaded(ETrue,fb,fw);
			ViewDelay();

			TRgb col3 = TRgb::Gray16(ii).Internal();
			TRgb col1 = FadeRgb(col3, fadeMapFactor, fadeMapOffset);
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			retBool = CheckBlankWindow(windowRect,col1,TheClient->iScreen);
#else
			retBool = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, col1);
#endif
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("CheckBlankWindow(windowRect,TRgb::Gray16((ii*wb+add)/15),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retBool);

			TheClient->iWs.SetSystemFaded(EFalse);
			ViewDelay();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			retBool = CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen);
#else
			retBool = LossyCheckBlankWindow(*TheClient->iScreen, *iTestWinCopy, windowRect, TRgb::Gray16(ii));
#endif
			TEST(retBool);
			if (!retBool)
				INFO_PRINTF3(_L("CheckBlankWindow(windowRect,TRgb::Gray16(ii),TheClient->iScreen) return value - Expected: %d, Actual: %d"), ETrue, retBool);
			}
		++test;
		}
#endif	//__MARM__
	}

void CTFade::FadeBehindWhenMovedL()
	{
	__ASSERT_ALWAYS(iBlankWin,AutoPanic(EAutoFadeWindow));
	TDisplayMode displayMode=iBlankWin->BaseWin()->DisplayMode();
	iBlankWin->BaseWin()->SetRequiredDisplayMode(EGray16);
	iBlankWin->SetColor(TRgb::Gray16(1));
	TSize size(iTestWinSize.iHeight/4,iTestWinSize.iWidth/4);
	CTBlankWindow* blankWinTemp=new(ELeave) CTBlankWindow();
	CleanupStack::PushL(blankWinTemp);
	blankWinTemp->SetUpL(TPoint(5,5),size,iBlankWin,*TheClient->iGc);
	blankWinTemp->BaseWin()->SetRequiredDisplayMode(EGray16);
	blankWinTemp->SetColor(TRgb::Gray16(15));
	blankWinTemp->BaseWin()->FadeBehind(ETrue);
	TheClient->Flush();
	blankWinTemp->SetPos(TPoint(5,120));
	//Check whether the back window is faded or not
	TBool retBool = iBlankWin->BaseWin()->IsFaded();
	TEST(retBool);
	if (!retBool)
		INFO_PRINTF3(_L("iBlankWin->BaseWin()->IsFaded() return value - Expected: %d, Actual: %d"), ETrue, retBool);

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	CheckRect(iWindowRect.iTl+TPoint(5,5),iWindowRect.iTl+TPoint(75,5),size,_L("CTFade::FadeBehindWhenMovedL()"));
#else
	TInt res = LossyCompareWindow(*TheClient->iScreen, *iTestWinCopy, *iCheckWinCopy, TRect(iWindowRect.iTl+TPoint(75,5),size));
	TEST(res);
#endif
	
	//Check whether the area behind the moved window and any other area on same window are identical
	blankWinTemp->BaseWin()->FadeBehind(EFalse);
	iBlankWin->BaseWin()->SetRequiredDisplayMode(displayMode);
	CleanupStack::PopAndDestroy(blankWinTemp);
	}

void CTFade::FadeBehindTransparentWindowL()
	{
	const TInt KNumberOfWindows = 3;
	CRedrawWin* win[KNumberOfWindows];
	TRect rect(iWindowRect);
	rect.Resize(-iWindowRect.Width()/3,-iWindowRect.Height()/3);
	rect.Move(iWindowRect.Width()/10,iWindowRect.Height()/5);
	TRect rectWin[KNumberOfWindows];
	
	for (TInt ii=0; ii<KNumberOfWindows; ++ii)		//Causes memory leakage under OOM
		{
		rectWin[ii] = rect;
		win[ii]=new(ELeave) CRedrawWin(this);
		win[ii]->ConstructExtLD(*TheClient->iGroup,rectWin[ii].iTl,rectWin[ii].Size());
		win[ii]->AssignGC(*TheClient->iGc);
		win[ii]->Win()->EnableRedrawStore(EFalse);
		win[ii]->BaseWin()->SetRequiredDisplayMode(EColor16MA);
		if (ii==0)
			{
			win[ii]->Win()->SetBackgroundColor(TRgb(200,0,0,255));
			}
		else
			{
			TEST(KErrNone == win[ii]->Win()->SetTransparencyAlphaChannel());
			if (iStep->TestStepResult() != EPass)
				{
				INFO_PRINTF1(_L("Transparency Alpha channel failed to be enabled"));
				}
			win[ii]->Win()->SetBackgroundColor(TRgb(40,100,0,0)); //RGB colour is of minor importance, as the window is fully transparent (Alpha=0)
			}
		win[ii]->Activate();

		//Make sure each window is drawn to the screen now when the new background 
		//colour have been set but before the call to SetFaded
		win[ii]->Win()->BeginRedraw();
		win[ii]->Win()->EndRedraw();
		
		rect.Resize(0,-iWindowRect.Height()/10);
		rect.Move(iWindowRect.Width()/10,iWindowRect.Height()/20);
		}
	TheClient->iWs.Flush();
	TheClient->iWs.Finish();
	
	win[0]->Win()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
	win[1]->Win()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
	win[2]->Win()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
	
	TheClient->iWs.Flush();
	TheClient->iWs.Finish();
	
	// As the windows ovelap on their left side, compare the top left corners of the faded windows in order to verify that 
	// for the opaque window the directly on top faded area and the faded area under the transparent window (the area that 
	// overlaps with the transparent windows) have the same colour.
	TRgb rgbWin[KNumberOfWindows];
	TheClient->iScreen->GetPixel(rgbWin[0], rectWin[0].iTl);
	TheClient->iScreen->GetPixel(rgbWin[1], rectWin[1].iTl);
	TheClient->iScreen->GetPixel(rgbWin[2], rectWin[2].iTl);
	
	TEST( ETrue == (rgbWin[0].Red()==rgbWin[1].Red())&&(rgbWin[0].Green()==rgbWin[1].Green())&&(rgbWin[0].Blue()==rgbWin[1].Blue()) );
	TEST( ETrue == (rgbWin[0].Red()==rgbWin[2].Red())&&(rgbWin[0].Green()==rgbWin[2].Green())&&(rgbWin[0].Blue()==rgbWin[2].Blue()) );
	
	for (TInt ii=0; ii<KNumberOfWindows; ++ii)
		{
		delete win[ii];
		}
	}

#define BACKUPWIN 11
#define REDRAWWIN 20
void CTFade::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	((CTFadeStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0218

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading colours in windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Fade the colours in windows and check they fade correctly

@SYMTestExpectedResults Colour fade correctly
*/
	case 1:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0218"));
		iTest->LogSubTest(_L("Color Check"));
		CreateBlankWindowL();
		ColorTest();
		//iTest->iState=5;
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0219

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading in a blank window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Fade in a blank window and check the fading occurs correctly

@SYMTestExpectedResults Fading in a blank window occurs correctly
*/
	case 2:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0219"));
		iTest->LogSubTest(_L("Blank Window"));
		BlankWindowL();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0220

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading in child windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Fade in child windows and check the fading occurs correctly

@SYMTestExpectedResults Fading in the child windows occurs correctly
*/
	case 3:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0220"));
		iTest->LogSubTest(_L("Fade Children"));
		FadeChildrenL();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0221

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading in newly created child windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Fade in newly created child windows and check the fading occurs correctly

@SYMTestExpectedResults Fading in the newly created child windows occurs correctly
*/
	case 4:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0221"));
		iTest->LogSubTest(_L("Fade Children newly created"));
		FadeChildrenAfterNewChildIsCreatedL();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0222

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading in window behind another window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Fade in window behind another window and check the fading occurs correctly

@SYMTestExpectedResults Fading in window occurs correctly
*/
	case 5:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0222"));
		iTest->LogSubTest(_L("Fade Behind"));
		FadeBehindL();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0223

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test differnt fading techniques in a window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Fade using different fading techniques in a window
					and check the fading occurs correctly

@SYMTestExpectedResults Fading in window occurs correctly
*/
	case 6:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0223"));
#if(defined(__DISPLAY_MODE_64K_COLOR))  //The test case Alternative Fadings1&2 & System Fade are not executed for EColor64k
		iTest->LogSubTest(_L("Alternative Fadings1"));
		ColorTest2();
#else
		LOG_MESSAGE(_L("Alternative Fadings1 test not run"));
#endif
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0224

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test differnt fading techniques in a window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Fade using different fading techniques in a window
					and check the fading occurs correctly

@SYMTestExpectedResults Fading in window occurs correctly
*/
	case 7:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0224"));
#if(defined(__DISPLAY_MODE_64K_COLOR))  //The test case Alternative Fadings1&2 & System Fade are not executed for EColor64k
		iTest->LogSubTest(_L("Alternative Fadings2"));
		FadeTest();
#else
		LOG_MESSAGE(_L("Alternative Fadings2 test not run"));
#endif
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0225

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test differnt system wide fading techniques in a window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Fade using different system wide fading techniques in a window
					and check the fading occurs correctly

@SYMTestExpectedResults Fading in system occurs correctly
*/
	case 8:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0225"));
		iTest->LogSubTest(_L("System Fade"));
		SystemFadeL();
		SystemAlternateFadeL();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0226

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading in window behind another window when the window has been moved

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Fade in window behind another window and check the fading occurs correctly

@SYMTestExpectedResults Fading in window occurs correctly
*/
	case 9:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0226"));
		iTest->LogSubTest(_L("Fade behind moved"));
		FadeBehindWhenMovedL();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0227

@SYMDEF             DEF081259

@SYMTestCaseDesc    Destroy the blnk window used for fading

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Destroy the blnk window used for fading and check it was deleted correctly

@SYMTestExpectedResults The window is destroyed
*/
	case 10:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0227"));
		DestroyBlankWindow();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0228

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test backup window creation and drawing

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create a backup window and draw in it

@SYMTestExpectedResults The drawing is correct in the window
*/
	case BACKUPWIN:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0228"));
		iTest->LogSubTest(_L("BackupWin Draw"));
		CreateBitmapsL();
		CreateBackupWindowL(EFalse);
		Draw();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0229

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading with backup window obscured

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test fading with the backup window obscured

@SYMTestExpectedResults Fading occurs correctly with window obscured
*/
	case BACKUPWIN+1:
	((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0229"));
		iTest->LogSubTest(_L("BackupWin Obscured"));
		ObscuredL();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0230

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading with backup window being moved

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test fading with the backup window being moved

@SYMTestExpectedResults Fading occurs correctly with window moved
*/
	case BACKUPWIN+2:
	((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0230"));
		iTest->LogSubTest(_L("BackupWin Moving"));
		MovingL();
		DestroyDrawableWindow();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0231

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test backup window creation and drawing

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create a backup window and draw in it

@SYMTestExpectedResults The drawing is correct in the window
*/
	case BACKUPWIN+3:
	((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0231"));
		iTest->LogSubTest(_L("BackupWin Draw"));
		CreateBackupWindowL(ETrue);
		Draw();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0232

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading with backup window obscured

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test fading with the backup window obscured

@SYMTestExpectedResults Fading occurs correctly with window obscured
*/
	case BACKUPWIN+4:
	((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0232"));
		iTest->LogSubTest(_L("BackupWin Obscured"));
		ObscuredL();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0233

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading with backup window being moved

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test fading with the backup window being moved

@SYMTestExpectedResults Fading occurs correctly with window moved
*/
	case BACKUPWIN+5:
	((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0233"));
		iTest->LogSubTest(_L("BackupWin Moving"));
		MovingL();
		DestroyDrawableWindow();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0234

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test redraw window creation and drawing

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create a redraw window and draw in it

@SYMTestExpectedResults The drawing is correct in the window
*/
	case REDRAWWIN:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0234"));
		iTest->LogSubTest(_L("RedrawWin Draw"));
		CreateRedrawWindowL();
		Draw();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0235

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading with redraw window obscured

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test fading with the redraw window obscured

@SYMTestExpectedResults Fading occurs correctly with window obscured
*/
	case REDRAWWIN+1:
	((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0235"));
		iTest->LogSubTest(_L("RedrawWin Obscured"));
		ObscuredL();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0236

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading with redraw window being moved

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test fading with the redraw window being moved

@SYMTestExpectedResults Fading occurs correctly with window moved
*/
	case REDRAWWIN+2:
	((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0236"));
		iTest->LogSubTest(_L("RedrawWin Moving"));
		MovingL();
		break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0237

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test different fading techniques within a redraw window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test different fading techniques within a redraw window

@SYMTestExpectedResults Fading occurs correctly for the different techniques
*/
	case REDRAWWIN+3:
	((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0237"));
		iTest->LogSubTest(_L("AlternativeFadeDraw"));
		Draw(ETrue);
		break;

/**
@SYMTestCaseID		GRAPHICS-WSERV-0238

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test fading within a redraw window using the graphic context

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test fading within a redraw window using the graphic context

@SYMTestExpectedResults Fading occurs correctly in the window
*/
	case REDRAWWIN+4:
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0238"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		iTest->LogSubTest(_L("GC Test"));
		GCDraw();
		DestroyDrawableWindow();
		DestroyBitmaps();
#endif
		break;

/**
@SYMTestCaseID		GRAPHICS-WSERV-0538

@SYMDEF             DEF120965

@SYMTestCaseDesc    Test fading under transparent window

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Construct opaque window lying under two transparent windows. Fade the opaque and the transparent windows.

@SYMTestExpectedResults Each of the overlapping areas should be faded only once.
*/
	case REDRAWWIN+5:
		{
		((CTFadeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0538"));
		iTest->LogSubTest(_L("Fade behind transparent window"));
		CRedrawWin* win = new(ELeave) CRedrawWin(this);
		win->ConstructL(TRect(0,0,0,0));
		TInt transparency = win->Win()->SetTransparencyAlphaChannel(); 
		if (transparency!=KErrNotSupported)
			{
			FadeBehindTransparentWindowL();
			}
		else
			{
			WARN_PRINTF1(_L("Transparency not supported. Skipping test."));
			}
		delete win;
		break;
		}
	case REDRAWWIN+6:
		((CTFadeStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		iTest->LogSubTest(_L("Test complete\n"));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
		DestroyDrawableWindow();
		DestroyBitmaps();
#endif
		((CTFadeStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
	default:
		((CTFadeStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		break;
		}
	((CTFadeStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(Fade)
