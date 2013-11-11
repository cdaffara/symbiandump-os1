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
 @file lbsnetinternaltest2Step.cpp
*/
#include "lbsnetinternaltest2Step.h"
#include "Te_lbsnetinternaltestSuiteDefs.h"

Clbsnetinternaltest2Step::~Clbsnetinternaltest2Step()
/**
 * Destructor
 */
	{
	}

Clbsnetinternaltest2Step::Clbsnetinternaltest2Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetinternaltest2Step);
	}

TVerdict Clbsnetinternaltest2Step::doTestStepPreambleL()
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


TVerdict Clbsnetinternaltest2Step::doTestStepL()
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
		// Test that only one message can be sent at a time.
		// If more than one is sent, the extra ones will complete early with an
		// an error in the TRequestStatus.
		//
		
		// Test the interface from the Network Gateway to Network Request Handler.
		RLbsNetChannel netRequestChannel;
		
		netRequestChannel.OpenL(RLbsNetChannel::EChannelNG2NRH, *this);
		CleanupClosePushL(netRequestChannel);
		
		TRequestStatus status;
		TLbsNetPosRequestQualityInt quality;
		TLbsNetPosRequestMethodInt method;
		TPositionInfo posInfo;
		TLbsNetLocationRequestMsg msg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 1), 
									  EFalse,
									  TLbsNetworkEnumInt::EServiceNetworkLocation,
									  quality,
									  method);
		netRequestChannel.SendMessage(msg, status);
		
		TRequestStatus status2;
		TLbsNetPosRequestQualityInt quality2;
		TLbsNetPosRequestMethodInt method2;
		TPositionInfo posInfo2;
		TLbsNetLocationRequestMsg msg2(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 2), 
									  EFalse,
									  TLbsNetworkEnumInt::EServiceMobileTerminated,
									  quality2,
									  method2);
		netRequestChannel.SendMessage(msg2, status2);
		
		User::WaitForRequest(status2);
		
		if (status2.Int() != KErrInUse)
			{
			User::Leave(status2.Int());
			}
		
		netRequestChannel.CancelSendMessageNotification();
		CleanupStack::PopAndDestroy(&netRequestChannel);
		
		// Test the interface from the Network Request Handler to Network Gateway.	
		RLbsNetChannel netGatewayChannel;
		
		netGatewayChannel.OpenL(RLbsNetChannel::EChannelNRH2NG, *this);
		CleanupClosePushL(netGatewayChannel);
		
		TRequestStatus status3;
		_LIT(KDestination, "08456484950");
		TLbsNetTransmitLocationRequestMsg msg3(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 3),
											   KDestination,
											   1);
		netGatewayChannel.SendMessage(msg3, status3);
		
		TRequestStatus status4;
		_LIT(KDestination2, "999LetsBeAvenue");
		TLbsNetTransmitLocationRequestMsg msg4(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 4),
											   KDestination2,
											   1);
		netGatewayChannel.SendMessage(msg4, status4);											   	
		
		User::WaitForRequest(status4);
		
		if (status4.Int() != KErrInUse)
			{
			User::Leave(status4.Int());
			}
		
		netGatewayChannel.CancelSendMessageNotification();		
		CleanupStack::PopAndDestroy(&netGatewayChannel);
		
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict Clbsnetinternaltest2Step::doTestStepPostambleL()
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
