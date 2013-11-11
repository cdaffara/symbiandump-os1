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

#include "lbsnetregstatusinttest0Step.h"
#include "Te_lbsnetregstatusinttestSuiteDefs.h"

Clbsnetregstatusinttest0Step::~Clbsnetregstatusinttest0Step()
/**
 * Destructor
 */
	{
	}

Clbsnetregstatusinttest0Step::Clbsnetregstatusinttest0Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetregstatusinttest0Step);
	}

TVerdict Clbsnetregstatusinttest0Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_lbsnetregstatusinttestSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetregstatusinttest0Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		///////////////////////////////////////////////////////////////////////
		//
		// Test network registration status interface
		//
		
		RLbsNetworkRegistrationStatus netRegStatus;
		netRegStatus.OpenL();
		CleanupClosePushL(netRegStatus);
		
		// Test that the initial value is ENotRegistered
		RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus netReg;
		User::LeaveIfError(netRegStatus.GetNetworkRegistrationStatus(netReg));
		TESTL(netReg == RLbsNetworkRegistrationStatus::ENotRegistered);
		
		// Test setting a new value and that we can get it
		User::LeaveIfError(netRegStatus.SetNetworkRegistrationStatus(RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork));
		User::LeaveIfError(netRegStatus.GetNetworkRegistrationStatus(netReg));
		TESTL(netReg == RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork);
		
		// Test that we receive notifications ok
		TRequestStatus status;
		netRegStatus.NotifyNetworkRegistrationStatusUpdate(status);
		User::LeaveIfError(netRegStatus.SetNetworkRegistrationStatus(RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork));
		User::WaitForRequest(status);
		User::LeaveIfError(netRegStatus.GetNetworkRegistrationStatus(netReg));
		TESTL(netReg == RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork);
		
		CleanupStack::PopAndDestroy(&netRegStatus);
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict Clbsnetregstatusinttest0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_lbsnetregstatusinttestSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
