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

#ifndef __GRAPHICSPERFORMANCEALPHADRAWING_H__
#define __GRAPHICSPERFORMANCEALPHADRAWING_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <w32std.h>

class CTAlphaDrawing : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTAlphaDrawing();
	~CTAlphaDrawing();
	
private:
	enum TTestCase
		{
		EUseOpaqueDraw,
		EUseTransparencyFactor,
		};

	void TestDrawWindowL(TDisplayMode aMode, TInt aIters, TTestCase aTestCase, const TDesC& aTestName);
	void DrawBlankWindowL (RWindow& aForeGndWin, TInt aIters);
	
	void DoTestDrawOpaqueWindowL(TDisplayMode aMode, TInt aIters);
	void DoTestDrawTransparentWindowFactorL(TDisplayMode aMode, TInt aIters);
	void DoTestDrawTransparentWindowBitmapL(TDisplayMode aMode, TInt aIters);
	void DoTestDrawTransparentWindowAlphaChannelL(TDisplayMode aMode, TInt aIters);
	void DoTestCreateWindowL(TDisplayMode aMode, TInt aIters);
	
	TInt CyclesPerPixel(TInt64 aDuration, TInt aIters, TInt aWidth, TInt aHeight, TInt aCPUSpeed);
	
	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
private:
									
	RWsSession 			iWsSession;
	RWindowGroup 		iWinGroup;
	RWindow				iBackgndWindow;

	CFbsBitmap*			iSourceImage;
	
	CWsScreenDevice* 	iScreenDevice;
	CWindowGc* 			iWindowGc;
	
	CFbsBitGc::TGraphicsOrientation	iRotation;

	TInt				iScreenMode;
	TInt				iCPUSpeed;	
	TInt64 				iDuration;
	};

_LIT(KTAlphaDrawing,"talphadrawing");

#endif //__GRAPHICSPERFORMANCEALPHADRAWING_H__
