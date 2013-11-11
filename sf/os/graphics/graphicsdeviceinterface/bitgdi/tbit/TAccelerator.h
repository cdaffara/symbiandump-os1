// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__TACCELERATOR_H__)
#define __TACCELERATOR_H__

#include <bitdev.h>
#include "TBMP.H"
#include "TGraphicsHarness.h"

class CTestBitmap
	{
public:
	inline CTestBitmap() : iBitmap(0), iDevice(0), iGc(0) {};
	~CTestBitmap();
	TInt ConstructL(TDisplayMode aDisplayMode,TSize aSize,TBool aHWBitmap=EFalse);
	TInt ConstructL();
	void Reset();
	TBool Compare(CTestBitmap* aBitmap2);
public:
	CFbsBitmap*			iBitmap;
	CFbsBitmapDevice*	iDevice;
	CFbsBitGc*			iGc;
	};

class CTAccelerator : public CTGraphicsBase
	{
public:
	CTAccelerator(CTestStep* aStep);
	void Test();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestL();
	void Cleanup();
	TInt Random(TInt aMax);
	// Tests
	void TestAcceleratedBitmapSpecL(TDisplayMode aDisplayMode,TBool aHWBitmap);
	void TestGraphicsOperations(TDisplayMode aDisplayMode,TBool aHWBitmap);
	void TestGraphicsOperationsWithScreenL(TDisplayMode aDisplayMode,TBool aHWBitmap);
	void TestAcceleratorInfo();
	void TestAcceleratorInfoForExtendedBitmapL();
	void TestFilledRect();
    void TestFilledRectWithPattern();
    void TestInvertRect();
    void TestFadeRect();
    void TestBitBlt();
    void TestBitBltMasked();
	void TestBitBltAlphaBitmap();
	void TestAlphaBlendTwoBitmaps();
	void TestAlphaBlendOneBitmap();
	void resetColorBitmaps();

	void TestScreenFilledRect();
	void TestScreenFilledRectWithPattern();
	void TestScreenInvertRect();
	void TestScreenFadeRect();
	void TestScreenBitBlt();
	void TestScreenBitBltMasked();
	void TestScreenBitBltAlphaBitmap();
	void TestScreenAlphaBlendTwoBitmaps();
	void TestScreenAlphaBlendOneBitmap();
	void TestScreenRotation();
	void TestUserDisplayMode();

	TRegionFix<41> iFixedRegion;
	const TRegion* iFixedRegionPtr;
	TRegionFix<95> iSubRegion;
	const TRegion* iSubRegionPtr;
	TRegionFix<1> iDefaultRegion;
	TRegionFix<1> iDefaultScreenRegion;
private:
	TUint					iRandomSeed;
	CTestBitmap				iBitmap1;
	CTestBitmap				iBitmap2;
	CTestBitmap				iBitmap3;
	CTestBitmap				iBitmap4;
	CTestBitmap				iBitmap5;
	CTestBitmap				iBitmap6;
	CTestBitmap				iBitmap7;
	CTestBitmap				iBitmap8;
	CTestBitmap				iTileBitmap;
	CTestBitmap				iAlphaBitmap;
	CGraphicsAccelerator*	iGraphicsAccelerator1;
	CGraphicsAccelerator*	iGraphicsAccelerator2;
	TBitmapLockCount		iBitmapLockCount;
	CFbsScreenDevice*		iHwScreenDevice;
	CFbsBitGc*				iScreenGc;
	};

class CTAcceleratorStep : public CTGraphicsStep
	{
public:
	CTAcceleratorStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTAcceleratorStep,"TAccelerator");


#endif

