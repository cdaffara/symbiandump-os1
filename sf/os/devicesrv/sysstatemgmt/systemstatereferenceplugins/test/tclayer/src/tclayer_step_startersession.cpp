// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/ssmstates.hrh>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmstatemanager.h>

// Include the header file for the class under test
#include <ssm/starterclient.h>
#include <ssm/startupdomainpskeys.h>
#include <e32property.h>

#include "ssmclayer.h"
#include "startupadaptationcommands.h"

#include "ssmdebug.h"

#include "tclayer_step_startersession.h"

//Include the test wrappers
#include "adaptationcliwrapper.h"
#include "ssmcmnwrapper.h"
#include "featurediscoverywrapper.h"

/**
 * Constructs a complete CClayerTestStarterSession object
 * @internalComponent
 */
CCLayerTestStarterSession::CCLayerTestStarterSession()
	{
	
	}

/**
 * Releases any resources associated with this test
 * @internalComponent
 */
CCLayerTestStarterSession::~CCLayerTestStarterSession()
	{
	
	}

/**
 * Test step preamble
 * @internalComponent
 */
TVerdict CCLayerTestStarterSession::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/**
 * Test step - a single test step performs a series of tests.
 * 
 * See the individual doTest... methods on this class for details of the tests.
 * 
 * All these tests use a variety of wrapper classes which provide equivalent methods 
 * for all the APIs that RStarterSession uses. These wrapper classes can be requested
 * (via static methods in the appropriate wrapper class) to modify their behaviour,
 * such as return errors. This allows the internals and error handling to be easily tested. 
 * 
 * @internalComponent
 */
TVerdict CCLayerTestStarterSession::doTestStepL()
	{
	INFO_PRINTF1(_L("Entering test for compatibility layer starter session"));
	__UHEAP_MARK;
	doTestConnectAndClose();
	doTestSetState();
	doTestRequestReset();
	doTestRequestShutdown();
	doTestRfForEmergencyCall();
	doTestEndSplashScreen();
	
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("Leaving test for compatibility layer starter session"));
	return TestStepResult();
	}

/**
 * Test step postamble
 * @internalComponent
 */
TVerdict CCLayerTestStarterSession::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		AFSS-CLAYER-0001
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0001
 */		

void CCLayerTestStarterSession::doTestConnectAndClose()
	{
	INFO_PRINTF1(_L("Entering CCLayerTestStarterSession::doTestConnectAndClose()"));
	INFO_PRINTF1(_L("Close on undisconnected RStartSession"));
	RStarterSession starterSession;
	starterSession.Close();
	
	// Set the next error returned from RSsmStateManager wrapper
	// This is to test that RStarterSession handles the return value from RSsmStateManager correctly
	SsmCmnWrapper::SetSsmStateManagerNextConnectReturn(KErrNotFound);
	
	INFO_PRINTF1(_L("Connect on RStarterSession with RSsmStateManger::Connect() failing"));
	TInt ret = starterSession.Connect();
	TEST(ret == KErrNotFound);
	
	// Set the next error returned from RSsmStateManger
	SsmCmnWrapper::SetSsmStateManagerNextConnectReturn(KErrNone);
	
	INFO_PRINTF1(_L("Connect on RStarterSession with RSsmStateManger::Connect() successful"));
	ret = starterSession.Connect();
	TEST(ret == KErrNone);
	// Check that the session connected correctly and copied across the handle
	TEST(starterSession.Handle() != KNullHandle);
	TEST(starterSession.Handle() == SsmCmnWrapper::SsmStateManagerHandle());

	
	INFO_PRINTF1(_L("Close on connected RStartSession"));
	starterSession.Close();
	TEST(starterSession.Handle() == KNullHandle);
	
	INFO_PRINTF1(_L("Double close on connected RStartSession"));
	starterSession.Close();
	TEST(starterSession.Handle() == KNullHandle);
	
	INFO_PRINTF1(_L("Exiting CCLayerTestStarterSession::doTestConnectAndClose()"));
	}

/**
 * This adjustment specifies how to convert start-up sub-states
 * to the states defined in StartupAdaptation::TGlobalState
 * 
 * It is calculated using the same method specified in CSaaStateAdaptation::MapToStartupAdaptationState(),
 * see that method for further details.
 * 
 * @internalComponent
 */
const TInt KStartupSubStateAdjustment = static_cast<TInt>(StartupAdaptation::ESWStateStartingUiServices) 
									- static_cast<TInt>(ESsmStartupSubStateNonCritical);

/**
Old Test CaseID 		AFSS-CLAYER-0002
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0001
 */		

void CCLayerTestStarterSession::doTestSetState()
	{
	INFO_PRINTF1(_L("Entering CCLayerTestStarterSession::doTestSetState()"));
	
	INFO_PRINTF1(_L("Connecting RStarterSession"));
	RStarterSession starterSession;
	TInt ret = starterSession.Connect();
	TEST(ret == KErrNone);
	TEST(starterSession.Handle() != KNullHandle);
	TEST(starterSession.Handle() == SsmCmnWrapper::SsmStateManagerHandle());
	
	INFO_PRINTF1(_L("Calling SetState(EAlarm)"));
	ret = starterSession.SetState(RStarterSession::EAlarm);
	TEST(ret == KErrNone);
	TEST(SsmCmnWrapper::GetLastMainStateRequest() == ESsmStartup);
	TEST(SsmCmnWrapper::GetLastSubStateRequest() == StartupAdaptation::ESWStateChargingToAlarm - KStartupSubStateAdjustment);
	    
	INFO_PRINTF1(_L("Calling SetState(ECharging)"));
	ret = starterSession.SetState(RStarterSession::ECharging);
	TEST(ret == KErrNone);
	TEST(SsmCmnWrapper::GetLastMainStateRequest() == ESsmStartup);
	TEST(SsmCmnWrapper::GetLastSubStateRequest() == StartupAdaptation::ESWStateAlarmToCharging - KStartupSubStateAdjustment);
		  
	INFO_PRINTF1(_L("Calling SetState(-1)"));
	ret = starterSession.SetState(static_cast<RStarterSession::TGlobalState>(-1));
	TEST(ret == KErrArgument);
	
	INFO_PRINTF1(_L("Close on connected RStartSession"));
	starterSession.Close();
	TEST(starterSession.Handle() == KNullHandle);
	
	INFO_PRINTF1(_L("Exiting CCLayerTestStarterSession::doTestSetState()"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0003
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0001
 */		

void CCLayerTestStarterSession::doTestRequestReset()
	{
	INFO_PRINTF1(_L("Entering CCLayerTestStarterSession::doTestRequestReset()"));
	
	INFO_PRINTF1(_L("Connecting RStarterSession"));
	RStarterSession starterSession;
	TInt ret = starterSession.Connect();
	TEST(ret == KErrNone);
	TEST(starterSession.Handle() != KNullHandle);
	TEST(starterSession.Handle() == SsmCmnWrapper::SsmStateManagerHandle());
	
	
	INFO_PRINTF1(_L("Calling Reset(ELanguageSwitchReset)"));
	starterSession.Reset(RStarterSession::ELanguageSwitchReset);
	TEST(SsmCmnWrapper::GetLastMainStateRequest() == ESsmShutdown);
	TEST(SsmCmnWrapper::GetLastSubStateRequest() == KSsmAnySubState);
	TEST(SsmCmnWrapper::GetLastReasonRequest() == RStarterSession::ELanguageSwitchReset);
	
	INFO_PRINTF1(_L("Calling Reset(EOperatorSettingReset)"));
	starterSession.Reset(RStarterSession::EOperatorSettingReset);
	TEST(SsmCmnWrapper::GetLastMainStateRequest() == ESsmShutdown);
	TEST(SsmCmnWrapper::GetLastSubStateRequest() == KSsmAnySubState);
	TEST(SsmCmnWrapper::GetLastReasonRequest() == RStarterSession::EOperatorSettingReset);
	
	INFO_PRINTF1(_L("Calling Reset(EFirmwareUpdate)"));
	starterSession.Reset(RStarterSession::EFirmwareUpdate);
	TEST(SsmCmnWrapper::GetLastMainStateRequest() == ESsmShutdown);
	TEST(SsmCmnWrapper::GetLastSubStateRequest() == KSsmAnySubState);
	TEST(SsmCmnWrapper::GetLastReasonRequest() == RStarterSession::EFirmwareUpdate);
	
	INFO_PRINTF1(_L("Close on connected RStartSession"));
	starterSession.Close();
	TEST(starterSession.Handle() == KNullHandle);
	
	INFO_PRINTF1(_L("Exiting CCLayerTestStarterSession::doTestRequestReset()"));
	}


/**
Old Test CaseID 		AFSS-CLAYER-0004
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0001
 */		

void CCLayerTestStarterSession::doTestRequestShutdown()
	{
	INFO_PRINTF1(_L("Entering CCLayerTestStarterSession::doTestRequestShutdown()"));

	INFO_PRINTF1(_L("Connecting RStarterSession"));
	RStarterSession starterSession;
	TInt ret = starterSession.Connect();
	TEST(ret == KErrNone);
	TEST(starterSession.Handle() != KNullHandle);
	TEST(starterSession.Handle() == SsmCmnWrapper::SsmStateManagerHandle());
	
	INFO_PRINTF1(_L("Calling Shutdown()"));
	starterSession.Shutdown();
	TEST(SsmCmnWrapper::GetLastMainStateRequest() == ESsmShutdown);
	TEST(SsmCmnWrapper::GetLastSubStateRequest() == KSsmAnySubState);
	TEST(SsmCmnWrapper::GetLastReasonRequest() == KSsmCLayerPowerOffReason);
	
	INFO_PRINTF1(_L("Close on connected RStartSession"));
	starterSession.Close();
	TEST(starterSession.Handle() == KNullHandle);
	
	INFO_PRINTF1(_L("Exiting CCLayerTestStarterSession::doTestRequestShutdown()"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0007
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0001
 */		

void CCLayerTestStarterSession::doTestRfForEmergencyCall()
	{
	INFO_PRINTF1(_L("Entering CCLayerTestStarterSession::doTestRfForEmergencyCall()"));

	INFO_PRINTF1(_L("Connecting RStarterSession"));
	RStarterSession starterSession;
	TInt ret = starterSession.Connect();
	TEST(ret == KErrNone);
	TEST(starterSession.Handle() != KNullHandle);
	TEST(starterSession.Handle() == SsmCmnWrapper::SsmStateManagerHandle());
	
	INFO_PRINTF1(_L("Failure to connect to RSsmEmergencyCallRfAdaptation"));
	// Set up connection to fail
	AdaptationCliWrapper::SetRSsmAdaptationBaseNextConnectReturn(KErrPermissionDenied);
	ret = starterSession.ActivateRfForEmergencyCall();
	TEST(ret == KErrPermissionDenied);
	// Allow connection to succeed on the next tests
	AdaptationCliWrapper::SetRSsmAdaptationBaseNextConnectReturn(KErrNone);
	
	INFO_PRINTF1(_L("Activation and deactivation of RF for emergency calls"));
	ret = starterSession.ActivateRfForEmergencyCall();
	TEST(ret == KErrNone);
	ret = starterSession.DeactivateRfAfterEmergencyCall();
	TEST(ret == KErrNone);
	
	INFO_PRINTF1(_L("Activation and deactivation of RF for emergency calls not supported"));
	AdaptationCliWrapper::SetRSsmEmergencyCallReturnValue(KErrNotSupported);
	ret = starterSession.ActivateRfForEmergencyCall();
	TEST(ret == KErrNotSupported);
	ret = starterSession.DeactivateRfAfterEmergencyCall();
	TEST(ret == KErrNotSupported);
	// Allow future calls to succeed
	AdaptationCliWrapper::SetRSsmEmergencyCallReturnValue(KErrNone);	
	
	INFO_PRINTF1(_L("Close on connected RStartSession"));
	starterSession.Close();
	TEST(starterSession.Handle() == KNullHandle);
	
	INFO_PRINTF1(_L("Exiting CCLayerTestStarterSession::doTestRfForEmergencyCall()"));
	}

/**
Old Test CaseID 		AFSS-CLAYER-0008
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CLAYER-0001
 */		

void CCLayerTestStarterSession::doTestEndSplashScreen()
	{
	INFO_PRINTF1(_L("Entering CCLayerTestStarterSession::doTestEndSplashScreen()"));

	INFO_PRINTF1(_L("Connecting RStarterSession"));
	RStarterSession starterSession;
	TInt ret = starterSession.Connect();
	TEST(ret == KErrNone);
	TEST(starterSession.Handle() != KNullHandle);
	TEST(starterSession.Handle() == SsmCmnWrapper::SsmStateManagerHandle());
	
	INFO_PRINTF1(_L("Deleting splash screen property"));
	ret = RProperty::Delete(KPSUidStartup, KPSSplashShutdown);
	TEST(ret == KErrNone || ret == KErrNotFound);
	INFO_PRINTF2(_L("ret val is : %d"), ret);
		
	
	INFO_PRINTF1(_L("Ending splashscreen without defining property"));
	ret = starterSession.EndSplashScreen();
	TEST(ret == KErrNotFound);
	INFO_PRINTF2(_L("ret val is : %d"), ret);
	
	INFO_PRINTF1(_L("Defining splash screen property"));
	// In a test enviroment we don't need to worry about protecting the property
	// so allow anything to change it
	_LIT_SECURITY_POLICY_PASS(KAllPass); //lint !e648 Suppress overflow in computing constant for operation
	ret = RProperty::Define(KPSUidStartup, KPSSplashShutdown, RProperty::EInt, KAllPass, KAllPass);
	INFO_PRINTF3(_L("KPSUidStartup = %x, KPSSplashShutdown = %x"), KPSUidStartup, KPSSplashShutdown);
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("ret val is : %d"), ret);
	
	INFO_PRINTF1(_L("Ending splashscreen with property defined"));
	ret = starterSession.EndSplashScreen();
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("ret val is : %d"), ret);
	
	INFO_PRINTF1(_L("Reading splash screen property"));
	TInt value;
	ret = RProperty::Get(KPSUidStartup, KPSSplashShutdown, value);
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("ret val is : %d"), ret);
	TEST(value == ESplashShutdown);
	INFO_PRINTF3(_L("Property value is : %d .Expected : %d"), ret, ESplashShutdown);
	
	INFO_PRINTF1(_L("Close on connected RStartSession"));
	starterSession.Close();
	TEST(starterSession.Handle() == KNullHandle);
	
	INFO_PRINTF1(_L("Exiting CCLayerTestStarterSession::doTestEndSplashScreen()"));
	}

