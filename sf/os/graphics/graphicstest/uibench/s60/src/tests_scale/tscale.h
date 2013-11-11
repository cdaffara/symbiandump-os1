// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSCALE_H__
#define __TSCALE_H__


#include "te_conestepbase.h"

#include <coecntrl.h> 
#include <coeaui.h>
#include <eikappui.h>


class CScaleAppUi;


class CTScale : public CTe_ConeStepBase
	{
public:
	CTScale();

private:
	// From CTestStep
    virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	// from CTe_graphicsperformanceSuiteStepBase
	virtual void InitUIL(CCoeEnv* aCoeEnv);

private:
    RSemaphore iSemaphore; // used to wait for font control to finish drawing
	CScaleAppUi*  iAppUi;
	};

_LIT(KTScale,"tscale");


class CScaleControl : public CCoeControl
	{
public:
    static CScaleControl* NewL(const TRect& aRect,const CCoeControl* aParent=NULL);
    static CScaleControl* NewLC(const TRect& aRect,const CCoeControl* aParent=NULL);
    ~CScaleControl();
    TInt Iterations();
    
    // from CCoeControl
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

private: 
    CScaleControl();
    void ConstructL(const TRect& aRect,const CCoeControl* aParent = NULL);
    
    // from CCoeControl
    void Draw(const TRect& aRect) const;    
    
private:
    RSemaphore iSemaphore; // handle to a Semaphore which is owned by the test step
	// The current zoom rectangle into the image
	TRect           iSourceRect; 
	// The pixel position of the most recent PointerEvent
	TPoint          iCurrentPointerPos; 	
	CFbsBitmap*		iSourceBitmap;
	TReal			iSourceRatio;
	TInt            iIterations;
	RWsSession&     iWsSession;
	};

class CScaleAppUi : public CEikAppUi 
	{
public:
	CScaleAppUi();
	~CScaleAppUi();
	void ConstructL();
	CScaleControl* ScaleControl();
	
private:
	CScaleControl* iScale;
	};

#endif
