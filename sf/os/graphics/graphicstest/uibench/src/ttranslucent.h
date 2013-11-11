// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TTRANSLUCENT_H__
#define __TTRANSLUCENT_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <w32std.h>
#include <test/ttmsgraphicsstep.h>



class CTTranslucent : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTTranslucent();
	~CTTranslucent();
		
private:
	class CWin : public CBase
	    {
	public:
	    CWin(RWsSession& aWsSession, RWindowGroup& aWindowGroup);
	    void ConstructL(const TRect& aRect);
	    ~CWin();
	    
	    RWindow& Window();

	protected:
		RWsSession& iWsSession;
		RWindowGroup& iWindowGroup;
	    RWindow iWindow;
	    TRect iRect;
	    };

	// From CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();

	enum TTestCase 
		{
		EBlankWindows,
		ETextWindows,
		EEllipseWindows,
		EBitmapWindows,
		};

	void RunTestCaseL(const TDesC& aTestName, TTestCase aTestCase, TInt aHorizontalWindows, TInt aVerticalWindows);
	void ConstructWindowsL(TInt aHorizontalWindows, TInt aVerticalWindows);
	void DestroyWindows();
	void DrawWindowL(CWin* aWindow, const TRect& aRect);

private:	
	RWsSession iWsSession;
	RWindowGroup iWindowGroup;
	CWsScreenDevice* iScreen;
	CWindowGc* iGc;
	TRect iScreenRect;

	TTestCase iTestCase;    
    CWin* iTopWindow;
    CWin* iBottomWindow;
	RPointerArray<CWin> iWindowArray;
	CFbsBitmap* iBitmap;
	};

_LIT(KTTranslucent,"ttranslucent");

#endif //__TTRANSLUCENT_H__
