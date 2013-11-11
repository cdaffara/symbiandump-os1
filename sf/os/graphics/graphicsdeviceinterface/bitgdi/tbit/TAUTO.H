// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__TAUTO_H__)
#define __TAUTO_H__


#include <bitdev.h>
#include <test/testexecutestepbase.h>
#include "TGraphicsHarness.h"

const TInt KNumRects = 8;


class CTAuto : public CTGraphicsBase
	{
public:
	CTAuto(CTestStep* aStep);
	~CTAuto();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);

private: // Memory testing
	void TestMem(TDisplayMode aDispMode);
	void ScreenDevice(TDisplayMode aDispMode);
	void BitmapDevice(TDisplayMode aDispMode);
	void LowLevelDevice(TDisplayMode aDispMode,TBool aScreen);
private: // Color mapping testing
	void TestMapL(TDisplayMode aDispMode,TInt aShadowFactor);
	void Clear(TRgb aColor);
	void TestMapColors();
	void TestShadowArea(TInt aShadowFactor);
	void TestFadeArea();
	void DoTestFadeArea(TUint8 aBlackMap,TUint8 aWhiteMap);
	void DoTestFadeArea(TRgb aColor,TRgb aFadedColor);
	TRgb FadeColor(TRgb aColor,TInt aBlackMap,TInt aWhiteMap);
	TRgb MapColorToDisplayMode(TRgb aColor);
private: // Bitmap drawing testing
	void TestBmpL(TDisplayMode aDispMode);
	void TestGc(CFbsBitGc* aGc,const TSize& aSize);
	void Colors(CFbsBitGc* aGc,const TSize& aSize);
	void Shapes(CFbsBitGc* aGc,const TSize& aSize);
private:// Bitmap Fading Testing
	void TestFadingL();
private: // Panic testing
	void TestPanics();
	void StartThread(TInt aOption);
private:
	CFbsScreenDevice* iScreenDevice;
	CFbsBitGc* iScreenGc;
	TSize iHalfScreen;
	TRect iTestRect[KNumRects];
	};

class CTAutoStep : public CTGraphicsStep
	{
public:
	CTAutoStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTAutoStep,"TAuto");

#endif
