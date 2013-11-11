// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "globalsettings.h"
#include "screenselect.h"
#include "wsdynamicresbase.h"
#include "wsdynamicresbasic.h"

_LIT( KServerName, "wsdynamicrestestserver" );

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
	ADD_TEST_SUITE( CWsDynamicResBasic );
	ADD_TEST_SUITE( CGlobalSettings );
	END_SUITE;
	}

_LIT(KSelectScreen0, "SelectScreen0");
_LIT(KSelectScreen1, "SelectScreen1");

GLDEF_C CTestStep* CreateTEFTestStep(const TDesC& aStepName, CTEFUnitServer& /* aServer */)
/**
 * Create individual test steps, outside the suite.
 */
	{
	// Initialise test step object to NULL if no TEF step is found
	CTestStep* testStep = NULL;

	if (aStepName == KSelectScreen0)
		testStep = new CScreenSelect(0, aStepName);
	else if (aStepName == KSelectScreen1)
		testStep = new CScreenSelect(1, aStepName);

	return testStep;
	}

