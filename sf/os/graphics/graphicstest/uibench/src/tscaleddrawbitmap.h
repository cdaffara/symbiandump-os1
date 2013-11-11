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
 
#ifndef __GRAPHICSPERFORMANCESCALEDDRAWBMP_H__
#define __GRAPHICSPERFORMANCESCALEDDRAWBMP_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <test/ttmsgraphicsstep.h>

class CTScaledDrawBitmap : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTScaledDrawBitmap();
	
private:
	void DoTestScaledDrawBitmapL (TInt aRotation, TDisplayMode aMode,  TDisplayMode aScreenMode, TInt aIters);
	
	// From CTestStep
	virtual TVerdict doTestStepL();

private:	
	CFbsBitGc::TGraphicsOrientation	iRotation;
	};

_LIT(KTScaledDrawBitmap,"tscaleddrawbitmap");

#endif //__GRAPHICSPERFORMANCESCALEDDRAWBMP_H__
