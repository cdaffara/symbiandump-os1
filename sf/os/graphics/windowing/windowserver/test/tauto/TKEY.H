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

#ifndef __TKEY_H__
#define __TKEY_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include <hal.h>
#include "TGraphicsHarness.h"

_LIT(KSemaphoreName,"KeyProcSem");


class CVisibleGroup: public CBase
	{
public:
	CVisibleGroup(TInt aScreenNo);
	~CVisibleGroup();
	void ConstructL();
	void SetMode(TInt aMode);
private:
	RWsSession iWs;
	RWindowGroup iGroup;
	CWsScreenDevice* iScr;
	TInt iScreenNo;
	TInt iMode;
	};


class ClientToKill : public CTClient
	{
public:
	ClientToKill();
	~ClientToKill();
	void ConstructL();
private:
	TestWindow *iTestWin;
	};



class CTKey : public CTWsGraphicsBase
	{
public:
	enum {EFuncCtrlShift=EModifierFunc|EModifierCtrl|EModifierShift};
public:
	CTKey(CTestStep* aStep);
	~CTKey();
	void ConstructL();

	void KeyOfDeathL();
	void KeyOfOrientation();
	void CycleDisplaySize();
	void ContrastKeysL();
	void BackLightKeysL();
	void BrightnessKeysL();
	void ScreenSizeModeKeysL();
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void SendKey(TInt aScanCode);
	void SendUpDown(TInt aScanCode);
	void SendCtrlAltShift(TInt aScanCode);
	void SendCtrlAltShift(TRawEvent::TType aType);
	void SetHotKey(THotKey aType, TUint aKeyCode);
	void LogWindowGroupIds();
	void TestContrastL(TBool aChange);
	void TestDefaultContrastL(TBool aChange);
	void TestBackLightL(TBool aChange);
	void TestDefaultBackLightL(TBool aChange);
	void TestBrightnessL(TBool aChange);
	void TestDefaultBrightnessL(TBool aChange);
	void TestScreenSizeModeL(TInt aKeys,TBool aChange);
	void TestDefaultScreenSizeModeL(TInt aKeys,TBool aChange);

private:
	RSemaphore iSem;
	TSize iWinSize;
	CVisibleGroup* iVisibleGroup;
	};

class CTKeyStep : public CTGraphicsStep
	{
public:
	CTKeyStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTKeyStep,"TKey");


#endif
