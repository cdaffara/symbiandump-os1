
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

#ifndef __TFADE_H__
#define __TFADE_H__

#include "AUTO.H"

#if defined(__MARM__)
	#define SMALL_RECTS
#endif
#define SIZE_X 10
#define SIZE_Y 5
#define DRAW_MODE EColor64K		//Mode used for testing fadded drawing
#define BLACK_NORMAL 128
#define WHITE_NORMAL 255
#define BLACK_ALTERNATE 0
#define WHITE_ALTERNATE 127
#define NUMBER_OF_WINDOWS	16

#define MODE_LT_64K(mode)	(TDisplayModeUtils::NumDisplayModeColors(mode)<65536)

#define ETrue	1
#define EFalse	0

class CTFade : public CTWsGraphicsBase
	{
public:
	CTFade(CTestStep* aStep);
	~CTFade();
	void ConstructL();
	TInt Content();
	void Drawing(TInt aDrawFunc, CBitmapContext *gc);
	//Different Tests
	void ColorTest();
	void BlankWindowL();
	void FadeChildrenL();
	void FadeChildrenAfterNewChildIsCreatedL();
	void FadeBehindL();
	void ColorTest2();
	void FadeTest();
	void Draw(TBool aAlternativeFade=EFalse);
	void ObscuredL();
	void MovingL();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	void GCDraw();
#endif
	void SystemFadeL();
	void SystemAlternateFadeL();
	void FadeBehindWhenMovedL();
	void FadeBehindTransparentWindowL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void CreateBlankWindowL();
	void DestroyBlankWindow();
	void CreateBackupWindowL(TBool aMaintainBackup);
	void CreateRedrawWindowL();
	void DestroyDrawableWindow();
	void CreateBitmapsL();
	void DestroyBitmaps();
	void Draw(TInt aDrawFunc, TAny *aParam, TBool aAlternativeFade=EFalse, TBool aFade=EFalse);
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	void GCDraw(TInt aDrawFunc, TAny *aParam, TBool aAlternativeFade=EFalse);
#endif
	void Draw(TInt aDrawFunc, TAny *aParam, CBitmapContext *gc);
	void CompareWithBitmap(TBool aFade);
	void CompareWindows(TBool aAlternativeFade=EFalse);
	void TestStrips(TRect aRect,TInt aHeight,TInt aNumNotFaded,TBool aInvert=EFalse);
	void TestBlocks(TRect aRect,TSize aSize,TInt aNumNotFaded,TBool aInvert=EFalse);
	TInt Fade4(TInt aGray4);
	TInt Fade16(TInt aGray16);
	TRgb FadeRgb(TRgb aColor);
	TRgb FadeRgb(TRgb aColor, TInt aFadeMapFactor, TInt aFadeMapOffset);
	inline void ViewDelay();
private:
	CTBlankWindow* iBlankWin;
	CTDrawableWin* iWin;
	TBool iOwnWindow;
	TRect iWindowRect;
	TRect iBaseRect;
	CFbsBitmap iNormalBitmap;
	CFbsBitmap iFadedBitmap;
	CFbsBitGc *iNormalBitGc;
	CFbsBitGc *iFadedBitGc;
	CFbsDevice *iNormalBitmapDevice;
	CFbsDevice *iFadedBitmapDevice;
	TInt iContent;
	TDisplayMode iBaseWinMode;
	TDisplayMode iDeviceDisplayMode;
	TSize iTestWinSize;
	TDisplayMode iFadeDrawMode;
	CFbsBitmap *iTestWinCopy;
	CFbsBitmap *iCheckWinCopy;
#if defined (__MARM_THUMB__)
GLDEF_D TBool CanFade;
GLDEF_D TBool CanFadeSet;
#endif
	};

class CRedrawWin : public CTWin
	{
public:
	CRedrawWin(CTFade *aTest);
	void ConstructL(TRect aArea);
	~CRedrawWin();
	void Draw();
	
protected:
	CTFade* iTest;
	};

class CTFadeStep : public CTGraphicsStep
	{
public:
	CTFadeStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTFadeStep,"TFade");


#endif
