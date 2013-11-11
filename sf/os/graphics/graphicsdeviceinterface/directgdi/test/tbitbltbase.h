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

#ifndef __TBITBLTBASE_H__
#define __TBITBLTBASE_H__

#include "tdirectgdi_test_step_base.h"
#include <bitdev.h>

// target bitmap size
static const TSize KTarget1Size(200, 200);
static const TSize KTarget2Size(512, 512); // large target
// source bitmap size
static const TSize KBitmap1Size(20, 20);
static const TSize KBitmap2Size(200, 200);

/**
Type of BitBlt family function
*/
enum TBitBltFuncType
	{
	EBitBlt,
	EDrawBitmap,
	EBitBltMasked,
	EDrawBitmapMasked
	};

/**
Base class for BitBlt() family tests.
*/
class CTBitBltBase : public CTDirectGdiStepBase
	{
public:
	CTBitBltBase();
	~CTBitBltBase();

protected:
	// base tests
	void TestPositioningBaseL(const TDesC& aTestName, TBitBltFuncType aFunc);
	void TestInvalidParametersBaseL(const TDesC& /*aTestName*/, TBitBltFuncType aFunc);
	void TestSourceBitmapCloningBaseL(const TDesC& aTestName, TBitBltFuncType aFunc);

	// helper functions
	void WriteTargetOutput(TPtrC aTestCaseName);
	virtual void CreateBitmapsL(TUidPixelFormat aPixelFormat);
	virtual void DeleteBitmaps();

	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	void RunBasicTests();

private:
	virtual void TestBasicL() = 0;

protected:
	TBool iLargeTarget;
	CFbsBitmap* iNotInitialisedBitmap;
	CFbsBitmap* iZeroSizeBitmap;
	CFbsBitmap* iCompressedBitmap;
	CFbsBitmap* iCheckedBoardBitmap1;
	CFbsBitmap* iCheckedBoardBitmap2;
	CFbsBitmap* iConcentricRectsBitmap1;
	CFbsBitmap* iConcentricRectsBitmap2;
	CFbsBitmap* iCheckedBoardWithAlphaBitmap;
	CFbsBitmap* iCompressedBitmapSmall;
	};

#endif
