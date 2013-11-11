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
 @file lbsnetgatewaystopstep.cpp
*/
#include "lbsnetgatewaystopstep.h"
#include "lbsnetregstatusint.h"
#include "Te_lbsnetgatewaySuiteDefs.h"
#include <lbs/lbsadmin.h>

Clbsnetgatewaystopstep::~Clbsnetgatewaystopstep()
/**
 * Destructor
 */
	{
	}

Clbsnetgatewaystopstep::Clbsnetgatewaystopstep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetgatewaystopstep);
	}

TVerdict Clbsnetgatewaystopstep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Clbsnetgatewaystopstep"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	
    InitialiseUidArray();
    iStubPMChannelArray = new RNetGatewayProtocolTestChannel;
    iStubPMChannelArray->OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, KTe_LbsNetGatewayTestProtocolIndex);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetgatewaystopstep::doTestStepL()
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
		// Special test step to shutdown the LBS
		// sub-system once all of the tests in this
		// test suite have been run.
		//
		
		// Shutdown NetGateway
		User::LeaveIfError(StopNetGateway());
		
		// Shutdown system controller interface
		RLbsProcessSupervisor::ShutDownL();

		// Shutdown the network registration status interface
		RLbsNetworkRegistrationStatus::ShutDownL();
				
		// Shutdown Network Location Manager interface.
		RLbsNetworkLocationRequests::ShutDownL();
		RLbsNetworkLocationResponses::ShutDownL();
		RLbsNetworkPositionUpdates::ShutDownL();
		
		// Shutdown A-GPS interface
		RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNG2AGPS);
		RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelAGPS2NG);
		RAssistanceDataCache::ShutDownL();
		
        // Shutdown the SuplPSY to NG interface 
        RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNG2SUPLPSY);
        RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelSUPLPSY2NG);
        
		// Shutdown NRH-NG interface
		RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNG2NRH);
		RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNRH2NG);
		
		// Reset the Lbs Admin repository to its default settings
		CLbsAdmin * admin = CLbsAdmin::NewL();
		CleanupStack::PushL(admin);
		admin->ResetToDefault();
		CleanupStack::PopAndDestroy(admin);

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict Clbsnetgatewaystopstep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Clbsnetgatewaystopstep"));
	// process something post setting to the test step
	iStubPMChannelArray->Close();
	
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
