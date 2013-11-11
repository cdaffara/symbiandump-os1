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
 @file lbsrootapiTest14Step.cpp
*/
#include "lbsrootapiTest14Step.h"
#include "te_lbsrootapitestsuiteDefs.h"

#include "Lbsrootapi.h"



CLbsRootApiTest14Step::~CLbsRootApiTest14Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest14Step::CLbsRootApiTest14Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest14Step);
	}

TVerdict CLbsRootApiTest14Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest14Step"));

	const TSecurityPolicy KReadPolicy(ECapabilityLocation);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);

	TRAPD(err,RProperty::Define(KTestLbsSecondCloseDownRequestKey, 
	                                           RProperty::EInt, 
	                                               KReadPolicy, 
	                                             KWritePolicy));
    if(err != KErrNone)
        {
        User::LeaveIfError(err);
        }
    
    iSecondCloseDownRequest.Attach(KLbsTestProcessUid, KTestLbsSecondCloseDownRequestKey);
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest14Step::DoMainL()
{
}
_LIT(KNrhProcessName, "lbsnetworkrequesthandler.exe[10281d45]0001");
_LIT(KAgpsProcessName, "lbsgpslocmanager.exe[10281D44]0001");
_LIT(KNgProcessName,   "lbsnetgateway.exe[10281D46]0001");

TVerdict CLbsRootApiTest14Step::doTestStepL()
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


void CLbsRootApiTest14Step::StartLbsTestStepL(TRequestStatus& aStatus)
	{
	INFO_PRINTF1(_L("CLbsRootApiTest14Step : Module info files are empty"));
	INFO_PRINTF1(_L("CLbsRootApiTest14Step : open RLbsSystemController"));

	iLbsRootProcessApi.OpenL(KLbsTestProcessUid);

	INFO_PRINTF1(_L("CLbsRootApiTest14Step : RequestSystemStartup"));
	TRequestStatus status;
	iLbsRootProcessApi.RequestSystemStartup(aStatus);
	}
    
void CLbsRootApiTest14Step::FinishLbsTestStep(TRequestStatus& /*aStatus*/,TBool& aFinished)
	{
	RProcess nrh;
	RProcess agps;
	RProcess ng;
	TInt err;
	INFO_PRINTF1(_L("CLbsRootApiTest14Step : Check processes are up..."));
	err = nrh.Open(KNrhProcessName);
	if(err)
		{
		INFO_PRINTF1(_L("CLbsRootApiTest14Step : ***ERROR - NRH not found"));
		SetTestStepResult(EFail);
		}
	err = agps.Open(KAgpsProcessName);
	if(err)
		{
		INFO_PRINTF1(_L("CLbsRootApiTest14Step : ***ERROR - GPS LOC Manager not found"));
		SetTestStepResult(EFail);
		nrh.Close();
		}
	err = ng.Open(KNgProcessName);
	if(err)
		{
		INFO_PRINTF1(_L("CLbsRootApiTest14Step : ***ERROR - Network Gateway not found"));
		SetTestStepResult(EFail);
		nrh.Close();
		agps.Close();
		}
            
	if(err == KErrNone)
		{
		INFO_PRINTF1(_L("CLbsRootApiTest14Step : Processes up OK"));    	
		}

    // Check the mdule info. For this test, there shoudn't be any 
    // - there are no module info files
    
    TUint moduleInfoCount;
    LbsModuleInfo::GetNumModules(moduleInfoCount);
    if(moduleInfoCount != 0)
        {
        err = KErrGeneral;
	    INFO_PRINTF1(_L("CLbsRootApiTest14Step : ***ERROR: found module info where none was expected"));    	
        }

	INFO_PRINTF1(_L("CLbsRootApiTest14Step : RequestSystemCloseDown"));
	iLbsRootProcessApi.RequestSystemCloseDown();
	if(err) // any error fails the test.
	    {
		SetTestStepResult(EFail);
	    }
	aFinished = ETrue;
}

TVerdict CLbsRootApiTest14Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest14Step"));
	iLbsRootProcessApi.Close();
	return TestStepResult();
	}
