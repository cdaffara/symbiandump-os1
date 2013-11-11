// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tnonredrawdrawing.h"
#include <gdi.h>
#include <hal.h>


const TInt KIterationsToTest = 100; // Number of iterations to run tests


CTNonRedrawDrawing::CTNonRedrawDrawing()
	{
	SetTestStepName(KTNonRedrawDrawing);
	}

CTNonRedrawDrawing::~CTNonRedrawDrawing()
	{
	delete iWindowGc;
	delete iScreenDevice;
	iBackgroundWindow.Close();
	iWinGroup.Close();
	iWsSession.Close();
	RFbsSession::Disconnect();
	}

/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0075
@SYMTestType UT
@SYMTestPriority Critical
@SYMDEF PDEF102791

@SYMTestCaseDesc
The test determines how long it takes to do non-redraw drawing on a transparent window
compared to non-redraw drawing on an opaque window.

@SYMTestActions
The test creates a background window that takes a relatively long time to render from its
redraw store by issuing a number of ellipse drawing commands. After that it creates a
foreground window, opaque then transparent, and does some non-redraw drawing on it for a
number of iterations.

@SYMTestExpectedResults
The test should pass and display test time both using an opaque window and a transparent one.
*/
void CTNonRedrawDrawing::NonRedrawDrawingL(TTestCase aTestCase)
	{
	iProfiler->InitResults();
	// Create foreground window
	RWindow foregroundWindow = RWindow(iWsSession);
	User::LeaveIfError(foregroundWindow.Construct(iWinGroup, 3));
	CleanupClosePushL(foregroundWindow);
	if (aTestCase == EUseTransparencyFactor)
		User::LeaveIfError(foregroundWindow.SetTransparencyFactor(TRgb(0xAA, 0xAA, 0xAA)));
	foregroundWindow.Activate();
	for (TInt i = KIterationsToTest; i > 0; --i)
		{
		// Initialize its redraw store
		foregroundWindow.BeginRedraw();
		iWindowGc->Activate(foregroundWindow);
		iWindowGc->SetBrushColor(KRgbYellow);
		iWindowGc->Clear();
		iWindowGc->Deactivate();
		foregroundWindow.EndRedraw();
		iWsSession.Flush();
		iProfiler->StartTimer();
		// Now do some non-redraw drawing
		iWindowGc->Activate(foregroundWindow);
		iWindowGc->SetBrushColor(KRgbRed);
		for (TInt x = 0; x < 200; x += 4)
			iWindowGc->Clear(TRect(x, x, x + 4, x + 4));
		iWindowGc->Deactivate();
		iWsSession.Flush();
		iProfiler->MarkResultSetL();
		// Empty redraw store to avoid OOM
		foregroundWindow.Invalidate();
		}
	CleanupStack::PopAndDestroy(&foregroundWindow);
	}

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CTNonRedrawDrawing::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	User::LeaveIfError(RFbsSession::Connect());
	User::LeaveIfError(iWsSession.Connect());
	iScreenDevice = new(ELeave) CWsScreenDevice(iWsSession);
	User::LeaveIfError(iScreenDevice->Construct(0)); // screen number (0 is first screen)
	iWindowGc = new(ELeave) CWindowGc(iScreenDevice);
	User::LeaveIfError(iWindowGc->Construct());
	iWinGroup = RWindowGroup(iWsSession);
	User::LeaveIfError(iWinGroup.Construct(1, EFalse));
	// Create background window
	iBackgroundWindow = RWindow(iWsSession);
	User::LeaveIfError(iBackgroundWindow.Construct(iWinGroup, 2));
	iBackgroundWindow.SetRequiredDisplayMode(EColor64K);
	iBackgroundWindow.Activate();
	// Initialize its redraw store with time-consuming ellipse drawing commands
	iBackgroundWindow.BeginRedraw();
	iWindowGc->Activate(iBackgroundWindow);
	TSize size = iBackgroundWindow.Size();
	for (TInt x = 0; x < 80; x += 4)
		{
		iWindowGc->SetPenColor(TRgb(0, x << 1, 0xFF));
		iWindowGc->DrawEllipse(TRect(x, x, size.iWidth - x, size.iHeight - x));
		}
	iWindowGc->Deactivate();
	iBackgroundWindow.EndRedraw();
	iWsSession.Flush();
	return TestStepResult();
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return TVerdict code
*/
TVerdict CTNonRedrawDrawing::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0075"));
	NonRedrawDrawingL(EUseOpaqueDraw);
	iProfiler->ResultsAnalysis(_L("Opaque Non-Redraw Drawing"), 0, 0, EColor64K, KIterationsToTest);
	NonRedrawDrawingL(EUseTransparencyFactor);
	iProfiler->ResultsAnalysis(_L("Transparent Non-Redraw Drawing"), 0, 0, EColor64K, KIterationsToTest);
	RecordTestResultL();
	return TestStepResult();
	}
