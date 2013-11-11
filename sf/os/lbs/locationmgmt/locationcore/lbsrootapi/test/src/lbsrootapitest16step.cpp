// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file NetworkRequestHandlerTest16Step.cpp
*/
#include "lbsrootapiTest16Step.h"
#include "te_lbsrootapitestsuiteDefs.h"

#include "Lbsrootapi.h"



CLbsRootApiTest16Step::~CLbsRootApiTest16Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest16Step::CLbsRootApiTest16Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest16Step);
	}

TVerdict CLbsRootApiTest16Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest16Step"));
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest16Step::DoMainL()
{
}

TVerdict CLbsRootApiTest16Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	
	  if (TestStepResult()==EPass)
		{
        CActiveScheduler* sched = new (ELeave)CActiveScheduler;

   	    CleanupStack::PushL(sched);
   	    CActiveScheduler::Install(sched);

        CTinyTestClass* tinyClass = CTinyTestClass::NewL(this);
        CleanupStack::PushL(tinyClass);
        
        CActiveScheduler::Start();
        
        CleanupStack::PopAndDestroy(tinyClass);
        CleanupStack::PopAndDestroy(sched);
		}		
		
	  return TestStepResult();
	}


void CLbsRootApiTest16Step::StartLbsTestStepL(TRequestStatus& aStatus)
    {
	INFO_PRINTF1(_L("CLbsRootApiTest16Step : open RLbsSystemController"));

    iLbsRootProcessApi.OpenL(KLbsTestProcessUid);
        
	INFO_PRINTF1(_L("CLbsRootApiTest16Step : RequestSystemStartup"));
    TRequestStatus status;
    iLbsRootProcessApi.StartSystem(aStatus);
    }
    
void CLbsRootApiTest16Step::FinishLbsTestStep(TRequestStatus& aStatus,TBool& aFinished)
    {
	// Check the status, which is the result of the attempt to start the system
	if(aStatus != ELbsStartupFail)
		{
		SetTestStepResult(EFail);
		}
		
	INFO_PRINTF1(_L("CLbsRootApiTest16Step : RequestSystemCloseDown"));
    iLbsRootProcessApi.RequestSystemCloseDown(aStatus);

    aFinished = ETrue;
    }
TVerdict CLbsRootApiTest16Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest16Step"));

    iLbsRootProcessApi.Close();
	return TestStepResult();
	}
