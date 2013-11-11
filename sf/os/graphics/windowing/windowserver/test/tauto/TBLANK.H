
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

#ifndef __TBLANK_H__
#define __TBLANK_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CBaseWindow : public CTWin
	{
public:
	CBaseWindow();
	void ConstructL(CTWinBase &parent);
	virtual void Draw();
	void SetColor(const TRgb &aRgb);
private:
	TRgb iRgb;
	};

class CTBlank : public CTWsGraphicsBase
	{
public:
	CTBlank(CTestStep* aStep);
	~CTBlank();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
public:
	void doDestruct();
	void ConstructL();
	void InvalidateTestWin(const TRect &aRect);
	void CheckBlankWindows();
	void SetColor(const TRgb &aRgb);
private:
	TBool iDoCheck;
	TSize iWinSize;
	CBaseWindow *iBaseWin;
	CTBlankWindow *iTestWin;
	};

class CTBlankStep : public CTGraphicsStep
	{
public:
	CTBlankStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTBlankStep,"TBlank");


#endif
