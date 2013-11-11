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
 @file NetworkRequestHandlerTest17Step.cpp
*/
#include "lbsrootapiTest17Step.h"
#include "te_lbsrootapitestsuiteDefs.h"

#include "Lbsrootapi.h"



CLbsRootApiTest17Step::~CLbsRootApiTest17Step()
/**
 * Destructor
 */
	{
    iLocServer.Close();
	}

CLbsRootApiTest17Step::CLbsRootApiTest17Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest17Step);
	}

TVerdict CLbsRootApiTest17Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest17Step"));
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest17Step::DoMainL()
{
}
_LIT(KNrhProcessName, "lbsnetworkrequesthandler.exe[10281d45]0001");
_LIT(KAgpsProcessName, "lbsgpslocmanager.exe[10281D44]0001");
_LIT(KNgProcessName,   "lbsnetgateway.exe[10281D46]0001");

TVerdict CLbsRootApiTest17Step::doTestStepL()
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


void CLbsRootApiTest17Step::StartLbsTestStepL(TRequestStatus& aStatus)
    {
	INFO_PRINTF1(_L("CLbsRootApiTest17Step : open RLbsSystemController"));

    iLbsRootProcessApi.OpenL(KLbsTestProcessUid);
        
    // Kick off the dummy location server.
    
    iLocServer.Create(_L("lbslocserver.exe"), KNullDesC);
    TRequestStatus status;
    iLocServer.Rendezvous(status);
    iLocServer.Resume();
    User::WaitForRequest(status);


	INFO_PRINTF1(_L("CLbsRootApiTest17Step : RequestSystemStartup"));
    iLbsRootProcessApi.RequestSystemStartup(aStatus);
    
    }
    
void CLbsRootApiTest17Step::FinishLbsTestStep(TRequestStatus& aStatus,TBool& aFinished)
    {    	    
        RProcess nrh;
        RProcess agps;
        RProcess ng;
        TInt err;
        err = nrh.Open(KNrhProcessName);
        if(err)
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
        if(err)
            {
		    SetTestStepResult(EFail);
		    nrh.Close();
		    agps.Close();
            }

	    INFO_PRINTF1(_L("CLbsRootApiTest17Step : RequestSystemCloseDown"));
        iLbsRootProcessApi.RequestSystemCloseDown(aStatus);
    aFinished = ETrue;
    }
TVerdict CLbsRootApiTest17Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest17Step"));

    iLbsRootProcessApi.Close();
	return TestStepResult();
	}
