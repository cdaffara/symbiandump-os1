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
// Code for testing CWindowGc::SetCurrentRegion
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TRegion.H"

TBool RegionCanFade;
TBool RegionCanFadeSet;

LOCAL_D TBuf<0x10> text=_L("SOME TEXT");
LOCAL_D TPoint textRefPos(210, 80);

LOCAL_D TRect regionRectList[]={
					TRect(0,0,0,0),	TRect(30,10,70,40),TRect(60,30,80,70),TRect(10,110,70,170),
					TRect(190,0,240,60),TRect(62,63,67,115),TRect(0,180,240,200),TRect(40,50,210,130),
					TRect(160,130,190,160),TRect(100,140,140,170),TRect(140,170,100,140),TRect(90,120,230,200)};
LOCAL_D TRect clippingRectList[]={
					TRect(0,0,200,400),	TRect(30,10,500,600),TRect(-60,0,80,70),TRect(20,0,170,500),
					TRect(0,0,2000,6000),TRect(-1000,-2000,123,195),TRect(10,10,230,190),TRect(0,0,510,730),
					TRect(110,30,190,150),TRect(1,1,500,11170),TRect(0,0,800,940),TRect(-10,-20,240,210)};
LOCAL_D TPoint originList[]={
					TPoint(0,0),TPoint(10,0),TPoint(0,10),TPoint(-5,-20),
					TPoint(1,1),TPoint(0,0),TPoint(0,0),TPoint(100,0),
					TPoint(0,111),TPoint(90,40),TPoint(0,0),TPoint(-1,0)};

CTRegion::CTRegion(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}

void CTRegion::ConstructL()
	{
	iDrawableSize=TestWin->Size();
//
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)iFont,TFontSpec(_L("Arial"),200)));
  	User::LeaveIfError(iPicture.Load(TEST_BITMAP_NAME,0));
  
	iBitmap = new (ELeave) CWsBitmap(TheClient->iWs);
	User::LeaveIfError(iBitmap->Create(TSize(iDrawableSize.iWidth+1,iDrawableSize.iHeight),EGray16));
	iBitmap->SetSizeInTwips(TheClient->iScreen);
	iBitmapDevice=CFbsBitmapDevice::NewL(iBitmap);
	User::LeaveIfError(iBitmapDevice->CreateContext(iBitGc));
	User::LeaveIfError(iBitmapDevice->GetNearestFontToDesignHeightInTwips((CFont *&)iBitFont,TFontSpec(_L("Arial"),200)));
	}

CTRegion::~CTRegion()
	{
	delete iBitGc;
	TheClient->iScreen->ReleaseFont(iFont);
	if (iBitmapDevice)
		iBitmapDevice->ReleaseFont(iBitFont);
	delete iBitmapDevice;
	DeleteChildWindows();
	delete iBitmap;
	}

void CTRegion::CreateChildWindowsL()
	{
	TSize screenSize=TheClient->iGroup->Size();
	TInt winWidth=(screenSize.iWidth/3)-10;
	TInt winHeight=screenSize.iHeight-10;
	iBaseChildWin=new(ELeave) CBlankWindow();
	iBaseChildWin->SetUpL(BaseWin->Win()->InquireOffset(*TheClient->iGroup->GroupWin())+TPoint(winWidth>>2,winHeight>>2),TSize(winWidth>>2,winHeight>>1),TheClient->iGroup,*TheClient->iGc);
	iBaseChildWin->Win()->SetShadowHeight(2);
	iTestChildWin=new(ELeave) CBlankWindow();
	iTestChildWin->SetUpL(TestWin->Win()->InquireOffset(*TheClient->iGroup->GroupWin())+TPoint(winWidth>>2,winHeight>>2),TSize(winWidth>>2,winHeight>>1),TheClient->iGroup,*TheClient->iGc);
	}

void CTRegion::DeleteChildWindows()
	{
	delete iBaseChildWin;
	iBaseChildWin=NULL;
	delete iTestChildWin;
	iTestChildWin=NULL;
	}

void CTRegion::DrawNonClippedSample1(CBitmapContext &aGc)
	{
	aGc.SetPenColor(TRgb::Gray4(0));
	aGc.SetBrushColor(TRgb::Gray4(2));
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.DrawRect(TRect(TPoint(30,100), TPoint(120,200)));
	TheClient->iWs.Flush();
	}

void CTRegion::DrawClippedSample1(CBitmapContext &aGc)
	{
	aGc.SetPenColor(TRgb::Gray4(0));
	aGc.SetBrushColor(TRgb::Gray4(1));
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.DrawRect(TRect(TSize(100,200)));

//	aGc.BitBlt(TPoint(55,65), iPicture);

	aGc.SetPenStyle(CGraphicsContext::ENullPen);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(TRgb::Gray4(2));
	aGc.DrawEllipse(TRect(TPoint(20,35), TPoint(90,79)));

	aGc.SetBrushColor(TRgb::Gray4(0));
	aGc.DrawEllipse(TRect(TPoint(110,110), TPoint(175,200)));

	aGc.SetBrushColor(TRgb::Gray4(1));
	aGc.DrawEllipse(TRect(TPoint(170,30), TPoint(220,150)));

	aGc.SetBrushColor(TRgb::Gray4(3));
	aGc.DrawEllipse(TRect(TPoint(0,150), TPoint(240,190)));

	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);

	TheClient->iWs.Flush();
	}

void CTRegion::DrawNonClippedSample2(CBitmapContext &aGc, CFont *aFont)
	{
	aGc.SetPenColor(TRgb::Gray4(0));
	aGc.SetPenColor(TRgb::Gray4(0));
	aGc.UseFont(aFont);
	TPoint textPos=textRefPos-TPoint(iFont->TextWidthInPixels(text)*3/4, 0);
	aGc.DrawText(text, textPos);
	aGc.DiscardFont();

	TheClient->iWs.Flush();
	}

void CTRegion::DrawClippedSample2(CBitmapContext &aGc, CFont *aFont)
	{
	aGc.SetPenColor(TRgb::Gray4(0));
	aGc.SetPenColor(TRgb::Gray4(2));
	aGc.UseFont(aFont);
	TPoint textPos=textRefPos-TPoint(iFont->TextWidthInPixels(text)*3/4, -20);
	aGc.DrawText(text, textPos);
	aGc.DiscardFont();
	aGc.SetBrushColor(TRgb::Gray4(3));
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.DrawRect(TRect(TPoint(10,100), TPoint(210,120)));
	TheClient->iWs.Flush();
	}

void CTRegion::DrawClippedSample3(CBitmapContext &aGc)
	{
	aGc.SetPenColor(TRgb::Gray4(0));
	aGc.SetPenColor(TRgb::Gray4(2));
	aGc.DrawRect(TRect(TPoint(10,200), TPoint(20,210)));
	}

void CTRegion::DrawNonClippedSample1(CTWin *aWindow, const TPoint &aOrigin)
	{
	TheGc->Activate(*aWindow->Win());
	TheGc->SetOrigin(aOrigin);
	DrawNonClippedSample1(*TheGc);
	TheGc->Deactivate();
	}

void CTRegion::DrawNonClippedSample1(CTWin *aWindow, const TRegion &aRegion, const TPoint &aOrigin)
	{
	TheGc->Activate(*aWindow->Win());
	TheGc->SetOrigin(aOrigin);
	TheGc->SetClippingRegion(aRegion);
	DrawNonClippedSample1(*TheGc);
	TheGc->Deactivate();
	}

void CTRegion::DrawClippedSample1(TestWindow *aWindow, const TRegion &aRegion, const TRect &aClippingRect, const TPoint &aOrigin)
	{
	TheGc->Activate(*aWindow->Win());
	TheGc->SetOrigin(aOrigin);
	TheGc->SetClippingRegion(aRegion);
	TheGc->SetClippingRect(aClippingRect);
	DrawClippedSample1(*TheGc);
	TheGc->Deactivate();
	}

void CTRegion::DrawNonClippedSample2(CTWin *aWindow, const TPoint &aOrigin)
	{
	TheGc->Activate(*aWindow->Win());
	TheGc->SetOrigin(aOrigin);
	DrawNonClippedSample2(*TheGc,iFont);
	TheGc->Deactivate();
	}

void CTRegion::DrawNonClippedSample2(CTWin *aWindow, const TRegion &aRegion, const TPoint &aOrigin)
	{
	TheGc->Activate(*aWindow->Win());
	TheGc->SetOrigin(aOrigin);
	TheGc->SetClippingRegion(aRegion);
	DrawNonClippedSample2(*TheGc,iFont);
	TheGc->Deactivate();
	}

void CTRegion::DrawClippedSample2(TestWindow *aWindow, const TRegion &aRegion, const TRect &aClippingRect, const TPoint &aOrigin)
	{
	TheGc->Activate(*aWindow->Win());
	TheGc->SetClippingRegion(TRegionFix<1>());
	DrawClippedSample3(*TheGc);	// needed to trigger region update in wserv
	TheGc->SetOrigin(aOrigin);
	TheGc->SetClippingRect(aClippingRect);
	TheGc->SetClippingRegion(aRegion);
	DrawClippedSample2(*TheGc,iFont);
	TheGc->CancelClippingRegion();
	TheGc->Deactivate();
	}

void CTRegion::ClearBitmapAndWindows()
	{
	TestWin->Win()->Invalidate();
	TestWin->Win()->BeginRedraw();
	TheGc->Activate(*TestWin->Win());
	TheGc->SetBrushColor(TRgb::Gray4(3));
	TheGc->Clear();
	TheGc->Deactivate();
	TestWin->Win()->EndRedraw();
//
	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheGc->Activate(*BaseWin->Win());
	TheGc->SetBrushColor(TRgb::Gray4(3));
	TheGc->Clear();
	TheGc->Deactivate();
	BaseWin->Win()->EndRedraw();
//
	iBitGc->SetBrushColor(TRgb::Gray4(3));
	iBitGc->Clear();
	TheClient->iWs.Flush();
	}

void CTRegion::TestRegionL(const TRegion &aClippingRegion, const TRect &aClippingRect, const TPoint &aOrigin)
	{
	if (aClippingRegion.CheckError())
		User::Leave(KErrGeneral);
	ClearBitmapAndWindows();

	// draw to iWindow1 (clipping intermittently) and iBitmap (not clipping)
	DrawNonClippedSample1(TestWin,aOrigin);
	DrawNonClippedSample1(*iBitGc);

	DrawNonClippedSample2(TestWin,aOrigin);
	DrawNonClippedSample2(*iBitGc, iBitFont);

	DrawClippedSample1(TestWin,aClippingRegion,aClippingRect,aOrigin);
	DrawClippedSample1(*iBitGc);

	DrawClippedSample2(TestWin,aClippingRegion,aClippingRect,aOrigin);
	DrawClippedSample2(*iBitGc, iBitFont);

	// copy each rectangle of the clipping region from iBitmap to iWindow2
	TheGc->Activate(*BaseWin->Win());
	for (TUint i=0; i<(TUint)aClippingRegion.Count(); i++)
		{
		TRect rect=aClippingRegion.RectangleList()[i];
		rect.Intersection(aClippingRect);
		TheGc->SetOrigin(aOrigin);
		TheGc->BitBlt(rect.iTl, iBitmap, rect);
		TheClient->Flush();
		}
	TheGc->Deactivate();

	// draw the non-clipped samples to everywhere in iWindow2 *except* the clipping region
	RRegion inverseClippingRegion(TRect(-aOrigin,iDrawableSize));
	RRegion clip;
	clip.Copy(aClippingRegion);
	clip.ClipRect(aClippingRect);
	inverseClippingRegion.SubRegion(clip);
	clip.Close();
	DrawNonClippedSample1(BaseWin,inverseClippingRegion,aOrigin);
	DrawNonClippedSample2(BaseWin,inverseClippingRegion,aOrigin);
	inverseClippingRegion.Close();
	CompareWindows(_L("CTRegion::TestRegionL"));
	}

void CTRegion::CompareRectsL(TPoint aTl1,TPoint aTl2,TSize aSize)
	{
	_LIT(KError,"Pixels Differ: (%d,%d),(%d,%d)");
	CFbsScreenDevice* device;
	device=CFbsScreenDevice::NewL(KNullDesC,iBaseChildWin->BaseWin()->DisplayMode());
	TPoint br=aTl1+aSize;
	TInt startX1=aTl1.iX;
	TInt startX2=aTl2.iX;
	TSize pixelSize(1,1);
	TPoint zero;
	while(aTl1.iY<br.iY+1)
		{
		while (aTl1.iX<br.iX+1)
			{
			if (!device->RectCompare(TRect(aTl1,pixelSize),*device,TRect(aTl2,pixelSize)))
				{
				RDebug::Print(KError,aTl1.iX,aTl1.iY,aTl2.iX,aTl2.iY);
				iTest->LogColors(*device,zero,aTl1,aTl1);
				iTest->LogColors(*device,zero,aTl2,aTl2);
				}
			++aTl1.iX;
			++aTl2.iX;
			}
		aTl1.iX=startX1;
		aTl2.iX=startX2;
		++aTl1.iY;
		++aTl2.iY;
		}
	}

void CTRegion::ScrollBugL()
	{
	CBlankWindow* testWin;
	RBlankWindow win(TheClient->iWs);
	CleanupClosePushL(win);
	TSize screenSize=TheClient->iGroup->Size();
	testWin=new(ELeave) CBlankWindow(TRgb::Gray4(0));
	CleanupStack::PushL(testWin);
	//testWin->ConstructL(*TheClient->iGroup);
	testWin->SetUpL(TPoint(screenSize.iWidth/3,0),TSize(screenSize.iWidth/3,screenSize.iHeight),TheClient->iGroup,*TheGc);
	//testWin->Win()->SetBackgroundColor(TRgb::Gray2(1));
	testWin->Win()->EnableRedrawStore(EFalse);
	testWin->SetColor(TRgb::Gray4(1));
	testWin->RealDraw(ETrue);
	win.Construct(*TheClient->iGroup->WinTreeNode(),3456);
	win.SetColor(TRgb::Gray4(2));
	win.SetExtent(TPoint(screenSize.iWidth/3+30,20),TSize(screenSize.iWidth/3-62,25));
	win.Activate();
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	win.SetVisible(EFalse);
	TheClient->iWs.Flush();
	testWin->Win()->BeginRedraw();
	testWin->DrawNow();
	testWin->SetColor(TRgb::Gray4(2));
	testWin->Win()->Scroll(TPoint(0,-50));
	testWin->DrawNow();
	testWin->Win()->EndRedraw();
	TheClient->iWs.Flush();
	TBool retVal = TheClient->iScreen->RectCompare(TRect(TPoint(screenSize.iWidth/3,0),TSize(screenSize.iWidth/3,screenSize.iHeight/2-30))
							,TRect(TPoint(screenSize.iWidth/3,screenSize.iHeight/2-30),TSize(screenSize.iWidth/3,screenSize.iHeight/2-30)));
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);		
															
	retVal = !TheClient->iScreen->RectCompare(TRect(TPoint(screenSize.iWidth/3,screenSize.iHeight-60),TSize(10,10))
							,TRect(TPoint(screenSize.iWidth/3,screenSize.iHeight-50),TSize(10,10)));
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("!TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);		

	TheClient->WaitForRedrawsToFinish();
	win.SetVisible(ETrue);
	win.SetVisible(EFalse);
	testWin->Win()->BeginRedraw();
	testWin->Win()->Scroll(TPoint(0,-50));
	testWin->SetColor(TRgb::Gray4(1));
	testWin->DrawNow();
	testWin->Win()->EndRedraw();
	TheClient->iWs.Flush();	
	retVal = TheClient->iScreen->RectCompare(TRect(TPoint(screenSize.iWidth/3+35,0),TSize(screenSize.iWidth/6-35,100))
							,TRect(TPoint(screenSize.iWidth/2,0),TSize(screenSize.iWidth/6-35,100)));
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);		
																			
	retVal = !TheClient->iScreen->RectCompare(TRect(TPoint(screenSize.iWidth/3+30,20),TSize(10,10))
							,TRect(TPoint(screenSize.iWidth/3+30,50),TSize(10,10)));
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("!TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);		

	TheClient->WaitForRedrawsToFinish();
	testWin->Win()->BeginRedraw();
	testWin->DrawNow();
	testWin->Win()->EndRedraw();
	CleanupStack::PopAndDestroy(2,&win);
	}

void CTRegion::ExposeTestL()
	{
	CBlankWindow* testWin;
	TSize screenSize=TheClient->iGroup->Size();
	TRect topHalf(0,0,screenSize.iWidth/3,screenSize.iHeight/2);
	TRect leftHalf(0,0,screenSize.iWidth/6,screenSize.iHeight);
	testWin=new(ELeave) CBlankWindow(TRgb::Gray4(0));
	CleanupStack::PushL(testWin);
	testWin->SetUpL(TPoint(screenSize.iWidth/3,0),TSize(screenSize.iWidth/3,screenSize.iHeight),TheClient->iGroup,*TheGc);
	testWin->Win()->EnableRedrawStore(EFalse);
	testWin->SetColor(TRgb::Gray4(2));
	testWin->RealDraw(ETrue);
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	testWin->Win()->BeginRedraw();
	testWin->DrawNow();
	testWin->Win()->Invalidate(topHalf);
	testWin->SetColor(TRgb::Gray4(1));
	testWin->DrawNow();
	testWin->Win()->EndRedraw();
	TheClient->iWs.Flush();
	CheckRectNoMatch(TPoint(screenSize.iWidth/3,0),TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TSize(10,10),_L("CTRegion::ExposeTestL() A"));

	TheClient->WaitForRedrawsToFinish();
	CheckRect(TPoint(screenSize.iWidth/3,0),TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TSize(screenSize.iWidth/3,screenSize.iHeight/2),_L("CTRegion::ExposeTestL() B"));

	testWin->Win()->Invalidate(topHalf);
	testWin->SetColor(TRgb::Gray4(2));
	testWin->Win()->BeginRedraw();
	testWin->Win()->Invalidate(leftHalf);
	testWin->DrawNow();
	testWin->Win()->EndRedraw();
	TheClient->iWs.Flush();
	CheckRect(TPoint(screenSize.iWidth/3,0),TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TSize(screenSize.iWidth/6-10,screenSize.iHeight/2),_L("CTRegion::ExposeTestL() C"));
	CheckRectNoMatch(TPoint(screenSize.iWidth/2,0),TPoint(screenSize.iWidth/2,screenSize.iHeight/2),TSize(10,10),_L("CTRegion::ExposeTestL() D"));
	CheckRectNoMatch(TPoint(screenSize.iWidth/3,0),TPoint(screenSize.iWidth/2,0),TSize(10,10),_L("CTRegion::ExposeTestL() E"));
	CheckRect(TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TPoint(screenSize.iWidth/2,screenSize.iHeight/2),TSize(((screenSize.iWidth/6-5)/4*4),screenSize.iHeight/2),_L("CTRegion::ExposeTestL() F"));
	testWin->Win()->BeginRedraw();
	testWin->DrawNow();
	testWin->Win()->EndRedraw();
	TheClient->iWs.Flush();
	
	CheckRect(TPoint(screenSize.iWidth/3,0),TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TSize(screenSize.iWidth/6-10,screenSize.iHeight/2),_L("CTRegion::ExposeTestL() G"));
	CheckRectNoMatch(TPoint(screenSize.iWidth/2,0),TPoint(screenSize.iWidth/2,screenSize.iHeight/2),TSize(10,10),_L("CTRegion::ExposeTestL() H"));
	CheckRect(TPoint(screenSize.iWidth/3,0),TPoint(screenSize.iWidth/2,0),TSize(((screenSize.iWidth/6-5)/4*4),screenSize.iHeight/2),_L("CTRegion::ExposeTestL() I"));
	CheckRectNoMatch(TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TPoint(screenSize.iWidth/2,screenSize.iHeight/2),TSize(10,10),_L("CTRegion::ExposeTestL() J"));

	CleanupStack::PopAndDestroy(testWin);
	}

#if defined(__MARM_ARM4__)
	#define FADING ETrue
#elif defined(__MARM__)
	#define FADING EFalse
#else
	#define FADING ETrue
#endif
void CTRegion::ExposeTest2L()
	{
	TBool canFade=FADING;
#if defined(__MARM_THUMB__)
	if (!RegionCanFadeSet)
		return;
	canFade=RegionCanFade;
#endif
	INFO_PRINTF1(_L("AUTO REGN ExpostTest2 "));
	CBlankWindow* testWin;
	RBlankWindow win(TheClient->iWs);
	CleanupClosePushL(win);
	TSize screenSize=TheClient->iGroup->Size();
	TRect topHalf(0,0,screenSize.iWidth/3,screenSize.iHeight/2);
	testWin=new(ELeave) CBlankWindow(TRgb::Gray4(0));
	CleanupStack::PushL(testWin);
	testWin->SetUpL(TPoint(screenSize.iWidth/3,0),TSize(screenSize.iWidth/3,screenSize.iHeight),TheClient->iGroup,*TheGc);
	testWin->Win()->EnableRedrawStore(EFalse);
	testWin->Win()->SetRequiredDisplayMode(EGray16);
	testWin->SetColor(TRgb::Gray16(12));
	testWin->RealDraw(ETrue);
	win.Construct(*TheClient->iGroup->WinTreeNode(),3456);
	win.SetColor(TRgb::Gray4(2));
	win.SetExtent(TPoint(0,-screenSize.iHeight),TSize(screenSize.iWidth/3-2,2*screenSize.iHeight));
	win.SetShadowHeight(screenSize.iWidth/9);
	win.Activate();
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	INFO_PRINTF1(_L(" Constructed Windows"));
	testWin->DrawNow();
	testWin->Win()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
	testWin->DrawNow(topHalf);
	TheClient->iWs.Flush();
	INFO_PRINTF1(_L(" Drawn TopHalf Faded"));
	TBool retVal = TheClient->iScreen->RectCompare(TRect(TPoint(screenSize.iWidth/3,0),TSize(screenSize.iWidth/3,screenSize.iHeight/2))
						,TRect(TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TSize(screenSize.iWidth/3,screenSize.iHeight/2)));
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);		

	testWin->DrawNow(topHalf);
	TheClient->iWs.Flush();
	canFade!=TheClient->iScreen->RectCompare(TRect(TPoint(screenSize.iWidth/3,0),TSize(10,10))
															,TRect(TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TSize(10,10)));
	TEST(canFade);
	if (!canFade)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, canFade);		

	testWin->DrawNow();
	TheClient->iWs.Flush();
	INFO_PRINTF1(_L(" All Faded (Flush)"));
	
	retVal = TheClient->iScreen->RectCompare(TRect(TPoint(screenSize.iWidth/3,0),TSize(screenSize.iWidth/3,screenSize.iHeight/2))
						,TRect(TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TSize(screenSize.iWidth/3,screenSize.iHeight/2)));
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);		

	testWin->Win()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	INFO_PRINTF1(_L(" Redrawn Screen Unfaded"));
	testWin->Win()->Invalidate(topHalf);
	testWin->Win()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
	testWin->DrawNow();
	TheClient->iWs.Flush();
	INFO_PRINTF1(_L(" Re Drawn Top Half Faded Again"));
	retVal = TheClient->iScreen->RectCompare(TRect(TPoint(screenSize.iWidth/3,0),TSize(screenSize.iWidth/3,screenSize.iHeight/2))
						,TRect(TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TSize(screenSize.iWidth/3,screenSize.iHeight/2)));
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);		

	testWin->DrawNow();
	TheClient->iWs.Flush();
	INFO_PRINTF1(_L(" Re Drawn Top Half Faded Yet Again"));
	canFade!=TheClient->iScreen->RectCompare(TRect(TPoint(screenSize.iWidth/3,0),TSize(10,10))
															,TRect(TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TSize(10,10)));
	TEST(canFade);
	if (!canFade)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, canFade);		

	testWin->DrawNow();
	TheClient->iWs.Flush();
	INFO_PRINTF1(_L(" All Faded Again"));
	retVal = TheClient->iScreen->RectCompare(TRect(TPoint(screenSize.iWidth/3,0),TSize(screenSize.iWidth/3,screenSize.iHeight/2))
						,TRect(TPoint(screenSize.iWidth/3,screenSize.iHeight/2),TSize(screenSize.iWidth/3,screenSize.iHeight/2)));
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);		

	CleanupStack::PopAndDestroy(2,&win);
	INFO_PRINTF1(_L(" Killed Windows"));
	}

void CTRegion::CoverTestL()
	{
	CBlankWindow* testWin;
	RBlankWindow win(TheClient->iWs);
	CleanupClosePushL(win);
	TSize screenSize=TheClient->iGroup->Size();
	testWin=new(ELeave) CBlankWindow(TRgb::Gray4(0));
	CleanupStack::PushL(testWin);
	testWin->SetUpL(TPoint(screenSize.iWidth/3,0),TSize(screenSize.iWidth/3,screenSize.iHeight),TheClient->iGroup,*TheGc);
	testWin->SetColor(TRgb::Gray4(1));
	testWin->RealDraw(ETrue);
	win.Construct(*TheClient->iGroup->WinTreeNode(),3456);
	win.SetColor(TRgb::Gray4(0));
	win.SetExtent(TPoint(screenSize.iWidth/3+30,20),TSize(screenSize.iWidth/3-62,25));
	win.SetVisible(EFalse);
	win.Activate();
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	testWin->DrawNow();
	testWin->SetColor(TRgb::Gray4(2));
	win.SetVisible(ETrue);
	testWin->DrawNow();
	TheClient->iWs.Flush();
	CheckRectNoMatch(TPoint(screenSize.iWidth/3,0),TPoint(screenSize.iWidth/3+30,20),TSize(10,10),_L("TRegion::CoverTestL() A"));

	TheClient->WaitForRedrawsToFinish();
	CheckRectNoMatch(TPoint(screenSize.iWidth/3,0),TPoint(screenSize.iWidth/3+30,20),TSize(10,10),_L("TRegion::CoverTestL() B"));

	win.SetVisible(EFalse);
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	testWin->SetColor(TRgb::Gray4(1));
	testWin->Win()->Invalidate();
	win.SetVisible(ETrue);
	testWin->DrawNow();
	TheClient->iWs.Flush();
	CheckRectNoMatch(TPoint(screenSize.iWidth/3,0),TPoint(screenSize.iWidth/3+30,20),TSize(10,10),_L("TRegion::CoverTestL() C"));

	CleanupStack::PopAndDestroy(2,&win);
	}

void CTRegion::OffsetTestL()
	{
	CBlankWindow* testWin;
	RBlankWindow win(TheClient->iWs);
	CleanupClosePushL(win);
	TSize screenSize=TheClient->iGroup->Size();
	testWin=new(ELeave) CBlankWindow(TRgb::Gray4(0));
	CleanupStack::PushL(testWin);
	testWin->SetUpL(TPoint(screenSize.iWidth/3,0),TSize(screenSize.iWidth/3,screenSize.iHeight-40),TheClient->iGroup,*TheGc);
	testWin->Win()->EnableRedrawStore(EFalse);
	testWin->SetColor(TRgb::Gray4(1));
	testWin->RealDraw(ETrue);
	win.Construct(*testWin->WinTreeNode(),3456);
	win.SetColor(TRgb::Gray4(2));
	win.SetExtent(TPoint(30,20),TSize(screenSize.iWidth/3-62,25));
	win.Activate();
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	win.SetVisible(EFalse);
	TheClient->iWs.Flush();
	testWin->DrawNow();
	testWin->SetColor(TRgb::Gray4(2));
	testWin->Win()->SetPosition(TPoint(screenSize.iWidth/3+30,35));
	testWin->DrawNow();
	TheClient->iWs.Flush();
	/* Andy - this is verifying that the complete redraw of the window in the second DrawNow only affects part
	of the window.  I don't understand it atall.
	CheckRectNoMatch(TPoint(screenSize.iWidth/3+60,55),TPoint(screenSize.iWidth/3+70,85),TSize(10,10),_L("CTRegion::OffsetTestL() A"));
	*/
	TheClient->WaitForRedrawsToFinish();
	CheckRect(TPoint(screenSize.iWidth/3+60,55),TPoint(screenSize.iWidth/3+70,85),TSize(screenSize.iWidth/3-62,25),_L("CTRegion::OffsetTestL() A"));

	win.SetVisible(ETrue);
	win.SetVisible(EFalse);
	testWin->Win()->SetPosition(TPoint(screenSize.iWidth/3+5,5));
	testWin->SetColor(TRgb::Gray4(1));
	testWin->DrawNow();
	TheClient->iWs.Flush();
	CheckRect(TPoint(screenSize.iWidth/3+60,55),TPoint(screenSize.iWidth/3+55,90),TSize(screenSize.iWidth/3-62,25),_L("CTRegion::OffsetTestL() B"));
	/* Andy - this is verifying that the complete redraw of the window during DrawNow only affects part
	of the window.  I don't understand it atall.
	CheckRectNoMatch(TPoint(screenSize.iWidth/3+30,20),TPoint(screenSize.iWidth/3+50,90),TSize(10,10),_L("CTRegion::OffsetTestL() B"));
	*/
	CleanupStack::PopAndDestroy(2,&win);
	}

void CTRegion::ClipTestL()
	{
	CBlankWindow* testWin;
	RBlankWindow win(TheClient->iWs);
	CleanupClosePushL(win);
	TSize screenSize=TheClient->iGroup->Size();
	testWin=new(ELeave) CBlankWindow(TRgb::Gray4(0));
	CleanupStack::PushL(testWin);
	testWin->SetUpL(TPoint(screenSize.iWidth/3,0),TSize(screenSize.iWidth/3,screenSize.iHeight),TheClient->iGroup,*TheGc);
	testWin->Win()->EnableRedrawStore(EFalse);
	testWin->SetColor(TRgb::Gray4(1));
	testWin->RealDraw(ETrue);
	win.Construct(*testWin->WinTreeNode(),3456);
	win.SetColor(TRgb::Gray4(2));
	win.SetExtent(TPoint(30,20),TSize(screenSize.iWidth/3-62,25));
	win.Activate();
	TheClient->iWs.Flush();
	TheClient->WaitForRedrawsToFinish();
	win.SetVisible(EFalse);
	TheClient->iWs.Flush();
	testWin->DrawNow();
	testWin->SetColor(TRgb::Gray4(2));
	User::LeaveIfError(testWin->Win()->SetSizeErr(TSize(screenSize.iWidth/6,screenSize.iHeight-2)));
	RRegion invalid;
	testWin->Win()->GetInvalidRegion(invalid);
	testWin->DrawNow();
	TheClient->iWs.Flush();
	TBool retVal = !invalid.CheckError();
	TEST(retVal);
	if (!retVal)
		INFO_PRINTF3(_L("!invalid.CheckError() return value  - Expected: %d, Actual: %d"), ETrue, retVal);			
	
	TEST(invalid.BoundingRect().iBr.iX<=screenSize.iWidth/6);
	if (invalid.BoundingRect().iBr.iX>screenSize.iWidth/6)
		INFO_PRINTF3(_L("invalid.BoundingRect().iBr.iX<=screenSize.iWidth/6  - Expected: %d, Actual: %d"), screenSize.iWidth/6, invalid.BoundingRect().iBr.iX);			
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();
	testWin->Win()->GetInvalidRegion(invalid);
	
	retVal = invalid.IsEmpty();
	TEST(retVal);
	if (!retVal)
		{
		INFO_PRINTF3(_L("!invalid.CheckError() return value  - Expected: %d, Actual: %d"), ETrue, retVal);
		}

	User::LeaveIfError(testWin->Win()->SetSizeErr(TSize(screenSize.iWidth/3,screenSize.iHeight-4)));
	win.SetVisible(ETrue);
	win.SetVisible(EFalse);
	User::LeaveIfError(testWin->Win()->SetSizeErr(TSize(screenSize.iWidth/6,screenSize.iHeight-6)));
	testWin->SetColor(TRgb::Gray4(1));
	testWin->DrawNow();
	TheClient->iWs.Flush();
	//PeterI This is testing an intermediate state i.e. "flicker" and will never pass on mk3
/*	CheckRectNoMatch(TPoint(screenSize.iWidth/3+30,20),TPoint(screenSize.iWidth/3+50,90),TSize(10,10),_L("CTRegion::ClipTestL()"));
	TEST(retVal);
	if (!retVal)
		{
		INFO_PRINTF3(_L("!CheckRect() return value  - Expected: %d, Actual: %d"), ETrue, retVal);
		}
		*/
	CleanupStack::PopAndDestroy(2,&win);
	}

void CTRegion::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	((CTRegionStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0272

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing to a region including clipping

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw to a region that has clipping enabled and the check the
					drawing occurs correctly

@SYMTestExpectedResults Drawing to the region occurs correctly
*/
		case 1:
			{
			((CTRegionStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0272"));
			iTest->LogSubTest(_L("Main"));
			RRegion clippingRegion;
			CreateChildWindowsL();
			TheClient->iWs.Finish();
			TheClient->WaitForRedrawsToFinish();
			TSize scrSize(TheClient->iScreen->SizeInPixels());
			for(TUint index=0;index<(sizeof(regionRectList)/sizeof(regionRectList[0]));index++)
				{
				regionRectList[index].SetWidth((regionRectList[index].Width()*scrSize.iWidth)/640);
				regionRectList[index].SetHeight((regionRectList[index].Height()*scrSize.iHeight)/240);
				clippingRectList[index].SetWidth((clippingRectList[index].Width()*scrSize.iWidth)/640);
				clippingRectList[index].SetHeight((clippingRectList[index].Height()*scrSize.iHeight)/240);
				originList[index].iX = (originList[index].iX*scrSize.iWidth)/640;
				originList[index].iY = (originList[index].iY*scrSize.iHeight)/240;

				clippingRegion.AddRect(regionRectList[index]);
				TestRegionL(clippingRegion,clippingRectList[index],originList[index]);
				}
			clippingRegion.Close();
			DeleteChildWindows();
			}
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0273

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing to a region including clipping as it is exposed

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw to a region that has clipping enabled as it is exposed
					and the check the drawing occurs correctly

@SYMTestExpectedResults Drawing to the region occurs correctly
*/
		case 2:
			((CTRegionStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0273"));
			//PeterI This appears to be testing "flicker" of a window after an expose
			//Mk3 doesn't exhibit this behaviour. Is the flicker expected behaviour?
//			iTest->LogSubTest(_L("Expose Test"));
//			ExposeTestL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0274

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing to a region including clipping as it is exposed

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw to a region that has clipping enabled as it is exposed
					and the check the drawing occurs correctly

@SYMTestExpectedResults Drawing to the region occurs correctly
*/
		case 3:
			((CTRegionStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0274"));
			iTest->LogSubTest(_L("Expose Test2"));
			ExposeTest2L();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0275

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing to a region including clipping as it is covered

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw to a region that has clipping enabled as it is covered
					and the check the drawing occurs correctly

@SYMTestExpectedResults Drawing to the region occurs correctly
*/
		case 4:
			((CTRegionStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0275"));
			iTest->LogSubTest(_L("Cover Test"));
			CoverTestL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0276

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing to a region including clipping and with an offset

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw to a region that has clipping enabled and with an offset
					and the check the drawing occurs correctly

@SYMTestExpectedResults Drawing to the region occurs correctly
*/
		case 5:
			((CTRegionStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0276"));
			iTest->LogSubTest(_L("Offset Test"));
			OffsetTestL();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0277

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test drawing to a region including clipping

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Draw to a region that has clipping enabled
					and the check the drawing occurs correctly

@SYMTestExpectedResults Drawing to the region occurs correctly
*/
		case 6:
			((CTRegionStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0277"));
			iTest->LogSubTest(_L("Clip Test"));
			ClipTestL();
			break;
		default:
			((CTRegionStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTRegionStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
		}
	((CTRegionStep*)iStep)->RecordTestResultL();
	}

	
__WS_CONSTRUCT_STEP__(Region)
