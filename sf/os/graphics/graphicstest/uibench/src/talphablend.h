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

#ifndef __GRAPHICSPERFORMANCE_ALPHABLEND_H__
#define __GRAPHICSPERFORMANCE_ALPHABLEND_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <w32std.h>
#include <test/ttmsgraphicsstep.h>

enum TBlendTestFunc {EBlendTestVerticalLine,EBlendTestDrawRect,EBlendTestDrawText,EBlendTestDrawVerticalText,EBlendTestDrawTextAntiAliased,EBlendTestDrawTextIType};

class CAlphaBlendTest : public CTe_graphicsperformanceSuiteStepBase
	{
private:
	void DoBitBltAlphaBitmapTestL(TDisplayMode aSrcMode,TDisplayMode aDstMode, RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, TInt aNumIterations);	
	void ClearWindow(RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, TRgb aColor);		
	void BitBlt(RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, CFbsBitmap& aImage);
	void DoAlphaBlendBitmapsBitmapTestL(TDisplayMode aSrcMode, TDisplayMode aDstMode, RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, TInt aNumIterations);		
	void DoDrawBitmapL(TBool aUseMask, TDisplayMode aSrcMode, TDisplayMode aDestMode, RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, TInt aNumIterations);
	void DoNormalBitBltL(TBool aUseMask, TDisplayMode aSrcMode, TDisplayMode aDstMode, RWsSession& aSession, RWindow& aWindow, CWindowGc* aGc, TInt aNumIterations);	
	void DoDrawBlendedTestsL(TBlendTestFunc aTestFunc, RWsSession& aSession, TInt aNumIterations);	
	void DoDrawBlendedTestL(TBlendTestFunc aTestFunc, TInt aAlpha, TDisplayMode aDisplayMode, RWsSession& aSession, TInt aNumIterations);
protected:
	TVerdict doTestStepL(TBool aIType);
	TBool CheckMonoTypeInstalledL();
	};

class CAlphaBlendTestNormal : public CAlphaBlendTest
	{
public:
	CAlphaBlendTestNormal();
	// From CTestStep
	virtual TVerdict doTestStepL();
	};

class CAlphaBlendTestIType : public CAlphaBlendTest
	{
public:
	CAlphaBlendTestIType();
	// From CTestStep
	virtual TVerdict doTestStepL();
	};

_LIT(KAlphaBlendTest,"TAlphaBlend");
_LIT(KAlphaBlendTestIType,"TAlphaBlendIType");

#endif //__GRAPHICSPERFORMANCE_ROTATION_H__
