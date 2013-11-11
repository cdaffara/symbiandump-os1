// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef __TBITBLTPERF_H__
#define __TBITBLTPERF_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <bitdev.h>

class CTBitBltPerf : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTBitBltPerf();
	~CTBitBltPerf();
	
private:
	enum TBitBltTest
		{
		EBitBlt,
		EBitBltMasked,		
		};

	void BitBltBitmapTestL(TBitBltTest aBitBltTest, CFbsBitmap* aBitmapToBlt,  CFbsBitmap* aBitmapMask, TRect aCropTo, TInt aOrientation, const TDesC& aTestName, CFbsBitGc* aGc);
	void MultiBitBltBitmapTestL(TBitBltTest aBitBltTest, RArray<CFbsBitmap*> aBitmapArray, CFbsBitmap* aBitmapMask, TBool aCrop, const TDesC& aTestName, CFbsBitGc* aGc);
	
	void SimpleBitBltL();
	void MaskedBitBltL();
	void MaskedBitBltSameL();
	void MaskedBitBltFlickerL();
	void MaskedBitBltAlphaL();
	void WriteAlphaLineExTestL();

	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
private:
	CFbsBitmapDevice* iBitmapDevice;
	CFbsBitmap* iBitmap12bit;
	CFbsBitmap* iBitmap16bit;
	CFbsBitmap* iBitmap24bit;
	CFbsBitmap* iBitmap32bit;
	CFbsBitmap* iTile;
	CFbsBitmap* iTile16bit;
	CFbsBitmap* iTile32bit;
	CFbsBitmap* iAlpha8bit;	
	};

_LIT(KTBitBltPerfName,"tbitbltperf");

#endif
