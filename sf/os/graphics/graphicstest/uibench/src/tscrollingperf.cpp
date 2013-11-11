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

#include "tscrollingperf.h"

const TInt KIterationsToTest = 16384;

CScrollingPerf::~CScrollingPerf()
	{
	delete iWGc;
	delete iWsScreenDevice;
	delete iBitmap32bit;
	iWs.Close();
	}

CScrollingPerf::CScrollingPerf()
	{
	SetTestStepName(KScrollingPerfName);
	}

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CScrollingPerf::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	User::LeaveIfError(iWs.Connect());
	iWsScreenDevice = new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iWsScreenDevice->Construct(0));
	User::LeaveIfError(iWsScreenDevice->CreateContext(iWGc));
	return TestStepResult();
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CScrollingPerf::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0019"));
	MoveClippedChildWindowL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0018"));
	ScrollRWindowL();
	RecordTestResultL();
	
	SetTestStepResult(EPass);	
	return TestStepResult();
	}

/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0018

   @SYMTestCaseDesc
   Tests how long it takes to scroll using the RWindow::Scroll API.

   @SYMTestActions
   Compare the results over time, and before and after changes to WSERV code.

   @SYMTestExpectedResults
   Test should pass and display total test time and time per Scroll() action
*/
void CScrollingPerf::ScrollRWindowL()
	{		
	const TRect cropTo(0,0,200,200);
	
	RWindowGroup topWind(iWs);
	CleanupClosePushL(topWind);
	User::LeaveIfError(topWind.Construct(1)); // cli hnd 1
	User::LeaveIfError(topWind.SetName(_L("ScrollRWindowL")));
	
	RWindow wind(iWs);
	CleanupClosePushL(wind);
	User::LeaveIfError(wind.Construct(topWind, 2)); // cli hnd 2
    wind.SetExtent(cropTo.iTl, cropTo.Size());
	wind.Activate();			
	
	const TInt scrollByX = 0;
	const TInt scrollByY = 2;
	const TRect invalidated(0, 0, 200, 2);
	
	iWGc->Activate(wind);
	iWGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	
	iProfiler->InitResults();
	for(TInt count=0; count<KIterationsToTest; ++count)
		{
		wind.Scroll(TPoint(scrollByX, scrollByY));
		wind.BeginRedraw();
		
		// do some bitshiftin' for fancy smooth colour bands
		// - otherwise there's little to see
		TInt step = (count >> 2);
		TInt r = (step & 0xf);
		TInt g = (step & 0xf0) >> 4;
		TInt b = (step & 0xf00) >> 8;
		if (step & 0x10) r = 15-r;
		if (step & 0x100) g = 15-g;
		r <<= 4;
		g <<= 4;
		b <<= 4;
		
		iWGc->SetPenColor(TRgb(r, g, b));
		iWGc->DrawRect(invalidated);
		wind.EndRedraw();
		iProfiler->MarkResultSetL();
		}
	
	iProfiler->ResultsAnalysis(_L("RWindowScrolling"), 0, 0, 0, KIterationsToTest);
	iWGc->Deactivate();
	CleanupStack::PopAndDestroy(2, &topWind);
	}

/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0019

   @SYMTestCaseDesc
   Tests how long it takes to move a child window against its parent window.

   @SYMTestActions
   Compare the results over time, and before and after changes to WSERV code.

   @SYMTestExpectedResults
   Test should pass and display total test time and time per SetPosition() action
*/
void CScrollingPerf::MoveClippedChildWindowL()
	{
	INFO_PRINTF1(_L("CScrollingPerf::MoveClippedChildWindowL"));
	
	const TRect parentExtent(32,32,160,160);
	TRect parentFill(0, 0, parentExtent.Width(), parentExtent.Height());
	parentFill.Shrink(16, 16);
	const TRect childExtent(0,0,256,256);
	TRect childFill(0, 0, childExtent.Width(), childExtent.Height());
	childFill.Shrink(16, 16);

	RWindowGroup topWind(iWs);
	CleanupClosePushL(topWind);
	topWind.Construct(1); // cli hnd 1
	User::LeaveIfError(topWind.SetName(_L("MoveClippedChildWindowL")));
	
	RWindow parentWind(iWs);
	CleanupClosePushL(parentWind);
	User::LeaveIfError(parentWind.Construct(topWind, 2)); // cli hnd 2
    parentWind.SetExtent(parentExtent.iTl, parentExtent.Size());
	parentWind.SetBackgroundColor(TRgb(0, 0, 0));
	parentWind.Activate();			
	
	RWindow childWind(iWs);
	CleanupClosePushL(childWind);
	User::LeaveIfError(childWind.Construct(parentWind, 3)); // cli hnd 3
    childWind.SetExtent(childExtent.iTl, childExtent.Size());
	childWind.SetBackgroundColor(TRgb(128, 128, 128));
	childWind.Activate();			
	
	parentWind.BeginRedraw();
	iWGc->Activate(parentWind);
	iWGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iWGc->SetBrushColor(TRgb(255, 0, 0));
	iWGc->DrawRect(parentFill);
	iWGc->Deactivate();
	parentWind.EndRedraw();
	
	childWind.BeginRedraw();
	iWGc->Activate(childWind);
	iWGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iWGc->SetBrushColor(TRgb(0, 255, 0));
	iWGc->DrawRect(childFill);
	iWGc->Deactivate();
	childWind.EndRedraw();
	
	TInt step=0; TInt x=0; TInt y=0;
	TInt inc, dir = 0;
	
	iProfiler->InitResults();
	for(TInt count=0; count<KIterationsToTest; ++count)
		{
		step = (count >> 2);
		inc = (step & 0xff);
		dir = (step & 0x300) >> 8;
		switch (dir)
			{
			case 0:
				x = inc;
				y = 0;
				break;
			case 1:
				x = 255;
				y = inc;
				break;
			case 2:
				x = 255-inc;
				y = 255;
				break;
			case 3:
				x = 0;
				y = 255-inc;
				break;
			}
		TPoint chPos(x-192, y-192);
		childWind.SetPosition(chPos);
		iProfiler->MarkResultSetL();
		}

	iProfiler->ResultsAnalysis(_L("ScrollMovingChild"), 0, 0, 0, KIterationsToTest);
	CleanupStack::PopAndDestroy(3, &topWind);
}
