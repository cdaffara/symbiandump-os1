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

#include "TSU_MmTsthStep02.h"
#include "TSU_MmTsthSuite02.h"

// preamble
TVerdict RTSUMmTsthStep02::OpenL()
	{
	// initialise iScript and associated objects for tests. if this fails, tests are
	// inconclusive (will not run)
	iLogClient  = iSuite->LogSystem();
	TRAPD(err, iTestUtils  = CTestUtils::NewL(iLogClient));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("Could not initialise CTestUtils, error code %d"), err);
		return iTestStepResult = EInconclusive;
		}

	// NB : CScript has a NewL() for completion only : it is through NewLC that the Test
	// Framework initialises its CScripts
	TInt64 defTime(-1);
	TRAPD(err2, iTestScript = CScript::NewLC(iTestUtils, iLogClient, defTime, KNullDesC);
				CleanupStack::Pop(iTestScript) );
	if(err2 != KErrNone)
		{
		ERR_PRINTF2(_L("Could not initialise CScript, error code %d"), err2);
		return iTestStepResult = EInconclusive;
		}

	return iTestStepResult = EPass;
	}

// postamble
void RTSUMmTsthStep02::Close()
	{
	delete iTestScript;
	delete iTestUtils;
	}
