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

#ifndef __TPAN_H__
#define __TPAN_H__


#include "te_conestepbase.h"

#include <coecntrl.h> 
#include <eikappui.h>


_LIT(KTPan,"tpan");


class CPanAppUi;
class CPanControl;


class CTPan : public CTe_ConeStepBase
	{
public:
	CTPan();

private:
    void PanBitmapL();
    
	// from CTestStep
    virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	// from CTe_graphicsperformanceSuiteStepBase
	virtual void InitUIL(CCoeEnv* aCoeEnv);
	
private:
    RSemaphore iSemaphore; // used to wait for font control to finish drawing
    CPanAppUi*  iAppUi;
	};


class CPanAppUi : public CEikAppUi 
    {
public:
    CPanAppUi();
    ~CPanAppUi();
    void ConstructL(const TRect& aRect);
    CPanControl* PanControl();
    
private:
    CPanControl* iPanControl;
    };


class CPanControl : public CCoeControl
	{	
public:
    static CPanControl* NewL(const TRect& aRect, const CCoeControl* aParent = NULL);
    static CPanControl* NewLC(const TRect& aRect, const CCoeControl* aParent = NULL);
    virtual ~CPanControl();
    TInt Iterations();
    CFbsBitmap* Bitmap();

private: 
	CPanControl();
    void ConstructL(const TRect& aRect, const CCoeControl* aParent);
    
    // from CCoeControl
    void Draw(const TRect& aRect) const;
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
private:
    RSemaphore iSemaphore; // handle to a Semaphore which is owned by the test step
	// The pixel position of the most recent PointerEvent
	TPoint iCurrentPointerPos;	
	CFbsBitmap* iSourceBitmap;
	TInt iIterations;
	RWsSession& iWsSession;
	};

#endif
