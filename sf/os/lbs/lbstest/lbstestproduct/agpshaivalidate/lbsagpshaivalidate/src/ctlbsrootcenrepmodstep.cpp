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
// @file ctlbsrootcenrepmodstep.cpp
// This is the class implementation for the step to modify the lbsroot's central rep 
// 
//

// EPOC includes.
#include <centralrepository.h>


// LBS includes. 

// LBS test includes.
#include "ctlbsrootcenrepmodstep.h"
#include "lbstestloggermacros.h"
#include "ctlbs.h"


/**
 * Destructor
 */
CT_LbsAGPSHAIValidateStep_RootCenRepMod::~CT_LbsAGPSHAIValidateStep_RootCenRepMod()
	{
	}


/**
 * Constructor
 */
CT_LbsAGPSHAIValidateStep_RootCenRepMod::CT_LbsAGPSHAIValidateStep_RootCenRepMod(CT_LbsAGPSHAIServerRoot& aParent) : iParent(aParent)
	{
	SetTestStepName(KLbsRootCenRepModStep);
	}


/**
Static Constructor
*/
CT_LbsAGPSHAIValidateStep_RootCenRepMod* CT_LbsAGPSHAIValidateStep_RootCenRepMod::New(CT_LbsAGPSHAIServerRoot& aParent)
	{
	return new CT_LbsAGPSHAIValidateStep_RootCenRepMod(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

 
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsAGPSHAIValidateStep_RootCenRepMod::doTestStepL()
	{
	// Test step used to start the location manager.
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_RootCenRepMod::doTestStepL()");

	if (TestStepResult() == EPass)
		{
	    CRepository* rep;
	    rep = CRepository::NewLC(KLbsCenRepUid); 
	    if(!rep)
	    	{
	    	TESTLOG(ELogP2, "FAILED: No cre file for lbsroot found at startup!");
	    	INFO_PRINTF1(_L("<font><b>FAILED: No cre file for lbsroot found at startup!</b></font>"));
	    	SetTestStepResult(EFail);
	    	}
		else
			{			
			TUint32 errkey;
			TInt err;
			
			// delete all keys of the form 0x00001NNN, 0x00002NNN or 0x00003NNN:
			err = rep->Delete(KLbsRootProcessNameFirstKey, KRootKeyMask, errkey);
			if(KErrNotFound == err)
				{
				TESTLOG(ELogP2, "Note: Found no Processes in lbsroot central rep file");
				}
			else if(KErrNone != err)
				{
				User::Leave(err);	
				}
			
			err = rep->Delete(KLbsRootProcessUidFirstKey, KRootKeyMask, errkey);
			if(KErrNone != err && KErrNotFound != err)
				{
				User::Leave(err);	
				}
			err = rep->Delete(KLbsRootProcessIsServerFirstKey, KRootKeyMask, errkey);
			if(KErrNone != err && KErrNotFound != err)
				{
				User::Leave(err);	
				}		
			
		    CleanupStack::PopAndDestroy(rep);
			}
		}
		
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_RootCenRepMod::doTestStepL()");

	return TestStepResult();
	}

