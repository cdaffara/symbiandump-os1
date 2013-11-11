// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>
#include "TSU_MmTsthStep12.h"
#include "TSU_MmTsthSuite12.h"

// preamble
TVerdict RTSUMmTsthStep12::OpenL()
	{
	// initialise iSuiteStub for tests. if this fails, tests are
	// inconclusive (will not run)
	CTestSuiteVirtualStub* theSuiteStub = new (ELeave) CTestSuiteVirtualStub;
	CleanupStack::PushL(theSuiteStub);
	TRAPD(err, theSuiteStub->ConstructL());
	if(err != KErrNone)
		{
		ERR_PRINTF1(_L("CTestSuiteVirtualStub::ConstructL() left"));
		CleanupStack::PopAndDestroy(theSuiteStub);
		return iTestStepResult = EInconclusive;
		}

	iSuiteStub = theSuiteStub;

	// NB ensure the suite can log - set its logger to ours
	iSuiteStub->SetLogSystem(iSuite->LogSystem());

	CleanupStack::Pop(theSuiteStub);
	return iTestStepResult = EPass;
	}

// postamble
void RTSUMmTsthStep12::Close()
	{
	delete iSuiteStub;
	}
