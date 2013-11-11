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
 @file te_agpsmanagerstep19.cpp
*/
 
#include "te_agpsmanagerstep19.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep19::~CTe_AgpsManagerStep19()
	{
	}

CTe_AgpsManagerStep19::CTe_AgpsManagerStep19()
	{
	SetTestStepName(KAgpsManagerStep19);
	}

TVerdict CTe_AgpsManagerStep19::doTestStepPreambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// Boundary value tests for positioning Method in Location Server requests.
// The number of positioning methods requested by the LS cannot be zero (Manager panics).
//
TVerdict CTe_AgpsManagerStep19::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
		TLbsNetPosRequestMethodInt posMethods;
		
		// Configure H/W Capabilities as Terminal Based and Terminal Assisted 
		__ASSERT_ALWAYS(KErrNone == iCapabilitiesProperty.Set(KManagerTestSuiteUid,CTe_AgpsManagerStepBase::ETestCapabilitiesKey,
															  TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB),
															  User::Invariant());
		
		//Inject a location request from LS for Autonomous method		
		iLsToManagerRequestInjector->RequestPositionUpdate(posMethods);
		iGpsModuleObserver->WaitForCallBack();
		
		}
	  return TestStepResult();
	}

TVerdict CTe_AgpsManagerStep19::doTestStepPostambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
