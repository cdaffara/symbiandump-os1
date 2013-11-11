// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TDEFECT2_H__
#define __TDEFECT2_H__

#include <bitdev.h>
#include "TGraphicsHarness.h"

//
//
//The main test class. Add your test methods here.
class CTDefect2 : public CTGraphicsBase
	{
public:
	CTDefect2(CTestStep* aStep);
	~CTDefect2();
	void TestL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();
private:
	void DEF039237L();
	void DEF039331L();
	void DEF039650L();
	void ExerciseCopyRect();
	TBool CopyRectReadOutsideBitmap(TInt aSingleMode =0,TInt aRetriesLeft =10);
	
	void GetPixelPerformance1L();
	void RotateMoveTextL();
	void SwapWidthAndHeightL();

	void DoRotateMoveTextL();
	void CreateScrDevAndContextL();
	TInt CreateScrDevAndContext(TDisplayMode aDisplayMode);
	TInt CreateScrDevAndContext(TInt aScreenNo, TDisplayMode aDisplayMode);
	void DeleteScreenDevice();
	void DeleteGraphicsContext();
	void CreateBitmapL(const TSize& aSize, TDisplayMode aMode);
	void DeleteBitmap();
	void DeleteBitmapDevice();
	void CreateFontL();
	void DestroyFont();
	void NonZeroOriginClearL();
	void DEF115395L();
	void TestDirtyMaskBitmapL();
	void ZeroSizedPatternBrushL();
	void CFbsBitGcInternalizeLFailL();
	void PixelsToTwipsConversionCheck();
	void CopyRectAlphaL();
	void TestSetBitsL();
    void TestMaskForAllCombinationL(TInt aChannelControl);
    void TestMaskForSelectedValuesL(TInt aChannelControl);
    void DoMaskTestL(TInt aSrcChannel, TInt aSrcMask, TInt aTargetMask, TInt aTargetChannel, TInt aChannelControl, CFbsBitmap* aSrcBmp, CFbsBitmap* aMaskBmp, CFbsBitmap* aTargetBmp, CBitmapContext* aTargetBmpContext);
    void CheckValues(TUint aAlphaPixelValue, TUint aChannelPixelValue, TInt& aFailsPerPass, TInt aTargetMask, TInt aTargetChannel, TInt aSrcMask, TInt aSrcChannel, TInt aOtherMask, TUint* aReadPixel);
    void LogColourEvent(TInt aPreMulDestPixColor,TInt aNonPreMulDestPixColor,TInt aPreMulSrcPixelColor,TInt aNonPreMulSrcPixelColor,TReal aVal1,TReal aVal2,TReal aVal3,TRefByValue<const TDesC> aMsg,TBool aErr);

private:
	CFbsScreenDevice* iScrDev;
	CFbsBitGc* iGc;
	TSize iSize;
	TDisplayMode iCurrentMode;	
	CFbsBitmap* iBitmap;
	CFbsBitmapDevice* iBmpDevice;
	};

class CTDefect2Step : public CTGraphicsStep
	{
public:
	CTDefect2Step();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTDefect2Step,"TDefect2");

#endif
