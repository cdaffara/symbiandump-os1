
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

#ifndef __TSCRMODE_H__
#define __TSCRMODE_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CSecondConnection;

class CSecondConnectionGroup : public CTWindowGroup
	{
public:
	CSecondConnectionGroup(CTClient *aClient, CSecondConnection *aSecondConnection, CTestBase *aTest, CTestStep* aTestStep);
	void ScreenDeviceChanged();
private:
	CTestBase *iTest;
	CSecondConnection *iSecondConnection;
	CTestStep* iTestStep;
	};

class CSecondConnection : public CBase
	{
public:
	~CSecondConnection();
	void ConstructL(TInt aScreenNumber,const TRect &aWinRect, CTestBase *aTest, CTestStep* aTestStep);
	void EnableMessages();
	void DisableMessages();
	TInt DeviceMessageCount() const;
	void ScreenDeviceChanged();
	void SetWindow2Visibility(TBool aVisible);
public:
	CSecondConnectionGroup *iGroup;
private:
	CTClient *iClient;
	CTBlankWindow *iWindow;
	CTBlankWindow *iWindow2;
	TInt iMessageCount;
	};
	
class SecondClientConnection : public CTClient
	{
public:
	SecondClientConnection();
	~SecondClientConnection();
	void ConstructL(TInt aScreenMode, const TPixelsTwipsAndRotation &aSizeAndRotation);
	};

class CTScrMode : public CTWsGraphicsBase
	{
public:
	CTScrMode(CTestStep* aStep);
	~CTScrMode();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void ConstructL();
	void doMoveWindowTest();
	TBool MoveWindow();
	void TestPanicsL();
	void ScaledDrawingL();
	void SetScreenModeEnforcement(TScreenModeEnforcement aMode);
	void CheckWindows(TBool aWinIsVis);
	void ValidateWin(TestWindow *aWin, TRgb aColor);
	void RedrawWin(RWindow &aWin, TRgb aColor);
	void ScreenRotationsL();
	void MoreScreenRotationsL();
	TBool RectClearBugL();
	void ScreenModeChange1L();
	void ScreenModeChange2L();
	TInt FindCurrentMode();
	TBool Equal(const TPixelsTwipsAndRotation& aLeft,const TPixelsTwipsAndRotation& aRight);
	void InvisibleWndAndInfiniteRedrawBugL();
	void GetScreenDisplayMode();
	void SetScreenModeAfterScreenDeviceDeletedL();
	void TestRotateAndScreenModeL();
private:
	void DrawTestBmpL(CFbsBitmap* aTestBitmap);
	void TestDrawingToWindows(CWindowGc* aWinGc1, CWindowGc* aWinGc2, RWindow& aWindow1,RWindow& aWindow2,CFbsBitmap* aBitmap64K,CFbsBitmap* aBitmap16M,CFbsFont* aFont,TBool aAllInRedraw, TBool aUseSystemGc, TBool aUseBmp16M);
private:
	TInt iSubState;
	CSecondConnection *iSecondConnection;
	TSize iWinSize;
	TPoint iWinPos;
	TInt iWinState;
	TScreenModeEnforcement iOldEnfMode;
	TScreenModeEnforcement iCurEnforcement;
	};


	
class CTScrModeStep : public CTGraphicsStep
	{
public:
	CTScrModeStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTScrModeStep,"TScrMode");


#endif
