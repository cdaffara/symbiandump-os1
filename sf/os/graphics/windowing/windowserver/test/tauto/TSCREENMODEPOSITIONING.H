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


#ifndef __TSCREENMODEPOSITIONING_H__
#define __TSCREENMODEPOSITIONING_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TScreenModeScaling.h"
#include "TGraphicsHarness.h"

class CConnection2;

class CConnection2Group : public CTWindowGroup
	{
public:
	CConnection2Group(CTClient *aClient, CConnection2 *aSecondConnection, CTestBase *aTest);
private:
	CTestBase* iTest;
public:
	CConnection2* iConnection2;
	};

class CConnection2 : public CBase
	{
public:
	~CConnection2();
	void ConstructL(CTestBase *aTest,CFbsBitmap& aBitmap);
	void DrawBitmapWin();
	CBmpWin* BitmapWin() {return iBitmapWin;}
public:
	CTClient* iClient;
	CFbsBitmap* iScreenBitmap;
	CSpriteWin*	iSpriteWin;
private:
	CConnection2Group *iGroup;
	CBmpWin* iBitmapWin;
	};



class CTScreenModePositioning : public CTWsGraphicsBase
	{
public:
	CTScreenModePositioning(CTestStep* aStep);
	~CTScreenModePositioning();
	void ConstructL();

	void WindowTestsL();
	void SpriteTestL();
	void RotationTestsL();
	TBool ScalingSupportedByDisplayMode();

protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TBool TestRect();
	void CopyScreenAndChangeBackToDefScrModeL();
	void PositionTest1L(TPoint aPosTSize);
	void PositionTest2L(TPoint aPos);
	void GetInvalidRegionTestL(TPoint aPos);
	void DoWindowTestsL();
	void CreateWindowsTestL(TPoint aPos);
	void NextScreenModeTestL(TPoint aPos);
	void ChangeScreenMode(CTClient* aClient,TPixelsAndRotation aPixelsAndRotation,TInt aMode);
	void ChangeScreenMode(CTClient* aClient,TPixelsAndRotation aPixelsAndRotation,TScreenModeEnforcement aScreenModeEnforcement,TInt aMode);
	void SetScreenMode(CTClient* aClient,TInt aMode,TScreenModeEnforcement aScreenModeEnforcement);
	void SetUpSpriteLC(RWsSprite &aSprite, RWsSession &aSession, RWindowTreeNode &aWindow,TInt aFlags=ESpriteNoShadows);
	void CompareRegionsL(const TRegion &aRegion1,const TRegion &aRegion2);
	void TestGetInvalidRegionL(TRect& aRect);
	void TestTopClientWindowPositionAPIs(TPoint aPos,RWindowBase* aWin);
	void TestChildWindowPositionAPIs(TPoint aPos,TPoint aParentPos,RWindowBase* aWin,RWindowBase* aParentWin);
	void CopyAndCompareL(TPoint aPos);
	void RestoreScreenMode();
private:
	CFbsBitmap iSpriteBitmap;
	CConnection2* iConnection2;
	CBasicWin *iTestWin;
	CTBackedUpWin* iBackedUpWin;
	CTBlankWindow* iBlankWin;
	CTBlankWindow* iTestChildWin;
	TInt iCurrentMode;
	TSize iTestWinSize;
	TPoint iCurrentScreenModeOrigin;
	TSize iCurrentScreenModeScale;
	TSize iCurrentScreenModeSize;
	TInt iScalingSupported;
 	};

class CTScreenModePositioningStep : public CTGraphicsStep
	{
public:
	CTScreenModePositioningStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTScreenModePositioningStep,"TScreenModePositioning");


#endif
