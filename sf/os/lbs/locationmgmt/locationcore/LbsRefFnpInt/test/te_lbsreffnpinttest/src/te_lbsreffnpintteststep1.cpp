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

#include "te_lbsreffnpintteststep1.h"
#include "te_lbsreffnpinttestsuitedefs.h"

const TInt KOwnerInt = 0x12345;
const TUid KOwnerUid = {KOwnerInt};


CLbsReffnpintTestStep1::~CLbsReffnpintTestStep1()
/**
 * Destructor
 */
	{
	}

CLbsReffnpintTestStep1::CLbsReffnpintTestStep1()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsReffnpintTestStep1);
	}

TVerdict CLbsReffnpintTestStep1::doTestStepPreambleL()
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


TVerdict CLbsReffnpintTestStep1::doTestStepL()
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
		// Test network location request interface
		//
		
		TestNetworkLocationRequestInterface();
		

		///////////////////////////////////////////////////////////////////////
		//
		// Test network location response interface
		//
		
		TestNetworkLocationResponseInterface();


		///////////////////////////////////////////////////////////////////////
		//
		// Test reference network position update interface
		//

		TestReferencePositionUpdateInterface();

		///////////////////////////////////////////////////////////////////////
		//
		// Test final network position update interface
		//
		
		TestFinalPositionUpdateInterface();


		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict CLbsReffnpintTestStep1::doTestStepPostambleL()
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

void CLbsReffnpintTestStep1::TestNetworkLocationRequestInterface()
	{
	RLbsNetworkLocationRequests requests;
	TRequestStatus status;
	
	requests.OpenL(KTe_lbsreffnpinttestSuiteUid);
	CleanupClosePushL(requests);
	
	// Test notification cancel
	requests.NotifyNetworkLocationRequestUpdate(status);
	requests.CancelNotifyNetworkLocationRequestUpdate();
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrCancel);
	
	// Test setting a request
	TLbsNetSessionIdInt sessionId(KTe_lbsreffnpinttestSuiteUid, 0x10000001);
	TLbsNetPosRequestQualityInt quality;
	TLbsNetLocNetworkLocationRequestMsg requestMsg(sessionId, 
												   ETrue,
												   quality);
	User::LeaveIfError(requests.SetNetworkLocationRequest(requestMsg));
	
	// Test getting a request
	const TLbsNetSessionIdInt KInvalidSessionId;
	TLbsNetLocNetworkLocationRequestMsg msg(KInvalidSessionId,
											EFalse,
											TLbsNetPosRequestQualityInt());
	User::LeaveIfError(requests.GetNetworkLocationRequest(msg));
	TESTL(msg.Type() == TLbsNetLocMsgBase::ENetLocMsgNetworkLocationRequest);
	
	// Test notification of a new request
	requests.NotifyNetworkLocationRequestUpdate(status);
	User::LeaveIfError(requests.SetNetworkLocationRequest(requestMsg));
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	
	// Test Network Location Request messages are sent ok
	
	// Test Network Location Cancel messages are sent ok

	CleanupStack::PopAndDestroy(1, &requests);	
	}

void CLbsReffnpintTestStep1::TestNetworkLocationResponseInterface()
	{
	RLbsNetworkLocationResponses responses;	
	TRequestStatus status;
		
	responses.OpenL(KTe_lbsreffnpinttestSuiteUid);
	CleanupClosePushL(responses);

	// Test notification cancel
	responses.NotifyNetworkLocationResponseUpdate(status);
	responses.CancelNotifyNetworkLocationResponseUpdate();
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrCancel);
	
	// Test setting a request
	TLbsNetSessionIdInt sessionId(KTe_lbsreffnpinttestSuiteUid, 0x10000002);
	TLbsNetLocNetworkLocationCompleteMsg completeMsg(sessionId, 
												     KErrNone);
	User::LeaveIfError(responses.SetNetworkLocationResponse(completeMsg));
	
	// Test getting a request
	const TLbsNetSessionIdInt KInvalidSessionId;
	TLbsNetLocNetworkLocationCompleteMsg msg(KInvalidSessionId, KErrAbort);
	User::LeaveIfError(responses.GetNetworkLocationResponse(msg));
	TESTL(msg.Type() == TLbsNetLocMsgBase::ENetLocMsgNetworkLocationComplete);
	
	// Test notification of a new request
	responses.NotifyNetworkLocationResponseUpdate(status);
	User::LeaveIfError(responses.SetNetworkLocationResponse(completeMsg));
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	
	// Test sending a complete message
			
	CleanupStack::PopAndDestroy(1, &responses);
	}

void CLbsReffnpintTestStep1::TestReferencePositionUpdateInterface()
	{
	RLbsNetworkPositionUpdates positions;
	const TLbsNetSessionIdInt sessionId(KOwnerUid, 2);
	TLbsNetSessionIdInt rxSessionId;
	positions.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionReference);
	CleanupClosePushL(positions);
	
	// Test Setting the position
	TPosition pos;
	pos.SetAccuracy(10.0f, 10.0f);
	pos.SetCoordinate(TReal64(1000.0), TReal64(250.4), TReal(50.6));
	TPositionInfo posInfo;
	posInfo.SetPosition(pos);
	TTime actualTime(_L("20060717:090227"));
	User::LeaveIfError(positions.SetPositionInfo(sessionId, KErrNone, posInfo, actualTime));
	
	// Test getting the position
	TPosition rxPos;
	TPositionInfo rxPosInfo;
	TTime rxTargetTime;
	TTime rxActualTime;
	User::LeaveIfError(positions.GetPositionInfo(rxSessionId, rxPosInfo, rxTargetTime, rxActualTime));
	TESTL(rxActualTime == actualTime);
	rxPosInfo.GetPosition(rxPos);
	TESTL(rxPos.Altitude() == pos.Altitude());
	TESTL(rxPos.HorizontalAccuracy() == pos.HorizontalAccuracy());
	TESTL(rxPos.VerticalAccuracy() == pos.VerticalAccuracy());
	TESTL(rxPos.Latitude() == pos.Latitude());
	TESTL(rxPos.Longitude() == pos.Longitude());
	TESTL(rxSessionId == sessionId);
	
	CleanupStack::PopAndDestroy(1, &positions);	
	}

void CLbsReffnpintTestStep1::TestFinalPositionUpdateInterface()
	{
	const TLbsNetSessionIdInt sessionId(KOwnerUid, 2);
	TLbsNetSessionIdInt rxSessionId;
	RLbsNetworkPositionUpdates positions;
	positions.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionFinal);
	CleanupClosePushL(positions);
	
	// Test Setting the position
	TPosition pos;
	pos.SetAccuracy(20.0f, 20.0f);
	pos.SetCoordinate(TReal64(1000.0), TReal64(250.4), TReal(50.6));
	TPositionInfo posInfo;
	posInfo.SetPosition(pos);
	TTime actualTime(_L("20060717:090227"));
	User::LeaveIfError(positions.SetPositionInfo(sessionId, KErrNone, posInfo, actualTime));
	
	// Test getting the position
	TPosition rxPos;
	TPositionInfo rxPosInfo;
	TTime rxTargetTime;
	TTime rxActualTime;
	User::LeaveIfError(positions.GetPositionInfo(rxSessionId, rxPosInfo, rxTargetTime, rxActualTime));
	TESTL(rxActualTime == actualTime);
	rxPosInfo.GetPosition(rxPos);
	TESTL(rxPos.Altitude() == pos.Altitude());
	TESTL(rxPos.HorizontalAccuracy() == pos.HorizontalAccuracy());
	TESTL(rxPos.VerticalAccuracy() == pos.VerticalAccuracy());
	TESTL(rxPos.Latitude() == pos.Latitude());
	TESTL(rxPos.Longitude() == pos.Longitude());
	TESTL(rxSessionId == sessionId);
	
	CleanupStack::PopAndDestroy(1, &positions);	
	}

