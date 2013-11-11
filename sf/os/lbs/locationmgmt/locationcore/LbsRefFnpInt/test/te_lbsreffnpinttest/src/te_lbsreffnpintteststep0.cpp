/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#include "te_lbsreffnpintteststep0.h"
#include "te_lbsreffnpinttestsuitedefs.h"

CLbsReffnpintTestStep0::~CLbsReffnpintTestStep0()
/**
 * Destructor
 */
	{
	}

CLbsReffnpintTestStep0::CLbsReffnpintTestStep0()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsReffnpintTestStep0);
	}

TVerdict CLbsReffnpintTestStep0::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_LbsReffnpintTestSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsReffnpintTestStep0::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//
		// Test opening and closing the interfaces
		//
		
		RLbsNetworkLocationRequests requests;
		RLbsNetworkLocationResponses responses;
		RLbsNetworkPositionUpdates positionsReference;
		RLbsNetworkPositionUpdates positionsFinal;
		
		requests.OpenL(KTe_lbsreffnpinttestSuiteUid);
		CleanupClosePushL(requests);
		
		responses.OpenL(KTe_lbsreffnpinttestSuiteUid);
		CleanupClosePushL(responses);
		
		positionsReference.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionReference);
		CleanupClosePushL(positionsReference);
		
		positionsFinal.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionFinal);
		CleanupClosePushL(positionsFinal);
		
		CleanupStack::PopAndDestroy(4, &requests);
		
		//
		// Test re-opening the interfaces, this should work without error
		//
		
		requests.OpenL(KTe_lbsreffnpinttestSuiteUid);
		CleanupClosePushL(requests);
		
		responses.OpenL(KTe_lbsreffnpinttestSuiteUid);
		CleanupClosePushL(responses);
		
		positionsReference.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionReference);
		CleanupClosePushL(positionsReference);
		
		positionsFinal.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionFinal);
		CleanupClosePushL(positionsFinal);
		
		CleanupStack::PopAndDestroy(4, &requests);
				
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict CLbsReffnpintTestStep0::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_LbsReffnpintTestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
