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

#include <test/ctefunitserver.h>
#include <test/tefunit.h>
#include "hwrmtest.h"

// Require: return server name : must be the same that the exe name
_LIT( KServerName, "te_hwrm" );
GLDEF_C const TTestName ServerName()
	{
	TTestName serverName(KServerName);
	return serverName;
	}

// Require: return the root test suite
GLDEF_C CTestSuite* CreateTestSuiteL()
	{
	START_SUITE
	ADD_TEST_SUITE(CHWRMTest)
	END_SUITE
	}

// Require: create std TEF test case
GLDEF_D CTestStep* CreateTEFTestStep(const TDesC& /*aStepName*/, CTEFUnitServer& /*aServer*/)
	{
	// this allow std TEF test steps to be created and run inside TEFUnit
	return NULL;
	}
