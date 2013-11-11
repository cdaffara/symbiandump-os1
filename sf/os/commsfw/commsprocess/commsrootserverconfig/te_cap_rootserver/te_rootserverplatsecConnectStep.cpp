// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Root Server Connection Test Step
// 
//

/**
 @file 
*/
#include "te_rootserverplatsecConnectStep.h"
#include "te_rootserverplatsecConnectSuiteDefs.h"
#include <rsshared.h>

/**
 Destructor
*/
CRootServerPlatSecConnectStep::~CRootServerPlatSecConnectStep()
	{
	}


/**
 Constructor
*/
CRootServerPlatSecConnectStep::CRootServerPlatSecConnectStep()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KRootServerPlatSecConnectStep);
	}


/**
 @return - TVerdict code
 Override of base class virtual
*/
TVerdict CRootServerPlatSecConnectStep::doTestStepPreambleL()
	{
	return TestStepResult();
	}



/**
 @return - TVerdict code
 Override of base class pure virtual
 Our implementation only gets called if the base class doTestStepPreambleL() did
 not leave. That being the case, the current test result value will be EPass.
*/
TVerdict CRootServerPlatSecConnectStep::doTestStepL()
	{

	SetTestStepResult(EFail);	

	INFO_PRINTF1(_L("Root Server Connection Attempt"));  //Block start 

	// capabilities required for connect to succeed
	TCapability cap[] = {ECapabilityNetworkControl, ECapability_Limit};
	
	TSecurityInfo info;
	info.Set(RProcess());
	TBool expectFail = EFalse;
	
	for (TInt i = 0; cap[i] != ECapability_Limit; i++) 
		{
	
		if (!(info.iCaps.HasCapability(cap[i])))
			{
			expectFail = ETrue;
			break;
			}
		
		}
	
	if (expectFail) 
		{
		INFO_PRINTF1(_L("Insufficient capabilities - expecting test to Fail"));
		}
		else
		{
		INFO_PRINTF1(_L("Sufficient capabilities - expecting test to Pass"));
		}

	// attempt a connection to Root server
	TInt result;
	iSessionHelper = new(ELeave) CSessionConnectionHelper ();
	CleanupStack::PushL (iSessionHelper);

	result = iSessionHelper->OpenSession (RootServer::KRootServerName, TVersion (0, 0, 0));
	INFO_PRINTF2(_L("CreateSession() returned with %d"), result);
	
	if (result == KErrNotFound) 
		{
		// not running attempt to start it
    	_LIT(KRootServerExe, "c32exe.exe");
		INFO_PRINTF1(_L("RootServer not running, attempting to start it."));

    	RProcess process;
    	result = process.Create (KRootServerExe, KNullDesC);

		TRequestStatus status;
    	if (result == KErrNone)
			{
			process.Rendezvous (status);
			process.Resume ();
			User::WaitForRequest (status);
			process.Close ();
			result = status.Int ();
			}
			
	   	INFO_PRINTF2(_L("RootServer startup completed with [Return Value: %d] "), result);
	    result = iSessionHelper->OpenSession (RootServer::KRootServerName, TVersion (0, 0, 0));
		INFO_PRINTF2(_L("CreateSession() returned with %d"), result);
		}
	
	if ( (result != KErrNone) && (result != KErrPermissionDenied))
		{
		// inconclusive result
		SetTestStepResult (EInconclusive);
		return TestStepResult();
		}

	if (result == KErrNone) iSessionHelper->Close ();
	CleanupStack::PopAndDestroy (iSessionHelper);
	
	if (expectFail)
		{
		if (result == KErrPermissionDenied) 
			{
			// expected to fail and did
			SetTestStepResult (EPass);
			}
		}
	else
		{
		if (result == KErrNone) 
			{
			// expected to pass and did
			SetTestStepResult (EPass);
			}
		}
	
	

	return TestStepResult();
	}



/**
 @return - TVerdict code
 Override of base class virtual
*/
TVerdict CRootServerPlatSecConnectStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

