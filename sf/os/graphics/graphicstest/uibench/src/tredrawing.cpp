// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tredrawing.h"

const TInt KIconSeparationInPixels = 18;
const TInt KIconSizePlusSeparation = 34;

// Test bitmap files
_LIT(KAlphaTestBitmap,"z:\\system\\data\\uibench_24bit.mbm");
_LIT(K12BitBitmap, "z:\\system\\data\\uibench_12bit.mbm");
_LIT(K16x16Icon,"z:\\system\\data\\16x16icon.mbm");

const TInt KIterationsToTest = 500;

//
// CRedrawingTest
//
CRedrawingTest::CRedrawingTest()
	{
	SetTestStepName(KRedrawingTest);
	}
	
CRedrawingTest::~CRedrawingTest()
	{
	delete iWsClient;		
	}	

inline CTProfiler& CRedrawingTest::Profiler() const
    {
    return *iProfiler;
    }

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CRedrawingTest::doTestStepPreambleL()
	{	
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();	
	SetScreenModeL(EColor16MU);
	return TestStepResult();
	}
	
TVerdict CRedrawingTest::doTestStepL()
	{
/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0060
@SYMTestType UT
@SYMTestPriority Critical
@SYMDEF DEF100017
@SYMTestCaseDesc Measures performance of redraws with bitblt
@SYMTestActions
1. Create a background window with an opaque bitmap.
2. Create two test windows both transparent with bitmaps bitblt masked to the windows.
3. Repeatly swap the order of the two test windows in a loop and measure the average performance after many iterations.

@SYMTestExpectedResults
Measure the performance of swapping the windows so that front window goes behind the back window.
*/	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0060"));
	RunRedrawWindowTestCaseL(_L("Redraw-Bitblt"), ETwoWindowBitblt);
	RecordTestResultL();
	
/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0061
@SYMTestType UT
@SYMTestPriority Critical
@SYMDEF DEF100017
@SYMTestCaseDesc Measures performance of redraws with bitblt masked
@SYMTestActions
1. Create a background window with an opaque bitmap.
2. Create two test windows both transparent with bitmaps bitblt masked to the windows.
3. Repeatly swap the order of the two test windows in a loop and measure the average performance after many iterations.

@SYMTestExpectedResults
Measure the performance of swapping the windows so that front window goes behind the back window.
*/	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0061"));
	RunRedrawWindowTestCaseL(_L("Redraw-BitbltMasked"), ETwoWindowBitbltMasked);
	RecordTestResultL();

/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0062
@SYMTestType UT
@SYMTestPriority Critical
@SYMDEF DEF100017
@SYMTestCaseDesc Measures performance of redraws of window with many small bitmaps

@SYMTestActions
1. Create a background window with an opaque bitmap.
2. Create two test windows both transparent with many small bitmaps bitblt to the windows.
3. Repeatly invalidate a small region of the window in a loop and measure performance.

@SYMTestExpectedResults
Measure the performance of invalidating a small region of the window.
*/	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0062"));
	RunRedrawWindowTestCaseL(_L("Redraw-ManyBitmapsBitblt"), ETwoWindowManyBitmapsBitblt);
	RecordTestResultL();
		
	return TestStepResult();
	}

/**
This method runs the actual test case given the test name and test case Enum

@param aTestName is the test case name to be displayed in the log file - must be unique for each test case
@param aTestCase the test case to run
*/
void CRedrawingTest::RunRedrawWindowTestCaseL(const TDesC& aTestName, TRedrawTestCase aTestCase)
	{
	iWsClient = new(ELeave) CWsClient(*this, aTestCase);
	iProfiler->StartTimer();
	iWsClient->ConstructL();	
	CActiveScheduler::Start(); 	// Starts the active schedule that starts the test
	iProfiler->MarkResultSetL();			// Returns here once the test has completed.
	iProfiler->ResultsAnalysisAverageByIterations(aTestName, 0, 0, ScreenDevice()->BitmapDevice().DisplayMode(), KIterationsToTest);
	delete iWsClient;
	iWsClient=NULL;
	}

//
// CWsRedrawer
//
CWsRedrawer::CWsRedrawer() : CActive(CActive::EPriorityLow)
    {
    }

void CWsRedrawer::ConstructL(CWsClient* aClient)
    {
    iClient = aClient;
    CActiveScheduler::Add(this);
    IssueRequest();
    }

CWsRedrawer::~CWsRedrawer()
    {
    Cancel();
    }

void CWsRedrawer::IssueRequest()
    {
    iClient->WsSession().RedrawReady(&iStatus);
    SetActive();
    }

void CWsRedrawer::DoCancel()
    {
    iClient->WsSession().RedrawReadyCancel();
    }

void CWsRedrawer::RunL()
    {
    // find out what needs to be done
    TWsRedrawEvent redrawEvent;
    iClient->WsSession().GetRedraw(redrawEvent); // get event
    CWindow* window=(CWindow*)(redrawEvent.Handle()); // get window
    if (window)
        {
        TRect rect=redrawEvent.Rect(); // and rectangle that needs redrawing
        // now do drawing
        iClient->Gc().Activate(window->Window());
        window->Window().BeginRedraw(rect);
        window->Draw(rect);
        window->Window().EndRedraw();
        iClient->Gc().Deactivate();
        }
    // maintain outstanding request
    IssueRequest();
    }

//
// CWindow
//
CWindow::CWindow(CWsClient* aClient, TRedrawTestCase aTestCase)
    : iClient(aClient), iTestCase(aTestCase)
    {
    }

void CWindow::ConstructL (const TRect& aRect, CWindow* aParent)
    {
    // If a parent window was specified, use it; if not, use the window group
    // (aParent defaults to 0).
    RWindowTreeNode* parent= aParent ? (RWindowTreeNode*) &(aParent->Window()) : &(iClient->WindowGroup());
    iWindow=RWindow(iClient->WsSession()); // use app's session to window server
    User::LeaveIfError(iWindow.Construct(*parent,(TUint32)this));
    iRect = aRect;
    iWindow.SetExtent(iRect.iTl, iRect.Size()); // set extent relative to group coords    
    }

CWindow::~CWindow()
    {
    iWindow.Close();
    }

RWindow& CWindow::Window()
    {
    return iWindow;
    }

void CWindow::Activate()
	{
	iWindow.Activate();
	}

CWindowGc& CWindow::SystemGc()
    {
    return iClient->Gc();
    }

//
// CMainWindow
//
CMainWindow::CMainWindow(CWsClient* aClient, TRedrawTestCase aTestCase) : CWindow (aClient, aTestCase)
    {
    }

void CMainWindow::ConstructL(const TRect& aRect, CWindow* aParent)
	{	
	CWindow::ConstructL(aRect,aParent);
	iBitmapImage = iClient->TestSuite().LoadBitmapL(KAlphaTestBitmap,0);
	}

CMainWindow::~CMainWindow()
    {
    delete iBitmapImage;
    }

void CMainWindow::Draw(const TRect& aRect)
    {    
    CWindowGc& gc=SystemGc();
    gc.SetClippingRect(aRect);
    gc.BitBlt(TPoint(0,0), iBitmapImage);
    }

//
// CTestWindow
//
CTestWindow::CTestWindow(CWsClient* aClient, TRedrawTestCase aTestCase) : CWindow (aClient, aTestCase)
    {
    }

void CTestWindow::ConstructL(const TRect& aRect, CWindow* aParent)
	{	
	CWindow::ConstructL(aRect,aParent);
	// Load all bitmaps
	iBitmapImage = iClient->TestSuite().LoadBitmapL(KAlphaTestBitmap,0);
	iBitmap12bit = iClient->TestSuite().LoadBitmapL(K12BitBitmap, 0);
	iAlpha8bitMask = iClient->TestSuite().CopyIntoNewBitmapL(iBitmap12bit, EGray256);
	i16x16Icon = iClient->TestSuite().LoadBitmapL(K16x16Icon, 0);	
	i16x16IconMonochrome = iClient->TestSuite().LoadBitmapL(K16x16Icon, 1);	
	}

CTestWindow::~CTestWindow()
    {
    delete iBitmapImage;
    delete iBitmap12bit;
    delete iAlpha8bitMask;
    delete i16x16Icon;
    delete i16x16IconMonochrome;
    iWindow.Close();
    }

void CTestWindow::Draw(const TRect& aRect)
    {
    CWindowGc& gc=SystemGc();
    gc.SetClippingRect(aRect);
    
    // -*-* Add new test cases here for drawing to test window
    switch(iTestCase)
    	{
    	case ETwoWindowBitblt:     		    		
    		gc.BitBlt(TPoint(0,0), iBitmapImage);
    		break;
    	
    	case ETwoWindowBitbltMasked:
    		{
    		TRect cropTo(0,0,300,300);
    		gc.BitBltMasked(TPoint(0,0), iBitmapImage, cropTo, iAlpha8bitMask, EFalse);
    		break;
    		}
    		
    	case ETwoWindowManyBitmapsBitblt:
			{			
			// Draw a matrix of bitmaps in the window
			for (TInt y=10;y>=0;--y)
				{				
				for (TInt x=10;x>=0;--x)
					{
					if (!iBitmapFlag)
						gc.BitBlt(TPoint(x*KIconSeparationInPixels, y*KIconSeparationInPixels), i16x16Icon);
					else
						gc.BitBlt(TPoint(x*KIconSeparationInPixels, y*KIconSeparationInPixels), i16x16IconMonochrome);
					}
				}
			iBitmapFlag=!iBitmapFlag;
			break;
			}    		
    		
    	}
    
    }

/** 
Create an application initiated draw by drawing doign a bitblt in a small area of the window
*/
void CTestWindow::AppInitiatedDraw()
	{
	TRect rect(KIconSeparationInPixels,KIconSeparationInPixels,KIconSizePlusSeparation,KIconSizePlusSeparation);
	CWindowGc& gc=SystemGc();
	gc.Activate(iWindow);
	iWindow.BeginRedraw(rect);
	if (!iBitmapFlag)
		gc.BitBlt(TPoint(KIconSeparationInPixels,KIconSeparationInPixels), i16x16IconMonochrome);
	else
		gc.BitBlt(TPoint(KIconSeparationInPixels,KIconSeparationInPixels), i16x16Icon);
	iWindow.EndRedraw();
	gc.Deactivate();
	iBitmapFlag=!iBitmapFlag;
	}

//
// CWsClient
//
CWsClient::CWsClient(CRedrawingTest& aTestSuite, TRedrawTestCase aTestCase) : CActive(CActive::EPriorityHigh), iTestCase(aTestCase), iTestSuite(aTestSuite)
    {
    }

void CWsClient::ConstructL()
    {    
    CActiveScheduler::Add(this);
    
    // Connect to windows server
    User::LeaveIfError(iWs.Connect());
    
    // construct our one and only window group
    iGroup=RWindowGroup(iWs);
    User::LeaveIfError(iGroup.Construct(2,ETrue)); // meaningless handle; enable focus
    
    // construct screen device and graphics context
    iScreen=new (ELeave) CWsScreenDevice(iWs); // make device for this session
    User::LeaveIfError(iScreen->Construct()); // and complete its construction
    User::LeaveIfError(iScreen->CreateContext(iGc)); // create graphics context
    
    iRect = TRect(0,0,iScreen->SizeInPixels().iWidth, iScreen->SizeInPixels().iHeight);
    
    // construct redrawer
    iRedrawer=new (ELeave) CWsRedrawer;
    iRedrawer->ConstructL(this);
    
    // construct main window
    ConstructMainWindowL();
    
    // request first event and start scheduler
    IssueRequest();
    }

CWsClient::~CWsClient()
    {
    Cancel();
    delete iGc;
    delete iScreen;
    delete iRedrawer;
    delete iMainWindow;
    delete iTestWindow;
    delete iTestWindow2;
	delete iTestActive;	
	
    iGroup.Close();
    iWs.Close();
    }

void CWsClient::IssueRequest()
    {
    iWs.EventReady(&iStatus); // request an event for standard events. i.e. All events except redraws and pointer priority key events
    SetActive();
    }

void CWsClient::DoCancel()
    {
    iWs.EventReadyCancel(); // cancel event request
    }

void CWsClient::ConstructMainWindowL()
    {	                
    iTestActive = new(ELeave)CWsClient::CRedrawingTestActive(*this);    
	iTestActive->ConstructL();

	// -*-* Add new windows for test cases here
	switch (iTestCase)	
		{
		case ETwoWindowBitblt:
		case ETwoWindowBitbltMasked:
			{
			// Create the main background window with a bitmap image. This window is not transparent.
		    iMainWindow=new (ELeave) CMainWindow(this, iTestCase);
		    iMainWindow->ConstructL(iRect);
		    iMainWindow->Activate();
		        
			// Create the first test window. This window is transparent.
		    iTestWindow = new(ELeave) CTestWindow(this, iTestCase);
		    iTestWindow->ConstructL(TRect (TPoint (10, 10), TSize (200, 200)), iMainWindow);
		    TInt ret = iTestWindow->Window().SetTransparencyFactor(TRgb(0xbbbbbb,128));
		    User::LeaveIfError(ret);
		    iTestWindow->Activate();    
		    
		    // Create the second test window. This window is also transparent.
		    iTestWindow2 = new(ELeave) CTestWindow(this, iTestCase);
		    iTestWindow2->ConstructL(TRect (TPoint (30, 30), TSize (250, 200)), iMainWindow);	    
		    ret = iTestWindow2->Window().SetTransparencyFactor(TRgb(0xddaa55,200));
		    User::LeaveIfError(ret);    
		    iTestWindow2->Activate();
		    break;
			}
			
		case ETwoWindowManyBitmapsBitblt:
			{
			// Create the main background window with a bitmap image. This window is not transparent.
		    iMainWindow=new (ELeave) CMainWindow(this, iTestCase);
		    iMainWindow->ConstructL(iRect);
		    iMainWindow->Activate();

			// Create the first test window. This window is transparent.
		    iTestWindow = new(ELeave) CTestWindow(this, iTestCase);
		    iTestWindow->ConstructL(TRect (TPoint (10, 10), TSize (200, 200)), iMainWindow);
		    TInt ret = iTestWindow->Window().SetTransparencyFactor(TRgb(0xbbbbbb,128));
		    User::LeaveIfError(ret);
		    iTestWindow->Activate(); 
		    
			// Create the second test window. This window is also transparent.
		    iTestWindow2 = new(ELeave) CTestWindow(this, iTestCase);
		    iTestWindow2->ConstructL(TRect (TPoint (10, 10), TSize (250, 200)), iMainWindow);	    
		    ret = iTestWindow2->Window().SetTransparencyFactor(TRgb(0xddaa55,200));
		    User::LeaveIfError(ret);    
		    iTestWindow2->Activate();   				
			}
			break;
		}
    }

// Handle standard events from the window server.
// Standard events include all events except redraws and priority key events.
void CWsClient::RunL()
	{
	}

//
// CWsClient::CRedrawingTestActive inner class
// Set to low priority to avoid starving other active objects from being executed.
//
CWsClient::CRedrawingTestActive::CRedrawingTestActive(CWsClient& aClient) : CActive(CActive::EPriorityLow), iClient(aClient)
	{	
	CActiveScheduler::Add(this);
	}
	
CWsClient::CRedrawingTestActive::~CRedrawingTestActive()
	{
	Cancel();
	}
	
void CWsClient::CRedrawingTestActive::ConstructL()
	{
	iClient.iTestSuite.Profiler().InitResults();
	// Start active object and run tests
	RequestComplete();
	}

void CWsClient::CRedrawingTestActive::DoCancel()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrCancel);
	}
	
void CWsClient::CRedrawingTestActive::RequestComplete()
	{		
	SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
	}

/**
The test iterates KIterationsToTest times, after which the active scheduler is stopped
which passes control back to CRedrawingTest.
*/	
void CWsClient::CRedrawingTestActive::RunL()
	{
	TInt status = iStatus.Int();
	if (status==KErrNone)	
		{	
		switch(iClient.iTestCase)
			{
			// -*-* Add test cases here
			case ETwoWindowBitblt:
			case ETwoWindowBitbltMasked:
				TestTwoWindowsBitBlt();
				break;
			case ETwoWindowManyBitmapsBitblt:
				TestInvalidateSmallArea();
				break;
			}
	
		if (++iIterationCount > KIterationsToTest)	// If we reached last iteration then stop the active scheduler
			{		
			CActiveScheduler::Stop();
			}
		}		
	RequestComplete();
	}

/**
This flips the window ordinals around each time it is called
*/
void CWsClient::CRedrawingTestActive::TestTwoWindowsBitBlt()
	{
	if (iFlipWindow)
		{		
		iClient.iTestWindow->Window().SetOrdinalPosition(0);
		iClient.iTestWindow2->Window().SetOrdinalPosition(1);
		iFlipWindow=EFalse;
		} 
	else 
		{
		iClient.iTestWindow->Window().SetOrdinalPosition(1);
		iClient.iTestWindow2->Window().SetOrdinalPosition(0);		
		iFlipWindow=ETrue;
		}
	iClient.iWs.Flush();
	}

/**
This invalidates a small area of the window and causes an appication initiated draw
*/
void CWsClient::CRedrawingTestActive::TestInvalidateSmallArea()
	{
	iClient.iTestWindow->Window().Invalidate(TRect(KIconSeparationInPixels,KIconSeparationInPixels,KIconSizePlusSeparation,KIconSizePlusSeparation));
	// Application inititated draw
	iClient.iTestWindow->AppInitiatedDraw();
	
	iClient.iWs.Flush();
	}
