// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 
#include "talphadrawing.h"
#include <hal.h>

// When enabled allows testing with a variable windows size rather than fixed size
#define _TESTWITHVARIABLEWINDOWSIZE

// Test bitmap file location
_LIT(KAlphaTestBitmap,"z:\\system\\data\\uibench_24bit.mbm");

const TInt KIterationsToTest = 100; 		// Number of iterations to run tests
const TInt KDelay = 100000; 		// 0.1 seconds
const TInt KRotationGranulatity = 8;// Rotation array granularity

/**
Clear window to selected colour
*/
LOCAL_C void ClearWindow(RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, TRgb aColor)
{
	// clear so we can see bitmap version has completed
	aWindow.Invalidate();
	aWindow.BeginRedraw();
	aGc->Activate(aWindow);
	aGc->SetBrushColor(aColor);
	aGc->Clear();
	aGc->Deactivate();
	aWindow.EndRedraw();
	aSession.Flush();
}

/**
Draws a b/w checkerboard onto a RWindow
*/
LOCAL_C void ChessBoard(RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc)
	{
	const TSize size = aWindow.Size();
	
	aGc->Activate(aWindow);
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetPenStyle(CGraphicsContext::ENullPen);

	aGc->SetBrushColor(KRgbBlack);
	aGc->Clear();
	aGc->SetBrushColor(KRgbWhite);
	TInt x0 = 0;
	TPoint point;
	const TInt checkerSize = 32;
	for(point.iY=0; point.iY < size.iHeight; point.iY += checkerSize)
		{
		for(point.iX=x0; point.iX < size.iWidth; point.iX += checkerSize*2)
			{
			TRect rect(point, TSize(checkerSize, checkerSize));
			aGc->DrawRect(rect);
			}
		x0 = checkerSize - x0;
		}
	aGc->Deactivate();
	aSession.Flush();
	}
	
/**
RBlankWindow on top layer gets set visible/invisible to measure timing of drawing the windows below
*/
void CTAlphaDrawing::DrawBlankWindowL (RWindow& /*aForeGndWin*/, TInt aIters)
	{	
	// Create a blank window that is the size of the screen so that we can set this visible to hide the window underneath
	// and then hide it to force redraw of the window underneath
	RBlankWindow blankWindow = RBlankWindow(iWsSession);
	CleanupClosePushL(blankWindow);
	User::LeaveIfError(blankWindow.Construct(iWinGroup, (TInt)this+1));	
	blankWindow.SetSize(iScreenDevice->SizeInPixels());
	blankWindow.SetColor(TRgb(0x3399ff));	
	blankWindow.Activate();
	
#ifdef _TESTWITHVARIABLEWINDOWSIZE
	// Create a small window that moves position
	RBlankWindow blankWindow2 = RBlankWindow(iWsSession);
	CleanupClosePushL(blankWindow2);
	User::LeaveIfError(blankWindow2.Construct(iWinGroup, (TInt)this+2));
	blankWindow2.SetSize(TSize(10,10));
	blankWindow2.SetColor(TRgb(0x99ff33));
	TInt px = iScreenDevice->SizeInPixels().iWidth/2;
	TInt py = iScreenDevice->SizeInPixels().iHeight/2;
	blankWindow2.SetPosition(TPoint(px, py));
	blankWindow2.Activate();
#endif	
	
	TBool onoff=ETrue;
	
	iProfiler->InitResults();
	for(TInt i=aIters; i>=0; i--)
		{
		blankWindow.SetVisible(onoff=!onoff);
				
#ifdef _TESTWITHVARIABLEWINDOWSIZE
		blankWindow2.SetSize(TSize(10+i, 10+i));
		blankWindow2.SetPosition(TPoint(px--, py--));
		if (px<0) px=150; 
		if (py<0) py=150;
#endif

		iWsSession.Flush();
		iProfiler->MarkResultSetL();
		}	
	
#ifdef _TESTWITHVARIABLEWINDOWSIZE
	CleanupStack::PopAndDestroy(&blankWindow2);
#endif
	CleanupStack::PopAndDestroy(&blankWindow);
	}

/**
Helper function to calculate processor cycles per pixel as a characteristic number for rendering speed
*/
TInt CTAlphaDrawing::CyclesPerPixel(TInt64 aDuration, TInt aIters, TInt aWidth, TInt aHeight, TInt aCPUSpeed) 
	{	
	TInt64 pixs=aWidth*aHeight;
	TInt64 cpuHz=aCPUSpeed;
	
	TInt64 ret=(aDuration*cpuHz)/pixs/1000/aIters;
	return ret;
	}

/*
Helper function that draws a window
*/
void CTAlphaDrawing::TestDrawWindowL(TDisplayMode aMode, TInt aIters, TTestCase aTestCase, const TDesC& aTestName)
	{
	RWindow foregndWindow = RWindow(iWsSession);
	User::LeaveIfError(foregndWindow.Construct(iWinGroup, (TInt)this+3));
	
	CFbsBitmap * bmpModeDep = CreateSoftwareBitmapLC(iScreenDevice->SizeInPixels(), aMode);
	CopyBitmapL(bmpModeDep, iSourceImage);
	
	foregndWindow.BeginRedraw();
	iWindowGc->Activate(foregndWindow);
	iWindowGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iWindowGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iWindowGc->DrawBitmap(iScreenDevice->SizeInPixels(), bmpModeDep, bmpModeDep->SizeInPixels());
	iWindowGc->Deactivate();
	foregndWindow.EndRedraw();
	iWsSession.Flush();
		
	switch (aTestCase)
		{
		case EUseOpaqueDraw:
			foregndWindow.SetNonTransparent();	
			break;
		case EUseTransparencyFactor:
			User::LeaveIfError(foregndWindow.SetTransparencyFactor(TRgb(0xbbbbbb,128)));
			break;
		}
		
	foregndWindow.Activate();

	// Start performance test and analyse results
	DrawBlankWindowL(foregndWindow, aIters);
	iProfiler->ResultsAnalysis(aTestName, 0, aMode, 0, aIters);

	foregndWindow.Close();
	
	CleanupStack::PopAndDestroy(bmpModeDep);
	User::After(KDelay);
	}

/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0013

   @SYMTestCaseDesc
   The test determines how long it takes to draw an opaque (non-transparent) window composition.

   @SYMTestActions
   Compare the results over time, and before and after changes to wserv code.

   @SYMTestExpectedResults
   Test should pass and display total test time and cycles per pixel
*/
void CTAlphaDrawing::DoTestDrawOpaqueWindowL(TDisplayMode aMode, TInt aIters) 
	{
	TestDrawWindowL(aMode, aIters, EUseOpaqueDraw, _L("WinOpaque"));
	}
	
/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0014

   @SYMTestCaseDesc
   The test determines how long it takes to draw transparent window compositions with window SetTransparencyFactor.

   @SYMTestActions
   Compare the results over time, and before and after changes to wserv code.

   @SYMTestExpectedResults
   Test should pass and display total test time and cycles per pixel
*/
void CTAlphaDrawing::DoTestDrawTransparentWindowFactorL(TDisplayMode aMode, TInt aIters) 
	{	
	TestDrawWindowL(aMode, aIters, EUseTransparencyFactor, _L("WinTrFac"));	
	}

/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0015

   @SYMTestCaseDesc
   The test determines how long it takes to draw transparent window compositions with SetTransparencyBitmap.

   @SYMTestActions
   Compare the results over time, and before and after changes to wserv code.

   @SYMTestExpectedResults
   Test should pass and display total test time and cycles per pixel
*/
void CTAlphaDrawing::DoTestDrawTransparentWindowBitmapL(TDisplayMode aMode, TInt aIters) 
	{	
	RWindow foregndWindow;
	foregndWindow = RWindow(iWsSession);
	User::LeaveIfError(foregndWindow.Construct(iWinGroup, (TInt)this+5));
	
	CFbsBitmap * sourceAlpha  = CreateSoftwareBitmapLC(iScreenDevice->SizeInPixels(), EGray256);
	VerticalGradientAlphaL(sourceAlpha, TRgb(0xffffffff), TRgb(0x00000000));
	
	CFbsBitmap * bmpModeDep  = CreateSoftwareBitmapLC(iScreenDevice->SizeInPixels(), aMode);
	CopyBitmapL(bmpModeDep, iSourceImage);
	
	foregndWindow.BeginRedraw();
	iWindowGc->Activate(foregndWindow);
	iWindowGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iWindowGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iWindowGc->DrawBitmap(iScreenDevice->SizeInPixels(), bmpModeDep, bmpModeDep->SizeInPixels());
	
	// Set transparency bitmap for this window
	User::LeaveIfError(foregndWindow.SetTransparencyBitmap(*sourceAlpha));
	
	iWindowGc->Deactivate();
	foregndWindow.EndRedraw();
	iWsSession.Flush();

	foregndWindow.Activate();
	
	DrawBlankWindowL(foregndWindow,aIters);
	iProfiler->ResultsAnalysis(_L("WinTrBmp"), 0, aMode, 0, aIters);
	
	foregndWindow.Close();
	
	CleanupStack::PopAndDestroy(2, sourceAlpha);
	User::After(KDelay);
	}

/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0016
@SYMTestPriority High
@SYMREQ 0000
@SYMTestCaseDesc 
Test test determines how long it takes to draw transparent window compositions with SetTransparencyAlphaChannel.

@SYMTestActions
Compare the results over time, and before and after changes to wserv code.

@SYMTestExpectedResults
Test should pass and display total test time and cycles per pixel
*/
void CTAlphaDrawing::DoTestDrawTransparentWindowAlphaChannelL(TDisplayMode aMode, TInt aIters) 
	{		
	RWindow foregndWindow;
	foregndWindow = RWindow(iWsSession);
	User::LeaveIfError(foregndWindow.Construct(iWinGroup, (TInt)this+6));	
	
	CFbsBitmap * sourceAlpha  = CreateSoftwareBitmapLC(iSourceImage->SizeInPixels(), EGray256);
	VerticalGradientAlphaL(sourceAlpha, TRgb(0xffffffff), TRgb(0x00000000));
	
	CFbsBitmap * bmpModeDep  = CreateSoftwareBitmapLC(iScreenDevice->SizeInPixels(), aMode);
	CopyBitmapL(bmpModeDep, iSourceImage);
		
	TPoint point;
	TRect  rect(iScreenDevice->SizeInPixels());
			
	foregndWindow.BeginRedraw();
	iWindowGc->Activate(foregndWindow);
	iWindowGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iWindowGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	
	iWindowGc->DrawBitmapMasked(rect, bmpModeDep, bmpModeDep->SizeInPixels(), sourceAlpha, EFalse);
	
	User::LeaveIfError(foregndWindow.SetTransparencyAlphaChannel());
	
	iWindowGc->Deactivate();
	foregndWindow.EndRedraw();
	iWsSession.Flush();

	foregndWindow.Activate();
	
	DrawBlankWindowL(foregndWindow,aIters);
	iProfiler->ResultsAnalysis(_L("WinTrAlphaCh"), 0, aMode, 0, aIters);
			
	foregndWindow.Close();
	CleanupStack::PopAndDestroy(2, sourceAlpha);			
	User::After(KDelay);
	}

/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0017

   @SYMTestCaseDesc
   Creates foreground window with alpha channel transparency.
   An image with alpha mask is bitblitted to this window.
   The test determines how long it takes to create a window

   @SYMTestActions
   Compare the results over time, and before and after changes to wserv code.

   @SYMTestExpectedResults
   Test should pass and display total test time and cycles per pixel
*/
void CTAlphaDrawing::DoTestCreateWindowL(TDisplayMode aMode, TInt aIters) 
	{
	RWindow foregndWindow;
	
	CFbsBitmap * sourceAlpha  = CreateSoftwareBitmapLC(iSourceImage->SizeInPixels(), EGray256);
	VerticalGradientAlphaL(sourceAlpha, TRgb(0xffffffff), TRgb(0x00000000));
		
	CFbsBitmap * bmpModeDep  = CreateSoftwareBitmapLC(iScreenDevice->SizeInPixels(), aMode);
	CopyBitmapL(bmpModeDep, iSourceImage);
	
	TPoint point;
	TRect  rect(iScreenDevice->SizeInPixels());
	iProfiler->InitResults();
	for(TInt i=KIterationsToTest; i>=0; i--)
		{
		foregndWindow = RWindow(iWsSession);
		User::LeaveIfError(foregndWindow.Construct(iWinGroup, (TInt)this+7));
		
		foregndWindow.BeginRedraw();
		iWindowGc->Activate(foregndWindow);
		iWindowGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		iWindowGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
		iWindowGc->DrawBitmapMasked(rect, bmpModeDep, bmpModeDep->SizeInPixels(), sourceAlpha, EFalse);
					
		TInt ret = foregndWindow.SetTransparencyAlphaChannel();
		if (ret == KErrNotSupported)
			ERR_PRINTF1(_L("Transparency not enabled - Turn on transparency in wsini.ini file"));			
		User::LeaveIfError(ret);			
		
		iWindowGc->Deactivate();
		foregndWindow.EndRedraw();
		iWsSession.Flush();

		foregndWindow.Activate();
				
		iWsSession.Flush();
		iProfiler->MarkResultSetL();
				
		foregndWindow.Close();
		}
	iProfiler->ResultsAnalysis(_L("WinCreation"), 0, aMode, 0, aIters);
	CleanupStack::PopAndDestroy(2, sourceAlpha);					
	User::After(KDelay);
	}

CTAlphaDrawing::~CTAlphaDrawing()
	{
	delete iSourceImage;
	delete iWindowGc;	
	delete iScreenDevice;
	iBackgndWindow.Close();
	iWinGroup.Close();
	iWsSession.Close();
	RFbsSession::Disconnect();
	}
	
CTAlphaDrawing::CTAlphaDrawing()
	{
	SetTestStepName(KTAlphaDrawing);
	}

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CTAlphaDrawing::doTestStepPreambleL()
	{			
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	
	HAL::Get(HALData::ECPUSpeed,iCPUSpeed);
	
	User::LeaveIfError(RFbsSession::Connect());
	User::LeaveIfError(iWsSession.Connect());

	iScreenDevice = new (ELeave) CWsScreenDevice(iWsSession);
	User::LeaveIfError(iScreenDevice->Construct(0));	// screen number (0 is first screen)
	
	iSourceImage  = LoadBitmapL(KAlphaTestBitmap,0);
	
	iWindowGc = new (ELeave) CWindowGc(iScreenDevice);
	User::LeaveIfError(iWindowGc->Construct());
	
	TDisplayMode windowMode = iScreenDevice->DisplayMode();
	
	iWinGroup  = RWindowGroup(iWsSession);
	User::LeaveIfError(iWinGroup.Construct(1, EFalse));
	
	iBackgndWindow = RWindow(iWsSession);
	User::LeaveIfError(iBackgndWindow.Construct(iWinGroup, (TInt)this+8));
	iBackgndWindow.Activate();
	
	iBackgndWindow.SetNonTransparent();	
	iBackgndWindow.Invalidate();
	
	iWsSession.Flush();	
	return TestStepResult();
	}
	
/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

Creates background window with black & white checker board. 
This background windows is used for each test case in this file

@return TVerdict code
*/	
TVerdict CTAlphaDrawing::doTestStepL()
	{	
	TInt iters = KIterationsToTest;
	
	INFO_PRINTF5(_L("TAlphaDrawing - Iterations:%d, CPUSpeed:%d kHz, Width: %d px, Height: %d px"),iters,iCPUSpeed,iScreenDevice->SizeInPixels().iWidth,iScreenDevice->SizeInPixels().iHeight);
	
	// tests to execute
	TBool iniok; TInt inival;
	iniok = GetIntFromConfig(_L("AlphaDrawingTests"), _L("testCreateWindowAlpha"), inival);
	TBool testCreateWindowAlpha					= ((iniok==EFalse) || (inival > 0));
	iniok = GetIntFromConfig(_L("AlphaDrawingTests"), _L("testDrawOpaqueWindow"), inival);	
	TBool testDrawOpaqueWindow					= ((iniok==EFalse) || (inival > 0));
	iniok = GetIntFromConfig(_L("AlphaDrawingTests"), _L("testDrawTransparentFactorWindow"), inival);
	TBool testDrawTransparentFactorWindow			= ((iniok==EFalse) || (inival > 0));
	iniok = GetIntFromConfig(_L("AlphaDrawingTests"), _L("testDrawTransparentWindowPerPixel"), inival);
	TBool testDrawTransparentWindowPerPixel			= ((iniok==EFalse) || (inival > 0));
	iniok = GetIntFromConfig(_L("AlphaDrawingTests"), _L("testDrawTransparentWindowPerPixelAlpha"), inival);
	TBool testDrawTransparentWindowPerPixelAlpha 	= ((iniok==EFalse) || (inival > 0));
	
	const TSize iWindowSize=iBackgndWindow.Size();
	
	// Initalise test - clear window and create checker board on background windows
	ClearWindow(iWsSession, iBackgndWindow, iWindowGc, BLACK_SEMI_TRANSPARENT);
	ChessBoard(iWsSession, iBackgndWindow, iWindowGc);
	
	iWsSession.Flush();
	
	TInt screenModesCnt=iScreenDevice->NumScreenModes();
	CArrayFixFlat<TInt> * rotationList=NULL;

	rotationList = new (ELeave) CArrayFixFlat<TInt>(KRotationGranulatity);
	CleanupStack::PushL(rotationList);
	
	for (TInt scm=0; scm<screenModesCnt; scm++) 
		{			
		iScreenDevice->GetRotationsList(scm,rotationList);
		
		for (TInt rot = 0; rot < rotationList->Count(); rot++) 
			{		
			TPixelsAndRotation	pxrot;
			pxrot.iPixelSize	=iWindowSize;
			
			iRotation=(CFbsBitGc::TGraphicsOrientation)rotationList->At(rot);
			pxrot.iRotation	=iRotation;
			
			iScreenDevice->SetScreenSizeAndRotation(pxrot);
			
			for(int m=0; m<1; m++)
				{
				// test drawing speed of window creation and destruction
				if ( testCreateWindowAlpha )
					{
					SetTestStepID(_L("GRAPHICS-UI-BENCH-0017"));
					DoTestCreateWindowL(KValidDisplayModes[m],iters);	
					RecordTestResultL();
					}
					
				// test drawing speed of an opaque window composition
				if ( testDrawOpaqueWindow )
					{	
					SetTestStepID(_L("GRAPHICS-UI-BENCH-0013"));
					DoTestDrawOpaqueWindowL(KValidDisplayModes[m],iters);	
					RecordTestResultL();
					}
				
				// test drawing speed of a transparent window composition with a transparency factor
				if ( testDrawTransparentFactorWindow )
					{
					SetTestStepID(_L("GRAPHICS-UI-BENCH-0014"));
					DoTestDrawTransparentWindowFactorL(KValidDisplayModes[m],iters);	
					RecordTestResultL();
					}
					
				// test drawing speed of a transparent window composition with a transparency bitmap
				if ( testDrawTransparentWindowPerPixel )
					{
					SetTestStepID(_L("GRAPHICS-UI-BENCH-0015"));
					DoTestDrawTransparentWindowBitmapL(KValidDisplayModes[m],iters);	
					RecordTestResultL();
					}
				
				// test drawing speed of a transparent window composition with alpha channel transparency
				if ( testDrawTransparentWindowPerPixelAlpha )
					{
					SetTestStepID(_L("GRAPHICS-UI-BENCH-0016"));
					DoTestDrawTransparentWindowAlphaChannelL(KValidDisplayModes[m],iters);	
					RecordTestResultL();
					}						
				} // m...Modes				
			} // rot ... Rotations			
		}	
	CleanupStack::PopAndDestroy(rotationList);
	TVerdict a = TestStepResult();
	return TestStepResult();
	}
