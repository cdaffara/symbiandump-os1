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
 @file The test server for the SAT.
*/

#include <test/ctefunitserver.h>
#include <test/tefunit.h>

#include "ccsatprovidelocalinfofu.h"
#include "ccsatsendussdfu.h"
#include "ccsatframesfu.h"
#include "ccsatsetupeventlistfu.h"
#include "ccsatrefreshfu.h"
#include "ccsatsetupmenufu.h"
#include "ccsatdisplaytextfu.h"
#include "ccsatgetinputfu.h"
#include "ccsatgetinkeyfu.h"
#include "ccsatsendsmsfu.h"
#include "ccsatsetupcallfu.h"
#include "ccsatopenchannelfu.h"
#include "ccsatclosechannelfu.h"
#include "ccsatmultimediamsgfu.h"
#include "ccsatlanguagenotificationfu.h"
#include "ccsatselectitemfu.h"
#include "ccsatsetupidlemodetextfu.h"
#include "ccsatsendssfu.h"
#include "ccsatsendreceivedatafu.h"
#include "ccsatsenddtmffu.h"
#include "ccsatgetchannelstatusfu.h"
#include "ccsatplaytonefu.h"
#include "ccsatlaunchbrowserfu.h"


// The server name: must be the same that the exe name
_LIT( KServerName, "te_csat_component" );

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
	START_SUITE
	
	ADD_TEST_SUITE(CCSatProvideLocalInfoFU)
	ADD_TEST_SUITE(CCSatSendUSSDFU)
	ADD_TEST_SUITE(CCSatFramesFU)
	ADD_TEST_SUITE(CCSatSetUpEventListFU)
	ADD_TEST_SUITE(CCSatRefreshFU)
	ADD_TEST_SUITE(CCSatSetUpMenuFU)
	ADD_TEST_SUITE(CCSatDisplayTextFU)	
	ADD_TEST_SUITE(CCSatGetInputFU)	
	ADD_TEST_SUITE(CCSatGetInkeyFU)	
	ADD_TEST_SUITE(CCSatSendSMSFU)
	ADD_TEST_SUITE(CCSatSetUpCallFU)	
	ADD_TEST_SUITE(CCSatOpenChannelFU)	
	ADD_TEST_SUITE(CCSatCloseChannelFU)	
	ADD_TEST_SUITE(CCSatMultimediaMsgFU)	
	ADD_TEST_SUITE(CCSatLanguageNotificationFU)	
	ADD_TEST_SUITE(CCSatSelectItemFU)	
	ADD_TEST_SUITE(CCSatSetUpIdleModeTextFU)	
	ADD_TEST_SUITE(CCSatSendSSFU)	
	ADD_TEST_SUITE(CCSatSendReceiveDataFU)
	ADD_TEST_SUITE(CCSatSendDTMFFU)
	ADD_TEST_SUITE(CCSatGetChannelStatusFU)	
	ADD_TEST_SUITE(CCSatPlayToneFU)	
	ADD_TEST_SUITE(CCSatLaunchBrowserFU)	
	
	END_SUITE
	}

// Require: create std TEF test case
GLDEF_D CTestStep* CreateTEFTestStep(const TDesC& /*aStepName*/, CTEFUnitServer& /*aServer*/)
	{
	// This allows std TEF test steps to be created and run inside TEFUnit
	return NULL;
	}
