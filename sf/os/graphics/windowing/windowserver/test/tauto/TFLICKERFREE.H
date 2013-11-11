// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests the usage of Off-Screen Bitmap
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TFLICKERFREE_H__
#define __TFLICKERFREE_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include <e32math.h>    //to include math finction rand()
#include "TGraphicsHarness.h"

//===================================================
// CBGWin Definition
//===================================================

class CBGWin : public CTWin
	{
public:
	CBGWin();
	~CBGWin();
	/*Draws and colors the rectagles that are to fill the screen.*/
	void Draw();
	void ConstructWin(TPoint, TSize, TBool);
	void EnableOSBd(TBool);
	};


//===================================================
// CFlickerTest Definition
//===================================================
class CTFlickerFree : public CTWsGraphicsBase
	{
public:
	CTFlickerFree(CTestStep* aStep);
	~CTFlickerFree();
	/*Sets the windowing environment,*/
	void ConstructL();

protected:
//from 	CTGraphicsStep - Calls the flicker-drawing test.
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	/*Starts first with flickering screen and after 4 senconds it
	  switches to flicker free drawing.*/
	void TestFlickering();

private:
	CBGWin *iBgWin;
	};

class CTFlickerFreeStep : public CTGraphicsStep
	{
public:
	CTFlickerFreeStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTFlickerFreeStep,"TFlickerFree");

#endif
