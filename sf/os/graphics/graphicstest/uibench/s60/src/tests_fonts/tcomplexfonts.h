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


#ifndef __TCOMPLEXFONTS_H__
#define __TCOMPLEXFONTS_H__


#include "te_conestepbase.h"

#include <coecntrl.h> 
#include <coeaui.h>
#include <txtrich.h>
#include <frmtlay.h> 
#include <frmtview.h> 
#include <txtfrmat.h> 


_LIT(KTComplexFonts,"tcomplexfonts");


/**
 * Cone control that contains formatted text. If the control receives
 * a pointer event it redraws itself and signals a Semaphore once it
 * has finished with formatting and redrawing the text.
 */
class CComplexFontsControl : public CCoeControl
	{
public:
    static CComplexFontsControl* NewL(const CCoeControl* aParent=NULL);
    static CComplexFontsControl* NewLC(const CCoeControl* aParent=NULL);
    ~CComplexFontsControl();

private:
    // From CCoeControl
    void Draw(const TRect& aRect) const;
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

	CComplexFontsControl();
    void ConstructL(const CCoeControl* aParent = NULL);
    
private:
    RSemaphore iSemaphore; // handle to a Semaphore which is owned by the test step
    
	CRichText* iRichText; // text
	
	CParaFormatLayer* iParaFormatLayer;
	CCharFormatLayer* iCharFormatLayer;
	
	CTextLayout* iLayout; // text layout
	CTextView* iTextView; // text view which covers the whole screen

	CParaFormat iParaFormat;
	CWsScreenDevice* iScreen;	
	RWsSession& iWsSession;
	};

class CComplexFontsAppUi : public CEikAppUi
    {
public:
    CComplexFontsAppUi();
    virtual ~CComplexFontsAppUi();
    void ConstructL();
    
private:   
    CComplexFontsControl* iComplexFontsControl;
    };

class CTComplexFonts : public CTe_ConeStepBase
	{
public:
	CTComplexFonts();

private:
	// From CTestStep
    virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	// from CTe_ConeStepBase
	virtual void InitUIL(CCoeEnv* aCoeEnv);
	
	void DrawComplexFontsL();
	void GenerateEventL(TRawEvent::TType aEventType);
	
private:
    RSemaphore iSemaphore; // used to wait for font control to finish drawing
	};


#endif
