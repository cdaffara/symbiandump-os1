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
 @file lbsnetinternaltest1Step.cpp
*/
#include "lbsnetinternaltest1Step.h"
#include "Te_lbsnetinternaltestSuiteDefs.h"

Clbsnetinternaltest1Step::~Clbsnetinternaltest1Step()
/**
 * Destructor
 */
	{
	}

Clbsnetinternaltest1Step::Clbsnetinternaltest1Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetinternaltest1Step);
	}

TVerdict Clbsnetinternaltest1Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_lbsnetinternaltestSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetinternaltest1Step::doTestStepL()
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
		// Test that messages can be sent & received in OOM conditions.
		//
		
		RLbsNetChannel netGatewayChannel;
		RLbsNetChannel netRequestChannel;
		
		// This setup is assumed to be outside of the OOM part.
		netGatewayChannel.OpenL(RLbsNetChannel::EChannelNRH2NG, *this);
		CleanupClosePushL(netGatewayChannel);
				
		netRequestChannel.OpenL(RLbsNetChannel::EChannelNG2NRH, *this);
		CleanupClosePushL(netRequestChannel);
		
		// Set next allocation to fail
		__UHEAP_SETFAIL(RHeap::EFailNext, 1);
		__UHEAP_MARK;
		
		TRequestStatus status;
		TLbsNetPosRequestQualityInt quality;
		TLbsNetPosRequestMethodInt method;
		TPositionInfo posInfo;
		TLbsNetLocationRequestMsg msg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 1), 
									  EFalse,
									  TLbsNetworkEnumInt::EServiceSelfLocation,
									  quality,
									  method);
		netRequestChannel.SendMessage(msg, status);
		
		CActiveScheduler::Start();
		
		User::WaitForRequest(status);
		
		_LIT(KDestination, "07760224840");
		TLbsNetTransmitLocationRequestMsg msg2(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 2),
											   KDestination,
											   1);
		netGatewayChannel.SendMessage(msg2, status);
		
		CActiveScheduler::Start();
		
		User::WaitForRequest(status);		
		
		__UHEAP_MARKEND;
		__UHEAP_RESET;
		
		CleanupStack::PopAndDestroy(2, &netGatewayChannel);

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict Clbsnetinternaltest1Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_lbsnetinternaltestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
