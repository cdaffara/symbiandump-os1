// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <test/ctefunitserver.h>
#include "openwftest.h"

_LIT( KServerName, "openwftestserver" );

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

    ADD_TEST_SUITE( COpenwfTest );

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
