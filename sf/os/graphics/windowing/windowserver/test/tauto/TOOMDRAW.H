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

#ifndef __TOOMDRAW_H__
#define __TOOMDRAW_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

#define LOGGING yes		//Log only failures
#if defined(LOGGING)
	#define FULLLOGGING yes
#endif

class CTestWin : public CTWin
	{
public:
	CTestWin();
	void Draw();
	};

class CBaseTestWin : public CTBackedUpWin
	{
public:
	CBaseTestWin();
	void Resized(const TSize &aNewSize);
	};

class CToomDraw : public CTWsGraphicsBase
	{
private:
	enum {EMaxOOMState=150};
public:
	CToomDraw(CTestStep* aStep);
	~CToomDraw();
	void ConstructL();
	void CreateTestWinL();
	void TriggerOomDrawsL(TInt aCount);
	TBool CheckWindowsL();
	void DeleteWindows();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TBool iDoCheck;
	TSize iScrSize;
	CTBlankWindow* iBlankWin;
	CTestWin *iTestWin;
	CBaseTestWin *iBaseWin;
	CTestWin *iBackWin;
	TInt iFails;
	TInt iFailAt;
	TInt iRetryCount;
	TInt iState;
	};

class CToomDrawStep : public CTGraphicsStep
	{
public:
	CToomDrawStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KToomDrawStep,"TOOMDRAW");


#endif
