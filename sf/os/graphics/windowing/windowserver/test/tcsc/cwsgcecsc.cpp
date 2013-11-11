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
// @file
// 
//

#include <w32std.h>
#include <e32std.h>
#include "cwsgcecsc.h"
#include "extendtef.h"
#include "teflogextensions.h"
#include "testscreencapture.h"


#if defined(__X86GCC__)
extern "C" TInt atexit(void (*function)(void))
	{
	return KErrNone;
	}
#endif

void TefUnitFailLeaveL()
	{
	User::Leave(KErrTEFUnitFail);
	}

CWSGceCsc::CWSGceCsc()
	{
	}

CWSGceCsc::~CWSGceCsc()
	{
	}

/**
Common set up code for all tests.

Creates the session and window group for further windows, plus a simple white
background to obscure any unwanted stuff behind the test. Sets up the surface
update session and surface manager, to work with surfaces. Creates a screen
device for use in the tests.
*/
void CWSGceCsc::SetupL()
	{
	CWsGceCscBase::SetupL();
	}

/**
Common tear down code for all tests.

Windows, group and session created are closed. Screen device is destroyed.
Surfaces, manager and update session are closed.
*/
void CWSGceCsc::TearDownL()
	{
	CWsGceCscBase::TearDownL();
	}

CTestSuite* CWSGceCsc::CreateSuiteL( const TDesC& aName)
	{
	SUB_SUITE;
	ADD_WSGCE_TEST_STEP(CWSGceCsc, GRAPHICS_WSERV_CSC_101L);
	ADD_WSGCE_TEST_STEP(CWSGceCsc, GRAPHICS_WSERV_CSC_102L);
	ADD_WSGCE_TEST_STEP(CWSGceCsc, GRAPHICS_WSERV_CSC_103L);
	ADD_WSGCE_TEST_STEP(CWSGceCsc, GRAPHICS_WSERV_CSC_104L);
	ADD_WSGCE_TEST_STEP(CWSGceCsc, GRAPHICS_WSERV_CSC_105L);
	END_SUITE;
	}

/**
 @SYMTestCaseID			GRAPHICS_WSERV_CSC_101L
 
 @SYMTestCaseDesc		Compare the screen composition using the new and legacy API
 						
 @SYMPREQ				CSC
 
 @SYMTestStatus			Implemented
 
 @SYMTestPriority		1
 
 @SYMTestPurpose		To check that the screen composition retrieved using the new and legacy APIs

 @SYMTestActions		Initialization.
						Construct a background opaque window.
						Create two bitmaps with the size equal with the composition.
						Draw something on the screen.
						Use legacy API CopyScreenToBitmap() to copy the screen to first bitmap.
						Use the the new API to copy the composition of the screen.
						Compare the results.
 
 @SYMTestExpectedResults  The bitmap should be identical	 
*/
void CWSGceCsc::GRAPHICS_WSERV_CSC_101L()
	{
	TRect screen(iScreenDevice->SizeInPixels());
	RWindow nonSurfacedWindow(iSession);
	iTestName.Format(_L("GRAPHICS_WSERV_CSC_101L"));

	INFO_PRINTF2(_L("%S"),&iTestName);
	iDisplayMode = iScreenDevice->DisplayMode();	// Get default display mode

	ConstructOpaqueWindowLC(nonSurfacedWindow, 
			                iCyan, 
		                    0, 
                            TPoint(0, 0),
                            screen.Size());
	iSession.Flush();
	Pause(1000);

	MTestScreenCapture* csc = static_cast<MTestScreenCapture*> (iScreenDevice->GetInterface(MTestScreenCapture::KUidTestScreenCaptureIf));
    ASSERT_NOT_NULL(csc);

	TSize compositionScreenSize;
	
	User::LeaveIfError(csc->GetCompositedSize(compositionScreenSize));
	
    CFbsBitmap *bmp1 = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bmp1);
    User::LeaveIfError(bmp1->Create(compositionScreenSize, EColor16MU));
    
    CFbsBitmap *bmp2 = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bmp2);
	User::LeaveIfError(bmp2->Create(compositionScreenSize, EColor16MU));
	
	iSession.Flush();
	Pause(200);
	
	iScreenDevice->CopyScreenToBitmap(bmp1);
	User::LeaveIfError(csc->ComposeScreen(*bmp2));
	
	SetBackgroundColorWindow(nonSurfacedWindow, iRed);
	iSession.Flush();
	
	Pause(500);
	
	iGc->Activate(nonSurfacedWindow);
	iGc->BitBlt(TPoint(0, 0), bmp1);
	iGc->Deactivate();
	iSession.Flush();
	Pause(500);
	
	SetBackgroundColorWindow(nonSurfacedWindow, iWhite);
	iSession.Flush();
	Pause(500);
	
	iGc->Activate(nonSurfacedWindow);
	iGc->BitBlt(TPoint(0, 0), bmp2);
	iGc->Deactivate();
	iSession.Flush();
	Pause(500);

    ASSERT_TRUE(Compare(*bmp1, *bmp2));    
	
	if (GCEIsSupported())
		{
		iUtility->DestroyAll();
		}
	
	CleanupStack::PopAndDestroy(bmp2);
	CleanupStack::PopAndDestroy(bmp1);
	CleanupStack::PopAndDestroy(&nonSurfacedWindow);
	iSession.Flush();
	Pause(1000);
	}

void CWSGceCsc::DrawBasicPattern (RWindow& aWin)
    {
    TSize rectSize(100, 100);
    aWin.Invalidate();
    aWin.BeginRedraw();
    iGc->Activate(aWin);
    iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iGc->SetBrushColor(KRgbDarkRed);
    iGc->DrawRect(TRect(TPoint(35, 5), rectSize));
    iGc->SetBrushColor(KRgbBlack);
    iGc->DrawRect(TRect(TPoint(55, 15), rectSize));
    iGc->SetBrushColor(KRgbDarkBlue);
    iGc->DrawRect(TRect(TPoint(75, 35), rectSize));
    iGc->SetBrushColor(KRgbYellow);
    iGc->DrawRect(TRect(TPoint(95, 55), rectSize));
    iGc->Deactivate();
    aWin.EndRedraw();
    iSession.Flush();
    Pause(500);
    }

/**
 @SYMTestCaseID			GRAPHICS_WSERV_CSC_102L
 
 @SYMTestCaseDesc		Compare the screen composition using the new and legacy API
 						
 @SYMPREQ				CSC
 
 @SYMTestStatus			Implemented
 
 @SYMTestPriority		1
 
 @SYMTestPurpose		To check that the screen composition retrieved using the new and legacy APIs 

 @SYMTestActions		Initialization.
 						Draw a pattern on the screen
 						Retrieve to a bitmap the UI content using the legacy API
 						Blank the screen.
 						Display the retrieved content to a surface
 						Retrieve the composited content
 						Compare the UI content with the composited content
 						NOTE:assumes current screen mode is at 0 offset
 
 @SYMTestExpectedResults  The UI content should be identical with the composited content	 
*/
void CWSGceCsc::GRAPHICS_WSERV_CSC_102L()
	{
	TRect screen(iScreenDevice->SizeInPixels());
	RWindow testWindow(iSession);
	RWindow surfacedTestWindow(iSession);
	TSurfaceId surfaceID;

    MTestScreenCapture* csc = static_cast<MTestScreenCapture*> (iScreenDevice->GetInterface(MTestScreenCapture::KUidTestScreenCaptureIf));
    ASSERT_NOT_NULL(csc);
	
	TSize compositionScreenSize;
	User::LeaveIfError(csc->GetCompositedSize(compositionScreenSize));
	
	iTestName.Format(_L("GRAPHICS_WSERV_CSC_102"));
	INFO_PRINTF2(_L("%S"),&iTestName);
	
	iDisplayMode = iScreenDevice->DisplayMode();

	TRAPD(err, surfaceID = iUtility->CreateSurfaceL(compositionScreenSize, 
			                                        KSurfaceFormat, 
			                                        compositionScreenSize.iWidth * KBytesPerPixel));
	ASSERT_EQUALS(err,KErrNone);
	
	ConstructOpaqueWindowLC(testWindow, 
			                iWhite, 
		                    0, 
                            TPoint(0, 0),
                            compositionScreenSize);

	iSession.Flush();
	Pause(100);
	
	CFbsBitmap *bmp1 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp1);
	User::LeaveIfError(bmp1->Create(compositionScreenSize, EColor16MU));
	
	CFbsBitmap *bmp2 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp2);
	User::LeaveIfError(bmp2->Create(compositionScreenSize, EColor16MU));
	
	iSession.Flush();
	Pause(500);

	SetBackgroundColorWindow(testWindow, iWhite);
	DrawBasicPattern (testWindow);
	
	iScreenDevice->CopyScreenToBitmap(bmp1);
	iSession.Flush();
    Pause(100);
    
	ConstructOpaqueSurfacedWindowLC(surfacedTestWindow, 
			                        iRed,
		                            0,
		                            TPoint(0, 0),
		                            compositionScreenSize,
		                            surfaceID);
	
	TRAP(err,iUtility->FillSurfaceL(surfaceID, iCyan));
	ASSERT_EQUALS(err,KErrNone);
	iSession.Flush();
    Pause(500);
    
	TRAP(err, iUtility->CopyBitmapToSurfaceL(surfaceID, *bmp1));
	ASSERT_EQUALS(err,KErrNone);
	iSession.Flush();
    Pause(500);

	User::LeaveIfError(csc->ComposeScreen(*bmp2));
    
    ASSERT_TRUE(Compare(*bmp1, *bmp2));    
	
	iUtility->DestroyAll();
	CleanupStack::PopAndDestroy(&surfacedTestWindow);
	CleanupStack::PopAndDestroy(bmp2);
	CleanupStack::PopAndDestroy(bmp1);
	CleanupStack::PopAndDestroy(&testWindow);
	iSession.Flush();
	Pause(1000);
	}

/**
 @SYMTestCaseID			GRAPHICS_WSERV_CSC_103L
 
 @SYMTestCaseDesc		Test TranslateExtent
 						
 @SYMPREQ				CSC
 
 @SYMTestStatus			Implemented
 
 @SYMTestPriority		1
 
 @SYMTestPurpose		To check that the correct translated extent is returned to the client 

 @SYMTestActions		Initialization
 						Change to app mode with offset and 90 degrees rotation
 						Translate a rectangle corresponding to the original app mode
 						Compare the expected rectangle with the translated rectangle
 
 @SYMTestExpectedResults  The translated rectangle should take into account the offset and rotation
							and be equal to the expected rectangle
*/
void CWSGceCsc::GRAPHICS_WSERV_CSC_103L()
	{
	TRect screen(iScreenDevice->SizeInPixels());
	RWindow testWindow(iSession);
	
	iTestName.Format(_L("GRAPHICS_WSERV_CSC_103L"));
	INFO_PRINTF2(_L("%S"),&iTestName);
	
	iDisplayMode = iScreenDevice->DisplayMode();
	
	ConstructOpaqueWindowLC(testWindow, 
			                iWhite, 
		                    0, 
                            TPoint(0, 0),
                            screen.Size());
	iSession.Flush();
	Pause(1000);

	// Get the screen modes
	RArray<TInt> modeList;
	TInt err = iScreenDevice->GetScreenSizeModeList(&modeList);
	ASSERT_TRUE(err > 1);  //2 modes expected
	ASSERT_TRUE(modeList.Count() > 1); //2 modes expected
	
	// The initial extent and the expected translated extent
	TRect initial(TPoint(10,10), TSize(20,30));
	TRect extent(initial);
	TRect expected(TPoint(15,15), TSize(20,30));
	
	// Change app mode to 5 pixels offset
	iScreenDevice->SetAppScreenMode(modeList[1]);
	iScreenDevice->SetScreenMode(modeList[1]);
	iSession.Flush();
	Pause(100);

	TPoint origin = iScreenDevice->GetCurrentScreenModeScaledOrigin();
	if(origin != TPoint(0,0))
		{
		// Calculate the new extent
	    MTestScreenCapture* csc = static_cast<MTestScreenCapture*> (iScreenDevice->GetInterface(MTestScreenCapture::KUidTestScreenCaptureIf));
	    ASSERT_NOT_NULL(csc);
		csc->TranslateExtent(initial, extent);	
		ASSERT_TRUE(extent == expected); 
		}
	else
		{
		INFO_PRINTF1(_L("Non zero offset not supported"));
		}
    // Change app mode
    iScreenDevice->SetAppScreenMode(modeList[0]);
    iScreenDevice->SetScreenMode(modeList[0]);
    iSession.Flush();
    Pause(100);

    modeList.Close();
	CleanupStack::PopAndDestroy(&testWindow);
	iSession.Flush();
	Pause(1000);
	}

/**
 @SYMTestCaseID			GRAPHICS_WSERV_CSC_104L
 
 @SYMTestCaseDesc		Compare the screen composition using bitmaps with different display modes
 						
 @SYMPREQ				CSC
 
 @SYMTestStatus			Implemented
 
 @SYMTestPriority		1
 
 @SYMTestPurpose		To check that bitmaps with different display modes  

 @SYMTestActions		Initialization.
 						Draw a pattern on the screen
 						Retrieve to a bitmap the UI content using the legacy API
 						Blank the screen.
 						Display the retrieved content to a surface
 						Retrieve the composited content to a 2nd bitmap
 						Compare the UI content with the composited content
 
 @SYMTestExpectedResults  The bitmaps format should not match and should return KErrArgument 
*/
void CWSGceCsc::GRAPHICS_WSERV_CSC_104L()
	{
	TRect screen(iScreenDevice->SizeInPixels());
	RWindow testWindow(iSession);
	RWindow surfacedTestWindow(iSession);
	TSurfaceId surfaceID;

    MTestScreenCapture* csc = static_cast<MTestScreenCapture*> (iScreenDevice->GetInterface(MTestScreenCapture::KUidTestScreenCaptureIf));
    ASSERT_NOT_NULL(csc);
	
	TSize compositionScreenSize;
	User::LeaveIfError(csc->GetCompositedSize(compositionScreenSize));
	
	iTestName.Format(_L("GRAPHICS_WSERV_CSC_102"));
	INFO_PRINTF2(_L("%S"),&iTestName);
	
	iDisplayMode = iScreenDevice->DisplayMode();

	TRAPD(err, surfaceID = iUtility->CreateSurfaceL(compositionScreenSize, 
			                                        KSurfaceFormat, 
			                                        compositionScreenSize.iWidth * KBytesPerPixel));
	ASSERT_EQUALS(err,KErrNone);
	
	ConstructOpaqueWindowLC(testWindow, 
			                iWhite, 
		                    0, 
                            TPoint(0, 0),
                            compositionScreenSize);

	iSession.Flush();
	Pause(100);
	
	CFbsBitmap *bmp1 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp1);
	User::LeaveIfError(bmp1->Create(compositionScreenSize, EColor16MU));
	
	// Invalid bitmap display mode
	CFbsBitmap *bmp2 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp2);
	User::LeaveIfError(bmp2->Create(compositionScreenSize, EColor256));
	
	iSession.Flush();
	Pause(500);

	// Draw a pattern using WServ
	SetBackgroundColorWindow(testWindow, iWhite);

	TSize rectSize(100, 100);
	testWindow.Invalidate();
	testWindow.BeginRedraw();
	iGc->Activate(testWindow);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(KRgbDarkRed);
	iGc->DrawRect(TRect(TPoint(35, 5), rectSize));
	iGc->SetBrushColor(KRgbBlack);
	iGc->DrawRect(TRect(TPoint(55, 15), rectSize));
	iGc->SetBrushColor(KRgbDarkBlue);
	iGc->DrawRect(TRect(TPoint(75, 35), rectSize));
	iGc->SetBrushColor(KRgbYellow);
	iGc->DrawRect(TRect(TPoint(95, 55), rectSize));
	iGc->Deactivate();
	testWindow.EndRedraw();
	iSession.Flush();
	Pause(500);
	
	iScreenDevice->CopyScreenToBitmap(bmp1);
	iSession.Flush();
    Pause(100);
    
	ConstructOpaqueSurfacedWindowLC(surfacedTestWindow, 
			                        iRed,
		                            0,
		                            TPoint(0, 0),
		                            compositionScreenSize,
		                            surfaceID);
	
	TRAP(err,iUtility->FillSurfaceL(surfaceID, iCyan));
	ASSERT_EQUALS(err,KErrNone);
	iSession.Flush();
    Pause(500);
    
	TRAP(err, iUtility->CopyBitmapToSurfaceL(surfaceID, *bmp1));
	ASSERT_EQUALS(err,KErrNone);
	iSession.Flush();
    Pause(500);

    // negative test, it should fail wit KErrArgument
	TInt ret = csc->ComposeScreen(*bmp2);
	ASSERT_EQUALS(ret, KErrArgument);
	
	iUtility->DestroyAll();
	CleanupStack::PopAndDestroy(&surfacedTestWindow);
	CleanupStack::PopAndDestroy(bmp2);
	CleanupStack::PopAndDestroy(bmp1);
	CleanupStack::PopAndDestroy(&testWindow);
	iSession.Flush();
	Pause(1000);
	}

/**
 @SYMTestCaseID         GRAPHICS_WSERV_CSC_105L
 @SYMTestCaseDesc       Correct bitmap size returned when in small screen mode
 @SYMDEF                DEF139191                   
 @SYMPREQ               CSC
 @SYMTestStatus         Implemented
 @SYMTestPriority       1
 @SYMTestPurpose        Checking GetCompositedSize is correct on gce & acceptable behaviour in non gce mode
 @SYMTestActions        For every available screen mode
                        Get size from GetCompositedSize
                        Draw some coloured rectangles on screen
                        If gce, do a full screen surface, and paste the coloured rectangles into the surface
                        ComposeScreen - bitmap 1 should be the same as bitmap 2
                        Note - non GCE mode - GetCompositedSize returns unrotated appmode size, so only
                            compares the intersection of that with actual screen size
                        Note - gce mode - GetCompositedSize returns actual rotated screen size, so 
                            does a complete screen comparison
 @SYMTestExpectedResults  All asserts should complete. Comparison should succeed!
*/
void CWSGceCsc::GRAPHICS_WSERV_CSC_105L()
    {
    iTestName.Format(_L("GRAPHICS_WSERV_CSC_105"));
    INFO_PRINTF2(_L("%S"),&iTestName);
    TInt err;
    //get list of modes
    RArray<TInt> screenModes;
    err = iScreenDevice->GetScreenSizeModeList(&screenModes);
    ASSERT_TRUE (err > 0);
    //get first mode info
    TPixelsAndRotation firstModeInfo;
    iScreenDevice->GetScreenModeSizeAndRotation(screenModes[0],firstModeInfo);

    //make sure csc is available
    MTestScreenCapture* csc = static_cast<MTestScreenCapture*> (iScreenDevice->GetInterface(MTestScreenCapture::KUidTestScreenCaptureIf));
    ASSERT_NOT_NULL(csc);

    TBool differentSizes = EFalse;
    TSizeMode currentModeInfo;
    TSize compositionScreenSize;
    //for every screen mode
    for (TInt ii = 0; ii < screenModes.Count(); ++ii && !differentSizes)
        {
        //set mode
        iScreenDevice->SetAppScreenMode(screenModes[ii]);
        iScreenDevice->SetScreenMode(screenModes[ii]);
        iSession.Flush();
        Pause(100);
        //get mode info
        currentModeInfo = iScreenDevice->GetCurrentScreenModeAttributes();
        if (currentModeInfo.iScreenSize.iWidth != firstModeInfo.iPixelSize.iWidth ||
                currentModeInfo.iScreenSize.iHeight != firstModeInfo.iPixelSize.iHeight ||
                currentModeInfo.iRotation != firstModeInfo.iRotation)
            {   //note that we have modes of different sizes / rotations
            differentSizes = ETrue;
            }

        RWindow testWindow(iSession);
        RWindow surfacedTestWindow(iSession);
        TSurfaceId surfaceID;

        err = csc->GetCompositedSize(compositionScreenSize);
        ASSERT_EQUALS (err,KErrNone);
        iDisplayMode = iScreenDevice->DisplayMode();
        
        if (GCEIsSupported())
            {   //create a surface
            TRAP(err, surfaceID = iUtility->CreateSurfaceL(compositionScreenSize, 
                                                       KSurfaceFormat, 
                                                       compositionScreenSize.iWidth * KBytesPerPixel));
            ASSERT_EQUALS(err,KErrNone);
            }
        
        //create a basic window
        ConstructOpaqueWindowLC(testWindow, 
                iWhite, 
                0, 
                -currentModeInfo.iOrigin,
                compositionScreenSize);  //negative origin to ensure it fills screen, not app mode area
        iSession.Flush();
        Pause(100);
        
        //bitmap 1 to compare against
        CFbsBitmap *bmp1 = new (ELeave) CFbsBitmap;
        CleanupStack::PushL(bmp1);
        User::LeaveIfError(bmp1->Create(compositionScreenSize, EColor16MU));
        //bitmap 2 to pass into csc
        CFbsBitmap *bmp2 = new (ELeave) CFbsBitmap;
        CleanupStack::PushL(bmp2);
        User::LeaveIfError(bmp2->Create(compositionScreenSize, EColor16MU));
        
        //draw coloured rectangles in the test window
        SetBackgroundColorWindow(testWindow, iWhite);
        DrawBasicPattern (testWindow);
        
        //copy screen to bitmap 1
        err = iScreenDevice->CopyScreenToBitmap(bmp1);
        ASSERT_EQUALS (err,KErrNone);
        iSession.Flush();
        Pause(100);
        
        if (GCEIsSupported())
            {   //place a window with a surface into the screen
            ConstructOpaqueSurfacedWindowLC(surfacedTestWindow, 
                    iRed,
                    0,
                    -currentModeInfo.iOrigin,
                    compositionScreenSize,
                    surfaceID);  //negative origin to ensure it fills screen,not app mode area

            //make surface boring cyan
            TRAP(err,iUtility->FillSurfaceL(surfaceID, iCyan));
            ASSERT_EQUALS(err,KErrNone);
            iSession.Flush();
            Pause(300);
            
            //paste bitmap 1 contents into the surface
            TRAP(err, iUtility->CopyBitmapToSurfaceL(surfaceID, *bmp1));
            ASSERT_EQUALS(err,KErrNone);
            iSession.Flush();
            Pause(300);
            }
        
        //use csc to get a copy of the screen
        err = csc->ComposeScreen(*bmp2);
        ASSERT_EQUALS (err, KErrNone);
        
        //should match the original bitmap copy of the screen
        ASSERT_TRUE(Compare(*bmp1, *bmp2));    
        
        if (GCEIsSupported())
            {
            iUtility->DestroyAll();
            CleanupStack::PopAndDestroy(&surfacedTestWindow);
            }
        else
            {
            surfacedTestWindow.Close();
            }
        CleanupStack::PopAndDestroy(bmp2);
        CleanupStack::PopAndDestroy(bmp1);
        CleanupStack::PopAndDestroy(&testWindow);
        iSession.Flush();
        Pause(300);
        }
    if (!differentSizes)
        {
        INFO_PRINTF1(_L("Didn't find screen modes of different sizes - test inconclusive"));
        }
    iScreenDevice->SetAppScreenMode(screenModes[0]);
    iScreenDevice->SetScreenMode(screenModes[0]);
    screenModes.Close();
    }
