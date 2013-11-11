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
 @internalComponent
*/

#include "t_wservratelimiterteststep.h"
#include <test/tefunit.h>
#include <e32const.h>
#include <gdi.h>
#include <stdlib.h>
#include "constants.h"

const TPoint KWinPos(0,0);
const TSize KWinSize(640,240);
const TRect KWinRect(KWinPos,KWinSize);
const TRect KEllipseRect(TPoint(10,10), TSize(50,50));

//Name of the test step
_LIT(KT_WServRateLimiterTestStep,"T_WServRateLimiterTestStep");


/**
  The test code manager, provides functions to set active object 
  with lowest priority for running tests in auto mode.
 */
class CTestManager : public CActive
	{
friend class CT_WServRateLimiterTestStep;
protected:
	static CTestManager* NewL(MTestCases* aAutoTestApp);
	~CTestManager();
	void FinishAllTestCases();
	void StartTest(); //initialize an active object and set it status as active
private: 
	CTestManager(MTestCases* aAutoTestApp);
	// from CActive
	void RunL();
	void DoCancel();
private:
	MTestCases* iTestCase;
	TInt iCurTestCaseNumber; //a current test case, every time when function RunTestCaseL is called this member inc by one
	TBool iTestCompleted;
	};

/** Construct an active object with the lowest priority */
CTestManager::CTestManager(MTestCases* aTestCases) :
			CActive(EPriorityIdle), 
			iTestCase(aTestCases)
	{
	}

CTestManager::~CTestManager()
	{
	Cancel();
	}

CTestManager* CTestManager::NewL(MTestCases* aTestCases)
	{
     CTestManager *theTestManager=new (ELeave) CTestManager(aTestCases);
     CActiveScheduler::Add(theTestManager);
	 return theTestManager;
	}

void CTestManager::RunL()
	{
	if(	iTestCompleted)
		{
		return;
		}
	
	// debug statement to indicate progress of test suite by
	// printing the sub-test that is currently active
	TTime timeStamp;
	timeStamp.HomeTime();
	TBuf<50> dateStr;
	_LIT(KDateString3,"%-B%:0%J%:1%T%:2%S%:3 %Cms");
	timeStamp.FormatL(dateStr, KDateString3);
	RDebug::Print(_L("%S - Test Case Number: %d"), &dateStr, ++iCurTestCaseNumber);
	
	iTestCase -> RunTestCaseL(iCurTestCaseNumber);
	if(!iTestCompleted)
		{
		StartTest();
		}
	}

void CTestManager::DoCancel()
	{
	iTestCompleted = ETrue;
	}

/**
 	Initialize an active object and set it as active
 */
void CTestManager::StartTest()
	{
	TRequestStatus *pS= (&iStatus);
	User::RequestComplete(pS, 0);
	SetActive();
	}


/**
 Stop active scheduler, and quit a test step	
 */
void CTestManager::FinishAllTestCases()
	{
	iTestCompleted = ETrue;
	CActiveScheduler::Stop();
	}


//
// Wait till redraws finished //
//

enum TTRateLimiterActivePriorities
	{
	ETRateLimiterRedrawActivePriority=-10,
	};

class CStopTheScheduler : public CAsyncOneShot
	{
public:
	CStopTheScheduler(TInt aPriority);
	static CStopTheScheduler* NewL(TInt aPriority);
	void RunL();
	};

CStopTheScheduler* CStopTheScheduler::NewL(TInt aPriority)
	{
	return new(ELeave)CStopTheScheduler(aPriority);
	}

CStopTheScheduler::CStopTheScheduler(TInt aPriority)
	:CAsyncOneShot(aPriority)
	{
	}

void CStopTheScheduler::RunL()
	{
	CActiveScheduler::Stop();
	}

void WaitForRedrawsToFinish()
	{
	CStopTheScheduler* ps=new CStopTheScheduler(ETRateLimiterRedrawActivePriority);
	if(ps)
		{
		ps->Call();
		CActiveScheduler::Start();
		delete ps;
		}
	}

/**
Constructor of CT_WServRateLimiterTestStep
*/
CT_WServRateLimiterTestStep::CT_WServRateLimiterTestStep():
		iTwoScreens(EFalse)
	{
	SetTestStepName(KT_WServRateLimiterTestStep);
	}

/**
Destructor of CT_WServRateLimiterTestStep
*/
CT_WServRateLimiterTestStep::~CT_WServRateLimiterTestStep()
	{
	delete iGc;
	delete iGc2;
	delete iScreen;
	delete iScreen1;
	iWinGroup.Close();
	iWinGroup1.Close();
	iWsSession.Flush();
	iWsSession.Close();
	if(iTestManager)
		{
		iTestManager -> Cancel();
		}
	delete iTestManager;
	}

/**
Starts test step
@return TVerdict pass / fail
*/
enum TVerdict CT_WServRateLimiterTestStep::doTestStepL()
	{
	iDisplayMode = EColor64K;
	User::LeaveIfError(iWsSession.Connect());

	// Screen 1 setup
	iScreen = new (ELeave) CWsScreenDevice(iWsSession);
	User::LeaveIfError(iScreen->Construct());
	iWinGroup = RWindowGroup(iWsSession);
	User::LeaveIfError(iWinGroup.Construct(KNullWsHandle, iScreen));
	iWinGroup.AutoForeground(ETrue);
	
	// Gc setup
	iGc = new (ELeave) CWindowGc(iScreen);
	User::LeaveIfError(iGc->Construct());

	// Screen 2 setup
	iScreen1 = new (ELeave) CWsScreenDevice(iWsSession);
	if (iWsSession.NumberOfScreens() > 1)
		{
		iTwoScreens = ETrue;
		}
	if (iTwoScreens)
		{
		User::LeaveIfError(iScreen1->Construct(1));
		iWinGroup1 = RWindowGroup(iWsSession);
		User::LeaveIfError(iWinGroup1.Construct(KNullWsHandle, iScreen1) );
		iWinGroup1.AutoForeground(ETrue);
		
		// Gc setup
		iGc2 = new (ELeave) CWindowGc(iScreen1);
		User::LeaveIfError(iGc2->Construct());
		}
	
	iWsSession.Finish();
		
	CActiveScheduler* theAs = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(theAs);
	CActiveScheduler::Install(theAs);
	
	iTestManager = CTestManager::NewL(this);
	iTestManager -> StartTest();
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(theAs);
	
	return TestStepResult();
	}

/**
Creates a RWindow
@param aWinGroup The window group object
@param aWin	The window object
@param aPos	The Position of the window
@param aBkgdColor The background color of the window
@param aSize The size of the window
@param aHandle The handle of the window
*/
void CT_WServRateLimiterTestStep::CreateRWindowL(const RWindowGroup& aWinGroup, 
												   RWindow& aWin, 
												   const TPoint& aPos, 
												   const TRgb& aBkgdColor, 
												   const TSize& aWinSize,
												   const TUint32 aHandle)
	{
	aWin = RWindow( iWsSession );
	CleanupClosePushL( aWin );
	User::LeaveIfError( aWin.Construct( aWinGroup, aHandle ) );
	CleanupStack::Pop(&aWin);
	aWin.SetExtent(aPos, aWinSize);
	aWin.SetBackgroundColor( aBkgdColor );
	aWin.Activate();
	aWin.SetVisible( ETrue );
	}

/**
Draw the actual animation
@param aWin The window object
@param aGraphic The graphic to draw
@param aGc The gc do draw with
*/
void CT_WServRateLimiterTestStep::DrawGraphic(RWindow& aWin, CWsGraphic* aGraphic, CWindowGc* aGc)
	{
	aGc->Activate(aWin);
	TBuf8<1> animData; 
	animData.Append(0);
	aGc->DrawWsGraphic(aGraphic->Id(),TRect(KWinPos, KWinSize),animData);
	aGc->Deactivate();
	}

/**
@param aActualFrameRate The actual frame rate of the animation
@param aExpectedFrameRate The expected frame rate of the animation
@param aErrorThreshhold The error allowed between the actual and expected frame rate
*/
TBool CT_WServRateLimiterTestStep::IsRunningAtExpectedFrameRate(TReal aActualFrameRate, TReal aExpectedFrameRate, TReal aErrorThreshHold)
	{
	TReal diff=aActualFrameRate-aExpectedFrameRate;
	return (-diff<=aErrorThreshHold && diff<=aErrorThreshHold);
	}

/**
@SYMTestCaseID			GRAPHICS-WSERV-2095-0014
@SYMPREQ				2095
@SYMTestCaseDesc		Running a CRP animation on one screen at a limited rate or both screens both at different rates			
@SYMTestActions			Run CRP animation on one or 2 screens if 2 are available 
@SYMTestStatus			Implemented
@SYMTestPriority		2
@SYMTestExpectedResults	If 1 screen available:
						CRP animation occurs at ratelimited rate
						If 2 screens available:
						CRP animation occur at different rates on both screens.
						CRP animation occur at expected rates on both screens.
@SYMTestType			CT
*/	
void CT_WServRateLimiterTestStep::TestRateLimitedAnimationL()
	{
	CWsRateLimitGraphic* rateLimitGraphic1 = CWsRateLimitGraphic::NewL();
	CleanupStack::PushL(rateLimitGraphic1);
	
	CWsRateLimitGraphic* rateLimitGraphic2 = NULL;
	if (iTwoScreens)
		{
		rateLimitGraphic2 = CWsRateLimitGraphic::NewL();
		CleanupStack::PushL(rateLimitGraphic2);
		}
	
	RWindow win1;
	CleanupClosePushL(win1);
	CreateRWindowL(iWinGroup, win1, KWinPos, KRgbBlue, KWinSize);
	win1.BeginRedraw();
	DrawGraphic(win1, rateLimitGraphic1, iGc); // running at a limited rate
	win1.EndRedraw();
	
	RWindow win2;
	if (iTwoScreens)
		{
		CleanupClosePushL(win2);
		CreateRWindowL(iWinGroup1, win2, KWinPos, KRgbBlue, KWinSize);
		win2.BeginRedraw();
		DrawGraphic(win2, rateLimitGraphic2, iGc2); // running flat out
		win2.EndRedraw();
		}
	
	iWsSession.Finish();
	
	User::After(KAnimLength + 2000000); // Animation length + 2 secs

	//Stores data obtained from server side of CRP
	TAnimData animData;
	
	//Invoke the plug-in and get the frame rate values
	Mem::FillZ(&animData, sizeof(TAnimData));
	
	// Retrieve anim data from first crp
	rateLimitGraphic1->QueryPluginForFrameRate(animData);
	iWsSession.Finish();
	WaitForRedrawsToFinish();
	TReal frameRate1 = animData.iFrameRate; 
	
	TReal frameRate2 = 0.0;
	if (iTwoScreens)
		{
		// Retrieve anim data from second crp
		rateLimitGraphic2->QueryPluginForFrameRate(animData);
		iWsSession.Finish();
		WaitForRedrawsToFinish();
		frameRate2 = animData.iFrameRate; 
		}
	
	/* Test, screen0(first screen) is running at approximately the correct rate */ 
	TReal rateLimtedExpectedFrameRate = KOneSecondInMicroSecs / KFrameRateDelay;
	INFO_PRINTF2(_L("Test if screen0 is running at approximately the correct rate of %f"), rateLimtedExpectedFrameRate);
	INFO_PRINTF2(_L("Screen0 Actual Frame Rate = %f"), frameRate1);
	TBool isCorrectRate1 = IsRunningAtExpectedFrameRate(frameRate1,rateLimtedExpectedFrameRate, KStdErrorThreshHold);
	TEST(isCorrectRate1);
	
	if (iTwoScreens)
		{
		/* Test, screen1(second screen) is running at approximately the correct rate */ 
		INFO_PRINTF2(_L("Test if screen1 is running at faster than %f fps"), KStdMinimumFrameRate);
		INFO_PRINTF2(_L("Screen1 Actual Frame Rate = %f"), frameRate2);
		TBool isCorrectRate2 = frameRate2 > KStdMinimumFrameRate;
		TEST(isCorrectRate2);
		}
	
	if (iTwoScreens)
		{
		CleanupStack::PopAndDestroy(4, rateLimitGraphic1);
		}
	else
		{
		CleanupStack::PopAndDestroy(2,rateLimitGraphic1);
		}

	}

void CT_WServRateLimiterTestStep::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	switch( iTestId )
		{
		case 0:
			if (iTwoScreens)
				{
				INFO_PRINTF1(_L("Testing both screens running animation at different frame rates"));
				}
			else
				{
				INFO_PRINTF1(_L("Testing one screen running animation at ratelimited frame rate"));
				}
			
			TestRateLimitedAnimationL();
			break;
		default:
			iTestManager->FinishAllTestCases();
			break;	
		}
	iTestId++;
	}
