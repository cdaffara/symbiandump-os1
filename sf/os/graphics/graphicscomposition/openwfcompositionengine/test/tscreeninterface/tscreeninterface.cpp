// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
//
// Description:
// Implementation of test class for OpenWF-C RI display driver interface 
//

#include <WF/openwfc_ri_display.h>
#include <WF/openwfc_ri_display_update.h>
#include <test/extendtef.h>
#include <test/tefunit.h>
#include <hal.h>
#include "tscreeninterface.h"

#define KCompositorVersion 0x01023456
#define KRgbRed         TRgb(0x0000ff)
#define KRgbGreen       TRgb(0x00ff00)
#define KRgbYellow      TRgb(0x00ffff)
#define KRgbBlue        TRgb(0xff0000)

const TInt KDefaultScreenNumber = 0;
const TTimeIntervalMicroSeconds32 KDelay = 1000000;  //delay in microseconds
const TTimeIntervalMicroSeconds32 KMinimalDelay = 1000;  //delay in microseconds

_LIT(KImageSectionName, "ImageInfo");
_LIT(KContiguousFlag, "ContiguousFlag");
_LIT(KNonFastpathablePixelFormat, "NonFastpathablePixelFormat");
_LIT(KFastpathablePixelFormat, "FastpathablePixelFormat");

/*
 * CTestScreenInterface implementation
 */

CTestScreenInterface::CTestScreenInterface()
:iContiguous(EFlagNotListed),
 iFastpathablePixelFormat(EUidPixelFormatARGB_8888_PRE),
 iNonFastpathablePixelFormat(EUidPixelFormatRGB_565),
 iMappable(ETrue),
 iMaxBuffers(2),
 iMinBuffers(1),
 iCacheAttrib(ECacheNotlisted)
	{
	// No implementation required
	}

CTestScreenInterface::~CTestScreenInterface()
	{
    TearDown();
	}

void CTestScreenInterface::SetupL()
	{
	iUtility = CStreamUtility::NewL();
	iRed.SetInternal(0xFFFF0000);
	iGreen.SetInternal(0xFF00FF00);
	iBlue.SetInternal(0xFF0000FF);
	ReadIniData();
    User::After(KDelay); // consider reading delay from ini file
	}

/**
 * test Suite furniture 
 **/
void CTestScreenInterface::TearDownL()
	{
    // Nothing leaves in this function, 
    // but TearDownL() is needed for the CTestFixture (TEF) interface
    TearDown();
	}

void CTestScreenInterface::TearDown()
    {
    if(iUtility)
        {
        delete iUtility;
        iUtility = NULL;
        }
    }

// Create a suite of all the tests
CTestSuite* CTestScreenInterface::CreateSuiteL(const TDesC& aName)
	{
	SymbianStreamRegisterScreenNotifications(0, 10, KCompositorVersion);
	SUB_SUITE_OPT(CTestScreenInterface,NULL);

		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_SCREENINTERFACE_0100L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_SCREENINTERFACE_0101L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_SCREENINTERFACE_0102L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_SCREENINTERFACE_0103L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_SCREENINTERFACE_0104L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_SCREENINTERFACE_0105L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_SCREENINTERFACE_0106L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_SCREENINTERFACE_0107L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_SCREENINTERFACE_0108L);		
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_SCREENINTERFACE_0109L);

        END_SUITE;	
	
	}

/// This handles any non-member uses of the extended ASSERT_XXX macros
void TefUnitFailLeaveL()
	{
	User::Leave(KErrTEFUnitFail);
	}

void CTestScreenInterface::ReadIniData()
    {
    TBool contiguousFlag;
    TBool returnValue = iConfig.GetBool(KImageSectionName, KContiguousFlag, contiguousFlag);
    if (returnValue)
        {
        if(contiguousFlag)
            {
            iContiguous = EContiguous;
            }
        else
            {
            iContiguous = ENonContiguous;
            }
        }
    
    TInt fastpathablePixelFormat;
    returnValue = iConfig.GetHex(KImageSectionName, KFastpathablePixelFormat, fastpathablePixelFormat);
    if(returnValue)
        {
        iFastpathablePixelFormat = static_cast<TUidPixelFormat>(fastpathablePixelFormat);
        }
    
    TInt nonFastpathablePixelFormat;
    returnValue = iConfig.GetHex(KImageSectionName, KNonFastpathablePixelFormat, nonFastpathablePixelFormat);
    if(returnValue)
        {
        iNonFastpathablePixelFormat = static_cast<TUidPixelFormat>(nonFastpathablePixelFormat);
        }
    }

TInt CTestScreenInterface::BaseTestContiguousFlag(TBool aContiguous)
     {
      COpenWFC_RI_Display* screenContext = COpenWFC_RI_Display::NewL(KDefaultScreenNumber);
      CleanupStack::PushL(screenContext);
      
      TUint32 screenRotation = COpenWFC_RI_Display::EScreenRotate0;
      screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                  &screenRotation,
                                  sizeof(screenRotation));
      screenContext->CommitAttribute();

      COpenWFC_RI_Display::TScreenGeometryAttribute attributes;
      screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry, &attributes, 
                                  sizeof(COpenWFC_RI_Display::TScreenGeometryAttribute));
      
      TSurfaceId surface = TSurfaceId::CreateNullId();
      SymbianStreamType ns=iUtility->CreateStreamL(TSize(attributes.iNormalWidth,attributes.iNormalHeight), 
                                                  iFastpathablePixelFormat, attributes.iNormalStride, 
                                                  surface, aContiguous, 1);
      ASSERT_FALSE((*ns).IsNull());

      iUtility->FillStreamL(ns, KRgbGreen);
      
      TInt err = screenContext->SetTopLayerSurface(ns);
      Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrAll, _L("SetTopLayerSurface() returned %i"),err );
	  RDebug::Printf("SetTopLayerSurface() returned %i",err);
      
	  if (err==KErrNone)
	      {    //Must successfully SetTopLayerSurface before calling UpdateDisplay. 
          screenContext->UpdateDisplay();
	      }

      CleanupStack::PopAndDestroy(screenContext);
      
      return err;
     }
     
/**
@SYMTestCaseID			GRAPHICS_OPENWFC_SCREENINTERFACE_0100
@SYMTestCaseDesc		Create a screen size stream and display on screen
@SYMREQ					
@SYMPREQ				REQ 417-54885
@SYMTestType            Unit Test			
@SYMTestPriority        High		
@SYMTestPurpose			Verify a COpenWFC_RI_Display object can be properly created and a stream can be displayed on screen 
@SYMTestActions	        
                        Create a screen context
                        Set display attributes 
                        Create a screen size stream based on the display attributes with contigous and non-contiguous flags
                        Fill the stream
                        Associates the stream with the topmost display layer
                        Update the screen
@SYMTestExpectedResults 
                        If contiguous flag is set in the ini file, and the stream is created using the flag, 
                        the stream should be properly displayed on screen. The stream created using the opposit flag from ini value should fail.
                        If contiguous flag is not set in the ini file, 
                        the stream should be properly displayed on screen for contiguous and non-contiguous flag.
 **/		
void CTestScreenInterface::GRAPHICS_OPENWFC_SCREENINTERFACE_0100L()
	{
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_SCREENINTERFACE_0100L()"));
    LOG((" *** CTestScreenInterface test 0100L"));
 
    if(iContiguous != EFlagNotListed)
        {

        // We expect the opposit flag to the one from the ini file to fail
        if(iContiguous == EContiguous)
            {
            ASSERT_TRUE(KErrNotSupported == BaseTestContiguousFlag(ENonContiguous));       
            }
        else
            {
            ASSERT_TRUE(KErrNotSupported == BaseTestContiguousFlag(EContiguous));       
            }
        // The contiguous flag from the ini file should pass
        ASSERT_TRUE(KErrNone == BaseTestContiguousFlag(iContiguous));
        // The last run here should be the "good" run so that it can do a Display() call.
        }
    else
        {
        // Contiguous or Non-contiguous flags should both pass if nothing set in the ini file
        ASSERT_TRUE(KErrNone == BaseTestContiguousFlag(EContiguous));       
        ASSERT_TRUE(KErrNone == BaseTestContiguousFlag(ENonContiguous));           
        }
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_SCREENINTERFACE_0100L() done"));
    LOG((" *** CTestScreenInterface test 0100L done"));
	}

/**
@SYMTestCaseID			GRAPHICS_OPENWFC_SCREENINTERFACE_0101
@SYMTestCaseDesc		Create a stream, display on screen, then rotate the screen involving scaling
@SYMREQ					
@SYMPREQ				REQ 417-54885
@SYMTestType            Unit Test			
@SYMTestPriority        Medium		
@SYMTestPurpose			Verify screen rotation works  
@SYMTestActions	
                        Create a screen context
                        Set display attributes 
                        Create a screen size stream 
                        Fill the stream
                        Associates the stream with the topmost display layer
                        Update the screen
                        Rotate the screen 90 degrees, 180 degrees, 270 degrees
                        Update the screen respectively
@SYMTestExpectedResults
                        Methods should complete without error.
                        The stream should be properly displayed on screen
 **/
void CTestScreenInterface::GRAPHICS_OPENWFC_SCREENINTERFACE_0101L()
	{
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_SCREENINTERFACE_0101L()"));
    LOG((" *** CTestScreenInterface test 0101L"));
 
    COpenWFC_RI_Display* screenContext = COpenWFC_RI_Display::NewL(KDefaultScreenNumber);
    CleanupStack::PushL(screenContext);
    
    COpenWFC_RI_Display::TScreenGeometryAttribute attributes;
    screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry, &attributes, 
                                sizeof(COpenWFC_RI_Display::TScreenGeometryAttribute));

    if (attributes.iSupportedRotations > 1)
        {    
        INFO_PRINTF2(_L("Number of supported rotations: %i "), attributes.iSupportedRotations);
        TUint32 screenRotation0 = COpenWFC_RI_Display::EScreenRotate0;
        screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                &screenRotation0,
                                sizeof(screenRotation0));
        screenContext->CommitAttribute();

        TSurfaceId surface = TSurfaceId::CreateNullId();
        SymbianStreamType ns=iUtility->CreateStreamL(TSize(attributes.iNormalWidth,attributes.iNormalHeight), 
                                                iFastpathablePixelFormat, attributes.iNormalStride, 
                                                surface, iContiguous, 1);
    
        ASSERT_FALSE((*ns).IsNull());
        iUtility->FillStreamL(ns, KRgbRed);
    
        TInt err = screenContext->SetTopLayerSurface(ns);
        ASSERT_TRUE(err == KErrNone);

        err = screenContext->UpdateDisplay();   
        ASSERT_TRUE(err == KErrNone);

        INFO_PRINTF1(_L("Rotate screen 90 degree"));
        // Rotate screen 90 degree and test
        TUint32 screenRotation1 = COpenWFC_RI_Display::EScreenRotate90;
        screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                &screenRotation1,
                                sizeof(screenRotation1));
        screenContext->CommitAttribute();

        err = screenContext->UpdateDisplay();   
        ASSERT_TRUE(err == KErrNone);    

        if (attributes.iSupportedRotations > 2)
            {
            INFO_PRINTF1(_L("Rotate screen 180 degree"));
            // Rotate screen 180 degree and test
            TUint32 screenRotation2 = COpenWFC_RI_Display::EScreenRotate180;
            screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                    &screenRotation2,
                                    sizeof(screenRotation2));
            screenContext->CommitAttribute();

            err = screenContext->UpdateDisplay();   
            ASSERT_TRUE(err == KErrNone);
            }

        if (attributes.iSupportedRotations > 3)
            {
            INFO_PRINTF1(_L("Rotate screen 270 degree"));
            // Rotate screen 270 degree and test
            TUint32 screenRotation3 = COpenWFC_RI_Display::EScreenRotate270;
            screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                    &screenRotation3,
                                    sizeof(screenRotation3));
            screenContext->CommitAttribute();

            err = screenContext->UpdateDisplay();   
            ASSERT_TRUE(err == KErrNone);
            }
        
        // Set screen back to rotation 0 
        screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                    &screenRotation0,
                                    sizeof(screenRotation0));
        screenContext->CommitAttribute();
        err = screenContext->UpdateDisplay();   
        ASSERT_TRUE(err == KErrNone);    
        }
    else
        {
        INFO_PRINTF1(_L("Screen rotation is not supported. Test skipped."));
        }

    CleanupStack::PopAndDestroy(screenContext);
	}

/**
@SYMTestCaseID			GRAPHICS_OPENWFC_SCREENINTERFACE_0102
@SYMTestCaseDesc        Create a stream, display on screen, then rotate the screen, not involving scaling
@SYMREQ                 
@SYMPREQ                REQ 417-54885
@SYMTestType            Unit Test           
@SYMTestPriority        Medium      
@SYMTestPurpose         Verify screen rotation works  
@SYMTestActions 
                        Create a screen context
                        Set display attributes 
                        Create a stream of the size (screenHeight, screenWidth)
                        Fill the stream
                        Associates the stream with the topmost display layer
                        Update the screen
                        Rotate the screen 90 degrees
                        Update the screen
@SYMTestExpectedResults
                        Methods should complete without error.
                        The stream should be properly displayed on screen
 **/		
void CTestScreenInterface::GRAPHICS_OPENWFC_SCREENINTERFACE_0102L()
	{
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_SCREENINTERFACE_0102L()"));
    LOG((" *** CTestScreenInterface test 0102L"));
 
    COpenWFC_RI_Display* screenContext = COpenWFC_RI_Display::NewL(KDefaultScreenNumber);
    CleanupStack::PushL(screenContext);
    
    COpenWFC_RI_Display::TScreenGeometryAttribute attributes;
    screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry, &attributes, 
                                sizeof(COpenWFC_RI_Display::TScreenGeometryAttribute));

    if (attributes.iSupportedRotations > 1)
        {    
        INFO_PRINTF2(_L("Number of supported rotations: %i "), attributes.iSupportedRotations);
        TUint32 screenRotation0 = COpenWFC_RI_Display::EScreenRotate0;
        screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                &screenRotation0,
                                sizeof(screenRotation0));
        screenContext->CommitAttribute();

        TSurfaceId surface = TSurfaceId::CreateNullId();
        SymbianStreamType ns=iUtility->CreateStreamL(TSize(attributes.iNormalHeight,attributes.iNormalWidth), 
                                                iFastpathablePixelFormat, attributes.iNormalStride, 
                                                surface, iContiguous, 1);
    
        ASSERT_FALSE((*ns).IsNull());
        iUtility->FillStreamL(ns, KRgbRed);
    
        TInt err = screenContext->SetTopLayerSurface(ns);
        ASSERT_TRUE(err == KErrNone);

        err = screenContext->UpdateDisplay();   
        ASSERT_TRUE(err == KErrNone);

        INFO_PRINTF1(_L("Rotate screen 90 degree"));
        // Rotate screen 90 degree and test
        TUint32 screenRotation1 = COpenWFC_RI_Display::EScreenRotate90;
        screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                &screenRotation1,
                                sizeof(screenRotation1));
        screenContext->CommitAttribute();

        err = screenContext->UpdateDisplay();   
        ASSERT_TRUE(err == KErrNone);    
        }
    else
        {
        INFO_PRINTF1(_L("Screen rotation is not supported. Test skipped."));
        }

    CleanupStack::PopAndDestroy(screenContext);
	}

/**
@SYMTestCaseID			GRAPHICS_OPENWFC_SCREENINTERFACE_0103
@SYMTestCaseDesc		Create a stream, fill the stream several times and display on screen
@SYMREQ					
@SYMPREQ                REQ 417-54885
@SYMTestType            Unit Test			
@SYMTestPriority        High				
@SYMTestPurpose	        Check displaying stream repeatedly	
@SYMTestActions	
                        Create a screen context
                        Set display attributes 
                        Create a screen size stream 
                        Fill the stream 
                        Associates the stream with the topmost display layer
                        Update the screen
                        Repeat filling the stream, associate with toplayer and update several times
                        Destory the screen context                       
@SYMTestExpectedResults
                        Methods should complete without error.
                        Each time, the stream should be properly displayed on screen
 **/
void CTestScreenInterface::GRAPHICS_OPENWFC_SCREENINTERFACE_0103L()
	{
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_SCREENINTERFACE_0103L()"));
    LOG((" *** CTestScreenInterface test 0103L"));
 
    COpenWFC_RI_Display* screenContext = COpenWFC_RI_Display::NewL(KDefaultScreenNumber);
    CleanupStack::PushL(screenContext);
    
    TUint32 screenRotation = COpenWFC_RI_Display::EScreenRotate0;
    screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                &screenRotation,
                                sizeof(screenRotation));
    screenContext->CommitAttribute();

    COpenWFC_RI_Display::TScreenGeometryAttribute attributes;
    screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry, &attributes, 
                                sizeof(COpenWFC_RI_Display::TScreenGeometryAttribute));
    
    TSurfaceId surface = TSurfaceId::CreateNullId();
    SymbianStreamType ns=iUtility->CreateStreamL(TSize(attributes.iNormalWidth,attributes.iNormalHeight), 
                                                iFastpathablePixelFormat, attributes.iNormalStride, 
                                                surface, iContiguous, 1);
    ASSERT_FALSE((*ns).IsNull());

    iUtility->FillStreamL(ns, KRgbYellow);    
    TInt err = screenContext->SetTopLayerSurface(ns);
    ASSERT_TRUE(err == KErrNone);    
    err = screenContext->UpdateDisplay();    
    ASSERT_TRUE(err == KErrNone);
    User::After(KDelay);

    iUtility->FillStreamL(ns, KRgbRed);    
    err = screenContext->SetTopLayerSurface(ns);
    ASSERT_TRUE(err == KErrNone);    
    err = screenContext->UpdateDisplay();    
    ASSERT_TRUE(err == KErrNone);
    User::After(KDelay);
    
    iUtility->FillStreamL(ns, KRgbGreen);    
    err = screenContext->SetTopLayerSurface(ns);
    ASSERT_TRUE(err == KErrNone);    
    err = screenContext->UpdateDisplay();    
    ASSERT_TRUE(err == KErrNone);
    User::After(KDelay);
    
    iUtility->FillStreamL(ns, KRgbYellow);    
    err = screenContext->SetTopLayerSurface(ns);
    ASSERT_TRUE(err == KErrNone);    
    err = screenContext->UpdateDisplay();    
    ASSERT_TRUE(err == KErrNone);
    User::After(KDelay);

    iUtility->FillStreamL(ns, KRgbRed);    
    err = screenContext->SetTopLayerSurface(ns);
    ASSERT_TRUE(err == KErrNone);    
    err = screenContext->UpdateDisplay();    
    ASSERT_TRUE(err == KErrNone);
    User::After(KDelay);

    // Set a non-topmost layer
    err = screenContext->SetLayerSurface(1, ns);
    ASSERT_TRUE(err == KErrArgument);    
    
    CleanupStack::PopAndDestroy(screenContext);    
	}

/**
@SYMTestCaseID          GRAPHICS_OPENWFC_SCREENINTERFACE_0104
@SYMTestCaseDesc        Display all display attributes
@SYMREQ                 
@SYMPREQ                REQ 417-54885
@SYMTestType            Unit Test
@SYMTestPriority        High
@SYMTestPurpose         Check display attributes 
@SYMTestActions 
                        Create a screen context
                        Set display attributes 
                        Get all display attribtues
                        Destory the screen context                       
@SYMTestExpectedResults
                        Methods should complete without error.
                        Display attributes should be correct
 **/
void CTestScreenInterface::GRAPHICS_OPENWFC_SCREENINTERFACE_0104L()
    {
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_SCREENINTERFACE_0104L()"));
    LOG((" *** CTestScreenInterface test 0104L"));
 
    COpenWFC_RI_Display* screenContext = COpenWFC_RI_Display::NewL(KDefaultScreenNumber);
    CleanupStack::PushL(screenContext);
    
    COpenWFC_RI_Display::TScreenGeometryAttribute attributes;
    screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry, &attributes, 
                                sizeof(COpenWFC_RI_Display::TScreenGeometryAttribute));
 
    INFO_PRINTF1(_L("Printing attribute values: "));
    INFO_PRINTF2(_L("EScreenAttributeSupportedRotation: %i "), attributes.iSupportedRotations);
    INFO_PRINTF2(_L("EScreenAttributeCurrentRotation: 0x%x "), attributes.iCurrentRotation);
    INFO_PRINTF2(_L("EScreenAttributeDefaultRotation: 0x%x "), attributes.iDefaultRotation);
    INFO_PRINTF2(_L("EScreenAttributeBytesPerPixel: %i "), attributes.iBytesPerPixel);
    INFO_PRINTF2(_L("EScreenAttributeFlippedHeight: %i "), attributes.iFlippedHeight);
    INFO_PRINTF2(_L("EScreenAttributeFlippedWidth: %i "), attributes.iFlippedWidth);
    INFO_PRINTF2(_L("EScreenAttributeFlippedStride: %i "), attributes.iFlippedStride);
    INFO_PRINTF2(_L("EScreenAttributeNormalHeight: %i "), attributes.iNormalHeight);
    INFO_PRINTF2(_L("EScreenAttributeNormalWidth: %i "), attributes.iNormalWidth);
    INFO_PRINTF2(_L("EScreenAttributeNormalStride: %i "), attributes.iNormalStride);
    INFO_PRINTF2(_L("EScreenAttributePixelFormat: 0x%x "), attributes.iPixelFormat);
    
    CleanupStack::PopAndDestroy(screenContext);    
    }

/**
@SYMTestCaseID          GRAPHICS_OPENWFC_SCREENINTERFACE_0105
@SYMTestCaseDesc        Create a screen size stream of non-fastpathable pixel format and display on screen
@SYMREQ                 
@SYMPREQ                REQ 417-54885
@SYMTestType            Unit Test - Negative Test        
@SYMTestPriority        High        
@SYMTestPurpose         Verify a COpenWFC_RI_Display object can be properly created and a stream can be displayed on screen 
@SYMTestActions         
                        Create a screen context
                        Set display attributes 
                        Create a screen size stream based on the display attributes with a non-fastpathable pixelformat
                        Fill the stream
                        Associate the stream with the topmost display layer
@SYMTestExpectedResults 
                        Test should pass.
                        SetTopLayerSurface should return KErrNotSupported.
 **/        
void CTestScreenInterface::GRAPHICS_OPENWFC_SCREENINTERFACE_0105L()
    {
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_SCREENINTERFACE_0105L()"));
    LOG((" *** CTestScreenInterface test 0105L"));
    
    if (!iNonFastpathablePixelFormat)
        {
        INFO_PRINTF1(_L("No non-fastpathable format specified - negative test cannot be run."));
        User::Leave(KErrTEFUnitInconclusive);
        }
    COpenWFC_RI_Display* screenContext = COpenWFC_RI_Display::NewL(KDefaultScreenNumber);
    CleanupStack::PushL(screenContext);
     
    TUint32 screenRotation = COpenWFC_RI_Display::EScreenRotate0;
    screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                 &screenRotation,
                                 sizeof(screenRotation));
    screenContext->CommitAttribute();

    COpenWFC_RI_Display::TScreenGeometryAttribute attributes;
    screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry, &attributes, 
                                 sizeof(COpenWFC_RI_Display::TScreenGeometryAttribute));
     
    TSurfaceId surface = TSurfaceId::CreateNullId();
    SymbianStreamType ns=iUtility->CreateStreamL(TSize(attributes.iNormalWidth,attributes.iNormalHeight), 
                                                 iNonFastpathablePixelFormat, attributes.iNormalStride, 
                                                 surface, iContiguous, 1);
    ASSERT_FALSE((*ns).IsNull());

    iUtility->FillStreamL(ns, KRgbGreen);
     
    ASSERT_TRUE(KErrNotSupported == screenContext->SetTopLayerSurface(ns));
     
    CleanupStack::PopAndDestroy(screenContext);
    }

/**
@SYMTestCaseID          GRAPHICS_OPENWFC_SCREENINTERFACE_0106
@SYMTestCaseDesc        Create a stream, display on screen and repeat for a number of times
@SYMREQ                 
@SYMPREQ                REQ 417-54885
@SYMTestType            Unit Test           
@SYMTestPriority        High        
@SYMTestPurpose         Check stream display repeatedly
@SYMTestActions 
                        Create a screen context
                        Set display attributes 
                        Create a stream 
                        Fill the stream
                        Associates the stream with the topmost display layer
                        Update the screen
                        Destory the screen context
                        Repeat the above actions for a number of times in a for loop
@SYMTestExpectedResults
                        Methods should complete without error.
                        Each time, the stream should be properly displayed on screen
 **/        
void CTestScreenInterface::GRAPHICS_OPENWFC_SCREENINTERFACE_0106L()
    {
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_SCREENINTERFACE_0106L()"));
    LOG((" *** CTestScreenInterface test 0106L"));
 
    SymbianStreamType ns = NULL;
    const TInt loopMaxNum = 10;
    
    for (TInt i = 0; i < loopMaxNum; ++i)
        {
        //INFO_PRINTF2(_L("Create screen context and stream loop: %i"), i);

        COpenWFC_RI_Display* screenContext = COpenWFC_RI_Display::NewL(KDefaultScreenNumber);
        CleanupStack::PushL(screenContext);
    
        TUint32 screenRotation = COpenWFC_RI_Display::EScreenRotate0;
        screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                &screenRotation,
                                sizeof(screenRotation));
        screenContext->CommitAttribute();

        COpenWFC_RI_Display::TScreenGeometryAttribute attributes;
        screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry, &attributes, 
                                sizeof(COpenWFC_RI_Display::TScreenGeometryAttribute));
    
        if(ns == NULL)
            {
            TSurfaceId surface = TSurfaceId::CreateNullId();
            ns=iUtility->CreateStreamL(TSize(attributes.iNormalWidth,attributes.iNormalHeight), 
                                                iFastpathablePixelFormat, attributes.iNormalStride, 
                                                surface, iContiguous, 1);
            ASSERT_FALSE((*ns).IsNull());
            }
    
        if(i%3 == 0)
            {
            iUtility->FillStreamL(ns, KRgbRed);
            }
        else if (i%3 == 1)
            {
            iUtility->FillStreamL(ns, KRgbGreen);                
            }
            else
                {
                iUtility->FillStreamL(ns, KRgbBlue);        
                }
    
        TInt err = screenContext->SetTopLayerSurface(ns);
        ASSERT_TRUE(err == KErrNone);
    
        err = screenContext->UpdateDisplay();   
        ASSERT_TRUE(err == KErrNone);

        CleanupStack::PopAndDestroy(screenContext);   
        
        User::After(KDelay);
        }
    }

/**
@SYMTestCaseID          GRAPHICS_OPENWFC_SCREENINTERFACE_0107
@SYMTestCaseDesc        Simple Non Trivial Attributes
@SYMREQ                 
@SYMPREQ                REQ 417-54885
@SYMTestType            Unit Test           
@SYMTestPriority        High        
@SYMTestPurpose         Check that simple non-trivial attributes are accepted by SetTopLayerSurface.
                        Note this test does not pass in "real" attributes, so the result is predictable.
@SYMTestActions 
                        Create a screen context
                        Set display attributes 
                        Create a fastpathable stream 
                        Fill the stream
                        Associate the stream with the topmost display layer 
                            and NULL Non Trivial Atrributes
                        Update the screen
                        Associate the stream with the topmost display layer 
                            and empty Non Trivial Atrributes
                        Update the screen
                        Associate the stream with the topmost display layer 
                            and wrong Non Trivial Atrributes
                        This should fail
                        Destory the screen context
@SYMTestExpectedResults
                        SetTopLayerSurface should not fail, except for wrong attribute.
                        SetTopLayerSurface should not crash or panic.
 **/        
void CTestScreenInterface::GRAPHICS_OPENWFC_SCREENINTERFACE_0107L()
    {
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_SCREENINTERFACE_0107L()"));
    LOG((" *** CTestScreenInterface test 0107L"));
    
    COpenWFC_RI_Display* screenContext = COpenWFC_RI_Display::NewL(KDefaultScreenNumber);
    CleanupStack::PushL(screenContext);
    
    TUint32 screenRotation = COpenWFC_RI_Display::EScreenRotate0;
    screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                             &screenRotation,
                             sizeof(screenRotation));
    screenContext->CommitAttribute();
    
    COpenWFC_RI_Display::TScreenGeometryAttribute attributes;
    screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry, &attributes, 
                             sizeof(COpenWFC_RI_Display::TScreenGeometryAttribute));
    
    TSurfaceId surface = TSurfaceId::CreateNullId();
    SymbianStreamType ns=iUtility->CreateStreamL(TSize(attributes.iNormalWidth,attributes.iNormalHeight), 
                                             iFastpathablePixelFormat, attributes.iNormalStride, 
                                             surface, iContiguous, 1);
    ASSERT_FALSE((*ns).IsNull());
    
    iUtility->FillStreamL(ns, KRgbGreen);
    
    TInt err = screenContext->SetTopLayerSurface(ns,NULL);
    ASSERT_FALSE(err);
    screenContext->UpdateDisplay();
    
    TInt emptyList=0;
    err = screenContext->SetTopLayerSurface(ns,&emptyList);
    ASSERT_FALSE(err);
    screenContext->UpdateDisplay();
    
    TInt wrongList=-1;
    err = screenContext->SetTopLayerSurface(ns,&wrongList);
    ASSERT_TRUE(err);
    
    //Calling UpdateDisplay should fall-back to the previous setting of top layer surface
    screenContext->UpdateDisplay();
   
    CleanupStack::PopAndDestroy(screenContext);
   }

/**
@SYMTestCaseID          GRAPHICS_OPENWFC_SCREENINTERFACE_0108
@SYMTestCaseDesc        Test getting and setting attributes
@SYMREQ                 
@SYMPREQ                REQ 417-54885
@SYMTestType            Unit Test           
@SYMTestPriority        High        
@SYMTestPurpose         Check getting and setting attibutes with good and bad values
@SYMTestActions 
                        Create a screen context
                        Get and set attributes with good and bad values
                        Destory the screen context
@SYMTestExpectedResults
                        GetAttribute should always succeed with all valid TScreenAttribute values 
                        and appropriate attribute size 
                        SetAttribute should succeed for modifying the current rotation
                        Invalid valued for getting and setting attributes should return correct error code
 **/        
void CTestScreenInterface::GRAPHICS_OPENWFC_SCREENINTERFACE_0108L()
    {
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_SCREENINTERFACE_0108L()"));
    LOG((" *** CTestScreenInterface test 0108L"));

    COpenWFC_RI_Display* screenContext = COpenWFC_RI_Display::NewL(KDefaultScreenNumber);
    CleanupStack::PushL(screenContext);
    
    COpenWFC_RI_Display::TScreenGeometryAttribute attributes;
    TInt err;
    
    for(TUint ii = COpenWFC_RI_Display::EScreenAttributeInvalid; ii <= COpenWFC_RI_Display::EScreenAttributeScreenGeometry; ++ii)
        {        
        if(ii == COpenWFC_RI_Display::EScreenAttributeScreenGeometry)
            {
            err = screenContext->GetAttribute(ii, &attributes, 
                             sizeof(COpenWFC_RI_Display::TScreenGeometryAttribute));    
            }
        else
            {
            err = screenContext->GetAttribute(ii, &attributes, 
                             sizeof(COpenWFC_RI_Display::TScreenAttribute));
            }
        
        if(ii == COpenWFC_RI_Display::EScreenAttributeInvalid)
            {
            ASSERT_TRUE(err == KErrNotSupported);
            }
        else
            {
            ASSERT_TRUE(err == KErrNone);
            }        
        }
    
    TUint32 screenRotation = COpenWFC_RI_Display::EScreenRotate0;
    err = screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, &screenRotation, 
                    sizeof(screenRotation));
    // The only parameter that can be modified is the current rotation
    ASSERT_TRUE(err == KErrNone);

    err = screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeSupportedRotation, &screenRotation, 
                    sizeof(screenRotation));
    ASSERT_TRUE(err == KErrNotSupported);

    // Set attribute to NULL
    err = screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry, NULL, 
                     sizeof(COpenWFC_RI_Display::TScreenGeometryAttribute));    
    ASSERT_TRUE(err == KErrArgument);

    // Set attribute size to be 0
    err = screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeBytesPerPixel, &attributes, 0);    
    ASSERT_TRUE(err == KErrArgument);

    // Set attribute == NULL
    err = screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeBytesPerPixel, NULL, 
                     sizeof(COpenWFC_RI_Display::TScreenAttribute));    
    ASSERT_TRUE(err != KErrNone);

    // Set attribute with wrong size
    err = screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry, &attributes, 
                     sizeof(COpenWFC_RI_Display::TScreenAttribute));    
    ASSERT_TRUE(err != KErrNone);

    CleanupStack::PopAndDestroy(screenContext);
    }

/**
@SYMTestCaseID          GRAPHICS_OPENWFC_SCREENINTERFACE_0109
@SYMTestCaseDesc        Create a stream, display on screen and repeat for a number of times
@SYMREQ                 
@SYMPREQ                REQ 417-54885
@SYMTestType            Unit Test           
@SYMTestPriority        High        
@SYMTestPurpose         Check stream display repeatedly
@SYMTestActions 
                        Create a screen context
                        Set display attributes 
                        Repeat the below actions for a number of times in a for loop
                        Create a stream 
                        Fill the stream
                        Associates the stream with the topmost display layer
                        Update the screen
                        Finally
                        Destory the screen context
                        
@SYMTestExpectedResults
                        Methods should complete without error.
                        Each time, the stream should be properly displayed on screen
                        The streams should be destroyed in a timely manner after they stop being displayed
 **/ 

void CTestScreenInterface::GRAPHICS_OPENWFC_SCREENINTERFACE_0109L()
    {
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_SCREENINTERFACE_0109L()"));
    LOG((" *** CTestScreenInterface test 0106L"));
    _LIT(KDirectPost, "DirectPost");
    TBool   directPost; //If DirectPost is not defined then the exact order of buffer releasing cannot be asserted.
    TBool definedDirectPost = iConfig.GetBool(KImageSectionName, KDirectPost, directPost);
    TInt offset;
    TInt error;
    TSurfaceId surface;
    SymbianStreamType ns = NULL;
    TSurfaceId prevsurface = TSurfaceId::CreateNullId();

    COpenWFC_RI_Display* screenContext = COpenWFC_RI_Display::NewL(KDefaultScreenNumber);
    CleanupStack::PushL(screenContext);
    
    for (TInt i = 0; i < 10; ++i)
        {
        //INFO_PRINTF2(_L("Create  stream loop: %i"), i);
    
        TUint32 screenRotation = COpenWFC_RI_Display::EScreenRotate0;
        screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                &screenRotation,
                                sizeof(screenRotation));
        screenContext->CommitAttribute();

        COpenWFC_RI_Display::TScreenGeometryAttribute attributes;
        screenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry, &attributes, 
                                sizeof(COpenWFC_RI_Display::TScreenGeometryAttribute));
    
        surface = TSurfaceId::CreateNullId();
        ns=iUtility->CreateStreamL(TSize(attributes.iNormalWidth,attributes.iNormalHeight), 
                                            iFastpathablePixelFormat, attributes.iNormalStride, 
                                            surface, iContiguous, 2);
        ASSERT_FALSE((*ns).IsNull());
    
        if(i%3 == 0)
            {
            iUtility->FillStreamL(ns, KRgbRed);
            }
        else if (i%3 == 1)
            {
            iUtility->FillStreamL(ns, KRgbGreen);                
            }
            else
                {
                iUtility->FillStreamL(ns, KRgbBlue);        
                }
    
        
        if (!prevsurface.IsNull())
            {
            error=iUtility->Manager().GetBufferOffset(prevsurface,0,offset);
            ASSERT_TRUE(definedDirectPost ? error == (directPost?KErrNone:KErrArgument) :1);
            }
        TInt err = screenContext->SetTopLayerSurface(ns);
        ASSERT_TRUE(err == KErrNone);
        //GetBufferOffset is a safe way to check if the surface is still alive
        if (!prevsurface.IsNull())
            {
            error=iUtility->Manager().GetBufferOffset(prevsurface,0,offset);
            ASSERT_TRUE(definedDirectPost ? error == (directPost?KErrNone:KErrArgument) :1);
            }
    
        err = screenContext->UpdateDisplay();   
        ASSERT_TRUE(err == KErrNone);

        User::After(KMinimalDelay);
        
        error=iUtility->Manager().GetBufferOffset(surface,0,offset);
        ASSERT_TRUE(definedDirectPost ? error == (directPost?KErrNone:KErrArgument) :1);
        
        if (!prevsurface.IsNull())
            {
            error=iUtility->Manager().GetBufferOffset(prevsurface,0,offset);
            ASSERT_TRUE(error);   //previous surface should definitely be deleted by now
            }
        iUtility->DestroyStream(ns);
        error=iUtility->Manager().GetBufferOffset(surface,0,offset);
        ASSERT_TRUE(definedDirectPost ? error == (directPost?KErrNone:KErrArgument) :1);
        prevsurface=surface;
        
        User::After(KDelay);
        }
    CleanupStack::PopAndDestroy(screenContext);   
    
    if (!prevsurface.IsNull())
        {
        error=iUtility->Manager().GetBufferOffset(surface,0,offset);
        ASSERT_TRUE(error);   //previous surface should definitely be deleted by now
        }
    }

