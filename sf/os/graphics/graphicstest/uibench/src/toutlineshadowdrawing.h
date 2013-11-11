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
*/

#ifndef __GRAPHICSPERFORMANCEULSHADOWDRAWING_H__
#define __GRAPHICSPERFORMANCEULSHADOWDRAWING_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <w32std.h>
#include <biditext.h>
#include <test/ttmsgraphicsstep.h>

/**
	Class CTOutlineShadowDrawing defined to test the performance with 
	Shadow and Outline effects.
*/
class CTOutlineShadowDrawing : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTOutlineShadowDrawing();
	~CTOutlineShadowDrawing();
	
private:
	void DoDrawTextOutlineShadowL(const TDesC& aFontFace, TInt aRotation, const TRgb& aOutlinePenColor, TBool aIsShadowOn, TBool aIsOutlineOn, TInt aDispModeIndex);
	void DoDrawVerticalTextOutlineShadowL(const TDesC& aFontFace, TInt aRotation, const TRgb& aOutlinePenColor, TBool aIsShadowOn, TBool aIsOutlineOn, TInt aDispModeIndex);
	void DoGetAPIsOutlineandShadowFontsL(const TDesC& aFontFace, TInt aRotation, TBool aShadowFlag, TBool aOutlineFlag);
	HBufC* GenerateTestCaseLC(const TDesC& aTestDesc, TBool aIsShadowOn, TBool aIsOutlineOn, TInt aOrientation, const TRgb& aPenColor);	
	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();

private:	
	CBitmapDevice* iDev;
	CFbsBitGc::TGraphicsOrientation	iRotation;	
	};

_LIT(KTOutlineShadowDrawing,"toutlineshadowdrawing");

#endif
