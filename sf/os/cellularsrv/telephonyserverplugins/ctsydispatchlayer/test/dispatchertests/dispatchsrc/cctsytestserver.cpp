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
 @file The test server for the Common TSY.
*/

#include <test/ctefunitserver.h>
#include <test/tefunit.h>


#include "cctsycellbroadcastfu.h"
#include "cctsycallcontrolmultipartyfu.h"
#include "cctsycallcontrolfu.h"
#include "cctsycellbroadcastfu.h"
#include "cctsypacketservicesfu.h"
#include "cctsyphonebookfu.h"
#include "cctsyphonebookenfu.h"
#include "cctsyphonebookonfu.h"
#include "cctsyphonefu.h"
#include "cctsysatfu.h"
#include "cctsysecurityfu.h"
#include "cctsysimfu.h"
#include "cctsysmsfu.h"
#include "cctsysupplementaryservicesfu.h"
#include "cctsyphonebookenfunegative.h"
#include "cctsycallcontrolfunegative.h"
#include "cctsypacketservicesfunegative.h"
#include "cctsyphonebookfunegative.h"
#include "cctsyphonefunegative.h"	
#include "cctsysecurityfunegative.h"
#include "cctsysupplementaryservicesfunegative.h"
#include "cctsyphonebookonfunegative.h"
#include "cctsycellbroadcastfunegative.h"
#include "cctsysimfunegative.h"
#include "cctsysmsfunegative.h"
#include "cctsycallcontrolmultipartyfunegative.h"
#include "cctsysatfunegative.h"
#include "cctsypacketcontextfu.h"

// The server name: must be the same that the exe name
_LIT( KServerName, "te_ctsydispatcher_component" );

/**
 * ServerName
 *
 * @return The TEF server name.
 */
GLDEF_C const TTestName ServerName()

	{
	TTestName serverName(KServerName);
	return serverName;
	}

/**
 * @return The top level suite.
 */
GLDEF_C CTestSuite* CreateTestSuiteL()
	{
	START_SUITE;

	ADD_TEST_SUITE(CCTsyPhoneFU);
	ADD_TEST_SUITE(CCTsyCallControlMultipartyFU);
	ADD_TEST_SUITE(CCTsyCallControlFU);
	ADD_TEST_SUITE(CCTsyCellBroadcastFU);
	ADD_TEST_SUITE(CCTsyPacketServicesFU);
	ADD_TEST_SUITE(CCTsyPhonebookFU);
	ADD_TEST_SUITE(CCTsyPhonebookEnFU);
	ADD_TEST_SUITE(CCTsyPhonebookOnFU);
	ADD_TEST_SUITE(CCTsySatFU);
	ADD_TEST_SUITE(CCTsySecurityFU);
	ADD_TEST_SUITE(CCTsySimFU);
	ADD_TEST_SUITE(CCTsySmsFU);
	ADD_TEST_SUITE(CCTsySupplementaryServicesFU);
	
	ADD_TEST_SUITE(CCTsyPhonebookEnFUNegative);
	ADD_TEST_SUITE(CCTsyCallControlFUNegative);
	ADD_TEST_SUITE(CCTsyPacketServicesFUNegative);
	ADD_TEST_SUITE(CCTsyPhonebookFUNegative);
	ADD_TEST_SUITE(CCTsyPhoneFUNegative);
	ADD_TEST_SUITE(CCTsySecurityFUNegative);
	ADD_TEST_SUITE(CCTsySupplementaryServicesFUNegative);
	
	ADD_TEST_SUITE(CCTsyPhonebookOnFUNegative);
	ADD_TEST_SUITE(CCTsyCellBroadcastFUNegative);
	ADD_TEST_SUITE(CCTsySimFUNegative);
	ADD_TEST_SUITE(CCTsySmsFUNegative);
	ADD_TEST_SUITE(CCTsyCallControlMultipartyFUNegative);
	ADD_TEST_SUITE(CCTsySatFUNegative);
	ADD_TEST_SUITE(CCTsyPacketContextFU);

	END_SUITE;
	}

// Require: create std TEF test case
GLDEF_D CTestStep* CreateTEFTestStep(const TDesC& /*aStepName*/, CTEFUnitServer& /*aServer*/)
	{
	// This allows std TEF test steps to be created and run inside TEFUnit
	return NULL;
	}
