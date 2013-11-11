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


#ifndef __TFULLSCREENTEXT_H__
#define __TFULLSCREENTEXT_H__


#include "te_conestepbase.h"

#include <coecntrl.h> 
#include <coeaui.h>
#include <eikappui.h>


_LIT(KFullScreenText,"tfullscreentext");


class CGlobalTextControl : public CCoeControl
	{
public:
    static CGlobalTextControl* NewL(const CCoeControl* aParent, 
    		TBool aDrawVertically, TBool aRightToLeft);
    static CGlobalTextControl* NewLC(const CCoeControl* aParent, 
    		TBool aDrawVertically, TBool aRightToLeft);
    ~CGlobalTextControl();

private: 
    CGlobalTextControl(TBool aDrawVertically, TBool aRightToLeft);
    void ConstructL(const CCoeControl* aParent);

    // From CCoeControl
    void Draw(const TRect& aRect) const;
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
private:    
    TBidiText* iBidiText;
    CWsScreenDevice* iScreen;
    RSemaphore iSemaphore;
        
    TMargins8 iMargin;	
	TBool iDrawVertically;
	TBool iRightToLeft;
	
	RWsSession& iWsSession;
	};


class CTFullScreenText : public CTe_ConeStepBase
	{
public:
	CTFullScreenText();

private:
	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	virtual void InitUIL(CCoeEnv* aCoeEnv);
	
	void FullScreenTextL();
	void GenerateEventL(TRawEvent::TType aEventType);

private:
    RSemaphore iSemaphore;
	TBool iDrawVertically;
	TBool iRightToLeft;
	};


class CFullScreenTextAppUi : public CEikAppUi
	{
public:
	CFullScreenTextAppUi();
	virtual ~CFullScreenTextAppUi();
	void ConstructL(TBool aDrawVertically, TBool aRightToLeft);
	
private:
	CGlobalTextControl* iGlobalTextControl;
	};

#endif
