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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TFADINGBITMAP_H__
#define __TFADINGBITMAP_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"


//===================================================
// CBaseWin Definition
// The plain green background window
//===================================================

class CBaseWin : public CTWin
	{
public:
	CBaseWin();
	~CBaseWin();
	void Draw();
	void ConstructWinL(TPoint, TSize, TBool);

public:
	CFbsBitmap *iTempBitmap;
	CFbsBitmap *iMaskGray256;
	CFbsBitmap *iMaskGray2;
	CFbsBitmap *iTempMask;
	CFbsBitmap *iBitmap;
	
	TRgb iFadedPixel;
	TRgb iNonFadedPixel;

	TBuf <11> iMode;
	};


//===================================================
// CTFadingBitmap Definition
// Bitmaps to be tested
//===================================================

class CTFadingBitmap : public CTWsGraphicsBase
	{
public:
	CTFadingBitmap(CTestStep* aStep);
	~CTFadingBitmap();
	/*Sets the windowing environment,*/
	void ConstructL();

protected:
	//from 	CTGraphicsStep - Calls the fading bitmap test.
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestFadingL ();

private:
	CBaseWin *iBgWin;
	TBool iTestResult;
	};

class CTFadingBitmapStep : public CTGraphicsStep
	{
public:
	CTFadingBitmapStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTFadingBitmapStep,"TFadingBitmap");

#endif
