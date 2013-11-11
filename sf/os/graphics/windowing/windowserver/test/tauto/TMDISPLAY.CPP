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
// Multiple display test
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/



#include "TMDISPLAY.H"
#include <hal.h>
//
//
CTClient* CreateClientL()
	{
	return (TheClient=new(ELeave) TestClient());
	}
	
TInt ProcMultiDisplay(TAny* aScreenNo)
	{
	return TestLibStartUp(CreateClientL,(TInt)aScreenNo);
	}


//
//

CTMultiDisplay::CTMultiDisplay(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("Testing Mutli display functions"));
	}

void CTMultiDisplay::ConstructL()
	{
	}
	
CTMultiDisplay::~CTMultiDisplay()
	{
	}

TInt DoSetFocusScreenL(TInt aFocusScreen,TAny* /*aArg*/)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	ws.SetFocusScreen(aFocusScreen);
	ws.Close();
	return EWsExitReasonBad;
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0051

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test focusing on the two different screens
					available

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test the response to different calls to focus
					on the two screens available

@SYMTestExpectedResults Foucisng on the different screens is correct
*/	
void CTMultiDisplay::DoFocusScreenTestL()
	{
	_LIT(KSubTitle,"FocusScreen API");
	INFO_PRINTF1(KSubTitle);
	
	TInt numberOfScreens;
	HAL::Get(HALData::EDisplayNumberOfScreens, numberOfScreens);
	TInt oldFocus=TheClient->iWs.GetFocusScreen();
		
	// test case #1: out of bounds screen number
	// expected result: client panic and focus stays on current screen
	TEST(iTest->TestWsPanicL(&DoSetFocusScreenL,EWservPanicScreenNumber,-1,NULL));
	TEST(TheClient->iWs.GetFocusScreen()==oldFocus);
	if (TheClient->iWs.GetFocusScreen()!=oldFocus)
		INFO_PRINTF3(_L("TheClient->iWs.GetFocusScreen() return value - Expected: %d, Actual: %d"), oldFocus, TheClient->iWs.GetFocusScreen());		

	TEST(iTest->TestWsPanicL(&DoSetFocusScreenL,EWservPanicScreenNumber,numberOfScreens,NULL));
	TEST(TheClient->iWs.GetFocusScreen()==oldFocus);
	if (TheClient->iWs.GetFocusScreen()!=oldFocus)
		INFO_PRINTF3(_L("TheClient->iWs.GetFocusScreen() return value - Expected: %d, Actual: %d"), oldFocus, TheClient->iWs.GetFocusScreen());		

	
	// test case #2: new focus equals to current focus
	// expected result: KErrNone and focus stays on current screen
	TInt ret = TheClient->iWs.SetFocusScreen(oldFocus);
	TEST(ret==KErrNone && TheClient->iWs.GetFocusScreen()==oldFocus);
	if (ret!=KErrNone || TheClient->iWs.GetFocusScreen()!=oldFocus)
		INFO_PRINTF5(_L("TheClient->iWs.SetFocusScreen(oldFocus)==KErrNone && TheClient->iWs.GetFocusScreen()==oldFocus - Expected: %d and %d, Actual: %d and %d"),KErrNone,  oldFocus, ret, TheClient->iWs.GetFocusScreen());		

	
	// test case #3: set focus to screen N where screen N is empty (doesn't have any windows)
	// expected result: KErrNotFound and focus stays on current screen
	TInt i;
	for(i=1;i<numberOfScreens;++i)
		{
		ret = TheClient->iWs.SetFocusScreen(i);
		TEST(ret==KErrNotReady && TheClient->iWs.GetFocusScreen()==oldFocus);
		if (ret!=KErrNotReady || TheClient->iWs.GetFocusScreen()!=oldFocus)
			INFO_PRINTF5(_L("TheClient->iWs.SetFocusScreen(i)==KErrNotReady && TheClient->iWs.GetFocusScreen()==oldFocus - Expected: %d and %d, Actual: %d and %d"),KErrNotReady,  oldFocus, ret, TheClient->iWs.GetFocusScreen());
		}
				
	// test case #4: set focus to screen N where screen N has focus-able window
	// expected result: KErrNone and focus is set to screen N
	CArrayPtrFlat<CMinWin>* wins;
	wins=new(ELeave) CArrayPtrFlat<CMinWin>(numberOfScreens);
	CleanupStack::PushL(wins);
	for(i=0;i<numberOfScreens;++i)
		{
		CMinWin* win=new(ELeave) CMinWin(i);
		CleanupStack::PushL(win);
		win->ConstructL();
		wins->AppendL(win);
		}
		
	for(i=1;i<numberOfScreens;++i)
		{
		ret = TheClient->iWs.SetFocusScreen(i);
		TEST(ret==KErrNone && TheClient->iWs.GetFocusScreen()==i);
		if (ret!=KErrNone || TheClient->iWs.GetFocusScreen()!=i)
			INFO_PRINTF5(_L("TheClient->iWs.SetFocusScreen(i)==KErrNone && TheClient->iWs.GetFocusScreen()==i - Expected: %d and %d, Actual: %d and %d"),KErrNone,  i, ret, TheClient->iWs.GetFocusScreen());

		}		
	
	// test case #5: set focus back from screen N to main screen (screen 0)
	// expected result: KErrNone and focus is set to screen 0
	ret = TheClient->iWs.SetFocusScreen(0);
	TEST(ret==KErrNone && TheClient->iWs.GetFocusScreen()==0);
	if (ret!=KErrNone || TheClient->iWs.GetFocusScreen()!=0)
		INFO_PRINTF5(_L("TheClient->iWs.SetFocusScreen(0)==KErrNone && TheClient->iWs.GetFocusScreen()==0 - Expected: %d and %d, Actual: %d and %d"),KErrNone,  0, ret, TheClient->iWs.GetFocusScreen());
	
	CleanupStack::PopAndDestroy(numberOfScreens+1,wins);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0052

@SYMDEF             DEF081259

@SYMTestCaseDesc    Launch a test from a new process and check
					that it passes indepenedent of screen focus

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Focus on a screen, launch a test process,
					switch to focus on the other screen, then
					then check the test process passed

@SYMTestExpectedResults The test in the process passed
*/		
void CTMultiDisplay::DoScreenTestL(TInt aScreenNo)
	{
	_LIT(KSubTest,"Screen %d");
	TBuf<16> subMsg;

	const TInt numScreens=TheClient->iWs.NumberOfScreens();
	if (numScreens<=aScreenNo)
		{
		_LIT(KLog,"WARNING!! Cannot run test for screen %d as the device only has %d screens.");
		LOG_MESSAGE3(KLog,aScreenNo,numScreens);
		aScreenNo=numScreens-1;
		}

	subMsg.AppendFormat(KSubTest,aScreenNo);
	LOG_MESSAGE(subMsg);

	// Must switch focus screen to relevant screen manually 
	CMinWin* win=new(ELeave) CMinWin(aScreenNo);
	CleanupStack::PushL(win);
	win->ConstructL();
	TheClient->iWs.SetFocusScreen(aScreenNo);

	CleanupStack::PopAndDestroy(win);
	TheClient->iWs.SetFocusScreen(0);
	CTestBase::iScreenNo=aScreenNo;

	//Set CTestBase::iNumberOfGrpWndsOnPrimaryScreenWithZeroPriority with the number of window groups
	//in default screen with priority zero. This will be used in TGwHandle test case.
	CTestBase::iNumberOfGrpWndsOnPrimaryScreenWithZeroPriority = TheClient->iWs.NumWindowGroups(0) - 1;
	}

void CTMultiDisplay::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest1,"Focus Screen");
	_LIT(KTest2,"Screen");
	((CTMultiDisplayStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch (++iTest->iState)
		{
		case 1:
			((CTMultiDisplayStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0051"));
			iTest->LogSubTest(KTest1);
			DoFocusScreenTestL();
			break;
		case 2:
			((CTMultiDisplayStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0052"));
			iTest->LogSubTest(KTest2);
			DoScreenTestL(1);
			break;
		default:
			((CTMultiDisplayStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTMultiDisplayStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;	
		}
	((CTMultiDisplayStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(MultiDisplay)
