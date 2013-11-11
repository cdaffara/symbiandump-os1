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

#ifndef __TOPTIMISEDBMP_H__
#define __TOPTIMISEDBMP_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <test/ttmsgraphicsstep.h>

class CTOptimisedBmp : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTOptimisedBmp();
	~CTOptimisedBmp();

private:
	// Tests for compression speed
	void CompressedBitmapTestL() ;
	void GetScanLineTestL();
	void ScanlineTestL(const CFbsBitmap& aBitmap, TDisplayMode aSrcDisplayMode, TDisplayMode aDestDisplayMode, const TDesC& aTestCaseName);
	void StripeBitmap(CFbsBitmap& aBitmap);
	void TestDecompressBitmapL(const TDesC& aFileName, const TDesC& aTestCaseName);
#ifdef _DEBUG
	void TestDecompressBitmapAltL(const TDesC& aFileName, const TDesC& aTestCaseName);
#endif
	void ResourceBitmapLoadL(const TDesC& aFileName, TInt32 aId, TBool aShareIfLoaded, const TDesC& aTestCaseName);
	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
private :
	RFs iFs ;
	CFileMan* iFileMan ;
	};

_LIT(KTOptimisedBmpStep,"TOptimisedBmp");

#endif
