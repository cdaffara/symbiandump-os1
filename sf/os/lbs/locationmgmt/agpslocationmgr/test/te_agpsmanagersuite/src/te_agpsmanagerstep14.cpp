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
 @file te_agpsmanagerstep14.cpp
*/
 
#include "te_agpsmanagerstep14.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep14::~CTe_AgpsManagerStep14()
	{
	}

CTe_AgpsManagerStep14::CTe_AgpsManagerStep14()
	{
	SetTestStepName(KAgpsManagerStep14);
	}

TVerdict CTe_AgpsManagerStep14::doTestStepPreambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

// Valid Combinations of Positioning Methods in Location
// Requests from LS and NRH with TAorTb H/W.
// This test ensures that, for hardware capable of mutually 
// exclusive Terminal Based or Terminal Assisted positioning,
// when both the LS and NRH issue a Location Request with a
// valid positioning method (Autonomous, Terminal Based or Terminal
// Assisted), the GPS Module receives a call to set new gps
// options with that positioning method.
//
TVerdict CTe_AgpsManagerStep14::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
		CLbsAdmin* admin;
		TLbsGpsOptions gpsOptions;
		TTime time;
		TLbsLocRequestQualityInt quality;

		// Set H/W Capabilities to Terminal Based OR Terminal Assisted
		__ASSERT_ALWAYS(KErrNone == iCapabilitiesProperty.Set(KManagerTestSuiteUid, CTe_AgpsManagerStepBase::ETestCapabilitiesKey,
				 TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted | TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased),
				 User::Invariant());
		
		// Set admin gps mode to Terminal Based. Get current GPS mode so
		// that admin can be returned to its original value at the end of the test
		//
		admin = CLbsAdmin::NewL();
		CleanupStack::PushL(admin);
		CLbsAdmin::TGpsMode originalMode; 
		admin->Get(KLbsSettingRoamingGpsMode, originalMode);
		admin->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsPreferTerminalBased);

		//*******************************************************************************************
		//************* LS: Autonomous; NRH: Autonomous *********************************************
		//*******************************************************************************************
		//Inject a location request from LS for Autonomous method		
		iLsToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodAut);
		// Verify that the Manager called SetGpsOptions to set autonomous followed by a location
		// request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsAutonomous == gpsOptions.GpsMode(), User::Invariant());
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());
	
		// Inject a location request from NRH for method Autonomous
		iNrhToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodAut);
		// Verify that the Manager sent to the module a location request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Cancel NRH location request
		iNrhToManagerRequestInjector->CancelPositionUpdate();
		// Verify new location request with only LS requirements sent to GPS Module
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		//*******************************************************************************************
		//************* LS: Autonomous; NRH: Terminal Based  ****************************************
		//*******************************************************************************************
		
		// Inject a location request from NRH for method Terminal Based (there is an outstanding 
		// LS request for autonomous)
		iNrhToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTb);
		// Verify that the Manager sent to the module new GPS options and a location request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalBased == gpsOptions.GpsMode(), User::Invariant());
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Cancel NRH location request
		iNrhToManagerRequestInjector->CancelPositionUpdate();
		// Verify a new GPS mode (autonomous) is sent to the module as well as
		// a new location request (only LS requirements sent to GPS Module)
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsAutonomous == gpsOptions.GpsMode(), User::Invariant());
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		//*******************************************************************************************
		//************* LS: Autonomous; NRH: Terminal Assisted  ****************************************
		//*******************************************************************************************
		
		// Switch admin to TA
		admin->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsPreferTerminalAssisted);
		

		// Inject a location request from NRH for method Terminal Based (there is an outstanding 
		// LS request for autonomous)
		iNrhToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTa);
		// Verify that the Manager sent to the module new GPS options and a location request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalAssisted == gpsOptions.GpsMode(), User::Invariant());
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Cancel NRH location request
		iNrhToManagerRequestInjector->CancelPositionUpdate();
		// Verify a new GPS mode (autonomous) is sent to the module as well as
		// a new location request (only LS requirements sent to GPS Module)
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsAutonomous == gpsOptions.GpsMode(), User::Invariant());
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Cancel LS location request
		iLsToManagerRequestInjector->CancelPositionUpdate();
		// Expect a cancel sent to the GPS Module
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateCancelled(), User::Invariant());
		
		// Switch admin to TB
		admin->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsPreferTerminalBased);
		
		//*******************************************************************************************
		//************* LS: Terminal Based; NRH: Autonomous *****************************************
		//*******************************************************************************************

			
		//Inject a location request from LS for Terminal Based method
		iLsToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTb);
		// Verify that the Manager called SetGpsOptions to set autonomous followed by a location
		// request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalBased == gpsOptions.GpsMode(), User::Invariant());
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());
	
		// Inject a location request from NRH for method Autonomous
		iNrhToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodAut);
		// Verify that the Manager sent to the module a location request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Cancel NRH location request
		iNrhToManagerRequestInjector->CancelPositionUpdate();
		// Verify new location request with only LS requirements sent to GPS Module
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		//*******************************************************************************************
		//************* LS: Terminal Based; NRH: Terminal Based *************************************
		//*******************************************************************************************

		// Inject a location request from NRH for method Terminal Based (there is an outstanding 
		// LS request for Terminal Based positioning)
		iNrhToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTb);
		// Verify that the Manager sent to the module a location request only
		iGpsModuleObserver->WaitForCallBack();
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Cancel NRH location request
		iNrhToManagerRequestInjector->CancelPositionUpdate();
		// Verify new location request with only LS requirements sent to GPS Module
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());


		//*******************************************************************************************
		//************* LS: Terminal Based; NRH: Terminal Assisted *************************************
		//*******************************************************************************************

		// Switch admin to TA
		admin->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsPreferTerminalAssisted);

		// Inject a location request from NRH for method Terminal Based (there is an outstanding 
		// LS request for Terminal Based positioning)
		iNrhToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTa);
		// Verify that the Manager sent to the module new GPS options and a location request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalAssisted == gpsOptions.GpsMode(), User::Invariant());
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Cancel NRH location request
		iNrhToManagerRequestInjector->CancelPositionUpdate();
		// Verify new configuration message for Terminal Based and a
		// new location request with only LS requirements sent to GPS Module
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalBased == gpsOptions.GpsMode(), User::Invariant());
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Cancel LS location request
		iLsToManagerRequestInjector->CancelPositionUpdate();
		// Expect a cancel sent to the GPS Module
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateCancelled(), User::Invariant());
		// Ensure that no other messages were sent to the GPS module
		__ASSERT_ALWAYS(!iGpsModuleObserver->IsMoreActivityObserved(), User::Invariant());


		//*******************************************************************************************
		//************* LS: Terminal Assisted; NRH: Terminal Assisted *******************************
		//*******************************************************************************************
		
		//Inject a location request from LS for Terminal Assisted mode		
		iLsToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTa);
		// Verify that the Manager called SetGpsOptions to set autonomous followed by a location
		// request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalAssisted == gpsOptions.GpsMode(), User::Invariant());
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());
	
		// Inject a location request from NRH for Terminal Assisted method
		iNrhToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTa);
		// Verify that the Manager sent to the module a location request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Cancel NRH location request
		iNrhToManagerRequestInjector->CancelPositionUpdate();
		// Check that a new location request with only LS requirements is sent to GPS Module
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());


		//*******************************************************************************************
		//************* LS: Terminal Assisted; NRH: Autonomous **************************************
		//*******************************************************************************************

		// Inject a location request from NRH for Autonomous method
		iNrhToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodAut);
		// Verify that the Manager sent to the module a configuration message for Autonomous
		// and a new location request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsAutonomous == gpsOptions.GpsMode(), User::Invariant());
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());
		
				// Cancel NRH location request
		iNrhToManagerRequestInjector->CancelPositionUpdate();
		// Verify new configuration message for Terminal Assisted and a
		// new location request with only LS requirements sent to GPS Module
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalAssisted == gpsOptions.GpsMode(), User::Invariant());
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		//*******************************************************************************************
		//************* LS: Terminal Assisted; NRH: Terminal Based **********************************
		//*******************************************************************************************

		// Inject a location request from NRH for Autonomous method
		iNrhToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTb);
		// Verify that the Manager sent to the module a configuration message for Autonomous
		// and a new location request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalBased == gpsOptions.GpsMode(), User::Invariant());
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());
		
		// Cancel NRH location request
		iNrhToManagerRequestInjector->CancelPositionUpdate();
		// Verify new configuration message for Terminal Assisted and a
		// new location request with only LS requirements sent to GPS Module
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalAssisted == gpsOptions.GpsMode(), User::Invariant());
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Cancel LS location request
		iLsToManagerRequestInjector->CancelPositionUpdate();
		// Expect a cancel sent to the GPS Module
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateCancelled(), User::Invariant());
		// Ensure that no other messages were sent to the GPS module
		__ASSERT_ALWAYS(!iGpsModuleObserver->IsMoreActivityObserved(), User::Invariant());
		
		// Reset Admin to to value it had before the test changed it
		admin->Set(KLbsSettingRoamingGpsMode, originalMode);

		CleanupStack::PopAndDestroy(admin);
		}
	  return TestStepResult();
	}

TVerdict CTe_AgpsManagerStep14::doTestStepPostambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
