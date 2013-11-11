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
// Test Direct Screen Access on a screen that supports transparency
// 
//

#include "tdirecta2.h"

const TInt KMainTestOrdinalPriority=65535;
const TInt KOntopOfAllOthersTestOrdinalPriority=65537;

CTDirect2::CTDirect2(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

CTDirect2::~CTDirect2()
	{
	delete iDsa;
	delete iUnderWindow;
	delete iOverWindow;
	delete iScreenDevice;
	delete iWindowGroup;
	delete iTimer;

	// put focus back to current screen as this test changed the focus screen to primary screen
	TheClient->iWs.SetFocusScreen(iTest->iScreenNumber);
	}

void CTDirect2::ConstructL()
	{
	// the following line makes sure that a console object hidden outside of
	// screens range doesn't affect test results being on top of tested objects
	TheClient->iGroup->GroupWin()->SetOrdinalPosition(0, KMainTestOrdinalPriority);
	}

TInt CTDirect2::Timeout(TAny* aDirect2)
	{
	static_cast<CTDirect2*>(aDirect2)->HandleTimeout();
	return KErrNone;
	}

void CTDirect2::HandleTimeout()
	{
	// Send window group to back to avoid possibility of it
	// interfering with other parts of this test
	iWindowGroup->GroupWin()->SetOrdinalPosition(0, -1);

	iTimer->Cancel(); // Don't call back again
	TEST(EFalse); // Fail the test, as we didn't get a DSA abort within timeout period
	iTestCaseComplete = ETrue; // Move to next test case
	}

void CTDirect2::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	iTimer->Cancel(); // As soon as abort is received, we don't need the timer anymore
	}

void CTDirect2::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/)
	{
	// Send window group to back to avoid possibility of it
	// interfering with other tests
	iWindowGroup->GroupWin()->SetOrdinalPosition(0, -1);

	iTestCaseComplete = ETrue; // Move to next test case

	// Don't bother restarting DSA, we were only interested in making sure the abort was sent by wserv
	}

// Starts DSA on a topmost window, then puts a translucent window ontop of it
// to make sure wserv sends a DSA Abort even though the translucent window
// hasn't changed the visible area of the DSA window
void CTDirect2::StartTranslucentWindowOverDsaL()
	{
	// Use a new window group so we can put windows ontop of all others
	iWindowGroup = new(ELeave) CTWindowGroup(TheClient);
	iWindowGroup->ConstructL();

	// Create new blank window
	iUnderWindow = new(ELeave) CTBlankWindow();
	iUnderWindow->SetUpL(TPoint(10,10), TSize(100,100), iWindowGroup, *TheClient->iGc);
	iUnderWindow->SetColor(TRgb(0,192,0));

	// Make window group visible ontop of all others
	User::LeaveIfError(iWindowGroup->GroupWin()->SetOrdinalPositionErr(0, KOntopOfAllOthersTestOrdinalPriority));
	TheClient->Flush();

	// Call Finish() to wait until under window has been rendered.
	// Once window has been rendered, we can be sure window server has
	// calculated the "top visible" region of the window.
	TheClient->iWs.Finish();

	// Start DSA on under window
	iScreenDevice = new(ELeave) CWsScreenDevice(TheClient->iWs);
	User::LeaveIfError(iScreenDevice->Construct(iTest->iScreenNumber));
	iDsa = CDirectScreenAccess::NewL(TheClient->iWs, *iScreenDevice, *iUnderWindow->BaseWin(), *this);
	iDsa->StartL();
	
	// Put translucent window ontop so as to reduce the top visible area, but leave the
	// visible area unchanged (as the translucent window doesn't change the visible area
	// of the window underneath it).
	iOverWindow = new(ELeave) CTTitledWindow();
	_LIT(KTranslucentWindowTitle, "Translucent window");
	iOverWindow->SetUpL(TPoint(60,60), TSize(150,100), iWindowGroup, *TheClient->iGc, NULL, ETrue);
	TWindowTitle windowTitle(KTranslucentWindowTitle);
	iOverWindow->SetTitle(windowTitle);
	iOverWindow->SetColor(TRgb(192, 0, 0, 128));
	TheClient->Flush();

	// Start a timer, if the timeout triggers, fail the test, as we should get
	// an abort DSA from wserv within the timeout
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	const TInt KTimeoutPeriod = 5000000; // 5 seconds should give the client plenty of time to respond to the abort
	iTimer->Start(KTimeoutPeriod, 0, TCallBack(CTDirect2::Timeout, this));

	// Do nothing more here, once over win is rendered, wserv should cause CAbortTest::AbortNow
	// to be called
	}

void CTDirect2::RunTestCaseL(TInt aCurTestCase)
	{
	((CTDirect2Step*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0176

@SYMDEF             PDEF116863

@SYMTestCaseDesc    Overlapping a translucent window ontop of a DSA window should abort DSA 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Start DSA on a blank window.
					Then place a translucent window so that it partially overlaps the DSA window.

@SYMTestExpectedResults Wserv should send a DSA abort when the translucent window is placed ontop of the DSA window.
*/
	case 1:
		((CTDirect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0176"));
		if (!iOnceOnly)
			{
			_LIT(DSA1,"Translucent window overlaping DSA window");
			INFO_PRINTF1(DSA1);
			StartTranslucentWindowOverDsaL(); // call this only once
			iOnceOnly = ETrue;
			}
		if (!iTestCaseComplete)
			{
			// Keep calling this test case until iTestCaseComplete is true
			ResetCounter(aCurTestCase-1);
			}
		else
			{
			iOnceOnly = EFalse;
			// Move to next test case in sequence
			iTestCaseComplete = EFalse;
			}	
		break;

	default:
		INFO_PRINTF1(_L("Test complete\n"));
		((CTDirect2Step*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTDirect2Step*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTDirect2Step*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(Direct2)

