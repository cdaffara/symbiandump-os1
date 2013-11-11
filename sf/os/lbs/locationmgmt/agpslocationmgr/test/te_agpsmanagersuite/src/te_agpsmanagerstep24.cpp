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
 @file te_agpsmanagerstep24.cpp
*/
 
#include "te_agpsmanagerstep24.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep24::~CTe_AgpsManagerStep24()
	{
	}

CTe_AgpsManagerStep24::CTe_AgpsManagerStep24()
	{
	SetTestStepName(KAgpsManagerStep24);
	}

TVerdict CTe_AgpsManagerStep24::doTestStepPreambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// Boundary value tests for positioning Method in Location Server requests.
// LS request a positioning method out of the valid range.
//
TVerdict CTe_AgpsManagerStep24::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
 		TInt invalidValue = TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted|TPositionModuleInfo::ETechnologyNetwork;
		TLbsNetPosRequestMethodInt methods;
		TLbsNetPosMethodInt posMethods[1];
   		posMethods[0].SetPosMethod(KLbsPositioningMeansGps,invalidValue);
      								   			   
   		__ASSERT_ALWAYS(KErrNone == methods.SetPosMethods(posMethods, 1), User::Invariant());
   		
   		// Inject a location request from LS for Terminal Based method		
		iLsToManagerRequestInjector->RequestPositionUpdate(methods);
		iGpsModuleObserver->WaitForCallBack();

 		}
	  return TestStepResult();
	}

TVerdict CTe_AgpsManagerStep24::doTestStepPostambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
