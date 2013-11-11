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
 @file te_agpsmanagerstep16.cpp
*/
 
#include "te_agpsmanagerstep16.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep16::~CTe_AgpsManagerStep16()
	{
	}

CTe_AgpsManagerStep16::CTe_AgpsManagerStep16()
	{
	SetTestStepName(KAgpsManagerStep16);
	}

TVerdict CTe_AgpsManagerStep16::doTestStepPreambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// Active LS Mode selection.
// This test checks that when the Location Server issues
// a Location Request with Autonomous Positioning method
// after a Location Request with Terminal Assisted or Terminal Based
// method, the GPS Module is not asked to switch to Autonomous.
// (This test is based on the Decision Table for Active LS Mode in 
// LBS Architectural Description)
//
TVerdict CTe_AgpsManagerStep16::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
		CLbsAdmin* admin;
		TLbsGpsOptions gpsOptions;
		TTime time;
		TLbsLocRequestQualityInt quality;

		// Configure H/W Capabilities as simultaneous Terminal Based and Terminal Assisted
		__ASSERT_ALWAYS(KErrNone == iCapabilitiesProperty.Set(KManagerTestSuiteUid,
															  CTe_AgpsManagerStepBase::ETestCapabilitiesKey,
															  TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB),
															  User::Invariant());
		
		// Set admin gps mode to Terminal Based. Get current GPS mode so
		// that admin can be returned to its original value at the end of the test
		//
		admin = CLbsAdmin::NewL();
		CleanupStack::PushL(admin);
		CLbsAdmin::TGpsMode originalMode; 
		admin->Get(KLbsSettingRoamingGpsMode, originalMode);
		admin->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsAlwaysTerminalBased);

		//Inject a location request from LS for Terminal Based method		
		iLsToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTb);
		// Verify that the Manager called SetGpsOptions to set Terminal Based followed by a location
		// request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalBased == gpsOptions.GpsMode(), User::Invariant());
		
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Check that if the GPS Module requests assistance data then a new SelfLocationRequest
		// is sent to the Network Gateway
		iGpsToManagerMessageInjector->InjectAssistanceDataRequest(EAssistanceDataReferenceLocation);
		iNetworkGatewayChannel->WaitForCallBack();
		__ASSERT_ALWAYS(iNetworkGatewayChannel->WasSystemStatusAdvised(), User::Invariant());
		iNetworkGatewayChannel->WaitForCallBack();
		__ASSERT_ALWAYS(iNetworkGatewayChannel->WasSelfLocationRequested(), User::Invariant());
		
		// Check that a new assistance data request results in
		// an assistance data request sent to the NG...
		iGpsToManagerMessageInjector->InjectAssistanceDataRequest(EAssistanceDataReferenceTime);
		iNetworkGatewayChannel->WaitForCallBack();
		__ASSERT_ALWAYS(iNetworkGatewayChannel->WasAssistanceDataRequested(), User::Invariant());
		
		// Inject a location request from LS for Autonomous positioning
		iLsToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodAut);
		// Verify that the Manager does not call SetGpsOptions to set Terminal Based followed by a location
		// request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(!iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Terminate the currently ongoing network session
		iNetworkGatewayChannel->SendSessionCompleteToAgpsManager(KErrNone);
		
		// Send a new LS Location Request for autonomous and check
		// that this time the GPS Module is set for Autonomous
		iLsToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodAut);
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsAutonomous == gpsOptions.GpsMode(), User::Invariant());

		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Check that the next time the GPS module requests assistance data
		// a new selflocationrequest(autonomous) is sent to the module
		iGpsToManagerMessageInjector->InjectAssistanceDataRequest(EAssistanceDataReferenceLocation);
		//iNetworkGatewayChannel->WaitForCallBack();
		//__ASSERT_ALWAYS(iNetworkGatewayChannel->WasSystemStatusAdvised(), User::Invariant());
		iNetworkGatewayChannel->WaitForCallBack();
		__ASSERT_ALWAYS(iNetworkGatewayChannel->WasSelfLocationRequested(), User::Invariant());

		// Reset Admin to to value it had before the test changed it
		admin->Set(KLbsSettingRoamingGpsMode, originalMode);

		CleanupStack::PopAndDestroy(admin);
		}
	  return TestStepResult();
	}

TVerdict CTe_AgpsManagerStep16::doTestStepPostambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
