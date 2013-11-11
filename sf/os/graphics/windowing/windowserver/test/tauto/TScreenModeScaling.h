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
// Definitions common for both scren mode positioning and scaling
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TSCREENMODESCALING_H__
#define __TSCREENMODESCALING_H__


#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

_LIT(ScalingTestName,"Screen Mode Scaling");

GLREF_C void ClearBitmap(CFbsBitmap* aBitMap);

class CBasicWin : public CTWin
	{
public:
	//Virtual Function from CTBaseWin
	void Draw();
	TInt iDrawn;
	};

class CBmpWin : public CTWin
	{
public:
	inline CBmpWin(CFbsBitmap& aScreenBmp) :iScreenBmp(aScreenBmp) {}
	//Virtual Function from CTBaseWin
	void Draw();
private:
	CFbsBitmap& iScreenBmp;
	};

class CSpriteWin : public CTWin
	{
public:
	inline CSpriteWin(CFbsBitmap& aSpriteBitmap) :iSpriteBitmap(aSpriteBitmap) {}
	~CSpriteWin();
	inline void SetState(TInt aState) {iState=aState;}
	void UpdateState(TInt aState);
	//Virtual Function from CTBaseWin
	void Draw();
private:
	CFbsBitmap& iSpriteBitmap;
	TInt iState;
	};

class CTestSpriteWin : public CTWin
	{
public:
	inline CTestSpriteWin(CFbsBitmap& aSpriteBitmap) :iSpriteBitmap(aSpriteBitmap) {}
	~CTestSpriteWin();
	inline void SetOrigin(TPoint aOrigin) {iOrigin=aOrigin;}
	void UpdateWin(TPoint aOrigin);
	//Virtual Function from CTBaseWin
	void Draw();
private:
	CFbsBitmap& iSpriteBitmap;
	TPoint iOrigin;
	};

class CBitMapWin : public CTWin
	{
public:
	inline CBitMapWin(CBitmap* aBackupBitmap) :iBackup(aBackupBitmap) {}
	//Virtual Function from CTBaseWin
	void Draw();
private:
	CBitmap* iBackup;
	};



class CTScreenModeScaling : public CTWsGraphicsBase
	{
public:
	CTScreenModeScaling(CTestStep* aStep);
	~CTScreenModeScaling();
	void ConstructL();
	void WindowTestsL();
	void SpriteTestL();
	void RotationTestsL();
	void TransparentTestsL();
	void AppScreenModeTestL();
	
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
	void DoWindowTestsL();
	void TestTopClientWindowPositionAPIs(TPoint aPos,RWindowBase* aWin);
	void TestChildWindowPositionAPIs(TPoint aPos,TPoint aParentPos,RWindowBase* aWin,RWindowBase* aParentWin);
	void TestRect();
	void CompareRegionsL(const TRegion &aRegion1,const TRegion &aRegion2);
	void TestGetInvalidRegionL(TRect& aRect);
	void CopyAndCompareL();
	void PositionTestL(TPoint aPostion);
	void BackedUpChildWindowTestL(TPoint aPostion);
	void NextScreenModeTestL(TPoint aPos);
	void GetInvalidRegionTestL(TPoint aPos);
	void TestDifferentScales(TInt aLastMode);
	void TestDifferentOrigin(TSizeMode &aMode,TPoint aOrigin);
	void DoSpriteTestsL();
	void TestDifferentOriginAndScaleForSpritesL(TSizeMode &aMode,TPoint aOrigin);
	void SetUpSpriteLC(RWsSprite &aSprite,RWsSession &aSession,RWindowTreeNode &aWindow,TInt aFlags=ESpriteNoShadows);
	void DrawTransparentWindows();
	void TestDifferentOriginAndScaleForTranspWin(TSizeMode &aMode,TPoint aOrigin);
private:
	CFbsBitmap iSpriteBitmap;
	CBmpWin* iBitmapWin;
	CFbsBitmap* iScreenBitmap;
	CBasicWin *iTestWin;
	CTBackedUpWin* iBackedUpWin;
	CTBlankWindow* iBlankWin;
	CTBlankWindow* iTestChildWin;
	TInt iCurrentMode;
	TSize iTestWinSize;
	TPoint iCurrentScreenModeOrigin;
	TSize iCurrentScreenModeScale;
	CTBackedUpWin* iCheckWin;
	CBitmap* iCheckBitmap;
	CBitmap* iTransparencyBitmap;
	CBitmap* iBackgroundBitmap;
	CBitmap* iForegroundBitmap;
	CBitMapWin* iTransWin;
	CBitMapWin* iBackgroundWin;
	TSize iWinSize;
	TDisplayMode iDisplayMode;
	};

class CTScreenModeScalingStep : public CTGraphicsStep
	{
public:
	CTScreenModeScalingStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTScreenModeScalingStep,"TScreenModeScaling");

#endif
