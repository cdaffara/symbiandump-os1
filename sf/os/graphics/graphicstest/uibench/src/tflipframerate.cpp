// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <graphics/directgdidriver.h>
#include <hal.h> 
#include "tflipframerate.h"

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
const TInt KWindowGroupHandle = 1;
const TInt KWindowHandle = 2;

const TInt KTextPhraseFontSize = 40;
const TReal KTextPhraseSpeed = 200.f;

const TRgb KBackgroundColour    = TRgb(255, 127, 0); // Orange
const TRgb KBrushColour         = TRgb(255, 255, 0); // Yellow
const TRgb KPenColour           = TRgb(0, 0, 255, 255); // Blue

const TUint KMinFrames = 51;// uibench, needs more than 50 results for the trimmed mean

_LIT(KTextPhrase, "Flip Performance");

TVerdict CTFlipFramerate::doTestStepPreambleL()
    {    
    // Window related Setup
    // Connect to windows server session    
    TESTL(KErrNone==iWs.Connect());
       
     // Make device for this session
    iScreenDev = new (ELeave) CWsScreenDevice(iWs);
    TESTL(KErrNone==iScreenDev->Construct(KSgScreenIdMain));

    // Create a window group
    iWinGroup = RWindowGroup(iWs);
    TESTL(KErrNone==iWinGroup.Construct(KWindowGroupHandle));

    // Create a window with the window group as a parent
    iWindow = RWindow(iWs);
    TESTL(KErrNone==iWindow.Construct(iWinGroup, KWindowHandle));
    
    // Activate window and set visible
    iWindow.Activate();
    iWindow.SetVisible(ETrue);

    // Connect to surface update session
    TESTL(KErrNone==iUpdateSession.Connect());   
    CTDirectGdiTestBase::doTestStepPreambleL();  
    iContext = CDirectGdiContext::NewL(*iDGdiDriver);
    
    // Image Related Setup
    // Setup off screen image
    iSurfaceSize=iWindow.Size();
    iImageInfo.iSizeInPixels = iSurfaceSize;    
    iImageInfo.iPixelFormat = EUidPixelFormatXRGB_8888;
    iImageInfo.iCpuAccess = ESgCpuAccessNone;
    iImageInfo.iUsage = ESgUsageOpenGlesTarget| ESgUsageDirectGdiSource | ESgUsageDirectGdiTarget;
    iImageInfo.iShareable = ETrue;
    iImageInfo.iScreenId = iScreenDev->GetScreenNumber();   
    
    TESTL(KErrNone==iImgCol.Create(iImageInfo, 1));
    TESTL(KErrNone==iImgCol.OpenImage(0, iImage));
    
    TESTL(KErrNone==iDGdiImageTarget->Create(iImage)); 

    TESTL(KErrNone==iSurfaceConfiguration.SetSurfaceId(iImgCol.SurfaceId()));
    TESTL(KErrNone==iSurfaceConfiguration.SetExtent(iWindow.Size()));

    GetFontL();
    
    TInt fastCounterFrequency;
    TESTL(KErrNone==HAL::Get(HALData::EFastCounterFrequency, fastCounterFrequency));
    
    // Initialise text scrolling variables
    iTextPhraseSpeed = KTextPhraseSpeed / fastCounterFrequency;  
    iLastFrameTime = User::FastCounter();
    
    return TestStepResult();
    }

TVerdict CTFlipFramerate::doTestStepPostambleL()
    {
    iImage.Close();
    iImgCol.Close();
    
    CTDirectGdiTestBase::doTestStepPostambleL();
    
    delete iContext;
    iWindow.Close();
    iWinGroup.Close();
    
    iUpdateSession.Close();
    delete iScreenDev;
    iWs.Close();
    
    iTsStore->ReleaseFont(iFont);
    delete iTsStore;
    iTsStore = NULL;
    return TestStepResult();    
    }

/**
Set-up a font to be used in the rendering
*/
void CTFlipFramerate::GetFontL()
    {
    _LIT(KFontFamily, "NewCourier");
    iTsStore = CFbsTypefaceStore::NewL(NULL);
    TFontSpec spec(KFontFamily, KTextPhraseFontSize);
    spec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
    spec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
    TESTL(KErrNone==iTsStore->GetNearestFontInPixels(iFont, spec));
    
    iTextPhraseLength = iFont->MeasureText(KTextPhrase, NULL, NULL);
    }

/**
Draw some content
*/
void CTFlipFramerate::RenderL()
    {
    iContext->Activate(*iDGdiImageTarget);
    iContext->SetBrushColor(KBackgroundColour);
    iContext->Clear(); 
    
    // Draw Text  
    iContext->SetFont(iFont);
    iContext->SetBrushStyle(DirectGdi::ENullBrush);
    TPoint textPos(iTextPos,iSurfaceSize.iHeight/2);
    
    TReal fastCounter = User::FastCounter();
    
    if(fastCounter<iLastFrameTime)
        {
        // Handle single roll-over of fastcounter        
        TReal actualCount = static_cast<TReal>(KMaxTUint) + fastCounter;                
        iTextPos+=(iTextPhraseSpeed * (actualCount-iLastFrameTime));        
        }
    else
        {        
        iTextPos+=(iTextPhraseSpeed * (fastCounter-iLastFrameTime));
        }
    
    iContext->SetBrushColor(KBrushColour);
    iContext->SetPenColor(KPenColour);
    iContext->DrawText(KTextPhrase,NULL, textPos);
 
    // Update the last frame time, used to animate that which is drawn in RenderL()
    iLastFrameTime = User::FastCounter();
    
    iDGdiDriver->Finish();
    }

/*
Set the size of the displayed window as a fraction of the screens size.

@param aWindowSizeDivisor  Controls the displayed window size
*/
void CTFlipFramerate::SetSizeL(const TScreenSizeDivisors aWindowSizeDivisor)
    {
    TSize size = iScreenDev->SizeInPixels();    
    size.iHeight/=aWindowSizeDivisor;
    size.iWidth/=aWindowSizeDivisor;
    iWindow.SetSize(size);    
    iImageInfo.iSizeInPixels = size;
    TESTL(KErrNone==iSurfaceConfiguration.SetExtent(size));    
    }

/*
Time how long it takes to display a submitted update

@param aIsFlipped  Whether or not the surface is to be vertically flipped before it it displayed
@param aWindowSizeDivisor  Controls the displayed window size to be tested
@param aOrientation  The surface rotation to be tested
@param aStepName  The name of the test, for logging.
*/
void CTFlipFramerate::TestFramerateOrientationL(   const TFlipped aIsFlipped,
                                        const TScreenSizeDivisors aWindowSizeDivisor,
                                        const CFbsBitGc::TGraphicsOrientation aOrientation)
    {    
    // Simulate small sized window (e.g. like video call) and implicitly disable fast pathing.
    SetSizeL(aWindowSizeDivisor);
    
    // Set-up the timer
    iProfiler->InitResults();
    
    TESTL(KErrNone==iSurfaceConfiguration.SetOrientation(aOrientation));
    TESTL(KErrNone==iSurfaceConfiguration.SetFlip(aIsFlipped));
    TESTL(KErrNone==iWindow.SetBackgroundSurface(iSurfaceConfiguration, ETrue));
        
    TRequestStatus availabilityStatus;
    TRequestStatus displayedStatus;
    TTimeStamp dummyTimeStamp;    
    TInt framesDone=0;
    
    iUpdateSession.NotifyWhenAvailable(availabilityStatus);
    TESTL(KErrNone==iUpdateSession.SubmitUpdate(KAllScreens, iImgCol.SurfaceId(), 0));      
    
    // uibench, needs more than 50 results for the trimmed mean, so repeat as required.
    while(framesDone < KMinFrames)
        {
        iTextPos = -iTextPhraseLength;  // enter stage left
        
        // scroll the text from left to right across the surface.
        while(iTextPos<=iSurfaceSize.iWidth)
            {              
            User::WaitForRequest(availabilityStatus);
            // Draw something
            RenderL();     
    
            // Request Notifications
            iUpdateSession.NotifyWhenAvailable(availabilityStatus);
            iUpdateSession.NotifyWhenDisplayed(displayedStatus, dummyTimeStamp);        
    
            // Start Timer & Submit Update
            iProfiler->StartTimer();        
            TESTL(KErrNone==iUpdateSession.SubmitUpdate(KAllScreens, iImgCol.SurfaceId(), 0));
            
            // Wait for the update to have been displayed and stop the timer
            User::WaitForRequest(displayedStatus);        
            iProfiler->MarkResultSetL();       
            framesDone++;
            }        
        }
    iProfiler->ResultsAnalysis(KTFlipFramerate,aOrientation,ENone,iScreenDev->CurrentScreenMode(),framesDone);    
    }

void CTFlipFramerate::TestStepFramerateL(   const TFlipped aIsFlipped,
                                            const TScreenSizeDivisors aWindowSizeDivisor,
                                            const TDesC& aStepName)
    {
    SetTestStepID(aStepName);
    TestFramerateOrientationL(aIsFlipped, aWindowSizeDivisor, CFbsBitGc::EGraphicsOrientationNormal);
    TestFramerateOrientationL(aIsFlipped, aWindowSizeDivisor, CFbsBitGc::EGraphicsOrientationRotated90);
    TestFramerateOrientationL(aIsFlipped, aWindowSizeDivisor, CFbsBitGc::EGraphicsOrientationRotated180);
    TestFramerateOrientationL(aIsFlipped, aWindowSizeDivisor, CFbsBitGc::EGraphicsOrientationRotated270);
    RecordTestResultL();
    }
#endif

TVerdict CTFlipFramerate::doTestStepL()
    {
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    INFO_PRINTF1(_L("CTBitBltPerfDirectGdi can only be run with RSgImage legacy"));
    return TestStepResult();
#else
    // Test to allow comparison between flipped and non-flipped performance
    // This is because camera data needs to be flipped abuot vertical axis to show the user what they expect,
    // e.g. during a voice call.
    // No API exists for a flip about a vertical axis so this is achieved by a flip about horizontal axis,
    // and a 180degree rotation.
    // Therefore this test generates results for flipped and non-flipped performance at all four orientations.
    // Additionally it tests at both full screen and quarter screen.  It does this for two reasons :-
    // 1) Full screen, non-flipped, non-rotated can be (and at the time of writing is) fast-pathed.
    //    This makes it awkward to use as a comparison.
    // 2) Some use-cases are likely to use a non-fullscreen window, e.g. voice call.
#ifdef __WINS__  
    INFO_PRINTF1(_L("Fullscreen, NotFlipped"));
    TestStepFramerateL(ENotFlipped,EScreensizeFull,_L("GRAPHICS-UI-BENCH-0173"));
    INFO_PRINTF1(_L("Fullscreen, Flipped"));
    TestStepFramerateL(EFlipped,EScreensizeFull,_L("GRAPHICS-UI-BENCH-0174"));
    INFO_PRINTF1(_L("Quarterscreen, NotFlipped"));
    TestStepFramerateL(ENotFlipped,EScreenSizeQuarter,_L("GRAPHICS-UI-BENCH-0175"));
    INFO_PRINTF1(_L("Quarterscreen, Flipped"));
    TestStepFramerateL(EFlipped,EScreenSizeQuarter,_L("GRAPHICS-UI-BENCH-0176"));    
    
    CloseTMSGraphicsStep();
    return TestStepResult();
#else
    INFO_PRINTF1(_L("Flip tests are running on emulator only, tests skipped."));
    return TestStepResult();
#endif
#endif   //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE 
    }

