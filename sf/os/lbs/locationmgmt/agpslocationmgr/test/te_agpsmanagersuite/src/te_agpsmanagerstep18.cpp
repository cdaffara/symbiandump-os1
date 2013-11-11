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
 @file te_agpsmanagerstep18.cpp
*/
 
#include "te_agpsmanagerstep18.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep18::~CTe_AgpsManagerStep18()
	{
	}

CTe_AgpsManagerStep18::CTe_AgpsManagerStep18()
	{
	SetTestStepName(KAgpsManagerStep18);
	}

TVerdict CTe_AgpsManagerStep18::doTestStepPreambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
//LS cancel when NRH active.
//This test checks that when the LS cancels its outstanding request
//the mode of the GPS module is changed by the Manager to the NRH mode.
//
TVerdict CTe_AgpsManagerStep18::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
		CLbsAdmin* admin;
		TLbsGpsOptions gpsOptions;
		TLbsGpsOptionsArray arrayOptions;
		TLbsGpsOptionsItem positionOrMeasurements;
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
		admin->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsPreferTerminalBased);

		// Inject a location request from LS for Terminal Based method		
		iLsToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTb);
		// Verify that the Manager called SetGpsOptions to set Terminal Based followed by a location
		// request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalBased == gpsOptions.GpsMode(), User::Invariant());
		
		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Inject a location request from NRH for method Terminal Assisted (there is an outstanding 
		// LS request for Terminal Based positioning)
		iNrhToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTa);
		// Verify that the Manager sent to the module new GPS options and a location request
		iGpsModuleObserver->WaitForCallBack();
		arrayOptions.ClearOptionItems();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(arrayOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsArrayClass == arrayOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsPreferTerminalBased == arrayOptions.GpsMode(), User::Invariant());
		__ASSERT_ALWAYS(2 == arrayOptions.NumOptionItems(), User::Invariant());
		__ASSERT_ALWAYS(KErrNone == arrayOptions.GetOptionItem(0, positionOrMeasurements), User::Invariant());
		__ASSERT_ALWAYS(TLbsGpsOptionsItem::EPosUpdateCalculation == positionOrMeasurements.PosUpdateType(), User::Invariant());
		__ASSERT_ALWAYS(KErrNone == arrayOptions.GetOptionItem(1, positionOrMeasurements), User::Invariant());
		__ASSERT_ALWAYS(TLbsGpsOptionsItem::EPosUpdateMeasurement == positionOrMeasurements.PosUpdateType(), User::Invariant());

		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());

		// Cancel the LS Location Request 
		// and verify that new GPS options are NOT sent to the module
		// Even though there is now no outstanding request from the LS
		// the mode (hybrid) should be latched
		iLsToManagerRequestInjector->CancelPositionUpdate();
		iGpsModuleObserver->WaitForCallBack();
		
		__ASSERT_ALWAYS(!iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
	
		// Ensure that no other messages were sent to the GPS module
		__ASSERT_ALWAYS(!iGpsModuleObserver->IsMoreActivityObserved(), User::Invariant());

		
		// Reset Admin to to value it had before the test changed it
		admin->Set(KLbsSettingRoamingGpsMode, originalMode);

		CleanupStack::PopAndDestroy(admin);
		}
	  return TestStepResult();
	}

TVerdict CTe_AgpsManagerStep18::doTestStepPostambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
