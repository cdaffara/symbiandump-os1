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
 @file te_agpsmanagerstep20.cpp
*/
 
#include "te_agpsmanagerstep20.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep20::~CTe_AgpsManagerStep20()
	{
	}

CTe_AgpsManagerStep20::CTe_AgpsManagerStep20()
	{
	SetTestStepName(KAgpsManagerStep20);
	}

TVerdict CTe_AgpsManagerStep20::doTestStepPreambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// Boundary value tests for positioning Method in Location Server requests.
// The number of positioning methods requested by the LS cannot be two (Manager panics).
//
TVerdict CTe_AgpsManagerStep20::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
		TLbsNetPosRequestMethodInt methods;
		TLbsNetPosMethodInt posMethods[2];
   		posMethods[0].SetPosMethod(KLbsPositioningMeansGps,
   								   TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted);
   		posMethods[1].SetPosMethod(KLbsPositioningMeansGps,
   								   TPositionModuleInfo::ETechnologyNetwork|TPositionModuleInfo::ETechnologyAssisted);			   
   		methods.SetPosMethods(posMethods, 2);
		
		// Configure H/W Capabilities as Terminal Based and Terminal Assisted 
		__ASSERT_ALWAYS(KErrNone == iCapabilitiesProperty.Set(KManagerTestSuiteUid,
															  CTe_AgpsManagerStepBase::ETestCapabilitiesKey,
															  TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB),
															  User::Invariant());
		
		//Inject a location request from LS for Autonomous method		
		iLsToManagerRequestInjector->RequestPositionUpdate(methods);
		iGpsModuleObserver->WaitForCallBack();
		}
	  return TestStepResult();
	}

TVerdict CTe_AgpsManagerStep20::doTestStepPostambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
