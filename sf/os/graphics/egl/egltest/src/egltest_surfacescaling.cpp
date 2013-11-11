// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "egltest_surfacescaling.h"

#include <test/tefunit.h> // for ASSERT macros
#include <test/egltestcommonutils.h>
#include "egltestcommoninisettings.h"

//We are restricted by the screen comparison utility that requires images to be EColor16MU
const TDisplayMode KTestSourceDisplayMode = EColor16MU;

CEglTest_SurfaceScalingBase::~CEglTest_SurfaceScalingBase()
    {
	CleanAll();
	delete iImageComparison;
	delete iScreenDevice;
    CloseWindow();
    CloseWsSession();
    }

TVerdict CEglTest_SurfaceScalingBase::doTestStepPreambleL()
    {
    TVerdict verdict = CEglTestStep::doTestStepPreambleL();

    INFO_PRINTF1(_L("doTestStepPreambleL() - Initialise surface scaling test settings..."));
    if(!CheckForExtensionL(KEGL_NOK_surface_scaling))
        {
		ERR_PRINTF1(_L("KEGL_NOK_surface_scaling not supported!"));
        User::Leave(KErrNotSupported);
        }
    //retrieve the pointers to the EGL surface scaling extension functions 
    iPfnEglQuerySurfaceScalingCapabilityNOK = reinterpret_cast <TFPtrEglQuerySurfaceScalingCapabilityNok> (eglGetProcAddress("eglQuerySurfaceScalingCapabilityNOK"));
    iPfnEglSetSurfaceScalingNOK = reinterpret_cast <TFPtrEglSetSurfaceScalingNok> (eglGetProcAddress("eglSetSurfaceScalingNOK"));
	if(!iPfnEglQuerySurfaceScalingCapabilityNOK)
		{
		ERR_PRINTF1(_L("   Cannot retrieve address of the \"eglQuerySurfaceScalingCapabilityNOK\" function"));
        User::Leave(KErrNotSupported);
		}
	if(!iPfnEglSetSurfaceScalingNOK)
		{
		ERR_PRINTF1(_L("   Cannot retrieve address of the \"eglSetSurfaceScalingNOK\" function"));
        User::Leave(KErrNotSupported);
		}

    //retrieve surface scaling ini settings
	CIniData* iniData = CIniData::NewL(KConfigFileName);
    CleanupStack::PushL(iniData);
    
    _LIT(KSectionSurfaceScaling, "SurfaceScaling");
    _LIT(KKeyAllScalable, "AllScalable"); 
    
    TPtrC scalable;
	if(!iniData->FindVar(KSectionSurfaceScaling,KKeyAllScalable,scalable))
        {
		ERR_PRINTF3(_L("   Cannot retrieve section:%S key:%S"), &KSectionSurfaceScaling, &KKeyAllScalable);
		User::Leave(KErrNotSupported);
        }
	iAllScalable = (scalable.FindF( _L("true"))==KErrNotFound)? EFalse : ETrue;
	INFO_PRINTF1(_L("  ************************************************************************"));
	INFO_PRINTF1(_L("  ****   The test will be run in following configuration: "));
	INFO_PRINTF2(_L("  ****   All window surfaces scalable: %S"), &scalable);
	INFO_PRINTF1(_L("  ************************************************************************"));
	CleanupStack::PopAndDestroy(iniData);

    //Initiate a window server session and create a window group
    OpenWsSessionL(KDefaultWindowGroupId);

    //Create a Screen Device
    const TInt screen0 = 0;
    iScreenDevice = new(ELeave) CWsScreenDevice(iWsSession);
    User::LeaveIfError(iScreenDevice->Construct(screen0));
    
    // get full screen size
    TPixelsAndRotation sizeAndRotation;
    iScreenDevice->GetDefaultScreenSizeAndRotation(sizeAndRotation);
    iScreenSize = sizeAndRotation.iPixelSize;
    		
	//Create the image comparison tool from the screen device as required by most tests 		
    iImageComparison = CTGraphicsScreenComparison::NewL(*iScreenDevice);

    return verdict;
    }

TVerdict CEglTest_SurfaceScalingBase::doTestStepPostambleL()
    {
	INFO_PRINTF1(_L("doTestStepPostambleL() - Cleaning up"));
	
	// cleanup egltest framework stuff
	CleanAll();

	// cleanup member variables
	delete iImageComparison;
	iImageComparison = NULL;
	delete iScreenDevice;
	iScreenDevice = NULL;
	
	// close window and wserver session
    CloseWindow();
    CloseWsSession();

    return CEglTestStep::doTestStepPostambleL();
    }

void CEglTest_SurfaceScalingBase::CreateAndActivateWindowL(const TSize& aWindowSize)
    {
    ConstructWindowL(iWindow, aWindowSize);
    }

void CEglTest_SurfaceScalingBase::CloseWindow()
    {
	iWindow.Close();
    }

//check if border color matches with expected values
void CEglTest_SurfaceScalingBase::CheckBorderColorL(EGLint aExpectedRedChannelColor, EGLint aExpectedBlueChannelColor, EGLint aExpectedGreenChannelColor)
    {
    EGLint value = 0xffff; //set color channel to some arbitrary number 
    ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_RED_NOK, &value));
    ASSERT_EQUALS(value, aExpectedRedChannelColor);

    value = 0xffff; //set color channel to some arbitrary number
    ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_GREEN_NOK, &value));
    ASSERT_EQUALS(value, aExpectedGreenChannelColor);

    value = 0xffff; //set color channel to some arbitrary number
    ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_BLUE_NOK, &value));
    ASSERT_EQUALS(value, aExpectedBlueChannelColor);
    }

//check if scaling attributes match with expected values
void CEglTest_SurfaceScalingBase::CheckScalingAttributesL(EGLint aExpectedSurfaceWidth, EGLint aExpectedSurfaceHeight, EGLint aExpectedExtentWidth, EGLint aExpectedExtentHeight, EGLint aExpectedOffsetX, EGLint aExpectedOffsetY)
    {
    EGLint value = 0xffff; //set initial value to some arbitrary number 
    ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_WIDTH, &value));
    ASSERT_EQUALS(value, aExpectedSurfaceWidth);
    
    value = 0xffff; //set initial value to some arbitrary number 
    ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_HEIGHT, &value));
    ASSERT_EQUALS(value, aExpectedSurfaceHeight);
    
    value = 0xffff; //set initial value to some arbitrary number 
    ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_WIDTH_NOK, &value));
    ASSERT_EQUALS(value, aExpectedExtentWidth);

    value = 0xffff; //set initial value to some arbitrary number 
    ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_HEIGHT_NOK, &value));
    ASSERT_EQUALS(value, aExpectedExtentHeight);
   
    value = 0xffff; //set initial value to some arbitrary number 
    ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_X_NOK, &value));
    ASSERT_EQUALS(value, aExpectedOffsetX);
   
    value = 0xffff; //set initial value to some arbitrary number 
    ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_Y_NOK, &value));
    ASSERT_EQUALS(value, aExpectedOffsetY);
    }

/*
 Simple create bitmap function to initialise a rectangular bitmap in four simple colours plus borders
     ---------------------------
    ¦         borderTop         ¦
    ¦     -----------------     ¦
    ¦ b. ¦        ¦        ¦ b. ¦
    ¦    ¦    1   ¦   2    ¦    ¦
    ¦ L  ¦        ¦        ¦ R  ¦
    ¦ e  ¦--------¦--------¦ i  ¦
    ¦ f  ¦        ¦        ¦ g  ¦
    ¦ t  ¦    3   ¦   4    ¦ h  ¦
    ¦    ¦        ¦        ¦ t  ¦
    ¦     -----------------     ¦
    ¦       borderBottom        ¦
     --------------------------- 
*/
CFbsBitmap* CEglTest_SurfaceScalingBase::CreateBitmapLC(const TSize& aSize, TInt aBorderTop, TInt aBorderBottom, TInt aBorderLeft, TInt aBorderRight, const TRgb& aBorderColor)
    {
    // create the bitmap to the requested size (DisplayMode set to default value)
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Create(aSize,KTestSourceDisplayMode));
    TEST(bitmap->SizeInPixels().iHeight == aSize.iHeight);
    TEST(bitmap->SizeInPixels().iWidth == aSize.iWidth);

    //Create a device and context for the purpose of generating the bitmap which will be the
    //master reference used in the test
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
    CleanupStack::PushL(bitmapDevice);
    CFbsBitGc* bitmapGc = CFbsBitGc::NewL();
    CleanupStack::PushL(bitmapGc);
    bitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
    bitmapGc->Activate(bitmapDevice);

    // First off, set the whole bitmap to the border colour
    bitmapGc->SetBrushColor(aBorderColor);
    bitmapGc->SetPenColor(aBorderColor);
    bitmapGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    bitmapGc->DrawRect(aSize);
    
    // Set each individual quadrant to a different arbitrary colour
    const TInt height = aSize.iHeight;
    const TInt width  = aSize.iWidth;
    const TInt quadrantHeight = (height - aBorderTop - aBorderBottom) / 2;
    const TInt quadrantWidth = (width - aBorderLeft - aBorderRight) / 2;

    // quadrant 1 - Colour KRgbMagenta
    TRect rect = TRect(TPoint(aBorderLeft, aBorderTop), TSize(quadrantWidth, quadrantHeight));
    bitmapGc->SetBrushColor(KRgbMagenta);
    bitmapGc->SetPenColor(KRgbMagenta);
    bitmapGc->DrawRect(rect);

    // quadrant 2 - Colour KRgbCyan
    rect = TRect(TPoint(aBorderLeft+quadrantWidth, aBorderTop), TSize(quadrantWidth, quadrantHeight));
    bitmapGc->SetBrushColor(KRgbCyan);
    bitmapGc->SetPenColor(KRgbCyan);
    bitmapGc->DrawRect(rect);

    // quadrant 3 - Colour KRgbYellow
    rect = TRect(TPoint(aBorderLeft, aBorderTop+quadrantHeight), TSize(quadrantWidth, quadrantHeight));
    bitmapGc->SetBrushColor(KRgbYellow);
    bitmapGc->SetPenColor(KRgbYellow);
    bitmapGc->DrawRect(rect);

    // quadrant 4 - Colour KRgbDarkGreen
    rect = TRect(TPoint(aBorderLeft+quadrantWidth, aBorderTop+quadrantHeight), TSize(quadrantWidth, quadrantHeight));
    bitmapGc->SetBrushColor(KRgbDarkGreen);
    bitmapGc->SetPenColor(KRgbDarkGreen);
    bitmapGc->DrawRect(rect);

    //clean-up
    CleanupStack::PopAndDestroy(2, bitmapDevice);
    return bitmap;
    }

void CEglTest_SurfaceScalingBase::WritePixelsToSurfaceL(const CFbsBitmap& aBitmap)
	{
    // Mind the fact that CFbsBitmap and VGImages use different coordinates origin
    const TSize bitmapSize = aBitmap.SizeInPixels();
    TUint8* address = reinterpret_cast<TUint8*>(aBitmap.DataAddress());
    const TInt stride = aBitmap.DataStride();
    address += (bitmapSize.iHeight - 1) * stride;
    
    // copy pixel data to the drawing surface
    vgWritePixels(address, -stride, KDefaultSurfaceFormat,0,0, bitmapSize.iWidth, bitmapSize.iHeight);
    ASSERT_TRUE(vgGetError()==VG_NO_ERROR);
    
    // force all outstanding requests on the current context to complete
    vgFinish();
    iWsSession.Finish();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0651

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Choose a suitable extent to test that specified border colours are correct.

@SYMTestActions
For a number of different situations, do the following:
1. Create a window of known size
2. Create a fixed size surface, filled in with a predefined bitmap
3. Draw the surface onto the window for a set extent and border colour
4. Compare window content and border against a reference bitmap containing expected values. 

@SYMTestExpectedResults
The window content matches the independently created reference bitmap in each situation
*/
TVerdict CEglTest_SurfaceScaling_Positive::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0651"));
    SetTestStepName(_L("GRAPHICS-EGL-0651"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScaling_Positive::doTestStepL started...."));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();
    
    //  NOTE: Borders depend on what you set as extent, offset, window and surface
    //        Care should be taken when choosing size values, better to use even numbers
	//		  (CreateBitmap splits rectangle in four, so we don't want a pixel mismatch there) 
    for (TInt index=1; index<=17; index++)
    	{
		// Set some default values which are used in most cases
		iWindowWidth = iWindowHeight = 200;
		iBorderColor = TRgb(0,0,0);
	    iBorderTop = iBorderBottom = iBorderLeft = iBorderRight = 0;

	    switch(index)
        {
		case 1:
			// surface scaled to same window size with no border
		    iSurfaceWidth = 100; iSurfaceHeight = 50;
		    iExtentWidth = 200; iExtentHeight = 200;
		    iOffsetX = 0; iOffsetY = 0;
			break;
		case 2:
			// surface scaled to half the window size with border top 
		    iSurfaceWidth = 100; iSurfaceHeight = 50;
		    iExtentWidth = 200; iExtentHeight = 100;
		    iOffsetX = 0; iOffsetY = 100;
		    iBorderColor = TRgb(0,0,0);         // arbitrary border colour
		    iBorderTop = 100;
			break;
		case 3:
			// surface scaled to half the window size with border bottom
		    iSurfaceWidth = 100; iSurfaceHeight = 50;
		    iExtentWidth = 200; iExtentHeight = 100;
		    iOffsetX = 0; iOffsetY = 0;
		    iBorderColor = TRgb(128,0,255);     // arbitrary border colour
		    iBorderBottom = 100;
			break;
		case 4:
			// surface scaled to half the window size with border top and bottom
		    iSurfaceWidth = 100; iSurfaceHeight = 50;
		    iExtentWidth = 200; iExtentHeight = 100;
		    iOffsetX = 0; iOffsetY = 50;
		    iBorderColor = TRgb(128,0,0);         // arbitrary border colour
		    iBorderTop = 50; iBorderBottom = 50;
			break;
		case 5:
			// surface scaled to half the window size with border left
		    iSurfaceWidth = 100; iSurfaceHeight = 50;
		    iExtentWidth = 100; iExtentHeight = 200;
		    iOffsetX = 100; iOffsetY = 0;
		    iBorderColor = TRgb(96,96,96);         // arbitrary border colour
		    iBorderLeft = 100;
			break;
		case 6:
			// surface scaled to half the window size with border right
		    iSurfaceWidth = 100; iSurfaceHeight = 50;
		    iExtentWidth = 100; iExtentHeight = 200;
		    iOffsetX = 0; iOffsetY = 0;
		    iBorderColor = TRgb(192,192,192);       // arbitrary border colour
		    iBorderRight = 100;
			break;
		case 7:
			// surface scaled to half the window size with border left and right
		    iSurfaceWidth = 100; iSurfaceHeight = 50;
		    iExtentWidth = 100; iExtentHeight = 200;
		    iOffsetX = 50; iOffsetY = 0;
		    iBorderColor = TRgb(0,0,0);				// arbitrary border colour
		    iBorderLeft = 50; iBorderRight = 50;
			break;
		case 8:
			// surface scaled in different proportions in width and height, with borders 
		    iSurfaceWidth = 100; iSurfaceHeight = 50;
		    iExtentWidth = 190; iExtentHeight = 10;
		    iOffsetX = 5; iOffsetY = 95;
		    iBorderColor = TRgb(240,240,240);       // arbitrary border colour
		    iBorderTop = 95; iBorderBottom = 95; iBorderLeft = 5; iBorderRight = 5;
			break;
		case 9:
			// surface scaled to double the window height size, surface cropped with no borders
		    iSurfaceWidth = 100; iSurfaceHeight = 50;
		    iExtentWidth = 400; iExtentHeight = 200;
		    iOffsetX = 0; iOffsetY = 0;
			break;
		case 10:
			// surface scaled to double the window width and height size, surface cropped with no borders
		    iSurfaceWidth = 100; iSurfaceHeight = 50;
		    iExtentWidth = 400; iExtentHeight = 400;
		    iOffsetX = 0; iOffsetY = 0;
			break;
		case 11:
			// surface scaled to double the window width and height size, surface cropped with borders top and left
		    iSurfaceWidth = 100; iSurfaceHeight = 50;
		    iExtentWidth = 400; iExtentHeight = 400;
		    iOffsetX = 100; iOffsetY = 100;
		    iBorderColor = TRgb(255,128,255);      // arbitrary border colour
		    iBorderTop = 100; iBorderLeft = 100;
			break;
		case 12:
			// QnHD to full screen size
			iWindowWidth = iScreenSize.iWidth; iWindowHeight = iScreenSize.iHeight;
		    iSurfaceWidth = 320; iSurfaceHeight = 180;
		    iExtentWidth = iWindowWidth; iExtentHeight = iWindowHeight;
		    iOffsetX = 0; iOffsetY = 0;
			break;
		case 13:
			// QVGA to full screen size
			iWindowWidth = iScreenSize.iWidth; iWindowHeight = iScreenSize.iHeight;
		    iSurfaceWidth = 320; iSurfaceHeight = 240;
		    iExtentWidth = iWindowWidth; iExtentHeight = iWindowHeight;
		    iOffsetX = 0; iOffsetY = 0;
			break;
		case 14:
			// HVGA to full screen size
			iWindowWidth = iScreenSize.iWidth; iWindowHeight = iScreenSize.iHeight;
		    iSurfaceWidth = 480; iSurfaceHeight = 320;
		    iExtentWidth = iWindowWidth; iExtentHeight = iWindowHeight;
		    iOffsetX = 0; iOffsetY = 0;
			break;
		case 15:
			// 480x270 to full screen size
			iWindowWidth = iScreenSize.iWidth; iWindowHeight = iScreenSize.iHeight;
		    iSurfaceWidth = 480; iSurfaceHeight = 270;
		    iExtentWidth = iWindowWidth; iExtentHeight = iWindowHeight;
		    iOffsetX = 0; iOffsetY = 0;
			break;
		case 16:
			// VGA to full screen size
			iWindowWidth = iScreenSize.iWidth; iWindowHeight = iScreenSize.iHeight;
		    iSurfaceWidth = 640; iSurfaceHeight = 480;
		    iExtentWidth = iWindowWidth; iExtentHeight = iWindowHeight;
		    iOffsetX = 0; iOffsetY = 0;
			break;
		case 17:
			// WVGA to full screen size
			iWindowWidth = iScreenSize.iWidth; iWindowHeight = iScreenSize.iHeight;
		    iSurfaceWidth = 768; iSurfaceHeight = 480;
		    iExtentWidth = iWindowWidth; iExtentHeight = iWindowHeight;
		    iOffsetX = 0; iOffsetY = 0;
			break;
		default:
		    ERR_PRINTF2(_L("Inconsistency in test code, case does not exist: %d."), index);
		    User::Leave(KErrNotSupported);
		    break;
        }

		// perform the testing
	    doTestPartialStepL();
    	}

    // clean-up
    CleanAll();

    INFO_PRINTF1(_L("....CEglTest_SurfaceScaling_Positive::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

TVerdict CEglTest_SurfaceScaling_Positive::doTestPartialStepL()
    {
    INFO_PRINTF5(_L("doTestPartialStepL started with (%d,%d) fixed surface scaled to (%d,%d) extent...."), iSurfaceWidth, iSurfaceHeight, iExtentWidth, iExtentHeight);

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(iWindowWidth, iWindowHeight));

    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;

    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL(numConfigs == 1); // Abort the test if the EGL config is not supported

    EGLint attrib_list[] = {
              EGL_FIXED_WIDTH_NOK, iSurfaceWidth,
              EGL_FIXED_HEIGHT_NOK, iSurfaceHeight,
              EGL_TARGET_EXTENT_OFFSET_X_NOK, iOffsetX,
              EGL_TARGET_EXTENT_OFFSET_Y_NOK, iOffsetY,
              EGL_TARGET_EXTENT_WIDTH_NOK, iExtentWidth,
              EGL_TARGET_EXTENT_HEIGHT_NOK, iExtentHeight,
              EGL_BORDER_COLOR_RED_NOK, iBorderColor.Red(),
              EGL_BORDER_COLOR_GREEN_NOK, iBorderColor.Green(),
              EGL_BORDER_COLOR_BLUE_NOK, iBorderColor.Blue(),
              EGL_NONE};
    
    // Create the window surface and the egl context and make them current
    iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    

    // Create the test bitmap that will be used in the scaled surface 
    //  NOTE: This bitmap generally does not have borders, 
    CFbsBitmap* testBitmap = CreateBitmapLC(TSize(iSurfaceWidth, iSurfaceHeight),0,0,0,0,TRgb(0,0,0));
    
    // Copy test bitmap to drawing surface
    WritePixelsToSurfaceL(*testBitmap);

    //call eglSwapBuffers
    ASSERT_EGL_TRUE(eglSwapBuffers(iDisplay, iEglSess->Surface()));
    //Wait for the draw operation to complete
    eglWaitClient();

    // Create the reference bitmap that should be expected after the scaling 
    //  NOTE: This bitmap may have borders
    //        Size could exceed that of the window, so check both extent and window
    const TInt refWidth = iExtentWidth>iWindowWidth ? iExtentWidth : iWindowWidth;
    const TInt refHeight = iExtentHeight>iWindowHeight ? iExtentHeight : iWindowHeight;
    CFbsBitmap* refBitmap = CreateBitmapLC(TSize(refWidth, refHeight), iBorderTop, iBorderBottom, iBorderLeft, iBorderRight, iBorderColor);

    // compare window contents with expected bitmap
    TInt comparison = iImageComparison->CompareScreenImageL(TSize(iWindowWidth, iWindowHeight), TPoint(0,0), TPoint(0,0), *refBitmap, TestStepName());
    TEST(comparison==KErrNone);
    INFO_PRINTF3(_L("CompareScreenImageL returns %d (expected is %d)"), comparison, KErrNone);

    /// Cleanup
    CleanupStack::PopAndDestroy(2, testBitmap); // refBitmap, testBitmap
    iEglSess->CleanupSurfaceAndContextL();
    CloseWindow();
    
    INFO_PRINTF1(_L("....doTestPartialStepL completed!"));
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0661

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Resize native window to verify target extent remains fixed.

@SYMTestActions
1. Create a window of known size
2. Create a fixed size surface, filled in with a predefined bitmap
3. Draw the surface onto the window for a set extent and border colour
4. Repeat step 3 but increasing native window size by (1,1) pixels.
5. Compare window content and border against a reference bitmap containing expected values. 

@SYMTestExpectedResults
The window content matches the independently created reference bitmap in each situation
*/
TVerdict CEglTest_SurfaceScaling_WindowResize::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0661"));
    SetTestStepName(_L("GRAPHICS-EGL-0661"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScaling_WindowResize::doTestStepL started...."));

    // set the initial value of a square window
    const TInt KWindowSize = 100;
    
    // surface with size and extent that matches the window size and no offset
    iWindowWidth = iWindowHeight = KWindowSize;
    iSurfaceWidth = iSurfaceHeight = KWindowSize;
    iExtentWidth = iExtentHeight = KWindowSize;
    iOffsetX = 0; iOffsetY = 0;
    iBorderColor = TRgb(0x99,0xcc,0xff);	// set some arbitrary colour
    iBorderTop = 0; iBorderBottom = 0; iBorderLeft = 0; iBorderRight = 0;

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(iWindowWidth, iWindowHeight));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;

    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL(numConfigs == 1); // Abort the test if the EGL config is not supported

    EGLint attrib_list[] = {
              EGL_FIXED_WIDTH_NOK, iSurfaceWidth,
              EGL_FIXED_HEIGHT_NOK, iSurfaceHeight,
              EGL_TARGET_EXTENT_OFFSET_X_NOK, iOffsetX,
              EGL_TARGET_EXTENT_OFFSET_Y_NOK, iOffsetY,
              EGL_TARGET_EXTENT_WIDTH_NOK, iExtentWidth,
              EGL_TARGET_EXTENT_HEIGHT_NOK, iExtentHeight,
              EGL_BORDER_COLOR_RED_NOK, iBorderColor.Red(),
              EGL_BORDER_COLOR_GREEN_NOK, iBorderColor.Green(),
              EGL_BORDER_COLOR_BLUE_NOK, iBorderColor.Blue(),
              EGL_NONE};
    
    // Create the window surface and the egl context and make them current
    iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    
    
    // Create the test bitmap that will be used in the scaled surface 
    //  NOTE: This bitmap generally does not have borders, 
    CFbsBitmap* testBitmap = CreateBitmapLC(TSize(iSurfaceWidth, iSurfaceHeight),0,0,0,0,TRgb(0,0,0));
    
    // Copy test bitmap to drawing surface
    WritePixelsToSurfaceL(*testBitmap);

    // Start doing iterations by increasing native window size.
    INFO_PRINTF1(_L("Start increasing native window size by (1,1)..."));
    iWindowWidth=1;
    iWindowHeight=1;
    while(iWindowWidth<=(KWindowSize*2) && iWindowHeight<=(KWindowSize*2))
    	{
		// Note that borders will appear while window size is bigger than extent
		//  - iSurfaceWidth/iSurfaceHeight - unmodified
		//  - iExtentWidth/iExtentHeight - unmodified
		//  - iOffsetX/iOffsetY - unmodified 
		// Set up expected values for the border
		iBorderTop = 0; iBorderLeft = 0; 
		iBorderBottom = iExtentHeight<iWindowHeight ? iWindowHeight-iExtentHeight : 0; 
		iBorderRight = iExtentWidth<iWindowWidth ? iWindowWidth-iExtentWidth : 0; 

		// resize window to new given size
	    iWindow.Invalidate();
		iWindow.BeginRedraw();
		iWindow.SetSize(TSize(iWindowWidth, iWindowHeight));
		iWindow.EndRedraw();

		// re-populate buffers with the unmodified fixed size surface content
	    WritePixelsToSurfaceL(*testBitmap);
		
		// perform the testing
		doTestPartialStepL();
		++iWindowWidth;
		++iWindowHeight;
    	}

    /// Cleanup
    CleanupStack::PopAndDestroy(testBitmap);
    iEglSess->CleanupSurfaceAndContextL();
    CleanAll();
    CloseWindow();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScaling_WindowResize::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

TVerdict CEglTest_SurfaceScaling_WindowResize::doTestPartialStepL()
    {
    INFO_PRINTF3(_L("doTestPartialStepL started for native window size of (%d, %d)...."), iWindowWidth, iWindowHeight);

	// resize may be effective only after swapbuffers 
	ASSERT_EGL_TRUE(eglSwapBuffers(iDisplay, iEglSess->Surface()));
	ASSERT_EGL_TRUE(eglWaitClient());
	
	//check all expected values
    CheckScalingAttributesL(iSurfaceWidth, iSurfaceHeight, iExtentWidth, iExtentHeight, iOffsetX, iOffsetY); 
    CheckBorderColorL(iBorderColor.Red(), iBorderColor.Blue(), iBorderColor.Green()); 

    // Create the reference bitmap that should be expected after the window resize 
    //  NOTE: This bitmap may have borders
    //        Size could exceed that of the window, so check both extent and window
    const TInt refWidth = iExtentWidth>iWindowWidth ? iExtentWidth : iWindowWidth;
    const TInt refHeight = iExtentHeight>iWindowHeight ? iExtentHeight : iWindowHeight;
    CFbsBitmap* refBitmap = CreateBitmapLC(TSize(refWidth, refHeight), iBorderTop, iBorderBottom, iBorderLeft, iBorderRight, iBorderColor);
    
    // compare window contents with expected bitmap
    TInt comparison = iImageComparison->CompareScreenImageL(TSize(iWindowWidth, iWindowHeight), TPoint(0,0), TPoint(0,0), *refBitmap, TestStepName());
    TEST(comparison==KErrNone);
    INFO_PRINTF3(_L("CompareScreenImageL returns %d (expected is %d)"), comparison, KErrNone);

    /// Cleanup
    CleanupStack::PopAndDestroy(refBitmap);

    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0668

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Create a fixed size surface and vary the extent position for a range of values.

@SYMTestActions
1. Create a window of known size
2. Create a fixed size surface, filled in with a predefined bitmap
3. Draw the surface onto the window for a set extent and border colour
4. Repeat step 3 but increasing target extent offset by (1,1) pixels.
5. Compare window content and border against a reference bitmap containing expected values. 

@SYMTestExpectedResults
The window content matches the independently created reference bitmap in each situation
*/
TVerdict CEglTest_SurfaceScaling_ExtentPositionChange::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0668"));
    SetTestStepName(_L("GRAPHICS-EGL-0668"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScaling_ExtentPositionChange::doTestStepL started...."));

    // surface with size and extent that matches the window size and no offset
    iWindowWidth = iWindowHeight = 100;
    iSurfaceWidth = iExtentWidth = iWindowWidth;
    iSurfaceHeight = iExtentHeight = iWindowHeight;
    iOffsetX = 0; iOffsetY = 0;
    iBorderColor = TRgb(0x11,0x22,0x33);	// set some arbitrary colour
    iBorderTop = 0; iBorderBottom = 0; iBorderLeft = 0; iBorderRight = 0;

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(iWindowWidth, iWindowHeight));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;

    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL(numConfigs == 1); // Abort the test if the EGL config is not supported

    EGLint attrib_list[] = {
              EGL_FIXED_WIDTH_NOK, iSurfaceWidth,
              EGL_FIXED_HEIGHT_NOK, iSurfaceHeight,
              EGL_TARGET_EXTENT_OFFSET_X_NOK, iOffsetX,
              EGL_TARGET_EXTENT_OFFSET_Y_NOK, iOffsetY,
              EGL_TARGET_EXTENT_WIDTH_NOK, iExtentWidth,
              EGL_TARGET_EXTENT_HEIGHT_NOK, iExtentHeight,
              EGL_BORDER_COLOR_RED_NOK, iBorderColor.Red(),
              EGL_BORDER_COLOR_GREEN_NOK, iBorderColor.Green(),
              EGL_BORDER_COLOR_BLUE_NOK, iBorderColor.Blue(),
              EGL_NONE};
    
    // Create the window surface and the egl context and make them current
    iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    
    
    // Create the test bitmap that will be used in the scaled surface 
    //  NOTE: This bitmap generally does not have borders, 
    CFbsBitmap* testBitmap = CreateBitmapLC(TSize(iSurfaceWidth, iSurfaceHeight),0,0,0,0,TRgb(0,0,0));
    
    // Copy test bitmap to drawing surface
    WritePixelsToSurfaceL(*testBitmap);

	// This test is a bit different. Since the extent remains the same, and we only change
    //  the offset, we create an oversized reference bitmap and compare the screen with a
    //  region contained within the reference bitmap 
    // Size is three times the extent so that it has extent size borders all around
    const TInt refWidth = 3*iExtentWidth;
    const TInt refHeight = 3*iExtentHeight;
    iBorderTop=iBorderBottom=iBorderLeft=iBorderRight=iExtentWidth;	
    CFbsBitmap* refBitmap = CreateBitmapLC(TSize(refWidth, refHeight), iBorderTop, iBorderBottom, iBorderLeft, iBorderRight, iBorderColor);

    // Start doing iterations by increasing target extent offset
    INFO_PRINTF1(_L("Start increasing target extent offset size by (1,1)..."));
    for (iOffsetX=-iExtentWidth,iOffsetY=-iExtentHeight; iOffsetX<=iExtentWidth&&iOffsetY<=iExtentHeight; ++iOffsetX,++iOffsetY)
    	{
		// Note that borders will appear other than when offset is (0,0)
		//  - iSurfaceWidth/iSurfaceHeight - unmodified
		//  - iExtentWidth/iExtentHeight - unmodified
	
	    // set new offset values
	    ASSERT_EGL_TRUE(iPfnEglSetSurfaceScalingNOK(iDisplay, iEglSess->Surface(), iOffsetX, iOffsetY, iExtentWidth, iExtentHeight));

		// re-populate buffers with the unmodified fixed size surface content
	    WritePixelsToSurfaceL(*testBitmap);

		// perform the testing
		doTestPartialStepL(*refBitmap);
    	}
    
    /// Cleanup
    CleanupStack::PopAndDestroy(2, testBitmap); //testBitmap, refBitmap
    iEglSess->CleanupSurfaceAndContextL();
    CleanAll();
    CloseWindow();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScaling_ExtentPositionChange::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

TVerdict CEglTest_SurfaceScaling_ExtentPositionChange::doTestPartialStepL(const CFbsBitmap& aRefBitmap)
    {
    INFO_PRINTF3(_L("doTestPartialStepL started for offset of (%d, %d)...."), iOffsetX, iOffsetY);

    // offset change may be effective only after swapbuffers 
	ASSERT_EGL_TRUE(eglSwapBuffers(iDisplay, iEglSess->Surface()));
	ASSERT_EGL_TRUE(eglWaitClient());
	// Finish() to ensure all native window operations related to the surface complete before image comparision.
	iWsSession.Finish();
	
	//check all expected values
    CheckScalingAttributesL(iSurfaceWidth, iSurfaceHeight, iExtentWidth, iExtentHeight, iOffsetX, iOffsetY); 
    CheckBorderColorL(iBorderColor.Red(), iBorderColor.Blue(), iBorderColor.Green()); 

    // Prepare the offset of the reference bitmap to know what sub-region should be compared
    const TInt refWidth = 3*iExtentWidth;
    const TInt refHeight = 3*iExtentHeight;
    TPoint bitmapOffset(refWidth-iExtentWidth-iRefBitmapOffset, refHeight-iExtentHeight-iRefBitmapOffset);
    iRefBitmapOffset++;		// increase offset for next iteration
    
    // Comparision takes into account specific region within the reference bitmap
    TInt comparison = iImageComparison->CompareScreenImageL(TSize(iWindowWidth, iWindowHeight), TPoint(0,0), bitmapOffset, aRefBitmap, TestStepName());
    TEST(comparison==KErrNone);
    INFO_PRINTF3(_L("CompareScreenImageL returns %d (expected is %d)"), comparison, KErrNone);

    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0669

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Create a fixed size surface and vary the extent size for a range of values

@SYMTestActions
1. Create a window of known size
2. Create a fixed size surface, filled in with a predefined bitmap
3. Draw the surface onto the window for a set extent and border colour
4. Repeat step 3 but increasing target extent size by (2,2) pixels.
5. Compare window content and border against a reference bitmap containing expected values. 

@SYMTestExpectedResults
The window content matches the independently created reference bitmap in each situation
*/
TVerdict CEglTest_SurfaceScaling_ExtentSizeChange::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0669"));
    SetTestStepName(_L("GRAPHICS-EGL-0669"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScaling_ExtentSizeChange::doTestStepL started...."));

    // set the initial value of a square window
    const TInt KWindowSize = 100;
    
    // surface with size and extent that matches the window size and no offset
    iWindowWidth = iWindowHeight = KWindowSize;
    iSurfaceWidth = iSurfaceHeight = KWindowSize;
    iExtentWidth = iExtentHeight = KWindowSize; 	
    iOffsetX = 0; iOffsetY = 0;
    iBorderColor = TRgb(0x80,0x40,0xF0);	// set some arbitrary colour
    iBorderTop = 0; iBorderBottom = 0; iBorderLeft = 0; iBorderRight = 0;

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(iWindowWidth, iWindowHeight));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;

    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL(numConfigs == 1); // Abort the test if the EGL config is not supported

    EGLint attrib_list[] = {
              EGL_FIXED_WIDTH_NOK, iSurfaceWidth,
              EGL_FIXED_HEIGHT_NOK, iSurfaceHeight,
              EGL_TARGET_EXTENT_OFFSET_X_NOK, iOffsetX,
              EGL_TARGET_EXTENT_OFFSET_Y_NOK, iOffsetY,
              EGL_TARGET_EXTENT_WIDTH_NOK, iExtentWidth,
              EGL_TARGET_EXTENT_HEIGHT_NOK, iExtentHeight,
              EGL_BORDER_COLOR_RED_NOK, iBorderColor.Red(),
              EGL_BORDER_COLOR_GREEN_NOK, iBorderColor.Green(),
              EGL_BORDER_COLOR_BLUE_NOK, iBorderColor.Blue(),
              EGL_NONE};
    
    // Create the window surface and the egl context and make them current
    iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    
    
    // Create the test bitmap that will be used in the scaled surface 
    //  NOTE: This bitmap generally does not have borders, 
    CFbsBitmap* testBitmap = CreateBitmapLC(TSize(iSurfaceWidth, iSurfaceHeight),0,0,0,0,TRgb(0,0,0));
    
    // Copy test bitmap to drawing surface
    WritePixelsToSurfaceL(*testBitmap);

    // Start doing iterations by increasing target extent size 
    //  We do increments of (2,2) because the reference bitmap is simetrically split into 4 areas, so size
    //  of this reference bitmap should be an even number of pixels
    INFO_PRINTF1(_L("Start increasing target extent size by (2,2)..."));
    iExtentWidth=2;
    iExtentHeight=2;
	while(iExtentWidth <= (KWindowSize*2) && iExtentHeight <= (KWindowSize*2))
    	{
		// Note that borders will appear while extent is smaller than window
		//  - iSurfaceWidth/iSurfaceHeight - unmodified
		//  - iOffsetX/iOffsetY - unmodified 
		// Set up expected values for the border
		iBorderTop = 0; iBorderLeft = 0; 
		iBorderBottom = iExtentHeight<iWindowHeight ? iWindowHeight-iExtentHeight : 0; 
		iBorderRight = iExtentWidth<iWindowWidth ? iWindowWidth-iExtentWidth : 0; 

	    // set new target extent values
	    ASSERT_EGL_TRUE(iPfnEglSetSurfaceScalingNOK(iDisplay, iEglSess->Surface(), iOffsetX, iOffsetY, iExtentWidth, iExtentHeight));

		// re-populate buffers with the unmodified fixed size surface content
	    WritePixelsToSurfaceL(*testBitmap);
		
		// perform the testing
		doTestPartialStepL();
		iExtentWidth+=2;
		iExtentHeight+=2;
		}

    /// Cleanup
    CleanupStack::PopAndDestroy(testBitmap);
    iEglSess->CleanupSurfaceAndContextL();
    CleanAll();
    CloseWindow();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScaling_ExtentSizeChange::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

TVerdict CEglTest_SurfaceScaling_ExtentSizeChange::doTestPartialStepL()
    {
    INFO_PRINTF3(_L("doTestPartialStepL started for extent size of (%d, %d)...."), iExtentWidth, iExtentHeight);

    // extent change may be effective only after swapbuffers 
	ASSERT_EGL_TRUE(eglSwapBuffers(iDisplay, iEglSess->Surface()));
	ASSERT_EGL_TRUE(eglWaitClient());
	// Finish() to ensure all native window operations related to the surface complete before image comparision.
	iWsSession.Finish();
	
	//check all expected values
    CheckScalingAttributesL(iSurfaceWidth, iSurfaceHeight, iExtentWidth, iExtentHeight, iOffsetX, iOffsetY); 
    CheckBorderColorL(iBorderColor.Red(), iBorderColor.Blue(), iBorderColor.Green()); 

    // Create the reference bitmap that should be expected after the window resize 
    //  NOTE: This bitmap may have borders
    //        Size could exceed that of the window, so check both extent and window
    const TInt refWidth = iExtentWidth>iWindowWidth ? iExtentWidth : iWindowWidth;
    const TInt refHeight = iExtentHeight>iWindowHeight ? iExtentHeight : iWindowHeight;
    CFbsBitmap* refBitmap = CreateBitmapLC(TSize(refWidth, refHeight), iBorderTop, iBorderBottom, iBorderLeft, iBorderRight, iBorderColor);
    
    // compare window contents with expected bitmap
    TInt comparison = iImageComparison->CompareScreenImageL(TSize(iWindowWidth, iWindowHeight), TPoint(0,0), TPoint(0,0), *refBitmap, TestStepName());
    TEST(comparison==KErrNone);
    INFO_PRINTF3(_L("CompareScreenImageL returns %d (expected is %d)"), comparison, KErrNone);

    /// Cleanup
    CleanupStack::PopAndDestroy(refBitmap);

    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0670

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Check that updating surface scaling attributes is visible only after calling eglSwapBuffers

@SYMTestActions
1. Create a window of known size
2. Create a fixed size surface, filled in with a predefined bitmap
3. Draw the surface onto the window for a set extent and border colour
4. Repeat step 3 but increasing target extent size and changing border colours
5. Compare window content and border against a reference bitmap containing expected values
  5a. Before calling eglSwapBuffers
  5b. After callnig eglSwapBuffers  

@SYMTestExpectedResults
5a. The window content matches the independently created reference with old values
5b. The window content matches the independently created reference with new values
*/
TVerdict CEglTest_SurfaceScaling_SwapBuffers::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0670"));
    SetTestStepName(_L("GRAPHICS-EGL-0670"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScaling_SwapBuffers::doTestStepL started...."));

    // set the initial value of a square window
    const TInt KWindowSize = 200;
    
    // surface with size that matches the window size and no offset and extent a quarter 
    // of that originally positioned in the middle (so borders should be seen)
    iWindowWidth = iWindowHeight = KWindowSize;
    iSurfaceWidth = iSurfaceHeight = KWindowSize;
    iExtentWidth = iExtentHeight = KWindowSize/4; 	
    iOffsetX = (iWindowWidth-iExtentWidth)/2; iOffsetY = (iWindowHeight-iExtentHeight)/2;	// to center the surface at the middle of the window
    iBorderColor = TRgb(0xFF,0xFF,0x00);	// set some arbitrary colour with blue channel to 0x00
	iBorderTop = iBorderBottom = iExtentHeight<iWindowHeight ? (iWindowHeight-iExtentHeight)/2 : 0; 
	iBorderLeft = iBorderRight = iExtentWidth<iWindowWidth ? (iWindowWidth-iExtentWidth)/2 : 0; 

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(iWindowWidth, iWindowHeight));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;

    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL(numConfigs == 1); // Abort the test if the EGL config is not supported

    EGLint attrib_list[] = {
              EGL_FIXED_WIDTH_NOK, iSurfaceWidth,
              EGL_FIXED_HEIGHT_NOK, iSurfaceHeight,
              EGL_TARGET_EXTENT_OFFSET_X_NOK, iOffsetX,
              EGL_TARGET_EXTENT_OFFSET_Y_NOK, iOffsetY,
              EGL_TARGET_EXTENT_WIDTH_NOK, iExtentWidth,
              EGL_TARGET_EXTENT_HEIGHT_NOK, iExtentHeight,
              EGL_BORDER_COLOR_RED_NOK, iBorderColor.Red(),
              EGL_BORDER_COLOR_GREEN_NOK, iBorderColor.Green(),
              EGL_BORDER_COLOR_BLUE_NOK, iBorderColor.Blue(),
              EGL_NONE};
    
    // Create the window surface and the egl context and make them current
    iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    
    
    // Create the test bitmap that will be used in the scaled surface 
    //  NOTE: This bitmap generally does not have borders, 
    CFbsBitmap* testBitmap = CreateBitmapLC(TSize(iSurfaceWidth, iSurfaceHeight),0,0,0,0,TRgb(0,0,0));
    
    // Copy test bitmap to drawing surface
    WritePixelsToSurfaceL(*testBitmap);

    // we need to update the window content for the first frame comparison inside the loop further down 
	ASSERT_EGL_TRUE(eglSwapBuffers(iDisplay, iEglSess->Surface()));
	ASSERT_EGL_TRUE(eglWaitClient());

	// Start doing iterations changing border colours and increasing target extent size 
    //  We do increments of (2,2) because the reference bitmap is simetrically split into 4 areas, so size
    //  of this reference bitmap should be an even number of pixels
    INFO_PRINTF1(_L("Start changing border colours and increasing target extent size by (2,2)..."));
    for (;;)
    	{
		// re-populate buffers with the unmodified fixed size surface content
		WritePixelsToSurfaceL(*testBitmap);

		// perform the testing
		doTestPartialStepL();
		
		// break the loop when necessary
		if (iBorderColor.Blue()>=255)
			break;
		}

    /// Cleanup
    CleanupStack::PopAndDestroy(testBitmap);
    iEglSess->CleanupSurfaceAndContextL();
    CleanAll();
    CloseWindow();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScaling_SwapBuffers::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

TVerdict CEglTest_SurfaceScaling_SwapBuffers::doTestPartialStepL()
    {
    INFO_PRINTF4(_L("doTestPartialStepL started for border colour (%d,%d,%d)...."), iBorderColor.Red(), iBorderColor.Green(), iBorderColor.Blue());

	// Note that borders will appear while extent is smaller than window
	//  - iSurfaceWidth/iSurfaceHeight - unmodified
	//  - iExtentWidth/iExtentHeight - modified 
	//  - iOffsetX/iOffsetY - modified
	
    // Set up new value for border blue channel, storing old ones
	TRgb oldBorderColor(iBorderColor);
	TInt newColorBlue = oldBorderColor.Blue() + 5;
	TESTL(newColorBlue>=0 && newColorBlue<=255);
	iBorderColor.SetBlue(newColorBlue);
	
    // Set up new values for extent/offset, storing old ones
	TInt oldExtentWidth = iExtentWidth;
	TInt oldExtentHeight = iExtentHeight;
	iExtentWidth+=2;
	iExtentHeight+=2;
	iOffsetX = (iWindowWidth-iExtentWidth)/2; 
	iOffsetY = (iWindowHeight-iExtentHeight)/2;
	
	// Set up expected values for the border, storing old ones
	TInt oldBorderTop = iBorderTop;
	TInt oldBorderBottom = iBorderBottom;
	TInt oldBorderLeft = iBorderLeft;
	TInt oldBorderRight = iBorderRight;
	iBorderTop = iBorderBottom = iExtentHeight<iWindowHeight ? (iWindowHeight-iExtentHeight)/2 : 0; 
	iBorderLeft = iBorderRight = iExtentWidth<iWindowWidth ? (iWindowWidth-iExtentWidth)/2 : 0; 

    // set new extent/offset values
    ASSERT_EGL_TRUE(iPfnEglSetSurfaceScalingNOK(iDisplay, iEglSess->Surface(), iOffsetX, iOffsetY, iExtentWidth, iExtentHeight));
    // set new border colour values
    ASSERT_EGL_TRUE(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_BLUE_NOK, iBorderColor.Blue()));
    
    // Check that values have been updated as expected
    CheckScalingAttributesL(iSurfaceWidth, iSurfaceHeight, iExtentWidth, iExtentHeight, iOffsetX, iOffsetY); 
    CheckBorderColorL(iBorderColor.Red(), iBorderColor.Blue(), iBorderColor.Green()); 

    // Check that the screen has NOT BEEN UPDATED just yet

    // Create the reference bitmap that should be expected before the extent/border changes 
    TInt refWidth = oldExtentWidth>iWindowWidth ? oldExtentWidth : iWindowWidth;
    TInt refHeight = oldExtentHeight>iWindowHeight ? oldExtentHeight : iWindowHeight;
    CFbsBitmap* oldRefBitmap = CreateBitmapLC(TSize(refWidth, refHeight), oldBorderTop, oldBorderBottom, oldBorderLeft, oldBorderRight, oldBorderColor);

    // compare screen with old reference bitmap
    TInt comparison = iImageComparison->CompareScreenImageL(TSize(iWindowWidth, iWindowHeight), TPoint(0,0), TPoint(0,0), *oldRefBitmap, TestStepName());
    TEST(comparison==KErrNone);
    INFO_PRINTF3(_L("CompareScreenImageL before eglSwapBuffers returns %d (expected is %d)"), comparison, KErrNone);

    // extent/offset changes MUST be effective only after swapbuffers 
	ASSERT_EGL_TRUE(eglSwapBuffers(iDisplay, iEglSess->Surface()));
	ASSERT_EGL_TRUE(eglWaitClient());
	// Finish() to ensure all native window operations related to the surface complete before image comparision.
	iWsSession.Finish();
	
	//check values still are as expected
    CheckScalingAttributesL(iSurfaceWidth, iSurfaceHeight, iExtentWidth, iExtentHeight, iOffsetX, iOffsetY); 
    CheckBorderColorL(iBorderColor.Red(), iBorderColor.Blue(), iBorderColor.Green()); 

    // Create the reference bitmap that should be expected after the extent/border changes 
    refWidth = iExtentWidth>iWindowWidth ? iExtentWidth : iWindowWidth;
    refHeight = iExtentHeight>iWindowHeight ? iExtentHeight : iWindowHeight;
    CFbsBitmap* newRefBitmap = CreateBitmapLC(TSize(refWidth, refHeight), iBorderTop, iBorderBottom, iBorderLeft, iBorderRight, iBorderColor);

    // compare screen with new reference bitmap
    comparison = iImageComparison->CompareScreenImageL(TSize(iWindowWidth, iWindowHeight), TPoint(0,0), TPoint(0,0), *newRefBitmap, TestStepName());
    TEST(comparison==KErrNone);
    INFO_PRINTF3(_L("CompareScreenImageL after eglSwapBuffers returns %d (expected is %d)"), comparison, KErrNone);

    /// Cleanup
    CleanupStack::PopAndDestroy(2, oldRefBitmap); // oldRefBitmap, newRefBitmap

    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0652

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Check that all configs that support surface scaling also support window surfaces.

@SYMTestActions
1. Query number of configs
2. Iterate through all configs and check if surface scaling is supported
3. If surface scaling is supported, chech that it supports window surfaces and 
	attempt to create a fixed size window surface
4. If surface scaling is not supported, check eglGetConfigAttrib return value and 
	attempt to create a fixed size window surface

@SYMTestExpectedResults
3. All configs that support surface scaling support window surfaces and window surface creation succeeds.
4. If surface scaling is not supported, eglGetConfigAttrib sets value to EGL_FALSE and window surface creation fails.
*/
TVerdict CEglTest_SurfaceScaling_WindowSurface_Check::doTestStepL()
    {
    INFO_PRINTF1(_L("CEglTest_SurfaceScaling_WindowSurface_Check::doTestStepL"));
    SetTestStepName(_L("GRAPHICS-EGL-0652"));
    SetTestStepID(_L("GRAPHICS-EGL-0652"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100, 100));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    EGLConfig matchingConfigs[KMaxEglConfigs];
    EGLint numConfigs = 0;

    EGLint attrib_list[] = {
					EGL_FIXED_WIDTH_NOK, 50,
					EGL_FIXED_HEIGHT_NOK, 50,
					EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
					EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
					EGL_TARGET_EXTENT_WIDTH_NOK, 100,
					EGL_TARGET_EXTENT_HEIGHT_NOK, 100,
					EGL_NONE};

    // Query total number of configs
    ASSERT_EGL_TRUE(eglGetConfigs(iDisplay, NULL, KMaxTInt, &numConfigs));
    TESTL(numConfigs >= 1 && numConfigs <= KMaxEglConfigs);
    INFO_PRINTF2(_L("Found %d configs in total"), numConfigs);

    // Get all configs
    ASSERT_EGL_TRUE(eglGetConfigs(iDisplay, matchingConfigs, KMaxEglConfigs, &numConfigs));

    // Check that if surface scaling is supported in the config, then window surfaces are supported too
    for(TInt i=0; i<numConfigs; i++)
        {
		// query scaling support
		EGLint scalingSupport = -32; // arbitraty value which is neither EGL_TRUE nor EGL_FALSE
		ASSERT_EGL_TRUE(eglGetConfigAttrib(iDisplay, matchingConfigs[i], EGL_SURFACE_SCALING_NOK, &scalingSupport));
		
		// query window surface support
		EGLint surfaceType=-1;
		ASSERT_EGL_TRUE(eglGetConfigAttrib(iDisplay, matchingConfigs[i], EGL_SURFACE_TYPE, &surfaceType));
		
		// check that if surface scaling is supported, it also supports window surfaces
		if (scalingSupport==EGL_TRUE)
			{
			INFO_PRINTF2(_L("Config %d supports surface scaling. Checking window surface support..."), i);
			TEST((surfaceType&EGL_WINDOW_BIT) == EGL_WINDOW_BIT);
			}
		else
			{
			// check value was updated accordingly
			INFO_PRINTF2(_L("Config %d does NOT support surface scaling."), i);
			TEST(scalingSupport==EGL_FALSE);
			}

		// check if config supports window surface, fixed window surface creation follows scaling support 
		if ((surfaceType&EGL_WINDOW_BIT) == EGL_WINDOW_BIT)
			{
			EGLSurface windowSurface = eglCreateWindowSurface(iDisplay, matchingConfigs[i], &iWindow, attrib_list);
			if (scalingSupport==EGL_TRUE)
				{
				INFO_PRINTF1(_L("Checking window surface creation succeeds..."));
				TEST_EGL_ERROR(windowSurface =! EGL_NO_SURFACE, EGL_SUCCESS);
				}
			else
				{
				INFO_PRINTF1(_L("Checking window surface creation fails..."));
				TEST_EGL_ERROR(windowSurface == EGL_NO_SURFACE, EGL_BAD_MATCH);
				}
			}
        }

    // clean-up
    CleanAll();
    CloseWindow();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScaling_WindowSurface_Check::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0653

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Negative test. This test is to check the return value and error code of eglCreateWindowSurface 
when creating a fixed size window surface with invalid combination of Surface Scaling attributes 
and illegal values for each attribute.

@SYMTestActions
1. Create a fixed size Window Surface with invalid combination of Surface Scaling attributes.
    - Only specify 5 out of 6 of the following attributes, when specified, should be valid.
    EGL_EXTENT_WIDTH_NOK, EGL_EXTENT_HEIGHT_NOK, EGL_TARGET_EXTENT_WIDTH_NOK, 
    EGL_TARGET_EXTENT_HEIGHT_NOK, EGL_TARGET_EXTENT_OFFSET_X_NOK, EGL_TARGET_EXTENT_OFFSET_Y_NOK
2. Create a fixed size Window Surface with illegal values for each attribute.
    [attribute]                         [illegal value]
    EGL_FIXED_WIDTH_NOK                 less than or equal to zero
    EGL_FIXED_HEIGHT_NOK                less than or equal to zero
    EGL_TARGET_EXTENT_WIDTH_NOK         less than or equal to zero
    EGL_TARGET_EXTENT_HEIGHT_NOK        less than or equal to zero

@SYMTestExpectedResults
1. EGL_NO_SURFACE is returned with error set to EGL_BAD_ATTRIBUTE
2. EGL_NO_SURFACE is returned with error set to EGL_BAD_ATTRIBUTE
*/
TVerdict CEglTest_SurfaceScaling_Negative_CreateWindowSurface::doTestStepL()
    {
    INFO_PRINTF1(_L("CEglTest_SurfaceScaling_Negative_CreateWindowSurface::doTestStepL"));
    SetTestStepName(_L("GRAPHICS-EGL-0653"));
    SetTestStepID(_L("GRAPHICS-EGL-0653"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100, 100));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();
    iEglSess->InitializeL();
    
    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    
    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL(numConfigs == 1); // Abort the test if the EGL config is not supported
    
    // Make the fixed size surface half width and height of the window
    TInt surfaceWidth = iWindow.Size().iWidth / 2;
    TInt surfaceHeight = iWindow.Size().iHeight / 2;
    
    TInt invalidCombinationAttrNum = 6;
    EGLint invalid_combination_attrib_list[][11] = {
            {
                EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                EGL_NONE},
            {
                EGL_FIXED_WIDTH_NOK, surfaceWidth,
                EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                EGL_NONE},
            {
                EGL_FIXED_WIDTH_NOK, surfaceWidth,
                EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                EGL_NONE},
            {
                EGL_FIXED_WIDTH_NOK, surfaceWidth,
                EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                EGL_NONE},
            {
                EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                EGL_NONE},
            {
                EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                EGL_NONE}
            };
    
    TInt illegalValAttrNum = 14;
    EGLint illegal_value_attrib_list[][19] = {
                {
                    EGL_FIXED_WIDTH_NOK, -1,
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, 0xfc,
                    EGL_BORDER_COLOR_GREEN_NOK, 0xfd,
                    EGL_BORDER_COLOR_BLUE_NOK, 0xfe,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, 0,
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, 0xfc,
                    EGL_BORDER_COLOR_GREEN_NOK, 0xfd,
                    EGL_BORDER_COLOR_BLUE_NOK, 0xfe,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,
                    EGL_FIXED_HEIGHT_NOK, -1,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, 0xfc,
                    EGL_BORDER_COLOR_GREEN_NOK, 0xfd,
                    EGL_BORDER_COLOR_BLUE_NOK, 0xfe,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,
                    EGL_FIXED_HEIGHT_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, 0xfc,
                    EGL_BORDER_COLOR_GREEN_NOK, 0xfd,
                    EGL_BORDER_COLOR_BLUE_NOK, 0xfe,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, -1,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, 0xfc,
                    EGL_BORDER_COLOR_GREEN_NOK, 0xfd,
                    EGL_BORDER_COLOR_BLUE_NOK, 0xfe,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, 0,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, 0xfc,
                    EGL_BORDER_COLOR_GREEN_NOK, 0xfd,
                    EGL_BORDER_COLOR_BLUE_NOK, 0xfe,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, -1,
                    EGL_BORDER_COLOR_RED_NOK, 0xfc,
                    EGL_BORDER_COLOR_GREEN_NOK, 0xfd,
                    EGL_BORDER_COLOR_BLUE_NOK, 0xfe,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, 0,
                    EGL_BORDER_COLOR_RED_NOK, 0xfc,
                    EGL_BORDER_COLOR_GREEN_NOK, 0xfd,
                    EGL_BORDER_COLOR_BLUE_NOK, 0xfe,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, -1,
                    EGL_BORDER_COLOR_GREEN_NOK, 0,
                    EGL_BORDER_COLOR_BLUE_NOK, 0,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, 0,
                    EGL_BORDER_COLOR_GREEN_NOK, -1,
                    EGL_BORDER_COLOR_BLUE_NOK, 0,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, 0,
                    EGL_BORDER_COLOR_GREEN_NOK, 0,
                    EGL_BORDER_COLOR_BLUE_NOK, -1,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, 256,
                    EGL_BORDER_COLOR_GREEN_NOK, 0,
                    EGL_BORDER_COLOR_BLUE_NOK, 0,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, 0,
                    EGL_BORDER_COLOR_GREEN_NOK, 256,
                    EGL_BORDER_COLOR_BLUE_NOK, 0,
                    EGL_NONE},
                {
                    EGL_FIXED_WIDTH_NOK, surfaceWidth,  
                    EGL_FIXED_HEIGHT_NOK, surfaceHeight,
                    EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                    EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                    EGL_TARGET_EXTENT_WIDTH_NOK, iWindow.Size().iWidth,
                    EGL_TARGET_EXTENT_HEIGHT_NOK, iWindow.Size().iHeight,
                    EGL_BORDER_COLOR_RED_NOK, 0,
                    EGL_BORDER_COLOR_GREEN_NOK, 0,
                    EGL_BORDER_COLOR_BLUE_NOK, 256,
                    EGL_NONE}
                };

    INFO_PRINTF1(_L("Calling eglCreateWindowSurface with invalid combination of Surface Scaling attributes... - only five out of six compulsary attributes specified"));
    for(TInt i = 0;i < invalidCombinationAttrNum;i++)
        {
        EGLSurface surface = eglCreateWindowSurface(iDisplay, matchingConfigs[0], &iWindow, invalid_combination_attrib_list[i]);    
        TEST_EGL_ERROR(surface == EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE);
        }
    
    INFO_PRINTF1(_L("Calling eglCreateWindowSurface with illegal values for each attribute..."));
    for(TInt i = 0;i < illegalValAttrNum;i++)
        {
        EGLSurface surface = eglCreateWindowSurface(iDisplay, matchingConfigs[0], &iWindow, illegal_value_attrib_list[i]);    
        TEST_EGL_ERROR(surface == EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE);
        }

    // clean-up
    CloseWindow();
    CleanAll();

    INFO_PRINTF1(_L("....CEglTest_SurfaceScaling_Negative_CreateWindowSurface::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0654

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Negative test. This test is to check the return value and error code when creating
a fixed size non window surface.

@SYMTestActions
1. Attempt to create a pixmap with scaling attributes specified
2. Attempt to create a pbuffer surface with scaling attributes specified
3. Attempt to create a window surface with scaling attributes specified but 
   config doesn't support surface scaling

@SYMTestExpectedResults
1. EGL_NO_SURFACE is returned with error set to EGL_BAD_ATTRIBUTE
2. EGL_NO_SURFACE is returned with error set to EGL_BAD_ATTRIBUTE
3. EGL_NO_SURFACE is returned with error set to EGL_BAD_MATCH
*/
TVerdict CEglTest_SurfaceScaling_Negative_FixedSize_NonWindowSurface::doTestStepL()
    {
    INFO_PRINTF1(_L("CEglTest_SurfaceScaling_Negative_FixedSize_NonWindowSurface::doTestStepL"));
    SetTestStepName(_L("GRAPHICS-EGL-0654"));
    SetTestStepID(_L("GRAPHICS-EGL-0654"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100, 100));

    // Create bitmap
    User::LeaveIfError(RFbsSession::Connect());
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Create(TSize(20, 20), EColor16MU));
    
    // Create display object
    GetDisplayL();
    CreateEglSessionL();
    iEglSess->InitializeL();
    
    EGLConfig config;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLint numConfigs = 0;
    
    // Use OpenVG to draw
    ASSERT_EGL_TRUE(eglBindAPI(EGL_OPENVG_API));
    
    const EGLint KFixedSize_surface_attrib_list[] = {
                EGL_FIXED_WIDTH_NOK, 100,
                EGL_FIXED_HEIGHT_NOK, 100,
                EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
                EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
                EGL_TARGET_EXTENT_WIDTH_NOK, 150,
                EGL_TARGET_EXTENT_HEIGHT_NOK, 150,
                EGL_NONE};
           
    // If all window surfaces in EGL are scalable, first case must be skipped. Others still valid
    TInt index = iAllScalable ? 1 : 0;
    for(; index < 3; index++)
        {
        switch(index)
            {
		case 0:
			INFO_PRINTF1(_L("Calling eglCreateWindowSurface with surface scaling attribute specified in a config that does not support scaling..."));
			ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribs_NoScaling], &config, 1, &numConfigs));
			TEST(numConfigs==1);
			surface = eglCreateWindowSurface(iDisplay, config, &iWindow, KFixedSize_surface_attrib_list);    
			TEST_EGL_ERROR(surface == EGL_NO_SURFACE, EGL_BAD_MATCH);
			break;
        case 1:
			INFO_PRINTF1(_L("Calling eglCreatePixmapSurface with surface scaling attribute specified..."));
		    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribs[EPixmapAttribsColor16MU], &config, 1, &numConfigs));
		    TEST(numConfigs==1);
			surface = eglCreatePixmapSurface(iDisplay, config, (NativePixmapType)bitmap, KFixedSize_surface_attrib_list);
			TEST_EGL_ERROR(surface == EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE);
            break;
        case 2:
            INFO_PRINTF1(_L("Calling eglCreatePbufferSurface with surface scaling attribute specified..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribs[EPBufferAttribsColor64K], &config, 1, &numConfigs));
            TEST(numConfigs==1);
            surface = eglCreatePbufferSurface(iDisplay, config, KFixedSize_surface_attrib_list);    
            TEST_EGL_ERROR(surface == EGL_NO_SURFACE, EGL_BAD_ATTRIBUTE);
            break;
			}
        }
   
    // cleanup
    CleanupStack::PopAndDestroy(bitmap);
    CloseWindow();
    CleanAll();
    RFbsSession::Disconnect();

    INFO_PRINTF1(_L("....CEglTest_SurfaceScaling_Negative_FixedSize_NonWindowSurface::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0655

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Exercising border color set by default.

@SYMTestActions
1. Create a fixed size EGL Window Surface without border color specified.
2. Create a fixed size EGL Window Surface with border color specified for one channel.
3. Create a fixed size EGL Window Surface with border color specified for two channels.
 
@SYMTestExpectedResults
Retrieved border color matches to the value it has been set at surface creation, 
if during the surface creation border color has not been specified it is set to black (0x0).   
*/
TVerdict CEglTest_SurfaceScalingDefaultBorderColor::doTestStepL()
    {
    SetTestStepName(_L("GRAPHICS-EGL-0655"));
    SetTestStepID(_L("GRAPHICS-EGL-0655"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScalingDefaultBorderColor::doTestStepL"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100,100));
    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    
    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();
    
    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    
    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL (numConfigs == 1); // Abort the test if the EGL config is not supported
    
    // Make the fixed size surface half width and height of the window
    const TInt KSurfaceWidth = KWindowWidth/2;
    const TInt KSurfaceHeight = KWindowHeight/2;
    
    EGLint attrib_list1[] = {
              EGL_FIXED_WIDTH_NOK, KSurfaceWidth,
              EGL_FIXED_HEIGHT_NOK, KSurfaceHeight,
              EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
              EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
              EGL_TARGET_EXTENT_WIDTH_NOK, KWindowWidth,
              EGL_TARGET_EXTENT_HEIGHT_NOK, KWindowHeight,
              EGL_NONE};

    EGLint attrib_list2[] = {
              EGL_FIXED_WIDTH_NOK, KSurfaceWidth,
              EGL_FIXED_HEIGHT_NOK, KSurfaceHeight,
              EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
              EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
              EGL_TARGET_EXTENT_WIDTH_NOK, KWindowWidth,
              EGL_TARGET_EXTENT_HEIGHT_NOK, KWindowHeight,
              EGL_BORDER_COLOR_RED_NOK, 0xf0,
              EGL_NONE};

    EGLint attrib_list3[] = {
              EGL_FIXED_WIDTH_NOK, KSurfaceWidth,
              EGL_FIXED_HEIGHT_NOK, KSurfaceHeight,
              EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
              EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
              EGL_TARGET_EXTENT_WIDTH_NOK, KWindowWidth,
              EGL_TARGET_EXTENT_HEIGHT_NOK, KWindowHeight,
              EGL_BORDER_COLOR_BLUE_NOK, 0xf8,
              EGL_BORDER_COLOR_GREEN_NOK, 0xfc,
              EGL_NONE};
    
    for(TInt index = 0; index < 3; index++)
        {
        EGLint* attrib_list = NULL;
        EGLint expectedRedChannelColor = 0; 
        EGLint expectedGreenChannelColor = 0; 
        EGLint expectedBlueChannelColor = 0; 

        switch(index)
            {
        case 0:
            attrib_list = attrib_list1;
            break;
        case 1:
            expectedRedChannelColor = 0xf0;
            attrib_list = attrib_list2;
            break;
        case 2:
            expectedBlueChannelColor = 0xf8;
            expectedGreenChannelColor = 0xfc;
            attrib_list = attrib_list3;
            break;
            }
        // Create the window surface and the egl context and make them current
        iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    
        
        // Check expected border colours 
        CheckBorderColorL(expectedRedChannelColor, expectedBlueChannelColor, expectedGreenChannelColor);
    
        // Cleanup
        iEglSess->CleanupSurfaceAndContextL();
        }
    
    CleanAll();
    CloseWindow();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScalingDefaultBorderColor::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }


/**
@SYMTestCaseID GRAPHICS-EGL-0656

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Modifying an existing fixed size surface border color. 

@SYMTestActions
1. Create a fixed size EGL Window Surface with border color specified.
2. Modify border color with the new values.
 
@SYMTestExpectedResults
New color will take place only after setting new values.   
*/

TVerdict CEglTest_SurfaceScalingModifyingBorderColor::doTestStepL()
    {
	SetTestStepName(_L("GRAPHICS-EGL-0656"));
	SetTestStepID(_L("GRAPHICS-EGL-0656"));
	INFO_PRINTF1(_L("CEglTest_SurfaceScalingModifyingBorderColor::doTestStepL"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100,100));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    
    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL (numConfigs == 1); // Abort the test if the EGL config is not supported
    
    // Make the fixed size surface half width and height of the window
    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    const TInt KSurfaceWidth = KWindowWidth/2;
    const TInt KSurfaceHeight = KWindowHeight/2;
    const TInt KExtentWidth = KSurfaceWidth;
    const TInt KExtentHeight = KSurfaceHeight;
    const EGLint KRedChannelColor = 0x20; 
    const EGLint KGreenChannelColor = 0x40; 
    const EGLint KBlueChannelColor = 0x60; 

    EGLint attrib_list[] = {
              EGL_FIXED_WIDTH_NOK, KSurfaceWidth,
              EGL_FIXED_HEIGHT_NOK, KSurfaceHeight,
              EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
              EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
              EGL_TARGET_EXTENT_WIDTH_NOK, KExtentWidth,
              EGL_TARGET_EXTENT_HEIGHT_NOK, KExtentHeight,
              EGL_BORDER_COLOR_RED_NOK, KRedChannelColor,
              EGL_BORDER_COLOR_BLUE_NOK, KBlueChannelColor,
              EGL_BORDER_COLOR_GREEN_NOK, KGreenChannelColor,
              EGL_NONE};

    // Create the window surface and the egl context and make them current
    iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    

    CheckBorderColorL(KRedChannelColor, KBlueChannelColor, KGreenChannelColor);
    
    //modify existing attributes
    const EGLint KOffsetColor = 100; 
    ASSERT_EGL_TRUE(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_RED_NOK, KRedChannelColor + KOffsetColor));
    ASSERT_EGL_TRUE(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_BLUE_NOK, KBlueChannelColor + KOffsetColor));
    ASSERT_EGL_TRUE(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_GREEN_NOK, KGreenChannelColor + KOffsetColor));

    //check that border color has been modified now
    CheckBorderColorL(KRedChannelColor+KOffsetColor, KBlueChannelColor+KOffsetColor, KGreenChannelColor+KOffsetColor);
    
    // Cleanup
    iEglSess->CleanupSurfaceAndContextL();
    CleanAll();
    CloseWindow();
    
	INFO_PRINTF1(_L("....CEglTest_SurfaceScalingModifyingBorderColor::doTestStepL completed!"));

    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID 	GRAPHICS-EGL-0657

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Negative testing. Modifying an existing non-fixed size surface border color. 

@SYMTestActions
1. Create a non-fixed size EGL Window Surface.
2. Try to set border color after surface creation.
3. Repeat steps 1-2 for pixmap and pbuffer surface

@SYMTestExpectedResults
Setting border color has no effect.   
*/
TVerdict CEglTest_SurfaceScalingModifyingBorderColorNonFixed::doTestStepL()
    {
	SetTestStepName(_L("GRAPHICS-EGL-0657"));
	SetTestStepID(_L("GRAPHICS-EGL-0657"));
	INFO_PRINTF1(_L("CEglTest_SurfaceScalingModifyingBorderColorNonFixed::doTestStepL"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100,100));

    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    const TInt KSurfaceWidth = KWindowWidth;
    const TInt KSurfaceHeight = KWindowHeight;

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    
    // If all window surfaces in EGL are scalable, first case must be skipped. Others still valid
    TInt index = iAllScalable ? 1 : 0;
    for(; index < 3; index++)
        {
        switch(index)
            {
        case 0:
            // Create the non-fixed size window surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the non-fixed size window surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribs_NoScaling], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow);    
            break;
        case 1:
            // Create the pbuffer surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the pbuffer surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribs[EPBufferAttribsColor16MU], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreatePbufferSurfaceAndMakeCurrentL(matchingConfigs[0], TSize(KSurfaceWidth, KSurfaceHeight));
            break;
        case 2:
            // Create the pixmap surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the pixmap surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribs[EPixmapAttribsColor16MU], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(TSize(KSurfaceWidth, KSurfaceHeight), EColor16MU);
            break;
            }

		// Set border color values to a non fixed size window (nothing should happen)
		const EGLint KRedChannelColor = 0x20; 
		const EGLint KGreenChannelColor = 0x40; 
		const EGLint KBlueChannelColor = 0x60; 
		ASSERT_EGL_TRUE(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_RED_NOK, KRedChannelColor));
		ASSERT_EGL_TRUE(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_BLUE_NOK, KBlueChannelColor));
		ASSERT_EGL_TRUE(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_GREEN_NOK, KGreenChannelColor));

		// Check all attributes (this is a non-fixed size window) 
		// Note that we cannot use CheckBorderColorL because values are not updated
		EGLint redChannelColor = -1; 
		EGLint greenChannelColor = -2; 
		EGLint blueChannelColor = -3; 
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_RED_NOK, &redChannelColor));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_BLUE_NOK, &blueChannelColor));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_GREEN_NOK, &greenChannelColor));
		TEST(redChannelColor == -1);
		TEST(greenChannelColor == -2);
		TEST(blueChannelColor == -3);

		// Set invalid border color values to a non fixed size window (nothing should happen)
		INFO_PRINTF1(_L("Attempt to set invalid border color values to a non fixed size window..."));
		ASSERT_EGL_TRUE(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_RED_NOK, KRedChannelColor+256));
		ASSERT_EGL_TRUE(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_BLUE_NOK, KBlueChannelColor+256));
		ASSERT_EGL_TRUE(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_GREEN_NOK, KGreenChannelColor+256));
		
		// Check all attributes (this is a non-fixed size window) 
		// Note that we cannot use CheckBorderColorL because values are not updated
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_RED_NOK, &redChannelColor));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_BLUE_NOK, &blueChannelColor));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_GREEN_NOK, &greenChannelColor));
		TEST(redChannelColor == -1);
		TEST(greenChannelColor == -2);
		TEST(blueChannelColor == -3);
    
	   	// destroy surface and context
	    iEglSess->CleanupSurfaceAndContextL();
        }
    
    // Cleanup
    CleanAll();
    CloseWindow();
    
	INFO_PRINTF1(_L("....CEglTest_SurfaceScalingModifyingBorderColorNonFixed::doTestStepL completed!"));

    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0658

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Negative testing. Attempt to modify an existing fixed size surface border color with invalid values. 

@SYMTestActions
1. Create a fixed size EGL Window Surface with border color specified.
2. Try to modify border color with the negative values.
3. Try to modify border color with the positive values greater than 255.
 
@SYMTestExpectedResults
Attempt to modify the border color with the new values will fail with error code EGL_BAD_ATTRIBUTE.   
*/
TVerdict CEglTest_SurfaceScalingModifyingInvalidBorderColor::doTestStepL()
    {
    SetTestStepName(_L("GRAPHICS-EGL-0658"));
    SetTestStepID(_L("GRAPHICS-EGL-0658"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScalingModifyingInvalidBorderColor::doTestStepL"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100,100));
    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    
    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL (numConfigs == 1); // Abort the test if the EGL config is not supported
    
    // Make the fixed size surface half width and height of the window
    const TInt KSurfaceWidth = KWindowWidth/2;
    const TInt KSurfaceHeight = KWindowHeight/2;
    const EGLint KRedChannelColor = 0x20; 
    const EGLint KGreenChannelColor = 0x40; 
    const EGLint KBlueChannelColor = 0x60; 

    EGLint attrib_list[] = {
              EGL_FIXED_WIDTH_NOK, KSurfaceWidth,
              EGL_FIXED_HEIGHT_NOK, KSurfaceHeight,
              EGL_TARGET_EXTENT_OFFSET_X_NOK, 0,
              EGL_TARGET_EXTENT_OFFSET_Y_NOK, 0,
              EGL_TARGET_EXTENT_WIDTH_NOK, KWindowWidth,
              EGL_TARGET_EXTENT_HEIGHT_NOK, KWindowHeight,
              EGL_BORDER_COLOR_RED_NOK, KRedChannelColor,
              EGL_BORDER_COLOR_BLUE_NOK, KBlueChannelColor,
              EGL_BORDER_COLOR_GREEN_NOK, KGreenChannelColor,
              EGL_NONE};


    // Create the window surface and the egl context and make them current
    iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    

    // check initial values match
    CheckBorderColorL(KRedChannelColor, KBlueChannelColor, KGreenChannelColor);
    
    //modify existing attributes with negative value
	INFO_PRINTF1(_L("Attempt to set negative border color values..."));
    TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_RED_NOK, -KRedChannelColor) == EGL_FALSE);
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_BLUE_NOK, -KBlueChannelColor) == EGL_FALSE);
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_GREEN_NOK, -KGreenChannelColor) == EGL_FALSE);
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);

    //check that border color has not been modified
    CheckBorderColorL(KRedChannelColor, KBlueChannelColor, KGreenChannelColor);
    
    //modify existing attributes with very big positive value
	INFO_PRINTF1(_L("Attempt to set border color values that are too big..."));
    TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_RED_NOK, KRedChannelColor + 256) == EGL_FALSE);
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_BLUE_NOK, KBlueChannelColor + 256) == EGL_FALSE);
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_GREEN_NOK, KGreenChannelColor + 256) == EGL_FALSE);
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);

    //check that border color has not been modified
    CheckBorderColorL(KRedChannelColor, KBlueChannelColor, KGreenChannelColor);

    /// Cleanup
    iEglSess->CleanupSurfaceAndContextL();
    CleanAll();
    CloseWindow();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScalingModifyingInvalidBorderColor::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0659

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Negative testing. Attempt to modify an extent with eglSurfaceAttrib() after surface has been created. 

@SYMTestActions
1. Create a fixed size EGL Window Surface.
2. Try to modify a surface extent parameters with arbitrary values via eglSurfaceAttrib().

@SYMTestExpectedResults
Attempt to modify the extent will fail with error code EGL_BAD_ATTRIBUTE.   
*/
TVerdict CEglTest_SurfaceScalingModifyingExtent::doTestStepL()
    {
    SetTestStepName(_L("GRAPHICS-EGL-0659"));
    SetTestStepID(_L("GRAPHICS-EGL-0659"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScalingModifyingExtent::doTestStepL"));

     // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100,100));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    INFO_PRINTF1(_L("Attempt to modify surface extent for fixed size surfaces..."));
    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
	TESTL (numConfigs == 1); // Abort the test if the EGL config is not supported

    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    const TInt KSurfaceWidth = KWindowWidth/2;
	const TInt KSurfaceHeight = KWindowHeight/2;
	const TInt KExtentWidth = KWindowWidth;
	const TInt KExtentHeight = KWindowHeight;
    const TInt KOffsetX = 10;
	const TInt KOffsetY = 20;
            
    EGLint attrib_list[] = {
                  EGL_FIXED_WIDTH_NOK, KSurfaceWidth,
                  EGL_FIXED_HEIGHT_NOK, KSurfaceHeight,
                  EGL_TARGET_EXTENT_OFFSET_X_NOK, KOffsetX,
                  EGL_TARGET_EXTENT_OFFSET_Y_NOK, KOffsetY,
                  EGL_TARGET_EXTENT_WIDTH_NOK, KExtentWidth,
                  EGL_TARGET_EXTENT_HEIGHT_NOK, KExtentHeight,
                  EGL_NONE};
    
	// Create the window surface and the egl context and make them current
	iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    

	// Invalid update - modify existing attributes with any value
	TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_FIXED_WIDTH_NOK, KSurfaceWidth/2) == EGL_FALSE);
	ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
	TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_FIXED_HEIGHT_NOK, KSurfaceHeight/2) == EGL_FALSE);
	ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
	TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_WIDTH_NOK, KExtentWidth/2) == EGL_FALSE);
	ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
	TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_HEIGHT_NOK, KExtentHeight/2) == EGL_FALSE);
	ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
	TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_X_NOK, KOffsetX/2) == EGL_FALSE);
	ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
	TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_Y_NOK, KOffsetY/2) == EGL_FALSE);
	ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);

    // check that attributes have not been modified
    CheckScalingAttributesL(KSurfaceWidth, KSurfaceHeight, KExtentWidth, KExtentHeight, KOffsetX, KOffsetY);

	/// Cleanup
    iEglSess->CleanupSurfaceAndContextL();
    CleanAll();
    CloseWindow();

    INFO_PRINTF1(_L("....CEglTest_SurfaceScalingModifyingExtent::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0671

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Negative testing. Attempt to modify an extent with eglSurfaceAttrib() after surface has been created. 

@SYMTestActions
1. Create a non fixed size EGL Window Surface.
2. Try to modify a surface extent parameters with arbitrary values via eglSurfaceAttrib().
3. Repeat steps 2-3 for pixmap and pbuffer surface

@SYMTestExpectedResults
Attempt to modify the extent will fail with error code EGL_BAD_ATTRIBUTE.   
*/
TVerdict CEglTest_SurfaceScalingModifyingExtentNonFixed::doTestStepL()
    {
    SetTestStepName(_L("GRAPHICS-EGL-0671"));
    SetTestStepID(_L("GRAPHICS-EGL-0671"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScalingModifyingExtentNonFixed::doTestStepL"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100,100));

    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    const TInt KSurfaceWidth = KWindowWidth;
    const TInt KSurfaceHeight = KWindowHeight;
	const TInt KExtentWidth = KWindowWidth;
	const TInt KExtentHeight = KWindowHeight;
    const TInt KOffsetX = 11;
	const TInt KOffsetY = 22;

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();
     
    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    
    // If all window surfaces in EGL are scalable, first case must be skipped. Others still valid
    TInt index = iAllScalable ? 1 : 0;
    for(; index < 3; index++)
        {
        switch(index)
            {
        case 0:
            // Create the non-fixed size window surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the non-fixed size window surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribs_NoScaling], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow);    
            break;
        case 1:
            // Create the pbuffer surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the pbuffer surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribs[EPBufferAttribsColor16MU], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreatePbufferSurfaceAndMakeCurrentL(matchingConfigs[0], TSize(KSurfaceWidth, KSurfaceHeight));
            break;
        case 2:
            // Create the pixmap surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the pixmap surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribs[EPixmapAttribsColor16MU], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(TSize(KSurfaceWidth, KSurfaceHeight), EColor16MU);
            break;
            }

		// Invalid update - modify existing attributes with any value
		TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_FIXED_WIDTH_NOK, KSurfaceWidth/2) == EGL_FALSE);
		ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
		TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_FIXED_HEIGHT_NOK, KSurfaceHeight/2) == EGL_FALSE);
		ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
		TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_WIDTH_NOK, KExtentWidth/2) == EGL_FALSE);
		ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
		TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_HEIGHT_NOK, KExtentHeight/2) == EGL_FALSE);
		ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
		TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_X_NOK, KOffsetX/2) == EGL_FALSE);
		ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
		TEST(eglSurfaceAttrib(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_Y_NOK, KOffsetY/2) == EGL_FALSE);
		ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);

	   	// destroy surface and context
	    iEglSess->CleanupSurfaceAndContextL();
        }
    
	/// Cleanup
    CleanAll();
    CloseWindow();

    INFO_PRINTF1(_L("....CEglTest_SurfaceScalingModifyingExtentNonFixed::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0660

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Query surface attrubutes related to scaling. 

@SYMTestActions
1. Create a non-fixed size EGL Window Surface.
2. Query surface attributes.

@SYMTestExpectedResults
2. Query surface attributes succeeds and all atributes matches to the expected values.
*/
TVerdict CEglTest_SurfaceScalingQuerySurface::doTestStepL()
    {
    SetTestStepName(_L("GRAPHICS-EGL-0660"));
    SetTestStepID(_L("GRAPHICS-EGL-0660"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScalingQuerySurface::doTestStepL"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100,100));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL (numConfigs == 1); // Abort the test if the EGL config is not supported

    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    const TInt KSurfaceWidth = KWindowWidth/2;
    const TInt KSurfaceHeight = KWindowHeight/2;
    const TInt KExtentWidth = KWindowWidth;
    const TInt KExtentHeight = KWindowHeight;
    const EGLint KXOffset = 10;
    const EGLint KYOffset = 20;
    const EGLint KBorderColorRed = 0xa0;
    const EGLint KBorderColorBlue = 0xf0;
    const EGLint KBorderColorGreen = 0xfc;

    EGLint attrib_list[] = {
                  EGL_FIXED_WIDTH_NOK, KSurfaceWidth,
                  EGL_FIXED_HEIGHT_NOK, KSurfaceHeight,
                  EGL_TARGET_EXTENT_OFFSET_X_NOK, KXOffset,
                  EGL_TARGET_EXTENT_OFFSET_Y_NOK, KYOffset,
                  EGL_TARGET_EXTENT_WIDTH_NOK, KExtentWidth,
                  EGL_TARGET_EXTENT_HEIGHT_NOK, KExtentHeight,
                  EGL_BORDER_COLOR_BLUE_NOK, KBorderColorBlue,
                  EGL_BORDER_COLOR_GREEN_NOK, KBorderColorGreen,
                  EGL_BORDER_COLOR_RED_NOK, KBorderColorRed,
                  EGL_NONE};

	// Create the window surface and the egl context and make them current
    iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    

    INFO_PRINTF1(_L("Query surface attributes and border colour back and check expected values..."));
    CheckBorderColorL(KBorderColorRed, KBorderColorBlue, KBorderColorGreen);
    CheckScalingAttributesL(KSurfaceWidth, KSurfaceHeight, KExtentWidth, KExtentHeight, KXOffset, KYOffset);
   
    INFO_PRINTF1(_L("Check EGL_FIXED_WIDTH_NOK and EGL_FIXED_HEIGHT_NOK cannot be queried..."));
    EGLint surfaceFixedWidth = -1;
    EGLint surfaceFixedHeight = -2;
    TEST(EGL_FALSE == eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_FIXED_WIDTH_NOK, &surfaceFixedWidth));
    ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
    TEST(EGL_FALSE == eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_FIXED_HEIGHT_NOK, &surfaceFixedHeight));
    ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
	TEST(surfaceFixedWidth == -1);
	TEST(surfaceFixedHeight == -2);
    
	// clean-up
	iEglSess->CleanupSurfaceAndContextL();
    CloseWindow();
    CleanAll();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScalingQuerySurface::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }
    
/**
@SYMTestCaseID GRAPHICS-EGL-0662

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Negative testing. Query surface attributes which are not supported by this API. 

@SYMTestActions
1. Create a non-fixed size EGL window surface.
2. Query surface attributes.
3. Repeat step 2 for EGL window surface, pixmap and pbuffer surface

@SYMTestExpectedResults
Query surface attributes will fail with error code EGL_BAD_ATTRIBUTE.   
*/
TVerdict CEglTest_SurfaceScalingQuerySurfaceNonFixed::doTestStepL()
    {
    SetTestStepName(_L("GRAPHICS-EGL-0662"));
    SetTestStepID(_L("GRAPHICS-EGL-0662"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScalingQuerySurfaceNonFixed::doTestStepL"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100,100));

    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    const TInt KSurfaceWidth = KWindowWidth;
    const TInt KSurfaceHeight = KWindowHeight;

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();
     
    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    
    // If all window surfaces in EGL are scalable, first case must be skipped. Others still valid
    TInt index = iAllScalable ? 1 : 0;
    for(; index < 3; index++)
        {
        switch(index)
            {
        case 0:
            // Create the non-fixed size window surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the non-fixed size window surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribs_NoScaling], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow);    
            break;
        case 1:
            // Create the pbuffer surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the pbuffer surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribs[EPBufferAttribsColor16MU], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreatePbufferSurfaceAndMakeCurrentL(matchingConfigs[0], TSize(KSurfaceWidth, KSurfaceHeight));
            break;
        case 2:
            // Create the pixmap surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the pixmap surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribs[EPixmapAttribsColor16MU], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(TSize(KSurfaceWidth, KSurfaceHeight), EColor16MU);
            break;
            }

        INFO_PRINTF1(_L("Check EGL_FIXED_WIDTH_NOK and EGL_FIXED_HEIGHT_NOK cannot be queried..."));
        EGLint surfaceFixedWidth = -1;
        EGLint surfaceFixedHeight = -2;
        TEST(EGL_FALSE == eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_FIXED_WIDTH_NOK, &surfaceFixedWidth));
        ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
        TEST(EGL_FALSE == eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_FIXED_HEIGHT_NOK, &surfaceFixedHeight));
        ASSERT_EGL_ERROR(EGL_BAD_ATTRIBUTE);
        TEST(surfaceFixedWidth == -1);
        TEST(surfaceFixedHeight == -2);
    
        INFO_PRINTF1(_L("Check surface size is as we would expect..."));
        EGLint surfaceWidth = 0;
    	EGLint surfaceHeight = 0;
    	ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_WIDTH, &surfaceWidth));
    	ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_HEIGHT, &surfaceHeight));
    	TEST(surfaceWidth == KWindowWidth);		// non-fixed size surface
    	TEST(surfaceHeight == KWindowHeight);	// non-fixed size surface

    	INFO_PRINTF1(_L("Check scaling attributes cannot be queried..."));
    	EGLint extentOffsetX = -1;
    	EGLint extentOffsetY = -2;
    	EGLint extentWidth = -3;
    	EGLint extentHeight = -4;
    	EGLint borderColorBlue = -5;
    	EGLint borderColorRed = -6;
    	EGLint borderColorGreen = -7;
    	ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_X_NOK, &extentOffsetX));
    	ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_Y_NOK, &extentOffsetY));
    	ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_WIDTH_NOK, &extentWidth));
    	ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_HEIGHT_NOK, &extentHeight));
    	ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_BLUE_NOK, &borderColorBlue));
    	ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_GREEN_NOK, &borderColorGreen));
    	ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_BORDER_COLOR_RED_NOK, &borderColorRed));
    	TEST(extentOffsetX == -1);
    	TEST(extentOffsetY == -2);
    	TEST(extentWidth == -3);
    	TEST(extentHeight == -4);
    	TEST(borderColorBlue == -5);
    	TEST(borderColorRed == -6);
    	TEST(borderColorGreen == -7);

    	// destroy surface and context
        iEglSess->CleanupSurfaceAndContextL();
        }

    // clean-up
    CloseWindow();
    CleanAll();

    INFO_PRINTF1(_L("....CEglTest_SurfaceScalingQuerySurfaceNonFixed::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0663

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Negative testing. Query surface scaling capability with invalid parameters. 

@SYMTestActions
1. Query surface scaling capability for the different configs.
2. Query surface scaling capability with invalid display.
3. Query surface scaling capability with negative surface width.
4. Query surface scaling capability with negative surface height.
5. Query surface scaling capability with negative target width.
6. Query surface scaling capability with negative target height.
7. Query surface scaling capability with zero surface width.
8. Query surface scaling capability with zero surface height.
9. Query surface scaling capability with zero target width.
10. Query surface scaling capability with zero target height.

@SYMTestExpectedResults
1. Query surface capability will fail with error code EGL_BAD_MATCH if config doesn't support scaling and succeed otherwise.   
2. Query surface capability will fail with error code EGL_BAD_DISPLAY.   
3. Query surface capability will fail with error code EGL_BAD_PARAMETER.   
4. Query surface capability will fail with error code EGL_BAD_PARAMETER.   
5. Query surface capability will fail with error code EGL_BAD_PARAMETER.   
6. Query surface capability will fail with error code EGL_BAD_PARAMETER.   
7. Query surface capability will fail with error code EGL_BAD_PARAMETER.   
8. Query surface capability will fail with error code EGL_BAD_PARAMETER.   
9. Query surface capability will fail with error code EGL_BAD_PARAMETER.   
10. Query surface capability will fail with error code EGL_BAD_PARAMETER.   
*/
TVerdict CEglTest_SurfaceScalingCapability::doTestStepL()
    {
    SetTestStepName(_L("GRAPHICS-EGL-0663"));
    SetTestStepID(_L("GRAPHICS-EGL-0663"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScalingCapability::doTestStepL"));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[KMaxEglConfigs];
    EGLint numConfigs = 0;
    
    CreateAndActivateWindowL(TSize(100,100));
    
    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    const TInt KSurfaceWidth = KWindowWidth/2;
    const TInt KSurfaceHeight = KWindowHeight/2;
    const EGLDisplay KInvalidDisplay = iDisplay - 100;
    EGLint capability = -1; //arbitrary number beyond the existing range 
    EGLConfig config = -1;
    
    INFO_PRINTF1(_L("Calling eglGetConfigs to get configs..."));
    ASSERT_EGL_TRUE(eglGetConfigs(iDisplay, matchingConfigs, KMaxEglConfigs, &numConfigs));
    for(TInt index = 0; index < numConfigs; index++)
        {
        EGLint value = EGL_FALSE;
        ASSERT_EGL_TRUE(eglGetConfigAttrib(iDisplay, matchingConfigs[index], EGL_SURFACE_SCALING_NOK, &value));
        if(value == EGL_FALSE)
            {
            TEST(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, matchingConfigs[index], KSurfaceWidth, KSurfaceHeight, KWindowWidth, KWindowHeight, &capability) == EGL_FALSE);
            TEST(capability == -1); //make sure that value has not been modified
            ASSERT_EGL_ERROR(EGL_BAD_MATCH);
            }
        else  
            {
            if(config == -1) 
                {//memorize the first config that supports scaling
                config = matchingConfigs[index];
                }
            ASSERT_EGL_TRUE(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, matchingConfigs[index], KSurfaceWidth, KSurfaceHeight, KWindowWidth, KWindowHeight, &capability));
            TEST((capability == EGL_SUPPORTED_NOK) || (capability == EGL_SLOW_NOK));
            capability = -1; 
            }
        }
    capability = -1; 
    TEST(config != -1); // make sure that at least one config supports scaling 
    
    //invalid display
    TEST(iPfnEglQuerySurfaceScalingCapabilityNOK(KInvalidDisplay, config, KSurfaceWidth, KSurfaceHeight, KWindowWidth, KWindowHeight, &capability) == EGL_FALSE);
    TEST(capability == -1); //make sure that value has not been modified
    ASSERT_EGL_ERROR(EGL_BAD_DISPLAY);

    //negative width
    TEST(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, config, -KSurfaceWidth, KSurfaceHeight, KWindowWidth, KWindowHeight, &capability) == EGL_FALSE);
    TEST(capability == -1); //make sure that value has not been modified
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    TEST(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, config, KSurfaceWidth, KSurfaceHeight, -KWindowWidth, KWindowHeight, &capability) == EGL_FALSE);
    TEST(capability == -1); //make sure that value has not been modified
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    //negative height
    TEST(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, config, KSurfaceWidth, -KSurfaceHeight, KWindowWidth, KWindowHeight, &capability) == EGL_FALSE);
    TEST(capability == -1); //make sure that value has not been modified
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    TEST(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, config, KSurfaceWidth, KSurfaceHeight, KWindowWidth, -KWindowHeight, &capability) == EGL_FALSE);
    TEST(capability == -1); //make sure that value has not been modified
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    
    //zero width
    TEST(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, config, -0, KSurfaceHeight, KWindowWidth, KWindowHeight, &capability) == EGL_FALSE);
    TEST(capability == -1); //make sure that value has not been modified
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    TEST(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, config, KSurfaceWidth, KSurfaceHeight, -0, KWindowHeight, &capability) == EGL_FALSE);
    TEST(capability == -1); //make sure that value has not been modified
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    //zero height
    TEST(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, config, KSurfaceWidth, -0, KWindowWidth, KWindowHeight, &capability) == EGL_FALSE);
    TEST(capability == -1); //make sure that value has not been modified
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
    TEST(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, config, KSurfaceWidth, KSurfaceHeight, KWindowWidth, -0, &capability) == EGL_FALSE);
    TEST(capability == -1); //make sure that value has not been modified
    ASSERT_EGL_ERROR(EGL_BAD_PARAMETER);
   
    // Cleanup
    CloseWindow();
    CleanAll();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScalingCapability::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0664

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Call eglSetSurfaceScalingNOK with fixed size surface and a target extent that can be set. 

@SYMTestActions
Retrieve all available EGL configs and for those that support surface scaling 
1. Create a window surface with surface scaling attributes and make it current
2. Query these attributes and check they correspond with what is expected
3. Query if surface supports scaling to a different extent
4. Set the new extent and new offset to the surface
5. Query these attributes

@SYMTestExpectedResults
5. Surface scaling attributes have been updated    
*/
TVerdict CEglTest_SurfaceScalingSet::doTestStepL()
    {
    SetTestStepName(_L("GRAPHICS-EGL-0664"));
    SetTestStepID(_L("GRAPHICS-EGL-0664"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScalingSet::doTestStepL"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100, 100));

    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    const TInt KSurfaceWidth = KWindowWidth/2;
    const TInt KSurfaceHeight = KWindowHeight/2;
    const TInt KExtentWidth = KWindowWidth;
    const TInt KExtentHeight = KWindowHeight;
    const EGLint KOffsetX = 3;
    const EGLint KOffsetY = 7;

    EGLint attrib_list[] = {
                  EGL_FIXED_WIDTH_NOK, KSurfaceWidth,
                  EGL_FIXED_HEIGHT_NOK, KSurfaceHeight,
                  EGL_TARGET_EXTENT_OFFSET_X_NOK, KOffsetX,
                  EGL_TARGET_EXTENT_OFFSET_Y_NOK, KOffsetY,
                  EGL_TARGET_EXTENT_WIDTH_NOK, KExtentWidth,
                  EGL_TARGET_EXTENT_HEIGHT_NOK, KExtentHeight,
                  EGL_NONE};

    // Create display object
    GetDisplayL();
    CreateEglSessionL();
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[KMaxEglConfigs];
    EGLint numConfigs = 0;

    // Query total number of configs
    ASSERT_EGL_TRUE(eglGetConfigs(iDisplay, NULL, KMaxTInt, &numConfigs));
    TESTL(numConfigs >= 1 && numConfigs <= KMaxEglConfigs);
    INFO_PRINTF2(_L("Found %d configs"), numConfigs);

    // Get the configs
    ASSERT_EGL_TRUE(eglGetConfigs(iDisplay, matchingConfigs, KMaxEglConfigs, &numConfigs));

    // Check if surface scaling is supported in the config, if so, create surface
    for(TInt i=0; i<numConfigs; i++)
        {
		EGLint value = -32; // arbitraty value which is neither EGL_TRUE nor EGL_FALSE
		ASSERT_EGL_TRUE(eglGetConfigAttrib(iDisplay, matchingConfigs[i], EGL_SURFACE_SCALING_NOK, &value));
		if (value==EGL_TRUE)
			{
			INFO_PRINTF2(_L("Config %d supports surface scaling. Create a window surface..."), i);

		    // Create the window surface and the egl context and make them current
			EGLint renderableType = 0;
			ASSERT_EGL_TRUE(eglGetConfigAttrib(iDisplay, matchingConfigs[i],  EGL_RENDERABLE_TYPE, &renderableType));
			if (renderableType&EGL_OPENVG_BIT)
				{
				iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[i], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);
				}
			else if (renderableType&EGL_OPENGL_ES_BIT)
				{
				iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[i], iWindow, EFalse, EGL_OPENGL_ES_API, 1, attrib_list);
				}
			else
				{
				WARN_PRINTF2(_L("Config %d does not support either OPENVG or OPENGL_ES. Skip!"), i);
				continue;
				}

		    // Check values are as expected
	        CheckScalingAttributesL(KSurfaceWidth, KSurfaceHeight, KExtentWidth, KExtentHeight, KOffsetX, KOffsetY);

	        // modify target extent atttributes
	        // 1 - first check that this new extent is supported, should do as we're reducing it
	        EGLint capability = -1; 	//arbitrary number beyond the existing range 
	        EGLint newExtentWidth = KExtentWidth / 2;
	    	EGLint newExtentHeight = KExtentHeight / 2;
	        EGLint newOffsetX = KOffsetX * 2;
	    	EGLint newOffsetY = KOffsetY * 2;
	        ASSERT_EGL_TRUE(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, matchingConfigs[i], KSurfaceWidth, KSurfaceHeight, newExtentWidth, newExtentHeight, &capability));
            TEST((capability == EGL_SUPPORTED_NOK) || (capability == EGL_SLOW_NOK));
			// 2 - set new extent
	        ASSERT_EGL_TRUE(iPfnEglSetSurfaceScalingNOK(iDisplay, iEglSess->Surface(), newOffsetX, newOffsetY, newExtentWidth, newExtentHeight));

	        // Check attributes have changed
	        CheckScalingAttributesL(KSurfaceWidth, KSurfaceHeight, newExtentWidth, newExtentHeight, newOffsetX, newOffsetY);

			// Cleanup
	        iEglSess->CleanupSurfaceAndContextL();
			}
        }
    
    // Cleanup
    CloseWindow();
    CleanAll();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScalingSet::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0665

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Negative testing. Call eglSetSurfaceScalingNOK with invalid parameters 

@SYMTestActions
1. Set surface scaling with invalid display.
2. Set surface scaling with negative target width.
3. Set surface scaling with negative target height.
3. Set surface scaling with zero target width.
3. Set surface scaling with zero target height.
@SYMTestExpectedResults
1. Set surface scaling will fail with error code EGL_BAD_DISPLAY.   
2. Set surface scaling will fail with error code EGL_BAD_PARAMETER.   
3. Set surface scaling will fail with error code EGL_BAD_PARAMETER.   
4. Set surface scaling will fail with error code EGL_BAD_PARAMETER.   
5. Set surface scaling will fail with error code EGL_BAD_PARAMETER.   
*/
TVerdict CEglTest_SurfaceScalingSetInvalidAttributes::doTestStepL()
    {
    SetTestStepName(_L("GRAPHICS-EGL-0665"));
    SetTestStepID(_L("GRAPHICS-EGL-0665"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScalingSetInvalidAttributes::doTestStepL"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100, 100));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL(numConfigs == 1); // Abort the test if the EGL config is not supported

    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    const TInt KSurfaceWidth = KWindowWidth/2;
    const TInt KSurfaceHeight = KWindowHeight/2;
    const TInt KExtentWidth = KWindowWidth;
    const TInt KExtentHeight = KWindowHeight;
    const EGLint KOffsetX = 11;
    const EGLint KOffsetY = 13;
    EGLint attrib_list[] = {
                  EGL_FIXED_WIDTH_NOK, KSurfaceWidth,
                  EGL_FIXED_HEIGHT_NOK, KSurfaceHeight,
                  EGL_TARGET_EXTENT_OFFSET_X_NOK, KOffsetX,
                  EGL_TARGET_EXTENT_OFFSET_Y_NOK, KOffsetY,
                  EGL_TARGET_EXTENT_WIDTH_NOK, KExtentWidth,
                  EGL_TARGET_EXTENT_HEIGHT_NOK, KExtentHeight,
                  EGL_NONE};

    // Create the window surface and the egl context and make them current
    iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    

    // Check all attributes
    CheckScalingAttributesL(KSurfaceWidth, KSurfaceHeight, KExtentWidth, KExtentHeight, KOffsetX, KOffsetY);

    for(TInt index = 0; index < 5; index++)
        {
		// new attribute values
		EGLDisplay newDisplay = iDisplay;
		EGLint newExtentWidth = KExtentWidth;
		EGLint newExtentHeight = KExtentHeight;
		EGLint newOffsetX = KOffsetX;
		EGLint newOffsetY = KOffsetY;
		// expected error value
		EGLint error = EGL_BAD_PARAMETER;
		
		switch(index)
            {
        case 0:
            // Invalid display - valid extent
            newDisplay = newDisplay - 100;
            error = EGL_BAD_DISPLAY;
            break;
        case 1:
            // Valid display - Negative extent width
        	newExtentWidth = -newExtentHeight;
            break;
        case 2:
            // Valid display - Negative extent height
        	newExtentHeight = -newExtentHeight;
        	break;
		case 3:
			// Valid display - zero extent width
			newExtentWidth = 0;
			break;
		case 4:
			// Valid display - zero extent height
			newExtentHeight = 0;
			break;
			}

		TEST(EGL_FALSE == iPfnEglSetSurfaceScalingNOK(newDisplay, iEglSess->Surface(), newOffsetX, newOffsetY, newExtentWidth, newExtentHeight));
		ASSERT_EGL_ERROR(error);

		// attributes haven't changed
	    CheckScalingAttributesL(KSurfaceWidth, KSurfaceHeight, KWindowWidth, KWindowHeight, KOffsetX, KOffsetY);
        }
    
    // Cleanup
    CleanAll();
    CloseWindow();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScalingSetInvalidAttributes::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0666

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Negative testing. Query scaling attributes in non-fixed size surfaces 

@SYMTestActions
1. Create a window surface from a non-fixed size surface
2. Set surface scaling with valid target extent.
3. Query scaling attributes
4. Repeat step 2-3 for EGL window surface, pixmap and pbuffer surface

@SYMTestExpectedResults
2. Set surface scaling will fail with error code EGL_BAD_MATCH.
3. Query scaling attributes does not fail, but values not updated
*/
TVerdict CEglTest_SurfaceScalingSetNonFixed::doTestStepL()
    {
    SetTestStepName(_L("GRAPHICS-EGL-0666"));
    SetTestStepID(_L("GRAPHICS-EGL-0666"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScalingSetNonFixed::doTestStepL"));

    // Establish the connection to the window server and create
    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100, 100));

    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    const TInt KSurfaceWidth = KWindowWidth;
    const TInt KSurfaceHeight = KWindowHeight;

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    // Choose EGL config - Make sure it DOES NOT support surface scaling
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    
    // If all window surfaces in EGL are scalable, first case must be skipped. Others still valid
    TInt index = iAllScalable ? 1 : 0;
    for(; index < 3; index++)
        {
        switch(index)
            {
        case 0:
            // Create the non-fixed size window surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the non-fixed size window surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribs_NoScaling], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow);    
            break;
        case 1:
            // Create the pbuffer surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the pbuffer surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribs[EPBufferAttribsColor16MU], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreatePbufferSurfaceAndMakeCurrentL(matchingConfigs[0], TSize(KSurfaceWidth, KSurfaceHeight));
            break;
        case 2:
            // Create the pixmap surface and the egl context and make them current
            INFO_PRINTF1(_L("Create the pixmap surface and the egl context and make them current..."));
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribs[EPixmapAttribsColor16MU], matchingConfigs, 1, &numConfigs));
            TESTL(numConfigs == 1);
            iEglSess->CreatePixmapSurfaceAndMakeCurrentAndMatchL(TSize(KSurfaceWidth, KSurfaceHeight), EColor16MU);
            break;
            }

		// Check all attributes (this is a non-fixed size surface)  - 
		// Note that we cannot use CheckScalingAttributesL because values are not updated
		EGLint surfaceWidth = 0;
		EGLint surfaceHeight = 0;
		EGLint extentWidth = -1;	
		EGLint extentHeight = -2;
		EGLint offsetX = -3;
		EGLint offsetY = -4;
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_WIDTH, &surfaceWidth));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_HEIGHT, &surfaceHeight));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_WIDTH_NOK, &extentWidth));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_HEIGHT_NOK, &extentHeight));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_X_NOK, &offsetX));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_Y_NOK, &offsetY));
		TEST((surfaceWidth == KWindowWidth) && (surfaceHeight == KWindowHeight));
		// following values should not be modified (non fixed size window)
		TEST((extentWidth == -1) && (extentHeight == -2));
		TEST((offsetX == -3) && (offsetY == -4));
	
		// new attribute values
		EGLint newExtentWidth = extentWidth / 2;
		EGLint newExtentHeight = extentHeight / 2;
		EGLint newOffsetX = offsetX * 2;
		EGLint newOffsetY = offsetY * 2;
	
		// Valid parameters - But non fixed size surface
		TEST(iPfnEglSetSurfaceScalingNOK(iDisplay, iEglSess->Surface(), newOffsetX, newOffsetY, newExtentWidth, newExtentHeight) == EGL_FALSE);
		ASSERT_EGL_ERROR(EGL_BAD_MATCH);
	
		// attributes haven't changed
		// Note that we cannot use CheckScalingAttributesL because values are not updated
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_WIDTH, &surfaceWidth));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_HEIGHT, &surfaceHeight));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_WIDTH_NOK, &extentWidth));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_HEIGHT_NOK, &extentHeight));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_X_NOK, &offsetX));
		ASSERT_EGL_TRUE(eglQuerySurface(iDisplay, iEglSess->Surface(), EGL_TARGET_EXTENT_OFFSET_Y_NOK, &offsetY));
		TEST((surfaceWidth == KWindowWidth) && (surfaceHeight == KWindowHeight));
		TEST((extentWidth == -1) && (extentHeight == -2));
		TEST((offsetX == -3) && (offsetY == -4));
	
    	// destroy surface and context
        iEglSess->CleanupSurfaceAndContextL();
        }

    // Cleanup
    CleanAll();
    CloseWindow();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScalingSetNonFixed::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0667

@SYMTestPriority 1

@SYMPREQ 2676

@SYMREQ 417-56592

@SYMTestCaseDesc
Negative testing. Calling extensions without initialising EGL

@SYMTestActions
Without initialising EGL
1. Call query surface scaling capability
2. Call set surface scaling

@SYMTestExpectedResults
1. Query scaling capability will fail with error code EGL_NOT_INITIALIZED.
2. Set surface scaling will fail with error code EGL_NOT_INITIALIZED.
*/
TVerdict CEglTest_SurfaceScalingNotInitialized::doTestStepL()
    {
    SetTestStepName(_L("GRAPHICS-EGL-0667"));
    SetTestStepID(_L("GRAPHICS-EGL-0667"));
    INFO_PRINTF1(_L("CEglTest_SurfaceScalingNotInitialized::doTestStepL"));

    // a WindowGroup and a Window object
    CreateAndActivateWindowL(TSize(100, 100));

    // Create display object
    GetDisplayL();
    CreateEglSessionL();    // initialise EGL Session so we can make use of its helper objects
    iEglSess->InitializeL();

    // Choose EGL config
    EGLConfig matchingConfigs[1];
    EGLint numConfigs = 0;
    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KScalingConfigAttribs[EWindowAttribsColor16MU_Scaling], matchingConfigs, 1, &numConfigs));
    TESTL(numConfigs == 1); // Abort the test if the EGL config is not supported

    const TInt KWindowWidth = iWindow.Size().iWidth;
    const TInt KWindowHeight = iWindow.Size().iHeight;
    const TInt KSurfaceWidth = KWindowWidth/2;
    const TInt KSurfaceHeight = KWindowHeight/2;
    const TInt KExtentWidth = KWindowWidth;
    const TInt KExtentHeight = KWindowHeight;
    const EGLint KOffsetX = 11;
    const EGLint KOffsetY = 13;
    EGLint attrib_list[] = {
                  EGL_FIXED_WIDTH_NOK, KSurfaceWidth,
                  EGL_FIXED_HEIGHT_NOK, KSurfaceHeight,
                  EGL_TARGET_EXTENT_OFFSET_X_NOK, KOffsetX,
                  EGL_TARGET_EXTENT_OFFSET_Y_NOK, KOffsetY,
                  EGL_TARGET_EXTENT_WIDTH_NOK, KExtentWidth,
                  EGL_TARGET_EXTENT_HEIGHT_NOK, KExtentHeight,
                  EGL_NONE};

    // Create the window surface and the egl context and make them current
    iEglSess->CreateWindowSurfaceAndMakeCurrentL(matchingConfigs[0], iWindow, EFalse, EGL_OPENVG_API, 1, attrib_list);    

    // First set new offset attributes (fairly simple, so should be supported)
    INFO_PRINTF1(_L("Set new offset attributes - should succeed..."));
    EGLint capability = -1; 	//arbitrary number beyond the existing range 
    ASSERT_EGL_TRUE(iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, matchingConfigs[0], KSurfaceWidth, KSurfaceHeight, KExtentWidth, KExtentHeight, &capability));
    TEST((capability == EGL_SUPPORTED_NOK) || (capability == EGL_SLOW_NOK));
    ASSERT_EGL_TRUE(iPfnEglSetSurfaceScalingNOK(iDisplay, iEglSess->Surface(), KOffsetX+1, KOffsetY+1, KExtentWidth, KExtentHeight));
    
    // Terminate display and try again
    INFO_PRINTF1(_L("Terminate display and try again, should fail now..."));
    eglTerminate(iDisplay);

    capability = -1; 	//arbitrary number beyond the existing range 
    TEST(EGL_FALSE == iPfnEglQuerySurfaceScalingCapabilityNOK(iDisplay, matchingConfigs[0], KSurfaceWidth, KSurfaceHeight, KExtentWidth, KExtentHeight, &capability));
    TEST(capability == -1); //make sure that value has not been modified
    ASSERT_EGL_ERROR(EGL_NOT_INITIALIZED);
    
    TEST(EGL_FALSE == iPfnEglSetSurfaceScalingNOK(iDisplay, iEglSess->Surface(), KOffsetX, KOffsetY, KExtentWidth, KExtentHeight));
    ASSERT_EGL_ERROR(EGL_NOT_INITIALIZED);
    
    // Cleanup
    CleanAll();
    CloseWindow();
    
    INFO_PRINTF1(_L("....CEglTest_SurfaceScalingNotInitialized::doTestStepL completed!"));
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

