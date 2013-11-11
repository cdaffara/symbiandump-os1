// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains test bitmap file names for TBitmap test app.
// 
//

#ifndef __TBITMAP_H__
#define __TBITMAP_H__

#include <gdi.h>
#include "TFBS.H"
#include "test/TGraphicsHarness.h"
#include "fbsbitmapasyncstreamer.h"


//The main test class. Add your test methods here.
class CTBitmap : public CTGraphicsBase
	{
public:
	CTBitmap(CTestStep* aStep);
	~CTBitmap();
	void DeleteScanLineBuffer();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
	void CheckScanlineL();
	void StreamBitmap();
	void StreamRectangle();
	void GetScanline();
	void SetScanline();
	void StretchScanline();
	void LoadInSegments();
	void SaveInSegments();
	void MonoDetect();
	void ChunkPileL();
	void LargeBitmaps();
	void HugeBitmaps();
	void Resize();
	void BitmapUtil();
	void BitmapTiming();
	void InvalidBitmap();
	void SaveAndLoad();
	void SaveAndLoadFast();
	void Color256BitmapUtil();
	void HardwareBitmaps();
	void Compress();
	void INC036062L();
	void PerformanceTest_INC036062L();
	void INC037474L();
	void DEF038286L();
	void DEF038286_2L();
	void MonohromeL();
	void GetVerticalScanLineL();
	void GetVerticalScanLinePerformanceL();
	void TwoComprLines();
	void DEF071843_16MAL();
	void DEF071843_16MUL();
	void DEF071843_16ML();
	void DEF074623_16MAL();
#ifdef _DEBUG
	void DEF095361_16MAL();
#endif	
	void TestPaletteCompressionL() ;
	void TestSynchronousBackgroundPaletteCompressionL() ;
	void TestAsynchronousBackgroundPaletteCompressionL() ;
	void CompareColourLikeness(const TRgb& aSrc, const TRgb& aDest, TInt aDifference=0);
	void GetAllBitmapsIdsL();
	void CheckBadBmp();
	void CheckSetDisplayModeL();
	void MultiThreadedTestsL();
	void MultiThreadedResizeTestL();
	void MultiThreadedCompressTestL();
	void DoMultiThreadedTestL(const TDesC& aThreadName, TInt (*aBitmapFunction)(CFbsBitmap*), TBool (*aCheckFunction)(const CFbsBitmap*));
#ifdef SYMBIAN_DEBUG_FBS_LOCKHEAP
	void MultiThreadedLockHeapTestL();
#endif
	void TestBgCompressL();
	void TestBgCompressInUseL();
	void TestDisconnectWithBitmapL();
	void TestTouchedAndVolatileL();
	void TestBitmapWhiteFillL();
#ifdef _DEBUG
	void TestBitmapUtilWithUnsupportedBitmap(const TDesC& aThreadName, TAny* aFunctionPtr);
	void TestBitmapUtilWithUnsupportedBitmaps();
#endif
private:
	void DoStreamBitmapSizes(TDisplayMode aDispMode);
	void DoStreamBitmap(const TSize& aSize,TDisplayMode aDispMode,TBool aBlank);
	void DoStreamRectangle(TDisplayMode aDispMode,TBool aBlank);
	void DoGetScanline(const TSize& aSize,TDisplayMode aDispMode);
	void DoGetScanlineCheck1(CFbsBitmap& aBitmap,CFbsBitmap& aReference,const TSize& aSize,TDisplayMode aDispMode);
	void DoGetScanlineCheck2(CFbsBitmap& aBitmap,CFbsBitmap& aReference,const TSize& aSize,TDisplayMode aDispMode);
	void DoGetScanlineCheck3(CFbsBitmap& aBitmap,TDisplayMode aDispMode);
	void DoGetScanlineAlt(TDisplayMode aDispMode);
	void DoSetScanline(TDisplayMode aDispMode);
	void DoStretchScanline(const TSize& aSize,TDisplayMode aDispMode);
	void DoMonoDetect(TDisplayMode aDisplayMode);
	void DoLargeBitmaps(const TSize& aSize,TDisplayMode aDisplayMode);
	void DoHugeBitmaps(const TSize& aSize,TDisplayMode aDisplayMode);
	void DoResize(TDisplayMode aDispMode);
	void DoBitmapUtil(const TSize& aSize,TDisplayMode aDispMode);
	void DoSaveAndLoad(const TSize& aSize,TDisplayMode aDispMode,TBool aBlank);
	void FuzzyCompareBitmapsColor256(CFbsBitmapEx& aBmp1,CFbsBitmapEx& aBmp2,TUint aDifLimit=7);
	void CheckScanLine(CFbsBitmapEx& aBitmap,CFbsBitmapEx& aReference,TDisplayMode aDispMode);
	void CheckWhite(TUint8* aData,TInt aDataLength,TDisplayMode aDispMode);
	void CompareBitmaps(CFbsBitmapEx& aBmp1,CFbsBitmapEx& aBmp2,TDisplayMode aDispMode);
	void DoCompareBitmaps(CFbsBitmapEx& aBmp1,CFbsBitmapEx& aBmp2,TDisplayMode aDispMode, TPoint aLineOrigin, TSize aBmpSize);
	void CopyBitmap(CFbsBitmapEx& aBmp1,const CFbsBitmapEx& aBmp2);
	void BlankBitmap(CFbsBitmapEx& aBitmap);
	void FillBitmap(CFbsBitmapEx& aBitmap);
	void StripeBitmap(CFbsBitmapEx& aBitmap);
	TRgb ExtractRgb(TUint8* aBuffer,TInt aPixelOffset,TDisplayMode aDispMode);

	static TInt BmpLoadCallBack(TAny* aPtr);
	static TInt BmpSaveCallBack(TAny* aPtr);
	CFbsBitmap* CreateBitmapLC(const TSize& aSize, TDisplayMode aMode);
	CFbsBitmap* CreateBitmapLC(const TSize& aSize, TDisplayMode aMode, const CFbsBitmap* aSrcBmp);
	CFbsBitmap* LoadBitmapLC(const TDesC& aFileName);
	void LoadAndCompressBitmapLC(CFbsBitmap*& aBmp, const TDesC& aBmpFile);
	void LoadAndNotCompressBitmapLC(CFbsBitmap*& aBmp, const TDesC& aBmpFile);
	void TestAssortedBPPandColourDepthL(const TDesC& aFilename, RFbsSession* aFbs);
	void DoStretchScanline(const TSize& aSize,TDisplayMode aSrcMode, TDisplayMode aDstMode, TBool aUseCompareLikeness);
	TInt ColorDifference(TDisplayMode aMode,TInt aAlpha=0xff);
private:
	CFbsBitmapAsyncStreamer* iBmpHandler;
	CFbsBitmap* iBitmap;
	CActiveScheduler* iAs;
	RFs iFs;
	TFileName iTestBitmapName;
	TBool iSkipRomBitmapTests;
	};


class CTBitmapStep : public CTGraphicsStep
	{
public:
	CTBitmapStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTBitmapStep,"TBitmap");

#endif//__TBITMAP_H__
