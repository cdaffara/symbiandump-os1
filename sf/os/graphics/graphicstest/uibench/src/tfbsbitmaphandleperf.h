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
 
#ifndef __TFBSBITMAPHANDLEPERF_H__
#define __TFBSBITMAPHANDLEPERF_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <fbs.h>

class CTFbsBitmapHandlePerf : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTFbsBitmapHandlePerf();
	~CTFbsBitmapHandlePerf();

private:
	void BitmapCreationL(const TSize aSizes[], TInt aMaxNumBitmaps, const TDisplayMode aDisplayMode);
	void BitmapCreationSimpleL(const TInt aWidth, const TInt aHeight, const TDisplayMode aDisplayMode, const TDesC& aTestDescription);
	void SmallBitmapCreationL(const TDisplayMode aDisplayMode, const TDesC& aTestDescription);
	void LargeBitmapCreationL(const TDisplayMode aDisplayMode, const TDesC& aTestDescription);
	void SmallBitmapCreationSimpleL();
	void LargeBitmapCreationSimpleL();
	void BitmapDuplicateL();
	
	// From CTestStep
	virtual TVerdict doTestStepL();

private:
	RPointerArray<CFbsBitmap> iTestBitmaps;
	};

_LIT(KTFbsBitmapHandlePerfName,"tfbsbitmaphandleperf");

#endif
