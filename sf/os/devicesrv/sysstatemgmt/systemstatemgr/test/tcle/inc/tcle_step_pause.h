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
 

#ifndef __TCLE_STEP_PAUSE_H__
#define __TCLE_STEP_PAUSE_H__

#include <test/testexecutestepbase.h>

#include "tcle_step_base.h"


_LIT(KTClePauseStep,"ClePauseStep");

class CClePauseTest : public CCleTestStepBase
	{
public:
	CClePauseTest();
	~CClePauseTest();

	//from CTestStep
	TVerdict doTestStepL();

private:
	CSsmCommandList* CreateCommandListL(TInt aCount);
	void CheckResultsAndCleanupL(RFs aFs, TInt aCount, TInt aPause);
	void RunDelayTest(RFs aFs, CSsmCommandList* aCmdList, TInt aCount, TInt aPause);
	};


#endif
