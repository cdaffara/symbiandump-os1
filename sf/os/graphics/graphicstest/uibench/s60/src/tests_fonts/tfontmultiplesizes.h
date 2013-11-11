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

This file specifies the interface to the test of having a full
 	   screen of text within a CONE control. 
*/


#ifndef __TFONTS_MULTIPLE_SIZES_H__
#define __TFONTS_MULTIPLE_SIZES_H__


#include "te_conestepbase.h"

#include <coecntrl.h> 
#include <coeaui.h>
#include <eikappui.h>


class CFontSizeTestAppUi;


/**
 * This control draws text in different fonts and heights to the screen. Each time it receives
 * a button down event it redraws itself. A semaphore is used to signal the test step that 
 * drawing has been finished.
 */
class CFontSizeTestControl : public CCoeControl
	{
public:
    static CFontSizeTestControl* NewL(const CCoeControl* aParent = NULL);
    static CFontSizeTestControl* NewLC(const CCoeControl* aParent = NULL);
    ~CFontSizeTestControl();    
    inline void SetIncreaseFontSize(TBool aIncreaseFontSize);
    
private: 
    CFontSizeTestControl();
    void ConstructL(const CCoeControl* aParent = NULL);
    
    // from CCoeControl
    void Draw(const TRect& aRect) const;
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
private:
    TBidiText* iBidiText;
    RSemaphore iSemaphore;
    
    TBool iIncreaseFontSize;
    TMargins8 iMargin;
    TRgb iBackgroundColor;
    
    RWsSession& iWsSession;
        
    // mutable, because const draw function needs to change font size
    mutable TInt iFontSize;
	};


class CTFontMultipleSizesTestStep : public CTe_ConeStepBase
	{
public:
	CTFontMultipleSizesTestStep();	
	~CTFontMultipleSizesTestStep();
	
private:
	// From CTestStep
    virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPostambleL();
	virtual void InitUIL(CCoeEnv* aCoeEnv);
	
	void drawFontMultipleSizesL();
	void GenerateEventL(TRawEvent::TType aEventType);
    	
private:
    CFontSizeTestAppUi* iAppUi;
    RSemaphore iSemaphore; // used to wait for font control to finish drawing
	};

_LIT(KFontMultipleSizesTestStep,"tfontmultiplesizes");

class CFontSizeTestAppUi : public CEikAppUi
	{
public:
	CFontSizeTestAppUi();
	~CFontSizeTestAppUi();
	void ConstructL();	
	inline CFontSizeTestControl* FontSizeControl() const;
	
private:
	CFontSizeTestControl* iControl;
	};


inline void CFontSizeTestControl::SetIncreaseFontSize(TBool aIncreaseFontSize)
    { 
    iIncreaseFontSize = aIncreaseFontSize;
    }

inline CFontSizeTestControl* CFontSizeTestAppUi::FontSizeControl() const
    {
    return iControl;
    }

#endif
