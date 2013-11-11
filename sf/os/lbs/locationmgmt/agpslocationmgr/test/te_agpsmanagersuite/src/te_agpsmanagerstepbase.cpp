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
 @file te_agpsmanagerstepbase.cpp
*/

#include "te_agpsmanagerstepbase.h"
#include "te_agpsmanagersuitedefs.h"


TVerdict CTe_AgpsManagerStepBase::doTestStepPreambleL()
	{
	// START MEMORY TEST
	__UHEAP_MARK;

	// Create and install the active scheduler
	// Construct a new ActiveScheduler and install it
	iActiveScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);

	static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	// Property used by the test to pass hardware capabilities
	// infomation to the Manager's main logic
	TUint err = RProperty::Define(ETestCapabilitiesKey,RProperty::EInt,KAllowAllPolicy,KAllowAllPolicy);

	err = iCapabilitiesProperty.Attach(KManagerTestSuiteUid,ETestCapabilitiesKey,EOwnerThread);
	ASSERT(KErrNone == err);

	// By default, configure TerminalBased capabilities. The derived test step will overwrite this.
	__ASSERT_ALWAYS(KErrNone == iCapabilitiesProperty.Set(KManagerTestSuiteUid,
														  CTe_AgpsManagerStepBase::ETestCapabilitiesKey,
														  TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased),
														  User::Invariant());

    TPositionExtendedSatelliteInfo posSatInfo;
	RLbsPositionUpdates::InitializeL(posSatInfo); 	//	used to publish position
		
	TPositionModuleStatus defaultPositionModuleStatus;
   	RLbsModuleStatus::InitializeL(defaultPositionModuleStatus);
		
	RLbsPositionUpdateRequests::InitializeL();
	RLbsNetworkRegistrationStatus::InitializeL();
	RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelAGPS2NG);
	RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2AGPS);
	// RAssistanceDataCache::InitializeL();
	// Buses for satellite measurement updates and for
	// partial updates not needed by any test yet
	// RLbsGpsMeasurementUpdates::InitializeL();

	
	// Create an instance of the AGPS Manager for testing.
	//
	iAgpsManager = CManagerMainLogic::NewL();

	iGpsModuleObserver = CTestGpsModuleObserver::NewL();
	iNetworkGatewayChannel= CTestAgpsToNgObserver::NewL();
	iLocationUpdatesObserver = CTestAgpsUpdatesObserver::NewL();
	iLsToManagerRequestInjector = CTestAgpsRequestSender::NewL(KLbsLocServerUidValue);
	iNrhToManagerRequestInjector = CTestAgpsRequestSender::NewL(KLbsNetRequestHandlerUidValue);

	// CManagerMainLogic must have been instantiated (above) for this to work: 
	iGpsToManagerMessageInjector = UnitTestGpsModuleGlobals::G_TestGpsModulePtr;
	iGpsToManagerMessageInjector->SetTestObserver(iGpsModuleObserver);
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_AgpsManagerStepBase::doTestStepPostambleL()
	{
	delete iActiveScheduler;
	delete iAgpsManager;
	delete iGpsModuleObserver;
	delete iNetworkGatewayChannel;
	delete iLocationUpdatesObserver;
	delete iLsToManagerRequestInjector;
	delete iNrhToManagerRequestInjector;

	// No need to delete iGpsToManagerMessageInjector because it is deleted when
	// iAgpsManager is deleted
	
	
	RLbsModuleStatus::ShutDownL();
	RLbsPositionUpdateRequests::ShutDownL();
	RLbsNetworkRegistrationStatus::ShutDownL();
	RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelAGPS2NG);
	RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNG2AGPS);
	//RAssistanceDataCache::ShutDownL();
	//RLbsGpsMeasurementUpdates::ShutDownL();

	__ASSERT_ALWAYS(KErrNone == RProperty::Delete(ETestCapabilitiesKey), User::Invariant());//failed to delete property?
		
	// END MEMORY TEST
	__UHEAP_MARKEND;
	return TestStepResult();
	}

CTe_AgpsManagerStepBase::~CTe_AgpsManagerStepBase()
	{
	}

CTe_AgpsManagerStepBase::CTe_AgpsManagerStepBase()
	{

	}
