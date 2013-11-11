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
 @file te_agpsmanagerstep23.cpp
*/
 
#include "te_agpsmanagerstep23.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep23::~CTe_AgpsManagerStep23()
	{
	}

CTe_AgpsManagerStep23::CTe_AgpsManagerStep23()
	{
	SetTestStepName(KAgpsManagerStep23);
	}

TVerdict CTe_AgpsManagerStep23::doTestStepPreambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

//
// Boundary value tests for positioning Method in Location Server requests.
// The number of positioning methods requested by the LS cannot be 11 (interface doesn't allow it).
//
TVerdict CTe_AgpsManagerStep23::doTestStepL()
	{
 	if (TestStepResult()==EPass)
 		{
		TLbsNetPosRequestMethod methods;
		TLbsNetPosMethod posMethods[11];
   		posMethods[0].SetPosMethod(KLbsPositioningMeansGps,
   								   TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted);
   		//posMethods[1].SetPosMethod(KLbsPositioningMeansGps,
   		//						   TPositionModuleInfo::ETechnologyNetwork|TPositionModuleInfo::ETechnologyAssisted);
   		// Add 9 more...
   		for (TUint index = 1; index < 11; index++ )
   			{
   			posMethods[index].SetPosMethod(KLbsPositioningMeansGps,
   						   TPositionModuleInfo::ETechnologyNetwork|TPositionModuleInfo::ETechnologyAssisted);
   			}
   								   			   
   		__ASSERT_ALWAYS(KErrNone != methods.SetPosMethods(posMethods, 11), User::Invariant());
 		}
	  return TestStepResult();
	}

TVerdict CTe_AgpsManagerStep23::doTestStepPostambleL()
	{
	CTe_AgpsManagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
