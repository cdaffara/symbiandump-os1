// Copyright (c) 7008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_agpsmanagerstep7.cpp
*/
 
#include "te_agpsmanagerstep7.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep7::~CTe_AgpsManagerStep7()
	{
	}

CTe_AgpsManagerStep7::CTe_AgpsManagerStep7()
	{
	SetTestStepName(KAgpsManagerStep7);
	}

TVerdict CTe_AgpsManagerStep7::doTestStepPreambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// Valid Positioning Method from the NRH in TerminalAssisted H/W.
// This test ensures that, for Terminal Assisted capable hardware,
// when the NRH issues a Location Request with a valid
// positioning method (TA), the GPS Module
// receives a call to set new gps options with that positioning method.
//
TVerdict CTe_AgpsManagerStep7::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
		CLbsAdmin* admin;
		TLbsGpsOptions gpsOptions;
		TTime time;
		TLbsLocRequestQualityInt quality;
		
		// Configure H/W as TerminalAssisted capable only
		__ASSERT_ALWAYS(KErrNone == iCapabilitiesProperty.Set(KManagerTestSuiteUid,
															  CTe_AgpsManagerStepBase::ETestCapabilitiesKey,
															  TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted),
															  User::Invariant());

		// Set admin gps mode to Terminal Assisted. Get current GPS mode so
		// that admin can be returned to its original value at the end of the test
		//
		admin = CLbsAdmin::NewL();
		CleanupStack::PushL(admin);
		CLbsAdmin::TGpsMode originalMode; 
		admin->Get(KLbsSettingRoamingGpsMode, originalMode);

		admin->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsAlwaysTerminalAssisted);
		
		iNrhToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTa);
		
		// Verify that the Manager called SetGpsOptions to set Terminal Based followed by a location
		// request
		iGpsModuleObserver->WaitForCallBack();
		__ASSERT_ALWAYS(iGpsModuleObserver->WereGpsOptionsSet(gpsOptions), User::Invariant());
		__ASSERT_ALWAYS(ELbsGpsOptionsClass == gpsOptions.ClassType(), User::Invariant());
		__ASSERT_ALWAYS(CLbsAdmin::EGpsAlwaysTerminalAssisted == gpsOptions.GpsMode(), User::Invariant());

		__ASSERT_ALWAYS(iGpsModuleObserver->WasLocationUpdateRequested(time, quality), User::Invariant());
		
		// Ensure that no other messages were sent to the GPS module
		__ASSERT_ALWAYS(!iGpsModuleObserver->IsMoreActivityObserved(), User::Invariant());

		// Reset Admin to to value it had before the test changed it
		admin->Set(KLbsSettingRoamingGpsMode, originalMode);
		CleanupStack::PopAndDestroy(admin);
		}
	  return TestStepResult();
	}

TVerdict CTe_AgpsManagerStep7::doTestStepPostambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
