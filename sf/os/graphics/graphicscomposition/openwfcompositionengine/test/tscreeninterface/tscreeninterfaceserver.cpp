// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
//
// Description:
//

#include <test/ctefunitserver.h>
#include "tscreeninterface.h"

_LIT( KServerName, "openwfcscreeninterfacetestserver" );

GLDEF_C const TTestName ServerName()
/**
 * ServerName
 *
 * @return - The TEF server name
 */
	{
	TTestName serverName(KServerName);
	return serverName;
	}

GLDEF_C CTestSuite* CreateTestSuiteL()
/**
 * Create the overall test suite.
 *
 * @return - The top level suite
 */
	{
	START_SUITE;

	ADD_TEST_SUITE( CTestScreenInterface );

	END_SUITE;
	}

GLDEF_C CTestStep* CreateTEFTestStep(const TDesC& , CTEFUnitServer& /* aServer */)
/**
 * Create individual test steps, outside the suite.
 */
	{
	// Initialise test step object to NULL if no TEF step is found
	CTestStep* testStep = NULL;

	return testStep;
	}
