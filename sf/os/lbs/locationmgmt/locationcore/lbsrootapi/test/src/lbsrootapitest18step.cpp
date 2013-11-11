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
 @file NetworkRequestHandlerTest18Step.cpp
*/
#include "lbsrootapiTest18Step.h"
#include "te_lbsrootapitestsuiteDefs.h"

#include "Lbsrootapi.h"



CLbsRootApiTest18Step::~CLbsRootApiTest18Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest18Step::CLbsRootApiTest18Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest18Step);
	}

TVerdict CLbsRootApiTest18Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest18Step"));
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest18Step::DoMainL()
{
}
_LIT(KNrhProcessName, "lbsnetworkrequesthandler.exe[10281d45]0001");
_LIT(KAgpsProcessName, "lbsgpslocmanager.exe[10281D44]0001");
_LIT(KNgProcessName,   "lbsnetgateway.exe[10281D46]0001");

TVerdict CLbsRootApiTest18Step::doTestStepL()
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


void CLbsRootApiTest18Step::StartLbsTestStepL(TRequestStatus& aStatus)
    {
	INFO_PRINTF1(_L("CLbsRootApiTest18Step : open RLbsSystemController"));

    iLbsRootProcessApi.OpenL(KLbsTestProcessUid);
        
	// For this test, the LBS system should already have been started 
	// with a single process (the AGPS manabger) active. Check it and no
	// other process is present.
	RProcess nrh;
	RProcess agps;
	RProcess ng;
	TInt err;
	err = nrh.Open(KNrhProcessName);
	if(err != KErrNotFound)
		{
		SetTestStepResult(EFail);
		}
	err = agps.Open(KAgpsProcessName);
	if(err)
		{
		SetTestStepResult(EFail);
		nrh.Close();
		}
	err = ng.Open(KNgProcessName);
	if(err != KErrNotFound)
		{
		SetTestStepResult(EFail);
		nrh.Close();
		agps.Close();
		}

	INFO_PRINTF1(_L("CLbsRootApiTest18Step : RequestSystemStartup"));
    TRequestStatus status;
    iLbsRootProcessApi.RequestSystemStartup(aStatus);
    }
    
void CLbsRootApiTest18Step::FinishLbsTestStep(TRequestStatus& aStatus,TBool& aFinished)
	{    	    
	if(aStatus.Int() != ELbsCloseDownRejected)
		{
		SetTestStepResult(EFail);
		}
	RProcess nrh;
	RProcess agps;
	RProcess ng;
	TInt err;
	err = nrh.Open(KNrhProcessName);
	if(err != KErrNotFound)
		{
		SetTestStepResult(EFail);
		}
	err = agps.Open(KAgpsProcessName);
	if(err)
		{
		SetTestStepResult(EFail);
		nrh.Close();
		}
	err = ng.Open(KNgProcessName);
	if(err != KErrNotFound)
		{
		SetTestStepResult(EFail);
		nrh.Close();
		agps.Close();
		}

	INFO_PRINTF1(_L("CLbsRootApiTest18Step : RequestSystemCloseDown"));
	iLbsRootProcessApi.RequestSystemCloseDown(aStatus);
	aFinished = ETrue;
	}
	
TVerdict CLbsRootApiTest18Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest18Step"));

    iLbsRootProcessApi.Close();
	return TestStepResult();
	}
