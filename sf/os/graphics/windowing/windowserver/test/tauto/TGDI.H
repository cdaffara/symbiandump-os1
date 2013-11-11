// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TGDI_H__
#define __TGDI_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include <bitstd.h>
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CGdiTestLowLevel;

CGdiTestLowLevel *SetOriginTest();
CGdiTestLowLevel *PolyLineTest();
CGdiTestLowLevel *PolygonTest();
CGdiTestLowLevel *BlitMaskedTest();
CGdiTestLowLevel *DrawArcTest();
CGdiTestLowLevel *DrawPieTest();
CGdiTestLowLevel *DrawRoundRectTest();
CGdiTestLowLevel *BoxTextTest();
CGdiTestLowLevel *VertTextTest();
CGdiTestLowLevel *JustifiedTextTest();
CGdiTestLowLevel *VertBoxTextTest();
CGdiTestLowLevel *WideLineTest();
CGdiTestLowLevel *DrawTextTest();
CGdiTestLowLevel *DrawRectTest();

class CTGdi;
class CGdiTestLowLevel : public CBase
//
// BaseTest uses code that is assumed to be good to draw to one window
// TheTest uses the code beng tested to draw a matching pattern
//
	{
public:
	void AssignGdi(CWindowGc *aGc);
	void SetGdiTest(CTGdi *aGdiTest);
	virtual void PreTestSetupL(const TRect &aRect, TInt aCount);
	virtual void BaseTest(const TRect &aRect, TInt aCount)=0;
	virtual void TheTest(const TRect &aRect, TInt aCount)=0;
	virtual void ConstructL(TInt aNumOfRects);
	virtual TInt Count(TBool aMainTests)=0;
protected:
	CWindowGc *iGdi;
	CTGdi *iGdiTest;
	};

class CGdiRect : public CGdiTestLowLevel
	{
public:
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
protected:
	TRgb iBrushCol;
	TRgb iPenCol;
	};


class CGdiDrawRect : public CGdiRect
	{
public:
	TInt Count(TBool aMainTests);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	};

class CGdiDrawRoundRect : public CGdiRect
	{
public:
	TInt Count(TBool aMainTests);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	};

class CGdiDrawACP : public CGdiTestLowLevel
	{
public:
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
protected:
	TRgb iBrushCol;
	TRgb iPenCol;
	};

class CGdiDrawArc : public CGdiDrawACP
	{
public:
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	};

class CGdiDrawPie : public CGdiDrawACP
	{
public:
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	};

class CGdiPolyLine : public CGdiTestLowLevel
	{
public:
	CGdiPolyLine();
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
private:
	CArrayFixSeg<TPoint> iPnts;
	};

class CGdiPolygon : public CGdiTestLowLevel
	{
private:
	enum {KMaxPolygonPoints=100};
	enum {KPolygonStep=4};
public:
	CGdiPolygon();
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
private:
	CArrayFixSeg<TPoint> iPnts;
	};

class CGdiWideLine : public CGdiTestLowLevel
	{
private:
	enum TMaxWidths {
		EMainNumWidths=16,
		EMinorNumWidths=4};
public:
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
private:
	TInt iWidth;
	};

class CGdiSetOrigin : public CGdiTestLowLevel
	{
public:
	~CGdiSetOrigin();
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void DrawIt(const TPoint &aOffset);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
private:
	CFbsFont *iFont;
	CArrayFixFlat<TPoint> *iPolylineArray;
	CArrayFixFlat<TPoint> *iPolygonArray;
	};

class CGdiDrawText : public CGdiTestLowLevel
	{
public:
	~CGdiDrawText();
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
private:
	TInt iWidth;
	CFbsFont *iFont;
	TBuf<0x400> iBigBuffer;
	};

class CGdiDrawVertText : public CGdiTestLowLevel
	{
public:
	~CGdiDrawVertText();
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
private:
	TInt iWidth;
	CFbsFont *iFont;
	TBuf<0x400> iBigBuffer;
	};

//class CGdiDrawJustifiedText : public CGdiTestLowLevel
//DEF107985 The test for JustifiedText has been removed as it was a bad/useless test

class CGdiBoxText : public CGdiTestLowLevel
	{
public:
	~CGdiBoxText();
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
private:
	TInt iWidth;
	TInt iBbLen;
	CFbsFont *iFont;
	TBuf<0x400> iBigBuffer;
	};

class CGdiVertBoxText : public CGdiTestLowLevel
	{
public:
	~CGdiVertBoxText();
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
private:
	TInt iWidth;
	TInt iBbLen;
	CFbsFont *iFont;
	TBuf<0x400> iBigBuffer;
	};
/*
class CGdiDottedLine : public CGdiTestLowLevel
	{
	enum TMaxPatterns {
		EMainMaxPatterns=5,
		EMinorMaxPatterns=3};
public:
	void SetGridRegion(TRegion &grid,const TRect &aRect);
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
private:
	TUint iPattern;
	TInt iPattLen;
	static TUint iPatterns[EMainMaxPatterns];
	static TInt iPattLens[EMainMaxPatterns];
	};
TUint CGdiDottedLine::iPatterns[]={
		0x55FF0033,
		0xFF00,
		0x1,
		0xFFFF,
		0xF0F0F0};
TInt CGdiDottedLine::iPattLens[]={
		32,
		16,
		2,
		32,
		24};
*/
class CTestWindow : public CBlankWindow
	{
public:
	CTestWindow(TRgb aCol);
	~CTestWindow();
	void ConstructL(TPoint aPos,TSize aSize,CTWinBase* aParent, CWindowGc& aGc );
	};

class CGdiBlitMasked: public CGdiTestLowLevel
	{
	enum TNumMasks {ENumMasks=2};
public:
	~CGdiBlitMasked();
	void PreTestSetupL(const TRect &aRect, TInt aCount);
	void BaseTest(const TRect &aRect, TInt aCount);
	void TheTest(const TRect &aRect, TInt aCount);
	void ConstructL(TInt aNumOfRects);
	TInt Count(TBool aMainTests);
private:
	void doCreateTestBitmapL(CFbsBitmap *aBitmap, CFbsBitGc *&aGc, CFbsBitmapDevice *&aDevice, const TSize &aSize);
	void createTestBitmapL(CFbsBitmap *&aBitmap, const TSize &aSize);
private:
	CFbsBitmap *iMask[ENumMasks];
	CFbsBitmap *iBitmap;
//
	CFbsBitmap *iScratch;
	CFbsBitmapDevice *iScratchDevice;
	CFbsBitGc *iScratchGc;
//
	CFbsBitmap *iScratchMask;
	CFbsBitmapDevice *iScratchMaskDevice;
	CFbsBitGc *iScratchMaskGc;
//
	TBool iInvertMask;
	CFbsBitmap *iCurrMask;
	CGraphicsContext::TDrawMode iDrawMode;
	};

class CTGdi : public CTWsGraphicsBase
	{
public:
	CTGdi(CTestStep* aStep);
	void ConstructL();
	TSize WinSize();
	void GdiTestL(CGdiTestLowLevel *aTest);
	void TestDefetct_DEF045746L();
	void DrawTextOnWindow(const TDesC& aText,CTWin* aWin);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void DoGdiTestL(const TRect &aRect, TInt aNum);
	void NonDrawing();
private:
	TSize iWinSize;
	TInt iSubState;
	CGdiTestLowLevel *iGdiTest;
	CTestWindow* iWin;
	CFont* iFont;
	TInt iTextOffsetX;
	TInt iTextOffsetY;
	TBool iTestPassing;
	};
	
class CTGdiStep : public CTGraphicsStep
	{
public:
	CTGdiStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTGdiStep,"TGdi");


#endif
