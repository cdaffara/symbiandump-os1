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
 @file te_agpsmanagerstep22.cpp
*/
 
#include "te_agpsmanagerstep22.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep22::~CTe_AgpsManagerStep22()
	{
	}

CTe_AgpsManagerStep22::CTe_AgpsManagerStep22()
	{
	SetTestStepName(KAgpsManagerStep22);
	}

TVerdict CTe_AgpsManagerStep22::doTestStepPreambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// Boundary value tests for positioning Method in Location Server requests.
// The number of positioning methods requested by the LS cannot be 10 (Manager panics).
//
TVerdict CTe_AgpsManagerStep22::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
		TLbsNetPosRequestMethodInt methods;
		TLbsNetPosMethodInt posMethods[19];
   		posMethods[0].SetPosMethod(KLbsPositioningMeansGps,
   								   TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted);
   		posMethods[1].SetPosMethod(KLbsPositioningMeansGps,
   								   TPositionModuleInfo::ETechnologyNetwork|TPositionModuleInfo::ETechnologyAssisted);
   		// Add 17 more...
   		for (TUint index = 2; index < 11; index++ )
   		{
   			posMethods[index].SetPosMethod(KLbsPositioningMeansGps,
   						   TPositionModuleInfo::ETechnologyNetwork|TPositionModuleInfo::ETechnologyAssisted);
   		}
   								   			   
   		__ASSERT_ALWAYS (KErrNone == methods.SetPosMethods(posMethods, 10), User::Invariant());
		
		// Configure H/W Capabilities as Terminal Based and Terminal Assisted only
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

TVerdict CTe_AgpsManagerStep22::doTestStepPostambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
