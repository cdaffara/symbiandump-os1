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
 @file te_agpsmanagerstep17.cpp
*/
 
#include "te_agpsmanagerstep17.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep17::~CTe_AgpsManagerStep17()
	{
	}

CTe_AgpsManagerStep17::CTe_AgpsManagerStep17()
	{
	SetTestStepName(KAgpsManagerStep17);
	}

TVerdict CTe_AgpsManagerStep17::doTestStepPreambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// Active LS Mode change results in new self location request.
// This test checks that a change in the active LS GPS Mode results
// in a Self Location Request sent to the Network Gateway.
//
TVerdict CTe_AgpsManagerStep17::doTestStepL()
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

		// Inject a location request from LS for Terminal Based method		
		iLsToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodAut);
		// Verify that the Manager called SetGpsOptions to set Terminal Based followed by a location
		// request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsAutonomous == gpsOptions.GpsMode(), User::Invariant());
		
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
		
		
		//...but if the Location Server changes the mode in the next Location Request...
		admin->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsPreferTerminalBased);
		iLsToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTb);
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalBased == gpsOptions.GpsMode(), User::Invariant());
		
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());
		
		//...the next time the GPS module wants assistance data...
		iGpsToManagerMessageInjector->InjectAssistanceDataRequest(EAssistanceDataIonosphericModel);
		
		//...a SelfLocationRequest is sent to the Network Gateway (instead of the 
		// assistance data request)
		iNetworkGatewayChannel->WaitForCallBack();
		__ASSERT_ALWAYS(iNetworkGatewayChannel->WasSelfLocationRequested(), User::Invariant());
		
		// Check that, after a new request from the LS, a new assistance data request from the module 
		// with the same mode (TB) results in an assistance data request sent to the NG
		iLsToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTb);
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		iGpsToManagerMessageInjector->InjectAssistanceDataRequest(EAssistanceDataReferenceTime);
		iNetworkGatewayChannel->WaitForCallBack();
		__ASSERT_ALWAYS(iNetworkGatewayChannel->WasAssistanceDataRequested(), User::Invariant());

		// Ensure that no other messages were sent to the GPS module or the 
		// Network Gateway
		__ASSERT_ALWAYS(!iGpsModuleObserver->IsMoreActivityObserved(), User::Invariant());
		__ASSERT_ALWAYS(!iNetworkGatewayChannel->IsMoreActivityObserved(), User::Invariant());
		
		// Reset Admin to to value it had before the test changed it
		admin->Set(KLbsSettingRoamingGpsMode, originalMode);

		CleanupStack::PopAndDestroy(admin);
		}
	  return TestStepResult();
	}

TVerdict CTe_AgpsManagerStep17::doTestStepPostambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
