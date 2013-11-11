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

#ifndef __TTEXTLISTSCROLL_H__
#define __TTEXTLISTSCROLL_H__


#include "te_conestepbase.h"
#include "wavegenerator.h"

#include <coecntrl.h> 
#include <coeaui.h>
#include <eikappui.h>


/**
This class scrolls a list box at different rates to simulate different gestures.
Each time a frame is drawn, the list box can be scrolled either forwards or 
backwards by a certain number of pixels. This simulates the list box moving 
faster or slower. A data set that contains different pixel amounts is used to 
change the rate over a fixed period of time. The data sets are generated either 
manually or by some equation. The first cycle of the test scrolls the list box 
down at a rate that increases over time before slowing down at the same rate. 
The list box then scrolls backwards the same way but at a smaller pixel rate 
(appearing to be scrolled slower). The last cycle of the test scrolls the list 
box quickly initially before gradually slowing down. Different data sets can be 
used to simulate different gestures.
*/
class CTextList : public CCoeControl
    {
public:
    static CTextList* NewL(const TRect& aRect,const CCoeControl* aParent = NULL);
    static CTextList* NewLC(const TRect& aRect,const CCoeControl* aParent = NULL);
    ~CTextList();

    // from CCoeControl
    TSize MinimumSize();
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

private:
    CTextList();
    void ConstructL(const TRect& aRect,const CCoeControl* aParent);
    static TInt TimerExpired(TAny* aPtr);
    void AdjustRateL();
    
    // from CCoeControl
    void Draw(const TRect& aRect) const;
    
private:
    RSemaphore iSemaphore; // handle to a Semaphore which is owned by the test step
    WaveGenerator::TValueList iCurrentDataSet;
    CDesCArrayFlat* iItemTextArray;
    CPeriodic* iTimer;
    // The pixel row number of the control to start drawing from
    TInt iPos;  
    TInt iPixelsPerFrame;
    TInt iTickCount;
    RWsSession& iWsSession;
    };

class CTextListScrollAppUi : public CEikAppUi 
    {
public:
    CTextListScrollAppUi();
    ~CTextListScrollAppUi();
    void ConstructL();
    
private:
    CTextList* iTextList;
    };


class CTTextListScroll : public CTe_ConeStepBase
    {
public:
    CTTextListScroll();

private:
    // From CTestStep
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPostambleL();
    virtual void InitUIL(CCoeEnv* aCoeEnv);

    void GenerateEventL(TRawEvent::TType aEventType);
    
private:
    CTextListScrollAppUi*  iAppUi;
    RSemaphore iSemaphore; // used to wait for font control to finish drawing
    };

_LIT(KTTextListScroll,"ttextlistscroll");


#endif
