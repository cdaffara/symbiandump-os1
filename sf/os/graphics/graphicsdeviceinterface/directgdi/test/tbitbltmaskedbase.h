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

#ifndef __TBITBLTMASKEDBASE_H__
#define __TBITBLTMASKEDBASE_H__

#include "tbitbltbase.h"


/**
Mask types
*/
enum TMaskType
	{
	EMaskL1,
	EMaskL1Inv,
	EMaskL8,
	EMaskIterationEnd
	};

/**
Base class for BitBltMasked() family tests.
*/
class CTBitBltMaskedBase : public CTBitBltBase
	{
public:
	CTBitBltMaskedBase();
	~CTBitBltMaskedBase();

protected:
	// base tests
	void TestPositioningBaseL(const TDesC& aTestName, TBitBltFuncType aFunc);
	void TestInvalidParametersBaseL(const TDesC& /*aTestName*/, TBitBltFuncType aFunc);
	void TestSourceBitmapCloningBaseL(const TDesC& aTestName, TBitBltFuncType aFunc);
	void TestMaskCloningBaseL(const TDesC& aTestName, TBitBltFuncType aFunc);

	// helper functions
	TInt WriteTargetOutput(TPtrC aTestCaseName);
	void CreateBitmapsL(TUidPixelFormat aPixelFormat);
	void DeleteBitmaps();

	// iterate mask type
	void BeginMaskIteration();
	TBool NextMaskIteration();
		
protected:
	CFbsBitmap* iNotInitialisedMask;
	CFbsBitmap* iZeroSizeMask;
	CFbsBitmap* iMask1L1;
	CFbsBitmap* iMask2L1;
	CFbsBitmap* iMask1L8;
	CFbsBitmap* iMask2L8;
	CFbsBitmap* iCompressedMaskL8;
	CFbsBitmap* iBlackWhiteBitmap;
	
	TInt iMaskType;
	TUidPixelFormat iMaskPixelFormat;
	TBool iInvertMask;
	CFbsBitmap* iCurrentMask1;
	CFbsBitmap* iCurrentMask2;
	};
	
#endif
