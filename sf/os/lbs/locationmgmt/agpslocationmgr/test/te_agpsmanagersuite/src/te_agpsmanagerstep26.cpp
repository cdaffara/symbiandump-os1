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
 @file te_agpsmanagerstep26.cpp
*/
 
#include "te_agpsmanagerstep26.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep26::~CTe_AgpsManagerStep26()
	{
	}

CTe_AgpsManagerStep26::CTe_AgpsManagerStep26()
	{
	SetTestStepName(KAgpsManagerStep26);
	}

TVerdict CTe_AgpsManagerStep26::doTestStepPreambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

// Invalid values of positioning method from the Location Server in TerminalBased-only h/w.
//
TVerdict CTe_AgpsManagerStep26::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
		CLbsAdmin* admin;
		TLbsGpsOptions gpsOptions;

		// Set H/W Capabilities to Autonomous
		__ASSERT_ALWAYS(KErrNone == iCapabilitiesProperty.Set(KManagerTestSuiteUid,
															  CTe_AgpsManagerStepBase::ETestCapabilitiesKey,
				 									 		  TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased),
				 									 		  User::Invariant());
		
		// Set admin gps mode to Terminal Based. Get current GPS mode so
		// that admin can be returned to its original value at the end of the test
		//
		admin = CLbsAdmin::NewL();
		CleanupStack::PushL(admin);
		CLbsAdmin::TGpsMode originalMode; 
		admin->Get(KLbsSettingRoamingGpsMode, originalMode);
		admin->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsPreferTerminalBased);

		//Inject a location request from LS for Terminal Based method		
		iLsToManagerRequestInjector->RequestPositionUpdate(CTestAgpsRequestSender::ETPosMethodTa);
		iGpsModuleObserver->WaitForCallBack();
		}
	  return TestStepResult();
	}

TVerdict CTe_AgpsManagerStep26::doTestStepPostambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
