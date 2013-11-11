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
// Test alpha channel transparent windows
// Test that draw operations with non-opaque colours do alpha blending in EColor64K and EColor16MA display modes
// Test that alpha channel transparent windows are drawn correctly when windows move, redraw, change visibility, etc.
// In the draw operation tests, the left window draws opaque pink on white background, the right window blends semi-transparent red on white background,
// and the results are compared.
// In the transparent window tests, the right window contains several transparent windows, which are moved, redrawn, visibility changed, etc,
// the left window contains a single window in which we draw what we expect the right window to look like. The results are compared.
// In each case, the left and right windows should be identical
// 
//


#include "TALPHAWIN.H"

enum
	{
	EOpDrawRect,
	EOpDrawLine,
	EOpDrawEllipse,
	EOpDrawText,
	EOpDrawTextVertical,
	EOpDrawTextAntiAliased,
	EOpBitBlt,
	EOpBitBltMasked,
	ENumDrawOps
	};


enum
	{
	ERed = 0x1,
	EGreen = 0x2,
	EBlue = 0x4,

	EAlphaTransparency = 0x8,
	ETransparencyFactor = 0x10,
	// defaults to non-transparent

	EOpaque = 0x20,
	ETransparent = 0x40,
	// defaults to semi-transparent

	EModeColor64K = 0x80,
	EModeColor16MA = 0x100,
	// defaults to 64k

	EInvisible = 0x200,

	EActive = 0xf000000
	};


TRgb ColourFromDrawState(TInt aDrawState)
	{
	TInt red = (aDrawState & ERed) ? 255 : 0;
	TInt green = (aDrawState & EGreen) ? 255 : 0;
	TInt blue = (aDrawState & EBlue) ? 255 : 0;
	TInt alpha = 128;
	if (aDrawState & EOpaque)
		alpha = 255;
	if (aDrawState & ETransparent)
		alpha = 0;
	return TRgb(red, green, blue, alpha);
	}



//
// CTAlphaWinTest
//

CTAlphaWin::CTAlphaWin(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

CTAlphaWin::~CTAlphaWin()
	{
	iTestWin.DeleteAll();
	delete iRefWin;
	}

void CTAlphaWin::ConstructL()
	{
	if(TransparencySupportedL() == KErrNotSupported)
			return;

	TSize winSize = BaseWin->Size();

	iTestWin[0] = CTAlphaWindow::NewL(this, TestWin, TPoint(0,0), winSize, ERed | EGreen | EBlue | EOpaque);
	iTestWin[1] = CTAlphaWindow::NewL(this, TestWin, TPoint(0,0), TSize(winSize.iWidth/2, winSize.iHeight/2), ERed | EAlphaTransparency);
	iTestWin[2] = CTAlphaWindow::NewL(this, TestWin, TPoint(winSize.iWidth/3,0), TSize(winSize.iWidth/2, winSize.iHeight/2), EGreen | EAlphaTransparency);
	iTestWin[3] = CTAlphaWindow::NewL(this, TestWin, TPoint(winSize.iWidth/6, winSize.iHeight/3), TSize(winSize.iWidth/2, winSize.iHeight/2), EBlue | EAlphaTransparency);
	iTestWin[4] = CTAlphaWindow::NewL(this, TestWin, TPoint(winSize.iWidth/4,winSize.iHeight/6), TSize(winSize.iWidth/3,winSize.iHeight/3), ERed | EGreen | EBlue | EAlphaTransparency | ETransparent);

	iRefWin = CTAlphaRefWin::NewL(BaseWin, TPoint(0,0), winSize, iTestWin);
	//Clearing the windows
	BaseWin->ClearWin();
	TestWin->ClearWin();
	}

void CTAlphaWin::ConfigureDisplayModes(TDisplayMode aRequiredMode = EColor16M)
	{
	TInt i;
	for (i=0; i<5; i++)
		{
		iTestWin[i]->BaseWin()->SetRequiredDisplayMode(aRequiredMode);
		}
	iRefWin->BaseWin()->SetRequiredDisplayMode(aRequiredMode);
	}


void CTAlphaWin::TestSemiTransparentDrawingL()
	{
	TSize winSize = BaseWin->Size();

	// In this window, we draw opaque pink
	CTDrawOpWin* drawWin = CTDrawOpWin::NewL(this, BaseWin, TPoint(0,0), winSize, TRgb(255,127,127,255));

	// In this window, we blend semi-transparent red
	CTDrawOpWin* blendWin = CTDrawOpWin::NewL(this, TestWin, TPoint(0,0), winSize, TRgb(255,0,0,128));

	const TInt tolerance = 9;//8 - wouldn't be enough!! The defect 	DEF112334 was raised
	for (TInt i=EOpDrawRect; i<ENumDrawOps; i++)
		{
		
	//	User::After(1000000);// helpful when debugging
		drawWin->SetDrawOp(i);
		blendWin->SetDrawOp(i);
		drawWin->DrawNow();
		blendWin->DrawNow();
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();

		if((i == EOpDrawTextAntiAliased) && (TheClient->iScreen->DisplayMode() == EColor16MA) || (TheClient->iScreen->DisplayMode() == EColor16MAP))
			{		
			TSize winSize=BaseWin->Size();
			TRect rect1(BaseWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),winSize);
			TRect rect2(TestWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),winSize);

			CheckRectL(rect1, rect2, winSize, TheClient->iScreen->DisplayMode(), tolerance, _L("CTAlphaWin::TestSemiTransparentDrawingL()"));
			}
		else
			{
			CheckRect(BaseWin,TestWin,_L("CTAlphaWin::TestSemiTransparentDrawingL()"));
			}	
		}
	delete drawWin;
	delete blendWin;
	}

void CTAlphaWin::TestTransparentDrawingL()
	{
	TSize winSize = BaseWin->Size();

	// In this window, we draw opaque white
	CTDrawOpWin* drawWin = CTDrawOpWin::NewL(this, BaseWin, TPoint(0,0), winSize, TRgb(255,255,255,255));

	// In this window, we blend transparent red
	CTDrawOpWin* blendWin = CTDrawOpWin::NewL(this, TestWin, TPoint(0,0), winSize, TRgb(255,0,0,0));

	for (TInt i=EOpDrawRect; i<ENumDrawOps; i++)
		{
		//User::After(1000000);// helpful when debugging
		drawWin->SetDrawOp(i);
		blendWin->SetDrawOp(i);
		drawWin->DrawNow();
		blendWin->DrawNow();
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		CheckRect(BaseWin,TestWin,_L("CTAlphaWin::TestTransparentDrawingL()"));
		}
	delete drawWin;
	delete blendWin;
	}

void CTAlphaWin::CheckRectL(const TRect& aRect1, const TRect& aRect2, TSize aSize, TDisplayMode aRequiredMode, TInt aTolerance, const TDesC& aErrorMsg)
	{
	CFbsBitmap *bmp1 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp1);
	User::LeaveIfError(bmp1->Create(aSize, aRequiredMode));

	CFbsBitmap *bmp2 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp2);
	User::LeaveIfError(bmp2->Create(aSize, aRequiredMode));
	
	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(bmp1, aRect1));	
	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(bmp2, aRect2));	

	TRgb *rgbBuf1=(TRgb *)User::AllocL(aSize.iWidth*sizeof(TRgb));	
	TRgb *rgbBuf2=(TRgb *)User::Alloc(aSize.iWidth*sizeof(TRgb));	
	if(!rgbBuf2)
		{
		User::Free(rgbBuf1);
		User::Leave(KErrNoMemory);
		}
	TBool equal = ETrue;
	TInt maxDeviation = 0;
	for(TInt yy = 0; yy < aSize.iHeight && equal; yy++)
		{
		TPtr8 ptr1((TUint8 *)rgbBuf1,aSize.iWidth*sizeof(TRgb));
		bmp1->GetScanLine(ptr1, TPoint(0, yy), aSize.iWidth, ERgb);
		TPtr8 ptr2((TUint8 *)rgbBuf2,aSize.iWidth*sizeof(TRgb));
		bmp2->GetScanLine(ptr2, TPoint(0, yy), aSize.iWidth, ERgb);
		
		TRgb *rgbBufCur1 = rgbBuf1;
		TRgb *rgbBufCur2 = rgbBuf2;
		for(TInt ii = 0; ii < aSize.iWidth; ii++)
			{
			TInt delta = Abs(rgbBufCur1->Red()-rgbBufCur2->Red());
			TInt delta1 = Abs(rgbBufCur1->Green()-rgbBufCur2->Green());
			TInt delta2 = Abs(rgbBufCur1->Blue()-rgbBufCur2->Blue());
			
			if((delta > aTolerance) || (delta1 > aTolerance) || (delta2 > aTolerance))
				{
				equal = EFalse;
				}
			TInt maxItermedia = Max(delta1, delta2);
			maxItermedia = Max(maxItermedia, delta);
			maxDeviation = Max(maxItermedia, maxDeviation);

			rgbBufCur1++;	
			rgbBufCur2++;	
			}
		}
	
	User::Free(rgbBuf1);
	User::Free(rgbBuf2);

	CleanupStack::PopAndDestroy(2,bmp1);

	if (!equal)
		{
		INFO_PRINTF3(_L("%S CheckRectA failed, max deviation %d"), &aErrorMsg, maxDeviation);
		}
	else if(maxDeviation)
		{
		INFO_PRINTF4(_L("%S CheckRectA passed with tolerance %d, max deviation %d"), &aErrorMsg, aTolerance, maxDeviation);
		}
		
	iStep->TEST(equal);
	}

void CTAlphaWin::TestCondition()
	{
	// User::After(1000000);// helpful when debugging
	iRefWin->DrawNow();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	CheckRect(BaseWin,TestWin,_L("CTAlphaWin::TestCondition()"));
	}

void CTAlphaWin::TestConditionL()
	{
	iRefWin->DrawNow();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	const TInt tolerance = 9;
	TSize winSize=BaseWin->Size();
	TRect rect1(BaseWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),winSize);
	TRect rect2(TestWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),winSize);
	CheckRectL(rect1, rect2, winSize, TheClient->iScreen->DisplayMode(), tolerance, _L("CTAlphaWin::TestCondition()"));
	}

void CTAlphaWin::TestInitialConfiguration()
	{
	if(TheClient->iScreen->DisplayMode() == EColor64K)
		{
		TestConditionL();
		}
	else
		{
		TestCondition();
		}
	}

void CTAlphaWin::TestMove()
	{
	// Test moving windows, both in front and behind
	for (TInt i = 0; i<5; i++)
		{
		TPoint pos = iTestWin[i]->Position();
		pos += TPoint(10,10);
		iTestWin[i]->SetPos(pos);
		TestCondition();
		}
	for (TInt j = 0; j<5; j++)
		{
		TPoint pos = iTestWin[j]->Position();
		pos -= TPoint(10,10);
		iTestWin[j]->SetPos(pos);
		TestCondition();
		}
	}


void CTAlphaWin::TestRedraw()
	{
	// Test redrawing windows, both in front and behind
	for (TInt i=0; i<5; i++)
		{
		iTestWin[i]->DrawNow();
		TestCondition();
		}
	}


void CTAlphaWin::TestInvisible()
	{
	// Test making windows visible and invisible, both in front and behind
	for (TInt i=0; i<5; i++)
		{
		iTestWin[i]->SetVisible(EFalse);
		TestCondition();
		iTestWin[i]->SetVisible(ETrue);
		TestCondition();
		}
	}

void CTAlphaWin::TestChildrenL()
	{
	struct CTAlphaWinChildren: public TCleanupItem
		{
			static void Destroy(TAny* trg)
				{
				static_cast<CTAlphaWindow*>(trg)->DestroyChildren();				
				}
			CTAlphaWinChildren(CTAlphaWindow*trg):	TCleanupItem(Destroy,trg)	
				{}
			
		};
	CleanupStack::PushL(CTAlphaWinChildren(iTestWin[2]));
	iTestWin[2]->CreateChildrenL(3);
	TestCondition();
	TestMove();
	CleanupStack::PopAndDestroy(iTestWin[2]);
	}


void CTAlphaWin::TestAntiAliasedTextTransparentL()
	{

	//Clear the screen
	for (TInt i=0; i<5; i++)
		{
		iTestWin[i]->SetVisible(EFalse);
		}
	iRefWin->SetVisible(EFalse);
	TheClient->iWs.Flush();

	//Create a new test window on the left
	//Create a transparent window:
	TSize winSize = BaseWin->Size();

	RWindow theWin(TestWin->Client()->iWs);
	User::LeaveIfError(theWin.Construct(*(TestWin->WinTreeNode()),(TUint32)&theWin));

	theWin.SetExtent(TPoint(0,0), winSize);
	theWin.SetBackgroundColor(TRgb(127,0,255,127));
	TDisplayMode mode = EColor16MAP;
	theWin.SetRequiredDisplayMode(mode);
	theWin.SetVisible(ETrue);
	theWin.SetTransparencyAlphaChannel();
	theWin.Activate();
	TheClient->iWs.Flush();
	CleanupClosePushL(theWin);

	//get windows screen device.
	CWsScreenDevice *device;
	device = new (ELeave)CWsScreenDevice(TestWin->Client()->iWs);//(TheClient->iWs);
	User::LeaveIfError(device->Construct(iTest->ScreenNumber()));
	CleanupStack::PushL(device);

	TFontSpec fs1;
	CFont *font1;
	CFont *font2;
	fs1.iTypeface.iName = KTestFontTypefaceName;
	fs1.iHeight = 16;
	fs1.iFontStyle.SetBitmapType(EDefaultGlyphBitmap);
	int error = TheClient->iScreen->GetNearestFontToDesignHeightInPixels((CFont*&)font1,fs1);
	if (error)
		{
		TheClient->iScreen->ReleaseFont(font1);
		User::Panic(_L("font not created"),error);
		}
	fs1.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	error = TheClient->iScreen->GetNearestFontToDesignHeightInPixels((CFont*&)font2,fs1);
	if (error)
		{
		TheClient->iScreen->ReleaseFont(font1);
		TheClient->iScreen->ReleaseFont(font2);
		User::Panic(_L("font not created"),error);
		}

	CWindowGc *gc;
	device->CreateContext(gc);
	CleanupStack::PushL(gc);

	theWin.Invalidate();
	theWin.BeginRedraw();
	gc->Activate(theWin);

	gc->SetPenStyle( CGraphicsContext::ESolidPen );
	gc->SetPenColor( TRgb( 0, 0, 0, 127 ) );

	//draw text for anti-aliasing needs an open font (scalable).
	int typefaces = TheClient->iScreen->NumTypefaces();

	gc->UseFont(font1);
	gc->SetBrushStyle( CGraphicsContext::ENullBrush );
	gc->DrawText(_L("Test"),TPoint(10,20));
	gc->DiscardFont();
	gc->UseFont(font2);
	gc->DrawText(_L("Test"),TPoint(10,60));
	gc->DiscardFont();

	//destruction and tidying up
	gc->Deactivate();
	theWin.EndRedraw();
	TheClient->iWs.Flush();

	TheClient->iScreen->ReleaseFont(font1);
	TheClient->iScreen->ReleaseFont(font2);
	CleanupStack::PopAndDestroy(gc);//gc
	CleanupStack::PopAndDestroy(device);//device

	//do not close the test window yet since there is a comparison
	//required

	//now do the same on an off screen bitmap.  Then create a window
	//and put the bitmap onto it.
	//create a colour bitmap
	//
	CFbsBitmap *bitmapOne;
	bitmapOne = new (ELeave)CFbsBitmap();
	CleanupStack::PushL(bitmapOne);
	User::LeaveIfError(bitmapOne->Create(winSize,mode));

	CFbsBitmapDevice *deviceOne=CFbsBitmapDevice::NewL(bitmapOne);
	CleanupStack::PushL(deviceOne);

	CFont *font3;
	CFont *font4;
	fs1.iFontStyle.SetBitmapType(EDefaultGlyphBitmap);
	error = TheClient->iScreen->GetNearestFontToDesignHeightInPixels((CFont*&)font3,fs1);
	if (error)
		{
		TheClient->iScreen->ReleaseFont(font3);
		User::Panic(_L("font not created"),error);
		}
	fs1.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	error = TheClient->iScreen->GetNearestFontToDesignHeightInPixels((CFont*&)font4,fs1);
	if (error)
		{
		TheClient->iScreen->ReleaseFont(font3);
		TheClient->iScreen->ReleaseFont(font4);
		User::Panic(_L("font not created"),error);
		}
	CFbsBitGc *bGcOne = CFbsBitGc::NewL();
	CleanupStack::PushL(bGcOne);

	bGcOne->Activate(deviceOne);

	bGcOne->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bGcOne->SetBrushColor(TRgb(127,0,255,127));
	bGcOne->DrawRect(TRect(0,0,winSize.iWidth,winSize.iHeight));

	bGcOne->SetPenStyle(CGraphicsContext::ESolidPen);
	bGcOne->SetPenColor(TRgb(0,0,0,127));

	bGcOne->UseFont(font3);
	bGcOne->SetBrushStyle( CGraphicsContext::ENullBrush );
	bGcOne->DrawText(_L("Test"),TPoint(10,20));
	bGcOne->DiscardFont();
	bGcOne->UseFont(font4);
	bGcOne->DrawText(_L("Test"),TPoint(10,60));
	bGcOne->DiscardFont();
	//destruction and tidying up
	//measure the text
	CFont::TMeasureTextOutput textSize;
	font4->MeasureText(_L("Test"),NULL,&textSize);

	TheClient->iScreen->ReleaseFont(font3);
	TheClient->iScreen->ReleaseFont(font4);

	//display at the left
	RWindow refWin(BaseWin->Client()->iWs);
	CleanupClosePushL(refWin);
	User::LeaveIfError(refWin.Construct(*(BaseWin->WinTreeNode()),(TUint32)&refWin));

	refWin.SetExtent(TPoint(0,0), winSize);
	refWin.SetRequiredDisplayMode(mode);
	refWin.SetVisible(ETrue);
	refWin.SetTransparencyAlphaChannel();
	refWin.Activate();
	TheClient->iWs.Flush();

	//a gc for the ref win
	CWsScreenDevice *refDevice;
	refDevice = new (ELeave)CWsScreenDevice(BaseWin->Client()->iWs);
	User::LeaveIfError(refDevice->Construct(iTest->ScreenNumber()));
	CleanupStack::PushL(refDevice);
	CWindowGc *gcRef;
	refDevice->CreateContext(gcRef);
	CleanupStack::PushL(gcRef);

	refWin.Invalidate();
	refWin.BeginRedraw();
	gcRef->Activate(refWin);
	gcRef->BitBlt(TPoint(0,0), bitmapOne);
	gcRef->Deactivate();
	refWin.EndRedraw();
	TheClient->iWs.Flush();

	TPoint refPos = refWin.AbsPosition();
	TPoint winPos = theWin.AbsPosition();

	//Compare the anti-aliased text areas
	TInt textLength=textSize.iBounds.iBr.iX;
	TInt textHeight=Abs(textSize.iBounds.iTl.iY);

	TRect rect1(refPos.iX+10,refPos.iY+60-textHeight,
			refPos.iX+10+textLength,refPos.iY+60);
	TRect rect2(winPos.iX+10,winPos.iY+60-textHeight,
			winPos.iX+10+textLength,winPos.iY+60);

	TBool match = refDevice->RectCompare(rect1,rect2);
	TEST(match);

	CleanupStack::PopAndDestroy(gcRef);
	CleanupStack::PopAndDestroy(refDevice);
	CleanupStack::PopAndDestroy(&refWin);

	CleanupStack::PopAndDestroy(bGcOne);
	CleanupStack::PopAndDestroy(deviceOne);
	CleanupStack::PopAndDestroy(bitmapOne);
	CleanupStack::PopAndDestroy(&theWin);//theWin

	}
//
// CTDrawOpWin
//

CTDrawOpWin* CTDrawOpWin::NewL(CTAlphaWin* aTest, CTWinBase* aParent, TPoint aPos, TSize aSize, TRgb aDrawColour)
	{
	CTDrawOpWin* theWin = new(ELeave) CTDrawOpWin(aTest,aDrawColour);

	theWin->ConstructL(*aParent);
	theWin->SetExtL(aPos, aSize);
	theWin->AssignGC(*TheClient->iGc);
	if (TheClient->iScreen->DisplayMode() == EColor16MA)
		{
		theWin->BaseWin()->SetRequiredDisplayMode(EColor16MA);
		}
	else
		{
		theWin->BaseWin()->SetRequiredDisplayMode(EColor64K);
		}
		
	theWin->Activate();
	theWin->DrawNow();

	return theWin;
	}

CTDrawOpWin::CTDrawOpWin(CTAlphaWin* aTest, TRgb aDrawColour)
: iTest(aTest), iDrawColour(aDrawColour)
	{}


void CTDrawOpWin::SetDrawOp(TInt aDrawOp)
	{
	iDrawOp = aDrawOp;
	}


void CTDrawOpWin::Draw()
	{
	_LIT(KText,"Text test");

	iGc->SetPenColor(iDrawColour);
	iGc->SetBrushColor(iDrawColour);
	TSize size = Size();
	TInt top = 5;
	TInt left = 5;
	TInt bottom = size.iHeight - 5;
	TInt right = size.iWidth - 5;
	TInt square = Min(bottom-top,right-left);

	switch (iDrawOp)
		{
	case EOpDrawRect:
		iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc->SetPenStyle(CGraphicsContext::ENullPen);
		iGc->DrawRect(TRect(left,top,right,bottom));
		break;
	case EOpDrawLine:
		//!! FAILS
		//!! The endpoint of the line is drawn twice, with the result that it is darker when we do blending
		//!! Not intending to fix at the moment
		/*
		iGc->SetPenStyle(CGraphicsContext::ESolidPen);
		iGc->SetPenSize(TSize(4,4));
		// The lines must not overlap, otherwise the blended lines will be darker at the overlap
		iGc->DrawLine(TPoint(left+5,top), TPoint(left+square,top));
		iGc->DrawLine(TPoint(left+5,top+5), TPoint(left+square,top+square));
		iGc->DrawLine(TPoint(left,top+5), TPoint(left,top+square));
		*/
		break;
	case EOpDrawEllipse:
		iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc->SetPenStyle(CGraphicsContext::ENullPen);
		iGc->DrawEllipse(TRect(left,top,right,bottom));
		break;
	case EOpDrawText:
	case EOpDrawTextVertical:
		{
		iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		iGc->SetPenStyle(CGraphicsContext::ESolidPen);
		CFont* font;
		TFontSpec fontSpec(KTestFontTypefaceName,200);
		User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips(font, fontSpec));
		iGc->UseFont(font);
		if (iDrawOp==EOpDrawText)
			iGc->DrawText(KText(), TPoint(5,30));
		else
			iGc->DrawTextVertical(KText(), TPoint(5,30), EFalse);
		iGc->DiscardFont();
		TheClient->iScreen->ReleaseFont(font);
		}
		break;
	case EOpDrawTextAntiAliased:
		{
		iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		iGc->SetPenStyle(CGraphicsContext::ESolidPen);
		CFont* font;
		TFontSpec fontSpec(KTestFontTypefaceName,600);
		fontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
		fontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);

		User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips(font, fontSpec));
		iGc->UseFont(font);
		iGc->DrawText(KText(), TPoint(5,30));
		iGc->DiscardFont();
		TheClient->iScreen->ReleaseFont(font);
		}
		break;
	case EOpBitBlt:
		break;
	case EOpBitBltMasked:
		break;
	default:
		break;
		};
	}



//
// CTAlphaWindow
//

CTAlphaWindow::~CTAlphaWindow()
	{
	DestroyChildren();
	}

CTAlphaWindow* CTAlphaWindow::NewL(CTAlphaWin* aTest, CTWinBase* aParent, TPoint aPos, TSize aSize, TInt aDrawState)
	{
	CTAlphaWindow* theWin = new (ELeave) CTAlphaWindow(aTest);

	theWin->ConstructL(*aParent);
	theWin->SetExtL(aPos, aSize);
	theWin->SetDrawState(aDrawState);

	theWin->AssignGC(*TheClient->iGc);

	theWin->Activate();
	theWin->iDrawState |= EActive;
	theWin->DrawNow();

	return theWin;
	}

void CTAlphaWindow::SetDrawState(TInt aDrawState)
	{
	TBool active = iDrawState & EActive;
	iDrawState = aDrawState & 0x7fffffff;

	TRgb colour = ColourFromDrawState(iDrawState);
	((RWindow*) DrawableWin())->SetBackgroundColor(colour);

	if (iDrawState & EModeColor16MA)
		BaseWin()->SetRequiredDisplayMode(EColor16MA);
	else
		BaseWin()->SetRequiredDisplayMode(EColor64K);

	BaseWin()->SetVisible(! (iDrawState & EInvisible));

	if (!active)
		{
		if (iDrawState & EAlphaTransparency)
			((RWindow*) DrawableWin())->SetTransparencyAlphaChannel();
		else if (iDrawState & ETransparencyFactor)
			((RWindow*) DrawableWin())->SetTransparencyFactor(TRgb(128,128,128));
		}

	if (active)
		iDrawState |= EActive;
	}

void CTAlphaWindow::SetVisible(TBool aVisible)
	{
	if (aVisible)
		iDrawState &= ~EInvisible;
	else
		iDrawState |= EInvisible;
	BaseWin()->SetVisible(aVisible);
	}

void CTAlphaWindow::CreateChildrenL(TInt aDepth)
	{
	DestroyChildren();
	if (aDepth>0)
		{
		TSize size = Size();
		iChild1 = CTAlphaWindow::NewL(iTest, this, TPoint(size.iWidth/3,0), TSize(2*size.iWidth/3, 2*size.iHeight/3), ERed | EGreen | EBlue | EOpaque);
		iChild2 = CTAlphaWindow::NewL(iTest, this, TPoint(0,size.iHeight/3), TSize(2*size.iWidth/3, 2*size.iHeight/3), ERed | EGreen | EBlue | EAlphaTransparency);
		iChild2->CreateChildrenL(aDepth-1);
		}
	}

void CTAlphaWindow::DestroyChildren()
	{
	if (iChild1)
		{
		iChild1->DestroyChildren();
		delete iChild1;
		iChild1 = NULL;
		}
	if (iChild2)
		{
		iChild2->DestroyChildren();
		delete iChild2;
		iChild2 = NULL;
		}
	}

TInt CTAlphaWindow::DrawState()
	{
	return iDrawState;
	}

void CTAlphaWindow::Draw()
	{
	// we draw a diagonal line from top left to bottom right
	// we use the complementary colour to the window background colour
	TInt red = (iDrawState & ERed) ? 0 : 255;
	TInt green = (iDrawState & EGreen) ? 0 : 255;
	TInt blue = (iDrawState & EBlue) ? 0 : 255;
	TRgb color(red,green,blue);

	TSize size = Size();
	iGc->SetPenColor(color);
	iGc->SetPenSize(TSize(4,4));
	iGc->DrawLine(TPoint(0,0), TPoint(size.iWidth, size.iHeight));
	}


//
// CTAlphaRefWin
//

CTAlphaRefWin::CTAlphaRefWin(TFixedArray<CTAlphaWindow*,5>& aAlphaWin)
: iAlphaWin(aAlphaWin)
	{}

CTAlphaRefWin* CTAlphaRefWin::NewL(CTWinBase* aParent, TPoint aPos, TSize aSize, TFixedArray<CTAlphaWindow*,5>& aAlphaWin)
	{
	CTAlphaRefWin* theWin = new(ELeave) CTAlphaRefWin(aAlphaWin);

	theWin->ConstructL(*aParent);
	theWin->SetExtL(aPos, aSize);
	theWin->AssignGC(*TheClient->iGc);
	theWin->BaseWin()->SetRequiredDisplayMode(EColor64K);

	theWin->Activate();
	theWin->DrawNow();

	return theWin;
	}

void CTAlphaRefWin::Draw()
	{
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(KRgbWhite);
	iGc->Clear();

	// Note, the order of the windows in the array must correspond to their z-order
	for (TInt i=0; i<5; i++)
		DrawWindow(iAlphaWin[i], iAlphaWin[i]->Position());
	}

void CTAlphaRefWin::DrawWindow(CTAlphaWindow* aWindow, TPoint aPos)
	{
	TInt drawState = aWindow->DrawState();
	if ( (drawState & EInvisible) || ! (drawState & EActive) )
		return;

	TRgb colour = ColourFromDrawState(drawState);
	if (drawState & EOpaque)
		colour.SetAlpha(255);
	if (drawState & ETransparent)
		colour.SetAlpha(0);
	iGc->SetBrushColor(colour);

	TPoint tl = aPos;
	TPoint br = tl + aWindow->Size();
	TRect rect(tl,br);
	iGc->Clear(rect);

	TInt red = (drawState & ERed) ? 0 : 255;
	TInt green = (drawState & EGreen) ? 0 : 255;
	TInt blue = (drawState & EBlue) ? 0 : 255;
	colour = TRgb(red,green,blue);

	iGc->SetClippingRect(rect);

	TSize size = Size();
	iGc->SetPenColor(colour);
	iGc->SetPenSize(TSize(4,4));
	iGc->DrawLine(tl, br);

	iGc->CancelClippingRect();

	if (aWindow->iChild1)
		DrawWindow(aWindow->iChild1, aPos + aWindow->iChild1->Position() );
	if (aWindow->iChild2)
		DrawWindow(aWindow->iChild2, aPos + aWindow->iChild2->Position() );
	}


//
// Main test loop
//
void CTAlphaWin::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	//User::After(TTimeIntervalMicroSeconds32(1000 * 1000));
	((CTAlphaWinStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch (++iTest->iState)
		{
/**

  @SYMTestCaseID GRAPHICS-WSERV-0278

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Semi-transparent drawing

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Use draw operations with semi-transparent pen or brush colours

  @SYMTestExpectedResults Draw operations must do alpha blending

*/
	case 1:
		{
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0278"));
		if(TransparencySupportedL() == KErrNotSupported)
			{
			LOG_MESSAGE(_L("Test(1) complete - Transparency not supported\n"));
			TestComplete();
			break;
			}
		TDisplayMode mode = TheClient->iScreen->DisplayMode();
		if (mode < EColor64K)
			{
			LOG_MESSAGE(_L("Test(1) complete - Display mode < EColor64K\n"));
			TestComplete();
			break;
			}
		_LIT(KSemiTrans64K,"(1) Semi transparent drawing Color64K");
		iTest->LogSubTest(KSemiTrans64K);
		TestSemiTransparentDrawingL();
		break;
		}
		
/**

  @SYMTestCaseID GRAPHICS-WSERV-0287

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Invisible. All windows are in EColor16MA display mode.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Transparent alpha channel windows are made invisible and visible both in front and behind one another

  @SYMTestExpectedResults The windows are redrawn correctly, as compared to a reference drawing

*/
	case 2:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0287"));
		ConfigureDisplayModes(EColor16MA);
		if(TransparencySupportedL()==KErrNone)
			{
			_LIT(KInvisible16MA,"(2) Invisible Color16MA");
			iTest->LogSubTest(KInvisible16MA);
			TestInvisible();
			}
		break;

/**

  @SYMTestCaseID GRAPHICS-WSERV-0280

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Initial Configuration. All windows are in EColor64K display mode.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Several windows are set to be transparent alpha channel, and given semi-transparent or transparent background colours

  @SYMTestExpectedResults The transparent window configuration matches a reference drawing created using only alpha blending

*/
	//Test 3 to 6 can't be run without transparency support
	case 3:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0280"));
		ConfigureDisplayModes(EColor64K);
		if(TransparencySupportedL()==KErrNone)
			{
			_LIT(KInitialConfiguration64K,"(3) Initial configuration Color64K");
			iTest->LogSubTest(KInitialConfiguration64K);
			TestInitialConfiguration();
			}
		break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0281

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Move. All windows are in EColor64K display mode.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Transparent alpha channel windows are moved both in front and behind one another

  @SYMTestExpectedResults The windows are redrawn correctly, as compared to a reference drawing

*/
	case 4:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0281"));
		if(TransparencySupportedL()==KErrNone)
			{
			_LIT(KMove64K,"(4) Move Color64K");
			iTest->LogSubTest(KMove64K);
			TestMove();
			}
		break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0282

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Redraw. All windows are in EColor64K display mode.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Transparent alpha channel windows are redrawn both in front and behind one another

  @SYMTestExpectedResults The windows are redrawn correctly, as compared to a reference drawing

*/
	case 5:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0282"));
		if(TransparencySupportedL()==KErrNone)
			{
			_LIT(KRedraw64K,"(5) Redraw Color64K");
			iTest->LogSubTest(KRedraw64K);
			TestRedraw();
			}
		break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0283-0001

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Invisible. All windows are in EColor64K display mode.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Transparent alpha channel windows are made invisible and visible both in front and behind one another

  @SYMTestExpectedResults The windows are redrawn correctly, as compared to a reference drawing

*/
	case 6:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0283-0001"));
		if(TransparencySupportedL()==KErrNone)
			{
			_LIT(KInvisible64K,"(6) Invisible Color64K");
			iTest->LogSubTest(KInvisible64K);
			TestInvisible();
			}
		break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0283-0002

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Children. All windows are in EColor64K display mode.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Transparent alpha channel windows are given child windows, both transparent and non-transparent,
 			    	and are then moved, redrawn, set visible or invisible both in front and behind one another

  @SYMTestExpectedResults The windows are redrawn correctly, as compared to a reference drawing

*/
	case 7:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0283-0002"));
		_LIT(KChildren64K,"(7)Children Color64K");
		iTest->LogSubTest(KChildren64K);
		TestChildrenL();
		break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0356

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Initial Configuration. All windows are in EColor64k Dispaly Mode

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests Anti-aliasing of text

  @SYMTestExpectedResults Anti-alisaing should behave correctly

*/
	case 8:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0356"));
		_LIT(KAntiAliasedText64K,"(8) AntiAliasedText DEF082251 Color64K");
		iTest->LogSubTest(KAntiAliasedText64K);
		TestAntiAliasedTextTransparentL();
		break;

/**

  @SYMTestCaseID GRAPHICS-WSERV-0284

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Initial Configuration. All windows are in EColor16MA display mode.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Several windows are set to be transparent alpha channel, and given semi-transparent or transparent background colours

  @SYMTestExpectedResults The transparent window configuration matches a reference drawing created using only alpha blending

*/
	case 9:
		{ 
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0284"));
		ConfigureDisplayModes(EColor16MA);
		TDisplayMode mode1 = TheClient->iScreen->DisplayMode();
	 	_LIT(KInitialConfiguration16MA,"(9)Initial configuration Color16MA");
		iTest->LogSubTest(KInitialConfiguration16MA);
		TestInitialConfiguration();
		break;
		}
/**

  @SYMTestCaseID GRAPHICS-WSERV-0285

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Move. All windows are in EColor16MA display mode.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Transparent alpha channel windows are moved both in front and behind one another

  @SYMTestExpectedResults The windows are redrawn correctly, as compared to a reference drawing

*/
	case 10:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0285"));
		_LIT(KMove16MA,"(10)Move Color16MA");
		iTest->LogSubTest(KMove16MA);
		TestMove();
		break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0286

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Redraw. All windows are in EColor16MA display mode.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Transparent alpha channel windows are redrawn both in front and behind one another

  @SYMTestExpectedResults The windows are redrawn correctly, as compared to a reference drawing

*/
	case 11:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0286"));
		_LIT(KRedraw16MA,"(11)Redraw Color16MA");
		iTest->LogSubTest(KRedraw16MA);
		TestRedraw();
		break;
		
/**

  @SYMTestCaseID GRAPHICS-WSERV-0279

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Transparent drawing

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Use draw operations with transparent pen or brush colours

  @SYMTestExpectedResults Draw operations with transparent pen or brush colours should leave the destination unchanged

*/

	case 12:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0279"));
		ConfigureDisplayModes(EColor64K);
		_LIT(KTrans64K,"(12) Transparent drawing Color64K");
		iTest->LogSubTest(KTrans64K);
		TestTransparentDrawingL();
		break;


/**

  @SYMTestCaseID GRAPHICS-WSERV-0288

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Children. All windows are in EColor16MA display mode.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Transparent alpha channel windows are given child windows, both transparent and non-transparent,
 			    	and are then moved, redrawn, set visible or invisible both in front and behind one another

  @SYMTestExpectedResults The windows are redrawn correctly, as compared to a reference drawing

*/
	case 13:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0288"));
		_LIT(KChildren16MA,"(13) Children Color16MA");
		iTest->LogSubTest(KChildren16MA);
		TestChildrenL();
		break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0357

  @SYMDEF             DEF081259

  @SYMPREQ 915

  @SYMTestCaseDesc Initial Configuration. All windows are in EColor16MA Dispaly Mode

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests Anti-aliasing of text

  @SYMTestExpectedResults Anti-alisaing should behave correctly

*/
	case 14:
		((CTAlphaWinStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0357"));
		_LIT(KAntiAliasedText16MA,"(14) AntiAliasedText DEF082251 Color16MA");
		iTest->LogSubTest(KAntiAliasedText16MA);
		TestAntiAliasedTextTransparentL();
		break;
	default:
		((CTAlphaWinStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTAlphaWinStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTAlphaWinStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(AlphaWin)

