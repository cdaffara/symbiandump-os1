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
 @internalComponent - Internal Nokia test code 
*/

#include <w32std.h>

#include <wspublishandsubscribedata.h>
#include "trenderorientation.h"

const TInt KPublishTimeout  = 1000000;  // 1 second in microseconds
const TInt KNumIterations   = 20;

// Values for the device orientation that we receive via P&S from the Theme Server
// Must match those in renderorientationtracker.h, and, obviously, those used by the real theme server 
const TUid  KThemeOrientationCategory   = {0x20022E82}; // == KHbPsHardwareCoarseOrientationCategoryUid 
const TUint KThemeOrientationKey        = 0x4F726965;   // == KHbPsHardwareCoarseOrientationKey 

void CTWindowSet::ConstructL()
    {
    User::LeaveIfError(iWs.Connect());
    iWs.SetAutoFlush(ETrue);
    
    iWindowGroup = RWindowGroup(iWs);
    User::LeaveIfError(iWindowGroup.Construct(reinterpret_cast<TUint32>(&iWindowGroup)));
    
    iChildWindow = RWindow(iWs);
    User::LeaveIfError(iChildWindow.Construct(iWindowGroup, reinterpret_cast<TUint32>(&iChildWindow)));
    }

CTWindowSet::~CTWindowSet()
    {
    Destroy();
    }

void CTWindowSet::Destroy()
    {
    iChildWindow.Close();
    iWindowGroup.Close();
    iWs.Close();    
    }

CTRenderOrientation::CTRenderOrientation()
    {
    // check that these two enums are aligned
    __ASSERT_COMPILE(EDisplayOrientationAuto == ENumWindowSets);
    
    SetTestStepName(KTRenderOrientation);
    }

CTRenderOrientation::~CTRenderOrientation()
    {
    }

/**
Gets the Render Orientation as published by window server

@return TRenderOrienation that was last publised by window server.
 */
TRenderOrientation CTRenderOrientation::GetRenderOrientationL()
    {    
    return GetOrientationL(iWsRenderOrientationProperty);    
    }

/**
Gets the Theme Orientation as published by theme server

@return TRenderOrienation that was last publised by theme server.
 */
TRenderOrientation CTRenderOrientation::GetThemeOrientationL()
    {    
    return GetOrientationL(iThemeOrientationProperty);    
    }

/**
Gets the orientation as published to the given RProperty

@return TRenderOrienation that was last publised to the given RProperty
 */
TRenderOrientation CTRenderOrientation::GetOrientationL(RProperty& aProperty)
    {
    TInt orientation=EDisplayOrientationNormal;
    User::LeaveIfError(aProperty.Get(orientation));
    
    TESTL(orientation >= EDisplayOrientationNormal);
    TESTL(orientation < EDisplayOrientationAuto);    
    
    return static_cast<TRenderOrientation>(orientation);    
    }

/**
Tests each usable TRenderOrientation KNumIterations times for the given test phase / use case.

@param aStepName - the test step ID to use
@param aTestPhase - the internal test phase 
 */
void CTRenderOrientation::TestOrientationChangeL(const TDesC& aStepName, TTestPhase aTestPhase)
    {
    SetTestStepID(aStepName);
    
    // more preamble to toggle between timing the wserv render orienation property
    // and the theme server orientation property
    RProperty *orientationProperty = NULL;
    switch(aTestPhase)
        {          
        case EThemeOrientationChangeOnly:
            {
            // render orientation ignores theme orientation
            iWindowSet[EFirstWindowSet].Session().IndicateAppOrientation(EDisplayOrientationNormal);
            // we want to subscribe and wait for the theme orientation published by the theme server
            orientationProperty = &iThemeOrientationProperty;            
            break;
            }
        case EThemeOrientationChange:
            {
            TESTL(EDisplayOrientationNormal == GetThemeOrientationL());
            iWindowSet[EFirstWindowSet].Session().IndicateAppOrientation(EDisplayOrientationAuto);
            }
            // deliberate drop-through
        default:
            // we want to subscribe and wait for the render orientation published by WServ            
            orientationProperty = &iWsRenderOrientationProperty;            
            break;
        }    
    
    TInt renderOrientation = GetRenderOrientationL();
    
    // For consistancy, check that we are starting from the same orientation
    TESTL(EDisplayOrientationNormal == renderOrientation);    
    
    // Set-up the timer
    iProfiler->InitResults();
    iTimingsTaken = 0;    
    
    // repeat numerous times to get a decent average
    for(TInt iterations=0; iterations < KNumIterations; ++iterations)
        {
        renderOrientation = GetRenderOrientationL();
        // For consistancy, check that we are starting from the same orientation
        TESTL(EDisplayOrientationNormal == renderOrientation);           
        
        // loop through the orientations, ending up changing back to normal
        for(++renderOrientation; renderOrientation <= EDisplayOrientationAuto; ++renderOrientation)
            {
            // % can be slow, do it outside of the timing
            TRenderOrientation testOrientation = static_cast<TRenderOrientation>(renderOrientation%EDisplayOrientationAuto);
            
            orientationProperty->Subscribe(iOrientationStatus);
            
            // start the timeout timer
            iTimeoutTimer.After(iTimeoutStatus, KPublishTimeout);
            // start the results timer
            iProfiler->StartTimer();
                        
            switch(aTestPhase)
                {
                case EIndicatedOrientationChange:
                    // Do the indicated orientation Change
                    iWindowSet[EFirstWindowSet].Session().IndicateAppOrientation(testOrientation);
                    break;
                    
                case EWindowOrdinalChange:
                    // move the relevant window group to the front
                    // N.B. this will go wrong if the number of orientations and windows are not equal
                    iWindowSet[testOrientation].WindowGroup().SetOrdinalPosition(0);
                    break;
                    
                case EThemeOrientationChange:
                    // Needs the focus window to be in auto mode
                    // deliberate drop through
                case EThemeOrientationChangeOnly:                                        
                    iThemeOrientationProperty.Set(testOrientation);
                    break;
                    
                default:
                    TESTL(EFalse);
                }
        
            // Wait for the update to have been published ( or time out while waiting )
            User::WaitForRequest(iOrientationStatus, iTimeoutStatus);
            
            iProfiler->MarkResultSetL();
            ++iTimingsTaken;
            
            if(KErrNone != iOrientationStatus.Int())
                {
                // timed out
                iWsRenderOrientationProperty.Cancel();                
                TESTL(EFalse);
                }
            else
                {
                // Check that it is actually the expected orientation
                if(EThemeOrientationChangeOnly == aTestPhase)
                    TESTL(GetThemeOrientationL() == testOrientation);
                else
                    TESTL(GetRenderOrientationL() == testOrientation);                
                }

            if(KRequestPending == iTimeoutStatus.Int())
                {
                // as expected, so cancel the timeout timer
                iTimeoutTimer.Cancel();
                }
            else
                {
                // timed out
                TESTL(EFalse);
                }
            }
        }    
    
    // wrap it up    
    iProfiler->ResultsAnalysis(KTRenderOrientation,KErrNotFound,ENone,ENone,iTimingsTaken);
    }

TVerdict CTRenderOrientation::doTestStepL()
    {     
    INFO_PRINTF1(_L("Testing: Indicated Orientation Change"));
    TestOrientationChangeL(_L("GRAPHICS-UI-BENCH-0201"), EIndicatedOrientationChange);
    
    INFO_PRINTF1(_L("Testing: Window Ordinal Position Change"));
    TestOrientationChangeL(_L("GRAPHICS-UI-BENCH-0202"), EWindowOrdinalChange);
    
    INFO_PRINTF1(_L("Testing: Theme Orientation Change"));
    TestOrientationChangeL(_L("GRAPHICS-UI-BENCH-0203"), EThemeOrientationChange);
    
    INFO_PRINTF1(_L("Testing: Theme Orientation Change Only"));
    TestOrientationChangeL(_L("GRAPHICS-UI-BENCH-0204"), EThemeOrientationChangeOnly);    
    
    return TestStepResult();    
    }

_LIT(KThemeServerPropertyDefine, "twsthemeserverpropertydefine.exe");
_LIT(KThemeServerPropertyDefineCmdDefine, "define");
_LIT(KThemeServerPropertyDefineCmdDelete, "delete");   

/**
Uses a test executable to define or delete a test version of the theme server rotation RProperty
 */
void CTRenderOrientation::ThemeServerProperty(const TDesC& aCmd)
    {
    /* This Process called with the argument KThemeServerPropertyDefineCmdDefine defines the
       theme server RProperty, or with KThemeServerPropertyDefineCmdDelete, deletes 
       the theme server RProperty.
       This is because an RProperty with this catagory UID can only be defined and deleted
       from within a process with the same UID3 as the RProperty catogory you are trying to
       define/delete */
    RProcess themeServerPropertyDefine;
    TInt err = themeServerPropertyDefine.Create(KThemeServerPropertyDefine, aCmd);
    if (KErrNone != err)
        {
        _LIT(KLog, "themeServerPropertyDefine.Create() failed with error: %d");
        INFO_PRINTF2(KLog, err);
        TEST(EFalse);        
        }
    
    // wait for themeServerPropertyDefine process to terminate
    TRequestStatus themeServerPropertyDefineLogonStatus;
    themeServerPropertyDefine.Logon(themeServerPropertyDefineLogonStatus);
    themeServerPropertyDefine.Resume();
    User::WaitForRequest(themeServerPropertyDefineLogonStatus);
    if (themeServerPropertyDefineLogonStatus != KErrNone)
        {
        _LIT(KLog, "themeServerPropertyDefine.Logon() failed with error: %d");
        INFO_PRINTF2(KLog, themeServerPropertyDefineLogonStatus);
        TEST(EFalse);        
        }
    themeServerPropertyDefine.Close();    
    }

/*
Initialise for the testing
 */
TVerdict CTRenderOrientation::doTestStepPreambleL()
    {
    // Create in reverse order so that windowSet 0 is at the front/foreground
    for(TInt windowSet = ENumWindowSets - 1; windowSet >= 0 ; --windowSet)
        {
        iWindowSet[windowSet].ConstructL();
        TRenderOrientation orientation = static_cast<TRenderOrientation>(windowSet%EDisplayOrientationAuto);
        iWindowSet[windowSet].Session().IndicateAppOrientation(orientation);
        iWindowSet[windowSet].WindowGroup().SetOrdinalPosition(0);
        }
    
    User::LeaveIfError(iWsRenderOrientationProperty.Attach(KRenderOrientationCategory, KRenderOrientationKey));
    
    ThemeServerProperty(KThemeServerPropertyDefineCmdDefine);
    User::LeaveIfError(iThemeOrientationProperty.Attach(KThemeOrientationCategory, KThemeOrientationKey));
    
    User::LeaveIfError(iTimeoutTimer.CreateLocal());
    
    return CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
    }

/*
Tidy up after the testing
 */
TVerdict CTRenderOrientation::doTestStepPostambleL()
    {
    iTimeoutTimer.Close();
    
    iThemeOrientationProperty.Close();
    ThemeServerProperty(KThemeServerPropertyDefineCmdDelete);    
    iWsRenderOrientationProperty.Close();
    
    for(TInt windowThing = 0; windowThing < ENumWindowSets; ++windowThing)
        {
        iWindowSet[windowThing].Destroy();
        }
    
    return CTe_graphicsperformanceSuiteStepBase::doTestStepPostambleL();
    }
