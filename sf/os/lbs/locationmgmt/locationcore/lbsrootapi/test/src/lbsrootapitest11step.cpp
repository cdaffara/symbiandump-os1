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
 @file lbsrootapiTest7Step.cpp
*/
#include "lbsrootapiTest7Step.h"
#include "te_lbsrootapitestsuiteDefs.h"

#include "Lbsrootapi.h"



CLbsRootApiTest7Step::~CLbsRootApiTest7Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest7Step::CLbsRootApiTest7Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest7Step);
	}

TVerdict CLbsRootApiTest7Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest7Step"));

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

void CLbsRootApiTest7Step::DoMainL()
{
}
_LIT(KNrhProcessName, "lbsnetworkrequesthandler.exe[10281d45]0001");
_LIT(KAgpsProcessName, "lbsgpslocmanager.exe[10281D44]0001");
_LIT(KNgProcessName,   "lbsnetgateway.exe[10281D46]0001");

TVerdict CLbsRootApiTest7Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    RProcess nrh;
    RProcess agps;
    RProcess ng;
    TInt processOpenStatus;
	TInt retVal;
	TVerdict result = EPass;
	  if (TestStepResult()==EPass)
		{

	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : open RLbsSystemController"));

        RLbsSystemController lbsRootProcessApi;
        lbsRootProcessApi.OpenL(KLbsTestProcessUid);

	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : Request SYNCHRONOUS SystemStartup "));
        retVal = lbsRootProcessApi.StartSystem();
        if(retVal != ELbsStartupSuccess)
            {
        	result = EFail;
            }
        retVal = lbsRootProcessApi.StartSystem();
        if(retVal != ELbsStartupSuccess)
            {
        	result = EFail;
            }
        retVal = lbsRootProcessApi.StartSystem();
        if(retVal != ELbsStartupSuccess)
            {
        	result = EFail;
            }
        retVal = lbsRootProcessApi.StartSystem();
        if(retVal != ELbsStartupSuccess)
            {
        	result = EFail;
            }
        retVal = lbsRootProcessApi.StartSystem();
        if(retVal != ELbsStartupSuccess)
            {
        	result = EFail;
            }
        retVal = lbsRootProcessApi.StartSystem();
        if(retVal != ELbsStartupSuccess)
            {
        	result = EFail;
            }
        retVal = lbsRootProcessApi.StartSystem();
        if(retVal != ELbsStartupSuccess)
            {
        	result = EFail;
            }
        retVal = lbsRootProcessApi.StartSystem();
        if(retVal != ELbsStartupSuccess)
            {
        	result = EFail;
            }
        retVal = lbsRootProcessApi.StartSystem();
        if(retVal != ELbsStartupSuccess)
            {
        	result = EFail;
            }
        retVal = lbsRootProcessApi.StartSystem();
        if(retVal != ELbsStartupSuccess)
            {
        	result = EFail;
            }
            
            
            
        // Try and open the processes, to check they've been brought up OK.
        
        processOpenStatus = nrh.Open(KNrhProcessName);
        if(processOpenStatus)
            {
        	result = EFail;
	        return TestStepResult();
            }
        processOpenStatus = agps.Open(KAgpsProcessName);
        if(processOpenStatus)
            {
        	result = EFail;
		    nrh.Close();
	        return TestStepResult();
            }
        processOpenStatus = ng.Open(KNgProcessName);
        if(processOpenStatus)
            {
        	result = EFail;
		    nrh.Close();
		    agps.Close();
	        return TestStepResult();
            }
            
	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : prod a process to request closedown when it's been asked to close down"));
        iSecondCloseDownRequest.Set(1);        

	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : request synchronous system closedown"));
        lbsRootProcessApi.RequestSystemCloseDown();
	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : request second synchronous system closedown"));
        lbsRootProcessApi.RequestSystemCloseDown();
	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : request third synchronous system closedown"));
        lbsRootProcessApi.RequestSystemCloseDown();
	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : request fourth synchronous system closedown"));
        lbsRootProcessApi.RequestSystemCloseDown();
	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : request fifth synchronous system closedown"));
        lbsRootProcessApi.RequestSystemCloseDown();
	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : request sixth synchronous system closedown"));
        lbsRootProcessApi.RequestSystemCloseDown();
	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : request seventh synchronous system closedown"));
        lbsRootProcessApi.RequestSystemCloseDown();
	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : request eighth synchronous system closedown"));
        lbsRootProcessApi.RequestSystemCloseDown();
	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : request ninth synchronous system closedown"));
        lbsRootProcessApi.RequestSystemCloseDown();
	    INFO_PRINTF1(_L("CLbsRootApiTest7Step : request tenth synchronous system closedown"));
        lbsRootProcessApi.RequestSystemCloseDown();
            
        processOpenStatus = nrh.Open(KNrhProcessName);
        if(processOpenStatus != KErrNotFound)
            {
        	result = EFail;
	        return TestStepResult();
            }
        processOpenStatus = agps.Open(KAgpsProcessName);
        if(processOpenStatus != KErrNotFound)
            {
        	result = EFail;
		    nrh.Close();
	        return TestStepResult();
            }
        processOpenStatus = ng.Open(KNgProcessName);
        if(processOpenStatus != KErrNotFound)
            {
        	result = EFail;
		    nrh.Close();
		    agps.Close();
	        return TestStepResult();
            }
    
		SetTestStepResult(result);

		}		
		
	  return TestStepResult();
	}

TVerdict CLbsRootApiTest7Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest7Step"));
	// process something post setting to the test step
	// uncomment the following line if you have post process or remove the following line if no post process
	SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
