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
// Test redraw storing
// Tests Storing the Redraw command mechanism in the window server.
// The first time a window is redrawn its commands are stored. When
// the window needs to be redrawn, the stored commands will be used
// rather than issue a redraw request to the client.
// The principle behind this test is to do lots of different types of drawing to a window,
// invalidate the window and test that no redrawing occurred, instead the stored commands
// should be used to generate the content of the window.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TREDRSTR.H"
#include "colorblender.h"

//#define LOGGING on

_LIT(KColorUnmatchedFormat, "Check failed, expected color value: 0x%08x, got: 0x%08x");
#define TEST_COLOR_MATCH(aExpected, aActual) \
	TEST(aExpected == aActual); \
	if(aExpected != aActual) \
		LOG_MESSAGE3(KColorUnmatchedFormat, aExpected.Value(), aActual.Value())

/*CPartialRedrawWin*/
void CPartialRedrawWin::Init()
	{
	iClientDrawn = EFalse;
	iClientCanDraw = ETrue;
	Win()->SetRequiredDisplayMode(EColor16MA);
	Win()->SetTransparencyAlphaChannel();
	Win()->SetBackgroundColor(TRgb(127,127,127,0));
	}
	
void CPartialRedrawWin::Draw()
	{
	DoDraw(*iGc);
	}
	
void CPartialRedrawWin::DrawToBmp(CGraphicsContext& aGc)
	{
	DoDraw(aGc);
	}	
	
void CPartialRedrawWin::DoDraw(CGraphicsContext& aGc)
	{
	if(!iClientCanDraw) return;
	iClientDrawn = ETrue;
	CPartialRedrawWin::DrawRects(aGc, iSize, TPoint(0,0), ETrue, EPartialRedraw_Unknown);
	}

/*static*/
void CPartialRedrawWin::DrawRects(CGraphicsContext& aGc, TSize aSize, TPoint aPosition, 
	TBool aIsFullRedraw, TPartialRedrawType aPartialRedrawType)
	{
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetPenColor(TRgb::Gray256(0));
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	if(aIsFullRedraw)
		{
		aGc.SetBrushColor(TRgb(200,200,200,127));
		aGc.DrawRect(TRect(aPosition, aSize));
		}
	else if (aPartialRedrawType!=EPartialRedraw_PreserveStoredCmds)
		{
		aGc.SetBrushColor(TRgb(200,200,200,127));	// same color as original background.
		aGc.SetPenStyle(CGraphicsContext::ENullPen);
		aGc.DrawRect(TRect(TPoint(10,10) + aPosition, aSize - TSize(20,20)));
		aGc.SetPenStyle(CGraphicsContext::ESolidPen);
		aGc.SetPenColor(TRgb::Gray256(0));
		}
	TSize r1 = TSize(aSize.iWidth/3, aSize.iHeight/5);
	TSize r2 = TSize(aSize.iWidth/9, 2*aSize.iHeight/3);
	aGc.SetBrushColor(TRgb(255, 0, 0, 127));
	aGc.DrawEllipse(TRect(TPoint(aSize.iWidth/3, aSize.iHeight/5)+aPosition, r1));
	aGc.SetBrushColor(TRgb(0, 255, 0, 127));
	aGc.DrawEllipse(TRect(TPoint(aSize.iWidth/3, 3*aSize.iHeight/5)+aPosition, r1));
	aGc.SetBrushColor(TRgb(0, 0, 255, 127));
	aGc.DrawEllipse(TRect(TPoint(4*aSize.iWidth/9, aSize.iHeight/6)+aPosition, r2));
	}
	
void CPartialRedrawWin::DrawPartial(TPartialRedrawType aPartialRedrawType)
	{
	TRect rect = TRect(TPoint(10,10), iSize - TSize(20,20));
	Invalidate(rect);
	Win()->BeginRedraw(rect);
	iGc->Activate(*Win());
	CPartialRedrawWin::DrawRects(*iGc, iSize, TPoint(0,0), EFalse, aPartialRedrawType);
	iGc->Deactivate();
	Win()->EndRedraw();
	}
void CPartialRedrawWin::RedrawSubRectWithBitmapL(TRgb aBitmapColour)
	{
	TInt bitmapWidth = Win()->Size().iWidth - 20;
	TInt bitmapHeight = Win()->Size().iHeight - 20;
	TSize bitmapSize(bitmapWidth, bitmapHeight);

	CFbsBitmap* fbsBitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(fbsBitmap);
	User::LeaveIfError(fbsBitmap->Create(bitmapSize, EColor16MU));

	// ensure colour is opaque
	aBitmapColour.SetAlpha(255);

	// draw on the bitmap
	TBitmapUtil bmpUtil(fbsBitmap);
	bmpUtil.Begin(TPoint(0, 0));
	TInt row, col;
	for(row = 0; row < bitmapWidth; ++row)
		{
		bmpUtil.SetPos(TPoint(row, 0));
		for(col = 0; col < bitmapHeight; ++col)
			{ // diagonal stripes
			if ( ((col + row) % 8) < 4 )
				{ // colour
				bmpUtil.SetPixel(aBitmapColour.Color16M());
				}
			else
				{ // semi-transparent white
				TRgb white(255, 255, 255, 128);
				bmpUtil.SetPixel(white.Color16M());
				}
			bmpUtil.IncYPos();
			}
		}
	bmpUtil.End();

	// send bitmap to screen
	TRect rect = TRect(TPoint(10,10), bitmapSize);
	Invalidate(rect);
	Win()->BeginRedraw(rect);
	iGc->Activate(*Win());
	iGc->DrawBitmap(rect, fbsBitmap);
	iGc->Deactivate();
	Win()->EndRedraw();
	CleanupStack::PopAndDestroy(fbsBitmap);
	}

/* CResetRedrawStoreWin */

const TInt KResetRedrawMaxAnimState=4;

void CResetRedrawStoreWin::PreSetSize(const TSize &aSize)
// Sets the size variable so draw code using it will use the new size
// before the window has actually been resized
	{
	iSize=aSize;
	}

void CResetRedrawStoreWin::Draw()
	{
	DoDraw(*iGc);
	}
	
void CResetRedrawStoreWin::DoDraw(CGraphicsContext& aGc) const
	{
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetPenColor(KRgbBlack);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(TRgb(200,200,200,127));
	aGc.DrawRect(TRect(iSize));
	TSize r1(iSize.iWidth/3, iSize.iHeight/5);
	TSize r2(iSize.iWidth/9, 2*iSize.iHeight/3);
	aGc.SetBrushColor(TRgb(255, 0, 0, 127));
	aGc.DrawEllipse(TRect(TPoint(iSize.iWidth/3, iSize.iHeight/5), r1));
	aGc.SetBrushColor(TRgb(0, 255, 0, 127));
	aGc.DrawEllipse(TRect(TPoint(iSize.iWidth/3, 3*iSize.iHeight/5), r1));
	aGc.SetBrushColor(TRgb(0, 0, 255, 127));
	aGc.DrawEllipse(TRect(TPoint(4*iSize.iWidth/9, iSize.iHeight/6), r2));
	DoDrawAnim(aGc);
	}

TRect CResetRedrawStoreWin::AnimRect() const
	{
	if (iUpdateInRedraw)
		{
		TInt row=iAnimState/iSize.iWidth;
		TInt col=iAnimState-row*iSize.iWidth;
		return(TRect(col,row,col+4,row+4));
		}
	return(TRect(iSize.iWidth/6,iSize.iHeight/4,iSize.iWidth*5/6,iSize.iHeight*3/4));
	}

void CResetRedrawStoreWin::DoDrawAnim(CGraphicsContext& aGc) const
	{
	if (iAnimState>0)
		{
		aGc.SetBrushColor(KRgbBlue);
		aGc.SetPenStyle(CGraphicsContext::ENullPen);
		aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		TInt animColState=iAnimState%KResetRedrawMaxAnimState;
		TRgb animCol(255*animColState/KResetRedrawMaxAnimState,0,255*(KResetRedrawMaxAnimState-animColState)/KResetRedrawMaxAnimState);
		aGc.SetBrushColor(animCol);
		aGc.DrawRect(AnimRect());
		}
	}

CResetRedrawStoreWin::~CResetRedrawStoreWin()
	{
	delete iExtraGc;
	}

void CResetRedrawStoreWin::SetUpdateInRedraw(TBool aUpdateInRedraw)
	{
	iUpdateInRedraw=aUpdateInRedraw;
	}

void CResetRedrawStoreWin::SetKeepGcActive(TBool aState)
	{
	if (iKeepGcActive!=aState)
		{
		iKeepGcActive=aState;
		if (iKeepGcActive)
			{
			iExtraGc=new(ELeave) CWindowGc(TheClient->iScreen);
			iExtraGc->Construct();
			iExtraGc->Activate(*Win());
			}
		else
			{
			iExtraGc->Deactivate();
			delete iExtraGc;
			iExtraGc=NULL;
			}
		}
	}

TBool CResetRedrawStoreWin::Failed() const
	{
	return iFailed;
	}
	
void CResetRedrawStoreWin::UpdateAnim(TInt aSteps)
	{
	TRect oldAnimRect(AnimRect());
	iAnimState+=aSteps;
	if (iUpdateInRedraw)
		{
		if (iAnimState>=(iSize.iWidth*iSize.iHeight))
			{
			iFailed=ETrue;
			}
		}
	else if (iAnimState>KResetRedrawMaxAnimState)
		{
		iAnimState-=KResetRedrawMaxAnimState;
		}
	CWindowGc *gc=Gc();
	if (iUpdateInRedraw)
		{
		Win()->Invalidate(oldAnimRect);
		Win()->BeginRedraw(oldAnimRect);
		if (iKeepGcActive)
			{
			DoDraw(*iExtraGc);
			}
		else
			{
			gc->Activate(*Win());
			DoDraw(*gc);
			gc->Deactivate();
			}
		Win()->EndRedraw();
		TRect animRect=AnimRect();
		Win()->Invalidate(animRect);
		Win()->BeginRedraw(animRect);
		}
	if (iKeepGcActive)
		DoDrawAnim(*iExtraGc);
	else
		{
		gc->Activate(*Win());
		DoDrawAnim(*gc);
		gc->Deactivate();
		}
	if (iUpdateInRedraw)
		Win()->EndRedraw();
	}

void CTRedrawStoring::GetTestWinSizeAndPos(TInt aWinIndex, TPoint& aPos, TSize& aSize) const
	{
	switch(aWinIndex)
		{
		case 0:
			// Centered window half the width of the parent window
			aSize.iWidth=iWinSize.iWidth/2;
			aSize.iHeight=iWinSize.iHeight/2;
			aPos.iX=iWinSize.iWidth/4;
			aPos.iY=iWinSize.iHeight/4;
			break;
		case 1:
			// 1/3rd parent window size window positioned 1/3rd spare size in from the bottom right
			aSize.iWidth=iWinSize.iWidth/3;
			aSize.iHeight=iWinSize.iHeight/3;
			aPos.iX=(iWinSize.iWidth-aSize.iWidth)*2/3;
			aPos.iY=(iWinSize.iHeight-aSize.iHeight)*2/3;
			break;
		}
	}

/*CRedrawStoreWin*/

void CRedrawStoreWin::Draw()
	{
	if (iTest->iQueueTest)
		iDrawOrder=iTest->iDrawOrder++;
	iTest->DoDrawingL(iGc);
	}


/*CNoDrawWin*/

void CNoDrawWin::Draw()
	{
	//Deliberately  have no drawing
	}

/*CBitmapMaskedWin*/
CBitmapMaskedWin* CBitmapMaskedWin::NewL(CFbsBitmap* aFbsBitmap,CFbsBitmap* aFbsMaskBitmap,
										CWsBitmap* aWsBitmap,CWsBitmap* aWsMaskBitmap,
										TRgb aBackground,TRect aRect,TBool aInvertMask,TBool aWsFbs)
	{
	CBitmapMaskedWin* self=new(ELeave) CBitmapMaskedWin(aFbsBitmap,aFbsMaskBitmap,aWsBitmap,
														aWsMaskBitmap,aRect,aInvertMask,aWsFbs);
	CleanupStack::PushL(self);
	self->ConstructL(*TheClient->iGroup);
	self->AssignGC(*TheClient->iGc);
	self->BaseWin()->SetRequiredDisplayMode(EColor16MU);
	self->Win()->SetBackgroundColor(aBackground);
	CleanupStack::Pop(self);
	return self;
	}

CBitmapMaskedWin::~CBitmapMaskedWin()
	{
	delete iFbsBitmap;
	delete iFbsMaskBitmap;
	delete iWsBitmap;
	if (!iWsFbs)
		{
		delete iWsMaskBitmap;	
		}
	}

void CBitmapMaskedWin::SetDestRectSize(const TSize aSize)
	{
	iRect.SetSize(aSize);
	}

void CBitmapMaskedWin::Draw()
	{
	if (iWsFbs)
		{
		TheClient->iGc->DrawBitmapMasked(iRect,iWsBitmap,TRect(iWsBitmap->SizeInPixels()),iWsMaskBitmap,iInvertMask);
		}
	else
		{
		TheClient->iGc->DrawBitmapMasked(iRect,iFbsBitmap,TRect(iFbsBitmap->SizeInPixels()),iFbsMaskBitmap,iInvertMask);
		}
	}


/* TESTCASE:	DEF095130
 * TITLE:		Redraw store for Alpha Channel Transparency.
 * IMPORTANCE:	1
 *
 * ACTION: a. Creates a window disable the redrawstore. Set the Alpha channel
 * Transparency.
 *
 * RESULT: Redraw store should be enabled and should redraw correctly.
 */
void CTRedrawStoring::DoRedrawStoreAlphaChannelTransTest()
	{
	// Create testwin and disable the redraw store
	// Set alpha transparency and check if redraw store is enabled
	RWindow win(TheClient->iWs);
	CleanupClosePushL(win);
	User::LeaveIfError(win.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle));
	win.SetExtent(iWinPos, iWinSize);
	win.SetRequiredDisplayMode(EColor256);
	win.EnableRedrawStore(EFalse);
	win.SetTransparencyAlphaChannel();
	TEST(win.IsRedrawStoreEnabled());
	CleanupStack::PopAndDestroy(&win);

	// Create a window and disable the redraw store
	// Set alpha transparency and check if redraw store is enabled
	// and check if redraw storing is done correctly
	RWindow wint(TheClient->iWs);
	CleanupClosePushL(wint);
	User::LeaveIfError(wint.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle));
	wint.SetExtent(iWinPos, iWinSize);
	wint.SetRequiredDisplayMode(iTestDisplayMode);
	wint.SetBackgroundColor(TRgb(255,255,255));
	wint.SetShadowDisabled(ETrue);
	wint.EnableRedrawStore(EFalse);
	wint.SetTransparencyAlphaChannel();
	wint.Activate();
	wint.BeginRedraw();
	TheClient->iGc->Activate(wint);
	DoDrawingL(23,TheClient->iGc,ETrue);
	TheClient->iGc->Deactivate();
	wint.EndRedraw();

	DoDrawingL(23,iCheckGc,EFalse);
	iCheckWin->BackedUpWin()->UpdateScreen();
	
	iBlankWin.SetOrdinalPosition(0);
	iBlankWin.SetVisible(ETrue);
	iBlankWin.SetVisible(EFalse);

	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TInt gap = 5;
	const TSize scrSize(TheClient->iScreen->SizeInPixels());
	TEST(TheClient->iScreen->RectCompare(TRect(TPoint(scrSize.iWidth-2*iWinSize.iWidth-gap,0),iWinSize),TRect(TPoint(scrSize.iWidth-iWinSize.iWidth,0),iWinSize)));
	CleanupStack::PopAndDestroy(&wint);
	}

/* TESTCASE:	PDEF091091
 * TITLE:		Redraw in between begin and end redraw.
 * IMPORTANCE:	1
 *
 * ACTION: Draws some content to test window in its redraw. Then starts drawing 
 * to test window by using BeginRedraw and EndRedraw methods. A blank window in 
 * front of test wndow is made visible and invisible in between BeginRedraw and
 * EndRedraw.
 * 
 *
 * RESULT: When the window is made visible again redraw should ot happen.
 */
void CTRedrawStoring::DoBeginEndRedraw()
	{
	// Check whether redrawstoring is working
	RedrawWindows();
	iBlankWin.SetVisible(ETrue);
	iBlankWin.SetVisible(EFalse);
	iClientDidDraw = EFalse;
	CheckWindowsMatch();
	TEST(!iClientDidDraw);
	if(iClientDidDraw != 0)
		INFO_PRINTF3(_L("iClientDidDraw Expected value %d Actual value %d"), 0, iClientDidDraw);
		
	// Change the size and make the blank window visible
	// Then start drawing by BeginRedraw and Activating its gc
	// Now make the blank window visible
	iTestWin->Win()->Invalidate();
	iBlankWin.SetSize(TSize(40,40));
	iBlankWin.SetVisible(ETrue);
	
	CWindowGc* gc = iTestWin->Gc();
	RWindow* win = iTestWin->Win();
	win->BeginRedraw();
	gc->Activate(*win);
	gc->SetPenStyle(CGraphicsContext::ESolidPen);
	gc->SetPenColor(TRgb(0,0,0));
	gc->SetPenSize(TSize(1,1));
	gc->DrawLine(TPoint(iWinSize.iWidth,0), TPoint(0, iWinSize.iHeight));
	iBlankWin.SetVisible(EFalse);
	gc->DrawLine(TPoint(0,0), TPoint(iWinSize.iWidth, iWinSize.iHeight));
	iBlankWin.SetVisible(ETrue);
	gc->DrawLine(TPoint(iWinSize.iWidth/2,0), TPoint(iWinSize.iWidth/2, iWinSize.iHeight));
	gc->DrawLine(TPoint(0,iWinSize.iHeight/2), TPoint(iWinSize.iWidth, iWinSize.iHeight/2));
	
	iCheckGc->Clear();
	iCheckGc->SetPenStyle(CGraphicsContext::ESolidPen);
	iCheckGc->SetPenColor(TRgb(0,0,0));
	iCheckGc->SetPenSize(TSize(1,1));
	iCheckGc->DrawLine(TPoint(iWinSize.iWidth/2,0), TPoint(iWinSize.iWidth/2, iWinSize.iHeight));
	iCheckGc->DrawLine(TPoint(0,iWinSize.iHeight/2), TPoint(iWinSize.iWidth, iWinSize.iHeight/2));
	iCheckGc->DrawLine(TPoint(0,0), TPoint(iWinSize.iWidth, iWinSize.iHeight));
	iCheckGc->DrawLine(TPoint(iWinSize.iWidth,0), TPoint(0, iWinSize.iHeight));
	iCheckWin->BackedUpWin()->UpdateScreen();
	
	iBlankWin.SetVisible(EFalse);
	
	// This is to check if any redraw happened in between Begin and EndRedraw
	/* Andy commented this out.  I'm not entirely sure what it's doing.  We just redrew a window
	while part of it was hidden, and then revealed the hidden part before calling EndRedraw, and
	this is testing that the new draw commands for the region revealed are not processed, or are not
	processed correctly. In the new window server they are processed and the region checked matches
	the test bitmap. */
//	TInt gap = 5;
//	const TSize scrSize(TheClient->iScreen->SizeInPixels());
//	TBool failed=DoCheckRect(TPoint(scrSize.iWidth-2*iWinSize.iWidth-gap,0),TPoint(scrSize.iWidth-iWinSize.iWidth,0),TSize(40,40));
	gc->Deactivate();
	win->EndRedraw();
/*	if (failed)
		{
		TEST(EFalse);
		return;
		} */

	// This is to check redraw is done after EndRedraw has called.
	iBlankWin.SetVisible(ETrue);
	iBlankWin.SetVisible(EFalse);
	CheckRect(iTestWin,iCheckWin,TRect(TSize(40,40)),_L("CTRedrawStoring::DoBeginEndRedraw"));
	// Finally bring every thing to normal
	RedrawWindows();
	CheckWindowsMatch();
	iBlankWin.SetSize(iWinSize);
	}
/**
@SYMTestCaseID			GRAPHICS-WSERV-00XX-0006

@SYMDEF             	INC087721

@SYMTestCaseDesc    	Invisible Redraw Storing Test
						Tests the non-redraw storing commands are stored/executed properly,
						in presence of partial redraw commands.
						When a semi-transparent window ST0 sits on top of an opaque window OW1,
						you want the screen to be drawn as OW1, then alpha-blend ST0 once.
						When ST0 is set to invisible, you want the screen to be drawn as OW1,
						i.e. window server to not issue redraw requests for ST0.
						When an opaque window OW2 sites on top of an opaque window OW1,
						you want the screen to be drawn as OW1, then over-print OW2 once.
						When OW2 is set invisible, you want the screen to be drawn as OW1,
						i.e. window server to not issue redraw requests for OW2.
						The reference document specifies that invisible windows do not receive
						any window server events, i.e. no redraw requests.			

@SYMTestPriority    	High

@SYMTestStatus      	Implemented

@SYMTestActions			Makes invisible a window with an invalid area

@SYMTestExpectedResults	When the window is made visible again it should display correctly
 */
void CTRedrawStoring::DoInvisibleRedrawStoreTestL( TBool aUseTransparency )
	{
	/*
	 * Obtain the color of a particular reference pixel which will be used for
	 * comparison later on when the blue test window is added covering it.
	 */
	const TPoint referencePixel(iWinPos+TPoint(50,50));
	TRgb backgroundReferenceColor;
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();
	TheClient->iScreen->GetPixel(backgroundReferenceColor, referencePixel);
	/*
	 * Add a blue test window: transparent or opaque given parameter aUseTransparency
	 */
	CInvisibleRedrawWin* testWin=new(ELeave) CInvisibleRedrawWin;
	CleanupStack::PushL(testWin);
	testWin->ConstructL(*TheClient->iGroup);
	testWin->AssignGC(*TheClient->iGc);
	testWin->SetExt(iWinPos+TPoint(25,25),TSize(300,200));
	testWin->Win()->SetRequiredDisplayMode(iTestDisplayMode);
	testWin->Win()->SetShadowDisabled(ETrue);
	if (aUseTransparency)
		{
		const TInt err = testWin->MakeTransparent();
		if (err)
			{
			TEST(EFalse);
			_LIT(KLog,"Failed to make the window transparent!");
			LOG_MESSAGE(KLog);
			}
		}
	/*
	 * Make the blue testWin window appear on top of the window at iWinPos
	 */
	testWin->Win()->Activate();
	testWin->Win()->Invalidate();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();
	/*
	 * By making the blue window invisible and then visible we can check to see if
	 * the window gets re-drawn correctly.  Redraws should not come in during the
	 * invisible phase, because "invisible windows do not receive any window server events"
	 * but should come in during the visible phase.  The background should have been
	 * drawn first and then the blue window alpha blended exactly once.
	 */
	testWin->MakeVisible(EFalse);
	testWin->Win()->Invalidate();
	testWin->MakeVisible(ETrue);
	testWin->Win()->Invalidate();
	/*
	 * Now check the screen has the desired color at the reference pixel.
	 */
	TRgb actualColor;
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();
	TheClient->iScreen->GetPixel(actualColor, referencePixel);
	if (aUseTransparency)
		{
		CColorBlender* blender = CColorBlender::NewLC(iTestDisplayMode);
		blender->SetInitialColor(backgroundReferenceColor);
		blender->Blend(TRgb(0, 0, 255, 127)); //the blue background of the window
		const TRgb expectedColor(blender->Color());
		TEST_COLOR_MATCH(expectedColor, actualColor);
		CleanupStack::PopAndDestroy(blender);
		}
	else
		{
		TEST_COLOR_MATCH(KRgbBlue, actualColor);
		}
	CleanupStack::PopAndDestroy(testWin);
	}

/*CInvisibleRedrawWin*/
CInvisibleRedrawWin::CInvisibleRedrawWin()
	: iVisible( ETrue )
	{}

TInt CInvisibleRedrawWin::MakeTransparent()
	{
	const TInt err = Win()->SetTransparencyAlphaChannel();
	if(!err)
		{
		Win()->SetBackgroundColor(TRgb(0, 0, 0, 0));
		iTransparent = ETrue;
		}
	return err;
	}

void CInvisibleRedrawWin::MakeVisible( TBool aVisible )
	{
		iVisible = aVisible;
		SetVisible( aVisible );
	}

void CInvisibleRedrawWin::Redraw()
	{
	iWin.BeginRedraw();
	DrawIfVisible();
	iWin.EndRedraw();
	}

void CInvisibleRedrawWin::Redraw( const TRect &aRect )
	{
	iWin.BeginRedraw( aRect );
	DrawIfVisible();
	iWin.EndRedraw();
	}

void CInvisibleRedrawWin::DrawIfVisible()
	{
	if (iVisible)
		{
		iGc->Activate( iWin );
		iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
		if(iTransparent)
			iGc->SetBrushColor( TRgb(0, 0, 255, 127) );
		else
			iGc->SetBrushColor( KRgbBlue );
		iGc->Clear();
		iGc->Deactivate();
		}
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-0498

@SYMDEF             	INC135845

@SYMTestCaseDesc    	UseBrushPattern test

@SYMTestPriority    	High

@SYMTestStatus      	Implemented

@SYMTestActions			Create a bitmap and use as brush. bitmap deleted immediately to
                        prove that wserv retains the handle

@SYMTestExpectedResults	No Panic BITGDI 13 
 */

void CTRedrawStoring::DoBrushDrawTestL()
	{
	CBrushDrawWin* testWin=new(ELeave) CBrushDrawWin;
	CleanupStack::PushL(testWin);
	testWin->ConstructL(*TheClient->iGroup);
	testWin->AssignGC(*TheClient->iGc);
	testWin->SetExt(iWinPos+TPoint(25,25),TSize(300,200));
	testWin->Win()->SetRequiredDisplayMode(iTestDisplayMode);
	testWin->Win()->SetShadowDisabled(ETrue);
	testWin->Activate();
	testWin->SetVisible(ETrue);

	testWin->DrawNow();
	
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	CleanupStack::PopAndDestroy(testWin);
	}

/*CBrushDrawWin*/
CBrushDrawWin::CBrushDrawWin()
	{}

void CBrushDrawWin::Draw()
	{
	Redraw();
	}

void CBrushDrawWin::Redraw()
	{
	CFbsBitmap *bitmap=new(ELeave) CFbsBitmap();
	User::LeaveIfError(bitmap->Load(TEST_BITMAP_NAME,0));
	TSize bitSize(bitmap->SizeInPixels());
	iGc->UseBrushPattern(bitmap);
	iGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iGc->DrawRect(TRect(TPoint(0, 0), bitSize));
	iGc->DiscardBrushPattern();
	delete bitmap;
	}

/*CTRedrawStoring*/
CTRedrawStoring::CTRedrawStoring(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}

CTRedrawStoring::~CTRedrawStoring()
	{
	delete iCheckWin;
	delete iCheckGc;
	delete iCheckDevice;
	delete iCheckBitmap;
	delete iTestWin;
	for(TInt bmp = 0; bmp < 3; ++bmp)
		delete iAlphaBitmap[bmp];
	iBlankWin.Close();
	iRegion.Close();
	iWinTestGc.Close();
	delete iNoDrawWin;
	delete iTestWinCopy;
	delete iCheckWinCopy;
	}

void CTRedrawStoring::ConstructL()
	{
	iState = 0;
	const TInt gap=5;
	iTestDisplayMode = TheClient->iScreen->DisplayMode();
	const TSize scrSize(TheClient->iScreen->SizeInPixels());
	iWinSize=TheClient->iScreen->SizeInPixels();
	iWinSize.iWidth=(scrSize.iWidth-gap)/3;
	CTBackedUpWin* checkWin=new(ELeave) CTBackedUpWin(iTestDisplayMode);
	checkWin->ConstructExtLD(*TheClient->iGroup,TPoint(scrSize.iWidth-iWinSize.iWidth,0),iWinSize);
	iCheckWin=checkWin;
	iCheckWin->Activate();
	RBackedUpWindow& win=*iCheckWin->BackedUpWin();
	win.MaintainBackup();
	iCheckBitmap=new(ELeave) CFbsBitmap();
	iCheckBitmap->Duplicate(win.BitmapHandle());
	iCheckDevice=CFbsBitmapDevice::NewL(iCheckBitmap);
	User::LeaveIfError(iCheckDevice->CreateContext(iCheckGc));
	iCheckGc->SetUserDisplayMode(iTestDisplayMode);
	CRedrawStoreWin* testWin=new(ELeave) CRedrawStoreWin(this);
	iWinPos.SetXY(scrSize.iWidth-2*iWinSize.iWidth-gap,0);
	testWin->ConstructExtLD(*TheClient->iGroup,iWinPos,iWinSize);
	iTestWin=testWin;
	iTestWin->AssignGC(*TheClient->iGc);
	RWindowBase& baseWin=*iTestWin->BaseWin();
	User::LeaveIfError(baseWin.SetRequiredDisplayMode(iTestDisplayMode));
	baseWin.SetShadowHeight(0);
	iTestWin->Activate();

	CNoDrawWin* noDrawWin=new(ELeave) CNoDrawWin();
	iWinPos.SetXY(scrSize.iWidth-2*iWinSize.iWidth-gap,0);
	noDrawWin->ConstructExtLD(*TheClient->iGroup,iWinPos,iWinSize);
	iNoDrawWin=noDrawWin;
	iNoDrawWin->AssignGC(*TheClient->iGc);
	RWindowBase& bWin=*iNoDrawWin->BaseWin();
	User::LeaveIfError(bWin.SetRequiredDisplayMode(EColor256));
	bWin.SetShadowHeight(0);
	
	iBlankWin=RBlankWindow(TheClient->iWs);
	User::LeaveIfError(iBlankWin.Construct(*TheClient->iGroup->WinTreeNode(),ENullWsHandle));
	iBlankWin.SetVisible(EFalse);
	User::LeaveIfError(iBlankWin.SetRequiredDisplayMode(EColor256));
	iBlankWin.SetColor(TRgb(48,240,32));
	iBlankWin.Activate();
	iWinTestGc=RWindow(TheClient->iWs);
	User::LeaveIfError(iWinTestGc.Construct(*TheClient->iGroup->WinTreeNode(),ENullWsHandle));
	iWinTestGc.SetVisible(EFalse);
	User::LeaveIfError(iWinTestGc.SetRequiredDisplayMode(EColor256));

	iDrawMode=EClientRedrawsNormal;
	iDoScrollTest=EFalse;
	iDrawOrder=0;
	//PeterI Alpha is supported but opacity is not
//	iAlphaSupported=OpacityAndAlphaSupportedL();
	iAlphaSupported =TransparencySupportedL();
	iXPlus = ETrue;
	iYPlus = EFalse;
	
	// Used for fading test
	iTestWinCopy = new (ELeave) CFbsBitmap();
	iTestWinCopy->Create(iTestWin->Size(),TheClient->iScreen->DisplayMode());
	iCheckWinCopy = new (ELeave) CFbsBitmap();
	iCheckWinCopy->Create(iCheckWin->Size(),TheClient->iScreen->DisplayMode());
	}

void CTRedrawStoring::CheckWindowsMatch()
	{
	TheClient->Flush();
	if (iDrawMode==EClientRedrawsNormal || iDrawMode==EClientRedrawsScrolled)
		TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();
	if(!iWindowsFaded)
		{
		_LIT(KLog,"RedrawStoring SubTest %d");
		TBuf<32> buf;
		buf.AppendFormat(KLog,iTest->iState);
		CheckRect(iTestWin,iCheckWin,TRect(iWinSize),buf);
		}
	else
		{
		TInt res = LossyCompareWindow(*TheClient->iScreen, *iTestWinCopy, *iCheckWinCopy, TRect(iCheckWin->Position(), iCheckWin->Size()));
		TEST(res);
		}
	}

void CTRedrawStoring::CheckWindowsNotMatch()
	{
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();
	CheckRectNoMatch(iTestWin,iCheckWin,TRect(iWinSize),_L("CTRedrawStoring::CheckWindowsNotMatch()"));
	}

void CTRedrawStoring::HideRevealTest()
	{
	iBlankWin.SetVisible(ETrue);
	iBlankWin.SetVisible(EFalse);
	CheckWindowsMatch();
	}

void CTRedrawStoring::MultipleHideReveal(TInt aX,TInt aY)
	{
	TInt xInc=(iWinSize.iWidth+aX-1)/aX;
	TInt yInc=(iWinSize.iHeight+aY-1)/aY;
	TInt xEnd=iWinPos.iX+iWinSize.iWidth;
	TInt yEnd=iWinPos.iY+iWinSize.iHeight;
	TInt xx,yy;
	for(xx=iWinPos.iX;xx<xEnd;xx+=xInc)
		{
		for(yy=iWinPos.iY;yy<yEnd;yy+=yInc)
			{
			iBlankWin.SetExtent(TPoint(xx,yy),TSize(xInc,yInc));
			HideRevealTest();
			}
		}
	}

void CTRedrawStoring::RedrawWindows()
	{
	iDrawMode=EClientRedrawsNormal;
	iTestWin->Invalidate();
	CheckWindowsMatch();
	iDrawMode=EServerRedraw;
	}

void CTRedrawStoring::DoDrawingL(CWindowGc* aWinGc)
	{
	iClientDidDraw = ETrue;
	switch (iDrawMode)
		{
	case EServerRedraw:
		TEST(EFalse);
		break;
	case EClientRedrawsNormal:
		if (iState>0)
			{
			DoDrawingL(0,aWinGc,ETrue);
			DoDrawingL(0,iCheckGc,EFalse);
			aWinGc->Deactivate();
			aWinGc->Activate(*iTestWin->DrawableWin());
			}
		DoDrawingL(iState,aWinGc,ETrue);
		DoDrawingL(iState,iCheckGc,EFalse);
		iCheckWin->BackedUpWin()->UpdateScreen();
		break;
	case EClientRedrawsScrolled:
		{
		DoDrawingL(0,aWinGc,ETrue);
		TRegionFix<8> region;
		region.AddRect(TRect(iWinSize));
		region.SubRect(iScrollTarget);
		aWinGc->SetClippingRegion(region);
		DoDrawingL(iState,aWinGc,ETrue);
		aWinGc->CancelClippingRegion();
		aWinGc->SetClippingRect(iScrollTarget);
		aWinGc->SetOrigin(iScrollTarget.iTl-iScrollSource);
		DoDrawingL(iState,aWinGc,ETrue);
		aWinGc->CancelClippingRect();
		break;
		}
		}
	TheClient->Flush();
	}

#define KLastDrawingCase 24		//This should always be the same as the value of last case number in the switch statement of the next function
void CTRedrawStoring::DoDrawingL(TInt aDraw,CBitmapContext* aGc,TBool aWinGc)
	{	
	switch (aDraw)
		{
	case 0:
	case 1:
		aGc->SetBrushColor(TRgb(255,(aDraw==0?255:0),255));
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->SetPenStyle(CGraphicsContext::ENullPen);
		aGc->DrawRect(iWinSize);
		iDoScrollTest=EFalse;
		break;
	case 2:
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetPenSize(TSize(1,1));
		aGc->SetPenColor(TRgb(0,0,0));
		aGc->DrawLine(TPoint(0,10),TPoint(iWinSize.iWidth,10));
		aGc->SetPenColor(TRgb(128,0,0));
		aGc->DrawLine(TPoint(0,iWinSize.iHeight-10),TPoint(iWinSize.iWidth,iWinSize.iHeight-10));
		aGc->SetPenColor(TRgb(0,128,0));
		aGc->DrawLine(TPoint(10,0),TPoint(10,iWinSize.iHeight));
		aGc->SetPenColor(TRgb(0,0,128));
		aGc->DrawLine(TPoint(iWinSize.iWidth-10,0),TPoint(iWinSize.iWidth-10,iWinSize.iHeight));
		iDoScrollTest=EFalse;
		break;
	case 3:
		//Do various drawing using: MoveTo, MoveBy, Plot, DrawLineTo, DrawLineBy
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetPenColor(TRgb(0,0,0));
		aGc->MoveTo(TPoint(iWinSize.iWidth, iWinSize.iHeight));
		aGc->DrawLineTo(TPoint(0, 0));
		aGc->MoveBy(TPoint(iWinSize.iWidth, 0));
		aGc->DrawLineTo(TPoint(0, iWinSize.iHeight));
		aGc->MoveTo(TPoint(0, iWinSize.iHeight/2));
		aGc->DrawLineBy(TPoint(iWinSize.iWidth, 0));
		aGc->SetPenSize(TSize(5,5));
		aGc->Plot(TPoint(iWinSize.iWidth/2, 20));
		aGc->Plot(TPoint(iWinSize.iWidth/2, iWinSize.iHeight/2));
		aGc->Plot(TPoint(iWinSize.iWidth/2, iWinSize.iHeight-20));
		aGc->SetPenSize(TSize(1,1));
		iDoScrollTest=EFalse;
		break;
	case 4:
		//Do various drawing with lines of different widths
		{
		TInt inc=iWinSize.iHeight/8;
		TInt penSize=2;
		TInt yy;
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetPenColor(TRgb(0,0,0));
		for (yy=0;yy<iWinSize.iHeight;yy+=inc)
			{
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
			if (yy%3==0)
				aGc->SetPenSize(TSize(penSize,penSize));
			else if (yy%3==1)
				aGc->SetPenSize(TSize(penSize,7*penSize/5));
			else
				aGc->SetPenSize(TSize(7*penSize/5,penSize));
#else
			aGc->SetPenSize(TSize(penSize,penSize));
#endif
			aGc->DrawLine(TPoint(2,yy),TPoint(iWinSize.iWidth-3,yy));
			penSize+=2;
			}
		aGc->SetPenSize(TSize(1,1));
		}
		iDoScrollTest=ETrue;
		break;
	case 5:
		//Some drawing using fading on the gc
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetPenColor(TRgb(0,0,255));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		aGc->SetFaded(ETrue);
#endif
		aGc->SetPenSize(TSize(10,10));
		aGc->DrawLine(TPoint(0,iWinSize.iHeight/2-5),TPoint(iWinSize.iWidth,iWinSize.iHeight/2-5));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		aGc->SetFaded(EFalse);
#endif
		aGc->DrawLine(TPoint(0,iWinSize.iHeight/2+5),TPoint(iWinSize.iWidth, iWinSize.iHeight/2+5));
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetPenColor(TRgb(0,0,255));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		aGc->SetFaded(ETrue);
		aGc->SetFadingParameters(0,127);		
#endif
		aGc->SetPenSize(TSize(10,10));
		aGc->DrawLine(TPoint(iWinSize.iWidth/2-5,0),TPoint(iWinSize.iWidth/2-5,iWinSize.iHeight));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		aGc->SetFaded(EFalse);
		// default params
		aGc->SetFadingParameters(128,255);
#endif
		aGc->DrawLine(TPoint(iWinSize.iWidth/2+5,0),TPoint(iWinSize.iWidth/2+5,iWinSize.iHeight));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		aGc->SetFaded(EFalse);
#endif
		iDoScrollTest=ETrue;
		break;
	case 6:
		// Fading on window
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetPenColor(TRgb(0,255,0));
		aGc->SetPenSize(TSize(10,10));
		aGc->DrawLine(TPoint(0,iWinSize.iHeight/2),TPoint(iWinSize.iWidth,iWinSize.iHeight/2));
		aGc->DrawLine(TPoint(iWinSize.iWidth/2,0),TPoint(iWinSize.iWidth/2,iWinSize.iHeight));
		iDoScrollTest=EFalse;
		break;
	case 7:
		//Some drawing with text - create and destroy the font as soon as used
		{
		CFbsFont *font;
		TFontSpec fspec(_L("Swiss"),190);
		User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)font,fspec));
		aGc->SetPenColor(TRgb(0,0,0));
		aGc->UseFont(font);
		aGc->DrawText(_L("Hello"), TPoint(20,20));
		aGc->DiscardFont();
		TheClient->iScreen->ReleaseFont(font);

		CFbsFont *font2;
		TInt fontSize = 100;
		TInt inc = 10;
		for (TInt i=0; i<20; i++)
			{
			TFontSpec fspec2(_L("Ariel"), fontSize);
			User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont *&)font2,fspec2));
			aGc->SetPenColor(TRgb(0,0,0));
			aGc->UseFont(font2);
			aGc->DrawText(_L("Hello"), TPoint(20,100));
			aGc->DiscardFont();
			TheClient->iScreen->ReleaseFont(font2);
			fontSize+=inc;
			}
		iDoScrollTest=ETrue;
		}
		break;
	case 8:
		//Some drawing with bitmaps - create and destroy the bitmap as soon as used
		{
		CFbsBitmap* testBitmap;	
		testBitmap=new(ELeave) CFbsBitmap();
		User::LeaveIfError(testBitmap->Load(TEST_BITMAP_NAME,0));
		aGc->DrawBitmap(TRect(TPoint(10,10), TPoint(150,150)), testBitmap);
		delete testBitmap;
		iDoScrollTest=ETrue;
		}
		break;
	case 9:
		//Some drawing with clipping regions and rects
		
		//clipping rect
	
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->SetPenStyle(CGraphicsContext::ENullPen);
		aGc->SetClippingRect(TRect(TPoint(50,0), TSize(iWinSize.iWidth/2,100)));
		aGc->SetBrushColor(TRgb(255,255,0));
		aGc->DrawRect(TRect(TPoint(0,0), TPoint(100,100)));
		aGc->SetBrushColor(TRgb(0,128,128));
		aGc->DrawRect(TRect(TPoint(iWinSize.iWidth/2,0), TSize(iWinSize.iWidth/2,100)));
		aGc->CancelClippingRect();

		
		//regions
		
		iRegion.AddRect(TRect(TPoint(0,30), TSize(3*iWinSize.iWidth/4,150)));
		iRegion.AddRect(TRect(TPoint(iWinSize.iWidth/2-20, 0), TSize(70,70)));

		aGc->SetClippingRegion(iRegion);

		aGc->SetBrushColor(TRgb(0,200,0));
		aGc->DrawRect(TRect(TPoint(5,5), TPoint(iWinSize.iWidth-50,200)));
		aGc->SetBrushColor(TRgb(200,0,0));
	 	aGc->DrawRect(TRect(TPoint(50,50), TPoint(iWinSize.iWidth/2,150)));
		aGc->SetBrushColor(TRgb(0,0,200));
		aGc->DrawRect(TRect(TPoint(20,10), TPoint(100,100)));

		aGc->CancelClippingRegion();

		iDoScrollTest=EFalse;
		break;
	case 10:
		//Some drawing with deactivating and reactivating the gc on the window (if it is indeed it is window gc)

		aGc->SetBrushColor(TRgb(0,0,255));
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->SetPenStyle(CGraphicsContext::ENullPen);
		aGc->DrawRect(TRect(TPoint(20,20), TSize(50,50)));

		if (aWinGc)
			{
			static_cast<CWindowGc*>(aGc)->Deactivate();

			// Associate gc with another window and change attributes
			static_cast<CWindowGc*>(aGc)->Activate(iWinTestGc);	
			aGc->SetPenStyle(CGraphicsContext::ESolidPen);
			aGc->SetPenColor(TRgb(0,0,255));
			static_cast<CWindowGc*>(aGc)->Deactivate();
			static_cast<CWindowGc*>(aGc)->Activate(*iTestWin->DrawableWin());
			}

		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->SetPenStyle(CGraphicsContext::ENullPen);
		aGc->SetBrushColor(TRgb(200,0,0));
		aGc->DrawRect(TRect(TPoint(70,70), TSize(50,50)));
		iDoScrollTest=EFalse;
		break;
	case 11:
		// Some drawing with polygons
		{
		aGc->SetBrushColor(TRgb(0,221,0));	
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->SetPenStyle(CGraphicsContext::ENullPen);

		TPoint point1(iWinSize.iWidth/3,iWinSize.iHeight/4*3);
		TPoint point2(iWinSize.iWidth/2,iWinSize.iHeight/5*4);
		TPoint point3(iWinSize.iWidth/3,iWinSize.iHeight-20);
		TPoint point4(iWinSize.iWidth/4,iWinSize.iHeight-20);
		TPoint point5(iWinSize.iWidth/6,iWinSize.iHeight-60);

		CArrayFix<TPoint>* points;
		points = new CArrayFixFlat<TPoint>(5);
		points->AppendL(point1);
		points->AppendL(point2);
		points->AppendL(point3);
		points->AppendL(point4);
		points->AppendL(point5);
		aGc->DrawPolygon(points);
		delete points;

		TPoint points2[5];
		points2[0].SetXY(iWinSize.iWidth/2,50);
		points2[1].SetXY(iWinSize.iWidth-50,iWinSize.iHeight/2);
		points2[2].SetXY(iWinSize.iWidth-70,iWinSize.iHeight/2+30);
		points2[3].SetXY(iWinSize.iWidth/3,iWinSize.iHeight/3);
		points2[4].SetXY(iWinSize.iWidth/4,iWinSize.iHeight/4);
		aGc->SetBrushColor(TRgb(221,0,0));
		aGc->DrawPolygon(points2,5);
		iDoScrollTest=ETrue;
		}
		break;
	case 12:
		{
		// Another Fading on Window Test
		aGc->SetPenStyle(CGraphicsContext::ENullPen);
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->SetBrushColor(TRgb(51,204,204));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		if (!iWindowsFaded || aWinGc)
			{
			aGc->SetFaded(ETrue);
			}
#endif
		aGc->DrawRect(TRect(iWinSize.iWidth/4-1,iWinSize.iHeight/4-1,3*iWinSize.iWidth/4+1,3*iWinSize.iHeight/4+1));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		aGc->SetFaded(EFalse);
#endif
		aGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetPenColor(TRgb(34,204,34));
		aGc->SetPenSize(TSize(8,8));
		aGc->DrawLine(TPoint(2,iWinSize.iHeight/2+5),TPoint(iWinSize.iWidth-2,iWinSize.iHeight/2-5));
		aGc->DrawLine(TPoint(iWinSize.iWidth/2+5,2),TPoint(iWinSize.iWidth/2-5,iWinSize.iHeight-2));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		if (!iWindowsFaded || aWinGc)
			{
			aGc->SetFaded(ETrue);
			}
#endif
		aGc->SetPenColor(TRgb(51,221,51));
		aGc->SetPenSize(TSize(3,3));
		aGc->SetBrushColor(TRgb(238,34,238));
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->DrawRect(TRect(3*iWinSize.iWidth/8-1,3*iWinSize.iHeight/8-1,5*iWinSize.iWidth/8+1,5*iWinSize.iHeight/8+1));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		aGc->SetFaded(EFalse);
#endif
		aGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		aGc->SetPenColor(TRgb(238,34,238));
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
		aGc->SetPenSize(TSize(8,9));
#else
		aGc->SetPenSize(TSize(8,8));
#endif
		aGc->DrawRect(TRect(iWinSize.iWidth/8-1,iWinSize.iHeight/8-1,7*iWinSize.iWidth/8+1,7*iWinSize.iHeight/8+1));
		iDoScrollTest=ETrue;
		}
		break;	
	case 15:
		//Some masked drawing with FBS bitmaps - create and destroy the bitmaps as soon as used
		{
		CFbsBitmap* testBitmap;	
		CFbsBitmap* maskBitmap;
		testBitmap=new(ELeave) CFbsBitmap();
		CleanupStack::PushL(testBitmap);
		maskBitmap=new(ELeave) CFbsBitmap();
		CleanupStack::PushL(maskBitmap);
		User::LeaveIfError(testBitmap->Load(TEST_BITMAP_NAME,2));
		User::LeaveIfError(maskBitmap->Load(TEST_BITMAP_NAME,4));
		aGc->BitBltMasked(TPoint(10,10), testBitmap, TRect(TPoint(0, 0), testBitmap->SizeInPixels()), maskBitmap, EFalse);
		CleanupStack::PopAndDestroy(2, testBitmap);
		iDoScrollTest=ETrue;
		}
		break;
	case 16:
		//As above, except using Ws bitmaps
		{
		CWsBitmap* testBitmap;	
		CWsBitmap* maskBitmap;
		testBitmap=new(ELeave) CWsBitmap(TheClient->iWs);
		CleanupStack::PushL(testBitmap);
		maskBitmap=new(ELeave) CWsBitmap(TheClient->iWs);
		CleanupStack::PushL(maskBitmap);
		User::LeaveIfError(testBitmap->Load(TEST_BITMAP_NAME,3));
		User::LeaveIfError(maskBitmap->Load(TEST_BITMAP_NAME,4));
		// If we don't cast to the window gc we don't see the WS version of the BitBltMasked function:
		if(aWinGc)
			((CWindowGc*)aGc)->BitBltMasked(TPoint(20,20), testBitmap, TRect(TPoint(0, 0), testBitmap->SizeInPixels()), maskBitmap, EFalse);
		else
			aGc->BitBltMasked(TPoint(20,20), testBitmap, TRect(TPoint(0, 0), testBitmap->SizeInPixels()), maskBitmap, EFalse);
		CleanupStack::PopAndDestroy(2, testBitmap);
		iDoScrollTest=ETrue;
		}
		break;
	case 19:
		//Some drawing with WS bitmaps
		{
		if(!iAlphaBitmap[0])
			{
			for(TInt bmp = 0; bmp < 3; ++bmp)
				{
				iAlphaBitmap[bmp] = new(ELeave) CWsBitmap(TheClient->iWs);
				User::LeaveIfError(iAlphaBitmap[bmp]->Load(TEST_BITMAP_NAME,2 + bmp));
				}
			}
		if(aWinGc)
			((CWindowGc*)aGc)->BitBlt(TPoint(20,20), iAlphaBitmap[0]);
		else
			aGc->BitBlt(TPoint(20,20), iAlphaBitmap[0]);
		iDoScrollTest=ETrue;
		}
		break;
	case 20:
		//Some drawing with alpha blended bitmaps
		if (iAlphaSupported)
			{
			aGc->SetFaded(EFalse);
			TPoint start(0,0);
			TSize size = iAlphaBitmap[0]->SizeInPixels();
			TPoint alphastart((start.iX + size.iWidth / 4), (start.iY + size.iHeight / 4));
		
			aGc->BitBlt(start, iAlphaBitmap[0], TRect(start, size));
			aGc->AlphaBlendBitmaps(start, iAlphaBitmap[1], TRect(start, size), iAlphaBitmap[2], alphastart);
			iDoScrollTest=ETrue;
			}
		break;
	case 21:
		// As in previous case, except using FBS bitmaps.
		if (iAlphaSupported)
			{
			aGc->SetFaded(EFalse);
			CFbsBitmap* baseBitmap;
			CFbsBitmap* testBitmap;
			CFbsBitmap* alphaBitmap;
			baseBitmap=new(ELeave) CFbsBitmap();
			CleanupStack::PushL(baseBitmap);
			testBitmap=new(ELeave) CFbsBitmap();
			CleanupStack::PushL(testBitmap);
			alphaBitmap=new(ELeave) CFbsBitmap();
			CleanupStack::PushL(alphaBitmap);
			User::LeaveIfError(baseBitmap->Load(TEST_BITMAP_NAME,2));
			User::LeaveIfError(testBitmap->Load(TEST_BITMAP_NAME,3));
			User::LeaveIfError(alphaBitmap->Load(TEST_BITMAP_NAME,4));
			TPoint start(0,0);
			TSize size = baseBitmap->SizeInPixels();
			TPoint alphastart((start.iX + size.iWidth / 4), (start.iY + size.iHeight / 4));
		
			aGc->BitBlt(start, baseBitmap, TRect(start, size));
			aGc->AlphaBlendBitmaps(start, testBitmap, TRect(start, size), alphaBitmap, alphastart);
			
			CleanupStack::PopAndDestroy(3, baseBitmap);
			iDoScrollTest=ETrue;
			}
		break;
	case 22:
		// Some default drawing for Begin EndRedraw test
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetPenSize(TSize(2,2));
		aGc->SetPenColor(TRgb(128,0,0));
		aGc->DrawLine(TPoint(10,10),TPoint(20,10));
		aGc->DrawLine(TPoint(20,10),TPoint(20,20));
		aGc->DrawLine(TPoint(20,20),TPoint(10,20));
		aGc->DrawLine(TPoint(10,20),TPoint(10,10));
		
		aGc->SetPenSize(TSize(4,4));
		aGc->SetPenColor(TRgb(0,0,128));
		aGc->DrawLine(TPoint(50,50),TPoint(150,50));
		aGc->DrawLine(TPoint(150,50),TPoint(150,150));
		aGc->DrawLine(TPoint(150,150),TPoint(50,150));
		aGc->DrawLine(TPoint(50,150),TPoint(50,50));
		iDoScrollTest=EFalse;
		break;
	case 23:
		aGc->SetBrushColor(TRgb(244,196,48));
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->SetPenStyle(CGraphicsContext::ENullPen);
		aGc->DrawRect(TRect(0,0,iWinSize.iWidth,iWinSize.iHeight/3));
		aGc->SetBrushColor(TRgb(255,255,255));
		aGc->DrawRect(TRect(0,iWinSize.iHeight/3,iWinSize.iWidth,iWinSize.iHeight*2/3));
		aGc->SetBrushColor(TRgb(3,192,60));
		aGc->DrawRect(TRect(0,iWinSize.iHeight*2/3,iWinSize.iWidth,iWinSize.iHeight));
		iDoScrollTest=EFalse;
		break;
	case 24:
		iClientDidDraw=ETrue;
		//Draw some rects to screen
		aGc->Reset();
		aGc->Clear();
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->SetBrushColor(TRgb(98,72,172));	
		aGc->DrawRect(TRect(20,iYPoz,250,280+iYPoz));
		aGc->SetBrushColor(TRgb(255,0,0));	
		aGc->DrawRect(TRect(0,iYPoz,200,200+iYPoz));
		aGc->SetBrushColor(TRgb(0,255,255));	
		aGc->DrawRect(TRect(10,15+iYPoz,250,115+iYPoz));
		aGc->SetBrushColor(TRgb(0,255,0));	
		aGc->DrawRect(TRect(0,50+iYPoz,100,250+iYPoz));
		aGc->SetBrushColor(TRgb(255,255,0));	
		aGc->DrawRect(TRect(50,50+iYPoz,150,150+iYPoz));
		aGc->SetBrushColor(TRgb(5,25,20));	
		aGc->DrawRect(TRect(120,170+iYPoz,220,250+iYPoz));	
		break;			
 		}
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0085

@SYMDEF             DEF081259

@SYMTestCaseDesc    Do Draw Test
					REQUIREMENT:	REQ2123
					GT0164/Delta/ 1450, 1460, 1470, 1490, 1500, 1510, 1520, 1530

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Lots of different type of drawing is done to the test window.
					(Including: normal drawing, fonts, bitmaps, fading on the GC,
					clipping regions and rects).
					A blank window is made visible then invisible above the test window
					The blank window's size and position is also changed many times.

@SYMTestExpectedResults  After the initial drawing of the test window, all the draw commands
						should be stored by the window server.  When the blank window is made
						visible/invisible above the test window a redraw message will be sent
						to the test window. The window will be redrawn using the draw commands
						stored in the server. Once all the redrawing is complete, the test window
						will be compared with a bitmap that has had the same draw commands applied
						to it.
						The test will fail if the bitmaps don't match or if the test window was
						redrawn not using the stored server side draw commands.
 */

void CTRedrawStoring::DoDrawTest()
	{
	RedrawWindows();
	HideRevealTest();
	iTestWin->SetVisible(EFalse);
	iTestWin->SetVisible(ETrue);
	CheckWindowsMatch();
	MultipleHideReveal(2,3);
	MultipleHideReveal(5,4);
	iBlankWin.SetExtent(iWinPos,iWinSize);
	HideRevealTest();
	CheckWindowsMatch();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0086

@SYMDEF             DEF081259

@SYMTestCaseDesc    Fade Window Test
					REQUIREMENT:	REQ2123
					GT0164/Delta/ 1480

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test window is faded and the GC associated with the bitmap used
					to check the test window is faded. The Draw Test in TestCase 1 is then
					applied.

@SYMTestExpectedResults  The test window and the check bitmap should both be faded and contain
					the same drawing. The test will fail if the bitmaps don't match or if the
					test window was redrawn not using the stored server side draw commands.

 */
void CTRedrawStoring::FadeWindowTest()
	{
	iWindowsFaded = ETrue;
	iTestWin->Win()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
	iCheckGc->SetFaded(ETrue);
	DoDrawTest();
	iDrawMode=EClientRedrawsNormal;
	iTestWin->Win()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
	iCheckGc->SetFaded(EFalse);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	iWindowsFaded = EFalse;
	}
	
/**
@SYMTestCaseID		GRAPHICS-WSERV-0087

@SYMDEF             DEF081259

@SYMTestCaseDesc    Fade Window Test 2
					REQUIREMENT:	REQ2123
					GT0164/Delta/ 1480

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     The test window is faded and the check window that uses the check bitmap
					is faded. A blank window is made visbible/invisible above the test window.
					Fading is switched off on both windows, they are redrawn and then compared.

@SYMTestExpectedResults  The test window and the check bitmap should both be faded. After showing 
					the blank window the test window will contain a couple of rectangles faded due to 
					Gc fade apart from the overall window fade (will look similar to double fading), whereas
					check window will have simply the overall window fade. Once both windows have been
					redrawn with the fading switched off, they should not look the same for the same
					reason explained above.

 */
void CTRedrawStoring::FadeWindowTest2L()
	{
	DoDrawTest();
	iWindowsFaded=ETrue;
	iTestWin->BaseWin()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);
	iCheckWin->BaseWin()->SetFaded(ETrue,RWindowTreeNode::EFadeWindowOnly);	
	CheckWindowsMatch();

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	//perform RedrawWindows() with CheckWindowsNotMatch()
	iDrawMode=EClientRedrawsNormal;
	iTestWin->Invalidate();
	CheckWindowsNotMatch();	
	iDrawMode=EServerRedraw;
	
	//perform HideRevealTest() with CheckWindowsNotMatch()
	iBlankWin.SetVisible(ETrue);
	iBlankWin.SetVisible(EFalse);
	CheckWindowsNotMatch();	
#endif

	iWindowsFaded=EFalse;
	iTestWin->BaseWin()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
	iCheckWin->BaseWin()->SetFaded(EFalse,RWindowTreeNode::EFadeWindowOnly);
	iDrawMode=EClientRedrawsNormal;
	DoDrawingL(0,iCheckGc,EFalse);
	DoDrawingL(iState,iCheckGc,EFalse);
	iCheckWin->BackedUpWin()->UpdateScreen();
	iDrawMode=EServerRedraw;
	CheckWindowsMatch();
	HideRevealTest();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0088

@SYMDEF             DEF081259

@SYMTestCaseDesc    Scroll Test
					REQUIREMENT:	REQ2123
					GT0164/Delta/ 1540

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Different areas of the test window are scrolled, the check bitmap
					window is also adjusted to reflect this scrolling. The blank window is
					then made visible/invisible above the test window

@SYMTestExpectedResults  The test will fail if the bitmaps don't match or if the test window was
					redrawn not using the stored server side draw commands.

 */
void CTRedrawStoring::ScrollTest()
	{
	CheckWindowsMatch();
	TInt x=iWinSize.iWidth/3;
	TInt w=iWinSize.iWidth/4;
	SetScrolling(TPoint(10,20),TRect(x,100,x+w,160));
	DoScrollTest();
	x=iWinSize.iWidth/2;
	w=iWinSize.iWidth/3;
	SetScrolling(TPoint(48,100),TRect(x,10,x+w,80));
	DoScrollTest();
	x=iWinSize.iWidth/10;
	w=iWinSize.iWidth/5;
	SetScrolling(TPoint(iWinSize.iWidth/2,20),TRect(x,100,x+w,150)); 
	DoScrollTest();
	}

void CTRedrawStoring::DoScrollTest()
	{
	TheClient->Flush();
	iDrawMode=EClientRedrawsScrolled;
	CheckWindowsMatch();
	iDrawMode=EServerRedraw;		
 	HideRevealTest();
	RedrawWindows();
	CheckWindowsMatch();
	}

void CTRedrawStoring::SetScrolling(TPoint aScrollSource, TRect aScrollTarget)
	{
	iScrollSource=aScrollSource;
	iScrollTarget=aScrollTarget;
	iTestWin->DrawableWin()->Scroll(iScrollTarget.iTl-iScrollSource,TRect(iScrollSource,iScrollTarget.Size()));	
	iCheckWin->DrawableWin()->Scroll(iScrollTarget.iTl-iScrollSource,TRect(iScrollSource,iScrollTarget.Size()));	
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0090

@SYMDEF             DEF081259

@SYMTestCaseDesc    Do Nothing in Redraw Test
					REQUIREMENT:	REQ2123
					GT0164/Delta/ 1570

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     A window is created that contains no drawing code. A blank window is
					made visible/invisible above this window.

@SYMTestExpectedResults  No buffer will be created server side because there are no draw commands
					to store. The server should be able to cope with an empty buffer when the
					redraw is issued, caused by the blank win.

 */
void CTRedrawStoring::DoNothingInRedrawTest()
	{
	// iNoDrawWin contains no drawing code, therefore no server side 
	// redraw store buffer will be created in a redraw.
	// When a redraw occurs because the blank win is made visible/invisible,
	// the server will try and access the non existant buffer, all being well
	// nothing should happen because the server can cope with an empty redraw 
	// buffer.

	iTestWin->SetVisible(EFalse);
	iNoDrawWin->Activate();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	iDrawMode=EServerRedraw;
	iBlankWin.SetOrdinalPosition(0);
	iBlankWin.SetVisible(ETrue);
	iBlankWin.SetVisible(EFalse);
	TheClient->Flush();

	//return to normal testing state
	iNoDrawWin->SetVisible(EFalse);
	iTestWin->SetVisible(ETrue);
	RedrawWindows();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0091

@SYMDEF             DEF081259

@SYMTestCaseDesc    Disable Redraw Store Test

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     A windows redraw store is disabled and enabled, and the window is exposed.

@SYMTestExpectedResults  When the redraw store is disabled, a client redraw should occur when the window
					is exposed. When it is enabled, no client redraw should occur. However, the
					first time it is exposed after enabling the store it will need a client redraw
					in order to fill the store.

 */
void CTRedrawStoring::DoDisableRedrawStoreTest()
	{
	_LIT(KLog1,"Redraw storing not enabled when expected to be");
	_LIT(KLog2,"No client redraw was done when it was expected");
	RedrawWindows();
	CheckWindowsMatch();
	
	iDrawMode=EServerRedraw;
	HideRevealTest();
	
	iClientDidDraw=EFalse;
	TBool isEnabled=iTestWin->Win()->IsRedrawStoreEnabled();
	TEST(isEnabled);
	if (!isEnabled)
		LOG_MESSAGE(KLog1);

	/*iTestWin->Win()->EnableRedrawStore(EFalse);
	isEnabled=iTestWin->Win()->IsRedrawStoreEnabled();
	TEST(!isEnabled);
	if (isEnabled)
		{
		_LIT(KLog,"Redraw storing enabled when expected not to be");
		LOG_MESSAGE(KLog);
		}

	iDrawMode=EClientRedrawsNormal;
	HideRevealTest();
	TEST(iClientDidDraw);
	if (!iClientDidDraw)
		{
		LOG_MESSAGE(KLog2);
		TheClient->WaitForRedrawsToFinish();
		if (iClientDidDraw)
			{
			_LIT(KLog,"After Waiting Redraws had taken place");
			LOG_MESSAGE(KLog);
			}
		}*/
	
	iTestWin->Win()->EnableRedrawStore(ETrue);
	isEnabled=iTestWin->Win()->IsRedrawStoreEnabled();
	TEST(isEnabled);
	if (!isEnabled)
		LOG_MESSAGE(KLog1);

	HideRevealTest();
	iDrawMode=EServerRedraw;
	HideRevealTest();

	iClientDidDraw=EFalse;
	TheClient->iWs.ClearAllRedrawStores();
	iDrawMode=EClientRedrawsNormal;
	HideRevealTest();
	TEST(iClientDidDraw);
	if (!iClientDidDraw)
		{
		LOG_MESSAGE(KLog2);
		TheClient->WaitForRedrawsToFinish();
		if (iClientDidDraw)
			{
			_LIT(KLog,"After Waiting Redraws had taken place");
			LOG_MESSAGE(KLog);
			}
		}

	HideRevealTest();
	iDrawMode=EServerRedraw;
	HideRevealTest();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0092

@SYMDEF             DEF081259

@SYMTestCaseDesc    Resize Redraws

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     A window is resized.

@SYMTestExpectedResults  When the window decreases in size, the server should be able to
					redraw it from the store. When it increases in size, a client redraw
					should occur.

 */
void CTRedrawStoring::DoResizeTest()
	{
	RedrawWindows();
	
	TSize oldsize = iTestWin->Win()->Size();

	iDrawMode=EServerRedraw;
	iTestWin->Win()->SetSize(TSize(8, 8));
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	
	iClientDidDraw=EFalse;	
	iDrawMode=EClientRedrawsNormal;
	iTestWin->Win()->SetSize(oldsize);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TEST(iClientDidDraw);
	if (!iClientDidDraw)
		INFO_PRINTF3(_L("iClientDidDraw - Expected: %d, Actual: %d"), ETrue, iClientDidDraw);

	}

/* TESTCASE:	9
 * TITLE:		Font Cache Overflow
 * IMPORTANCE:	1
 * REQUIREMENT:	DEF065463
 * 
 *
 * API:	
 * #
 *
 * ACTION:
 * The Font Cache is overflowed
 *
 * RESULT:
 * If the font cache overflows or under out of memory conditions,
 * there should be no leaves, panics or incorrect behaviour of the
 * local array of font handles.
 */
 
 void CTRedrawStoring::DoFontCacheOverflowTestL()
 	{
	RWindow window(TheClient->iWs);
 	User::LeaveIfError(window.Construct(*TheClient->iGroup->WinTreeNode(), ENullWsHandle));
  	CleanupClosePushL(window);
  	window.Activate();
  	// Display mode is set after window.Activate() purposely to check that drawing 
  	// is done in the right mode, in order to test fix for DEF083327
  	User::LeaveIfError(window.SetRequiredDisplayMode(EColor256));
  			
	// run test using a single gc
	FontCacheOverflowDrawingTestL(EFalse, window);
	// reset for next test
	window.Invalidate();
	iXPlus = ETrue;
	iYPlus = EFalse;
	// run test using multiple gcs
	FontCacheOverflowDrawingTestL(ETrue, window);
	
	CleanupStack::PopAndDestroy(&window);
 	}
 	
/* TESTCASE:	22
* TITLE:		Scroll Window
* IMPORTANCE:	
* REQUIREMENT:	
* 
*
* API:	
* #
*
* ACTION:
* A window is scrolled then a blank window is popped up, made visible and then 
* invisible in order to test that partial redraw storing is drawing window 
* contents properly after hiding the blank window.
* 
* Before the fix, the contents used to disappear as partial redraw storing was 
* not storing the commands to draw areas outside the defined clipping rect, in 
* this case the area covered by the popped window. Now, the fix makes sure that 
* full redraw to the window is applied when there are atored commands but the 
* changes are performed only in the covered area.
*
* RESULT:
* When the blank window is hidden, the covered area will be redrawn and will
* contain the original contents before poping the blank window. 
*
*/
void CTRedrawStoring::ScrollWinTest()
	{
	iDrawMode=EClientRedrawsNormal;

	// Drawing the contents first before scrolling
	iTestWin->DrawNow();

	// Scrolling the test window and updating its y position
	iTestWin->DrawableWin()->Scroll(TPoint(0, 25));	
	iYPoz += 25;

	// Invalidating and redrawing the area that should be updated
	TRect invalidRect(0,25, iWinSize.iWidth, 25*2);
	iTestWin->Invalidate(invalidRect);
	iTestWin->Redraw(invalidRect);			// Redraw is used instead of DrawNow becuase the later calls Invalidate on the whole window
		
	// Displaying and then hiding the popup blank window
	iBlankWin.SetExtent(TPoint(iWinSize.iWidth+40,30), TSize(120, 100));
	iBlankWin.SetVisible(ETrue);
	iBlankWin.SetVisible(EFalse);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	// Resetting iBlankWin to its original size and position for future use 
	// by other test cases
	iBlankWin.SetExtent(iWinPos, iWinSize);	
	CheckWindowsMatch();
	iYPoz=0;
	}


TPoint CTRedrawStoring::ComputeTextPosition(TPoint aPoint)
 	{
 	// Bounces text around the screen
 	const TInt KSpacing = 30;
 	
 	if(iXPlus)
 		{
 		aPoint.iX += KSpacing;
 		}
 	else
 		{
 		aPoint.iX -= KSpacing;
 		}
 	if(aPoint.iX > iWinSize.iWidth)
 		{
 		aPoint.iX = iWinSize.iWidth - (aPoint.iX - iWinSize.iWidth);
 		iXPlus = EFalse;
 		}
 	else if(aPoint.iX < 0)
 		{
 		aPoint.iX = -1*aPoint.iX;
 		iXPlus = ETrue;
 		}
 		
 	if(iYPlus)
 		{
 		aPoint.iY += KSpacing;
 		}
 	else
 		{
 		aPoint.iY -= KSpacing;
 		}
 	if(aPoint.iY > iWinSize.iHeight)
 		{
 		aPoint.iY = iWinSize.iHeight - (aPoint.iY - iWinSize.iHeight);
 		iYPlus = EFalse;
 		}
 	else if(aPoint.iY < 0)
 		{
 		aPoint.iY = -1*aPoint.iY;
 		iYPlus = ETrue;
 		}
 	return aPoint;
 	}
 	
void CTRedrawStoring::FontCacheOverflowDrawingTestL(TBool aDiffGc, RWindow& aWindow)
	{
	const TInt KNumFonts = 250;
	const TInt KNumFontTypes = TheClient->iScreen->NumTypefaces();
 	const TInt KMaxFontSize = 21; // font sizes to be tested in range 1 to 21
 	const TInt KNumTestStyles = 4; 
 	const TInt KNumSizes = KNumFonts/(KNumFontTypes * KNumTestStyles) + 1; // chooses a number of font sizes to overflow cache, rounded up
 	TInt textStyle = 0; //determines whether text is not changed (0), bold (1), bold and italic (2) or italic (3)
	TInt fontType = 0; //increment for different font types
	TInt currentSize = 1; // start with a font size of 1
	TInt fontSizeIncrement = KMaxFontSize - currentSize; //defaults to 20
	if(KNumSizes>2)
		{
		fontSizeIncrement = KMaxFontSize/(KNumSizes-1);
		}
	TInt numGcs = 1; 
 	if(aDiffGc)
 		{
 		numGcs = KNumFonts;
 		}
	_LIT(KTestText,"b8-/+.,*:");
	const TSize KScrSize(TheClient->iScreen->SizeInPixels());
	TSize fontCacheWinSize(KScrSize.iWidth/2,KScrSize.iHeight);
	iTestWinPoint.SetXY(fontCacheWinSize.iWidth/2, fontCacheWinSize.iHeight/2); //draw initially near the middle of the screen

	CWindowGc* winGc = NULL;
	RArray<CWindowGc*> winGcList;
	CleanupClosePushL(winGcList);
	
	aWindow.BeginRedraw();

	// fill an array with fonts of different styles (see textStyle comment), types, and sizes
	RArray<CFont*> fontArray;
	CleanupClosePushL(fontArray);
	for(TInt ii = 0; ii < KNumFonts; ii++)
		{
		if(ii && !(ii % (KNumTestStyles * KNumSizes)))
			{
			fontType++;
			textStyle = 0;
			currentSize = 1;
			}
		else if(ii && !(ii % KNumTestStyles))
			{
			currentSize += fontSizeIncrement;
			textStyle = 0;
			}
		TTypefaceSupport support;
		TheClient->iScreen->TypefaceSupport(support, fontType);
		TFontSpec fspec(support.iTypeface.iName.Des(), currentSize);
		switch(textStyle++)
			{
		case 0:
			fspec.iFontStyle.SetPosture(EPostureUpright);
			break;
		case 1:
			fspec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
			break;
		case 2:
			fspec.iFontStyle.SetPosture(EPostureItalic);
			break;
		case 3:
			fspec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
			break;
			}
		CFont* font = NULL;
		User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInPixels(font, fspec));
		User::LeaveIfError(fontArray.Append(font));
		font = NULL;
	
		// Draw to left half of screen using either one gc for all fonts, or using a font per gc, dependent on value of aDiffGc	
		if(ii<numGcs)
			{
			winGc = new(ELeave) CWindowGc(TheClient->iScreen);
			CleanupStack::PushL(winGc); 
			User::LeaveIfError(winGc->Construct());
			winGc->Activate(aWindow);
			User::LeaveIfError(winGcList.Append(winGc));
			}
		winGc->UseFont(fontArray[ii]);
		winGc->SetPenColor(TRgb::Color256(ii));
		winGc->DrawText(KTestText, iTestWinPoint = ComputeTextPosition(iTestWinPoint));
		}
	
	aWindow.EndRedraw();
	TheClient->Flush();
	
	// Copy the drawing to a bitmap and redraw to the right half of the screen
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	bitmap->Create(TSize(fontCacheWinSize.iWidth, fontCacheWinSize.iHeight), EColor256);
	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(bitmap, TRect(fontCacheWinSize)));
	TPoint copiedBitmapOrigin(fontCacheWinSize.iWidth, 0);
	TRect bitmapArea(copiedBitmapOrigin, bitmap->SizeInPixels());
	aWindow.Invalidate(bitmapArea);
	aWindow.BeginRedraw(bitmapArea);
	winGc->BitBlt(copiedBitmapOrigin, bitmap);
	aWindow.EndRedraw();
	CleanupStack::PopAndDestroy(bitmap);
		
	// Trigger a redraw (left half of screen)
	RBlankWindow blankWindow(TheClient->iWs);
	CleanupClosePushL(blankWindow);
 	User::LeaveIfError(blankWindow.Construct(*TheClient->iGroup->WinTreeNode(), ENullWsHandle));
	blankWindow.SetSize(TSize(fontCacheWinSize.iWidth, fontCacheWinSize.iHeight));
 	blankWindow.Activate();
 	TheClient->Flush();
 	blankWindow.SetVisible(EFalse);
 	TheClient->Flush();
 	CleanupStack::PopAndDestroy(&blankWindow);
 	TheClient->WaitForRedrawsToFinish();
 	// Compare what is redrawn with copy of original drawing
	TEST(TheClient->iScreen->RectCompare(TRect(fontCacheWinSize),TRect(copiedBitmapOrigin,fontCacheWinSize))); 		
 	// Clean up all memory 		
	for(TInt kk = 0; kk < KNumFonts; kk++)
		{
		if(kk < numGcs)
			{
			winGcList[kk]->Deactivate();
			}
		TheClient->iScreen->ReleaseFont(fontArray[kk]);
		}
	CleanupStack::PopAndDestroy(2+numGcs, &winGcList); 
	}

// As a full fledged test code is written for this implementation.
// so this test code checks whether this defect is fixed.
void CTRedrawStoring::DoTestDrawBitmapMaskedL(TInt aWsBitmap/*=EFalse*/)
	{
	// Create a source bitmap with display mode EColor16MU and Fill RGB lines successively
	TInt bitmapWidth=iWinSize.iWidth-40;
	TInt bitmapHeight=80;
	TSize bitmapSize(bitmapWidth,bitmapHeight);
	CFbsBitmap* fbsBitmap=NULL;
	CWsBitmap* wsBitmap=NULL;
	if (aWsBitmap)
		{
		wsBitmap=new(ELeave) CWsBitmap(TheClient->iWs);
		CleanupStack::PushL(wsBitmap);
		User::LeaveIfError(wsBitmap->Create(bitmapSize,EColor16MU));
		}
	else
		{
		fbsBitmap=new(ELeave) CFbsBitmap();
		CleanupStack::PushL(fbsBitmap);
		User::LeaveIfError(fbsBitmap->Create(bitmapSize,EColor16MU));
		}

	TBitmapUtil bmpUtil(aWsBitmap ? wsBitmap : fbsBitmap);
	bmpUtil.Begin(TPoint(0,0));
	TInt row,col;
	for(row=0;row<bitmapWidth;++row)
		{
		bmpUtil.SetPos(TPoint(row,0));
		for(col=0;col<bitmapHeight;++col)
			{
			if (row%3==0)
				{
				TRgb rgb(255,0,0);
				bmpUtil.SetPixel(rgb.Color16M());
				}
			else if (row%3==1)
				{
				TRgb rgb(0,255,0);
				bmpUtil.SetPixel(rgb.Color16M());
				}
			else
				{
				TRgb rgb(0,0,255);
				bmpUtil.SetPixel(rgb.Color16M());
				}
			bmpUtil.IncYPos();
			}
		}
	bmpUtil.End();
	
	// Create mask bitmap with display mode EGray256 and Fill white and black lines successively
	CFbsBitmap* fbsBitmapMask=NULL;
	CWsBitmap* wsBitmapMask=NULL;
	if (aWsBitmap)
		{
		wsBitmapMask=new(ELeave) CWsBitmap(TheClient->iWs);
		CleanupStack::PushL(wsBitmapMask);
		User::LeaveIfError(wsBitmapMask->Create(bitmapSize,EGray256));
		}
	else
		{
		fbsBitmapMask=new(ELeave) CFbsBitmap();
		CleanupStack::PushL(fbsBitmapMask);
		User::LeaveIfError(fbsBitmapMask->Create(bitmapSize,EGray256));
		}

	TBitmapUtil bmpUtilMask(aWsBitmap ? wsBitmapMask : fbsBitmapMask);
	bmpUtilMask.Begin(TPoint(0,0));
	for(row=0;row<bitmapWidth;++row)
		{
		bmpUtilMask.SetPos(TPoint(row,0));
		for(col=0;col<bitmapHeight;++col)
			{
			if (row%2==0)
				{
				bmpUtilMask.SetPixel(0xff000000);
				}
			else
				{
				bmpUtilMask.SetPixel(0xffffffff);
				}
			bmpUtilMask.IncYPos();
			}
		}
	bmpUtilMask.End();
	CleanupStack::Pop(2); // wsBitmap or fbsBitmap and fbsBitmapMask or wsBitmapMask

	// Create window and draw the content of it by using DrawBitmapMasked
	// Background to be red
	TSize screenSize=TheClient->iScreen->SizeInPixels();
	iWinRect.SetRect(screenSize.iWidth/3,0,2*screenSize.iWidth/3,screenSize.iHeight);
	iBitmapMaskedWin=CBitmapMaskedWin::NewL(fbsBitmap,fbsBitmapMask,wsBitmap,wsBitmapMask,KRgbRed,bitmapSize,EFalse,aWsBitmap);
	CleanupStack::PushL(iBitmapMaskedWin);
	iBitmapMaskedWin->SetExt(TPoint(screenSize.iWidth/3,0),iWinRect.Size());
	iBitmapMaskedWin->Activate();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	
	// Create a bitmap window which in its draw function it just bitblts its content
	// First fill that bitmap with red color
	iTestWinPoint.SetXY(2*screenSize.iWidth/3,0);
	iTestBitmap=CBitmap::NewL(iWinRect.Size(),EColor16MU);
	CleanupStack::PushL(iTestBitmap);
	iTestBitmap->Gc().SetBrushColor(TRgb(255,0,0));
	iTestBitmap->Gc().SetBrushStyle(CGraphicsContext::ESolidBrush);
	iTestBitmap->Gc().SetPenStyle(CGraphicsContext::ENullPen);
	iTestBitmap->Gc().DrawRect(iWinRect.Size());
	iTestBitmap->Gc().Reset();
	iTestBitmapWin=new(ELeave) CBitMapWin(iTestBitmap);
	CleanupStack::PushL(iTestBitmapWin);
	iTestBitmapWin->ConstructExtLD(*TheClient->iGroup,iTestWinPoint,iWinRect.Size());
	iTestBitmapWin->BaseWin()->SetRequiredDisplayMode(EColor16MU);
	iTestBitmapWin->BaseWin()->SetShadowDisabled(ETrue);
	iTestBitmapWin->BaseWin()->SetShadowHeight(0);
	iTestBitmapWin->AssignGC(*TheClient->iGc);
	iTestBitmapWin->Activate();
	
	// This if for testing with Invertmask as EFalse
	TSize tempSize=bitmapSize;
	DrawBitmapAndCheckL(tempSize,EColor16MU,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),EFalse);

	tempSize.SetSize(bitmapSize.iWidth*11/10,bitmapSize.iHeight*11/10);
	iBitmapMaskedWin->SetDestRectSize(tempSize);
	iBitmapMaskedWin->DrawNow();
	TheClient->WaitForRedrawsToFinish();
	DrawBitmapAndCheckL(tempSize,EColor16MU,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),EFalse);

	tempSize.SetSize(bitmapSize.iWidth*2/3,bitmapSize.iHeight*2/3);
	iBitmapMaskedWin->SetDestRectSize(tempSize);
	iBitmapMaskedWin->DrawNow();
	TheClient->WaitForRedrawsToFinish();
	DrawBitmapAndCheckL(tempSize,EColor16MU,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),EFalse);

	// This if for testing with Invertmask as ETrue
	tempSize=bitmapSize;
	iBitmapMaskedWin->SetInvertMask(ETrue);
	iBitmapMaskedWin->SetDestRectSize(tempSize);
	iBitmapMaskedWin->DrawNow();
	TheClient->WaitForRedrawsToFinish();
	DrawBitmapAndCheckL(tempSize,EColor16MU,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),ETrue);

	tempSize.SetSize(bitmapSize.iWidth*11/10,bitmapSize.iHeight*11/10);
	iBitmapMaskedWin->SetDestRectSize(tempSize);
	iBitmapMaskedWin->DrawNow();
	TheClient->WaitForRedrawsToFinish();
	DrawBitmapAndCheckL(tempSize,EColor16MU,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),ETrue);

	tempSize.SetSize(bitmapSize.iWidth*2/3,bitmapSize.iHeight*2/3);
	iBitmapMaskedWin->SetDestRectSize(tempSize);
	iBitmapMaskedWin->DrawNow();
	TheClient->WaitForRedrawsToFinish();
	DrawBitmapAndCheckL(tempSize,EColor16MU,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),ETrue);

	// With bitmap's display mode as EColor256 and invertmask EFalse
	if (aWsBitmap)
		{
		wsBitmap->SetDisplayMode(EColor256);
		}
	else
		{
		fbsBitmap->SetDisplayMode(EColor256);	
		}
	iBitmapMaskedWin->BaseWin()->SetRequiredDisplayMode(EColor256);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	// Delete the tempbitmap and tempbitmapwin and recreate once again.
	CleanupStack::PopAndDestroy(2); // iTestBitmap, iTestBitmapWin
	iTestBitmap=NULL;
	iTestBitmapWin=NULL;
	iTestBitmap=CBitmap::NewL(iWinRect.Size(),EColor256);
	CleanupStack::PushL(iTestBitmap);
	iTestBitmap->Gc().SetBrushColor(TRgb(255,0,0));
	iTestBitmap->Gc().SetBrushStyle(CGraphicsContext::ESolidBrush);
	iTestBitmap->Gc().SetPenStyle(CGraphicsContext::ENullPen);
	iTestBitmap->Gc().DrawRect(iWinRect.Size());
	iTestBitmap->Gc().Reset();
	iTestBitmapWin=new(ELeave) CBitMapWin(iTestBitmap);
	CleanupStack::PushL(iTestBitmapWin);
	iTestBitmapWin->ConstructExtLD(*TheClient->iGroup,iTestWinPoint,iWinRect.Size());
	iTestBitmapWin->BaseWin()->SetRequiredDisplayMode(EColor256);
	iTestBitmapWin->BaseWin()->SetShadowDisabled(ETrue);
	iTestBitmapWin->BaseWin()->SetShadowHeight(0);
	iTestBitmapWin->AssignGC(*TheClient->iGc);
	iTestBitmapWin->Activate();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	tempSize=bitmapSize;
	iBitmapMaskedWin->SetInvertMask(EFalse);
	iBitmapMaskedWin->SetDestRectSize(tempSize);
	iBitmapMaskedWin->DrawNow();
	TheClient->WaitForRedrawsToFinish();
	DrawBitmapAndCheckL(tempSize,EColor256,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),EFalse);

	tempSize.SetSize(bitmapSize.iWidth*11/10,bitmapSize.iHeight*11/10);
	iBitmapMaskedWin->SetDestRectSize(tempSize);
	iBitmapMaskedWin->DrawNow();
	TheClient->WaitForRedrawsToFinish();
	DrawBitmapAndCheckL(tempSize,EColor256,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),EFalse);

	tempSize.SetSize(bitmapSize.iWidth*2/3,bitmapSize.iHeight*2/3);
	iBitmapMaskedWin->SetDestRectSize(tempSize);
	iBitmapMaskedWin->DrawNow();
	TheClient->WaitForRedrawsToFinish();
	DrawBitmapAndCheckL(tempSize,EColor256,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),EFalse);

	// With bitmap's display mode as EColor256 and invertmask ETrue
	tempSize=bitmapSize;
	iBitmapMaskedWin->SetInvertMask(ETrue);
	iBitmapMaskedWin->SetDestRectSize(tempSize);
	iBitmapMaskedWin->DrawNow();
	TheClient->WaitForRedrawsToFinish();
	DrawBitmapAndCheckL(tempSize,EColor256,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),ETrue);

	tempSize.SetSize(bitmapSize.iWidth*11/10,bitmapSize.iHeight*11/10);
	iBitmapMaskedWin->SetDestRectSize(tempSize);
	iBitmapMaskedWin->DrawNow();
	TheClient->WaitForRedrawsToFinish();
	DrawBitmapAndCheckL(tempSize,EColor256,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),ETrue);

	tempSize.SetSize(bitmapSize.iWidth*2/3,bitmapSize.iHeight*2/3);
	iBitmapMaskedWin->SetDestRectSize(tempSize);
	iBitmapMaskedWin->DrawNow();
	TheClient->WaitForRedrawsToFinish();
	DrawBitmapAndCheckL(tempSize,EColor256,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),ETrue);

	//To test if DrawBitmapMask uses stored commands when called to redraw the bitmap.
	if (aWsBitmap)
		{
		delete wsBitmapMask; //deleting the bitmap
		wsBitmapMask=NULL;
		DrawBitmapAndCheckL(tempSize,EColor256,(aWsBitmap ? wsBitmap : fbsBitmap),(aWsBitmap ? wsBitmapMask : fbsBitmapMask),ETrue);
		}
	CleanupStack::PopAndDestroy(3,iBitmapMaskedWin); // iBitmapMaskedWin,iTestBitmap,iTestBitmapWin
	}

void CTRedrawStoring::DrawBitmapAndCheckL(const TSize aSize,TDisplayMode aDisplayMode,CFbsBitmap* aSrceBitmap,CFbsBitmap* aMaskBitmap,TBool aInvertMask)
	{
	TBool retVal;
	if (aMaskBitmap)
		{
		TRect srceRect(aSrceBitmap->SizeInPixels());
		TRect destRect(aSize);
		CBitmap* srcTempBitmap=CBitmap::NewL(aSize,aDisplayMode);
		CleanupStack::PushL(srcTempBitmap);
		srcTempBitmap->Gc().DrawBitmap(destRect,aSrceBitmap,srceRect);
		CBitmap* maskTempBitmap=CBitmap::NewL(aSize,EGray256);
		CleanupStack::PushL(maskTempBitmap);
		maskTempBitmap->Gc().DrawBitmap(destRect,aMaskBitmap,srceRect);
		iTestBitmap->Gc().SetPenStyle(CGraphicsContext::ENullPen);
		iTestBitmap->Gc().SetBrushColor(TRgb(255,0,0));
		iTestBitmap->Gc().SetBrushStyle(CGraphicsContext::ESolidBrush);
		iTestBitmap->Gc().DrawRect(iWinRect.Size());
		iTestBitmap->Gc().BitBltMasked(TPoint(),&srcTempBitmap->Bitmap(),destRect,&maskTempBitmap->Bitmap(),aInvertMask);
		iTestBitmap->Gc().Reset();
		iTestBitmapWin->DrawNow();
		TheClient->iWs.Finish();
		TheClient->WaitForRedrawsToFinish();
		retVal = TheClient->iScreen->RectCompare(TRect(iWinRect.iTl,aSize),TRect(iTestWinPoint,aSize));
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);
		CleanupStack::PopAndDestroy(2,srcTempBitmap);
		}
	else 
	//To test if DrawBitmapMask uses stored commands, when called to redraw the bitmap.
	//After the bitmap "wsBitmapMask" is being deleted, the window "iBitmapMaskWin" is first made invisible 
	//and then visible on the screen. This operation invokes draw function which redraws the bitmap by using the stored commands.
		{
		iBitmapMaskedWin->SetVisible(EFalse);
		iBitmapMaskedWin->SetVisible(ETrue);
		retVal = TheClient->iScreen->RectCompare(TRect(iWinRect.iTl,aSize),TRect(iTestWinPoint,aSize));
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("TheClient->iScreen->RectCompare() return value  - Expected: %d, Actual: %d"), ETrue, retVal);
		}
	}


/**
	@SYMTestCaseID GRAPHICS-CODEBASE-WSERV-0052-0001
	@SYMPREQ PGM027
  
	@SYMTestCaseDesc Tests CWsBitmap::BitBltMasked and CFbsBitmap::BitBltMasked API's with \n
	By passing Null and unexpected values.
   
	@SYMTestPriority 1 
  
	@SYMTestStatus Implemented
   
	@SYMTestActions Call BitBltMasked with different ways
	Source Bitpmap as NULL and MaskBitmap
	Source Bitmap and MaskBitmap as NULL (For both CFbsBitmap, CWsBitmap)
		
	@SYMTestExpectedResults Should not panic even if the passed bitmaps are NULL.

 */		
void CTRedrawStoring::DoBitBltAndMaskedNegTestsL()
	{
	CWsBitmap* testBitmap=NULL;	
	CWsBitmap* maskBitmap=NULL;
	// Passing null Masked bitmap
	(TheClient->iGc)->BitBltMasked(TPoint(20,20), testBitmap, TRect(TPoint(0, 0), TSize(20,20)), maskBitmap, EFalse);
	testBitmap=new(ELeave) CWsBitmap(TheClient->iWs);
	CleanupStack::PushL(testBitmap);
	User::LeaveIfError(testBitmap->Load(TEST_BITMAP_NAME,3));
	(TheClient->iGc)->BitBltMasked(TPoint(20,20), testBitmap, TRect(TPoint(0, 0), TSize(20,20)), maskBitmap, EFalse);
	CleanupStack::PopAndDestroy(testBitmap);
	testBitmap=NULL;
	maskBitmap=new(ELeave) CWsBitmap(TheClient->iWs);
	CleanupStack::PushL(maskBitmap);
	User::LeaveIfError(maskBitmap->Load(TEST_BITMAP_NAME,4));
	// Passing null source bitmap
	(TheClient->iGc)->BitBltMasked(TPoint(20,20), testBitmap, TRect(TPoint(0, 0), TSize(20,20)), maskBitmap, EFalse);
	CleanupStack::PopAndDestroy(maskBitmap);
	CFbsBitmap* samBitmap=NULL;	
	CFbsBitmap* mskBitmap=NULL;
	// Passing null Masked bitmap
	(TheClient->iGc)->BitBltMasked(TPoint(20,20), samBitmap, TRect(TPoint(0, 0), TSize(20,20)), mskBitmap, EFalse);
	samBitmap=new(ELeave) CWsBitmap(TheClient->iWs);
	CleanupStack::PushL(samBitmap);
	User::LeaveIfError(samBitmap->Load(TEST_BITMAP_NAME,3));
	(TheClient->iGc)->BitBltMasked(TPoint(20,20), samBitmap, TRect(TPoint(0, 0), TSize(20,20)), mskBitmap, EFalse);
	CleanupStack::PopAndDestroy(samBitmap);
	samBitmap=NULL;
	mskBitmap=new(ELeave) CWsBitmap(TheClient->iWs);
	CleanupStack::PushL(mskBitmap);
	User::LeaveIfError(mskBitmap->Load(TEST_BITMAP_NAME,4));
	// Passing null source bitmap
	(TheClient->iGc)->BitBltMasked(TPoint(20,20), testBitmap, TRect(TPoint(0, 0), TSize(20,20)), mskBitmap, EFalse);
	CleanupStack::PopAndDestroy(mskBitmap);
	CWsBitmap* cwBitmap=NULL;	
	TPoint pos(0,0);
	// Passing null CWsBitmap
	TheClient->iGc->BitBlt(pos,cwBitmap);
	TheClient->iGc->BitBlt(pos,cwBitmap,TRect(0,0,10,10));
	// Passing null CFbsBitmap
	CFbsBitmap* fbsBitmap=NULL;	
	TheClient->iGc->BitBlt(pos,fbsBitmap);
	TheClient->iGc->BitBlt(pos,fbsBitmap,TRect(0,0,10,10));
	}

/* TESTCASE:	INC095798
 * TITLE:		Partial Draw Now Test
 * IMPORTANCE:	1
 *
 * ACTION: Changes the color of a rectangle inside a window to simulate CCoeControl::DrawNow()
 * for an embedded control that does not own a window.
 *
 * RESULT: The rectangle should change color immediately, without waiting for a redraw event
 * from the Window Server, and this should work with or without partial redraw storing and
 * with or without transparency.
 */
void CTRedrawStoring::DoPartialDrawNowTestL( TBool aUseTransparency )
	{
	/*
	 * Obtain the color of a particular reference pixel which will be used for
	 * comparison later on when the test window is added covering it.
	 */
	const TPoint referencePixel(iWinPos+TPoint(50,50));
	TRgb backgroundReferenceColor;
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();
	TheClient->iScreen->GetPixel(backgroundReferenceColor, referencePixel);
	/*
	 * Add a test window which emulates a CONE control with a lodger.
	 * The window is transparent according to parameter aUseTransparency
	 */
	CPartialDrawNowWin* testWin=new(ELeave) CPartialDrawNowWin;
	CleanupStack::PushL(testWin);
	testWin->ConstructL(*TheClient->iGroup);
	testWin->AssignGC(*TheClient->iGc);
	testWin->SetExt(iWinPos+TPoint(25,25),TSize(300,200));
	testWin->Win()->SetRequiredDisplayMode(iTestDisplayMode);
	testWin->Win()->SetShadowDisabled(ETrue);
	if (aUseTransparency)
		TEST(testWin->MakeTransparent() == KErrNone);
	testWin->Win()->Activate();
	testWin->Redraw();
	testWin->SetLodger(TRect(20,20,30,30));
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	TheClient->iWs.Finish();
	TRgb actualColor;
	TheClient->iScreen->GetPixel(actualColor,iWinPos+TPoint(50,50));
	if (aUseTransparency)
		{
		CColorBlender* blender = CColorBlender::NewLC(iTestDisplayMode);
		blender->SetInitialColor(backgroundReferenceColor);
		blender->Blend(TRgb(0, 0, 255, 127)); //the blue background of the window
		blender->Blend(TRgb(0, 255, 0, 127)); //the green color of the lodger
		const TRgb expectedColor = blender->Color();
		TEST_COLOR_MATCH(expectedColor, actualColor);
		CleanupStack::PopAndDestroy(blender);
		}
	else
		{
		TEST_COLOR_MATCH(KRgbGreen, actualColor);
		}
	CleanupStack::PopAndDestroy(testWin);
	}

/*CPartialDrawNowWin*/
CPartialDrawNowWin::CPartialDrawNowWin()
	: iLodger( 0, 0, 0, 0 )
	{}

TInt CPartialDrawNowWin::MakeTransparent()
	{
	const TInt err = iWin.SetTransparencyAlphaChannel();
	if(!err)
		{
		iWin.SetBackgroundColor(TRgb(0, 0, 0, 0));
		iTransparent = ETrue;
		}
	return err;
	}

void CPartialDrawNowWin::SetLodger( const TRect &aLodger )
	{
	iLodger = aLodger;
	iWin.Invalidate( aLodger );
	Redraw( aLodger );
	}

void CPartialDrawNowWin::Redraw()
	{
	iWin.BeginRedraw();
	DrawWindowAndLodger();
	iWin.EndRedraw();
	}

void CPartialDrawNowWin::Redraw( const TRect &aRect )
	{
	iWin.BeginRedraw( aRect );
	DrawWindowAndLodger();
	iWin.EndRedraw();
	}

void CPartialDrawNowWin::DrawWindowAndLodger()
	{
	iGc->Activate( iWin );
	iGc->SetPenStyle( CGraphicsContext::ENullPen );
	iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
	iGc->SetBrushColor( iTransparent ? TRgb(0, 0, 255, 127) : KRgbBlue );
	iGc->Clear();
	if (!iLodger.IsEmpty())
		{
		iGc->SetBrushColor( iTransparent ? TRgb(0, 255, 0, 127) : KRgbGreen );
		iGc->DrawRect( iLodger );
		}
	iGc->Deactivate();
	}



/* TESTCASE:	PDEF101789
   TITLE:		Expose Window Test for PDEF101789: WServ does not perform well in 9.2 release.
  
   ACTION: 		Draws a base window followed by a top window that completly covers the base window.
   				The base window has an area invalidated and the top window is then moved out of the 
  				way to expose the bottom window. The invalid are is then drawn to within a begin/end redraw
  
   RESULT: 		The invalid area on the base window should be redrawn correctly betweeen the begin/end 
  				redraw pair after the base window has been exposed. The invalid area is drawn in a different
  				colour to the base window.
 */
void CTRedrawStoring::DoExposeTestL(TInt aIteration)
	{
	_LIT(KErrorMessage,"Expected colour value does not match actual value : Windows not drawn correctly");
	
	TPartialRedrawType type = iTest->RedrawStoreTypeL();
	if(type==EPartialRedraw_FullRedrawSupport)
		{
		//draw a green coloured base window
		CPartialRedrawBottomWin* bottomWin = new (ELeave) CPartialRedrawBottomWin();
		CleanupStack::PushL(bottomWin);
		bottomWin->ConstructL(*TheClient->iGroup);
		bottomWin->Init();
		bottomWin->AssignGC(*TheClient->iGc);
		bottomWin->BaseWin()->SetShadowDisabled(ETrue);
		bottomWin->BaseWin()->SetShadowHeight(0);
		bottomWin->SetExt(iWinPos+TPoint(10,10), iTest->StdTestWindowSize());
		bottomWin->Win()->Activate();
		bottomWin->DrawPartial(TRect(TPoint(0,0), iTest->StdTestWindowSize()));
		TheClient->Flush();
		
		//draw a red coloured top window that completely covers the base window
		CPartialRedrawTopWin* topWin = new (ELeave) CPartialRedrawTopWin();
		CleanupStack::PushL(topWin);
		topWin->ConstructL(*TheClient->iGroup);
		topWin->Init();
		topWin->AssignGC(*TheClient->iGc);
		topWin->BaseWin()->SetShadowDisabled(ETrue);
		topWin->BaseWin()->SetShadowHeight(0);
		topWin->SetExt(iWinPos+TPoint(10,10),iTest->StdTestWindowSize());
		topWin->Win()->Activate();
		topWin->DrawPartial(TRect(TPoint(0,0), iTest->StdTestWindowSize()));
		TheClient->Flush();

		//Invalidate the an area on the bottom window. 
		TRect rect(TPoint(10,10), TSize(iTest->StdTestWindowSize().iWidth/4, iTest->StdTestWindowSize().iHeight/4));
		bottomWin->Win()->Invalidate(rect);

		//Now expose the bottom window by moving the top window out of the way 
		//using one of the methods below
		switch(aIteration)
			{
			case 0:
				topWin->Win()->SetOrdinalPosition(-10);	
				break;
			case 1:
				topWin->SetPos(iWinPos + TPoint(150,150));
				break;
			case 2:
				topWin->SetVisible(EFalse);
				break;	
			}

		//now do a begin/end redraw to draw a blue rect to the invalid area on the bottom window 
		bottomWin->Win()->BeginRedraw(rect);
		bottomWin->Gc()->Activate(*bottomWin->Win());
		bottomWin->Gc()->SetBrushStyle(CGraphicsContext::ESolidBrush);
		bottomWin->Gc()->SetBrushColor(TRgb(0,0,255,255));
		bottomWin->Gc()->SetPenStyle(CGraphicsContext::ESolidPen);
		bottomWin->Gc()->SetPenColor(0);	
		bottomWin->Gc()->DrawRect(rect);
		bottomWin->Gc()->Deactivate();
		bottomWin->Win()->EndRedraw();
		TheClient->Flush();

		//get the color of a pixel within the invalid area that should be coloured 
		//blue if the window is redrawn correctly. In the defect this blue area is NOT drawn
		TPoint point =iWinPos + TPoint(30,30); 
		TRgb colour;
		TheClient->iScreen->GetPixel(colour,point);
		TRgb expectedColour=TRgb(0,0,255,255);
		TEST(colour == expectedColour);
		if (colour!=expectedColour)
			INFO_PRINTF1(KErrorMessage);
	
		CleanupStack::PopAndDestroy(2, bottomWin);
		}
	}
/*CPartialRedrawTopWin*/
void CPartialRedrawTopWin::Init()
	{
	Win()->SetRequiredDisplayMode(EColor16MA);
	Win()->SetTransparencyAlphaChannel();
	Win()->SetBackgroundColor(TRgb(255,255,255,255));
	}

void CPartialRedrawTopWin::Draw()
	{
	DoDraw();
	}

void CPartialRedrawTopWin::DoDraw()
	{
	DrawFullWindowRect();
	}

void CPartialRedrawTopWin::DrawFullWindowRect()
	{
	TRect rect = TRect(TPoint(0,0),iSize);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(TRgb(255,0,0,255));
	iGc->DrawRect(rect);
	}

void CPartialRedrawTopWin::DrawPartial(TRect aRect)
	{
	Invalidate(aRect);
	Win()->BeginRedraw(aRect);
	iGc->Activate(*Win());
	DrawFullWindowRect();
	iGc->Deactivate();
	Win()->EndRedraw();
	}
	
/*CPartialRedrawBottomWin*/	
void CPartialRedrawBottomWin::Init()
	{
	Win()->SetRequiredDisplayMode(EColor16MA);
	Win()->SetTransparencyAlphaChannel();
	Win()->SetBackgroundColor(TRgb(255,255,255,255));
	}

void CPartialRedrawBottomWin::Draw()
	{
	DoDraw();
	}

void CPartialRedrawBottomWin::DoDraw()
	{
	DrawFullWindowRect();
	}

void CPartialRedrawBottomWin::DrawFullWindowRect()
	{
	TRect rect = TRect(TPoint(0,0),iSize);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(TRgb(0,255,0,255));
	iGc->DrawRect(rect);
	}

void CPartialRedrawBottomWin::DrawPartial(TRect aRect)
	{
	Invalidate(aRect);
	Win()->BeginRedraw(aRect);
	iGc->Activate(*Win());
	DrawFullWindowRect();
	iGc->Deactivate();
	Win()->EndRedraw();
	}

//CPartialRedrawTiledWin

void CPartialRedrawTiledWin::Init(TRgb aColour,TBool aTransparent)
	{
	iColour=aColour;
	Win()->SetRequiredDisplayMode(EColor16MA);
	if(aTransparent)
		{
		Win()->SetTransparencyAlphaChannel();
		}
	Win()->SetBackgroundColor(iColour);
	}

void CPartialRedrawTiledWin::Draw()
	{
	DoDraw();
	}

void CPartialRedrawTiledWin::DoDraw()
	{
	DrawFullWindowRect();
	}

void CPartialRedrawTiledWin::DrawFullWindowRect()
	{
	TRect rect = TRect(TPoint(0,0),iSize);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(iColour);
	iGc->DrawRect(rect);
	}

void CPartialRedrawTiledWin::DrawPartial(TRect aRect)
	{
	Invalidate(aRect);
	Win()->BeginRedraw(aRect);
	iGc->Activate(*Win());
	DrawFullWindowRect();
	iGc->Deactivate();
	Win()->EndRedraw();
	}
	
/* TESTCASE:	DEF101889	
   TITLE:	 	Expose Window Test for DEF101889 Windows tiled by opaque children do not redraw correctly 
   				under certain use cases
 
   ACTION: 		Draws a base window then a further window over it that is tiled by opaque children. 
   				This window is then, either completely or partially, covered by a transparent window. 
   				The tiled window is then set invisible. Drawing is then performed to the base window.
  
   RESULT: 		The windows should be correctly drawn. More specifically the transparent top window should draw
   				the correct window underneath it. i.e. after the setinvisible command the base window should be 
   				drawn 
 */
void CTRedrawStoring::DoExposeTest2L(TInt aIteration)
	{
	//This test reproduces problems found during the fixing of DEF096874: WServ does not perform well in 9.2 release.
	//The issues (described later) only exhbit themselves when there are no shadows present in the system.
	//Unfortunatly there is no direct way of disabling shadows from the test code so under normal running the 
	//following tests will never hit the defect and always pass.
	//To disable shadows the WSERV source code has to be manually altered by editing the CWsClientWindow::CommandL
	//method in clwin.cpp. In the EWsWinOpSetShadowHeight case alter the SetHeightDiff(*pData.Int); function call
	//to read SetHeightDiff(0);
	//The use cases are related to DEF096874 in that the problem occurs when we have 2 windows overlaying each other 
	//where the top window is completely tiled by child windows. DEF096874 occurs when the window that is tiled is made 
	//invisible, with the result that the windows are not redrawn correctly. 
	//The use cases reproduced by this test are when the two windows are either fully or partially obscured by a 
	//further transparent window laid over the both of them. When the tiled window is made invisible then 
	//the windows are not updated properly resulting in either transparency problems or the windows not being drawn 
	//correctly. 
	//There are further use cases not addressed here i.e. tiled windows becoming visible underneath a transparent window
	//that relate to the same fundamental problem but are correctlly addressed by the defect fix.

	TPartialRedrawType type=iTest->RedrawStoreTypeL();
	if(type!=EPartialRedraw_FullRedrawSupport)
		return;

	_LIT(KErrorMessage,"Pixel expected to have colour 0x%x has color 0x%x");
	const TSize winSize=iTest->StdTestWindowSize();
	const TInt offset=winSize.iWidth/2;

	//draw a green coloured base window
	CPartialRedrawBottomWin* underWin = new(ELeave) CPartialRedrawBottomWin();
	CleanupStack::PushL(underWin);
	underWin->ConstructL(*TheClient->iGroup);
	underWin->Init();
	underWin->AssignGC(*TheClient->iGc);
	underWin->BaseWin()->SetShadowDisabled(ETrue);
	underWin->SetExt(iWinPos+TPoint(10,10),winSize);
	underWin->Win()->Activate();
	underWin->DrawPartial(TRect(winSize));
	if (TDisplayModeUtils::NumDisplayModeColors(underWin->BaseWin()->DisplayMode())<=4096)
		{
		CleanupStack::PopAndDestroy(underWin);
		_LIT(KLog,"Cannot run test without more than 4K colors");
		LOG_MESSAGE(KLog);
		return;
		}

	//draw a red coloured top window that completly covers the base window
	CPartialRedrawTopWin* overWin = new (ELeave) CPartialRedrawTopWin();
	CleanupStack::PushL(overWin);
	overWin->ConstructL(*TheClient->iGroup);
	overWin->Init();
	overWin->AssignGC(*TheClient->iGc);
	overWin->BaseWin()->SetShadowDisabled(ETrue);
	overWin->SetExt(iWinPos+TPoint(10,10),winSize);
	overWin->Win()->Activate();
	overWin->DrawPartial(TRect(winSize));

	//create the two tiles to attach to the top window
	CPartialRedrawTiledWin* tile =  new (ELeave) CPartialRedrawTiledWin();
	CleanupStack::PushL(tile);
	tile->ConstructL(*overWin);
	tile->Init(TRgb(255,255,0,255),EFalse);
	tile->AssignGC(*TheClient->iGc);
	tile->BaseWin()->SetShadowDisabled(ETrue);
	tile->SetSize(winSize);
	tile->Win()->Activate();
	tile->DrawPartial(TRect(winSize));

	//create a transparent window overlaying the whole arrangement
	CPartialRedrawTiledWin* transparentWin =  new (ELeave) CPartialRedrawTiledWin();
	CleanupStack::PushL(transparentWin);
	transparentWin->ConstructL(*TheClient->iGroup);
	transparentWin->Init(TRgb(255,255,255,0),ETrue);
	transparentWin->AssignGC(*TheClient->iGc);
	transparentWin->BaseWin()->SetShadowDisabled(ETrue);
	//for the first iteration have the transparent window fully covering the other windows
	//for the second iteration have the tansparent window partially covering the other windows.
	transparentWin->SetExt(iWinPos+TPoint(10+(aIteration==0?0:offset),10),winSize);
	transparentWin->Win()->Activate();
	transparentWin->DrawPartial(TRect(winSize));

	//Now expose the bottom window (underWin) by setting the top window (overWin) invisible 
	//the opaque child window (tile) should also go invisible 
	overWin->SetVisible(EFalse);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	//get the color of a pixel within the window. If everything has been drawn correctly the 
	//pixel should be green ( the colour of the base window, underWin)
	TPoint point =iWinPos + TPoint(30,30); 
	TRgb colour;
	TheClient->iScreen->GetPixel(colour,point);
	TRgb expectedColour=TRgb(0,255,0,255);
	TEST(colour == expectedColour);	
	if (colour!=expectedColour)
		LOG_MESSAGE3(KErrorMessage,expectedColour.Value(),colour.Value());
	//for partially covered windows the above code tests the uncovered region so an additional test
	//is needed on a pixel in the covered region.
	if (aIteration!=0)
		{
		point+=TPoint(offset,0);
		TheClient->iScreen->GetPixel(colour,point);
		TEST(colour==expectedColour);
		if (colour!=expectedColour)
			LOG_MESSAGE3(KErrorMessage,expectedColour.Value(),colour.Value());
		}
	CleanupStack::PopAndDestroy(4, underWin); //tile,topWin,transparentWin
	}

/* Test automatically purging the redraw store */

CResetRedrawStoreWin* CTRedrawStoring::CreatePartialRedrawWinLC(const TPoint &aPos, const TSize &aSize, CTWin *aParent)
	{
	CResetRedrawStoreWin* testWin = new (ELeave) CResetRedrawStoreWin();
	CleanupStack::PushL(testWin);
	if (aParent)
		testWin->ConstructL(*aParent);
	else
		testWin->ConstructL(*TheClient->iGroup);
	testWin->Init();
	testWin->AssignGC(*TheClient->iGc);
	testWin->BaseWin()->SetShadowDisabled(ETrue);
	testWin->BaseWin()->SetShadowHeight(0);
	testWin->Win()->SetVisible(EFalse);
	testWin->Win()->Activate();
	testWin->SetExt(aPos,aSize);
	testWin->SetVisible(ETrue);
	return(testWin);
	}

CNoDrawWin* CTRedrawStoring::CreateNoDrawWinLC(const TPoint &aPos, const TSize &aSize)
	{
	CNoDrawWin* noDrawWin=new (ELeave) CNoDrawWin();
	CleanupStack::PushL(noDrawWin);
	noDrawWin->ConstructExtLD(*TheClient->iGroup, aPos, aSize);
	noDrawWin->AssignGC(*TheClient->iGc);
	noDrawWin->Win()->SetRequiredDisplayMode(iTestDisplayMode);
	noDrawWin->Win()->SetTransparencyAlphaChannel();
	noDrawWin->Win()->SetBackgroundColor(TRgb(127,127,127,127));
	noDrawWin->BaseWin()->SetShadowDisabled(ETrue);
	noDrawWin->BaseWin()->SetShadowHeight(0);
	noDrawWin->Win()->SetVisible(ETrue);
	noDrawWin->Activate();
	noDrawWin->Win()->BeginRedraw();
	noDrawWin->Gc()->Activate(*noDrawWin->Win());
	CPartialRedrawWin::DrawRects(*noDrawWin->Gc(), aSize, 
	  TPoint(0,0), ETrue, EPartialRedraw_Unknown);
	noDrawWin->Gc()->Deactivate();
	noDrawWin->Win()->EndRedraw();
	return(noDrawWin);
	}
	
void CTRedrawStoring::AutoResetRedrawStoreTestsL()
	{
	//PeterI This tests redraw store resetting by monitoring the wserv heap.
	//It currently fails as it performs too many iterations of the test i.e. it doesn't reset as frequently
	//as the orignal werv. Needs investigation to determine what the expected number of resets should be for 
	//the Mk3 wserv.
	/*
	if (iPartialRedrawType==EPartialRedraw_FullRedrawSupport)
		{
		const TInt startWsHeapCount=TheClient->iWs.HeapCount();
		const TInt KNumFlagsToTest=4;
		const TInt KNumFlagStatesToTest=1<<KNumFlagsToTest;
		for(TUint flags=0;flags<KNumFlagStatesToTest;flags++)
			{
		#if defined(LOGGING)
			_LIT(KLog,"AutoResetRedrawStoreTestsL, running test with flags 0x%x");
			LOG_MESSAGE2(KLog,flags);
		#endif
			DoAutoResetRedrawStoreTestL(flags&0x01,flags&0x02,flags&0x04,flags&0x08);
			}
		// Granularity of buffers can leave odd extra cells, hard to specify an exact amount
		// This may need tweaking again future, should hand verify any gains are not cumulative
		// by running this test multiple times and making sure the total does not keep
		// rising everytime around.
		const TInt KHeapTotalSafetyMargin=16;
		const TInt endHeapCount=TheClient->iWs.HeapCount();
		TEST((startWsHeapCount+KHeapTotalSafetyMargin)>=endHeapCount);
		}
	*/
	}
	
void CTRedrawStoring::DoAutoResetRedrawStoreTestL(TBool aTwoWins, TBool aAnimateBothWins, TBool aKeepGcActive, TBool aUpdateInRedraw)
	{
	TRect testRect1(iTestWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),iWinSize);
	TRect testRect2(iCheckWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),iWinSize);
//
	TSize testWinSize1(iWinSize.iWidth/3,iWinSize.iHeight/3);
	TSize testWinSize2(iWinSize.iWidth/2,iWinSize.iHeight/2);
	TPoint topLeft=iWinPos+TPoint(100,100);
	TInt tooBig=topLeft.iX+testWinSize1.iWidth-testRect1.iBr.iX;
	if (tooBig>0)
		topLeft.iX-=tooBig;
	
	CResetRedrawStoreWin* testWin1=CreatePartialRedrawWinLC(topLeft,testWinSize1);
	CResetRedrawStoreWin* testWin2=CreatePartialRedrawWinLC(topLeft-TPoint(50,50),testWinSize2);
	testWin1->SetUpdateInRedraw(aUpdateInRedraw);
	testWin2->SetUpdateInRedraw(aUpdateInRedraw);
//
	topLeft+=iCheckWin->Position()-iWinPos;
	CResetRedrawStoreWin* tW1=CreatePartialRedrawWinLC(topLeft,testWinSize1);
	CResetRedrawStoreWin* tW2=CreatePartialRedrawWinLC(topLeft-TPoint(50,50),testWinSize2);
	tW1->SetUpdateInRedraw(aUpdateInRedraw);
	tW2->SetUpdateInRedraw(aUpdateInRedraw);
//
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
//	
	const TInt KNumTestResets1=5;
	const TInt KNumTestResets2=15;
	const TInt KMaxIterationsPerReset=20;
	const TInt numTestResets=aTwoWins?KNumTestResets2:KNumTestResets1;
	const TInt maxTotalIterations=numTestResets*KMaxIterationsPerReset;
	TInt resets=0;
//
	const TInt startWsHeapCount=TheClient->iWs.HeapCount();
	TInt baseWsHeapCount=startWsHeapCount;
	TInt prevWsHeapCount=0;
	TInt totalIterations=0;
	testWin1->SetKeepGcActive(aKeepGcActive);
	if (aAnimateBothWins)
		testWin2->SetKeepGcActive(aKeepGcActive);
	do
		{
		testWin1->UpdateAnim(1);
		testWin1->UpdateAnim(1);
		testWin1->UpdateAnim(1);
		if (aAnimateBothWins)
			testWin2->UpdateAnim(3);
		if (aTwoWins)
			{
			tW1->UpdateAnim(1);
			tW1->UpdateAnim(2);
			if (aAnimateBothWins)
				{
				tW2->UpdateAnim(1);
				tW2->UpdateAnim(1);
				tW2->UpdateAnim(1);
				}
			}
		TBool failed=testWin1->Failed();
		TEST(!failed);
		if (failed)
			{
			_LIT(KLog,"Window had fail flag set");
			LOG_MESSAGE(KLog);
			}
//
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		if (aTwoWins && !aUpdateInRedraw)
			{
			TBool match=TheClient->iScreen->RectCompare(testRect1,testRect2);
			TEST(match);
			if (!match)
				{
				_LIT(KLog,"Rectangle Area doesn't match, resets=%d (TwoWins=%d,AnimateBoth=%d,KeepActive=%d,InRedraw=%d)");
				LOG_MESSAGE6(KLog,resets,aTwoWins,aAnimateBothWins,aKeepGcActive,aUpdateInRedraw);
				}
			}
		const TInt wsHeapCount=TheClient->iWs.HeapCount();
		TInt lowGap=wsHeapCount-baseWsHeapCount;
		TInt highGap=prevWsHeapCount-wsHeapCount;
		if (prevWsHeapCount>0 && ((aAnimateBothWins || aTwoWins)?highGap>0:lowGap<highGap))
			{
			baseWsHeapCount=wsHeapCount;
			resets++;
			}
		totalIterations++;
		if (totalIterations>=maxTotalIterations)
			{
			TEST(EFalse);
			_LIT(KLog,"Too many iterations, number %d, max expect %d");
			LOG_MESSAGE3(KLog,totalIterations,maxTotalIterations);
			break;
			}
		prevWsHeapCount=wsHeapCount;
		} while(resets<numTestResets);
	if (!aTwoWins && !aAnimateBothWins)
		{	// With two wins resetting of the redraw store will be out of sync, so heap won't be reset
		if (aTwoWins || aAnimateBothWins || aKeepGcActive || !aUpdateInRedraw)
			{	// First time around with aUpdateInRedraw causes extra redraw store buffer allocation
			const TInt endHeapCount=TheClient->iWs.HeapCount();
			const TInt KHeapSafetyMargin=4;	// Granularity of buffers can leave odd extra cells, hard to specify an exact amount
			TBool heapUsageError=(startWsHeapCount+KHeapSafetyMargin>=endHeapCount);
			TEST(heapUsageError);
			if (!heapUsageError)
				{
				_LIT(KLog,"Memory Allocation Error, Before=%d, After=%d (Allowable Margin=%d)");
				LOG_MESSAGE4(KLog,startWsHeapCount,endHeapCount,KHeapSafetyMargin);
				}
			}
		}
//
	CleanupStack::PopAndDestroy(4, testWin1);
	}

void CTRedrawStoring::RedrawStoreWithSetExtentL()
/* Test how the redraw store deals with windows changing their extent
*/
	{
	TSize testSize(4*iWinSize.iWidth/7, iWinSize.iHeight);
	TRect testRect1(iTestWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),testSize);
	TRect testRect2(iCheckWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),testSize);
	TSize testWinSize1a;
	TSize testWinSize2;
	TPoint winOffset1;
	TPoint winOffset2;
	GetTestWinSizeAndPos(1,winOffset1,testWinSize1a);
	GetTestWinSizeAndPos(0,winOffset2,testWinSize2);
	TPoint winPos1(iWinPos + winOffset1);
	enum TSetExtentTestMode {ESetExtentTestModeExpandXY,ESetExtentTestModeExpandY,ESetExtentTestModeShrinkXY,ESetExtentTestModeShrinkY,ESetExtentTestModeShrinkXExpandY,ESetExtentTestModeCount};
	for(TInt extMode=ESetExtentTestModeExpandXY;extMode<ESetExtentTestModeCount;extMode++)
		{
		enum TSetExtentInvalidateTestMode {ESetExtentInvalidateTestModeBefore,ESetExtentInvalidateTestModeAfter,ESetExtentInvalidateTestModeBeforeWithRedraw,ESetExtentInvalidateTestModeCount};
		for(TInt invalidateMode=ESetExtentInvalidateTestModeBefore;invalidateMode<ESetExtentInvalidateTestModeCount;invalidateMode++)
			{
			TSize testWinSize1b(testWinSize1a);
			switch(extMode)
				{
				case ESetExtentTestModeExpandXY:
					testWinSize1b.iWidth=iWinSize.iWidth/4;
					testWinSize1b.iHeight=iWinSize.iHeight/4;
					break;
				case ESetExtentTestModeExpandY:
					testWinSize1b.iHeight=iWinSize.iHeight/4;
					break;
				case ESetExtentTestModeShrinkXY:
					testWinSize1b.iWidth=iWinSize.iWidth/2;
					testWinSize1b.iHeight=iWinSize.iHeight/2;
					break;
				case ESetExtentTestModeShrinkY:
					testWinSize1b.iHeight=iWinSize.iHeight/2;
					break;
				case ESetExtentTestModeShrinkXExpandY:
					testWinSize1b.iWidth=iWinSize.iWidth/2;
					testWinSize1b.iHeight=iWinSize.iHeight/4;
					break;
				}

			CResetRedrawStoreWin* testWin1=CreatePartialRedrawWinLC(winPos1, testWinSize1b);
			CResetRedrawStoreWin* testWin2=CreatePartialRedrawWinLC(iWinPos + winOffset2, testWinSize2);
			CResetRedrawStoreWin* tW1=CreatePartialRedrawWinLC(iCheckWin->Position() + winOffset1, testWinSize1a);
			CResetRedrawStoreWin* tW2=CreatePartialRedrawWinLC(iCheckWin->Position() + winOffset2, testWinSize2);
			TheClient->Flush();
			TheClient->WaitForRedrawsToFinish();
//
			if (invalidateMode==ESetExtentInvalidateTestModeBeforeWithRedraw)
				{
				testWin1->PreSetSize(testWinSize1a);
				testWin1->DrawNow();
				}
			if (invalidateMode==ESetExtentInvalidateTestModeBefore)
				testWin1->Invalidate();
			testWin1->SetExt(winPos1,testWinSize1a);
			if (invalidateMode==ESetExtentInvalidateTestModeAfter)
				testWin1->Invalidate();
			TheClient->Flush();
			TBool redrawWaiting=TheClient->WaitUntilRedrawPending();
			TheClient->WaitForRedrawsToFinish();
			TInt testRet=TheClient->iScreen->RectCompare(testRect1,testRect2);
			if (!testRet)
				{
				TEST(EFalse);
				_LIT(KRedrawStoreSetExtentFail,"Fade Regions fail: extMode=%d, invalidateMode=%d");
				LOG_MESSAGE3(KRedrawStoreSetExtentFail,extMode,invalidateMode);
				}
//
			CleanupStack::PopAndDestroy(4, testWin1);
			}
		}
	}

void CTRedrawStoring::PartialRedrawWithEmptyRedrawStoreL()
	{
	for(TInt numWins=1;numWins<4;numWins++)
		{
		const TInt KNumTestFlags=3;
		for(TUint flags=0;flags<(1<<KNumTestFlags);flags++)
			DoPartialRedrawWithEmptyRedrawStoreL(numWins,flags&0x1,flags&0x2,flags&0x4);
		}
	}
	
void CTRedrawStoring::DoPartialRedrawWithEmptyRedrawStoreL(TInt aNumWins, TBool aDoWinOnTop, TBool aRedrawWindow, TBool aChildWindows)
/* This code has been written to verify how the partial redraw store deals with the
case where it gets a partial redraw at a time when the redraw store is empty and awaiting
a replacement set of commands using low priority redraws.
*/
	{
	if (aChildWindows && aNumWins<2)
		return;	// No point in this one, same as without flag set
	TSize testWinSize1(iWinSize.iWidth/3, iWinSize.iHeight/3);
	TSize testWinSize2(iWinSize.iWidth/2, iWinSize.iHeight/2);
	TSize testWinSize3(iWinSize.iWidth*2/3, iWinSize.iHeight/4);
	TPoint winOffset1(iWinSize.iWidth/2,iWinSize.iHeight/2);
	TPoint nullPos;
	if (aChildWindows)
		{
		testWinSize1.iWidth*=2;
		testWinSize1.iHeight*=2;
		winOffset1.iX=Min(50,iWinSize.iWidth-testWinSize1.iWidth);
		winOffset1.iY=50;
		}
	CResetRedrawStoreWin* testWin1=CreatePartialRedrawWinLC(iWinPos + winOffset1, testWinSize1);
	CResetRedrawStoreWin* tW1=CreatePartialRedrawWinLC(iCheckWin->Position() + winOffset1, testWinSize1);
	CResetRedrawStoreWin* testWin2=NULL;
	CResetRedrawStoreWin* testWin3=NULL;
	if (aChildWindows)
		{
		TPoint winOffset2(TPoint(testWinSize1.iWidth/4,testWinSize1.iHeight/3));
		testWin2=CreatePartialRedrawWinLC(winOffset2, testWinSize2, aChildWindows?testWin1:NULL);
		CreatePartialRedrawWinLC(winOffset2, testWinSize2, aChildWindows?tW1:NULL);
		if (aNumWins>2)
			{
			TPoint winOffset3(TPoint(testWinSize1.iWidth/2,testWinSize1.iHeight*2/3));
			testWin3=CreatePartialRedrawWinLC(winOffset3, testWinSize3, aChildWindows?testWin1:NULL);
			CreatePartialRedrawWinLC(winOffset3, testWinSize3, aChildWindows?tW1:NULL);
			}
		}
	else
		{
		if (aNumWins>1)
			{
			TPoint winOffset2(TPoint(50,50));
			testWin2=CreatePartialRedrawWinLC(iWinPos + winOffset2, testWinSize2, aChildWindows?testWin1:NULL);
			CreatePartialRedrawWinLC(iCheckWin->Position() + winOffset2, testWinSize2, aChildWindows?tW1:NULL);
			if (aNumWins>2)
				{
				TPoint winOffset3(TPoint(iWinSize.iWidth/6,iWinSize.iHeight/3));
				testWin3=CreatePartialRedrawWinLC(iWinPos + winOffset3, testWinSize3, aChildWindows?testWin1:NULL);
				CreatePartialRedrawWinLC(iCheckWin->Position() + winOffset3, testWinSize3, aChildWindows?tW1:NULL);
				}
			}
		}
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	iDrawMode=EClientRedrawsNormal;
//
	TSize testSize(4*iWinSize.iWidth/7, iWinSize.iHeight);
	TRect testRect1(iTestWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),testSize);
	TRect testRect2(iCheckWin->BaseWin()->InquireOffset(*TheClient->iGroup->WinTreeNode()),testSize);
	TRect partialRedrawRect1(0,testWinSize1.iHeight/4,testWinSize1.iWidth,testWinSize1.iHeight*3/4);
	TRect partialRedrawRect2(0,testWinSize2.iHeight/4,testWinSize1.iWidth,testWinSize2.iHeight*3/4);
	TRect partialRedrawRect3(testWinSize3.iWidth/4,0,testWinSize3.iWidth*3/4,testWinSize3.iHeight);
	iBlankWin.SetExtent(iWinPos+winOffset1+partialRedrawRect1.iTl, partialRedrawRect1.Size());
	const TInt KDoWindow1=0x01;
	const TInt KDoWindow2=0x02;
	const TInt KDoWindow3=0x04;
	TInt numWinModes=1<<aNumWins;
	for(TInt invalidateWindowFlags=0;invalidateWindowFlags<numWinModes;invalidateWindowFlags++)
		{
		TheClient->iWs.ClearAllRedrawStores();
		if (invalidateWindowFlags&KDoWindow1)
			testWin1->Invalidate(partialRedrawRect1);
		if (invalidateWindowFlags&KDoWindow2)
			testWin2->Invalidate(partialRedrawRect2);
		if (invalidateWindowFlags&KDoWindow3)
			testWin3->Invalidate(partialRedrawRect3);
		if (aRedrawWindow)
			{
			if (invalidateWindowFlags&KDoWindow1)
				testWin1->Redraw(partialRedrawRect1);
			if (invalidateWindowFlags&KDoWindow2)
				testWin1->Redraw(partialRedrawRect2);
			if (invalidateWindowFlags&KDoWindow3)
				testWin1->Redraw(partialRedrawRect3);
			}
		if (aDoWinOnTop)
			{
			iBlankWin.SetOrdinalPosition(0);
			iBlankWin.SetVisible(ETrue);
			iBlankWin.SetVisible(EFalse);
			}
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
	//
		TBool match=TheClient->iScreen->RectCompare(testRect1,testRect2);
		TEST(match);
		if (!match)
			{
			_LIT(KLog,"Rectangle area doesn't match, windows=%d, flags=%d,%d,%d");
			LOG_MESSAGE5(KLog,aNumWins,aDoWinOnTop,aRedrawWindow,aChildWindows);
			}
		}
	// Resetting iBlankWin to its original size and position for future use 
	// by other test cases
	iBlankWin.SetExtent(iWinPos,iWinSize);
	// window handles which are pushed onto cleanup stack, shall be deleted here
	CleanupStack::PopAndDestroy(aNumWins*2,testWin1);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-103713-0001

@SYMDEF             PDEF106998

@SYMTestCaseDesc    Empty Draw Test

@SYMTestPriority    

@SYMTestStatus      Implemented

@SYMTestActions     Draws an empty base window followed by an empty top window. 
					 The top window is drawn in one of two cases: 
				     completely covering the bottom window or 
				     only covering the area that will be drawn to
  
  					A red rectangle is drawn to the bottom window and the top window is made invisible.
 
@SYMTestExpectedResults  The tested pixel colour should be red. Test will fail if the red rectangle 
							is not drawn or if an infinite loop is detected.

*/
void CTRedrawStoring::DoEmptyDrawTestL(TInt aTestMode)
 	{
 	_LIT(KErrorMessage,"Infinite Loop");

	TPartialRedrawType type = iTest->RedrawStoreTypeL();
 	if(type==EPartialRedraw_FullRedrawSupport)
		{
		TBool testStatus = EFalse;
			
	 	//draw an empty, green base window
	 	CPartialRedrawEmptyWin* bottomWin = new (ELeave) CPartialRedrawEmptyWin();
	 	CleanupStack::PushL(bottomWin);
	 	bottomWin->ConstructL(*TheClient->iGroup);
	 	bottomWin->Init(KRgbGreen);
	 	bottomWin->AssignGC(*TheClient->iGc);
	 	bottomWin->BaseWin()->SetShadowDisabled(ETrue);
	 	bottomWin->BaseWin()->SetShadowHeight(0);
	 	bottomWin->SetExt(iWinPos+TPoint(10,10), iWinSize);
	 	bottomWin->Win()->Activate();
	 	bottomWin->DrawPartial(TRect(TPoint(0,0), iWinSize));
	 	TheClient->Flush();

	 	//draw an empty, blue top window
	 	CPartialRedrawEmptyWin* topWin = new (ELeave) CPartialRedrawEmptyWin();
	 	CleanupStack::PushL(topWin);
	 	topWin->ConstructL(*TheClient->iGroup);
	 	topWin->Init(KRgbBlue);
	 	topWin->AssignGC(*TheClient->iGc);
	 	topWin->BaseWin()->SetShadowDisabled(ETrue);
	 	topWin->BaseWin()->SetShadowHeight(0);

	 	switch(aTestMode)
	 		{
	 		case 0:
	 		// top window is completely covering the base window
		 	topWin->SetExt(iWinPos+TPoint(10,10), iWinSize);
	 			break;
	 		case 1:
	 		// top window only covers the upper left hand corner, 
	 		//  over where the red rectangle will be drawn
		 	topWin->SetExt(iWinPos+TPoint(-5,-5), iWinSize);
				break;
	 		}

	 	topWin->Win()->Activate();		
	 	topWin->DrawPartial(TRect(TPoint(0,0), iWinSize));
		TheClient->Flush();

	 	//Invalidate the an area on the top window. 
	 	TRect smallrect(TPoint(10,10), TSize(iWinSize.iWidth/4, iWinSize.iHeight/4));
	 	topWin->Win()->Invalidate(smallrect);
	 	
	 	//draw a small red rectangle on the bottom window
		bottomWin->Win()->BeginRedraw(smallrect);
	 	bottomWin->Gc()->Activate(*bottomWin->Win());
	 	bottomWin->Gc()->SetBrushStyle(CGraphicsContext::ESolidBrush);
	 	bottomWin->Gc()->SetBrushColor(KRgbRed);
	 	bottomWin->Gc()->SetPenStyle(CGraphicsContext::ESolidPen);
	 	bottomWin->Gc()->SetPenColor(0);	
	 	bottomWin->Gc()->DrawRect(smallrect);
	 	bottomWin->Gc()->Deactivate();
	 	bottomWin->Win()->EndRedraw();
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();

		// hide the top window, so that the bottom window will be redrawn
		topWin->SetVisible(EFalse);
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();

		// check to see if an 'infinite' loop occured
		if ((topWin->ReturnCount() > KEmptyLoopThreshold) || (bottomWin->ReturnCount() > KEmptyLoopThreshold))
			INFO_PRINTF1(KErrorMessage);

	 	//get the color of a pixel within the invalid area that should be coloured 
	 	//red if the window is redrawn correctly.
	 	TPoint point =iWinPos + TPoint(30,30); 
	 	TRgb colour;
	 	TheClient->iScreen->GetPixel(colour,point);
	 	TRgb expectedColour = KRgbRed;

	 	if ((colour == expectedColour) && (topWin->ReturnCount() < KEmptyLoopThreshold) && (bottomWin->ReturnCount() < KEmptyLoopThreshold))
	 		testStatus = ETrue;
	 	
	 	TEST(testStatus);
	 	
		CleanupStack::PopAndDestroy(2, bottomWin);
		}
 	}

void CTRedrawStoring::DoPolygonRedrawTestSetL()
	{
	_LIT(KRedrawStoringPolygon0,"Test polygon redraw in opaque window");
	_LIT(KRedrawStoringPolygon1,"Test polygon low priority redraw in opaque window");
	_LIT(KRedrawStoringPolygon2,"Test polygon redraw in transparent window");
	_LIT(KRedrawStoringPolygon3,"Test polygon low priority redraw in transparent window");
	INFO_PRINTF1(KRedrawStoringPolygon0);
	DoPolygonRedrawTestL(0,0);			// Polygon redraw in opaque window
	INFO_PRINTF1(KRedrawStoringPolygon1);
	DoPolygonRedrawTestL(0,1);			// Polygon low priority redraw in opaque window
	INFO_PRINTF1(KRedrawStoringPolygon2);
	DoPolygonRedrawTestL(1,0);			// Polygon redraw in transparent window
	INFO_PRINTF1(KRedrawStoringPolygon3);
	DoPolygonRedrawTestL(1,1);			// Polygon low priority redraw in transparent window
	}
 	
void CTRedrawStoring::DoPolygonRedrawTestL(TInt aWindowMode, TInt aTestMode)
 	{
	//Used to place windows.
	TInt gap = 5;
	const TSize scrSize(TheClient->iScreen->SizeInPixels());
	TheClient->iWs.SetBufferSizeL(640);

	CPartialRedrawPolygonWin* polyTestWin = NULL;
	if (aTestMode == 0)	//If polygon redraw test.
		{
		//Draw a green test window with a polygon in it.
		polyTestWin = new (ELeave) CPartialRedrawPolygonWin();
	 	CleanupStack::PushL(polyTestWin);
	 	polyTestWin->ConstructL(*TheClient->iGroup);
	 	polyTestWin->Init(aWindowMode, KRgbGreen);	//aWindowMode 0=opaque 1=transparent
	 	polyTestWin->AssignGC(*TheClient->iGc);
	 	polyTestWin->BaseWin()->SetShadowDisabled(ETrue);
	 	polyTestWin->BaseWin()->SetShadowHeight(0);
	 	polyTestWin->SetExt(TPoint(scrSize.iWidth-iWinSize.iWidth,0), iWinSize);
	 	polyTestWin->Win()->Activate();
	 	polyTestWin->DrawPartial();
	 	TheClient->Flush();
		}

	//Draw a green base window with a polygon in it.
 	CPartialRedrawPolygonWin* bottomWin = new (ELeave) CPartialRedrawPolygonWin();
 	CleanupStack::PushL(bottomWin);
 	bottomWin->ConstructL(*TheClient->iGroup);
 	bottomWin->Init(aWindowMode, KRgbGreen);	//aWindowMode 0=opaque 1=transparent
 	bottomWin->AssignGC(*TheClient->iGc);
 	bottomWin->BaseWin()->SetShadowDisabled(ETrue);
 	bottomWin->BaseWin()->SetShadowHeight(0);
 	bottomWin->SetExt(TPoint(scrSize.iWidth-(2*iWinSize.iWidth)-gap,0), iWinSize);
 	bottomWin->Win()->Activate();
 	bottomWin->DrawPartial();
 	TheClient->Flush();

	//Draw an empty, blue transparent top window.
 	CPartialRedrawEmptyWin* topWin = new (ELeave) CPartialRedrawEmptyWin();
 	CleanupStack::PushL(topWin);
 	topWin->ConstructL(*TheClient->iGroup);
 	topWin->Init(KRgbBlue);
 	topWin->AssignGC(*TheClient->iGc);
 	topWin->BaseWin()->SetShadowDisabled(ETrue);
 	topWin->BaseWin()->SetShadowHeight(0);
 	topWin->SetExt(TPoint(scrSize.iWidth-(2*iWinSize.iWidth)-gap,0), iWinSize);
 	topWin->Win()->Activate();		
 	topWin->DrawPartial(TRect(TPoint(0,0), iWinSize));
	TheClient->Flush();

	if (aTestMode == 1)		//If polygon low priority redraw test.
		{
		//Clear all redraw stores.
		TheClient->iWs.ClearAllRedrawStores();
		TheClient->Flush();
		TheClient->WaitForRedrawsToFinish();
		}

	//Hide the top window, so the bottom window will be redrawn.
	topWin->SetVisible(EFalse);
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	if (aTestMode==0)		//If polygon redraw test.
		{
		//Compare bottomWin against polyTestWin.
		TEST(TheClient->iScreen->RectCompare(TRect(TPoint(scrSize.iWidth-2*iWinSize.iWidth-gap,0),iWinSize),TRect(TPoint(scrSize.iWidth-iWinSize.iWidth,0),iWinSize)));
		CleanupStack::PopAndDestroy(3,polyTestWin);
		}
	else					//If polygon low priority redraw test.
		{
		//Test bottomWin has only called DoDraw once.
		TEST(bottomWin->ReturnCount()==1);
		if (bottomWin->ReturnCount()!=1)
			{
			_LIT(KLog,"Number of redraws of bottom window %d, 1 expected (windowMode %d)");
			LOG_MESSAGE3(KLog,bottomWin->ReturnCount(),aWindowMode);
			}
		CleanupStack::PopAndDestroy(2,bottomWin);
		}
	}

void CTRedrawStoring::DoRedrawOOMTestL()
	{
	_LIT(KFailedTestInfo,"Failure information: redrawCount=%d  failRate=%d");
	_LIT(KCompletedTest,"OOM test started succeeding at failRate = %d");
	const TInt KConsecutiveSuccessfulRedraws = 20;
	
	//draw a white test window
	CRedrawRectWin* testWin = new (ELeave) CRedrawRectWin();
	CleanupStack::PushL(testWin);
	testWin->ConstructL(*TheClient->iGroup);
	testWin->Init();
	testWin->AssignGC(*TheClient->iGc);
	testWin->BaseWin()->SetShadowDisabled(ETrue);
	testWin->BaseWin()->SetShadowHeight(0);
	testWin->SetExt(iWinPos+TPoint(0,0), iWinSize);
	testWin->Win()->Activate();
	testWin->DrawNow();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();
	
	TPoint pointTest = iWinPos + TPoint(30,30);
	TRgb colourTest;
	TRgb expectedColour = KRgbGreen;
	TInt numberOfSuccessfulRedraws = 0;
	TInt failRate = 1;
	do
		{
		expectedColour=((expectedColour==KRgbGreen)?KRgbRed:KRgbGreen);
		testWin->ResetWindow(expectedColour);
		testWin->SetLogging(failRate<3?this:NULL);
		testWin->Win()->Invalidate();
		TheClient->iWs.HeapSetFail(RHeap::EDeterministic,failRate);
		TheClient->WaitForRedrawsToFinish();
		TheClient->iWs.HeapSetFail(RHeap::ENone,0);
		TheClient->iScreen->GetPixel(colourTest,pointTest);
		const TInt redrawCount = testWin->RedrawCount();

		if (redrawCount>2)				//If DoDraw called too often:
			{
			TBool passed=(failRate<3 && redrawCount<9);		//For a failrate of 2 allow upto 8 redraws
			TEST(passed);					//Fail.
			LOG_MESSAGE3(KFailedTestInfo,redrawCount,failRate);
			if (!passed)
				{
				CleanupStack::PopAndDestroy(testWin);
				return;
				}
			}
		else if (colourTest==expectedColour && redrawCount==1)	//If drawn correctly.
			{
		#if defined(LOGGING)
			_LIT(KLog,"FailRate %d  Drawing Corect  RedrawCount %d");
			LOG_MESSAGE3(KLog,failRate,redrawCount);
		#endif
			numberOfSuccessfulRedraws++;
			}
		else									//If not drawn.
			{
		#if defined(LOGGING)
			_LIT(KLog,"FailRate %d  Drawing Wrong   RedrawCount %d");
			LOG_MESSAGE3(KLog,failRate,redrawCount);
		#endif
			numberOfSuccessfulRedraws=0;
			}
		failRate++;
		} while (numberOfSuccessfulRedraws<KConsecutiveSuccessfulRedraws);
	LOG_MESSAGE2(KCompletedTest,(failRate-KConsecutiveSuccessfulRedraws-1));
	CleanupStack::PopAndDestroy(testWin);
	}

void CTRedrawStoring::RedrawStoreWithBadRectL()
	{
	RWindow win(TheClient->iWs);
	CleanupClosePushL(win);
	User::LeaveIfError(win.Construct(*TheClient->iGroup->GroupWin(),ENullWsHandle));
	win.SetRequiredDisplayMode(EColor64K);
	TPoint winPos(270,70);
	win.SetExtent(winPos, TSize(100,100));
	win.SetBackgroundColor( KRgbRed );
	win.Activate();
	
	TheGc->Activate(win);
	win.BeginRedraw();
	TheGc->SetBrushColor(KRgbGreen);
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
 	TheGc->DrawRect(TRect(0,0,100,40));
	win.EndRedraw();
		
	win.BeginRedraw(TRect(10,20,20,0));
	TheGc->SetBrushColor(KRgbBlue);
 	TheGc->DrawRect(TRect(0,0,40,100));
	win.EndRedraw();
	
	win.SetVisible(EFalse);
	win.SetVisible(ETrue);
   	TheGc->Deactivate();
   	TheClient->Flush();
   	
	TRgb color;
	TheClient->iScreen->GetPixel(color,winPos+TPoint(20,20));
	TBool passed=(color==KRgbGreen);
	TEST(passed);
   	
	CleanupStack::Pop(&win);
	win.Close();
	}


/*CPartialRedrawEmptyWin*/

void CPartialRedrawEmptyWin::Init(TRgb aColor)
	{
	Win()->SetRequiredDisplayMode(EColor16MA);
	Win()->SetTransparencyAlphaChannel();
	Win()->SetBackgroundColor(aColor);
	iCount = 0;
	}

void CPartialRedrawEmptyWin::Draw()
	{
	DoDraw();
	iCount++;
	}

void CPartialRedrawEmptyWin::DoDraw()
	{
	DrawFullWindowRect();
	}

void CPartialRedrawEmptyWin::DrawFullWindowRect()
	{
	// Only draw when we've looped too many times
	if (ReturnCount() > KEmptyLoopThreshold)
		{
		TRect rect = TRect(TPoint(0,0),iSize);
		iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc->SetBrushColor(KRgbBlack);
		iGc->DrawRect(rect);
		}
	}

void CPartialRedrawEmptyWin::DrawPartial(TRect aRect)
	{
	Invalidate(aRect);
	Win()->BeginRedraw(aRect);
	iGc->Activate(*Win());
	DrawFullWindowRect();
	iGc->Deactivate();
	Win()->EndRedraw();
	}

inline TInt CPartialRedrawEmptyWin::ReturnCount()
	{
	return iCount;
	}


/*CPartialRedrawPolygonWin*/

void CPartialRedrawPolygonWin::Init(TInt aWindowMode, TRgb aColor)
	{
	Win()->SetRequiredDisplayMode(EColor16MA);
	if (aWindowMode == 1)
		{
		Win()->SetTransparencyAlphaChannel();
		}
	Win()->SetBackgroundColor(aColor);
	iCount = 0;
	}

void CPartialRedrawPolygonWin::Draw()
	{
	DoDraw();
	iCount++;
	}

void CPartialRedrawPolygonWin::DoDraw()
	{
	DrawFullWindowPolygonL();
	}

void CPartialRedrawPolygonWin::DrawFullWindowPolygonL()
	{
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(KRgbBlack);
	CArrayFixFlat<TPoint>* longPolygon = new CArrayFixFlat<TPoint>(84);
	CleanupStack::PushL(longPolygon);
	TInt forLoop = 0, loopValue = 0;
	TInt tempX=18, tempY=49;
	TPoint polygonPoint(tempX, tempY);
	
	//Create jagged line for the polygon
	for (forLoop=0; forLoop<81; forLoop++)
		{
		tempX += 2;
		if (loopValue==0)
			{
			tempY +=2;
			loopValue = 1;
			}
		else
			{
			tempY -=2;
			loopValue = 0;
			}
		polygonPoint.SetXY(tempX, tempY);
		longPolygon->AppendL(polygonPoint);
		}
	polygonPoint.SetXY(tempX,70);
	longPolygon->AppendL(polygonPoint);
	polygonPoint.SetXY(20,70);
	longPolygon->AppendL(polygonPoint);
	iGc->DrawPolygon(longPolygon);
	CleanupStack::PopAndDestroy(longPolygon);
	}

void CPartialRedrawPolygonWin::DrawPartial()
	{
	Invalidate();
	Win()->BeginRedraw();
	iGc->Activate(*Win());
	DrawFullWindowPolygonL();
	iGc->Deactivate();
	Win()->EndRedraw();
	}

inline TInt CPartialRedrawPolygonWin::ReturnCount()
	{
	return iCount;
	}


/*CRedrawRectWin*/

void CRedrawRectWin::Init()
	{
	Win()->SetRequiredDisplayMode(EColor16MA);
	Win()->SetTransparencyAlphaChannel();
	Win()->SetBackgroundColor(KRgbWhite);
	iRedrawCount = 0;
	iRectColour = KRgbGreen;
	}

void CRedrawRectWin::Draw()
	{
	DoDraw();
	iRedrawCount++;
	}

void CRedrawRectWin::DoDraw()
	{
	DrawFullWindowRect();
	}

void CRedrawRectWin::DrawFullWindowRect()
	{
	TRect Rect(TPoint(10,10), TSize(30, 30));
	Gc()->SetBrushStyle(CGraphicsContext::ESolidBrush);
	Gc()->SetBrushColor(iRectColour);
	Gc()->SetPenStyle(CGraphicsContext::ESolidPen);
	Gc()->SetPenColor(0);	
	Gc()->DrawRect(Rect);
	}

void CRedrawRectWin::DrawNow()
	{
	Win()->Invalidate();
	Win()->BeginRedraw();
	Gc()->Activate(*Win());
	DrawFullWindowRect();
	Gc()->Deactivate();
	Win()->EndRedraw();
	}

inline TInt CRedrawRectWin::RedrawCount()
	{
	return iRedrawCount;
	}

void CRedrawRectWin::ResetWindow(TRgb aColour)
	{
	iRectColour = aColour;
	iRedrawCount = 0;
	}

inline void CRedrawRectWin::SetLogging(CTWsGraphicsBase* aTest)
	{
	iLog=aTest;
	}

void CRedrawRectWin::Redraw(const TRect& aRect)
	{
	if (iLog)
		{
		_LIT(KLog,"Redraw Count %d  Rect=(%d,%d,%d,%d)");
		iLog->LOG_MESSAGE6(KLog,RedrawCount(),aRect.iTl.iX,aRect.iTl.iY,aRect.iBr.iX,aRect.iBr.iY);
		}
	CTWin::Redraw(aRect);
	}


//

void CTRedrawStoring::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KNormalDrawing,"Normal Draw Test");
	_LIT(KFadeWindow1,"Fade Window1");
	_LIT(KFadeWindow2,"Fade Window2");
	_LIT(KRedrawQueue2,"Empty Redraw Queue");
	_LIT(KDisableRedrawStore,"Disable redraw store");
	_LIT(KResizeRedraws,"Redraw on Resize event");
	_LIT(KFontCacheOverflow,"Font Cache Overflow test");
	_LIT(KDrawBitmapMask,"Test DrawBitmapMasked");
	_LIT(KInvisibleRedrawStore,"Test invisible window redraw storing");
	_LIT(KBrushDraw,"Test UseBrushPattern storing");
	_LIT(KInvisibleRedrawStoreTransparent,"Test invisible transparent window redraw storing");
	_LIT(KPartialDrawNow,"Test partial DrawNow");
	_LIT(KPartialDrawNowTransparent,"Test partial transparent DrawNow");
	_LIT(KBeginEndRedraw,"Redraw in between Begin and EndRedraw");
	_LIT(KRedrawStoreAlphaChannelTransparency,"Redraw store for Alpha Channel Transparency");
	_LIT(KDrawBitBltAndMaskedNegTestsL,"Test BitBltMasked by passing Neg,UnExpected Values");
	_LIT(KRedrawStoringExposeWindow,"Redraw Storing Window Exposed");
	_LIT(KRedrawStoringExposeWindow2,"Redraw Storing Window behind Transparent Exposed");
	_LIT(KAutoResetRedrawStore,"Test automatic redraw store reset");
	_LIT(KRedrawStoreWithSetExtent,"Redraw store with set extent");
	_LIT(KPartialRedrawWithEmptyRedrawStore,"Partial redraw with empty redraw store");
	_LIT(KScrollingWin,"Test scrolling when partial redraw is enabled");
	_LIT(KRedrawStoringEmptyDrawWindow0,"Empty window under redraw storing - full case");
	_LIT(KRedrawStoringEmptyDrawWindow1,"Empty window under redraw storing - corner case");
	_LIT(KRedrawOOMTest,"Testing OOM redraw");
	_LIT(KRedrawWithBadRect, "Redraw storing when BeginRedraw with bad rect");
	if (iState==0)
		{
		// Check to see if Transparency is enabled before running tests
		if (TransparencySupportedL()==KErrNotSupported)
			{
			_LIT(KLog,"Transparency is not enabled");
			LOG_MESSAGE(KLog);
			TestComplete();
			return;
			}
		}
	TInt err=KErrNone;
	//if (iTest->iState==1) iTest->iState=KLastDrawingCase+2;	//Set one less that the test you want to run
	iState=++iTest->iState;
	((CTRedrawStoringStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(iTest->iState)
		{
	case 6:
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0086"));
		// Special case handled seperately because unfading the 
		// window requires a redraw
		iTest->LogSubTest(KFadeWindow1);
		FadeWindowTest();
		break; 
	case 12:
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0087"));
		iTest->LogSubTest(KFadeWindow2);
		FadeWindowTest2L();
		break;
	case 14:
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0090"));
		iTest->LogSubTest(KRedrawQueue2);
		DoNothingInRedrawTest();
		break;
	case 17:
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0091"));
		iTest->LogSubTest(KDisableRedrawStore);
		DoDisableRedrawStoreTest();
		break;
	case 18:
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0092"));
		iTest->LogSubTest(KResizeRedraws);
		DoResizeTest();
		break;
	case 22:
/**
	@SYMTestCaseID GRAPHICS-WSERV-0508
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0508"));
		iTest->LogSubTest(KBeginEndRedraw);
		DoBeginEndRedraw();
		break;
	case 23:
/**
	@SYMTestCaseID GRAPHICS-WSERV-0509
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0509"));
		iTest->LogSubTest(KRedrawStoreAlphaChannelTransparency);
		DoRedrawStoreAlphaChannelTransTest();
		break;
	case 24:
/**
	@SYMTestCaseID GRAPHICS-WSERV-0510
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0510"));
		iTest->LogSubTest(KScrollingWin);
		ScrollWinTest();
		break;
	case KLastDrawingCase + 1:
/**
	@SYMTestCaseID GRAPHICS-WSERV-0511
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0511"));
		iTest->LogSubTest(KDrawBitmapMask);
		TRAP(err,DoTestDrawBitmapMaskedL());
		break;
	case KLastDrawingCase + 2:

		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0511"));
		iTest->LogSubTest(KDrawBitmapMask);
		TRAP(err,DoTestDrawBitmapMaskedL(ETrue));
		break;
	case KLastDrawingCase + 3:
/**
	@SYMTestCaseID GRAPHICS-WSERV-0512
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0512"));
		iTest->LogSubTest(KFontCacheOverflow);
		DoFontCacheOverflowTestL();
		break;
	case KLastDrawingCase + 4:
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-00XX-0006"));
		iTest->LogSubTest(KInvisibleRedrawStore);
		TRAP(err,DoInvisibleRedrawStoreTestL( EFalse ));
		break;
	case KLastDrawingCase + 5:
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-00XX-0006"));
		iTest->LogSubTest(KInvisibleRedrawStoreTransparent);
		TRAP(err,DoInvisibleRedrawStoreTestL( ETrue ));
		break;
	case KLastDrawingCase + 6:
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-CODEBASE-WSERV-0052-0001"));
		iTest->LogSubTest(KDrawBitBltAndMaskedNegTestsL);
		TRAP(err,DoBitBltAndMaskedNegTestsL());
		break;
	case KLastDrawingCase + 7:	
/**
	@SYMTestCaseID GRAPHICS-WSERV-0513
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0513"));
		iTest->LogSubTest(KPartialDrawNow);
		TRAP(err,DoPartialDrawNowTestL(EFalse));
		break;
	case KLastDrawingCase + 8:
/**
	@SYMTestCaseID GRAPHICS-WSERV-0514
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0514"));
		iTest->LogSubTest(KPartialDrawNowTransparent);
		TRAP(err,DoPartialDrawNowTestL(ETrue));
		break;
	case KLastDrawingCase + 9:
/**
	@SYMTestCaseID GRAPHICS-WSERV-0515
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0515"));
		TInt iteration;
		for(iteration=0;iteration<3 && err==KErrNone;iteration++)
			{
			iTest->LogSubTest(KRedrawStoringExposeWindow);
			TRAP(err,DoExposeTestL(iteration));
			}
		break;
	case KLastDrawingCase + 10:
		{
/**
	@SYMTestCaseID GRAPHICS-WSERV-0516
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0516"));
		TInt iteration;
		for(iteration=0;iteration<2 && err==KErrNone;iteration++)
			{
			iTest->LogSubTest(KRedrawStoringExposeWindow2);
			TRAP(err,DoExposeTest2L(iteration));
			}
		break;
		}
	case KLastDrawingCase + 11:
		((CTRedrawStoringStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		iTest->LogSubTest(KAutoResetRedrawStore);
		AutoResetRedrawStoreTestsL();
		break;
	case KLastDrawingCase + 12:
/**
	@SYMTestCaseID GRAPHICS-WSERV-0517
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0517"));
		iTest->LogSubTest(KRedrawStoreWithSetExtent);
		RedrawStoreWithSetExtentL();
		break;
	case KLastDrawingCase + 13:
/**
	@SYMTestCaseID GRAPHICS-WSERV-0518
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0518"));
		iTest->LogSubTest(KPartialRedrawWithEmptyRedrawStore);
		PartialRedrawWithEmptyRedrawStoreL();
		break;
	case KLastDrawingCase + 14:
	((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-103713-0001"));
		iTest->LogSubTest(KRedrawStoringEmptyDrawWindow0);
		TRAP(err,DoEmptyDrawTestL(0));	// Completely covered case
		if (err!=KErrNone)
			break;
		iTest->LogSubTest(KRedrawStoringEmptyDrawWindow1);
		TRAP(err,DoEmptyDrawTestL(1));	// Quarter covered case
		break;

/**
@SYMTestCaseID				GRAPHICS-WSERV-0439

@SYMDEF						DEF107817

@SYMTestCaseDesc			Drawing polygons with many points panics WServ (redraw store enabled)

@SYMTestPriority			Normal

@SYMTestStatus				Implemented

@SYMTestActions				Draw a polygon in opaque and transparent windows testing redraw and low priority redraw

@SYMTestExpectedResults		Redraw tests display correctly, low priority redraw tests call DoDraw only once
*/
	case KLastDrawingCase + 15:
	((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("	GRAPHICS-WSERV-0439"));
		TRAP(err,DoPolygonRedrawTestSetL());
		break;

/**
@SYMTestCaseID				GRAPHICS-WSERV-0442

@SYMDEF						DEF107984

@SYMTestCaseDesc			OOM causing infinite redraw loop

@SYMTestPriority			Normal

@SYMTestStatus				Implemented

@SYMTestActions				Redraw rectangles in OOM situations

@SYMTestExpectedResults		There are no extended redraw loops
*/
	case KLastDrawingCase + 16:
	    ((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0442"));
		iTest->LogSubTest(KRedrawOOMTest);
		TRAP(err,DoRedrawOOMTestL());
		break;
	case KLastDrawingCase + 17:
/**
	@SYMTestCaseID GRAPHICS-WSERV-0519
*/
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0519"));
		iTest->LogSubTest(KRedrawWithBadRect);
		TRAP(err,RedrawStoreWithBadRectL());
		break;
	case KLastDrawingCase + 18:
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0498"));
		iTest->LogSubTest(KBrushDraw);
		TRAP(err,DoBrushDrawTestL());
		break;
	case KLastDrawingCase + 19:
		((CTRedrawStoringStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTRedrawStoringStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break; 
	default:
		iTest->LogSubTest(KNormalDrawing);
		((CTRedrawStoringStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0085"));
		DoDrawTest();
		if (iDoScrollTest)
			ScrollTest();
		}
	((CTRedrawStoringStep*)iStep)->RecordTestResultL();
	if (err!=KErrNone)
		{
		TEST(EFalse);
		_LIT(KLog,"Sub-Test[%d] left with error code %d");
		LOG_MESSAGE3(KLog,iState,err);
		}
	}


__WS_CONSTRUCT_STEP__(RedrawStoring)
