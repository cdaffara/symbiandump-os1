// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsrootcenrepresetstep.cpp
// This is the class implementation for the step to reset the lbsroot's central rep 
// 
//

// EPOC includes.
#include <centralrepository.h>


// LBS includes. 

// LBS test includes.
#include "ctlbsrootcenrepresetstep.h"
#include "lbstestloggermacros.h"
#include "ctlbs.h"


/**
 * Destructor
 */
CT_LbsAGPSHAIValidateStep_RootCenRepReset::~CT_LbsAGPSHAIValidateStep_RootCenRepReset()
	{
	}


/**
 * Constructor
 */
CT_LbsAGPSHAIValidateStep_RootCenRepReset::CT_LbsAGPSHAIValidateStep_RootCenRepReset(CT_LbsAGPSHAIServerRoot& aParent) : iParent(aParent)
	{
	SetTestStepName(KLbsRootCenRepResetStep);
	}


/**
Static Constructor
*/
CT_LbsAGPSHAIValidateStep_RootCenRepReset* CT_LbsAGPSHAIValidateStep_RootCenRepReset::New(CT_LbsAGPSHAIServerRoot& aParent)
	{
	return new CT_LbsAGPSHAIValidateStep_RootCenRepReset(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

 
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsAGPSHAIValidateStep_RootCenRepReset::doTestStepL()
	{
	// Test step used to start the location manager.
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_RootCenRepReset::doTestStepL()");

	if (TestStepResult() == EPass)
		{
	    CRepository* rep;
	    rep = CRepository::NewL(KLbsCenRepUid); 
	    if(!rep)
	    	{
	    	TESTLOG(ELogP2, "FAILED: No cre file for lbsroot found!");
	    	INFO_PRINTF1(_L("<font><b>FAILED: No cre file for lbsroot found!</b></font>"));
	    	SetTestStepResult(EFail);
	    	}
		else
			{
			CleanupStack::PushL(rep);
						
			// reset the lbsroot cenrep file to how it was at boot:
			User::LeaveIfError(rep->Reset());
			
		    CleanupStack::PopAndDestroy(rep);
			}
		}
		
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_RootCenRepReset::doTestStepL()");

	return TestStepResult();
	}

