// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tredrawstoreperf.h"

const TInt KIterationsToTest = 100; // Number of iterations to run tests
TRect seg1;
TRect seg2;
TRect seg3;
TRect seg4;
TRect seg5;
TRect seg6;

CTRedrawstorePerf::CTRedrawstorePerf()
	{
	SetTestStepName(KTRedrawstorePerf);
	}

CTRedrawstorePerf::~CTRedrawstorePerf()
	{
	delete iGc;
	delete iScreenDevice;
	iWin.Close();
	iWinGroup.Close();
	iWs.Close();
	}

void CTRedrawstorePerf::CreateRWindowL()
	{
	iWin = RWindow(iWs);
	User::LeaveIfError(iWin.Construct(iWinGroup, 2));
	User::LeaveIfError(iWin.SetTransparencyAlphaChannel());	
	iWin.SetBackgroundColor(TRgb(255,255,255,128 ));
	iWin.Activate();
	Init();
	}

void CTRedrawstorePerf::Init()
	{
	// Create RedrawStore segments
	seg1=TRect( TSize( iWin.Size() ) );	
	seg2=TRect( TPoint(50,0), seg1.Center()-TPoint(20,20) );
	seg3=TRect( TPoint(seg1.Width()/2 + 20, 0), TPoint(seg1.Width()-50, seg1.Height()/2 -20 ) );
	seg4=TRect( TPoint(50, seg1.Height()/2 + 20 ), TPoint(seg1.Width()/2 - 20, seg1.Height() ) );
	seg5=TRect( seg1.Center()+TPoint(20,20), TPoint( seg1.Width()-50, seg1.Height()) );
	seg6=TRect( seg1.Center()-TPoint(75,75), TSize(150,150) );
	}

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CTRedrawstorePerf::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return TVerdict code
*/
TVerdict CTRedrawstorePerf::doTestStepL()
	{
	User::LeaveIfError(iWs.Connect());	
	iScreenDevice = new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScreenDevice->Construct(0)); // screen number (0 is first screen)
	iGc = new(ELeave) CWindowGc(iScreenDevice);
	User::LeaveIfError(iGc->Construct());
	iWinGroup = RWindowGroup(iWs);
	User::LeaveIfError(iWinGroup.Construct(1, EFalse));

	SetTestStepID(_L("GRAPHICS-UI-BENCH-0137"));
	TestStoreWithFewRectsL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0167"));
	TestStoreWithManyRectsL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0139"));
	TestPlaybackWithFewRectsL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0168"));
	TestPlaybackWithManyRectsL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0141"));
	TestStoreAndPlaybackWithFewRectsL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0142"));
	TestStoreAndPlaybackWithManyRectsL();
	RecordTestResultL();
	
	return TestStepResult();
	}

void CTRedrawstorePerf::TestStoringL(const TDesC& aTestName, TRedrawstoreTestCase aTestCase)
	{
	iProfiler->InitResults();
	for (TInt count=0; count<KIterationsToTest; count++)
		{
		CreateRWindowL();
		iWin.SetVisible(EFalse); // To make sure no playing back occurs
		iWs.Finish();		
		iGc->Activate(iWin);
		iProfiler->StartTimer();		
		if (aTestCase==EFragmentStore)
			{
			PopulateRedrawStore();
			}
		else if (aTestCase==EFragmentStoreWithManyRects)
			{
			PopulateRedrawStoreWithManyRects();
			}			
		iProfiler->MarkResultSetL();		
		iGc->Deactivate();	
		iWin.Close();
		}
	iProfiler->ResultsAnalysis(aTestName, 0, 0, iScreenDevice->DisplayMode(), KIterationsToTest);
	}

void CTRedrawstorePerf::TestPlayingbackL(const TDesC& aTestName, TRedrawstoreTestCase aTestCase)
	{
	iProfiler->InitResults();
	for (TInt count=0; count<KIterationsToTest; count++)
		{
		CreateRWindowL();
		iGc->Activate(iWin);
		if (aTestCase==EFragmentStore)
			{
			PopulateRedrawStore();
			}
		else if (aTestCase==EFragmentStoreWithManyRects)
			{
			PopulateRedrawStoreWithManyRects();
			}			
		iWin.SetVisible( EFalse );
		iWs.Finish();		
		iProfiler->StartTimer();
		iWin.SetVisible( ETrue );	// Make iWin visible to trigger playing back of redraw cmds
		iWs.Finish();
		iProfiler->MarkResultSetL();
		iGc->Deactivate();	
		iWin.Close();
		}
	iProfiler->ResultsAnalysis(aTestName, 0, 0, iScreenDevice->DisplayMode(), KIterationsToTest);
	}

void CTRedrawstorePerf::TestStoringAndPlayingbackL(const TDesC& aTestName, TRedrawstoreTestCase aTestCase)
	{
	iProfiler->InitResults();
	for (TInt count=0; count<KIterationsToTest; count++)
		{
		CreateRWindowL();
		iGc->Activate(iWin);
		iProfiler->StartTimer();		
		if (aTestCase==EFragmentStore)
			{
			PopulateRedrawStore();
			}
		else if (aTestCase==EFragmentStoreWithManyRects)
			{
			PopulateRedrawStoreWithManyRects();
			}				
		iWs.Finish();
		iProfiler->MarkResultSetL();
		iGc->Deactivate();	
		iWin.Close();
		}
	iProfiler->ResultsAnalysis(aTestName, 0, 0, iScreenDevice->DisplayMode(), KIterationsToTest);
	}


// Fragments the fullscreen window by creating segments and drawing overlaping rects across the whole window 
void CTRedrawstorePerf::PopulateRedrawStoreWithManyRects()
	{
	iWin.Invalidate();
	iWin.BeginRedraw();
	iGc->SetBrushColor(KRgbCyan);
	iGc->Clear();
	iWin.EndRedraw();

	TInt x=0;
	TBool offset=EFalse;
	for(TInt y=0; y<=seg1.Height(); y+=20)
		{
		if (offset)
			{
			x=x+20;
			offset=EFalse;
			}
		else
			{
			offset=ETrue;
			}
		
		for(; x<=seg1.Width(); x+=40)
			{
			TRect seg(TPoint(x,y),TSize(30,30));
			iWin.Invalidate(seg);
			iWin.BeginRedraw(seg);
			iGc->SetPenColor(KRgbRed);
			iGc->DrawRect(seg);
			iWin.EndRedraw();
			}
		x=0;
		}
	}

void CTRedrawstorePerf::PopulateRedrawStore()
	{
	iWin.Invalidate(seg1);
	iWin.BeginRedraw();
	iGc->SetPenColor(KRgbBlack);
	iGc->Reset();
	iGc->DrawRect(seg1);	
	iWin.EndRedraw();
	
	iWin.Invalidate(seg2);
	iWin.BeginRedraw(seg2);
	iGc->SetPenColor(KRgbBlack);
	iGc->DrawRect(seg2);
	iWin.EndRedraw();	
	
	iWin.Invalidate(seg3);
	iWin.BeginRedraw(seg3);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(KRgbBlue);
	iGc->DrawRect(seg3);
	iWin.EndRedraw();
	
	iWin.Invalidate(seg4);
	iWin.BeginRedraw(seg4);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(KRgbGreen);
	iGc->DrawRect(seg4);
	iWin.EndRedraw();
		
	iWin.Invalidate(seg5);
	iWin.BeginRedraw(seg5);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(KRgbRed);
	iGc->DrawRect(seg5);
	iWin.EndRedraw();	
		
	iWin.Invalidate(seg6);
	iWin.BeginRedraw(seg6);
	iGc->Reset();
	iGc->DrawRect(seg6);
	iWin.EndRedraw();
	}
	
/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0137

	@SYMTestCaseDesc
	The test case mesures the time it takes to store redraw commands  

	@SYMTestActions
	A fullscreen window(RWindow) is created and made invisible. The window is then segmented
 	6 times, drawn to storing the corresponding Redraw Commands and destroyed.

	@SYMTestExpectedResults
	Test should pass and display average test time per iteration
*/
void CTRedrawstorePerf::TestStoreWithFewRectsL()
	{
	_LIT(KTestName, "TestStoreWithFewRectsL");
	TestStoringL(KTestName,EFragmentStore);
	}

/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0167

	@SYMTestCaseDesc
	A pathological test case. It measures the time it takes to store Redraw commands corresponding to 
	hundereds of overlapping segments

	@SYMTestActions
	A fullscreen window(RWindow) is created and made invisible. The window is then segmented
 	a few hundered times, drawn to storing the corresponding Redraw Commands and destroyed.

	@SYMTestExpectedResults
	Test should pass and display average test time per iteration
*/
void CTRedrawstorePerf::TestStoreWithManyRectsL()
	{
	_LIT(KTestName, "TestStoreWithManyRectsL");
	TestStoringL(KTestName,EFragmentStoreWithManyRects);
	}

/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0139

	@SYMTestCaseDesc
	This test case measures the time it takes to play back the commands from the RedrawStore

	@SYMTestActions
	A fullscreen window(RWindow) is created, segmented 6 times and drawn to storing the corresponding
	Redraw Commands. It is made invisible and then visible again which triggers the playing back of 
	commands.The time is measured between when the window is made visible and the screen has been rendered.
	The time is logged and window is destroyed.
	
	@SYMTestExpectedResults
	Test should pass and display average test time per iteration
*/
void CTRedrawstorePerf::TestPlaybackWithFewRectsL()
	{
	_LIT(KTestName, "TestPlaybackWithFewRectsL");
	TestPlayingbackL(KTestName,EFragmentStore);
	}

/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0168

	@SYMTestCaseDesc
	A pathological test case. It measures the time it takes to play back the commands corresponding to 
	hundereds of overlapping segments, from the RedrawStore

	@SYMTestActions
	A fullscreen window(RWindow) is created, segmented a few hundered times and drawn to storing the
	corresponding Redraw Commands. It is made invisible and then visible again which triggers the playing
	back of commands.The time is measured between when the window is made visible and the screen has been
	rendered. The time is logged and window is destroyed.

	@SYMTestExpectedResults
	Test should pass and display average test time per iteration
*/
void CTRedrawstorePerf::TestPlaybackWithManyRectsL()
	{
	_LIT(KTestName, "TestPlaybackWithManyRectsL");
	TestPlayingbackL(KTestName,EFragmentStoreWithManyRects);
	}

/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0141

	@SYMTestCaseDesc
	This test cases measures the time it takes to store and play back redraw commands from the RedrawStore

	@SYMTestActions
	A fullscreen window(RWindow) is created, segmented 6 times and drawn to storing the corresponding
	Redraw Commands. Finish() is called then to trigger the playing back from RedrawStore. The time it takes
	to store and play back commands is logged and window is destroyed. 

	@SYMTestExpectedResults
	Test should pass and display average test time per iteration
*/
void CTRedrawstorePerf::TestStoreAndPlaybackWithFewRectsL()
	{
	_LIT(KTestName, "TestStoreAndPlaybackWithFewRectsL");
	TestStoringAndPlayingbackL(KTestName,EFragmentStore);
	}

/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0142

	@SYMTestCaseDesc
	A pathological test case. It measures the time it takes to store and playback commands corresponding to 
	hundereds of overlapping segments, from RedrawStore

	@SYMTestActions
	A fullscreen window(RWindow) is created, segmented a few hundered times and drawn to storing the
	corresponding Redraw Commands. Finish() is called then to render the screen by playing back the
	commands from the RedrawStore. The time it takes to store and play back commands is logged and window
	is destroyed. 

	@SYMTestExpectedResults
	Test should pass and display average test time per iteration
*/	
void CTRedrawstorePerf::TestStoreAndPlaybackWithManyRectsL()
	{
	_LIT(KTestName, "TestStoreAndPlaybackWithManyRectsL");
	TestStoringAndPlayingbackL(KTestName,EFragmentStoreWithManyRects);
	}
