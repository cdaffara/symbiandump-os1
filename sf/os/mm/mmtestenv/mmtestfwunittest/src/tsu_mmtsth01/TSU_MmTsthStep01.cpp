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
#include "script.h"
#include "parseline.h"

#include "TSU_MmTsthStep01.h"
#include "TSU_MmTsthSuite01.h"

// preamble
TVerdict RTSUMmTsthStep01::OpenL()
	{
	// initialise iParseLine and associated objects for tests. if this fails, tests are
	// inconclusive (will not run)
	// initialise a CParseLine
	iLogClient  = iSuite->LogSystem();
	TInt64 theGuardTimer (-1);

	TRAPD(err, iTestUtils  = CTestUtils::NewL(iLogClient));
	if(err != KErrNone)
	{
		ERR_PRINTF2(_L("Could not initialise CTestUtils, error code %d"), err);
		return iTestStepResult = EInconclusive;
	}

	// NB : CScript does not have a NewL()
	TRAPD(err2, iTestScript = CScript::NewLC(iTestUtils, iLogClient, theGuardTimer, KNullDesC);
				CleanupStack::Pop(iTestScript) );
	if(err2 != KErrNone)
	{
		ERR_PRINTF2(_L("Could not initialise CScript, error code %d"), err2);
		delete iTestUtils;
		return iTestStepResult = EInconclusive;
	}

	TRAPD(err3, iParseLine = CParseLine::NewL(iTestScript, iTestUtils, iLogClient, theGuardTimer, KNullDesC));
	if(err3 != KErrNone)
	{
		ERR_PRINTF2(_L("Could not initialise CParseLine, error code %d"), err3);
		delete iTestUtils;
		delete iTestScript;
		return iTestStepResult = EInconclusive;
	}

	return iTestStepResult = EPass;
	}

// postamble
void RTSUMmTsthStep01::Close()
	{
	delete iParseLine;
	delete iTestScript;
	delete iTestUtils;
	}
