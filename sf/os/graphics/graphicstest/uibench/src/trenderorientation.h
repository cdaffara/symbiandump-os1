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
 @internalComponent - Internal Symbian test code 
*/

#ifndef TRENDERORIENTATION_H
#define TRENDERORIENTATION_H

#include <e32property.h>
#include "te_graphicsperformanceSuiteStepBase.h"

/**
CTWindowSet contains a window server session, a group window and a child window.
 */
class CTWindowSet : public CBase
    {
public:
    ~CTWindowSet();
    void ConstructL();
    void Destroy();

    inline RWsSession&     Session()
        {return iWs;};
    inline RWindowGroup&   WindowGroup()
        {return iWindowGroup;};

private:
    RWsSession      iWs;
    RWindowGroup    iWindowGroup;
    RWindow         iChildWindow;    
    };

/**
Test case to test the performance of various use cases which affect the render orientation RProperty
*/
class CTRenderOrientation : public CTe_graphicsperformanceSuiteStepBase
    {
public:
    CTRenderOrientation();
    ~CTRenderOrientation();

    // From CTestStep
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    
    virtual TVerdict doTestStepL();

private:
    enum TWindowSet
        {
        // One per orientation
        EFirstWindowSet,
        ESecondWindowSet,
        EThirdWindowSet,
        EFourthWindowSet,
        
        ENumWindowSets
        };
    
    enum TTestPhase
        {
        EIndicatedOrientationChange,    // window indicates a new render orientation
        EWindowOrdinalChange,           // window order changed, render orientation dictated by a different window with a different orientation
        EThemeOrientationChange,        // theme orientation changed, render orienation dictated by this due to relevant window being set to EDisplayOrientationAuto
        EThemeOrientationChangeOnly,    // theme orientation change only, to compare timings to above
        
        ENumTestPhases
        };    
    
    void TestOrientationChangeL(const TDesC& aStepName, TTestPhase aTestPhase);
    
    TRenderOrientation  GetRenderOrientationL();
    TRenderOrientation  GetThemeOrientationL();
    TRenderOrientation  GetOrientationL(RProperty& aProperty);
    void                ThemeServerProperty(const TDesC& aCmd);
    
private:    
    CTWindowSet     iWindowSet[ENumWindowSets];
    RProperty       iWsRenderOrientationProperty;
    RProperty       iThemeOrientationProperty;
    
    RTimer          iTimeoutTimer;
    TRequestStatus  iTimeoutStatus;
    TRequestStatus  iOrientationStatus;
    TInt            iTimingsTaken;
    };

_LIT(KTRenderOrientation,"trenderorientation");

#endif /* TRENDERORIENTATION_H */
