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
 
#ifndef __TSHADOWPERF_H__
#define __TSHADOWPERF_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <bitdraw.h>
#include <test/ttmsgraphicsstep.h>

class CTShadowPerf : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTShadowPerf();
	~CTShadowPerf();
	
private:
	enum TShadowTest
		{
		EShadow,
		EFade,		
		};
		
	void ShadowAreaL();
	void FadeAreaL();
	void FadedShadowTestL();
	void AntialiasedTextTestL();
	void AreaTestL(TShadowTest aShadowTest, const TDesC& aTestName, TInt aDisplayModeIndex);

	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	};

_LIT(KTShadowPerfName,"tshadowperf");

#endif
