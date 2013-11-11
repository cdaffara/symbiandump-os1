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

#ifndef __GRAPHICSPERFORMANCE_REDRAWING_H__
#define __GRAPHICSPERFORMANCE_REDRAWING_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <w32std.h>
#include <test/ttmsgraphicsstep.h>

class CWindow;
class CWsClient;
class CRedrawingTest;

// List of test cases implemented
// -*-* To add new test case add new enum to bottom of list
enum TRedrawTestCase 
	{
	ETwoWindowBitblt,				// Test two windows with alpha bitblited images
	ETwoWindowBitbltMasked,			// Test two windows with alpha bitblt masked images
	ETwoWindowManyBitmapsBitblt,	// Test many small bitmaps in window with invaldate small area
	};

/**
This class is an active object that receives redraw events from the windows server
*/
class CWsRedrawer : public CActive
    {
public:    
    CWsRedrawer();
    void ConstructL(CWsClient* aClient);
    ~CWsRedrawer();
    void IssueRequest();
    
    // From CActive    
    void DoCancel();
    void RunL();
    
private:
    CWsClient* iClient;
    };        

/**
Window base abstract class. Derive from this class to introduce new types of windows
*/
class CWindow : public CBase
    {
public:
    CWindow(CWsClient* aClient, TRedrawTestCase aTestCase);
    void ConstructL (const TRect& aRect, CWindow* aParent=0);
    ~CWindow();
    
    RWindow& Window();
    void Activate();
    CWindowGc& SystemGc(); 
    
    virtual void Draw(const TRect& aRect) = 0;

protected:
    RWindow 		iWindow; 	// window server window
    TRect 			iRect; 		// rectangle re owning window 		
    CWsClient* 		iClient; 	// client including session and group    
	TRedrawTestCase iTestCase;
    };

/**
Main window used to draw background
*/
class CMainWindow : public CWindow
    {
public:
    CMainWindow (CWsClient* aClient, TRedrawTestCase aTestCase);
    ~CMainWindow ();
    void ConstructL(const TRect& aRect, CWindow* aParent=0);
    void Draw (const TRect& aRect);    
    
private:
	CFbsBitmap* iBitmapImage;
    };

/**
Test window for use in test cases
*/
class CTestWindow : public CWindow
    {
public:
    CTestWindow(CWsClient* aClient, TRedrawTestCase aTestCase);
    void ConstructL (const TRect& aRect, CWindow* aParent=0);
    ~CTestWindow();
    void Draw(const TRect& aRect);
    void AppInitiatedDraw();

private:
	TBool iBitmapFlag;			// Flag to indicate which image to bitblt
    CFbsBitmap* iBitmap12bit;	
    CFbsBitmap* iAlpha8bitMask;  
    CFbsBitmap* iBitmapImage;
    CFbsBitmap* i16x16Icon;
    CFbsBitmap* i16x16IconMonochrome;
    };

/**
The windows server client is an active object to run the redraw test cases
*/
class CWsClient : public CActive
    {
public:		
	CWsClient(CRedrawingTest& iTestSuite, TRedrawTestCase aTestCase);
    void ConstructL();
    ~CWsClient();
    CRedrawingTest& TestSuite() {	return iTestSuite; 	}
    RWsSession& WsSession()	{ return iWs; }
    CWindowGc& Gc() { return *iGc; }
    RWindowGroup& WindowGroup() { return iGroup; }
    
private:
    void ConstructMainWindowL();    
    void IssueRequest();
    
    // From CActive
    void DoCancel();
    void RunL();   
     
	// Nested class to handle test case in separate active object
	    class CRedrawingTestActive : public CActive
			{
		public:
			CRedrawingTestActive(CWsClient& aClient);
			~CRedrawingTestActive();
			
			void ConstructL();
		
		private:
			// From CActive
			void RunL();
			void DoCancel();
			
			void RequestComplete();
			
			// Test cases
			void TestTwoWindowsBitBlt();
			void TestInvalidateSmallArea();

		private:
			CWsClient& 	iClient;
			TBool		iFlipWindow;
			TInt 		iIterationCount;
			};	
	// End nested class CRedrawingTestActive
	    
private:
	RWsSession iWs;
	CWindowGc* iGc;
	RWindowGroup iGroup;	
	CWsScreenDevice* iScreen;
	TRect iRect;

    CWsRedrawer* iRedrawer;        
 	CRedrawingTestActive* iTestActive;
	TRedrawTestCase iTestCase;    
    CMainWindow* iMainWindow;
    CTestWindow* iTestWindow;    
    CTestWindow* iTestWindow2;    
    CRedrawingTest& iTestSuite; 
    };

/**
This the the test case derived from CTe_graphicsperformanceSuiteStepBase
It will construct the rest of the framework
*/
class CRedrawingTest : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CRedrawingTest();
	~CRedrawingTest();
	inline CTProfiler& Profiler() const; 	
private:
	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
	void RunRedrawWindowTestCaseL(const TDesC& aTestName, TRedrawTestCase aTestCase);
	
private:	
	CWsClient* iWsClient;
	};

_LIT(KRedrawingTest,"TRedrawing");

#endif //__GRAPHICSPERFORMANCE_REDRAWING_H__
